#ifndef PROXY_MUTEX_H
#define PROXY_MUTEX_H

#include <pthread.h>
#include "../templates/single_instance.h"
#include "thread_exception.h"

class mutex : public single_instance {

    pthread_mutex_t _mutex;
    int depth = 0;

public:
    mutex() {   //  TODO: (*)
        _mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    }

    ~mutex() {
        pthread_mutex_destroy(&_mutex);
    }

    void lock() {
        if (pthread_mutex_lock(&_mutex) < 0)
            throw (thread_exception());
        ++depth;
    }

    bool try_lock() {
        if (pthread_mutex_trylock(&_mutex) != 0)
            return false;
        ++depth;
        return true;
    }

    void unlock() {
        --depth;
        if (pthread_mutex_unlock(&_mutex) < 0)
            throw (thread_exception());
    }

    friend class conditional;
};

#endif //PROXY_MUTEX_H
