//
// Created by Mikhail on 14/09/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
#define EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP

#include <gtest/gtest.h>
#include <ostream>
#include <concepts>

#include "MOS6502.hpp"
#include "helpers.hpp"



using namespace Emulator;


class MOS6502_TextFixture: public MOS6502 {
protected:
    void reset_registers() noexcept;

    [[nodiscard]] std::expected<void, ROM::StackOverride> set_word(Word address, Word value) noexcept;

    ROM::WriteResult write_immediate(Byte value) noexcept;
    ROM::WriteResult write_zero_page(Byte address, Byte value) noexcept;
    ROM::WriteResult write_zero_page_X(Byte address, Byte value) noexcept;
    ROM::WriteResult write_zero_page_Y(Byte address, Byte value) noexcept;
    ROM::WriteResult write_absolute(Word address, Byte value) noexcept;
    ROM::WriteResult write_absolute_X(Word address, Byte value) noexcept;
    ROM::WriteResult write_absolute_Y(Word address, Byte value) noexcept;
    ROM::WriteResult write_indirect_X(Byte tableAddress, Word targetAddress, Byte value) noexcept;
    ROM::WriteResult write_indirect_Y(Byte tableAddress, Word targetAddress, Byte value) noexcept;
};


struct BinaryOpParameters {
    friend std::ostream &operator<<(std::ostream &os, const BinaryOpParameters &parameters);

    Byte AC, memory;
    bool carry;
    Byte result;
    std::vector<Flag> flagsSet;
};

class MOS6502_TestFixture_BinaryOp: public ::testing::TestWithParam<BinaryOpParameters>, public MOS6502_TextFixture {
    void SetUp() override;

public:

    void test_arithmetic(OpCode opcode,
                         const BinaryOpParameters &params,
                         Word size,
                         size_t duration,
                         const std::function<ROM::WriteResult(Byte)> &writer
                         );
};

class MOS6502_TestFixture_ADC: public MOS6502_TestFixture_BinaryOp {};

class MOS6502_TestFixture_SBC: public MOS6502_TestFixture_BinaryOp {};

class MOS6502_TestFixture_AND: public MOS6502_TestFixture_BinaryOp {};


#endif //EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
