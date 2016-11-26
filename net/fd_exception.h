#ifndef PROXY_FD_EXCEPTION_H
#define PROXY_FD_EXCEPTION_H

#include <exception>

class fd_exception : public std::exception {

public:
    const char *what() const noexcept {
        return "no file descriptors available";
    };
};

#endif //PROXY_FD_EXCEPTION_H
