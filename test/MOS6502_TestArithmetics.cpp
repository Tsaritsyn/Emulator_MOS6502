//
// Created by Mikhail on 19/09/2023.
//

//#include "MOS6502_TestFixture.hpp"
//
//constexpr std::array<std::tuple<Byte, Byte, bool>, 32> testedInputsADC {
//        std::tuple<Byte, Byte, bool>{0, 0, false},
//        {0, 0, true},
//        {0, 1, false},
//        {0, -1, false},
//        {0, 1, true},
//        {0, -1, true},
//        {0, 10, false},
//        {0, -10, false},
//        {0, 10, true},
//        {0, -10, true},
//        {10, -10, false},
//        {10, -10, true},
//        {10, -11, false},
//        {10, -11, true},
//        {10, -20, false},
//        {10, -20, true},
//        {10, 20, false},
//        {10, 20, true},
//        {-10, -20, false},
//        {-10, -20, true},
//        {127, 127, false},
//        {127, 127, true},
//        {-128, -128, false},
//        {-128, -128, true},
//        {-127, -127, false},
//        {-127, -127, true},
//        {100, -100, false},
//        {100, -100, true},
//        {100, -110, false},
//        {100, -110, true},
//        {110, -100, false},
//        {110, -100, true}
//};
//
//constexpr std::array<std::tuple<Byte, Byte, bool>, 20> testedInputsSBC {
//        std::tuple<Byte, Byte, bool>{0, 0, true},
//        {0, 0, false},
//        {0, 1, true},
//        {0, -1, true},
//        {0, 1, false},
//        {0, -1, false},
//        {0, 2, true},
//        {0, -2, true},
//        {0, 2, false},
//        {0, -2, false},
//        {10, 11, true},
//        {10, 11, false},
//        {10, 20, true},
//        {10, 20, false},
//        {-10, -11, true},
//        {-10, -11, false},
//        {-10, -20, true},
//        {-10, -20, false},
//        {10, -10, true},
//        {10, -10, false}
//};
//
//static std::array<Addressing, 13> testedAddressings {
//        // no page crossing
//        Addressing::Immediate(),
//        Addressing::ZeroPage(0x10),
//        Addressing::ZeroPageX(0x10, 0x10),
//        Addressing::Absolute(0x2010),
//        Addressing::AbsoluteX(0x2010, 0x10),
//        Addressing::AbsoluteY(0x2010, 0x10),
//        Addressing::IndirectX(0x10, 0x2010, 0x10),
//        Addressing::IndirectY(0x10, 0x2010, 0x10),
//
//        // page crossing
//        Addressing::ZeroPageX(0xf0, 0x30),
//        Addressing::AbsoluteX(0x20f0, 0x20),
//        Addressing::AbsoluteY(0x20f0, 0x20),
//        Addressing::IndirectX(0xf0, 0x2010, 0x30),
//        Addressing::IndirectY(0x10, 0x20f0, 0x30),
//};
//
//
//TEST_F(MOS6502_TestFixture, Test_ADC) {
//    for (const auto &[value1, value2, carry]: testedInputsADC)
//        for (const auto& addressing: testedAddressings) {
//            test_arithmetics(ArithmeticOperation::ADD, value1, value2, carry, addressing);
//            test_arithmetics(ArithmeticOperation::ADD, value2, value1, carry, addressing);
//        }
//}
//
//TEST_F(MOS6502_TestFixture, Test_SBC) {
//    for (const auto &[value1, value2, carry]: testedInputsSBC)
//        for (const auto& addressing: testedAddressings) {
//            test_arithmetics(ArithmeticOperation::SUB, value1, value2, carry, addressing);
//            test_arithmetics(ArithmeticOperation::SUB, value2, value1, carry, addressing);
//        }
//}