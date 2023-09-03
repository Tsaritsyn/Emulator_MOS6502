#include <iostream>
#include <variant>

#include "MOS6502.hpp"

struct Case1 { static constexpr int n = 1; };
struct Case2 { static constexpr int n = 1; };
using Cases = std::variant<Case1, Case2>;


int main() {

    Cases cases = Case1{};


    return 0;
}
