#ifndef PROXY_SINGLE_THREAD_PROXY_H
#define PROXY_SINGLE_THREAD_PROXY_H

#include <set>
#include "../poll/poller.h"
#include "../session/session.h"
#include "../cache/cache.h"
#include "../thread/conditional.h"
#include "../net/dns_wrap.h"

class single_thread_proxy : public single_instance,
                            public observer {

    static const int MAX_WAIT_TIME = 500;  //  in millis

    poller proxy_poller = poller(MAX_WAIT_TIME);
    cache * const proxy_cache;

    dns_wrap * const proxy_dns = new dns_wrap();

    std::set<session *> sessions;

    conditional cond;
    std::set<pollable *> accepted;

public:
    single_thread_proxy(cache *proxy_cache);
    single_thread_proxy(cache *proxy_cache, pollable *notifier);
    virtual ~single_thread_proxy() { }

    virtual void start();

    void update() override { }

    void update(void *arg) override {
        auto notifier = (pollable *) arg;
        notifier->read(NULL, 0);
    }

private:
    virtual void synchronize();

    virtual void handle_ready();

    void clean_out_of_date();
    void clean_completed_sessions();

    friend class proxy;
};

#endif //PROXY_SINGLE_THREAD_PROXY_H
