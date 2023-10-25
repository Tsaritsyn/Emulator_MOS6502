//
// Created by Mikhail on 18/09/2023.
//

//#include "MOS6502_TestFixture.hpp"
//
//using namespace Emulator;
//
//constexpr std::array<Byte, 5> testedInputsAND{0, 1, static_cast<unsigned char>(-1), 10, static_cast<unsigned char>(-10)};
//
//TEST_F(MOS6502_TestFixture, TestPLA) {
//    for (auto value: testedInputsAND)
//        test_pull_from_stack(Emulator::Register::AC, value);
//}
//
//TEST_F(MOS6502_TestFixture, TestPLP) {
//    for (auto value: testedInputsAND)
//        test_pull_from_stack(Emulator::Register::SR, value);
//}