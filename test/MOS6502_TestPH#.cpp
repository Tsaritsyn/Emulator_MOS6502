//
// Created by Mikhail on 30/10/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Transfer, PHA) {
    test_push(PHA_IMPLICIT,
              GetParam(),
              STACK_PUSH_PARAMS,
              writer_to(AC));
    EXPECT_EQ(SR, 0);
}

TEST_P(MOS6502_TestFixture_Transfer, PHP) {
    test_push(PHP_IMPLICIT,
              GetParam(),
              STACK_PUSH_PARAMS,
              [this](Byte value) { SR = value; return WriteResult{}; });
}