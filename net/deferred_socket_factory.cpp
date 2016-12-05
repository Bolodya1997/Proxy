#include "deferred_socket_factory.h"

using namespace net;

void deferred_socket_factory::update()  {
    class socket *tmp;
    deferred_socket *d_socket;

    if (empty)
        return;

    try {
        if (!accept_sockets.empty()) {
            accept_data ac_d = accept_sockets.front();
            tmp = dynamic_cast<socket *>(ac_d.accepter->accept());

            d_socket = ac_d.d_socket;
            accept_sockets.pop_front();
        } else if (!connect_sockets.empty()) {
            connect_data co_d = connect_sockets.front();
            tmp = new socket(co_d.hostname, co_d.port);

            d_socket = co_d.d_socket;
            connect_sockets.pop_front();
        } else {
            empty = true;
            return;
        }
    } catch (fd_exception) {
        return;
    }

    std::cerr << "---" << std::endl;

    d_socket->init(tmp);
    delete tmp;
}

deferred_socket_factory *deferred_socket_factory::instance = NULL;
