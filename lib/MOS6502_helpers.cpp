//
// Created by Mikhail on 30/08/2023.
//

#include <iostream>
#include <format>
#include <utility>

#include "MOS6502_helpers.hpp"

namespace Emulator {

    void set_bit(Byte &byte, int number, bool value) {
        const Byte setter = static_cast<Byte>(1 << number);
        if (value) byte |= setter;
        else byte &= ~setter;
    }


    bool get_bit(Byte byte, int number) {
        return byte & (1 << number);
    }


    std::string to_string(Register aRegister) {
        switch (aRegister) {
            case Register::AC: return "A";
            case Register::X:  return "X";
            case Register::Y:  return "Y";
            case Register::SP: return "SP";
            case Register::SR: return "SR";
        }
        std::unreachable();
    }


    int add_with_overflow(int a, int b, bool &overflow, int rmin, int rmax) {
        const auto diff = rmax + 1 - rmin;

        int result = a + b + overflow;
        overflow = (result < rmin) || result > rmax;

        while (result < rmin) result += diff;
        while (result > rmax) result -= diff;
        return result;
    }

    int subtract_with_overflow(int a, int b, bool &overflow, int rmin, int rmax) {
        const auto diff = rmax + 1 - rmin;

        int result = a - b - !overflow;
        overflow = !((result < rmin) || result > rmax);

        while (result < rmin) result += diff;
        while (result > rmax) result -= diff;
        return result;
    }

}
