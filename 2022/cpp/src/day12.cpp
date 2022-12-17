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
    static constexpr auto invalidIndex = std::numeric_limits<Index>::max();
    static constexpr auto infiniteDistance = std::numeric_limits<Distance>::max();
    struct Position
    {
        Coordinate row;
        Coordinate col;
    };
    struct Input
    {
        Index indexS{invalidIndex};
        Index indexE{invalidIndex};
        Map map{};
    };


    [[nodiscard]] constexpr bool operator==(const Position &lhs, const Position &rhs)
    {
        return (lhs.row == rhs.row) && (lhs.col == rhs.col);
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto add(const Position &lhs, const Position &rhs)
    {
        return Position{lhs.row + rhs.row, lhs.col + rhs.col};
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto parse_map(std::string_view input) -> Input
    {
        Input parsed;
        auto &map = parsed.map;
        map.reserve(2048);
        Index currentIndex = 0u;
        for (const auto &line : LinesView{input})
        {
            if ((line.empty()) || ((map.size() > 1u) && (map[0].size() != line.size())))
            {
                return Input{};
            }
            auto &row = map.emplace_back();
            if (map.size() > 1u)
            {
                row.reserve(map[0].size());
            }
            for (char c : line)
            {
                if (c == 'S')
                {
                    if (parsed.indexS != invalidIndex)
                    {
                        return Input{};
                    }
                    c = 'a';
                    parsed.indexS = currentIndex;
                }
                else if (c == 'E')
                {
                    if (parsed.indexE != invalidIndex)
                    {
                        return Input{};
                    }
                    c = 'z';
                    parsed.indexE = currentIndex;
                }
                else if ((c < 'a') || (c > 'z'))
                {
                    return Input{};
                }
                row.push_back(c);
                currentIndex++;
            }
        }
        if ((parsed.indexS == invalidIndex) || (parsed.indexE == invalidIndex) || (map.size() > std::numeric_limits<Coordinate>::max()) || (map[0].size() > std::numeric_limits<Coordinate>::max()))
        {
            return Input{};
        }
        return parsed;
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
        const bool managableClimbHeight = newValue <= (oldValue + 1);
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

    [[nodiscard]] auto part_1(const Input &input) -> IPuzzle::Solution_t
    {
        if ((input.indexS == invalidIndex) || (input.indexE == invalidIndex))
        {
            return std::monostate{};
        }
        const auto shortestDistance = dijkstra(input.map, input.indexS, input.indexE);
        if (shortestDistance == infiniteDistance)
        {
            return std::monostate{};
        }
        return shortestDistance;
    }

    [[nodiscard]] auto part_2(const Input &input) -> IPuzzle::Solution_t
    {
        if ((input.indexS == invalidIndex) || (input.indexE == invalidIndex))
        {
            return std::monostate{};
        }
        auto shortestDistance = infiniteDistance;
        for (auto startIndex = find_square_of_height(input.map, 'a'); startIndex != invalidIndex; startIndex = find_square_of_height(input.map, 'a', startIndex + 1u))
        {
            const auto distance = std::min(shortestDistance, dijkstra(input.map, startIndex, input.indexE));
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
    AOC_Y2022_CONSTEXPR PuzzleDay12Impl(std::string_view input) : parsed(parse_map(input)) {}
    Input parsed;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(12)

PuzzleDay12::PuzzleDay12(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay12Impl>(input))
{
}
PuzzleDay12::~PuzzleDay12() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay12::Part1()
{
    return part_1(pImpl->parsed);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay12::Part2()
{
    return part_2(pImpl->parsed);
}

} // namespace AOC::Y2022
