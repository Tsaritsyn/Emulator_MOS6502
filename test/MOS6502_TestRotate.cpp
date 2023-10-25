//
// Created by Mikhail on 21/09/2023.
//

//#include "MOS6502_TestFixture.hpp"
//
//constexpr std::array<Byte, 9> testedInputs {0, 1, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
//
//static std::array<Addressing, 7> testedAddressings {
//        // no page crossing
//        Addressing::Accumulator(),
//        Addressing::ZeroPage(0x10),
//        Addressing::ZeroPageX(0x10, 0x10),
//        Addressing::Absolute(0x2010),
//        Addressing::AbsoluteX(0x2010, 0x10),
//
//        // page crossing
//        Addressing::ZeroPageX(0xf0, 0x30),
//        Addressing::AbsoluteX(0x20f0, 0x20),
//};
//
//
//TEST_F(MOS6502_TestFixture, TestROL) {
//    for (const auto value: testedInputs)
//        for (const auto& addressing: testedAddressings) {
//            test_rotate(ShiftDirection::LEFT, value, false, addressing);
//            test_rotate(ShiftDirection::LEFT, value, true, addressing);
//        }
//}
//
//TEST_F(MOS6502_TestFixture, TestROR) {
//    for (const auto value: testedInputs)
//        for (const auto& addressing: testedAddressings) {
//            test_rotate(ShiftDirection::RIGHT, value, false, addressing);
//            test_rotate(ShiftDirection::RIGHT, value, true, addressing);
//        }
//}
