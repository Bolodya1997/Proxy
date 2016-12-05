#ifndef PROXY_DEFERRED_SOCKET_FACTORY_H
#define PROXY_DEFERRED_SOCKET_FACTORY_H

#include <map>
#include <iostream>
#include "../logging.h"
#include "socket.h"

namespace net {

    class deferred_socket_factory {

        class deferred_socket : public net::socket {
            short saved_actions = 0;

            deferred_socket() : socket(-1) { }

        public:
            void close() override;

            pollable *set_actions(short actions) override;
            short get_actions() override;

        private:
            void init(socket *tmp) {
                this->filed = get_filed(*tmp);
                get_filed(*tmp) = -1;

                set_actions(saved_actions);
            }

            friend class deferred_socket_factory;
        };

        struct accept_data {
            deferred_socket *d_socket;
            pollable *accepter;
        };

        struct connect_data {
            deferred_socket *d_socket;
            std::string hostname;
            uint16_t port;
        };

        std::map<deferred_socket *, accept_data> accept_sockets;
        std::map<deferred_socket *, connect_data> connect_sockets;

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

        deferred_socket *get_accept_socket(pollable *accepter);
        deferred_socket *get_connect_socket(std::string hostname, uint16_t port);
        void update();
    };

}

#endif //PROXY_DEFERRED_SOCKET_FACTORY_H
