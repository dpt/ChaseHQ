# TODO

- fix: High score screen bugs
  - flicker at bottom when scrolling
  - hard to enter name
  - characters vanish
  - test: non-highest score
- fix: Disable test flag by default (perhaps put it on an F-key)
- timing: Properly calibrate the game against the original (needs emulator
  T-state recording)
- Complete decoding of all stage data (via the level converter script)
- Stretchy test app produces crap output [checked - still a bit rubbish] -
  delete it?
- Start backporting changes to the disasm
- 48K mode drum timing is off (interrupts?)
- Quit in pause mode locks up?

### Authority sweep (comments that read as guesswork)

Each item below is a comment that admits doubt, talks about the porting process
rather than the code, or leaves skool markup in place. Resolve the question
against the skool then rewrite the comment as a statement — or delete it.

- Meta comments about the port rather than the game: `Bank3.c:4303` ("Delete all
  of this zero init and Claude verbiage")
- Outstanding `TODO:` markers not already tracked above: `Bank3.c:642`,
  `Bank7.c:185` (48K sound code in bank 7 not ported), `Main.c:17339` ("the
  call-return setup needs analysing"), `SDLMain.c:345` and `SDLMain.c:1073`
  (border colour). The `TODO: Calibrate` cluster in `Internal.h` is covered
  under P2, and the `Bank3.c` SFX gaps under P3

- Audit "Conv: added" vars
- Big reformat pass once happy with code
- Name the numbered `chqstate` SM field groups after what distinguishes them,
  the way `dr_{left,right}_{markings,fill}_page` now are: `dr_backbuf_1/_2`,
  `dt_fill_start_a/_b`, `dee_draw_tunnel_1/_2`, `meter_1/2_level`. Each numbers
  genuinely parallel SM sites, so this needs the consumers traced first — the
  rest of the naming pass is done

## P7 — Investigation / meta

- Investigate Claude fixes in case they've made the C diverge from the asm
- Why don't watchpoints work in CLion?

## FOR WRITEUP / NOTES

- Document the game from start until first frame built
- Explain that it's "Actual C code" designed to be read by humans - not machine
  generated grot
- Also it omits unneeded ops - it should be efficient (but nowhere near the
  original)
- Document how functions are created from source Z80
- Add deep dives for each area of the game
- Discuss black screen edges and overdraw
- Claude on medium effort + advisor notes
- Example: creating a whole new stage + backporting to Speccy

## IDEAS (backlog)

- Overlay for controls (using the same font as the game)
- Lots more unit tests
- Design a text format for holding stages
- Annotate all state members to show which functions consume/mutate them
- Is fixing the keypress thread race important?
- Add new level(s) - actual good one, not just test
- Fix all the rough masks to improve the look
- Arcade machine based on this code
- Just Tony and Raymond having a chat
- Random level generator
- Extra bumpy roads
- Improved AI driver
- Lua/Python integration
- Diagram generation for stretchy graphics
- Emscripten build
- Pull across the demo version of the game
