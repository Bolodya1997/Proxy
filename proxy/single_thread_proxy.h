#ifndef PROXY_SINGLE_THREAD_PROXY_H
#define PROXY_SINGLE_THREAD_PROXY_H

#include <set>
#include "../poll/poller.h"
#include "../session/session.h"
#include "../cache/cache.h"

class single_thread_proxy : public single_instance {

    static const int MAX_WAIT_TIME = 3000;  //  in millis

    poller proxy_poller = poller(MAX_WAIT_TIME);
    cache &proxy_cache;

    std::set<session *> sessions;

public:
    single_thread_proxy(cache &proxy_cache);
    single_thread_proxy(cache &proxy_cache, uint16_t port);

    void start();

private:
    void handle_ready();
    void clean_out_of_date();
    void clean_completed_sessions();
};

#endif //PROXY_SINGLE_THREAD_PROXY_H
