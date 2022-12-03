#include "ipuzzle.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"
#include "ctre.hpp"

#include <algorithm>
#include <memory>
#include <numeric>
#include <string_view>
#include <utility>
#include <vector>

namespace AOC::Y2022
{

namespace {
    enum class Shape : std::uint8_t
    {
        rock = 0u , paper = 1u, scissors = 2u, max = 3u,
    };

    enum class Winner : std::uint8_t {
        first = 0u, draw = 1u, second = 2u, max = 3u,
    };

    static constexpr std::array winTable = {
        Shape::paper,
        Shape::scissors,
        Shape::rock,
        Shape::max,
    };

    using Turn = std::pair<Shape, Shape>;

    AOC_Y2022_CONSTEXPR auto convert_input_shape(char input, char rockCharacter)
    {

        input -= rockCharacter;
        Shape ret = static_cast<Shape>(input);
        if (ret >= Shape::max)
        {
            return Shape::max;
        }
        return ret;
    }

    AOC_Y2022_CONSTEXPR auto parse_input(std::string_view input_string_view)
    {
        std::vector<Turn> strategy;
        for_each_line(input_string_view, [&strategy](const std::string_view &line) -> bool {
            if (auto [whole, theirs, ours] = ctre::match<"(^[A-C]) ([X-Z])$">(line); whole)
            {
                strategy.emplace_back(convert_input_shape(theirs.to_view()[0], 'A'), convert_input_shape(ours.to_view()[0], 'X'));
                return true;
            }
            strategy.clear();
            return false;
        });
        return strategy;
    }

    AOC_Y2022_CONSTEXPR auto determine_winner(const Shape first, const Shape second)
    {
        if ((first == second) || (first >= Shape::max) || (second >= Shape::max))
        {
            return Winner::draw;
        }
        return (second == winTable[static_cast<size_t>(first)])
            ? Winner::second
            : Winner::first;
    }

    AOC_Y2022_CONSTEXPR auto update_score(auto &score, Winner winner, Shape shape)
    {
        switch (winner)
        {
            case Winner::first:
                break;
            case Winner::second:
                score += 6u;
                break;
            case Winner::draw:
                score += 3u;
                break;
            default:
                return false;
        }
        score += static_cast<std::uint8_t>(shape) + 1u;
        return true;
    }

    AOC_Y2022_CONSTEXPR auto select_shape(const Shape firstShape, Winner desiredOutcome)
    {
        switch (desiredOutcome)
        {
            using enum Winner;
                default:
                case draw:
                    return firstShape;
                case first:
                    return winTable[static_cast<std::size_t>(select_shape(firstShape, Winner::second))];
                case second:
                    return winTable[static_cast<std::size_t>(firstShape)];
        }
    }

    AOC_Y2022_CONSTEXPR auto part_1(const auto &strategyGuide)
    {
        std::uint32_t score = 0u;
        for(const auto &turn : strategyGuide)
        {
            const auto winner = determine_winner(turn.first, turn.second);
            if (!update_score(score, winner, turn.second))
            {
                return 0u;
            }
        }
        return score;
    }

    AOC_Y2022_CONSTEXPR auto part_2(const auto &strategyGuide)
    {
        std::uint32_t score = 0u;
        for (const auto &turn : strategyGuide)
        {
            const auto desiredOutcome = static_cast<Winner>(turn.second);
            const auto &theirShape = turn.first;
            const auto ourShape = select_shape(theirShape, desiredOutcome);
            if (!update_score(score, desiredOutcome, ourShape))
            {
                return 0u;
            }
        }
        return score;
    }

}

class PuzzleDay02Impl final {
public:
    AOC_Y2022_CONSTEXPR PuzzleDay02Impl(std::vector<Turn> &&strategy) : strategyGuide(strategy) {}
    std::vector<Turn> strategyGuide;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(02)

PuzzleDay02::PuzzleDay02(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay02Impl>(parse_input(input)))
{
}

PuzzleDay02::~PuzzleDay02() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay02::Part1()
{
    if (!pImpl || (pImpl->strategyGuide.size() < 2))
    {
        return std::monostate{};
    }
    return part_1(pImpl->strategyGuide);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay02::Part2()
{
    if (!pImpl || (pImpl->strategyGuide.size() < 2))
    {
        return std::monostate{};
    }
    return part_2(pImpl->strategyGuide);
}

#if AOC_Y2022_CONSTEXPR_UNIT_TEST
namespace
{
constexpr const char * exampleInput = R"DAY02(A Y
B X
C Z
)DAY02";
consteval bool TestDay02()
{
    PuzzleDay02Impl parsed{parse_input(exampleInput)};

    if (15 != part_1(parsed.strategyGuide))
    {
        return false;
    }

    return 12 == part_2(parsed.strategyGuide);
}

static_assert(TestDay02(), "");
static_assert(Shape::rock == select_shape(Shape::rock, Winner::draw), "");
static_assert(Shape::paper == select_shape(Shape::paper, Winner::draw), "");
static_assert(Shape::scissors == select_shape(Shape::scissors, Winner::draw), "");
static_assert(Shape::scissors == select_shape(Shape::rock, Winner::first), "");
static_assert(Shape::rock == select_shape(Shape::paper, Winner::first), "");
static_assert(Shape::paper == select_shape(Shape::scissors, Winner::first), "");
static_assert(Shape::paper == select_shape(Shape::rock, Winner::second), "");
static_assert(Shape::scissors == select_shape(Shape::paper, Winner::second), "");
static_assert(Shape::rock == select_shape(Shape::scissors, Winner::second), "");

} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
