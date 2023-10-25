//
// Created by Mikhail on 16/10/2023.
//

#include <iostream>
#include <format>
#include "ROM.hpp"


Emulator::Word Emulator::ROM::get_word(Emulator::Word address) const {
    WordToBytes buf;
    buf.low() = m_bytes[address];
    buf.high() = m_bytes[address + 1];
    return buf.word;
}

Emulator::Byte Emulator::ROM::fetch_byte(Emulator::Word address, size_t &cycle) const {
    cycle++;
    return m_bytes[address];
}

std::expected<void, Emulator::ROM::StackOverride> Emulator::ROM::set_byte(Word address, Byte value, size_t &cycle) {
    cycle++;
    return set_byte(address, value);
}

std::expected<void, Emulator::ROM::StackOverride> Emulator::ROM::set_byte(Emulator::Word address, Emulator::Byte value) {
    std::cout << "Address in stack: " << is_in_stack(address) << '\n';
    if (is_in_stack(address)) {
        std::cout << "Address in stack\n";
        return std::unexpected<StackOverride>{address};
    }
    m_bytes[address] = value;
    return {};
}
