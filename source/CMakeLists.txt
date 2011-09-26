cmake_minimum_required(VERSION 2.4)


if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# whether uses third library
SET( USE_3ND_LIBS true )

IF( USE_3ND_LIBS )

##################################################
# Find extra cmake modules dirs
#
# You can clone all modules from izenesoft.com:/git/cmake.git.
# 
# Copy it to your install dir or change the variable
# EXTRA_CMAKE_MODULES_DIRS.
#
IF(IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/cmake")
  SET(EXTRA_CMAKE_MODULES_DIRS ${EXTRA_CMAKE_MODULES_DIRS} "${CMAKE_INSTALL_PREFIX}/cmake")
ENDIF(IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/cmake")
SET(ENV_EXTRA_CMAKE_MODULES_DIRS $ENV{EXTRA_CMAKE_MODULES_DIRS})
IF(ENV_EXTRA_CMAKE_MODULES_DIRS)
  SET(EXTRA_CMAKE_MODULES_DIRS ${EXTRA_CMAKE_MODULES_DIRS} ${ENV_EXTRA_CMAKE_MODULES_DIRS})
ENDIF(ENV_EXTRA_CMAKE_MODULES_DIRS)
IF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake")
  SET(EXTRA_CMAKE_MODULES_DIRS ${EXTRA_CMAKE_MODULES_DIRS} "${CMAKE_SOURCE_DIR}/cmake")
ENDIF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake")
IF(NOT EXTRA_CMAKE_MODULES_DIRS)
  MESSAGE(FATAL_ERROR
    "Cannot find EXTRA_CMAKE_MODULES_DIRS.
Please clone it from izenesoft.com:/git/cmake.git. Then set EXTRA_CMAKE_MODULES_DIRS (-DEXTRA_CMAKE_MODULES_DIRS=/path/to/cmake)to path of the cloned repository. You also can copy the cloned cmakedirectory to ${CMAKE_INSTALL_PREFIX} or ${CMAKE_SOURCE_DIR}")
ENDIF(NOT EXTRA_CMAKE_MODULES_DIRS)
SET(CMAKE_MODULE_PATH ${EXTRA_CMAKE_MODULES_DIRS})

ENDIF( USE_3ND_LIBS )


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

add_definitions("-Wall -fPIC")# -O3


FIND_PACKAGE(Threads REQUIRED)

SET(Boost_ADDITIONAL_VERSIONS 1.39 1.39.0 1.38 1.38.0 1.37 1.37.0)
FIND_PACKAGE(Boost 1.38 REQUIRED
  COMPONENTS
  system
  program_options
  thread
  regex
  date_time
  serialization
  filesystem
  unit_test_framework
  iostreams
  )

IF( USE_MICROHTTPD )
	MESSAGE( STATUS "libmicrohttpd is the third library used for demo page, you should go to the directory source/libmicrohttpd first, then type: ./configure and make in sequence.")
	SET(MHTTPD_HOME ./libmicrohttpd )
	include_directories( ${MHTTPD_HOME} ${MHTTPD_HOME}/src/include)
	link_directories( ${PROJECT_SOURCE_DIR}/${MHTTPD_HOME}/src/daemon/.libs )
	add_definitions( -DUSE_MICROHTTPD=TRUE )
ENDIF( USE_MICROHTTPD )



include_directories(../include)
include_directories(./corpus)
include_directories(./maxent)
include_directories(./vsynonym)
include_directories(./binsrc)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

SET(LIBS_ME maxent_static)
SET(LIBS_CORPUS corpus)
SET(LIBS_CMAC_STATIC cmac_static)
SET(LIBS_CMAC cmac)
SET(LIBS_TIXML tixml_static)

add_subdirectory(maxent)
add_subdirectory(tixml)
add_subdirectory(src cmac)
add_subdirectory(corpus)
add_subdirectory(binsrc)
add_subdirectory(test)
IF( USE_3ND_LIBS )
    add_subdirectory(boosttest)
ENDIF( USE_3ND_LIBS )


# Install Headers & Knowledges
INSTALL(
  DIRECTORY "${CMAKE_SOURCE_DIR}/../include/"
  DESTINATION include
  FILES_MATCHING PATTERN "*.h"
  )
INSTALL(
  DIRECTORY "${CMAKE_SOURCE_DIR}/../db/icwb/utf8/"
  DESTINATION db/icwb/utf8
  )

# CPACK
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "$ENV{BUILD_NUMBER}")
IF(NOT CPACK_PACKAGE_VERSION_PATCH)
  SET(CPACK_PACKAGE_VERSION_PATCH 0)
ENDIF()

SET(CPACK_PACKAGE_VENDOR "iZENEsoft")
SET(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
SET(CPACK_PACKAGE_NAME "icma")
SET(CPACK_PACKAGE_CONTACT "support@izenesoft.com")
SET(CPACK_GENERATOR TBZ2)
FIND_PROGRAM(DPKG_CMD dpkg)
IF(DPKG_CMD)
  LIST(APPEND CPACK_GENERATOR DEB)
ENDIF()
FIND_PROGRAM(RPMBUILD_EXECUTABLE rpmbuild)
IF(RPMBUILD_EXECUTABLE)
  LIST(APPEND CPACK_GENERATOR RPM)
ENDIF()

SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_IGNORE_FILES
  "/\\\\.git/;\\\\.gitignore$;\\\\.swp$;\\\\.#;/#;~$"
  )

INCLUDE(CPack)
CONFIGURE_FILE(
  "icma-version.h.cmake"
  "${CMAKE_SOURCE_DIR}/../include/icma-version.h"
  @ONLY
  )
