#include <iostream>
#include "proxy_session.h"
#include "forward_session.h"

using namespace std;

void proxy_session::update() {
    switch (stage) {
        case CLIENT_REQUEST:
            client_request_routine();
            break;
        case CONNECT:
            connect_routine();
            break;
        case REQUEST_SERVER:
            request_server_routine();
            break;

        case SERVER_RESPONSE:
            server_response_routine();
            break;
        case CACHE_CLIENT:
            cache_client_routine();
            break;

        case RESPONSE_CLIENT:
            response_client_routine();
            break;
        case ERROR_CLIENT:
            error_client_routine();
    }
}

void proxy_session::close() {
    switch (stage) {
        case CACHE_CLIENT:
        case RESPONSE_CLIENT:
        case ERROR_CLIENT:
            logging::silent_end();
            set_complete();
            return;

        case CLIENT_REQUEST:
            error += "408 Request Time-out";
            break;

        case CONNECT:
        case REQUEST_SERVER:
        case SERVER_RESPONSE:
            error += "504 Gateway Time-out";
    }
    error += "\r\n\r\n";

    client->set_actions(POLL_WR);
    stage = ERROR_CLIENT;
}

void proxy_session::client_request_routine() {
    ssize_t n = client->read(buff, BUFF_SIZE);
    if (n < 1) {
        close();
        return;
    }

    request.add_data(buff, n);
    if (!request.is_ready())
        return;

    net::accept_socket_factory::get_instance()->free_reserved_fd(client);

    if (request.is_get()) {
        entry = _cache.get_entry(request.get_absolute_url());
        if (entry) {
            read_from_cache();
            return;
        }
    }

    client->set_actions(0);
    stage = CONNECT;

    init_server();
}

void proxy_session::read_from_cache() {
    entry->add_observer(this);

    logging::load(request.get_absolute_url());

    if (server) {
        server->close();
        pollables.erase(server);
    } else {
        net::accept_socket_factory::get_instance()->update();
    }

    client->set_actions(POLL_WR);
    stage = CACHE_CLIENT;
}

void proxy_session::init_server() {
    string host = request.get_host().first;
    uint16_t port = request.get_host().second;
    try {
        server = new net::socket(host, port);
    } catch (exception) {
        close();
        return;
    }
    server->set_owner(this);
    pollables.insert(server);
    _poller.add_timed(server->set_actions(POLL_CO));
}

void proxy_session::connect_routine() {
    try {
        server->connect();
    } catch (net_exception) {
        close();
        return;
    }

    if (request.is_connect()) {
        string ok = "HTTP/1.0 200 OK\r\n\r\n";
        response.add_data(ok.data(), ok.length());

        client->set_actions(POLL_WR);
        server->set_actions(0);
        stage = RESPONSE_CLIENT;
        return;
    }

    server->set_actions(POLL_WR);
    stage = REQUEST_SERVER;
}

void proxy_session::request_server_routine() {
    string str = request.get_data();
    ssize_t n = server->write(str.data() + request_pos, str.length() - request_pos);
    if (n == -1) {
        close();
        return;
    }

    request_pos += n;
    if (request_pos < str.length())
        return;

    if (!request.is_get()) {
        auto *fwd = new forward_session(client, server);
        pollables.clear();
        set_complete();

        throw (fwd);
    }

    server->set_actions(POLL_RE);
    stage = SERVER_RESPONSE;
}

void proxy_session::server_response_routine() {
    ssize_t n = server->read(buff, BUFF_SIZE);
    if (n < 1) {
        close();
        return;
    }

    response.add_data(buff, n);
    if (!response.is_ready())
        return;

    if (response.is_workable()) {
        try {
            entry = _cache.get_entry(request.get_absolute_url());
            if (entry)
                read_from_cache();
            else
                write_to_cache();
            return;
        } catch (no_place_exception) {
            logging::cache_fw(request.get_absolute_url());
        }
    } else {
        logging::request_fw(request.get_absolute_url());
    }

    client->set_actions(POLL_WR);
    server->set_actions(0);
    stage = RESPONSE_CLIENT;
}

/*
 * cache takes control on server
 */
void proxy_session::write_to_cache() {
    entry = _cache.add_entry(request.get_absolute_url(), response.get_length(), server);
    string tmp = response.get_data();
    entry->add_data(tmp.data(), tmp.length());
    entry->add_observer(this);

    pollables.erase(server);

    client->set_actions(POLL_WR);
    stage = CACHE_CLIENT;
}

void proxy_session::cache_client_routine() {
    if (!client->is_writable() && client->get_actions() != 0)
        return;

    if (!entry->is_valid()) {
        close();
        return;
    }

    string str = entry->get_data();
    if (str.length() == entry_pos) {
        client->set_actions(0);
        return;
    }

    ssize_t n = client->write(str.data() + entry_pos, str.length() - entry_pos);
    if (n == -1) {
        close();
        return;
    }

    entry_pos += n;
    if (entry->is_complete() && entry_pos == str.length())
        set_complete();     //  success

    client->set_actions(POLL_WR);
}

void proxy_session::response_client_routine() {
    string str = response.get_data();
    ssize_t n = client->write(str.data() + response_pos, str.length() - response_pos);
    if (n == -1) {
        close();
        return;
    }

    response_pos += n;
    if (response_pos < str.length())
        return;

    auto fwd = new forward_session(server, client);
    pollables.clear();
    set_complete();

    throw (fwd);
}

void proxy_session::error_client_routine() {
    ssize_t n = client->write(error.data(), error.length());
    if (n == -1) {
        close();
        return;
    }

    error = error.substr((unsigned long) n);
    if (error.empty()) {
        set_complete();
        return;
    }
}
