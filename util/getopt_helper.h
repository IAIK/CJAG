#ifndef BLACKHAT_GETOPT_HELPER_H
#define BLACKHAT_GETOPT_HELPER_H

#include <getopt.h>

typedef struct {
    const char* name;
    int has_arg;
    int* flag;
    int val;
    const char* description;
    const char* arg_name;
} getopt_arg_t;

struct option* getopt_get_long_options(getopt_arg_t* opt);

#endif //BLACKHAT_GETOPT_HELPER_H
