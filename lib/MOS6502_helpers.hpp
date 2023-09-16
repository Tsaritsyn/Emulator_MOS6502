//
// Created by Mikhail on 30/08/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_HELPERS_HPP
#define EMULATOR_MOS6502_MOS6502_HELPERS_HPP

#include <vector>
#include <ostream>
#include <iostream>
#include <optional>

#include "MOS6502_definitions.hpp"

namespace Emulator {

    void set_bit(Byte &byte, int number, bool value = true);

    bool get_bit(Byte byte, int number);

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
}

#endif //EMULATOR_MOS6502_MOS6502_HELPERS_HPP
