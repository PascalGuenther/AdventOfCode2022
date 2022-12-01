#if !defined(AOC_Y2022_UTILS_HPP)
#define AOC_Y2022_UTILS_HPP (1)

#include "../include/types.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <vector>

namespace AOC::Y2022
{

constexpr bool ParseLines(std::string_view input, auto &&fnLineCb, const bool exitOnEmptyLine = true)
{
    while (!input.empty())
    {
        const auto rfOrLn = input.find_first_of("\r\n");
        const auto line = (rfOrLn == input.npos) ? input.substr(0) : input.substr(0, rfOrLn);
        if (exitOnEmptyLine && line.empty())
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

template <typename T> constexpr T ParseNumber(std::string_view str, const std::uint8_t base = 10u)
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
            digit = static_cast<T>(c - '0');
        }
        else if ((base > 10) && ((c >= 'a') && (c <= 'z')))
        {
            digit = static_cast<T>(c - 'a' + 10);
        }
        else if ((base > 10) && ((c >= 'A') && (c <= 'Z')))
        {
            digit = static_cast<T>(c - 'A' + 10);
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
static_assert(5 == ParseNumber<int>("5"));
static_assert(50 == ParseNumber<int>(" 50"));
static_assert(450 == ParseNumber<int>(" +   450fdsgs"));
static_assert(-21 == ParseNumber<int>("- 21"));
static_assert(-14 == ParseNumber<int>("- 1110", 2));
static_assert(0xFFFF == ParseNumber<unsigned int>(" +  FFFF", 16));
static_assert(0xDEADBEEFul == ParseNumber<unsigned int>(" +  DeADbEEF", 16));

template <typename T, char DELIMITER> AOC_Y2022_CONSTEXPR std::vector<T> ParseToVectorOfNums(std::string_view str)
{
    std::vector<T> ret{};
    for (std::size_t start = 0; str.size(); str.remove_prefix(start + 1))
    {
        ret.push_back(ParseNumber<T>(str));
        start = str.find_first_of(DELIMITER);
        if (start == str.npos)
        {
            break;
        }
    }
    return ret;
}

template <typename T, std::size_t N> consteval auto Vector2Array(const std::vector<T> &vec)
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

    [[nodiscard]] AOC_Y2022_CONSTEXPR T operator()(std::size_t col, std::size_t row) const
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

[[nodiscard]] AOC_Y2022_CONSTEXPR inline Vector2D<std::uint8_t> ParseToVector2D(std::string_view input,
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
    if (!ParseLines(input, forEachLine) || !validateShape())
    {
        return {0, std::vector<std::uint8_t>()};
    }
    else
    {
        return {width, std::move(parsed)};
    }
}

} // namespace AOC::Y2022

#endif // !defined(AOC_Y2022_UTILS_HPP)
