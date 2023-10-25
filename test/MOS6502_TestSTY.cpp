//
// Created by Mikhail on 18/09/2023.
//

//#include "MOS6502_TestFixture.hpp"
//
//constexpr std::array<Byte, 4> testedInputsAND {0, 1, 10, static_cast<unsigned char>(-10)};
//
//static std::array<Addressing, 4> testedAddressings {
//        // no page crossing
//        Addressing::ZeroPage(0x10),
//        Addressing::ZeroPageX(0x10, 0x10),
//        Addressing::Absolute(0x2010),
//
//        // page crossing
//        Addressing::ZeroPageX(0xf0, 0x30),
//};
//
//TEST_F(MOS6502_TestFixture, Test_STY) {
//    for (auto value: testedInputsAND)
//        for (const auto& addressing: testedAddressings)
//            test_storage(Emulator::Register::Y, value, addressing);
//}