//
// Created by Mikhail on 16/10/2023.
//

#include "ROM.hpp"

Emulator::Word Emulator::ROM::fetch_word_and_proceed(Word &address, size_t &cycle) const {
    WordToBytes buf;
    buf.low = fetch_byte_and_proceed(address, cycle);
    buf.high = fetch_byte_and_proceed(address, cycle);
    return buf.word;
}

void Emulator::ROM::set_word_and_proceed(Emulator::ROM::SetWordInputAddressModified input) {
    WordToBytes buf(input.value);
    set_byte_and_proceed({.address = input.address, .value = buf.low, .cycle = input.cycle});
    set_byte_and_proceed({.address = input.address, .value = buf.high, .cycle = input.cycle});
}

Emulator::Word Emulator::ROM::fetch_word(Emulator::Word address, size_t &cycle) const {
    WordToBytes buf;
    buf.low = fetch_byte(address, cycle);
    buf.high = fetch_byte(address + 1, cycle);
    return buf.word;
}

void Emulator::ROM::set_word(Emulator::ROM::SetWordInputAddressNotModified input) {
    WordToBytes buf(input.value);
    set_byte({.address = input.address, .value = buf.low, .cycle = input.cycle});
    set_byte({.address = (Word)(input.address + 1), .value = buf.high, .cycle = input.cycle});
}
