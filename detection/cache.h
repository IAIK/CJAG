#ifndef BLACKHAT_CACHE_H
#define BLACKHAT_CACHE_H

#include <stdint.h>

typedef struct {
    int size;
    int ways;
    int line_size;
    int sets;
    int partitions;
    int miss_threshold;
} cache_config_l3_t;

void show_cache_info();
cache_config_l3_t get_l3_info();

#endif //BLACKHAT_CACHE_H
