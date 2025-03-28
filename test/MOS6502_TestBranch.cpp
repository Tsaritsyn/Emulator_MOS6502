//
// Created by Mikhail on 25/09/2023.
//

#include "MOS6502_TestFixture.hpp"

using namespace Emulator;

constexpr std::array<Word, 6> testedPCs{0, 0xfe, 0x0200, 0x02ff, 0x03a7, 0xfffe};
constexpr std::array<char, 7> testedOffsets{0, 1, -1, 10, -10, 127, -128};


TEST_F(MOS6502_TestFixture, TestBCC) {
    for (const auto initialPC: testedPCs)
        for (const auto offset: testedOffsets) {
            test_branch(Flag::CARRY, SET, CLEAR, initialPC, offset);
            test_branch(Flag::CARRY, CLEAR, CLEAR, initialPC, offset);
        }
}

TEST_F(MOS6502_TestFixture, TestBCS) {
    for (const auto initialPC: testedPCs)
        for (const auto offset: testedOffsets) {
            test_branch(Flag::CARRY, SET, SET, initialPC, offset);
            test_branch(Flag::CARRY, CLEAR, SET, initialPC, offset);
        }
}

TEST_F(MOS6502_TestFixture, TestBNE) {
    for (const auto initialPC: testedPCs)
        for (const auto offset: testedOffsets) {
            test_branch(Flag::ZERO, SET, CLEAR, initialPC, offset);
            test_branch(Flag::ZERO, CLEAR, CLEAR, initialPC, offset);
        }
}

TEST_F(MOS6502_TestFixture, TestBEQ) {
    for (const auto initialPC: testedPCs)
        for (const auto offset: testedOffsets) {
            test_branch(Flag::ZERO, SET, SET, initialPC, offset);
            test_branch(Flag::ZERO, CLEAR, SET, initialPC, offset);
        }
}

TEST_F(MOS6502_TestFixture, TestBPL) {
    for (const auto initialPC: testedPCs)
        for (const auto offset: testedOffsets) {
            test_branch(Flag::NEGATIVE, SET, CLEAR, initialPC, offset);
            test_branch(Flag::NEGATIVE, CLEAR, CLEAR, initialPC, offset);
        }
}

TEST_F(MOS6502_TestFixture, TestBMI) {
    for (const auto initialPC: testedPCs)
        for (const auto offset: testedOffsets) {
            test_branch(Flag::NEGATIVE, SET, SET, initialPC, offset);
            test_branch(Flag::NEGATIVE, CLEAR, SET, initialPC, offset);
        }
}

TEST_F(MOS6502_TestFixture, TestBVC) {
    for (const auto initialPC: testedPCs)
        for (const auto offset: testedOffsets) {
            test_branch(Flag::OVERFLOW_F, SET, CLEAR, initialPC, offset);
            test_branch(Flag::OVERFLOW_F, CLEAR, CLEAR, initialPC, offset);
        }
}

TEST_F(MOS6502_TestFixture, TestBVS) {
    for (const auto initialPC: testedPCs)
        for (const auto offset: testedOffsets) {
            test_branch(Flag::OVERFLOW_F, SET, SET, initialPC, offset);
            test_branch(Flag::OVERFLOW_F, CLEAR, SET, initialPC, offset);
        }
}