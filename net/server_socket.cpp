#include <sys/socket.h>
#include <errno.h>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include "server_socket.h"
#include "net_exception.h"
#include "socket.h"

using namespace net;
using namespace std;

server_socket::server_socket(uint16_t port) {
    filed = ::socket(AF_INET, SOCK_STREAM, 0);
    if (filed < 0)
        throw (net_exception("socket", errno));

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
        throw (net_exception("bind", errno));

    if (listen(filed, QUEUE_SIZE) < 0)
        throw (net_exception("listen", errno));

    int saved_flags = fcntl(filed, F_GETFL);
    fcntl(filed, F_SETFL, saved_flags | O_NONBLOCK);
}

pollable *server_socket::accept() {
    pollable::accept();

    int cli_filed = accept4(filed, NULL, 0, SOCK_NONBLOCK);   //  TODO:   add referred
    if (cli_filed < 0)
        throw (net_exception("accept", errno));

    return new socket(cli_filed);
}
