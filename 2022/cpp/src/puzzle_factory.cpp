#include "puzzle_factory.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"

#define AOC_Y2022_SUPPORTED_PUZZLES_LIST(xMacro)                   \
    xMacro(01)                                                     \
    xMacro(02)                                                     \
    xMacro(03)                                                     \
    xMacro(04)                                                     \
    xMacro(05)                                                     \
    xMacro(06)                                                     \
    xMacro(07)                                                     \
    xMacro(08)                                                     \
    xMacro(09)                                                     \
    xMacro(10)                                                     \

namespace AOC::Y2022
{

AOC_Y2022_SUPPORTED_PUZZLES_LIST(AOC_Y2022_PUZZLE_CLASS_DECLARATION)

std::unique_ptr<IPuzzle> PuzzleFactory::CreatePuzzle(const std::uint8_t game, const std::string_view input)
{
    switch (game)
    {

    default:
        return nullptr;

#define AOC_Y2022_PUZZLE_CASE(day)                                                                                     \
    case parse_number<std::uint8_t>(#day): /* avoid that leading zeroes are interpreted as octal */                    \
        return std::make_unique<PuzzleDay##day>(input);

        AOC_Y2022_SUPPORTED_PUZZLES_LIST(AOC_Y2022_PUZZLE_CASE)

#undef AOC_Y2022_PUZZLE_CASE
    }
}

} // namespace AOC::Y2022
