//
// Created by Mikhail on 14/09/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
#define EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP

#include <concepts>
#include <functional>

#include <gtest/gtest.h>

#include "MOS6502.hpp"
#include "Addressing.hpp"
#include "MOS6502_helpers.hpp"
#include "helpers.hpp"



using namespace Emulator;

struct MOS6502_TestFixture:  public ::testing::Test, public MOS6502 {
private:

    void reset() noexcept override;
    void write_word(Word word, Word address) noexcept;
    /**
     * Writes all intermediate values (if any) into memory necessary for reading/writing of the target value (if any).
     *
     * @return reference to the memory or register where the target value must be put to (or where to read it from), if any.
     */
    std::optional<Address> prepare_memory(const Addressing& addressing);
    std::optional<Address> prepare_and_execute(OpCode opcode, std::optional<Byte> value, std::optional<Addressing> addressing = std::nullopt);

    void check_register(Register reg,
                        Byte expectedValue,
                        Word expectedPCShift,
                        size_t expectedDuration,
                        const std::string& testID,
                        std::optional<ProcessorStatus> expectedFlags = std::nullopt);

    void check_memory(Word address,
                      Byte expectedValue,
                      Word expectedPCShift,
                      size_t expectedDuration,
                      const std::string &testID,
                      ProcessorStatus expectedFlags = 0);

    Byte& stack(Byte address);

public:
    enum struct ArithmeticOperation {ADD, SUB};
    enum struct ChangeByOne {INCREMENT, DECREMENT};

    void test_loading(Register reg, Byte value, const Addressing& addressing);

    void test_storage(Register reg, Byte value, const Addressing& addressing);

    void test_transfer(Register from, Register to, Byte value);

    void test_push_to_stack(Register reg, Byte value);

    void test_pull_from_stack(Register reg, Byte value);

    void test_logical(LogicalOperation operation, Byte value, Byte mem, const Addressing &addressing);

    void test_bit_test(Byte value, Byte mem, const Addressing& addressing);

    void test_arithmetics(ArithmeticOperation operation, Byte value, Byte mem, bool carry, const Addressing& addressing);

    void test_compare_register(Register reg, Byte registerValue, const Addressing& addressing, Byte memoryValue);

    void test_deincrement_memory(ChangeByOne operation, Byte value, const Addressing &addressing);

    void test_deincrement_register(ChangeByOne operation, Byte value, Register reg);

    void test_shift_left(Byte value, const Addressing& addressing);

    void test_shift_right(Byte value, const Addressing& addressing);

    void test_rotate_left(Byte value, const Addressing& addressing);

    void test_rotate_right(Byte value, const Addressing& addressing);

    void test_jump(const Addressing& addressing);

    void test_jump_to_subroutine(Word address);

    void test_return_from_subroutine();

    void test_branch(Flag flag, bool value, bool targetValue);

    void test_force_interrupt();

    void test_nop();

    void test_return_from_interrupt();
};


#endif //EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
