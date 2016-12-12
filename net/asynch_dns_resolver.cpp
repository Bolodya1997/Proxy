#include <netdb.h>
#include "asynch_dns_resolver.h"

using namespace std;

asynch_dns_resolver *asynch_dns_resolver::instance = NULL;

pollable *asynch_dns_resolver::add_query(string hostname, uint16_t port,
                                         observer *owner) {
    int signum = SIGRTMIN;
    for (; signum < SIGNAL_RANGE; signum++) {
        if (!signals[signum])
            break;
    }
    if (signum == SIGRTMAX)
        throw net_exception("signals overflow");
    signals[signum] = true;

    auto sig_w = new signal_wrap(signum);
    query _query = {
            .hostname = hostname,
            .port = port,
            .signum = signum
    };
    set_query(_query);
    queries.insert({ sig_w, _query });

    sig_w->set_owner(owner);
    return sig_w->set_actions(POLL_RE);
}

void asynch_dns_resolver::set_query(query &_query) {
    auto mask = new addrinfo;
    bzero(mask, sizeof(addrinfo));
    mask->ai_family = AF_INET;
    mask->ai_socktype = SOCK_DGRAM;
    auto host = new gaicb();
    bzero(host, sizeof(gaicb));

    host->ar_name = _query.hostname.c_str();
    host->ar_service = to_string(_query.port).c_str();
    host->ar_request = mask;

    sigevent sigev;
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = _query.signum;
    sigev.sigev_value.sival_ptr = host;

    getaddrinfo_a(GAI_NOWAIT, &host, 1, &sigev);
}

sockaddr_in asynch_dns_resolver::handle_response(pollable *sig_w) {
    signalfd_siginfo siginfo;
    if (sig_w->read(&siginfo, sizeof(signalfd_siginfo)) < sizeof(signalfd_siginfo))
        throw (net_exception("getaddrinfo_a"));

    sockaddr_in sock_addr = get_sockaddr(siginfo);

    query &_query = queries[sig_w];
    signals[_query.signum] = true;

    queries.erase(sig_w);
    sig_w->close();

    return sock_addr;
}

sockaddr_in asynch_dns_resolver::get_sockaddr(signalfd_siginfo &info) {
    gaicb *host = (gaicb *) info.ssi_ptr;
    addrinfo *_addrinfo = host->ar_result;

    delete host->ar_request;
    delete host;

    if (_addrinfo == NULL)
        throw (net_exception("getaddrinfo_a"));
    if (_addrinfo->ai_addr == NULL) {
        freeaddrinfo(_addrinfo);
        throw (net_exception("getaddrinfo_a"));
    }
    sockaddr_in sock_addr = *(sockaddr_in *) _addrinfo->ai_addr;
    freeaddrinfo(_addrinfo);

    return sock_addr;
}
