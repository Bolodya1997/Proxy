#ifndef PROXY_SESSION_H
#define PROXY_SESSION_H

#include <vector>
#include "session_rw_adapter.h"
#include "../templates/observer.h"

class session : public single_instance,
                public observer {
protected:
    static const int BUFF_SIZE = 1024 * 10;

    std::vector<session_rw_adapter *> adapters;

    char buff[BUFF_SIZE];

public:
    virtual ~session() {
        for (auto it = adapters.begin(); it != adapters.end(); it = adapters.erase(it)) {
            session_rw_adapter *cur = *it;
            cur->close();
        }
    }

    virtual bool is_complete() = 0;

protected:
    void set_session(session_rw_adapter *adapter) {
        adapter->_session = this;
    }
};

#endif //PROXY_SESSION_H
