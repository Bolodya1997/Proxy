#ifndef PROXY_CACHE_LOADER_H
#define PROXY_CACHE_LOADER_H

#include "../session/session.h"
#include "cache_entry.h"

class cache_loader : public session {

    pollable *server;
    cache_entry *entry;

    bool server_closed = false;

public:
    cache_loader(pollable *server, cache_entry *entry)
            : server(server), entry(entry) {
        pollables.insert(server);

        this->server->set_actions(POLL_RE);
        server->set_owner(this);
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
