cmake_minimum_required(VERSION 4.1)
project(ChaseHQ C)

set(CMAKE_C_STANDARD 90)
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
        ZXSpectrum/slopay-chip.c
        ZXSpectrum/slopay-chip.h
        ZXSpectrum/Spectrum.c
        ZXSpectrum/Spectrum.h
        ZXSpectrum/Z80.h
        ChaseHQ/ChaseHQ.c
        ChaseHQ/ChaseHQ.h
        ChaseHQ/ChaseHQ-Create.c
        ChaseHQ/ChaseHQ-Internal.h
        ChaseHQ/ChaseHQ-Stages.c
        ChaseHQ/ChaseHQ-Stages.h
        ChaseHQ/Data/ChaseHQ-CommonData.c
        ChaseHQ/Data/ChaseHQ-CommonData.h
        ChaseHQ/Data/ChaseHQ-SoundSamples.c
        ChaseHQ/Data/ChaseHQ-SoundSamples.h
        ChaseHQ/Data/ChaseHQ-Stage1Data.c
        ChaseHQ/Data/ChaseHQ-Stage1Data.h
        ChaseHQ/Data/ChaseHQ-Stage2Data.c
        ChaseHQ/Data/ChaseHQ-Stage2Data.h
        ChaseHQ/Data/ChaseHQ-Stage3Data.c
        ChaseHQ/Data/ChaseHQ-Stage3Data.h
        ChaseHQ/Data/ChaseHQ-Stage4Data.c
        ChaseHQ/Data/ChaseHQ-Stage4Data.h
        ChaseHQ/Data/ChaseHQ-Stage5Data.c
        ChaseHQ/Data/ChaseHQ-Stage5Data.h)

add_executable(ChaseHQ ${SOURCE_FILES})
target_compile_definitions(ChaseHQ PRIVATE TGE_SDL)
target_include_directories(ChaseHQ PRIVATE ${SDL2_INCLUDE_DIRS} .)
target_link_libraries(ChaseHQ ${SDL2_LIBRARIES})
if(APPLE)
  target_link_libraries(ChaseHQ "-framework CoreFoundation")
endif()

# ---- Unit tests (no SDL2 dependency) ------------------------------------

set(TEST_SOURCE_FILES
        Tests/TestDrawRoad.c
        ZXSpectrum/Kempston.c
        ZXSpectrum/Keyboard.c
        ZXSpectrum/Screen.c
        ZXSpectrum/slopay-chip.c
        ZXSpectrum/Spectrum.c
        ChaseHQ/ChaseHQ.c
        ChaseHQ/ChaseHQ-Create.c
        ChaseHQ/ChaseHQ-Stages.c
        ChaseHQ/Data/ChaseHQ-CommonData.c
        ChaseHQ/Data/ChaseHQ-SoundSamples.c
        ChaseHQ/Data/ChaseHQ-Stage1Data.c
        ChaseHQ/Data/ChaseHQ-Stage2Data.c
        ChaseHQ/Data/ChaseHQ-Stage3Data.c
        ChaseHQ/Data/ChaseHQ-Stage4Data.c
        ChaseHQ/Data/ChaseHQ-Stage5Data.c)

add_executable(ChaseHQ_Tests ${TEST_SOURCE_FILES})
target_compile_definitions(ChaseHQ_Tests PRIVATE CHQ_TESTS)
target_include_directories(ChaseHQ_Tests PRIVATE .)

# ---- Stretchy object renderer (visual debug tool, no SDL2 dependency) ------

set(STRETCHY_SOURCE_FILES ${TEST_SOURCE_FILES})
list(REMOVE_ITEM STRETCHY_SOURCE_FILES Tests/TestDrawRoad.c)
list(APPEND STRETCHY_SOURCE_FILES Tests/RenderStretchyObject.c)

add_executable(StretchyRenderer ${STRETCHY_SOURCE_FILES})
target_compile_definitions(StretchyRenderer PRIVATE CHQ_TESTS)
target_include_directories(StretchyRenderer PRIVATE .)

# ---- Stage data generation --------------------------------------------------
# Regenerate committed .c files from their skool sources.
# Usage: cmake --build <dir> --target convert_stages

set(CONVERT_SCRIPT ${CMAKE_SOURCE_DIR}/../convert_stage.py)
set(SKOOL_DIR      ${CMAKE_SOURCE_DIR}/..)
set(DATA_DIR       ${CMAKE_SOURCE_DIR}/ChaseHQ/Data)

add_custom_target(convert_stages
  COMMAND sh -c "python3 ${CONVERT_SCRIPT} ${SKOOL_DIR}/ChaseHQ-128K-bank-1.skool 2\
 --obj-names NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,HUGE_ROCK,PALM_TREE,LEAVES,DOUBLE_LAMP,OBJ8\
 > ${DATA_DIR}/ChaseHQ-Stage2Data.c"
  COMMAND sh -c "python3 ${CONVERT_SCRIPT} ${SKOOL_DIR}/ChaseHQ-128K-bank-6.skool 3\
 --obj-names NONE,TUNNEL_LIGHT,OVERHEAD_BRIDGE,SHORT_POLE,TOWER_BLOCK,SPEED_LIMIT_SIGN,TELEGRAPH_POLE,OBJ7\
 > ${DATA_DIR}/ChaseHQ-Stage3Data.c"
  COMMAND sh -c "python3 ${CONVERT_SCRIPT} ${SKOOL_DIR}/ChaseHQ-128K-bank-6.skool 4\
 --obj-names NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,NEAR_COLUMN,FAR_COLUMN,PILE_OF_ROCKS,STREET_LAMP,TURN_SIGN_POINTING_LEFT,TURN_SIGN_POINTING_RIGHT\
 > ${DATA_DIR}/ChaseHQ-Stage4Data.c"
  COMMAND sh -c "python3 ${CONVERT_SCRIPT} ${SKOOL_DIR}/ChaseHQ-128K-bank-7.skool 5\
 --obj-names NONE,TUNNEL_LIGHT,OVERHEAD_BRIDGE,OBJ3,CACTUS,DOUBLE_STREET_LAMP,HUGE_ROCK,TELEGRAPH_POLE\
 > ${DATA_DIR}/ChaseHQ-Stage5Data.c"
  COMMENT "Regenerating stage 2-5 data from bank skool files"
  VERBATIM)

# ---- Formatting -------------------------------------------------------------

find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
  set(ALL_SOURCE_FILES ${SOURCE_FILES} ${TEST_SOURCE_FILES})
  list(FILTER ALL_SOURCE_FILES INCLUDE REGEX "\\.(c|h)$")
  list(REMOVE_DUPLICATES ALL_SOURCE_FILES)
  add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting sources with clang-format"
    VERBATIM)
endif()
