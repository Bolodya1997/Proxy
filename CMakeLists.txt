cmake_minimum_required(VERSION 3.3)
project(Proxy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
link_libraries(pthread)

set(SOURCE_FILES main.cpp
                 logging.h)
set(PROXY_FILES proxy/single_thread_proxy.cpp
                proxy/proxy.cpp
                proxy/signal_wrap.h)
set(NET_SOURCE net/server_socket.cpp
               net/net_exception.h
               net/socket.cpp
               net/accept_socket_factory.cpp
               net/fd_exception.h)
set(POLL_SOURCE poll/poller.cpp
                poll/pollable.cpp
                poll/fd_watcher.h)
set(HTTP_SOURCE http/request_parser.cpp
                http/response_parser.cpp)
set(CACHE_SOURCE cache/cache_entry.h
                 cache/cache.cpp
                 cache/cache_loader.h
                 cache/no_place_exception.h)
set(SESSION_SOURCE session/proxy_session.cpp
                   session/session.h
                   session/forward_session.cpp
                   session/session_exception.h)
set(TEMPLATES_SOURCE templates/single_instance.h
                     templates/observer.h
                     templates/observable.h
                     templates/synchronisable.h)
set(THREAD_SOURCE thread/thread.h
                  thread/thread_exception.h
                  thread/mutex.h
                  thread/rw_lock.h
                  thread/conditional.h)
add_executable(Proxy ${SOURCE_FILES}
                     ${PROXY_FILES}
                     ${NET_SOURCE}
                     ${POLL_SOURCE}
                     ${HTTP_SOURCE}
                     ${CACHE_SOURCE}
                     ${SESSION_SOURCE}
                     ${TEMPLATES_SOURCE}
                     ${THREAD_SOURCE})