//
// Created by Mikhail on 14/10/2023.
//

#ifndef EMULATOR_MOS6502_OPERATION_HPP
#define EMULATOR_MOS6502_OPERATION_HPP

#include <vector>
#include <expected>

#include "MOS6502_definitions.hpp"
#include "Error.hpp"
#include "ROM.hpp"


template<typename ... Ts>
struct Overload : Ts ... {
    using Ts::operator() ...;
};
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;


namespace Emulator {

    struct ADC_Immediate   { constexpr static OpCode opcode = ADC_IMMEDIATE;   Byte value; };
    struct ADC_ZeroPage    { constexpr static OpCode opcode = ADC_ZERO_PAGE;   Byte address; };
    struct ADC_ZeroPageX   { constexpr static OpCode opcode = ADC_ZERO_PAGE_X; Byte address; };
    struct ADC_Absolute    { constexpr static OpCode opcode = ADC_ABSOLUTE;    Word address; };
    struct ADC_AbsoluteX   { constexpr static OpCode opcode = ADC_ABSOLUTE_X;  Word address; };
    struct ADC_AbsoluteY   { constexpr static OpCode opcode = ADC_ABSOLUTE_Y;  Word address; };
    struct ADC_IndirectX   { constexpr static OpCode opcode = ADC_INDIRECT_X;  Byte address; };
    struct ADC_IndirectY   { constexpr static OpCode opcode = ADC_INDIRECT_Y;  Byte address; };

    struct AND_Immediate   { constexpr static OpCode opcode = AND_IMMEDIATE;   Byte value; };
    struct AND_ZeroPage    { constexpr static OpCode opcode = AND_ZERO_PAGE;   Byte address; };
    struct AND_ZeroPageX   { constexpr static OpCode opcode = AND_ZERO_PAGE_X; Byte address; };
    struct AND_Absolute    { constexpr static OpCode opcode = AND_ABSOLUTE;    Word address; };
    struct AND_AbsoluteX   { constexpr static OpCode opcode = AND_ABSOLUTE_X;  Word address; };
    struct AND_AbsoluteY   { constexpr static OpCode opcode = AND_ABSOLUTE_Y;  Word address; };
    struct AND_IndirectX   { constexpr static OpCode opcode = AND_INDIRECT_X;  Byte address; };
    struct AND_IndirectY   { constexpr static OpCode opcode = AND_INDIRECT_Y;  Byte address; };

    struct ASL_Accumulator { constexpr static OpCode opcode = ASL_ACCUMULATOR; };
    struct ASL_ZeroPage    { constexpr static OpCode opcode = ASL_ZERO_PAGE;   Byte address; };
    struct ASL_ZeroPageX   { constexpr static OpCode opcode = ASL_ZERO_PAGE_X; Byte address; };
    struct ASL_Absolute    { constexpr static OpCode opcode = ASL_ABSOLUTE;    Word address; };
    struct ASL_AbsoluteX   { constexpr static OpCode opcode = ASL_ABSOLUTE_X;  Word address; };

    struct BCC             { constexpr static OpCode opcode = BCC_RELATIVE;    char offset; };
    struct BCS             { constexpr static OpCode opcode = BCS_RELATIVE;    char offset; };
    struct BEQ             { constexpr static OpCode opcode = BEQ_RELATIVE;    char offset; };
    struct BNE             { constexpr static OpCode opcode = BNE_RELATIVE;    char offset; };
    struct BMI             { constexpr static OpCode opcode = BMI_RELATIVE;    char offset; };
    struct BPL             { constexpr static OpCode opcode = BPL_RELATIVE;    char offset; };
    struct BVC             { constexpr static OpCode opcode = BVC_RELATIVE;    char offset; };
    struct BVS             { constexpr static OpCode opcode = BVS_RELATIVE;    char offset; };

    struct BIT_ZeroPage    { constexpr static OpCode opcode = BIT_ZERO_PAGE;   Byte address; };
    struct BIT_Absolute    { constexpr static OpCode opcode = BIT_ABSOLUTE;    Word address; };

    struct BRK             { constexpr static OpCode opcode = BRK_IMPLICIT; };

    struct CLC             { constexpr static OpCode opcode = CLC_IMPLICIT; };
    struct CLD             { constexpr static OpCode opcode = CLD_IMPLICIT; };
    struct CLI             { constexpr static OpCode opcode = CLI_IMPLICIT; };
    struct CLV             { constexpr static OpCode opcode = CLV_IMPLICIT; };

    struct CMP_Immediate   { constexpr static OpCode opcode = CMP_IMMEDIATE;   Byte value; };
    struct CMP_ZeroPage    { constexpr static OpCode opcode = CMP_ZERO_PAGE;   Byte address; };
    struct CMP_ZeroPageX   { constexpr static OpCode opcode = CMP_ZERO_PAGE_X; Byte address; };
    struct CMP_Absolute    { constexpr static OpCode opcode = CMP_ABSOLUTE;    Word address; };
    struct CMP_AbsoluteX   { constexpr static OpCode opcode = CMP_ABSOLUTE_X;  Word address; };
    struct CMP_AbsoluteY   { constexpr static OpCode opcode = CMP_ABSOLUTE_Y;  Word address; };
    struct CMP_IndirectX   { constexpr static OpCode opcode = CMP_INDIRECT_X;  Byte address; };
    struct CMP_IndirectY   { constexpr static OpCode opcode = CMP_INDIRECT_Y;  Byte address; };

    struct CPX_Immediate   { constexpr static OpCode opcode = CPX_IMMEDIATE;   Byte value; };
    struct CPX_ZeroPage    { constexpr static OpCode opcode = CPX_ZERO_PAGE;   Byte address; };
    struct CPX_Absolute    { constexpr static OpCode opcode = CPX_ABSOLUTE;    Word address; };

    struct CPY_Immediate   { constexpr static OpCode opcode = CPY_IMMEDIATE;   Byte value; };
    struct CPY_ZeroPage    { constexpr static OpCode opcode = CPY_ZERO_PAGE;   Byte address; };
    struct CPY_Absolute    { constexpr static OpCode opcode = CPY_ABSOLUTE;    Word address; };

    struct DEC_ZeroPage    { constexpr static OpCode opcode = DEC_ZERO_PAGE;   Byte address; };
    struct DEC_ZeroPageX   { constexpr static OpCode opcode = DEC_ZERO_PAGE_X; Byte address; };
    struct DEC_Absolute    { constexpr static OpCode opcode = DEC_ABSOLUTE;    Word address; };
    struct DEC_AbsoluteX   { constexpr static OpCode opcode = DEC_ABSOLUTE_X;  Word address; };

    struct DEX             { constexpr static OpCode opcode = DEX_IMPLICIT; };
    struct DEY             { constexpr static OpCode opcode = DEY_IMPLICIT; };

    struct EOR_Immediate   { constexpr static OpCode opcode = EOR_IMMEDIATE;   Byte value; };
    struct EOR_ZeroPage    { constexpr static OpCode opcode = EOR_ZERO_PAGE;   Byte address; };
    struct EOR_ZeroPageX   { constexpr static OpCode opcode = EOR_ZERO_PAGE_X; Byte address; };
    struct EOR_Absolute    { constexpr static OpCode opcode = EOR_ABSOLUTE;    Word address; };
    struct EOR_AbsoluteX   { constexpr static OpCode opcode = EOR_ABSOLUTE_X;  Word address; };
    struct EOR_AbsoluteY   { constexpr static OpCode opcode = EOR_ABSOLUTE_Y;  Word address; };
    struct EOR_IndirectX   { constexpr static OpCode opcode = EOR_INDIRECT_X;  Byte address; };
    struct EOR_IndirectY   { constexpr static OpCode opcode = EOR_INDIRECT_Y;  Byte address; };

    struct INC_ZeroPage    { constexpr static OpCode opcode = INC_ZERO_PAGE;   Byte address; };
    struct INC_ZeroPageX   { constexpr static OpCode opcode = INC_ZERO_PAGE_X; Byte address; };
    struct INC_Absolute    { constexpr static OpCode opcode = INC_ABSOLUTE;    Word address; };
    struct INC_AbsoluteX   { constexpr static OpCode opcode = INC_ABSOLUTE_X;  Word address; };

    struct INX             { constexpr static OpCode opcode = INX_IMPLICIT; };
    struct INY             { constexpr static OpCode opcode = INY_IMPLICIT; };

    struct JMP_Absolute    { constexpr static OpCode opcode = JMP_ABSOLUTE;    Word address; };
    struct JMP_Indirect    { constexpr static OpCode opcode = JMP_INDIRECT;    Word address; };

    struct JSR             { constexpr static OpCode opcode = JSR_ABSOLUTE;    Word address; };

    struct LDA_Immediate   { constexpr static OpCode opcode = LDA_IMMEDIATE;   Byte value; };
    struct LDA_ZeroPage    { constexpr static OpCode opcode = LDA_ZERO_PAGE;   Byte address; };
    struct LDA_ZeroPageX   { constexpr static OpCode opcode = LDA_ZERO_PAGE_X; Byte address; };
    struct LDA_Absolute    { constexpr static OpCode opcode = LDA_ABSOLUTE;    Word address; };
    struct LDA_AbsoluteX   { constexpr static OpCode opcode = LDA_ABSOLUTE_X;  Word address; };
    struct LDA_AbsoluteY   { constexpr static OpCode opcode = LDA_ABSOLUTE_Y;  Word address; };
    struct LDA_IndirectX   { constexpr static OpCode opcode = LDA_INDIRECT_X;  Byte address; };
    struct LDA_IndirectY   { constexpr static OpCode opcode = LDA_INDIRECT_Y;  Byte address; };

    struct LDX_Immediate   { constexpr static OpCode opcode = LDX_IMMEDIATE;   Byte value; };
    struct LDX_ZeroPage    { constexpr static OpCode opcode = LDX_ZERO_PAGE;   Byte address; };
    struct LDX_ZeroPageY   { constexpr static OpCode opcode = LDX_ZERO_PAGE_Y; Byte address; };
    struct LDX_Absolute    { constexpr static OpCode opcode = LDX_ABSOLUTE;    Word address; };
    struct LDX_AbsoluteY   { constexpr static OpCode opcode = LDX_ABSOLUTE_Y;  Word address; };

    struct LDY_Immediate   { constexpr static OpCode opcode = LDY_IMMEDIATE;   Byte value; };
    struct LDY_ZeroPage    { constexpr static OpCode opcode = LDY_ZERO_PAGE;   Byte address; };
    struct LDY_ZeroPageX   { constexpr static OpCode opcode = LDY_ZERO_PAGE_X; Byte address; };
    struct LDY_Absolute    { constexpr static OpCode opcode = LDY_ABSOLUTE;    Word address; };
    struct LDY_AbsoluteX   { constexpr static OpCode opcode = LDY_ABSOLUTE_X;  Word address; };

    struct LSR_Accumulator { constexpr static OpCode opcode = LSR_ACCUMULATOR; };
    struct LSR_ZeroPage    { constexpr static OpCode opcode = LSR_ZERO_PAGE;   Byte address; };
    struct LSR_ZeroPageX   { constexpr static OpCode opcode = LSR_ZERO_PAGE_X; Byte address; };
    struct LSR_Absolute    { constexpr static OpCode opcode = LSR_ABSOLUTE;    Word address; };
    struct LSR_AbsoluteX   { constexpr static OpCode opcode = LSR_ABSOLUTE_X;  Word address; };

    struct NOP             { constexpr static OpCode opcode = NOP_IMPLICIT; };

    struct ORA_Immediate   { constexpr static OpCode opcode = ORA_IMMEDIATE;   Byte value; };
    struct ORA_ZeroPage    { constexpr static OpCode opcode = ORA_ZERO_PAGE;   Byte address; };
    struct ORA_ZeroPageX   { constexpr static OpCode opcode = ORA_ZERO_PAGE_X; Byte address; };
    struct ORA_Absolute    { constexpr static OpCode opcode = ORA_ABSOLUTE;    Word address; };
    struct ORA_AbsoluteX   { constexpr static OpCode opcode = ORA_ABSOLUTE_X;  Word address; };
    struct ORA_AbsoluteY   { constexpr static OpCode opcode = ORA_ABSOLUTE_Y;  Word address; };
    struct ORA_IndirectX   { constexpr static OpCode opcode = ORA_INDIRECT_X;  Byte address; };
    struct ORA_IndirectY   { constexpr static OpCode opcode = ORA_INDIRECT_Y;  Byte address; };

    struct PHA             { constexpr static OpCode opcode = PHA_IMPLICIT; };
    struct PHP             { constexpr static OpCode opcode = PHP_IMPLICIT; };

    struct PLA             { constexpr static OpCode opcode = PLA_IMPLICIT; };
    struct PLP             { constexpr static OpCode opcode = PLP_IMPLICIT; };

    struct ROL_Accumulator { constexpr static OpCode opcode = ROL_ACCUMULATOR; };
    struct ROL_ZeroPage    { constexpr static OpCode opcode = ROL_ZERO_PAGE;   Byte address; };
    struct ROL_ZeroPageX   { constexpr static OpCode opcode = ROL_ZERO_PAGE_X; Byte address; };
    struct ROL_Absolute    { constexpr static OpCode opcode = ROL_ABSOLUTE;    Word address; };
    struct ROL_AbsoluteX   { constexpr static OpCode opcode = ROL_ABSOLUTE_X;  Word address; };

    struct ROR_Accumulator { constexpr static OpCode opcode = ROR_ACCUMULATOR; };
    struct ROR_ZeroPage    { constexpr static OpCode opcode = ROR_ZERO_PAGE;   Byte address; };
    struct ROR_ZeroPageX   { constexpr static OpCode opcode = ROR_ZERO_PAGE_X; Byte address; };
    struct ROR_Absolute    { constexpr static OpCode opcode = ROR_ABSOLUTE;    Word address; };
    struct ROR_AbsoluteX   { constexpr static OpCode opcode = ROR_ABSOLUTE_X;  Word address; };

    struct RTI             { constexpr static OpCode opcode = RTI_IMPLICIT; };
    struct RTS             { constexpr static OpCode opcode = RTS_IMPLICIT; };

    struct SBC_Immediate   { constexpr static OpCode opcode = SBC_IMMEDIATE;   Byte value; };
    struct SBC_ZeroPage    { constexpr static OpCode opcode = SBC_ZERO_PAGE;   Byte address; };
    struct SBC_ZeroPageX   { constexpr static OpCode opcode = SBC_ZERO_PAGE_X; Byte address; };
    struct SBC_Absolute    { constexpr static OpCode opcode = SBC_ABSOLUTE;    Word address; };
    struct SBC_AbsoluteX   { constexpr static OpCode opcode = SBC_ABSOLUTE_X;  Word address; };
    struct SBC_AbsoluteY   { constexpr static OpCode opcode = SBC_ABSOLUTE_Y;  Word address; };
    struct SBC_IndirectX   { constexpr static OpCode opcode = SBC_INDIRECT_X;  Byte address; };
    struct SBC_IndirectY   { constexpr static OpCode opcode = SBC_INDIRECT_Y;  Byte address; };

    struct SEC             { constexpr static OpCode opcode = SEC_IMPLICIT; };
    struct SED             { constexpr static OpCode opcode = SED_IMPLICIT; };
    struct SEI             { constexpr static OpCode opcode = SEI_IMPLICIT; };

    struct STA_ZeroPage    { constexpr static OpCode opcode = STA_ZERO_PAGE;   Byte address; };
    struct STA_ZeroPageX   { constexpr static OpCode opcode = STA_ZERO_PAGE_X; Byte address; };
    struct STA_Absolute    { constexpr static OpCode opcode = STA_ABSOLUTE;    Word address; };
    struct STA_AbsoluteX   { constexpr static OpCode opcode = STA_ABSOLUTE_X;  Word address; };
    struct STA_AbsoluteY   { constexpr static OpCode opcode = STA_ABSOLUTE_Y;  Word address; };
    struct STA_IndirectX   { constexpr static OpCode opcode = STA_INDIRECT_X;  Byte address; };
    struct STA_IndirectY   { constexpr static OpCode opcode = STA_INDIRECT_Y;  Byte address; };

    struct STX_ZeroPage    { constexpr static OpCode opcode = STX_ZERO_PAGE;   Byte address; };
    struct STX_ZeroPageY   { constexpr static OpCode opcode = STX_ZERO_PAGE_Y; Byte address; };
    struct STX_Absolute    { constexpr static OpCode opcode = STX_ABSOLUTE;    Word address; };

    struct STY_ZeroPage    { constexpr static OpCode opcode = STY_ZERO_PAGE;   Byte address; };
    struct STY_ZeroPageX   { constexpr static OpCode opcode = STY_ZERO_PAGE_X; Byte address; };
    struct STY_Absolute    { constexpr static OpCode opcode = STY_ABSOLUTE;    Word address; };

    struct TAX             { constexpr static OpCode opcode = TAX_IMPLICIT; };
    struct TAY             { constexpr static OpCode opcode = TAY_IMPLICIT; };
    struct TSX             { constexpr static OpCode opcode = TSX_IMPLICIT; };
    struct TXA             { constexpr static OpCode opcode = TXA_IMPLICIT; };
    struct TXS             { constexpr static OpCode opcode = TXS_IMPLICIT; };
    struct TYA             { constexpr static OpCode opcode = TYA_IMPLICIT; };




    using Operation = std::variant<
            ADC_Immediate,
            ADC_ZeroPage,
            ADC_ZeroPageX,
            ADC_Absolute,
            ADC_AbsoluteX,
            ADC_AbsoluteY,
            ADC_IndirectX,
            ADC_IndirectY,

            AND_Immediate,
            AND_ZeroPage,
            AND_ZeroPageX,
            AND_Absolute,
            AND_AbsoluteX,
            AND_AbsoluteY,
            AND_IndirectX,
            AND_IndirectY,

            ASL_Accumulator,
            ASL_ZeroPage,
            ASL_ZeroPageX,
            ASL_Absolute,
            ASL_AbsoluteX,

            BCC,
            BCS,
            BEQ,
            BNE,
            BMI,
            BPL,
            BVC,
            BVS,

            BIT_ZeroPage,
            BIT_Absolute,

            BRK,

            CLC,
            CLD,
            CLI,
            CLV,

            CMP_Immediate,
            CMP_ZeroPage,
            CMP_ZeroPageX,
            CMP_Absolute,
            CMP_AbsoluteX,
            CMP_AbsoluteY,
            CMP_IndirectX,
            CMP_IndirectY,

            CPX_Immediate,
            CPX_ZeroPage,
            CPX_Absolute,

            CPY_Immediate,
            CPY_ZeroPage,
            CPY_Absolute,

            DEC_ZeroPage,
            DEC_ZeroPageX,
            DEC_Absolute,
            DEC_AbsoluteX,

            DEX,
            DEY,

            EOR_Immediate,
            EOR_ZeroPage,
            EOR_ZeroPageX,
            EOR_Absolute,
            EOR_AbsoluteX,
            EOR_AbsoluteY,
            EOR_IndirectX,
            EOR_IndirectY,

            INC_ZeroPage,
            INC_ZeroPageX,
            INC_Absolute,
            INC_AbsoluteX,

            INX,
            INY,

            JMP_Absolute,
            JMP_Indirect,

            JSR,

            LDA_Immediate,
            LDA_ZeroPage,
            LDA_ZeroPageX,
            LDA_Absolute,
            LDA_AbsoluteX,
            LDA_AbsoluteY,
            LDA_IndirectX,
            LDA_IndirectY,

            LDX_Immediate,
            LDX_ZeroPage,
            LDX_ZeroPageY,
            LDX_Absolute,
            LDX_AbsoluteY,

            LDY_Immediate,
            LDY_ZeroPage,
            LDY_ZeroPageX,
            LDY_Absolute,
            LDY_AbsoluteX,

            LSR_Accumulator,
            LSR_ZeroPage,
            LSR_ZeroPageX,
            LSR_Absolute,
            LSR_AbsoluteX,

            NOP,

            ORA_Immediate,
            ORA_ZeroPage,
            ORA_ZeroPageX,
            ORA_Absolute,
            ORA_AbsoluteX,
            ORA_AbsoluteY,
            ORA_IndirectX,
            ORA_IndirectY,

            PHA,
            PHP,

            PLA,
            PLP,

            ROL_Accumulator,
            ROL_ZeroPage,
            ROL_ZeroPageX,
            ROL_Absolute,
            ROL_AbsoluteX,

            ROR_Accumulator,
            ROR_ZeroPage,
            ROR_ZeroPageX,
            ROR_Absolute,
            ROR_AbsoluteX,

            RTI,
            RTS,

            SBC_Immediate,
            SBC_ZeroPage,
            SBC_ZeroPageX,
            SBC_Absolute,
            SBC_AbsoluteX,
            SBC_AbsoluteY,
            SBC_IndirectX,
            SBC_IndirectY,

            SEC,
            SED,
            SEI,

            STA_ZeroPage,
            STA_ZeroPageX,
            STA_Absolute,
            STA_AbsoluteX,
            STA_AbsoluteY,
            STA_IndirectX,
            STA_IndirectY,

            STX_ZeroPage,
            STX_ZeroPageY,
            STX_Absolute,

            STY_ZeroPage,
            STY_ZeroPageX,
            STY_Absolute,

            TAX,
            TAY,
            TSX,
            TXA,
            TXS,
            TYA
            >;



    std::string description(const Operation &operation) noexcept;

    std::vector<Byte> encode(const Operation &operation) noexcept;

    /// PC and cycle are inout parameters
    std::expected<Operation, InvalidOperation> decode(const ROM &memory, Word &PC, size_t &cycle) noexcept;
}

#endif //EMULATOR_MOS6502_OPERATION_HPP
