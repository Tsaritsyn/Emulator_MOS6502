#include <iostream>
#include "MOS6502.hpp"

using namespace Emulator;

int main() {
    ROM memory;

    constexpr Word programStart = 0x0200;
    Byte index = 0;

    constexpr Byte n1 = 100;
    constexpr Byte n2 = 100;
    constexpr Word ADDRESS_RESULT_HIGH = 1;

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
     * Actual code
     */

    // STA $00
    memory[programStart + index++] = STA_ZERO_PAGE;
    memory[programStart + index++] = 0;

    // LDA #00
    memory[programStart + index++] = LDA_IMMEDIATE;
    memory[programStart + index++] = 0;

    // loop: CPX #0
    const Word loop = programStart + index;
    memory[programStart + index++] = CPX_IMMEDIATE;
    memory[programStart + index++] = 0;

    // BEQ +3
    memory[programStart + index++] = BEQ_RELATIVE;
    memory[programStart + index++] = 11;

    // CLC
    memory[programStart + index++] = CLC_IMPLICIT;

    // ADC $00
    memory[programStart + index++] = ADC_ZERO_PAGE;
    memory[programStart + index++] = 0;

    // BCC +3
    memory[programStart + index++] = BCC_RELATIVE;
    memory[programStart + index++] = 2;

    // INC ADDRESS_RESULT_HIGH
    memory[programStart + index++] = INC_ZERO_PAGE;
    memory[programStart + index++] = ADDRESS_RESULT_HIGH;

    // DEX
    memory[programStart + index++] = DEX_IMPLICIT;

    // JMP loop
    const WordToBytes buf(loop);
    memory[programStart + index++] = JMP_ABSOLUTE;
    memory[programStart + index++] = buf.low;
    memory[programStart + index++] = buf.high;

    // BRK
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
