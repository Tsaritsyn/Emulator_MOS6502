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
    /**
     * Writes all intermediate values (if any) into memory necessary for reading/writing of the target value (if any).
     *
     * @return reference to the memory or register where the target value must be put to (or where to read it from), if any.
     */
    Byte* const prepare_memory(const Addressing& addressing);

public:
    void test_load_accumulator(Byte value, const Addressing& addressing);
    void test_load_X(Byte value, const Addressing& addressing);
    void test_load_Y(Byte value, const Addressing& addressing);

    void test_store_accumulator(Byte value, const Addressing& addressing);
    void test_store_X(Byte value, const Addressing& addressing);
    void test_store_Y(Byte value, const Addressing& addressing);

    void test_register_transfer(Byte value, Register from, Register to);
};


#endif //EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
