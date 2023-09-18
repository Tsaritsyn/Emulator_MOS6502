//
// Created by Mikhail on 16/09/2023.
//

#include "MOS6502_TestFixture.hpp"

constexpr std::array<Byte, 4> testedInputs {0, 1, 10, static_cast<unsigned char>(-10)};

static std::array<Addressing, 12> testedAddressings {
        // no page crossing
        Addressing::ZeroPage(0x10),
        Addressing::ZeroPageX(0x10, 0x10),
        Addressing::Absolute(0x2010),
        Addressing::AbsoluteX(0x2010, 0x10),
        Addressing::AbsoluteY(0x2010, 0x10),
        Addressing::IndirectX(0x10, 0x2010, 0x10),
        Addressing::IndirectY(0x10, 0x2010, 0x10),

        // page crossing
        Addressing::ZeroPageX(0xf0, 0x30),
        Addressing::AbsoluteX(0x20f0, 0x20),
        Addressing::AbsoluteY(0x20f0, 0x20),
        Addressing::IndirectX(0xf0, 0x2010, 0x30),
        Addressing::IndirectY(0x10, 0x20f0, 0x30),
};

TEST_F(MOS6502_TestFixture, Test_STA) {
    for (auto value: testedInputs)
        for (const auto& addressing: testedAddressings)
            test_store_accumulator(value, addressing);
}