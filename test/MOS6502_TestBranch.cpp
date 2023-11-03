//
// Created by Mikhail on 03/11/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Branch, BCC) {
    test_branch(BCC_RELATIVE,
                GetParam(),
                Flag::CARRY,
                CLEAR);
}

TEST_P(MOS6502_TestFixture_Branch, BCS) {
    test_branch(BCS_RELATIVE,
                GetParam(),
                Flag::CARRY,
                SET);
}

TEST_P(MOS6502_TestFixture_Branch, BEQ) {
    test_branch(BEQ_RELATIVE,
                GetParam(),
                Flag::ZERO,
                SET);
}

TEST_P(MOS6502_TestFixture_Branch, BNE) {
    test_branch(BNE_RELATIVE,
                GetParam(),
                Flag::ZERO,
                CLEAR);
}

TEST_P(MOS6502_TestFixture_Branch, BMI) {
    test_branch(BMI_RELATIVE,
                GetParam(),
                Flag::NEGATIVE,
                SET);
}

TEST_P(MOS6502_TestFixture_Branch, BPL) {
    test_branch(BPL_RELATIVE,
                GetParam(),
                Flag::NEGATIVE,
                CLEAR);
}

TEST_P(MOS6502_TestFixture_Branch, BVC) {
    test_branch(BVC_RELATIVE,
                GetParam(),
                Flag::OVERFLOW_F,
                CLEAR);
}

TEST_P(MOS6502_TestFixture_Branch, BVS) {
    test_branch(BVS_RELATIVE,
                GetParam(),
                Flag::OVERFLOW_F,
                SET);
}


INSTANTIATE_TEST_SUITE_P(PositiveOffset_SamePage, MOS6502_TestFixture_Branch, ::testing::Values(
    BranchParameters{.initialPC = 0, .offset = 1, .finalPC = 3, .doBranch = true, .duration = BRANCH_DURATION_JUMP_SAME_PAGE},
    BranchParameters{.initialPC = 0, .offset = 1, .finalPC = 2, .doBranch = false, .duration = BRANCH_DURATION_NO_JUMP},
    BranchParameters{.initialPC = 0, .offset = 13, .finalPC = 15, .doBranch = true, .duration = BRANCH_DURATION_JUMP_SAME_PAGE},
    BranchParameters{.initialPC = 0, .offset = 13, .finalPC = 2, .doBranch = false, .duration = BRANCH_DURATION_NO_JUMP}
));

INSTANTIATE_TEST_SUITE_P(NegativeOffset_SamePage, MOS6502_TestFixture_Branch, ::testing::Values(
    BranchParameters{.initialPC = 20, .offset = -1, .finalPC = 21, .doBranch = true, .duration = BRANCH_DURATION_JUMP_SAME_PAGE},
    BranchParameters{.initialPC = 20, .offset = -1, .finalPC = 22, .doBranch = false, .duration = BRANCH_DURATION_NO_JUMP},
    BranchParameters{.initialPC = 20, .offset = -10, .finalPC = 12, .doBranch = true, .duration = BRANCH_DURATION_JUMP_SAME_PAGE},
    BranchParameters{.initialPC = 20, .offset = -10, .finalPC = 22, .doBranch = false, .duration = BRANCH_DURATION_NO_JUMP}
));

INSTANTIATE_TEST_SUITE_P(ZeroOffset, MOS6502_TestFixture_Branch, ::testing::Values(
    BranchParameters{.initialPC = 20, .offset = 0, .finalPC = 22, .doBranch = true, .duration = BRANCH_DURATION_JUMP_SAME_PAGE},
    BranchParameters{.initialPC = 20, .offset = 0, .finalPC = 22, .doBranch = false, .duration = BRANCH_DURATION_NO_JUMP}
));

INSTANTIATE_TEST_SUITE_P(PositiveOffset_DifferentPage, MOS6502_TestFixture_Branch, ::testing::Values(
    BranchParameters{.initialPC = 0x0200 + 200, .offset = 100, .finalPC = 0x0200 + 302, .doBranch = true, .duration = BRANCH_DURATION_JUMP_OTHER_PAGE},
    BranchParameters{.initialPC = 0x0200 + 200, .offset = 100, .finalPC = 0x0200 + 202, .doBranch = false, .duration = BRANCH_DURATION_NO_JUMP}
));

INSTANTIATE_TEST_SUITE_P(NegativeOffset_DifferentPage, MOS6502_TestFixture_Branch, ::testing::Values(
    BranchParameters{.initialPC = 0x0300 + 50, .offset = -100, .finalPC = 0x0300 + (52 - 100), .doBranch = true, .duration = BRANCH_DURATION_JUMP_OTHER_PAGE},
    BranchParameters{.initialPC = 0x0300 + 50, .offset = -100, .finalPC = 0x0300 + 52, .doBranch = false, .duration = BRANCH_DURATION_NO_JUMP}
));
