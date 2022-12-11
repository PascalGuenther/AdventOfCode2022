#include "ipuzzle.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"
#include "ctre.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <numeric>
#include <string_view>
#include <utility>
#include <vector>

namespace AOC::Y2022
{

namespace
{
    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_1(const Vector2D<std::uint8_t> &forest)
    {
        std::uint32_t visibleTrees = 0u;
        const auto width = forest.width();
        const auto height = forest.height();
        if ((width == 0u) || (height == 0u))
        {
            return std::monostate{};
        }
        for (std::size_t row = 0u; row != height; ++row)
        {
            for (std::size_t col = 0u; col != width; ++col)
            {
                if ((row == 0u) || (col == 0u) || (row == (width - 1u)) || (col == (height - 1u)))
                {
                    ++visibleTrees;
                    continue;
                }
                const auto thisHeight = forest(col, row);
                bool notVisible = false;
                for (std::size_t otherCol = 0u; otherCol != col; ++otherCol)
                {
                    if (thisHeight <= forest(otherCol, row))
                    {
                        notVisible = true;
                        break;
                    }
                }
                if(!notVisible)
                {
                    ++visibleTrees;
                    continue;
                }
                notVisible = false;
                for (std::size_t otherCol = col + 1u; otherCol != width; ++otherCol)
                {
                    if (thisHeight <= forest(otherCol, row))
                    {
                        notVisible = true;
                        break;
                    }
                }
                if(!notVisible)
                {
                    ++visibleTrees;
                    continue;
                }
                notVisible = false;
                for (std::size_t otherRow = 0u; otherRow != row; ++otherRow)
                {
                    if (thisHeight <= forest(col, otherRow))
                    {
                        notVisible = true;
                        break;
                    }
                }
                if(!notVisible)
                {
                    ++visibleTrees;
                    continue;
                }
                notVisible = false;
                for (std::size_t otherRow = row + 1u; otherRow != height; ++otherRow)
                {
                    if (thisHeight <= forest(col, otherRow))
                    {
                        notVisible = true;
                        break;
                    }
                }
                if(!notVisible)
                {
                    ++visibleTrees;
                    continue;
                }
            }
        }
        return visibleTrees;
    }

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_2(const Vector2D<std::uint8_t> &forest)
    {
        std::uint32_t highestScenicScore = 0u;
        const auto width = forest.width();
        const auto height = forest.height();
        if ((width == 0u) || (height == 0u))
        {
            return std::monostate{};
        }
        for (std::size_t row = 1u; row != (height - 1u); ++row)
        {
            for (std::size_t col = 1u; col != (width - 1u); ++col)
            {
                const auto thisHeight = forest(col, row);

                std::uint32_t scoreUp = 0u;
                do
                {
                    scoreUp++;
                } while ((scoreUp < row) && (forest(col, row - scoreUp) < thisHeight));

                std::uint32_t scoreLeft = 0u;
                do
                {
                    scoreLeft++;
                } while ((scoreLeft < col) && (forest(col - scoreLeft, row) < thisHeight));

                std::uint32_t scoreDown = 0u;
                do
                {
                    scoreDown++;
                } while (((row + scoreDown + 1u) < height) && (forest(col, row + scoreDown) < thisHeight));

                std::uint32_t scoreRight = 0u;
                do
                {
                    scoreRight++;
                } while (((col + scoreRight + 1u) < width) && (forest(col + scoreRight, row) < thisHeight));

                const auto thisScenicScore = scoreUp * scoreDown * scoreLeft * scoreRight;
                highestScenicScore = std::max(highestScenicScore, thisScenicScore);
            }
        }
        return highestScenicScore;
    }


} // namespace


class PuzzleDay08Impl final {
  public:
    AOC_Y2022_CONSTEXPR PuzzleDay08Impl(std::string_view input) : forest(parse_to_vector_2d(input)) {}
    Vector2D<std::uint8_t> forest;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(08)

PuzzleDay08::PuzzleDay08(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay08Impl>(input))
{
}
PuzzleDay08::~PuzzleDay08() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay08::Part1()
{
    return part_1(pImpl->forest);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay08::Part2()
{
    return part_2(pImpl->forest);
}

#if AOC_Y2022_CONSTEXPR_UNIT_TEST
namespace
{
static constexpr const char *exampleInput =
R"ExampleInput(30373
25512
65332
33549
35390
)ExampleInput";

consteval bool TestDay08()
{
    PuzzleDay08Impl parsed{exampleInput};
    if (21 != std::get<std::int64_t>(part_1(parsed.forest)))
    {
        return false;
    }
    return (8 == std::get<std::int64_t>(part_2(parsed.forest)));
}
static_assert(TestDay08(), "Wrong results for example input");

} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
