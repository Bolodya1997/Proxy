#ifndef PROXY_PROXY_H
#define PROXY_PROXY_H

#include "single_thread_proxy.h"

class proxy {

    std::vector<single_thread_proxy> proxies;

    cache proxy_cache;

public:
    proxy(uint16_t port, unsigned thread_count);

    void start();
};

#endif //PROXY_PROXY_H
