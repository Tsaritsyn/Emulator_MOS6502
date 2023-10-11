//
// Created by Mikhail on 30/08/2023.
//

#include <iostream>
#include <format>
#include "MOS6502_helpers.hpp"
#include "Result.hpp"

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
            case AddressingMode::IMPLICIT:    return os << "Implicit";
            case AddressingMode::ACCUMULATOR: return os << "Accumulator";
            case AddressingMode::IMMEDIATE:   return os << "Immediate";
            case AddressingMode::ZERO_PAGE:   return os << "Zero Page";
            case AddressingMode::ZERO_PAGE_X: return os << "Zero Page,X";
            case AddressingMode::ZERO_PAGE_Y: return os << "Zero Page,Y";
            case AddressingMode::RELATIVE:    return os << "Relative";
            case AddressingMode::ABSOLUTE:    return os << "Absolute";
            case AddressingMode::ABSOLUTE_X:  return os << "Absolute,X";
            case AddressingMode::ABSOLUTE_Y:  return os << "Absolute,Y";
            case AddressingMode::INDIRECT:    return os << "Indirect";
            case AddressingMode::INDIRECT_X:  return os << "Indirect,X";
            case AddressingMode::INDIRECT_Y:  return os << "Indirect,Y";
        }
        std::unreachable();
    }


    std::ostream &operator<<(std::ostream &os, Register aRegister) {
        switch (aRegister) {
            case Register::AC: return os << 'A';
            case Register::X:  return os << 'X';
            case Register::Y:  return os << 'Y';
            case Register::SP: return os << "SP";
            case Register::SR: return os << "SR";
        }
        std::unreachable();
    }


    std::ostream &operator<<(std::ostream &os, Flag flag) {
        switch (flag) {
            case NEGATIVE:          return os << "Negative";
            case OVERFLOW_F:          return os << "Overflow";
            case BREAK:             return os << "Break";
            case DECIMAL:           return os << "Decimal";
            case INTERRUPT_DISABLE: return os << "Interrupt disable";
            case ZERO:              return os << "Zero";
            case CARRY:             return os << "Carry";
        }
        std::unreachable();
    }


    std::ostream &operator<<(std::ostream &os, Instruction instruction) {
        switch (instruction) {
            case Instruction::ADC: return os << "ADC";
            case Instruction::AND: return os << "AND";
            case Instruction::ASL: return os << "ASL";
            case Instruction::BCC: return os << "BCC";
            case Instruction::BCS: return os << "BCS";
            case Instruction::BEQ: return os << "BEQ";
            case Instruction::BIT: return os << "BIT";
            case Instruction::BMI: return os << "BMI";
            case Instruction::BNE: return os << "BNE";
            case Instruction::BPL: return os << "BPL";
            case Instruction::BRK: return os << "BRK";
            case Instruction::BVC: return os << "BVC";
            case Instruction::BVS: return os << "BVS";
            case Instruction::CLC: return os << "CLC";
            case Instruction::CLD: return os << "CLD";
            case Instruction::CLI: return os << "CLI";
            case Instruction::CLV: return os << "CLV";
            case Instruction::CMP: return os << "CMP";
            case Instruction::CPX: return os << "CPX";
            case Instruction::CPY: return os << "CPY";
            case Instruction::DEC: return os << "DEC";
            case Instruction::DEX: return os << "DEX";
            case Instruction::DEY: return os << "DEY";
            case Instruction::EOR: return os << "EOR";
            case Instruction::INC: return os << "INC";
            case Instruction::INX: return os << "INX";
            case Instruction::INY: return os << "INY";
            case Instruction::JMP: return os << "JMP";
            case Instruction::JSR: return os << "JSR";
            case Instruction::LDA: return os << "LDA";
            case Instruction::LDX: return os << "LDX";
            case Instruction::LDY: return os << "LDY";
            case Instruction::LSR: return os << "LSR";
            case Instruction::NOP: return os << "NOP";
            case Instruction::ORA: return os << "ORA";
            case Instruction::PHA: return os << "PHA";
            case Instruction::PHP: return os << "PHP";
            case Instruction::PLA: return os << "PLA";
            case Instruction::PLP: return os << "PLP";
            case Instruction::ROL: return os << "ROL";
            case Instruction::ROR: return os << "ROR";
            case Instruction::RTI: return os << "RTI";
            case Instruction::RTS: return os << "RTS";
            case Instruction::SBC: return os << "SBC";
            case Instruction::SEC: return os << "SEC";
            case Instruction::SED: return os << "SED";
            case Instruction::SEI: return os << "SEI";
            case Instruction::STA: return os << "STA";
            case Instruction::STX: return os << "STX";
            case Instruction::STY: return os << "STY";
            case Instruction::TAX: return os << "TAX";
            case Instruction::TAY: return os << "TAY";
            case Instruction::TSX: return os << "TSX";
            case Instruction::TXA: return os << "TXA";
            case Instruction::TXS: return os << "TXS";
            case Instruction::TYA: return os << "TYA";
        }
        std::unreachable();
    }

    Result <OpCode> opcode(Instruction instruction, std::optional<AddressingMode> addressingMode) {
        switch (instruction) {
            case Instruction::ADC:
                switch (addressingMode.value_or(AddressingMode::IMMEDIATE)) {
                    case AddressingMode::IMMEDIATE:   return ADC_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:   return ADC_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return ADC_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return ADC_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return ADC_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:  return ADC_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:  return ADC_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:  return ADC_INDIRECT_Y;
                    default: break;
                }

            case Instruction::AND:
                switch (addressingMode.value_or(AddressingMode::IMMEDIATE)) {
                    case AddressingMode::IMMEDIATE:   return AND_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:   return AND_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return AND_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return AND_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return AND_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:  return AND_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:  return AND_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:  return AND_INDIRECT_Y;
                    default: break;
                }

            case Instruction::ASL:
                switch (addressingMode.value_or(AddressingMode::ACCUMULATOR)) {
                    case AddressingMode::ACCUMULATOR: return ASL_ACCUMULATOR;
                    case AddressingMode::ZERO_PAGE:   return ASL_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return ASL_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return ASL_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return ASL_ABSOLUTE_X;
                    default: break;
                }

            case Instruction::BCC:
                if (addressingMode.value_or(AddressingMode::RELATIVE) == AddressingMode::RELATIVE) return BCC_RELATIVE;
                else break;

            case Instruction::BCS:
                if (addressingMode.value_or(AddressingMode::RELATIVE) == AddressingMode::RELATIVE) return BCS_RELATIVE;
                else break;

            case Instruction::BEQ:
                if (addressingMode.value_or(AddressingMode::RELATIVE) == AddressingMode::RELATIVE) return BEQ_RELATIVE;
                else break;

            case Instruction::BIT:
                switch (addressingMode.value_or(AddressingMode::ZERO_PAGE)) {
                    case AddressingMode::ZERO_PAGE: return BIT_ZERO_PAGE;
                    case AddressingMode::ABSOLUTE:  return BIT_ABSOLUTE;
                    default: break;
                }

            case Instruction::BMI:
                if (addressingMode.value_or(AddressingMode::RELATIVE) == AddressingMode::RELATIVE) return BMI_RELATIVE;
                else break;

            case Instruction::BNE:
                if (addressingMode.value_or(AddressingMode::RELATIVE) == AddressingMode::RELATIVE) return BNE_RELATIVE;
                else break;

            case Instruction::BPL:
                if (addressingMode.value_or(AddressingMode::RELATIVE) == AddressingMode::RELATIVE) return BPL_RELATIVE;
                else break;

            case Instruction::BRK:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return BRK_IMPLICIT;
                else break;

            case Instruction::BVC:
                if (addressingMode.value_or(AddressingMode::RELATIVE) == AddressingMode::RELATIVE) return BVC_RELATIVE;
                else break;

            case Instruction::BVS:
                if (addressingMode.value_or(AddressingMode::RELATIVE) == AddressingMode::RELATIVE) return BVS_RELATIVE;
                else break;

            case Instruction::CLC:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return CLC_IMPLICIT;
                else break;

            case Instruction::CLD:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return CLD_IMPLICIT;
                else break;

            case Instruction::CLI:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return CLI_IMPLICIT;
                else break;

            case Instruction::CLV:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return CLV_IMPLICIT;
                else break;

            case Instruction::CMP:
                switch (addressingMode.value_or(AddressingMode::IMMEDIATE)) {
                    case AddressingMode::IMMEDIATE:   return CMP_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:   return CMP_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return CMP_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return CMP_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return CMP_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:  return CMP_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:  return CMP_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:  return CMP_INDIRECT_Y;
                    default: break;
                }

            case Instruction::CPX:
                switch (addressingMode.value_or(AddressingMode::IMMEDIATE)) {
                    case AddressingMode::IMMEDIATE: return CPX_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE: return CPX_ZERO_PAGE;
                    case AddressingMode::ABSOLUTE:  return CPX_ABSOLUTE;
                    default: break;
                }

            case Instruction::CPY:
                switch (addressingMode.value_or(AddressingMode::IMMEDIATE)) {
                    case AddressingMode::IMMEDIATE: return CPY_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE: return CPY_ZERO_PAGE;
                    case AddressingMode::ABSOLUTE:  return CPY_ABSOLUTE;
                    default: break;
                }

            case Instruction::DEC:
                switch (addressingMode.value_or(AddressingMode::ZERO_PAGE)) {
                    case AddressingMode::ZERO_PAGE:   return DEC_ZER0_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return DEC_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return DEC_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return DEC_ABSOLUTE_X;
                    default: break;
                }

            case Instruction::DEX:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return DEX_IMPLICIT;
                else break;

            case Instruction::DEY:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return DEY_IMPLICIT;
                else break;

            case Instruction::EOR:
                switch (addressingMode.value_or(AddressingMode::IMMEDIATE)) {
                    case AddressingMode::IMMEDIATE:   return EOR_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:   return EOR_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return EOR_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return EOR_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return EOR_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:  return EOR_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:  return EOR_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:  return EOR_INDIRECT_Y;
                    default: break;
                }

            case Instruction::INC:
                switch (addressingMode.value_or(AddressingMode::ZERO_PAGE)) {
                    case AddressingMode::ZERO_PAGE:   return INC_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return INC_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return INC_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return INC_ABSOLUTE_X;
                    default: break;
                }

            case Instruction::INX:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return INX_IMPLICIT;
                else break;

            case Instruction::INY:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return INY_IMPLICIT;
                else break;

            case Instruction::JMP:
                switch (addressingMode.value_or(AddressingMode::ABSOLUTE)) {
                    case AddressingMode::ABSOLUTE: return JMP_ABSOLUTE;
                    case AddressingMode::INDIRECT: return JMP_INDIRECT;
                    default: break;
                }

            case Instruction::JSR:
                if (addressingMode.value_or(AddressingMode::ABSOLUTE) == AddressingMode::ABSOLUTE) return JSR_ABSOLUTE;
                else break;

            case Instruction::LDA:
                switch (addressingMode.value_or(AddressingMode::IMMEDIATE)) {
                    case AddressingMode::IMMEDIATE:   return LDA_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:   return LDA_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return LDA_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return LDA_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return LDA_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:  return LDA_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:  return LDA_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:  return LDA_INDIRECT_Y;
                    default: break;
                }

            case Instruction::LDX:
                switch (addressingMode.value_or(AddressingMode::IMMEDIATE)) {
                    case AddressingMode::IMMEDIATE:   return LDX_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:   return LDX_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_Y: return LDX_ZERO_PAGE_Y;
                    case AddressingMode::ABSOLUTE:    return LDX_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_Y:  return LDX_ABSOLUTE_Y;
                    default: break;
                }

            case Instruction::LDY:
                switch (addressingMode.value_or(AddressingMode::IMMEDIATE)) {
                    case AddressingMode::IMMEDIATE:   return LDY_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:   return LDY_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return LDY_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return LDY_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return LDY_ABSOLUTE_X;
                    default: break;
                }

            case Instruction::LSR:
                switch (addressingMode.value_or(AddressingMode::ACCUMULATOR)) {
                    case AddressingMode::ACCUMULATOR: return LSR_ACCUMULATOR;
                    case AddressingMode::ZERO_PAGE:   return LSR_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return LSR_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return LSR_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return LSR_ABSOLUTE_X;
                    default: break;
                }

            case Instruction::NOP:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return NOP_IMPLICIT;
                else break;

            case Instruction::ORA:
                switch (addressingMode.value_or(AddressingMode::IMMEDIATE)) {
                    case AddressingMode::IMMEDIATE:   return ORA_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:   return ORA_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return ORA_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return ORA_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return ORA_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:  return ORA_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:  return ORA_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:  return ORA_INDIRECT_Y;
                    default: break;
                }

            case Instruction::PHA:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return PHA_IMPLICIT;
                else break;

            case Instruction::PHP:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return PHP_IMPLICIT;
                else break;

            case Instruction::PLA:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return PLA_IMPLICIT;
                else break;

            case Instruction::PLP:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return PLP_IMPLICIT;
                else break;

            case Instruction::ROL:
                switch (addressingMode.value_or(AddressingMode::ACCUMULATOR)) {
                    case AddressingMode::ACCUMULATOR: return ROL_ACCUMULATOR;
                    case AddressingMode::ZERO_PAGE:   return ROL_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return ROL_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return ROL_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return ROL_ABSOLUTE_X;
                    default: break;
                }

            case Instruction::ROR:
                switch (addressingMode.value_or(AddressingMode::ACCUMULATOR)) {
                    case AddressingMode::ACCUMULATOR: return ROR_ACCUMULATOR;
                    case AddressingMode::ZERO_PAGE:   return ROR_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return ROR_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return ROR_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return ROR_ABSOLUTE_X;
                    default: break;
                }

            case Instruction::RTI:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return RTI_IMPLICIT;
                else break;

            case Instruction::RTS:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return RTS_IMPLICIT;
                else break;

            case Instruction::SBC:
                switch (addressingMode.value_or(AddressingMode::IMMEDIATE)) {
                    case AddressingMode::IMMEDIATE:   return SBC_IMMEDIATE;
                    case AddressingMode::ZERO_PAGE:   return SBC_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return SBC_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return SBC_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return SBC_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:  return SBC_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:  return SBC_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:  return SBC_INDIRECT_Y;
                    default: break;
                }

            case Instruction::SEC:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return SEC_IMPLICIT;
                else break;

            case Instruction::SED:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return SED_IMPLICIT;
                else break;

            case Instruction::SEI:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return SEI_IMPLICIT;
                else break;

            case Instruction::STA:
                switch (addressingMode.value_or(AddressingMode::ZERO_PAGE)) {
                    case AddressingMode::ZERO_PAGE:   return STA_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return STA_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return STA_ABSOLUTE;
                    case AddressingMode::ABSOLUTE_X:  return STA_ABSOLUTE_X;
                    case AddressingMode::ABSOLUTE_Y:  return STA_ABSOLUTE_Y;
                    case AddressingMode::INDIRECT_X:  return STA_INDIRECT_X;
                    case AddressingMode::INDIRECT_Y:  return STA_INDIRECT_Y;
                    default: break;
                }

            case Instruction::STX:
                switch (addressingMode.value_or(AddressingMode::ZERO_PAGE)) {
                    case AddressingMode::ZERO_PAGE:   return STX_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_Y: return STX_ZERO_PAGE_Y;
                    case AddressingMode::ABSOLUTE:    return STX_ABSOLUTE;
                    default: break;
                }

            case Instruction::STY:
                switch (addressingMode.value_or(AddressingMode::ZERO_PAGE)) {
                    case AddressingMode::ZERO_PAGE:   return STY_ZERO_PAGE;
                    case AddressingMode::ZERO_PAGE_X: return STY_ZERO_PAGE_X;
                    case AddressingMode::ABSOLUTE:    return STY_ABSOLUTE;
                    default: break;
                }

            case Instruction::TAX:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return TAX_IMPLICIT;
                else break;

            case Instruction::TAY:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return TAY_IMPLICIT;
                else break;

            case Instruction::TSX:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return TSX_IMPLICIT;
                else break;

            case Instruction::TXA:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return TXA_IMPLICIT;
                else break;

            case Instruction::TXS:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return TXS_IMPLICIT;
                else break;

            case Instruction::TYA:
                if (addressingMode.value_or(AddressingMode::IMPLICIT) == AddressingMode::IMPLICIT) return TYA_IMPLICIT;
                else break;
        }

        std::stringstream message;
        message << "opcode: instruction " << instruction << " does not support " << addressingMode << " addressing\n";
        return {message.str()};
    }


    std::string byte_description(Byte byte) {
        switch (byte) {
            case ADC_IMMEDIATE:   [[fallthrough]];
            case ADC_ZERO_PAGE:   [[fallthrough]];
            case ADC_ZERO_PAGE_X: [[fallthrough]];
            case ADC_ABSOLUTE:    [[fallthrough]];
            case ADC_ABSOLUTE_X:  [[fallthrough]];
            case ADC_ABSOLUTE_Y:  [[fallthrough]];
            case ADC_INDIRECT_X:  [[fallthrough]];
            case ADC_INDIRECT_Y:  return "ADC";

            case AND_IMMEDIATE:   [[fallthrough]];
            case AND_ZERO_PAGE:   [[fallthrough]];
            case AND_ZERO_PAGE_X: [[fallthrough]];
            case AND_ABSOLUTE:    [[fallthrough]];
            case AND_ABSOLUTE_X:  [[fallthrough]];
            case AND_ABSOLUTE_Y:  [[fallthrough]];
            case AND_INDIRECT_X:  [[fallthrough]];
            case AND_INDIRECT_Y:  return "AND";

            case ASL_ACCUMULATOR: [[fallthrough]];
            case ASL_ZERO_PAGE:   [[fallthrough]];
            case ASL_ZERO_PAGE_X: [[fallthrough]];
            case ASL_ABSOLUTE:    [[fallthrough]];
            case ASL_ABSOLUTE_X:  return "ASL";

            case BCC_RELATIVE:    return "BCC";
            case BCS_RELATIVE:    return "BCS";
            case BEQ_RELATIVE:    return "BEQ";
            case BMI_RELATIVE:    return "BMI";
            case BNE_RELATIVE:    return "BNE";
            case BPL_RELATIVE:    return "BPL";
            case BVC_RELATIVE:    return "BVC";
            case BVS_RELATIVE:    return "BVS";

            case BIT_ZERO_PAGE:   [[fallthrough]];
            case BIT_ABSOLUTE:    return "BIT";

            case BRK_IMPLICIT:    return "BRK";

            case CLC_IMPLICIT:    return "CLC";
            case CLD_IMPLICIT:    return "CLD";
            case CLI_IMPLICIT:    return "CLI";
            case CLV_IMPLICIT:    return "CLV";

            case CMP_IMMEDIATE:   [[fallthrough]];
            case CMP_ZERO_PAGE:   [[fallthrough]];
            case CMP_ZERO_PAGE_X: [[fallthrough]];
            case CMP_ABSOLUTE:    [[fallthrough]];
            case CMP_ABSOLUTE_X:  [[fallthrough]];
            case CMP_ABSOLUTE_Y:  [[fallthrough]];
            case CMP_INDIRECT_X:  [[fallthrough]];
            case CMP_INDIRECT_Y:  return "CMP";

            case CPX_IMMEDIATE:   [[fallthrough]];
            case CPX_ZERO_PAGE:   [[fallthrough]];
            case CPX_ABSOLUTE:    return "CPX";

            case CPY_IMMEDIATE:   [[fallthrough]];
            case CPY_ZERO_PAGE:   [[fallthrough]];
            case CPY_ABSOLUTE:    return "CPY";

            case DEC_ZER0_PAGE:   [[fallthrough]];
            case DEC_ZERO_PAGE_X: [[fallthrough]];
            case DEC_ABSOLUTE:    [[fallthrough]];
            case DEC_ABSOLUTE_X:  return "DEC";

            case DEX_IMPLICIT:    return "DEX";
            case DEY_IMPLICIT:    return "DEY";

            case EOR_IMMEDIATE:   [[fallthrough]];
            case EOR_ZERO_PAGE:   [[fallthrough]];
            case EOR_ZERO_PAGE_X: [[fallthrough]];
            case EOR_ABSOLUTE:    [[fallthrough]];
            case EOR_ABSOLUTE_X:  [[fallthrough]];
            case EOR_ABSOLUTE_Y:  [[fallthrough]];
            case EOR_INDIRECT_X:  [[fallthrough]];
            case EOR_INDIRECT_Y:  return "EOR";

            case INC_ZERO_PAGE:   [[fallthrough]];
            case INC_ZERO_PAGE_X: [[fallthrough]];
            case INC_ABSOLUTE:    [[fallthrough]];
            case INC_ABSOLUTE_X:  return "INC";

            case INX_IMPLICIT:    return "INX";
            case INY_IMPLICIT:    return "INY";

            case JMP_ABSOLUTE:    [[fallthrough]];
            case JMP_INDIRECT:    return "JMP";
            case JSR_ABSOLUTE:    return "JSR";

            case LDA_IMMEDIATE:   [[fallthrough]];
            case LDA_ZERO_PAGE:   [[fallthrough]];
            case LDA_ZERO_PAGE_X: [[fallthrough]];
            case LDA_ABSOLUTE:    [[fallthrough]];
            case LDA_ABSOLUTE_X:  [[fallthrough]];
            case LDA_ABSOLUTE_Y:  [[fallthrough]];
            case LDA_INDIRECT_X:  [[fallthrough]];
            case LDA_INDIRECT_Y:  return "LDA";

            case LDX_IMMEDIATE:   [[fallthrough]];
            case LDX_ZERO_PAGE:   [[fallthrough]];
            case LDX_ZERO_PAGE_Y: [[fallthrough]];
            case LDX_ABSOLUTE:    [[fallthrough]];
            case LDX_ABSOLUTE_Y:  return "LDX";

            case LDY_IMMEDIATE:   [[fallthrough]];
            case LDY_ZERO_PAGE:   [[fallthrough]];
            case LDY_ZERO_PAGE_X: [[fallthrough]];
            case LDY_ABSOLUTE:    [[fallthrough]];
            case LDY_ABSOLUTE_X:  return "LDY";

            case LSR_ACCUMULATOR: [[fallthrough]];
            case LSR_ZERO_PAGE:   [[fallthrough]];
            case LSR_ZERO_PAGE_X: [[fallthrough]];
            case LSR_ABSOLUTE:    [[fallthrough]];
            case LSR_ABSOLUTE_X:  return "LSR";

            case NOP_IMPLICIT:    return "NOP";

            case ORA_IMMEDIATE:   [[fallthrough]];
            case ORA_ZERO_PAGE:   [[fallthrough]];
            case ORA_ZERO_PAGE_X: [[fallthrough]];
            case ORA_ABSOLUTE:    [[fallthrough]];
            case ORA_ABSOLUTE_X:  [[fallthrough]];
            case ORA_ABSOLUTE_Y:  [[fallthrough]];
            case ORA_INDIRECT_X:  [[fallthrough]];
            case ORA_INDIRECT_Y:  return "ORA";

            case PHA_IMPLICIT:    return "PHA";
            case PHP_IMPLICIT:    return "PHP";

            case PLA_IMPLICIT:    return "PLA";
            case PLP_IMPLICIT:    return "PLP";

            case ROL_ACCUMULATOR: [[fallthrough]];
            case ROL_ZERO_PAGE:   [[fallthrough]];
            case ROL_ZERO_PAGE_X: [[fallthrough]];
            case ROL_ABSOLUTE:    [[fallthrough]];
            case ROL_ABSOLUTE_X:  return "ROL";

            case ROR_ACCUMULATOR: [[fallthrough]];
            case ROR_ZERO_PAGE:   [[fallthrough]];
            case ROR_ZERO_PAGE_X: [[fallthrough]];
            case ROR_ABSOLUTE:    [[fallthrough]];
            case ROR_ABSOLUTE_X:  return "ROR";

            case RTI_IMPLICIT:    return "RTI";
            case RTS_IMPLICIT:    return "RTS";

            case SBC_IMMEDIATE:   [[fallthrough]];
            case SBC_ZERO_PAGE:   [[fallthrough]];
            case SBC_ZERO_PAGE_X: [[fallthrough]];
            case SBC_ABSOLUTE:    [[fallthrough]];
            case SBC_ABSOLUTE_X:  [[fallthrough]];
            case SBC_ABSOLUTE_Y:  [[fallthrough]];
            case SBC_INDIRECT_X:  [[fallthrough]];
            case SBC_INDIRECT_Y:  return "SBC";

            case SEC_IMPLICIT:    return "SEC";
            case SED_IMPLICIT:    return "SED";
            case SEI_IMPLICIT:    return "SEI";

            case STA_ZERO_PAGE:   [[fallthrough]];
            case STA_ZERO_PAGE_X: [[fallthrough]];
            case STA_ABSOLUTE:    [[fallthrough]];
            case STA_ABSOLUTE_X:  [[fallthrough]];
            case STA_ABSOLUTE_Y:  [[fallthrough]];
            case STA_INDIRECT_X:  [[fallthrough]];
            case STA_INDIRECT_Y:  return "STA";

            case STX_ZERO_PAGE:   [[fallthrough]];
            case STX_ZERO_PAGE_Y: [[fallthrough]];
            case STX_ABSOLUTE:    return "STX";

            case STY_ZERO_PAGE:   [[fallthrough]];
            case STY_ZERO_PAGE_X: [[fallthrough]];
            case STY_ABSOLUTE:    return "STY";

            case TAX_IMPLICIT:    return "TAX";
            case TAY_IMPLICIT:    return "TAY";
            case TSX_IMPLICIT:    return "TSX";
            case TXA_IMPLICIT:    return "TXA";
            case TXS_IMPLICIT:    return "TXS";
            case TYA_IMPLICIT:    return "TYA";
        }

        return std::vformat("0x{:02x}", std::make_format_args(byte));
    }

}
