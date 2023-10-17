//
// Created by Mikhail on 16/10/2023.
//

#ifndef EMULATOR_MOS6502_ROM_HPP
#define EMULATOR_MOS6502_ROM_HPP

#include "MOS6502_definitions.hpp"

namespace Emulator {

    class ROM {

    public:
        ROM(): m_bytes{} {};

        Byte& operator [](Word address) { return m_bytes[address]; }

        Byte operator [](Word address) const { return m_bytes[address]; }

        Byte fetch_byte(Word &address, size_t &cycle) const { cycle++; return m_bytes[address++]; }

        Word fetch_word(Word &address, size_t &cycle) const;

        struct SetByteInput { Word &address; Byte value; size_t &cycle; };
        void set_byte(SetByteInput input) { input.cycle++; m_bytes[input.address++] = input.value; }

        struct SetWordInput { Word &address; Word value; size_t cycle; };
        void set_word(SetWordInput input);

    private:
        std::array<Byte, UINT16_MAX> m_bytes;
    };

}

#endif //EMULATOR_MOS6502_ROM_HPP
