#ifndef PROXY_FD_WATCHER_H
#define PROXY_FD_WATCHER_H

class fd_watcher {
public:
    virtual ~fd_watcher() { }

    virtual void update() = 0;
};

#endif //PROXY_FD_WATCHER_H
