#include "forward_session.h"

using namespace std;

void forward_session::update() {
    set_read(0);
    set_read(1);
    if (read_ready[0] || read_ready[1])
        read_routine();

    set_write(0);
    set_write(1);
    if (write_ready[0] || write_ready[1])
        write_routine();
}

void forward_session::set_read(int ad) {
    if (pollables[ad]->is_readable()) {
        short old_actions = pollables[ad]->get_actions();
        pollables[ad]->set_actions(old_actions & ~POLL_RE);
        read_ready[ad] = true;
    }
}

void forward_session::set_write(int ad) {
    if (pollables[ad]->is_writable()) {
        short old_actions = pollables[ad]->get_actions();
        pollables[ad]->set_actions(old_actions & ~POLL_WR);
        write_ready[ad] = true;
    }
}

void forward_session::read_routine() {
    if (in_pos != 0)
        return;

    if (read_ready[0]) {
        read_ready[0] = false;
        last_in = 0;
    } else {
        read_ready[1] = false;
        last_in = 1;
    }

    ssize_t n = pollables[last_in]->read(buff, BUFF_SIZE);
    if (n < 1) {
        complete = true;
        return;
    }

    in_pos = (size_t) n;
    return;
}

void forward_session::write_routine() {
    if (in_pos == 0)
        return;

    if (!write_ready[!last_in])  //  pollable can't read his data
        return;

    ssize_t n = pollables[!last_in]->write(buff + out_pos, in_pos - out_pos);
    if (n == -1) {
        complete = true;
        return;
    }

    out_pos += n;
    if (out_pos < in_pos)
        return;

    reset();
}
