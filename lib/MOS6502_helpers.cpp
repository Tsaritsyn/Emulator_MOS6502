//
// Created by Mikhail on 30/08/2023.
//

#include <iostream>
#include <format>
#include <utility>
#include <algorithm>

#include "MOS6502_helpers.hpp"

namespace Emulator {

    void set_bit(Byte &byte, int number, bool value) {
        if (value) byte |= 1 << number;
        else byte &= ~(Byte)(1 << number);
    }


    bool get_bit(Byte byte, int number) {
        return byte & (1 << number);
    }


    std::string to_string(Register aRegister) {
        switch (aRegister) {
            case Register::AC: return "A";
            case Register::X:  return "X";
            case Register::Y:  return "Y";
            case Register::SP: return "SP";
            case Register::SR: return "SR";
        }
        std::unreachable();
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

            case DEC_ZERO_PAGE:   [[fallthrough]];
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

            default:
                return std::vformat("0x{:02x}", std::make_format_args(byte));
        }
    }

    int add_with_overflow(int a, int b, bool &overflow, int rmin, int rmax) {
        const auto diff = rmax + 1 - rmin;

        int result = a + b + overflow;
        overflow = (result < rmin) || result > rmax;

        while (result < rmin) result += diff;
        while (result > rmax) result -= diff;
        return result;
    }

    int subtract_with_overflow(int a, int b, bool &overflow, int rmin, int rmax) {
        const auto diff = rmax + 1 - rmin;

        int result = a - b - !overflow;
        overflow = !((result < rmin) || result > rmax);

        while (result < rmin) result += diff;
        while (result > rmax) result -= diff;
        return result;
    }

}
