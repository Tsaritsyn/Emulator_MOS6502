//
// Created by Mikhail on 04/09/2023.
//

#include "MOS6502_TestFixture.hpp"


void MOS6502_TestFixture::write_argument(Byte value, const Addressing &addressing) {
    if (const auto implicit = std::get_if<Implicit>(&addressing))
        return;
    if (const auto accumulator = std::get_if<Accumulator>(&addressing)) {
        AC = value;
        return;
    }
    if (const auto immediate = std::get_if<Immediate>(&addressing)) {
        memory[PC + 1] = value;
        return;
    }
    if (const auto zeroPage = std::get_if<ZeroPage>(&addressing)) {
        memory[zeroPage->address] = value;
        memory[PC + 1] = zeroPage->address;
        return;
    }
    if (const auto zeroPageX = std::get_if<ZeroPageX>(&addressing)) {
        X = zeroPageX->X;
        memory[zeroPageX->address + zeroPageX->X] = value;
        memory[PC + 1] = zeroPageX->address;
        return;
    }
    if (const auto zeroPageY = std::get_if<ZeroPageY>(&addressing)) {
        Y = zeroPageY->Y;
        memory[zeroPageY->address + zeroPageY->Y] = value;
        memory[PC + 1] = zeroPageY->address;
        return;
    }
    if (const auto relative = std::get_if<Relative>(&addressing)) {
        PC += relative->offset + 1;
        return;
    }
    if (const auto absolute = std::get_if<Absolute>(&addressing)) {
        memory[absolute->address] = value;
        write_word(absolute->address, PC + 1);
        return;
    }
    if (const auto absoluteX = std::get_if<AbsoluteX>(&addressing)) {
        X = absoluteX->X;
        memory[absoluteX->address + absoluteX->X] = value;
        write_word(absoluteX->address, PC + 1);
        return;
    }
    if (const auto absoluteY = std::get_if<AbsoluteY>(&addressing)) {
        Y = absoluteY->Y;
        memory[absoluteY->address + absoluteY->Y] = value;
        write_word(absoluteY->address, PC + 1);
        return;
    }
    if (const auto indirect = std::get_if<Indirect>(&addressing)) {
        write_word(indirect->address, PC + 1);
        write_word(indirect->newAddress, indirect->address);
        return;
    }
    if (const auto indirectX = std::get_if<IndirectX>(&addressing)) {
        X = indirectX->X;
        memory[indirectX->targetAddress] = value;
        write_word(indirectX->targetAddress, indirectX->tableAddress + indirectX->X);
        memory[PC + 1] = indirectX->tableAddress;
        return;
    }
    if (const auto indirectY = std::get_if<IndirectY>(&addressing)) {
        Y = indirectY->Y;
        write_word(indirectY->targetAddress, indirectY->tableAddress);
        memory[indirectY->targetAddress + Y] = value;
        memory[PC + 1] = indirectY->tableAddress;
        return;
    }

    throw std::runtime_error("write_argument: unhandled addressing mode type");
}


void MOS6502_TestFixture::test_instruction(const InstructionArguments &instruction,
                                           const Addressing &addressing) {
    // setup
    SR = 0;
    PC = 0;
    cycle = 0;
    AC = 0;

    const auto opcodeOptional = opcode(instruction_code(instruction), mode(addressing));
    assert(opcodeOptional.has_value());
    memory[PC] = opcodeOptional.value();

    std::optional<Byte> expectedResult;
    const Byte* obtainedResultPtr = nullptr;

    if (const auto arithmetics = std::get_if<Arithmetics>(&instruction)) {
        expectedResult = instruction_result(*arithmetics);
        SR[CARRY] = arithmetics->carry;
        write_argument(arithmetics->AC, Accumulator{});
        write_argument(arithmetics->memory, addressing);
        obtainedResultPtr = &AC;
    }

    else if (const auto logical = std::get_if<Logical>(&instruction)) {
        expectedResult = instruction_result(*logical);
        write_argument(logical->AC, Accumulator{});
        write_argument(logical->memory, addressing);
        obtainedResultPtr = &AC;
    }

    else if (const auto shiftLeft = std::get_if<ShiftLeft>(&instruction)) {
        expectedResult = instruction_result(*shiftLeft);
        write_argument(shiftLeft->value, addressing);
        obtainedResultPtr = point_to_value(addressing);
    }


    execute_current_command();

    std::stringstream ss;
    ss << "instruction: " << instruction << "; addressing: " << addressing;

    if (expectedResult.has_value()) {
        ASSERT_FALSE(obtainedResultPtr == nullptr);
        EXPECT_EQ(*obtainedResultPtr, expectedResult.value()) << ss.str();
    }
    EXPECT_EQ(SR, instruction_flags(instruction)) << ss.str();

    EXPECT_EQ(cycle, instruction_duration(instruction, addressing)) << ss.str();
    ASSERT_EQ(PC, size(addressing)) << ss.str();
}

const Byte *MOS6502_TestFixture::point_to_value(const Addressing &addressing) const {
    if (const auto implicit = std::get_if<Implicit>(&addressing))
        return nullptr;
    if (const auto accumulator = std::get_if<Accumulator>(&addressing))
        return &AC;
    if (const auto immediate = std::get_if<Immediate>(&addressing))
        return &memory[PC + 1];
    if (const auto zeroPage = std::get_if<ZeroPage>(&addressing))
        return &memory[zeroPage->address];
    if (const auto zeroPageX = std::get_if<ZeroPageX>(&addressing))
        return &memory[zeroPageX->address + zeroPageX->X];
    if (const auto zeroPageY = std::get_if<ZeroPageY>(&addressing))
        return &memory[zeroPageY->address + zeroPageY->Y];
    if (const auto relative = std::get_if<Relative>(&addressing))
        return &memory[PC + relative->offset + 1];
    if (const auto absolute = std::get_if<Absolute>(&addressing))
        return &memory[absolute->address];
    if (const auto absoluteX = std::get_if<AbsoluteX>(&addressing))
        return &memory[absoluteX->address + absoluteX->X];
    if (const auto absoluteY = std::get_if<AbsoluteY>(&addressing))
        return &memory[absoluteY->address + absoluteY->Y];
    if (const auto indirect = std::get_if<Indirect>(&addressing))
        return &memory[indirect->newAddress];
    if (const auto indirectX = std::get_if<IndirectX>(&addressing))
        return &memory[indirectX->targetAddress];
    if (const auto indirectY = std::get_if<IndirectY>(&addressing))
        return &memory[indirectY->targetAddress + Y];

    throw std::runtime_error("point_to_value: unhandled addressing mode type");
}


