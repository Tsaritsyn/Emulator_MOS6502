//
// Created by Mikhail on 18/09/2023.
//

#include "MOS6502_TestFixture.hpp"

using namespace Emulator;

constexpr static std::array<Byte, 5> testedInputs = {0, 1, 10, static_cast<unsigned char>(-10), 255};

constexpr static std::array<std::pair<Register, Register>, 6> testedRegisters = {
        std::pair<Register, Register>{Register::AC, Register::X},
        {Register::X, Register::AC},
        {Register::AC, Register::Y},
        {Register::Y, Register::AC},
        {Register::SP, Register::X},
        {Register::X, Register::SP}
};


TEST_F(MOS6502_TestFixture, TestRegisterTransfer) {
    for (const auto value: testedInputs)
        for (const auto &[from, to]: testedRegisters)
            test_transfer(from, to, value);
}
