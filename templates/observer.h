#ifndef PROXY_OBSERVER_H
#define PROXY_OBSERVER_H

class observer {
public:
    virtual void update() = 0;
    virtual void update(void *arg) = 0;
};

#endif //PROXY_OBSERVER_H
