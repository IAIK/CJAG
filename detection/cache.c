#include <stdio.h>
#include <stdint.h>
#include "cache.h"


cache_config_l3_t get_l3_info() {
    uint32_t eax, ebx, ecx, edx;
    int level = 0;
    cache_config_l3_t config;

    do {
        asm volatile("cpuid" : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) : "a" (4), "c" (level));
        int type = eax & 0x1f;
        if(!type) break;
        level++;
        config.line_size = (ebx & 0xfff) + 1;
        config.ways = ((ebx >> 22) & 0x3ff) + 1;
        config.sets = ecx + 1;
        config.partitions = ((ebx >> 12) & 0x3ff) + 1;
        config.size = config.line_size * config.ways * config.sets * config.partitions;
    } while(1);
    return config;
}


void show_cache_info() {
    cache_config_l3_t l3 = get_l3_info();
    printf("%d KB, %d-way L3\n", l3.size / 1024, l3.ways);
}