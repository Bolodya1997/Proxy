#include <sys/socket.h>
#include <errno.h>
#include <string>
#include <netinet/in.h>
#include <fcntl.h>
#include "server_socket.h"
#include "net_exception.h"
#include "socket.h"

using namespace net;
using namespace std;

server_socket::server_socket(uint16_t port) {
    filed = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (filed < 0)
        throw (net_exception("socket"));

    int _1[1] = { 1 };
    setsockopt(filed, SOL_SOCKET, SO_REUSEADDR, _1, sizeof(int));

    in_addr inet_addr = {
            .s_addr = INADDR_ANY
    };
    sockaddr_in sock_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = inet_addr
    };
    if (::bind(filed, (sockaddr *) &sock_addr, sizeof(sockaddr_in)))
        throw (net_exception("bind"));

    if (listen(filed, QUEUE_SIZE) < 0)
        throw (net_exception("listen"));
}

pollable *server_socket::accept() throw(fd_exception) {
    pollable::accept();

    int cli_filed = accept4(filed, NULL, 0, SOCK_NONBLOCK);
    if (cli_filed < 0) {
        if (errno == ENFILE)
            throw (fd_exception());
        throw (net_exception("accept"));
    }

    return new socket(cli_filed);
}
