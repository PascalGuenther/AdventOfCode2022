cmake_minimum_required(VERSION 3.16)

include(FetchContent)
FetchContent_Declare(
  CompileTimeRegularExpressions
  GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
  GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(CompileTimeRegularExpressions)

set(AOC_Y2022_DAYS_LIST "01")
set(AOC_Y2022_DAYS_LIST ${AOC_Y2022_DAYS_LIST} PARENT_SCOPE)
set(AOC_Y2022_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)


target_sources(AOC_Y2022  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/puzzle_factory.cpp
)

foreach(DAY ${AOC_Y2022_DAYS_LIST})
    target_sources(AOC_Y2022 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/day${DAY}.cpp
    )
endforeach()
