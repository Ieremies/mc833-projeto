cmake_minimum_required(VERSION 3.18)

project(mc833_projeto C)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP REQUIRED)

include_directories(src)

add_executable(tcp-server
        src/data/catalog.c
        src/data/catalog.h
        src/data/movie.c
        src/data/movie.h
        src/tcp/server.c
        src/utils/utils.h
        src/utils/utils.c
        src/utils/net_utils.c
        src/utils/net_utils.h)
target_link_libraries(tcp-server PUBLIC OpenMP::OpenMP_C)

add_executable(tcp-client
        src/data/catalog.c
        src/data/catalog.h
        src/data/movie.c
        src/data/movie.h
        src/tcp/client.c
        src/utils/utils.h
        src/utils/net_utils.c
        src/utils/net_utils.h src/utils/utils.c)

add_executable(udp-server
        src/data/catalog.c
        src/data/catalog.h
        src/data/movie.c
        src/data/movie.h
        src/udp/server.c
        src/utils/utils.h
        src/utils/utils.c
        src/utils/net_utils.c
        src/utils/net_utils.h)

add_executable(udp-client
        src/data/catalog.c
        src/data/catalog.h
        src/data/movie.c
        src/data/movie.h
        src/udp/client.c
        src/utils/utils.h
        src/utils/net_utils.c
        src/utils/net_utils.h)