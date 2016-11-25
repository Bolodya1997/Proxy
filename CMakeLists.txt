cmake_minimum_required(VERSION 3.3)
project(Proxy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp
                 proxy.cpp proxy.h)
set(NET_SOURCE net/server_socket.cpp net/server_socket.h
               net/net_exception.h
               net/socket.cpp net/socket.h)
set(POLL_SOURCE poll/poller.cpp poll/poller.h
                poll/pollable.cpp poll/pollable.h)
set(HTTP_SOURCE http/request_parser.cpp http/request_parser.h
                http/response_parser.cpp http/response_parser.h)
set(CACHE_SOURCE cache/cache_entry.h
                 cache/cache.cpp cache/cache.h
                 cache/cache_loader.h
                 cache/no_place_exception.h)
set(SESSION_SOURCE session/proxy_session.cpp session/proxy_session.h
                   session/session_rw_adapter.h
                   session/session.h
                   session/forward_session.cpp session/forward_session.h)
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