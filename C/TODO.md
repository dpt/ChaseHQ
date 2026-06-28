# BUGS

- Accelerating forward past a point the breaks down [done]
- Steering seems broken - unsure
- No turbo LO/HI readout nor 'STAGE 1' readout - update_scoreboard incomplete
- Radio screen sometimes have leftover transition animation parts
- Main game runs too fast - need to calibrate against original
- Object plotting broken and commented out for now
- Perp escape scene causes an immediate crash


# IDEAS

- Update summaries of major functions to docs/
- Update CLAUDE.md now the stage data is in
- Sort macros
- Design a text format for holding stages
- Merge draw_road backinto one big unholy lump
- Annotate all state members to show which functions consume/mutate them
- Add deep dives for each area of the game (each main loop item?)
- Document the game from the moment it starts until it builds the first frame?- Is fixing the keypress thread race important?


# DONE

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

