#include "poller.h"

using namespace std;

void poller::add(pollable * _pollable) {
    int fd = _pollable->get_pollfd().fd;

    pollables.insert({ fd, _pollable });
}

void poller::poll() {
    pollfd *pollfds = new pollfd[pollables.size()];

    int pos = 0;
    for (auto it = pollables.begin(); it != pollables.end();) {
        if (it->second->is_closed()) {
            pollable *to_del = it->second;
            
            it = pollables.erase(it);
            delete to_del;
            continue;
        }
        pollfds[pos++] = it->second->get_pollfd();
        ++it;
    }

    ::poll(pollfds, pollables.size(), timeout);

    ready.clear();
    for (int i = 0; i < pollables.size(); i++) {
        if (pollfds[i].revents == 0)
            continue;

        auto cur = pollables[pollfds[i].fd];
        cur->set_revents(pollfds[i].revents);
        ready.push_back(cur);
    }

    delete[] pollfds;
}

std::vector<pollable *> &poller::get_ready() {
    return ready;
}