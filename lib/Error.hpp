//
// Created by Mikhail on 16/10/2023.
//

#ifndef EMULATOR_MOS6502_ERROR_HPP
#define EMULATOR_MOS6502_ERROR_HPP

#include <string>
#include <utility>
#include <format>

#include "MOS6502_definitions.hpp"

namespace Emulator {
    struct InvalidOperation {
        Byte opCode;

        [[nodiscard]] std::string to_string() const noexcept {
            return std::vformat("Given opcode {:#02x} does not correspond to any known operation", std::make_format_args(opCode));
        }
    };
}

#endif //EMULATOR_MOS6502_ERROR_HPP
