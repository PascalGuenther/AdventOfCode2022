#include "ipuzzle.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"
#include "ctre.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>


namespace AOC::Y2022
{

namespace
{
    enum class Operation : std::uint8_t
    {
        add, multiply, subtract, max
    };

    enum class OperandType : bool
    {
        old, immediate,
    };

    using WorryLevel = std::int64_t;

    struct Monkey final
    {
        std::vector<WorryLevel> items{};
        OperandType operandTypeL{};
        OperandType operandTypeR{};
        Operation operation{};
        WorryLevel immediateL{};
        WorryLevel immediateR{};
        WorryLevel divisor{};
        std::size_t throwTargetIfTrue{};
        std::size_t throwTargetIfFalse{};
    };

    using MonkeyList = std::vector<Monkey>;

#if 0
^Monkey \\d+:$
^  Starting items: (\\d+)(?:, \\d+)*$
^  Operation: new = (old|\\d+) (\\+|\\-|\\*) (old|\\d+)$
^  Test: divisible by (\\d+)$
^    If true: throw to monkey (\\d+)$
^    If false: throw to monkey (\\d+)$
#endif
    AOC_Y2022_CONSTEXPR auto parse_monkeys(std::string_view input) -> MonkeyList
    {
        enum class ParseState
        {
            emptyLine, newMonkey, startingItems, operation, test, ThrowTargetTrue, throwTargetFalse,
        };
        MonkeyList parsedMonkeys;
        ParseState parse = ParseState::newMonkey;
        for (const auto &line : LinesView{input})
        {
            switch (parse)
            {
                default:
                    return MonkeyList{};
                case ParseState::emptyLine:
                {
                    if (!line.empty())
                    {
                        return MonkeyList{};
                    }
                    parse = ParseState::newMonkey;
                    continue;
                }

                case ParseState::newMonkey:
                {
                    if (auto [m, numString] = ctre::match<"^Monkey (\\d+):$">(line); m)
                    {
                        if (parsedMonkeys.size() != parse_number<std::size_t>(numString))
                        {
                            return MonkeyList{};
                        }
                        parsedMonkeys.emplace_back();
                        parse = ParseState::startingItems;
                        continue;
                    }
                    else
                    {
                        return MonkeyList{};
                    }
                }

                case ParseState::startingItems:
                {
                    if (auto [m, itemMatch] = ctre::match<"^  Starting items: (\\d+)(?:, \\d+)*$">(line); m)
                    {
                        auto &monkey = parsedMonkeys.back();
                        auto items = line.substr(18u);
                        while(items.size() > 0u) {
                            const auto delimiterPosition= items.find(", ");
                            const auto item = items.substr(0u, delimiterPosition == items.npos ? items.size() : delimiterPosition);
                            monkey.items.push_back(parse_number<WorryLevel>(item));
                            if (delimiterPosition == items.npos)
                            {
                                break;
                            }
                            items.remove_prefix(delimiterPosition + 2u);
                        }
                        std::reverse(monkey.items.begin(), monkey.items.end());
                        parse = ParseState::operation;
                        continue;
                    }
                    else
                    {
                        return MonkeyList{};
                    }
                }

                case ParseState::operation:
                {
                    if (auto [m, oldL, numL, opMul, opPlus, opMinus, oldR, numR] = ctre::match<"^  Operation: new = (?:(old)|(\\d+)) (?:(\\*)|(\\+)|(-)) (?:(old)|(\\d+))$">(line); m)
                    {
                        auto &monkey = parsedMonkeys.back();
                        if (oldL)
                        {
                            monkey.operandTypeL = OperandType::old;
                        }
                        else if (numL)
                        {
                            monkey.operandTypeL = OperandType::immediate;
                            monkey.immediateL = parse_number<WorryLevel>(numL);
                        }
                        if (oldR)
                        {
                            monkey.operandTypeR = OperandType::old;
                        }
                        else if (numR)
                        {
                            monkey.operandTypeR = OperandType::immediate;
                            monkey.immediateR = parse_number<WorryLevel>(numR);
                        }
                        monkey.operation = opMul ? Operation::multiply : (opPlus ? Operation::add : Operation::subtract);
                        parse = ParseState::test;
                        continue;
                    }
                    else
                    {
                        return MonkeyList{};
                    }
                }

                case ParseState::test:
                {
                    if (auto [m, divisorString] = ctre::match<"^  Test: divisible by (\\d+)$">(line); m)
                    {
                        parsedMonkeys.back().divisor = parse_number<WorryLevel>(divisorString);
                        if (0 == parsedMonkeys.back().divisor)
                        {
                            return MonkeyList{};
                        }
                        parse = ParseState::ThrowTargetTrue;
                        continue;
                    }
                    else
                    {
                        return MonkeyList{};
                    }
                }

                case ParseState::ThrowTargetTrue:
                {
                    if (auto [m, targetString] = ctre::match<"^    If true: throw to monkey (\\d+)$">(line); m)
                    {
                        parsedMonkeys.back().throwTargetIfTrue = parse_number<std::size_t>(targetString);
                        parse = ParseState::throwTargetFalse;
                        continue;
                    }
                    else
                    {
                        return MonkeyList{};
                    }
                }

                case ParseState::throwTargetFalse:
                {
                    if (auto [m, targetString] = ctre::match<"^    If false: throw to monkey (\\d+)$">(line); m)
                    {
                        parsedMonkeys.back().throwTargetIfFalse = parse_number<std::size_t>(targetString);
                        parse = ParseState::emptyLine;
                        continue;
                    }
                    else
                    {
                        return MonkeyList{};
                    }
                }
            }
        }
        if (parse != ParseState::emptyLine)
        {
            return MonkeyList{};
        }
        for (const auto &monkey : parsedMonkeys)
        {
            if ((monkey.divisor == 0) || (monkey.throwTargetIfTrue >= parsedMonkeys.size()) || (monkey.throwTargetIfFalse >= parsedMonkeys.size()))
            {
                return MonkeyList{};
            }
        }
        return parsedMonkeys;
    };

    template<std::size_t ROUNDS>
    AOC_Y2022_CONSTEXPR auto play(const MonkeyList &monkeys, auto reduceWorries) -> IPuzzle::Solution_t
    {
        const auto numberOfMonkeys = monkeys.size();
        if (numberOfMonkeys < 2u)
        {
            return std::monostate{};
        }
        MonkeyList playingMonkeys = monkeys;
        std::vector<std::int64_t> numberOfInspections(numberOfMonkeys);
        for (std::size_t round = 0u; round != ROUNDS; ++round)
        {
            for (std::size_t monkeyIdx = 0u; monkeyIdx != numberOfMonkeys; ++monkeyIdx)
            {
                auto &player = playingMonkeys[monkeyIdx];
                const auto numItems = playingMonkeys[monkeyIdx].items.size();
                numberOfInspections[monkeyIdx] += static_cast<std::int64_t>(numItems);
                if (numItems < 1u)
                {
                    continue;
                }
                for (std::size_t itemIdx = 0u; itemIdx != numItems; ++itemIdx)
                {
                    const auto reverseItem = numItems - itemIdx - 1u;
                    const auto operandL = (player.operandTypeL == OperandType::old) ? player.items[reverseItem] : player.immediateL;
                    const auto operandR = (player.operandTypeR == OperandType::old) ? player.items[reverseItem] : player.immediateR;
                    switch (player.operation)
                    {
                        default:
                        return std::monostate{};
                        break; case Operation::add: player.items[reverseItem] = operandL + operandR;
                        break; case Operation::subtract: player.items[reverseItem] = operandL - operandR;
                        break; case Operation::multiply: player.items[reverseItem] = operandL * operandR;
                    }
                    reduceWorries(player.items[reverseItem]);
                    bool testResult = ((player.items[reverseItem] % player.divisor) == 0);
                    std::size_t throwTarget = testResult ? player.throwTargetIfTrue : player.throwTargetIfFalse;
                    if (throwTarget == monkeyIdx)
                    {
                        return std::monostate{};
                    }
                    auto &target = playingMonkeys[throwTarget].items;
                    using ItemsDifference = typename std::iterator_traits<typename decltype(player.items)::iterator>::difference_type;
                    target.insert(target.end(),
                        std::make_move_iterator(player.items.begin() + static_cast<ItemsDifference>(reverseItem)),
                        std::make_move_iterator(player.items.begin() + static_cast<ItemsDifference>(reverseItem) + 1));
                }
                player.items.erase(player.items.begin(), player.items.end());
            }
        }
        constexpr std::size_t significantItems = 2u;
        std::partial_sort(numberOfInspections.begin(), numberOfInspections.begin() + significantItems, numberOfInspections.end(), std::greater());
        const auto monkeyBusiness = std::accumulate(numberOfInspections.begin(), numberOfInspections.begin() + significantItems, static_cast<std::int64_t>(1u), std::multiplies<std::int64_t>());
        return monkeyBusiness;
    }

    AOC_Y2022_CONSTEXPR auto part_1(const MonkeyList &monkeys) -> IPuzzle::Solution_t
    {
        return play<20u>(monkeys, [](auto &worries) {
            worries /= WorryLevel{3u};
        });
    }

    AOC_Y2022_CONSTEXPR auto part_2(const MonkeyList &monkeys) -> IPuzzle::Solution_t
    {
        const auto modulo = std::accumulate(monkeys.begin(), monkeys.end(), WorryLevel{1}, [](const auto &acc, const auto &monkey) {
            return acc * monkey.divisor;
        });
        return play<10'000u>(monkeys, [modulo](auto &worries) {
            worries %= modulo;
        });
    }

} // namespace


class PuzzleDay11Impl final {
  public:
    AOC_Y2022_CONSTEXPR PuzzleDay11Impl(std::string_view input) : monkeys(parse_monkeys(input)) {}
    MonkeyList monkeys;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(11)

PuzzleDay11::PuzzleDay11(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay11Impl>(input))
{
}
PuzzleDay11::~PuzzleDay11() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay11::Part1()
{
    return part_1(pImpl->monkeys);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay11::Part2()
{
    return part_2(pImpl->monkeys);
}

#if AOC_Y2022_CONSTEXPR_UNIT_TEST
namespace
{

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

consteval bool TestDay11()
{
    PuzzleDay11Impl parsed1{exampleInput1};
    if (10605 != std::get<std::int64_t>(part_1(parsed1.monkeys)))
    {
        return false;
    }

    // This takes way too long
    // if (2713310158 != std::get<std::int64_t>(part_2(parsed1.monkeys)))
    // {
    //     return false;
    // }

    return true;
}
static_assert(TestDay11(), "Wrong results for example input");

} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
