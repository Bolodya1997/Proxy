#ifndef PROXY_THREAD_H
#define PROXY_THREAD_H

#include <pthread.h>
#include <functional>
#include "../templates/single_instance.h"
#include "thread_exception.h"

typedef std::function<void(void)> runnable;

class thread : public single_instance {

    pthread_t _thread;
    runnable routine;

public:
    thread(runnable routine) : routine(routine) { }

    void start() {
        if (pthread_create(&_thread, NULL, starter, &routine) < 0)
            throw (thread_exception());
    }

    void cancel() {
        if (pthread_cancel(_thread) < 0)
            throw (thread_exception());
    }

    void join() {
        if (pthread_join(_thread, NULL) < 0)
            throw (thread_exception());
    }

private:
    static void *starter(void *arg) {
        auto routine = *(runnable *) arg;
        routine();

        return NULL;
    }
};

#endif //PROXY_THREAD_H
