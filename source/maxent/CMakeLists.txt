SET(ME_COMMON_SRC display.cpp gistrainer.cpp lbfgs_wrapper.c maxentmodel.cpp trainer.cpp mmapfile.c lbfgstrainer.cpp modelfile.cpp maxent_cmdline.c lbfgs.c)

ADD_EXECUTABLE(maxent ${ME_COMMON_SRC} maxent.cpp)
