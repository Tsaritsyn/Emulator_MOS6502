//
// Created by Mikhail on 27/10/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Decrement, Test_DEX) {
    test_unary(DEX_IMPLICIT,
               GetParam(),
               IMPLIED_PARAMS,
               writer_to(X),
               reader_from(X));
}


TEST_P(MOS6502_TestFixture_Decrement, Test_DEY) {
    test_unary(DEY_IMPLICIT,
               GetParam(),
               IMPLIED_PARAMS,
               writer_to(Y),
               reader_from(Y));
}


TEST_P(MOS6502_TestFixture_Decrement, Test_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_unary(DEC_ZERO_PAGE,
               GetParam(),
               UNARY_ZERO_PAGE_PARAMS,
               writer_to_zero_page(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               reader_from_zero_page(address));
}

TEST_P(MOS6502_TestFixture_Decrement, Test_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_unary(DEC_ZERO_PAGE_X,
               GetParam(),
               UNARY_ZERO_PAGE_X_PARAMS,
               writer_to_zero_page_X(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               reader_from_zero_page_X(address));
}

TEST_P(MOS6502_TestFixture_Decrement, Test_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_unary(DEC_ZERO_PAGE_X,
               GetParam(),
               UNARY_ZERO_PAGE_X_PARAMS,
               writer_to_zero_page_X(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               reader_from_zero_page_X(address));
}

TEST_P(MOS6502_TestFixture_Decrement, Test_Absolute) {
    constexpr static Word address = 0x02f0;
    test_unary(DEC_ABSOLUTE,
               GetParam(),
               UNARY_ABSOLUTE_PARAMS,
               writer_to_absolute(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               reader_from_absolute(address));
}

TEST_P(MOS6502_TestFixture_Decrement, Test_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x05;
    test_unary(DEC_ABSOLUTE_X,
               GetParam(),
               UNARY_ABSOLUTE_X_PARAMS,
               writer_to_absolute_X(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               reader_from_absolute_X(address));
}

TEST_P(MOS6502_TestFixture_Decrement, Test_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_unary(DEC_ABSOLUTE_X,
               GetParam(),
               UNARY_ABSOLUTE_X_PARAMS,
               writer_to_absolute_X(address),
            // we cannot construct a reader here, since memory[address] gives us copy of byte, not a reference to actual byte of memory
               reader_from_absolute_X(address));
}



INSTANTIATE_TEST_SUITE_P(PositiveResult, MOS6502_TestFixture_Decrement, ::testing::Values(
        UnaryOpParameters{.arg = 2, .carry = 0, .result = 1, .flagsSet = {}},
        UnaryOpParameters{.arg = 5, .carry = 0, .result = 4, .flagsSet = {}},
        UnaryOpParameters{.arg = 0x80, .carry = 0, .result = 0x7f, .flagsSet = {}}
));

INSTANTIATE_TEST_SUITE_P(NegativeResult, MOS6502_TestFixture_Decrement, ::testing::Values(
        UnaryOpParameters{.arg = 0x81, .carry = 0, .result = 0x80, .flagsSet = {Flag::NEGATIVE}},
        UnaryOpParameters{.arg = 0x93, .carry = 0, .result = 0x92, .flagsSet = {Flag::NEGATIVE}}
));



INSTANTIATE_TEST_SUITE_P(Overflow, MOS6502_TestFixture_Decrement, ::testing::Values(
        UnaryOpParameters{.arg = 0, .carry = 0, .result = 0xff, .flagsSet = {Flag::NEGATIVE}}
));
