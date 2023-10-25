//
// Created by Mikhail on 25/10/2023.
//

#include "MOS6502_TestFixture.hpp"
#include "helpers.hpp"

TEST_P(MOS6502_TestFixture_ADC, Test_Immediate) {
    test_arithmetic(ADC_IMMEDIATE, GetParam(), 2, 2, [this](Byte value) { return write_immediate(value); });
}


TEST_P(MOS6502_TestFixture_ADC, Test_ZeroPage) {
    test_arithmetic(ADC_ZERO_PAGE, GetParam(), 2, 3, [this](Byte value) { return write_zero_page(0xf0, value); });
}


TEST_P(MOS6502_TestFixture_ADC, Test_ZeroPageX_NoPageCrossing) {
    test_arithmetic(ADC_ZERO_PAGE_X, GetParam(), 2, 4, [this](Byte value) {
        X = 0x05;
        return write_zero_page_X(0xf0, value);
    });
}


TEST_P(MOS6502_TestFixture_ADC, Test_ZeroPageX_PageCrossing) {
    test_arithmetic(ADC_ZERO_PAGE_X, GetParam(), 2, 4, [this](Byte value) {
        X = 0x40;
        return write_zero_page_X(0xf0, value);
    });
}


TEST_P(MOS6502_TestFixture_ADC, Test_Absolute) {
    test_arithmetic(ADC_ABSOLUTE, GetParam(), 3, 4, [this](Byte value) { return write_absolute(0x02f0, value); });
}


TEST_P(MOS6502_TestFixture_ADC, Test_AbsoluteX_NoPageCrossing) {
    test_arithmetic(ADC_ABSOLUTE_X, GetParam(), 3, 4, [this](Byte value) {
        X = 0x01;
        return write_absolute_X(0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_ADC, Test_AbsoluteX_PageCrossing) {
    test_arithmetic(ADC_ABSOLUTE_X, GetParam(), 3, 5, [this](Byte value) {
        X = 0x20;
        return write_absolute_X(0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_ADC, Test_AbsoluteY_NoPageCrossing) {
    test_arithmetic(ADC_ABSOLUTE_Y, GetParam(), 3, 4, [this](Byte value) {
        Y = 0x01;
        return write_absolute_Y(0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_ADC, Test_AbsoluteY_PageCrossing) {
    test_arithmetic(ADC_ABSOLUTE_Y, GetParam(), 3, 5, [this](Byte value) {
        Y = 0x20;
        return write_absolute_Y(0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_ADC, Test_IndirectX_NoPageCrossing) {
    test_arithmetic(ADC_INDIRECT_X, GetParam(), 2, 6, [this](Byte value) {
        X = 0x01;
        return write_indirect_X(0xf0, 0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_ADC, Test_IndirectX_PageCrossing) {
    test_arithmetic(ADC_INDIRECT_X, GetParam(), 2, 6, [this](Byte value) {
        X = 0x40;
        return write_indirect_X(0xf0, 0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_ADC, Test_IndirectY_NoPageCrossing) {
    test_arithmetic(ADC_INDIRECT_Y, GetParam(), 2, 5, [this](Byte value) {
        Y = 0x01;
        return write_indirect_Y(0xf0, 0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_ADC, Test_IndirectY_PageCrossing) {
    test_arithmetic(ADC_INDIRECT_Y, GetParam(), 2, 6, [this](Byte value) {
        Y = 0x40;
        return write_indirect_Y(0xf0, 0x02f0, value);
    });
}



INSTANTIATE_TEST_SUITE_P(NoFlags, MOS6502_TestFixture_ADC, ::testing::Values(
    ArithmeticTestParameters{1, 0, false, 1, {}},
    ArithmeticTestParameters{0, 1, false, 1, {}},
    ArithmeticTestParameters{0, 0, true, 1, {}}
));


INSTANTIATE_TEST_SUITE_P(ZeroResult, MOS6502_TestFixture_ADC, ::testing::Values(
    ArithmeticTestParameters{0, 0, false, 0, {Flag::ZERO}},
    ArithmeticTestParameters{(Byte)-1, 0, true, 0, {Flag::ZERO, Flag::CARRY}},
    ArithmeticTestParameters{0, (Byte)-1, true, 0, {Flag::ZERO, Flag::CARRY}},
    ArithmeticTestParameters{(Byte)INT8_MIN, (Byte)INT8_MIN, false, 0, {Flag::ZERO, Flag::OVERFLOW_F, Flag::CARRY}}
));


INSTANTIATE_TEST_SUITE_P(NegativeResult, MOS6502_TestFixture_ADC, ::testing::Values(
    ArithmeticTestParameters{(Byte)-1, 0, false, (Byte)-1, {Flag::NEGATIVE}},
    ArithmeticTestParameters{0, (Byte)-1, false, (Byte)-1, {Flag::NEGATIVE}},
    ArithmeticTestParameters{(Byte)-10, (Byte)-10, false, (Byte)-20, {Flag::NEGATIVE, Flag::CARRY}},
    ArithmeticTestParameters{INT8_MAX, INT8_MAX, true, UINT8_MAX, {Flag::OVERFLOW_F, Flag::NEGATIVE}}
));


INSTANTIATE_TEST_SUITE_P(LeadingToOverflow, MOS6502_TestFixture_ADC, ::testing::Values(
    ArithmeticTestParameters{(Byte)-100, (Byte)-100, false, (Byte)-200, {Flag::OVERFLOW_F, Flag::CARRY}},
    ArithmeticTestParameters{INT8_MAX, INT8_MAX, true, UINT8_MAX, {Flag::OVERFLOW_F, Flag::NEGATIVE}},
    ArithmeticTestParameters{(Byte)INT8_MIN, (Byte)INT8_MIN, false, 0, {Flag::ZERO, Flag::OVERFLOW_F, Flag::CARRY}},
    ArithmeticTestParameters{(Byte)INT8_MIN, (Byte)INT8_MIN, true, 1, {Flag::OVERFLOW_F, Flag::CARRY}}
));


INSTANTIATE_TEST_SUITE_P(SettingCarry, MOS6502_TestFixture_ADC, ::testing::Values(
        ArithmeticTestParameters{(Byte)-1, 0, true, 0, {Flag::ZERO, Flag::CARRY}},
        ArithmeticTestParameters{0, (Byte)-1, true, 0, {Flag::ZERO, Flag::CARRY}},
        ArithmeticTestParameters{(Byte)-10, (Byte)-10, false, (Byte)-20, {Flag::NEGATIVE, Flag::CARRY}},
        ArithmeticTestParameters{(Byte)-100, (Byte)-100, false, (Byte)-200, {Flag::OVERFLOW_F, Flag::CARRY}},
        ArithmeticTestParameters{(Byte)INT8_MIN, (Byte)INT8_MIN, false, 0, {Flag::ZERO, Flag::OVERFLOW_F, Flag::CARRY}},
        ArithmeticTestParameters{(Byte)INT8_MIN, (Byte)INT8_MIN, true, 1, {Flag::OVERFLOW_F, Flag::CARRY}}
));



