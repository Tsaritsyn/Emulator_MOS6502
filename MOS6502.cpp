//
// Created by Mikhail on 28/08/2023.
//

#include <sstream>
#include <iomanip>
#include <bitset>

#include "MOS6502.hpp"

#define HEX_FORMAT "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex
#define HEX_CULL_FORMAT std::uppercase << std::setfill('0') << std::setw(2) << std::hex

namespace Emulator {

    constexpr Byte BIT0 = 1;
    constexpr Byte BIT1 = 1 << 1;
    constexpr Byte BIT2 = 1 << 2;
    constexpr Byte BIT3 = 1 << 3;
    constexpr Byte BIT4 = 1 << 4;
    constexpr Byte BIT5 = 1 << 5;
    constexpr Byte BIT6 = 1 << 6;
    constexpr Byte BIT7 = 1 << 7;


    void set_bit(Byte &byte, int number, bool to) {
        if (to) byte |= 1 << number;
        else byte &= ~(1 << number);
    }


    bool check_bit(Byte byte, int number) {
        return byte & (1 << number);
    }


    void MOS6502::set_flag(Flag flag, bool value, bool increment_cycle) {
        set_bit(SR, flag, value);
        if (increment_cycle) cycle++;
    }


    Byte MOS6502::read_current_byte() {
        return read_byte(PC++);
    }


    Word MOS6502::read_current_word() {
        WordToBytes buf{};
        buf.set_high(read_current_byte());
        buf.set_low(read_current_byte());
        return buf.word;
    }


    Word MOS6502::read_reversed_word(Word address) {
        WordToBytes buf{};
        buf.set_low(read_byte(address));
        buf.set_high(read_byte(address + 1));
        return buf.word;
    }


    Byte MOS6502::read_byte(Word address) {
        cycle++;
        return memory[address];
    }


    void MOS6502::load_register(Register reg, AddressingMode mode) {
        // TODO: check for forbidden register-mode pairs
        Word address = determine_address(mode);
        Byte value = read_byte(address);
        write_to_register(reg, value);
    }


    void MOS6502::write_to_register(Register reg, Byte value) {
        cycle++;
        switch (reg) {
            case Register::AC:
                AC = value;
                break;

            case Register::X:
                X = value;
                break;

            case Register::Y:
                Y = value;
                break;

            case Register::SP:
                // writing to stack register does not affect the flags
                SP = value;
                return;

            case Register::SR:
                // writing to processor status register does not affect the flags
                SR = value;
                return;
        }


        // setting flags
        set_flag(Flag::ZERO, value == 0);
        set_flag(Flag::NEGATIVE, check_bit(value, 7));
    }


    std::string MOS6502::dump(bool include_memory) const {
        std::stringstream ss;
        ss << "Registers: AC = " << HEX_FORMAT << (int) AC << ", X = " << HEX_FORMAT << (int) X << ", Y = "
           << HEX_FORMAT << (int) Y;
        ss << "\nProgram counter = " << HEX_FORMAT << (int) PC << ", Stack pointer = " << HEX_FORMAT << (int) SP;
        ss << "\nFlags: " << std::bitset<8>(SR);
        ss << "\nCurrent cycle = " << cycle;
        ss << "\nZero page: ";
        for (int i = 0x00; i < 0xFF; i++) ss << HEX_FORMAT << (int) memory[i] << ' ';
        ss << "\nStack: ";
        for (int i = 0x0100; i < 0x01ff; i++) ss << HEX_FORMAT << (int) memory[i] << ' ';
        ss << "\nSpecial addresses: non-maskable interrupt handler = " << HEX_FORMAT << (int) memory[0xfffb]
           << HEX_CULL_FORMAT << (int) memory[0xfffa]
           << ", power on reset location = " << HEX_FORMAT << (int) memory[0xfffd] << HEX_CULL_FORMAT
           << (int) memory[0xfffc]
           << ", BRK/interrupt request handler = " << HEX_FORMAT << (int) memory[0xffff] << HEX_CULL_FORMAT
           << (int) memory[0xfffe];

        if (include_memory) {
            ss << "\nRemaining memory:\n";
            for (int i = 0x01ff + 1; i < 0xfffa; i++) ss << HEX_FORMAT << (int) memory[i] << ' ';
            ss << "\nEND OF MEMORY.";
        }
        return ss.str();
    }


    void MOS6502::write_byte(Byte value, Word address, bool set_flags) {
        memory[address] = value;
        cycle++;

        if (set_flags) {
            // setting flags
            set_flag(Flag::ZERO, value == 0);
            set_flag(Flag::NEGATIVE, check_bit(value, 7));
        }
    }


    Byte MOS6502::read_from_register(Register reg) {
        cycle++;
        switch (reg) {
            case Register::AC:
                return AC;
            case Register::X:
                return X;
            case Register::Y:
                return Y;
            case Register::SP:
                return SP;
            case Register::SR:
                return SR;
        }
    }


    void MOS6502::store_register(Register reg, AddressingMode mode) {
        // TODO: check for forbidden register-mode pairs
        Word address = determine_address(mode);
        Byte value = read_from_register(reg);
        write_byte(value, address);
    }


    Word MOS6502::determine_address(AddressingMode mode) {
        switch (mode) {
            case AddressingMode::IMMEDIATE:
                return PC;

            case AddressingMode::ZERO_PAGE:
                return read_current_byte();

            case AddressingMode::ZERO_PAGE_X:
                return read_current_byte() + read_from_register(Register::X);

            case AddressingMode::ZERO_PAGE_Y:
                return read_current_byte() + read_from_register(Register::Y);

            case AddressingMode::RELATIVE:
                return PC + (int8_t) read_current_byte();

            case AddressingMode::ABSOLUTE:
                // in this mode the full 2-byte address is given as the instruction parameter, therefore we have to read 2 bytes
                // first byte is the most significant one
                return read_current_word();

            case AddressingMode::ABSOLUTE_X: {
                // in this mode the full 2-byte address is given as the instruction parameter, therefore we have to read 2 bytes
                // first byte is the most significant one
                Word address = read_current_byte();
                address <<= 8;

                // in this mode we need to add the value in X to the given address and keep track if page was crossed
                Word tmp = read_current_byte();
                tmp += read_from_register(Register::X);
                // if addition of the address with the value in X leads to change of the page, an extra cycle is spent
                if (tmp > UINT8_MAX) cycle++;

                address += tmp;
                return address;
            }

            case AddressingMode::ABSOLUTE_Y: {
                // in this mode the full 2-byte address is given as the instruction parameter, therefore we have to read 2 bytes
                // first byte is the most significant one
                Word address = read_current_byte();
                address <<= 8;

                // in this mode we need to add the value in Y to the given address and keep track if page was crossed
                Word tmp = read_current_byte();
                tmp += read_from_register(Register::Y);
                // if addition of the address with the value in Y leads to change of the page, an extra cycle is spent
                if (tmp > UINT8_MAX) cycle++;

                address += tmp;
                return address;
            }

            case AddressingMode::INDIRECT:
                return read_reversed_word(read_current_word());

            case AddressingMode::INDEXED_INDIRECT: {
                // in this mode we do all operations in zero page
                Byte tableAddress = read_current_byte();

                Byte targetAddress = tableAddress + read_from_register(Register::X);

                // for some reason, these two bytes must be swapped
                // so if memory at targetAddress gives 0x0080, then address will be 0x8000
                return read_reversed_word(targetAddress);
            }

            case AddressingMode::INDIRECT_INDEXED: {
                Byte zeroPageLocation = read_current_byte();

                Word leastSignificantByteOfAddress = read_byte(zeroPageLocation);
                leastSignificantByteOfAddress += read_from_register(Register::Y);
                if (leastSignificantByteOfAddress > UINT8_MAX) cycle++;

                Word address = read_byte(zeroPageLocation + 1);
                address <<= 8;
                address += leastSignificantByteOfAddress;
                return address;
            }
        }
    }


    void MOS6502::push_to_stack(Register reg) {
        if (reg != Register::AC && reg != Register::SR) {
            std::stringstream ss;
            ss << "Cannot push register " << reg << " on stack";
            throw std::invalid_argument(ss.str());
        }

        push_byte_to_stack(read_from_register(reg));
    }


    void MOS6502::push_byte_to_stack(Byte value) {
        Word address = 0x0100 + read_from_register(Register::SP);
        write_byte(value, address);
        SP++;
    }


    Byte MOS6502::pull_byte_from_stack() {
        Word address = 0x0100 + read_from_register(Register::SP);
        Byte value = read_byte(address);
        // for some reason, pulling from stack takes 1 more cycle than pushing to it, so we do write_to_register instead of manipulating SP directly
        write_to_register(Register::SP, SP - 1);
        return value;
    }


    void MOS6502::pull_from_stack(Register to) {
        if (to != Register::AC && to != Register::SR) {
            std::stringstream ss;
            ss << "Cannot pull from stack to register " << to;
            throw std::invalid_argument(ss.str());
        }

        write_to_register(to, pull_byte_from_stack());
    }


    void MOS6502::logical_and(AddressingMode mode) {
        Byte rhs = read_byte(mode);
        write_to_register(Register::AC, AC & rhs);
    }


    void MOS6502::logical_xor(AddressingMode mode) {
        Byte rhs = read_byte(mode);
        write_to_register(Register::AC, AC ^ rhs);
    }


    void MOS6502::logical_or(AddressingMode mode) {
        Byte rhs = read_byte(mode);
        write_to_register(Register::AC, AC | rhs);
    }


    void MOS6502::bit_test(AddressingMode mode) {
        Byte accumulator_byte = read_from_register(Register::AC);
        Byte memory_byte = read_byte(mode);
        bool test_result = accumulator_byte & memory_byte;

        set_flag(Flag::ZERO, test_result);
        set_flag(Flag::OVERFLOW, check_bit(memory_byte, OVERFLOW));
        set_flag(Flag::NEGATIVE, check_bit(memory_byte, 7));
    }


    void MOS6502::add_with_carry(AddressingMode mode) {
        bool initial_sign_bit = check_bit(AC, NEGATIVE);

        Byte rhs = read_byte(mode);
        bool rhs_sign_bit = check_bit(rhs, NEGATIVE);

        write_to_register(Register::AC, AC + rhs + check_flag(CARRY));

        bool resulting_sign_bit = check_bit(AC, NEGATIVE);

        // carry flag is set only when the result crossed 255
        set_flag(Flag::OVERFLOW, initial_sign_bit && !resulting_sign_bit);

        // overflow flag is set every time when the sign of the result is incorrect
        set_flag(Flag::OVERFLOW, initial_sign_bit == rhs_sign_bit && initial_sign_bit != resulting_sign_bit);
    }


    void MOS6502::subtract_with_carry(AddressingMode mode) {
        bool initial_sign_bit = check_bit(AC, NEGATIVE);

        Byte rhs = read_byte(mode);
        bool rhs_sign_bit = check_bit(rhs, NEGATIVE);

        write_to_register(Register::AC, AC - rhs - !check_flag(CARRY));

        bool resulting_sign_bit = check_bit(AC, NEGATIVE);

        // carry flag is set only when the result crossed 0
        set_flag(Flag::OVERFLOW, !initial_sign_bit && resulting_sign_bit);

        // overflow flag is set every time when the sign of the result is incorrect
        set_flag(Flag::OVERFLOW, initial_sign_bit != rhs_sign_bit && initial_sign_bit != resulting_sign_bit);
    }


    void MOS6502::compare_register(Register reg, AddressingMode mode) {
        if (reg != Register::AC && reg != Register::X && reg != Register::Y) {
            std::stringstream ss;
            ss << "Cannot compare register " << reg;
            throw std::invalid_argument(ss.str());
        }

        Byte reg_value = read_from_register(reg);
        Byte memory_value = read_byte(mode);

        set_flag(Flag::CARRY, reg_value >= memory_value);
        set_flag(Flag::ZERO, reg_value == memory_value);
        set_flag(Flag::NEGATIVE, reg_value < memory_value);
    }


    void MOS6502::increment_memory(AddressingMode mode) {
        Word address = determine_address(mode);
        write_byte(read_byte(address) + 1, address, true);
    }


    void MOS6502::decrement_memory(AddressingMode mode) {
        Word address = determine_address(mode);
        write_byte(read_byte(address) - 1, address, true);
    }


    void MOS6502::increment_register(Register reg) {
        if (reg != Register::X && reg != Register::Y) {
            std::stringstream ss;
            ss << "Cannot increment register " << reg;
            throw std::invalid_argument(ss.str());
        }

        write_to_register(reg, read_from_register(reg) + 1);
    }


    void MOS6502::decrement_register(Register reg) {
        if (reg != Register::X && reg != Register::Y) {
            std::stringstream ss;
            ss << "Cannot decrement register " << reg;
            throw std::invalid_argument(ss.str());
        }

        write_to_register(reg, read_from_register(reg) - 1);
    }


    void MOS6502::shift_left_accumulator() {
        set_flag(Flag::CARRY, check_bit(AC, 7));
        write_to_register(Register::AC, read_from_register(Register::AC) << 1);
    }


    void MOS6502::shift_left_memory(AddressingMode mode) {
        Word address = determine_address(mode);
        Byte value = read_byte(address);

        set_flag(Flag::CARRY, check_bit(value, 7));

        write_byte(value << 1, address, true);
    }


    void MOS6502::shift_right_accumulator() {
        set_flag(Flag::CARRY, check_bit(AC, 0));
        write_to_register(Register::AC, read_from_register(Register::AC) >> 1);
    }


    void MOS6502::shift_right_memory(AddressingMode mode) {
        Word address = determine_address(mode);
        Byte value = read_byte(address);

        set_flag(Flag::CARRY, check_bit(value, 0));

        write_byte(value >> 1, address, true);
    }


    void MOS6502::rotate_left_accumulator() {
        set_flag(Flag::CARRY, check_bit(AC, 7));
        write_to_register(Register::AC, (read_from_register(Register::AC) << 1) + check_flag(CARRY));
    }


    void MOS6502::rotate_left_memory(AddressingMode mode) {
        Word address = determine_address(mode);
        Byte value = read_byte(address);

        set_flag(Flag::CARRY, check_bit(value, 7));

        write_byte((value << 1) + check_flag(CARRY), address, true);
    }


    void MOS6502::rotate_right_accumulator() {
        set_flag(Flag::CARRY, check_bit(AC, CARRY));

        Byte value = read_from_register(Register::AC);
        value >>= 1;
        set_bit(value, 7, check_flag(CARRY));

        write_to_register(Register::AC, value);
    }


    void MOS6502::rotate_right_memory(AddressingMode mode) {
        Word address = determine_address(mode);
        Byte value = read_byte(address);
        set_flag(Flag::CARRY, check_bit(value, 0));

        value >>= 1;
        set_bit(value, 7, check_flag(CARRY));

        write_byte(value, address, true);
    }


    void MOS6502::jump(AddressingMode mode) {
        PC = determine_address(mode);
    }


    void MOS6502::jump_to_subroutine() {
        Word targetAddress = determine_address(AddressingMode::ABSOLUTE);
        push_word_to_stack(PC - 1);
        PC = targetAddress;
    }


    void MOS6502::return_from_subroutine() {
        PC = pull_word_from_stack() + 1;
    }


    void MOS6502::push_word_to_stack(Word value) {
        push_byte_to_stack(value);
        value >>= 8;
        push_byte_to_stack(value);
    }


    Word MOS6502::pull_word_from_stack() {
        Word result = pull_byte_from_stack();
        result <<= 8;
        return result + pull_byte_from_stack();
    }


    bool MOS6502::check_flag(Flag flag) const {
        return check_bit(SR, flag);
    }


    void MOS6502::branch_if_carry_clear() {
        Word new_address = determine_address(AddressingMode::RELATIVE);
        if (!check_flag(CARRY)) PC = new_address;
    }


    void MOS6502::branch_if_carry_set() {
        Word new_address = determine_address(AddressingMode::RELATIVE);
        if (check_flag(CARRY)) PC = new_address;
    }


    void MOS6502::branch_if_zero_clear() {
        Word new_address = determine_address(AddressingMode::RELATIVE);
        if (!check_flag(ZERO)) PC = new_address;
    }


    void MOS6502::branch_if_zero_set() {
        Word new_address = determine_address(AddressingMode::RELATIVE);
        if (check_flag(ZERO)) PC = new_address;
    }


    void MOS6502::branch_if_negative_clear() {
        Word new_address = determine_address(AddressingMode::RELATIVE);
        if (!check_flag(NEGATIVE)) PC = new_address;
    }


    void MOS6502::branch_if_negative_set() {
        Word new_address = determine_address(AddressingMode::RELATIVE);
        if (check_flag(NEGATIVE)) PC = new_address;
    }


    void MOS6502::branch_if_overflow_clear() {
        Word new_address = determine_address(AddressingMode::RELATIVE);
        if (!check_flag(OVERFLOW)) PC = new_address;
    }


    void MOS6502::branch_if_overflow_set() {
        Word new_address = determine_address(AddressingMode::RELATIVE);
        if (check_flag(OVERFLOW)) PC = new_address;
    }


    void MOS6502::force_interrupt() {
        push_word_to_stack(PC);
        push_to_stack(Register::SR);
        PC = read_reversed_word(BRK_HANDLER);
    }


    void MOS6502::return_from_interrupt() {
        pull_from_stack(Register::SR);
        PC = pull_word_from_stack() + 1;
    }


    void MOS6502::reset() {
        PC = read_reversed_word(RESET_LOCATION);
        cycle = 7;
    }


    std::ostream &operator<<(std::ostream &os, AddressingMode mode) {
        switch (mode) {
            case AddressingMode::IMMEDIATE:
                os << "Immediate";
                break;
            case AddressingMode::ZERO_PAGE:
                os << "Zero Page";
                break;
            case AddressingMode::ZERO_PAGE_X:
                os << "Zero Page,X";
                break;
            case AddressingMode::ZERO_PAGE_Y:
                os << "Zero Page,Y";
                break;
            case AddressingMode::RELATIVE:
                os << "Relative";
                break;
            case AddressingMode::ABSOLUTE:
                os << "Absolute";
                break;
            case AddressingMode::ABSOLUTE_X:
                os << "Absolute,X";
                break;
            case AddressingMode::ABSOLUTE_Y:
                os << "Absolute,Y";
                break;
            case AddressingMode::INDIRECT:
                os << "Indirect";
                break;
            case AddressingMode::INDEXED_INDIRECT:
                os << "Indexed Indirect";
                break;
            case AddressingMode::INDIRECT_INDEXED:
                os << "Indirect Indexed";
                break;
        }
        return os;
    }


    std::ostream &operator<<(std::ostream &os, Register aRegister) {
        switch (aRegister) {
            case Register::AC:
                os << 'A';
                break;
            case Register::X:
                os << 'X';
                break;
            case Register::Y:
                os << 'Y';
                break;
            case Register::SP:
                os << "SP";
                break;
            case Register::SR:
                os << "SR";
                break;
        }
        return os;
    }

}






