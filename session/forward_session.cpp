#include "forward_session.h"

using namespace std;

void forward_session::update() {
    if (stage == INPUT_BUFF)
        input_buff_routine();
    else
        buff_output_routine();
}

void forward_session::input_buff_routine() {
    ssize_t n = input->read(buff, BUFF_SIZE);
    if (n < 1) {
        if (!revertable) {
            complete = true;
            return;
        }

        swap(input, output);
        revertable = false;

        input->set_actions(POLL_RE);
        output->set_actions(0);
        stage = INPUT_BUFF;
        return;
    }

    buff_write_pos = (size_t) n;

    input->set_actions(0);
    output->set_actions(POLL_WR);
    stage = BUFF_OUTPUT;
}

void forward_session::buff_output_routine() {
    ssize_t n = output->write(buff + buff_read_pos, buff_write_pos - buff_read_pos);
    if (n == -1) {
        complete = true;
        return;
    }

    buff_read_pos += n;
    if (buff_read_pos < buff_write_pos)
        return;

    buff_write_pos = 0;
    buff_read_pos = 0;

    input->set_actions(POLL_RE);
    output->set_actions(0);
    stage = INPUT_BUFF;
}
