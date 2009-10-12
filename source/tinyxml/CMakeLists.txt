set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)

SET(TINYXML_SRC  tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp )
ADD_LIBRARY(tinyxml STATIC ${TINYXML_SRC})


ADD_LIBRARY(tinyxml_shared SHARED ${TINYXML_SRC})
SET_TARGET_PROPERTIES ( tinyxml_shared PROPERTIES OUTPUT_NAME tinyxml CLEAN_DIRECT_OUTPUT 1)
