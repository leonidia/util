CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(KORA-TESTS)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

FIND_PACKAGE(Boost 1.40.0 REQUIRED)

ADD_DEFINITIONS(-DGTEST_USE_OWN_TR1_TUPLE=1)
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/foreign/gtest ${CMAKE_BINARY_DIR}/gtest)

INCLUDE_DIRECTORIES(
    SYSTEM ${Boost_INCLUDE_DIRS}
    SYSTEM ${CMAKE_SOURCE_DIR}/foreign/gtest/include
    ${CMAKE_SOURCE_DIR}/include
)

LINK_DIRECTORIES(
    ${Boost_LIBRARY_DIRS}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/gtest
)

ADD_EXECUTABLE(kora-tests
    config/config
    config/parser
    dynamic/dynamic
    dynamic/constructor
    dynamic/converter
    dynamic/json
    dynamic/object
    utility/lazy_false
    utility/make_unique
    utility/noexcept
    utility/noreturn
    utility/nullptr
    utility/sfinae
    utility/stdatomic
    utility/type_traits
    utility/underlying_type
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

ADD_TEST(NAME kora-tests COMMAND kora-tests)

ADD_CUSTOM_COMMAND(
    TARGET kora-tests
    COMMENT "Running unit tests"
    POST_BUILD COMMAND ctest --output-on-failure -R kora-tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
