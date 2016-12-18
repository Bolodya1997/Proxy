#ifndef PROXY_ASYNCH_DNS_RESOLVER_H
#define PROXY_ASYNCH_DNS_RESOLVER_H

#include <string>
#include <csignal>
#include <map>
#include <queue>
#include <sys/signalfd.h>
#include "socket.h"
#include "../poll/poller.h"
#include "signal_wrap.h"

/*
 * uses SIGRTMIN -> SIGRTMAX
 */
class asynch_dns_resolver {

    static const int SIGNAL_RANGE = 32;     //  TODO: replace with (SIGRTMAX - SIGRTMIN)

    struct query {
        std::string hostname;
        uint16_t port;
        int signum;
    };

    static asynch_dns_resolver *instance;

    std::map<pollable *, query> queries;
    std::queue<pollable *> postponed_queries;
    bool signals[SIGNAL_RANGE] = {0};

    asynch_dns_resolver() { }
    ~asynch_dns_resolver() {
        delete instance;
    }

public:
    static asynch_dns_resolver *get_instance() {
        if (instance == NULL)
            instance = new asynch_dns_resolver();
        return instance;
    }

    pollable *add_query(std::string hostname, uint16_t port, observer *owner);

    sockaddr_in handle_response(pollable *sig_w);
private:
    void set_query(query &_query);

    sockaddr_in get_sockaddr(signalfd_siginfo &info);
};

#endif //PROXY_ASYNCH_DNS_RESOLVER_H
