#include <unistd.h>
#include "watchdog.h"


void *watchdog(void *arg) {
    watchdog_t *params = (watchdog_t *) arg;
    while (!params->done && params->timeout) {
        params->timeout--;
        sleep(1);
    }
    if (!params->done) {
        params->callback(params->data);
    }
    return NULL;
}

void watchdog_start(watchdog_t *w, int timeout, void (*callback)(void*), void* data) {
    w->timeout = timeout;
    w->interval = timeout;
    w->callback = callback;
    w->data = data;
    w->done = 0;
    pthread_create(&w->thread, NULL, watchdog, (void *) w);
}

void watchdog_reset(watchdog_t *w) {
    if (w) {
        w->timeout = w->interval;
    }
}


void watchdog_done(watchdog_t *w) {
    if (w) {
        w->done = 1;
        w->timeout = w->interval;
    }
}
