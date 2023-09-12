//
// Created by Mikhail on 30/08/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_HELPERS_HPP
#define EMULATOR_MOS6502_MOS6502_HELPERS_HPP

#include <vector>
#include <ostream>
#include <iostream>

#include "MOS6502_definitions.hpp"

namespace Emulator {

    void set_bit(Byte &byte, int number, bool value = true);

    bool get_bit(Byte byte, int number);

    Byte flag_combination(const std::vector<Flag> &flags);

    std::ostream &operator<<(std::ostream &os, AddressingMode mode);

    std::ostream &operator<<(std::ostream &os, Instruction instruction);

    std::ostream &operator<<(std::ostream &os, Register aRegister);

    std::ostream &operator<<(std::ostream &os, Flag flag);

    template <class T>
    std::ostream& operator <<(std::ostream &os, const std::vector<T> &vec) {
        os << '{';
        for (const auto &element: vec) os << element << ", ";
        os << '}';
        return os;
    }

    template <class T>
    std::pair<T, bool> add_and_clip(
            T a,
            T b,
            bool carry,
            T min,
            T max
            ) {
        T range = max - min;
        T result = a + b + carry;

        bool overflow = result >= max || result < min;
        while (result >= max) result -= range;
        while (result < min) result += range;

        return {result, overflow};
    }

    template <class T>
    std::pair<T, bool> subtract_and_clip(
            T a,
            T b,
            bool carry,
            T min,
            T max
    ) {
        T range = max - min;
        T result = a - b - !carry;

        bool overflow = result >= max || result < min;
        while (result >= max) result -= range;
        while (result < min) result += range;

        return {result, overflow};
    }

    OpCode opcode(Instruction instruction, AddressingMode mode);
}

#endif //EMULATOR_MOS6502_MOS6502_HELPERS_HPP
