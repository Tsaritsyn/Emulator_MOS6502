//
// Created by Mikhail on 07/09/2023.
//

#ifndef EMULATOR_MOS6502_INSTRUCTION_HPP
#define EMULATOR_MOS6502_INSTRUCTION_HPP

#include <variant>
#include <optional>
#include <iostream>


#include "MOS6502_helpers.hpp"
#include "Addressing.hpp"

using namespace Emulator;

enum class ArithmeticOperation {
    ADD,
    SUBTRACT
};

struct Arithmetics { ArithmeticOperation operation; Byte AC, memory; bool carry; };
struct Logical { LogicalOperation operation; Byte AC, memory; };
struct ShiftLeft { Byte value; };
struct Branch { Flag flag; bool targetValue, givenValue; };
struct BitTest { Byte AC, memory; };
struct ForceInterrupt {};
struct SetFlag { Flag flag; bool value; };
struct Compare { Register reg; Byte value, memory; };
struct DecrementMemory { Byte memory; };
struct DecrementRegister { Register reg; Byte value; };
struct IncrementMemory { Byte memory; };
struct IncrementRegister { Register reg; Byte value; };
struct Jump {};
struct JumpToSubroutine {};
struct Load { Register reg; Byte value; };
struct ShiftRight { Byte value; };
struct NoOperation {};
struct Push { Register reg; Byte value; };
struct Pull { Register reg; Byte value; };
struct RotateLeft { Byte value; };
struct RotateRight { Byte value; };
struct ReturnFromInterrupt {};
struct ReturnFromSubroutine {};
struct Store { Register reg; Byte value; };
struct Transfer { Register from, to; Byte value; };
using InstructionArguments = std::variant<Arithmetics, Logical, ShiftLeft, Branch, BitTest, ForceInterrupt, SetFlag, Compare,
DecrementMemory, DecrementRegister, IncrementMemory, IncrementRegister, Jump, JumpToSubroutine, Load, ShiftRight,
NoOperation, Push, Pull, RotateLeft, RotateRight, ReturnFromInterrupt, ReturnFromSubroutine, Store, Transfer>;

std::ostream& operator <<(std::ostream &os, const InstructionArguments &instruction);

std::optional<Byte> instruction_result(const InstructionArguments &instruction);

ProcessorStatus set_register_flags(Byte value);

ProcessorStatus instruction_flags(const InstructionArguments &instruction);

Instruction instruction_code(const InstructionArguments &args);

bool page_crossed(const Addressing &addressing);

static size_t arithmetic_duration(const Addressing &addressing);

size_t instruction_duration(const InstructionArguments &instruction, const Addressing &addressing);



#endif //EMULATOR_MOS6502_INSTRUCTION_HPP
