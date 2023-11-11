//
// Created by Mikhail on 16/10/2023.
//

#ifndef EMULATOR_MOS6502_ROM_HPP
#define EMULATOR_MOS6502_ROM_HPP

#include <format>
#include <expected>

#include "MOS6502_definitions.hpp"

namespace Emulator {

    class ROM {

    public:

        /// address where the attempt to write to was made
        struct StackOverride { Word address; };

        static constexpr Word INTERRUPT_HANDLER = 0xFFFA;
        static constexpr Word RESET_LOCATION = 0xFFFC;
        static constexpr Word BRK_HANDLER = 0xFFFE;
        static constexpr Word STACK_BOTTOM = 0x0100;


        ROM(): m_bytes{} {};

        void reset() noexcept { for (auto &byte: m_bytes) byte = 0; }

        /// simply returns a value at the given address
        [[nodiscard]] Byte operator [](Word address) const { return m_bytes[address]; }

        /// returns value at the given address, incrementing the cycle
        [[nodiscard]] Byte fetch_byte(Word address, size_t &cycle) const;

        /// simply returns a big-endian word with the low byte stored at the given address
        [[nodiscard]] Word get_word(Word address) const;

        std::expected<void, StackOverride> set_byte(Word address, Byte value);

        /// writes the byte to the given address incrementing only the cycle
        std::expected<void, StackOverride> set_byte(Word address, Byte value, size_t &cycle);

        [[nodiscard]] Byte stack(Byte index) const noexcept { return m_bytes[STACK_BOTTOM + index]; }
        Byte& stack(Byte index) noexcept                    { return m_bytes[STACK_BOTTOM + index]; }

        [[nodiscard]] static bool is_in_stack(Word address) noexcept { return (address >= STACK_BOTTOM) && (address <= STACK_BOTTOM + UINT8_MAX); }

    private:
        std::array<Byte, UINT16_MAX> m_bytes;
    };

}

#endif //EMULATOR_MOS6502_ROM_HPP
