#ifndef BLACKHAT_SLICE_H
#define BLACKHAT_SLICE_H

#include <stdint.h>

int get_cache_slice(uint64_t phys_addr, int slices);

#endif //BLACKHAT_SLICE_H
