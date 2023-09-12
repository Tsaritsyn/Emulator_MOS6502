//
// Created by Mikhail on 04/09/2023.
//

#include "MOS6502_TestFixture.hpp"


void MOS6502_TestFixture::prepare_instruction(Byte value, const Addressing &addressing) {
    if (const auto immediate = std::get_if<Immediate>(&addressing)) {
        memory[PC + 1] = value;
    }
    else if (const auto zeroPage = std::get_if<ZeroPage>(&addressing)) {
        memory[zeroPage->address] = value;
        memory[PC + 1] = zeroPage->address;
    }
    else if (const auto zeroPageX = std::get_if<ZeroPageX>(&addressing)) {
        X = zeroPageX->X;
        memory[zeroPageX->address + zeroPageX->X] = value;
        memory[PC + 1] = zeroPageX->address;
    }
    else if (const auto zeroPageY = std::get_if<ZeroPageY>(&addressing)) {
        Y = zeroPageY->Y;
        memory[zeroPageY->address + zeroPageY->Y] = value;
        memory[PC + 1] = zeroPageY->address;
    }
    else if (const auto relative = std::get_if<Relative>(&addressing)) {
        PC += relative->offset + 1;
    }
    else if (const auto absolute = std::get_if<Absolute>(&addressing)) {
        memory[absolute->address] = value;
        write_word(absolute->address, PC + 1);
    }
    else if (const auto absoluteX = std::get_if<AbsoluteX>(&addressing)) {
        X = absoluteX->X;
        memory[absoluteX->address + absoluteX->X] = value;
        write_word(absoluteX->address, PC + 1);
    }
    else if (const auto absoluteY = std::get_if<AbsoluteY>(&addressing)) {
        Y = absoluteY->Y;
        memory[absoluteY->address + absoluteY->Y] = value;
        write_word(absoluteY->address, PC + 1);
    }
    else if (const auto indirect = std::get_if<Indirect>(&addressing)) {
        write_word(indirect->address, PC + 1);
        write_word(indirect->newAddress, indirect->address);
    }
    else if (const auto indirectX = std::get_if<IndirectX>(&addressing)) {
        X = indirectX->X;
        memory[indirectX->targetAddress] = value;
        write_word(indirectX->targetAddress, indirectX->tableAddress + indirectX->X);
        memory[PC + 1] = indirectX->tableAddress;
    }
    else if (const auto indirectY = std::get_if<IndirectY>(&addressing)) {
        Y = indirectY->Y;
        write_word(indirectY->targetAddress, indirectY->tableAddress);
        memory[indirectY->targetAddress + Y] = value;
        memory[PC + 1] = indirectY->tableAddress;
    }
    else throw std::runtime_error("prepare_instruction: unhandled addressing mode type");
}


//void MOS6502_TestFixture::test_ADC(AddressingMode mode, const OperandsAdd &operands, bool crossPage) {
//    // setup
//    SR = 0;
//    PC = 0;
//    cycle = 0;
//
//    AC = operands.AC;
//    SR[CARRY] = operands.carry;
//
//    /// number of cycles to be elapsed
//    size_t expectedCommandDuration;
//
//    // addressing-specific setup
//    switch (mode) {
//        case AddressingMode::IMMEDIATE: {
//            memory[PC] = OpCode::ADC_IMMEDIATE;
//            prepare_instruction(operands.memory, Immediate{});
//
//            expectedCommandDuration = 2;
//        } break;
//
//        case AddressingMode::ZERO_PAGE: {
//            memory[PC] = OpCode::ADC_ZERO_PAGE;
//            prepare_instruction(operands.memory, ZeroPage{0xFF});
//
//            expectedCommandDuration = 3;
//        } break;
//
//        case AddressingMode::ZERO_PAGE_X: {
//            memory[PC] = OpCode::ADC_ZERO_PAGE_X;
//            prepare_instruction(operands.memory, ZeroPageX{0xF0, 0x0F});
//
//            expectedCommandDuration = 4;
//        } break;
//
//        case AddressingMode::ABSOLUTE: {
//            memory[PC] = OpCode::ADC_ABSOLUTE;
//            prepare_instruction(operands.memory, Absolute{0x1234});
//
//            expectedCommandDuration = 4;
//        } break;
//
//        case AddressingMode::ABSOLUTE_X: {
//            memory[PC] = OpCode::ADC_ABSOLUTE_X;
//
//            if (!crossPage) {
//                prepare_instruction(operands.memory, AbsoluteX{0x1200, 0x34});
//                expectedCommandDuration = 4;
//            }
//            else {
//                // crossing page
//                prepare_instruction(operands.memory, AbsoluteX{0x12f0, 0x34});
//                expectedCommandDuration = 5;
//            }
//        } break;
//
//        case AddressingMode::ABSOLUTE_Y: {
//            memory[PC] = OpCode::ADC_ABSOLUTE_Y;
//
//            if (!crossPage) {
//                prepare_instruction(operands.memory, AbsoluteY{0x1200, 0x34});
//                expectedCommandDuration = 4;
//            }
//            else {
//                // crossing page
//                prepare_instruction(operands.memory, AbsoluteY{0x12f0, 0x34});
//                expectedCommandDuration = 5;
//            }
//        } break;
//
//        case AddressingMode::INDIRECT_X: {
//            memory[PC] = OpCode::ADC_INDIRECT_X;
//            prepare_instruction(operands.memory, IndirectX{0x10, 0x1234, 0x0f});
//
//            expectedCommandDuration = 6;
//        } break;
//
//        case AddressingMode::INDIRECT_Y: {
//            memory[PC] = OpCode::ADC_INDIRECT_Y;
//
//            if (!crossPage) {
//                prepare_instruction(operands.memory, IndirectY{0x10, 0x1234, 0x20});
//                expectedCommandDuration = 5;
//            }
//            else {
//                // crossing page
//                prepare_instruction(operands.memory, IndirectY{0x10, 0x12f0, 0x20});
//                expectedCommandDuration = 6;
//            }
//        } break;
//
//        default: {
//            std::stringstream ss;
//            ss << "Unsupported addressing mode " << mode << " for ADC command";
//            throw std::invalid_argument(ss.str());
//        }
//    }
//
//    execute_current_command();
//
//    std::stringstream ss;
//    ss << mode << ": " << (int)operands.AC << " + " << (int)operands.memory << " + " << (int)operands.carry << " = " << (int)operands.result() << "; " << operands.flags();
//
//    EXPECT_EQ(AC, operands.result()) << ss.str();
//    EXPECT_EQ(SR, operands.flags()) << ss.str();
//
//    EXPECT_EQ(cycle, expectedCommandDuration) << ss.str();
//    ASSERT_EQ(PC, instruction_size(mode)) << ss.str();
//}


//void MOS6502_TestFixture::test_AND(AddressingMode mode, const OperandsAnd &operands, bool crossPage) {
//    // setup
//    SR = 0;
//    PC = 0;
//    cycle = 0;
//
//    AC = operands.AC;
//
//    /// number of cycles to be elapsed
//    size_t expectedCommandDuration;
//
//    // addressing-specific setup
//    switch (mode) {
//        case AddressingMode::IMMEDIATE: {
//            memory[PC] = OpCode::AND_IMMEDIATE;
//            prepare_instruction(operands.memory, Immediate{});
//
//            expectedCommandDuration = 2;
//        } break;
//
//        case AddressingMode::ZERO_PAGE: {
//            memory[PC] = OpCode::AND_ZERO_PAGE;
//            prepare_instruction(operands.memory, ZeroPage{0xFF});
//
//            expectedCommandDuration = 3;
//        } break;
//
//        case AddressingMode::ZERO_PAGE_X: {
//            memory[PC] = OpCode::AND_ZERO_PAGE_X;
//            prepare_instruction(operands.memory, ZeroPageX{0xF0, 0x0F});
//
//            expectedCommandDuration = 4;
//        } break;
//
//        case AddressingMode::ABSOLUTE: {
//            memory[PC] = OpCode::AND_ABSOLUTE;
//            prepare_instruction(operands.memory, Absolute{0x1234});
//
//            expectedCommandDuration = 4;
//        } break;
//
//        case AddressingMode::ABSOLUTE_X: {
//            memory[PC] = OpCode::AND_ABSOLUTE_X;
//
//            if (!crossPage) {
//                prepare_instruction(operands.memory, AbsoluteX{0x1200, 0x34});
//                expectedCommandDuration = 4;
//            }
//            else {
//                // crossing page
//                prepare_instruction(operands.memory, AbsoluteX{0x12f0, 0x34});
//                expectedCommandDuration = 5;
//            }
//        } break;
//
//        case AddressingMode::ABSOLUTE_Y: {
//            memory[PC] = OpCode::AND_ABSOLUTE_Y;
//
//            if (!crossPage) {
//                prepare_instruction(operands.memory, AbsoluteY{0x1200, 0x34});
//                expectedCommandDuration = 4;
//            }
//            else {
//                // crossing page
//                prepare_instruction(operands.memory, AbsoluteY{0x12f0, 0x34});
//                expectedCommandDuration = 5;
//            }
//        } break;
//
//        case AddressingMode::INDIRECT_X: {
//            memory[PC] = OpCode::AND_INDIRECT_X;
//            prepare_instruction(operands.memory, IndirectX{0x10, 0x1234, 0x0f});
//
//            expectedCommandDuration = 6;
//        } break;
//
//        case AddressingMode::INDIRECT_Y: {
//            memory[PC] = OpCode::AND_INDIRECT_Y;
//
//            if (!crossPage) {
//                prepare_instruction(operands.memory, IndirectY{0x10, 0x1234, 0x20});
//                expectedCommandDuration = 5;
//            }
//            else {
//                // crossing page
//                prepare_instruction(operands.memory, IndirectY{0x10, 0x12f0, 0x20});
//                expectedCommandDuration = 6;
//            }
//        } break;
//
//        default: {
//            std::stringstream ss;
//            ss << "Unsupported addressing mode " << mode << " for AND command";
//            throw std::invalid_argument(ss.str());
//        }
//    }
//
//    execute_current_command();
//
//    std::stringstream ss;
//    ss << mode << ": " << HEX_BYTE(operands.AC) << " & " << HEX_BYTE(operands.memory) << " = " << HEX_BYTE(operands.result()) << "; " << operands.flags();
//
//    EXPECT_EQ(AC, operands.result()) << ss.str();
//    EXPECT_EQ(SR.to_ulong(), operands.flags().to_ulong()) << ss.str();
//
//    EXPECT_EQ(cycle, expectedCommandDuration) << ss.str();
//    ASSERT_EQ(PC, instruction_size(mode)) << ss.str();
//}


Word MOS6502_TestFixture::instruction_size(AddressingMode mode) {
    switch (mode) {
        case AddressingMode::IMPLICIT: case AddressingMode::ACCUMULATOR:
            return 1;
        case AddressingMode::IMMEDIATE: case AddressingMode::ZERO_PAGE: case AddressingMode::ZERO_PAGE_X: case AddressingMode::ZERO_PAGE_Y: case AddressingMode::RELATIVE:
            return 2;
        case AddressingMode::ABSOLUTE: case AddressingMode::ABSOLUTE_X: case AddressingMode::ABSOLUTE_Y: case AddressingMode::INDIRECT:
            return 3;
        case AddressingMode::INDIRECT_X: case AddressingMode::INDIRECT_Y:
            return 2;
    }

    throw std::runtime_error("MOS6502_TestFixture::instruction_size: unhandled addressing mode");
}


void MOS6502_TestFixture::test_instruction(const InstructionArguments &instruction,
                                           const Addressing &addressing) {
    // setup
    SR = 0;
    PC = 0;
    cycle = 0;
    AC = 0;

    memory[PC] = opcode(instruction_code(instruction), mode(addressing));

    std::optional<Byte> result;

    if (const auto arithmetics = std::get_if<Arithmetics>(&instruction)) {
        result = instruction_result(*arithmetics);
        AC = arithmetics->AC;
        SR[CARRY] = arithmetics->carry;
        prepare_instruction(arithmetics->memory, addressing);
    }


    execute_current_command();

    std::stringstream ss;
    ss << "instruction: " << instruction << "; addressing: " << addressing;

    if (result.has_value()) EXPECT_EQ(AC, result.value()) << ss.str();
    EXPECT_EQ(SR, instruction_flags(instruction)) << ss.str();

    EXPECT_EQ(cycle, instruction_duration(instruction, addressing)) << ss.str();
    ASSERT_EQ(PC, size(addressing)) << ss.str();
}


