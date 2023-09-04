//
// Created by Mikhail on 30/08/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
#define EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP

#include <gtest/gtest.h>
#include <variant>

#include "MOS6502.hpp"
#include "MOS6502_helpers.hpp"
#include "MOS6502_definitions.hpp"

using namespace Emulator;




struct MOS6502_TestFixture: public ::testing::Test, public MOS6502 {
    struct Operands {
        Byte AC;
        Byte memory;
        bool carry;


        [[nodiscard]] constexpr Byte sum() const { return AC + memory + carry; }

        [[nodiscard]] constexpr std::vector<Flag> flags_sum() const {
            std::vector<Flag> flags{};

            auto [_, overflowUnsigned] = add_and_clip<int>(AC, memory, carry, 0, UINT8_MAX + 1);
            if (overflowUnsigned) flags.push_back(CARRY);

            auto [valueSigned, overflowSigned] = add_and_clip<int>((char)AC, (char)memory, carry, CHAR_MIN, CHAR_MAX + 1);
            if (valueSigned == 0) flags.push_back(ZERO);
            if (valueSigned < 0) flags.push_back(NEGATIVE);
            if (overflowSigned) flags.push_back(OVERFLOW);

            return flags;
        }


        [[nodiscard]] constexpr Byte sub() const { return AC - memory - !carry; }

        [[nodiscard]] constexpr std::vector<Flag> flags_sub() const {
            std::vector<Flag> flags{};

            auto [_, overflowUnsigned] = subtract_and_clip<int>(AC, memory, carry, 0, UINT8_MAX + 1);
            if (!overflowUnsigned) flags.push_back(CARRY);

            auto [valueSigned, overflowSigned] = subtract_and_clip<int>((char)AC, (char)memory, carry, CHAR_MIN, CHAR_MAX + 1);
            if (valueSigned == 0) flags.push_back(ZERO);
            if (valueSigned < 0) flags.push_back(NEGATIVE);
            if (overflowSigned) flags.push_back(OVERFLOW);

            return flags;
        }
    };



    // Addressing mode cases
    struct Immediate { Byte value; };
    struct ZeroPage { Byte address, value; };
    struct ZeroPageX { Byte address, X, value; };
    struct ZeroPageY { Byte address, Y, value; };
    struct Relative { char offset; };
    struct Absolute { Word address; Byte value; };
    struct AbsoluteX { Word address; Byte X, value; };
    struct AbsoluteY { Word address; Byte Y, value; };
    struct Indirect { Word address, newAddress; };
    struct IndirectX { Byte tableAddress; Word targetAddress; Byte X, value; };
    struct IndirectY { Byte tableAddress; Word targetAddress; Byte Y, value; };
    using Addressing = std::variant<Immediate, ZeroPage, ZeroPageX, ZeroPageY, Relative, Absolute, AbsoluteX, AbsoluteY, Indirect, IndirectX, IndirectY>;

    void prepare_instruction(const Addressing &args);



    /**
     * Writes the given word to the given address in memory.
     * Low byte of the word will be written first.
     */
    void write_word(Word word, Word to) {
        const WordToBytes buf(word);
        memory[to] = buf.low;
        memory[to + 1] = buf.high;
    }


    void test_ADC(AddressingMode mode, Operands operands, bool crossPage);
};


#endif //EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
