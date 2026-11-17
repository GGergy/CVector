cmake_minimum_required(VERSION 4.0)
project(CVector C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -Wpedantic)
add_compile_options(-Werror)

set(SOURCES
        main.c
        cvector.c
)

add_executable(CVector ${SOURCES})
