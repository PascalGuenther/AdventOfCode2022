cmake_minimum_required(VERSION 3.16)

add_subdirectory(aoc_cli)
if(EMSCRIPTEN)
  add_subdirectory(aoc-web)
endif()

