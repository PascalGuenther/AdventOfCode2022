#include "ipuzzle.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"
#include "ctre.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace AOC::Y2022
{

namespace
{

    enum class Blast : std::uint8_t
    {
        left, right,
    };
    using Blasts = std::vector<Blast>;

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto parse(const std::string_view input) -> Blasts
    {
        Blasts blasts;
        for (const char &c : input)
        {
            switch (c)
            {
                default: return Blasts{};
                break; case '<': blasts.push_back(Blast::left);
                break; case '>': blasts.push_back(Blast::right);
                break; case '\r': case '\n': return blasts;
            }
        }
        return blasts;
    }

    enum class GameMode
    {
        buildTower, detectPeriodicity,
    };
    using Row = std::uint8_t;
    using Chamber = std::vector<Row>;
    using Rock = std::array<Row, 4u>;
    using Displacement = int;
    static constexpr Displacement startDisplacement = 2;
    static constexpr std::array baseRocks
    {
        Rock
        {
            0b0'0011110u,
            0b0'0000000u,
            0b0'0000000u,
            0b0'0000000u,
        },
        Rock
        {
            0b0'0001000u,
            0b0'0011100u,
            0b0'0001000u,
            0b0'0000000u,
        },
        Rock
        {
            0b0'0011100u,
            0b0'0000100u,
            0b0'0000100u,
            0b0'0000000u,
        },
        Rock
        {
            0b0'0010000u,
            0b0'0010000u,
            0b0'0010000u,
            0b0'0010000u,
        },
        Rock
        {
            0b0'0011000u,
            0b0'0011000u,
            0b0'0000000u,
            0b0'0000000u,
        },
    };
    static_assert(std::all_of(baseRocks.begin(), baseRocks.end(), [](const auto &rock) {
        return std::all_of(rock.begin(), rock.end(), [](const auto &row) {
            constexpr Row invalidStartPositions = 0b1'1100001;
            return 0u == (row & invalidStartPositions);
        });
    }), "");

    static constexpr std::size_t maxRockTypes = baseRocks.size();
    static constexpr auto maxDisplacements = []() {
        std::array<Displacement, maxRockTypes> displacements{};
        for (std::size_t rockType = 0u; rockType != maxRockTypes; ++rockType)
        {
            const auto &rock = baseRocks[rockType];
            auto compressed = baseRocks[rockType][0u];
            for (const auto &row : rock)
            {
                compressed |= row;
            }
            for (std::uint8_t displacement = 0u; displacement < (8u * sizeof(rock[0u])); ++displacement)
            {
                if (compressed & (1u << displacement))
                {
                    displacements[rockType] = displacement + startDisplacement;
                    break;
                }
            }
        }
        return displacements;
    }();
    static_assert(std::all_of(maxDisplacements.begin(), maxDisplacements.end(), [](const auto &displacement) {
        return (displacement > 0) && (displacement < static_cast<int>(8 * sizeof(Row)));
    }), "");
    [[nodiscard]] AOC_Y2022_CONSTEXPR auto rock_game(const Blasts &blasts, std::int64_t maxRockCount) -> IPuzzle::Solution_t
    {
        if (blasts.size() < 1u)
        {
            return std::monostate{};
        }


        std::size_t blastIndex = 0u;
        const auto getNextBlast = [&blasts, &blastIndex] () {
            const auto blast = blasts[blastIndex];
            ++blastIndex;
            if (blastIndex >= blasts.size())
            {
                blastIndex = 0u;
            }
            return blast;
        };

        std::int64_t rockCount = 0u;
        std::size_t rockType = 0u;
        const auto createNextRock = [&rockCount, &rockType]() {
            const auto rockAndMaxDisplacement = std::pair {baseRocks[rockType], maxDisplacements[rockType]};
            ++rockCount;
            ++rockType;
            if (rockType >= maxRockTypes)
            {
                rockType = 0u;
            }
            return rockAndMaxDisplacement;
        };
        Chamber chamber{};
        std::vector<std::pair<std::int64_t, std::int64_t>> gameStates(maxRockTypes * blasts.size(), {0u, 0u});
        auto repeatedPatternHeight = std::numeric_limits<std::int64_t>::max();
        const auto repeatedPatternAcceleration = [&chamber, &repeatedPatternHeight, &rockCount, &maxRockCount](auto &savedGame) {
            const auto currentHeight = static_cast<int64_t>(chamber.size());
            if ((currentHeight < 1) || (savedGame.first < 1) || (repeatedPatternHeight != std::numeric_limits<std::int64_t>::max()))
            {
                return;
            }
            const auto growth = currentHeight - savedGame.first;
            if (savedGame.first < growth)
            {
                return;
            }
            using chamberIteratorDiff = typename std::iterator_traits<typename decltype(chamber)::iterator>::difference_type;
            const auto savedGameBegin = chamber.begin() + static_cast<chamberIteratorDiff>(savedGame.first - growth);
            const auto currentGameBegin = chamber.begin() + static_cast<chamberIteratorDiff>(savedGame.first);
            const auto currentGameEnd = chamber.end();
            const bool matchesPreviousPattern = std::equal(currentGameBegin, currentGameEnd, savedGameBegin);
            if (!matchesPreviousPattern)
            {
                return;
            }
            const auto remainingRocks = maxRockCount - rockCount;
            const auto rocksPerCycle = rockCount - savedGame.second;
            decltype(remainingRocks) remainingFullCycles = remainingRocks / rocksPerCycle;
            rockCount += remainingFullCycles * rocksPerCycle;
            repeatedPatternHeight = remainingFullCycles * growth;
        };
        while (rockCount < maxRockCount)
        {
            const auto towerHeightBeforeFall = chamber.size();
            auto rockAndDisplacement = createNextRock();
            auto &rock = rockAndDisplacement.first;
            const auto &maxDisplacement = rockAndDisplacement.second;
            auto currentDisplacement = startDisplacement;
            constexpr std::size_t dropHeight = 3u;
            auto rockBottom = towerHeightBeforeFall + dropHeight;
            while (true)
            {
                const auto detectCollision = [&chamber, towerHeightBeforeFall](const Rock &rock_, const std::size_t rockBottom_) {
                    for (std::size_t idx = 0u; idx != rock_.size(); ++idx) 
                    {
                        std::size_t chamberRow = rockBottom_ + idx;
                        if (chamberRow >= towerHeightBeforeFall)
                        {
                            return false;
                        }
                        if (0u != (chamber[chamberRow] & rock_[idx]))
                        {
                            return true;
                        }
                    }
                    return false;
                };
                const auto blast = getNextBlast();
                const bool wouldTouchWall = (blast == Blast::left)
                    ? (currentDisplacement <= 0)
                    : (currentDisplacement >= maxDisplacement);
                if (!wouldTouchWall)
                {
                    auto pushedRock = rock;
                    for (auto &row : pushedRock)
                    {
                        if (blast == Blast::left)
                        {
                            row <<= 1;
                        }
                        else
                        {
                            row >>= 1;
                        }
                    }
                    
                    const bool wouldBePushedIntoOtherRock = detectCollision(pushedRock, rockBottom);
                    if (!wouldBePushedIntoOtherRock)
                    {
                        rock = pushedRock;
                        currentDisplacement += (blast == Blast::left) ? (-1) : 1;
                    }
                }
                bool canFallWithoutCollision = towerHeightBeforeFall < rockBottom;
                if (canFallWithoutCollision)
                {
                    --rockBottom;
                    continue;
                }
                if (rockBottom == 0)
                {
                    break;
                }
                const auto newBottom = rockBottom - 1u;
                bool wouldFallIntoOtherRock = detectCollision(rock, newBottom);
                if (wouldFallIntoOtherRock)
                {
                    break;
                }
                rockBottom = newBottom;

            }

            for (std::size_t u = 0u; u != rock.size(); ++u)
            {
                if (rock[u] == 0u)
                {
                    break;
                }
                const auto insertPos = rockBottom + u;
                if (chamber.size() > insertPos)
                {
                    chamber[insertPos] |= rock[u];
                }
                else
                {
                    chamber.push_back(rock[u]);
                }
            }
            auto &savedGame = gameStates[(baseRocks.size() * blastIndex) + rockType];
            repeatedPatternAcceleration(savedGame);
            savedGame = {static_cast<std::int64_t>(chamber.size()), rockCount};
        };
        if (repeatedPatternHeight == std::numeric_limits<std::int64_t>::max())
        {
            repeatedPatternHeight = 0;
        }
        return repeatedPatternHeight + static_cast<std::int64_t>(chamber.size());
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto part_1(const Blasts &blasts) -> IPuzzle::Solution_t
    {
       return rock_game(blasts, 2022);
      // return std::monostate{};
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto part_2(const Blasts &blasts) -> IPuzzle::Solution_t
    {
        return rock_game(blasts, 1'000'000'000'000ll);
    }

} // namespace

class PuzzleDay17Impl final {
  public:
    AOC_Y2022_CONSTEXPR PuzzleDay17Impl(std::string_view input) : input(parse(input)) {}
    Blasts input;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(17)


PuzzleDay17::PuzzleDay17(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay17Impl>(input))
{
}
PuzzleDay17::~PuzzleDay17() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay17::Part1()
{
    return part_1(pImpl->input);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay17::Part2()
{
    return part_2(pImpl->input);
}

#if AOC_Y2022_CONSTEXPR_UNIT_TEST
namespace
{

static constexpr const char *exampleInput1 = ">>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>";

consteval bool TestDay17()
{
    PuzzleDay17Impl parsed1{exampleInput1};
    if (3068 != std::get<std::int64_t>(part_1(parsed1.input)))
    {
        return false;
    }

    if (1514285714288 != std::get<std::int64_t>(part_2(parsed1.input)))
    {
        return false;
    }

    return true;
}
static_assert(TestDay17(), "Wrong results for example input");

} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
