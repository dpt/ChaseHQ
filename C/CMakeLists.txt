cmake_minimum_required(VERSION 4.1)
project(ChaseHQ C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")

find_package(SDL2 REQUIRED)

set(SOURCE_FILES
        Main.c
        ZXSpectrum/Kempston.c
        ZXSpectrum/Kempston.h
        ZXSpectrum/Keyboard.c
        ZXSpectrum/Keyboard.h
        ZXSpectrum/Macros.h
        ZXSpectrum/Pixels.h
        ZXSpectrum/Screen.c
        ZXSpectrum/Screen.h
        ZXSpectrum/Spectrum.c
        ZXSpectrum/Spectrum.h
        ZXSpectrum/Z80.h
        ChaseHQ/ChaseHQ.c
        ChaseHQ/ChaseHQ.h
        ChaseHQ/ChaseHQ-CommonData.c
        ChaseHQ/ChaseHQ-CommonData.h
        ChaseHQ/ChaseHQ-Create.c
        ChaseHQ/ChaseHQ-Internal.h
        ChaseHQ/ChaseHQ-SoundSamples.c
        ChaseHQ/ChaseHQ-SoundSamples.h
        ChaseHQ/ChaseHQ-Stage1Data.c
        ChaseHQ/ChaseHQ-Stage1Data.h
        ChaseHQ/ChaseHQ-Stage2Data.c
        ChaseHQ/ChaseHQ-Stage2Data.h
        ChaseHQ/ChaseHQ-Stage3Data.c
        ChaseHQ/ChaseHQ-Stage3Data.h
        ChaseHQ/ChaseHQ-Stage4Data.c
        ChaseHQ/ChaseHQ-Stage4Data.h
        ChaseHQ/ChaseHQ-Stage5Data.c
        ChaseHQ/ChaseHQ-Stage5Data.h
        ChaseHQ/ChaseHQ-Stages.c
        ChaseHQ/ChaseHQ-Stages.h)

add_executable(ChaseHQ ${SOURCE_FILES})
target_compile_definitions(ChaseHQ PRIVATE TGE_SDL)
target_include_directories(ChaseHQ PRIVATE ${SDL2_INCLUDE_DIRS} .)
target_link_libraries(ChaseHQ ${SDL2_LIBRARIES})

# ---- Unit tests (no SDL2 dependency) ------------------------------------

set(TEST_SOURCE_FILES
        Tests/TestDrawRoad.c
        ZXSpectrum/Kempston.c
        ZXSpectrum/Keyboard.c
        ZXSpectrum/Screen.c
        ZXSpectrum/Spectrum.c
        ChaseHQ/ChaseHQ.c
        ChaseHQ/ChaseHQ-CommonData.c
        ChaseHQ/ChaseHQ-Create.c
        ChaseHQ/ChaseHQ-SoundSamples.c
        ChaseHQ/ChaseHQ-Stage1Data.c
        ChaseHQ/ChaseHQ-Stage2Data.c
        ChaseHQ/ChaseHQ-Stage3Data.c
        ChaseHQ/ChaseHQ-Stage4Data.c
        ChaseHQ/ChaseHQ-Stage5Data.c
        ChaseHQ/ChaseHQ-Stages.c)

add_executable(ChaseHQ_Tests ${TEST_SOURCE_FILES})
target_compile_definitions(ChaseHQ_Tests PRIVATE CHQ_TESTS)
target_include_directories(ChaseHQ_Tests PRIVATE .)
