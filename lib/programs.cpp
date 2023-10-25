//
// Created by Mikhail on 04/10/2023.
//

#include "programs.hpp"

std::array<Byte, 29> program_multiplication(Word startAddress) {
    /*
     * Assembly listing of the program:
     *
     * TAY             ; transfer first argument to Y
     * LDA TMP_ADDRESS ; load byte of temporary address to accumulator
     * PHA             ; push byte of temporary address from accumulator to stack
     * STY TMP_ADDRESS ; store the first argument to the temporary address
     * STA TMP_ADDRESS ; store the first number to temporary memory address
     * LDA #00         ; initialize the result (low byte)
     * LDY #00         ; initialize the result (high byte)
     * loop: CPX #0    ; start the loop - compare the second number to 0
     * BEQ +10         ; if it is zero, then jump out of the loop, to the next instruction after the program
     * CLC             ; clear carry
     * ADC TMP_ADDRESS ; add the first number to the result
     * BCC +1          ; if carry is not set, skip the next instruction
     * INY             ; if carry is set, increment the high byte of the result
     * DEX             ; decrement the second number
     * JMP loop        ; continue the loop
     * TAX             ; transfer low byte of result to X
     * PLA             ; pull byte of temporary address from stack to accumulator
     * STA TMP_ADDRESS ; store byte of temporary address back to memory
     * TXA             ; transfer low byte of result back to accumulator
     */

    constexpr Byte TMP_ADDRESS = 0;
    std::array<Byte, 29> program{};
    Byte index = 0;


    /*
     * Pushing the byte at the TMP_ADDRESS to stack
     */
    program[index++] = TAY_IMPLICIT;

    program[index++] = LDA_ZERO_PAGE;
    program[index++] = TMP_ADDRESS;

    program[index++] = PHA_IMPLICIT;

    program[index++] = STY_ZERO_PAGE;
    program[index++] = TMP_ADDRESS;


    /*
     * Multiplying two numbers
     */

    program[index++] = LDA_IMMEDIATE;
    program[index++] = 0;

    program[index++] = LDY_IMMEDIATE;
    program[index++] = 0;

    const Word loop = startAddress + index;
    program[index++] = CPX_IMMEDIATE;
    program[index++] = 0;

    program[index++] = BEQ_RELATIVE;
    program[index++] = 10;

    program[index++] = CLC_IMPLICIT;

    program[index++] = ADC_ZERO_PAGE;
    program[index++] = TMP_ADDRESS;

    program[index++] = BCC_RELATIVE;
    program[index++] = 1;

    program[index++] = INY_IMPLICIT;

    program[index++] = DEX_IMPLICIT;

    const WordToBytes buf(loop);
    program[index++] = JMP_ABSOLUTE;
    program[index++] = buf.low();
    program[index++] = buf.high();


    /*
     * Pulling byte of TMP_ADDRESS from stack
     */

    program[index++] = TAX_IMPLICIT;

    program[index++] = PLA_IMPLICIT;

    program[index++] = STA_ZERO_PAGE;
    program[index++] = TMP_ADDRESS;

    program[index++] = TXA_IMPLICIT;

    return program;
}
