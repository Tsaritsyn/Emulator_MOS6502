//
// Created by Mikhail on 25/10/2023.
//

#include "MOS6502_TestFixture.hpp"
#include "helpers.hpp"

TEST_P(MOS6502_TestFixture_SBC, Test_Immediate) {
    test_arithmetic(SBC_IMMEDIATE, GetParam(), 2, 2, [this](Byte value) { return write_immediate(value); });
}


TEST_P(MOS6502_TestFixture_SBC, Test_ZeroPage) {
    test_arithmetic(SBC_ZERO_PAGE, GetParam(), 2, 3, [this](Byte value) { return write_zero_page(0xf0, value); });
}


TEST_P(MOS6502_TestFixture_SBC, Test_ZeroPageX_NoPageCrossing) {
    test_arithmetic(SBC_ZERO_PAGE_X, GetParam(), 2, 4, [this](Byte value) {
        X = 0x05;
        return write_zero_page_X(0xf0, value);
    });
}


TEST_P(MOS6502_TestFixture_SBC, Test_ZeroPageX_PageCrossing) {
    test_arithmetic(SBC_ZERO_PAGE_X, GetParam(), 2, 4, [this](Byte value) {
        X = 0x40;
        return write_zero_page_X(0xf0, value);
    });
}


TEST_P(MOS6502_TestFixture_SBC, Test_Absolute) {
    test_arithmetic(SBC_ABSOLUTE, GetParam(), 3, 4, [this](Byte value) { return write_absolute(0x02f0, value); });
}


TEST_P(MOS6502_TestFixture_SBC, Test_AbsoluteX_NoPageCrossing) {
    test_arithmetic(SBC_ABSOLUTE_X, GetParam(), 3, 4, [this](Byte value) {
        X = 0x01;
        return write_absolute_X(0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_SBC, Test_AbsoluteX_PageCrossing) {
    test_arithmetic(SBC_ABSOLUTE_X, GetParam(), 3, 5, [this](Byte value) {
        X = 0x20;
        return write_absolute_X(0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_SBC, Test_AbsoluteY_NoPageCrossing) {
    test_arithmetic(SBC_ABSOLUTE_Y, GetParam(), 3, 4, [this](Byte value) {
        Y = 0x01;
        return write_absolute_Y(0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_SBC, Test_AbsoluteY_PageCrossing) {
    test_arithmetic(SBC_ABSOLUTE_Y, GetParam(), 3, 5, [this](Byte value) {
        Y = 0x20;
        return write_absolute_Y(0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_SBC, Test_IndirectX_NoPageCrossing) {
    test_arithmetic(SBC_INDIRECT_X, GetParam(), 2, 6, [this](Byte value) {
        X = 0x01;
        return write_indirect_X(0xf0, 0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_SBC, Test_IndirectX_PageCrossing) {
    test_arithmetic(SBC_INDIRECT_X, GetParam(), 2, 6, [this](Byte value) {
        X = 0x40;
        return write_indirect_X(0xf0, 0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_SBC, Test_IndirectY_NoPageCrossing) {
    test_arithmetic(SBC_INDIRECT_Y, GetParam(), 2, 5, [this](Byte value) {
        Y = 0x01;
        return write_indirect_Y(0xf0, 0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_SBC, Test_IndirectY_PageCrossing) {
    test_arithmetic(SBC_INDIRECT_Y, GetParam(), 2, 6, [this](Byte value) {
        Y = 0x40;
        return write_indirect_Y(0xf0, 0x02f0, value);
    });
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
