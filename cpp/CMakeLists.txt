cmake_minimum_required(VERSION 3.16)

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

add_library(aoc_y2022_core STATIC)
target_compile_features(aoc_y2022_core PUBLIC cxx_std_23)

default_compile_options(aoc_y2022_core)

add_subdirectory(include)
add_subdirectory(src)

add_subdirectory(examples)

enable_testing()
add_subdirectory(tests)
