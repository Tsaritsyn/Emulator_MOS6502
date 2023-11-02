//
// Created by Mikhail on 26/10/2023.
//

#include "MOS6502_TestFixture.hpp"
#include "helpers.hpp"


TEST_P(MOS6502_TestFixture_EOR, Test_Immediate) {
    test_binary(EOR_IMMEDIATE,
                GetParam(),
                BINARY_IMMEDIATE_PARAMS,
                writer_to(AC),
                writer_to_immediate(),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_EOR, Test_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_binary(EOR_ZERO_PAGE,
                GetParam(),
                BINARY_ZERO_PAGE_PARAMS,
                writer_to(AC),
                writer_to_zero_page(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_EOR, Test_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_binary(EOR_ZERO_PAGE_X,
                GetParam(),
                BINARY_ZERO_PAGE_X_PARAMS,
                writer_to(AC),
                writer_to_zero_page_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_EOR, Test_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_binary(EOR_ZERO_PAGE_X,
                GetParam(),
                BINARY_ZERO_PAGE_X_PARAMS,
                writer_to(AC),
                writer_to_zero_page_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_EOR, Test_Absolute) {
    constexpr static Word address = 0x02f0;
    test_binary(EOR_ABSOLUTE,
                GetParam(),
                BINARY_ABSOLUTE_PARAMS,
                writer_to(AC),
                writer_to_absolute(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_EOR, Test_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x01;
    test_binary(EOR_ABSOLUTE_X,
                GetParam(),
                BINARY_ABSOLUTE_X_NO_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_absolute_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_EOR, Test_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_binary(EOR_ABSOLUTE_X,
                GetParam(),
                BINARY_ABSOLUTE_X_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_absolute_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_EOR, Test_AbsoluteY_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x01;
    test_binary(EOR_ABSOLUTE_Y,
                GetParam(),
                BINARY_ABSOLUTE_X_NO_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_absolute_Y(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_EOR, Test_AbsoluteY_PageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x20;
    test_binary(EOR_ABSOLUTE_Y,
                GetParam(),
                BINARY_ABSOLUTE_X_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_absolute_Y(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_EOR, Test_IndirectX_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x01;
    test_binary(EOR_INDIRECT_X,
                GetParam(),
                BINARY_INDIRECT_X_PARAMS,
                writer_to(AC),
                writer_to_indirect_X(tableAddress, targetAddress),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_EOR, Test_IndirectX_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x40;
    test_binary(EOR_INDIRECT_X,
                GetParam(),
                BINARY_INDIRECT_X_PARAMS,
                writer_to(AC),
                writer_to_indirect_X(tableAddress, targetAddress),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_EOR, Test_IndirectY_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x01;
    test_binary(EOR_INDIRECT_Y,
                GetParam(),
                BINARY_INDIRECT_Y_NO_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_indirect_Y(tableAddress, targetAddress),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_EOR, Test_IndirectY_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x40;
    test_binary(EOR_INDIRECT_Y,
                GetParam(),
                BINARY_INDIRECT_Y_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_indirect_Y(tableAddress, targetAddress),
                reader_from(AC));
}



INSTANTIATE_TEST_SUITE_P(WithZero, MOS6502_TestFixture_EOR, ::testing::Values(
        BinaryOpParameters{0, 0, false, 0, {Flag::ZERO}},
        BinaryOpParameters{1, 0, false, 1, {}},
        BinaryOpParameters{2, 0, false, 2, {}},
        BinaryOpParameters{0x37, 0, false, 0x37, {}},
        BinaryOpParameters{(Byte)-1, 0, false, (Byte)-1, {Flag::NEGATIVE}},
        BinaryOpParameters{(Byte)-100, 0, false, (Byte)-100, {Flag::NEGATIVE}}
));

INSTANTIATE_TEST_SUITE_P(WithAllOnes, MOS6502_TestFixture_EOR, ::testing::Values(
        BinaryOpParameters{1, 0xff, false, 0xfe, {Flag::NEGATIVE}},
        BinaryOpParameters{2, 0xff, false, 0xfd, {Flag::NEGATIVE}},
        BinaryOpParameters{0x37, 0xff, false, 0xc8, {Flag::NEGATIVE}},
        BinaryOpParameters{0xff, 0xff, false, 0, {Flag::ZERO}}
));

INSTANTIATE_TEST_SUITE_P(ArbitraryNumbers, MOS6502_TestFixture_EOR, ::testing::Values(
        BinaryOpParameters{0x59, 0x3c, false, 0x65, {}},
        BinaryOpParameters{0x82, 0x5e, false, 0xdc, {Flag::NEGATIVE}},
        BinaryOpParameters{0xd7, 0x2f, false, 0xf8, {Flag::NEGATIVE}},
        BinaryOpParameters{0xab, 0x7d, false, 0xd6, {Flag::NEGATIVE}},
        BinaryOpParameters{0xf3, 0xe9, false, 0x1a, {}}
));

