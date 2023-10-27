//
// Created by Mikhail on 26/10/2023.
//

#include "MOS6502_TestFixture.hpp"
#include "helpers.hpp"


TEST_P(MOS6502_TestFixture_ORA, Test_Immediate) {
    test_binary(ORA_IMMEDIATE,
                GetParam(),
                ExecutionParameters::binary_immediate(),
                writer_to(AC),
                writer_to_immediate(),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ORA, Test_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_binary(ORA_ZERO_PAGE,
                GetParam(),
                ExecutionParameters::binary_zero_page(),
                writer_to(AC),
                writer_to_zero_page(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ORA, Test_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_binary(ORA_ZERO_PAGE_X,
                GetParam(),
                ExecutionParameters::binary_zero_page_indexed(),
                writer_to(AC),
                writer_to_zero_page_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ORA, Test_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_binary(ORA_ZERO_PAGE_X,
                GetParam(),
                ExecutionParameters::binary_zero_page_indexed(),
                writer_to(AC),
                writer_to_zero_page_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ORA, Test_Absolute) {
    constexpr static Word address = 0x02f0;
    test_binary(ORA_ABSOLUTE,
                GetParam(),
                ExecutionParameters::binary_absolute(),
                writer_to(AC),
                writer_to_absolute(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ORA, Test_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x01;
    test_binary(ORA_ABSOLUTE_X,
                GetParam(),
                ExecutionParameters::binary_absolute_indexed(false),
                writer_to(AC),
                writer_to_absolute_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ORA, Test_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_binary(ORA_ABSOLUTE_X,
                GetParam(),
                ExecutionParameters::binary_absolute_indexed(true),
                writer_to(AC),
                writer_to_absolute_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ORA, Test_AbsoluteY_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x01;
    test_binary(ORA_ABSOLUTE_Y,
                GetParam(),
                ExecutionParameters::binary_absolute_indexed(false),
                writer_to(AC),
                writer_to_absolute_Y(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ORA, Test_AbsoluteY_PageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x20;
    test_binary(ORA_ABSOLUTE_Y,
                GetParam(),
                ExecutionParameters::binary_absolute_indexed(true),
                writer_to(AC),
                writer_to_absolute_Y(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ORA, Test_IndirectX_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x01;
    test_binary(ORA_INDIRECT_X,
                GetParam(),
                ExecutionParameters::binary_indirect_X(),
                writer_to(AC),
                writer_to_indirect_X(tableAddress, targetAddress),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ORA, Test_IndirectX_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x40;
    test_binary(ORA_INDIRECT_X,
                GetParam(),
                ExecutionParameters::binary_indirect_X(),
                writer_to(AC),
                writer_to_indirect_X(tableAddress, targetAddress),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ORA, Test_IndirectY_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x01;
    test_binary(ORA_INDIRECT_Y,
                GetParam(),
                ExecutionParameters::binary_indirect_Y(false),
                writer_to(AC),
                writer_to_indirect_Y(tableAddress, targetAddress),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ORA, Test_IndirectY_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x40;
    test_binary(ORA_INDIRECT_Y,
                GetParam(),
                ExecutionParameters::binary_indirect_Y(true),
                writer_to(AC),
                writer_to_indirect_Y(tableAddress, targetAddress),
                reader_from(AC));
}



INSTANTIATE_TEST_SUITE_P(WithZero, MOS6502_TestFixture_OR, ::testing::Values(
        BinaryOpParameters{0, 0, false, 0, {Flag::ZERO}},
        BinaryOpParameters{1, 0, false, 1, {}},
        BinaryOpParameters{2, 0, false, 2, {}},
        BinaryOpParameters{0x37, 0, false, 0x37, {}},
        BinaryOpParameters{(Byte)-1, 0, false, (Byte)-1, {Flag::NEGATIVE}},
        BinaryOpParameters{(Byte)-100, 0, false, (Byte)-100, {Flag::NEGATIVE}}
));

INSTANTIATE_TEST_SUITE_P(WithAllOnes, MOS6502_TestFixture_OR, ::testing::Values(
        BinaryOpParameters{1, 0xff, false, 0xff, {Flag::NEGATIVE}},
        BinaryOpParameters{2, 0xff, false, 0xff, {Flag::NEGATIVE}},
        BinaryOpParameters{0x37, 0xff, false, 0xff, {Flag::NEGATIVE}},
        BinaryOpParameters{(Byte)-1, 0xff, false, 0xff, {Flag::NEGATIVE}},
        BinaryOpParameters{(Byte)-100, 0xff, false, 0xff, {Flag::NEGATIVE}}
));

INSTANTIATE_TEST_SUITE_P(ArbitraryNumbers, MOS6502_TestFixture_OR, ::testing::Values(
        BinaryOpParameters{0x59, 0x3c, false, 0x7d, {}},
        BinaryOpParameters{0x82, 0x5e, false, 0xde, {Flag::NEGATIVE}},
        BinaryOpParameters{0xd7, 0x2f, false, 0xff, {Flag::NEGATIVE}},
        BinaryOpParameters{0xab, 0x7d, false, 0xff, {Flag::NEGATIVE}},
        BinaryOpParameters{0xf3, 0xe9, false, 0xfb, {Flag::NEGATIVE}}
));

