//
// Created by Mikhail on 28/08/2023.
//

#include <bitset>
#include <utility>
#include <format>

#include "MOS6502.hpp"



namespace Emulator {


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
        auto result = std::vformat("Registers: AC = {:d}, X = {:d}, Y = {:d}\n", std::make_format_args(AC, X, Y));
        result += std::vformat("Program counter = {:#04x}, Stack pointer = {:#02x}\n", std::make_format_args(PC, SP));
        result += std::vformat("Flags: {}\n", std::make_format_args(SR.to_string()));
        result += std::vformat("Current cycle = {:d}\n", std::make_format_args(cycle));

        result += "Zero page: ";
        for (int i = 0; i <= UINT8_MAX; i++) result += std::vformat("{:#02x} ", std::make_format_args(memory[i]));

        result += "\nStack: ";
        for (int i = 0; i <= UINT8_MAX; i++) result += std::vformat("{:#02x} ", std::make_format_args(memory.stack(i)));

        result += std::vformat("\nSpecial addresses:\n\tnon-maskable interrupt handler = {:#04x}\n\tpower on reset location = {:#04x}\n\tBRK/interrupt request handler = {:#04x}\n",
                               std::make_format_args(memory.get_word(ROM::INTERRUPT_HANDLER), memory.get_word(ROM::RESET_LOCATION), memory.get_word(ROM::BRK_HANDLER)));

        if (include_memory) {
            result += "Remaining memory:\n";
            for (int i = 0x1000; i <= UINT16_MAX; i++)
                if (!memory.is_in_stack(i)) result += std::vformat("{:#02x} ", std::make_format_args(memory[i]));
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
                return SR.to_byte();
        }

        throw std::runtime_error("Unknown register");
    }



    void MOS6502::push_byte_to_stack(Byte value) {
        cycle++;
        memory.stack(SP--) = value;
    }


    Byte MOS6502::pull_byte_from_stack() {
        cycle += 2;
        return memory.stack(++SP);
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


    void MOS6502::reset() {
        PC = fetch_word(ROM::RESET_LOCATION);
        cycle = 7;
        SR[Flag::INTERRUPT_DISABLE] = true;
    }


    std::expected<MOS6502::SuccessfulTermination, MOS6502::ErrorTermination> MOS6502::execute() {
        size_t commandsExecuted = 0;
        while (true) {
            Word commandAddress = PC;

            // if there's no value of maxNumberOfCommandsToExecute, we should discard this condition
            if (commandsExecuted == maxNumberOfCommandsToExecute.value_or(commandsExecuted + 1))
                return StopOnMaxReached{.address = commandAddress};

            if (auto operation = fetch_operation(); operation.has_value()) {
                if (stopOnBRK && std::holds_alternative<BRK>(operation.value())) return StopOnBreak{.address = commandAddress};

                execute(operation.value());
            }
            else return std::unexpected(UnknownOperation{.address = commandAddress});

            commandsExecuted++;
        }
    }

    void MOS6502::set_writing_flags(Byte value) {
        SR[Flag::ZERO] = value == 0;
        SR[Flag::NEGATIVE] = (char)value < 0;
    }

    void MOS6502::execute(const Operation &operation) noexcept {
        std::visit(Overload {
                [this](ADC_Immediate op)   { add_to_accumulator(op.value); },
                [this](ADC_ZeroPage op)    { add_to_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE)); },
                [this](ADC_ZeroPageX op)   { add_to_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); },
                [this](ADC_Absolute op)    { add_to_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE)); },
                [this](ADC_AbsoluteX op)   { add_to_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_X)); },
                [this](ADC_AbsoluteY op)   { add_to_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); },
                [this](ADC_IndirectX op)   { add_to_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_X)); },
                [this](ADC_IndirectY op)   { add_to_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_Y)); },

                [this](AND_Immediate op)   { and_with_accumulator(op.value); },
                [this](AND_ZeroPage op)    { and_with_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE)); },
                [this](AND_ZeroPageX op)   { and_with_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); },
                [this](AND_Absolute op)    { and_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE)); },
                [this](AND_AbsoluteX op)   { and_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_X)); },
                [this](AND_AbsoluteY op)   { and_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); },
                [this](AND_IndirectX op)   { and_with_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_X)); },
                [this](AND_IndirectY op)   { and_with_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_Y)); },

                [this](ASL_Accumulator op) { set_register(Register::AC, shift_left(AC)); },
                [this](ASL_ZeroPage op)    { perform_at(op.address, AddressingMode::ZERO_PAGE, &MOS6502::shift_left); },
                [this](ASL_ZeroPageX op)   { perform_at(op.address, AddressingMode::ZERO_PAGE_X, &MOS6502::shift_left); },
                [this](ASL_Absolute op)    { perform_at(op.address, AddressingMode::ABSOLUTE, &MOS6502::shift_left); },
                [this](ASL_AbsoluteX op)   {
                    perform_at(op.address, AddressingMode::ABSOLUTE_X, &MOS6502::shift_left);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },

                [this](BCS op)             { if (SR[Flag::CARRY]) branch(op.offset); },
                [this](BCC op)             { if (!SR[Flag::CARRY]) branch(op.offset); },
                [this](BEQ op)             { if (SR[Flag::ZERO]) branch(op.offset); },
                [this](BNE op)             { if (!SR[Flag::ZERO]) branch(op.offset); },
                [this](BMI op)             { if (SR[Flag::NEGATIVE]) branch(op.offset); },
                [this](BPL op)             { if (!SR[Flag::NEGATIVE]) branch(op.offset); },
                [this](BVS op)             { if (SR[Flag::OVERFLOW_F]) branch(op.offset); },
                [this](BVC op)             { if (!SR[Flag::OVERFLOW_F]) branch(op.offset); },

                [this](BIT_ZeroPage op)    { bit_test(fetch_from(op.address, AddressingMode::ZERO_PAGE)); },
                [this](BIT_Absolute op)    { bit_test(fetch_from(op.address, AddressingMode::ABSOLUTE)); },

                [this](BRK op) {
                    // for some reason, the byte right next to the BRK command must be skipped
                    push_word_to_stack(PC + 1);
                    PC = fetch_word(ROM::BRK_HANDLER);
                    SR[Flag::BREAK] = SET;
                    cycle++;
                },

                [this](CLC op)             { SR[Flag::CARRY] = CLEAR; cycle++; },
                [this](CLD op)             { SR[Flag::DECIMAL] = CLEAR; cycle++; },
                [this](CLI op)             { SR[Flag::INTERRUPT_DISABLE] = CLEAR; cycle++; },
                [this](CLV op)             { SR[Flag::OVERFLOW_F] = CLEAR; cycle++; },

                [this](CMP_Immediate op)   { compare(AC, op.value); },
                [this](CMP_ZeroPage op)    { compare(AC, fetch_from(op.address, AddressingMode::ZERO_PAGE)); },
                [this](CMP_ZeroPageX op)   { compare(AC, fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); },
                [this](CMP_Absolute op)    { compare(AC, fetch_from(op.address, AddressingMode::ABSOLUTE)); },
                [this](CMP_AbsoluteX op)   { compare(AC, fetch_from(op.address, AddressingMode::ABSOLUTE_X)); },
                [this](CMP_AbsoluteY op)   { compare(AC, fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); },
                [this](CMP_IndirectX op)   { compare(AC, fetch_from(op.address, AddressingMode::INDIRECT_X)); },
                [this](CMP_IndirectY op)   { compare(AC, fetch_from(op.address, AddressingMode::INDIRECT_Y)); },

                [this](CPX_Immediate op)   { compare(X, op.value); },
                [this](CPX_ZeroPage op)    { compare(X, fetch_from(op.address, AddressingMode::ZERO_PAGE)); },
                [this](CPX_Absolute op)    { compare(X, fetch_from(op.address, AddressingMode::ABSOLUTE)); },

                [this](CPY_Immediate op)   { compare(Y, op.value); },
                [this](CPY_ZeroPage op)    { compare(Y, fetch_from(op.address, AddressingMode::ZERO_PAGE)); },
                [this](CPY_Absolute op)    { compare(Y, fetch_from(op.address, AddressingMode::ABSOLUTE)); },

                [this](DEC_ZeroPage op)    { perform_at(op.address, AddressingMode::ZERO_PAGE, &MOS6502::decrement); },
                [this](DEC_ZeroPageX op)   { perform_at(op.address, AddressingMode::ZERO_PAGE_X, &MOS6502::decrement); },
                [this](DEC_Absolute op)    { perform_at(op.address, AddressingMode::ABSOLUTE, &MOS6502::decrement); },
                [this](DEC_AbsoluteX op)   {
                    perform_at(op.address, AddressingMode::ABSOLUTE_X, &MOS6502::decrement);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },

                [this](DEX op)             { set_register(Register::X, X - 1); cycle++; },
                [this](DEY op)             { set_register(Register::Y, Y - 1); cycle++; },

                [this](EOR_Immediate op)   { xor_with_accumulator(op.value); },
                [this](EOR_ZeroPage op)    { xor_with_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE)); },
                [this](EOR_ZeroPageX op)   { xor_with_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); },
                [this](EOR_Absolute op)    { xor_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE)); },
                [this](EOR_AbsoluteX op)   { xor_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_X)); },
                [this](EOR_AbsoluteY op)   { xor_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); },
                [this](EOR_IndirectX op)   { xor_with_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_X)); },
                [this](EOR_IndirectY op)   { xor_with_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_Y)); },

                [this](INC_ZeroPage op)    { perform_at(op.address, AddressingMode::ZERO_PAGE, &MOS6502::increment); },
                [this](INC_ZeroPageX op)   { perform_at(op.address, AddressingMode::ZERO_PAGE_X, &MOS6502::increment); },
                [this](INC_Absolute op)    { perform_at(op.address, AddressingMode::ABSOLUTE, &MOS6502::increment); },
                [this](INC_AbsoluteX op)   {
                    perform_at(op.address, AddressingMode::ABSOLUTE_X, &MOS6502::increment);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                },

                [this](INX op)             { set_register(Register::X, X + 1); cycle++; },
                [this](INY op)             { set_register(Register::Y, Y + 1); cycle++; },

                [this](JMP_Absolute op)    { PC = op.address; },
                [this](JMP_Indirect op)    { PC = fetch_word(op.address); },

                [this](JSR op) {
                    push_word_to_stack(PC - 1);
                    PC = op.address;
                    cycle++;
                },

                [this](LDA_Immediate op)   { set_register(Register::AC, op.value); },
                [this](LDA_ZeroPage op)    { set_register(Register::AC, fetch_from(op.address, AddressingMode::ZERO_PAGE)); },
                [this](LDA_ZeroPageX op)   { set_register(Register::AC, fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); },
                [this](LDA_Absolute op)    { set_register(Register::AC, fetch_from(op.address, AddressingMode::ABSOLUTE)); },
                [this](LDA_AbsoluteX op)   { set_register(Register::AC, fetch_from(op.address, AddressingMode::ABSOLUTE_X)); },
                [this](LDA_AbsoluteY op)   { set_register(Register::AC, fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); },
                [this](LDA_IndirectX op)   { set_register(Register::AC, fetch_from(op.address, AddressingMode::INDIRECT_X)); },
                [this](LDA_IndirectY op)   { set_register(Register::AC, fetch_from(op.address, AddressingMode::INDIRECT_Y)); },

                [this](LDX_Immediate op)   { set_register(Register::X, op.value); },
                [this](LDX_ZeroPage op)    { set_register(Register::X, fetch_from(op.address, AddressingMode::ZERO_PAGE)); },
                [this](LDX_ZeroPageY op)   { set_register(Register::X, fetch_from(op.address, AddressingMode::ZERO_PAGE_Y)); },
                [this](LDX_Absolute op)    { set_register(Register::X, fetch_from(op.address, AddressingMode::ABSOLUTE)); },
                [this](LDX_AbsoluteY op)   { set_register(Register::X, fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); },

                [this](LDY_Immediate op)   { set_register(Register::Y, op.value); },
                [this](LDY_ZeroPage op)    { set_register(Register::Y, fetch_from(op.address, AddressingMode::ZERO_PAGE)); },
                [this](LDY_ZeroPageX op)   { set_register(Register::Y, fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); },
                [this](LDY_Absolute op)    { set_register(Register::Y, fetch_from(op.address, AddressingMode::ABSOLUTE)); },
                [this](LDY_AbsoluteX op)   { set_register(Register::Y, fetch_from(op.address, AddressingMode::ABSOLUTE_X)); },

                [this](LSR_Accumulator op) { set_register(Register::AC, shift_right(AC)); },
                [this](LSR_ZeroPage op)    { perform_at(op.address, AddressingMode::ZERO_PAGE, &MOS6502::shift_right); },
                [this](LSR_ZeroPageX op)   { perform_at(op.address, AddressingMode::ZERO_PAGE_X, &MOS6502::shift_right); },
                [this](LSR_Absolute op)    { perform_at(op.address, AddressingMode::ABSOLUTE, &MOS6502::shift_right); },
                [this](LSR_AbsoluteX op)   {
                    perform_at(op.address, AddressingMode::ABSOLUTE_X, &MOS6502::shift_right);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },

                [this](NOP op)             { cycle++; },

                [this](ORA_Immediate op)   { or_with_accumulator(op.value); },
                [this](ORA_ZeroPage op)    { or_with_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE)); },
                [this](ORA_ZeroPageX op)   { or_with_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); },
                [this](ORA_Absolute op)    { or_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE)); },
                [this](ORA_AbsoluteX op)   { or_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_X)); },
                [this](ORA_AbsoluteY op)   { or_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); },
                [this](ORA_IndirectX op)   { or_with_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_X)); },
                [this](ORA_IndirectY op)   { or_with_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_Y)); },

                [this](PHA op)             { push_byte_to_stack(AC); cycle++; },
                [this](PHP op)             { push_byte_to_stack(SR.to_byte()); cycle++; },

                [this](PLA op)             { set_register(Register::AC, pull_byte_from_stack()); cycle++; },
                [this](PLP op)             { set_register(Register::SR, pull_byte_from_stack()); cycle++; },

                [this](ROL_Accumulator op) { set_register(Register::AC, rotate_left(AC)); },
                [this](ROL_ZeroPage op)    { perform_at(op.address, AddressingMode::ZERO_PAGE, &MOS6502::rotate_left); },
                [this](ROL_ZeroPageX op)   { perform_at(op.address, AddressingMode::ZERO_PAGE_X, &MOS6502::rotate_left); },
                [this](ROL_Absolute op)    { perform_at(op.address, AddressingMode::ABSOLUTE, &MOS6502::rotate_left); },
                [this](ROL_AbsoluteX op)   {
                    perform_at(op.address, AddressingMode::ABSOLUTE_X, &MOS6502::rotate_left);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },

                [this](ROR_Accumulator op) { set_register(Register::AC, rotate_right(AC)); },
                [this](ROR_ZeroPage op)    { perform_at(op.address, AddressingMode::ZERO_PAGE, &MOS6502::rotate_right); },
                [this](ROR_ZeroPageX op)   { perform_at(op.address, AddressingMode::ZERO_PAGE_X, &MOS6502::rotate_right); },
                [this](ROR_Absolute op)    { perform_at(op.address, AddressingMode::ABSOLUTE, &MOS6502::rotate_right); },
                [this](ROR_AbsoluteX op)   {
                    perform_at(op.address, AddressingMode::ABSOLUTE_X, &MOS6502::rotate_right);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },

                [this](RTI op) {
                    SR = pull_byte_from_stack();
                    PC = pull_word_from_stack();
                    cycle--;
                },

                [this](RTS op)             { PC = pull_word_from_stack() + 1; cycle++; },

                [this](SBC_Immediate op)   { subtract_from_accumulator(op.value); },
                [this](SBC_ZeroPage op)    { subtract_from_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE)); },
                [this](SBC_ZeroPageX op)   { subtract_from_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); },
                [this](SBC_Absolute op)    { subtract_from_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE)); },
                [this](SBC_AbsoluteX op)   { subtract_from_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_X)); },
                [this](SBC_AbsoluteY op)   { subtract_from_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); },
                [this](SBC_IndirectX op)   { subtract_from_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_X)); },
                [this](SBC_IndirectY op)   { subtract_from_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_Y)); },

                [this](SEC op)             { SR[Flag::CARRY] = SET; },
                [this](SED op)             { SR[Flag::DECIMAL] = SET; },
                [this](SEI op)             { SR[Flag::INTERRUPT_DISABLE] = SET; },

                [this](STA_ZeroPage op)    { write_to(op.address, AddressingMode::ZERO_PAGE, AC); },
                [this](STA_ZeroPageX op)   { write_to(op.address, AddressingMode::ZERO_PAGE_X, AC); },
                [this](STA_Absolute op)    { write_to(op.address, AddressingMode::ABSOLUTE, AC); },
                [this](STA_AbsoluteX op)   {
                    write_to(op.address, AddressingMode::ABSOLUTE_X, AC);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },
                [this](STA_AbsoluteY op)   {
                    write_to(op.address, AddressingMode::ABSOLUTE_Y, AC);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },
                [this](STA_IndirectX op)   { write_to(op.address, AddressingMode::INDIRECT_X, AC); },
                [this](STA_IndirectY op)   {
                    write_to(op.address, AddressingMode::INDIRECT_Y, AC);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },

                [this](STX_ZeroPage op)    { write_to(op.address, AddressingMode::ZERO_PAGE, X); },
                [this](STX_ZeroPageY op)   { write_to(op.address, AddressingMode::ZERO_PAGE_Y, X); },
                [this](STX_Absolute op)    { write_to(op.address, AddressingMode::ABSOLUTE, X); },

                [this](STY_ZeroPage op)    { write_to(op.address, AddressingMode::ZERO_PAGE, Y); },
                [this](STY_ZeroPageX op)   { write_to(op.address, AddressingMode::ZERO_PAGE_X, Y); },
                [this](STY_Absolute op)    { write_to(op.address, AddressingMode::ABSOLUTE, Y); },

                [this](TAX op) { set_register(Register::X, AC); cycle++; },
                [this](TAY op) { set_register(Register::Y, AC); cycle++; },
                [this](TSX op) { set_register(Register::X, SP); cycle++; },
                [this](TXA op) { set_register(Register::AC, X); cycle++; },
                [this](TXS op) { set_register(Register::SP, X); cycle++; },
                [this](TYA op) { set_register(Register::AC, Y); cycle++; }
            },
                   operation
        );
    }

    void MOS6502::add_to_accumulator(Byte value) noexcept {
        SR[Flag::OVERFLOW_F] = SR[Flag::CARRY];
        add_with_overflow((char)AC, (char)value, SR[Flag::OVERFLOW_F], INT8_MIN, INT8_MAX);

        set_register(Register::AC, add_with_overflow(AC, value, SR[Flag::CARRY], 0, UINT8_MAX));
    }

    void MOS6502::and_with_accumulator(Byte value) noexcept {
        set_register(Register::AC, AC & value);
    }

    Byte MOS6502::shift_left(Byte value) noexcept {
        cycle++;
        SR[Flag::CARRY] = get_bit(value, 7);
        set_writing_flags(value << 1);
        return value << 1;
    }

//    void MOS6502::perform_at_zero_page(Byte address, MOS6502::ByteOperator byteOperator) noexcept {
//        auto targetAddress = resolve_zero_page(address);
//        memory.set_byte({.address = targetAddress, .value = (this->*byteOperator)(memory.fetch_byte(targetAddress, cycle)), .cycle = cycle});
//    }
//
//    void MOS6502::perform_at_zero_page_indexed(Byte address, Byte index, MOS6502::ByteOperator byteOperator) noexcept {
//        auto targetAddress = resolve_zero_page_indexed(address, index);
//        memory.set_byte({.address = targetAddress, .value = (this->*byteOperator)(memory.fetch_byte(targetAddress, cycle)), .cycle = cycle});
//    }
//
//    void MOS6502::perform_at_absolute(Word address, MOS6502::ByteOperator byteOperator) noexcept {
//        auto targetAddress = resolve_absolute(address);
//        memory.set_byte({.address = targetAddress, .value = (this->*byteOperator)(memory.fetch_byte(targetAddress, cycle)), .cycle = cycle});
//    }
//
//    void MOS6502::perform_at_absolute_indexed(Word address, Byte index, MOS6502::ByteOperator byteOperator) noexcept {
//        auto targetAddress = resolve_absolute_indexed(address, index);
//        memory.set_byte({.address = targetAddress, .value = (this->*byteOperator)(memory.fetch_byte(targetAddress, cycle)), .cycle = cycle});
//    }

    void MOS6502::branch(char offset) noexcept {
        cycle++;
        Word newPC = PC + offset;
//        if (WordToBytes(PC).high != WordToBytes(newPC).high) cycle++;

        PC = newPC;
    }

    void MOS6502::bit_test(Byte value) noexcept {
        Byte result = AC & value;

        SR[Flag::ZERO] = result == 0;

        // for some reason, these flags are taken not from the result, but from the tested values
        SR[Flag::OVERFLOW_F] = get_bit(value, (int)Flag::OVERFLOW_F);
        SR[Flag::NEGATIVE] = get_bit(value, (int)Flag::NEGATIVE);
    }

    void MOS6502::compare(Byte reg, Byte value) noexcept {
        SR[Flag::CARRY] = reg >= value;
        SR[Flag::ZERO] = reg == value;
        SR[Flag::NEGATIVE] = reg < value;
    }

    Byte MOS6502::decrement(Byte value) noexcept {
        cycle++;
        Byte result = value - 1;
        set_writing_flags(result);
        return result;
    }

    void MOS6502::xor_with_accumulator(Byte value) noexcept {
        set_register(Register::AC, AC ^ value);
    }

    Byte MOS6502::increment(Byte value) noexcept {
        cycle++;
        Byte result = value + 1;
        set_writing_flags(result);
        return result;
    }

    Byte MOS6502::shift_right(Byte value) noexcept {
        cycle++;
        SR[Flag::CARRY] = get_bit(value, 0);
        set_writing_flags(value >> 1);
        return value >> 1;
    }

    void MOS6502::or_with_accumulator(Byte value) noexcept {
        set_register(Register::AC, AC | value);
    }

    Byte MOS6502::rotate_left(Byte value) noexcept {
        cycle++;
        Byte newValue = value << 1;
        set_bit(newValue, 0, SR[Flag::CARRY]);
        set_writing_flags(newValue);
        SR[Flag::CARRY] = get_bit(value, 7);
        set_writing_flags(newValue);
        return newValue;
    }

    Byte MOS6502::rotate_right(Byte value) noexcept {
        cycle++;
        Byte newValue = value >> 1;
        set_bit(newValue, 7, SR[Flag::CARRY]);
        set_writing_flags(newValue);
        SR[Flag::CARRY] = get_bit(value, 0);
        set_writing_flags(newValue);
        return newValue;
    }

    void MOS6502::subtract_from_accumulator(Byte value) noexcept {
        SR[Flag::OVERFLOW_F] = SR[Flag::CARRY];
        subtract_with_overflow((char)AC, (char)value, SR[Flag::OVERFLOW_F], INT8_MIN, INT8_MAX);
        SR[Flag::OVERFLOW_F] = !SR[Flag::OVERFLOW_F];

        set_register(Register::AC, subtract_with_overflow(AC, value, SR[Flag::CARRY], 0, UINT8_MAX));
    }

    Byte MOS6502::index_zero_page(Byte address, Byte index) noexcept {
        cycle++;
        pageCrossed = index > UINT8_MAX - address;
        return address + index;
    }

    Word MOS6502::index_absolute(Word address, Byte index) noexcept {
        Word result = address + index;
        pageCrossed = WordToBytes(result).high != WordToBytes(address).high;
        if (pageCrossed) cycle++;
        return result;
    }


    Word MOS6502::fetch_word() noexcept {
        WordToBytes result;
        result.low = memory.fetch_byte(PC++, cycle);
        result.high = memory.fetch_byte(PC++, cycle);
        return result.word;
    }

    Word MOS6502::fetch_word(Word address) noexcept {
        WordToBytes result;
        result.low = memory.fetch_byte(address, cycle);
        result.high = memory.fetch_byte(address + 1, cycle);
        return result.word;
    }

    std::expected<Operation, InvalidOperation> MOS6502::fetch_operation() noexcept {
        Byte opCode = memory.fetch_byte(PC++, cycle);

        switch (opCode) {
            default: return std::unexpected(InvalidOperation{.opCode = opCode});

            case ADC_IMMEDIATE:   return ADC_Immediate{.value = memory.fetch_byte(PC++, cycle)};
            case ADC_ZERO_PAGE:   return ADC_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case ADC_ZERO_PAGE_X: return ADC_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case ADC_ABSOLUTE:    return ADC_Absolute{.address = fetch_word()};
            case ADC_ABSOLUTE_X:  return ADC_AbsoluteX{.address = fetch_word()};
            case ADC_ABSOLUTE_Y:  return ADC_AbsoluteY{.address = fetch_word()};
            case ADC_INDIRECT_X:  return ADC_IndirectX{.address = memory.fetch_byte(PC++, cycle)};
            case ADC_INDIRECT_Y:  return ADC_IndirectY{.address = memory.fetch_byte(PC++, cycle)};

            case AND_IMMEDIATE:   return AND_Immediate{.value = memory.fetch_byte(PC++, cycle)};
            case AND_ZERO_PAGE:   return AND_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case AND_ZERO_PAGE_X: return AND_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case AND_ABSOLUTE:    return AND_Absolute{.address = fetch_word()};
            case AND_ABSOLUTE_X:  return AND_AbsoluteX{.address = fetch_word()};
            case AND_ABSOLUTE_Y:  return AND_AbsoluteY{.address = fetch_word()};
            case AND_INDIRECT_X:  return AND_IndirectX{.address = memory.fetch_byte(PC++, cycle)};
            case AND_INDIRECT_Y:  return AND_IndirectY{.address = memory.fetch_byte(PC++, cycle)};

            case ASL_ACCUMULATOR: return ASL_Accumulator{};
            case ASL_ZERO_PAGE:   return ASL_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case ASL_ZERO_PAGE_X: return ASL_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case ASL_ABSOLUTE:    return ASL_Absolute{.address = fetch_word()};
            case ASL_ABSOLUTE_X:  return ASL_AbsoluteX{.address = fetch_word()};

            case BCC_RELATIVE:    return BCC{.offset = (char)memory.fetch_byte(PC++, cycle)};
            case BCS_RELATIVE:    return BCS{.offset = (char)memory.fetch_byte(PC++, cycle)};
            case BEQ_RELATIVE:    return BEQ{.offset = (char)memory.fetch_byte(PC++, cycle)};
            case BNE_RELATIVE:    return BNE{.offset = (char)memory.fetch_byte(PC++, cycle)};
            case BMI_RELATIVE:    return BMI{.offset = (char)memory.fetch_byte(PC++, cycle)};
            case BPL_RELATIVE:    return BPL{.offset = (char)memory.fetch_byte(PC++, cycle)};
            case BVC_RELATIVE:    return BVC{.offset = (char)memory.fetch_byte(PC++, cycle)};
            case BVS_RELATIVE:    return BVS{.offset = (char)memory.fetch_byte(PC++, cycle)};

            case BIT_ZERO_PAGE:   return BIT_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case BIT_ABSOLUTE:    return BIT_Absolute{.address = fetch_word()};

            case BRK_IMPLICIT:    return BRK{};

            case CLC_IMPLICIT:    return CLC{};
            case CLD_IMPLICIT:    return CLD{};
            case CLI_IMPLICIT:    return CLI{};
            case CLV_IMPLICIT:    return CLV{};

            case CMP_IMMEDIATE:   return CMP_Immediate{.value = memory.fetch_byte(PC++, cycle)};
            case CMP_ZERO_PAGE:   return CMP_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case CMP_ZERO_PAGE_X: return CMP_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case CMP_ABSOLUTE:    return CMP_Absolute{.address = fetch_word()};
            case CMP_ABSOLUTE_X:  return CMP_AbsoluteX{.address = fetch_word()};
            case CMP_ABSOLUTE_Y:  return CMP_AbsoluteY{.address = fetch_word()};
            case CMP_INDIRECT_X:  return CMP_IndirectX{.address = memory.fetch_byte(PC++, cycle)};
            case CMP_INDIRECT_Y:  return CMP_IndirectY{.address = memory.fetch_byte(PC++, cycle)};

            case CPX_IMMEDIATE:   return CPX_Immediate{.value = memory.fetch_byte(PC++, cycle)};
            case CPX_ZERO_PAGE:   return CPX_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case CPX_ABSOLUTE:    return CPX_Absolute{.address = fetch_word()};

            case CPY_IMMEDIATE:   return CPY_Immediate{.value = memory.fetch_byte(PC++, cycle)};
            case CPY_ZERO_PAGE:   return CPY_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case CPY_ABSOLUTE:    return CPY_Absolute{.address = fetch_word()};

            case DEC_ZERO_PAGE:   return DEC_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case DEC_ZERO_PAGE_X: return DEC_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case DEC_ABSOLUTE:    return DEC_Absolute{.address = fetch_word()};
            case DEC_ABSOLUTE_X:  return DEC_AbsoluteX{.address = fetch_word()};

            case DEX_IMPLICIT:    return DEX{};
            case DEY_IMPLICIT:    return DEY{};

            case EOR_IMMEDIATE:   return EOR_Immediate{.value = memory.fetch_byte(PC++, cycle)};
            case EOR_ZERO_PAGE:   return EOR_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case EOR_ZERO_PAGE_X: return EOR_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case EOR_ABSOLUTE:    return EOR_Absolute{.address = fetch_word()};
            case EOR_ABSOLUTE_X:  return EOR_AbsoluteX{.address = fetch_word()};
            case EOR_ABSOLUTE_Y:  return EOR_AbsoluteY{.address = fetch_word()};
            case EOR_INDIRECT_X:  return EOR_IndirectX{.address = memory.fetch_byte(PC++, cycle)};
            case EOR_INDIRECT_Y:  return EOR_IndirectY{.address = memory.fetch_byte(PC++, cycle)};

            case INC_ZERO_PAGE:   return INC_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case INC_ZERO_PAGE_X: return INC_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case INC_ABSOLUTE:    return INC_Absolute{.address = fetch_word()};
            case INC_ABSOLUTE_X:  return INC_AbsoluteX{.address = fetch_word()};

            case INX_IMPLICIT:    return INX{};
            case INY_IMPLICIT:    return INY{};

            case JMP_ABSOLUTE:    return JMP_Absolute{.address = fetch_word()};
            case JMP_INDIRECT:    return JMP_Indirect{.address = fetch_word()};

            case JSR_ABSOLUTE:    return JSR{.address = fetch_word()};

            case LDA_IMMEDIATE:   return LDA_Immediate{.value = memory.fetch_byte(PC++, cycle)};
            case LDA_ZERO_PAGE:   return LDA_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case LDA_ZERO_PAGE_X: return LDA_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case LDA_ABSOLUTE:    return LDA_Absolute{.address = fetch_word()};
            case LDA_ABSOLUTE_X:  return LDA_AbsoluteX{.address = fetch_word()};
            case LDA_ABSOLUTE_Y:  return LDA_AbsoluteY{.address = fetch_word()};
            case LDA_INDIRECT_X:  return LDA_IndirectX{.address = memory.fetch_byte(PC++, cycle)};
            case LDA_INDIRECT_Y:  return LDA_IndirectY{.address = memory.fetch_byte(PC++, cycle)};

            case LDX_IMMEDIATE:   return LDX_Immediate{.value = memory.fetch_byte(PC++, cycle)};
            case LDX_ZERO_PAGE:   return LDX_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case LDX_ZERO_PAGE_Y: return LDX_ZeroPageY{.address = memory.fetch_byte(PC++, cycle)};
            case LDX_ABSOLUTE:    return LDX_Absolute{.address = fetch_word()};
            case LDX_ABSOLUTE_Y:  return LDX_AbsoluteY{.address = fetch_word()};

            case LDY_IMMEDIATE:   return LDY_Immediate{.value = memory.fetch_byte(PC++, cycle)};
            case LDY_ZERO_PAGE:   return LDY_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case LDY_ZERO_PAGE_X: return LDY_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case LDY_ABSOLUTE:    return LDY_Absolute{.address = fetch_word()};
            case LDY_ABSOLUTE_X:  return LDY_AbsoluteX{.address = fetch_word()};

            case LSR_ACCUMULATOR: return LSR_Accumulator{};
            case LSR_ZERO_PAGE:   return LSR_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case LSR_ZERO_PAGE_X: return LSR_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case LSR_ABSOLUTE:    return LSR_Absolute{.address = fetch_word()};
            case LSR_ABSOLUTE_X:  return LSR_AbsoluteX{.address = fetch_word()};

            case NOP_IMPLICIT:    return NOP{};

            case ORA_IMMEDIATE:   return ORA_Immediate{.value = memory.fetch_byte(PC++, cycle)};
            case ORA_ZERO_PAGE:   return ORA_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case ORA_ZERO_PAGE_X: return ORA_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case ORA_ABSOLUTE:    return ORA_Absolute{.address = fetch_word()};
            case ORA_ABSOLUTE_X:  return ORA_AbsoluteX{.address = fetch_word()};
            case ORA_ABSOLUTE_Y:  return ORA_AbsoluteY{.address = fetch_word()};
            case ORA_INDIRECT_X:  return ORA_IndirectX{.address = memory.fetch_byte(PC++, cycle)};
            case ORA_INDIRECT_Y:  return ORA_IndirectY{.address = memory.fetch_byte(PC++, cycle)};

            case PHA_IMPLICIT:    return PHA{};
            case PHP_IMPLICIT:    return PHP{};

            case PLA_IMPLICIT:    return PLA{};
            case PLP_IMPLICIT:    return PLP{};

            case ROL_ACCUMULATOR: return ROL_Accumulator{};
            case ROL_ZERO_PAGE:   return ROL_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case ROL_ZERO_PAGE_X: return ROL_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case ROL_ABSOLUTE:    return ROL_Absolute{.address = fetch_word()};
            case ROL_ABSOLUTE_X:  return ROL_AbsoluteX{.address = fetch_word()};

            case ROR_ACCUMULATOR: return ROR_Accumulator{};
            case ROR_ZERO_PAGE:   return ROR_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case ROR_ZERO_PAGE_X: return ROR_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case ROR_ABSOLUTE:    return ROR_Absolute{.address = fetch_word()};
            case ROR_ABSOLUTE_X:  return ROR_AbsoluteX{.address = fetch_word()};

            case RTI_IMPLICIT:    return RTI{};
            case RTS_IMPLICIT:    return RTS{};

            case SBC_IMMEDIATE:   return SBC_Immediate{.value = memory.fetch_byte(PC++, cycle)};
            case SBC_ZERO_PAGE:   return SBC_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case SBC_ZERO_PAGE_X: return SBC_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case SBC_ABSOLUTE:    return SBC_Absolute{.address = fetch_word()};
            case SBC_ABSOLUTE_X:  return SBC_AbsoluteX{.address = fetch_word()};
            case SBC_ABSOLUTE_Y:  return SBC_AbsoluteY{.address = fetch_word()};
            case SBC_INDIRECT_X:  return SBC_IndirectX{.address = memory.fetch_byte(PC++, cycle)};
            case SBC_INDIRECT_Y:  return SBC_IndirectY{.address = memory.fetch_byte(PC++, cycle)};

            case SEC_IMPLICIT:    return SEC{};
            case SED_IMPLICIT:    return SED{};
            case SEI_IMPLICIT:    return SEI{};

            case STA_ZERO_PAGE:   return STA_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case STA_ZERO_PAGE_X: return STA_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case STA_ABSOLUTE:    return STA_Absolute{.address = fetch_word()};
            case STA_ABSOLUTE_X:  return STA_AbsoluteX{.address = fetch_word()};
            case STA_ABSOLUTE_Y:  return STA_AbsoluteY{.address = fetch_word()};
            case STA_INDIRECT_X:  return STA_IndirectX{.address = memory.fetch_byte(PC++, cycle)};
            case STA_INDIRECT_Y:  return STA_IndirectY{.address = memory.fetch_byte(PC++, cycle)};

            case STX_ZERO_PAGE:   return STX_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case STX_ZERO_PAGE_Y: return STX_ZeroPageY{.address = memory.fetch_byte(PC++, cycle)};
            case STX_ABSOLUTE:    return STX_Absolute{.address = fetch_word()};

            case STY_ZERO_PAGE:   return STY_ZeroPage{.address = memory.fetch_byte(PC++, cycle)};
            case STY_ZERO_PAGE_X: return STY_ZeroPageX{.address = memory.fetch_byte(PC++, cycle)};
            case STY_ABSOLUTE:    return STY_Absolute{.address = fetch_word()};

            case TAX_IMPLICIT:    return TAX{};
            case TAY_IMPLICIT:    return TAY{};
            case TSX_IMPLICIT:    return TSX{};
            case TXA_IMPLICIT:    return TXA{};
            case TXS_IMPLICIT:    return TXS{};
            case TYA_IMPLICIT:    return TYA{};
        }
    }

    Word MOS6502::resolve(Word address, AddressingMode mode) noexcept {
        switch (mode) {
            case AddressingMode::ZERO_PAGE:   return address;
            case AddressingMode::ZERO_PAGE_X: return index_zero_page(address, X);
            case AddressingMode::ZERO_PAGE_Y: return index_zero_page(address, Y);
            case AddressingMode::ABSOLUTE:    return address;
            case AddressingMode::ABSOLUTE_X:  return index_absolute(address, X);
            case AddressingMode::ABSOLUTE_Y:  return index_absolute(address, Y);
            case AddressingMode::INDIRECT_X:  return fetch_word(index_zero_page(address, X));
            case AddressingMode::INDIRECT_Y:  return index_absolute(fetch_word(address), Y);
        }

        std::unreachable();
    }

    Byte MOS6502::fetch_from(Word address, AddressingMode mode) noexcept {
        return memory.fetch_byte(resolve(address, mode), cycle);
    }

    void MOS6502::write_to(Word address, AddressingMode mode, Byte value) noexcept {
        memory.set_byte({.address = resolve(address, mode), .value = value, .cycle = cycle});
    }

    void MOS6502::perform_at(Word address, AddressingMode mode, MOS6502::ByteOperator byteOperator) noexcept {
        auto targetAddress = resolve(address, mode);
        memory.set_byte({.address = targetAddress, .value = (this->*byteOperator)(memory.fetch_byte(targetAddress, cycle)), .cycle = cycle});
    }

}






