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

    struct ADC_Immediate { Byte value; };
    struct ADC_ZeroPage { Byte address; };
    struct ADC_ZeroPageX { Byte address; };
    struct ADC_Absolute { Word address; };
    struct ADC_AbsoluteX { Word address; };
    struct ADC_AbsoluteY { Word address; };
    struct ADC_IndirectX { Byte address; };
    struct ADC_IndirectY { Byte address; };

    struct AND_Immediate { Byte value; };
    struct AND_ZeroPage { Byte address; };
    struct AND_ZeroPageX { Byte address; };
    struct AND_Absolute { Word address; };
    struct AND_AbsoluteX { Word address; };
    struct AND_AbsoluteY { Word address; };
    struct AND_IndirectX { Byte address; };
    struct AND_IndirectY { Byte address; };

    struct ASL_Accumulator {};
    struct ASL_ZeroPage { Byte address; };
    struct ASL_ZeroPageX { Byte address; };
    struct ASL_Absolute { Word address; };
    struct ASL_AbsoluteX { Word address; };

    struct BCC { char offset; };
    struct BCS { char offset; };
    struct BEQ { char offset; };
    struct BNE { char offset; };
    struct BMI { char offset; };
    struct BPL { char offset; };
    struct BVC { char offset; };
    struct BVS { char offset; };

    struct BIT_ZeroPage { Byte address; };
    struct BIT_Absolute { Word address; };

    struct BRK {};

    struct CLC {};
    struct CLD {};
    struct CLI {};
    struct CLV {};

    struct CMP_Immediate { Byte value; };
    struct CMP_ZeroPage { Byte address; };
    struct CMP_ZeroPageX { Byte address; };
    struct CMP_Absolute { Word address; };
    struct CMP_AbsoluteX { Word address; };
    struct CMP_AbsoluteY { Word address; };
    struct CMP_IndirectX { Byte address; };
    struct CMP_IndirectY { Byte address; };

    struct CPX_Immediate { Byte value; };
    struct CPX_ZeroPage { Byte address; };
    struct CPX_Absolute { Word address; };

    struct CPY_Immediate { Byte value; };
    struct CPY_ZeroPage { Byte address; };
    struct CPY_Absolute { Word address; };

    struct DEC_ZeroPage { Byte address; };
    struct DEC_ZeroPageX { Byte address; };
    struct DEC_Absolute { Word address; };
    struct DEC_AbsoluteX { Word address; };

    struct DEX {};
    struct DEY {};

    struct EOR_Immediate { Byte value; };
    struct EOR_ZeroPage { Byte address; };
    struct EOR_ZeroPageX { Byte address; };
    struct EOR_Absolute { Word address; };
    struct EOR_AbsoluteX { Word address; };
    struct EOR_AbsoluteY { Word address; };
    struct EOR_IndirectX { Byte address; };
    struct EOR_IndirectY { Byte address; };

    struct INC_ZeroPage { Byte address; };
    struct INC_ZeroPageX { Byte address; };
    struct INC_Absolute { Word address; };
    struct INC_AbsoluteX { Word address; };

    struct INX {};
    struct INY {};

    struct JMP_Absolute { Word address; };
    struct JMP_Indirect { Word address; };

    struct JSR { Word address; };

    struct LDA_Immediate { Byte value; };
    struct LDA_ZeroPage { Byte address; };
    struct LDA_ZeroPageX { Byte address; };
    struct LDA_Absolute { Word address; };
    struct LDA_AbsoluteX { Word address; };
    struct LDA_AbsoluteY { Word address; };
    struct LDA_IndirectX { Byte address; };
    struct LDA_IndirectY { Byte address; };

    struct LDX_Immediate { Byte value; };
    struct LDX_ZeroPage { Byte address; };
    struct LDX_ZeroPageY { Byte address; };
    struct LDX_Absolute { Word address; };
    struct LDX_AbsoluteY { Word address; };

    struct LDY_Immediate { Byte value; };
    struct LDY_ZeroPage { Byte address; };
    struct LDY_ZeroPageX { Byte address; };
    struct LDY_Absolute { Word address; };
    struct LDY_AbsoluteX { Word address; };

    struct LSR_Accumulator {};
    struct LSR_ZeroPage { Byte address; };
    struct LSR_ZeroPageX { Byte address; };
    struct LSR_Absolute { Word address; };
    struct LSR_AbsoluteX { Word address; };

    struct NOP {};

    struct ORA_Immediate { Byte value; };
    struct ORA_ZeroPage { Byte address; };
    struct ORA_ZeroPageX { Byte address; };
    struct ORA_Absolute { Word address; };
    struct ORA_AbsoluteX { Word address; };
    struct ORA_AbsoluteY { Word address; };
    struct ORA_IndirectX { Byte address; };
    struct ORA_IndirectY { Byte address; };

    struct PHA {};
    struct PHP {};

    struct PLA {};
    struct PLP {};

    struct ROL_Accumulator {};
    struct ROL_ZeroPage { Byte address; };
    struct ROL_ZeroPageX { Byte address; };
    struct ROL_Absolute { Word address; };
    struct ROL_AbsoluteX { Word address; };

    struct ROR_Accumulator {};
    struct ROR_ZeroPage { Byte address; };
    struct ROR_ZeroPageX { Byte address; };
    struct ROR_Absolute { Word address; };
    struct ROR_AbsoluteX { Word address; };

    struct RTI {};
    struct RTS {};

    struct SBC_Immediate { Byte value; };
    struct SBC_ZeroPage { Byte address; };
    struct SBC_ZeroPageX { Byte address; };
    struct SBC_Absolute { Word address; };
    struct SBC_AbsoluteX { Word address; };
    struct SBC_AbsoluteY { Word address; };
    struct SBC_IndirectX { Byte address; };
    struct SBC_IndirectY { Byte address; };

    struct SEC {};
    struct SED {};
    struct SEI {};

    struct STA_ZeroPage { Byte address; };
    struct STA_ZeroPageX { Byte address; };
    struct STA_Absolute { Word address; };
    struct STA_AbsoluteX { Word address; };
    struct STA_AbsoluteY { Word address; };
    struct STA_IndirectX { Byte address; };
    struct STA_IndirectY { Byte address; };

    struct STX_ZeroPage { Byte address; };
    struct STX_ZeroPageY { Byte address; };
    struct STX_Absolute { Word address; };

    struct STY_ZeroPage { Byte address; };
    struct STY_ZeroPageX { Byte address; };
    struct STY_Absolute { Word address; };

    struct TAX {};
    struct TAY {};
    struct TSX {};
    struct TXA {};
    struct TXS {};
    struct TYA {};




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


    std::string description(const Operation &operation);

    std::vector<Byte> encode(const Operation &operation);


    /// PC and cycle are inout parameters
    std::expected<Operation, InvalidOperation> decode(const ROM &memory, Word &PC, size_t &cycle);
}

#endif //EMULATOR_MOS6502_OPERATION_HPP
