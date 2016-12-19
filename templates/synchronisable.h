#ifndef PROXY_SYNCRONISABLE_H
#define PROXY_SYNCRONISABLE_H

#include "../thread/mutex.h"

#define critical_section_open(synch) \
    { synchronisable::critical_section __critical_guard(synch)
#define critical_section_close }

class synchronisable : public single_instance {
protected:
    mutex guard;

public:
    class critical_section {

        mutex *lock;

    public:

        critical_section(synchronisable *obj) : lock(&obj->guard) {
            lock->lock();
        }

        ~critical_section() {
            lock->unlock();
        }

        friend class synchronisable;
    };
};

#endif //PROXY_SYNCRONISABLE_H
