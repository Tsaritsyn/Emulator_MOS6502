//
// Created by Mikhail on 19/09/2023.
//

#include "MOS6502_TestFixture.hpp"

constexpr std::array<std::pair<Byte, Byte>, 6> testedInputs {
    std::pair<Byte, Byte>{0, 0},
    {10, 10},
    {-10, -10},
    {0, 10},
    {0, -10},
    {10, -10},
};

static std::array<Addressing, 13> testedAddressingsCMP {
        // no page crossing
        Addressing::Immediate(),
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

static std::array<Addressing, 3> testedAddressingsCPX {
        // no page crossing
        Addressing::Immediate(),
        Addressing::ZeroPage(0x10),
        Addressing::Absolute(0x2010),
};


TEST_F(MOS6502_TestFixture, TestCMP) {
    for (const auto &[value1, value2]: testedInputs)
        for (const auto &addressing: testedAddressingsCMP) {
            test_compare_register(Emulator::Register::AC, value1, addressing, value2);
            test_compare_register(Emulator::Register::AC, value2, addressing, value1);
        }
}

TEST_F(MOS6502_TestFixture, TestCPX) {
    for (const auto &[value1, value2]: testedInputs)
        for (const auto &addressing: testedAddressingsCPX) {
            test_compare_register(Emulator::Register::X, value1, addressing, value2);
            test_compare_register(Emulator::Register::X, value2, addressing, value1);
        }
}

TEST_F(MOS6502_TestFixture, TestCPY) {
    for (const auto &[value1, value2]: testedInputs)
        for (const auto &addressing: testedAddressingsCPX) {
            test_compare_register(Emulator::Register::Y, value1, addressing, value2);
            test_compare_register(Emulator::Register::Y, value2, addressing, value1);
        }
}