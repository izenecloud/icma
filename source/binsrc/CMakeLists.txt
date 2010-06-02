set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)

ADD_EXECUTABLE(cmactrainer TrainerCMD.cc)
TARGET_LINK_LIBRARIES(cmactrainer ${LIBS_CMAC})

ADD_EXECUTABLE(cmacsegger SeggerCMD.cc)
TARGET_LINK_LIBRARIES(cmacsegger ${LIBS_CMAC})

ADD_EXECUTABLE(cmac_run cmac_run.cc)
TARGET_LINK_LIBRARIES(cmac_run ${LIBS_CMAC})

ADD_EXECUTABLE(t_wordbound t_wordbound.cc)
TARGET_LINK_LIBRARIES(t_wordbound ${LIBS_CMAC})

IF( USE_MICROHTTPD )
    ADD_EXECUTABLE(cma_webdemo cma_webdemo.cc)
    TARGET_LINK_LIBRARIES(cma_webdemo pthread microhttpd ${LIBS_CMAC})
ENDIF( USE_MICROHTTPD )