#ifndef PROXY_POLLABLE_H
#define PROXY_POLLABLE_H

#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>
#include "../templates/single_instance.h"

enum {
    POLL_AC = 0x01,
    POLL_RE = 0x02,
    POLL_WR = 0x04
};

class pollable : public single_instance {

    pollfd _pollfd;
    bool acceptable;

    bool closed = false;

protected:
    int filed;

    int &get_filed(pollable &obj) {
        return obj.filed;
    }

public:
    pollable();
    virtual ~pollable() {
        ::close(filed);
    }

    void close();
    bool is_closed() {
        return closed;
    }

    pollable *set_actions(short actions);
    short get_actions();

    bool is_acceptable();
    virtual pollable *accept();

    bool is_readable();
    virtual ssize_t read(void *buff, size_t n);

    bool is_writable();
    virtual ssize_t write(const void *buff, size_t n);

private:
    pollfd get_pollfd();
    void set_revents(short revents) {
        _pollfd.revents = revents;
    }

    virtual void guard() = 0;

    friend class poller;
};

#endif //PROXY_POLLABLE_H
