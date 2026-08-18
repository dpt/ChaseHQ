# TODO

- Audit "Conv: added" vars
- Audit: Can we delete a lot of the logtime calls now?
- Bug: 48K mode drum timing is off (lack of interrupts?)
- Bug: Highscore: Characters vanish
- Bug: Highscore: Flicker at bottom when scrolling
- Bug: Highscore: Hard to enter name
- Bug: Highscore: Test: non-highest score
- Compare the demo level with stage 4
- Continue backporting changes to the disasm
- Flush out this 'it's a custom level' germ of an idea that keeps reappearing
- Investigate Claude fixes in case they've made the C diverge from the asm
- New video
- Optimisations
- Slap warnings on the level converter now it's finished?
- Timing: Properly calibrate the game against the original (needs emulator
  T-state recording)
- Idea: Mouse steering!

### Authority sweep (comments that read as guesswork)

Each item below is a comment that admits doubt, talks about the porting process
rather than the code, or leaves skool markup in place. Resolve the question
against the skool then rewrite the comment as a statement — or delete it.

- Outstanding `TODO:` markers not already tracked above: `Bank3.c:6309` (copy
  hi-score name/rank rows), `Bank3.c:9389` (drum duration tune-by-ear),
  `Bank7.c:395` (48K sound code in bank 7 not ported), `Main.c:20680` ("the
  call-return setup needs analysing"), `SDLMain.c:1789` (border colour). The
  `TODO: Calibrate` cluster in `Internal.h` is covered under P2, and the
  `Bank3.c` SFX gaps under P3

- Name the numbered `chqstate` SM field groups after what distinguishes them,
  the way `dr_{left,right}_{markings,fill}_page` now are: `dr_backbuf_1/_2`,
  `dt_fill_start_a/_b`, `dee_draw_tunnel_1/_2`, `meter_1/2_level`. Each numbers
  genuinely parallel SM sites, so this needs the consumers traced first — the
  rest of the naming pass is done

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
