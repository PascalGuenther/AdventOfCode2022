#if !defined(AOC_Y2022_PUZZLE_COMMON_HPP)
#define AOC_Y2022_PUZZLE_COMMON_HPP (1)

#define AOC_Y2022_PUZZLE_CLASS_DECLARATION(day)                                                                        \
    class PuzzleDay##day##Impl;                                                                                        \
    class PuzzleDay##day final : public IPuzzle                                                                        \
    {                                                                                                                  \
      public:                                                                                                          \
        explicit PuzzleDay##day(std::string_view input);                                                               \
        [[nodiscard]] Solution_t Part1() override;                                                                     \
        [[nodiscard]] Solution_t Part2() override;                                                                     \
        ~PuzzleDay##day() override;                                                                                    \
                                                                                                                       \
      private:                                                                                                         \
        std::unique_ptr<PuzzleDay##day##Impl> pImpl;                                                                   \
    };

#endif // !defined(AOC_Y2022_PUZZLE_COMMON_HPP)
