//
// Created by Mikhail on 14/09/2023.
//

#include <iostream>
#include "helpers.hpp"

ProcessorStatus set_register_flags_for(Byte value) {
    ProcessorStatus flags{};
    flags[ZERO] = value == 0;
    flags[NEGATIVE] = (char)value < 0;
    return flags;
}

bool page_crossed(Word address, int offset) {
    const WordToBytes buf(address);
    const WordToBytes bufNew((Word)(address + offset));
    return buf.high != bufNew.high;
}

static std::pair<Byte, bool> add_within_range(int value1, int value2, bool carry, int rmin, int rmax) {
    int result = value1 + value2 + carry;
    return {result, result > rmax || result < rmin};
}

std::pair<Byte, ProcessorStatus> add_with_carry(Byte value1, Byte value2, bool carry) {
    ProcessorStatus flags{};

    const auto &[unsignedResult, unsignedOverflow] = add_within_range(value1, value2, carry, 0, UINT8_MAX);
    flags[CARRY] = unsignedOverflow;

    const auto &[_, signedOverflow] = add_within_range((char)value1, (char)value2, carry, INT8_MIN, INT8_MAX);
    flags[OVERFLOW_F] = signedOverflow;

    return {unsignedResult, flags | set_register_flags_for(unsignedResult)};
}

static std::pair<Byte, bool> subtract_within_range(int value1, int value2, bool carry, int rmin, int rmax) {
    int result = value1 - value2 - !carry;
    return {result, result > rmax || result < rmin};
}

std::pair<Byte, ProcessorStatus> subtract_with_carry(Byte value1, Byte value2, bool carry) {
    ProcessorStatus flags{};

    const auto &[unsignedResult, unsignedOverflow] = subtract_within_range(value1, value2, carry, 0, UINT8_MAX);
    flags[CARRY] = !unsignedOverflow;

    const auto &[_, signedOverflow] = subtract_within_range((char)value1, (char)value2, carry, INT8_MIN, INT8_MAX);
    flags[OVERFLOW_F] = signedOverflow;

    return {unsignedResult, flags | set_register_flags_for(unsignedResult)};
}
