#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include "socket.h"

using namespace net;
using namespace std;

socket::socket(string hostname, unsigned short int port) {
    filed = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (filed < 0) {
        if (errno == ENFILE || errno == EMFILE)
            throw (fd_exception());
        throw (net_exception("socket"));
    }

    hostent *host = gethostbyname(hostname.data());
    if (host == NULL) {
        throw (net_exception("gethostbyname"));
    }

    in_addr inet_addr = **((in_addr **) host->h_addr_list);
    sock_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = inet_addr
    };

    if (::connect(filed, (sockaddr *) &sock_addr, sizeof(sockaddr_in)) < 0 && errno != EINPROGRESS) {
        throw (net_exception("connect"));
    }
}

void socket::connect() {
    pollable::connect();
}

ssize_t socket::write(const void *buff, size_t n) {
    pollable::write(buff, n);

    return send(filed, buff, n, MSG_DONTWAIT | MSG_NOSIGNAL);
}

ssize_t socket::read(void *buff, size_t n) {
    pollable::read(buff, n);

    return recv(filed, buff, n, MSG_DONTWAIT | MSG_NOSIGNAL);
}