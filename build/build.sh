#!/bin/bash

# build path
CMA_PROJECT_HOME="`dirname $0`/.."
BUILD_PATH="${CMA_PROJECT_HOME}/build/temp"

# build type
BUILD_TYPE=release

# build system under win32 platform
WIN32_BUILD_SYSTEM="Visual Studio 9 2008"

if [ "$1" = "clean" ]
then 
    if test -d $BUILD_PATH
    then rm -r $BUILD_PATH
    fi

    # remove all lib files
    rm -fr $CMA_PROJECT_HOME/lib/lib*

    # remove all executable files
    rm -fr $CMA_PROJECT_HOME/bin/cma* 
elif [ "$1" = "" ] || [ "$1" = "debug" ] || [ "$1" = "release" ] || [ "$1" = "profile" ]
then
    if [ "$1" != "" ]
    then
        BUILD_TYPE=$1
    fi

    echo "build type:" $BUILD_TYPE

    if [ -d $BUILD_PATH ]
    then
        echo $BUILD_PATH directory exists
    else
        echo make directory $BUILD_PATH
        mkdir $BUILD_PATH
    fi

    cd $BUILD_PATH

    if [ "$2" = "" -o "$2" = "linux" ]
    then
        echo "generating Makefiles for GCC"
        cmake -G "Unix Makefiles" -DCMAKE_COMPILER_IS_GNUCXX=1 -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_CMA_DEBUG_PRINT=0 ../../source
        make all
    elif [ "$2" = "win32" ]
    then
        echo "generating MSVC project (\"$WIN32_BUILD_SYSTEM\" is required)"
        cmake -G "$WIN32_BUILD_SYSTEM" -DCMAKE_COMPILER_IS_MSVC=1 -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_CMA_DEBUG_PRINT=0 ../../source
        if [ $? = 0 ]
        then
            echo "MSVC project file \"temp\CMA.sln\" is generated,"
            echo "please open and build it inside MSVC IDE."
        fi
    else
        echo "unkown platform $2, please use \"linux\" or \"win32\"."
        exit 1
    fi

else
    echo "usage: $0 [debug|release|profile|clean] [linux|win32]"
    exit 1
fi
