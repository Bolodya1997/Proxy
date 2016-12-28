#ifndef PROXY_SOCKET_H
#define PROXY_SOCKET_H

#include <string>
#include <netinet/in.h>
#include "server_socket.h"
#include "net_exception.h"

class session;

namespace net {

    class socket : public pollable {

    protected:
        socket(int filed) {
            this->filed = filed;
        }

    public:
        socket(std::string hostname, uint16_t port);
        socket(in_addr addr, uint16_t port);

        void close() override {
            pollable::close();
            shutdown(filed, SHUT_RDWR);
        }

        void connect() override;

        ssize_t write(const void *buff, size_t n) override;
        ssize_t read(void *buff, size_t n) override;

        friend pollable *server_socket::accept();

    private:
        void __abstract_guard() override { };
    };
}

#endif //PROXY_SOCKET_H
