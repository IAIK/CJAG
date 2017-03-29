#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cpuid.h>

int get_physical_cores() {
    FILE *f = fopen("/proc/cpuinfo", "r");
    if (!f) {
        return 0;
    }
    char *line = NULL;

    int cores[256] = {0};
    size_t len = 0;
    while (getline(&line, &len, f) != -1) {
        if (strncmp(line, "core id", 7) == 0) {
            int id = 0;
            sscanf(strrchr(line, ':') + 1, "%d", &id);
            if (id >= 0 && id < 256) {
                cores[id]++;
            }
        }
    }
    free(line);
    fclose(f);

    int phys_cores = 0;
    for (int i = 0; i < 256; i++) {
        if (cores[i]) {
            phys_cores++;
        }
    }
    return phys_cores;
}

int get_cpu_architecture() {
    unsigned int model;
    int name[4] = {0, 0, 0, 0};
    __cpuid(0, model, name[0], name[2], name[1]);

    if (strcmp((char *) name, "GenuineIntel") != 0) return -1;
    return model;
}

int get_slices() {
    int slices = get_physical_cores();
    if (get_cpu_architecture() >= 0x16) {
        slices <<= 1;
    }
    return slices;
}