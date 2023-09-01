//
// Created by Mikhail on 28/08/2023.
//

#include <sstream>
#include <iomanip>
#include <bitset>
#include <iostream>
#include <optional>

#include "MOS6502.hpp"
#include "MOS6502_helpers.hpp"



namespace Emulator {

    void MOS6502::set_flag(Flag flag, bool value, bool increment_cycle) {
        set_bit(SR, flag, value);
        if (increment_cycle) cycle++;
    }


    Byte MOS6502::read_current_byte() {
        Byte result = read_byte(PC++);
        return result;
    }


    Word MOS6502::read_current_word() {
        WordToBytes buf{};
        buf.low = read_current_byte();
        buf.high = read_current_byte();
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
        set_register(reg, value, true);
    }


    void MOS6502::set_register(Register reg, Byte value, bool advanceCycle) {
        if (advanceCycle) cycle++;

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
        ss << "Registers: AC = " << HEX_BYTE((int) AC) << ", X = " << HEX_BYTE((int) X) << ", Y = " << HEX_BYTE((int) Y);
        ss << "\nProgram counter = " << HEX_BYTE((int) PC) << ", Stack pointer = " << HEX_BYTE((int) SP);
        ss << "\nFlags: " << std::bitset<8>(SR);
        ss << "\nCurrent cycle = " << cycle;
        ss << "\nZero page: ";
        for (int i = 0x00; i < 0xFF; i++) ss << HEX_BYTE((int) memory[i]) << ' ';
        ss << "\nStack: ";
        for (int i = 0x0100; i < 0x01ff; i++) ss << HEX_BYTE((int) memory[i]) << ' ';
        ss << "\nSpecial addresses: non-maskable interrupt handler = " << HEX_BYTE((int) memory[0xfffb])
           << HEX_CULL_BYTE((int) memory[0xfffa])
           << ", power on reset location = " << HEX_BYTE((int) memory[0xfffd]) << HEX_CULL_BYTE((int) memory[0xfffc])
           << ", BRK/interrupt request handler = " << HEX_BYTE((int) memory[0xffff]) << HEX_CULL_BYTE((int) memory[0xfffe]);

        if (include_memory) {
            ss << "\nRemaining memory:\n";
            for (int i = 0x01ff + 1; i < 0xfffa; i++) ss << HEX_BYTE((int) memory[i]) << ' ';
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


    Byte MOS6502::get_register(Emulator::Register reg) const {
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

        throw std::runtime_error("Unknown register");
    }


    void MOS6502::store_register(Register reg, AddressingMode mode) {
        write_byte(get_register(reg), determine_address(mode));
    }


    Word MOS6502::determine_address(AddressingMode mode) {
        switch (mode) {
            case AddressingMode::IMMEDIATE:
                return PC;

            case AddressingMode::ZERO_PAGE:
                // when nothing is added to the zero-page address, it does not elapse an additional cycle
                return read_zero_page_address_and_add(std::nullopt);

            case AddressingMode::ZERO_PAGE_X:
                return read_zero_page_address_and_add(X);

            case AddressingMode::ZERO_PAGE_Y:
                return read_zero_page_address_and_add(Y);

            case AddressingMode::RELATIVE:
                return PC + read_current_byte();

            case AddressingMode::ABSOLUTE:
                return read_current_word();

            case AddressingMode::ABSOLUTE_X:
                return add_word(read_current_word(), X);

            case AddressingMode::ABSOLUTE_Y:
                return add_word(read_current_word(), Y);

            case AddressingMode::INDIRECT:
                return read_reversed_word(read_current_word());

            case AddressingMode::INDIRECT_X:
                return read_reversed_word(read_zero_page_address_and_add(X));

            case AddressingMode::INDIRECT_Y:
                return add_word(read_reversed_word(read_zero_page_address_and_add(std::nullopt)), Y);
        }

        throw std::runtime_error("Some addressing modes were not handled");
    }


    void MOS6502::push_to_stack(Register reg) {
        push_byte_to_stack(get_register(reg));
    }


    void MOS6502::push_byte_to_stack(Byte value) {
        write_byte(value, STACK_LEFT_ADDR + SP--);
    }


    Byte MOS6502::pull_byte_from_stack() {
        return read_byte(STACK_LEFT_ADDR + SP++);
    }


    void MOS6502::pull_from_stack(Register to) {
        set_register(to, pull_byte_from_stack(), true);
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

        set_register(Register::AC, result, true);
    }


    void MOS6502::bit_test(AddressingMode mode) {
        Byte memory_byte = read_byte(mode);
        bool test_result = AC & memory_byte;

        set_flag(Flag::ZERO, test_result);
        set_flag(Flag::OVERFLOW, check_bit(memory_byte, OVERFLOW));
        set_flag(Flag::NEGATIVE, check_bit(memory_byte, 7));
    }


    void MOS6502::add_with_carry(AddressingMode mode) {
        bool initialACSignBit = check_bit(AC, NEGATIVE);

        Byte mem = read_byte(mode);
        bool initialMemSignBit = check_bit(mem, NEGATIVE);

        bool carry = check_flag(CARRY);
        Byte result = add_bytes(AC, mem, carry);
        bool resultSignBit = check_bit(result, NEGATIVE);

        set_register(Register::AC, result, false);
        set_flag(Flag::CARRY, carry);
        // overflow flag is set every time when the sign of the result is incorrect
        set_flag(Flag::OVERFLOW, (initialACSignBit == initialMemSignBit) && (initialACSignBit != resultSignBit));
    }


    void MOS6502::subtract_with_carry(AddressingMode mode) {
        bool initial_sign_bit = check_bit(AC, NEGATIVE);

        Byte rhs = read_byte(mode);
        bool rhs_sign_bit = check_bit(rhs, NEGATIVE);

        set_register(Register::AC, AC - rhs - !check_flag(CARRY), true);

        bool resulting_sign_bit = check_bit(AC, NEGATIVE);

        // carry flag is set only when the result crossed 0
        set_flag(Flag::OVERFLOW, !initial_sign_bit && resulting_sign_bit);

        // overflow flag is set every time when the sign of the result is incorrect
        set_flag(Flag::OVERFLOW, initial_sign_bit != rhs_sign_bit && initial_sign_bit != resulting_sign_bit);
    }


    void MOS6502::compare_register(Register reg, AddressingMode mode) {
        Byte reg_value = get_register(reg);
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
        set_register(reg, get_register(reg) + 1, true);
    }


    void MOS6502::decrement_register(Register reg) {
        set_register(reg, get_register(reg) - 1, true);
    }


    void MOS6502::shift_left_accumulator() {
        set_flag(Flag::CARRY, check_bit(AC, 7));
        set_register(Register::AC, get_register(Register::AC) << 1, true);
    }


    void MOS6502::shift_left_memory(AddressingMode mode) {
        Word address = determine_address(mode);
        Byte value = read_byte(address);

        set_flag(Flag::CARRY, check_bit(value, 7));

        write_byte(value << 1, address, true);
    }


    void MOS6502::shift_right_accumulator() {
        set_flag(Flag::CARRY, check_bit(AC, 0));
        set_register(Register::AC, get_register(Register::AC) >> 1, true);
    }


    void MOS6502::shift_right_memory(AddressingMode mode) {
        Word address = determine_address(mode);
        Byte value = read_byte(address);

        set_flag(Flag::CARRY, check_bit(value, 0));

        write_byte(value >> 1, address, true);
    }


    void MOS6502::rotate_left_accumulator() {
        set_flag(Flag::CARRY, check_bit(AC, 7));
        set_register(Register::AC, (AC << 1) + check_flag(CARRY), true);
    }


    void MOS6502::rotate_left_memory(AddressingMode mode) {
        Word address = determine_address(mode);
        Byte value = read_byte(address);

        set_flag(Flag::CARRY, check_bit(value, 7));

        write_byte((value << 1) + check_flag(CARRY), address, true);
    }


    void MOS6502::rotate_right_accumulator() {
        set_flag(Flag::CARRY, check_bit(AC, CARRY));

        Byte value = AC;
        value >>= 1;
        set_bit(value, 7, check_flag(CARRY));

        set_register(Register::AC, value, true);
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
        return (mode == AddressingMode::IMMEDIATE) ? read_current_byte() : read_byte(determine_address(mode));
    }



    void MOS6502::execute_command(OpCode opCode) {
        switch (opCode) {
            case OpCode::ADC_IMMEDIATE:
                add_with_carry(AddressingMode::IMMEDIATE);
                break;
            case OpCode::ADC_ZERO_PAGE:
                add_with_carry(AddressingMode::ZERO_PAGE);
                break;
            case OpCode::ADC_ZERO_PAGE_X:
                add_with_carry(AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::ADC_ABSOLUTE:
                add_with_carry(AddressingMode::ABSOLUTE);
                break;
            case OpCode::ADC_ABSOLUTE_X:
                add_with_carry(AddressingMode::ABSOLUTE_X);
                break;
            case OpCode::ADC_ABSOLUTE_Y:
                add_with_carry(AddressingMode::ABSOLUTE_Y);
                break;
            case OpCode::ADC_INDIRECT_X:
                add_with_carry(AddressingMode::INDIRECT_X);
                break;
            case OpCode::ADC_INDIRECT_Y:
                add_with_carry(AddressingMode::INDIRECT_Y);
                break;

            case OpCode::AND_IMMEDIATE:
                logical(LogicalOperation::AND, AddressingMode::IMMEDIATE);
                break;
            case OpCode::AND_ZERO_PAGE:
                logical(LogicalOperation::AND, AddressingMode::ZERO_PAGE);
                break;
            case OpCode::AND_ZERO_PAGE_X:
                logical(LogicalOperation::AND, AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::AND_ABSOLUTE:
                logical(LogicalOperation::AND, AddressingMode::ABSOLUTE);
                break;
            case OpCode::AND_ABSOLUTE_X:
                logical(LogicalOperation::AND, AddressingMode::ABSOLUTE_X);
                break;
            case OpCode::AND_ABSOLUTE_Y:
                logical(LogicalOperation::AND, AddressingMode::ABSOLUTE_Y);
                break;
            case OpCode::AND_INDIRECT_X:
                logical(LogicalOperation::AND, AddressingMode::INDIRECT_X);
                break;
            case OpCode::AND_INDIRECT_Y:
                logical(LogicalOperation::AND, AddressingMode::INDIRECT_Y);
                break;

            case OpCode::ASL_ACCUMULATOR:
                shift_left_accumulator();
                break;
            case OpCode::ASL_ZERO_PAGE:
                shift_left_memory(AddressingMode::ZERO_PAGE);
                break;
            case OpCode::ASL_ZERO_PAGE_X:
                shift_left_memory(AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::ASL_ABSOLUTE:
                shift_left_memory(AddressingMode::ABSOLUTE);
                break;
            case OpCode::ASL_ABSOLUTE_X:
                shift_left_memory(AddressingMode::ABSOLUTE_X);
                break;

            case OpCode::BCC:
                branch_if(CARRY, CLEAR);
                break;
            case OpCode::BCS:
                branch_if(CARRY, SET);
                break;
            case OpCode::BEQ:
                branch_if(ZERO, SET);
                break;
            case OpCode::BMI:
                branch_if(NEGATIVE, SET);
                break;
            case OpCode::BNE:
                branch_if(ZERO, CLEAR);
                break;
            case OpCode::BPL:
                branch_if(NEGATIVE, CLEAR);
                break;
            case OpCode::BVC:
                branch_if(OVERFLOW, CLEAR);
                break;
            case OpCode::BVS:
                branch_if(OVERFLOW, SET);
                break;

            case OpCode::BIT_ZERO_PAGE:
                bit_test(AddressingMode::ZERO_PAGE);
                break;
            case OpCode::BIT_ABSOLUTE:
                bit_test(AddressingMode::ABSOLUTE);
                break;

            case OpCode::BRK:
                force_interrupt();
                break;

            case OpCode::CLC:
                set_flag(CARRY, CLEAR, true);
                break;
            case OpCode::CLD:
                set_flag(DECIMAL, CLEAR, true);
                break;
            case OpCode::CLI:
                set_flag(INTERRUPT_DISABLE, CLEAR, true);
                break;
            case OpCode::CLV:
                set_flag(OVERFLOW, CLEAR, true);
                break;

            case OpCode::CMP_IMMEDIATE:
                compare_register(Register::AC, AddressingMode::IMMEDIATE);
                break;
            case OpCode::CMP_ZERO_PAGE:
                compare_register(Register::AC, AddressingMode::ZERO_PAGE);
                break;
            case OpCode::CMP_ZERO_PAGE_X:
                compare_register(Register::AC, AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::CMP_ABSOLUTE:
                compare_register(Register::AC, AddressingMode::ABSOLUTE);
                break;
            case OpCode::CMP_ABSOLUTE_X:
                compare_register(Register::AC, AddressingMode::ABSOLUTE_X);
                break;
            case OpCode::CMP_ABSOLUTE_Y:
                compare_register(Register::AC, AddressingMode::ABSOLUTE_Y);
                break;
            case OpCode::CMP_INDIRECT_X:
                compare_register(Register::AC, AddressingMode::INDIRECT_X);
                break;
            case OpCode::CMP_INDIRECT_Y:
                compare_register(Register::AC, AddressingMode::INDIRECT_Y);
                break;

            case OpCode::CPX_IMMEDIATE:
                compare_register(Register::X, AddressingMode::IMMEDIATE);
                break;
            case OpCode::CPX_ZERO_PAGE:
                compare_register(Register::X, AddressingMode::ZERO_PAGE);
                break;
            case OpCode::CPX_ABSOLUTE:
                compare_register(Register::X, AddressingMode::ABSOLUTE);
                break;

            case OpCode::CPY_IMMEDIATE:
                compare_register(Register::Y, AddressingMode::IMMEDIATE);
                break;
            case OpCode::CPY_ZERO_PAGE:
                compare_register(Register::Y, AddressingMode::ZERO_PAGE);
                break;
            case OpCode::CPY_ABSOLUTE:
                compare_register(Register::Y, AddressingMode::ABSOLUTE);
                break;

            case OpCode::DEC_ZER0_PAGE:
                decrement_memory(AddressingMode::ZERO_PAGE);
                break;
            case OpCode::DEC_ZERO_PAGE_X:
                decrement_memory(AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::DEC_ABSOLUTE:
                decrement_memory(AddressingMode::ABSOLUTE);
                break;
            case OpCode::DEC_ABSOLUTE_X:
                decrement_memory(AddressingMode::ABSOLUTE_X);
                break;

            case OpCode::DEX:
                decrement_register(Register::X);
                break;
            case OpCode::DEY:
                decrement_register(Register::Y);
                break;

            case OpCode::EOR_IMMEDIATE:
                logical(LogicalOperation::XOR, AddressingMode::IMMEDIATE);
                break;
            case OpCode::EOR_ZERO_PAGE:
                logical(LogicalOperation::XOR, AddressingMode::ZERO_PAGE);
                break;
            case OpCode::EOR_ZERO_PAGE_X:
                logical(LogicalOperation::XOR, AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::EOR_ABSOLUTE:
                logical(LogicalOperation::XOR, AddressingMode::ABSOLUTE);
                break;
            case OpCode::EOR_ABSOLUTE_X:
                logical(LogicalOperation::XOR, AddressingMode::ABSOLUTE_X);
                break;
            case OpCode::EOR_ABSOLUTE_Y:
                logical(LogicalOperation::XOR, AddressingMode::ABSOLUTE_Y);
                break;
            case OpCode::EOR_INDIRECT_X:
                logical(LogicalOperation::XOR, AddressingMode::INDIRECT_X);
                break;
            case OpCode::EOR_INDIRECT_Y:
                logical(LogicalOperation::XOR, AddressingMode::INDIRECT_Y);
                break;

            case OpCode::INC_ZERO_PAGE:
                increment_memory(AddressingMode::ZERO_PAGE);
                break;
            case OpCode::INC_ZERO_PAGE_X:
                increment_memory(AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::INC_ABSOLUTE:
                increment_memory(AddressingMode::ABSOLUTE);
                break;
            case OpCode::INC_ABSOLUTE_X:
                increment_memory(AddressingMode::ABSOLUTE_X);
                break;

            case OpCode::INX:
                increment_register(Register::X);
                break;
            case OpCode::INY:
                increment_register(Register::Y);
                break;

            case OpCode::JMP_ABSOLUTE:
                jump(AddressingMode::ABSOLUTE);
                break;
            case OpCode::JMP_INDIRECT:
                jump(AddressingMode::INDIRECT);
                break;

            case OpCode::JSR:
                jump_to_subroutine();
                break;

            case OpCode::LDA_IMMEDIATE:
                load_register(Register::AC, AddressingMode::IMMEDIATE);
                break;
            case OpCode::LDA_ZERO_PAGE:
                load_register(Register::AC, AddressingMode::ZERO_PAGE);
                break;
            case OpCode::LDA_ZERO_PAGE_X:
                load_register(Register::AC, AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::LDA_ABSOLUTE:
                load_register(Register::AC, AddressingMode::ABSOLUTE);
                break;
            case OpCode::LDA_ABSOLUTE_X:
                load_register(Register::AC, AddressingMode::ABSOLUTE_X);
                break;
            case OpCode::LDA_ABSOLUTE_Y:
                load_register(Register::AC, AddressingMode::ABSOLUTE_Y);
                break;
            case OpCode::LDA_INDIRECT_X:
                load_register(Register::AC, AddressingMode::INDIRECT_X);
                break;
            case OpCode::LDA_INDIRECT_Y:
                load_register(Register::AC, AddressingMode::INDIRECT_Y);
                break;

            case OpCode::LDX_IMMEDIATE:
                load_register(Register::X, AddressingMode::IMMEDIATE);
                break;
            case OpCode::LDX_ZERO_PAGE:
                load_register(Register::X, AddressingMode::ZERO_PAGE);
                break;
            case OpCode::LDX_ZERO_PAGE_Y:
                load_register(Register::X, AddressingMode::ZERO_PAGE_Y);
                break;
            case OpCode::LDX_ABSOLUTE:
                load_register(Register::X, AddressingMode::ABSOLUTE);
                break;
            case OpCode::LDX_ABSOLUTE_Y:
                load_register(Register::X, AddressingMode::ABSOLUTE_Y);
                break;

            case OpCode::LDY_IMMEDIATE:
                load_register(Register::Y, AddressingMode::IMMEDIATE);
                break;
            case OpCode::LDY_ZERO_PAGE:
                load_register(Register::Y, AddressingMode::ZERO_PAGE);
                break;
            case OpCode::LDY_ZERO_PAGE_X:
                load_register(Register::Y, AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::LDY_ABSOLUTE:
                load_register(Register::Y, AddressingMode::ABSOLUTE);
                break;
            case OpCode::LDY_ABSOLUTE_X:
                load_register(Register::Y, AddressingMode::ABSOLUTE_X);
                break;

            case OpCode::LSR_ACCUMULATOR:
                shift_right_accumulator();
                break;
            case OpCode::LSR_ZERO_PAGE:
                shift_right_memory(AddressingMode::ZERO_PAGE);
                break;
            case OpCode::LSR_ZERO_PAGE_X:
                shift_right_memory(AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::LSR_ABSOLUTE:
                shift_right_memory(AddressingMode::ABSOLUTE);
                break;
            case OpCode::LSR_ABSOLUTE_X:
                shift_right_memory(AddressingMode::ABSOLUTE_X);
                break;

            case OpCode::NOP:
                nop();
                break;

            case OpCode::ORA_IMMEDIATE:
                logical(LogicalOperation::OR, AddressingMode::IMMEDIATE);
                break;
            case OpCode::ORA_ZERO_PAGE:
                logical(LogicalOperation::OR, AddressingMode::ZERO_PAGE);
                break;
            case OpCode::ORA_ZERO_PAGE_X:
                logical(LogicalOperation::OR, AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::ORA_ABSOLUTE:
                logical(LogicalOperation::OR, AddressingMode::ABSOLUTE);
                break;
            case OpCode::ORA_ABSOLUTE_X:
                logical(LogicalOperation::OR, AddressingMode::ABSOLUTE_X);
                break;
            case OpCode::ORA_ABSOLUTE_Y:
                logical(LogicalOperation::OR, AddressingMode::ABSOLUTE_Y);
                break;
            case OpCode::ORA_INDIRECT_X:
                logical(LogicalOperation::OR, AddressingMode::INDIRECT_X);
                break;
            case OpCode::ORA_INDIRECT_Y:
                logical(LogicalOperation::OR, AddressingMode::INDIRECT_Y);
                break;

            case OpCode::PHA:
                push_to_stack(Register::AC);
                break;
            case OpCode::PHP:
                push_to_stack(Register::SR);
                break;

            case OpCode::PLA:
                pull_from_stack(Register::AC);
                break;
            case OpCode::PLP:
                pull_from_stack(Register::SR);
                break;

            case OpCode::ROL_ACCUMULATOR:
                rotate_left_accumulator();
                break;
            case OpCode::ROL_ZERO_PAGE:
                rotate_left_memory(AddressingMode::ZERO_PAGE);
                break;
            case OpCode::ROL_ZERO_PAGE_X:
                rotate_left_memory(AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::ROL_ABSOLUTE:
                rotate_left_memory(AddressingMode::ABSOLUTE);
                break;
            case OpCode::ROL_ABSOLUTE_X:
                rotate_left_memory(AddressingMode::ABSOLUTE_X);
                break;

            case OpCode::ROR_ACCUMULATOR:
                rotate_right_accumulator();
                break;
            case OpCode::ROR_ZERO_PAGE:
                rotate_right_memory(AddressingMode::ZERO_PAGE);
                break;
            case OpCode::ROR_ZERO_PAGE_X:
                rotate_right_memory(AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::ROR_ABSOLUTE:
                rotate_right_memory(AddressingMode::ABSOLUTE);
                break;
            case OpCode::ROR_ABSOLUTE_X:
                rotate_right_memory(AddressingMode::ABSOLUTE_X);
                break;

            case OpCode::RTI:
                return_from_interrupt();
                break;
            case OpCode::RTS:
                return_from_subroutine();
                break;

            case OpCode::SBC_IMMEDIATE:
                subtract_with_carry(AddressingMode::IMMEDIATE);
                break;
            case OpCode::SBC_ZERO_PAGE:
                subtract_with_carry(AddressingMode::ZERO_PAGE);
                break;
            case OpCode::SBC_ZERO_PAGE_X:
                subtract_with_carry(AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::SBC_ABSOLUTE:
                subtract_with_carry(AddressingMode::ABSOLUTE);
                break;
            case OpCode::SBC_ABSOLUTE_X:
                subtract_with_carry(AddressingMode::ABSOLUTE_X);
                break;
            case OpCode::SBC_ABSOLUTE_Y:
                subtract_with_carry(AddressingMode::ABSOLUTE_Y);
                break;
            case OpCode::SBC_INDIRECT_X:
                subtract_with_carry(AddressingMode::INDIRECT_X);
                break;
            case OpCode::SBC_INDIRECT_Y:
                subtract_with_carry(AddressingMode::INDIRECT_Y);
                break;

            case OpCode::SEC:
                set_flag(CARRY, SET, true);
                break;
            case OpCode::SED:
                set_flag(DECIMAL, SET, true);
                break;
            case OpCode::SEI:
                set_flag(INTERRUPT_DISABLE, SET, true);
                break;

            case OpCode::STA_ZERO_PAGE:
                store_register(Register::AC, AddressingMode::ZERO_PAGE);
                break;
            case OpCode::STA_ZERO_PAGE_X:
                store_register(Register::AC, AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::STA_ABSOLUTE:
                store_register(Register::AC, AddressingMode::ABSOLUTE);
                break;
            case OpCode::STA_ABSOLUTE_X:
                store_register(Register::AC, AddressingMode::ABSOLUTE_X);
                break;
            case OpCode::STA_ABSOLUTE_Y:
                store_register(Register::AC, AddressingMode::ABSOLUTE_Y);
                break;
            case OpCode::STA_INDIRECT_X:
                store_register(Register::AC, AddressingMode::INDIRECT_X);
                break;
            case OpCode::STA_INDIRECT_Y:
                store_register(Register::AC, AddressingMode::INDIRECT_Y);
                break;

            case OpCode::STX_ZERO_PAGE:
                store_register(Register::X, AddressingMode::ZERO_PAGE);
                break;
            case OpCode::STX_ZERO_PAGE_Y:
                store_register(Register::X, AddressingMode::ZERO_PAGE_Y);
                break;
            case OpCode::STX_ABSOLUTE:
                store_register(Register::X, AddressingMode::ABSOLUTE);
                break;

            case OpCode::STY_ZERO_PAGE:
                store_register(Register::Y, AddressingMode::ZERO_PAGE);
                break;
            case OpCode::STY_ZERO_PAGE_X:
                store_register(Register::Y, AddressingMode::ZERO_PAGE_X);
                break;
            case OpCode::STY_ABSOLUTE:
                store_register(Register::Y, AddressingMode::ABSOLUTE);
                break;

            case OpCode::TAX:
                transfer_registers(Register::AC, Register::X);
                break;
            case OpCode::TAY:
                transfer_registers(Register::AC, Register::Y);
                break;
            case OpCode::TSX:
                transfer_registers(Register::SP, Register::X);
                break;
            case OpCode::TXA:
                transfer_registers(Register::X, Register::AC);
                break;
            case OpCode::TXS:
                transfer_registers(Register::X, Register::SP);
                break;
            case OpCode::TYA:
                transfer_registers(Register::Y, Register::AC);
                break;

            default:
                std::stringstream ss;
                ss << "Illegal op code " << HEX_BYTE((int)opCode) << " at the address " << HEX_WORD(PC);
                throw std::runtime_error(ss.str());
        }
    }


    void MOS6502::execute() {
        while (true) {
            auto opCode = OpCode{(read_current_byte())};
            execute_command(opCode);
        }
    }


    void MOS6502::execute_current_command() {
        OpCode opCode{read_current_byte()};
        execute_command(opCode);
    }


    Word MOS6502::add_word(Word word, Byte byte) {
        WordToBytes buf(word);
        bool carry = false;
        buf.low = add_bytes(buf.low, byte, carry);
        if (carry) cycle++;
        buf.high = add_bytes(buf.high, 0, carry);
        return buf.word;
    }

    Byte MOS6502::add_bytes(Byte target, Byte other, bool &carry) {
        int result = target + other + carry;
        carry = result > UINT8_MAX;
        return result;
    }

    Word MOS6502::read_zero_page_address_and_add(std::optional<Byte> shift) {
        Byte address = read_current_byte();
        if (shift.has_value()) {
            address += shift.value();
            cycle++;
        }
        return address;
    }


}






