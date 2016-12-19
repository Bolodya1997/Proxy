#include <iostream>
#include "cache.h"
#include "../logging.h"

using namespace std;
using namespace chrono;

cache_entry *cache::get_entry(string &absolute_url) {
    auto it = entry_map.find(absolute_url);
    if (it == entry_map.end())
        return NULL;

    if (!it->second->is_valid()) {
        remove_entry(it);
        return NULL;
    }

    return it->second;
}

std::pair<cache_entry *, cache_loader *> cache::add_entry(std::string &absolute_url,
                                                        unsigned long size,
                                                        pollable *server) {
    if (size > CACHE_PAGE_SIZE)
        throw (no_place_exception());

    millis cur = duration_cast<millis>(system_clock::now().time_since_epoch());
    while (this->size + size > CACHE_CAPACITY)
        remove_last_used_entry(cur);

    entry_map.insert({ absolute_url, new cache_entry(size) });
    this->size += size;
    cache_entry *entry = entry_map[absolute_url];

    logging::store(absolute_url);

    return { entry, new cache_loader(server, entry) };
}

void cache::remove_last_used_entry(millis min_time) {
    auto min_it = entry_map.end();
    for (auto it = entry_map.begin(); it != entry_map.end(); it++) {
        cache_entry *entry = it->second;
        if (entry->is_in_use() || !entry->is_complete())
            continue;

        millis entry_time = entry->get_last_access_time();
        if (entry_time < min_time) {
            min_time = entry_time;
            min_it = it;
        }
    }

    if (min_it == entry_map.end())
        throw (no_place_exception());

    remove_entry(min_it);
}

void cache::remove_entry(map<string, cache_entry *>::iterator it) {
    logging::remove(it->first);

    cache_entry *to_del = it->second;
    entry_map.erase(it);

    size -= to_del->get_size();
    delete to_del;
}
