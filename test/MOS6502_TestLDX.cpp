//
// Created by Mikhail on 14/09/2023.
//

#include "MOS6502_TestFixture.hpp"

constexpr std::array<Byte, 4> testedInputsAND {0, 1, 10, static_cast<unsigned char>(-10)};

static std::array<Addressing, 7> testedAddressings {
        // no page crossing
        Addressing::Immediate(),
        Addressing::ZeroPage(0x10),
        Addressing::ZeroPageY(0x10, 0x10),
        Addressing::Absolute(0x2010),
        Addressing::AbsoluteY(0x2010, 0x10),

        // page crossing
        Addressing::ZeroPageY(0xf0, 0x30),
        Addressing::AbsoluteY(0x20f0, 0x20),
};

TEST_F(MOS6502_TestFixture, Test_LDX) {
    for (auto value: testedInputsAND)
        for (const auto& addressing: testedAddressings)
            test_loading(Emulator::Register::X, value, addressing);
}
