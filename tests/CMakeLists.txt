CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(KORA-TESTS)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

FIND_PACKAGE(Boost 1.40.0 REQUIRED)

ADD_DEFINITIONS(-DGTEST_USE_OWN_TR1_TUPLE=1)
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/foreign/gtest ${CMAKE_BINARY_DIR}/gtest)

INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    SYSTEM ${CMAKE_SOURCE_DIR}/foreign/gtest/include
    ${CMAKE_SOURCE_DIR}/include
)

LINK_DIRECTORIES(
    ${Boost_LIBRARY_DIRS}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/gtest
)

ADD_EXECUTABLE(kora-tests
    config
    dynamic/dynamic
    dynamic/dynamic_constructor
    dynamic/json
    dynamic/object
)

TARGET_LINK_LIBRARIES(kora-tests
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    gtest_main
    gtest
    kora-util
)

SET_TARGET_PROPERTIES(kora-tests PROPERTIES
    COMPILE_FLAGS "-std=c++0x -W -Wall -Werror -Wextra -pedantic"
)

# Allow target called "test".
IF(POLICY CMP0037)
    CMAKE_POLICY(PUSH)
    CMAKE_POLICY(SET CMP0037 OLD)
ENDIF()

ADD_CUSTOM_TARGET(test COMMAND kora-tests)

IF(POLICY CMP0037)
    CMAKE_POLICY(POP)
ENDIF()
