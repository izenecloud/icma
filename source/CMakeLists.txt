cmake_minimum_required(VERSION 2.4)

#set(CMAKE_VERBOSE_MAKEFILE on)

# In CMake, compiler flags can be grouped in Configurations
#

# - Set default configurations definition, also add configuration Debugfull and Profile.
#
# Debugfull adds more debug information, and Profile adds profiling information as well.
#

IF (NOT CMAKE_BUILD_TYPE)
  #SET(CMAKE_BUILD_TYPE RelWithDebInfo)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF (NOT CMAKE_BUILD_TYPE)

MESSAGE(STATUS "CMAKE_COMPILER_IS_GNUCXX: " ${CMAKE_COMPILER_IS_GNUCXX})
IF (CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} "Debugfull" "Profile")

  # Select flags.
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
  # Disable assertion macros in RELEASE
  SET(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG")
  SET(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
  SET(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline -DIZS_DEBUG" CACHE STRING "Debugfull build type")
  SET(CMAKE_CXX_FLAGS_PROFILE        "-pg -O2 -DNDEBUG")
  #SET(CMAKE_CXX_FLAGS_PROFILE        "-g3 -fno-inline -ftest-coverage -fprofile-arcs" CACHE STRING "Profile build type")

  MARK_AS_ADVANCED(CMAKE_CXX_FLAGS_DEBUGFULL CMAKE_CXX_FLAGS_PROFILE)
ENDIF (CMAKE_COMPILER_IS_GNUCXX)


project(CMA-CHEN)

add_definitions("-Wall -Wno-deprecated -Wno-write-strings")# -O3

include_directories(../include)
include_directories(./include)
include_directories(./corpus)
include_directories(./maxent)
include_directories(./vsynonym)

#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)


SET(LIBS_ME maxent)
SET(LIBS_CORPUS corpus)
SET(LIBS_CMAC cmac)

add_subdirectory(maxent)
add_subdirectory(src cmac)
add_subdirectory(corpus)
add_subdirectory(test)
