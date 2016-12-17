#ifndef PROXY_OBSERVABLE_H
#define PROXY_OBSERVABLE_H

#include <set>
#include <vector>
#include "observer.h"

class observable {
protected:
    std::set<observer *> observers;

public:
    virtual ~observable() { }

    virtual void add_observer(observer *_observer) {
        observers.insert(_observer);
    }

    virtual void remove_observer(observer *_observer) {
        auto to_del = observers.find(_observer);
        if (to_del != observers.end())
            observers.erase(to_del);
    }

    virtual void update_all() {
        std::vector<observer *> to_update;

        for (auto it = observers.begin(); it != observers.end(); it++)
            to_update.push_back(*it);

        for (auto it = to_update.begin(); it != to_update.end(); it++)
            (*it)->update();
    }

private:
    virtual void guard() = 0;
};

#endif //PROXY_OBSERVABLE_H
