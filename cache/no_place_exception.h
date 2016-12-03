#ifndef PROXY_NO_PLACE_EXCEPTION_H
#define PROXY_NO_PLACE_EXCEPTION_H

#include <exception>

class no_place_exception : public std::exception {

public:
    virtual const char *what() const noexcept override {
        return "data is too big to be cached";
    }
};

#endif //PROXY_NO_PLACE_EXCEPTION_H
