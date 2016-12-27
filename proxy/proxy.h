#ifndef PROXY_PROXY_H
#define PROXY_PROXY_H

#include <csignal>
#include "single_thread_proxy.h"

class proxy : public single_thread_proxy {

    static const int SIG_MIN = __SIGRTMIN;

    std::vector<single_thread_proxy *> proxies;

    pollable *proxy_server;

public:
    proxy(uint16_t port, unsigned thread_count);

    void start() override;

private:
    virtual void synchronize() override { }

    virtual void handle_ready() override;
    void balance(pollable *connection);
};

#endif //PROXY_PROXY_H
