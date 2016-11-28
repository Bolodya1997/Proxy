#ifndef PROXY_PROXY_SESSION_H
#define PROXY_PROXY_SESSION_H

#include "../poll/poller.h"
#include "../http/request_parser.h"
#include "../http/response_parser.h"
#include "session.h"
#include "../cache/cache.h"

/*
 * adapters[0] = client
 * adapters[1] = server
 */
class proxy_session : public session {

    enum {
        CLIENT_REQUEST,
        REQUEST_SERVER,

        SERVER_RESPONSE,
        CACHE_CLIENT,
        WAIT_CACHE,

        RESPONSE_CLIENT
    };
    int stage = CLIENT_REQUEST;

    poller &_poller;
    cache &_cache;

    session_rw_adapter *client;
    session_rw_adapter *server;

    http::request_parser request;
    size_t request_pos = 0;

    http::response_parser response;
    size_t response_pos = 0;

    cache_entry *entry = NULL;
    size_t entry_pos = 0;

    bool complete = false;

public:
    proxy_session(poller &_poller, cache &_cache, pollable *client)
            : _poller(_poller), _cache(_cache) {
        this->client = new session_rw_adapter(this, client);
        adapters.push_back(this->client);

        _poller.add_timed(this->client->set_actions(POLL_RE));
    }

    void update() override;
    bool is_complete() override {
        return complete;
    }

private:
    void set_complete() {
        complete = true;
        if (entry)
            entry->remove_observer(this);
    }

    void client_request_routine();
    void request_server_routine();

    void server_response_routine();
    void cache_client_routine();
    void wait_cache_routine();

    void response_client_routine();

    void read_from_cache();
    void write_to_cache() throw(no_place_exception);
};

#endif //PROXY_PROXY_SESSION_H
