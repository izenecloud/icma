SET(CM_SRC analyzer.cpp knowledge.cpp cma_factory.cpp CPPStringUtils.cc sentence.cpp)

ADD_LIBRARY(cmac STATIC ${CM_SRC})
