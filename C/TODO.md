# TODO

## BUGS

- Stretchy test app produces crap output
- Jittery hazards (teleporting)
- (bitmap?) Crash when during the smash phase
- 48k music routine seizes up


## TODO

- Fix Burst of randomish noises when the game is quit
- Restart bip-bow ticking twice as fast as it should [game pacing / calibration?]
- Beeper sfx: implemented (virtual T-state clock + box-filter mixing).
  Remaining: calibrate the per-loop T-state constants (DJNZ_LOOP_TSTATES
  sites in ChaseHQ.c) against the original, and pick a final
  BEEPER_VOLUME_PCT (Main.c).
- Overhead graphic format / handler
- Helicopter data
- 'TEST' marker not drawn when in test mode (might need more menu work)
- Ensure if-else structuring is added where sensible (top-down)
- Identify unrolled ops to roll up
- Guard words?
- Document expected values/ranges for e.g. xpos arrays
- Remove Z80 macros where it clarifies
- Remove any local vars from state
- Sort all decls by order of use
- Sort all params by register
- Sound effects, music, timing, menu animation
- Ensure partial screen updates are correct
- Complete decoding of all stage data
- Why don't watchpoints work in CLion?
- Properly calibrate the game against the original. Would need an emulator
  where I can record the T-states elapsed between two points.
- Merge draw_road backinto one big unholy lump
- Investigate Claude fixes in case they've made the C diverge from the asm.
- Drop 'ChaseHQ-' from all the source filenames.


## IDEAS

- Factor our the 22 depth levels value out to a constant
- Scan for type problems
- Build a tiny test level
- Identify missing cases where wraparound is required.
- Remove as much casting as possible (Claude tends to add it).
- Hoist out all interesting Z80 addresses to constants
- Lots more unit tests.
- Fix all warnings pass
- Update summaries of major functions to docs/
- Sort macros
- Design a text format for holding stages
- Annotate all state members to show which functions consume/mutate them
- Add deep dives for each area of the game (each main loop item?)
- Document the game from the moment it starts until it builds the first frame?
- Is fixing the keypress thread race important?
- Have an example of creating a whole new stage. Then backporting it to the Speccy.
- Document how functions are created from source Z80 (eg. coping with banked regs).
- Document road_pos valid range
- Big reformat pass once happy with code.
- Add new levels
- Fix all the rough masks to improve the look
- Arcade machine based on this code
- Just tony and raymond having a chat, describing the game
- Random level generator (on the fly?)
- Extra bumpy roads
- Improved AI driver
- Lua/Python integration
- Diagram generation for stretchy graphics (via stretchy test app)


## FOR WRITEUP

- Discuss the black screen edges and the overdraw it disguises


## NOTES

- Claude on medium effort + advisor seems to solve more issues than Claude
  itself on high effort. Though the advisor API seems intermittent...


Drawing
-------
speccy->draw is called to say some part of the screen has changed
speccy->draw calls zx_draw
zx_draw does all the box merging - no screen pixels are converted until asked for
zx_draw calls draw_handler 
draw_handler doesn't have to do anything, but it's best to.


Decompiler
----------
Reads in a SkoolKit function and traces register and flag creation-use.
Like SSA but then retains registers that are mutated.
Z80 specialties.
Generates unique vars names.
Can name vars using {C=foo} in SkoolKit comments.

TODO: basic blocks
within basic blocks can do constant folding

Knows about self modifying locations.

Knows about constants.

Outputs to a text table or a graphviz dot file.

TODO: Output C decls and expressions.

