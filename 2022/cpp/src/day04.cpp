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

    struct Elf {
        std::uint32_t x;
        std::uint32_t y;
    };
    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t solve_part(std::string_view input_string_view, auto &&predicate)
    {
        std::vector<std::pair<Elf, Elf>> elves;
        for (const auto line : LineView{input_string_view})
        {
            if (line.empty())
            {
                break;
            }
            if (auto [m, start1, end1, start2, end2] = ctre::match<"^([0-9]+)-([0-9]+),([0-9]+)-([0-9]+)$">(line); m)
            {
                elves.emplace_back(
                    Elf{parse_number<std::uint32_t>(start1), parse_number<std::uint32_t>(end1)},
                    Elf{parse_number<std::uint32_t>(start2), parse_number<std::uint32_t>(end2)}
                );
                if ((elves.back().first.y < elves.back().first.x) || (elves.back().second.y < elves.back().second.x))
                {
                    return std::monostate{};
                }
            }
            else
            {
                return std::monostate{};
            }
        }
        return std::count_if(elves.begin(), elves.end(), predicate);
    }

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_1(std::string_view input)
    {
        const auto predicate1 = [](const std::pair<Elf, Elf> &e) {
           const bool fullyContained = ((e.first.x >= e.second.x) && (e.first.y <= e.second.y))
               || ((e.second.x >= e.first.x) && (e.second.y <= e.first.y));
           return fullyContained;
        };
        return solve_part(input, predicate1);
    }

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_2(std::string_view input)
    {
        const auto predicate2 = [](const std::pair<Elf, Elf> &e) {
            // x1           y1
            //              x2                   y2
            //
            //                        x1           y1
            //   x2                   y2
            const bool overlapping = (e.first.y >= e.second.x) && (e.first.x <= e.second.y);
            return overlapping;
        };
        return solve_part(input, predicate2);
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
