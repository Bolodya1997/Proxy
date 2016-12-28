#ifndef PROXY_QUERY_WRAP_H
#define PROXY_QUERY_WRAP_H

#include "../tadns/tadns.h"
#include "../poll/pollable.h"

class dns_wrap : public pollable,
                 public observer {

    dns *_dns;

public:
    dns_wrap() {
        _dns = dns_init();
        filed = dns_get_fd(_dns);

        set_owner(this);
    }

    void lookup(void *caller, std::string host, dns_callback_t callback) {
        dns_queue(_dns, caller, host.c_str(), DNS_A_RECORD, callback);
    }

    void end_lookup(void *caller) {
        dns_cancel(_dns, caller);
    }

    virtual void update() override { }

    virtual void update(void *arg) override {
        read(NULL, 0);
    }

    void close() override {
        pollable::close();
        dns_fini(_dns);
    }

    ssize_t read(void *buff, size_t n) override {
        pollable::read(buff, n);

        dns_poll(_dns);
        return 0;
    }

private:
    void __abstract_guard() override { }
};

#endif //PROXY_QUERY_WRAP_Hvoid dns_wrap::__abstract_guard(){
