#ifndef PROXY_CACHE_H
#define PROXY_CACHE_H

#include <map>
#include <string>
#include <memory>
#include "cache_entry.h"
#include "../templates/observable.h"
#include "../poll/pollable.h"
#include "no_place_exception.h"
#include "cache_loader.h"

class cache : public single_instance {

    static const unsigned long CACHE_CAPACITY = 1024 * 1024 * 5;
    static const unsigned long CACHE_PAGE_SIZE = CACHE_CAPACITY / 10 * 9;

    std::map<std::string, cache_entry *> entry_map;
    unsigned long size = 0;

    std::set<session *> &sessions;

public:
    cache(std::set<session *> &sessions) : sessions(sessions) { }

    cache_entry *get_entry(std::string &absolute_url);
    cache_entry *add_entry(std::string &absolute_url, unsigned long size, pollable *server);

private:
    void remove_last_used_entry(millis min_time);
    void remove_entry(std::map<std::string, cache_entry *>::iterator it);
};


#endif //PROXY_CACHE_H
