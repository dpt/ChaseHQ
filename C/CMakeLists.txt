cmake_minimum_required(VERSION 4.1)
project(ChaseHQ C)

set(CMAKE_C_STANDARD 90)
# ponytail: -Wmissing-braces/-Wmissing-field-initializers fire ~950 times on
# the nested-struct data tables' idiomatic partial-brace initializers; not
# bugs, just noise for this codebase's style. Re-enable if that changes.
# -Wunterminated-string-initialization fires on key_names (CommonData.c):
# a packed row-major char table sized to its exact content, never treated
# as a NUL-terminated string.
add_compile_options(-Wall -Wextra -Wno-missing-braces -Wno-missing-field-initializers -Wno-unterminated-string-initialization)
add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)

find_package(SDL3 REQUIRED)

set(INCLUDE_DIRS include libraries)

set(SOURCE_FILES
        SDLMain.c
        include/ZXSpectrum/Kempston.h
        include/ZXSpectrum/Keyboard.h
        include/ZXSpectrum/Macros.h
        include/ZXSpectrum/Pixels.h
        include/ZXSpectrum/slopay-chip.h
        include/ZXSpectrum/Spectrum.h
        include/ZXSpectrum/Z80.h
        include/ChaseHQ/ChaseHQ.h
        libraries/ZXSpectrum/Kempston.c
        libraries/ZXSpectrum/Keyboard.c
        libraries/ZXSpectrum/Screen.c
        libraries/ZXSpectrum/Screen.h
        libraries/ZXSpectrum/slopay-chip.c
        libraries/ZXSpectrum/Spectrum.c
        libraries/ZXSpectrum/Z80.c
        libraries/ChaseHQ/Engine/Bank3.c
        libraries/ChaseHQ/Engine/Main.c
        libraries/ChaseHQ/Engine/Create.c
        libraries/ChaseHQ/Engine/Internal.h
        libraries/ChaseHQ/Engine/State.h
        libraries/ChaseHQ/Engine/Tests.h
        libraries/ChaseHQ/Data/CommonData.c
        libraries/ChaseHQ/Data/CommonData.h
        libraries/ChaseHQ/Data/SoundSamples.c
        libraries/ChaseHQ/Data/SoundSamples.h
        libraries/ChaseHQ/Data/Stages.c
        libraries/ChaseHQ/Data/Stages.h
        libraries/ChaseHQ/Data/Stage1Data.c
        libraries/ChaseHQ/Data/Stage1Data.h
        libraries/ChaseHQ/Data/Stage2Data.c
        libraries/ChaseHQ/Data/Stage2Data.h
        libraries/ChaseHQ/Data/Stage3Data.c
        libraries/ChaseHQ/Data/Stage3Data.h
        libraries/ChaseHQ/Data/Stage4Data.c
        libraries/ChaseHQ/Data/Stage4Data.h
        libraries/ChaseHQ/Data/Stage5Data.c
        libraries/ChaseHQ/Data/Stage5Data.h
        libraries/ChaseHQ/Data/Stage6Data.c
        libraries/ChaseHQ/Data/Stage6Data.h
        libraries/ChaseHQ/Data/TitleScreenData.c
        libraries/ChaseHQ/Data/TitleScreenData.h
        libraries/ChaseHQ/Data/Bank3Data.c
        libraries/ChaseHQ/Data/Bank3Data.h
        libraries/ChaseHQ/Engine/Types.h
        libraries/ChaseHQ/Engine/Bank3.h
        libraries/ChaseHQ/Engine/Bank3State.h)

add_executable(ChaseHQ ${SOURCE_FILES})
target_compile_definitions(ChaseHQ PRIVATE TGE_SDL)
target_include_directories(ChaseHQ PRIVATE ${INCLUDE_DIRS})
target_link_libraries(ChaseHQ SDL3::SDL3)
if(APPLE)
  target_link_libraries(ChaseHQ "-framework CoreFoundation")
endif()

# ---- Unit tests (no SDL2 dependency) ------------------------------------

set(TEST_SOURCE_FILES
        Tests/TestDrawRoad.c
        libraries/ZXSpectrum/Kempston.c
        libraries/ZXSpectrum/Keyboard.c
        libraries/ZXSpectrum/Screen.c
        libraries/ZXSpectrum/slopay-chip.c
        libraries/ZXSpectrum/Spectrum.c
        libraries/ZXSpectrum/Z80.c
        libraries/ChaseHQ/Engine/Bank3.c
        libraries/ChaseHQ/Engine/Main.c
        libraries/ChaseHQ/Engine/Create.c
        libraries/ChaseHQ/Data/Stages.c
        libraries/ChaseHQ/Data/CommonData.c
        libraries/ChaseHQ/Data/SoundSamples.c
        libraries/ChaseHQ/Data/Stage1Data.c
        libraries/ChaseHQ/Data/Stage2Data.c
        libraries/ChaseHQ/Data/Stage3Data.c
        libraries/ChaseHQ/Data/Stage4Data.c
        libraries/ChaseHQ/Data/Stage5Data.c
        libraries/ChaseHQ/Data/Stage6Data.c
        libraries/ChaseHQ/Data/TitleScreenData.c)

add_executable(ChaseHQ_Tests ${TEST_SOURCE_FILES})
target_compile_definitions(ChaseHQ_Tests PRIVATE CHQ_TESTS)
target_include_directories(ChaseHQ_Tests PRIVATE ${INCLUDE_DIRS})

# ---- Stretchy object renderer (visual debug tool, no SDL2 dependency) ------

set(STRETCHY_SOURCE_FILES ${TEST_SOURCE_FILES})
list(REMOVE_ITEM STRETCHY_SOURCE_FILES Tests/TestDrawRoad.c)
list(APPEND STRETCHY_SOURCE_FILES Tests/RenderStretchyObject.c)

add_executable(StretchyRenderer ${STRETCHY_SOURCE_FILES})
target_compile_definitions(StretchyRenderer PRIVATE CHQ_TESTS)
target_include_directories(StretchyRenderer PRIVATE ${INCLUDE_DIRS})

# ---- Stage data generation --------------------------------------------------
# Regenerate committed .c files from their skool sources.
# Usage: cmake --build <dir> --target convert_stages

set(CONVERT_SCRIPT ${CMAKE_SOURCE_DIR}/../convert_stage.py)
set(SKOOL_DIR      ${CMAKE_SOURCE_DIR}/..)
set(DATA_DIR       ${CMAKE_SOURCE_DIR}/libraries/ChaseHQ/Data)

add_custom_target(convert_stages
  COMMAND sh -c "python3 ${CONVERT_SCRIPT} ${SKOOL_DIR}/ChaseHQ-128K-bank-1.skool 2\
 --obj-names NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,HUGE_ROCK,PALM_TREE,LEAVES,DOUBLE_LAMP,OBJ8\
 > ${DATA_DIR}/Stage2Data.c"
  COMMAND sh -c "python3 ${CONVERT_SCRIPT} ${SKOOL_DIR}/ChaseHQ-128K-bank-6.skool 3\
 --obj-names NONE,TUNNEL_LIGHT,OVERHEAD_BRIDGE,SHORT_POLE,TOWER_BLOCK,SPEED_LIMIT_SIGN,TELEGRAPH_POLE,OBJ7\
 > ${DATA_DIR}/Stage3Data.c"
  COMMAND sh -c "python3 ${CONVERT_SCRIPT} ${SKOOL_DIR}/ChaseHQ-128K-bank-6.skool 4\
 --obj-names NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,NEAR_COLUMN,FAR_COLUMN,PILE_OF_ROCKS,STREET_LAMP,TURN_SIGN_POINTING_LEFT,TURN_SIGN_POINTING_RIGHT\
 > ${DATA_DIR}/Stage4Data.c"
  COMMAND sh -c "python3 ${CONVERT_SCRIPT} ${SKOOL_DIR}/ChaseHQ-128K-bank-7.skool 5\
 --obj-names NONE,TUNNEL_LIGHT,OVERHEAD_BRIDGE,OBJ3,CACTUS,DOUBLE_STREET_LAMP,HUGE_ROCK,TELEGRAPH_POLE\
 > ${DATA_DIR}/Stage5Data.c"
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
