//
// Created by Mikhail on 30/10/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Push, PHA) {
    test_push(PHA_IMPLICIT, GetParam(), ExecutionParameters::stack_push(), writer_to(AC));
    EXPECT_EQ(SR, 0);
}

TEST_P(MOS6502_TestFixture_Push, PHP) {
    test_push(PHP_IMPLICIT,
              GetParam(),
              ExecutionParameters::stack_push(),
              [this](Byte value) { SR = value; return WriteResult{}; });
}


INSTANTIATE_TEST_SUITE_P(AllTests, MOS6502_TestFixture_Push, ::testing::Values(0, 1, 10, 100, (Byte)-10, 255));