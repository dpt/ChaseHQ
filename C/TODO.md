# TODO

- Bug: 48K mode drum timing is off (lack of interrupts?). The end screen's
  copy of the engine (`es_playdrum_go`, bank 7) has the same gap
- Continue backporting changes to the disasm
- Flush out this 'it's a custom level' germ of an idea that keeps reappearing
- New video
- Optimisations
- Timing: Properly calibrate the game against the original (needs emulator T-state recording)
- RISC OS: Sound support (...how?)
- Graphics import/export still needs work

## FOR WRITEUP / NOTES

- Document the game from start until its first frame is built
- Also it omits unneeded ops - it should be efficient (but nowhere near the
  original)
- Document how functions are created from source Z80
- Add deep dives for each area of the game
- Discuss black screen edges and overdraw
- Claude on medium effort + advisor notes
- Example: creating a whole new stage + backporting to Speccy

https://en.wikipedia.org/wiki/Muntzing
