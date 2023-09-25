//
// Created by Mikhail on 25/09/2023.
//

#include "MOS6502_TestFixture.hpp"

using namespace Emulator;

static constexpr std::array<Word, 8> testedAddresses{0, 10, 0xff, 0x0200, 0x02f0, 0x02ff, 0xff00, 0x36fe};
static constexpr std::array<Byte, 6> testedSRs{0, 10, 0xff, 0xf0, 0x0f, 0x8a};


TEST_F(MOS6502_TestFixture, TestRTI) {
    for (const auto previousPC: testedAddresses)
        for (const auto previousSR: testedSRs)
            test_return_from_interrupt(previousPC, previousSR);
}
