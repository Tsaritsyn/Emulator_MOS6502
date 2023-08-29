cmake_minimum_required(VERSION 3.26)
project(Emulator_MOS6502)

set(CMAKE_CXX_STANDARD 23)

add_executable(Emulator_MOS6502 main.cpp
        CPU.cpp
        CPU.hpp
)
