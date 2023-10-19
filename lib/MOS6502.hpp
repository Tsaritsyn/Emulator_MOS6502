//
// Created by Mikhail on 28/08/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_HPP
#define EMULATOR_MOS6502_MOS6502_HPP

#include "MOS6502_helpers.hpp"
#include "ROM.hpp"
#include "Operation.hpp"
#include "ProcessorStatus.hpp"

#include <optional>
#include <bitset>
#include <functional>


namespace Emulator {
    /**
     * Emulator of MOS 6502 microprocessor. It has three 8-bit registers and 64Kb of memory.
     */
    class MOS6502 {

    public:

        /*
         * Successful termination statuses
         */

        struct StopOnBreak { Word address; };
        /// address of the next command
        struct StopOnMaxReached { Word address; };

        using SuccessfulTermination = std::variant<StopOnBreak, StopOnMaxReached>;

        /*
         * Error termination statuses
         */

        struct UnknownOperation { Word address; };

        using ErrorTermination = std::variant<UnknownOperation>;



        bool verbose = false;


        [[nodiscard]] std::string dump(bool include_memory = false) const;

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
        void burn(const ROM &newMemory) noexcept { memory = newMemory; }

        std::expected<SuccessfulTermination, ErrorTermination> execute();

        Byte& operator [](const Location& address);

        Byte operator [](const Location& address) const;

        void execute(const Operation& operation) noexcept;

        void set_stop_on_break(bool value) noexcept { stopOnBRK = value; }

        void set_maximum_number_of_commands_to_execute(size_t value) noexcept { maxNumberOfCommandsToExecute = value; }


    private:
        friend class MOS6502_TestFixture;

        using ByteOperator = Byte(MOS6502::*)(Byte);



        // **************** //
        // HELPER FUNCTIONS //
        // **************** //

        [[nodiscard]] Byte index_zero_page(Byte address, Byte index) noexcept;
        [[nodiscard]] Word index_absolute(Word address, Byte index) noexcept;

        /*
         * Resolve the actual address of the stored byte
         */

        static Word resolve_zero_page(Byte address) noexcept              { return address; }
        Word resolve_zero_page_indexed(Byte address, Byte index) noexcept { return index_zero_page(address, index); }
        static Word resolve_absolute(Word address) noexcept               { return address; }
        Word resolve_absolute_indexed(Word address, Byte index) noexcept  { return index_absolute(address, index); }
        Word resolve_indirect(Word address) noexcept                      { return memory.fetch_word(address, cycle); }
        Word resolve_indirect_X(Byte address) noexcept                    { return memory.fetch_word(index_zero_page(address, X), cycle); }
        Word resolve_indirect_Y(Byte address) noexcept                    { return index_absolute(memory.fetch_word(address, cycle), Y); }


        /*
         * Fetching a byte from memory using different addressing modes
         */

        [[nodiscard]] Byte fetch_from_zero_page(Byte address) noexcept                     { return memory.fetch_byte(resolve_zero_page(address), cycle); }
        [[nodiscard]] Byte fetch_from_zero_page_indexed(Byte address, Byte index) noexcept { return memory.fetch_byte(resolve_zero_page_indexed(address, index), cycle); }
        [[nodiscard]] Byte fetch_from_absolute(Word address) noexcept                      { return memory.fetch_byte(resolve_absolute(address), cycle); }
        [[nodiscard]] Byte fetch_from_absolute_indexed(Word address, Byte index) noexcept  {  return memory.fetch_byte(resolve_absolute_indexed(address, index), cycle); }
        [[nodiscard]] Byte fetch_from_indirect(Word address) noexcept                      { return memory.fetch_byte(resolve_indirect(address), cycle); }
        [[nodiscard]] Byte fetch_from_indirect_X(Byte address) noexcept                    { return memory.fetch_byte(resolve_indirect_X(address), cycle); }
        [[nodiscard]] Byte fetch_from_indirect_Y(Byte address) noexcept                    { return memory.fetch_byte(resolve_indirect_Y(address), cycle); }


        /*
         * Write the given value to the address resolved according to the mode
         */

        void write_to_zero_page(Byte value, Byte address) noexcept                     { memory.set_byte({.address = resolve_zero_page(address), .value = value, .cycle = cycle}); }
        void write_to_zero_page_indexed(Byte value, Byte address, Byte index) noexcept { memory.set_byte({.address = resolve_zero_page_indexed(address, index), .value = value, .cycle = cycle}); }
        void write_to_absolute(Byte value, Word address) noexcept                      { memory.set_byte({.address = resolve_absolute(address), .value = value, .cycle = cycle}); }
        void write_to_absolute_indexed(Byte value, Word address, Byte index) noexcept  { memory.set_byte({.address = resolve_absolute_indexed(address, index), .value = value, .cycle = cycle}); }
        void write_to_indirect_X(Byte value, Byte address) noexcept                    { memory.set_byte({.address = resolve_indirect_X(address), .value = value, .cycle = cycle}); }
        void write_to_indirect_Y(Byte value, Byte address) noexcept                    { memory.set_byte({.address = resolve_indirect_Y(address), .value = value, .cycle = cycle}); }


        /*
         * Replacing a byte of memory with a new value
         */

        void perform_at_zero_page(Byte address, ByteOperator byteOperator) noexcept;
        void perform_at_zero_page_indexed(Byte address, Byte index, ByteOperator byteOperator) noexcept;
        void perform_at_absolute(Word address, ByteOperator byteOperator) noexcept;
        void perform_at_absolute_indexed(Word address, Byte index, ByteOperator byteOperator) noexcept;

        void set_register(Register reg, Byte value);

        void set_writing_flags(Byte value);

        [[nodiscard]] Byte get_register(Register reg) const;

        void push_byte_to_stack(Byte value);

        /// first pushes the least significant byte, then the most significant
        void push_word_to_stack(Word value);

        Byte pull_byte_from_stack();

        /// first pulls the most significant byte, then the least significant
        Word pull_word_from_stack();

        void add_to_accumulator(Byte value) noexcept;

        void and_with_accumulator(Byte value) noexcept;

        Byte shift_left(Byte value) noexcept;

        void branch(char offset) noexcept;

        void bit_test(Byte value) noexcept;

        void compare(Byte reg, Byte value) noexcept;

        Byte decrement(Byte value) noexcept;

        void xor_with_accumulator(Byte value) noexcept;

        Byte increment(Byte value) noexcept;

        Byte shift_right(Byte value) noexcept;

        void or_with_accumulator(Byte value) noexcept;

        Byte rotate_left(Byte value) noexcept;

        Byte rotate_right(Byte value) noexcept;

        void subtract_from_accumulator(Byte value) noexcept;


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

        // auxiliary variables, not defined by the MOS6502 specifications
        bool pageCrossed;

        // execution conditions
        bool stopOnBRK;
        std::optional<size_t> maxNumberOfCommandsToExecute;
    };
}

#endif //EMULATOR_MOS6502_MOS6502_HPP
