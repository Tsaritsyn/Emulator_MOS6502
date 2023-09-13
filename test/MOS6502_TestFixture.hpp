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
#include "Addressing.hpp"
#include "Instruction.hpp"

using namespace Emulator;




struct MOS6502_TestFixture: public ::testing::Test, public MOS6502 {

    /// null if the addressing mode does not imply reading/writing any values from memory or registers
    const Byte* point_to_value(const Addressing& addressing) const;

    void write_argument(Byte value, const Addressing &addressing);

    Word PC_after(Word PCBefore, const InstructionArguments &instruction, const Addressing &addressing);

    ProcessorStatus flags_after(const ProcessorStatus &flagsBefore, const InstructionArguments &instruction, const Addressing &addressing);



    /**
     * Writes the given word to the given address in memory.
     * Low byte of the word will be written first.
     */
    void write_word(Word word, Word to) {
        const WordToBytes buf(word);
        memory[to] = buf.low;
        memory[to + 1] = buf.high;
    }

    void test_instruction(const InstructionArguments &instruction, const Addressing &addressing);
};


#endif //EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
