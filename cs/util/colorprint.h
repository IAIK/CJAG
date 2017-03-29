#ifndef CJAG_COLORPRINT
#define CJAG_COLORPRINT

typedef enum {
    RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, RESET
} ansi_color_t;

#define ERROR_TAG "[r][ERROR][/r] "

void printf_color(int color, const char *fmt, ...);

#endif
