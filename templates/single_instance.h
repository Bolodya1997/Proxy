#ifndef PROXY_SINGLE_INSTANCE_H
#define PROXY_SINGLE_INSTANCE_H

class single_instance {
public:
    single_instance() { }
    virtual ~single_instance() { }

    single_instance(const single_instance &other) = delete;
    single_instance &operator=(const single_instance &other) = delete;
};

#endif //PROXY_SINGLE_INSTANCE_H
