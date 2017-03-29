#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <sched.h>
#include "util/cache.h"
#include "util/getopt_helper.h"
#include "util/colorprint.h"
#include "cachespeed.h"

static int hit[MAX_TIME];
static int miss[MAX_TIME];
static int dummy_mem[4096];

static int l1_thrash[256 * 1024 / sizeof(int)];

static getopt_arg_t options[] =
        {
                {"repeat",   required_argument, NULL, 'r', "Set the number of measurements to [y]REPEAT[/y] (default: 1000000)", "REPEAT"},
                {"no-color", no_argument,       NULL, 'n', "Disable color output.", NULL},
                {"verbose",  no_argument,       NULL, 'v', "Verbose mode",          NULL},
                {"help",     no_argument,       NULL, 'h', "Show this help.",       NULL},
                {NULL, 0,                       NULL, 0, NULL,                      NULL}
        };


int main(int argc, char* argv[]) {
    int color = 1;
    int verbose = 0;
    size_t repeat = REPEAT;

    struct option *long_options = getopt_get_long_options((getopt_arg_t *) options);
    int c;
    while ((c = getopt_long(argc, argv, ":r:nhv", long_options, NULL)) != EOF) {
        switch (c) {
            case 'r':
                repeat = strtoul(optarg, NULL, 0);
                break;
            case 'n':
                color = 0;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'h':
                show_usage(argv[0], color);
                return 0;
            case ':':
                printf_color(color, ERROR_TAG "Option [c]-%c[/c] requires an [y]argument[/y].\n",
                             optopt);
                printf("\n");
                show_usage(argv[0], color);
                return 1;
            case '?':
                if (isprint(optopt)) {
                    printf_color(color, ERROR_TAG "[y]Unknown[/y] option [c]-%c[/c].\n", optopt);
                } else {
                    printf_color(color, ERROR_TAG "[y]Unknown[/y] option character [c]\\x%x[/c].\n",
                                 optopt);
                }
                printf("\n");
                show_usage(argv[0], color);
                return 1;
            default:
                show_usage(argv[0], color);
                return 0;
        }
    }

    free(long_options);

    show_splash(color);

    volatile int *dummy = &(dummy_mem[2048]);

    printf_color(color, "[g][ * ][/g] Measuring [c]lfence[/c] overhead...\n");
    for (int i = 0; i < repeat; i++) {
        size_t start = rdtscl();
        size_t end = rdtscl();
        size_t diff = end - start;
        if (diff < 0) {
            diff = 0;
        }
        if (diff < MAX_TIME) {
            hit[diff]++;
        }
    }
    printf_color(color, "[g][ + ][/g] Done.\n");
    if(verbose) {
        print_histogram(hit, NULL, 1);
    }
    int lfence = get_single_threshold(hit, 95);
    memset(hit, 0, sizeof(hit));

    sched_yield();
    sched_yield();
    sched_yield();

    printf_color(color, "[g][ * ][/g] Measuring [c]mfence[/c] overhead...\n");
    for (int i = 0; i < repeat; i++) {
        size_t start = rdtsc();
        size_t end = rdtsc();
        size_t diff = end - start;
        if (diff < 0) {
            diff = 0;
        }
        if (diff < MAX_TIME) {
            hit[diff]++;
        }
    }
    printf_color(color, "[g][ + ][/g] Done.\n");
    if(verbose) {
        print_histogram(hit, NULL, 1);
    }
    int mfence = get_single_threshold(hit, 95);
    memset(hit, 0, sizeof(hit));

    sched_yield();
    sched_yield();
    sched_yield();

    printf_color(color, "[g][ * ][/g] Measuring [c]cpuid[/c] overhead...\n");
    for (int i = 0; i < repeat; i++) {
        size_t start = rdtscl();
        asm volatile("xor %%rax, %%rax\ncpuid":: : "rax", "rbx", "rcx", "rdx");
        size_t end = rdtscl();
        size_t diff = end - start;
        if (diff < 0) {
            diff = 0;
        }
        if (diff < MAX_TIME) {
            hit[diff]++;
        }
    }
    printf_color(color, "[g][ + ][/g] Done.\n");
    if(verbose) {
        print_histogram(hit, NULL, 1);
    }
    int cpuid = get_single_threshold(hit, 95) - lfence;
    memset(hit, 0, sizeof(hit));

    sched_yield();
    sched_yield();
    sched_yield();

    printf_color(color, "[g][ * ][/g] Measuring [c]L1[/c] hit time...\n");
    // cache hit
    for (int i = 0; i < repeat; i++) {
        size_t start = rdtsc();
        maccess(dummy);
        size_t end = rdtsc();
        size_t diff = end - start;
        if (diff < 0) {
            diff = 0;
        }
        if (diff < MAX_TIME) {
            hit[diff]++;
        }
    }
    printf_color(color, "[g][ + ][/g] Done.\n");
    if(verbose) {
        print_histogram(hit, NULL, 1);
    }
    int l1 = get_single_threshold(hit, 95) - mfence;
    memset(hit, 0, sizeof(hit));

    sched_yield();
    sched_yield();
    sched_yield();

    printf_color(color, "[g][ * ][/g] Measuring [c]L1[/c] miss time...\n");
    for (int i = 0; i < repeat; i++) {
        maccess(dummy);
        asm volatile("xor %%rax, %%rax\ncpuid":: : "rax", "rbx", "rcx", "rdx");
        memset(l1_thrash, i & 0xff, sizeof(l1_thrash));
        asm volatile("xor %%rax, %%rax\ncpuid":: : "rax", "rbx", "rcx", "rdx");
        size_t start = rdtsc();
        maccess(dummy);
        size_t end = rdtsc();
        size_t diff = end - start;
        if (diff < 0) {
            diff = 0;
        }
        if (diff < MAX_TIME) {
            hit[diff]++;
        }
    }
    printf_color(color, "[g][ + ][/g] Done.\n");
    if(verbose) {
        print_histogram(hit, NULL, 1);
    }
    int l2 = get_single_threshold(hit, 95) - mfence;
    memset(hit, 0, sizeof(hit));

    sched_yield();
    sched_yield();
    sched_yield();

    printf_color(color, "[g][ * ][/g] Measuring [c]L3[/c] miss time...\n");
    for (int i = 0; i < repeat; i++) {
        flush(dummy);
        asm volatile("xor %%rax, %%rax\ncpuid":: : "rax", "rbx", "rcx", "rdx");
        uint64_t start = rdtsc();
        maccess(dummy);
        uint64_t end = rdtsc();
        asm volatile("xor %%rax, %%rax\ncpuid":: : "rax", "rbx", "rcx", "rdx");
        size_t diff = end - start;
        if (diff < 0) {
            diff = 0;
        }
        if (diff < MAX_TIME) {
            miss[diff]++;
        }
    }
    printf_color(color, "[g][ + ][/g] Done.\n");
    int dram = get_single_threshold(miss, 10) - mfence;

    printf_color(color, "\n\n         | [m]cycles[/m]    | [m]+ lfence[/m]  | [m]+ mfence[/m]  | [m]+ cpuid[/m]\n");
    printf_color(color, "---------+-----------+-----------+-----------+------------\n");
    printf_color(color, "[c] L1 Hit  [/c]|%10d |%10d |%10d |%10d \n", l1, l1 + lfence, l1 + mfence, l1 + cpuid);
    printf_color(color, "[c] L1 Miss [/c]|%10d |%10d |%10d |%10d \n", l2, l2 + lfence, l2 + mfence, l2 + cpuid);
    printf_color(color, "[c] L3 Miss [/c]|%10d |%10d |%10d |%10d \n", dram, dram + lfence, dram + mfence, dram + cpuid);
    printf_color(color, "---------+-----------+-----------+-----------+------------\n");

    return 0;
}

// ---------------------------------------------------------------------------
void show_splash(int color) {
    printf_color(color, "[r]                                (                            [/r]\n");
    printf_color(color, "[r]   (                  )         )\\ )                   (    [/r]\n");
    printf_color(color, "[r]   )\\      )       ( /(    (   (()/(          (    (   )\\ ) [/r]\n");
    printf_color(color, "[r] (((_)  ( /(   (   )\\())  ))\\   /(_))`  )    ))\\  ))\\ (()/( [/r]\n");
    printf_color(color,
                 "[r] )\\[y]___[/y]  )(_))  )\\ ((_)\\  /((_) ([y]_[/y]))  /(/(   /((_)/((_) (([y]_[/y]))[/r]\n");
    printf_color(color,
                 "[r](([y]/ __|[/y](([y]_[/y])[y]_  [/y]((_)[y]| |[/y](_)([y]_[/y]))[y]   / __|[/y]((_)[y]_[/y]\\ ([y]_[/y])) ([y]_[/y]))[y]   _| | [/y][/r]\n");
    printf_color(color, "[y] | (__ / _` |/ _| | ' \\ / -_)  \\__ \\| '_ \\[r])[/r]/ -_)/ -_)/ _` | [/y]\n");
    printf_color(color, "[y]  \\___|\\__,_|\\__| |_||_|\\___|  |___/| .__/ \\___|\\___|\\__,_| [/y]\n");
    printf_color(color, "[y]                                    |_|                     [/y]\n");
}

// ---------------------------------------------------------------------------
void show_usage(char *binary, int color) {
    printf_color(color, "[y]USAGE[/y]\n  %s [g][options][/g] \n\n", binary);
    getopt_arg_t null_option = {0};
    size_t count = 0;
    printf_color(color, "\n[y]OPTIONS[/y]\n");
    do {
        if (!memcmp((void *) &options[count], (void *) &null_option, sizeof(getopt_arg_t))) {
            break;
        } else if (options[count].description) {
            printf_color(color, "  [g]-%c[/g]%s[y]%s[/y]%s [g]%s%s%s[/g][y]%s[/y]\n     ",
                         options[count].val,
                         options[count].has_arg != no_argument ? " " : "",
                         options[count].has_arg != no_argument ? options[count].arg_name : "",
                         options[count].name ? "," : "", options[count].name ? "--" : "", options[count].name,
                         options[count].has_arg != no_argument ? "=" : "",
                         options[count].has_arg != no_argument ? options[count].arg_name : "");
            printf_color(color, options[count].description);
            printf_color(color, "\n\n");
        }
        count++;
    } while (1);
    printf("\n");
    printf_color(color,
                 "[y]EXAMPLE[/y]\n  Start [c]%s[/c].\n\n\n", binary);
    printf_color(color,
                 "[y]AUTHORS[/y]\n  Written by Michael Schwarz.\n\n");

}


// ---------------------------------------------------------------------------
void print_histogram(int *h, int *m, int perc) {
    int min_val = 0, max_val = MAX_TIME - 1;
    size_t sum_h = 0, sum_m = 0;
    for (int i = 0; i < MAX_TIME; i++) {
        if (h) {
            sum_h += h[i];
        }
        if (m) {
            sum_m += m[i];
        }
    }
    for (int i = 0; i < MAX_TIME; i++) {
        if ((h && h[i]) || (m && m[i])) {
            min_val = i;
            break;
        }
    }
    for (int i = MAX_TIME - 1; i >= 0; i--) {
        if ((h && h[i]) || (m && m[i])) {
            max_val = i + 1;
            break;
        }
    }

    size_t p_sum_h = 0, p_sum_m = 0;
    for (int i = min_val; i < max_val; i++) {
        printf("%10d: ", i);
        if (h) {
            p_sum_h += h[i];
            if (perc) {
                printf("%10d  (%f%%)   ", h[i], 100.0 * p_sum_h / sum_h);
            } else {
                printf("%10d   ", h[i]);
            }
        }
        if (m) {
            p_sum_m += m[i];
            if (perc) {
                printf("%10d  (%f%%)   ", m[i], 100.0 * p_sum_m / sum_m);
            } else {
                printf("%10d   ", m[i]);
            }
        }
        printf("\n");
    }
}

// ---------------------------------------------------------------------------
int get_single_threshold(int *h, double perc) {
    size_t sum_h = 0, p_sum_h = 0;
    for (int i = 0; i < MAX_TIME; i++) {
        sum_h += h[i];
    }
    for (int i = 0; i < MAX_TIME; i++) {
        p_sum_h += h[i];
        if (100.0 * p_sum_h / sum_h >= perc) {
            return i;
        }
    }
    return -1;
}