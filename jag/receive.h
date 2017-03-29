#ifndef BLACKHAT_RECEIVE_H
#define BLACKHAT_RECEIVE_H
#include "../cjag.h"
#include "common.h"
#include <stdint.h>

void jag_receive(void **ret_addrs, size_t* recv_sets, cjag_config_t* config, jag_callback_t cb);

#endif //BLACKHAT_RECEIVE_H
