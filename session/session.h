#ifndef PROXY_SESSION_H
#define PROXY_SESSION_H

#include <vector>
#include "../templates/observer.h"
#include "../net/socket.h"

class session : public single_instance,
                public observer {
protected:
    static const int BUFF_SIZE = 1024 * 10;

    std::vector<pollable *> pollables;

    char buff[BUFF_SIZE];

public:
    virtual ~session() {
        for (auto it = pollables.begin(); it != pollables.end(); it = pollables.erase(it)) {
            pollable *cur = *it;
            cur->close();
        }
    }

    virtual void update(void *arg) override { } //  TODO: improve session implements algorithms

    virtual bool is_complete() = 0;
};

#endif //PROXY_SESSION_H