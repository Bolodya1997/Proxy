#include "deferred_socket_factory.h"

using namespace net;

//  deferred_socket_factory::deferred_socket

void deferred_socket_factory::deferred_socket::close() {
    pollable::close();

    unsigned long n = 0;
    n += instance->accept_sockets.erase(this);
    n += instance->connect_sockets.erase(this);

    if (n > 0)
        logging::init_deferred(instance->accept_sockets.size() +
                               instance->connect_sockets.size());
}

pollable *deferred_socket_factory::deferred_socket::set_actions(short actions) {
    if (filed == -1) {
        saved_actions = actions;
        return this;
    }

    return pollable::set_actions(actions);
}

short deferred_socket_factory::deferred_socket::get_actions() {
    if (filed == -1)
        return saved_actions;

    return pollable::get_actions();
}

//  deferred_socket_factory

deferred_socket_factory::deferred_socket
*deferred_socket_factory::get_accept_socket(pollable *accepter) {
    auto *d_socket = new deferred_socket();
    accept_data tmp = {
            .d_socket = d_socket,
            .accepter = accepter
    };
    accept_sockets.insert({ d_socket, tmp });

    logging::get_deferred(accept_sockets.size() + connect_sockets.size());

    empty = false;
    return d_socket;
}

deferred_socket_factory::deferred_socket
*deferred_socket_factory::get_connect_socket(sockaddr_in sock_addr) {
    auto *d_socket = new deferred_socket();
    connect_data tmp = {
            .d_socket = d_socket,
            .sock_addr = sock_addr
    };
    connect_sockets.insert({ d_socket, tmp });

    logging::get_deferred(accept_sockets.size() + connect_sockets.size());

    empty = false;
    return d_socket;
}

void deferred_socket_factory::update()  {
    class socket *tmp;
    deferred_socket *d_socket;

    if (empty)
        return;

    try {
        if (!connect_sockets.empty()) {
            connect_data co_d = connect_sockets.begin()->second;
            tmp = new socket(co_d.sock_addr);

            d_socket = co_d.d_socket;
            connect_sockets.erase(d_socket);
        } else if (!accept_sockets.empty()) {
            accept_data ac_d = accept_sockets.begin()->second;
            tmp = dynamic_cast<socket *>(ac_d.accepter->accept());
            ac_d.accepter->set_actions(POLL_AC);

            d_socket = ac_d.d_socket;
            accept_sockets.erase(d_socket);
        } else {
            empty = true;
            return;
        }
    } catch (fd_exception) {
        return;
    }

    logging::init_deferred(accept_sockets.size() + connect_sockets.size());

    d_socket->init(tmp);
    delete tmp;
}

deferred_socket_factory *deferred_socket_factory::instance = NULL;
