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

std::optional<Location> MOS6502_TestFixture::prepare_memory(const Addressing &addressing) noexcept {
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

    const auto instructionResult = [from, to]() -> Result<Instruction>{
        switch (from) {
            case Register::AC:
                switch (to) {
                    case Register::Y: return {Instruction::TAY};
                    case Register::X: return {Instruction::TAX};
                    default: break;
                }
            case Register::SP: {
                if (to == Register::X) return {Instruction::TSX};
                else break;
            }
            case Register::X:
                switch (to) {
                    case Register::AC: return {Instruction::TXA};
                    case Register::SP: return {Instruction::TXS};
                    default: break;
                }
            case Register::Y: {
                if (to == Register::AC) return {Instruction::TYA};
                else break;
            }

            default:
                break;
        }

        std::stringstream message;
        message << "test_transfer: cannot move value from " << from << " to " << to << '\n';
        return {message.str()};
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();

    for (const auto instruction: instructionResult) {
        std::stringstream testID;
        testID << "Test " << instruction << "(value: " << (int) value << ")";

        const auto opcodeResult = opcode(instruction);
        ASSERT_FALSE(opcodeResult.failed()) << testID.str() << ' ' << opcodeResult.fail_message();

        for (const auto opCode: opcodeResult) {
            (*this)[from] = value;
            memory[PC] = opCode;
            execute_current_command();

            EXPECT_EQ((*this)[to], value) << testID.str();
            EXPECT_EQ(SR, (to == Register::SP) ? 0 : set_register_flags_for(value)) << testID.str();
            EXPECT_EQ(PC, 1) << testID.str();
            EXPECT_EQ(cycle, 2) << testID.str();
        }
    }
}

Result<std::optional<Location>>
MOS6502_TestFixture::prepare_and_execute(Instruction instruction, std::optional<Byte> value, std::optional<Addressing> addressing) noexcept {
    // if addressing provided, will prepare the memory and return the target address, otherwise returns nullopt
    const auto location = addressing.and_then([this](const Addressing& addr){return prepare_memory(addr);});

    if (value.has_value() && location.has_value()) (*this)[location.value()] = value.value();

    // if addressing provided, will use its mode, otherwise nullopt
    const auto result = opcode(instruction, addressing.and_then([](const Addressing& addr) -> std::optional<AddressingMode> { return addr.getMode(); }));
    if (result.failed()) {
        std::stringstream ss;
        ss << "Could not determine opcode.\n" << result.fail_message();
        return {ss.str()};
    }
    for (const auto opCode: result) memory[PC] = opCode;

    execute_current_command();
    return location;
}

void MOS6502_TestFixture::test_loading(Register reg, Byte value, const Addressing &addressing) {
    reset();

    const auto instructionResult = [reg]() -> Result<Instruction> {
        switch (reg) {
            case Register::AC: return Instruction::LDA;
            case Register::X: return Instruction::LDX;
            case Register::Y: return Instruction::LDY;
            default:
                std::stringstream message;
                message << "test_loading: unsupported register " << reg << " for load instruction\n";
                return {message.str()};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();

    for (const auto instruction: instructionResult) {
        std::stringstream testID;
        testID << "Test " << instruction << "(value: " << (int) value << ", addressing: " << addressing << ")";

        const auto durationResult = [&addressing, instruction]() -> Result<size_t> {
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
                    std::stringstream message;
                    message << "loading_parameters: provided addressing mode " << addressing.getMode()
                              << " is not supported by " << instruction << " instruction\n";
                    return {message.str()};
            }
        }();
        ASSERT_FALSE(durationResult.failed()) << testID.str() << ' ' << durationResult.fail_message();

        for (const auto duration: durationResult) {
            prepare_and_execute(instruction, value, addressing);
            check_location(reg, value, addressing.PC_shift(), duration, testID.str(),
                           set_register_flags_for(value));
        }
    }
}

void MOS6502_TestFixture::test_storage(Register reg, Byte value, const Addressing &addressing) {
    reset();

    const auto instructionResult = [reg]() -> Result<Instruction> {
        switch (reg) {
            case Register::AC: return Instruction::STA;
            case Register::X: return Instruction::STX;
            case Register::Y: return Instruction::STY;
            default:
                std::stringstream message;
                message << "test_loading: unsupported register " << reg << " for store instruction\n";
                return {message.str()};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();
    for (const auto instruction: instructionResult) {
        std::stringstream testID;
        testID << "Test " << instruction << "(value: " << (int) value << ", addressing: " << addressing << ")";

        const auto durationResult = [&addressing, instruction]() -> Result<size_t> {
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
                    std::stringstream message;
                    message << "loading_parameters: provided addressing mode " << addressing.getMode()
                            << " is not supported by " << instruction << " instruction\n";
                    return {message.str()};
            }
        }();
        ASSERT_FALSE(durationResult.failed()) << testID.str() << ' ' << durationResult.fail_message();

        for (const auto duration: durationResult) {
            (*this)[reg] = value;
            const auto locationResult = prepare_and_execute(instruction, std::nullopt, addressing);

            ASSERT_FALSE(locationResult.failed()) << testID.str() << locationResult.fail_message();
            for (const auto location: locationResult) {
                ASSERT_TRUE(location.has_value()) << testID.str();
                check_location(location.value(), value, addressing.PC_shift(), duration, testID.str());
            }
        }
    }
}

void MOS6502_TestFixture::test_push_to_stack(Register reg, Byte value) {
    reset();

    const auto instructionResult = [reg]() -> Result<Instruction> {
        switch (reg) {
            case Register::AC: return Instruction::PHA;
            case Register::SR: return Instruction::PHP;
            default:
                std::stringstream message;
                message << "test_push_to_stack: unsupported register " << reg << " for stack push instruction\n";
                return {message.str()};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();
    for (const auto instruction: instructionResult) {
        std::stringstream testID;
        testID << "Test " << instruction << "(value: " << (int) value << ")";

        // SR cannot be set via [] operator
        if (reg == Emulator::Register::AC) AC = value;
        else SR = value;

        const auto opcodeResult = opcode(instruction);
        ASSERT_FALSE(opcodeResult.failed()) << testID.str() << ' ' << opcodeResult.fail_message();

        for (const auto opCode: opcodeResult) memory[PC] = opCode;
        execute_current_command();

        EXPECT_EQ(stack(SP + 1), value) << testID.str();
        EXPECT_EQ(SP, 254) << testID.str();
        EXPECT_EQ(cycle, 3) << testID.str();
        EXPECT_EQ(PC, 1) << testID.str();
    }
}

Byte &MOS6502_TestFixture::stack(Byte address) noexcept {
    return memory[STACK_BOTTOM + address];
}

void MOS6502_TestFixture::test_pull_from_stack(Register reg, Byte value) {
    reset();

    const auto instructionResult = [reg]() -> Result<Instruction > {
        switch (reg) {
            case Register::AC: return Instruction::PLA;
            case Register::SR: return Instruction::PLP;
            default:
                std::stringstream message;
                message << "test_pull_from_stack: unsupported register " << reg << " for stack push instruction\n";
                return {message.str()};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();

    for (const auto instruction: instructionResult) {
        std::stringstream testID;
        testID << "Test " << instruction << "(value: " << (int)value << ")";

        stack(SP--) = value;

        const auto opcodeResult = opcode(instruction);
        ASSERT_FALSE(opcodeResult.failed()) << opcodeResult.fail_message();

        for (const auto opCode: opcodeResult) {
            memory[PC] = opCode;
            execute_current_command();

            EXPECT_EQ((reg == Emulator::Register::AC) ? AC : SR, value) << testID.str();
            EXPECT_EQ(SP, 255) << testID.str();
            EXPECT_EQ(cycle, 4) << testID.str();
            EXPECT_EQ(PC, 1) << testID.str();
        }

    }


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

    std::stringstream testID;
    testID << "Test " << instruction << "(AC: " << HEX_BYTE(value) << ", memory: " << HEX_BYTE(mem) << ", addressing: " << addressing << ")";

    const auto durationResult = [&addressing]() -> Result<size_t> {
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
                std::stringstream message;
                message << "test_logical: provided addressing mode " << addressing.getMode()
                          << " is not supported by EOR instruction\n";
                return {message.str()};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID.str() << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        AC = value;
        prepare_and_execute(instruction, mem, addressing);
        check_location(Register::AC, expectedResult, addressing.PC_shift(), duration, testID.str(),
                       set_register_flags_for(expectedResult));
    }
}

void MOS6502_TestFixture::test_bit_test(Byte value, Byte mem, const Addressing &addressing) {
    reset();

    std::stringstream testID;
    testID << "Test BIT(AC: " << HEX_BYTE(value) << ", memory: " << HEX_BYTE(mem) << ", addressing: " << addressing << ")";

    const auto durationResult = [&addressing]() -> Result<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ZERO_PAGE: return 3;
            case AddressingMode::ABSOLUTE:  return 4;
            default:
                std::stringstream message;
                message << "test_bit_test: provided addressing mode " << addressing.getMode()
                          << " is not supported by BIT instruction\n";
                return {message.str()};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID.str() << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        AC = value;
        prepare_and_execute(Instruction::BIT, mem, addressing);

        ProcessorStatus expectedFlags{};
        expectedFlags[ZERO] = (value & mem) == 0;
        expectedFlags[OVERFLOW] = get_bit(mem, OVERFLOW);
        expectedFlags[NEGATIVE] = get_bit(mem, NEGATIVE);

        EXPECT_EQ(SR, expectedFlags) << testID.str();
        EXPECT_EQ(cycle, duration) << testID.str();
        EXPECT_EQ(PC, addressing.PC_shift()) << testID.str();
    }
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

    std::stringstream testID;
    testID << "Test " << instruction << "(AC: " << HEX_BYTE(value) << ", memory: " << HEX_BYTE(mem) << ", carry: " << carry << ", addressing: " << addressing << ")";

    const auto &[expectedResult, expectedFlags] = arithmeticFn(value, mem, carry);

    const auto durationResult = [&addressing, instruction]() -> Result<size_t> {
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
                std::stringstream message;
                message << "test_arithmetics: provided addressing mode " << addressing.getMode()
                        << " is not supported by " << instruction << " instruction\n";
                return {message.str()};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID.str() << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        AC = value;
        SR[CARRY] = carry;
        prepare_and_execute(instruction, mem, addressing);
        check_location(Register::AC, expectedResult, addressing.PC_shift(), duration, testID.str(),
                       expectedFlags);
    }
}

void MOS6502_TestFixture::test_compare_register(Register reg, Byte registerValue, const Addressing &addressing,
                                                Byte memoryValue) {
    reset();

    const auto instructionResult = [reg]() -> Result<Instruction> {
        switch (reg) {
            case Register::AC: return Instruction::CMP;
            case Register::X:  return Instruction::CPX;
            case Register::Y:  return Instruction::CPY;
            default:
                std::stringstream message;
                message << "test_compare_register: register " << reg << " cannot be used for comparison command\n";
                return {message.str()};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();

    for (const auto instruction: instructionResult) {
        std::stringstream testID;
        testID << "Test " << instruction << "(register: " << HEX_BYTE(registerValue) << ", memory: "
               << HEX_BYTE(memoryValue) << ", addressing: " << addressing << ")";

        const auto durationResult = [&addressing, instruction]() -> Result<size_t> {
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
                    std::stringstream message;
                    message << "test_compare_register: provided addressing mode " << addressing.getMode()
                              << " is not supported by " << instruction << " instruction\n";
                    return {message.str()};
            }
        }();
        ASSERT_FALSE(durationResult.failed()) << testID.str() << ' ' << durationResult.fail_message();

        for (const auto duration: durationResult) {
            (*this)[reg] = registerValue;
            prepare_and_execute(instruction, memoryValue, addressing);

            ProcessorStatus expectedFlags{};
            expectedFlags[ZERO] = registerValue == memoryValue;
            expectedFlags[CARRY] = registerValue >= memoryValue;
            expectedFlags[NEGATIVE] = registerValue < memoryValue;

            EXPECT_EQ(cycle, duration);
            EXPECT_EQ(PC, addressing.PC_shift());
            EXPECT_EQ(SR, expectedFlags);
        }
    }
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

    std::stringstream testID;
    testID << "Test " << instruction << "(value: " << (int) value << ", addressing: " << addressing << ")";

    const auto durationResult = [&addressing, instruction]() -> Result<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ZERO_PAGE:   return 5;
            case AddressingMode::ZERO_PAGE_X: [[fallthrough]];
            case AddressingMode::ABSOLUTE:    return 6;
            case AddressingMode::ABSOLUTE_X:  return 7;
            default:
                std::stringstream message;
                message << "test_deincrement_memory: provided addressing mode " << addressing.getMode() << " is not supported by " << instruction << " instruction\n";
                return {message.str()};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID.str() << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        const auto locationResult = prepare_and_execute(instruction, value, addressing);
        ASSERT_FALSE(locationResult.failed()) << locationResult.fail_message();

        for (const auto location: locationResult) {
            ASSERT_TRUE(location.has_value()) << testID.str();
            check_location(location.value(), expectedResult, addressing.PC_shift(), duration, testID.str(),
                           set_register_flags_for(expectedResult));
        }
    }
}

void MOS6502_TestFixture::test_deincrement_register(MOS6502_TestFixture::ChangeByOne operation, Byte value, Register reg) {
    reset();

    const auto instructionResult = [operation, reg]() -> Result<Instruction> {
        switch (reg) {
            case Emulator::Register::X: {
                switch (operation) {
                    case ChangeByOne::INCREMENT: return Emulator::Instruction::INX;
                    case ChangeByOne::DECREMENT: return Instruction::DEX;
                }
                std::unreachable();
            }
            case Emulator::Register::Y: {
                switch (operation) {
                    case ChangeByOne::INCREMENT: return Emulator::Instruction::INY;
                    case ChangeByOne::DECREMENT: return Instruction::DEY;
                }
                std::unreachable();
            }
            default:
                std::stringstream message;
                message << "test_deincrement_register: register " << reg << " cannot be used for increment or decrement command\n";
                return {message.str()};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();

    for (const auto instruction: instructionResult) {
        std::stringstream testID;
        testID << "Test " << instruction << "(value: " << (int) value << ")";

        const auto expectedResult = [operation, value]() -> Byte {
            switch (operation) {
                case ChangeByOne::INCREMENT: return value + 1;
                case ChangeByOne::DECREMENT: return value - 1;
            }
            std::unreachable();
        }();

        (*this)[reg] = value;
        prepare_and_execute(instruction, value);

        check_location(reg, expectedResult, 1, 2, testID.str(), set_register_flags_for(expectedResult));
    }
}

void MOS6502_TestFixture::test_shift(MOS6502_TestFixture::ShiftDirection direction,
                                     Byte value,
                                     const Addressing &addressing) {
    reset();

    const auto &[instruction, expectedResult, expectedCarry] = [direction, value]() -> std::tuple<Instruction, Byte, bool> {
        switch (direction) {
            case ShiftDirection::LEFT:  return {Instruction::ASL, value << 1, get_bit(value, 7)};
            case ShiftDirection::RIGHT: return {Instruction::LSR, value >> 1, get_bit(value, 0)};
        }
        std::unreachable();
    }();

    std::stringstream testID;
    testID << "Test " << instruction << "(value: " << HEX_BYTE(value) << ", addressing: " << addressing << ")";

    const auto durationResult = [&addressing, instruction]() -> Result<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ACCUMULATOR: return 2;
            case AddressingMode::ZERO_PAGE:   return 5;
            case AddressingMode::ZERO_PAGE_X: [[fallthrough]];
            case AddressingMode::ABSOLUTE:    return 6;
            case AddressingMode::ABSOLUTE_X:  return 7;
            default:
                std::stringstream message;
                message << "test_shift: provided addressing mode " << addressing.getMode() << " is not supported by " << instruction << " instruction\n";
                return {message.str()};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID.str() << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        ProcessorStatus expectedFlags = set_register_flags_for(expectedResult);
        expectedFlags[CARRY] = expectedCarry;

        const auto locationResult = prepare_and_execute(instruction, value, addressing);
        ASSERT_FALSE(locationResult.failed()) << testID.str() << ' ' << locationResult.fail_message();

        for (const auto location: locationResult) {
            ASSERT_TRUE(location.has_value()) << testID.str();
            check_location(location.value(), expectedResult, addressing.PC_shift(), duration, testID.str(),
                           expectedFlags);
        }
    }
}

void MOS6502_TestFixture::check_location(Location location, Byte expectedValue, Word expectedPC, size_t expectedCycle,
                                         const std::string &testID,
                                         ProcessorStatus expectedFlags) const {
    EXPECT_EQ((*this)[location], expectedValue) << testID;
    EXPECT_EQ(SR, expectedFlags) << testID;
    EXPECT_EQ(PC, expectedPC) << testID;
    EXPECT_EQ(cycle, expectedCycle) << testID;
}

void MOS6502_TestFixture::test_rotate(MOS6502_TestFixture::ShiftDirection direction, Byte value, bool carry,
                                      const Addressing &addressing) {
    reset();

    const auto &[instruction, expectedResult, expectedCarry] = [direction, value, carry]() -> std::tuple<Instruction, Byte, bool> {
        switch (direction) {
            case ShiftDirection::LEFT:  return {Instruction::ROL, (value << 1) + carry, get_bit(value, 7)};
            case ShiftDirection::RIGHT: return {Instruction::ROR, (value >> 1) + ((int)carry << 7), get_bit(value, 0)};
        }
        std::unreachable();
    }();

    std::stringstream testID;
    testID << "Test " << instruction << "(value: " << HEX_BYTE(value) << ", carry: " << carry << ", addressing: " << addressing << ")";

    const auto durationResult = [&addressing, instruction]() -> Result<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ACCUMULATOR: return 2;
            case AddressingMode::ZERO_PAGE:   return 5;
            case AddressingMode::ZERO_PAGE_X: [[fallthrough]];
            case AddressingMode::ABSOLUTE:    return 6;
            case AddressingMode::ABSOLUTE_X:  return 7;
            default:
                std::stringstream message;
                message << "test_rotate: provided addressing mode " << addressing.getMode() << " is not supported by " << instruction << " instruction\n";
                return {message.str()};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID.str() << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        ProcessorStatus expectedFlags = set_register_flags_for(expectedResult);
        expectedFlags[CARRY] = expectedCarry;

        SR[CARRY] = carry;
        const auto locationResult = prepare_and_execute(instruction, value, addressing);
        ASSERT_FALSE(locationResult.failed()) << testID.str() << ' ' << locationResult.fail_message();

        for (const auto location: locationResult) {
            ASSERT_TRUE(location.has_value()) << testID.str();
            check_location(location.value(), expectedResult, addressing.PC_shift(), duration, testID.str(),
                           expectedFlags);
        }
    }
}
