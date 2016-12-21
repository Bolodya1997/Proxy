#ifndef PROXY_SYNCRONISABLE_H
#define PROXY_SYNCRONISABLE_H

#include "../thread/mutex.h"

#define synchronised_section_open(synch) \
    { synchronisable::synchronised_section __critical_guard(synch)
#define synchronised_section_close }

class synchronisable : public single_instance {
protected:
    mutex guard;

public:
    class synchronised_section {

        mutex *lock;

    public:

        synchronised_section(synchronisable *obj) : lock(&obj->guard) {
            lock->lock();
        }

        ~synchronised_section() {
            lock->unlock();
        }
    };
};

#endif //PROXY_SYNCRONISABLE_H
