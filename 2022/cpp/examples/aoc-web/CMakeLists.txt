cmake_minimum_required(VERSION 3.16)

set(AOC_WEBAPP_OUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/aoc-2022-www)

add_custom_command(OUTPUT {CMAKE_CURRENT_BINARY_DIR}/library.js
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND tsc ${CMAKE_CURRENT_SOURCE_DIR}/src/library.ts --target es2015 --module none --outFile ${CMAKE_CURRENT_BINARY_DIR}/library.js
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
    COMMAND tsc ${CMAKE_CURRENT_SOURCE_DIR}/src/aoc.ts --target es2015 --module none --outFile "${AOC_WEBAPP_OUT_DIR}/out/aoc.js"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS aoc-2022-wasm
)

# write git commit hash into HTTP reqeust to prevent browsers from using cached version of our javascript
execute_process(
    COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE AOC_GIT_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/www/index.html.in
    ${AOC_WEBAPP_OUT_DIR}/index.html
)
add_custom_command(
    TARGET aoc-2022-web POST_BUILD
    COMMAND cp "$<TARGET_FILE:aoc-2022-wasm>" "${AOC_WEBAPP_OUT_DIR}/out/"
    COMMAND cp "$<TARGET_FILE_DIR:aoc-2022-wasm>/$<TARGET_FILE_BASE_NAME:aoc-2022-wasm>.wasm" "${AOC_WEBAPP_OUT_DIR}/out/"
    VERBATIM
)

add_subdirectory(src)

default_compile_options(aoc-2022-wasm)
