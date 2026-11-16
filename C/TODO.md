# TODO

- Audit "Conv: added" vars
- Bug: 48K mode drum timing is off (lack of interrupts?)
- Bug: Highscore: Characters vanish
- Bug: Highscore: Flicker at bottom when scrolling
- Bug: Highscore: Some characters collide (check against the original)
- Bug: Highscore: Test: non-highest score
- Bug: Success tune cut short?
- End screen: port bank 7's own 48K interrupt handler and music engine
  (`es_setup_interrupts`, `$E01E`); `Bank7.c` stubs it
- Continue backporting changes to the disasm
- Flush out this 'it's a custom level' germ of an idea that keeps reappearing
- Investigate Claude fixes in case they've made the C diverge from the asm
- New video
- Optimisations
- Slap warnings on the level converter now it's finished?
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
