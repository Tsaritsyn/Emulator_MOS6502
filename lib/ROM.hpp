//
// Created by Mikhail on 16/10/2023.
//

#ifndef EMULATOR_MOS6502_ROM_HPP
#define EMULATOR_MOS6502_ROM_HPP

#include <format>

#include "MOS6502_definitions.hpp"

namespace Emulator {

    class ROM {

    public:

        static constexpr Word INTERRUPT_HANDLER = 0xFFFA;
        static constexpr Word RESET_LOCATION = 0xFFFC;
        static constexpr Word BRK_HANDLER = 0xFFFE;


        ROM(): m_bytes{} {};

        void reset() noexcept { for (auto &byte: m_bytes) byte = 0; }

        /// simply returns a value at the given address
        [[nodiscard]] Byte operator [](Word address) const { return m_bytes[address]; }
        /// returns read-write value at a given address
        Byte& operator [](Word address);

        /// returns value at the given address, incrementing the address and the cycle
        [[nodiscard]] Byte fetch_byte_and_proceed(Word &address, size_t &cycle) const;
        /// returns value at the given address, incrementing only the cycle
        [[nodiscard]] Byte fetch_byte(Word address, size_t &cycle) const;

        /// returns big-endian word with the low byte stored at the given address, increases address and cycle
        [[nodiscard]] Word fetch_word_and_proceed(Word &address, size_t &cycle) const;
        /// returns big-endian word with the low byte stored at the given address, increases only cycle
        [[nodiscard]] Word fetch_word(Word address, size_t &cycle) const;

        /// simply returns a big-endian word with the low byte stored at the given address
        [[nodiscard]] Word get_word(Word address) const;

        struct SetByteInputAddressModified { Word &address; Byte value; size_t &cycle; };
        /// writes the byte to the given address incrementing the address and the cycle
        void set_byte_and_proceed(SetByteInputAddressModified input);

        struct SetByteInputAddressNotModified { Word address; Byte value; size_t &cycle; };
        /// writes the byte to the given address incrementing only the cycle
        void set_byte(SetByteInputAddressNotModified input);

        [[nodiscard]] Byte stack(Byte index) const noexcept { return m_bytes[STACK_BOTTOM + index]; }
        Byte& stack(Byte index) noexcept                    { return m_bytes[STACK_BOTTOM + index]; }

        [[nodiscard]] static bool is_in_stack(Word address) noexcept { return (address >= STACK_BOTTOM) && (address <= STACK_BOTTOM + UINT8_MAX); }

    private:
        static constexpr Word STACK_BOTTOM = 0x0100;

        std::array<Byte, UINT16_MAX> m_bytes;
    };

}

#endif //EMULATOR_MOS6502_ROM_HPP
