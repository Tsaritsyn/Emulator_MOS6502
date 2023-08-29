//
// Created by Mikhail on 29/08/2023.
//

#ifndef EMULATOR_MOS6502_TEST_HPP
#define EMULATOR_MOS6502_TEST_HPP

#include <gtest/gtest.h>

#include "../MOS6502.hpp"



class MOS6502_TestFixture: public ::testing::Test {
    Emulator::MOS6502 cpu;

};



#endif //EMULATOR_MOS6502_TEST_HPP
