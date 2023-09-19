#include <iostream>
#include <variant>
#include <utility>

struct One {int a;};
struct Two {int a; double b;};
using Variants = std::variant<One, Two>;

void print(Variants variants) {
    if (const auto ptrOne = std::get_if<One>(&variants)) {
        const auto &[a] = *ptrOne;
        std::cout << "One(a: " << a << ")\n";
        return;
    }
    if (const auto ptrTwo = std::get_if<Two>(&variants)) {
        const auto &[a, b] = *ptrTwo;
        std::cout << "Two(a: " << a << ", b: " << b << ")\n";
        return;
    }
    std::unreachable();
}


int main() {

    print(One{1});
    print(Two{3, 2.5});

    return 0;
}
