#include <iostream>

#include "CPU.hpp"

int main() {
    CPU cpu{};

    cpu.AC = 100;
    std::cout << cpu.dump() << "\n\n";

    cpu.PC = 0;
    cpu.memory[0] = 1;
    cpu.memory[1] = -10;
//    cpu.store_accumulator();
    cpu.load_register(CPU::Register::AC, CPU::AddressingMode::ZERO_PAGE);

    std::cout << cpu.dump() << '\n';

    return 0;
}
