#include <iostream>
#include "poller.h"
#include "../net/deferred_socket_factory.h"

using namespace std;
using namespace chrono;

void poller::add_timed(pollable *_pollable) {
    millis now = duration_cast<millis>(system_clock::now().time_since_epoch());
    auto *tmp = new timed_pollable();
    tmp->last_use = now;
    tmp->_pollable = _pollable;

    timed_pollables.insert(tmp);
}

void poller::add_untimed(pollable *_pollable) {
    untimed_pollables.insert({ _pollable });
}

void poller::poll() {
    unsigned long size = untimed_pollables.size() + timed_pollables.size();
    pollfd *pollfds = new pollfd[size];
    size = fill_pollfds(pollfds);

    ::poll(pollfds, size, MAX_WAIT_TIME);

    fill_ready(pollfds);
    fill_out_of_date();

    delete[] pollfds;
}

unsigned long poller::fill_pollfds(pollfd *pollfds) {
    unsigned long pos = 0;
    for (auto it = untimed_pollables.begin(); it != untimed_pollables.end();) {
        if ((*it)->is_closed()) {
            pollable *to_del = (*it);

            it = untimed_pollables.erase(it);
            delete to_del;

            net::deferred_socket_factory::get_instance()->update();
            continue;
        }
        pollfds[pos++] = (*it)->get_pollfd();
        ++it;
    }

    for (auto it = timed_pollables.begin(); it != timed_pollables.end();) {
        if ((*it)->_pollable->is_closed()) {
            timed_pollable *to_del = *it;

            it = timed_pollables.erase(it);
            delete to_del->_pollable;
            delete to_del;

            net::deferred_socket_factory::get_instance()->update();
            continue;
        }
        pollfds[pos++] = (*it)->_pollable->get_pollfd();
        ++it;
    }

    return pos;
}

void poller::fill_ready(pollfd *pollfds) {
    ready.clear();
    int pos = 0;
    for (auto it = untimed_pollables.begin(); it != untimed_pollables.end(); it++, pos++) {
        if (pollfds[pos].revents == 0)
            continue;

        (*it)->set_revents(pollfds[pos].revents);
        ready.push_back(*it);
    }

    millis now = duration_cast<millis>(system_clock::now().time_since_epoch());
    for (auto it = timed_pollables.begin(); it != timed_pollables.end(); it++, pos++) {
        if (pollfds[pos].revents == 0) {
            if (pollfds[pos].events == 0)
                (*it)->last_use = now;
            continue;
        }

        (*it)->last_use = now;
        (*it)->_pollable->set_revents(pollfds[pos].revents);
        ready.push_back((*it)->_pollable);
    }
}

void poller::fill_out_of_date() {
    millis now = duration_cast<millis>(system_clock::now().time_since_epoch());

    out_of_date.clear();
    for (auto it = timed_pollables.begin(); it != timed_pollables.end(); it++) {
        if ((now - (*it)->last_use).count() > MAX_WAIT_TIME)
            out_of_date.push_back((*it)->_pollable);
    }
}