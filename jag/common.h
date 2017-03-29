#ifndef BLACKHAT_COMMON_H
#define BLACKHAT_COMMON_H

#include <stdint.h>
#include "../cjag.h"

int jag_init(cjag_config_t* config);

int jag_free(cjag_config_t* config);

#define GET_EVICTION_SET(addr, set, cfg) (void**)(((char**)(addr)) + (cfg)->cache_ways * (set))

volatile void **jag_get_cache_sets(cjag_config_t* config);

uint32_t jag_check_set(volatile uint8_t **s_addrs, uint32_t target_misses, uint32_t read_timeout, cjag_config_t* config);


typedef void (*jag_callback_t)(cjag_config_t*, int);


#endif //BLACKHAT_COMMON_H
