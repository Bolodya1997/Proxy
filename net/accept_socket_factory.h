#ifndef PROXY_ACCEPT_SOCKET_FACTORY_H
#define PROXY_ACCEPT_SOCKET_FACTORY_H

#include <map>
#include <list>
#include <iostream>
#include "../logging.h"
#include "socket.h"
#include "../poll/fd_watcher.h"
#include "../templates/synchronisable.h"

namespace net { 
    
    /*
     *      #######################################
     *      #                                     #
     *      #         MUST BE THREAD SAFE         #
     *      #                                     #
     *      #######################################
     */
    class accept_socket_factory : public fd_watcher, 
                                  public synchronisable {

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

            friend class accept_socket_factory;
        };

        struct accept_data {
            net::socket *socket;
            pollable *accepter;
            int fd_reserved;
            bool ready;
        };

        std::map<pollable *, accept_data> accept_sockets;

        static accept_socket_factory *instance;

        accept_socket_factory() { }
        ~accept_socket_factory() { }

    public:
        static accept_socket_factory *get_instance() {
            return instance;
        }

        socket *get_accept_socket(pollable *accepter);
        void free_reserved_fd(pollable *socket);

        void update() noexcept;

    private:
        int reserve_fd();
        socket *accept(pollable *accepter, bool *ready);
    };

}

#endif //PROXY_ACCEPT_SOCKET_FACTORY_H
