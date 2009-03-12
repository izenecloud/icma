cmake_minimum_required(VERSION 2.4)

#set(CMAKE_VERBOSE_MAKEFILE on)

project(CMA-CHEN)

add_definitions("-Wall -Wno-deprecated -Wno-write-strings")# -O3

include_directories(../include)
include_directories(./include)
include_directories(./corpus)
include_directories(./maxent)

SET(LIBS_ME maxent)
SET(LIBS_CORPUS corpus)


add_subdirectory(maxent)
add_subdirectory(src)
add_subdirectory(corpus)
add_subdirectory(test)
