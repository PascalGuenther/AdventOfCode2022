cmake_minimum_required(VERSION 3.16)

add_custom_command(OUTPUT {CMAKE_CURRENT_BINARY_DIR}/library.js
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND tsc ${CMAKE_CURRENT_SOURCE_DIR}/src/library.ts --target es5 --module none --outFile ${CMAKE_CURRENT_BINARY_DIR}/library.js
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/library.ts
)

add_executable(aoc-2022-wasm EXCLUDE_FROM_ALL
    {CMAKE_CURRENT_BINARY_DIR}/library.js
)

target_compile_features(aoc-2022-wasm PUBLIC
    cxx_std_23
)
target_link_libraries(aoc-2022-wasm PRIVATE
    aoc_y2022_core
)
target_link_options(aoc-2022-wasm PRIVATE
   --js-library "${CMAKE_CURRENT_BINARY_DIR}/library.js"
   -s EXPORTED_RUNTIME_METHODS=cwrap
)

add_custom_target(aoc-2022-web
    COMMAND tsc
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS aoc-2022-wasm
)
add_custom_command(
    TARGET aoc-2022-web POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:aoc-2022-wasm>/aoc-2022-www/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/www" "$<TARGET_FILE_DIR:aoc-2022-wasm>/aoc-2022-www"
    COMMAND cp "$<TARGET_FILE:aoc-2022-wasm>" "$<TARGET_FILE_DIR:aoc-2022-wasm>/aoc-2022-www/out/"
    COMMAND cp "$<TARGET_FILE_DIR:aoc-2022-wasm>/$<TARGET_FILE_BASE_NAME:aoc-2022-wasm>.wasm" "$<TARGET_FILE_DIR:aoc-2022-wasm>/aoc-2022-www/out/"
    VERBATIM
)

add_subdirectory(src)

default_compile_options(aoc-2022-wasm)
