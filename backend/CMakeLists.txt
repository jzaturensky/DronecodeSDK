cmake_minimum_required(VERSION 3.1)

set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto/protos)
set(PLUGINS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins)

include(GNUInstallDirs)
#include(cmake/helpers/build_external.cmake)
find_package(OpenSSL REQUIRED)
#message(FATAL_ERROR "blah: ${OpenSSL_FOUND}")
find_package(gRPC REQUIRED)

add_subdirectory(src)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
