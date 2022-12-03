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
    using Calories = std::uint32_t;

    AOC_Y2022_CONSTEXPR auto parse_input(std::string_view input_string_view)
    {
        return parse_to_vector_of_numbers<Calories, '\n'>(input_string_view);
    }

    AOC_Y2022_CONSTEXPR auto get_elf_list(const auto &parsedInput)
    {
        std::vector<Calories> elfCalories;
        elfCalories.push_back(0u);
        for(const auto &food_item_calories : parsedInput)
        {
            if (food_item_calories == 0u)
            {
                elfCalories.push_back(0u);
                continue;
            }
            elfCalories.back() += food_item_calories;
        }
        return elfCalories;
    }

    AOC_Y2022_CONSTEXPR auto part_1(const auto &parsedInput)
    {
        const auto elfCalories = get_elf_list(parsedInput);
        return *std::max_element(elfCalories.begin(), elfCalories.end());
    }

    AOC_Y2022_CONSTEXPR auto part_2(const auto &parsedInput)
    {
        auto elfCalories = get_elf_list(parsedInput);
        constexpr auto numberOfTopElves = 3u;
        std::partial_sort(elfCalories.begin(), elfCalories.begin() + numberOfTopElves, elfCalories.end(), std::greater());
        return std::accumulate(elfCalories.begin(), elfCalories.begin() + numberOfTopElves, 0u);
    }
}

class PuzzleDay01Impl final {
public:
    AOC_Y2022_CONSTEXPR PuzzleDay01Impl(std::vector<Calories> &&calories) : caloriesList(calories) {}
    std::vector<Calories> caloriesList;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(01)

PuzzleDay01::PuzzleDay01(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay01Impl>(parse_input(input)))
{
}

PuzzleDay01::~PuzzleDay01() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay01::Part1()
{
    if (!pImpl || (pImpl->caloriesList.size() < 2))
    {
        return std::monostate{};
    }
    return part_1(pImpl->caloriesList);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay01::Part2()
{
    if (!pImpl || (pImpl->caloriesList.size() < 2))
    {
        return std::monostate{};
    }
    return part_2(pImpl->caloriesList);
}

#if AOC_Y2022_CONSTEXPR_UNIT_TEST
namespace
{
constexpr const char * exampleInput = R"DAY01(
1000
2000
3000

4000

5000
6000

7000
8000
9000

10000
)DAY01";
consteval bool TestDay01()
{
    PuzzleDay01Impl parsed{parse_input(exampleInput)};

    if (24000u != part_1(parsed.caloriesList))
    {
        return false;
    }

    return 45000u == part_2(parsed.caloriesList);
}

static_assert(TestDay01(), "");
} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
