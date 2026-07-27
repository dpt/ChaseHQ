# Rebuilding ZX Spectrum _Chase H.Q._ in C

Reimplementation by David Thomas, 2023-2026

<p align="center">
  <img src="./static-images/move-we-must.png" alt="Recreated Chase H.Q."/>
</p>

This directory holds a mostly hand-written, human-readable C reimplementation of the ZX Spectrum _Chase H.Q._ game engine. It's not a decompiler dump - it's intended to be read and learned from. Every function is translated from the [SkoolKit disassembly](../Speccy/README.md), with the original Z80 addresses kept in the comments so the two can be read side by side, compared (... debugged).

See the [sister project README](../Speccy/README.md) for the disassembly this is derived from.

## Status

- Engine logic translated across `Main.c` (~18k lines of fun), `Bank3.c` (~3.7k lines of 128K title-screen/tune code) and `Bank7.c` (~1.4k lines of end screen animation).
- All five stages have the original per-stage road, object and sprite data (`Stage1Data.c`–`Stage6Data.c`).
  - There's a sixth stage in the mix to help decode the magic.
- The game runs and plays through the stages, with known rough edges: title screen music/animation timing, perp-car jitter, an incomplete 128K options menu and gaps in the sound-effect subsystem — see `TODO.md` for the full list.

## Building

CMake is the canonical build system. SDL3 is required.

``` sh
cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
./cmake-build-debug/ChaseHQ
```

Unit tests:

``` sh
cmake --build cmake-build-debug --target ChaseHQ_Tests
./cmake-build-debug/ChaseHQ_Tests
```

Formatting (clang-format, K&R, 2-space indent, 80 columns):

``` sh
cmake --build cmake-build-debug --target format
```

## Layout

- `include/<Module>/` — public headers (`C99/Types.h`, `ZXSpectrum/*.h`, `ChaseHQ/ChaseHQ.h`)
- `libraries/ZXSpectrum/` — a thin ZX Spectrum facade: `in`/`out`/`draw`/`stamp`/`sleep` callbacks and a screen buffer. Game code never touches SDL.
- `libraries/ChaseHQ/Engine/` — the game itself: `Main.c`, `Bank3.c` (128K title screen and tune player), `Bank7.c` (end sequence), `State.h` (all mutable state, ordered by original Z80 address)
- `libraries/ChaseHQ/Data/` — read-only tables: stages, sound samples, title screen, bank 3 data
- `apps/sdl3/` — the host: SDL3 window, event loop, audio and a CRT shader
- `Tests/` — unit tests and a standalone stretchy-object renderer
- `docs/` — how the road drawing works, the stage data format, translation principles and a catalogue of Z80-to-C pitfalls

## How the translation is written

The C is a model of a Z80 program, written to be read. Locals are named after the register that held the value (`A_prev_height`, `HLbackdrop`, `DEscr` - naming is not always consistent), declared at the top of scope in order of first use, each with a `/* intent (was X) */` comment. `EXX` and `EX AF,AF'` banking is modelled explicitly, because that gets confusing quickly otherwise. Deliberate departures from a literal translation are marked `// Conv:`.

`docs/translation-principles.md` and `docs/translation-pitfalls.md` cover this in detail, and `docs/function_comment_template_example.c` is a worked example of what a finished function might look like.

## Stage data

Stage data is extracted from the disassembly rather than typed in by hand: `../Speccy/scripts/convert_stage.py` turns a bank skool file into a C stage data skeleton, though not always successfully. See `docs/convert-stage.md` for usage and what still needs completing by hand afterwards.
