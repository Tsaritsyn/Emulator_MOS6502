#include <iostream>

#include "MOS6502.hpp"

int main() {
    Emulator::MOS6502 cpu{};

    for (Emulator::Byte i = 0; i < 255; i++) {
//        printf("0x%02x\n", i);
        Emulator::OpCode opCode{i};
        printf("0x%02x\n", opCode);
        cpu.execute_command(opCode);
    }

    return 0;
}
