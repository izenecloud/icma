INCLUDE_DIRECTORIES(../include)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

#ignore lbfgs
#SET(ME_COMMON_SRC display.cpp gistrainer.cpp lbfgs.c maxentmodel.cpp trainer.cpp mmapfile.c lbfgstrainer.cpp modelfile.cpp maxent_cmdline.c lbfgs_algo.c)

#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)

SET(ME_COMMON_SRC display.cpp gistrainer.cpp maxentmodel.cpp trainer.cpp mmapfile.c modelfile.cpp maxent_cmdline.c )

ADD_LIBRARY(maxent_static STATIC ${ME_COMMON_SRC})
SET_TARGET_PROPERTIES ( maxent_static PROPERTIES OUTPUT_NAME maxent CLEAN_DIRECT_OUTPUT 1)

ADD_EXECUTABLE(maxent_cmd maxent.cpp)
TARGET_LINK_LIBRARIES(maxent_cmd maxent_static)

#ADD_LIBRARY(maxent_shared SHARED ${ME_COMMON_SRC})
#SET_TARGET_PROPERTIES ( maxent_shared PROPERTIES OUTPUT_NAME maxent CLEAN_DIRECT_OUTPUT 1)

