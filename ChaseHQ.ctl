; ChaseHQ.ctl

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
> $4000 ; $ED28        is the stack
> $4000 ; $EE00..$EEFF is (cleared by $87DD) a cyclic road buffer of some sort [perhaps split in two or more sections]
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
> $4000 ;

@ $4000 org
@ $4000 set-warnings=1
@ $4000 start
@ $4000 writer=:ChaseHQ.ChaseHQAsmWriter

b $4000 Screen memory
B $4000,6144,8 Screen bitmap
B $5800,768,8 Screen attributes
c $5B00 Could be loader code
C $5B22 Point #REGhl at $5B7D
C $5B2B Fetch a word from #REGhl and stack it
C $5B31 Routine that ...
C $5B4C Game entry point
C $5B58 Routine that ...
C $5B66 Routine that ...
C $5B67 Move $5C00..$76EF to $C000 onwards
C $5B75 Routine that loads a word then OUTs $FE with zero, then jumps to that word
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
D $5C00 #HTML[#CALL:graphic($5C00,80,24,0,1)]
;B $5B00,240 Hill backdrop (80x24) seems to be pre-shifted by #R$C8BE (overlaps game setup)
B $5C00,240 Hill backdrop (80x24) inverted. Varies per level

b $5CF0 Per-level data
;
@ $5CF0 label=perp_mugshot_attributes
W $5CF0,2 Address of perp's mugshot attributes
@ $5CF2 label=perp_mugshot_bitmap
W $5CF2,2 Address of perp's mugshot bitmap
;
@ $5CF4 label=ground_colour
W $5CF4,2 Screen attributes used for the ground colour (a pair of matching bytes)
;
;W $5CF6,18,2 TBD - likely addresses used by $AC1F
; This has various per-level data pointers but it's not consistent.
W $5CF6,2 TBD
W $5CF8,2 Loaded by $900F. Points to an array of 7 byte entries.
W $5CFA,2 TBD
W $5CFC,2 -> "Entry 3"
W $5CFE,2 <turn sign lods> inconsistent...
W $5D00,2 -> "Entry 9 (turn sign)"
W $5D02,2 -> "Entry 12"
W $5D04,2 -> Nancy's report
W $5D06,2 -> Arrest messages

w $5D0C Table of addresses of LODs (levels of detail - a set of sprites of various sizes representing the same object).
@ $5D0C label=lods_table
  $5D0C,2 -> stones_lods
  $5D0E,2 -> dust_lods
  $5D10,2 -> car_lods (perp's car)
  $5D12,2 -> lambo_lods
  $5D14,2 -> truck_lods
  $5D16,2 -> lambo_lods
  $5D18,2 -> car_lods

b $5D1A
B $5D1A,1 loaded by $A813
B $5D1B,1 loaded by $A6A7
B $5D1C,1 loaded by $A759

@ $5D1D label=setup_game_data
w $5D1D
W $5D1D,2 road_pos
N $5D1F These all point a byte earlier than the real data start point.
W $5D1F,2 -> Start stretch, curvature
W $5D21,2 -> Start stretch, height
W $5D23,2 -> Start stretch, lanes
W $5D25,2 -> Start stretch, right-side objects
W $5D27,2 -> Start stretch, left-side objects
W $5D29,2 -> Start stretch, hazards
;
@ $5D2B label=attract_data
w $5D2B
W $5D2B,2 road_pos
N $5D2D These all point a byte earlier than the real data start point.
W $5D2D,2 -> Loop section, curvature
W $5D2F,2 -> Loop section, height
W $5D31,2 -> Loop section, lanes
W $5D33,2 -> Loop section, right-side objects
W $5D35,2 -> Loop section, left-side objects
W $5D37,2 -> Loop section, hazards

b $5D39 Nancy's report
B $5D39 Nancy ($01)
W $5D3A Pointers to Nancy's message (for Stage 1)
W $5D3A,2 Points at "THIS IS NANCY..."
W $5D3C,2 Points at "EMERGENCY HERE..."
W $5D3E,2 Points at "IS FLEEING ..."
W $5D40,2 Points at "VEHICLE IS..."
B $5D42,1 TBD $FE (pause?)
W $5D43,2 -> Random choice of ("WE READ..." / "ROGER!" / "GOTCHA...")

b $5D45 Messages - Nancy
T $5D45,40 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $5D6D,40 "EMERGENCY HERE. RALPH THE IDAHO SLASHER,"
T $5D95,42 "IS FLEEING TOWARDS THE SUBURBS. THE TARGET"
T $5DBF,46 "VEHICLE IS A WHITE BRITISH SPORTS CAR... OVER."

b $5DED Messages - Arrest
B $5DED,1 Frame delay until first message?
B $5DEE,1 Frame delay until next message?
;
B $5DEF,1 Flags (?)
B $5DF0,1 Attribute (black)
W $5DF1,2 Back buffer address
W $5DF3,2 Attribute address
T $5DF5,27 "OK! YOU ARE UNDER ARREST ON"
B $5E10,1 Frame delay until next message?
;
B $5E11,1 Flags (?)
B $5E12,1 Attribute (black)
W $5E13,2 Back buffer address
W $5E15,2 Attribute address
T $5E17,26 "SUSPICION OF FIRST DEGREE "
B $5E31,1 Frame delay until next message?
;
B $5E32,1 Flags (?)
B $5E33,1 Attribute (black)
W $5E34,2 Back buffer address
W $5E36,2 Attribute address
T $5E38,6 "MURDER"
B $5E3E,1 Frame delay until next message?

b $5E3F Graphics or object? definitions
D $5E3F All are stored inverted except where noted.
B $5E3F Pointed to by $5CFA
;
b $5E40 Pointed to by $5CF6
B $5E40,1
W $5E41,2 Address of tumbleweed_lods table
B $5E43,1
W $5E44,2 Address of barrier_lods table
;

; Entry 0 does not look plausible.
;
N $5E46 Entry 1
B $5E46,3 TBD
W $5E49,2 Arg for routine passed in DE
W $5E4B,2 -> Routine at $9252
;
N $5E4D Entry 2 (empty)
B $5E4D,3
W $5E50,2
W $5E52,2
;
N $5E54 Entry 3
B $5E54,3 TBD
W $5E57,2 Arg for routine passed in DE
W $5E59,2 -> Routine at $9171
;
N $5E5B Entry 4
B $5E5B,3 TBD - remaining bytes of this entry
W $5E5E,2 Arg for routine passed in DE
W $5E60,2 -> Routine at $9171. Loaded into HL at $9017 then jumped to.
;
N $5E62 Entry 5
B $5E62,3 TBD - remaining bytes of this entry
W $5E65,2 Arg for routine passed in DE
W $5E67,2 -> Routine at $9171
;
N $5E69 Entry 6
B $5E69,3 TBD - remaining bytes of this entry
W $5E6C,2 Arg for routine passed in DE
W $5E6E,2 -> Routine at $9171
;
N $5E70 Entry 7
B $5E70,3 TBD - remaining bytes of this entry
W $5E73,2 Arg for routine passed in DE
W $5E75,2 -> Routine at $9171
;
N $5E77 Entry 8
B $5E77,3 TBD - remaining bytes of this entry
W $5E7A,2 Arg for routine passed in DE
W $5E7C,2 Routine?
;W $5E7D,2 -> car_lods could be coincidence?
;
N $5E7E Entry 9 (turn sign)
B $5E7E,3 TBD - remaining bytes of this entry
W $5E81,2 -> -> turn_sign_lods
W $5E83,2 -> Routine at $92E1
;
N $5E85 Entry 10
B $5E85,3 TBD
W $5E88,2 Arg
W $5E8A,2 -> Routine at $924D
;
N $5E8C Entry 11 (empty)
B $5E8C,3
W $5E8F,2
W $5E91,2
;
N $5E93 Entry 12
B $5E93,3 TBD
W $5E96,2 Arg
W $5E98,2 -> Routine at $916C
;
N $5E9A Entry 13
B $5E9A,3 TBD
W $5E9D,2 Arg
W $5E9F,2 -> Routine at $916C
;
N $5EA1 Entry 14
B $5EA1,3 TBD
W $5EA4,2 Arg
W $5EA6,2 -> Routine at $916C
;
N $5EA8 Entry 15
B $5EA8,3 TBD
W $5EAB,2 Arg
W $5EAD,2 -> Routine at $916C
;
N $5EAF Entry 16
B $5EAF,3 TBD
W $5EB2,2 Arg
W $5EB4,2 -> Routine at $916C
;
N $5EB6 Entry 17
B $5EB6,3 TBD
W $5EB9,2 Arg
W $5EBB,2 -> Routine at $9278
;
N $5EBD Entry 18 (turn sign)
B $5EBD,3 TBD
W $5EC0,2 Arg
W $5EC2,2 -> Routine at $9278
;
; $5EC3 is referenced so perhaps we're dealing with 1-indexed data here.
b $5EC4 Initial map segment
;
;b $6088 APPROX?! map segment - bumpy road then tunnel
;B $6088
;B $60A3 Looks like the graphic data for a lamppost but I suspect it's a height(?) related data for the bumpy road
;B $60E6
;B $60EF
;B $6109
;B $6144
;
;B $61D3 Ref'd from attract_data
;B $6208 Ref'd from attract_data
;B $625C Ref'd from attract_data
;B $6276 Ref'd from attract_data
;B $6293 Ref'd from attract_data
;B $6313 Ref'd from attract_data
;
; $5FE2 is likely a word (as are other zero values)
;
N $5EC4 Start stretch
;
N $5EC4 Start stretch, curvature
B $5EC4,2 Straight x2
B $5EC6,1 Left
B $5EC7,,1
B $5ECA,5 Straight x5
B $5ECF,,1
B $5ED0,1 Left
B $5ED1,,1
B $5ED5,1 Straight
B $5ED6,,1
B $5ED7,1 Left
B $5ED8,,1
B $5ED9,1 Right
B $5EDA,,1
B $5EDC,1 Straight
B $5EDD,,1
B $5EDE,2 Escape, Command 2 (Road Fork)
W $5EE0,2 Address of left fork data
W $5EE2,2 Address of right fork data
;
N $5EE4 Start stretch, height
B $5EE4,5 Level ground x 5
B $5EE9,,1
B $5F01,7 Level ground x 7
B $5F08,,1
B $5F09,2 Escape, Command 2 (Road Fork)
W $5F0B,2 Address of left fork data
W $5F0D,2 Address of right fork data
;
N $5F0F Start stretch, lanes
B $5F0F Count
B $5F10 TBD - three lanes
B $5F11 Count
B $5F12 TBD - widening
B $5F13 Count
B $5F14 TBD - four lanes
B $5F15 Count
B $5F16 TBD - four lanes
B $5F17,2 Escape, Command 2 (Road Fork)
W $5F19,2 Address of left fork data
W $5F1B,2 Address of right fork data
;
N $5F1D Start stretch, hazards
B $5F1D,1 Wait for 147
B $5F1E,2 Escape, Command 14 (Disable Car Spawning)
B $5F20,1 Wait for 12
B $5F21,2 Escape, Command 12 (Set Floating Arrow to Right)
B $5F23,1 Wait for 1
B $5F24,2 Escape, Command 2 (Road Fork)
W $5F26,2 Address of left fork data
W $5F28,2 Address of right fork data
;
N $5F2A Start stretch, left-side objects
B $5F2A,,1
B $5F72,2 Escape, Command 2 (Road Fork)
W $5F74,2 Address of left fork data
W $5F76,2 Address of right fork data
;
N $5F78 Start stretch, right-side objects
B $5F78,,1
B $5FCC,2 Escape, Command 2 (Road Fork)
W $5FCE,2 Address of left fork data
W $5FD0,2 Address of right fork data
;
;
N $5FD2 Left fork
;
N $5FD2 Left fork, curvature
B $5FD2,,1
B $5FE2,2 Escape, Command 0 (Continue at <Address>)
W $5FE4,2 Address
;
N $5FE6 Left fork, height
B $5FE6,,1
B $5FFF,2 Escape, Command 0 (Continue at <Address>)
W $6001,2 Address
;
N $6003 Left fork, lanes
B $6003,,1
B $6005,2 Escape, Command 0 (Continue at <Address>)
W $6007,2 Address
;
N $6009 Left fork, hazards
B $6009,1 Wait for 10
B $600A,2 Escape, Command 13 (Enable Car Spawning)
B $600C,1 Wait for 95
B $600D,2 Escape, Command 0 (Continue at <Address>)
W $600F,2 Address [Loaded by $C04A]
;
N $6011 Left fork, left-side objects
B $6011,,1
B $604E,2 Escape, Command 0 (Continue at <Address>)
W $6050,2 Address
;
N $6052 Left fork, right-side objects
B $6052,,1
B $6084,2 Escape, Command 0 (Continue at <Address>)
W $6086,2 Address
;
;
N $6088 Right fork (dirt track), curvature
B $6088,1 Straight
B $6089,,1
B $608A,1 Left
B $608B,,1
B $608C,1 Hard left
B $608D,1 Hard left
B $608E,,1
B $6090,1 Straight
B $6091,,1
B $6096,1 Hard left
B $6097,,1
B $6098,1 Super hard left
B $6099,,1
B $609D,1 Straight
B $609E,,1
B $609F,2 Escape, Command 0 (Continue at <Address>)
W $60A1,2 Address
;
N $60A3 Right fork (dirt track), height
B $60A3,1 Level ground
B $60A4,,1
B $60E1,2 Escape, Command 0 (Continue at <Address>)
W $60E3,2 Address
;
N $60E5 Right fork (dirt track), lanes
B $60E5,,1
W $60ED,2 Address
;
N $60EF Right fork (dirt track), hazards
B $60EF,1 Wait for 10
B $60F0,2 Escape, Command 9 (Put Barriers on Both Sides)
B $60F2,1 Wait for 1
B $60F3,2 Escape, Command 6 (Put Tumbleweeds on Both Sides)
B $60F5,1 Wait for 41
B $60F6,2 Escape, Command 9 (Put Barriers on Both Sides)
B $60F8,1 Wait for 4
B $60F9,2 Escape, Command 6 (Put Tumbleweeds on Both Sides)
B $60FB,1 Wait for 42
B $60FC,2 Escape, Command 9 (Put Barriers on Both Sides)
B $60FE,1 Wait for 2
B $60FF,2 Escape, Command 3 (Disable Hazards)
B $6101,1 Wait for 1
B $6102,2 Escape, Command 13 (Enable Car Spawning)
B $6104,1 Wait for 4
B $6105,2 Escape, Command 0 (Continue at <Address>)
W $6107,2 Address
;
N $6109 Right fork (dirt track), left-side objects
B $6109,,1
W $6141,2 Address
;
N $6143 Right fork (dirt track), right-side objects
B $6143,,1
W $6171,2 Address
;
;
N $6173 Tunnel section, curvature
B $6173,,1
B $6185,2 Escape, Command 0 (Continue at <Address>)
W $6187,2 -> Loop section
;
N $6189 Tunnel section, height
B $6189,,1
W $61A3,2
;
N $61A5 Tunnel section, lanes
B $61A5,,1
W $61B7,2
;
N $61B9 Tunnel section, hazards
B $61B9,1 Wait for 95
B $61BA,2 Escape, Command 0 (Continue at <Address>)
W $61BC,2 Loaded by $C04A
;
N $61BE Tunnel section, left-side objects
B $61BE,,1
W $61C7,2
;
N $61C9 Tunnel section, right-side objects
B $61C9,,1
W $61D2,2
;
;
N $61D4 Loop section, curvature
B $61D4,,1
B $6205,2 Escape, Command 0 (Continue at <Address>)
W $6207,2 Loop
;
N $6209 Loop section, height
B $6209,,1
B $6259,2 Escape, Command 0 (Continue at <Address>)
W $625B,2 Loop
;
N $625D Loop section, lanes
B $625D,,1
B $6273,2 Escape, Command 0 (Continue at <Address>)
W $6275,2 Loop
;
N $6277 Loop section, hazards
B $6277,1 Wait for 42
B $6278,2 Escape, Command 7 (Barriers on Left)
B $627A,1 Wait for 2
B $627B,2 Escape, Command 3 (Hazards off)
B $627D,1 Wait for 4
B $627E,2 Escape, Command 7 (Barriers on Left)
B $6280,1 Wait for 2
B $6281,2 Escape, Command 3 (Hazards off)
B $6283,1 Wait for 28
B $6284,2 Escape, Command 8 (Barriers on Right)
B $6286,1 Wait for 2
B $6287,2 Escape, Command 3 (Hazards off)
B $6289,1 Wait for 99
B $628A,2 Escape, Command 8 (Barriers on Right)
B $628C,1 Wait for 1
B $628D,2 Escape, Command 3 (Hazards off)
B $628F,1 Wait for 30
B $6290,2 Escape, Command 0 (Continue at <Address>)
W $6292,2 Loop [Loaded by $C04A]
;
N $6294 Loop section, left-side objects
B $6294,,1
B $6310,2 Escape, Command 0 (Continue at <Address>)
W $6312,2 Loop
;
N $6314 Loop section, right-side objects
B $6314,,1
B $6386,2 Escape, Command 0 (Continue at <Address>)
W $6388,2 Loop

b $638A Perp's face
N $638A Bitmap data for perp w/ sunglasses (32x40). Stored top-down.
N $638A #HTML[#CALL:face($638A)]
B $638A,160,4
N $642A Attribute data for perp w/ sunglasses (4x5). Stored top-down.
B $642A,20,4

b $643E LODs
D $643E LOD = level of detail. These structures collect together the variously sized versions of the same game object.
;
; Flags in these LOD structures:
; - 1 seems to indicate pre-shifted bitmaps
; - 2 is probably to flip
;
@ $643E label=lambo_lods
N $643E LOD
B $643E,1 Width (bytes)
B $643F,1 Flags
B $6440,1 Height (pixels)
W $6441,2 Bitmap
W $6443,2 Pre-shifted bitmap
L $643E,7,6
;
@ $6468 label=truck_lods
N $6468 LOD
B $6468,1 Width (bytes)
B $6469,1 Flags
B $646A,1 Height (pixels)
W $646B,2 Bitmap
W $646D,2 Pre-shifted bitmap
L $6468,7,6
;
@ $6492 label=car_lods
N $6492 LOD
B $6492,1 Width (bytes)
B $6493,1 Flags
B $6494,1 Height (pixels)
W $6495,2 Bitmap
W $6497,2 Pre-shifted bitmap
L $6492,7,6
;
N $64BC Lamborghini_1 (48x30)
N $64BC #HTML[#CALL:graphic($64BC,48,30,0,1)]
B $64BC,180,6
;
N $6570 Lamborghini_2 (40x22)
N $6570 #HTML[#CALL:graphic($6570,40,22,0,1)]
B $6570,110,5
;
N $65DE Lamborghini_3 (24x15)
N $65DE #HTML[#CALL:graphic($65DE,24,15,0,1)]
B $65DE,45,3
;
N $660B Truck_1 (48x39)
N $660B #HTML[#CALL:graphic($660B,48,39,0,1)]
B $660B,234,6
;
N $66F5 Truck_2 (40x29)
N $66F5 #HTML[#CALL:graphic($66F5,40,29,0,1)]
B $66F5,145,5
;
N $6786 Truck_3 (24x20)
N $6786 #HTML[#CALL:graphic($6786,24,20,0,1)]
B $6786,60,3
;
N $67C2 Car_1 (48x31)
N $67C2 #HTML[#CALL:graphic($67C2,48,31,0,1)]
B $67C2,186,6
;
N $687C Car_2 (40x22)
N $687C #HTML[#CALL:graphic($687C,40,22,0,1)]
B $687C,110,5
;
N $68EA Car_3 (24x16)
N $68EA #HTML[#CALL:graphic($68EA,24,16,0,1)]
B $68EA,48,3
;
N $691A Lamborghini_4 (24x8)
N $691A #HTML[#CALL:graphic($691A,24,8,1,1)]
B $691A,48,6 Masked bitmap data
;
N $694A Lamborghini_5 (24x8) pre-shifted
N $694A #HTML[#CALL:graphic($694A,24,8,1,1)]
B $694A,48,6 Masked bitmap data
;
N $697A Truck_4/5 (16x12)
N $697A #HTML[#CALL:graphic($697A,16,12,1,1)]
B $697A,48,4 Masked bitmap data
;
N $69AA Truck_4/5 (16x12) pre-shifted
N $69AA #HTML[#CALL:graphic($69AA,16,12,1,1)]
B $69AA,48,4 Masked bitmap data
;
N $69DA Car_4/5 (24x9)
N $69DA #HTML[#CALL:graphic($69DA,24,9,1,1)]
B $69DA,54,6 Masked bitmap data
;
N $6A10 Car_4/5 (24x9) pre-shifted
N $6A10 #HTML[#CALL:graphic($6A10,24,9,1,1)]
B $6A10,54,6 Masked bitmap data
;
@ $6A46 label=stones_lods
N $6A46 LOD
B $6A46,1 Width (bytes)
B $6A47,1 Flags
B $6A48,1 Height (pixels)
W $6A49,2 Bitmap
W $6A4B,2 Pre-shifted bitmap
L $6A46,7,6
;
@ $6A70 label=dust_lods
N $6A70 LOD
B $6A70,1 Width (bytes)
B $6A71,1 Flags
B $6A72,1 Height (pixels)
W $6A73,2 Bitmap
W $6A75,2 Pre-shifted bitmap
L $6A70,7,6
;
N $6A9A Stones (16x5)
N $6A9A #HTML[#CALL:graphic($6A9A,16,5,1,1)]
B $6A9A,20,4 Masked bitmap data
;
N $6AAE Stones (16x4)
N $6AAE #HTML[#CALL:graphic($6AAE,16,4,1,1)]
B $6AAE,16,4 Masked bitmap data
;
N $6ABE Stones (16x4) pre-shifted
N $6ABE #HTML[#CALL:graphic($6ABE,16,4,1,1)]
B $6ABE,16,4 Masked bitmap data
;
N $6ACE Stones (16x3)
N $6ACE #HTML[#CALL:graphic($6ACE,16,3,1,1)]
B $6ACE,12,4 Masked bitmap data
;
N $6ADA Stones (16x3) pre-shifted
N $6ADA #HTML[#CALL:graphic($6ADA,16,3,1,1)]
B $6ADA,12,4 Masked bitmap data
;
N $6AE6 Stones (8x2)
N $6AE6 #HTML[#CALL:graphic($6AE6,8,2,1,1)]
B $6AE6,4,2 Masked bitmap data
;
N $6AEA Stones (8x2) pre-shifted
N $6AEA #HTML[#CALL:graphic($6AEA,8,2,1,1)]
B $6AEA,4,2 Masked bitmap data
;
N $6AEE Stones (8x1)
N $6AEE #HTML[#CALL:graphic($6AEE,8,1,1,1)]
B $6AEE,2,2 Masked bitmap data
;
N $6AF0 Stones (8x1) pre-shifted
N $6AF0 #HTML[#CALL:graphic($6AF0,8,1,1,1)]
B $6AF0,2,2 Masked bitmap data
;
N $6AF2 Dust (8x1)
N $6AF2 #HTML[#CALL:graphic($6AF2,8,1,1,1)]
B $6AF2,2 Masked bitmap data
;
N $6AF4 Dust (8x1) pre-shifted
N $6AF4 #HTML[#CALL:graphic($6AF4,8,1,1,1)]
B $6AF4,2 Masked bitmap data
;
W $6AF6,2 -> turn_sign_lods
B $6AF8
N $6B0C Ref'd by graphic entry 8 and 17
W $6B0C,2 -> turn_sign_lods
;
@ $6B22 label=turn_sign_lods
N $6B22 LOD
B $6B22,1 Width (bytes)
B $6B23,1 Flags
B $6B24,1 Height (pixels)
W $6B25,2 Bitmap
W $6B27,2 Pre-shifted bitmap
L $6B22,7,10
;
N $6B68 Turn right sign (32x40)
N $6B68 #HTML[#CALL:graphic($6B68,32,40,0,1)]
B $6B68,160,4
;
N $6C08 Turn right sign (24x30)
N $6C08 #HTML[#CALL:graphic($6C08,24,30,0,1)]
B $6C08,90,3
;
N $6C62 Turn right sign (16x20)
N $6C62 #HTML[#CALL:graphic($6C62,16,20,0,1)]
B $6C62,40,2
;
N $6C8A Turn right sign (16x16)
N $6C8A #HTML[#CALL:graphic($6C8A,16,16,1,1)]
B $6C8A,64,4 Masked bitmap data
;
N $6CCA Turn right sign (16x13)
N $6CCA #HTML[#CALL:graphic($6CCA,16,13,1,1)]
B $6CCA,52,2 Masked bitmap data
;
N $6CFE Turn right sign (16x13) pre-shifted
N $6CFE #HTML[#CALL:graphic($6CFE,16,13,1,1)]
B $6CFE,52,2 Masked bitmap data
;
N $6D32 Turn right sign (16x10)
N $6D32 #HTML[#CALL:graphic($6D32,16,10,1,1)]
B $6D32,,4 Masked bitmap data
;
N $6D5A Turn right sign (16x10) pre-shifted
N $6D5A #HTML[#CALL:graphic($6D5A,16,10,1,1)]
B $6D5A,,4 Masked bitmap data
;
;
@ $6D82 label=tumbleweed_lods
N $6D82 LOD
B $6D82,1 Width (bytes)
B $6D83,1 Flags
B $6D84,1 Height (pixels)
W $6D85,2 Bitmap
W $6D87,2 Pre-shifted bitmap
L $6D82,7,6
;
N $6DAC Tumbleweed_1 (16x16)
N $6DAC #HTML[#CALL:graphic($6DAC,16,16,0,1)]
B $6DAC,32,2
;
N $6DCC Tumbleweed_2 (16x11)
N $6DCC #HTML[#CALL:graphic($6DCC,16,11,0,1)]
B $6DCC,22,2
;
N $6DE2 Tumbleweed_3 (8x9)
N $6DE2 #HTML[#CALL:graphic($6DE2,8,9,0,1)]
B $6DE2,9,1
;
N $6DEB Tumbleweed_4 (8x7)
N $6DEB #HTML[#CALL:graphic($6DEB,8,7,0,1)]
B $6DEB,7,1
;
;
@ $6DF2 label=barrier_lods
N $6DF2 LOD
B $6DF2,1 Width (bytes)
B $6DF3,1 Flags
B $6DF4,1 Height (pixels)
W $6DF5,2 Bitmap
W $6DF7,2 Pre-shifted bitmap
L $6DF2,7,6
;
N $6E1C Barrier (32x17)
N $6E1C #HTML[#CALL:graphic($6E1C,32,17,0,1)]
B $6E1C,68,4
;
N $6E60 Barrier (24x13)
N $6E60 #HTML[#CALL:graphic($6E60,24,13,0,1)]
B $6E60,39,3
;
N $6E87 Barrier (16x9)
N $6E87 #HTML[#CALL:graphic($6E87,16,9,0,1)]
B $6E87,18,2
;
N $6E99 Barrier (16x7) pre-shifted
N $6E99 #HTML[#CALL:graphic($6E99,16,7,1,1)]
B $6E99,28,4 Masked bitmap data
;
N $6EB5 Barrier (16x7) pre-shifted
N $6EB5 #HTML[#CALL:graphic($6EB5,16,7,1,1)]
B $6EB5,28,4 Masked bitmap data
;
B $6ED1
N $6ED1 Ref'd by graphic entry 6
W $6EDB,2 Ptr?
N $6EDE Ref'd by graphic entry 15
W $6EE8,2 Ptr?
W $6EEB,2 Address of another LOD table
W $6F01,2 Address of another LOD table
  $6F03,20 TBD
;
@ $6F17 label=streetlamp_lods
N $6F17 LOD
B $6F17,1 Width (bytes)
B $6F18,1 Flags
B $6F19,1 Height
W $6F1A,2 Bitmap
W $6F1C,2 Pre-shifted bitmap
L $6F17,7,5
;
@ $6F3A label=streetlamp_flipped_lods
N $6F3A LOD
B $6F3A,1 Width (bytes)
B $6F3B,1 Flags
B $6F3C,1 Height
W $6F3D,2 Bitmap
W $6F3F,2 Pre-shifted bitmap
L $6F3A,7,5
;
N $6F5D Street lamp top (32x8)
N $6F5D #HTML[#CALL:graphic($6F5D,32,8,0,1)]
B $6F7D,15,3
;
N $6F7D Street lamp top (24x5)
N $6F7D #HTML[#CALL:graphic($6F7D,24,5,0,1)]
B $6F8C,12,3
;
N $6F8C Street lamp top (24x4)
N $6F8C #HTML[#CALL:graphic($6F8C,24,4,0,1)]
B $6F98,12,3
;
N $6F98 Street lamp top (24x4) pre-shifted
N $6F98 #HTML[#CALL:graphic($6F98,24,4,0,1)]
B $6FA4,16,2
;
N $6FA4 Street lamp top (16x4)
N $6FA4 #HTML[#CALL:graphic($6FA4,16,4,1,1)]
B $6FB4,16,2 Masked bitmap data
;
N $6FB4 Street lamp top (16x4) pre-shifted
N $6FB4 #HTML[#CALL:graphic($6FB4,16,4,1,1)]
B $6FC4,12,2 Masked bitmap data
;
N $6FC4 Street lamp top (16x3)
N $6FC4 #HTML[#CALL:graphic($6FC4,16,3,1,1)]
B $6FD0,12,2 Masked bitmap data
;
N $6FD0 Street lamp top (16x3) pre-shifted
N $6FD0 #HTML[#CALL:graphic($6FD0,16,3,1,1)]
B $6F5D,32,4 Masked bitmap data
;
;
N $6FDC Ref'd by graphic entry 7
N $6FE6 Ref'd by graphic entry 16
;B $7069 is the top part of a streetlamp
N $70F6 Ref'd by graphic entry 4 and 13
N $7106 Ref'd by graphic entry 5 and 14
;
@ $717E label=tree_lods
N $717E LOD
B $717E,1 Width (bytes)
B $717F,1 Flags
B $7180,1 Height
W $7181,2 Bitmap
W $7183,2 Pre-shifted bitmap
L $717E,7,25
;
@ $722D label=bitmap_tree_middle_64x16
N $722D Tree middle (64x16)
N $722D #HTML[#CALL:graphic($722D,64,16,0,1)]
B $722D
;
@ $72AD label=bitmap_tree_bottom_64x5
N $72AD Tree bottom (64x5)
N $72AD #HTML[#CALL:graphic($72AD,64,5,0,1)]
B $72AD
;
@ $72D5 label=bitmap_tree_trunk_16x8
N $72D5 Tree trunk (16x8)
N $72D5 #HTML[#CALL:graphic($72D5,16,8,0,1)]
B $72D5
;
@ $72E5 label=bitmap_tree_shadow_64x5
N $72E5 Tree shadow (64x5)
N $72E5 #HTML[#CALL:graphic($72E5,64,5,0,1)]
B $72E5
;
@ $730D label=bitmap_tree_middle_48x12
N $730D Tree middle (48x12)
N $730D #HTML[#CALL:graphic($730D,48,12,0,1)]
B $730D
;
@ $7355 label=bitmap_tree_bottom_48x4
N $7355 Tree bottom (48x4)
N $7355 #HTML[#CALL:graphic($7355,48,4,0,1)]
B $7355
;
@ $736D label=bitmap_tree_shadow_48x4
N $736D Tree shadow (48x4)
N $736D #HTML[#CALL:graphic($736D,48,4,0,1)]
B $736D
;
@ $7385 label=bitmap_tree_middle_32x8
N $7385 Tree middle (32x8)
N $7385 #HTML[#CALL:graphic($7385,32,8,0,1)]
B $7385
;
@ $73A5 label=bitmap_tree_bottom_32x3
N $73A5 Tree bottom (32x3)
N $73A5 #HTML[#CALL:graphic($73A5,32,3,0,1)]
B $73A5
;
@ $73B1 label=bitmap_tree_shadow_16x2
N $73B1 Tree shadow (16x2)
N $73B1 #HTML[#CALL:graphic($73B1,16,2,0,1)]
B $73B1
;
@ $73B9 label=bitmap_tree_middle_24x7
N $73B9 Tree middle (24x7)
N $73B9 #HTML[#CALL:graphic($73B9,24,7,0,1)]
B $73B9
;
@ $73CE label=bitmap_tree_bottom_24x2
N $73CE Tree bottom (24x2)
N $73CE #HTML[#CALL:graphic($73CE,24,2,0,1)]
B $73CE
;
@ $73D4 label=bitmap_tree_trunk_8x4
N $73D4 Tree trunk (8x4)
N $73D4 #HTML[#CALL:graphic($73D4,8,4,0,1)]
B $73D4
;
@ $73D8 label=bitmap_tree_shadow_24x2
N $73D8 Tree shadow (24x2)
N $73D8 #HTML[#CALL:graphic($73D8,24,2,0,1)]
B $73D8
;
@ $73DE label=bitmap_tree_top_64x13
N $73DE Tree top (64x13)
N $73DE #HTML[#CALL:graphic($73DE,64,13,1,1)]
B $73DE  Masked bitmap data
;
@ $74AE label=bitmap_tree_top_48x10
N $74AE Tree top (48x10)
N $74AE #HTML[#CALL:graphic($74AE,48,10,1,1)]
B $74AE,120,12 Masked bitmap data
;
@ $7526 label=bitmap_tree_top_32x5
N $7526 Tree top (32x5)
N $7526 #HTML[#CALL:graphic($7526,32,5,1,1)]
B $7526,40,8 Masked bitmap data
;
@ $754E label=bitmap_tree_top_24x4
N $754E Tree top (24x4)
N $754E #HTML[#CALL:graphic($754E,24,4,1,1)]
B $754E,24,6 Masked bitmap data
;
@ $7566 label=bitmap_tree_top_24x3
N $7566 Tree top (24x3)
N $7566 #HTML[#CALL:graphic($7566,24,3,1,1)]
B $7566,18,6 Masked bitmap data
;
@ $7578 label=bitmap_tree_top_24x3s
N $7578 Tree top (24x3) pre-shifted
N $7578 #HTML[#CALL:graphic($7578,24,3,1,1)]
B $7578,18,6 Masked bitmap data
;
@ $758A label=bitmap_tree_trunk_16x6
N $758A Tree trunk (16x6)
N $758A #HTML[#CALL:graphic($758A,16,6,1,1)]
B $758A,24,4 Masked bitmap data
;
@ $75A2 label=bitmap_tree_trunk_16x4
N $75A2 Tree trunk (16x4)
N $75A2 #HTML[#CALL:graphic($75A2,16,4,1,1)]
B $75A2,16,4 Masked bitmap data
;
@ $75B2 label=bitmap_tree_middle_24x5
N $75B2 Tree middle (24x5)
N $75B2 #HTML[#CALL:graphic($75B2,24,5,1,1)]
B $75B2,30,6 Masked bitmap data
;
; Naming collision here...
@ $75D0 label=bitmap_tree_bottom_24x2_another
N $75D0 Tree bottom (24x2)
N $75D0 #HTML[#CALL:graphic($75D0,24,2,1,1)]
B $75D0,12,6 Masked bitmap data
;
@ $75DC label=bitmap_tree_trunk_24x3
N $75DC Tree trunk (24x3)
N $75DC #HTML[#CALL:graphic($75DC,24,3,1,1)]
B $75DC,18,6 Masked bitmap data
;
@ $75EE label=bitmap_tree_shadow_24x1
N $75EE Tree shadow (24x1)
N $75EE #HTML[#CALL:graphic($75EE,24,1,1,1)]
B $75EE,6,6 Masked bitmap data
;
@ $75F4 label=bitmap_tree_middle_24x5s
N $75F4 Tree middle (24x5) pre-shifted
N $75F4 #HTML[#CALL:graphic($75F4,24,5,1,1)]
B $75F4,30,6 Masked bitmap data
;
@ $7612 label=bitmap_tree_bottom_24x2s
N $7612 Tree bottom (24x2) pre-shifted
N $7612 #HTML[#CALL:graphic($7612,24,2,1,1)]
B $7612,12,6 Masked bitmap data
;
@ $761E label=bitmap_tree_trunk_24x3s
N $761E Tree trunk (24x3) pre-shifted
N $761E #HTML[#CALL:graphic($761E,24,3,1,1)]
B $761E,18,6 Masked bitmap data
;
@ $7630 label=bitmap_tree_shadow_24x1s
N $7630 Tree shadow (24x1) pre-shifted
N $7630 #HTML[#CALL:graphic($7630,24,1,1,1)]
B $7630,6,6 Masked bitmap data
;

u $7636

b $76F0 Turbo icons
D $76F0 #HTML[#CALL:anim($76F0,16,14,1,1,3)]
;
N $76F0 Frame 1 (16x14)
N $76F0 #HTML[#CALL:graphic($76F0,16,14,1,1)]
B $76F0,,4 Masked bitmap data
;
N $7728 Frame 2 (16x14)
N $7728 #HTML[#CALL:graphic($7728,16,14,1,1)]
B $7728,,4 Masked bitmap data
;
N $7760 Frame 3 (16x14)
N $7760 #HTML[#CALL:graphic($7760,16,14,1,1)]
B $7760,,4 Masked bitmap data

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

  $78B6,344,8 Tiles used to draw the pre-game screen #HTML[#CALL:graphic($78B6,8,43*8,0,0)]

b $7BE9 Graphics: Faces
;
N $7BE9 Nancy's face (32x40)
N $7BE9 #HTML[#CALL:face($7BE9)]
B $7BE9,160,4 Bitmap data, top-down format
B $7C89,20,4 Attribute data for above
;
N $7C9D Raymond's face (32x40)
N $7C9D #HTML[#CALL:face($7C9D)]
B $7C9D,160,4 Bitmap data, top-down format
B $7D3D,20,4 Attribute data for above
;
N $7D51 Tony's face (32x40)
N $7D51 #HTML[#CALL:face($7D51)]
B $7D51,160,4 Bitmap data, top-down format
B $7DF1,20,4 Attribute data for above

b $7E05 Ref'd by graphic entry 3 and 12

b $8000 temporaries?
;
@ $8000 label=test_mode_flag
  $8000,1 Test mode enable flag (cheat mode)
;
  $8001,1 TBD
;
@ $8002 label=score_bcd
  $8002,4 Score digits as BCD (4 bytes / 8 digits, little endian)
;
  $8006,1 Incremented on reset?
;
@ $8007 label=wanted_stage_number
  $8007,1 Stage number we're loading (1..5 or 6 for end credits)
;
  $8008,12 TBD

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
  $8092 Point #REGhl at tape_messsages
  $8095 ...
;
  $8088 Subroutine
  $809D,3 Call print_message
  $80A2,3 Call transition
  $80A5,3 Call draw_screen
N $80B9 Tape loading
  $80B9 Setup to load a block ???
  $80C0 Routine
  $814B Subroutine
;
@ $8169 label=tape_messsages
D $8169 Tape loading messages
;
B $8169,1 attr?
W $816A,2 Screen position (8,64)
W $816C,2 Screen attribute position (1,8)
T $816E,30 "REWIND TAPE TO START OF SIDE 2"
;
B $818C,1 attr?
W $818D,2 Screen position (88,96)
W $818F,2 Screen attribute position (11,12)
T $8191,10 "START TAPE"
;
B $819B,1 attr?
W $819C,2 Screen position (72,128)
W $819E,2 Screen attribute position (9,16)
T $81A0,15 "SEARCHING FOR 1"
;
B $81AF,1 attr?
W $81B0,2 Screen position (104,160)
W $81B2,2 Screen attribute position (13,20)
T $81B4,7 "FOUND 1"
;
B $81BB,1 attr?
W $81BC,2 Screen position (80,96)
W $81BE,2 Screen attribute position (10,12)
T $81C0,13 "STOP THE TAPE"
;
B $81CD,1 attr?
W $81CE,2 Screen position (88,128)
W $81D0,2 Screen attribute position (11,16)
T $81D2,11 "PRESS  GEAR"

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

@ $8204 label=engine_sfx_setup
c $8204 Generates engine noise
  $8204 Get speed value
  $8207,2 A = speed / 2 (top half)
  $8209 A = speed / 2 (bottom half)
  $820B A = ~A
  $820C A /= 4
  $8210 A |= 1
  $8212 L = A
  $8213 H = 3
  $8215 If in high gear goto $821D
  $821B L >>= 1
  $821D If $A23B == 0 goto $8225
  $8223 H = 1
  $8225 A = L
  $8225 Self modify iterations -- L
  $8229 Self modify delay loop -- H
  $822D Self modify delay loop -- (5 - A)
@ $8234 label=engine_sfx_play
  $8234 Return if $A230 is >= 3
  $823A Self modifying counter produces 0,1,2
  $8242 Return if nonzero
N $8243 This produces the engine sound effect.
  $8243 <Self modified> iterations
  $8245 Output 0
  $8248,4 Delay for B cycles (self modified above)
  $824C Output EAR + MIC
  $8250,4 Delay for B cycles (self modified above)
  $8254,3 Loop until iterations is zero
  $8257 Return

@ $8258 label=attract_mode
c $8258 Attract mode -- keyscan / cpu driver / text
  $8258 HL -> attract_data
  $825B Call setup_game
  $825E Reset credits/copyright message blinker to show credits
  $8262 Set speed to $190
  $8268,3 Call keyscan
  $826B Return if fire was pressed
  $826E,3 Call cpu_driver
  $8271 HL = attract mode messages "CHASE HQ" "PRESS GEAR TO PLAY" ...
  $8274 Number of messages?
  $8276 Causes blinking of "PRESS GEAR TO PLAY"
  $827C
  $827E nmessages++
  $827F Load flags
  $8280 Call print_message
  $8283 Loop while B > 0
  $8285 Attribute related
N $828B Alternate between credits and copyright messages.
  $828B,7 Blinking on/off self modified pattern
  $8292 #REGhl -> credits_messages
  $8295 Skip next LD HL based on blink above
  $8297 #REGhl -> copyright_messages
  $829A Call message_printing_related
  $829D Call transition
  $82A0 Call draw_screen
  $82A3 Loop
;
@ $82A6 label=attract_messages
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
@ $82CC label=credits_messages
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
;
@ $8320 label=copyright_messages
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

b $837A Unknown - mostly NUL bytes

c $83B5 Hooks for 128K functions
N $83B5 Some of these unknowns will be for the 128K sampled speech.
;
@ $83B5 label=unknown_hook_1
  $83B5 Only a RET
;
@ $83B8 label=engine_sfx_play_hook
  $83B8 Call engine_sfx_play
;
@ $83BB label=silence_audio_hook
  $83BB Likely silences audio in 128K mode
;
@ $83BE label=unknown_hook_2
  $83BE Only a RET
;
@ $83C1 label=turbo_sfx_play_hook
  $83C1 Likely the turbo boost sound effect
;
@ $83C4 label=engine_sfx_setup_hook
  $83C4 Call engine_sfx_setup
;
@ $83C7 label=unknown_hook_3
  $83C7 Only a RET
;
@ $83CA label=attract_mode_hook
  $83CA Call attract_mode

c $83CD Bootstrap
; This gets hit when a game finishes and we return to the attract mode.
N $83CD Builds a table of flipped bytes at $EF00.
  $83CD Point #REGhl at $EF00
@ $83D0 label=flip_table_loop
  $83D0 8 iterations
  $83D2 #REGa is the index into the table
@ $83D3 label=flip_byte_loop
  $83D3 Shift a bit out of #REGa leftwards and into #REGc rightwards
  $83D6 Loop for 8 bits
  $83D8 Write #REGc out and advance
  $83DA Loop for 256 iterations (when #REGl overflows)
  $83DC Call attract_mode_hook
  $83DF $A13C = 0
  $83E3 Zero $8002..$8006
  $83EC $8007 = 1 => Load stage 1
  $83EE credits = 2
  $83F2 Call the main loop
  $83F5 If $A139 is non-zero this calls $81AA which is the end of a string...
  $83FF Loop

@ $8401 label=main_loop
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
  $841B
  $841E
  $8421 Call setup_game
  $8424 $A13B cycles 3,2,1 then repeats
  $842E $A26B = (A * 4) OR 2
  $8435 Is $A139 zero? (used later)
  $8439 $A188 = $FF  I suspect this keeps the perp spawned
  $843E
  $8441 Call chatter
@ $8444 label=main_loop_loop
  $8444 Call drive_sfx
  $8447 Call keyscan
  $844A Call tick
  $844D Call check_user_input
  $8450 Call read_map
  $8453 Call main_loop_7
  $8456 Call main_loop_8
  $8459 Call main_loop_9
  $845C Call cycle_counters
  $845F Call engine_sfx_play_hook
  $8462 Call main_loop_10
  $8465 Call main_loop_11
  $8468 Call engine_sfx_play_hook
  $846B Call main_loop_12
  $846E Call engine_sfx_play_hook
  $8471 Call main_loop_13
  $8474 Call engine_sfx_play_hook
  $8477 Call main_loop_14
  $847A Call main_loop_15
  $847D Call main_loop_16
  $8480 Call main_loop_17
  $8483 Call main_loop_18
  $8486 Call engine_sfx_play_hook
  $8489 Call main_loop_19
  $848C Call main_loop_20
  $848F Call engine_sfx_play_hook
  $8492 Call main_loop_21
  $8495 Call main_loop_22
  $8498 Call engine_sfx_play_hook
  $849B Call main_loop_23
  $849E Call engine_sfx_play_hook
  $84A1 Call main_loop_24
  $84A4 Call speed_score
  $84A7 Call main_loop_25
  $84AA Call overtake_bonus
  $84AD Call engine_sfx_play_hook
  $84B0 Call main_loop_26
  $84B3 Call smash_bar_etc
  $84B6 Call transition
  $84B9 Call engine_sfx_play_hook
  $84BC Call draw_screen
  $84BF Call main_loop_27
  $84C2 Is test mode enabled?
  $84C5 If not, goto no_test_mode
;
@ $84C8 label=handle_test_mode
N $84C8 Test mode handling
  $84C8 Read keys 1/2/3/4/5
  $84CC Change to active high
  $84CD Mask off just keys
  $84CF If none are set then jump
  $84D1
  $84D2
  $84D5
  $84D8 Call silence_audio_hook
  $84DB
  $84DC Is bit 0 set? (key 1 to restart the level)
  $84DD Restart level if so
  $84E0 HL = &wanted_stage_number
  $84E3 Is bit 1 set? (key 2 to load the next level)
  $84E4 Jump if NOT
  $84E6 wanted_stage_number++
  $84E7 Exit via load_stage
  $84EA Is bit 2 set? (key 3 to load the end screen)
  $84EB Jump if NOT
  $84ED wanted_stage_number = 6
  $84EF Exit via load_stage
;
; (key 4 or 5 for an extra credit)
  $84F2 Increment credits unless maxed out at 9
@ $84FB label=no_test_mode
  $84FB
  $84FE
  $84FF
  $8502
  $8505
  $8507
  $8509
  $850A
  $850C Call hooked function (likely sfx)
  $850F Loop
;
  $8512
  $8515
  $8516
  $8519
  $851A
  $851D
  $851F
  $8522
  $8524 Call setup_transition
  $8527 Loop

@ $852A label=cpu_driver
c $852A CPU driver
D $852A This runs the game loop while driving the car.
  $852A Get road position
  $852D Subtract centre value. Carry flag will be set if we're on the right hand side of the road
  $8534 Set input ACCELERATE + RIGHT
  $8536 Jump cd_check_speed if we're on the left
  $8538 Subtract "ideal road position"? value. Carry will be set if we're on the right of it.
  $853D Set input ACCELERATE + LEFT
  $853F Jump cd_check_speed if we're on the right
  $8541 Set input ACCELERATE
@ $8543 label=cd_check_speed
  $8543 Move input into #REGc for the moment
  $8544 Calculate speed - 150
  $854C Load gear
  $854F Set gear to low if speed < 150, otherwise high
  $8551
  $8553 Set input GEAR
@ $8555 label=cd_set_input
  $8555 Set user input
  $8559 Call read_map
  $855C Call main_loop_9
  $855F Call cycle_counters
  $8562 Call main_loop_10
  $8565 Call main_loop_11
  $8568 Call main_loop_12
  $856B Call main_loop_13
  $856E Call main_loop_14
  $8571 Call main_loop_15
  $8574 Call main_loop_16
  $8577 Call main_loop_18
  $857A Call main_loop_20
  $857D Call main_loop_19
  $8580 Call main_loop_8
  $8583 Call main_loop_22
  $8586 Call main_loop_23
  $8589 Exit via main_loop_24

c $858C Probably the "CHASE HQ MONITORING SYSTEM" pre-game screen
  $858C
  $858F Call setup_game
  $8592
  $8594
  $8597 Call setup_transition
  $859A Call clear_screen_set_attrs
  $859D
  $859E
  $85A1
  $85A2
  $85A5 Call chatter
@ $85A8 loop_85a8
  $85A8
  $85AB Call main_loop_26
  $85AE
  $85B1
  $85B4 Call transition
  $85B7 Call draw_screen
  $85BA
  $85BD
  $85BE
  $85C0
  $85C3
  $85C4
  $85C5
  $85C7
  $85C9 Call keyscan
  $85CC
  $85CE
  $85D0 Call clear_chatter
  $85D3
  $85D6
  $85D8 If no carry call setup_transition
  $85DB Loop

b $85DD

c $85E4

c $860F

c $865A

c $86F6 18 byte chunks of this gets copied to $C82D by $C808

@ $873C label=escape_scene
c $873C Escape scene
  $873C Call silence_audio_hook
  $873F
  $8742 Call setup_game
  $8745
  $8748
  $874B
  $874E
  $8751
  $8754
  $8756
  $8759
  $875C
  $875E
  $8761
  $8764 Call chatter
@ $8767 label=8767_loop
  $8767
  $876A
  $876D
  $876F Call NZ message_printing_related
  $8772 Call read_map
  $8775 Call main_loop_10
  $8778 Call main_loop_11
  $877B Call main_loop_12
  $877E Call main_loop_13
  $8781 Call main_loop_14
  $8784 Call main_loop_15
  $8787 Call main_loop_16
  $878A Call main_loop_19
  $878D Call main_loop_23
  $8790 Call main_loop_25
  $8793 Call main_loop_26
  $8796 Call transition
  $8799 Call draw_screen
  $879C
  $879F
  $87A0
  $87A2
  $87A5
  $87A7
  $87A9
  $87AC
  $87AE
  $87B0
  $87B2
  $87B5
  $87B8
  $87BB
  $87BE
  $87C1
  $87C4
  $87C5
  $87C7
  $87CA
  $87CB
  $87CD
  $87D0
  $87D1
  $87D2
  $87D4
  $87D6 Call NZ setup_transition
  $87D9 Loop

@ $87DC label=setup_game
c $87DC Sets up the game or the level?
R $87DC I:HL Address of 14 bytes of data to be copied to $A26C+
  $87DC Preserve data pointer
  $87DD Zero $EE00..$EEFF, and point $A240 at $EE00
  $87EC A = 0
  $87ED Copy (47 bytes) at $A13E to $A16D - saved game state
  $87F8 Zero 208 bytes at $A19D
  $8801,1 Restore data pointer
  $8802 Copy the 14 bytes passed in to $A26C
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
  $882A Set $A191 to value of lods_table[2] (seems to be the perp's car)
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
  $8860,2 Clear the lights' BRIGHT bit
  $8870,3 Call silence_audio_hook
; flashing lights are 5 attrs wide, 4 high

@ $8876 label=check_user_input
c $8876 User input checking
  $8876 Load transition_control
  $8879 Address of user_input
  $887C Ignore the user's input unless transition_control is set to 4
  $8880 Clear user_input
  $8882 Return
;
  $8883 AND user_input with user input mask
  $8888 AND with mask for (Quit+Pause+Turbo), return if none
  $888B Quit bit set? Goto quit_key if so
  $888E Pause bit set? Goto pause_key if so
N $8891 Turbo was pressed
  $8891 Address of boost timer
  $8894 Return if a turbo boost is in effect
  $8897 Return if no turbo boosts are left
  $889C Set 60 ticks of boost
  $889E HL -> Random choice of (WHOAAAAA! / GREAT! / ONE MORE TIME.)
  $88A1 TBD
  $88A3 Call chatter
  $88A6 Exit via turbo_sfx_play_hook
;
@ $88A9 label=quit_key
  $88A9 If $A26A != 0 then return
  $88AE Call clear_chatter
  $88B1 Call fill_attributes
  $88B4 user_input_mask = 0  -- A is zero from fill_attributes
  $88B8 $A26A = 1
  $88BB Return
;
@ $88BC label=pause_key
  $88BC Call silence_audio_hook
  $88BF Call keyscan while PAUSE is pressed
  $88C6 Call keyscan until keys are pressed
  $88CD Call keyscan until keys are released (debounce)
  $88D4 Return

@ $88D5 label=clear_game_attrs
c $88D5 Clears the game screen attributes to zero

@ $88E2 label=clear_game_screen
c $88E2 Clears the game screen attributes and the game screen to zero

@ $88F2 label=start_sfx
c $88F2 Starts a sound effect
R $88F2 I:BC e.g. $0704 $0801 $0604
  $88F2 If $A238 is zero then goto sfx_related_assign
  $88F8 Return if $A238 < C
@ $88FA label=sfx_related_assign
  $88FA wordat($A237) = BC
  $8902 Return

@ $8903 label=drive_sfx
c $8903 Drives sound effects
  $8903 If $A23B != 0 goto $8916
  $8909 *$A23D |= *$A23C
  $8910 Counters?
  $8913 CALL NZ, start_sfx
;
  $8916 Call engine_sfx_setup_hook
  $8919 Call engine_sfx_play_hook
  $891C Call another hooked routine
  $891F If $A237 == 0 return
  $8924 #REGe = $A237 * 4
  $8927 $A237 = 0
  $892B $A238 = 0
  $892E Make #REGde full offset
  $892F #REGhl -> Base of table, but 4 bytes earlier
  $8932 Point at array element
  $8933 Load argument value into #REGde
  $8937 Load address of routine into #REGhl
  $893B Exit via routine at #REGhl
N $893C Pairs of words: (argument, address of sound effect routine)
W $893C Argument in #REGde for sound effect routine
W $893E Sound effect routine $8A0F - "pitt" and "pfff" and gear noise too?
W $8940 Argument in #REGde for sound effect routine: D = delay of 8
W $8942 Sound effect routine $89D9
W $8944 Argument in #REGde for sound effect routine
W $8946 Sound effect routine $8960 - crash
W $8948 Argument in #REGde for sound effect routine
W $894A Sound effect routine $8960 - crash
W $894C Argument in #REGde for sound effect routine
W $894E Sound effect routine $89D9
W $8950 Argument in #REGde for sound effect routine
W $8952 Sound effect routine $8A17
W $8954 Argument in #REGde for sound effect routine - "tit-tit" used when cornering
W $8956 Sound effect routine $8A17
W $8958 Argument in #REGde for sound effect routine - time running out high "bip"
W $895A Sound effect routine $8A36 - "bip-bow"
W $895C Argument in #REGde for sound effect routine - time running out low "bow"
W $895E Sound effect routine $8A36 - "bip-bow"

@ $8960 label=sfx8960
c $8960 Sound effect - crash
R $8960 I:D Inner loop count
  $8960 #REGhl -> Effect data table
  $8963 Length of table is 93
@ $8965 label=sfx8960_loop_outer
  $8965 Set inner loop counter
@ $8966 label=sfx8960_loop_inner
  $8966 #REGa = 16
  $8968 Top bit set of this byte?
  $896A Don't clear the EAR bit
  $896C Otherwise clear the EAR bit
  $896E Output
  $8970 Rotate the effect data
  $8972 Delay
  $8974 Loop while #REGb
  $8976 Move to the next byte of effect data
  $8977 Loop while #REGc
  $897B Return
B $897C Effect data table

@ $89D9 label=sfx89d9
c $89D9 Sound effect - not sure
R $89D9 I:D Delay value
  $89D9 Length of table is 32
  $89DB #REGhl -> Effect data table
  $89DE Zero output byte
@ $89DF label=sfx89d9_loop
  $89DF B = *HL
  $89E0 Output to
  $89E2 Delay for 'D' iterations
  $89E6 Loop for 'B' iterations
  $89E8 Wiggle the EAR bit
  $89EA Move to the next byte of effect data
  $89EB Loop while #REGc
  $89EE Return
@ $89EF label=sfx_data
B $89EF Effect data table

@ $8A0F label=sfx8a0f
c $8A0F Sound effect - "pitt" and "pfff"
R $8A0F I:D Affects delay loops
R $8A0F I:E Inner loop count
  $8A0F Half duty cycle
  $8A11 Return if 1
;
@ $8A17 label=sfx8a0f_loop_outer
  $8A17 Set inner loop counter
@ $8A18 label=sfx8a0f_loop_inner
  $8A18 Call rng
  $8A1B #REGa &= 16
  $8A1D If zero goto sfx8a0f_continue
  $8A1F Delay loop of (24 - #REGd) iterations
  $8A25 #REGa = 24 set EAR and MIC bits <not sure of the effect>
  $8A27 Output
  $8A29 Delay loop of #REGd iterations
  $8A2C Output
@ $8A2F label=sfx8a0f_continue
  $8A2F Loop while #REGc
  $8A32 Loop while #REGd
  $8A35 Return

; a zero in bit 3 activates the MIC output, whilst a one in bit 4 activates the
; EAR output and the internal speaker. However, the EAR and MIC sockets are
; connected only by resistors, so activating one activates the other; the EAR is
; generally used for output as it produces a louder sound.

@ $8A36 label=sfx8a36
c $8A36 Sound effect - "bip-bow"
R $8A36 I:D Delay at start
R $8A36 I:E Same as D
  $8A36 C = 20
  $8A38 Inner loop counter. H is decremented and restored from L each loop.
@ $8A3B label=sfx8a36_loop
  $8A3B Delay loop of D iterations
  $8A3E D = E
  $8A3F Delay loop of (24 - C) iterations
  $8A45 A = 24 set EAR and MIC bits <not sure of the effect>
  $8A47 Output A
  $8A49 Delay loop of C iterations
  $8A4C Output 0
  $8A4F Loop while H
  $8A52 H = L
  $8A53 Loop while C
  $8A56 Return

@ $8A57 label=main_loop_7
c $8A57
  $8A74,3 Call silence_audio_hook
  $8ABB,3 Call fill_attributes
  $8ACD,2 transition_control byte
  $8B06 Set score
  $8B10,3 Call increment_score
  $8B87,3 Point at "CLEAR BONUS - TIME BONUS - SCORE" messages
  $8B8A,3 Call message_printing_related
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
  $8C50 Call message_printing_related
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
  $8E42 Address of current message thing (frame delay, flags, attrs, bufaddr, attraddr, string)
  $8E45 B = 0 -- self modified by ($8E58 increments it), ($8E8D resets it to 1)
;
@ $8E47 label=sub_8e47_loop
  $8E47 sub_8e42_1
  $8E49 A = 20 -- self modified by $8E4C, $8E51, ($8E84 resets it perhaps)
  $8E4B A--
  $8E4C self modify $8E49
  $8E4F Return if non-zero
;
  $8E50 A = *HL
  $8E51 self modify $8E49
  $8E54 increment $8E46, affects TIME UP state if disabled
  $8E5B B++
;
@ $8E5C label=sub_8e42_1
  $8E5C HL++
  $8E5D A = *HL
  $8E5E test flags
  $8E5F jump if zero
  $8E61 Call print_message
  $8E64 Loop?
  $8E66 transition_control = HL[-1]
  $8E6B Return

@ $8E6C label=print_message
c $8E6C Print a message
R $8E6C I:A  Flags byte
R $8E6C I:HL -> as yet unnamed message structure
  $8E6C Preserve/bank flags byte in #REGa
  $8E6D Preserve #REGbc
  $8E6E Skip flags byte
  $8E6F Load attribute byte into #REGa
  $8E71 Load back buffer address into #REGde
  $8E75 Load attribute address into #REGbc
N $8E79 #REGhl now points at the string.
  $8E79 Call alt draw_string entry point
  $8E7C Restore #REGbc
  $8E7D Return

@ $8E7E label=message_printing_related
c $8E7E Message printing related, called for TIME UP, CONTINUE, etc. messages
R $8E7E I:HL -> something  e.g. -> credits_messages
  $8E7E A = 2
@ $8E80 label=j_8e80
  $8E80 transition_control = A
  $8E83 Modify 'LD A' at $8E49 with frame delay
  $8E87 Modify 'LD HL' at $8E42 to be message struct e.g. $82CD
  $8E8B Modify 'LD B' at $8E45 to be ?
  $8E90 Return

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
  $8EE4 Exit via pf_attrs_bit

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

@ $8F5F label=main_loop_23
c $8F5F
  $8FCE,5 Return if floating_arrow is zero
  $9003 DE = E * 7
  $900B Push return address onto stack
  $901C <return>

c $9023
  $903A Push return address onto stack
  $903E
  $904B <return>

; unclear if/how this routine gets entered
c $9052

c $916C
  $9171

c $924D
  $9252

c $9278

c $92E1 Drives the sprite plotters  WHAT CALLS THIS PROB $901B
  $93D3,3 Exit via plot_masked_sprite
  $93D6,3 Exit via plot_sprite
  $93D9,3 Exit via plot_masked_sprite_flipped
  $93DC,3 Exit via plot_sprite_flipped
  $93E8,4 #REGix = Base of jump table
  $9496 E = -E

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
c $9542 Sprite plotter for back buffer, which flips
  $9542 TBD

c $9618 Random number generator
R $9618 O:A Random byte?
; Disable this and only Lambos spawn on the left. TBD what other effects it has.
; $F0C6 also modifies these random bytes.
@ $9618 label=rng_seed
B $9618,3 Seed / initial state
@ $961B label=rng
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

b $98A9 In-game chatter structures
;
@ $98A9 label=pilot_turn_left_chatter
  $98A9,0 Pilot ($00)
W $98AA,2 -> "THIS IS SPECIAL INVESTIGATION AIRBORNE."
W $98AC,2 -> "THE TARGET VEHICLE HAS TURNED"
W $98AE,2 -> "LEFT UP AHEAD... OVER."
  $98B0,1 TBD $FE
W $98B1,2 -> Tony: "WE READ LOUD AND CLEAR! OVER." <STOP>
;
@ $98B3 label=pilot_turn_right_chatter
  $98B3,0 Pilot ($00)
W $98B4,2 -> "THIS IS SPECIAL INVESTIGATION AIRBORNE."
W $98B6,2 -> "THE TARGET VEHICLE HAS TURNED"
W $98B8,2 -> "RIGHT UP AHEAD... OVER."
  $98BA,1 TBD $FE
W $98BB,2 -> Tony: "WE READ LOUD AND CLEAR! OVER." <STOP>
;
@ $98BD label=acknowledge_chatter
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
@ $98D6 label=smash_chatter
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
@ $98F8 label=get_moving_chatter
  $98F8,1 Raymond ($02)
W $98F9,2 -> "LET'S GET MOVIN' MAN!"
  $98FB,1 <STOP>
;
@ $98FC label=time_running_out_chatter
  $98FC,1 Nancy ($01)
W $98FD,2 -> "THIS IS NANCY AT CHASE H.Q."
W $98FF,2 -> "IF YOU KEEP MESSIN' AROUND LIKE THAT"
W $9901,2 -> "YOUR TIME IS GOING TO RUN OUT... OVER."
  $9903,1 <STOP>
;
@ $9904 label=ouch_chatter
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
@ $9917 label=turbo_chatter
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
@ $992A label=failed_chatter
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
@ $9941 label=lets_go_chatter
  $9941,1 Tony ($03)
W $9942,2 -> "LET'S GO. MR. DRIVER."
  $9944,1 <STOP>

@ $9945 label=chatter
c $9945 Chatter routine
R $9945 I:A TBD
R $9945 I:HL Address of message set
  $9945 B = A
  $9946
  $9949 If A == 0 jump forward to 9955
  $994C If A >= 3 jump forward to 9955
; A is 1 or 2
  $9950
  $9953 If A >= B return
;
  $9955 $963E = B
  $9959 Set message set pointer
  $995C $9633 = 0
  $9960 $963D = 1
  $9964 Return

@ $9965 label=main_loop_26
c $9965 Noise effect, Message plotting
  $9965 A = TBD963d - 1
  $9969 if A was zero jump
  $996B A--
  $996C if A was zero jump to sub998f_do_noise_effect
  $996E A--
  $996F if A was NOT zero jump
; A always zero here
  $9971 Decrement noise_counter
  $9975 A = *HL A = noise_counter
  $9976 Jump if noise_counter was non-zero
  $9979 Zero this
  $997C Call noise_plot_attrs -- clear to black
;
  $997F Plot space character
  $9981 {$AA ROR 1 becomes $55 becomes $AA and so on
  $9983 probably a delay / alternating call
  $9984 }
  $9987 ...and then gets overwritten?
  $9989 jump on $55 -> $AA transitions?
  $998C,3 Goto plot_mini_font_1
;
@ $998F label=sub998f_do_noise_effect
  $998F If noise_counter > 0 call noise_effect
  $9996 If $9633 == 0 jump $99b6
  $9999
  $999A
  $999C $9633--
  $999D
  $99A0
  $99A1 If A was zero jump sub998f_another_something
  $99A3 Load address of next character
  $99A6 Go back 1
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
  $99BD HL = next_message
  $99C0 A = *HL first char of
  $99C1 String terminator? jump to clear_chatter if so
  $99C5,2 this tests for $FE -- different terminator?
  $99C7 If not $fe goto chatter_message
; $FE means clear first?
  $99C9
  $99CA next_message = *HL
  $99D1 goto 99e4_exit
;
@ $99D3 label=clear_chatter
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
  $9A3D
  $9A3E Call plot_mini_font_2, with #REGd as ASCII character to plot
  $9A41
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

@ $9AAB label=plot_face
R $9AAB I:HL Address of face to plot (32x40 bitmap followed by 4x5 attribute bytes)
R $9AAB I:DE Address of screen location (real screen)
c $9AAB Plots a face
  $9AAB Byte length of bitmap - counter
  $9AAE Save current screen address
@ $9AAF label=pf_loop
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
@ $9ACE label=pf_attrs_bit
  $9ACE Restore current screen address
  $9ACF Extract line bits (0..7)
  $9AD5 Turn it into an attribute address (works for first band only?)
  $9AD8 Byte length of attributes - counter
@ $9ADA label=pf_attrs_loop
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
  $9AFE If not string terminator, goto pmf_regular_char
N $9B02 String terminator
  $9B02 $BF / 8?
  $9B04
  $9B06
@ $9B08 label=rpmf_egular_char
  $9B08 A = A*5+2
  $9B0E 8 - $C0 / 8 ??
  $9B12 divider / rounding?
  $9B19 Modify jump table - A * 4 - 4 bytes per sequence
  $9B21 General multiplier by A
  $9B24 Self modify $9B88 which is a mask
  $9B27 Get ASCII character
N $9B28 Turn ASCII into glyph IDs
  $9B28 likely the screen addr top
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

@ $9BCF label=tick
c $9BCF Game timer
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
  $9C15 user_input_mask = $FF (allow all keys)
  $9C19 Loop back to $9BF9
@ $9C1B label=tick_time_up
  $9C1B,3 Point at "TIME UP" message
  $9C1E,3 Call message_printing_related
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
  $9C6A transition_control = 3 -- Flag set to zero when attributes have been set
  $9C6D turbos = 3
  $9C70 Set remaining time to 60 (BCD) [doesn't affect stuff if altered?!]
@ $9C7E label=tick_print_continue
  $9C7E,3 Print "CONTINUE THIS MISSION" messages
  $9C81,3 Call message_printing_related
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
  $9D17 HL = &score_bcd
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
@ $9D37 label=cob_overtake_loop
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
  $9D4A B--, goto cob_overtake_loop
  $9D4C $A22B = 0
  $9D50 Return

c $9D51 Update scoreboard and flashing lights
;
B $9D51
@ $9D57 label=hi
T $9D57 "HI"
@ $9D59 label=lo
T $9D59 "LO"
@ $9D5B label=stage
T $9D5B "STAGE " message shown in the score area.
@ $9D61 label=stage_n
B $9D61 #R$9D61 writes the current score number here in ASCII.
;
@ $9D62 label=update_scoreboard
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

@ $9E11 label=plot_turbos_and_scores
c $9E11 Draws the turbo sprites and updates the displayed scores.
  $9E11 If no turbo boosts remain jump to plot_scores_only
  $9E17 Preserve number of turbo boosts in #REGc
  $9E18 Read boost time remaining and set flags
  $9E1C Point #REGhl at base of turbo sprites
  $9E1F Avoid frame increment and address calculation if possible
  $9E21 Get frame number (0, 1 or 2)
  $9E29 Self modify frame number
  $9E2C Avoid frame address calculation if possible
  $9E2E 56 bytes per frame
  $9E31 Calculate the frame address
@ $9E35 label=ptas_turbo_setup
  $9E35 Self modify $9E44 to load #REGsp with address of frame
  $9E38 Self modify $9E79 to restore #REGsp once drawing is done
  $9E3C Low byte of back buffer draw address
;
N $9E3E Draw a whole frame of turbo sprite.
N $9E3E Note that the frame data is stored in memory inverted (bottom first), so we draw the bottom row first and then proceed upwards.
@ $9E3E label=ptas_turbo_loops
  $9E3E,3 Base address of frames
  $9E41 Temporarily decrement the number of turbos remaining to draw
  $9E42 All but the final turbo sprite use the zeroth frame
  $9E44 #REGsp is pointed at frame data (self modified)
  $9E47 Restore number of turbos
  $9E48 Form back buffer position
  $9E4B Preserve partial back buffer position
  $9E4C Height of frame
;
N $9E4E Draw a scanline of frame.
@ $9E4E label=ptas_turbo_scanline
  $9E4E Pop some frame data off the stack (E is mask data, D is bitmap data)
  $9E4F Write to screen: Screen = (Screen AND Mask) OR Bitmap
  $9E53 Advance screen address to next column
  $9E54 (Repeat)
  $9E59 Step back
  $9E5A Move up a row (standard pattern)
@ $9E70 label=plot_turbo_continue
  $9E70 Loop until out of rows
  $9E72 Restore back buffer draw address
  $9E73 Advance screen address to next turbo position
  $9E75 Decrement number of turbos
  $9E76 Loop until none are left
  $9E78 Restore #REGsp
@ $9E7B label=plot_speed_only
  $9E7B Point #REGde at speed digits screen position (144, 9)
  $9E7F Load speed into #REGde
  $9E7E Bank
;
N $9E83 Scale internal speed (0..511) to displayed speed by multiplying by 82 then discarding the bottom two digits.
  $9E83 Initialise counter
  $9E86 Do 7 digits / iterations
  $9E88 Multiplier of 82 (a percentage: speed * multiplier / 100 = displayed speed)
  $9E8A Shift one bit out
  $9E8B Jump if not adding
  $9E8D Add
@ $9E8E label=ptas_speed_multiply_loop
  $9E8E Double
  $9E8F Loop
;
N $9E91 Count 10,000s.
  $9E91 10000
  $9E94 Initialise #REGd and #REGe counters to -1
  $9E97 ?Clear carry flag?
@ $9E98 label=ptas_speed_10000s_loop
  $9E98 Increment counter
  $9E99 Decrease total by 10,000
  $9E9B Loop until #REGhl goes negative
  $9E9D Correct for overshoot
  $9E9E ?Clear carry flag?
;
N $9E9F Count 1,000s.
  $9E9F 1000
@ $9EA2 label=plot_speed_1000s_loop
  $9EA2 Increment counter
  $9EA3 Decrease total by 1,000
  $9EA5 Loop until total goes negative
  $9EA7 Correct for overshoot
;
N $9EA8 Count 100s.
  $9EA8 Initialise counter (to zero not -1 as in previous cases)
  $9EA9 100
@ $9EAC label=ptas_speed_100s_loop
  $9EAC Increment counter
  $9EAD Decrease total by 100
  $9EAF Loop until total goes negative
  $9EB1 Correct for starting early
;
N $9EB2 Plot speed digits.
N $9EB2 We divide by 100 here by throwing away the bottom two digits.
  $9EB2 Stack 100s counter
  $9EB3 Stack 1,000s counter
  $9EB5 Get 10,000s counter
  $9EB6 Plot a digit for 10,000s
  $9EBA Plot a digit for 1,000s
  $9EBE Plot a digit for 100s
;
N $9EC1 Time.
  $9EC1,3 Point #REGde at time digits screen position (120, 9)
  $9EC4 Bank
  $9EC5 Point #REGde at time_bcd (one BCD byte)
  $9EC8 Point #REGhl at time_digits + 1
  $9ECB One pair of digits
  $9ECD Call plot_led_digits
;
N $9ED0 Distance.
  $9ED0,3 Point #REGde at distance_bcd + 1 (the second of two BCD bytes)
  $9ED3 L = Distance byte from first 'hazard' (the perp)
  $9ED7 H = 17th byte from first hazard (not sure yet how they relate)
;
N $9EDB Count 1000s (no loop here - it's not required)
  $9EDB 1,000
  $9EDE Decrease total by 1,000
  $9EE0 BCD "10"
  $9EE2 Jump if distance >= 1,000
  $9EE4 Otherwise correct for overshoot
  $9EE5 BCD "00"
;
N $9EE6 Count 100s
  $9EE6 100
@ $9EE9 label=ptas_scores_distance_100s_loop
  $9EE9 Increment counter
  $9EEA Decrease total by 100
  $9EEC Loop until total goes negative
  $9EEF Correct for overshoot
  $9EF0 Correct for starting early
  $9EF1 distance_bcd[1] = counter
;
N $9EF2 Count 10s
  $9EF2 10 (#REGb's already zero)
  $9EF4 ?Clear carry flag?
  $9EF5 #REGa = $F0 (BCD)
@ $9EF7 label=ptas_scores_distance_10s_loop
  $9EF7 Increment counter
  $9EF9 ?Clear carry flag?
  $9EFA Decrease total by 10
  $9EFC Loop until total goes negative
  $9EFF Correct for overshoot
  $9F00 OR in the remainder
  $9F01 distance_bcd[0] = #REGa
  $9F03,3 Point #REGde at distance digits screen position (136,33)
  $9F06 Bank
  $9F07,3 Point #REGde at distance_bcd + 1 (two BCD bytes)
  $9F0A,3 Point #REGhl at distance_digits + 3
  $9F0D Two pairs of digits (4 digits)
  $9F0F Call ptas_led_digits
  $9F12,3 Point #REGde at score digits screen position
  $9F15 Bank
  $9F16,3 Point #REGde at score (four BCD bytes)
  $9F19,3 Point #REGhl at score digits (eight bytes)
  $9F1C B = 4
;
N $9F1E Plots scoreboard digits (#REGde -> BCD) only if different than recorded values (#REGhl -> byte per digit). #REGb is the count.
@ $9F1E label=ptas_led_digits
  $9F1E Read a pair of digits (BCD)
  $9F1F Save digits for later
  $9F20 Unpack a BCD digit
  $9F26 If different than stored then plot
  $9F29 Move screen position
;
@ $9F2C label=ptas_led_next_half
  $9F2C Move to next recorded value
  $9F2D Examine next digit
  $9F30 If different than stored then plot
  $9F33 Move screen position
;
@ $9F36 label=ptas_led_next_whole
  $9F36 Move to next recorded value
  $9F37 Move to next digits
  $9F38 Loop until B is zero
  $9F3A Return
;
@ $9F3B label=ptas_led_plot_1st
  $9F3B Update drawn digit
  $9F3C Done
  $9F3C Plot the digit
  $9F3F Jump back to handle next digit (second half of a pair)
;
@ $9F41 label=ptas_led_plot_2nd
  $9F41 Update drawn digit
  $9F42 Plot the digit
  $9F45 Jump back to handle hext digit (next whole pair)

@ $9F47 label=ledfont_plot
c $9F47 Plots an 8x15 LED font digit
D $9F47 This appears to be set up to work for Y coordinates of 1, 9, 17, ...
R $9F47 I:A  Glyph ID (0..9)
R $9F47 I:DE Address of (real) screen location
  $9F47 Bank
  $9F48 #REGhl = &ledfont[glyphID * 15]
  $9F55 Save the screen address
  $9F56 Save #REGd
  $9F57 Transfer a byte
  $9F59 Next row down
  $9F5A Undo LDI's increment
  $9F5B Transfer six more rows
  $9F71 Restore #REGd and move to an even line
  $9F73 Move to the next screen row (group of 8)
  $9F77 Transfer eight more rows
  $9F96 Move to next column
  $9F97 Bank
  $9F98 Return

c $9F99 Another draw string entry point?
  $9F99
  $9F9A Bank
  $9F9B HL' = BC   so BC is ptr to text
  $9F9C DE = 32
  $9F9F C = A
  $9FA0 Bank
  $9FA1 goto draw_string_entry
;
@ $9FA3 label=draw_string
; DE -> screen address
; HL -> string
  $9FA3 Draws a string
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
  $A004
  $A005 B = 4
@ $A007 label=dc_loop1
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
@ $A019 label=dc_loop2
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
  $A023
  $A024 B = 7
@ $A026 label=dc_loop3
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
  $A031
  $A032 B = 7
@ $A034 label=dc_loop4
; single height inverted
  $A034 *DE = ~*HL
  $A037 Move to next glyph row
  $A038 Move to next row
  $A039 While iterations remain, goto loop4
  $A03B JR $A0A4
;
@ $A03D label=dc_double_height_glyph
N $A03D Plots double-height glyphs. DE->screen HL->glyph def
  $A03D
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
@ $A074 label=dc_a074
  $A074
  $A075 E++
  $A076
  $A077
  $A078 *HL |= C
  $A07B HL += DE
  $A07C *HL |= C
  $A07F
  $A080 L++
  $A081
  $A082 Return
;
  $A083
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
@ $A0A4 label=dc_a0a4
  $A0A4
  $A0A5 E++
  $A0A6
  $A0A7 *HL |= C
  $A0AA L++
  $A0AB
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

c $A0CC Keyscan
@ $A0CC label=kempston_flag
B $A0CC,1 Set to 1 if Kempston joystick is chosen, 0 otherwise
@ $A0CD label=keydefs_probably
B $A0CD,8 must be keydefs
@ $A0D5 label=user_input
B $A0D5,1 User input: QPBFUDLR - Quit Pause Boost Fire Up Down Left Right. Note that attract mode is driven through this var.
;
@ $A0D6 label=keyscan
C $A0D6 Main input handler
R $A0D6 O:A User input byte (as $A0D5)
  $A0D6 If not Kempston then goto $A0F3
  $A0DC Read Kempston joystick port. Returns 000FUDLR active high
  $A0E0
  $A0E1
  $A0E3
  $A0E6
  $A0E9
  $A0EA
  $A0EB
  $A0EC
  $A0EE
  $A0EF
  $A0F0
  $A0F1
;
@ $A0F3 label=ks_keyboard
  $A0F3
  $A0F5
  $A0F8
;
@ $A0FB label=ks_common
  $A0FB
  $A0FD
  $A0FF
  $A100
  $A102
  $A104
;
  $A105
  $A107
  $A109
  $A10A
  $A10C
;
  $A10E
  $A111 Return
;
  $A112 Outer keyboard loop
  $A113
  $A114
  $A117 clear carry
  $A118
  $A11A loop while top bit set?
  $A11C
  $A11D Return
;
@ $A11E label=keyscan_inner
  $A11E Inner keyboard loop.... what's in #REGa?
  $A11F B=(A&7)+1, C=5-(A>>3)
  $A12D A = $FE ROR B
  $A132 Port $FE is the AND of all columns/rows? This is the main keyboard access
;
  $A134 C = A ROR C
  $A138 Return

g $A139
  $A139,1 This is always zero, yet the code checks it. If it's set then $83F5 uses it to jump to $81AA, which is the end of a string. $8435 tests it too and calls <chatter> when it's zero, which it always is. $8AD6 also tries to use it to jump into a string (twice). Could be dead code or 128K version hook?
;
@ $A13A label=current_stage_number
  $A13A,1 Current stage number
  $A13B,1 Used by $8425  -- seems to start at 4 then cycle 3/2/1 with each restart of the game, another random factor?
;
@ $A13C label=overtake_bonus
  $A13C,1 Overtake combo bonus counter. BCD. This increases by 2 for each overtake and is reset on a crash.
;
@ $A13D label=credits
  $A13D,1 Number of credits remaining (2 for a new game)
  $A13E,47 Canned data or Data restored for attract mode?
  $A16D
; this might be an idle timer, when it hits zero Raymond cajoles us
  $A16E,1 used during attract mode, road gradient/angle or something?
;
@ $A16F label=user_input_mask
  $A16F,1 user_input mask, set to $C0 (Quit+Pause) when perp is fully smashed, or $FF otherwise
;
@ $A170 label=turbos
  $A170,1 Number of turbo boosts remaining (3 for a new game)
;
W $A171,2 seems to be the horizon level, possibly relative (used during attract mode)
  $A174,1 Low/high gear flag?
  $A175,8 Score digits. One digit per byte, least significant first. This seems to be recording what's on screen so digit plotting can be bypassed.
;
@ $A175 label=score_digits
  $A175,8 Score as displayed. Stored as one digit per byte. Least significant digit first.
;
@ $A17D label=time_sixteenths
  $A17D,1 Seems to be a 1/16ths second counter. Counts from $F to $0. $A17E is decremented when it hits zero.
;
@ $A17E label=time_bcd
  $A17E,1 Time remaining. Stored as BCD.
;
@ $A17F label=time_digits
  $A17F,2 Time remaining as displayed. Stored as one digit per byte.
;
@ $A181 label=distance_digits
  $A181,4 Distance as displayed. Stored as one digit per byte.
;
@ $A185 label=a185
  $A185,1 Used by read_map when handling the hazards section
;
@ $A186 label=horizon_attribute
W $A186,2 Attribute address of horizon. Points to last attribute on the line which shows the ground. (e.g. $59DF)
;
@ $A188 label=hazards
  $A188,120 Set by $843B. Groups of 20 bytes. This area looks like a table of spawned vehicles or objects. The first entry is the perp.
;  +0 is a used flag, either $00 or $FF
;  +1 looks distance related
;  +2/3/4/5/6 TBD
;  +7 byte  TBD used by hazard_hit
;  +8 byte  gets copied from the hazards table
;  +9 word  address of e.g. car_lods
; +11 word  address of routine
; +13 word  set to $190 by fully_smashed (likely a horizontal position)
; +15 byte  TBD used by hazard_hit, counter which gets set to 2 then reduced
; +17 byte  TBD used by hazard_hit, indexes table $ACDB
; +18 byte  TBD used by hazard_hit
; +19 byte  TBD used by hazard_hit
;  $A19C,20
;  $A1B0,20
;  $A1C4,20
;  $A1D8,20
;  $A1EC,20
;
  $A200 TBD
;$A19D+ Memory gets wiped
;
  $A220,1 #R$8014 sets this to the level number that it's going to load [but I don't see it using it again]. #R$858C sets it to $F8. #R$BC3E uses it to avoid some work.
;
  $A221,1 $ABCE, $AD0D reads  I don't yet see what writes it.
;
  $A222,1 $8F9A reads  $ADA0, $AE83, $AF41 writes
;
  $A223,1 Stage number as shown. Stored as ASCII.
;
  $A224,1 Hazards related. $AAC6, $AB33 reads  $AB96, $C013 writes
;
@ $A225 label=stop_car_spawning
  $A225,1 Inhibits cars from spawning.
;
@ $A226 label=correct_fork
  $A226,1 Holds the correct direction to take at forks (1 => left, 2 => right).
;
@ $A227 label=floating_arrow
  $A227,1 Shows the floating left/right arrow (0 => off, 1 => left, 2 => right).
;
  $A228,1 $B421 reads  $B4C8 writes
;
  $A229,1 <Timing related?>
;
  $A22A,1 Used by $B6D6 and others
;
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
  $A230,1 Used by draw_screen. Seems to inhibit car spawning too.
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
@ $A234 label=counter_A
  $A234,1 Cycles 0-1-2-3 as the game runs (used to animate turbo smoke CHECK)
@ $A235 label=counter_B
  $A235,1 Cycles 0-1 as the game runs (used to toggle flash the red/blue lights)
@ $A236 label=counter_C
  $A236,1 Cycles 0-1-2-3 as the game runs - at half rate
;
N $A237 These seem to get altered even when no sound is being produced.
@ $A237 label=sfx_0
  $A237,1 Used by $88F2
@ $A238 label=sfx_1
  $A238,1 Used by $88F2
;
  $A239,1 Used by $F265
;
  $A23A,1 Used by $F2F6
;
  $A23B,1 loaded by $821d, drive_sfx, $c0ee, $fd21 set by $C102
;
  $A23C,1 $8909, $BE28 reads  $A3FA, $BDFF, $BE2C writes
;
  $A23D,1 $BE33 reads  $890F, $A3D2, $BDFC, $BE37 writes
;
  $A23E,1 $B104, $B40C reads  $A3FD, $A52A, $B07D, $B322, $B404, $B443 writes
;
  $A23F,1 numerous references, seems related to level position
;
; seems to 'rotate' the whole buffer
W $A240,2 Used by $B8F6, $BBF7, $87E0  seems to point at $EE00..$EEFF
;
@ $A242 label=curvature_byte
  $A242,1 Current curvature byte (minus 16) [$BE3E reads $BC17,$BE7D writes]
;
@ $A243 label=height_byte
  $A243,1 Current height byte (minus 16) [$BE90 reads $BC1A,$BECF writes]
;
@ $A244 label=leftside_byte
  $A244,1 [$BF9E reads $BC1D,$BFDD writes]
;
@ $A245 label=rightside_byte
  $A245,1 Current rightside byte [$BF55 reads $BC20,$BF94 writes]
;
@ $A246 label=hazards_byte
  $A246,1 Current hazards byte (minus one) [$BFE7 reads $BC23,$C055 writes]
;
@ $A247 label=lanes_counter_byte
  $A247,1 [$BEDB reads $BC26,$BF15,$BF29 writes]
;
  $A248,1 [$A955 reads $C457,$C526 writes]
;
  $A249,1 [$A539,$B988,$BAE0,$BB6E reads $BA89,$BC2C writes]
;
@ $A24A label=speed
W $A24A,2 Speed (0..511). Max when in lo gear =~ $E6, hi gear =~ $168, turbo =~ $1FF.
;
  $A24C,1 $B1B7 reads  $B1E4 writes
;
  $A24D,1 $B3B4, $B432 reads  $B2E5, $B314, $B32A writes
;
@ $A24E label=boost
  $A24E,1 Turbo boost time remaining (60..0)
;
  $A24F,1 $8BDF, $B070, $B0D4, $B3BA writes ... but no readers?
;
  $A250,1 Turn severity (0/1/2)
;
  $A251,1 Some sort of flip flag used for car rendering (observed 0/1)
;
  $A252,1 TBD
;
@ $A253 label=gear
  $A253,1 0 => Low gear, 1 => High gear
;
  $A254,1 Cleared by $884B
;
@ $A255 label=distance_bcd
  $A255,2 Distance as BCD (2 bytes / 4 digits, little endian)
;
  $A257,1 ...
;
  $A258,1 Used by $B8D8
  $A259,1 Used by $B92B
  $A25A,1 Used by $B8D2
  $A25C,1 Used by $B84E
  $A25D,1 Used by $B85D
  $A268,1 Used by $BB69 - skips routine if not set
  $A26A,1 Used by quit_key - quit in progress flag?
  $A26B,1 Used by $8432
;
@ $A26C label=road_pos
W $A26C,2 Road position of car. Left..Right = $1E2...$03A, $105 is centre.
;
@ $A26E label=road_curvature_ptr
W $A26E,2 Address of the previous curvature data byte
@ $A270 label=road_height_ptr
W $A270,2 Address of the previous height data byte
@ $A272 label=road_lanes_ptr
W $A272,2 Address of the previous lane data byte
@ $A274 label=road_rightside_ptr
W $A274,2 Address of the previous right object data byte
@ $A276 label=road_leftside_ptr
W $A276,2 Address of the previous left object data byte
@ $A278 label=road_hazard_ptr
W $A278,2 Address of the previous hazard object data byte

b $A27A Font: 8x7 bitmap
D $A27A #HTML[#CALL:graphic($A27A,8,41*7,0,0)]
B $A27A,,7 Exclamation mark
B $A281,,7 Open bracket
B $A288,,7 Close bracket
B $A28F,,7 Comma
B $A296,,7 Full stop
B $A29D,,7 0..9
B $A2E3,,7 A-Z

@ $A399 label=main_loop_22
c $A399

@ $A579 label=main_loop_14
c $A579

@ $A60E label=cycle_counters
c $A60E Counters
  $A60E Cycle $A234 0-1-2-3
  $A616 Cycle $A235 0-1
  $A61B Return if it's zero
  $A61C Cycle $A236 0-1-2-3 at half rate
  $A622 Return

@ $A623 label=hazard_template
b $A623 Hazard template?
D $A623 $A845 copies these 20 bytes to hazards array.
B $A623,1 Used flag
W $A62C,2
W $A62E,2 Routine at $A8CD
W $A630,2

c $A637

b $A7E7

@ $A7F3 label=main_loop_9
c $A7F3
  $A7F7 Don't spawn any cars if this flag is set

c $A89C

c $A8CD Hazard handler routine
  $A8D3 Check 'don't spawn cars' flag

@ $A955 label=main_loop_18
c $A955 TBD
  $A955 If $A248 == 0 Return
  $A95A If $A254 == 0 Return
  $A95F Point #REGde at the stack
  $A962 Call rng
  $A965 Stack 1 if it's +ve or zero, or 2 if it's -ve
  $A96D Call rng
  $A970 Stack that random byte
  $A972 Self modify 'LD A' at $A97E to load 1
  $A977 Self modify 'LD A' at $COBB to load 1
  $A97A Self modify 'LD A' at $A9DE to load 1
  $A97D Return

@ $A97E label=main_loop_20
c $A97E

c $A9DE

c $AA38
D $AA38 $AB89 self modifies $8FA4 to call this.

@ $AAC6 label=main_loop_21
c $AAC6

@ $AB33 label=main_loop_17
c $AB33 Helicopter related
  $AB52 Point #REGhl at pilot "turn left" messages
  $AB59 Point #REGhl at pilot "turn right" messages

@ $AB9A label=spawn_hazards
c $AB9A
;
@ $ABF6 label=sh_spawn
N $ABF6 Spawns hazards in the road, like barriers and tumbleweeds.
N $ABF6 I:B Stored at entry+5  e.g. $20/$46/$56/$50/$B4
N $ABF6 I:C Stored at entry+1  e.g. $13
R $ABF6 I:DE Is an offset into a hazards data table (0 => tumbleweed, 3 => barrier)
  $ABF6 Bank entry registers
  $ABF7 6 iterations
  $ABF9 Point #REGhl at hazards table
  $ABFC Stride of 20 bytes
@ $ABFF label=sh_loop
  $ABFF Check the flag byte to see if the entry is used (it's either $00 if empty, or $FF if used, so rotating it in place is not a problem)
  $AC01 Jump to sh_found_spare if it didn't carry
  $AC03 Move to the next entry
  $AC04 Loop while iterations remain
  $AC06 TBD
  $AC07 Return
;
@ $AC08 label=sh_found_spare
N $AC08 #REGhl points to the unused entry
  $AC08 #REGix = #REGhl
  $AC0B Zero 20 bytes at #REGhl
  $AC15 Restore entry registers
  $AC16 wordat(IX + 11) = Address of $AC3C routine
  $AC1F Point #REGhl at the hazards data table entry (*$5CF6 -> $5E40 + 3)
  $AC23 Copy flag TBD
  $AC28 Copy lod address (e.g. tumbleweed_lods)
  $AC31 IX[5] = B TBD
  $AC34 IX[1] = C TBD
  $AC37 Mark the entry as used
  $AC3B Return

@ $AC3C label=hazard_hit
c $AC3C Test for collision with hazard
R $AC3C I:IX Address of hazard structure ($A188+)
  $AC3C IX[15] appears to be a delay of some sort
  $AC3F If IX[15] != 0 then goto hh_ac92
  $AC42 If IX[7] == 0 then return
N $AC47 If we arrive here then a hit has occurred.
  $AC47 Load speed
  $AC4B If IX[7] < 0 then #REGde = 280
  $AC51 #REGbc = #REGde
  $AC53 Point #REGhl at $AD03 table
N $AC56 This must be mapping the speed into the 5-entry array.
  $AC56 A = D     top part of speed
  $AC57 E ROL 1   double speed
  $AC59 A ROL 1
  $AC5A A &= 3    mask top part to 3  (for ref. turbo speed = $1FF)
  $AC5C RR D      halve D to carry?
  $AC5E D = 0
  $AC60 A = (A + D + carry) * 2  scale for element size of 2
  $AC62 E = A   it's an offset in $AD03
  $AC63 HL += DE
  $AC64 Copy (two bytes?) to IX+17 from table
  $AC6D double BC?  adjusted speed retained from earlier
  $AC71 If B >= 2 BC = 350  so this is if we hit the object very fast it gets put on the left?
  $AC79 Set bottom byte of horizontal position
  $AC7C Increment IX[7]
  $AC7F JR Z,$AC84   if B is zero then don't store it
  $AC81 Set top byte of horizontal position
  $AC84 Increment IX[1]
  $AC87 Call start_sfx with BC = $0503
  $AC8D A = 2        [set this to 1 and it drives off like a car]
  $AC8F Set IX[15] to 2
@ $AC92 label=hh_ac92
  $AC92 A--
  $AC93 If A == 0 return
  $AC94 IX[16] = $ACDB[IX[17]]
  $ACA1 Increment IX[17]
  $ACA4 Load horizontal position into #REGhl
  $ACAA #REGde = #REGhl
  $ACAC Divide horizontal position by 2?
  $ACB0 Then by 16 once it's 8 bits
  $ACB8 Then subtract from the previous position
  $ACBA Set horizontal position
  $ACC0 Toggle the bottom bit of IX[19]
  $ACC8 Decrement IX[18]
  $ACCB If IX[19] != 0 then return
  $ACCC Zero the horizontal position
  $ACD3 Set IX[19] to 1
  $ACD7 Set IX[15] to 1
  $ACDA Return
;
B $ACDB $AC94 uses this
W $AD03 $AC53 uses this

c $AD0D

c $AD51

@ $ADA0 label=main_loop_19
c $ADA0
  $ADA0 A = 0
  $ADA1 $A222 = 0
  $ADA4 IY = $E3xx
  $ADA7 IX = &hazards[0]
  $ADAB DE = 20
  $ADAE B = 6
@ $ADB0 label=ml19_loop
  $ADB0 If the hazard is active ...
  $ADB4 Bank
  $ADB5 Call $ADBE
  $ADB8 Unbank
  $ADB9 Move to next hazard
  $ADBB Loop while #REGb
  $ADBD,1 Return
;
  $AE1B Increment overtake_bonus
  $AE1F,1 Return
;
  $AF7E,3 Get smash_factor
  $AF91,3 HL -> $CDEC
  $AF9E,3 Get smash_factor
  $AFCF,3 Get smash_factor

b $B045 Used by $B968

@ $B063 label=main_loop_8
c $B063
  $B080 Clear up/down/left/right bits of user input
  $B098 Read boost time remaining
  $B0A2 Decrement turbo boost count [POKE $B0A5 for Infinite turbos]
  $B0A9 TBD
  $B0AF Read user input
  $B1EC Cap speed to $1FF

@ $B318 label=main_loop_24
c $B318
  $B318 If speed > 0 jump $B325
  $B31F TBD...

c $B4CC
  $B4D8 time_sixteenths/$A17D = 15, time_bcd/$A17E = $60
  $B4E1 Point #REGhl at left light's attributes
  $B4E4 Toggle its brightness
  $B4E7,3 Point at "SIGHTING OF TARGET VEHICLE" message
  $B4EA,3 Call message_printing_related
  $B4ED Exit via hooked routine (likely sfx, siren?)

@ $B4F0 label=smash
c $B4F0 Smash handling
  $B4F0 Cycle #REGa one step through 0..3 each time the routine is entered
  $B4F8 *$B55C = $CE33 + #REGa * 6
  $B506 $B54A = 9
  $B50B Load and increment smash_counter
  $B50F 20 hits? [POKE $B50F for Single hit capture]
  $B511 Exit via fully_smashed if so
  $B514 19 hits?
  $B516 Goto smash_b522 if not
N $B518 We have 19 hits
  $B518
  $B519 A = 10
  $B51B HL = raymond_says_one_more_time
  $B51E Call chatter
  $B521
@ $B522 label=smash_b522
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
  $B595
  $B596 Point #REGhl at ? then add (#REGa * 4)
  $B59F Point #REGde at the car tiles [needs a proper label]
  $B5A2 C=7
  $B5A4 Call sub_b627
  $B5A7
  $B5A8 117 - (self modified value $B5AB)
  $B5AC D=A
  $B5AD A=C+B+(self modified value $B5B0)
  $B5B1 If A >= 9 A -= 9
  $B5B7,3 Point #REGhl at car definitions[A] (entries are 20 bytes wide)
  $B5C4
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
  $B5DE
  $B5DF
  $B5E0 H=*HL
  $B5E1 L=A
  $B5E2 A=5
  $B5E4 E=A
  $B5E5 D=0
  $B5E7
  $B5E8
  $B5E9
  $B5EA IF $A251 jump to draw_car_perhaps_flipped
  $B5F0
  $B5F1 Call plot_sprite -- #REGa is (how many pixels to plot - 1) / 8
  $B5F4 Goto draw_car_cont
@ $B5F6 label=draw_car_perhaps_flipped
  $B5F6
  $B5F7 L--
  $B5F8 Call plot_sprite_flipped
@ $B5FB label=draw_car_cont
  $B5FB
  $B5FC
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
  $B628
  $B629 D=A - *HL++
  $B62C B=*HL++
  $B62E A=*HL++
  $B630
  $B631 H=*HL
  $B632 L=A
  $B633
  $B634
  $B635
  $B636 A=*$A251
  $B639 B=A
  $B63A E=C
  $B63B C--
  $B63C If A==0 C=A
  $B640
  $B641 Draws right side of car
  $B6C4,3 Read from SUB @ $B5AA
  $B647 Return

c $B648

c $B67C
  $B6DE,5 Divide by 8
;
  $B701 Select a plotter? Could be clipping stuff too.
  $B708 This multiplies by six - the length of each load-mask-store step in the plotter core.
  $B70C Add that to #REGix
  $B711 Set loop counter for 15 iterations?
  $B714

@ $B716 label=plot_masked_sprite
c $B716 Masked sprite plotter
D $B716 This plots a lot of the game's masked graphics.
D $B716 The stack points to pairs of bitmap and mask bytes and HL must point to the screen buffer. Uses AND-OR type masking. Proceeds left-right. Doesn't flip the bytes so there must be an alternative for that.
R $B716 I:B  Rows
R $B716 I:DE Stride (e.g. 10 for 40px wide)
R $B716 I:HL Address of data to plot
  $B716 Save #REGsp for restoration on exit
@ $B71C label=plot_masked_sprite_loop
  $B71F Restore original #REGsp
@ $B724 label=plot_masked_sprite_entry
  $B724,1 Point #REGhl at the graphic data
@ $B729 label=plot_masked_sprite_core_thingy
  $B729 Load a bitmap and mask pair (#REGd,#REGe)
  $B72A Load the screen pixels and AND with mask
  $B72C OR in new pixels and store back to screen
  $B72E Move to next screen pixel
  $B72F <Repeat 8 times>
  $B758 Handle end of row. This must be adjusting the screen pointer.

@ $B76C label=plot_masked_sprite_flipped
c $B76C Masked sprite plotter which flips
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
;
B $B79C TBD
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
  $B7EF self modify $B71F - exit of plot_masked_sprite
  $B7F3 IX = &plot_masked_sprite_core_thingy (jump table)
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
  $B825 JP plot_masked_sprite_entry

b $B828 breaks/crashes road rendering if messed with

@ $B848 label=main_loop_11
c $B848

c $B8D2

@ $B9F4 label=main_loop_12
c $B9F4

@ $BB69 label=main_loop_27
c $BB69
  $BB94 Self modified by $BE5C to be the left route's curvature data
  $BBC1 Self modified by $BE62 to be the right route's curvature data

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

@ $BDFB label=read_map
c $BDFB Map reader
  $BDFB ($A23D) = 0  -- state var
  $BDFF ($A23C) = 0  -- state var
  $BE02 ($A254) = 0  -- state var
  $BE05 HL = $A23F
  $BE08 DE = speed
  $BE0C A = Bottom byte of speed
  $BE0D If speed < 255 then jump
  $BE11 Preserve
  $BE13 Call subfunction $BE1F  -- processing something twice as much when at high speed?
  $BE16 Restore
;
@ $BE18 label=j_be18
  $BE18 $A23F += bottom byte of speed in A
  $BE1A A = 0 doesn't seem self modified
  $BE1C Goto j_c0d9 if no carry
;
@ $BE1F label=j_be1f
  $BE1F A = *$A240 + 1  -- Accessing A240 as a byte here
  $BE22 *$A240 = A
  $BE23 HL = $EE00 | (A + 95)  -- index $EE00, the cyclic road buffer thing
  $BE28 *$A23C |= *HL
  $BE2F L += 32
  $BE33 *$A23D |= *HL
  $BE3A L -= 96
;
N $BE3E -- CURVATURE --
  $BE3E A = curvature_byte - 16
  $BE43 Goto rm_save_curvature_byte if no carry
;
  $BE45 Read and increment address of curvature data
  $BE4A Read a curvature byte
  $BE4B Set flags
  $BE4C Goto rm_curvature_regular_byte if non-zero
;
@ $BE4E label=rm_curvature_escape_byte
N $BE4E Handle curvature escape byte.
  $BE4E #REGhl is now address of escape byte
  $BE4F Increment, read command byte, increment again
  $BE52 Goto rm_curvature_jump_command if command byte is zero
  $BE55 Goto rm_curvature_one_command if command byte is one
;
N $BE58 Otherwise it must be a road split command (byte == 2).
  $BE58 Load address of left route's curvature data
  $BE5C Self modify 'LD HL,$xxxx' at $BB94 to load the address
  $BE60 Load address of right route's curvature data
  $BE64 Self modify 'LD HL,$xxxx' at $BBC1 to load the address
  $BE67 HL = $E2C6 -> road split left curvature data
  $BE6A Goto rm_read_curvature
;
@ $BE6C label=rm_curvature_one_command
N $BE6C Handle a command byte of one: TBD.
  $BE6C HL = <map curvature address> -- Self modified by $BBCC
  $BE6F Goto rm_read_curvature
;
@ $BE71 label=rm_curvature_jump_command
N $BE71 Handle a command byte of zero: jump to the address given in next two bytes.
  $BE71 HL = wordat(HL)
;
@ $BE75 label=rm_read_curvature
  $BE75 #REGde is now map curvature address
  $BE76 Read a curvature byte (regular, not an escape)
;
@ $BE77 label=rm_curvature_regular_byte
  $BE77 road_curvature_ptr = #REGde
  $BE7B A -= 16
;
@ $BE7D label=rm_save_curvature_byte
  $BE7D curvature_byte = A  -- Save original byte
  $BE80 A &= 15  -- Mask off direction bits
  $BE82 Goto rm_set_curvature if (A & (1<<3)) is zero   -- bit 3 => left (so jump if right)
  $BE86 A = -(A & 7)  -- Left direction becomes -ve
;
@ $BE8A label=rm_set_curvature
N $BE8A Write the direction into the cyclic road buffer.
  $BE8A *HL = A * 2  -- Sampled HL: $EEA9 [in cyclic road buffer]
  $BE8C L += 32
;
N $BE90 -- HEIGHT --
  $BE90 A = height_byte - 16
  $BE95 Goto rm_save_height_byte if no carry
;
  $BE97 Read and increment address of height data
  $BE9C Read a height byte
  $BE9D Set flags
  $BE9E Goto rm_height_regular_byte if non-zero
;
N $BEA0 Handle height escape byte.
  $BEA0 #REGhl is now address of escape byte
  $BEA1 Increment, read command byte, increment again
  $BEA4 Goto rm_height_jump_command if command byte is zero
  $BEA8 Goto rm_height_one_command if command byte is one
;
N $BEAA Otherwise it must be a road split command (byte == 2).
  $BEAA Load address of left route's height data
  $BEAE Self modify 'LD DE,$xxxx' at $BB97 to load the address
  $BEB2 Load address of right route's height data
  $BEB6 Self modify 'LD DE,$xxxx' at $BBC4 to load the address
  $BEB9 HL = $E2CC -> road split height data
  $BEBC Goto rm_read_height
;
@ $BEBE label=rm_height_one_command
  $BEBE HL = <something>  -- Self modified by $BBCF
  $BEC1 Goto rm_read_height
;
@ $BEC3 label=rm_height_jump_command
  $BEC3 HL = wordat(HL)
;
@ $BEC7 label=rm_read_height
  $BEC7 #REGde is now height address
  $BEC8 Read a height byte (regular, not an escape)
;
@ $BEC9 label=rm_height_regular_byte
  $BEC9 road_height_ptr = #REGde    -- update pointer
  $BECD A -= 16
;
@ $BECF label=rm_save_height_byte
  $BECF height_byte = A  -- Save original byte
  $BED2 *HL = (A & 15) - 8  -- Sampled HL: $EEC9 [in cyclic road buffer]
  $BED7 L += 32
;
N $BEDB -- LANES --
N $BEDB Lanes bytes are counted (like RLE).
  $BEDB Decrement the lanes counter
  $BEE0 Goto rm_lanes_count_resume if non-zero
;
N $BEE2 Lanes counter hit zero, so reload.
  $BEE2 Read and increment address of lanes data
  $BEE7 Read a lanes byte
  $BEE8 Set flags
  $BEE9 Goto rm_lanes_regular_byte if non-zero
;
N $BEEB Handle lanes escape byte.
  $BEEB #REGhl is now address of escape byte
  $BEEC Increment, read command byte, increment again
  $BEEF Goto rm_lanes_jump_command if command byte is zero
  $BEF2 Goto rm_lanes_one_command if command byte is one
;
N $BEF5 Otherwise it must be a road split command (byte == 2).
  $BEF5 Load address of left route's lanes data
  $BEF9 Self modify 'LD BC,$xxxx' at $BB9A to load the address
  $BEFD Load address of right route's height data
  $BF01 Self modify 'LD BC,$xxxx' at $BBC7 to load the address
  $BF04 HL = $E2D1 -> road split lanes data
  $BF07 Goto rm_read_lanes
;
@ $BF09 label=rm_lanes_one_command
  $BF09 HL = $0000  -- This looks setup for it as others, but doesn't seem to be self modified.
  $BF0C Goto rm_read_lanes
;
@ $BF0E label=rm_lanes_jump_command
  $BF0E HL = wordat(HL)
;
@ $BF12 label=rm_read_lanes
  $BF12 #REGde is now lanes address
  $BF13 Read a lanes byte (regular, not an escape)
;
@ $BF14 label=rm_lanes_regular_byte
  $BF14 lanes_counter_byte = (lanes byte) - 1
  $BF18 road_lanes_ptr = ++DE
  $BF1D *HL = *DE & $F7  -- HL points into cyclic road buffer? Sampled HL = $EE60
  $BF21 A = *DE & $FB
  $BF24 Self modify 'LD A,$xx' at $BF2C
  $BF27 Goto rm_lanes_done
;
N $BF29 Resume updating lanes data.
@ $BF29 label=rm_lanes_count_resume
  $BF29 Store decremented lanes counter
  $BF2C A = <self modified value>  -- Self modified above AND below
  $BF2E C = A  -- Save temporarily
  $BF2F Goto rm_lanes_bf39 if (A & $0C) is zero
;
N $BF33 (bottom two bits significant)
  $BF33 A = C & $F3
  $BF36 Self modify 'LD A,$xx' at $BF2C
;
@ $BF39 label=rm_lanes_bf39
  $BF39 *HL = C  -- Sampled HL: $EEE9
;
@ $BF3A label=rm_lanes_done
  $BF3A L += 32
;
N $BF3E -- RIGHT-SIDE OBJECTS --
  $BF3E Decrement the rightside counter
  $BF42 Goto $BF55 if zero
;
N $BF44 Empty space inbetween objects.
  $BF44 *HL = 0
  $BF47 A = L + 32
  $BF4A L = A
  $BF4B *HL = 0
  $BF4C A += 32
  $BF4E L = A
  $BF4F *HL = 0
  $BF50 A = 1
  $BF52 Goto rm_all_hazards
;
N $BF55 ???
@ $BF55 label=j_BF55
  $BF55 A = rightside_byte - 16
  $BF5A Goto rm_rightside_count_resume if no carry
;
N $BF5C Rightside counter hit zero, so reload.
  $BF5C Read and increment address of rightside data
  $BF61 Read a rightside byte
  $BF62 Set flags
  $BF63 Goto rm_rightside_regular_byte if non-zero
;
N $BF65 Handle rightside escape byte.
  $BF65 #REGhl is now address of escape byte
  $BF66 Increment, read command byte, increment again
  $BF69 Goto rm_rightside_jump_command if command byte is zero
  $BF6C Goto rm_rightside_one_command if command byte is one
;
N $BF6F Otherwise it must be a road split command (byte == 2).
  $BF6F Load address of left route's rightside data
  $BF73 Set modify 'LD DE,$xxxx' at $BB8D to load the address
  $BF77 Load address of right route's rightside data
  $BF7B Self modify 'LD DE,$xxxx' at $BBBA to load the address
  $BF7E HL = $E2C1  [overlapping use of data TBD]
  $BF81 Goto rm_read_rightside
;
@ $BF83 label=rm_rightside_one_command
  $BF83 HL = $0000  -- This looks setup for it as others, but doesn't seem to be self modified.
  $BF86 Goto rm_read_rightside
;
@ $BF88 label=rm_rightside_jump_command
  $BF88 HL = wordat(HL)
;
@ $BF8C label=rm_read_rightside
  $BF8C #REGde is now lanes address
  $BF8D Read a rightside byte (regular, not an escape)
;
@ $BF8E label=rm_rightside_regular_byte
  $BF8E road_rightside_ptr = DE
  $BF92 A -= 16
;
@ $BF94 label=rm_rightside_count_resume
  $BF94 rightside_byte = A  -- Store decremented rightside counter
  $BF97 *HL = A & $0F  -- Write to cyclic road buffer
  $BF9A L += 32
;
N $BF9E -- LEFT-SIDE OBJECTS --
  $BF9E A = *$A244 - 16
  $BFA3 Goto rm_leftside_count_resume if no carry
  $BFA5 DE = road_leftside_ptr + 1
  $BFAA Read a lanes byte
  $BFAB Set flags
  $BFAC Goto rm_leftside_regular_byte if non-zero
;
N $BFAE Handle leftside escape byte.
  $BFAE #REGhl is now address of escape byte
  $BFAF Increment, read command byte, increment again
  $BFB2 Goto rm_leftside_jump_command if command byte is zero
  $BFB5 Goto rm_leftside_one_command if command byte is one
;
N $BFB8 Otherwise it must be a road split command (byte == 2).
  $BFB8 Load address of left route's leftside data
  $BFBC Self modify 'LD BC,$xxxx' at $BB90
  $BFC0 Load address of right route's leftside data
  $BFC4 Self modify 'LD BC,$xxxx' at $BBBD
  $BFC7 HL = $E2C0  [overlapping use of data TBD]
  $BFCA Goto rm_read_leftside
;
@ $BFCC label=rm_leftside_one_command
  $BFCC HL = $0000  -- This looks setup for it as others, but doesn't seem to be self modified.
  $BFCF Goto rm_read_leftside
;
@ $BFD1 label=rm_leftside_jump_command
  $BFD1 HL = wordat(HL)
;
@ $BFD5 label=rm_read_leftside
  $BFD5 #REGde is now lanes address
  $BFD6 Read a leftside byte (regular, not an escape)
;
@ $BFD7 label=rm_leftside_regular_byte
  $BFD7 road_leftside_ptr = DE
  $BFDB A -= 16
;
@ $BFDD label=rm_leftside_count_resume
  $BFDD leftside_byte = A  -- Store decremented leftside counter
  $BFE0 *HL = A & $0F  -- Write to cyclic road buffer
  $BFE3 L += 32
;
N $BFE7 -- HAZARDS --
  $BFE7 A = hazards_byte - 1  [it's a counter]
  $BFEC Goto rm_no_hazards if no carry
  $BFEE DE = road_hazard_ptr + 1
;
@ $BFF3 label=rm_restart_hazards_read
  $BFF3 Read a hazards byte
  $BFF4 Set flags
  $BFF5 Goto rm_hazards_regular_byte if non-zero
;
N $BFF7 Handle hazards escape byte.
  $BFF7 #REGhl is now address of escape byte
  $BFF8 Increment, read command byte, increment again
  $BFFB Set flags
  $BFFC Goto rm_hazards_jump_command if command byte is zero
  $BFFF Goto rm_hazards_one_command if command byte is one
  $C001 Goto rm_hazards_road_fork_command if command byte is two
  $C005 Goto rm_hazards_set_hazard_command if command byte is < 10  -- 3..9
  $C009 Goto rm_hazards_set_fork_command if command byte is < 13    -- 10..12
  $C00D Goto rm_hazards_spawning_command if command byte is < 15    -- 13..14
  $C011 *$A224 = A - 11  -- Otherwise  [helicopter related]
  $C016 Goto rm_do_restart
;
N $C018 Handle car spawning commands (13/14 = enable/disable car spawning).
@ $C018 label=rm_hazards_spawning_command
  $C018 stop_car_spawning = A - 10
  $C01D Goto rm_do_restart
;
N $C01F Handle floating arrow commands (10/11/12 = off/left/right).
@ $C01F label=rm_hazards_set_fork_command
  $C01F floating_arrow = A - 7
  $C024 Goto rm_do_restart if zero
  $C026 correct_fork = A
;
@ $C029 label=rm_do_restart
  $C029 EX DE,HL
  $C02A Goto rm_restart_hazards_read
;
; This needs more analysis since the visible results don't match the map data.
N $C02C Handle hazard commands (3 = off, 4/5/6 = tumbleweeds left/right/both, 7/8/9 => barriers)
N $C02C When on left it uses lane 1, on right it uses lane 4, both it uses lane 2 and 3.
@ $C02C label=rm_hazards_set_hazard_command
  $C02C Self modify "LD (HL),$xx" at $C058 to load A
  $C02F Goto rm_read_hazards
;
@ $C031 label=rm_hazards_road_fork_command
  $C031 Load address of left route's hazards data  -- Sampled HL: $5F26 (road split)
  $C035 Self modify 'LD HL,$xxxx' at $BB8A
  $C039 Load address of right route's hazards data
  $C03D Self modify 'LD HL,$xxxx' at $BBB7
  $C040 HL = $E2B8
  $C043 Goto rm_read_hazards
;
@ $C045 label=rm_hazards_one_command
  $C045 HL = 0
  $C048 Goto rm_read_hazards
;
@ $C04A label=rm_hazards_jump_command
  $C04A HL = wordat(HL)  [Sampled HL: $6292 $6107 $61BC $600F]
;
@ $C04E label=rm_read_hazards
  $C04E #REGde is now hazards address
  $C04F Read a hazards byte (regular, not an escape)
;
@ $C050 label=rm_hazards_regular_byte
  $C050 road_hazard_ptr = DE  -- Update hazard objects ptr
  $C054 A--
;
@ $C055 label=rm_no_hazards
  $C055 hazards_byte = A   -- Update hazards byte
  $C058 *HL = 0  [cyclic road buffer]
  $C05A A = 2
;
@ $C05C label=rm_all_hazards
  $C05C *$A185 = A
  $C05F Call $B8D2
  $C062 IX = &hazards
  $C066 DE = 20  -- array entry stride
  $C069 B = 6, C = 0
;
@ $C06C label=rm_c06c
  $C06C RLC (IX)  -- test the used flag
  $C070 JR C,$C080  -- jump if used
;
@ $C072 label=rm_c072_continue
  $C072 IX += DE
  $C074 Loop while #REGb
  $C076 A = C  -- A = 0?
  $C077 HL = $A22B
  $C07A HL = $A22B  -- duplicate instruction
  $C07D *HL = A
  $C07E Goto rm_c0bb
;
@ $C080 label=rm_c080
  $C080 A = IX[15] + 1
  $C084 Goto rm_c096 if zero
  $C086 IX[1]--
  $C089 Goto rm_c072_continue if non-zero
  $C08C IX[0] = 0  -- mark the hazard entry unused
  $C090 RLA  -- testing top bit of A
  $C091 Goto rm_c072_continue if no carry
  $C093 C++
  $C094 Goto rm_c072_continue
;
@ $C096 label=rm_c096
  $C096 A = IX[1] - 1
  $C09B IX[1] = A
  $C09E Goto rm_c0b2 if carry
  $C0A0 Goto rm_c072_continue if non-zero
  $C0A2 A = IX[17]
  $C0A5 Set flags
  $C0A6 Goto rm_c072_continue if non-zero
  $C0A8 IX[1] = 1
  $C0AC IX[4] = 255
  $C0B0 Goto rm_c072_continue
;
@ $C0B2 label=rm_c0b2
  $C0B2 IX[17]--
  $C0B9 Goto rm_c072_continue
;
@ $C0BB label=rm_c0bb
  $C0BB A = xx  -- Self modified by $A977 + $A9A0 only
  $C0BD Set flags
  $C0BE Goto rm_c0d7 if zero
  $C0C1 HL = $ED73  -- This must be a buffer pointer at this point
  $C0C4 DE = $ED77
  $C0C7 BC = 38
;
@ $C0CA label=rm_c0ca
  $C0CA HL -= 2
  $C0CC DE -= 2
  $C0CE *DE-- = *HL--; BC--
  $C0D0 *DE-- = *HL--; BC--
  $C0D2 Goto rm_c0ca if PE  PE => B became zero
  $C0D5 HL++
  $C0D6 *HL = B
;
@ $C0D7 label=rm_c0d7
  $C0D7 A = 1
;
@ $C0D9 label=rm_c0d9
  $C0D9 *$A254 += A
  $C0DE Exit via $AD0D

@ $C0E1 label=main_loop_15
c $C0E1

c $C15B Tunnel entrance/exit drawing code
  $C221 Jump table (self modified)
  $C236 Jump table (self modified)

c $C2E7

@ $C452 label=main_loop_13
c $C452 Landscape related
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
  $C483 HL = $D010  doubtful that this is a pointer
  $C486 A = $D0
  $C487 B = $55 -- Set this to $00 and the landscape goes blank - half the time
  $C489 If B carried out earlier then jump
  $C48B HL = $0030
  $C48E B = $00
@ $C48F label=j_c48f
  $C48F Self modify XOR at $C6D3 to be zero [OR $D0 -- check other code again]
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
  $C60A,2 Jump table (self modified)
  $C61B,2 Jump table (self modified)
  $C62C,2 Jump table (self modified)
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

c $CBA4

c $CBC5

c $CBCE

c $CBD6

@ $CD3A label=main_loop_10
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
D $CDEC $AF91 uses this.

b $CE33 Used by $B4FD
W $CE33,24,6
  $CE4B,19
  $CE5E,19
  $CE71,19
  $CE84,19
  $CE97,19

b $CEAA Used by $B578

@ $CEDA label=hero_car_refs
b $CEDA Hero car drawing instructions
D $CEDA 9 of them. 20 bytes per entry.
B $CEDA TBD
B $CEDB,1 rows
W $CEDC,2 -> middle graphic
B $CEDE,1 TBD
B $CEDF,1 rows
W $CEE0,2 -> top graphic
B $CEE2,1 TBD
B $CEE3,1 rows
W $CEE4,2 -> bottom graphic
B $CEE6,1 TBD
B $CEE7,1 rows
W $CEE8,2 -> left graphic
B $CEEA,1 TBD
B $CEEB,1 rows
W $CEEC,2 -> right graphic
L $CEDA,20,9
;
@ $CF8E label=hero_car_shadow
N $CF8E Hero car adornments (shadow, smoke?)
B $CF8E,1 TBD
B $CF8F,1 rows
W $CF90,2 -> data
L $CF8E,4,3
;
@ $CF9E label=hero_car_smoke
W $CF9E,2 -> Turbo smoke plume data frame 1
W $CFA4,2 -> Turbo smoke plume data frame 2
W $CFAA,2 -> Turbo smoke plume data frame 3
W $CFB0,2 -> Turbo smoke plume data frame 4
;
@ $CFB2 label=unknown_cfb2
B $CFB2,,3 TBD groups of 3 bytes ref'd by $B6A6
;
@ $D027 label=car_adornments
B $D027,1 7 rows high
B $D028,1 1 byte wide
W $D029,2 -> Cherry light
;
B $D02B,1 14 rows high
B $D02C,1 3 bytes wide
W $D02D,2 -> Flashing cherry light
;
B $D02F,1 20 rows high
B $D030,1 3 bytes wide
W $D031,2 -> Crash/spark data
;
B $D033,1 4 rows high
B $D034,1 2 bytes wide
W $D035,2 -> Graphic data (can't tell)
;
B $D037,1 9 rows high
B $D038,1 2 bytes wide
W $D039,2 -> Debris
;
B $D03B,1 12 rows high
B $D03C,1 1 bytes wide
W $D03D,2 -> More debris (perhaps)
;
; Sometimes plotted as 24x28 when it's "HERE! + Arrow"
N $D03F Arrow graphic (24x21)
N $D03F #HTML[#CALL:graphic($D03F,24,21,1,1)]
B $D03F,126,6 Masked bitmap data
;
N $D0BD "HERE!" graphic (24x7)
N $D0BD #HTML[#CALL:graphic($D0BD,24,7,1,1)]
B $D0BD,42,6 Masked bitmap data
;
N $D0E7 Cherry light (sits on roof of car) (8x7)
N $D0E7 #HTML[#CALL:graphic($D0E7,8,7,1,1)]
B $D0E7,14,2 Masked bitmap data
;
N $D0F5 Illuminated cherry light (24x14)
N $D0F5 #HTML[#CALL:graphic($D0F5,24,14,1,1)]
B $D0F5,84,6 Masked bitmap data
;
N $D149 Crash/spark (24x20)
N $D149 #HTML[#CALL:graphic($D149,24,20,1,1)]
B $D149,120,6 Masked bitmap data
;
N $D1C1 Graphic (16x4)
N $D1C1 #HTML[#CALL:graphic($D1C1,16,4,1,1)] LOOKS WRONG
B $D1C1,16,4 Masked bitmap data
;
N $D1D1 Debris (16x9)
N $D1D1 #HTML[#CALL:graphic($D1D1,16,9,1,1)] LOOKS WRONG
B $D1D1,36,4 Masked bitmap data
;
N $D1F5 More debris (8x12)
N $D1F5 #HTML[#CALL:graphic($D1F5,8,12,1,1)] LOOKS WRONG
B $D1F5,24,2 Masked bitmap data
;
;
N $D20D Turbo smoke plume animation
N $D20D (32x16) masked per frame. 4 frames.
N $D20D #HTML[#CALL:anim($D20D,32,16,1,1,4)]
;
N $D20D Turbo smoke plume data frame 1
N $D20D #HTML[#CALL:graphic($D20D,32,16,1,1)]
B $D20D Masked bitmap data
;
N $D28D Turbo smoke plume data frame 2
N $D28D #HTML[#CALL:graphic($D28D,32,16,1,1)]
B $D28D Masked bitmap data
;
N $D30D Turbo smoke plume data frame 3
N $D30D #HTML[#CALL:graphic($D30D,32,16,1,1)]
B $D30D Masked bitmap data
;
N $D38D Turbo smoke plume data frame 4
N $D38D #HTML[#CALL:graphic($D38D,32,16,1,1)]
B $D38D Masked bitmap data
;
;
N $D40D Hero car graphics
N $D40D 9 sets
N $D40D Centre parts
;
N $D40D Straight (40x14)
N $D40D #HTML[#CALL:graphic($D40D,40,14,0,1)]
B $D40D,70,5 Non-masked, inverted bitmap data
;
N $D453 Straight + Turn right (40x17)
N $D453 #HTML[#CALL:graphic($D453,40,17,0,1)]
B $D453,85,5 Non-masked, inverted bitmap data
;
N $D4A8 Straight + Turn right hard (40x16)
N $D4A8 #HTML[#CALL:graphic($D4A8,40,16,0,1)]
B $D4A8,80,5 Non-masked, inverted bitmap data
;
N $D4F8 Up (40x14)
N $D4F8 #HTML[#CALL:graphic($D4F8,40,14,0,1)]
B $D4F8,70,5 Non-masked, inverted bitmap data
;
N $D53E Up + Turn right (40x17)
N $D53E #HTML[#CALL:graphic($D53E,40,17,0,1)]
B $D53E,85,5 Non-masked, inverted bitmap data
;
N $D593 Up + Turn right hard (40x15)
N $D593 #HTML[#CALL:graphic($D593,40,15,0,1)]
B $D593,75,5 Non-masked, inverted bitmap data
;
N $D5DE Down (40x14)
N $D5DE #HTML[#CALL:graphic($D5DE,40,14,0,1)]
B $D5DE,70,5 Non-masked, inverted bitmap data
;
N $D624 Down + Turn right (40x16)
N $D624 #HTML[#CALL:graphic($D624,40,16,0,1)]
B $D624,80,5 Non-masked, inverted bitmap data
;
N $D674 Down + Turn right hard (40x16)
N $D674 #HTML[#CALL:graphic($D674,40,16,0,1)]
B $D674,80,5 Non-masked, inverted bitmap data
;
;
N $D6C4 Straight parts
;
N $D6C4 Top of car (40x9)
N $D6C4 #HTML[#CALL:graphic($D6C4,40,9,1,1)]
B $D6C4,90,10 Masked, inverted bitmap data
;
N $D71E Bottom of car (40x6)
N $D71E #HTML[#CALL:graphic($D71E,40,6,1,1)]
B $D71E,60,10 Masked, inverted bitmap data
;
N $D75A Left of car (8x14)
N $D75A #HTML[#CALL:graphic($D75A,8,14,1,1)]
B $D75A,28,2 Masked, inverted bitmap data
;
N $D776 Right of car (8x14)
N $D776 #HTML[#CALL:graphic($D776,8,14,1,1)]
B $D776,28,2 Masked, inverted bitmap data
;
;
N $D792 Straight + Turn right parts
;
N $D792 Top of car (40x9)
N $D792 #HTML[#CALL:graphic($D792,40,8,1,1)]
B $D792,80,10 Masked, inverted bitmap data
;
N $D7E2 Bottom of car (40x6)
N $D7E2 #HTML[#CALL:graphic($D7E2,40,4,1,1)]
B $D7E2,40,10 Masked, inverted bitmap data
;
N $D80A Left of car (8x13)
N $D80A #HTML[#CALL:graphic($D80A,8,13,1,1)]
B $D80A,26,2 Masked, inverted bitmap data
;
N $D824 Right of car (8x13)
N $D824 #HTML[#CALL:graphic($D824,8,13,1,1)]
B $D824,26,2 Masked, inverted bitmap data
;
;
N $D83E Straight + Turn right hard parts
;
N $D83E Top of car (40x9)
N $D83E #HTML[#CALL:graphic($D83E,40,9,1,1)]
B $D83E,90,10 Masked, inverted bitmap data
;
N $D898 Bottom of car (40x4)
N $D898 #HTML[#CALL:graphic($D898,40,4,1,1)]
B $D898,40,10 Masked, inverted bitmap data
;
N $D8C0 Left of car (8x12)
N $D8C0 #HTML[#CALL:graphic($D8C0,8,12,1,1)]
B $D8C0,24,2 Masked, inverted bitmap data
;
N $D8D8 Right of car (8x15)
N $D8D8 #HTML[#CALL:graphic($D8D8,8,15,1,1)]
B $D8D8,30,2 Masked, inverted bitmap data
;
;
N $D8F6 Up parts
;
N $D8F6 Top of car (40x10)
N $D8F6 #HTML[#CALL:graphic($D8F6,40,10,1,1)]
B $D8F6,100,10 Masked, inverted bitmap data
;
N $D95A Bottom of car (40x6)
N $D95A #HTML[#CALL:graphic($D95A,40,6,1,1)]
B $D95A,60,10 Masked, inverted bitmap data
;
N $D996 Left of car (8x13)
N $D996 #HTML[#CALL:graphic($D996,8,13,1,1)]
B $D996,26,2 Masked, inverted bitmap data
;
N $D9B0 Right of car (8x14)
N $D9B0 #HTML[#CALL:graphic($D9B0,8,14,1,1)]
B $D9B0,28,2 Masked, inverted bitmap data
;
;
N $D9CC Up + Turn right parts
;
N $D9CC Top of car (40x9)
N $D9CC #HTML[#CALL:graphic($D9CC,40,9,1,1)]
B $D9CC,90,10 Masked, inverted bitmap data
;
N $DA26 Bottom of car (40x4)
N $DA26 #HTML[#CALL:graphic($DA26,40,4,1,1)]
B $DA26,40,10 Masked, inverted bitmap data
;
N $DA4E Left of car (8x12)
N $DA4E #HTML[#CALL:graphic($DA4E,8,12,1,1)]
B $DA4E,24,2 Masked, inverted bitmap data
;
N $DA66 Right of car (8x14)
N $DA66 #HTML[#CALL:graphic($DA66,8,14,1,1)]
B $DA66,28,2 Masked, inverted bitmap data
;
;
N $DA82 Up + Turn right hard parts
;
N $DA82 Top of car (40x9)
N $DA82 #HTML[#CALL:graphic($DA82,40,9,1,1)]
B $DA82,90,10 Masked, inverted bitmap data
;
N $DADC Bottom of car (40x6)
N $DADC #HTML[#CALL:graphic($DADC,40,6,1,1)]
B $DADC,60,10 Masked, inverted bitmap data
;
N $DB18 Left of car (8x11)
N $DB18 #HTML[#CALL:graphic($DB18,8,11,1,1)]
B $DB18,22,2 Masked, inverted bitmap data
;
N $DB2E Right of car (8x15)
N $DB2E #HTML[#CALL:graphic($DB2E,8,15,1,1)]
B $DB2E,30,2 Masked, inverted bitmap data
;
;
N $DB4C Down parts
;
N $DB4C Top of car (40x8)
N $DB4C #HTML[#CALL:graphic($DB4C,40,8,1,1)]
B $DB4C,80,10 Masked, inverted bitmap data
;
N $DB9C Bottom of car (40x6)
N $DB9C #HTML[#CALL:graphic($DB9C,40,6,1,1)]
B $DB9C,60,10 Masked, inverted bitmap data
;
N $DBD8 Left of car (8x13)
N $DBD8 #HTML[#CALL:graphic($DBD8,8,13,1,1)]
B $DBD8,26,2 Masked, inverted bitmap data
;
N $DBF2 Right of car (8x14)
N $DBF2 #HTML[#CALL:graphic($DBF2,8,14,1,1)]
B $DBF2,28,2 Masked, inverted bitmap data
;
;
N $DC0E Down + Turn right parts
;
N $DC0E Top of car (40x8)
N $DC0E #HTML[#CALL:graphic($DC0E,40,8,1,1)]
B $DC0E,80,10 Masked, inverted bitmap data
;
N $DC5E Bottom of car (40x4)
N $DC5E #HTML[#CALL:graphic($DC5E,40,4,1,1)]
B $DC5E,40,10 Masked, inverted bitmap data
;
N $DC86 Left of car (8x13)
N $DC86 #HTML[#CALL:graphic($DC86,8,13,1,1)]
B $DC86,26,2 Masked, inverted bitmap data
;
N $DCA0 Right of car (8x13)
N $DCA0 #HTML[#CALL:graphic($DCA0,8,13,1,1)]
B $DCA0,26,2 Masked, inverted bitmap data
;
;
N $DCBA Down + Turn right hard parts
;
N $DCBA Top of car (40x8)
N $DCBA #HTML[#CALL:graphic($DCBA,40,8,1,1)]
B $DCBA,80,10 Masked, inverted bitmap data
;
N $DD0A Bottom of car (40x4)
N $DD0A #HTML[#CALL:graphic($DD0A,40,4,1,1)]
B $DD0A,40,10 Masked, inverted bitmap data
;
N $DD32 Left of car (8x13)
N $DD32 #HTML[#CALL:graphic($DD32,8,13,1,1)]
B $DD32,26,2 Masked, inverted bitmap data
;
N $DD4C Right of car (8x15)
N $DD4C #HTML[#CALL:graphic($DD4C,8,15,1,1)]
B $DD4C,30,2 Masked, inverted bitmap data
;
;
N $DD6A Shadow parts
;
N $DD6A Shadow + Straight (56x12)
N $DD6A #HTML[#CALL:graphic($DD6A,56,12,1,1)]
B $DD6A,168,14 Masked, inverted bitmap data
;
N $DE12 Shadow + Turn right (56x12)
N $DE12 #HTML[#CALL:graphic($DE12,56,12,1,1)]
B $DE12,168,14 Masked, inverted bitmap data
;
N $DEBA Shadow + Turn right hard (56x12)
N $DEBA #HTML[#CALL:graphic($DEBA,56,12,1,1)]
B $DEBA,168,14 Masked, inverted bitmap data

@ $DF62 label=ledfont
b $DF62 LED style numeric font used for scores
D $DF62 8x15 pixels, digits 0..9 only
D $DF62 #HTML[#CALL:graphic($DF62,8,10*15,0,0)]
B $DF62,150,15

@ $DFF8 label=minifont
b $DFF8 Mini font used for in-game messages
D $DFF8 8x6 pixels, though the digits are thinner than 8, A-Z + five symbols.
D $DFF8 #HTML[#CALL:graphic($DFF8,8,31*6,0,0)]
B $DFF8,186,6

b $E0B2 Graphics defns
B $E0B2,,7 <Byte width, Flags, Height, Ptr, Ptr>
B $E1DF,,5 <Byte width, Flags, Height, Ptr>  [floating arrow defns]
;
; The arrow is used for road turn indicator AND used for "HERE!".
; This is just the arrow.
;B $E1E5,1 3 bytes wide
;B $E1E6,1 21 rows high
;W $E1E7,2 -> Arrow
;

b $E1E9 Ref'd by graphic entry 1 and 10

; it's helpful to look at this as it's the simplest section of road in the game.
; change $E2B4 to $55 (from $35) and the barriers are much earlier on the road. (tunnel is later!)
; change it to $00 and I get a double road, no perp car and other wrongness
; 1 => the tunnel's there straight away
; 2 => same
; $0F => tunnel's a short distance away
; so it's a distance or length which affects how distant the tunnel is, or end of road
; $FF => tunnel is a huge distance away

; $E2B5 is $81
; 0 => road is four lanes wide               ||||
; 1 => road is two lanes wide of three  e.g. ||__
; 2 => road is two lanes wide of three  e.g. _||_
; 3 => road is two lanes wide of four   e.g. __||
; 4, 8, 16, 32, 64, $80, $84 => as for 0
; 5 => as for 1
; 6 => as for 2
; 7 => as for 3
; $81 => three lanes wide as expected        |||_
; $82 => three lanes of four                 _|||
; $83 => two lane? but then it extends off to the right
; $FF => two roads, no car etc.
; $C1 => three lanes wide, but no markings
; $C2 => as for 2, but no markings
; $E1 => mad effect again

; $E2B6 is $FF
; 0 => mad double lane effect
; 1,2 => tunnel has no walls and four lanes inside it
; $F0 => seems ok
; $0F => ok but scanline glitch on horizon
; ... length of tunnel?

; $E2B7 is $45
; 0 => no tunnel appears

; $E2B8 is $0C
; can't see any diff when i meddle this value
;
; ...
;
; $E2AA (earlier) is $F0 only
; $F1 => road curves to right, tunnel lights are drawn everywhere
; $F2 => crash
; $F3 => road curves to right and bollards are drawn a-la road split
; $F4 => road curves to right, trees everywhere (then a crash)
; $F5 => road curves, bushes everywhere (no crash)
; $F6 => road curves, streetlamps everywhere (then a crash)
; $F7 => road curves, pylons everywhere (and rendering reflections?)
; $F8 => road STRAIGHT, turn left signs eveywhere
; $F9 => road curves LEFT, turn RIGHT signs everywhere
; $FA,$FB,$FC,$FE,$FF => crash
; $FD => road curves LEFT, three lanes, trees everywhere, in road
;
; ...seems to affect more than it should
;
; $5EC4 (first byte of map)
;   $00 is an escape byte
;   $02 curves right
;   $0F as for $EF
;   $E0 looks the same
;   $E2 curves right
;   $E8 straight
;   $E9 slight left, $EA, $EB more intense curves
;   $EF extreme curve LEFT
;   $F0 is straight
;   $F1..$F3 curve right ($F4 or greater will mess up)
;   $F8 seems straight
;   $F9..$FB curve left ($FC or greater will mess up_
; this implies that there's a curvature value in the state and that the commands are relative to it, but then other tests don't show that...2
;
; most regular reads are by BE4A

; could $FF be terminators?
; end two words seem to always be preceded by an $Fx byte yet FF and Fx bytes are found elsewhere in the data
; this doesn't hold for the initial map segment data.

; $5EE4 sets road height for this segment
; $F8 -> $F0, road ascends very high (impractically so)
;        $F7, raised slightly
;        $F9, lowered slightly
;        $FF, road descends
;        $FD, descending
;        $F3, ascending - steep

; $5F0F
; $1E -> $00, crazyness

; $625D this map data chunk (chunk 3) is the lanes/road width
; seems to need 0,0,0 not 0,0 for escapes OR it's using words, not bytes as its quantum
; $FE $9E => three lanes
; $xx $yy xx alters the period, so seems like runlength <count><type>
; game data seems to avoid runlengths > 254
; $xx $01 => two lanes
; $xx $00 => four lanes
; $xx $02 => two lanes
; $xx $00 $xx $01 => four/two alternating every stripe
; $xx $00 $xx $00 => four lanes
; $xx $01 $xx $00 => four/two alternating but objects end up in the road (might just be a phase thing)
; $01 $02 $01 $01 => two lanes, but shifting each segment
; so the byte could be a left side subtracting value
; $01 $03 $01 $01 => two lanes, shifting each segment, no overlap
; $01 $03 $01 $03 => two lanes on the right hand sidea
; $01 $06 => diagonal appears
; $01 $00 $10 $06 => four lanes down to 2 (left side slopes in)
;  ||
; /||
; |||T
; $01 $00 $10 $07 => four lanes down to 2 (left side slopes in)
;   ||
;  /||
; T|||
; $10 $07 $01 $00
; $xx 21 => two lanes
; $xx 4x => tunnel (seems to persist and/or flicker off)
; $xx 81 => three lanes
; $xx C1 => four lanes + stones are enabled (from then onwards?)
; $xx C3 => two lanes + stones
; $xx 04, 08, 10, 20, C0, 80, 0C, E0, F0 => nothing obvious
; $xx ff => madness, then crashing

; lane objects's affects objects on the road
; $5F1D $93 -> $01 made the "turn right" arrow appear
; $5F1E  00 ->  01 made the arrow appear too late and cars appeared on dirt track
; so those two could be a 16-bit counter?
; $5F1F  0E ->  01 puts two barriers where the turn right arrow normally appears
; could be an object ID?
; 0D, 0C => right facing arrow
; 08 => arrow + 5 barriers on the right periodically



; Road-side Objects (trees, signs, ...)
; -----------------
; Stage 1.
; Top nibble is not yet understood.
; You get two objects drawn per byte - two stripes of road are one byte.
; Any other values seem unpredictable (objects in road, or crashes).
;
; $x0 => no object (where x > 0)
; $x1 => tunnel light
; $x3 => barrier poles
; $x4 => tree
; $x5 => bush
; $x6 => lamp post (will flip to match side)
; $x7 => telegraph pole
; $x8 => left bend sign
; $x9 => right bend sign

b $E2AA Data for perp escape scene
N $E2AA Perp escape scene, curvature
B $E2AA,1 Straight
B $E2AB,2 Escape, Command 0 (Continue at <Address>)
W $E2AD,2 Loop
;
N $E2AF Perp escape scene, height
B $E2AF,1 Level ground
B $E2B0,2 Escape, Command 0 (Continue at <Address>)
W $E2B2,2 Loop
;
N $E2B4 Perp escape scene, lanes
B $E2B4,1 Length byte ?
B $E2B5,1 Lanes byte ?
B $E2B6,1 TBD
B $E2B7,1 TBD
B $E2B8,1 TBD
B $E2B9,1 TBD
B $E2BA,1 TBD
B $E2BB,1 TBD
B $E2BC,2 Escape, Command 0 (Continue at <Address>)
W $E2BE,2 Loop (partial)
;
N $E2C0 Perp escape scene, lane objects [not sure now, could be right curve road]
B $E2C0,1 TBD
B $E2C1,1 TBD
B $E2C2,2 Escape, Command 0 (Continue at <Address>)
W $E2C4,2 Loop (partial)
;
N $E2C6 Road split, curvature
B $E2C6,1 TBD
B $E2C7,1 Curve left (67%)
B $E2C8,2 Escape, Command 0 (Continue at <Address>)
W $E2CA,2 Loop (partial)
;
N $E2CC Road split, height
B $E2CC,1 TBD
B $E2CD,2 Escape, Command 0 (Continue at <Address>)
W $E2CF,2 Loop
;
B $E2D1 road split lane data TBD
B $E2D2 Ref'd by $BBEF  --  Used after road split
B $E2D5 Ref'd by $BB7E, $BBA5
B $E2D9 Ref'd by $BB78, $BBAB
B $E2DD Ref'd by $BBE3
B $E2E1 Curvature escape byte
B $E2E2 Ref'd by $BBE9
B $E2E7 Ref'd by $BB84
B $E2F3 Ref'd by $BBB1

b $E34B
B $E34B,3 3 bytes set to 8 by $880A

b $E364 Transition masks
N $E364 Spiral inward animation mask (8x8, 11 frames)
N $E364 #HTML[#CALL:anim($E364,8,8,0,0,11)]
N $E364 #HTML[#CALL:graphic($E364,8,11*8,0,0)]
B $E364,88,8
;
N $E3BC Circle expanding animation mask (8x8, 7 frames)
N $E3BC #HTML[#CALL:anim($E3BC,8,8,0,0,7)]
N $E3BC #HTML[#CALL:graphic($E3BC,8,7*8,0,0)]
B $E3BC,56,8

c $E540 Looks like a table of flipped bytes (but not quite)

b $E601

c $E839 looks like initialisation code / relocation
  $E839 must be the loop counter
  $E83A Point #REGhl at $e858 table
@ $E83D label=loopy
  $E83D PUSH BC       ;
  $E83E DE = wordat(HL) HL += 2
  $E842 Stack DE
  $E843 DE = wordat(HL) HL += 2 -- dest
  $E847 BC = wordat(HL) HL += 2 -- count
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
;
N $E8A6 Square zoom in animation mask (8x8, 5 frames)
N $E8A6 #HTML[#CALL:anim($E8A6,8,8,0,0,5)]
N $E8A6 #HTML[#CALL:graphic($E8A6,8,5*8,0,0)]
B $E8A6,40,8
;
N $E8CE Diamond zoom in animation mask (8x8, 6 frames)
N $E8CE #HTML[#CALL:anim($E8CE,8,8,0,0,6)]
N $E8CE #HTML[#CALL:graphic($E8CE,8,6*8,0,0)]
B $E8CE,48,8
;
B $E8FE TBD
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
@ $EC19 label=menu_draw_string_loop
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
  $F2B2,3 Exit via (if P) print_message
  $F3A5,3 Call silence_audio_hook
  $F460,3 Call print_message
  $F485,3 Call message_printing_related
  $F488,3 Call transition
  $F48B,3 Call draw_screen

b $F491 Credits / Score messages
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
