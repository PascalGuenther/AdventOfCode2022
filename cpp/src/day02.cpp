#include "ipuzzle.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"

#include <algorithm>
#include <memory>
#include <numeric>
#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

namespace AOC::Y2022
{

namespace {
    enum class Shape : std::uint8_t {
        invalid = 0u,
        rock = 1,
        paper = 2,
        scissors = 3,
    };

    enum class Winner {
        draw,
        first,
        second,
    };

    using Turn = std::pair<Shape, char>;


    AOC_Y2022_CONSTEXPR auto convert_input_shape(char input, bool second)
    {
        if (second)
        {
            input -= 'X' - 'A';
        }
        switch(input)
        {
            case 'A':
                return Shape::rock;
                break;
            case 'B':
                return Shape::paper;
                break;
            case 'C':
                return Shape::scissors;
                break;
            default:
                return Shape::invalid;
        }
    }

    AOC_Y2022_CONSTEXPR auto parse_input(std::string_view input_string_view)
    {
        std::vector<Turn> strategy;
        auto forEachLine = [&strategy](const std::string_view &line) -> bool {
            if (line.empty() || (line.size() < 3u))
            {
                return false;
            }
            enum InputPos {
                they = 0u,
                us = 2u,
            };
            const auto theirs = convert_input_shape(line[InputPos::they], false);
            if ((theirs == Shape::invalid) || (convert_input_shape(line[InputPos::us], true) == Shape::invalid))
            {
                return false;
            }
            strategy.emplace_back(theirs, line[InputPos::us]);
            return true;
        };
        ParseLines(input_string_view, forEachLine, true);
        return strategy;
    }

    AOC_Y2022_CONSTEXPR auto determine_winner(const Shape first, const Shape second)
    {
        
        switch (first)
        {
            case Shape::rock:
            {
                switch (second)
                {
                    case Shape::rock: return Winner::draw;
                    case Shape::paper: return Winner::second;
                    case Shape::scissors: return Winner::first;
                    default: return Winner::draw;
                }
                break;
            }
            case Shape::paper:
            {
                switch (second)
                {
                    case Shape::rock: return Winner::first;
                    case Shape::paper: return Winner::draw;
                    case Shape::scissors: return Winner::second;
                    default: return Winner::draw;
                }
                break;
            }
            case Shape::scissors:
            {
                switch (second)
                {
                    case Shape::rock: return Winner::second;
                    case Shape::paper: return Winner::first;
                    case Shape::scissors: return Winner::draw;
                    default: return Winner::draw;
                }
                break;
            }
            default: return Winner::draw;
        }
    }

    AOC_Y2022_CONSTEXPR auto calculate_score(Winner winner, Shape shape)
    {
        std::uint32_t score = 0u;
        switch (winner)
        {
            case Winner::first:
                break;
            case Winner::second:
                score += 6u;
                break;
            case Winner:: draw:
                score += 3u;
                break;
            default:
                return 0u;
        }
        score += static_cast<std::uint32_t>(shape);
        return score;
    }

    AOC_Y2022_CONSTEXPR auto part_1(const auto &strategyGuide)
    {
        std::uint32_t score = 0u;
        for(const auto &turn : strategyGuide)
        {
            const auto ourShape = convert_input_shape(turn.second, true);
            const auto winner = determine_winner(turn.first, ourShape);
            const auto addToScore = calculate_score(winner, ourShape);
            if (addToScore == 0u) {
                return 0u;
            }
            score += addToScore;
        }
        return score;
    }

    AOC_Y2022_CONSTEXPR auto part_2(const auto &strategyGuide)
    {
        std::uint32_t score = 0u;
        for (const auto &turn : strategyGuide)
        {
            auto winner = Winner::draw;
            switch (turn.second)
            {
            case 'X':
                winner = Winner::first;
                break;
            case 'Y':
                winner = Winner::draw;
                break;
            case 'Z':
                winner = Winner::second;
                break;
            
            default:
                return  0u;
            }

            auto ourShape = Shape::invalid;
            const auto &theirShape = turn.first;
            switch (winner)
            {
                case Winner::draw:
                    ourShape = theirShape;
                    break;
                case Winner::first:
                    switch (theirShape)
                    {
                        case Shape::rock:
                            ourShape = Shape::scissors;
                            break;

                        case Shape::paper:
                            ourShape = Shape::rock;
                            break;
                        
                        case Shape::scissors:
                            ourShape = Shape::paper;
                            break;
                        
                        default:
                            break;
                    }
                    break;
                case Winner::second:
                    switch (theirShape)
                    {
                        case Shape::rock:
                            ourShape = Shape::paper;
                            break;

                        case Shape::paper:
                            ourShape = Shape::scissors;
                            break;
                        
                        case Shape::scissors:
                            ourShape = Shape::rock;
                            break;
                        
                        default:
                            break;
                    }
                    break;
                default:
                    return 0u;
            }
            const auto addToScore = calculate_score(winner, ourShape);
            if (addToScore == 0u) {
                return 0u;
            }
            score += addToScore;
        }
        return score;
    }

}

class PuzzleDay02Impl final {
public:
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
} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
