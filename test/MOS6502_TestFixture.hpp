//
// Created by Mikhail on 14/09/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
#define EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP

#include <gtest/gtest.h>
#include <ostream>

#include "MOS6502.hpp"
#include "helpers.hpp"



using namespace Emulator;


class MOS6502_TextFixture: public MOS6502 {
protected:
    void reset_registers() noexcept;

    std::expected<void, ROM::StackOverride> set_word(Word address, Word value) noexcept;
};



struct ArithmeticTestParameters {
    friend std::ostream &operator<<(std::ostream &os, const ArithmeticTestParameters &parameters);

    Byte AC, memory;
    bool carry;
    Byte result;
    std::vector<Flag> flagsSet;
};

class MOS6502_TestFixture_Arithmetic: public ::testing::TestWithParam<ArithmeticTestParameters>, public MOS6502_TextFixture {
    void SetUp() override;
};

class MOS6502_TestFixture_ADC: public MOS6502_TestFixture_Arithmetic {};

class MOS6502_TestFixture_SBC: public MOS6502_TestFixture_Arithmetic {};


#endif //EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
