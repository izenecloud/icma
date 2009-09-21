INCLUDE_DIRECTORIES(.)

EXEC_PROGRAM(cp ${CMAKE_CURRENT_SOURCE_DIR}  ARGS "-r data ${CMAKE_CURRENT_BINARY_DIR}")

#Test ME
ADD_EXECUTABLE(t_display t_display.cc)
TARGET_LINK_LIBRARIES(t_display ${LIBS_ME})

ADD_EXECUTABLE(t_event t_event.cc)
TARGET_LINK_LIBRARIES(t_event ${LIBS_ME})

#ADD_EXECUTABLE(t_lbfgstrainer t_lbfgstrainer.cc)
#TARGET_LINK_LIBRARIES(t_lbfgstrainer ${LIBS_ME})

ADD_EXECUTABLE(t_maxentmodel t_maxentmodel.cc)
TARGET_LINK_LIBRARIES(t_maxentmodel ${LIBS_ME})

ADD_EXECUTABLE(t_gistrainer t_gistrainer.cc)
TARGET_LINK_LIBRARIES(t_gistrainer ${LIBS_ME})

ADD_EXECUTABLE(t_mmapfile t_mmapfile.cc)
TARGET_LINK_LIBRARIES(t_mmapfile ${LIBS_ME})

ADD_EXECUTABLE(t_modelfile t_modelfile.cc)
TARGET_LINK_LIBRARIES(t_modelfile ${LIBS_ME})

ADD_EXECUTABLE(t_itemmap t_itemmap.cc)
TARGET_LINK_LIBRARIES(t_itemmap ${LIBS_ME})

ADD_EXECUTABLE(t_cppstringutils t_cppstringutils.cc)
TARGET_LINK_LIBRARIES(t_cppstringutils ${LIBS_CMAC})

ADD_EXECUTABLE(t_hit_preprocess t_hit_preprocess.cc)
TARGET_LINK_LIBRARIES(t_hit_preprocess ${LIBS_CORPUS})

ADD_EXECUTABLE(t_pfr_preprocess t_pfr_preprocess.cc)
TARGET_LINK_LIBRARIES(t_pfr_preprocess ${LIBS_CORPUS})

ADD_EXECUTABLE(fetch_suspected_word fetch_suspected_word.cc)
TARGET_LINK_LIBRARIES(fetch_suspected_word ${LIBS_CMAC})

ADD_EXECUTABLE(fetch_special_word fetch_special_word.cc)
TARGET_LINK_LIBRARIES(fetch_special_word ${LIBS_CMAC})