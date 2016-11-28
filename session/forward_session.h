#ifndef PROXY_FORWARD_SESSION_H
#define PROXY_FORWARD_SESSION_H

#include "session.h"

/*
 * adapters[0] = input
 * adapters[1] = output
 */
class forward_session : public session {

    int last_in;

    bool read_ready[2] = { false, false };
    bool write_ready[2] = { false, false };

    size_t in_pos = 0;
    size_t out_pos = 0;

    bool complete = false;

public:
    forward_session(net::socket *_1, net::socket *_2) {
        adapters.push_back(_1);
        _1->set_session(this);

        adapters.push_back(_2);
        _2->set_session(this);

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

        adapters[0]->set_actions(POLL_RE | POLL_WR);
        adapters[1]->set_actions(POLL_RE | POLL_WR);
    }
};

#endif //PROXY_FORWARD_SESSION_H
