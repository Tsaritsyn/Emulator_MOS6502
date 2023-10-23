//
// Created by Mikhail on 23/10/2023.
//

#include <format>
#include "helpers.hpp"


std::string byte_description(Byte byte) {
    switch (byte) {
        case ADC_IMMEDIATE:   return "ADC Immediate";
        case ADC_ZERO_PAGE:   return "ADC Zero Page";
        case ADC_ZERO_PAGE_X: return "ADC Zero Page,X";
        case ADC_ABSOLUTE:    return "ADC Absolute";
        case ADC_ABSOLUTE_X:  return "ADC Absolute,X";
        case ADC_ABSOLUTE_Y:  return "ADC Absolute,Y";
        case ADC_INDIRECT_X:  return "ADC Indirect,X";
        case ADC_INDIRECT_Y:  return "ADC Indirect,Y";

        case AND_IMMEDIATE:   return "AND Immediate";
        case AND_ZERO_PAGE:   return "AND Zero Page";
        case AND_ZERO_PAGE_X: return "AND Zero Page,X";
        case AND_ABSOLUTE:    return "AND Absolute";
        case AND_ABSOLUTE_X:  return "AND Absolute,X";
        case AND_ABSOLUTE_Y:  return "AND Absolute,Y";
        case AND_INDIRECT_X:  return "AND Indirect,X";
        case AND_INDIRECT_Y:  return "AND Indirect,Y";

        case ASL_ACCUMULATOR: return "ASL Accumulator";
        case ASL_ZERO_PAGE:   return "ASL Zero Page";
        case ASL_ZERO_PAGE_X: return "ASL Zero Page,X";
        case ASL_ABSOLUTE:    return "ASL Absolute";
        case ASL_ABSOLUTE_X:  return "ASL Absolute,X";

        case BCC_RELATIVE:    return "BCC";
        case BCS_RELATIVE:    return "BCS";
        case BEQ_RELATIVE:    return "BEQ";
        case BMI_RELATIVE:    return "BMI";
        case BNE_RELATIVE:    return "BNE";
        case BPL_RELATIVE:    return "BPL";
        case BVC_RELATIVE:    return "BVC";
        case BVS_RELATIVE:    return "BVS";

        case BIT_ZERO_PAGE:   return "BIT Zero Page";
        case BIT_ABSOLUTE:    return "BIT Absolute";

        case BRK_IMPLICIT:    return "BRK";

        case CLC_IMPLICIT:    return "CLC";
        case CLD_IMPLICIT:    return "CLD";
        case CLI_IMPLICIT:    return "CLI";
        case CLV_IMPLICIT:    return "CLV";

        case CMP_IMMEDIATE:   return "CMP Immediate";
        case CMP_ZERO_PAGE:   return "CMP Zero Page";
        case CMP_ZERO_PAGE_X: return "CMP Zero Page,X";
        case CMP_ABSOLUTE:    return "CMP Absolute";
        case CMP_ABSOLUTE_X:  return "CMP Absolute,X";
        case CMP_ABSOLUTE_Y:  return "CMP Absolute,Y";
        case CMP_INDIRECT_X:  return "CMP Indirect,X";
        case CMP_INDIRECT_Y:  return "CMP Indirect,Y";

        case CPX_IMMEDIATE:   return "CPX Immediate";
        case CPX_ZERO_PAGE:   return "CPX Zero Page";
        case CPX_ABSOLUTE:    return "CPX Absolute";

        case CPY_IMMEDIATE:   return "CPY Immediate";
        case CPY_ZERO_PAGE:   return "CPY Zero Page";
        case CPY_ABSOLUTE:    return "CPY Absolute";

        case DEC_ZERO_PAGE:   return "DEC Zero Page";
        case DEC_ZERO_PAGE_X: return "DEC Zero Page,X";
        case DEC_ABSOLUTE:    return "DEC Absolute";
        case DEC_ABSOLUTE_X:  return "DEC Absolute,X";

        case DEX_IMPLICIT:    return "DEX";
        case DEY_IMPLICIT:    return "DEY";

        case EOR_IMMEDIATE:   return "EOR Immediate";
        case EOR_ZERO_PAGE:   return "EOR Zero Page";
        case EOR_ZERO_PAGE_X: return "EOR Zero Page,X";
        case EOR_ABSOLUTE:    return "EOR Absolute";
        case EOR_ABSOLUTE_X:  return "EOR Absolute,X";
        case EOR_ABSOLUTE_Y:  return "EOR Absolute,Y";
        case EOR_INDIRECT_X:  return "EOR Indirect,X";
        case EOR_INDIRECT_Y:  return "EOR Indirect,Y";

        case INC_ZERO_PAGE:   return "INC Zero Page";
        case INC_ZERO_PAGE_X: return "INC Zero Page,X";
        case INC_ABSOLUTE:    return "INC Absolute";
        case INC_ABSOLUTE_X:  return "INC Absolute,X";

        case INX_IMPLICIT:    return "INX";
        case INY_IMPLICIT:    return "INY";

        case JMP_ABSOLUTE:    return "JMP Absolute";
        case JMP_INDIRECT:    return "JMP Indirect";
        case JSR_ABSOLUTE:    return "JSR";

        case LDA_IMMEDIATE:   return "LDA Immediate";
        case LDA_ZERO_PAGE:   return "LDA Zero Page";
        case LDA_ZERO_PAGE_X: return "LDA Zero Page,X";
        case LDA_ABSOLUTE:    return "LDA Absolute";
        case LDA_ABSOLUTE_X:  return "LDA Absolute,X";
        case LDA_ABSOLUTE_Y:  return "LDA Absolute,Y";
        case LDA_INDIRECT_X:  return "LDA Indirect,X";
        case LDA_INDIRECT_Y:  return "LDA Indirect,Y";

        case LDX_IMMEDIATE:   return "LDX Immediate";
        case LDX_ZERO_PAGE:   return "LDX Zero Page";
        case LDX_ZERO_PAGE_Y: return "LDX_ZERO_PAGE_Y";
        case LDX_ABSOLUTE:    return "LDX Absolute";
        case LDX_ABSOLUTE_Y:  return "LDX Absolute,Y";

        case LDY_IMMEDIATE:   return "LDY Immediate";
        case LDY_ZERO_PAGE:   return "LDY Zero Page";
        case LDY_ZERO_PAGE_X: return "LDY Zero Page,X";
        case LDY_ABSOLUTE:    return "LDY Absolute";
        case LDY_ABSOLUTE_X:  return "LDY Absolute,X";

        case LSR_ACCUMULATOR: return "LSR Accumulator";
        case LSR_ZERO_PAGE:   return "LSR_ZERO_PAGE";
        case LSR_ZERO_PAGE_X: return "LSR Zero Page,X";
        case LSR_ABSOLUTE:    return "LSR Absolute";
        case LSR_ABSOLUTE_X:  return "LSR Absolute,X";

        case NOP_IMPLICIT:    return "NOP";

        case ORA_IMMEDIATE:   return "ORA Immediate";
        case ORA_ZERO_PAGE:   return "ORA Zero Page";
        case ORA_ZERO_PAGE_X: return "ORA Zero Page,X";
        case ORA_ABSOLUTE:    return "ORA Absolute";
        case ORA_ABSOLUTE_X:  return "ORA Absolute,X";
        case ORA_ABSOLUTE_Y:  return "ORA Absolute,Y";
        case ORA_INDIRECT_X:  return "ORA Indirect,X";
        case ORA_INDIRECT_Y:  return "ORA Indirect,Y";

        case PHA_IMPLICIT:    return "PHA";
        case PHP_IMPLICIT:    return "PHP";

        case PLA_IMPLICIT:    return "PLA";
        case PLP_IMPLICIT:    return "PLP";

        case ROL_ACCUMULATOR: return "ROL Accumulator";
        case ROL_ZERO_PAGE:   return "ROL Zero Page";
        case ROL_ZERO_PAGE_X: return "ROL Zero Page,X";
        case ROL_ABSOLUTE:    return "ROL Absolute";
        case ROL_ABSOLUTE_X:  return "ROL Absolute,X";

        case ROR_ACCUMULATOR: return "ROR Accumulator";
        case ROR_ZERO_PAGE:   return "ROR Zero Page";
        case ROR_ZERO_PAGE_X: return "ROR Zero Page,X";
        case ROR_ABSOLUTE:    return "ROR Absolute";
        case ROR_ABSOLUTE_X:  return "ROR Absolute,X";

        case RTI_IMPLICIT:    return "RTI";
        case RTS_IMPLICIT:    return "RTS";

        case SBC_IMMEDIATE:   return "SBC Immediate";
        case SBC_ZERO_PAGE:   return "SBC Zero Page";
        case SBC_ZERO_PAGE_X: return "SBC Zero Page,X";
        case SBC_ABSOLUTE:    return "SBC Absolute";
        case SBC_ABSOLUTE_X:  return "SBC Absolute,X";
        case SBC_ABSOLUTE_Y:  return "SBC Absolute,Y";
        case SBC_INDIRECT_X:  return "SBC Indirect,X";
        case SBC_INDIRECT_Y:  return "SBC Indirect,Y";

        case SEC_IMPLICIT:    return "SEC";
        case SED_IMPLICIT:    return "SED";
        case SEI_IMPLICIT:    return "SEI";

        case STA_ZERO_PAGE:   return "STA Zero Page";
        case STA_ZERO_PAGE_X: return "STA Zero Page,X";
        case STA_ABSOLUTE:    return "STA Absolute";
        case STA_ABSOLUTE_X:  return "STA Absolute,X";
        case STA_ABSOLUTE_Y:  return "STA Absolute,Y";
        case STA_INDIRECT_X:  return "STA Indirect,X";
        case STA_INDIRECT_Y:  return "STA Indirect,Y";

        case STX_ZERO_PAGE:   return "STX Zero Page";
        case STX_ZERO_PAGE_Y: return "STX Zero Page,X";
        case STX_ABSOLUTE:    return "STX Absolute";

        case STY_ZERO_PAGE:   return "STY Zero Page";
        case STY_ZERO_PAGE_X: return "STY Zero Page,X";
        case STY_ABSOLUTE:    return "STY Absolute";

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
