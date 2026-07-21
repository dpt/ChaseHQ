# TODO

## P1 — Bugs / broken behaviour

- Perp car in wrong place when caught? Original bug?
- 48k music routine seizes up
- Title screen
  - music still not right
  - music plays while animating
  - animations don't stop in some cases
  - animations don't work if screen clearing is enabled
  - title screen doesn't stop and yield to attract mode

## P2 — Game feel / calibration / timing

- Restart bip-bow ticking twice as fast as it should
- Beeper sfx: calibrate per-loop T-state constants + pick `BEEPER_VOLUME_PCT`
- timing: Properly calibrate the game against the original (needs emulator T-state recording)

## P3 — Incomplete / missing content

- Complete decoding of all stage data (via the level converter script)

## P4 — Polish / visual correctness

- Stretchy test app produces crap output
- Ensure partial screen updates are correct
- Draw road pipeline: merge `draw_road` back into one big unholy lump
- 'TEST' marker not drawn when in test mode

## P5 — Clarity pass

- Fix `Awidth_bytes` vars which seem to be x coords
- Remove Z80 macros where it clarifies
- Remove any vars which could be local from state
- Sort all decls by order of use
- Sort all params by register
- Audit "Conv: added" vars
- Match up menu funcs for string printing with the main code - are they the same code?

## P6 — Features / extras

- 128K menu: music, animation, high score, etc.
- game finish screen

## P7 — Investigation / meta

- Investigate Claude fixes in case they've made the C diverge from the asm
- Why don't watchpoints work in CLion?

## IDEAS (backlog)

- VSync
- Factor out more constants (e.g. 21)
- Scan for type problems
- Identify missing cases where wraparound is required
- Remove as much casting as possible (Claude tends to add it)
- Hoist out all interesting Z80 addresses to constants
- Lots more unit tests
- Fix all warnings pass
- Update summaries of major functions to docs/
- Sort macros
- Design a text format for holding stages
- Annotate all state members to show which functions consume/mutate them
- Document the game from start until first frame built
- Is fixing the keypress thread race important?
- Document `road_pos` valid range
- Big reformat pass once happy with code
- Add new level(s)
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

## FOR WRITEUP / NOTES

- Explain that it's "Actual C code" designed to be read by humans - not machine generated grot
- Also it omits unneeded ops - it should be efficient (but nowhere near the original)
- Document how functions are created from source Z80
- Add deep dives for each area of the game
- Discuss black screen edges and overdraw
- Claude on medium effort + advisor notes
- Example: creating a whole new stage + backporting to Speccy

