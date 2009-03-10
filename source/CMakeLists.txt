cmake_minimum_required(VERSION 2.6)

#set(CMAKE_VERBOSE_MAKEFILE on)

project(CMA)

add_definitions("-Wall -lf2c -lm")# -O3

include_directories(../include)
include_directories(./include)
include_directories(./corpus)
include_directories(./maxent)


add_subdirectory(src)
add_subdirectory(corpus)
add_subdirectory(maxent)
add_subdirectory(test)
