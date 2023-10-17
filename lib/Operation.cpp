//
// Created by Mikhail on 14/10/2023.
//

#include <format>
#include "Operation.hpp"

using namespace Emulator;

constexpr static inline std::string accumulator() { return "A"; }
static inline std::string immediate(Byte value)   { return std::vformat("#{:02u}", std::make_format_args(value)); }
static inline std::string zeroPage(Byte address)  { return std::vformat("${:02x}", std::make_format_args(address)); }
static inline std::string zeroPageX(Byte address) { return std::vformat("${:02x},X", std::make_format_args(address)); }
static inline std::string zeroPageY(Byte address) { return std::vformat("${:02x},Y", std::make_format_args(address)); }
static inline std::string relative(char offset)   { return std::vformat("*{:+}", std::make_format_args(offset)); }
static inline std::string absolute(Word address)  { return std::vformat("${:04x}", std::make_format_args(address)); }
static inline std::string absoluteX(Word address) { return std::vformat("${:04x},X", std::make_format_args(address)); }
static inline std::string absoluteY(Word address) { return std::vformat("${:04x},Y", std::make_format_args(address)); }
static inline std::string indirect(Word address)  { return std::vformat("(${:04x})", std::make_format_args(address)); }
static inline std::string indirectX(Word address) { return std::vformat("(${:04x},X)", std::make_format_args(address)); }
static inline std::string indirectY(Word address) { return std::vformat("(${:04x}),Y", std::make_format_args(address)); }


std::string Emulator::description(const Operation &operation) {
    return std::visit(Overload {
            [](ADC_Immediate op)   { return "ADC " + immediate(op.value); },
            [](ADC_ZeroPage op)    { return "ADC " + zeroPage(op.address); },
            [](ADC_ZeroPageX op)   { return "ADC " + zeroPageX(op.address); },
            [](ADC_Absolute op)    { return "ADC " + absolute(op.address); },
            [](ADC_AbsoluteX op)   { return "ADC " + absoluteX(op.address); },
            [](ADC_AbsoluteY op)   { return "ADC " + absoluteY(op.address); },
            [](ADC_IndirectX op)   { return "ADC " + indirectX(op.address); },
            [](ADC_IndirectY op)   { return "ADC " + indirectY(op.address); },

            [](AND_Immediate op)   { return "AND " + immediate(op.value); },
            [](AND_ZeroPage op)    { return "AND " + zeroPage(op.address); },
            [](AND_ZeroPageX op)   { return "AND " + zeroPageX(op.address); },
            [](AND_Absolute op)    { return "AND " + absolute(op.address); },
            [](AND_AbsoluteX op)   { return "AND " + absoluteX(op.address); },
            [](AND_AbsoluteY op)   { return "AND " + absoluteY(op.address); },
            [](AND_IndirectX op)   { return "AND " + indirectX(op.address); },
            [](AND_IndirectY op)   { return "AND " + indirectY(op.address); },

            [](ASL_Accumulator op) { return "ASL " + accumulator(); },
            [](ASL_ZeroPage op)    { return "ASL " + zeroPage(op.address); },
            [](ASL_ZeroPageX op)   { return "ASL " + zeroPageX(op.address); },
            [](ASL_Absolute op)    { return "ASL " + absolute(op.address); },
            [](ASL_AbsoluteX op)   { return "ASL " + absoluteX(op.address); },

            [](BCC op)             { return "BCC " + relative(op.offset); },
            [](BCS op)             { return "BCS " + relative(op.offset); },
            [](BEQ op)             { return "BEQ " + relative(op.offset); },
            [](BNE op)             { return "BNE " + relative(op.offset); },
            [](BMI op)             { return "BMI " + relative(op.offset); },
            [](BPL op)             { return "BPL " + relative(op.offset); },
            [](BVC op)             { return "BVC " + relative(op.offset); },
            [](BVS op)             { return "BVS " + relative(op.offset); },

            [](BIT_ZeroPage op)    { return "BIT " + zeroPage(op.address); },
            [](BIT_Absolute op)    { return "BIT " + absolute(op.address); },

            [](BRK op)             { return std::string("BRK"); },

            [](CLC op)             { return std::string("CLC"); },
            [](CLD op)             { return std::string("CLD"); },
            [](CLI op)             { return std::string("CLI"); },
            [](CLV op)             { return std::string("CLV"); },

            [](CMP_Immediate op)   { return "CMP " + immediate(op.value); },
            [](CMP_ZeroPage op)    { return "CMP " + zeroPage(op.address); },
            [](CMP_ZeroPageX op)   { return "CMP " + zeroPageX(op.address); },
            [](CMP_Absolute op)    { return "CMP " + absolute(op.address); },
            [](CMP_AbsoluteX op)   { return "CMP " + absoluteX(op.address); },
            [](CMP_AbsoluteY op)   { return "CMP " + absoluteY(op.address); },
            [](CMP_IndirectX op)   { return "CMP " + indirectX(op.address); },
            [](CMP_IndirectY op)   { return "CMP " + indirectY(op.address); },

            [](CPX_Immediate op)   { return "CPX " + immediate(op.value); },
            [](CPX_ZeroPage op)    { return "CPX " + zeroPage(op.address); },
            [](CPX_Absolute op)    { return "CPX " + absolute(op.address); },

            [](CPY_Immediate op)   { return "CPY " + immediate(op.value); },
            [](CPY_ZeroPage op)    { return "CPY " + zeroPage(op.address); },
            [](CPY_Absolute op)    { return "CPY " + absolute(op.address); },

            [](DEC_ZeroPage op)    { return "DEC " + zeroPage(op.address); },
            [](DEC_ZeroPageX op)   { return "DEC " + zeroPageX(op.address); },
            [](DEC_Absolute op)    { return "DEC " + absolute(op.address); },
            [](DEC_AbsoluteX op)   { return "DEC " + absoluteX(op.address); },

            [](DEX op)             { return std::string("DEX"); },
            [](DEY op)             { return std::string("DEY"); },

            [](EOR_Immediate op)   { return "EOR " + immediate(op.value); },
            [](EOR_ZeroPage op)    { return "EOR " + zeroPage(op.address); },
            [](EOR_ZeroPageX op)   { return "EOR " + zeroPageX(op.address); },
            [](EOR_Absolute op)    { return "EOR " + absolute(op.address); },
            [](EOR_AbsoluteX op)   { return "EOR " + absoluteX(op.address); },
            [](EOR_AbsoluteY op)   { return "EOR " + absoluteY(op.address); },
            [](EOR_IndirectX op)   { return "EOR " + indirectX(op.address); },
            [](EOR_IndirectY op)   { return "EOR " + indirectY(op.address); },

            [](INC_ZeroPage op)    { return "INC " + zeroPage(op.address); },
            [](INC_ZeroPageX op)   { return "INC " + zeroPageX(op.address); },
            [](INC_Absolute op)    { return "INC " + absolute(op.address); },
            [](INC_AbsoluteX op)   { return "INC " + absoluteX(op.address); },

            [](INX op)             { return std::string("INX"); },
            [](INY op)             { return std::string("INY"); },

            [](JMP_Absolute op)    { return "JMP " + absolute(op.address); },
            [](JMP_Indirect op)    { return "JMP " + indirect(op.address); },

            [](JSR op)             { return std::string("JSR"); },

            [](LDA_Immediate op)   { return "LDA " + immediate(op.value); },
            [](LDA_ZeroPage op)    { return "LDA " + zeroPage(op.address); },
            [](LDA_ZeroPageX op)   { return "LDA " + zeroPageX(op.address); },
            [](LDA_Absolute op)    { return "LDA " + absolute(op.address); },
            [](LDA_AbsoluteX op)   { return "LDA " + absoluteX(op.address); },
            [](LDA_AbsoluteY op)   { return "LDA " + absoluteY(op.address); },
            [](LDA_IndirectX op)   { return "LDA " + indirectX(op.address); },
            [](LDA_IndirectY op)   { return "LDA " + indirectY(op.address); },

            [](LDX_Immediate op)   { return "LDX " + immediate(op.value); },
            [](LDX_ZeroPage op)    { return "LDX " + zeroPage(op.address); },
            [](LDX_ZeroPageY op)   { return "LDX " + zeroPageY(op.address); },
            [](LDX_Absolute op)    { return "LDX " + absolute(op.address); },
            [](LDX_AbsoluteY op)   { return "LDX " + absoluteY(op.address); },

            [](LDY_Immediate op)   { return "LDY " + immediate(op.value); },
            [](LDY_ZeroPage op)    { return "LDY " + zeroPage(op.address); },
            [](LDY_ZeroPageX op)   { return "LDY " + zeroPageX(op.address); },
            [](LDY_Absolute op)    { return "LDY " + absolute(op.address); },
            [](LDY_AbsoluteX op)   { return "LDY " + absoluteX(op.address); },

            [](LSR_Accumulator op) { return "LSR " + accumulator(); },
            [](LSR_ZeroPage op)    { return "LSR " + zeroPage(op.address); },
            [](LSR_ZeroPageX op)   { return "LSR " + zeroPageX(op.address); },
            [](LSR_Absolute op)    { return "LSR " + absolute(op.address); },
            [](LSR_AbsoluteX op)   { return "LSR " + absoluteX(op.address); },

            [](NOP op)             { return std::string("NOP"); },

            [](ORA_Immediate op)   { return "ORA " + immediate(op.value); },
            [](ORA_ZeroPage op)    { return "ORA " + zeroPage(op.address); },
            [](ORA_ZeroPageX op)   { return "ORA " + zeroPageX(op.address); },
            [](ORA_Absolute op)    { return "ORA " + absolute(op.address); },
            [](ORA_AbsoluteX op)   { return "ORA " + absoluteX(op.address); },
            [](ORA_AbsoluteY op)   { return "ORA " + absoluteY(op.address); },
            [](ORA_IndirectX op)   { return "ORA " + indirectX(op.address); },
            [](ORA_IndirectY op)   { return "ORA " + indirectY(op.address); },

            [](PHA op)             { return std::string("PHA"); },
            [](PHP op)             { return std::string("PHP"); },

            [](PLA op)             { return std::string("PLA"); },
            [](PLP op)             { return std::string("PLP"); },

            [](ROL_Accumulator op) { return "ROL " + accumulator(); },
            [](ROL_ZeroPage op)    { return "ROL " + zeroPage(op.address); },
            [](ROL_ZeroPageX op)   { return "ROL " + zeroPageX(op.address); },
            [](ROL_Absolute op)    { return "ROL " + absolute(op.address); },
            [](ROL_AbsoluteX op)   { return "ROL " + absoluteX(op.address); },

            [](ROR_Accumulator op) { return "ROR " + accumulator(); },
            [](ROR_ZeroPage op)    { return "ROR " + zeroPage(op.address); },
            [](ROR_ZeroPageX op)   { return "ROR " + zeroPageX(op.address); },
            [](ROR_Absolute op)    { return "ROR " + absolute(op.address); },
            [](ROR_AbsoluteX op)   { return "ROR " + absoluteX(op.address); },

            [](RTI op)             { return std::string("RTI"); },
            [](RTS op)             { return std::string("RTS"); },

            [](SBC_Immediate op)   { return "SBC " + immediate(op.value); },
            [](SBC_ZeroPage op)    { return "SBC " + zeroPage(op.address); },
            [](SBC_ZeroPageX op)   { return "SBC " + zeroPageX(op.address); },
            [](SBC_Absolute op)    { return "SBC " + absolute(op.address); },
            [](SBC_AbsoluteX op)   { return "SBC " + absoluteX(op.address); },
            [](SBC_AbsoluteY op)   { return "SBC " + absoluteY(op.address); },
            [](SBC_IndirectX op)   { return "SBC " + indirectX(op.address); },
            [](SBC_IndirectY op)   { return "SBC " + indirectY(op.address); },

            [](SEC op)             { return std::string("SEC"); },
            [](SED op)             { return std::string("SED"); },
            [](SEI op)             { return std::string("SEI"); },

            [](STA_ZeroPage op)    { return "STA " + zeroPage(op.address); },
            [](STA_ZeroPageX op)   { return "STA " + zeroPageX(op.address); },
            [](STA_Absolute op)    { return "STA " + absolute(op.address); },
            [](STA_AbsoluteX op)   { return "STA " + absoluteX(op.address); },
            [](STA_AbsoluteY op)   { return "STA " + absoluteY(op.address); },
            [](STA_IndirectX op)   { return "STA " + indirectX(op.address); },
            [](STA_IndirectY op)   { return "STA " + indirectY(op.address); },

            [](STX_ZeroPage op)    { return "STX " + zeroPage(op.address); },
            [](STX_ZeroPageY op)   { return "STX " + zeroPageY(op.address); },
            [](STX_Absolute op)    { return "STX " + absolute(op.address); },

            [](STY_ZeroPage op)    { return "STY " + zeroPage(op.address); },
            [](STY_ZeroPageX op)   { return "STY " + zeroPageX(op.address); },
            [](STY_Absolute op)    { return "STY " + absolute(op.address); },

            [](TAX op)             { return std::string("TAX"); },
            [](TAY op)             { return std::string("TAY"); },
            [](TSX op)             { return std::string("TSX"); },
            [](TXA op)             { return std::string("TXA"); },
            [](TXS op)             { return std::string("TXS"); },
            [](TYA op)             { return std::string("TYA"); }
        },
               operation);
}

std::vector<Byte> Emulator::encode(const Operation &operation) {
    return std::visit(Overload {
        [](ADC_Immediate op) -> std::vector<Byte> { return {ADC_IMMEDIATE, op.value}; },
        [](ADC_ZeroPage op) -> std::vector<Byte> { return {ADC_ZERO_PAGE, op.address}; },
        [](ADC_ZeroPageX op) -> std::vector<Byte> { return {ADC_ZERO_PAGE_X, op.address}; },
        [](ADC_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ADC_ABSOLUTE, address.low, address.high};
        },
        [](ADC_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ADC_ABSOLUTE_X, address.low, address.high};
        },
        [](ADC_AbsoluteY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ADC_ABSOLUTE_Y, address.low, address.high};
        },
        [](ADC_IndirectX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ADC_INDIRECT_X, address.low, address.high};
        },
        [](ADC_IndirectY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ADC_INDIRECT_Y, address.low, address.high};
        },

        [](AND_Immediate op) -> std::vector<Byte> { return {AND_IMMEDIATE, op.value}; },
        [](AND_ZeroPage op) -> std::vector<Byte> { return {AND_ZERO_PAGE, op.address}; },
        [](AND_ZeroPageX op) -> std::vector<Byte> { return {AND_ZERO_PAGE_X, op.address}; },
        [](AND_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {AND_ABSOLUTE, address.low, address.high};
        },
        [](AND_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {AND_ABSOLUTE_X, address.low, address.high};
        },
        [](AND_AbsoluteY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {AND_ABSOLUTE_Y, address.low, address.high};
        },
        [](AND_IndirectX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {AND_INDIRECT_X, address.low, address.high};
        },
        [](AND_IndirectY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {AND_INDIRECT_Y, address.low, address.high};
        },

        [](ASL_Accumulator op) -> std::vector<Byte> { return {ASL_ACCUMULATOR}; },
        [](ASL_ZeroPage op) -> std::vector<Byte> { return {ASL_ZERO_PAGE, op.address}; },
        [](ASL_ZeroPageX op) -> std::vector<Byte> { return {ASL_ZERO_PAGE_X, op.address}; },
        [](ASL_Absolute op) -> std::vector<Byte> {
            WordToBytes address(op.address);
            return {ASL_ABSOLUTE, address.low, address.high};
        },
        [](ASL_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address(op.address);
            return {ASL_ABSOLUTE_X, address.low, address.high};
        },

        [](BCC op) -> std::vector<Byte> { return {BCC_RELATIVE}; },
        [](BCS op) -> std::vector<Byte> { return {BCS_RELATIVE}; },
        [](BEQ op) -> std::vector<Byte> { return {BEQ_RELATIVE}; },
        [](BNE op) -> std::vector<Byte> { return {BNE_RELATIVE}; },
        [](BMI op) -> std::vector<Byte> { return {BMI_RELATIVE}; },
        [](BPL op) -> std::vector<Byte> { return {BPL_RELATIVE}; },
        [](BVC op) -> std::vector<Byte> { return {BVC_RELATIVE}; },
        [](BVS op) -> std::vector<Byte> { return {BVS_RELATIVE}; },

        [](BIT_ZeroPage op) -> std::vector<Byte> { return std::vector{BIT_ZERO_PAGE, op.address}; },
        [](BIT_Absolute op) -> std::vector<Byte> {
            WordToBytes address(op.address);
            return {BIT_ABSOLUTE, address.low, address.high};
        },

        [](BRK op) -> std::vector<Byte> { return {BRK_IMPLICIT}; },

        [](CLC op) -> std::vector<Byte> { return {CLC_IMPLICIT}; },
        [](CLD op) -> std::vector<Byte> { return {CLD_IMPLICIT}; },
        [](CLI op) -> std::vector<Byte> { return {CLI_IMPLICIT}; },
        [](CLV op) -> std::vector<Byte> { return {CLV_IMPLICIT}; },

        [](CMP_Immediate op) -> std::vector<Byte> { return {CMP_IMMEDIATE, op.value}; },
        [](CMP_ZeroPage op) -> std::vector<Byte> { return {CMP_ZERO_PAGE, op.address}; },
        [](CMP_ZeroPageX op) -> std::vector<Byte> { return {CMP_ZERO_PAGE_X, op.address}; },
        [](CMP_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {CMP_ABSOLUTE, address.low, address.high};
        },
        [](CMP_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {CMP_ABSOLUTE_X, address.low, address.high};
        },
        [](CMP_AbsoluteY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {CMP_ABSOLUTE_Y, address.low, address.high};
        },
        [](CMP_IndirectX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {CMP_INDIRECT_X, address.low, address.high};
        },
        [](CMP_IndirectY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {CMP_INDIRECT_Y, address.low, address.high};
        },

        [](CPX_Immediate op) -> std::vector<Byte> { return {CPX_IMMEDIATE, op.value}; },
        [](CPX_ZeroPage op) -> std::vector<Byte> { return {CPX_ZERO_PAGE, op.address}; },
        [](CPX_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {CPX_ABSOLUTE, address.low, address.high};
        },

        [](CPY_Immediate op) -> std::vector<Byte> { return {CPY_IMMEDIATE, op.value}; },
        [](CPY_ZeroPage op) -> std::vector<Byte> { return {CPY_ZERO_PAGE, op.address}; },
        [](CPY_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {CPY_ABSOLUTE, address.low, address.high};
        },

        [](DEC_ZeroPage op) -> std::vector<Byte> { return {DEC_ZERO_PAGE, op.address}; },
        [](DEC_ZeroPageX op) -> std::vector<Byte> { return {DEC_ZERO_PAGE_X, op.address}; },
        [](DEC_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {DEC_ABSOLUTE, address.low, address.high};
        },
        [](DEC_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {DEC_ABSOLUTE_X, address.low, address.high};
        },

        [](DEX op) -> std::vector<Byte> { return {DEX_IMPLICIT}; },
        [](DEY op) -> std::vector<Byte> { return {DEY_IMPLICIT}; },

        [](EOR_Immediate op) -> std::vector<Byte> { return {EOR_IMMEDIATE, op.value}; },
        [](EOR_ZeroPage op) -> std::vector<Byte> { return {EOR_ZERO_PAGE, op.address}; },
        [](EOR_ZeroPageX op) -> std::vector<Byte> { return {EOR_ZERO_PAGE_X, op.address}; },
        [](EOR_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {EOR_ABSOLUTE, address.low, address.high};
        },
        [](EOR_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {EOR_ABSOLUTE_X, address.low, address.high};
        },
        [](EOR_AbsoluteY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {EOR_ABSOLUTE_Y, address.low, address.high};
        },
        [](EOR_IndirectX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {EOR_INDIRECT_X, address.low, address.high};
        },
        [](EOR_IndirectY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {EOR_INDIRECT_Y, address.low, address.high};
        },

        [](INC_ZeroPage op) -> std::vector<Byte> { return {INC_ZERO_PAGE, op.address}; },
        [](INC_ZeroPageX op) -> std::vector<Byte> { return {INC_ZERO_PAGE_X, op.address}; },
        [](INC_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {INC_ABSOLUTE, address.low, address.high};
        },
        [](INC_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {INC_ABSOLUTE_X, address.low, address.high};
        },

        [](INX op) -> std::vector<Byte> { return {INX_IMPLICIT}; },
        [](INY op) -> std::vector<Byte> { return {INY_IMPLICIT}; },

        [](JMP_Absolute op) -> std::vector<Byte> {
            WordToBytes address(op.address);
            return {JMP_ABSOLUTE, address.low, address.high};
        },
        [](JMP_Indirect op) -> std::vector<Byte> {
            WordToBytes address(op.address);
            return {JMP_INDIRECT, address.low, address.high};
        },

        [](JSR op) -> std::vector<Byte> {
            WordToBytes address(op.address);
            return {JSR_ABSOLUTE, address.low, address.high};
        },

        [](LDA_Immediate op) -> std::vector<Byte> { return {LDA_IMMEDIATE, op.value}; },
        [](LDA_ZeroPage op) -> std::vector<Byte> { return {LDA_ZERO_PAGE, op.address}; },
        [](LDA_ZeroPageX op) -> std::vector<Byte> { return {LDA_ZERO_PAGE_X, op.address}; },
        [](LDA_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {LDA_ABSOLUTE, address.low, address.high};
        },
        [](LDA_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {LDA_ABSOLUTE_X, address.low, address.high};
        },
        [](LDA_AbsoluteY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {LDA_ABSOLUTE_Y, address.low, address.high};
        },
        [](LDA_IndirectX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {LDA_INDIRECT_X, address.low, address.high};
        },
        [](LDA_IndirectY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {LDA_INDIRECT_Y, address.low, address.high};
        },

        [](LDX_Immediate op) -> std::vector<Byte> { return {LDX_IMMEDIATE, op.value}; },
        [](LDX_ZeroPage op) -> std::vector<Byte> { return {LDX_ZERO_PAGE, op.address}; },
        [](LDX_ZeroPageY op) -> std::vector<Byte> { return {LDX_ZERO_PAGE_Y, op.address}; },
        [](LDX_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {LDX_ABSOLUTE, address.low, address.high};
        },
        [](LDX_AbsoluteY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {LDX_ABSOLUTE_Y, address.low, address.high};
        },

        [](LDY_Immediate op) -> std::vector<Byte> { return {LDY_IMMEDIATE, op.value}; },
        [](LDY_ZeroPage op) -> std::vector<Byte> { return {LDY_ZERO_PAGE, op.address}; },
        [](LDY_ZeroPageX op) -> std::vector<Byte> { return {LDY_ZERO_PAGE_X, op.address}; },
        [](LDY_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {LDY_ABSOLUTE, address.low, address.high};
        },
        [](LDY_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {LDY_ABSOLUTE_X, address.low, address.high};
        },

        [](LSR_Accumulator op) -> std::vector<Byte> { return {LSR_ACCUMULATOR}; },
        [](LSR_ZeroPage op) -> std::vector<Byte> { return {LSR_ZERO_PAGE, op.address}; },
        [](LSR_ZeroPageX op) -> std::vector<Byte> { return {LSR_ZERO_PAGE_X, op.address}; },
        [](LSR_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {LSR_ABSOLUTE, address.low, address.high};
        },
        [](LSR_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {LSR_ABSOLUTE_X, address.low, address.high};
        },

        [](NOP op) -> std::vector<Byte> { return {NOP_IMPLICIT}; },

        [](ORA_Immediate op) -> std::vector<Byte> { return {ORA_IMMEDIATE, op.value}; },
        [](ORA_ZeroPage op) -> std::vector<Byte> { return {ORA_ZERO_PAGE, op.address}; },
        [](ORA_ZeroPageX op) -> std::vector<Byte> { return {ORA_ZERO_PAGE_X, op.address}; },
        [](ORA_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ORA_ABSOLUTE, address.low, address.high};
        },
        [](ORA_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ORA_ABSOLUTE_X, address.low, address.high};
        },
        [](ORA_AbsoluteY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ORA_ABSOLUTE_Y, address.low, address.high};
        },
        [](ORA_IndirectX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ORA_INDIRECT_X, address.low, address.high};
        },
        [](ORA_IndirectY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ORA_INDIRECT_Y, address.low, address.high};
        },

        [](PHA op) -> std::vector<Byte> { return {PHA_IMPLICIT}; },
        [](PHP op) -> std::vector<Byte> { return {PHP_IMPLICIT}; },

        [](PLA op) -> std::vector<Byte> { return {PLA_IMPLICIT}; },
        [](PLP op) -> std::vector<Byte> { return {PLP_IMPLICIT}; },

        [](ROL_Accumulator op) -> std::vector<Byte> { return {ROL_ACCUMULATOR}; },
        [](ROL_ZeroPage op) -> std::vector<Byte> { return {ROL_ZERO_PAGE, op.address}; },
        [](ROL_ZeroPageX op) -> std::vector<Byte> { return {ROL_ZERO_PAGE_X, op.address}; },
        [](ROL_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ROL_ABSOLUTE, address.low, address.high};
        },
        [](ROL_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ROL_ABSOLUTE_X, address.low, address.high};
        },

        [](ROR_Accumulator op) -> std::vector<Byte> { return {ROR_ACCUMULATOR}; },
        [](ROR_ZeroPage op) -> std::vector<Byte> { return {ROR_ZERO_PAGE, op.address}; },
        [](ROR_ZeroPageX op) -> std::vector<Byte> { return {ROR_ZERO_PAGE_X, op.address}; },
        [](ROR_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ROR_ABSOLUTE, address.low, address.high};
        },
        [](ROR_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {ROR_ABSOLUTE_X, address.low, address.high};
        },

        [](RTI op) -> std::vector<Byte> { return {RTI_IMPLICIT}; },
        [](RTS op) -> std::vector<Byte> { return {RTS_IMPLICIT}; },

        [](SBC_Immediate op) -> std::vector<Byte> { return {SBC_IMMEDIATE, op.value}; },
        [](SBC_ZeroPage op) -> std::vector<Byte> { return {SBC_ZERO_PAGE, op.address}; },
        [](SBC_ZeroPageX op) -> std::vector<Byte> { return {SBC_ZERO_PAGE_X, op.address}; },
        [](SBC_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {SBC_ABSOLUTE, address.low, address.high};
        },
        [](SBC_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {SBC_ABSOLUTE_X, address.low, address.high};
        },
        [](SBC_AbsoluteY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {SBC_ABSOLUTE_Y, address.low, address.high};
        },
        [](SBC_IndirectX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {SBC_INDIRECT_X, address.low, address.high};
        },
        [](SBC_IndirectY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {SBC_INDIRECT_Y, address.low, address.high};
        },

        [](SEC op) -> std::vector<Byte> { return {SEC_IMPLICIT}; },
        [](SED op) -> std::vector<Byte> { return {SED_IMPLICIT}; },
        [](SEI op) -> std::vector<Byte> { return {SEI_IMPLICIT}; },

        [](STA_ZeroPage op) -> std::vector<Byte> { return {STA_ZERO_PAGE, op.address}; },
        [](STA_ZeroPageX op) -> std::vector<Byte> { return {STA_ZERO_PAGE_X, op.address}; },
        [](STA_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {STA_ABSOLUTE, address.low, address.high};
        },
        [](STA_AbsoluteX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {STA_ABSOLUTE_X, address.low, address.high};
        },
        [](STA_AbsoluteY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {STA_ABSOLUTE_Y, address.low, address.high};
        },
        [](STA_IndirectX op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {STA_INDIRECT_X, address.low, address.high};
        },
        [](STA_IndirectY op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {STA_INDIRECT_Y, address.low, address.high};
        },

        [](STX_ZeroPage op) -> std::vector<Byte> { return {STX_ZERO_PAGE, op.address}; },
        [](STX_ZeroPageY op) -> std::vector<Byte> { return {STX_ZERO_PAGE_Y, op.address}; },
        [](STX_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {STX_ABSOLUTE, address.low, address.high};
        },

        [](STY_ZeroPage op) -> std::vector<Byte> { return {STY_ZERO_PAGE, op.address}; },
        [](STY_ZeroPageX op) -> std::vector<Byte> { return {STY_ZERO_PAGE_X, op.address}; },
        [](STY_Absolute op) -> std::vector<Byte> {
            WordToBytes address{op.address};
            return {STY_ABSOLUTE, address.low, address.high};
        },

        [](TAX op) -> std::vector<Byte> { return {TAX_IMPLICIT}; },
        [](TAY op) -> std::vector<Byte> { return {TAY_IMPLICIT}; },
        [](TSX op) -> std::vector<Byte> { return {TSX_IMPLICIT}; },
        [](TXA op) -> std::vector<Byte> { return {TXA_IMPLICIT}; },
        [](TXS op) -> std::vector<Byte> { return {TXS_IMPLICIT}; },
        [](TYA op) -> std::vector<Byte> { return {TYA_IMPLICIT}; }
    },
          operation);
}

std::expected<Operation, InvalidOperation> decode(const ROM &memory, Word &PC, size_t &cycle) {
    Byte opCode = memory.fetch_byte(PC, cycle);

    switch (opCode) {
        default: return std::unexpected<InvalidOperation>{opCode};

        case ADC_IMMEDIATE:   return ADC_Immediate{.value = memory.fetch_byte(PC, cycle)};
        case ADC_ZERO_PAGE:   return ADC_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case ADC_ZERO_PAGE_X: return ADC_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case ADC_ABSOLUTE:    return ADC_Absolute{.address = memory.fetch_word(PC, cycle)};
        case ADC_ABSOLUTE_X:  return ADC_AbsoluteX{.address = memory.fetch_word(PC, cycle)};
        case ADC_ABSOLUTE_Y:  return ADC_AbsoluteY{.address = memory.fetch_word(PC, cycle)};
        case ADC_INDIRECT_X:  return ADC_IndirectX{.address = memory.fetch_byte(PC, cycle)};
        case ADC_INDIRECT_Y:  return ADC_IndirectY{.address = memory.fetch_byte(PC, cycle)};

        case AND_IMMEDIATE:   return AND_Immediate{.value = memory.fetch_byte(PC, cycle)};
        case AND_ZERO_PAGE:   return AND_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case AND_ZERO_PAGE_X: return AND_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case AND_ABSOLUTE:    return AND_Absolute{.address = memory.fetch_word(PC, cycle)};
        case AND_ABSOLUTE_X:  return AND_AbsoluteX{.address = memory.fetch_word(PC, cycle)};
        case AND_ABSOLUTE_Y:  return AND_AbsoluteY{.address = memory.fetch_word(PC, cycle)};
        case AND_INDIRECT_X:  return AND_IndirectX{.address = memory.fetch_byte(PC, cycle)};
        case AND_INDIRECT_Y:  return AND_IndirectY{.address = memory.fetch_byte(PC, cycle)};

        case ASL_ACCUMULATOR: return ASL_Accumulator{};
        case ASL_ZERO_PAGE:   return ASL_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case ASL_ZERO_PAGE_X: return ASL_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case ASL_ABSOLUTE:    return ASL_Absolute{.address = memory.fetch_word(PC, cycle)};
        case ASL_ABSOLUTE_X:  return ASL_AbsoluteX{.address = memory.fetch_word(PC, cycle)};

        case BCC_RELATIVE:    return BCC{.offset = (char)memory.fetch_byte(PC, cycle)};
        case BCS_RELATIVE:    return BCS{.offset = (char)memory.fetch_byte(PC, cycle)};
        case BEQ_RELATIVE:    return BEQ{.offset = (char)memory.fetch_byte(PC, cycle)};
        case BNE_RELATIVE:    return BNE{.offset = (char)memory.fetch_byte(PC, cycle)};
        case BMI_RELATIVE:    return BMI{.offset = (char)memory.fetch_byte(PC, cycle)};
        case BPL_RELATIVE:    return BPL{.offset = (char)memory.fetch_byte(PC, cycle)};
        case BVC_RELATIVE:    return BVC{.offset = (char)memory.fetch_byte(PC, cycle)};
        case BVS_RELATIVE:    return BVS{.offset = (char)memory.fetch_byte(PC, cycle)};

        case BIT_ZERO_PAGE:   return BIT_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case BIT_ABSOLUTE:    return BIT_Absolute{.address = memory.fetch_word(PC, cycle)};

        case BRK_IMPLICIT:    return BRK{};

        case CLC_IMPLICIT:    return CLC{};
        case CLD_IMPLICIT:    return CLD{};
        case CLI_IMPLICIT:    return CLI{};
        case CLV_IMPLICIT:    return CLV{};

        case CMP_IMMEDIATE:   return CMP_Immediate{.value = memory.fetch_byte(PC, cycle)};
        case CMP_ZERO_PAGE:   return CMP_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case CMP_ZERO_PAGE_X: return CMP_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case CMP_ABSOLUTE:    return CMP_Absolute{.address = memory.fetch_word(PC, cycle)};
        case CMP_ABSOLUTE_X:  return CMP_AbsoluteX{.address = memory.fetch_word(PC, cycle)};
        case CMP_ABSOLUTE_Y:  return CMP_AbsoluteY{.address = memory.fetch_word(PC, cycle)};
        case CMP_INDIRECT_X:  return CMP_IndirectX{.address = memory.fetch_byte(PC, cycle)};
        case CMP_INDIRECT_Y:  return CMP_IndirectY{.address = memory.fetch_byte(PC, cycle)};

        case CPX_IMMEDIATE:   return CPX_Immediate{.value = memory.fetch_byte(PC, cycle)};
        case CPX_ZERO_PAGE:   return CPX_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case CPX_ABSOLUTE:    return CPX_Absolute{.address = memory.fetch_word(PC, cycle)};

        case CPY_IMMEDIATE:   return CPY_Immediate{.value = memory.fetch_byte(PC, cycle)};
        case CPY_ZERO_PAGE:   return CPY_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case CPY_ABSOLUTE:    return CPY_Absolute{.address = memory.fetch_word(PC, cycle)};

        case DEC_ZERO_PAGE:   return DEC_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case DEC_ZERO_PAGE_X: return DEC_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case DEC_ABSOLUTE:    return DEC_Absolute{.address = memory.fetch_word(PC, cycle)};
        case DEC_ABSOLUTE_X:  return DEC_AbsoluteX{.address = memory.fetch_word(PC, cycle)};

        case DEX_IMPLICIT:    return DEX{};
        case DEY_IMPLICIT:    return DEY{};

        case EOR_IMMEDIATE:   return EOR_Immediate{.value = memory.fetch_byte(PC, cycle)};
        case EOR_ZERO_PAGE:   return EOR_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case EOR_ZERO_PAGE_X: return EOR_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case EOR_ABSOLUTE:    return EOR_Absolute{.address = memory.fetch_word(PC, cycle)};
        case EOR_ABSOLUTE_X:  return EOR_AbsoluteX{.address = memory.fetch_word(PC, cycle)};
        case EOR_ABSOLUTE_Y:  return EOR_AbsoluteY{.address = memory.fetch_word(PC, cycle)};
        case EOR_INDIRECT_X:  return EOR_IndirectX{.address = memory.fetch_byte(PC, cycle)};
        case EOR_INDIRECT_Y:  return EOR_IndirectY{.address = memory.fetch_byte(PC, cycle)};

        case INC_ZERO_PAGE:   return INC_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case INC_ZERO_PAGE_X: return INC_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case INC_ABSOLUTE:    return INC_Absolute{.address = memory.fetch_word(PC, cycle)};
        case INC_ABSOLUTE_X:  return INC_AbsoluteX{.address = memory.fetch_word(PC, cycle)};

        case INX_IMPLICIT:    return INX{};
        case INY_IMPLICIT:    return INY{};

        case JMP_ABSOLUTE:    return JMP_Absolute{.address = memory.fetch_word(PC, cycle)};
        case JMP_INDIRECT:    return JMP_Indirect{.address = memory.fetch_word(PC, cycle)};

        case JSR_ABSOLUTE:    return JSR{.address = memory.fetch_word(PC, cycle)};

        case LDA_IMMEDIATE:   return LDA_Immediate{.value = memory.fetch_byte(PC, cycle)};
        case LDA_ZERO_PAGE:   return LDA_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case LDA_ZERO_PAGE_X: return LDA_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case LDA_ABSOLUTE:    return LDA_Absolute{.address = memory.fetch_word(PC, cycle)};
        case LDA_ABSOLUTE_X:  return LDA_AbsoluteX{.address = memory.fetch_word(PC, cycle)};
        case LDA_ABSOLUTE_Y:  return LDA_AbsoluteY{.address = memory.fetch_word(PC, cycle)};
        case LDA_INDIRECT_X:  return LDA_IndirectX{.address = memory.fetch_byte(PC, cycle)};
        case LDA_INDIRECT_Y:  return LDA_IndirectY{.address = memory.fetch_byte(PC, cycle)};

        case LDX_IMMEDIATE:   return LDX_Immediate{.value = memory.fetch_byte(PC, cycle)};
        case LDX_ZERO_PAGE:   return LDX_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case LDX_ZERO_PAGE_Y: return LDX_ZeroPageY{.address = memory.fetch_byte(PC, cycle)};
        case LDX_ABSOLUTE:    return LDX_Absolute{.address = memory.fetch_word(PC, cycle)};
        case LDX_ABSOLUTE_Y:  return LDX_AbsoluteY{.address = memory.fetch_word(PC, cycle)};

        case LDY_IMMEDIATE:   return LDY_Immediate{.value = memory.fetch_byte(PC, cycle)};
        case LDY_ZERO_PAGE:   return LDY_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case LDY_ZERO_PAGE_X: return LDY_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case LDY_ABSOLUTE:    return LDY_Absolute{.address = memory.fetch_word(PC, cycle)};
        case LDY_ABSOLUTE_X:  return LDY_AbsoluteX{.address = memory.fetch_word(PC, cycle)};

        case LSR_ACCUMULATOR: return LSR_Accumulator{};
        case LSR_ZERO_PAGE:   return LSR_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case LSR_ZERO_PAGE_X: return LSR_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case LSR_ABSOLUTE:    return LSR_Absolute{.address = memory.fetch_word(PC, cycle)};
        case LSR_ABSOLUTE_X:  return LSR_AbsoluteX{.address = memory.fetch_word(PC, cycle)};

        case NOP_IMPLICIT:    return NOP{};

        case ORA_IMMEDIATE:   return ORA_Immediate{.value = memory.fetch_byte(PC, cycle)};
        case ORA_ZERO_PAGE:   return ORA_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case ORA_ZERO_PAGE_X: return ORA_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case ORA_ABSOLUTE:    return ORA_Absolute{.address = memory.fetch_word(PC, cycle)};
        case ORA_ABSOLUTE_X:  return ORA_AbsoluteX{.address = memory.fetch_word(PC, cycle)};
        case ORA_ABSOLUTE_Y:  return ORA_AbsoluteY{.address = memory.fetch_word(PC, cycle)};
        case ORA_INDIRECT_X:  return ORA_IndirectX{.address = memory.fetch_byte(PC, cycle)};
        case ORA_INDIRECT_Y:  return ORA_IndirectY{.address = memory.fetch_byte(PC, cycle)};

        case PHA_IMPLICIT:    return PHA{};
        case PHP_IMPLICIT:    return PHP{};

        case PLA_IMPLICIT:    return PLA{};
        case PLP_IMPLICIT:    return PLP{};

        case ROL_ACCUMULATOR: return ROL_Accumulator{};
        case ROL_ZERO_PAGE:   return ROL_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case ROL_ZERO_PAGE_X: return ROL_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case ROL_ABSOLUTE:    return ROL_Absolute{.address = memory.fetch_word(PC, cycle)};
        case ROL_ABSOLUTE_X:  return ROL_AbsoluteX{.address = memory.fetch_word(PC, cycle)};

        case ROR_ACCUMULATOR: return ROR_Accumulator{};
        case ROR_ZERO_PAGE:   return ROR_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case ROR_ZERO_PAGE_X: return ROR_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case ROR_ABSOLUTE:    return ROR_Absolute{.address = memory.fetch_word(PC, cycle)};
        case ROR_ABSOLUTE_X:  return ROR_AbsoluteX{.address = memory.fetch_word(PC, cycle)};

        case RTI_IMPLICIT:    return RTI{};
        case RTS_IMPLICIT:    return RTS{};

        case SBC_IMMEDIATE:   return SBC_Immediate{.value = memory.fetch_byte(PC, cycle)};
        case SBC_ZERO_PAGE:   return SBC_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case SBC_ZERO_PAGE_X: return SBC_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case SBC_ABSOLUTE:    return SBC_Absolute{.address = memory.fetch_word(PC, cycle)};
        case SBC_ABSOLUTE_X:  return SBC_AbsoluteX{.address = memory.fetch_word(PC, cycle)};
        case SBC_ABSOLUTE_Y:  return SBC_AbsoluteY{.address = memory.fetch_word(PC, cycle)};
        case SBC_INDIRECT_X:  return SBC_IndirectX{.address = memory.fetch_byte(PC, cycle)};
        case SBC_INDIRECT_Y:  return SBC_IndirectY{.address = memory.fetch_byte(PC, cycle)};

        case SEC_IMPLICIT:    return SEC{};
        case SED_IMPLICIT:    return SED{};
        case SEI_IMPLICIT:    return SEI{};

        case STA_ZERO_PAGE:   return STA_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case STA_ZERO_PAGE_X: return STA_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case STA_ABSOLUTE:    return STA_Absolute{.address = memory.fetch_word(PC, cycle)};
        case STA_ABSOLUTE_X:  return STA_AbsoluteX{.address = memory.fetch_word(PC, cycle)};
        case STA_ABSOLUTE_Y:  return STA_AbsoluteY{.address = memory.fetch_word(PC, cycle)};
        case STA_INDIRECT_X:  return STA_IndirectX{.address = memory.fetch_byte(PC, cycle)};
        case STA_INDIRECT_Y:  return STA_IndirectY{.address = memory.fetch_byte(PC, cycle)};

        case STX_ZERO_PAGE:   return STX_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case STX_ZERO_PAGE_Y: return STX_ZeroPageY{.address = memory.fetch_byte(PC, cycle)};
        case STX_ABSOLUTE:    return STX_Absolute{.address = memory.fetch_word(PC, cycle)};

        case STY_ZERO_PAGE:   return STY_ZeroPage{.address = memory.fetch_byte(PC, cycle)};
        case STY_ZERO_PAGE_X: return STY_ZeroPageX{.address = memory.fetch_byte(PC, cycle)};
        case STY_ABSOLUTE:    return STY_Absolute{.address = memory.fetch_word(PC, cycle)};

        case TAX_IMPLICIT:    return TAX{};
        case TAY_IMPLICIT:    return TAY{};
        case TSX_IMPLICIT:    return TSX{};
        case TXA_IMPLICIT:    return TXA{};
        case TXS_IMPLICIT:    return TXS{};
        case TYA_IMPLICIT:    return TYA{};
    }
}
