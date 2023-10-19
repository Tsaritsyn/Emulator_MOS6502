//
// Created by Mikhail on 30/08/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_HELPERS_HPP
#define EMULATOR_MOS6502_MOS6502_HELPERS_HPP

#include <vector>
#include <ostream>
#include <iostream>
#include <optional>
#include <utility>
#include <expected>

#include "MOS6502_definitions.hpp"
#include "Result.hpp"
#include "ProcessorStatus.hpp"

namespace Emulator {

    void set_bit(Byte &byte, int number, bool value = true);

    bool get_bit(Byte byte, int number);

    std::string to_string(AddressingMode mode);

    std::string to_string(Instruction instruction);

    std::string to_string(Register reg);

    template <class T>
    std::ostream& operator <<(std::ostream &os, const std::vector<T> &vec) {
        os << '{';
        for (const auto &element: vec) os << element << ", ";
        os << '}';
        return os;
    }

    Result<OpCode> opcode(Instruction instruction, std::optional<AddressingMode> addressingMode = std::nullopt);

    template <typename T>
    std::string to_string(std::optional<T> opt) {
        if (opt.has_value()) return to_string(opt.value());
        else return "nullopt";
    }

    std::string byte_description(Byte byte);

    int add_with_overflow(int a, int b, bool &overflow, int rmin, int rmax);

    int subtract_with_overflow(int a, int b, bool &overflow, int rmin, int rmax);
}


#endif //EMULATOR_MOS6502_MOS6502_HELPERS_HPP
