#ifndef BLACKHAT_ERROR_H
#define BLACKHAT_ERROR_H

typedef enum {
    ERROR_OOM,
    ERROR_NO_HUGEPAGES,
    ERROR_TOO_MANY_CPUS,
    ERROR_NO_CACHE_SETS,
    ERROR_UNMAP_FAILED,
    ERROR_SLICES_NOT_SUPPORTED,
    ERROR_INVALID_WAYS,
    ERROR_MISSING_OPTION,
    ERROR_INVALID_SPEED,
    ERROR_TIMEOUT_TOO_LOW,
    ERROR_INVALID_PARAMETERS
} error_code_t;

#define ERROR_ON(condition, error_code, color) do { if(condition) return show_error(error_code, color); } while(0)
#define ABORT_ON(condition, error_code, color) do { if(condition) exit(show_error(error_code, color)); } while(0)

int show_error(error_code_t e, int color);


#endif //BLACKHAT_ERROR_H
