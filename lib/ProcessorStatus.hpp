//
// Created by Mikhail on 18/10/2023.
//

#ifndef EMULATOR_MOS6502_PROCESSORSTATUS_HPP
#define EMULATOR_MOS6502_PROCESSORSTATUS_HPP

#include <array>
#include <string>
#include "MOS6502_definitions.hpp"

namespace Emulator {

    enum class Flag {
        /**
         * The negative flag is set if the result of the last operation had bit 7 set to a one.
         */
        NEGATIVE = 7,

        /**
         * The overflow flag is set during arithmetic operations if the result has yielded an invalid 2's complement result
         *  (e.g. adding to positive numbers and ending up with a negative result: 64 + 64 => -128).
         * It is determined by looking at the carry between bits 6 and 7 and between bit 7 and the carry flag.
         */
        OVERFLOW_F = 6,

        /**
         * The break command bit is set when a BRK instruction has been executed and an interrupt has been generated to process it.
         */
        BREAK = 4,

        /**
         * While the decimal mode flag is set the processor will obey the rules of Binary Coded Decimal (BCD) arithmetic during addition and subtraction.
         * The flag can be explicitly set using 'Set Decimal Flag' (SED) and cleared with 'Clear Decimal Flag' (CLD).
         */
        // TODO: implement addition and subtraction in decimal mode
        DECIMAL = 3,

        /**
         * The interrupt disable flag is set if the program has executed a 'Set Interrupt Disable' (SEI) instruction.
         * While this flag is set the processor will not respond to interrupts from devices until it is cleared by a 'Clear Interrupt Disable' (CLI) instruction.
         */
        INTERRUPT_DISABLE = 2,

        /**
         * The zero flag is set if the result of the last operation as was zero.
         */
        ZERO = 1,

        /**
         * The carry flag is set if the last operation caused an overflow from bit 7 of the result or an underflow from bit 0.
         * This condition is set during arithmetic, comparison and during logical shifts.
         * It can be explicitly set using the 'Set Carry Flag' (SEC) instruction and cleared with 'Clear Carry Flag' (CLC).
         */
        CARRY = 0
    };

    std::string to_string(Flag flag);



    class ProcessorStatus {
    public:

        ProcessorStatus(Byte value = 0) noexcept;

        void reset() noexcept;

        [[nodiscard]] bool operator [](Flag i) const noexcept;
        [[nodiscard]] bool& operator [](Flag i) noexcept;

        [[nodiscard]] std::string to_string() const noexcept;

        [[nodiscard]] std::string verbose_description() const noexcept;

        [[nodiscard]] Byte to_byte() const noexcept;

        ProcessorStatus operator |(const ProcessorStatus &other) const noexcept;

        bool operator ==(int value) const noexcept { return to_byte() == value; }
        bool operator ==(const ProcessorStatus &other) const noexcept;

        friend std::ostream& operator <<(std::ostream& os, const ProcessorStatus& status);

    private:
        std::array<bool, 8> flags;
    };

}


#endif //EMULATOR_MOS6502_PROCESSORSTATUS_HPP
