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
    struct Position final
    {
        Coordinate row;
        Coordinate col;
    };
    struct Input final
    {
        Index indexS{invalidIndex};
        Index indexE{invalidIndex};
        Map map{};
    };
    struct DistanceMap final
    {
        std::vector<std::vector<Distance>> distancesFromE{};
        Distance distanceFromEtoS{infiniteDistance};
    };

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
        const auto oldValue = get_value(map, toPosition);
        const auto newValue = get_value(map, fromPosition);
        const bool managableClimbHeight = newValue <= (oldValue + 1);
        return managableClimbHeight;
    }

    auto dijkstra(const Input &input) -> DistanceMap
    {
        const auto &map = input.map;
        const auto &destinationIndex = input.indexE;
        const auto &startIndex = input.indexS;
        if ((map.size() < 1u) || (destinationIndex == invalidIndex))
        {
            return {};
        }
        const auto destination = get_position(map, destinationIndex);
        const auto start = (startIndex != invalidIndex)
            ? get_position(map, startIndex)
            : Position{.row = 0, .col = 0};
        constexpr Distance initialDistance = 0;
        const auto rows = map.size();
        const auto cols = map[0].size();
        std::vector<std::vector<Distance>> distancesFromDestination(rows, std::vector<Distance>(cols, infiniteDistance));
        distancesFromDestination[static_cast<std::size_t>(destination.row)][static_cast<std::size_t>(destination.col)] = initialDistance;
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
                .position = destination,
                .distance = initialDistance,
            },
        };
        do
        {
            auto vertexHandle = prioritySearchQueue.extract(prioritySearchQueue.begin());
            const auto &vertex = vertexHandle.value();
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
                auto &neighborDistance = distancesFromDestination[static_cast<std::size_t>(neighborPosition.row)][static_cast<std::size_t>(neighborPosition.col)];
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
        
        const auto distanceFromEtoS = (startIndex == invalidIndex)
            ? infiniteDistance
            : distancesFromDestination[static_cast<std::size_t>(start.row)][static_cast<std::size_t>(start.col)];
        return { .distancesFromE = distancesFromDestination, .distanceFromEtoS = distanceFromEtoS };
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

    [[nodiscard]] auto part_1(const DistanceMap &distances) -> IPuzzle::Solution_t
    {
        if (distances.distanceFromEtoS == infiniteDistance)
        {
            return std::monostate{};
        }
        return distances.distanceFromEtoS;
    }

    [[nodiscard]] auto part_2(const DistanceMap &distances, const Input &parsed) -> IPuzzle::Solution_t
    {
        const auto &map = parsed.map;
        const auto &distancesFromE = distances.distancesFromE;
        if ((parsed.indexS == invalidIndex) || (parsed.indexE == invalidIndex) || (distancesFromE.size() != map.size()))
        {
            return std::monostate{};
        }
        auto shortestDistance = infiniteDistance;
        for (auto startIndex = find_square_of_height(map, 'a'); startIndex != invalidIndex; startIndex = find_square_of_height(map, 'a', startIndex + 1u))
        {
            const auto startPositionCandidate = get_position(map, startIndex);
            const auto distance = distancesFromE[static_cast<std::size_t>(startPositionCandidate.row)][static_cast<std::size_t>(startPositionCandidate.col)];
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
    PuzzleDay12Impl(std::string_view input)
      : parsed(parse_map(input))
      , distancesToE(dijkstra(this->parsed))
    {
    }
    //  the order the class members appear in is important because
    // of cross-referencing during construction
    Input parsed;
    DistanceMap distancesToE;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(12)

PuzzleDay12::PuzzleDay12(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay12Impl>(input))
{
}
PuzzleDay12::~PuzzleDay12() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay12::Part1()
{
    return part_1(pImpl->distancesToE);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay12::Part2()
{
    return part_2(pImpl->distancesToE, pImpl->parsed);
}

} // namespace AOC::Y2022
