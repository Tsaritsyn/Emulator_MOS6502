//
// Created by Mikhail on 27/10/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_ROL, Test_Accumulator) {
    test_unary(ROL_ACCUMULATOR,
               GetParam(),
               UNARY_ACCUMULATOR_PARAMS,
               writer_to(AC),
               reader_from(AC));
}

TEST_P(MOS6502_TestFixture_ROL, Test_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_unary(ROL_ZERO_PAGE,
               GetParam(),
               UNARY_ZERO_PAGE_PARAMS,
               writer_to_zero_page(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address]; });
}

TEST_P(MOS6502_TestFixture_ROL, Test_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_unary(ROL_ZERO_PAGE_X,
               GetParam(),
               UNARY_ZERO_PAGE_X_PARAMS,
               writer_to_zero_page_X(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[static_cast<Byte>(address + X)]; });
}

TEST_P(MOS6502_TestFixture_ROL, Test_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_unary(ROL_ZERO_PAGE_X,
               GetParam(),
               UNARY_ZERO_PAGE_X_PARAMS,
               writer_to_zero_page_X(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[static_cast<Byte>(address + X)]; });
}

TEST_P(MOS6502_TestFixture_ROL, Test_Absolute) {
    constexpr static Word address = 0x02f0;
    test_unary(ROL_ABSOLUTE,
               GetParam(),
               UNARY_ABSOLUTE_PARAMS,
               writer_to_absolute(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address]; });
}

TEST_P(MOS6502_TestFixture_ROL, Test_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x05;
    test_unary(ROL_ABSOLUTE_X,
               GetParam(),
               UNARY_ABSOLUTE_X_PARAMS,
               writer_to_absolute_X(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address + X]; });
}

TEST_P(MOS6502_TestFixture_ROL, Test_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_unary(ROL_ABSOLUTE_X,
               GetParam(),
               UNARY_ABSOLUTE_X_PARAMS,
               writer_to_absolute_X(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               [this]() { return memory[address + X]; });
}



INSTANTIATE_TEST_SUITE_P(CarryReset, MOS6502_TestFixture_ROL, ::testing::Values(
        UnaryOpParameters{.arg = 0, .carry = 0, .result = 0, .flagsSet = {Flag::ZERO}},
        UnaryOpParameters{.arg = 0x80, .carry = 0, .result = 0, .flagsSet = {Flag::ZERO, Flag::CARRY}},
        UnaryOpParameters{.arg = 0x40, .carry = 0, .result = 0x80, .flagsSet = {Flag::NEGATIVE}},
        UnaryOpParameters{.arg = 0xc0, .carry = 0, .result = 0x80, .flagsSet = {Flag::NEGATIVE, Flag::CARRY}},
        UnaryOpParameters{.arg = 0xff, .carry = 0, .result = 0xfe, .flagsSet = {Flag::NEGATIVE, Flag::CARRY}},
        UnaryOpParameters{.arg = 0x7f, .carry = 0, .result = 0xfe, .flagsSet = {Flag::NEGATIVE}},
        UnaryOpParameters{.arg = 0x39, .carry = 0, .result = 0x72, .flagsSet = {}}
));

INSTANTIATE_TEST_SUITE_P(CarrySet, MOS6502_TestFixture_ROL, ::testing::Values(
        UnaryOpParameters{.arg = 0, .carry = 1, .result = 1, .flagsSet = {}},
        UnaryOpParameters{.arg = 0x80, .carry = 1, .result = 1, .flagsSet = {Flag::CARRY}},
        UnaryOpParameters{.arg = 0x40, .carry = 1, .result = 0x81, .flagsSet = {Flag::NEGATIVE}},
        UnaryOpParameters{.arg = 0xc0, .carry = 1, .result = 0x81, .flagsSet = {Flag::NEGATIVE, Flag::CARRY}},
        UnaryOpParameters{.arg = 0xff, .carry = 1, .result = 0xff, .flagsSet = {Flag::NEGATIVE, Flag::CARRY}},
        UnaryOpParameters{.arg = 0x7f, .carry = 1, .result = 0xff, .flagsSet = {Flag::NEGATIVE}},
        UnaryOpParameters{.arg = 0x39, .carry = 1, .result = 0x73, .flagsSet = {}}
));
