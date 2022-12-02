cmake_minimum_required(VERSION 3.16)

add_executable(aoc_y2022_cli)
target_compile_features(aoc_y2022_cli PUBLIC
    cxx_std_23
)

target_link_libraries(aoc_y2022_cli PRIVATE
    aoc_y2022_core
)

add_subdirectory(src)

default_compile_options(aoc_y2022_cli)
