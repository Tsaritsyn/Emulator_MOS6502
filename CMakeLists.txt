cmake_minimum_required(VERSION 3.26)
project(Emulator_MOS6502)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/8a6feabf04bec8fb125e0df0ad1195c42350725f.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(Emulator_MOS6502 main.cpp
        lib/MOS6502.cpp
        lib/MOS6502.hpp
        lib/MOS6502_definitions.hpp
        lib/MOS6502_helpers.cpp
        lib/MOS6502_helpers.hpp
)

target_include_directories(Emulator_MOS6502 PRIVATE lib)

add_executable(Emulator_MOS6502_Test
        lib/MOS6502.cpp
        lib/MOS6502.hpp
        lib/MOS6502_definitions.hpp
        lib/MOS6502_helpers.cpp
        lib/MOS6502_helpers.hpp
        test/MOS6502_TestFixture.cpp
        test/MOS6502_TestFixture.hpp
        test/Addressing.cpp
        test/Addressing.hpp
        test/MOS6502_TestLDA.cpp
        test/helpers.cpp
        test/helpers.hpp
        test/MOS6502_TestLDX.cpp
        test/MOS6502_TestLDY.cpp
        test/MOS6502_TestSTA.cpp
        test/MOS6502_TestSTX.cpp
        test/MOS6502_TestSTY.cpp
        test/MOS6502_TestRegisterTransfer.cpp
        test/MOS6502_TestRegisterPush.cpp
        test/MOS6502_TestRegisterPull.cpp
        test/MOS6502_TestLogical.cpp
        test/MOS6502_TestBIT.cpp
        test/MOS6502_TestArithmetics.cpp
        test/MOS6502_TestCompare.cpp
        test/MOS6502_TestDeincrementMemory.cpp
        test/MOS6502_TestDeincrementRegisters.cpp
        test/MOS6502_TestShift.cpp
)

target_include_directories(Emulator_MOS6502_Test PRIVATE lib test)

target_link_libraries(Emulator_MOS6502_Test gtest gtest_main)
add_test(NAME test COMMAND Emulator_MOS6502)
