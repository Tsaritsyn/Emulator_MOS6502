//
// Created by Mikhail on 16/10/2023.
//

#include "ROM.hpp"

Emulator::Word Emulator::ROM::fetch_word(Emulator::Word &address, size_t &cycle) const {
    cycle++;
    WordToBytes buf;
    buf.low = m_bytes[address++];
    buf.high = m_bytes[address++];
    return buf.word;
}

void Emulator::ROM::set_word(Emulator::ROM::SetWordInput input) {
    input.cycle++;
    WordToBytes buf(input.value);
    m_bytes[input.address++] = buf.low;
    m_bytes[input.address++] = buf.high;
}
