#include <iostream>
#include "cache.h"

using namespace std;
using namespace chrono;

cache_entry *cache::get_entry(string &absolute_url) {
    auto it = entry_map.find(absolute_url);
    if (it == entry_map.end())
        return NULL;

    return it->second;
}

cache_entry *cache::add_entry(string &absolute_url, unsigned long size,
                      pollable *server) throw(no_place_exception) {
    if (size > CACHE_PAGE_SIZE)
        throw (no_place_exception());

    millis cur = duration_cast<millis>(system_clock::now().time_since_epoch());
    while (this->size + size > CACHE_CAPACITY)
        remove_last_used_entry(cur);

    entry_map.insert({ absolute_url, new cache_entry(size) });
    this->size += size;
    cache_entry *entry = entry_map[absolute_url];

    cerr << "store: " << absolute_url << endl;

    sessions.insert(new cache_loader(server, entry));
    return entry;
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

    cerr << "remove: " << min_it->first << endl;

    cache_entry *entry = min_it->second;
    entry_map.erase(min_it);

    size -= entry->get_size();
    delete entry;
}
