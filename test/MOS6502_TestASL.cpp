//
// Created by Mikhail on 26/10/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_ASL, Test_Accumulator) {
    test_unary(ASL_ACCUMULATOR,
               GetParam(),
               ExecutionParameters::unary_accumulator(),
               writer_to(AC),
               reader_from(AC));
}

TEST_P(MOS6502_TestFixture_ASL, Test_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_unary(ASL_ZERO_PAGE,
               GetParam(),
               ExecutionParameters::unary_zero_page(),
               writer_to_zero_page(address),
               // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address]; });
}

TEST_P(MOS6502_TestFixture_ASL, Test_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_unary(ASL_ZERO_PAGE_X,
               GetParam(),
               ExecutionParameters::unary_zero_page_indexed(),
               writer_to_zero_page_X(address),
               // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[(Byte)(address + X)]; });
}

TEST_P(MOS6502_TestFixture_ASL, Test_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_unary(ASL_ZERO_PAGE_X,
               GetParam(),
               ExecutionParameters::unary_zero_page_indexed(),
               writer_to_zero_page_X(address),
               // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[(Byte)(address + X)]; });
}

TEST_P(MOS6502_TestFixture_ASL, Test_Absolute) {
    constexpr static Word address = 0x02f0;
    test_unary(ASL_ABSOLUTE,
               GetParam(),
               ExecutionParameters::unary_absolute(),
               writer_to_absolute(address),
               // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address]; });
}

TEST_P(MOS6502_TestFixture_ASL, Test_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x05;
    test_unary(ASL_ABSOLUTE_X,
               GetParam(),
               ExecutionParameters::unary_absolute_indexed(),
               writer_to_absolute_X(address),
               // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address + X]; });
}

TEST_P(MOS6502_TestFixture_ASL, Test_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_unary(ASL_ABSOLUTE_X,
               GetParam(),
               ExecutionParameters::unary_absolute_indexed(),
               writer_to_absolute_X(address),
               // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address + X]; });
}



INSTANTIATE_TEST_SUITE_P(NoFlags, MOS6502_TestFixture_ASL, ::testing::Values(
        UnaryOpParameters{.arg = 0, .carry = 0, .result = 0, .flagsSet = {Flag::ZERO}},
        UnaryOpParameters{.arg = 0x80, .carry = 0, .result = 0, .flagsSet = {Flag::ZERO, Flag::CARRY}},
        UnaryOpParameters{.arg = 0x40, .carry = 0, .result = 0x80, .flagsSet = {Flag::NEGATIVE}},
        UnaryOpParameters{.arg = 0xc0, .carry = 0, .result = 0x80, .flagsSet = {Flag::NEGATIVE, Flag::CARRY}},
        UnaryOpParameters{.arg = 0xff, .carry = 0, .result = 0xfe, .flagsSet = {Flag::NEGATIVE, Flag::CARRY}},
        UnaryOpParameters{.arg = 0x7f, .carry = 0, .result = 0xfe, .flagsSet = {Flag::NEGATIVE}},
        UnaryOpParameters{.arg = 0x39, .carry = 0, .result = 0x72, .flagsSet = {}}
));
