#ifndef PROXY_REQUEST_PARSER_H
#define PROXY_REQUEST_PARSER_H

#include <string>
#include <vector>
#include "../templates/single_instance.h"

namespace http {

    class request_parser : public single_instance {

        std::string local_url;
        std::string absolute_url;

        std::string hostname;
        uint16_t port;

        std::vector<std::string> data;

        bool ready = false;
        bool workable = true;

    public:
        request_parser() {
            data.push_back(std::string());
        }

        void add_data(char *in_data, ssize_t size);

        bool is_ready() {
            return ready;
        }

        bool is_workable() {
            return workable;
        }

        std::string &get_absolute_url() {
            return absolute_url;
        }

        std::pair<std::string, uint16_t> get_host() {
            return { hostname, port };
        }

        std::string get_data();

    private:
        void parse();

        void parse_hostname();
        void parse_url();
    };

}

#endif //PROXY_REQUEST_PARSER_H
