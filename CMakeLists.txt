project(roman)

cmake_minimum_required(VERSION 2.6)

add_definitions(-std=c++14 -Wall -Wextra)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR})

execute_process(COMMAND git submodule update --init --recursive)

include_directories(${CMAKE_SOURCE_DIR}/bandit/)
enable_testing()
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/bandit/cmake/cotire/CMake/")
include(cotire)

add_executable(roman_tests roman_tests.cpp)
cotire(roman_tests)
add_test(RomanTests roman_tests)
