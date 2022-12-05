# Advent of Code 2022
[Advent of Code 2022](https://adventofcode.com/2022)

## C++

C++20 solutions for advent of code 2022.

Features:
- low memory usage
- good performance
- puzzle solutions are `constexpr`, thus, they can be validated using `static_assert` (on some modern compilers, at least)
- [Compile-time regular expressions (`ctre`)](https://github.com/hanickadot/compile-time-regular-expressions) for input validation
- unit testing (with example inputs)

### Supported compilers
- Visual Studio 2022
- gcc12 or higher

### Building the application
Either, open the directory directly in VisualStudio,  
or run
- Configure CMake: `cmake -S . -B build`
- Build the project
   - only the main program: `cmake --build build --target aoc-2022-cli`
   - or, build everything: `cmake --build build`
- Run a specific day (Example: Day 01): `build/bin/aoc-2022-cli 01 2022/input/day01.txt`
