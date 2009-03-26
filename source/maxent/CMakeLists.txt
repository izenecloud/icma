#ignore lbfgs
#SET(ME_COMMON_SRC display.cpp gistrainer.cpp lbfgs.c maxentmodel.cpp trainer.cpp mmapfile.c lbfgstrainer.cpp modelfile.cpp maxent_cmdline.c lbfgs_algo.c)

SET(ME_COMMON_SRC display.cpp gistrainer.cpp maxentmodel.cpp trainer.cpp mmapfile.c modelfile.cpp maxent_cmdline.c )

ADD_LIBRARY(maxent STATIC ${ME_COMMON_SRC})

ADD_EXECUTABLE(maxent_cmd maxent.cpp)

#TARGET_LINK_LIBRARIES(maxent libf2c.so libm.so)

TARGET_LINK_LIBRARIES(maxent_cmd maxent)
