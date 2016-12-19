#include <iostream>
#include <memory>
#include <sys/resource.h>
#include "proxy/single_thread_proxy.h"

using namespace std;

uint16_t get_port(int argc, char **argv) {
    if (argc < 2){
        cerr << "Proxy port" << endl;
        exit(-1);
    }

    return (uint16_t) stoi(argv[1]);
}

/*
 * ./Proxy port
 */
int main(int argc, char **argv) {
    uint16_t port = get_port(argc, argv);

    rlimit limit;
    getrlimit(RLIMIT_NOFILE, &limit);
    limit.rlim_cur = 600;
    setrlimit(RLIMIT_NOFILE, &limit);

    single_thread_proxy _proxy(port);
    _proxy.start();
}