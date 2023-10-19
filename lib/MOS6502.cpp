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
        PC = memory.fetch_word(ROM::RESET_LOCATION, cycle);
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

            if (auto operation = decode(memory, PC, cycle); operation.has_value()) {
                if (stopOnBRK && std::holds_alternative<BRK>(operation.value())) return StopOnBreak{.address = commandAddress};

                execute(operation.value());
            }
            else return std::unexpected(UnknownOperation{.address = commandAddress});

            commandsExecuted++;
        }
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

    void MOS6502::set_writing_flags(Byte value) {
        SR[Flag::ZERO] = value == 0;
        SR[Flag::NEGATIVE] = (char)value < 0;
    }

    void MOS6502::execute(const Operation &operation) noexcept {
        std::visit(Overload {
                [this](ADC_Immediate op)   { add_to_accumulator(op.value); },
                [this](ADC_ZeroPage op)    { add_to_accumulator(fetch_from_zero_page(op.address)); },
                [this](ADC_ZeroPageX op)   { add_to_accumulator(fetch_from_zero_page_indexed(op.address, X)); },
                [this](ADC_Absolute op)    { add_to_accumulator(fetch_from_absolute(op.address)); },
                [this](ADC_AbsoluteX op)   { add_to_accumulator(fetch_from_absolute_indexed(op.address, X)); },
                [this](ADC_AbsoluteY op)   { add_to_accumulator(fetch_from_absolute_indexed(op.address, Y)); },
                [this](ADC_IndirectX op)   { add_to_accumulator(fetch_from_indirect_X(op.address)); },
                [this](ADC_IndirectY op)   { add_to_accumulator(fetch_from_indirect_Y(op.address)); },

                [this](AND_Immediate op)   { and_with_accumulator(op.value); },
                [this](AND_ZeroPage op)    { and_with_accumulator(fetch_from_zero_page(op.address)); },
                [this](AND_ZeroPageX op)   { and_with_accumulator(fetch_from_zero_page_indexed(op.address, X)); },
                [this](AND_Absolute op)    { and_with_accumulator(fetch_from_absolute(op.address)); },
                [this](AND_AbsoluteX op)   { and_with_accumulator(fetch_from_absolute_indexed(op.address, X)); },
                [this](AND_AbsoluteY op)   { and_with_accumulator(fetch_from_absolute_indexed(op.address, Y)); },
                [this](AND_IndirectX op)   { and_with_accumulator(fetch_from_indirect_X(op.address)); },
                [this](AND_IndirectY op)   { and_with_accumulator(fetch_from_indirect_Y(op.address)); },

                [this](ASL_Accumulator op) { set_register(Register::AC, shift_left(AC)); },
                [this](ASL_ZeroPage op)    { perform_at_zero_page(op.address, &MOS6502::shift_left); },
                [this](ASL_ZeroPageX op)   { perform_at_zero_page_indexed(op.address, X, &MOS6502::shift_left); },
                [this](ASL_Absolute op)    { perform_at_absolute(op.address, &MOS6502::shift_left); },
                [this](ASL_AbsoluteX op)   {
                    perform_at_absolute_indexed(op.address, X, &MOS6502::shift_left);
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

                [this](BIT_ZeroPage op)    { bit_test(fetch_from_zero_page(op.address)); },
                [this](BIT_Absolute op)    { bit_test(fetch_from_absolute(op.address)); },

                [this](BRK op) {
                    // for some reason, the byte right next to the BRK command must be skipped
                    push_word_to_stack(PC + 1);
                    PC = memory.fetch_word(ROM::BRK_HANDLER, cycle);
                    SR[Flag::BREAK] = SET;
                    cycle++;
                },

                [this](CLC op)             { SR[Flag::CARRY] = CLEAR; cycle++; },
                [this](CLD op)             { SR[Flag::DECIMAL] = CLEAR; cycle++; },
                [this](CLI op)             { SR[Flag::INTERRUPT_DISABLE] = CLEAR; cycle++; },
                [this](CLV op)             { SR[Flag::OVERFLOW_F] = CLEAR; cycle++; },

                [this](CMP_Immediate op)   { compare(AC, op.value); },
                [this](CMP_ZeroPage op)    { compare(AC, fetch_from_zero_page(op.address)); },
                [this](CMP_ZeroPageX op)   { compare(AC, fetch_from_zero_page_indexed(op.address, X)); },
                [this](CMP_Absolute op)    { compare(AC, fetch_from_absolute(op.address)); },
                [this](CMP_AbsoluteX op)   { compare(AC, fetch_from_absolute_indexed(op.address, X)); },
                [this](CMP_AbsoluteY op)   { compare(AC, fetch_from_absolute_indexed(op.address, Y)); },
                [this](CMP_IndirectX op)   { compare(AC, fetch_from_indirect_X(op.address)); },
                [this](CMP_IndirectY op)   { compare(AC, fetch_from_indirect_Y(op.address)); },

                [this](CPX_Immediate op)   { compare(X, op.value); },
                [this](CPX_ZeroPage op)    { compare(X, fetch_from_zero_page(op.address)); },
                [this](CPX_Absolute op)    { compare(X, fetch_from_absolute(op.address)); },

                [this](CPY_Immediate op)   { compare(Y, op.value); },
                [this](CPY_ZeroPage op)    { compare(Y, fetch_from_zero_page(op.address)); },
                [this](CPY_Absolute op)    { compare(Y, fetch_from_absolute(op.address)); },

                [this](DEC_ZeroPage op)    { perform_at_zero_page(op.address, &MOS6502::decrement); },
                [this](DEC_ZeroPageX op)   { perform_at_zero_page_indexed(op.address, X, &MOS6502::decrement); },
                [this](DEC_Absolute op)    { perform_at_absolute(op.address, &MOS6502::decrement); },
                [this](DEC_AbsoluteX op)   {
                    perform_at_absolute_indexed(op.address, X, &MOS6502::decrement);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },

                [this](DEX op)             { set_register(Register::X, X - 1); cycle++; },
                [this](DEY op)             { set_register(Register::Y, Y - 1); cycle++; },

                [this](EOR_Immediate op)   { xor_with_accumulator(op.value); },
                [this](EOR_ZeroPage op)    { xor_with_accumulator(fetch_from_zero_page(op.address)); },
                [this](EOR_ZeroPageX op)   { xor_with_accumulator(fetch_from_zero_page_indexed(op.address, X)); },
                [this](EOR_Absolute op)    { xor_with_accumulator(fetch_from_absolute(op.address)); },
                [this](EOR_AbsoluteX op)   { xor_with_accumulator(fetch_from_absolute_indexed(op.address, X)); },
                [this](EOR_AbsoluteY op)   { xor_with_accumulator(fetch_from_absolute_indexed(op.address, Y)); },
                [this](EOR_IndirectX op)   { xor_with_accumulator(fetch_from_indirect_X(op.address)); },
                [this](EOR_IndirectY op)   { xor_with_accumulator(fetch_from_indirect_Y(op.address)); },

                [this](INC_ZeroPage op)    { perform_at_zero_page(op.address, &MOS6502::increment); },
                [this](INC_ZeroPageX op)   { perform_at_zero_page_indexed(op.address, X, &MOS6502::increment); },
                [this](INC_Absolute op)    { perform_at_absolute(op.address, &MOS6502::increment); },
                [this](INC_AbsoluteX op)   {
                    perform_at_absolute_indexed(op.address, X, &MOS6502::increment);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                },

                [this](INX op)             { set_register(Register::X, X + 1); cycle++; },
                [this](INY op)             { set_register(Register::Y, Y + 1); cycle++; },

                [this](JMP_Absolute op)    { PC = op.address; },
                [this](JMP_Indirect op)    { PC = memory.fetch_word(op.address, cycle); },

                [this](JSR op) {
                    push_word_to_stack(PC - 1);
                    PC = op.address;
                    cycle++;
                },

                [this](LDA_Immediate op)   { set_register(Register::AC, op.value); },
                [this](LDA_ZeroPage op)    { set_register(Register::AC, fetch_from_zero_page(op.address)); },
                [this](LDA_ZeroPageX op)   { set_register(Register::AC, fetch_from_zero_page_indexed(op.address, X)); },
                [this](LDA_Absolute op)    { set_register(Register::AC, fetch_from_absolute(op.address)); },
                [this](LDA_AbsoluteX op)   { set_register(Register::AC, fetch_from_absolute_indexed(op.address, X)); },
                [this](LDA_AbsoluteY op)   { set_register(Register::AC, fetch_from_absolute_indexed(op.address, Y)); },
                [this](LDA_IndirectX op)   { set_register(Register::AC, fetch_from_indirect_X(op.address)); },
                [this](LDA_IndirectY op)   { set_register(Register::AC, fetch_from_indirect_Y(op.address)); },

                [this](LDX_Immediate op)   { set_register(Register::X, op.value); },
                [this](LDX_ZeroPage op)    { set_register(Register::X, fetch_from_zero_page(op.address)); },
                [this](LDX_ZeroPageY op)   { set_register(Register::X, fetch_from_zero_page_indexed(op.address, Y)); },
                [this](LDX_Absolute op)    { set_register(Register::X, fetch_from_absolute(op.address)); },
                [this](LDX_AbsoluteY op)   { set_register(Register::X, fetch_from_absolute_indexed(op.address, Y)); },

                [this](LDY_Immediate op)   { set_register(Register::Y, op.value); },
                [this](LDY_ZeroPage op)    { set_register(Register::Y, fetch_from_zero_page(op.address)); },
                [this](LDY_ZeroPageX op)   { set_register(Register::Y, fetch_from_zero_page_indexed(op.address, X)); },
                [this](LDY_Absolute op)    { set_register(Register::Y, fetch_from_absolute(op.address)); },
                [this](LDY_AbsoluteX op)   { set_register(Register::Y, fetch_from_absolute_indexed(op.address, X)); },

                [this](LSR_Accumulator op) { set_register(Register::AC, shift_right(AC)); },
                [this](LSR_ZeroPage op)    { perform_at_zero_page(op.address, &MOS6502::shift_right); },
                [this](LSR_ZeroPageX op)   { perform_at_zero_page_indexed(op.address, X, &MOS6502::shift_right); },
                [this](LSR_Absolute op)    { perform_at_absolute(op.address, &MOS6502::shift_right); },
                [this](LSR_AbsoluteX op)   {
                    perform_at_absolute_indexed(op.address, X, &MOS6502::shift_right);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },

                [this](NOP op)             { cycle++; },

                [this](ORA_Immediate op)   { or_with_accumulator(op.value); },
                [this](ORA_ZeroPage op)    { or_with_accumulator(fetch_from_zero_page(op.address)); },
                [this](ORA_ZeroPageX op)   { or_with_accumulator(fetch_from_zero_page_indexed(op.address, X)); },
                [this](ORA_Absolute op)    { or_with_accumulator(fetch_from_absolute(op.address)); },
                [this](ORA_AbsoluteX op)   { or_with_accumulator(fetch_from_absolute_indexed(op.address, X)); },
                [this](ORA_AbsoluteY op)   { or_with_accumulator(fetch_from_absolute_indexed(op.address, Y)); },
                [this](ORA_IndirectX op)   { or_with_accumulator(fetch_from_indirect_X(op.address)); },
                [this](ORA_IndirectY op)   { or_with_accumulator(fetch_from_indirect_Y(op.address)); },

                [this](PHA op)             { push_byte_to_stack(AC); cycle++; },
                [this](PHP op)             { push_byte_to_stack(SR.to_byte()); cycle++; },

                [this](PLA op)             { set_register(Register::AC, pull_byte_from_stack()); cycle++; },
                [this](PLP op)             { set_register(Register::SR, pull_byte_from_stack()); cycle++; },

                [this](ROL_Accumulator op) { set_register(Register::AC, rotate_left(AC)); },
                [this](ROL_ZeroPage op)    { perform_at_zero_page(op.address, &MOS6502::rotate_left); },
                [this](ROL_ZeroPageX op)   { perform_at_zero_page_indexed(op.address, X, &MOS6502::rotate_left); },
                [this](ROL_Absolute op)    { perform_at_absolute(op.address, &MOS6502::rotate_left); },
                [this](ROL_AbsoluteX op)   {
                    perform_at_absolute_indexed(op.address, X, &MOS6502::rotate_left);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },

                [this](ROR_Accumulator op) { set_register(Register::AC, rotate_right(AC)); },
                [this](ROR_ZeroPage op)    { perform_at_zero_page(op.address, &MOS6502::rotate_right); },
                [this](ROR_ZeroPageX op)   { perform_at_zero_page_indexed(op.address, X, &MOS6502::rotate_right); },
                [this](ROR_Absolute op)    { perform_at_absolute(op.address, &MOS6502::rotate_right); },
                [this](ROR_AbsoluteX op)   {
                    perform_at_absolute_indexed(op.address, X, &MOS6502::rotate_right);
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
                [this](SBC_ZeroPage op)    { subtract_from_accumulator(fetch_from_zero_page(op.address)); },
                [this](SBC_ZeroPageX op)   { subtract_from_accumulator(fetch_from_zero_page_indexed(op.address, X)); },
                [this](SBC_Absolute op)    { subtract_from_accumulator(fetch_from_absolute(op.address)); },
                [this](SBC_AbsoluteX op)   { subtract_from_accumulator(fetch_from_absolute_indexed(op.address, X)); },
                [this](SBC_AbsoluteY op)   { subtract_from_accumulator(fetch_from_absolute_indexed(op.address, Y)); },
                [this](SBC_IndirectX op)   { subtract_from_accumulator(fetch_from_indirect_X(op.address)); },
                [this](SBC_IndirectY op)   { subtract_from_accumulator(fetch_from_indirect_Y(op.address)); },

                [this](SEC op)             { SR[Flag::CARRY] = SET; },
                [this](SED op)             { SR[Flag::DECIMAL] = SET; },
                [this](SEI op)             { SR[Flag::INTERRUPT_DISABLE] = SET; },

                [this](STA_ZeroPage op)    { write_to_zero_page(AC, op.address); },
                [this](STA_ZeroPageX op)   { write_to_zero_page_indexed(AC, op.address, X); },
                [this](STA_Absolute op)    { write_to_absolute(AC, op.address); },
                [this](STA_AbsoluteX op)   {
                    write_to_absolute_indexed(AC, op.address, X);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },
                [this](STA_AbsoluteY op)   {
                    write_to_absolute_indexed(AC, op.address, Y);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },
                [this](STA_IndirectX op)   { write_to_indirect_X(AC, op.address); },
                [this](STA_IndirectY op)   {
                    write_to_indirect_Y(AC, op.address);
                    // this instruction takes an additional cycle even when the page is not crossed
                    if (!pageCrossed) cycle++;
                    },

                [this](STX_ZeroPage op)    { write_to_zero_page(X, op.address); },
                [this](STX_ZeroPageY op)   { write_to_zero_page_indexed(X, op.address, Y); },
                [this](STX_Absolute op)    { write_to_absolute(X, op.address); },

                [this](STY_ZeroPage op)    { write_to_zero_page(Y, op.address); },
                [this](STY_ZeroPageX op)   { write_to_zero_page_indexed(Y, op.address, X); },
                [this](STY_Absolute op)    { write_to_absolute(Y, op.address); },

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

    void MOS6502::perform_at_zero_page(Byte address, MOS6502::ByteOperator byteOperator) noexcept {
        auto targetAddress = resolve_zero_page(address);
        memory.set_byte({.address = targetAddress, .value = (this->*byteOperator)(memory.fetch_byte(targetAddress, cycle)), .cycle = cycle});
    }

    void MOS6502::perform_at_zero_page_indexed(Byte address, Byte index, MOS6502::ByteOperator byteOperator) noexcept {
        auto targetAddress = resolve_zero_page_indexed(address, index);
        memory.set_byte({.address = targetAddress, .value = (this->*byteOperator)(memory.fetch_byte(targetAddress, cycle)), .cycle = cycle});
    }

    void MOS6502::perform_at_absolute(Word address, MOS6502::ByteOperator byteOperator) noexcept {
        auto targetAddress = resolve_absolute(address);
        memory.set_byte({.address = targetAddress, .value = (this->*byteOperator)(memory.fetch_byte(targetAddress, cycle)), .cycle = cycle});
    }

    void MOS6502::perform_at_absolute_indexed(Word address, Byte index, MOS6502::ByteOperator byteOperator) noexcept {
        auto targetAddress = resolve_absolute_indexed(address, index);
        memory.set_byte({.address = targetAddress, .value = (this->*byteOperator)(memory.fetch_byte(targetAddress, cycle)), .cycle = cycle});
    }

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

}






