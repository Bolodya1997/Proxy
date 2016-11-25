#ifndef PROXY_POLLER_H
#define PROXY_POLLER_H

#include <map>
#include <sys/poll.h>
#include <vector>
#include <memory>
#include "pollable.h"

class poller : public single_instance {

    std::map<int, pollable *> pollables;
    int timeout;

    std::vector<pollable *> ready;

public:
    poller(int timeout) : timeout(timeout) { }

    void add(pollable * _pollable);
    void poll();

    std::vector<pollable *> &get_ready();
};

#endif //PROXY_POLLER_H
