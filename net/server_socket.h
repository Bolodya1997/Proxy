#ifndef PROXY_SERVER_SOCKET_H
#define PROXY_SERVER_SOCKET_H

#include <stdint.h>
#include "../poll/pollable.h"
#include "fd_exception.h"
#include "net_exception.h"

namespace net {

    class server_socket : public pollable {

        const static int QUEUE_SIZE = 10;

    public:
        server_socket(uint16_t port);

        net::socket *accept() override;

    private:
        void guard() override { };
    };

}

#endif //PROXY_SERVER_SOCKET_H
