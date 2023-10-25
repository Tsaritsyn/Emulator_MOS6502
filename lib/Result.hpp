//
// Created by Mikhail on 22/09/2023.
//

#ifndef EMULATOR_MOS6502_RESULT_HPP
#define EMULATOR_MOS6502_RESULT_HPP

#include <string_view>
#include <optional>


/**
 * This class mimics the behaviour of Rust's or Swift's Result type.
 * It provides an iterator to access its content.
 * Therefore, if used in a for loop, it can be decomposed directly, without need of dereferencing the pointer.
 * If Result contains value, the iterator is equal to begin(), and being incremented, gets to end(), so the loop will only have one iteration.
 * In case the Result contains an error message, the iterator is equal to end(), and the loop will never be executed.
 *
 * @example
 *  #include <iostream>
    #include "Result.hpp"

    Result<std::pair<int, int>> fn(int n) {
        if (n >= 0) return {{n, -n}};
        else return {"negative input"};
    }

    int main() {

        // will output "1 -1" twice
        {
            const auto result = fn(1);
            if (result.failed()) std::cout << "message: " << result.message() << '\n';
            for (const auto &[x, y]: result) {
                std::cout << x << ' ' << y << '\n';
            }
            const auto &[x, y] = result.success();
            std::cout << x << ' ' << y << '\n';
        }

        // will output "message: negative input"
        {
            auto result = fn(-1);
            if (result.failed()) std::cout << "message: " << result.message() << '\n';
            for (const auto &[x, y]: result) {
                std::cout << x << ' ' << y << '\n';
            }
            // will throw std::bad_optional_access
            // const auto &[x, y] = result.success();
            // std::cout << x << ' ' << y << '\n';
        }

        return 0;
    }
 */
template <typename T>
class Result {
    using Message = std::string_view;

    bool m_success;
    union U {
        T success;
        Message message;

        explicit U(const T& success_) noexcept: success{success_} {};
        explicit U(const Message& message_) noexcept: message{message_} {};

        ~U() {}
    } m_result;

public:

    ~Result() {
        if (m_success) m_result.success.~T();
        else m_result.message.~basic_string_view<char>();
    }

    struct Iterator {
        using Tag = std::input_iterator_tag;
        using DiffT = std::ptrdiff_t;

        Iterator(const T* ptr) noexcept: m_ptr{ptr} {};

        const T& operator *() const noexcept { return *m_ptr; }
        const T* operator ->() const noexcept { return m_ptr; }

        Iterator operator ++() noexcept { return *this = end(); }
        Iterator operator ++(int) noexcept {
            Iterator prev = *this;
            *this = end();
            return prev;
        }

        friend bool operator ==(const Iterator& iter1, const Iterator& iter2) noexcept { return iter1.m_ptr == iter2.m_ptr; }
        friend bool operator !=(const Iterator& iter1, const Iterator& iter2) noexcept { return iter1.m_ptr != iter2.m_ptr; }

    private:
        const T* m_ptr;
    };

    Result(const T& value) noexcept: m_success{true}, m_result(value) {};
    Result(const Message& message) noexcept: m_success{false}, m_result(message) {};

    Iterator begin() const noexcept { return (m_success) ? &m_result.success : nullptr; }
    static Iterator end() noexcept { return nullptr; }

    [[nodiscard]] bool failed() const noexcept { return !m_success; }

    /**
     * The only method that may throw an exception.
     *
     * @return value of the underlying type, if it contains any
     * @throws std::bad_optional_access if does not contain a value
     */
    [[nodiscard]] const T& success() const {
        if (m_success) return m_result.success;
        else throw std::bad_optional_access();
    }

    /**
     * @return provided message, if does not contain a successful value; empty string if does
     */
    [[nodiscard]] Message fail_message() const noexcept { return (m_success) ? "" : m_result.message; }

};


#endif //EMULATOR_MOS6502_RESULT_HPP
