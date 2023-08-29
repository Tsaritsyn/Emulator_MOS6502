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

    enum Flag {
        NEGATIVE = 7,
        OVERFLOW = 6,
        BREAK = 4,
        DECIMAL = 3,
        INTERRUPT = 2,
        ZERO = 1,
        CARRY = 0
    };

    enum class AddressingMode {
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
        AC, X, Y, SP, SR
    };

    [[nodiscard]] std::string dump(bool include_memory = false) const;


    // ******************* //
    // REGISTER OPERATIONS //
    // ******************* //

    void load_register(Register reg, AddressingMode mode);

    void store_register(Register reg, AddressingMode mode);

    void transfer_registers(Register from, Register to) { write_to_register(to, read_from_register(from)); };



    // **************** //
    // STACK OPERATIONS //
    // **************** //

    void push_to_stack(Register reg);

    void pull_from_stack(Register to);


    // ****************** //
    // LOGICAL OPERATIONS //
    // ****************** //

    /**
     * Performs AND operation on AC and a byte from memory.
     * Result is written back to AC.
     */
    void logical_and(AddressingMode mode);

    /**
     * Performs XOR operation on AC and a byte from memory.
     * Result is written back to AC.
     */
    void logical_xor(AddressingMode mode);

    /**
     * Performs OR operation on AC and a byte from memory.
     * Result is written back to AC.
     */
    void logical_or(AddressingMode mode);

    /**
     * This instructions is used to test if one or more bits are set in a target memory location.
     * The mask pattern in A is ANDed with the value in memory to set or clear the zero flag, but the result is not kept.
     * Bits 7 and 6 of the value from memory are copied into the N and V flags.
     */
    void bit_test(AddressingMode mode);


    // ********************* //
    // ARITHMETIC OPERATIONS //
    // ********************* //

    /**
     * Adds value in memory to the accumulator.
     * The result is written back to accumulator.
     */
    void add_with_carry(AddressingMode mode);

    /**
     * Subtracts value in memory from the accumulator.
     * The result is written back to accumulator.
     */
    void subtract_with_carry(AddressingMode mode);

    /**
     * Compares value in register to the value in memory.
     * Can only be used with AC, X or Y.
     * If register is greater or equal, carry flag is set, if less, negative flag is set, if equal, zero flag is set.
     */
    void compare_register(Register reg, AddressingMode mode);


    // ************************* //
    // INCREMENTS AND DECREMENTS //
    // ************************* //

    void increment_memory(AddressingMode mode);

    void decrement_memory(AddressingMode mode);

    void increment_register(Register reg);

    void decrement_register(Register reg);


    // ****** //
    // SHIFTS //
    // ****** //

    void shift_left_accumulator();

    void shift_left_memory(AddressingMode mode);

    void shift_right_accumulator();

    void shift_right_memory(AddressingMode mode);

    void rotate_left_accumulator();

    void rotate_left_memory(AddressingMode mode);

    void rotate_right_accumulator();

    void rotate_right_memory(AddressingMode mode);


    // *************** //
    // JUMPS AND CALLS //
    // *************** //

    void jump(AddressingMode mode);

    void jump_to_subroutine();

    void return_from_subroutine();


    // ******** //
    // BRANCHES //
    // ******** //

    void branch_if_carry_clear();

    void branch_if_carry_set();

    void branch_if_zero_clear();

    void branch_if_zero_set();

    void branch_if_negative_clear();

    void branch_if_negative_set();

    void branch_if_overflow_clear();

    void branch_if_overflow_set();


    // **************** //
    // SYSTEM FUNCTIONS //
    // **************** //

    void force_interrupt();

    void nop() { cycle++; };

    void return_from_interrupt();



//private:

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
     *  0xFFFC-0xFFFD - address of power on reset location,
     *  0xFFFE-0xFFFF - address of BRK/interrupt request handler.
     */
    static constexpr Word INTERRUPT_HANDLER = 0xFFFA;
    static constexpr Word RESET_LOCATION = 0xFFFC;
    static constexpr Word BRK_HANDLER = 0xFFFE;
    std::array<Byte, UINT16_MAX> memory;

    /// current cycle of the processor
    size_t cycle;



    // **************** //
    // HELPER FUNCTIONS //
    // **************** //

    void set_flag(Flag flag, bool value, bool increment_cycle = false);

    void write_to_register(Register reg, Byte value);

    /// reads the byte at the address specified by program counter and increments the latter
    Byte read_current_byte();

    /// reads the 2 consecutive bytes, the first of which is addressed by the program counter and increases the latter by 2
    Word inline read_current_word() { return ((Word)read_current_byte() << 8) + read_current_byte(); }

    /// reads word, least significant byte of which is pointed at by the address
    Word inline read_reversed_word(Word address) { return ((Word)read_byte(address + 1) << 8) + read_byte(address); }

    /// reads byte from memory determining the address according to the given scheme
    Byte read_byte(AddressingMode mode) { return read_byte(determine_address(mode)); };

    /// reads byte from memory at a given address
    Byte read_byte(Word address);

    /// writes the specified value to the specified address in memory
    void write_byte(Byte value, Word address, bool set_flags = false);

    Byte read_from_register(Register reg);

    Word determine_address(AddressingMode mode);

    void push_byte_to_stack(Byte value);

    /// first pushes the least significant byte, then the most significant
    void push_word_to_stack(Word value);

    Byte pull_byte_from_stack();

    /// first pulls the most significant byte, then the least significant
    Word pull_word_from_stack();

    [[nodiscard]] bool check_flag(Flag flag) const;
};



std::ostream& operator <<(std::ostream &os, CPU::AddressingMode mode);

std::ostream& operator <<(std::ostream &os, CPU::Register aRegister);


#endif //EMULATOR_MOS6502_CPU_HPP
