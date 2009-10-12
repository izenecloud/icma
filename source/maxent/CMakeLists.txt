#ignore lbfgs
#SET(ME_COMMON_SRC display.cpp gistrainer.cpp lbfgs.c maxentmodel.cpp trainer.cpp mmapfile.c lbfgstrainer.cpp modelfile.cpp maxent_cmdline.c lbfgs_algo.c)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)

SET(ME_COMMON_SRC display.cpp gistrainer.cpp maxentmodel.cpp trainer.cpp mmapfile.c modelfile.cpp maxent_cmdline.c )

ADD_LIBRARY(maxent STATIC ${ME_COMMON_SRC})
ADD_EXECUTABLE(maxent_cmd maxent.cpp)


ADD_LIBRARY(maxent_shared SHARED ${ME_COMMON_SRC})
SET_TARGET_PROPERTIES ( maxent_shared PROPERTIES OUTPUT_NAME maxent CLEAN_DIRECT_OUTPUT 1)

TARGET_LINK_LIBRARIES(maxent_cmd maxent)
