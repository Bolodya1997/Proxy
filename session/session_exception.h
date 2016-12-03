#ifndef PROXY_SESSION_EXCEPTION_H
#define PROXY_SESSION_EXCEPTION_H

#include <exception>

class session_exception : public std::exception {

public:
    virtual const char *what() const noexcept override {
        return "not this session pollable";
    };
};

#endif //PROXY_SESSION_EXCEPTION_H
