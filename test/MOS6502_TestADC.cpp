//
// Created by Mikhail on 25/10/2023.
//

#include "MOS6502_TestFixture.hpp"
#include "helpers.hpp"

TEST_P(MOS6502_TestFixture_ADC, Test_Immediate) {
    test_binary(ADC_IMMEDIATE,
                GetParam(),
                BINARY_IMMEDIATE_PARAMS,
                writer_to(AC),
                writer_to_immediate(),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ADC, Test_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_binary(ADC_ZERO_PAGE,
                GetParam(),
                BINARY_ZERO_PAGE_PARAMS,
                writer_to(AC),
                writer_to_zero_page(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ADC, Test_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_binary(ADC_ZERO_PAGE_X,
                GetParam(),
                BINARY_ZERO_PAGE_X_PARAMS,
                writer_to(AC),
                writer_to_zero_page_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ADC, Test_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_binary(ADC_ZERO_PAGE_X,
                GetParam(),
                BINARY_ZERO_PAGE_X_PARAMS,
                writer_to(AC),
                writer_to_zero_page_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ADC, Test_Absolute) {
    constexpr static Word address = 0x02f0;
    test_binary(ADC_ABSOLUTE,
                GetParam(),
                BINARY_ABSOLUTE_PARAMS,
                writer_to(AC),
                writer_to_absolute(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ADC, Test_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x01;
    test_binary(ADC_ABSOLUTE_X,
                GetParam(),
                BINARY_ABSOLUTE_X_NO_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_absolute_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ADC, Test_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_binary(ADC_ABSOLUTE_X,
                GetParam(),
                BINARY_ABSOLUTE_X_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_absolute_X(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ADC, Test_AbsoluteY_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x01;
    test_binary(ADC_ABSOLUTE_Y,
                GetParam(),
                BINARY_ABSOLUTE_X_NO_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_absolute_Y(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ADC, Test_AbsoluteY_PageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x20;
    test_binary(ADC_ABSOLUTE_Y,
                GetParam(),
                BINARY_ABSOLUTE_X_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_absolute_Y(address),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ADC, Test_IndirectX_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x01;
    test_binary(ADC_INDIRECT_X,
                GetParam(),
                BINARY_INDIRECT_X_PARAMS,
                writer_to(AC),
                writer_to_indirect_X(tableAddress, targetAddress),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ADC, Test_IndirectX_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x40;
    test_binary(ADC_INDIRECT_X,
                GetParam(),
                BINARY_INDIRECT_X_PARAMS,
                writer_to(AC),
                writer_to_indirect_X(tableAddress, targetAddress),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ADC, Test_IndirectY_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x01;
    test_binary(ADC_INDIRECT_Y,
                GetParam(),
                BINARY_INDIRECT_Y_NO_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_indirect_Y(tableAddress, targetAddress),
                reader_from(AC));
}


TEST_P(MOS6502_TestFixture_ADC, Test_IndirectY_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x40;
    test_binary(ADC_INDIRECT_Y,
                GetParam(),
                BINARY_INDIRECT_Y_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_indirect_Y(tableAddress, targetAddress),
                reader_from(AC));
}



INSTANTIATE_TEST_SUITE_P(NoFlags, MOS6502_TestFixture_ADC, ::testing::Values(
        BinaryOpParameters{1, 0, false, 1, {}},
        BinaryOpParameters{0, 1, false, 1, {}},
        BinaryOpParameters{0, 0, true, 1, {}}
));


INSTANTIATE_TEST_SUITE_P(ZeroResult, MOS6502_TestFixture_ADC, ::testing::Values(
        BinaryOpParameters{0, 0, false, 0, {Flag::ZERO}},
        BinaryOpParameters{static_cast<Byte>(-1), 0, true, 0, {Flag::ZERO, Flag::CARRY}},
        BinaryOpParameters{0, static_cast<Byte>(-1), true, 0, {Flag::ZERO, Flag::CARRY}},
        BinaryOpParameters{static_cast<Byte>(INT8_MIN), static_cast<Byte>(INT8_MIN), false, 0, {Flag::ZERO, Flag::OVERFLOW_F, Flag::CARRY}}
));


INSTANTIATE_TEST_SUITE_P(NegativeResult, MOS6502_TestFixture_ADC, ::testing::Values(
        BinaryOpParameters{static_cast<Byte>(-1), 0, false, static_cast<Byte>(-1), {Flag::NEGATIVE}},
        BinaryOpParameters{0, static_cast<Byte>(-1), false, static_cast<Byte>(-1), {Flag::NEGATIVE}},
        BinaryOpParameters{static_cast<Byte>(-10), static_cast<Byte>(-10), false, static_cast<Byte>(-20), {Flag::NEGATIVE, Flag::CARRY}},
        BinaryOpParameters{INT8_MAX, INT8_MAX, true, UINT8_MAX, {Flag::OVERFLOW_F, Flag::NEGATIVE}}
));


INSTANTIATE_TEST_SUITE_P(LeadingToOverflow, MOS6502_TestFixture_ADC, ::testing::Values(
        BinaryOpParameters{static_cast<Byte>(-100), static_cast<Byte>(-100), false, static_cast<Byte>(-200), {Flag::OVERFLOW_F, Flag::CARRY}},
        BinaryOpParameters{INT8_MAX, INT8_MAX, true, UINT8_MAX, {Flag::OVERFLOW_F, Flag::NEGATIVE}},
        BinaryOpParameters{static_cast<Byte>(INT8_MIN), static_cast<Byte>(INT8_MIN), false, 0, {Flag::ZERO, Flag::OVERFLOW_F, Flag::CARRY}},
        BinaryOpParameters{static_cast<Byte>(INT8_MIN), static_cast<Byte>(INT8_MIN), true, 1, {Flag::OVERFLOW_F, Flag::CARRY}}
));


INSTANTIATE_TEST_SUITE_P(SettingCarry, MOS6502_TestFixture_ADC, ::testing::Values(
        BinaryOpParameters{static_cast<Byte>(-1), 0, true, 0, {Flag::ZERO, Flag::CARRY}},
        BinaryOpParameters{0, static_cast<Byte>(-1), true, 0, {Flag::ZERO, Flag::CARRY}},
        BinaryOpParameters{static_cast<Byte>(-10), static_cast<Byte>(-10), false, static_cast<Byte>(-20), {Flag::NEGATIVE, Flag::CARRY}},
        BinaryOpParameters{static_cast<Byte>(-100), static_cast<Byte>(-100), false, static_cast<Byte>(-200), {Flag::OVERFLOW_F, Flag::CARRY}},
        BinaryOpParameters{static_cast<Byte>INT8_MIN, static_cast<Byte>INT8_MIN, false, 0, {Flag::ZERO, Flag::OVERFLOW_F, Flag::CARRY}},
        BinaryOpParameters{static_cast<Byte>INT8_MIN, static_cast<Byte>INT8_MIN, true, 1, {Flag::OVERFLOW_F, Flag::CARRY}}
));



