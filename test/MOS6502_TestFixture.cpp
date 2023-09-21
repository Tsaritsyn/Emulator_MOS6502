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

std::optional<Address> MOS6502_TestFixture::prepare_and_execute(OpCode opcode, std::optional<Byte> value, std::optional<Addressing> addressing) {
    const auto address = (addressing.has_value()) ? prepare_memory(addressing.value()) : std::nullopt;
    if (value.has_value() && address.has_value()) (*this)[address.value()] = value.value();
    memory[PC] = opcode;
    execute_current_command();
    return address;
}

void MOS6502_TestFixture::check_register(Register reg,
                                         Byte expectedValue,
                                         Word expectedPCShift,
                                         size_t expectedDuration,
                                         const std::string &testID,
                                         std::optional<ProcessorStatus> expectedFlags) {
    EXPECT_EQ((*this)[reg], expectedValue) << testID;
    EXPECT_EQ(SR, expectedFlags.value_or(set_register_flags_for(expectedValue))) << testID;
    EXPECT_EQ(PC, expectedPCShift) << testID;
    EXPECT_EQ(cycle, expectedDuration) << testID;
}

void MOS6502_TestFixture::test_loading(Register reg, Byte value, const Addressing &addressing) {
    reset();

    const auto instruction = [reg]() -> std::optional<Instruction> {
        switch (reg) {
            case Register::AC: return Instruction::LDA;
            case Register::X: return Instruction::LDX;
            case Register::Y: return Instruction::LDY;
            default:
                std::cerr << "test_loading: unsupported register " << reg << " for load instruction\n";
                return std::nullopt;
        }
    }();

    const auto opCode = opcode(instruction.value(), addressing.getMode());

    const auto duration = [&addressing, instruction]() -> std::optional<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::IMMEDIATE:   return 2;
            case AddressingMode::ZERO_PAGE:   return 3;
            case AddressingMode::ZERO_PAGE_X: [[fallthrough]];
            case AddressingMode::ZERO_PAGE_Y: [[fallthrough]];
            case AddressingMode::ABSOLUTE:    return 4;
            case AddressingMode::ABSOLUTE_X:  [[fallthrough]];
            case AddressingMode::ABSOLUTE_Y:  return 4 + addressing.page_crossed();
            case AddressingMode::INDIRECT_X:  return 6;
            case AddressingMode::INDIRECT_Y:  return 5 + addressing.page_crossed();
            default:
                std::cerr << "loading_parameters: provided addressing mode " << addressing.getMode() << " is not supported by " << instruction.value() << " instruction\n";
                return std::nullopt;
        }
    }();

    std::stringstream testID;
    testID << "Test " << instruction << "(value: " << (int)value << ", addressing: " << addressing << ")";

    prepare_and_execute(opCode.value(), value, addressing);
    check_register(reg, value, addressing.PC_shift(), duration.value(), testID.str());
}

void MOS6502_TestFixture::test_storage(Register reg, Byte value, const Addressing &addressing) {
    reset();

    const auto instruction = [reg]() -> std::optional<Instruction> {
        switch (reg) {
            case Register::AC: return Instruction::STA;
            case Register::X: return Instruction::STX;
            case Register::Y: return Instruction::STY;
            default:
                std::cerr << "test_loading: unsupported register " << reg << " for store instruction\n";
                return std::nullopt;
        }
    }();

    const auto opCode = opcode(instruction.value(), addressing.getMode());

    const auto duration = [&addressing, instruction]() -> std::optional<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ZERO_PAGE:   return 3;
            case AddressingMode::ZERO_PAGE_X: [[fallthrough]];
            case AddressingMode::ZERO_PAGE_Y: [[fallthrough]];
            case AddressingMode::ABSOLUTE:    return 4;
            case AddressingMode::ABSOLUTE_X:  [[fallthrough]];
            case AddressingMode::ABSOLUTE_Y:  return 5;
            case AddressingMode::INDIRECT_X:  [[fallthrough]];
            case AddressingMode::INDIRECT_Y:  return 6;
            default:
                std::cerr << "loading_parameters: provided addressing mode " << addressing.getMode() << " is not supported by " << instruction.value() << " instruction\n";
                return std::nullopt;
        }
    }();

    std::stringstream testID;
    testID << "Test " << instruction << "(value: " << (int)value << ", addressing: " << addressing << ")";

    (*this)[reg] = value;
    const auto address = prepare_and_execute(opCode.value(), std::nullopt, addressing);
    ASSERT_TRUE(address.has_value());
    const auto memAddress = std::get_if<Word>(&address.value());
    ASSERT_TRUE(memAddress != nullptr);
    check_memory(*memAddress, value, addressing.PC_shift(), duration.value(), testID.str());
}

void MOS6502_TestFixture::check_memory(Word address, Byte expectedValue, Word expectedPCShift, size_t expectedDuration,
                                       const std::string &testID,
                                       ProcessorStatus expectedFlags) {

    EXPECT_EQ(memory[address], expectedValue) << testID;
    EXPECT_EQ(SR, expectedFlags) << testID;
    EXPECT_EQ(PC, expectedPCShift) << testID;
    EXPECT_EQ(cycle, expectedDuration) << testID;
}

void MOS6502_TestFixture::test_push_to_stack(Register reg, Byte value) {
    reset();

    const auto instruction = [reg]() -> std::optional<Instruction> {
        switch (reg) {
            case Register::AC: return Instruction::PHA;
            case Register::SR: return Instruction::PHP;
            default:
                std::cerr << "test_push_to_stack: unsupported register " << reg << " for stack push instruction\n";
                return std::nullopt;
        }
    }();

    std::stringstream testID;
    testID << "Test " << instruction << "(value: " << (int)value << ")";

    if (reg == Emulator::Register::AC) AC = value;
    else SR = value;

    memory[PC] = opcode(instruction.value()).value();
    execute_current_command();

    EXPECT_EQ(stack(SP + 1), value) << testID.str();
    EXPECT_EQ(SP, 254) << testID.str();
    EXPECT_EQ(cycle, 3) << testID.str();
    EXPECT_EQ(PC, 1) << testID.str();
}

Byte &MOS6502_TestFixture::stack(Byte address) {
    return memory[STACK_BOTTOM + address];
}

void MOS6502_TestFixture::test_pull_from_stack(Register reg, Byte value) {
    reset();

    const auto instruction = [reg]() -> std::optional<Instruction > {
        switch (reg) {
            case Register::AC: return Instruction::PLA;
            case Register::SR: return Instruction::PLP;
            default:
                std::cerr << "test_pull_from_stack: unsupported register " << reg << " for stack pull instruction\n";
                return std::nullopt;
        }
    }();

    std::stringstream testID;
    testID << "Test " << instruction << "(value: " << (int)value << ")";

    stack(SP--) = value;
    memory[PC] = opcode(instruction.value()).value();
    execute_current_command();

    EXPECT_EQ((reg == Emulator::Register::AC) ? AC : SR, value) << testID.str();
    EXPECT_EQ(SP, 255) << testID.str();
    EXPECT_EQ(cycle, 4) << testID.str();
    EXPECT_EQ(PC, 1) << testID.str();
}

void MOS6502_TestFixture::test_logical(LogicalOperation operation, Byte value, Byte mem, const Addressing &addressing) {
    reset();

    const auto &[expectedResult, instruction] = [operation, value, mem]() -> std::pair<Byte, Instruction> {
        switch (operation) {
            case LogicalOperation::AND: return {value & mem, Instruction::AND};
            case LogicalOperation::XOR: return {value ^ mem, Instruction::EOR};
            case LogicalOperation::OR:  return {value | mem, Instruction::ORA};
        }
        std::unreachable();
    }();

    const auto duration = [&addressing]() -> std::optional<size_t> {
        switch (addressing.getMode()) {
            case Emulator::AddressingMode::IMMEDIATE:   return 2;
            case Emulator::AddressingMode::ZERO_PAGE:   return 3;
            case Emulator::AddressingMode::ZERO_PAGE_X: [[fallthrough]];
            case Emulator::AddressingMode::ABSOLUTE:    return 4;
            case Emulator::AddressingMode::ABSOLUTE_X:  [[fallthrough]];
            case Emulator::AddressingMode::ABSOLUTE_Y:  return 4 + addressing.page_crossed();
            case Emulator::AddressingMode::INDIRECT_X:  return 6;
            case Emulator::AddressingMode::INDIRECT_Y:  return 5 + addressing.page_crossed();
            default:
                std::cerr << "test_logical: provided addressing mode " << addressing.getMode()
                          << " is not supported by EOR instruction\n";
                return std::nullopt;
        }
    }();

    std::stringstream testID;
    testID << "Test " << instruction << "(AC: " << HEX_BYTE(value) << ", memory: " << HEX_BYTE(mem) << ", addressing: " << addressing << ")";

    AC = value;
    prepare_and_execute(opcode(instruction, addressing.getMode()).value(), mem, addressing);
    check_register(Emulator::Register::AC, expectedResult, addressing.PC_shift(), duration.value(), testID.str());
}

void MOS6502_TestFixture::test_bit_test(Byte value, Byte mem, const Addressing &addressing) {
    reset();

    const auto duration = [&addressing]() -> std::optional<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ZERO_PAGE: return 3;
            case AddressingMode::ABSOLUTE:  return 4;
            default:
                std::cerr << "test_bit_test: provided addressing mode " << addressing.getMode()
                          << " is not supported by BIT instruction\n";
                return std::nullopt;
        }
    }();

    std::stringstream testID;
    testID << "Test BIT(AC: " << HEX_BYTE(value) << ", memory: " << HEX_BYTE(mem) << ", addressing: " << addressing << ")";

    AC = value;
    prepare_and_execute(opcode(Instruction::BIT, addressing.getMode()).value(), mem, addressing);

    ProcessorStatus expectedFlags{};
    expectedFlags[ZERO] = (value & mem) == 0;
    expectedFlags[OVERFLOW] = get_bit(mem, OVERFLOW);
    expectedFlags[NEGATIVE] = get_bit(mem, NEGATIVE);

    EXPECT_EQ(SR, expectedFlags) << testID.str();
    EXPECT_EQ(cycle, duration) << testID.str();
    EXPECT_EQ(PC, addressing.PC_shift()) << testID.str();
}

void MOS6502_TestFixture::test_arithmetics(MOS6502_TestFixture::ArithmeticOperation operation, Byte value, Byte mem,
                                           bool carry, const Addressing &addressing) {
    typedef std::function<std::pair<Byte, ProcessorStatus>(Byte, Byte, bool)> ArithmeticFn;

    reset();

    const auto &[instruction, arithmeticFn] = [operation]() -> std::pair<Instruction, ArithmeticFn> {
        switch (operation) {
            case ArithmeticOperation::ADD: return {Instruction::ADC, ::add_with_carry};
            case ArithmeticOperation::SUB: return {Instruction::SBC, ::subtract_with_carry};
        }
        std::unreachable();
    }();

    const auto &[expectedResult, expectedFlags] = arithmeticFn(value, mem, carry);

    const auto duration = [&addressing, instruction]() -> std::optional<size_t> {
        switch (addressing.getMode()) {
            case Emulator::AddressingMode::IMMEDIATE:   return 2;
            case Emulator::AddressingMode::ZERO_PAGE:   return 3;
            case Emulator::AddressingMode::ZERO_PAGE_X: [[fallthrough]];
            case Emulator::AddressingMode::ABSOLUTE:    return 4;
            case Emulator::AddressingMode::ABSOLUTE_X:  [[fallthrough]];
            case Emulator::AddressingMode::ABSOLUTE_Y:  return 4 + addressing.page_crossed();
            case Emulator::AddressingMode::INDIRECT_X:  return 6;
            case Emulator::AddressingMode::INDIRECT_Y:  return 5 + addressing.page_crossed();
            default:
                std::cerr << "test_arithmetics: provided addressing mode " << addressing.getMode() << " is not supported by " << instruction << " instruction\n";
                return std::nullopt;
        }
    }();

    std::stringstream testID;
    testID << "Test " << instruction << "(AC: " << HEX_BYTE(value) << ", memory: " << HEX_BYTE(mem) << ", carry: " << carry << ", addressing: " << addressing << ")";

    AC = value;
    SR[CARRY] = carry;
    prepare_and_execute(opcode(instruction, addressing.getMode()).value(), mem, addressing);
    check_register(Emulator::Register::AC, expectedResult, addressing.PC_shift(), duration.value(), testID.str(), expectedFlags);
}

void MOS6502_TestFixture::test_compare_register(Register reg, Byte registerValue, const Addressing &addressing,
                                                Byte memoryValue) {
    reset();

    const auto instruction = [reg]() -> std::optional<Instruction> {
        switch (reg) {
            case Register::AC: return Instruction::CMP;
            case Register::X:  return Instruction::CPX;
            case Register::Y:  return Instruction::CPY;
            default:
                std::cerr << "test_compare_register: register " << reg << " cannot be used for comparison command\n";
                return std::nullopt;
        }
    }();

    const auto duration = [&addressing, instruction]() -> std::optional<size_t> {
        switch (addressing.getMode()) {
            case Emulator::AddressingMode::IMMEDIATE:   return 2;
            case Emulator::AddressingMode::ZERO_PAGE:   return 3;
            case Emulator::AddressingMode::ZERO_PAGE_X: [[fallthrough]];
            case Emulator::AddressingMode::ABSOLUTE:    return 4;
            case Emulator::AddressingMode::ABSOLUTE_X:  [[fallthrough]];
            case Emulator::AddressingMode::ABSOLUTE_Y:  return 4 + addressing.page_crossed();
            case Emulator::AddressingMode::INDIRECT_X:  return 6;
            case Emulator::AddressingMode::INDIRECT_Y:  return 5 + addressing.page_crossed();
            default:
                std::cerr << "test_compare_register: provided addressing mode " << addressing.getMode() << " is not supported by " << instruction << " instruction\n";
                return std::nullopt;
        }
    }();

    std::stringstream testID;
    testID << "Test " << instruction << "(register: " << HEX_BYTE(registerValue) << ", memory: " << HEX_BYTE(memoryValue) << ", addressing: " << addressing << ")";

    (*this)[reg] = registerValue;
    prepare_and_execute(opcode(instruction.value(), addressing.getMode()).value(), memoryValue, addressing);

    ProcessorStatus expectedFlags{};
    expectedFlags[ZERO] = registerValue == memoryValue;
    expectedFlags[CARRY] = registerValue >= memoryValue;
    expectedFlags[NEGATIVE] = registerValue < memoryValue;

    EXPECT_EQ(cycle, duration.value());
    EXPECT_EQ(PC, addressing.PC_shift());
    EXPECT_EQ(SR, expectedFlags);
}

void MOS6502_TestFixture::test_deincrement_memory(ChangeByOne operation, Byte value, const Addressing &addressing) {
    reset();

    const auto &[instruction, expectedResult] = [operation, value]() -> std::pair<Instruction, Byte> {
        switch (operation) {
            case ChangeByOne::INCREMENT: return {Instruction::INC, value + 1};
            case ChangeByOne::DECREMENT: return {Instruction::DEC, value - 1};
        }
        std::unreachable();
    }();

    const auto duration = [&addressing, instruction]() -> std::optional<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ZERO_PAGE:   return 5;
            case AddressingMode::ZERO_PAGE_X: [[fallthrough]];
            case AddressingMode::ABSOLUTE:    return 6;
            case AddressingMode::ABSOLUTE_X:  return 7;
            default:
                std::cerr << "test_deincrement_memory: provided addressing mode " << addressing.getMode() << " is not supported by " << instruction << " instruction\n";
                return std::nullopt;
        }
    }();

    std::stringstream testID;
    testID << "Test " << instruction << "(value: " << (int)value << ", addressing: " << addressing << ")";
    const auto address = prepare_and_execute(opcode(instruction, addressing.getMode()).value(), value, addressing);

    ASSERT_TRUE(std::holds_alternative<Word>(address.value())) << testID.str();
    check_memory(std::get<Word>(address.value()), expectedResult, addressing.PC_shift(), duration.value(), testID.str(), set_register_flags_for(expectedResult));
}

void MOS6502_TestFixture::test_deincrement_register(MOS6502_TestFixture::ChangeByOne operation, Byte value, Register reg) {
    reset();

    const auto instruction = [operation, reg]() -> std::optional<Instruction> {
        switch (reg) {
            case Emulator::Register::X:
                switch (operation) {
                    case ChangeByOne::INCREMENT: return Emulator::Instruction::INX;
                    case ChangeByOne::DECREMENT: return Instruction::DEX;
                }
                std::unreachable();
            case Emulator::Register::Y:
                switch (operation) {
                    case ChangeByOne::INCREMENT: return Emulator::Instruction::INY;
                    case ChangeByOne::DECREMENT: return Instruction::DEY;
                }
                std::unreachable();
            default:
                std::cerr << "test_deincrement_register: register " << reg << " cannot be used for increment or decrement command\n";
                return std::nullopt;
        }
    }();

    const auto expectedResult = [operation, value]() -> Byte {
        switch (operation) {
            case ChangeByOne::INCREMENT: return value + 1;
            case ChangeByOne::DECREMENT: return value - 1;
        }
        std::unreachable();
    }();

    std::stringstream testID;
    testID << "Test " << instruction << "(value: " << (int)value << ")";

    (*this)[reg] = value;
    prepare_and_execute(opcode(instruction.value()).value(), value);

    check_register(reg, expectedResult, 1, 2, testID.str());
}
