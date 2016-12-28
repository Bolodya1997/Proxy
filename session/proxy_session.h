#ifndef PROXY_PROXY_SESSION_H
#define PROXY_PROXY_SESSION_H

#include "../poll/poller.h"
#include "../http/request_parser.h"
#include "../http/response_parser.h"
#include "session.h"
#include "../cache/cache.h"
#include "../net/accept_socket_factory.h"
#include "../net/dns_wrap.h"

class proxy_session : public session {

    enum {
        CLIENT_REQUEST,
        LOOKUP,
        CONNECT,
        REQUEST_SERVER,

        SERVER_RESPONSE,
        CACHE_CLIENT,

        RESPONSE_CLIENT,
        ERROR_CLIENT
    };
    int stage = CLIENT_REQUEST;

    poller &_poller;
    cache &_cache;
    dns_wrap * const _dns;

    pollable *client;
    pollable *server = NULL;

    http::request_parser request;
    size_t request_pos = 0;

    http::response_parser response;
    size_t response_pos = 0;

    std::string error = "HTTP/1.0 ";

    cache_entry *entry = NULL;
    size_t entry_pos = 0;

    bool complete = false;

public:
    proxy_session(poller &_poller, cache &_cache, dns_wrap *_dns, pollable *client)
            : _poller(_poller), _cache(_cache), _dns(_dns), client(client) {
        pollables.insert(client);

        client->set_owner(this);
        _poller.add_timed(this->client->set_actions(POLL_RE));
    }
    ~proxy_session() {
        set_complete();
    }

    void update() override;
    void update(void *arg) override;

    bool is_complete() override {
        return complete;
    }

    void close() override;

private:
    void set_complete() {
        if (complete)
            return;
        complete = true;

        auto as_factory = net::accept_socket_factory::get_instance();
        synchronised_section_open(as_factory);

        as_factory->free_reserved_fd(client);
        as_factory->update();

        synchronised_section_close;

        if (entry)
            entry->remove_observer(this);
    }

    void client_request_routine();
    void connect_routine();
    void request_server_routine();

    void server_response_routine();
    void cache_client_routine();

    void response_client_routine();
    void error_client_routine();

    void dns_lookup();
    void init_server(in_addr addr);

    void read_from_cache();
    void write_to_cache();
};

#endif //PROXY_PROXY_SESSION_H
