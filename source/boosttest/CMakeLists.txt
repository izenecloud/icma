SET(Boost_ADDITIONAL_VERSIONS 1.39 1.39.0 1.38 1.38.0 1.37 1.37.0)
FIND_PACKAGE(Boost 1.36 COMPONENTS
    unit_test_framework )
    
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../testbin)

# For hudson, Unit-test/Smoke-test:

ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK=1)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/runner/)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ADD_EXECUTABLE(t_icma_core
  t_icma_core.cpp
  runner/Runner.cpp
  )

TARGET_LINK_LIBRARIES(t_icma_core
  ${LIBS_CMAC}
  ${Boost_LIBRARIES}
  )    