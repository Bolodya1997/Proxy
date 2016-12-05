#ifndef PROXY_POLLER_H
#define PROXY_POLLER_H

#include <set>
#include <sys/poll.h>
#include <vector>
#include <memory>
#include <chrono>
#include "pollable.h"

typedef std::chrono::milliseconds millis;

class poller : public single_instance {

    const int MAX_WAIT_TIME;

    struct timed_pollable {
        millis last_use;
        pollable *_pollable;
    };

    std::set<timed_pollable *> timed_pollables;
    std::set<pollable *> untimed_pollables;

    std::vector<pollable *> ready;
    std::vector<pollable *> out_of_date;

public:
    poller(int max_wait_time)
            : MAX_WAIT_TIME(max_wait_time) { }

    void add_timed(pollable *_pollable);
    void add_untimed(pollable *_pollable);

    void poll();

    std::vector<pollable *> &get_ready() {
        return ready;
    }

    std::vector<pollable *> &get_out_of_date() {
        return out_of_date;
    }

private:
    unsigned long fill_pollfds(pollfd *pollfds);
    void fill_ready(pollfd *pollfds);
    void fill_out_of_date();
;};

#endif //PROXY_POLLER_H
