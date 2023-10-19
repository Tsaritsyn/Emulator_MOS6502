//
// Created by Mikhail on 14/09/2023.
//

#include <format>
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
    SR.reset();
    SP = 255;

    memory.reset();
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
            return {std::forward<Word>((Byte)(address + index))};
        }
        case AddressingMode::ZERO_PAGE_Y: {
            const auto [address, index] = addressing.getZeroPageY().value();
            Y = index;
            memory[PC + 1] = address;
            return {std::forward<Word>((Byte)(address + index))};
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
                return {"test_transfer: cannot move value from " + to_string(from) + " to " + to_string(to)};
        }

        std::unreachable();
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();

    for (const auto instruction: instructionResult) {
        std::string testID = std::vformat("Test {}(value: {:d})",
                                          std::make_format_args(to_string(instruction), value));

        const auto opcodeResult = opcode(instruction);
        ASSERT_FALSE(opcodeResult.failed()) << testID << ' ' << opcodeResult.fail_message();

        for (const auto opCode: opcodeResult) {
            (*this)[from] = value;
            memory[PC] = opCode;

            maxNumberOfCommandsToExecute = 1;
            execute();

            EXPECT_EQ((*this)[to], value) << testID;
            EXPECT_EQ(SR, (to == Register::SP) ? ProcessorStatus(0) : set_register_flags_for(value)) << testID;
            EXPECT_EQ(PC, 1) << testID;
            EXPECT_EQ(cycle, 2) << testID;
        }
    }
}

Result<std::optional<Location>>
MOS6502_TestFixture::prepare_and_execute(Instruction instruction, std::optional<Addressing> addressing,
                                         std::optional<Byte> value) noexcept {
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

    maxNumberOfCommandsToExecute = 1;
    execute();
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
                return {"test_loading: unsupported register " + to_string(reg) + " for load instruction"};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();

    for (const auto instruction: instructionResult) {
        std::string testID = std::vformat("Test {}(value: {:d}, addressing: {})",
                                          std::make_format_args(to_string(instruction), value, addressing.to_string()));

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
                    return {"loading_parameters: provided addressing mode " + to_string(addressing.getMode())
                            + " is not supported by " + to_string(instruction) + " instruction"};
            }
        }();
        ASSERT_FALSE(durationResult.failed()) << testID << ' ' << durationResult.fail_message();

        for (const auto duration: durationResult) {
            prepare_and_execute(instruction, addressing, value);
            check_location(reg, value, addressing.PC_shift(), duration, testID,
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
                return {"test_loading: unsupported register " + to_string(reg) + " for store instruction"};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();
    for (const auto instruction: instructionResult) {
        std::string testID = std::vformat("Test {}(value: {:d}, addressing: {})",
                                          std::make_format_args(to_string(instruction), value, addressing.to_string()));

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
                    return {"loading_parameters: provided addressing mode " + to_string(addressing.getMode())
                            + " is not supported by " + to_string(instruction) + " instruction"};
            }
        }();
        ASSERT_FALSE(durationResult.failed()) << testID << ' ' << durationResult.fail_message();

        for (const auto duration: durationResult) {
            (*this)[reg] = value;
            const auto locationResult = prepare_and_execute(instruction, addressing);

            ASSERT_FALSE(locationResult.failed()) << testID << locationResult.fail_message();
            for (const auto location: locationResult) {
                ASSERT_TRUE(location.has_value()) << testID;
                check_location(location.value(), value, addressing.PC_shift(), duration, testID);
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
                return {"test_push_to_stack: unsupported register " + to_string(reg) + " for stack push instruction"};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();
    for (const auto instruction: instructionResult) {
        std::string testID = std::vformat("Test {}(value: {:d})",
                                          std::make_format_args(to_string(instruction), value));

        // SR cannot be set via [] operator
        if (reg == Emulator::Register::AC) AC = value;
        else SR = value;

        const auto opcodeResult = opcode(instruction);
        ASSERT_FALSE(opcodeResult.failed()) << testID << ' ' << opcodeResult.fail_message();

        for (const auto opCode: opcodeResult) memory[PC] = opCode;

        maxNumberOfCommandsToExecute = 1;
        execute();

        EXPECT_EQ(memory.stack(SP + 1), value) << testID;
        EXPECT_EQ(SP, 254) << testID;
        EXPECT_EQ(cycle, 3) << testID;
        EXPECT_EQ(PC, 1) << testID;
    }
}

void MOS6502_TestFixture::test_pull_from_stack(Register reg, Byte value) {
    reset();

    const auto instructionResult = [reg]() -> Result<Instruction > {
        switch (reg) {
            case Register::AC: return Instruction::PLA;
            case Register::SR: return Instruction::PLP;
            default:
                return {"test_pull_from_stack: unsupported register " + to_string(reg) + " for stack push instruction"};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();

    for (const auto instruction: instructionResult) {
        std::string testID = std::vformat("Test {}(value: {:d})",
                                          std::make_format_args(to_string(instruction), value));

        memory.stack(SP--) = value;

        const auto opcodeResult = opcode(instruction);
        ASSERT_FALSE(opcodeResult.failed()) << opcodeResult.fail_message();

        for (const auto opCode: opcodeResult) {
            memory[PC] = opCode;

            maxNumberOfCommandsToExecute = 1;
            execute();

            EXPECT_EQ((reg == Emulator::Register::AC) ? AC : SR, value) << testID;
            EXPECT_EQ(SP, 255) << testID;
            EXPECT_EQ(cycle, 4) << testID;
            EXPECT_EQ(PC, 1) << testID;
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

    std::string testID = std::vformat("Test {}(AC: 0x{:02x}, memory: 0x{:02x}, addressing: {})",
                                            std::make_format_args(to_string(instruction), AC, mem, addressing.to_string()));

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
                return {"test_logical: provided addressing mode " + to_string(addressing.getMode())
                        + " is not supported by EOR instruction"};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        AC = value;
        prepare_and_execute(instruction, addressing, mem);
        check_location(Register::AC, expectedResult, addressing.PC_shift(), duration, testID,
                       set_register_flags_for(expectedResult));
    }
}

void MOS6502_TestFixture::test_bit_test(Byte value, Byte mem, const Addressing &addressing) {
    reset();

    std::string testID = std::vformat("Test BIT(AC: {:#02x}, memory: {:#02x}, addressing: {})",
                                      std::make_format_args(AC, mem, addressing.to_string()));

    const auto durationResult = [&addressing]() -> Result<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ZERO_PAGE: return 3;
            case AddressingMode::ABSOLUTE:  return 4;
            default:
                return {"test_bit_test: provided addressing mode " + to_string(addressing.getMode())
                        + " is not supported by BIT instruction"};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        AC = value;
        prepare_and_execute(Instruction::BIT, addressing, mem);

        ProcessorStatus expectedFlags{};
        expectedFlags[Flag::ZERO] = (value & mem) == 0;
        expectedFlags[Flag::OVERFLOW_F] = get_bit(mem, (int)Flag::OVERFLOW_F);
        expectedFlags[Flag::NEGATIVE] = get_bit(mem, (int)Flag::NEGATIVE);

        EXPECT_EQ(SR, expectedFlags) << testID;
        EXPECT_EQ(cycle, duration) << testID;
        EXPECT_EQ(PC, addressing.PC_shift()) << testID;
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

    std::string testID = std::vformat("Test {}(AC: {:d}, memory: {:d}, carry: {:d}, addressing: {})",
                                      std::make_format_args(to_string(instruction), AC, mem, carry, addressing.to_string()));

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
                return {"test_arithmetics: provided addressing mode " + to_string(addressing.getMode()) + " is not supported by " + to_string(instruction) + " instruction"};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        AC = value;
        SR[Flag::CARRY] = carry;
        prepare_and_execute(instruction, addressing, mem);
        check_location(Register::AC, expectedResult, addressing.PC_shift(), duration, testID,
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
                return {"test_compare_register: register " + to_string(reg) + " cannot be used for comparison command\n"};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();

    for (const auto instruction: instructionResult) {
        std::string testID = std::vformat("Test {}(register: {:d}, memory: {:d}, addressing: {})",
                                          std::make_format_args(to_string(instruction), registerValue, memoryValue, addressing.to_string()));

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
                    return {"test_compare_register: provided addressing mode " + to_string(addressing.getMode())
                            + " is not supported by " + to_string(instruction) + " instruction"};
            }
        }();
        ASSERT_FALSE(durationResult.failed()) << testID << ' ' << durationResult.fail_message();

        for (const auto duration: durationResult) {
            (*this)[reg] = registerValue;
            prepare_and_execute(instruction, addressing, memoryValue);

            ProcessorStatus expectedFlags{};
            expectedFlags[Flag::ZERO] = registerValue == memoryValue;
            expectedFlags[Flag::CARRY] = registerValue >= memoryValue;
            expectedFlags[Flag::NEGATIVE] = registerValue < memoryValue;

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

    std::string testID = std::vformat("Test {}(value: {:d}, addressing: {})",
                                      std::make_format_args(to_string(instruction), value, addressing.to_string()));

    const auto durationResult = [&addressing, instruction]() -> Result<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ZERO_PAGE:   return 5;
            case AddressingMode::ZERO_PAGE_X: [[fallthrough]];
            case AddressingMode::ABSOLUTE:    return 6;
            case AddressingMode::ABSOLUTE_X:  return 7;
            default:
                return {"test_deincrement_memory: provided addressing mode " + to_string(addressing.getMode()) + " is not supported by " + to_string(instruction) + " instruction"};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        const auto locationResult = prepare_and_execute(instruction, addressing, value);
        ASSERT_FALSE(locationResult.failed()) << locationResult.fail_message();

        for (const auto location: locationResult) {
            ASSERT_TRUE(location.has_value()) << testID;
            check_location(location.value(), expectedResult, addressing.PC_shift(), duration, testID,
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
                return {"test_deincrement_register: register " + to_string(reg) + " cannot be used for increment or decrement command"};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();

    for (const auto instruction: instructionResult) {
        std::string testID = std::vformat("Test {}(value: {:d})",
                                          std::make_format_args(to_string(instruction), value));

        const auto expectedResult = [operation, value]() -> Byte {
            switch (operation) {
                case ChangeByOne::INCREMENT: return value + 1;
                case ChangeByOne::DECREMENT: return value - 1;
            }
            std::unreachable();
        }();

        (*this)[reg] = value;
        prepare_and_execute(instruction, std::nullopt, value);

        check_location(reg, expectedResult, 1, 2, testID, set_register_flags_for(expectedResult));
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

    std::string testID = std::vformat("Test {}(value: {:#02x}, addressing: {})",
                                      std::make_format_args(to_string(instruction), value, addressing.to_string()));

    const auto durationResult = [&addressing, instruction]() -> Result<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ACCUMULATOR: return 2;
            case AddressingMode::ZERO_PAGE:   return 5;
            case AddressingMode::ZERO_PAGE_X: [[fallthrough]];
            case AddressingMode::ABSOLUTE:    return 6;
            case AddressingMode::ABSOLUTE_X:  return 7;
            default:
                return {"test_shift: provided addressing mode " + to_string(addressing.getMode()) + " is not supported by " + to_string(instruction) + " instruction"};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        ProcessorStatus expectedFlags = set_register_flags_for(expectedResult);
        expectedFlags[Flag::CARRY] = expectedCarry;

        const auto locationResult = prepare_and_execute(instruction, addressing, value);
        ASSERT_FALSE(locationResult.failed()) << testID << ' ' << locationResult.fail_message();

        for (const auto location: locationResult) {
            ASSERT_TRUE(location.has_value()) << testID;
            check_location(location.value(), expectedResult, addressing.PC_shift(), duration, testID,
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

    std::string testID = std::vformat("Test {}(value: {:#02x}, carry: {:d}, addressing: {})",
                                      std::make_format_args(to_string(instruction), value, carry, addressing.to_string()));

    const auto durationResult = [&addressing, instruction]() -> Result<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ACCUMULATOR: return 2;
            case AddressingMode::ZERO_PAGE:   return 5;
            case AddressingMode::ZERO_PAGE_X: [[fallthrough]];
            case AddressingMode::ABSOLUTE:    return 6;
            case AddressingMode::ABSOLUTE_X:  return 7;
            default:
                return {"test_rotate: provided addressing mode " + to_string(addressing.getMode()) + " is not supported by " + to_string(instruction) + " instruction"};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        ProcessorStatus expectedFlags = set_register_flags_for(expectedResult);
        expectedFlags[Flag::CARRY] = expectedCarry;

        SR[Flag::CARRY] = carry;
        const auto locationResult = prepare_and_execute(instruction, addressing, value);
        ASSERT_FALSE(locationResult.failed()) << testID << ' ' << locationResult.fail_message();

        for (const auto location: locationResult) {
            ASSERT_TRUE(location.has_value()) << testID;
            check_location(location.value(), expectedResult, addressing.PC_shift(), duration, testID,
                           expectedFlags);
        }
    }
}

void MOS6502_TestFixture::test_jump(const Addressing &addressing) {
    reset();

    constexpr Instruction instruction = Instruction::JMP;

    std::string testID = std::vformat("Test {}(addressing: {})",
                                      std::make_format_args(to_string(instruction), addressing.to_string()));

    const auto durationResult = [&addressing]() -> Result<size_t> {
        switch (addressing.getMode()) {
            case AddressingMode::ABSOLUTE: return 3;
            case AddressingMode::INDIRECT: return 5;
            default:
                return {"test_jump: provided addressing mode " + to_string(addressing.getMode()) + " is not supported by " + to_string(instruction) + " instruction"};
        }
    }();
    ASSERT_FALSE(durationResult.failed()) << testID << ' ' << durationResult.fail_message();

    for (const auto duration: durationResult) {
        const auto opcodeResult = opcode(instruction);
        ASSERT_FALSE(opcodeResult.failed()) << testID << ' ' << opcodeResult.fail_message();

        const auto locationResult = prepare_and_execute(instruction, addressing);
        ASSERT_FALSE(locationResult.failed()) << testID << ' ' << locationResult.fail_message();

        switch (addressing.getMode()) {
            case Emulator::AddressingMode::ABSOLUTE:
                EXPECT_EQ(PC, addressing.getAbsolute().value().address) << testID;
                break;
            case Emulator::AddressingMode::INDIRECT:
                EXPECT_EQ(PC, addressing.getIndirect().value().targetAddress) << testID;
                break;
            default:
                std::unreachable();
        }
        EXPECT_EQ(cycle, duration) << testID;
    }
}

void MOS6502_TestFixture::test_jump_to_subroutine(Word address) {
    reset();

    const auto instruction = Instruction::JSR;
    const auto addressing = Addressing::Absolute(address);

    std::string testID = std::vformat("Test {}(addressing: {})",
                                      std::make_format_args(to_string(instruction), addressing.to_string()));

    const WordToBytes pcBuf(PC + 2);

    const auto result = prepare_and_execute(instruction, addressing);
    ASSERT_FALSE(result.failed()) << testID << ' ' << result.fail_message();

    ASSERT_EQ(SP, 253) << testID;
    ASSERT_EQ(memory.stack(255), pcBuf.high) << testID;
    ASSERT_EQ(memory.stack(254), pcBuf.low) << testID;
    ASSERT_EQ(PC, address) << testID;
    ASSERT_EQ(cycle, 6) << testID;
}

void MOS6502_TestFixture::test_return_from_subroutine(Word targetPC) {
    reset();

    const WordToBytes pcBuf(targetPC);
    memory.stack(SP--) = pcBuf.high;
    memory.stack(SP--) = pcBuf.low;

    const auto instruction = Instruction::RTS;

    std::string testID = std::vformat("Test {}(target PC: {:#04x})",
                                      std::make_format_args(to_string(instruction), targetPC));

    const auto result = prepare_and_execute(instruction);
    ASSERT_FALSE(result.failed()) << testID << ' ' << result.fail_message();

    EXPECT_EQ(PC, targetPC + 1) << testID;
    EXPECT_EQ(cycle, 6) << testID;
    EXPECT_EQ(SP, 255) << testID;
}

void MOS6502_TestFixture::test_branch(Flag flag, bool value, bool targetValue, Word initialPC, char offset) {
    reset();
    PC = initialPC;
    SR[flag] = value;

    const auto instructionResult = [flag, targetValue]() -> Result<Instruction> {
        switch (flag) {
            case Flag::NEGATIVE: return (targetValue) ? Instruction::BMI : Instruction::BPL;
            case Flag::CARRY: return (targetValue) ? Instruction::BCS : Instruction::BCC;
            case Flag::ZERO: return (targetValue) ? Instruction::BEQ : Instruction::BNE;
            case Flag::OVERFLOW_F: return (targetValue) ? Instruction::BVS : Instruction::BVC;
            default:
                return {"test_branch: unsupported flag " + to_string(flag) + "for branching"};
        }
    }();
    ASSERT_FALSE(instructionResult.failed()) << instructionResult.fail_message();

    for (const auto instruction: instructionResult) {
        std::string testID = std::vformat("Test {}(flag: {}, given value: {:d}, expected value: {:d}, initial PC: {:#04x}, offset: {:d})",
                                          std::make_format_args(to_string(instruction), to_string(flag), value, targetValue, initialPC, offset));

        const auto addressing = Addressing::Relative(initialPC, offset);

        const auto initialSR = SR;

        const auto executionResult = prepare_and_execute(instruction, addressing);
        ASSERT_FALSE(executionResult.failed()) << testID << ' ' << executionResult.fail_message();

        const auto branchHappened = value == targetValue;
        EXPECT_EQ(PC, (branchHappened) ? (Word)(initialPC + offset + 2) : (Word)(initialPC + 2)) << testID;
        EXPECT_EQ(cycle, (branchHappened) ? 3 : 2) << testID;
        EXPECT_EQ(SR, initialSR);
    }
}

void MOS6502_TestFixture::test_brk(Word initialPC, Word interruptVector) {
    reset();
    PC = initialPC;

    const WordToBytes storedPC(PC + 2);
    constexpr Instruction instruction = Instruction::BRK;

    std::string testID = std::vformat("Test {}(initial PC: {:#04x}, interrupt vector: {:#04x})",
                                      std::make_format_args(to_string(instruction), initialPC, interruptVector));

    write_word(interruptVector, ROM::BRK_HANDLER);
    const auto executionResult = prepare_and_execute(instruction);
    ASSERT_FALSE(executionResult.failed()) << testID << ' ' << executionResult.fail_message();

    EXPECT_EQ(SP, 253) << testID;
    EXPECT_EQ(memory.stack(255), storedPC.high) << testID;
    EXPECT_EQ(memory.stack(254), storedPC.low) << testID;
    EXPECT_EQ(PC, interruptVector) << testID;
    EXPECT_EQ(cycle, 6) << testID;
    EXPECT_EQ(SR[Flag::BREAK], SET) << testID;
}

void MOS6502_TestFixture::test_nop() {
    reset();

    constexpr Instruction instruction = Instruction::NOP;
    std::string testID =  "Test " + to_string(instruction);

    const auto executionResult = prepare_and_execute(instruction);
    ASSERT_FALSE(executionResult.failed()) << testID << ' ' << executionResult.fail_message();

    EXPECT_EQ(cycle, 2) << testID;
    EXPECT_EQ(PC, 1) << testID;
}

void MOS6502_TestFixture::test_return_from_interrupt(Word previousPC, Byte previousSR) {
    reset();

    const WordToBytes previousPCbuf(previousPC);
    memory.stack(SP--) = previousPCbuf.high;
    memory.stack(SP--) = previousPCbuf.low;
    memory.stack(SP--) = previousSR;

    constexpr Instruction instruction = Instruction::RTI;
    std::string testID = std::vformat("Test {}(previous PC: {:#04x}, previous SR: {})",
                                      std::make_format_args(to_string(instruction), previousPC, ProcessorStatus(previousSR).to_string()));

    const auto executionResult = prepare_and_execute(instruction);
    ASSERT_FALSE(executionResult.failed()) << testID << ' ' << executionResult.fail_message();

    EXPECT_EQ(PC, previousPC) << testID;
    EXPECT_EQ(SR, previousSR) << testID;
    EXPECT_EQ(SP, 255) << testID;
    EXPECT_EQ(cycle, 6) << testID;
}

