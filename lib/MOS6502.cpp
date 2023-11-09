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


    std::string MOS6502::dump(bool include_memory) const noexcept {
        auto result = std::vformat("Registers: AC = {:d}, X = {:d}, Y = {:d}\n", std::make_format_args(AC, X, Y));
        result += std::vformat("Program counter = {:#04x}, Stack pointer = {:#02x}\n", std::make_format_args(PC, SP));
        result += std::vformat("Flags: {}\n", std::make_format_args(SR.to_string()));
        result += std::vformat("Current cycle = {:d}\n", std::make_format_args(cycle));

        result += "Zero page: ";
        for (Word i = 0; i <= UINT8_MAX; i++) result += std::vformat("{:#02x} ", std::make_format_args(memory[i]));

        result += "\nStack: ";
        for (int i = 0; i <= UINT8_MAX; i++) result += std::vformat("{:#02x} ", std::make_format_args(memory.stack((Byte)i)));

        result += std::vformat("\nSpecial addresses:\n\tnon-maskable interrupt handler = {:#04x}\n\tpower on reset location = {:#04x}\n\tBRK/interrupt request handler = {:#04x}\n",
                               std::make_format_args(memory.get_word(ROM::INTERRUPT_HANDLER), memory.get_word(ROM::RESET_LOCATION), memory.get_word(ROM::BRK_HANDLER)));

        if (include_memory) {
            result += "Remaining memory:\n";
            for (int i = 0x1000; i <= UINT16_MAX; i++)
                if (!Emulator::ROM::is_in_stack((Word)i)) result += std::vformat("{:#02x} ", std::make_format_args(memory[(Word)i]));
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



    std::expected<void, MOS6502::StackOverflow> MOS6502::push_byte_to_stack(Byte value) {
        if (SP == 0) return std::unexpected(StackOverflow(StackOverflow::Type::STACK_FULL));

        cycle++;
        memory.stack(SP--) = value;
        return {};
    }


    std::expected<Byte, MOS6502::StackOverflow> MOS6502::pull_byte_from_stack() {
        if (SP == UINT8_MAX) return std::unexpected<StackOverflow>{StackOverflow::Type::STACK_EMPTY};

        cycle += 2;
        return memory.stack(++SP);
    }


    std::expected<void, MOS6502::StackOverflow> MOS6502::push_word_to_stack(Word value) {
        WordToBytes buf(value);

        auto result = push_byte_to_stack(buf.high());
        if (!result.has_value()) return result;

        return push_byte_to_stack(buf.low());
    }


    std::expected<Word, MOS6502::StackOverflow> MOS6502::pull_word_from_stack() {
        WordToBytes buf{};

        auto result = pull_byte_from_stack();
        if (result.has_value()) buf.low() = result.value();

        return pull_byte_from_stack().transform([&buf](Byte byte) -> Word {
            buf.high() = byte;
            return buf.word;
        });
    }


    void MOS6502::reset() noexcept {
        PC = fetch_word(ROM::RESET_LOCATION);
        cycle = 7;
        SR[Flag::INTERRUPT_DISABLE] = true;
    }


    std::expected<MOS6502::SuccessfulTermination, MOS6502::ErrorTermination> MOS6502::execute() noexcept {
        size_t commandsExecuted = 0;
        while (true) {
            Word commandAddress = PC;

            // if there's no value of maxNumberOfCommandsToExecute, we should discard this condition
            if (commandsExecuted == maxNumberOfCommandsToExecute.value_or(commandsExecuted + 1))
                return StopOnMaxReached{.address = commandAddress};

            if (auto operation = fetch_operation(); operation.has_value()) {
                if (stopOnBRK && std::holds_alternative<BRK>(operation.value())) return StopOnBreak{.address = commandAddress};

                auto result = execute(operation.value());
                if (!result.has_value()) return std::unexpected{result.error()};
            }
            else return std::unexpected{ParseError(commandAddress)};

            commandsExecuted++;
        }
    }

    void MOS6502::set_writing_flags(Byte value) {
        SR[Flag::ZERO] = value == 0;
        SR[Flag::NEGATIVE] = (char)value < 0;
    }

    MOS6502::ExecutionResult MOS6502::execute(const Operation &operation) noexcept {
        return std::visit(Overload {
                [this](ADC_Immediate op) -> ExecutionResult   { add_to_accumulator(op.value); return {}; },
                [this](ADC_ZeroPage op) -> ExecutionResult    { add_to_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE)); return {}; },
                [this](ADC_ZeroPageX op) -> ExecutionResult   { add_to_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); return {}; },
                [this](ADC_Absolute op) -> ExecutionResult    { add_to_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE)); return {}; },
                [this](ADC_AbsoluteX op) -> ExecutionResult   { add_to_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_X)); return {}; },
                [this](ADC_AbsoluteY op) -> ExecutionResult   { add_to_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); return {}; },
                [this](ADC_IndirectX op) -> ExecutionResult   { add_to_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_X)); return {}; },
                [this](ADC_IndirectY op) -> ExecutionResult   { add_to_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_Y)); return {}; },

                [this](AND_Immediate op) -> ExecutionResult   { and_with_accumulator(op.value); return {}; },
                [this](AND_ZeroPage op) -> ExecutionResult    { and_with_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE)); return {}; },
                [this](AND_ZeroPageX op) -> ExecutionResult   { and_with_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); return {}; },
                [this](AND_Absolute op) -> ExecutionResult    { and_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE)); return {}; },
                [this](AND_AbsoluteX op) -> ExecutionResult   { and_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_X)); return {}; },
                [this](AND_AbsoluteY op) -> ExecutionResult   { and_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); return {}; },
                [this](AND_IndirectX op) -> ExecutionResult   { and_with_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_X)); return {}; },
                [this](AND_IndirectY op) -> ExecutionResult   { and_with_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_Y)); return {}; },

                [this](ASL_Accumulator) -> ExecutionResult    { set_register(Register::AC, shift_left(AC)); return {}; },
                [this](ASL_ZeroPage op) -> ExecutionResult    { return perform_at(op.address, AddressingMode::ZERO_PAGE, &MOS6502::shift_left); },
                [this](ASL_ZeroPageX op) -> ExecutionResult   { return perform_at(op.address, AddressingMode::ZERO_PAGE_X, &MOS6502::shift_left); },
                [this](ASL_Absolute op) -> ExecutionResult    { return perform_at(op.address, AddressingMode::ABSOLUTE, &MOS6502::shift_left); },
                [this](ASL_AbsoluteX op) -> ExecutionResult   {
                    return perform_at(op.address, AddressingMode::ABSOLUTE_X, &MOS6502::shift_left).transform([this]() {
                        // this instruction takes an additional cycle even when the page is not crossed
                        if (!pageCrossed) cycle++;
                    });
                },

                [this](BCS op) -> ExecutionResult             { if (SR[Flag::CARRY]) return branch(op.offset); return {}; },
                [this](BCC op) -> ExecutionResult             { if (!SR[Flag::CARRY]) return branch(op.offset); return {}; },
                [this](BEQ op) -> ExecutionResult             { if (SR[Flag::ZERO]) return branch(op.offset); return {}; },
                [this](BNE op) -> ExecutionResult             { if (!SR[Flag::ZERO]) return branch(op.offset); return {}; },
                [this](BMI op) -> ExecutionResult             { if (SR[Flag::NEGATIVE]) return branch(op.offset); return {}; },
                [this](BPL op) -> ExecutionResult             { if (!SR[Flag::NEGATIVE]) return branch(op.offset); return {}; },
                [this](BVS op) -> ExecutionResult             { if (SR[Flag::OVERFLOW_F]) return branch(op.offset); return {}; },
                [this](BVC op) -> ExecutionResult             { if (!SR[Flag::OVERFLOW_F]) return branch(op.offset); return {}; },

                [this](BIT_ZeroPage op) -> ExecutionResult    { bit_test(fetch_from(op.address, AddressingMode::ZERO_PAGE)); return {}; },
                [this](BIT_Absolute op) -> ExecutionResult    { bit_test(fetch_from(op.address, AddressingMode::ABSOLUTE)); return {}; },

                [this](BRK) -> ExecutionResult {
                    // for some reason, the byte right next to the BRK command must be skipped
                    return push_word_to_stack(PC + 1).transform([this](){
                        PC = fetch_word(ROM::BRK_HANDLER);
                        SR[Flag::BREAK] = SET;
                        cycle++;
                    });
                },

                [this](CLC) -> ExecutionResult                { SR[Flag::CARRY] = CLEAR; cycle++; return {}; },
                [this](CLD) -> ExecutionResult                { SR[Flag::DECIMAL] = CLEAR; cycle++; return {}; },
                [this](CLI) -> ExecutionResult                { SR[Flag::INTERRUPT_DISABLE] = CLEAR; cycle++; return {}; },
                [this](CLV) -> ExecutionResult                { SR[Flag::OVERFLOW_F] = CLEAR; cycle++; return {}; },

                [this](CMP_Immediate op) -> ExecutionResult   { compare(AC, op.value); return {}; },
                [this](CMP_ZeroPage op) -> ExecutionResult    { compare(AC, fetch_from(op.address, AddressingMode::ZERO_PAGE)); return {}; },
                [this](CMP_ZeroPageX op) -> ExecutionResult   { compare(AC, fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); return {}; },
                [this](CMP_Absolute op) -> ExecutionResult    { compare(AC, fetch_from(op.address, AddressingMode::ABSOLUTE)); return {}; },
                [this](CMP_AbsoluteX op) -> ExecutionResult   { compare(AC, fetch_from(op.address, AddressingMode::ABSOLUTE_X)); return {}; },
                [this](CMP_AbsoluteY op) -> ExecutionResult   { compare(AC, fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); return {}; },
                [this](CMP_IndirectX op) -> ExecutionResult   { compare(AC, fetch_from(op.address, AddressingMode::INDIRECT_X)); return {}; },
                [this](CMP_IndirectY op) -> ExecutionResult   { compare(AC, fetch_from(op.address, AddressingMode::INDIRECT_Y)); return {}; },

                [this](CPX_Immediate op) -> ExecutionResult   { compare(X, op.value); return {}; },
                [this](CPX_ZeroPage op) -> ExecutionResult    { compare(X, fetch_from(op.address, AddressingMode::ZERO_PAGE)); return {}; },
                [this](CPX_Absolute op) -> ExecutionResult    { compare(X, fetch_from(op.address, AddressingMode::ABSOLUTE)); return {}; },

                [this](CPY_Immediate op) -> ExecutionResult   { compare(Y, op.value); return {}; },
                [this](CPY_ZeroPage op) -> ExecutionResult    { compare(Y, fetch_from(op.address, AddressingMode::ZERO_PAGE)); return {}; },
                [this](CPY_Absolute op) -> ExecutionResult    { compare(Y, fetch_from(op.address, AddressingMode::ABSOLUTE)); return {}; },

                [this](DEC_ZeroPage op) -> ExecutionResult    { return perform_at(op.address, AddressingMode::ZERO_PAGE, &MOS6502::decrement); },
                [this](DEC_ZeroPageX op) -> ExecutionResult   { return perform_at(op.address, AddressingMode::ZERO_PAGE_X, &MOS6502::decrement); },
                [this](DEC_Absolute op) -> ExecutionResult    { return perform_at(op.address, AddressingMode::ABSOLUTE, &MOS6502::decrement); },
                [this](DEC_AbsoluteX op) -> ExecutionResult   {
                    return perform_at(op.address, AddressingMode::ABSOLUTE_X, &MOS6502::decrement).transform([this]() {
                        // this instruction takes an additional cycle even when the page is not crossed
                        if (!pageCrossed) cycle++;
                    });
                },

                [this](DEX) -> ExecutionResult                { set_register(Register::X, X - 1); cycle++; return {}; },
                [this](DEY) -> ExecutionResult                { set_register(Register::Y, Y - 1); cycle++; return {}; },

                [this](EOR_Immediate op) -> ExecutionResult   { xor_with_accumulator(op.value); return {}; },
                [this](EOR_ZeroPage op) -> ExecutionResult    { xor_with_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE)); return {}; },
                [this](EOR_ZeroPageX op) -> ExecutionResult   { xor_with_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); return {}; },
                [this](EOR_Absolute op) -> ExecutionResult    { xor_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE)); return {}; },
                [this](EOR_AbsoluteX op) -> ExecutionResult   { xor_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_X)); return {}; },
                [this](EOR_AbsoluteY op) -> ExecutionResult   { xor_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); return {}; },
                [this](EOR_IndirectX op) -> ExecutionResult   { xor_with_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_X)); return {}; },
                [this](EOR_IndirectY op) -> ExecutionResult   { xor_with_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_Y)); return {}; },

                [this](INC_ZeroPage op) -> ExecutionResult    { return perform_at(op.address, AddressingMode::ZERO_PAGE, &MOS6502::increment); },
                [this](INC_ZeroPageX op) -> ExecutionResult   { return perform_at(op.address, AddressingMode::ZERO_PAGE_X, &MOS6502::increment); },
                [this](INC_Absolute op) -> ExecutionResult    { return perform_at(op.address, AddressingMode::ABSOLUTE, &MOS6502::increment); },
                [this](INC_AbsoluteX op) -> ExecutionResult   {
                    return perform_at(op.address, AddressingMode::ABSOLUTE_X, &MOS6502::increment).transform([this]() {
                        // this instruction takes an additional cycle even when the page is not crossed
                        if (!pageCrossed) cycle++;
                    });
                },

                [this](INX) -> ExecutionResult                { set_register(Register::X, X + 1); cycle++; return {}; },
                [this](INY) -> ExecutionResult                { set_register(Register::Y, Y + 1); cycle++; return {}; },

                [this](JMP_Absolute op) -> ExecutionResult    { PC = op.address; return {}; },
                [this](JMP_Indirect op) -> ExecutionResult    { PC = fetch_word(op.address); return {}; },

                [this](JSR op) -> ExecutionResult {
                    return push_word_to_stack(PC - 1).transform([this, op](){
                        PC = op.address;
                        cycle++;
                    });
                },

                [this](LDA_Immediate op) -> ExecutionResult   { set_register(Register::AC, op.value); return {}; },
                [this](LDA_ZeroPage op) -> ExecutionResult    { set_register(Register::AC, fetch_from(op.address, AddressingMode::ZERO_PAGE)); return {}; },
                [this](LDA_ZeroPageX op) -> ExecutionResult   { set_register(Register::AC, fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); return {}; },
                [this](LDA_Absolute op) -> ExecutionResult    { set_register(Register::AC, fetch_from(op.address, AddressingMode::ABSOLUTE)); return {}; },
                [this](LDA_AbsoluteX op) -> ExecutionResult   { set_register(Register::AC, fetch_from(op.address, AddressingMode::ABSOLUTE_X)); return {}; },
                [this](LDA_AbsoluteY op) -> ExecutionResult   { set_register(Register::AC, fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); return {}; },
                [this](LDA_IndirectX op) -> ExecutionResult   { set_register(Register::AC, fetch_from(op.address, AddressingMode::INDIRECT_X)); return {}; },
                [this](LDA_IndirectY op) -> ExecutionResult   { set_register(Register::AC, fetch_from(op.address, AddressingMode::INDIRECT_Y)); return {}; },

                [this](LDX_Immediate op) -> ExecutionResult   { set_register(Register::X, op.value); return {}; },
                [this](LDX_ZeroPage op) -> ExecutionResult    { set_register(Register::X, fetch_from(op.address, AddressingMode::ZERO_PAGE)); return {}; },
                [this](LDX_ZeroPageY op) -> ExecutionResult   { set_register(Register::X, fetch_from(op.address, AddressingMode::ZERO_PAGE_Y)); return {}; },
                [this](LDX_Absolute op) -> ExecutionResult    { set_register(Register::X, fetch_from(op.address, AddressingMode::ABSOLUTE)); return {}; },
                [this](LDX_AbsoluteY op) -> ExecutionResult   { set_register(Register::X, fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); return {}; },

                [this](LDY_Immediate op) -> ExecutionResult   { set_register(Register::Y, op.value); return {}; },
                [this](LDY_ZeroPage op) -> ExecutionResult    { set_register(Register::Y, fetch_from(op.address, AddressingMode::ZERO_PAGE)); return {}; },
                [this](LDY_ZeroPageX op) -> ExecutionResult   { set_register(Register::Y, fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); return {}; },
                [this](LDY_Absolute op) -> ExecutionResult    { set_register(Register::Y, fetch_from(op.address, AddressingMode::ABSOLUTE)); return {}; },
                [this](LDY_AbsoluteX op) -> ExecutionResult   { set_register(Register::Y, fetch_from(op.address, AddressingMode::ABSOLUTE_X)); return {}; },

                [this](LSR_Accumulator) -> ExecutionResult    { set_register(Register::AC, shift_right(AC)); return {}; },
                [this](LSR_ZeroPage op) -> ExecutionResult    { return perform_at(op.address, AddressingMode::ZERO_PAGE, &MOS6502::shift_right); },
                [this](LSR_ZeroPageX op) -> ExecutionResult   { return perform_at(op.address, AddressingMode::ZERO_PAGE_X, &MOS6502::shift_right); },
                [this](LSR_Absolute op) -> ExecutionResult    { return perform_at(op.address, AddressingMode::ABSOLUTE, &MOS6502::shift_right); },
                [this](LSR_AbsoluteX op) -> ExecutionResult   {
                    return perform_at(op.address, AddressingMode::ABSOLUTE_X, &MOS6502::shift_right).transform([this]() {
                        // this instruction takes an additional cycle even when the page is not crossed
                        if (!pageCrossed) cycle++;
                    });
                },

                [this](NOP) -> ExecutionResult                { cycle++; return {}; },

                [this](ORA_Immediate op) -> ExecutionResult   { or_with_accumulator(op.value); return {}; },
                [this](ORA_ZeroPage op) -> ExecutionResult    { or_with_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE)); return {}; },
                [this](ORA_ZeroPageX op) -> ExecutionResult   { or_with_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); return {}; },
                [this](ORA_Absolute op) -> ExecutionResult    { or_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE)); return {}; },
                [this](ORA_AbsoluteX op) -> ExecutionResult   { or_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_X)); return {}; },
                [this](ORA_AbsoluteY op) -> ExecutionResult   { or_with_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); return {}; },
                [this](ORA_IndirectX op) -> ExecutionResult   { or_with_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_X)); return {}; },
                [this](ORA_IndirectY op) -> ExecutionResult   { or_with_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_Y)); return {}; },

                [this](PHA) -> ExecutionResult                { return push_byte_to_stack(AC); cycle++; },
                [this](PHP) -> ExecutionResult                { return push_byte_to_stack(SR.to_byte()); cycle++; },

                [this](PLA) -> ExecutionResult {
                    return pull_byte_from_stack().transform([this](Byte value){
                        set_register(Register::AC, value);
                        cycle++;
                    });
                },
                [this](PLP) -> ExecutionResult {
                    return pull_byte_from_stack().transform([this](Byte value){
                        set_register(Register::SR, value);
                        cycle++;
                    });
                },

                [this](ROL_Accumulator) -> ExecutionResult    { set_register(Register::AC, rotate_left(AC)); return {}; },
                [this](ROL_ZeroPage op) -> ExecutionResult    { return perform_at(op.address, AddressingMode::ZERO_PAGE, &MOS6502::rotate_left); },
                [this](ROL_ZeroPageX op) -> ExecutionResult   { return perform_at(op.address, AddressingMode::ZERO_PAGE_X, &MOS6502::rotate_left); },
                [this](ROL_Absolute op) -> ExecutionResult    { return perform_at(op.address, AddressingMode::ABSOLUTE, &MOS6502::rotate_left); },
                [this](ROL_AbsoluteX op) -> ExecutionResult   {
                    return perform_at(op.address, AddressingMode::ABSOLUTE_X, &MOS6502::rotate_left).transform([this]() {
                        // this instruction takes an additional cycle even when the page is not crossed
                        if (!pageCrossed) cycle++;
                    });
                },

                [this](ROR_Accumulator) -> ExecutionResult    { set_register(Register::AC, rotate_right(AC)); return {}; },
                [this](ROR_ZeroPage op) -> ExecutionResult    { return perform_at(op.address, AddressingMode::ZERO_PAGE, &MOS6502::rotate_right); },
                [this](ROR_ZeroPageX op) -> ExecutionResult   { return perform_at(op.address, AddressingMode::ZERO_PAGE_X, &MOS6502::rotate_right); },
                [this](ROR_Absolute op) -> ExecutionResult    { return perform_at(op.address, AddressingMode::ABSOLUTE, &MOS6502::rotate_right); },
                [this](ROR_AbsoluteX op) -> ExecutionResult   {
                    return perform_at(op.address, AddressingMode::ABSOLUTE_X, &MOS6502::rotate_right).transform([this]() {
                        // this instruction takes an additional cycle even when the page is not crossed
                        if (!pageCrossed) cycle++;
                    });
                },

                [this](RTI) -> ExecutionResult {
                    auto result = pull_byte_from_stack();
                    if (result.has_value()) SR = result.value();
                    else return std::unexpected{result.error()};

                    return pull_byte_from_stack().transform([this](Byte value){
                        PC = value;
                        cycle--;
                    });
                },

                [this](RTS) -> ExecutionResult {
                    return pull_word_from_stack().transform([this](Word value){
                        PC = value + 1;
                        cycle++;
                    });
                },

                [this](SBC_Immediate op) -> ExecutionResult   { subtract_from_accumulator(op.value); return {}; },
                [this](SBC_ZeroPage op) -> ExecutionResult    { subtract_from_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE)); return {}; },
                [this](SBC_ZeroPageX op) -> ExecutionResult   { subtract_from_accumulator(fetch_from(op.address, AddressingMode::ZERO_PAGE_X)); return {}; },
                [this](SBC_Absolute op) -> ExecutionResult    { subtract_from_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE)); return {}; },
                [this](SBC_AbsoluteX op) -> ExecutionResult   { subtract_from_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_X)); return {}; },
                [this](SBC_AbsoluteY op) -> ExecutionResult   { subtract_from_accumulator(fetch_from(op.address, AddressingMode::ABSOLUTE_Y)); return {}; },
                [this](SBC_IndirectX op) -> ExecutionResult   { subtract_from_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_X)); return {}; },
                [this](SBC_IndirectY op) -> ExecutionResult   { subtract_from_accumulator(fetch_from(op.address, AddressingMode::INDIRECT_Y)); return {}; },

                [this](SEC) -> ExecutionResult                { SR[Flag::CARRY] = SET; return {}; },
                [this](SED) -> ExecutionResult                { SR[Flag::DECIMAL] = SET; return {}; },
                [this](SEI) -> ExecutionResult                { SR[Flag::INTERRUPT_DISABLE] = SET; return {}; },

                [this](STA_ZeroPage op) -> ExecutionResult    { return write_to(op.address, AddressingMode::ZERO_PAGE, AC); },
                [this](STA_ZeroPageX op) -> ExecutionResult   { return write_to(op.address, AddressingMode::ZERO_PAGE_X, AC); },
                [this](STA_Absolute op) -> ExecutionResult    { return write_to(op.address, AddressingMode::ABSOLUTE, AC); },
                [this](STA_AbsoluteX op) -> ExecutionResult   {
                    return write_to(op.address, AddressingMode::ABSOLUTE_X, AC).transform([this](){
                        // this instruction takes an additional cycle even when the page is not crossed
                        if (!pageCrossed) cycle++;
                    });
                },
                [this](STA_AbsoluteY op) -> ExecutionResult {
                    return write_to(op.address, AddressingMode::ABSOLUTE_Y, AC).transform([this](){
                        // this instruction takes an additional cycle even when the page is not crossed
                        if (!pageCrossed) cycle++;
                    });
                },
                [this](STA_IndirectX op) -> ExecutionResult   { return write_to(op.address, AddressingMode::INDIRECT_X, AC); },
                [this](STA_IndirectY op) -> ExecutionResult {
                    return write_to(op.address, AddressingMode::INDIRECT_Y, AC).transform([this](){
                        // this instruction takes an additional cycle even when the page is not crossed
                        if (!pageCrossed) cycle++;
                    });
                },

                [this](STX_ZeroPage op) -> ExecutionResult    { return write_to(op.address, AddressingMode::ZERO_PAGE, X); },
                [this](STX_ZeroPageY op) -> ExecutionResult   { return write_to(op.address, AddressingMode::ZERO_PAGE_Y, X); },
                [this](STX_Absolute op) -> ExecutionResult    { return write_to(op.address, AddressingMode::ABSOLUTE, X); },

                [this](STY_ZeroPage op) -> ExecutionResult    { return write_to(op.address, AddressingMode::ZERO_PAGE, Y); },
                [this](STY_ZeroPageX op) -> ExecutionResult   { return write_to(op.address, AddressingMode::ZERO_PAGE_X, Y); },
                [this](STY_Absolute op) -> ExecutionResult    { return write_to(op.address, AddressingMode::ABSOLUTE, Y); },

                [this](TAX) -> ExecutionResult                { set_register(Register::X, AC); cycle++; return {}; },
                [this](TAY) -> ExecutionResult                { set_register(Register::Y, AC); cycle++; return {}; },
                [this](TSX) -> ExecutionResult                { set_register(Register::X, SP); cycle++; return {}; },
                [this](TXA) -> ExecutionResult                { set_register(Register::AC, X); cycle++; return {}; },
                [this](TXS) -> ExecutionResult                { set_register(Register::SP, X); cycle++; return {}; },
                [this](TYA) -> ExecutionResult                { set_register(Register::AC, Y); cycle++; return {}; }
            },
                   operation
        );
    }

    void MOS6502::add_to_accumulator(Byte value) noexcept {
        SR[Flag::OVERFLOW_F] = SR[Flag::CARRY];
        add_with_overflow((char)AC, (char)value, SR[Flag::OVERFLOW_F], INT8_MIN, INT8_MAX);

        set_register(Register::AC, (Byte)add_with_overflow(AC, value, SR[Flag::CARRY], 0, UINT8_MAX));
    }

    void MOS6502::and_with_accumulator(Byte value) noexcept {
        set_register(Register::AC, AC & value);
    }

    Byte MOS6502::shift_left(Byte value) noexcept {
        cycle++;
        SR[Flag::CARRY] = get_bit(value, 7);

        const Byte result = (Byte)(value << 1);
        set_writing_flags(result);
        return result;
    }

    std::expected<void, MOS6502::AddressOverflow> MOS6502::branch(char offset) noexcept {
        if (offset > UINT16_MAX - PC || (offset < 0 && -offset > PC)) return std::unexpected(AddressOverflow());

        PC = (Word)(PC + offset);
        cycle++;
        return {};
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
        Byte newValue = (Byte)(value << 1);
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

        set_register(Register::AC, (Byte)subtract_with_overflow(AC, value, SR[Flag::CARRY], 0, UINT8_MAX));
    }

    Byte MOS6502::index_zero_page(Byte address, Byte index) noexcept {
        cycle++;
        pageCrossed = index > UINT8_MAX - address;
        return address + index;
    }

    Word MOS6502::index_absolute(Word address, Byte index) noexcept {
        Word result = address + index;
        pageCrossed = WordToBytes(result).high() != WordToBytes(address).high();
        if (pageCrossed) cycle++;
        return result;
    }


    Word MOS6502::fetch_word() noexcept {
        WordToBytes result;
        result.low() = memory.fetch_byte(PC++, cycle);
        result.high() = memory.fetch_byte(PC++, cycle);
        return result.word;
    }

    Word MOS6502::fetch_word(Word address) noexcept {
        WordToBytes result;
        result.low() = memory.fetch_byte(address, cycle);
        result.high() = memory.fetch_byte(address + 1, cycle);
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
            case AddressingMode::ZERO_PAGE_X: return index_zero_page((Byte)address, X);
            case AddressingMode::ZERO_PAGE_Y: return index_zero_page((Byte)address, Y);
            case AddressingMode::ABSOLUTE:    return address;
            case AddressingMode::ABSOLUTE_X:  return index_absolute(address, X);
            case AddressingMode::ABSOLUTE_Y:  return index_absolute(address, Y);
            case AddressingMode::INDIRECT_X:  return fetch_word(index_zero_page((Byte)address, X));
            case AddressingMode::INDIRECT_Y:  return index_absolute(fetch_word(address), Y);
        }

        std::unreachable();
    }

    Byte MOS6502::fetch_from(Word address, AddressingMode mode) noexcept {
        return memory.fetch_byte(resolve(address, mode), cycle);
    }

    std::expected<void, ROM::StackOverride> MOS6502::write_to(Word address, AddressingMode mode, Byte value) noexcept {
        return memory.set_byte(resolve(address, mode), value, cycle);
    }

    std::expected<void, ROM::StackOverride> MOS6502::perform_at(Word address, AddressingMode mode, MOS6502::ByteOperator byteOperator) noexcept {
        auto targetAddress = resolve(address, mode);
        return memory.set_byte(targetAddress, (this->*byteOperator)(memory.fetch_byte(targetAddress, cycle)), cycle);
    }

}






