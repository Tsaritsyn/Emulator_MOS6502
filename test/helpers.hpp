//
// Created by Mikhail on 14/09/2023.
//

#ifndef EMULATOR_MOS6502_HELPERS_HPP
#define EMULATOR_MOS6502_HELPERS_HPP

#include "MOS6502_definitions.hpp"
#include "ProcessorStatus.hpp"

using namespace Emulator;

ProcessorStatus set_register_flags_for(Byte value);

bool page_crossed(Word address, int offset);

std::pair<Byte, ProcessorStatus> add_with_carry(Byte value1, Byte value2, bool carry);

std::pair<Byte, ProcessorStatus> subtract_with_carry(Byte value1, Byte value2, bool carry);

#endif //EMULATOR_MOS6502_HELPERS_HPP
