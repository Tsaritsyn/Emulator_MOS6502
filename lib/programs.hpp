//
// Created by Mikhail on 04/10/2023.
//

#ifndef EMULATOR_MOS6502_PROGRAMS_HPP
#define EMULATOR_MOS6502_PROGRAMS_HPP

#include "MOS6502_definitions.hpp"

using namespace Emulator;

/**
 * Implements multiplication of two numbers a and b such that 0 <= a, b < 256.
 * The first number must be stored in accumulator, the second one in X register.
 * The result of multiplication consists of 2 bytes: high is written into Y, low into accumulator.
 *
 * @param startAddress address where the program will be written (address of its first operation); needed for jumping
 * @return the sequence of bytes executable by a MOS6502 CPU
 */
std::array<Byte, 29> program_multiplication(Word startAddress);

#endif //EMULATOR_MOS6502_PROGRAMS_HPP
