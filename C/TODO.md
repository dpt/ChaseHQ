perhaps build unit tests now to flush out problems ahead of time.


# BUGS

- Wonky feeling car speed / anims at start
- Lane change diagonal transitions are not working
- Message cursor seems to vanish when idle
- Forks seize the game up
- draw_scene_objects => crash
- Transitions still not always right (perhaps just spiral?)
- Floating fork arrow vanishes too soon
- Tunnel drawing wrong
- No hazards
- Wonky use of BCpadding in draw_object_clipped - passed as a height


# TODO

- Ensure partial screen updates are correct
- Ensure if-else structuring is added where sensible (top-down)
- Identify unrolled ops to roll up
- Guard words?
- Object plotting broken and commented out for now
- Main game runs too fast - need to calibrate against original
- Document expected values/ranges for e.g. xpos arrays
- Fix stage 2+ data
- Remove Z80 macros where it clarifies
- Remove any local vars from state
- Sort all decls by order of use
- Sort all params by register
- Add an ADDRTOBACKBUF that takes (H,L) and does (H<<8)|L itself


# IDEAS

- Scan for type problems
- Build a tiny test level
- Identify missing cases where wraparound is required.
- Remove as much casting as possible (Claude tends to add it).
- Hoist out all interesting Z80 addresses to constants
- Lots more unit tests.
- Should hi2xpostab return u16* ?
- Fix all warnings pass
- Update summaries of major functions to docs/
- Update CLAUDE.md now the stage data is in
- Sort macros
- Design a text format for holding stages
- Merge draw_road backinto one big unholy lump
- Annotate all state members to show which functions consume/mutate them
- Add deep dives for each area of the game (each main loop item?)
- Document the game from the moment it starts until it builds the first frame?
- Is fixing the keypress thread race important?
- Have an example of creating a whole new stage. Then backporting it to the Speccy.
- Document how functions are created from source Z80 (eg. coping with banked regs).
- Document road_pos valid range
- Big reformat pass once happy with code.


# DONE

- Hitting invisible objects (right hand objects mispositioned on 3-lane)
- The pregame radio screen sometimes has leftover transition animation parts
- Backdrop does not horizontally scroll (to the right)
- Perp escape scene causes an immediate crash (prob. tunnel code)
- Car (still) not jumping on steep roads
- Mark functions as HQ once processed
- Car starts level at speed
- Sky attributes intruding on road
- Road corrupt in distance
- Road corrupt (looks like a wall) at looping point
- Lane markings don't appear closest to player: draw_road_lanes_change
  (Bresenham interpolation) is called at map position 3 (stage 1 lane
  transition) and pushes some xpos entries off-screen at Lrow=0xFF.
  Verify whether this matches Z80 behaviour or is a translation bug.
  [unsure what fixed this...]
- Toggling gear down not working
- Startup scene obviously wrong road type (should be three lanes)
- Steering seems broken - unsure - is car actually moving?
  - Steeting right is working?
- Add Code tidying agent - validating for readability too
- No turbo LO/HI readout nor 'STAGE 1' readout - update_scoreboard incomplete [done]
- Accelerating forward past a point the breaks down [done]
- Rename PERPCAUGHTPHASES [done]
- Promote all vars if possible [done]
- Add a CODEOWNERS file [done]
- Improve arg names [done]
- Add symbols for states of hand_flag [done]
- Add symbols for smash_counter max value, for perp speed [done]
- LO_ADD -> U16_ADD_LO [done]
- Start unit tests [done]
- Fix stageXdata TODOs [part done]
- Convert hex in stageXdata to __XX__ style [part done]
- Promote args where possible [done]
- While the road drawing looks mostly correct the checkerboard pattern isn't right [done]


# NOTES

Claude on medium effort + advisor seems to solve more issues than Claude itself on high effort. Though the advisor API seems intermittent...


Drawing
-------
speccy->draw is called to say some part of the screen has changed
speccy->draw calls zx_draw
zx_draw does all the box merging - no screen pixels are converted until asked for
zx_draw calls draw_handler 
draw_handler doesn't have to do anything, but it's best to.

