#include <iostream>
#include "proxy.h"
#include "net/socket.h"
#include "session/proxy_session.h"

using namespace std;

proxy::proxy(uint16_t port)
        : proxy_server(new net::server_socket(port)),
          proxy_poller(POLLER_TIMEOUT),
          proxy_cache(sessions) {
    proxy_server->set_actions(POLL_AC);
    proxy_poller.add(proxy_server);
}

void proxy::start() {
    while (true) {
        proxy_poller.poll();

        vector<pollable *> &ready = proxy_poller.get_ready();
        for (auto it = ready.begin(); it != ready.end(); it++) {
            pollable *cur = *it;

            if (cur->is_acceptable()) {
                pollable *client = cur->accept();
                sessions.insert(new proxy_session(proxy_poller, proxy_cache, client));
            } else {
                auto *cur_adapter = dynamic_cast<session_rw_adapter *>(cur);
                try {
                    cur_adapter->get_session()->update();
                } catch (session *_session) {
                    sessions.insert(_session);  //  TODO:   #1
                }
            }
        }

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
}