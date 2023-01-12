cmake_minimum_required(VERSION 3.22)

project(
    uint128_t
    VERSION 0.1.0
    DESCRIPTION "An unsigned 128 bit integer type for C++"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRCS uint128_t.cpp)

set(UINT128_LIB uint128)

add_library(${UINT128_LIB} ${SRCS})
set_property(
    TARGET uint128
    PROPERTY POSITION_INDEPENDENT_CODE ON)

if (WITH_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(
    DIRECTORY include/uint128
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT devel)
