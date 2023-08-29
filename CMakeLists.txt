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
        MOS6502.cpp
        MOS6502.hpp
)

add_executable(Emulator_MOS6502_Test
        test/test.hpp
        test/test.cpp
        test/test1.cpp
)

target_link_libraries(Emulator_MOS6502_Test gtest gtest_main)
add_test(NAME test COMMAND Emulator_MOS6502)
