#if !defined(AOC_Y2022_GAME_FACTORY_HPP)
#define AOC_Y2022_GAME_FACTORY_HPP (1)

#include "ipuzzle.hpp"
#include "types.hpp"

#include <cstdint>
#include <memory>
#include <string_view>

namespace AOC::Y2022
{

AOC_Y2022_API
class PuzzleFactory final
{
  public:
    static std::unique_ptr<IPuzzle> CreatePuzzle(std::uint8_t game, std::string_view input);
};

} // namespace AOC::Y2022

#endif // !defined(AOC_Y2022_GAME_FACTORY_HPP)
