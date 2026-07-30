# TODO

## P1 — Bugs / broken behaviour

- Title screen
  - title screen doesn't stop and yield to attract mode
  - music still not right (e.g. playing things twice or more over before stopping)
  - ~~animations don't work if screen clearing is enabled~~ — **done** in
    `363d910`. `clear_playfield_buffer` (`Bank3.c:1724`) is fully translated
    and called from `titlescr_animate_frame`; verified against `$CC04-$CC4F`
- End screen
  - the interrupt mechanism wasn't ported so the timing feels a little off

## P2 — Game feel / calibration / timing

- Restart bip-bow ticking twice as fast as it should
- Beeper sfx: calibrate per-loop T-state constants + pick `BEEPER_VOLUME_PCT`
- timing: Properly calibrate the game against the original (needs emulator T-state recording)
- Calibrate `TITLE_MUSIC_TSTATES`, `KEMPSTON_MUSIC_TSTATES`, `OMD_MUSIC_TSTATES`, `SUCCESS_MUSIC_TSTATES`, `SPEECH_NIBBLE_TSTATES` (`Internal.h`) — placeholder values
- Title jingle T-state count is a guess, tune by ear (`Bank3.c:3502`)
- Need to model RAM bank contention?

## P3 — Incomplete / missing content

- Ensure that funcs are in the original game order (esp. Bank7) -- one of the scripts covers this
- Complete decoding of all stage data (via the level converter script)
- Split the main loop up into menu/main phases
- `Bank3.c` SFX subsystem gaps (out of scope stubs, need wiring):
  - high-score name/rank copy from `$C403`
  - active-control-config header write at (`$8008`)
  - `pitch_offset_default`/`pitch_offset_cur` and `envelope_shape_default`/`envelope_shape_ptr` left NULL pending `decode_pattern_command` table support

## P4 — Polish / visual correctness

- Stretchy test app produces crap output
- Fix all warnings pass

## P5 — Clarity pass

### Authority sweep (comments that read as guesswork)

Each item below is a comment that admits doubt, talks about the porting
process rather than the code, or leaves skool markup in place. Resolve the
question against the skool then rewrite the comment as a statement — or
delete it.

- ~~Open questions in `perp_behaviour`~~ — **done**. `hit_timer` is a
  four-frame post-collision cooldown, not a speed boost; the `+2`/`−3` pair is
  one distance proximity window, not lanes; the `road_pos` band walk derives
  the legal min/max lane the same way `get_spawn_lanes` does; the `changing
  lane` flag is confirmed; the close-range term raises the perp's *speed* by 30
  per unit closed inside 13 (rubber-banding); `$A77D-$A781` really is dead code
  in the original — only the flat `+30` has any effect
- Open questions in the draw path: `Main.c:4994` ("Loop until ... <what
  condition?>"), `5064`, `5128`, `9156` (the `RR` audit below), `9231`,
  `10238`, `11185`, `11200`, `11275`, `11294`, `11370`, `11372`, `15191`
- Open questions in road/curvature code: `Main.c:12372`, `12375`, `12376`
  ("why negate, is this a counter?"), `12402`, `14069`, `14816`, `15839`,
  `15843`, `15881`, `16011`, `16037-16039` ("C is ?", "L is ?", "DE is ?")
- Unexplained banking comments: `Main.c:4668`, `7873` ("seems to be unused"),
  `11638`, `11854`, `11859`, `14969`, `14988`
- Hedged behaviour notes to confirm and restate: `Main.c:652`, `1645`, `1717`
  ("not sure why"), `2209`, `2285`, `6362`, `7559`, `7790`, `8121`, `8203`,
  `8377`, `8437`, `9446-9475`, `9627`, `9630` ("weird"), `10184`, `10641`,
  `10647`, `10658`, `10709-10710`, `10742`, `10945`, `11012`, `11081`,
  `12416`, `13936`, `16553`, `16700`, `16973`
- Meta comments about the port rather than the game: `Bank3.c:4281` ("Delete
  all of this zero init and Claude verbiage") still open; `Main.c:3109` and
  `Main.c:3182` **done**
- ~~Field comments in `State.h` ending in a question mark~~ — **done**.
  `$5B00..$5BFF` is a reserved 256-byte page holding a 240-byte pre-shifted
  backdrop; `dhs_heli_vert_base` is the helicopter y base; `smoke_bitmap_index`
  is a 0..5 level-of-detail index from draw-list depth; `ahc_hand_delay` is a
  per-step frame counter, not a frame index
- ~~Variable comments with no register attribution~~ — **done**.
  `reveal_perp_car`'s height/bitmap are plain `B`/`HL` with no banking; the
  pregame `rows` value is `A` at `$86B7`
- ~~Skool markup leaking into C comments~~ — **done**, 23 sites rewritten to
  plain register names and `$xxxx` addresses
- Commented-out translation scaffolding, mostly "removed presumed unused" or
  "might not need": `Main.c:4279`, `5112`, `5268`, `5733`, `6672`, `7120`,
  `7128`, `9052`, `9179`, `10584`, `11192`, `11692`, `11857`, `11859`,
  `12216-12223`, `16067-16069`, plus `SDLMain.c:1036-1041`. Confirm each is
  dead then delete it
- `HACK` marker on `INITIAL_ATTRACT_SPEED` (`Internal.h:29`) — "// HACK was
  400" alongside a value of 400 says nothing; establish the original value
- Outstanding `TODO:` markers not already tracked above: `Bank3.c:640`,
  `Bank3.c:3604`, `Bank7.c:186` (48K sound code in bank 7 not ported),
  `Main.c:17002` ("the call-return setup needs analysing"), `SDLMain.c:344`
  and `SDLMain.c:1072` (border colour). The `TODO: Calibrate` cluster in
  `Internal.h` is covered under P2

- Audit `RR(Atotal)` in Main.c:8988 (`// halve?`) — carry-in provenance from the preceding multiply loop is unclear, unlike the two confirmed/removed forced-carry-0 cases
- Audit "Conv: added" vars
- Match up menu funcs for string printing with the main code - are they the same code?
- Big reformat pass once happy with code
- Sort macros
- Update summaries of major functions to docs/
- Scan for type problems
- Identify missing cases where wraparound is required
- Remove as much casting as possible (Claude tends to add it) [part done]
- C89 compat [part done]

## P7 — Investigation / meta

- Investigate Claude fixes in case they've made the C diverge from the asm
- Why don't watchpoints work in CLion?

## FOR WRITEUP / NOTES

- Document the game from start until first frame built
- Explain that it's "Actual C code" designed to be read by humans - not machine generated grot
- Also it omits unneeded ops - it should be efficient (but nowhere near the original)
- Document how functions are created from source Z80
- Add deep dives for each area of the game
- Discuss black screen edges and overdraw
- Claude on medium effort + advisor notes
- Example: creating a whole new stage + backporting to Speccy

## IDEAS (backlog)

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


| Lanes & Objs   | Curve | Height | Hazards | Loop |
| -------------- | ----- | ------ | ------- | ---- |
| B : . . . :    |   >>  |   :    |         |  AA  |
|   : . . . : T  |   >>  |   :    |    B    |      |
| B : . . . :    |   >   |   :    |         |      |
|   : . .  /  T  |   >   |   :    |         |      |
| B : . . :      |   >   |   vv   |         |      |
|   : . . :   T  |   >   |   v    |         |      |
|   : . . :      |   :   |   :    |         | *AA* |
|   : . . :   T  |   :   |   ^^   |         |      |
| T : . . :      |   :   |   ^    |         |      |
|   : . . :   T  |   :   |   :    |         |      |

