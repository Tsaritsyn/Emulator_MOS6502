//
// Created by Mikhail on 14/09/2023.
//

#include <utility>
#include <format>
#include "Addressing.hpp"

using namespace Emulator;

Addressing Addressing::Implicit() noexcept {
    return {Emulator::AddressingMode::IMPLICIT};
}

Addressing Addressing::Accumulator() noexcept {
    return {Emulator::AddressingMode::ACCUMULATOR};
}

Addressing Addressing::Immediate() noexcept {
    return {Emulator::AddressingMode::IMMEDIATE};
}

Addressing Addressing::ZeroPage(Byte address) noexcept {
    auto result = Addressing(Emulator::AddressingMode::ZERO_PAGE);
    result.args.simple = {address};
    return result;
}

Addressing Addressing::ZeroPageX(Byte address, Byte X) noexcept {
    auto result = Addressing(Emulator::AddressingMode::ZERO_PAGE_X);
    result.args.indexed = {address, X};
    return result;
}

Addressing Addressing::ZeroPageY(Byte address, Byte Y) noexcept {
    auto result = Addressing(Emulator::AddressingMode::ZERO_PAGE_Y);
    result.args.indexed = {address, Y};
    return result;
}

Addressing Addressing::Relative(Word PC, char offset) noexcept {
    auto result = Addressing(Emulator::AddressingMode::RELATIVE);
    result.args.relative = {PC, offset};
    return result;
}

Addressing Addressing::Absolute(Word address) noexcept {
    auto result = Addressing(Emulator::AddressingMode::ABSOLUTE);
    result.args.simple = {address};
    return result;
}

Addressing Addressing::AbsoluteX(Word address, Byte X) noexcept {
    auto result = Addressing(Emulator::AddressingMode::ABSOLUTE_X);
    result.args.indexed = {address, X};
    return result;
}

Addressing Addressing::AbsoluteY(Word address, Byte Y) noexcept {
    auto result = Addressing(Emulator::AddressingMode::ABSOLUTE_Y);
    result.args.indexed = {address, Y};
    return result;
}

Addressing Addressing::Indirect(Word tableAddress, Word targetAddress) noexcept {
    auto result = Addressing(Emulator::AddressingMode::INDIRECT);
    result.args.indirect = {tableAddress, targetAddress};
    return result;
}

Addressing Addressing::IndirectX(Byte tableAddress, Word targetAddress, Byte X) noexcept {
    auto result = Addressing(Emulator::AddressingMode::INDIRECT_X);
    result.args.indirectIndexed = {tableAddress, targetAddress, X};
    return result;
}

Addressing Addressing::IndirectY(Byte tableAddress, Word targetAddress, Byte Y) noexcept {
    auto result = Addressing(Emulator::AddressingMode::INDIRECT_Y);
    result.args.indirectIndexed = {tableAddress, targetAddress, Y};
    return result;
}

std::optional<Addressing::EmptyT> Addressing::getImplicit() const noexcept {
    if (mode == Emulator::AddressingMode::IMPLICIT) return args.none;
    else return std::nullopt;
}

std::optional<Addressing::EmptyT> Addressing::getAccumulator() const noexcept {
    if (mode == Emulator::AddressingMode::ACCUMULATOR) return args.none;
    else return std::nullopt;
}

std::optional<Addressing::EmptyT> Addressing::getImmediate() const noexcept {
    if (mode == Emulator::AddressingMode::IMMEDIATE) return args.none;
    else return std::nullopt;
}

std::optional<Addressing::SimpleT> Addressing::getZeroPage() const noexcept {
    if (mode == Emulator::AddressingMode::ZERO_PAGE) return args.simple;
    else return std::nullopt;
}

std::optional<Addressing::IndexedT> Addressing::getZeroPageX() const noexcept {
    if (mode == Emulator::AddressingMode::ZERO_PAGE_X) return args.indexed;
    else return std::nullopt;
}

std::optional<Addressing::IndexedT> Addressing::getZeroPageY() const noexcept {
    if (mode == Emulator::AddressingMode::ZERO_PAGE_Y) return args.indexed;
    else return std::nullopt;
}

std::optional<Addressing::RelativeT> Addressing::getRelative() const noexcept {
    if (mode == Emulator::AddressingMode::RELATIVE) return args.relative;
    else return std::nullopt;
}

std::optional<Addressing::SimpleT> Addressing::getAbsolute() const noexcept {
    if (mode == Emulator::AddressingMode::ABSOLUTE) return args.simple;
    else return std::nullopt;
}

std::optional<Addressing::IndexedT> Addressing::getAbsoluteX() const noexcept {
    if (mode == Emulator::AddressingMode::ABSOLUTE_X) return args.indexed;
    else return std::nullopt;
}

std::optional<Addressing::IndexedT> Addressing::getAbsoluteY() const noexcept {
    if (mode == Emulator::AddressingMode::ABSOLUTE_Y) return args.indexed;
    else return std::nullopt;
}

std::optional<Addressing::IndirectT> Addressing::getIndirect() const noexcept {
    if (mode == Emulator::AddressingMode::INDIRECT) return args.indirect;
    else return std::nullopt;
}

std::optional<Addressing::IndirectIndexedT> Addressing::getIndirectX() const noexcept {
    if (mode == Emulator::AddressingMode::INDIRECT_X) return args.indirectIndexed;
    else return std::nullopt;
}

std::optional<Addressing::IndirectIndexedT> Addressing::getIndirectY() const noexcept {
    if (mode == Emulator::AddressingMode::INDIRECT_Y) return args.indirectIndexed;
    else return std::nullopt;
}

std::string Addressing::to_string() const {
    switch (mode) {
        case AddressingMode::IMPLICIT:
            return "Implicit";
        case AddressingMode::ACCUMULATOR:
            return "Accumulator";
        case AddressingMode::IMMEDIATE:
            return "Immediate";
        case AddressingMode::ZERO_PAGE:
            return std::vformat("Zero Page(address: {:#04x})", std::make_format_args(args.simple.address));
        case AddressingMode::ZERO_PAGE_X:
            return std::vformat("Zero Page,X(address: {:#04x}, X: {:#02x})", std::make_format_args(args.indexed.address, args.indexed.index));
        case AddressingMode::ZERO_PAGE_Y:
            return std::vformat("Zero Page,Y(address: {:#04x}, Y: {:#02x})", std::make_format_args(args.indexed.address, args.indexed.index));
        case AddressingMode::RELATIVE:
            return std::vformat("Relative(PC: {:#04x}, offset: {:d})", std::make_format_args(args.relative.PC, args.relative.offset));
        case AddressingMode::ABSOLUTE:
            return std::vformat("Absolute(address: {:#04x})", std::make_format_args(args.simple.address));
        case AddressingMode::ABSOLUTE_X:
            return std::vformat("Absolute,X(address: {:#04x}, X: {:#02x})", std::make_format_args(args.indexed.address, args.indexed.index));
        case AddressingMode::ABSOLUTE_Y:
            return std::vformat("Absolute,Y(address: {:#04x}, X: {:#02x})", std::make_format_args(args.indexed.address, args.indexed.index));
        case AddressingMode::INDIRECT:
            return std::vformat("Indirect(table address: {:#04x}, target address: {:#04x})", std::make_format_args(args.indirect.tableAddress, args.indirect.targetAddress));
        case AddressingMode::INDIRECT_X:
            return std::vformat("Indirect,X(table address: {:#04x}, target address: {:#04x}, X: {:#02x})", std::make_format_args(args.indirectIndexed.tableAddress, args.indirectIndexed.targetAddress, args.indirectIndexed.index));
        case AddressingMode::INDIRECT_Y:
            return std::vformat("Indirect,Y(table address: {:#04x}, target address: {:#04x}, Y: {:#02x})", std::make_format_args(args.indirectIndexed.tableAddress, args.indirectIndexed.targetAddress, args.indirectIndexed.index));
    }

    std::unreachable();
}

int Addressing::PC_shift() const noexcept {
    switch (mode) {
        case AddressingMode::IMPLICIT: [[fallthrough]];
        case AddressingMode::ACCUMULATOR:
            return 1;
        case AddressingMode::IMMEDIATE: [[fallthrough]];
        case AddressingMode::ZERO_PAGE: [[fallthrough]];
        case AddressingMode::ZERO_PAGE_X: [[fallthrough]];
        case AddressingMode::ZERO_PAGE_Y: [[fallthrough]];
        case AddressingMode::RELATIVE:
            return 2;
        case AddressingMode::ABSOLUTE: [[fallthrough]];
        case AddressingMode::ABSOLUTE_X: [[fallthrough]];
        case AddressingMode::ABSOLUTE_Y: [[fallthrough]];
        case AddressingMode::INDIRECT:
            return 3;
        case AddressingMode::INDIRECT_X: [[fallthrough]];
        case AddressingMode::INDIRECT_Y:
            return 2;
    }

    std::unreachable();
}

bool Addressing::page_crossed() const noexcept {
    switch (mode) {
        case AddressingMode::IMPLICIT: [[fallthrough]];
        case AddressingMode::ACCUMULATOR: [[fallthrough]];
        case AddressingMode::IMMEDIATE: [[fallthrough]];
        case AddressingMode::ZERO_PAGE:
            return false;
        case AddressingMode::ZERO_PAGE_X: [[fallthrough]];
        case AddressingMode::ZERO_PAGE_Y:
            return ::page_crossed(args.indexed.address, args.indexed.index);
        case AddressingMode::RELATIVE:
            return ::page_crossed(args.relative.PC, args.relative.offset);
        case AddressingMode::ABSOLUTE:
            return false;
        case AddressingMode::ABSOLUTE_X: [[fallthrough]];
        case AddressingMode::ABSOLUTE_Y:
            return ::page_crossed(args.indexed.address, args.indexed.index);
        case AddressingMode::INDIRECT:
            return false;
        case AddressingMode::INDIRECT_X:
            return ::page_crossed(args.indirectIndexed.tableAddress, args.indirectIndexed.index);
        case AddressingMode::INDIRECT_Y:
            return ::page_crossed(args.indirectIndexed.targetAddress, args.indirectIndexed.index);
    }

    std::unreachable();
}
