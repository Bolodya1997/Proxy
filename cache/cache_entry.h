#ifndef PROXY_CACHE_ENTRY_H
#define PROXY_CACHE_ENTRY_H

#include <string>
#include <chrono>
#include <cstring>
#include "../templates/single_instance.h"
#include "../templates/observable.h"

typedef std::chrono::milliseconds millis;

class cache_entry : public single_instance,
                    public observable {

    millis last_access_time;

    unsigned long size;
    std::string data;

    bool complete = false;
    bool valid = true;

public:
    cache_entry(size_t size) : size(size){
        auto now = std::chrono::system_clock::now().time_since_epoch();
        last_access_time = std::chrono::duration_cast<millis>(now);
    };

    void add_data(const void *buff, size_t n) {
        if (n < 1) {
            valid = false;
        } else {
            data.append((const char *) buff, n);
            if (data.length() == size)
                complete = true;
        }
        update_all();
    }

    const std::string &get_data() {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        last_access_time = std::chrono::duration_cast<millis>(now);

        return data;
    }

    millis get_last_access_time() {
        return last_access_time;
    }

    unsigned long get_size() {
        return size;
    }

    bool is_complete() {
        return complete;
    }

    bool is_in_use() {
        return !observers.empty();
    }

    bool is_valid() {
        return valid;
    }

private:
    void guard() override { };
};

#endif //PROXY_CACHE_ENTRY_H
