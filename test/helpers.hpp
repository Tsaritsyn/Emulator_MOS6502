//
// Created by Mikhail on 14/09/2023.
//

#ifndef EMULATOR_MOS6502_HELPERS_HPP
#define EMULATOR_MOS6502_HELPERS_HPP

#include <vector>
#include "ProcessorStatus.hpp"

using namespace Emulator;

ProcessorStatus status_from_flags(const std::vector<Flag>& flags);

std::ostream &operator <<(std::ostream &os, Flag flag);

#endif //EMULATOR_MOS6502_HELPERS_HPP
