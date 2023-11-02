//
// Created by Mikhail on 30/10/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Transfer, STX_ZeroPage) {
    constexpr static Byte address = 0xf0;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STX_ZERO_PAGE,
                                GetParam(),
                                TRANSFER_ZERO_PAGE_PARAMS,
                                writer_to(X),
                                reader_from_zero_page(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STX_ZeroPageY_NoPageCrossing) {
    constexpr static Byte address = 0xf0;
    Y = 0x01;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STX_ZERO_PAGE_Y,
                                GetParam(),
                                TRANSFER_ZERO_PAGE_X_PARAMS,
                                writer_to(X),
                                reader_from_zero_page_Y(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STX_ZeroPageY_PageCrossing) {
    constexpr static Byte address = 0xf0;
    Y = 0x40;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STX_ZERO_PAGE_Y,
                                GetParam(),
                                TRANSFER_ZERO_PAGE_X_PARAMS,
                                writer_to(X),
                                reader_from_zero_page_Y(address));
}

TEST_P(MOS6502_TestFixture_Transfer, STX_Absolute) {
    constexpr static Word address = 0x02f0;

    ASSERT_TRUE(set_operation_arg(address));
    test_transfer_without_flags(STX_ABSOLUTE,
                                GetParam(),
                                TRANSFER_ABSOLUTE_PARAMS,
                                writer_to(X),
                                reader_from_absolute(address));
}


