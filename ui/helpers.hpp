//
// Created by Mikhail on 23/10/2023.
//

#ifndef EMULATOR_MOS6502_HELPERS_HPP
#define EMULATOR_MOS6502_HELPERS_HPP

#include <string>

#include "MOS6502_definitions.hpp"

#include <QColor>

using namespace Emulator;

std::string byte_description(Byte byte);

std::optional<size_t> max_duration_of(Byte opcode);

struct Style { QColor foreground, background; };
Style color_by(size_t duration);

#endif //EMULATOR_MOS6502_HELPERS_HPP
