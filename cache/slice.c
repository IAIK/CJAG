#include "slice.h"

// -----------------------------------------------------------------------------
int get_cache_slice(uint64_t phys_addr, int slices) {
    static const int h0[] = {6, 10, 12, 14, 16, 17, 18, 20, 22, 24, 25, 26, 27, 28, 30, 32, 33, 35, 36};
    static const int h1[] = {7, 11, 13, 15, 17, 19, 20, 21, 22, 23, 24, 26, 28, 29, 31, 33, 34, 35, 37};
    static const int h2[] = {8, 12, 13, 16, 19, 22, 23, 26, 27, 30, 31, 34, 35, 36, 37};
    int slice;

    int count = sizeof(h0) / sizeof(h0[0]);
    int hash0 = 0;
    for (int i = 0; i < count; i++) {
        hash0 ^= (phys_addr >> h0[i]) & 1;
    }
    slice = hash0;

    int hash1 = 0;
    count = sizeof(h1) / sizeof(h1[0]);
    if (slices > 2) {
        for (int i = 0; i < count; i++) {
            hash1 ^= (phys_addr >> h1[i]) & 1;
        }
        slice = hash1 << 1 | hash0;
    }

    int hash2 = 0;
    count = sizeof(h2) / sizeof(h2[0]);
    if (slices > 4) {
        for (int i = 0; i < count; i++) {
            hash2 ^= (phys_addr >> h2[i]) & 1;
        }
        slice = (hash2 << 2) | (hash1 << 1) | hash0;
    }
    return slice;
}
