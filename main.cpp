#include <iostream>

#include "MOS6502.hpp"
#include "programs.hpp"

using namespace Emulator;

int main() {
    ROM memory;

    constexpr Word programStart = 0x0200;
    Byte index = 0;

    constexpr Byte n1 = 255;
    constexpr Byte n2 = 255;

    /*
     * Start-up sequence
     */

    // LDX #FF
    memory[programStart + index++] = LDX_IMMEDIATE;
    memory[programStart + index++] = 0xff;
    // TSX
    memory[programStart + index++] = TSX_IMPLICIT;
    // CLI
    memory[programStart + index++] = CLI_IMPLICIT;

    /*
     * Preparing arguments
     */

    // LDA n1
    memory[programStart + index++] = LDA_IMMEDIATE;
    memory[programStart + index++] = n1;
    // LDX n2
    memory[programStart + index++] = LDX_IMMEDIATE;
    memory[programStart + index++] = n2;

    /*
     * Actual multiplication program
     */

    const auto program = program_multiplication(programStart + index);
    for (const auto byte: program)
        memory[programStart + index++] = byte;

    memory[programStart + index++] = BRK_IMPLICIT;


    /*
     * Setting PC after reset to the start of the program
     */

    const WordToBytes buf1(programStart);
    memory[MOS6502::RESET_LOCATION] = buf1.low;
    memory[MOS6502::RESET_LOCATION + 1] = buf1.high;


    /*
     * Burning the device and executing the program
     */

    MOS6502 cpu{};
    cpu.burn(memory);

    cpu.reset();

//    cpu.verbose = true;
    cpu.execute(true);

    std::cout << "\n\n";
    std::cout << cpu.dump(false) << '\n';


    return 0;
}
