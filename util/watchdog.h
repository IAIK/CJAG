#ifndef TOOL_WATCHDOG_H
#define TOOL_WATCHDOG_H

#include <pthread.h>

typedef struct _watchdog_t {
    int done;
    int interval;
    int timeout;
    pthread_t thread;
    void (*callback)(void*);
    void* data;
} watchdog_t;

void watchdog_start(watchdog_t *w, int timeout, void (*callback)(void*), void* data);

void watchdog_reset(watchdog_t *w);

void watchdog_done(watchdog_t *w);


#endif //TOOL_WATCHDOG_H
