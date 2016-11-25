#ifndef PROXY_FORWARD_SESSION_H
#define PROXY_FORWARD_SESSION_H

#include "session.h"

/*
 * adapters[0] = input
 * adapters[1] = output
 */
class forward_session : public session {

    enum {
        INPUT_BUFF,
        BUFF_OUTPUT
    };
    int stage = INPUT_BUFF;

    session_rw_adapter *input;
    session_rw_adapter *output;

    size_t buff_write_pos = 0;
    size_t buff_read_pos = 0;

    bool revertable;
    bool complete = false;

public:
    forward_session(session_rw_adapter *input, session_rw_adapter *output,
                    bool revertable = false)
            : input(input), output(output), revertable(revertable) {
        adapters.push_back(input);
        set_session(input);

        adapters.push_back(output);
        set_session(output);

        input->set_actions(POLL_RE);
        output->set_actions(0);
    }

    void update() override;
    bool is_complete() override {
        return complete;
    }

private:
    void input_buff_routine();
    void buff_output_routine();
};

#endif //PROXY_FORWARD_SESSION_H
