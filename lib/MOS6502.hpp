//
// Created by Mikhail on 28/08/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_HPP
#define EMULATOR_MOS6502_MOS6502_HPP

#include "MOS6502_definitions.hpp"
#include "ROM.hpp"

#include <optional>
#include <bitset>


namespace Emulator {
    /**
     * Emulator of MOS 6502 microprocessor. It has three 8-bit registers and 64Kb of memory.
     */
    class MOS6502 {

    public:

        static constexpr Word STACK_BOTTOM = 0x0100;
        static constexpr Word INTERRUPT_HANDLER = 0xFFFA;
        static constexpr Word RESET_LOCATION = 0xFFFC;
        static constexpr Word BRK_HANDLER = 0xFFFE;

        bool verbose = false;


        [[nodiscard]] std::string dump(bool include_memory = false) const;

        void print_stack() const;

        virtual /**
         * Program counter is set to the value of RESET_LOCATION, cycle is set to 7, interrupt disable flag is set to 1.
         * It is recommended to begin the program with setting the stack pointer by LDX <value> and TXS.
         *
         * Example of a start-up program:
         *  LDX #FF;
         *  TXS;
         *  <periphery setting>;
         *  CLI;
         */
        void reset();

        /// sets the memory of the processor to the exact same values as the given new memory
        void burn(const ROM &newMemory) { memory = newMemory; }

        void execute_command(OpCode opCode);

        OpCode execute_current_command();

        void execute(bool stopOnBRK);

        Byte& operator [](const Location& address);

        Byte operator [](const Location& address) const;


    private:
        friend class MOS6502_TestFixture;

        // ******************* //
        // REGISTER OPERATIONS //
        // ******************* //

        void load_register(Register reg, AddressingMode mode);

        void store_register(Register reg, AddressingMode mode);

        void transfer_registers(Register from, Register to);



        // **************** //
        // STACK OPERATIONS //
        // **************** //

        void push_to_stack(Register reg);

        void pull_from_stack(Register to);


        // ****************** //
        // LOGICAL OPERATIONS //
        // ****************** //

        /**
         * Performs specified operation on AC and a byte from memory.
         * Result is written back to AC.
         */
        void logical(LogicalOperation operation, AddressingMode mode);

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

        void branch_if(Flag flag_to_check, bool value_to_expect);


        // **************** //
        // SYSTEM FUNCTIONS //
        // **************** //

        void brk();

        void nop() { cycle++; };

        void return_from_interrupt();



        // **************** //
        // HELPER FUNCTIONS //
        // **************** //

        void set_register(Register reg, Byte value);

        /// reads the byte at the address specified by program counter and increments the latter
        Byte read_current_byte();

        /// reads word wih least significant byte of which being at PC; increases PC by 2
        Word read_current_word();

        /// reads word, least significant byte of which is pointed at by the address
        Word read_reversed_word(Word address);

        /// reads byte from memory determining the address according to the given scheme
        Byte read_byte(AddressingMode mode); // { return read_byte(determine_address(mode)); };

        /// reads byte from memory at a given address
        Byte read_byte(Word address);

        /// writes the specified value to the specified address in memory
        void write_byte(Byte value, Word address, bool set_flags = false);

        [[nodiscard]] Byte get_register(Register reg) const;

        Word determine_address(AddressingMode mode, bool elapseCycleWhenNotCrossingPage = false);

        void push_byte_to_stack(Byte value);

        /// first pushes the least significant byte, then the most significant
        void push_word_to_stack(Word value);

        Byte pull_byte_from_stack();

        /// first pulls the most significant byte, then the least significant
        Word pull_word_from_stack();

        /**
         * Performs two byte additions, first low byte of the target with other byte, than high byte of the target with carry from the first step.
         * If the first step overflows Byte, 1 cycle will be elapsed, otherwise none.
         */
        Word add_word(Word word, Byte byte, bool takeCycleIfLowNotOverflowed = false);

        static Byte add_bytes(Byte target, Byte other, bool &carry);

        static Byte subtract_bytes(Byte target, Byte other, bool &carry);



    protected:

        /// program counter
        Word PC;
        /// accumulator
        Byte AC;
        /// registers
        Byte X, Y;
        /// status register
        ProcessorStatus SR;
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
        ROM memory;

        /// current cycle of the processor
        size_t cycle;
    };
}

#endif //EMULATOR_MOS6502_MOS6502_HPP
