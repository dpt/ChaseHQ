# TODO

- Finish zxgame branch work (just merging?)
- Bug: 48K mode drum timing is off (lack of interrupts?). The end screen's
  copy of the engine (`es_playdrum_go`, bank 7) has the same gap
- Timing: Properly calibrate the game against the original (needs emulator T-state recording)
- Graphics import/export still needs work
- Explore Optimisations

- Implement RISC OS sound support.
- RO build could disable libpng/libjpeg when using DPTLib.


## FOR WRITEUP / NOTES

- New video
- Document the game from start until its first frame is built
- Also it omits unneeded ops - it should be efficient (but nowhere near the
  original)
- Document how functions are created from source Z80
- Add deep dives for each area of the game
- Discuss black screen edges and overdraw
- Claude on medium effort + advisor notes
- Example: creating a whole new stage + backporting to Speccy

https://en.wikipedia.org/wiki/Muntzing
