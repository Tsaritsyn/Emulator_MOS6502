//
// Created by Mikhail on 27/10/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Increment, Test_INX) {
    test_unary(INX_IMPLICIT,
               GetParam(),
               ExecutionParameters::implied(),
               writer_to(X),
               reader_from(X));
}


TEST_P(MOS6502_TestFixture_Increment, Test_INY) {
    test_unary(INY_IMPLICIT,
               GetParam(),
               ExecutionParameters::implied(),
               writer_to(Y),
               reader_from(Y));
}


TEST_P(MOS6502_TestFixture_Increment, Test_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_unary(INC_ZERO_PAGE,
               GetParam(),
               ExecutionParameters::unary_zero_page(),
               writer_to_zero_page(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address]; });
}

TEST_P(MOS6502_TestFixture_Increment, Test_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_unary(INC_ZERO_PAGE_X,
               GetParam(),
               ExecutionParameters::unary_zero_page_indexed(),
               writer_to_zero_page_X(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[(Byte)(address + X)]; });
}

TEST_P(MOS6502_TestFixture_Increment, Test_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_unary(INC_ZERO_PAGE_X,
               GetParam(),
               ExecutionParameters::unary_zero_page_indexed(),
               writer_to_zero_page_X(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[(Byte)(address + X)]; });
}

TEST_P(MOS6502_TestFixture_Increment, Test_Absolute) {
    constexpr static Word address = 0x02f0;
    test_unary(INC_ABSOLUTE,
               GetParam(),
               ExecutionParameters::unary_absolute(),
               writer_to_absolute(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address]; });
}

TEST_P(MOS6502_TestFixture_Increment, Test_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x05;
    test_unary(INC_ABSOLUTE_X,
               GetParam(),
               ExecutionParameters::unary_absolute_indexed(),
               writer_to_absolute_X(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address + X]; });
}

TEST_P(MOS6502_TestFixture_Increment, Test_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_unary(INC_ABSOLUTE_X,
               GetParam(),
               ExecutionParameters::unary_absolute_indexed(),
               writer_to_absolute_X(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address + X]; });
}



INSTANTIATE_TEST_SUITE_P(PositiveResult, MOS6502_TestFixture_Increment, ::testing::Values(
        UnaryOpParameters{.arg = 0, .carry = 0, .result = 1, .flagsSet = {}},
        UnaryOpParameters{.arg = 1, .carry = 0, .result = 2, .flagsSet = {}},
        UnaryOpParameters{.arg = 10, .carry = 0, .result = 11, .flagsSet = {}}
));

INSTANTIATE_TEST_SUITE_P(NegativeResult, MOS6502_TestFixture_Increment, ::testing::Values(
        UnaryOpParameters{.arg = 0x7f, .carry = 0, .result = 0x80, .flagsSet = {Flag::NEGATIVE}},
        UnaryOpParameters{.arg = 0x93, .carry = 0, .result = 0x94, .flagsSet = {Flag::NEGATIVE}}
));



INSTANTIATE_TEST_SUITE_P(Overflow, MOS6502_TestFixture_Increment, ::testing::Values(
        UnaryOpParameters{.arg = 0xff, .carry = 0, .result = 0, .flagsSet = {Flag::ZERO}}
));
