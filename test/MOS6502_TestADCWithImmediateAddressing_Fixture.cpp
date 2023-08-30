//
// Created by Mikhail on 30/08/2023.
//

#include "MOS6502_TestADCWithImmediateAddressing_Fixture.hpp"
#include "MOS6502_helpers.hpp"

using namespace Emulator;

// Tests without carry

TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoPositivesWOCarryToPositive1) {
    test(1, 2, false, 3, 0);
}

TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoPositivesWOCarryToPositive2) {
    test(2, 1, false, 3, 0);
}


TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_PositiveAndZeroWOCarryToPositive1) {
    test(1, 0, false, 1, 0);
}

TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_PositiveAndZeroWOCarryToPositive2) {
    test(0, 1, false, 1, 0);
}


TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoZerosWOCarryToZero) {
    test(0, 0, false, 0, flag_combination({ZERO}));
}


TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_NegativeAndZeroWOCarryToNegative1) {
    test(-1, 0, false, -1, flag_combination({NEGATIVE}));
}

TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_NegativeAndZeroWOCarryToNegative2) {
    test(0, -1, false, -1, flag_combination({NEGATIVE}));
}


TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoPositivesWOCarryToNegative1) {
    test(100, 110, false, -46, flag_combination({NEGATIVE, OVERFLOW}));
}

TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoPositivesWOCarryToNegative2) {
    test(110, 100, false, -46, flag_combination({NEGATIVE, OVERFLOW}));
}


TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_PositiveAndNegativeWOCarryToZero1) {
    test(1, -1, false, 0, flag_combination({ZERO, CARRY}));
}

TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_PositiveAndNegativeWOCarryToZero2) {
    test(-1, 1, false, 0, flag_combination({ZERO, CARRY}));
}


TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoNegativesWOCarryToNegative1) {
    test(-10, -20, false, -30, flag_combination({NEGATIVE, CARRY}));
}

TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoNegativesWOCarryToNegative2) {
    test(-20, -10, false, -30, flag_combination({NEGATIVE, CARRY}));
}


TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoNegativesWOCarryToPositive1) {
    test(-70, -80, false, 106, flag_combination({CARRY, OVERFLOW}));
}

TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoNegativesWOCarryToPositive2) {
    test(-80, -70, false, 106, flag_combination({CARRY, OVERFLOW}));
}



// Tests with carry

TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoPositivesWithCarryToPositive1) {
    test(1, 2, true, 4, 0);
}

TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoPositivesWithCarryToPositive2) {
    test(2, 1, true, 4, 0);
}


TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoZerosWithCarryToPositive) {
    test(0, 0, true, 1, 0);
}


TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_NegativeAndZeroWithCarryToZero1) {
    test(-1, 0, true, 0, flag_combination({ZERO, CARRY}));
}

TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_NegativeAndZeroWithCarryToZero2) {
    test(0, -1, true, 0, flag_combination({ZERO, CARRY}));
}


TEST_F(MOS6502_TestADCWithImmediateAddressing_Fixture, Test_TwoPositivesWithCarryToZero) {
    test(127, 127, true, -1, flag_combination({NEGATIVE, OVERFLOW}));
}