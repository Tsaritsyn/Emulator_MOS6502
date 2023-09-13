//
// Created by Mikhail on 13/09/2023.
//

#include "MOS6502_TestFixture.hpp"

constexpr std::array<Branch, 2> testedInputs {
        Branch{CARRY, CLEAR, true},
        {CARRY, CLEAR, false}
};


constexpr std::array<Relative, 2> testedAddressings {
    Relative{0, }
};