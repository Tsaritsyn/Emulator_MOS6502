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


    void test_ADC(AddressingMode mode, Operands operands) {
        // setup
        SR = 0;
        PC = 0;
        cycle = 0;

        AC = operands.AC;
        SR[CARRY] = operands.carry;

        /// number of cycles to be elapsed
        size_t expectedCommandDuration;
        /// expected PC shift in bytes
        Word expectedCommandSize;

        // addressing-specific setup
        switch (mode) {
            case AddressingMode::IMMEDIATE: {
                memory[PC] = OpCode::ADC_IMMEDIATE;
                prepare_instruction(Immediate{operands.memory});

                expectedCommandSize = 2;
                expectedCommandDuration = 2;
            } break;

            case AddressingMode::ZERO_PAGE: {
                memory[PC] = OpCode::ADC_ZERO_PAGE;
                prepare_instruction(ZeroPage{0xFF, operands.memory});

                expectedCommandSize = 2;
                expectedCommandDuration = 3;
            } break;

            case AddressingMode::ZERO_PAGE_X: {
                memory[PC] = OpCode::ADC_ZERO_PAGE_X;
                prepare_instruction(ZeroPageX{0xF0, 0x0F, operands.memory});

                expectedCommandSize = 2;
                expectedCommandDuration = 4;
            } break;

            case AddressingMode::ABSOLUTE: {
                memory[PC] = OpCode::ADC_ABSOLUTE;
                prepare_instruction(Absolute{0x1234, operands.memory});

                expectedCommandSize = 3;
                expectedCommandDuration = 4;
            } break;

            case AddressingMode::ABSOLUTE_X: {
                memory[PC] = OpCode::ADC_ABSOLUTE_X;
                prepare_instruction(AbsoluteX{0x1200, 0x34, operands.memory});

                // TODO: prepare the case where the page is crossed
                expectedCommandSize = 3;
                expectedCommandDuration = 4;
            } break;

            case AddressingMode::ABSOLUTE_Y: {
                memory[PC] = OpCode::ADC_ABSOLUTE_Y;
                prepare_instruction(AbsoluteY{0x1200, 0x34, operands.memory});

                // TODO: prepare the case where the page is crossed
                expectedCommandSize = 3;
                expectedCommandDuration = 4;
            } break;

            case AddressingMode::INDIRECT_X: {
                memory[PC] = OpCode::ADC_INDIRECT_X;
                prepare_instruction(IndirectX{0x10, 0x1234, 0x0f, operands.memory});

                expectedCommandSize = 2;
                expectedCommandDuration = 6;
            } break;

            case AddressingMode::INDIRECT_Y: {
                memory[PC] = OpCode::ADC_INDIRECT_Y;
                prepare_instruction(IndirectY{0x10, 0x1234, 0x20, operands.memory });

                // TODO: prepare the case where the page is crossed
                expectedCommandSize = 2;
                expectedCommandDuration = 5;
            } break;

            default: {
                std::stringstream ss;
                ss << "Unsupported addressing mode " << mode << " for ADC command";
                throw std::invalid_argument(ss.str());
            }
        }

        execute_current_command();

        std::stringstream ss;
        ss << mode << ": " << (int)operands.AC << " + " << (int)operands.memory << " + " << (int)operands.carry << " = " << (int)operands.sum() << "; " << operands.flags_sum();

        EXPECT_EQ(AC, operands.sum()) << ss.str();
        EXPECT_EQ(SR, flag_combination(operands.flags_sum())) << ss.str();

        EXPECT_EQ(cycle, expectedCommandDuration) << ss.str();
        ASSERT_EQ(PC, expectedCommandSize) << ss.str();
    }

};


#endif //EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
