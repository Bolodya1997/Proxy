#ifndef PROXY_SESSION_H
#define PROXY_SESSION_H

#include <vector>
#include "../templates/observer.h"
#include "../net/socket.h"

class session : public single_instance,
                public observer {
protected:
    static const int BUFF_SIZE = 1024 * 10;

    std::vector<net::socket *> adapters;

    char buff[BUFF_SIZE];

public:
    virtual ~session() {
        for (auto it = adapters.begin(); it != adapters.end(); it = adapters.erase(it)) {
            net::socket *cur = *it;
            cur->close();
        }
    }

    virtual bool is_complete() = 0;
};

#endif //PROXY_SESSION_H
