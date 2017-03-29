//
// Created by mschwarz on 2/19/17.
//

#include "set.h"
#include <stdint.h>

uint32_t get_cache_set_index(uint64_t phys_addr) {
    uint64_t mask = ((uint64_t) 1 << 17) - 1;
    return (phys_addr & mask) >> 6;
}