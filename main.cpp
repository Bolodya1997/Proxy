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

struct A {
    ~A() {
        cout << "A destroyed" << endl;
    }
};

struct B {
    ~B() {
        cout << "B destroyed" << endl;
    }

    A foo1() {
        cout << "foo1" << endl;
        return A();
    }

    void foo2() {
        foo1();
        cout << "foo2" << endl;
    }
};

/*
 * ./Proxy port
 */
int main(int argc, char **argv) {
    uint16_t port = get_port(argc, argv);

//    B b;
//    b.foo2();

    rlimit limit;
    getrlimit(RLIMIT_NOFILE, &limit);
    limit.rlim_cur = 600;
    setrlimit(RLIMIT_NOFILE, &limit);

    proxy _proxy(port);
    _proxy.start();
}