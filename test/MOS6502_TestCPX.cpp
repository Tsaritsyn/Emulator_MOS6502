//
// Created by Mikhail on 02/11/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_CPX, CPX_Immediate) {
    test_compare(CPX_IMMEDIATE,
                 GetParam(),
                 COMPARE_IMMEDIATE_PARAMS,
                 writer_to(X),
                 writer_to_immediate());
}

TEST_P(MOS6502_TestFixture_CPX, Test_ZeroPage) {
    constexpr static Byte address = 0xf0;
    test_compare(CPX_ZERO_PAGE,
                 GetParam(),
                 COMPARE_ZERO_PAGE_PARAMS,
                 writer_to(X),
                 writer_to_zero_page(address));
}

TEST_P(MOS6502_TestFixture_CPX, Test_Absolute) {
    constexpr static Word address = 0x02f0;
    test_compare(CPX_ABSOLUTE,
                 GetParam(),
                 COMPARE_ABSOLUTE_PARAMS,
                 writer_to(X),
                 writer_to_absolute(address));
}



INSTANTIATE_TEST_SUITE_P(TestEqual, MOS6502_TestFixture_CPX, ::testing::Values(
        CompareParameters{.arg1 = 0, .arg2 = 0, .flagsSet = {Flag::ZERO, Flag::CARRY}},
        CompareParameters{.arg1 = 10, .arg2 = 10, .flagsSet = {Flag::ZERO, Flag::CARRY}},
        CompareParameters{.arg1 = 200, .arg2 = 200, .flagsSet = {Flag::ZERO, Flag::CARRY}}
));

INSTANTIATE_TEST_SUITE_P(TestGreater, MOS6502_TestFixture_CPX, ::testing::Values(
        CompareParameters{.arg1 = 1, .arg2 = 0, .flagsSet = {Flag::CARRY}},
        CompareParameters{.arg1 = 15, .arg2 = 10, .flagsSet = {Flag::CARRY}},
        CompareParameters{.arg1 = 210, .arg2 = 200, .flagsSet = {Flag::CARRY}}
));

INSTANTIATE_TEST_SUITE_P(TestLess, MOS6502_TestFixture_CPX, ::testing::Values(
        CompareParameters{.arg1 = 0, .arg2 = 1, .flagsSet = {Flag::NEGATIVE}},
        CompareParameters{.arg1 = 10, .arg2 = 15, .flagsSet = {Flag::NEGATIVE}},
        CompareParameters{.arg1 = 200, .arg2 = 210, .flagsSet = {Flag::NEGATIVE}}
));