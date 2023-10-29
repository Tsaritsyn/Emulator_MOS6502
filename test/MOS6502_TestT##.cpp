//
// Created by Mikhail on 29/10/2023.
//

#include "MOS6502_TestFixture.hpp"


TEST_P(MOS6502_TestFixture_Transfer, TAX) {
test_transfer(TAX_IMPLICIT,
        GetParam(),
        ExecutionParameters::implied(),
        writer_to(AC),
        reader_from(X));
}

TEST_P(MOS6502_TestFixture_Transfer, TAY) {
    test_transfer(TAY_IMPLICIT,
                  GetParam(),
                  ExecutionParameters::implied(),
                  writer_to(AC),
                  reader_from(Y));
}

TEST_P(MOS6502_TestFixture_Transfer, TXA) {
    test_transfer(TXA_IMPLICIT,
                  GetParam(),
                  ExecutionParameters::implied(),
                  writer_to(X),
                  reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, TYA) {
    test_transfer(TYA_IMPLICIT,
                  GetParam(),
                  ExecutionParameters::implied(),
                  writer_to(Y),
                  reader_from(AC));
}

TEST_P(MOS6502_TestFixture_Transfer, TSX) {
    test_transfer(TSX_IMPLICIT,
                  GetParam(),
                  ExecutionParameters::implied(),
                  writer_to(SP),
                  reader_from(X));
}

TEST_P(MOS6502_TestFixture_Transfer, TXS) {
    test_transfer_to_SP(TXS_IMPLICIT,
                  GetParam(),
                  ExecutionParameters::implied(),
                  writer_to(X),
                  reader_from(SP));
}
