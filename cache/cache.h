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
#include "../thread/mutex.h"
#include "../templates/synchronisable.h"

/*
 *      #######################################
 *      #                                     #
 *      #         MUST BE THREAD SAFE         #
 *      #                                     #
 *      #######################################
 */
class cache : public synchronisable {

    static const unsigned long CACHE_CAPACITY = 1024 * 1024 * 5;
    static const unsigned long CACHE_PAGE_SIZE = CACHE_CAPACITY / 10 * 9;

    std::map<std::string, cache_entry *> entry_map;
    unsigned long size = 0;

public:
    cache_entry *get_entry(std::string &absolute_url);
    std::pair<cache_entry *, cache_loader *> add_entry(std::string &absolute_url,
                                                     unsigned long size,
                                                     pollable *server);

private:
    void remove_last_used_entry(millis min_time);
    void remove_entry(std::map<std::string, cache_entry *>::iterator it);
};


#endif //PROXY_CACHE_H
