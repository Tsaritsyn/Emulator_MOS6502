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

void MOS6502_TestFixture_BinaryOp::SetUp() {
    reset_registers();
    memory.reset();

    maxNumberOfCommandsToExecute = 0;
    stop_on_break(false);
}

void
MOS6502_TestFixture_BinaryOp::test_binary(OpCode opcode,
                                          const BinaryOpParameters &params,
                                          const ExecutionParameters &execParams,
                                          const Writer &arg1Writer,
                                          const Writer &arg2Writer,
                                          const Reader &resultReader) {
    SR[Emulator::Flag::CARRY] = params.carry;

    ASSERT_TRUE(memory.set_byte(PC, opcode).has_value());
    ASSERT_TRUE(arg1Writer(params.arg1).has_value());
    ASSERT_TRUE(arg2Writer(params.arg2).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(resultReader(), params.result);
    EXPECT_EQ(PC, execParams.size);
    EXPECT_EQ(cycle, execParams.duration);
    EXPECT_EQ(SR, status_from_flags(params.flagsSet));
}

MOS6502_TextFixture::Writer MOS6502_TextFixture::writer_to(Byte &loc) noexcept {
    return [&loc](Byte value) -> ROM::WriteResult {
        loc = value;
        return {};
    };
}

MOS6502_TextFixture::Reader MOS6502_TextFixture::reader_from(const Byte &loc) noexcept {
    // here we have to pass by reference, because it is captured at the moment of creation
    return [&loc]() { return loc; };
}

MOS6502_TextFixture::Writer MOS6502_TextFixture::writer_to_immediate() noexcept {
    return [this](Byte value) -> ROM::WriteResult {return memory.set_byte(PC + 1, value);};
}

MOS6502_TextFixture::Writer MOS6502_TextFixture::writer_to_zero_page(Byte address) noexcept {
    return [this, address](Byte value) -> ROM::WriteResult {
        return memory.set_byte(PC + 1, address)
            .and_then([this, address, value](){ return memory.set_byte(address, value); });
    };
}

MOS6502_TextFixture::Writer MOS6502_TextFixture::writer_to_zero_page_X(Byte address) noexcept {
    return [this, address](Byte value) -> ROM::WriteResult {
        return memory.set_byte(PC + 1, address)
                .and_then([this, address, value](){ return memory.set_byte((Byte)(address + X), value); });
    };
}

MOS6502_TextFixture::Writer MOS6502_TextFixture::writer_to_zero_page_Y(Byte address) noexcept {
    return [this, address](Byte value) -> ROM::WriteResult {
        return memory.set_byte(PC + 1, address)
                .and_then([this, address, value](){ return memory.set_byte((Byte)(address + Y), value); });
    };
}

MOS6502_TextFixture::Writer MOS6502_TextFixture::writer_to_absolute(Word address) noexcept {
    return [this, address](Byte value) -> ROM::WriteResult {
        return set_word(PC + 1, address)
                .and_then([this, address, value](){ return memory.set_byte(address, value); });
    };
}

MOS6502_TextFixture::Writer MOS6502_TextFixture::writer_to_absolute_X(Word address) noexcept {
    return [this, address](Byte value) -> ROM::WriteResult {
        return set_word(PC + 1, address)
                .and_then([this, address, value](){ return memory.set_byte(address + X, value); });
    };
}

MOS6502_TextFixture::Writer MOS6502_TextFixture::writer_to_absolute_Y(Word address) noexcept {
    return [this, address](Byte value) -> ROM::WriteResult {
        return set_word(PC + 1, address)
                .and_then([this, address, value](){ return memory.set_byte(address + Y, value); });
    };
}

MOS6502_TextFixture::Writer MOS6502_TextFixture::writer_to_indirect_X(Byte tableAddress, Word targetAddress) noexcept {
    return [this, tableAddress, targetAddress](Byte value) -> ROM::WriteResult {
        return memory.set_byte(PC + 1, tableAddress)
            .and_then([this, tableAddress, targetAddress](){ return set_word((Byte)(tableAddress + X), targetAddress); })
            .and_then([this, targetAddress, value](){ return memory.set_byte(targetAddress, value); });
    };
}

MOS6502_TextFixture::Writer MOS6502_TextFixture::writer_to_indirect_Y(Byte tableAddress, Word targetAddress) noexcept {
    return [this, tableAddress, targetAddress](Byte value) -> ROM::WriteResult {
        return memory.set_byte(PC + 1, tableAddress)
            .and_then([this, tableAddress, targetAddress](){ return set_word(tableAddress, targetAddress); })
            .and_then([this, targetAddress, value](){ return memory.set_byte(targetAddress + Y, value); });
    };
}


std::ostream &operator<<(std::ostream &os, const BinaryOpParameters &parameters) {
    os << std::format("({:d}, {:d}, {:d}) -> {:d}, flags set: [", parameters.arg1, parameters.arg2, parameters.carry, parameters.result);
    std::ranges::for_each(parameters.flagsSet, [&os](Flag flag){ os << flag << ", "; });
    return os << ']';
}

std::ostream &operator<<(std::ostream &os, const UnaryOpParameters &parameters) {
    os << std::format("(0x{:02x}, {:d}) -> 0x{:02x}, flags set: [", parameters.arg, parameters.carry, parameters.result);
    std::ranges::for_each(parameters.flagsSet, [&os](Flag flag){ os << flag << ", "; });
    return os << ']';
}

void MOS6502_TestFixture_UnaryOp::SetUp() {
    reset_registers();
    memory.reset();

    maxNumberOfCommandsToExecute = 0;
    stop_on_break(false);
}

void MOS6502_TestFixture_UnaryOp::test_unary(OpCode opcode,
                                             const UnaryOpParameters &params,
                                             const ExecutionParameters &execParams,
                                             const Writer &argWriter,
                                             const Reader &resultReader) {
    SR[Flag::CARRY] = params.carry;

    ASSERT_TRUE(memory.set_byte(PC, opcode).has_value());
    ASSERT_TRUE(argWriter(params.arg).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(resultReader(), params.result);
    EXPECT_EQ(PC, execParams.size);
    EXPECT_EQ(cycle, execParams.duration);
    EXPECT_EQ(SR, status_from_flags(params.flagsSet));
}

void MOS6502_TestFixture_Transfer::SetUp() {
    reset_registers();
    memory.reset();

    maxNumberOfCommandsToExecute = 0;
    stop_on_break(false);
}

void MOS6502_TestFixture_Transfer::test_transfer(OpCode opcode,
                                                 Byte arg,
                                                 const ExecutionParameters &execParams,
                                                 const MOS6502_TextFixture::Writer &argWriter,
                                                 const MOS6502_TextFixture::Reader &resultReader) {
    const auto expectedStatus = [arg]{
        ProcessorStatus result(0);
        if (arg == 0) result[Flag::ZERO] = SET;
        if ((char)arg < 0) result[Flag::NEGATIVE] = SET;
        return result;
    }();

    ASSERT_TRUE(memory.set_byte(PC, opcode).has_value());
    ASSERT_TRUE(argWriter(arg).has_value());

    maxNumberOfCommandsToExecute = 1;
    ASSERT_TRUE(execute().has_value());

    EXPECT_EQ(resultReader(), arg);
    EXPECT_EQ(PC, execParams.size);
    EXPECT_EQ(cycle, execParams.duration);
    EXPECT_EQ(SR, expectedStatus);
}
