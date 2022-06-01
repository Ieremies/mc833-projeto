cmake_minimum_required(VERSION 3.18)

project(mc833_projeto C)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP REQUIRED)

include_directories(src)

add_executable(server
        src/data/catalog.c
        src/data/catalog.h
        src/data/movie.c
        src/data/movie.h
        src/tcp/server.c
        src/utils/utils.h
        src/utils/net_utils.c
        src/utils/net_utils.h)
target_link_libraries(server PUBLIC OpenMP::OpenMP_C)

add_executable(client
        src/data/catalog.c
        src/data/catalog.h
        src/data/movie.c
        src/data/movie.h
        src/tcp/client.c
        src/utils/utils.h
        src/utils/net_utils.c
        src/utils/net_utils.h)
