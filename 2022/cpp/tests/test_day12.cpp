
#include "ipuzzle.hpp"
#include "puzzle_common.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string_view>

namespace AOC::Y2022
{

AOC_Y2022_PUZZLE_CLASS_DECLARATION(12)

namespace
{

class Day12Test : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        pPuzzle1 = std::make_unique<PuzzleDay12>(exampleInput1);
    }

    // void TearDown() override {}
    std::unique_ptr<IPuzzle> pPuzzle1;
static constexpr const char *exampleInput1 =
R"ExampleInput1(Sabqponm
abcryxxl
accszExk
acctuvwj
abdefghi
)ExampleInput1";

};

TEST_F(Day12Test, Part1)
{
    ASSERT_EQ(31, std::get<std::int64_t>(pPuzzle1->Part1()));
}

TEST_F(Day12Test, Part2)
{
    ASSERT_EQ(29, std::get<std::int64_t>(pPuzzle1->Part2()));
}

} // namespace
} // namespace AOC::Y2022
