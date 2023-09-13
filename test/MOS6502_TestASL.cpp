//
// Created by Mikhail on 13/09/2023.
//


#include "MOS6502_TestFixture.hpp"

constexpr std::array<ShiftLeft, 6> testedInputs {
    ShiftLeft{0},
    {1},
    {0b10000000},
    {0b11000000},
    {0b10000001},
    {0x9d}
};


constexpr std::array<Addressing, 6> testedAddressings {
    Accumulator{},
    ZeroPage{0xFF},
    ZeroPageX{0xF0, 0x0F},
    Absolute{0x1234},
    // no page crossing
    AbsoluteX{0x1200, 0x34},
    // with page crossing
    AbsoluteX{0x12f0, 0x34}
};


TEST_F(MOS6502_TestFixture, TestASL) {
    for (const auto &addressing: testedAddressings) {
        for (const auto &input: testedInputs) {
            test_instruction(input, addressing);
        }
    }
}