//
// Created by Mikhail on 14/09/2023.
//

#include "helpers.hpp"

ProcessorStatus status_from_flags(const std::vector<Flag>& flags) {
    ProcessorStatus result(0);
    for (auto flag: flags) result[flag] = SET;
    return result;
}

std::ostream &operator<<(std::ostream &os, Flag flag) {
    return os << to_string(flag);
}

