#ifndef PROXY_SIGNAL_WRAP_H
#define PROXY_SIGNAL_WRAP_H

#include <sys/signalfd.h>
#include <signal.h>
#include "../poll/pollable.h"

class signal_wrap : public pollable {

    sigset_t mask;

public:
    signal_wrap(int signum) {
        sigemptyset(&mask);
        sigaddset(&mask, signum);
        if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
            throw (std::exception());

        filed = signalfd(-1, &mask, SFD_NONBLOCK);
        if (filed == -1) {
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            throw (std::exception());
        }
    }

    ~signal_wrap() {
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
    }

    virtual ssize_t read(void *buff, size_t) override {
        signalfd_siginfo tmp;
        if (buff == NULL)
            buff = &tmp;

        return ::read(filed, buff, sizeof(signalfd_siginfo));
    }

private:
    void __abstract_guard() override { };
};

#endif //PROXY_SIGNAL_WRAP_H
