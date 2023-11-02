//
// Created by Mikhail on 02/11/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_CMP, CMP_Immediate) {
    test_compare(CMP_IMMEDIATE,
                 GetParam(),
                 COMPARE_IMMEDIATE_PARAMS,
                 writer_to(AC),
                 writer_to_immediate());
}

TEST_P(MOS6502_TestFixture_CMP, Test_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_compare(CMP_ZERO_PAGE,
                GetParam(),
                COMPARE_ZERO_PAGE_PARAMS,
                writer_to(AC),
                writer_to_zero_page(address));
}

TEST_P(MOS6502_TestFixture_CMP, Test_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x05;
    test_compare(CMP_ZERO_PAGE_X,
                GetParam(),
                COMPARE_ZERO_PAGE_X_PARAMS,
                writer_to(AC),
                writer_to_zero_page_X(address));
}

TEST_P(MOS6502_TestFixture_CMP, Test_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;
    test_compare(CMP_ZERO_PAGE_X,
                GetParam(),
                COMPARE_ZERO_PAGE_X_PARAMS,
                writer_to(AC),
                writer_to_zero_page_X(address));
}

TEST_P(MOS6502_TestFixture_CMP, Test_Absolute) {
    constexpr static Word address = 0x02f0;
    test_compare(CMP_ABSOLUTE,
                GetParam(),
                COMPARE_ABSOLUTE_PARAMS,
                writer_to(AC),
                writer_to_absolute(address));
}

TEST_P(MOS6502_TestFixture_CMP, Test_AbsoluteX_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x01;
    test_compare(CMP_ABSOLUTE_X,
                GetParam(),
                COMPARE_ABSOLUTE_X_NO_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_absolute_X(address));
}

TEST_P(MOS6502_TestFixture_CMP, Test_AbsoluteX_PageCrossing) {
    constexpr static Word address = 0x02f0;
    X = 0x20;
    test_compare(CMP_ABSOLUTE_X,
                GetParam(),
                COMPARE_ABSOLUTE_X_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_absolute_X(address));
}

TEST_P(MOS6502_TestFixture_CMP, Test_AbsoluteY_NoPageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x01;
    test_compare(CMP_ABSOLUTE_Y,
                GetParam(),
                COMPARE_ABSOLUTE_X_NO_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_absolute_Y(address));
}

TEST_P(MOS6502_TestFixture_CMP, Test_AbsoluteY_PageCrossing) {
    constexpr static Word address = 0x02f0;
    Y = 0x20;
    test_compare(CMP_ABSOLUTE_Y,
                GetParam(),
                COMPARE_ABSOLUTE_X_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_absolute_Y(address));
}

TEST_P(MOS6502_TestFixture_CMP, Test_IndirectX_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x01;
    test_compare(CMP_INDIRECT_X,
                GetParam(),
                COMPARE_INDIRECT_X_PARAMS,
                writer_to(AC),
                writer_to_indirect_X(tableAddress, targetAddress));
}

TEST_P(MOS6502_TestFixture_CMP, Test_IndirectX_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    X = 0x40;
    test_compare(CMP_INDIRECT_X,
                GetParam(),
                COMPARE_INDIRECT_X_PARAMS,
                writer_to(AC),
                writer_to_indirect_X(tableAddress, targetAddress));
}

TEST_P(MOS6502_TestFixture_CMP, Test_IndirectY_NoPageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x01;
    test_compare(CMP_INDIRECT_Y,
                GetParam(),
                COMPARE_INDIRECT_Y_NO_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_indirect_Y(tableAddress, targetAddress));
}

TEST_P(MOS6502_TestFixture_CMP, Test_IndirectY_PageCrossing) {
    constexpr static Byte tableAddress = 0xf0;
    constexpr static Word targetAddress = 0x02f0;
    Y = 0x40;
    test_compare(CMP_INDIRECT_Y,
                GetParam(),
                COMPARE_INDIRECT_Y_PAGE_CROSSING_PARAMS,
                writer_to(AC),
                writer_to_indirect_Y(tableAddress, targetAddress));
}



INSTANTIATE_TEST_SUITE_P(TestEqual, MOS6502_TestFixture_CMP, ::testing::Values(
        CompareParameters{.arg1 = 0, .arg2 = 0, .flagsSet = {Flag::ZERO, Flag::CARRY}},
        CompareParameters{.arg1 = 10, .arg2 = 10, .flagsSet = {Flag::ZERO, Flag::CARRY}},
        CompareParameters{.arg1 = 200, .arg2 = 200, .flagsSet = {Flag::ZERO, Flag::CARRY}}
));

INSTANTIATE_TEST_SUITE_P(TestGreater, MOS6502_TestFixture_CMP, ::testing::Values(
        CompareParameters{.arg1 = 1, .arg2 = 0, .flagsSet = {Flag::CARRY}},
        CompareParameters{.arg1 = 15, .arg2 = 10, .flagsSet = {Flag::CARRY}},
        CompareParameters{.arg1 = 210, .arg2 = 200, .flagsSet = {Flag::CARRY}}
));

INSTANTIATE_TEST_SUITE_P(TestLess, MOS6502_TestFixture_CMP, ::testing::Values(
        CompareParameters{.arg1 = 0, .arg2 = 1, .flagsSet = {Flag::NEGATIVE}},
        CompareParameters{.arg1 = 10, .arg2 = 15, .flagsSet = {Flag::NEGATIVE}},
        CompareParameters{.arg1 = 200, .arg2 = 210, .flagsSet = {Flag::NEGATIVE}}
));