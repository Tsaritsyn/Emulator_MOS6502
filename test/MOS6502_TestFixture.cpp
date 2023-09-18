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


void MOS6502_TestFixture::test_load_accumulator(Byte value, const Addressing& addressing) {
    reset();

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

    Byte* const valuePtr = prepare_memory(addressing);
    ASSERT_TRUE(valuePtr != nullptr);
    *valuePtr = value;
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

    Byte* const valuePtr = prepare_memory(addressing);
    ASSERT_TRUE(valuePtr != nullptr);
    *valuePtr = value;
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

    Byte* const valuePtr = prepare_memory(addressing);
    ASSERT_TRUE(valuePtr != nullptr);
    *valuePtr = value;
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

void MOS6502_TestFixture::test_store_accumulator(Byte value, const Addressing &addressing) {
    reset();

    const auto [opcode, duration] = [&addressing]() -> std::pair<OpCode, size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ZERO_PAGE:
                return {STA_ZERO_PAGE, 3};
            case AddressingMode::ZERO_PAGE_X:
                return {STA_ZERO_PAGE_X, 4};
            case AddressingMode::ABSOLUTE:
                return {STA_ABSOLUTE, 4};
            case AddressingMode::ABSOLUTE_X:
                return {STA_ABSOLUTE_X, 5};
            case AddressingMode::ABSOLUTE_Y:
                return {STA_ABSOLUTE_Y, 5};
            case AddressingMode::INDIRECT_X:
                return {STA_INDIRECT_X, 6};
            case AddressingMode::INDIRECT_Y:
                return {STA_INDIRECT_Y, 6};

            default:
                std::cerr << "test_store_accumulator: provided addressing mode " << addressing.getMode() << " is not supported by STA instruction\n";
                throw std::runtime_error("unsupported addressing mode");
        }
    }();

    AC = value;
    const Byte* const valuePtr = prepare_memory(addressing);
    memory[PC] = opcode;
    execute_current_command();

    std::stringstream testID;
    testID << "Test STA(value: " << (int)value << ", addressing: " << addressing << ")";

    ASSERT_TRUE(valuePtr != nullptr);
    EXPECT_EQ(*valuePtr, value) << testID.str();
    EXPECT_EQ(SR, 0) << testID.str();
    EXPECT_EQ(PC, addressing.PC_shift()) << testID.str();
//    std::cout << testID.str() << ' ' << cycle << ' ' << duration << ' ' << addressing.page_crossed() << '\n';
    EXPECT_EQ(cycle, duration) << testID.str();
}

Byte* const MOS6502_TestFixture::prepare_memory(const Addressing &addressing) {
    switch (addressing.getMode()) {
        case AddressingMode::IMPLICIT:
            return nullptr;
        case AddressingMode::ACCUMULATOR:
            return &AC;
        case AddressingMode::IMMEDIATE:
            return &memory[PC + 1];
        case AddressingMode::ZERO_PAGE: {
            const auto [address] = addressing.getZeroPage().value();
            memory[PC + 1] = address;
            return &memory[address];
        }
        case AddressingMode::ZERO_PAGE_X: {
            const auto [address, index] = addressing.getZeroPageX().value();
            X = index;
            memory[PC + 1] = address;
            return &memory[address + index];
        }
        case AddressingMode::ZERO_PAGE_Y: {
            const auto [address, index] = addressing.getZeroPageY().value();
            Y = index;
            memory[PC + 1] = address;
            return &memory[address + index];
        }
        case AddressingMode::RELATIVE: {
            const auto [PC_, offset] = addressing.getRelative().value();
            PC = PC_;
            memory[PC + 1] = offset;
            return nullptr;
        }
        case AddressingMode::ABSOLUTE: {
            const auto [address] = addressing.getAbsolute().value();
            write_word(address, PC + 1);
            return &memory[address];
        }
        case AddressingMode::ABSOLUTE_X: {
            const auto [address, index] = addressing.getAbsoluteX().value();
            X = index;
            write_word(address, PC + 1);
            return &memory[address + index];
        }
        case AddressingMode::ABSOLUTE_Y: {
            const auto [address, index] = addressing.getAbsoluteY().value();
            Y = index;
            write_word(address, PC + 1);
            return &memory[address + index];
        }
        case AddressingMode::INDIRECT: {
            const auto [tableAddress, targetAddress] = addressing.getIndirect().value();
            write_word(targetAddress, tableAddress);
            write_word(tableAddress, PC + 1);
            return &memory[targetAddress];
        }
        case AddressingMode::INDIRECT_X: {
            const auto [tableAddress, targetAddress, index] = addressing.getIndirectX().value();
            X = index;
            write_word(targetAddress, (Byte)(tableAddress + index));
            memory[PC + 1] = tableAddress;
            return &memory[targetAddress];
        }
        case AddressingMode::INDIRECT_Y: {
            const auto [tableAddress, targetAddress, index] = addressing.getIndirectY().value();
            Y = index;
            write_word(targetAddress, tableAddress);
            memory[PC + 1] = tableAddress;
            return &memory[targetAddress + index];
        }
    }

    std::unreachable();
}

void MOS6502_TestFixture::test_store_X(Byte value, const Addressing &addressing) {
    reset();

    const auto [opcode, duration] = [&addressing]() -> std::pair<OpCode, size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ZERO_PAGE:
                return {STX_ZERO_PAGE, 3};
            case AddressingMode::ZERO_PAGE_Y:
                return {STX_ZERO_PAGE_Y, 4};
            case AddressingMode::ABSOLUTE:
                return {STX_ABSOLUTE, 4};

            default:
                std::cerr << "test_store_X: provided addressing mode " << addressing.getMode() << " is not supported by STX instruction\n";
                throw std::runtime_error("unsupported addressing mode");
        }
    }();

    X = value;
    const Byte* const valuePtr = prepare_memory(addressing);
    memory[PC] = opcode;
    execute_current_command();

    std::stringstream testID;
    testID << "Test STX(value: " << (int)value << ", addressing: " << addressing << ")";

    ASSERT_TRUE(valuePtr != nullptr);
    EXPECT_EQ(*valuePtr, value) << testID.str();
    EXPECT_EQ(SR, 0) << testID.str();
    EXPECT_EQ(PC, addressing.PC_shift()) << testID.str();
    EXPECT_EQ(cycle, duration) << testID.str();
}

void MOS6502_TestFixture::test_store_Y(Byte value, const Addressing &addressing) {
    reset();

    const auto [opcode, duration] = [&addressing]() -> std::pair<OpCode, size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ZERO_PAGE:
                return {STY_ZERO_PAGE, 3};
            case AddressingMode::ZERO_PAGE_X:
                return {STY_ZERO_PAGE_X, 4};
            case AddressingMode::ABSOLUTE:
                return {STY_ABSOLUTE, 4};

            default:
                std::cerr << "test_store_Y: provided addressing mode " << addressing.getMode() << " is not supported by STY instruction\n";
                throw std::runtime_error("unsupported addressing mode");
        }
    }();

    Y = value;
    const Byte* const valuePtr = prepare_memory(addressing);
    memory[PC] = opcode;
    execute_current_command();

    std::stringstream testID;
    testID << "Test STY(value: " << (int)value << ", addressing: " << addressing << ")";

    ASSERT_TRUE(valuePtr != nullptr);
    EXPECT_EQ(*valuePtr, value) << testID.str();
    EXPECT_EQ(SR, 0) << testID.str();
    EXPECT_EQ(PC, addressing.PC_shift()) << testID.str();
    EXPECT_EQ(cycle, duration) << testID.str();
}

void MOS6502_TestFixture::test_register_transfer(Byte value, Register from, Register to) {
    reset();

    const auto &[opcode, name] = [from, to] -> std::pair<OpCode, std::string>{
        switch (from) {
            case Register::AC:
                switch (to) {
                    case Register::Y:
                        return {TAY_IMPLICIT, "TAY"};
                    case Register::X:
                        return {TAX_IMPLICIT, "TAX"};
                    default:
                        std::cerr << "test_register_transfer: cannot move value from AC to " << to << '\n';
                        throw std::invalid_argument("Invalid target register");
                }
            case Register::SP:
                if (to != Register::X) {
                    std::cerr << "test_register_transfer: cannot move value from SP to " << to << '\n';
                    throw std::invalid_argument("Invalid target register");
                }
                return {TSX_IMPLICIT, "TSX"};
            case Register::X:
                switch (to) {
                    case Register::AC:
                        return {TXA_IMPLICIT, "TXA"};
                    case Register::SP:
                        return {TXS_IMPLICIT, "TXS"};
                    default:
                        std::cerr << "test_register_transfer: cannot move value from X to " << to << '\n';
                        throw std::invalid_argument("Invalid target register");
                }
            case Register::Y:
                if (to != Register::AC) {
                    std::cerr << "test_register_transfer: cannot move value from Y to " << to << '\n';
                    throw std::invalid_argument("Invalid target register");
                }
                return {TYA_IMPLICIT, "TYA"};
            default:
                std::cerr << "test_register_transfer: cannot move value from " << from << '\n';
                throw std::invalid_argument("Invalid source register");
        }
    }();

    (*this)[from] = value;
    memory[PC] = opcode;
    execute_current_command();

    std::stringstream testID;
    testID << "Test " << name << "(value: " << (int)value << ")";

    EXPECT_EQ((*this)[to], value) << testID.str();
    EXPECT_EQ(SR, (to == Register::SP) ? 0 : set_register_flags_for(value)) << testID.str();
    EXPECT_EQ(PC, 1) << testID.str();
    EXPECT_EQ(cycle, 2) << testID.str();
}
