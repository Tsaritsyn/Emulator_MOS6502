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


    void set_bit(Byte &byte, int number, bool value) {
        if (value) byte |= 1 << number;
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
        buf.high = read_current_byte();
        buf.low = read_current_byte();
        return buf.word;
    }


    Word MOS6502::read_reversed_word(Word address) {
        WordToBytes buf{};
        buf.low = read_byte(address);
        buf.high = read_byte(address + 1);
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
//        cycle++;
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

        throw std::runtime_error("Some registers were not handled");
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

            case AddressingMode::INDIRECT_X: {
                // in this mode we do all operations in zero page
                Byte tableAddress = read_current_byte();

                Byte targetAddress = tableAddress + read_from_register(Register::X);

                // for some reason, these two bytes must be swapped
                // so if memory at targetAddress gives 0x0080, then address will be 0x8000
                return read_reversed_word(targetAddress);
            }

            case AddressingMode::INDIRECT_Y: {
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

        throw std::runtime_error("Some addressing modes were not handled");
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
        SP--;
    }


    Byte MOS6502::pull_byte_from_stack() {
        // for some reason, pulling from stack takes 1 more cycle than pushing to it, so we do write_to_register instead of manipulating SP directly
        // equivalent for SP++
        write_to_register(Register::SP, SP + 1);

        Word address = 0x0100 + read_from_register(Register::SP);
        Byte value = read_byte(address);
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


    void MOS6502::logical(LogicalOperation operation, AddressingMode mode) {
        Byte mem = read_byte(mode);

        Byte result;
        switch (operation) {
            case LogicalOperation::AND:
                result = AC & mem;
                break;
            case LogicalOperation::OR:
                result = AC | mem;
                break;
            case LogicalOperation::XOR:
                result = AC ^ mem;
                break;
        }

        write_to_register(Register::AC, result);
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
        printf("AC = %d, memory = %d, carry = %d\n", AC, rhs, check_flag(CARRY));
        bool rhs_sign_bit = check_bit(rhs, NEGATIVE);

        write_to_register(Register::AC, AC + rhs + check_flag(CARRY));

        bool resulting_sign_bit = check_bit(AC, NEGATIVE);

        // carry flag is set only when the result crossed 255
        set_flag(Flag::CARRY, (initial_sign_bit || rhs_sign_bit) && !resulting_sign_bit);

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
        WordToBytes buf(value);
        push_byte_to_stack(buf.high);
        push_byte_to_stack(buf.low);
    }


    Word MOS6502::pull_word_from_stack() {
        WordToBytes buf{};
        buf.low = pull_byte_from_stack();
        buf.high = pull_byte_from_stack();
        return buf.word;
    }


    bool MOS6502::check_flag(Flag flag) const {
        return check_bit(SR, flag);
    }


    void MOS6502::branch_if(Flag flag_to_check, bool value_to_expect) {
        Word new_address = determine_address(AddressingMode::RELATIVE);
        if (check_flag(flag_to_check) == value_to_expect) PC = new_address;
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
        set_flag(INTERRUPT_DISABLE, true);
    }



    Byte MOS6502::read_byte(AddressingMode mode) {
        if (mode == AddressingMode::IMMEDIATE) return read_current_byte();

        return read_byte(determine_address(mode));
    }



    void MOS6502::execute_current_command() {
        OpCode opCode{read_current_byte()};
        constexpr bool CLEAR = false;
        constexpr bool SET = true;

        switch (opCode) {
            case ADC_IMMEDIATE:
                add_with_carry(AddressingMode::IMMEDIATE);
                break;
            case ADC_ZERO_PAGE:
                add_with_carry(AddressingMode::ZERO_PAGE);
                break;
            case ADC_ZERO_PAGE_X:
                add_with_carry(AddressingMode::ZERO_PAGE_X);
                break;
            case ADC_ABSOLUTE:
                add_with_carry(AddressingMode::ABSOLUTE);
                break;
            case ADC_ABSOLUTE_X:
                add_with_carry(AddressingMode::ABSOLUTE_X);
                break;
            case ADC_ABSOLUTE_Y:
                add_with_carry(AddressingMode::ABSOLUTE_Y);
                break;
            case ADC_INDIRECT_X:
                add_with_carry(AddressingMode::INDIRECT_X);
                break;
            case ADC_INDIRECT_Y:
                add_with_carry(AddressingMode::INDIRECT_Y);
                break;

            case AND_IMMEDIATE:
                logical(LogicalOperation::AND, AddressingMode::IMMEDIATE);
                break;
            case AND_ZERO_PAGE:
                logical(LogicalOperation::AND, AddressingMode::ZERO_PAGE);
                break;
            case AND_ZERO_PAGE_X:
                logical(LogicalOperation::AND, AddressingMode::ZERO_PAGE_X);
                break;
            case AND_ABSOLUTE:
                logical(LogicalOperation::AND, AddressingMode::ABSOLUTE);
                break;
            case AND_ABSOLUTE_X:
                logical(LogicalOperation::AND, AddressingMode::ABSOLUTE_X);
                break;
            case AND_ABSOLUTE_Y:
                logical(LogicalOperation::AND, AddressingMode::ABSOLUTE_Y);
                break;
            case AND_INDIRECT_X:
                logical(LogicalOperation::AND, AddressingMode::INDIRECT_X);
                break;
            case AND_INDIRECT_Y:
                logical(LogicalOperation::AND, AddressingMode::INDIRECT_Y);
                break;

            case ASL_ACCUMULATOR:
                shift_left_accumulator();
                break;
            case ASL_ZERO_PAGE:
                shift_left_memory(AddressingMode::ZERO_PAGE);
                break;
            case ASL_ZERO_PAGE_X:
                shift_left_memory(AddressingMode::ZERO_PAGE_X);
                break;
            case ASL_ABSOLUTE:
                shift_left_memory(AddressingMode::ABSOLUTE);
                break;
            case ASL_ABSOLUTE_X:
                shift_left_memory(AddressingMode::ABSOLUTE_X);
                break;

            case BCC:
                branch_if(CARRY, CLEAR);
                break;
            case BCS:
                branch_if(CARRY, SET);
                break;
            case BEQ:
                branch_if(ZERO, SET);
                break;
            case BMI:
                branch_if(NEGATIVE, SET);
                break;
            case BNE:
                branch_if(ZERO, CLEAR);
                break;
            case BPL:
                branch_if(NEGATIVE, CLEAR);
                break;
            case BVC:
                branch_if(OVERFLOW, CLEAR);
                break;
            case BVS:
                branch_if(OVERFLOW, SET);
                break;

            case BIT_ZERO_PAGE:
                bit_test(AddressingMode::ZERO_PAGE);
                break;
            case BIT_ABSOLUTE:
                bit_test(AddressingMode::ABSOLUTE);
                break;

            case BRK:
                force_interrupt();
                break;

            case CLC:
                set_flag(CARRY, CLEAR, true);
                break;
            case CLD:
                set_flag(DECIMAL, CLEAR, true);
                break;
            case CLI:
                set_flag(INTERRUPT_DISABLE, CLEAR, true);
                break;
            case CLV:
                set_flag(OVERFLOW, CLEAR, true);
                break;

            case CMP_IMMEDIATE:
                compare_register(Register::AC, AddressingMode::IMMEDIATE);
                break;
            case CMP_ZERO_PAGE:
                compare_register(Register::AC, AddressingMode::ZERO_PAGE);
                break;
            case CMP_ZERO_PAGE_X:
                compare_register(Register::AC, AddressingMode::ZERO_PAGE_X);
                break;
            case CMP_ABSOLUTE:
                compare_register(Register::AC, AddressingMode::ABSOLUTE);
                break;
            case CMP_ABSOLUTE_X:
                compare_register(Register::AC, AddressingMode::ABSOLUTE_X);
                break;
            case CMP_ABSOLUTE_Y:
                compare_register(Register::AC, AddressingMode::ABSOLUTE_Y);
                break;
            case CMP_INDIRECT_X:
                compare_register(Register::AC, AddressingMode::INDIRECT_X);
                break;
            case CMP_INDIRECT_Y:
                compare_register(Register::AC, AddressingMode::INDIRECT_Y);
                break;

            case CPX_IMMEDIATE:
                compare_register(Register::X, AddressingMode::IMMEDIATE);
                break;
            case CPX_ZERO_PAGE:
                compare_register(Register::X, AddressingMode::ZERO_PAGE);
                break;
            case CPX_ABSOLUTE:
                compare_register(Register::X, AddressingMode::ABSOLUTE);
                break;

            case CPY_IMMEDIATE:
                compare_register(Register::Y, AddressingMode::IMMEDIATE);
                break;
            case CPY_ZERO_PAGE:
                compare_register(Register::Y, AddressingMode::ZERO_PAGE);
                break;
            case CPY_ABSOLUTE:
                compare_register(Register::Y, AddressingMode::ABSOLUTE);
                break;

            case DEC_ZER0_PAGE:
                decrement_memory(AddressingMode::ZERO_PAGE);
                break;
            case DEC_ZERO_PAGE_X:
                decrement_memory(AddressingMode::ZERO_PAGE_X);
                break;
            case DEC_ABSOLUTE:
                decrement_memory(AddressingMode::ABSOLUTE);
                break;
            case DEC_ABSOLUTE_X:
                decrement_memory(AddressingMode::ABSOLUTE_X);
                break;

            case DEX:
                decrement_register(Register::X);
                break;
            case DEY:
                decrement_register(Register::Y);
                break;

            case EOR_IMMEDIATE:
                logical(LogicalOperation::XOR, AddressingMode::IMMEDIATE);
                break;
            case EOR_ZERO_PAGE:
                logical(LogicalOperation::XOR, AddressingMode::ZERO_PAGE);
                break;
            case EOR_ZERO_PAGE_X:
                logical(LogicalOperation::XOR, AddressingMode::ZERO_PAGE_X);
                break;
            case EOR_ABSOLUTE:
                logical(LogicalOperation::XOR, AddressingMode::ABSOLUTE);
                break;
            case EOR_ABSOLUTE_X:
                logical(LogicalOperation::XOR, AddressingMode::ABSOLUTE_X);
                break;
            case EOR_ABSOLUTE_Y:
                logical(LogicalOperation::XOR, AddressingMode::ABSOLUTE_Y);
                break;
            case EOR_INDIRECT_X:
                logical(LogicalOperation::XOR, AddressingMode::INDIRECT_X);
                break;
            case EOR_INDIRECT_Y:
                logical(LogicalOperation::XOR, AddressingMode::INDIRECT_Y);
                break;

            case INC_ZERO_PAGE:
                increment_memory(AddressingMode::ZERO_PAGE);
                break;
            case INC_ZERO_PAGE_X:
                increment_memory(AddressingMode::ZERO_PAGE_X);
                break;
            case INC_ABSOLUTE:
                increment_memory(AddressingMode::ABSOLUTE);
                break;
            case INC_ABSOLUTE_X:
                increment_memory(AddressingMode::ABSOLUTE_X);
                break;

            case INX:
                increment_register(Register::X);
                break;
            case INY:
                increment_register(Register::Y);
                break;

            case JMP_ABSOLUTE:
                jump(AddressingMode::ABSOLUTE);
                break;
            case JMP_INDIRECT:
                jump(AddressingMode::INDIRECT);
                break;

            case JSR:
                jump_to_subroutine();
                break;

            case LDA_IMMEDIATE:
                load_register(Register::AC, AddressingMode::IMMEDIATE);
                break;
            case LDA_ZERO_PAGE:
                load_register(Register::AC, AddressingMode::ZERO_PAGE);
                break;
            case LDA_ZERO_PAGE_X:
                load_register(Register::AC, AddressingMode::ZERO_PAGE_X);
                break;
            case LDA_ABSOLUTE:
                load_register(Register::AC, AddressingMode::ABSOLUTE);
                break;
            case LDA_ABSOLUTE_X:
                load_register(Register::AC, AddressingMode::ABSOLUTE_X);
                break;
            case LDA_ABSOLUTE_Y:
                load_register(Register::AC, AddressingMode::ABSOLUTE_Y);
                break;
            case LDA_INDIRECT_X:
                load_register(Register::AC, AddressingMode::INDIRECT_X);
                break;
            case LDA_INDIRECT_Y:
                load_register(Register::AC, AddressingMode::INDIRECT_Y);
                break;

            case LDX_IMMEDIATE:
                load_register(Register::X, AddressingMode::IMMEDIATE);
                break;
            case LDX_ZERO_PAGE:
                load_register(Register::X, AddressingMode::ZERO_PAGE);
                break;
            case LDX_ZERO_PAGE_Y:
                load_register(Register::X, AddressingMode::ZERO_PAGE_Y);
                break;
            case LDX_ABSOLUTE:
                load_register(Register::X, AddressingMode::ABSOLUTE);
                break;
            case LDX_ABSOLUTE_Y:
                load_register(Register::X, AddressingMode::ABSOLUTE_Y);
                break;

            case LDY_IMMEDIATE:
                load_register(Register::Y, AddressingMode::IMMEDIATE);
                break;
            case LDY_ZERO_PAGE:
                load_register(Register::Y, AddressingMode::ZERO_PAGE);
                break;
            case LDY_ZERO_PAGE_X:
                load_register(Register::Y, AddressingMode::ZERO_PAGE_X);
                break;
            case LDY_ABSOLUTE:
                load_register(Register::Y, AddressingMode::ABSOLUTE);
                break;
            case LDY_ABSOLUTE_X:
                load_register(Register::Y, AddressingMode::ABSOLUTE_X);
                break;

            case LSR_ACCUMULATOR:
                shift_right_accumulator();
                break;
            case LSR_ZERO_PAGE:
                shift_right_memory(AddressingMode::ZERO_PAGE);
                break;
            case LSR_ZERO_PAGE_X:
                shift_right_memory(AddressingMode::ZERO_PAGE_X);
                break;
            case LSR_ABSOLUTE:
                shift_right_memory(AddressingMode::ABSOLUTE);
                break;
            case LSR_ABSOLUTE_X:
                shift_right_memory(AddressingMode::ABSOLUTE_X);
                break;

            case NOP:
                nop();
                break;

            case ORA_IMMEDIATE:
                logical(LogicalOperation::OR, AddressingMode::IMMEDIATE);
                break;
            case ORA_ZERO_PAGE:
                logical(LogicalOperation::OR, AddressingMode::ZERO_PAGE);
                break;
            case ORA_ZERO_PAGE_X:
                logical(LogicalOperation::OR, AddressingMode::ZERO_PAGE_X);
                break;
            case ORA_ABSOLUTE:
                logical(LogicalOperation::OR, AddressingMode::ABSOLUTE);
                break;
            case ORA_ABSOLUTE_X:
                logical(LogicalOperation::OR, AddressingMode::ABSOLUTE_X);
                break;
            case ORA_ABSOLUTE_Y:
                logical(LogicalOperation::OR, AddressingMode::ABSOLUTE_Y);
                break;
            case ORA_INDIRECT_X:
                logical(LogicalOperation::OR, AddressingMode::INDIRECT_X);
                break;
            case ORA_INDIRECT_Y:
                logical(LogicalOperation::OR, AddressingMode::INDIRECT_Y);
                break;

            case PHA:
                push_to_stack(Register::AC);
                break;
            case PHP:
                push_to_stack(Register::SR);
                break;

            case PLA:
                pull_from_stack(Register::AC);
                break;
            case PLP:
                pull_from_stack(Register::SR);
                break;

            case ROL_ACCUMULATOR:
                rotate_left_accumulator();
                break;
            case ROL_ZERO_PAGE:
                rotate_left_memory(AddressingMode::ZERO_PAGE);
                break;
            case ROL_ZERO_PAGE_X:
                rotate_left_memory(AddressingMode::ZERO_PAGE_X);
                break;
            case ROL_ABSOLUTE:
                rotate_left_memory(AddressingMode::ABSOLUTE);
                break;
            case ROL_ABSOLUTE_X:
                rotate_left_memory(AddressingMode::ABSOLUTE_X);
                break;

            case ROR_ACCUMULATOR:
                rotate_right_accumulator();
                break;
            case ROR_ZERO_PAGE:
                rotate_right_memory(AddressingMode::ZERO_PAGE);
                break;
            case ROR_ZERO_PAGE_X:
                rotate_right_memory(AddressingMode::ZERO_PAGE_X);
                break;
            case ROR_ABSOLUTE:
                rotate_right_memory(AddressingMode::ABSOLUTE);
                break;
            case ROR_ABSOLUTE_X:
                rotate_right_memory(AddressingMode::ABSOLUTE_X);
                break;

            case RTI:
                return_from_interrupt();
                break;
            case RTS:
                return_from_subroutine();
                break;

            case SBC_IMMEDIATE:
                subtract_with_carry(AddressingMode::IMMEDIATE);
                break;
            case SBC_ZERO_PAGE:
                subtract_with_carry(AddressingMode::ZERO_PAGE);
                break;
            case SBC_ZERO_PAGE_X:
                subtract_with_carry(AddressingMode::ZERO_PAGE_X);
                break;
            case SBC_ABSOLUTE:
                subtract_with_carry(AddressingMode::ABSOLUTE);
                break;
            case SBC_ABSOLUTE_X:
                subtract_with_carry(AddressingMode::ABSOLUTE_X);
                break;
            case SBC_ABSOLUTE_Y:
                subtract_with_carry(AddressingMode::ABSOLUTE_Y);
                break;
            case SBC_INDIRECT_X:
                subtract_with_carry(AddressingMode::INDIRECT_X);
                break;
            case SBC_INDIRECT_Y:
                subtract_with_carry(AddressingMode::INDIRECT_Y);
                break;

            case SEC:
                set_flag(CARRY, SET, true);
                break;
            case SED:
                set_flag(DECIMAL, SET, true);
                break;
            case SEI:
                set_flag(INTERRUPT_DISABLE, SET, true);
                break;

            case STA_ZERO_PAGE:
                store_register(Register::AC, AddressingMode::ZERO_PAGE);
                break;
            case STA_ZERO_PAGE_X:
                store_register(Register::AC, AddressingMode::ZERO_PAGE_X);
                break;
            case STA_ABSOLUTE:
                store_register(Register::AC, AddressingMode::ABSOLUTE);
                break;
            case STA_ABSOLUTE_X:
                store_register(Register::AC, AddressingMode::ABSOLUTE_X);
                break;
            case STA_ABSOLUTE_Y:
                store_register(Register::AC, AddressingMode::ABSOLUTE_Y);
                break;
            case STA_INDIRECT_X:
                store_register(Register::AC, AddressingMode::INDIRECT_X);
                break;
            case STA_INDIRECT_Y:
                store_register(Register::AC, AddressingMode::INDIRECT_Y);
                break;

            case STX_ZERO_PAGE:
                store_register(Register::X, AddressingMode::ZERO_PAGE);
                break;
            case STX_ZERO_PAGE_Y:
                store_register(Register::X, AddressingMode::ZERO_PAGE_Y);
                break;
            case STX_ABSOLUTE:
                store_register(Register::X, AddressingMode::ABSOLUTE);
                break;

            case STY_ZERO_PAGE:
                store_register(Register::Y, AddressingMode::ZERO_PAGE);
                break;
            case STY_ZERO_PAGE_X:
                store_register(Register::Y, AddressingMode::ZERO_PAGE_X);
                break;
            case STY_ABSOLUTE:
                store_register(Register::Y, AddressingMode::ABSOLUTE);
                break;

            case TAX:
                transfer_registers(Register::AC, Register::X);
                break;
            case TAY:
                transfer_registers(Register::AC, Register::Y);
                break;
            case TSX:
                transfer_registers(Register::SP, Register::X);
                break;
            case TXA:
                transfer_registers(Register::X, Register::AC);
                break;
            case TXS:
                transfer_registers(Register::X, Register::SP);
                break;
            case TYA:
                transfer_registers(Register::Y, Register::AC);
                break;
        }
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
            case AddressingMode::INDIRECT_X:
                os << "Indexed Indirect";
                break;
            case AddressingMode::INDIRECT_Y:
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



    Byte flag_combination(const std::vector<Flag> &flags) {
        Byte result = 0;
        for (auto flag: flags) set_bit(result, flag);
        return result;
    }

}






