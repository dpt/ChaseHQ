# When the RISC OS build recurses to build host tools it enters here.
if(HOST_TOOLS_ONLY)
  add_subdirectory(apps/riscos/!ChaseHQ)
  return()
endif()

cmake_minimum_required(VERSION 3.20)
project(ChaseHQ C)

set(CMAKE_C_STANDARD 90)
# ponytail: -Wmissing-braces/-Wmissing-field-initializers fire ~950 times on
# the nested-struct data tables' idiomatic partial-brace initializers; not
# bugs, just noise for this codebase's style. Re-enable if that changes.
# -Wunterminated-string-initialization fires on key_names (CommonData.c):
# a packed row-major char table sized to its exact content, never treated
# as a NUL-terminated string.
# /wd4132: forward "static const T x[N];" declarations ahead of their
# initialized definition (the C89 idiom used throughout Data/*.c) are flagged
# as uninitialized const objects; the later definition always supplies the
# initializer, so this is noise, not a bug.
# /wd4245: negative int literals stored into "const u8" script byte-code
# tables (Bank3.c OSS_OP_* macros encode signed deltas as bytes) and the
# ~0 all-bits-set sentinel in Spectrum.c trip the signed/unsigned mismatch
# check; both are deliberate.
# /wd4295: fixed-width ASCII fields (high_score_row_t's score/stage_code/name,
# high_score_rank_suffixes, high_score_stage_codes in Bank3.c; key_names in
# Main.c) are sized exactly to their string literal's content with no room
# for a NUL -- the same packed layout -Wunterminated-string-initialization
# accepts for key_names on Clang, above.
# /wd4324: chqstate embeds a jmp_buf, which MSVC x64 aligns to 16 bytes for
# its SSE register save area; the containing struct gets padded to match.
# Nothing in our code to fix -- it's an artefact of jmp_buf's own alignment.
# /wd4996: Tests/ uses portable fopen/getenv; fopen_s/_dupenv_s are MSVC-only
# extensions with no C89 equivalent, not viable in code the other platforms
# also build.
if(MSVC)
  add_compile_options(/W4 /wd4132 /wd4245 /wd4295 /wd4324 /wd4996)
else()
  add_compile_options(-Wall -Wextra -Wno-missing-braces -Wno-missing-field-initializers)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-unterminated-string-initialization)
  endif()
  # No AddressSanitizer for RISC OS: GCCSDK has no runtime for it.
  if(NOT TARGET_RISCOS)
    add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
    add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)
  endif()
endif()

option(CHQ_ENABLE_TEST_STAGE "Include the port-added stage 6 test level in normal progression" OFF)
option(CHQ_ENABLE_MASKED_VEHICLES "Draw stage vehicle sprites with real silhouette masking on near frames instead of solid rectangular blits" OFF)

# ZXGame provides the virtual Spectrum and the front-ends. It is
# fetched from GitHub. Use a local checkout with
# -DFETCHCONTENT_SOURCE_DIR_ZXGAME=<path>.
include(FetchContent)
FetchContent_Declare(ZXGame
  GIT_REPOSITORY https://github.com/dpt/ZXGame
  GIT_TAG 0.1.0)
FetchContent_MakeAvailable(ZXGame)

set(INCLUDE_DIRS include libraries)

set(SOURCE_FILES
        apps/sdl3/Main.c
        include/ChaseHQ/ChaseHQ.h
        include/ChaseHQ/GameOps.h
        libraries/ChaseHQ/Data/CommonData.c
        libraries/ChaseHQ/Data/CommonData.h
        libraries/ChaseHQ/Data/LoadingScreen.c
        libraries/ChaseHQ/Data/LoadingScreen.h
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
        libraries/ChaseHQ/Engine/GameOps.c
        libraries/ChaseHQ/Engine/Internal.h
        libraries/ChaseHQ/Engine/Main.c
        libraries/ChaseHQ/Engine/State.h
        libraries/ChaseHQ/Engine/Tests.h
        libraries/ChaseHQ/Engine/Types.h
)

if(TARGET_RISCOS)
  add_subdirectory(apps/riscos/!ChaseHQ)
  return()
endif()

if(TARGET ZXGameSDL3)
  add_executable(ChaseHQ ${SOURCE_FILES})
  target_include_directories(ChaseHQ PRIVATE ${INCLUDE_DIRS})
  target_link_libraries(ChaseHQ ZXGameSDL3)
  zxgame_sdl3_app(ChaseHQ)
  if(CHQ_ENABLE_TEST_STAGE)
    target_sources(ChaseHQ PRIVATE libraries/ChaseHQ/Data/Stage6Data.c libraries/ChaseHQ/Data/Stage6Data.h)
    target_compile_definitions(ChaseHQ PRIVATE CHQ_ENABLE_TEST_STAGE)
  endif()
  if(CHQ_ENABLE_MASKED_VEHICLES)
    target_compile_definitions(ChaseHQ PRIVATE CHQ_ENABLE_MASKED_VEHICLES)
  endif()
endif()

if(APPLE)
  add_subdirectory(apps/macos)
endif()

# ---- Unit tests (no SDL2 dependency) ------------------------------------

set(TEST_SOURCE_FILES
        Tests/UnitTest.c
        libraries/ChaseHQ/Engine/Bank3.c
        libraries/ChaseHQ/Engine/Bank7.c
        libraries/ChaseHQ/Engine/Main.c
        libraries/ChaseHQ/Engine/Create.c
        libraries/ChaseHQ/Engine/GameOps.c
        libraries/ChaseHQ/Data/Stages.c
        libraries/ChaseHQ/Data/CommonData.c
        libraries/ChaseHQ/Data/LoadingScreen.c
        libraries/ChaseHQ/Data/SoundSamples.c
        libraries/ChaseHQ/Data/Stage1Data.c
        libraries/ChaseHQ/Data/Stage2Data.c
        libraries/ChaseHQ/Data/Stage3Data.c
        libraries/ChaseHQ/Data/Stage4Data.c
        libraries/ChaseHQ/Data/Stage5Data.c)

add_executable(ChaseHQ_Tests ${TEST_SOURCE_FILES})
target_compile_definitions(ChaseHQ_Tests PRIVATE CHQ_TESTS)
target_include_directories(ChaseHQ_Tests PRIVATE ${INCLUDE_DIRS})
target_link_libraries(ChaseHQ_Tests ZXSpectrum)
# UnitTest.c's checks are all assert() calls; NDEBUG (added by non-Debug
# CMAKE_BUILD_TYPE) would strip them and turn every test into a silent no-op.
target_compile_options(ChaseHQ_Tests PRIVATE -UNDEBUG)
if(CHQ_ENABLE_TEST_STAGE)
  target_sources(ChaseHQ_Tests PRIVATE libraries/ChaseHQ/Data/Stage6Data.c libraries/ChaseHQ/Data/Stage6Data.h)
  target_compile_definitions(ChaseHQ_Tests PRIVATE CHQ_ENABLE_TEST_STAGE)
endif()
if(CHQ_ENABLE_MASKED_VEHICLES)
  target_compile_definitions(ChaseHQ_Tests PRIVATE CHQ_ENABLE_MASKED_VEHICLES)
endif()

# ---- Title drum renderer (offline audio timing check, no SDL dependency) ---
# Renders the 128K title tune's beeper drums from the engine's own virtual
# T-state clock, with wall-clock pacing removed, and reports the inter-onset
# intervals. See the file header for the real-hardware reference figures.

set(TITLEDRUMS_SOURCE_FILES ${TEST_SOURCE_FILES})
list(REMOVE_ITEM TITLEDRUMS_SOURCE_FILES Tests/UnitTest.c)
list(APPEND TITLEDRUMS_SOURCE_FILES Tests/RenderTitleDrums.c)

add_executable(TitleDrums ${TITLEDRUMS_SOURCE_FILES})
target_compile_definitions(TitleDrums PRIVATE CHQ_TESTS)
target_include_directories(TitleDrums PRIVATE ${INCLUDE_DIRS})
target_link_libraries(TitleDrums ZXSpectrum)
if(CHQ_ENABLE_TEST_STAGE)
  target_sources(TitleDrums PRIVATE libraries/ChaseHQ/Data/Stage6Data.c libraries/ChaseHQ/Data/Stage6Data.h)
  target_compile_definitions(TitleDrums PRIVATE CHQ_ENABLE_TEST_STAGE)
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
  # These headers hand-align struct fields / macro tables against blocks of
  # "/* ... */" commentary between entries. clang-format's
  # AlignConsecutiveDeclarations/AlignConsecutiveMacros only align within a
  # single run of declarations with no intervening comment or blank line, so
  # a raw pass flattens every column to the width of one entry instead of the
  # widest in the group -- format_all.py's align_declarations.py cannot help
  # either, as it only realigns function-local variable blocks, not struct
  # fields or top-level macros. Left to hand maintenance until a script
  # exists for this case.
  list(REMOVE_ITEM ALL_SOURCE_FILES
    libraries/ChaseHQ/Engine/State.h
    libraries/ChaseHQ/Engine/Bank3State.h
    libraries/ChaseHQ/Engine/Bank7State.h
    libraries/ChaseHQ/Engine/Internal.h
    libraries/ChaseHQ/Data/Stages.h
    libraries/ChaseHQ/Data/CommonData.h)
  # clang-format cannot align a signature against the prologue above it, or
  # a declaration block against its "(was X)" comments, so format_all.py
  # runs afterwards over the .c files: it reflows signatures and \param
  # blocks, realigns declaration blocks, and checks prologue structure
  # (check_prologues.py has no --fix -- a missing \param needs a human).
  set(SIGNATURE_SOURCE_FILES ${ALL_SOURCE_FILES})
  list(FILTER SIGNATURE_SOURCE_FILES INCLUDE REGEX "\\.c$")
  add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
    COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/format_all.py --fix
            ${SIGNATURE_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting sources with clang-format"
    VERBATIM)
endif()
