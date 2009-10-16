cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

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

add_definitions("-Wall -Wno-deprecated -Wno-write-strings -fPIC")# -O3

SET(USE_IZENEWISELIB FALSE)


IF(USE_IZENEWISELIB)

FIND_PACKAGE(Threads REQUIRED)

SET(Boost_ADDITIONAL_VERSIONS 1.39 1.39.0 1.38 1.38.0 1.37 1.37.0)
FIND_PACKAGE(Boost 1.35.0 REQUIRED COMPONENTS thread serialization)


SET(wiselib_FOUND FALSE)
IF(IS_DIRECTORY "$ENV{WISELIB}"
    	AND IS_DIRECTORY "$ENV{WISELIB}/include"
		AND IS_DIRECTORY "$ENV{WISELIB}/lib")
	SET(wiselib_FOUND TRUE)
	SET(wiselib_INCLUDE_DIRS "$ENV{WISELIB}/include")
	SET(wiselib_LIBRARY_DIRS "$ENV{WISELIB}/lib")
ENDIF(IS_DIRECTORY "$ENV{WISELIB}"
		AND IS_DIRECTORY "$ENV{WISELIB}/include"
		AND IS_DIRECTORY "$ENV{WISELIB}/lib")
IF(NOT wiselib_FOUND)
	MESSAGE(FATAL_ERROR "cannot found wiselib, please set env variable WISELIB")
ENDIF(NOT wiselib_FOUND)

SET(izenelib_FOUND FALSE)
IF(IS_DIRECTORY "$ENV{IZENELIB}"
    AND IS_DIRECTORY "$ENV{IZENELIB}/include"
    AND IS_DIRECTORY "$ENV{IZENELIB}/lib")
  SET(izenelib_FOUND TRUE)
  SET(izenelib_INCLUDE_DIRS "$ENV{IZENELIB}/include")
  SET(izenelib_LIBRARY_DIRS "$ENV{IZENELIB}/lib")
ENDIF(IS_DIRECTORY "$ENV{IZENELIB}"
  AND IS_DIRECTORY "$ENV{IZENELIB}/include"
  AND IS_DIRECTORY "$ENV{IZENELIB}/lib")
IF(NOT izenelib_FOUND)
  MESSAGE(FATAL_ERROR "cannot found izenelib, please set env variable IZENELIB")
ENDIF(NOT izenelib_FOUND)


IF(wiselib_FOUND)
    MESSAGE(STATUS "wiselib:")
	MESSAGE(STATUS "  header: ${wiselib_INCLUDE_DIRS}")
ELSE(wiselib_FOUND)
	MESSAGE(STATUS "wiselib: not found")
ENDIF(wiselib_FOUND)

IF(izenelib_FOUND)
	MESSAGE(STATUS "izenelib:")
	MESSAGE(STATUS "  header: ${izenelib_INCLUDE_DIRS}")
ELSE(izenelib_FOUND)
	MESSAGE(STATUS "izenelib: not found")
ENDIF(izenelib_FOUND)


LINK_DIRECTORIES(
    	${wiselib_LIBRARY_DIRS}
    	${izenelib_LIBRARY_DIRS}
	)

SET(LIBS_WISELIB wiselib)
#SET(LIBS_IZENELIB izene)

include_directories(${izenelib_INCLUDE_DIRS})
include_directories(${wiselib_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

ENDIF(USE_IZENEWISELIB)

IF( USE_MICROHTTPD )
	MESSAGE( STATUS "libmicrohttpd is the third library used for demo page, you should go to the directory source/libmicrohttpd first, then type: ./configure and make in sequence.")
	SET(MHTTPD_HOME ./libmicrohttpd )
	include_directories( ${MHTTPD_HOME} ${MHTTPD_HOME}/src/include)
	link_directories( ${PROJECT_SOURCE_DIR}/${MHTTPD_HOME}/src/daemon/.libs )
	add_definitions( -DUSE_MICROHTTPD=TRUE )
ENDIF( USE_MICROHTTPD )

include_directories(../include)
include_directories(./include)
include_directories(./corpus)
include_directories(./maxent)
include_directories(./vsynonym)
include_directories(./tinyxml)


#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)

SET(LIBS_ME maxent)
SET(LIBS_CORPUS corpus)
SET(LIBS_CMAC cmac_static)
SET(LIBS_XML tinyxml)

add_subdirectory(tinyxml)
add_subdirectory(maxent)
add_subdirectory(src cmac)
add_subdirectory(corpus)
add_subdirectory(test)
