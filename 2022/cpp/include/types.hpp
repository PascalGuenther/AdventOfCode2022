#if !defined(AOC_Y2022_TYPES_HPP)
#define AOC_Y2022_TYPES_HPP (1)

#if !defined(AOC_Y2022_API)
#define AOC_Y2022_API
#endif

#if (defined(__GNUC__) && (__GNUC__ >= 12)) || ((defined(_MSC_VER) && (_MSC_VER >= 1933)))
#define AOC_Y2022_CONSTEXPR_UNIT_TEST   (1)
#else
#define AOC_Y2022_CONSTEXPR_UNIT_TEST   (0)
#endif


#if AOC_Y2022_CONSTEXPR_UNIT_TEST
#define AOC_Y2022_CONSTEXPR constexpr
#else
#define AOC_Y2022_CONSTEXPR
#endif

#endif // !defined(AOC_Y2022_TYPES_HPP)
