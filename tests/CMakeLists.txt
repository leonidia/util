CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(LEONIDIA-TESTS)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

FIND_PACKAGE(Boost 1.40.0 REQUIRED)

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

ADD_EXECUTABLE(leonidia-tests
    dynamic
)

TARGET_LINK_LIBRARIES(leonidia-tests
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    gtest_main
    gtest
    leonidia-util
)

SET_TARGET_PROPERTIES(leonidia-tests PROPERTIES
    COMPILE_FLAGS "-std=c++0x -W -Wall -Werror -Wextra -pedantic"
)

ADD_TEST(NAME leonidia-tests COMMAND leonidia-tests)
