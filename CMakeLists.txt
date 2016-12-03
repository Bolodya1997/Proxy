cmake_minimum_required(VERSION 3.3)
project(Proxy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES main.cpp
                 proxy.cpp)
set(NET_SOURCE net/server_socket.cpp
               net/net_exception.h
               net/socket.cpp
               net/deferred_socket.h
               net/deferred_socket_factory.cpp
               net/fd_exception.h)
set(POLL_SOURCE poll/poller.cpp
                poll/pollable.cpp)
set(HTTP_SOURCE http/request_parser.cpp
                http/response_parser.cpp)
set(CACHE_SOURCE cache/cache_entry.h
                 cache/cache.cpp
                 cache/cache_loader.h
                 cache/no_place_exception.h)
set(SESSION_SOURCE session/proxy_session.cpp
                   session/session.h
                   session/forward_session.cpp)
set(TEMPLATES_SOURCE templates/single_instance.h
                     templates/observer.h
                     templates/observable.h)
add_executable(Proxy ${SOURCE_FILES}
                     ${NET_SOURCE}
                     ${POLL_SOURCE}
                     ${HTTP_SOURCE}
                     ${CACHE_SOURCE}
                     ${SESSION_SOURCE}
                     ${TEMPLATES_SOURCE})