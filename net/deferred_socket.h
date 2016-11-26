#ifndef PROXY_DEFERRED_SOCKET_H
#define PROXY_DEFERRED_SOCKET_H

#include "socket.h"

namespace net {

    class deferred_socket_factory;

    class deferred_socket : public socket {

        short saved_actions = 0;

        deferred_socket() : socket(-1) { }

    public:
        pollable *set_actions(short actions) override {
            if (filed == -1) {
                saved_actions = actions;
                return this;
            }

            return pollable::set_actions(actions);
        }

        short get_actions() override {
            if (filed == -1)
                return saved_actions;

            return pollable::get_actions();
        }

    private:
        void init(socket *tmp) {
            this->filed = get_filed(*tmp);
            get_filed(*tmp) = -1;

            set_actions(saved_actions);
        }

        friend class deferred_socket_factory;
    };
}

#endif //PROXY_DEFERRED_SOCKET_H
