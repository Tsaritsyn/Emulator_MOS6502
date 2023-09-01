#include <iostream>

#include "MOS6502.hpp"


template <class TL, class TR>
constexpr auto circ_add(
        const TL lhs,
        const TR rhs,
        // if we put these two values to the template, we will only be able to make them integers
        const decltype(lhs+rhs) min = 0,
        const decltype(lhs+rhs) max = 360
        ) {
    auto result = lhs + rhs;
    auto range = max - min;

    while (result >= max) result -= range;
    while (result < min) result += range;

    return result;
}

int main() {

    constexpr int a = 200;
    constexpr int b = 200;
    constexpr auto c = circ_add(a, b);

    constexpr int d = 50;
    constexpr float e = -100.9;
    constexpr auto f = circ_add(d, e);


//    Emulator::MOS6502 cpu{};
//
//    for (Emulator::Byte i = 0; i < 255; i++) {
////        printf("0x%02x\n", i);
//        Emulator::OpCode opCode{i};
//        printf("0x%02x\n", opCode);
//        cpu.execute_command(opCode);
//    }

    return 0;
}
