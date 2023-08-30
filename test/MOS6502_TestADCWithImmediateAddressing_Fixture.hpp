//
// Created by Mikhail on 30/08/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_TESTADCWITHIMMEDIATEADDRESSING_FIXTURE_HPP
#define EMULATOR_MOS6502_MOS6502_TESTADCWITHIMMEDIATEADDRESSING_FIXTURE_HPP

#include <gtest/gtest.h>

#include "MOS6502.hpp"

using namespace Emulator;


class MOS6502_TestADCWithImmediateAddressing_Fixture: public ::testing::Test, public MOS6502 {
    void SetUp() override {
        SR = 0;
        PC = 0;
    }

public:

    void test(Byte initialAC, Byte initialMemory, bool carry, Byte expectedSum, Byte expectedFlags) {
        AC = initialAC;
        set_flag(CARRY, carry);

        memory[PC] = OpCode::ADC_IMMEDIATE;
        memory[PC + 1] = initialMemory;

        size_t initialCycle = cycle;
        Word initialPC = PC;

        execute_current_command();

        EXPECT_EQ(AC, expectedSum);
        EXPECT_EQ(SR, expectedFlags);
        EXPECT_EQ(cycle, initialCycle + 2);
        ASSERT_EQ(PC, initialPC + 2);
    }
};


#endif //EMULATOR_MOS6502_MOS6502_TESTADCWITHIMMEDIATEADDRESSING_FIXTURE_HPP
