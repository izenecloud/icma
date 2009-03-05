cmake_minimum_required(VERSION 2.6)

#set(CMAKE_VERBOSE_MAKEFILE on)

project(CMA)

add_definitions("-Wall")# -O3

include_directories(../include)
include_directories(./include)
include_directories(./corpus)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)

add_subdirectory(src)
add_subdirectory(corpus)
add_subdirectory(test)
