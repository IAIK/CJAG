#ifndef CS_CACHEUTILS_C_H
#define CS_CACHEUTILS_C_H

#include <stdint.h>

uint64_t rdtsc();
uint64_t rdtscl();
void maccess(volatile void *p);
void flush(volatile void *p);

#endif //CS_CACHEUTILS_C_H
