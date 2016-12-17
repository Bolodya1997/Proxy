#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include "socket.h"

using namespace net;
using namespace std;

socket::socket(string hostname, unsigned short int port) {
    addrinfo filter, *list;
    bzero(&filter, sizeof(addrinfo));
    filter.ai_family = AF_INET;
    filter.ai_socktype = SOCK_STREAM;
    int res = getaddrinfo(hostname.data(), to_string(port).data(), &filter, &list); //  TODO: retry to make async

    if (res != 0 || list == NULL)
        throw (net_exception("gethostbyname"));

    sockaddr_in sock_addr = *(sockaddr_in *) list->ai_addr;
    freeaddrinfo(list);

    filed = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (filed < 0) {
        if (errno == EMFILE)
            throw (fd_exception());
        throw (net_exception("socket"));
    }

    if (::connect(filed, (sockaddr *) &sock_addr, sizeof(sockaddr_in)) < 0
        && errno != EINPROGRESS) {
        throw (net_exception("connect"));
    }
}

void socket::connect() {
    pollable::connect();    //  TODO: check if it is really connected
}

ssize_t socket::write(const void *buff, size_t n) {
    pollable::write(buff, n);

    return send(filed, buff, n, MSG_DONTWAIT | MSG_NOSIGNAL);
}

ssize_t socket::read(void *buff, size_t n) {
    pollable::read(buff, n);

    return recv(filed, buff, n, MSG_DONTWAIT | MSG_NOSIGNAL);
}