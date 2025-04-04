//
// Created by Mikhail on 30/08/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_DEFINITIONS_HPP
#define EMULATOR_MOS6502_MOS6502_DEFINITIONS_HPP

#include <cstdint>
#include <array>
#include <variant>

// https://stackoverflow.com/questions/4239993/determining-endianness-at-compile-time
#if 'AB' == 0x4142
#define BIG_ENDIAN 1
#else
#define BIG_ENDIAN 0
#endif

namespace Emulator {
    using Byte = uint8_t;
    using Word = uint16_t;


    constexpr bool CLEAR = false;
    constexpr bool SET = true;

    struct WordToBytes {
#if BIG_ENDIAN
        Byte &low = converter.bytes[0];
        Byte &high = converter.bytes[1];
#else
        Byte &low = converter.bytes[1];
        Byte &high = converter.bytes[0];
#endif
        Word &word = converter.word;

        WordToBytes(Word word = 0): converter{.word = word} {};

    private:
        union { Word word; Byte bytes[2]; } converter;
    };


    enum class AddressingMode {

        /**
         * An instruction using zero page addressing mode has only an 8 bit address operand.
         * This limits it to addressing only the first 256 bytes of memory (e.g. $0000 to $00FF)
         *  where the most significant byte of the address is always zero.
         * In zero page mode only the least significant byte of the address is held in the instruction making it
         *  shorter by one byte (important for space saving) and one less memory fetch during execution (important for speed).
         */
        ZERO_PAGE,

        /**
         * The address to be accessed by an instruction using indexed zero page addressing is calculated by taking
         *  the 8 bit zero page address from the instruction and adding the current value of the X register to it.
         * For example if the X register contains $0F and the instruction LDA $80,X is executed then the accumulator
         *  will be loaded from $008F (e.g. $80 + $0F => $8F).
         *
         * NB:
         * The address calculation wraps around if the sum of the base address and the register exceed $FF.
         * If we repeat the last example but with $FF in the X register then the accumulator will be loaded from $007F
         *  (e.g. $80 + $FF => $7F) and not $017F.
         */
        ZERO_PAGE_X,

        /**
         * The address to be accessed by an instruction using indexed zero page addressing is calculated by taking
         *  the 8 bit zero page address from the instruction and adding the current value of the Y register to it.
         * This mode can only be used with the LDX and STX instructions.
         */
        ZERO_PAGE_Y,

        /**
         * Instructions using absolute addressing contain a full 16 bit address to identify the target location.
         */
        ABSOLUTE,

        /**
         * The address to be accessed by an instruction using X register indexed absolute addressing is computed
         *  by taking the 16 bit address from the instruction and added the contents of the X register.
         * For example if X contains $92 then an STA $2000,X instruction will store the accumulator at $2092 (e.g. $2000 + $92).
         */
        ABSOLUTE_X,

        /**
         * The Y register indexed absolute addressing mode is the same as the previous mode only with the contents of
         *  the Y register added to the 16 bit address from the instruction.
         */
        ABSOLUTE_Y,

        /**
         * Indexed indirect addressing is normally used in conjunction with a table of address held on zero page.
         * The address of the table is taken from the instruction and the X register added to it (with zero page wrap around)
         *  to give the location of the least significant byte of the target address.
         */
        INDIRECT_X,

        /**
         * Indirect indexed addressing is the most common indirection mode used on the 6502.
         * In instruction contains the zero page location of the least significant byte of 16 bit address.
         * The Y register is dynamically added to this value to generated the actual target address for operation.
         */
        INDIRECT_Y
    };


    enum OpCode: Byte {
        ADC_IMMEDIATE   = 0x69,
        ADC_ZERO_PAGE   = 0x65,
        ADC_ZERO_PAGE_X = 0x75,
        ADC_ABSOLUTE    = 0x6D,
        ADC_ABSOLUTE_X  = 0x7D,
        ADC_ABSOLUTE_Y  = 0x79,
        ADC_INDIRECT_X  = 0x61,
        ADC_INDIRECT_Y  = 0x71,

        AND_IMMEDIATE   = 0x29,
        AND_ZERO_PAGE   = 0x25,
        AND_ZERO_PAGE_X = 0x35,
        AND_ABSOLUTE    = 0x2D,
        AND_ABSOLUTE_X  = 0x3D,
        AND_ABSOLUTE_Y  = 0x39,
        AND_INDIRECT_X  = 0x21,
        AND_INDIRECT_Y  = 0x31,

        ASL_ACCUMULATOR = 0x0A,
        ASL_ZERO_PAGE   = 0x06,
        ASL_ZERO_PAGE_X = 0x016,
        ASL_ABSOLUTE    = 0x0E,
        ASL_ABSOLUTE_X  = 0x1E,

        BCC_RELATIVE    = 0x90,
        BCS_RELATIVE    = 0xB0,
        BEQ_RELATIVE    = 0xF0,
        BMI_RELATIVE    = 0x30,
        BNE_RELATIVE    = 0xD0,
        BPL_RELATIVE    = 0x10,
        BVC_RELATIVE    = 0x50,
        BVS_RELATIVE    = 0x70,

        BIT_ZERO_PAGE   = 0x24,
        BIT_ABSOLUTE    = 0X2C,

        BRK_IMPLICIT    = 0x00,

        CLC_IMPLICIT    = 0x18,
        CLD_IMPLICIT    = 0xD8,
        CLI_IMPLICIT    = 0x58,
        CLV_IMPLICIT    = 0xB8,

        CMP_IMMEDIATE   = 0xC9,
        CMP_ZERO_PAGE   = 0xC5,
        CMP_ZERO_PAGE_X = 0xD5,
        CMP_ABSOLUTE    = 0xCD,
        CMP_ABSOLUTE_X  = 0xDD,
        CMP_ABSOLUTE_Y  = 0xD9,
        CMP_INDIRECT_X  = 0xC1,
        CMP_INDIRECT_Y  = 0xD1,

        CPX_IMMEDIATE   = 0xE0,
        CPX_ZERO_PAGE   = 0xE4,
        CPX_ABSOLUTE    = 0xEC,

        CPY_IMMEDIATE   = 0xC0,
        CPY_ZERO_PAGE   = 0xC4,
        CPY_ABSOLUTE    = 0xCC,

        DEC_ZERO_PAGE   = 0xC6,
        DEC_ZERO_PAGE_X = 0xD6,
        DEC_ABSOLUTE    = 0xCE,
        DEC_ABSOLUTE_X  = 0xDE,

        DEX_IMPLICIT    = 0xCA,
        DEY_IMPLICIT    = 0x88,

        EOR_IMMEDIATE   = 0x49,
        EOR_ZERO_PAGE   = 0x45,
        EOR_ZERO_PAGE_X = 0x55,
        EOR_ABSOLUTE    = 0x4D,
        EOR_ABSOLUTE_X  = 0x5D,
        EOR_ABSOLUTE_Y  = 0x59,
        EOR_INDIRECT_X  = 0x41,
        EOR_INDIRECT_Y  = 0x51,

        INC_ZERO_PAGE   = 0xE6,
        INC_ZERO_PAGE_X = 0xF6,
        INC_ABSOLUTE    = 0xEE,
        INC_ABSOLUTE_X  = 0xFE,

        INX_IMPLICIT    = 0xE8,
        INY_IMPLICIT    = 0xC8,

        JMP_ABSOLUTE    = 0x4C,
        JMP_INDIRECT    = 0x6C,

        JSR_ABSOLUTE    = 0x20,

        LDA_IMMEDIATE   = 0xA9,
        LDA_ZERO_PAGE   = 0xA5,
        LDA_ZERO_PAGE_X = 0xB5,
        LDA_ABSOLUTE    = 0xAD,
        LDA_ABSOLUTE_X  = 0xBD,
        LDA_ABSOLUTE_Y  = 0xB9,
        LDA_INDIRECT_X  = 0xA1,
        LDA_INDIRECT_Y  = 0xB1,

        LDX_IMMEDIATE   = 0xA2,
        LDX_ZERO_PAGE   = 0xA6,
        LDX_ZERO_PAGE_Y = 0xB6,
        LDX_ABSOLUTE    = 0xAE,
        LDX_ABSOLUTE_Y  = 0xBE,

        LDY_IMMEDIATE   = 0xA0,
        LDY_ZERO_PAGE   = 0xA4,
        LDY_ZERO_PAGE_X = 0xB4,
        LDY_ABSOLUTE    = 0xAC,
        LDY_ABSOLUTE_X  = 0xBC,

        LSR_ACCUMULATOR = 0x4A,
        LSR_ZERO_PAGE   = 0x46,
        LSR_ZERO_PAGE_X = 0x56,
        LSR_ABSOLUTE    = 0x4E,
        LSR_ABSOLUTE_X  = 0X5E,

        NOP_IMPLICIT    = 0xEA,

        ORA_IMMEDIATE   = 0x09,
        ORA_ZERO_PAGE   = 0x05,
        ORA_ZERO_PAGE_X = 0x15,
        ORA_ABSOLUTE    = 0x0D,
        ORA_ABSOLUTE_X  = 0x1D,
        ORA_ABSOLUTE_Y  = 0x19,
        ORA_INDIRECT_X  = 0x01,
        ORA_INDIRECT_Y  = 0x11,

        PHA_IMPLICIT    = 0x48,
        PHP_IMPLICIT    = 0x08,

        PLA_IMPLICIT    = 0x68,
        PLP_IMPLICIT    = 0x28,

        ROL_ACCUMULATOR = 0x2A,
        ROL_ZERO_PAGE   = 0x26,
        ROL_ZERO_PAGE_X = 0x36,
        ROL_ABSOLUTE    = 0x2E,
        ROL_ABSOLUTE_X  = 0x3E,

        ROR_ACCUMULATOR = 0x6A,
        ROR_ZERO_PAGE   = 0x66,
        ROR_ZERO_PAGE_X = 0x76,
        ROR_ABSOLUTE    = 0x6E,
        ROR_ABSOLUTE_X  = 0x7E,

        RTI_IMPLICIT    = 0x40,
        RTS_IMPLICIT    = 0x60,

        SBC_IMMEDIATE   = 0xE9,
        SBC_ZERO_PAGE   = 0xE5,
        SBC_ZERO_PAGE_X = 0xF5,
        SBC_ABSOLUTE    = 0xED,
        SBC_ABSOLUTE_X  = 0xFD,
        SBC_ABSOLUTE_Y  = 0xF9,
        SBC_INDIRECT_X  = 0xE1,
        SBC_INDIRECT_Y  = 0xF1,

        SEC_IMPLICIT    = 0x38,
        SED_IMPLICIT    = 0xF8,
        SEI_IMPLICIT    = 0x78,

        STA_ZERO_PAGE   = 0x85,
        STA_ZERO_PAGE_X = 0x95,
        STA_ABSOLUTE    = 0x8D,
        STA_ABSOLUTE_X  = 0x9D,
        STA_ABSOLUTE_Y  = 0x99,
        STA_INDIRECT_X  = 0x81,
        STA_INDIRECT_Y  = 0x91,

        STX_ZERO_PAGE   = 0x86,
        STX_ZERO_PAGE_Y = 0x96,
        STX_ABSOLUTE    = 0x8E,

        STY_ZERO_PAGE   = 0x84,
        STY_ZERO_PAGE_X = 0x94,
        STY_ABSOLUTE    = 0x8C,

        TAX_IMPLICIT    = 0xAA,
        TAY_IMPLICIT    = 0xA8,
        TSX_IMPLICIT    = 0xBA,
        TXA_IMPLICIT    = 0x8A,
        TXS_IMPLICIT    = 0x9A,
        TYA_IMPLICIT    = 0x98
    };

    enum class Register { AC, X, Y, SP, SR };



}

#endif //EMULATOR_MOS6502_MOS6502_DEFINITIONS_HPP
