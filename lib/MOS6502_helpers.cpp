//
// Created by Mikhail on 30/08/2023.
//

#include <iostream>
#include "MOS6502_helpers.hpp"

namespace Emulator {

    void set_bit(Byte &byte, int number, bool value) {
        if (value) byte |= 1 << number;
        else byte &= ~(1 << number);
    }


    bool get_bit(Byte byte, int number) {
        return byte & (1 << number);
    }


    std::ostream &operator<<(std::ostream &os, AddressingMode mode) {
        switch (mode) {
            case AddressingMode::IMPLICIT:
                return os << "Implicit";
            case AddressingMode::ACCUMULATOR:
                return os << "Accumulator";
            case AddressingMode::IMMEDIATE:
                return os << "Immediate";
            case AddressingMode::ZERO_PAGE:
                return os << "Zero Page";
            case AddressingMode::ZERO_PAGE_X:
                return os << "Zero Page,X";
            case AddressingMode::ZERO_PAGE_Y:
                return os << "Zero Page,Y";
            case AddressingMode::RELATIVE:
                return os << "Relative";
            case AddressingMode::ABSOLUTE:
                return os << "Absolute";
            case AddressingMode::ABSOLUTE_X:
                return os << "Absolute,X";
            case AddressingMode::ABSOLUTE_Y:
                return os << "Absolute,Y";
            case AddressingMode::INDIRECT:
                return os << "Indirect";
            case AddressingMode::INDIRECT_X:
                return os << "Indirect,X";
            case AddressingMode::INDIRECT_Y:
                return os << "Indirect,Y";
        }

        throw std::runtime_error("Unhandled addressing mode");
    }


    std::ostream &operator<<(std::ostream &os, Register aRegister) {
        switch (aRegister) {
            case Register::AC:
                return os << 'A';
            case Register::X:
                return os << 'X';
            case Register::Y:
                return os << 'Y';
            case Register::SP:
                return os << "SP";
            case Register::SR:
                return os << "SR";
        }

        throw std::runtime_error("Unhandled register");
    }


    std::ostream &operator<<(std::ostream &os, Flag flag) {
        switch (flag) {
            case NEGATIVE:
                return os << "Negative";
            case OVERFLOW:
                return os << "Overflow";
            case BREAK:
                return os << "Break";
            case DECIMAL:
                return os << "Decimal";
            case INTERRUPT_DISABLE:
                return os << "Interrupt disable";
            case ZERO:
                return os << "Zero";
            case CARRY:
                return os << "Carry";
        }

        throw std::runtime_error("Unhandled flag");
    }


    std::ostream &operator<<(std::ostream &os, Instruction instruction) {
        switch (instruction) {
            case Instruction::ADC:
                return os << "ADC";
            case Instruction::AND:
                return os << "AND";
            case Instruction::ASL:
                return os << "ASL";
            case Instruction::BCC:
                return os << "BCC";
            case Instruction::BCS:
                return os << "BCS";
            case Instruction::BEQ:
                return os << "BEQ";
            case Instruction::BIT:
                return os << "BIT";
            case Instruction::BMI:
                return os << "BMI";
            case Instruction::BNE:
                return os << "BNE";
            case Instruction::BPL:
                return os << "BPL";
            case Instruction::BRK:
                return os << "BRK";
            case Instruction::BVC:
                return os << "BVC";
            case Instruction::BVS:
                return os << "BVS";
            case Instruction::CLC:
                return os << "CLC";
            case Instruction::CLD:
                return os << "CLD";
            case Instruction::CLI:
                return os << "CLI";
            case Instruction::CLV:
                return os << "CLV";
            case Instruction::CMP:
                return os << "CMP";
            case Instruction::CPX:
                return os << "CPX";
            case Instruction::CPY:
                return os << "CPY";
            case Instruction::DEC:
                return os << "DEC";
            case Instruction::DEX:
                return os << "DEX";
            case Instruction::DEY:
                return os << "DEY";
            case Instruction::EOR:
                return os << "EOR";
            case Instruction::INC:
                return os << "INC";
            case Instruction::INX:
                return os << "INX";
            case Instruction::INY:
                return os << "INY";
            case Instruction::JMP:
                return os << "JMP";
            case Instruction::JSR:
                return os << "JSR";
            case Instruction::LDA:
                return os << "LDA";
            case Instruction::LDX:
                return os << "LDX";
            case Instruction::LDY:
                return os << "LDY";
            case Instruction::LSR:
                return os << "LSR";
            case Instruction::NOP:
                return os << "NOP";
            case Instruction::ORA:
                return os << "ORA";
            case Instruction::PHA:
                return os << "PHA";
            case Instruction::PHP:
                return os << "PHP";
            case Instruction::PLA:
                return os << "PLA";
            case Instruction::PLP:
                return os << "PLP";
            case Instruction::ROL:
                return os << "ROL";
            case Instruction::ROR:
                return os << "ROR";
            case Instruction::RTI:
                return os << "RTI";
            case Instruction::RTS:
                return os << "RTS";
            case Instruction::SBC:
                return os << "SBC";
            case Instruction::SEC:
                return os << "SEC";
            case Instruction::SED:
                return os << "SED";
            case Instruction::SEI:
                return os << "SEI";
            case Instruction::STA:
                return os << "STA";
            case Instruction::STX:
                return os << "STX";
            case Instruction::STY:
                return os << "STY";
            case Instruction::TAX:
                return os << "TAX";
            case Instruction::TAY:
                return os << "TAY";
            case Instruction::TSX:
                return os << "TSX";
            case Instruction::TXA:
                return os << "TXA";
            case Instruction::TXS:
                return os << "TXS";
            case Instruction::TYA:
                return os << "TYA";
        }

        throw std::runtime_error("operator<<: unknown instruction");
    }

}
