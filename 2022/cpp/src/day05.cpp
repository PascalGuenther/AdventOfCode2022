#include "ipuzzle.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"
#include "ctre.hpp"

#include <algorithm>
#include <bit>
#include <iterator>
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

    enum class SearchMode {
        crates,
        numbers,
        instructions,
    };
    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t solve_part(std::string_view input_string_view, bool retainOrder, const std::string_view *expectedResult = nullptr)
    {
        const auto lines = LinesView{input_string_view};
        SearchMode mode{SearchMode::crates};
        std::vector<std::vector<char>> stacks(10u);
        for (const auto &line : lines)
        {
            switch (mode)
            {
                case SearchMode::crates:
                {
                    if (line.size() < 3u)
                    {
                        return std::monostate{};
                    }
                    if ((line[1] >= '0') && (line[1] <= '9'))
                    {
                        mode = SearchMode::numbers;
                        continue;
                    }
                    for (std::size_t stackNumber = 0u; stackNumber < stacks.size(); ++stackNumber)
                    {
                        const auto charIndex = (4u * stackNumber) + 1u;
                        if (charIndex >= line.size())
                        {
                            break;
                        }
                        const auto c = line[charIndex];
                        if ((c >= 'A') && (c <= 'Z'))
                        {
                            stacks[stackNumber].push_back(c);
                        }
                        else if (!stacks[stackNumber].empty())
                        {
                            return std::monostate{};
                        }
                    }
                    break;
                }
                case SearchMode::numbers:
                    if(!line.empty())
                    {
                        return std::monostate{};
                    }
                    mode = SearchMode::instructions;
                    for (auto &stack : stacks)
                    {
                        std::reverse(stack.begin(), stack.end());
                    }
                    break;
                case SearchMode::instructions:
                {
                    const auto numOfStacks = stacks.size();
                    if (auto [m, quantityString, sourceString, destinationString] = ctre::match<"^move ([0-9]+) from ([0-9]+) to ([0-9]+)">(line); m)
                    {
                        const auto quantity = parse_number<std::int32_t>(quantityString);
                        const auto source = parse_number<std::size_t>(sourceString) - 1u;
                        const auto destination = parse_number<std::size_t>(destinationString) - 1u;
                        if ((source >= numOfStacks) || (destination >= numOfStacks))
                        {
                            return std::monostate{};
                        }
                        auto &from = stacks[source];
                        auto &to = stacks[destination];
                        if ((quantity < 1) || (from.size() < static_cast<std::size_t>(quantity)))
                        {
                            return std::monostate{};
                        }
                        if (retainOrder)
                        {
                            to.insert(to.end(), std::make_move_iterator(from.end() - quantity), std::make_move_iterator(from.end()));
                        }
                        else
                        {
                            to.insert(to.end(), std::make_move_iterator(from.rbegin()), std::make_move_iterator(from.rbegin() + quantity));
                        }
                        from.erase(from.end() - quantity, from.end());
                    }
                    else
                    {
                        return std::monostate{};
                    }
                    break;
                }
                
                default:
                    return std::monostate{};
            }
        }
        if (expectedResult != nullptr)
        {
            auto remaining = expectedResult->size();
            for (const auto &stack : stacks)
            {
                if (stack.empty())
                {
                    continue;
                }
                if (remaining == 0u)
                {
                    return 0;
                }
                if (expectedResult->at(expectedResult->size() - remaining) != stack.back())
                {
                    return 0;
                }
                --remaining;
            }
            return 1;
        }
        std::string result;
        for (const auto &stack : stacks)
        {
            if (stack.empty())
            {
                continue;
            }
            result += stack.back();
        }
        return result;
    }

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_1(std::string_view input)
    {
        return solve_part(input, false);
    }

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_2(std::string_view input)
    {
        return solve_part(input, true);
    }

}

class PuzzleDay05Impl final {
public:
    AOC_Y2022_CONSTEXPR PuzzleDay05Impl(std::string_view input) : input(input) {}
    std::string input;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(05)

PuzzleDay05::PuzzleDay05(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay05Impl>(input))
{
}
PuzzleDay05::~PuzzleDay05() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay05::Part1()
{
    if (!pImpl || (pImpl->input.size() < 2u))
    {
        return std::monostate{};
    }
    return part_1(pImpl->input);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay05::Part2()
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
R"ExampleInput(    [D]    
[N] [C]    
[Z] [M] [P]
 1   2   3 

move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2
)ExampleInput";
consteval bool TestDay05()
{
    std::string_view expectPart1 = "CMZ";
    if (1 != std::get<std::int64_t>(solve_part(exampleInput, false, &expectPart1)))
    {
        return false;
    }

    std::string_view expectPart2 = "MCD";
    return 1 == std::get<std::int64_t>(solve_part(exampleInput, true, &expectPart2));
}
static_assert(TestDay05(), "Wrong results for example input");

} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
