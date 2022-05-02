cmake_minimum_required(VERSION 3.22)

project(mc833_projeto C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(mc833_projeto
        src/catalogo.c
        src/catalogo.h
        src/filme.c
        src/filme.h
        src/generos.c
        src/generos.h)
