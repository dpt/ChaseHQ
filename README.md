# _Chase H.Q._ for ZX Spectrum — reverse-engineered and reconstructed in portable C

by David Thomas, 2023-2026

This is an in-progress disassembly and rebuild of the
[ZX Spectrum conversion of Chase H.Q. by Ocean Software](https://spectrumcomputing.co.uk/entry/903/ZX-Spectrum/Chase_HQ).

<p align="center">
  <img src="./static-images/loading.jpeg" alt="Loading screen by Harbonaut"/>
</p>

## [`Speccy/`](Speccy/README.md) — the disassembly

A [SkoolKit](https://skoolkit.ca/) disassembly of the original game: control
files, skool files and a Makefile that fetches the tape, makes a pristine
snapshot and builds assembly listings, TAP/Z80 files and a cross-referenced HTML
disassembly. Just the 128K version is covered, with a control/skool file for
each paged bank.

The current disassembly output is
[available here](https://dpt.github.io/ChaseHQ/). POKEs for infinite credits,
time, turbos and more are listed in that README too.

## [`C/`](C/README.md) — the rebuild

A mostly hand-written, human-readable C reimplementation of the game engine,
translated function by function from the disassembly and keeping the original
game's Z80 addresses in the comments. It builds with CMake, links against SDL3
and plays through all five stages.

The two feed each other: the disassembly explains the game, the C port proves
the explanation is right. They do get out of sync sometimes.

[Play it in your browser](https://dpt.github.io/ChaseHQ/play/) — an
Emscripten/WebAssembly build of the SDL3 app.

## [`C/apps/riscos/`](C/apps/riscos/README.md) — the native RISC OS application

A self-contained 32-bit `!ChaseHQ` desktop application, built from the same
engine sources. See that directory's README for build instructions and host
shortcuts.

## Links

- [My videos about this project](https://www.youtube.com/watch?v=hWz7eTfn2l4&list=PLAn5dfRl63Oc)
- [Obligatory Wikipedia link](https://en.wikipedia.org/wiki/Chase_H.Q.)
- [Spectrum Computing database entry](https://spectrumcomputing.co.uk/entry/903/ZX-Spectrum/Chase_HQ)
- [MobyGames database entry](https://www.mobygames.com/game/9832/chase-hq/)
- [Spectrum Computing forums thread](https://spectrumcomputing.co.uk/forums/viewtopic.php?p=113633)
- [Reassembler's interview with original artist Will Harbison](http://reassembler.blogspot.com/2012/06/interview-with-spectrum-legend-bill.html)
- [Original artist Will Harbison's video about the game](https://www.youtube.com/watch?v=1pT6KOJFgMg)
- [Hacker news discussion about the game's loader](https://news.ycombinator.com/item?id=8850193)
- [Play the original Chase H.Q. online via QAOP](http://torinak.com/qaop#!chasehq)
- [Lou's Pseudo 3D Page](https://www.extentofthejam.com/pseudo/index.html)

## Related

- [My previous Speccy disassembly - The Great Escape](https://github.com/dpt/The-Great-Escape) ... [and its rebuild in C](https://github.com/dpt/The-Great-Escape-in-C)
- [My AY-8912 emulator, SlopAY](https://github.com/dpt/SlopAY)
