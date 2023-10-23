//
// Created by Mikhail on 14/09/2023.
//

#ifndef EMULATOR_MOS6502_ADDRESSING_HPP
#define EMULATOR_MOS6502_ADDRESSING_HPP

#include <optional>
#include <ostream>

#include "MOS6502_definitions.hpp"
#include "helpers.hpp"

using namespace Emulator;


struct Addressing {
    using EmptyT = struct {};
    using SimpleT = struct { Word address; };
    using IndexedT = struct { Word address; Byte index; };
    using RelativeT = struct { Word PC; char offset; };
    using IndirectT = struct { Word tableAddress, targetAddress; };
    using IndirectIndexedT = struct { Byte tableAddress; Word targetAddress; Byte index; };


    static Addressing Implicit() noexcept;
    static Addressing Accumulator() noexcept;
    static Addressing Immediate() noexcept;
    static Addressing ZeroPage(Byte address) noexcept;
    static Addressing ZeroPageX(Byte address, Byte X) noexcept;
    static Addressing ZeroPageY(Byte Address, Byte Y) noexcept;
    static Addressing Relative(Word PC, char offset) noexcept;
    static Addressing Absolute(Word address) noexcept;
    static Addressing AbsoluteX(Word address, Byte X) noexcept;
    static Addressing AbsoluteY(Word address, Byte Y) noexcept;
    static Addressing Indirect(Word tableAddress, Word targetAddress) noexcept;
    static Addressing IndirectX(Byte tableAddress, Word targetAddress, Byte X) noexcept;
    static Addressing IndirectY(Byte tableAddress, Word targetAddress, Byte Y) noexcept;


    [[nodiscard]] AddressingModeTest getMode() const { return mode; }

    [[nodiscard]] std::optional<EmptyT> getImplicit() const noexcept;
    [[nodiscard]] std::optional<EmptyT> getAccumulator() const noexcept;
    [[nodiscard]] std::optional<EmptyT> getImmediate() const noexcept;
    [[nodiscard]] std::optional<SimpleT> getZeroPage() const noexcept;
    [[nodiscard]] std::optional<IndexedT> getZeroPageX() const noexcept;
    [[nodiscard]] std::optional<IndexedT> getZeroPageY() const noexcept;
    [[nodiscard]] std::optional<RelativeT> getRelative() const noexcept;
    [[nodiscard]] std::optional<SimpleT> getAbsolute() const noexcept;
    [[nodiscard]] std::optional<IndexedT> getAbsoluteX() const noexcept;
    [[nodiscard]] std::optional<IndexedT> getAbsoluteY() const noexcept;
    [[nodiscard]] std::optional<IndirectT> getIndirect() const noexcept;
    [[nodiscard]] std::optional<IndirectIndexedT> getIndirectX() const noexcept;
    [[nodiscard]] std::optional<IndirectIndexedT> getIndirectY() const noexcept;


    [[nodiscard]] int PC_shift() const noexcept;
    [[nodiscard]] bool page_crossed() const noexcept;

    [[nodiscard]] std::string to_string() const;


//    friend std::ostream &operator<<(std::ostream &os, const Addressing &addressing);


private:
    Addressing(AddressingModeTest mode): mode{mode} { args.none = {}; };

    AddressingModeTest mode;

private:

    union {
        EmptyT none;
        SimpleT simple;
        IndexedT indexed;
        RelativeT relative;
        IndirectT indirect;
        IndirectIndexedT indirectIndexed;
    } args{};

};


#endif //EMULATOR_MOS6502_ADDRESSING_HPP
