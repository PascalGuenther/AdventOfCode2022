#include "ipuzzle.hpp"
#include "puzzle_common.hpp"
#include "utils.hpp"
#include "ctre.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace AOC::Y2022
{

namespace
{

    struct Display
    {
        using Row = std::array<char, 40u>;
        static constexpr Row emptyRow
        {
            '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
        };
        std::array<Row, 6u> rows
        {
            emptyRow,
            emptyRow,
            emptyRow,
            emptyRow,
            emptyRow,
            emptyRow,
        };
    };

    struct Instruction final
    {
        enum class Opcode
        {
            noop, addx,
        };
        using Operand = std::int32_t;
        AOC_Y2022_CONSTEXPR Instruction(Opcode opcode, Operand operand = 0) : opcode(opcode), operand(operand) {}
        Opcode opcode;
        Operand operand;
    };

    using InstructionList = std::vector<Instruction>;

    AOC_Y2022_CONSTEXPR auto parse_motions(std::string_view input) -> InstructionList
    {
        InstructionList instructions;
        instructions.reserve(128u);
        for (const auto &line : LinesView{input})
        {
            if (line.empty())
            {
                return instructions;
            }
            if (line == "noop")
            {
                instructions.emplace_back(Instruction::Opcode::noop);
                continue;
            }
            if (auto [match, operandString] = ctre::match<"^addx (-?\\d+)$">(line); match)
            {
                instructions.emplace_back(Instruction::Opcode::addx, parse_number<Instruction::Operand>(operandString));
            }
            else
            {
                return InstructionList{};
            }
        }
        return instructions;
    }

    class Cpu final
    {
      public:
        using Register = std::int32_t;
        enum class State : std::uint8_t {
            busy, ready,
        };
      public:
        AOC_Y2022_CONSTEXPR auto process_instruction(const Instruction &instruction) -> State
        {
            if (state == State::busy)
            {
                cpuRegister += instruction.operand;
                state = State::ready;
                return state;
            }
            switch (instruction.opcode)
            {   
                default:
                case Instruction::Opcode::noop:
                    return state;
                case Instruction::Opcode::addx:
                    state = State::busy;
                    return state;
            }
        }

        AOC_Y2022_CONSTEXPR auto get_register() const -> Register
        {
            return cpuRegister;
        }
      private:
        Register cpuRegister{1};
        State state{State::ready};
    };


    AOC_Y2022_CONSTEXPR auto simulate_cpu(const InstructionList &input, auto &onEachCycle) -> bool
    {
        if (input.empty())
        {
            return false;
        }
        Cpu cpu{};
        std::int32_t cycle = 0;
        for (const auto &instruction : input)
        {
            do
            {
                ++cycle;
                onEachCycle(cycle, cpu);
            } while (Cpu::State::ready != cpu.process_instruction(instruction));
        }
        return true;
    }

    AOC_Y2022_CONSTEXPR auto part_1(const InstructionList &input) -> IPuzzle::Solution_t
    {
        std::int32_t sumOfSignalStrengths = 0;
        const auto accumulateSignalStrengths = [&sumOfSignalStrengths](const std::int32_t cycle, const Cpu &cpu) {
            if (((cycle - 20) % 40) != 0)
            {
                return;
            }
            const decltype(sumOfSignalStrengths) signalStrength = cycle * cpu.get_register();
            sumOfSignalStrengths += signalStrength;
        };
        if (simulate_cpu(input, accumulateSignalStrengths))
        {
            return sumOfSignalStrengths;
        }
        return std::monostate{};
    }

    AOC_Y2022_CONSTEXPR auto part_2(const InstructionList &input, Display &display) -> bool
    {
        bool noError = true;
        const auto accumulateSignalStrengths = [&display, &noError](const std::int32_t cycle, const Cpu &cpu) {
            std::int32_t height{static_cast<std::int32_t>(display.rows.size())};
            std::int32_t width{static_cast<std::int32_t>(display.rows[0].size())};
            const std::int32_t pixel = (cycle - 1) % width;
            const std::int32_t row = (cycle - 1) / width;
            if (row >= height)
            {
                noError = false;
                return;
            }
            constexpr std::int32_t spriteWidth = 3;
            const auto spritePosition = cpu.get_register() - (spriteWidth / 2);
            if ((pixel >= spritePosition) && (pixel < (spritePosition + spriteWidth)))
            {
                display.rows[static_cast<std::size_t>(row)][static_cast<std::size_t>(pixel)] = '#';
            }
        };
        if (simulate_cpu(input, accumulateSignalStrengths))
        {
            return noError;
        }
        return false;
    }

} // namespace


class PuzzleDay10Impl final {
  public:
    AOC_Y2022_CONSTEXPR PuzzleDay10Impl(std::string_view input) : instructions(parse_motions(input)) {}
    InstructionList instructions;
};

AOC_Y2022_PUZZLE_CLASS_DECLARATION(10)

PuzzleDay10::PuzzleDay10(const std::string_view input)
    : pImpl(std::make_unique<PuzzleDay10Impl>(input))
{
}
PuzzleDay10::~PuzzleDay10() = default;

[[nodiscard]] IPuzzle::Solution_t PuzzleDay10::Part1()
{
    return part_1(pImpl->instructions);
}

[[nodiscard]] IPuzzle::Solution_t PuzzleDay10::Part2()
{
    Display display;
    if (!part_2(pImpl->instructions, display))
    {
        return std::monostate{};
    }
    std::string output;
    for (const auto &row : display.rows)
    {
        for (const auto c : row)
        {
            output += c;
        }
        output += '\n';
    }
    return output;
}

#if AOC_Y2022_CONSTEXPR_UNIT_TEST
namespace
{
static constexpr const char *exampleInput1 =
R"ExampleInput1(addx 15
addx -11
addx 6
addx -3
addx 5
addx -1
addx -8
addx 13
addx 4
noop
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx -35
addx 1
addx 24
addx -19
addx 1
addx 16
addx -11
noop
noop
addx 21
addx -15
noop
noop
addx -3
addx 9
addx 1
addx -3
addx 8
addx 1
addx 5
noop
noop
noop
noop
noop
addx -36
noop
addx 1
addx 7
noop
noop
noop
addx 2
addx 6
noop
noop
noop
noop
noop
addx 1
noop
noop
addx 7
addx 1
noop
addx -13
addx 13
addx 7
noop
addx 1
addx -33
noop
noop
noop
addx 2
noop
noop
noop
addx 8
noop
addx -1
addx 2
addx 1
noop
addx 17
addx -9
addx 1
addx 1
addx -3
addx 11
noop
noop
addx 1
noop
addx 1
noop
noop
addx -13
addx -19
addx 1
addx 3
addx 26
addx -30
addx 12
addx -1
addx 3
addx 1
noop
noop
noop
addx -9
addx 18
addx 1
addx 2
noop
noop
addx 9
noop
noop
noop
addx -1
addx 2
addx -37
addx 1
addx 3
noop
addx 15
addx -21
addx 22
addx -6
addx 1
noop
addx 2
addx 1
noop
addx -10
noop
noop
addx 20
addx 1
addx 2
addx 2
addx -6
addx -11
noop
noop
noop
)ExampleInput1";

consteval bool TestDay10()
{
    PuzzleDay10Impl parsed1{exampleInput1};
    if (13140 != std::get<std::int64_t>(part_1(parsed1.instructions)))
    {
        return false;
    }
    Display display{};
    if (!part_2(parsed1.instructions, display))
    {
        return false;
    }
    if ("##..##..##..##..##..##..##..##..##..##.." != std::string_view{display.rows[0].begin(), display.rows[0].end()}) return false;
    if ("###...###...###...###...###...###...###." != std::string_view{display.rows[1].begin(), display.rows[1].end()}) return false;
    if ("####....####....####....####....####...." != std::string_view{display.rows[2].begin(), display.rows[2].end()}) return false;
    if ("#####.....#####.....#####.....#####....." != std::string_view{display.rows[3].begin(), display.rows[3].end()}) return false;
    if ("######......######......######......####" != std::string_view{display.rows[4].begin(), display.rows[4].end()}) return false;
    if ("#######.......#######.......#######....." != std::string_view{display.rows[5].begin(), display.rows[5].end()}) return false;

    return true;
}
static_assert(TestDay10(), "Wrong results for example input");

} // namespace
#endif // AOC_Y2022_CONSTEXPR_UNIT_TEST

} // namespace AOC::Y2022
