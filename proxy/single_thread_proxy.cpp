#include <iostream>
#include "single_thread_proxy.h"
#include "../session/proxy_session.h"

using namespace std;

single_thread_proxy::single_thread_proxy(cache *proxy_cache)
        : proxy_cache(proxy_cache) { }

single_thread_proxy::single_thread_proxy(cache *proxy_cache, pollable *notifier)
        : single_thread_proxy(proxy_cache) {

    notifier->set_owner(this);
    proxy_poller.add_untimed(notifier->set_actions(POLL_RE));
}

void single_thread_proxy::start() {
    while (true) {
        synchronize();

        proxy_poller.poll();

        handle_ready();
        clean_out_of_date();
        clean_completed_sessions();
    }
}

void single_thread_proxy::synchronize() {
    cond.get_mutex().lock();

    do {
        for (auto it = accepted.begin(); it != accepted.end(); it = accepted.erase(it))
            sessions.insert(new proxy_session(proxy_poller, *proxy_cache, *it));

        if (sessions.empty())
            cond.wait();
        else
            break;

    } while (true);

    cond.get_mutex().unlock();
}

void single_thread_proxy::handle_ready() {
    vector<pollable *> &ready = proxy_poller.get_ready();

    if (ready.empty())
        logging::empty();

    for (auto it = ready.begin(); it != ready.end(); it++) {
        pollable *cur = *it;

        if (cur->is_connectable() || cur->is_readable() || cur->is_writable()) {
            try {
                cur->get_owner()->update(cur);
            } catch (session *_session) {
                sessions.insert(_session);
            } catch (...) {
                auto *cur_session = dynamic_cast<session *>(cur->get_owner());
                cur_session->close();
            }
        }
    }
}

void single_thread_proxy::clean_out_of_date() {
    vector<pollable *> &out_of_date = proxy_poller.get_out_of_date();
    for (auto it = out_of_date.begin(); it != out_of_date.end(); it++) {
        pollable *cur = *it;

        auto *cur_session = dynamic_cast<session *>(cur->get_owner());
        cur_session->close();
    }
}

void single_thread_proxy::clean_completed_sessions() {
    for (auto it = sessions.begin(); it != sessions.end();) {
        session *cur = *it;
        if (!cur->is_complete()) {
            it++;
            continue;
        }

        it = sessions.erase(it);
        delete cur;
    }
}
