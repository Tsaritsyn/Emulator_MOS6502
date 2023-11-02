//
// Created by Mikhail on 14/09/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Transfer, LDA_Immediate) {
    test_transfer_with_flags(LDA_IMMEDIATE,
                             GetParam(),
                             TRANSFER_IMMEDIATE_PARAMS,
                             writer_to_immediate(),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_transfer_with_flags(LDA_ZERO_PAGE,
                             GetParam(),
                             TRANSFER_ZERO_PAGE_PARAMS,
                             writer_to_zero_page(address),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_transfer_with_flags(LDA_ZERO_PAGE_X,
                             GetParam(),
                             TRANSFER_ZERO_PAGE_X_PARAMS,
                             writer_to_zero_page_X(address),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_transfer_with_flags(LDA_ZERO_PAGE_X,
                             GetParam(),
                             TRANSFER_ZERO_PAGE_X_PARAMS,
                             writer_to_zero_page_X(address),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_Absolute) {
    constexpr static Word address = 0x02f0;
    test_transfer_with_flags(LDA_ABSOLUTE,
                             GetParam(),
                             TRANSFER_ABSOLUTE_PARAMS,
                             writer_to_absolute(address),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x01;
    test_transfer_with_flags(LDA_ABSOLUTE_X,
                             GetParam(),
                             TRANSFER_ABSOLUTE_X_NO_PAGE_CROSSING_PARAMS,
                             writer_to_absolute_X(address),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_transfer_with_flags(LDA_ABSOLUTE_X,
                             GetParam(),
                             TRANSFER_ABSOLUTE_X_PAGE_CROSSING_PARAMS,
                             writer_to_absolute_X(address),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_AbsoluteY_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x01;
    test_transfer_with_flags(LDA_ABSOLUTE_Y,
                             GetParam(),
                             TRANSFER_ABSOLUTE_X_NO_PAGE_CROSSING_PARAMS,
                             writer_to_absolute_Y(address),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_AbsoluteY_PageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x20;
    test_transfer_with_flags(LDA_ABSOLUTE_Y,
                             GetParam(),
                             TRANSFER_ABSOLUTE_X_PAGE_CROSSING_PARAMS,
                             writer_to_absolute_Y(address),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_IndirectX_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x01;
    test_transfer_with_flags(LDA_INDIRECT_X,
                             GetParam(),
                             TRANSFER_INDIRECT_X_PARAMS,
                             writer_to_indirect_X(tableAddress, targetAddress),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_IndirectX_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x40;
    test_transfer_with_flags(LDA_INDIRECT_X,
                             GetParam(),
                             TRANSFER_INDIRECT_X_PARAMS,
                             writer_to_indirect_X(tableAddress, targetAddress),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_IndirectY_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x01;
    test_transfer_with_flags(LDA_INDIRECT_Y,
                             GetParam(),
                             TRANSFER_INDIRECT_Y_NO_PAGE_CROSSING_PARAMS,
                             writer_to_indirect_Y(tableAddress, targetAddress),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_IndirectY_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x40;
    test_transfer_with_flags(LDA_INDIRECT_Y,
                             GetParam(),
                             TRANSFER_INDIRECT_Y_PAGE_CROSSING_PARAMS,
                             writer_to_indirect_Y(tableAddress, targetAddress),
                             reader_from(AC));
}

INSTANTIATE_TEST_SUITE_P(AllTests, MOS6502_TestFixture_Transfer, ::testing::Values(0, 1, 10, 100, (Byte)-10, 255));

