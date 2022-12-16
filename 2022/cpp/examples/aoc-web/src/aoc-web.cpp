#include "aoc_y2022.hpp"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>
#include <variant>

#include <emscripten.h>

extern "C" {
    extern void jsReceivePuzzleSolutionCb(int day, int part, const char *output, int outputLength);
    extern void jsDebugCb(int out);
}

EMSCRIPTEN_KEEPALIVE
extern "C" int solve_puzzle(const int day, const char * const input, const int inputLength)
{
    jsDebugCb(42);
    if ((day < 1) || (day > 25) || (input == nullptr) || (inputLength < 1))
    {
        jsDebugCb(__LINE__);
        jsDebugCb(day);
        jsDebugCb(reinterpret_cast<int>(input));
        jsDebugCb(inputLength);
        return EXIT_FAILURE;
    }

    const std::string_view input_view{input, static_cast<std::size_t>(inputLength)};
    auto pPuzzle = AOC::Y2022::PuzzleFactory::CreatePuzzle(static_cast<std::uint8_t>(day), input_view);
    if (pPuzzle == nullptr)
    {
        jsDebugCb(__LINE__);
        return EXIT_FAILURE;
    }

    const auto solvePart = [day](const auto &result, int part) -> bool {
        std::string resultString;
        if (std::holds_alternative<std::int64_t>(result))
        {
            resultString = std::to_string(std::get<std::int64_t>(result));
        }
        else if (std::holds_alternative<std::string>(result))
        {
            resultString = std::move(std::get<std::string>(result));
        }
        if (resultString.size() < 1)
        {
            jsReceivePuzzleSolutionCb(day, part, nullptr, 0);
            jsDebugCb(__LINE__);
            return false;
        }
        jsReceivePuzzleSolutionCb(day, part, resultString.c_str(), static_cast<int>(resultString.size()));
        return true;
    };
    bool success = solvePart(pPuzzle->Part1(), 1);
    success = solvePart(pPuzzle->Part2(), 2) && success;
    if (!success)
    {
        jsDebugCb(__LINE__);
        return EXIT_FAILURE;
    }
    jsDebugCb(__LINE__);
    return EXIT_SUCCESS;
}

EMSCRIPTEN_KEEPALIVE
extern "C" int has_implementation_for_day(int day)
{
    if ((day < 1) || (day > 25))
    {
        return false;
    }
    return AOC::Y2022::PuzzleFactory::has_implementation_for_day(static_cast<std::uint8_t>(day)) ? 1 : 0;
}
