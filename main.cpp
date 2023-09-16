#include <iostream>
#include <variant>

#include "MOS6502.hpp"

template<typename T>
concept Printable = requires(std::ostream& os, T a) {
    os << a;
};

void print() {
    std::cout << '\n';
}

void print(const Printable auto& value) {
    std::cout << value << '\n';
}

//template<Printable T, Printable... Args>
//void print(const T& first, const Args&... args) {
//    std::cout << first << ", ";
//    print(args...);
//}

void print(const Printable auto& first, const Printable auto&... args) {
    std::cout << first << ", ";
    print(args...);
}

struct Struct {
    friend std::ostream &operator<<(std::ostream &os, const Struct &aStruct) {
        return os << "Struct";
    }
};


int main() {
    print();
    print(1);
    print(1, 2);
    print(1, 2, 3, 4);
    print(Struct{});

    return 0;
}
