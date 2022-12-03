# Advent of Code 2022
[Advent of Code 2022](https://adventofcode.com/2022)

## C++

C++20 solutions for advent of code 2022.

Features:
- low memory usage
- good performance
- many solutions can be validated at compile time using `static_assert` and the given example inputs

### Supported compilers
- Visual Studio 2022
- gcc12 or higher

### Building the application
Either, open the directory directly in VisualStudio,  
or run
- `cmake -S . -B build`
- Build only the main program: `cmake --build build --target aoc-2022-cli`
- or, build everything: `cmake --build build`
- Run a specific day (Example: Day 01): `build/bin/aoc-2022-cli 01 input/day01.txt`
