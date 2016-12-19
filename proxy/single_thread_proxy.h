#ifndef PROXY_SINGLE_THREAD_PROXY_H
#define PROXY_SINGLE_THREAD_PROXY_H

#include <set>
#include "../poll/poller.h"
#include "../session/session.h"
#include "../cache/cache.h"
#include "../thread/conditional.h"

class single_thread_proxy : public single_instance {

    static const int MAX_WAIT_TIME = 3000;  //  in millis

    poller proxy_poller = poller(MAX_WAIT_TIME);
    cache * const proxy_cache;

    std::set<session *> sessions;

    conditional cond;
    std::set<pollable *> added_connections;

public:
    single_thread_proxy(cache *proxy_cache);
    virtual ~single_thread_proxy() { }

    virtual void start();

private:
    virtual void synchronize();

    virtual void handle_ready();

    void clean_out_of_date();
    void clean_completed_sessions();

    friend class proxy;
};

#endif //PROXY_SINGLE_THREAD_PROXY_H
