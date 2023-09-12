//
// Created by Mikhail on 04/09/2023.
//

#ifndef EMULATOR_MOS6502_ADDRESSING_HPP
#define EMULATOR_MOS6502_ADDRESSING_HPP

#include <variant>

#include "MOS6502_definitions.hpp"

using namespace Emulator;

// Addressing mode cases
struct Implicit {};
struct Accumulator {};
struct Immediate {};
struct ZeroPage { Byte address; };
struct ZeroPageX { Byte address, X; };
struct ZeroPageY { Byte address, Y; };
struct Relative { Word PC; char offset; };
struct Absolute { Word address; };
struct AbsoluteX { Word address; Byte X; };
struct AbsoluteY { Word address; Byte Y; };
struct Indirect { Word address, newAddress; };
struct IndirectX { Byte tableAddress; Word targetAddress; Byte X; };
struct IndirectY { Byte tableAddress; Word targetAddress; Byte Y; };
using Addressing = std::variant<Implicit, Accumulator, Immediate, ZeroPage, ZeroPageX, ZeroPageY, Relative, Absolute, AbsoluteX, AbsoluteY, Indirect, IndirectX, IndirectY>;

Word size(const Addressing &addressing);

AddressingMode mode(const Addressing &addressing);

std::ostream& operator<<(std::ostream &os, Addressing addressing);

#endif //EMULATOR_MOS6502_ADDRESSING_HPP
