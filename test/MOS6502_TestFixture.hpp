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

    static Writer writer_to(Byte &loc) noexcept;

    static Reader reader_from(const Byte &loc) noexcept;

protected:
    void reset_registers() noexcept;

    [[nodiscard]] std::expected<void, ROM::StackOverride> set_word(Word address, Word value) noexcept;

    Writer writer_to_immediate() noexcept;
    Writer writer_to_zero_page(Byte address) noexcept;
    Writer writer_to_zero_page_X(Byte address) noexcept;
    Writer writer_to_zero_page_Y(Byte address) noexcept;
    Writer writer_to_absolute(Word address) noexcept;
    Writer writer_to_absolute_X(Word address) noexcept;
    Writer writer_to_absolute_Y(Word address) noexcept;
    Writer writer_to_indirect_X(Byte tableAddress, Word targetAddress) noexcept;
    Writer writer_to_indirect_Y(Byte tableAddress, Word targetAddress) noexcept;
};


struct ExecutionParameters {
    Word size;
    size_t duration;

    [[nodiscard]] constexpr static ExecutionParameters binary_immediate() noexcept                        { return {.size = 2, .duration = 2}; }
    [[nodiscard]] constexpr static ExecutionParameters binary_zero_page() noexcept                        { return {.size = 2, .duration = 3}; }
    [[nodiscard]] constexpr static ExecutionParameters binary_zero_page_indexed() noexcept                { return {.size = 2, .duration = 4}; }
    [[nodiscard]] constexpr static ExecutionParameters binary_absolute() noexcept                         { return {.size = 3, .duration = 4}; }
    [[nodiscard]] constexpr static ExecutionParameters binary_absolute_indexed(bool pageCrossed) noexcept { return {.size = 3, .duration = 4 + static_cast<size_t>(pageCrossed)}; }
    [[nodiscard]] constexpr static ExecutionParameters binary_indirect_X() noexcept                       { return {.size = 2, .duration = 6}; }
    [[nodiscard]] constexpr static ExecutionParameters binary_indirect_Y(bool pageCrossed) noexcept       { return {.size = 2, .duration = 5 + static_cast<size_t>(pageCrossed)}; }

    [[nodiscard]] constexpr static ExecutionParameters unary_accumulator() noexcept       { return {.size = 1, .duration = 2}; }
    [[nodiscard]] constexpr static ExecutionParameters unary_zero_page() noexcept         { return {.size = 2, .duration = 5}; }
    [[nodiscard]] constexpr static ExecutionParameters unary_zero_page_indexed() noexcept { return {.size = 2, .duration = 6}; }
    [[nodiscard]] constexpr static ExecutionParameters unary_absolute() noexcept          { return {.size = 3, .duration = 6}; }
    [[nodiscard]] constexpr static ExecutionParameters unary_absolute_indexed() noexcept  { return {.size = 3, .duration = 7}; }

    [[nodiscard]] constexpr static ExecutionParameters implied() noexcept { return {.size = 1, .duration = 2}; }

    [[nodiscard]] constexpr static ExecutionParameters transfer_immediate() noexcept                        { return binary_immediate(); };
    [[nodiscard]] constexpr static ExecutionParameters transfer_zero_page() noexcept                        { return binary_zero_page(); };
    [[nodiscard]] constexpr static ExecutionParameters transfer_zero_page_indexed() noexcept                { return binary_zero_page_indexed(); };
    [[nodiscard]] constexpr static ExecutionParameters transfer_absolute() noexcept                         { return binary_absolute(); };
    [[nodiscard]] constexpr static ExecutionParameters transfer_absolute_indexed(bool pageCrossed) noexcept { return binary_absolute_indexed(pageCrossed); };
    [[nodiscard]] constexpr static ExecutionParameters transfer_indirect_X() noexcept                       { return binary_indirect_X(); };
    [[nodiscard]] constexpr static ExecutionParameters transfer_indirect_Y(bool pageCrossed) noexcept       { return binary_indirect_Y(pageCrossed); };
};


struct BinaryOpParameters {
    friend std::ostream &operator<<(std::ostream &os, const BinaryOpParameters &parameters);

    Byte arg1, arg2;
    bool carry;
    Byte result;
    std::vector<Flag> flagsSet;
};

class MOS6502_TestFixture_BinaryOp: public ::testing::TestWithParam<BinaryOpParameters>, public MOS6502_TextFixture {
    void SetUp() override;

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
    void SetUp() override;

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
    void SetUp() override;

public:
    void test_transfer(OpCode opcode,
                    Byte arg,
                    const ExecutionParameters &execParams,
                    const Writer &argWriter,
                    const Reader &resultReader);

    void test_transfer_to_SP(OpCode opcode,
                       Byte arg,
                       const ExecutionParameters &execParams,
                       const Writer &argWriter,
                       const Reader &resultReader);
};

INSTANTIATE_TEST_SUITE_P(AllTests, MOS6502_TestFixture_Transfer, ::testing::Values(0, 1, 10, 100, (Byte)-10, 255));


#endif //EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
