#include <iostream>
#include <memory>
#include <sys/resource.h>
#include "proxy.h"

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

//    int fd = socket(AF_INET, SOCK_STREAM, 0);
//    addrinfo filter, *list;
//    bzero(&filter, sizeof(addrinfo));
//    filter.ai_family = AF_INET;
//    filter.ai_socktype = SOCK_STREAM;
//    getaddrinfo("img-s-msn-com.akamaized.net", "http", &filter, &list);
//
//    cerr << connect(fd, list->ai_addr, list->ai_addrlen) << endl;
//
    rlimit limit;
    getrlimit(RLIMIT_NOFILE, &limit);
    limit.rlim_cur = 100;
    setrlimit(RLIMIT_NOFILE, &limit);

    proxy _proxy(port);
    _proxy.start();
}