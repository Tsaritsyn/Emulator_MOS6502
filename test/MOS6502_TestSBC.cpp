//
// Created by Mikhail on 25/10/2023.
//

#include "MOS6502_TestFixture.hpp"
#include "helpers.hpp"

TEST_P(MOS6502_TestFixture_SBC, Test_Immediate) {
    test_binary(SBC_IMMEDIATE,
                GetParam(),
                ExecutionParameters::binary_immediate(),
                writer_to(AC),
                writer_to_immediate(),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_SBC, Test_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_binary(SBC_ZERO_PAGE,
                GetParam(),
                ExecutionParameters::binary_zero_page(),
                writer_to(AC),
                writer_to_zero_page(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_SBC, Test_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_binary(SBC_ZERO_PAGE_X,
                GetParam(),
                ExecutionParameters::binary_zero_page_indexed(),
                writer_to(AC),
                writer_to_zero_page_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_SBC, Test_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_binary(SBC_ZERO_PAGE_X,
                GetParam(),
                ExecutionParameters::binary_zero_page_indexed(),
                writer_to(AC),
                writer_to_zero_page_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_SBC, Test_Absolute) {
    constexpr static Word address = 0x02f0;
    test_binary(SBC_ABSOLUTE,
                GetParam(),
                ExecutionParameters::binary_absolute(),
                writer_to(AC),
                writer_to_absolute(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_SBC, Test_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x01;
    test_binary(SBC_ABSOLUTE_X,
                GetParam(),
                ExecutionParameters::binary_absolute_indexed(false),
                writer_to(AC),
                writer_to_absolute_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_SBC, Test_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_binary(SBC_ABSOLUTE_X,
                GetParam(),
                ExecutionParameters::binary_absolute_indexed(true),
                writer_to(AC),
                writer_to_absolute_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_SBC, Test_AbsoluteY_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x01;
    test_binary(SBC_ABSOLUTE_Y,
                GetParam(),
                ExecutionParameters::binary_absolute_indexed(false),
                writer_to(AC),
                writer_to_absolute_Y(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_SBC, Test_AbsoluteY_PageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x20;
    test_binary(SBC_ABSOLUTE_Y,
                GetParam(),
                ExecutionParameters::binary_absolute_indexed(true),
                writer_to(AC),
                writer_to_absolute_Y(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_SBC, Test_IndirectX_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x01;
    test_binary(SBC_INDIRECT_X,
                GetParam(),
                ExecutionParameters::binary_indirect_X(),
                writer_to(AC),
                writer_to_indirect_X(tableAddress, targetAddress),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_SBC, Test_IndirectX_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x40;
    test_binary(SBC_INDIRECT_X,
                GetParam(),
                ExecutionParameters::binary_indirect_X(),
                writer_to(AC),
                writer_to_indirect_X(tableAddress, targetAddress),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_SBC, Test_IndirectY_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x01;
    test_binary(SBC_INDIRECT_Y,
                GetParam(),
                ExecutionParameters::binary_indirect_Y(false),
                writer_to(AC),
                writer_to_indirect_Y(tableAddress, targetAddress),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_SBC, Test_IndirectY_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x40;
    test_binary(SBC_INDIRECT_Y,
                GetParam(),
                ExecutionParameters::binary_indirect_Y(true),
                writer_to(AC),
                writer_to_indirect_Y(tableAddress, targetAddress),
                reader_from(AC));
}



INSTANTIATE_TEST_SUITE_P(ZeroResult, MOS6502_TestFixture_SBC, ::testing::Values(
        BinaryOpParameters{0, 0, true, 0, {Flag::ZERO, Flag::CARRY}},
        BinaryOpParameters{1, 1, true, 0, {Flag::ZERO, Flag::CARRY}},
        BinaryOpParameters{1, 0, false, 0, {Flag::ZERO, Flag::CARRY}},
        BinaryOpParameters{(Byte)INT8_MIN, INT8_MAX, false, 0, {Flag::ZERO, Flag::OVERFLOW_F, Flag::CARRY}}
));


INSTANTIATE_TEST_SUITE_P(NegativeResult, MOS6502_TestFixture_SBC, ::testing::Values(
        BinaryOpParameters{0, 1, true, (Byte)-1, {Flag::NEGATIVE}},
        BinaryOpParameters{0, 0, false, (Byte)-1, {Flag::NEGATIVE}},
        BinaryOpParameters{0, 1, false, (Byte)-2, {Flag::NEGATIVE}},
        BinaryOpParameters{100, (Byte)-100, true, 200, {Flag::NEGATIVE, Flag::OVERFLOW_F}}
));


INSTANTIATE_TEST_SUITE_P(PositiveResult, MOS6502_TestFixture_SBC, ::testing::Values(
        BinaryOpParameters{1, 0, true, 1, {Flag::CARRY}},
        BinaryOpParameters{(Byte)-100, (Byte)100, true, (Byte)-200, {Flag::OVERFLOW_F, Flag::CARRY}}
));


INSTANTIATE_TEST_SUITE_P(OverflowFromPositiveToNegative, MOS6502_TestFixture_SBC, ::testing::Values(
        BinaryOpParameters{100, (Byte)-100, true, 200, {Flag::NEGATIVE, Flag::OVERFLOW_F}}
));


INSTANTIATE_TEST_SUITE_P(OverflowFromNegativeToPositive, MOS6502_TestFixture_SBC, ::testing::Values(
        BinaryOpParameters{(Byte)-100, (Byte)100, true, (Byte)-200, {Flag::OVERFLOW_F, Flag::CARRY}},
        BinaryOpParameters{(Byte)INT8_MIN, INT8_MAX, false, 0, {Flag::ZERO, Flag::OVERFLOW_F, Flag::CARRY}}
));


INSTANTIATE_TEST_SUITE_P(ClearingCarry, MOS6502_TestFixture_SBC, ::testing::Values(
        BinaryOpParameters{0, 1, true, (Byte)-1, {Flag::NEGATIVE}},
        BinaryOpParameters{0, 0, false, (Byte)-1, {Flag::NEGATIVE}},
        BinaryOpParameters{0, 1, false, (Byte)-2, {Flag::NEGATIVE}},
        BinaryOpParameters{100, (Byte)-100, true, 200, {Flag::NEGATIVE, Flag::OVERFLOW_F}}
));
