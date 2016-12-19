#include <iostream>
#include <memory>
#include <sys/resource.h>
#include "proxy/proxy.h"

using namespace std;

string error = "Proxy port [thread_count]";

uint16_t port;
unsigned thread_count = 1;

void parse_arguments(int argc, char **argv) {
    if (argc < 2)
        throw (exception());

    port = (uint16_t) stoi(argv[1]);

    if (argc > 2)
        thread_count = (unsigned int) stoul(argv[2]);
}

/*
 * ./Proxy port [thread count]
 */
int main(int argc, char **argv) {
    try {
        parse_arguments(argc, argv);
    } catch (exception) {
        cerr << error << endl;
        return -1;
    }

    rlimit limit;
    getrlimit(RLIMIT_NOFILE, &limit);
    limit.rlim_cur = 600;
    setrlimit(RLIMIT_NOFILE, &limit);

    proxy _proxy(port, thread_count);
    _proxy.start();
}