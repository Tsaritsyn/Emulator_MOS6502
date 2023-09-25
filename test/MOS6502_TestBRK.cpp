//
// Created by Mikhail on 25/09/2023.
//

#include "MOS6502_TestFixture.hpp"

using namespace Emulator;

static constexpr std::array<Word, 8> testedAddresses{0, 10, 0xff, 0x0200, 0x02f0, 0x02ff, 0xff00, 0x36fe};


TEST_F(MOS6502_TestFixture, TestBRK) {
    for (const auto initialPC: testedAddresses)
        for (const auto interruptVector: testedAddresses)
            test_brk(initialPC, interruptVector);
}