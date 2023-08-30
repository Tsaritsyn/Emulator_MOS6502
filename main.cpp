#include <iostream>

#include "MOS6502.hpp"

int main() {
//    Emulator::MOS6502 cpu{};
//

//
//    std::cout << cpu.dump() << '\n';


    Emulator::Converter converter{};
    converter.high = 0x12;
    converter.low = 0x34;
    printf("%#04x\n", converter.buffer.word);

    return 0;
}
