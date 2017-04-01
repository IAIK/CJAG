#include "common.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../cache/evict.h"
#include "../cache/set.h"
#include "../cache/slice.h"
#include "../util/timing.h"
#include "../util/error.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))

int jag_init(cjag_config_t* config) {
    config->n_addr_per_set = 16 / config->cache_slices;
    config->n_pages = (int) (ceil((float) config->cache_ways / config->n_addr_per_set) + 1);
    config->addr = NULL;

    config->cache_sets = jag_get_cache_sets(config);
    if(!config->cache_sets) {
        return 0;
    }

    return 1;
}

int jag_free(cjag_config_t* config) {
    free(config->cache_sets);
    free(config->addr);
    return !munmap(config->addresses, config->n_pages * 2 * 1024 * 1024);
}


volatile void **
jag_get_cache_sets(cjag_config_t *config) {
    const int n_addr_per_set = 16 / config->cache_slices;
    const int n_pages = (int) (ceil((float) config->cache_ways / n_addr_per_set) + 1);
    const int set_offset = 0;
    const int sub_set_offset = 64; //this should not change the process at all, but prevent interference from traffic on 4k aligned sets
    const int probe_count = MIN(2, n_addr_per_set); // 2 seems to be enough, but won't just work for 16 cores
    const int n_samples = 16 * 1024;
    uint8_t *cache_set[n_pages][32][config->cache_slices][n_addr_per_set];
    uint8_t cache_set_counter[32][config->cache_slices];
    int slice_offset[n_pages];

    uint8_t *addr = mmap(NULL, n_pages * 2 * 1024 * 1024, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, 0, 0);
    if (addr == MAP_FAILED) {
        return NULL;
    }

    config->addresses = (void*)addr;

    memset(cache_set, 0, sizeof(cache_set));
    memset(slice_offset, 0, sizeof(slice_offset));

    for (int i = 0; i < n_pages; ++i) {
        memset(cache_set_counter, 0, sizeof(cache_set_counter));
        for (int j = 0; j < 512; ++j) {
            uint8_t *cur_addr = addr + i * 2 * 1024 * 1024 + 4096 * j;
            int slice = get_cache_slice((uint64_t) cur_addr & ((1 << 21) - 1), config->cache_slices);
            int set = get_cache_set_index((uint64_t)(cur_addr)) >> 6;
            cache_set[i][set][slice][cache_set_counter[set][slice]++] = cur_addr + sub_set_offset;
        }
    }

    volatile uint8_t *test_eviction_set[config->cache_slices * n_pages * n_addr_per_set];
    volatile uint8_t *eviction_set[config->cache_slices * n_pages * n_addr_per_set];
    volatile uint8_t *probe_set[probe_count];

    int tes_size = 0;
    int es_size = 0;

    for (int p = 1; p < n_pages; ++p) {
        for (int s = 0; s < config->cache_slices; ++s) {
            for (int i = 0; i < n_addr_per_set; ++i) {
                test_eviction_set[tes_size] = cache_set[p][set_offset][s][i];
                tes_size++;
            }
        }
    }

    //turns out, 2 probes are enough for pretty much any stress level
    for (int i = 0; i < probe_count; ++i)
        probe_set[i] = cache_set[0][set_offset][0][i];

    for (int p = n_pages - 1; p > 0; --p) {
        tes_size = p * config->cache_slices * n_addr_per_set;
        slice_offset[p] = config->cache_slices - 1;
        for (int i = 0; i < config->cache_slices; ++i) {
            int fast = 0, slow = 0;
            tes_size -= n_addr_per_set;
            for (int c = 0; c < n_samples; ++c) {
                uint32_t time = test_evict_set(probe_set, eviction_set, test_eviction_set, probe_count, es_size,
                                               tes_size);
                if (time > config->cache_miss_threshold)
                    slow++;
                else
                    fast++;
            }

            //is faster, but might have a few errors
            if (slow < fast)
                break;

            slice_offset[p]--;

            if (slice_offset[p] < 0) {
                return NULL;
            }
        }
        //a bit slower, but almost no errors
        for (int s = 0; s < config->cache_slices; ++s) {
            for (int i = 0; i < n_addr_per_set; ++i) {
                eviction_set[es_size] = cache_set[p][set_offset][s][i];
                es_size++;
            }
        }
    }

    uint8_t *final_cache_sets[32 * config->cache_slices][config->cache_ways]; //[set][index]

    slice_offset[0] = 0;


    for (int set = 0; set < 32; ++set) {
        for (int slice = 0; slice < config->cache_slices; ++slice) {
            for (int page = 0; page < n_pages - 1; ++page) {
                for (int i = 0; i < n_addr_per_set; ++i) {
                    if ((page * n_addr_per_set + i) == config->cache_ways)
                        break;
                    final_cache_sets[set * config->cache_slices + (slice ^ slice_offset[page])]
                    [page * n_addr_per_set + i] = cache_set[page][set][slice][i] - sub_set_offset;
                }
            }
        }
    }

    volatile void **cache_set_out = malloc(sizeof(final_cache_sets));
    memcpy(cache_set_out, final_cache_sets, sizeof(final_cache_sets));

    return cache_set_out;
}


uint32_t
jag_check_set(volatile uint8_t **s_addrs, uint32_t target_misses, uint32_t read_timeout, cjag_config_t *config) {
    uint8_t hit[500], miss[500];
    uint32_t hits = 0, misses = 0, reads = 0;
    size_t time, delta;
    int window_index = 0, max_misses = 0;
    int i;

    memset(hit, 0, sizeof(hit));
    memset(miss, 0, sizeof(miss));

    for(i = 0; i <= config->cache_kill_count; i++) {
        ABORT_ON((ssize_t)s_addrs[i] <= 0, ERROR_INVALID_PARAMETERS, config->color_output);
    }
    evict_set(s_addrs, config->cache_kill_count);

    while (reads < read_timeout) //if the sender isn't active this could run forever otherwise
    {
        time = rdtsc();
        access_set(s_addrs, config->cache_probe_count);
        delta = rdtsc() - time;

        hits -= hit[window_index];
        misses -= miss[window_index];

        if (delta < config->cache_miss_threshold) {
            hits++;
            hit[window_index] = 1;
            miss[window_index] = 0;
        } else {
            misses++;
            hit[window_index] = 0;
            miss[window_index] = 1;
        }

        if (misses > max_misses)
            max_misses = misses;

        if (misses >= target_misses)
            break;

        reads++;
        window_index = (window_index + 1) % 500;
    }

    return max_misses;
}
