//
// Created by Mikhail on 28/08/2023.
//

#include <sstream>
#include <iomanip>
#include <bitset>

#include "CPU.hpp"


constexpr CPU::Byte BIT0 = 1;
constexpr CPU::Byte BIT1 = 1 << 1;
constexpr CPU::Byte BIT2 = 1 << 2;
constexpr CPU::Byte BIT3 = 1 << 3;
constexpr CPU::Byte BIT4 = 1 << 4;
constexpr CPU::Byte BIT5 = 1 << 5;
constexpr CPU::Byte BIT6 = 1 << 6;
constexpr CPU::Byte BIT7 = 1 << 7;

#define HEX_FORMAT "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex
#define HEX_CULL_FORMAT std::uppercase << std::setfill('0') << std::setw(2) << std::hex



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
    // TODO: check for forbidden register-mode pairs
    Word address = determine_address(mode);
    Byte value = read_byte(address);
    write_to_register(reg, value);
}



void CPU::write_to_register(Register reg, Byte value) {
    switch (reg) {
        case Register::AC:
            AC = value;
            break;

        case Register::X:
            X = value;
            break;

        case Register::Y:
            Y = value;
            break;
    }
    cycle++;

    // setting flags
    if (value == 0) set_flag(Flag::ZERO);
    if (value & BIT7) set_flag(Flag::NEGATIVE);
}



std::string CPU::dump(bool include_memory) const {
    std::stringstream ss;
    ss << "Registers: AC = " << HEX_FORMAT << (int)AC << ", X = " << HEX_FORMAT << (int)X << ", Y = " << HEX_FORMAT << (int)Y;
    ss << "\nProgram counter = " << HEX_FORMAT << (int)PC << ", Stack pointer = " << HEX_FORMAT << (int)SP;
    ss << "\nFlags: " << std::bitset<8>(SR);
    ss << "\nCurrent cycle = " << cycle;
    ss << "\nZero page: ";
    for (int i = 0x00; i < 0xFF; i++) ss << HEX_FORMAT << (int)memory[i] << ' ';
    ss << "\nStack: ";
    for (int i = 0x0100; i < 0x01ff; i++) ss << HEX_FORMAT << (int)memory[i] << ' ';
    ss << "\nSpecial addresses: non-maskable interrupt handler = " << HEX_FORMAT << (int)memory[0xfffb] << HEX_CULL_FORMAT << (int)memory[0xfffa]
        << ", power on reset location = " << HEX_FORMAT << (int)memory[0xfffd] << HEX_CULL_FORMAT << (int)memory[0xfffc]
        << ", BRK/interrupt request handler = " << HEX_FORMAT << (int)memory[0xffff] << HEX_CULL_FORMAT << (int)memory[0xfffe];

    if (include_memory) {
        ss << "\nRemaining memory:\n";
        for (int i = 0x01ff + 1; i < 0xfffa; i++) ss << HEX_FORMAT << (int)memory[i] << ' ';
        ss << "\nEND OF MEMORY.";
    }
    return ss.str();
}



void CPU::write_byte(CPU::Byte value, CPU::Word address) {
    memory[address] = value;
    cycle++;
}



CPU::Byte CPU::read_from_register(CPU::Register reg) {
    cycle++;
    switch (reg) {
        case Register::AC:
            return AC;
        case Register::X:
            return X;
        case Register::Y:
            return Y;
    }
}



void CPU::store_register(CPU::Register reg, CPU::AddressingMode mode) {
    // TODO: check for forbidden register-mode pairs
    Word address = determine_address(mode);
    Byte value = read_from_register(reg);
    write_byte(value, address);
}



CPU::Word CPU::determine_address(CPU::AddressingMode mode) {
    switch (mode) {
        case AddressingMode::IMPLICIT: case AddressingMode::ACCUMULATOR:
            // the return value does not matter
            return 0;

        case AddressingMode::IMMEDIATE:
            return PC;

        case AddressingMode::ZERO_PAGE:
            return read_current_byte();

        case AddressingMode::ZERO_PAGE_X:
            return read_current_byte() + read_from_register(Register::X);

        case AddressingMode::ZERO_PAGE_Y:
            return read_current_byte() + read_from_register(Register::Y);

        case AddressingMode::RELATIVE:
            PC += (int8_t)read_current_byte() + 1;
            return PC;

        case AddressingMode::ABSOLUTE:
            // in this mode the full 2-byte address is given as the instruction parameter, therefore we have to read 2 bytes
            // first byte is the most significant one
            return read_current_word();

        case AddressingMode::ABSOLUTE_X: {
            // in this mode the full 2-byte address is given as the instruction parameter, therefore we have to read 2 bytes
            // first byte is the most significant one
            Word address = read_current_byte();
            address <<= 8;

            // in this mode we need to add the value in X to the given address and keep track if page was crossed
            Word tmp = read_current_byte();
            tmp += read_from_register(Register::X);
            // if addition of the address with the value in X leads to change of the page, an extra cycle is spent
            if (tmp > UINT8_MAX) cycle++;

            address += tmp;
            return address;
        }

        case AddressingMode::ABSOLUTE_Y: {
            // in this mode the full 2-byte address is given as the instruction parameter, therefore we have to read 2 bytes
            // first byte is the most significant one
            Word address = read_current_byte();
            address <<= 8;

            // in this mode we need to add the value in Y to the given address and keep track if page was crossed
            Word tmp = read_current_byte();
            tmp += read_from_register(Register::Y);
            // if addition of the address with the value in Y leads to change of the page, an extra cycle is spent
            if (tmp > UINT8_MAX) cycle++;

            address += tmp;
            return address;
        }

        case AddressingMode::INDIRECT:
            return read_reversed_word(read_current_word());

        case AddressingMode::INDEXED_INDIRECT: {
            // in this mode we do all operations in zero page
            Byte tableAddress = read_current_byte();

            Byte targetAddress = tableAddress + read_from_register(Register::X);

            // for some reason, these two bytes must be swapped
            // so if memory at targetAddress gives 0x0080, then address will be 0x8000
            return read_reversed_word(targetAddress);
        }

        case AddressingMode::INDIRECT_INDEXED: {
            Byte zeroPageLocation = read_current_byte();

            Word leastSignificantByteOfAddress = read_byte(zeroPageLocation);
            leastSignificantByteOfAddress += read_from_register(Register::Y);
            if (leastSignificantByteOfAddress > UINT8_MAX) cycle++;

            Word address = read_byte(zeroPageLocation + 1);
            address <<= 8;
            address += leastSignificantByteOfAddress;
            return address;
        }
    }
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
    return os;
}



std::ostream& operator <<(std::ostream &os, CPU::Register aRegister) {
    switch (aRegister) {
        case CPU::Register::AC:
            os << 'A';
            break;
        case CPU::Register::X:
            os << 'X';
            break;
        case CPU::Register::Y:
            os << 'Y';
            break;
    }
}






