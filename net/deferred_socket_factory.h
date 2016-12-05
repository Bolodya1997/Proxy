#ifndef PROXY_DEFERRED_SOCKET_FACTORY_H
#define PROXY_DEFERRED_SOCKET_FACTORY_H

#include <list>
#include <iostream>
#include "deferred_socket.h"

namespace net {

    class deferred_socket_factory {

        struct accept_data {
            deferred_socket *d_socket;
            pollable *accepter;
        };

        struct connect_data {
            deferred_socket *d_socket;
            std::string hostname;
            uint16_t port;
        };

        std::list<accept_data> accept_sockets;
        std::list<connect_data> connect_sockets;

        bool empty = true;

        static deferred_socket_factory *instance;

        deferred_socket_factory() { }
        ~deferred_socket_factory() { }

    public:
        static deferred_socket_factory *get_instance() {
            if (!instance)
                instance = new deferred_socket_factory();

            return instance;
        }

        deferred_socket *get_accept_socket(pollable *accepter) {
            auto *d_socket = new deferred_socket();
            accept_sockets.push_back({ d_socket, accepter });

            std::cerr << "+++" << std::endl;

            empty = false;
            return d_socket;
        }

        deferred_socket *get_connect_socket(std::string hostname, uint16_t port) {
            auto *d_socket = new deferred_socket();
            connect_sockets.push_back({ d_socket, hostname, port });

            std::cerr << "+++" << std::endl;

            empty = false;
            return d_socket;
        }

        void update();
    };

}

#endif //PROXY_DEFERRED_SOCKET_FACTORY_H
