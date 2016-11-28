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

        session *_session = NULL;

    protected:
        socket(int filed) {
            this->filed = filed;
        }

    public:
        socket(std::string hostname, unsigned short int port);

        session *get_session() {
            return _session;
        }

        void set_session(session *_session) {
            this->_session = _session;
        }

        void connect() override;

        ssize_t write(const void *buff, size_t n) override;
        ssize_t read(void *buff, size_t n) override;

        friend socket *server_socket::accept();

    private:
        void guard() override { };
    };
}

#endif //PROXY_SOCKET_H
