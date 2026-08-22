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
  add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
  add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)
endif()

option(CHQ_ENABLE_TEST_STAGE "Include the port-added stage 6 test level in normal progression" OFF)
option(CHQ_ENABLE_MASKED_VEHICLES "Draw stage vehicle sprites with real silhouette masking on near frames instead of solid rectangular blits" OFF)

# ponytail: SDL3 is only needed by the ChaseHQ app target. Configuring
# without it still gets you ChaseHQ_Tests, which is all CI builds.
if(EMSCRIPTEN)
  set(SDL3_FOUND TRUE)
else()
  find_package(SDL3 QUIET)
  if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found - skipping the ChaseHQ app target")
  endif()
endif()

# ponytail: CRT shader needs SPIR-V on Linux (Vulkan backend, no MSL).
# glslangValidator is the common Vulkan SDK / glslang-tools compiler; if it's
# missing the app still builds, just without the CRT shader, same as the
# existing APPLE-only gate.
if(UNIX AND NOT APPLE)
  find_program(GLSLANG_VALIDATOR glslangValidator)
  if(NOT GLSLANG_VALIDATOR)
    message(STATUS "glslangValidator not found - skipping the CRT shader on this platform")
  endif()
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
  if(CHQ_ENABLE_MASKED_VEHICLES)
    target_compile_definitions(ChaseHQ PRIVATE CHQ_ENABLE_MASKED_VEHICLES)
  endif()
  if(EMSCRIPTEN)
    set_target_properties(ChaseHQ PROPERTIES SUFFIX ".html")
    target_compile_options(ChaseHQ PRIVATE "-sUSE_SDL=3" "-pthread")
    target_link_options(ChaseHQ PRIVATE
      "-sUSE_SDL=3" "-pthread" "-sPTHREAD_POOL_SIZE=4"
      "-sINITIAL_MEMORY=67108864" "-sEXIT_RUNTIME=0"
      # ponytail: no AUDIO_WORKLET/WASM_WORKERS -- SDL3's Emscripten audio
      # backend is a main-thread ScriptProcessorNode and never uses them.
      "--shell-file=${CMAKE_SOURCE_DIR}/apps/sdl3/shell.html")
    set_target_properties(ChaseHQ PROPERTIES LINK_DEPENDS
      "${CMAKE_SOURCE_DIR}/apps/sdl3/shell.html")
    add_custom_command(TARGET ChaseHQ POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/apps/sdl3/coi-serviceworker.js"
        "$<TARGET_FILE_DIR:ChaseHQ>/coi-serviceworker.js")
    # ponytail: CRT shader on the web build is GLSL ES over a raw GL context
    # (WebGL1/GLES2), not SDL_GPU -- SDL3 has no stable WebGPU backend yet.
    # Compiled at runtime by the browser, same as the macOS MSL path, so no
    # extra build step is needed beyond this define.
    target_compile_definitions(ChaseHQ PRIVATE CHQ_CRT_SHADER=1 CHQ_CRT_SHADER_GLES=1)
  else()
    target_link_libraries(ChaseHQ SDL3::SDL3)
  endif()
  if(APPLE)
    target_link_libraries(ChaseHQ "-framework CoreFoundation")
    target_compile_definitions(ChaseHQ PRIVATE CHQ_CRT_SHADER=1)
  endif()
  if(UNIX AND NOT APPLE AND GLSLANG_VALIDATOR)
    set(CRT_SHADER_DIR ${CMAKE_SOURCE_DIR}/apps/sdl3/shaders)
    set(CRT_SPIRV_GEN_DIR ${CMAKE_BINARY_DIR}/generated)
    set(CRT_SPIRV_HEADER ${CRT_SPIRV_GEN_DIR}/CRTShaderSPIRV.h)
    file(MAKE_DIRECTORY ${CRT_SPIRV_GEN_DIR})
    add_custom_command(
      OUTPUT ${CRT_SPIRV_HEADER}
      COMMAND ${GLSLANG_VALIDATOR} -V ${CRT_SHADER_DIR}/crt.vert -o ${CRT_SPIRV_GEN_DIR}/crt.vert.spv
      COMMAND ${GLSLANG_VALIDATOR} -V ${CRT_SHADER_DIR}/crt.frag -o ${CRT_SPIRV_GEN_DIR}/crt.frag.spv
      COMMAND ${CMAKE_COMMAND} -DINPUT=${CRT_SPIRV_GEN_DIR}/crt.vert.spv -DOUTPUT=${CRT_SPIRV_GEN_DIR}/crt_vert_spirv.h -DVARNAME=chq_crt_vertex_spirv -P ${CMAKE_SOURCE_DIR}/cmake/EmbedSPIRV.cmake
      COMMAND ${CMAKE_COMMAND} -DINPUT=${CRT_SPIRV_GEN_DIR}/crt.frag.spv -DOUTPUT=${CRT_SPIRV_GEN_DIR}/crt_frag_spirv.h -DVARNAME=chq_crt_fragment_spirv -P ${CMAKE_SOURCE_DIR}/cmake/EmbedSPIRV.cmake
      COMMAND ${CMAKE_COMMAND} -E cat ${CRT_SPIRV_GEN_DIR}/crt_vert_spirv.h ${CRT_SPIRV_GEN_DIR}/crt_frag_spirv.h > ${CRT_SPIRV_HEADER}
      DEPENDS ${CRT_SHADER_DIR}/crt.vert ${CRT_SHADER_DIR}/crt.frag ${CMAKE_SOURCE_DIR}/cmake/EmbedSPIRV.cmake
      VERBATIM)
    add_custom_target(crt_shader_spirv DEPENDS ${CRT_SPIRV_HEADER})
    add_dependencies(ChaseHQ crt_shader_spirv)
    target_include_directories(ChaseHQ PRIVATE ${CRT_SPIRV_GEN_DIR})
    target_compile_definitions(ChaseHQ PRIVATE CHQ_CRT_SHADER=1 CHQ_CRT_SHADER_SPIRV=1)
  endif()
  if(WIN32)
    add_custom_command(TARGET ChaseHQ POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:ChaseHQ>
      VERBATIM)
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
  # slopay-chip.{c,h} are ported third-party code with their own hand-tuned
  # formatting (see .clang-format-ignore, which this target does not read).
  list(REMOVE_ITEM ALL_SOURCE_FILES
    libraries/ChaseHQ/Engine/State.h
    libraries/ChaseHQ/Engine/Bank3State.h
    libraries/ChaseHQ/Engine/Bank7State.h
    libraries/ChaseHQ/Engine/Internal.h
    libraries/ChaseHQ/Data/Stages.h
    libraries/ChaseHQ/Data/CommonData.h
    include/ZXSpectrum/Pixels.h
    libraries/ZXSpectrum/slopay-chip.c
    include/ZXSpectrum/slopay-chip.h)
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
