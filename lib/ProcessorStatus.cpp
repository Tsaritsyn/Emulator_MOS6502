//
// Created by Mikhail on 18/10/2023.
//

#include <utility>
#include <format>
#include <ranges>
#include <iostream>
#include <algorithm>

#include "ProcessorStatus.hpp"


std::string Emulator::to_string(Emulator::Flag flag) {
    switch (flag) {
        case Flag::NEGATIVE:          return "Negative";
        case Flag::OVERFLOW_F:        return "Overflow";
        case Flag::BREAK:             return "Break";
        case Flag::DECIMAL:           return "Decimal";
        case Flag::INTERRUPT_DISABLE: return "Interrupt disable";
        case Flag::ZERO:              return "Zero";
        case Flag::CARRY:             return "Carry";
    }
    std::unreachable();
}

bool Emulator::ProcessorStatus::operator[](Emulator::Flag i) const noexcept {
    switch (i)
    {
        case Flag::NEGATIVE:          return flags[7];
        case Flag::OVERFLOW_F:        return flags[6];
        case Flag::BREAK:             return flags[4];
        case Flag::DECIMAL:           return flags[3];
        case Flag::INTERRUPT_DISABLE: return flags[2];
        case Flag::ZERO:              return flags[1];
        case Flag::CARRY:             return flags[0];
    }

    std::unreachable();
}

bool &Emulator::ProcessorStatus::operator[](Emulator::Flag i) noexcept {
    switch (i) {
        case Flag::NEGATIVE:          return flags[7];
        case Flag::OVERFLOW_F:        return flags[6];
        case Flag::BREAK:             return flags[4];
        case Flag::DECIMAL:           return flags[3];
        case Flag::INTERRUPT_DISABLE: return flags[2];
        case Flag::ZERO:              return flags[1];
        case Flag::CARRY:             return flags[0];
    }

    std::unreachable();
}

Emulator::ProcessorStatus::ProcessorStatus(Emulator::Byte value) noexcept {
    for (bool & flag : flags) {
        flag = value % 2;
        value >>= 1;
    }
}

std::string Emulator::ProcessorStatus::to_string() const noexcept {
    std::string result;
    for (auto flag: flags) result += (flag) ? '1' : '0';
    return result;
}

std::string Emulator::ProcessorStatus::verbose_description() const noexcept {
    return std::vformat("Processor status register:\n\tCARRY = {:d}\n\tZERO = {:d}\n\tINTERRUPT DISABLE = {:d}\n\tDECIMAL = {:d}\n\tBREAK = {:d}\n\tOVERFLOW = {:d}\n\tNEGATIVE = {:d}\n",
                        std::make_format_args((*this)[Flag::CARRY], (*this)[Flag::ZERO], (*this)[Flag::INTERRUPT_DISABLE], (*this)[Flag::DECIMAL], (*this)[Flag::BREAK], (*this)[Flag::OVERFLOW_F], (*this)[Flag::NEGATIVE])
                        );
}

Emulator::Byte Emulator::ProcessorStatus::to_byte() const noexcept {
    int result = 0;
    for (auto flag: std::ranges::reverse_view(flags)) {
        result <<= 1;
        result += flag;
    }
    return static_cast<Byte>(result);
}

Emulator::ProcessorStatus Emulator::ProcessorStatus::operator|(const Emulator::ProcessorStatus &other) const noexcept {
    auto result = ProcessorStatus();
    for (size_t i = 0; i < flags.size(); i++)
        result.flags[i] = flags[i] || other.flags[i];
    return result;
}

bool Emulator::ProcessorStatus::operator==(const Emulator::ProcessorStatus &other) const noexcept {
    for (size_t i = 0; i < flags.size(); i++)
        if (flags[i] != other.flags[i]) return false;
    return true;
}

void Emulator::ProcessorStatus::reset() noexcept {
    for (bool & flag : flags) flag = false;
}

std::ostream &Emulator::operator<<(std::ostream &os, const Emulator::ProcessorStatus &status) noexcept {
    if (std::ranges::all_of(status.flags, [](bool flag){ return !flag; }))
        return os << "all zero";

    int ind = 0;
    std::ranges::for_each(status.flags, [&ind, &os](bool flag){ os << to_string(static_cast<Flag>(ind++)) << " = " << flag << ", "; });
    return os;
}
