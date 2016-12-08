#ifndef PROXY_LOGGING_H
#define PROXY_LOGGING_H

#include <string>
#include <iostream>
#include <chrono>

namespace logging {

    static void store(std::string url) {
//        std::cerr << "store: " + url << std::endl;
    }

    static void load(std::string url) {
//        std::cerr << "load: " + url << std::endl;
    }

    static void remove(std::string url) {
//        std::cerr << "remove: " + url << std::endl;
    }

    static void cache_fw(std::string url) {
//        std::cerr << "cache_fw: " + url << std::endl;
    }

    static void request_fw(std::string url) {
//        std::cerr << "request_fw: " + url << std::endl;
    }

    static void empty() {
        static int counter = 0;

        int i = 0;
        do {
            std::cerr << "------";
        } while (i++ < counter);
        std::cerr << std::endl;

        counter = (counter + 1) % 3;
    }

    static void get_deferred(unsigned long count) {
        std::cerr << "deferred +> " << count << std::endl;
    }

    static void init_deferred(unsigned long count) {
        std::cerr << "deferred -> " << count << std::endl;
    }

    static std::chrono::milliseconds start() {
        return std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::system_clock::now().time_since_epoch());
    }

    static void end(std::chrono::milliseconds &start) {
//        std::cerr
//        << (std::chrono::duration_cast<std::chrono::milliseconds>
//                            (std::chrono::system_clock::now().time_since_epoch()) - start).count()
//        << std::endl;
    }
}

#endif //PROXY_LOGGING_H
