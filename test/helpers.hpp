//
// Created by Mikhail on 14/09/2023.
//

#ifndef EMULATOR_MOS6502_HELPERS_HPP
#define EMULATOR_MOS6502_HELPERS_HPP

//#include <optional>

#include <vector>
#include "ProcessorStatus.hpp"
//#include "Result.hpp"

using namespace Emulator;

ProcessorStatus status_from_flags(const std::vector<Flag>& flags);

std::ostream &operator <<(std::ostream &os, Flag flag);

//ProcessorStatus set_register_flags_for(Byte value);
//
//bool page_crossed(Word address, int offset);
//
//std::pair<Byte, ProcessorStatus> add_with_carry(Byte value1, Byte value2, bool carry);
//
//std::pair<Byte, ProcessorStatus> subtract_with_carry(Byte value1, Byte value2, bool carry);
//
//std::string to_string(AddressingModeTest mode);
//
//std::string to_string(Instruction instruction);
//
//Result<OpCode> opcode(Instruction instruction, std::optional<AddressingModeTest> addressingMode = std::nullopt);
//
//template <typename T>
//std::string to_string(std::optional<T> opt) {
//    if (opt.has_value()) return to_string(opt.value());
//    else return "nullopt";
//}

#endif //EMULATOR_MOS6502_HELPERS_HPP
