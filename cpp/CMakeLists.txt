cmake_minimum_required(VERSION 3.16)

project(AdventOfCode VERSION 2022.12.2)

function(default_compile_options target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wpedantic
            -Wextra
            -Wconversion
            -Wnarrowing
            -Werror
        )
    endif()
endfunction()

add_library(AOC_Y2022 STATIC)
target_compile_features(AOC_Y2022 PUBLIC cxx_std_23)

default_compile_options(AOC_Y2022)

add_subdirectory(include)
add_subdirectory(src)

add_subdirectory(examples)

enable_testing()
add_subdirectory(tests)
