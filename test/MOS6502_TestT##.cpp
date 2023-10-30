//
// Created by Mikhail on 29/10/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Transfer, TAX) {
    test_transfer_with_flags(TAX_IMPLICIT,
                             GetParam(),
                             ExecutionParameters::implied(),
                             writer_to(AC),
                             reader_from(X));
}

TEST_P(MOS6502_TestFixture_Transfer, TAY) {
    test_transfer_with_flags(TAY_IMPLICIT,
                             GetParam(),
                             ExecutionParameters::implied(),
                             writer_to(AC),
                             reader_from(Y));
}

TEST_P(MOS6502_TestFixture_Transfer, TXA) {
    test_transfer_with_flags(TXA_IMPLICIT,
                             GetParam(),
                             ExecutionParameters::implied(),
                             writer_to(X),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, TYA) {
    test_transfer_with_flags(TYA_IMPLICIT,
                             GetParam(),
                             ExecutionParameters::implied(),
                             writer_to(Y),
                             reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, TSX) {
    test_transfer_with_flags(TSX_IMPLICIT,
                             GetParam(),
                             ExecutionParameters::implied(),
                             writer_to(SP),
                             reader_from(X));
}

TEST_P(MOS6502_TestFixture_Transfer, TXS) {
    test_transfer_without_flags(TXS_IMPLICIT,
                                GetParam(),
                                ExecutionParameters::implied(),
                                writer_to(X),
                                reader_from(SP));
}
