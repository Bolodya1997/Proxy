#ifndef PROXY_FORWARD_SESSION_H
#define PROXY_FORWARD_SESSION_H

#include "session.h"

/*
 * pollables[0] = input
 * pollables[1] = output
 */
class forward_session : public session {

    int last_in;

    bool read_ready[2] = { false, false };
    bool write_ready[2] = { false, false };

    size_t in_pos = 0;
    size_t out_pos = 0;

    bool complete = false;

public:
    forward_session(pollable *_1, pollable *_2) {
        pollables.push_back(_1);
        _1->set_owner(this);

        pollables.push_back(_2);
        _2->set_owner(this);

        _1->set_actions(POLL_RE | POLL_WR);
        _2->set_actions(POLL_RE | POLL_WR);
    }

    void update() override;
    bool is_complete() override {
        return complete;
    }

private:
    void set_write(int ad);
    void set_read(int ad);

    void read_routine();
    void write_routine();

    void reset() {
        in_pos = 0;
        out_pos = 0;

        pollables[0]->set_actions(POLL_RE | POLL_WR);
        pollables[1]->set_actions(POLL_RE | POLL_WR);
    }
};

#endif //PROXY_FORWARD_SESSION_H
