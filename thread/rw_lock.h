#ifndef PROXY_RW_LOCK_H
#define PROXY_RW_LOCK_H

#include <pthread.h>
#include "../templates/single_instance.h"
#include "thread_exception.h"

class rw_lock : public single_instance {

    pthread_rwlock_t _rw_lock;

public:
    rw_lock() {
        pthread_rwlock_init(&_rw_lock, NULL);
    }

    ~rw_lock() {
        pthread_rwlock_destroy(&_rw_lock);
    }

    void read_lock() {
        if (pthread_rwlock_rdlock(&_rw_lock) < 0)
            throw (thread_exception());
    }

    void write_lock() {
        if (pthread_rwlock_wrlock(&_rw_lock) < 0)
            throw (thread_exception());
    }

    void unlock() {
        if (pthread_rwlock_unlock(&_rw_lock) < 0)
            throw (thread_exception());
    }
};

#endif //PROXY_RW_LOCK_H
