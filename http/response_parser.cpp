#include "response_parser.h"

using namespace http;
using namespace std;

void response_parser::add_data(const char *in_data, ssize_t size) {
    string *last = &data.back();

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
 * standard code text_code\r\n
 * ...\r\n
 * Content-Length: length\r\n
 * ...\r\n
 * \r\n
 *
 * MUST BE:
 *      code == 200
 */
void response_parser::parse() {
    string &head = data.front();

    unsigned long code_begin = head.find(' ') + 1;
    unsigned long code_end = head.find(' ', code_begin);
    string code = head.substr(code_begin, code_end - code_begin);
    if (code != "200")
        workable = false;

    head_length = head.length();
    for (auto it = ++data.begin(); it != data.end(); it++) {
        if (it->find("Content-Length:") == 0) {
            unsigned long length_begin = it->find(' ') + 1;
            string _length = it->substr(length_begin);

            length = stoul(_length.erase(_length.length() - 2));
        }

        head_length += it->length();
    }

    if (length == -1)
        workable = false;

    ready = true;
}

string response_parser::get_data() {
    string res;
    for (auto it = data.begin(); it != data.end(); it++)
        res += *it;

    return res;
}
