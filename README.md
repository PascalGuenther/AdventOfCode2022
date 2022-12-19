# Advent of Code 2022
[Advent of Code 2022](https://adventofcode.com/2022)

## C++
C++20 solutions for advent of code 2022.

### Variants

- [Web application](https://pascalguenther.github.io/AdventOfCode2022/)  
  C++ code is compiled into WebAssembly, which can be executed by the browser. The static webpage is hosted on Github pages. Supports drag&drop for files. Input can be edited and result will update live. You could call it an Advent-of-Code-REPL 😎  
  CMake target: `aoc-2020-web`. Compile with emscripten (see [GitHub action file](/Users/pascal/repos/adventOfCode2022/.github/workflows/webapp.yml) for an example)
- Command line application  
  A standard command line application, which expects the day and the path to the input file as parameters.  
  CMake target: `aoc-2020-web`
  

### Features
- low memory usage
- good performance
- most puzzle solutions are `constexpr`, thus, they can be validated using `static_assert` (on some modern compilers, at least)
  - except for day 12, since it uses `std::set`which is not yet `constexpr`-enabled in C++20
- [Compile-time regular expressions (`ctre`)](https://github.com/hanickadot/compile-time-regular-expressions) for input validation
- unit testing (with example inputs)

### Supported compilers
- Visual Studio 2022
- gcc 12 or higher
- clang 14 or higher

### Building the command line application
Either, open the directory directly in VisualStudio,  
or run
- Configure CMake: `cmake -S . -B build`
- Build the project
   - only the main program: `cmake --build build --target aoc-2022-cli`
   - or, build everything: `cmake --build build`
- Run a specific day (Example: Day 01): `build/bin/aoc-2022-cli 01 2022/input/day01.txt`
