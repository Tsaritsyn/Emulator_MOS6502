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
        test/MOS6502_TestFixture.hpp
        test/MOS6502_TestADC.cpp
        test/MOS6502_TestFixture.cpp
        test/Addressing.hpp
        test/Instruction.hpp
        test/Instruction.cpp
        test/Addressing.cpp
        test/MOS6502_TestSBC.cpp
        test/MOS6502_TestASL.cpp
        test/MOS6502_TestAND.cpp
        test/MOS6502_TestBCC.cpp
)

target_include_directories(Emulator_MOS6502_Test PRIVATE lib test)

target_link_libraries(Emulator_MOS6502_Test gtest gtest_main)
add_test(NAME test COMMAND Emulator_MOS6502)
