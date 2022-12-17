#include "ipuzzle.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"
#include "ctre.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <numeric>
#include <string_view>
#include <utility>
#include <vector>

namespace AOC::Y2022
{

namespace
{

    struct Motion final
    {
        enum class Direction : std::uint8_t
        {
            up, down, left, right, max
        };
        AOC_Y2022_CONSTEXPR Motion(Direction direction, std::uint8_t steps) : direction(direction), steps(steps) {}
        Direction direction;
        std::uint8_t steps;
    };

    using MotionList = std::vector<Motion>;

    AOC_Y2022_CONSTEXPR auto parse_motions(std::string_view input) -> MotionList
    {
        MotionList motions;
        motions.reserve(128u);
        for (const auto &line : LinesView{input})
        {
            if (line.empty())
            {
                return motions;
            }
            if (auto [match, stepsString] = ctre::match<"^[URLD] ([0-9]+)$">(line); match)
            {
                Motion::Direction direction = Motion::Direction::max;
                switch (line[0u])
                {
                    default: direction = Motion::Direction::max;
                    break; case 'U': direction = Motion::Direction::up;
                    break; case 'D': direction = Motion::Direction::down;
                    break; case 'L': direction = Motion::Direction::left;
                    break; case 'R': direction = Motion::Direction::right;
                }
                if (direction == Motion::Direction::max)
                {
                    return MotionList{};
                }
                motions.emplace_back(direction, parse_number<std::uint8_t>(stepsString));
            }
            else
            {
                return MotionList{};
            }
        }
        return motions;
    }

    template<std::size_t RopeLen>
    AOC_Y2022_CONSTEXPR auto simulate_rope(const MotionList &input) -> IPuzzle::Solution_t
    {
        static_assert(RopeLen > 1u, "");
        if (input.size() < 1u)
        {
            return std::monostate{};
        }
        using Position = std::pair<std::int32_t, std::int32_t>;
        struct Knot final
        {
            AOC_Y2022_CONSTEXPR auto move(Motion::Direction direction) -> void
            {
                switch (direction)
                {
                    default: return;
                    return; case Motion::Direction::up: --position.second;
                    return; case Motion::Direction::down: ++position.second;
                    return; case Motion::Direction::left: --position.first;
                    return; case Motion::Direction::right: ++position.first;
                }
                return;
            }

            AOC_Y2022_CONSTEXPR auto follow(const Knot &other) -> void
            {
                const auto &otherPos = other.position;
                const Position distance {otherPos.first - position.first, otherPos.second - position.second};
                const auto adjacent = (AOC::Y2022::abs(distance.first) <= 1)
                  && (AOC::Y2022::abs(distance.second) <= 1);
                if (adjacent)
                {
                    return;
                }
                const auto limitMovement = [](const auto v) {
                    if (v < -1)
                    {
                        return -1;
                    }
                    if (v > 1)
                    {
                        return 1;
                    }
                    return v;
                };
                const Position movement {limitMovement(distance.first), limitMovement(distance.second)};
                const Position updatedPosition {position.first + movement.first, position.second += movement.second};
                position = updatedPosition;
                return;
            }

            Position position{0, 0};
        };
        std::array<Knot, RopeLen> knots{};
        knots.fill(Knot{});
        std::vector<Position> visitedPositions{};
        const auto visit = [&visitedPositions](const Position &position) -> void {
            visitedPositions.push_back(position);
        };
        visit(knots.back().position);
        for (const auto &motion : input)
        {
            for (size_t step = 0u; step < motion.steps; ++step)
            {
                knots.front().move(motion.direction);
                for (std::size_t knotIdx = 1u; knotIdx < knots.size(); ++knotIdx)
                {
                    const auto &predecessor = knots[knotIdx - 1u];
                    knots[knotIdx].follow(predecessor);
                }
                visit(knots.back().position);
            }
        }
        std::sort(visitedPositions.begin(), visitedPositions.end());
        const auto last = std::unique(visitedPositions.begin(), visitedPositions.end());
        return static_cast<std::int64_t>(std::distance(visitedPositions.begin(), last));
    }


    AOC_Y2022_CONSTEXPR auto part_1(const MotionList &input) -> IPuzzle::Solution_t
    {
        return simulate_rope<2u>(input);
    }

    AOC_Y2022_CONSTEXPR auto part_2(const MotionList &input) -> IPuzzle::Solution_t
    {
        return simulate_rope<10u>(input);
    }

} // namespace


class PuzzleDay09Impl final {
  public:
    AOC_Y2022_CONSTEXPR PuzzleDay09Impl(std::string_view input) : motions(parse_motions(input)) {}
    MotionList motions;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(09)

PuzzleDay09::PuzzleDay09(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay09Impl>(input))
{
}
PuzzleDay09::~PuzzleDay09() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay09::Part1()
{
    return part_1(pImpl->motions);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay09::Part2()
{
    return part_2(pImpl->motions);
}

#if AOC_Y2022_CONSTEXPR_UNIT_TEST
namespace
{
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

consteval bool TestDay09()
{
    PuzzleDay09Impl parsed1{exampleInput1};
    PuzzleDay09Impl parsed2{exampleInput2};
    if (13 != std::get<std::int64_t>(part_1(parsed1.motions)))
    {
        return false;
    }
    if (1 != std::get<std::int64_t>(part_2(parsed1.motions)))
    {
        return false;
    }
    return (36 == std::get<std::int64_t>(part_2(parsed2.motions)));
}
static_assert(TestDay09(), "Wrong results for example input");

} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
