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


