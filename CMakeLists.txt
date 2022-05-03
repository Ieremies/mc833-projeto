cmake_minimum_required(VERSION 3.22)

project(mc833_projeto C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(server
        src/catalogo.c
        src/catalogo.h
        src/filme.c
        src/filme.h
        src/generos.c
        src/generos.h
        src/tcp/server.c
        src/net_utils.c
        src/net_utils.h)

add_executable(client
        src/catalogo.c
        src/catalogo.h
        src/filme.c
        src/filme.h
        src/generos.c
        src/generos.h
        src/tcp/client.c
        src/net_utils.c
        src/net_utils.h)
