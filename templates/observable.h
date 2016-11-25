#ifndef PROXY_OBSERVABLE_H
#define PROXY_OBSERVABLE_H

#include <set>
#include "observer.h"

class observable {
protected:
    std::set<observer *> observers;

public:
    virtual void add_observer(observer *_observer) {
        observers.insert(_observer);
    }

    virtual void remove_observer(observer *_observer) {
        auto to_del = observers.find(_observer);
        if (to_del != observers.end())
            observers.erase(to_del);
    }

    virtual void update_all() {
        for (auto it = observers.begin(); it != observers.end(); it++)
            (*it)->update();
    }

private:
    virtual void guard() = 0;
};

#endif //PROXY_OBSERVABLE_H
