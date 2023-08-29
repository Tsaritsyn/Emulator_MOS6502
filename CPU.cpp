//
// Created by Mikhail on 28/08/2023.
//

#include <sstream>

#include "CPU.hpp"


constexpr CPU::Byte BIT0 = 1;
constexpr CPU::Byte BIT1 = 1 << 1;
constexpr CPU::Byte BIT2 = 1 << 2;
constexpr CPU::Byte BIT3 = 1 << 3;
constexpr CPU::Byte BIT4 = 1 << 4;
constexpr CPU::Byte BIT5 = 1 << 5;
constexpr CPU::Byte BIT6 = 1 << 6;
constexpr CPU::Byte BIT7 = 1 << 7;



void CPU::set_flag(CPU::Flag flag) {
    switch (flag) {
        case Flag::CARRY:
            SR |= BIT0;
            break;
        case Flag::ZERO:
            SR |= BIT1;
            break;
        case Flag::INTERRUPT:
            SR |= BIT2;
            break;
        case Flag::DECIMAL:
            SR |= BIT3;
            break;
        case Flag::BREAK:
            SR |= BIT4;
            break;
        case Flag::OVERFLOW:
            SR |= BIT6;
            break;
        case Flag::NEGATIVE:
            SR |= BIT7;
            break;
    }
}



CPU::Byte CPU::read_current_byte() {
    return read_byte(PC++);
}



CPU::Byte CPU::read_byte(Word address) {
    cycle++;
    return memory[address];
}



void CPU::load_register(Register reg, AddressingMode mode) {
    // it will be determined below
    /// value that will be loaded into the register
    Byte value;

    switch (mode) {
        case AddressingMode::IMMEDIATE:
        {
            value = read_current_byte();
            load_register(AC, value);
        } break;

        case AddressingMode::ZERO_PAGE:
        {
            Byte address = read_current_byte();
            value = read_byte(address);
        } break;

        case AddressingMode::ZERO_PAGE_X:
        {
            Byte address = read_current_byte();

            address += X;
            // this mode requires an extra cycle, which can only be spent on the action above
            // though it is unclear whether this cycle is spent on reading from register X or on addition of addresses
            cycle++;

            value = read_byte(address);
        } break;

        case AddressingMode::ABSOLUTE:
        {
            // in this mode the full 2-byte address is given as the instruction parameter, therefore we have to read 2 bytes
            // first byte is the most significant one
            Word address = read_current_byte();
            address <<= 8;
            address += read_current_byte();

            value = read_byte(address);
        } break;

        case AddressingMode::ABSOLUTE_X:
        {
            // in this mode the full 2-byte address is given as the instruction parameter, therefore we have to read 2 bytes
            // first byte is the most significant one
            Word address = read_current_byte();
            address <<= 8;

            // in this mode we need to add the value in X to the given address and keep track if page was crossed
            Word tmp = read_current_byte();
            tmp += X;
            // if addition of the address with the value in X leads to change of the page, an extra cycle is spent
            if (tmp > UINT8_MAX) cycle++;

            address += tmp;

            value = read_byte(address);
        } break;

        case AddressingMode::ABSOLUTE_Y:
        {
            // in this mode the full 2-byte address is given as the instruction parameter, therefore we have to read 2 bytes
            // first byte is the most significant one
            Word address = read_current_byte();
            address <<= 8;

            // in this mode we need to add the value in Y to the given address and keep track if page was crossed
            Word tmp = read_current_byte();
            tmp += Y;
            // if addition of the address with the value in Y leads to change of the page, an extra cycle is spent
            if (tmp > UINT8_MAX) cycle++;

            address += tmp;

            value = read_byte(address);
        } break;

        case AddressingMode::INDEXED_INDIRECT:
        {
            // in this mode we do all operations in zero page
            Byte tableAddress = read_current_byte();

            Byte targetAddress = tableAddress + X;
            cycle++;

            // for some reason, these two bytes must be swapped
            // so if memory at targetAddress gives 0x0080, then address will be 0x8000
            Word address = read_byte(targetAddress + 1);
            address << 8;
            address += read_byte(targetAddress);

            value = read_byte(address);
        } break;

        case AddressingMode::INDIRECT_INDEXED:
        {
            Byte zeroPageLocation = read_current_byte();

            Word leastSignificantByteOfAddress = read_byte(zeroPageLocation);
            leastSignificantByteOfAddress += Y;
            if (leastSignificantByteOfAddress > UINT8_MAX) cycle++;

            Word address = read_byte(zeroPageLocation + 1);
            address <<= 8;
            address += leastSignificantByteOfAddress;

            value = read_byte(address);
        } break;

        default:
            std::stringstream errorMessage;
            errorMessage << "Unsupported addressing mode for LDA instruction: " << mode;
            throw std::invalid_argument(errorMessage.str());
    }

    load_register(AC, value);
}



void CPU::load_register(CPU::Byte &reg, CPU::Byte value) {
    reg = value;
    cycle++;

    // setting flags
    if (reg == 0) set_flag(Flag::ZERO);
    if (reg & BIT7) set_flag(Flag::NEGATIVE);
}



std::ostream& operator <<(std::ostream &os, CPU::AddressingMode mode) {
    switch (mode) {
        case CPU::AddressingMode::IMPLICIT:
            os << "Implicit";
            break;
        case CPU::AddressingMode::ACCUMULATOR:
            os << "Accumulator";
            break;
        case CPU::AddressingMode::IMMEDIATE:
            os << "Immediate";
            break;
        case CPU::AddressingMode::ZERO_PAGE:
            os << "Zero Page";
            break;
        case CPU::AddressingMode::ZERO_PAGE_X:
            os << "Zero Page,X";
            break;
        case CPU::AddressingMode::ZERO_PAGE_Y:
            os << "Zero Page,Y";
            break;
        case CPU::AddressingMode::RELATIVE:
            os << "Relative";
            break;
        case CPU::AddressingMode::ABSOLUTE:
            os << "Absolute";
            break;
        case CPU::AddressingMode::ABSOLUTE_X:
            os << "Absolute,X";
            break;
        case CPU::AddressingMode::ABSOLUTE_Y:
            os << "Absolute,Y";
            break;
        case CPU::AddressingMode::INDIRECT:
            os << "Indirect";
            break;
        case CPU::AddressingMode::INDEXED_INDIRECT:
            os << "Indexed Indirect";
            break;
        case CPU::AddressingMode::INDIRECT_INDEXED:
            os << "Indirect Indexed";
            break;
    }
}






