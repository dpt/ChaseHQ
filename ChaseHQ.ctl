> $4000 ; ChaseHQ.skool
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
> $4000 ; http://www.extentofthejam.com/pseudo/
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
> $4000 ; - High score entry
> $4000 ; - Logo animation > music plays > attract mode  (48K just does attract mode)
> $4000 ; - Best Officers (high score) on attract screen
> $4000 ; - Input device/define keys is retained
> $4000 ;
> $4000 ;
> $4000 ; MEMORY MAP
> $4000 ; ----------
> $4000 ; The memory map changes once the game's been shifted into its regular
> $4000 ; running state.
> $4000 ;
> $4000 ; $5B00..$5BFF is a pre-shifted version of the backdrop
> $4000 ; $5C00..$5CFF is the regular version of the backdrop
> $4000 ; $8DBB (word) is the address of the current transition animation
> $4000 ; $E500        is ?
> $4000 ; $E600..$E7FF is road drawing data
> $4000 ; $E900        is a table
> $4000 ; $EA30..$EAFE (words) is 104 words related to road drawing
> $4000 ; $EB00        is ?
> $4000 ; $ED28        is the stack (growing downwards)
> $4000 ; $EE00..$EEFF is the road buffer. holds data unpacked from maps. it's cyclic. fixed sections for each datum. cleared by $87DD.
> $4000 ; $EF00..$EFFF is a table of flipped bytes
> $4000 ; $F000..$FFFF is a 4KB back buffer
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
> $4000 ; - is attract mode a CPU player or a recording? how does it loop?
> $4000 ; - exact scoring rules
> $4000 ; - overhead tunnel type drawing
> $4000 ; - background music/drums on menu screen
> $4000 ;
@ $4000 org
@ $4000 set-warnings=1
@ $4000 start
@ $4000 writer=:ChaseHQ.ChaseHQAsmWriter
b $4000 Screen memory
B $4000,6144,8 Screen bitmap
B $5800,768,8 Screen attributes
c $5B00 Could be loader code
C $5B22,9 Point #REGhl at $5B7D
C $5B2B,6 Fetch a word from #REGhl and stack it
C $5B31,27 Routine that ...
C $5B4C,12 Game entry point
C $5B58,14 Routine that ...
C $5B66,1 Routine that ...
C $5B67,14 Move $5C00..$76EF to $C000 onwards
C $5B75,8 Routine that loads a word then OUTs $FE with zero, then jumps to that word
N $5B7D Some sort of instruction stream
W $5B7D,2,2 Probable address of routine $5B31
B $5B7F,3,3
B $5B82,2,2 Probable address of routine $5B75
B $5B84,3,3
W $5B87,2,2 Probable address of routine $5B31
B $5B89,4,4
W $5B8D,2,2 Probable address of routine $5B58
B $5B8F,2,2
W $5B91,2,2 Probable address of routine $5B66
W $5B93,2,2 Probable address of routine $5B31
B $5B95,4,4
W $5B99,2,2 Probable address of routine $5B58
B $5B9B,2,2
W $5B9D,2,2 Probable address of routine $5B31
B $5B9F,4,4
W $5BA3,2,2 Probable address of routine $5B31
B $5BA5,48,8
B $5BD5,43,8*5,3 gets IX pointed at it by $5B32
b $5C00 Graphics
D $5C00 #HTML[#CALL:graphic($5C00,80,24,0,1)]
B $5C00,240,8 Hill backdrop (80x24) inverted. Varies per level
b $5CF0 Per-level data
@ $5CF0 label=perp_mugshot_attributes
W $5CF0,2,2 Address of perp's mugshot attributes
@ $5CF2 label=perp_mugshot_bitmap
W $5CF2,2,2 Address of perp's mugshot bitmap
@ $5CF4 label=ground_colour
W $5CF4,2,2 Screen attributes used for the ground colour (a pair of matching bytes)
W $5CF6,2,2
W $5CF8,2,2 Loaded by $900F. Points to an array of 7 byte entries.
W $5CFA,2,2 Address of graphic/object definitions.
W $5CFC,2,2 -> "Entry 3"
W $5CFE,2,2 <turn sign lods> inconsistent...
@ $5D00 label=data_5d00
W $5D00,2,2 -> "Entry 9 (turn sign)"
W $5D02,2,2 -> "Entry 12"
W $5D04,2,2 -> Nancy's report
W $5D06,2,2 -> Arrest messages
B $5D08,4,4
w $5D0C Table of addresses of LODs (levels of detail - a set of sprites of various sizes representing the same object).
@ $5D0C label=lods_table
W $5D0C,2,2 -> stones_lods
W $5D0E,2,2 -> dust_lods
W $5D10,2,2 -> car_lods (perp's car)
W $5D12,2,2 -> lambo_lods
W $5D14,2,2 -> truck_lods
W $5D16,2,2 -> lambo_lods
W $5D18,2,2 -> car_lods
b $5D1A Data block at 5D1A
B $5D1A,1,1 loaded by $A813
B $5D1B,1,1 loaded by $A6A7
B $5D1C,1,1 loaded by $A759
w $5D1D Data block at 5D1D
@ $5D1D label=setup_game_data
W $5D1D,2,2 road_pos
N $5D1F These all point a byte earlier than the real data start point.
W $5D1F,2,2 -> Start stretch, curvature
W $5D21,2,2 -> Start stretch, height
W $5D23,2,2 -> Start stretch, lanes
W $5D25,2,2 -> Start stretch, right-side objects
W $5D27,2,2 -> Start stretch, left-side objects
W $5D29,2,2 -> Start stretch, hazards
w $5D2B Data block at 5D2B
@ $5D2B label=attract_data
W $5D2B,2,2 road_pos
N $5D2D These all point a byte earlier than the real data start point.
W $5D2D,2,2 -> Loop section, curvature
W $5D2F,2,2 -> Loop section, height
W $5D31,2,2 -> Loop section, lanes
W $5D33,2,2 -> Loop section, right-side objects
W $5D35,2,2 -> Loop section, left-side objects
W $5D37,2,2 -> Loop section, hazards
b $5D39 Nancy's report
B $5D39,1,1 Nancy ($01)
W $5D3A,2,2 Points at "THIS IS NANCY..."
W $5D3C,2,2 Points at "EMERGENCY HERE..."
W $5D3E,2,2 Points at "IS FLEEING ..."
W $5D40,2,2 Points at "VEHICLE IS..."
B $5D42,1,1 (pause?)
W $5D43,2,2 -> Random choice of ("WE READ..." / "ROGER!" / "GOTCHA...")
b $5D45 Messages - Nancy
T $5D45,40,39:n1 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $5D6D,40,39:n1 "EMERGENCY HERE. RALPH THE IDAHO SLASHER,"
T $5D95,42,41:n1 "IS FLEEING TOWARDS THE SUBURBS. THE TARGET"
T $5DBF,46,45:n1 "VEHICLE IS A WHITE BRITISH SPORTS CAR... OVER."
b $5DED Messages - Arrest
B $5DED,1,1 Frame delay until first message?
B $5DEE,1,1 Frame delay until next message?
B $5DEF,1,1 Flags (?)
B $5DF0,1,1 Attribute (black)
W $5DF1,2,2 Back buffer address
W $5DF3,2,2 Attribute address
T $5DF5,27,26:n1 "OK! YOU ARE UNDER ARREST ON"
B $5E10,1,1 Frame delay until next message?
B $5E11,1,1 Flags (?)
B $5E12,1,1 Attribute (black)
W $5E13,2,2 Back buffer address
W $5E15,2,2 Attribute address
T $5E17,26,25:n1 "SUSPICION OF FIRST DEGREE "
B $5E31,1,1 Frame delay until next message?
B $5E32,1,1 Flags (?)
B $5E33,1,1 Attribute (black)
W $5E34,2,2 Back buffer address
W $5E36,2,2 Attribute address
T $5E38,6,5:n1 "MURDER"
B $5E3E,1,1 Frame delay until next message?
b $5E3F Graphics or object? definitions
D $5E3F All are stored inverted except where noted.
B $5E3F,1,1 Pointed to by $5CFA
b $5E40 Pointed to by $5CF6
B $5E40,1,1
W $5E41,2,2 Address of tumbleweed_lods table
B $5E43,1,1
W $5E44,2,2 Address of barrier_lods table
N $5E46 Entry 1
B $5E46,3,3
W $5E49,2,2 Arg for routine passed in DE
W $5E4B,2,2 -> Routine at $9252
N $5E4D Entry 2 (empty)
B $5E4D,3,3
W $5E50,4,2
N $5E54 Entry 3
B $5E54,3,3
W $5E57,2,2 Arg for routine passed in DE
W $5E59,2,2 -> Routine at $9171
N $5E5B Entry 4
B $5E5B,3,3 remaining bytes of this entry
W $5E5E,2,2 Arg for routine passed in DE
W $5E60,2,2 -> Routine at $9171. Loaded into HL at $9017 then jumped to.
N $5E62 Entry 5
B $5E62,3,3 remaining bytes of this entry
W $5E65,2,2 Arg for routine passed in DE
W $5E67,2,2 -> Routine at $9171
N $5E69 Entry 6
B $5E69,3,3 remaining bytes of this entry
W $5E6C,2,2 Arg for routine passed in DE
W $5E6E,2,2 -> Routine at $9171
N $5E70 Entry 7
B $5E70,3,3 remaining bytes of this entry
W $5E73,2,2 Arg for routine passed in DE
W $5E75,2,2 -> Routine at $9171
N $5E77 Entry 8
B $5E77,3,3 remaining bytes of this entry
W $5E7A,2,2 Arg for routine passed in DE
W $5E7C,2,2 Routine?
N $5E7E Entry 9 (turn sign)
B $5E7E,3,3 remaining bytes of this entry
W $5E81,2,2 -> -> turn_sign_lods
W $5E83,2,2 -> Routine at $92E1
N $5E85 Entry 10
B $5E85,3,3
W $5E88,2,2 Arg
W $5E8A,2,2 -> Routine at $924D
N $5E8C Entry 11 (empty)
B $5E8C,3,3
W $5E8F,4,2
N $5E93 Entry 12
B $5E93,3,3
W $5E96,2,2 Arg
W $5E98,2,2 -> Routine at $916C
N $5E9A Entry 13
B $5E9A,3,3
W $5E9D,2,2 Arg
W $5E9F,2,2 -> Routine at $916C
N $5EA1 Entry 14
B $5EA1,3,3
W $5EA4,2,2 Arg
W $5EA6,2,2 -> Routine at $916C
N $5EA8 Entry 15
B $5EA8,3,3
W $5EAB,2,2 Arg
W $5EAD,2,2 -> Routine at $916C
N $5EAF Entry 16
B $5EAF,3,3
W $5EB2,2,2 Arg
W $5EB4,2,2 -> Routine at $916C
N $5EB6 Entry 17
B $5EB6,3,3
W $5EB9,2,2 Arg
W $5EBB,2,2 -> Routine at $9278
N $5EBD Entry 18 (turn sign)
B $5EBD,3,3
W $5EC0,2,2 Arg
W $5EC2,2,2 -> Routine at $9278
b $5EC4 Initial map segment
N $5EC4 Start stretch
N $5EC4 Start stretch, curvature
B $5EC4,32,8
N $5EE4 Start stretch, height
B $5EE4,43,8*5,3
N $5F0F Start stretch, lanes
B $5F0F,14,8,6
N $5F1D Start stretch, hazards
B $5F1D,13,8,5
N $5F2A Start stretch, left-side objects
B $5F2A,78,8*9,6
N $5F78 Start stretch, right-side objects
B $5F78,90,8*11,2
N $5FD2 Left fork
N $5FD2 Left fork, curvature
B $5FD2,20,8*2,4
N $5FE6 Left fork, height
B $5FE6,29,8*3,5
N $6003 Left fork, lanes
B $6003,6,6
N $6009 Left fork, hazards
B $6009,8,8
N $6011 Left fork, left-side objects
B $6011,65,8*8,1
N $6052 Left fork, right-side objects
B $6052,54,8*6,6
N $6088 Right fork (dirt track), curvature
B $6088,27,8*3,3
N $60A3 Right fork (dirt track), height
B $60A3,66,8*8,2
N $60E5 Right fork (dirt track), lanes
B $60E5,10,8,2
N $60EF Right fork (dirt track), hazards
B $60EF,26,8*3,2
N $6109 Right fork (dirt track), left-side objects
B $6109,58,8*7,2
N $6143 Right fork (dirt track), right-side objects
B $6143,48,8
N $6173 Tunnel section, curvature
B $6173,22,8*2,6
N $6189 Tunnel section, height
B $6189,28,8*3,4
N $61A5 Tunnel section, lanes
B $61A5,20,8*2,4
N $61B9 Tunnel section, hazards
B $61B9,5,5
N $61BE Tunnel section, left-side objects
B $61BE,11,8,3
N $61C9 Tunnel section, right-side objects
B $61C9,11,8,3
N $61D4 Loop section, curvature
B $61D4,53,8*6,5
N $6209 Loop section, height
B $6209,84,8*10,4
N $625D Loop section, lanes
B $625D,26,8*3,2
N $6277 Loop section, hazards
B $6277,29,8*3,5
N $6294 Loop section, left-side objects
B $6294,128,8
N $6314 Loop section, right-side objects
B $6314,118,8*14,6
b $638A Perp's face
N $638A Bitmap data for perp w/ sunglasses (32x40). Stored top-down.
N $638A #HTML[#CALL:face($638A)]
B $638A,160,4
N $642A Attribute data for perp w/ sunglasses (4x5). Stored top-down.
B $642A,20,4
b $643E LODs
D $643E LOD = level of detail. These structures collect together the variously sized versions of the same game object.
N $643E Lamborghini LOD
@ $643E label=lambo_lods
B $643E,1,1 Width (bytes)
B $643F,1,1 Flags
B $6440,1,1 Height (pixels)
W $6441,2,2 Bitmap
W $6443,2,2 Pre-shifted bitmap
N $6445 Lamborghini LOD
B $6445,1,1 Width (bytes)
B $6446,1,1 Flags
B $6447,1,1 Height (pixels)
W $6448,2,2 Bitmap
W $644A,2,2 Pre-shifted bitmap
N $644C Lamborghini LOD
B $644C,1,1 Width (bytes)
B $644D,1,1 Flags
B $644E,1,1 Height (pixels)
W $644F,2,2 Bitmap
W $6451,2,2 Pre-shifted bitmap
N $6453 Lamborghini LOD
B $6453,1,1 Width (bytes)
B $6454,1,1 Flags
B $6455,1,1 Height (pixels)
W $6456,2,2 Bitmap
W $6458,2,2 Pre-shifted bitmap
N $645A Lamborghini LOD
B $645A,1,1 Width (bytes)
B $645B,1,1 Flags
B $645C,1,1 Height (pixels)
W $645D,2,2 Bitmap
W $645F,2,2 Pre-shifted bitmap
N $6461 Lamborghini LOD
B $6461,1,1 Width (bytes)
B $6462,1,1 Flags
B $6463,1,1 Height (pixels)
W $6464,2,2 Bitmap
W $6466,2,2 Pre-shifted bitmap
N $6468 Truck LOD
@ $6468 label=truck_lods
B $6468,1,1 Width (bytes)
B $6469,1,1 Flags
B $646A,1,1 Height (pixels)
W $646B,2,2 Bitmap
W $646D,2,2 Pre-shifted bitmap
N $646F Truck LOD
B $646F,1,1 Width (bytes)
B $6470,1,1 Flags
B $6471,1,1 Height (pixels)
W $6472,2,2 Bitmap
W $6474,2,2 Pre-shifted bitmap
N $6476 Truck LOD
B $6476,1,1 Width (bytes)
B $6477,1,1 Flags
B $6478,1,1 Height (pixels)
W $6479,2,2 Bitmap
W $647B,2,2 Pre-shifted bitmap
N $647D Truck LOD
B $647D,1,1 Width (bytes)
B $647E,1,1 Flags
B $647F,1,1 Height (pixels)
W $6480,2,2 Bitmap
W $6482,2,2 Pre-shifted bitmap
N $6484 Truck LOD
B $6484,1,1 Width (bytes)
B $6485,1,1 Flags
B $6486,1,1 Height (pixels)
W $6487,2,2 Bitmap
W $6489,2,2 Pre-shifted bitmap
N $648B Truck LOD
B $648B,1,1 Width (bytes)
B $648C,1,1 Flags
B $648D,1,1 Height (pixels)
W $648E,2,2 Bitmap
W $6490,2,2 Pre-shifted bitmap
N $6492 Car LOD
@ $6492 label=car_lods
B $6492,1,1 Width (bytes)
B $6493,1,1 Flags
B $6494,1,1 Height (pixels)
W $6495,2,2 Bitmap
W $6497,2,2 Pre-shifted bitmap
N $6499 Car LOD
B $6499,1,1 Width (bytes)
B $649A,1,1 Flags
B $649B,1,1 Height (pixels)
W $649C,2,2 Bitmap
W $649E,2,2 Pre-shifted bitmap
N $64A0 Car LOD
B $64A0,1,1 Width (bytes)
B $64A1,1,1 Flags
B $64A2,1,1 Height (pixels)
W $64A3,2,2 Bitmap
W $64A5,2,2 Pre-shifted bitmap
N $64A7 Car LOD
B $64A7,1,1 Width (bytes)
B $64A8,1,1 Flags
B $64A9,1,1 Height (pixels)
W $64AA,2,2 Bitmap
W $64AC,2,2 Pre-shifted bitmap
N $64AE Car LOD
B $64AE,1,1 Width (bytes)
B $64AF,1,1 Flags
B $64B0,1,1 Height (pixels)
W $64B1,2,2 Bitmap
W $64B3,2,2 Pre-shifted bitmap
N $64B5 Car LOD
B $64B5,1,1 Width (bytes)
B $64B6,1,1 Flags
B $64B7,1,1 Height (pixels)
W $64B8,2,2 Bitmap
W $64BA,2,2 Pre-shifted bitmap
N $64BC Lamborghini_1 (48x30)
N $64BC #HTML[#CALL:graphic($64BC,48,30,0,1)]
B $64BC,180,6 Bitmap data
N $6570 Lamborghini_2 (40x22)
N $6570 #HTML[#CALL:graphic($6570,40,22,0,1)]
B $6570,110,5 Bitmap data
N $65DE Lamborghini_3 (24x15)
N $65DE #HTML[#CALL:graphic($65DE,24,15,0,1)]
B $65DE,45,3 Bitmap data
N $660B Truck_1 (48x39)
N $660B #HTML[#CALL:graphic($660B,48,39,0,1)]
B $660B,234,6 Bitmap data
N $66F5 Truck_2 (40x29)
N $66F5 #HTML[#CALL:graphic($66F5,40,29,0,1)]
B $66F5,145,5 Bitmap data
N $6786 Truck_3 (24x20)
N $6786 #HTML[#CALL:graphic($6786,24,20,0,1)]
B $6786,60,3 Bitmap data
N $67C2 Car_1 (48x31)
N $67C2 #HTML[#CALL:graphic($67C2,48,31,0,1)]
B $67C2,186,6 Bitmap data
N $687C Car_2 (40x22)
N $687C #HTML[#CALL:graphic($687C,40,22,0,1)]
B $687C,110,5 Bitmap data
N $68EA Car_3 (24x16)
N $68EA #HTML[#CALL:graphic($68EA,24,16,0,1)]
B $68EA,48,3 Bitmap data
N $691A Lamborghini_4 (24x8)
N $691A #HTML[#CALL:graphic($691A,24,8,1,1)]
B $691A,48,6 Masked bitmap data
N $694A Lamborghini_5 (24x8) pre-shifted
N $694A #HTML[#CALL:graphic($694A,24,8,1,1)]
B $694A,48,6 Masked bitmap data
N $697A Truck_4/5 (16x12)
N $697A #HTML[#CALL:graphic($697A,16,12,1,1)]
B $697A,48,4 Masked bitmap data
N $69AA Truck_4/5 (16x12) pre-shifted
N $69AA #HTML[#CALL:graphic($69AA,16,12,1,1)]
B $69AA,48,4 Masked bitmap data
N $69DA Car_4/5 (24x9)
N $69DA #HTML[#CALL:graphic($69DA,24,9,1,1)]
B $69DA,54,6 Masked bitmap data
N $6A10 Car_4/5 (24x9) pre-shifted
N $6A10 #HTML[#CALL:graphic($6A10,24,9,1,1)]
B $6A10,54,6 Masked bitmap data
N $6A46 Stones LOD
@ $6A46 label=stones_lods
B $6A46,1,1 Width (bytes)
B $6A47,1,1 Flags
B $6A48,1,1 Height (pixels)
W $6A49,2,2 Bitmap
W $6A4B,2,2 Pre-shifted bitmap
N $6A4D Stones LOD
B $6A4D,1,1 Width (bytes)
B $6A4E,1,1 Flags
B $6A4F,1,1 Height (pixels)
W $6A50,2,2 Bitmap
W $6A52,2,2 Pre-shifted bitmap
N $6A54 Stones LOD
B $6A54,1,1 Width (bytes)
B $6A55,1,1 Flags
B $6A56,1,1 Height (pixels)
W $6A57,2,2 Bitmap
W $6A59,2,2 Pre-shifted bitmap
N $6A5B Stones LOD
B $6A5B,1,1 Width (bytes)
B $6A5C,1,1 Flags
B $6A5D,1,1 Height (pixels)
W $6A5E,2,2 Bitmap
W $6A60,2,2 Pre-shifted bitmap
N $6A62 Stones LOD
B $6A62,1,1 Width (bytes)
B $6A63,1,1 Flags
B $6A64,1,1 Height (pixels)
W $6A65,2,2 Bitmap
W $6A67,2,2 Pre-shifted bitmap
N $6A69 Stones LOD
B $6A69,1,1 Width (bytes)
B $6A6A,1,1 Flags
B $6A6B,1,1 Height (pixels)
W $6A6C,2,2 Bitmap
W $6A6E,2,2 Pre-shifted bitmap
N $6A70 Dust LOD
@ $6A70 label=dust_lods
B $6A70,1,1 Width (bytes)
B $6A71,1,1 Flags
B $6A72,1,1 Height (pixels)
W $6A73,2,2 Bitmap
W $6A75,2,2 Pre-shifted bitmap
N $6A77 Dust LOD
B $6A77,1,1 Width (bytes)
B $6A78,1,1 Flags
B $6A79,1,1 Height (pixels)
W $6A7A,2,2 Bitmap
W $6A7C,2,2 Pre-shifted bitmap
N $6A7E Dust LOD
B $6A7E,1,1 Width (bytes)
B $6A7F,1,1 Flags
B $6A80,1,1 Height (pixels)
W $6A81,2,2 Bitmap
W $6A83,2,2 Pre-shifted bitmap
N $6A85 Dust LOD
B $6A85,1,1 Width (bytes)
B $6A86,1,1 Flags
B $6A87,1,1 Height (pixels)
W $6A88,2,2 Bitmap
W $6A8A,2,2 Pre-shifted bitmap
N $6A8C Dust LOD
B $6A8C,1,1 Width (bytes)
B $6A8D,1,1 Flags
B $6A8E,1,1 Height (pixels)
W $6A8F,2,2 Bitmap
W $6A91,2,2 Pre-shifted bitmap
N $6A93 Dust LOD
B $6A93,1,1 Width (bytes)
B $6A94,1,1 Flags
B $6A95,1,1 Height (pixels)
W $6A96,2,2 Bitmap
W $6A98,2,2 Pre-shifted bitmap
N $6A9A Stones (16x5)
N $6A9A #HTML[#CALL:graphic($6A9A,16,5,1,1)]
B $6A9A,20,4 Masked bitmap data
N $6AAE Stones (16x4)
N $6AAE #HTML[#CALL:graphic($6AAE,16,4,1,1)]
B $6AAE,16,4 Masked bitmap data
N $6ABE Stones (16x4) pre-shifted
N $6ABE #HTML[#CALL:graphic($6ABE,16,4,1,1)]
B $6ABE,16,4 Masked bitmap data
N $6ACE Stones (16x3)
N $6ACE #HTML[#CALL:graphic($6ACE,16,3,1,1)]
B $6ACE,12,4 Masked bitmap data
N $6ADA Stones (16x3) pre-shifted
N $6ADA #HTML[#CALL:graphic($6ADA,16,3,1,1)]
B $6ADA,12,4 Masked bitmap data
N $6AE6 Stones (8x2)
N $6AE6 #HTML[#CALL:graphic($6AE6,8,2,1,1)]
B $6AE6,4,2 Masked bitmap data
N $6AEA Stones (8x2) pre-shifted
N $6AEA #HTML[#CALL:graphic($6AEA,8,2,1,1)]
B $6AEA,4,2 Masked bitmap data
N $6AEE Stones (8x1)
N $6AEE #HTML[#CALL:graphic($6AEE,8,1,1,1)]
B $6AEE,2,2 Masked bitmap data
N $6AF0 Stones (8x1) pre-shifted
N $6AF0 #HTML[#CALL:graphic($6AF0,8,1,1,1)]
B $6AF0,2,2 Masked bitmap data
N $6AF2 Dust (8x1)
N $6AF2 #HTML[#CALL:graphic($6AF2,8,1,1,1)]
B $6AF2,2,2 Masked bitmap data
N $6AF4 Dust (8x1) pre-shifted
N $6AF4 #HTML[#CALL:graphic($6AF4,8,1,1,1)]
B $6AF4,2,2 Masked bitmap data
W $6AF6,2,2 -> turn_sign_lods
B $6AF8,20,8*2,4 Read by $928D and $92F6
N $6B0C Ref'd by graphic entry 8 and 17
W $6B0C,2,2 -> turn_sign_lods
B $6B0E,20,8*2,4
N $6B22 Turn sign LOD
@ $6B22 label=turn_sign_lods
B $6B22,1,1 Width (bytes)
B $6B23,1,1 Flags
B $6B24,1,1 Height (pixels)
W $6B25,2,2 Bitmap
W $6B27,2,2 Pre-shifted bitmap
N $6B29 Turn sign LOD
B $6B29,1,1 Width (bytes)
B $6B2A,1,1 Flags
B $6B2B,1,1 Height (pixels)
W $6B2C,2,2 Bitmap
W $6B2E,2,2 Pre-shifted bitmap
N $6B30 Turn sign LOD
B $6B30,1,1 Width (bytes)
B $6B31,1,1 Flags
B $6B32,1,1 Height (pixels)
W $6B33,2,2 Bitmap
W $6B35,2,2 Pre-shifted bitmap
N $6B37 Turn sign LOD
B $6B37,1,1 Width (bytes)
B $6B38,1,1 Flags
B $6B39,1,1 Height (pixels)
W $6B3A,2,2 Bitmap
W $6B3C,2,2 Pre-shifted bitmap
N $6B3E Turn sign LOD
B $6B3E,1,1 Width (bytes)
B $6B3F,1,1 Flags
B $6B40,1,1 Height (pixels)
W $6B41,2,2 Bitmap
W $6B43,2,2 Pre-shifted bitmap
N $6B45 Turn sign LOD
B $6B45,1,1 Width (bytes)
B $6B46,1,1 Flags
B $6B47,1,1 Height (pixels)
W $6B48,2,2 Bitmap
W $6B4A,2,2 Pre-shifted bitmap
N $6B4C Turn sign LOD
B $6B4C,1,1 Width (bytes)
B $6B4D,1,1 Flags
B $6B4E,1,1 Height (pixels)
W $6B4F,2,2 Bitmap
W $6B51,2,2 Pre-shifted bitmap
N $6B53 Turn sign LOD
B $6B53,1,1 Width (bytes)
B $6B54,1,1 Flags
B $6B55,1,1 Height (pixels)
W $6B56,2,2 Bitmap
W $6B58,2,2 Pre-shifted bitmap
N $6B5A Turn sign LOD
B $6B5A,1,1 Width (bytes)
B $6B5B,1,1 Flags
B $6B5C,1,1 Height (pixels)
W $6B5D,2,2 Bitmap
W $6B5F,2,2 Pre-shifted bitmap
N $6B61 Turn sign LOD
B $6B61,1,1 Width (bytes)
B $6B62,1,1 Flags
B $6B63,1,1 Height (pixels)
W $6B64,2,2 Bitmap
W $6B66,2,2 Pre-shifted bitmap
N $6B68 Turn right sign (32x40)
N $6B68 #HTML[#CALL:graphic($6B68,32,40,0,1)]
B $6B68,160,4
N $6C08 Turn right sign (24x30)
N $6C08 #HTML[#CALL:graphic($6C08,24,30,0,1)]
B $6C08,90,3
N $6C62 Turn right sign (16x20)
N $6C62 #HTML[#CALL:graphic($6C62,16,20,0,1)]
B $6C62,40,2
N $6C8A Turn right sign (16x16)
N $6C8A #HTML[#CALL:graphic($6C8A,16,16,1,1)]
B $6C8A,64,4 Masked bitmap data
N $6CCA Turn right sign (16x13)
N $6CCA #HTML[#CALL:graphic($6CCA,16,13,1,1)]
B $6CCA,52,2 Masked bitmap data
N $6CFE Turn right sign (16x13) pre-shifted
N $6CFE #HTML[#CALL:graphic($6CFE,16,13,1,1)]
B $6CFE,52,2 Masked bitmap data
N $6D32 Turn right sign (16x10)
N $6D32 #HTML[#CALL:graphic($6D32,16,10,1,1)]
B $6D32,40,4 Masked bitmap data
N $6D5A Turn right sign (16x10) pre-shifted
N $6D5A #HTML[#CALL:graphic($6D5A,16,10,1,1)]
B $6D5A,40,4 Masked bitmap data
N $6D82 LOD
@ $6D82 label=tumbleweed_lods
B $6D82,1,1 Width (bytes)
B $6D83,1,1 Flags
B $6D84,1,1 Height (pixels)
W $6D85,2,2 Bitmap
W $6D87,2,2 Pre-shifted bitmap
N $6D89 LOD
B $6D89,1,1 Width (bytes)
B $6D8A,1,1 Flags
B $6D8B,1,1 Height (pixels)
W $6D8C,2,2 Bitmap
W $6D8E,2,2 Pre-shifted bitmap
N $6D90 LOD
B $6D90,1,1 Width (bytes)
B $6D91,1,1 Flags
B $6D92,1,1 Height (pixels)
W $6D93,2,2 Bitmap
W $6D95,2,2 Pre-shifted bitmap
N $6D97 LOD
B $6D97,1,1 Width (bytes)
B $6D98,1,1 Flags
B $6D99,1,1 Height (pixels)
W $6D9A,2,2 Bitmap
W $6D9C,2,2 Pre-shifted bitmap
N $6D9E LOD
B $6D9E,1,1 Width (bytes)
B $6D9F,1,1 Flags
B $6DA0,1,1 Height (pixels)
W $6DA1,2,2 Bitmap
W $6DA3,2,2 Pre-shifted bitmap
N $6DA5 LOD
B $6DA5,1,1 Width (bytes)
B $6DA6,1,1 Flags
B $6DA7,1,1 Height (pixels)
W $6DA8,2,2 Bitmap
W $6DAA,2,2 Pre-shifted bitmap
N $6DAC Tumbleweed_1 (16x16)
N $6DAC #HTML[#CALL:graphic($6DAC,16,16,0,1)]
B $6DAC,32,2
N $6DCC Tumbleweed_2 (16x11)
N $6DCC #HTML[#CALL:graphic($6DCC,16,11,0,1)]
B $6DCC,22,2
N $6DE2 Tumbleweed_3 (8x9)
N $6DE2 #HTML[#CALL:graphic($6DE2,8,9,0,1)]
B $6DE2,9,1
N $6DEB Tumbleweed_4 (8x7)
N $6DEB #HTML[#CALL:graphic($6DEB,8,7,0,1)]
B $6DEB,7,1
N $6DF2 LOD
@ $6DF2 label=barrier_lods
B $6DF2,1,1 Width (bytes)
B $6DF3,1,1 Flags
B $6DF4,1,1 Height (pixels)
W $6DF5,2,2 Bitmap
W $6DF7,2,2 Pre-shifted bitmap
N $6DF9 LOD
B $6DF9,1,1 Width (bytes)
B $6DFA,1,1 Flags
B $6DFB,1,1 Height (pixels)
W $6DFC,2,2 Bitmap
W $6DFE,2,2 Pre-shifted bitmap
N $6E00 LOD
B $6E00,1,1 Width (bytes)
B $6E01,1,1 Flags
B $6E02,1,1 Height (pixels)
W $6E03,2,2 Bitmap
W $6E05,2,2 Pre-shifted bitmap
N $6E07 LOD
B $6E07,1,1 Width (bytes)
B $6E08,1,1 Flags
B $6E09,1,1 Height (pixels)
W $6E0A,2,2 Bitmap
W $6E0C,2,2 Pre-shifted bitmap
N $6E0E LOD
B $6E0E,1,1 Width (bytes)
B $6E0F,1,1 Flags
B $6E10,1,1 Height (pixels)
W $6E11,2,2 Bitmap
W $6E13,2,2 Pre-shifted bitmap
N $6E15 LOD
B $6E15,1,1 Width (bytes)
B $6E16,1,1 Flags
B $6E17,1,1 Height (pixels)
W $6E18,2,2 Bitmap
W $6E1A,2,2 Pre-shifted bitmap
N $6E1C Barrier (32x17)
N $6E1C #HTML[#CALL:graphic($6E1C,32,17,0,1)]
B $6E1C,68,4
N $6E60 Barrier (24x13)
N $6E60 #HTML[#CALL:graphic($6E60,24,13,0,1)]
B $6E60,39,3
N $6E87 Barrier (16x9)
N $6E87 #HTML[#CALL:graphic($6E87,16,9,0,1)]
B $6E87,18,2
N $6E99 Barrier (16x7) pre-shifted
N $6E99 #HTML[#CALL:graphic($6E99,16,7,1,1)]
B $6E99,28,4 Masked bitmap data
N $6EB5 Barrier (16x7) pre-shifted
N $6EB5 #HTML[#CALL:graphic($6EB5,16,7,1,1)]
B $6EB5,28,4 Masked bitmap data
N $6ED1 Ref'd by graphic entry 6
B $6ED1,10,8,2
W $6EDB,2,2 Ptr?
B $6EDD,1,1
N $6EDE Ref'd by graphic entry 15
B $6EDE,10,8,2
W $6EE8,2,2 Ptr?
B $6EEA,1,1
W $6EEB,2,2 Address of another LOD table
B $6EED,20,8*2,4
W $6F01,2,2 Address of another LOD table
B $6F03,20,8*2,4
N $6F17 LOD
@ $6F17 label=streetlamp_lods
B $6F17,1,1 Width (bytes)
B $6F18,1,1 Flags
B $6F19,1,1 Height
W $6F1A,2,2 Bitmap
W $6F1C,2,2 Pre-shifted bitmap
N $6F1E LOD
B $6F1E,1,1 Width (bytes)
B $6F1F,1,1 Flags
B $6F20,1,1 Height
W $6F21,2,2 Bitmap
W $6F23,2,2 Pre-shifted bitmap
N $6F25 LOD
B $6F25,1,1 Width (bytes)
B $6F26,1,1 Flags
B $6F27,1,1 Height
W $6F28,2,2 Bitmap
W $6F2A,2,2 Pre-shifted bitmap
N $6F2C LOD
B $6F2C,1,1 Width (bytes)
B $6F2D,1,1 Flags
B $6F2E,1,1 Height
W $6F2F,2,2 Bitmap
W $6F31,2,2 Pre-shifted bitmap
N $6F33 LOD
B $6F33,1,1 Width (bytes)
B $6F34,1,1 Flags
B $6F35,1,1 Height
W $6F36,2,2 Bitmap
W $6F38,2,2 Pre-shifted bitmap
N $6F3A LOD
@ $6F3A label=streetlamp_flipped_lods
B $6F3A,1,1 Width (bytes)
B $6F3B,1,1 Flags
B $6F3C,1,1 Height
W $6F3D,2,2 Bitmap
W $6F3F,2,2 Pre-shifted bitmap
N $6F41 LOD
B $6F41,1,1 Width (bytes)
B $6F42,1,1 Flags
B $6F43,1,1 Height
W $6F44,2,2 Bitmap
W $6F46,2,2 Pre-shifted bitmap
N $6F48 LOD
B $6F48,1,1 Width (bytes)
B $6F49,1,1 Flags
B $6F4A,1,1 Height
W $6F4B,2,2 Bitmap
W $6F4D,2,2 Pre-shifted bitmap
N $6F4F LOD
B $6F4F,1,1 Width (bytes)
B $6F50,1,1 Flags
B $6F51,1,1 Height
W $6F52,2,2 Bitmap
W $6F54,2,2 Pre-shifted bitmap
N $6F56 LOD
B $6F56,1,1 Width (bytes)
B $6F57,1,1 Flags
B $6F58,1,1 Height
W $6F59,2,2 Bitmap
W $6F5B,2,2 Pre-shifted bitmap
N $6F5D Street lamp top (32x8)
N $6F5D #HTML[#CALL:graphic($6F5D,32,8,0,1)]
B $6F5D,32,4 Masked bitmap data
N $6F7D Street lamp top (24x5)
N $6F7D #HTML[#CALL:graphic($6F7D,24,5,0,1)]
B $6F7D,15,3
N $6F8C Street lamp top (24x4)
N $6F8C #HTML[#CALL:graphic($6F8C,24,4,0,1)]
B $6F8C,12,3
N $6F98 Street lamp top (24x4) pre-shifted
N $6F98 #HTML[#CALL:graphic($6F98,24,4,0,1)]
B $6F98,12,3
N $6FA4 Street lamp top (16x4)
N $6FA4 #HTML[#CALL:graphic($6FA4,16,4,1,1)]
B $6FA4,16,2
N $6FB4 Street lamp top (16x4) pre-shifted
N $6FB4 #HTML[#CALL:graphic($6FB4,16,4,1,1)]
B $6FB4,16,2 Masked bitmap data
N $6FC4 Street lamp top (16x3)
N $6FC4 #HTML[#CALL:graphic($6FC4,16,3,1,1)]
B $6FC4,12,2 Masked bitmap data
N $6FD0 Street lamp top (16x3) pre-shifted
N $6FD0 #HTML[#CALL:graphic($6FD0,16,3,1,1)]
B $6FD0,12,2 Masked bitmap data
N $6FDC Ref'd by graphic entry 7
B $6FDC,10,8,2
N $6FE6 Ref'd by graphic entry 16
B $6FE6,272,8
N $70F6 Ref'd by graphic entry 4 and 13
B $70F6,16,8
N $7106 Ref'd by graphic entry 5 and 14
B $7106,120,8
N $717E LOD
@ $717E label=tree_lods
B $717E,1,1 Width (bytes)
B $717F,1,1 Flags
B $7180,1,1 Height
W $7181,2,2 Bitmap
W $7183,2,2 Pre-shifted bitmap
N $7185 LOD
B $7185,1,1 Width (bytes)
B $7186,1,1 Flags
B $7187,1,1 Height
W $7188,2,2 Bitmap
W $718A,2,2 Pre-shifted bitmap
N $718C LOD
B $718C,1,1 Width (bytes)
B $718D,1,1 Flags
B $718E,1,1 Height
W $718F,2,2 Bitmap
W $7191,2,2 Pre-shifted bitmap
N $7193 LOD
B $7193,1,1 Width (bytes)
B $7194,1,1 Flags
B $7195,1,1 Height
W $7196,2,2 Bitmap
W $7198,2,2 Pre-shifted bitmap
N $719A LOD
B $719A,1,1 Width (bytes)
B $719B,1,1 Flags
B $719C,1,1 Height
W $719D,2,2 Bitmap
W $719F,2,2 Pre-shifted bitmap
N $71A1 LOD
B $71A1,1,1 Width (bytes)
B $71A2,1,1 Flags
B $71A3,1,1 Height
W $71A4,2,2 Bitmap
W $71A6,2,2 Pre-shifted bitmap
N $71A8 LOD
B $71A8,1,1 Width (bytes)
B $71A9,1,1 Flags
B $71AA,1,1 Height
W $71AB,2,2 Bitmap
W $71AD,2,2 Pre-shifted bitmap
N $71AF LOD
B $71AF,1,1 Width (bytes)
B $71B0,1,1 Flags
B $71B1,1,1 Height
W $71B2,2,2 Bitmap
W $71B4,2,2 Pre-shifted bitmap
N $71B6 LOD
B $71B6,1,1 Width (bytes)
B $71B7,1,1 Flags
B $71B8,1,1 Height
W $71B9,2,2 Bitmap
W $71BB,2,2 Pre-shifted bitmap
N $71BD LOD
B $71BD,1,1 Width (bytes)
B $71BE,1,1 Flags
B $71BF,1,1 Height
W $71C0,2,2 Bitmap
W $71C2,2,2 Pre-shifted bitmap
N $71C4 LOD
B $71C4,1,1 Width (bytes)
B $71C5,1,1 Flags
B $71C6,1,1 Height
W $71C7,2,2 Bitmap
W $71C9,2,2 Pre-shifted bitmap
N $71CB LOD
B $71CB,1,1 Width (bytes)
B $71CC,1,1 Flags
B $71CD,1,1 Height
W $71CE,2,2 Bitmap
W $71D0,2,2 Pre-shifted bitmap
N $71D2 LOD
B $71D2,1,1 Width (bytes)
B $71D3,1,1 Flags
B $71D4,1,1 Height
W $71D5,2,2 Bitmap
W $71D7,2,2 Pre-shifted bitmap
N $71D9 LOD
B $71D9,1,1 Width (bytes)
B $71DA,1,1 Flags
B $71DB,1,1 Height
W $71DC,2,2 Bitmap
W $71DE,2,2 Pre-shifted bitmap
N $71E0 LOD
B $71E0,1,1 Width (bytes)
B $71E1,1,1 Flags
B $71E2,1,1 Height
W $71E3,2,2 Bitmap
W $71E5,2,2 Pre-shifted bitmap
N $71E7 LOD
B $71E7,1,1 Width (bytes)
B $71E8,1,1 Flags
B $71E9,1,1 Height
W $71EA,2,2 Bitmap
W $71EC,2,2 Pre-shifted bitmap
N $71EE LOD
B $71EE,1,1 Width (bytes)
B $71EF,1,1 Flags
B $71F0,1,1 Height
W $71F1,2,2 Bitmap
W $71F3,2,2 Pre-shifted bitmap
N $71F5 LOD
B $71F5,1,1 Width (bytes)
B $71F6,1,1 Flags
B $71F7,1,1 Height
W $71F8,2,2 Bitmap
W $71FA,2,2 Pre-shifted bitmap
N $71FC LOD
B $71FC,1,1 Width (bytes)
B $71FD,1,1 Flags
B $71FE,1,1 Height
W $71FF,2,2 Bitmap
W $7201,2,2 Pre-shifted bitmap
N $7203 LOD
B $7203,1,1 Width (bytes)
B $7204,1,1 Flags
B $7205,1,1 Height
W $7206,2,2 Bitmap
W $7208,2,2 Pre-shifted bitmap
N $720A LOD
B $720A,1,1 Width (bytes)
B $720B,1,1 Flags
B $720C,1,1 Height
W $720D,2,2 Bitmap
W $720F,2,2 Pre-shifted bitmap
N $7211 LOD
B $7211,1,1 Width (bytes)
B $7212,1,1 Flags
B $7213,1,1 Height
W $7214,2,2 Bitmap
W $7216,2,2 Pre-shifted bitmap
N $7218 LOD
B $7218,1,1 Width (bytes)
B $7219,1,1 Flags
B $721A,1,1 Height
W $721B,2,2 Bitmap
W $721D,2,2 Pre-shifted bitmap
N $721F LOD
B $721F,1,1 Width (bytes)
B $7220,1,1 Flags
B $7221,1,1 Height
W $7222,2,2 Bitmap
W $7224,2,2 Pre-shifted bitmap
N $7226 LOD
B $7226,1,1 Width (bytes)
B $7227,1,1 Flags
B $7228,1,1 Height
W $7229,2,2 Bitmap
W $722B,2,2 Pre-shifted bitmap
N $722D Tree middle (64x16)
N $722D #HTML[#CALL:graphic($722D,64,16,0,1)]
@ $722D label=bitmap_tree_middle_64x16
B $722D,128,8
N $72AD Tree bottom (64x5)
N $72AD #HTML[#CALL:graphic($72AD,64,5,0,1)]
@ $72AD label=bitmap_tree_bottom_64x5
B $72AD,40,8
N $72D5 Tree trunk (16x8)
N $72D5 #HTML[#CALL:graphic($72D5,16,8,0,1)]
@ $72D5 label=bitmap_tree_trunk_16x8
B $72D5,16,8
N $72E5 Tree shadow (64x5)
N $72E5 #HTML[#CALL:graphic($72E5,64,5,0,1)]
@ $72E5 label=bitmap_tree_shadow_64x5
B $72E5,40,8
N $730D Tree middle (48x12)
N $730D #HTML[#CALL:graphic($730D,48,12,0,1)]
@ $730D label=bitmap_tree_middle_48x12
B $730D,72,8
N $7355 Tree bottom (48x4)
N $7355 #HTML[#CALL:graphic($7355,48,4,0,1)]
@ $7355 label=bitmap_tree_bottom_48x4
B $7355,24,8
N $736D Tree shadow (48x4)
N $736D #HTML[#CALL:graphic($736D,48,4,0,1)]
@ $736D label=bitmap_tree_shadow_48x4
B $736D,24,8
N $7385 Tree middle (32x8)
N $7385 #HTML[#CALL:graphic($7385,32,8,0,1)]
@ $7385 label=bitmap_tree_middle_32x8
B $7385,32,8
N $73A5 Tree bottom (32x3)
N $73A5 #HTML[#CALL:graphic($73A5,32,3,0,1)]
@ $73A5 label=bitmap_tree_bottom_32x3
B $73A5,12,8,4
N $73B1 Tree shadow (16x2)
N $73B1 #HTML[#CALL:graphic($73B1,16,2,0,1)]
@ $73B1 label=bitmap_tree_shadow_16x2
B $73B1,8,8
N $73B9 Tree middle (24x7)
N $73B9 #HTML[#CALL:graphic($73B9,24,7,0,1)]
@ $73B9 label=bitmap_tree_middle_24x7
B $73B9,21,8*2,5
N $73CE Tree bottom (24x2)
N $73CE #HTML[#CALL:graphic($73CE,24,2,0,1)]
@ $73CE label=bitmap_tree_bottom_24x2
B $73CE,6,6
N $73D4 Tree trunk (8x4)
N $73D4 #HTML[#CALL:graphic($73D4,8,4,0,1)]
@ $73D4 label=bitmap_tree_trunk_8x4
B $73D4,4,4
N $73D8 Tree shadow (24x2)
N $73D8 #HTML[#CALL:graphic($73D8,24,2,0,1)]
@ $73D8 label=bitmap_tree_shadow_24x2
B $73D8,6,6
N $73DE Tree top (64x13)
N $73DE #HTML[#CALL:graphic($73DE,64,13,1,1)]
@ $73DE label=bitmap_tree_top_64x13
B $73DE,208,8 Masked bitmap data
N $74AE Tree top (48x10)
N $74AE #HTML[#CALL:graphic($74AE,48,10,1,1)]
@ $74AE label=bitmap_tree_top_48x10
B $74AE,120,12 Masked bitmap data
N $7526 Tree top (32x5)
N $7526 #HTML[#CALL:graphic($7526,32,5,1,1)]
@ $7526 label=bitmap_tree_top_32x5
B $7526,40,8 Masked bitmap data
N $754E Tree top (24x4)
N $754E #HTML[#CALL:graphic($754E,24,4,1,1)]
@ $754E label=bitmap_tree_top_24x4
B $754E,24,6 Masked bitmap data
N $7566 Tree top (24x3)
N $7566 #HTML[#CALL:graphic($7566,24,3,1,1)]
@ $7566 label=bitmap_tree_top_24x3
B $7566,18,6 Masked bitmap data
N $7578 Tree top (24x3) pre-shifted
N $7578 #HTML[#CALL:graphic($7578,24,3,1,1)]
@ $7578 label=bitmap_tree_top_24x3s
B $7578,18,6 Masked bitmap data
N $758A Tree trunk (16x6)
N $758A #HTML[#CALL:graphic($758A,16,6,1,1)]
@ $758A label=bitmap_tree_trunk_16x6
B $758A,24,4 Masked bitmap data
N $75A2 Tree trunk (16x4)
N $75A2 #HTML[#CALL:graphic($75A2,16,4,1,1)]
@ $75A2 label=bitmap_tree_trunk_16x4
B $75A2,16,4 Masked bitmap data
N $75B2 Tree middle (24x5)
N $75B2 #HTML[#CALL:graphic($75B2,24,5,1,1)]
@ $75B2 label=bitmap_tree_middle_24x5
B $75B2,30,6 Masked bitmap data
N $75D0 Tree bottom (24x2)
N $75D0 #HTML[#CALL:graphic($75D0,24,2,1,1)]
@ $75D0 label=bitmap_tree_bottom_24x2_another
B $75D0,12,6 Masked bitmap data
N $75DC Tree trunk (24x3)
N $75DC #HTML[#CALL:graphic($75DC,24,3,1,1)]
@ $75DC label=bitmap_tree_trunk_24x3
B $75DC,18,6 Masked bitmap data
N $75EE Tree shadow (24x1)
N $75EE #HTML[#CALL:graphic($75EE,24,1,1,1)]
@ $75EE label=bitmap_tree_shadow_24x1
B $75EE,6,6 Masked bitmap data
N $75F4 Tree middle (24x5) pre-shifted
N $75F4 #HTML[#CALL:graphic($75F4,24,5,1,1)]
@ $75F4 label=bitmap_tree_middle_24x5s
B $75F4,30,6 Masked bitmap data
N $7612 Tree bottom (24x2) pre-shifted
N $7612 #HTML[#CALL:graphic($7612,24,2,1,1)]
@ $7612 label=bitmap_tree_bottom_24x2s
B $7612,12,6 Masked bitmap data
N $761E Tree trunk (24x3) pre-shifted
N $761E #HTML[#CALL:graphic($761E,24,3,1,1)]
@ $761E label=bitmap_tree_trunk_24x3s
B $761E,18,6 Masked bitmap data
N $7630 Tree shadow (24x1) pre-shifted
N $7630 #HTML[#CALL:graphic($7630,24,1,1,1)]
@ $7630 label=bitmap_tree_shadow_24x1s
B $7630,6,6 Masked bitmap data
u $7636 Unused
B $7636,186,8*23,2
b $76F0 Turbo icons
D $76F0 #HTML[#CALL:anim($76F0,16,14,1,1,3)]
N $76F0 Frame 1 (16x14)
N $76F0 #HTML[#CALL:graphic($76F0,16,14,1,1)]
B $76F0,56,4 Masked bitmap data
N $7728 Frame 2 (16x14)
N $7728 #HTML[#CALL:graphic($7728,16,14,1,1)]
B $7728,56,4 Masked bitmap data
N $7760 Frame 3 (16x14)
N $7760 #HTML[#CALL:graphic($7760,16,14,1,1)]
B $7760,56,4 Masked bitmap data
b $7798 Pre-game screen messages
@ $7798 label=pre_game_messages
B $7798,1,1 .
W $7799,2,2 Back buffer address
W $779B,2,2 Attributes address
T $779D,28,27:n1 "CHASE H.Q. MONITORING SYSTEM" text is white
B $77B9,1,1 flags? attrs?   text is yellow
W $77BA,2,2 Back buffer address
W $77BC,2,2 Attributes address
T $77BE,4,3:n1 "TUNE"
B $77C2,1,1 flags? attrs?   text is yellow
W $77C3,2,2 Back buffer address
W $77C5,2,2 Attributes address
T $77C7,6,5:n1 "VOLUME"
B $77CD,1,1 flags? attrs?   text is white
W $77CE,2,2 Back buffer address
W $77D0,2,2 Attributes address
T $77D2,6,5:n1 "SIGNAL"
b $77D8 Data for pre-game screen.
B $77D8,136,8 Commands to draw the pre-game screen. RLE'd tile references etc.
B $7860,71,8*8,7 Seems to be tiles pointed at by car rendering code
B $78A7,15,8,7 .
B $78B6,344,8 Tiles used to draw the pre-game screen #HTML[#CALL:graphic($78B6,8,43*8,0,0)]
B $7A0E,475,8*59,3
b $7BE9 Graphics: Faces
N $7BE9 Nancy's face (32x40)
N $7BE9 #HTML[#CALL:face($7BE9)]
B $7BE9,160,4 Bitmap data, top-down format
B $7C89,20,4 Attribute data for above
N $7C9D Raymond's face (32x40)
N $7C9D #HTML[#CALL:face($7C9D)]
B $7C9D,160,4 Bitmap data, top-down format
B $7D3D,20,4 Attribute data for above
N $7D51 Tony's face (32x40)
N $7D51 #HTML[#CALL:face($7D51)]
B $7D51,160,4 Bitmap data, top-down format
B $7DF1,20,4 Attribute data for above
b $7E05 Ref'd by graphic entry 3 and 12
B $7E05,507,8*63,3
g $8000 temporaries?
@ $8000 label=test_mode_flag
B $8000,1,1 Test mode enable flag (cheat mode)
B $8001,1,1
@ $8002 label=score_bcd
B $8002,4,4 Score digits as BCD (4 bytes / 8 digits, little endian)
B $8006,1,1 Incremented on reset?
@ $8007 label=wanted_stage_number
B $8007,1,1 Stage number we're loading (1..5 or 6 for end credits)
B $8008,12,8,4
c $8014 Load a stage
D $8014 Used by the routine at #R$8401.
@ $8014 label=load_stage
C $8014,3 Load wanted_stage_number
C $8017,3 Point #REGhl at current_stage_number
C $801A,2 Exit if they match
C $801C,1 current_stage_number = wanted_stage_number
C $801D,3 Set $A220 to wanted level no.
C $8020,5 Update the level number in "SEARCHING FOR <N>"
C $8025,3 Call clear_screen_set_attrs
C $8028,3 Call clear_game_attrs
C $802B,2 Transition type?
C $802D,3 Call setup_transition
C $8032,3 Call TBD subroutine
C $8035,4 IX = &hazards[1]
C $803C,3 Call TBD subroutine
C $803F,2 loop while failed perhaps?
C $8041,3 #REGhl = &hazards[1]
C $8044,1 Load used flag [doesn't add up - this is a level number?]
C $8046,1 equal to <distance related>?
C $8047,2 jump if not
C $8049,2 #REGa += (48 + 128) (make it ASCII and terminate it)
C $804B,3 Set x in "FOUND x"
C $8050,3 #REGa = wanted_stage_number
C $8053,1 equal?
C $8054,2 not the wanted stage
C $8056,3 Call TBD subroutine
C $8059,3 Call tape loading subroutine
N $805E Success - must have loaded the correct level data.
C $805E,1 A = 0
C $805F,2 Set border?
C $8061,3 Call clear_screen_set_attrs
C $8064,3 Call clear_game_attrs
C $8067,3 HL -> "STOP THE TAPE" message structure
C $806C,3 Call sub_8098
C $806F,1 Preserve ?
N $8070 Wait for a keypress - debounce.
C $8070,3 Call keyscan
C $8075,2 Loop while key not pressed
C $8077,3 Call keyscan
C $807C,2 Loop while key pressed
C $8080,3 Call setup_transition
C $8083,1 Restore ?
C $8084,2 B = 2
C $8086,2 Exit via sub_8098
@ $8088 label=sub_8088
C $8088,3 Point #REGhl at "START TAPE" message structure
C $808B,4 A = wanted_stage_number - 1
C $808F,2 Jump if > 0
C $8092,3 Point #REGhl at tape_messsages
@ $8095 label=sub_8095
C $8095,3 why call adjacent instr? to exec this func twice?
@ $8098 label=sub_8098
C $8098,1 preserve counter
C $8099,1 preserve message pointer
@ $809A label=sub_809A
C $809A,2 flags byte for print_message
C $809C,1 why dec here?
N $809D This entry point is used by the routine at #R$F220.
@ $809D label=sub_809D
C $809D,3 Call print_message
C $80A0,2 printing a whole set by looping?
C $80A2,3 Call transition
C $80A5,3 Call draw_screen
C $80A8,1 restore message pointer
C $80A9,1 restore counter
N $80AA This entry point is used by the routine at #R$F220.
C $80AA,3 A = transition_control
C $80AD,2 Return if zero
C $80AF,8 Delay loop
C $80B7,2 Loop
N $80B9 Tape loading
C $80B9,7 Setup to load a block ???
N $80C0 This entry point is used by the routine at #R$5B00.
@ $80C0 label=sub_80c0
C $80C0,1 D++
C $80C1,2 A = 152
C $80C3,1 set carry?
C $80C4,1 bank
C $80C5,1 D--
C $80C6,1 disable interrupts
C $80C7,2 set MIC bit (deactivates MIC)
C $80C9,2 output
C $80CB,2 input
C $80CD,1 why shift?
C $80CE,2 check EAR input bit 6
C $80D0,2 set border?
C $80D3,1 A == A ?
C $80D5,3 Call sub_814b
C $80DA,3 Delay time
C $80DD,2 Delay for B iterations -- is B not initialised?
C $80DF,1 HL--
C $80E0,4 Loop while HL > 0
C $80E4,3 Call sub_8147
C $80E9,2 B = 156
C $80EB,3 Call sub_8147
C $80F0,2 A = 198
C $80F8,2 B = 201
C $80FA,3 Call sub_814b
C $8100,2 212
C $8104,3 Call sub_814b
C $8108,4 A = C ^= 3
C $810E,2 B = 176
C $8113,2 wuuuut?
C $811E,1 A ^= L
C $8120,3 RR C and copy to A?
C $8123,1 DE++
C $812A,2 B = 178
C $812E,3 Call sub_8147
C $8132,2 A = 200
C $8137,2 B = 176
C $813C,2 A = H ^ L
C $813E,1 save A
C $813F,4 Loop while DE > 0
C $8143,1 restore A
C $8144,2 is it 1?
C $8146,1 Return (with flags?)
@ $8147 label=sub_8147
@ $814B label=sub_814b
C $814B,5 Delay loop of 22 iterations
C $8150,1 clear carry flag
C $8151,1 what's in B?
C $8152,1 return if zero
C $8153,2 %01111111
C $8155,2 read from port
C $8159,1 A ^= C
C $815A,2 check EAR input bit 6
C $815C,2 loop if zero
C $815E,3 A = C = ~C
C $8163,2 set MIC bit (deactivate MIC)
C $8165,2 output
C $8167,1 set carry
C $8168,1 Return
@ $8169 label=tape_messsages
B $8169,1,1 attr?
W $816A,2,2 Screen position (8,64)
W $816C,2,2 Screen attribute position (1,8)
T $816E,30,29:n1 "REWIND TAPE TO START OF SIDE 2"
B $818C,1,1 attr?
W $818D,2,2 Screen position (88,96)
W $818F,2,2 Screen attribute position (11,12)
T $8191,10,9:n1 "START TAPE"
B $819B,1,1 attr?
W $819C,2,2 Screen position (72,128)
W $819E,2,2 Screen attribute position (9,16)
T $81A0,15,14:n1 "SEARCHING FOR 1"
B $81AF,1,1 attr?
W $81B0,2,2 Screen position (104,160)
W $81B2,2,2 Screen attribute position (13,20)
T $81B4,7,6:n1 "FOUND 1"
B $81BB,1,1 attr?
W $81BC,2,2 Screen position (80,96)
W $81BE,2,2 Screen attribute position (10,12)
T $81C0,13,12:n1 "STOP THE TAPE"
B $81CD,1,1 attr?
W $81CE,2,2 Screen position (88,128)
W $81D0,2,2 Screen attribute position (11,16)
T $81D2,11,10:n1 "PRESS  GEAR"
b $81DD Start of game chatter
@ $81DD label=start_chatter
B $81DD,1,1 Three-way random choice ($FC)
W $81DE,2,2 -> Tony: "GIDDY UP BOY!" <STOP>
W $81E0,2,2 -> Tony: "HOLD ON MAN" <STOP>
W $81E2,2,2 -> Tony: "LET'S GO. MR. DRIVER." <STOP>
B $81E4,1,1 Tony ($03)
W $81E5,2,2 -> "GIDDY UP BOY!"
B $81E7,1,1 <STOP>
B $81E8,1,1 Tony ($03)
W $81E9,2,2 -> "HOLD ON MAN"
B $81EB,1,1 <STOP>
T $81EC,13,12:n1 "GIDDY UP BOY!"
T $81F9,11,10:n1 "HOLD ON MAN"
c $8204 Generates engine noise
D $8204 Used by the routine at #R$83B5.
@ $8204 label=engine_sfx_setup
C $8204,3 Get speed value
C $8207,2 A = speed / 2 (top half)
N $8209 This entry point is used by the routine at #R$F220.
C $8209,2 A = speed / 2 (bottom half)
C $820B,1 A = ~A
C $820C,4 A /= 4
C $8210,2 A |= 1
C $8212,1 L = A
C $8213,2 H = 3
C $8215,6 If in high gear goto $821D
C $821B,2 L >>= 1
C $821D,6 If $A23B == 0 goto $8225
C $8223,2 H = 1
C $8225,4 Self modify iterations -- L
C $8229,4 Self modify delay loop -- H
C $822D,7 Self modify delay loop -- (5 - A)
N $8234 This entry point is used by the routine at #R$83B5.
@ $8234 label=engine_sfx_play
C $8234,6 Return if perp_caught_stage is >= 3 (when car stops)
C $823A,8 Self modifying counter produces 0,1,2
C $8242,1 Return if nonzero
N $8243 This produces the engine sound effect.
C $8243,2 <Self modified> iterations
C $8245,3 Output 0
C $8248,4 Delay for B cycles (self modified above)
C $824C,4 Output EAR + MIC
C $8250,4 Delay for B cycles (self modified above)
C $8254,3 Loop until iterations is zero
C $8257,1 Return
c $8258 Attract mode -- keyscan / cpu driver / text
D $8258 Used by the routine at #R$83B5.
@ $8258 label=attract_mode
C $8258,3 HL -> attract_data
C $825B,3 Call setup_game
C $825E,4 Reset credits/copyright message blinker to show credits
C $8262,6 Set speed to $190
C $8268,3 Call keyscan
C $826B,3 Return if fire was pressed
C $826E,3 Call cpu_driver
N $8271 This entry point is used by the routine at #R$F220.
C $8271,3 HL = attract mode messages "CHASE HQ" "PRESS GEAR TO PLAY" ...
C $8274,2 Number of messages?
C $8276,6 Causes blinking of "PRESS GEAR TO PLAY"
C $827E,1 nmessages++
C $827F,1 Load flags
C $8280,3 Call print_message
C $8283,2 Loop while B > 0
C $8285,6 Attribute related
N $828B Alternate between credits and copyright messages.
C $828B,7 Blinking on/off self modified pattern
C $8292,3 #REGhl -> credits_messages
C $8295,2 Skip next LD HL based on blink above
C $8297,3 #REGhl -> copyright_messages
C $829A,3 Call message_printing_related
C $829D,3 Call transition
C $82A0,3 Call draw_screen
C $82A3,3 Loop
@ $82A6 label=attract_messages
B $82A6,1,1 Flags (double height)
B $82A7,1,1 Attribute (black)
W $82A8,2,2 Back buffer address
W $82AA,2,2 Attribute address
T $82AC,8,7:n1 "CHASE HQ"
B $82B4,1,1 Flags (regular)
B $82B5,1,1 Attribute (black)
W $82B6,2,2 Back buffer address
W $82B8,2,2 Attribute address
T $82BA,18,17:n1 "PRESS GEAR TO PLAY"  (this blinks)
@ $82CC label=credits_messages
B $82CC,1,1 Frame delay until the following messages start being shown (0 = 256)
B $82CD,1,1 Frame delay until next message?
B $82CE,1,1 Flags (regular)
B $82CF,1,1 Attribute (red)
W $82D0,2,2 Back buffer address
W $82D2,2,2 Attribute address
T $82D4,20,19:n1 "PROGRAM      JOBBEEE"
B $82E8,1,1 Frame delay?
B $82E9,1,1 Flags (regular)
B $82EA,1,1 Attribute (red)
W $82EB,2,2 Back buffer address
W $82ED,2,2 Attribute address
T $82EF,20,19:n1 "GRAPHICS        BILL"
B $8303,1,1 Delay + stop flags?  $28
B $8304,1,1 Flags (regular)
B $8305,1,1 Attribute (red)
W $8306,2,2 Back buffer address
W $8308,2,2 Attribute address
T $830A,20,19:n1 "MUSIC       JON DUNN"
B $831E,2,1
@ $8320 label=copyright_messages
B $8320,2,2 (Frame delays as #R$82CC)
B $8322,1,1 Flags (regular)
B $8323,1,1 Attribute (red)
W $8324,2,2 Back buffer address
W $8326,2,2 Attribute address
T $8328,23,22:n1 "(C) 1989 OCEAN SOFTWARE"
B $833F,1,1 Frame delay?
B $8340,1,1 Flags (regular)
B $8341,1,1 Attribute (red)
W $8342,2,2 Back buffer address
W $8344,2,2 Attribute address
T $8346,26,25:n1 "(C) 1988 TAITO CORPORATION"
B $8360,1,1 Delay + stop flags?  $28
B $8361,1,1 Flags (regular)
B $8362,1,1 Attribute (red)
W $8363,2,2 Back buffer address
W $8365,2,2 Attribute address
T $8367,19,18:n1 "ALL RIGHTS RESERVED"
b $837A Unknown - mostly NUL bytes
B $837A,59,8*7,3
c $83B5 Hooks for 128K functions
D $83B5 Used by the routine at #R$B4CC.
N $83B5 Some of these unknowns will be for the 128K sampled speech.
@ $83B5 label=unknown_hook_1
C $83B5,3 Only a RET
N $83B8 This entry point is used by the routines at #R$8401 and #R$8903.
@ $83B8 label=engine_sfx_play_hook
C $83B8,3 Call engine_sfx_play
N $83BB This entry point is used by the routines at #R$8401, #R$873C, #R$87DC, #R$8876, #R$8A57 and #R$F220.
@ $83BB label=silence_audio_hook
C $83BB,3 Likely silences audio in 128K mode
N $83BE This entry point is used by the routine at #R$8903.
@ $83BE label=unknown_hook_2
C $83BE,3 Only a RET
N $83C1 This entry point is used by the routine at #R$8876.
@ $83C1 label=turbo_sfx_play_hook
C $83C1,3 Likely the turbo boost sound effect
N $83C4 This entry point is used by the routine at #R$8903.
@ $83C4 label=engine_sfx_setup_hook
C $83C4,3 Call engine_sfx_setup
N $83C7 This entry point is used by the routines at #R$8401 and #R$9BCF.
@ $83C7 label=unknown_hook_3
C $83C7,3 Only a RET
N $83CA This entry point is used by the routine at #R$83CD.
@ $83CA label=attract_mode_hook
C $83CA,3 Call attract_mode
c $83CD Bootstrap
D $83CD Used by the routine at #R$E839.
N $83CD Builds a table of flipped bytes at $EF00.
C $83CD,3 Point #REGhl at $EF00
@ $83D0 label=flip_table_loop
C $83D0,2 8 iterations
C $83D2,1 #REGa is the index into the table
@ $83D3 label=flip_byte_loop
C $83D3,3 Shift a bit out of #REGa leftwards and into #REGc rightwards
C $83D6,2 Loop for 8 bits
C $83D8,2 Write #REGc out and advance
C $83DA,2 Loop for 256 iterations (when #REGl overflows)
C $83DC,3 Call attract_mode_hook
C $83DF,4 $A13C = 0
C $83E3,9 Zero $8002..$8006
C $83EC,2 $8007 = 1 => Load stage 1
C $83EE,4 credits = 2
C $83F2,3 Call the main loop
C $83F5,10 If $A139 is non-zero this calls $81AA which is the end of a string...
C $83FF,2 Loop
c $8401 Main loop
D $8401 Used by the routines at #R$83CD and #R$8A57.
@ $8401 label=main_loop
C $8401,3 Call load_stage
C $8404,7 Are we on stage 6 (end credits) jump $841B if not so
N $840B Run the end credits.
@ $840B label=ml_run_end_credits
C $840B,3 Call $5C00 [which is data in earlier stages, must be end credit anim]
C $840E,5 Reset (wanted) stage to 1
C $8413,5 Call load_stage
C $8418,2 Set (wanted) stage to 6   [not sure why]
C $841A,1 Return
N $841B Run the main game loop.
@ $841B label=ml_not_credits
C $841B,3 Call run_pregame_screen
C $841E,3 Address of setup_game_data
C $8421,3 Call setup_game
C $8424,10 $A13B cycles 3,2,1 then repeats
C $842E,7 $A26B = (A * 4) OR 2
C $8435,4 Is $A139 zero? (used later)
C $8439,5 $A188 = $FF  I suspect this keeps the perp spawned
C $8441,3 Call chatter
@ $8444 label=main_loop_loop
C $8444,3 Call drive_sfx
C $8447,3 Call keyscan
C $844A,3 Call tick
C $844D,3 Call check_user_input
C $8450,3 Call read_map
C $8453,3 Call handle_perp_caught
C $8456,3 Call move_hero_car
C $8459,3 Call spawn_cars
C $845C,3 Call cycle_counters
C $845F,3 Call engine_sfx_play_hook
C $8462,3 Call main_loop_10
C $8465,3 Call main_loop_11
C $8468,3 Call engine_sfx_play_hook
C $846B,3 Call main_loop_12
C $846E,3 Call engine_sfx_play_hook
C $8471,3 Call main_loop_13
C $8474,3 Call engine_sfx_play_hook
C $8477,3 Call main_loop_14
C $847A,3 Call tunnel
C $847D,3 Call main_loop_16
C $8480,3 Call helicopter
C $8483,3 Call main_loop_18
C $8486,3 Call engine_sfx_play_hook
C $8489,3 Call main_loop_19
C $848C,3 Call main_loop_20
C $848F,3 Call engine_sfx_play_hook
C $8492,3 Call move_helicopter
C $8495,3 Call check_collisions
C $8498,3 Call engine_sfx_play_hook
C $849B,3 Call main_loop_23
C $849E,3 Call engine_sfx_play_hook
C $84A1,3 Call main_loop_24
C $84A4,3 Call speed_score
C $84A7,3 Call main_loop_25
C $84AA,3 Call overtake_bonus
C $84AD,3 Call engine_sfx_play_hook
C $84B0,3 Call drive_chatter
C $84B3,3 Call smash_bar_etc
C $84B6,3 Call transition
C $84B9,3 Call engine_sfx_play_hook
C $84BC,3 Call draw_screen
C $84BF,3 Call main_loop_27
C $84C2,3 Is test mode enabled?
C $84C5,3 If not, goto no_test_mode
N $84C8 Test mode handling
@ $84C8 label=handle_test_mode
C $84C8,4 Read keys 1/2/3/4/5
C $84CC,1 Change to active high
C $84CD,2 Mask off just keys
C $84CF,2 If none are set then jump
C $84D8,3 Call silence_audio_hook
C $84DC,1 Is bit 0 set? (key 1 to restart the level)
C $84DD,3 Restart level if so
C $84E0,3 HL = &wanted_stage_number
C $84E3,1 Is bit 1 set? (key 2 to load the next level)
C $84E4,2 Jump if NOT
C $84E6,1 wanted_stage_number++
C $84E7,3 Exit via load_stage
C $84EA,1 Is bit 2 set? (key 3 to load the end screen)
C $84EB,2 Jump if NOT
C $84ED,2 wanted_stage_number = 6
C $84EF,3 Exit via load_stage
C $84F2,9 Increment credits unless maxed out at 9
@ $84FB label=no_test_mode
C $850C,3 Call hooked function (likely sfx)
C $850F,3 Loop
C $8524,3 Call setup_transition
C $8527,3 Loop
c $852A CPU driver
D $852A This runs the game loop while driving the car.
@ $852A label=cpu_driver
C $852A,3 Get road position
C $852D,7 Subtract centre value. Carry flag will be set if we're on the right hand side of the road
C $8534,2 Set input ACCELERATE + RIGHT
C $8536,2 Jump cd_check_speed if we're on the left
C $8538,5 Subtract "ideal road position"? value. Carry will be set if we're on the right of it.
C $853D,2 Set input ACCELERATE + LEFT
C $853F,2 Jump cd_check_speed if we're on the right
C $8541,2 Set input ACCELERATE
@ $8543 label=cd_check_speed
C $8543,1 Move input into #REGc for the moment
C $8544,8 Calculate speed - 150
C $854C,3 Load gear
C $854F,2 Set gear to low if speed < 150, otherwise high
C $8553,2 Set input GEAR
@ $8555 label=cd_set_input
C $8555,4 Set user input
C $8559,3 Call read_map
C $855C,3 Call spawn_cars
C $855F,3 Call cycle_counters
C $8562,3 Call main_loop_10
C $8565,3 Call main_loop_11
C $8568,3 Call main_loop_12
C $856B,3 Call main_loop_13
C $856E,3 Call main_loop_14
C $8571,3 Call tunnel
C $8574,3 Call main_loop_16
C $8577,3 Call main_loop_18
C $857A,3 Call main_loop_20
C $857D,3 Call main_loop_19
C $8580,3 Call move_hero_car
C $8583,3 Call check_collisions
C $8586,3 Call main_loop_23
C $8589,3 Exit via main_loop_24
c $858C Pre-game radio screen ("CHASE HQ MONITORING SYSTEM")
D $858C Used by the routine at #R$8401.
@ $858C label=run_pregame_screen
C $858C,3 Address of setup_game_data
C $858F,3 Call setup_game
C $8592,5 Set $A220 to $F8  (TBD)
C $8597,3 Call setup_transition
C $859A,3 Call clear_screen_set_attrs
C $859D,4 Reset car revealing height counter in #R$85E4
C $85A1,1 A = $FF (TBD)
C $85A2,3 Load Nancy's report for this level
C $85A5,3 Call chatter
@ $85A8 label=rps_loop
C $85A8,3 Call draw_pregame
C $85AB,3 Call drive_chatter
C $85AE,3 Call reveal_perp_car
C $85B1,3 Call animate_meters
C $85B4,3 Call transition
C $85B7,3 Call draw_screen
C $85BA,6 Loop if transition_control is non-zero
C $85C0,5 Return if $963D is zero
C $85C5,4 Jump to rps_xxx if $963D >= 3
C $85C9,3 Call keyscan
C $85CC,4 Loop until fire is hit
C $85D0,3 Call clear_chatter
@ $85D6 label=rps_xxx
C $85D8,3 If no carry call setup_transition
C $85DB,2 Loop
b $85DD Data block at 85DD
B $85DD,7,7
c $85E4 Reveals the perp's car on the pre-game screen.
D $85E4 Used by the routine at #R$858C.
@ $85E4 label=reveal_perp_car
C $85E4,6 Return if wanted_stage_number is 5 (the perp's car is hidden on the pre-game screen for that stage)
N $85EA The perp's car is revealed from the bottom-up.
C $85EA,2 Load (self modified) height counter
C $85EC,1 Increment it
C $85ED,5 Max height is 50
C $85F2,3 Update height counter
C $85F5,3 Load address of perp's car LOD
C $85F8,3 Read lod.width_bytes
C $85FB,3 Read lod.height
C $85FE,4 If height counter < lod.height, B = height counter
C $8602,5 Load address of bitmap data
C $8607,1 Stride of bitmap data in bytes (D is still zero)
C $8608,1 Bank for plot_sprite entry
C $8609,3 Address in back buffer to plot at
C $860C,3 Exit via plot_sprite
c $860F Animate the signal meters.
D $860F Used by the routine at #R$858C.
N $860F Update the first meter.
@ $860F label=animate_meters
C $860F,3 Call rng
C $8612,1 Set flags
C $8613,2 Self modified counter
N $8615 Use sign of the random value to enlarge or reduce the apparent meter level.
C $8615,3 Jump to enlarge case if random value is positive
C $8618,2 Decrement counter [Why use SUB here?]
C $861A,2 Jump if >= 0
C $861C,2 Increment counter [Why use ADD here?]
C $861E,4 Jump if < 8
C $8622,1 Decrement counter (max out at 7)
C $8623,3 Self modify counter in $8613
C $8626,3 Screen attribute (23,16)
C $8629,3 Call am_set_attrs
N $862C Update the second meter. Repeats the above.
C $862C,3 Call rng
C $862F,1 Set flags
C $8630,2 Self modified counter
N $8632 Use sign of the random value to enlarge or reduce the apparent meter level.
C $8632,3 Jump to enlarge case if random value is positive
C $8635,2 Decrement counter [Why use SUB here?]
C $8637,2 Jump if >= 0
C $8639,2 Increment counter [Why use ADD here?]
C $863B,4 Jump if < 8
C $863F,1 Decrement counter (max out at 7)
C $8640,3 Self modify counter in $8630
C $8643,3 Screen attribute (23,18)
N $8646 The signal meter bar is seven segments wide. Fill the left hand portion with our chosen amount of green segments and the right hand side with the complement of red segments.
@ $8646 label=am_set_attrs
C $8646,1 Set flags from counter in #REGa
C $8647,2 Jump if zero
C $8649,1 Copy counter to loop counter
C $864A,3 Set attribute byte to black ink over green
C $864D,2 Loop while #REGb
@ $864F label=am_set_right_attrs
C $864F,3 A = 7 - A
C $8652,1 Return if zero
C $8653,1 Copy counter to loop counter
C $8654,3 Set attribute byte to black ink over red
C $8657,2 Loop while #REGb
C $8659,1 Return
c $865A Draw the pre-game screen.
D $865A Used by the routine at #R$858C.
@ $865A label=draw_pregame
C $865A,3 Address of data
@ $865D label=dp_loop
C $865D,1 A = *HL
C $865E,1 Set flags
C $865F,3 Jump to rendering messages only, if zero
C $8662,1 HL++
C $8663,4 Jump if < $D0
C $8667,4 Jump if < $E0  -- $D0..DF
C $866B,4 Jump if >= $F0
N $866F Otherwise handle $E0..$EF. This just seems to set a value.
C $866F,2 A -= $E0
C $8671,3 Self modify $86C8 with A
C $8674,2 Loop
N $8676 Handle $D0..DF.
C $8676,2 A -= $D0
C $8678,6 Self modify $86C2 with A * 8
C $867E,2 Loop
N $8680 Handle $F0..FF.
C $8680,1 D = A
C $8681,2 E = *HL++
C $8683,2 Loop
N $8685 Handle $00..$CF.
C $8687,2 B = 1    single tile?
C $8689,2 Jump if >= $1F
C $868B,1 B = A    run of tiles?
C $868C,2 A = *HL++
N $868E Handle $1F..?
C $868F,2 A -= $1F   A is now a tile index
C $8691,1 multiply by 8 - size of tile
C $8693,1 why the different rotates?
C $8695,3 BC = A
C $8698,2 shift in carry flag?
C $869A,3 Address of base of tiles
C $869D,1 Point at tile we need
N $86A0 label=loop_86a0
N $86A3 Plot a tile.
C $86A3,2 B = 8
C $86A5,3 *HL = *DE++
C $86A8,1 H++
C $86A9,2 Loop
C $86AC,6 D = (B << 2) & $20
C $86B2,5 E = (C & $1F) + D
C $86B7,4 A = (C & $E0) << 1
C $86BB,2 D = $59
C $86BF,1 D++
C $86C0,1 A += E
C $86C1,1 E = A
C $86C2,2 A = xx   Self modified
C $86C4,1 Set flags
C $86C5,2 Jump if zero
C $86C7,1 *DE = A
C $86C8,2 A = xx   Self modified
C $86CA,1 A--
C $86CB,2 Jump if zero
C $86CD,3 A = H & $0F
C $86D0,2 Jump if non-zero
C $86D2,4 H -= $10
C $86D6,4 L += $20
C $86DC,2 HL = BC
C $86DE,1 L++
C $86E1,2 Loop loop_86a0 while B
C $86E5,3 Loop
C $86E8,2 4 messages to print
C $86EA,3 Address of pre_game_messages
C $86ED,2 Flags TBD
C $86EF,1 HL--
C $86F0,3 Call print_message
C $86F3,2 Loop while B
C $86F5,1 Return
b $86F6 18 byte chunks of this gets copied to $C82D by $C808
B $86F6,36,8*4,4
N $871A 14 bytes of setup_game data used by escape_scene. Map ptr are all a byte earlier than their actual start data.
@ $871A label=escape_scene_data
W $871A,2,2 road_pos
W $871C,2,2 road_curvature_ptr
W $871E,2,2 road_height_ptr
W $8720,2,2 road_lanes_ptr
W $8722,2,2 road_rightside_ptr (note: reuse of road curvature data)
W $8724,2,2 road_leftside_ptr (note: reuse of road curvature data)
W $8726,2,2 road_hazard_ptr
N $8728 20 bytes copied to hazards[0]
B $8728,1,1 Used flag
B $8729,8,8
W $8731,2,2
W $8733,2,2 Routine at $ADF9 (it's just a RET)
W $8735,2,2
B $8737,5,5
c $873C Escape scene
D $873C Used by the routine at #R$8401.
@ $873C label=escape_scene
C $873C,3 Call silence_audio_hook
C $873F,3 Address of escape_scene_data
C $8742,3 Call setup_game
C $8745,6 Set speed to $FA [speed of the camera]
C $874B,11 Initialise hazards[0] (the perp)
C $8756,6 Set $A191 to value of lods_table[2] (the perp's car)
C $875C,5 This stops $AD0D from running
C $8761,3 Address of failed_chatter ("wrong job" / "one more try" / "mediocre driver")
C $8764,3 Call chatter
N $8767 Print "GAME OVER" once the transition has completed.
@ $8767 label=es_loop
C $8767,3 Address of game_over_message
C $876A,8 If transition_control != 4 Call message_printing_related
C $8772,3 Call read_map
C $8775,3 Call main_loop_10
C $8778,3 Call main_loop_11
C $877B,3 Call main_loop_12
C $877E,3 Call main_loop_13
C $8781,3 Call main_loop_14
C $8784,3 Call tunnel
C $8787,3 Call main_loop_16
C $878A,3 Call main_loop_19
C $878D,3 Call main_loop_23
C $8790,3 Call main_loop_25
C $8793,3 Call drive_chatter
C $8796,3 Call transition
C $8799,3 Call draw_screen
C $879C,6 Jump to es_loop unless the tunnel has appeared
C $87A2,3 reading from tunnel code [15 when tunnel is small, 6 when fills screen]
C $87A5,4 loop if tunnel code value >= 7
C $87A9,3 $A189 is hazards 2nd byte
C $87AC,4 Jump if it != 5
C $87B0,11 Activate three hazards? [i.e. the three barriers]
C $87BB,6 Set speed to zero [speed of camera]
C $87C1,6 Loop while the perp is still active in the hazards
C $87C7,6 Loop while chatter is still happening?
C $87CD,5 Return if transition_control is zero
C $87D2,2 If transition_control is 4
C $87D6,3 Call setup_transition if non-zero
C $87D9,3 Loop
c $87DC Sets up the game or the level?
D $87DC Used by the routines at #R$8258, #R$8401, #R$858C, #R$873C and #R$F220.
R $87DC I:HL Address of 14 bytes of data to be copied to $A26C+
@ $87DC label=setup_game
C $87DC,1 Preserve data pointer
C $87DD,15 Zero $EE00..$EEFF, and reset road_buffer_offset to $EE00
C $87EC,1 A = 0
C $87ED,11 Copy (47 bytes) at $A13E to $A16D - saved game state
C $87F8,9 Zero 208 bytes at $A19D
C $8801,1 Restore data pointer
C $8802,5 Copy the 14 bytes passed in to $A26C (road_pos, road_curvature_ptr, etc.)
C $8807,3 Pre-shift the backdrop image
C $880A,6 $E34B = 8
C $8810,2 $E34C = 0
C $8812,1 $E34D = 0
C $8813,5 $8F82 = NOP (instruction)
C $8818,2 $8F83 = NOP (instruction)
C $881A,1 $8F84 = NOP (instruction)
C $881B,9 NOP 6 bytes [of instructions] at $8FA4
C $8824,3 Self modify "LD (HL),x" at $C058 to x = 0
C $8827,3 Self modify "LD A,x" at $B063 to x = 0  car jump height
C $882A,6 Set $A191 to value of lods_table[2] (the perp's car)
C $8830,15 Zero $EE00..$EEFF, and reset road_buffer_offset to $EE00 [duplicates work from earlier]
@ $883F label=loop883f
C $883F,2 32 iterations   - affects start position?
C $8841,1 Preserve BC
C $8842,3 Address of fast_counter
C $8845,3 Call HUGE function
C $8848,1 Restore BC
C $8849,2 Loop
C $884B,4 $A254 = 0  -- clear spawning flag
C $884F,2 Transition type?
C $8851,3 Call setup_transition
C $8854,3 Call clear_screen_set_attrs
C $8857,6 Point #REGhl at left light's attributes
C $885D,3 Point #REGhl at right light's attributes then FALL THROUGH
@ $8860 label=clear_lights
C $8860,2 Clear the lights' BRIGHT bit
C $8870,3 Call silence_audio_hook
c $8876 User input checking
D $8876 Used by the routine at #R$8401.
@ $8876 label=check_user_input
C $8876,3 Load transition_control
C $8879,3 Address of user_input
C $887C,4 Ignore the user's input unless transition_control is set to 4
C $8880,2 Clear user_input
C $8882,1 Return
C $8883,5 AND user_input with user input mask
C $8888,3 AND with mask for (Quit+Pause+Turbo), return if none
C $888B,3 Quit bit set? Jump to quit_key if so
C $888E,3 Pause bit set? Jump to pause_key if so
N $8891 Turbo was pressed
C $8891,3 Address of boost timer
C $8894,3 Return if a turbo boost is in effect
C $8897,5 Return if no turbo boosts are left
C $889C,2 Set 60 ticks of boost
C $889E,3 HL -> Random choice of (WHOAAAAA! / GREAT! / ONE MORE TIME.)
C $88A3,3 Call chatter
C $88A6,3 Exit via turbo_sfx_play_hook
N $88A9 This entry point is used by the routine at #R$9BCF.
@ $88A9 label=quit_key
C $88A9,5 If $A26A != 0 then return
C $88AE,3 Call clear_chatter
C $88B1,3 Call fill_attributes
C $88B4,4 user_input_mask = 0  -- A is zero from fill_attributes
C $88B8,3 $A26A = 1
C $88BB,1 Return
@ $88BC label=pause_key
C $88BC,3 Call silence_audio_hook
C $88BF,7 Call keyscan while PAUSE is pressed
C $88C6,7 Call keyscan until keys are pressed
C $88CD,7 Call keyscan until keys are released (debounce)
C $88D4,1 Return
c $88D5 Clears the game screen attributes to zero
D $88D5 Used by the routines at #R$8014 and #R$88E2.
@ $88D5 label=clear_game_attrs
c $88E2 Clears the game screen attributes and the game screen to zero
D $88E2 Used by the routine at #R$BDC1.
@ $88E2 label=clear_game_screen
c $88F2 Starts a sound effect
D $88F2 Used by the routines at #R$8401, #R$8903, #R$9BCF, #R$A399, #R$A637, #R$A8CD, #R$AC3C, #R$B063 and #R$B318.
R $88F2 I:BC e.g. $0704 $0801 $0604
@ $88F2 label=start_sfx
C $88F2,6 If $A238 is zero then goto sfx_related_assign
C $88F8,2 Return if $A238 < C
@ $88FA label=sfx_related_assign
C $88FA,8 wordat($A237) = BC
C $8902,1 Return
c $8903 Drives sound effects
D $8903 Used by the routine at #R$8401.
@ $8903 label=drive_sfx
C $8903,6 If $A23B != 0 goto $8916
C $8909,7 *$A23D |= *$A23C
C $8910,3 Counters?
C $8913,3 Call start_sfx if non-zero
C $8916,3 Call engine_sfx_setup_hook
C $8919,3 Call engine_sfx_play_hook
C $891C,3 Call another hooked routine
C $891F,5 If $A237 == 0 return
C $8924,3 #REGe = $A237 * 4 -- stride of table
C $8927,4 $A237 = 0
C $892B,3 $A238 = 0
C $892E,1 Make #REGde full offset
C $892F,3 #REGhl -> Base of table, but 4 bytes earlier
C $8932,1 Point at array element
C $8933,4 Load argument value into #REGde
C $8937,4 Load address of routine into #REGhl
C $893B,1 Exit via routine at #REGhl
N $893C Pairs of words: (argument, address of sound effect routine)
W $893C,2,2 Argument in #REGde for sound effect routine
W $893E,2,2 Sound effect routine $8A0F - "pitt" and "pfff" and gear noise too?
W $8940,2,2 Argument in #REGde for sound effect routine: D = delay of 8
W $8942,2,2 Sound effect routine $89D9
W $8944,2,2 Argument in #REGde for sound effect routine
W $8946,2,2 Sound effect routine $8960 - crash
W $8948,2,2 Argument in #REGde for sound effect routine
W $894A,2,2 Sound effect routine $8960 - crash
W $894C,2,2 Argument in #REGde for sound effect routine
W $894E,2,2 Sound effect routine $89D9
W $8950,2,2 Argument in #REGde for sound effect routine
W $8952,2,2 Sound effect routine $8A17
W $8954,2,2 Argument in #REGde for sound effect routine - "tit-tit" used when cornering
W $8956,2,2 Sound effect routine $8A17
W $8958,2,2 Argument in #REGde for sound effect routine - time running out high "bip"
W $895A,2,2 Sound effect routine $8A36 - "bip-bow"
W $895C,2,2 Argument in #REGde for sound effect routine - time running out low "bow"
W $895E,2,2 Sound effect routine $8A36 - "bip-bow"
c $8960 Sound effect - crash
R $8960 I:D Inner loop count
@ $8960 label=sfx8960
C $8960,3 #REGhl -> Effect data table
C $8963,2 Length of table is 93
@ $8965 label=sfx8960_loop_outer
C $8965,1 Set inner loop counter
@ $8966 label=sfx8960_loop_inner
C $8966,2 #REGa = 16
C $8968,2 Top bit set of this byte?
C $896A,2 Don't clear the EAR bit
C $896C,2 Otherwise clear the EAR bit
C $896E,2 Output
C $8970,2 Rotate the effect data
C $8972,2 Delay
C $8974,2 Loop while #REGb
C $8976,1 Move to the next byte of effect data
C $8977,4 Loop while #REGc
C $897B,1 Return
B $897C,93,8*11,5 Effect data table
c $89D9 Sound effect - not sure
R $89D9 I:D Delay value
@ $89D9 label=sfx89d9
C $89D9,2 Length of table is 32
C $89DB,3 #REGhl -> Effect data table
C $89DE,1 Zero output byte
@ $89DF label=sfx89d9_loop
C $89DF,1 B = *HL
C $89E0,2 Output to
C $89E2,4 Delay for 'D' iterations
C $89E6,2 Loop for 'B' iterations
C $89E8,2 Wiggle the EAR bit
C $89EA,1 Move to the next byte of effect data
C $89EB,3 Loop while #REGc
C $89EE,1 Return
@ $89EF label=sfx_data
B $89EF,32,8 Effect data table
c $8A0F Sound effect - "pitt" and "pfff"
R $8A0F I:D Affects delay loops
R $8A0F I:E Inner loop count
@ $8A0F label=sfx8a0f
C $8A0F,2 Half duty cycle
C $8A11,6 Return if 1
@ $8A17 label=sfx8a0f_loop_outer
C $8A17,1 Set inner loop counter
@ $8A18 label=sfx8a0f_loop_inner
C $8A18,3 Call rng
C $8A1B,2 #REGa &= 16
C $8A1D,2 If zero goto sfx8a0f_continue
C $8A1F,6 Delay loop of (24 - #REGd) iterations
C $8A25,2 #REGa = 24 set EAR and MIC bits <not sure of the effect>
C $8A27,2 Output
C $8A29,3 Delay loop of #REGd iterations
C $8A2C,3 Output
@ $8A2F label=sfx8a0f_continue
C $8A2F,3 Loop while #REGc
C $8A32,3 Loop while #REGd
C $8A35,1 Return
c $8A36 Sound effect - "bip-bow"
R $8A36 I:D Delay at start
R $8A36 I:E Same as D
@ $8A36 label=sfx8a36
C $8A36,2 C = 20
C $8A38,3 Inner loop counter. H is decremented and restored from L each loop.
@ $8A3B label=sfx8a36_loop
C $8A3B,3 Delay loop of D iterations
C $8A3E,1 D = E
C $8A3F,6 Delay loop of (24 - C) iterations
C $8A45,2 A = 24 set EAR and MIC bits <not sure of the effect>
C $8A47,2 Output A
C $8A49,3 Delay loop of C iterations
C $8A4C,3 Output 0
C $8A4F,3 Loop while H
C $8A52,1 H = L
C $8A53,3 Loop while C
N $8A56 This entry point is used by the routine at #R$83B5.
C $8A56,1 Return
c $8A57 Handle perp caught.
D $8A57 This handles slowing cars down to a stop and calculating and displaying the bonus score when the perp has been caught.
@ $8A57 label=handle_perp_caught
C $8A57,5 Return if perp_caught_stage is zero
C $8A5C,4 Jump to hpc_stage1 if it's one
C $8A60,3 Jump to hpc_stage2 if it's two
C $8A63,3 Jump to hpc_stage3 if it's three
C $8A66,3 Jump to hpc_stage4 if it's four
C $8A69,1 Bank
C $8A6A,5 Return if transition_control != 0
C $8A6F,1 Unbank
C $8A70,3 Jump to hpc_stage5 if perp_caught_stage is five
N $8A73 Otherwise all of the caught stages are complete and we can move on to the next stage.
C $8A73,1 [Where's the PUSH?]
C $8A74,3 Call silence_audio_hook
C $8A77,4 Increment wanted_stage_number
C $8A7B,3 Jump to main_loop
@ $8A7E label=hpc_stage5
C $8A7E,5 perp_caught_stage = 6
C $8A83,2 A = 8 -- transition type
C $8A85,3 Jump to setup_transition
@ $8A88 label=hpc_stage2
C $8A88,3 A = *$A22A
C $8A8B,4 Jump to $8AB2 if A >= 16
C $8A8F,5 $A22A = A + 4
C $8A94,7 HL = road_pos + 12
C $8A9C,5 HL -= $126
C $8AA2,2 Jump to $8AA5 if HL < $126  -- Suspect this is driving the car to stop the perp
C $8AA5,3 road_pos = HL
C $8AA8,5 A = fast_counter + 32
C $8AAD,1 Return if carry
C $8AAE,3 fast_counter = A
C $8AB1,1 Return
C $8AB2,5 perp_caught_stage = 3
C $8AB7,4 $8ABF = 4
C $8ABB,3 Call fill_attributes
@ $8ABE label=hpc_stage3
C $8ABE,2 Self modified by $8AB8
C $8AC0,1 A--
C $8AC1,3 Self modify 'LD A' at $8ABE to load A
C $8AC4,1 Return if A != 0
C $8AC5,5 perp_caught_stage = 4
C $8ACA,3 HL = *$5D06   -- arrest messages
C $8ACD,2 A = 1         -- transition_control byte
C $8ACF,3 Jump to j_8e80   -- inside message_printing_related
@ $8AD2 label=hpc_stage4
C $8AD2,7 If *$A139 != 0 call $8193  -- strange code calling into a string
C $8AD9,5 Return if transition_control != 0
C $8ADE,7 If *$A139 != 0 call $8193  -- strange code again
C $8AE5,5 perp_caught_stage = 5
N $8AEA Calculate clear bonus.
C $8AEA,2 '0'
C $8AEC,4 D = wanted_stage_number
C $8AF0,2 L = D + '0'
C $8AF2,3 A = *$8006
C $8AF5,3 Jump to hpc_8b03 if zero
C $8AF8,8 D <<= 4? or make BCD?
C $8B00,1 H = L
C $8B01,2 L = ' '
@ $8B03 label=hpc_8b03
C $8B03,3 Set first two digits of the clear bonus to #REGhl, "LH0,000"
C $8B06,10 Set score
C $8B10,3 Call increment_score
C $8B13,3 Get time remaining (BCD)
C $8B16,3 Set A in "TIME BONUS   A  X 5000"
C $8B19,1 C = A
C $8B1A,4 A <<= 4?
C $8B1E,2 A &= $F
C $8B22,2 A = ' '
C $8B27,1 B = A -- iterations
C $8B28,2 A += '0'
C $8B2B,3 DE = $0500
@ $8B2E label=hpc_increment_score_loop_1
C $8B2E,1 A = 0
C $8B2F,3 Call increment_score
C $8B32,2 Loop while B
C $8B35,3 Set A in "TIME BONUS   A  X 5000"
C $8B38,3 A = C & 15
C $8B3E,1 B = A -- iterations
C $8B3F,3 DE = $0050
@ $8B42 label=hpc_increment_score_loop_2
C $8B42,1 A = 0
C $8B43,3 Call increment_score
C $8B46,2 Loop while B
C $8B49,2 A += '0'
C $8B4B,3 Set A in "TIME BONUS   xA X 5000"
C $8B51,3 Point DE at last digit of score_bcd (big end)
C $8B54,3 Point HL at x in "SCORE         x       " - the ten millions?
@ $8B57 label=hpc_loop_3
C $8B57,1 A = *DE    -- fetch digits
C $8B58,6 A = (A << 4) & 15   -- extract first digit
C $8B64,2 A = ' '
C $8B68,2 C = $FF
C $8B6A,2 A += '0'
C $8B6C,2 *HL++ = A
C $8B6E,3 A = *DE & 15
C $8B77,2 A = ' '
C $8B7B,2 C = $FF
C $8B7D,2 A += '0'
C $8B7F,1 *HL = A
C $8B80,1 HL++
C $8B81,1 DE--
C $8B82,2 Loop hpc_loop_3 while B
C $8B84,1 HL--
C $8B85,2 *HL |= 1<<7
C $8B87,3 Point at "CLEAR BONUS - TIME BONUS - SCORE" messages
C $8B8A,3 Call message_printing_related
@ $8B8D label=hpc_stage1
C $8B8D,3 A = *$A18D  -- hazards[0].something  perp car's current hz pos?  byte only? i thought it was  a word
C $8B90,2 C = 45  -- likely where the perp car should stop (horizontally)
C $8B92,1 Compare
C $8B93,2 B = 5
C $8B95,2 -- no change required
C $8B97,2 -- change by 5
C $8B99,2 B = -5  -- change by -5
C $8B9B,2 C = A + B
C $8B9D,4 *$A18D = C  -- hazards[0].something  perp car hz pos
C $8BA1,3 HL = road_pos
C $8BA4,1 Stack it
C $8BA5,3 DE = $0105  -- centre road position
C $8BA8,2 HL -= DE
C $8BAA,1 Unstack read_pos
C $8BAB,2 A = 9
C $8BAD,2 Jump to $8BBA if HL >= DE
C $8BB2,2 HL -= DE
C $8BB4,2 A = 10
C $8BB8,2 A = 8
C $8BBA,1 C = A
C $8BBB,2 A &= 3
C $8BBF,3 A = *$A189  -- hazards[0].<distance related>
C $8BCC,7 speed = 0
C $8BD3,3 *$A18C = 0  -- hazards[0].something
C $8BD6,4 *$A189 = 1  -- hazards[0].<distance related>
C $8BDA,4 perp_caught_stage = 2
C $8BDE,4 smoke = 3
C $8BE2,3 Jump to hpc_set_perp_pos
C $8BE5,3 A = *$A189  -- hazards[0].<distance related>
C $8BEF,3 A = ~A + $F
C $8BF2,3 DE = 20
C $8BF5,1 B = A  -- iterations
C $8BF6,2 HL -= DE
C $8BF8,2 Loop while B
C $8BFB,3 HL = speed
C $8BFF,4 Jump if HL < DE
C $8C03,2 Clear Up, i.e. stop accelerating
C $8C05,7 Jump if HL < 50
C $8C0C,2 Set Down bit, i.e. brake
C $8C0F,3 DE = 150
C $8C12,2 HL -= DE
C $8C14,3 A = gear
C $8C17,2 -- why this op?
C $8C1B,2 Set Fire
C $8C1D,4 user_input = C
C $8C21,3 A195 is horz position of perp car
C $8C24,11 Jump to hpc_set_perp_pos if HL <= $46
C $8C2F,5 HL -= 5
N $8C35 This entry point is used by the routine at #R$8C3A.
@ $8C35 label=hpc_set_perp_pos
C $8C35,4 $A195 = #REGde (position)
C $8C39,1 Return
c $8C3A Fully smashed
D $8C3A Used by the routine at #R$B4F0.
@ $8C3A label=fully_smashed
C $8C3A,5 Set perp_caught_stage to 1 - starts the pull over sequence
C $8C3F,4 $A22F = 2
C $8C43,5 Set smash_counter to 20
C $8C48,5 Set user input mask to (Quit+Pause)
C $8C4D,3 Print the "OK! PULL OVER CREEP!" message
C $8C50,3 Call message_printing_related
C $8C53,3 #REGde = $190
C $8C56,2 Exit via hpc_set_perp_pos
b $8C58 End of level messages
@ $8C58 label=score_messages
B $8C58,3,3
B $8C5B,1,1 Attribute (red)
W $8C5C,2,2 Back buffer address
W $8C5E,2,2 Attribute address
T $8C60,22,21:n1 "CLEAR BONUS      0,000" -- double height
B $8C76,2,2
B $8C78,1,1 Attribute (red)
W $8C79,2,2 Back buffer address
W $8C7B,2,2 Attribute address
T $8C7D,22,21:n1 "TIME BONUS      X 5000" -- double height
B $8C93,2,2
B $8C95,1,1 Attribute (red)
W $8C96,2,2 Back buffer address
W $8C98,2,2 Attribute address
T $8C9A,22,21:n1 "SCORE                 " -- double height
@ $8CB2 label=sighting_message
B $8CB0,5,2,3
B $8CB5,1,1 Attribute (black)
W $8CB6,2,2 Back buffer address
W $8CB8,2,2 Attribute address
T $8CBA,26,25:n1 "SIGHTING OF TARGET VEHICLE"
@ $8CD6 label=pull_over_message
B $8CD4,5,2,3
B $8CD9,1,1 Attribute (black)
W $8CDA,2,2 Back buffer address
W $8CDC,2,2 Attribute address
T $8CDE,20,19:n1 "OK! PULL OVER CREEP!"
@ $8CF4 label=game_over_message
B $8CF2,5,2,3
B $8CF7,1,1 Attribute (black)
W $8CF8,2,2 Back buffer address
W $8CFA,2,2 Attribute address
T $8CFC,9,8:n1 "GAME OVER"
@ $8D07 label=time_up_message
B $8D05,5,2,3
B $8D0A,1,1 Attribute (black)
W $8D0B,2,2 Back buffer address
W $8D0D,2,2 Attribute address
T $8D0F,7,6:n1 "TIME UP"
@ $8D18 label=continue_messages
B $8D16,5,2,3
B $8D1B,1,1 Attribute (black)
W $8D1C,2,2 Back buffer address
W $8D1E,2,2 Attribute address
T $8D20,9,8:n1 "CONTINUE "
B $8D29,2,2
B $8D2B,1,1 Attribute (black)
W $8D2C,2,2 Back buffer address
W $8D2E,2,2 Attribute address
T $8D30,12,11:n1 "THIS MISSION"
B $8D3C,2,2
B $8D3E,1,1 Attribute (red)
W $8D3F,2,2 Back buffer address
W $8D41,2,2 Attribute address
T $8D43,16,15:n1 "PUSH GEAR BUTTON"
B $8D53,2,2
B $8D55,1,1 Attribute (red)
W $8D56,2,2 Back buffer address
W $8D58,2,2 Attribute address
T $8D5A,22,21:n1 "BEFORE TIMER REACHES 0"
B $8D70,2,2
B $8D72,1,1 Attribute (black)
W $8D73,2,2 Back buffer address
W $8D75,2,2 Attribute address
T $8D77,7,6:n1 "TIME 10"
B $8D7E,2,2
B $8D80,1,1 Attribute (black)
W $8D81,2,2 Back buffer address
W $8D83,2,2 Attribute address
T $8D85,8,7:n1 "CREDIT  "
B $8D8D,2,2
c $8D8F Drives transitions.
D $8D8F Used by the routines at #R$8014, #R$8258, #R$8401, #R$858C, #R$873C and #R$F220.
@ $8D8F label=transition
C $8D8F,3 Check the flag/counter that fill_attributes resets
C $8D92,2 Exit early if its zero
C $8D94,1 Decrement it
C $8D95,8 If zero, call draw_mugshots
C $8D9D,3 Call fill_attributes
C $8DA0,16 (self modified)
C $8DB0,4 Move to next frame? (self modified)
C $8DB4,9 Set anim address? (self modify)
C $8DBD,2 8 chunks
@ $8DBF label=transition_loop
C $8DBF,1 Fetch a byte of anim?
C $8DC0,1 Bank
C $8DC1,1 Get it into #REGe
C $8DC2,1 Preserve H in D (which is safe)
C $8DC3,2 Reset ptr
C $8DC5,3 Draw a chunk
C $8DC8,2 Reset ptr
C $8DCA,1 Move up (screen-wise) by 8
C $8DCE,3 Draw a chunk
C $8DD1,2 H = D - 1 -- drop down 1 row
C $8DD3,1 Unbank
C $8DD4,1 Next byte of anim
C $8DD5,2 Loop
C $8DD7,1 Return
@ $8DD8 label=fade_chunk
C $8DD8,2 8 rows
@ $8DDA label=fade_row_loop
C $8DDA,2 6 iterations (of 5 each) = 30 bytes written (~ a scanline)
@ $8DDC label=fade_column_loop
C $8DDC,3 OR #REGe into the screen pixels
C $8DDF,17 Repeat another four times
C $8DF0,2 Loop
C $8DF2,2 Move to next start address
C $8DF4,4 Loop
C $8DF8,1 Return
c $8DF9 Seems to setup a transition.
D $8DF9 Used by the routines at #R$8014, #R$8401, #R$858C, #R$873C, #R$87DC, #R$8A57 and #R$F220.
R $8DF9 I:A ?
@ $8DF9 label=setup_transition
C $8DF9,1 Set flags from #REGa
C $8DFA,3 #REGbc = #REGa
C $8DFD,3 Points to some data TBD
C $8E00,3 Jump over if #REGa was +ve
C $8E03,1 #REGb = $FF
C $8E04,3 Points to some data TBD
@ $8E07 label=dunno_jump
C $8E07,4 Self modify
C $8E0B,3 Call random
C $8E0E,2 Mask 0..3
C $8E10,7 HL = A * 3 + DE
C $8E18,3 Self modify
C $8E1B,4 Load DE from HL
C $8E1F,4 Set transition animation start address
C $8E23,2 4 frames of animation? 4 states?
C $8E28,1 Return
c $8E29 Fills the attribute bytes leftwards from column 1.
D $8E29 Used by the routines at #R$8876, #R$8A57 and #R$8D8F.
@ $8E29 label=fill_attributes
C $8E29,3 Screen attribute position (1,8)
C $8E2C,2 Clear #REGb for #REGc to be set in a moment
C $8E2E,2 16 rows of attributes
C $8E30,2 Destination address
C $8E32,2 28 bytes to copy (why isn't this 29?)
C $8E34,1 Dest++
C $8E35,2 Fill by 'rolling'
C $8E37,4 Skip four (32 wide - 28)
C $8E3B,1 Row counter--
C $8E3C,2 Loop until it hits zero
C $8E3E,3 Zero this flag
C $8E41,1 Return
c $8E42 Subroutine.
D $8E42 Used by the routines at #R$8D8F and #R$8E91.
@ $8E42 label=sub_8e42
C $8E42,3 Address of current message thing (frame delay, flags, attrs, bufaddr, attraddr, string)
C $8E45,2 B = 0 -- self modified by ($8E58 increments it), ($8E8D resets it to 1)
@ $8E47 label=sub_8e47_loop
C $8E47,2 sub_8e42_1
C $8E49,2 A = 20 -- self modified by $8E4C, $8E51, ($8E84 resets it perhaps)
C $8E4B,1 A--
C $8E4C,3 self modify $8E49
C $8E4F,1 Return if non-zero
C $8E50,1 A = *HL
C $8E51,3 self modify $8E49
C $8E54,7 increment $8E46, affects TIME UP state if disabled
C $8E5B,1 B++
@ $8E5C label=sub_8e42_1
C $8E5C,1 HL++
C $8E5D,1 A = *HL
C $8E5E,1 test flags
C $8E5F,2 jump if zero
C $8E61,3 Call print_message
C $8E64,2 Loop?
C $8E66,5 transition_control = HL[-1]
C $8E6B,1 Return
c $8E6C Print a message
D $8E6C Used by the routines at #R$8014, #R$8258, #R$865A, #R$8E42 and #R$F220.
R $8E6C I:A Flags byte
R $8E6C I:HL -> as yet unnamed message structure
@ $8E6C label=print_message
C $8E6C,1 Preserve/bank flags byte in #REGa
C $8E6D,1 Preserve #REGbc
C $8E6E,1 Skip flags byte
C $8E6F,2 Load attribute byte into #REGa
C $8E71,4 Load back buffer address into #REGde
C $8E75,4 Load attribute address into #REGbc
N $8E79 #REGhl now points at the string.
C $8E79,3 Call alt draw_string entry point
C $8E7C,1 Restore #REGbc
C $8E7D,1 Return
c $8E7E Message printing related, called for TIME UP, CONTINUE, etc. messages
D $8E7E Used by the routines at #R$8258, #R$873C, #R$8A57, #R$8C3A, #R$9BCF, #R$B4CC and #R$F220.
R $8E7E I:HL -> something  e.g. -> credits_messages
@ $8E7E label=message_printing_related
C $8E7E,2 A = 2
N $8E80 This entry point is used by the routine at #R$8A57.
@ $8E80 label=j_8e80
C $8E80,3 transition_control = A
C $8E83,4 Modify 'LD A' at $8E49 with frame delay
C $8E87,4 Modify 'LD HL' at $8E42 to be message struct e.g. $82CD
C $8E8B,5 Modify 'LD B' at $8E45 to be ?
C $8E90,1 Return
c $8E91 Copies the mugshots onto the screen when perp is caught
D $8E91 Used by the routine at #R$8D8F.
@ $8E91 label=draw_mugshots
C $8E91,3 -> Perp's mugshot attributes
C $8E94,3 Final byte of bitmap (back buffer)
C $8E97,3 Screen position (40,104)
C $8E9A,3 Call draw_mugshot
C $8E9D,3 -> Tony mugshot attributes
C $8EA0,3 Final byte of bitmap (back buffer)
C $8EA3,3 Screen position (160,104)
C $8EA6,3 Call draw_mugshot
C $8EA9,3 -> Raymond mugshot attributes
C $8EAC,3 Final byte of bitmap (back buffer)
C $8EAF,3 Screen position (200,104)
C $8EB2,3 Call draw_mugshot
C $8EB5,2 Exit via sub_8e42
c $8EB7 Draws a mugshot
D $8EB7 Used by the routine at #R$8E91.
R $8EB7 I:BC Screen position (used for attributes)
R $8EB7 I:DE Address of last byte of bitmap data to be written (in back buffer)
R $8EB7 I:HL Address of mugshot attributes (screen)
@ $8EB7 label=draw_mugshot
C $8EB7,1 Preserve screen position
C $8EB8,1 Preserve address of mugshot attributes
C $8EB9,1 Move back a byte to point at end of bitmap data (we assume that the bitmap data precedes the attribute bytes)
C $8EBA,3 Size of mugshot data (32x40 bitmap)
@ $8EBD label=dm_loop
C $8EBD,1 Save #REGe
C $8EBE,8 Transfer four bytes (#REGde, #REGhl and #REGbc are decremented by 4)
C $8EC6,1 Restore #REGe
C $8EC7,3 If #REGbc becomes zero then proceed to dm_plot_attrs
N $8ECA Move to next scanline
C $8ECA,1 Save for checking in a moment
C $8ECB,1 Move to next scanline (visually upwards)
C $8ECC,2 Would it have rolled over into the top nibble?
C $8ECE,3 No - continue
N $8ED1 It rolled over
C $8ED1,4 Put back the bit stolen by rollover
C $8ED5,4 Move to next chunk of 16 scanlines
C $8ED9,3 Continue if it didn't roll over
C $8EDC,4 Otherwise move to the next chunk of 128 scanlines (would put us outside the back buffer)
C $8EE0,3 Loop
@ $8EE3 label=dm_plot_attrs
C $8EE3,1 Restore address of mugshot attributes
C $8EE4,3 Exit via pf_attrs_bit
c $8EE7 Smash bar etc.
D $8EE7 Used by the routine at #R$8401.
@ $8EE7 label=smash_bar_etc
C $8EE7,5 Return if the perp has not yet been sighted
C $8EEC,6 Return if perp_caught_stage is >= 3 (car has stopped)
C $8EF2,3 Back buffer address of bottom of bar
C $8EF5,3 #REGd = 15 = mask, #REGe = 16 = row stride
N $8EF8 Draws bottom two rows
C $8EF8,2 2 rows
C $8EFA,3 Call draw_smash_bar_solid_bit
C $8EFD,3 Get smash_counter
C $8F00,3 Jump straight to drawing solid if it's zero (whole bar solid)
C $8F03,1 Set iterations in #REGc for call
C $8F04,1 Preserve smash_counter
C $8F05,3 Call draw_smash_bar_segment
C $8F08,1 Restore smash_counter
@ $8F09 label=draw_smash_bar_solid_top
C $8F09,1 Iterations = smash_counter
C $8F0A,6 B = ~(A * 3) + 63 = amount of solid rows to draw
C $8F10,3 Call draw_smash_bar_solid_bit
@ $8F13 label=draw_smash_bar_segment
C $8F13,2 Set 8 pixels to "X......X"
C $8F15,1 Save for checking in a moment
C $8F16,1 Move to next scanline
C $8F17,1 Would it have rolled over into the top nibble?
C $8F18,3 No - continue
N $8F1B It rolled over
C $8F1B,3 Put back the bit stolen by rollover
C $8F1E,4 Move to next chunk of 16 scanlines
C $8F22,3 Continue if it didn't roll over
C $8F25,3 Otherwise move to the next chunk of 128 scanlines (would put us outside the back buffer)
@ $8F28 label=draw_smash_bar_segment2
C $8F28,2 Set 8 pixels to "X......X"
C $8F2A,1 Save for checking in a moment
C $8F2B,1 Move to next scanline
C $8F2C,1 Would it have rolled over into the top nibble?
N $8F2D It rolled over
C $8F2D,3 No - continue
C $8F30,3 Put back the bit stolen by rollover
C $8F33,4 Move to next chunk of 16 scanlines
C $8F37,3 Continue if it didn't roll over
C $8F3A,3 Otherwise move to the next chunk of 128 scanlines (would put us outside the back buffer)
@ $8F3D label=draw_smash_bar_segment_cont
C $8F3D,2 1 row
C $8F3F,3 Call draw_smash_bar_solid_bit
C $8F42,4 Loop while iterations remain
C $8F46,1 Return
@ $8F47 label=draw_smash_bar_solid_bit
C $8F47,2 Set 8 pixels to solid black
N $8F49 Move to next scanline
C $8F49,1 Save for checking in a moment
C $8F4A,1 Move to next scanline (visually upwards)
C $8F4B,1 Would it have rolled over into the top nibble?
C $8F4C,3 No - continue
N $8F4F It rolled over
C $8F4F,3 Put back the bit stolen by rollover
C $8F52,4 Move to next chunk of 16 scanlines
C $8F56,3 Continue if it didn't roll over
C $8F59,3 Otherwise move to the next chunk of 128 scanlines (would put us outside the back buffer)
@ $8F5C label=draw_smash_bar_cont
C $8F5C,2 Loop while iterations remain
C $8F5E,1 Return
c $8F5F Routine at 8F5F
D $8F5F Used by the routines at #R$8401, #R$852A and #R$873C.
@ $8F5F label=main_loop_23
C $8F5F,3 Point #REGde at the stack
C $8F62,3 Self modify 'LD HL' at $A9E2 to load the stack address
C $8F65,6 Self modify 'LD HL' at $AECF to load $E900
C $8F6B,3 HL = $E301
C $8F6E,3 DE = $E336
C $8F71,3 BC = $1520
C $8F74,3 *HL += C
C $8F77,3 *DE += C
C $8F7A,1 E++
C $8F7B,1 L++
C $8F7C,2 Loop to $8F74 while B
C $8F7E,4 IY = $E315
C $8F82,1 Self modified
C $8F83,1 Self modified
C $8F84,1 Self modified
C $8F85,2 IY--
C $8F87,5 A = road_buffer_offset + $73
C $8F8C,3 HL = $EE00 | A
C $8F8F,4 IX = $EAB0
C $8F93,3 BC = $1420
C $8F96,4 Preserve IX, HL, BC
C $8F9D,1 Set flags
C $8FA4,1 Self modified
C $8FA5,1 Self modified
C $8FA6,1 Self modified
C $8FA7,1 Self modified
C $8FA8,1 Self modified
C $8FA9,1 Self modified
C $8FAA,4 Restore IX, HL, BC
C $8FAE,1 A = *HL
C $8FAF,1 Set flags
C $8FB2,4 IX += 2
C $8FB6,3 L += C
C $8FB9,1 A = *HL
C $8FBA,1 Set flags
C $8FBB,2 Exit via $9023 if non-zero
N $8FBD This entry point is used by the routine at #R$9023.
C $8FBD,4 IX += 2
C $8FC1,4 L -= $21
C $8FC5,2 IY--
C $8FC7,2 Loop to $8F96 while B
C $8FC9,3 Load self modified instructions from earlier
C $8FCC,1 Set flags
C $8FCD,1 Return if non-zero (not NOP)
C $8FCE,5 Return if floating_arrow is zero
C $8FD3,3 HL = &floating_arrow_defns[0]
C $8FD6,2 E = $78
C $8FD8,1 A--
C $8FDB,3 HL = &floating_arrow_defns[1]
C $8FDE,2 E = $80
C $8FE0,2 D = $30
C $8FE2,2 C = *HL++
C $8FE4,1 B = *HL
C $8FE5,2 B >>= 1
C $8FEA,1 E = C
C $8FEB,2 C = $00
C $8FEE,1 HL++
C $8FEF,2 D = *HL++
C $8FF1,2 A = *HL++
C $8FF3,1 H = *HL
C $8FF4,1 L = A
C $8FF5,3 Exit via $B6DD
C $8FF8,1 E = A
C $8FF9,3 A = IX[1]
C $8FFC,1 Set flags
C $9003,8 DE = E * 7
C $900B,3 Push return address onto stack
C $9012,1 HL += DE
C $9013,4 DE = wordat(HL); HL += 2
C $9017,4 HL = wordat(HL); HL += 1
C $901B,1 Jump to HL
C $9020,3 Jump to $8FB2
c $9023 Routine at 9023
D $9023 Used by the routine at #R$8F5F.
C $9023,1 E = A
C $9024,4 Jump if A == 2
C $9028,3 A = IX[1]
C $902B,1 Set flags
C $902E,4 Preserve IX, HL, BC
C $9032,8 DE = E * 7
C $903A,4 Set return address
C $903E,3 turn sign lods
C $9041,1 HL += DE
C $9042,2 E = *HL++
C $9044,2 D = *HL++
C $9046,2 A = *HL++
C $9048,1 H = *HL
C $9049,1 L = A
@ $904B label=return_904B
C $904B,4 Restore IX, HL, BC
C $904F,3 Loop?
c $9052 Routine at 9052 - gets hit on stage 3 when drawing the overhead structure graphics. sampled IX=$EAB2 DE=$6F26 HL=$9052 BC=$1420 (when in stage3!)
@ $9052 label=draw_overhead
C $9052,4 Preserve
C $9056,3 A = IX[1]
C $9059,1 Set flags
C $905D,4 Restore (restore DE to HL)
C $9061,1 HL++
C $9062,3 DE = wordat(HL); HL++
C $9065,2 DE += 2
C $9067,5 A = fast_counter & $E0
C $906C,1 L = A
C $9071,1 A -= L
C $9076,1 A -= L
C $9077,1 A += B
C $9078,1 L = A
C $9079,2 H = $E6
C $907B,6 A = IY[0] - IY[$35]
C $9081,1 C = A
C $9082,1 A = *HL
C $9083,1 L = A
C $9084,2 SRL A
C $9086,1 A += L
C $9087,1 A -= C
C $9088,3 *$90F2 = A
C $908B,2 A = B - 1
C $9091,2 A = $09
C $9093,1 B = A
C $9094,5 HL = A * 2 + DE
C $9099,1 C = *HL
C $909A,1 A = B
C $909B,8 HL = ((A * 2) + B + 20) + DE
C $90A3,2 D = 1
C $90A5,3 A = IX[1]
C $90A8,1 Set flags
C $90AD,6 A = IX[0] + $18 - C
C $90B5,2 A -= 8
C $90BD,6 A >>= 3
C $90C3,1 D = A
C $90C4,4 IX -= 2
C $90C8,2 E = $1F
C $90CA,3 A = IX[1]
C $90CD,1 Set flags
C $90D1,4 A = IX[0] + C
C $90DD,6 A >>= 3
C $90E3,1 E = A
C $90E4,1 C = D
C $90E5,3 A = E * 2 - C
C $90E8,3 A = ~A + $3D
C $90EB,3 *$9116 = A   -- Self modify jump table target
C $90EE,3 A = IY[$35]
C $90F1,2 A -= $00     SM?
C $90F5,1 A++
C $90F6,1 B = *HL
C $90F7,1 A -= B
C $90FA,1 A += B
C $90FB,1 B = A
C $90FC,1 HL++
C $90FD,3 DE = wordat(HL); HL++
C $9101,1 L = A
C $9102,5 H = (A & $0F) + $F0
C $9107,6 L = (L & $70) * 2 + C
@ $910F label=continue_910f
C $910F,1 DE++
C $9112,1 Return
C $9113,1 A = *DE
C $9114,1 C = L
C $9115,2 Self modified
C $9117,58 *HL++ = A  -- 28 times
C $9151,1 *HL = A
C $9152,1 L = C  -- restore
N $9153 Move to next scanline
C $9153,1 Save for checking in a moment
C $9154,1 Move to next scanline (visually upwards)
C $9155,2 Would it have rolled over into the top nibble?
C $9157,3 No - continue
N $915A It rolled over
C $915A,4 Put back the bit stolen by rollover
C $915E,4 Move to next chunk of 16 scanlines
C $9162,3 Continue if it didn't roll over
C $9165,4 Otherwise move to the next chunk of 128 scanlines (would put us outside the back buffer)
C $9169,3 Loop
c $916C Routine at 916C
D $916C Used by the routine at #R$9052.
C $916C,3 HL = $9293
C $9171,3 HL = $92FC    -- entry point
C $9174,3 *$91CE = HL
C $9177,3 *$9244 = HL
C $917A,5 A = fast_counter & $E0
C $917F,6 A -= A >> 2
C $9189,3 L = A - L + B
C $918C,2 H = $E6   ; $E6xx data
C $918E,1 A = *HL
C $918F,3 *$91DC = A
C $9193,1 A = B
C $9194,2 CP 10
C $9198,2 A = 10
C $919B,1 A--
C $919C,1 E = A
C $919D,2 D = 0
C $919F,1 C = D
C $91A0,4 *$91BB = DE
C $91A4,3 A = -C
C $91A7,3 *$933E = A
C $91AA,2 B = *HL - 1
C $91AD,1 HL++
C $91AE,4 DE = wordat(HL); HL += 2
C $91B7,3 DE = wordat(HL); HL++
C $91BA,3 must be SM
C $91BD,1 HL += BC
C $91BE,2 A = *HL++
C $91C0,3 HL = *HL
C $91C3,1 HL += DE
C $91C5,1 B--
C $91C8,1 B = *HL
C $91C9,2 HL -= 2
C $91CC,1 B = A
C $91D1,2 A = C + B
C $91D3,1 C = A
C $91DB,2 A = $00
C $91DD,1 B--
C $91E0,1 B--
C $91E3,1 B--
C $91E6,1 B--
C $91E9,1 B--
C $91EC,1 B--
C $91EF,1 B--
C $91F2,1 A += A
C $91F5,4 A >>= 2
C $91FD,1 B = A
C $9200,1 A += B
C $9205,1 B = A
C $9206,4 B >>= 2
C $920A,1 A -= B
C $920D,1 B = A
C $920E,6 B >>= 3
C $9214,1 A += B
C $921B,1 B = A
C $921E,1 A += B
C $921F,1 A -= C
C $9224,2 A = 1
C $9226,1 B = A
C $9228,6 A = IY[$35] + 1 - C - B
C $9230,1 A += B
C $9231,1 B = A
C $9232,1 A = B
C $9233,3 *$9405 = A
C $9236,1 A = *HL
C $9237,1 HL -= 2
C $9238,1 HL--
C $9239,3 *$9416 = A
C $923C,2 A = $02
C $923E,3 *$93C1 = A
C $9242,1 B = A
C $9246,4 *$93C1 = 0
c $924D Routine at 924D
R $924D R:B ?
C $924D,3 HL = $9279
C $9252,3 HL = $92E2   graphic 1 uses this entry
C $9255,1 A = B
C $9256,2 CP 16
C $9258,1 Return if A >= 16
C $925A,5 A = fast_counter & $E0
C $925F,1 L = A
C $9260,4 L >>= 2
C $9264,1 A -= L
C $9265,4 L >>= 2
C $9269,1 A -= L
C $926A,1 A += B
C $926B,1 L = A
C $926C,2 H = $E6    ; $E6xx data
C $926E,1 A = *HL
C $926F,8 A = (A >> 2) - A
C $9277,1 Return
c $9278 Routine at 9278
R $9278 I:B ?
R $9278 I:IX ?
R $9278 I:DE Address of arg, e.g. $6B0C
@ $9278 label=sub_9278
C $9278,4 *$933E = 0
C $927C,1 A = B
C $927D,2 CP 10
C $927F,2 Jump to $9283 if A < 10
C $9281,2 A = 10  -- so it's a max() operator
C $9284,3 DE = wordat(HL); HL++   -- loads address of turn_sign_lods
C $9287,6 HL += A * 2 - 1   -- index the table
C $928D,2 B = *HL++   -- this reads 6AF8
C $928F,3 HL = *HL   -- read as byte
C $9292,1 HL += DE
N $9293 This entry point is used by the routine at #R$916C.
C $9293,6 A = IX[0] + 16 - B
C $9299,1 Return if carry (if IX[0] + 16 < B)
N $929A This entry point is used by the routines at #R$A9DE, #R$AA38 and #R$ADA0.
C $929A,3 Return if A < 8
C $929D,2 C = 0
C $929F,1 E = *HL
C $92A0,6 likely E << 3
C $92A6,1 A -= E
C $92A9,4 Jump if A >= 8
C $92AD,1 E = *HL
C $92B0,3 *$9396 = A
C $92B3,2 A = E - 1
C $92B5,3 BC = $0101
C $92BB,1 E = *HL
C $92BC,2 A &= $FC
C $92C0,3 *$9396 = A
C $92C4,1 B = A
C $92C5,3 A += E - 33
C $92C8,2 Return if carry or zero
C $92CA,1 D = A
C $92CB,4 C = E - A
C $92CF,1 A = D
C $92D0,2 B = 1
C $92D2,1 HL++
C $92D3,3 D = *HL >> 1
C $92D9,1 B--
C $92DA,1 A++
C $92DB,2 C = 0
c $92E1 Drives the sprite plotters  WHAT CALLS THIS PROB $901B
C $92E1,4 *$933E = 0
C $92E5,1 A = B
C $92E6,2 CP 10
C $92EA,2 A = 10
C $92ED,3 DE = wordat(HL); HL++
C $92F0,6 HL += A * 2 - 1
C $92F6,2 B = *HL++
C $92F8,4 HL = *HL + DE
C $92FC,3 A = IX[0]
C $9303,1 A += B
C $9306,1 A += B
C $9307,1 Return if carry
C $9308,1 Return if zero
N $9309 This entry point is used by the routines at #R$A9DE, #R$AA38 and #R$ADA0.
C $930B,1 Return if not carry
N $930E This entry point is used by the routine at #R$9278.
C $930E,4 A = (A & $FC) >> 2
C $9312,3 *$9396 = A
C $9315,1 A >>= 1
C $9316,1 B = A
C $9317,1 E = *HL
C $9318,3 A = ~A + 32  -- (31 - A)
C $931E,1 A = E
C $931F,1 HL++
C $9320,3 D = *HL >> 1
C $9326,1 C = A
C $9328,3 C = E - C
N $932B This entry point is used by the routine at #R$9278.
C $932B,7 *$9396 = ~(*$9396)
N $9333 This entry point is used by the routine at #R$9278.
C $933C,1 HL++
C $933D,2 D = $00
C $933F,6 A = IY[0] - IY[$35]
C $9347,4 A = IY[$35] + D
C $934D,1 A++
C $934E,1 D = *HL
C $934F,1 A -= D
C $9352,1 A = 0
C $9353,1 A += D
C $9354,1 HL++
C $9355,2 D = 1
C $935D,1 A -= D
C $9361,1 A += D
C $9362,1 D = A
C $9369,3 A = IY[$35]
C $936D,3 A = *HL - 1 - D
C $9374,3 A = *$93C1
C $9377,1 Set flags
C $9378,1 Return if zero
C $9379,3 A = *$933E
C $937C,1 D = *HL
C $937D,1 A -= D
C $937F,3 *$933E = A
C $9382,4 A = *$9405 - D
C $9386,2 Return if carry or zero
C $9388,3 *$9405 = A
C $938D,1 A++
C $938E,1 D++
C $938F,1 HL++
C $9392,1 D = A
C $9393,2 B = $00
C $9395,2 A = $00
C $939A,2 HL += 2
C $939C,5 HL = wordat(HL) + BC
C $93A1,3 *$9413 = HL
C $93A4,1 C = E
C $93A6,1 A--
C $93A9,1 HL += BC
C $93AA,1 A--
C $93AE,1 B = D
C $93AF,2 D = $00
C $93B4,1 D = A
C $93B5,5 H = (A & $0F) + $F0
C $93BA,1 A = D
C $93BB,5 L = (A & $70) * 2 + B
C $93C0,2 A = <...> -- Self modified by various
C $93C2,1 Set flags
C $93C6,1 A--
C $93D3,3 prob plot_sprite_flipped
C $93D6,3 Exit via plot_sprite
C $93D9,3 Exit via plot_masked_sprite_flipped
C $93DC,3 Exit via plot_sprite_flipped
C $93E3,2 A >>= 1
C $93E8,4 #REGix = Base of jump table
C $93EC,3 (~#REGa + 5) is (4 - #REGa)
C $93EF,9 IX += A * 5
C $93F8,3 BC = &plot_sprite_entry
C $93FB,4 *$9410 = BC
C $93FF,4 *$941E = BC
C $9404,3 A = 0 - B
C $940B,3 *$9405 = A
C $940F,3 Call plot_sprite_entry
C $9412,3 HL = <...>  Self modified by $93A1
C $9415,2 B = <...>   Self modified by $9239
C $941A,1 A += B
C $941B,1 B = A
C $941D,3 Exit via plot_sprite_entry
@ $9420 label=plot_sprite_xxx_odd
C $9420,1 Increment #REGa for upcoming calculation
C $9421,4 Point #REGix at start of plot instructions
C $9425,3 (~#REGa + 5) is (4 - #REGa) is the number of plot operations to skip
C $9428,3 Multiply #REGa by 5: the length of an individual plot operation
C $942B,4 Move result to #REGbc
C $942F,2 Add it to #REGix to complete the jump target
C $9431,3 BC = $9503
C $9436,4 IX = $B729
C $943A,3 (~#REGa + 9) is (8 - #REGa)
C $943D,9 IX += A * 6
C $9446,8 *$946D = *$9413  -- Self modify LD HL at $946C to load ?
C $944E,6 *$9470 = *$9416
C $9454,6 *$9460 = *$9405
C $945A,2 B = $0F
C $945D,2 D = $00
C $945F,3 A = 0 - B
C $9466,3 *$9460 = A
C $9469,3 Call plot_masked_sprite
C $946C,3 HL = <...>  -- Self modified by $9446
C $946F,2 B = $00
C $9474,2 B += A
C $9476,3 Exit via plot_masked_sprite
C $947B,2 D = $00
C $947F,1 H = D
C $9480,1 L = D
C $9481,1 A = B
C $9482,2 B = 5
C $9484,1 A--
C $9485,2 A <<= 2
@ $9487 label=loop_9487
C $9487,1 A <<= 1
C $9488,3 If carried out then HL += DE
C $948B,1 HL += HL
C $948C,2 Loop to loop_9487 while #REGb
C $9491,1 HL += DE
C $9493,1 HL += BC
C $9495,1 D--
C $9496,4 E = -E
C $949B,1 fall through
c $949C Sprite plotter for back buffer, up to 64px wide, 15px high, no mask, no flip.
D $949C Used by the routines at #R$85E4, #R$92E1 and #R$B58E.
R $949C I:A Plot (#REGa + 1) * 8 pixels
R $949C I:DE' Stride of bitmap data in bytes
R $949C I:HL Address in back buffer to plot at
R $949C I:HL' Address of bitmap data
@ $949C label=plot_sprite
C $949C,5 Use ps_odd if the bottom bit is set (odd widths)
C $94A1,4 #REGix = Base of jump table
C $94A5,3 (~#REGa + 5) is (4 - #REGa) is the number of plot operations to skip
C $94A8,4 Multiply #REGa by 5: the length of an individual plot operation
C $94AC,3 Move result to #REGbc
C $94AF,2 Add it to #REGix to complete the jump target
N $94B1 This entry point is used by the routine at #R$92E1.
@ $94B1 label=plot_sprite_entry
C $94B1,4 Save #REGsp to restore later (self modify)
C $94B5,3 #REGb = 15 rows to draw, #REGc = 16, an increment value used later
C $94B8,1 Bank
C $94B9,2 Jump into body of loop
@ $94BB label=ps_even_loop
C $94BB,1 Bank
@ $94BC label=ps_even_continue
C $94BC,2 Next scanline
C $94BE,3 Restore #REGsp (self modified)
C $94C1,1 Return
@ $94C2 label=ps_even_next
C $94C2,1 Calculate address of next bitmap scanline
@ $94C3 label=ps_even_body
C $94C3,1 Put it in #REGsp (so we can use POP for speed)
C $94C4,2 Unbank
C $94C6,2 Jump table
C $94C8,5 Transfer two bitmap bytes (16 pixels) from the "stack" to screen buffer
C $94CD,5 Transfer another 16 pixels
C $94D2,5 Transfer another 16 pixels
C $94D7,4 Transfer another 16 pixels
C $94DB,1 Restore #REGhl
C $94DC,2 Save H in A then H--
C $94DE,1 Extract low four bits of row address
C $94DF,3 If zero we'll need to handle it below, otherwise just loop
C $94E2,3 H += 16
C $94E5,4 Decrement the high three bits of row address
C $94E9,3 No carry, so finish scanline
C $94EC,1 H -= 16
C $94ED,5 Loop back to ps_even_loop
@ $94F2 label=ps_odd
C $94F2,1 Increment #REGa for upcoming calculation
C $94F3,4 Point #REGix at start of plot instructions
C $94F7,3 (~#REGa + 5) is (4 - #REGa) is the number of plot operations to skip
C $94FA,4 Multiply #REGa by 5: the length of an individual plot operation
C $94FE,3 Move result to #REGbc
C $9501,2 Add it to #REGix to complete the jump target
C $9503,4 Save #REGsp to restore later (self modify)
C $9507,3 #REGb = 15 rows to draw, #REGc = 16, an increment value used later
C $950A,1 Bank
C $950B,2 Jump into body of loop
@ $950D label=ps_odd_loop
C $950D,1 Bank
@ $950E label=ps_odd_continue
C $950E,2 Next scanline
C $9510,3 Restore #REGsp (self modified)
C $9513,1 Return
@ $9514 label=ps_odd_next
C $9514,1 Calculate address of next bitmap scanline
@ $9515 label=ps_odd_body
C $9515,1 Put it in #REGsp (so we can use POP for speed)
C $9516,2 Unbank
C $9518,2 Jump table
C $951A,5 Transfer two bitmap bytes (16 pixels) from the "stack" to screen buffer
C $951F,5 Transfer another 16 pixels
C $9524,5 Transfer another 16 pixels
C $9529,2 Transfer another 8 pixels
C $952B,1 Restore #REGhl
C $952C,2 Save H in A then H--
N $952E Decrement the screen address.
C $952E,1 Extract low four bits of row address
C $952F,3 If zero we'll need to handle it below, otherwise just loop
C $9532,3 H += 16
C $9535,4 Decrement the high three bits of row address
C $9539,3 No carry, so finish scanline
C $953C,3 H -= 16
C $953F,3 Loop back to ps_odd_loop
c $9542 Sprite plotter for back buffer, up to 64px wide, 15px high, no mask, flips.
D $9542 Used by the routines at #R$92E1 and #R$B58E.
R $9542 I:A Plot (#REGa + 1) * 8 pixels
R $9542 I:DE' Stride of bitmap data in bytes
R $9542 I:HL Address in back buffer to plot at
R $9542 I:HL' Address of bitmap data
@ $9542 label=plot_sprite_flipped
C $9542,4 Advance the back buffer pointer to the end of the first line, so we can draw in reverse
C $9546,5 Use psf_odd if the bottom bit is set (odd widths)
C $954B,4 Save #REGsp to restore later (self modify)
C $954F,4 #REGix = Base of jump table
C $9553,3 (~#REGa + 5) is (4 - #REGa) is the number of plot operations to skip
C $9556,5 Multiply #REGa by 9: the length of an individual plot operation
C $955B,3 Move result to #REGbc
C $955E,2 Add it to #REGix to complete the jump target
C $9560,2 Point #REGde at table of flipped bytes at $EF00
C $9562,1 Bank
C $9565,2 Jump into body of loop
@ $9567 label=psf_even_continue
C $9567,1 Bank
C $9568,2 Next scanline
C $956A,3 Restore #REGsp (self modified)
C $956D,1 Return
@ $956E label=psf_even_next
C $956E,1 Calculate address of next bitmap scanline
@ $956F label=psf_even_body
C $956F,1 Put it in #REGsp (so we can use POP for speed)
C $9570,1 Unbank
C $9571,1 [check elsewhere too]
C $9573,2 Jump table
C $9575,1 Transfer two bitmap bytes (16 pixels) from the "stack" to screen buffer with flipping
C $9576,1 Set flip table address
C $9577,2 Flip byte and write to screen
C $9579,1 Advance backwards
C $957A,1 Set flip table address
C $957B,2 Flip byte and write to screen
C $957D,1 Advance backwards
C $957E,9 Transfer another 16 pixels
C $9587,9 Transfer another 16 pixels
C $9590,8 Transfer another 16 pixels
C $9599,1 Restore #REGhl
C $959A,2 Save H in A then H--
C $959C,2 Extract low four bits of row address
C $959E,3 If zero we'll need to handle it below, otherwise just loop
C $95A1,4 H += 16
C $95A5,4 Decrement the high three bits of row address
C $95A9,3 No carry, so finish scanline
C $95AC,4 H -= 16
C $95B0,3 Loop back to psf_even_continue
@ $95B3 label=psf_odd
C $95B3,1 Increment #REGa for upcoming calculation
C $95B4,4 Save #REGsp to restore later (self modify) [in a different position to others]
C $95B8,4 Point #REGix at start of plot instructions
C $95BC,3 (~#REGa + 5) is (4 - #REGa) is the number of plot operations to skip
C $95BF,5 Multiply #REGa by 9: the length of an individual plot operation
C $95C4,3 Move result to #REGbc
C $95C7,2 Add it to #REGix to complete the jump target
C $95C9,2 Point #REGde at table of flipped bytes at $EF00
C $95CB,1 Bank
C $95CE,2 Jump into body of loop
@ $95D0 label=psf_odd_continue
C $95D0,1 Bank
C $95D1,2 Next scanline
C $95D3,3 Restore #REGsp (self modified)
C $95D6,1 Return
@ $95D7 label=psf_odd_next
C $95D7,1 Calculate address of next bitmap scanline
@ $95D8 label=psf_odd_body
C $95D8,1 Put it in #REGsp (so we can use POP for speed)
C $95D9,1 Unbank
C $95DC,2 Jump table
C $95DE,1 Transfer two bitmap bytes (16 pixels) from the "stack" to screen buffer with flipping
C $95DF,1 Set flip table address
C $95E0,2 Flip byte and write to screen
C $95E2,1 Advance backwards
C $95E3,1 Set flip table address
C $95E4,2 Flip byte and write to screen
C $95E6,1 Advance backwards
C $95E7,9 Transfer another 16 pixels
C $95F0,9 Transfer another 16 pixels
C $95F9,4 Transfer another 8 pixels
C $95FE,1 Restore #REGhl
C $95FF,2 Save H in A then H--
N $9601 Decrement the screen address.
C $9601,2 Extract low four bits of row address
C $9603,3 If zero we'll need to handle it below, otherwise just loop
C $9606,4 H += 16
C $960A,4 Decrement the high three bits of row address
C $960E,3 No carry, so finish scanline
C $9611,4 H -= 16
C $9615,3 Loop back to psf_odd_continue
c $9618 Random number generator
R $9618 O:A Random byte?
@ $9618 label=rng_seed
B $9618,3,3 Seed / initial state
N $961B This entry point is used by the routines at #R$860F, #R$8A0F, #R$8DF9, #R$99EC, #R$A637, #R$A7F3 and #R$A955.
@ $961B label=rng
C $961B,3 Point at rng_seed / state bytes
C $961E,4 rng_seed[0] -= 141
C $9622,4 rng_seed[1] += 3
C $9626,2 A = rng_seed[0] + rng_seed[1]
C $9628,1 Rotate A by 1
C $9629,2 Rotate rng_seed[2] by 1
C $962B,2 rng_seed[2] += A
C $962D,1 Return
g $962E In-game message variables
@ $962E label=next_character
W $962E,2,2 Address of the next character in the current message
@ $9630 label=next_message
W $9630,2,2 Address of the _next_ message to show DOUBTING THIS NOW ... pointer to next command?
@ $9632 label=message_x
B $9632,1,1 Index of next character in the message bar
@ $9633 label=bleh
B $9633,1,1 zeroed by $995d set to 10 by $9a49
@ $9634 label=noise_bytes
B $9634,5,5 Five bytes used for noise when character pictures 'noise in'
B $9639,3,3
@ $963C label=noise_counter
B $963C,1,1 Noise effect counter (4..0)
@ $963D label=TBD963d
B $963D,1,1 read by $9946, one'd by $9961, set by $99d9 character index?
@ $963E label=TBD963e
B $963E,1,1 read by $9950, set by $9956
b $963F In-game messages
T $963F,27,26:n1 "THIS IS NANCY AT CHASE H.Q."
T $965A,39,38:n1 "THIS IS SPECIAL INVESTIGATION AIRBORNE."
T $9681,29,28:n1 "THE TARGET VEHICLE HAS TURNED"
T $969E,23,22:n1 "RIGHT UP AHEAD... OVER."
T $96B5,22,21:n1 "LEFT UP AHEAD... OVER."
T $96CB,29,28:n1 "WE READ LOUD AND CLEAR! OVER."
T $96E8,6,5:n1 "ROGER!"
T $96EE,19,18:n1 "GOTCHA NANCY BABY!"
T $9701,23,22:n1 "WHAT ARE YOU DOING MAN!!"
T $9718,37,36:n1 "THE BAD GUYS ARE GOING THE OTHER WAY."
T $973D,36,35:n1 "IF YOU KEEP MESSIN' AROUND LIKE THAT"
T $9761,38,37:n1 "YOUR TIME IS GOING TO RUN OUT... OVER."
T $9787,21,20:n1 "LET'S GET MOVIN' MAN!"
T $979C,7,6:n1 "OH, NO!"
T $97A3,7,6:n1 "PLEASE!"
T $97AA,6,5:n1 "GREAT!"
T $97B0,5,4:n1 "OUCH!"
T $97B5,21,20:n1 "LET'S GO. MR. DRIVER."
T $97CA,9,8:n1 "YAOOOOOW!"
T $97D3,10,9:n1 "BEAR DOWN."
T $97DD,19,18:n1 "MORE, PUSH IT MORE!"
T $97F0,14,13:n1 "ONE MORE TIME."
T $97FE,7,6:n1 "OH MAN."
T $9805,9,8:n1 "WHOAAAAA!"
T $980E,7,6:n1 "HARDER!"
T $9815,34,33:n1 "WE THINK YOU PICKED THE WRONG JOB."
T $9837,32,31:n1 "BETTER CHECK THE CLASSIFIED ADS."
T $9857,34,33:n1 "ONE MORE TRY FOR BEING A GOOD BOY!"
T $9879,34,33:n1 "YOU'RE A MEDIOCRE DRIVER, BROTHER!"
T $989B,14,13:n1 "SEE YOU LATER."
b $98A9 In-game chatter structures
@ $98A9 label=pilot_turn_left_chatter
B $98A9,1,1 Pilot ($00)
W $98AA,2,2 -> "THIS IS SPECIAL INVESTIGATION AIRBORNE."
W $98AC,2,2 -> "THE TARGET VEHICLE HAS TURNED"
W $98AE,2,2 -> "LEFT UP AHEAD... OVER."
B $98B0,1,1
W $98B1,2,2 -> Tony: "WE READ LOUD AND CLEAR! OVER." <STOP>
@ $98B3 label=pilot_turn_right_chatter
B $98B3,1,1 Pilot ($00)
W $98B4,2,2 -> "THIS IS SPECIAL INVESTIGATION AIRBORNE."
W $98B6,2,2 -> "THE TARGET VEHICLE HAS TURNED"
W $98B8,2,2 -> "RIGHT UP AHEAD... OVER."
B $98BA,1,1
W $98BB,2,2 -> Tony: "WE READ LOUD AND CLEAR! OVER." <STOP>
@ $98BD label=acknowledge_chatter
B $98BD,1,1 Three-way random choice ($FC)
W $98BE,2,2 -> Tony: "WE READ LOUD AND CLEAR! OVER." <STOP>
W $98C0,2,2 -> Raymond: "ROGER!" <STOP>
W $98C2,2,2 -> Tony: "GOTCHA NANCY BABY!" <STOP>
B $98C4,1,1 Tony ($03)
W $98C5,2,2 -> "WE READ LOUD AND CLEAR! OVER."
B $98C7,1,1 <STOP>
B $98C8,1,1 Raymond ($02)
W $98C9,2,2 -> "ROGER!"
B $98CB,1,1 <STOP>
B $98CC,1,1 Tony ($03)
W $98CD,2,2 -> "GOTCHA NANCY BABY!"
B $98CF,1,1 <STOP>
B $98D0,1,1 Raymond ($02)
W $98D1,2,2 -> "WHAT ARE YOU DOING MAN!!"
W $98D3,2,2 -> "THE BAD GUYS ARE GOING THE OTHER WAY."
B $98D5,1,1 <STOP>
@ $98D6 label=smash_chatter
B $98D6,1,1 Three-way random choice ($FC)
W $98D7,2,2 -> Raymond: "BEAR DOWN." <STOP>
W $98D9,2,2 -> Three-way random choice of "OH MAN." / "HARDER!" / "PLEASE!"
W $98DB,2,2 -> Raymond: "MORE, PUSH IT MORE!" <STOP>
B $98DD,1,1 Raymond ($02)
W $98DE,2,2 -> "BEAR DOWN."
B $98E0,1,1 <STOP>
B $98E1,1,1 Raymond ($02)
W $98E2,2,2 -> "MORE, PUSH IT MORE!"
B $98E4,1,1 <STOP>
B $98E5,1,1 Raymond ($02)
W $98E6,2,2 -> "HARDER!"
B $98E8,1,1 <STOP>
B $98E9,1,1 Raymond ($02)
W $98EA,2,2 -> "OH MAN."
B $98EC,1,1 <STOP>
B $98ED,1,1 Three-way random choice ($FC)
W $98EE,2,2 -> Raymond: "OH MAN." <STOP>
W $98F0,2,2 -> Raymond: "HARDER!" <STOP>
W $98F2,2,2 -> Raymond: "PLEASE!" <STOP>
B $98F4,1,1 Raymond ($02)
W $98F5,2,2 -> "PLEASE!"
B $98F7,1,1 <STOP>
@ $98F8 label=get_moving_chatter
B $98F8,1,1 Raymond ($02)
W $98F9,2,2 -> "LET'S GET MOVIN' MAN!"
B $98FB,1,1 <STOP>
@ $98FC label=time_running_out_chatter
B $98FC,1,1 Nancy ($01)
W $98FD,2,2 -> "THIS IS NANCY AT CHASE H.Q."
W $98FF,2,2 -> "IF YOU KEEP MESSIN' AROUND LIKE THAT"
W $9901,2,2 -> "YOUR TIME IS GOING TO RUN OUT... OVER."
B $9903,1,1 <STOP>
@ $9904 label=ouch_chatter
B $9904,1,1 Three-way random choice ($FC)
W $9905,2,2 -> Raymond: "OH, NO!" <STOP>
W $9907,2,2 -> Raymond: "OUCH!" <STOP>
W $9909,2,2 -> Raymond: "YAOOOOOW!" <STOP>
B $990B,1,1 Raymond ($02)
W $990C,2,2 -> "OH, NO!"
B $990E,1,1 <STOP>
B $990F,1,1 Raymond ($02)
W $9910,2,2 -> "OUCH!"
B $9912,1,1 <STOP>
B $9913,1,1 Raymond ($02)
W $9914,2,2 -> "YAOOOOOW!"
B $9916,1,1 <STOP>
@ $9917 label=turbo_chatter
B $9917,1,1 Three-way random choice ($FC)
W $9918,2,2 -> Tony: "WHOAAAAA!" <STOP>
W $991A,2,2 -> Tony: "GREAT!" <STOP>
W $991C,2,2 -> Raymond: "ONE MORE TIME." <STOP>
B $991E,1,1 Tony ($03)
W $991F,2,2 -> "WHOAAAAA!"
B $9921,1,1 <STOP>
B $9922,1,1 Tony ($03)
W $9923,2,2 -> "GREAT!"
B $9925,1,1 <STOP>
@ $9926 label=raymond_says_one_more_time
B $9926,1,1 Raymond ($02)
W $9927,2,2 -> "ONE MORE TIME."
B $9929,1,1 <STOP>
@ $992A label=failed_chatter
B $992A,1,1 Three-way random choice ($FC)
W $992B,2,2 -> Nancy: "WE THINK YOU PICKED THE WRONG JOB." "BETTER CHECK THE CLASSIFIED ADS."
W $992D,2,2 -> Nancy: "ONE MORE TRY FOR BEING A GOOD BOY!"
W $992F,2,2 -> Nancy: "YOU'RE A MEDIOCRE DRIVER, BROTHER!" "SEE YOU LATER."
B $9931,1,1 Nancy ($01)
W $9932,2,2 -> "WE THINK YOU PICKED THE WRONG JOB."
W $9934,2,2 -> "BETTER CHECK THE CLASSIFIED ADS."
B $9936,1,1 <STOP>
B $9937,1,1 Nancy ($01)
W $9938,2,2 -> "ONE MORE TRY FOR BEING A GOOD BOY!"
B $993A,1,1 <STOP>
B $993B,1,1 Nancy ($01)
W $993C,2,2 -> "YOU'RE A MEDIOCRE DRIVER, BROTHER!"
W $993E,2,2 -> "SEE YOU LATER."
B $9940,1,1 <STOP>
@ $9941 label=lets_go_chatter
B $9941,1,1 Tony ($03)
W $9942,2,2 -> "LET'S GO. MR. DRIVER."
B $9944,1,1 <STOP>
c $9945 Chatter routine
D $9945 Used by the routines at #R$8401, #R$858C, #R$873C, #R$8876, #R$9BCF, #R$A637, #R$A8CD, #R$AB33, #R$B063, #R$B4F0 and #R$B9F4.
R $9945 I:A TBD
R $9945 I:HL Address of message set
@ $9945 label=chatter
C $9945,1 B = A
C $9949,3 If A == 0 jump forward to 9955
C $994C,4 If A >= 3 jump forward to 9955
C $9953,2 If A >= B return
C $9955,4 $963E = B
C $9959,3 Set message set pointer
C $995C,4 $9633 = 0
C $9960,4 $963D = 1
C $9964,1 Return
c $9965 Noise effect, Message plotting
D $9965 Used by the routines at #R$8401, #R$858C and #R$873C.
@ $9965 label=drive_chatter
C $9965,4 A = TBD963d - 1
C $9969,2 if A was zero jump
C $996B,1 A--
C $996C,2 if A was zero jump to sub998f_do_noise_effect
C $996E,1 A--
C $996F,2 if A was NOT zero jump
C $9971,4 Decrement noise_counter
C $9975,1 A = *HL A = noise_counter
C $9976,3 Jump if noise_counter was non-zero
C $9979,3 Zero this
C $997C,3 Call noise_plot_attrs -- clear to black
C $997F,2 Plot space character
C $9981,2 {$AA ROR 1 becomes $55 becomes $AA and so on
C $9983,1 probably a delay / alternating call
C $9984,3 }
C $9987,2 ...and then gets overwritten?
C $9989,3 jump on $55 -> $AA transitions?
C $998C,3 Jump to plot_mini_font_1
@ $998F label=sub998f_do_noise_effect
C $998F,7 If noise_counter > 0 call noise_effect
C $9996,3 If $9633 == 0 jump $99b6
C $999C,1 $9633--
C $99A1,2 If A was zero jump sub998f_another_something
C $99A3,3 Load address of next character
C $99A6,1 Go back 1
C $99A7,1 Load the character for when we call plot_mini_font*
C $99A8,2 Clear string terminator bit
C $99AA,4 message_x - 1
C $99AE,2 rotate it by B  (B is noise counter) .. wut?
C $99B0,3 if carry jump plot_mini_font_2
C $99B3,3 Jump to plot_mini_font_1
@ $99B6 label=sub998f_something
C $99B6,7 If message_x != 0 jump $9a30
@ $99BD label=sub998f_another_something
C $99BD,3 HL = next_message
C $99C0,1 A = *HL first char of
C $99C1,4 String terminator? jump to clear_chatter if so
C $99C5,2 this tests for $FE -- different terminator?
C $99C7,2 If not $fe goto chatter_message
C $99CA,7 next_message = *HL
C $99D1,2 goto 99e4_exit
N $99D3 This entry point is used by the routines at #R$858C and #R$8876.
@ $99D3 label=clear_chatter
C $99D3,5 Set the noise effect counter to 4
C $99D8,4 Set $963D to 3
C $99DC,3 Exit via clear_message_line
@ $99DF label=99df_something_else
C $99DF,5 $963D = 2
@ $99E4 label=99e4_exit
C $99E4,8 Call clear_message_line
c $99EC Shows the alerts and remarks from the game's characters
D $99EC Used by the routine at #R$9A55.
@ $99EC label=print_chatter
C $99EC,3 Fetch address of next remark
@ $99EF label=chatter_loop
C $99EF,2 Read a byte and advance
C $99F1,2 if (byte != $FC) then jump plot_character
@ $99F5 label=random_chatter
N $99F6 We have a three-way choice here
C $99F6,3 Call rng - returns a random byte
C $99FA,4 If random value < $55 jump load_message_ptr -- a 33.3% chance
C $99FE,2 Skip first option message
C $9A00,4 If random value < $AA jump load_message_ptr -- a 66.6% chance
C $9A04,2 Skip second option message
@ $9A06 label=load_message_ptr
C $9A06,4 Load the address of the message
C $9A0A,2 Loop back - it could be another random case
N $9A0C #REGa is the index of the face to show
@ $9A0C label=plot_character
C $9A0C,1 Is it zero? (The definable/pilot character's face)
C $9A0D,1 Preserve message pointer
C $9A0E,6 Plot whatever face #R$5CF2 points to, otherwise calculate
C $9A14,3 Load base address of face graphics (BUT it's actually earlier than the real base because we're 1-indexed)
C $9A17,3 Length of face graphic (bitmap + attrs = 4*8*5 + 4*5)
C $9A1A,3 Get face graphic address
@ $9A1D label=do_plot
C $9A1D,3 Set plot address to (176,8)
C $9A20,3 Call plot_face
C $9A23,1 Restore message pointer
N $9A24 This entry point is used by the routine at #R$9965.
@ $9A24 label=chatter_message
C $9A24,4 Fetch address of message to start showing
C $9A28,3 Save address of next message to show (or perhaps the pointer)
C $9A2B,4 Save next character address
C $9A2F,1 Cause a clear_message_line and fall through
N $9A30 This entry point is used by the routine at #R$9965.
C $9A30,6 if (A == 0) clear_message_line
C $9A36,3 Fetch next_character
C $9A39,1 The character itself
C $9A3A,2 Clear the string terminator bit
C $9A3C,1 Preserve message_x
C $9A3E,3 Call plot_mini_font_2, with #REGd as ASCII character to plot
C $9A42,2 Test string terminator bit
C $9A44,1 Move to the next character in the string
C $9A45,2 If not terminated then jump over
C $9A47,5 $9633 = 10
C $9A4C,1 Restore message_x
C $9A4D,4 Increment and store message_x
C $9A51,3 Update next_character
C $9A54,1 Return
c $9A55 Noise in/out effect
D $9A55 Used by the routine at #R$9965.
R $9A55 I:A Noise effect counter
@ $9A55 label=noise_effect
C $9A55,4 Decrement the noise effect counter
C $9A59,3 Jump to print_chatter if it's zero, otherwise fallthrough
N $9A5C This entry point is used by the routine at #R$9965.
C $9A5C,1 Shift A's bottom bit into carry
C $9A5D,2 A = 255
C $9A5F,2 D = 32  -- ASCII character for plot_mini_font*
C $9A61,5 If A on input was odd then jump
C $9A66,2 else call plot_mini_font_2 and continue at $9a6b
C $9A68,3 Call plot_mini_font_1
C $9A6B,3 Set plot address to (176,8)
C $9A6E,4 B,C = 4 columns/bytes, 40 rows
C $9A72,1 Preserve row ptr
C $9A73,3 Point #REGhl at noise_bytes
@ $9A76 label=noise_plot_loop
C $9A76,2 A = *HL - B -- Subtract row counter (4,3,2,1)
C $9A78,2 *HL++ = A
C $9A7A,1 Rotate A left by 1
C $9A7B,2 *HL += A
C $9A7D,2 *DE++ = A -- Store 8 random bits
C $9A7F,2 Loop while columns remain
C $9A81,1 Restore row ptr
C $9A82,1 Move to next row
C $9A83,15 Usual row movement magic here
C $9A92,4 Loop while rows remain
C $9A96,2 #REGa = attribute $47 => BRIGHT + white over black
N $9A98 This entry point is used by the routine at #R$9965.
@ $9A98 label=noise_plot_attrs
C $9A98,3 Screen attribute position (22,1)
C $9A9B,2 5 rows
C $9A9D,3 32 - 3 = row skip
@ $9AA0 label=9aa0_loop
C $9AA0,7 Set four attribute bytes
C $9AA7,1 Move to start of next row
C $9AA8,2 Loop while rows remain
C $9AAA,1 Return
c $9AAB Plots a face
D $9AAB Used by the routine at #R$99EC.
R $9AAB I:HL Address of face to plot (32x40 bitmap followed by 4x5 attribute bytes)
R $9AAB I:DE Address of screen location (real screen)
@ $9AAB label=plot_face
C $9AAB,3 Byte length of bitmap - counter
C $9AAE,1 Save current screen address
@ $9AAF label=pf_loop
C $9AAF,1 Save screen address
C $9AB0,8 Transfer four bytes
C $9AB8,1 Restore source address
C $9AB9,3 If #REGbc became zero then proceed to plotting the attributes (PO => BC == 0)
C $9ABC,1 Move down a scanline
C $9ABD,5 Loop until we've done 8 lines (we've overflowed into the band bits)
C $9AC2,4 Move down a row (8 lines)
C $9AC6,2 If carry then loop (overflow is ok)
C $9AC8,4 Step back 8 lines (undo overflow)
C $9ACC,2 Loop
N $9ACE This entry point is used by the routine at #R$8EB7.
@ $9ACE label=pf_attrs_bit
C $9ACE,1 Restore current screen address
C $9ACF,6 Extract line bits (0..7)
C $9AD5,3 Turn it into an attribute address (works for first band only?)
C $9AD8,2 Byte length of attributes - counter
@ $9ADA label=pf_attrs_loop
C $9ADA,8 Transfer four attributes
C $9AE2,1 If #REGbc became zero then return
C $9AE3,4 Move down an attribute row
C $9AE7,2 If no carry then loop
C $9AE9,1 Move down 8 attribute rows
C $9AEA,2 Loop
c $9AEC Plot mini font characters
D $9AEC Used by the routines at #R$9965 and #R$9A55.
R $9AEC I:D The character to plot (ASCII)
@ $9AEC label=plot_mini_font_1
C $9AEF,2 Jump to pmf_go
N $9AF1 This entry point is used by the routines at #R$9965, #R$99EC and #R$9A55.
@ $9AF1 label=plot_mini_font_2
C $9AF1,3 values to self modify with
@ $9AF4 label=pmf_go
C $9AF5,8 self modify LD C and OR 7 later
C $9AFE,4 If not string terminator, goto pmf_regular_char
N $9B02 String terminator
C $9B02,2 $BF / 8?
@ $9B08 label=rpmf_egular_char
C $9B08,6 A = A*5+2
C $9B0E,4 8 - $C0 / 8 ??
C $9B12,7 divider / rounding?
C $9B19,8 Modify jump table - A * 4 - 4 bytes per sequence
C $9B21,3 General multiplier by A
C $9B24,3 Self modify $9B88 which is a mask
C $9B27,1 Get ASCII character
N $9B28 Turn ASCII into glyph IDs
C $9B28,2 likely the screen addr top
C $9B2A,6 Is it '.'? glyph ID = 26; goto have_glyph_id
C $9B30,5 Is it ','? glyph ID = 27; goto have_glyph_id
C $9B35,5 Is it '!'? glyph ID = 28; goto have_glyph_id
C $9B3A,5 Is it ' '? glyph ID = 29; goto have_glyph_id
C $9B3F,5 Is it '''? glyph ID = 30; goto have_glyph_id
C $9B44,4 Is it >= ';'? goto have_ascii
C $9B48,4 C += A - 47 -- not convinced this is ever used
@ $9B4C label=pmf_have_glyph_id
C $9B4C,3 Turn the glyph ID in #REGc into ASCII in #REGa
@ $9B4F label=pmf_have_ascii
C $9B4F,9 #REGbc = (#REGa - 'A') * 6
C $9B58,8 Point #REGhl at minifont
C $9B60,3 Self modified earlier
C $9B63,3 Self modified earlier
C $9B66,2 Jump table (self modified)
C $9B88,2 This must be self modified
C $9B8A,4 *scr++ = (*scr AND A) OR B
C $9B8E,2 *scr-- = C
C $9B90,1 Next source byte ?
C $9B91,16 Move to next scanline
C $9BA1,2 Decrement row counter
C $9BA3,3 Loop while rows remain
C $9BA6,1 Return
c $9BA7 Clear the whole message line
D $9BA7 Used by the routines at #R$9965 and #R$99EC.
@ $9BA7 label=clear_message_line
C $9BA7,3 Point #REGhl at screen coordinate (8,53)
C $9BAA,2 B = 0  top of LDIR count and byte to wipe memory with
C $9BAC,2 Clear six rows
@ $9BAE label=cml_loop
C $9BAF,3 DE = HL + 1
C $9BB2,2 29 bytes to clear
C $9BB4,1 Preserve HL
C $9BB5,3 Zero 29 bytes at DE
C $9BB9,16 Move to next scanline
@ $9BC9 label=cml_continue
C $9BCA,1 Decrement row counter
C $9BCB,3 Loop while rows remain to clear
C $9BCE,1 Return
c $9BCF Game timer
D $9BCF Used by the routine at #R$8401.
@ $9BCF label=tick
C $9BCF,5 Return if perp_caught_stage > 0
C $9BD4,6 Return if transition_control == 4 -- don't tick while transitions are running
C $9BDA,3 Point #REGhl at time_bcd
C $9BDD,6 Jump to $9C0D if time_up_state was 1 - out of time
C $9BE3,3 Jump to $9C31 if time_up_state was 2 - show "TIME UP" message
C $9BE6,3 Jump to $9C50 if time_up_state was 3 - show "CONTINUE" message & run countdown
C $9BE9,2 Return if it was 4 - countdown elapsed
N $9BEB Otherwise it's zero?
C $9BEB,1 A = time_bcd
C $9BEC,1 Set flags
C $9BED,2 Jump to tick_time_remaining if non-zero
N $9BEF Out of time.
C $9BEF,4 time_up_state = 1
C $9BF3,5 user_input_mask = $93 (allow Quit, Fire, Left, Right) [Q: Why left and right?]
C $9BF8,1 Return
@ $9BF9 label=tick_time_remaining
C $9BF9,2 Decrement the 1/16th sec counter
C $9BFB,1 Return if not zero
C $9BFC,2 Reset time_sixteenths to 15
C $9BFE,6 Decrement time_bcd [POKE $9C01 for Infinite time]
C $9C04,3 Return if <> 15s remain
C $9C07,3 Nancy berating us running out of time message
C $9C0A,3 Exit via chatter
C $9C0D,4 Is time_bcd zero? Jump to time_up if so
C $9C11,4 time_up_state = 0
C $9C15,4 user_input_mask = $FF (allow all keys)
C $9C19,2 Loop back to $9BF9
@ $9C1B label=tick_time_up
C $9C1B,3 Point at "TIME UP" message
C $9C1E,3 Call message_printing_related
C $9C21,6 Return if speed > 0
C $9C27,5 time_up_state = 2
C $9C2C,2 A = 4
C $9C2E,3 Exit via unknown_hook_3
C $9C31,5 Return if transition_control is non-zero
C $9C36,8 Exit via quit_key if no credits remain
C $9C3E,1 Decrement credits [POKE $9C3E for Infinite credits]
C $9C3F,5 Turn it into ASCII and poke it into the "CREDIT x" string
C $9C44,5 time_up_state = 3
C $9C49,3 HL = $0115
C $9C4C,3 Self modify 'LD HL' at $9C84 to load HL
C $9C4F,1 Return
C $9C50,5 Is fire pressed?
C $9C55,2 Jump if not (?)
N $9C57 Resetting mission code.
C $9C57,4 Reset time_up_state to zero
C $9C5B,5 Reset smash_factor to zero
C $9C60,2 Reset smash_counter to zero
C $9C62,5 Set user input mask to allow everything through
C $9C67,3 $A252 = 3
C $9C6A,3 transition_control = 3 -- Flag set to zero when attributes have been set
C $9C6D,3 turbos = 3
C $9C70,9 Set remaining time to 60 (BCD) [doesn't affect stuff if altered?!]
N $9C79 This entry point is used by the routines at #R$858C and #R$F220.
C $9C7B,3 }
@ $9C7E label=tick_print_continue
C $9C7E,3 Print "CONTINUE THIS MISSION" messages
C $9C81,3 Call message_printing_related
C $9C84,3 <self modified>
C $9C87,1 H--
C $9C8A,2 H = 6
C $9C8C,1 L--
C $9C8D,1 A = L
C $9C8F,3 BC = $0801  -- 8 => bip
C $9C94,1 B++  -- 9 => bow
C $9C95,3 Call start_sfx
C $9C98,1 A = L
C $9C99,1 Set flags
C $9C9C,1 A++
C $9C9D,3 *$A26A = A
C $9CA0,5 time_up_state = 4
C $9CA5,3 Self modify 'LD HL' at $9C84 to load HL
C $9CA8,1 A = L
C $9CA9,2 A >>= 1
C $9CAB,3 DE = $8D7C
C $9CAE,2 CP 10
C $9CB2,2 A = $31
C $9CB4,2 L = $00
C $9CB8,1 L = A
C $9CB9,2 A = $20
C $9CBB,1 *DE = A
C $9CBC,3 A = L + $B0
C $9CBF,1 DE++
C $9CC0,1 *DE = A
C $9CC1,1 Return
c $9CC2 Increments score in proportion to current speed
D $9CC2 Used by the routine at #R$8401.
@ $9CC2 label=speed_score
C $9CC2,3 Fetch current speed into #REGhl
C $9CC5,1 Speed low byte
C $9CC6,2 H >>= 1 but pointless?
C $9CC8,1 A <<= 1
C $9CC9,4 A >>= 2
C $9CCD,3 Pointless?
C $9CD0,1 BCD correction
C $9CD1,3 DE = 0
C $9CD4,2 Exit via increment_score
c $9CD6 Routine at 9CD6
D $9CD6 Used by the routines at #R$9D2E, #R$A637 and #R$B9F4.
C $9CD6,1 B=A
C $9CD7,3 "HI"
C $9CDA,2 C=255
C $9CDC,1 A=B
C $9CDD,3 Call j_9cfc
C $9CE0,2 terminate string?
C $9CE2,1 A=B
C $9CE6,1 A=E
C $9CEA,1 A=E
C $9CEE,1 A=D
C $9CF2,1 A=D
@ $9CF8 label=j_9cf8
C $9CF8,4 A ROR 4
@ $9CFC label=j_9cfc
C $9CFC,2 A &= 15
C $9CFE,2 if A is not zero goto 9D07
C $9D00,2 top bit set?
C $9D02,2 jump if so
C $9D04,1 where's the matching PUSH? or are we just discarding the RET addr?
@ $9D07 label=j_9d07
C $9D07,2 C = 0
C $9D09,2 A += 48
C $9D0B,1 HL--
C $9D0C,1 *HL = A
C $9D0D,1 Return
C $9D0E,3 Self modify the LD HL at $9D9B
C $9D11,5 Set the bonus_flag
C $9D16,1 A = B
c $9D17 Increments the score by (D,E,A).
D $9D17 Used by the routines at #R$8A57 and #R$9CC2.
R $9D17 I:A Low byte of increment
R $9D17 I:E Middle byte of increment
R $9D17 I:D High byte of increment
@ $9D17 label=increment_score
C $9D17,3 HL = &score_bcd
C $9D1A,1 A += *HL
C $9D1B,1 BCD correct A
C $9D1C,2 *HL++ = A
C $9D1E,2 A = E + *HL + carry
C $9D20,1 BCD correct A
C $9D21,2 *HL++ = A
C $9D23,2 A = D + *HL + carry
C $9D25,1 BCD correct A
C $9D26,2 *HL++ = A
C $9D28,3 A = 0 + *HL + carry
C $9D2B,1 BCD correct A
C $9D2C,1 *HL = A
C $9D2D,1 Return
c $9D2E Calculate overtake bonus
D $9D2E Used by the routine at #R$8401.
R $9D2E I:A Iterations (prob number of overtakes?)
@ $9D2E label=calc_overtake_bonus
C $9D2E,5 If $A22B is zero then return
C $9D33,1 B = A -- iterations
C $9D34,3 Point #REGhl at overtake_bonus
@ $9D37 label=cob_overtake_loop
C $9D37,3 A = *HL + 2  [increment bonus by 2 up to a max of 128]
C $9D3A,1 BCD correction
C $9D3B,6 If A >= 128 then A = 128  [80 actual points?]
C $9D41,1 *HL = A
C $9D42,1 Bank
C $9D43,1 E = A -- turn score into 00nn00
C $9D44,1 A = 0
C $9D45,1 D = 0
C $9D49,1 Unbank
C $9D4A,2 B--, goto cob_overtake_loop
C $9D4C,4 $A22B = 0
C $9D50,1 Return
c $9D51 Update scoreboard and flashing lights
B $9D51,6,6
@ $9D57 label=hi
T $9D57,2,1:n1 "HI"
@ $9D59 label=lo
T $9D59,2,1:n1 "LO"
@ $9D5B label=stage
T $9D5B,6,6 "STAGE " message shown in the score area.
@ $9D61 label=stage_n
B $9D61,1,1 #R$9D61 writes the current score number here in ASCII.
N $9D62 This entry point is used by the routines at #R$8401, #R$873C and #R$87DC.
@ $9D62 label=update_scoreboard
C $9D62,9 Check if stage has changed?
C $9D6B,2 Adding 48 to make it a digit then $80 to terminate the string.
C $9D6D,6 Update "STAGE N"
C $9D73,3 Screen pixel (48,36)
C $9D76,3 Point at stage text
C $9D79,3 Draw it
C $9D7C,6 Is the bonus flag set?
C $9D82,4 Clear it
C $9D86,3 Screen pixel (64,24)
C $9D89,21 Clear the area 5*7 bytes
C $9D9E,22 Draw it
C $9DB4,4 42 => BRIGHT + red over black
C $9DB8,2 46 => BRIGHT + yellow over black
C $9DBA,3 Point at attributes (8,3)
C $9DBD,6 Set them to #REGc
C $9DCD,3 Screen pixel (118,36)
C $9DD0,6 "LO"
C $9DD6,3 "HI"
C $9DD9,13 Draw it
C $9DE6,3 Point #REGhl at left light's attributes
C $9DE9,3 Toggle its brightness
C $9DEC,3 Point #REGhl at right light's attributes
C $9DEF,5 Toggle its brightness
c $9DF4 Toggle the light's BRIGHT bit (#REGhl -> attrs)
D $9DF4 Used by the routines at #R$9D51 and #R$B4CC.
@ $9DF4 label=toggle_light_brightness
C $9DF4,3 B = 4, C = $40 (BRIGHT bit)
C $9DF7,19 Toggle attribute byte on five successive locations
C $9E0A,4 Move to next attribute row
C $9E0E,3 Repeat for four rows
c $9E11 Draws the turbo sprites and updates the displayed scores.
D $9E11 Used by the routine at #R$9D51.
@ $9E11 label=plot_turbos_and_scores
C $9E11,6 If no turbo boosts remain jump to plot_scores_only
C $9E17,1 Preserve number of turbo boosts in #REGc
C $9E18,4 Read boost time remaining and set flags
C $9E1C,3 Point #REGhl at base of turbo sprites
C $9E1F,2 Avoid frame increment and address calculation if possible
C $9E21,8 Get frame number (0, 1 or 2)
C $9E29,3 Self modify frame number
C $9E2C,2 Avoid frame address calculation if possible
C $9E2E,3 56 bytes per frame
C $9E31,4 Calculate the frame address
@ $9E35 label=ptas_turbo_setup
C $9E35,3 Self modify $9E44 to load #REGsp with address of frame
C $9E38,4 Self modify $9E79 to restore #REGsp once drawing is done
C $9E3C,2 Low byte of back buffer draw address
N $9E3E Draw a whole frame of turbo sprite.
N $9E3E Note that the frame data is stored in memory inverted (bottom first), so we draw the bottom row first and then proceed upwards.
@ $9E3E label=ptas_turbo_loops
C $9E3E,3 Base address of frames
C $9E41,1 Temporarily decrement the number of turbos remaining to draw
C $9E42,2 All but the final turbo sprite use the zeroth frame
C $9E44,3 #REGsp is pointed at frame data (self modified)
C $9E47,1 Restore number of turbos
C $9E48,3 Form back buffer position
C $9E4B,1 Preserve partial back buffer position
C $9E4C,2 Height of frame
N $9E4E Draw a scanline of frame.
@ $9E4E label=ptas_turbo_scanline
C $9E4E,1 Pop some frame data off the stack (E is mask data, D is bitmap data)
C $9E4F,4 Write to screen: Screen = (Screen AND Mask) OR Bitmap
C $9E53,1 Advance screen address to next column
C $9E54,5 (Repeat)
C $9E59,1 Step back
C $9E5A,22 Move up a row (standard pattern)
@ $9E70 label=plot_turbo_continue
C $9E70,2 Loop until out of rows
C $9E72,1 Restore back buffer draw address
C $9E73,2 Advance screen address to next turbo position
C $9E75,1 Decrement number of turbos
C $9E76,2 Loop until none are left
C $9E78,3 Restore #REGsp
@ $9E7B label=plot_speed_only
C $9E7B,3 Point #REGde at speed digits screen position (144, 9)
C $9E7E,1 Bank
C $9E7F,4 Load speed into #REGde
N $9E83 Scale internal speed (0..511) to displayed speed by multiplying by 82 then discarding the bottom two digits.
C $9E83,3 Initialise counter
C $9E86,2 Do 7 digits / iterations
C $9E88,2 Multiplier of 82 (a percentage: speed * multiplier / 100 = displayed speed)
C $9E8A,1 Shift one bit out
C $9E8B,2 Jump if not adding
C $9E8D,1 Add
@ $9E8E label=ptas_speed_multiply_loop
C $9E8E,1 Double
C $9E8F,2 Loop
N $9E91 Count 10,000s.
C $9E91,3 10000
C $9E94,3 Initialise #REGd and #REGe counters to -1
C $9E97,1 ?Clear carry flag?
@ $9E98 label=ptas_speed_10000s_loop
C $9E98,1 Increment counter
C $9E99,2 Decrease total by 10,000
C $9E9B,2 Loop until #REGhl goes negative
C $9E9D,1 Correct for overshoot
C $9E9E,1 ?Clear carry flag?
N $9E9F Count 1,000s.
C $9E9F,3 1000
@ $9EA2 label=plot_speed_1000s_loop
C $9EA2,1 Increment counter
C $9EA3,2 Decrease total by 1,000
C $9EA5,2 Loop until total goes negative
C $9EA7,1 Correct for overshoot
N $9EA8 Count 100s.
C $9EA8,1 Initialise counter (to zero not -1 as in previous cases)
C $9EA9,3 100
@ $9EAC label=ptas_speed_100s_loop
C $9EAC,1 Increment counter
C $9EAD,2 Decrease total by 100
C $9EAF,2 Loop until total goes negative
C $9EB1,1 Correct for starting early
N $9EB2 Plot speed digits.
N $9EB2 We divide by 100 here by throwing away the bottom two digits.
C $9EB2,1 Stack 100s counter
C $9EB3,2 Stack 1,000s counter
C $9EB5,1 Get 10,000s counter
C $9EB6,4 Plot a digit for 10,000s
C $9EBA,4 Plot a digit for 1,000s
C $9EBE,3 Plot a digit for 100s
N $9EC1 Time.
C $9EC1,3 Point #REGde at time digits screen position (120, 9)
C $9EC4,1 Bank
C $9EC5,3 Point #REGde at time_bcd (one BCD byte)
C $9EC8,3 Point #REGhl at time_digits + 1
C $9ECB,2 One pair of digits
C $9ECD,3 Call plot_led_digits
N $9ED0 Distance.
C $9ED0,3 Point #REGde at distance_bcd + 1 (the second of two BCD bytes)
C $9ED3,4 L = Distance byte from first 'hazard' (the perp)
C $9ED7,4 H = 17th byte from first hazard (not sure yet how they relate)
N $9EDB Count 1000s (no loop here - it's not required)
C $9EDB,3 1,000
C $9EDE,2 Decrease total by 1,000
C $9EE0,2 BCD "10"
C $9EE2,2 Jump if distance >= 1,000
C $9EE4,1 Otherwise correct for overshoot
C $9EE5,1 BCD "00"
N $9EE6 Count 100s
C $9EE6,3 100
@ $9EE9 label=ptas_scores_distance_100s_loop
C $9EE9,1 Increment counter
C $9EEA,2 Decrease total by 100
C $9EEC,3 Loop until total goes negative
C $9EEF,1 Correct for overshoot
C $9EF0,1 Correct for starting early
C $9EF1,1 distance_bcd[1] = counter
N $9EF2 Count 10s
C $9EF2,2 10 (#REGb's already zero)
C $9EF4,1 ?Clear carry flag?
C $9EF5,2 #REGa = $F0 (BCD)
@ $9EF7 label=ptas_scores_distance_10s_loop
C $9EF7,2 Increment counter
C $9EF9,1 ?Clear carry flag?
C $9EFA,2 Decrease total by 10
C $9EFC,3 Loop until total goes negative
C $9EFF,1 Correct for overshoot
C $9F00,1 OR in the remainder
C $9F01,2 distance_bcd[0] = #REGa
C $9F03,3 Point #REGde at distance digits screen position (136,33)
C $9F06,1 Bank
C $9F07,3 Point #REGde at distance_bcd + 1 (two BCD bytes)
C $9F0A,3 Point #REGhl at distance_digits + 3
C $9F0D,2 Two pairs of digits (4 digits)
C $9F0F,3 Call ptas_led_digits
C $9F12,3 Point #REGde at score digits screen position
C $9F15,1 Bank
C $9F16,3 Point #REGde at score (four BCD bytes)
C $9F19,3 Point #REGhl at score digits (eight bytes)
C $9F1C,2 B = 4
N $9F1E Plots scoreboard digits (#REGde -> BCD) only if different than recorded values (#REGhl -> byte per digit). #REGb is the count.
@ $9F1E label=ptas_led_digits
C $9F1E,1 Read a pair of digits (BCD)
C $9F1F,1 Save digits for later
C $9F20,6 Unpack a BCD digit
C $9F26,3 If different than stored then plot
C $9F29,3 Move screen position
@ $9F2C label=ptas_led_next_half
C $9F2C,1 Move to next recorded value
C $9F2D,3 Examine next digit
C $9F30,3 If different than stored then plot
C $9F33,3 Move screen position
@ $9F36 label=ptas_led_next_whole
C $9F36,1 Move to next recorded value
C $9F37,1 Move to next digits
C $9F38,2 Loop until B is zero
C $9F3A,1 Return
@ $9F3B label=ptas_led_plot_1st
C $9F3B,1 Update drawn digit
C $9F3C,3 Plot the digit
C $9F3F,2 Jump back to handle next digit (second half of a pair)
@ $9F41 label=ptas_led_plot_2nd
C $9F41,1 Update drawn digit
C $9F42,3 Plot the digit
C $9F45,2 Jump back to handle hext digit (next whole pair)
c $9F47 Plots an 8x15 LED font digit
D $9F47 This appears to be set up to work for Y coordinates of 1, 9, 17, ...
R $9F47 I:A Glyph ID (0..9)
R $9F47 I:DE Address of (real) screen location
@ $9F47 label=ledfont_plot
C $9F47,1 Bank
C $9F48,13 #REGhl = &ledfont[glyphID * 15]
C $9F55,1 Save the screen address
C $9F56,1 Save #REGd
C $9F57,2 Transfer a byte
C $9F59,1 Next row down
C $9F5A,1 Undo LDI's increment
C $9F5B,22 Transfer six more rows
C $9F71,2 Restore #REGd and move to an even line
C $9F73,4 Move to the next screen row (group of 8)
C $9F77,31 Transfer eight more rows
C $9F96,1 Move to next column
C $9F97,1 Bank
C $9F98,1 Return
c $9F99 Another draw string entry point?
D $9F99 Used by the routine at #R$8E6C.
C $9F9A,1 Bank
C $9F9B,1 HL' = BC   so BC is ptr to text
C $9F9C,3 DE = 32
C $9F9F,1 C = A
C $9FA0,1 Bank
C $9FA1,2 goto draw_string_entry
N $9FA3 The string is terminated by setting the topmost bit of the final character.
@ $9FA3 label=draw_string
C $9FA3,3 A' = 1  Set drawing type (single height, plots to real screen)
@ $9FA6 label=draw_string_entry
C $9FA6,8 Load a byte and mask off the text part
C $9FAE,6 Was bit 7 set?, quit if so, otherwise loop
c $9FB4 Draws a character (to buffer or screen?)
D $9FB4 Used by the routine at #R$9F99.
R $9FB4 I:A Character to draw (ASCII)
R $9FB4 I:A' Rendering type (double height, invert, etc.)
R $9FB4 I:HL ...
R $9FB4 I:DE Screen address
R $9FB4 O:HL ...
R $9FB4 O:DE Screen address moved to next column
@ $9FB4 label=draw_char
C $9FB4,4 If it's not a space character, goto dc_not_space with #REGa reduced
N $9FB8 It's a space
C $9FB8,1 Move screen address to the next column
C $9FBA,1 <move something else that's banked>
C $9FBC,1 Return
N $9FBD Map ASCII to glyph IDs
@ $9FBD label=dc_not_space
C $9FBD,2 Offset = 18
C $9FBF,4 Jump if 'A' or above
C $9FC3,2 Offset = 11
C $9FC5,4 Jump if '0' or above
C $9FC9,2 Glyph ID = 0
C $9FCB,3 Jump if '!'
C $9FCE,1 Glyph ID = 1
C $9FCF,4 Jump if '('
C $9FD3,1 Glyph ID = 2
C $9FD4,3 Jump if ')'
C $9FD7,1 Glyph ID = 3
C $9FD8,4 Jump if ','
C $9FDC,1 Glyph ID = 4
C $9FDD,2 Anything else is '.'
N $9FDF #REGa is an ASCII character - 32
@ $9FDF label=dc_have_range
C $9FDF,2 Convert (ASCII - 32) to glyph ID. #REGc = #REGa - Offset
N $9FE1 #REGc is a glyph ID
@ $9FE1 label=dc_have_single
C $9FE1,8 Multiply the glyph ID by 7
C $9FE9,2 Add in the carry
C $9FEB,1 #REGbc is now the byte offset
C $9FEC,3 Point #REGhl at 8x7 font
C $9FEF,1 Point to glyph data
C $9FF0,3 Get type in #REGc (passed in #REGa' on entry)
C $9FF3,4 If 1 then jump $A0AD
C $9FF7,5 If 2 then jump $A083
C $9FFC,3 If 3 then jump $A03D -- double height
C $9FFF,3 If 4 then jump $A031 -- single height inverted
C $A002,2 If 5 then jump $A023 -- double height + inverted
N $A004 Otherwise it's type 6
C $A005,2 B = 4
@ $A007 label=dc_loop1
C $A007,1 A = *HL
C $A008,1 *DE = A
C $A009,1 Move to next row
C $A00A,1 *DE = A
C $A00B,1 Move to next row
C $A00C,1 Move to next glyph row
C $A00D,2 While iterations remain, goto loop1
C $A00F,4 D -= 8
C $A013,4 E += 32
C $A017,2 B = 3
@ $A019 label=dc_loop2
C $A019,1 A = *HL
C $A01A,1 *DE = A
C $A01B,1 Move to next row
C $A01C,1 *DE = A
C $A01D,1 Move to next row
C $A01E,1 Move to next glyph row
C $A01F,2 While iterations remain, goto loop2
C $A024,2 B = 7
@ $A026 label=dc_loop3
C $A026,2 A = ~*HL
C $A028,1 *DE = A
C $A029,1 Move to next row
C $A02A,1 *DE = A
C $A02B,1 Move to next row
C $A02C,1 Move to next glyph row
C $A02D,2 While iterations remain, goto loop3
C $A032,2 B = 7
@ $A034 label=dc_loop4
C $A034,3 *DE = ~*HL
C $A037,1 Move to next glyph row
C $A038,1 Move to next row
C $A039,2 While iterations remain, goto loop4
N $A03D Plots double-height glyphs. DE->screen HL->glyph def
@ $A03D label=dc_double_height_glyph
C $A03E,2 *DE = 0
C $A040,1 D++
C $A041,2 *DE = *HL
C $A043,1 D++ -- next row
C $A046,1 E--
C $A047,1 D++
C $A048,42 Repeat six times
C $A072,1 A = 0
C $A073,1 *DE = A
@ $A074 label=dc_a074
C $A075,1 E++
C $A078,3 *HL |= C
C $A07B,1 HL += DE
C $A07C,3 *HL |= C
C $A080,1 L++
C $A082,1 Return
C $A084,1 A = 0
C $A085,1 *DE = A
C $A086,1 D++
C $A089,1 E--
C $A08A,1 D++
C $A08D,1 E--
C $A08E,1 D++
C $A091,1 E--
C $A092,1 D++
C $A095,1 E--
C $A096,1 D++
C $A099,1 E--
C $A09A,1 D++
C $A09D,1 E--
C $A09E,1 D++
C $A0A1,1 E--
C $A0A2,1 D++
C $A0A3,1 *DE = A
@ $A0A4 label=dc_a0a4
C $A0A5,1 E++
C $A0A7,3 *HL |= C
C $A0AA,1 L++
C $A0AC,1 Return
C $A0AD,1 Save screen pointer
C $A0AE,2 7 iterations/rows
@ $A0B0 label=dc_loop_a0b0
C $A0B0,2 Transfer a byte
C $A0B2,1 Move to next scanline (Y0++)
C $A0B3,1 Move to next row of glyph data
C $A0B4,3 Did we rollover?
C $A0B7,2 If no rollover goto loop
C $A0B9,4 Undo rollover
C $A0BD,4 Y3++
C $A0C1,2 If no rollover goto loop
C $A0C3,4 There was rollover, so move it to Y6
C $A0C7,2 Loop until we've done 7 lines
C $A0C9,1 Restore screen pointer
C $A0CA,1 Move to next column
C $A0CB,1 Return
c $A0CC Keyscan
@ $A0CC label=kempston_flag
B $A0CC,1,1 Set to 1 if Kempston joystick is chosen, 0 otherwise
@ $A0CD label=keydefs_probably
B $A0CD,8,8 must be keydefs
@ $A0D5 label=user_input
B $A0D5,1,1 User input: QPBFUDLR - Quit Pause Boost Fire Up Down Left Right. Note that attract mode is driven through this var.
N $A0D6 This entry point is used by the routines at #R$8014, #R$8258, #R$8401, #R$858C, #R$8876 and #R$F220.
@ $A0D6 label=keyscan
C $A0D6,6 If not Kempston then goto $A0F3
C $A0DC,4 Read Kempston joystick port. Returns 000FUDLR active high
@ $A0F3 label=ks_keyboard
@ $A0FB label=ks_common
C $A111,1 Return
C $A112,1 Outer keyboard loop
C $A117,1 invert carry
C $A11A,2 loop while top bit set?
C $A11D,1 Return
@ $A11E label=keyscan_inner
C $A11E,1 Inner keyboard loop.... what's in #REGa?
C $A11F,14 B=(A&7)+1, C=5-(A>>3)
C $A12D,5 A = $FE ROR B
C $A132,2 Port $FE is the AND of all columns/rows? This is the main keyboard access
C $A134,4 C = A ROR C
C $A138,1 Return
g $A139 Game status buffer entry at A139
B $A139,1,1 This is always zero, yet the code checks it. If it's set then $83F5 uses it to jump to $81AA, which is the end of a string. $8435 tests it too and calls <chatter> when it's zero, which it always is. $8AD6 also tries to use it to jump into a string (twice). Could be dead code or 128K version hook?
@ $A13A label=current_stage_number
B $A13A,1,1 Current stage number
B $A13B,1,1 Used by $8425  -- seems to start at 4 then cycle 3/2/1 with each restart of the game, another random factor?
@ $A13C label=overtake_bonus
B $A13C,1,1 Overtake combo bonus counter. BCD. This increases by 2 for each overtake and is reset on a crash.
@ $A13D label=credits
B $A13D,1,1 Number of credits remaining (2 for a new game)
B $A13E,47,8*5,7 Canned data or Data restored for attract mode?
B $A16D,1,1 Used by $BC36
@ $A16E label=idle_timer
B $A16E,1,1 Idle timer. Counts down from 100. Recommences whenever the hero car is stopped. When it hits zero Raymond will say "LET'S GET MOVIN' MAN!" and it will be reset to 100.
@ $A16F label=user_input_mask
B $A16F,1,1 User input mask, set to $C0 (Quit+Pause) when perp is fully smashed, or $FF otherwise
@ $A170 label=turbos
B $A170,1,1 Number of turbo boosts remaining (3 for a new game)
W $A171,2,2 seems to be the horizon level, possibly relative (used during attract mode)
@ $A173 label=mystery_A173
B $A173,1,1 This is decremented by $8BC6 but nothing else seems to read it.
B $A174,1,1 Low/high gear flag?
@ $A175 label=score_digits
B $A175,8,8 Score digits. One digit per byte, least significant first. This seems to be recording what's on screen so digit plotting can be bypassed.
@ $A17D label=time_sixteenths
B $A17D,1,1 Seems to be a 1/16ths second counter. Counts from $F to $0. $A17E is decremented when it hits zero.
@ $A17E label=time_bcd
B $A17E,1,1 Time remaining. Stored as BCD.
@ $A17F label=time_digits
B $A17F,2,2 Time remaining as displayed. Stored as one digit per byte.
@ $A181 label=distance_digits
B $A181,4,4 Distance as displayed. Stored as one digit per byte.
@ $A185 label=no_objects_counter
B $A185,1,1 If set causes no objects or hazards to be emitted.
@ $A186 label=horizon_attribute
W $A186,2,2 Attribute address of horizon. Points to last attribute on the line which shows the ground. (e.g. $59DF)
N $A188 +0 is a used flag, either $00 or $FF +1 looks distance related +2/3/4/5/6 TBD +7 byte  TBD used by hazard_hit +8 byte  gets copied from the hazards table +9 word  address of e.g. car lod +11 word  address of routine +13 word  set to $190 by fully_smashed (likely a horizontal position) +15 byte  TBD used by hazard_hit, counter which gets set to 2 then reduced +17 byte  TBD used by hazard_hit, indexes table $ACDB +18 byte  TBD used by hazard_hit +19 byte TBD used by hazard_hit
@ $A188 label=hazards
B $A188,120,8 Set by $843B. Groups of 20 bytes. This area looks like a table of spawned vehicles or objects. The first entry is the perp.
B $A200,32,8
B $A220,1,1 #R$8014 sets this to the level number that it's going to load [but I don't see it using it again]. #R$858C sets it to $F8. #R$BC3E uses it to avoid some work.
B $A221,1,1 $ABCE, $AD0D reads  I don't yet see what writes it.
B $A222,1,1 $8F9A reads  $ADA0, $AE83, $AF41 writes
B $A223,1,1 Stage number as shown. Stored as ASCII.
@ $A224 label=helicopter_control
B $A224,1,1 Set to 1 -> helicopter moves out to the left, gets set to zero. Set to 3/4 -> Helicopter moves in from left, gets set to five. $AAC6, $AB33 reads  $AB96, $C013 writes
@ $A225 label=stop_car_spawning
B $A225,1,1 Inhibits cars from spawning.
@ $A226 label=correct_fork
B $A226,1,1 Holds the correct direction to take at forks (1 => left, 2 => right).
@ $A227 label=floating_arrow
B $A227,1,1 Shows the floating left/right arrow (0 => off, 1 => left, 2 => right).
B $A228,1,1 $B421 reads  $B4C8 writes
@ $A229 label=time_up_state
B $A229,1,1 1 => out of time, 2 => "TIME UP" message is printed; 3 => "CONTINUE THIS MISSION" message is printed and a countdown runs; 4 => countdown elapsed; 0 otherwise
B $A22A,1,1 Used by $B6D6 and others
B $A22B,1,1 Used by $9D2E and others. Stops overtake bonus working.
@ $A22C label=bonus_flag
B $A22C,1,1 Bonus flag (1 triggers the effect)
@ $A22D label=bonus_counter
B $A22D,1,1 Counter for bonus effect (goes up to 7?)
@ $A22E label=sighted_flag
B $A22E,1,1 Set to 1 when the perp has been sighted. Enables flashing lights and the smash bar.
B $A22F,1,1 Set to 2 by fully_smashed
@ $A230 label=perp_caught_stage
B $A230,1,1 Set to > 0 by fully_smashed when the perp has been caught. #R$8A57 progresses this through stages 1..6 until the cars are slowed to a halt and the bonuses are printed. Used by draw_screen. Seems to inhibit car spawning too.
@ $A231 label=transition_control
B $A231,1,1 Transition control/counter. Set to zero when fill_attributes has run. Set to 4 while transitioning. Also 2 sometimes. Set to 1 when the perp has been caught and we're drawing mugshots.
@ $A232 label=smash_factor
B $A232,1,1 Set to 0..6 if (0..3, 4..6, 7..10, 11..13, 14..16, 17+) smashes
@ $A233 label=smash_counter
B $A233,1,1 Smash counter (0..20)
@ $A234 label=counter_A
B $A234,1,1 Cycles 0-1-2-3 as the game runs (used to animate turbo smoke CHECK)
@ $A235 label=counter_B
B $A235,1,1 Cycles 0-1 as the game runs (used to toggle flash the red/blue lights)
@ $A236 label=counter_C
B $A236,1,1 Cycles 0-1-2-3 as the game runs - at half rate
N $A237 These seem to get altered even when no sound is being produced.
@ $A237 label=sfx_0
B $A237,1,1 Used by $88F2
@ $A238 label=sfx_1
B $A238,1,1 Used by $88F2
B $A239,1,1 Used by $F265
B $A23A,1,1 Used by $F2F6
@ $A23B label=tunnel_sfx
B $A23B,1,1 Set to 5 when we're in a tunnel. Used to modulate sfx.
B $A23C,1,1 $8909, $BE28 reads  $A3FA, $BDFF, $BE2C writes
B $A23D,1,1 $BE33 reads  $890F, $A3D2, $BDFC, $BE37 writes
@ $A23E label=off_road
B $A23E,1,1 0 => Fully on-road, 1 => One wheel off-road, 2 => Both wheels off-road [samples: $B104, $B40C reads  $A3FD, $A52A, $B07D, $B322, $B404, $B443 writes]
@ $A23F label=fast_counter
B $A23F,1,1 this seems to be a very fast counter related to level position
@ $A240 label=road_buffer_offset
W $A240,2,2 Current offset (in bytes) into the road buffer at $EE00..$EEFF. Much of the code treats this as a byte. Used by $B8F6, $BBF7, $87E0.
@ $A242 label=curvature_byte
B $A242,1,1 Current curvature byte (minus 16) [$BE3E reads $BC17,$BE7D writes]
@ $A243 label=height_byte
B $A243,1,1 Current height byte (minus 16) [$BE90 reads $BC1A,$BECF writes]
@ $A244 label=leftside_byte
B $A244,1,1 [$BF9E reads $BC1D,$BFDD writes]
@ $A245 label=rightside_byte
B $A245,1,1 Current rightside byte [$BF55 reads $BC20,$BF94 writes]
@ $A246 label=hazards_byte
B $A246,1,1 Current hazards byte (minus one) [$BFE7 reads $BC23,$C055 writes]
@ $A247 label=lanes_counter_byte
B $A247,1,1 [$BEDB reads $BC26,$BF15,$BF29 writes]
B $A248,1,1 [$A955 reads $C457,$C526 writes]
@ $A249 label=fork_taken
B $A249,1,1 Set to 0 if no fork, or the left fork was taken, or 1 if the right fork was taken. [$A539,$B988,$BAE0,$BB6E reads $BA89,$BC2C writes]
@ $A24A label=speed
W $A24A,2,2 Speed (0..511). Max when in lo gear =~ $E6, hi gear =~ $168, turbo =~ $1FF.
B $A24C,1,1 $B1B7 reads  $B1E4 writes
@ $A24D label=cornering
B $A24D,1,1 Likely a cornering force flag. Used to trigger smoke. $B3B4, $B432 reads  $B2E5, $B314, $B32A writes
@ $A24E label=boost
B $A24E,1,1 Turbo boost time remaining (60..0)
@ $A24F label=smoke
B $A24F,1,1 Smoke time remaining. This is set to 4 on low-to-high gear changes and to 3 when the hero car lands after a jump. This isn't set for turbo boosts however.
@ $A250 label=turn_speed
B $A250,1,1 Turn speed (0/1/2) ignoring direction
@ $A251 label=flip
B $A251,1,1 1 => Horizontally flip the hero car, 0 => Don't
B $A252,1,1
@ $A253 label=gear
B $A253,1,1 0 => Low gear, 1 => High gear
@ $A254 label=another_spawning_flag
B $A254,1,1 Cleared by $884B   suspected spawning flag (different to flag $A225)
@ $A255 label=distance_bcd
B $A255,2,2 Distance as BCD (2 bytes / 4 digits, little endian)
B $A257,1,1 ...
B $A258,1,1 Used by $B8D8
B $A259,1,1 Used by $B92B
B $A25A,1,1 Used by $B8D2
B $A25B,1,1
B $A25C,1,1 Used by $B84E
B $A25D,1,1 Used by $B85D
B $A25E,1,1
W $A25F,2,2 Used by $B29A
B $A261,1,1 Used by $B297
B $A262,1,1
B $A263,1,1 Used by $B2AE
B $A264,1,1 Used by $B2B2 -- together might be a 16-bit qty
B $A265,1,1 Used by $BC2C -- set to $60 when the split road becomes visible, zero otherwise
B $A266,1,1 Used by $A3A6 -- counts down (from 16?) when the split road approaches, zero when the split actually starts
B $A267,1,1 Used by $BC39 -- as 16-bit
B $A268,1,1 Used by $BB69 - skips routine if not set
B $A269,1,1 Used by $BC29
B $A26A,1,1 Used by quit_key - quit in progress flag?
B $A26B,1,1 Used by $8432
@ $A26C label=road_pos
W $A26C,2,2 Road position of car. Left..Right = $1E2...$03A, $105 is centre.
@ $A26E label=road_curvature_ptr
W $A26E,2,2 Address of the previous curvature data byte
@ $A270 label=road_height_ptr
W $A270,2,2 Address of the previous height data byte
@ $A272 label=road_lanes_ptr
W $A272,2,2 Address of the previous lane data byte
@ $A274 label=road_rightside_ptr
W $A274,2,2 Address of the previous right object data byte
@ $A276 label=road_leftside_ptr
W $A276,2,2 Address of the previous left object data byte
@ $A278 label=road_hazard_ptr
W $A278,2,2 Address of the previous hazard object data byte
b $A27A Font: 8x7 bitmap
D $A27A #HTML[#CALL:graphic($A27A,8,41*7,0,0)]
B $A27A,7,7 Exclamation mark
B $A281,7,7 Open bracket
B $A288,7,7 Close bracket
B $A28F,7,7 Comma
B $A296,7,7 Full stop
B $A29D,70,7 0..9
B $A2E3,182,7 A-Z
c $A399 Checks for collisions.
D $A399 Used by the routines at #R$8401 and #R$852A.
@ $A399 label=check_collisions
C $A399,3 HL = $0048
C $A39C,3 DE = $01D8
C $A3A0,3 A = *$A265  -- fairly sure this is a split road flag
C $A3A3,1 Set flags
C $A3A4,2 Jump to cc_at_split if zero
C $A3A6,3 A = *$A266  -- counts down as split approaches
C $A3A9,1 Set flags
C $A3AA,3 Jump to cc_not_split if zero
C $A3AD,1 A--
C $A3AE,1 Return if zero  [can't collide immediately before split?]
N $A3AF Check left hand side of car.
@ $A3AF label=cc_at_split
C $A3AF,3 HL = *$EAFE
C $A3B2,1 A = H  (0 or 255)
C $A3B3,1 Set flags
C $A3B6,9 Jump to $A3D1 if HL < $40
C $A3BF,8 Jump to $A3D5 if HL < $6A
C $A3C7,4 HL -= $85
C $A3CB,1 A++  -- A == 1 => partially off-road
C $A3CC,2 Jump to cc_split_set if A was 255
C $A3CE,1 A++  -- A == 2 => fully off-road
C $A3CF,2 Jump to cc_split_set
C $A3D1,4 *$A23D = 0
N $A3D5 Check right hand side of car.
C $A3D5,3 HL = *$EAFC
C $A3D8,1 A = H
C $A3D9,1 Set flags
C $A3DA,2 A = 0
C $A3DE,9 Jump to $A3FA if HL >= $BE
C $A3E7,9 Jump to $A3FD if HL >= $8E
C $A3F0,4 HL -= $7C
C $A3F4,1 A++  -- A == 1 => partially off-road
C $A3F5,2 Jump to cc_split_set if no carry
C $A3F7,1 A++  -- A == 2 => fully off-road
C $A3F8,2 Jump to cc_split_set
C $A3FA,3 *$A23C = A
@ $A3FD label=cc_split_set
C $A3FD,3 off_road = A  -- 0/1/2 => on-road/one wheel off-road/both wheels off-road
C $A400,1 Set flags
C $A401,2 C = $00
C $A403,2 not off road?
C $A405,6 L = road_buffer_offset + 64
C $A40B,2 HL = $EE00 | L
C $A40D,1 A = *HL
C $A417,3 A = road_pos.hi
C $A41C,1 C = A
C $A41D,2 A = 20
C $A420,3 A = C & 1
C $A426,3 HL = $00D1
C $A429,3 DE = $0195
C $A42D,1 Set flags
C $A42E,2 C = 1
C $A432,1 C++
@ $A433 label=cc_hit_tunnel_wall
C $A434,3 BC = $0604
C $A437,3 Call start_sfx
C $A43B,1 A = C
C $A43C,3 ($B3DC) = A
C $A440,3 ($B396) = HL
C $A443,4 ($B3A4) = DE
C $A447,1 Set flags
C $A448,1 Return if non-zero
C $A449,5 A = road_buffer_offset + 96
C $A44E,1 L = A
C $A450,2 H = $EE
C $A452,3 A = road_buffer_offset
C $A456,1 A = *HL
C $A459,1 L++
C $A45A,1 A |= *HL
C $A45D,12 HL = $5CFA[A * 7]
C $A469,2 C = *HL++
C $A46B,2 E = *HL++
C $A46D,1 A = *HL
C $A46E,1 D = B
C $A46F,3 HL = *$EAFC
C $A473,2 HL -= BC
C $A478,2 HL -= DE
C $A47D,1 A = 0
C $A47E,2 Jump to cc_hit_scenery
C $A481,2 A += 32
C $A483,3 HL = $EE00 | A
C $A486,3 A = road_buffer_offset
C $A48A,1 A = *HL
C $A48D,1 L++
C $A48E,1 A |= *HL
C $A48F,1 Return if zero
C $A490,12 HL = $5D00[A * 7]
C $A49C,2 C = *HL++
C $A49E,2 E = *HL++
C $A4A0,1 A = *HL
C $A4A1,1 D = B  must be zero?
C $A4A2,3 HL = *$EAFE
C $A4A6,2 HL -= BC
C $A4A9,1 Return if carry
C $A4AA,2 HL -= DE
C $A4AC,1 Return if no carry
C $A4AE,2 A = $01
@ $A4B0 label=cc_hit_scenery
C $A4B1,3 BC = $0403      -- arrive here if drive into scenery, e.g. a tree
C $A4B4,3 Call start_sfx
N $A4B8 This entry point is used by the routines at #R$A637 and #R$A8CD.
C $A4B8,3 HL = &<crashed flag>
C $A4BB,2 [why inc then dec?]
C $A4BE,2 Set crashed flag
C $A4C0,4 *$B36F = A++
C $A4C4,5 *$B38E = A
C $A4C9,3 *$B385 = 5
C $A4CC,3 HL = speed
C $A4CF,1 Preserve HL
C $A4D0,2 H >>= 1
C $A4D2,1 A = L
C $A4D5,8 A = (A << 3) + 16
C $A4DD,2 L = 24
C $A4E2,1 L = A
C $A4E3,3 *$B357 = HL
C $A4E7,3 HL = A
C $A4EC,2 HL -= DE
C $A4F2,3 Self modify 'LD BC' at $B32E to load HL
C $A4F5,1 Return
@ $A4F6 label=cc_not_split
C $A4F6,3 HL = *$E8FE  [mystery var]
C $A4F9,1 A = H  (0 or 255)
C $A4FA,1 Set flags
C $A4FB,2 A isn't zero
C $A4FD,11 Jump to $A510 if HL < $6A
C $A508,2 HL -= $85
C $A50A,1 A++  -- A == 1 => partially off-road
C $A50B,2 Jump to cc_not_split_set if HL < $85
C $A50D,1 A++  -- A == 2 => fully off-road
C $A50E,2 Jump to cc_not_split_set
C $A510,3 HL = *$EDFC
C $A513,1 A = H
C $A514,1 Set flags
C $A515,2 A = 0
C $A517,2 JR NZ,cc_not_split_set
C $A519,9 Jump to cc_not_split_set if HL >= $8E
C $A522,4 HL -= $7C
C $A526,1 A++  -- A == 1 => partially off-road
C $A527,2 Jump to cc_not_split_set if HL >= $7C
C $A529,1 A++  -- A == 2 => fully off-road
@ $A52A label=cc_not_split_set
C $A52A,3 off_road = A
C $A52D,1 A = 0
C $A52E,3 *$B3DC = A
C $A532,3 *$B396 = HL
C $A535,4 *$B3A4 = DE
C $A539,6 Jump to $A55C if the right fork was taken
C $A53F,3 HL = *$5CFC
C $A542,2 C = *HL++
C $A544,2 E = *HL++
C $A546,1 A = *HL
C $A547,2 B = 0
C $A549,1 D = B
C $A54A,3 HL = *$EAFE
C $A54D,5 Return if HL >= BC
C $A552,3 Return if HL < DE
C $A555,2 A = $8C
C $A558,1 A = 0
C $A559,3 Jump to cc_hit_scenery
C $A55C,3 HL = *$5D02
C $A55F,2 C = *HL++
C $A561,2 E = *HL++
C $A563,2 B = 0
C $A565,1 D = B
C $A566,3 HL = *$EAFC
C $A569,5 Return if HL < BC
C $A56E,3 Return if HL >= DE
C $A571,2 A = $8C
C $A574,2 A = 1
C $A576,3 Jump to cc_hit_scenery
c $A579 Routine at A579
D $A579 Used by the routines at #R$8401, #R$852A and #R$873C.
@ $A579 label=main_loop_14
C $A579,3 HL = $E34F  -- somewhere in data block $E34B
C $A57C,2 B = 21  -- iterations
C $A57E,1 A = 0
@ $A57F label=ml14_loop1
C $A57F,1 A += *HL
C $A580,2 *HL++ = A
C $A582,2 Loop ml14_loop1 while B
C $A584,4 Self modify 'LD SP' at $A60A to restore SP
C $A588,3 SP = $EB00
C $A58B,8 DE = $EE00 | (road_buffer_offset + 64)
C $A593,4 IY = $E34F
C $A597,2 B = 21
C $A599,3 A = ($A265)  -- split road flag
C $A59C,1 Set flags
C $A59D,3 Jump to ml14_loop2 if zero [not in road split]
C $A5A0,3 A = ($A266)  -- split road countdown
C $A5A3,1 Set flags
C $A5A4,2 [not about to road split]
C $A5A6,1 B = A
@ $A5A7 label=ml14_loop2
C $A5A7,1 A = *DE
C $A5A9,1 E = A
C $A5AA,5 A = ~(IY[0] * 2)
C $A5AF,1 L = A
C $A5B0,3 A = E & 3
C $A5B5,2 H = $E8
C $A5B7,1 B = *HL
C $A5B8,1 L--
C $A5B9,1 C = *HL
C $A5BB,2 H = $EC
C $A5BD,2 Jump to ml14_a5da
C $A5BF,3 H = A + $E7
C $A5C2,1 B = *HL
C $A5C3,1 L--
C $A5C4,1 C = *HL
C $A5CF,2 A = 3
C $A5D3,2 A = 3
C $A5D7,1 A--
C $A5D8,2 H += A
@ $A5DA label=ml14_a5da
C $A5DA,1 C = *HL
C $A5DB,1 L++
C $A5DC,1 B = *HL
C $A5DF,2 IY++
C $A5E1,1 E++
C $A5E2,2 Loop to ml14_loop2 while B
C $A5E4,3 A = *$A266  -- split road countdown
C $A5E7,1 Set flags
C $A5E8,3 Jump to ml14_return if zero
C $A5EB,6 A = ~*$A266 + 22
C $A5F1,2 Jump to ml14_return if zero
C $A5F3,1 B = A
C $A5F4,2 H = $EB
@ $A5F6 label=ml14_loop3
C $A5F6,6 L = ~(IY[0] * 2)
C $A5FC,1 H--
C $A5FD,1 D = *HL
C $A5FE,1 L--
C $A5FF,1 E = *HL
C $A601,1 H++
C $A602,1 E = *HL
C $A603,1 L++
C $A604,1 D = *HL
C $A606,2 IY++
C $A608,2 Loop ml14_loop3 while B
@ $A60A label=ml14_return
C $A60A,3 Restore stack pointer -- Self modified above on entry
C $A60D,1 Return
c $A60E Counters
D $A60E Used by the routines at #R$8401 and #R$852A.
@ $A60E label=cycle_counters
C $A60E,8 Cycle $A234 0-1-2-3
C $A616,5 Cycle $A235 0-1
C $A61B,1 Return if it's zero
C $A61C,6 Cycle $A236 0-1-2-3 at half rate
C $A622,1 Return
b $A623 Hazard template?
D $A623 $A845 copies these 20 bytes to hazards array.
@ $A623 label=hazard_template
B $A623,1,1 Used flag
B $A624,8,8
W $A62C,2,2
W $A62E,2,2 Routine at $A8CD
W $A630,2,2
B $A632,5,5
c $A637 Smash handling.
D $A637 This gets called whenever the perp is within sight of the hero car.
@ $A637 label=smash_handler
C $A637,5 Return if perp_caught_stage > 0
C $A63C,7 Call $B4CC if sighted_flag is zero
C $A643,3 A = IX[7]
C $A646,1 Set flags
C $A64C,3 IX[7]++
C $A64F,1 Return if non-zero
C $A650,2 Preserve IY
C $A652,3 C = IX[1]
C $A655,2 B = 5  iterations
C $A657,4 IY = &hazards[1]
C $A65B,3 DE = 20  stride of hazards
@ $A65E label=loop_a65e
C $A65E,6 If the hazard is active then jump to $A66C
C $A664,2 Move to next hazard
C $A666,2 Loop while #REGb
C $A668,2 Restore IY
C $A66A,2 Jump to $A68F
C $A66C,3 A = IY[15]
C $A673,4 A = IY[1] - C
C $A67A,2 A += 2
C $A67E,2 A -= 3
C $A682,3 A = IY[17]
C $A685,3 CP IX[18]
C $A68F,2 A = 0
C $A691,1 Set flags
C $A694,3 A = IX[1]
C $A697,2 CP 7
C $A69B,2 A = 20  self modified below
C $A69D,1 A--
C $A6A0,1 C++
C $A6A1,3 Call rng
C $A6A6,1 C = A
C $A6A7,4 A = ($5D1B) + C
C $A6AB,3 *$A69C = A  -- Self modify LD A at $A69B
C $A6AE,3 HL = road_pos
C $A6B1,3 DE = $A4
C $A6B4,2 HL -= DE
C $A6B6,3 BC = $0404
C $A6B9,2 Jump to $A6CF if road_pos < $A4
C $A6BB,2 E = $46
C $A6BD,1 B--
C $A6BE,2 HL -= DE   not restored so includes prev subtraction
C $A6C2,1 B--
C $A6C3,1 C--
C $A6C4,2 HL -= DE
C $A6C8,1 B--
C $A6C9,1 C--
C $A6CA,2 HL -= DE
C $A6CE,1 C--
C $A6CF,3 A = IX[18]
C $A6D8,3 C = IX[18]
C $A6DB,3 Call rng
C $A6DE,1 C++
C $A6DF,1 Set flags
C $A6E3,2 C -= 2
C $A6E5,1 A = C
C $A6E6,1 Set flags
C $A6E7,2 C = 2
C $A6F0,2 C = $FE
C $A6F2,1 A += C
C $A6F3,3 IX[18] = A
C $A6F6,3 C = IX[1]
C $A6F9,3 Call get_spawn_lanes
C $A6FC,3 A = IX[18]
C $A702,2 A += 2
C $A704,3 IX[18] = A
C $A708,4 Jump to $A711 if A <= C
C $A70C,2 A -= 2
C $A70E,3 IX[18] = A
C $A711,3 A = IX[18]
C $A714,3 HL = $A7E6  -> $A7E7 data block
C $A717,1 A += L
C $A718,1 L = A
C $A719,3 A = IX[5]
C $A71C,1 CP *HL
C $A71D,2 C = 1
C $A723,2 A -= 10
C $A727,1 CP *HL
C $A72A,1 C--
C $A72B,1 A = *HL
C $A72E,2 A += 10
C $A732,1 CP *HL
C $A735,1 C--
C $A736,1 A = *HL
C $A737,3 IX[5] = A
C $A73A,4 Self modify 'LD A' at $A68F to load C
C $A73E,2 A = 0
C $A740,1 Set flags
C $A741,3 DE = $1E
C $A744,3 HL = $E6
C $A749,2 A = <...>  -- Self modified below
C $A74B,1 A--
C $A74C,3 Self modify 'LD A' at $A749 to load A
C $A751,1 Preserve HL
C $A752,3 Call rng
C $A755,1 Restore HL
C $A756,3 C = A & 15  mask random value
C $A759,4 A = ($5D1C) + C   value in $5D1A data block
C $A75D,3 Self modify 'LD A' at $A749 to load A
C $A760,2 A = 10
C $A762,1 A--
C $A763,3 ($A73F) = A
C $A768,3 A = IX[1]
C $A76B,2 CP 13
C $A76F,4 B = ~A + 14 -- iterations
@ $A773 label=loop_a773
C $A773,1 HL += DE
C $A774,2 Loop to loop_a773 while B
C $A776,5 A = IX[1] - 6
C $A77D,5 A = (A + 5) * 8
C $A782,1 HL += DE
C $A783,6 wordat(IX + 13) = HL
C $A789,1 Return
C $A78A,4 IX[7] = $FC
C $A793,2 A -= 3
C $A796,3 A = boost
C $A799,1 Set flags
C $A79A,2 A = $C8
C $A79E,2 A = $E6
C $A7A4,3 Read HL from LD BC at $B32E
C $A7A7,4 HL += 40
C $A7AB,3 ($B32F) = HL
C $A7AE,2 D = 0
C $A7B1,3 HL = $B4F0
C $A7BC,2 D = 4
C $A7BE,5 D = wanted_stage_number + D
C $A7C3,2 E = 0
C $A7C5,3 A = *$8006
C $A7C8,1 Set flags
C $A7CB,1 A = D
C $A7CC,1 D = E
C $A7CD,4 A <<= 4  prob
C $A7D1,1 E = A
C $A7D2,1 A = 0
C $A7D6,2 A = 5
C $A7D8,3 Self modify 'LD A' at $A73E to load A
C $A7DB,3 Point #REGhl at smash_chatter ("BEAR DOWN" / "OH MAN" / etc.)
C $A7DE,3 Call chatter
C $A7E1,3 BC = $0301
C $A7E4,3 Exit via start_sfx
b $A7E7 Data block at A7E7
B $A7E7,12,8,4
c $A7F3 Spawns cars
D $A7F3 Used by the routines at #R$8401 and #R$852A.
@ $A7F3 label=spawn_cars
C $A7F3,9 Return if perp_caught_stage or stop_car_spawning flags are set
C $A7FC,3 A = *$A254  -- spawning flag
C $A7FF,2 Return if $A254 is zero
C $A801,4 A = <...> - A  Self modified below
C $A805,3 Self modify above
C $A808,1 Return if ?
C $A809,3 Call rng
C $A80C,3 C = A & 15
C $A80F,3 A = sighted_flag
C $A812,1 Set flags
C $A816,2 Jump to $A81A if sighted_flag was zero
C $A818,2 A += 25
C $A81A,1 A += C
C $A81B,3 Self modify above
C $A81E,3 BC = $0500
C $A821,4 IX = &hazards[1]
C $A825,3 DE = 20  stride of hazards
@ $A828 label=sc_loop
C $A828,6 If the hazard is not active jump to sc_fill_in
C $A82E,3 A = IX[15]
C $A837,2 IX += DE
C $A839,2 Loop to sc_loop while B
C $A83B,1 Return
@ $A83C label=sc_fill_in
C $A83F,3 DE = IX
C $A842,3 BC = 20
C $A845,3 Point #REGhl at hazard_template
C $A848,2 Copy hazard_template
C $A84A,2 Buffer offset of 20 for get_spawn_lanes
C $A84C,3 Call get_spawn_lanes  -- Gets car spawning positions (B = min, C = max?)
C $A84F,3 Call rng
C $A852,3 A = (A & 3) + B
C $A855,4 If A >= C A = C
C $A859,3 IX[17] = A
C $A85C,3 IX[18] = A
C $A85F,8 A = $A7E6[A]
C $A867,3 IX[5] = A
C $A86A,3 A = sighted_flag
C $A86D,1 Set flags
C $A86E,2 A = 4
C $A872,1 A <<= 1
C $A873,2 HL += A
C $A875,4 IX[13] = *HL
C $A879,3 Call rng
C $A87C,3 C = A & 6
C $A87F,3 A = sighted_flag
C $A882,1 Set flags
C $A885,2 A = 6
C $A88A,2 C--
C $A88C,2 B = 0
C $A88E,3 HL = &lods_table[3] (first of the generic car lods)
C $A891,1 HL += BC
C $A892,9 wordat(IX+9) = HL
C $A89B,1 Return
c $A89C Returns lanes that cars can spawn in.
D $A89C Return $0101 and cars only appear in the leftmost lane. $0102 (1st and 2nd, mainly 2nd). $0104 first three lanes. $0304 - third and fourth lanes.
R $A89C I:C Buffer offset
R $A89C O:BC TBD
@ $A89C label=get_spawn_lanes
C $A89C,6 A = [current buffer offset] + 66 + C   (wrapping around)
C $A8A2,4 A = $EE00[A]
C $A8A6,1 Set flags
C $A8A7,3 BC = $0104
C $A8AA,1 Return if zero
C $A8AB,1 E = A  preserve
C $A8AC,2 A &= $C1
C $A8AE,2 CP $C1
C $A8B0,1 Return if zero
C $A8B1,2 CP $41
C $A8B3,3 BC = $0103
C $A8B6,1 Return if zero
C $A8B7,4 A = (E & $82) << 1
C $A8BB,2 Jump to $A8C5 if didn't carry
C $A8BD,3 BC = $0204
C $A8C0,1 Return if non-zero
C $A8C1,3 BC = $0103
C $A8C4,1 Return
C $A8C5,3 BC = $0102
C $A8C8,1 Return if zero
C $A8C9,3 BC = $0304
C $A8CC,1 Return
c $A8CD Hazard handler routine? Triggered at road split
@ $A8CD label=hazard_handler_a8cd
C $A8CD,6 Return if perp_caught_stage > 0
C $A8D3,6 Check stop_car_spawning flag
C $A8D9,4 IX[14] = $01  -- horizontal position
C $A8DD,4 IX[13] = $FF
C $A8E1,3 C = IX[1]  -- buffer offset
C $A8E4,3 Call get_spawn_lanes
C $A8E7,3 A = IX[17]
C $A8EA,6 If A < B IX[18] = B
C $A8F0,8 If A > C IX[18] = C
C $A8F8,3 A = IX[18]
C $A8FB,5 Jump to $A926 if A == IX[17]
C $A902,3 HL = $A7E6   -> $A7E7 data block
C $A905,1 C = A
C $A906,1 A += L
C $A907,1 L = A
C $A908,3 A = IX[5]
C $A90F,2 A -= 5
C $A911,1 CP *HL
C $A914,1 A = *HL
C $A915,3 IX[17] = C
C $A918,2 Jump to $A923
C $A91A,2 A += 5
C $A91C,1 CP *HL
C $A91F,1 A = *HL
C $A920,3 IX[17] = C
C $A923,3 IX[5] = A
C $A926,3 A = IX[7]
C $A929,1 Set flags
C $A92A,1 Return if zero
C $A92B,1 Preserve AF
C $A92C,4 IX[7] = 0
C $A930,5 Return if crashed flag set
C $A935,3 IX[0] = A
C $A938,3 overtake_bonus = A
C $A93B,2 A = $96
C $A93D,1 Restore AF
C $A93E,4 Jump if A < 3
C $A942,2 A -= 3
C $A947,3 HL = ouch_chatter
C $A94A,2 A = 3
C $A94C,3 Call chatter
C $A94F,3 BC = $0302
C $A952,3 Call start_sfx
c $A955 Used by the routines at #R$8401 and #R$852A.
D $A955 Used by the routines at #R$8401 and #R$852A.
@ $A955 label=main_loop_18
C $A955,5 Return if $A248 is zero
C $A95A,5 Return if $A254 is zero
C $A95F,3 Point #REGhl at something above the stack
C $A962,3 Call rng
C $A965,8 Stack 1 if it's +ve or zero, or 2 if it's -ve
C $A96D,3 Call rng
C $A970,2 Stack that random byte
C $A972,5 Self modify 'LD A' at $A97E to load 1
C $A977,3 Self modify 'LD A' at $COBB to load 1
C $A97A,3 Self modify 'LD A' at $A9DE to load 1
C $A97D,1 Return
c $A97E Used by the routines at #R$8401 and #R$852A.
D $A97E Used by the routines at #R$8401 and #R$852A.
@ $A97E label=main_loop_20
C $A97E,2 A = <...>  -- Self modified by $A974
C $A980,2 Return if #REGa is zero
C $A982,6 #REGiy = $E361
C $A988,3 BC = $1400
C $A98B,3 Point #REGhl at something above the stack
N $A98E Scan through whatever-it-is for a non-zero byte. Move 4 bytes per iteration.
@ $A98E label=ml20_loop1
C $A98E,1 A = *HL++
C $A98F,2 Set flags
C $A991,2 Jump to A9A7 if non-zero
C $A993,3 HL += 3
@ $A996 label=ml20_loop1_continue
C $A996,2 IY--
C $A998,2 Loop to ml20_loop1 while B
C $A99A,1 A = C
C $A99B,1 Set flags
C $A99C,1 Return if non-zero
C $A99D,3 Self modify LD A @ $A97E
C $A9A0,3 Self modify LD A @ $C0BB
C $A9A3,3 Self modify LD A @ $A9DE
C $A9A6,1 Return
C $A9A7,2 A = *HL++
C $A9A9,1 C++
C $A9AB,1 Bank A
C $A9AC,6 L = ~(IY[1] * 2)
C $A9B2,2 H = $E8
C $A9B4,1 D = *HL
C $A9B5,1 L--
C $A9B6,1 E = *HL
C $A9B7,2 H = $EC
C $A9B9,1 A = *HL
C $A9BA,1 L++
C $A9BB,1 H = *HL
C $A9BC,1 L = A
C $A9BD,1 Set flags
C $A9BF,2 HL -= DE
C $A9C2,3 HL = $0000
C $A9C5,1 Unbank A
N $A9C6 Multiplier.
C $A9C6,2 B = 8
@ $A9C8 label=ml20_loop2
C $A9C9,3 If carry HL += DE
C $A9CC,1 HL <<= 1
C $A9CD,2 Loop to ml20_loop2 while B
C $A9CF,1 A = H
C $A9D1,1 C = A
C $A9D3,1 HL += BC
C $A9D7,4 DE = wordat(HL); HL += 2
C $A9DB,3 Jump to ml20_loop1_continue
c $A9DE dust/stones
D $A9DE Used by the routine at #R$8F5F.
@ $A9DE label=sub_A9DE
C $A9DE,2 A = <...>  Self modified by $A97A, $A9A3
C $A9E0,1 Set flags
C $A9E1,1 Return if zero
C $A9E2,3 HL = $ED28    -- stack
C $A9E5,3 A = *HL; HL += 2
C $A9E8,1 Set flags
C $A9E9,2 Jump to A9F1 if non-zero
C $A9EB,2 HL += 2
C $A9ED,3 Self modify 'LD HL' at $A9E2 to load HL
C $A9F0,1 Return
C $A9F1,4 DE = lods_table[0] (stones_lods)
C $A9F5,1 A--
C $A9F6,6 If non-zero DE = lods_table[1] (dust_lods)
C $A9FC,2 C = *HL++
C $A9FE,1 A = *HL
C $AA00,1 HL++
C $AA01,3 Self modify 'LD HL' at $A9E2 to load HL
C $AA04,1 A = 0
C $AA05,3 Self modify 'LD D' at $933D to load zero
C $AA08,1 H = A
C $AA09,1 A = B
C $AA0A,1 A--
C $AA0B,6 If A >= 11 A = 10
C $AA11,2 A >>= 1
C $AA13,6 L = A * 7
C $AA19,1 HL += DE
C $AA1A,7 E = *HL * 8
C $AA22,1 Set flags
C $AA23,1 A = C
C $AA24,2 C = $00
C $AA26,3 Jump to $AA33 if negative
C $AA29,3 Return if non-zero
C $AA2C,3 Jump to $9303 if A >= 128
C $AA2F,1 A += E
C $AA30,3 Jump to $929A
C $AA33,1 A += E
C $AA34,1 Return if no carry
C $AA35,3 Jump to $929A
c $AA38 Routine at AA38
D $AA38 $AB89 self modifies $8FA4 to call this.
@ $AA38 label=sub_AA38
C $AA38,1 A = B
C $AA39,3 Return if A != 3
C $AA3C,6 A = IY[$4F] - IY[$4E]
C $AA42,2 D = 0
C $AA44,1 H = D -- ie HL = 0
C $AA45,1 L = D
C $AA46,1 E = A
C $AA47,3 A = fast_counter
C $AA4A,2 B = 8
C $AA4C,2 A &= $E0
@ $AA4E label = subAA38_loop1
C $AA4F,3 If carry HL += DE
C $AA52,1 HL <<= 1
C $AA53,2 Loop $AA4E while B
C $AA55,1 A = H
C $AA57,3 Self modify 'LD A' at $AA8C
C $AA5A,2 A = <...> -- Self modified by $AAEE
C $AA5C,3 A -= IY[$4E]
C $AA5F,3 Self modify 'ADD A' at $AA76
C $AA62,2 B = 5  iterations
C $AA64,5 A = counter_A & 1  -- counter used for turbo smoke
C $AA69,3 HL = *$5D08  -- set to zero for stage 1...
C $AA6C,2 If A was set
C $AA6E,3 HL = *$5D0A
@ $AA71 label=loop_aa71
C $AA71,4 DE = wordat(HL); HL += 2
C $AA75,1 A = *DE
C $AA76,2 A += <...> -- Self modified by $AA5F
C $AA78,1 DE++
C $AA79,4 Preserve IY, HL, BC
C $AA7D,3 Call sub_AA94
C $AA80,4 Restore IY, HL, BC
C $AA84,2 Loop loop_aa71 while B
C $AA86,3 DE = wordat(HL); HL++
C $AA89,1 A = 0
C $AA8A,2 Preserve IY
C $AA8C,2 A = <...> -- Self modified by $AA57
C $AA8E,3 Call sub_AA94
C $AA91,2 Restore IY
C $AA93,1 Return
@ $AA94 label=sub_AA94
C $AA94,3 BC = <...> -- Self modified by $AB00, $AB2F + others
C $AA97,2 A = -A
C $AA99,3 Self modify the 'LD D' at $933D to load A
C $AA9C,5 HL = *DE++
C $AAA1,1 Set flags
C $AAA5,1 H--
C $AAA6,1 HL += BC
C $AAA8,1 B = *HL
C $AAA9,6 B <<= 3
C $AAAF,1 A = D
C $AAB0,1 Set flags
C $AAB1,1 A = E
C $AAB2,2 C = 0
C $AAB7,1 Return if non-zero
C $AAB8,5 Jump to $9309 if A >= 128
C $AABD,1 A += B
C $AABE,3 Jump to $929A
C $AAC1,1 A += B
C $AAC2,1 Return if no carry
C $AAC3,3 Jump to $929A
c $AAC6 Routine at AAC6
D $AAC6 Used by the routine at #R$8401.
@ $AAC6 label=move_helicopter
C $AAC6,5 Return if helicopter_control is zero
C $AACB,2 A = <...>  -- Self modified by $AAD3 below
C $AACD,4 Jump to $AAD6 if A == 97
C $AAD1,2 A -= 2
C $AAD3,3 Self modify $AACB above
C $AAD6,1 C = A
C $AAD7,2 A = <...>  -- Self modified below
C $AAD9,3 A = (A + 1) & 3
C $AADC,3 Self modify $AAD7 above
C $AADF,2 A = <...> -- Self modified by $AAE5 below
C $AAE1,2 what are we testing here?
C $AAE3,2 A = -A
C $AAE5,3 Self modify $AADF above
C $AAE8,2 A += <...>  -- Increment by old amount -- Self modified below
C $AAEA,3 Self modify above with new value of A
C $AAED,1 A += C
C $AAEE,3 Self modify 'LD A' @ $AA5A
C $AAF1,3 HL = road_pos
C $AAF4,2 DE = HL
C $AAF6,3 BC = <...>  -- Self modified by $AAFC below, $AB60  -- prev road pos?
C $AAF9,1 Set flags
C $AAFA,2 HL -= BC
C $AAFC,4 Self modify 'LD BC' above to load DE
C $AB00,4 Load DE from 'LD BC' @ $AA94
C $AB04,1 HL += DE
C $AB05,1 Preserve HL
C $AB06,3 DE = $0070
C $AB09,2 B = 0
C $AB0B,1 Set flags
C $AB0C,2 HL -= DE
C $AB0E,1 Restore HL
C $AB11,2 A = 8
C $AB13,1 Preserve AF
C $AB17,1 B--
C $AB18,2 A = -A
C $AB1A,1 C = A
C $AB1B,1 HL += BC
C $AB1C,1 Preserve HL
C $AB1D,1 Set flags
C $AB1E,2 HL -= DE
C $AB20,1 Restore HL
C $AB24,1 Restore AF
C $AB2A,1 Restore AF
C $AB2F,3 ($AA95) = HL
C $AB32,1 Return
c $AB33 Helicopter.
D $AB33 Used by the routine at #R$8401.
@ $AB33 label=helicopter
C $AB33,5 Return if helicopter_control is zero
C $AB38,3 Jump to hc_1 if helicopter_control is one
C $AB3B,3 Jump to hc_pick_direction if helicopter_control >= 3
N $AB3E Otherwise helicopter_control is 2.
C $AB3E,3 Read from 'LD BC' at $AA94
C $AB41,1 Set flags
C $AB42,1 Return if zero
C $AB43,1 A = 0  -- NOP
C $AB44,2 DE = 0  -- NOPs
C $AB47,1 A = 0
C $AB48,2 Jump to hc_3
@ $AB4A label=hc_1
C $AB4A,3 HL = $FFC8
C $AB4D,2 New value for helicopter_control is 2
C $AB4F,2 Jump to hc_2
@ $AB51 label=hc_pick_direction
C $AB51,1 A--
C $AB52,3 Point #REGhl at pilot "turn left" messages
C $AB55,2 Jump to hc_chatter if A now zero (helicopter_control == 3)
C $AB57,1 A--
C $AB58,1 Return if A now non-zero
C $AB59,3 Point #REGhl at pilot "turn right" messages (helicopter_control == 4)
@ $AB5C label=hc_chatter
C $AB5C,8 Self modify 'LD BC' @ $AAF6 to load road_pos
C $AB64,7 Self modify 'LD BC' @ $AA94 to load $FFE8
C $AB6B,4 Self modify 'LD A' @ $AAD7 to load zero
C $AB6F,3 Self modify 'ADD A' @ $AAE8 to load zero
C $AB72,4 Self modify 'LD A' @ $AADF to load one
C $AB76,2 A = 15
C $AB78,3 Call chatter
C $AB7B,3 HL = $0070
C $AB7E,5 Self modify 'LD A' at $AACB to load $85
C $AB83,2 New value for helicopter_control is 5
@ $AB85 label=hc_2
C $AB85,1 Preserve AF
C $AB86,3 Self modify 'LD DE' at $AB06 to load $0070, or $FFC8
C $AB89,3 Address of sub_AA38
C $AB8C,2 Opcode for CALL
@ $AB8E label=hc_3
C $AB8E,7 Self modify $8FA4 to be CALL sub_AA38, or NOPs
C $AB95,1 Restore AF
C $AB96,3 helicopter_control = A
C $AB99,1 Return
c $AB9A Routine at AB9A
D $AB9A Used by the routines at #R$8401, #R$852A and #R$873C.
@ $AB9A label=spawn_hazards
C $AB9A,3 Spawning flag
C $AB9E,1 Return if zero
N $ABF6 Spawns hazards in the road, like barriers and tumbleweeds.
N $ABF6 I:B Stored at entry+5  e.g. $20/$46/$56/$50/$B4
N $ABF6 I:C Stored at entry+1  e.g. $13
@ $ABF6 label=sh_spawn
C $ABF6,1 Bank entry registers
C $ABF7,2 6 iterations
C $ABF9,3 Point #REGhl at hazards table
C $ABFC,3 Stride of 20 bytes
@ $ABFF label=sh_loop
C $ABFF,2 Check the flag byte to see if the entry is used (it's either $00 if empty, or $FF if used, so rotating it in place is not a problem)
C $AC01,2 Jump to sh_found_spare if it didn't carry
C $AC03,1 Move to the next entry
C $AC04,2 Loop while iterations remain
C $AC07,1 Return
N $AC08 #REGhl points to the unused entry
@ $AC08 label=sh_found_spare
C $AC08,3 #REGix = #REGhl
C $AC0B,10 Zero 20 bytes at #REGhl
C $AC15,1 Restore entry registers
C $AC16,9 wordat(IX + 11) = Address of $AC3C routine
C $AC1F,4 Point #REGhl at the hazards data table entry (*$5CF6 -> $5E40 + 3)
C $AC23,5 Copy flag TBD
C $AC28,9 Copy lod address (e.g. tumbleweed_lods)
C $AC31,3 IX[5] = B TBD
C $AC34,3 IX[1] = C TBD
C $AC37,4 Mark the entry as used
C $AC3B,1 Return
c $AC3C Test for collision with hazard
R $AC3C I:IX Address of hazard structure ($A188+)
@ $AC3C label=hazard_hit
C $AC3C,3 IX[15] appears to be a delay of some sort
C $AC3F,3 If IX[15] != 0 then goto hh_ac92
C $AC42,5 If IX[7] == 0 then return
N $AC47 If we arrive here then a hit has occurred.
C $AC47,4 Load speed
C $AC4B,6 If IX[7] < 0 then #REGde = 280
C $AC51,2 #REGbc = #REGde
C $AC53,3 Point #REGhl at $AD03 table
N $AC56 This must be mapping the speed into the 5-entry array.
C $AC56,1 A = D     top part of speed
C $AC57,2 E ROL 1   double speed
C $AC59,1 A ROL 1
C $AC5A,2 A &= 3    mask top part to 3 (for ref. turbo speed = $1FF)
C $AC5C,2 RR D      halve D to carry?
C $AC5E,2 D = 0
C $AC60,2 A = (A + D + carry) * 2  scale for element size of 2
C $AC62,1 E = A   it's an offset in $AD03
C $AC63,1 HL += DE
C $AC64,9 Copy (two bytes?) to IX+17 from table
C $AC6D,4 double BC?  adjusted speed retained from earlier
C $AC71,8 If B >= 2 BC = 350  so this is if we hit the object very fast it gets put on the left?
C $AC79,3 Set bottom byte of horizontal position
C $AC7C,3 Increment IX[7]
C $AC7F,2 if B is zero then don't store it
C $AC81,3 Set top byte of horizontal position
C $AC84,3 Increment IX[1]
C $AC87,6 Call start_sfx with BC = $0503
C $AC8D,2 A = 2        [set this to 1 and it drives off like a car]
C $AC8F,3 Set IX[15] to 2
@ $AC92 label=hh_ac92
C $AC92,1 A--
C $AC93,1 If A == 0 return
C $AC94,13 IX[16] = $ACDB[IX[17]]
C $ACA1,3 Increment IX[17]
C $ACA4,6 Load horizontal position into #REGhl
C $ACAA,2 #REGde = #REGhl
C $ACAC,4 Divide horizontal position by 2?
C $ACB0,8 Then by 16 once it's 8 bits
C $ACB8,2 Then subtract from the previous position
C $ACBA,6 Set horizontal position
C $ACC0,8 Toggle the bottom bit of IX[19]
C $ACC8,3 Decrement IX[18]
C $ACCB,1 If IX[19] != 0 then return
C $ACCC,7 Zero the horizontal position
C $ACD3,4 Set IX[19] to 1
C $ACD7,3 Set IX[15] to 1
C $ACDA,1 Return
B $ACDB,40,8 $AC94 uses this
W $AD03,10,2 $AC53 uses this
c $AD0D Routine at AD0D
D $AD0D Used by the routine at #R$BDFB.
c $AD51 Routine at AD51
D $AD51 Used by the routines at #R$AD0D and #R$ADA0.
C $AD71,3 A = fast_counter
c $ADA0 Routine at ADA0
D $ADA0 Used by the routines at #R$8401, #R$852A and #R$873C.
@ $ADA0 label=main_loop_19
C $ADA0,1 A = 0
C $ADA1,3 $A222 = 0
C $ADA4,3 IY = $E3xx
C $ADA7,4 IX = &hazards[0]
C $ADAB,3 DE = 20
C $ADAE,2 B = 6
@ $ADB0 label=ml19_loop
C $ADB0,4 If the hazard is active ...
C $ADB4,1 Bank
C $ADB5,3 Call $ADBE
C $ADB8,1 Unbank
C $ADB9,2 Move to next hazard
C $ADBB,2 Loop while #REGb
C $ADBD,1 Return
C $AE03,3 A = fast_counter
C $AE1B,4 Increment overtake_bonus
C $AE1F,1 Return
N $AECF This entry point is used by the routine at #R$8F5F.
C $AF7E,3 Get smash_factor
C $AF91,3 HL -> $CDEC
C $AF9E,3 Get smash_factor
C $AFCF,3 Get smash_factor
w $B045 Hero car jump table
D $B045 Values used to make the car move vertically (by self modifying #R$B58E). Used by $B968.
@ $B045 label=jump_table
W $B045,18,2 Main table (vertical delta describing an arc, TBD)
W $B057,12,2 Sub-table (another byte pair) [suspect this really starts two bytes later]
c $B063 Hero car jumps; gear changing; off road checks; speed adjustment
D $B063 Used by the routines at #R$8401 and #R$852A.
@ $B063 label=move_hero_car
C $B063,2 Load jump counter. Self modified by $8827 and $B965. Highest is 8.
C $B065,3 Jump to mhc_not_jumping if the jump counter is zero
N $B068 Hero car is jumping.
@ $B068 label=mhc_jumping
C $B068,4 Decrement the jump counter in (self modified) $B063
C $B06C,2 Jump to $B079 if we're still jumping
N $B06E Hero car has landed.
@ $B06E label=mhc_landed
C $B06E,5 smoke = 3
C $B073,3 Thump sfx as the car lands
C $B076,3 Call start_sfx
N $B079 Hero car is in mid-air.
@ $B079 label=mhc_midair
C $B079,3 Point #REGhl at entry in jump table. Self modified by $B96C and $B092
C $B07C,4 off_road = 0
C $B080,8 Clear up/down/left/right bits of user input (can't turn in mid-air etc.)
C $B088,5 $B5B0 = *HL++      -- Self modified value in draw_car
C $B08D,5 A = *$B5AB + *HL++ -- Self modified value in draw_car - vertical shift?
C $B092,3 Update HL load above, $B079
@ $B095 label=mhc_not_jumping
C $B095,3 *$B5AB = A
C $B098,3 Address of boost timer
C $B09B,4 Jump if a turbo boost is not in effect
C $B09F,1 Decrement boost timer
C $B0A0,2 Jump if boost timer is non-zero
C $B0A2,7 Decrement turbo boost count [POKE $B0A5,0 for Infinite turbos]
N $B0A9 Handle smoke effect.
@ $B0A9 label=mhc_smoke
C $B0A9,1 #REGhl now points at smoke
C $B0AA,4 Jump if smoke is zero
C $B0AE,1 Decrement smoke
N $B0AF Handle gear changes.
C $B0AF,3 Read user input
C $B0B2,1 C = A
C $B0B3,3 Read from the LD A at $B325 (crash flag)
C $B0B6,3 Jump if zero
C $B0B9,4 C = C & $10, i.e. Fire/Gear
C $B0BD,1 A = C
C $B0BF,2 Test it again? [Strange to re-test it?]
C $B0C1,3 Address of gear flag
C $B0C4,3 A = *$A252
C $B0C7,2 Jump if fire/gear is unset
C $B0C9,3 Jump if A252 is set
C $B0CC,4 Toggle gear flag
C $B0D0,2 A = 4
C $B0D2,2 Jump if low gear?
C $B0D4,3 smoke = A
C $B0D7,2 Decement A  [why write it as a SUB 1?]
C $B0D9,2 jump if +ve?
C $B0DB,3 $A252 = A
C $B0DE,1 Read current gear flag
C $B0DF,1 Bank it
C $B0E0,3 Read current speed
C $B0E3,1 Stack it
C $B0E4,5 HL -= 120
C $B0E9,2 Jump if speed >= 120
C $B0EB,6 Jump if perp_caught_stage > 0
N $B0F1 Handle idle timer.
C $B0F1,3 Address of idle timer
C $B0F4,1 Decrement idle timer
C $B0F5,2 Jump if non-zero
C $B0F7,2 Reset idle timer to 100
C $B0F9,2 A = 10
C $B0FB,3 HL = get_moving_chatter
C $B0FE,3 Call chatter
C $B101,1 Pop speed
C $B102,2 DE = HL
C $B104,3 Load the off-road flag
C $B107,3 Jump if zero
N $B10A Handle off-road (it can be 1 or 2).
C $B10A,3 BC = 120  -- factor for off-road == 1
C $B10D,3 Jump if off-road was one
C $B110,3 BC = 110  -- factor for off-road == 2
C $B113,1 Clear carry?
C $B114,2 HL -= BC  -- 16-bit sub just for testing
C $B116,2 HL = DE
C $B118,2 Jump if HL < BC (result of subtraction)
C $B11A,1 A = L
C $B11B,2 H >>= 1  -- 9 bit rotate right through carry
C $B11D,4 A <<= 4  -- 9 bit rotate left through carry
C $B121,6 A = -((A & $0F) | 1)
C $B127,3 BC = $FF00 | A
C $B12D,3 A = boost (time remaining)
C $B130,1 Test then bank the flags
C $B131,1 Bank
C $B132,1 Check previously banked A
C $B135,3 BC = 470
C $B138,1 Unbank boost + flags
C $B139,2 Uses banked flags
C $B13B,3 BC = 230
C $B13E,2 HL -= BC
C $B143,3 C = ~L
C $B146,3 B = ~H
C $B149,1 BC++
C $B14A,1 A = C
C $B14B,2 B >>= 1
C $B14E,2 B >>= 1
C $B153,5 C = (A & $3F) | 1
C $B15A,1 A = L
C $B15B,2 H >>= 1
C $B15E,2 H >>= 1
C $B163,6 A = -((A & $1F) | 1)
C $B169,3 BC = $FF00 | A
C $B16E,3 BC = 220
C $B171,2 HL -= BC  -- for flags
C $B173,2 HL = DE
C $B175,2 from result of calc
C $B177,3 BC = 470
C $B17D,1 A = E
C $B17E,1 B = D
C $B17F,2 B >>= 1
C $B182,2 B >>= 1
C $B187,5 C = (A | 1) & $1F
C $B18E,3 BC = 695
C $B194,3 BC = 360
C $B19A,2 Get stacked #REGaf
C $B19C,3 Test bit 2?
C $B1A1,3 BC = $FFEC
C $B1AC,2 HL = DE
C $B1AE,1 Clear carry?
C $B1AF,2 HL += BC
C $B1B4,3 HL = $0000
C $B1B7,5 A = *$A24C - 1
C $B1BE,3 A = $B5B0
C $B1C1,1 Set flags
C $B1C4,1 C = A
C $B1C5,1 A = H
C $B1C6,1 A |= L
C $B1C9,5 A = (C - 5) | 1
C $B1CE,1 C = A
C $B1CF,2 B = 0
C $B1D4,1 B--
C $B1D5,2 DE = HL
C $B1D7,1 HL += BC
C $B1D8,3 BC = 695
C $B1DC,2 HL -= BC
C $B1E2,2 A = 3
C $B1E4,3 *$A24C = A
C $B1E7,1 A = H
C $B1E8,2 CP 2
C $B1EC,3 Cap speed to $1FF
C $B1EF,3 Set speed to #REGhl
C $B1F3,3 A = *$A263
C $B1F6,1 B = A
C $B1F7,3 A = *$A264
C $B1FA,1 C = A
C $B1FB,3 A = *$B064  -- Jump counter [self modified]
C $B1FE,1 Set flags
C $B20A,3 A = C - 9
C $B20F,1 A = 0
C $B210,1 C = A
C $B211,3 A = B - 9
C $B216,1 A = 0
C $B217,1 B = A
C $B21A,3 A = B + 4
C $B21D,5 Jump if A >= 36
C $B222,1 B = A
C $B223,3 C -= B
C $B228,2 C = 0
C $B22C,3 A = C + 4
C $B22F,2 C = $24
C $B234,1 C = A
C $B235,3 B -= C
C $B23A,2 B = 0
C $B23C,3 HL = speed
C $B23F,1 A = L
C $B243,2 A >>= 1
C $B245,2 A >>= 1
C $B247,1 L = A
C $B248,2 L >>= 1
C $B24A,1 A += L
C $B24E,1 B = A
C $B252,1 C = A
C $B254,3 BC = $0000
C $B257,1 E = B
C $B258,3 A = *$A25C
C $B25B,1 Set flags
C $B262,1 E++
C $B263,2 A = -A
C $B265,3 HL = $B827
C $B268,1 C = A
C $B269,1 HL += BC
C $B26A,3 A = *$A261
C $B26D,1 C = A
C $B26F,4 A = fast_counter - C
C $B273,1 Set flags
C $B276,1 C = *HL
C $B277,1 A -= C
C $B27A,1 B++
C $B27C,1 A += C
C $B280,1 A = B
C $B281,1 Set flags
C $B284,3 HL = $A262
C $B287,1 A += *HL
C $B288,1 *HL = A
C $B289,3 A = B * 3
C $B28C,2 B = 0
C $B292,1 B--
C $B293,2 A = -A
C $B295,1 C = A
C $B297,3 *$A261 = A
C $B29A,4 HL = *$A25F + BC
C $B29E,3 DE = $0000
C $B2A1,4 $A25F = DE
C $B2A5,3 A = *$B326  -- Read self modified op in main_loop_24 -- crashed flag
C $B2A8,1 Set flags
C $B2AC,1 D = H
C $B2AE,4 *$A263 = B
C $B2B2,4 *$A264 = C
C $B2B6,1 A -= B
C $B2BA,1 D--
C $B2BB,1 E = A
C $B2BE,1 HL += DE
C $B2BF,1 Set flags
C $B2C5,2 A = -A
C $B2C7,2 CP 17
C $B2C9,2 A = 0
C $B2E3,2 A = 1
C $B2E5,3 cornering = A
C $B2E8,4 BC = road_pos
C $B2EC,1 HL += BC
C $B2ED,3 road_pos = HL
C $B2F0,2 D = 1
C $B2F2,1 A = E
C $B2F3,1 Set flags [why are some ORs and some ANDs?]
C $B2F7,1 D--
C $B2F8,2 A = -A
C $B2FA,2 CP 12
C $B2FC,2 B = 2
C $B300,1 B--
C $B301,2 CP 6
C $B305,1 B--
C $B306,1 A = B
C $B307,3 turn_speed = A
C $B30A,1 A = D
C $B30B,3 flip = A
C $B30E,3 A = *$B064  -- Jump counter [self modified]
C $B311,1 Set flags
C $B312,1 Return if zero
C $B313,4 cornering = 0
C $B317,1 Return
c $B318 Routine at B318
D $B318 Used by the routines at #R$8401 and #R$852A.
@ $B318 label=main_loop_24
C $B318,7 If speed > 0 jump $B325
C $B31F,3 Self modify 'LD A' at $B3DB to load A
C $B322,3 off_road = A
C $B325,2 [this seems to be a crashed flag]
C $B327,1 Set flags
C $B32A,3 cornering = A
C $B32E,3 BC = <...>
C $B331,2 HL -= BC
C $B334,2 Jump to $B349 if HL < BC
C $B336,1 D = H
C $B337,1 A = L
C $B338,2 D >>= 1
C $B33C,2 A >>= 1
C $B33E,2 OR 3
C $B340,1 E = A
C $B341,2 HL -= DE
C $B343,2 equal
C $B345,2 A = 2
C $B347,2 HL > DE
C $B349,4 Clear crashed flag
C $B34D,1 A++
C $B350,3 ($A24A) = HL
C $B353,3 turn_speed = A
C $B356,3 HL = $0000
C $B359,1 D = H
C $B35A,1 E = L
C $B35B,2 D >>= 1
C $B35D,2 RR E
C $B35F,2 E >>= 1
C $B361,2 E >>= 1
C $B363,2 E >>= 1
C $B365,2 HL -= DE
C $B367,3 ($B357) = HL
C $B36B,3 HL = road_pos
C $B36E,2 C = 0
C $B370,1 A = C
C $B371,2 AND 1
C $B373,3 flip = A
C $B376,1 C--
C $B379,1 C--
C $B37C,1 HL += DE
C $B37F,2 HL -= DE
C $B381,3 road_pos = HL
C $B384,2 A = $00
C $B386,1 Set flags
C $B389,1 A--
C $B38A,3 ($B385) = A
C $B38D,2 A = $00
C $B38F,3 ($B3DC) = A
C $B392,3 HL = road_pos
C $B395,3 DE = $0048
C $B398,1 A = H
C $B399,1 Set flags
C $B39F,1 A = L
C $B3A0,1 A -= E
C $B3A3,3 DE = $01D8
C $B3A6,1 A = H
C $B3AC,1 A = L
C $B3AD,1 A -= E
C $B3B1,3 road_pos = HL
C $B3B4,7 A = cornering | smoke
C $B3BB,6 Call start_sfx with cornering noise
C $B3C1,6 Jump if perp_caught_stage > 0
C $B3C7,1 A--
C $B3CA,2 CP $02
C $B3CE,2 A = $02
C $B3D0,3 turn_speed = A
C $B3D3,5 flip = 1
C $B3DB,2 A = $00
C $B3DD,1 Set flags
C $B3E0,1 C = A
C $B3E2,3 A += C + $18
C $B3E5,1 C = A
C $B3E6,3 A = turn_speed
C $B3E9,2 CP 2
C $B3ED,3 A = flip
C $B3F0,1 Set flags
C $B3F3,1 C++
C $B3F4,1 C++
C $B3F6,6 B = counter_A & 1
C $B3FD,1 C = A
C $B3FF,1 A = C
C $B403,4 off_road = 0
C $B40A,2 B = $00
C $B40C,3 A = off_road
C $B40F,1 A--
C $B412,9 B = (counter_C & 1) * 3
C $B41B,3 A = turn_speed
C $B421,3 A = *$A228
C $B424,1 Set flags
C $B427,1 A = 0
C $B428,3 BC = $0102
C $B42E,4 B = counter_A
C $B432,7 Jump if cornering
C $B439,3 A = counter_C
C $B43C,1 B = A
C $B43D,1 HL++
C $B43E,2 A = *HL++
C $B440,1 A |= *HL
C $B443,3 A = off_road
C $B446,3 Return if not fully off-road
C $B449,1 A = 0
C $B44B,1 A = B
C $B44D,3 Call draw_smoke  (seems to be the right side)
C $B450,2 A = $01
C $B454,3 Exit via draw_smoke
C $B457,3 A = *$A22F
C $B45A,1 Set flags
C $B45B,1 Return if zero
C $B45C,1 A--
C $B460,1 B = A
C $B461,2 C = 0
C $B464,3 A = turn_speed
C $B467,2 CP 2
C $B469,2 A = $24
C $B46E,5 A = flip + $25
C $B473,3 Exit via $B69E
C $B476,2 C = 0
C $B478,2 A = 0
C $B47A,1 A--
C $B47B,3 *$B479 = A
C $B480,2 B = 2
C $B482,1 C++
C $B483,1 A = C
C $B484,2 CP 4
C $B488,1 A++
C $B489,1 C = A
C $B48A,2 CP 2
C $B48E,1 B++
C $B48F,1 A = B
C $B490,3 Self modify 'LD A' at $B478 to load A
C $B493,1 A = C
C $B494,3 *$B477 = A
C $B497,2 CP 7
C $B49B,4 *$A22F = 0
C $B49F,1 Return
C $B4A0,3 A = turn_speed
C $B4A3,2 CP 2
C $B4A5,2 A = 6
C $B4A9,10 A = (A << 3) - flip + 13
C $B4B3,1 A += C
C $B4B9,1 C = A
C $B4BA,3 A = *$B477
C $B4BD,4 Jump to $B4C6 if A >= 4
C $B4C1,1 C++
C $B4C2,1 A = C
C $B4C3,3 Exit via $B699
C $B4C6,5 *$A228 = 1
C $B4CB,1 Return
c $B4CC Routine at B4CC
D $B4CC Used by the routine at #R$A637.
C $B4CC,5 *$B477 = 0
C $B4D1,3 *$A22F = 1
C $B4D4,3 *$A22E = 1
C $B4D7,1 A++
C $B4DB,6 time_sixteenths/$A17D = 15, time_bcd/$A17E = $60
C $B4E1,3 Point #REGhl at left light's attributes
C $B4E4,3 Toggle its brightness
C $B4E7,3 Point at "SIGHTING OF TARGET VEHICLE" message
C $B4EA,3 Call message_printing_related
C $B4ED,3 Exit via hooked routine (likely sfx, siren?)
c $B4F0 Smash handling
@ $B4F0 label=smash
C $B4F0,8 Cycle #REGa one step through 0..3 each time the routine is entered
C $B4F8,14 *$B55C = $CE33 + #REGa * 6
C $B506,5 $B54A = 9
C $B50B,4 Load and increment smash_counter
C $B50F,2 20 hits? [POKE $B50F for Single hit capture]
C $B511,3 Exit via fully_smashed if so
C $B514,2 19 hits?
C $B516,2 Jump to smash_b522 if not
N $B518 We have 19 hits
C $B519,2 A = 10
C $B51B,3 HL = raymond_says_one_more_time
C $B51E,3 Call chatter
@ $B522 label=smash_b522
C $B522,3 Set smash_counter to #REGa
C $B525,5 Set smash_factor to zero if smash_counter is zero
C $B52A,5 Set smash_factor to 1 if smash_counter < 4
C $B52F,5 Set smash_factor to 2 if smash_counter < 7
C $B534,5 Set smash_factor to 3 if smash_counter < 11
C $B539,5 Set smash_factor to 4 if smash_counter < 14
C $B53E,5 Set smash_factor to 5 if smash_counter < 17
C $B543,1 Otherwise set smash_factor to 6
@ $B544 label=set_smash_factor
C $B544,4 Set smash_factor to #REGc
C $B548,1 Return
c $B549 Routine at B549
D $B549 Used by the routine at #R$B318.
c $B58E Draws the car
D $B58E Used by the routine at #R$B318.
R $B58E I:A TBD
@ $B58E label=draw_car
C $B58E,6 If #REGa is zero then flip = A
C $B594,1 C=A
C $B596,9 Point #REGhl at ? then add (#REGa * 4)
C $B59F,3 Point #REGde at the car tiles [needs a proper label]
C $B5A2,2 C=7
C $B5A4,3 Call sub_b627
C $B5A8,4 117 - (self modified value $B5AB)
C $B5AC,1 D=A
C $B5AD,4 A=C+B+(self modified value $B5B0)
C $B5B1,6 If A >= 9 A -= 9
C $B5B7,3 Point #REGhl at car definitions[A] (entries are 20 bytes wide)
C $B5C5,4 A=*$A22A + *HL
C $B5C9,2 A=0-A
C $B5CB,1 A+=D  (aka A=D-A)
C $B5CC,1 E=A
C $B5CD,2 A = A & $0F
C $B5CF,2 A += $F0
C $B5D1,1 D=A
C $B5D2,1 A=E
C $B5D3,2 A &= $70
C $B5D5,1 A = A*2
C $B5D6,2 A += $0D
C $B5D8,1 E=A
C $B5D9,1 HL++
C $B5DA,2 B=*HL++
C $B5DC,2 A=*HL++
C $B5E0,1 H=*HL
C $B5E1,1 L=A
C $B5E2,2 A=5
C $B5E4,1 E=A
C $B5E5,2 D=0
C $B5EA,6 If flip jump to draw_car_perhaps_flipped
C $B5F1,3 Call plot_sprite -- #REGa is (how many pixels to plot - 1) / 8
C $B5F4,2 Jump to draw_car_cont
@ $B5F6 label=draw_car_perhaps_flipped
C $B5F7,1 L--
C $B5F8,3 Call plot_sprite_flipped
@ $B5FB label=draw_car_cont
C $B5FD,1 HL++
C $B5FE,7 E=$68 C=$05 (width) CALL $B627  draws the top (windscreen)
C $B605,7 E=$68 C=$05 (width) CALL $B627  draws the bottom (wheels)
C $B60C,6 Check flip flag
C $B612,4 unflipped
C $B616,2 flipped
C $B618,3 Draws left side of car
C $B61B,2 C = 1
C $B61D,3 A = flip
C $B620,1 Set flags
C $B621,2 E = $60
C $B623,2 Jump if flipped
C $B625,2 E = $90
@ $B627 label=sub_b627
C $B627,1 A=D
C $B629,3 D=A - *HL++
C $B62C,2 B=*HL++
C $B62E,2 A=*HL++
C $B631,1 H=*HL
C $B632,1 L=A
C $B636,3 A=flip
C $B639,1 B=A
C $B63A,1 E=C
C $B63B,1 C--
C $B63C,4 If A==0 C=A
C $B641,6 Draws right side of car
C $B647,1 Return
c $B648 Draw the hero car's smoke
D $B648 Used by the routine at #R$B318.
@ $B648 label=draw_smoke
C $B648,11 #REGhl = &hero_car_smoke[A]  A is 0..3
C $B653,3 A = *$B064 -- jump counter
C $B656,1 Set flags
C $B657,1 Return if non-zero -- don't draw smoke if jumping
C $B658,2 C = *HL++  -- these must be size and position
C $B65A,2 B = *HL++
C $B65C,2 D = *HL++
C $B65E,2 E = *HL++
C $B660,2 A = *HL++  -- frame pointer
C $B662,1 H = *HL
C $B663,1 L = A
C $B665,1 Bank
C $B668,1 B = A
C $B669,1 E = C
C $B66A,1 C--
C $B66B,1 Set flags
C $B66C,2 Jump to $B673 if non-zero
C $B66E,1 C = A
C $B66F,1 Unbank
C $B670,1 A = E
C $B671,2 Jump to $B675
C $B673,1 Unbank
C $B674,1 A = D
C $B675,3 E = A + 127 -- horizontal position
C $B678,2 D = 119  -- vertical position
C $B67A,2 Exit via $B6D6
c $B67C Routine at B67C
D $B67C Used by the routine at #R$B318.
R $B67C I:A ?
R $B67C I:B ? (gets compared to turn_speed) e.g. 1
R $B67C I:C ? (used wrt flipping)           e.g. 2
@ $B67C label=sub_b67c
C $B67C,7 A += counter_C & 1
C $B683,1 Bank A
C $B684,6 Jump to $B698 if turn_speed < B
C $B68A,3 A = flip
C $B68D,1 Set flags
C $B68E,2 A = 0
C $B690,2 Jump to sub_b67c_no_flip if flip is zero
C $B692,1 A += C
@ $B693 label=sub_b67c_no_flip
C $B693,1 A += C
C $B694,1 C = A
C $B696,1 A += C
N $B699 This entry point is used by the routine at #R$B318.
C $B69A,3 BC = 0   -- not self modified
N $B69E This entry point is used by the routine at #R$B318.
C $B69E,2 E = $80
C $B6A0,6 BC = A * 3
C $B6A6,4 HL = $CFB2 + BC  -> unknown_cfb2
C $B6AA,5 D = *HL++ + $79
C $B6AF,4 E = *HL++ + E
C $B6B3,1 C = *HL  -- B is still zero
C $B6B4,4 HL = &car_adornments + BC
C $B6B8,2 B = *HL++  -- height
C $B6BA,2 C = *HL++  -- byte width
C $B6BC,4 HL = wordat(HL); HL++   -- masked bitmap data
C $B6C0,1 A = C
C $B6C2,1 E = A  -- bank byte width in E'
C $B6C4,3 Read from SUB @ $B5AA
C $B6C7,4 D -= A
C $B6CB,3 A = *$B5B0 -- Self modified value in draw_car
C $B6CE,2 A >>= 1
C $B6D0,4 Jump to $B6D6 if zero
C $B6D4,2 D += A - 2
N $B6D6 This entry point is used by the routines at #R$B58E and #R$B648.
C $B6D6,7 D = -(*$A22A) + D
N $B6DD This entry point is used by the routines at #R$8F5F and #R$B549.
C $B6DD,1 A = E
C $B6DE,5 Divide by 8
C $B6E3,1 E = A
C $B6E4,1 A = D
C $B6E6,6 D = (D & $0F) + $F0
C $B6ED,5 E += (A & $70) * 2
C $B6F3,1 E = C  -- byte width
C $B6F4,2 E <<= 1
C $B6F8,1 A = E
C $B6F9,2 B >>= 1
C $B6FB,1 unclear why, is this EX'ing AF to get the carry flag?
C $B6FC,1 HL += BC
N $B701 This entry point is used by the routine at #R$92E1.
C $B701,4 IX = plot_masked_sprite_core_thingy
C $B705,3 A = ~A + 9
C $B708,4 This multiplies by six - the length of each load-mask-store step in the plotter core.
C $B70C,5 Add that to #REGix
C $B711,2 B = 15 -- Set loop counter for 15 iterations?
C $B714,2 D = 0 -- fall through
c $B716 Masked sprite plotter
D $B716 This plots a lot of the game's masked graphics.
D $B716 The stack points to pairs of bitmap and mask bytes and HL must point to the screen buffer. Uses AND-OR type masking. Proceeds left-right. Doesn't flip the bytes so there must be an alternative for that.
R $B716 I:B Rows
R $B716 I:DE Stride (e.g. 10 for 40px wide)
R $B716 I:HL Address of data to plot
@ $B716 label=plot_masked_sprite
@ $B71C label=plot_masked_sprite_loop
C $B716,9 Save #REGsp for restoration on exit
C $B71F,5 Restore original #REGsp
N $B724 This entry point is used by the routine at #R$B7EF.
@ $B724 label=plot_masked_sprite_entry
C $B724,1 Point #REGhl at the graphic data
@ $B729 label=plot_masked_sprite_core_thingy
C $B729,1 Load a bitmap and mask pair (#REGd,#REGe)
C $B72A,2 Load the screen pixels and AND with mask
C $B72C,2 OR in new pixels and store back to screen
C $B72E,1 Move to next screen pixel
C $B72F,41 <Repeat 8 times>
C $B758,20 Handle end of row. This must be adjusting the screen pointer.
c $B76C Masked sprite plotter which flips
D $B76C Used by the routine at #R$92E1.
@ $B76C label=plot_masked_sprite_flipped
C $B76C,3 #REGde = #REGa
C $B76F,1 #REGhl += #REGde
N $B770 This entry point is used by the routine at #R$B67C.
C $B771,4 Save #REGsp for restoration on exit
C $B775,4 Point #REGix at mystery table
C $B779,3 Subtract 8 ???
C $B77C,3 Multiply by 8
C $B77F,5 Add to IX
C $B784,2 H = $EF
C $B794,8 Return
B $B79C,8,8
C $B7A4,1 Load a bitmap and mask pair (B,C)
C $B7A5,1 Set flip table index (assuming table is aligned)
C $B7A6,1 Load the screen pixels
C $B7A7,1 AND screen pixels with flipped mask
C $B7A8,1 Set flip table index
C $B7A9,1 OR in new flipped pixels
C $B7AA,1 Store back to the screen
C $B7AB,1 Move to next screen pixel (downwards in memory)
C $B7AC,47 Repeat 8 times
C $B7DB,20 Handle end of row. This must be adjusting the screen pointer.
c $B7EF Routine at B7EF
D $B7EF Used by the routine at #R$92E1.
C $B7EF,4 self modify $B71F - exit of plot_masked_sprite
C $B7F3,4 IX = &plot_masked_sprite_core_thingy (jump table)
C $B7F7,3 A = 8 - A
C $B7FA,9 IX += A * 6
C $B803,2 B = $0F
C $B806,2 D = 0
C $B80A,1 H = D
C $B80B,1 L = D
C $B80C,1 A = B
C $B80D,2 B = 5
C $B80F,1 A--
@ $B812 label=j_b812
C $B813,2 JR NC,j_b816
C $B815,1 HL += DE
@ $B816 label=j_b816
C $B816,1 HL += HL
C $B817,2 DJNZ j_b812
C $B81C,1 HL += DE
C $B81E,1 HL += BC
C $B820,1 D--
C $B821,4 E = -E
C $B825,3 Exit via plot_masked_sprite_entry
b $B828 breaks/crashes road rendering if messed with
B $B828,32,8
c $B848 Routine at B848
D $B848 Used by the routines at #R$8401, #R$852A and #R$873C.
@ $B848 label=main_loop_11
C $B8A1,3 A = fast_counter
c $B8D2 Routine at B8D2
D $B8D2 Used by the routine at #R$BDFB.
C $B93E,3 Load jump counter
C $B95A,8 HL = $B057 + A*2  looks early.. might be 1-indexed? [sampled A: 3,2,4]
C $B962,2 E = *HL++  offset into B045
C $B964,1 Load new jump value [sampled: $B060]
C $B965,3 Set jump counter
C $B968,4 HL = $B045 + E  points into jump values table
C $B96C,28 Self modify $B079 {Form road_buffer_offset }
C $B988,5 Check fork_taken
C $B98D,2 Jump if left fork was taken
C $B9EC,1 A = 0
C $B9ED,3 $A261 = 0
C $B9F0,3 $A262 = 0
C $B9F3,1 Return
c $B9F4 Routine at B9F4
D $B9F4 Used by the routines at #R$8401, #R$852A and #R$873C.
@ $B9F4 label=main_loop_12
C $B9F4,3 A = road_buffer_offset [as byte]
C $B9F7,2 A += 64 -- is lanes data
C $B9F9,3 DE = $EE00 | A
C $B9FC,3 B=20, C=$E1 -- counter, mask
C $B9FF,2 Counter/Index
N $BA01 Suspect this is counting the distance to road split.
C $BA01,1 Load a lanes byte
C $BA02,5 Jump if (byte & $E1) == $E1
C $BA07,1 Advance to next lanes byte (wrapping around)
C $BA08,1 Increment counter
C $BA09,2 Loop while B
N $BA0B No road split found?
C $BA0B,3 [road is still rendered if this is NOPped, but road is in wrong position]
C $BA0E,4 Self modify return path
C $BA12,3 must be data
C $BA17,3 Self modify $BA35 to load $EA30
C $BA1A,3 Self modify $BA3F to load $EB30
C $BA1D,3 Self modify $BA44 to load $E930
C $BA23,3 Self modify $BA27 to load ($E800 | A)
C $BA26,1 Bank
C $BA27,4 Self modified
C $BA32,2 BC = HL
C $BA38,2 DE = BC
C $BA47,2 A += 2
C $BA49,1 Unbank
C $BA4A,3 Loop while A > 0 ?
@ $BA4D label=ml12_exit
C $BA4D,3 Restore original SP
C $BA50,1 Return
N $BA51 Handle road split.
C $BA52,3 Set split road countdown
C $BA57,5 Set split road visible
C $BA5D,3 A = ~A + $69
C $BA64,3 A = *DE & 4
C $BA6F,2 A = -A
C $BA74,4 Get road position
C $BA78,2 A = 1
C $BA7A,1 D--
C $BA81,1 A = E
C $BA86,2 Jump if E < 12
C $BA88,1 A--
C $BA89,3 Set fork_taken to #REGa
C $BA8C,1 A++
C $BA8E,3 Address of correct_fork
C $BA91,1 Match?
C $BA92,3 -> Tony: "LET'S GO. MR. DRIVER." <STOP>
C $BA95,2 Jump if correct fork was taken
C $BA99,3 A195 is horz position of perp car
C $BA9C,5 A = wanted_stage_number + 4
C $BAA1,1 D = A
C $BAA2,1 A = 0
C $BAA3,1 E = A
C $BAA7,3 -> Raymond: "WHAT ARE YOU DOING MAN!!" / "THE BAD GUYS ARE GOING THE OTHER WAY." <STOP>
C $BAAC,3 Call chatter
C $BAB0,3 Spawning flag
C $BAB4,2 Jump if zero
C $BAB6,1 C = A
C $BAB7,3 A = <mystery>
C $BABA,1 A += C
C $BABB,1 C = A
C $BABC,2 A -= 2
C $BAC0,1 C = A
C $BAC1,3 DE = 16
C $BAC4,1 HL += DE
C $BAD0,3 A = fast_counter
C $BAD3,4 A ROR 4
C $BAD7,2 A &= $0F
C $BAD9,2 A -= 16
C $BADB,3 DE = $FF00 | A
C $BADE,1 HL += DE
C $BAE0,6 Jump if fork_taken was 1 (right fork taken)
N $BAE6 Left fork was taken.
C $BAEA,3 Get road position
C $BAEF,3 Set road position
N $BAF7 Right fork was taken.
C $BAFB,3 Get road position
C $BB01,3 Set road position
C $BB08,3 Get road position
c $BB69 Setup for road handling perhaps split related
D $BB69 Used by the routine at #R$8401.
@ $BB69 label=main_loop_27
C $BB69,5 Return if $A268 is zero
C $BB6E,4 A = fork_taken - 1
C $BB72,2 Jump if A is zero (right fork taken)
N $BB74 Set up left route.
C $BB74,3 DE = $FC01  (super super hard left?, 1?)
C $BB7B,3 road_leftside_ptr  = $E2D9
C $BB7E,6 road_rightside_ptr = $E2D5
C $BB84,6 road_lanes_ptr     = $E2E7
C $BB8A,3 Self modified by $C035 to be the left route's hazards data
C $BB8D,3 Self modified by $BF73 to be the left route's rightside data
C $BB90,3 Self modified by $BFBC to be the left route's leftside data
C $BB94,3 Self modified by $BE5C to be the left route's curvature data
C $BB97,3 Self modified by $BEAE to be the left route's height data
C $BB9A,3 Self modified by $BEF9 to be the left route's lanes data
C $BB9D,2 A = 64   offset into road buffer
C $BB9F,2 Jump to ml27_bbcc
N $BBA1 Set up right route.
C $BBA1,3 DE = $0403 (super super hard right?, 3?)
C $BBA5,6 road_leftside_ptr  = $E2D5
C $BBAB,6 road_rightside_ptr = $E2D9
C $BBB1,6 road_lanes_ptr     = $E2F3
C $BBB7,3 Self modified by $C03D to be the right route's hazards data
C $BBBA,3 Self modified by $BF73 to be the right route's rightside data
C $BBBD,3 Self modified by $BFC4 to be the right route's leftside data
C $BBC1,3 Self modified by $BE62 to be the right route's curvature data
C $BBC4,3 Self modified by $BEB6 to be the right route's height data
C $BBC7,3 Self modified by $BF01 to be the right route's lanes data
C $BBCA,2 A = 32   offset into road buffer
@ $BBCC label=ml27_bbcc
C $BBCC,3 Self modify rm_curvature_one_command handler
C $BBCF,4 Self modify rm_height_one_command handler
C $BBD3,4 Self modify rm_lanes_one_command handler
C $BBD8,4 Self modify rm_leftside_one_command handler
C $BBDC,4 Self modify rm_rightside_one_command handler
C $BBE0,3 Self modify rm_hazards_one_command handler
C $BBE3,6 road_curvature_ptr = $E2DD
C $BBE9,6 road_height_ptr    = $E2E2
C $BBEF,6 road_hazard_ptr    = $E2D2
C $BBF6,1 C = A
C $BBF7,3 A = road_buffer_offset [as byte]
C $BBFA,3 HL = $EE00 | A  [unsure why the code does this when $A240 seems to be the right value]
C $BBFD,6 Set the 32 bytes at #REGhl to #REGd
C $BC03,9 Set the 32 bytes at #REGhl + 64 to #REGe
C $BC0C,9 Zero the 32 bytes at #REGhl + 64 + #REGc
C $BC15,5 curvature_byte     = 0
C $BC1A,3 height_byte        = 0
C $BC1D,3 leftside_byte      = 0
C $BC20,3 rightside_byte     = 0
C $BC23,3 hazards_byte       = 0
C $BC26,3 lanes_counter_byte = 0
C $BC29,3 $A269              = 0
C $BC2C,3 fork_taken         = 0
C $BC2F,3 $A265              = 0
C $BC32,4 no_objects_counter = 1
C $BC36,3 $A16D              = 1
C $BC39,4 $A267              = 0  [B & C are zero here]
C $BC3D,1 Return
c $BC3E Copies the backbuffer at $F000 to the screen.
D $BC3E Used by the routines at #R$8014, #R$8258, #R$8401, #R$858C, #R$873C and #R$F220.
@ $BC3E label=draw_screen
C $BC3E,4 Point #REGhl at screen pixel (136,64). This is positioned halfway across so we can PUSH to the screen via SP.
C $BC42,3 Point #REGhl' at backbuffer + 1 byte.
C $BC45,4 Self modify #REGsp restore instruction
N $BC49 A sequence that transfers 16 bytes
C $BC49,1 Point #REGsp at the back buffer
C $BC4A,12 Pull in 16 bytes from the back buffer (order: AF' DE BC AF' DE' BC' IX IY), incrementing SP
C $BC56,1 Point #REGsp at screen
C $BC57,7 Push out 16 bytes to the screen (reversing the order), decrementing SP
C $BC5E,1 Advance screen pointer
C $BC5F,5 Continue pushing
C $BC64,1 Advance back buffer pointer
C $BC65,28 Transfer another 16 bytes (128 pixels)
C $BC81,28 Transfer another 16 bytes (128 pixels)
C $BC9D,28 Transfer another 16 bytes (128 pixels)
C $BCB9,2 is the back buffer row a multiple of 4?
C $BCBB,3 loop if so
C $BCBE,5 Advance back buffer pointer (backwards) by a half row?
C $BCC3,5 Advance screen pointer (backwards) by half a row?
N $BCC8 This is a similar sequence but only moves 14 bytes (not using IY)
C $BCC8,1 Point #REGsp at the back buffer
C $BCC9,23 Pull in 16 bytes from the back buffer (order: AF' DE BC AF' DE' BC' IX), incrementing SP
C $BCE0,24 Transfer another 14 bytes (112 pixels)
C $BCF8,24 Transfer another 14 bytes (112 pixels)
C $BD10,24 Transfer another 14 bytes (112 pixels)
C $BD28,2 Is the back buffer row multiple of 4?
C $BD2D,2 Is the back buffer row multiple of 8?
C $BD31,2 L -= $F0, H = $F0
C $BD36,3 L >= $F0
C $BD39,3 PE = parity even
C $BD3D,3 Increment the screen address
@ $BD45 label=draw_screen_bd45
C $BD46,3 Point #REGhl at screen pixel (136,128).
C $BD4A,3 Copy more
@ $BD4D label=draw_screen_bd4d
C $BD51,1 Which one are we modifying here?
C $BD54,2 Ditto, which?
@ $BD5A label=draw_screen_attributes
C $BD5A,6 Don't update the attributes if the level intro screen is being shown
C $BD63,1 Seems to be related to the horizon level
C $BD68,1 Jump to exit check if both are zero
C $BD6B,3 E = A * 4
C $BD6E,1 A = $FF if carry set, zero otherwise
C $BD6F,1 D = A
C $BD70,3 Fetch the address of the first line of ground attributes (+ 31)
C $BD73,3 Set the sky colour screen attributes (always black over bright cyan)
C $BD76,2 If A was zero then jump (Z => sky, NZ => ground)
C $BD78,4 Load the ground colour screen attributes (varies per level)
C $BD7C,2 Point #REGsp at the screen attributes (DE = $FFE0 = -32)
C $BD7E,15 Fill 30 bytes - length of attribute line minus the two blank edges
C $BD8F,1 Move to next line of attributes
C $BD90,3 Save the address of the first line of ground attributes (+ 31)
C $BD93,6 Exit if sighted_flag is zero (flashing lights / smash mode)
C $BD99,7 Jump if perp_caught_stage >= 3 (when car stops)
N $BDA0 Set the smash meter attributes
C $BDA0,3 Screen attribute (2,11)
C $BDA3,3 = 32
C $BDA6,2 Black over bright red
C $BDA8,4 Set two attrs
C $BDAC,2 Black over bright magenta
C $BDAE,4 Set two attrs
C $BDB2,2 Black over bright green
C $BDB4,4 Set two attrs
C $BDB8,2 Black over bright white
C $BDBA,3 Set two attrs
@ $BDBD label=draw_screen_exit
C $BDBD,3 Restore #REGsp (self modified at start of routine)
C $BDC0,1 Return
c $BDC1 Clears screen then sets in-game attributes
D $BDC1 Used by the routines at #R$8014, #R$858C and #R$87DC.
@ $BDC1 label=clear_screen_set_attrs
C $BDC1,3 Call clear_game_screen
C $BDC4,13 Clear the game screen pixels to $FF (redoes similar work just done)
C $BDD1,12 Clear the game screen attributes to $28 (black over cyan) - first two rows only
C $BDDD,6 Clear the next three rows to $68 (black over bright cyan)
C $BDE3,9 Clear the next 11 rows to the current ground colour
C $BDEC,14 Clear the edges of the game screen to black on black
C $BDFA,1 Return
c $BDFB Map reader
D $BDFB Used by the routines at #R$8401, #R$852A and #R$873C.
@ $BDFB label=read_map
C $BDFB,4 ($A23D) = 0  -- state var TBD
C $BDFF,3 ($A23C) = 0  -- state var TBD
C $BE02,3 ($A254) = 0  -- spawning flag
C $BE05,3 HL = Address of fast_counter
C $BE08,4 DE = speed
C $BE0C,1 A = Bottom byte of speed
C $BE0D,4 If speed < 255 then jump
C $BE11,2 Preserve
C $BE13,3 Call subfunction $BE1F  -- processing something twice as much when at high speed?
C $BE16,2 Restore
@ $BE18 label=j_be18
C $BE18,2 fast_counter += bottom byte of speed in A
C $BE1A,2 A = 0 doesn't seem self modified
C $BE1C,3 Jump to j_c0d9 if no carry
N $BE1F This entry point is used by the routine at #R$87DC.
@ $BE1F label=rm_cycle_buffer_offset
C $BE1F,3 A = road_buffer_offset + 1  [Accessing road_buffer_offset as a byte here]
C $BE22,1 road_buffer_offset = A      [Again, as a byte]
C $BE23,5 HL = $EE00 | (A + 95)  -- index $EE00, the cyclic road buffer thing
C $BE28,7 *$A23C |= *HL
C $BE2F,4 L += 32    -- offset 128
C $BE33,7 *$A23D |= *HL
C $BE3A,4 L -= 96    -- offset 32
N $BE3E -- CURVATURE --
N $BE3E The top nibble of each byte is a counter. The bottom nibble is curvature data.
C $BE3E,5 A = curvature_byte - 16
C $BE43,2 Jump to rm_save_curvature_byte if curvature_byte >= 16
N $BE45 Load a new curvature byte.
C $BE45,5 Read and increment address of curvature data
C $BE4A,1 Read a curvature byte
C $BE4B,1 Set flags
C $BE4C,2 Jump to rm_curvature_regular_byte if non-zero
N $BE4E Handle curvature escape byte.
@ $BE4E label=rm_curvature_escape_byte
C $BE4E,1 #REGhl is now address of escape byte
C $BE4F,3 Increment, read command byte, increment again
C $BE52,3 Jump to rm_curvature_jump_command if command byte is zero
C $BE55,3 Jump to rm_curvature_one_command if command byte is one
N $BE58 Otherwise it must be a road split command (byte == 2).
C $BE58,4 Load address of left route's curvature data
C $BE5C,4 Self modify 'LD HL,$xxxx' at $BB94 to load the address
C $BE60,4 Load address of right route's curvature data
C $BE64,3 Self modify 'LD HL,$xxxx' at $BBC1 to load the address
C $BE67,3 HL = $E2C6 -> road split left curvature data
C $BE6A,2 Jump to rm_read_curvature
N $BE6C Handle a command byte of one: TBD.
@ $BE6C label=rm_curvature_one_command
C $BE6C,3 HL = <map curvature address> -- Self modified by $BBCC
C $BE6F,2 Jump to rm_read_curvature
N $BE71 Handle a command byte of zero: jump to the address given in next two bytes.
@ $BE71 label=rm_curvature_jump_command
C $BE71,4 HL = wordat(HL)
@ $BE75 label=rm_read_curvature
C $BE75,1 #REGde is now map curvature address
C $BE76,1 Read a curvature byte (regular, not an escape)
@ $BE77 label=rm_curvature_regular_byte
C $BE77,4 road_curvature_ptr = #REGde
C $BE7B,2 A -= 16  -- decrement counter in top nibble
@ $BE7D label=rm_save_curvature_byte
C $BE7D,3 curvature_byte = A
C $BE80,2 Mask off direction bits
C $BE82,4 Jump to rm_set_curvature if (A & (1<<3)) is zero   -- bit 3 => left (so jump if right)
C $BE86,4 A = -(A & 7)  -- Left direction becomes -ve
N $BE8A Write the direction into the cyclic road buffer.
@ $BE8A label=rm_set_curvature
C $BE8A,2 *HL = A * 2  -- Sampled HL: $EEA9 [in cyclic road buffer]
C $BE8C,4 L += 32
N $BE90 -- HEIGHT --
N $BE90 The top nibble of each byte is a counter. The bottom nibble is height data.
C $BE90,5 A = height_byte - 16
C $BE95,2 Jump to rm_save_height_byte if height_byte >= 16
C $BE97,5 Read and increment address of height data
C $BE9C,1 Read a height byte
C $BE9D,1 Set flags
C $BE9E,2 Jump to rm_height_regular_byte if non-zero
N $BEA0 Handle height escape byte.
C $BEA0,1 #REGhl is now address of escape byte
C $BEA1,3 Increment, read command byte, increment again
C $BEA4,4 Jump to rm_height_jump_command if command byte is zero
C $BEA8,2 Jump to rm_height_one_command if command byte is one
N $BEAA Otherwise it must be a road split command (byte == 2).
C $BEAA,4 Load address of left route's height data
C $BEAE,4 Self modify 'LD DE,$xxxx' at $BB97 to load the address
C $BEB2,4 Load address of right route's height data
C $BEB6,3 Self modify 'LD DE,$xxxx' at $BBC4 to load the address
C $BEB9,3 HL = $E2CC -> road split height data
C $BEBC,2 Jump to rm_read_height
@ $BEBE label=rm_height_one_command
C $BEBE,3 HL = <something>  -- Self modified by $BBCF
C $BEC1,2 Jump to rm_read_height
@ $BEC3 label=rm_height_jump_command
C $BEC3,4 HL = wordat(HL)
@ $BEC7 label=rm_read_height
C $BEC7,1 #REGde is now height address
C $BEC8,1 Read a height byte (regular, not an escape)
@ $BEC9 label=rm_height_regular_byte
C $BEC9,4 road_height_ptr = #REGde
C $BECD,2 A -= 16  -- decrement counter in top nibble
@ $BECF label=rm_save_height_byte
C $BECF,3 height_byte = A
C $BED2,5 *HL = (A & 15) - 8  -- Sampled HL: $EEC9 [in cyclic road buffer]
C $BED7,4 L += 32
N $BEDB -- LANES --
N $BEDB Lanes bytes are counted (like RLE).
C $BEDB,5 Decrement the lanes counter
C $BEE0,2 Jump to rm_lanes_count_resume if non-zero
N $BEE2 Lanes counter hit zero, so reload.
C $BEE2,5 Read and increment address of lanes data
C $BEE7,1 Read a lanes byte
C $BEE8,1 Set flags
C $BEE9,2 Jump to rm_lanes_regular_byte if non-zero
N $BEEB Handle lanes escape byte.
C $BEEB,1 #REGhl is now address of escape byte
C $BEEC,3 Increment, read command byte, increment again
C $BEEF,3 Jump to rm_lanes_jump_command if command byte is zero
C $BEF2,3 Jump to rm_lanes_one_command if command byte is one
N $BEF5 Otherwise it must be a road split command (byte == 2).
C $BEF5,4 Load address of left route's lanes data
C $BEF9,4 Self modify 'LD BC,$xxxx' at $BB9A to load the address
C $BEFD,4 Load address of right route's height data
C $BF01,3 Self modify 'LD BC,$xxxx' at $BBC7 to load the address
C $BF04,3 HL = $E2D1 -> road split lanes data
C $BF07,2 Jump to rm_read_lanes
@ $BF09 label=rm_lanes_one_command
C $BF09,3 HL = $0000  -- This looks setup for it as others, but doesn't seem to be self modified.
C $BF0C,2 Jump to rm_read_lanes
@ $BF0E label=rm_lanes_jump_command
C $BF0E,4 HL = wordat(HL)
@ $BF12 label=rm_read_lanes
C $BF12,1 #REGde is now lanes address
C $BF13,1 Read a lanes byte (regular, not an escape)
@ $BF14 label=rm_lanes_regular_byte
C $BF14,4 lanes_counter_byte = (lanes byte) - 1
C $BF18,5 road_lanes_ptr = ++DE
C $BF1D,4 *HL = *DE & $F7  -- HL points into cyclic road buffer? Sampled HL = $EE60
C $BF21,3 A = *DE & $FB
C $BF24,3 Self modify 'LD A,$xx' at $BF2C
C $BF27,2 Jump to rm_lanes_done
N $BF29 Resume updating lanes data.
@ $BF29 label=rm_lanes_count_resume
C $BF29,3 Store decremented lanes counter
C $BF2C,2 A = <self modified value>  -- Self modified above AND below
C $BF2E,1 C = A  -- Save temporarily
C $BF2F,4 Jump to rm_lanes_bf39 if (A & $0C) is zero
N $BF33 (bottom two bits significant)
C $BF33,3 A = C & $F3
C $BF36,3 Self modify 'LD A,$xx' at $BF2C
@ $BF39 label=rm_lanes_bf39
C $BF39,1 *HL = C  -- Sampled HL: $EEE9
@ $BF3A label=rm_lanes_done
C $BF3A,4 L += 32
N $BF3E -- SKIPPING ALL OBJECTS AND HAZARDS --
C $BF3E,4 Decrement no_objects_counter
C $BF42,2 Jump to rm_rightside if it's zero
N $BF44 Clear the right side, left side and hazards
C $BF44,7 HL[ 0] = 0
C $BF4B,4 HL[32] = 0
C $BF4F,1 HL[64] = 0
C $BF50,2 Reset no_objects_counter to 1
C $BF52,3 Jump to rm_all_hazards
N $BF55 -- RIGHT-SIDE OBJECTS --
N $BF55 The top nibble of each byte is a counter. The bottom nibble is rightside data.
@ $BF55 label=rm_rightside
C $BF55,5 A = rightside_byte - 16
C $BF5A,2 Jump to rm_rightside_count_resume if rightside_byte >= 16
N $BF5C Rightside counter hit zero, so reload.
C $BF5C,5 Read and increment address of rightside data
C $BF61,1 Read a rightside byte
C $BF62,1 Set flags
C $BF63,2 Jump to rm_rightside_regular_byte if non-zero
N $BF65 Handle rightside escape byte.
C $BF65,1 #REGhl is now address of escape byte
C $BF66,3 Increment, read command byte, increment again
C $BF69,3 Jump to rm_rightside_jump_command if command byte is zero
C $BF6C,3 Jump to rm_rightside_one_command if command byte is one
N $BF6F Otherwise it must be a road split command (byte == 2).
C $BF6F,4 Load address of left route's rightside data
C $BF73,4 Set modify 'LD DE,$xxxx' at $BB8D to load the address
C $BF77,4 Load address of right route's rightside data
C $BF7B,3 Self modify 'LD DE,$xxxx' at $BBBA to load the address
C $BF7E,3 HL = $E2C1  [overlapping use of data TBD]
C $BF81,2 Jump to rm_read_rightside
@ $BF83 label=rm_rightside_one_command
C $BF83,3 HL = $0000  -- This looks setup for it as others, but doesn't seem to be self modified.
C $BF86,2 Jump to rm_read_rightside
@ $BF88 label=rm_rightside_jump_command
C $BF88,4 HL = wordat(HL)
@ $BF8C label=rm_read_rightside
C $BF8C,1 #REGde is now lanes address
C $BF8D,1 Read a rightside byte (regular, not an escape)
@ $BF8E label=rm_rightside_regular_byte
C $BF8E,4 road_rightside_ptr = DE
C $BF92,2 A -= 16
@ $BF94 label=rm_rightside_count_resume
C $BF94,3 rightside_byte = A  -- Store decremented rightside counter
C $BF97,3 *HL = A & $0F  -- Write to cyclic road buffer
C $BF9A,4 L += 32
N $BF9E -- LEFT-SIDE OBJECTS --
C $BF9E,5 A = leftside_byte - 16
C $BFA3,2 Jump to rm_leftside_count_resume if leftside_byte >= 16
C $BFA5,5 DE = road_leftside_ptr + 1
C $BFAA,1 Read a lanes byte
C $BFAB,1 Set flags
C $BFAC,2 Jump to rm_leftside_regular_byte if non-zero
N $BFAE Handle leftside escape byte.
C $BFAE,1 #REGhl is now address of escape byte
C $BFAF,3 Increment, read command byte, increment again
C $BFB2,3 Jump to rm_leftside_jump_command if command byte is zero
C $BFB5,3 Jump to rm_leftside_one_command if command byte is one
N $BFB8 Otherwise it must be a road split command (byte == 2).
C $BFB8,4 Load address of left route's leftside data
C $BFBC,4 Self modify 'LD BC,$xxxx' at $BB90
C $BFC0,4 Load address of right route's leftside data
C $BFC4,3 Self modify 'LD BC,$xxxx' at $BBBD
C $BFC7,3 HL = $E2C0  [overlapping use of data TBD]
C $BFCA,2 Jump to rm_read_leftside
@ $BFCC label=rm_leftside_one_command
C $BFCC,3 HL = $0000  -- This looks setup for it as others, but doesn't seem to be self modified.
C $BFCF,2 Jump to rm_read_leftside
@ $BFD1 label=rm_leftside_jump_command
C $BFD1,4 HL = wordat(HL)
@ $BFD5 label=rm_read_leftside
C $BFD5,1 #REGde is now lanes address
C $BFD6,1 Read a leftside byte (regular, not an escape)
@ $BFD7 label=rm_leftside_regular_byte
C $BFD7,4 road_leftside_ptr = DE
C $BFDB,2 A -= 16
@ $BFDD label=rm_leftside_count_resume
C $BFDD,3 leftside_byte = A  -- Store decremented leftside counter
C $BFE0,3 *HL = A & $0F  -- Write to cyclic road buffer
C $BFE3,4 L += 32
N $BFE7 -- HAZARDS --
C $BFE7,5 A = hazards_byte - 1  [it's a counter]
C $BFEC,2 Jump to rm_no_hazards if no carry
C $BFEE,5 DE = road_hazard_ptr + 1
@ $BFF3 label=rm_restart_hazards_read
C $BFF3,1 Read a hazards byte
C $BFF4,1 Set flags
C $BFF5,2 Jump to rm_hazards_regular_byte if non-zero
N $BFF7 Handle hazards escape byte.
C $BFF7,1 #REGhl is now address of escape byte
C $BFF8,3 Increment, read command byte, increment again
C $BFFB,1 Set flags
C $BFFC,3 Jump to rm_hazards_jump_command if command byte is zero
C $BFFF,2 Jump to rm_hazards_one_command if command byte is one
C $C001,4 Jump to rm_hazards_road_fork_command if command byte is two
C $C005,4 Jump to rm_hazards_set_hazard_command if command byte is < 10 -- 3..9
C $C009,4 Jump to rm_hazards_set_arrow_command if command byte is < 13 -- 10..12
C $C00D,4 Jump to rm_hazards_spawning_command if command byte is < 15 -- 13..14
C $C011,5 Otherwise helicopter_control = A - 11
C $C016,2 Jump to rm_do_restart
N $C018 Handle car spawning commands (13/14 = enable/disable car spawning).
@ $C018 label=rm_hazards_spawning_command
C $C018,5 stop_car_spawning = A - 10
C $C01D,2 Jump to rm_do_restart
N $C01F Handle floating arrow commands (10/11/12 = off/left/right).
@ $C01F label=rm_hazards_set_arrow_command
C $C01F,5 floating_arrow = A - 7
C $C024,2 Jump to rm_do_restart if zero
C $C026,3 correct_fork = A
@ $C029 label=rm_do_restart
C $C02A,2 Jump to rm_restart_hazards_read
N $C02C Handle hazard commands (3 = off, 4/5/6 = tumbleweeds left/right/both, 7/8/9 => barriers)
N $C02C When on left it uses lane 1, on right it uses lane 4, both it uses lane 2 and 3.
@ $C02C label=rm_hazards_set_hazard_command
C $C02C,3 Self modify "LD (HL),$xx" at $C058 to load A  [which is command_no-3]
C $C02F,2 Jump to rm_read_hazards
@ $C031 label=rm_hazards_road_fork_command
C $C031,4 Load address of left route's hazards data  -- Sampled HL: $5F26 (road split)
C $C035,4 Self modify 'LD HL,$xxxx' at $BB8A
C $C039,4 Load address of right route's hazards data
C $C03D,3 Self modify 'LD HL,$xxxx' at $BBB7
C $C040,3 HL = $E2B8
C $C043,2 Jump to rm_read_hazards
@ $C045 label=rm_hazards_one_command
C $C045,3 HL = 0
C $C048,2 Jump to rm_read_hazards
@ $C04A label=rm_hazards_jump_command
C $C04A,4 HL = wordat(HL)  [Sampled HL: $6292 $6107 $61BC $600F]
@ $C04E label=rm_read_hazards
C $C04E,1 #REGde is now hazards address
C $C04F,1 Read a hazards byte (regular, not an escape)
@ $C050 label=rm_hazards_regular_byte
C $C050,4 road_hazard_ptr = DE  -- Update hazard objects ptr
C $C054,1 A--
@ $C055 label=rm_no_hazards
C $C055,3 hazards_byte = A   -- Update hazards byte
C $C058,2 *HL = 0  [cyclic road buffer]
C $C05A,2 A = 2
@ $C05C label=rm_all_hazards
C $C05C,3 Set no_objects_counter to #REGa
C $C05F,3 Call $B8D2
C $C062,4 IX = &hazards
C $C066,3 DE = 20  -- array entry stride
C $C069,3 B = 6, C = 0
@ $C06C label=rm_c06c
C $C06C,4 test the used flag
C $C070,2 jump if used
@ $C072 label=rm_c072_continue
C $C072,2 IX += DE
C $C074,2 Loop while #REGb
C $C076,1 A = C  -- A = 0?
C $C077,3 HL = $A22B
C $C07A,3 HL = $A22B  -- duplicate instruction
C $C07D,1 *HL = A
C $C07E,2 Jump to rm_c0bb
@ $C080 label=rm_c080
C $C080,4 A = IX[15] + 1
C $C084,2 Jump to rm_c096 if zero
C $C086,3 IX[1]--
C $C089,3 Jump to rm_c072_continue if non-zero
C $C08C,4 IX[0] = 0  -- mark the hazard entry unused
C $C090,1 testing top bit of A
C $C091,2 Jump to rm_c072_continue if no carry
C $C093,1 C++
C $C094,2 Jump to rm_c072_continue
@ $C096 label=rm_c096
C $C096,5 A = IX[1] - 1
C $C09B,3 IX[1] = A
C $C09E,2 Jump to rm_c0b2 if carry
C $C0A0,2 Jump to rm_c072_continue if non-zero
C $C0A2,3 A = IX[17]
C $C0A5,1 Set flags
C $C0A6,2 Jump to rm_c072_continue if non-zero
C $C0A8,4 IX[1] = 1
C $C0AC,4 IX[4] = 255
C $C0B0,2 Jump to rm_c072_continue
@ $C0B2 label=rm_c0b2
C $C0B2,7 IX[17]--
C $C0B9,2 Jump to rm_c072_continue
@ $C0BB label=rm_c0bb
C $C0BB,2 A = xx  -- Self modified by $A977 + $A9A0 only
C $C0BD,1 Set flags
C $C0BE,3 Jump to rm_c0d7 if zero
C $C0C1,3 HL = $ED73  -- This must be a buffer pointer at this point
C $C0C4,3 DE = $ED77
C $C0C7,3 BC = 38
@ $C0CA label=rm_c0ca
C $C0CA,2 HL -= 2
C $C0CC,2 DE -= 2
C $C0CE,2 *DE-- = *HL--; BC--
C $C0D0,2 *DE-- = *HL--; BC--
C $C0D2,3 Jump to rm_c0ca if PE  PE => B became zero
C $C0D5,1 HL++
C $C0D6,1 *HL = B
@ $C0D7 label=rm_c0d7
C $C0D7,2 A = 1
@ $C0D9 label=rm_c0d9
C $C0D9,5 *$A254 += A  -- spawning flag
C $C0DE,3 Exit via $AD0D
c $C0E1 Routine at C0E1
D $C0E1 Used by the routines at #R$8401, #R$852A and #R$873C.
@ $C0E1 label=tunnel
C $C0E1,3 Read 'LD A' at $C160 (tunnel drawing code)
C $C0E4,1 Set flags
C $C0E5,3 Read 'LD A' at $C88F (TBD)
C $C0E8,2 Jump if tunnel has appeared
N $C0EA Tunnel hasn't appeared.
C $C0EA,1 Set flags
C $C0ED,4 tunnel_sfx = 0
C $C0F1,2 #REGhl = 0
C $C0F3,3 $8F82 = NOP (instruction)
C $C0F6,3 $8F83 & $8F84 = NOP (instruction)
C $C0F9,3 $8FA7 = NOP (instruction)
C $C0FC,3 $8FA8 & $8FA9 = NOP (instruction)
C $C0FF,1 Return
N $C100 Tunnel has appeared.
C $C100,5 tunnel_sfx = 5  -- this quietens noises when in the tunnel
C $C105,3 somewhere in road height data table
C $C108,6 #REGbc = wordat(HL); #REGhl -= 4
C $C10E,1 Bank
C $C10F,3 somewhere in road height data table
C $C112,6 #REGde = wordat(HL); #REGhl -= 4
@ $C118 label=tunnel_loop
C $C118,1 Bank
C $C119,3 #REGde = wordat(HL); #REGhl--
C $C11C,1 Stack #REGde
C $C11D,3 #REGhl -= 3
C $C121,2 #REGhl -= #REGbc
C $C123,1 Unstack
C $C124,2 Jump if #REGhl < #REGbc
C $C127,1 Bank
C $C128,6 #REGbc = wordat(HL); #REGhl -= 4
C $C12F,2 #REGhl -= #REGbc
C $C131,2 Jump if #REGhl < BC
C $C134,2 #REGde = #REGbc
C $C136,1 A--
C $C137,2 Loop to tunnel_loop
C $C139,3 A = 9 - A
C $C13C,3 [15 when tunnel is small, 6 when fills screen]
C $C15A,1 Return
c $C15B Tunnel entrance/exit drawing code
C $C1E6,3 A = fast_counter
C $C221,21 Jump table (self modified)
C $C236,177 Jump table (self modified)
c $C2E7 Routine at C2E7
D $C2E7 Used by the routine at #R$C452.
C $C33D,3 A = fast_counter
C $C3B5,3 A = fast_counter
c $C452 Landscape related
D $C452 Used by the routines at #R$8401, #R$852A and #R$873C.
@ $C452 label=main_loop_13
C $C452,5 Self modify #REGsp restore instruction
C $C457,3 $A248 = 0
C $C45A,3 Self modify 'LD A' at $C160 to be zero (tunnel drawing code)
C $C45D,3 Self modify 'LD A' at $C88F to be zero
C $C460,5 Self modify 'LD A' at $C6D8 to be 3
C $C465,4 #REGiy = $E301
C $C469,6 C = $60 - IY[0]
C $C46F,5 A = road_buffer_offset + $40
C $C474,5 IX = $EE00 + A
C $C479,3 B = A
C $C47C,3 $C6B3 = A & 1
C $C47F,4 RR B twice
C $C483,3 HL = $D010  doubtful that this is a pointer
C $C486,1 A = $D0
C $C487,2 B = $55 -- Set this to $00 and the landscape goes blank - half the time
C $C489,2 If B carried out earlier then jump
C $C48B,3 HL = $0030
C $C48E,1 B = $00
@ $C48F label=j_c48f
C $C48F,3 Self modify XOR at $C6D3 to be zero [OR $D0 -- check other code again]
C $C492,1 A = $00
C $C493,3 Self modify ADD A at $C677
C $C496,1 A = $30
C $C497,3 Self modify ADD A at $C651 to be $30
C $C49A,1 A = $31
C $C49B,3 Self modify ADD A at $C698 to be $31
C $C49E,3 HL = $C534
C $C4A1,3 Self modify JP Z at $C4B2 to be $C534
C $C4A4,2 L = $FF
C $C4A6,3 DE = $0100
C $C4A9,1 A = B
C $C4AA,3 Self modify ADD A at $C6BC to be zero?
N $C4AD This entry point is used by the routine at #R$C598.
N $C54D This entry point is used by the routine at #R$C2E7.
N $C565 This entry point is used by the routine at #R$C58A.
C $C579,3 }
c $C57C smells like scanline/buffer pointer movement
D $C57C Used by the routine at #R$C598.
C $C57C,4 E -= 32
C $C580,3 jump if it went -ve
C $C583,4 D += 16
C $C587,3 jump
c $C58A ditto
D $C58A Used by the routine at #R$C452.
C $C58A,4 E -= 32
C $C58E,3 jump if it went -ve
C $C591,4 D += 16
C $C595,3 jump
c $C598 Road plotting
D $C598 Used by the routine at #R$C452.
N $C5A7 This entry point is used by the routine at #R$C57C.
C $C60A,2 Jump table (self modified)
C $C61B,2 Jump table (self modified)
C $C62C,2 Jump table (self modified)
N $C62E This entry point is used by the routine at #R$C452.
N $C79A This entry point is used by the routines at #R$CBA4 and #R$CBC5.
C $C7EA,3 Point at first hill backdrop
C $C7EF,3 Point at second hill backdrop
C $C813,4 E -= 32
C $C817,3 Jump if E < 32
C $C81A,4 D -= 16
N $C821 This entry point is used by the routine at #R$C86E.
N $C824 This entry point is used by the routine at #R$C598.
N $C82A This entry point is used by the routine at #R$C813.
c $C86E Routine at C86E
N $C887 This entry point is used by the routine at #R$C598.
c $C8BE Builds the pre-shifted first hill backdrop
D $C8BE Used by the routine at #R$87DC.
@ $C8BE label=pre_shift_backdrop
C $C8BE,3 Point at second hill backdrop
C $C8C1,3 Point at first hill backdrop
C $C8C6,3 Each is 250 bytes long
C $C8C9,2 Copy 2nd to first
C $C8CC,3 Jump 10 bytes fwd
C $C8D1,1 pop old DE
C $C8D2,3 10 x 24
C $C8D7,2 Nibble swap thing
c $C8E3 Routine at C8E3
D $C8E3 Used by the routine at #R$C452.
C $C8E4,3 Read 'LD A' at $C6D8
C $C8E7,3 Self modify 'LD A' at $CB65
C $C905,3 Self modify "ADD A,x" at $CADC
C $C90C,3 Self modify "ADD A,x" at $CABB
N $C915 This entry point is used by the routines at #R$CBA4 and #R$CBC5.
N $C929 This entry point is used by the routine at #R$C94C.
C $C93E,4 E -= 32
C $C942,3 Jump if E < 32
C $C945,4 D -= 16
C $C94C,4 E -= 32
C $C950,3 Jump if E < 32
C $C953,4 D -= 16
C $C95B,6 Self modify 'JP NZ' at $CB31 to be $C963
C $C963,1 A = D
C $C964,1 D--
C $C965,2 A &= $0F
C $C969,4 Self modify 'LD DE' at $CA00 to load current DE
C $C970,3 B = 16, C = $F8 (mask)
C $C973,2 HL = $E8xx
C $C977,3 (A & mask) == 0
C $C9B2,1 D = A
C $C9B3,3 A = ~(A - E) + B
C $C9B6,3 Self modify jump table target
C $C9B9,1 H++
C $C9BA,1 A = *HL
C $C9BB,1 Set flags
C $C9C9,3 A = HL[-1]
C $C9CC,1 A &= C
C $C9CD,3 A >>= 3
C $C9D8,1 E = A
C $C9D9,1 A -= D
C $C9DA,1 A = ~A
C $C9DB,1 A += B
C $C9DF,1 H += 2
C $C9E1,1 A = *HL
C $CA11,17 Jump table (self modified)
C $CA22,17 Jump table (self modified)
C $CA33,17 Jump table (self modified)
C $CA44,17 Jump table (self modified)
C $CA55,2 Jump table (self modified)
N $CA57 This entry point is used by the routine at #R$C8E3.
C $CA66,1 B = E
C $CA67,1 C--
C $CA68,2 H = $E4
C $CA6A,1 Bank/unbank
C $CA6B,2 H = $E8
C $CA6D,1 A = *HL
C $CA6E,1 Set flags
C $CA6F,2 Jump if non-zero
C $CA71,3 A = HL[-1]
C $CA74,1 Bank/unbank
C $CA75,1 E = A
C $CA76,2 A &= 7
C $CA78,2 A = A ROL 2
C $CA7A,2 A += <self modified>
C $CA7C,1 L = A
C $CA7D,1 A = E
C $CA7E,3 A = A (rotate right through carry) 3
C $CA81,3 A = (A & 31) + B
C $CA84,1 E = A
C $CA85,1 A = *DE
C $CA86,1 A = A & *HL
C $CA87,1 L++
C $CA88,1 A |= *HL
C $CA89,1 *DE = A
C $CA8A,2 L += 2
C $CA8C,1 E++
C $CA8D,2 *DE++ = *HL++, BC--
C $CA8F,1 Bank/unbank
N $CA90 Similar chunk to above
C $CA90,1 H++
C $CA91,1 A = *HL
C $CA92,1 Set flags
C $CA93,2 Jump if non-zero
C $CA95,3 A = HL[-1]
C $CA98,1 Bank/unbank
C $CA99,1 E = A
C $CA9A,2 A &= 7
C $CA9C,1 A = A ROL 1
C $CA9D,2 A += <self modified>
C $CA9F,1 L = A
C $CAA0,1 A = E
C $CAA1,3 A = A (rotate right through carry) 3
C $CAA4,3 A = (A & 31) + B
C $CAA7,1 E = A
C $CAA8,2 *DE++ = *HL++, BC--
C $CAAA,1 A = *HL
C $CAAB,1 *DE = A
C $CAAC,1 Bank/unbank
N $CAAD Similar chunk again to above... different rotates
C $CAAD,1 H++
C $CAAE,1 A = *HL
C $CAAF,1 Set flags
C $CAB0,2 Jump if non-zero
C $CAB2,3 A = HL[-1]
C $CAB5,1 Bank/unbank
C $CAB6,1 E = A
C $CAB7,2 A &= 7
C $CAB9,2 A = A ROL 2
C $CABB,2 A += <self modified>
C $CABD,1 L = A
C $CABE,1 A = E
C $CABF,3 A = A (rotate right through carry) 3
C $CAC2,3 A = (A & 31) + B
C $CAC5,1 E = A
C $CAC6,2 *DE++ = *HL++, BC--
C $CAC8,2 A = *DE & *HL
C $CACA,1 L++
C $CACB,1 A |= *HL
C $CACC,1 *DE = A
C $CACD,1 Bank/unbank
N $CACE Similar chunk again
C $CACE,1 H++
C $CACF,1 A = *HL
C $CAD0,1 Set flags
C $CAD1,2 Jump if non-zero
C $CAD3,3 A = HL[-1]
C $CAD6,1 Bank/unbank
C $CAD7,1 E = A
C $CAD8,2 A &= 7
C $CADA,2 A = A ROL 2
C $CADC,2 A += <self modified>
C $CADE,1 L = A
C $CADF,1 A = E
C $CAE0,3 A = A (rotate right through carry) 3
C $CAE3,3 A = (A & 31) + B
C $CAE6,1 E = A
C $CAE7,2 A = *DE & *HL
C $CAE9,1 L++
C $CAEA,1 A |= *HL
C $CAEB,1 *DE = A
C $CAEC,2 L += 2
C $CAEE,1 E++
C $CAEF,2 *DE++ = *HL++, BC--
C $CAF1,1 Bank/unbank
N $CAF2 Similar chunk again
C $CB48,8 Self modify "LD A,x" at $CA7A
C $CB50,3 Self modify "ADD A,x" at $CADC
C $CB53,1 A++
C $CB57,3 Self modify "ADD A,x" at $CABB
C $CB5A,8 Self modify "LD A,x" at $CA9D [nested self modification - INCEPTION ACHIEVED]
C $CB6E,10 Self modify "XOR x" at $CB5D
C $CB79,10 self modified chunk Jump if zero
C $CB83,8 Self modify "LD A,x" at $CA7A
C $CBA1,3 }
c $CBA4 Routine at CBA4
c $CBC5 Routine at CBC5
D $CBC5 Used by the routine at #R$C95A.
C $CBC8,3 Jump if A < 80
c $CBCE Routine at CBCE - road drawing stuff
D $CBCE Used by the routine at #R$B9F4.
C $CBCE,3 Load two high bytes ($EExx table offset, $ECxx table offset)
C $CBD1,3 $ED $44 => NEG instr for $CC21 & $CC22
N $CBD6 This entry point is used by the routine at #R$B9F4.
C $CBD6,3 Load two high bytes ($ED00 table offset, $E900 table offset)
C $CBD9,3 Pair of NOPs for $CC21 & $CC22
N $CBDC This entry point is used by the routine at #R$CBCE.
C $CBDC,4 Write instructions in #REGde to $CC21 & $CC22
C $CBE0,4 Self modify 'LD HL' at $CC70 to load ($ED00 + H)
C $CBE4,4 Self modify 'LD HL' at $CCA5 to load ($E900 + L)
C $CBE8,3 Load road_buffer_offset
C $CBEB,1 Read road buffer byte
C $CBEC,3 A = fast_counter
C $CBEF,2 take top three bits
C $CBF2,5 A -= (B >> 2)
C $CBF7,5 A -= (B >> 4)
C $CBFC,10 #REGiy = $E600 + A + $B0
C $CC06,3 Call multiply (A = multiplier, C = multiplicand)
C $CC09,6 A = (128 - A) & $FE
C $CC0F,5 IX = $E500 + A  -- distance shift table
C $CC14,3 not a pointer, separate values
C $CC17,2 20 iterations
C $CC19,1 Bank
C $CC1A,4 Get road position
C $CC1F,1 Unbank
C $CC20,1 Read road buffer byte
C $CC21,2 Self modified above - Set to NOP or NEG
C $CC23,1 Advance road buffer pointer (wrapping)
C $CC24,1 Bank
C $CC2C,3 Reads from the $E540 table (distance shift table)
N $CC2F Subtract DE from table entry, result in BC.
C $CC2F,1 A -= E
C $CC30,1 C = A
C $CC31,3 high byte
C $CC34,1 A -= D
C $CC35,1 B = A
C $CC36,3 IY seems to point to $E6xx..E7xx
N $CC3B This is a multiplier of HL (distance shift value computed above) by A (value from $E600+).
C $CC3B,1 shift left
C $CC3C,2 top bit not set
C $CC3E,2 HL = BC  copy of distance shift value
C $CC40,1 shift result left. HL seems to be zeroed above, so this is confusing
C $CC41,1 shift topmost bit out
C $CC42,2 jump if top bit not set
C $CC44,1 otherwise HL += BC
C $CC45,5 repeat
C $CC4A,5 repeat
C $CC4F,5 repeat
C $CC54,5 repeat
C $CC59,1 A = H
C $CC5A,2 L <<= 1
C $CC5C,2 H = 0
C $CC5E,1 A += H + carry
C $CC5F,1 L = A
C $CC60,1 A <<= 1
C $CC61,2 jump if top bit not set
C $CC63,1 H--
C $CC64,1 A >>= 1
C $CC67,1 Unbank
C $CC68,2 *DE++ = A
C $CC6A,2 Loop back to read buffer byte bit
C $CC6F,1 Bank
C $CC73,3 subroutine
C $CC76,3 A = fast_counter
C $CC79,2 take top three bits
C $CC7B,11 same pattern as above
C $CC88,3 HL = $E700 | A
C $CC8B,2 Jump if A+$60 had no carry
C $CC8D,1 Add carry otherwise
C $CC8E,3 [points to presumed lanes data in pristine memory map but must be something else]
C $CC91,2 B = 20
C $CC93,3 *DE += *HL
C $CC96,1 HL++
C $CC97,1 E++
C $CC98,2 Loop while B
C $CC9A,7 HL = road position - 295
C $CCA1,1 swap
C $CCA2,2 B = 0
C $CCA4,1 Unbank
C $CCA5,3 table?
C $CCA8,4 table?
C $CCAC,2 B = 21
C $CCAE,4 Save #REGsp to restore later (self modify)
C $CCB2,1 Put address in #REGsp (so we can use POP for speed)
C $CCB3,1 Bank
C $CCB4,3 A = B - 2
C $CCB7,3 A += *IY
C $CCBA,2 IY++
C $CCBC,3 A -= *IY
C $CCC2,2 A += 2
C $CCC4,3 IY[$4E] = A
C $CCC7,1 A -= B
C $CCC8,1 C = A
C $CCC9,1 B = A
C $CCCA,3 L = IY[$1F]
C $CCCD,2 test bit 7 of L
C $CCCF,2 jump if ?
C $CCD1,4 L = -L
C $CCD5,1 A = B
C $CCD6,1 A < L ?
C $CCD7,2 A = <DEC DE>
C $CCD9,2 jump if A < L
C $CCDE,1 A < L ?
C $CCDF,2 A = <INC DE>
C $CCE1,2 jump if A < L
C $CCE3,3 Self modify instruction below
C $CCE6,1 A = B
C $CCE7,1 A >>= 1
C $CCE8,1 A += L
C $CCE9,1 A < C ?
C $CCEA,2 jump if A < C
C $CCEC,1 A -= C
C $CCED,1 Self modified: INC DE or DEC DE
C $CCEF,2 Loop while B
C $CCF1,1 Unbank
C $CCF2,2 Outer loop?
C $CCF4,3 Restore original SP (must be self modified)
C $CCF7,1 Return
C $CCF8,3 Self modify instruction below
C $CCFB,1 A = 0
C $CCFC,1 Self modified: INC DE or DEC DE
C $CCFD,1 A += C
C $CCFE,2 jump if overflow
C $CD00,1 A < L ?
C $CD01,2 jump if A < L
C $CD03,1 A -= L
C $CD05,2 Loop while B
C $CD07,1 Bank
N $CD08 This entry point is used by the routine at #R$F220.
C $CD08,2 Outer loop?
C $CD0A,2 Exit
C $CD0C,4 IY[$4E] = 1
C $CD10,1 A++
C $CD13,1 A++
C $CD14,1 B = A
C $CD15,3 A = IY[$1F]
C $CD18,1 L = A
C $CD19,1 A <<= 1
C $CD1A,1 A = A - A - carry
C $CD1B,1 H = A
C $CD1C,1 HL += DE
C $CD1D,1 swap
C $CD1E,1 push
C $CD1F,1 Unbank
C $CD20,1 B--
C $CD21,3 Outer loop?
C $CD24,3 Exit
C $CD27,1 B = A
C $CD28,3 A = IY[$1F]
C $CD2B,1 L = A
C $CD2C,1 A <<= 1
C $CD2D,1 A = A - A - carry
C $CD2E,1 H = A
C $CD2F,1 HL += DE
C $CD30,1 swap
C $CD31,1 push
C $CD32,1 Bank
C $CD33,1 B--
C $CD34,3 Outer loop?
C $CD37,3 Exit
c $CD3A Routine at CD3A
D $CD3A Used by the routines at #R$8401, #R$852A and #R$873C. Point #REGiy at the road buffer height data.
@ $CD3A label=main_loop_10
C $CD3A,3 Set #REGiy high byte to road buffer memory region ($EExx)
C $CD3D,3 Load road_buffer_offset into #REGa
C $CD40,2 Add 32 so it's the height data offset
C $CD42,2 #REGiy is now the road buffer height data pointer
C $CD44,3 Read a height byte into #REGc
C $CD47,5 A = fast_counter & $E0
C $CD4C,1 L = A
C $CD4D,1 B = A
N $CD4E Reduces A by 31.25% ... unsure why that figure.
C $CD4E,4 Divide A by 4 and subtract
C $CD52,1 A -= L
C $CD53,4 Divide A by 16 and subtract
C $CD57,1 A -= L
C $CD58,4 HL = $E600 | (A + 1)
C $CD5C,1 A = B  multiplicand
C $CD5D,3 Call multiply (A = multiplier, C = multiplicand)
C $CD60,2 A = -A
C $CD62,1 C = A
C $CD63,1 Bank
C $CD64,2 B = $15
C $CD66,3 DE = $E301
@ $CD69 label=ml10_loop
C $CD69,1 Unbank
C $CD6A,3 E = *HL << 1
C $CD6D,1 Preserve HL
C $CD6E,2 D = 0
C $CD70,1 L = 0
C $CD71,1 H = 0
C $CD72,1 A = C  C is multiply result from above
C $CD73,3 A += *IY   height byte
C $CD76,1 C = A
C $CD7C,1 A = E
C $CD7D,1 E--
C $CD7E,2 A = -A
C $CD80,1 E = A
C $CD81,1 A = C
C $CD82,2 A = -A
C $CD84,2 A <<= 2
C $CD88,2 HL = DE
C $CD8A,1 HL *= 2
C $CD8B,1 A <<= 1
C $CD8E,1 HL += DE
C $CD8F,1 HL <<= 1
C $CD90,1 A <<= 1
C $CD93,1 HL += DE
C $CD94,1 HL <<= 1
C $CD95,1 A <<= 1
C $CD98,1 HL += DE
C $CD99,1 HL <<= 1
C $CD9A,1 A <<= 1
C $CD9D,1 HL += DE
C $CD9E,1 HL <<= 1
C $CD9F,1 A <<= 1
C $CDA2,1 HL += DE
C $CDA3,1 HL <<= 1
C $CDA4,1 A <<= 1
C $CDA7,1 HL += DE
C $CDA8,1 HL <<= 1
C $CDA9,1 A = H
@ $CDAA label=ml10_continue
C $CDAA,1 Restore HL
C $CDAB,1 A += *HL
C $CDAC,1 L++
C $CDAD,1 Bank
C $CDAE,1 *DE = A
C $CDAF,1 E++
C $CDB0,2 IYl++
C $CDB2,2 Loop to ml10_loop
C $CDB4,2 A = $A0
C $CDB6,1 *DE = A
@ $CDC0 label=ml10_loop2
C $CDC0,1 A = *DE
C $CDC5,1 C = A
C $CDC6,1 *HL = C
C $CDC7,1 L++
C $CDC8,1 E++
C $CDC9,2 Loop ml10_loop2 while B
C $CDCB,6 C = (C + 3) & $F8
C $CDD1,1 A -= *HL
C $CDD2,1 *HL = C
C $CDD3,1 L++
C $CDD4,1 *HL = A
C $CDD5,1 Return
c $CDD6 Multiplier
D $CDD6 Multiplies #REGc by the top three bits of #REGa then divides by 8 with rounding, on exit.
R $CDD6 Used by the routines at #R$CBD6 and #R$CD3A.
N $CDD6 I:A Multiplier (number to multiply by)  e.g. $A0, $E7, $20, $C0, $E6 I:C Multiplicand (value to multiply)    e.g. $05, $02, $05, $03, $FE O:A Result                              e.g. $03, $02, $01, $02, $FE
@ $CDD6 label=multiply
C $CDD6,2 3 iterations only
C $CDD8,1 Copy of multiplier to destroy
C $CDD9,1 Initialise total
@ $CDDA label=mult_loop
C $CDDA,2 Shift the most significant bit out of multiplier
C $CDDC,3 If the MSB was set then total += multiplicand
@ $CDDF label=mult_continue
C $CDDF,1 total <<= 1
C $CDE0,2 While iterations remain, goto mult_loop
C $CDE2,1 Undo final shift
C $CDE3,8 Divide by 8 with rounding
C $CDEB,1 Return
b $CDEC Data block at CDEC
D $CDEC $AF91 uses this.
B $CDEC,71,8*8,7
b $CE33 Used by $B4FD
W $CE33,24,6
B $CE4B,95,8*2,3,8*2,3,8*2,3,8*2,3,8*2,3
b $CEAA Used by $B578
B $CEAA,48,8
b $CEDA Hero car drawing instructions
D $CEDA 9 of them. 20 bytes per entry.
@ $CEDA label=hero_car_refs
B $CEDA,1,1
B $CEDB,1,1 rows
W $CEDC,2,2 -> middle graphic
B $CEDE,1,1
B $CEDF,1,1 rows
W $CEE0,2,2 -> top graphic
B $CEE2,1,1
B $CEE3,1,1 rows
W $CEE4,2,2 -> bottom graphic
B $CEE6,1,1
B $CEE7,1,1 rows
W $CEE8,2,2 -> left graphic
B $CEEA,1,1
B $CEEB,1,1 rows
W $CEEC,2,2 -> right graphic
B $CEEE,1,1
B $CEEF,1,1 rows
W $CEF0,2,2 -> middle graphic
B $CEF2,1,1
B $CEF3,1,1 rows
W $CEF4,2,2 -> top graphic
B $CEF6,1,1
B $CEF7,1,1 rows
W $CEF8,2,2 -> bottom graphic
B $CEFA,1,1
B $CEFB,1,1 rows
W $CEFC,2,2 -> left graphic
B $CEFE,1,1
B $CEFF,1,1 rows
W $CF00,2,2 -> right graphic
B $CF02,1,1
B $CF03,1,1 rows
W $CF04,2,2 -> middle graphic
B $CF06,1,1
B $CF07,1,1 rows
W $CF08,2,2 -> top graphic
B $CF0A,1,1
B $CF0B,1,1 rows
W $CF0C,2,2 -> bottom graphic
B $CF0E,1,1
B $CF0F,1,1 rows
W $CF10,2,2 -> left graphic
B $CF12,1,1
B $CF13,1,1 rows
W $CF14,2,2 -> right graphic
B $CF16,1,1
B $CF17,1,1 rows
W $CF18,2,2 -> middle graphic
B $CF1A,1,1
B $CF1B,1,1 rows
W $CF1C,2,2 -> top graphic
B $CF1E,1,1
B $CF1F,1,1 rows
W $CF20,2,2 -> bottom graphic
B $CF22,1,1
B $CF23,1,1 rows
W $CF24,2,2 -> left graphic
B $CF26,1,1
B $CF27,1,1 rows
W $CF28,2,2 -> right graphic
B $CF2A,1,1
B $CF2B,1,1 rows
W $CF2C,2,2 -> middle graphic
B $CF2E,1,1
B $CF2F,1,1 rows
W $CF30,2,2 -> top graphic
B $CF32,1,1
B $CF33,1,1 rows
W $CF34,2,2 -> bottom graphic
B $CF36,1,1
B $CF37,1,1 rows
W $CF38,2,2 -> left graphic
B $CF3A,1,1
B $CF3B,1,1 rows
W $CF3C,2,2 -> right graphic
B $CF3E,1,1
B $CF3F,1,1 rows
W $CF40,2,2 -> middle graphic
B $CF42,1,1
B $CF43,1,1 rows
W $CF44,2,2 -> top graphic
B $CF46,1,1
B $CF47,1,1 rows
W $CF48,2,2 -> bottom graphic
B $CF4A,1,1
B $CF4B,1,1 rows
W $CF4C,2,2 -> left graphic
B $CF4E,1,1
B $CF4F,1,1 rows
W $CF50,2,2 -> right graphic
B $CF52,1,1
B $CF53,1,1 rows
W $CF54,2,2 -> middle graphic
B $CF56,1,1
B $CF57,1,1 rows
W $CF58,2,2 -> top graphic
B $CF5A,1,1
B $CF5B,1,1 rows
W $CF5C,2,2 -> bottom graphic
B $CF5E,1,1
B $CF5F,1,1 rows
W $CF60,2,2 -> left graphic
B $CF62,1,1
B $CF63,1,1 rows
W $CF64,2,2 -> right graphic
B $CF66,1,1
B $CF67,1,1 rows
W $CF68,2,2 -> middle graphic
B $CF6A,1,1
B $CF6B,1,1 rows
W $CF6C,2,2 -> top graphic
B $CF6E,1,1
B $CF6F,1,1 rows
W $CF70,2,2 -> bottom graphic
B $CF72,1,1
B $CF73,1,1 rows
W $CF74,2,2 -> left graphic
B $CF76,1,1
B $CF77,1,1 rows
W $CF78,2,2 -> right graphic
B $CF7A,1,1
B $CF7B,1,1 rows
W $CF7C,2,2 -> middle graphic
B $CF7E,1,1
B $CF7F,1,1 rows
W $CF80,2,2 -> top graphic
B $CF82,1,1
B $CF83,1,1 rows
W $CF84,2,2 -> bottom graphic
B $CF86,1,1
B $CF87,1,1 rows
W $CF88,2,2 -> left graphic
B $CF8A,1,1
B $CF8B,1,1 rows
W $CF8C,2,2 -> right graphic
N $CF8E Hero car adornments (shadow, smoke?)
@ $CF8E label=hero_car_shadow
B $CF8E,1,1
B $CF8F,1,1 rows
W $CF90,2,2 -> data
N $CF92 Hero car adornments (shadow, smoke?)
B $CF92,1,1
B $CF93,1,1 rows
W $CF94,2,2 -> data
N $CF96 Hero car adornments (shadow, smoke?)
B $CF96,1,1
B $CF97,1,1 rows
W $CF98,2,2 -> data
@ $CF9A label=hero_car_smoke
W $CF9A,4,2
W $CF9E,2,2 -> Turbo smoke plume data frame 1
W $CFA0,4,2
W $CFA4,2,2 -> Turbo smoke plume data frame 2
W $CFA6,4,2
W $CFAA,2,2 -> Turbo smoke plume data frame 3
W $CFAC,4,2
W $CFB0,2,2 -> Turbo smoke plume data frame 4
@ $CFB2 label=unknown_cfb2
B $CFB2,117,3 TBD groups of 3 bytes ref'd by $B6A6. 3rd byte is index into car_adornments.
@ $D027 label=car_adornments
B $D027,1,1 7 rows high
B $D028,1,1 1 byte wide
W $D029,2,2 -> Cherry light
B $D02B,1,1 14 rows high
B $D02C,1,1 3 bytes wide
W $D02D,2,2 -> Flashing cherry light
B $D02F,1,1 20 rows high
B $D030,1,1 3 bytes wide
W $D031,2,2 -> Crash/spark data
B $D033,1,1 4 rows high
B $D034,1,1 2 bytes wide
W $D035,2,2 -> Graphic data (can't tell)
B $D037,1,1 9 rows high
B $D038,1,1 2 bytes wide
W $D039,2,2 -> Debris
B $D03B,1,1 12 rows high
B $D03C,1,1 1 bytes wide
W $D03D,2,2 -> More debris (perhaps)
N $D03F Arrow graphic (24x21)
N $D03F #HTML[#CALL:graphic($D03F,24,21,1,1)]
B $D03F,126,6 Masked bitmap data
N $D0BD "HERE!" graphic (24x7)
N $D0BD #HTML[#CALL:graphic($D0BD,24,7,1,1)]
B $D0BD,42,6 Masked bitmap data
N $D0E7 Cherry light (sits on roof of car) (8x7)
N $D0E7 #HTML[#CALL:graphic($D0E7,8,7,1,1)]
B $D0E7,14,2 Masked bitmap data
N $D0F5 Illuminated cherry light (24x14)
N $D0F5 #HTML[#CALL:graphic($D0F5,24,14,1,1)]
B $D0F5,84,6 Masked bitmap data
N $D149 Crash/spark (24x20)
N $D149 #HTML[#CALL:graphic($D149,24,20,1,1)]
B $D149,120,6 Masked bitmap data
N $D1C1 Graphic (16x4)
N $D1C1 #HTML[#CALL:graphic($D1C1,16,4,1,1)] LOOKS WRONG
B $D1C1,16,4 Masked bitmap data
N $D1D1 Debris (16x9)
N $D1D1 #HTML[#CALL:graphic($D1D1,16,9,1,1)] LOOKS WRONG
B $D1D1,36,4 Masked bitmap data
N $D1F5 More debris (8x12)
N $D1F5 #HTML[#CALL:graphic($D1F5,8,12,1,1)] LOOKS WRONG
B $D1F5,24,2 Masked bitmap data
N $D20D Turbo smoke plume animation
N $D20D (32x16) masked per frame. 4 frames.
N $D20D #HTML[#CALL:anim($D20D,32,16,1,1,4)]
N $D20D Turbo smoke plume data frame 1
N $D20D #HTML[#CALL:graphic($D20D,32,16,1,1)]
B $D20D,128,8 Masked bitmap data
N $D28D Turbo smoke plume data frame 2
N $D28D #HTML[#CALL:graphic($D28D,32,16,1,1)]
B $D28D,128,8 Masked bitmap data
N $D30D Turbo smoke plume data frame 3
N $D30D #HTML[#CALL:graphic($D30D,32,16,1,1)]
B $D30D,128,8 Masked bitmap data
N $D38D Turbo smoke plume data frame 4
N $D38D #HTML[#CALL:graphic($D38D,32,16,1,1)]
B $D38D,128,8 Masked bitmap data
N $D40D Hero car graphics
N $D40D 9 sets
N $D40D Centre parts
N $D40D Straight (40x14)
N $D40D #HTML[#CALL:graphic($D40D,40,14,0,1)]
B $D40D,70,5 Non-masked, inverted bitmap data
N $D453 Straight + Turn right (40x17)
N $D453 #HTML[#CALL:graphic($D453,40,17,0,1)]
B $D453,85,5 Non-masked, inverted bitmap data
N $D4A8 Straight + Turn right hard (40x16)
N $D4A8 #HTML[#CALL:graphic($D4A8,40,16,0,1)]
B $D4A8,80,5 Non-masked, inverted bitmap data
N $D4F8 Up (40x14)
N $D4F8 #HTML[#CALL:graphic($D4F8,40,14,0,1)]
B $D4F8,70,5 Non-masked, inverted bitmap data
N $D53E Up + Turn right (40x17)
N $D53E #HTML[#CALL:graphic($D53E,40,17,0,1)]
B $D53E,85,5 Non-masked, inverted bitmap data
N $D593 Up + Turn right hard (40x15)
N $D593 #HTML[#CALL:graphic($D593,40,15,0,1)]
B $D593,75,5 Non-masked, inverted bitmap data
N $D5DE Down (40x14)
N $D5DE #HTML[#CALL:graphic($D5DE,40,14,0,1)]
B $D5DE,70,5 Non-masked, inverted bitmap data
N $D624 Down + Turn right (40x16)
N $D624 #HTML[#CALL:graphic($D624,40,16,0,1)]
B $D624,80,5 Non-masked, inverted bitmap data
N $D674 Down + Turn right hard (40x16)
N $D674 #HTML[#CALL:graphic($D674,40,16,0,1)]
B $D674,80,5 Non-masked, inverted bitmap data
N $D6C4 Straight parts
N $D6C4 Top of car (40x9)
N $D6C4 #HTML[#CALL:graphic($D6C4,40,9,1,1)]
B $D6C4,90,10 Masked, inverted bitmap data
N $D71E Bottom of car (40x6)
N $D71E #HTML[#CALL:graphic($D71E,40,6,1,1)]
B $D71E,60,10 Masked, inverted bitmap data
N $D75A Left of car (8x14)
N $D75A #HTML[#CALL:graphic($D75A,8,14,1,1)]
B $D75A,28,2 Masked, inverted bitmap data
N $D776 Right of car (8x14)
N $D776 #HTML[#CALL:graphic($D776,8,14,1,1)]
B $D776,28,2 Masked, inverted bitmap data
N $D792 Straight + Turn right parts
N $D792 Top of car (40x9)
N $D792 #HTML[#CALL:graphic($D792,40,8,1,1)]
B $D792,80,10 Masked, inverted bitmap data
N $D7E2 Bottom of car (40x6)
N $D7E2 #HTML[#CALL:graphic($D7E2,40,4,1,1)]
B $D7E2,40,10 Masked, inverted bitmap data
N $D80A Left of car (8x13)
N $D80A #HTML[#CALL:graphic($D80A,8,13,1,1)]
B $D80A,26,2 Masked, inverted bitmap data
N $D824 Right of car (8x13)
N $D824 #HTML[#CALL:graphic($D824,8,13,1,1)]
B $D824,26,2 Masked, inverted bitmap data
N $D83E Straight + Turn right hard parts
N $D83E Top of car (40x9)
N $D83E #HTML[#CALL:graphic($D83E,40,9,1,1)]
B $D83E,90,10 Masked, inverted bitmap data
N $D898 Bottom of car (40x4)
N $D898 #HTML[#CALL:graphic($D898,40,4,1,1)]
B $D898,40,10 Masked, inverted bitmap data
N $D8C0 Left of car (8x12)
N $D8C0 #HTML[#CALL:graphic($D8C0,8,12,1,1)]
B $D8C0,24,2 Masked, inverted bitmap data
N $D8D8 Right of car (8x15)
N $D8D8 #HTML[#CALL:graphic($D8D8,8,15,1,1)]
B $D8D8,30,2 Masked, inverted bitmap data
N $D8F6 Up parts
N $D8F6 Top of car (40x10)
N $D8F6 #HTML[#CALL:graphic($D8F6,40,10,1,1)]
B $D8F6,100,10 Masked, inverted bitmap data
N $D95A Bottom of car (40x6)
N $D95A #HTML[#CALL:graphic($D95A,40,6,1,1)]
B $D95A,60,10 Masked, inverted bitmap data
N $D996 Left of car (8x13)
N $D996 #HTML[#CALL:graphic($D996,8,13,1,1)]
B $D996,26,2 Masked, inverted bitmap data
N $D9B0 Right of car (8x14)
N $D9B0 #HTML[#CALL:graphic($D9B0,8,14,1,1)]
B $D9B0,28,2 Masked, inverted bitmap data
N $D9CC Up + Turn right parts
N $D9CC Top of car (40x9)
N $D9CC #HTML[#CALL:graphic($D9CC,40,9,1,1)]
B $D9CC,90,10 Masked, inverted bitmap data
N $DA26 Bottom of car (40x4)
N $DA26 #HTML[#CALL:graphic($DA26,40,4,1,1)]
B $DA26,40,10 Masked, inverted bitmap data
N $DA4E Left of car (8x12)
N $DA4E #HTML[#CALL:graphic($DA4E,8,12,1,1)]
B $DA4E,24,2 Masked, inverted bitmap data
N $DA66 Right of car (8x14)
N $DA66 #HTML[#CALL:graphic($DA66,8,14,1,1)]
B $DA66,28,2 Masked, inverted bitmap data
N $DA82 Up + Turn right hard parts
N $DA82 Top of car (40x9)
N $DA82 #HTML[#CALL:graphic($DA82,40,9,1,1)]
B $DA82,90,10 Masked, inverted bitmap data
N $DADC Bottom of car (40x6)
N $DADC #HTML[#CALL:graphic($DADC,40,6,1,1)]
B $DADC,60,10 Masked, inverted bitmap data
N $DB18 Left of car (8x11)
N $DB18 #HTML[#CALL:graphic($DB18,8,11,1,1)]
B $DB18,22,2 Masked, inverted bitmap data
N $DB2E Right of car (8x15)
N $DB2E #HTML[#CALL:graphic($DB2E,8,15,1,1)]
B $DB2E,30,2 Masked, inverted bitmap data
N $DB4C Down parts
N $DB4C Top of car (40x8)
N $DB4C #HTML[#CALL:graphic($DB4C,40,8,1,1)]
B $DB4C,80,10 Masked, inverted bitmap data
N $DB9C Bottom of car (40x6)
N $DB9C #HTML[#CALL:graphic($DB9C,40,6,1,1)]
B $DB9C,60,10 Masked, inverted bitmap data
N $DBD8 Left of car (8x13)
N $DBD8 #HTML[#CALL:graphic($DBD8,8,13,1,1)]
B $DBD8,26,2 Masked, inverted bitmap data
N $DBF2 Right of car (8x14)
N $DBF2 #HTML[#CALL:graphic($DBF2,8,14,1,1)]
B $DBF2,28,2 Masked, inverted bitmap data
N $DC0E Down + Turn right parts
N $DC0E Top of car (40x8)
N $DC0E #HTML[#CALL:graphic($DC0E,40,8,1,1)]
B $DC0E,80,10 Masked, inverted bitmap data
N $DC5E Bottom of car (40x4)
N $DC5E #HTML[#CALL:graphic($DC5E,40,4,1,1)]
B $DC5E,40,10 Masked, inverted bitmap data
N $DC86 Left of car (8x13)
N $DC86 #HTML[#CALL:graphic($DC86,8,13,1,1)]
B $DC86,26,2 Masked, inverted bitmap data
N $DCA0 Right of car (8x13)
N $DCA0 #HTML[#CALL:graphic($DCA0,8,13,1,1)]
B $DCA0,26,2 Masked, inverted bitmap data
N $DCBA Down + Turn right hard parts
N $DCBA Top of car (40x8)
N $DCBA #HTML[#CALL:graphic($DCBA,40,8,1,1)]
B $DCBA,80,10 Masked, inverted bitmap data
N $DD0A Bottom of car (40x4)
N $DD0A #HTML[#CALL:graphic($DD0A,40,4,1,1)]
B $DD0A,40,10 Masked, inverted bitmap data
N $DD32 Left of car (8x13)
N $DD32 #HTML[#CALL:graphic($DD32,8,13,1,1)]
B $DD32,26,2 Masked, inverted bitmap data
N $DD4C Right of car (8x15)
N $DD4C #HTML[#CALL:graphic($DD4C,8,15,1,1)]
B $DD4C,30,2 Masked, inverted bitmap data
N $DD6A Shadow parts
N $DD6A Shadow + Straight (56x12)
N $DD6A #HTML[#CALL:graphic($DD6A,56,12,1,1)]
B $DD6A,168,14 Masked, inverted bitmap data
N $DE12 Shadow + Turn right (56x12)
N $DE12 #HTML[#CALL:graphic($DE12,56,12,1,1)]
B $DE12,168,14 Masked, inverted bitmap data
N $DEBA Shadow + Turn right hard (56x12)
N $DEBA #HTML[#CALL:graphic($DEBA,56,12,1,1)]
B $DEBA,168,14 Masked, inverted bitmap data
b $DF62 LED style numeric font used for scores
D $DF62 8x15 pixels, digits 0..9 only
D $DF62 #HTML[#CALL:graphic($DF62,8,10*15,0,0)]
@ $DF62 label=ledfont
B $DF62,150,15
b $DFF8 Mini font used for in-game messages
D $DFF8 8x6 pixels, though the digits are thinner than 8, A-Z + five symbols.
D $DFF8 #HTML[#CALL:graphic($DFF8,8,31*6,0,0)]
@ $DFF8 label=minifont
B $DFF8,186,6
b $E0B2 Graphics defns
B $E0B2,301,7 <Byte width, Flags, Height, Ptr, Ptr>
@ $E1DF label=floating_arrow_defns
B $E1DF,10,5 <Byte width, Flags, Height, Ptr> [floating arrow defns]
b $E1E9 Ref'd by graphic entry 1 and 10
B $E1E9,187,8*23,3
b $E2A4 Data for perp escape scene
N $E2A4 Perp escape scene, hazards
B $E2A4,6,6
N $E2AA Perp escape scene, curvature
B $E2AA,1,1 Straight
B $E2AB,2,2 Escape, Command 0 (Continue at <Address>)
W $E2AD,2,2 Loop
N $E2AF Perp escape scene, height
B $E2AF,1,1 Level ground
B $E2B0,2,2 Escape, Command 0 (Continue at <Address>)
W $E2B2,2,2 Loop
N $E2B4 Perp escape scene, lanes
B $E2B4,1,1 Length byte ?
B $E2B5,1,1 Lanes byte ?
B $E2B6,6,1
B $E2BC,2,2 Escape, Command 0 (Continue at <Address>)
W $E2BE,2,2 Loop (partial)
N $E2C0 Perp escape scene, lane objects [not sure now, could be right curve road]
B $E2C0,2,1
B $E2C2,2,2 Escape, Command 0 (Continue at <Address>)
W $E2C4,2,2 Loop (partial)
N $E2C6 Road split, curvature
B $E2C6,1,1
B $E2C7,1,1 Curve left (67%)
B $E2C8,2,2 Escape, Command 0 (Continue at <Address>)
W $E2CA,2,2 Loop (partial)
N $E2CC Road split, height
B $E2CC,1,1
B $E2CD,2,2 Escape, Command 0 (Continue at <Address>)
W $E2CF,2,2 Loop
b $E2D1 Road split data
B $E2D1,1,1
B $E2D2,3,3 Ref'd by $BBEF  --  Used after road split
B $E2D5,4,4 rightside data  [flips around depending on who's accessing it]
B $E2D9,4,4 leftside data   [flips]
B $E2DD,4,4 Ref'd by $BBE3
B $E2E1,1,1 Curvature escape byte
B $E2E2,5,5 Ref'd by $BBE9
B $E2E7,12,8,4 lanes data
B $E2F3,88,8 lanes data
b $E34B Data block at E34B
B $E34B,3,3 3 bytes set to 8 by $880A
B $E34E,22,8*2,6
b $E364 Transition masks
N $E364 Spiral inward animation mask (8x8, 11 frames)
N $E364 #HTML[#CALL:anim($E364,8,8,0,0,11)]
N $E364 #HTML[#CALL:graphic($E364,8,11*8,0,0)]
B $E364,88,8
N $E3BC Circle expanding animation mask (8x8, 7 frames)
N $E3BC #HTML[#CALL:anim($E3BC,8,8,0,0,7)]
N $E3BC #HTML[#CALL:graphic($E3BC,8,7*8,0,0)]
B $E3BC,388,8*48,4
w $E540 A table of 96 words being 10^x or similar function. Distance horizontal shift? Field of view table? It affects the horizontal position of the road.
W $E540,192,2
b $E600 Data block at E600. Seems to be 24 groups of 22 bytes.
B $E600,528,22
c $E810 looks like initialisation code / relocation
C $E813,3 Call clear_game_attrs
C $E839,1 must be the loop counter
C $E83A,3 Point #REGhl at $e858 table
@ $E83D label=loopy
C $E83E,4 DE = wordat(HL) HL += 2
C $E842,1 Stack DE
C $E843,4 DE = wordat(HL) HL += 2 -- dest
C $E847,4 BC = wordat(HL) HL += 2 -- count
C $E84B,1 exchange top of stack and HL. HL is now src
C $E84C,2 Do copy
C $E84E,2 Restore base pointer and count
C $E850,2 Loop
C $E852,3 Set up the stack
C $E855,3 Start the game
@ $E858 label=e858_copy_blocks
W $E858,2,2 src ptr
W $E85A,2,2 dst ptr
W $E85C,2,2 count
W $E85E,2,2 src ptr
W $E860,2,2 dst ptr
W $E862,2,2 count
W $E864,2,2 src ptr
W $E866,2,2 dst ptr
W $E868,2,2 count
W $E86A,2,2 src ptr
W $E86C,2,2 dst ptr
W $E86E,2,2 count
W $E870,2,2 src ptr
W $E872,2,2 dst ptr
W $E874,2,2 count
B $E876,48,3
N $E8A6 Square zoom in animation mask (8x8, 5 frames)
N $E8A6 #HTML[#CALL:anim($E8A6,8,8,0,0,5)]
N $E8A6 #HTML[#CALL:graphic($E8A6,8,5*8,0,0)]
B $E8A6,40,8
N $E8CE Diamond zoom in animation mask (8x8, 6 frames)
N $E8CE #HTML[#CALL:anim($E8CE,8,8,0,0,6)]
N $E8CE #HTML[#CALL:graphic($E8CE,8,6*8,0,0)]
B $E8CE,60,8*7,4
W $E90A,2,2 -> "STOP THE TAPE" + "PRESS ANY KEY" message set
B $E90C,3,3
c $E90F Routine at E90F
C $E925,3 Call clear_screen
C $E928,3 Point #REGhl at input menu messages (NUL terminated)
C $E92B,6 Call menu_draw_strings
C $E931,9 Keyscan for 1, 2, 3, 4, 5
C $E93A,3 Keyscan for 0, 9, 8, 7, 6
C $E93D,3 Keyscan for P, O, I, U, Y
C $E940,3 Keyscan for ENTER, L, K, J, H
C $E943,25 Keyscan for SPACE, SYM SHFT, M, N, B
C $E95C,3 Address of three bytes to populate $A0CD.. with
C $E95F,4 Set Kempston flag
C $E963,6 Populate $A0CD
C $E969,7 Populate $A0D0
C $E970,3 Call clear_screen
C $E973,3 Address of "control options cannot be remodified" text (NUL terminated)
C $E976,7 Call menu_draw_strings
C $E97D,31 Keyscan
C $E99C,24 Jump to clear_screen
b $E9B4 Messages
B $E9B4,1,1 Attribute: Green ink over black
W $E9B5,2,2 Screen position (80,96) (why is this screen, not attrs?)
T $E9B7,13,12:n1 "STOP THE TAPE"
B $E9C4,1,1 Attribute: Cyan ink over black
W $E9C5,2,2 Screen attribute position (4,18)
T $E9C7,25,24:n1 "PRESS ANY KEY TO CONTINUE"
B $E9E0,1,1 Stop marker?
B $E9E1,1,1 Attribute: Green ink over black
W $E9E2,2,2 Screen position (88,80)
T $E9E4,11,10:n1 "CHASE  H.Q."
B $E9EF,1,1 Attribute: Cyan ink over black
W $E9F0,2,2 Screen position (48,112)
T $E9F2,20,19:n1 "1. SINCLAIR JOYSTICK"
B $EA06,1,1 Attribute: Cyan ink over black
W $EA07,2,2 Screen position (48,128)
T $EA09,18,17:n1 "2. CURSOR JOYSTICK"
B $EA1B,1,1 Attribute: Cyan ink over black
W $EA1C,2,2 Screen position (48,144)
T $EA1E,20,19:n1 "3. KEMPSTON JOYSTICK"
B $EA32,1,1 Attribute: Cyan ink over black
W $EA33,2,2 Screen position (48,160)
T $EA35,11,10:n1 "4. KEYBOARD"
B $EA40,1,1 Attribute: Cyan ink over black
W $EA41,2,2 Screen position (48,176)
T $EA43,14,13:n1 "5. DEFINE KEYS"
B $EA51,1,1 Stop marker?
B $EA52,1,1 Attribute: Red ink over black
W $EA53,2,2 Screen position (72,80)
T $EA55,14,13:n1 "REDEFINE  KEYS"
B $EA63,1,1 Attribute: Bright yellow ink over black + Single height bit
W $EA64,2,2 Screen position (72,112)
T $EA66,12,11:n1 "GEAR........"
B $EA72,1,1 Attribute: Bright yellow ink over black + Single height bit
W $EA73,2,2 Screen position (72,120)
T $EA75,12,11:n1 "ACCELERATE.."
B $EA81,1,1 Attribute: Bright yellow ink over black + Single height bit
W $EA82,2,2 Screen position (72,128)
T $EA84,12,11:n1 "BRAKE......."
B $EA90,1,1 Attribute: Bright yellow ink over black + Single height bit
W $EA91,2,2 Screen position (72,136)
T $EA93,12,11:n1 "LEFT........"
B $EA9F,1,1 Attribute: Bright yellow ink over black + Single height bit
W $EAA0,2,2 Screen position (72,144)
T $EAA2,12,11:n1 "RIGHT......."
B $EAAE,1,1 Attribute: Bright green ink over black + Single height bit
W $EAAF,2,2 Screen position (72,160)
T $EAB1,12,11:n1 "QUIT........"
B $EABD,1,1 Attribute: Bright green ink over black + Single height bit
W $EABE,2,2 Screen position (72,168)
T $EAC0,12,11:n1 "PAUSE......."
B $EACC,1,1 Attribute: Bright green ink over black + Single height bit
W $EACD,2,2 Screen position (72,176)
T $EACF,12,11:n1 "TURBO......."
B $EADB,1,1 Probably a stop marker
B $EADC,5,5 Seems to be an empty message structure
B $EAE1,1,1 Attribute: Bright blue over black + Single height bit
W $EAE2,2,2 Screen position (0,0)
T $EAE4,4,3:n1 "TEST"
B $EAE8,1,1 Attribute: Red ink over black
W $EAE9,2,2 Screen position (32,72)
T $EAEB,24,23:n1 "CHASE H.Q.     TEST MODE"
B $EB03,1,1 Attribute: Bright cyan ink over black + Single height bit
W $EB04,2,2 Screen position (0,128)
T $EB06,10,9:n1 "IN GAME..."
B $EB10,1,1 Attribute: Bright green ink over black + Single height bit
W $EB11,2,2 Screen position (8,144)
T $EB13,29,28:n1 "PRESS 1....... RESTART LEVEL."
B $EB30,1,1 Attribute: Bright green ink over black + Single height bit
W $EB31,2,2 Screen position (56,152)
T $EB33,20,19:n1 "2....... NEXT LEVEL."
B $EB47,1,1 Attribute: Bright green ink over black + Single height bit
W $EB48,2,2 Screen position (56,160)
T $EB4A,20,19:n1 "3....... END SCREEN."
B $EB5E,1,1 Attribute: Bright green ink over black + Single height bit
W $EB5F,2,2 Screen position (56,168)
T $EB61,22,21:n1 "4....... EXTRA CREDIT."
B $EB77,1,1
B $EB78,1,1 Attribute: Red
W $EB79,2,2 Screen position (88,80)
T $EB7B,11,10:n1 "CHASE  H.Q."
B $EB86,1,1 Attribute: Bright yellow ink over black + Single height bit
W $EB87,2,2 Screen position (16,112)
T $EB89,27,26:n1 "PLEASE NOTE CONTROL OPTIONS"
B $EBA4,1,1 Attribute: Bright yellow ink over black + Single height bit
W $EBA5,2,2 Screen position (40,120)
T $EBA7,21,20:n1 "CANNOT BE REMODIFIED."
B $EBBC,1,1 Attribute: Bright cyan ink over black + Single height bit
W $EBBD,2,2 Screen position (8,144)
T $EBBF,31,30:n1 "ARE YOU HAPPY WITH YOUR CHOICE."
B $EBDE,1,1 Attribute: White
W $EBDF,2,2 Screen position (40,160)
T $EBE1,21,20:n1 "PRESS YES(Y) OR NO(N)"
B $EBF6,1,1
c $EBF7 Renders strings until it hits a NUL byte
D $EBF7 Used by the routines at #R$E90F and #R$ECF3.
@ $EBF7 label=menu_draw_strings
C $EBF7,3 Call menu_draw_string
C $EBFA,3 If the next byte's zero then return
C $EBFD,2 Otherwise loop
c $EBFF Renders a string.
D $EBFF Used by the routines at #R$EBF7 and #R$ED6D.
R $EBFF I:HL Address of a message structure (byte: attribute byte, word: destination screen address, bytes: top bit set terminated ASCII string)
R $EBFF O:HL Address of next unconsumed byte
@ $EBFF label=menu_draw_string
C $EBFF,1 Load attribute byte
C $EC00,5 Bank the top bit of the attribute byte as the C flag (this is the single height flag)
C $EC05,1 Advance
C $EC06,4 Load screen address into #REGde
C $EC0A,1 Stack screen address
N $EC0B Calculate the attribute address from the screen address
C $EC0B,9 #REGh = $58 + ((D >> 3) & 3)
C $EC14,1 #REGl = E
N $EC16 #REGhl' is screen address, #REGde' is attribute address
C $EC16,1 Get screen address and preserve old #REGhl
C $EC17,2 Preserve regs
@ $EC19 label=menu_draw_string_loop
C $EC19,3 Mask off the terminator bit
C $EC1C,1 #REGa = Character to draw
C $EC1D,3 Draw the character
C $EC21,2 End of string?
C $EC23,1 Advance to next char irrespective
C $EC24,2 Loop if not
C $EC27,3 Restore regs
C $EC2B,1 Return
c $EC2C Renders a single character. Compare #R$9FB4
D $EC2C Used by the routine at #R$EBFF.
R $EC2C I:A The character to plot (ASCII)
R $EC2C I:F' Carry flag set to draw single height characters
R $EC2C I:DE' Screen address (UDG aligned)
R $EC2C I:HL' Attribute address
@ $EC2C label=menu_draw_char
C $EC2C,6 Handle spaces
C $EC32,3 Advance attribute address
C $EC35,2 Map character ranges:
C $EC37,4 >= 'A' then glyph offset C=(65-32-18)=15
C $EC3B,6 >= '0' then glyph offset C=(48-32-11)=5
C $EC41,5 == '!' then glyph offset C=0
C $EC46,5 == '(' then glyph offset C=1
C $EC4B,4 == ')' then glyph offset C=2
C $EC4F,5 == ',' then glyph offset C=3
C $EC54,3 Anything else becomes a full stop
C $EC57,2 ASCII - 32 - offset in #REGc
C $EC59,6 Multiply #REGc by seven - the height of a glyph
C $EC5F,5 #REGbc = #REGa -- Why the RL?
C $EC64,3 Point #REGhl at 8x7 font
C $EC67,7 Point at glyph
C $EC6E,3 If the carry flag is set then draw single height characters
@ $EC71 label=menu_draw_char_double_height
C $EC71,1 Load a row of glyph
C $EC72,1 Put it on the screen
C $EC73,1 Move to next scanline
C $EC74,2 Put another row
C $EC76,1 Undo LDI
C $EC77,1 Move to next scanline
C $EC78,19 Repeat ...
C $EC8B,7 Move to next scanline after boundary (DE += $F81F)
C $EC92,22 Repeat ...
C $ECA8,3 ...
C $ECAB,1 Save #REGl
C $ECAC,2 Set the BRIGHT bit
C $ECAE,1 Set the screen attribute
C $ECAF,4 Move to the next attribute row
C $ECB3,2 Clear the BRIGHT bit
C $ECB5,1 Set the screen attribute
C $ECB6,1 Restore #REGl
C $ECB7,1 Advance cursor
C $ECB9,1 Return
@ $ECBA label==menu_draw_char_single_height
C $ECBA,28 Plot 8x7 character  HL->character, DE->screen
C $ECD6,1 Set the screen attribute
C $ECD7,1 Advance cursor
C $ECD9,1 Return
c $ECDA Clears the screen.
D $ECDA Used by the routines at #R$E90F and #R$ECF3.
@ $ECDA label=clear_screen
C $ECDA,12 Wipe bottom 2/3rds of attributes
C $ECE6,12 Wipe bottom 2/3rds of pixels
C $ECF2,1 Return
c $ECF3 Redefine keys screen.
D $ECF3 Used by the routine at #R$E90F.
@ $ECF3 label=redefine_keys
C $ECF3,3 Call clear_screen
C $ECF6,3 Address of "REDEFINE KEYS" strings
C $ECF9,3 Call menu_draw_strings
@ $ED02 label=rdk_loop_1
C $ED02,3 Preserve registers
C $ED05,3 Call define_keys
C $ED08,3 Restore registers
N $ED0B Debounce?
C $ED0B,1 A = 0
C $ED0E,3 A = ~A & $1F
C $ED16,1 C++
C $ED17,1 HL++
C $ED18,2 Loop rdk_loop_1 while B
C $ED1A,2 B = 20
@ $ED1C label=rdk_loop_2
C $ED1C,1 Preserve
C $ED1D,3 Call define_keys
C $ED20,1 Restore
C $ED21,2 Loop rdk_loop_2 while B
N $ED23 Test keys are "SHOCKED<ENTER>".
C $ED23,2 B = 8
C $ED25,3 DE -> shocked bytes
C $ED28,3 HL -> $EE38
@ $ED2B label=rdk_loop_3
C $ED2B,1 A = *DE
C $ED2C,1 CP *HL
C $ED2D,1 HL++
C $ED2E,1 DE++
C $ED2F,1 Return if non-zero
C $ED30,2 Loop rdk_loop_3 while B
C $ED32,5 Set test mode flag
C $ED37,3 Call clear_screen
C $ED3A,3 Address of TEST MODE strings
C $ED3D,3 Call menu_draw_strings
C $ED40,3 Call define_keys
N $ED43 Debounce?
C $ED4B,2 Loop
c $ED4D Routine at ED4D
D $ED4D Used by the routine at #R$ED6D.
C $ED55,3 A = ~A & $1F
C $ED58,2 Jump to $ED66 if zero
C $ED5A,1 D++
C $ED5C,1 H = A
C $ED5D,1 A = E
C $ED5E,2 A -= 8
C $ED60,2 H >>= 1
C $ED65,1 D = A
C $ED66,1 E--
C $ED6B,1 Compare A to itself then exit?
C $ED6C,1 Return
c $ED6D Routine at ED6D
D $ED6D Used by the routine at #R$ECF3.
C $ED6F,3 Call define_keys
C $ED77,1 D++
C $ED7A,1 D--
C $ED7B,1 A = D
C $ED81,1 B = C
C $ED82,1 B--
C $ED88,1 HL++
C $ED8D,3 Last byte of 'shocked'
C $ED90,2 B = 0
C $ED92,1 HL += BC
C $ED93,1 *HL = A
C $ED94,3 -> "B N M ..."
C $ED97,2 D = 0
C $ED99,3 B = A & 7
C $ED9C,4 A *= 5
C $EDA0,6 B >>= 3
C $EDA6,1 A += B
C $EDA7,1 A += A
C $EDA8,1 E = A
C $EDA9,1 HL += DE
C $EDB2,1 A = *HL
C $EDB6,1 HL++
C $EDB7,1 A = *HL
C $EDB8,2 Terminate the string
C $EDBA,3 Write new character
C $EDBD,3 -> test mode strings
C $EDC0,3 Call menu_draw_strings
C $EDC8,1 A = B
C $EDC9,3 Return if A != 4
C $EDCC,4 E += $20
C $EDD1,4 D += 8
C $EDD5,1 Return
t $EDD6 Names of keys
@ $EDD6 label=key_names
T $EDD6,10,10 $7F
T $EDE0,10,10 $BF
T $EDEA,10,10 $DF
T $EDF4,10,10 $EF
T $EDFE,10,10 $F7
T $EE08,10,10 $FB
T $EE12,10,10 $FD
T $EE1C,10,10 $FE
b $EE26 Sinclair joystick input scheme
B $EE26,5,5
b $EE2B Cursor joystick input scheme
B $EE2B,5,5
b $EE30 "SHOCKED<ENTER>" ?
B $EE30,8,8
b $EE38 temp input scheme buffer?
B $EE38,5,5
b $EE3D Data block at EE3D
B $EE3D,3,3
b $EE40 Data block at EE40
B $EE40,46,8*5,6
c $EE6E Routine at EE6E
D $EE6E Used by the routine at #R$EE9E.
@ $EE6E label=sub_EE6E
C $EE6E,2 Self modified by $EE7E
C $EE70,1 A--
C $EE71,3 Self modify $EE6E
C $EE74,1 Return if non-zero  -- so it's a delay?
C $EE75,3 Self modified by $EE83 [sampled: F10C, F10E, F102, F104, F106, F108, F10A, ]
@ $EE78 label=j_ee78
C $EE78,2 A = *HL++
C $EE7A,4 Jump to $EE98 if it's $FF
C $EE7E,3 Self modify $EE6E
C $EE81,2 C = *HL++
C $EE83,3 Self modify $EE75
C $EE86,2 B = 0
C $EE88,3 Address of music data
C $EE8B,1 HL += BC
C $EE8C,2 A = *HL++
C $EE8E,3 Self modify $EEB9
C $EE91,3 Self modify $EEAD
C $EE94,3 Self modify $EEC9
C $EE97,1 Return
C $EE98,4 HL = wordat(HL); HL++
C $EE9C,2 Goto j_ee78
c $EE9E Loads of self modifying hopping around...
D $EE9E Used by the routines at #R$E90F, #R$ECF3 and #R$ED6D.
@ $EE9E label=define_keys
C $EE9E,4 Enable wait/spinlock at $EF13
C $EEA2,2 Self modified by $EEA8
C $EEA4,1 Set flags
C $EEA5,2 Jump to $EEAD if non-zero
C $EEA7,1 A++
C $EEA8,3 Self modify $EEA2
C $EEAB,2 Jump to dk_eec9
C $EEAD,2 Self modified by $EEBB, cycles 5,3,2,1
C $EEAF,1 A--
C $EEB0,3 Jump to dk_eeb9 if zero
C $EEB3,3 Self modified by $EE8E
C $EEB6,3 Jump to dk_ef00
@ $EEB9 label=dk_eeb9
C $EEB9,2 Self modified by $EE8E
C $EEBB,3 Self modify 'LD A' @ $EEAD
C $EEBE,3 Self modified, cycles $F12x .. $F2xx ish
@ $EEC1 label=dk_eec1
C $EEC1,2 A = *HL - 1
C $EEC3,3 Jump to dk_eed2 if non-zero
C $EEC6,3 Call sub_EE6E
@ $EEC9 label=dk_eec9
C $EEC9,3 HL = xxxx     Self modified by $EE94
C $EECC,3 *$EEBF = HL
C $EECF,3 Goto dk_eec1
@ $EED2 label=dk_eed2
C $EED2,1 HL++
C $EED3,3 *$EEBF = HL
C $EED6,1 A++
C $EED7,2 >= 128?
C $EEDB,2 A &= $7F   note
C $EEDE,5 Self modify 'LD A' @ $EEAD
C $EEE3,3 Self modify 'LD A' @ $EF00
C $EEE7,1 D = A
C $EEE8,2 A &= 7
C $EEEA,2 Jump to dk_ef00 if zero
C $EEEC,1 B = A   the 'note'
C $EEED,1 A = D
C $EEEE,6 A >>= 3
C $EEF4,1 B--
C $EEF5,3 Jump to playdrum_2 if zero
C $EEF8,1 B--
C $EEF9,3 Jump to playdrum_1 if zero
C $EEFC,1 B--
C $EEFD,3 Jump to $F0C6 if zero
@ $EF00 label=dk_ef00
C $EF00,2 Self modified by $EEE3, $EF09
C $EF02,1 Set flags
C $EF03,2 Jump to dk_ef0d if zero
C $EF05,3 Self modify 'LD A' @ $EEAD
C $EF08,1 (*HL)--
C $EF09,3 Self modify 'LD A' @ $EF00
C $EF0C,1 (*HL)--
@ $EF0D label=dk_ef0d
C $EF0D,2 Self modified by $EF33
C $EF0F,1 A--
C $EF10,3 Jump to playdrum_go if zero
N $EF13 This entry point is used by the routines at #R$EF22 and #R$F0C6.
@ $EF13 label=dk_wait
C $EF13,5 Wait/spinlock.  Self modified
C $EF18,1 Return
c $EF19 How does this get entered?
C $EF19,1 Preserve registers
C $EF1A,5 Unlock the wait/spinlock. -- Self modify $EF13
C $EF1F,1 Restore registers
C $EF20,1 Enable interrupts
C $EF21,1 Return
c $EF22 Drum sample player.
D $EF22 Used by the routine at #R$EE9E.
R $EF22 I:B Speed value?
@ $EF22 label=playdrum_2
C $EF22,3 Address of drum 2 data
C $EF25,2 Sample bytes remaining = 108
C $EF27,2 Jump to playdrum_start
N $EF29 This entry point is used by the routine at #R$EE9E.
@ $EF29 label=playdrum_1
C $EF29,3 Address of drum 1 data
C $EF2C,2 Sample bytes remaining = 252
@ $EF2E label=playdrum_start
C $EF2E,3 Self modify 'LD B' @ $EF39 to be A  -- speed value passed in?
C $EF31,5 Self modify 'LD A' @ $EF0D to be 1  -- set to 1 while playing?
C $EF36,2 Jump to playdrum_go
N $EF38 This entry point is used by the routine at #R$EE9E.
@ $EF39 label=playdrum_go
C $EF39,2 Self modified by $EF2E (sampled: 8, 3, 1)
@ $EF3B label=playdrum_loop
C $EF3B,2 Output flag
C $EF3D,1 Delay?
C $EF3E,2 Test a bit
C $EF40,2 Don't reset output if sample bit set
C $EF42,2 Reset flag if bit was zero
C $EF44,2 Output it
C $EF46,2 Rotate sample byte in-place
C $EF48,2 Loop to playdrum_loop while #REGb
C $EF4A,1 Move to next sample byte
C $EF4B,1 Decrement sample bytes remaining
C $EF4C,2 Jump to playdrum_end_of_sample if no bytes remain
C $EF4E,3 Read A from 'LD A' @ $EF13  -- wait/spinlock
C $EF51,1 Set flags
C $EF52,3 Jump to playdrum_go if zero
C $EF56,1 Return
@ $EF57 label=playdrum_end_of_sample
C $EF57,4 Self modify 'LD A' @ $EF0D to be 0  -- set to 0 when playing stops?
C $EF5B,3 Jump to dk_playdrum_finished
@ $EF5E label=drum1
B $EF5E,252,8*31,4 Drum 1 sample/data
@ $F05A label=drum2
B $F05A,108,8*13,4 Drum 2 sample/data
c $F0C6 White noise generator?
D $F0C6 Used by the routine at #R$EE9E.
C $F0C6,1 E = A  Outer-outer counter
@ $F0C7 label=noise_outer_loop
C $F0C7,2 D = 50  Outer counter
@ $F0C9 label=noise_loop
C $F0C9,3 Point at rng_seed / state bytes
C $F0CC,3 rng_seed[0] += 3  Increment first byte of seed by 3
C $F0CF,1 Load it into #REGb
C $F0D0,1 HL++
C $F0D1,4 rng_seed[1] -= 141   Note that this is different order to rng/$961B
C $F0D5,1 A = rng_seed[0] + rng_seed[1]
C $F0D6,1 HL++
C $F0D7,1 Rotate A by 1
C $F0D8,2 Rotate rng_seed[2] by 1
C $F0DA,1 A += *HL
C $F0DB,1 *HL = A
C $F0DC,2 A &= 16
C $F0DE,2 Jump to $F0F0 if zero
C $F0E0,4 B = 24 - E
C $F0E4,2 Delay loop
C $F0E6,2 A = 24   EAR + MIC ?
C $F0E8,2 Output
C $F0EA,1 B = E
C $F0EB,2 Delay loop
C $F0ED,1 A = 0
C $F0EE,2 Output
C $F0F0,1 Decrement outer counter
C $F0F1,2 Jump to noise_loop if non-zero
C $F0F3,3 Read A from 'LD A' @ $EF13  -- spinlock/wait
C $F0F6,1 Set flags
C $F0F7,1 Return if carry set
C $F0F8,1 Decrement outer-outer counter
C $F0F9,2 Jump to noise_outer_loop if non-zero
C $F0FB,3 Jump to dk_wait
b $F0FE Music data
B $F0FE,4,4
W $F102,14,2 Patterns (offset, repetitions?)
B $F110,1,1
B $F111,271,8*33,7 Music
c $F220 routine/data copied to $8014 during init? (926 bytes long)
C $F2B2,3 Exit via (if P) print_message
C $F3A5,3 Call silence_audio_hook
C $F460,3 Call print_message
C $F485,3 Call message_printing_related
C $F488,3 Call transition
C $F48B,3 Call draw_screen
b $F491 Credits / Score messages
B $F491,6,6
T $F497,10,10 "PRESS GEAR"
B $F4A1,7,7
T $F4A8,17,16:n1 "ENTER FOR OPTIONS"
B $F4B9,8,8
T $F4C1,7,6:n1 "CREDITS"
B $F4C8,7,7
T $F4CF,20,19:n1 "PROGRAM      JOBBEEE"
B $F4E3,7,7
T $F4EA,20,19:n1 "GRAPHICS        BILL"
B $F4FE,7,7
T $F505,20,19:n1 "MUSIC       JON DUNN"
B $F519,10,8,2
T $F523,13,12:n1 "BEST OFFICERS"
B $F530,7,7
T $F537,28,27:n1 "RANK  SCORE  STAGE PLAY NAME"
B $F553,7,7
T $F55A,28,27:n1 "1ST  56784010  ALL    1  JOB"
B $F576,7,7
T $F57D,28,27:n1 "2ND  35678000   4     1  ABC"
B $F599,7,7
T $F5A0,28,27:n1 "3RD   4340300   3     2  DEF"
B $F5BC,2,2
b $F5BE Data block at F5BE
B $F5BE,788,8*98,4
b $F8D2 Data block at F8D2
B $F8D2,513,8*64,1
b $FAD3 Data block at FAD3
B $FAD3,865,8*108,1
b $FE34 Data block at FE34
B $FE34,22,8*2,6
b $FE4A Data block at FE4A
B $FE4A,4,4
b $FE4E Data block at FE4E
B $FE4E,22,8*2,6
b $FE64 Data block at FE64
B $FE64,6,6
b $FE6A Data block at FE6A
B $FE6A,10,8,2
b $FE74 Data block at FE74
B $FE74,74,8*9,2
b $FEBE Data block at FEBE
B $FEBE,64,8
c $FEFE Routine at FEFE
b $FF01 Data block at FF01
B $FF01,255,8*31,7
