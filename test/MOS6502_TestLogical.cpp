//
// Created by Mikhail on 18/09/2023.
//

//#include "MOS6502_TestFixture.hpp"
//
//constexpr std::array<std::pair<Byte, Byte>, 9> testedInputsAND {
//    std::pair<Byte, Byte>{0, 0},
//    {0, 1},
//    {0xff, 1},
//    {0xff, 0},
//    {10, 10},
//    {0xff, 0x7a},
//    {0xff, -100},
//    {0xf0, 0x0f},
//    {0xfd, 0x6f}
//};
//
//constexpr std::array<std::pair<Byte, Byte>, 9> testedInputsOR {
//        std::pair<Byte, Byte>{0, 0},
//        {0, 1},
//        {0xff, 1},
//        {0xff, 0},
//        {10, 10},
//        {0, 0x7a},
//        {0, -100},
//        {0xf0, 0x0f},
//        {0xfd, 0x6f}
//};
//
//constexpr std::array<std::pair<Byte, Byte>, 9> testedInputsXOR {
//        std::pair<Byte, Byte>{0, 0},
//        {0, 1},
//        {0xff, 1},
//        {0xff, 0},
//        {10, 10},
//        {0, 0x7a},
//        {0, -100},
//        {0xf0, 0x0f},
//        {0xfd, 0x6f}
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
//TEST_F(MOS6502_TestFixture, Test_AND) {
//    for (const auto &[value1, value2]: testedInputsAND)
//        for (const auto& addressing: testedAddressings) {
//            test_logical(LogicalOperation::AND, value1, value2, addressing);
//            test_logical(LogicalOperation::AND, value2, value1, addressing);
//        }
//}
//
//TEST_F(MOS6502_TestFixture, Test_OR) {
//    for (const auto &[value1, value2]: testedInputsOR)
//        for (const auto& addressing: testedAddressings) {
//            test_logical(LogicalOperation::OR, value1, value2, addressing);
//            test_logical(LogicalOperation::OR, value2, value1, addressing);
//        }
//}
//
//TEST_F(MOS6502_TestFixture, Test_XOR) {
//    for (const auto &[value1, value2]: testedInputsXOR)
//        for (const auto& addressing: testedAddressings) {
//            test_logical(LogicalOperation::XOR, value1, value2, addressing);
//            test_logical(LogicalOperation::XOR, value2, value1, addressing);
//        }
//}
