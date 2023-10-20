//
// Created by Mikhail on 14/10/2023.
//

#include <format>
#include <iostream>

#include "Operation.hpp"

using namespace Emulator;

constexpr static inline std::string accumulator_description(const std::string &name) { return std::format("{} A", name); }
static inline std::string immediate_description(std::string name, Byte value)        { return std::vformat("{} #{:02u}", std::make_format_args(std::move(name), value)); }
static inline std::string zeroPage_description(std::string name, Byte address)       { return std::vformat("{} ${:02x}", std::make_format_args(std::move(name), address)); }
static inline std::string zeroPageX_description(std::string name, Byte address)      { return std::vformat("{} ${:02x},X", std::make_format_args(std::move(name), address)); }
static inline std::string zeroPageY_description(std::string name, Byte address)      { return std::vformat("{} ${:02x},Y", std::make_format_args(std::move(name), address)); }
static inline std::string relative_description(std::string name, char offset)        { return std::vformat("{} *{:+}", std::make_format_args(std::move(name), offset)); }
static inline std::string absolute_description(std::string name, Word address)       { return std::vformat("{} ${:04x}", std::make_format_args(std::move(name), address)); }
static inline std::string absoluteX_description(std::string name, Word address)      { return std::vformat("{} ${:04x},X", std::make_format_args(std::move(name), address)); }
static inline std::string absoluteY_description(std::string name, Word address)      { return std::vformat("{} ${:04x},Y", std::make_format_args(std::move(name), address)); }
static inline std::string indirect_description(std::string name, Word address)       { return std::vformat("{} (${:04x})", std::make_format_args(std::move(name), address)); }
static inline std::string indirectX_description(std::string name, Word address)      { return std::vformat("{} (${:04x},X)", std::make_format_args(std::move(name), address)); }
static inline std::string indirectY_description(std::string name, Word address)      { return std::vformat("{} (${:04x}),Y", std::make_format_args(std::move(name), address)); }



static inline std::vector<Byte> no_argument(OpCode opCode) { return {opCode}; }
static inline std::vector<Byte> byte_argument(OpCode opCode, Byte arg) { return {opCode, arg}; }
static std::vector<Byte> word_argument(OpCode opCode, Word arg) {
    WordToBytes buf(arg);
    return {opCode, buf.low, buf.high};
}



std::string Emulator::description(const Operation &operation) noexcept {
    return std::visit(Overload {
            [](ADC_Immediate op)   { return immediate_description("ADC", op.value); },
            [](ADC_ZeroPage op)    { return zeroPage_description("ADC", op.address); },
            [](ADC_ZeroPageX op)   { return zeroPageX_description("ADC", op.address); },
            [](ADC_Absolute op)    { return absolute_description("ADC", op.address); },
            [](ADC_AbsoluteX op)   { return absoluteX_description("ADC", op.address); },
            [](ADC_AbsoluteY op)   { return absoluteY_description("ADC", op.address); },
            [](ADC_IndirectX op)   { return indirectX_description("ADC", op.address); },
            [](ADC_IndirectY op)   { return indirectY_description("ADC", op.address); },

            [](AND_Immediate op)   { return immediate_description("AND", op.value); },
            [](AND_ZeroPage op)    { return zeroPage_description("AND", op.address); },
            [](AND_ZeroPageX op)   { return zeroPageX_description("AND", op.address); },
            [](AND_Absolute op)    { return absolute_description("AND", op.address); },
            [](AND_AbsoluteX op)   { return absoluteX_description("AND", op.address); },
            [](AND_AbsoluteY op)   { return absoluteY_description("AND", op.address); },
            [](AND_IndirectX op)   { return indirectX_description("AND", op.address); },
            [](AND_IndirectY op)   { return indirectY_description("AND", op.address); },

            [](ASL_Accumulator op) { return accumulator_description("ASL"); },
            [](ASL_ZeroPage op)    { return zeroPage_description("ASL", op.address); },
            [](ASL_ZeroPageX op)   { return zeroPageX_description("ASL", op.address); },
            [](ASL_Absolute op)    { return absolute_description("ASL", op.address); },
            [](ASL_AbsoluteX op)   { return absoluteX_description("ASL", op.address); },

            [](BCC op)             { return relative_description("BCC", op.offset); },
            [](BCS op)             { return relative_description("BCC", op.offset); },
            [](BEQ op)             { return relative_description("BCC", op.offset); },
            [](BNE op)             { return relative_description("BCC", op.offset); },
            [](BMI op)             { return relative_description("BCC", op.offset); },
            [](BPL op)             { return relative_description("BCC", op.offset); },
            [](BVC op)             { return relative_description("BCC", op.offset); },
            [](BVS op)             { return relative_description("BCC", op.offset); },

            [](BIT_ZeroPage op)    { return zeroPage_description("BIT", op.address); },
            [](BIT_Absolute op)    { return absolute_description("BIT", op.address); },

            [](BRK op)             { return std::string("BRK"); },

            [](CLC op)             { return std::string("CLC"); },
            [](CLD op)             { return std::string("CLD"); },
            [](CLI op)             { return std::string("CLI"); },
            [](CLV op)             { return std::string("CLV"); },

            [](CMP_Immediate op)   { return immediate_description("CMP", op.value); },
            [](CMP_ZeroPage op)    { return zeroPage_description("CMP", op.address); },
            [](CMP_ZeroPageX op)   { return zeroPageX_description("CMP", op.address); },
            [](CMP_Absolute op)    { return absolute_description("CMP", op.address); },
            [](CMP_AbsoluteX op)   { return absoluteX_description("CMP", op.address); },
            [](CMP_AbsoluteY op)   { return absoluteY_description("CMP", op.address); },
            [](CMP_IndirectX op)   { return indirectX_description("CMP", op.address); },
            [](CMP_IndirectY op)   { return indirectY_description("CMP", op.address); },

            [](CPX_Immediate op)   { return immediate_description("CPX", op.value); },
            [](CPX_ZeroPage op)    { return zeroPage_description("CPX", op.address); },
            [](CPX_Absolute op)    { return absolute_description("CPX", op.address); },

            [](CPY_Immediate op)   { return immediate_description("CPY", op.value); },
            [](CPY_ZeroPage op)    { return zeroPage_description("CPY", op.address); },
            [](CPY_Absolute op)    { return absolute_description("CPY", op.address); },

            [](DEC_ZeroPage op)    { return zeroPage_description("DEC", op.address); },
            [](DEC_ZeroPageX op)   { return zeroPageX_description("DEC", op.address); },
            [](DEC_Absolute op)    { return absolute_description("DEC", op.address); },
            [](DEC_AbsoluteX op)   { return absoluteX_description("DEC", op.address); },

            [](DEX op)             { return std::string("DEX"); },
            [](DEY op)             { return std::string("DEY"); },

            [](EOR_Immediate op)   { return immediate_description("EOR", op.value); },
            [](EOR_ZeroPage op)    { return zeroPage_description("EOR", op.address); },
            [](EOR_ZeroPageX op)   { return zeroPageX_description("EOR", op.address); },
            [](EOR_Absolute op)    { return absolute_description("EOR", op.address); },
            [](EOR_AbsoluteX op)   { return absoluteX_description("EOR", op.address); },
            [](EOR_AbsoluteY op)   { return absoluteY_description("EOR", op.address); },
            [](EOR_IndirectX op)   { return indirectX_description("EOR", op.address); },
            [](EOR_IndirectY op)   { return indirectY_description("EOR", op.address); },

            [](INC_ZeroPage op)    { return zeroPage_description("INC", op.address); },
            [](INC_ZeroPageX op)   { return zeroPageX_description("INC", op.address); },
            [](INC_Absolute op)    { return absolute_description("INC", op.address); },
            [](INC_AbsoluteX op)   { return absoluteX_description("INC", op.address); },

            [](INX op)             { return std::string("INX"); },
            [](INY op)             { return std::string("INY"); },

            [](JMP_Absolute op)    { return absolute_description("JMP", op.address); },
            [](JMP_Indirect op)    { return indirect_description("JMP", op.address); },

            [](JSR op)             { return std::string("JSR"); },

            [](LDA_Immediate op)   { return immediate_description("LDA", op.value); },
            [](LDA_ZeroPage op)    { return zeroPage_description("LDA", op.address); },
            [](LDA_ZeroPageX op)   { return zeroPageX_description("LDA", op.address); },
            [](LDA_Absolute op)    { return absolute_description("LDA", op.address); },
            [](LDA_AbsoluteX op)   { return absoluteX_description("LDA", op.address); },
            [](LDA_AbsoluteY op)   { return absoluteY_description("LDA", op.address); },
            [](LDA_IndirectX op)   { return indirectX_description("LDA", op.address); },
            [](LDA_IndirectY op)   { return indirectY_description("LDA", op.address); },

            [](LDX_Immediate op)   { return immediate_description("LDX", op.value); },
            [](LDX_ZeroPage op)    { return zeroPage_description("LDX", op.address); },
            [](LDX_ZeroPageY op)   { return zeroPageY_description("LDX", op.address); },
            [](LDX_Absolute op)    { return absolute_description("LDX", op.address); },
            [](LDX_AbsoluteY op)   { return absoluteY_description("LDX", op.address); },

            [](LDY_Immediate op)   { return immediate_description("LDY", op.value); },
            [](LDY_ZeroPage op)    { return zeroPage_description("LDY", op.address); },
            [](LDY_ZeroPageX op)   { return zeroPageX_description("LDY", op.address); },
            [](LDY_Absolute op)    { return absolute_description("LDY", op.address); },
            [](LDY_AbsoluteX op)   { return absoluteX_description("LDY", op.address); },

            [](LSR_Accumulator op) { return accumulator_description("LSR"); },
            [](LSR_ZeroPage op)    { return zeroPage_description("LSR", op.address); },
            [](LSR_ZeroPageX op)   { return zeroPageX_description("LSR", op.address); },
            [](LSR_Absolute op)    { return absolute_description("LSR", op.address); },
            [](LSR_AbsoluteX op)   { return absoluteX_description("LSR", op.address); },

            [](NOP op)             { return std::string("NOP"); },

            [](ORA_Immediate op)   { return immediate_description("ORA", op.value); },
            [](ORA_ZeroPage op)    { return zeroPage_description("ORA", op.address); },
            [](ORA_ZeroPageX op)   { return zeroPageX_description("ORA", op.address); },
            [](ORA_Absolute op)    { return absolute_description("ORA", op.address); },
            [](ORA_AbsoluteX op)   { return absoluteX_description("ORA", op.address); },
            [](ORA_AbsoluteY op)   { return absoluteY_description("ORA", op.address); },
            [](ORA_IndirectX op)   { return indirectX_description("ORA", op.address); },
            [](ORA_IndirectY op)   { return indirectY_description("ORA", op.address); },

            [](PHA op)             { return std::string("PHA"); },
            [](PHP op)             { return std::string("PHP"); },

            [](PLA op)             { return std::string("PLA"); },
            [](PLP op)             { return std::string("PLP"); },

            [](ROL_Accumulator op) { return accumulator_description("ROL"); },
            [](ROL_ZeroPage op)    { return zeroPage_description("ROL", op.address); },
            [](ROL_ZeroPageX op)   { return zeroPageX_description("ROL", op.address); },
            [](ROL_Absolute op)    { return absolute_description("ROL", op.address); },
            [](ROL_AbsoluteX op)   { return absoluteX_description("ROL", op.address); },

            [](ROR_Accumulator op) { return accumulator_description("ROR"); },
            [](ROR_ZeroPage op)    { return zeroPage_description("ROR", op.address); },
            [](ROR_ZeroPageX op)   { return zeroPageX_description("ROR", op.address); },
            [](ROR_Absolute op)    { return absolute_description("ROR", op.address); },
            [](ROR_AbsoluteX op)   { return absoluteX_description("ROR", op.address); },

            [](RTI op)             { return std::string("RTI"); },
            [](RTS op)             { return std::string("RTS"); },

            [](SBC_Immediate op)   { return immediate_description("SBC", op.value); },
            [](SBC_ZeroPage op)    { return zeroPage_description("SBC", op.address); },
            [](SBC_ZeroPageX op)   { return zeroPageX_description("SBC", op.address); },
            [](SBC_Absolute op)    { return absolute_description("SBC", op.address); },
            [](SBC_AbsoluteX op)   { return absoluteX_description("SBC", op.address); },
            [](SBC_AbsoluteY op)   { return absoluteY_description("SBC", op.address); },
            [](SBC_IndirectX op)   { return indirectX_description("SBC", op.address); },
            [](SBC_IndirectY op)   { return indirectY_description("SBC", op.address); },

            [](SEC op)             { return std::string("SEC"); },
            [](SED op)             { return std::string("SED"); },
            [](SEI op)             { return std::string("SEI"); },

            [](STA_ZeroPage op)    { return zeroPage_description("STA", op.address); },
            [](STA_ZeroPageX op)   { return zeroPageX_description("STA", op.address); },
            [](STA_Absolute op)    { return absolute_description("STA", op.address); },
            [](STA_AbsoluteX op)   { return absoluteX_description("STA", op.address); },
            [](STA_AbsoluteY op)   { return absoluteY_description("STA", op.address); },
            [](STA_IndirectX op)   { return indirectX_description("STA", op.address); },
            [](STA_IndirectY op)   { return indirectY_description("STA", op.address); },

            [](STX_ZeroPage op)    { return zeroPage_description("STX", op.address); },
            [](STX_ZeroPageY op)   { return zeroPageY_description("STX", op.address); },
            [](STX_Absolute op)    { return absolute_description("STX", op.address); },

            [](STY_ZeroPage op)    { return zeroPage_description("STY", op.address); },
            [](STY_ZeroPageX op)   { return zeroPageX_description("STY", op.address); },
            [](STY_Absolute op)    { return absolute_description("STY", op.address); },

            [](TAX op)             { return std::string("TAX"); },
            [](TAY op)             { return std::string("TAY"); },
            [](TSX op)             { return std::string("TSX"); },
            [](TXA op)             { return std::string("TXA"); },
            [](TXS op)             { return std::string("TXS"); },
            [](TYA op)             { return std::string("TYA"); }
        },
               operation);
}

std::vector<Byte> Emulator::encode(const Operation &operation) noexcept {
    return std::visit(Overload {
        [](ADC_Immediate op)   { return byte_argument(op.opcode, op.value); },
        [](ADC_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](ADC_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](ADC_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](ADC_AbsoluteX op)   { return word_argument(op.opcode, op.address); },
        [](ADC_AbsoluteY op)   { return word_argument(op.opcode, op.address); },
        [](ADC_IndirectX op)   { return byte_argument(op.opcode, op.address); },
        [](ADC_IndirectY op)   { return byte_argument(op.opcode, op.address); },

        [](AND_Immediate op)   { return byte_argument(op.opcode, op.value); },
        [](AND_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](AND_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](AND_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](AND_AbsoluteX op)   { return word_argument(op.opcode, op.address); },
        [](AND_AbsoluteY op)   { return word_argument(op.opcode, op.address); },
        [](AND_IndirectX op)   { return byte_argument(op.opcode, op.address); },
        [](AND_IndirectY op)   { return byte_argument(op.opcode, op.address); },

        [](ASL_Accumulator op) { return no_argument(op.opcode); },
        [](ASL_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](ASL_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](ASL_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](ASL_AbsoluteX op)   { return word_argument(op.opcode, op.address); },

        [](BCC op)             { return byte_argument(op.opcode, op.offset); },
        [](BCS op)             { return byte_argument(op.opcode, op.offset); },
        [](BEQ op)             { return byte_argument(op.opcode, op.offset); },
        [](BNE op)             { return byte_argument(op.opcode, op.offset); },
        [](BMI op)             { return byte_argument(op.opcode, op.offset); },
        [](BPL op)             { return byte_argument(op.opcode, op.offset); },
        [](BVC op)             { return byte_argument(op.opcode, op.offset); },
        [](BVS op)             { return byte_argument(op.opcode, op.offset); },

        [](BIT_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](BIT_Absolute op)    { return word_argument(op.opcode, op.address); },

        [](BRK op)             { return no_argument(op.opcode); },

        [](CLC op)             { return no_argument(op.opcode); },
        [](CLD op)             { return no_argument(op.opcode); },
        [](CLI op)             { return no_argument(op.opcode); },
        [](CLV op)             { return no_argument(op.opcode); },

        [](CMP_Immediate op)   { return byte_argument(op.opcode, op.value); },
        [](CMP_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](CMP_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](CMP_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](CMP_AbsoluteX op)   { return word_argument(op.opcode, op.address); },
        [](CMP_AbsoluteY op)   { return word_argument(op.opcode, op.address); },
        [](CMP_IndirectX op)   { return byte_argument(op.opcode, op.address); },
        [](CMP_IndirectY op)   { return byte_argument(op.opcode, op.address); },

        [](CPX_Immediate op)   { return byte_argument(op.opcode, op.value); },
        [](CPX_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](CPX_Absolute op)    { return word_argument(op.opcode, op.address); },

        [](CPY_Immediate op)   { return byte_argument(op.opcode, op.value); },
        [](CPY_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](CPY_Absolute op)    { return word_argument(op.opcode, op.address); },

        [](DEC_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](DEC_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](DEC_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](DEC_AbsoluteX op)   { return word_argument(op.opcode, op.address); },

        [](DEX op)             { return no_argument(op.opcode); },
        [](DEY op)             { return no_argument(op.opcode); },

        [](EOR_Immediate op)   { return byte_argument(op.opcode, op.value); },
        [](EOR_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](EOR_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](EOR_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](EOR_AbsoluteX op)   { return word_argument(op.opcode, op.address); },
        [](EOR_AbsoluteY op)   { return word_argument(op.opcode, op.address); },
        [](EOR_IndirectX op)   { return byte_argument(op.opcode, op.address); },
        [](EOR_IndirectY op)   { return byte_argument(op.opcode, op.address); },

        [](INC_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](INC_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](INC_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](INC_AbsoluteX op)   { return word_argument(op.opcode, op.address); },

        [](INX op)             { return no_argument(op.opcode); },
        [](INY op)             { return no_argument(op.opcode); },

        [](JMP_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](JMP_Indirect op)    { return word_argument(op.opcode, op.address); },

        [](JSR op)             { return word_argument(op.opcode, op.address); },

        [](LDA_Immediate op)   { return byte_argument(op.opcode, op.value); },
        [](LDA_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](LDA_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](LDA_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](LDA_AbsoluteX op)   { return word_argument(op.opcode, op.address); },
        [](LDA_AbsoluteY op)   { return word_argument(op.opcode, op.address); },
        [](LDA_IndirectX op)   { return byte_argument(op.opcode, op.address); },
        [](LDA_IndirectY op)   { return byte_argument(op.opcode, op.address); },

        [](LDX_Immediate op)   { return byte_argument(op.opcode, op.value); },
        [](LDX_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](LDX_ZeroPageY op)   { return byte_argument(op.opcode, op.address); },
        [](LDX_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](LDX_AbsoluteY op)   { return word_argument(op.opcode, op.address); },

        [](LDY_Immediate op)   { return byte_argument(op.opcode, op.value); },
        [](LDY_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](LDY_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](LDY_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](LDY_AbsoluteX op)   { return word_argument(op.opcode, op.address); },

        [](LSR_Accumulator op) { return no_argument(op.opcode); },
        [](LSR_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](LSR_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](LSR_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](LSR_AbsoluteX op)   { return word_argument(op.opcode, op.address); },

        [](NOP op)             { return no_argument(op.opcode); },

        [](ORA_Immediate op)   { return byte_argument(op.opcode, op.value); },
        [](ORA_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](ORA_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](ORA_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](ORA_AbsoluteX op)   { return word_argument(op.opcode, op.address); },
        [](ORA_AbsoluteY op)   { return word_argument(op.opcode, op.address); },
        [](ORA_IndirectX op)   { return byte_argument(op.opcode, op.address); },
        [](ORA_IndirectY op)   { return byte_argument(op.opcode, op.address); },

        [](PHA op)             { return no_argument(op.opcode); },
        [](PHP op)             { return no_argument(op.opcode); },

        [](PLA op)             { return no_argument(op.opcode); },
        [](PLP op)             { return no_argument(op.opcode); },

        [](ROL_Accumulator op) { return no_argument(op.opcode); },
        [](ROL_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](ROL_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](ROL_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](ROL_AbsoluteX op)   { return word_argument(op.opcode, op.address); },

        [](ROR_Accumulator op) { return no_argument(op.opcode); },
        [](ROR_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](ROR_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](ROR_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](ROR_AbsoluteX op)   { return word_argument(op.opcode, op.address); },

        [](RTI op)             { return no_argument(op.opcode); },
        [](RTS op)             { return no_argument(op.opcode); },

        [](SBC_Immediate op)   { return byte_argument(op.opcode, op.value); },
        [](SBC_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](SBC_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](SBC_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](SBC_AbsoluteX op)   { return word_argument(op.opcode, op.address); },
        [](SBC_AbsoluteY op)   { return word_argument(op.opcode, op.address); },
        [](SBC_IndirectX op)   { return byte_argument(op.opcode, op.address); },
        [](SBC_IndirectY op)   { return byte_argument(op.opcode, op.address); },

        [](SEC op)             { return no_argument(op.opcode); },
        [](SED op)             { return no_argument(op.opcode); },
        [](SEI op)             { return no_argument(op.opcode); },

        [](STA_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](STA_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](STA_Absolute op)    { return word_argument(op.opcode, op.address); },
        [](STA_AbsoluteX op)   { return word_argument(op.opcode, op.address); },
        [](STA_AbsoluteY op)   { return word_argument(op.opcode, op.address); },
        [](STA_IndirectX op)   { return byte_argument(op.opcode, op.address); },
        [](STA_IndirectY op)   { return byte_argument(op.opcode, op.address); },

        [](STX_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](STX_ZeroPageY op)   { return byte_argument(op.opcode, op.address); },
        [](STX_Absolute op)    { return word_argument(op.opcode, op.address); },

        [](STY_ZeroPage op)    { return byte_argument(op.opcode, op.address); },
        [](STY_ZeroPageX op)   { return byte_argument(op.opcode, op.address); },
        [](STY_Absolute op)    { return word_argument(op.opcode, op.address); },

        [](TAX op)             { return no_argument(op.opcode); },
        [](TAY op)             { return no_argument(op.opcode); },
        [](TSX op)             { return no_argument(op.opcode); },
        [](TXA op)             { return no_argument(op.opcode); },
        [](TXS op)             { return no_argument(op.opcode); },
        [](TYA op)             { return no_argument(op.opcode); }
    },
          operation);
}

