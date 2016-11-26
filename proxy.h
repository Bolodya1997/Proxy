#ifndef PROXY_PROXY_H
#define PROXY_PROXY_H

#include <set>
#include "poll/poller.h"
#include "session/session.h"
#include "cache/cache.h"

class proxy : public single_instance {

    static const int POLLER_TIMEOUT = -1;   //  TODO:   add connection close on timeout

    pollable *proxy_server;
    poller proxy_poller;
    cache proxy_cache;

    std::set<session *> sessions;

public:
    proxy(uint16_t port);

    void start();
};

#endif //PROXY_PROXY_H
