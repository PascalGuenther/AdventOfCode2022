cmake_minimum_required(VERSION 3.16)

project(AdventOfCode VERSION 2022.12.3)

option(AOC_Y2022_TESTING "Enable testing for AdventOfCode 20200" On)

if(AOC_Y2022_TESTING)
  enable_testing()
endif()

add_subdirectory(cpp)

