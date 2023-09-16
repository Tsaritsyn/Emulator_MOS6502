//
// Created by Mikhail on 14/09/2023.
//

#include "MOS6502_TestFixture.hpp"
#include "helpers.hpp"

void MOS6502_TestFixture::write_word(Word word, Word address) noexcept {
    const WordToBytes buf(word);
    memory[address] = buf.low;
    memory[address + 1] = buf.high;
}


void MOS6502_TestFixture::write_to_memory(Byte value, const Addressing &addressing) noexcept {
    switch (addressing.getMode()) {
        case AddressingMode::IMPLICIT:
            return;
        case AddressingMode::ACCUMULATOR:
            AC = value;
            return;
        case AddressingMode::IMMEDIATE:
            memory[PC + 1] = value;
            return;
        case AddressingMode::ZERO_PAGE: {
            const auto [address] = addressing.getZeroPage().value();
            memory[address] = value;
            memory[PC + 1] = address;
            return;
        }
        case AddressingMode::ZERO_PAGE_X: {
            const auto [address, index] = addressing.getZeroPageX().value();
            X = index;
            memory[address + index] = value;
            memory[PC + 1] = address;
            return;
        }
        case AddressingMode::ZERO_PAGE_Y: {
            const auto [address, index] = addressing.getZeroPageY().value();
            Y = index;
            memory[address + index] = value;
            memory[PC + 1] = address;
            return;
        }
        case AddressingMode::RELATIVE: {
            const auto [PC_, offset] = addressing.getRelative().value();
            PC = PC_;
            memory[PC + 1] = offset;
            return;
        }
        case AddressingMode::ABSOLUTE: {
            const auto [address] = addressing.getAbsolute().value();
            memory[address] = value;
            write_word(address, PC + 1);
            return;
        }
        case AddressingMode::ABSOLUTE_X: {
            const auto [address, index] = addressing.getAbsoluteX().value();
            X = index;
            memory[address + index] = value;
            write_word(address, PC + 1);
            return;
        }
        case AddressingMode::ABSOLUTE_Y: {
            const auto [address, index] = addressing.getAbsoluteY().value();
            Y = index;
            memory[address + index] = value;
            write_word(address, PC + 1);
            return;
        }
        case AddressingMode::INDIRECT: {
            const auto [tableAddress, targetAddress] = addressing.getIndirect().value();
            memory[targetAddress] = value;
            write_word(targetAddress, tableAddress);
            write_word(tableAddress, PC + 1);
            return;
        }
        case AddressingMode::INDIRECT_X: {
            const auto [tableAddress, targetAddress, index] = addressing.getIndirectX().value();
            X = index;
            memory[targetAddress] = value;
            write_word(targetAddress, (Byte)(tableAddress + index));
            memory[PC + 1] = tableAddress;
            return;
        }
        case AddressingMode::INDIRECT_Y: {
            const auto [tableAddress, targetAddress, index] = addressing.getIndirectY().value();
            Y = index;
            memory[targetAddress + index] = value;
            write_word(targetAddress, tableAddress);
            memory[PC + 1] = tableAddress;
            return;
        }
    }

    std::unreachable();
}


void MOS6502_TestFixture::test_load_accumulator(Byte value, const Addressing& addressing) {
    reset();

    write_to_memory(value, addressing);

    const auto [opcode, duration] = [&addressing]() -> std::pair<OpCode, size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::IMMEDIATE:
                return {LDA_IMMEDIATE, 2};
            case AddressingMode::ZERO_PAGE:
                return {LDA_ZERO_PAGE, 3};
            case AddressingMode::ZERO_PAGE_X:
                return {LDA_ZERO_PAGE_X, 4};
            case AddressingMode::ABSOLUTE:
                return {LDA_ABSOLUTE, 4};
            case AddressingMode::ABSOLUTE_X:
                return {LDA_ABSOLUTE_X, 4 + addressing.page_crossed()};
            case AddressingMode::ABSOLUTE_Y:
                return {LDA_ABSOLUTE_Y, 4 + addressing.page_crossed()};
            case AddressingMode::INDIRECT_X:
                return {LDA_INDIRECT_X, 6};
            case AddressingMode::INDIRECT_Y:
                return {LDA_INDIRECT_Y, 5 + addressing.page_crossed()};

            default:
                std::cerr << "test_load_accumulator: provided addressing mode " << addressing.getMode() << " is not supported by LDA instruction\n";
                throw std::runtime_error("unsupported addressing mode");
        }
    }();

    memory[PC] = opcode;
    execute_current_command();

    std::stringstream testID;
    testID << "Test LDA(value: " << (int)value << ", addressing: " << addressing << ")";

    EXPECT_EQ(AC, value) << testID.str();
    EXPECT_EQ(SR, set_register_flags_for(value)) << testID.str();
    EXPECT_EQ(PC, addressing.PC_shift()) << testID.str();
    EXPECT_EQ(cycle, duration) << testID.str();
}

void MOS6502_TestFixture::test_load_X(Byte value, const Addressing &addressing) {
    reset();

    write_to_memory(value, addressing);

    const auto [opcode, duration] = [&addressing]() -> std::pair<OpCode, size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::IMMEDIATE:
                return {LDX_IMMEDIATE, 2};
            case AddressingMode::ZERO_PAGE:
                return {LDX_ZERO_PAGE, 3};
            case AddressingMode::ZERO_PAGE_Y:
                return {LDX_ZERO_PAGE_Y, 4};
            case AddressingMode::ABSOLUTE:
                return {LDX_ABSOLUTE, 4};
            case AddressingMode::ABSOLUTE_Y:
                return {LDX_ABSOLUTE_Y, 4 + addressing.page_crossed()};

            default:
                std::cerr << "test_load_X: provided addressing mode " << addressing.getMode() << " is not supported by LDX instruction\n";
                throw std::runtime_error("unsupported addressing mode");
        }
    }();

    memory[PC] = opcode;
    execute_current_command();

    std::stringstream testID;
    testID << "Test LDX(value: " << (int)value << ", addressing: " << addressing << ")";

    EXPECT_EQ(X, value) << testID.str();
    EXPECT_EQ(SR, set_register_flags_for(value)) << testID.str();
    EXPECT_EQ(PC, addressing.PC_shift()) << testID.str();
    EXPECT_EQ(cycle, duration) << testID.str();
}

void MOS6502_TestFixture::test_load_Y(Byte value, const Addressing &addressing) {
    reset();

    write_to_memory(value, addressing);

    const auto [opcode, duration] = [&addressing]() -> std::pair<OpCode, size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::IMMEDIATE:
                return {LDY_IMMEDIATE, 2};
            case AddressingMode::ZERO_PAGE:
                return {LDY_ZERO_PAGE, 3};
            case AddressingMode::ZERO_PAGE_X:
                return {LDY_ZERO_PAGE_X, 4};
            case AddressingMode::ABSOLUTE:
                return {LDY_ABSOLUTE, 4};
            case AddressingMode::ABSOLUTE_X:
                return {LDY_ABSOLUTE_X, 4 + addressing.page_crossed()};

            default:
                std::cerr << "test_load_Y: provided addressing mode " << addressing.getMode() << " is not supported by LDY instruction\n";
                throw std::runtime_error("unsupported addressing mode");
        }
    }();

    memory[PC] = opcode;
    execute_current_command();

    std::stringstream testID;
    testID << "Test LDY(value: " << (int)value << ", addressing: " << addressing << ")";

    EXPECT_EQ(Y, value) << testID.str();
    EXPECT_EQ(SR, set_register_flags_for(value)) << testID.str();
    EXPECT_EQ(PC, addressing.PC_shift()) << testID.str();
    EXPECT_EQ(cycle, duration) << testID.str();
}

void MOS6502_TestFixture::reset() noexcept {
    PC = 0;
    cycle = 0;
    AC = 0;
    X = 0;
    Y = 0;
    SR = 0;
}
