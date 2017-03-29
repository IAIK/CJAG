#ifndef BLACKHAT_EVICT_H
#define BLACKHAT_EVICT_H

#include <stdint.h>
#include <stdio.h>

void evict_set(volatile uint8_t **addresses, int kill_count);
void access_set(volatile uint8_t **addresses, int probe_count);
size_t test_evict_set(volatile uint8_t **probe_set, volatile uint8_t **eviction_set, volatile uint8_t **test_eviction_set,
               int probe_count, int es_size, int tes_size);

#endif //BLACKHAT_EVICT_H
