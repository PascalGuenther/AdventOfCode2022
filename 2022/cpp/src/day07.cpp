#include "ipuzzle.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"
#include "ctre.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

namespace AOC::Y2022
{

namespace
{
    class Object
    {
      protected:
        enum class Type
        {
            tree,
            blob,
        };
      protected:
        constexpr Object(Object *parent, Type type): parent(parent), type(type) {}
        constexpr Type get_type() const { return type; }
        virtual constexpr std::size_t get_size(bool recursive) const = 0;
      private:
        Object *parent;
        Type type;
    };

    class Blob final : public Object
    {
      public:
        constexpr Blob(Object *parent, std::size_t blobSize) : Object(parent, Type::blob), blobSize(blobSize) {}
        constexpr std::size_t get_size(bool /* recursive */) const override
        {
            return blobSize;
        }
      private:
        std::size_t blobSize;
    };

    class Tree final : public Object
    {
      private:
        using Entry = std::pair<std::unique_ptr<Object>, std::array<char, 32u>>;
      public:
        constexpr Tree(Object *parent) : Object(parent, Type::tree) {}
        #if 0
        #endif
        constexpr auto add_child(std::unique_ptr<Object> child, std::string_view name)
        {
            decltype(children[0].second) copiedName{};
            std::size_t pos = 0u;
            for (const auto c : name)
            {
                if (pos == (name.size() - 1u))
                {
                    copiedName[pos] = '\0';
                    break;
                }
                copiedName[pos] = c;
            }
            std::vector<std::unique_ptr<Object>> o;
            children.emplace_back(std::move(child), copiedName);
        }

        constexpr std::size_t get_size(bool recursive) const override
        {
            return std::accumulate(children.begin(), children.end(), static_cast<std::size_t>(0u), [recursive](const auto &child, const auto acc) {
                return acc + child.get_size(recursive);
            });
        }
      private:
        std::vector<Entry> children;
        //std::vector<int> children;
    };

} // namespace


class PuzzleDay07Impl final {
  public:
    AOC_Y2022_CONSTEXPR PuzzleDay07Impl(std::string_view) {}
    std::unique_ptr<Object> root;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(07)

PuzzleDay07::PuzzleDay07(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay07Impl>(input))
{
}
PuzzleDay07::~PuzzleDay07() = default;
#if 0
[[nodiscard]] IPuzzle::Solution_t PuzzleDay07::Part1()
{
    if (!pImpl || (pImpl->root == nullptr))
    {
        return std::monostate{};
    }
    return part_1(pImpl->root);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay07::Part2()
{
    if (!pImpl || (pImpl->root == nullptr))
    {
        return std::monostate{};
    }
    return part_2(pImpl->root);
}
#endif
#if 0// AOC_Y2022_CONSTEXPR_UNIT_TEST
namespace
{
constexpr const char * exampleInput =
R"ExampleInput(    [D]    
[N] [C]    
[Z] [M] [P]
 1   2   3 

move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2
)ExampleInput";
consteval bool TestDay07()
{
    std::string_view expectPart1 = "CMZ";
    if (1 != std::get<std::int64_t>(solve_part(exampleInput, false, &expectPart1)))
    {
        return false;
    }

    std::string_view expectPart2 = "MCD";
    return 1 == std::get<std::int64_t>(solve_part(exampleInput, true, &expectPart2));
}
static_assert(TestDay07(), "Wrong results for example input");

} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
