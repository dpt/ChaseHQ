; ChaseHQ.ctl
;
; This is a SkoolKit control file to disassemble the ZX Spectrum 48K version of
; Chase H.Q. by Ocean Software. A home computer version of the arcade game by
; Taito Corporation.
;
; Reverse engineering by David Thomas, 2023.
;
; This is intended to document the game when it is in a "pristine" as-loaded
; state.
;
;
; AUTHORS
; -------
; Code by JOBBEEE aka John O'Brien
; <https://www.mobygames.com/person/28679/john-obrien/>
;
; Graphics by BILL aka Bill Harbison
; <https://www.mobygames.com/person/31804/bill-harbison/>
;
; Music by JON DUNN aka Jonathan Dunn
; <https://www.mobygames.com/person/31702/jonathan-dunn/>
;
;
; OTHER VERSIONS
; --------------
; The Amstrad CPC version is by the same authors.
;
;
; RELATED GAMES
; -------------
; "WEC Le Mans" (1988) - An earlier game by the same authors.
; <https://www.mobygames.com/game/15167/wec-le-mans-24/>
;
; "Batman The Movie" (1989) - The same authors worked on the driving sequences
; for this Atari ST/Amiga game.
; <https://www.mobygames.com/game/3848/batman/>
;
; "Burnin' Rubber" (1990) - A later Amstrad game by JOBBEEE very much like WEC
; Le Mans.
; <https://www.mobygames.com/game/70894/burnin-rubber/>
;
;
; RESEARCH
; --------
; https://www.mobygames.com/game/9832/chase-hq/
;
; http://reassembler.blogspot.com/2012/06/interview-with-spectrum-legend-bill.html
;
; https://spectrumcomputing.co.uk/entry/903/ZX-Spectrum/Chase_HQ
; Known Errors:
; Bugfix provided by Russell Marks:
; "When you finish a stage, a hidden bonus is sometimes given randomly. At
; address $8b3d, instructions EX AF,AF' and LD B,A are in the wrong order.
; Therefore calculation of time bonus is using random value from A' instead of
; low digit from remaining time. Fixed with POKE 35645,71: POKE 35646,8."
;
; https://news.ycombinator.com/item?id=8850193
;
;
; SECRETS
; -------
; Redefine keys to "SHOCKED<ENTER>" to activate test/cheat mode.
;
;
; 48K/128K VERSION DIFFERENCES
; ----------------------------
; - AY music + sampled speech and effects
; - Text animations
; - High score entry
; - Best Officers (high score) on attract screen
; - Input device/define keys is retained
;
;
; MEMORY MAP
; ----------
; "Once running" => once the game's been shifted into its regular running state.
;
; $5B00..$5BFF is a pre-shifted version of the backdrop (once running)
; $5C00..$5CFF is the regular version of the backdrop (once running)
; $F000..$FFFF is a 4KB back buffer (once running)
;
; An back buffer address of 0b1111BAAACCCXXXXX means:
; - X = X position byte (0..31)
; - A = Y position row bits 0..2
; - B = Y position row bit  3
; - C = Y position row bits 4..6
;
; There's a column spare at either side of the back buffer (same as the main
; screen). This seems to be to avoid full clipping.
;
;
; NOTES
; -----
; Strings are top bit set terminated ASCII.
;
;
; THINGS TO LOCATE
; ----------------
; Considering this is just level 1 in 48K:
; - input handlers
; - score, time, speed, distance, gear, stage no., turbo count, credit count
; - music data and player
; - most of car sprites
; - car smoke sprites
; - lamp post sprites
; - tree sprites
; - car's gravity / jump handling
; - collision detection
; - sound effects
; - tunnel handling
; - route/map
; - road drawing code
; - road split handling (it draws both in one pass)
; - floating arrow sprite for road split
; - floating HERE! arrow
; - road signs
; - desert road handling
; - background drawing code (hills in 1st level)
; - attract mode
; - text overlay handling
; - hi score stuff
; - cheat mode ("SHOCKED")
; - smash meter overlay handling
; - redefine keys screen
; - names of keys
; - default key defs A/Z K/L
; - message drawing code (white bar)
; - turbo sprites/anim
; - turbo handling
; - level loading code
; - level format (and which memory regions are altered)
; - helicopter (later level)
; - flashing light on car (and hand)
; - ok creep! auto driving
; - Chase HQ monitoring system
; - picture handling / noise-in-out effect
; - car graphic encoding
; - is attract mode a CPU player or a recording?

@ $4000 start
@ $4000 org
b $4000 Screen memory
B $4000 Screen bitmap
B $5800 Screen attributes

c $5B00 Could be loader code
  $5B22 Point #REGhl at $5B7D
  $5B2B Fetch a word from #REGhl and stack it
  $5B31 Routine that ...
  $5B58 Routine that ...
  $5B4C Game entry point
  $5B66 Routine that ...
  $5B67 Move $5C00..$76EF to $C000 onwards
  $5B75 Routine that loads a word then OUTs $FE with zero, then jumps to that word
N $5B7D Some sort of instruction stream
W $5B7D,2 Probable address of routine $5B31
B $5B7F
B $5B82,2 Probable address of routine $5B75
B $5B84
W $5B87,2 Probable address of routine $5B31
B $5B89
W $5B8D,2 Probable address of routine $5B58
B $5B8F
W $5B91,2 Probable address of routine $5B66
W $5B93,2 Probable address of routine $5B31
B $5B95
W $5B99,2 Probable address of routine $5B58
B $5B9B
W $5B9D,2 Probable address of routine $5B31
B $5B9F
W $5BA3,2 Probable address of routine $5B31
B $5BA5 TBD
B $5BD5 gets IX pointed at it by $5B32

b $5C00 Graphics
;B $5B00,240 Hill backdrop (80x24) seems to be pre-shifted by #R$C8BE  (overlaps game setup)
B $5C00,240 Hill backdrop (80x24)

b $5CF0 TBD
  $5CF0 TBD
W $5CF2,2 Address of face to plot
W $5CF4,2 Screen attributes used for the ground colour (a pair of matching bytes)
  $5CF6 TBD

@ $5D0C label=lods_table
w $5D0C Table of addresses of LODs (levels of detail - a set of sprites of various sizes representing the same object).
  $5D0C,2 -> some1_lods
  $5D0E,2 -> some2_lods
  $5D10,2 -> car_lods
  $5D12,2 -> lambo_lods
  $5D14,2 -> truck_lods
  $5D16,2 -> lambo_lods
  $5D18,2 -> car_lods

b $5D1A TBD

w $5D3A Pointers to Nancy's message (for Stage 1)
  $5D3A,2 Points at "THIS IS NANCY..."
  $5D3C,2 Points at "EMERGENCY HERE..."
  $5D3E,2 Points at "IS FLEEING ..."
  $5D40,2 Points at "VEHICLE IS..."
B $5D42,3 TBD

b $5D45 Messages - Nancy
T $5D45,40 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $5D6D,40 "EMERGENCY HERE. RALPH THE IDAHO SLASHER,"
T $5D95,42 "IS FLEEING TOWARDS THE SUBURBS. THE TARGET"
T $5DBF,46 "VEHICLE IS A WHITE BRITISH SPORTS CAR... OVER."

b $5DED Messages - Arrest
B $5DED,1 Frame delay until first message?
B $5DEE,1 Frame delay until next message?
B $5DEF,1 Flags (?)
B $5DF0,1 Attribute (black)
W $5DF1,2 Back buffer address
W $5DF3,2 Attribute address
T $5DF5,27 "OK! YOU ARE UNDER ARREST ON"
;
B $5E10,1 Frame delay until next message?
B $5E11,1 Flags (?)
B $5E12,1 Attribute (black)
W $5E13,2 Back buffer address
W $5E15,2 Attribute address
T $5E17,26 "SUSPICION OF FIRST DEGREE "
;
B $5E31,1 Frame delay until next message?
B $5E32,1 Flags (?)
B $5E33,1 Attribute (black)
W $5E34,2 Back buffer address
W $5E36,2 Attribute address
T $5E38,6 "MURDER"

b $5E3E Graphics. All are stored inverted except where noted.
  $5E3E,3 TBD
W $5E41,2 Address of tumbleweed LODs table
  $5E43,1 TBD
W $5E44,2 Address of another LOD table
  $5E46 TBD these bytes seem to get hit when we're in the tunnel ... the lights?
  $5E5B hit all the time must be tree/bush defs or LODs
  $5E91 could be lods




B $638A,160,4 Graphic: Perp w/ sunglasses face (32x40). Stored top-down.
B $642A,20,4 Attribute data for perp.
;
; Flags in these LOD structures:
; - 1 seems to indicate interleaved masks
; - 2 is probably to flip
;
@ $643E label=lambo_lods
B $643E,1 Width (bytes)
B $643F,1 Flags
B $6440,1 Height (pixels)
W $6441,2 Bitmap
W $6443,2 Mask
L $643E,7,6
;
@ $6468 label=truck_lods
B $6468,1 Width (bytes)
B $6469,1 Flags
B $646A,1 Height (pixels)
W $646B,2 Bitmap
W $646D,2 Mask
L $6468,7,6
;
@ $6492 label=car_lods
B $6492,1 Width (bytes)
B $6493,1 Flags
B $6494,1 Height (pixels)
W $6495,2 Bitmap
W $6497,2 Mask
L $6492,7,6
;
B $64BC,180,6 Lamborghini_1 (48x30)
B $6570,110,5 Lamborghini_2 (40x22)
B $65DE,45,3 Lamborghini_3 (24x15)
;
B $660B,234,6 Truck_1 (48x39)
B $66F5,145,5 Truck_2 (40x29)
B $6786,60,3 Truck_3 (24x20)
;
B $67C2,186,6 Car_1 (48x31)
B $687C,110,5 Car_2 (40x22)
B $68EA,48,3 Car_3 (24x16)
;
B $691A,24,3 Lamborghini_4 perhaps (24x8)
B $694A,24,3 Lamborghini_5 mask (24x8)
;
B $697A,24 Truck_4/5
B $69AA,24 Truck_4/5 mask
;
B $69DA,27 Car_4/5
B $6A10,27 Car_4/5 mask
;
B $6932 TBD
B $6962 TBD
B $6992 TBD
B $69C2 TBD
B $69C2 TBD
B $69F5 TBD
B $6A2B mystery block
;
@ $6A46 label=some1_lods
B $6A46,1 Width (bytes)
B $6A47,1 Flags
B $6A48,1 Height (pixels)
W $6A49,2 Bitmap
W $6A4B,2 Mask
L $6A46,7,6
;
@ $6A70 label=some2_lods
B $6A70,1 Width (bytes)
B $6A71,1 Flags
B $6A72,1 Height (pixels)
W $6A73,2 Bitmap
W $6A75,2 Mask
L $6A70,7,6
;
B $6A9A,10 bitmap interleaved
B $6AA4,1 TBD
B $6AAE,8 bitmap interleaved
B $6ABE,8 mask
B $6ACE,6 bitmap
B $6ADA,6 mask
B $6AE6,2 bitmap
B $6AEA,2 mask
B $6AEE,1 bitmap
B $6AF0,1 mask
B $6AF2,1 bitmap
B $6AF4,1 mask
B $6AF5 TBD
;
@ $6B22 label=turn_sign_lods
B $6B22,1 Width (bytes)
B $6B23,1 Flags
B $6B24,1 Height (pixels)
W $6B25,2 Bitmap
W $6B27,2 Mask
L $6B22,7,10
;
B $6B68,160,4 Turn right sign (32x40)
B $6C08,90,3 Turn right sign (24x30)
B $6C62,40,2 Turn right sign (16x20)
B $6C8A,32,2 Turn right sign (16x16)
B $6CAA TBD bitmap/mask interleaved
B $6CCA,26,2 Turn right sign (16x13)
B $6CE4 TBD
B $6CFE,26,2 Turn right sign mask (16x13)
;
B $6D18 TBD looks like graphics
;
@ $6D82 label=tumbleweed_lods
B $6D82,1 Width (bytes)
B $6D83,1 Flags
B $6D84,1 Height (pixels)
W $6D85,2 Bitmap
W $6D87,2 Mask
L $6D82,7,6
;
B $6DAC,32,2 Tumbleweed_1 (16x16)
B $6DCC,22,2 Tumbleweed_2 (16x11)
B $6DE2,9,1 Tumbleweed_3 (8x9)
B $6DEB,7,1 Tumbleweed_4 (8x7)
;
@ $6DF2 label=barrier_lods
B $6DF2,1 Width (bytes)
B $6DF3,1 Flags
B $6DF4,1 Height (pixels)
W $6DF5,2 Bitmap
W $6DF7,2 Mask
L $6DF2,7,6
;
B $6E1C,68,4 Barrier (32x17)
B $6E60,39,3 Barrier (24x13)
B $6E87,18,2 Barrier (16x9)
B $6E99,14,2 Barrier (16x7)
B $6EA7 TBD
B $6EB5,14,2 Mask for barrier (16x7)

B $6EC3 TBD

W $6EEB,2 Address of another LOD table

B $6F17,1 Width (bytes)
B $6F18,1 Flags
B $6F19,1 Height
W $6F1A,2 Bitmap
W $6F1C,2 Mask
L $6F17,7,10

B $6F5D,32,4 graphic_6eeb (32x8)
B $6F7D,15,3 graphic_6eeb (24x5)
B $6F8C,12,3 graphic_6eeb (24x4)
B $6F98,12,3 graphic_6eeb mask (24x4)
B $6FA4,8,2 graphic_6eeb (16x4)
B $6FB4,8,2 graphic_6eeb mask (16x4)
B $6FC4,6,2 graphic_6eeb (16x3)
B $6FD0,6,2 graphic_6eeb mask (16x3)



;B $7069 is the top part of a streetlamp
;B $73D4 looks like tree trunk
B $73B9,10 Part of tree graphic - 16px x 5 rows (?)

b $76F0 Used by plot_scores_etc

@ $7798 label=pre_game_messages
b $7798 Pre-game screen messages
;
B $7798,1 flags? attrs?   text is white
W $7799,2 Back buffer address
W $779B,2 Attributes address
T $779D,28 "CHASE H.Q. MONITORING SYSTEM"   text is white
;
B $77B9,1 flags? attrs?   text is yellow
W $77BA,2 Back buffer address
W $77BC,2 Attributes address
T $77BE,4 "TUNE"
;
B $77C2,1 flags? attrs?   text is yellow
W $77C3,2 Back buffer address
W $77C5,2 Attributes address
T $77C7,6 "VOLUME"
;
B $77CD,1 flags? attrs?   text is white
W $77CE,2 Back buffer address
W $77D0,2 Attributes address
T $77D2,6 "SIGNAL"

b $77D8
  $7860 Seems to be tiles pointed at by car rendering code
  $78B6,64 8 Tiles used for borders on the pre-game screen

  $7BE9,160,4 Graphic: Nancy's face (32x40). Stored top-down.
  $7C89,20,4 Attribute data for above.
  $7C9D,160,4 Graphic: Raymond's face (32x40). Stored top-down.
  $7D3D,20,4 Attribute data for above.
  $7D51,1 TBD
  $7D52,160,4 Graphic: Tony's face (32x40). Stored top-down.
  $7DF2,20,4 Attribute data for above.

b $8000 temporaries?
  $8002,4 Score digits as BCD (4 bytes / 8 digits)
  $8006,1 incremented on reset?
  $8007,1 Current/wanted? stage number (1...)

c $8014 Load a stage?
  $8014 Get stage number
  $8017
  $801A,2 Exit if they match
  $801C Equalise
  $801D Set $A220 to level no.
  $8020 Update the level number in "SEARCHING FOR <N>"
  $8025 Call clear_screen_set_attrs
  $8028 Call clear_game_attrs
  $802B Transition type?
  $802D Call setup_transition
  $8030 ...

  $8088 subroutine
N $80B9 Tape loading
  $80B9 Setup to load a block ???
  $80C0 Routine
  $814B Subroutine

t $8169 Mainly tape loading messages
;
B $8169,1 attr?
W $816A,2 Screen position (8,64)
W $816C,2 Screen attribute position (1,8)
  $816E,30 "REWIND TAPE TO START OF SIDE 2"
;
B $818C,1 attr?
W $818D,2 Screen position (88,96)
W $818F,2 Screen attribute position (11,12)
  $8191,10 "START TAPE"
;
B $819B,1 attr?
W $819C,2 Screen position (72,128)
W $819E,2 Screen attribute position (9,16)
  $81A0,15 "SEARCHING FOR 1"
;
B $81AF,1 attr?
W $81B0,2 Screen position (104,160)
W $81B2,2 Screen attribute position (13,20)
  $81B4,7 "FOUND 1"
;
B $81BB,1 attr?
W $81BC,2 Screen position (80,96)
W $81BE,2 Screen attribute position (10,12)
  $81C0,13 "STOP THE TAPE"
;
B $81CD,1 attr?
W $81CE,2 Screen position (88,128)
W $81D0,2 Screen attribute position (11,16)
  $81D2,11 "PRESS  GEAR"
;
B $81DD Unknown
;
  $81EC,13 "GIDDY UP BOY!"
;
  $81F9,11 "HOLD ON MAN"

c $8204 In-game sound effects
  $8246 Produces engine sound effect
  $824E Produces the steering squeak

b $82A6 Attract mode messages
; set to 0 for double height with a gap(?)
; set to 1 for no gap at the top
; set to 2 for single height
; 3 for dbl
; 4 for single height inverted colrs
; 5 for dbl + invt
; 6 as 5 but with the gap
B $82A6,1 Flags (double height)
B $82A7,1 Attribute (black)
W $82A8,2 Back buffer address
W $82AA,2 Attribute address
T $82AC,8 "CHASE HQ"
;
B $82B4,1 Flags (regular)
B $82B5,1 Attribute (black)
W $82B6,2 Back buffer address
W $82B8,2 Attribute address
T $82BA,18 "PRESS GEAR TO PLAY"  (this blinks)
;
; all the rest are red and appear in sequence
;
B $82CC,1 Frame delay until the following messages start being shown (0 = 256)
B $82CD,1 Frame delay until next message?
;
B $82CE,1 Flags (regular)
B $82CF,1 Attribute (red)
W $82D0,2 Back buffer address
W $82D2,2 Attribute address
T $82D4,20 "PROGRAM      JOBBEEE"
;
B $82E8,1 Frame delay?
;
B $82E9,1 Flags (regular)
B $82EA,1 Attribute (red)
W $82EB,2 Back buffer address
W $82ED,2 Attribute address
T $82EF,20 "GRAPHICS        BILL"
;
B $8303,1 Delay + stop flags?  $28
;
B $8304,1 Flags (regular)
B $8305,1 Attribute (red)
W $8306,2 Back buffer address
W $8308,2 Attribute address
T $830A,20 "MUSIC       JON DUNN"
;
; set to 1 => perp + hero mugshots gets drawn
; set to 2 => copyrights don't get shown
; set to 3 => as normal
; set to 4 => starts doing the transition stuff with random bytes
;
B $831E
B $831F
B $8320 (Frame delays as #R$82CC)
;
B $8322,1 Flags (regular)
B $8323,1 Attribute (red)
W $8324,2 Back buffer address
W $8326,2 Attribute address
T $8328,23 "(C) 1989 OCEAN SOFTWARE"
;
B $833F Frame delay?
;
B $8340,1 Flags (regular)
B $8341,1 Attribute (red)
W $8342,2 Back buffer address
W $8344,2 Attribute address
T $8346,26 "(C) 1988 TAITO CORPORATION"
;
B $8360 Delay + stop flags?  $28
;
B $8361,1 Flags (regular)
B $8362,1 Attribute (red)
W $8363,2 Back buffer address
W $8365,2 Attribute address
T $8367,19 "ALL RIGHTS RESERVED"

b $837A unknown

c $83B5
c $83B8
c $83BB
c $83BE
c $83C1
c $83C4
c $83C7
c $83CA

c $83CD
  $83F5 If $A139 is nonzero this calls $81AA which is the end of a string...

c $8401 Possible main game loop?
c $852A
c $858C
b $85DD
c $85E4
c $860F
c $865A
b $86F6

c $87DC
  $8807 Pre-shift the backdrop image
  $8857 Point #REGhl at left light's attributes
  $885D Point #REGhl at right light's attributes then FALL THROUGH
@ $8860 label=clear_lights
  $8860 Clear the lights' BRIGHT bit
; flashing lights are 5 attrs wide, 4 high

c $8876
  $88B1,3 Call fill_attributes

@ $88D5 label=clear_game_attrs
c $88D5 Clears the game screen attributes to zero

@ $88E2 label=clear_game_screen
c $88E2 Clears the game screen attributes and the game screen to zero

c $88F2
c $8903
b $893C
t $8958
b $895B
c $8960
b $897C
c $89D9
b $89EF

c $8A0F Sound effects - "pitt" and "pff" etc.
c $8A36 Sound effects - makes the "bip-bow" time running out effect

c $8A57
  $8ABB,3 Call fill_attributes
  $8B87,3 Point at "CLEAR BONUS - TIME BONUS - SCORE" messages

@ $8C3A label=fully_smashed
c $8C3A
  $8C3C TBD = (draw_screen flag) + 1
  $8C43 Set smash counter to 20
  $8C48 $A16F = $C0
  $8C4D,3 Print the "OK! PULL OVER CREEP!" message
  $8C53
  $8C56

; Screens:
; TIME UP - is solo
; GAME OVER - is solo
; CONTINUE - THIS MISSION - PUSH GEAR BUTTON - BEFORE TIMER REACHES 0 - TIME  <N> - CREDITS  -- together

b $8C58 End of level messages
;
@ $8C58 label=score_messages
B $8C58 TBD
;
B $8C5B,1 Attribute (red)
W $8C5C,2 Back buffer address
W $8C5E,2 Attribute address
T $8C60,22 "CLEAR BONUS      0,000" -- double height
;
B $8C78,1 Attribute (red)
W $8C79,2 Back buffer address
W $8C7B,2 Attribute address
T $8C7D,22 "TIME BONUS      X 5000" -- double height
;
B $8C95,1 Attribute (red)
W $8C96,2 Back buffer address
W $8C98,2 Attribute address
T $8C9A,22 "SCORE                 " -- double height
;
@ $8CB2 label=sighting_message
B $8CB2 TBD
;
B $8CB5,1 Attribute (black)
W $8CB6,2 Back buffer address
W $8CB8,2 Attribute address
T $8CBA,26 "SIGHTING OF TARGET VEHICLE"
;
@ $8CD6 label=pull_over_message
B $8CD6 TBD
;
B $8CD9,1 Attribute (black)
W $8CDA,2 Back buffer address
W $8CDC,2 Attribute address
T $8CDE,20 "OK! PULL OVER CREEP!"
;
@ $8CF4 label=game_over_message
B $8CF4 TBD
;
B $8CF7,1 Attribute (black)
W $8CF8,2 Back buffer address
W $8CFA,2 Attribute address
T $8CFC,9 "GAME OVER"
;
@ $8D07 label=time_up_message
B $8D07 TBD
;
B $8D0A,1 Attribute (black)
W $8D0B,2 Back buffer address
W $8D0D,2 Attribute address
T $8D0F,7 "TIME UP"
;
@ $8D18 label=continue_messages
B $8D18 TBD
;
B $8D1B,1 Attribute (black)
W $8D1C,2 Back buffer address
W $8D1E,2 Attribute address
T $8D20,9 "CONTINUE "
;
B $8D2B,1 Attribute (black)
W $8D2C,2 Back buffer address
W $8D2E,2 Attribute address
T $8D30,12 "THIS MISSION"
;
B $8D3E,1 Attribute (red)
W $8D3F,2 Back buffer address
W $8D41,2 Attribute address
T $8D43,16 "PUSH GEAR BUTTON"
;
B $8D55,1 Attribute (red)
W $8D56,2 Back buffer address
W $8D58,2 Attribute address
T $8D5A,22 "BEFORE TIMER REACHES 0"
;
B $8D72,1 Attribute (black)
W $8D73,2 Back buffer address
W $8D75,2 Attribute address
T $8D77,7 "TIME 10"
;
B $8D80,1 Attribute (black)
W $8D81,2 Back buffer address
W $8D83,2 Attribute address
T $8D85,8 "CREDIT  "

@ $8D8F label=transition
c $8D8F Drives transitions.
  $8D8F Check the flag/counter that fill_attributes resets
  $8D92 Exit early if its zero
  $8D94 Decrement it
  $8D95 If zero, call $8E91 ...
  $8D9D Call fill_attributes
  $8DA0 (self modified)
  $8DB0 Move to next frame? (self modified)
  $8DB4 Set anim address? (self modify)
  $8DBD 8 chunks
@ $8DBF label=transition_loop
  $8DBF Fetch a byte of anim?
  $8DC0 Bank
  $8DC1 Get it into #REGe
  $8DC2 Preserve H in D (which is safe)
  $8DC3 Reset ptr
  $8DC5 Draw a chunk
  $8DC8 Reset ptr
  $8DCA Move up (screen-wise) by 8
  $8DCB
  $8DCD
  $8DCE Draw a chunk
  $8DD1 H = D - 1 -- drop down 1 row
  $8DD3 Unbank
  $8DD4 Next byte of anim
  $8DD5 Loop
  $8DD7 Return
;
@ $8DD8 label=fade_chunk
  $8DD8 Draws an 8-row chunk of the fade in/out effect.
;R $8DD8 I:HL Pointer to start address in back buffer.
;R $8DD8 I:E  Value to OR with the pixels.
  $8DD8 8 rows
@ $8DDA label=fade_row_loop
  $8DDA 6 iterations (of 5 each) = 30 bytes written (~ a scanline)
@ $8DDC label=fade_column_loop
  $8DDC OR #REGe into the screen pixels
  $8DDF Repeat another four times
  $8DF0 Loop
  $8DF2 Move to next start address
  $8DF4 Loop
  $8DF8 Return

@ $8DF9 label=setup_transition
R $8DF9 I:A ?
c $8DF9 Seems to setup a transition.
  $8DF9 Set flags from #REGa
  $8DFA #REGbc = #REGa
  $8DFD Points to some data TBD
  $8E00 Jump over if #REGa was +ve
  $8E03 #REGb = $FF
  $8E04 Points to some data TBD
@ $8E07 label=dunno_jump
  $8E07 Self modify
  $8E0B Call random
  $8E0E Mask 0..3
  $8E10 HL = A * 3 + DE
  $8E17
  $8E18 Self modify
  $8E1B Load DE from HL
  $8E1F Set transition animation start address
  $8E23 4 frames of animation? 4 states?
  $8E25
  $8E28 Return

@ $8E29 label=fill_attributes
c $8E29 Fills the attribute bytes leftwards from column 1.
  $8E29 Screen attribute position (1,8)
  $8E2C Clear #REGb for #REGc to be set in a moment
  $8E2E 16 rows of attributes
  $8E30 Destination address
  $8E32 28 bytes to copy (why isn't this 29?)
  $8E34 Dest++
  $8E35 Fill by 'rolling'
  $8E37 Skip four (32 wide - 28)
  $8E3B Row counter--
  $8E3C Loop until it hits zero
  $8E3E Zero this flag
  $8E41 Return

@ $8E42 label=sub_8e42
c $8E42 Subroutine.
  $8E42 LD HL,$0000   ; self modified by $8E88 only
  $8E45 LD B,$00      ; self modified by ($8E58 increments it), ($8E8D resets it to 1)
  $8E47 DJNZ $8E5C    ;
  $8E49 LD A,$14      ; self modified by $8E4C, $8E51, ($8E84 resets it perhaps)
  $8E4B DEC A         ;
  $8E4C LD ($8E4A),A  ; self modify
  $8E4F RET NZ        ;
  $8E50 LD A,(HL)     ;
  $8E51 LD ($8E4A),A  ; self modify
  $8E54 LD A,($8E46)  ; increment $8E46
  $8E57 INC A         ; affects TIME UP state if disabled
  $8E58 LD ($8E46),A  ;
  $8E5B INC B         ;
@ $8E5C label=sub_8e42_1
  $8E5C INC HL        ;
  $8E5D LD A,(HL)     ;
  $8E5E AND A         ; test flags
  $8E5F JR Z,$8E66    ; exit
  $8E61 CALL $8E6C    ;
  $8E64 JR $8E47      ;
  $8E66 DEC HL        ;
  $8E67 LD A,(HL)     ;
  $8E68 LD ($A231),A  ;
  $8E6B Return

c $8E6C Message printing related. Increments HL then loads A,E,D,C,B from where HL points.

c $8E7E Message printing related, called for TIME UP, CONTINUE, etc. messages.

c $8E91

c $8EB7

c $8EE7

c $8F13

c $8F47

c $8F5F

c $901C

c $9023

c $904B
b $9052

c $916C

c $9171

c $924D

c $9252

c $9278

c $92E1

@ $949C label=plot_sprite
c $949C Sprite plotter for back buffer, up to 64px wide, 15px high, no flip.
R $949C I:A   Plot (A+1)*8 pixels
R $949C I:DE' Stride of bitmap data in bytes
R $949C I:HL  Address in back buffer to plot at
R $949C I:HL' Address of bitmap data
  $949C Use plot_sprite if the bottom bit is set (odd widths)
  $94A5 (~#REGa + 5) is (4 - #REGa) is the number of plot operations to skip
  $94A8 Multiply #REGa by 5: the length of an individual plot operation
  $94AC Move result to #REGbc
  $94AF Add it to #REGix to complete the jump target
  $94B1 Save #REGsp to restore later (self modify)
  $94B5 #REGb = 15 rows to draw, #REGc = 16, an increment value used later
  $94B8 Bank
  $94B9 Jump into body of loop
@ $94BB label=ps_even_loop
  $94BB Bank
  $94BC Next scanline
  $94BE Restore #REGsp (self modified)
  $94C1 Return
  $94C2 Calculate address of next bitmap scanline
  $94C3 Put it in #REGsp (so we can use POP for speed)
  $94C4 Unbank
  $94C6 Jump table
  $94C8 Transfer two bitmap bytes (16 pixels) from the "stack" to screen buffer
  $94CD Transfer another 16 pixels
  $94D2 Transfer another 16 pixels
  $94D7 Transfer another 16 pixels
  $94DB Restore #REGhl
  $94DC Save H in A then H--
  $94DE Extract low four bits of row address
  $94DF If zero we'll need to handle it below, otherwise just loop
  $94E2 H += 16
  $94E5 Decrement the high three bits of row address
  $94E9 No carry, so finish scanline
  $94EC H -= 16
  $94ED Loop back to ps_even_loop
;
@ $94F2 label=plot_sprite_odd
  $94F2 Increment #REGa for upcoming calculation
  $94F3 Point #REGix at start of plot instructions
  $94F7 (~#REGa + 5) is (4 - #REGa) is the number of plot operations to skip
  $94FA Multiply #REGa by 5: the length of an individual plot operation
  $94FE Move result to #REGbc
  $9501 Add it to #REGix to complete the jump target
  $9503 Save #REGsp to restore later (self modify)
  $9507 #REGb = 15 rows to draw, #REGc = 16, an increment value used later
  $950A Bank
  $950B Jump into body of loop
@ $950D label=ps_odd_loop
  $950D Bank
  $950E Next scanline
  $9510 Restore #REGsp (self modified)
  $9513 Return
  $9514 Calculate address of next bitmap scanline
  $9515 Put it in #REGsp (so we can use POP for speed)
  $9516 Unbank
  $9518 Jump table
  $951A Transfer two bitmap bytes (16 pixels) from the "stack" to screen buffer
  $951F Transfer another 16 pixels
  $9524 Transfer another 16 pixels
  $9529 Transfer another 8 pixels
  $952B Restore #REGhl
  $952C Save H in A then H--
N $952E Decrement the screen address
  $952E Extract low four bits of row address
; A &= 0xF   if nz then do next scanline (dec is fine in this case)
; else A was 0, so to step back we need to adjust the 'CCC' bits
  $952F If zero we'll need to handle it below, otherwise just loop
  $9532 H += 16
  $9535 Decrement the high three bits of row address
  $9539 No carry, so finish scanline
; otherwise CCC field was zero
  $953C H -= 16
  $953F Loop back to ps_odd_loop

c $9542
c $9567
c $956E
b $9575
c $9587
c $95B3
c $95D0
c $95D7
b $95DE
c $95E7

@ $9618 rng
c $9618 Random number generator
R $9618 O:A Random byte?
; Disable this and only Lambos spawn on the left. TBD what other effects it has.
; $F0C6 also modifies these random bytes.
B $9618,3 Seed / initial state
  $961B Point at seed / state bytes
  $961E seed[0] -= $8D
  $9622 seed[1] += 3
  $9626 A = seed[0] + seed[1]
  $9628 Rotate A by 1
  $9629 Rotate seed[2] by 1
  $962B seed[2] += A
  $962D Return

g $962E In-game message variables
W $962E,2 Address of the next character of the current message
@ $9630 label=next_message
W $9630,2 Address of the _next_ message to show DOUBTING THIS NOW ... pointer to next command?
  $9632,1 Seems to be the current index of the message bar
  $9633,1 zeroed by $995d
  $9634,5 seems to be the noise used when character pictures 'noise in'. 5 high so this is the height? or a seed?
  $963C,1 Noise effect counter (4..0)
  $963D,1 read by $9946, one'd by $9961, set by $99d9
  $963E,1 read by $9950, set by $9956

b $963F In-game messages
T $963F,27 "THIS IS NANCY AT CHASE H.Q."
T $965A,39 "THIS IS SPECIAL INVESTIGATION AIRBORNE."
T $9681 "THE TARGET VEHICLE HAS TURNED"
T $969E "RIGHT UP AHEAD... OVER."
T $96B5 "LEFT UP AHEAD... OVER."
T $96CB "WE READ LOUD AND CLEAR! OVER."
T $96E8 "ROGER!"
T $96EE "GOTCHA NANCY BABY!"
T $9701 "WHAT ARE YOU DOING MAN!!"
T $9718 "THE BAD GUYS ARE GOING THE OTHER WAY."
T $973D "IF YOU KEEP MESSIN' AROUND LIKE THAT"
T $9761 "YOUR TIME IS GOING TO RUN OUT... OVER."
T $9787 "LET'S GET MOVIN' MAN!"
T $979C "OH, NO!"
T $97A3 "PLEASE!"
T $97AA "GREAT!"
T $97B0 "OUCH!"
T $97B5 "LET'S GO. MR. DRIVER."
T $97CA "YAOOOOOW!"
T $97D3 "BEAR DOWN."
T $97DD "MORE, PUSH IT MORE!"
T $97F0 "ONE MORE TIME."
T $97FE "OH MAN."
T $9805 "WHOAAAAA!"
T $980E "HARDER!"
T $9815 "WE THINK YOU PICKED THE WRONG JOB."
T $9837 "BETTER CHECK THE CLASSIFIED ADS."
T $9857 "ONE MORE TRY FOR BEING A GOOD BOY!"
T $9879 "YOU'RE A MEDIOCRE DRIVER, BROTHER!"
T $989B,14 "SEE YOU LATER."

b $98A9 In-game message structures
;
  $98A9,0 Pilot ($00)
W $98AA,2 -> "THIS IS SPECIAL INVESTIGATION AIRBORNE."
W $98AC,2 -> "THE TARGET VEHICLE HAS TURNED"
W $98AE,2 -> "LEFT UP AHEAD... OVER."
  $98B0,1 TBD $FE
W $98B1,2 -> Tony: "WE READ LOUD AND CLEAR! OVER." <STOP>
;
  $98B3,0 Pilot ($00)
W $98B4,2 -> "THIS IS SPECIAL INVESTIGATION AIRBORNE."
W $98B6,2 -> "THE TARGET VEHICLE HAS TURNED"
W $98B8,2 -> "RIGHT UP AHEAD... OVER."
  $98BA,1 TBD $FE
W $98BB,2 -> Tony: "WE READ LOUD AND CLEAR! OVER." <STOP>
;
  $98BD,1 Three-way random choice ($FC)
W $98BE,2 -> Tony: "WE READ LOUD AND CLEAR! OVER." <STOP>
W $98C0,2 -> Raymond: "ROGER!" <STOP>
W $98C2,2 -> Tony: "GOTCHA NANCY BABY!" <STOP>
;
  $98C4,1 Tony ($03)
W $98C5,2 -> "WE READ LOUD AND CLEAR! OVER."
  $98C7,1 <STOP>
;
  $98C8,1 Raymond ($02)
W $98C9,2 -> "ROGER!"
  $98CB,1 <STOP>
;
  $98CC,1 Tony ($03)
W $98CD,2 -> "GOTCHA NANCY BABY!"
  $98CF,1 <STOP>
;
  $98D0,1 Raymond ($02)
W $98D1,2 -> "WHAT ARE YOU DOING MAN!!"
W $98D3,2 -> "THE BAD GUYS ARE GOING THE OTHER WAY."
  $98D5,1 <STOP>
;
  $98D6,1 Three-way random choice ($FC)
W $98D7,2 -> Raymond: "BEAR DOWN." <STOP>
W $98D9,2 -> Three-way random choice of "OH MAN." / "HARDER!" / "PLEASE!"
W $98DB,2 -> Raymond: "MORE, PUSH IT MORE!" <STOP>
;
  $98DD,1 Raymond ($02)
W $98DE,2 -> "BEAR DOWN."
  $98E0,1 <STOP>
;
  $98E1,1 Raymond ($02)
W $98E2,2 -> "MORE, PUSH IT MORE!"
  $98E4,1 <STOP>
;
  $98E5,1 Raymond ($02)
W $98E6,2 -> "HARDER!"
  $98E8,1 <STOP>
;
  $98E9,1 Raymond ($02)
W $98EA,2 -> "OH MAN."
  $98EC,1 <STOP>
;
  $98ED,1 Three-way random choice ($FC)
W $98EE,2 -> Raymond: "OH MAN." <STOP>
W $98F0,2 -> Raymond: "HARDER!" <STOP>
W $98F2,2 -> Raymond: "PLEASE!" <STOP>
;
  $98F4,1 Raymond ($02)
W $98F5,2 -> "PLEASE!"
  $98F7,1 <STOP>
;
  $98F8,1 Raymond ($02)
W $98F9,2 -> "LET'S GET MOVIN' MAN!"
  $98FB,1 <STOP>
;
  $98FC,1 Nancy ($01)
W $98FD,2 -> "THIS IS NANCY AT CHASE H.Q."
W $98FF,2 -> "IF YOU KEEP MESSIN' AROUND LIKE THAT"
W $9901,2 -> "YOUR TIME IS GOING TO RUN OUT... OVER."
  $9903,1 <STOP>
;
  $9904,1 Three-way random choice ($FC)
W $9905,2 -> Raymond: "OH, NO!" <STOP>
W $9907,2 -> Raymond: "OUCH!" <STOP>
W $9909,2 -> Raymond: "YAOOOOOW!" <STOP>
;
  $990B,1 Raymond ($02)
W $990C,2 -> "OH, NO!"
  $990E,1 <STOP>
;
  $990F,1 Raymond ($02)
W $9910,2 -> "OUCH!"
  $9912,1 <STOP>
;
  $9913,1 Raymond ($02)
W $9914,2 -> "YAOOOOOW!"
  $9916,1 <STOP>
;
  $9917,1 Three-way random choice ($FC)
W $9918,2 -> Tony: "WHOAAAAA!" <STOP>
W $991A,2 -> Tony: "GREAT!" <STOP>
W $991C,2 -> Raymond: "ONE MORE TIME." <STOP>
;
  $991E,1 Tony ($03)
W $991F,2 -> "WHOAAAAA!"
  $9921,1 <STOP>
;
  $9922,1 Tony ($03)
W $9923,2 -> "GREAT!"
  $9925,1 <STOP>
;
  $9926,1 Raymond ($02)
W $9927,2 -> "ONE MORE TIME."
  $9929,1 <STOP>
;
  $992A,1 Three-way random choice ($FC)
W $992B,2 -> Nancy: "WE THINK YOU PICKED THE WRONG JOB." "BETTER CHECK THE CLASSIFIED ADS."
W $992D,2 -> Nancy: "ONE MORE TRY FOR BEING A GOOD BOY!"
W $992F,2 -> Nancy: "YOU'RE A MEDIOCRE DRIVER, BROTHER!" "SEE YOU LATER."
;
  $9931,1 Nancy ($01)
W $9932,2 -> "WE THINK YOU PICKED THE WRONG JOB."
W $9934,2 -> "BETTER CHECK THE CLASSIFIED ADS."
  $9936,1 <STOP>
;
  $9937,1 Nancy ($01)
W $9938,2 -> "ONE MORE TRY FOR BEING A GOOD BOY!"
  $993A,1 <STOP>
;
  $993B,1 Nancy ($01)
W $993C,2 -> "YOU'RE A MEDIOCRE DRIVER, BROTHER!"
W $993E,2 -> "SEE YOU LATER."
  $9940,1 <STOP>
;
  $9941,1 Tony ($03)
W $9942,2 -> "LET'S GO. MR. DRIVER."
  $9944,1 <STOP>

c $9945
  $9959 Set message set pointer
  $995D

c $9965
  $998C,3 Goto plot_mini_font_1
  $99AA using message bar index
  $99B3,3 Goto plot_mini_font_1
  $99B6 using message bar index
  $99C1 End of message
  $99C5 this tests for $FE -- different terminator?
@ $99D3 label=end_of_message ?
  $99D3 Set the noise effect counter to 4
  $99D8 Set $963D to 3
  $99DC Exit via clear_message_line
  $99DF TBD
  $99E4 Call clear_message_line

; message format
; <byte> 0/1/2/3 pilot/nancy/raymond/tony face
;        0 uses $5CF2 as face base - only used for the chopper pilot?
;        $FC => call rnd and use one of three following pointers as an address (repeat)

c $99EC remarks/alerts from nancy etc.
  $99EC Fetch address of next remark
@ $99EF label=loop
  $99EF Read a byte and advance
  $99F1,2 if (byte != $FC) then jump specified_character
@ $99F5 label=random_choice
N $99F6 We have a three-way choice here
  $99F6,3 Call rng
  $99FA if A < $55 jump load_message_ptr   33.3% chance
  $99FE Skip first option message
  $9A00 if A < $AA jump load_message_ptr   66.6% chance
  $9A04 Skip second option message
@ $9A06 label=load_message_ptr
  $9A06 HL = wordat(HL)
  $9A0A goto loop
@ $9A0C specified_character
  $9A0C Is it zero? A's the index of the face to show
  $9A0D Preserve HL
  $9A0E Plot whatever's in $5CF2 if A was zero (doesn't seem to be the noise case, likely dead code), otherwise calculate
  $9A14 Base of face graphics (points 1 earlier than actual base because we're 1-indexed)
  $9A17 Length of face graphic (bitmap + attrs = 4*8*5 + 4*5)
  $9A1A Get face graphic address
@ $9A1D label=do_plot
  $9A1D Set plot address to (176,8)
  $9A20 Call plot_face
  $9A23 Restore HL
@ $9A24 label=message_stuff
  $9A24 Fetch address of message to start showing
  $9A28,3 Save address of next message to show or perhaps the pointer
;
  $9A30,6 if (A == 0) clear_message_line
;
  $9A4E updates message bar index

c $9A55 Noise in/out effect
  $9A55 Decrement the noise effect counter
  $9A59 Jump to $99EC once it's zero
; This entry point is used by the routine at #R$9965.
;
  $9A96 $47 => BRIGHT + white over black
; This entry point is used by the routine at #R$9965.
  $9A98 Screen attribute position (22,1)
  $9A9B 5 rows
  $9A9D 32 - 3 = row skip
@ $9AA0 label=9aa0_loop
  $9AA0 Set four attribute bytes
  $9AA7 Move to start of next row
  $9AA8 Loop while rows remain
  $9AAA Return

@ $9AAB plot_face
R $9AAB I:HL Address of face to plot (32x40 bitmap followed by 4x5 attribute bytes)
R $9AAB I:DE Address of screen location (real screen)
c $9AAB Plots a face
  $9AAB Byte length of bitmap - counter
  $9AAE Save current screen address
@ $9AAF face_loop
  $9AAF Save screen address
  $9AB0 Transfer four bytes
  $9AB8 Restore source address
  $9AB9 If #REGbc became zero then proceed to plotting the attributes (PO => BC == 0)
  $9ABC Move down a scanline
  $9ABD Loop until we've done 8 lines (we've overflowed into the band bits)
  $9AC2 Move down a row (8 lines)
  $9AC6 If carry then loop (overflow is ok)
  $9AC8 Step back 8 lines (undo overflow)
  $9ACC Loop
@ $9ACE plot_face_attrs
  $9ACE Restore current screen address
  $9ACF Extract line bits (0..7)
  $9AD5 Turn it into an attribute address (works for first band only?)
  $9AD8 Byte length of attributes - counter
@ $9ADA plot_face_attrs_loop
  $9ADA Transfer four attributes
  $9AE2 If #REGbc became zero then return
  $9AE3 Move down an attribute row
  $9AE7 If no carry then loop
  $9AE9 Move down 8 attribute rows
  $9AEA Loop

@ $9AEC label=plot_mini_font_1
c $9AEC Plot mini font characters
; $9AEC called constantly to clear? or on flash?
  $9AEC
  $9AEF Goto pmf_go
@ $9AF1 label=plot_mini_font_2
R $9AF1 I:A Index into string, or $FF
  $9AF1 values to self modify with
@ $9AF4 label=pmf_go
  $9AF4
  $9AF5 self modify LD C and OR 7 later
  $9AFD
  $9AFE If not string terminator, goto regular_char
N $9B02 String terminator
  $9B02 $BF / 8?
  $9B04
  $9B06
@ $9B08 label=regular_char
  $9B08 A = A*5+2
  $9B0E 8 - $C0 / 8 ??
  $9B12 divider / rounding?
  $9B19 Modify jump table - A * 4 - 4 bytes per sequence
  $9B21 General multiplier by A
  $9B24 Self modify $9B88 which is a mask
  $9B27 Get ASCII character
N $9B28 Turn ASCII into glyph IDs
  $9B28 LD D,$45      ; likely the screen addr top
  $9B2A Is it '.'? glyph ID = 26; goto have_glyph_id
  $9B30 Is it ','? glyph ID = 27; goto have_glyph_id
  $9B35 Is it '!'? glyph ID = 28; goto have_glyph_id
  $9B3A Is it ' '? glyph ID = 29; goto have_glyph_id
  $9B3F Is it '''? glyph ID = 30; goto have_glyph_id
  $9B44 Is it >= ';'? goto have_ascii
  $9B48 C += A - 47 -- not convinced this is ever used
@ $9B4C label=have_glyph_id
  $9B4C Turn the glyph ID in #REGc into ASCII in #REGa
@ $9B4F label=have_ascii
  $9B4F #REGbc = (#REGa - 'A') * 6
  $9B58 Point #REGhl at minifont
  $9B60 Self modified earlier
  $9B63 Self modified earlier
  $9B66 Jump table (self modified)
; BC must be the row of font data
; shift a 16-bit number right by up to 8  or is the rotation important?
  $9B68
; HL is the screen pointer here
  $9B88 This must be self modified
  $9B8A *scr++ = (*scr AND A) OR B
  $9B8E *scr-- = C
  $9B90 Next source byte ?
N $9B91 Usual scanline stepping magic
  $9B91 Next row
  $9B92
  $9B93
  $9B95
  $9B97
  $9B98
  $9B9A
  $9B9B
  $9B9D
  $9B9E
  $9BA0
  $9BA1 Decrement row counter
  $9BA3 Loop while rows remain
  $9BA6 Return

@ $9BA7 label=clear_message_line
c $9BA7 Clear the whole message line
  $9BA7 Point #REGhl at screen coordinate (8,53)
  $9BAA B = 0  top of LDIR count and byte to wipe memory with
  $9BAC Clear six rows
@ $9BAE label=cml_loop
  $9BAE
  $9BAF DE = HL + 1
  $9BB2 29 bytes to clear
  $9BB4 Preserve HL
  $9BB5 Zero 29 bytes at DE
  $9BB8
  $9BB9 H++
  $9BBA if (H & 7) goto cml_continue
  $9BBF L += 32
  $9BC3 JR C,cml_continue
  $9BC5 H -= 8
@ $9BC9 label=cml_continue
  $9BC9
  $9BCA Decrement row counter
  $9BCB Loop while rows remain to clear
  $9BCE Return

c $9BCF
  $9C1B,3 Point at "TIME UP" message
N $9C57 Resetting mission code.
  $9C57 Reset $A229
  $9C5B Reset perp smash flag count thing to zero
  $9C60 Reset smash counter to zero
  $9C62 $A16F = $FF
  $9C67 $A252 = 3
  $9C6A $A231 = 3 -- Flag set to zero when attributes have been set
  $9C6D $A170 = 3 -- Used in plot_scores_etc
  $9C70 Set remaining time to 60 (BCD)
  $9C7E,3 Point at "CONTINUE THIS MISSION" messages

c $9CC2

c $9CD6

c $9CF8

c $9D2E

t $9D51 Text
@ $9D57 label=hi
T $9D57 "HI"
@ $9D59 label=lo
T $9D59 "LO"
@ $9D5B label=stage
T $9D5B "STAGE " message shown in the score area.
@ $9D61 label=stage_n
B $9D61 #R$9D61 writes the current score number here in ASCII.

c $9D62
  $9D62 Check if stage has changed?
  $9D6B Adding 48 to make it a digit then $80 to terminate the string.
  $9D6D Update "STAGE N"
  $9D73 Screen pixel (48,36)
  $9D76 Point at stage text
  $9D79 Draw it
  $9D7C Is the bonus flag set?
  $9D82 Clear it
  $9D86 Screen pixel (64,24)
  $9D89 Clear the area 5*7 bytes
  $9D9E Draw it
  $9DB4 42 => BRIGHT + red over black
  $9DB8 46 => BRIGHT + yellow over black
  $9DBA Point at attributes (8,3)
  $9DBD Set them to #REGc
  $9DC3
  $9DCD Screen pixel (118,36)
  $9DD0 "LO"
  $9DD6 "HI"
  $9DD9 Draw it

  $9DE6 Point #REGhl at left light's attributes
  $9DE9 Toggle its brightness
  $9DEC Point #REGhl at right light's attributes
  $9DEF Toggle its brightness

@ $9DF4 label=toggle_light_brightness
c $9DF4 Toggle the light's BRIGHT bit (#REGhl -> attrs)
  $9DF4 B = 4, C = $40 (BRIGHT bit)
  $9DF7 Toggle attribute byte on five successive locations
  $9E0A Move to next attribute row
  $9E0E Repeat for four rows

@ $9E11 label=plot_scores_etc
c $9E11
@ $9E7B label=plot_scores_only
  $9E7B Point #REGde at speed digits screen position
  $9EB6 Plot a digit
  $9EBA Plot a digit
  $9EBE Plot a digit
  $9F03 Point #REGde at distance digits screen position (136,33)
  $9F12 Point #REGde at score digits screen position
  $9F16 Point #REGde at score (four BCD bytes)
  $9F19 Point #REGhl at score digits (eight bytes)
  $9F1E C = A = *DE; A >>= 4; if (A == *HL) ...
  $9F1F Keep for later
  $9F26 If different than stored then plot
  $9F2D Examine next digit
  $9F30 If different than stored then plot
  $9EC1 Point #REGde at time digits position
  $9F3B Update new digit
  $9F3C Done
  $9F3C Plot a digit
  $9F45 Plot a digit
  $9F3F Next digit (second half of a pair)
  $9F45 Next digit (next pair)

@ $9F47 label=ledfont_plot
c $9F47 Plots an LED font digit
;(A = index, DE -> scr ptr)  vertical would need to be 1, 9, 17, ..
  $9F48 HL = &ledfont[A * 15]
  $9F55 save the scr ptr
  $9F56 save D
  $9F57 transfer a byte
  $9F59 next row
  $9F5A step back
  $9F5B transfer 7 rows
  $9F71 restore D
  $9F73 E += 31
  $9F77 transfer another 8 rows
  $9F96 move to next column

c $9F99

@ $9FA3 label=draw_string
c $9FA3 Draws a string
N $9FA3 The string is terminated by setting the topmost bit of the final character.
  $9FA6 Load a byte ahd mask off the text part
  $9FAE Was bit 7 set?, quit if so, otherwise loop

@ $9FB4 label=draw_char
c $9FB4 Draws a character
  $9FEC Point #REGhl at 8x7 font

@ $A03D label=double_height_glyph
c $A03D Plots double-height glyphs. DE->screen HL->font def

c $A0CC
B $A0CC
@ $A0D6 label=keyscan
  $A0D6 Main keyboard handler.
  $A112 Outer keyboard loop.
  $A11E Inner keyboard loop. #REGa = ?
  $A11F ...B=(A&7)+1, C=5-(A>>3)
; rotate $FE by B
; IN that port
; rotate the result by C

g $A139
  $A139,1 This is always zero, yet the code checks it. If it's set then $83F5 uses it to jump to $81AA, which is the end of a string. $8435 tests it too and calls $9945 when it's zero, which it always is. $8AD6 also tries to use it to jump into a string (twice). Could be dead code or 128K version hook?
  $A13A,1 Current stage number
  $A13B,1 Used by $8424
  $A16E,1 used during attract mode, road gradient/angle or something?
  $A16F,1 Set to $C0 when fully smashed
  $A170,1 used in plot_scores_etc. set to 3 by $9C65.
W $A171,2 seems to be the horizon level, possibly relative (used during attract mode)
  $A174,1 Low/high gear flag?
  $A175,8 Score digits. One digit per byte, least significant first. This seems to be recording what's on screen so digit plotting can be bypassed.
@ $A175 label=score_digits
  $A175,8 Score. Stored as one digit per byte.
@ $A17E label=time_bcd
  $A17E,1 Time remaining. Stored as BCD.
@ $A17F label=time_digits
  $A17F,2 Time remaining. Stored as one digit per byte.
@ $A181 label=distance_digits
  $A181,4 Distance. Stored as one digit per byte.
W $A186,2 Attribute address of horizon. Points to last attribute on the line which shows the ground. (e.g. $59DF)
  $A188,1
W $A195,2 Set to $190 by fully_smashed
  $A220,1 Set to $F8 when the CHQ MONITORING SYSTEM screen is being shown. (draw_screen uses this to skip for some work, just tests for non-zero). #R$8014 sets this to the level number.
  $A223,1 Stage number as shown. Stored as ASCII.
  $A229,1
  $A22A,1 Used by $B6D6 and others
  $A22C,1 Bonus flag (1 triggers the effect)
  $A22D,1 Counter for bonus effect (goes up to 7?)
  $A22E,1 ... light toggle flashing related
  $A22F,1
  $A230,1 Used by draw_screen
  $A231,1 Transition control/counter. Set to zero when fill_attributes has run.
  $A232,1 Set to 2 when the perp is fully smashed, set to 6 when pulled over
  $A233,1 Smash counter (0..20)
  $A234,1 Cycles 0/1/2/3 as the game runs.
  $A235,1 ... light toggle flashing related? seems to cycle 0/1 as the game runs.
  $A236,1
  $A240,1 Used by $B8F6, $BBF7
  $A249,1 Counter used by $BB6E
  $A24A,1 Used by $B848
  $A24E used in plot_scores_etc
  $A250,1
  $A251,1
  $A252,1
  $A253,1 Low/high gear flag? or visual state?
  $A258,1 Used by $B8D8
  $A259,1 Used by $B92B
  $A256,1 Distance related perhaps?
  $A25A,1 Used by $B8D2
  $A25C,1 Used by $B84E
  $A25D,1 Used by $B85D
  $A268,1 Used by $BB69 - skips routine if not set
  $A26B,1 Used by $8432

b $A27A Font: 8x7 bitmap
B $A27A Exclamation mark
B $A282 Open bracket
B $A28A Close bracket
B $A292 Comma
B $A29A Full stop
B $A2A2 0..9
B $A2F2 A-Z

c $A399
c $A579
c $A60E
b $A623
c $A637
b $A7E7
c $A7F3
c $A89C
c $A8CD
c $A955
c $A97E
c $A9DE
b $AA38
c $AAC6

c $AB33
  $AB52 Point #REGhl at pilot "turn left" messages
  $AB59 Point #REGhl at pilet "turn right" messages

c $AB9A
c $ABF6
c $AC3C
b $ACDB
c $AD0D
c $AD4B
c $AD51
c $ADA0
c $ADBE
c $AECF
c $AFF1
b $B045
c $B063
c $B318
c $B457

c $B4CC
  $B4E1 Point #REGhl at left light's attributes
  $B4E4 Toggle its brightness
  $B4E7,3 Point at "SIGHTING OF TARGET VEHICLE" message
  $B4EA
  $B4ED

c $B4F0

c $B549

c $B58E Draws the car
  $B6C4 Read from SUB @ $B5AA
  $B5FE Hit while drawing car. loads values then calls $B627
  $B596 Point HL at ? then add (A * 4)
  $B605 Hit while drawing car. draws wheels
  $B610 Draws left side of car
  $B627 Subroutine
  $B641 Draws right side of car

c $B648
  $B6DE,5 Divide by 8

c $B67C
;
  $B701 Select a plotter? Could be clipping stuff too.
  $B708 This multiplies by six - the length of each load-mask-store step in the plotter core.
  $B70C Add that to #REGix.
  $B711 Set loop counter for 15 iterations?
  $B714

@ $B716 label=masked_tile_plotter
c $B716 Save #REGsp for restoration on exit
@ $B71C label=masked_tile_plotter_loop
  $B71F Restore original #REGsp
@ $B724 label=masked_tile_plotter_entry
  $B729 Masked tile plotter. The stack points to pairs of bitmap and mask bytes and HL must point to the screen buffer. Uses AND-OR type masking. Proceeds left-right. Doesn't flip the bytes so there must be an alternative for that.
@ $B729 label=masked_tile_plotter_core_thingy
  $B729 Load a bitmap and mask pair (D,E)
  $B72A Load the screen pixels
  $B72B AND screen pixels with mask
  $B72C OR in new pixels
  $B72D Store back to the screen
  $B72E Move to next screen pixel
  $B72F Repeat 8 times
  $B758 Handle end of row. This must be adjusting the screen pointer.

c $B76C
  $B771 Save #REGsp for restoration on exit
  $B775 Point #REGix at ??? table
  $B779 Subtract 8 ???
  $B77C,3 Multiply by 8
  $B77F Add to IX
  $B784,2 HEF
B $B79C TBD
N $B7A4 Masked tile plotter with flipping
  $B7A4 Load a bitmap and mask pair (B,C)
  $B7A5 Set flip table index (assuming table is aligned)
  $B7A6 Load the screen pixels
  $B7A7 AND screen pixels with flipped mask
  $B7A8 Set flip table index
  $B7A9 OR in new flipped pixels
  $B7AA Store back to the screen
  $B7AB Move to next screen pixel (downwards in memory)
  $B7AC Repeat 8 times
  $B7DB Handle end of row. This must be adjusting the screen pointer.

c $B7EF
b $B828
c $B848
c $B8D2
c $B9F4
c $BB69

@ $BC3E label=draw_screen
c $BC3E Copies the backbuffer at $F000 to the screen.
  $BC3E Point #REGhl at screen pixel (136,64). This is positioned halfway across so we can PUSH to the screen via SP.
  $BC42 Point #REGhl' at backbuffer + 1 byte.
  $BC45 Self modify #REGsp restore instruction
;
;* Spectrum screen memory has the arrangement:
;* 0b010BBLLLRRRCCCCC (B = band, L = line, R = row, C = column) B > R > L > C
;
; Stack-wise: PUSH decrements, POP increments
N $BC49 A sequence that transfers 16 bytes
  $BC49 Point #REGsp at the back buffer
  $BC4A Pull in 16 bytes from the back buffer (order: AF' DE BC AF' DE' BC' IX IY), incrementing SP
  $BC56 Point #REGsp at screen
  $BC57 Push out 16 bytes to the screen (reversing the order), decrementing SP
  $BC5E,1 Advance screen pointer
  $BC5F Continue pushing
  $BC64 Advance back buffer pointer
;
  $BC65 Transfer another 16 bytes (128 pixels)
  $BC81 Transfer another 16 bytes (128 pixels)
  $BC9D Transfer another 16 bytes (128 pixels)
;
  $BCB9 is the back buffer row a multiple of 4?
  $BCBB loop if so
  $BCBE Advance back buffer pointer (backwards) by a half row?
  $BCC3 Advance screen pointer (backwards) by half a row?
;
N $BCC8 This is a similar sequence but only moves 14 bytes (not using IY)
  $BCC8 Point #REGsp at the back buffer
  $BCC9 Pull in 16 bytes from the back buffer (order: AF' DE BC AF' DE' BC' IX), incrementing SP
;
  $BCE0 Transfer another 14 bytes (112 pixels)
  $BCF8 Transfer another 14 bytes (112 pixels)
  $BD10 Transfer another 14 bytes (112 pixels)
;
  $BD28 Is the back buffer row multiple of 4?
  $BD2A
;
  $BD2D Is the back buffer row multiple of 8?
  $BD2F
;
  $BD31 L -= $F0, H = $F0
  $BD33
  $BD34
  $BD35
;
  $BD36 L >= $F0
  $BD39 PE = parity even
;
  $BD3C
  $BD3D Increment the screen address
  $BD40
  $BD41
;
  $BD42
;
@ $BD45 label=draw_screen_bd45
  $BD45
  $BD46 Point #REGhl at screen pixel (136,128).
  $BD49
  $BD4A Copy more
;
@ $BD4D label=draw_screen_bd4d
  $BD4D
  $BD4E
  $BD51 Which one are we modifying here?
  $BD52
  $BD53
  $BD54 Ditto, which?
  $BD56
  $BD57
;
@ $BD5A label=draw_screen_attributes
  $BD5A Don't update the attributes if the level intro screen is being shown
  $BD60
  $BD63 Seems to be related to the horizon level
  $BD64
  $BD65
  $BD66
  $BD67
  $BD68 Jump to exit check if both are zero
  $BD69
  $BD6B E = A * 4
  $BD6E A = $FF if carry set, zero otherwise
  $BD6F D = A
  $BD70 Fetch the address of the first line of ground attributes (+ 31)
  $BD73 Set the sky colour screen attributes (always black over bright cyan)
  $BD76 If A was zero then jump (Z => sky, NZ => ground)
  $BD78 Load the ground colour screen attributes (varies per level)
  $BD7C Point #REGsp at the screen attributes (DE = $FFE0 = -32)
  $BD7E Fill 30 bytes - length of attribute line minus the two blank edges
  $BD8D
  $BD8F Move to next line of attributes
  $BD90 Save the address of the first line of ground attributes (+ 31)
;
  $BD93 Exit if $A22E is zero (flashing lights / smash mode)
;
  $BD99
  $BD9C
  $BD9E Jump if >= 3
N $BDA0 Set the smash meter attributes
  $BDA0 Screen attribute (2,11)
  $BDA3 = 32
  $BDA6 Black over bright red
  $BDA8 Set two attrs
  $BDAC Black over bright magenta
  $BDAE Set two attrs
  $BDB2 Black over bright green
  $BDB4 Set two attrs
  $BDB8 Black over bright white
  $BDBA Set two attrs
;
@ $BDBD label=draw_screen_exit
  $BDBD Restore #REGsp (self modified at start of routine)
  $BDC0 Return

@ $BDC1 label=clear_screen_set_attrs
c $BDC1 Clears screen then sets in-game attributes
  $BDC1 Call clear_game_screen
  $BDC4 Clear the game screen pixels to $FF (redoes similar work just done)
  $BDD1 Clear the game screen attributes to $28 (black over cyan) - first two rows only
  $BDDD Clear the next three rows to $68 (black over bright cyan)
  $BDE3 Clear the next 11 rows to the current ground colour
  $BDEC Clear the edges of the game screen to black on black
  $BDFA Return

c $BDFB
c $C0E1
c $C15B
b $C223
c $C22A
b $C238
c $C23F
c $C2E7
c $C452

; $C551 seems to be part of the stripes rendering

c $C57C
c $C58A
c $C598
c $C60C
c $C61D

c $C62E
;$C635 does the road plotting
  $C7EA,3 Point at first hill backdrop
  $C7EF,3 Point at second hill backdrop

c $C813
c $C821
c $C86E

@ $C8BE label=pre_shift_backdrop
c $C8BE Builds the pre-shifted first hill backdrop
; Q. How much is it shifting it by?
  $C8BE Point at second hill backdrop
  $C8C1 Point at first hill backdrop
  $C8C4
  $C8C5
  $C8C6 Each is 250 bytes long
  $C8C9 Copy 2nd to first
  $C8CB
  $C8CC Jump 10 bytes fwd
  $C8CF
  $C8D0
  $C8D1 pop old DE
  $C8D2 10 x 24
  $C8D5
  $C8D6
  $C8D7 Nibble swap thing
  $C8D9
  $C8DA
  $C8DB
  $C8DD
  $C8DE
  $C8DF
  $C8E2

c $C8E3
c $C93E
c $C94C
c $C95A
b $CA13
c $CA17
b $CA24
c $CA2A
b $CA35
c $CA36
b $CA46
c $CA4F
c $CA57
b $CBA4
c $CBC5
c $CBCE
c $CBD6
c $CD3A
c $CDD6
b $CDEC
b $CEDA

b $CF8E

b $D20E Smoke plume. Perhaps 64x64 in total, but includes masks.

b $D40D Middle sections of hero car. 9 sets of 40x15? Could be different sizes.
B $D40D Section 1 - 40x13 px
B $D44E Section 2
; further ones follow

b $DD6A Components of the car graphic or its shadow?. Straight on. Byte pairs of value and mask. three sets of 14 x 8 UDGs.
b $DE12 Same, but turning.
b $DEBA Same again, but turning hard.

@ $DF62 label=ledfont
b $DF62 LED numeric font used for scores
B $DF62,150,15 8x15 pixels digits 0..9 only

@ $DFF8 label=minifont
b $DFF8 Mini font used for in-game messages
B $DFF8,174,6 8x6 pixels (though the digits are thinner than 8) A-Z + (probably 3 symbols)

b $E1B8

b $E2D2
b $E2D5
b $E2D9
b $E2DD
b $E2E2
b $E2E7
b $E2F3

b $E34B
B $E34B,3 3 bytes set to 8 by $880A

b $E35D (roughly) spiral inward animation mask used for transitions
b $E3BD circle expanding animation mask used for transitions
b $E540 looks like a table of flipped bytes (but not quite)

b $E601

c $E839 looks like initialisation code / relocation
  $E839 POP BC        ; must be the loop counter
  $E83A Point #REGhl at $e858 table
@ $E83D label=loopy
  $E83D PUSH BC       ;
  $E83E DE = wordat(HL) HL += 2
  $E842 Stack DE
  $E843 DE = wordat(HL) HL += 2  ; dest
  $E847 BC = wordat(HL) HL += 2  ; count
  $E84B exchange top of stack and HL. HL is now src
  $E84C Do copy
  $E84E Restore base pointer and count
  $E850 Loop
  $E852 LD SP,$ED28   ; odd
  $E855 JP $83CD      ; odd
;
@ $E858 label=e858_copy_blocks
W $E858,2 src ptr
W $E85A,2 dst ptr
W $E85C,2 count
;
W $E85E,2 src ptr
W $E860,2 dst ptr
W $E862,2 count
;
W $E864,2 src ptr
W $E866,2 dst ptr
W $E868,2 count
;
W $E86A,2 src ptr
W $E86C,2 dst ptr
W $E86E,2 count
;
W $E870,2 src ptr
W $E872,2 dst ptr
W $E874,2 count
;
B $E876,24,3
B $E88E,24,3
B $E8A6,40,8 looks circular?
B $E8CE,49,8
;
B $E8FF TBD
W $E90A,2 -> "STOP THE TAPE" + "PRESS ANY KEY" message set
B $E90C TBD

c $E90F
  $E928 Point #REGhl at input menu messages
  $E931 Keyscan for 1, 2, 3, 4, 5
  $E93A Keyscan for 0, 9, 8, 7, 6
  $E93D Keyscan for P, O, I, U, Y
  $E940 Keyscan for ENTER, L, K, J, H
  $E943 Keyscan for SPACE, SYM SHFT, M, N, B

b $E9B4 Messages
;
B $E9B4,1 Attribute: Green ink over black
W $E9B5,2 Screen position (80,96)  (why is this screen, not attrs?)
T $E9B7,13 "STOP THE TAPE"
;
B $E9C4,1 Attribute: Cyan ink over black
W $E9C5,2 Screen attribute position (4,18)
T $E9C7,25 "PRESS ANY KEY TO CONTINUE"
;
B $E9E0,1 Stop marker?
;
B $E9E1,1 Attribute: Green ink over black
W $E9E2,2 Screen position (88,80)
T $E9E4,11 "CHASE  H.Q."
;
B $E9EF,1 Attribute: Cyan ink over black
W $E9F0,2 Screen position (48,112)
T $E9F2,20 "1. SINCLAIR JOYSTICK"
;
B $EA06,1 Attribute: Cyan ink over black
W $EA07,2 Screen position (48,128)
T $EA09,18 "2. CURSOR JOYSTICK"
;
B $EA1B,1 Attribute: Cyan ink over black
W $EA1C,2 Screen position (48,144)
T $EA1E,20 "3. KEMPSTON JOYSTICK"
;
B $EA32,1 Attribute: Cyan ink over black
W $EA33,2 Screen position (48,160)
T $EA35,11 "4. KEYBOARD"
;
B $EA40,1 Attribute: Cyan ink over black
W $EA41,2 Screen position (48,176)
T $EA43,14 "5. DEFINE KEYS"
;
B $EA51,1 Stop marker?
;
B $EA52,1 Attribute: Red ink over black
W $EA53,2 Screen position (72,80)
T $EA55,14 "REDEFINE  KEYS"
;
B $EA63,1 Attribute: Bright yellow ink over black + Single height bit
W $EA64,2 Screen position (72,112)
T $EA66,12 "GEAR........"
;
B $EA72,1 Attribute: Bright yellow ink over black + Single height bit
W $EA73,2 Screen position (72,120)
T $EA75,12 "ACCELERATE.."
;
B $EA81,1 Attribute: Bright yellow ink over black + Single height bit
W $EA82,2 Screen position (72,128)
T $EA84,12 "BRAKE......."
;
B $EA90,1 Attribute: Bright yellow ink over black + Single height bit
W $EA91,2 Screen position (72,136)
T $EA93,12 "LEFT........"
;
B $EA9F,1 Attribute: Bright yellow ink over black + Single height bit
W $EAA0,2 Screen position (72,144)
T $EAA2,12 "RIGHT......."
;
B $EAAE,1 Attribute: Bright green ink over black + Single height bit
W $EAAF,2 Screen position (72,160)
T $EAB1,12 "QUIT........"
;
B $EABD,1 Attribute: Bright green ink over black + Single height bit
W $EABE,2 Screen position (72,168)
T $EAC0,12 "PAUSE......."
;
B $EACC,1 Attribute: Bright green ink over black + Single height bit
W $EACD,2 Screen position (72,176)
T $EACF,12 "TURBO......."
;
B $EADB,1 Probably a stop marker
;
B $EADC,5 Seems to be an empty message structure
;
B $EAE1,1 Attribute: Bright blue over black + Single height bit
W $EAE2,2 Screen position (0,0)
T $EAE4,4 "TEST"
;
B $EAE8,1 Attribute: Red ink over black
W $EAE9,2 Screen position (32,72)
T $EAEB,24 "CHASE H.Q.     TEST MODE"
;
B $EB03,1 Attribute: Bright cyan ink over black + Single height bit
W $EB04,2 Screen position (0,128)
T $EB06,10 "IN GAME..."
;
B $EB10,1 Attribute: Bright green ink over black + Single height bit
W $EB11,2 Screen position (8,144)
T $EB13,29 "PRESS 1....... RESTART LEVEL."
;
B $EB30,1 Attribute: Bright green ink over black + Single height bit
W $EB31,2 Screen position (56,152)
T $EB33,20 "2....... NEXT LEVEL."
;
B $EB47,1 Attribute: Bright green ink over black + Single height bit
W $EB48,2 Screen position (56,160)
T $EB4A,20 "3....... END SCREEN."
;
B $EB5E,1 Attribute: Bright green ink over black + Single height bit
W $EB5F,2 Screen position (56,168)
T $EB61,22 "4....... EXTRA CREDIT."
;
B $EB78,1 Attribute: Red
W $EB79,2 Screen position (88,80)
T $EB7B,11 "CHASE  H.Q."
;
B $EB86,1 Attribute: Bright yellow ink over black + Single height bit
W $EB87,2 Screen position (16,112)
T $EB89,27 "PLEASE NOTE CONTROL OPTIONS"
;
B $EBA4,1 Attribute: Bright yellow ink over black + Single height bit
W $EBA5,2 Screen position (40,120)
T $EBA7,21 "CANNOT BE REMODIFIED."
;
B $EBBC,1 Attribute: Bright cyan ink over black + Single height bit
W $EBBD,2 Screen position (8,144)
T $EBBF,31 "ARE YOU HAPPY WITH YOUR CHOICE."
;
B $EBDE,1 Attribute: White
W $EBDF,2 Screen position (40,160)
T $EBE1,21 "PRESS YES(Y) OR NO(N)"

c $EBF7

@ $EBFF label=menu_draw_string
c $EBFF Renders a string.
R $EBFF I:HL Address of a message structure (byte: attribute byte, word: destination screen address, bytes: top bit set terminated ASCII string)
  $EBFF Load attribute byte
  $EC00 Bank the top bit of the attribute byte as the C flag (this is the single height flag)
  $EC05 Advance
  $EC06 Load screen address into #REGde
  $EC0A Stack screen address
N $EC0B Calculate the attribute address from the screen address
  $EC0B #REGh = $58 + ((D >> 3) & 3)
  $EC14 #REGl = E
  $EC15
N $EC16 #REGhl' is screen address, #REGde' is attribute address
  $EC16 Get screen address and preserve old #REGhl
  $EC17 Preserve regs
@ $EC19 menu_draw_string_loop
  $EC19 Mask off the terminator bit
  $EC1C #REGa = Character to draw
  $EC1D Draw the character
  $EC20
  $EC21 End of string?
  $EC23 Advance to next char irrespective
  $EC24 Loop if not
  $EC26
  $EC27 Restore regs
  $EC2A
  $EC2B Return

@ $EC2C label=menu_draw_char
c $EC2C Renders a single character. Compare #R$9FB4
R $EC2C I:A   The character to plot (ASCII)
R $EC2C I:F'  Carry flag set to draw single height characters
R $EC2C I:DE' Screen address (UDG aligned)
R $EC2C I:HL' Attribute address
  $EC2C Handle spaces
  $EC32 Advance attribute address
  $EC35 Map character ranges:
  $EC37 >= 'A' then glyph offset C=(65-32-18)=15
  $EC3B >= '0' then glyph offset C=(48-32-11)=5
  $EC41 == '!' then glyph offset C=0
  $EC46 == '(' then glyph offset C=1
  $EC4B == ')' then glyph offset C=2
  $EC4F == ',' then glyph offset C=3
  $EC54 Anything else becomes a full stop
  $EC57 ASCII - 32 - offset in #REGc
; C is now an offset
  $EC59 Multiply #REGc by seven - the height of a glyph
  $EC5F #REGbc = #REGa -- Why the RL?
  $EC64,3 Point #REGhl at 8x7 font
  $EC67 Point at glyph
  $EC6E If the carry flag is set then draw single height characters
@ $EC71 label=menu_draw_char_double_height
  $EC71 Load a row of glyph
  $EC72 Put it on the screen
  $EC73 Move to next scanline
  $EC74 Put another row
  $EC76 Undo LDI
  $EC77 Move to next scanline
  $EC78 Repeat ...
  $EC8B Move to next scanline after boundary (DE += $F81F)
  $EC92 Repeat ...
  $ECA8 ...
  $ECAB Save #REGl
  $ECAC Set the BRIGHT bit
  $ECAE,1 Set the screen attribute
  $ECAF Move to the next attribute row
  $ECB3 Clear the BRIGHT bit
  $ECB5,1 Set the screen attribute
  $ECB6 Restore #REGl
  $ECB7,1 Advance cursor
  $ECB9 Return
@ $ECBA label==menu_draw_char_single_height
  $ECBA Plot 8x7 character  HL->character, DE->screen
  $ECD6,1 Set the screen attribute
  $ECD7,1 Advance cursor
  $ECD9 Return

@ $ECDA label=clear_screen
c $ECDA Clears the screen.

c $ECF3

c $ED4D
  $ED94,3 -> "B N M ..."
  $EDBD,3 -> test mode strings

t $EDD6 Names of keys

b $EE26
b $EE2B
b $EE30
b $EE3D
b $EE40

c $EE6E

c $EE9E Define keys
  $EEAD Self modified, cycles 5,3,2,1
  $EEBE Self modified, cycles $F12x .. $F2xx ish

c $EF19
c $EF22
c $EF29
c $EF38

b $EF5E Drum related

c $F0C6 White noise generator?
  $F0C9 loop: Point at random bytes
  $F0CC Increment first byte by 3, Load it into #REGb

b $F0FE

c $F220 routine/data copied to $8014 during init? (926 bytes long)
  $F220 routine
;b $F490 Credits / Score messages
T $F497,10 "PRESS GEAR"
T $F4A8,17 "ENTER FOR OPTIONS"
T $F4C1,7 "CREDITS"
T $F4CF,20 "PROGRAM      JOBBEEE"
T $F4EA,20 "GRAPHICS        BILL"
T $F505,20 "MUSIC       JON DUNN"
T $F523,13 "BEST OFFICERS"
T $F537,28 "RANK  SCORE  STAGE PLAY NAME"
T $F55A,28 "1ST  56784010  ALL    1  JOB"
T $F57D,28 "2ND  35678000   4     1  ABC"
T $F5A0,28 "3RD   4340300   3     2  DEF"

b $F5BE
b $F8D2
b $FAD3
b $FE34
b $FE4A
b $FE4E
b $FE64
b $FE6A
b $FE74
b $FEBE
c $FEFE
b $FF01
