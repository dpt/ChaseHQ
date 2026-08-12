> $4000 ; demo.skool
> $4000 ;
> $4000 ; This is a SkoolKit control file that disassembles the Sinclair User issue 94
> $4000 ; "Megatape 23" promotional demo of "Chase H.Q." by Ocean Software.
> $4000 ;
> $4000 ; <https://worldofspectrum.org/archive/software/cover-tapes-and-electronic-magazines/sinclair-user-issue-94-megatape-23-sinclair-user>
> $4000 ;
> $4000 ; This is a distinct build from the retail game disassembled by
> $4000 ; ../ChaseHQ-128K.skool: a single 48K (non-banked) memory layout, its own
> $4000 ; "Paul Owens" tape protection loader (handled by SkoolKit's built-in
> $4000 ; accelerator=rom,paul-owens sim-load config), and its own menu system. It
> $4000 ; features a single stage using stage 1's graphics and a custom map, no retry
> $4000 ; on fail.
> $4000 ;
> $4000 ; Reverse engineering by David Thomas <dave@davespace.co.uk>, 2026.
> $4000 ; Sources live at <https://github.com/dpt/ChaseHQ>.
> $4000 ;
> $4000 ;
> $4000 ; MEMORY MAP (pristine, as-loaded state)
> $4000 ; ---------------------------------------
> $4000 ; $4000-$5AFF  Screen memory (bitmap + attributes), loaded by tape block 4
> $4000 ; $5B00-$5BFF  Unloaded gap; corresponds to the retail game's pre-shifted
> $4000 ;              backdrop scratch area at the same address
> $4000 ; $5C00-$EE7F  Main data+code block, loaded by tape block 5 (37504 bytes).
> $4000 ;              Starts at the same address the retail 128K game uses for its
> $4000 ;              own per-stage data. Roughly half of this block is byte-for-byte
> $4000 ;              identical to the retail 128K game (compared against every one
> $4000 ;              of its eight $C000-$FFFF banks plus the fixed $4000-$BFFF
> $4000 ;              region): almost the entire game engine from $8153 onward, plus
> $4000 ;              per-stage data cherry-picked from stages 1, 4 and 5. Matched
> $4000 ;              spans are labelled below with their retail source address;
> $4000 ;              "not yet identified" spans are either genuinely demo-specific
> $4000 ;              (menu, custom map) or draw on retail bank 0, which the main
> $4000 ;              project hasn't disassembled separately (see
> $4000 ;              ../ChaseHQ-128K.ctl - only banks 1,3,4,6,7 have their own ctl,
> $4000 ;              since banks 2/5 are the always-resident fixed region and bank 0
> $4000 ;              was never split out)
> $4000 ; $8000        Real code entry point: the loader hands off here with
> $4000 ;              JP C,$8000 once both blocks have loaded successfully
> $4000 ; $EE80-$FFFF  Not part of any tape block; leftover/padding RAM content, not
> $4000 ;              meaningful (confirmed for $EE80-$EFFF by inspection; assumed
> $4000 ;              likewise up to $FFFF pending further checking)
> $4000 ; $F000-$F0EB  Paul Owens protection loader (BASIC loader's resident machine
> $4000 ;              code, disassembled by hand - see below)
> $F000 ;
> $F000 ; PAUL OWENS LOADER ($F000-$F0EB)
> $F000 ; --------------------------------
> $F000 ; $F000  Entry point. Clears attributes, then calls the block loader twice
> $F000 ; $F017  CALL $F043 to load the screen block to $4000, length 6912
> $F000 ; $F02A  CALL $F043 to load the main block to $5C00, length 37504
> $F000 ; $F034  JP C,$8000 - handoff to real game code on successful load
> $F000 ; $F037  Border-flash error loop, taken on load failure
> $F000 ; $F043  Block loader: reads a flag byte, then a length-prefixed run of
> $F000 ;         bytes via the low-level bit/edge-reading primitives at $F0CA
> $F000 ; $F0CA  Low-level tape bit-read primitive
> $F000 ; $F0CE  Low-level tape edge-read primitive
> $F000 ;
@ $4000 start
b $4000 Screen memory
u $5B00 Unloaded gap (pre-shifted backdrop scratch area in the retail game)
b $5C00 [Stage 1] Horizon graphic (identical to retail fixed $5C00)
b $5CF0 Not yet identified
b $5D37 [Stage 1] Nancy's perp description (identical to retail fixed $5D45)
b $5E23 Not yet identified
b $6016 [Stage 4] Map curvature data (identical to retail bank6 $E577)
b $6030 Not yet identified
b $6035 [Stage 4] Map height data (identical to retail bank6 $E59A)
b $6065 Not yet identified
b $6080 [Stage 4] Map hazards data (identical to retail bank6 $E5FB)
b $609E Not yet identified
b $613D [Stage 4] Map right object data (identical to retail bank6 $E6CF)
b $6170 Not yet identified
b $619D [Stage 1] Ralph the Idaho Slasher's mugshot (identical to retail fixed $638A)
b $6254 Not yet identified
b $62CF [Stage 1] LODs (identical to retail fixed $64BC)
b $6CD6 Not yet identified
b $6CE5 [Stage 5] Perp's mugshot (identical to retail bank7 $D9E5)
b $6D13 Not yet identified
b $6D32 [Stage 1] LODs (identical to retail fixed $703F)
b $6DEA Not yet identified
b $6DF5 [Stage 4] Pilot's mugshot (identical to retail bank6 $EFF5)
b $705A Not yet identified
b $7063 [Stage 5] Perp's mugshot (identical to retail bank7 $D295)
s $73EE Not yet identified
b $76D6 Identical to retail bank5 $F6D6 (not yet disassembled there)
c $8000 Not yet identified
c $8153 Reveals the perp's car on the pre-game screen (identical to retail fixed $85F5)
c $816D Not yet identified
c $81A1 Animate the signal meters (identical to retail fixed $8643)
c $81C0 Not yet identified
c $81DC Draw the pre-game screen (identical to retail fixed $867E)
c $8278 Not yet identified
c $828F Sets up the stage (identical to retail fixed $87F3)
c $82FE Not yet identified
c $83EE Crash sound effect (identical to retail fixed $8963)
c $8469 "Thud" sound effect (identical to retail fixed $89DE)
c $84A1 Not yet identified
c $84A6 Cornering sound effect (identical to retail fixed $8A1B)
c $84E2 Not yet identified
c $8520 Generates engine noise (48K) (identical to retail fixed $8242)
c $8536 Not yet identified
c $855C Identical to retail bank0 $F271 (not yet disassembled there)
c $8575 Not yet identified
c $86BB Handle perp caught (identical to retail fixed $8B19)
c $87B9 Not yet identified
c $87F5 Fully smashed (identical to retail fixed $8C53)
c $8934 Not yet identified
c $8973 Drives transitions (the fade between scenes) (identical to retail fixed $8DD1)
c $899A Not yet identified
c $89CA Sets up a transition (the fade between scenes) (identical to retail fixed $8E28)
c $89E3 Not yet identified
c $8BEB Draws overhead graphics (identical to retail fixed $908B)
c $8CBA Not yet identified
c $8D0A Draws stretchy objects, such as trees (identical to retail fixed $91AA)
c $8D96 Not yet identified
c $8DDC Draws objects (left hand version) (identical to retail fixed $927C)
c $8E39 Not yet identified
c $8E45 Draws objects (right hand version) (identical to retail fixed $92E5)
c $9001 Not yet identified
c $9015 Sprite plotter for back buffer, up to 64px wide, 15px high, no mask, no flip (identical to retail fixed $94B5)
c $9092 Not yet identified
c $90B3 Sprite plotter for back buffer, up to 64px wide, 15px high, no mask, flips (identical to retail fixed $9553)
c $9101 Not yet identified
c $911C Routine at 9565 (identical to retail fixed $95BC)
c $9166 Not yet identified
c $917E Random number generator (identical to retail fixed $961E)
c $940A Not yet identified
b $94B3 [Messages] Start of stage chatter (identical to retail fixed $81EB)
c $94CC Not yet identified
c $9580 Shows the chatter - the alerts and remarks from the game's characters (identical to retail fixed $99F9)
c $95AA Not yet identified
c $95FD Noise in/out effect used for mugshots (identical to retail fixed $9A76)
c $9643 Plots a face on the screen (identical to retail fixed $9ABC)
c $9680 Not yet identified
c $9684 Plot mini font characters (identical to retail fixed $9AFD)
c $9755 Not yet identified
c $97EB Add a bonus (identical to retail fixed $9CF6)
c $9806 Not yet identified
c $980F Increments the score by (D,E,A) (identical to retail fixed $9D1A)
c $9826 Not yet identified
c $9871 Update scoreboard and flashing lights (identical to retail fixed $9D86)
c $98FF Not yet identified
c $9927 Draws the turbo sprites and updates the displayed speed, time, distance and score (identical to retail fixed $9E3C)
c $9A2A Not yet identified
c $9A3F Plots an 8x15 LED font digit to the screen (identical to retail fixed $9F54)
c $9A98 Another draw string entry point? (identical to retail fixed $9FAD)
c $9ADA Not yet identified
c $9AE6 Draws a character (to buffer or screen?) (identical to retail fixed $9FFB)
c $9BC4 Not yet identified
c $9BE6 Keyscan (identical to retail fixed $A0FB)
c $9C25 Not yet identified
g $9C59 Game status buffer entry at A139 (identical to retail fixed $A16B)
c $9E8C Not yet identified
c $9E8F Checks for scenery collisions (identical to retail fixed $A3B2)
c $9F82 Not yet identified
c $9FAC Scenery was hit (identical to retail fixed $A4CF)
c $9FC3 Not yet identified
c $A019 Lays out roadside objects (identical to retail fixed $A5CD)
c $A030 Not yet identified
c $A037 Counters (identical to retail fixed $A611)
c $A054 Not yet identified
c $A081 Perp car behaviour (identical to retail fixed $A65B)
c $A1BF Not yet identified
c $A29A Spawns cars (identical to retail fixed $A882)
c $A2B7 Not yet identified
c $A2BD Returns the range of lanes that cars or hazards should spawn within (identical to retail fixed $A8A5)
c $A2E8 Not yet identified
c $A2FF Hazard handler routine? (identical to retail fixed $A8E7)
c $A34B Not yet identified
c $A37F Spawn hazards (identical to retail fixed $ABAC)
c $A41E Not yet identified
c $A429 Test for collision with hazard (identical to retail fixed $AC56)
c $A4E3 Not yet identified
c $A4F8 Checks for hazard collisions (identical to retail fixed $AD25)
c $A547 Not yet identified
c $A54D Hazard collision (identical to retail fixed $AD7A)
c $A577 Not yet identified
c $A58B Draws all hazards (identical to retail fixed $ADB8)
c $A6B4 This entry point is used by the routine at #R$8F5F. (identical to retail fixed $AEE1)
c $A83F Not yet identified
c $A8DA Hero car jumps; gear changing; turbos; off road checks; speed adjustment; turning (identical to retail fixed $B107)
c $AABB Not yet identified
c $AB00 Animates the hero car (identical to retail fixed $B32D)
c $AB87 Not yet identified
c $ACF8 Smash handling (identical to retail fixed $B525)
c $AD1B Not yet identified
c $AD2C Draws the debris animation (identical to retail fixed $B559)
c $AD4E Not yet identified
c $AD9B Draws the car (identical to retail fixed $B5C8)
c $AE0C Not yet identified
c $AE29 Draw the hero car's turbo smoke (identical to retail fixed $B656)
c $AE53 Not yet identified
c $AE60 Likely NOT just drawing the cherry (identical to retail fixed $B68D)
c $AE7C Not yet identified
c $AEAC Draw a masked bitmap at (E,D) (identical to retail fixed $B6D9)
c $AED4 Not yet identified
c $AEED Masked sprite plotter (identical to retail fixed $B71A)
c $AF32 Not yet identified
c $AF4C Masked sprite plotter which flips (identical to retail fixed $B779)
c $AFB5 Not yet identified
c $AFCA Masked + inverted sprite plotter (identical to retail fixed $B7F7)
b $AFFB Horizon image related (identical to retail fixed $B828)
c $B01C Not yet identified
c $B11B Horizon stuff / Car jumping stuff (identical to retail fixed $B948)
c $B1BF Not yet identified
c $B218 Copies the back buffer at $F000 to the screen (and sets attributes) (identical to retail fixed $BC49)
c $B393 Clears the playfield then sets its attributes (identical to retail fixed $BDC4)
c $B3CE Not yet identified
c $B5B2 Identical to retail bank0 $C08C (not yet disassembled there)
c $B5E3 Not yet identified
c $B6A3 Identical to retail bank0 $C39F (not yet disassembled there)
c $B6B9 Not yet identified
c $B6DF Identical to retail bank0 $C3DB (not yet disassembled there)
c $B6FD Not yet identified
c $B709 Identical to retail bank0 $C405 (not yet disassembled there)
c $B732 Not yet identified
c $B89F Identical to retail bank0 $C5F9 (not yet disassembled there)
c $B8E7 Not yet identified
c $B8EA Identical to retail bank0 $C644 (not yet disassembled there)
c $B930 Identical to retail bank0 $C68A (not yet disassembled there)
c $B95F Not yet identified
c $BA1B Identical to retail bank0 $C7DA (not yet disassembled there)
c $BA3C Not yet identified
c $BA62 Identical to retail bank0 $C821 (not yet disassembled there)
c $BAD6 Identical to retail bank0 $C895 (not yet disassembled there)
c $BAFF Identical to retail bank0 $C8BE (not yet disassembled there)
c $BB23 Not yet identified
c $BB6C Identical to retail bank0 $CC1E (not yet disassembled there)
c $BBC1 Not yet identified
c $BC10 Identical to retail bank0 $CCC2 (not yet disassembled there)
c $BC2C Not yet identified
c $BC49 Identical to retail bank0 $CCFB (not yet disassembled there)
c $BC72 Not yet identified
c $BCCA Identical to retail bank0 $CD7C (not yet disassembled there)
c $BD08 Not yet identified
c $BD13 Identical to retail bank0 $CDC5 (not yet disassembled there)
c $BD3A Not yet identified
c $C361 Identical to retail bank0 $E9EF (not yet disassembled there)
c $C452 Not yet identified
c $C455 Print-string loop: prints each character at (HL) until the terminator is printed (identical to retail bank3 $FD9F)
c $C47B Not yet identified
c $C47E Identical to retail bank0 $EC20 (not yet disassembled there)
c $C4C2 Not yet identified
c $C4CB Prints a single character: unpacks its position and blits the glyph bitmap to screen (identical to retail bank3 $FE15)
c $C4EA Not yet identified
c $C4EC Interpreter handler: draw graphic frame (reads a byte then a word pointer from the script -- e.g. one of the bitmap_endshot_N pointers at $E104 onward -- calls draw_endshot to blit it, then rejoins run_script's loop) (identical to retail bank7 $E37E)
c $C508 Not yet identified
c $C517 Identical to retail bank0 $ECB7 (not yet disassembled there)
c $C556 Not yet identified
c $C58C Scans the keyboard matrix for a single currently-held key (identical to retail bank3 $FF0C)
c $C5B1 Not yet identified
c $C5D9 Waits for a fresh single keypress, rejecting ambiguous or empty scans (identical to retail bank3 $FF56)
c $C672 Identical to retail bank0 $EE38 (not yet disassembled there)
c $C694 Not yet identified
c $C770 Drum sample players (identical to retail bank7 $F8CD)
c $C78B Not yet identified
c $C798 Identical to retail bank0 $EF5E (not yet disassembled there)
b $C906 Data block at F8F5 (identical to retail bank7 $F9F9)
c $C92E Not yet identified
c $C94C Identical to retail bank0 $F111 (not yet disassembled there)
c $CA5C Not yet identified
c $CA6E Identical to retail bank0 $CE00 (not yet disassembled there)
c $CAA1 Not yet identified
c $CABB Identical to retail bank0 $CE4B (not yet disassembled there)
c $CB4B Not yet identified
c $CC20 Identical to retail bank0 $CFB2 (not yet disassembled there)
c $CC99 Not yet identified
c $CCAD Identical to retail bank0 $D03F (not yet disassembled there)
c $DE4A Not yet identified
c $DE59 Identical to retail bank0 $E364 (not yet disassembled there)
c $DEE7 Not yet identified
c $DEFF Identical to retail bank0 $E8A6 (not yet disassembled there)
c $DF57 Not yet identified
c $E100 Identical to retail bank0 $E400 (not yet disassembled there)
s $E510 Not yet identified
c $E525 Identical to retail bank0 $F5C3 (not yet disassembled there)
s $EE60 Not yet identified
i $EE80 Leftover/padding RAM content; not part of any tape block
c $F000 Paul Owens protection loader
i $F0EC Leftover/padding RAM content; not part of any tape block
