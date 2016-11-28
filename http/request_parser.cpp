#include "request_parser.h"

using namespace http;
using namespace std;

void request_parser::add_data(char *in_data, ssize_t size) {
    string *last = &data.front();

    for (int i = 0; i < size; i++) {
        if (last->find("\r\n") != string::npos) {
            data.push_back(string());
            last = &data.back();
        }

        (*last) += in_data[i];
        if (*last == "\r\n" && !ready)
            parse();
    }
}

/*
 * method uri standard\r\n
 * Host: hostname\r\n
 * ...\r\n
 * \r\n
 *
 * MUST BE:
 *      method   == GET
 *      standard == HTTP/1.0
 */
void request_parser::parse() {
    string &head = data.front();

    unsigned long method_end = head.find(' ');
    string method = head.substr(0, method_end);
    if (method != "GET")
        workable = false;

    unsigned long url_start = method_end + 1;
    unsigned long url_end = head.find(' ', url_start);
    absolute_url = head.substr(url_start, url_end - url_start);

    string standard = head.substr(url_end + 1);
//    if (standard != "HTTP/1.0\r\n")
//        workable = false;

    for (auto it = ++data.begin(); it != data.end(); it++) {
        if (it->find("Host:") == 0) {
            unsigned long hostname_start = it->find(' ') + 1;

            hostname = it->substr(hostname_start);
            hostname.erase(hostname.length() - 2);
        } else if (it->find("Connection:") == 0) {
            *it = "Connection: close\r\n";
        }
    }
    parse_url();
    parse_hostname();

    head = method + ' ' + local_url + ' ' + standard;
    ready = true;
}

/*
 * http://hostname/local_url
 */
void request_parser::parse_url() {
    unsigned long hostname_start = absolute_url.find("://") + 3;
    if (hostname_start - 3 == string::npos) {
        local_url = absolute_url;
        absolute_url = "http://" + hostname + local_url;
        return;
    }

    unsigned long hostname_end = absolute_url.find('/', hostname_start);
    if (hostname.empty())
        hostname = absolute_url.substr(hostname_start, hostname_end - hostname_start);

    local_url = absolute_url.substr(hostname_end);
}

/*
 * hostname[:port]
 */
void request_parser::parse_hostname() {
    unsigned long port_start = hostname.find(':');
    if (port_start == string::npos) {
        port = 80;
    } else {
        port = (uint16_t) stoi(hostname.substr(port_start + 1));
        hostname = hostname.substr(0, port_start);
    }
}

string request_parser::get_data() {
    string res;
    for (auto it = data.begin(); it != data.end(); it++)
        res += *it;

    return res;
}
