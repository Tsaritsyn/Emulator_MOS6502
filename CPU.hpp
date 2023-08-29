//
// Created by Mikhail on 28/08/2023.
//

#ifndef EMULATOR_MOS6502_CPU_HPP
#define EMULATOR_MOS6502_CPU_HPP

#include <cstdint>
#include <array>

/**
 * Emulator of MOS 6502 microprocessor. It has three 8-bit registers and 64Kb of memory.
 */
class CPU {
public:
    using Byte = uint8_t;
    using Word = uint16_t;

    enum class Flag {
        NEGATIVE,
        OVERFLOW,
        BREAK,
        DECIMAL,
        INTERRUPT,
        ZERO,
        CARRY
    };

    enum class AddressingMode {
        IMPLICIT,
        ACCUMULATOR,
        IMMEDIATE,
        ZERO_PAGE,
        ZERO_PAGE_X,
        ZERO_PAGE_Y,
        RELATIVE,
        ABSOLUTE,
        ABSOLUTE_X,
        ABSOLUTE_Y,
        INDIRECT,
        INDEXED_INDIRECT,
        INDIRECT_INDEXED
    };

    enum class Register {
        AC, X, Y
    };


    // ********************* //
    // LOAD/STORE OPERATIONS //
    // ********************* //

    void load_register(Register reg, AddressingMode mode);

    void store_accumulator();

    void store_X_register();

    void store_Y_register();



private:

    /// program counter
    Word PC;
    /// accumulator
    Byte AC;
    /// registers
    Byte X, Y;
    /// status register
    Byte SR;
    /// stack pointer
    Byte SP;

    /**
     * CPU memory storing processor stack and instructions referenced by program counter.
     *
     * Memory layout: 0x0000-0x00FF - zero page (256 bytes), 0x0100-0x01FF - system stack (256 bytes), other - program data.
     *
     * Special addresses: 0xFFFA-0xFFFB - address of non-maskable interrupt handler,
     *  0xFFFC-0xFFFD - address of power on reset location, 0xFFFE-0xFFFF - address of BRK/interrupt request handler.
     */
    std::array<Byte, UINT16_MAX> memory;

    /// current cycle of the processor
    size_t cycle;

    void set_flag(Flag flag);

    void load_register(Byte &reg, Byte value);

    Byte read_current_byte();

    /// reads byte from memory at a given address
    Byte read_byte(Word address);
};



std::ostream& operator <<(std::ostream &os, CPU::AddressingMode mode);

std::ostream& operator <<(std::ostream &os, CPU::Register aRegister);


#endif //EMULATOR_MOS6502_CPU_HPP
