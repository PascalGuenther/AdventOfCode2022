
#include "ipuzzle.hpp"
#include "puzzle_common.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string_view>

namespace AOC::Y2022
{

AOC_Y2022_PUZZLE_CLASS_DECLARATION(05)

namespace
{

class Day05Test : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        pPuzzle = std::make_unique<PuzzleDay05>(exampleInput);
    }

    // void TearDown() override {}
    std::unique_ptr<IPuzzle> pPuzzle;
    static constexpr const char * exampleInput =
R"ExampleInput(    [D]    
[N] [C]    
[Z] [M] [P]
 1   2   3 

move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2
)ExampleInput";
};

TEST_F(Day05Test, Part1)
{
    ASSERT_EQ(std::get<std::string>(pPuzzle->Part1()), "CMZ");
}

TEST_F(Day05Test, Part2)
{
    ASSERT_EQ(std::get<std::string>(pPuzzle->Part2()), "MCD");
}

} // namespace
} // namespace AOC::Y2022
