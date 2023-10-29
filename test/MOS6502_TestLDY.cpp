//
// Created by Mikhail on 16/09/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Transfer, LDY_Immediate) {
    test_transfer(LDY_IMMEDIATE,
                  GetParam(),
                  ExecutionParameters::transfer_immediate(),
                  writer_to_immediate(),
                  reader_from(Y));
}

TEST_P(MOS6502_TestFixture_Transfer, LDY_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_transfer(LDY_ZERO_PAGE,
                  GetParam(),
                  ExecutionParameters::transfer_zero_page(),
                  writer_to_zero_page(address),
                  reader_from(Y));
}

TEST_P(MOS6502_TestFixture_Transfer, LDY_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_transfer(LDY_ZERO_PAGE_X,
                  GetParam(),
                  ExecutionParameters::transfer_zero_page_indexed(),
                  writer_to_zero_page_X(address),
                  reader_from(Y));
}

TEST_P(MOS6502_TestFixture_Transfer, LDY_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_transfer(LDY_ZERO_PAGE_X,
                  GetParam(),
                  ExecutionParameters::transfer_zero_page_indexed(),
                  writer_to_zero_page_X(address),
                  reader_from(Y));
}

TEST_P(MOS6502_TestFixture_Transfer, LDY_Absolute) {
    constexpr static Word address = 0x02f0;
    test_transfer(LDY_ABSOLUTE,
                  GetParam(),
                  ExecutionParameters::transfer_absolute(),
                  writer_to_absolute(address),
                  reader_from(Y));
}

TEST_P(MOS6502_TestFixture_Transfer, LDY_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x01;
    test_transfer(LDY_ABSOLUTE_X,
                  GetParam(),
                  ExecutionParameters::transfer_absolute_indexed(false),
                  writer_to_absolute_X(address),
                  reader_from(Y));
}

TEST_P(MOS6502_TestFixture_Transfer, LDY_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_transfer(LDY_ABSOLUTE_X,
                  GetParam(),
                  ExecutionParameters::transfer_absolute_indexed(true),
                  writer_to_absolute_X(address),
                  reader_from(Y));
}
