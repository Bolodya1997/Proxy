#include <iostream>
#include "single_thread_proxy.h"
#include "../session/proxy_session.h"

using namespace std;

single_thread_proxy::single_thread_proxy(uint16_t port)
        : proxy_server(new net::server_socket(port)),
          proxy_poller(MAX_WAIT_TIME),
          proxy_cache(sessions) {

    pollable::set_watcher(net::accept_socket_factory::get_instance());

    proxy_server->set_actions(POLL_AC);
    proxy_poller.add_untimed(proxy_server);
}

void single_thread_proxy::start() {
    while (true) {
        proxy_poller.poll();

        handle_ready();
        clean_out_of_date();
        clean_completed_sessions();
    }
}

void single_thread_proxy::handle_ready() {
    vector<pollable *> &ready = proxy_poller.get_ready();

    if (ready.empty())
        logging::empty();

    for (auto it = ready.begin(); it != ready.end(); it++) {
        pollable *cur = *it;

        if (cur->is_acceptable()) {
            auto as_factory = net::accept_socket_factory::get_instance();
            net::socket *client = as_factory->get_accept_socket(cur);
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
                cur_session->close();
            }
        }
    }
}

void single_thread_proxy::clean_out_of_date() {
    vector<pollable *> &out_of_date = proxy_poller.get_out_of_date();
    for (auto it = out_of_date.begin(); it != out_of_date.end(); it++) {
        pollable *cur = *it;

        auto *cur_session = dynamic_cast<session *>(cur->get_owner());
        cur_session->close();
    }
}

void single_thread_proxy::clean_completed_sessions() {
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