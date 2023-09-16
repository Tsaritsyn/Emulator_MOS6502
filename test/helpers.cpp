//
// Created by Mikhail on 14/09/2023.
//

#include "helpers.hpp"

ProcessorStatus set_register_flags_for(Byte value) {
    ProcessorStatus flags{};
    flags[ZERO] = value == 0;
    flags[NEGATIVE] = (char)value < 0;
    return flags;
}

bool page_crossed(Word address, int offset) {
    const WordToBytes buf(address);
    int result = buf.low + offset;
    return result < 0 || result > UINT8_MAX;
}
