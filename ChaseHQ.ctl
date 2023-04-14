> $4000 ; ChaseHQ.ctl
> $4000 ;
> $4000 ; This is a SkoolKit control file to disassemble the ZX Spectrum 48K version of
> $4000 ; Chase H.Q. by Ocean Software. A home computer version of the arcade game by
> $4000 ; Taito Corporation.
> $4000 ;
> $4000 ; Reverse engineering by David Thomas, 2023.
> $4000 ;
> $4000 ; This is intended to document the game when it is in a "pristine" as-loaded
> $4000 ; state.
> $4000 ;
> $4000 ;
> $4000 ; AUTHORS
> $4000 ; -------
> $4000 ; Code by JOBBEEE aka John O'Brien
> $4000 ; <https://www.mobygames.com/person/28679/john-obrien/>
> $4000 ;
> $4000 ; Graphics by BILL aka Bill Harbison
> $4000 ; <https://www.mobygames.com/person/31804/bill-harbison/>
> $4000 ;
> $4000 ; Music by JON DUNN aka Jonathan Dunn
> $4000 ; <https://www.mobygames.com/person/31702/jonathan-dunn/>
> $4000 ;
> $4000 ;
> $4000 ; OTHER VERSIONS
> $4000 ; --------------
> $4000 ; The Amstrad CPC version is by the same authors.
> $4000 ;
> $4000 ;
> $4000 ; RELATED GAMES
> $4000 ; -------------
> $4000 ; "WEC Le Mans" (1988) - An earlier game by the same authors.
> $4000 ; <https://www.mobygames.com/game/15167/wec-le-mans-24/>
> $4000 ;
> $4000 ; "Batman The Movie" (1989) - The same authors worked on the driving sequences
> $4000 ; for this Atari ST/Amiga game.
> $4000 ; <https://www.mobygames.com/game/3848/batman/>
> $4000 ;
> $4000 ; "Burnin' Rubber" (1990) - A later Amstrad game by JOBBEEE very much like WEC
> $4000 ; Le Mans.
> $4000 ; <https://www.mobygames.com/game/70894/burnin-rubber/>
> $4000 ;
> $4000 ;
> $4000 ; RESEARCH
> $4000 ; --------
> $4000 ; https://www.mobygames.com/game/9832/chase-hq/
> $4000 ;
> $4000 ; http://reassembler.blogspot.com/2012/06/interview-with-spectrum-legend-bill.html
> $4000 ;
> $4000 ; https://spectrumcomputing.co.uk/entry/903/ZX-Spectrum/Chase_HQ
> $4000 ; Known Errors:
> $4000 ; Bugfix provided by Russell Marks:
> $4000 ; "When you finish a stage, a hidden bonus is sometimes given randomly. At
> $4000 ; address $8b3d, instructions EX AF,AF' and LD B,A are in the wrong order.
> $4000 ; Therefore calculation of time bonus is using random value from A' instead of
> $4000 ; low digit from remaining time. Fixed with POKE 35645,71: POKE 35646,8."
> $4000 ;
> $4000 ; https://news.ycombinator.com/item?id=8850193
> $4000 ;
> $4000 ;
> $4000 ; SECRETS
> $4000 ; -------
> $4000 ; Redefine keys to "SHOCKED<ENTER>" to activate test/cheat mode.
> $4000 ;
> $4000 ;
> $4000 ; 48K/128K VERSION DIFFERENCES
> $4000 ; ----------------------------
> $4000 ; - AY music + sampled speech and effects
> $4000 ; - Text animations
> $4000 ; - High score entry
> $4000 ; - Best Officers (high score) on attract screen
> $4000 ; - Input device/define keys is retained
> $4000 ;
> $4000 ;
> $4000 ; MEMORY MAP
> $4000 ; ----------
> $4000 ; "Once running" => once the game's been shifted into its regular running state.
> $4000 ;
> $4000 ; $5B00..$5BFF is a pre-shifted version of the backdrop (once running)
> $4000 ; $5C00..$5CFF is the regular version of the backdrop (once running)
> $4000 ; $ED28        is the stack
> $4000 ; $EE00..$EEFF is (cleared by $87DD) a cyclic road buffer of some sort
> $4000 ; $EF00..$EFFF is a table of flipped bytes (once running)
> $4000 ; $F000..$FFFF is a 4KB back buffer (once running)
> $4000 ;
> $4000 ; An back buffer address of 0b1111BAAACCCXXXXX means:
> $4000 ; - X = X position byte (0..31)
> $4000 ; - A = Y position row bits 0..2
> $4000 ; - B = Y position row bit  3
> $4000 ; - C = Y position row bits 4..6
> $4000 ;
> $4000 ; There's a column spare at either side of the back buffer (same as the main
> $4000 ; screen). This seems to be to avoid full clipping.
> $4000 ;
> $4000 ;
> $4000 ; NOTES
> $4000 ; -----
> $4000 ; Strings are top bit set terminated ASCII.
> $4000 ;
> $4000 ;
> $4000 ; THINGS TO LOCATE
> $4000 ; ----------------
> $4000 ; Considering this is just level 1 in 48K:
> $4000 ; - input handlers
> $4000 ; - score, time, speed, distance, gear, stage no., turbo count, credit count
> $4000 ; - music data and player
> $4000 ; - most of car sprites
> $4000 ; - car smoke sprites
> $4000 ; - lamp post sprites
> $4000 ; - tree sprites
> $4000 ; - car's gravity / jump handling
> $4000 ; - collision detection
> $4000 ; - sound effects
> $4000 ; - tunnel handling
> $4000 ; - route/map
> $4000 ; - road drawing code
> $4000 ; - road split handling (it draws both in one pass)
> $4000 ; - floating arrow sprite for road split
> $4000 ; - floating HERE! arrow
> $4000 ; - road signs
> $4000 ; - desert road handling
> $4000 ; - background drawing code (hills in 1st level)
> $4000 ; - attract mode
> $4000 ; - text overlay handling
> $4000 ; - hi score stuff
> $4000 ; - cheat mode ("SHOCKED")
> $4000 ; - smash meter overlay handling
> $4000 ; - redefine keys screen
> $4000 ; - names of keys
> $4000 ; - default key defs A/Z K/L
> $4000 ; - message drawing code (white bar)
> $4000 ; - turbo sprites/anim
> $4000 ; - turbo handling
> $4000 ; - level loading code
> $4000 ; - level format (and which memory regions are altered)
> $4000 ; - helicopter (later level)
> $4000 ; - flashing light on car (and hand)
> $4000 ; - ok creep! auto driving
> $4000 ; - Chase HQ monitoring system
> $4000 ; - picture handling / noise-in-out effect
> $4000 ; - car graphic encoding
> $4000 ; - is attract mode a CPU player or a recording?
> $4000 ; - exact scoring rules

@ $4000 start
@ $4000 org
b $4000 Screen memory
B $4000 Screen bitmap
B $5800 Screen attributes

c $5B00 Could be loader code
  $5B22 Point #REGhl at $5B7D
  $5B2B Fetch a word from #REGhl and stack it
  $5B31 Routine that ...
  $5B4C Game entry point
  $5B58 Routine that ...
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

b $5CF0 (More) per-level data
;
@ $5CF0 label=perp_mugshot_attributes
W $5CF0,2 Address of perp's mugshot attributes
@ $5CF2 label=perp_mugshot_bitmap
W $5CF2,2 Address of perp's mugshot bitmap
;
@ $5CF4 label=ground_colour
W $5CF4,2 Screen attributes used for the ground colour (a pair of matching bytes)
;
W $5CF6,18,2 TBD - likely addresses used by $AC1F
  $5D08 TBD

w $5D0C Table of addresses of LODs (levels of detail - a set of sprites of various sizes representing the same object).
@ $5D0C label=lods_table
  $5D0C,2 -> stones_lods
  $5D0E,2 -> dust_lods
  $5D10,2 -> car_lods (perp's car)
  $5D12,2 -> lambo_lods
  $5D14,2 -> truck_lods
  $5D16,2 -> lambo_lods
  $5D18,2 -> car_lods

b $5D1A TBD
B $5D1A,1 loaded by $A813
B $5D1B,1 loaded by $A6A7
B $5D1C,1 loaded by $A759
B $5D1D   pointed at by $841E, $858C
B $5D2B   pointed at by $F426, $8258

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
B $5E3E,3 TBD
W $5E41,2 Address of tumbleweed LODs table
B $5E43,1 TBD
W $5E44,2 Address of another LOD table
B $5E46 TBD these bytes seem to get hit when we're in the tunnel ... the lights?
B $5E5B hit all the time must be tree/bush defs or LODs
B $5E91 could be lods




B $638A,160,4 Graphic: Perp w/ sunglasses face (32x40). Stored top-down.
B $642A,20,4 Attribute data for perp.
;
; Flags in these LOD structures:
; - 1 seems to indicate interleaved masks? not so sure now. Think it might be pre-shifted bitmaps.
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
B $6932 TBD
B $694A,24,3 Lamborghini_5 mask (24x8)
B $6962 TBD
B $697A,24 Truck_4/5
B $6992 TBD
B $69AA,24 Truck_4/5 mask
B $69C2 TBD
B $69C2 TBD
B $69DA,27 Car_4/5
B $69F5 TBD
B $6A10,27 Car_4/5 mask
B $6A2B mystery block
;
@ $6A46 label=stones_lods
B $6A46,1 Width (bytes)
B $6A47,1 Flags
B $6A48,1 Height (pixels)
W $6A49,2 Bitmap
W $6A4B,2 Mask
L $6A46,7,6
;
@ $6A70 label=dust_lods
B $6A70,1 Width (bytes)
B $6A71,1 Flags
B $6A72,1 Height (pixels)
W $6A73,2 Bitmap
W $6A75,2 Mask
L $6A70,7,6
;
; TODO: Having bitmap/mask doesn't make sense if it's got embedded masks, so what's going on there? Are they actually pre-shifted sprites?
;
B $6A9A,20,4 Stones bitmap interleaved (16x2x5)
B $6AAE,16,4 Stones bitmap interleaved (16x2x4)
B $6ABE,16,4 Stones mask interleaved (16x2x4)
B $6ACE,12,4 Stones bitmap interleaved (16x2x3)
B $6ADA,12,4 Stones mask interleaved (16x2x3)
B $6AE6,4,2 Stones bitmap interleaved (8x2x2)
B $6AEA,4,2 Stones mask interleaved (8x2x2)
B $6AEE,2,2 Stones bitmap interleaved (8x2x1)
B $6AF0,2,2 Stones mask interleaved (8x2x1)
;
B $6AF2,2 Dust bitmap interleaved (8x2)
B $6AF4,2 Dust mask interleaved (8x2)
;
B $6AF6 TBD
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
B $6C8A,64,4 Turn right sign interleaved (16x2x16)
B $6CCA,52,2 Turn right sign interleaved (16x2x13)
B $6CFE,52,2 Turn right sign interleaved "mask" (16x2x13)
;
B $6D32,,4 TBD another interleaved turn right sign (16x10)
B $6D5A,,4 TBD and another, looks pre-rotated
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
B $6E99,28,4 Barrier interleaved (16x2x7)
B $6EB5,28,4 Mask for barrier interleaved (16x2x7)

B $6ED1 TBD
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
B $6FAC,8,8 TBD
B $6FB4,8,2 graphic_6eeb mask (16x4)
B $6FBC,8,8 TBD
B $6FC4,6,2 graphic_6eeb (16x3)
B $6FCA,6,6 TBD
B $6FD0,6,2 graphic_6eeb mask (16x3)



;B $7069 is the top part of a streetlamp
;B $73D4 looks like tree trunk
B $73B9,10 Part of tree graphic - 16px x 5 rows (?)

b $76F0 Turbo icons
B $76F0,168,4 Three frames 16x14 each, looks like a mask-bitmap-mask-bitmap arrangement

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
  $7D51,160,4 Graphic: Tony's face (32x40). Stored top-down.
  $7DF1,20,4 Attribute data for above.

b $8000 temporaries?
  $8000,1 Test mode enable flag (cheat mode)
  $8002,4 Score digits as BCD (4 bytes / 8 digits, little endian)
  $8006,1 incremented on reset?
@ $8007 label=stage_number
  $8007,1 Stage number we're loading (1..5 or 6 for end credits)

@ $8014 label=load_stage
c $8014 Load a stage
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

@ $81DD label=start_chatter
b $81DD Start of game chatter
B $81DD,1 Three-way random choice ($FC)
;
W $81DE,2 -> Tony: "GIDDY UP BOY!" <STOP>
W $81E0,2 -> Tony: "HOLD ON MAN" <STOP>
W $81E2,2 -> Tony: "LET'S GO. MR. DRIVER." <STOP>
;
B $81E4,1 Tony ($03)
W $81E5,2 -> "GIDDY UP BOY!"
B $81E7,1 <STOP>
;
B $81E8,1 Tony ($03)
W $81E9,2 -> "HOLD ON MAN"
B $81EB,1 <STOP>
;
T $81EC,13 "GIDDY UP BOY!"
;
T $81F9,11 "HOLD ON MAN"

c $8204 In-game sound effects
  $8246 Produces engine sound effect
  $824E Produces the steering squeak

b $82A6 Attract mode messages
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

; These are all just thunks. Possibly hooks for 128K mode sfx.
c $83B5 just a RET
c $83B8 sound effects
c $83BB just a RET
c $83BE just a RET
c $83C1 just a RET
c $83C4 TBD
c $83C7 just a RET
c $83CA TBD

c $83CD Bootstrap
; This gets hit when a game finishes and we return to the attract mode.
N $83CD Builds a table of flipped bytes at $EF00.
  $83CD Point #REGhl at $EF00
@ $83D0 flip_table_loop
  $83D0 8 iterations
  $83D2 #REGa is the index into the table
@ $83D3 flip_byte_loop
  $83D3 Shift a bit out of #REGa leftwards and into #REGc rightwards
  $83D6 Loop for 8 bits
  $83D8 Write #REGc out and advance
  $83DA Loop for 256 iterations (when #REGl overflows)
;
  $83DC Indirect jump TBD
;
  $83DF $A13C = 0
  $83E3 Zero $8002..$8006
  $83EC $8007 = 1 => Load stage 1
  $83EE credits = 2
  $83F2 Call the main loop
  $83F5 If $A139 is non-zero this calls $81AA which is the end of a string...
  $83FF Loop

c $8401 Main loop
  $8401 Call load_stage
  $8404 Are we on stage 6 (end credits) jump $841B if not so
@ $840B label=handle_end_credits
  $840B Call $5C00 [which is data in earlier stages, must be end credit anim]
  $840E Reset (wanted) stage to 1
  $8413 Call load_stage
  $8418 Set (wanted) stage to 6   [not sure why]
  $841A Return
@ $841B label=l_841b
  $841B CALL $858C    ; TBD
  $841E LD HL,$5D1D   ;
  $8421 Call "Initialisation"
  $8424 $A13B cycles 3,2,1 then repeats
  $842E $A26B = (A * 4) OR 2
  $8435 Is $A139 zero? (used later)
  $8439 $A188 = $FF
  $843E LD HL,$81DD   ;
  $8441 message related
  $8444 TBD
  $8447 Call keyscan
  $844A TBD - has TIME UP and missing restart stuff
  $844D TBD - has quit handling
  $8450 TBD
  $8453 TBD - some bonus handling
  $8456 TBD
  $8459 TBD
  $845C TBD
  $845F <sound>
  $8462 TBD
  $8465 TBD
  $8468 <sound>
  $846B TBD
  $846E <sound>
  $8471 TBD - landscape related
  $8474 <sound>
  $8477 TBD
  $847A TBD
  $847D TBD
  $8480 TBD - helicopter related
  $8483 TBD
  $8486 <sound>
  $8489 TBD
  $848C TBD
  $848F <sound>
  $8492 TBD
  $8495 TBD
  $8498 <sound>
  $849B TBD
  $849E <sound>
  $84A1 TBD
  $84A4 Call speed_score
  $84A7 stage/bonus/LO-HI/flashing lights
  $84AA Call overtake_bonus
  $84AD <sound>
  $84B0 noise effect, message plotting
  $84B3 TBD - tiny func
  $84B6 Call transition
  $84B9 <sound>
  $84BC Call draw_screen -- copy backbuffer to screen
  $84BF TBD
  $84C2 Is test mode enabled?
  $84C5 If not, goto no_test_mode
N $84C8 Test mode handling
  $84C8 Read keys 1/2/3/4/5
  $84CC Change to active high
  $84CD Mask off just keys
  $84CF If none are set then jump
  $84D1 EX AF,AF'     ;
  $84D2 LD BC,$0804   ;
  $84D5 TBD
  $84D8 <128k hook?>
  $84DB EX AF,AF'     ;
  $84DC Is bit 0 set? (key 1 to restart the level)
  $84DD Restart level if so
  $84E0 HL = &stage_number
  $84E3 Is bit 1 set? (key 2 to load the next level)
  $84E4 Jump if NOT
  $84E6 stage_number++
  $84E7 Exit via load_stage
  $84EA Is bit 2 set? (key 3 to load the end screen)
  $84EB Jump if NOT
  $84ED stage_number = 6
  $84EF Exit via load_stage
; (key 4 or 5 for an extra credit)
  $84F2 Increment credits unless maxed out at 9
@ $84FB label=no_test_mode
  $84FB LD A,($A231)  ;
  $84FE AND A         ;
  $84FF JP NZ,$8444   ;
  $8502 LD HL,$A26B   ;
  $8505 SRL (HL)      ;
  $8507 JR NC,$8512   ;
  $8509 A = *HL
  $850A LD (HL),$00   ;
  $850C CALL $83C7    ;
  $850F JP $8444      ;
  $8512 LD A,($A26A)  ;
  $8515 AND A         ;
  $8516 JP Z,$8444    ;
  $8519 DEC A         ;
  $851A JP NZ,$873C   ;
  $851D LD A,$02      ;
  $851F LD ($A26A),A  ;
  $8522 LD A,$08      ;
  $8524 CALL $8DF9    ;
  $8527 JP $8444      ;

c $852A
  $8556 Set user input

c $858C
b $85DD

c $85E4

c $860F

c $865A
b $86F6

c $87DC Initialisation
  $87DD Zero $EE00..$EEFF, and point $A240 at $EE00
  $87EC A = 0
  $87ED Copy (47 bytes) at $A13E to $A16D - saved game state
  $87F8 Zero 208 bytes at $A19D
  $8807,3 Pre-shift the backdrop image
  $880A $E34B = 8
; These self modified instructions change from NOPs to PUSHes when we're
; drawing the tunnel. This will just be a reset though.
; #REGa being zero here is a latent bug. I can't see any explicit reset.
  $8810 $E34C = 0
  $8812 $E34D = 0
  $8813 $8F82 = NOP (instruction)
  $8818 $8F83 = NOP (instruction)
  $881A $8F84 = NOP (instruction)
  $881B NOP 6 bytes [of instructions] at $8FA4
  $8824 Self modify "LD (HL),xx" at $C058 xx = 0
  $8827 Self modify "LD A,xx" at $B063 xx = 0
  $882A Set $A191 to value of lods_table[2]
  $8830 Zero $EE00..$EEFF, and point $A240 at $EE00 [duplicates work from earlier]
@ $883F label=loop883f
; alter this iterations count and we seem to start later in the level
  $883F 32 iterations   - affects start position?
  $8841 Preserve BC
  $8842 HL = $A23F
  $8845 Call HUGE function
  $8848 Restore BC
  $8849 Loop
  $884B $A254 = 0
  $884F Transition type?
  $8851 Call setup_transition
  $8854 Call clear_screen_set_attrs
  $8857 Point #REGhl at left light's attributes
  $885D Point #REGhl at right light's attributes then FALL THROUGH
@ $8860 label=clear_lights
  $8860 Clear the lights' BRIGHT bit
; flashing lights are 5 attrs wide, 4 high

c $8876
  $8876,3 Inhibits user input when $A231 isn't 4
  $8879,3 Address of user_input
  $8883 AND user_input with user input mask
  $8888 AND with mask for (Quit+Pause+Turbo), return if none
  $888B Quit bit set? Goto quit_bit if so
  $8897 Return if no turbo boosts left
  $88B1,3 Call fill_attributes
@ $88A9 label=quit_bit
  $88A9

@ $88D5 label=clear_game_attrs
c $88D5 Clears the game screen attributes to zero

@ $88E2 label=clear_game_screen
c $88E2 Clears the game screen attributes and the game screen to zero

c $88F2
R $88F2 I:BC e.g. $0704 $0801 $0604
  $88F2 If $A238 is zero then jump $88FA
  $88F8 Return if C > $A238
  $88FA $A237 = B
  $88FE $A238 = C
  $8902 Return

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
  $8B06 Set score
  $8B10,3 Call increment_score
  $8B87,3 Point at "CLEAR BONUS - TIME BONUS - SCORE" messages
;
@ $8C35 label=j_8c35
  $8C35 $A195 = DE
  $8C39 Return

@ $8C3A label=fully_smashed
c $8C3A Fully smashed
  $8C3A draw_screen flag = 1
  $8C3F $A22F = 2
  $8C43 Set smash_counter to 20
  $8C48 Set user input mask to (Quit+Pause)
  $8C4D Print the "OK! PULL OVER CREEP!" message
  $8C53
  $8C56 Exit via $8C35

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
  $8D95 If zero, call draw_mugshots
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
  $8E4B A--
  $8E4C LD ($8E4A),A  ; self modify
  $8E4F Return if non-zero
  $8E50 A = *HL
  $8E51 LD ($8E4A),A  ; self modify
  $8E54 increment $8E46, affects TIME UP state if disabled
  $8E5B B++
@ $8E5C label=sub_8e42_1
  $8E5C HL++
  $8E5D A = *HL
  $8E5E AND A         ; test flags
  $8E5F JR Z,$8E66    ; exit
  $8E61 CALL $8E6C    ;
  $8E64 JR $8E47      ;
  $8E66 DEC HL        ;
  $8E67 A = *HL
  $8E68 LD ($A231),A  ;
  $8E6B Return

c $8E6C Message printing related. Increments HL then loads A,E,D,C,B from where HL points.
  $8E6C EX AF,AF'     ;
  $8E6D Preserve ?
  $8E6E HL++
  $8E6F A = *HL
  $8E70 HL++
  $8E71 E = *HL
  $8E72 HL++
  $8E73 D = *HL
  $8E74 HL++
  $8E75 C = *HL
  $8E76 HL++
  $8E77 B = *HL
  $8E78 HL++
  $8E79 CALL $9F99    ;
  $8E7C POP BC        ;
  $8E7D Return

c $8E7E Message printing related, called for TIME UP, CONTINUE, etc. messages.

; This gets hit when the perp is pulled over (transition_control == 1)
@ $8E91 label=draw_mugshots
c $8E91 Copies the mugshots onto the screen when perp is caught
  $8E91 -> Perp's mugshot attributes
  $8E94 Final byte of bitmap (back buffer)
  $8E97 Screen position (40,104)
  $8E9A Call draw_mugshot
  $8E9D -> Tony mugshot attributes
  $8EA0 Final byte of bitmap (back buffer)
  $8EA3 Screen position (160,104)
  $8EA6 Call draw_mugshot
  $8EA9 -> Raymond mugshot attributes
  $8EAC Final byte of bitmap (back buffer)
  $8EAF Screen position (200,104)
  $8EB2 Call draw_mugshot
  $8EB5 Exit via sub_8e42

@ $8EB7 label=draw_mugshot
c $8EB7 Draws a mugshot
R $8EB7 I:BC Screen position (used for attributes)
R $8EB7 I:DE Address of last byte of bitmap data to be written (in back buffer)
R $8EB7 I:HL Address of mugshot attributes (screen)
  $8EB7 Preserve screen position
  $8EB8 Preserve address of mugshot attributes
  $8EB9 Move back a byte to point at end of bitmap data (we assume that the bitmap data precedes the attribute bytes)
  $8EBA Size of mugshot data (32x40 bitmap)
@ $8EBD label=dm_loop
  $8EBD Save #REGe
  $8EBE Transfer four bytes (#REGde, #REGhl and #REGbc are decremented by 4)
  $8EC6 Restore #REGe
  $8EC7 If #REGbc becomes zero then proceed to dm_plot_attrs
N $8ECA Move to next scanline
  $8ECA Save for checking in a moment
  $8ECB Move to next scanline (visually upwards)
  $8ECC Would it have rolled over into the top nibble?
  $8ECE No - continue
N $8ED1 It rolled over
  $8ED1 Put back the bit stolen by rollover
  $8ED5 Move to next chunk of 16 scanlines
  $8ED9 Continue if it didn't roll over
  $8EDC Otherwise move to the next chunk of 128 scanlines (would put us outside the back buffer)
  $8EE0 Loop
@ $8EE3 label=dm_plot_attrs
  $8EE3 Restore address of mugshot attributes
  $8EE4 Exit via plot_face_attrs_bit

@ $8EE7 label=smash_bar_etc
c $8EE7 Smash bar etc.
  $8EE7 Return if the perp has not yet been sighted
  $8EEC Return if $A230 is >= 3
  $8EF2 Back buffer address of bottom of bar
  $8EF5 #REGd = 15 = mask, #REGe = 16 = row stride
N $8EF8 Draws bottom two rows
  $8EF8 2 rows
  $8EFA Call draw_smash_bar_solid_bit
  $8EFD Get smash_counter
  $8F00 Jump straight to drawing solid if it's zero (whole bar solid)
  $8F03 Set iterations in #REGc for call
  $8F04 Preserve smash_counter
  $8F05 Call draw_smash_bar_segment
  $8F08 Restore smash_counter
@ $8F09 label=draw_smash_bar_solid_top
  $8F09 Iterations = smash_counter
  $8F0A B = ~(A * 3) + 63 = amount of solid rows to draw
  $8F10 Call draw_smash_bar_solid_bit
;
@ $8F13 label=draw_smash_bar_segment
  $8F13 Set 8 pixels to "X......X"
  $8F15 Save for checking in a moment
  $8F16 Move to next scanline
  $8F17 Would it have rolled over into the top nibble?
  $8F18 No - continue
N $8F1B It rolled over
  $8F1B Put back the bit stolen by rollover
  $8F1E Move to next chunk of 16 scanlines
  $8F22 Continue if it didn't roll over
  $8F25 Otherwise move to the next chunk of 128 scanlines (would put us outside the back buffer)
@ $8F28 label=draw_smash_bar_segment2
  $8F28 Set 8 pixels to "X......X"
  $8F2A Save for checking in a moment
  $8F2B Move to next scanline
  $8F2C Would it have rolled over into the top nibble?
  $8F2D No - continue
N $8F2D It rolled over
  $8F30 Put back the bit stolen by rollover
  $8F33 Move to next chunk of 16 scanlines
  $8F37 Continue if it didn't roll over
  $8F3A Otherwise move to the next chunk of 128 scanlines (would put us outside the back buffer)
@ $8F3D label=draw_smash_bar_segment_cont
  $8F3D 1 row
  $8F3F Call draw_smash_bar_solid_bit
  $8F42 Loop while iterations remain
  $8F46 Return
;
@ $8F47 label=draw_smash_bar_solid_bit
  $8F47 Set 8 pixels to solid black
N $8F49 Move to next scanline
  $8F49 Save for checking in a moment
  $8F4A Move to next scanline (visually upwards)
  $8F4B Would it have rolled over into the top nibble?
  $8F4C No - continue
N $8F4F It rolled over
  $8F4F Put back the bit stolen by rollover
  $8F52 Move to next chunk of 16 scanlines
  $8F56 Continue if it didn't roll over
  $8F59 Otherwise move to the next chunk of 128 scanlines (would put us outside the back buffer)
@ $8F5C label=draw_smash_bar_cont
  $8F5C Loop while iterations remain
  $8F5E Return

c $8F5F

c $901C

c $9023

c $904B

; unclear if/how this routine gets entered
c $9052

c $916C

c $9171

c $924D

c $9252

c $9278

c $92E1
  $93D6,3 Exit via plot_sprite
  $93DC,3 Exit via plot_sprite_flipped
  $93E8,4 #REGix = Base of jump table

@ $949C label=plot_sprite
c $949C Sprite plotter for back buffer, up to 64px wide, 15px high, no mask, no flip.
R $949C I:A   Plot (A+1)*8 pixels
R $949C I:DE' Stride of bitmap data in bytes
R $949C I:HL  Address in back buffer to plot at
R $949C I:HL' Address of bitmap data
  $949C Use plot_sprite_odd if the bottom bit is set (odd widths)
  $94A1 #REGix = Base of jump table
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

@ $9542 label=plot_sprite_flipped
c $9542 Sprite plotter for back buffer, with flipping
  $9542 TBD

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
@ $962E label=next_character
W $962E,2 Address of the next character in the current message
@ $9630 label=next_message
W $9630,2 Address of the _next_ message to show DOUBTING THIS NOW ... pointer to next command?
@ $9632 label=message_x
  $9632,1 Index of next character in the message bar
@ $9633 label=bleh
  $9633,1 zeroed by $995d set to 10 by $9a49
@ $9634 label=noise_bytes
  $9634,5 Five bytes used for noise when character pictures 'noise in'
@ $963C label=noise_counter
  $963C,1 Noise effect counter (4..0)
@ $963D label=TBD963d
  $963D,1 read by $9946, one'd by $9961, set by $99d9  character index?
@ $963E label=TBD963e
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
@ $9926 label=raymond_says_one_more_time
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

@ $9945 label=chatter
c $9945 Chatter routine
R $9945 I:A TBD
R $9945 I:HL Address of message set
  $9945 B = A
  $9946 LD A,($963D)  ;
  $9949 If A == 0 jump forward to 9955
  $994C If A >= 3 jump forward to 9955
; A is 1 or 2
  $9950 LD A,($963E)  ;
  $9953 If A >= B return
;
  $9955 $963E = B
  $9959 Set message set pointer
  $995C $9633 = 0
  $9960 $963D = 1
  $9964 Return

c $9965
  $9965 LD A,($963D)  ; A = TBD963d - 1
  $9969 JR Z,$99DF    ; if A was zero jump
  $996B A--
  $996C JR Z,$998F    ; if A was zero jump to sub998f_do_noise_effect
  $996E A--
  $996F JR NZ,$997F   ; if A was NOT zero jump
; A always zero here
  $9971 Decrement noise_counter
  $9975 A = *HL A = noise_counter
  $9976 JP NZ,$9A5C   ; Jump if noise_counter was non-zero
  $9979 LD ($963D),A  ; Zero this
  $997C Call noise_plot_attrs -- clear to black
;
  $997F Plot space character
  $9981 LD A,$AA      ; {$AA ROR 1 becomes $55 becomes $AA and so on
  $9983 RRCA          ; probably a delay / alternating call
  $9984 LD ($9982),A  ; }
  $9987 LD A,$FF      ; ...and then gets overwritten?
  $9989 JP C,$9AF1    ; jump on $55 -> $AA transitions?
  $998C,3 Goto plot_mini_font_1
;
@ $998F label=sub998f_do_noise_effect
  $998F If noise_counter > 0 call noise_effect
  $9996 LD A,($9633)  ; If $9633 == 0 jump $99b6
  $9999 AND A         ;
  $999A JR Z,$99B6    ;
  $999C DEC A         ; $9633--
  $999D LD ($9633),A  ;
  $99A0 LD B,A        ;
  $99A1 JR Z,$99BD    ; If A was zero jump sub998f_another_something
  $99A3 LD HL,($962E) ; Load address of next character
  $99A6 DEC HL        ; Go back 1
  $99A7 Load the character for when we call plot_mini_font*
  $99A8 Clear string terminator bit
  $99AA message_x - 1
  $99AE rotate it by B  (B is noise counter) .. wut?
  $99B0,3 if carry jump plot_mini_font_2
  $99B3,3 Goto plot_mini_font_1
;
@ $99B6 label=sub998f_something
  $99B6 If message_x != 0 jump $9a30
;
@ $99BD label=sub998f_another_something
  $99BD LD HL,($9630) ; HL = next_message
  $99C0 A = *HL first char of
  $99C1 String terminator? jump to end_of_message if so
  $99C5,2 this tests for $FE -- different terminator?
  $99C7 JR NZ,$9A24   ; If not $fe goto chatter_message
; $FE means clear first?
  $99C9 HL++
  $99CA next_message = *HL
  $99D1 JR $99E4      ; goto 99e4_exit
;
@ $99D3 label=end_of_message ?
  $99D3 Set the noise effect counter to 4
  $99D8 Set $963D to 3
  $99DC Exit via clear_message_line
;
@ $99DF label=99df_something_else
  $99DF $963D = 2
;
@ $99E4 label=99e4_exit
  $99E4 Call clear_message_line

; message format
; <byte> 0/1/2/3 pilot/nancy/raymond/tony face
;        0 uses $5CF2 as face base - only used for the chopper pilot?
;        $FC => call rnd and use one of three following pointers as an address (repeat)

@ $99EC label=print_chatter
c $99EC Shows the alerts and remarks from the game's characters
  $99EC Fetch address of next remark
@ $99EF label=chatter_loop
  $99EF Read a byte and advance
  $99F1,2 if (byte != $FC) then jump plot_character
@ $99F5 label=random_chatter
N $99F6 We have a three-way choice here
  $99F6,3 Call rng - returns a random byte
  $99FA If random value < $55 jump load_message_ptr -- a 33.3% chance
  $99FE Skip first option message
  $9A00 If random value < $AA jump load_message_ptr -- a 66.6% chance
  $9A04 Skip second option message
@ $9A06 label=load_message_ptr
  $9A06 Load the address of the message
  $9A0A Loop back - it could be another random case
@ $9A0C label=plot_character
N $9A0C #REGa is the index of the face to show
  $9A0C Is it zero? (The definable/pilot character's face)
  $9A0D Preserve message pointer
  $9A0E Plot whatever face #R$5CF2 points to, otherwise calculate
  $9A14 Load base address of face graphics (BUT it's actually earlier than the real base because we're 1-indexed)
  $9A17 Length of face graphic (bitmap + attrs = 4*8*5 + 4*5)
  $9A1A Get face graphic address
@ $9A1D label=do_plot
  $9A1D Set plot address to (176,8)
  $9A20 Call plot_face
  $9A23 Restore message pointer
@ $9A24 label=chatter_message
  $9A24 Fetch address of message to start showing
  $9A28,3 Save address of next message to show (or perhaps the pointer)
  $9A2B,4 Save next character address
  $9A2F Cause a clear_message_line and fall through
;
  $9A30,6 if (A == 0) clear_message_line
  $9A36 Fetch next_character
  $9A39 The character itself
  $9A3A Clear the string terminator bit
  $9A3C Preserve message_x
  $9A3D PUSH HL
  $9A3E Call plot_mini_font_2, with #REGd as ASCII character to plot
  $9A41 POP HL
  $9A42 Test string terminator bit
  $9A44 Move to the next character in the string
  $9A45 If not terminated then jump over
  $9A47 $9633 = 10
  $9A4C Restore message_x
  $9A4D Increment and store message_x
  $9A51 Update next_character
  $9A54 Return

@ $9A55 label=noise_effect
c $9A55 Noise in/out effect
R $9A55 I:A Noise effect counter
  $9A55 Decrement the noise effect counter
  $9A59 Jump to print_chatter if it's zero, otherwise fallthrough
  $9A5C Shift A's bottom bit into carry
  $9A5D A = 255
  $9A5F D = 32  -- ASCII character for plot_mini_font*
  $9A61 If A on input was odd then jump
  $9A66 else call plot_mini_font_2 and continue at $9a6b
;
  $9A68 Call plot_mini_font_1
;
  $9A6B Set plot address to (176,8)
  $9A6E B,C = 4 columns/bytes, 40 rows
  $9A72 Preserve row ptr
  $9A73 Point #REGhl at noise_bytes
@ $9A76 label=noise_plot_loop
  $9A76 A = *HL - B -- Subtract row counter (4,3,2,1)
  $9A78 *HL++ = A
  $9A7A Rotate A left by 1
  $9A7B *HL += A
  $9A7D *DE++ = A -- Store 8 random bits
  $9A7F Loop while columns remain
  $9A81 Restore row ptr
  $9A82 Move to next row
  $9A83 Usual row movement magic here
  $9A92 Loop while rows remain
;
  $9A96 #REGa = attribute $47 => BRIGHT + white over black
@ $9A98 label=noise_plot_attrs
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
@ $9ACE label=plot_face_attrs_bit
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
R $9AEC I:D The character to plot (ASCII)
; $9AEC called constantly to clear? or on flash?
  $9AEC
  $9AEF Goto pmf_go
@ $9AF1 label=plot_mini_font_2
;R $9AF1 I:A Index into string, or $FF
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
@ $9B4C label=pmf_have_glyph_id
  $9B4C Turn the glyph ID in #REGc into ASCII in #REGa
@ $9B4F label=pmf_have_ascii
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
  $9B91 Move to next scanline
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
  $9BB9 Move to next scanline
@ $9BC9 label=cml_continue
  $9BC9
  $9BCA Decrement row counter
  $9BCB Loop while rows remain to clear
  $9BCE Return

c $9BCF tick
  $9BCF TBD
  $9BF9 Decrement the 1/16th sec counter
  $9BFB Return if not zero
  $9BFC Reset time_sixteenths to 15
  $9BFE Decrement time_bcd [POKE $9C01 for Infinite time]
  $9C04 Return if <> 15s remain
  $9C07 Nancy berating us running out of time message
  $9C0A Exit via chatter
  $9C0D Is time_bcd zero? Jump to time_up if so
  $9C11 Zero $A229
  $9C15 user_input_mask = $FF
  $9C19 Loop back to $9BF9
@ $9C1B label=time_up
  $9C1B,3 Point at "TIME UP" message
  $9C3B Jump if no credits remain
  $9C3E Decrement credits [POKE $9C3E for Infinite credits]
  $9C3F Turn it into ASCII and poke it into the "CREDIT x" string
  $9C50,5 Is fire pressed?
  $9C55 Jump if not (?)
N $9C57 Resetting mission code.
  $9C57 Reset $A229
  $9C5B Reset smash_factor to zero
  $9C60 Reset smash_counter to zero
  $9C62 Set user input mask to allow everything through
  $9C67 $A252 = 3
  $9C6A $A231 = 3 -- Flag set to zero when attributes have been set
  $9C6D turbos = 3
  $9C70 Set remaining time to 60 (BCD) [doesn't affect stuff if altered?!]
@ $9C7E label=print_continue
  $9C7E,3 Print "CONTINUE THIS MISSION" messages
  $9C84 <self modified>

@ $9CC2 label=speed_score
c $9CC2 Increments score in proportion to current speed
  $9CC2 Fetch current speed into #REGhl
  $9CC5 Speed low byte
  $9CC6 H >>= 1 but pointless?
  $9CC8 A <<= 1
  $9CC9 A >>= 2
  $9CCD Pointless?
  $9CD0 BCD correction
  $9CD1 DE = 0
  $9CD4 Exit via increment_score

c $9CD6
  $9CD6 B=A
  $9CD7 "HI"
  $9CDA C=255
  $9CDC A=B
  $9CDD Call j_9cfc
  $9CE0 terminate string?
  $9CE2 A=B
  $9CE3
  $9CE6 A=E
  $9CE7
  $9CEA A=E
  $9CEB
  $9CEE A=D
  $9CEF
  $9CF2 A=D
  $9CF3
  $9CF6
@ $9CF8 label=j_9cf8
  $9CF8 A ROR 4
@ $9CFC label=j_9cfc
  $9CFC A &= 15
  $9CFE if A is not zero goto 9D07
  $9D00 top bit set?
  $9D02 jump if so
  $9D04 where's the matching PUSH? or are we just discarding the RET addr?
  $9D05 jump 9d0e
@ $9D07 label=j_9d07
  $9D07 C = 0
  $9D09 A += 48
  $9D0B HL--
  $9D0C *HL = A
  $9D0D Return
  $9D0E Self modify the LD HL at $9D9B
  $9D11 Set the bonus_flag
  $9D16 A = B
; fallthrough

@ $9D17 label=increment_score
c $9D17 Increments the score by (D,E,A).
R $9D17 I:A Low byte of increment
R $9D17 I:E Middle byte of increment
R $9D17 I:D High byte of increment
  $9D17 HL = &score_digits_as_BCD
  $9D1A A += *HL
  $9D1B BCD correct A
  $9D1C *HL++ = A
  $9D1E A = E + *HL + carry
  $9D20 BCD correct A
  $9D21 *HL++ = A
  $9D23 A = D + *HL + carry
  $9D25 BCD correct A
  $9D26 *HL++ = A
  $9D28 A = 0 + *HL + carry
  $9D2B BCD correct A
  $9D2C *HL = A
  $9D2D Return

@ $9D2E label=calc_overtake_bonus
c $9D2E Calculate overtake bonus
R $9D2E I:A Iterations (prob number of overtakes?)
  $9D2E If $A22B is zero then return
  $9D33 B = A -- iterations
  $9D34 Point #REGhl at overtake_bonus
@ $9D37 label=overtake_loop
  $9D37 A = *HL + 2  [increment bonus by 2 up to a max of 128]
  $9D3A BCD correction
  $9D3B If A >= 128 then A = 128  [80 actual points?]
  $9D41 *HL = A
  $9D42 Bank
  $9D43 E = A -- turn score into 00nn00
  $9D44 A = 0
  $9D45 D = 0
  $9D46 CALL $9CD6
  $9D49 Unbank
  $9D4A B--, goto overtake_loop
  $9D4C $A22B = 0
  $9D50 Return

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

@ $9E11 label=plot_turbos_scores_etc
c $9E11
  $9E11 If no turbos jump to plot_scores_only
  $9E17 C = number of turbos
  $9E1C Base of turbo sprites
  $9E2E 56 bytes per sprite
  $9E31 Must be a sprite index
  $9E32 Find frame
  $9E35
  $9E3E Base of turbo sprites
  $9E44 must be the sp trick
  $9E4E plots turbos
@ $9E7B label=plot_scores_only
  $9E7B Point #REGde at speed digits screen position
  $9E7F #REGde = speed
  $9E83 TBD
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

c $9F99 Another draw string entry point?
  $9F99 PUSH BC       ;
  $9F9A EXX           ;
  $9F9B POP HL        ;
  $9F9C LD DE,$0020   ;
  $9F9F LD C,A        ;
  $9FA0 EXX           ;
  $9FA1 JR $9FA6      ;

@ $9FA3 label=draw_string
c $9FA3 Draws a string
  $9FA3 A' = 1  Set drawing type (single height, plots to real screen)
N $9FA3 The string is terminated by setting the topmost bit of the final character.
@ $9FA6 label=draw_string_entry
  $9FA6 Load a byte and mask off the text part
  $9FAE Was bit 7 set?, quit if so, otherwise loop

; 0 for double height with a gap(?)
; 1 for no gap at the top
; 2 for single height
; 3 for dbl
; 4 for single height inverted colrs
; 5 for dbl + invt
; 6 as 5 but with the gap

@ $9FB4 label=draw_char
c $9FB4 Draws a character (to buffer or screen?)
R $9FB4 I:A  Character to draw (ASCII)
R $9FB4 I:A' Rendering type (double height, invert, etc.)
R $9FB4 I:HL ...
R $9FB4 I:DE Screen address
R $9FB4 O:HL ...
R $9FB4 O:DE Screen address moved to next column
  $9FB4 If it's not a space character, goto dc_not_space with #REGa reduced
N $9FB8 It's a space
  $9FB8 Move screen address to the next column
  $9FB9
  $9FBA <move something else that's banked>
  $9FBB
  $9FBC Return
;
@ $9FBD label=dc_not_space
N $9FBD Map ASCII to glyph IDs
  $9FBD Offset = 18
  $9FBF Jump if 'A' or above
  $9FC3 Offset = 11
  $9FC5 Jump if '0' or above
  $9FC9 Glyph ID = 0
  $9FCB Jump if '!'
  $9FCE Glyph ID = 1
  $9FCF Jump if '('
  $9FD3 Glyph ID = 2
  $9FD4 Jump if ')'
  $9FD7 Glyph ID = 3
  $9FD8 Jump if ','
  $9FDC Glyph ID = 4
  $9FDD Anything else is '.'
;
N $9FDF #REGa is an ASCII character - 32
@ $9FDF label=dc_have_range
  $9FDF Convert (ASCII - 32) to glyph ID. #REGc = #REGa - Offset
@ $9FE1 label=dc_have_single
N $9FE1 #REGc is a glyph ID
  $9FE1 Multiply the glyph ID by 7
  $9FE9 Add in the carry
  $9FEB #REGbc is now the byte offset
  $9FEC,3 Point #REGhl at 8x7 font
  $9FEF Point to glyph data
  $9FF0 Get type in #REGc (passed in #REGa' on entry)
  $9FF3 If 1 then jump $A0AD
  $9FF7 If 2 then jump $A083
  $9FFC If 3 then jump $A03D -- double height
  $9FFF If 4 then jump $A031 -- single height inverted
  $A002 If 5 then jump $A023 -- double height + inverted
N $A004 Otherwise it's type 6
  $A004 PUSH DE
  $A005 B = 4
@ $A007 label=loop1
; double height
  $A007 A = *HL
  $A008 *DE = A
  $A009 Move to next row
  $A00A *DE = A
  $A00B Move to next row
  $A00C Move to next glyph row
  $A00D While iterations remain, goto loop1
  $A00F D -= 8
  $A013 E += 32
  $A017 B = 3
@ $A019 label=loop2
; double height too
  $A019 A = *HL
  $A01A *DE = A
  $A01B Move to next row
  $A01C *DE = A
  $A01D Move to next row
  $A01E Move to next glyph row
  $A01F While iterations remain, goto loop2
  $A021 JR $A074
;
  $A023 PUSH DE
  $A024 B = 7
@ $A026 label=loop3
; double height + inverted
  $A026 A = ~*HL
  $A028 *DE = A
  $A029 Move to next row
  $A02A *DE = A
  $A02B Move to next row
  $A02C Move to next glyph row
  $A02D While iterations remain, goto loop3
  $A02F JR $A074
;
  $A031 PUSH DE
  $A032 B = 7
@ $A034 label=loop4
; single height inverted
  $A034 *DE = ~*HL
  $A037 Move to next glyph row
  $A038 Move to next row
  $A039 While iterations remain, goto loop4
  $A03B JR $A0A4
;
@ $A03D label=double_height_glyph
c $A03D Plots double-height glyphs. DE->screen HL->glyph def
  $A03D PUSH DE
  $A03E *DE = 0
  $A040 D++
  $A041 *DE = *HL
  $A043 D++ -- next row
  $A044 LDI
  $A046 E--
  $A047 D++
  $A048 Repeat six times
  $A072 A = 0
  $A073 *DE = A
;
@ $A074 label=j_a074
  $A074 POP DE
  $A075 E++
  $A076 EXX
  $A077 PUSH HL
  $A078 *HL |= C
  $A07B HL += DE
  $A07C *HL |= C
  $A07F POP HL
  $A080 L++
  $A081 EXX
  $A082 Return
;
  $A083 PUSH DE
  $A084 A = 0
  $A085 *DE = A
  $A086 D++
  $A087 LDI
  $A089 E--
  $A08A D++
  $A08B LDI
  $A08D E--
  $A08E D++
  $A08F LDI
  $A091 E--
  $A092 D++
  $A093 LDI
  $A095 E--
  $A096 D++
  $A097 LDI
  $A099 E--
  $A09A D++
  $A09B LDI
  $A09D E--
  $A09E D++
  $A09F LDI
  $A0A1 E--
  $A0A2 D++
  $A0A3 *DE = A
;
@ $A0A4 label=j_a0a4
  $A0A4 POP DE
  $A0A5 E++
  $A0A6 EXX
  $A0A7 *HL |= C
  $A0AA L++
  $A0AB EXX
  $A0AC Return
;
; this routine plots to the real screen
  $A0AD Save screen pointer
  $A0AE 7 iterations/rows
@ $A0B0 label=dc_loop_a0b0
  $A0B0 Transfer a byte
  $A0B2 Move to next scanline (Y0++)
  $A0B3 Move to next row of glyph data
  $A0B4 Did we rollover?
  $A0B7 If no rollover goto loop
  $A0B9 Undo rollover
  $A0BD Y3++
  $A0C1 If no rollover goto loop
  $A0C3 There was rollover, so move it to Y6
  $A0C7 Loop until we've done 7 lines
  $A0C9 Restore screen pointer
  $A0CA Move to next column
  $A0CB Return

c $A0CC
@ $A0CC label=kempston_flag
B $A0CC,1 Set to 1 if Kempston joystick is chosen, 0 otherwise
B $A0CD,8 must be keydefs
@ $A0D5 label=user_input
B $A0D5,1 User input: QPBFUDLR - Quit Pause Boost Fire Up Down Left Right. Note that attract mode is driven through this var.
;
@ $A0D6 label=keyscan
C $A0D6 Main input handler
  $A0D6 If not Kempston then goto $A0F3
  $A0DC Read Kempston joystick port. Returns 000FUDLR active high
  $A0E0 PUSH AF      ;
  $A0E1 LD E,$20     ;
  $A0E3 LD HL,$A0CD  ;
  $A0E6 CALL $A112   ;
  $A0E9 RRCA         ;
  $A0EA RRCA         ;
  $A0EB RRCA         ;
  $A0EC AND $E0      ;
  $A0EE POP DE       ;
  $A0EF OR D         ;
  $A0F0 LD E,A       ;
  $A0F1 JR $A0FB     ;
;
@ $A0F3 label=keyscan_keyboard
  $A0F3 LD E,$01     ;
  $A0F5 LD HL,$A0CD  ;
  $A0F8 CALL $A112   ;
;
@ $A0FB label=keyscan_common
  $A0FB AND $03      ;
  $A0FD CP $03       ;
  $A0FF LD A,E       ;
  $A100 JR NZ,$A105  ;
  $A102 AND $FC      ;
  $A104 LD E,A       ;
;
  $A105 AND $0C      ;
  $A107 CP $0C       ;
  $A109 LD A,E       ;
  $A10A JR NZ,$A10E  ;
  $A10C AND $F3      ;
;
  $A10E LD ($A0D5),A ;
  $A111 Return
;
  $A112 LD A,(HL)    ; Outer keyboard loop
  $A113 INC HL       ;
  $A114 CALL $A11E   ;
  $A117 CCF          ; clear carry
  $A118 RL E         ;
  $A11A JR NC,$A112  ; loop while top bit set?
  $A11C LD A,E       ;
  $A11D Return
;
@ $A11E label=keyscan_inner
  $A11E LD C,A       ; Inner keyboard loop.... what's in #REGa?
  $A11F B=(A&7)+1, C=5-(A>>3)
  $A12D A = $FE ROR B
  $A132 IN A,($FE)   ; Port $FE is the AND of all columns/rows? This is the main keyboard access
;
  $A134 C = A ROR C
  $A138 Return

g $A139
  $A139,1 This is always zero, yet the code checks it. If it's set then $83F5 uses it to jump to $81AA, which is the end of a string. $8435 tests it too and calls <chatter> when it's zero, which it always is. $8AD6 also tries to use it to jump into a string (twice). Could be dead code or 128K version hook?
  $A13A,1 Current stage number
  $A13B,1 Used by $8425  -- seems to start at 4 then cycle 3/2/1 with each restart of the game, another random factor?
@ $A13C label=overtake_bonus
  $A13C,1 Overtake combo bonus counter. BCD. This increases by 2 for each overtake and is reset on a crash.
@ $A13D label=credits
  $A13D,1 Number of credits remaining (2 for a new game)
  $A13E,47 Canned data or Data restored for attract mode?
  $A16D
; this might be an idle timer, when it hits zero Raymond cajoles us
  $A16E,1 used during attract mode, road gradient/angle or something?
@ $A16F label=user_input_mask
  $A16F,1 user_input mask, set to $C0 (Quit+Pause) when perp is fully smashed, or $FF otherwise
@ $A170 label=turbos
  $A170,1 Number of turbo boosts remaining (3 for a new game)
W $A171,2 seems to be the horizon level, possibly relative (used during attract mode)
  $A174,1 Low/high gear flag?
  $A175,8 Score digits. One digit per byte, least significant first. This seems to be recording what's on screen so digit plotting can be bypassed.
@ $A175 label=score_digits
  $A175,8 Score. Stored as one digit per byte.
@ $A17D label=time_sixteenths
  $A17D,1 Seems to be a 1/16ths second counter. Counts from $F to $0. $A17E is decremented when it hits zero.
@ $A17E label=time_bcd
  $A17E,1 Time remaining. Stored as BCD.
@ $A17F label=time_digits
  $A17F,2 Time remaining. Stored as one digit per byte.
@ $A181 label=distance_digits
  $A181,4 Distance. Stored as one digit per byte.
W $A186,2 Attribute address of horizon. Points to last attribute on the line which shows the ground. (e.g. $59DF)
  $A188,1 Set by $843B
W $A191,2 Address of car_lods
W $A195,2 Set to $190 by fully_smashed
  $A19D Memory gets wiped
  $A220,1 Set to $F8 when the CHQ MONITORING SYSTEM screen is being shown. (draw_screen uses this to skip for some work, just tests for non-zero). #R$8014 sets this to the level number.
  $A223,1 Stage number as shown. Stored as ASCII.
  $A229,1 <Timing related?>
  $A22A,1 Used by $B6D6 and others
  $A22B,1 Used by $9D2E and others. Stops overtake bonus working.
;
@ $A22C label=bonus_flag
  $A22C,1 Bonus flag (1 triggers the effect)
;
@ $A22D label=bonus_counter
  $A22D,1 Counter for bonus effect (goes up to 7?)
;
@ $A22E label=sighted_flag
  $A22E,1 Set to 1 when the perp has been sighted. Enables flashing lights and the smash bar.
;
  $A22F,1 Set to 2 by fully_smashed
;
  $A230,1 Used by draw_screen
;
@ $A231 label=transition_control
  $A231,1 Transition control/counter. Set to zero when fill_attributes has run. Set to 4 while transitioning. Also 2 sometimes. Set to 1 when the perp has been caught and we're drawing mugshots.
;
@ $A232 label=smash_factor
  $A232,1 Set to 0..6 if (0..3, 4..6, 7..10, 11..13, 14..16, 17+) smashes
;
@ $A233 label=smash_counter
  $A233,1 Smash counter (0..20)
;
  $A234,1 Cycles 0/1/2/3 as the game runs.
  $A235,1 ... light toggle flashing related? seems to cycle 0/1 as the game runs.
  $A236,1
  $A237,1 Used by $88F2
  $A238,1 Used by $88F2
  $A23B,1 loaded by $821d,$8903,$c0ee,$fd21 set by $C102
  $A23E,1 written by $B322
  $A23F,1 numerous references, seems related to level position
W $A240,2 Used by $B8F6, $BBF7, $87E0  seems to point at $EE00..$EEFF
  $A248,1 Used by $C452
  $A249,1 Counter used by $BB6E
;
@ $A24A label=speed
W $A24A,2 Speed (max when in lo gear =~ $E6 hi gear =~ $168 turbo =~ $1FF)
;
  $A24D,1 written by $B32A
  $A24E,1 used in plot_scores_etc
  $A250,1 Turn severity (0/1/2)
  $A251,1 Some sort of flip flag used for car rendering (observed 0/1)
  $A252,1
  $A253,1 Low/high gear flag? or visual state?
  $A254,1 Cleared by $884B
  $A258,1 Used by $B8D8
  $A259,1 Used by $B92B
  $A256,1 Distance related perhaps?
  $A25A,1 Used by $B8D2
  $A25C,1 Used by $B84E
  $A25D,1 Used by $B85D
  $A268,1 Used by $BB69 - skips routine if not set
  $A26B,1 Used by $8432

b $A27A Font: 8x7 bitmap
B $A27A,,7 Exclamation mark
B $A281,,7 Open bracket
B $A288,,7 Close bracket
B $A28F,,7 Comma
B $A296,,7 Full stop
B $A29D,,7 0..9
B $A2E3,,7 A-Z

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
  $AE1B Increment overtake_bonus
  $AE1F Return
;
  $AF7E,3 Get smash_factor
  $AF91,3 HL -> $CDEC
  $AF9E,3 Get smash_factor
  $AFCF,3 Get smash_factor

c $AFF1
b $B045

c $B063
  $B080 Clear up/down/left/right bits of user input
  $B0A2 Decrement turbo boost count [POKE $B0A5 for Infinite turbos]
  $B0A9 TBD
  $B0AF Read user input
  $B1EC Cap speed to $1FF

c $B318
  $B318 If speed > 0 jump $B325
  $B31F TBD...

c $B457

c $B4CC
  $B4D8 time_sixteenths/$A17D = 15, time_bcd/$A17E = $60
  $B4E1 Point #REGhl at left light's attributes
  $B4E4 Toggle its brightness
  $B4E7,3 Point at "SIGHTING OF TARGET VEHICLE" message
  $B4EA
  $B4ED

@ $B4F0 smash
c $B4F0 Smash handling
  $B4F0 Cycle #REGa one step through 0..3 each time the routine is entered
  $B4F8 *$B55C = $CE33 + #REGa * 6
  $B506 $B54A = 9
  $B50B Load and increment smash_counter
  $B50F 20 hits? [POKE $B50F for Single hit capture]
  $B511 Exit via fully_smashed if so
  $B514 19 hits?
  $B516 Goto j_b522 if not
N $B518 We have 19 hits
  $B518
  $B519 A = 10
  $B51B HL = raymond_says_one_more_time
  $B51E Call chatter
  $B521
@ $B522 label=j_b522
  $B522 Set smash_counter to #REGa
  $B525 Set smash_factor to zero if smash_counter is zero
  $B52A Set smash_factor to 1 if smash_counter < 4
  $B52F Set smash_factor to 2 if smash_counter < 7
  $B534 Set smash_factor to 3 if smash_counter < 11
  $B539 Set smash_factor to 4 if smash_counter < 14
  $B53E Set smash_factor to 5 if smash_counter < 17
  $B543 Otherwise set smash_factor to 6
@ $B544 label=set_smash_factor
  $B544 Set smash_factor to #REGc
  $B548 Return

c $B549

@ $B58E label=draw_car
c $B58E Draws the car
R $B58E I:A TBD
  $B58E If #REGa is zero then $A251 = A
  $B594 C=A
  $B595 PUSH BC
  $B596 Point #REGhl at ? then add (#REGa * 4)
  $B59F Point #REGde at the car tiles [needs a proper label]
  $B5A2 C=7
  $B5A4 Call sub_b627
  $B5A7 POP BC
  $B5A8 117 - (self modified value $B5AB)
  $B5AC D=A
  $B5AD A=C+B+(self modified value $B5B0)
  $B5B1 If A >= 9 A -= 9
  $B5B7,3 Point #REGhl at car definitions[A] (entries are 20 bytes wide)
  $B5C4 PUSH DE
  $B5C5 A=*$A22A + *HL
  $B5C9 A=0-A
  $B5CB A+=D  (aka A=D-A)
  $B5CC E=A
  $B5CD A = A & $0F
  $B5CF A += $F0
  $B5D1 D=A
  $B5D2 A=E
  $B5D3 A &= $70
  $B5D5 A = A*2
  $B5D6 A += $0D
  $B5D8 E=A
  $B5D9 HL++
  $B5DA B=*HL++
  $B5DC A=*HL++
  $B5DE PUSH HL
  $B5DF PUSH DE
  $B5E0 H=*HL
  $B5E1 L=A
  $B5E2 A=5
  $B5E4 E=A
  $B5E5 D=0
  $B5E7 EXX
  $B5E8 POP HL
  $B5E9 EX AF,AF'
  $B5EA IF $A251 jump to draw_car_perhaps_flipped
  $B5F0 EX AF,AF'
  $B5F1 Call plot_sprite -- #REGa is (how many pixels to plot - 1) / 8
  $B5F4 Goto draw_car_cont
@ $B5F6 label=draw_car_perhaps_flipped
  $B5F6 EX AF,AF'
  $B5F7 L--
  $B5F8 Call plot_sprite_flipped
@ $B5FB label=draw_car_cont
  $B5FB POP HL
  $B5FC POP DE
  $B5FD HL++
  $B5FE E=$68 C=$05 (width) CALL $B627  draws the top (windscreen)
  $B605 E=$68 C=$05 (width) CALL $B627  draws the bottom (wheels)
  $B60C Check flip flag
  $B612 unflipped
  $B616 flipped
  $B618 Draws left side of car
; fallthrough
; C = <something> e.g. 7
; E = <something> e.g. $68
@ $B627 label=sub_b627
  $B627 A=D
  $B628 PUSH DE
  $B629 D=A - *HL++
  $B62C B=*HL++
  $B62E A=*HL++
  $B630 PUSH HL
  $B631 H=*HL
  $B632 L=A
  $B633 PUSH BC
  $B634 EXX
  $B635 POP BC
  $B636 A=*$A251
  $B639 B=A
  $B63A E=C
  $B63B C--
  $B63C If A==0 C=A
  $B640 EXX
  $B641 Draws right side of car
  $B6C4 Read from SUB @ $B5AA
  $B647 Return

c $B648
  $B6DE,5 Divide by 8

c $B67C
;
  $B701 Select a plotter? Could be clipping stuff too.
  $B708 This multiplies by six - the length of each load-mask-store step in the plotter core.
  $B70C Add that to #REGix
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
  $B76C #REGde = #REGa
  $B76F #REGhl += #REGde
  $B770
  $B771 Save #REGsp for restoration on exit
  $B775 Point #REGix at mystery table
  $B779 Subtract 8 ???
  $B77C,3 Multiply by 8
  $B77F Add to IX
  $B784,2 H = $EF
  $B794 Return
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
  $B7EF self modify $B71F - exit of masked_tile_plotter
  $B7F3 IX = &masked_tile_plotter_core_thingy (jump table)
  $B7F7 A = 8 - A
  $B7FA IX += A * 6
  $B803 B = $0F
  $B805
  $B806 D = 0
  $B808
  $B809
  $B80A H = D
  $B80B L = D
  $B80C A = B
  $B80D B = 5
  $B80F A--
  $B810 RLA
  $B811 RLA
@ $B812 label=j_b812
  $B812 RLA
  $B813 JR NC,j_b816
  $B815 HL += DE
@ $B816 label=j_b816
  $B816 HL += HL
  $B817 DJNZ j_b812
  $B819 RLA
  $B81A JR NC,$B81D
  $B81C HL += DE
  $B81D
  $B81E HL += BC
  $B81F
  $B820 D--
  $B821 E = -E
  $B825 JP masked_tile_plotter_entry

b $B828 breaks/crashes road rendering if messed with

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
  $BD93 Exit if sighted_flag is zero (flashing lights / smash mode)
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
  $C07A,3 Duplicate instruction

c $C15B Tunnel entrace/exit drawing code
  $C221 Jump table (self modified)
  $C236 Jump table (self modified)

c $C2E7

c $C452
  $C452 Self modify #REGsp restore instruction
  $C457 $A248 = 0
  $C45A Self modify 'LD A' at $C160 to be zero (tunnel drawing code)
  $C45D Self modify 'LD A' at $C88F to be zero
  $C460 Self modify 'LD A' at $C6D8 to be 3
  $C465 #REGiy = $E301
  $C469 C = $60 - IY[0]
  $C46F A = *$A240 + $40
  $C474 IX = $EE00 + A
  $C479 B = A
  $C47C $C6B3 = A & 1
  $C47F RR B twice
  $C483 HL = $D010
  $C486 A = $D0
  $C487 B = $55 -- Set this to $00 and the landscape goes blank - half the time
  $C489 If B carried out earlier then jump
  $C48B HL = $0030
  $C48E B = $00
  $C48F Self modify XOR at $C6D3 to be zero
  $C492 A = $00
  $C493 Self modify ADD A at $C677
  $C496 A = $30
  $C497 Self modify ADD A at $C651 to be $30
  $C49A A = $31
  $C49B Self modify ADD A at $C698 to be $31
  $C49E HL = $C534
  $C4A1 Self modify JP Z at $C4B2 to be $C534
  $C4A4 L = $FF
  $C4A6 DE = $0100
  $C4A9 A = B
  $C4AA Self modify ADD A at $C6BC to be zero?

; $C551 seems to be part of the stripes rendering

c $C57C smells like scanline/buffer pointer movement
  $C57C E -= 32
  $C580 jump if it went -ve
  $C583 D += 16
  $C587 jump

c $C58A ditto
  $C58A E -= 32
  $C58E jump if it went -ve
  $C591 D += 16
  $C595 jump

c $C598 Road plotting
  $C60A Jump table (self modified)
  $C61B Jump table (self modified)
  $C62C Jump table (self modified)
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
  $CA11 Jump table (self modified)
  $CA22 Jump table (self modified)
  $CA33 Jump table (self modified)
  $CA44 Jump table (self modified)
  $CA55 Jump table (self modified)

b $CBA4
c $CBC5
c $CBCE
c $CBD6
c $CD3A

; Fixed point 5.3? or 3.5?
c $CDD6 Multiplier
R $CDD6 I:A Multiplicand (value to multiply)
R $CDD6 I:C Multiplier (number to multiply by)
  $CDD6 3 iterations (something limited to 8?)
  $CDD8 Copy of value to destroy
  $CDD9 Initialise total
@ $CDDA label=mult_loop
  $CDDA Shift the most significant bit out
  $CDDC If the bit was set then total += multiplier
@ $CDDF label=mult_continue
  $CDDF total <<= 1
  $CDE0 While iterations remain, goto mult_loop
  $CDE2 Undo final shift
  $CDE3 Divide by 8 with rounding?
  $CDEB Return

b $CDEC

b $CE33
W $CE33,24,6
  $CE4B,19
  $CE5E,19
  $CE71,19
  $CE84,19
  $CE97,19

b $CEAA  ref'd by $B578

b $CEDA This must be the car draw instructions. 9 of them. 20 bytes per entry.
B $CEDA TBD
B $CEDB,1 14 rows
W $CEDC,2 Address of middle graphic part
B $CEDE,1 TBD
B $CEDF,1 rows
W $CEE0,2 -> TBD
B $CEE2,1 TBD
B $CEE3,1 rows
W $CEE4,2 -> TBD
B $CEE6,1 TBD
B $CEE7,1 rows
W $CEE8,2 -> TBD
B $CEEA,1 TBD
B $CEEB,1 rows
W $CEEC,2 -> TBD
L $CEDA,20,9

b $CF8E
B $CF8E,1 TBD
B $CF8F,1 rows
W $CF90,2 -> data
L $CF8E,4,3
;
W $CF9E,2 -> data
W $CFA4,2 -> data
W $CFAA,2 -> data
W $CFB0,2 -> data

b $D010

b $D20D Turbo smoke plume. 32x16 per frame. 4 frames. mask-bitmap arrangement.
B $D20D
B $D28D
B $D30D
B $D38D

b $D40D Middle sections of hero car. 9 sets
B $D40D,,5 Car middle 1 - 40x14px
B $D453,,5 Car middle 2 - 40x17px
B $D4A8,,5 Car middle 3 - 40x16px
B $D4F8,,5 Car middle 4 - 40x14px
B $D53E,,5 Car middle 5 - 40x17px
B $D593,,5 Car middle 6 - 40x15px
B $D5DE,,5 Car middle 7 - 40x14px
B $D624,,5 Car middle 8 - 40x16px
B $D674,,5 Car middle 9 - 40x16px
;
B $D6C4 chunks of car parts, looks 10 bytes wide

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
  $E852 Set up the stack
  $E855 Start the game
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
  $E925 Call clear_screen
  $E928 Point #REGhl at input menu messages (NUL terminated)
  $E92B Call menu_draw_strings
  $E931 Keyscan for 1, 2, 3, 4, 5
  $E93A Keyscan for 0, 9, 8, 7, 6
  $E93D Keyscan for P, O, I, U, Y
  $E940 Keyscan for ENTER, L, K, J, H
  $E943 Keyscan for SPACE, SYM SHFT, M, N, B
  $E95C Address of three bytes to populate $A0CD.. with
  $E95F Set Kempston flag
  $E963 Populate $A0CD
  $E969 Populate $A0D0
  $E970 Call clear_screen
  $E973 Address of "control options cannot be remodified" text (NUL terminated)
  $E976 Call menu_draw_strings
  $E97D Keyscan
  $E99C Goto clear_screen

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

@ $EBF7 label=menu_draw_strings
c $EBF7 Renders strings until it hits a NUL byte
  $EBF7 Call menu_draw_string
  $EBFA If the next byte's zero then return
  $EBFD Otherwise loop

@ $EBFF label=menu_draw_string
c $EBFF Renders a string.
R $EBFF I:HL Address of a message structure (byte: attribute byte, word: destination screen address, bytes: top bit set terminated ASCII string)
R $EBFF O:HL Address of next unconsumed byte
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
  $ECF3 Call clear_screen
  $ECF6 Address of "REDEFINE KEYS" strings
  $ECF9 Call menu_draw_strings
  $ED23 Test keys are "SHOCKED<ENTER>"
  $ED32 Set test mode flag
  $ED37 Call clear_screen
  $ED3A Address of TEST MODE strings
  $ED3D,3 Call menu_draw_strings
  $ED40,3 Call define_keys
  $ED43 Debounce?

c $ED4D
  $ED94,3 -> "B N M ..."
  $EDBD,3 -> test mode strings

t $EDD6 Names of keys

b $EE26 Sinclair joystick input scheme
b $EE2B Cursor joystick input scheme
b $EE30 "SHOCKED<ENTER>" ?
b $EE38 temp input scheme buffer?
b $EE3D
b $EE40

c $EE6E

@ $EE9E label=define_keys
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
