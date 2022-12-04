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
        uint32_t overlappingPairs = 0u;
        for_each_line(input_string_view, [&parsingError, &overlappingPairs, &part](const std::string_view &line) -> bool {
            if (line.empty())
            {
                return false;
            }
            using Elf = std::pair<uint32_t, uint32_t>;
            Elf elf1{};
            Elf elf2{};
            if (auto [m, start1, end1, start2, end2] = ctre::match<"^([0-9]+)-([0-9]+),([0-9]+)-([0-9]+)$">(line); m)
            {
                elf1.first = parse_number<decltype(elf1.first)>(start1);
                elf1.second = parse_number<decltype(elf1.second)>(end1);
                elf2.first = parse_number<decltype(elf2.first)>(start2);
                elf2.second = parse_number<decltype(elf2.second)>(end2);
                if ((elf1.second < elf1.first) || (elf2.second < elf2.first))
                {
                    parsingError = true;
                    return false;
                }
                if (elf2.first < elf1.first)
                {
                    std::swap(elf1, elf2);
                }
                if (part == PuzzlePart::one)
                {
                    const bool fullyContained = (elf1.first == elf2.first) || ((elf2.first >= elf1.first) && (elf2.second <= elf1.second));
                    if (fullyContained)
                    {
                        overlappingPairs++;
                    }
                }
                else
                {
                    const bool overlapping = elf2.first <= elf1.second;
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
