#include "ipuzzle.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"
#include "ctre.hpp"

#include <algorithm>
#include <bit>
#include <limits>
#include <memory>
#include <numeric>
#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

namespace AOC::Y2022
{

namespace {

    enum class PuzzlePart
    {
        one, two,
    };
    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t solve_part([[maybe_unused]] PuzzlePart part, std::string_view input_string_view)
    {
        bool parsingError = false;
        std::uint32_t overlappingPairs = 0u;
        for_each_line(input_string_view, [&parsingError, &overlappingPairs, &part](const std::string_view &line) -> bool {
            if (line.empty())
            {
                return false;
            }
            if (auto [m, start1, end1, start2, end2] = ctre::match<"^([0-9]+)-([0-9]+),([0-9]+)-([0-9]+)$">(line); m)
            {
                struct Elf {
                    std::uint32_t x;
                    std::uint32_t y;
                };
                std::pair<Elf, Elf> elves{
                    {parse_number<std::uint32_t>(start1), parse_number<std::uint32_t>(end1)},
                    {parse_number<std::uint32_t>(start2), parse_number<std::uint32_t>(end2)},
                };
                if ((elves.first.y < elves.first.x) || (elves.second.y < elves.second.x))
                {
                    parsingError = true;
                    return false;
                }
                if (part == PuzzlePart::one)
                {
                    const bool fullyContained = ((elves.first.x >= elves.second.x) && (elves.first.y <= elves.second.y))
                        || ((elves.second.x >= elves.first.x) && (elves.second.y <= elves.first.y));
                    if (fullyContained)
                    {
                        overlappingPairs++;
                    }
                }
                else
                {
                    // x1           y1
                    //              x2                   y2
                    //
                    //                        x1           y1
                    //   x2                   y2
                    const bool overlapping = (elves.first.y >= elves.second.x) && (elves.first.x <= elves.second.y);
                    if (overlapping)
                    {
                        overlappingPairs++;
                    }
                }
                return true;
            }
            else
            {
                parsingError = true;
                return false;
            }
        });
        if (parsingError)
        {
            return std::monostate{};
        }
        return overlappingPairs;
    }

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_1(std::string_view input)
    {
        return solve_part(PuzzlePart::one, input);
    }

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_2(std::string_view input)
    {
        return solve_part(PuzzlePart::two, input);
    }

}

class PuzzleDay04Impl final {
public:
    AOC_Y2022_CONSTEXPR PuzzleDay04Impl(std::string_view input) : input(input) {}
    std::string input;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(04)

PuzzleDay04::PuzzleDay04(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay04Impl>(input))
{
}
PuzzleDay04::~PuzzleDay04() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay04::Part1()
{
    if (!pImpl || (pImpl->input.size() < 2u))
    {
        return std::monostate{};
    }
    return part_1(pImpl->input);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay04::Part2()
{
    if (!pImpl || (pImpl->input.size() < 2u))
    {
        return std::monostate{};
    }
    return part_2(pImpl->input);
}

#if AOC_Y2022_CONSTEXPR_UNIT_TEST
namespace
{
constexpr const char * exampleInput =
R"ExampleInput(2-4,6-8
2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8
)ExampleInput";
consteval bool TestDay04()
{

    if (2 != std::get<std::int64_t>(part_1(exampleInput)))
    {
        return false;
    }

    return 4 == std::get<std::int64_t>(part_2(exampleInput));
}
static_assert(TestDay04(), "Wrong results for example input");

} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
