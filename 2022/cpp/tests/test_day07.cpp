
#include "ipuzzle.hpp"
#include "puzzle_common.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string_view>

namespace AOC::Y2022
{

AOC_Y2022_PUZZLE_CLASS_DECLARATION(07)

namespace
{

class Day07Test : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        pPuzzle = std::make_unique<PuzzleDay07>(exampleInput);
    }

    // void TearDown() override {}
    std::unique_ptr<IPuzzle> pPuzzle;
    static constexpr const char *exampleInput =
R"ExampleInput($ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k
)ExampleInput";
};

TEST_F(Day07Test, Part1)
{
    ASSERT_EQ(std::get<std::int64_t>(pPuzzle->Part1()), 95437);
}

TEST_F(Day07Test, Part2)
{
    ASSERT_EQ(std::get<std::int64_t>(pPuzzle->Part2()), 24933642);
}

} // namespace
} // namespace AOC::Y2022
