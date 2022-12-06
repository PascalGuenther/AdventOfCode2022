
#include "ipuzzle.hpp"
#include "puzzle_common.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string_view>

namespace AOC::Y2022
{

AOC_Y2022_PUZZLE_CLASS_DECLARATION(06)

namespace
{

class Day06Test : public ::testing::Test
{
};

TEST_F(Day06Test, Part1)
{
    ASSERT_EQ( 5, std::get<std::int64_t>(PuzzleDay06{"bvwbjplbgvbhsrlpgdmjqwftvncz"}.Part1()));
    ASSERT_EQ( 6, std::get<std::int64_t>(PuzzleDay06{"nppdvjthqldpwncqszvftbrmjlhg"}.Part1()));
    ASSERT_EQ(10, std::get<std::int64_t>(PuzzleDay06{"nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg"}.Part1()));
    ASSERT_EQ(11, std::get<std::int64_t>(PuzzleDay06{"zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw"}.Part1()));
}

TEST_F(Day06Test, Part2)
{
    ASSERT_EQ(19, std::get<std::int64_t>(PuzzleDay06{"mjqjpqmgbljsphdztnvjfqwrcgsmlb"}.Part2()));
    ASSERT_EQ(23, std::get<std::int64_t>(PuzzleDay06{"bvwbjplbgvbhsrlpgdmjqwftvncz"}.Part2()));
    ASSERT_EQ(23, std::get<std::int64_t>(PuzzleDay06{"nppdvjthqldpwncqszvftbrmjlhg"}.Part2()));
    ASSERT_EQ(29, std::get<std::int64_t>(PuzzleDay06{"nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg"}.Part2()));
    ASSERT_EQ(26, std::get<std::int64_t>(PuzzleDay06{"zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw"}.Part2()));
}

} // namespace
} // namespace AOC::Y2022
