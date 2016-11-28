#ifndef PROXY_CACHE_LOADER_H
#define PROXY_CACHE_LOADER_H

#include "../session/session.h"
#include "cache_entry.h"

/*
 * adapters[0] = server
 */
class cache_loader : public session {

    net::socket *server;
    cache_entry *entry;

    bool server_closed = false;

public:
    cache_loader(net::socket *server, cache_entry *entry)
            : server(server), entry(entry) {
        adapters.push_back(server);

        this->server->set_actions(POLL_RE);
        server->set_session(this);
    }

    void update() override {
        ssize_t n = server->read(buff, BUFF_SIZE);
        if (n < 1) {
            server_closed = true;
        }
        entry->add_data(buff, (size_t) n);
    }

    bool is_complete() override {
        return server_closed || entry->is_complete();
    };
};

#endif //PROXY_CACHE_LOADER_H
