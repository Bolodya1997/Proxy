#ifndef PROXY_OBSERVABLE_H
#define PROXY_OBSERVABLE_H

#include <set>
#include <vector>
#include "observer.h"
#include "synchronisable.h"

/*
 *      #######################################
 *      #                                     #
 *      #         MUST BE THREAD SAFE         #
 *      #                                     #
 *      #######################################
 */
class observable : public synchronisable {
    std::set<observer *> observers;

public:
    virtual ~observable() { }

    virtual void add_observer(observer *_observer) {
        critical_section_open(this);

        observers.insert(_observer);

        critical_section_close;
    }

    virtual void remove_observer(observer *_observer) {
        critical_section_open(this);

        auto to_del = observers.find(_observer);
        if (to_del != observers.end())
            observers.erase(to_del);

        critical_section_close;
    }

    virtual bool is_empty() {
        critical_section_open(this);

        return observers.empty();

        critical_section_close;
    }

    virtual void update_all() {
        std::vector<observer *> to_update;
        critical_section_open(this);

        for (auto it = observers.begin(); it != observers.end(); it++)
            to_update.push_back(*it);

        for (auto it = to_update.begin(); it != to_update.end(); it++)
            (*it)->update();

        critical_section_close;
    }

private:
    virtual void __abstract_guard() = 0;
};

#endif //PROXY_OBSERVABLE_H
