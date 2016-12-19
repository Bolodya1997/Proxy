#ifndef PROXY_CACHE_ENTRY_H
#define PROXY_CACHE_ENTRY_H

#include <string>
#include <chrono>
#include <cstring>
#include "../templates/single_instance.h"
#include "../templates/observable.h"
#include "../thread/rw_lock.h"
#include "../templates/synchronisable.h"

typedef std::chrono::milliseconds millis;

/*
 *      #######################################
 *      #                                     #
 *      #         MUST BE THREAD SAFE         #
 *      #                                     #
 *      #######################################
 */
class cache_entry : public observable {

    millis last_access_time;

    rw_lock lock;

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
        lock.write_lock();

        if (n < 1) {
            valid = false;
        } else {
            data.append((const char *) buff, n);
            if (data.length() == size)
                complete = true;
        }

        lock.unlock();
        update_all();
    }

    void read_start() {
        lock.read_lock();
    }

    void read_end() {
        lock.unlock();
    }

    //  read lock
    const std::string &get_data() {
        if (guard.try_lock()) {
            auto now = std::chrono::system_clock::now().time_since_epoch();
            last_access_time = std::chrono::duration_cast<millis>(now);

            guard.unlock();
        }

        return data;
    }

    millis get_last_access_time() {
        critical_section_open(this);

        return last_access_time;

        critical_section_close;
    }

    //  read lock
    unsigned long get_size() {
        return size;
    }

    //  read lock
    bool is_complete() {
        return complete;
    }

    bool is_in_use() {
        return !is_empty();
    }

    //  read lock
    bool is_valid() {
        return valid;
    }

private:
    void __abstract_guard() override { };

    friend class cache_loader;
};

#endif //PROXY_CACHE_ENTRY_H
