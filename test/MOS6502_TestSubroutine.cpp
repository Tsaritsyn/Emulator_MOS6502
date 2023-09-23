//
// Created by Mikhail on 23/09/2023.
//

#include "MOS6502_TestFixture.hpp"

constexpr std::array<Word, 6> testedAddresses{0x000, 0x003, 0x0200, 0x02ff, 0xfffe, 0x8645};


TEST_F(MOS6502_TestFixture, TestJSR) {
    for (const auto address: testedAddresses)
        test_jump_to_subroutine(address);
}

TEST_F(MOS6502_TestFixture, TestRTS) {
    for (const auto address: testedAddresses)
        test_return_from_subroutine(address);
}
