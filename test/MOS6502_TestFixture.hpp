//
// Created by Mikhail on 30/08/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
#define EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP

#include <gtest/gtest.h>

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
            const unsigned sum = AC + memory + carry;
            std::vector<Flag> result{};

            if ((Byte) sum == 0) result.push_back(ZERO);
            if ((char) sum < 0)  result.push_back(NEGATIVE);
            if (sum > UINT8_MAX) result.push_back(CARRY);
            // every time when sign changes illegally
            if (((char) AC > 0 && (char) memory > 0 && (char) sum <= 0)
                || ((char) AC > 0 && (char) memory == 0 && (char) sum <= 0)
                || ((char) AC == 0 && (char) memory > 0 && (char) sum <= 0)
                || ((char) AC < 0 && (char) memory < 0 && (char) sum >= 0))
                result.push_back(OVERFLOW);

            return result;
        }
    };



    /**
     * Writes the given word to the given address in memory.
     * Low byte of the word will be written first.
     */
    void write_word(Word word, Word address) {
        const WordToBytes buf(word);
        memory[address] = buf.low;
        memory[address + 1] = buf.high;
    }


    void test_ADC(AddressingMode mode, Operands operands) {
        // setup
        SR = 0;
        PC = 0;
        cycle = 0;

        AC = operands.AC;
        set_flag(CARRY, operands.carry, false);

        /// number of cycles to be elapsed
        size_t expectedCommandDuration;
        /// expected PC shift in bytes
        Word expectedCommandSize;

        // addressing-specific setup
        switch (mode) {
            case AddressingMode::IMMEDIATE: {
                memory[PC] = OpCode::ADC_IMMEDIATE;
                memory[PC + 1] = operands.memory;

                expectedCommandSize = 2;
                expectedCommandDuration = 2;
            } break;

            case AddressingMode::ZERO_PAGE: {
                constexpr Byte zeroPageAddress = 0xFF;
                memory[zeroPageAddress] = operands.memory;
                memory[PC] = OpCode::ADC_ZERO_PAGE;
                memory[PC + 1] = zeroPageAddress;

                expectedCommandSize = 2;
                expectedCommandDuration = 3;
            } break;

            case AddressingMode::ZERO_PAGE_X: {
                constexpr Byte zeroPageAddress = 0xF0;
                X = 0x0F;

                memory[zeroPageAddress + X] = operands.memory;
                memory[PC] = OpCode::ADC_ZERO_PAGE_X;
                memory[PC + 1] = zeroPageAddress;

                expectedCommandSize = 2;
                expectedCommandDuration = 4;
            } break;

//            case AddressingMode::ZERO_PAGE_Y: {
//                Byte zeroPageAddress = 0xF0;
//                Y = 0x0F;
//                memory[zeroPageAddress + Y] = initialMemory;
//                memory[PC] = OpCode::ADC_ZERO_PAGE_Y;
//                memory[PC + 1] = zeroPageAddress;
//            } break;

//            case AddressingMode::RELATIVE:
//                break;

            case AddressingMode::ABSOLUTE: {
                constexpr Word address = 0x1234;

                memory[address] = operands.memory;
                memory[PC] = OpCode::ADC_ABSOLUTE;
                write_word(address, PC + 1);

                expectedCommandSize = 3;
                expectedCommandDuration = 4;
            } break;

            case AddressingMode::ABSOLUTE_X: {
                constexpr Word address = 0x1200;
                X = 0x34;

                memory[address + X] = operands.memory;
                memory[PC] = OpCode::ADC_ABSOLUTE_X;
                write_word(address, PC + 1);

                // TODO: prepare the case where the page is crossed
                expectedCommandSize = 3;
                expectedCommandDuration = 4;
            } break;

            case AddressingMode::ABSOLUTE_Y: {
                constexpr Word address = 0x1200;
                Y = 0x34;

                memory[address + Y] = operands.memory;
                memory[PC] = OpCode::ADC_ABSOLUTE_Y;
                write_word(address, PC + 1);

                // TODO: prepare the case where the page is crossed
                expectedCommandSize = 3;
                expectedCommandDuration = 4;
            } break;

//            case AddressingMode::INDIRECT:
//                break;

            case AddressingMode::INDIRECT_X: {
                constexpr Byte tableAddress = 0x10;
                X = 0x0f;
                constexpr Word targetAddress = 0x1234;

                memory[targetAddress] = operands.memory;
                write_word(targetAddress, tableAddress + X);

                memory[PC] = OpCode::ADC_INDIRECT_X;
                memory[PC + 1] = tableAddress;

                expectedCommandSize = 2;
                expectedCommandDuration = 6;
            } break;

            case AddressingMode::INDIRECT_Y: {
                constexpr Byte tableAddress = 0x10;
                Y = 0x20;
                constexpr Word targetAddress = 0x1234;

                write_word(targetAddress, tableAddress);
                memory[targetAddress + Y] = operands.memory;

                memory[PC] = OpCode::ADC_INDIRECT_Y;
                memory[PC + 1] = tableAddress;

                // TODO: prepare the case where the page is crossed
                expectedCommandSize = 2;
                expectedCommandDuration = 5;
            } break;
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
