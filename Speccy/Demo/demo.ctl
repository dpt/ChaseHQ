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
b $5CF0 Pointer table indexing into the DEBAUCHER script data at $5E23 (and other cluster assets); its first word is read indirectly by L89E3_7 (LD HL,(L5CF0)) as a scene-drawing source address
b $5D37 [Stage 1] Nancy's perp description (identical to retail fixed $5D45)
T $5D37,40,39:n1 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $5D5F,38,37:n1 "EMERGENCY HERE. JASON THE UGLY PIRATE,"
T $5D85,42,41:n1 "IS FLEEING TOWARDS THE SUBURBS. THE TARGET"
T $5DAF,41,40:n1 "VEHICLE IS A WHITE RELIANT ROBIN... OVER."
T $5DDE,29,28:n1 "'YOK! YOU ARE UNDER ARREST ON"
T $5E00,28,27:n1 "CYSUSPICION OF FIRST DEGREE "
b $5E23 DEBAUCHERY script/command table for the L89E3 interpreter (5-byte entries: byte + two words); begins with the literal ASCII string "DEBAUCHERY" (credit/tag; the final Y is a high-bit-terminated byte, per the T sub-block below) and embeds target words such as $E510
T $5E23,10,9:n1 "DEBAUCHERY"
b $6016 [Stage 4] Map curvature data (identical to retail bank6 $E577)
b $6030 Tail of the Map curvature stream plus its trailing 2-byte Target/loop-back pointer to $6016 (stream start)
b $6035 [Stage 4] Map height data (identical to retail bank6 $E59A)
b $6065 Tail and Target pointer ($6035, back to the Map height stream) followed by the Map lanes stream and its own trailing Target pointer
b $6080 [Stage 4] Map hazards data (identical to retail bank6 $E5FB)
b $609E Tail and Target pointer ($6080, back to the Map hazards stream) followed by the Map left object stream data
b $613D [Stage 4] Map right object data (identical to retail bank6 $E6CF)
b $6170 Tail of the Map right object stream plus a trailing Target/loop-back pointer
b $619D [Stage 1] Ralph the Idaho Slasher's mugshot (identical to retail fixed $638A)
b $6254 LOD-selection pointer table (address-pair and count entries) indexing into the LODs array at $62CF
b $62CF [Stage 1] LODs (identical to retail fixed $64BC)
b $6CD6 Pointer/index table entries (count and address pairs), continuing into the LODs table at $6D32
b $6CE5 [Stage 5] Perp's mugshot (identical to retail bank7 $D9E5)
b $6D13 LOD-selection pointer table (address-pair and count entries) for the LODs array at $6D32
b $6D32 [Stage 1] LODs (identical to retail fixed $703F)
b $6DEA Pointer/index table entries (count and address pairs) indexing frames of the Pilot's mugshot at $6DF5
b $6DF5 [Stage 4] Pilot's mugshot (identical to retail bank6 $EFF5)
b $705A Pointer/index table entries (count and address pairs) indexing frames of the Perp's mugshot at $7063
b $7063 [Stage 5] Perp's mugshot (identical to retail bank7 $D295)
s $73EE Zero-fill (744 bytes)
b $76D6 Identical to retail bank5 $F6D6 (not yet disassembled there)
T $779B,30,29:n1 "'YCHASE H.Q. MONITORING SYSTEM"
T $77BD,5,4:n1 "YTUNE"
T $77C5,8,7:n1 "bZVOLUME"
T $77D1,7,6:n1 "YSIGNAL"
c $8000 Demo's own entry point and per-frame main loop: sets up stack/screen, detects 128K hardware via port $7FFD, then dispatches to the engine subsystems each frame
c $8153 Reveals the perp's car on the pre-game screen (identical to retail fixed $85F5)
c $816D Same routine as retail fixed $860F (Animate the signal meters); differs only in an embedded address literal (data lives at a different offset in this build)
c $81A1 Animate the signal meters (identical to retail fixed $8643)
c $81C0 Same routine as retail fixed $8662 (Draw the pre-game screen); differs only in an embedded address literal (data lives at a different offset in this build)
c $81DC Draw the pre-game screen (identical to retail fixed $867E)
c $8278 Same routine as retail fixed $87DC (Sets up the stage); differs only in an embedded address literal (data lives at a different offset in this build)
c $828F Sets up the stage (identical to retail fixed $87F3)
c $82FE Cluster of helper routines: key-combo/cheat check feeding chatter, key-release wait loop, screen/attribute clear helpers, keep-highest-value tracker, and an indexed jump-table dispatcher (table data at $83C3)
c $83EE Crash sound effect (identical to retail fixed $8963)
c $8469 "Thud" sound effect (identical to retail fixed $89DE)
c $84A1 Same routine as retail fixed $8A16 (Cornering sound effect); differs only in an embedded address literal (data lives at a different offset in this build)
c $84A6 Cornering sound effect (identical to retail fixed $8A1B)
c $84E2 Same routine as retail fixed $8204 (Generates engine noise (48K)); differs only in an embedded address literal (data lives at a different offset in this build)
c $8520 Generates engine noise (48K) (identical to retail fixed $8242)
c $8536 Sets up an oscillating sprite position/attribute value (writes $9CFF/$9D06-$9D08); used by other init routines
c $855C Identical to retail bank0 $F271 (not yet disassembled there)
c $8575 Continues the oscillator setup from L855C, writing wobble parameters to $9CFF/$9D01/$9D06
c $86BB Handle perp caught (identical to retail fixed $8B19)
c $87B9 Same routine as retail fixed $8C17 (Handle perp caught); differs only in an embedded address literal (data lives at a different offset in this build)
c $87F5 Fully smashed (identical to retail fixed $8C53)
B $87F5,11,11
T $8800,24,23:n1 "EYCLEAR BONUS      0,000"
B $8818,6,6
T $881E,23,22:n1 "YTIME BONUS      X 5000"
B $8835,5,5
T $883A,24,23:n1 "EZSCORE                 "
B $8852,8,8
T $885A,28,27:n1 "CYSIGHTING OF TARGET VEHICLE"
B $8876,8,8
T $887E,22,21:n1 "FYOK! PULL OVER CREEP!"
B $8894,8,8
T $889C,11,10:n1 "KYGAME OVER"
B $88A7,8,8
T $88AF,9,8:n1 "LYTIME UP"
B $88B8,8,8
T $88C0,11,10:n1 "LYCONTINUE "
B $88CB,6,6
T $88D1,13,12:n1 "YTHIS MISSION"
B $88DE,6,6
T $88E4,17,16:n1 "YPUSH GEAR BUTTON"
B $88F5,6,6
T $88FB,23,22:n1 "ZBEFORE TIMER REACHES 0"
B $8912,5,5
T $8917,9,8:n1 "LZTIME 10"
B $8920,6,6
T $8926,9,8:n1 "ZCREDIT  "
c $8934 Same routine as retail fixed $8D92 (Drives transitions (the fade between scenes)); differs only in an embedded address literal (data lives at a different offset in this build)
c $8973 Drives transitions (the fade between scenes) (identical to retail fixed $8DD1)
c $899A Same routine as retail fixed $8DF8 (Drives transitions (the fade between scenes)); differs only in an embedded address literal (data lives at a different offset in this build)
c $89CA Sets up a transition (the fade between scenes) (identical to retail fixed $8E28)
c $89E3 Script/table interpreter driving demo sequences (reads command bytes from data tables such as L5CF0, dispatches sprite placement via L9A3F_0)
c $8BEB Draws overhead graphics (identical to retail fixed $908B)
c $8CBA Same routine as retail fixed $915A (Draws overhead graphics); differs only in an embedded address literal (data lives at a different offset in this build)
c $8D0A Draws stretchy objects, such as trees (identical to retail fixed $91AA)
c $8D96 Same routine as retail fixed $9236 (Draws stretchy objects, such as trees); differs only in an embedded address literal (data lives at a different offset in this build)
c $8DDC Draws objects (left hand version) (identical to retail fixed $927C)
c $8E39 Same routine as retail fixed $92D9 (Draws objects (left hand version)); differs only in an embedded address literal (data lives at a different offset in this build)
c $8E45 Draws objects (right hand version) (identical to retail fixed $92E5)
c $9001 Sets up IX and stack pointer for the sprite plotter at L9015
c $9015 Sprite plotter for back buffer, up to 64px wide, 15px high, no mask, no flip (identical to retail fixed $94B5)
c $9092 Same routine as retail fixed $9532 (Sprite plotter for back buffer, up to 64px wide, 15px high, no mask, no flip); differs only in an embedded address literal (data lives at a different offset in this build)
c $90B3 Sprite plotter for back buffer, up to 64px wide, 15px high, no mask, flips (identical to retail fixed $9553)
c $9101 Same routine as retail fixed $95A1 (Routine at 9565); differs only in an embedded address literal (data lives at a different offset in this build)
c $911C Routine at 9565 (identical to retail fixed $95BC)
c $9166 Same routine as retail fixed $9606 (Routine at 9565); differs only in an embedded address literal (data lives at a different offset in this build)
c $917E Random number generator (identical to retail fixed $961E); the code is only the first ~33 bytes, followed by the embedded police-radio chatter dialogue text table (e.g. "THIS IS SPECIAL INVESTIGATION AIRBORNE", "GOTCHA, NANCY BABY") that runs to the end of the block
B $917E,33,33
T $919F,27,26:n1 "THIS IS NANCY AT CHASE H.Q."
T $91BA,39,38:n1 "THIS IS SPECIAL INVESTIGATION AIRBORNE."
T $91E1,29,28:n1 "THE TARGET VEHICLE HAS TURNED"
T $91FE,23,22:n1 "RIGHT UP AHEAD... OVER."
T $9215,22,21:n1 "LEFT UP AHEAD... OVER."
T $922B,29,28:n1 "WE READ LOUD AND CLEAR! OVER."
T $9248,6,5:n1 "ROGER!"
T $924E,19,18:n1 "GOTCHA, NANCY BABY!"
T $9261,23,22:n1 "WHAT ARE YOU DOING MAN!"
T $9278,37,36:n1 "THE BAD GUYS ARE GOING THE OTHER WAY."
T $929D,36,35:n1 "IF YOU KEEP MESSIN' AROUND LIKE THAT"
T $92C1,38,37:n1 "YOUR TIME IS GOING TO RUN OUT... OVER."
T $92E7,21,20:n1 "LET'S GET MOVIN' MAN!"
T $92FC,7,6:n1 "OH, NO!"
T $9303,7,6:n1 "PLEASE!"
T $930A,6,5:n1 "GREAT!"
T $9310,5,4:n1 "OUCH!"
T $9315,21,20:n1 "LET'S GO. MR. DRIVER."
T $932A,9,8:n1 "YAOOOOOW!"
T $9333,10,9:n1 "BEAR DOWN."
T $933D,19,18:n1 "MORE, PUSH IT MORE!"
T $9350,14,13:n1 "ONE MORE TIME."
T $935E,7,6:n1 "OH MAN."
T $9365,9,8:n1 "WHOAAAAA!"
T $936E,7,6:n1 "HARDER!"
T $9375,34,33:n1 "WE THINK YOU PICKED THE WRONG JOB."
T $9397,32,31:n1 "BETTER CHECK THE CLASSIFIED ADS."
T $93B7,34,33:n1 "ONE MORE TRY FOR BEING A GOOD BOY!"
T $93D9,34,33:n1 "YOU'RE A MEDIOCRE DRIVER, BROTHER!"
T $93FB,14,13:n1 "SEE YOU LATER."
c $940A Data table used by L917E; misdisassembles as garbage instructions, not real code
b $94B3 [Messages] Start of stage chatter (identical to retail fixed $81EB)
T $94B4,13,12:n1 "GIDDY UP BOY!"
T $94C1,11,10:n1 "HOLD ON MAN"
c $94CC Same routine as retail fixed $9945 (Start chatter); differs only in an embedded address literal (data lives at a different offset in this build)
c $9580 Shows the chatter - the alerts and remarks from the game's characters (identical to retail fixed $99F9)
c $95AA Same routine as retail fixed $9A23 (Shows the chatter - the alerts and remarks from the game's characters); differs only in an embedded address literal (data lives at a different offset in this build)
c $95FD Noise in/out effect used for mugshots (identical to retail fixed $9A76)
c $9643 Plots a face on the screen (identical to retail fixed $9ABC)
c $9680 Sets font-colour byte then falls into the mini-font plotter at L9684
c $9684 Plot mini font characters (identical to retail fixed $9AFD)
c $9755 Demo sequence/phase state machine driving chatter and score-increment demos (reads/writes $9D15/$9D1C/$9D1D)
c $97EB Add a bonus (identical to retail fixed $9CF6)
c $9806 Same routine as retail fixed $9D11 (Add a bonus); differs only in an embedded address literal (data lives at a different offset in this build)
c $980F Increments the score by (D,E,A) (identical to retail fixed $9D1A)
c $9826 Cycles a BCD score digit (DAA-based increment) then falls into the scoreboard update at L9871; contains a short embedded data table
c $9871 Update scoreboard and flashing lights (identical to retail fixed $9D86)
c $98FF Same routine as retail fixed $9E14 (Draws the turbo sprites and updates the displayed speed, time, distance and score); differs only in an embedded address literal (data lives at a different offset in this build)
c $9927 Draws the turbo sprites and updates the displayed speed, time, distance and score (identical to retail fixed $9E3C)
c $9A2A Same routine as retail fixed $9F3F (Draws the turbo sprites and updates the displayed speed, time, distance and score); differs only in an embedded address literal (data lives at a different offset in this build)
c $9A3F Plots an 8x15 LED font digit to the screen (identical to retail fixed $9F54)
c $9A98 Another draw string entry point? (identical to retail fixed $9FAD)
c $9ADA Same routine as retail fixed $9FEF (Draws a character (to buffer or screen?)); differs only in an embedded address literal (data lives at a different offset in this build)
c $9AE6 Draws a character (to buffer or screen?) (identical to retail fixed $9FFB)
c $9BC4 Same routine as retail fixed $A0D9 (Keyscan); differs only in an embedded address literal (data lives at a different offset in this build)
c $9BE6 Keyscan (identical to retail fixed $A0FB)
c $9C25 Score accumulator buffer (BCD bytes) written by the add-bonus routine at L980F; not real code
g $9C59 Game status buffer entry at A139 (identical to retail fixed $A16B)
c $9E8C Stub feeding the scenery-collision check at L9E8F
c $9E8F Checks for scenery collisions (identical to retail fixed $A3B2)
c $9F82 Same routine as retail fixed $A4A5 (Checks for scenery collisions); differs only in an embedded address literal (data lives at a different offset in this build)
c $9FAC Scenery was hit (identical to retail fixed $A4CF)
c $9FC3 Roadside-object layout helper: checksums a colour table at $E04F then walks object entries, feeding LA019
c $A019 Lays out roadside objects (identical to retail fixed $A5CD)
c $A030 Same routine as retail fixed $A60A (Lays out roadside objects); differs only in an embedded address literal (data lives at a different offset in this build)
c $A037 Counters (identical to retail fixed $A611)
c $A054 Perp-car state dispatcher (checks $9D1A/(IX+7)) feeding LA081 (Perp car behaviour); starts with a short embedded data table
c $A081 Perp car behaviour (identical to retail fixed $A65B)
c $A1BF Collision/crash-sound trigger and hazard/perp-car spawner, feeding LA29A (Spawns cars)
c $A29A Spawns cars (identical to retail fixed $A882)
c $A2B7 Lookup-table lead-in feeding LA2BD (lane-range lookup)
c $A2BD Returns the range of lanes that cars or hazards should spawn within (identical to retail fixed $A8A5)
c $A2E8 Same routine as retail fixed $A8D0 (Hazard handler routine?); differs only in an embedded address literal (data lives at a different offset in this build)
c $A2FF Hazard handler routine? (identical to retail fixed $A8E7)
c $A34B Hazard-hit sound trigger, feeding LA37F (Spawn hazards)
c $A37F Spawn hazards (identical to retail fixed $ABAC)
c $A41E Same routine as retail fixed $AC4B (Test for collision with hazard); differs only in an embedded address literal (data lives at a different offset in this build)
c $A429 Test for collision with hazard (identical to retail fixed $AC56)
c $A4E3 Same routine as retail fixed $AD10 (Checks for hazard collisions); differs only in an embedded address literal (data lives at a different offset in this build)
c $A4F8 Checks for hazard collisions (identical to retail fixed $AD25)
c $A547 Tail-decision stub feeding LA54D (Hazard collision)
c $A54D Hazard collision (identical to retail fixed $AD7A)
c $A577 Same routine as retail fixed $ADA4 (Draws all hazards); differs only in an embedded address literal (data lives at a different offset in this build)
c $A58B Draws all hazards (identical to retail fixed $ADB8)
c $A6B4 This entry point is used by the routine at #R$8F5F. (identical to retail fixed $AEE1)
c $A83F Same routine as retail fixed $B06C (Hero car jumps; gear changing; turbos; off road checks; speed adjustment; turning); differs only in an embedded address literal (data lives at a different offset in this build)
c $A8DA Hero car jumps; gear changing; turbos; off road checks; speed adjustment; turning (identical to retail fixed $B107)
c $AABB Same routine as retail fixed $B2E8 (Hero car jumps; gear changing; turbos; off road checks; speed adjustment; turning); differs only in an embedded address literal (data lives at a different offset in this build)
c $AB00 Animates the hero car (identical to retail fixed $B32D)
c $AB87 Same routine as retail fixed $B3B4 (Animates the hero car); differs only in an embedded address literal (data lives at a different offset in this build)
c $ACF8 Smash handling (identical to retail fixed $B525)
c $AD1B Same routine as retail fixed $B548 (Smash handling); differs only in an embedded address literal (data lives at a different offset in this build)
c $AD2C Draws the debris animation (identical to retail fixed $B559)
c $AD4E Same routine as retail fixed $B57B (Draws the debris animation); differs only in an embedded address literal (data lives at a different offset in this build)
c $AD9B Draws the car (identical to retail fixed $B5C8)
c $AE0C Same routine as retail fixed $B639 (Draws a car part); differs only in an embedded address literal (data lives at a different offset in this build)
c $AE29 Draw the hero car's turbo smoke (identical to retail fixed $B656)
c $AE53 Same routine as retail fixed $B680 (Likely NOT just drawing the cherry); differs only in an embedded address literal (data lives at a different offset in this build)
c $AE60 Likely NOT just drawing the cherry (identical to retail fixed $B68D)
c $AE7C Same routine as retail fixed $B6A9 (This entry point is used by the routine at #R$B318.); differs only in an embedded address literal (data lives at a different offset in this build)
c $AEAC Draw a masked bitmap at (E,D) (identical to retail fixed $B6D9)
c $AED4 Same routine as retail fixed $B701 (Draw a masked bitmap at (E,D)); differs only in an embedded address literal (data lives at a different offset in this build)
c $AEED Masked sprite plotter (identical to retail fixed $B71A)
c $AF32 Same routine as retail fixed $B75F (Masked sprite plotter); differs only in an embedded address literal (data lives at a different offset in this build)
c $AF4C Masked sprite plotter which flips (identical to retail fixed $B779)
c $AFB5 Same routine as retail fixed $B7E2 (Masked sprite plotter which flips); differs only in an embedded address literal (data lives at a different offset in this build)
c $AFCA Masked + inverted sprite plotter (identical to retail fixed $B7F7)
b $AFFB Horizon image related (identical to retail fixed $B828)
c $B01C Same routine as retail fixed $B849 (Scroll the horizon); differs only in an embedded address literal (data lives at a different offset in this build)
c $B11B Horizon stuff / Car jumping stuff (identical to retail fixed $B948)
c $B1BF Clears horizon state bytes then interpolates perspective heights via SP-based reads, feeding LB218 (screen buffer copy)
c $B218 Copies the back buffer at $F000 to the screen (and sets attributes) (identical to retail fixed $BC49)
c $B393 Clears the playfield then sets its attributes (identical to retail fixed $BDC4)
c $B3CE Updates horizon/scenery state ($9D28-$9D5C) and attribute bytes from curvature, feeding LB5E3
c $B5B2 Identical to retail bank0 $C08C (not yet disassembled there)
c $B5E3 Accumulates into $9D40 then drives car-part collision/perspective calculations feeding LB732
c $B6A3 Identical to retail bank0 $C39F (not yet disassembled there)
c $B6B9 Same routine as retail bank0 $C3B5 (not yet disassembled there); differs only in an embedded address literal
c $B6DF Identical to retail bank0 $C3DB (not yet disassembled there)
c $B6FD Same routine as retail bank0 $C3F9 (not yet disassembled there); differs only in an embedded address literal
c $B709 Identical to retail bank0 $C405 (not yet disassembled there)
c $B732 Bresenham-style perspective/edge interpolation for hazard rendering, including a self-modifying stub and per-frame setup entry
c $B89F Identical to retail bank0 $C5F9 (not yet disassembled there)
c $B8E7 Stub feeding the routine at LB8EA (identical to retail bank0 $C644)
c $B8EA Identical to retail bank0 $C644 (not yet disassembled there)
c $B930 Identical to retail bank0 $C68A (not yet disassembled there)
c $B95F Horizon-scroll/curvature toggling and animation-frame counter driver
c $BA1B Identical to retail bank0 $C7DA (not yet disassembled there)
c $BA3C Same routine as retail bank0 $C7FB (not yet disassembled there); differs only in an embedded address literal
c $BA62 Identical to retail bank0 $C821 (not yet disassembled there)
c $BAD6 Identical to retail bank0 $C895 (not yet disassembled there)
c $BAFF Identical to retail bank0 $C8BE (not yet disassembled there)
c $BB23 Initialises horizon-scroll offset tables, called by LB1BF, feeding LBB6C
c $BB6C Identical to retail bank0 $CC1E (not yet disassembled there)
c $BBC1 Same routine as retail bank0 $CC73 (not yet disassembled there); differs only in an embedded address literal
c $BC10 Identical to retail bank0 $CCC2 (not yet disassembled there)
c $BC2C Same routine as retail bank0 $CCDE (not yet disassembled there); differs only in an embedded address literal
c $BC49 Identical to retail bank0 $CCFB (not yet disassembled there)
c $BC72 Same routine as retail bank0 $CD24 (not yet disassembled there); differs only in an embedded address literal
c $BCCA Identical to retail bank0 $CD7C (not yet disassembled there)
c $BD08 Same routine as retail bank0 $CDBA (not yet disassembled there); differs only in an embedded address literal
c $BD13 Identical to retail bank0 $CDC5 (not yet disassembled there)
c $BD3A Options-menu driver: keyscans for menu selections, displays option text via LC452, drives demo/chatter modes
B $BD3A,289,289
T $BE5B,41,40:n1 "WELCOME TO THE 'SINCLAIR USER' MEGA TAPE."
T $BE84,37,36:n1 "THIS IS AN EXCLUSIVELY DESIGNED DEMO,"
T $BEA9,39,38:n1 "OF THE SMASH TAITO COIN OP,  CHASE H.Q."
T $BED0,40,39:n1 "CHASE H.Q. IS NOW AVAILABLE ON CASSETTE,"
T $BEF8,43,42:n1 "AND ON DISC FROM A SOFTWARE STORE NEAR YOU."
T $BF23,45,44:n1 "LOOK OUT FOR OTHER TOP QUALITY OCEAN GAMES..."
T $BF50,22,21:n1 "OPERATION THUNDERBOLT."
T $BF66,17,16:n1 "THE UNTOUCHABLES."
T $BF77,10,9:n1 "AND CABAL."
T $BF81,20,19:n1 "HAVE A ZAPPING XMAS,"
T $BF95,45,44:n1 "AND AN HAPPY 'ITS MY TURN TO USE THE TELE NOW"
T $BFC2,46,45:n1 "BECAUSE I WANT TO PLAY THAT WICKED DOUBLE PLUS"
T $BFF0,25,24:n1 "RAD OCEAN GAME' NEW YEAR."
B $C009,1,1
T $C00A,12,11:n1 "+HCHASE H.Q."
B $C016,2,2
T $C018,19,18:n1 "H1. CONTROL OPTIONS"
B $C02B,2,2
T $C02D,17,16:n1 "H2. GAME FEATURES"
B $C03E,1,1
T $C03F,14,13:n1 "'P3. PLAY DEMO"
B $C04D,2,2
T $C04F,27,26:n1 "PPROGGY BITS...     JOBBEEE"
B $C06A,2,2
T $C06C,27,26:n1 "PBLOBISH STUFF...      BILL"
B $C087,35,35
T $C0AA,28,27:n1 "HCHASE H.Q.   GAME FEATURES"
B $C0C6,2,2
T $C0C8,33,32:n1 "HTHIS CONVERSION FEATURES SOME OF"
B $C0E9,2,2
T $C0EB,26,25:n1 "HTHE SMOOTHEST AND FASTEST"
B $C105,2,2
T $C107,32,31:n1 "H3D DRIVING ACTION YET SEEN ON A"
B $C127,2,2
T $C129,10,9:n1 "PSPECTRUM."
B $C133,1,1
T $C134,33,32:n1 "`PEACH LEVEL CONTAINS A DIFFERENT"
B $C155,2,2
T $C157,33,32:n1 "PCRIMINAL TO BE CAUGHT AND BASHED"
B $C178,2,2
T $C17A,14,13:n1 "POFF THE ROAD."
B $C188,2,2
T $C18A,17,16:n1 "PPRESS ANY KEY..."
B $C19B,2,2
T $C19D,32,31:n1 "!HALL THE ROADSIDE FEATURES HAVE"
B $C1BD,1,1
T $C1BE,18,17:n1 "HHBEEN INCLUDED..."
B $C1D0,2,2
T $C1D2,17,16:n1 "HLOOK OUT FOR THE"
B $C1E3,2,2
T $C1E5,33,32:n1 "HHELICOPTER ON LEVEL 2,THE NARROW"
B $C206,2,2
T $C208,33,32:n1 "HTUNNELS AND THE SPLITTING ROADS."
B $C229,2,2
T $C22B,30,29:n1 "HLISTEN TO THE 6 CHANNEL MUSIC"
B $C249,2,2
T $C24B,13,12:n1 "PAND EFFECTS."
B $C258,1,1
T $C259,31,30:n1 "BPTHERE IS EVEN LOTS OF SAMPLED"
B $C278,1,1
T $C279,30,29:n1 "bPSPEECH TAKEN FROM THE ARCADE"
B $C297,2,2
T $C299,9,8:n1 "PMACHINE."
B $C2A2,2,2
T $C2A4,17,16:n1 "PPRESS ANY KEY..."
B $C2B5,2,2
T $C2B7,27,26:n1 "HALL OF THE ANIMATED INTRO"
B $C2D2,1,1
T $C2D3,34,33:n1 "@HSEQUANCES HAVE ALSO BEEN CRAMMED"
B $C2F5,1,1
T $C2F6,19,18:n1 "gHINTO THE SPECTRUM"
B $C309,2,2
T $C30B,27,26:n1 "HALL THIS AND MORE FOR YOUR"
B $C326,2,2
T $C328,26,25:n1 "HENJOYMENT THIS CHRISTMAS."
B $C342,2,2
T $C344,17,16:n1 "PPRESS ANY KEY..."
B $C355,2,2
T $C357,9,8:n1 "MHOPTIONS"
c $C361 Identical to retail bank0 $E9EF (not yet disassembled there)
B $C361,1,1
T $C362,21,20:n1 "H1. SINCLAIR JOYSTICK"
B $C377,2,2
T $C379,19,18:n1 "P2. CURSOR JOYSTICK"
B $C38C,1,1
T $C38D,22,21:n1 "FP3. KEMPSTON JOYSTICK"
B $C3A3,2,2
T $C3A5,12,11:n1 "P4. KEYBOARD"
B $C3B1,2,2
T $C3B3,15,14:n1 "P5. DEFINE KEYS"
B $C3C2,2,2
T $C3C4,16,15:n1 "IHREDEFINE  KEYS"
B $C3D4,2,2
T $C3D6,13,12:n1 "HGEAR........"
B $C3E3,2,2
T $C3E5,13,12:n1 "HACCELERATE.."
B $C3F2,2,2
T $C3F4,13,12:n1 "PBRAKE......."
B $C401,1,1
T $C402,14,13:n1 ")PLEFT........"
B $C410,1,1
T $C411,14,13:n1 "IPRIGHT......."
B $C41F,2,2
T $C421,13,12:n1 "PQUIT........"
B $C42E,2,2
T $C430,13,12:n1 "PPAUSE......."
B $C43D,2,2
T $C43F,13,12:n1 "PTURBO......."
c $C452 Stub feeding the print-string loop at LC455
c $C455 Print-string loop: prints each character at (HL) until the terminator is printed (identical to retail bank3 $FD9F)
c $C47B Tail-call glue between LC455_1 and LC47E
c $C47E Identical to retail bank0 $EC20 (not yet disassembled there)
c $C4C2 Computes a character glyph offset into the $9D66 table, feeding LC4CB
c $C4CB Prints a single character: unpacks its position and blits the glyph bitmap to screen (identical to retail bank3 $FE15)
c $C4EA Carry-check glue feeding LC4EC_0
c $C4EC Interpreter handler: draw graphic frame (reads a byte then a word pointer from the script -- e.g. one of the bitmap_endshot_N pointers at $E104 onward -- calls draw_endshot to blit it, then rejoins run_script's loop) (identical to retail bank7 $E37E)
c $C508 Attribute row-clear helper feeding LC517
c $C517 Identical to retail bank0 $ECB7 (not yet disassembled there)
c $C556 Menu-item scroll/highlight animation, part of the options-menu machinery
c $C58C Scans the keyboard matrix for a single currently-held key (identical to retail bank3 $FF0C)
c $C5B1 Wait-for-key-selection loop for the options menu, calls LC58C
c $C5D9 Waits for a fresh single keypress, rejecting ambiguous or empty scans (identical to retail bank3 $FF56); the routine ends around $C618, followed by an embedded keyboard-layout label string ("B N M SYSPH J K L ENY U I O P 6 7 8 9 0 5 4 3 2 1 T R E W Q G F D S A V C X Z CP#") used by the redefine-keys option
T $C618,81 "B N M SYSPH J K L ENY U I O P 6 7 8 9 0 5 4 3 2 1 T R E W Q G F D S A V C X Z CP#"
c $C672 Identical to retail bank0 $EE38 (not yet disassembled there)
c $C694 Same routine as retail bank0 $EE5A (not yet disassembled there); differs only in an embedded address literal
c $C770 Drum sample players (identical to retail bank7 $F8CD)
c $C78B Tail-branch glue between LC770 and LC694_18
c $C798 Identical to retail bank0 $EF5E (not yet disassembled there)
b $C906 Data block at F8F5 (identical to retail bank7 $F9F9)
c $C92E Menu-item dispatch checking key/(HL) then falling to LC694_18; followed by an embedded data table
c $C94C Identical to retail bank0 $F111 (not yet disassembled there)
c $CA5C Data table feeding LCA6E
c $CA6E Identical to retail bank0 $CE00 (not yet disassembled there)
c $CAA1 Data table feeding LCABB
c $CABB Identical to retail bank0 $CE4B (not yet disassembled there)
c $CB4B Data table; misdisassembles as OUT/CALL instructions, not real code
c $CC20 Identical to retail bank0 $CFB2 (not yet disassembled there)
c $CC99 Data table feeding LCCAD
c $CCAD Identical to retail bank0 $D03F (not yet disassembled there)
c $DE4A Data table feeding LCCAD/LDE59
c $DE59 Identical to retail bank0 $E364 (not yet disassembled there)
c $DEE7 Data table; misdisassembles as garbage instructions
c $DEFF Identical to retail bank0 $E8A6 (not yet disassembled there)
c $DF57 Data table (mixed byte values, not code)
c $E100 Identical to retail bank0 $E400 (not yet disassembled there)
s $E510 Zero-fill (21 bytes)
c $E525 Identical to retail bank0 $F5C3 (not yet disassembled there)
s $EE60 Zero-fill (32 bytes)
i $EE80 Leftover/padding RAM content; not part of any tape block
c $F000 Paul Owens protection loader
i $F0EC Leftover/padding RAM content; not part of any tape block
