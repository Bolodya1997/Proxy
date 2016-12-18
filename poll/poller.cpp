#include <iostream>
#include "poller.h"
#include "../net/accept_socket_factory.h"

using namespace std;
using namespace chrono;

void poller::add_timed(pollable *_pollable) {
    millis now = duration_cast<millis>(system_clock::now().time_since_epoch());
    auto *tmp = new timed_pollable();
    tmp->last_use = now;
    tmp->_pollable = _pollable;

    timed_pollables.push_back(tmp);
}

void poller::add_untimed(pollable *_pollable) {
    untimed_pollables.push_back(_pollable);
}

void poller::poll() {
    vector<pollfd> pollfds;
    fill_pollfds(pollfds);

    ::poll(pollfds.data(), pollfds.size(), MAX_WAIT_TIME);

    fill_ready(pollfds);
    fill_out_of_date();
}

void poller::fill_pollfds(vector<pollfd> &pollfds) {
    for (auto it = untimed_pollables.begin(); it != untimed_pollables.end();) {
        if ((*it)->is_closed()) {
            pollable *to_del = (*it);

            it = untimed_pollables.erase(it);
            delete to_del;

            continue;
        }
        pollfds.push_back((*it)->get_pollfd());
        ++it;
    }

    timed_pos = pollfds.size();

    for (auto it = timed_pollables.begin(); it != timed_pollables.end();) {
        if ((*it)->_pollable->is_closed()) {
            timed_pollable *to_del = *it;

            it = timed_pollables.erase(it);
            delete to_del->_pollable;
            delete to_del;

            continue;
        }
        pollfds.push_back((*it)->_pollable->get_pollfd());
        ++it;
    }
}

void poller::fill_ready(vector<pollfd> &pollfds) {
    ready.clear();
    int pos = 0;
    for (auto it = untimed_pollables.begin();
         it != untimed_pollables.end() && pos != timed_pos;
         it++, pos++) {

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