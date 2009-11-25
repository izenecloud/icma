SET(CMAKE_CURRENT_BINARY_DIR cmac)

SET(CM_BASIC_SRC analyzer.cpp CMA_ME_Analyzer.cc CMAPOSTagger.cc sentence.cpp CateStrTokenizer.cc CMA_ME_Factory.cc  CPPStringUtils.cc CMABasicTrainer.cc  CMA_ME_Knowledge.cc knowledge.cpp VTrieCode.cc cma_factory.cpp CMAPOCTagger.cc pos_table.cpp cma_ctype_core.cc cma_ctype.cpp cma_ctype_big5.cpp cma_ctype_gb2312.cpp cma_ctype_gb18030.cpp cma_ctype_utf8.cpp cma_ctype_utf16.cpp cma_wtype.cpp tokenizer.cpp)

#FILE( GLOB_RECURSE SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.cc )

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)


ADD_LIBRARY(cmac_static STATIC ${CM_BASIC_SRC})
TARGET_LINK_LIBRARIES(cmac_static ${LIBS_ME} ${LIBS_XML} 
		#wiselib procmeminfo
	)
SET_TARGET_PROPERTIES ( cmac_static PROPERTIES OUTPUT_NAME cmac CLEAN_DIRECT_OUTPUT 1)

ADD_LIBRARY(cmac_shared SHARED ${CM_BASIC_SRC})
TARGET_LINK_LIBRARIES(cmac_shared maxent_static tinyxml_static
		#wiselib procmeminfo
	)
SET_TARGET_PROPERTIES ( cmac_shared PROPERTIES OUTPUT_NAME cmac CLEAN_DIRECT_OUTPUT 1)

ADD_EXECUTABLE(cmactrainer TrainerCMD.cc)
TARGET_LINK_LIBRARIES(cmactrainer ${LIBS_CMAC})

ADD_EXECUTABLE(cmacsegger SeggerCMD.cc)
TARGET_LINK_LIBRARIES(cmacsegger ${LIBS_CMAC})

ADD_EXECUTABLE(cmac_run cmac_run.cc)
TARGET_LINK_LIBRARIES(cmac_run ${LIBS_CMAC})

ADD_EXECUTABLE(cmac_run2 cmac_run2.cc)
TARGET_LINK_LIBRARIES(cmac_run2 ${LIBS_CMAC} pthread)

IF( USE_MICROHTTPD )
	ADD_EXECUTABLE(cma_webdemo cma_webdemo.cc)
	TARGET_LINK_LIBRARIES(cma_webdemo pthread microhttpd ${LIBS_CMAC})
ENDIF( USE_MICROHTTPD )
