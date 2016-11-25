#ifndef PROXY_SOCKET_H
#define PROXY_SOCKET_H

#include <string>
#include "server_socket.h"

namespace net {

    class socket : public pollable {

    private:
        socket(int filed) {
            this->filed = filed;
        }

    public:
        socket(std::string hostname, uint16_t port);

        ssize_t write(const void *buff, size_t n) override;
        ssize_t read(void *buff, size_t n) override;

        friend pollable *server_socket::accept();

    private:
        void guard() override { };
    };
}

#endif //PROXY_SOCKET_H
