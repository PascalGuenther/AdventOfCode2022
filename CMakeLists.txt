cmake_minimum_required(VERSION 3.16)

project(AdventOfCode VERSION 2022.12.10)

option(AOC_TESTING "Enable testing for AdventOfCode" On)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(AOC_TESTING)
  enable_testing()
endif()

add_subdirectory(2022)
