//
// Created by Mikhail on 23/09/2023.
//

#include "MOS6502_TestFixture.hpp"

constexpr std::array<Word, 6> testedAddresses{0x000, 0x003, 0x0200, 0x02ff, 0xffff, 0x8645};


TEST_F(MOS6502_TestFixture, TestJSR) {
    for (const auto address: testedAddresses)
        test_jump_to_subroutine(address);
}
