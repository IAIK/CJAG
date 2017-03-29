#include "evict.h"
#include "../util/timing.h"

//haswell +
void evict_set(volatile uint8_t **addresses, int kill_count) {
    for (size_t i = 0; i < kill_count; ++i) {
        *addresses[i];
        *addresses[i + 1];
        *addresses[i];
        *addresses[i + 1];
    }
}

void access_set(volatile uint8_t **addresses, int probe_count) {
    for (size_t i = 0; i < probe_count; ++i) {
        *addresses[i];
        *addresses[i + 1];
        *addresses[i + 2];
        *addresses[i];
        *addresses[i + 1];
        *addresses[i + 2];
    }
}

//sandy only
size_t
test_evict_set(volatile uint8_t **probe_set, volatile uint8_t **eviction_set, volatile uint8_t **test_eviction_set,
               int probe_count, int es_size, int tes_size) // not cached
{
    if (es_size != 0) {
        for (size_t i = 0; i < es_size - 1; ++i) {
            *eviction_set[i];
            *eviction_set[i + 1];
        }
    }
    if (tes_size != 0) {
        for (size_t i = 0; i < tes_size - 1; ++i) {
            *test_eviction_set[i];
            *test_eviction_set[i + 1];
        }
    }

    size_t time = rdtsc();
    for (size_t i = 0; i < probe_count; ++i) {
        *probe_set[i];
    }
    size_t delta = rdtsc() - time;
    return delta;
}