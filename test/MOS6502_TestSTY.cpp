//
// Created by Mikhail on 30/10/2023.
//

#include "MOS6502_TestFixture.hpp"

TEST_P(MOS6502_TestFixture_Transfer, STY_ZeroPage) {
    constexpr static Byte address = 0xf0;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STY_ZERO_PAGE,
                                GetParam(),
                                ExecutionParameters::transfer_zero_page(),
                                writer_to(Y),
                                reader_from_zero_page(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STY_ZeroPageX_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x01;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STY_ZERO_PAGE_X,
                                GetParam(),
                                ExecutionParameters::transfer_zero_page_indexed(),
                                writer_to(Y),
                                reader_from_zero_page_X(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STY_ZeroPageX_PageCrossing) {
    constexpr static Byte address = 0xf0;
    X = 0x40;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STY_ZERO_PAGE_X,
                                GetParam(),
                                ExecutionParameters::transfer_zero_page_indexed(),
                                writer_to(Y),
                                reader_from_zero_page_X(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STY_Absolute) {
    constexpr static Word address = 0x02f0;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STY_ABSOLUTE,
                                GetParam(),
                                ExecutionParameters::transfer_absolute(),
                                writer_to(Y),
                                reader_from_absolute(address));
}