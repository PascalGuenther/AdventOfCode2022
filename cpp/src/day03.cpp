#include "ipuzzle.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"
#include "ctre.hpp"

#include <algorithm>
#include <bit>
#include <memory>
#include <numeric>
#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

namespace AOC::Y2022
{

namespace {

    using Bitset = std::uint64_t;
    using Rucksack = std::pair<Bitset, Bitset>;

    AOC_Y2022_CONSTEXPR auto parse_input(std::string_view input_string_view)
    {
        std::vector<Rucksack> rucksacks;
        bool parsingError = false;
        for_each_line(input_string_view, [&rucksacks, &parsingError](const std::string_view &line) -> bool {
            if (ctre::match<"^(?:[A-Za-z]{2})+$">(line))
            {
                const auto middle = line.size() / 2u;
                const auto compartment1 = line.substr(0u, middle);
                const auto compartment2 = line.substr(middle);
                const auto createBitset = [](const auto &input) {
                    Bitset bitset = 0u;
                    for (const auto c : input)
                    {
                        if (std::islower(c))
                        {
                            bitset |= 1ull << (c - 'a');
                            continue;
                        }
                        bitset |= 1ull << (c - 'A' + 26);
                    }
                    return bitset;
                };
                rucksacks.emplace_back(createBitset(compartment1), createBitset(compartment2));
                if (std::popcount(rucksacks.back().first & rucksacks.back().second) != 1u)
                {
                    parsingError = true;
                    return false;
                }
                return true;
            }
            parsingError = true;
            return false;
        });
        if (parsingError)
        {
            rucksacks.clear();
        }
        return rucksacks;
    }

    AOC_Y2022_CONSTEXPR auto part_1(const auto &rucksacks)
    {
        std::uint32_t prioSum = 0u;
        for (const auto &rucksack : rucksacks)
        {
            const auto item = rucksack.first & rucksack.second;
            for (auto i = 0u; i != (8u * sizeof(item)); ++i)
            {
                if (item & (1ull << i))
                {
                    prioSum += i + 1u;
                    break;
                }
            }
        }
        return prioSum;
    }

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_2(const auto rucksacks)
    {
        static constexpr auto groupSize = 3u;
        const auto numRucksacks = rucksacks.size();
        if (numRucksacks % groupSize != 0u)
        {
            return std::monostate{};
        }
        std::uint32_t prioSum = 0u;
        for(auto start = 0ull; start < numRucksacks; start = start + groupSize)
        {
            Bitset sharedItem{std::numeric_limits<Bitset>::max()};
            for (auto i = 0ull; i != groupSize; ++i)
            {
                const auto &rucksack = rucksacks[start + i];
                sharedItem &= rucksack.first| rucksack.second;
            }
            if (std::popcount(sharedItem) != 1u)
            {
                return std::monostate{};
            }
            for (auto i = 0u; i != (8u * sizeof(sharedItem)); ++i)
            {
                if (sharedItem & (1ull << i))
                {
                    prioSum += i + 1u;
                    break;
                }
            }
        }
        return prioSum;
    }

}

class PuzzleDay03Impl final {
public:
    AOC_Y2022_CONSTEXPR PuzzleDay03Impl(std::vector<Rucksack> &&rucksacks) : rucksacks(rucksacks) {}
    std::vector<Rucksack> rucksacks;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(03)

PuzzleDay03::PuzzleDay03(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay03Impl>(parse_input(input)))
{
}

PuzzleDay03::~PuzzleDay03() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay03::Part1()
{
    if (!pImpl || (pImpl->rucksacks.size() < 2))
    {
        return std::monostate{};
    }
    return part_1(pImpl->rucksacks);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay03::Part2()
{
    if (!pImpl || (pImpl->rucksacks.size() < 2))
    {
        return std::monostate{};
    }
    return part_2(pImpl->rucksacks);
}

#if AOC_Y2022_CONSTEXPR_UNIT_TEST
namespace
{
constexpr const char * exampleInput =
R"ExampleInput(vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw
)ExampleInput";
consteval bool TestDay03()
{
    PuzzleDay03Impl parsed{parse_input(exampleInput)};

    if (157 != part_1(parsed.rucksacks))
    {
        return false;
    }

    return 70 == std::get<std::int64_t>(part_2(parsed.rucksacks));
}

} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
