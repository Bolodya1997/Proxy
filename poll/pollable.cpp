#include <unistd.h>
#include "pollable.h"
#include "../net/socket.h"

using namespace std;

pollable::pollable() : filed(-1) {
    _pollfd.revents = 0;
}

pollable *pollable::set_actions(short actions) {
    acceptable = (bool) (actions & POLL_AC);

    _pollfd.events = 0;
    if (actions & (POLL_AC | POLL_RE))
        _pollfd.events |= POLLIN;

    if (actions & POLL_WR)
        _pollfd.events |= POLLOUT;

    return this;
}

pollfd pollable::get_pollfd() {
    _pollfd.fd = filed;

    return _pollfd;
}

void pollable::set_revents(short revents) {
    _pollfd.revents = revents;
}

void pollable::close() {
    _pollfd.revents = 0;
    closed = true;
}

bool pollable::is_closed() {
    return closed;
}

bool pollable::is_acceptable() {
    return acceptable && (_pollfd.revents & POLLIN);
}

pollable *pollable::accept() {
    _pollfd.revents &= ~POLLIN;

    return NULL;
}

bool pollable::is_readable() {
    return (bool) (_pollfd.revents & POLLIN);
}

ssize_t pollable::read(void *, size_t) {
    _pollfd.revents &= ~POLLIN;

    return 0;
}

bool pollable::is_writable() {
    return (bool) (_pollfd.revents & POLLOUT);
}

ssize_t pollable::write(const void *, size_t) {
    _pollfd.revents &= ~POLLOUT;

    return 0;
}
