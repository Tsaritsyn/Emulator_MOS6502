//
// Created by Mikhail on 09/09/2023.
//

#include "MOS6502_TestFixture.hpp"

constexpr std::array<Arithmetics, 25> testedInputs {
        Arithmetics{ArithmeticOperation::SUBTRACT, 1, 2, true},
        {ArithmeticOperation::SUBTRACT, 2, 1, true},
        {ArithmeticOperation::SUBTRACT, 1, 0, true},
        {ArithmeticOperation::SUBTRACT, 0, 1, true},
        {ArithmeticOperation::SUBTRACT, 0, 0, true},
        {ArithmeticOperation::SUBTRACT, 1, 1, true},
        {ArithmeticOperation::SUBTRACT, static_cast<Byte>(-1), 0, true},
        {ArithmeticOperation::SUBTRACT, 0, static_cast<Byte>(-1), true},
        {ArithmeticOperation::SUBTRACT, static_cast<Byte>(-1), static_cast<Byte>(-1), true},
        {ArithmeticOperation::SUBTRACT, 100, 110, true},
        {ArithmeticOperation::SUBTRACT, 110, 100, true},
        {ArithmeticOperation::SUBTRACT, 1, static_cast<Byte>(-1), true},
        {ArithmeticOperation::SUBTRACT, static_cast<Byte>(-1), 1, true},
        {ArithmeticOperation::SUBTRACT, static_cast<Byte>(-10), static_cast<Byte>(-20), true},
        {ArithmeticOperation::SUBTRACT, static_cast<Byte>(-20), static_cast<Byte>(-10), true},
        {ArithmeticOperation::SUBTRACT, static_cast<Byte>(-70), static_cast<Byte>(-80), true},
        {ArithmeticOperation::SUBTRACT, static_cast<Byte>(-80), static_cast<Byte>(-70), true},
        {ArithmeticOperation::SUBTRACT, 1, 2, false},
        {ArithmeticOperation::SUBTRACT, 2, 1, false},
        {ArithmeticOperation::SUBTRACT, static_cast<Byte>(-1), 0, false},
        {ArithmeticOperation::SUBTRACT, 0, static_cast<Byte>(-1), false},
        {ArithmeticOperation::SUBTRACT, 1, 1, false},
        {ArithmeticOperation::SUBTRACT, 0, 0, false},
        {ArithmeticOperation::SUBTRACT, static_cast<Byte>(-10), static_cast<Byte>(-20), false},
        {ArithmeticOperation::SUBTRACT, static_cast<Byte>(-20), static_cast<Byte>(-10), false},
};


constexpr std::array<Addressing, 12> testedAddressings {
        Immediate{},
        ZeroPage{0xFF},
        // no page wrapping
        ZeroPageX{0xF0, 0x0F},
        // with page wrapping
        ZeroPageX{0xF0, 0xF0},
        Absolute{0x1234},
        // no page crossing
        AbsoluteX{0x1200, 0x34},
        // with page crossing
        AbsoluteX{0x12f0, 0x34},
        // no page crossing
        AbsoluteY{0x1200, 0x34},
        // with page crossing
        AbsoluteY{0x12f0, 0x34},
        IndirectX{0x10, 0x1234, 0x0f},
        // no page crossing
        IndirectY{0x10, 0x1234, 0x20},
        // with page crossing
        IndirectY{0x10, 0x12f0, 0x20}
};


TEST_F(MOS6502_TestFixture, TestSBC) {
    for (const auto &addressing: testedAddressings) {
        for (const auto &input: testedInputs) {
            test_instruction(input, addressing);
        }
    }
}