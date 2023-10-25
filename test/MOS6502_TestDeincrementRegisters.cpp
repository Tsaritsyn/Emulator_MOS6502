//
// Created by Mikhail on 21/09/2023.
//

//#include "MOS6502_TestFixture.hpp"
//
//constexpr std::array<Byte, 5> testedInputs {0, 1, 10, static_cast<unsigned char>(-1), static_cast<unsigned char>(-10)};
//
//
//TEST_F(MOS6502_TestFixture, Test_INX) {
//    for (auto value: testedInputs)
//        test_deincrement_register(ChangeByOne::INCREMENT, value, Register::X);
//}
//
//TEST_F(MOS6502_TestFixture, Test_INY) {
//    for (auto value: testedInputs)
//        test_deincrement_register(ChangeByOne::INCREMENT, value, Register::Y);
//}
//
//TEST_F(MOS6502_TestFixture, Test_DEX) {
//    for (auto value: testedInputs)
//        test_deincrement_register(ChangeByOne::DECREMENT, value, Register::X);
//}
//
//TEST_F(MOS6502_TestFixture, Test_DEY) {
//    for (auto value: testedInputs)
//        test_deincrement_register(ChangeByOne::DECREMENT, value, Register::Y);
//}