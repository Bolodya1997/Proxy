#ifndef PROXY_SESSION_H
#define PROXY_SESSION_H

#include <set>
#include "../templates/observer.h"
#include "../net/socket.h"
#include "session_exception.h"

class session : public single_instance,
                public observer {
protected:
    static const int BUFF_SIZE = 1024;

    std::set<pollable *> pollables;

    char buff[BUFF_SIZE];

public:
    virtual ~session() {
        for (auto it = pollables.begin(); it != pollables.end(); it = pollables.erase(it)) {
            pollable *cur = *it;
            cur->close();
        }
    }

    virtual void update() override = 0;

    virtual void update(void *arg) override {   //  TODO: need to upgrade forward_session, session(?) with update() -> update(*arg)
        if (pollables.find((pollable *) arg) == pollables.end())
            throw (session_exception());

        this->update();
    }

    virtual bool is_complete() = 0;
};

#endif //PROXY_SESSION_H