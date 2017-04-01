#include "error.h"
#include "colorprint.h"
#include <stdio.h>

int show_error(error_code_t e, int color) {
    switch(e) {
        case ERROR_TOO_MANY_CPUS:
            printf_color(color, ERROR_TAG "Sorry, only CPUs with [y]up to 8 cores (4 for Skylake)[/y] are supported at the moment\n");
            break;

        case ERROR_NO_HUGEPAGES:
            printf_color(color, ERROR_TAG "Huge pages are not available!\nActivate them by running\n\n    [y]sudo sysctl -w vm.nr_hugepages=32[/y]\n\n");
            break;

        case ERROR_OOM:
            printf_color(color, ERROR_TAG "Out of memory\n");
            break;

        case ERROR_NO_CACHE_SETS:
            printf_color(color, ERROR_TAG "Could not retrieve cache sets, please [g]try to restart[/g]\n");
            break;

        case ERROR_UNMAP_FAILED:
            printf_color(color, ERROR_TAG "munmap failed\n");
            break;

        case ERROR_SLICES_NOT_SUPPORTED:
            printf_color(color, ERROR_TAG "Number of [y]slices[/y] must be between [y]1 and 8[/y], and a [y]power of two[/y]\n");
            break;

        case ERROR_INVALID_WAYS:
            printf_color(color, ERROR_TAG "Number of [y]ways[/y] is invalid\n");
            break;

        case ERROR_MISSING_OPTION:
            printf_color(color, ERROR_TAG "Parameter requires an [y]option[/y]\n");
            break;

        case ERROR_INVALID_SPEED:
            printf_color(color, ERROR_TAG "Invalid [y]delay[/y] factor!\n");
            break;

        case ERROR_TIMEOUT_TOO_LOW:
            printf_color(color, ERROR_TAG "[y]Timeout[/y] must be at least [y]2[/y] seconds!\n");
            break;

        case ERROR_INVALID_PARAMETERS:
            printf_color(color, ERROR_TAG "The combination of [y]parameters[/y] is [y]invalid[/y]\n");
            break;

        default:
            printf_color(color, ERROR_TAG "An [r]unknown[/r] error occurred\n");
            break;

    }
    return 1;
}
