
#include "ipuzzle.hpp"
#include "puzzle_common.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string_view>

namespace AOC::Y2022
{

AOC_Y2022_PUZZLE_CLASS_DECLARATION(17)

namespace
{

class Day17Test : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        pPuzzle1 = std::make_unique<PuzzleDay17>(exampleInput1);
    }

    // void TearDown() override {}
    std::unique_ptr<IPuzzle> pPuzzle1;
    static constexpr const char *exampleInput1 = ">>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>";

};

TEST_F(Day17Test, Part1)
{
    ASSERT_EQ(3068, std::get<std::int64_t>(pPuzzle1->Part1()));
}

TEST_F(Day17Test, Part2)
{
    ASSERT_EQ(1'514'285'714'288, std::get<std::int64_t>(pPuzzle1->Part2()));
}

} // namespace
} // namespace AOC::Y2022
