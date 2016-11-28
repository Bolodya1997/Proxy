#include "poller.h"

using namespace std;
using namespace chrono;

const millis poller::MAX_WAIT_DURATION = duration<int>(poller::MAX_WAIT_TIME);

void poller::add_timed(pollable *_pollable) {
    int fd = _pollable->get_pollfd().fd;
    millis now = duration_cast<millis>(system_clock::now().time_since_epoch());

    timed_pollables.insert({ fd, { now, _pollable } });
}

void poller::add_untimed(pollable *_pollable) {
    int fd = _pollable->get_pollfd().fd;
    untimed_pollables.insert({ fd, _pollable });
}

void poller::poll() {
    unsigned long size = untimed_pollables.size() + timed_pollables.size();
    pollfd *pollfds = new pollfd[size];
    fill_pollfds(pollfds);

    ::poll(pollfds, size, MAX_WAIT_TIME);

    fill_ready(pollfds);
    fill_out_of_date();

    delete[] pollfds;
}

void poller::fill_pollfds(pollfd *pollfds) {
    int pos = 0;
    for (auto it = untimed_pollables.begin(); it != untimed_pollables.end();) {
        if (it->second->is_closed()) {
            pollable *to_del = it->second;

            it = untimed_pollables.erase(it);
            delete to_del;
            continue;
        }
        pollfds[pos++] = it->second->get_pollfd();
        ++it;
    }

    for (auto it = timed_pollables.begin(); it != timed_pollables.end();) {
        if (it->second._pollable->is_closed()) {
            pollable *to_del = it->second._pollable;

            it = timed_pollables.erase(it);
            delete to_del;
            continue;
        }
        pollfds[pos++] = it->second._pollable->get_pollfd();
        ++it;
    }
}

void poller::fill_ready(pollfd *pollfds) {
    ready.clear();
    int u_i;
    for (u_i = 0; u_i < untimed_pollables.size(); u_i++) {
        if (pollfds[u_i].revents == 0)
            continue;

        auto cur = untimed_pollables[pollfds[u_i].fd];
        cur->set_revents(pollfds[u_i].revents);
        ready.push_back(cur);
    }

    millis now = duration_cast<millis>(system_clock::now().time_since_epoch());
    for (int t_i = 0; t_i < timed_pollables.size(); t_i++) {
        if (pollfds[u_i + t_i].revents == 0)
            continue;

        auto cur = timed_pollables[pollfds[u_i + t_i].fd];
        cur.last_use = now;
        cur._pollable->set_revents(pollfds[u_i + t_i].revents);
        ready.push_back(cur._pollable);
    }
}

void poller::fill_out_of_date() {
    millis now = duration_cast<millis>(system_clock::now().time_since_epoch());

    out_of_date.clear();
    for (auto it = timed_pollables.begin(); it != timed_pollables.end(); it++) {
        if (now - it->second.last_use > MAX_WAIT_DURATION)
            out_of_date.push_back(it->second._pollable);
    }
}