#ifndef PROXY_PROXY_H
#define PROXY_PROXY_H

#include <set>
#include "poll/poller.h"
#include "session/session.h"
#include "cache/cache.h"

class proxy : public single_instance {

    static const int MAX_WAIT_TIME = 500;  //  in millis

    pollable *proxy_server;
    poller proxy_poller;
    cache proxy_cache;

    std::set<session *> sessions;

public:
    proxy(uint16_t port);

    void start();

private:
    void handle_ready();
    void clean_out_of_date();
    void clean_completed_sessions();
};

#endif //PROXY_PROXY_H
