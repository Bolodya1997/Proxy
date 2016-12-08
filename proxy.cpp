#include <iostream>
#include "proxy.h"
#include "session/proxy_session.h"
#include "net/deferred_socket_factory.h"

using namespace std;

proxy::proxy(uint16_t port)
        : proxy_server(new net::server_socket(port)),
          proxy_poller(MAX_WAIT_TIME),
          proxy_cache(sessions) {
    proxy_server->set_actions(POLL_AC);
    proxy_poller.add_untimed(proxy_server);
}

void proxy::start() {
    while (true) {
        proxy_poller.poll();

        handle_ready();
        clean_out_of_date();
        clean_completed_sessions();
    }
}

void proxy::handle_ready() {
    vector<pollable *> &ready = proxy_poller.get_ready();

    if (ready.empty())
        logging::empty();

    for (auto it = ready.begin(); it != ready.end(); it++) {
        pollable *cur = *it;

        if (cur->is_acceptable()) {
            net::socket *client;
            try {
                client = dynamic_cast<net::socket *>(cur->accept());
            } catch (fd_exception) {
                auto ds_factory = net::deferred_socket_factory::get_instance();
                client = ds_factory->get_accept_socket(cur);
                cur->set_actions(0);
            }
            sessions.insert(new proxy_session(proxy_poller, proxy_cache, client));

            continue;
        }

        if (cur->is_connectable() || cur->is_readable() || cur->is_writable()) {
            auto *cur_session = dynamic_cast<session *>(cur->get_owner());
            try {
                cur_session->update(cur);
            } catch (session *_session) {
                sessions.insert(_session);
            } catch (...) {
                sessions.erase(cur_session);
                delete cur_session;
            }
        }
    }
}

void proxy::clean_out_of_date() {
    vector<pollable *> &out_of_date = proxy_poller.get_out_of_date();
    for (auto it = out_of_date.begin(); it != out_of_date.end(); it++) {
        pollable *cur = *it;
        if (cur->is_closed())
            continue;

        auto *cur_session = dynamic_cast<session *>(cur->get_owner());
        sessions.erase(cur_session);

        auto p_ses = dynamic_cast<proxy_session *>(cur_session);    //  FIXME: debug
        if (p_ses != NULL)
            cerr << p_ses->request.get_data() << p_ses->stage << "\n\n";

        delete cur_session;
    }
}

void proxy::clean_completed_sessions() {
    for (auto it = sessions.begin(); it != sessions.end();) {
        session *cur = *it;
        if (!cur->is_complete()) {
            it++;
            continue;
        }

        it = sessions.erase(it);
        delete cur;
    }

}