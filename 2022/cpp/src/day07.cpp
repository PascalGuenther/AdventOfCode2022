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

    class Directory final
    {
    public:
        using size_type = std::uint32_t;
        class Name
        {
        public:
            AOC_Y2022_CONSTEXPR Name(const std::string_view name)
            {
                if ((name.size() < 1u) || (name.size() >= m_name.size()))
                {
                return;
                }
                std::size_t pos = 0u;
                for (const auto &c : name)
                {
                    m_name[pos++] = c;
                }
            }
            AOC_Y2022_CONSTEXPR const auto & get() const { return m_name; }
        private:
            std::array<char, 32u> m_name{};
        };
    public:
        AOC_Y2022_CONSTEXPR Directory(Directory *parent = nullptr) : parent(parent) {}

        AOC_Y2022_CONSTEXPR auto get_files_size() const -> size_type
        {
            return std::accumulate(files.begin(), files.end(), static_cast<size_type>(0u), [](const auto &acc, const auto &file) {
                return acc + file.second;
            });
        }

        AOC_Y2022_CONSTEXPR auto get_subdirectories_size() const -> size_type
        {
            return std::accumulate(subdirectories.begin(), subdirectories.end(), static_cast<size_type>(0u), [](const auto &acc, const auto &directory){
                return acc + directory.second.get_subdirectories_size() + directory.second.get_files_size();
            });
        }

        AOC_Y2022_CONSTEXPR auto get_size() const -> size_type
        {
            return this->get_subdirectories_size() + this->get_files_size();
        }

        AOC_Y2022_CONSTEXPR void add_subdirectory(std::string_view name)
        {
            subdirectories.emplace_back(Name(name), this);
        }

        AOC_Y2022_CONSTEXPR void add_file(std::string_view name, size_type size)
        {
            files.emplace_back(Name(name), size);
        }

        AOC_Y2022_CONSTEXPR auto get_subdirectory(std::string_view name) -> Directory *
        {
            if (name == "..")
            {
            return parent;
            }
            auto it = std::find_if(subdirectories.begin(), subdirectories.end(), [&name](const auto &directory){
                return directory.first.get().data() == name;
            });
            if (it == subdirectories.end())
            {
                return nullptr;
            }
            return &(it->second);
        }

        AOC_Y2022_CONSTEXPR const auto & get_subdirectories() const
        {
            return subdirectories;
        }
    private:
        Directory *parent;
        std::vector<std::pair<Name, Directory>> subdirectories;
        std::vector<std::pair<Name, size_type>> files;
    };

    AOC_Y2022_CONSTEXPR std::unique_ptr<Directory> parse_file_system(std::string_view input)
    {
        auto root = std::make_unique<Directory>();
        Directory *current = nullptr;
        bool isLsOutput = false;
        for (const auto &line : LinesView(input))
        {
            if (line.size() < 1u)
            {
                return nullptr;
            }
            if (line[0] == '$')
            {
                if (line == "$ ls")
                {
                    isLsOutput = true;
                    continue;
                }
                isLsOutput = false;
            }
            if (line == "$ cd /")
            {
                current = root.get();
                continue;
            }
            if (current == nullptr)
            {
                return nullptr;
            }
            if (auto [cdMatch, subdirectory] = ctre::match<"^\\$ cd ([a-zA-Z0-9]+|[.]{2})$">(line); cdMatch)
            {
                current = current->get_subdirectory(subdirectory);
                if (current == nullptr)
                {
                return nullptr;
                }
                continue;
            }
            if (!isLsOutput || (current == nullptr))
            {
                return nullptr;
            }
            if (auto [dirMatch, dirName] = ctre::match<"^dir ([a-zA-Z0-9]+)$">(line); dirMatch)
            {
                current->add_subdirectory(dirName);
                continue;
            }
            if (auto [fileMatch, fileSize, fileName] = ctre::match<"([0-9]+) ([a-zA-Z0-9\\.]+)$">(line); fileMatch)
            {
                current->add_file(fileName, parse_number<std::uint32_t>(fileSize));
                continue;
            }
            return nullptr;
        }
        return root;
    }

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_1(const Directory &root)
    {
        uint32_t result = 0u;
        const auto countSizeOfSmallDirectories = [&result](const auto &directory, auto &&recursion) -> void {
            constexpr std::uint32_t sizeLimit = 100'000u;
            const auto dirSize = directory.get_size();
            if (dirSize <= sizeLimit)
            {
                result += dirSize;
            }
            for (const auto &[name, subdirectory] : directory.get_subdirectories())
            {
                recursion(subdirectory, recursion);
            }
        };
        countSizeOfSmallDirectories(root, countSizeOfSmallDirectories);
        return result;
    }

    AOC_Y2022_CONSTEXPR IPuzzle::Solution_t part_2(const Directory &root)
    {
        constexpr Directory::size_type totalDiskSpace = 70'000'000u;
        constexpr Directory::size_type updateSize = 30'000'000u;
        const auto usedSpace = root.get_size();
        if (usedSpace > totalDiskSpace)
        {
            return std::monostate{};
        }
        const auto unusedSpace = totalDiskSpace - usedSpace;
        if (unusedSpace >= updateSize)
        {
            return std::monostate{};
        }
        const auto neededSpace = updateSize - unusedSpace;
        Directory::size_type freedUpSpace = root.get_size();
        const auto findDirectoryToDelete = [&freedUpSpace, &neededSpace](const auto &directory, auto &&recursion) -> void {
            const auto dirSize = directory.get_size();
            if (dirSize < neededSpace)
            {
                return;
            }
            freedUpSpace = std::min(freedUpSpace, dirSize);
            for (const auto &[name, subdirectory] : directory.get_subdirectories())
            {
                recursion(subdirectory, recursion);
            }
        };
        findDirectoryToDelete(root, findDirectoryToDelete);
        return freedUpSpace;
    }

} // namespace


class PuzzleDay07Impl final {
  public:
    AOC_Y2022_CONSTEXPR PuzzleDay07Impl(std::string_view input) : root(parse_file_system(input)) {}
    std::unique_ptr<Directory> root;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(07)

PuzzleDay07::PuzzleDay07(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay07Impl>(input))
{
}
PuzzleDay07::~PuzzleDay07() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay07::Part1()
{
    if (!pImpl || (pImpl->root == nullptr))
    {
        return std::monostate{};
    }
    return part_1(*(pImpl->root));
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay07::Part2()
{
    if (!pImpl || (pImpl->root == nullptr))
    {
        return std::monostate{};
    }
    return part_2(*(pImpl->root));
}

#if AOC_Y2022_CONSTEXPR_UNIT_TEST
namespace
{
    static constexpr const char *exampleInput =
R"ExampleInput($ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k
)ExampleInput";

consteval bool TestDay07()
{
    if (95437 != std::get<std::int64_t>(part_1(*parse_file_system(exampleInput))))
    {
        return false;
    }
    return (24933642 == std::get<std::int64_t>(part_2(*parse_file_system(exampleInput))));


    //return 1 == std::get<std::int64_t>((exampleInput, true, &expectPart2);
}
static_assert(TestDay07(), "Wrong results for example input");

} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
