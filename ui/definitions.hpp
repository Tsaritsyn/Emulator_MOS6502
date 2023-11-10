//
// Created by Mikhail on 23/10/2023.
//

#ifndef EMULATOR_MOS6502_DEFINITIONS_HPP
#define EMULATOR_MOS6502_DEFINITIONS_HPP

#include "MOS6502_definitions.hpp"

using namespace Emulator;

constexpr std::array allOpcodes {
        ADC_IMMEDIATE,
        ADC_ZERO_PAGE,
        ADC_ZERO_PAGE_X,
        ADC_ABSOLUTE,
        ADC_ABSOLUTE_X,
        ADC_ABSOLUTE_Y,
        ADC_INDIRECT_X,
        ADC_INDIRECT_Y,

        AND_IMMEDIATE,
        AND_ZERO_PAGE,
        AND_ZERO_PAGE_X,
        AND_ABSOLUTE,
        AND_ABSOLUTE_X,
        AND_ABSOLUTE_Y,
        AND_INDIRECT_X,
        AND_INDIRECT_Y,

        ASL_ACCUMULATOR,
        ASL_ZERO_PAGE,
        ASL_ZERO_PAGE_X,
        ASL_ABSOLUTE,
        ASL_ABSOLUTE_X,

        BCC_RELATIVE,
        BCS_RELATIVE,
        BEQ_RELATIVE,
        BMI_RELATIVE,
        BNE_RELATIVE,
        BPL_RELATIVE,
        BVC_RELATIVE,
        BVS_RELATIVE,

        BIT_ZERO_PAGE,
        BIT_ABSOLUTE,

        BRK_IMPLICIT,

        CLC_IMPLICIT,
        CLD_IMPLICIT,
        CLI_IMPLICIT,
        CLV_IMPLICIT,

        CMP_IMMEDIATE  ,
        CMP_ZERO_PAGE  ,
        CMP_ZERO_PAGE_X,
        CMP_ABSOLUTE   ,
        CMP_ABSOLUTE_X ,
        CMP_ABSOLUTE_Y ,
        CMP_INDIRECT_X ,
        CMP_INDIRECT_Y ,

        CPX_IMMEDIATE,
        CPX_ZERO_PAGE,
        CPX_ABSOLUTE ,

        CPY_IMMEDIATE,
        CPY_ZERO_PAGE,
        CPY_ABSOLUTE ,

        DEC_ZERO_PAGE  ,
        DEC_ZERO_PAGE_X,
        DEC_ABSOLUTE   ,
        DEC_ABSOLUTE_X ,

        DEX_IMPLICIT,
        DEY_IMPLICIT,

        EOR_IMMEDIATE  ,
        EOR_ZERO_PAGE  ,
        EOR_ZERO_PAGE_X,
        EOR_ABSOLUTE   ,
        EOR_ABSOLUTE_X ,
        EOR_ABSOLUTE_Y ,
        EOR_INDIRECT_X ,
        EOR_INDIRECT_Y ,

        INC_ZERO_PAGE  ,
        INC_ZERO_PAGE_X,
        INC_ABSOLUTE   ,
        INC_ABSOLUTE_X ,

        INX_IMPLICIT,
        INY_IMPLICIT,

        JMP_ABSOLUTE,
        JMP_INDIRECT,

        JSR_ABSOLUTE,

        LDA_IMMEDIATE  ,
        LDA_ZERO_PAGE  ,
        LDA_ZERO_PAGE_X,
        LDA_ABSOLUTE   ,
        LDA_ABSOLUTE_X ,
        LDA_ABSOLUTE_Y ,
        LDA_INDIRECT_X ,
        LDA_INDIRECT_Y ,

        LDX_IMMEDIATE  ,
        LDX_ZERO_PAGE  ,
        LDX_ZERO_PAGE_Y,
        LDX_ABSOLUTE   ,
        LDX_ABSOLUTE_Y ,

        LDY_IMMEDIATE  ,
        LDY_ZERO_PAGE  ,
        LDY_ZERO_PAGE_X,
        LDY_ABSOLUTE   ,
        LDY_ABSOLUTE_X ,

        LSR_ACCUMULATOR,
        LSR_ZERO_PAGE  ,
        LSR_ZERO_PAGE_X,
        LSR_ABSOLUTE   ,
        LSR_ABSOLUTE_X ,

        NOP_IMPLICIT,

        ORA_IMMEDIATE  ,
        ORA_ZERO_PAGE  ,
        ORA_ZERO_PAGE_X,
        ORA_ABSOLUTE   ,
        ORA_ABSOLUTE_X ,
        ORA_ABSOLUTE_Y ,
        ORA_INDIRECT_X ,
        ORA_INDIRECT_Y ,

        PHA_IMPLICIT,
        PHP_IMPLICIT,

        PLA_IMPLICIT,
        PLP_IMPLICIT,

        ROL_ACCUMULATOR,
        ROL_ZERO_PAGE  ,
        ROL_ZERO_PAGE_X,
        ROL_ABSOLUTE   ,
        ROL_ABSOLUTE_X ,

        ROR_ACCUMULATOR,
        ROR_ZERO_PAGE  ,
        ROR_ZERO_PAGE_X,
        ROR_ABSOLUTE   ,
        ROR_ABSOLUTE_X ,

        RTI_IMPLICIT,
        RTS_IMPLICIT,

        SBC_IMMEDIATE  ,
        SBC_ZERO_PAGE  ,
        SBC_ZERO_PAGE_X,
        SBC_ABSOLUTE   ,
        SBC_ABSOLUTE_X ,
        SBC_ABSOLUTE_Y ,
        SBC_INDIRECT_X ,
        SBC_INDIRECT_Y ,

        SEC_IMPLICIT,
        SED_IMPLICIT,
        SEI_IMPLICIT,

        STA_ZERO_PAGE  ,
        STA_ZERO_PAGE_X,
        STA_ABSOLUTE   ,
        STA_ABSOLUTE_X ,
        STA_ABSOLUTE_Y ,
        STA_INDIRECT_X ,
        STA_INDIRECT_Y ,

        STX_ZERO_PAGE  ,
        STX_ZERO_PAGE_Y,
        STX_ABSOLUTE   ,

        STY_ZERO_PAGE  ,
        STY_ZERO_PAGE_X,
        STY_ABSOLUTE   ,

        TAX_IMPLICIT,
        TAY_IMPLICIT,
        TSX_IMPLICIT,
        TXA_IMPLICIT,
        TXS_IMPLICIT,
        TYA_IMPLICIT
};

#endif //EMULATOR_MOS6502_DEFINITIONS_HPP
