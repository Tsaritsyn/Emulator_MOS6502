//
// Created by Mikhail on 14/09/2023.
//

#ifndef EMULATOR_MOS6502_HELPERS_HPP
#define EMULATOR_MOS6502_HELPERS_HPP

#include "MOS6502_definitions.hpp"

using namespace Emulator;

ProcessorStatus set_register_flags_for(Byte value);

bool page_crossed(Word address, int offset);

#endif //EMULATOR_MOS6502_HELPERS_HPP
