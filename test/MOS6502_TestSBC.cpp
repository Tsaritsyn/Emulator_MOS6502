//
// Created by Mikhail on 25/10/2023.
//

#include "MOS6502_TestFixture.hpp"
#include "helpers.hpp"

TEST_P(MOS6502_TestFixture_SBC, Test_Immediate) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;
    ASSERT_TRUE(memory.set_byte(PC, SBC_IMMEDIATE).has_value());
    ASSERT_TRUE(memory.set_byte(PC + 1, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 2);
    EXPECT_EQ(cycle, 2);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture_SBC, Test_ZeroPage) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Byte address = 0xff;

    ASSERT_TRUE(memory.set_byte(PC, SBC_ZERO_PAGE).has_value());
    ASSERT_TRUE(memory.set_byte(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 2);
    EXPECT_EQ(cycle, 3);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture_SBC, Test_ZeroPageX_NoPageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Byte address = 0xf0;
    X = 0x05;

    ASSERT_TRUE(memory.set_byte(PC, SBC_ZERO_PAGE_X).has_value());
    ASSERT_TRUE(memory.set_byte(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address + X, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 2);
    EXPECT_EQ(cycle, 4);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture_SBC, Test_ZeroPageX_PageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Byte address = 0xf0;
    X = 0x40;

    ASSERT_TRUE(memory.set_byte(PC, SBC_ZERO_PAGE_X).has_value());
    ASSERT_TRUE(memory.set_byte(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte((Byte)(address + X), parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 2);
    EXPECT_EQ(cycle, 4);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture_SBC, Test_Absolute) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word address = 0x02f0;

    ASSERT_TRUE(memory.set_byte(PC, SBC_ABSOLUTE).has_value());
    ASSERT_TRUE(set_word(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 3);
    EXPECT_EQ(cycle, 4);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture_SBC, Test_AbsoluteX_NoPageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word address = 0x02f0;
    X = 0x01;

    ASSERT_TRUE(memory.set_byte(PC, SBC_ABSOLUTE_X).has_value());
    ASSERT_TRUE(set_word(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address + X, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 3);
    EXPECT_EQ(cycle, 4);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture_SBC, Test_AbsoluteX_PageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word address = 0x02f0;
    X = 0x20;

    ASSERT_TRUE(memory.set_byte(PC, SBC_ABSOLUTE_X).has_value());
    ASSERT_TRUE(set_word(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address + X, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 3);
    EXPECT_EQ(cycle, 5);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture_SBC, Test_AbsoluteY_NoPageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word address = 0x02f0;
    Y = 0x01;

    ASSERT_TRUE(memory.set_byte(PC, SBC_ABSOLUTE_Y).has_value());
    ASSERT_TRUE(set_word(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address + Y, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 3);
    EXPECT_EQ(cycle, 4);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture_SBC, Test_AbsoluteY_PageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word address = 0x02f0;
    Y = 0x20;

    ASSERT_TRUE(memory.set_byte(PC, SBC_ABSOLUTE_Y).has_value());
    ASSERT_TRUE(set_word(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address + Y, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 3);
    EXPECT_EQ(cycle, 5);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture_SBC, Test_IndirectX_NoPageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word targetAddress = 0x02f0;
    static constexpr Byte tableAddress = 0xf0;
    X = 0x01;

    ASSERT_TRUE(memory.set_byte(PC, SBC_INDIRECT_X).has_value());
    ASSERT_TRUE(memory.set_byte(PC + 1, tableAddress).has_value());
    ASSERT_TRUE(set_word((Byte)(tableAddress + X), targetAddress).has_value());
    ASSERT_TRUE(memory.set_byte(targetAddress, parameters.memory));

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 2);
    EXPECT_EQ(cycle, 6);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture_SBC, Test_IndirectX_PageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word targetAddress = 0x02f0;
    static constexpr Byte tableAddress = 0xf0;
    X = 0x40;

    ASSERT_TRUE(memory.set_byte(PC, SBC_INDIRECT_X).has_value());
    ASSERT_TRUE(memory.set_byte(PC + 1, tableAddress).has_value());
    ASSERT_TRUE(set_word((Byte)(tableAddress + X), targetAddress).has_value());
    ASSERT_TRUE(memory.set_byte(targetAddress, parameters.memory));

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 2);
    EXPECT_EQ(cycle, 6);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture_SBC, Test_IndirectY_NoPageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word targetAddress = 0x02f0;
    static constexpr Byte tableAddress = 0xf0;
    Y = 0x01;

    ASSERT_TRUE(memory.set_byte(PC, SBC_INDIRECT_Y).has_value());
    ASSERT_TRUE(memory.set_byte(PC + 1, tableAddress).has_value());
    ASSERT_TRUE(set_word(tableAddress, targetAddress).has_value());
    ASSERT_TRUE(memory.set_byte(targetAddress + Y, parameters.memory));

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 2);
    EXPECT_EQ(cycle, 5);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture_SBC, Test_IndirectY_PageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word targetAddress = 0x02f0;
    static constexpr Byte tableAddress = 0xf0;
    Y = 0x40;

    ASSERT_TRUE(memory.set_byte(PC, SBC_INDIRECT_Y).has_value());
    ASSERT_TRUE(memory.set_byte(PC + 1, tableAddress).has_value());
    ASSERT_TRUE(set_word(tableAddress, targetAddress).has_value());
    ASSERT_TRUE(memory.set_byte(targetAddress + Y, parameters.memory));

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 2);
    EXPECT_EQ(cycle, 6);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}



INSTANTIATE_TEST_SUITE_P(ZeroResult, MOS6502_TestFixture_SBC, ::testing::Values(
    ArithmeticTestParameters{0, 0, true, 0, {Flag::ZERO, Flag::CARRY}},
    ArithmeticTestParameters{1, 1, true, 0, {Flag::ZERO, Flag::CARRY}},
    ArithmeticTestParameters{1, 0, false, 0, {Flag::ZERO, Flag::CARRY}},
    ArithmeticTestParameters{(Byte)INT8_MIN, INT8_MAX, false, 0, {Flag::ZERO, Flag::OVERFLOW_F, Flag::CARRY}}
));


INSTANTIATE_TEST_SUITE_P(NegativeResult, MOS6502_TestFixture_SBC, ::testing::Values(
        ArithmeticTestParameters{0, 1, true, (Byte)-1, {Flag::NEGATIVE}},
        ArithmeticTestParameters{0, 0, false, (Byte)-1, {Flag::NEGATIVE}},
        ArithmeticTestParameters{0, 1, false, (Byte)-2, {Flag::NEGATIVE}},
        ArithmeticTestParameters{100, (Byte)-100, true, 200, {Flag::NEGATIVE, Flag::OVERFLOW_F}}
));


INSTANTIATE_TEST_SUITE_P(PositiveResult, MOS6502_TestFixture_SBC, ::testing::Values(
        ArithmeticTestParameters{1, 0, true, 1, {Flag::CARRY}},
        ArithmeticTestParameters{(Byte)-100, (Byte)100, true, (Byte)-200, {Flag::OVERFLOW_F, Flag::CARRY}}
));


INSTANTIATE_TEST_SUITE_P(OverflowFromPositiveToNegative, MOS6502_TestFixture_SBC, ::testing::Values(
        ArithmeticTestParameters{100, (Byte)-100, true, 200, {Flag::NEGATIVE, Flag::OVERFLOW_F}}
));


INSTANTIATE_TEST_SUITE_P(OverflowFromNegativeToPositive, MOS6502_TestFixture_SBC, ::testing::Values(
        ArithmeticTestParameters{(Byte)-100, (Byte)100, true, (Byte)-200, {Flag::OVERFLOW_F, Flag::CARRY}},
        ArithmeticTestParameters{(Byte)INT8_MIN, INT8_MAX, false, 0, {Flag::ZERO, Flag::OVERFLOW_F, Flag::CARRY}}
));


INSTANTIATE_TEST_SUITE_P(ClearingCarry, MOS6502_TestFixture_SBC, ::testing::Values(
        ArithmeticTestParameters{0, 1, true, (Byte)-1, {Flag::NEGATIVE}},
        ArithmeticTestParameters{0, 0, false, (Byte)-1, {Flag::NEGATIVE}},
        ArithmeticTestParameters{0, 1, false, (Byte)-2, {Flag::NEGATIVE}},
        ArithmeticTestParameters{100, (Byte)-100, true, 200, {Flag::NEGATIVE, Flag::OVERFLOW_F}}
));
