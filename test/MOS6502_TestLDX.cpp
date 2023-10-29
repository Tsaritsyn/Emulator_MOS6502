//
// Created by Mikhail on 29/10/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Transfer, LDX_Immediate) {
    test_transfer(LDX_IMMEDIATE,
                  GetParam(),
                  ExecutionParameters::transfer_immediate(),
                  writer_to_immediate(),
                  reader_from(X));
}

TEST_P(MOS6502_TestFixture_Transfer, LDX_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_transfer(LDX_ZERO_PAGE,
                  GetParam(),
                  ExecutionParameters::transfer_zero_page(),
                  writer_to_zero_page(address),
                  reader_from(X));
}

TEST_P(MOS6502_TestFixture_Transfer, LDX_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    Y = 0x05;
    test_transfer(LDX_ZERO_PAGE_Y,
                  GetParam(),
                  ExecutionParameters::transfer_zero_page_indexed(),
                  writer_to_zero_page_Y(address),
                  reader_from(X));
}

TEST_P(MOS6502_TestFixture_Transfer, LDX_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    Y = 0x40;
    test_transfer(LDX_ZERO_PAGE_Y,
                  GetParam(),
                  ExecutionParameters::transfer_zero_page_indexed(),
                  writer_to_zero_page_Y(address),
                  reader_from(X));
}

TEST_P(MOS6502_TestFixture_Transfer, LDX_Absolute) {
    constexpr static Word address = 0x02f0;
    test_transfer(LDX_ABSOLUTE,
                  GetParam(),
                  ExecutionParameters::transfer_absolute(),
                  writer_to_absolute(address),
                  reader_from(X));
}

TEST_P(MOS6502_TestFixture_Transfer, LDX_AbsoluteY_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x01;
    test_transfer(LDX_ABSOLUTE_Y,
                  GetParam(),
                  ExecutionParameters::transfer_absolute_indexed(false),
                  writer_to_absolute_Y(address),
                  reader_from(X));
}

TEST_P(MOS6502_TestFixture_Transfer, LDX_AbsoluteY_PageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x20;
    test_transfer(LDX_ABSOLUTE_Y,
                  GetParam(),
                  ExecutionParameters::transfer_absolute_indexed(true),
                  writer_to_absolute_Y(address),
                  reader_from(X));
}