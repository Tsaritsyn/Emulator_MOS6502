//
// Created by Mikhail on 13/09/2023.
//

#include "MOS6502_TestFixture.hpp"

constexpr std::array<Logical, 11> testedInputs {
        Logical{LogicalOperation::AND, 0, 0},
        {LogicalOperation::AND, 0, 1},
        {LogicalOperation::AND, 1, 0},
        {LogicalOperation::AND, 0, 0xff},
        {LogicalOperation::AND, 0xff, 0},
        {LogicalOperation::AND, 0xff, 1},
        {LogicalOperation::AND, 1, 0xff},
        {LogicalOperation::AND, 0xff, static_cast<Byte>(-30)},
        {LogicalOperation::AND, static_cast<Byte>(-30), 0xff},
        {LogicalOperation::AND, 0xa9, 0x8d},
        {LogicalOperation::AND, 0x8d, 0xa9},
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


TEST_F(MOS6502_TestFixture, TestAND) {
    for (const auto &addressing: testedAddressings) {
        for (const auto &input: testedInputs) {
            test_instruction(input, addressing);
        }
    }
}
