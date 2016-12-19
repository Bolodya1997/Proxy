#include "proxy.h"
#include "../net/accept_socket_factory.h"
#include "../thread/thread.h"
#include "../session/proxy_session.h"

using namespace std;

proxy::proxy(uint16_t port, unsigned thread_count)
        : single_thread_proxy(new cache()) {

    pollable::set_watcher(net::accept_socket_factory::get_instance());

    proxy_server = new net::server_socket(port);
    proxy_poller.add_untimed(proxy_server->set_actions(POLL_AC));

    for (int i = 0; i < thread_count - 1; i++)
        proxies.push_back(new single_thread_proxy(proxy_cache));
}

void proxy::start() {
    for (int i = 0; i < proxies.size(); i++) {
        runnable starter = [this, i]() -> void {
            proxies[i]->start();
        };
        thread(starter).start();
    }

    single_thread_proxy::start();
}

void proxy::handle_ready() {
    single_thread_proxy::handle_ready();

    if (!proxy_server->is_acceptable())
        return;

    net::socket *client;
    auto as_factory = net::accept_socket_factory::get_instance();
    critical_section_open(as_factory);

    client = as_factory->get_accept_socket(proxy_server);

    critical_section_close;

    balance(client);
}

void proxy::balance(pollable *connection) {
    single_thread_proxy *chosen = this;

    for (int i = 0; i < proxies.size(); i++) {
        if (proxies[i]->sessions.size() < chosen->sessions.size())
            chosen = proxies[i];
    }

    if (chosen == this) {
        sessions.insert(new proxy_session(proxy_poller, *proxy_cache, connection));
    } else {
        chosen->cond.get_mutex().lock();

        chosen->added_connections.insert(connection);
        chosen->cond.notify();

        chosen->cond.get_mutex().unlock();
    }
}
