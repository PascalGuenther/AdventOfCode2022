#if !defined(AOC_Y2022_UTILS_HPP)
#define AOC_Y2022_UTILS_HPP (1)

#include "../include/types.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string_view>
#include <type_traits>
#include <vector>

namespace AOC::Y2022
{

/*!
 * \brief Enum class representing the behavior of for_each_line when an empty line is encountered.
 *        break_if_empty: for_each_line will return false when it encounters an empty line.
 *        continue_if_empty: for_each_line will continue processing even if it encounters an empty line.
 */
enum class ParseLine
{
    break_if_empty,
    continue_if_empty,
};
/*!
 * \brief Iterates over each line in the given input string and calls the given callback function.
 *
 * \param input The input string to be parsed into lines.
 * \param fnLineCb The callback function that is called for each line. The line is passed as the only argument.
 *                 The callback function should return true if the iteration should continue, or false if it
 *                 should be interrupted.
 * \param mode The behavior of for_each_line when an empty line is encountered.
 *             If not specified, the default value is ParseLine::break_if_empty.
 *
 * \return true if the iteration completed successfully, or false if it was interrupted by the callback function
 *         or by encountering an empty line (depending on the value of the mode parameter).
 */
constexpr bool for_each_line(std::string_view input, auto &&fnLineCb, ParseLine mode = ParseLine::break_if_empty)
{
    while (!input.empty())
    {
        const auto crOrLn = input.find_first_of("\r\n");
        const auto line = (crOrLn == input.npos) ? input.substr(0) : input.substr(0, crOrLn);
        if ((mode == ParseLine::break_if_empty) && line.empty())
        {
            return false;
        }
        if (!fnLineCb(line))
        {
            return false;
        }
        const auto lf = input.find('\n');
        if (lf == input.npos)
        {
            return true;
        }
        input.remove_prefix(lf + 1u);
    }
    return true;
}

/*!
 * \brief A view class that provides an iterator over the lines in a given string.
 *
 * \tparam T The type of string to be iterated over (e.g. std::string, std::string_view).
 *
 * The iterator returned by begin() will initially point to the first line in the string.
 * The end() iterator is reached when the end of the string is reached, or when an empty line is encountered.
 *
 * Example usage:
 * \code
 * std::string input = "hello\nworld\n\nthis\nhas\nmultiple\nlines";
 * LinesView<std::string> view(input);
 * for (const auto &line : view)
 * {
 *     std::cout << line << std::endl;
 * }
 * \endcode
 *
 * This will output the following:
 * \verbatim
hello
world

this
has
multiple
lines
\endverbatim
 *
 * Note that the empty line after "world" is not included in the iteration, because it marks the end of the view.
 */
template <typename T>
class LinesView final
{
  public:
    class Iterator final
    {
      public:
        using S = typename T::size_type;
        using difference_type = typename std::iterator_traits<typename T::iterator>::difference_type;
        using iterator_category = typename std::iterator_traits<typename T::iterator>::iterator_category;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
      public:
        constexpr Iterator(const T *view = nullptr, S pos = T::npos) : view(view), pos(pos) {}

        constexpr Iterator &operator++()
        {
            if (view->size() <= pos)
            {
                pos = T::npos;
                return *this;
            }
            const auto ln = view->find('\n', pos);
            const auto behindLn = ln + 1u;
            pos = ((ln == T::npos) || (behindLn >= view->size())) ? T::npos : behindLn;
            return *this;
        }
        constexpr T operator*() const
        {
            if ((pos >= view->size()) || (pos == T::npos))
            {
                return std::string_view{};
            }
            const auto crOrLn = view->find_first_of("\r\n", pos);
            const auto line = (crOrLn == view->npos) ? view->substr(pos) : view->substr(pos, crOrLn - pos);
            return line;
        }

        constexpr bool operator!=(const Iterator &other) const
        {
            return pos != other.pos;
        };

      private:
        const T *view;
        S pos{0u};
    };
  public:
    constexpr LinesView(const T view) : view(view) {}

    constexpr Iterator begin() const
    {
        return Iterator{&view, 0u};
    }

    constexpr Iterator end() const
    {
        return Iterator{};
    }
  private:
    T view;
};

template <typename T> constexpr T parse_number(std::string_view str, const std::uint8_t base = 10u)
{
    T ret{0};
    [[maybe_unused]] bool bNegative = false;
    bool bParsedFirstDigit = false;
    for (; str.size(); str.remove_prefix(1u))
    {
        const auto c = str.front();
        if (!bParsedFirstDigit)
        {
            switch (c)
            {
            case '-':
                if constexpr (std::is_unsigned_v<std::decay_t<T>>)
                {
                    return ret;
                }
                bNegative = true;
                continue;
            case '+':
            case ' ':
                continue;

            default:
                break;
            }
        }
        T digit = 0;
        if ((c >= '0') && (c <= '9'))
        {
            digit = static_cast<T>(c) - static_cast<T>('0');
        }
        else if ((base > 10) && ((c >= 'a') && (c <= 'z')))
        {
            digit = static_cast<T>(c) - static_cast<T>('a') + static_cast<T>(10);
        }
        else if ((base > 10) && ((c >= 'A') && (c <= 'Z')))
        {
            digit = static_cast<T>(c) - static_cast<T>('A') + static_cast<T>(10);
        }
        else
        {
            break;
        }
        if (digit >= base)
        {
            break;
        }
        bParsedFirstDigit = true;
        ret *= base;
        ret += digit;
    }
    if constexpr (std::is_signed_v<std::decay_t<T>>)
    {
        if (bNegative)
        {
            ret *= -1;
        }
    }
    return ret;
}
static_assert(5 == parse_number<int>("5"));
static_assert(50 == parse_number<int>(" 50"));
static_assert(450 == parse_number<int>(" +   450fdsgs"));
static_assert(-21 == parse_number<int>("- 21"));
static_assert(-14 == parse_number<int>("- 1110", 2));
static_assert(0xFFFF == parse_number<unsigned int>(" +  FFFF", 16));
static_assert(0xDEADBEEFul == parse_number<unsigned int>(" +  DeADbEEF", 16));

template <typename T, char DELIMITER> AOC_Y2022_CONSTEXPR std::vector<T> parse_to_vector_of_numbers(std::string_view str)
{
    std::vector<T> ret{};
    for (std::size_t start = 0; str.size(); str.remove_prefix(start + 1))
    {
        ret.push_back(parse_number<T>(str));
        start = str.find_first_of(DELIMITER);
        if (start == str.npos)
        {
            break;
        }
    }
    return ret;
}

template <typename T, std::size_t N> consteval auto vector_to_array(const std::vector<T> &vec)
{
    std::array<T, N> ret{};
    std::copy(vec.begin(), vec.end(), ret.begin());
    return ret;
}

template <typename T> class Vector2D
{
  public:
    [[nodiscard]] AOC_Y2022_CONSTEXPR Vector2D(std::size_t width, std::vector<T> &&vec) : m_width(width), m_vec(vec)
    {
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR T &operator()(std::size_t col, std::size_t row)
    {
        return m_vec[m_width * row + col];
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR const T &operator()(std::size_t col, std::size_t row) const
    {
        return m_vec[m_width * row + col];
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR T &operator[](std::size_t pos)
    {
        return m_vec[pos];
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR const T &operator[](std::size_t pos) const
    {
        return m_vec[pos];
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto width() const
    {
        return m_width;
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto height() const
    {
        const auto w = width();
        const auto s = size();
        const std::size_t h = (w != 0) ? (s / w) : ((s == 0) ? 0 : 1);
        return h;
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto size() const
    {
        return m_vec.size();
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto empty() const
    {
        return m_vec.empty();
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto begin() const
    {
        return m_vec.begin();
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto end() const
    {
        return m_vec.end();
    }

    [[nodiscard]] AOC_Y2022_CONSTEXPR auto square() const
    {
        return ((height() > 0) && (height() == width()));
    }

  private:
    std::size_t m_width;
    std::vector<T> m_vec;
};

[[nodiscard]] AOC_Y2022_CONSTEXPR inline Vector2D<std::uint8_t> parse_to_vector_2d(std::string_view input,
                                                                                   const bool requireSquare = false)
{
    std::vector<std::uint8_t> parsed;
    parsed.reserve(input.size());
    std::size_t width{0};
    auto forEachLine = [&parsed, &width](const std::string_view &line) -> bool {
        if (width == 0)
        {
            width = line.size();
        }
        else if (width != line.size())
        {
            return false;
        }
        for (auto &&c : line)
        {
            if ((c < '0') || (c > '9'))
            {
                return false;
            }
            parsed.emplace_back(c - '0');
        }
        return true;
    };
    const auto validateShape = [&width, &parsed, &requireSquare] {
        return ((width > 0) && ((parsed.size() % width) == 0) &&
                ((!requireSquare) || (parsed.size() != (width * width))));
    };
    if (!for_each_line(input, forEachLine) || !validateShape())
    {
        return {0, std::vector<std::uint8_t>()};
    }
    else
    {
        return {width, std::move(parsed)};
    }
}

[[nodiscard]] AOC_Y2022_CONSTEXPR auto abs(const auto &v)
{
    return (v < 0) ? (-v) : v;
}

} // namespace AOC::Y2022

#endif // !defined(AOC_Y2022_UTILS_HPP)
