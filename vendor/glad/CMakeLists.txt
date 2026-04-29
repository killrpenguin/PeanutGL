cmake_minimum_required(VERSION 3.15...4.1)

project(glad)

set(HEADERS include/KHR/khrplatform.h include/glad/gl.h)

add_library(glad STATIC src/gl.c ${HEADERS})

target_include_directories(glad PUBLIC include)
