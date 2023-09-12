//
// Created by Mikhail on 31/08/2023.
//

#include "MOS6502_TestFixture.hpp"

using namespace Emulator;

constexpr std::array<Arithmetics, 23> testedInputs {
        Arithmetics{ArithmeticOperation::ADD, 1, 2, false},
        {ArithmeticOperation::ADD, 2, 1, false},
        {ArithmeticOperation::ADD, 1, 0, false},
        {ArithmeticOperation::ADD, 0, 1, false},
        {ArithmeticOperation::ADD, 0, 0, false},
        {ArithmeticOperation::ADD, static_cast<Byte>(-1), 0, false},
        {ArithmeticOperation::ADD, 0, static_cast<Byte>(-1), false},
        {ArithmeticOperation::ADD, 100, 110, false},
        {ArithmeticOperation::ADD, 110, 100, false},
        {ArithmeticOperation::ADD, 1, static_cast<Byte>(-1), false},
        {ArithmeticOperation::ADD, static_cast<Byte>(-1), 1, false},
        {ArithmeticOperation::ADD, static_cast<Byte>(-10), static_cast<Byte>(-20), false},
        {ArithmeticOperation::ADD, static_cast<Byte>(-20), static_cast<Byte>(-10), false},
        {ArithmeticOperation::ADD, static_cast<Byte>(-70), static_cast<Byte>(-80), false},
        {ArithmeticOperation::ADD, static_cast<Byte>(-80), static_cast<Byte>(-70), false},
        {ArithmeticOperation::ADD, 1, 2, true},
        {ArithmeticOperation::ADD, 2, 1, true},
        {ArithmeticOperation::ADD, static_cast<Byte>(-1), 0, true},
        {ArithmeticOperation::ADD, 0, static_cast<Byte>(-1), true},
        {ArithmeticOperation::ADD, 127, 127, true},
        {ArithmeticOperation::ADD, 0, 0, true},
        {ArithmeticOperation::ADD, static_cast<Byte>(-10), static_cast<Byte>(-20), true},
        {ArithmeticOperation::ADD, static_cast<Byte>(-20), static_cast<Byte>(-10), true},
};


constexpr std::array<Addressing, 11> testedAddressings {
    Immediate{},
    ZeroPage{0xFF},
    ZeroPageX{0xF0, 0x0F},
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


TEST_F(MOS6502_TestFixture, TestADC) {
    for (const auto &addressing: testedAddressings) {
        for (const auto &input: testedInputs) {
            test_instruction(input, addressing);
        }
    }
}