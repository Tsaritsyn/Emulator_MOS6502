//
// Created by Mikhail on 14/09/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Transfer, LDA_Immediate) {
    test_transfer(LDA_IMMEDIATE,
                  GetParam(),
                  ExecutionParameters::transfer_immediate(),
                  writer_to_immediate(),
                  reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_transfer(LDA_ZERO_PAGE,
                GetParam(),
                ExecutionParameters::transfer_zero_page(),
                writer_to_zero_page(address),
                reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_transfer(LDA_ZERO_PAGE_X,
                GetParam(),
                ExecutionParameters::transfer_zero_page_indexed(),
                writer_to_zero_page_X(address),
                reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_transfer(LDA_ZERO_PAGE_X,
                GetParam(),
                ExecutionParameters::transfer_zero_page_indexed(),
                writer_to_zero_page_X(address),
                reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_Absolute) {
    constexpr static Word address = 0x02f0;
    test_transfer(LDA_ABSOLUTE,
                GetParam(),
                ExecutionParameters::transfer_absolute(),
                writer_to_absolute(address),
                reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x01;
    test_transfer(LDA_ABSOLUTE_X,
                GetParam(),
                ExecutionParameters::transfer_absolute_indexed(false),
                writer_to_absolute_X(address),
                reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_transfer(LDA_ABSOLUTE_X,
                GetParam(),
                ExecutionParameters::transfer_absolute_indexed(true),
                writer_to_absolute_X(address),
                reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_AbsoluteY_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x01;
    test_transfer(LDA_ABSOLUTE_Y,
                GetParam(),
                ExecutionParameters::transfer_absolute_indexed(false),
                writer_to_absolute_Y(address),
                reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_AbsoluteY_PageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x20;
    test_transfer(LDA_ABSOLUTE_Y,
                GetParam(),
                ExecutionParameters::transfer_absolute_indexed(true),
                writer_to_absolute_Y(address),
                reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_IndirectX_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x01;
    test_transfer(LDA_INDIRECT_X,
                GetParam(),
                ExecutionParameters::transfer_indirect_X(),
                writer_to_indirect_X(tableAddress, targetAddress),
                reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_IndirectX_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x40;
    test_transfer(LDA_INDIRECT_X,
                GetParam(),
                ExecutionParameters::transfer_indirect_X(),
                writer_to_indirect_X(tableAddress, targetAddress),
                reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_IndirectY_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x01;
    test_transfer(LDA_INDIRECT_Y,
                GetParam(),
                ExecutionParameters::transfer_indirect_Y(false),
                writer_to_indirect_Y(tableAddress, targetAddress),
                reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, LDA_IndirectY_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x40;
    test_transfer(LDA_INDIRECT_Y,
                GetParam(),
                ExecutionParameters::transfer_indirect_Y(true),
                writer_to_indirect_Y(tableAddress, targetAddress),
                reader_from(AC));
}

