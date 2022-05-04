cmake_minimum_required(VERSION 3.22)

project(mc833_projeto C)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP REQUIRED)

include_directories(src)

add_executable(server
        src/catalog.c
        src/catalog.h
        src/movie.c
        src/movie.h
        src/tcp/server.c
        src/utils.h
        src/net_utils.c
        src/net_utils.h)
target_link_libraries(server PUBLIC OpenMP::OpenMP_C)

add_executable(client
        src/catalog.c
        src/catalog.h
        src/movie.c
        src/movie.h
        src/tcp/client.c
        src/utils.h
        src/net_utils.c
        src/net_utils.h)
