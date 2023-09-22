#include <iostream>
#include "Result.hpp"

Result<std::pair<int, int>> fn(int n) {
    if (n >= 0) return {{n, -n}};
    else return {"negative input"};
}

int main() {

    // will output "1 -1" twice
    {
        const auto result = fn(1);
        if (result.failed()) std::cout << "message: " << result.fail_message() << '\n';
        for (const auto &[x, y]: result) {
            std::cout << x << ' ' << y << '\n';
        }
        const auto &[x, y] = result.success();
        std::cout << x << ' ' << y << '\n';
    }

    // will output "message: negative input"
    {
        auto result = fn(-1);
        if (result.failed()) std::cout << "message: " << result.fail_message() << '\n';
        for (const auto &[x, y]: result) {
            std::cout << x << ' ' << y << '\n';
        }
        // will throw std::bad_optional_access
        // const auto &[x, y] = result.success();
        // std::cout << x << ' ' << y << '\n';
    }

    return 0;
}
