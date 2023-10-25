//
// Created by Mikhail on 14/09/2023.
//

#include <algorithm>
#include "MOS6502_TestFixture.hpp"

void MOS6502_TextFixture::reset_registers() noexcept {
    AC = 0;
    X = 0;
    Y = 0;
    SR = 0;
    SP = UINT8_MAX;
    PC = 0;
}

std::expected<void, ROM::StackOverride> MOS6502_TextFixture::set_word(Word address, Word value) noexcept {
    WordToBytes buf(value);
    return memory.set_byte(address, buf.low()).and_then(
            [this, buf, address](){ return memory.set_byte(address + 1, buf.high()); }
            );
}

void MOS6502_TestFixture_Arithmetic::SetUp() {
    reset_registers();
    memory.reset();
}

std::ostream &operator<<(std::ostream &os, const ArithmeticTestParameters &parameters) {
    os << std::format("({:d}, {:d}, {:d}) -> {:d}, flags set: [", parameters.AC, parameters.memory, parameters.carry, parameters.result);
    std::ranges::for_each(parameters.flagsSet, [&os](Flag flag){ os << flag << ", "; });
    return os << ']';
}
