#include <iostream>
#include "proxy_session.h"
#include "../net/socket.h"
#include "../net/net_exception.h"

using namespace std;

void proxy_session::update() {
    switch (stage) {
        case CLIENT_REQUEST:
            client_request_routine();
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
        case WAIT_CACHE:
            wait_cache_routine();
            break;

        case RESPONSE_CLIENT:
            response_client_routine();
            break;
        case SERVER_BUFF:
            server_buff_routine();
            break;
        case BUFF_CLIENT:
            buff_client_routine();
    }
}

void proxy_session::client_request_routine() {
    ssize_t n = client->read(buff, BUFF_SIZE);
    if (n < 1) {
        set_complete();
        return;
    }

    request.add_data(buff, n);
    if (!request.is_ready())
        return;

    try {
        auto socket = new net::socket(request.get_host().first, request.get_host().second);
        server = new session_rw_adapter(this, socket);
    } catch (net_exception) {
        set_complete();
        return;
    }
    adapters.push_back(server);
    _poller.add(server->set_actions(POLL_WR));

    client->set_actions(0);
    stage = REQUEST_SERVER;
}

void proxy_session::request_server_routine() {
    string str = request.get_data();
    ssize_t n = server->write(str.data() + request_pos, str.length() - request_pos);
    if (n == -1) {
        set_complete();
        return;
    }

    request_pos += n;
    if (request_pos < str.length())
        return;

    if (!request.is_workable()) {
        set_complete();
        return;
    }

    entry = _cache.get_entry(request.get_absolute_url());
    if (entry) {
        read_from_cache();
    } else {
        server->set_actions(POLL_RE);
        stage = SERVER_RESPONSE;
    }
}

void proxy_session::read_from_cache() {
    entry->add_observer(this);

    cerr << "load: " << request.get_absolute_url() << endl;

    client->set_actions(POLL_WR);
    server->set_actions(0);
    stage = CACHE_CLIENT;
}

void proxy_session::server_response_routine() {
    ssize_t n = server->read(buff, BUFF_SIZE);
    if (n < 1) {
        set_complete();
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
            cerr << "ch fwd: " << request.get_absolute_url() << endl;
        }
    } else {
        cerr << "rq fwd: " << request.get_absolute_url() << endl;
    }

    client->set_actions(POLL_WR);
    server->set_actions(0);
    stage = RESPONSE_CLIENT;
}

/*
 * cache takes control on server
 */
void proxy_session::write_to_cache() throw(no_place_exception) {
    entry = _cache.add_entry(request.get_absolute_url(), response.get_length(),
                             server);
    string tmp = response.get_data();
    entry->add_data(tmp.data(), tmp.length());
    entry->add_observer(this);

    server = NULL;
    adapters.pop_back();    //  erase server

    client->set_actions(POLL_WR);
    stage = CACHE_CLIENT;
}

void proxy_session::cache_client_routine() {
    if (!client->is_writable())
        return;

    string str = entry->get_data();
    if (str.length() == entry_pos) {
        client->set_actions(0);
        stage = WAIT_CACHE;
        return;
    }

    ssize_t n = client->write(str.data() + entry_pos, str.length() - entry_pos);
    if (n == -1) {
        set_complete();
        return;
    }

    entry_pos += n;
    if (entry->is_complete() && entry_pos == str.length())
        set_complete();     //  success
}

void proxy_session::wait_cache_routine() {
    if (!entry->is_valid()) {
        set_complete();
        return;
    }

    string str = entry->get_data();
    ssize_t n = client->write(str.data() + entry_pos, str.length() - entry_pos);
    if (n == -1) {
        set_complete();
        return;
    }

    entry_pos += n;
    if (entry->is_complete() && entry_pos == str.length()) {
        set_complete();     //  success
        return;
    }

    client->set_actions(POLL_WR);
    stage = CACHE_CLIENT;
}

void proxy_session::response_client_routine() {
    string str = response.get_data();
    ssize_t n = client->write(str.data() + response_pos, str.length() - response_pos);
    if (n == -1) {
        set_complete();
        return;
    }

    response_pos += n;
    if (response_pos < str.length())
        return;

    client->set_actions(0);
    server->set_actions(POLL_RE);
    stage = SERVER_BUFF;
}

void proxy_session::server_buff_routine() {
    ssize_t n = server->read(buff, BUFF_SIZE);
    if (n < 1) {
        set_complete();
        return;
    }

    buff_write_pos = (size_t) n;

    client->set_actions(POLL_WR);
    server->set_actions(0);
    stage = BUFF_CLIENT;
}

void proxy_session::buff_client_routine() {
    ssize_t n = client->write(buff + buff_read_pos, buff_write_pos - buff_read_pos);
    if (n == -1) {
        set_complete();
        return;
    }

    buff_read_pos += n;
    if (buff_read_pos < buff_write_pos)
        return;

    buff_write_pos = 0;
    buff_read_pos = 0;

    client->set_actions(0);
    server->set_actions(POLL_RE);
    stage = SERVER_BUFF;
}
