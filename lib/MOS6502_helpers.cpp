//
// Created by Mikhail on 30/08/2023.
//

#include "MOS6502_helpers.hpp"

namespace Emulator {

    void set_bit(Byte &byte, int number, bool value) {
        if (value) byte |= 1 << number;
        else byte &= ~(1 << number);
    }


    bool check_bit(Byte byte, int number) {
        return byte & (1 << number);
    }


    Byte flag_combination(const std::vector<Flag> &flags) {
        Byte result = 0;
        for (auto flag: flags) set_bit(result, flag);
        return result;
    }


    std::ostream &operator<<(std::ostream &os, AddressingMode mode) {
        switch (mode) {
            case AddressingMode::IMMEDIATE:
                os << "Immediate";
                break;
            case AddressingMode::ZERO_PAGE:
                os << "Zero Page";
                break;
            case AddressingMode::ZERO_PAGE_X:
                os << "Zero Page,X";
                break;
            case AddressingMode::ZERO_PAGE_Y:
                os << "Zero Page,Y";
                break;
            case AddressingMode::RELATIVE:
                os << "Relative";
                break;
            case AddressingMode::ABSOLUTE:
                os << "Absolute";
                break;
            case AddressingMode::ABSOLUTE_X:
                os << "Absolute,X";
                break;
            case AddressingMode::ABSOLUTE_Y:
                os << "Absolute,Y";
                break;
            case AddressingMode::INDIRECT:
                os << "Indirect";
                break;
            case AddressingMode::INDIRECT_X:
                os << "Indexed Indirect";
                break;
            case AddressingMode::INDIRECT_Y:
                os << "Indirect Indexed";
                break;
        }
        return os;
    }


    std::ostream &operator<<(std::ostream &os, Register aRegister) {
        switch (aRegister) {
            case Register::AC:
                os << 'A';
                break;
            case Register::X:
                os << 'X';
                break;
            case Register::Y:
                os << 'Y';
                break;
            case Register::SP:
                os << "SP";
                break;
            case Register::SR:
                os << "SR";
                break;
        }
        return os;
    }

}
