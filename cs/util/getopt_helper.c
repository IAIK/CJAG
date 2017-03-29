#include "getopt_helper.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct option* getopt_get_long_options(getopt_arg_t* opt) {
    size_t count = 0;
    getopt_arg_t null_option = {0};
    do {
        if(!memcmp((void*)&opt[count], (void*)&null_option, sizeof(getopt_arg_t))) {
            break;
        } else {
            count++;
        }
    } while(1);

    struct option* gopt = malloc(sizeof(struct option) * count);
    if(!gopt) {
        return NULL;
    }
    for(int i = 0; i < count; i++) {
        gopt[i].name = opt[i].name;
        gopt[i].has_arg = opt[i].has_arg;
        gopt[i].flag = opt[i].flag;
        gopt[i].val = opt[i].val;
    }
    return gopt;
}