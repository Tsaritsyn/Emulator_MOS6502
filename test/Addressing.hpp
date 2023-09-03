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
struct Immediate { Byte value; };
struct ZeroPage { Byte address, value; };
struct ZeroPageX { Byte address, X, value; };
struct ZeroPageY { Byte address, Y, value; };
struct Relative { char offset; };
struct Absolute { Word address; Byte value; };
struct AbsoluteX { Word address; Byte X, value; };
struct AbsoluteY { Word address; Byte Y, value; };
struct Indirect { Word address, newAddress; };
struct IndirectX { Byte tableAddress; Word targetAddress; Byte X, value; };
struct IndirectY { Byte tableAddress; Word targetAddress; Byte Y, value; };
using Addressing = std::variant<Implicit, Accumulator, Immediate, ZeroPage, ZeroPageX, ZeroPageY, Relative, Absolute, AbsoluteX, AbsoluteY, Indirect, IndirectX, IndirectY>;


constexpr Word size(const Addressing &addressing) {
    if (std::get_if<Implicit>(&addressing) ||
        std::get_if<Accumulator>(&addressing)) return 1;
    else if (std::get_if<Immediate>(&addressing) ||
             std::get_if<ZeroPage>(&addressing) ||
             std::get_if<ZeroPageX>(&addressing) ||
             std::get_if<ZeroPageY>(&addressing) ||
             std::get_if<Relative>(&addressing) ||
             std::get_if<IndirectX>(&addressing) ||
             std::get_if<IndirectY>(&addressing)) return 2;
    else if (std::get_if<Absolute>(&addressing) ||
             std::get_if<AbsoluteX>(&addressing) ||
             std::get_if<AbsoluteY>(&addressing) ||
             std::get_if<Indirect>(&addressing)) return 3;
    else throw std::runtime_error("MOS6502_TestFixture::size: unhandled addressing");
}

std::ostream& operator<<(std::ostream &os, Addressing addressing) {
    if (const auto args = std::get_if<Implicit>(&addressing))
        return os << "Implicit";
    else if (const auto args = std::get_if<Accumulator>(&addressing))
        return os << "Accumulator";
    else if (const auto args = std::get_if<Immediate>(&addressing))
        return os << "Immediate(value: " << HEX_BYTE(args->value) << ")";
    else if (const auto args = std::get_if<ZeroPage>(&addressing))
        return os << "Zero Page(address: " << HEX_BYTE(args->address) << ", value: " << HEX_BYTE(args->value) << ")";
    else if (const auto args = std::get_if<ZeroPageX>(&addressing))
        return os << "Zero Page,X(address: " << args->address << ", X: " << args->X << ", value: " << args->value << ")";
    else if (const auto args = std::get_if<ZeroPageY>(&addressing))
        return os << "Zero Page,Y(address: " << args->address << ", Y: " << args->Y << ", value: " << args->value << ")";
    else if (const auto args = std::get_if<Relative>(&addressing))
        return os << "Relative(offset: " << (int)args->offset << ")";
    else if (const auto args = std::get_if<Absolute>(&addressing))
        return os << "Absolute(address: " << args->address << ", value: " << args->value << ")";
    else if (const auto args = std::get_if<AbsoluteX>(&addressing))
        return os << "Absolute,X(address: " << args->address << ", X: " << args->X << ", value: " << args->value << ")";
    else if (const auto args = std::get_if<AbsoluteY>(&addressing))
        return os << "Absolute,Y(address: " << args->address << ", Y: " << args->Y << ", value: " << args->value << ")";
    else if (const auto args = std::get_if<Indirect>(&addressing))
        return os << "Indirect(address: " << args->address << ", new address: " << args->newAddress << ")";
    else if (const auto args = std::get_if<IndirectX>(&addressing))
        return os << "Indexed Indirect(table address: " << args->tableAddress << ", target address: " << args->targetAddress << ", X: " << args->X << ", value: " << args->value << ")";
    else if (const auto args = std::get_if<IndirectY>(&addressing))
        return os << "Indirect Indexed(table address: " << args->tableAddress << ", target address: " << args->targetAddress << ", Y: " << args->Y << ", value: " << args->value << ")";
    else throw std::runtime_error("operator<<: unhandled addressing");
}

#endif //EMULATOR_MOS6502_ADDRESSING_HPP
