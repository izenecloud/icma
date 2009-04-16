cmake_minimum_required(VERSION 2.4)

#set(CMAKE_VERBOSE_MAKEFILE on)

project(CMA-CHEN)

add_definitions("-Wall -Wno-deprecated -Wno-write-strings")# -O3

include_directories(../include)
include_directories(./include)
include_directories(./corpus)
include_directories(./maxent)
include_directories(./vsynonym)

SET(LIBS_ME maxent)
SET(LIBS_CORPUS corpus)
SET(LIBS_CMAC cmac)

add_subdirectory(maxent)
add_subdirectory(src cmac)
add_subdirectory(corpus)
add_subdirectory(test)
