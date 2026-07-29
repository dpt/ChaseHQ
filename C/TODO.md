# TODO

## P1 — Bugs / broken behaviour

- Title screen
  - title screen doesn't stop and yield to attract mode
  - music still not right (e.g. playing things twice or more over before stopping)
  - animations don't work if screen clearing is enabled (`clear_playfield_buffer` still stubbed with `return;` at `Bank3.c:1782`)
- End screen
  - interrupt mechanism wasn't ported so is the timing a bit off?

## P2 — Game feel / calibration / timing

- Restart bip-bow ticking twice as fast as it should
- Beeper sfx: calibrate per-loop T-state constants + pick `BEEPER_VOLUME_PCT`
- timing: Properly calibrate the game against the original (needs emulator T-state recording)
- Calibrate `TITLE_MUSIC_TSTATES`, `KEMPSTON_MUSIC_TSTATES`, `OMD_MUSIC_TSTATES`, `SUCCESS_MUSIC_TSTATES`, `SPEECH_NIBBLE_TSTATES` (`Internal.h`) — placeholder values
- Title jingle T-state count is a guess, tune by ear (`Bank3.c:3502`)
- Need to model RAM bank contention?

## P3 — Incomplete / missing content

- Fix 48K funcs which aren't hooked up.
- Ensure that funcs are in the original game order (esp. Bank7)
- Complete decoding of all stage data (via the level converter script)
- Split the main loop up into menu/main phases
- `Bank3.c` SFX subsystem gaps (out of scope stubs, need wiring):
  - high-score check (`$C00C`), not disassembled
  - high-score name/rank copy from `$C403`
  - active-control-config header write at (`$8008`)
  - `pitch_offset_default`/`pitch_offset_cur` and `envelope_shape_default`/`envelope_shape_ptr` left NULL pending `decode_pattern_command` table support

## P4 — Polish / visual correctness

- Stretchy test app produces crap output
- 'TEST' marker not drawn when in test mode [CHECK]
- Fix all warnings pass
- Write a code formatting script

## P5 — Clarity pass

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

## P6 — Features / extras

- 128K menu: music, animation, high score, etc.
- game finish screen
- CRT shader enhancements

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

