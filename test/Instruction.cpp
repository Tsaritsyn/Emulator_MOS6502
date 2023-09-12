//
// Created by Mikhail on 08/09/2023.
//

#include <cassert>

#include "Instruction.hpp"

std::ostream &operator<<(std::ostream &os, const InstructionArguments &instruction) {
    if (const auto arithmetics = std::get_if<Arithmetics>(&instruction)) {
        switch (arithmetics->operation) {
            case ArithmeticOperation::ADD:
                os << "ADC";
                break;
            case ArithmeticOperation::SUBTRACT:
                os << "SBC";
                break;
        }
        return os << "(AC: " << (int)arithmetics->AC << ", memory: " << (int)arithmetics->memory << ", carry: " << arithmetics->carry << ")";
    }

    else if (const auto logical = std::get_if<Logical>(&instruction)) {
        switch (logical->operation) {
            case Emulator::LogicalOperation::AND:
                os << "AND";
                break;
            case Emulator::LogicalOperation::OR:
                os << "ORA";
                break;
            case Emulator::LogicalOperation::XOR:
                os << "EOR";
                break;
        }
        return os << "(AC: " << HEX_BYTE(logical->AC) << ", memory: " << HEX_BYTE(logical->memory) << ")";
    }

    else if (const auto shiftLeft = std::get_if<ShiftLeft>(&instruction))
        return os << "ASL(value: " << shiftLeft->value << ")";

    else if (const auto branch = std::get_if<Branch>(&instruction)) {
        switch (branch->flag) {
            case Emulator::CARRY:
                return os << (branch->targetValue ? "BCS" : "BCC") << "(carry: " << branch->value << ")";
            case Emulator::ZERO:
                return os << (branch->targetValue ? "BEQ" : "BNE") << "(zero: " << branch->value << ")";
            case Emulator::NEGATIVE:
                return os << (branch->targetValue ? "BMI" : "BPL") << "(negative: " << branch->value << ")";
            case Emulator::OVERFLOW:
                return os << (branch->targetValue ? "BVS" : "BVC") << "(overflow: " << branch->value << ")";
            default:
                throw std::invalid_argument("operator<<: invalid flag for Branch instruction");
        }
    }

    else if (const auto bitTest = std::get_if<BitTest>(&instruction))
        return os << "BIT(AC: " << bitTest->AC << ", memory: " << bitTest->memory << ")";

    else if (std::get_if<ForceInterrupt>(&instruction))
        return os << "BRK";

    else if (const auto setFlag = std::get_if<SetFlag>(&instruction)) {
        switch (setFlag->flag) {
            case Emulator::CARRY:
                return os << (setFlag->value ? "SEC" : "CLC");
            case Emulator::DECIMAL:
                return os << (setFlag->value ? "SED" : "CLD");
            case Emulator::INTERRUPT_DISABLE:
                return os << (setFlag->value ? "SEI" : "CLI");
            case Emulator::OVERFLOW:
                assert(setFlag->value == CLEAR);
                return os << "CLV";
            default:
                throw std::invalid_argument("operator<<: invalid flag for SetFlag instruction");
        }
    }

    else if (const auto compare = std::get_if<Compare>(&instruction)) {
        switch (compare->reg) {
            case Emulator::Register::AC:
                return os << "CMP(AC: " << compare->value << ", memory: " << compare->memory << ")";
            case Emulator::Register::X:
                return os << "CPX(X: " << compare->value << ", memory: " << compare->memory << ")";
            case Emulator::Register::Y:
                return os << "CPY(Y: " << compare->value << ", memory: " << compare->memory << ")";
            default:
                throw std::invalid_argument("operator<<: invalid register for Compare instruction");
        }
    }

    else if (const auto decrementMemory = std::get_if<DecrementMemory>(&instruction))
        return os << "DEC(memory: " << decrementMemory->memory << ")";

    else if (const auto decrementRegister = std::get_if<DecrementRegister>(&instruction))
        switch (decrementRegister->reg) {
            case Emulator::Register::X:
                return os << "DEX(X: " << decrementRegister->value << ")";
            case Emulator::Register::Y:
                return os << "DEY(Y: " << decrementRegister->value << ")";
            default:
                throw std::invalid_argument("operator<<: invalid register for Decrement instruction");
        }

    else if (const auto incrementMemory = std::get_if<IncrementMemory>(&instruction))
        return os << "INC(memory: " << incrementMemory->memory << ")";

    else if (const auto incrementRegister = std::get_if<IncrementRegister>(&instruction))
        switch (incrementRegister->reg) {
            case Emulator::Register::X:
                return os << "INX(X: " << incrementRegister->value << ")";
            case Emulator::Register::Y:
                return os << "INY(Y: " << incrementRegister->value << ")";
            default:
                throw std::invalid_argument("operator<<: invalid register for Increment instruction");
        }

    else if (std::get_if<Jump>(&instruction))
        return os << "JMP";

    else if (std::get_if<JumpToSubroutine>(&instruction))
        return os << "JSR";

    else if (const auto load = std::get_if<Load>(&instruction))
        switch (load->reg) {
            case Emulator::Register::AC:
                return os << "LDA(value: " << load->value << ")";
            case Emulator::Register::X:
                return os << "LDX(value: " << load->value << ")";
            case Emulator::Register::Y:
                return os << "LDY(value: " << load->value << ")";
            default:
                throw std::invalid_argument("operator<<: invalid register for Load instruction");
        }

    else if (const auto shiftRight = std::get_if<ShiftRight>(&instruction))
        return os << "LSR(value: " << shiftRight->value << ")";

    else if (std::get_if<NoOperation>(&instruction))
        return os << "NOP";

    else if (const auto push = std::get_if<Push>(&instruction)) {
        switch (push->reg) {
            case Emulator::Register::AC:
                os << "PHA";
                break;
            case Emulator::Register::SR:
                os << "PHP";
                break;
            default:
                throw std::invalid_argument("operator<<: invalid register for Push instruction");
        }
        return os << "(value: " << push->value << ")";
    }

    else if (const auto pull = std::get_if<Pull>(&instruction)) {
        switch (pull->reg) {
            case Emulator::Register::AC:
                os << "PLA";
                break;
            case Emulator::Register::SR:
                os << "PLP";
                break;
            default:
                throw std::invalid_argument("operator<<: invalid register for Push instruction");
        }
        return os << "(value: " << pull->value << ")";
    }

    else if (const auto rotateLeft = std::get_if<RotateLeft>(&instruction))
        return os << "ROL(value: " << rotateLeft->value << ")";

    else if (const auto rotateRight = std::get_if<RotateRight>(&instruction))
        return os << "ROR(value: " << rotateRight->value << ")";

    else if (std::get_if<ReturnFromInterrupt>(&instruction))
        return os << "RTI";

    else if (std::get_if<ReturnFromSubroutine>(&instruction))
        return os << "RTS";

    else if (const auto store = std::get_if<Store>(&instruction))
        switch (store->reg) {
            case Emulator::Register::AC:
                return os << "STA(value: " << store->value << ")";
            case Emulator::Register::X:
                return os << "STX(value: " << store->value << ")";
            case Emulator::Register::Y:
                return os << "STY(value: " << store->value << ")";
            default:
                throw std::invalid_argument("operator<<: invalid register for Store instruction");
        }

    else if (const auto transfer = std::get_if<Transfer>(&instruction))
        switch (transfer->from) {
            case Emulator::Register::AC:
                switch (transfer->to) {
                    case Emulator::Register::X:
                        return os << "TAX(value: " << transfer->value << ")";
                    case Emulator::Register::Y:
                        return os << "TAY(value: " << transfer->value << ")";
                    default:
                        throw std::invalid_argument("operator<<: invalid target register for Transfer instruction");
                }
                case Emulator::Register::X:
                    assert(transfer->to == Emulator::Register::AC);
                    return os << "TXA(value: " << transfer->value << ")";
            case Emulator::Register::Y:
                assert(transfer->to == Emulator::Register::AC);
                return os << "TYA(value: " << transfer->value << ")";
            default:
                throw std::invalid_argument("operator<<: invalid source register for Transfer instruction");
        }
}


Instruction instruction_code(const InstructionArguments &args) {
    if (const auto arithmetics = std::get_if<Arithmetics>(&args)) {
        switch (arithmetics->operation) {
            case ArithmeticOperation::ADD:
                return Instruction::ADC;
            case ArithmeticOperation::SUBTRACT:
                return Instruction::SBC;
        }
    }
    else if (const auto logical = std::get_if<Logical>(&args)) {
        switch (logical->operation) {
            case LogicalOperation::AND:
                return Instruction::AND;
            case LogicalOperation::OR:
                return Instruction::ORA;
            case LogicalOperation::XOR:
                return Instruction::EOR;
        }
    }
    else if (std::get_if<ShiftLeft>(&args))
        return Emulator::Instruction::ASL;
    else if (const auto branch = std::get_if<Branch>(&args)) {
        switch (branch->flag) {
            case Emulator::CARRY:
                return branch->value ? Instruction::BCS : Instruction::BCC;
            case Emulator::ZERO:
                return branch->value ? Instruction::BEQ : Instruction::BNE;
            case Emulator::NEGATIVE:
                return branch->value ? Instruction::BMI : Instruction::BPL;
            case Emulator::OVERFLOW:
                return branch->value ? Instruction::BVS : Instruction::BVC;
            default:
                throw std::invalid_argument("instruction_code: invalid flag for Branch instruction");
        }
    }
    else if (std::get_if<BitTest>(&args))
        return Instruction::BIT;
    else if (std::get_if<ForceInterrupt>(&args))
        return Emulator::Instruction::BRK;
    else if (const auto setFlag = std::get_if<SetFlag>(&args)) {
        switch (setFlag->flag) {
            case Emulator::CARRY:
                return setFlag->value ? Emulator::Instruction::SEC : Emulator::Instruction::CLC;
            case Emulator::DECIMAL:
                return setFlag->value ? Emulator::Instruction::SED : Emulator::Instruction::CLD;
            case Emulator::INTERRUPT_DISABLE:
                return setFlag->value ? Emulator::Instruction::SEI : Emulator::Instruction::CLI;
            case Emulator::OVERFLOW:
                return Emulator::Instruction::CLV;
            default:
                throw std::invalid_argument("instruction_code: invalid flag for SetFlag instruction");
        }
    }
    else if (const auto compare = std::get_if<Compare>(&args)) {
        switch (compare->reg) {
            case Emulator::Register::AC:
                return Emulator::Instruction::CMP;
            case Emulator::Register::X:
                return Emulator::Instruction::CPX;
            case Emulator::Register::Y:
                return Emulator::Instruction::CPY;
            default:
                throw std::invalid_argument("instruction_code: invalid register for Compare instruction");
        }
    }
    else if (std::get_if<DecrementMemory>(&args))
        return Emulator::Instruction::DEC;
    else if (const auto decrement = std::get_if<DecrementRegister>(&args)) {
        switch (decrement->reg) {
            case Emulator::Register::X:
                return Emulator::Instruction::DEX;
            case Emulator::Register::Y:
                return Emulator::Instruction::DEY;
            default:
                throw std::invalid_argument("instruction_code: invalid register for Decrement instruction");
        }
    }
    else if (std::get_if<IncrementMemory>(&args))
        return Emulator::Instruction::INC;
    else if (const auto increment = std::get_if<IncrementRegister>(&args)) {
        switch (increment->reg) {
            case Emulator::Register::X:
                return Emulator::Instruction::INX;
            case Emulator::Register::Y:
                return Emulator::Instruction::INY;
            default:
                throw std::invalid_argument("instruction_code: invalid register for Increment instruction");
        }
    }
    else if (std::get_if<Jump>(&args))
        return Emulator::Instruction::JMP;
    else if (std::get_if<JumpToSubroutine>(&args))
        return Emulator::Instruction::JSR;
    else if (const auto load = std::get_if<Load>(&args)) {
        switch (load->reg) {
            case Emulator::Register::AC:
                return Emulator::Instruction::LDA;
            case Emulator::Register::X:
                return Emulator::Instruction::LDX;
            case Emulator::Register::Y:
                return Emulator::Instruction::LDY;
            default:
                throw std::invalid_argument("instruction_code: invalid register for Load instruction");
        }
    }
    else if (std::get_if<ShiftRight>(&args))
        return Emulator::Instruction::LSR;
    else if (std::get_if<NoOperation>(&args))
        return Emulator::Instruction::NOP;
    else if (const auto pull = std::get_if<Pull>(&args)) {
        switch (pull->reg) {
            case Emulator::Register::AC:
                return Emulator::Instruction::PLA;
            case Emulator::Register::SR:
                return Emulator::Instruction::PLP;
            default:
                throw std::invalid_argument("instruction_code: invalid register for Pull instruction");
        }
    }
    else if (std::get_if<RotateLeft>(&args))
        return Emulator::Instruction::ROL;
    else if (std::get_if<RotateRight>(&args))
        return Emulator::Instruction::ROR;
    else if (std::get_if<ReturnFromInterrupt>(&args))
        return Emulator::Instruction::RTI;
    else if (std::get_if<ReturnFromSubroutine>(&args))
        return Emulator::Instruction::RTS;
    else if (const auto store = std::get_if<Store>(&args)) {
        switch (store->reg) {
            case Emulator::Register::AC:
                return Emulator::Instruction::STA;
            case Emulator::Register::X:
                return Emulator::Instruction::STX;
            case Emulator::Register::Y:
                return Emulator::Instruction::STY;
            default:
                throw std::invalid_argument("instruction_code: invalid register for Store instruction");
        }
    }
    else if (const auto transfer = std::get_if<Transfer>(&args)) {
        switch (transfer->from) {
            case Emulator::Register::AC:
                switch (transfer->to) {
                    case Emulator::Register::X:
                        return Emulator::Instruction::TAX;
                    case Emulator::Register::Y:
                        return Emulator::Instruction::TAY;
                    default:
                        throw std::invalid_argument("instruction_code: invalid target register for Transfer instruction");
                }
            case Emulator::Register::X:
                assert(transfer->to == Emulator::Register::AC);
                return Emulator::Instruction::TXA;
            case Emulator::Register::Y:
                assert(transfer->to == Emulator::Register::AC);
                return Emulator::Instruction::TYA;
            default:
                throw std::invalid_argument("instruction_code: invalid source register for Transfer instruction");
        }
    }
    else throw std::invalid_argument("instruction: no instruction with given arguments");
}


bool page_crossed(const Addressing &addressing) {
    if (const auto absoluteX = std::get_if<AbsoluteX>(&addressing))
        return (Word)WordToBytes{absoluteX->address}.low + absoluteX->X >= UINT8_MAX;
    else if (const auto absoluteY = std::get_if<AbsoluteY>(&addressing))
        return (Word)WordToBytes{absoluteY->address}.low + absoluteY->Y >= UINT8_MAX;
    else if (const auto indirectY = std::get_if<IndirectY>(&addressing))
        return (Word)WordToBytes{indirectY->targetAddress}.low + indirectY->Y >= UINT8_MAX;
    else if (const auto relative = std::get_if<Relative>(&addressing)) {
        const int newPC = (int)WordToBytes{relative->PC}.low + relative->offset;
        return newPC >= UINT8_MAX || newPC < 0;
    }
    else return false;
}


size_t arithmetic_duration(const Addressing &addressing) {
    if (std::get_if<Immediate>(&addressing)) return 2;
    else if (std::get_if<ZeroPage>(&addressing)) return 3;
    else if (std::get_if<ZeroPageX>(&addressing)) return 4;
    else if (std::get_if<Absolute>(&addressing)) return 4;
    else if (std::get_if<AbsoluteX>(&addressing)) return 4 + page_crossed(addressing);
    else if (const auto absoluteY = std::get_if<AbsoluteY>(&addressing)) return 4 + page_crossed(addressing);
    else if (std::get_if<IndirectX>(&addressing)) return 6;
    else if (const auto indirectY = std::get_if<IndirectY>(&addressing)) return 5 + page_crossed(addressing);
    else throw std::invalid_argument("instruction_duration: unsupported addressing for Arithmetics or Logical instructions");
}


size_t instruction_duration(const InstructionArguments &instruction, const Addressing &addressing) {
    if (std::get_if<Arithmetics>(&instruction) ||
        std::get_if<Logical>(&instruction))
        return arithmetic_duration(addressing);

    else if (std::get_if<ShiftLeft>(&instruction) ||
             std::get_if<ShiftRight>(&instruction) ||
             std::get_if<RotateRight>(&instruction)) {
        if (std::get_if<Accumulator>(&addressing)) return 2;
        else if (std::get_if<ZeroPage>(&addressing)) return 5;
        else if (std::get_if<ZeroPageX>(&addressing)) return 6;
        else if (std::get_if<Absolute>(&addressing)) return 6;
        else if (std::get_if<AbsoluteX>(&addressing)) return 7;
    }

    else if (const auto branch = std::get_if<Branch>(&instruction)) {
        assert(std::get_if<Relative>(&addressing) != nullptr);
        if (branch->value != branch->targetValue) return 2;
        else return 3 + page_crossed(addressing);
    }

    else if (std::get_if<BitTest>(&instruction)) {
        if (std::get_if<ZeroPage>(&addressing)) return 3;
        else if (std::get_if<Absolute>(&addressing)) return 4;
        else throw std::invalid_argument("instruction_duration: unsupported addressing for BitTest instructions");
    }

    else if (std::get_if<ForceInterrupt>(&instruction)) {
        assert(std::get_if<Implicit>(&addressing) != nullptr);
        return 7;
    }

    else if (std::get_if<SetFlag>(&instruction)) {
        assert(std::get_if<Implicit>(&addressing) != nullptr);
        return 2;
    }

    else if (const auto compare = std::get_if<Compare>(&instruction)) {
        switch (compare->reg) {
            case Emulator::Register::AC:
                return arithmetic_duration(addressing);

            case Emulator::Register::X: case Emulator::Register::Y: {
                if (std::get_if<Immediate>(&addressing)) return 2;
                else if (std::get_if<ZeroPage>(&addressing)) return 3;
                else if (std::get_if<Absolute>(&addressing)) return 4;
                else throw std::invalid_argument("instruction_duration: unsupported addressing for CMP instruction");
            }

            default:
                throw std::invalid_argument("instruction_duration: unsupported register for Compare instruction");
        }
    }

    else if (std::get_if<DecrementMemory>(&instruction) ||
             std::get_if<IncrementMemory>(&instruction)) {
        if (std::get_if<ZeroPage>(&addressing)) return 5;
        else if (std::get_if<ZeroPageX>(&addressing)) return 6;
        else if (std::get_if<Absolute>(&addressing)) return 6;
        else if (std::get_if<AbsoluteX>(&addressing)) return 7;
        else throw std::invalid_argument("instruction_duration: unsupported addressing for DEC or INC instruction");
    }

    else if (std::get_if<DecrementRegister>(&instruction) ||
             std::get_if<IncrementRegister>(&instruction)) {
        assert(std::get_if<Implicit>(&addressing) != nullptr);
        return 2;
    }

    else if (std::get_if<Jump>(&instruction)) {
        if (std::get_if<Absolute>(&addressing)) return 3;
        else if (std::get_if<Indirect>(&addressing)) return 5;
        else throw std::invalid_argument("instruction_duration: unsupported addressing for JMP instruction");
    }

    else if (std::get_if<JumpToSubroutine>(&instruction)) {
        assert(std::get_if<Absolute>(&addressing) != nullptr);
        return 6;
    }

    else if (const auto load = std::get_if<Load>(&instruction)) {
        switch (load->reg) {
            case Emulator::Register::AC:
                return arithmetic_duration(addressing);
            case Emulator::Register::X: {
                if (std::get_if<Immediate>(&addressing)) return 2;
                else if (std::get_if<ZeroPage>(&addressing)) return 3;
                else if (std::get_if<ZeroPageY>(&addressing)) return 4;
                else if (std::get_if<Absolute>(&addressing)) return 4;
                else if (const auto absoluteY = std::get_if<AbsoluteY>(&addressing)) return 4 + page_crossed(addressing);
                else throw std::invalid_argument("instruction_duration: invalid addressing for LDX instruction");
            }
            case Emulator::Register::Y: {
                if (std::get_if<Immediate>(&addressing)) return 2;
                else if (std::get_if<ZeroPage>(&addressing)) return 3;
                else if (std::get_if<ZeroPageX>(&addressing)) return 4;
                else if (std::get_if<Absolute>(&addressing)) return 4;
                else if (const auto absoluteX = std::get_if<AbsoluteX>(&addressing)) return 4 + page_crossed(addressing);
                else throw std::invalid_argument("instruction_duration: invalid addressing for LDY instruction");
            }
        }
    }

    else if (std::get_if<NoOperation>(&instruction))
        return 2;

    else if (std::get_if<Push>(&instruction)) {
        assert(std::get_if<Implicit>(&addressing));
        return 3;
    }

    else if (std::get_if<Pull>(&instruction)) {
        assert(std::get_if<Implicit>(&addressing));
        return 4;
    }

    else if (std::get_if<ReturnFromInterrupt>(&instruction) ||
             std::get_if<ReturnFromSubroutine>(&instruction)) {
        assert(std::get_if<Implicit>(&addressing));
        return 6;
    }

    else if (const auto store = std::get_if<Store>(&instruction)) {
        switch (store->reg) {
            case Emulator::Register::AC: {
                if (std::get_if<ZeroPage>(&addressing)) return 3;
                else if (std::get_if<ZeroPageX>(&addressing)) return 4;
                else if (std::get_if<Absolute>(&addressing)) return 4;
                else if (std::get_if<AbsoluteX>(&addressing)) return 5;
                else if (std::get_if<AbsoluteY>(&addressing)) return 5;
                else if (std::get_if<IndirectX>(&addressing)) return 6;
                else if (std::get_if<IndirectY>(&addressing)) return 6;
                else throw std::invalid_argument("instruction_duration: invalid addressing for STA instruction");
            }
            case Emulator::Register::X: {
                if (std::get_if<ZeroPage>(&addressing)) return 3;
                else if (std::get_if<ZeroPageY>(&addressing)) return 4;
                else if (std::get_if<Absolute>(&addressing)) return 4;
                else throw std::invalid_argument("instruction_duration: invalid addressing for STX instruction");
            }
            case Emulator::Register::Y: {
                if (std::get_if<ZeroPage>(&addressing)) return 3;
                else if (std::get_if<ZeroPageX>(&addressing)) return 4;
                else if (std::get_if<Absolute>(&addressing)) return 4;
                else throw std::invalid_argument("instruction_duration: invalid addressing for STY instruction");
            }
        }
    }

    else if (std::get_if<Transfer>(&instruction)) {
        assert(std::get_if<Implicit>(&addressing));
        return 2;
    }
}


ProcessorStatus instruction_flags(const InstructionArguments &instruction) {
    if (const auto arithmetics = std::get_if<Arithmetics>(&instruction))
        switch (arithmetics->operation) {
            case ArithmeticOperation::ADD: {
                ProcessorStatus flags{};

                const auto [_, overflowUnsigned] = add_and_clip<int>(arithmetics->AC, arithmetics->memory, arithmetics->carry, 0, UINT8_MAX + 1);
                flags[CARRY] = overflowUnsigned;

                const auto [valueSigned, overflowSigned] = add_and_clip<int>((char)arithmetics->AC, (char)arithmetics->memory, arithmetics->carry, CHAR_MIN, CHAR_MAX + 1);
                flags[OVERFLOW] = overflowSigned;

                return flags | set_register_flags(valueSigned);
            }

            case ArithmeticOperation::SUBTRACT: {
                ProcessorStatus flags{};

                const auto [_, overflowUnsigned] = subtract_and_clip<int>(arithmetics->AC, arithmetics->memory, arithmetics->carry, 0, UINT8_MAX + 1);
                flags[CARRY] = !overflowUnsigned;

                const auto [valueSigned, overflowSigned] = subtract_and_clip<int>((char)arithmetics->AC, (char)arithmetics->memory, arithmetics->carry, CHAR_MIN, CHAR_MAX + 1);
                flags[OVERFLOW] = overflowSigned;

                return flags | set_register_flags(valueSigned);
            }
        }

    else if (const auto logical = std::get_if<Logical>(&instruction))
        return set_register_flags(instruction_result(*logical).value());

    else if (const auto shiftLeft = std::get_if<ShiftLeft>(&instruction)) {
        ProcessorStatus flags{};
        flags[CARRY] = get_bit(shiftLeft->value, 7);
        return flags | set_register_flags(shiftLeft->value << 1);
    }

    else if (std::get_if<Branch>(&instruction))
        return {0};

    else if (const auto bitTest = std::get_if<BitTest>(&instruction)) {
        const Byte result = bitTest->AC & bitTest->memory;
        ProcessorStatus flags{};
        flags[ZERO] = result == 0;
        flags[OVERFLOW] = get_bit(result, OVERFLOW);
        flags[NEGATIVE] = get_bit(result, NEGATIVE);
        return flags;
    }

    else if (std::get_if<ForceInterrupt>(&instruction)) {
        ProcessorStatus flags{};
        flags[BREAK] = SET;
        return flags;
    }

    else if (const auto setFlag = std::get_if<SetFlag>(&instruction)) {
        ProcessorStatus flags{};
        flags[setFlag->flag] = setFlag->value;
        return flags;
    }

    else if (const auto compare = std::get_if<Compare>(&instruction)) {
        ProcessorStatus flags{};
        flags[CARRY] = compare->value >= compare->memory;
        flags[ZERO] = compare->value == compare->memory;
        flags[NEGATIVE] = compare->value < compare->memory;
        return flags;
    }

    else if (std::get_if<DecrementMemory>(&instruction) ||
            std::get_if<DecrementRegister>(&instruction) ||
            std::get_if<IncrementMemory>(&instruction) ||
            std::get_if<IncrementRegister>(&instruction))
        return set_register_flags(instruction_result(instruction).value());

    else if (std::get_if<Jump>(&instruction))
        return {0};

    else if (std::get_if<JumpToSubroutine>(&instruction))
        return {0};

    else if (const auto load = std::get_if<Load>(&instruction))
        return set_register_flags(load->value);

    else if (const auto shiftRight = std::get_if<ShiftRight>(&instruction)) {
        ProcessorStatus flags{};
        flags[CARRY] = get_bit(shiftRight->value, 0);
        return flags | set_register_flags(shiftRight->value >> 1);
    }

    else if (std::get_if<NoOperation>(&instruction))
        return {0};

    else if (std::get_if<Push>(&instruction))
        return {0};

    else if (const auto pull = std::get_if<Pull>(&instruction))
        if (pull->reg == Emulator::Register::SR) return {pull->value};
        else return set_register_flags(pull->value);

    else if (const auto rotateLeft = std::get_if<RotateLeft>(&instruction)) {
        ProcessorStatus flags{};
        flags[CARRY] = get_bit(rotateLeft->value, 7);
        return flags | set_register_flags(instruction_result(instruction).value());
    }

    else if (const auto rotateRight = std::get_if<RotateRight>(&instruction)){
        ProcessorStatus flags{};
        flags[CARRY] = get_bit(rotateRight->value, 0);
        return flags | set_register_flags(instruction_result(instruction).value());
    }

    else if (std::get_if<ReturnFromInterrupt>(&instruction))
        return {0};

    else if (std::get_if<ReturnFromSubroutine>(&instruction))
        return {0};

    else if (const auto store = std::get_if<Store>(&instruction))
        return {0};

    else if (const auto transfer = std::get_if<Transfer>(&instruction))
        return set_register_flags(transfer->value);
}


std::optional<Byte> instruction_result(const InstructionArguments &instruction) {
    if (const auto arithmetics = std::get_if<Arithmetics>(&instruction)) {
        switch (arithmetics->operation) {
            case ArithmeticOperation::ADD:
                return arithmetics->AC + arithmetics->memory + arithmetics->carry;
            case ArithmeticOperation::SUBTRACT:
                return arithmetics->AC - arithmetics->memory - !arithmetics->carry;
        }
    }

    else if (const auto logical = std::get_if<Logical>(&instruction)) {
        switch (logical->operation) {
            case LogicalOperation::AND:
                return logical->AC & logical->memory;
            case LogicalOperation::OR:
                return logical->AC | logical->memory;
            case LogicalOperation::XOR:
                return logical->AC ^ logical->memory;
        }
    }

    else if (const auto shiftLeft = std::get_if<ShiftLeft>(&instruction))
        return shiftLeft->value << 1;

    else if (std::get_if<Branch>(&instruction))
        return std::nullopt;

    else if (const auto bitTest = std::get_if<BitTest>(&instruction))
        return std::nullopt;

    else if (std::get_if<ForceInterrupt>(&instruction))
        return std::nullopt;

    else if (const auto setFlag = std::get_if<SetFlag>(&instruction))
        return std::nullopt;

    else if (const auto compare = std::get_if<Compare>(&instruction))
        return std::nullopt;

    else if (const auto decrementMemory = std::get_if<DecrementMemory>(&instruction))
        return decrementMemory->memory - 1;

    else if (const auto decrement = std::get_if<DecrementRegister>(&instruction))
        return decrement->value - 1;

    else if (const auto incrementMemory = std::get_if<IncrementMemory>(&instruction))
        return incrementMemory->memory + 1;

    else if (const auto increment = std::get_if<IncrementRegister>(&instruction))
        return increment->value + 1;

    else if (std::get_if<Jump>(&instruction))
        return std::nullopt;

    else if (std::get_if<JumpToSubroutine>(&instruction))
        return std::nullopt;

    else if (const auto load = std::get_if<Load>(&instruction))
        return load->value;

    else if (const auto shiftRight = std::get_if<ShiftRight>(&instruction))
        return shiftRight->value >> 1;

    else if (std::get_if<NoOperation>(&instruction))
        return std::nullopt;

    else if (const auto pull = std::get_if<Pull>(&instruction))
        return pull->value;

    else if (const auto rotateLeft = std::get_if<RotateLeft>(&instruction)) {
        bool carry = get_bit(rotateLeft->value, 7);
        Byte result = rotateLeft->value << 1;
        set_bit(result, 0, carry);
        return result;
    }

    else if (const auto rotateRight = std::get_if<RotateRight>(&instruction)){
        bool carry = get_bit(rotateRight->value, 0);
        Byte result = rotateRight->value >> 1;
        set_bit(result, 7, carry);
        return result;
    }

    else if (std::get_if<ReturnFromInterrupt>(&instruction))
        return std::nullopt;

    else if (std::get_if<ReturnFromSubroutine>(&instruction))
        return std::nullopt;

    else if (const auto store = std::get_if<Store>(&instruction))
        return std::nullopt;

    else if (const auto transfer = std::get_if<Transfer>(&instruction))
        return transfer->value;
}


ProcessorStatus set_register_flags(Byte value) {
    Emulator::ProcessorStatus flags{};
    flags[ZERO] = value == 0;
    flags[NEGATIVE] = (char)value < 0;
    return flags;
}



