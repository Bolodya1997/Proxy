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

    int res = getaddrinfo(hostname.c_str(), to_string(port).data(), &filter, &list);
    if (res != 0 || list == NULL) {
        if (errno == ETIMEDOUT)
            throw (fd_exception());

        throw (net_exception("gethostbyname"));
    }

    sockaddr_in sock_addr = *(sockaddr_in *) list->ai_addr;
    freeaddrinfo(list);

    this->~socket();
    new(this) socket(sock_addr.sin_addr, port);
}

socket::socket(in_addr addr, uint16_t port) {
    filed = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (filed < 0) {
        if (errno == EMFILE)
            throw (fd_exception());
        throw (net_exception("socket"));
    }

    sockaddr_in sock_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = addr
    };
    if (::connect(filed, (sockaddr *) &sock_addr, sizeof(sockaddr_in)) < 0
        && errno != EINPROGRESS) {
        throw (net_exception("connect"));
    }
}

void socket::connect() {
    pollable::connect();

    int opt;
    socklen_t len = sizeof(int);
    getsockopt(filed, SOL_SOCKET, SO_ERROR, &opt, &len);

    if (opt != 0)
        throw (net_exception("connect"));
}

ssize_t socket::write(const void *buff, size_t n) {
    pollable::write(buff, n);

    return send(filed, buff, n, MSG_DONTWAIT | MSG_NOSIGNAL);
}

ssize_t socket::read(void *buff, size_t n) {
    pollable::read(buff, n);

    return recv(filed, buff, n, MSG_DONTWAIT | MSG_NOSIGNAL);
}
