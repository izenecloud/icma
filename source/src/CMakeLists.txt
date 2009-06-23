#SET(CMAKE_CURRENT_BINARY_DIR cmac)

SET(CM_BASIC_SRC analyzer.cpp CMA_ME_Analyzer.cc CMAPOSTagger.cc sentence.cpp CateStrTokenizer.cc CMA_ME_Factory.cc  CPPStringUtils.cc CMABasicTrainer.cc  CMA_ME_Knowledge.cc knowledge.cpp VTrieCode.cc cma_factory.cpp CMAPOCTagger.cc pos_table.cpp cma_ctype.cpp cma_ctype_big5.cpp cma_ctype_gb2312.cpp cma_wtype.cpp tokenizer.cpp)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)


ADD_LIBRARY(${LIBS_CMAC} STATIC ${CM_BASIC_SRC})
TARGET_LINK_LIBRARIES(${LIBS_CMAC} ${LIBS_ME})

ADD_EXECUTABLE(cmactrainer TrainerCMD.cc)
TARGET_LINK_LIBRARIES(cmactrainer ${LIBS_CMAC})

ADD_EXECUTABLE(cmacsegger SeggerCMD.cc)
TARGET_LINK_LIBRARIES(cmacsegger ${LIBS_CMAC})

