//
// Created by Mikhail on 28/08/2023.
//

#include <sstream>
#include <iomanip>
#include <bitset>
#include <iostream>
#include <utility>
#include <cassert>
#include <format>

#include "MOS6502.hpp"
#include "MOS6502_helpers.hpp"



namespace Emulator {

    void MOS6502::load_register(Register reg, AddressingMode mode) {
        set_register(reg, read_byte(mode));
    }


    void MOS6502::set_register(Register reg, Byte value) {
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
        set_writing_flags(value);
    }


    std::string MOS6502::dump(bool include_memory) const {
        auto result = std::vformat("Registers: AC = {:u}, X = {:u}, Y = {:u}\n", std::make_format_args(AC, X, Y));
        result += std::vformat("Program counter = {:#04x}, Stack pointer = {:#02x}\n", std::make_format_args(PC, SP));
        result += std::vformat("Flags: {}\n", std::make_format_args(SR.to_string()));
        result += std::vformat("Current cycle = {:u}\n", std::make_format_args(cycle));

        result += "Zero page: ";
        for (int i = 0; i <= UINT8_MAX; i++) result += std::vformat("{:#02x} ", std::make_format_args(memory[i]));

        result += "\nStack: ";
        for (int i = 0; i <= UINT8_MAX; i++) result += std::vformat("{:#02x} ", std::make_format_args(memory[STACK_BOTTOM + i]));

        result += std::vformat("\nSpecial addresses:\n\tnon-maskable interrupt handler = {:#04x}\n\tpower on reset location = {:#04x}\n\tBRK/interrupt request handler = {#04x}\n",
                               std::make_format_args(memory.get_word(INTERRUPT_HANDLER), memory.get_word(RESET_LOCATION), memory.get_word(BRK_HANDLER)));

        if (include_memory) {
            result += "Remaining memory:\n";
            for (int i = STACK_BOTTOM + UINT8_MAX + 1; i <= UINT16_MAX; i++) result += std::vformat("{:#02} ", std::make_format_args(memory[i]));
            result += '\n';
        }

        return result + "END OF DUMP.";
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
                return SR.to_ulong();
        }

        throw std::runtime_error("Unknown register");
    }


    void MOS6502::store_register(Register reg, AddressingMode mode) {
        auto value = get_register(reg);
        write_byte(mode, value, true);
    }


    Word MOS6502::determine_address(AddressingMode mode, bool elapseCycleWhenNotCrossingPage) {
        switch (mode) {
            case AddressingMode::IMPLICIT:
            case AddressingMode::ACCUMULATOR:
                return 0;

            case AddressingMode::IMMEDIATE:
                return PC;

            case AddressingMode::ZERO_PAGE:
                return memory.fetch_byte_and_proceed(PC, cycle);

            case AddressingMode::ZERO_PAGE_X:
                return add_word(memory.fetch_byte_and_proceed(PC, cycle), X, true);

            case AddressingMode::ZERO_PAGE_Y:
                return add_word(memory.fetch_byte_and_proceed(PC, cycle), Y, true);

            case AddressingMode::RELATIVE: {
                const char offset = (char)memory.fetch_byte_and_proceed(PC, cycle);
                return PC + offset;
            }

            case AddressingMode::ABSOLUTE:
                return memory.fetch_word_and_proceed(PC, cycle);

            case AddressingMode::ABSOLUTE_X:
                return add_word(memory.fetch_word_and_proceed(PC, cycle), X, elapseCycleWhenNotCrossingPage);

            case AddressingMode::ABSOLUTE_Y:
                return add_word(memory.fetch_word_and_proceed(PC, cycle), Y, elapseCycleWhenNotCrossingPage);

            case AddressingMode::INDIRECT: {
                auto tmpAddress = memory.fetch_word_and_proceed(PC, cycle);
                return memory.fetch_word(tmpAddress, cycle);
            }

            case AddressingMode::INDIRECT_X: {
                cycle++;
                return memory.fetch_word((Byte)(memory.fetch_byte_and_proceed(PC, cycle) + X), cycle);
            }

            case AddressingMode::INDIRECT_Y:
                return add_word(memory.fetch_word((Word)memory.fetch_byte_and_proceed(PC, cycle), cycle), Y, elapseCycleWhenNotCrossingPage);
        }

        throw std::runtime_error("Some addressing modes were not handled");
    }


    void MOS6502::push_to_stack(Register reg) {
        push_byte_to_stack(get_register(reg));
        cycle++;
    }


    void MOS6502::push_byte_to_stack(Byte value) {
        memory.set_byte({.address = (Word)(STACK_BOTTOM + SP--), .value = value, .cycle = cycle});
    }


    Byte MOS6502::pull_byte_from_stack() {
        cycle++;
        return memory.fetch_byte(STACK_BOTTOM + ++SP, cycle);
    }


    void MOS6502::pull_from_stack(Register to) {
        set_register(to, pull_byte_from_stack());
        cycle++;
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

        set_register(Register::AC, result);
    }


    void MOS6502::bit_test(AddressingMode mode) {
        Byte memory_byte = read_byte(mode);
        Byte test_result = AC & memory_byte;

        SR[ZERO] = test_result == 0;

        SR[OVERFLOW_F] = get_bit(memory_byte, OVERFLOW_F);
        SR[NEGATIVE] = get_bit(memory_byte, NEGATIVE);
    }


    void MOS6502::add_with_carry(AddressingMode mode) {
        const bool initialACSignBit = get_bit(AC, NEGATIVE);

        const Byte mem = read_byte(mode);
        const bool initialMemSignBit = get_bit(mem, NEGATIVE);

        bool carry = SR[CARRY];

        if (verbose) std::cout << "Add with carry: AC = " << (int)AC << ", memory = " << (int)mem << ", carry = " << carry << "\n";

        set_register(Register::AC, add_bytes(AC, mem, carry));
        const bool resultSignBit = get_bit(AC, NEGATIVE);

        SR[CARRY] = carry;
        // overflow flag is set every time when the sign of the result is incorrect
        SR[OVERFLOW_F] = (initialACSignBit == initialMemSignBit) && (initialACSignBit != resultSignBit);
    }


    void MOS6502::subtract_with_carry(AddressingMode mode) {
        const bool initial_sign_bit = get_bit(AC, NEGATIVE);

        const Byte rhs = read_byte(mode);
        const bool rhs_sign_bit = get_bit(rhs, NEGATIVE);

        bool carry = SR[CARRY];
        set_register(Register::AC, subtract_bytes(AC, rhs, carry));
        const bool resulting_sign_bit = get_bit(AC, NEGATIVE);

        // carry flag is set only when the result crossed 0
        SR[CARRY] = carry;
        // overflow flag is set every time when the sign of the result is incorrect
        SR[OVERFLOW_F] = (initial_sign_bit != rhs_sign_bit) && (initial_sign_bit != resulting_sign_bit);
    }


    void MOS6502::compare_register(Register reg, AddressingMode mode) {
        Byte reg_value = get_register(reg);
        Byte memory_value = read_byte(mode);

        if (verbose) std::cout << "Comparing " << (int)reg_value << " to " << (int)memory_value;

        SR[CARRY] = reg_value >= memory_value;
        SR[ZERO] = reg_value == memory_value;
        SR[NEGATIVE] = reg_value < memory_value;

        if (verbose) std::cout << ", resulting flags " << SR << '\n';
    }


    void MOS6502::increment_memory(AddressingMode mode) {
        const Word address = determine_address(mode, true);
        auto value = memory.fetch_byte(address, cycle);
        Byte newValue = value + 1;

        set_writing_flags(newValue);
        memory.set_byte({.address = address, .value = newValue, .cycle = cycle});

        cycle++;
    }


    void MOS6502::decrement_memory(AddressingMode mode) {
        const Word address = determine_address(mode, true);
        Byte value = memory.fetch_byte(address, cycle);
        Byte newValue = value - 1;

        set_writing_flags(newValue);
        memory.set_byte({.address = address, .value = newValue, .cycle = cycle});
        cycle++;
    }


    void MOS6502::increment_register(Register reg) {
        set_register(reg, get_register(reg) + 1);
        cycle++;
    }


    void MOS6502::decrement_register(Register reg) {
        set_register(reg, get_register(reg) - 1);
        cycle++;
    }


    void MOS6502::shift_left_accumulator() {
        SR[CARRY] = get_bit(AC, 7);
        set_register(Register::AC, AC << 1);
        cycle++;
    }


    void MOS6502::shift_left_memory(AddressingMode mode) {
        Word address = determine_address(mode, true);
        Byte value = memory.fetch_byte(address, cycle);

        SR[CARRY] = get_bit(value, 7);

        Byte newValue = value << 1;
        set_writing_flags(newValue);
        memory.set_byte({.address = address, .value = newValue, .cycle = cycle});
        cycle++;
    }


    void MOS6502::shift_right_accumulator() {
        SR[CARRY] = get_bit(AC, 0);
        set_register(Register::AC, AC >> 1);
        cycle++;
    }


    void MOS6502::shift_right_memory(AddressingMode mode) {
        Word address = determine_address(mode, true);
        Byte value = memory.fetch_byte(address, cycle);
        Byte newValue = value >> 1;

        SR[CARRY] = get_bit(value, 0);
        set_writing_flags(newValue);
        memory.set_byte({.address = address, .value = newValue, .cycle = cycle});
        cycle++;
    }


    void MOS6502::rotate_left_accumulator() {
        const bool newCarry = get_bit(AC, 7);
        set_register(Register::AC, (AC << 1) + SR[CARRY]);
        SR[CARRY] = newCarry;
        cycle++;
    }


    void MOS6502::rotate_left_memory(AddressingMode mode) {
        Word address = determine_address(mode, true);
        Byte value = memory.fetch_byte(address, cycle);
        Byte newValue = (value << 1) + SR[CARRY];

        SR[CARRY] = get_bit(value, 7);
        set_writing_flags(newValue);
        memory.set_byte({.address = address, .value = newValue, .cycle = cycle});
        cycle++;
    }


    void MOS6502::rotate_right_accumulator() {
        Byte value = AC;
        value >>= 1;
        set_bit(value, 7, SR[CARRY]);

        const bool newCarry = get_bit(AC, 0);
        set_register(Register::AC, value);
        SR[CARRY] = newCarry;
        cycle++;
    }


    void MOS6502::rotate_right_memory(AddressingMode mode) {
        Word address = determine_address(mode, true);
        Byte value = memory.fetch_byte(address, cycle);
        const bool newCarry = get_bit(value, 0);
        value >>= 1;
        set_bit(value, 7, SR[CARRY]);

        SR[CARRY] = newCarry;
        set_writing_flags(value);
        memory.set_byte({.address = address, .value = value, .cycle = cycle});
        cycle++;
    }


    void MOS6502::jump(AddressingMode mode) {
        PC = determine_address(mode);
    }


    void MOS6502::jump_to_subroutine() {
        Word targetAddress = determine_address(AddressingMode::ABSOLUTE);
        push_word_to_stack(PC - 1);
        PC = targetAddress;
        cycle++;
    }


    void MOS6502::return_from_subroutine() {
        PC = pull_word_from_stack() + 1;
        cycle++;
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


    void MOS6502::branch_if(Flag flag_to_check, bool value_to_expect) {
//        const Word initialAddress = PC;
        Word new_address = determine_address(AddressingMode::RELATIVE);

        if (verbose) std::cout << "Branching on " << flag_to_check << ' ' << value_to_expect;

        if (SR[flag_to_check] == value_to_expect) {
            PC = new_address;

            // elapse cycle for successful branching
            cycle++;
            // TODO: find out if this is really necessary; if yes, then does page crossing counts from the initialPC or from initialPC + 2
            // elapse cycle if page crossed
//            if (WordToBytes(initialAddress).high != WordToBytes(new_address).high) cycle++;
        }
        else if (verbose) std::cout << ": failed\n";
    }


    void MOS6502::brk() {
        // it stores initial PC + 2, where initial is the address of the command
        push_word_to_stack(PC + 1);
        PC = memory.fetch_word(BRK_HANDLER, cycle);
        SR[BREAK] = SET;
        cycle++;
    }


    void MOS6502::return_from_interrupt() {
        SR = pull_byte_from_stack();
        PC = pull_word_from_stack();
        cycle--;
    }


    void MOS6502::reset() {
        PC = memory.fetch_word(RESET_LOCATION, cycle);
        cycle = 7;
        SR[INTERRUPT_DISABLE] = true;
    }



    Byte MOS6502::read_byte(AddressingMode mode) {
        return (mode == AddressingMode::IMMEDIATE) ? memory.fetch_byte_and_proceed(PC, cycle) : memory.fetch_byte(determine_address(mode), cycle);
    }



    void MOS6502::execute_command(OpCode opCode) {
        switch (opCode) {
            case OpCode::ADC_IMMEDIATE:
                add_with_carry(AddressingMode::IMMEDIATE);
                return;
            case OpCode::ADC_ZERO_PAGE:
                add_with_carry(AddressingMode::ZERO_PAGE);
                return;
            case OpCode::ADC_ZERO_PAGE_X:
                add_with_carry(AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::ADC_ABSOLUTE:
                add_with_carry(AddressingMode::ABSOLUTE);
                return;
            case OpCode::ADC_ABSOLUTE_X:
                add_with_carry(AddressingMode::ABSOLUTE_X);
                return;
            case OpCode::ADC_ABSOLUTE_Y:
                add_with_carry(AddressingMode::ABSOLUTE_Y);
                return;
            case OpCode::ADC_INDIRECT_X:
                add_with_carry(AddressingMode::INDIRECT_X);
                return;
            case OpCode::ADC_INDIRECT_Y:
                add_with_carry(AddressingMode::INDIRECT_Y);
                return;

            case OpCode::AND_IMMEDIATE:
                logical(LogicalOperation::AND, AddressingMode::IMMEDIATE);
                return;
            case OpCode::AND_ZERO_PAGE:
                logical(LogicalOperation::AND, AddressingMode::ZERO_PAGE);
                return;
            case OpCode::AND_ZERO_PAGE_X:
                logical(LogicalOperation::AND, AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::AND_ABSOLUTE:
                logical(LogicalOperation::AND, AddressingMode::ABSOLUTE);
                return;
            case OpCode::AND_ABSOLUTE_X:
                logical(LogicalOperation::AND, AddressingMode::ABSOLUTE_X);
                return;
            case OpCode::AND_ABSOLUTE_Y:
                logical(LogicalOperation::AND, AddressingMode::ABSOLUTE_Y);
                return;
            case OpCode::AND_INDIRECT_X:
                logical(LogicalOperation::AND, AddressingMode::INDIRECT_X);
                return;
            case OpCode::AND_INDIRECT_Y:
                logical(LogicalOperation::AND, AddressingMode::INDIRECT_Y);
                return;

            case OpCode::ASL_ACCUMULATOR:
                shift_left_accumulator();
                return;
            case OpCode::ASL_ZERO_PAGE:
                shift_left_memory(AddressingMode::ZERO_PAGE);
                return;
            case OpCode::ASL_ZERO_PAGE_X:
                shift_left_memory(AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::ASL_ABSOLUTE:
                shift_left_memory(AddressingMode::ABSOLUTE);
                return;
            case OpCode::ASL_ABSOLUTE_X:
                shift_left_memory(AddressingMode::ABSOLUTE_X);
                return;

            case OpCode::BCC_RELATIVE:
                branch_if(CARRY, CLEAR);
                return;
            case OpCode::BCS_RELATIVE:
                branch_if(CARRY, SET);
                return;
            case OpCode::BEQ_RELATIVE:
                branch_if(ZERO, SET);
                return;
            case OpCode::BMI_RELATIVE:
                branch_if(NEGATIVE, SET);
                return;
            case OpCode::BNE_RELATIVE:
                branch_if(ZERO, CLEAR);
                return;
            case OpCode::BPL_RELATIVE:
                branch_if(NEGATIVE, CLEAR);
                return;
            case OpCode::BVC_RELATIVE:
                branch_if(OVERFLOW_F, CLEAR);
                return;
            case OpCode::BVS_RELATIVE:
                branch_if(OVERFLOW_F, SET);
                return;

            case OpCode::BIT_ZERO_PAGE:
                bit_test(AddressingMode::ZERO_PAGE);
                return;
            case OpCode::BIT_ABSOLUTE:
                bit_test(AddressingMode::ABSOLUTE);
                return;

            case OpCode::BRK_IMPLICIT:
                brk();
                return;

            case OpCode::CLC_IMPLICIT:
                SR[CARRY] = CLEAR;
                return;
            case OpCode::CLD_IMPLICIT:
                SR[DECIMAL] = CLEAR;
                return;
            case OpCode::CLI_IMPLICIT:
                SR[INTERRUPT_DISABLE] = CLEAR;
                return;
            case OpCode::CLV_IMPLICIT:
                SR[OVERFLOW_F] = CLEAR;
                return;

            case OpCode::CMP_IMMEDIATE:
                compare_register(Register::AC, AddressingMode::IMMEDIATE);
                return;
            case OpCode::CMP_ZERO_PAGE:
                compare_register(Register::AC, AddressingMode::ZERO_PAGE);
                return;
            case OpCode::CMP_ZERO_PAGE_X:
                compare_register(Register::AC, AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::CMP_ABSOLUTE:
                compare_register(Register::AC, AddressingMode::ABSOLUTE);
                return;
            case OpCode::CMP_ABSOLUTE_X:
                compare_register(Register::AC, AddressingMode::ABSOLUTE_X);
                return;
            case OpCode::CMP_ABSOLUTE_Y:
                compare_register(Register::AC, AddressingMode::ABSOLUTE_Y);
                return;
            case OpCode::CMP_INDIRECT_X:
                compare_register(Register::AC, AddressingMode::INDIRECT_X);
                return;
            case OpCode::CMP_INDIRECT_Y:
                compare_register(Register::AC, AddressingMode::INDIRECT_Y);
                return;

            case OpCode::CPX_IMMEDIATE:
                compare_register(Register::X, AddressingMode::IMMEDIATE);
                return;
            case OpCode::CPX_ZERO_PAGE:
                compare_register(Register::X, AddressingMode::ZERO_PAGE);
                return;
            case OpCode::CPX_ABSOLUTE:
                compare_register(Register::X, AddressingMode::ABSOLUTE);
                return;

            case OpCode::CPY_IMMEDIATE:
                compare_register(Register::Y, AddressingMode::IMMEDIATE);
                return;
            case OpCode::CPY_ZERO_PAGE:
                compare_register(Register::Y, AddressingMode::ZERO_PAGE);
                return;
            case OpCode::CPY_ABSOLUTE:
                compare_register(Register::Y, AddressingMode::ABSOLUTE);
                return;

            case OpCode::DEC_ZERO_PAGE:
                decrement_memory(AddressingMode::ZERO_PAGE);
                return;
            case OpCode::DEC_ZERO_PAGE_X:
                decrement_memory(AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::DEC_ABSOLUTE:
                decrement_memory(AddressingMode::ABSOLUTE);
                return;
            case OpCode::DEC_ABSOLUTE_X:
                decrement_memory(AddressingMode::ABSOLUTE_X);
                return;

            case OpCode::DEX_IMPLICIT:
                decrement_register(Register::X);
                return;
            case OpCode::DEY_IMPLICIT:
                decrement_register(Register::Y);
                return;

            case OpCode::EOR_IMMEDIATE:
                logical(LogicalOperation::XOR, AddressingMode::IMMEDIATE);
                return;
            case OpCode::EOR_ZERO_PAGE:
                logical(LogicalOperation::XOR, AddressingMode::ZERO_PAGE);
                return;
            case OpCode::EOR_ZERO_PAGE_X:
                logical(LogicalOperation::XOR, AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::EOR_ABSOLUTE:
                logical(LogicalOperation::XOR, AddressingMode::ABSOLUTE);
                return;
            case OpCode::EOR_ABSOLUTE_X:
                logical(LogicalOperation::XOR, AddressingMode::ABSOLUTE_X);
                return;
            case OpCode::EOR_ABSOLUTE_Y:
                logical(LogicalOperation::XOR, AddressingMode::ABSOLUTE_Y);
                return;
            case OpCode::EOR_INDIRECT_X:
                logical(LogicalOperation::XOR, AddressingMode::INDIRECT_X);
                return;
            case OpCode::EOR_INDIRECT_Y:
                logical(LogicalOperation::XOR, AddressingMode::INDIRECT_Y);
                return;

            case OpCode::INC_ZERO_PAGE:
                increment_memory(AddressingMode::ZERO_PAGE);
                return;
            case OpCode::INC_ZERO_PAGE_X:
                increment_memory(AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::INC_ABSOLUTE:
                increment_memory(AddressingMode::ABSOLUTE);
                return;
            case OpCode::INC_ABSOLUTE_X:
                increment_memory(AddressingMode::ABSOLUTE_X);
                return;

            case OpCode::INX_IMPLICIT:
                increment_register(Register::X);
                return;
            case OpCode::INY_IMPLICIT:
                increment_register(Register::Y);
                return;

            case OpCode::JMP_ABSOLUTE:
                jump(AddressingMode::ABSOLUTE);
                return;
            case OpCode::JMP_INDIRECT:
                jump(AddressingMode::INDIRECT);
                return;

            case OpCode::JSR_ABSOLUTE:
                jump_to_subroutine();
                return;

            case OpCode::LDA_IMMEDIATE:
                load_register(Register::AC, AddressingMode::IMMEDIATE);
                return;
            case OpCode::LDA_ZERO_PAGE:
                load_register(Register::AC, AddressingMode::ZERO_PAGE);
                return;
            case OpCode::LDA_ZERO_PAGE_X:
                load_register(Register::AC, AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::LDA_ABSOLUTE:
                load_register(Register::AC, AddressingMode::ABSOLUTE);
                return;
            case OpCode::LDA_ABSOLUTE_X:
                load_register(Register::AC, AddressingMode::ABSOLUTE_X);
                return;
            case OpCode::LDA_ABSOLUTE_Y:
                load_register(Register::AC, AddressingMode::ABSOLUTE_Y);
                return;
            case OpCode::LDA_INDIRECT_X:
                load_register(Register::AC, AddressingMode::INDIRECT_X);
                return;
            case OpCode::LDA_INDIRECT_Y:
                load_register(Register::AC, AddressingMode::INDIRECT_Y);
                return;

            case OpCode::LDX_IMMEDIATE:
                load_register(Register::X, AddressingMode::IMMEDIATE);
                return;
            case OpCode::LDX_ZERO_PAGE:
                load_register(Register::X, AddressingMode::ZERO_PAGE);
                return;
            case OpCode::LDX_ZERO_PAGE_Y:
                load_register(Register::X, AddressingMode::ZERO_PAGE_Y);
                return;
            case OpCode::LDX_ABSOLUTE:
                load_register(Register::X, AddressingMode::ABSOLUTE);
                return;
            case OpCode::LDX_ABSOLUTE_Y:
                load_register(Register::X, AddressingMode::ABSOLUTE_Y);
                return;

            case OpCode::LDY_IMMEDIATE:
                load_register(Register::Y, AddressingMode::IMMEDIATE);
                return;
            case OpCode::LDY_ZERO_PAGE:
                load_register(Register::Y, AddressingMode::ZERO_PAGE);
                return;
            case OpCode::LDY_ZERO_PAGE_X:
                load_register(Register::Y, AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::LDY_ABSOLUTE:
                load_register(Register::Y, AddressingMode::ABSOLUTE);
                return;
            case OpCode::LDY_ABSOLUTE_X:
                load_register(Register::Y, AddressingMode::ABSOLUTE_X);
                return;

            case OpCode::LSR_ACCUMULATOR:
                shift_right_accumulator();
                return;
            case OpCode::LSR_ZERO_PAGE:
                shift_right_memory(AddressingMode::ZERO_PAGE);
                return;
            case OpCode::LSR_ZERO_PAGE_X:
                shift_right_memory(AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::LSR_ABSOLUTE:
                shift_right_memory(AddressingMode::ABSOLUTE);
                return;
            case OpCode::LSR_ABSOLUTE_X:
                shift_right_memory(AddressingMode::ABSOLUTE_X);
                return;

            case OpCode::NOP_IMPLICIT:
                nop();
                return;

            case OpCode::ORA_IMMEDIATE:
                logical(LogicalOperation::OR, AddressingMode::IMMEDIATE);
                return;
            case OpCode::ORA_ZERO_PAGE:
                logical(LogicalOperation::OR, AddressingMode::ZERO_PAGE);
                return;
            case OpCode::ORA_ZERO_PAGE_X:
                logical(LogicalOperation::OR, AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::ORA_ABSOLUTE:
                logical(LogicalOperation::OR, AddressingMode::ABSOLUTE);
                return;
            case OpCode::ORA_ABSOLUTE_X:
                logical(LogicalOperation::OR, AddressingMode::ABSOLUTE_X);
                return;
            case OpCode::ORA_ABSOLUTE_Y:
                logical(LogicalOperation::OR, AddressingMode::ABSOLUTE_Y);
                return;
            case OpCode::ORA_INDIRECT_X:
                logical(LogicalOperation::OR, AddressingMode::INDIRECT_X);
                return;
            case OpCode::ORA_INDIRECT_Y:
                logical(LogicalOperation::OR, AddressingMode::INDIRECT_Y);
                return;

            case OpCode::PHA_IMPLICIT:
                push_to_stack(Register::AC);
                return;
            case OpCode::PHP_IMPLICIT:
                push_to_stack(Register::SR);
                return;

            case OpCode::PLA_IMPLICIT:
                pull_from_stack(Register::AC);
                return;
            case OpCode::PLP_IMPLICIT:
                pull_from_stack(Register::SR);
                return;

            case OpCode::ROL_ACCUMULATOR:
                rotate_left_accumulator();
                return;
            case OpCode::ROL_ZERO_PAGE:
                rotate_left_memory(AddressingMode::ZERO_PAGE);
                return;
            case OpCode::ROL_ZERO_PAGE_X:
                rotate_left_memory(AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::ROL_ABSOLUTE:
                rotate_left_memory(AddressingMode::ABSOLUTE);
                return;
            case OpCode::ROL_ABSOLUTE_X:
                rotate_left_memory(AddressingMode::ABSOLUTE_X);
                return;

            case OpCode::ROR_ACCUMULATOR:
                rotate_right_accumulator();
                return;
            case OpCode::ROR_ZERO_PAGE:
                rotate_right_memory(AddressingMode::ZERO_PAGE);
                return;
            case OpCode::ROR_ZERO_PAGE_X:
                rotate_right_memory(AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::ROR_ABSOLUTE:
                rotate_right_memory(AddressingMode::ABSOLUTE);
                return;
            case OpCode::ROR_ABSOLUTE_X:
                rotate_right_memory(AddressingMode::ABSOLUTE_X);
                return;

            case OpCode::RTI_IMPLICIT:
                return_from_interrupt();
                return;
            case OpCode::RTS_IMPLICIT:
                return_from_subroutine();
                return;

            case OpCode::SBC_IMMEDIATE:
                subtract_with_carry(AddressingMode::IMMEDIATE);
                return;
            case OpCode::SBC_ZERO_PAGE:
                subtract_with_carry(AddressingMode::ZERO_PAGE);
                return;
            case OpCode::SBC_ZERO_PAGE_X:
                subtract_with_carry(AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::SBC_ABSOLUTE:
                subtract_with_carry(AddressingMode::ABSOLUTE);
                return;
            case OpCode::SBC_ABSOLUTE_X:
                subtract_with_carry(AddressingMode::ABSOLUTE_X);
                return;
            case OpCode::SBC_ABSOLUTE_Y:
                subtract_with_carry(AddressingMode::ABSOLUTE_Y);
                return;
            case OpCode::SBC_INDIRECT_X:
                subtract_with_carry(AddressingMode::INDIRECT_X);
                return;
            case OpCode::SBC_INDIRECT_Y:
                subtract_with_carry(AddressingMode::INDIRECT_Y);
                return;

            case OpCode::SEC_IMPLICIT:
                SR[CARRY] = SET;
                return;
            case OpCode::SED_IMPLICIT:
                SR[DECIMAL] = SET;
                return;
            case OpCode::SEI_IMPLICIT:
                SR[INTERRUPT_DISABLE] = SET;
                return;

            case OpCode::STA_ZERO_PAGE:
                store_register(Register::AC, AddressingMode::ZERO_PAGE);
                return;
            case OpCode::STA_ZERO_PAGE_X:
                store_register(Register::AC, AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::STA_ABSOLUTE:
                store_register(Register::AC, AddressingMode::ABSOLUTE);
                return;
            case OpCode::STA_ABSOLUTE_X:
                store_register(Register::AC, AddressingMode::ABSOLUTE_X);
                return;
            case OpCode::STA_ABSOLUTE_Y:
                store_register(Register::AC, AddressingMode::ABSOLUTE_Y);
                return;
            case OpCode::STA_INDIRECT_X:
                store_register(Register::AC, AddressingMode::INDIRECT_X);
                return;
            case OpCode::STA_INDIRECT_Y:
                store_register(Register::AC, AddressingMode::INDIRECT_Y);
                return;

            case OpCode::STX_ZERO_PAGE:
                store_register(Register::X, AddressingMode::ZERO_PAGE);
                return;
            case OpCode::STX_ZERO_PAGE_Y:
                store_register(Register::X, AddressingMode::ZERO_PAGE_Y);
                return;
            case OpCode::STX_ABSOLUTE:
                store_register(Register::X, AddressingMode::ABSOLUTE);
                return;

            case OpCode::STY_ZERO_PAGE:
                store_register(Register::Y, AddressingMode::ZERO_PAGE);
                return;
            case OpCode::STY_ZERO_PAGE_X:
                store_register(Register::Y, AddressingMode::ZERO_PAGE_X);
                return;
            case OpCode::STY_ABSOLUTE:
                store_register(Register::Y, AddressingMode::ABSOLUTE);
                return;

            case OpCode::TAX_IMPLICIT:
                transfer_registers(Register::AC, Register::X);
                return;
            case OpCode::TAY_IMPLICIT:
                transfer_registers(Register::AC, Register::Y);
                return;
            case OpCode::TSX_IMPLICIT:
                transfer_registers(Register::SP, Register::X);
                return;
            case OpCode::TXA_IMPLICIT:
                transfer_registers(Register::X, Register::AC);
                return;
            case OpCode::TXS_IMPLICIT:
                transfer_registers(Register::X, Register::SP);
                return;
            case OpCode::TYA_IMPLICIT:
                transfer_registers(Register::Y, Register::AC);
                return;
        }

        throw std::runtime_error(std::vformat("Illegal op code {:#02x} at the address {:#04x}", std::make_format_args(std::to_underlying(opCode), PC)));
    }


    void MOS6502::execute(bool stopOnBRK) {
        while (true) {
            auto opCode = execute_current_command();

            if (verbose) std::cout << "AC = " << (int)AC << ", X = " << (int)X << ", Y = " << (int)Y << "; flags = " << SR << '\n';

            if (stopOnBRK && opCode == BRK_IMPLICIT) return;
        }
    }


    OpCode MOS6502::execute_current_command() {
        OpCode opCode{memory.fetch_byte_and_proceed(PC, cycle)};
        execute_command(opCode);
        return opCode;
    }


    Word MOS6502::add_word(Word word, Byte byte, bool takeCycleIfLowNotOverflowed) {
        WordToBytes buf(word);
        bool carry = false;
        buf.low = add_bytes(buf.low, byte, carry);
        if (carry || takeCycleIfLowNotOverflowed) cycle++;
        buf.high = add_bytes(buf.high, 0, carry);
        return buf.word;
    }

    Byte MOS6502::add_bytes(Byte target, Byte other, bool &carry) {
        int result = target + other + carry;
        carry = result > UINT8_MAX;
        return result;
    }

    Byte MOS6502::subtract_bytes(Byte target, Byte other, bool &carry) {
        int result = target - other - !carry;
        carry = result >= 0;
        return result;
    }

    Byte &MOS6502::operator[](const Location &address) {
        if (const auto memoryAddress = std::get_if<Word>(&address)) return memory[*memoryAddress];
        if (const auto reg = std::get_if<Register>(&address))
            switch (*reg) {
            case Register::AC: return AC;
            case Register::X: return X;
                case Register::Y: return Y;
                case Register::SP: return SP;
                case Register::SR: throw std::invalid_argument("SR cannot be set via subscript operator");
            }
        std::unreachable();
    }

    Byte MOS6502::operator[](const Location &address) const {
        if (const auto memoryAddress = std::get_if<Word>(&address)) return memory[*memoryAddress];
        if (const auto reg = std::get_if<Register>(&address)) return get_register(*reg);
        std::unreachable();
    }

    void MOS6502::transfer_registers(Register from, Register to) {
        cycle++;
        set_register(to, get_register(from));
    }

    void MOS6502::set_writing_flags(Byte value) {
        SR[ZERO] = value == 0;
        SR[NEGATIVE] = (char)value < 0;
    }

    void MOS6502::write_byte(AddressingMode mode, Byte value, bool elapseCycleWhenNotCrossingPage) {
        assert(mode != AddressingMode::IMMEDIATE);
        memory.set_byte(ROM::SetByteInputAddressNotModified{.address = determine_address(mode, elapseCycleWhenNotCrossingPage), .value = value, .cycle = cycle});
    }

}






