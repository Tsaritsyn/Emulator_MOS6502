//
// Created by Mikhail on 14/09/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
#define EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP

#include <gtest/gtest.h>

#include "MOS6502.hpp"
#include "Addressing.hpp"
#include "MOS6502_helpers.hpp"



using namespace Emulator;

struct MOS6502_TestFixture:  public ::testing::Test, public MOS6502 {
private:

    void reset() noexcept override;
    void write_word(Word word, Word address) noexcept;
    void write_to_memory(Byte value, const Addressing& addressing) noexcept;

public:
    void test_load_accumulator(Byte value, const Addressing& addressing);
    void test_load_X(Byte value, const Addressing& addressing);
    void test_load_Y(Byte value, const Addressing& addressing);
};


#endif //EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
