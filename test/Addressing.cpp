//
// Created by Mikhail on 09/09/2023.
//

#include <iostream>
#include "Addressing.hpp"

Word size(const Addressing &addressing) {
    if (std::get_if<Implicit>(&addressing) ||
        std::get_if<Accumulator>(&addressing)) return 1;
    else if (std::get_if<Immediate>(&addressing) ||
             std::get_if<ZeroPage>(&addressing) ||
             std::get_if<ZeroPageX>(&addressing) ||
             std::get_if<ZeroPageY>(&addressing) ||
             std::get_if<Relative>(&addressing) ||
             std::get_if<IndirectX>(&addressing) ||
             std::get_if<IndirectY>(&addressing)) return 2;
    else if (std::get_if<Absolute>(&addressing) ||
             std::get_if<AbsoluteX>(&addressing) ||
             std::get_if<AbsoluteY>(&addressing) ||
             std::get_if<Indirect>(&addressing)) return 3;
    else throw std::runtime_error("MOS6502_TestFixture::size: unhandled addressing");
}


AddressingMode mode(const Addressing &addressing) {
    if (std::get_if<Implicit>(&addressing))
        return Emulator::AddressingMode::IMPLICIT;
    else if (std::get_if<Accumulator>(&addressing))
        return Emulator::AddressingMode::ACCUMULATOR;
    else if (std::get_if<Immediate>(&addressing))
        return Emulator::AddressingMode::IMMEDIATE;
    else if (std::get_if<ZeroPage>(&addressing))
        return Emulator::AddressingMode::ZERO_PAGE;
    else if (std::get_if<ZeroPageX>(&addressing))
        return Emulator::AddressingMode::ZERO_PAGE_X;
    else if (std::get_if<ZeroPageY>(&addressing))
        return Emulator::AddressingMode::ZERO_PAGE_Y;
    else if (std::get_if<Relative>(&addressing))
        return Emulator::AddressingMode::RELATIVE;
    else if (std::get_if<Absolute>(&addressing))
        return Emulator::AddressingMode::ABSOLUTE;
    else if (std::get_if<AbsoluteX>(&addressing))
        return Emulator::AddressingMode::ABSOLUTE_X;
    else if (std::get_if<AbsoluteY>(&addressing))
        return Emulator::AddressingMode::ABSOLUTE_Y;
    else if (std::get_if<Indirect>(&addressing))
        return Emulator::AddressingMode::INDIRECT;
    else if (std::get_if<IndirectX>(&addressing))
        return Emulator::AddressingMode::INDIRECT_X;
    else if (std::get_if<IndirectY>(&addressing))
        return Emulator::AddressingMode::INDIRECT_Y;
    else throw std::runtime_error("MOS6502_TestFixture::mode: unhandled addressing");
}


std::ostream &operator<<(std::ostream &os, Addressing addressing) {
    if (std::get_if<Implicit>(&addressing))
        return os << "Implicit";
    else if (std::get_if<Accumulator>(&addressing))
        return os << "Accumulator";
    else if (const auto immediate = std::get_if<Immediate>(&addressing))
        return os << "Immediate";
    else if (const auto zeroPage = std::get_if<ZeroPage>(&addressing))
        return os << "Zero Page(address: " << HEX_BYTE(zeroPage->address) << ")";
    else if (const auto zeroPageX = std::get_if<ZeroPageX>(&addressing))
        return os << "Zero Page,X(address: " << HEX_BYTE(zeroPageX->address) << ", X: " << HEX_BYTE(zeroPageX->X) << ")";
    else if (const auto zeroPageY = std::get_if<ZeroPageY>(&addressing))
        return os << "Zero Page,Y(address: " << HEX_BYTE(zeroPageY->address) << ", Y: " << HEX_BYTE(zeroPageY->Y) << ")";
    else if (const auto relative = std::get_if<Relative>(&addressing))
        return os << "Relative(offset: " << (int)(relative->offset) << ")";
    else if (const auto absolute = std::get_if<Absolute>(&addressing))
        return os << "Absolute(address: " << HEX_WORD(absolute->address) << ")";
    else if (const auto absoluteX = std::get_if<AbsoluteX>(&addressing))
        return os << "Absolute,X(address: " << HEX_WORD(absoluteX->address) << ", X: " << HEX_BYTE(absoluteX->X) << ")";
    else if (const auto absoluteY = std::get_if<AbsoluteY>(&addressing))
        return os << "Absolute,Y(address: " << HEX_WORD(absoluteY->address) << ", Y: " << HEX_BYTE(absoluteY->Y) << ")";
    else if (const auto indirect = std::get_if<Indirect>(&addressing))
        return os << "Indirect(address: " << HEX_WORD(indirect->address) << ", new address: " << HEX_WORD(indirect->newAddress) << ")";
    else if (const auto indirectX = std::get_if<IndirectX>(&addressing))
        return os << "Indexed Indirect(table address: " << HEX_BYTE(indirectX->tableAddress) << ", target address: " << HEX_WORD(indirectX->targetAddress) << ", X: " << HEX_BYTE(indirectX->X) << ")";
    else if (const auto indirectY = std::get_if<IndirectY>(&addressing))
        return os << "Indirect Indexed(table address: " << HEX_BYTE(indirectY->tableAddress) << ", target address: " << HEX_WORD(indirectY->targetAddress) << ", Y: " << HEX_BYTE(indirectY->Y) << ")";
    else throw std::runtime_error("operator<<: unhandled addressing");
}
