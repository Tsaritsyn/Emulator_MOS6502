//
// Created by Mikhail on 29/10/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Transfer, STA_ZeroPage) {
    constexpr static Byte address = 0xf0;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STA_ZERO_PAGE,
                             GetParam(),
                             ExecutionParameters::transfer_zero_page(),
                             writer_to(AC),
                             reader_from_zero_page(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STA_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x01;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STA_ZERO_PAGE_X,
                             GetParam(),
                             ExecutionParameters::transfer_zero_page_indexed(),
                             writer_to(AC),
                             reader_from_zero_page_X(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STA_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STA_ZERO_PAGE_X,
                             GetParam(),
                             ExecutionParameters::transfer_zero_page_indexed(),
                             writer_to(AC),
                             reader_from_zero_page_X(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STA_Absolute) {
    constexpr static Word address = 0x02f0;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STA_ABSOLUTE,
                             GetParam(),
                             ExecutionParameters::transfer_absolute(),
                             writer_to(AC),
                             reader_from_absolute(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STA_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x01;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STA_ABSOLUTE_X,
                             GetParam(),
                             ExecutionParameters::transfer_absolute_indexed(true),
                             writer_to(AC),
                             reader_from_absolute_X(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STA_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STA_ABSOLUTE_X,
                             GetParam(),
                             ExecutionParameters::transfer_absolute_indexed(true),
                             writer_to(AC),
                             reader_from_absolute_X(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STA_AbsoluteY_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x01;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STA_ABSOLUTE_Y,
                             GetParam(),
                             ExecutionParameters::transfer_absolute_indexed(true),
                             writer_to(AC),
                             reader_from_absolute_Y(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STA_AbsoluteY_PageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x20;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STA_ABSOLUTE_Y,
                             GetParam(),
                             ExecutionParameters::transfer_absolute_indexed(true),
                             writer_to(AC),
                             reader_from_absolute_Y(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STA_IndirectX_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;

    X = 0x01;
    ASSERT_TRUE(set_operation_arg(tableAddress));
    ASSERT_TRUE(set_word((Byte)(tableAddress + X), targetAddress));

    test_transfer_without_flags(STA_INDIRECT_X,
                             GetParam(),
                             ExecutionParameters::transfer_indirect_X(),
                             writer_to(AC),
                             reader_from_indirect_X(targetAddress));
}

TEST_P(MOS6502_TestFixture_Transfer, STA_IndirectX_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;

    X = 0x40;
    ASSERT_TRUE(set_operation_arg(tableAddress));
    ASSERT_TRUE(set_word((Byte)(tableAddress + X), targetAddress));

    test_transfer_without_flags(STA_INDIRECT_X,
                             GetParam(),
                             ExecutionParameters::transfer_indirect_X(),
                             writer_to(AC),
                             reader_from_indirect_X(targetAddress));
}

TEST_P(MOS6502_TestFixture_Transfer, STA_IndirectY_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;

    Y = 0x01;
    ASSERT_TRUE(set_operation_arg(tableAddress));
    ASSERT_TRUE(set_word(tableAddress, targetAddress));

    test_transfer_without_flags(STA_INDIRECT_Y,
                             GetParam(),
                             ExecutionParameters::transfer_indirect_X(),
                             writer_to(AC),
                             reader_from_indirect_Y(targetAddress));
}

TEST_P(MOS6502_TestFixture_Transfer, STA_IndirectY_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;

    Y = 0x20;
    ASSERT_TRUE(set_operation_arg(tableAddress));
    ASSERT_TRUE(set_word(tableAddress, targetAddress));

    test_transfer_without_flags(STA_INDIRECT_Y,
                             GetParam(),
                             ExecutionParameters::transfer_indirect_X(),
                             writer_to(AC),
                             reader_from_indirect_Y(targetAddress));
}