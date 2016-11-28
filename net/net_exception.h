#ifndef PROXY_NET_EXCEPTION_H
#define PROXY_NET_EXCEPTION_H

#include <exception>
#include <string>

class net_exception : public std::exception {

    std::string cause;
public:
    net_exception(std::string cause) : cause(cause) { }

    const char *what() const noexcept (true) {
        return cause.data();
    }
};


#endif //PROXY_NET_EXCEPTION_H
