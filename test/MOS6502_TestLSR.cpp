//
// Created by Mikhail on 26/10/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_LSR, Test_Accumulator) {
    test_unary(LSR_ACCUMULATOR,
               GetParam(),
               ExecutionParameters::unary_accumulator(),
               writer_to(AC),
               reader_from(AC));
}

TEST_P(MOS6502_TestFixture_LSR, Test_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_unary(LSR_ZERO_PAGE,
               GetParam(),
               ExecutionParameters::unary_zero_page(),
               writer_to_zero_page(address),
               // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address]; });
}

TEST_P(MOS6502_TestFixture_LSR, Test_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_unary(LSR_ZERO_PAGE_X,
               GetParam(),
               ExecutionParameters::unary_zero_page_indexed(),
               writer_to_zero_page_X(address),
               // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[(Byte)(address + X)]; });
}

TEST_P(MOS6502_TestFixture_LSR, Test_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_unary(LSR_ZERO_PAGE_X,
               GetParam(),
               ExecutionParameters::unary_zero_page_indexed(),
               writer_to_zero_page_X(address),
               // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[(Byte)(address + X)]; });
}

TEST_P(MOS6502_TestFixture_LSR, Test_Absolute) {
    constexpr static Word address = 0x02f0;
    test_unary(LSR_ABSOLUTE,
               GetParam(),
               ExecutionParameters::unary_absolute(),
               writer_to_absolute(address),
               // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address]; });
}

TEST_P(MOS6502_TestFixture_LSR, Test_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x05;
    test_unary(LSR_ABSOLUTE_X,
               GetParam(),
               ExecutionParameters::unary_absolute_indexed(),
               writer_to_absolute_X(address),
               // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address + X]; });
}

TEST_P(MOS6502_TestFixture_LSR, Test_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_unary(LSR_ABSOLUTE_X,
               GetParam(),
               ExecutionParameters::unary_absolute_indexed(),
               writer_to_absolute_X(address),
               // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address + X]; });
}



INSTANTIATE_TEST_SUITE_P(ZeroResult, MOS6502_TestFixture_LSR, ::testing::Values(
        UnaryOpParameters{.arg = 0, .carry = 0, .result = 0, .flagsSet = {Flag::ZERO}},
        UnaryOpParameters{.arg = 1, .carry = 0, .result = 0, .flagsSet = {Flag::ZERO, Flag::CARRY}}
));

INSTANTIATE_TEST_SUITE_P(NonZeroResult, MOS6502_TestFixture_LSR, ::testing::Values(
        UnaryOpParameters{.arg = 0x02, .carry = 0, .result = 0x01, .flagsSet = {}},
        UnaryOpParameters{.arg = 0x03, .carry = 0, .result = 0x01, .flagsSet = {Flag::CARRY}},
        UnaryOpParameters{.arg = 0xff, .carry = 0, .result = 0x7f, .flagsSet = {Flag::CARRY}},
        UnaryOpParameters{.arg = 0xfe, .carry = 0, .result = 0x7f, .flagsSet = {}}
));



INSTANTIATE_TEST_SUITE_P(SettingCarry, MOS6502_TestFixture_LSR, ::testing::Values(
        UnaryOpParameters{.arg = 1, .carry = 0, .result = 0, .flagsSet = {Flag::ZERO, Flag::CARRY}},
        UnaryOpParameters{.arg = 0x03, .carry = 0, .result = 0x01, .flagsSet = {Flag::CARRY}},
        UnaryOpParameters{.arg = 0xff, .carry = 0, .result = 0x7f, .flagsSet = {Flag::CARRY}}
));
