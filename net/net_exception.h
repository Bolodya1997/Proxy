#ifndef PROXY_NET_EXCEPTION_H
#define PROXY_NET_EXCEPTION_H

#include <exception>
#include <string>
#include <cstring>

class net_exception : public std::exception {

    std::string cause;
public:
    net_exception(std::string cause) : cause(cause) { }

    const char *what() const noexcept override {
        return (cause + ": " + strerror(errno)).data();
    }
};


#endif //PROXY_NET_EXCEPTION_H
