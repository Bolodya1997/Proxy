#ifndef PROXY_CONDITIONAL_H
#define PROXY_CONDITIONAL_H

#include <cerrno>
#include "mutex.h"

class conditional : public single_instance {

    pthread_cond_t _cond;
    mutex _mutex;

    bool flag = false;

public:
    conditional() {     //  TODO: change on checkable (*)
        _cond = PTHREAD_COND_INITIALIZER;
    }

    ~conditional() {
        pthread_cond_broadcast(&_cond);
        pthread_cond_destroy(&_cond);
    }

    mutex &get_mutex() {
        return _mutex;
    }

    void wait() {
        if (_mutex.depth != 1)
            throw (thread_exception());

        while (!flag) {
            --_mutex.depth;
            int code = pthread_cond_wait(&_cond, &_mutex._mutex);
            ++_mutex.depth;

            if (code != 0 && code != EINTR)
                throw (thread_exception());
        }
        flag = false;
    }

    void notify() {
        if (_mutex.depth < 1)
            throw (thread_exception());

        pthread_cond_signal(&_cond);
        flag = true;
    }
};

#endif //PROXY_CONDITIONAL_H
