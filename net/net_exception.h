#ifndef PROXY_NET_EXCEPTION_H
#define PROXY_NET_EXCEPTION_H

#include <exception>
#include <string>
#include <string.h>

class net_exception : public std::exception {

    std::string cause;
public:
    net_exception(std::string cause, int code) : cause(cause) {
        this->cause += ": " + std::string(strerror(code));
    }
    ~net_exception() { };

    const char *what() const noexcept (true) {
        return cause.data();
    }
};


#endif //PROXY_NET_EXCEPTION_H
