# Native macOS (Cocoa/OpenGL) front-end. The host itself is ZXGameMacOS in
# ZXGame; this only supplies main(), the xibs and the bundle.
#
# ponytail: no app icon yet, add AppIcon.icns to RESOURCES when there is art.

set(MACOS_SOURCES ${SOURCE_FILES})
list(REMOVE_ITEM MACOS_SOURCES apps/sdl3/Main.c)
list(TRANSFORM MACOS_SOURCES PREPEND ${CMAKE_SOURCE_DIR}/)

set(XIBS en.lproj/MainMenu en.lproj/ZXGameWindow)
set(RESOURCES en.lproj/Credits.rtf)

add_executable(ChaseHQApp MACOSX_BUNDLE
  main.m
  ${MACOS_SOURCES}
  ${RESOURCES})
target_include_directories(ChaseHQApp PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/libraries)
target_link_libraries(ChaseHQApp ZXGameMacOS)

set_target_properties(ChaseHQApp PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "$<CONFIG>"
  OUTPUT_NAME "ChaseHQ"
  MACOSX_BUNDLE_BUNDLE_NAME "Chase H.Q."
  MACOSX_BUNDLE_GUI_IDENTIFIER "uk.co.davespace.chasehq"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
  MACOSX_BUNDLE_COPYRIGHT "Copyright 2023-2026 David Thomas")

if(CHQ_ENABLE_TEST_STAGE)
  target_sources(ChaseHQApp PRIVATE
    ${CMAKE_SOURCE_DIR}/libraries/ChaseHQ/Data/Stage6Data.c)
  target_compile_definitions(ChaseHQApp PRIVATE CHQ_ENABLE_TEST_STAGE)
endif()
if(CHQ_ENABLE_MASKED_VEHICLES)
  target_compile_definitions(ChaseHQApp PRIVATE CHQ_ENABLE_MASKED_VEHICLES)
endif()

set_source_files_properties(${RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

find_program(IBTOOL ibtool REQUIRED)
add_custom_command(TARGET ChaseHQApp PRE_BUILD
  COMMAND mkdir -p ./\$<CONFIG>/ChaseHQ.app/Contents/Resources)
foreach(xib ${XIBS})
  add_custom_command(TARGET ChaseHQApp POST_BUILD
    COMMAND ${IBTOOL} --errors --warnings --notices
      --output-format human-readable-text
      --compile ./\$<CONFIG>/ChaseHQ.app/Contents/Resources/${xib}.nib
      ${CMAKE_CURRENT_SOURCE_DIR}/${xib}.xib
    COMMENT "Compiling ${xib}.xib")
endforeach()
