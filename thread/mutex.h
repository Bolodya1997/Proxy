#ifndef PROXY_MUTEX_H
#define PROXY_MUTEX_H

#include <pthread.h>
#include "../templates/single_instance.h"
#include "thread_exception.h"

class mutex : public single_instance {

    pthread_mutex_t _mutex;

public:
    mutex() {
        _mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    }

    ~mutex() {
        pthread_mutex_destroy(&_mutex);
    }

    void lock() {
        if (pthread_mutex_lock(&_mutex) < 0)
            throw (thread_exception());
    }

    bool try_lock() {
        return pthread_mutex_trylock(&_mutex) == 0;
    }

    void unlock() {
        if (pthread_mutex_unlock(&_mutex) < 0)
            throw (thread_exception());
    }
};

#endif //PROXY_MUTEX_H
