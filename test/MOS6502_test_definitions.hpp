//
// Created by Mikhail on 20/10/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_TEST_DEFINITIONS_HPP
#define EMULATOR_MOS6502_MOS6502_TEST_DEFINITIONS_HPP

#include "MOS6502_definitions.hpp"

using namespace Emulator;

enum class AddressingModeTest {

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

using Location = std::variant<Word, Register>;

enum class LogicalOperation { AND, OR, XOR };

#endif //EMULATOR_MOS6502_MOS6502_TEST_DEFINITIONS_HPP
