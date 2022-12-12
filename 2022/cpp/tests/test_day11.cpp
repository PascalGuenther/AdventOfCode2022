
#include "ipuzzle.hpp"
#include "puzzle_common.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string_view>

namespace AOC::Y2022
{

AOC_Y2022_PUZZLE_CLASS_DECLARATION(11)

namespace
{

class Day11Test : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        pPuzzle1 = std::make_unique<PuzzleDay11>(exampleInput1);
    }

    // void TearDown() override {}
    std::unique_ptr<IPuzzle> pPuzzle1;
static constexpr const char *exampleInput1 =
R"ExampleInput1(Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3

Monkey 1:
  Starting items: 54, 65, 75, 74
  Operation: new = old + 6
  Test: divisible by 19
    If true: throw to monkey 2
    If false: throw to monkey 0

Monkey 2:
  Starting items: 79, 60, 97
  Operation: new = old * old
  Test: divisible by 13
    If true: throw to monkey 1
    If false: throw to monkey 3

Monkey 3:
  Starting items: 74
  Operation: new = old + 3
  Test: divisible by 17
    If true: throw to monkey 0
    If false: throw to monkey 1
)ExampleInput1";

};

TEST_F(Day11Test, Part1)
{
    ASSERT_EQ(10605, std::get<std::int64_t>(pPuzzle1->Part1()));
}

TEST_F(Day11Test, Part2)
{
    ASSERT_EQ(2713310158, std::get<std::int64_t>(pPuzzle1->Part2()));
}

} // namespace
} // namespace AOC::Y2022
