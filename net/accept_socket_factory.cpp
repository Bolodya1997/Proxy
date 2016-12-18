#include "accept_socket_factory.h"

using namespace std;
using namespace net;

//  accept_socket_factory::deferred_socket

void accept_socket_factory::deferred_socket::close() {
    pollable::close();

    auto it = instance->accept_sockets.find(this);
    if (it == instance->accept_sockets.end())
        return;


    accept_data ac_data = it->second;
    instance->accept_sockets.erase(it);

    if (ac_data.fd_reserved >= 0) {
        ::close(ac_data.fd_reserved);
        instance->update();
    }

    logging::init_deferred(instance->accept_sockets.size());
}

pollable *accept_socket_factory::deferred_socket::set_actions(short actions) {
    if (filed == -1) {
        saved_actions = actions;
        return this;
    }

    return pollable::set_actions(actions);
}

short accept_socket_factory::deferred_socket::get_actions() {
    if (filed == -1)
        return saved_actions;

    return pollable::get_actions();
}

//  accept_socket_factory

net::socket *accept_socket_factory::get_accept_socket(pollable *accepter) {
    accept_data ac_data;
    ac_data.accepter = accepter;

    bool ready = true;

    ac_data.fd_reserved = reserve_fd();
    if (ac_data.fd_reserved < 0) {
        ac_data.socket = new deferred_socket();
        ready = false;
    } else {
        try {
            ac_data.socket = accept(accepter, &ready);
        } catch (net_exception) {
            close(ac_data.fd_reserved);
            throw (exception());
        }
    }

    if (!ready)
        accepter->set_actions(POLL_NO);

    ac_data.ready = ready;
    accept_sockets.insert({ ac_data.socket, ac_data });

    logging::get_deferred(accept_sockets.size());
    return ac_data.socket;
}

int accept_socket_factory::reserve_fd() {
    int fd = dup(0);
    if (fd < 0 && errno != EMFILE)
            throw (exception());

    return fd;
}

net::socket *accept_socket_factory::accept(pollable *accepter, bool *ready) {
    net::socket *ac_socket;
    try {
        ac_socket = dynamic_cast<socket *>(accepter->accept());
    } catch (fd_exception) {
        ac_socket = new deferred_socket();
        *ready = false;
    }

    return ac_socket;
}

void accept_socket_factory::free_reserved_fd(pollable *socket) {
    auto it = accept_sockets.find(socket);
    if (it == accept_sockets.end())
        return;

    close(it->second.fd_reserved);
    accept_sockets.erase(it);

    logging::init_deferred(accept_sockets.size());
}

void accept_socket_factory::update() noexcept {     //  FIXME: no exceptions allowed
    if (accept_sockets.empty())
        return;

    auto it = accept_sockets.begin();
    while (it->second.ready)
        ++it;
    if (it == accept_sockets.end())
        return;

    accept_data &ac_data = it->second;
    if (ac_data.fd_reserved < 0) {
        ac_data.fd_reserved = reserve_fd();
        return;
    }

    deferred_socket *d_socket = dynamic_cast<deferred_socket *>(ac_data.socket);
    try {
        net::socket *tmp = dynamic_cast<socket *>(ac_data.accepter->accept());
        d_socket->init(tmp);

        ac_data.ready = true;
        ac_data.accepter->set_actions(POLL_AC);

        delete tmp;
    } catch (fd_exception) {
        return;
    }
}

accept_socket_factory *accept_socket_factory::instance = NULL;
