# TODO

## P1 — Bugs / broken behaviour

- Perp car in wrong place when caught? Original bug?
- 48k music routine seizes up
- Title screen
  - music still not right (e.g. playing things twice or more over before stopping)
  - music plays while animating
  - animations don't stop in some cases
  - animations don't work if screen clearing is enabled
  - title screen doesn't stop and yield to attract mode

## P2 — Game feel / calibration / timing

- Restart bip-bow ticking twice as fast as it should
- Beeper sfx: calibrate per-loop T-state constants + pick `BEEPER_VOLUME_PCT`
- timing: Properly calibrate the game against the original (needs emulator T-state recording)
- Calibrate `TITLE_MUSIC_TSTATES`, `KEMPSTON_MUSIC_TSTATES`, `OMD_MUSIC_TSTATES`, `SUCCESS_MUSIC_TSTATES`, `SPEECH_NIBBLE_TSTATES` (`Internal.h`) — placeholder values
- Title jingle T-state count is a guess, tune by ear (`Bank3.c:3458`)

## P3 — Incomplete / missing content

- Complete decoding of all stage data (via the level converter script)
- Split the main loop up into menu/main phases
- `Stage3Data.c` / `Stage5Data.c`: decode `addrof_helicopter_stuff_1`/`addrof_helicopter_stuff_2`, currently NULL raw data
- Border colour not implemented, always black (`SDLMain.c:297`, `SDLMain.c:780`)
- `Bank3.c` SFX subsystem gaps (out of scope stubs, need wiring):
  - SFX trigger-table setup (`$F7DB`-`$F82C`) and `stst_load_sfx_script` call
  - digitised-sample SFX subsystem (`$F837`/`$F895`/`$F8A2`)
  - `stop_music_and_silence` (`$ED0B`) calls not wired (3 sites)
  - coin-slot input read (`$800E`) not wired
  - high-score check (`$C00C`), not disassembled
  - high-score name/rank copy from `$C403`
  - active-control-config header write at (`$8008`)
  - scene-selector SM operand seed with `A_anykey`
  - "fire pressed -> start game via `$FBA2`" wiring
  - `pitch_offset_default`/`pitch_offset_cur` and `envelope_shape_default`/`envelope_shape_ptr` left NULL pending `decode_pattern_command` table support
- `Main.c:1090` stub function body, "Write this"

## P4 — Polish / visual correctness

- Stretchy test app produces crap output
- Ensure partial screen updates are correct
- Draw road pipeline: merge `draw_road` back into one big unholy lump
- 'TEST' marker not drawn when in test mode
- Fix all warnings pass

## P5 — Clarity pass

- Fix `Awidth_bytes` vars which seem to be x coords
- Remove Z80 macros where it clarifies
- Audit "Conv: added" vars
- Match up menu funcs for string printing with the main code - are they the same code?
- Big reformat pass once happy with code
- Sort macros
- Update summaries of major functions to docs/
- Hoist out all interesting Z80 addresses to constants
- Factor out more constants (e.g. 21)
- Scan for type problems
- Identify missing cases where wraparound is required
- Remove as much casting as possible (Claude tends to add it)

## P6 — Features / extras

- 128K menu: music, animation, high score, etc.
- game finish screen

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

