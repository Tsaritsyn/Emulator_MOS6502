//
// Created by Mikhail on 30/08/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_DEFINITIONS_HPP
#define EMULATOR_MOS6502_MOS6502_DEFINITIONS_HPP

#include <cstdint>
#include <array>
#include <iomanip>
#include <bitset>
#include <variant>

#define HEX_BYTE(byte) "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << ((int)(byte)) << std::dec
#define HEX_WORD(word) "0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << ((int)(word)) << std::dec
#define HEX_CULL_BYTE(byte) std::uppercase << std::setfill('0') << std::setw(2) << std::hex << ((int)(byte)) << std::dec


namespace Emulator {
    using Byte = uint8_t;
    using Word = uint16_t;
    using ROM = std::array<Byte, UINT16_MAX>;
    using ProcessorStatus = std::bitset<8>;


    constexpr bool CLEAR = false;
    constexpr bool SET = true;

    template <typename T>
    concept Printable = requires(std::ostream& os, const T& a) { os << a; };



    struct WordToBytes {
        // TODO: depend on the endianness https://stackoverflow.com/questions/1001307/detecting-endianness-programmatically-in-a-c-program
        Byte &low = converter.bytes[0];
        Byte &high = converter.bytes[1];
        Word &word = converter.word;

        WordToBytes(Word word = 0): converter{.word = word} {};

    private:
        union {
            Word word;
            Byte bytes[2];
        } converter;
    };


    enum Flag {
        /**
         * The negative flag is set if the result of the last operation had bit 7 set to a one.
         */
        NEGATIVE = 7,

        /**
         * The overflow flag is set during arithmetic operations if the result has yielded an invalid 2's complement result
         *  (e.g. adding to positive numbers and ending up with a negative result: 64 + 64 => -128).
         * It is determined by looking at the carry between bits 6 and 7 and between bit 7 and the carry flag.
         */
        OVERFLOW = 6,

        /**
         * The break command bit is set when a BRK instruction has been executed and an interrupt has been generated to process it.
         */
        BREAK = 4,

        /**
         * While the decimal mode flag is set the processor will obey the rules of Binary Coded Decimal (BCD) arithmetic during addition and subtraction.
         * The flag can be explicitly set using 'Set Decimal Flag' (SED) and cleared with 'Clear Decimal Flag' (CLD).
         */
        DECIMAL = 3,

        /**
         * The interrupt disable flag is set if the program has executed a 'Set Interrupt Disable' (SEI) instruction.
         * While this flag is set the processor will not respond to interrupts from devices until it is cleared by a 'Clear Interrupt Disable' (CLI) instruction.
         */
        INTERRUPT_DISABLE = 2,

        /**
         * The zero flag is set if the result of the last operation as was zero.
         */
        ZERO = 1,

        /**
         * The carry flag is set if the last operation caused an overflow from bit 7 of the result or an underflow from bit 0.
         * This condition is set during arithmetic, comparison and during logical shifts.
         * It can be explicitly set using the 'Set Carry Flag' (SEC) instruction and cleared with 'Clear Carry Flag' (CLC).
         */
        CARRY = 0
    };


    enum class AddressingMode {

        /**
         * For many 6502 instructions the source and destination of the information to be manipulated is implied directly by the function of the instruction itself and no further operand needs to be specified.
         * Operations like 'Clear Carry Flag' (CLC) and 'Return from Subroutine' (RTS) are implicit.
         */
        IMPLICIT,

        /**
         * Some instructions have an option to operate directly upon the accumulator.
         * The programmer specifies this by using a special operand value, 'A'.
         */
        ACCUMULATOR,

        /**
         * Immediate addressing allows the programmer to directly specify an 8 bit constant within the instruction.
         * It is indicated by a '#' symbol followed by an numeric expression
         */
        IMMEDIATE,

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
         * Relative addressing mode is used by branch instructions (e.g. BEQ, BNE, etc.) which contain a signed 8 bit relative offset
         *  (e.g. -128 to +127) which is added to program counter if the condition is true.
         * As the program counter itself is incremented during instruction execution by two the effective address range
         *  for the target instruction must be with -126 to +129 bytes of the branch.
         */
        RELATIVE,

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
         * JMP is the only 6502 instruction to support indirection.
         * The instruction contains a 16 bit address which identifies the location of the least significant byte of
         *  another 16 bit memory address which is the real target of the instruction.
         * For example if location $0120 contains $FC and location $0121 contains $BA then the instruction JMP ($0120)
         *  will cause the next instruction execution to occur at $BAFC (e.g. the contents of $0120 and $0121).
         */
        INDIRECT,

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


    enum class Instruction {
        /**
         * Add with Carry
         *
         * A,Z,C,N = A+M+C
         * This instruction adds the contents of a memory location to the accumulator together with the carry bit.
         * If overflow occurs the carry bit is set, this enables multiple byte addition to be performed.
         */
        ADC,

        /**
         * Logical AND
         *
         * A,Z,N = A&M
         * A logical AND is performed, bit by bit, on the accumulator contents using the contents of a byte of memory.
         */
        AND,

        /**
         * Arithmetic Shift Left
         *
         * A,Z,C,N = M*2 or M,Z,C,N = M*2
         * This operation shifts all the bits of the accumulator or memory contents one bit left.
         * Bit 0 is set to 0 and bit 7 is placed in the carry flag.
         * The effect of this operation is to multiply the memory contents by 2 (ignoring 2's complement considerations), setting the carry if the result will not fit in 8 bits.
         */
        ASL,

        /**
         * Branch if Carry Clear
         *
         * If the carry flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
         */
        BCC,

        /**
         * Branch if Carry Set
         *
         * If the carry flag is set then add the relative displacement to the program counter to cause a branch to a new location.
         */
        BCS,

        /**
         * Branch if Carry Set
         *
         * If the carry flag is set then add the relative displacement to the program counter to cause a branch to a new location.
         */
        BEQ,

        /**
         * Branch if Equal
         *
         * If the zero flag is set then add the relative displacement to the program counter to cause a branch to a new location.
         */
        BIT,

        /**
         * Bit Test
         *
         * A & M, N = M7, V = M6
         * This instructions is used to test if one or more bits are set in a target memory location.
         * The mask pattern in A is ANDed with the value in memory to set or clear the zero flag, but the result is not kept.
         * Bits 7 and 6 of the value from memory are copied into the N and V flags.
         */
        BMI,

        /**
         * Branch if Minus
         *
         * If the negative flag is set then add the relative displacement to the program counter to cause a branch to a new location.
         */
        BNE,

        /**
         * Branch if Not Equal
         *
         * If the zero flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
         */
        BPL,

        /**
         * Branch if Positive
         *
         * If the negative flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
         */
        BRK,

        /**
         * Force Interrupt
         *
         * The BRK instruction forces the generation of an interrupt request.
         * The program counter and processor status are pushed on the stack then the IRQ interrupt vector at $FFFE/F is loaded into the PC and the break flag in the status set to one.
         */
        BVC,

        /**
         * Branch if Overflow Clear
         *
         * If the overflow flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
         */
        BVS,

        /**
         * Branch if Overflow Set
         *
         * If the overflow flag is set then add the relative displacement to the program counter to cause a branch to a new location.
         */
        CLC,

        /**
         * Clear Carry Flag
         *
         * C = 0
         * Set the carry flag to zero.
         */
        CLD,

        /**
         * Clear Decimal Mode
         *
         * D = 0
         * Sets the decimal mode flag to zero.
         */
        CLI,

        /**
         * Clear Interrupt Disable
         *
         * I = 0
         * Clears the interrupt disable flag allowing normal interrupt requests to be serviced.
         */
        CLV,

        /**
         * Clear Overflow Flag
         *
         * V = 0
         * Clears the overflow flag.
         */
        CMP,

        /**
         * Compare
         *
         * Z,C,N = A-M
         * This instruction compares the contents of the accumulator with another memory held value and sets the zero and carry flags as appropriate.
         */
        CPX,

        /**
         * Compare X Register
         *
         * Z,C,N = X-M
         * This instruction compares the contents of the X register with another memory held value and sets the zero and carry flags as appropriate.
         */
        CPY,

        /**
         * Compare Y Register
         *
         * Z,C,N = Y-M
         * This instruction compares the contents of the Y register with another memory held value and sets the zero and carry flags as appropriate.
         */
        DEC,

        /**
         * Decrement Memory
         *
         * M,Z,N = M-1
         * Subtracts one from the value held at a specified memory location setting the zero and negative flags as appropriate.
         */
        DEX,

        /**
         * Decrement X Register
         *
         * X,Z,N = X-1
         * Subtracts one from the X register setting the zero and negative flags as appropriate.
         */
        DEY,

        /**
         * Exclusive OR
         *
         * A,Z,N = A^M
         * An exclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory.
         */
        EOR,

        /**
         * Increment Memory
         *
         * M,Z,N = M+1
         * Adds one to the value held at a specified memory location setting the zero and negative flags as appropriate.
         */
        INC,

        /**
         * Increment X Register
         *
         * X,Z,N = X+1
         * Adds one to the X register setting the zero and negative flags as appropriate.
         */
        INX,

        /**
         * Increment Y Register
         *
         * Y,Z,N = Y+1
         * Adds one to the Y register setting the zero and negative flags as appropriate.
         */
        INY,

        /**
         * Jump
         *
         * Sets the program counter to the address specified by the operand.
         *
         * NB:
         * An original 6502 has does not correctly fetch the target address if the indirect vector falls on a page boundary (e.g. $xxFF where xx is any value from $00 to $FF).
         * In this case fetches the LSB from $xxFF as expected but takes the MSB from $xx00.
         * This is fixed in some later chips like the 65SC02 so for compatibility always ensure the indirect vector is not at the end of the page.
         */
        JMP,

        /**
         * Jump to Subroutine
         *
         * The JSR instruction pushes the address (minus one) of the return point on to the stack and then sets the program counter to the target memory address.
         */
        JSR,

        /**
         * Load Accumulator
         *
         * A,Z,N = M
         * Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate.
         */
        LDA,

        /**
         * Load X Register
         *
         * X,Z,N = M
         * Loads a byte of memory into the X register setting the zero and negative flags as appropriate.
         */
        LDX,

        /**
         * Load Y Register
         *
         * Y,Z,N = M
         * Loads a byte of memory into the Y register setting the zero and negative flags as appropriate.
         */
        LDY,

        /**
         * Logical Shift Right
         *
         * A,C,Z,N = A/2 or M,C,Z,N = M/2
         * Each of the bits in A or M is shift one place to the right.
         * The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
         */
        LSR,

        /**
         * No Operation
         *
         * The NOP instruction causes no changes to the processor other than the normal incrementing of the program counter to the next instruction.
         */
        NOP,

        /**
         * Logical Inclusive OR
         *
         * A,Z,N = A|M
         * An inclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory.
         */
        ORA,

        /**
         * Push Accumulator
         *
         * Pushes a copy of the accumulator on to the stack.
         */
        PHA,

        /**
         * Push Processor Status
         *
         * Pushes a copy of the status flags on to the stack.
         */
        PHP,

        /**
         * Pull Accumulator
         *
         * Pulls an 8 bit value from the stack and into the accumulator.
         * The zero and negative flags are set as appropriate.
         */
        PLA,

        /**
         * Pull Processor Status
         *
         * Pulls an 8 bit value from the stack and into the processor flags.
         * The flags will take on new states as determined by the value pulled.
         */
        PLP,

        /**
         * Rotate Left
         *
         * Move each of the bits in either A or M one place to the left.
         * Bit 0 is filled with the current value of the carry flag whilst the old bit 7 becomes the new carry flag value.
         */
        ROL,

        /**
         * Rotate Right
         *
         * Move each of the bits in either A or M one place to the right.
         * Bit 7 is filled with the current value of the carry flag whilst the old bit 0 becomes the new carry flag value.
         */
        ROR,

        /**
         * Return from Interrupt
         *
         * The RTI instruction is used at the end of an interrupt processing routine.
         * It pulls the processor flags from the stack followed by the program counter.
         */
        RTI,

        /**
         * Return from Subroutine
         *
         * The RTS instruction is used at the end of a subroutine to return to the calling routine.
         * It pulls the program counter (minus one) from the stack.
         */
        RTS,

        /**
         * Subtract with Carry
         *
         * A,Z,C,N = A-M-(1-C)
         * This instruction subtracts the contents of a memory location to the accumulator together with the not of the carry bit.
         * If overflow occurs the carry bit is clear, this enables multiple byte subtraction to be performed.
         */
        SBC,

        /**
         * Set Carry Flag
         *
         * C = 1
         * Set the carry flag to one.
         */
        SEC,

        /**
         * Set Decimal Flag
         *
         * D = 1
         * Set the decimal mode flag to one.
         */
        SED,

        /**
         * Set Interrupt Disable
         *
         * I = 1
         * Set the interrupt disable flag to one.
         */
        SEI,

        /**
         * Store Accumulator
         *
         * M = A
         * Stores the contents of the accumulator into memory.
         */
        STA,

        /**
         * Store X Register
         *
         * M = X
         * Stores the contents of the X register into memory.
         */
        STX,

        /**
         * Store Y Register
         *
         * M = Y
         * Stores the contents of the Y register into memory.
         */
        STY,

        /**
         * Transfer Accumulator to X
         *
         * X = A
         * Copies the current contents of the accumulator into the X register and sets the zero and negative flags as appropriate.
         */
        TAX,

        /**
         * Transfer Accumulator to Y
         *
         * Y = A
         * Copies the current contents of the accumulator into the Y register and sets the zero and negative flags as appropriate.
         */
        TAY,

        /**
         * Transfer Stack Pointer to X
         *
         * X = S
         * Copies the current contents of the stack register into the X register and sets the zero and negative flags as appropriate.
         */
        TSX,

        /**
         * Transfer X to Accumulator
         *
         * A = X
         * Copies the current contents of the X register into the accumulator and sets the zero and negative flags as appropriate.
         */
        TXA,

        /**
         * Transfer X to Stack Pointer
         *
         * S = X
         * Copies the current contents of the X register into the stack register.
         */
        TXS,

        /**
         * Transfer Y to Accumulator
         *
         * A = Y
         * Copies the current contents of the Y register into the accumulator and sets the zero and negative flags as appropriate.
         */
        TYA
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

        DEC_ZER0_PAGE   = 0xC6,
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


    enum class LogicalOperation { AND, OR, XOR };

    enum class Register { AC, X, Y, SP, SR };

    using Location = std::variant<Word, Register>;

}

#endif //EMULATOR_MOS6502_MOS6502_DEFINITIONS_HPP
