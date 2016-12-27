#ifndef PROXY_SOCKET_H
#define PROXY_SOCKET_H

#include <string>
#include <netinet/in.h>
#include "server_socket.h"
#include "net_exception.h"

class session;

namespace net {

    class socket : public pollable {

        sockaddr_in sock_addr;

    protected:
        socket(int filed) {
            this->filed = filed;
        }

    public:
        socket(std::string hostname, unsigned short int port);
        socket(sockaddr_in sock_addr);

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
