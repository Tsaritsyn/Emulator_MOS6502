//
// Created by Mikhail on 02/11/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Transfer, PLA) {
    Byte arg = GetParam();
    test_pull(PLA_IMPLICIT,
              arg,
              STACK_PULL_PARAMS,
              reader_from(AC));

    ProcessorStatus expectedFlags(0);
    if (arg == 0) expectedFlags[Emulator::Flag::ZERO] = SET;
    if ((char)arg < 0) expectedFlags[Emulator::Flag::NEGATIVE] = SET;
    EXPECT_EQ(SR, expectedFlags);
}

TEST_P(MOS6502_TestFixture_Transfer, PLP) {
    test_pull(PLP_IMPLICIT,
              GetParam(),
              STACK_PULL_PARAMS,
              [this]() { return SR.to_byte(); });
}
