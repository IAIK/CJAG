#ifndef CS_CACHESPEED_H
#define CS_CACHESPEED_H

#define MAX_TIME 10000
#define REPEAT 1000000

int get_single_threshold(int *h, double perc);
void print_histogram(int *h, int *m, int perc);
void show_splash(int color);
void show_usage(char *binary, int color);


#endif //CS_CACHESPEED_H
