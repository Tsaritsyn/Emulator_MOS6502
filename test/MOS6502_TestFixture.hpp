//
// Created by Mikhail on 14/09/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
#define EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP

#include <gtest/gtest.h>
#include <ostream>
#include <concepts>

#include "MOS6502.hpp"
#include "helpers.hpp"



using namespace Emulator;


class MOS6502_TextFixture: public MOS6502 {
public:
    using Writer = std::function<ROM::WriteResult(Byte)>;
    using Reader = std::function<Byte(void)>;
    using WriteResult = std::expected<void, ROM::StackOverride>;

    static Writer writer_to(Byte &loc) noexcept;

    static Reader reader_from(const Byte &loc) noexcept;

    [[nodiscard]] WriteResult set_operation_arg(Byte byte) noexcept;
    [[nodiscard]] WriteResult set_operation_arg(Word word) noexcept;

    /// number of bytes currently in stack
    [[nodiscard]] Byte stack_size() const noexcept;

    /// 0 corresponds to the topmost item in stack
    [[nodiscard]] Byte stack_item(Byte index) const noexcept;

    [[nodiscard]] bool is_stack_full() const noexcept;

protected:
    void setup() noexcept;

    void reset_registers() noexcept;

    [[nodiscard]] WriteResult set_word(Word address, Word value) noexcept;

    Writer writer_to_immediate() noexcept;
    Writer writer_to_zero_page(Byte address) noexcept;
    Writer writer_to_zero_page_X(Byte address) noexcept;
    Writer writer_to_zero_page_Y(Byte address) noexcept;
    Writer writer_to_absolute(Word address) noexcept;
    Writer writer_to_absolute_X(Word address) noexcept;
    Writer writer_to_absolute_Y(Word address) noexcept;
    Writer writer_to_indirect_X(Byte tableAddress, Word targetAddress) noexcept;
    Writer writer_to_indirect_Y(Byte tableAddress, Word targetAddress) noexcept;

    Reader reader_from_zero_page(Byte address) noexcept;
    Reader reader_from_zero_page_X(Byte address) noexcept;
    Reader reader_from_zero_page_Y(Byte address) noexcept;
    Reader reader_from_absolute(Word address) noexcept;
    Reader reader_from_absolute_X(Word address) noexcept;
    Reader reader_from_absolute_Y(Word address) noexcept;
    Reader reader_from_indirect_X(Word targetAddress) noexcept;
    Reader reader_from_indirect_Y(Word targetAddress) noexcept;
};


struct ExecutionParameters { Word size; size_t duration; };


constexpr inline ExecutionParameters BINARY_IMMEDIATE_PARAMS                     = {.size = 2, .duration = 2};
constexpr inline ExecutionParameters BINARY_ZERO_PAGE_PARAMS                     = {.size = 2, .duration = 3};
constexpr inline ExecutionParameters BINARY_ZERO_PAGE_X_PARAMS                   = {.size = 2, .duration = 4};
constexpr inline ExecutionParameters BINARY_ABSOLUTE_PARAMS                      = {.size = 3, .duration = 4};
constexpr inline ExecutionParameters BINARY_ABSOLUTE_X_NO_PAGE_CROSSING_PARAMS   = {.size = 3, .duration = 4};
constexpr inline ExecutionParameters BINARY_ABSOLUTE_X_PAGE_CROSSING_PARAMS      = {.size = 3, .duration = 5};
constexpr inline ExecutionParameters BINARY_INDIRECT_X_PARAMS                    = {.size = 2, .duration = 6};
constexpr inline ExecutionParameters BINARY_INDIRECT_Y_NO_PAGE_CROSSING_PARAMS   = {.size = 2, .duration = 5};
constexpr inline ExecutionParameters BINARY_INDIRECT_Y_PAGE_CROSSING_PARAMS      = {.size = 2, .duration = 6};

constexpr inline ExecutionParameters UNARY_ACCUMULATOR_PARAMS                    = {.size = 1, .duration = 2};
constexpr inline ExecutionParameters UNARY_ZERO_PAGE_PARAMS                      = {.size = 2, .duration = 5};
constexpr inline ExecutionParameters UNARY_ZERO_PAGE_X_PARAMS                    = {.size = 2, .duration = 6};
constexpr inline ExecutionParameters UNARY_ABSOLUTE_PARAMS                       = {.size = 3, .duration = 6};
constexpr inline ExecutionParameters UNARY_ABSOLUTE_X_PARAMS                     = {.size = 3, .duration = 7};

constexpr inline ExecutionParameters IMPLIED_PARAMS                              = {.size = 1, .duration = 2};

constexpr inline ExecutionParameters TRANSFER_IMMEDIATE_PARAMS                   = {.size = 2, .duration = 2};
constexpr inline ExecutionParameters TRANSFER_ZERO_PAGE_PARAMS                   = {.size = 2, .duration = 3};
constexpr inline ExecutionParameters TRANSFER_ZERO_PAGE_X_PARAMS                 = {.size = 2, .duration = 4};
constexpr inline ExecutionParameters TRANSFER_ABSOLUTE_PARAMS                    = {.size = 3, .duration = 4};
constexpr inline ExecutionParameters TRANSFER_ABSOLUTE_X_NO_PAGE_CROSSING_PARAMS = {.size = 3, .duration = 4};
constexpr inline ExecutionParameters TRANSFER_ABSOLUTE_X_PAGE_CROSSING_PARAMS    = {.size = 3, .duration = 5};
constexpr inline ExecutionParameters TRANSFER_INDIRECT_X_PARAMS                  = {.size = 2, .duration = 6};
constexpr inline ExecutionParameters TRANSFER_INDIRECT_Y_NO_PAGE_CROSSING_PARAMS = {.size = 2, .duration = 5};
constexpr inline ExecutionParameters TRANSFER_INDIRECT_Y_PAGE_CROSSING_PARAMS    = {.size = 2, .duration = 6};

constexpr inline ExecutionParameters STACK_PUSH_PARAMS                           = {.size = 1, .duration = 3};
constexpr inline ExecutionParameters STACK_PULL_PARAMS                           = {.size = 1, .duration = 4};

constexpr inline ExecutionParameters COMPARE_IMMEDIATE_PARAMS                    = {.size = 2, .duration = 2};
constexpr inline ExecutionParameters COMPARE_ZERO_PAGE_PARAMS                    = {.size = 2, .duration = 3};
constexpr inline ExecutionParameters COMPARE_ZERO_PAGE_X_PARAMS                  = {.size = 2, .duration = 4};
constexpr inline ExecutionParameters COMPARE_ABSOLUTE_PARAMS                     = {.size = 3, .duration = 4};
constexpr inline ExecutionParameters COMPARE_ABSOLUTE_X_NO_PAGE_CROSSING_PARAMS  = {.size = 3, .duration = 4};
constexpr inline ExecutionParameters COMPARE_ABSOLUTE_X_PAGE_CROSSING_PARAMS     = {.size = 3, .duration = 5};
constexpr inline ExecutionParameters COMPARE_INDIRECT_X_PARAMS                   = {.size = 2, .duration = 6};
constexpr inline ExecutionParameters COMPARE_INDIRECT_Y_NO_PAGE_CROSSING_PARAMS  = {.size = 2, .duration = 5};
constexpr inline ExecutionParameters COMPARE_INDIRECT_Y_PAGE_CROSSING_PARAMS     = {.size = 2, .duration = 6};


struct BinaryOpParameters {
    friend std::ostream &operator<<(std::ostream &os, const BinaryOpParameters &parameters);

    Byte arg1, arg2;
    bool carry;
    Byte result;
    std::vector<Flag> flagsSet;
};

class MOS6502_TestFixture_BinaryOp: public ::testing::TestWithParam<BinaryOpParameters>, public MOS6502_TextFixture {
    void SetUp() override { setup(); };

public:

    void
    test_binary(OpCode opcode,
                const BinaryOpParameters &params,
                const ExecutionParameters &execParams,
                const Writer &arg1Writer,
                const Writer &arg2Writer,
                const Reader &resultReader);
};

class MOS6502_TestFixture_ADC: public MOS6502_TestFixture_BinaryOp {};
class MOS6502_TestFixture_SBC: public MOS6502_TestFixture_BinaryOp {};
class MOS6502_TestFixture_AND: public MOS6502_TestFixture_BinaryOp {};
class MOS6502_TestFixture_ORA: public MOS6502_TestFixture_BinaryOp {};
class MOS6502_TestFixture_EOR: public MOS6502_TestFixture_BinaryOp {};



struct UnaryOpParameters {
    friend std::ostream &operator<<(std::ostream &os, const UnaryOpParameters &parameters);

    Byte arg;
    bool carry;
    Byte result;
    std::vector<Flag> flagsSet;
};

class MOS6502_TestFixture_UnaryOp: public ::testing::TestWithParam<UnaryOpParameters>, public MOS6502_TextFixture {
    void SetUp() override { setup(); };

public:
    void test_unary(OpCode opcode,
                    const UnaryOpParameters &params,
                    const ExecutionParameters &execParams,
                    const Writer &argWriter,
                    const Reader &resultReader);
};

class MOS6502_TestFixture_LSR: public MOS6502_TestFixture_UnaryOp {};
class MOS6502_TestFixture_ASL: public MOS6502_TestFixture_UnaryOp {};
class MOS6502_TestFixture_ROL: public MOS6502_TestFixture_UnaryOp {};
class MOS6502_TestFixture_ROR: public MOS6502_TestFixture_UnaryOp {};

class MOS6502_TestFixture_Increment: public MOS6502_TestFixture_UnaryOp {};
class MOS6502_TestFixture_Decrement: public MOS6502_TestFixture_UnaryOp {};



class MOS6502_TestFixture_Transfer: public ::testing::TestWithParam<Byte>, public MOS6502_TextFixture {
    void SetUp() override { setup(); };

public:
    void test_transfer_with_flags(OpCode opcode,
                                  Byte arg,
                                  const ExecutionParameters &execParams,
                                  const Writer &argWriter,
                                  const Reader &resultReader);

    void test_transfer_without_flags(OpCode opcode,
                                     Byte arg,
                                     const ExecutionParameters &execParams,
                                     const Writer &argWriter,
                                     const Reader &resultReader);

    void test_push(OpCode opcode,
                   Byte arg,
                   const ExecutionParameters &execParams,
                   const Writer &argWriter);

    void test_pull(OpCode opcode,
                   Byte arg,
                   const ExecutionParameters &execParams,
                   const Reader &resultReader);
};



struct CompareParameters {
    friend std::ostream &operator<<(std::ostream &os, const CompareParameters &parameters);

    Byte arg1, arg2;
    std::vector<Flag> flagsSet;
};

class MOS6502_TestFixture_Compare: public ::testing::TestWithParam<CompareParameters>, public MOS6502_TextFixture {
    void SetUp() override { setup(); };

public:
    void test_compare(OpCode opcode,
                      const CompareParameters &params,
                      const ExecutionParameters &execParams,
                      const Writer &arg1Writer,
                      const Writer &arg2Writer);
};

class MOS6502_TestFixture_CMP: public MOS6502_TestFixture_Compare {};
class MOS6502_TestFixture_CPX: public MOS6502_TestFixture_Compare {};
class MOS6502_TestFixture_CPY: public MOS6502_TestFixture_Compare {};


struct BranchParameters {
    friend std::ostream &operator<<(std::ostream &os, const BranchParameters &parameters);

    Word initialPC;
    char offset;
    Word finalPC;
    bool doBranch;
    size_t duration;
};

constexpr size_t BRANCH_DURATION_NO_JUMP = 2;
constexpr size_t BRANCH_DURATION_JUMP_SAME_PAGE = 3;
constexpr size_t BRANCH_DURATION_JUMP_OTHER_PAGE = 4;

class MOS6502_TestFixture_Branch: public ::testing::TestWithParam<BranchParameters>, public MOS6502_TextFixture {
    void SetUp() override { setup(); };

public:
    /// here execParams.size is the resulting PC after the branch
    void test_branch(OpCode opcode,
                      const BranchParameters &params,
                      Flag targetFlag,
                      bool targetValue);
};


#endif //EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
