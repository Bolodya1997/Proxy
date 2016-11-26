#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <iostream>
#include "socket.h"
#include "net_exception.h"

using namespace net;
using namespace std;

socket::socket(string hostname, uint16_t port) throw(fd_exception) {
    filed = ::socket(AF_INET, SOCK_STREAM, 0);
    if (filed < 0) {
        if (errno == ENFILE)
            throw (fd_exception());
        throw (net_exception("socket"));
    }

    hostent *host = gethostbyname(hostname.data());
    if (host == NULL)
        throw (net_exception("gethostbyname"));

    in_addr inet_addr = **((in_addr **) host->h_addr_list);
    sockaddr_in sock_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = inet_addr
    };
    if (connect(filed, (sockaddr *) &sock_addr, sizeof(sockaddr_in)) < 0)
        throw (net_exception("connect"));

    int saved_flags = fcntl(filed, F_GETFL);
    fcntl(filed, F_SETFL, saved_flags | O_NONBLOCK);
}

ssize_t socket::write(const void *buff, size_t n) {
    pollable::write(buff, n);

    return send(filed, buff, n, MSG_DONTWAIT | MSG_NOSIGNAL);
}

ssize_t socket::read(void *buff, size_t n) {
    pollable::read(buff, n);

    return recv(filed, buff, n, MSG_DONTWAIT | MSG_NOSIGNAL);
}