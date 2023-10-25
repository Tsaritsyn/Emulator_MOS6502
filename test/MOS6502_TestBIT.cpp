//
// Created by Mikhail on 19/09/2023.
//

//#include "MOS6502_TestFixture.hpp"
//
//constexpr std::array<std::pair<Byte, Byte>, 9> testedInputs {
//        std::pair<Byte, Byte>{0, 0},
//        {0, 1},
//        {0xff, 1},
//        {0xff, 0},
//        {10, 10},
//        {0xff, 0x7a},
//        {0xff, -100},
//        {0xf0, 0x0f},
//        {0xfd, 0x6f}
//};
//
//static std::array<Addressing, 2> testedAddressings {
//        Addressing::ZeroPage(0x10),
//        Addressing::Absolute(0x2010),
//};
//
//
//TEST_F(MOS6502_TestFixture, Test_BIT) {
//    for (const auto &[value1, value2]: testedInputs)
//        for (const auto& addressing: testedAddressings){
//            test_bit_test(value1, value2, addressing);
//            test_bit_test(value2, value1, addressing);
//        }
//}
