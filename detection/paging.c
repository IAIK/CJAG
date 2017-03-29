#include "paging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------
int has_huge_pages() {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) {
        return 0;
    }
    char *line = NULL;

    size_t len = 0;
    int hp = 0;
    while (getline(&line, &len, f) != -1) {
        if (strncmp(line, "HugePages_Total", 15) == 0) {
            int size = 0;
            sscanf(&line[17], "%d", &size);
            if (size > 0) {
                hp = 1;
                break;
            }
        }
    }
    free(line);
    fclose(f);
    return hp;
}
