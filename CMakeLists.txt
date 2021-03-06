cmake_minimum_required(VERSION 3.10.0)
project(iceberg-webserver)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.60 REQUIRED COMPONENTS system)
find_package(Boost 1.60 REQUIRED COMPONENTS log)
message(STATUS "Boost version: ${Boost_VERSION}")

include(lib/sqlite3-cmake/CMakeLists.txt)

include_directories(include)

# TODO(!): Update name and srcs
# add_library(example_lib src/example_lib.cc)
add_library(server_lib src/core/server.cc)
add_library(session_lib src/core/session.cc)
add_library(config_parser_lib src/core/config_parser.cc)
add_library(http_request_lib src/core/http_request.cc)
add_library(http_response_lib src/core/http_response.cc)
add_library(echo_handler_lib include/handler.h src/handlers/echo_handler.cc)
add_library(static_handler_lib src/handlers/static_handler.cc)
add_library(not_found_handler_lib src/handlers/not_found_handler.cc)
add_library(bad_request_handler_lib src/handlers/bad_request_handler.cc)
add_library(sleep_handler_lib src/handlers/sleep_handler.cc)
add_library(status_handler_lib src/handlers/status_handler.cc)
add_library(proxy_lib src/handlers/reverse_proxy_handler.cc)
add_library(new_meme_handler_lib src/handlers/new_meme_handler.cc)
add_library(view_meme_handler_lib src/handlers/view_meme_handler.cc)
add_library(list_meme_handler_lib src/handlers/list_meme_handler.cc)
add_library(delete_meme_handler_lib src/handlers/delete_meme_handler.cc)
add_library(handler_manager_lib src/core/handler_manager.cc)
add_library(logger_lib src/core/logger.cc)
add_library(location_lib src/core/location.cc)
add_library(data_store_lib src/data/data_store.cc)
add_library(meme_db src/data/meme_db.cc)
add_library(meme_lib src/data/meme.cc)

target_link_libraries(config_parser_lib echo_handler_lib)
target_link_libraries(http_request_lib Boost::system session_lib)
target_link_libraries(session_lib Boost::system http_request_lib http_response_lib handler_manager_lib
                      logger_lib config_parser_lib data_store_lib)
target_link_libraries(server_lib Boost::system session_lib logger_lib)
target_link_libraries(echo_handler_lib http_request_lib http_response_lib)
target_link_libraries(static_handler_lib http_request_lib http_response_lib config_parser_lib location_lib)
target_link_libraries(not_found_handler_lib http_request_lib http_response_lib config_parser_lib location_lib)
target_link_libraries(status_handler_lib http_request_lib http_response_lib data_store_lib)
target_link_libraries(proxy_lib http_request_lib http_response_lib config_parser_lib)
target_link_libraries(new_meme_handler_lib http_request_lib http_response_lib config_parser_lib meme_db)
target_link_libraries(view_meme_handler_lib http_request_lib http_response_lib config_parser_lib meme_db meme_lib)
target_link_libraries(list_meme_handler_lib http_request_lib http_response_lib config_parser_lib meme_db meme_lib)
target_link_libraries(meme_db sqlite3 ${CMAKE_DL_LIBS})
target_link_libraries(handler_manager_lib echo_handler_lib static_handler_lib not_found_handler_lib
                      bad_request_handler_lib status_handler_lib proxy_lib sleep_handler_lib
                      new_meme_handler_lib view_meme_handler_lib list_meme_handler_lib delete_meme_handler_lib)
target_link_libraries(logger_lib Boost::log)

# TODO(!): Update executable name, srcs, and deps
# add_executable(example_server src/example_server_main.cc)
# target_link_libraries(example_server example_lib Boost::system)
add_executable(server src/core/main.cc)
target_link_libraries(server Boost::system server_lib session_lib config_parser_lib Boost::log logger_lib location_lib)

# TODO(!): Update test executable name, srcs, and deps
# add_executable(example_lib_test tests/example_lib_test.cc)
# target_link_libraries(example_lib_test example_lib gtest_main)
add_executable(config_parser_lib_test tests/config_parser_test.cc)
target_link_libraries(config_parser_lib_test config_parser_lib location_lib gtest_main)

add_executable(http_request_test tests/http_request_test.cc)
target_link_libraries(http_request_test http_request_lib gtest_main)

add_executable(http_response_test tests/http_response_test.cc)
target_link_libraries(http_response_test http_response_lib gtest_main)

add_executable(echo_handler_test tests/echo_handler_test.cc)
target_link_libraries(echo_handler_test http_request_lib http_response_lib echo_handler_lib
config_parser_lib gtest_main)

add_executable(static_handler_test tests/static_handler_test.cc)
target_link_libraries(static_handler_test http_request_lib http_response_lib static_handler_lib
config_parser_lib location_lib gtest_main)

add_executable(status_handler_test tests/status_handler_test.cc)
target_link_libraries(status_handler_test http_request_lib http_response_lib status_handler_lib
config_parser_lib location_lib data_store_lib gtest_main)

add_executable(location_test tests/location_test.cc)
target_link_libraries(location_test config_parser_lib location_lib gtest_main)

add_executable(reverse_proxy_handler_test tests/reverse_proxy_handler_test.cc)
target_link_libraries(reverse_proxy_handler_test http_request_lib http_response_lib static_handler_lib
config_parser_lib location_lib proxy_lib gtest_main)

add_executable(view_meme_handler_test tests/view_meme_handler_test.cc)
target_link_libraries(view_meme_handler_test http_request_lib http_response_lib view_meme_handler_lib meme_db meme_lib
config_parser_lib gtest_main)

add_executable(list_meme_handler_test tests/list_meme_handler_test.cc)
target_link_libraries(list_meme_handler_test http_request_lib http_response_lib list_meme_handler_lib meme_db meme_lib
config_parser_lib gtest_main)

add_executable(delete_meme_handler_test tests/delete_meme_handler_test.cc)
target_link_libraries(delete_meme_handler_test http_request_lib http_response_lib delete_meme_handler_lib meme_db meme_lib
config_parser_lib gtest_main)

# TODO(!): Update with test binary
# gtest_discover_tests(example_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(config_parser_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(http_request_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(http_response_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(echo_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(static_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(status_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(location_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(reverse_proxy_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(view_meme_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(list_meme_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(delete_meme_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)




add_test(
    NAME server_integration_test 
    COMMAND /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/server_integration_test.sh
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_test(
    NAME multithreading_test
    COMMAND /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/multithreading_test.sh
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_test(
    NAME proxy_integration_test
    COMMAND /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/proxy_integration_test.sh
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    
add_test(
    NAME meme_creation_test
    COMMAND /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/meme_creation_test.sh
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# TODO(!): Update with target/test targets
# include(cmake/CodeCoverageReportConfig.cmake)
# generate_coverage_report(TARGETS example_server example_lib TESTS example_lib_test)

include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(
    TARGETS server Boost::system server_lib session_lib config_parser_lib 
        http_request_lib http_response_lib
    TESTS config_parser_lib_test http_request_test http_response_test 
        echo_handler_test static_handler_test location_test status_handler_test reverse_proxy_handler_test view_meme_handler_test list_meme_handler_test delete_meme_handler_test
)

include(cmake/CppLint.cmake)
add_style_check_target(
    TARGET_NAME lint
    SOURCES_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/include/* 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*)

# Copies the server config folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})