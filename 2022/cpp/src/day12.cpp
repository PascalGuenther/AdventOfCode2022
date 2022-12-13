#include "puzzle_common.hpp"
#include "ipuzzle.hpp"
#include "utils.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <memory>
#include <numeric>
#include <set>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace AOC::Y2022
{

namespace
{

    using Coordinate = std::int32_t;
    using Distance = Coordinate;
    using Index = std::size_t;
    using Row = std::vector<char>;
    using Map = std::vector<Row>;
    struct Position
    {
        Coordinate row;
        Coordinate col;
    };

    static constexpr auto invalidIndex = std::numeric_limits<Index>::max();
    static constexpr auto infiniteDistance = std::numeric_limits<Distance>::max();

    [[nodiscard]] constexpr bool operator==(const Position &lhs, const Position &rhs)
    {
        return (lhs.row == rhs.row) && (lhs.col == rhs.col);
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto add(const Position &lhs, const Position &rhs)
    {
        return Position{lhs.row + rhs.row, lhs.col + rhs.col};
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto parse_map(std::string_view input) -> Map
    {
        Map map;
        map.reserve(32);
        bool foundS = false;
        bool foundE = false;
        for (const auto &line : LinesView{input})
        {
            if ((line.empty()) || ((map.size() > 1u) && (map[0].size() != line.size())))
            {
                return Map{};
            }
            auto &row = map.emplace_back();
            if (map.size() > 1u)
            {
                row.reserve(map[0].size());
            }
            for (const char &c :line)
            {
                if (c == 'S')
                {
                    if (foundS)
                    {
                        return Map{};
                    }
                    foundS = true;
                }
                else if (c == 'E')
                {
                    if (foundE)
                    {
                        return Map{};
                    }
                    foundE = true;
                }
                else if ((c < 'a') || (c > 'z'))
                {
                    return Map{};
                }
                row.push_back(c);
            }
        }
        if (!foundS || !foundE || (map.size() > std::numeric_limits<Coordinate>::max()) || (map[0].size() > std::numeric_limits<Coordinate>::max()))
        {
            return Map{};
        }
        return map;
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto get_position(const Map &map, const Index &index)
    {
        const auto numCols = map[0].size();
        return Position { static_cast<Coordinate>(index / numCols), static_cast<Coordinate>(index % numCols) };
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto get_value(const Map &map, const Position &position)
    {
        return map[static_cast<std::size_t>(position.row)][static_cast<std::size_t>(position.col)];
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto get_value(const Map &map, const Index &index)
    {
        return get_value(map, get_position(map, index));
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto is_within_map(const Map &map, const Position &position)
    {
        if ((position.row < 0) || position.col < 0)
        {
            return false;
        }
        const auto numRows = map.size();
        const auto numCols = map[0].size();
        return (static_cast<std::size_t>(position.row) < numRows) && (static_cast<std::size_t>(position.col) < numCols);
    }


    [[nodiscard]] AOC_Y2022_CONSTEXPR auto is_valid_step(const Map &map, const Position &fromPosition, const Position &toPosition)
    {
        if (!is_within_map(map, toPosition))
        {
            return false;
        }
        const auto oldValue = get_value(map, fromPosition);
        const auto newValue = get_value(map, toPosition);
        const auto adjustedOldValue = (oldValue == 'S') ? 'a' : oldValue;
        const auto adjustedNewValue = (newValue == 'E') ? 'z' : newValue;
        const bool managableClimbHeight = adjustedNewValue <= (adjustedOldValue + 1);
        return managableClimbHeight;
    }

    auto dijkstra(const Map &map, const Index &startIndex, const Index &destinationIndex) -> Distance
    {
        const auto start = get_position(map, startIndex);
        const auto destination = get_position(map, destinationIndex);
        const Distance startDistance = 0;
        const auto rows = map.size();
        const auto cols = map[0].size();
        std::vector<std::vector<Distance>> distancesFromStart(rows, std::vector<Distance>(cols, infiniteDistance));
        distancesFromStart[static_cast<std::size_t>(start.row)][static_cast<std::size_t>(start.col)] = startDistance;
        struct Vertex
        {
            [[nodiscard]] constexpr bool operator<(const Vertex &other) const
            {
                if (this->distance != other.distance)
                {
                    return this->distance < other.distance;
                }
                if (this->position.row != other.position.row)
                {
                    return this->position.row < other.position.row;
                }
                return this->position.col < other.position.col;
            }
            Position position;
            Distance distance;
        };
        std::set<Vertex> prioritySearchQueue
        {
            {
                .position = start,
                .distance = startDistance,
            },
        };
        do
        {
            auto vertexHandle = prioritySearchQueue.extract(prioritySearchQueue.begin());
            const auto &vertex = vertexHandle.value();
            const auto shortestPathFound = vertex.position == destination;
            if (shortestPathFound)
            {
                return vertex.distance;
            }
            static constexpr std::array<Position, 4u> directions
            {
                Position{ 0,  1},
                Position{ 0, -1},
                Position{ 1,  0},
                Position{-1,  0},
            };
            for (const auto &direction : directions)
            {
                const auto neighborPosition = add(vertex.position, direction);
                if (!is_valid_step(map, vertex.position, neighborPosition))
                {
                    continue;
                }
                auto &neighborDistance = distancesFromStart[static_cast<std::size_t>(neighborPosition.row)][static_cast<std::size_t>(neighborPosition.col)];
                const auto distanceFromCurrent = vertex.distance + 1;
                const bool betterPathFound = neighborDistance >= distanceFromCurrent;
                if (betterPathFound)
                {
                    prioritySearchQueue.erase({ .position = neighborPosition, .distance = neighborDistance});
                    neighborDistance = distanceFromCurrent;
                    prioritySearchQueue.insert({ .position = neighborPosition, .distance = neighborDistance});
                }
            }
        } while (!prioritySearchQueue.empty());
        return distancesFromStart[static_cast<std::size_t>(destination.row)][static_cast<std::size_t>(destination.col)];
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto find_square_of_height(const Map &map, char searchForHeight, Index startIndex = 0u) -> Index
    {
        const auto numRows = map.size();
        const auto numCols = map[0].size();
        const auto endIndex = numRows * numCols;
        for (auto index = startIndex; index < endIndex; ++index)
        {
            if (get_value(map, index) == searchForHeight)
            {
                return index;
            }
        }
        return invalidIndex;
    }

    [[nodiscard]] auto part_1(const auto &map) -> IPuzzle::Solution_t
    {
        const auto startIndex = find_square_of_height(map, 'S');
        const auto destinationIndex = find_square_of_height(map, 'E');
        if ((startIndex == invalidIndex) || (destinationIndex == invalidIndex))
        {
            return std::monostate{};
        }
        return dijkstra(map, startIndex, destinationIndex);
    }

    [[nodiscard]] auto part_2(const auto &map) -> IPuzzle::Solution_t
    {
        const auto destinationIndex = find_square_of_height(map, 'E');
        auto shortestDistance = infiniteDistance;
        if (destinationIndex == invalidIndex)
        {
            return std::monostate{};
        }
        for (auto startIndex = find_square_of_height(map, 'a'); startIndex != invalidIndex; startIndex = find_square_of_height(map, 'a', startIndex + 1u))
        {
            
            const auto distance = std::min(shortestDistance, dijkstra(map, startIndex, destinationIndex));
            shortestDistance = std::min(distance, shortestDistance);
        }
        if (shortestDistance == infiniteDistance)
        {
            return std::monostate{};
        }
        return shortestDistance;
    }

} // namespace

class PuzzleDay12Impl final {
  public:
    AOC_Y2022_CONSTEXPR PuzzleDay12Impl(std::string_view input) : map(parse_map(input)) {}
    Map map;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(12)

PuzzleDay12::PuzzleDay12(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay12Impl>(input))
{
}
PuzzleDay12::~PuzzleDay12() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay12::Part1()
{
    return part_1(pImpl->map);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay12::Part2()
{
    return part_2(pImpl->map);
}

} // namespace AOC::Y2022
