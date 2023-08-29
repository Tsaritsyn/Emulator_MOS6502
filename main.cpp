#include <iostream>

#include "MOS6502.hpp"

int main() {
//    Emulator::MOS6502 cpu{};
//
//    cpu.AC = 100;
//    std::cout << cpu.dump() << "\n\n";
//
//    cpu.PC = 0;
//    cpu.memory[0] = 1;
//    cpu.memory[1] = -10;
////    cpu.store_accumulator();
//    cpu.load_register(Emulator::Register::AC, Emulator::AddressingMode::ZERO_PAGE);
//
//    std::cout << cpu.dump() << '\n';

    Emulator::WordToBytes buf{};
    buf.set_high(0x12);
    buf.set_low(0x34);
    printf("%#04x\n", buf.word);

    return 0;
}
