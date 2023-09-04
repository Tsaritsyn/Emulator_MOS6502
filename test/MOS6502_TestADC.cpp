//
// Created by Mikhail on 31/08/2023.
//

#include "MOS6502_TestFixture.hpp"

static constexpr std::array<AddressingMode, 8> testedModes{
        AddressingMode::IMMEDIATE,
        AddressingMode::ZERO_PAGE,
        AddressingMode::ZERO_PAGE_X,
        AddressingMode::ABSOLUTE,
        AddressingMode::ABSOLUTE_X,
        AddressingMode::ABSOLUTE_Y,
        AddressingMode::INDIRECT_X,
        AddressingMode::INDIRECT_Y
};

static constexpr std::array<MOS6502_TestFixture::Operands, 21> testedInputs{
        MOS6502_TestFixture::Operands{1, 2, false},
        {2, 1, false},
        {1, 0, false},
        {0, 1, false},
        {0, 0, false},
        {static_cast<Byte>(-1), 0, false},
        {0, static_cast<Byte>(-1), false},
        {100, 110, false},
        {110, 100, false},
        {1, static_cast<Byte>(-1), false},
        {static_cast<Byte>(-1), 1, false},
        {static_cast<Byte>(-10), static_cast<Byte>(-20), false},
        {static_cast<Byte>(-20), static_cast<Byte>(-10), false},
        {static_cast<Byte>(-70), static_cast<Byte>(-80), false},
        {static_cast<Byte>(-80), static_cast<Byte>(-70), false},
        {1, 2, true},
        {2, 1, true},
        {0, 0, true},
        {static_cast<Byte>(-1), 0, true},
        {0, static_cast<Byte>(-1), true},
        {127, 127, true}
};



TEST_F(MOS6502_TestFixture, TestADC) {
    for (const auto &mode: testedModes) {
        for (const auto &operands: testedInputs) {
            test_ADC(mode, operands, false);
        }
    }
}


TEST_F(MOS6502_TestFixture, TestADCWithPageCrossing) {
    for (const auto &mode: {AddressingMode::ABSOLUTE_X, AddressingMode::ABSOLUTE_Y, AddressingMode::INDIRECT_Y}) {
        for (const auto &operands: testedInputs) {
            test_ADC(mode, operands, true);
        }
    }
}