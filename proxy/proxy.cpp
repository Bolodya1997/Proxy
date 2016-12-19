#include "proxy.h"
#include "../net/accept_socket_factory.h"
#include "../thread/thread.h"

proxy::proxy(uint16_t port, unsigned thread_count) {

    pollable::set_watcher(net::accept_socket_factory::get_instance());

    proxies.push_back(single_thread_proxy(proxy_cache, port));

    for (int i = 1; i < thread_count; i++)
        proxies.push_back(single_thread_proxy(proxy_cache));
}

void proxy::start() {
    for (int i = 1; i < proxies.size(); i++) {
        runnable starter = [this, i]() -> void {
            proxies[i].start();
        };
        thread(starter).start();
    }

    proxies[0].start();
}
