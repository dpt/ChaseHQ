cmake_minimum_required(VERSION 3.20)
project(ChaseHQ C)

set(CMAKE_C_STANDARD 90)
# ponytail: -Wmissing-braces/-Wmissing-field-initializers fire ~950 times on
# the nested-struct data tables' idiomatic partial-brace initializers; not
# bugs, just noise for this codebase's style. Re-enable if that changes.
# -Wunterminated-string-initialization fires on key_names (CommonData.c):
# a packed row-major char table sized to its exact content, never treated
# as a NUL-terminated string.
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wno-missing-braces -Wno-missing-field-initializers)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-unterminated-string-initialization)
  endif()
  add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
  add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)
endif()

option(CHQ_ENABLE_TEST_STAGE "Include the port-added stage 6 test level in normal progression" OFF)

# ponytail: SDL3 is only needed by the ChaseHQ app target. Configuring
# without it still gets you ChaseHQ_Tests and StretchyRenderer, which is all
# CI builds.
find_package(SDL3 QUIET)
if(NOT SDL3_FOUND)
  message(STATUS "SDL3 not found - skipping the ChaseHQ app target")
endif()

set(INCLUDE_DIRS include libraries)

set(SOURCE_FILES
        apps/sdl3/CRTShader.c
        apps/sdl3/CRTShader.h
        apps/sdl3/SDLMain.c
        include/ChaseHQ/ChaseHQ.h
        include/ZXSpectrum/Kempston.h
        include/ZXSpectrum/Keyboard.h
        include/ZXSpectrum/Macros.h
        include/ZXSpectrum/Pixels.h
        include/ZXSpectrum/Spectrum.h
        include/ZXSpectrum/Z80.h
        include/ZXSpectrum/slopay-chip.h
        libraries/ChaseHQ/Data/Bank3Data.c
        libraries/ChaseHQ/Data/Bank3Data.h
        libraries/ChaseHQ/Data/Bank7Data.c
        libraries/ChaseHQ/Data/Bank7Data.h
        libraries/ChaseHQ/Data/CommonData.c
        libraries/ChaseHQ/Data/CommonData.h
        libraries/ChaseHQ/Data/SoundSamples.c
        libraries/ChaseHQ/Data/SoundSamples.h
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
        libraries/ChaseHQ/Data/Stages.c
        libraries/ChaseHQ/Data/Stages.h
        libraries/ChaseHQ/Engine/Bank3.c
        libraries/ChaseHQ/Engine/Bank3.h
        libraries/ChaseHQ/Engine/Bank3State.h
        libraries/ChaseHQ/Engine/Bank7.c
        libraries/ChaseHQ/Engine/Bank7.h
        libraries/ChaseHQ/Engine/Create.c
        libraries/ChaseHQ/Engine/Internal.h
        libraries/ChaseHQ/Engine/Main.c
        libraries/ChaseHQ/Engine/State.h
        libraries/ChaseHQ/Engine/Tests.h
        libraries/ChaseHQ/Engine/Types.h
        libraries/ZXSpectrum/Kempston.c
        libraries/ZXSpectrum/Keyboard.c
        libraries/ZXSpectrum/Screen.c
        libraries/ZXSpectrum/Screen.h
        libraries/ZXSpectrum/Spectrum.c
        libraries/ZXSpectrum/Z80.c
        libraries/ZXSpectrum/slopay-chip.c)

if(SDL3_FOUND)
  add_executable(ChaseHQ ${SOURCE_FILES})
  target_compile_definitions(ChaseHQ PRIVATE TGE_SDL)
  target_include_directories(ChaseHQ PRIVATE ${INCLUDE_DIRS})
  if(CHQ_ENABLE_TEST_STAGE)
    target_sources(ChaseHQ PRIVATE libraries/ChaseHQ/Data/Stage6Data.c libraries/ChaseHQ/Data/Stage6Data.h)
    target_compile_definitions(ChaseHQ PRIVATE CHQ_ENABLE_TEST_STAGE)
  endif()
  target_link_libraries(ChaseHQ SDL3::SDL3)
  if(APPLE)
    target_link_libraries(ChaseHQ "-framework CoreFoundation")
    target_compile_definitions(ChaseHQ PRIVATE CHQ_CRT_SHADER=1)
  endif()
endif()

# ---- Unit tests (no SDL2 dependency) ------------------------------------

set(TEST_SOURCE_FILES
        Tests/UnitTest.c
        libraries/ZXSpectrum/Kempston.c
        libraries/ZXSpectrum/Keyboard.c
        libraries/ZXSpectrum/Screen.c
        libraries/ZXSpectrum/slopay-chip.c
        libraries/ZXSpectrum/Spectrum.c
        libraries/ZXSpectrum/Z80.c
        libraries/ChaseHQ/Engine/Bank3.c
        libraries/ChaseHQ/Engine/Bank7.c
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
        libraries/ChaseHQ/Data/Bank3Data.c
        libraries/ChaseHQ/Data/Bank3Data.h
        libraries/ChaseHQ/Data/Bank7Data.c
        libraries/ChaseHQ/Data/Bank7Data.h)

add_executable(ChaseHQ_Tests ${TEST_SOURCE_FILES})
target_compile_definitions(ChaseHQ_Tests PRIVATE CHQ_TESTS)
target_include_directories(ChaseHQ_Tests PRIVATE ${INCLUDE_DIRS})
if(CHQ_ENABLE_TEST_STAGE)
  target_sources(ChaseHQ_Tests PRIVATE libraries/ChaseHQ/Data/Stage6Data.c libraries/ChaseHQ/Data/Stage6Data.h)
  target_compile_definitions(ChaseHQ_Tests PRIVATE CHQ_ENABLE_TEST_STAGE)
endif()

# ---- Stretchy object renderer (visual debug tool, no SDL2 dependency) ------

set(STRETCHY_SOURCE_FILES ${TEST_SOURCE_FILES})
list(REMOVE_ITEM STRETCHY_SOURCE_FILES Tests/UnitTest.c)
list(APPEND STRETCHY_SOURCE_FILES Tests/RenderStretchyObject.c)

add_executable(StretchyRenderer ${STRETCHY_SOURCE_FILES})
target_compile_definitions(StretchyRenderer PRIVATE CHQ_TESTS)
target_include_directories(StretchyRenderer PRIVATE ${INCLUDE_DIRS})
if(CHQ_ENABLE_TEST_STAGE)
  target_sources(StretchyRenderer PRIVATE libraries/ChaseHQ/Data/Stage6Data.c libraries/ChaseHQ/Data/Stage6Data.h)
  target_compile_definitions(StretchyRenderer PRIVATE CHQ_ENABLE_TEST_STAGE)
endif()

# ---- Stage data generation --------------------------------------------------
# Regenerate committed .c files from their skool sources.
# Usage: cmake --build <dir> --target convert_stages
#
# WARNING: destructive, and not part of any build. convert_stage.py emits a
# skeleton that does NOT compile: MAP_LANES_* transition macros are undefined,
# lookup_map_goto's signature conflicts with its forward declaration, and the
# obj_t/overhead_span_t/helicopter tables come back as raw bytes and NULL
# TODOs. The committed Stage{2-5}Data.c files hold that manual completion, so
# running this discards it. Scaffold a new stage with it; edit an existing one
# by hand. See docs/convert-stage.md.

set(CONVERT_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/convert_stage.py)
set(SKOOL_DIR      ${CMAKE_SOURCE_DIR}/../Speccy)
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
  COMMENT "Regenerating stage 2-5 data from bank skool files - WARNING: overwrites the committed files with skeletons that do not compile; see docs/convert-stage.md"
  VERBATIM)

# ---- Formatting -------------------------------------------------------------

find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
  set(ALL_SOURCE_FILES ${SOURCE_FILES} ${TEST_SOURCE_FILES})
  list(FILTER ALL_SOURCE_FILES INCLUDE REGEX "\\.(c|h)$")
  list(REMOVE_DUPLICATES ALL_SOURCE_FILES)
  # clang-format cannot align a signature against the prologue above it, so
  # format_signatures.py runs afterwards over the .c files to lay out the
  # signatures and their \param/\return blocks.
  set(SIGNATURE_SOURCE_FILES ${ALL_SOURCE_FILES})
  list(FILTER SIGNATURE_SOURCE_FILES INCLUDE REGEX "\\.c$")
  add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
    COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/format_signatures.py --fix
            ${SIGNATURE_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting sources with clang-format"
    VERBATIM)
endif()
