#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "colorprint.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_WHITE   "\x1b[0m"


typedef struct {
    ansi_color_t stack[16];
    int ptr;
} colorstack_t;

static void colorstack_push(colorstack_t *stack, ansi_color_t color) {
    if (stack->ptr >= 16) {
        return;
    }
    stack->stack[stack->ptr++] = color;
}

static ansi_color_t colorstack_pop(colorstack_t *stack) {
    if (stack->ptr <= 0) {
        return RESET;
    }
    return stack->stack[--stack->ptr];
}

static int append_color(char *str, int pos, ansi_color_t color) {
    const char *colors[] = {
            [RED] = ANSI_COLOR_RED,
            [GREEN] = ANSI_COLOR_GREEN,
            [BLUE] = ANSI_COLOR_BLUE,
            [CYAN] = ANSI_COLOR_CYAN,
            [MAGENTA] = ANSI_COLOR_MAGENTA,
            [YELLOW] = ANSI_COLOR_YELLOW,
            [WHITE] = ANSI_COLOR_WHITE,
            [RESET] = ANSI_COLOR_RESET
    };
    const char *color_str = colors[color];
    int len = strlen(color_str);
    int i;
    for (i = 0; i < len; i++) {
        str[pos + i] = color_str[i];
    }
    return pos + len;
}

void printf_color(int enable, const char *fmt, ...) {
    colorstack_t stack = {{0}};
    ansi_color_t colors[] = {
            ['r'] = RED,
            ['g'] = GREEN,
            ['b'] = BLUE,
            ['c'] = CYAN,
            ['m'] = MAGENTA,
            ['y'] = YELLOW,
            ['w'] = WHITE,
            ['/'] = RESET
    };

    int i, len = strlen(fmt), ptr = 0;
    char *fmt_replace = (char *) malloc(len * 3 + 1);
    ansi_color_t color = RESET;
    for (i = 0; i < len; i++) {
        if (fmt[i] == '[' &&
            ((i + 2 < len && fmt[i + 2] == ']') || (i + 3 < len && fmt[i + 1] == '/' && fmt[i + 3] == ']'))) {
            if (i != len - 1) {
                if (fmt[i + 1] == '/') {
                    color = colorstack_pop(&stack);
                    i++;
                } else {
                    colorstack_push(&stack, color);
                    color = colors[(int) fmt[i + 1]];
                    if(color == RESET) {
                        color = colorstack_pop(&stack);
                        fmt_replace[ptr++] = '[';
                        i--;
                    }
                }
                if (enable) {
                    ptr = append_color(fmt_replace, ptr, color);
                }
                i += 2;
                continue;
            }
        } else {
            fmt_replace[ptr++] = fmt[i];
        }
    }
    fmt_replace[ptr] = 0;

    fmt = fmt_replace;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    free((char*)fmt);
}
