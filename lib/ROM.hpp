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
        [[nodiscard]] Byte operator [](Word address) const { return m_bytes[address]; }

        [[nodiscard]] Byte fetch_byte_and_proceed(Word &address, size_t &cycle) const { cycle++; return m_bytes[address++]; }
        [[nodiscard]] Byte fetch_byte(Word address, size_t &cycle) const { cycle++; return m_bytes[address]; }

        [[nodiscard]] Word fetch_word_and_proceed(Word &address, size_t &cycle) const;
        [[nodiscard]] Word fetch_word(Word address, size_t &cycle) const;

        [[nodiscard]] Word get_word(Word address) const;

        struct SetByteInputAddressModified { Word &address; Byte value; size_t &cycle; };
        void set_byte_and_proceed(SetByteInputAddressModified input) { input.cycle++; m_bytes[input.address++] = input.value; }

        struct SetByteInputAddressNotModified { Word address; Byte value; size_t &cycle; };
        void set_byte(SetByteInputAddressNotModified input) { input.cycle++; m_bytes[input.address] = input.value; }

        struct SetWordInputAddressModified { Word &address; Word value; size_t cycle; };
        void set_word_and_proceed(SetWordInputAddressModified input);

        struct SetWordInputAddressNotModified { Word address; Word value; size_t cycle; };
        void set_word(SetWordInputAddressNotModified input);

    private:
        std::array<Byte, UINT16_MAX> m_bytes;
    };

}

#endif //EMULATOR_MOS6502_ROM_HPP
