#ifndef PROXY_SESSION_RW_ADAPTER_H
#define PROXY_SESSION_RW_ADAPTER_H

#include "../poll/pollable.h"
#include "session.h"

class session;

class session_rw_adapter : public pollable {

    session *_session;
    pollable * const rw_pollable;

public:
    session_rw_adapter(session *_session, pollable *rw_pollable)
            : _session(_session), rw_pollable(rw_pollable) {
        filed = get_filed(*rw_pollable);
    }
    ~session_rw_adapter() {
        delete rw_pollable;
        filed = -1;
    }

    session *get_session() {
        return _session;
    }

    ssize_t read(void *buff, size_t n) override {
        return rw_pollable->read(buff, n);
    }

    ssize_t write(const void *buff, size_t n) override {
        return rw_pollable->write(buff, n);
    }

private:
    void guard() override { }

    friend class session;
};

#endif //PROXY_SESSION_RW_ADAPTER_H
