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

void MOS6502_TestFixture_Arithmetic::test_arithmetic(OpCode opcode,
                                                     const ArithmeticTestParameters &params,
                                                     Word size,
                                                     size_t duration,
                                                     const std::function<ROM::WriteResult(Byte)> &writer) {
    AC = params.AC;
    SR[Emulator::Flag::CARRY] = params.carry;

    ASSERT_TRUE(memory.set_byte(PC, opcode).has_value());
    ASSERT_TRUE(writer(params.memory).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(AC, params.result);
    EXPECT_EQ(PC, size);
    EXPECT_EQ(cycle, duration);
    EXPECT_EQ(SR, status_from_flags(params.flagsSet));
}

ROM::WriteResult MOS6502_TextFixture::write_immediate(Byte value) noexcept {
    return memory.set_byte(PC + 1, value);
}

ROM::WriteResult MOS6502_TextFixture::write_zero_page(Byte address, Byte value) noexcept {
    return memory.set_byte(PC + 1, address)
        .and_then([this, address, value](){ return memory.set_byte(address, value); });
}

ROM::WriteResult MOS6502_TextFixture::write_zero_page_X(Byte address, Byte value) noexcept {
    return memory.set_byte(PC + 1, address)
        .and_then([this, address, value](){ return memory.set_byte((Byte)(address + X), value); });
}

ROM::WriteResult MOS6502_TextFixture::write_zero_page_Y(Byte address, Byte value) noexcept {
    return memory.set_byte(PC + 1, address)
        .and_then([this, address, value](){ return memory.set_byte((Byte)(address + Y), value); });
}

ROM::WriteResult MOS6502_TextFixture::write_absolute(Word address, Byte value) noexcept {
    return set_word(PC + 1, address)
        .and_then([this, address, value](){ return memory.set_byte(address, value); });
}

ROM::WriteResult MOS6502_TextFixture::write_absolute_X(Word address, Byte value) noexcept {
    return set_word(PC + 1, address)
        .and_then([this, address, value](){ return memory.set_byte(address + X, value); });
}

ROM::WriteResult MOS6502_TextFixture::write_absolute_Y(Word address, Byte value) noexcept {
    return set_word(PC + 1, address)
        .and_then([this, address, value](){ return memory.set_byte(address + Y, value); });
}

ROM::WriteResult MOS6502_TextFixture::write_indirect_X(Byte tableAddress, Word targetAddress, Byte value) noexcept {
    return memory.set_byte(PC + 1, tableAddress)
        .and_then([this, tableAddress, targetAddress](){ return set_word((Byte)(tableAddress + X), targetAddress); })
        .and_then([this, targetAddress, value](){ return memory.set_byte(targetAddress, value); });
}

ROM::WriteResult MOS6502_TextFixture::write_indirect_Y(Byte tableAddress, Word targetAddress, Byte value) noexcept {
    return memory.set_byte(PC + 1, tableAddress)
        .and_then([this, tableAddress, targetAddress](){ return set_word(tableAddress, targetAddress); })
        .and_then([this, targetAddress, value](){ return memory.set_byte(targetAddress + Y, value); });
}

std::ostream &operator<<(std::ostream &os, const ArithmeticTestParameters &parameters) {
    os << std::format("({:d}, {:d}, {:d}) -> {:d}, flags set: [", parameters.AC, parameters.memory, parameters.carry, parameters.result);
    std::ranges::for_each(parameters.flagsSet, [&os](Flag flag){ os << flag << ", "; });
    return os << ']';
}
