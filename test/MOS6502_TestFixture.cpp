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

void MOS6502_TestFixture::reset() noexcept {
    PC = 0;
    cycle = 0;
    AC = 0;
    X = 0;
    Y = 0;
    SR = 0;
    SP = 255;
}

std::optional<Address> MOS6502_TestFixture::prepare_memory(const Addressing &addressing) {
    switch (addressing.getMode()) {
        case AddressingMode::IMPLICIT:
            return std::nullopt;
        case AddressingMode::ACCUMULATOR:
            return Register::AC;
        case AddressingMode::IMMEDIATE:
            return std::forward<Word>(PC + 1);
        case AddressingMode::ZERO_PAGE: {
            const auto [address] = addressing.getZeroPage().value();
            memory[PC + 1] = address;
            return {address};
        }
        case AddressingMode::ZERO_PAGE_X: {
            const auto [address, index] = addressing.getZeroPageX().value();
            X = index;
            memory[PC + 1] = address;
            return {std::forward<Word>(address + index)};
        }
        case AddressingMode::ZERO_PAGE_Y: {
            const auto [address, index] = addressing.getZeroPageY().value();
            Y = index;
            memory[PC + 1] = address;
            return {std::forward<Word>(address + index)};
        }
        case AddressingMode::RELATIVE: {
            const auto [PC_, offset] = addressing.getRelative().value();
            PC = PC_;
            memory[PC + 1] = offset;
            return std::nullopt;
        }
        case AddressingMode::ABSOLUTE: {
            const auto [address] = addressing.getAbsolute().value();
            write_word(address, PC + 1);
            return address;
        }
        case AddressingMode::ABSOLUTE_X: {
            const auto [address, index] = addressing.getAbsoluteX().value();
            X = index;
            write_word(address, PC + 1);
            return {std::forward<Word>(address + index)};
        }
        case AddressingMode::ABSOLUTE_Y: {
            const auto [address, index] = addressing.getAbsoluteY().value();
            Y = index;
            write_word(address, PC + 1);
            return {std::forward<Word>(address + index)};
        }
        case AddressingMode::INDIRECT: {
            const auto [tableAddress, targetAddress] = addressing.getIndirect().value();
            write_word(targetAddress, tableAddress);
            write_word(tableAddress, PC + 1);
            return targetAddress;
        }
        case AddressingMode::INDIRECT_X: {
            const auto [tableAddress, targetAddress, index] = addressing.getIndirectX().value();
            X = index;
            write_word(targetAddress, (Byte)(tableAddress + index));
            memory[PC + 1] = tableAddress;
            return targetAddress;
        }
        case AddressingMode::INDIRECT_Y: {
            const auto [tableAddress, targetAddress, index] = addressing.getIndirectY().value();
            Y = index;
            write_word(targetAddress, tableAddress);
            memory[PC + 1] = tableAddress;
            return {std::forward<Word>(targetAddress + index)};
        }
    }

    std::unreachable();
}

void MOS6502_TestFixture::test_transfer(Register from, Register to, Byte value) {
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
                        std::cerr << "test_transfer: cannot move value from AC to " << to << '\n';
                        throw std::invalid_argument("Invalid target register");
                }
            case Register::SP:
                if (to != Register::X) {
                    std::cerr << "test_transfer: cannot move value from SP to " << to << '\n';
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
                        std::cerr << "test_transfer: cannot move value from X to " << to << '\n';
                        throw std::invalid_argument("Invalid target register");
                }
            case Register::Y:
                if (to != Register::AC) {
                    std::cerr << "test_transfer: cannot move value from Y to " << to << '\n';
                    throw std::invalid_argument("Invalid target register");
                }
                return {TYA_IMPLICIT, "TYA"};
            default:
                std::cerr << "test_transfer: cannot move value from " << from << '\n';
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

std::optional<Address> MOS6502_TestFixture::prepare_and_execute(OpCode opcode, std::optional<Byte> value, const Addressing &addressing) {
    const auto address = prepare_memory(addressing);
    if (value.has_value()) (*this)[address.value()] = value.value();
    memory[PC] = opcode;
    execute_current_command();
    return address;
}

void MOS6502_TestFixture::check_register_loading(Register reg, Byte expectedValue, Word expectedPCShift,
                                                 size_t expectedDuration, const std::string &testID) {
    EXPECT_EQ((*this)[reg], expectedValue) << testID;
    EXPECT_EQ(SR, set_register_flags_for(expectedValue)) << testID;
    EXPECT_EQ(PC, expectedPCShift) << testID;
    EXPECT_EQ(cycle, expectedDuration) << testID;
}

std::pair<OpCode, size_t> MOS6502_TestFixture::loading_parameters(Register reg, const Addressing &addressing) {
    switch (reg) {
        case Emulator::Register::AC:
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
                    std::cerr << "loading_parameters: provided addressing mode " << addressing.getMode() << " is not supported by load " << reg << " instruction\n";
                    throw std::runtime_error("unsupported addressing mode");
            }

        case Emulator::Register::X:
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
                    std::cerr << "loading_parameters: provided addressing mode " << addressing.getMode() << " is not supported by load " << reg << " instruction\n";
                    throw std::runtime_error("unsupported addressing mode");
            }

        case Emulator::Register::Y:
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
                    std::cerr << "loading_parameters: provided addressing mode " << addressing.getMode() << " is not supported by load " << reg << " instruction\n";
                    throw std::runtime_error("unsupported addressing mode");
            }

        default:
            std::cerr << "loading_parameters: unsupported register " << reg << " for load instruction\n";
            throw std::runtime_error("unsupported register");
    }
}

void MOS6502_TestFixture::test_loading(Register reg, Byte value, const Addressing &addressing) {
    reset();

    const auto [opcode, duration] = loading_parameters(reg, addressing);
    const std::string commandName = [reg](){
        switch (reg) {
            case Emulator::Register::AC: return "LDA";
            case Emulator::Register::X: return "LDX";
            case Emulator::Register::Y: return "LDY";
            default:
                std::cerr << "test_loading: unsupported register " << reg << " for load instruction\n";
                throw std::runtime_error("unsupported register");
        }
    }();
    std::stringstream testID;
    testID << "Test " << commandName << "(value: " << (int)value << ", addressing: " << addressing << ")";

    prepare_and_execute(opcode, value, addressing);
    check_register_loading(reg, value, addressing.PC_shift(), duration, testID.str());
}

void MOS6502_TestFixture::test_storage(Register reg, Byte value, const Addressing &addressing) {
    reset();

    const auto [opcode, duration] = storage_parameters(reg, addressing);
    const std::string commandName = [reg](){
        switch (reg) {
            case Emulator::Register::AC: return "STA";
            case Emulator::Register::X: return "STX";
            case Emulator::Register::Y: return "STY";
            default:
                std::cerr << "test_storage: unsupported register " << reg << " for store instruction\n";
                throw std::runtime_error("unsupported register");
        }
    }();
    std::stringstream testID;
    testID << "Test " << commandName << "(value: " << (int)value << ", addressing: " << addressing << ")";

    (*this)[reg] = value;
    const auto address = prepare_and_execute(opcode, std::nullopt, addressing);
    ASSERT_TRUE(address.has_value());
    const auto memAddress = std::get_if<Word>(&address.value());
    ASSERT_TRUE(memAddress != nullptr);
    check_register_storage(*memAddress, value, addressing.PC_shift(), duration, testID.str());
}

std::pair<OpCode, size_t> MOS6502_TestFixture::storage_parameters(Register reg, const Addressing &addressing) {
    switch (reg) {
        case Register::AC:
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

        case Register::X:
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

        case Register::Y:
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

        default:
            std::cerr << "test_storage: unsupported register " << reg << " for store instruction\n";
            throw std::runtime_error("unsupported register");
    }
}

void MOS6502_TestFixture::check_register_storage(Word address, Byte expectedValue, Word expectedPCShift,
                                                 size_t expectedDuration, const std::string &testID) {

    EXPECT_EQ(memory[address], expectedValue) << testID;
    EXPECT_EQ(SR, 0) << testID;
    EXPECT_EQ(PC, expectedPCShift) << testID;
    EXPECT_EQ(cycle, expectedDuration) << testID;
}

void MOS6502_TestFixture::test_push_to_stack(Register reg, Byte value) {
    reset();

    const auto &[opCode, commandName] = [reg]() -> std::pair<OpCode, std::string>{
        switch (reg) {
            case Register::AC: return {PHA_IMPLICIT, "PHA"};
            case Register::SR: return {PHP_IMPLICIT, "PHP"};
            default:
                std::cerr << "test_push_to_stack: unsupported register " << reg << " for stack push instruction\n";
                throw std::runtime_error("unsupported register");
        }
    }();

    if (reg == Emulator::Register::AC) AC = value;
    else SR = value;

    memory[PC] = opCode;
    execute_current_command();

    EXPECT_EQ(stack(SP + 1), value);
    EXPECT_EQ(SP, 254);
    EXPECT_EQ(cycle, 3);
    EXPECT_EQ(PC, 1);
}

Byte &MOS6502_TestFixture::stack(Byte address) {
    return memory[STACK_BOTTOM + address];
}

void MOS6502_TestFixture::test_pull_from_stack(Register reg, Byte value) {
    reset();

    const auto &[opCode, commandName] = [reg]() -> std::pair<OpCode, std::string>{
        switch (reg) {
            case Register::AC: return {PLA_IMPLICIT, "PLA"};
            case Register::SR: return {PLP_IMPLICIT, "PLP"};
            default:
                std::cerr << "test_pull_from_stack: unsupported register " << reg << " for stack pull instruction\n";
                throw std::runtime_error("unsupported register");
        }
    }();

    stack(SP--) = value;
    memory[PC] = opCode;
    execute_current_command();

    EXPECT_EQ((reg == Emulator::Register::AC) ? AC : SR, value);
    EXPECT_EQ(SP, 255);
    EXPECT_EQ(cycle, 4);
    EXPECT_EQ(PC, 1);
}
