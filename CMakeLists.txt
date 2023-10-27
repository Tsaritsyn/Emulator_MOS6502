cmake_minimum_required(VERSION 3.26)
project(Emulator_MOS6502)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror" )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Woverloaded-virtual -Wunused -Wduplicated-cond -Wduplicated-branches -Wnull-dereference -Wuseless-cast -Wimplicit-fallthrough" )

# allows huge std::variant
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -Wa,-mbig-obj" )

# allows usage of Dr. Memory address sanitizer on the built app
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -ggdb" )




add_executable(Emulator_MOS6502 main.cpp
        lib/MOS6502.cpp
        lib/MOS6502.hpp
        lib/MOS6502_definitions.hpp
        lib/MOS6502_helpers.cpp
        lib/MOS6502_helpers.hpp
        lib/Result.hpp
        lib/programs.cpp
        lib/programs.hpp
        ui/mainwindow.cpp
        ui/mainwindow.hpp
        ui/pageview.cpp
        ui/pageview.hpp
        ui/byteview.cpp
        ui/byteview.hpp
        lib/Operation.cpp
        lib/Operation.hpp
        lib/Error.hpp
        lib/ROM.cpp
        lib/ROM.hpp
        lib/ProcessorStatus.cpp
        lib/ProcessorStatus.hpp
        ui/helpers.cpp
        ui/helpers.hpp
        ui/definitions.hpp
        ui/registerview.cpp
        ui/registerview.hpp
        ui/constantsview.cpp
        ui/constantsview.hpp
)

target_include_directories(Emulator_MOS6502 PRIVATE lib ui)




set(QT6_INSTALL_LOCATION "D:\\Qt\\6.6.0\\mingw_64\\")
list(APPEND CMAKE_PREFIX_PATH "${QT6_INSTALL_LOCATION}")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

target_link_libraries(Emulator_MOS6502
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()






include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/8a6feabf04bec8fb125e0df0ad1195c42350725f.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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
        test/MOS6502_TestRotate.cpp
        lib/Result.hpp
        test/MOS6502_TestJMP.cpp
        test/MOS6502_TestSubroutine.cpp
        test/MOS6502_TestBranch.cpp
        test/MOS6502_TestBRK.cpp
        test/TestNOP.cpp
        test/MOS6502_TestRTI.cpp
        lib/Operation.cpp
        lib/Operation.hpp
        lib/Error.hpp
        lib/ROM.cpp
        lib/ROM.hpp
        lib/ProcessorStatus.cpp
        lib/ProcessorStatus.hpp
        test/MOS6502_test_definitions.hpp
        test/MOS6502_TestADC.cpp
        test/MOS6502_TestSBC.cpp
        test/MOS6502_TestAND.cpp
        test/MOS6502_TestORA.cpp
        test/MOS6502_TestEOR.cpp
        test/MOS6502_TestLSR.cpp
        test/MOS6502_TestASL.cpp
        test/MOS6502_TestROL.cpp
)

target_include_directories(Emulator_MOS6502_Test PRIVATE lib test)

target_link_libraries(Emulator_MOS6502_Test gtest gtest_main)
add_test(NAME test COMMAND Emulator_MOS6502)
