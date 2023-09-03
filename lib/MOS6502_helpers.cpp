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


    bool check_bit(Byte byte, int number) {
        return byte & (1 << number);
    }


    Byte flag_combination(const std::vector<Flag> &flags) {
        Byte result = 0;
        for (auto flag: flags) set_bit(result, flag);
        return result;
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
                return os << "Indexed Indirect";
            case AddressingMode::INDIRECT_Y:
                return os << "Indirect Indexed";
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


    OpCode opcode(Instruction instruction, AddressingMode mode) {
        switch (instruction) {
            case Instruction::ADC:
                switch (mode) {
                    case AddressingMode::IMMEDIATE:
                        return ADC_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:
                        return ADC_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return ADC_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return ADC_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return ADC_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:
                        return ADC_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:
                        return ADC_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:
                        return ADC_INDIRECT_Y;

                    default:
                        break;
                }

            case Instruction::AND:
                switch (mode) {
                    case AddressingMode::IMMEDIATE:
                        return AND_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:
                        return AND_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return AND_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return AND_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return AND_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:
                        return AND_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:
                        return AND_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:
                        return AND_INDIRECT_Y;

                    default:
                        break;
                }

            case Instruction::ASL:
                switch (mode) {
                    case AddressingMode::ACCUMULATOR:
                        return ASL_ACCUMULATOR;
                    case AddressingMode::ZERO_PAGE:
                        return ASL_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return ASL_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return ASL_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return ASL_ABSOLUTE_X;

                    default:
                        break;
                }

            case Instruction::BCC:
                if (mode == AddressingMode::RELATIVE) return BCC;
                else break;

            case Instruction::BCS:
                if (mode == AddressingMode::RELATIVE) return BCS;
                else break;

            case Instruction::BEQ:
                if (mode == AddressingMode::RELATIVE) return BEQ;
                else break;

            case Instruction::BIT:
                switch (mode) {
                    case AddressingMode::ZERO_PAGE:
                        return BIT_ZERO_PAGE;
                    case AddressingMode::ABSOLUTE:
                        return BIT_ABSOLUTE;

                    default:
                        break;
                }

            case Instruction::BMI:
                if (mode == AddressingMode::RELATIVE) return BMI;
                else break;

            case Instruction::BNE:
                if (mode == AddressingMode::RELATIVE) return BNE;
                else break;

            case Instruction::BPL:
                if (mode == AddressingMode::RELATIVE) return BPL;
                else break;

            case Instruction::BRK:
                if (mode == AddressingMode::IMPLICIT) return BRK;
                else break;

            case Instruction::BVC:
                if (mode == AddressingMode::RELATIVE) return BVC;
                else break;

            case Instruction::BVS:
                if (mode == AddressingMode::RELATIVE) return BVS;
                else break;

            case Instruction::CLC:
                if (mode == AddressingMode::IMPLICIT) return CLC;
                else break;

            case Instruction::CLD:
                if (mode == AddressingMode::IMPLICIT) return CLD;
                else break;

            case Instruction::CLI:
                if (mode == AddressingMode::IMPLICIT) return CLI;
                else break;

            case Instruction::CLV:
                if (mode == AddressingMode::IMPLICIT) return CLV;
                else break;

            case Instruction::CMP:
                switch (mode) {
                    case AddressingMode::IMMEDIATE:
                        return CMP_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:
                        return CMP_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return CMP_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return CMP_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return CMP_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:
                        return CMP_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:
                        return CMP_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:
                        return CMP_INDIRECT_Y;

                    default:
                        break;
                }

            case Instruction::CPX:
                switch (mode) {
                    case AddressingMode::IMMEDIATE:
                        return CPX_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:
                        return CPX_ZERO_PAGE;
                    case AddressingMode::ABSOLUTE:
                        return CPX_ABSOLUTE;

                    default:
                        break;
                }

            case Instruction::CPY:
                switch (mode) {
                    case AddressingMode::IMMEDIATE:
                        return CPY_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:
                        return CPY_ZERO_PAGE;
                    case AddressingMode::ABSOLUTE:
                        return CPY_ABSOLUTE;

                    default:
                        break;
                }

            case Instruction::DEC:
                switch (mode) {
                    case AddressingMode::ZERO_PAGE:
                        return DEC_ZER0_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return DEC_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return DEC_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return DEC_ABSOLUTE_X;

                    default:
                        break;
                }

            case Instruction::DEX:
                if (mode == AddressingMode::IMPLICIT) return DEX;
                else break;

            case Instruction::DEY:
                if (mode == AddressingMode::IMPLICIT) return DEY;
                else break;

            case Instruction::EOR:
                switch (mode) {
                    case AddressingMode::IMMEDIATE:
                        return EOR_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:
                        return EOR_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return EOR_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return EOR_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return EOR_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:
                        return EOR_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:
                        return EOR_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:
                        return EOR_INDIRECT_Y;

                    default:
                        break;
                }

            case Instruction::INC:
                switch (mode) {
                    case AddressingMode::ZERO_PAGE:
                        return INC_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return INC_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return INC_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return INC_ABSOLUTE_X;

                    default:
                        break;
                }

            case Instruction::INX:
                if (mode == AddressingMode::IMPLICIT) return INX;
                else break;

            case Instruction::INY:
                if (mode == AddressingMode::IMPLICIT) return INY;
                else break;

            case Instruction::JMP:
                switch (mode) {
                    case AddressingMode::ABSOLUTE:
                        return JMP_ABSOLUTE;
                    case AddressingMode::INDIRECT:
                        return JMP_INDIRECT;

                    default:
                        break;
                }

            case Instruction::JSR:
                if (mode == AddressingMode::ABSOLUTE) return JSR;
                else break;

            case Instruction::LDA:
                switch (mode) {
                    case AddressingMode::IMMEDIATE:
                        return LDA_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:
                        return LDA_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return LDA_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return LDA_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return LDA_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:
                        return LDA_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:
                        return LDA_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:
                        return LDA_INDIRECT_Y;

                    default:
                        break;
                }

            case Instruction::LDX:
                switch (mode) {
                    case AddressingMode::IMMEDIATE:
                        return LDX_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:
                        return LDX_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_Y:
                        return LDX_ZERO_PAGE_Y;
                    case AddressingMode::ABSOLUTE:
                        return LDX_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_Y:
                        return LDX_ABSOLUTE_Y;

                    default:
                        break;
                }

            case Instruction::LDY:
                switch (mode) {
                    case AddressingMode::IMMEDIATE:
                        return LDY_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:
                        return LDY_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return LDY_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return LDY_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return LDY_ABSOLUTE_X;

                    default:
                        break;
                }

            case Instruction::LSR:
                switch (mode) {
                    case AddressingMode::ACCUMULATOR:
                        return LSR_ACCUMULATOR;
                    case AddressingMode::ZERO_PAGE:
                        return LSR_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return LSR_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return LSR_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return LSR_ABSOLUTE_X;

                    default:
                        break;
                }

            case Instruction::NOP:
                if (mode == AddressingMode::IMPLICIT) return NOP;
                else break;

            case Instruction::ORA:
                switch (mode) {
                    case AddressingMode::IMMEDIATE:
                        return ORA_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:
                        return ORA_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return ORA_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return ORA_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return ORA_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:
                        return ORA_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:
                        return ORA_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:
                        return ORA_INDIRECT_Y;

                    default:
                        break;
                }

            case Instruction::PHA:
                if (mode == AddressingMode::IMPLICIT) return PHA;
                else break;

            case Instruction::PHP:
                if (mode == AddressingMode::IMPLICIT) return PHP;
                else break;

            case Instruction::PLA:
                if (mode == AddressingMode::IMPLICIT) return PLA;
                else break;

            case Instruction::PLP:
                if (mode == AddressingMode::IMPLICIT) return PLP;
                else break;

            case Instruction::ROL:
                switch (mode) {
                    case AddressingMode::ACCUMULATOR:
                        return ROL_ACCUMULATOR;
                    case AddressingMode::ZERO_PAGE:
                        return ROL_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return ROL_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return ROL_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return ROL_ABSOLUTE_X;

                    default:
                        break;
                }

            case Instruction::ROR:
                switch (mode) {
                    case AddressingMode::ACCUMULATOR:
                        return ROR_ACCUMULATOR;
                    case AddressingMode::ZERO_PAGE:
                        return ROR_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return ROR_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return ROR_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return ROR_ABSOLUTE_X;

                    default:
                        break;
                }

            case Instruction::RTI:
                if (mode == AddressingMode::IMPLICIT) return RTI;
                else break;

            case Instruction::RTS:
                if (mode == AddressingMode::IMPLICIT) return RTS;
                else break;

            case Instruction::SBC:
                switch (mode) {
                    case AddressingMode::IMMEDIATE:
                        return SBC_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:
                        return SBC_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return SBC_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return SBC_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return SBC_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:
                        return SBC_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:
                        return SBC_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:
                        return SBC_INDIRECT_Y;

                    default:
                        break;
                }

            case Instruction::SEC:
                if (mode == AddressingMode::IMPLICIT) return SEC;
                else break;

            case Instruction::SED:
                if (mode == AddressingMode::IMPLICIT) return SED;
                else break;

            case Instruction::SEI:
                if (mode == AddressingMode::IMPLICIT) return SEI;
                else break;

            case Instruction::STA:
                switch (mode) {
                    case AddressingMode::ZERO_PAGE:
                        return STA_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return STA_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return STA_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:
                        return STA_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:
                        return STA_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:
                        return STA_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:
                        return STA_INDIRECT_Y;

                    default:
                        break;
                }

            case Instruction::STX:
                switch (mode) {
                    case AddressingMode::ZERO_PAGE:
                        return STX_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_Y:
                        return STX_ZERO_PAGE_Y;
                    case AddressingMode::ABSOLUTE:
                        return STX_ABSOLUTE;

                    default:
                        break;
                }

            case Instruction::STY:
                switch (mode) {
                    case AddressingMode::ZERO_PAGE:
                        return STY_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X:
                        return STY_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:
                        return STY_ABSOLUTE;

                    default:
                        break;
                }

            case Instruction::TAX:
                if (mode == AddressingMode::IMPLICIT) return TAX;
                else break;

            case Instruction::TAY:
                if (mode == AddressingMode::IMPLICIT) return TAY;
                else break;

            case Instruction::TSX:
                if (mode == AddressingMode::IMPLICIT) return TSX;
                else break;

            case Instruction::TXA:
                if (mode == AddressingMode::IMPLICIT) return TXA;
                else break;

            case Instruction::TXS:
                if (mode == AddressingMode::IMPLICIT) return TXS;
                else break;

            case Instruction::TYA:
                if (mode == AddressingMode::IMPLICIT) return TYA;
                else break;
        }

        std::cerr << "Invalid instruction (" << instruction << ") and addressing mode (" << mode << ") pair\n";
        throw std::invalid_argument("Cannot determine opcode");
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
