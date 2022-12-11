
#include "ipuzzle.hpp"
#include "puzzle_common.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string_view>

namespace AOC::Y2022
{

AOC_Y2022_PUZZLE_CLASS_DECLARATION(09)

namespace
{

class Day09Test : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        pPuzzle1 = std::make_unique<PuzzleDay09>(exampleInput1);
        pPuzzle2 = std::make_unique<PuzzleDay09>(exampleInput2);
    }

    // void TearDown() override {}
    std::unique_ptr<IPuzzle> pPuzzle1;
    std::unique_ptr<IPuzzle> pPuzzle2;
static constexpr const char *exampleInput1 =
R"ExampleInput1(R 4
U 4
L 3
D 1
R 4
D 1
L 5
R 2
)ExampleInput1";

static constexpr const char *exampleInput2 =
R"ExampleInput2(R 5
U 8
L 8
D 3
R 17
D 10
L 25
U 20
)ExampleInput2";
};

TEST_F(Day09Test, Part1)
{
    ASSERT_EQ(std::get<std::int64_t>(pPuzzle1->Part1()), 13);
}

TEST_F(Day09Test, Part2)
{
    ASSERT_EQ(std::get<std::int64_t>(pPuzzle1->Part2()), 1);
    ASSERT_EQ(std::get<std::int64_t>(pPuzzle2->Part2()), 36);
}

} // namespace
} // namespace AOC::Y2022
