#ifndef PROXY_RESPONSE_PARSER_H
#define PROXY_RESPONSE_PARSER_H

#include <string>
#include <vector>
#include "../templates/single_instance.h"

namespace http {

    class response_parser : public single_instance {

        unsigned long head_length;
        unsigned long length = (unsigned long) -1;

        std::vector<std::string> data;

        bool ready = false;
        bool workable = true;

    public:
        response_parser() {
            data.push_back(std::string());
        }

        void add_data(const char *in_data, ssize_t size);

        bool is_ready() {
            return ready;
        }

        bool is_workable() {
            return workable;
        }

        std::string get_data();

        unsigned long get_length() {
            return head_length + length;
        }

    private:
        void parse();
    };

}

#endif //PROXY_RESPONSE_PARSER_H
