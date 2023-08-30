//
// Created by Mikhail on 28/08/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_HPP
#define EMULATOR_MOS6502_MOS6502_HPP

#include <cstdint>
#include <array>
#include <vector>


namespace Emulator {

    using Byte = uint8_t;
    using Word = uint16_t;
    using ROM = std::array<Byte, UINT16_MAX>;




    struct WordToBytes {
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
        NEGATIVE = 7,
        OVERFLOW = 6,
        BREAK = 4,
        DECIMAL = 3,
        INTERRUPT_DISABLE = 2,
        ZERO = 1,
        CARRY = 0
    };

    Byte flag_combination(const std::vector<Flag> &flags);


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
        INDIRECT_X,
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

        BCC             = 0x90,
        BCS             = 0xB0,
        BEQ             = 0xF0,
        BMI             = 0x30,
        BNE             = 0xD0,
        BPL             = 0x10,
        BVC             = 0x50,
        BVS             = 0x70,

        BIT_ZERO_PAGE   = 0x24,
        BIT_ABSOLUTE    = 0X2C,

        BRK             = 0x00,

        CLC             = 0x18,
        CLD             = 0xD8,
        CLI             = 0x58,
        CLV             = 0xB8,

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

        DEX             = 0xCA,
        DEY             = 0x88,

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

        INX             = 0xE8,
        INY             = 0xC8,

        JMP_ABSOLUTE    = 0x4C,
        JMP_INDIRECT    = 0x6C,

        JSR             = 0x20,

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

        NOP             = 0xEA,

        ORA_IMMEDIATE   = 0x09,
        ORA_ZERO_PAGE   = 0x05,
        ORA_ZERO_PAGE_X = 0x15,
        ORA_ABSOLUTE    = 0x0D,
        ORA_ABSOLUTE_X  = 0x1D,
        ORA_ABSOLUTE_Y  = 0x19,
        ORA_INDIRECT_X  = 0x01,
        ORA_INDIRECT_Y  = 0x11,

        PHA             = 0x48,
        PHP             = 0x08,

        PLA             = 0x68,
        PLP             = 0x28,

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

        RTI             = 0x40,
        RTS             = 0x60,

        SBC_IMMEDIATE   = 0xE9,
        SBC_ZERO_PAGE   = 0xE5,
        SBC_ZERO_PAGE_X = 0xF5,
        SBC_ABSOLUTE    = 0xED,
        SBC_ABSOLUTE_X  = 0xFD,
        SBC_ABSOLUTE_Y  = 0xF9,
        SBC_INDIRECT_X  = 0xE1,
        SBC_INDIRECT_Y  = 0xF1,

        SEC             = 0x38,
        SED             = 0xF8,
        SEI             = 0x78,

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

        TAX             = 0xAA,
        TAY             = 0xA8,
        TSX             = 0xBA,
        TXA             = 0x8A,
        TXS             = 0x9A,
        TYA             = 0x98
    };


    enum class LogicalOperation { AND, OR, XOR };


    enum class Register { AC, X, Y, SP, SR };


    constexpr Byte BIT0 = 1;
    constexpr Byte BIT1 = 1 << 1;
    constexpr Byte BIT2 = 1 << 2;
    constexpr Byte BIT3 = 1 << 3;
    constexpr Byte BIT4 = 1 << 4;
    constexpr Byte BIT5 = 1 << 5;
    constexpr Byte BIT6 = 1 << 6;
    constexpr Byte BIT7 = 1 << 7;


    void set_bit(Byte &byte, int number, bool value = true);


    bool check_bit(Byte byte, int number);



    /**
     * Emulator of MOS 6502 microprocessor. It has three 8-bit registers and 64Kb of memory.
     */
    class MOS6502 {

    public:

        [[nodiscard]] std::string dump(bool include_memory = false) const;

        /**
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

        void execute_current_command();


    protected:
        friend class MOS6502_TestFixture;

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

        void force_interrupt();

        void nop() { cycle++; };

        void return_from_interrupt();



        // **************** //
        // HELPER FUNCTIONS //
        // **************** //

        void set_flag(Flag flag, bool value, bool increment_cycle = false);

        void write_to_register(Register reg, Byte value);

        /// reads the byte at the address specified by program counter and increments the latter
        Byte read_current_byte();

        /// reads the 2 consecutive bytes, the first of which is addressed by the program counter and increases the latter by 2
        Word read_current_word();

        /// reads word, least significant byte of which is pointed at by the address
        Word read_reversed_word(Word address);

        /// reads byte from memory determining the address according to the given scheme
        Byte read_byte(AddressingMode mode); // { return read_byte(determine_address(mode)); };

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
        ROM memory;

        /// current cycle of the processor
        size_t cycle;
    };


    std::ostream &operator<<(std::ostream &os, AddressingMode mode);

    std::ostream &operator<<(std::ostream &os, Register aRegister);

}

#endif //EMULATOR_MOS6502_MOS6502_HPP
