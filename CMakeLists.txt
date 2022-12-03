cmake_minimum_required(VERSION 3.16)

project(AdventOfCode VERSION 2022.12.3)

option(AOC_Y2022_TESTING "Enable testing for AdventOfCode 20200" On)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(AOC_Y2022_TESTING)
  enable_testing()
endif()

add_subdirectory(cpp)

