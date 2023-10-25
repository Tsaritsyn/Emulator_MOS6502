//
// Created by Mikhail on 25/10/2023.
//

#include "MOS6502_TestFixture.hpp"
#include "helpers.hpp"

TEST_P(MOS6502_TestFixture, Test_Immediate) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;
    ASSERT_TRUE(memory.set_byte(PC, ADC_IMMEDIATE).has_value());
    ASSERT_TRUE(memory.set_byte(PC + 1, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 2);
    EXPECT_EQ(cycle, 2);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture, Test_ZeroPage) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Byte address = 0xff;

    ASSERT_TRUE(memory.set_byte(PC, ADC_ZERO_PAGE).has_value());
    ASSERT_TRUE(memory.set_byte(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 2);
    EXPECT_EQ(cycle, 3);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture, Test_ZeroPageX_NoPageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Byte address = 0xf0;
    X = 0x05;

    ASSERT_TRUE(memory.set_byte(PC, ADC_ZERO_PAGE_X).has_value());
    ASSERT_TRUE(memory.set_byte(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address + X, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 2);
    EXPECT_EQ(cycle, 4);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture, Test_ZeroPageX_PageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Byte address = 0xf0;
    X = 0x40;

    ASSERT_TRUE(memory.set_byte(PC, ADC_ZERO_PAGE_X).has_value());
    ASSERT_TRUE(memory.set_byte(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte((Byte)(address + X), parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 2);
    EXPECT_EQ(cycle, 4);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture, Test_Absolute) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word address = 0x02f0;

    ASSERT_TRUE(memory.set_byte(PC, ADC_ABSOLUTE).has_value());
    ASSERT_TRUE(set_word(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 3);
    EXPECT_EQ(cycle, 4);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture, Test_AbsoluteX_NoPageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word address = 0x02f0;
    X = 0x01;

    ASSERT_TRUE(memory.set_byte(PC, ADC_ABSOLUTE_X).has_value());
    ASSERT_TRUE(set_word(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address + X, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 3);
    EXPECT_EQ(cycle, 4);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture, Test_AbsoluteX_PageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word address = 0x02f0;
    X = 0x20;

    ASSERT_TRUE(memory.set_byte(PC, ADC_ABSOLUTE_X).has_value());
    ASSERT_TRUE(set_word(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address + X, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 3);
    EXPECT_EQ(cycle, 5);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture, Test_AbsoluteY_NoPageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word address = 0x02f0;
    Y = 0x01;

    ASSERT_TRUE(memory.set_byte(PC, ADC_ABSOLUTE_Y).has_value());
    ASSERT_TRUE(set_word(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address + Y, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 3);
    EXPECT_EQ(cycle, 4);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture, Test_AbsoluteY_PageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word address = 0x02f0;
    Y = 0x20;

    ASSERT_TRUE(memory.set_byte(PC, ADC_ABSOLUTE_Y).has_value());
    ASSERT_TRUE(set_word(PC + 1, address).has_value());
    ASSERT_TRUE(memory.set_byte(address + Y, parameters.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, parameters.result);
    EXPECT_EQ(PC, 3);
    EXPECT_EQ(cycle, 5);
    EXPECT_EQ(SR, status_from_flags(parameters.flagsSet));
}


TEST_P(MOS6502_TestFixture, Test_IndirectX_NoPageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word targetAddress = 0x02f0;
    static constexpr Byte tableAddress = 0xf0;
    X = 0x01;

    ASSERT_TRUE(memory.set_byte(PC, ADC_INDIRECT_X).has_value());
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


TEST_P(MOS6502_TestFixture, Test_IndirectX_PageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word targetAddress = 0x02f0;
    static constexpr Byte tableAddress = 0xf0;
    X = 0x40;

    ASSERT_TRUE(memory.set_byte(PC, ADC_INDIRECT_X).has_value());
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


TEST_P(MOS6502_TestFixture, Test_IndirectY_NoPageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word targetAddress = 0x02f0;
    static constexpr Byte tableAddress = 0xf0;
    Y = 0x01;

    ASSERT_TRUE(memory.set_byte(PC, ADC_INDIRECT_Y).has_value());
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


TEST_P(MOS6502_TestFixture, Test_IndirectY_PageCrossing) {
    auto parameters = GetParam();

    AC = parameters.AC;
    SR[Emulator::Flag::CARRY] = parameters.carry;

    static constexpr Word targetAddress = 0x02f0;
    static constexpr Byte tableAddress = 0xf0;
    Y = 0x40;

    ASSERT_TRUE(memory.set_byte(PC, ADC_INDIRECT_Y).has_value());
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



INSTANTIATE_TEST_SUITE_P(ADC_Tests, MOS6502_TestFixture, ::testing::Values(
    TestParameters{0, 0, false, 0, {Flag::ZERO}},
    TestParameters{1, 0, false, 1, {}},
    TestParameters{0, 1, false, 1, {}},
    TestParameters{0, 0, true, 1, {}},
    TestParameters{(Byte)-1, 0, false, (Byte)-1, {Flag::NEGATIVE}},
    TestParameters{0, (Byte)-1, false, (Byte)-1, {Flag::NEGATIVE}},
    TestParameters{(Byte)-1, 0, true, 0, {Flag::ZERO, Flag::CARRY}},
    TestParameters{0, (Byte)-1, true, 0, {Flag::ZERO, Flag::CARRY}},
    TestParameters{(Byte)-10, (Byte)-10, false, (Byte)-20, {Flag::NEGATIVE, Flag::CARRY}},
    TestParameters{(Byte)-100, (Byte)-100, false, (Byte)-200, {Flag::OVERFLOW_F, Flag::CARRY}},
    TestParameters{INT8_MAX, INT8_MAX, true, UINT8_MAX, {Flag::OVERFLOW_F, Flag::NEGATIVE}},
    TestParameters{(Byte)INT8_MIN, (Byte)INT8_MIN, false, 0, {Flag::ZERO, Flag::OVERFLOW_F, Flag::CARRY}},
    TestParameters{(Byte)INT8_MIN, (Byte)INT8_MIN, true, 1, {Flag::OVERFLOW_F, Flag::CARRY}}
));



