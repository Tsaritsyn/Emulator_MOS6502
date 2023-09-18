//
// Created by Mikhail on 14/09/2023.
//

#ifndef EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
#define EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP

#include <gtest/gtest.h>

#include "MOS6502.hpp"
#include "Addressing.hpp"
#include "MOS6502_helpers.hpp"



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
    std::optional<Address> prepare_and_execute(OpCode opcode, std::optional<Byte> value, const Addressing& addressing);

    void check_register_loading(Register reg, Byte expectedValue, Word expectedPCShift, size_t expectedDuration, const std::string& testID);
    std::pair<OpCode, size_t> loading_parameters(Register reg, const Addressing& addressing);

    std::pair<OpCode, size_t> storage_parameters(Register reg, const Addressing& addressing);
    void check_register_storage(Word address, Byte expectedValue, Word expectedPCShift, size_t expectedDuration, const std::string& testID);

    Byte& stack(Byte address);

public:
    void test_loading(Register reg, Byte value, const Addressing& addressing);

    void test_storage(Register reg, Byte value, const Addressing& addressing);

    void test_transfer(Register from, Register to, Byte value);

    void test_push_to_stack(Register reg, Byte value);
};


#endif //EMULATOR_MOS6502_MOS6502_TESTFIXTURE_HPP
