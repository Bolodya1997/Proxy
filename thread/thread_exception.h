#ifndef PROXY_THREAD_EXCEPTION_H
#define PROXY_THREAD_EXCEPTION_H

#include <exception>

class thread_exception : public std::exception {

public:
    virtual const char *what() const noexcept override {
        return "thread exception";
    };
};

#endif //PROXY_THREAD_EXCEPTION_H
