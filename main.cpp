#include <iostream>
#include <memory>
#include "proxy.h"
#include "net/socket.h"

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

    proxy _proxy(port);
    _proxy.start();
}