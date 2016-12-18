#include <unistd.h>
#include "pollable.h"
#include "../net/socket.h"

using namespace std;

fd_watcher *pollable::watcher = NULL;

pollable::pollable() : filed(-1) {
    _pollfd.events = 0;
    _pollfd.revents = 0;
}

pollable *pollable::set_actions(short actions) {
    acceptable = (bool) (actions & POLL_AC);
    connectable = (bool) (actions & POLL_CO);

    _pollfd.events = 0;
    if (actions & (POLL_AC | POLL_RE))
        _pollfd.events |= POLLIN;

    if (actions & (POLL_WR | POLL_CO))
        _pollfd.events |= POLLOUT;

    return this;
}

short pollable::get_actions() {
    short res = 0;

    if (acceptable)
        res |= POLL_AC;

    if (connectable)
        res |= POLL_CO;

    if (_pollfd.events & POLLIN)
        res |= POLL_RE;

    if (_pollfd.events & POLLOUT)
        res |= POLL_WR;

    return res;
}

pollfd pollable::get_pollfd() {
    _pollfd.fd = filed;

    return _pollfd;
}

void pollable::close() {
    _pollfd.revents = 0;
    closed = true;
}

bool pollable::is_acceptable() {
    return acceptable && (_pollfd.revents & POLLIN);
}

pollable *pollable::accept() {
    _pollfd.revents &= ~POLLIN;

    return NULL;
}

bool pollable::is_connectable() {
    return connectable && (_pollfd.revents & POLLOUT);
}

void pollable::connect() {
    _pollfd.revents &= ~POLLOUT;
}

bool pollable::is_readable() {
    if (is_acceptable())
        return false;

    return (bool) (_pollfd.revents & POLLIN);
}

ssize_t pollable::read(void *, size_t) {
    _pollfd.revents &= ~POLLIN;

    return 0;
}

bool pollable::is_writable() {
    if (is_connectable())
        return false;

    return (bool) (_pollfd.revents & POLLOUT);
}

ssize_t pollable::write(const void *, size_t) {
    _pollfd.revents &= ~POLLOUT;

    return 0;
}
