#ifndef PROXY_SIGNAL_WRAP_H
#define PROXY_SIGNAL_WRAP_H

#include <sys/signalfd.h>
#include <signal.h>
#include "../poll/pollable.h"

class signal_wrap : public pollable {

    int signum;
    sigset_t mask;

public:
    signal_wrap(int signum) : signum(signum) {

        sigemptyset(&mask);
        sigaddset(&mask, signum);
        sigprocmask(SIG_BLOCK, &mask, NULL);

        filed = signalfd(-1, &mask, SFD_NONBLOCK);
        if (filed < 0) {
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            throw (net_exception("signalfd"));
        }
    }

    virtual void close() override {
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        signum = -1;
    }

    virtual bool is_closed() override {
        return signum == -1;
    }

    ssize_t read(void *buff, size_t n) override {
        pollable::read(buff, n);
        return ::read(filed, buff, n);
    }

private:
    virtual void guard() override { }
};

#endif //PROXY_SIGNAL_WRAP_H
