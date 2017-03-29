#include "cache.h"

// ---------------------------------------------------------------------------
uint64_t rdtsc() {
    uint64_t a, d;
    asm volatile ("mfence");
    asm volatile ("rdtsc" : "=a" (a), "=d" (d));
    asm volatile ("mfence");
    return (d << 32) | a;
}

// ---------------------------------------------------------------------------
uint64_t rdtscl() {
    uint64_t a, d;
    asm volatile ("lfence");
    asm volatile ("rdtsc" : "=a" (a), "=d" (d));
    asm volatile ("lfence");
    return (d << 32) | a;
}

// ---------------------------------------------------------------------------
void maccess(volatile void *p) {
    asm volatile ("movq (%0), %%rax\n"
    :
    : "c" (p)
    : "rax");
}

// ---------------------------------------------------------------------------
void flush(volatile void *p) {
    asm volatile ("clflush 0(%0)\n"
    :
    : "c" (p)
    : "rax");
}