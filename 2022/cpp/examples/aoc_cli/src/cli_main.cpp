#include "aoc_y2022.hpp"

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <variant>

int main(const int argc, const char *const argv[])
{
    std::cout << "==Advent of Code 2022==\n";
    if (argc < 3)
    {
        const char *const executableName = ((argc > 0) && (argv != nullptr) && (argv[0u] != nullptr)) ? argv[0u] : "";
        std::cout << "usage: " << executableName << " <day> <inputfile>\n";
        std::cout << "example: " << executableName << " 1 input01.txt\n";
        return (argc == 1) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    const auto day = [&argv]() -> std::uint8_t {
        if (argv == nullptr)
        {
            return 0;
        }
        const auto dayTmp = std::strtol(argv[1u], nullptr, 0);
        if ((dayTmp < 1l) || (dayTmp > 25l))
        {
            std::cerr << "error: please select a day from 1 and 24\n";
            return 0;
        }
        return static_cast<std::uint8_t>(dayTmp);
    }();
    if (day == 0u)
    {
        return EXIT_FAILURE;
    }

    std::ifstream ifs(argv[2u]);
    if (!ifs.is_open())
    {
        std::cerr << "error: cannot open input file\n";
        return EXIT_FAILURE;
    }
    const std::string input((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();
    if (input.empty())
    {
        std::cerr << "error: empty input\n";
        return EXIT_FAILURE;
    }

    auto pPuzzle = AOC::Y2022::PuzzleFactory::CreatePuzzle(day, input);
    if (pPuzzle == nullptr)
    {
        std::cerr << "sorry, no implementation for day " << unsigned(day) << '\n';
        return EXIT_FAILURE;
    }

    const auto solvePart = [](const auto &result, bool bPart2) -> bool {
        const bool emptyResult = std::holds_alternative<std::monostate>(result);
        if (!emptyResult)
        {
            std::cout << "result of part " << (bPart2 ? 2 : 1) << ":\n";
            if (std::holds_alternative<std::int64_t>(result))
            {
                std::cout << std::get<std::int64_t>(result) << '\n';
                return true;
            }
            if (std::holds_alternative<std::string>(result))
            {
                std::cout << std::get<std::string>(result) << '\n';
                return true;
            }
        }
        std::cerr << "sorry! failed to solve part " << (bPart2 ? 2 : 1) << '\n';
        return false;
    };
    if (solvePart(pPuzzle->Part1(), false) && solvePart(pPuzzle->Part2(), true))
    {
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}
