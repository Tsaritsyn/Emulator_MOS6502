//
// Created by Mikhail on 26/10/2023.
//

#include "MOS6502_TestFixture.hpp"
#include "helpers.hpp"


TEST_P(MOS6502_TestFixture_OR, Test_Immediate) {
    test_arithmetic(ORA_IMMEDIATE, GetParam(), 2, 2, [this](Byte value) { return write_immediate(value); });
}

TEST_P(MOS6502_TestFixture_OR, Test_ZeroPage) {
    test_arithmetic(ORA_ZERO_PAGE, GetParam(), 2, 3, [this](Byte value) { return write_zero_page(0xf0, value); });
}


TEST_P(MOS6502_TestFixture_OR, Test_ZeroPageX_NoPageCrossing) {
    test_arithmetic(ORA_ZERO_PAGE_X, GetParam(), 2, 4, [this](Byte value) {
        X = 0x05;
        return write_zero_page_X(0xf0, value);
    });
}


TEST_P(MOS6502_TestFixture_OR, Test_ZeroPageX_PageCrossing) {
    test_arithmetic(ORA_ZERO_PAGE_X, GetParam(), 2, 4, [this](Byte value) {
        X = 0x40;
        return write_zero_page_X(0xf0, value);
    });
}


TEST_P(MOS6502_TestFixture_OR, Test_Absolute) {
    test_arithmetic(ORA_ABSOLUTE, GetParam(), 3, 4, [this](Byte value) { return write_absolute(0x02f0, value); });
}


TEST_P(MOS6502_TestFixture_OR, Test_AbsoluteX_NoPageCrossing) {
    test_arithmetic(ORA_ABSOLUTE_X, GetParam(), 3, 4, [this](Byte value) {
        X = 0x01;
        return write_absolute_X(0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_OR, Test_AbsoluteX_PageCrossing) {
    test_arithmetic(ORA_ABSOLUTE_X, GetParam(), 3, 5, [this](Byte value) {
        X = 0x20;
        return write_absolute_X(0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_OR, Test_AbsoluteY_NoPageCrossing) {
    test_arithmetic(ORA_ABSOLUTE_Y, GetParam(), 3, 4, [this](Byte value) {
        Y = 0x01;
        return write_absolute_Y(0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_OR, Test_AbsoluteY_PageCrossing) {
    test_arithmetic(ORA_ABSOLUTE_Y, GetParam(), 3, 5, [this](Byte value) {
        Y = 0x20;
        return write_absolute_Y(0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_OR, Test_IndirectX_NoPageCrossing) {
    test_arithmetic(ORA_INDIRECT_X, GetParam(), 2, 6, [this](Byte value) {
        X = 0x01;
        return write_indirect_X(0xf0, 0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_OR, Test_IndirectX_PageCrossing) {
    test_arithmetic(ORA_INDIRECT_X, GetParam(), 2, 6, [this](Byte value) {
        X = 0x40;
        return write_indirect_X(0xf0, 0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_OR, Test_IndirectY_NoPageCrossing) {
    test_arithmetic(ORA_INDIRECT_Y, GetParam(), 2, 5, [this](Byte value) {
        Y = 0x01;
        return write_indirect_Y(0xf0, 0x02f0, value);
    });
}


TEST_P(MOS6502_TestFixture_OR, Test_IndirectY_PageCrossing) {
    test_arithmetic(ORA_INDIRECT_Y, GetParam(), 2, 6, [this](Byte value) {
        Y = 0x40;
        return write_indirect_Y(0xf0, 0x02f0, value);
    });
}



INSTANTIATE_TEST_SUITE_P(WithZero, MOS6502_TestFixture_OR, ::testing::Values(
        BinaryOpParameters{0, 0, false, 0, {Flag::ZERO}},
        BinaryOpParameters{1, 0, false, 1, {}},
        BinaryOpParameters{2, 0, false, 2, {}},
        BinaryOpParameters{0x37, 0, false, 0x37, {}},
        BinaryOpParameters{(Byte)-1, 0, false, (Byte)-1, {Flag::NEGATIVE}},
        BinaryOpParameters{(Byte)-100, 0, false, (Byte)-100, {Flag::NEGATIVE}}
));

INSTANTIATE_TEST_SUITE_P(WithAllOnes, MOS6502_TestFixture_OR, ::testing::Values(
        BinaryOpParameters{1, 0xff, false, 0xff, {Flag::NEGATIVE}},
        BinaryOpParameters{2, 0xff, false, 0xff, {Flag::NEGATIVE}},
        BinaryOpParameters{0x37, 0xff, false, 0xff, {Flag::NEGATIVE}},
        BinaryOpParameters{(Byte)-1, 0xff, false, 0xff, {Flag::NEGATIVE}},
        BinaryOpParameters{(Byte)-100, 0xff, false, 0xff, {Flag::NEGATIVE}}
));

INSTANTIATE_TEST_SUITE_P(ArbitraryNumbers, MOS6502_TestFixture_OR, ::testing::Values(
        BinaryOpParameters{0x59, 0x3c, false, 0x7d, {}},
        BinaryOpParameters{0x82, 0x5e, false, 0xde, {Flag::NEGATIVE}},
        BinaryOpParameters{0xd7, 0x2f, false, 0xff, {Flag::NEGATIVE}},
        BinaryOpParameters{0xab, 0x7d, false, 0xff, {Flag::NEGATIVE}},
        BinaryOpParameters{0xf3, 0xe9, false, 0xfb, {Flag::NEGATIVE}}
));

