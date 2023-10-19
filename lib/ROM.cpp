//
// Created by Mikhail on 16/10/2023.
//

#include <iostream>
#include <format>
#include "ROM.hpp"

Emulator::Word Emulator::ROM::fetch_word_and_proceed(Word &address, size_t &cycle) const {
    WordToBytes buf;
    buf.low = fetch_byte_and_proceed(address, cycle);
    buf.high = fetch_byte_and_proceed(address, cycle);
    return buf.word;
}

Emulator::Word Emulator::ROM::fetch_word(Emulator::Word address, size_t &cycle) const {
    WordToBytes buf;
    buf.low = fetch_byte(address, cycle);
    buf.high = fetch_byte(address + 1, cycle);
    return buf.word;
}

Emulator::Word Emulator::ROM::get_word(Emulator::Word address) const {
    WordToBytes buf;
    buf.low = m_bytes[address];
    buf.high = m_bytes[address + 1];
    return buf.word;
}

Emulator::Byte &Emulator::ROM::operator[](Emulator::Word address) {
    if (address >= STACK_BOTTOM && address - STACK_BOTTOM <= UINT8_MAX) {
        std::cerr << std::vformat("warning: writing to stack address 0x{:04x}\n", std::make_format_args(address));
//        throw std::runtime_error("");
    }
    return m_bytes[address];
}

Emulator::Byte Emulator::ROM::fetch_byte_and_proceed(Emulator::Word &address, size_t &cycle) const {
    cycle++;
    return m_bytes[address++];
}

Emulator::Byte Emulator::ROM::fetch_byte(Emulator::Word address, size_t &cycle) const {
    cycle++;
    return m_bytes[address];
}

void Emulator::ROM::set_byte_and_proceed(Emulator::ROM::SetByteInputAddressModified input) {
    input.cycle++;
    (*this)[input.address++] = input.value;
}

void Emulator::ROM::set_byte(Emulator::ROM::SetByteInputAddressNotModified input) {
    input.cycle++;
    (*this)[input.address] = input.value;
}
