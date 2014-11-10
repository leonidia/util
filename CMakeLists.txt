CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(KORA-UTIL)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

OPTION(ENABLE_TESTING "Enable testing" ON)
OPTION(BUILD_DOC "Generate Doxygen documentation" ON)

FIND_PACKAGE(Boost 1.40.0 REQUIRED)

INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(BEFORE
    ${PROJECT_SOURCE_DIR}/include
    SYSTEM ${PROJECT_SOURCE_DIR}/foreign/rapidjson/include
)

ADD_LIBRARY(kora-util SHARED
    src/dynamic/dynamic
    src/dynamic/error
    src/dynamic/json
    src/dynamic/object
    src/config/config
    src/config/error
    src/config/parser
)

TARGET_LINK_LIBRARIES(kora-util
    ${Boost_LIBRARIES}
)

SET_TARGET_PROPERTIES(kora-util PROPERTIES
    VERSION 1
    COMPILE_FLAGS "-std=c++0x -W -Wall -Werror -pedantic -fvisibility=hidden"
)

IF(NOT KORA_LIBDIR)
    SET(KORA_LIBDIR lib)
ENDIF()

INSTALL(
    TARGETS
        kora-util
    LIBRARY DESTINATION ${KORA_LIBDIR} COMPONENT runtime
    ARCHIVE DESTINATION ${KORA_LIBDIR} COMPONENT development
)

INSTALL(
    DIRECTORY
        include/
    DESTINATION include
    COMPONENT development
)

IF(ENABLE_TESTING)
    ADD_SUBDIRECTORY(tests)
ENDIF()

IF(BUILD_DOC)
    FIND_PACKAGE(Doxygen)

    IF(DOXYGEN_FOUND)
        SET(DOC_TARGET "doc")
        CONFIGURE_FILE(
            doc/Doxyfile.in
            ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
            @ONLY
        )
        ADD_CUSTOM_TARGET(${DOC_TARGET}
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
            COMMENT "Building documentation"
            SOURCE doc/Doxyfile.in
        )
    endif()
endif()
