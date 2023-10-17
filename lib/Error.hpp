//
// Created by Mikhail on 16/10/2023.
//

#ifndef EMULATOR_MOS6502_ERROR_HPP
#define EMULATOR_MOS6502_ERROR_HPP

#include <string>
#include <utility>

#include "MOS6502_definitions.hpp"

namespace Emulator {

    struct Error {
        std::string message;

        Error(std::string message = ""): message(std::move(message)) {}
    };

    struct InvalidOperation: Error {
        Byte opCode;

        InvalidOperation(Byte opCode): opCode(opCode), Error(std::vformat("illegal opcode {:02x}", std::make_format_args(opCode))) {}
    };
}

#endif //EMULATOR_MOS6502_ERROR_HPP
