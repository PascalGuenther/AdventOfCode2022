#include "ipuzzle.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"

#include <memory>
#include <numeric>
#include <string_view>
#include <utility>
namespace AOC::Y2022
{

namespace {

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t solve_part(std::string_view input_string_view, const std::size_t windowSize)
    {
        for (auto marker = windowSize; marker <= input_string_view.size(); ++marker)
        {
            std::uint32_t bitset{};
            for (std::size_t offset = windowSize; offset > 0u; --offset)
            {
                const auto &c = input_string_view[marker - offset];
                if ((c < 'a') || (c > 'z'))
                {
                    return std::monostate{};
                }
                const auto bit = static_cast<decltype(bitset)>(1u) << static_cast<decltype(c)>(c - 'a');
                const bool alreadyInWindow = (bit & bitset) != 0u;
                if (alreadyInWindow)
                {
                    break;
                }
                if (offset == 1u)
                {
                    return static_cast<std::int64_t>(marker);
                }
                bitset |= bit;
            }
        }
        return std::monostate{};
    }

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_1(std::string_view input)
    {
        return solve_part(input, 4u);
    }

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_2(std::string_view input)
    {
        return solve_part(input, 14u);
    }

}

class PuzzleDay06Impl final {
public:
    AOC_Y2022_CONSTEXPR PuzzleDay06Impl(std::string_view input) : input(input) {}
    std::string input;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(06)

PuzzleDay06::PuzzleDay06(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay06Impl>(input))
{
}
PuzzleDay06::~PuzzleDay06() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay06::Part1()
{
    if (!pImpl || (pImpl->input.size() < 2u))
    {
        return std::monostate{};
    }
    return part_1(pImpl->input);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay06::Part2()
{
    if (!pImpl || (pImpl->input.size() < 2u))
    {
        return std::monostate{};
    }
    return part_2(pImpl->input);
}

#if AOC_Y2022_CONSTEXPR_UNIT_TEST

static_assert( 5 == std::get<std::int64_t>(part_1("bvwbjplbgvbhsrlpgdmjqwftvncz")),"");
static_assert( 6 == std::get<std::int64_t>(part_1("nppdvjthqldpwncqszvftbrmjlhg")),"");
static_assert(10 == std::get<std::int64_t>(part_1("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg")),"");
static_assert(11 == std::get<std::int64_t>(part_1("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw")),"");

static_assert(19 == std::get<std::int64_t>(part_2("mjqjpqmgbljsphdztnvjfqwrcgsmlb")), "");
static_assert(23 == std::get<std::int64_t>(part_2("bvwbjplbgvbhsrlpgdmjqwftvncz")), "");
static_assert(23 == std::get<std::int64_t>(part_2("nppdvjthqldpwncqszvftbrmjlhg")), "");
static_assert(29 == std::get<std::int64_t>(part_2("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg")), "");
static_assert(26 == std::get<std::int64_t>(part_2("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw")), "");

#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
