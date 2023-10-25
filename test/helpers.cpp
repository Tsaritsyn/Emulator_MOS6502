//
// Created by Mikhail on 14/09/2023.
//

//#include <iostream>
//#include <utility>
//#include "helpers.hpp"
//
//ProcessorStatus set_register_flags_for(Byte value) {
//    ProcessorStatus flags{};
//    flags[Flag::ZERO] = value == 0;
//    flags[Flag::NEGATIVE] = (char)value < 0;
//    return flags;
//}
//
//bool page_crossed(Word address, int offset) {
//    const WordToBytes buf(address);
//    const WordToBytes bufNew((Word)(address + offset));
//    return buf.high != bufNew.high;
//}
//
//static std::pair<Byte, bool> add_within_range(int value1, int value2, bool carry, int rmin, int rmax) {
//    int result = value1 + value2 + carry;
//    return {result, result > rmax || result < rmin};
//}
//
//std::pair<Byte, ProcessorStatus> add_with_carry(Byte value1, Byte value2, bool carry) {
//    ProcessorStatus flags{};
//
//    const auto &[unsignedResult, unsignedOverflow] = add_within_range(value1, value2, carry, 0, UINT8_MAX);
//    flags[Flag::CARRY] = unsignedOverflow;
//
//    const auto &[_, signedOverflow] = add_within_range((char)value1, (char)value2, carry, INT8_MIN, INT8_MAX);
//    flags[Flag::OVERFLOW_F] = signedOverflow;
//
//    return {unsignedResult, flags | set_register_flags_for(unsignedResult)};
//}
//
//static std::pair<Byte, bool> subtract_within_range(int value1, int value2, bool carry, int rmin, int rmax) {
//    int result = value1 - value2 - !carry;
//    return {result, result > rmax || result < rmin};
//}
//
//std::pair<Byte, ProcessorStatus> subtract_with_carry(Byte value1, Byte value2, bool carry) {
//    ProcessorStatus flags{};
//
//    const auto &[unsignedResult, unsignedOverflow] = subtract_within_range(value1, value2, carry, 0, UINT8_MAX);
//    flags[Flag::CARRY] = !unsignedOverflow;
//
//    const auto &[_, signedOverflow] = subtract_within_range((char)value1, (char)value2, carry, INT8_MIN, INT8_MAX);
//    flags[Flag::OVERFLOW_F] = signedOverflow;
//
//    return {unsignedResult, flags | set_register_flags_for(unsignedResult)};
//}
//
//std::string to_string(AddressingModeTest mode) {
//    switch (mode) {
//        case AddressingModeTest::IMPLICIT:    return "Implicit";
//        case AddressingModeTest::ACCUMULATOR: return "Accumulator";
//        case AddressingModeTest::IMMEDIATE:   return "Immediate";
//        case AddressingModeTest::ZERO_PAGE:   return "Zero Page";
//        case AddressingModeTest::ZERO_PAGE_X: return "Zero Page,X";
//        case AddressingModeTest::ZERO_PAGE_Y: return "Zero Page,Y";
//        case AddressingModeTest::RELATIVE:    return "Relative";
//        case AddressingModeTest::ABSOLUTE:    return "Absolute";
//        case AddressingModeTest::ABSOLUTE_X:  return "Absolute,X";
//        case AddressingModeTest::ABSOLUTE_Y:  return "Absolute,Y";
//        case AddressingModeTest::INDIRECT:    return "Indirect";
//        case AddressingModeTest::INDIRECT_X:  return "Indirect,X";
//        case AddressingModeTest::INDIRECT_Y:  return "Indirect,Y";
//    }
//    std::unreachable();
//}
//
//
//std::string to_string(Instruction instruction) {
//    switch (instruction) {
//        case Instruction::ADC: return "ADC";
//        case Instruction::AND: return "AND";
//        case Instruction::ASL: return "ASL";
//        case Instruction::BCC: return "BCC";
//        case Instruction::BCS: return "BCS";
//        case Instruction::BEQ: return "BEQ";
//        case Instruction::BIT: return "BIT";
//        case Instruction::BMI: return "BMI";
//        case Instruction::BNE: return "BNE";
//        case Instruction::BPL: return "BPL";
//        case Instruction::BRK: return "BRK";
//        case Instruction::BVC: return "BVC";
//        case Instruction::BVS: return "BVS";
//        case Instruction::CLC: return "CLC";
//        case Instruction::CLD: return "CLD";
//        case Instruction::CLI: return "CLI";
//        case Instruction::CLV: return "CLV";
//        case Instruction::CMP: return "CMP";
//        case Instruction::CPX: return "CPX";
//        case Instruction::CPY: return "CPY";
//        case Instruction::DEC: return "DEC";
//        case Instruction::DEX: return "DEX";
//        case Instruction::DEY: return "DEY";
//        case Instruction::EOR: return "EOR";
//        case Instruction::INC: return "INC";
//        case Instruction::INX: return "INX";
//        case Instruction::INY: return "INY";
//        case Instruction::JMP: return "JMP";
//        case Instruction::JSR: return "JSR";
//        case Instruction::LDA: return "LDA";
//        case Instruction::LDX: return "LDX";
//        case Instruction::LDY: return "LDY";
//        case Instruction::LSR: return "LSR";
//        case Instruction::NOP: return "NOP";
//        case Instruction::ORA: return "ORA";
//        case Instruction::PHA: return "PHA";
//        case Instruction::PHP: return "PHP";
//        case Instruction::PLA: return "PLA";
//        case Instruction::PLP: return "PLP";
//        case Instruction::ROL: return "ROL";
//        case Instruction::ROR: return "ROR";
//        case Instruction::RTI: return "RTI";
//        case Instruction::RTS: return "RTS";
//        case Instruction::SBC: return "SBC";
//        case Instruction::SEC: return "SEC";
//        case Instruction::SED: return "SED";
//        case Instruction::SEI: return "SEI";
//        case Instruction::STA: return "STA";
//        case Instruction::STX: return "STX";
//        case Instruction::STY: return "STY";
//        case Instruction::TAX: return "TAX";
//        case Instruction::TAY: return "TAY";
//        case Instruction::TSX: return "TSX";
//        case Instruction::TXA: return "TXA";
//        case Instruction::TXS: return "TXS";
//        case Instruction::TYA: return "TYA";
//    }
//    std::unreachable();
//}
//
//
//Result <OpCode> opcode(Instruction instruction, std::optional<AddressingModeTest> addressingModeTest) {
//    switch (instruction) {
//        case Instruction::ADC:
//            switch (addressingModeTest.value_or(AddressingModeTest::IMMEDIATE)) {
//                case AddressingModeTest::IMMEDIATE:   return ADC_IMMEDIATE;
//                case AddressingModeTest::ZERO_PAGE:   return ADC_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return ADC_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return ADC_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return ADC_ABSOLUTE_X;
//                case AddressingModeTest::ABSOLUTE_Y:  return ADC_ABSOLUTE_Y;
//                case AddressingModeTest::INDIRECT_X:  return ADC_INDIRECT_X;
//                case AddressingModeTest::INDIRECT_Y:  return ADC_INDIRECT_Y;
//                default: break;
//            }
//
//        case Instruction::AND:
//            switch (addressingModeTest.value_or(AddressingModeTest::IMMEDIATE)) {
//                case AddressingModeTest::IMMEDIATE:   return AND_IMMEDIATE;
//                case AddressingModeTest::ZERO_PAGE:   return AND_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return AND_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return AND_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return AND_ABSOLUTE_X;
//                case AddressingModeTest::ABSOLUTE_Y:  return AND_ABSOLUTE_Y;
//                case AddressingModeTest::INDIRECT_X:  return AND_INDIRECT_X;
//                case AddressingModeTest::INDIRECT_Y:  return AND_INDIRECT_Y;
//                default: break;
//            }
//
//        case Instruction::ASL:
//            switch (addressingModeTest.value_or(AddressingModeTest::ACCUMULATOR)) {
//                case AddressingModeTest::ACCUMULATOR: return ASL_ACCUMULATOR;
//                case AddressingModeTest::ZERO_PAGE:   return ASL_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return ASL_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return ASL_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return ASL_ABSOLUTE_X;
//                default: break;
//            }
//
//        case Instruction::BCC:
//            if (addressingModeTest.value_or(AddressingModeTest::RELATIVE) == AddressingModeTest::RELATIVE) return BCC_RELATIVE;
//            else break;
//
//        case Instruction::BCS:
//            if (addressingModeTest.value_or(AddressingModeTest::RELATIVE) == AddressingModeTest::RELATIVE) return BCS_RELATIVE;
//            else break;
//
//        case Instruction::BEQ:
//            if (addressingModeTest.value_or(AddressingModeTest::RELATIVE) == AddressingModeTest::RELATIVE) return BEQ_RELATIVE;
//            else break;
//
//        case Instruction::BIT:
//            switch (addressingModeTest.value_or(AddressingModeTest::ZERO_PAGE)) {
//                case AddressingModeTest::ZERO_PAGE: return BIT_ZERO_PAGE;
//                case AddressingModeTest::ABSOLUTE:  return BIT_ABSOLUTE;
//                default: break;
//            }
//
//        case Instruction::BMI:
//            if (addressingModeTest.value_or(AddressingModeTest::RELATIVE) == AddressingModeTest::RELATIVE) return BMI_RELATIVE;
//            else break;
//
//        case Instruction::BNE:
//            if (addressingModeTest.value_or(AddressingModeTest::RELATIVE) == AddressingModeTest::RELATIVE) return BNE_RELATIVE;
//            else break;
//
//        case Instruction::BPL:
//            /*if (addressingModeTest.value_or(AddressingModeTest::RELATIVE) == AddressingModeTest::RELATIVE)*/ return BPL_RELATIVE;
//            //else break;
//
//        case Instruction::BRK:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return BRK_IMPLICIT;
//            else break;
//
//        case Instruction::BVC:
//            if (addressingModeTest.value_or(AddressingModeTest::RELATIVE) == AddressingModeTest::RELATIVE) return BVC_RELATIVE;
//            else break;
//
//        case Instruction::BVS:
//            if (addressingModeTest.value_or(AddressingModeTest::RELATIVE) == AddressingModeTest::RELATIVE) return BVS_RELATIVE;
//            else break;
//
//        case Instruction::CLC:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return CLC_IMPLICIT;
//            else break;
//
//        case Instruction::CLD:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return CLD_IMPLICIT;
//            else break;
//
//        case Instruction::CLI:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return CLI_IMPLICIT;
//            else break;
//
//        case Instruction::CLV:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return CLV_IMPLICIT;
//            else break;
//
//        case Instruction::CMP:
//            switch (addressingModeTest.value_or(AddressingModeTest::IMMEDIATE)) {
//                case AddressingModeTest::IMMEDIATE:   return CMP_IMMEDIATE;
//                case AddressingModeTest::ZERO_PAGE:   return CMP_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return CMP_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return CMP_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return CMP_ABSOLUTE_X;
//                case AddressingModeTest::ABSOLUTE_Y:  return CMP_ABSOLUTE_Y;
//                case AddressingModeTest::INDIRECT_X:  return CMP_INDIRECT_X;
//                case AddressingModeTest::INDIRECT_Y:  return CMP_INDIRECT_Y;
//                default: break;
//            }
//
//        case Instruction::CPX:
//            switch (addressingModeTest.value_or(AddressingModeTest::IMMEDIATE)) {
//                case AddressingModeTest::IMMEDIATE: return CPX_IMMEDIATE;
//                case AddressingModeTest::ZERO_PAGE: return CPX_ZERO_PAGE;
//                case AddressingModeTest::ABSOLUTE:  return CPX_ABSOLUTE;
//                default: break;
//            }
//
//        case Instruction::CPY:
//            switch (addressingModeTest.value_or(AddressingModeTest::IMMEDIATE)) {
//                case AddressingModeTest::IMMEDIATE: return CPY_IMMEDIATE;
//                case AddressingModeTest::ZERO_PAGE: return CPY_ZERO_PAGE;
//                case AddressingModeTest::ABSOLUTE:  return CPY_ABSOLUTE;
//                default: break;
//            }
//
//        case Instruction::DEC:
//            switch (addressingModeTest.value_or(AddressingModeTest::ZERO_PAGE)) {
//                case AddressingModeTest::ZERO_PAGE:   return DEC_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return DEC_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return DEC_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return DEC_ABSOLUTE_X;
//                default: break;
//            }
//
//        case Instruction::DEX:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return DEX_IMPLICIT;
//            else break;
//
//        case Instruction::DEY:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return DEY_IMPLICIT;
//            else break;
//
//        case Instruction::EOR:
//            switch (addressingModeTest.value_or(AddressingModeTest::IMMEDIATE)) {
//                case AddressingModeTest::IMMEDIATE:   return EOR_IMMEDIATE;
//                case AddressingModeTest::ZERO_PAGE:   return EOR_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return EOR_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return EOR_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return EOR_ABSOLUTE_X;
//                case AddressingModeTest::ABSOLUTE_Y:  return EOR_ABSOLUTE_Y;
//                case AddressingModeTest::INDIRECT_X:  return EOR_INDIRECT_X;
//                case AddressingModeTest::INDIRECT_Y:  return EOR_INDIRECT_Y;
//                default: break;
//            }
//
//        case Instruction::INC:
//            switch (addressingModeTest.value_or(AddressingModeTest::ZERO_PAGE)) {
//                case AddressingModeTest::ZERO_PAGE:   return INC_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return INC_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return INC_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return INC_ABSOLUTE_X;
//                default: break;
//            }
//
//        case Instruction::INX:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return INX_IMPLICIT;
//            else break;
//
//        case Instruction::INY:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return INY_IMPLICIT;
//            else break;
//
//        case Instruction::JMP:
//            switch (addressingModeTest.value_or(AddressingModeTest::ABSOLUTE)) {
//                case AddressingModeTest::ABSOLUTE: return JMP_ABSOLUTE;
//                case AddressingModeTest::INDIRECT: return JMP_INDIRECT;
//                default: break;
//            }
//
//        case Instruction::JSR:
//            if (addressingModeTest.value_or(AddressingModeTest::ABSOLUTE) == AddressingModeTest::ABSOLUTE) return JSR_ABSOLUTE;
//            else break;
//
//        case Instruction::LDA:
//            switch (addressingModeTest.value_or(AddressingModeTest::IMMEDIATE)) {
//                case AddressingModeTest::IMMEDIATE:   return LDA_IMMEDIATE;
//                case AddressingModeTest::ZERO_PAGE:   return LDA_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return LDA_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return LDA_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return LDA_ABSOLUTE_X;
//                case AddressingModeTest::ABSOLUTE_Y:  return LDA_ABSOLUTE_Y;
//                case AddressingModeTest::INDIRECT_X:  return LDA_INDIRECT_X;
//                case AddressingModeTest::INDIRECT_Y:  return LDA_INDIRECT_Y;
//                default: break;
//            }
//
//        case Instruction::LDX:
//            switch (addressingModeTest.value_or(AddressingModeTest::IMMEDIATE)) {
//                case AddressingModeTest::IMMEDIATE:   return LDX_IMMEDIATE;
//                case AddressingModeTest::ZERO_PAGE:   return LDX_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_Y: return LDX_ZERO_PAGE_Y;
//                case AddressingModeTest::ABSOLUTE:    return LDX_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_Y:  return LDX_ABSOLUTE_Y;
//                default: break;
//            }
//
//        case Instruction::LDY:
//            switch (addressingModeTest.value_or(AddressingModeTest::IMMEDIATE)) {
//                case AddressingModeTest::IMMEDIATE:   return LDY_IMMEDIATE;
//                case AddressingModeTest::ZERO_PAGE:   return LDY_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return LDY_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return LDY_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return LDY_ABSOLUTE_X;
//                default: break;
//            }
//
//        case Instruction::LSR:
//            switch (addressingModeTest.value_or(AddressingModeTest::ACCUMULATOR)) {
//                case AddressingModeTest::ACCUMULATOR: return LSR_ACCUMULATOR;
//                case AddressingModeTest::ZERO_PAGE:   return LSR_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return LSR_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return LSR_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return LSR_ABSOLUTE_X;
//                default: break;
//            }
//
//        case Instruction::NOP:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return NOP_IMPLICIT;
//            else break;
//
//        case Instruction::ORA:
//            switch (addressingModeTest.value_or(AddressingModeTest::IMMEDIATE)) {
//                case AddressingModeTest::IMMEDIATE:   return ORA_IMMEDIATE;
//                case AddressingModeTest::ZERO_PAGE:   return ORA_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return ORA_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return ORA_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return ORA_ABSOLUTE_X;
//                case AddressingModeTest::ABSOLUTE_Y:  return ORA_ABSOLUTE_Y;
//                case AddressingModeTest::INDIRECT_X:  return ORA_INDIRECT_X;
//                case AddressingModeTest::INDIRECT_Y:  return ORA_INDIRECT_Y;
//                default: break;
//            }
//
//        case Instruction::PHA:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return PHA_IMPLICIT;
//            else break;
//
//        case Instruction::PHP:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return PHP_IMPLICIT;
//            else break;
//
//        case Instruction::PLA:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return PLA_IMPLICIT;
//            else break;
//
//        case Instruction::PLP:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return PLP_IMPLICIT;
//            else break;
//
//        case Instruction::ROL:
//            switch (addressingModeTest.value_or(AddressingModeTest::ACCUMULATOR)) {
//                case AddressingModeTest::ACCUMULATOR: return ROL_ACCUMULATOR;
//                case AddressingModeTest::ZERO_PAGE:   return ROL_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return ROL_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return ROL_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return ROL_ABSOLUTE_X;
//                default: break;
//            }
//
//        case Instruction::ROR:
//            switch (addressingModeTest.value_or(AddressingModeTest::ACCUMULATOR)) {
//                case AddressingModeTest::ACCUMULATOR: return ROR_ACCUMULATOR;
//                case AddressingModeTest::ZERO_PAGE:   return ROR_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return ROR_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return ROR_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return ROR_ABSOLUTE_X;
//                default: break;
//            }
//
//        case Instruction::RTI:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return RTI_IMPLICIT;
//            else break;
//
//        case Instruction::RTS:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return RTS_IMPLICIT;
//            else break;
//
//        case Instruction::SBC:
//            switch (addressingModeTest.value_or(AddressingModeTest::IMMEDIATE)) {
//                case AddressingModeTest::IMMEDIATE:   return SBC_IMMEDIATE;
//                case AddressingModeTest::ZERO_PAGE:   return SBC_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return SBC_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return SBC_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return SBC_ABSOLUTE_X;
//                case AddressingModeTest::ABSOLUTE_Y:  return SBC_ABSOLUTE_Y;
//                case AddressingModeTest::INDIRECT_X:  return SBC_INDIRECT_X;
//                case AddressingModeTest::INDIRECT_Y:  return SBC_INDIRECT_Y;
//                default: break;
//            }
//
//        case Instruction::SEC:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return SEC_IMPLICIT;
//            else break;
//
//        case Instruction::SED:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return SED_IMPLICIT;
//            else break;
//
//        case Instruction::SEI:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return SEI_IMPLICIT;
//            else break;
//
//        case Instruction::STA:
//            switch (addressingModeTest.value_or(AddressingModeTest::ZERO_PAGE)) {
//                case AddressingModeTest::ZERO_PAGE:   return STA_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return STA_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return STA_ABSOLUTE;
//                case AddressingModeTest::ABSOLUTE_X:  return STA_ABSOLUTE_X;
//                case AddressingModeTest::ABSOLUTE_Y:  return STA_ABSOLUTE_Y;
//                case AddressingModeTest::INDIRECT_X:  return STA_INDIRECT_X;
//                case AddressingModeTest::INDIRECT_Y:  return STA_INDIRECT_Y;
//                default: break;
//            }
//
//        case Instruction::STX:
//            switch (addressingModeTest.value_or(AddressingModeTest::ZERO_PAGE)) {
//                case AddressingModeTest::ZERO_PAGE:   return STX_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_Y: return STX_ZERO_PAGE_Y;
//                case AddressingModeTest::ABSOLUTE:    return STX_ABSOLUTE;
//                default: break;
//            }
//
//        case Instruction::STY:
//            switch (addressingModeTest.value_or(AddressingModeTest::ZERO_PAGE)) {
//                case AddressingModeTest::ZERO_PAGE:   return STY_ZERO_PAGE;
//                case AddressingModeTest::ZERO_PAGE_X: return STY_ZERO_PAGE_X;
//                case AddressingModeTest::ABSOLUTE:    return STY_ABSOLUTE;
//                default: break;
//            }
//
//        case Instruction::TAX:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return TAX_IMPLICIT;
//            else break;
//
//        case Instruction::TAY:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return TAY_IMPLICIT;
//            else break;
//
//        case Instruction::TSX:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return TSX_IMPLICIT;
//            else break;
//
//        case Instruction::TXA:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return TXA_IMPLICIT;
//            else break;
//
//        case Instruction::TXS:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return TXS_IMPLICIT;
//            else break;
//
//        case Instruction::TYA:
//            if (addressingModeTest.value_or(AddressingModeTest::IMPLICIT) == AddressingModeTest::IMPLICIT) return TYA_IMPLICIT;
//            else break;
//    }
//
//    return {"opcode: instruction " + to_string(instruction) + " does not support " + to_string(addressingModeTest) + " addressing"};
//}
#include "helpers.hpp"

ProcessorStatus status_from_flags(const std::vector<Flag>& flags) {
    ProcessorStatus result(0);
    for (auto flag: flags) result[flag] = SET;
    return result;
}

std::ostream &operator<<(std::ostream &os, Flag flag) {
    return os << to_string(flag);
}

