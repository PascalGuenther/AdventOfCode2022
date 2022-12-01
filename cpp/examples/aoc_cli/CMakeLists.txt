cmake_minimum_required(VERSION 3.16)

add_executable(AOC_Y2022_CLI)
target_compile_features(AOC_Y2022 PUBLIC cxx_std_23)

target_link_libraries(AOC_Y2022_CLI PRIVATE
    AOC_Y2022
)

add_subdirectory(src)

default_compile_options(AOC_Y2022_CLI)
