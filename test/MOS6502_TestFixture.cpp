//
// Created by Mikhail on 04/09/2023.
//

#include "MOS6502_TestFixture.hpp"


void MOS6502_TestFixture::prepare_instruction(const Addressing &args) {
    if (const auto mode = std::get_if<Immediate>(&args)) {
        memory[PC + 1] = mode->value;
    }
    else if (const auto mode = std::get_if<ZeroPage>(&args)) {
        memory[mode->address] = mode->value;
        memory[PC + 1] = mode->address;
    }
    else if (const auto mode = std::get_if<ZeroPageX>(&args)) {
        X = mode->X;
        memory[mode->address + mode->X] = mode->value;
        memory[PC + 1] = mode->address;
    }
    else if (const auto mode = std::get_if<ZeroPageY>(&args)) {
        Y = mode->Y;
        memory[mode->address + mode->Y] = mode->value;
        memory[PC + 1] = mode->address;
    }
    else if (const auto mode = std::get_if<Relative>(&args)) {
        PC += mode->offset + 1;
    }
    else if (const auto mode = std::get_if<Absolute>(&args)) {
        memory[mode->address] = mode->value;
        write_word(mode->address, PC + 1);
    }
    else if (const auto mode = std::get_if<AbsoluteX>(&args)) {
        X = mode->X;
        memory[mode->address + mode->X] = mode->value;
        write_word(mode->address, PC + 1);
    }
    else if (const auto mode = std::get_if<AbsoluteY>(&args)) {
        Y = mode->Y;
        memory[mode->address + mode->Y] = mode->value;
        write_word(mode->address, PC + 1);
    }
    else if (const auto mode = std::get_if<Indirect>(&args)) {
        write_word(mode->address, PC + 1);
        write_word(mode->newAddress, mode->address);
    }
    else if (const auto mode = std::get_if<IndirectX>(&args)) {
        X = mode->X;
        memory[mode->targetAddress] = mode->value;
        write_word(mode->targetAddress, mode->tableAddress + mode->X);
        memory[PC + 1] = mode->tableAddress;
    }
    else if (const auto mode = std::get_if<IndirectY>(&args)) {
        Y = mode->Y;
        write_word(mode->targetAddress, mode->tableAddress);
        memory[mode->targetAddress + Y] = mode->value;
        memory[PC + 1] = mode->tableAddress;
    }
    else throw std::runtime_error("prepare_instruction: unhandled addressing mode type");
}


void MOS6502_TestFixture::test_ADC(AddressingMode mode, MOS6502_TestFixture::Operands operands, bool crossPage) {
    // setup
    SR = 0;
    PC = 0;
    cycle = 0;

    AC = operands.AC;
    SR[CARRY] = operands.carry;

    /// number of cycles to be elapsed
    size_t expectedCommandDuration;
    /// expected PC shift in bytes
    Word expectedCommandSize;

    // addressing-specific setup
    switch (mode) {
        case AddressingMode::IMMEDIATE: {
            memory[PC] = OpCode::ADC_IMMEDIATE;
            prepare_instruction(Immediate{operands.memory});

            expectedCommandSize = 2;
            expectedCommandDuration = 2;
        } break;

        case AddressingMode::ZERO_PAGE: {
            memory[PC] = OpCode::ADC_ZERO_PAGE;
            prepare_instruction(ZeroPage{0xFF, operands.memory});

            expectedCommandSize = 2;
            expectedCommandDuration = 3;
        } break;

        case AddressingMode::ZERO_PAGE_X: {
            memory[PC] = OpCode::ADC_ZERO_PAGE_X;
            prepare_instruction(ZeroPageX{0xF0, 0x0F, operands.memory});

            expectedCommandSize = 2;
            expectedCommandDuration = 4;
        } break;

        case AddressingMode::ABSOLUTE: {
            memory[PC] = OpCode::ADC_ABSOLUTE;
            prepare_instruction(Absolute{0x1234, operands.memory});

            expectedCommandSize = 3;
            expectedCommandDuration = 4;
        } break;

        case AddressingMode::ABSOLUTE_X: {
            memory[PC] = OpCode::ADC_ABSOLUTE_X;

            if (!crossPage) {
                prepare_instruction(AbsoluteX{0x1200, 0x34, operands.memory});
                expectedCommandDuration = 4;
            }
            else {
                // crossing page
                prepare_instruction(AbsoluteX{0x12f0, 0x34, operands.memory});
                expectedCommandDuration = 5;
            }

            expectedCommandSize = 3;
        } break;

        case AddressingMode::ABSOLUTE_Y: {
            memory[PC] = OpCode::ADC_ABSOLUTE_Y;

            if (!crossPage) {
                prepare_instruction(AbsoluteY{0x1200, 0x34, operands.memory});
                expectedCommandDuration = 4;
            }
            else {
                // crossing page
                prepare_instruction(AbsoluteY{0x12f0, 0x34, operands.memory});
                expectedCommandDuration = 5;
            }

            expectedCommandSize = 3;
        } break;

        case AddressingMode::INDIRECT_X: {
            memory[PC] = OpCode::ADC_INDIRECT_X;
            prepare_instruction(IndirectX{0x10, 0x1234, 0x0f, operands.memory});

            expectedCommandSize = 2;
            expectedCommandDuration = 6;
        } break;

        case AddressingMode::INDIRECT_Y: {
            memory[PC] = OpCode::ADC_INDIRECT_Y;

            if (!crossPage) {
                prepare_instruction(IndirectY{0x10, 0x1234, 0x20, operands.memory});
                expectedCommandDuration = 5;
            }
            else {
                // crossing page
                prepare_instruction(IndirectY{0x10, 0x12f0, 0x20, operands.memory});
                expectedCommandDuration = 6;
            }

            expectedCommandSize = 2;
        } break;

        default: {
            std::stringstream ss;
            ss << "Unsupported addressing mode " << mode << " for ADC command";
            throw std::invalid_argument(ss.str());
        }
    }

    execute_current_command();

    std::stringstream ss;
    ss << mode << ": " << (int)operands.AC << " + " << (int)operands.memory << " + " << (int)operands.carry << " = " << (int)operands.sum() << "; " << operands.flags_sum();

    EXPECT_EQ(AC, operands.sum()) << ss.str();
    EXPECT_EQ(SR, flag_combination(operands.flags_sum())) << ss.str();

    EXPECT_EQ(cycle, expectedCommandDuration) << ss.str();
    ASSERT_EQ(PC, expectedCommandSize) << ss.str();
}


