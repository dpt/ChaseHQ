> $4000 ; ChaseHQ.skool
> $4000 ;
> $4000 ; This is a SkoolKit control file that disassembles the ZX Spectrum version
> $4000 ; of Chase H.Q. by Ocean Software. A home computer version of the arcade game
> $4000 ; by Taito Corporation.
> $4000 ;
> $4000 ; This documents the 48K version of game when it is in a "pristine" as-loaded
> $4000 ; state. It also covers a lot of the 128K code too, but doesn't yet describe any
> $4000 ; of the extra memory banks.
> $4000 ;
> $4000 ; Reverse engineering by David Thomas, 2023-2024.
> $4000 ;
> $4000 ;
> $4000 ; AUTHORS
> $4000 ; -------
> $4000 ; Code by John O'Brien aka JOBBEEE
> $4000 ; <https://www.mobygames.com/person/28679/john-obrien/>
> $4000 ;
> $4000 ; Graphics by Bill Harbison aka BILL aka Harbonaut
> $4000 ; <https://www.mobygames.com/person/31804/bill-harbison/>
> $4000 ;
> $4000 ; Music by Jonathan Dunn aka JON DUNN
> $4000 ; <https://www.mobygames.com/person/31702/jonathan-dunn/>
> $4000 ;
> $4000 ;
> $4000 ; ALTERNATIVE VERSIONS
> $4000 ; --------------------
> $4000 ; There's a demo version of Chase H.Q. on Sinclair User issue 94: Megatape 23.
> $4000 ; https://worldofspectrum.org/archive/software/cover-tapes-and-electronic-magazines/sinclair-user-issue-94-megatape-23-sinclair-user.
> $4000 ; It features a single stage, with the graphics from stage 1 of the real game
> $4000 ; and what seems to be a custom map. There's no retry on fail and it's a tough
> $4000 ; level.
> $4000 ;
> $4000 ;
> $4000 ; RELATED PORTS
> $4000 ; -------------
> $4000 ; The Amstrad CPC version is by the same authors and was produced after this
> $4000 ; Spectrum version.
> $4000 ;
> $4000 ;
> $4000 ; RELATED GAMES
> $4000 ; -------------
> $4000 ; "WEC Le Mans" (Imagine, 1989)
> $4000 ; [Lamb/Mullins/Dunn/Harbison/Morrall]
> $4000 ; - Shares a composer and an artist with ZX Chase HQ.
> $4000 ; <https://www.mobygames.com/game/15167/wec-le-mans-24/>
> $4000 ;
> $4000 ; "Batman: The Movie" (Ocean Software, 1989)
> $4000 ; [Lamb/O'Brien/Shortt/Drake/Harbison/Palmer/Hemphill/Dunn/Cannon]
> $4000 ; - ZX Chase HQ's authors worked on the driving sequences for the Atari ST and
> $4000 ;   Amiga versions of this game.
> $4000 ; <https://www.mobygames.com/game/3848/batman/>
> $4000 ;
> $4000 ; "Burnin' Rubber" (Ocean Software, 1990)
> $4000 ; [O'Brien/Hemphill/Dunn]
> $4000 ; - A later Amstrad game by John O'Brien very much like WEC Le Mans.
> $4000 ; <https://www.mobygames.com/game/70894/burnin-rubber/>
> $4000 ;
> $4000 ;
> $4000 ; RESEARCH
> $4000 ; --------
> $4000 ; https://en.wikipedia.org/wiki/Chase_H.Q.
> $4000 ; https://spectrumcomputing.co.uk/entry/903/ZX-Spectrum/Chase_HQ
> $4000 ; https://www.mobygames.com/game/9832/chase-hq/
> $4000 ; http://reassembler.blogspot.com/2012/06/interview-with-spectrum-legend-bill.html
> $4000 ; https://news.ycombinator.com/item?id=8850193 (discussion about the game loader)
> $4000 ; http://www.extentofthejam.com/pseudo/ (Lou's Pseudo 3D page)
> $4000 ;
> $4000 ;
> $4000 ; BUGS
> $4000 ; ----
> $4000 ; Bugfix provided by Russell Marks:
> $4000 ; "When you finish a stage, a hidden bonus is sometimes given randomly. At
> $4000 ;  address $8b3d, instructions EX AF,AF' and LD B,A are in the wrong order.
> $4000 ;  Therefore calculation of time bonus is using random value from A' instead of
> $4000 ;  low digit from remaining time. Fixed with POKE 35645,71: POKE 35646,8."
> $4000 ;
> $4000 ;
> $4000 ; SECRETS
> $4000 ; -------
> $4000 ; Redefine keys to "SHOCKED<ENTER>" to activate test/cheat mode.
> $4000 ;
> $4000 ;
> $4000 ; 48K/128K VERSION DIFFERENCES
> $4000 ; ----------------------------
> $4000 ; - 48K version is a multiloader
> $4000 ; - 128K version has AY music with sampled speech and effects
> $4000 ; - 128K has logo animation > music plays > attract mode (48K version only has attract mode)
> $4000 ; - 128K has a Best Officers (high score) on attract screen and a high score entry screen
> $4000 ; - 128K retains the input device/define keys code (48K has to overwrite it for space reasons)
> $4000 ;
> $4000 ;
> $4000 ; MEMORY MAP
> $4000 ; ----------
> $4000 ; The memory map changes once the game's been shifted into its regular
> $4000 ; running state:
> $4000 ;
> $4000 ; $5B00..$5BFF is a pre-shifted version of the backdrop
> $4000 ; $5C00..$76EF is the per-stage data:
> $4000 ; - $5C00..$5CFF is the regular version of the backdrop
> $4000 ; $8DBB (word) is the address of the current transition animation
> $4000 ; $E300..$E316 is a height table (22 bytes long)
> $4000 ; $E336..      is TBD
> $4000 ; $E410..$E4CF is a road edge/markings table
> $4000 ; $E4D0..$E4FF is road lane markings
> $4000 ; $E500        is road bending tables
> $4000 ; $E600..$E80F is road drawing scaling tables (3x8 groups of 22 bytes)
> $4000 ; $E830..$E8FF is 104 words for road drawing (left)
> $4000 ; $E900        is a table
> $4000 ; $E930..$E9FF is 104 words for road drawing (centre left)
> $4000 ; $EA00..$EA2F is the diamond zoom-in mask
> $4000 ; $EA30..$EAFF is 104 words for road drawing (centre)
> $4000 ; $EB00..$EB27 is the square zoom-in mask
> $4000 ; $EB30..$EBFF is 104 words for road drawing (centre right)
> $4000 ; $EC00..$EC2F is TBD (transition uses this)
> $4000 ; $EC30..$ECFF is 104 words for road drawing (right)
> $4000 ; $ED28        is the stack (growing downwards)
> $4000 ; $ED30..?     is (possibly another 104 word road drawing buffer)
> $4000 ; $EE00..$EEFF is the road buffer. holds data unpacked from maps. it's cyclic. 32 byte fixed sections for each datum (curvature, height, lanes, right side objects, left side objects, hazards). cleared by $87DD.
> $4000 ; $EF00..$EFFF is a table of flipped bytes
> $4000 ; $F000..$FFFF is a 4KB back buffer
> $4000 ;
> $4000 ; A back buffer address of 0b1111BAAACCCXXXXX means:
> $4000 ; - X = X position byte (0..31)
> $4000 ; - A = Y position row bits 0..2
> $4000 ; - B = Y position row bit  3
> $4000 ; - C = Y position row bits 4..6
> $4000 ;
> $4000 ; There's a column spare at either side of the back buffer (same as the main
> $4000 ; screen). This seems to be to avoid full clipping.
> $4000 ;
> $4000 ;
> $4000 ; 128K MEMORY MAP
> $4000 ; ---------------
> $4000 ; The per-stage data has a worst-case size of $1AF0 bytes.
> $4000 ;
> $4000 ; Bank 1 @ $C000..$DAEF is stage 1's data
> $4000 ; Bank 1 @ $E000..$FAEF is stage 2's data
> $4000 ; Bank 3 @ $C000..$FFFF? is the animated title screen
> $4000 ; Bank 4 @ $C000..$FF66 is sampled speech
> $4000 ; Bank 6 @ $C000..$DAEF is stage 3's data
> $4000 ; Bank 6 @ $E000..$FAEF is stage 4's data
> $4000 ; Bank 7 @ $C000..$DAEF is stage 5's data
> $4000 ; Bank 7 @ $E000..$FAEF is the end screen
> $4000 ;
> $4000 ;
> $4000 ; 128K SOUND
> $4000 ; ----------
> $4000 ; The engine drone is played on Channel C.
> $4000 ;
> $4000 ;
> $4000 ; NOTES
> $4000 ; -----
> $4000 ; Strings are top bit set terminated ASCII.
> $4000 ;
> $4000 ;
> $4000 ; TODO
> $4000 ; ----
> $4000 ; - Decode all stages' level data.
> $4000 ; - Disassemble the Chase HQ demo version.
> $4000 ; - $8008 area used at all?
> $4000 ; - Stretchy streetlamps etc. encoding
> $4000 ; - CHASE HQ MONITORING SYSTEM drawing code
> $4000 ; - $E34B block is what?
> $4000 ; - Stripy tunnel fills.
> $4000 ;
> $4000
@ $4000 org
@ $4000 set-warnings=1
@ $4000 start
@ $4000 writer=:ChaseHQ.ChaseHQAsmWriter
b $4000 Screen memory
D $4000 #UDGTABLE { #SCR(loading) | This is the loading screen. } TABLE#
B $4000,6144,8 Screen bitmap
B $5800,768,8 Screen attributes
c $5B00 The game has loaded
D $5B00 This is entered when the game is first loaded. It disables interrupts, moves memory into the right position, detects 128K machines and then runs the secondary loader to load the first stage (48K) or all stages (128K) as required.
N $5B00 The game starts here.
@ $5B00 label=start
C $5B00,1 Disable interrupts
C $5B01,3 Set stack pointer
N $5B04 Copy $4910 bytes from $5C00+ to $76F0+ (copying backwards to avoid overlap).
C $5B04,3 Final source byte
C $5B07,3 Final destination byte
C $5B0A,3 Count
C $5B0D,2 Copy
N $5B0F Detect 128K machines.
N $5B0F Assuming bank 0 is mapped in to start with, we copy a byte from bank 0.
@ $5B0F label=detect_128k
C $5B0F,4 Read the byte at $FFF0 into #REGe
N $5B13 Now we modify it, then store it back after attempting to page in bank 1.
C $5B13,7 Attempt to map RAM bank 1 to $C000
C $5B1A,2 Increment the read byte by $FD (a convenient non-zero value)
C $5B1C,1 Store it
N $5B1D If the modified byte remains present after restoring bank 0 then it's a 48K machine.
C $5B1D,3 Attempt to map RAM bank 0 to $C000
C $5B20,1 Load potentially changed byte
C $5B21,1 Restore original byte
C $5B22,3 Point #REGhl at loader_commands_48K
N $5B25 If byte was modified no paging took place, so this is a 48K machine.
C $5B25,3 If modified, jump to loader with loader_commands_48K setup
@ $5B28 label=mode_is_128k
C $5B28,3 Otherwise point #REGhl at loader_commands_128K
N $5B2B The loader accepts a command list and uses it to load the game into memory. The 48K command list at #R$5B7D loads stage 1, then starts the game. The 128K command list at #R$5B87 loads all stages while paging in the appropriate banks, then starts the game.
@ $5B2B label=loader
C $5B2B,4 Load a handler address from #REGhl
C $5B2F,2 Call the handler
N $5B31 Loader handler that loads a stage.
@ $5B31 label=loader_load
C $5B31,1 Preserve command list address
C $5B32,4 Address of loader_scratch
C $5B36,3 Length of loader_scratch
C $5B39,3 Call loader_load_chunk
C $5B3C,1 Restore command list address
@ $5B3D label=loader_load_headerless
C $5B3D,7 Load address from command list into #REGix
C $5B44,4 Load length from command list into #REGde
C $5B48,1 Preserve command list address
C $5B49,3 Call loader_load_chunk
N $5B4C The following instruction is the point at which this disassembly was captured.
C $5B4C,1 Restore command list address
C $5B4D,2 Process next entry
N $5B4F Subroutine that loads #REGde bytes to address #REGix.
@ $5B4F label=loader_load_chunk
C $5B4F,3 Call tape_load_at_ix
C $5B52,1 Return if no errors
N $5B53 Infinitely cycle through border colours if a tape loading error occurred.
@ $5B53 label=loader_load_failed
N $5B58 Loader handler that pages in the specified RAM bank.
@ $5B58 label=loader_set_bank
C $5B58,2 Page in RAM where interrupt vector lives
C $5B5A,2 Set Interrupt Control Vector Register
C $5B5C,1 Load 128K paging flags byte
C $5B5D,2 Second byte is unused
C $5B5F,5 128K: Set paging register
C $5B64,2 Process next entry
N $5B66 Loader handler that copies data around.
@ $5B66 label=loader_copy
C $5B66,1 Preserve command list address
C $5B67,11 Move $5C00..$76EF to $C000 onwards
C $5B72,1 Restore command list address
C $5B73,2 Process next entry
N $5B75 Loader handler that starts the game.
@ $5B75 label=loader_start_game
C $5B75,4 Load entrypoint address from #REGhl
C $5B79,3 Set border to black
C $5B7C,1 Exit via entrypoint just read
N $5B7D Loader commands for 48K mode.
@ $5B7D label=loader_commands_48K
W $5B7D,6,2,4 Load stage 1 at $5C00 ($1AF0 bytes)
W $5B83,4,2 Start game at entrypt_48k
N $5B87 Loader commands for 128K mode.
@ $5B87 label=loader_commands_128K
W $5B87,6,2,4 Load stage 1 at $5C00 ($1AF0 bytes)
M $5B8D,4 Set bank 1 (second byte is unused in Set Bank commands)
W $5B8D,2,2
B $5B8F,2,2
W $5B91,2,2 Copy $5C00..$76EF to $C000
W $5B93,6,2,4 Load stage 2 at $E000 ($1AF0 bytes)
M $5B99,4 Set bank 6
W $5B99,2,2
B $5B9B,2,2
W $5B9D,6,2,4 Load stage 3 at $C000 ($1AF0 bytes)
W $5BA3,6,2,4 Load stage 4 at $E000 ($1AF0 bytes)
M $5BA9,4 Set bank 7
W $5BA9,2,2
B $5BAB,2,2
W $5BAD,6,2,4 Load stage 5 at $C000 ($1AF0 bytes)
W $5BB3,6,2,4 Load end screen at $E000 ($1AF0 bytes)
M $5BB9,4 Set bank 3
W $5BB9,2,2
B $5BBB,2,2
W $5BBD,6,2,4 (Headerless) Load animated title screen at $C000 ($4000 bytes)
M $5BC3,4 Set bank 4
W $5BC3,2,2
B $5BC5,2,2
W $5BC7,6,2,4 (Headerless) Load sampled speech at $C000 ($4000 bytes)
M $5BCD,4 Set bank 0
W $5BCD,2,2
B $5BCF,2,2
W $5BD1,4,2 Start game at entrypt_128k
@ $5BD5 label=loader_scratch
B $5BD5,2,2 #R$5B31 loads two bytes to here.  -- unsure what uses them
u $5BD7 Padding bytes
D $5BD7 Bytes to pad this section out to $5C00. These bytes do get touched by e.g. #R$C861.
S $5BD7,31,$1F
B $5BF6,10,10
b $5C00 [Stage 1] Horizon graphic
D $5C00 #HTML[#CALL:graphic($5C00,80,24,0,1)]
@ $5C00 label=bitmap_horizon
B $5C00,240,8 Horizon backdrop (80x24). Non-masked, inverted, bitmap data. Varies per stage.
b $5CF0 [Stage 1] Per-stage data
@ $5CF0 label=addrof_perp_mugshot_attributes
W $5CF0,2,2 Address of Ralph's mugshot attributes
@ $5CF2 label=addrof_perp_mugshot_bitmap
W $5CF2,2,2 Address of Ralph's mugshot bitmap
@ $5CF4 label=ground_colour
W $5CF4,2,2 Screen attributes used for the ground colour (a pair of matching bytes)
@ $5CF6 label=addrof_hittable_objects
W $5CF6,2,2 Loaded by #R$AC1F. Address of table of LODs for tumbleweeds, barriers.
@ $5CF8 label=addrof_right_hand_handlers
@ $5CF8 ssub=DEFW right_hand_handlers - 7
W $5CF8,2,2 Loaded by #R$900F. Address of an array of 7 byte entries. Points 7 bytes earlier to permit 1-indexing.
@ $5CFA label=addrof_right_hand_objects
@ $5CFA ssub=DEFW right_hand_graphics_defs - 7
W $5CFA,2,2 Loaded by #R$A465. Address of graphics entry 1. Points 7 bytes earlier to permit 1-indexing.
@ $5CFC label=addrof_right_hand_short_pole_object
W $5CFC,2,2 Loaded by #R$A53F. Address of graphics entry 3.
@ $5CFE label=addrof_turn_sign_handlers
W $5CFE,2,2 Address of turn sign arg and handler address.
@ $5D00 label=addrof_left_hand_objects
@ $5D00 ssub=DEFW left_hand_graphics_defs - 7
W $5D00,2,2 Loaded by #R$A490. Address of graphics entry 10. Points 7 bytes earlier to permit 1-indexing.
@ $5D02 label=addrof_left_hand_short_pole_object
W $5D02,2,2 Loaded by #R$A55C. Address of graphics entry 12.
@ $5D04 label=addrof_perp_description
W $5D04,2,2 Address of Nancy's perp description.
@ $5D06 label=addrof_arrest_messages
W $5D06,2,2 Address of arrest messages.
@ $5D08 label=addrof_helicopter_stuff_1
W $5D08,2,2 Loaded by #R$AA69. Helicopter related.
@ $5D0A label=addrof_helicopter_stuff_2
W $5D0A,2,2 Loaded by #R$AA6E. Helicopter related.
w $5D0C [Stage 1] Table of addresses of LODs
D $5D0C LODs = Level Of Detail - a set of sprites of various sizes representing the same object.
@ $5D0C label=lods_stones
W $5D0C,2,2 Address of LODs for stones.
@ $5D0E label=lods_dust
W $5D0E,2,2 Address of LODs for dust.
@ $5D10 label=lods_perp_car
W $5D10,2,2 Address of LODs for car (perp's car).
@ $5D12 label=lods_vehicles
W $5D12,2,2 Address of LODs for lambo.
W $5D14,2,2 Address of LODs for truck.
W $5D16,2,2 Address of LODs for lambo (again).
W $5D18,2,2 Address of LODs for car (generic car).
b $5D1A [Stage 1] Per-stage difficulty settings
@ $5D1A label=car_spawn_rate
B $5D1A,1,1 How often cars spawn. Lower values spawn cars more often.
@ $5D1B label=smash_5d1b
B $5D1B,1,1 Loaded by #R$A6A7. Used by smash_handler.
@ $5D1C label=smash_5d1c
B $5D1C,1,1 Loaded by #R$A759.
w $5D1D [Stage 1] Per-stage setup data
@ $5D1D label=stage_set_up_data
W $5D1D,2,2 road_pos
N $5D1F The following all point a byte earlier than the real data's start.
@ $5D1F ssub=DEFW map_start_curvature - 1
W $5D1F,2,2 -> Start stretch, curvature
@ $5D21 ssub=DEFW map_start_height - 1
W $5D21,2,2 -> Start stretch, height
@ $5D23 ssub=DEFW map_start_lanes - 1
W $5D23,2,2 -> Start stretch, lanes
@ $5D25 ssub=DEFW map_start_rightobjs - 1
W $5D25,2,2 -> Start stretch, right-side objects
@ $5D27 ssub=DEFW map_start_leftobjs - 1
W $5D27,2,2 -> Start stretch, left-side objects
@ $5D29 ssub=DEFW map_start_hazards - 1
W $5D29,2,2 -> Start stretch, hazards
w $5D2B [Stage 1] Per-stage attract mode data
@ $5D2B label=attract_data
W $5D2B,2,2 road_pos
N $5D2D These all point a byte earlier than the real data start point.
@ $5D2D ssub=DEFW map_loop_curvature - 1
W $5D2D,2,2 -> Loop section, curvature @ #R$61D4
@ $5D2F ssub=DEFW map_loop_height - 1
W $5D2F,2,2 -> Loop section, height @ #R$6209
@ $5D31 ssub=DEFW map_loop_lanes - 1
W $5D31,2,2 -> Loop section, lanes @ #R$625D
@ $5D33 ssub=DEFW map_loop_rightobjs - 1
W $5D33,2,2 -> Loop section, right-side objects @ #R$6314
@ $5D35 ssub=DEFW map_loop_leftobjs - 1
W $5D35,2,2 -> Loop section, left-side objects @ #R$6294
@ $5D37 ssub=DEFW map_loop_hazards - 1
W $5D37,2,2 -> Loop section, hazards @ #R$6277
b $5D39 [Stage 1] Nancy's perp description
@ $5D39 label=perp_description
B $5D39,1,1 Nancy ($01)
W $5D3A,2,2 Points at "THIS IS NANCY..."
W $5D3C,2,2 Points at "EMERGENCY HERE..."
W $5D3E,2,2 Points at "IS FLEEING ..."
W $5D40,2,2 Points at "VEHICLE IS..."
B $5D42,1,1 (pause?)
W $5D43,2,2 -> Random choice of ("WE READ..." / "ROGER!" / "GOTCHA...")
@ $5D45 label=perp_description_1
T $5D45,40,39:n1 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
@ $5D6D label=perp_description_2
T $5D6D,40,39:n1 "EMERGENCY HERE. RALPH THE IDAHO SLASHER,"
@ $5D95 label=perp_description_3
T $5D95,42,41:n1 "IS FLEEING TOWARDS THE SUBURBS. THE TARGET"
@ $5DBF label=perp_description_4
T $5DBF,46,45:n1 "VEHICLE IS A WHITE BRITISH SPORTS CAR... OVER."
b $5DED [Stage 1] Arrest messages
@ $5DED label=arrest_messages
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
b $5E3F [Stage 1] Graphics definitions
D $5E3F All are stored inverted except where noted.
@ $5E3F label=graphics_defs
B $5E3F,1,1 Pointed to by #R$5CFA
N $5E40 Hittable hazards: tumbleweed and barrier.
@ $5E40 label=hittable_objects_defs
B $5E40,1,1 Pointed to by #R$5CF6
W $5E41,2,2 Address of tumbleweed_lods table
B $5E43,1,1 ?
W $5E44,2,2 Address of barrier_lods table
N $5E46 Right hand side objects.
N $5E46 Entry 1 (tunnel light)
@ $5E46 label=right_hand_graphics_defs
B $5E46,3,3 (hit coord max, hit coord min, ?)
@ $5E49 label=right_hand_handlers
W $5E49,2,2 Arg for routine passed in DE
W $5E4B,2,2 -> Routine at #R$9252
N $5E4D Entry 2 (empty)
B $5E4D,3,3
W $5E50,4,2
N $5E54 Entry 3 (short pole)
@ $5E54 label=right_hand_short_pole_object
B $5E54,3,3
W $5E57,2,2 Arg for routine passed in DE
W $5E59,2,2 -> Routine at #R$9171
N $5E5B Entry 4 (tree, right hand side)
B $5E5B,3,3
W $5E5E,2,2 Arg for routine passed in DE
W $5E60,2,2 -> Routine at #R$9171.
N $5E62 Entry 5 (bush)
B $5E62,3,3
W $5E65,2,2 Arg for routine passed in DE
W $5E67,2,2 -> Routine at #R$9171
N $5E69 Entry 6 (street lamp)
B $5E69,3,3
W $5E6C,2,2 Arg for routine passed in DE
W $5E6E,2,2 -> Routine at #R$9171
N $5E70 Entry 7 (telegraph pole)
B $5E70,3,3
W $5E73,2,2 Arg for routine passed in DE
W $5E75,2,2 -> Routine at #R$9171
N $5E77 Entry 8 (turn sign, pointing left)
B $5E77,3,3
W $5E7A,2,2 Arg for routine passed in DE
W $5E7C,2,2 -> Routine at #R$92E1
N $5E7E Entry 9 (turn sign, pointing right)
B $5E7E,3,3
@ $5E81 label=right_hand_turn_sign_handlers
W $5E81,2,2 -> -> turn_sign_lods
W $5E83,2,2 -> Routine at #R$92E1
N $5E85 Left hand side objects.
N $5E85 Entry 10 (tunnel light)
@ $5E85 label=left_hand_graphics_defs
B $5E85,3,3
W $5E88,2,2 Arg for routine passed in DE
W $5E8A,2,2 -> Routine at #R$924D
N $5E8C Entry 11 (empty)
B $5E8C,3,3
W $5E8F,4,2
N $5E93 Entry 12 (short pole)
@ $5E93 label=left_hand_short_pole_object
B $5E93,3,3
W $5E96,2,2 Arg for routine passed in DE
W $5E98,2,2 -> Routine at #R$916C
N $5E9A Entry 13 (tree, left hand side)
B $5E9A,3,3
W $5E9D,2,2 Arg for routine passed in DE
W $5E9F,2,2 -> Routine at #R$916C
N $5EA1 Entry 14 (bush)
B $5EA1,3,3
W $5EA4,2,2 Arg for routine passed in DE
W $5EA6,2,2 -> Routine at #R$916C
N $5EA8 Entry 15 (street lamp)
B $5EA8,3,3
W $5EAB,2,2 Arg for routine passed in DE
W $5EAD,2,2 -> Routine at #R$916C
N $5EAF Entry 16 (telegraph pole)
B $5EAF,3,3
W $5EB2,2,2 Arg for routine passed in DE
W $5EB4,2,2 -> Routine at #R$916C
N $5EB6 Entry 17 (turn sign, pointing left)
B $5EB6,3,3
W $5EB9,2,2 Arg for routine passed in DE
W $5EBB,2,2 -> Routine at #R$9278
N $5EBD Entry 18 (turn sign, pointing right)
B $5EBD,3,3
W $5EC0,2,2 Arg for routine passed in DE
W $5EC2,2,2 -> Routine at #R$9278
b $5EC4 [Stage 1] Map: Start stretch
N $5EC4 Start stretch
N $5EC4 Start stretch, curvature
@ $5EC4 label=map_start_curvature
B $5EC4,32,8
N $5EE4 Start stretch, height
@ $5EE4 label=map_start_height
B $5EE4,43,8*5,3
N $5F0F Start stretch, lanes
@ $5F0F label=map_start_lanes
B $5F0F,14,8,6
N $5F1D Start stretch, hazards
@ $5F1D label=map_start_hazards
B $5F1D,13,8,5
N $5F2A Start stretch, left-side objects
@ $5F2A label=map_start_leftobjs
B $5F2A,78,8*9,6
N $5F78 Start stretch, right-side objects
@ $5F78 label=map_start_rightobjs
B $5F78,90,8*11,2
b $5FD2 [Stage 1] Map: Left fork
N $5FD2 Left fork
N $5FD2 Left fork, curvature
@ $5FD2 label=map_left_curvature
B $5FD2,20,8*2,4
N $5FE6 Left fork, height
@ $5FE6 label=map_left_height
B $5FE6,29,8*3,5
N $6003 Left fork, lanes
@ $6003 label=map_left_lanes
B $6003,6,6
N $6009 Left fork, hazards
@ $6009 label=map_left_hazards
B $6009,8,8
N $6011 Left fork, left-side objects
@ $6011 label=map_left_leftobjs
B $6011,65,8*8,1
N $6052 Left fork, right-side objects
@ $6052 label=map_left_rightobjs
B $6052,54,8*6,6
b $6088 [Stage 1] Map: Right fork (dirt track)
N $6088 Right fork (dirt track), curvature
@ $6088 label=map_right_curvature
B $6088,27,8*3,3
N $60A3 Right fork (dirt track), height
@ $60A3 label=map_right_height
B $60A3,66,8*8,2
N $60E5 Right fork (dirt track), lanes
@ $60E5 label=map_right_lanes
B $60E5,10,8,2
N $60EF Right fork (dirt track), hazards
@ $60EF label=map_right_hazards
B $60EF,26,8*3,2
N $6109 Right fork (dirt track), left-side objects
@ $6109 label=map_right_leftobjs
B $6109,58,8*7,2
N $6143 Right fork (dirt track), right-side objects
@ $6143 label=map_right_rightobjs
B $6143,48,8
b $6173 [Stage 1] Map: Tunnel section
N $6173 Tunnel section, curvature
@ $6173 label=map_tunnel_curvature
B $6173,22,8*2,6
N $6189 Tunnel section, height
@ $6189 label=map_tunnel_height
B $6189,28,8*3,4
N $61A5 Tunnel section, lanes
@ $61A5 label=map_tunnel_lanes
B $61A5,20,8*2,4
N $61B9 Tunnel section, hazards
@ $61B9 label=map_tunnel_hazards
B $61B9,5,5
N $61BE Tunnel section, left-side objects
@ $61BE label=map_tunnel_leftobjs
B $61BE,11,8,3
N $61C9 Tunnel section, right-side objects
@ $61C9 label=map_tunnel_rightobjs
B $61C9,11,8,3
b $61D4 [Stage 1] Map: Loop section
N $61D4 Loop section, curvature
@ $61D4 label=map_loop_curvature
B $61D4,53,8*6,5
N $6209 Loop section, height
@ $6209 label=map_loop_height
B $6209,84,8*10,4
N $625D Loop section, lanes
@ $625D label=map_loop_lanes
B $625D,26,8*3,2
N $6277 Loop section, hazards
@ $6277 label=map_loop_hazards
B $6277,29,8*3,5
N $6294 Loop section, left-side objects
@ $6294 label=map_loop_leftobjs
B $6294,128,8
N $6314 Loop section, right-side objects
@ $6314 label=map_loop_rightobjs
B $6314,118,8*14,6
b $638A [Stage 1] Ralph the Idaho Slasher's mugshot
N $638A Bitmap data for Ralph the Idaho Slasher's mugshot (32x40). Stored top-down.
N $638A #HTML[#CALL:face($638A)]
@ $638A label=bitmap_ralph
B $638A,160,4
N $642A Attribute data for Ralph the Idaho Slasher's mugshot (4x5). Stored top-down.
@ $642A label=attrs_ralph
B $642A,20,4
b $643E [Stage 1] LODs
D $643E LOD = Level of Detail. These structures collect together the variously sized versions of the same game object.
N $643E Lamborghini LOD 1
@ $643E label=lambo_lods
B $643E,1,1 Width (bytes)
B $643F,1,1 Flags
B $6440,1,1 Height (pixels)
W $6441,2,2 Bitmap
W $6443,2,2 Pre-shifted bitmap
N $6445 Lamborghini LOD 2
B $6445,1,1 Width (bytes)
B $6446,1,1 Flags
B $6447,1,1 Height (pixels)
W $6448,2,2 Bitmap
W $644A,2,2 Pre-shifted bitmap
N $644C Lamborghini LOD 3
B $644C,1,1 Width (bytes)
B $644D,1,1 Flags
B $644E,1,1 Height (pixels)
W $644F,2,2 Bitmap
W $6451,2,2 Pre-shifted bitmap
N $6453 Lamborghini LOD 4
B $6453,1,1 Width (bytes)
B $6454,1,1 Flags
B $6455,1,1 Height (pixels)
W $6456,2,2 Bitmap
W $6458,2,2 Pre-shifted bitmap
N $645A Lamborghini LOD 5
B $645A,1,1 Width (bytes)
B $645B,1,1 Flags
B $645C,1,1 Height (pixels)
W $645D,2,2 Bitmap
W $645F,2,2 Pre-shifted bitmap
N $6461 Lamborghini LOD 6
B $6461,1,1 Width (bytes)
B $6462,1,1 Flags
B $6463,1,1 Height (pixels)
W $6464,2,2 Bitmap
W $6466,2,2 Pre-shifted bitmap
N $6468 Truck LOD 1
@ $6468 label=truck_lods
B $6468,1,1 Width (bytes)
B $6469,1,1 Flags
B $646A,1,1 Height (pixels)
W $646B,2,2 Bitmap
W $646D,2,2 Pre-shifted bitmap
N $646F Truck LOD 2
B $646F,1,1 Width (bytes)
B $6470,1,1 Flags
B $6471,1,1 Height (pixels)
W $6472,2,2 Bitmap
W $6474,2,2 Pre-shifted bitmap
N $6476 Truck LOD 3
B $6476,1,1 Width (bytes)
B $6477,1,1 Flags
B $6478,1,1 Height (pixels)
W $6479,2,2 Bitmap
W $647B,2,2 Pre-shifted bitmap
N $647D Truck LOD 4
B $647D,1,1 Width (bytes)
B $647E,1,1 Flags
B $647F,1,1 Height (pixels)
W $6480,2,2 Bitmap
W $6482,2,2 Pre-shifted bitmap
N $6484 Truck LOD 5
B $6484,1,1 Width (bytes)
B $6485,1,1 Flags
B $6486,1,1 Height (pixels)
W $6487,2,2 Bitmap
W $6489,2,2 Pre-shifted bitmap
N $648B Truck LOD 6
B $648B,1,1 Width (bytes)
B $648C,1,1 Flags
B $648D,1,1 Height (pixels)
W $648E,2,2 Bitmap
W $6490,2,2 Pre-shifted bitmap
N $6492 Car LOD 1
@ $6492 label=car_lods
B $6492,1,1 Width (bytes)
B $6493,1,1 Flags
B $6494,1,1 Height (pixels)
W $6495,2,2 Bitmap
W $6497,2,2 Pre-shifted bitmap
N $6499 Car LOD 2
B $6499,1,1 Width (bytes)
B $649A,1,1 Flags
B $649B,1,1 Height (pixels)
W $649C,2,2 Bitmap
W $649E,2,2 Pre-shifted bitmap
N $64A0 Car LOD 3
B $64A0,1,1 Width (bytes)
B $64A1,1,1 Flags
B $64A2,1,1 Height (pixels)
W $64A3,2,2 Bitmap
W $64A5,2,2 Pre-shifted bitmap
N $64A7 Car LOD 4
B $64A7,1,1 Width (bytes)
B $64A8,1,1 Flags
B $64A9,1,1 Height (pixels)
W $64AA,2,2 Bitmap
W $64AC,2,2 Pre-shifted bitmap
N $64AE Car LOD 5
B $64AE,1,1 Width (bytes)
B $64AF,1,1 Flags
B $64B0,1,1 Height (pixels)
W $64B1,2,2 Bitmap
W $64B3,2,2 Pre-shifted bitmap
N $64B5 Car LOD 6
B $64B5,1,1 Width (bytes)
B $64B6,1,1 Flags
B $64B7,1,1 Height (pixels)
W $64B8,2,2 Bitmap
W $64BA,2,2 Pre-shifted bitmap
N $64BC Bitmap: Lamborghini_1 (48x30)
N $64BC #HTML[#CALL:graphic($64BC,48,30,0,1)]
@ $64BC label=bitmap_lamborghini_1
B $64BC,180,6 Bitmap data
N $6570 Bitmap: Lamborghini_2 (40x22)
N $6570 #HTML[#CALL:graphic($6570,40,22,0,1)]
@ $6570 label=bitmap_lamborghini_2
B $6570,110,5 Bitmap data
N $65DE Bitmap: Lamborghini_3 (24x15)
N $65DE #HTML[#CALL:graphic($65DE,24,15,0,1)]
@ $65DE label=bitmap_lamborghini_3
B $65DE,45,3 Bitmap data
N $660B Bitmap: Truck_1 (48x39)
N $660B #HTML[#CALL:graphic($660B,48,39,0,1)]
@ $660B label=bitmap_truck_1
B $660B,234,6 Bitmap data
N $66F5 Bitmap: Truck_2 (40x29)
N $66F5 #HTML[#CALL:graphic($66F5,40,29,0,1)]
@ $66F5 label=bitmap_truck_2
B $66F5,145,5 Bitmap data
N $6786 Bitmap: Truck_3 (24x20)
N $6786 #HTML[#CALL:graphic($6786,24,20,0,1)]
@ $6786 label=bitmap_truck_3
B $6786,60,3 Bitmap data
N $67C2 Bitmap: Car_1 (48x31)
N $67C2 #HTML[#CALL:graphic($67C2,48,31,0,1)]
@ $67C2 label=bitmap_car_1
B $67C2,186,6 Bitmap data
N $687C Bitmap: Car_2 (40x22)
N $687C #HTML[#CALL:graphic($687C,40,22,0,1)]
@ $687C label=bitmap_car_2
B $687C,110,5 Bitmap data
N $68EA Bitmap: Car_3 (24x16)
N $68EA #HTML[#CALL:graphic($68EA,24,16,0,1)]
@ $68EA label=bitmap_car_3
B $68EA,48,3 Bitmap data
N $691A Bitmap: Lamborghini_4 (24x8)
N $691A #HTML[#CALL:graphic($691A,24,8,1,1)]
@ $691A label=bitmap_lamborghini_4
B $691A,48,6 Masked bitmap data
N $694A Bitmap: Lamborghini_4 (24x8) pre-shifted
N $694A #HTML[#CALL:graphic($694A,24,8,1,1)]
@ $694A label=bitmap_lamborghini_4s
B $694A,48,6 Masked bitmap data
N $697A Bitmap: Truck_4 (16x12)
N $697A #HTML[#CALL:graphic($697A,16,12,1,1)]
@ $697A label=bitmap_truck_4
B $697A,48,4 Masked bitmap data
N $69AA Bitmap: Truck_4 (16x12) pre-shifted
N $69AA #HTML[#CALL:graphic($69AA,16,12,1,1)]
@ $69AA label=bitmap_truck_4s
B $69AA,48,4 Masked bitmap data
N $69DA Bitmap: Car_4 (24x9)
N $69DA #HTML[#CALL:graphic($69DA,24,9,1,1)]
@ $69DA label=bitmap_car_4
B $69DA,54,6 Masked bitmap data
N $6A10 Bitmap: Car_4 (24x9) pre-shifted
N $6A10 #HTML[#CALL:graphic($6A10,24,9,1,1)]
@ $6A10 label=bitmap_car_4s
B $6A10,54,6 Masked bitmap data
N $6A46 Stones LOD 1
@ $6A46 label=stones_lods
B $6A46,1,1 Width (bytes)
B $6A47,1,1 Flags
B $6A48,1,1 Height (pixels)
W $6A49,2,2 Bitmap
W $6A4B,2,2 Pre-shifted bitmap
N $6A4D Stones LOD 2
B $6A4D,1,1 Width (bytes)
B $6A4E,1,1 Flags
B $6A4F,1,1 Height (pixels)
W $6A50,2,2 Bitmap
W $6A52,2,2 Pre-shifted bitmap
N $6A54 Stones LOD 3
B $6A54,1,1 Width (bytes)
B $6A55,1,1 Flags
B $6A56,1,1 Height (pixels)
W $6A57,2,2 Bitmap
W $6A59,2,2 Pre-shifted bitmap
N $6A5B Stones LOD 4
B $6A5B,1,1 Width (bytes)
B $6A5C,1,1 Flags
B $6A5D,1,1 Height (pixels)
W $6A5E,2,2 Bitmap
W $6A60,2,2 Pre-shifted bitmap
N $6A62 Stones LOD 5
B $6A62,1,1 Width (bytes)
B $6A63,1,1 Flags
B $6A64,1,1 Height (pixels)
W $6A65,2,2 Bitmap
W $6A67,2,2 Pre-shifted bitmap
N $6A69 Stones LOD 6
B $6A69,1,1 Width (bytes)
B $6A6A,1,1 Flags
B $6A6B,1,1 Height (pixels)
W $6A6C,2,2 Bitmap
W $6A6E,2,2 Pre-shifted bitmap
N $6A70 Dust LOD 1
@ $6A70 label=dust_lods
B $6A70,1,1 Width (bytes)
B $6A71,1,1 Flags
B $6A72,1,1 Height (pixels)
W $6A73,2,2 Bitmap
W $6A75,2,2 Pre-shifted bitmap
N $6A77 Dust LOD 2
B $6A77,1,1 Width (bytes)
B $6A78,1,1 Flags
B $6A79,1,1 Height (pixels)
W $6A7A,2,2 Bitmap
W $6A7C,2,2 Pre-shifted bitmap
N $6A7E Dust LOD 3
B $6A7E,1,1 Width (bytes)
B $6A7F,1,1 Flags
B $6A80,1,1 Height (pixels)
W $6A81,2,2 Bitmap
W $6A83,2,2 Pre-shifted bitmap
N $6A85 Dust LOD 4
B $6A85,1,1 Width (bytes)
B $6A86,1,1 Flags
B $6A87,1,1 Height (pixels)
W $6A88,2,2 Bitmap
W $6A8A,2,2 Pre-shifted bitmap
N $6A8C Dust LOD 5
B $6A8C,1,1 Width (bytes)
B $6A8D,1,1 Flags
B $6A8E,1,1 Height (pixels)
W $6A8F,2,2 Bitmap
W $6A91,2,2 Pre-shifted bitmap
N $6A93 Dust LOD 6
B $6A93,1,1 Width (bytes)
B $6A94,1,1 Flags
B $6A95,1,1 Height (pixels)
W $6A96,2,2 Bitmap
W $6A98,2,2 Pre-shifted bitmap
N $6A9A Bitmap: Stones (16x5)
N $6A9A #HTML[#CALL:graphic($6A9A,16,5,1,1)]
@ $6A9A label=bitmap_stones_1
B $6A9A,20,4 Masked bitmap data
N $6AAE Bitmap: Stones (16x4)
N $6AAE #HTML[#CALL:graphic($6AAE,16,4,1,1)]
@ $6AAE label=bitmap_stones_2
B $6AAE,16,4 Masked bitmap data
N $6ABE Bitmap: Stones (16x4) pre-shifted
N $6ABE #HTML[#CALL:graphic($6ABE,16,4,1,1)]
@ $6ABE label=bitmap_stones_2s
B $6ABE,16,4 Masked bitmap data
N $6ACE Bitmap: Stones (16x3)
N $6ACE #HTML[#CALL:graphic($6ACE,16,3,1,1)]
@ $6ACE label=bitmap_stones_3
B $6ACE,12,4 Masked bitmap data
N $6ADA Bitmap: Stones (16x3) pre-shifted
N $6ADA #HTML[#CALL:graphic($6ADA,16,3,1,1)]
@ $6ADA label=bitmap_stones_3s
B $6ADA,12,4 Masked bitmap data
N $6AE6 Bitmap: Stones (8x2)
N $6AE6 #HTML[#CALL:graphic($6AE6,8,2,1,1)]
@ $6AE6 label=bitmap_stones_4
B $6AE6,4,2 Masked bitmap data
N $6AEA Bitmap: Stones (8x2) pre-shifted
N $6AEA #HTML[#CALL:graphic($6AEA,8,2,1,1)]
@ $6AEA label=bitmap_stones_4s
B $6AEA,4,2 Masked bitmap data
N $6AEE Bitmap: Stones (8x1)
N $6AEE #HTML[#CALL:graphic($6AEE,8,1,1,1)]
@ $6AEE label=bitmap_stones_5
B $6AEE,2,2 Masked bitmap data
N $6AF0 Bitmap: Stones (8x1) pre-shifted
N $6AF0 #HTML[#CALL:graphic($6AF0,8,1,1,1)]
@ $6AF0 label=bitmap_stones_5s
B $6AF0,2,2 Masked bitmap data
N $6AF2 Bitmap: Dust (8x1)
N $6AF2 #HTML[#CALL:graphic($6AF2,8,1,1,1)]
@ $6AF2 label=bitmap_dust_1
B $6AF2,2,2 Masked bitmap data
N $6AF4 Bitmap: Dust (8x1) pre-shifted
N $6AF4 #HTML[#CALL:graphic($6AF4,8,1,1,1)]
@ $6AF4 label=bitmap_dust_1s
B $6AF4,2,2 Masked bitmap data
@ $6AF6 label=turn_sign_right
W $6AF6,2,2 -> turn_sign_lods
B $6AF8,20,2 Read by #R$928D and #R$92F6
N $6B0C Referenced by graphic entry 8 and 17
@ $6B0C label=turn_sign_left
W $6B0C,2,2 -> turn_sign_lods
B $6B0E,20,2
N $6B22 Turn sign LOD 1
@ $6B22 label=turn_sign_lods
B $6B22,1,1 Width (bytes)
B $6B23,1,1 Flags
B $6B24,1,1 Height (pixels)
W $6B25,2,2 Bitmap
W $6B27,2,2 Pre-shifted bitmap
N $6B29 Turn sign LOD 2
B $6B29,1,1 Width (bytes)
B $6B2A,1,1 Flags
B $6B2B,1,1 Height (pixels)
W $6B2C,2,2 Bitmap
W $6B2E,2,2 Pre-shifted bitmap
N $6B30 Turn sign LOD 3
B $6B30,1,1 Width (bytes)
B $6B31,1,1 Flags
B $6B32,1,1 Height (pixels)
W $6B33,2,2 Bitmap
W $6B35,2,2 Pre-shifted bitmap
N $6B37 Turn sign LOD 4
B $6B37,1,1 Width (bytes)
B $6B38,1,1 Flags
B $6B39,1,1 Height (pixels)
W $6B3A,2,2 Bitmap
W $6B3C,2,2 Pre-shifted bitmap
N $6B3E Turn sign LOD 5
B $6B3E,1,1 Width (bytes)
B $6B3F,1,1 Flags
B $6B40,1,1 Height (pixels)
W $6B41,2,2 Bitmap
W $6B43,2,2 Pre-shifted bitmap
N $6B45 Turn sign LOD 1 (flipped)
B $6B45,1,1 Width (bytes)
B $6B46,1,1 Flags
B $6B47,1,1 Height (pixels)
W $6B48,2,2 Bitmap
W $6B4A,2,2 Pre-shifted bitmap
N $6B4C Turn sign LOD 2 (flipped)
B $6B4C,1,1 Width (bytes)
B $6B4D,1,1 Flags
B $6B4E,1,1 Height (pixels)
W $6B4F,2,2 Bitmap
W $6B51,2,2 Pre-shifted bitmap
N $6B53 Turn sign LOD 3 (flipped)
B $6B53,1,1 Width (bytes)
B $6B54,1,1 Flags
B $6B55,1,1 Height (pixels)
W $6B56,2,2 Bitmap
W $6B58,2,2 Pre-shifted bitmap
N $6B5A Turn sign LOD 4 (flipped)
B $6B5A,1,1 Width (bytes)
B $6B5B,1,1 Flags
B $6B5C,1,1 Height (pixels)
W $6B5D,2,2 Bitmap
W $6B5F,2,2 Pre-shifted bitmap
N $6B61 Turn sign LOD 5 (flipped)
B $6B61,1,1 Width (bytes)
B $6B62,1,1 Flags
B $6B63,1,1 Height (pixels)
W $6B64,2,2 Bitmap
W $6B66,2,2 Pre-shifted bitmap
N $6B68 Turn right sign (32x40)
N $6B68 #HTML[#CALL:graphic($6B68,32,40,0,1)]
@ $6B68 label=bitmap_turnsign_1
B $6B68,160,4 Bitmap data
N $6C08 Turn right sign (24x30)
N $6C08 #HTML[#CALL:graphic($6C08,24,30,0,1)]
@ $6C08 label=bitmap_turnsign_2
B $6C08,90,3 Bitmap data
N $6C62 Turn right sign (16x20)
N $6C62 #HTML[#CALL:graphic($6C62,16,20,0,1)]
@ $6C62 label=bitmap_turnsign_3
B $6C62,40,2 Bitmap data
N $6C8A Turn right sign (16x16)
N $6C8A #HTML[#CALL:graphic($6C8A,16,16,1,1)]
@ $6C8A label=bitmap_turnsign_4
B $6C8A,64,4 Masked bitmap data
N $6CCA Turn right sign (16x13)
N $6CCA #HTML[#CALL:graphic($6CCA,16,13,1,1)]
@ $6CCA label=bitmap_turnsign_5
B $6CCA,52,2 Masked bitmap data
N $6CFE Turn right sign (16x13) pre-shifted
N $6CFE #HTML[#CALL:graphic($6CFE,16,13,1,1)]
@ $6CFE label=bitmap_turnsign_5s
B $6CFE,52,2 Masked bitmap data
N $6D32 Turn right sign (16x10)
N $6D32 #HTML[#CALL:graphic($6D32,16,10,1,1)]
@ $6D32 label=bitmap_turnsign_6
B $6D32,40,4 Masked bitmap data
N $6D5A Turn right sign (16x10) pre-shifted
N $6D5A #HTML[#CALL:graphic($6D5A,16,10,1,1)]
@ $6D5A label=bitmap_turnsign_6s
B $6D5A,40,4 Masked bitmap data
N $6D82 Tumbleweed LOD 1
@ $6D82 label=tumbleweed_lods
B $6D82,1,1 Width (bytes)
B $6D83,1,1 Flags
B $6D84,1,1 Height (pixels)
W $6D85,2,2 Bitmap
W $6D87,2,2 Pre-shifted bitmap
N $6D89 Tumbleweed LOD 2
B $6D89,1,1 Width (bytes)
B $6D8A,1,1 Flags
B $6D8B,1,1 Height (pixels)
W $6D8C,2,2 Bitmap
W $6D8E,2,2 Pre-shifted bitmap
N $6D90 Tumbleweed LOD 3
B $6D90,1,1 Width (bytes)
B $6D91,1,1 Flags
B $6D92,1,1 Height (pixels)
W $6D93,2,2 Bitmap
W $6D95,2,2 Pre-shifted bitmap
N $6D97 Tumbleweed LOD 4
B $6D97,1,1 Width (bytes)
B $6D98,1,1 Flags
B $6D99,1,1 Height (pixels)
W $6D9A,2,2 Bitmap
W $6D9C,2,2 Pre-shifted bitmap
N $6D9E Tumbleweed LOD 5
B $6D9E,1,1 Width (bytes)
B $6D9F,1,1 Flags
B $6DA0,1,1 Height (pixels)
W $6DA1,2,2 Bitmap
W $6DA3,2,2 Pre-shifted bitmap
N $6DA5 Tumbleweed LOD 6
B $6DA5,1,1 Width (bytes)
B $6DA6,1,1 Flags
B $6DA7,1,1 Height (pixels)
W $6DA8,2,2 Bitmap
W $6DAA,2,2 Pre-shifted bitmap
N $6DAC Bitmap: Tumbleweed_1 (16x16)
N $6DAC #HTML[#CALL:graphic($6DAC,16,16,0,1)]
@ $6DAC label=bitmap_tumbleweed_1
B $6DAC,32,2 Bitmap data
N $6DCC Bitmap: Tumbleweed_2 (16x11)
N $6DCC #HTML[#CALL:graphic($6DCC,16,11,0,1)]
@ $6DCC label=bitmap_tumbleweed_2
B $6DCC,22,2 Bitmap data
N $6DE2 Bitmap: Tumbleweed_3 (8x9)
N $6DE2 #HTML[#CALL:graphic($6DE2,8,9,0,1)]
@ $6DE2 label=bitmap_tumbleweed_3
B $6DE2,9,1 Bitmap data
N $6DEB Bitmap: Tumbleweed_4 (8x7)
N $6DEB #HTML[#CALL:graphic($6DEB,8,7,0,1)]
@ $6DEB label=bitmap_tumbleweed_4
B $6DEB,7,1 Bitmap data
N $6DF2 Barrier LOD 1
@ $6DF2 label=barrier_lods
B $6DF2,1,1 Width (bytes)
B $6DF3,1,1 Flags
B $6DF4,1,1 Height (pixels)
W $6DF5,2,2 Bitmap
W $6DF7,2,2 Pre-shifted bitmap
N $6DF9 Barrier LOD 2
B $6DF9,1,1 Width (bytes)
B $6DFA,1,1 Flags
B $6DFB,1,1 Height (pixels)
W $6DFC,2,2 Bitmap
W $6DFE,2,2 Pre-shifted bitmap
N $6E00 Barrier LOD 3
B $6E00,1,1 Width (bytes)
B $6E01,1,1 Flags
B $6E02,1,1 Height (pixels)
W $6E03,2,2 Bitmap
W $6E05,2,2 Pre-shifted bitmap
N $6E07 Barrier LOD 4
B $6E07,1,1 Width (bytes)
B $6E08,1,1 Flags
B $6E09,1,1 Height (pixels)
W $6E0A,2,2 Bitmap
W $6E0C,2,2 Pre-shifted bitmap
N $6E0E Barrier LOD 5
B $6E0E,1,1 Width (bytes)
B $6E0F,1,1 Flags
B $6E10,1,1 Height (pixels)
W $6E11,2,2 Bitmap
W $6E13,2,2 Pre-shifted bitmap
N $6E15 Barrier LOD 6
B $6E15,1,1 Width (bytes)
B $6E16,1,1 Flags
B $6E17,1,1 Height (pixels)
W $6E18,2,2 Bitmap
W $6E1A,2,2 Pre-shifted bitmap
N $6E1C Bitmap: Barrier (32x17)
N $6E1C #HTML[#CALL:graphic($6E1C,32,17,0,1)]
@ $6E1C label=bitmap_barrier_1
B $6E1C,68,4 Bitmap data
N $6E60 Bitmap: Barrier (24x13)
N $6E60 #HTML[#CALL:graphic($6E60,24,13,0,1)]
@ $6E60 label=bitmap_barrier_2
B $6E60,39,3 Bitmap data
N $6E87 Bitmap: Barrier (16x9)
N $6E87 #HTML[#CALL:graphic($6E87,16,9,0,1)]
@ $6E87 label=bitmap_barrier_3
B $6E87,18,2 Bitmap data
N $6E99 Bitmap: Barrier (16x7)
N $6E99 #HTML[#CALL:graphic($6E99,16,7,1,1)]
@ $6E99 label=bitmap_barrier_4
B $6E99,28,4 Masked bitmap data
N $6EB5 Bitmap: Barrier (16x7) pre-shifted
N $6EB5 #HTML[#CALL:graphic($6EB5,16,7,1,1)]
@ $6EB5 label=bitmap_barrier_4s
B $6EB5,28,4 Masked bitmap data
@ $6ED1 label=stretchy_streetlamp_right
B $6ED1,1,1
W $6ED2,2,2
B $6ED4,1,1
W $6ED5,2,2
B $6ED7,1,1
W $6ED8,2,2
N $6EDA 0 => draws loads 1 => draws none  (1 is terminator) 2 => draws it once 3 => draws at least 3/4 4+ => draws none
B $6EDA,1,1 flag meaning draw once, or repeat, or?
W $6EDB,2,2 top
B $6EDD,1,1 terminator
@ $6EDE label=stretchy_streetlamp_left
B $6EDE,1,1
W $6EDF,2,2
B $6EE1,1,1
W $6EE2,2,2
B $6EE4,1,1
W $6EE5,2,2
B $6EE7,1,1
W $6EE8,2,2
B $6EEA,1,1
@ $6EEB label=streetlamptop_6eeb
W $6EEB,2,2 Address of another LOD table
B $6EED,20,2
@ $6F01 label=streetlamptop_flipped_6f01
W $6F01,2,2 Address of another LOD table
B $6F03,20,2
N $6F17 Street lamp LOD 1
@ $6F17 label=streetlamptop_lods
B $6F17,1,1 Width (bytes)
B $6F18,1,1 Flags
B $6F19,1,1 Height
W $6F1A,2,2 Bitmap
W $6F1C,2,2 Pre-shifted bitmap
N $6F1E Street lamp LOD 2
B $6F1E,1,1 Width (bytes)
B $6F1F,1,1 Flags
B $6F20,1,1 Height
W $6F21,2,2 Bitmap
W $6F23,2,2 Pre-shifted bitmap
N $6F25 Street lamp LOD 3
B $6F25,1,1 Width (bytes)
B $6F26,1,1 Flags
B $6F27,1,1 Height
W $6F28,2,2 Bitmap
W $6F2A,2,2 Pre-shifted bitmap
N $6F2C Street lamp LOD 4
B $6F2C,1,1 Width (bytes)
B $6F2D,1,1 Flags
B $6F2E,1,1 Height
W $6F2F,2,2 Bitmap
W $6F31,2,2 Pre-shifted bitmap
N $6F33 Street lamp LOD 5
B $6F33,1,1 Width (bytes)
B $6F34,1,1 Flags
B $6F35,1,1 Height
W $6F36,2,2 Bitmap
W $6F38,2,2 Pre-shifted bitmap
N $6F3A Street lamp LOD 1 (flipped)
@ $6F3A label=streetlamptop_flipped_lods
B $6F3A,1,1 Width (bytes)
B $6F3B,1,1 Flags
B $6F3C,1,1 Height
W $6F3D,2,2 Bitmap
W $6F3F,2,2 Pre-shifted bitmap
N $6F41 Street lamp LOD 2 (flipped)
B $6F41,1,1 Width (bytes)
B $6F42,1,1 Flags
B $6F43,1,1 Height
W $6F44,2,2 Bitmap
W $6F46,2,2 Pre-shifted bitmap
N $6F48 Street lamp LOD 3 (flipped)
B $6F48,1,1 Width (bytes)
B $6F49,1,1 Flags
B $6F4A,1,1 Height
W $6F4B,2,2 Bitmap
W $6F4D,2,2 Pre-shifted bitmap
N $6F4F Street lamp LOD 4 (flipped)
B $6F4F,1,1 Width (bytes)
B $6F50,1,1 Flags
B $6F51,1,1 Height
W $6F52,2,2 Bitmap
W $6F54,2,2 Pre-shifted bitmap
N $6F56 Street lamp LOD 5 (flipped)
B $6F56,1,1 Width (bytes)
B $6F57,1,1 Flags
B $6F58,1,1 Height
W $6F59,2,2 Bitmap
W $6F5B,2,2 Pre-shifted bitmap
N $6F5D Street lamp top (32x8)
N $6F5D #HTML[#CALL:graphic($6F5D,32,8,0,1)]
@ $6F5D label=bitmap_streetlamptop_1
B $6F5D,32,4 Bitmap data
N $6F7D Street lamp top (24x5)
N $6F7D #HTML[#CALL:graphic($6F7D,24,5,0,1)]
@ $6F7D label=bitmap_streetlamptop_2
B $6F7D,15,3 Bitmap data
N $6F8C Street lamp top (24x4)
N $6F8C #HTML[#CALL:graphic($6F8C,24,4,0,1)]
@ $6F8C label=bitmap_streetlamptop_3
B $6F8C,12,3 Bitmap data
N $6F98 Street lamp top (24x4) pre-shifted
N $6F98 #HTML[#CALL:graphic($6F98,24,4,0,1)]
@ $6F98 label=bitmap_streetlamptop_3s
B $6F98,12,3 Bitmap data
N $6FA4 Street lamp top (16x4)
N $6FA4 #HTML[#CALL:graphic($6FA4,16,4,1,1)]
@ $6FA4 label=bitmap_streetlamptop_4
B $6FA4,16,2 Masked bitmap data
N $6FB4 Street lamp top (16x4) pre-shifted
N $6FB4 #HTML[#CALL:graphic($6FB4,16,4,1,1)]
@ $6FB4 label=bitmap_streetlamptop_4s
B $6FB4,16,2 Masked bitmap data
N $6FC4 Street lamp top (16x3)
N $6FC4 #HTML[#CALL:graphic($6FC4,16,3,1,1)]
@ $6FC4 label=bitmap_streetlamptop_5
B $6FC4,12,2 Masked bitmap data
N $6FD0 Street lamp top (16x3) pre-shifted
N $6FD0 #HTML[#CALL:graphic($6FD0,16,3,1,1)]
@ $6FD0 label=bitmap_streetlamptop_5s
B $6FD0,12,2 Masked bitmap data
N $6FDC Referenced by graphic entry 7
@ $6FDC label=stretchy_telegraphpole_right
B $6FDC,1,1
W $6FDD,2,2
B $6FDF,1,1
W $6FE0,2,2
B $6FE2,1,1
W $6FE3,2,2
B $6FE5,1,1
N $6FE6 Referenced by graphic entry 16
@ $6FE6 label=stretchy_telegraphpole_left
B $6FE6,1,1
W $6FE7,2,2
B $6FE9,1,1
W $6FEA,2,2
B $6FEC,1,1
W $6FED,2,2
B $6FEF,1,1
@ $6FF0 label=telegraphpoletop_6ff0
W $6FF0,2,2
B $6FF2,20,2
@ $7006 label=telegraphpoletop_7006
W $7006,2,2
B $7008,20,2
@ $701C label=telegraphpoletop_lods
B $701C,1,1 Width (bytes)
B $701D,1,1 Flags
B $701E,1,1 Height
W $701F,2,2 Bitmap
W $7021,2,2 Pre-shifted bitmap
B $7023,1,1 Width (bytes)
B $7024,1,1 Flags
B $7025,1,1 Height
W $7026,2,2 Bitmap
W $7028,2,2 Pre-shifted bitmap
B $702A,1,1 Width (bytes)
B $702B,1,1 Flags
B $702C,1,1 Height
W $702D,2,2 Bitmap
W $702F,2,2 Pre-shifted bitmap
B $7031,1,1 Width (bytes)
B $7032,1,1 Flags
B $7033,1,1 Height
W $7034,2,2 Bitmap
W $7036,2,2 Pre-shifted bitmap
B $7038,1,1 Width (bytes)
B $7039,1,1 Flags
B $703A,1,1 Height
W $703B,2,2 Bitmap
W $703D,2,2 Pre-shifted bitmap
N $703F Top of telegraph pole (24x13)
N $703F #HTML[#CALL:graphic($703F,24,13,0,1)]
@ $703F label=bitmap_telegraphpoletop_1
B $703F,8,8 Bitmap data
B $7047,31,8*3,7
N $7066 Top of telegraph pole (24x10)
N $7066 #HTML[#CALL:graphic($7066,24,10,0,1)]
@ $7066 label=bitmap_telegraphpoletop_2
B $7066,8,8 Bitmap data
B $706E,22,8*2,6
N $7084 Top of telegraph pole (24x7)
N $7084 #HTML[#CALL:graphic($7084,24,7,0,1)]
@ $7084 label=bitmap_telegraphpoletop_3
B $7084,8,8 Bitmap data
B $708C,13,8,5
N $7099 Top of telegraph pole (24x7) pre-shifted
N $7099 #HTML[#CALL:graphic($7099,24,7,0,1)]
@ $7099 label=bitmap_telegraphpoletop_3s
B $7099,8,8 Bitmap data
B $70A1,13,8,5
N $70AE Top of telegraph pole (16x5) masked
N $70AE #HTML[#CALL:graphic($70AE,16,5,1,1)]
@ $70AE label=bitmap_telegraphpoletop_4
B $70AE,8,8 Masked bitmap data
B $70B6,12,8,4
N $70C2 Top of telegraph pole (16x5) pre-shifted and masked
N $70C2 #HTML[#CALL:graphic($70C2,16,5,1,1)]
@ $70C2 label=bitmap_telegraphpoletop_4s
B $70C2,8,8 Masked bitmap data
B $70CA,12,8,4
N $70D6 Top of telegraph pole (16x4)
N $70D6 #HTML[#CALL:graphic($70D6,16,4,1,1)]
@ $70D6 label=bitmap_telegraphpoletop_5
B $70D6,8,8 Masked bitmap data
B $70DE,8,8
N $70E6 Top of telegraph pole (16x4) pre-shifted and masked
N $70E6 #HTML[#CALL:graphic($70E6,16,4,1,1)]
@ $70E6 label=bitmap_telegraphpoletop_5s
B $70E6,8,8 Masked bitmap data
@ $70F6 label=stretchy_tree_right
B $70EE,9,8,1
W $70F7,2,2
B $70F9,1,1
W $70FA,2,2
B $70FC,1,1
W $70FD,2,2
B $70FF,1,1
W $7100,2,2
B $7102,1,1
W $7103,2,2
@ $7106 label=stretchy_tree_left
B $7105,2,1
W $7107,2,2
B $7109,1,1
W $710A,2,2
B $710C,1,1
W $710D,2,2
B $710F,1,1
@ $7110 label=tree_7110
W $7110,2,2 -> tree_lods
B $7112,20,2
@ $7126 label=tree_7126
W $7126,2,2 -> tree_lods
B $7128,20,2
@ $713C label=tree_713c
W $713C,2,2 -> tree_lods
B $713E,20,2
@ $7152 label=tree_7152
W $7152,2,2 -> tree_lods
B $7154,20,2
@ $7168 label=tree_7168
W $7168,2,2 -> tree_lods
B $716A,20,2
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
N $71E0 LOD - Tree top (64x13)
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
u $7636 [Stage 1] Spare space
D $7636 This is the end of the per-stage data.
S $7636,186,$BA
b $76F0 [Graphics] Turbo icons
D $76F0 #HTML[#CALL:anim($76F0,16,14,1,1,3)]
N $76F0 Frame 1 (16x14)
N $76F0 #HTML[#CALL:graphic($76F0,16,14,1,1)]
@ $76F0 label=bitmap_turbospin_1
B $76F0,56,4 Masked bitmap data
N $7728 Frame 2 (16x14)
N $7728 #HTML[#CALL:graphic($7728,16,14,1,1)]
@ $7728 label=bitmap_turbospin_2
B $7728,56,4 Masked bitmap data
N $7760 Frame 3 (16x14)
N $7760 #HTML[#CALL:graphic($7760,16,14,1,1)]
@ $7760 label=bitmap_turbospin_3
B $7760,56,4 Masked bitmap data
b $7798 [Pre-game] Messages
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
b $77D8 [Pre-game] Drawing commands for the pre-game screen
@ $77D8 label=pregame_data
B $77D8,1,1 Set colour 12 (Bright Green)
B $77D9,1,1 Draw horizontally
@ $77DA keep
W $77DA,2,2 Set address to (1,0)
B $77DC,1,1 Plot tile 0
B $77DD,1,1 Repeat 28
B $77DE,1,1 Plot tile 6
B $77DF,1,1 Plot tile 1
@ $77E0 keep
W $77E0,2,2 Set address to (1,0)
B $77E2,1,1 Plot tile 2
B $77E3,1,1 Repeat 28
B $77E4,1,1 Plot tile 5
B $77E5,1,1 Plot tile 3
@ $77E6 keep
W $77E6,2,2 Set address to (1,1)
B $77E8,1,1 Plot tile 4
@ $77E9 keep
W $77E9,2,2 Set address to (30,1)
B $77EB,1,1 Plot tile 7
B $77EC,1,1 Set colour 13 (Bright Cyan)
@ $77ED keep
W $77ED,2,2 Set address to (1,4)
B $77EF,1,1 Plot tile 0
B $77F0,1,1 Repeat 28
B $77F1,1,1 Plot tile 6
B $77F2,1,1 Plot tile 1
@ $77F3 keep
W $77F3,2,2 Set address to (1,13)
B $77F5,1,1 Plot tile 2
B $77F6,1,1 Repeat 28
B $77F7,1,1 Plot tile 5
B $77F8,1,1 Plot tile 3
B $77F9,1,1 Draw vertically
@ $77FA keep
W $77FA,2,2 Set address to (1,5)
B $77FC,1,1 Repeat 10
B $77FD,1,1 Plot tile 4
@ $77FE keep
W $77FE,2,2 Set address to (30,5)
B $7800,1,1 Repeat 10
B $7801,1,1 Plot tile 7
B $7802,1,1 Set colour 0 (Black)
B $7803,1,1 Draw horizontally
@ $7804 keep
W $7804,2,2 Set address to (23,9)
B $7806,1,1 Repeat 7
B $7807,1,1 Plot tile 8
@ $7808 keep
W $7808,2,2 Set address to (23,9)
B $780A,1,1 Repeat 7
B $780B,1,1 Plot tile 8
@ $780C keep
W $780C,2,2 Set address to (9,5)
B $780E,1,1 Set colour 15 (Bright White)
B $780F,1,1 Plot tile 11
B $7810,1,1 Plot tile 12
B $7811,1,1 Repeat 9
B $7812,1,1 Plot tile 13
B $7813,1,1 Plot tile 14
B $7814,1,1 Plot tile 15
B $7815,1,1 Plot tile 16
@ $7816 keep
W $7816,2,2 Set address to (9,4)
B $7818,1,1 Set colour 15 (Bright White)
B $7819,1,1 Plot tile 22
B $781A,1,1 Set colour 13 (Bright Cyan)
B $781B,1,1 Plot tile 23
B $781C,1,1 Repeat 9
B $781D,1,1 Plot tile 24
B $781E,1,1 Plot tile 25
B $781F,1,1 Plot tile 26
B $7820,1,1 Set colour 15 (Bright White)
B $7821,1,1 Plot tile 27
@ $7822 keep
W $7822,2,2 Set address to (11,5)
B $7824,1,1 Set colour 6 (Yellow)
B $7825,1,1 Plot tile 35
B $7826,1,1 Plot tile 17
B $7827,1,1 Plot tile 17
B $7828,1,1 Repeat 4
B $7829,1,1 Plot tile 18
B $782A,1,1 Plot tile 17
B $782B,1,1 Plot tile 17
B $782C,1,1 Plot tile 36
@ $782D keep
W $782D,2,2 Set address to (11,8)
B $782F,1,1 Set colour 14 (Bright Yellow)
B $7830,1,1 Repeat 10
B $7831,1,1 Plot tile 44
@ $7832 keep
W $7832,2,2 Set address to (11,9)
B $7834,1,1 Repeat 10
B $7835,1,1 Plot tile 44
@ $7836 keep
W $7836,2,2 Set address to (11,8)
B $7838,1,1 Repeat 10
B $7839,1,1 Plot tile 44
@ $783A keep
W $783A,2,2 Set address to (11,9)
B $783C,1,1 Repeat 10
B $783D,1,1 Plot tile 44
@ $783E keep
W $783E,2,2 Set address to (11,12)
B $7840,1,1 Repeat 10
B $7841,1,1 Plot tile 44
B $7842,1,1 Draw vertically
@ $7843 keep
W $7843,2,2 Set address to (9,5)
B $7845,1,1 Set colour 7 (White)
B $7846,1,1 Plot tile 33
B $7847,1,1 Set colour 13 (Bright Cyan)
B $7848,1,1 Plot tile 33
B $7849,1,1 Set colour 5 (Cyan)
B $784A,1,1 Repeat 6
B $784B,1,1 Plot tile 33
@ $784C keep
W $784C,2,2 Set address to (22,5)
B $784E,1,1 Set colour 7 (White)
B $784F,1,1 Plot tile 38
B $7850,1,1 Set colour 13 (Bright Cyan)
B $7851,1,1 Plot tile 38
B $7852,1,1 Set colour 5 (Cyan)
B $7853,1,1 Repeat 6
B $7854,1,1 Plot tile 38
@ $7855 keep
W $7855,2,2 Set address to (10,5)
B $7857,1,1 Repeat 3
B $7858,1,1 Plot tile 34
@ $7859 keep
W $7859,2,2 Set address to (21,5)
B $785B,1,1 Repeat 3
B $785C,1,1 Plot tile 37
B $785D,1,1 Draw horizontally
@ $785E keep
W $785E,2,2 Set address to (10,13)
B $7860,1,1 Set colour 13 (Bright Cyan)
B $7861,1,1 Plot tile 28
B $7862,1,1 Set colour 14 (Bright Yellow)
B $7863,1,1 Plot tile 29
B $7864,1,1 Repeat 8
B $7865,1,1 Plot tile 30
B $7866,1,1 Plot tile 31
B $7867,1,1 Set colour 13 (Bright Cyan)
B $7868,1,1 Plot tile 32
@ $7869 keep
W $7869,2,2 Set address to (10,12)
B $786B,1,1 Plot tile 39
B $786C,1,1 Plot tile 40
B $786D,1,1 Repeat 8
B $786E,1,1 Plot tile 41
B $786F,1,1 Plot tile 42
B $7870,1,1 Plot tile 43
@ $7871 keep
W $7871,2,2 Set address to (10,13)
B $7873,1,1 Set colour 13 (Bright Cyan)
B $7874,1,1 Repeat 12
B $7875,1,1 Plot tile 19
@ $7876 keep
W $7876,2,2 Set address to (4,8)
B $7878,1,1 Set colour 15 (Bright White)
B $7879,1,1 Plot tile 9
B $787A,1,1 Set colour 13 (Bright Cyan)
B $787B,1,1 Plot tile 10
@ $787C keep
W $787C,2,2 Set address to (4,9)
B $787E,1,1 Set colour 14 (Bright Yellow)
B $787F,1,1 Plot tile 20
B $7880,1,1 Set colour 5 (Cyan)
B $7881,1,1 Plot tile 21
@ $7882 keep
W $7882,2,2 Set address to (4,12)
B $7884,1,1 Set colour 15 (Bright White)
B $7885,1,1 Plot tile 9
B $7886,1,1 Set colour 13 (Bright Cyan)
B $7887,1,1 Plot tile 10
@ $7888 keep
W $7888,2,2 Set address to (4,13)
B $788A,1,1 Set colour 14 (Bright Yellow)
B $788B,1,1 Plot tile 20
B $788C,1,1 Set colour 5 (Cyan)
B $788D,1,1 Plot tile 21
@ $788E keep
W $788E,2,2 Set address to (24,13)
B $7890,1,1 Set colour 15 (Bright White)
B $7891,1,1 Plot tile 9
B $7892,1,1 Set colour 13 (Bright Cyan)
B $7893,1,1 Plot tile 10
@ $7894 keep
W $7894,2,2 Set address to (24,13)
B $7896,1,1 Set colour 14 (Bright Yellow)
B $7897,1,1 Plot tile 20
B $7898,1,1 Set colour 5 (Cyan)
B $7899,1,1 Plot tile 21
@ $789A keep
W $789A,2,2 Set address to (27,13)
B $789C,1,1 Set colour 15 (Bright White)
B $789D,1,1 Plot tile 9
B $789E,1,1 Set colour 13 (Bright Cyan)
B $789F,1,1 Plot tile 10
@ $78A0 keep
W $78A0,2,2 Set address to (27,13)
B $78A2,1,1 Set colour 14 (Bright Yellow)
B $78A3,1,1 Plot tile 20
B $78A4,1,1 Set colour 5 (Cyan)
B $78A5,1,1 Plot tile 21
B $78A6,1,1 Stop
N $78A7 45 tiles used to draw the pre-game screen.
@ $78A7 label=pregame_tiles
B $78A7,8,8 Tile 0 #HTML[#CALL:graphic($78A7,8,8,0,0)]
B $78AF,8,8 Tile 1 #HTML[#CALL:graphic($78AF,8,8,0,0)]
B $78B7,8,8 Tile 2 #HTML[#CALL:graphic($78B7,8,8,0,0)]
B $78BF,8,8 Tile 3 #HTML[#CALL:graphic($78BF,8,8,0,0)]
B $78C7,8,8 Tile 4 #HTML[#CALL:graphic($78C7,8,8,0,0)]
B $78CF,8,8 Tile 5 #HTML[#CALL:graphic($78CF,8,8,0,0)]
B $78D7,8,8 Tile 6 #HTML[#CALL:graphic($78D7,8,8,0,0)]
B $78DF,8,8 Tile 7 #HTML[#CALL:graphic($78DF,8,8,0,0)]
B $78E7,8,8 Tile 8 #HTML[#CALL:graphic($78E7,8,8,0,0)]
B $78EF,8,8 Tile 9 #HTML[#CALL:graphic($78EF,8,8,0,0)]
B $78F7,8,8 Tile 10 #HTML[#CALL:graphic($78F7,8,8,0,0)]
B $78FF,8,8 Tile 11 #HTML[#CALL:graphic($78FF,8,8,0,0)]
B $7907,8,8 Tile 12 #HTML[#CALL:graphic($7907,8,8,0,0)]
B $790F,8,8 Tile 13 #HTML[#CALL:graphic($790F,8,8,0,0)]
B $7917,8,8 Tile 14 #HTML[#CALL:graphic($7917,8,8,0,0)]
B $791F,8,8 Tile 15 #HTML[#CALL:graphic($791F,8,8,0,0)]
B $7927,8,8 Tile 16 #HTML[#CALL:graphic($7927,8,8,0,0)]
B $792F,8,8 Tile 17 #HTML[#CALL:graphic($792F,8,8,0,0)]
B $7937,8,8 Tile 18 #HTML[#CALL:graphic($7937,8,8,0,0)]
B $793F,8,8 Tile 19 #HTML[#CALL:graphic($793F,8,8,0,0)]
B $7947,8,8 Tile 20 #HTML[#CALL:graphic($7947,8,8,0,0)]
B $794F,8,8 Tile 21 #HTML[#CALL:graphic($794F,8,8,0,0)]
B $7957,8,8 Tile 22 #HTML[#CALL:graphic($7957,8,8,0,0)]
B $795F,8,8 Tile 23 #HTML[#CALL:graphic($795F,8,8,0,0)]
B $7967,8,8 Tile 24 #HTML[#CALL:graphic($7967,8,8,0,0)]
B $796F,8,8 Tile 25 #HTML[#CALL:graphic($796F,8,8,0,0)]
B $7977,8,8 Tile 26 #HTML[#CALL:graphic($7977,8,8,0,0)]
B $797F,8,8 Tile 27 #HTML[#CALL:graphic($797F,8,8,0,0)]
B $7987,8,8 Tile 28 #HTML[#CALL:graphic($7987,8,8,0,0)]
B $798F,8,8 Tile 29 #HTML[#CALL:graphic($798F,8,8,0,0)]
B $7997,8,8 Tile 30 #HTML[#CALL:graphic($7997,8,8,0,0)]
B $799F,8,8 Tile 31 #HTML[#CALL:graphic($799F,8,8,0,0)]
B $79A7,8,8 Tile 32 #HTML[#CALL:graphic($79A7,8,8,0,0)]
B $79AF,8,8 Tile 33 #HTML[#CALL:graphic($79AF,8,8,0,0)]
B $79B7,8,8 Tile 34 #HTML[#CALL:graphic($79B7,8,8,0,0)]
B $79BF,8,8 Tile 35 #HTML[#CALL:graphic($79BF,8,8,0,0)]
B $79C7,8,8 Tile 36 #HTML[#CALL:graphic($79C7,8,8,0,0)]
B $79CF,8,8 Tile 37 #HTML[#CALL:graphic($79CF,8,8,0,0)]
B $79D7,8,8 Tile 38 #HTML[#CALL:graphic($79D7,8,8,0,0)]
B $79DF,8,8 Tile 39 #HTML[#CALL:graphic($79DF,8,8,0,0)]
B $79E7,8,8 Tile 40 #HTML[#CALL:graphic($79E7,8,8,0,0)]
B $79EF,8,8 Tile 41 #HTML[#CALL:graphic($79EF,8,8,0,0)]
B $79F7,8,8 Tile 42 #HTML[#CALL:graphic($79F7,8,8,0,0)]
B $79FF,8,8 Tile 43 #HTML[#CALL:graphic($79FF,8,8,0,0)]
B $7A07,8,8 Tile 44 #HTML[#CALL:graphic($7A07,8,8,0,0)]
b $7A0F [Graphics] Smoke and fire graphics
@ $7A0F label=bitmap_smoke1
B $7A0F,52,8*6,4 16x13 pixels, masked #HTML[#CALL:graphic($7A0F,16,13,1,1)]
@ $7A43 label=bitmap_smoke2
B $7A43,44,8*5,4 16x11 pixels, masked #HTML[#CALL:graphic($7A43,16,11,1,1)]
@ $7A6F label=bitmap_smoke3
B $7A6F,36,8*4,4 16x9 pixels, masked #HTML[#CALL:graphic($7A6F,16,9,1,1)]
@ $7A93 label=bitmap_smoke4
B $7A93,14,8,6 8x7 pixels, masked #HTML[#CALL:graphic($7A93,8,7,1,1)]
@ $7AA1 label=bitmap_smoke5
B $7AA1,10,8,2 8x5 pixels, masked #HTML[#CALL:graphic($7AA1,8,5,1,1)]
@ $7AAB label=bitmap_smoke6
B $7AAB,6,6 8x3 pixels, masked #HTML[#CALL:graphic($7AAB,8,3,1,1)]
@ $7AB1 label=bitmap_fire1
B $7AB1,64,4 32x16 pixels #HTML[#CALL:graphic($7AB1,32,16,0,1)]
@ $7AF1 label=bitmap_fire2
B $7AF1,64,4 32x16 pixels #HTML[#CALL:graphic($7AF1,32,16,0,1)]
@ $7B31 label=bitmap_fire3
B $7B31,48,3 24x8 pixels, masked #HTML[#CALL:graphic($7B31,24,8,1,1)]
@ $7B61 label=bitmap_fire4
B $7B61,48,3 24x8 pixels, masked #HTML[#CALL:graphic($7B61,24,8,1,1)]
@ $7B91 label=bitmap_fire5
B $7B91,20,4 16x5 pixels, masked #HTML[#CALL:graphic($7B91,16,5,1,1)]
@ $7BA5 label=bitmap_fire5s
B $7BA5,20,4 16x5 pixels, masked #HTML[#CALL:graphic($7BA5,16,5,1,1)]
@ $7BB9 label=bitmap_fire6
B $7BB9,24,4 16x6 pixels, masked #HTML[#CALL:graphic($7BB9,16,6,1,1)]
@ $7BD1 label=bitmap_fire6s
B $7BD1,24,4 16x6 pixels, masked #HTML[#CALL:graphic($7BD1,16,6,1,1)]
b $7BE9 [Graphics] Faces
N $7BE9 Nancy's face (32x40)
N $7BE9 #HTML[#CALL:face($7BE9)]
@ $7BE9 label=bitmap_nancy
B $7BE9,160,4 Bitmap data, top-down format
@ $7C89 label=attrs_nancy
B $7C89,20,4 Attribute data for above
N $7C9D Raymond's face (32x40)
N $7C9D #HTML[#CALL:face($7C9D)]
@ $7C9D label=bitmap_raymond
B $7C9D,160,4 Bitmap data, top-down format
@ $7D3D label=attrs_raymond
B $7D3D,20,4 Attribute data for above
N $7D51 Tony's face (32x40)
N $7D51 #HTML[#CALL:face($7D51)]
@ $7D51 label=bitmap_tony
B $7D51,160,4 Bitmap data, top-down format
@ $7DF1 label=attrs_tony
B $7DF1,20,4 Attribute data for above
b $7E05 [Graphics] Street lamps etc.
D $7E05 Used for both left and right variations.
@ $7E05 label=stretchy_shortpole
B $7E05,1,1
W $7E06,2,2
B $7E08,1,1
W $7E09,2,2
B $7E0B,1,1 1 => end
@ $7E0C label=shortpole_7e0c
W $7E0C,2,2 -> streetlampbody_lods
B $7E0E,20,2
@ $7E22 label=shortpole_7e22
W $7E22,2,2
B $7E24,20,2
@ $7E38 label=streetlampbody_7e38
W $7E38,2,2
B $7E3A,20,2
@ $7E4E label=streetlampbody_7e4e
W $7E4E,2,2
B $7E50,20,2
@ $7E64 label=streetlampbody_7e64
W $7E64,2,2
B $7E66,20,2
@ $7E7A label=streetlampbody_7e7a
W $7E7A,2,2
B $7E7C,20,2
@ $7E90 label=streetlampbody_7e90
W $7E90,2,2
B $7E92,20,2
@ $7EA6 label=streetlampbody_7ea6
W $7EA6,2,2
B $7EA8,20,2
@ $7EBC label=streetlampbody_lods
B $7EBC,1,1 Width (bytes)
B $7EBD,1,1 Flags
B $7EBE,1,1 Height (pixels)
W $7EBF,2,2 Bitmap
W $7EC1,2,2 Pre-shifted bitmap
B $7EC3,1,1 Width (bytes)
B $7EC4,1,1 Flags
B $7EC5,1,1 Height (pixels)
W $7EC6,2,2 Bitmap
W $7EC8,2,2 Pre-shifted bitmap
B $7ECA,1,1 Width (bytes)
B $7ECB,1,1 Flags
B $7ECC,1,1 Height (pixels)
W $7ECD,2,2 Bitmap
W $7ECF,2,2 Pre-shifted bitmap
B $7ED1,1,1 Width (bytes)
B $7ED2,1,1 Flags
B $7ED3,1,1 Height (pixels)
W $7ED4,2,2 Bitmap
W $7ED6,2,2 Pre-shifted bitmap
B $7ED8,1,1 Width (bytes)
B $7ED9,1,1 Flags
B $7EDA,1,1 Height (pixels)
W $7EDB,2,2 Bitmap
W $7EDD,2,2 Pre-shifted bitmap
B $7EDF,1,1 Width (bytes)
B $7EE0,1,1 Flags
B $7EE1,1,1 Height (pixels)
W $7EE2,2,2 Bitmap
W $7EE4,2,2 Pre-shifted bitmap
B $7EE6,1,1 Width (bytes)
B $7EE7,1,1 Flags
B $7EE8,1,1 Height (pixels)
W $7EE9,2,2 Bitmap
W $7EEB,2,2 Pre-shifted bitmap
B $7EED,1,1 Width (bytes)
B $7EEE,1,1 Flags
B $7EEF,1,1 Height (pixels)
W $7EF0,2,2 Bitmap
W $7EF2,2,2 Pre-shifted bitmap
B $7EF4,1,1 Width (bytes)
B $7EF5,1,1 Flags
B $7EF6,1,1 Height (pixels)
W $7EF7,2,2 Bitmap
W $7EF9,2,2 Pre-shifted bitmap
B $7EFB,1,1 Width (bytes)
B $7EFC,1,1 Flags
B $7EFD,1,1 Height (pixels)
W $7EFE,2,2 Bitmap
W $7F00,2,2 Pre-shifted bitmap
B $7F02,1,1 Width (bytes)
B $7F03,1,1 Flags
B $7F04,1,1 Height (pixels)
W $7F05,2,2 Bitmap
W $7F07,2,2 Pre-shifted bitmap
B $7F09,1,1 Width (bytes)
B $7F0A,1,1 Flags
B $7F0B,1,1 Height (pixels)
W $7F0C,2,2 Bitmap
W $7F0E,2,2 Pre-shifted bitmap
B $7F10,1,1 Width (bytes)
B $7F11,1,1 Flags
B $7F12,1,1 Height (pixels)
W $7F13,2,2 Bitmap
W $7F15,2,2 Pre-shifted bitmap
B $7F17,1,1 Width (bytes)
B $7F18,1,1 Flags
B $7F19,1,1 Height (pixels)
W $7F1A,2,2 Bitmap
W $7F1C,2,2 Pre-shifted bitmap
B $7F1E,1,1 Width (bytes)
B $7F1F,1,1 Flags
B $7F20,1,1 Height (pixels)
W $7F21,2,2 Bitmap
W $7F23,2,2 Pre-shifted bitmap
N $7F25 Bottom two rows of largest scale lamppost.
@ $7F25 label=bitmap_streetlampbody_1
B $7F25,4,4 Masked bitmap data
N $7F29 Bottom repeating section of largest scale lamppost.
@ $7F29 label=bitmap_streetlampbody_2
B $7F29,4,4 Masked bitmap data
N $7F2D Upper repeating section of largest scale lamppost.
@ $7F2D label=bitmap_streetlampbody_3
B $7F2D,4,4 Masked bitmap data
@ $7F31 label=bitmap_streetlampbody_4
B $7F31,4,4 Masked bitmap data
@ $7F35 label=bitmap_streetlampbody_5
B $7F35,4,4 Masked bitmap data
@ $7F39 label=bitmap_streetlampbody_6
B $7F39,4,4 Masked bitmap data
@ $7F3D label=bitmap_streetlampbody_7
B $7F3D,2,2 Masked bitmap data
@ $7F3F label=bitmap_streetlampbody_8
B $7F3F,4,4 Masked bitmap data
@ $7F43 label=bitmap_streetlampbody_9
B $7F43,4,4 Masked bitmap data
@ $7F47 label=bitmap_streetlampbody_7s
B $7F47,2,2 Masked bitmap data
@ $7F49 label=bitmap_streetlampbody_8s
B $7F49,4,4 Masked bitmap data
@ $7F4D label=bitmap_streetlampbody_9s
B $7F4D,4,4 Masked bitmap data
@ $7F51 label=bitmap_streetlampbody_10
B $7F51,2,2 Masked bitmap data
@ $7F53 label=bitmap_streetlampbody_11
B $7F53,4,4 Masked bitmap data
@ $7F57 label=bitmap_streetlampbody_12
B $7F57,4,4 Masked bitmap data
@ $7F5B label=bitmap_streetlampbody_10s
B $7F5B,2,2 Masked bitmap data
@ $7F5D label=bitmap_streetlampbody_11s
B $7F5D,4,4 Masked bitmap data
@ $7F61 label=bitmap_streetlampbody_12s
B $7F61,4,4 Masked bitmap data
@ $7F65 label=bitmap_streetlampbody_13
B $7F65,4,4 Masked bitmap data
@ $7F69 label=bitmap_streetlampbody_14
B $7F69,8,8 Masked bitmap data
@ $7F71 label=bitmap_streetlampbody_15
B $7F71,8,8 Masked bitmap data
@ $7F79 label=bitmap_streetlampbody_13s
B $7F79,4,4 Masked bitmap data
@ $7F7D label=bitmap_streetlampbody_14s
B $7F7D,8,8 Masked bitmap data
@ $7F85 label=bitmap_streetlampbody_15s
B $7F85,8,8 Masked bitmap data
u $7F8D Unused
B $7F8D,115,8*14,3
g $8000 Game status buffer entry at 8000
@ $8000 label=test_mode_flag
B $8000,1,1 Test mode enable flag (cheat mode)
@ $8001 label=attract_cycle
B $8001,1,1 [128K] Attract mode message cycle. Used by #R$F437. When zero shows the "ENTER FOR OPTIONS" message.
@ $8002 label=score_bcd
B $8002,4,4 Score digits as BCD (4 bytes / 8 digits, little endian)
@ $8006 label=retry_count
B $8006,1,1 0 for first attempt, 1 if second, 2 if final. [Used for bonuses only?]
@ $8007 label=wanted_stage_number
B $8007,1,1 Stage number we're loading (1..5 or 6 for the end screen)
u $8008 Unused
B $8008,8,8 Can't see any consistent use of these.
B $8010,4,4
c $8014 Load a stage
D $8014 Used by the routine at #R$8401.
@ $8014 label=load_stage
C $8014,3 Load wanted_stage_number
C $8017,3 Point #REGhl at current_stage_number
C $801A,2 Exit if they match
C $801C,1 current_stage_number = wanted_stage_number
C $801D,3 Set dont_draw_screen_attrs to a non-zero value
@ $8022 ssub=LD (searching_for_n + 14),A
C $8020,5 Update the level number in "SEARCHING FOR <N>"
@ $8025 label=ls_8025
C $8025,3 Call clear_screen_set_attrs
C $8028,3 Call clear_game_attrs
C $802B,2 Reverse transition
C $802D,3 Call setup_transition
@ $8032 label=ls_8032
C $8032,3 Call ls_8088
@ $8035 label=ls_load_header
C $8035,4 Load to address &hazards[1] as a temporary scratch area
C $8039,3 Load 2 bytes
C $803C,3 Call tape_load_at_ix subroutine
C $803F,2 Loop to ls_load_header while failed
C $8041,3 #REGhl = &hazards[1]
@ $8045 refs=:$E810
C $8044,3 Are the two loaded bytes equal? [seems odd]
C $8047,2 Loop to ls_load_header if not
C $8049,2 Make it ASCII and terminate it by adding 48+128
@ $804B ssub=LD (found_n + 6),A
C $804B,3 Set x in "FOUND x"
C $8050,3 #REGa = wanted_stage_number
C $8053,1 equal?
C $8054,2 not the wanted stage
C $8056,3 Call ls_8088
C $8059,3 Call tape_load_to_5c00
C $805C,2 Jump to ls_8025 if no carry
N $805E Success - must have loaded the correct level data.
C $805E,3 Set border to black
C $8061,3 Call clear_screen_set_attrs
C $8064,3 Call clear_game_attrs
C $8067,3 HL -> "STOP THE TAPE" message structure
C $806C,3 Call ls_8098
C $806F,1 Preserve ?
N $8070 Wait for a keypress - debounce.
C $8070,3 Call keyscan
C $8073,4 Loop while key not pressed
C $8077,3 Call keyscan
C $807A,4 Loop while key pressed
C $807E,2 Forward transition
C $8080,3 Call setup_transition
C $8083,1 Restore ?
C $8084,2 B = 2
C $8086,2 Exit via ls_8098
N $8088 #REGb is passed in.
@ $8088 label=ls_8088
C $8088,3 Point #REGhl at "START TAPE" message structure
C $808B,4 A = wanted_stage_number - 1
C $808F,2 Jump if > 0
@ $8091 refs=:$E810,$F220
C $8092,3 Point #REGhl at tape_messsages
@ $8095 label=ls_8095
C $8095,3 why call adjacent instr? to exec this func twice?
@ $8098 label=ls_8098
C $8098,1 preserve counter
C $8099,1 preserve message pointer
@ $809A label=ls_809A
C $809A,2 flags byte for print_message
C $809C,1 why dec here?
N $809D This entry point is used by the routine at #R$F220.
@ $809D label=ls_809D
C $809D,3 Call print_message
C $80A0,2 printing a whole set by looping?
C $80A2,3 Call transition
C $80A5,3 Call draw_screen
C $80A8,1 restore message pointer
C $80A9,1 restore counter
N $80AA This entry point is used by the routine at #R$F220.
C $80AA,3 Load transition_control
C $80AD,2 Return if zero
C $80AF,8 Delay loop
C $80B7,2 Loop
c $80B9 Tape loading
D $80B9 Used by the routine at #R$8014.
R $80B9 O:F Carry set on return likely signals success.
@ $80B9 label=tape_load_to_5c00
C $80B9,7 Setup to load a block at $5C00 of length $1AF0
N $80C0 This entry point is used by the routine at #R$5B00. IX = address DE = bytes
@ $80C0 label=tape_load_at_ix
C $80C0,1 D++
C $80C1,2 A = 152
C $80C3,1 Set carry flag
C $80C4,1 bank
C $80C5,1 D--  -- weird inc then dec
C $80C6,1 Disable interrupts
C $80C7,2 Set MIC bit (deactivates MIC)
C $80C9,2 output
C $80CB,2 input
C $80CD,1 why shift?
C $80CE,2 Check EAR input bit 6
C $80D0,2 set border?
C $80D3,1 Set Z? [weird]
@ $80D4 label=tl_80d4
@ $80D5 label=tl_80d5
C $80D5,3 Call tl_delay_814b
C $80DA,3 Delay time
@ $80DD label=tl_delay
C $80DD,2 Delay for B iterations -- is B not initialised?
C $80DF,1 HL--
C $80E0,4 Loop while HL > 0
C $80E4,3 Call tl_8147
@ $80E9 label=tl_80e9
C $80E9,2 B = 156
C $80EB,3 Call tl_8147
@ $80EE refs=:$E810
@ $80EE label=tl_80ee
C $80F0,2 A = 198
@ $80F8 label=tl_80f8
C $80F8,2 B = 201
C $80FA,3 Call tl_delay_814b
C $8100,2 212
C $8104,3 Call tl_delay_814b
C $8108,4 A = C ^= 3
C $810E,2 B = 176
@ $8112 label=tl_8112
C $8113,2 wuuuut?
@ $8117 label=tl_8117
@ $811C label=tl_811c
C $811E,1 A ^= L
C $8120,3 RR C and copy to A?
C $8123,1 DE++
@ $8126 label=tl_8126
@ $8128 label=tl_8128
C $812A,2 B = 178
@ $812C label=tl_812c
@ $812E label=tl_812e
C $812E,3 Call tl_8147
C $8132,2 A = 200
C $8137,2 B = 176
C $813C,2 A = H ^ L
C $813E,1 Save A
C $813F,4 Loop while DE > 0
C $8143,1 Restore A
C $8144,2 Is it 1?
C $8146,1 Return (with flags?)
@ $8147 label=tl_8147
C $8147,3 Call tl_delay_814b
@ $814B label=tl_delay_814b
@ $814D label=tl_delay_loop
C $814B,5 Delay loop of 22 iterations
C $8150,1 Clear carry flag
@ $8151 label=tl_8151
C $8151,1 what's in B?
C $8152,1 Return if zero (failure)
C $8153,2 %01111111
C $8155,2 Read from port
C $8159,1 A ^= C
C $815A,2 check EAR input bit 6
C $815C,2 loop if zero
C $815E,3 A = C = ~C
C $8163,2 Set MIC bit (deactivate MIC)
C $8165,2 Output
C $8167,1 Set carry
C $8168,1 Return (success)
@ $8169 label=rewind_tape
B $8169,1,1 attr?
W $816A,2,2 Screen position (8,64)
W $816C,2,2 Screen attribute position (1,8)
T $816E,30,29:n1 "REWIND TAPE TO START OF SIDE 2"
@ $818C label=start_tape
B $818C,1,1 attr?
W $818D,2,2 Screen position (88,96)
W $818F,2,2 Screen attribute position (11,12)
T $8191,10,9:n1 "START TAPE"
B $819B,1,1 attr?
W $819C,2,2 Screen position (72,128)
W $819E,2,2 Screen attribute position (9,16)
@ $81A0 label=searching_for_n
T $81A0,15,14:n1 "SEARCHING FOR 1"
B $81AF,1,1 attr?
W $81B0,2,2 Screen position (104,160)
W $81B2,2,2 Screen attribute position (13,20)
@ $81B4 label=found_n
T $81B4,7,6:n1 "FOUND 1"
@ $81BB label=stop_the_tape_press_gear
B $81BB,1,1 attr?
W $81BC,2,2 Screen position (80,96)
W $81BE,2,2 Screen attribute position (10,12)
T $81C0,13,12:n1 "STOP THE TAPE"
B $81CD,1,1 attr?
W $81CE,2,2 Screen position (88,128)
W $81D0,2,2 Screen attribute position (11,16)
T $81D2,11,10:n1 "PRESS  GEAR"
b $81DD [Messages] Start of stage chatter
@ $81DD label=start_stage_chatter
B $81DD,1,1 Three-way random choice ($FC)
W $81DE,2,2 -> Tony: "GIDDY UP BOY!" <STOP>
W $81E0,2,2 -> Tony: "HOLD ON MAN" <STOP>
W $81E2,2,2 -> Tony: "LET'S GO. MR. DRIVER." <STOP>
@ $81E4 label=giddy_up_boy
B $81E4,1,1 Tony ($03)
W $81E5,2,2 -> "GIDDY UP BOY!"
B $81E7,1,1 <STOP>
@ $81E8 label=hold_on_man
B $81E8,1,1 Tony ($03)
W $81E9,2,2 -> "HOLD ON MAN"
B $81EB,1,1 <STOP>
@ $81EC label=string_giddy_up_boy
T $81EC,13,12:n1 "GIDDY UP BOY!"
@ $81F9 label=string_hold_on_man
T $81F9,11,10:n1 "HOLD ON MAN"
c $8204 Generates engine noise
D $8204 Used by the routine at #R$83B5.
@ $8204 label=setup_engine_sfx
C $8204,3 Load speed into #REGhl
C $8207,2 Bottom bit of #REGh moves to carry (#REGh now unused)
N $8209 This entry point is used by the routine at #R$F220.
C $8209,1 Speed low byte
C $820A,1 Halve speed, shifting carry in as MSB
C $820B,1 A = ~A
C $820C,4 Divide by four
C $8210,2 A |= 1
C $8212,3 #REGhl = 0x0300 | #REGa
C $8215,6 Jump if in high gear
C $821B,2 L >>= 1
C $821D,6 Jump if tunnel_sfx == 0
C $8223,2 H = 1
C $8225,4 Self modify iterations -- L
C $8229,4 Self modify delay loop -- H
C $822D,7 Self modify delay loop -- (5 - A)
N $8234 This entry point is used by the routine at #R$83B5.
@ $8234 label=play_engine_sfx
C $8234,6 Return if perp_caught_phase is >= 3 (when car stops)
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
c $8258 Attract mode
D $8258 Used by the routine at #R$83B5.
@ $8258 label=attract_mode
C $8258,3 #REGhl -> attract_data
C $825B,3 Call set_up_stage
C $825E,4 Reset credits/copyright message blinker to show credits
C $8262,6 Set speed to $190
C $8268,3 Call keyscan
C $826B,3 Return if fire was pressed
C $826E,3 Call cpu_driver
N $8271 This entry point is used by the routine at #R$F220.
C $8271,3 #REGhl -> attract mode messages "CHASE HQ" "PRESS GEAR TO PLAY" ...
C $8274,2 Number of messages?
N $8276 Blink "PRESS GEAR TO PLAY".
C $8276,2 A = <initally $F0, self modified>
C $8278,1 Rotate
C $8279,3 Self modify above
C $827C,2 If carry set the hide the final message
C $827E,1 Otherwise include it
N $827F Display 'B' messages.
C $827F,1 Load flags
C $8280,3 Call print_message
C $8283,2 Loop while #REGb > 0
C $8285,6 Jump if transition_control > 0
N $828B Alternate between credits and copyright messages.
C $828B,7 Blinking on/off self modified pattern
C $8292,3 #REGhl -> credits_messages
C $8295,2 Skip next LD HL based on blink above
C $8297,3 #REGhl -> copyright_messages
C $829A,3 Call setup_overlay_messages
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
B $837A,2,1
u $837C Unused
S $837C,57,$39
c $83B5 Indirect calls for 48K/128K features
D $83B5 The main difference is in sound, but attract mode also has an alternate implementation in 128K mode.
N $83B5 Used by the routine at #R$B4CC.
@ $83B5 label=start_siren_hook
C $83B5,3 No-op when in 48K mode -- likely siren
N $83B8 This entry point is used by the routines at #R$8401 and #R$8903.
@ $83B8 label=play_engine_sfx_hook
C $83B8,3 Call play_engine_sfx when in 48K mode
N $83BB This entry point is used by the routines at #R$8401, #R$873C, #R$87DC, #R$8876, #R$8A57 and #R$F220.
@ $83BB label=silence_audio_hook
C $83BB,3 No-op when in 48K mode
N $83BE This entry point is used by the routine at #R$8903.
@ $83BE label=write_registers_hook
C $83BE,3 No-op when in 48K mode
N $83C1 This entry point is used by the routine at #R$8876.
@ $83C1 label=play_turbo_sfx_hook
C $83C1,3 No-op when in 48K mode
N $83C4 This entry point is used by the routine at #R$8903.
@ $83C4 label=setup_engine_sfx_hook
C $83C4,3 Call setup_engine_sfx when in 48K mode
N $83C7 This entry point is used by the routines at #R$8401 and #R$9BCF.
@ $83C7 label=play_speech_hook
C $83C7,3 No-op when in 48K mode
N $83CA This entry point is used by the routine at #R$83CD.
@ $83CA label=attract_mode_hook
C $83CA,3 Call attract_mode when in 48K mode
c $83CD Bootstrap / Über main loop
D $83CD This is the true start and main loop of the game. It's called once the memory map is set up. It builds a table of 256 flipped bytes at $EF00 then starts attract mode. When the player causes attract mode to yield, the overtaking bonus, the score and the retry count are reset as are the wanted stage number and the number of credits. The main loop is then called to run the game. Then when the main loop yields it will start the 128K animated title screen, or just loop if we're running on a 48K machine.
D $83CD Used by the routine at #R$E839.
N $83CD Build a table of flipped bytes at $EF00.
@ $83CD label=bootstrap
C $83CD,3 Point #REGhl at $EF00
@ $83D0 label=bs_flip_table_loop
C $83D0,2 8 iterations
C $83D2,1 #REGa is the index into the table
@ $83D3 label=bs_flip_byte_loop
C $83D3,3 Shift a bit out of #REGa leftwards and into #REGc rightwards
C $83D6,2 Loop for 8 bits
C $83D8,2 Write #REGc out and advance
C $83DA,2 Loop for 256 iterations (until #REGl overflows)
N $83DC Start attract mode.
C $83DC,3 Call attract_mode_hook
N $83DF When attract mode yields then we set up the game.
C $83DF,4 Reset overtake_bonus
N $83E3 Clear score_bcd and retry_count.
@ $83E8 label=bs_clear_loop
N $83EC Reset wanted_stage_number and credits.
C $83EC,2 wanted_stage_number = 1
C $83EE,4 credits = 2
C $83F2,3 Call the main loop
C $83F5,10 Call relocated plsp_f3b6_128k (animated title screen?) if in 128K mode
C $83FF,2 Loop
c $8401 Main loop
D $8401 This is the "main loop" because it does all of the driving of the primary game functions, but it's really a subroutine of the uber main loop above.
R $8401 Used by the routines at #R$83CD and #R$8A57.
@ $8401 label=main_loop
C $8401,3 Call load_stage
C $8404,7 Jump to #R$841B if we're not on the end screen (stage 6)
N $840B Run the end screen.
@ $840B label=ml_run_end_screen
C $840B,3 Call the end screen animation routine
C $840E,5 Reset (wanted) stage to 1
C $8413,5 Call load_stage
C $8418,2 Set (wanted) stage to 6   [not sure why]
C $841A,1 Return
N $841B Run the main game loop.
@ $841B label=ml_not_credits
C $841B,3 Call run_pregame_screen
C $841E,3 Address of stage_set_up_data
C $8421,3 Call set_up_stage
@ $842D label=ml_store_start_speech
C $8424,10 Cycle start_speech_cycle 3,2,1 then repeat
N $842E Choose the startup speech sample.
C $842E,7 start_speech = (#REGa << 2) OR 2  -- The number of leading zeroes are used to encode a delay
C $8435,4 Test 128K mode flag
C $8439,5 Set hazards[0].used to $FF  -- I suspect this keeps the perp spawned
C $843E,3 Address of start_stage_chatter
C $8441,3 Call start_chatter if not in 128K mode (priority $FF)
@ $8444 label=ml_loop
C $8444,3 Call drive_sfx
C $8447,3 Call keyscan
C $844A,3 Call tick
C $844D,3 Call check_user_input
C $8450,3 Call read_map
C $8453,3 Call handle_perp_caught
C $8456,3 Call move_hero_car
C $8459,3 Call spawn_cars
C $845C,3 Call cycle_counters
C $845F,3 Call play_engine_sfx_hook
C $8462,3 Call build_height_table
C $8465,3 Call scroll_horizon
C $8468,3 Call play_engine_sfx_hook
C $846B,3 Call layout_road
C $846E,3 Call play_engine_sfx_hook
C $8471,3 Call draw_road
C $8474,3 Call play_engine_sfx_hook
C $8477,3 Call layout_objects
C $847A,3 Call prepare_tunnel
C $847D,3 Call spawn_barriers
C $8480,3 Call drive_helicopter
C $8483,3 Call choose_dirt_and_stones
C $8486,3 Call play_engine_sfx_hook
C $8489,3 Call draw_hazards
C $848C,3 Call layout_dirt_and_stones
C $848F,3 Call play_engine_sfx_hook
C $8492,3 Call move_helicopter
C $8495,3 Call check_collisions
C $8498,3 Call play_engine_sfx_hook
C $849B,3 Call draw_everything_else
C $849E,3 Call play_engine_sfx_hook
C $84A1,3 Call animate_hero_car
C $84A4,3 Call speed_score
C $84A7,3 Call update_scoreboard
C $84AA,3 Call calc_overtake_bonus
C $84AD,3 Call play_engine_sfx_hook
C $84B0,3 Call drive_chatter
C $84B3,3 Call smash_bar_etc
C $84B6,3 Call transition
C $84B9,3 Call play_engine_sfx_hook
C $84BC,3 Call draw_screen
C $84BF,3 Call exit_fork
C $84C2,3 Is test mode enabled?
C $84C5,3 If not, goto not_test_mode
N $84C8 Test mode handling
@ $84C8 label=ml_handle_test_mode
C $84C8,4 Read keys 1/2/3/4/5
C $84CC,1 Change to active high
C $84CD,2 Mask off just keys
C $84CF,2 If none are set then jump
C $84D1,1 Bank key flags
C $84D2,3 Effect 8 (bip), Priority 4
C $84D5,3 Call start_sfx
C $84D8,3 Call silence_audio_hook
C $84DB,1 Unbank key flags
C $84DC,1 Is bit 0 set? (key 1 to restart the level)
C $84DD,3 Restart level if so
C $84E0,3 HL = &wanted_stage_number
C $84E3,1 Is bit 1 set? (key 2 to load the next level)
C $84E4,2 Jump if NOT
C $84E6,1 wanted_stage_number++
C $84E7,3 Loop to main_loop
@ $84EA label=ml_check_for_3
C $84EA,1 Is bit 2 set? (key 3 to load the end screen)
C $84EB,2 Jump if NOT
C $84ED,2 wanted_stage_number = 6
C $84EF,3 Loop to main_loop
@ $84F2 label=ml_increment_credits
C $84F2,9 Increment credits unless maxed out at 9
@ $84FB label=ml_not_test_mode
C $84FB,3 Load transition_control
C $84FE,4 Jump to ml_loop if non-zero
N $8502 Play speech when we see a 1-bit shift out of start_speech.
C $8502,3 Address of start_speech
C $8505,2 Shift the counter right
C $8507,2 Jump if no carry
C $8509,1 Load speech sample index
C $850A,2 Zero start_speech
C $850C,3 Call play_speech_hook
C $850F,3 Loop to ml_loop
@ $8512 label=ml_check_quit
C $8512,7 Loop to ml_loop if quit_state is zero
N $8519 Quitting the game is in progress.
C $8519,1 Decrement quit_state
C $851A,3 Jump to escape_scene if non-zero
C $851D,5 quit_state = 2
C $8522,2 Forward transition
C $8524,3 Call setup_transition
C $8527,3 Loop to ml_loop
c $852A CPU driver
D $852A This runs the game loop while driving the car.
@ $852A label=cpu_driver
C $852A,3 Get road position
C $852D,7 Subtract centre value. Carry flag will be set if we're on the right hand side of the road
C $8534,2 Set input ACCELERATE + RIGHT
C $8536,2 Jump to cd_check_speed if we're on the left
C $8538,5 Subtract "ideal road position"? value. Carry will be set if we're on the right of it.
C $853D,2 Set input ACCELERATE + LEFT
C $853F,2 Jump to cd_check_speed if we're on the right
C $8541,2 Set input ACCELERATE
@ $8543 label=cd_check_speed
C $8543,1 Move input into #REGc for the moment
C $8544,8 Calculate (speed - 150)
C $854C,3 Load gear
C $854F,2 Set gear to low if speed < 150, otherwise high
C $8553,2 Set input GEAR
@ $8555 label=cd_set_input
C $8555,4 Set user input
C $8559,3 Call read_map
C $855C,3 Call spawn_cars
C $855F,3 Call cycle_counters
C $8562,3 Call build_height_table
C $8565,3 Call scroll_horizon
C $8568,3 Call layout_road
C $856B,3 Call draw_road
C $856E,3 Call layout_objects
C $8571,3 Call prepare_tunnel
C $8574,3 Call spawn_barriers
C $8577,3 Call choose_dirt_and_stones
C $857A,3 Call layout_dirt_and_stones
C $857D,3 Call draw_hazards
C $8580,3 Call move_hero_car
C $8583,3 Call check_collisions
C $8586,3 Call draw_everything_else
C $8589,3 Exit via animate_hero_car
c $858C Pre-game radio screen ("CHASE HQ MONITORING SYSTEM")
D $858C Used by the routine at #R$8401.
@ $858C label=run_pregame_screen
C $858C,3 Address of stage_set_up_data
C $858F,3 Call set_up_stage
C $8592,2 Reverse transition
C $8594,3 Set dont_draw_screen_attrs to a non-zero value
C $8597,3 Call setup_transition
C $859A,3 Call clear_screen_set_attrs
C $859D,4 Reset the counter in #R$85E4 that reveals the perp's car
C $85A1,1 Set chatter priority to $FF
C $85A2,3 Load Nancy's report for this level
C $85A5,3 Call start_chatter (priority $FF)
@ $85A8 label=rps_loop
C $85A8,3 Call draw_pregame
C $85AB,3 Call drive_chatter
C $85AE,3 Call reveal_perp_car
C $85B1,3 Call animate_meters
C $85B4,3 Call transition
C $85B7,3 Call draw_screen
C $85BA,6 Loop to rps_loop while transition_control is non-zero
N $85C0 transition_control is now zero.
C $85C0,5 Return if chatter_state is idle (zero)
C $85C5,4 Jump to rps_start_game if chatter_state is stopping (>= 3)
C $85C9,3 Call keyscan
C $85CC,4 Loop to rps_start_game unless fire was hit
N $85D0 Fire was hit.
C $85D0,3 Call drive_chatter_stop
C $85D3,3 Exit via play_start_noise (will RET for us)
@ $85D6 label=rps_start_game
C $85D6,2 Forward transition
N $85D8 We can only arrive here if no carry... so why the conditional CALL?
C $85D8,3 If no carry (A was >= 3) call setup_transition
C $85DB,2 Loop to rps_loop
N $85DD Since nothing can arrive here this can only be dead code.
@ $85DD label=rps_dead_code
C $85DD,6 Loop to rps_loop if transition_control is non-zero
C $85E3,1 Return
c $85E4 Reveals the perp's car on the pre-game screen
D $85E4 Used by the routine at #R$858C.
@ $85E4 label=reveal_perp_car
C $85E4,6 Return if wanted_stage_number is 5 (the perp's car is hidden on the pre-game screen for that stage)
N $85EA The perp's car is revealed from the bottom-up.
C $85EA,2 Load height counter (self modified)
C $85EC,1 Increment it
C $85ED,5 Max height is 50
@ $85F2 label=rpc_height_max_ok
C $85F2,3 Update height counter
C $85F5,3 Load address of perp's car LOD
C $85F8,3 Read lod.width_bytes
C $85FB,3 Read lod.height
C $85FE,4 If height counter < lod.height #REGb = height counter
@ $8602 label=rpc_height_lod_ok
C $8602,5 Load address of bitmap data
C $8607,1 Stride of bitmap data in bytes (#REGd is still zero)
C $8608,1 Bank for plot_sprite entry
C $8609,3 Address in back buffer to plot at
C $860C,3 Exit via plot_sprite
c $860F Animate the signal meters
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
C $8623,3 Self modify counter in #R$8613
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
C $8640,3 Self modify counter in #R$8630
C $8643,3 Screen attribute (23,18)
N $8646 The signal meter bar is seven segments wide. Fill the left hand portion with our chosen amount of green segments and the right hand side with the complement of red segments.
@ $8646 label=am_set_attrs
C $8646,1 Set flags from counter in #REGa
C $8647,2 Jump if zero
C $8649,1 Copy counter to loop counter
C $864A,3 Set attribute byte to black ink over green
C $864D,2 Loop while #REGb > 0
@ $864F label=am_set_right_attrs
C $864F,3 A = 7 - A
C $8652,1 Return if zero
C $8653,1 Copy counter to loop counter
C $8654,3 Set attribute byte to black ink over red
C $8657,2 Loop while #REGb > 0
C $8659,1 Return
c $865A Draw the pre-game screen
D $865A The pre-game screen ("CHASE H.Q. MONITORING SYSTEM") is drawn by following the commands stored at #R$77D8. Possible commands are "plot tile" (single or up to 30 repetitions), "set background colour", "set address", "draw horizontally", "draw vertically" or "stop". The tiles are the 45 8x8 pixel tiles that live at #R$78A7.
D $865A Once the tiles are drawn the strings at #R$7798 are drawn by calling #R$8E6C.
D $865A Used by the routine at #R$858C.
@ $865A label=draw_pregame
C $865A,3 Address of pre-game screen data
@ $865D label=dp_get_command
C $865D,1 Read a command byte
C $865E,1 Set flags
C $865F,3 Zero is the "Stop" command so jump to rendering messages
C $8662,1 Advance to next command byte
C $8663,4 Jump if < $D0 (either "Repeat" or "Plot tile")
C $8667,4 Jump if < $D0..$DF ("Set background colour")
C $866B,4 Jump if >= $F0 ("Set address")
N $866F $E1 is "Draw horizontally". $E2 is "Draw vertically".
@ $866F label=dp_set_direction
C $866F,5 Self modify #R$86C8 with (#REGa - $E0)
C $8674,2 Loop dp_get_command
N $8676 $D0..$DF is "Set background colour".
@ $8676 label=dp_set_bg_colour
C $8676,8 Self modify #R$86C2 with (#REGa - $D0) << 3
C $867E,2 Loop dp_get_command
N $8680 $F0..$FF is "Set address".
@ $8680 label=dp_set_address
C $8680,3 Load new backbuffer address into #REGde
C $8683,2 Loop dp_get_command
N $8685 $00..$CF could be either "Repeat" or "Plot tile".
@ $8685 label=dp_repeat_or_plot_tile
C $8685,2 Command bytes >= $1F emit single tiles
C $8687,2 Set count to 1 irrespective
C $8689,2 Jump if so
N $868B Plot multiple tiles.
C $868B,1 Command bytes <= $1E emit repeated tiles
C $868C,1 Read tile index
C $868D,1 Advance to next command
@ $868E label=dp_do_plot
C $868E,1 Preserve command pointer
C $868F,2 Turn command byte into a tile index
C $8691,3 Multiply by 8 - size of tile. Carry flag is updated
C $8694,1 Preserve count
C $8695,3 Expand tile offset into #REGbc
C $8698,2 Add in carry flag
C $869A,4 HL = &pregame_tiles[A]
C $869E,1 Restore count
C $869F,1 #REGde becomes tile ptr, #REGhl becomes backbuffer ptr
@ $86A0 label=dp_line_loop
C $86A0,1 Count
C $86A1,1 Tile ptr
C $86A2,1 Backbuffer ptr
N $86A3 Plot a tile.
C $86A3,2 8 rows per tile
@ $86A5 label=dp_tile_loop
C $86A5,2 Copy a row of tile pixels to the screen
C $86A7,1 Move to next tile row
C $86A8,1 Move to next scanline
C $86A9,2 Loop dp_tile_loop
C $86AB,1 Pop backbuffer ptr
N $86AC Seems to be building an attribute address?
C $86AC,6 Get bit 3 of Y
C $86B2,5 Add to X bits
C $86B7,4 Get bits 4..6 of Y. Bit 6 goes to carry
C $86BB,4 Select second third of attributes
C $86BF,1 Otherwise select final third of attributes
@ $86C0 label=dp_attrs
C $86C0,2 #REGde is now the attribute byte address
C $86C2,2 Load <self modified> attribute value, as set by the $Dx commands
C $86C4,4 Set attribute byte if non-zero
@ $86C8 label=dp_direction
C $86C8,3 Load <self modified - 1> direction value, as set by the $Ex commands
C $86CB,2 Jump if horizontal
@ $86CD label=dp_direction_vertical
C $86CD,3 Get bits 0..3 of Y
C $86D0,2 Jump to dp_nextone if non-zero
N $86D2 #REGh is incremented by 1 earlier. So if it was 0b11111111 it's now 0b00000000 so make it 0b11110000. Does this happen in any other case?
C $86D2,4 H -= 16
C $86D6,4 Increment bits 4..6 of Y (visually downwards by 16?)
C $86DA,2 Jump to dp_nextone
@ $86DC label=dp_direction_horizontal
C $86DC,3 Move one character to the right
@ $86DF label=dp_nextone
C $86DF,1 Restore tile ptr
C $86E0,1 Restore count
C $86E1,2 Loop dp_line_loop while #REGb > 0
C $86E3,1 #REGde = Back buffer ptr
C $86E4,1 Restore command pointer
C $86E5,3 Loop dp_get_command
@ $86E8 label=dp_print_strings
C $86E8,2 Four messages to print
C $86EA,3 Address of pre_game_messages
@ $86ED label=dp_print_string
C $86ED,2 Flags TBD
C $86EF,1 HL--
C $86F0,3 Call print_message
C $86F3,2 Loop to dp_print_string while #REGb > 0
C $86F5,1 Return
c $86F6 Routine at 86F6
D $86F6 18 byte chunks of this 36-byte table get copied to #R$C82D by #R$C808. $2C = INC, $00 = NOP, $ED+$A0 = LDI.
@ $86F6 label=backdrop_shift_instrs
b $871A Escape scene setup data
D $871A 14 bytes of set_up_stage data used by escape_scene. Map data addresses are all a byte earlier than their actual start data.
@ $871A label=escape_scene_data
W $871A,2,2 road_pos
@ $871C ssub=DEFW perp_escape_curvature - 1
W $871C,2,2 road_curvature_ptr
@ $871E ssub=DEFW perp_escape_height - 1
W $871E,2,2 road_height_ptr
@ $8720 ssub=DEFW perp_escape_lanes - 1
W $8720,2,2 road_lanes_ptr
@ $8722 ssub=DEFW perp_escape_hazards + 5
W $8722,2,2 road_rightside_ptr (note: reuse of road curvature data)
@ $8724 ssub=DEFW perp_escape_hazards + 5
W $8724,2,2 road_leftside_ptr (note: reuse of road curvature data)
@ $8726 ssub=DEFW perp_escape_hazards - 1
W $8726,2,2 road_hazard_ptr
N $8728 20 bytes copied to hazards[0]
@ $8728 label=escape_scene_hazards
B $8728,1,1 Used flag
B $8729,8,8
W $8731,2,2
W $8733,2,2 Routine at #R$ADF9 (it's just a RET)
W $8735,2,2
B $8737,5,5
c $873C Escape scene
D $873C Used by the routine at #R$8401.
@ $873C label=escape_scene
C $873C,3 Call silence_audio_hook
C $873F,3 Address of escape_scene_data
C $8742,3 Call set_up_stage
C $8745,6 Set speed to $FA [speed of the camera]
C $874B,11 Initialise hazards[0] (the perp)
@ $8759 ssub=LD (hazard_0 + 1),HL
C $8756,6 Set $A191 to the perp's car LOD
C $875C,5 inhibit_collision_detection = $FF  -- Stops #R$AD0D from running
C $8761,3 Address of failed_chatter ("wrong job" / "one more try" / "mediocre driver")
C $8764,3 Call start_chatter (priority $FF)
N $8767 Print "GAME OVER" once the transition has completed.
@ $8767 label=es_loop
C $8767,3 Address of game_over_message
C $876A,8 If transition_control != 4 Call setup_overlay_messages
C $8772,3 Call read_map
C $8775,3 Call build_height_table
C $8778,3 Call scroll_horizon
C $877B,3 Call layout_road
C $877E,3 Call draw_road
C $8781,3 Call layout_objects
C $8784,3 Call prepare_tunnel
C $8787,3 Call spawn_barriers
C $878A,3 Call draw_hazards
C $878D,3 Call draw_everything_else
C $8790,3 Call update_scoreboard
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
C $87C7,6 Loop while chatter_state > 0 => chatter is still happening
C $87CD,5 Return if transition_control is zero
C $87D2,2 If transition_control is 4
C $87D4,2 Forward transition
C $87D6,3 Call setup_transition if non-zero
C $87D9,3 Loop
c $87DC Sets up the stage
D $87DC Used by the routines at #R$8258, #R$8401, #R$858C, #R$873C and #R$F220.
R $87DC I:HL Address of 14 bytes of set up data to be copied to #R$A26C onwards.
@ $87DC label=set_up_stage
C $87DC,1 Preserve data pointer
N $87DD Zero $EE00..$EEFF and reset road_buffer_offset to $EE00.
C $87EC,1 A = 0
N $87ED Copy 47 bytes of saved game state at #R$A13E to #R$A16D.
N $87F8 Zero 208 bytes at $A19C onwards (hazard_1 onwards).
C $8801,1 Restore data pointer
N $8802 Copy the 14 bytes of set up data passed in to #R$A26C (road_pos, road_curvature_ptr, etc.)
N $8807 Pre-shift the backdrop image.
N $880A Set backdrop position in the <road drawing table TBD>.
C $880A,6 $E34B = 8
C $8810,2 $E34C = 0
C $8812,1 $E34D = 0
N $8813 NOP some things TBD.
C $8813,5 $8F82 = NOP (instruction)
C $8818,2 $8F83 = NOP (instruction)
C $881A,1 $8F84 = NOP (instruction)
N $881B NOP the 6 instruction bytes at $8FA4: the helicopter and tunnel drawing calls.
N $8824 ...
C $8824,3 Self modify "LD (HL),x" at #R$C058 to x = 0
N $8827 Reset car jump counter.
C $8827,3 Self modify "LD A,x" at #R$B063 to x = 0
C $882A,6 Set $A191 to the perp's car LOD
N $8830 Zero $EE00..$EEFF and reset road_buffer_offset to $EE00 [duplicates work from earlier].
N $883F Run the map reader 32 times [enough to draw the screen?]
C $883F,2 32 iterations
@ $8841 label=sus_loop
C $8841,1 Preserve BC
C $8842,3 Address of fast_counter == road_buffer_offset - 1 since rm_cycle_buffer_offset will INC HL [why not call an instruction later?]
C $8845,3 Call rm_cycle_buffer_offset
C $8848,1 Restore BC
C $8849,2 Loop
N $884B Disallow spawning.
C $884B,4 Clear allow_spawning
N $884F Set up screen.
C $884F,2 Reverse transition
C $8851,3 Call setup_transition
C $8854,3 Call clear_screen_set_attrs
C $8857,6 Point #REGhl at left light's attributes
C $885D,3 Point #REGhl at right light's attributes then FALL THROUGH
N $8860 Clear the lights' BRIGHT bit.
@ $8860 label=sus_clear_lights
C $8860,2 4 rows
C $8862,2 5 columns
C $8864,2 Clear BRIGHT bit
N $8870 Exit, updating the scoreboard. [This must happen twice since this block is CALLed.]
C $8870,3 Call silence_audio_hook
C $8873,3 Exit via update_scoreboard
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
C $88A1,5 Call start_chatter (priority 2)
C $88A6,3 Exit via play_turbo_sfx_hook
N $88A9 This entry point is used by the routine at #R$9BCF.
@ $88A9 label=cui_quit_key
C $88A9,5 If quit_state != 0 then return (quit in progress)
C $88AE,3 Call drive_chatter_stop
C $88B1,3 Call fill_attributes
C $88B4,4 user_input_mask = 0  -- A is zero from fill_attributes
C $88B8,3 quit_state = 1  -- start quitting
C $88BB,1 Return
@ $88BC label=cui_pause_key
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
R $88F2 I:B Sound effect index: 1..9. This indexes the table at #R$893C.
R $88F2 I:C Priority. Lower priority effects will take precedence.
@ $88F2 label=start_sfx
C $88F2,6 If current effect's priority is zero then assign
C $88F8,2 Return if current effect's priority is lower than request
@ $88FA label=ss_assign
C $88FA,4 sfx_index = B
C $88FE,4 sfx_priority = C
C $8902,1 Return
c $8903 Drives sound effects
D $8903 Used by the routine at #R$8401.
@ $8903 label=drive_sfx
C $8903,6 Jump if tunnel_sfx
C $8909,7 var_a23d |= var_a23c
C $8910,3 Effect 7 (tit-tit), Priority 4
C $8913,3 Call start_sfx if non-zero
C $8916,3 Call setup_engine_sfx_hook
C $8919,3 Call play_engine_sfx_hook
C $891C,3 Call write_registers_hook
C $891F,5 If sfx_index == 0 return
C $8924,3 #REGe = sfx_index * 4 -- stride of table
C $8927,4 sfx_index = 0
C $892B,3 sfx_priority = 0
C $892E,1 Make #REGde full offset
@ $892F ssub=LD HL,(sfx_table - 4)
C $892F,3 #REGhl -> Base of table, but 4 bytes earlier
C $8932,1 Point at array element
C $8933,4 Load argument value into #REGde (note: big endian!)
C $8937,4 Load address of routine into #REGhl
C $893B,1 Exit via routine at #REGhl
N $893C Pairs of words: (argument [big endian], address of sound effect routine) The first word is the argument passed in #REGde to sound effect routine. The second word is the address of the routine.
N $893C Effect 1 - Cornering squeal
@ $893C label=sfx_table
W $893C,4,2
N $8940 Effect 2 - Car landing (low thump)
W $8940,4,2
N $8944 Effect 3 - Crash (high) - Used for hits on cars
W $8944,4,2
N $8948 Effect 4 - Crash (low) - Used for hits on scenery
W $8948,4,2
N $894C Effect 5 - Hazard hit (high thump)
W $894C,4,2
N $8950 Effect 6 - Tunnel wall hit
W $8950,4,2
N $8954 Effect 7 - Cornering ("tit-tit")
W $8954,4,2
N $8958 Effect 8 - Time running out high ("bip")
W $8958,4,2
N $895C Effect 9 - Time running out low ("bow")
W $895C,4,2
c $8960 Sound effect - crash
R $8960 I:D Inner loop count
@ $8960 label=sfx_crash
C $8960,3 #REGhl -> Effect data table
C $8963,2 Length of table is 93
@ $8965 label=sfx_crash_loop_outer
C $8965,1 Set inner loop counter
@ $8966 label=sfx_crash_loop_inner
C $8966,2 #REGa = 16
C $8968,2 Top bit set of this byte?
C $896A,2 Don't clear the EAR bit
C $896C,2 Otherwise clear the EAR bit
C $896E,2 Output
C $8970,2 Rotate the effect data
C $8972,2 Delay
C $8974,2 Loop while #REGb > 0
C $8976,1 Move to the next byte of effect data
C $8977,4 Loop while #REGc > 0
C $897B,1 Return
@ $897C label=sfx_crash_table
B $897C,93,8*11,5 Effect data table
c $89D9 Sound effect - thud
R $89D9 I:D Delay value
@ $89D9 label=sfx_thud
C $89D9,2 Length of table is 32
C $89DB,3 #REGhl -> Effect data table
C $89DE,1 Zero output byte
@ $89DF label=sfx_thud_loop
C $89DF,1 B = *HL
C $89E0,2 Output to
C $89E2,4 Delay for 'D' iterations
C $89E6,2 Loop for 'B' iterations
C $89E8,2 Wiggle the EAR bit
C $89EA,1 Move to the next byte of effect data
C $89EB,3 Loop while #REGc > 0
C $89EE,1 Return
@ $89EF label=sfx_thud_table
B $89EF,32,8 Effect data table
c $8A0F Sound effect - cornering
R $8A0F I:D Affects delay loops
R $8A0F I:E Inner loop count
@ $8A0F label=sfx_cornering
C $8A0F,2 Half duty cycle
C $8A11,6 Return if 1
@ $8A17 label=sfx_cornering_loop_outer
C $8A17,1 Set inner loop counter
@ $8A18 label=sfx_cornering_loop_inner
C $8A18,3 Call rng
C $8A1B,2 #REGa &= 16
C $8A1D,2 If zero goto sfx_cornering_continue
C $8A1F,6 Delay loop of (24 - #REGd) iterations
C $8A25,2 #REGa = 24 set EAR and MIC bits <not sure of the effect>
C $8A27,2 Output
C $8A29,3 Delay loop of #REGd iterations
C $8A2C,3 Output
@ $8A2F label=sfx_cornering_continue
C $8A2F,3 Loop while #REGc > 0
C $8A32,3 Loop while #REGd > 0
C $8A35,1 Return
c $8A36 Sound effect - "bip-bow"
R $8A36 I:D Delay at start
R $8A36 I:E Same as D
@ $8A36 label=sfx_bipbow
C $8A36,2 C = 20
C $8A38,3 Inner loop counter. H is decremented and restored from L each loop.
@ $8A3B label=sfx_bipbow_loop
C $8A3B,3 Delay loop of D iterations
C $8A3E,1 D = E
C $8A3F,6 Delay loop of (24 - C) iterations
C $8A45,2 A = 24 set EAR and MIC bits <not sure of the effect>
C $8A47,2 Output A
C $8A49,3 Delay loop of C iterations
C $8A4C,3 Output 0
C $8A4F,3 Loop while #REGh > 0
C $8A52,1 H = L
C $8A53,3 Loop while #REGc > 0
N $8A56 This entry point is used by the routine at #R$83B5.
C $8A56,1 Return
c $8A57 Handle perp caught
D $8A57 This handles slowing cars down to a stop and calculating and displaying the bonus score when the perp has been caught.
@ $8A57 label=handle_perp_caught
C $8A57,5 Return if perp_caught_phase is zero
C $8A5C,4 Jump to #R$8B8D if it's currently one
C $8A60,3 Jump to #R$8A88 if it's currently two
C $8A63,3 Jump to #R$8ABE if it's currently three
C $8A66,3 Jump to #R$8AD2 if it's currently four
C $8A69,1 Bank
C $8A6A,5 Return if transition_control != 0
C $8A6F,1 Unbank
C $8A70,3 Jump to #R$8A7E if perp_caught_phase is currently five
N $8A73 Otherwise all of the perp caught phases are complete and we can move on to the next stage.
C $8A73,1 Throw away the return address causing the remainder of main_loop to be bypassed
C $8A74,3 Call silence_audio_hook
C $8A77,4 Increment wanted_stage_number
C $8A7B,3 Exit via main_loop
@ $8A7E label=hpc_phase5
C $8A7E,5 perp_caught_phase = 6
C $8A83,2 Forward transition
C $8A85,3 Exit via setup_transition
@ $8A88 label=hpc_phase2
C $8A88,3 A = var_a22a
C $8A8B,4 Jump to #R$8AB2 if A >= 16
C $8A8F,5 var_a22a = A + 4
C $8A94,7 HL = road_pos + 12
C $8A9C,5 HL -= $126
C $8AA2,2 Jump to #R$8AA5 if HL < $126  -- Suspect this is driving the car to stop the perp
@ $8AA5 label=hpc_8aa5
C $8AA5,3 road_pos = HL
C $8AA8,5 A = fast_counter + 32
C $8AAD,1 Return if carry
C $8AAE,3 fast_counter = A
C $8AB1,1 Return
@ $8AB2 label=hpc_start_phase_3
C $8AB2,5 perp_caught_phase = 3
C $8AB7,4 Self modify the 'LD A,x' at #R$8ABE below to load 4
C $8ABB,3 Exit via fill_attributes
@ $8ABE label=hpc_phase3
C $8ABE,2 Self modified by #R$8AB8 above
C $8AC0,1 A--
C $8AC1,3 Self modify 'LD A' at #R$8ABE to load A
C $8AC4,1 Return if A != 0
C $8AC5,5 perp_caught_phase = 4
C $8ACA,3 Point #REGhl at addrof_arrest_messages
C $8ACD,2 Set transition_control to 1 (draw mugshots)
C $8ACF,3 Exit via setup_overlay_messages_with_A
@ $8AD2 label=hpc_phase4
C $8AD2,7 Call relocated f39f_128k if in 128K mode
C $8AD9,5 Return if transition_control != 0
C $8ADE,7 Call relocated f39f_128k if in 128K mode
C $8AE5,5 perp_caught_phase = 5
N $8AEA Calculate clear bonus.
C $8AEA,2 '0'
C $8AEC,4 D = wanted_stage_number
C $8AF0,2 L = D + '0'
C $8AF2,3 Load retry_count
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
@ $8B27 label=hpc_8b27
C $8B27,1 B = A -- iterations
C $8B28,2 A += '0'
C $8B2B,3 DE = $0500
@ $8B2E label=hpc_increment_score_loop_1
C $8B2E,1 A = 0
C $8B2F,3 Call increment_score
C $8B32,2 Loop while #REGb > 0
@ $8B34 label=hpc_8b34
C $8B35,3 Set A in "TIME BONUS   A  X 5000"
C $8B38,3 A = C & 15
C $8B3B,2 Jump if zero
N $8B3D To apply Russell Marks' fix here transpose the next two instructions.
C $8B3E,1 B = A -- iterations [should be the low nibble of score]
C $8B3F,3 DE = $0050
@ $8B42 label=hpc_increment_score_loop_2
C $8B42,1 A = 0
C $8B43,3 Call increment_score
C $8B46,2 Loop while #REGb > 0
@ $8B48 label=hpc_8b48
C $8B49,2 A += '0'
C $8B4B,3 Set A in "TIME BONUS   xA X 5000"
C $8B51,3 Point DE at last digit of score_bcd (big end)
C $8B54,3 Point HL at x in "SCORE         x       " - the ten millions?
@ $8B57 label=hpc_loop_3
C $8B57,1 A = *DE    -- fetch digits
C $8B58,6 A = (A << 4) & 15   -- extract first digit
C $8B64,2 A = ' '
@ $8B68 label=hpc_8b68
C $8B68,2 C = $FF
C $8B6A,2 A += '0'
@ $8B6C label=hpc_8b6c
C $8B6C,2 *HL++ = A
C $8B6E,3 A = *DE & 15
C $8B77,2 A = ' '
@ $8B7B label=hpc_8b7b
C $8B7B,2 C = $FF
C $8B7D,2 A += '0'
@ $8B7F label=hpc_8b7f
C $8B7F,1 *HL = A
C $8B80,1 HL++
C $8B81,1 DE--
C $8B82,2 Loop hpc_loop_3 while #REGb > 0
C $8B84,1 HL--
C $8B85,2 *HL |= 1<<7
C $8B87,3 Point at "CLEAR BONUS - TIME BONUS - SCORE" messages
C $8B8A,3 Call setup_overlay_messages
@ $8B8D label=hpc_phase1
C $8B8D,3 Load the perp car's horizontal position (byte)
C $8B90,3 Is it 35?
C $8B93,2 Change by 5
C $8B95,2 Jump if no change required
C $8B97,2 Jump to change if pos < 35
C $8B99,2 Otherwise change by -5
@ $8B9B label=hpc_change_perp_pos
C $8B9B,2 C = A + B  -- new pos = old pos + delta
@ $8B9D label=hpc_assign_perp_pos
C $8B9D,4 Set the perp car's horizontal position (byte)
N $8BA1 Now move the hero car. Similar code to cpu_driver.
C $8BA1,3 Get road position
C $8BA4,7 Subtract centre value. Carry flag will be set if we're on the right hand side of the road
C $8BAB,2 Set input ACCELERATE + RIGHT
C $8BAD,2 Jump to #R$8BBA if we're on the left
C $8BAF,5 Subtract "ideal road position"? value. Carry will be set if we're on the right of it.
C $8BB4,2 Set input ACCELERATE + LEFT
C $8BB6,2 Jump to #R$8BBA if we're on the right
C $8BB8,2 Set input ACCELERATE
@ $8BBA label=hpc_assign_hero_pos
C $8BBA,1 Move input into #REGc for the moment
C $8BBB,2 Mask off LEFT and RIGHT flags
C $8BBD,2 Jump to #R$8BE5 if non-zero
N $8BBF Going straight. Are we within distance?
C $8BBF,3 A = *$A189  -- hazards[0]+1 <distance related>
C $8BC2,4 Jump if <distance value> >= 3
N $8BC6 Count down while we're within distance.
C $8BC6,4 perp_halt_counter--
C $8BCA,2 Jump if countdown is non-zero
N $8BCC Countdown hit zero: stop.
C $8BCC,7 speed = 0
C $8BD3,3 *$A18C = 0  -- hazards[0].something
C $8BD6,4 *$A189 = 1  -- hazards[0].<distance related>
C $8BDA,4 perp_caught_phase = 2
C $8BDE,4 smoke = 3
C $8BE2,3 Jump to hpc_set_perp_pos
N $8BE5 HL = 350 - (16 - <distance related>) * 20  -- compared to speed later
@ $8BE5 label=hpc_8be5
C $8BE5,3 A = *$A189  -- hazards[0]+1 <distance related>
C $8BE8,3 HL = 350  -- compared to speed later
C $8BEB,4 Jump if <distance related> >= 15
C $8BEF,3 A = 16 - <distance related>
C $8BF2,3 DE = 20
C $8BF5,1 B = A  -- iterations
@ $8BF6 label=hpc_8bf6
C $8BF6,2 HL -= DE
C $8BF8,2 Loop while #REGb > 0
@ $8BFA label=hpc_8bfa
C $8BFA,1 DE = HL
C $8BFB,3 Load speed
C $8BFE,1 Preserve speed
C $8BFF,4 Jump if speed < DE  -- DE is e.g. 350
N $8C03 #REGc is a user input here.
C $8C03,2 Clear input ACCELERATE
C $8C05,7 Jump if HL < 50
C $8C0C,2 Set input BRAKE
@ $8C0E label=hpc_8c0e
C $8C0E,1 Restore speed
C $8C0F,5 Calculate speed - 150
C $8C14,3 Load gear
C $8C17,2 Set gear to low if speed < 150, otherwise high
C $8C1B,2 Set input GEAR
@ $8C1D label=hpc_8c1d
C $8C1D,4 Set user input
@ $8C21 ssub=LD HL,(hazard_0 + 13)
C $8C21,3 Load the horizontal position of the perp's car into #REGhl
C $8C24,11 Jump to hpc_set_perp_pos if HL <= 70
C $8C2F,5 HL -= 5
N $8C35 This entry point is used by the routine at #R$8C3A.
@ $8C35 label=hpc_set_perp_pos
@ $8C35 ssub=LD (hazard_0 + 13),DE
C $8C35,4 Set the horizontal position of the perp's car to #REGde
C $8C39,1 Return
c $8C3A Fully smashed
D $8C3A Used by the routine at #R$B4F0.
@ $8C3A label=fully_smashed
C $8C3A,5 Set perp_caught_phase to 1 - starts the pull over sequence
C $8C3F,4 Show the "stop" hand (hand_flag = 2)
C $8C43,5 Set smash_counter to 20
C $8C48,5 Set user input mask to (Quit+Pause) to inhibit player control
C $8C4D,3 Print the "OK! PULL OVER CREEP!" message
C $8C50,3 Call setup_overlay_messages
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
@ $8D77 label=time_n
T $8D77,7,6:n1 "TIME 10"
B $8D7E,2,2
B $8D80,1,1 Attribute (black)
W $8D81,2,2 Back buffer address
W $8D83,2,2 Attribute address
@ $8D85 label=credit_n
T $8D85,8,7:n1 "CREDIT  "
B $8D8D,2,2
c $8D8F Drives transitions (scene fades)
D $8D8F Used by the routines at #R$8014, #R$8258, #R$8401, #R$858C, #R$873C and #R$F220.
@ $8D8F label=transition
C $8D8F,3 Load transition_control
C $8D92,2 Exit early if it's zero
C $8D94,4 If transition_control was 1, call draw_mugshots
C $8D98,4 If transition_control was 2, call draw_overlay_messages
C $8D9C,4 If transition_control was 3, call fill_attributes
N $8DA0 Otherwise transition_control was 4.
C $8DA0,3 A = <self modified> - 1  -- Self modified by #R$8E18, and below
C $8DA3,3 Self modify above
C $8DA6,2 Jump if non-zero
C $8DA8,3 transition_control = A
C $8DAB,2 Jump to t_8db7
@ $8DAD label=t_8dad
C $8DAD,3 Read #R$8DBA below
C $8DB0,3 Load offset  -- Self modified by #R$8E07
C $8DB3,1 Move to next frame
C $8DB4,3 Set anim address? (Self modify #R$8DBA below)
@ $8DB7 label=t_8db7
C $8DB7,2 H = $FF  -- row counter
C $8DB9,1 Bank
C $8DBA,3 HL = <self modified>  -- Self modified by #R$8E1F, and above
C $8DBD,2 8 chunks
@ $8DBF label=t_loop
C $8DBF,1 Load a byte of anim?
C $8DC0,1 Unbank
C $8DC1,1 Get it into #REGe (value ORred in)
C $8DC2,1 Preserve H in D (which is safe)
C $8DC3,2 Reset ptr
C $8DC5,3 Draw all even? chunks
C $8DC8,2 Reset ptr
C $8DCA,4 Move up (screen-wise) by 8
C $8DCE,3 Draw all odd? chunks
C $8DD1,2 H = D - 1 -- drop down 1 row
C $8DD3,1 Bank
C $8DD4,1 Next byte of anim
C $8DD5,2 Loop
C $8DD7,1 Return
N $8DD8 Fade odd/even UDG rows of the screen with a single byte.
@ $8DD8 label=t_fade_chunk
C $8DD8,2 8 rows
@ $8DDA label=t_fade_row_loop
C $8DDA,2 6 iterations (of 5 ops each in the loop below) = 30 bytes written (~ a scanline)
@ $8DDC label=t_fade_column_loop
C $8DDC,3 OR #REGe into the screen pixels
C $8DDF,17 Repeat another four times
C $8DF0,2 Loop
C $8DF2,2 Move to next start address
C $8DF4,4 Loop
C $8DF8,1 Return
c $8DF9 Seems to setup a transition
D $8DF9 Used by the routines at #R$8014, #R$8401, #R$858C, #R$873C, #R$87DC, #R$8A57 and #R$F220.
R $8DF9 I:A $08 or $F8 to animate forwards or backwards respectively.
@ $8DF9 label=setup_transition
C $8DF9,1 Set flags from #REGa
C $8DFA,3 Widen #REGa to #REGbc
C $8DFD,3 Points to first half of table
C $8E00,3 Jump over if #REGa was +ve
C $8E03,1 #REGb = $FF to make #REGbc properly negative
C $8E04,3 Points to second half of table
@ $8E07 label=st_set_offset
C $8E07,4 Self modify "LD DE,x" at #R$8DB0 to load BC
N $8E0B Pick a random entry in the table.
C $8E0B,5 Get a random number 0..3
C $8E10,7 HL = A * 3 + DE
C $8E17,1 Load first byte from entry
C $8E18,3 Self modify "LD A,x" at #R$8DA0 to load A
C $8E1B,4 DE = wordat(++HL)
C $8E1F,4 Set transition animation start address
C $8E23,2 4 frames of animation? 4 states?
C $8E25,3 transition_control = A
C $8E28,1 Return
c $8E29 Fills the attribute bytes leftwards from column 1
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
C $8E3E,3 transition_control = 0
C $8E41,1 Return
c $8E42 Draw overlay messages
D $8E42 Used by the routines at #R$8D8F and #R$8E91.
@ $8E42 label=draw_overlay_messages
C $8E42,3 Address of current message thing (frame delay, flags, attrs, bufaddr, attraddr, string)
C $8E45,2 B = 0 -- self modified by (#R$8E58 increments it), (#R$8E8D resets it to 1)
@ $8E47 label=dom_loop
C $8E47,2 Loop? to dom_1
C $8E49,2 A = <self modified> frame delay. Self modified by #R$8E4C, #R$8E51, (#R$8E84 sets it)
C $8E4B,1 A--
C $8E4C,3 Self modify #R$8E49 above
C $8E4F,1 Return if non-zero
C $8E50,1 A = *HL
C $8E51,3 Self modify #R$8E49 above
C $8E54,7 Increment operand of #R$8E45, affects TIME UP state if disabled
C $8E5B,1 B++
@ $8E5C label=dom_1
C $8E5C,2 A = *++HL
C $8E5E,3 Jump to dom_2 if A is zero
C $8E61,3 Call print_message
C $8E64,2 Loop to dom_loop
@ $8E66 label=dom_2
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
c $8E7E Setup overlay messages
D $8E7E Called for TIME UP, CONTINUE, BEST OFFICERS, etc. messages.
D $8E7E Used by the routines at #R$8258, #R$873C, #R$8A57, #R$8C3A, #R$9BCF, #R$B4CC and #R$F220.
R $8E7E I:HL Address of a message structure, e.g. credits_messages. [One beginning with a frame delay.]
@ $8E7E label=setup_overlay_messages
C $8E7E,2 Set transition_control to 2 (show overlay)
N $8E80 This entry point is used by the routine at #R$8A57.
@ $8E80 label=setup_overlay_messages_with_A
C $8E80,3 Set transition_control to #REGa
C $8E83,4 Modify 'LD A,x' @ #R$8E49 with frame delay
N $8E87 Set the overlay message structure address.
C $8E87,4 Modify 'LD HL,x' @ #R$8E42 to be a (different type) message struct address, e.g. $82CD
C $8E8B,5 Modify 'LD B,x' @ #R$8E45 to be 1
C $8E90,1 Return
c $8E91 Copies the mugshots onto the screen when perp is caught
D $8E91 Used by the routine at #R$8D8F.
@ $8E91 label=draw_mugshots
C $8E91,3 Load end address of perp's mugshot bitmap into #REGhl
C $8E94,3 Address of final byte of bitmap (back buffer)
C $8E97,3 Screen position (40,104)
C $8E9A,3 Draw
C $8E9D,3 Load end address of Tony's mugshot bitmap into #REGhl
C $8EA0,3 Final byte of bitmap (back buffer)
C $8EA3,3 Screen position (160,104)
C $8EA6,3 Draw
C $8EA9,3 Load end address of Raymond's mugshot bitmap into #REGhl
C $8EAC,3 Final byte of bitmap (back buffer)
C $8EAF,3 Screen position (200,104)
C $8EB2,3 Draw
C $8EB5,2 Exit via draw_overlay_messages
c $8EB7 Draws a mugshot
D $8EB7 Used by the routine at #R$8E91.
R $8EB7 I:BC Screen position (used for attributes)
R $8EB7 I:DE Address of last byte of bitmap data to be written (in back buffer)
R $8EB7 I:HL Address of start of mugshot attributes / end of bitmap data
@ $8EB7 label=draw_mugshot
C $8EB7,1 Preserve screen position
C $8EB8,1 Preserve address of mugshot attributes
N $8EB9 We assume that the bitmap data precedes the attribute bytes.
C $8EB9,1 Move back one byte to point at end of bitmap data
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
c $8EE7 Draw the smash bar
D $8EE7 Used by the routine at #R$8401.
@ $8EE7 label=draw_smash_bar
C $8EE7,5 Return if the perp has not yet been sighted
C $8EEC,6 Return if perp_caught_phase is >= 3 (car has stopped)
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
c $8F5F Draws anything that's not the road or the hero car
D $8F5F Used by the routines at #R$8401, #R$852A and #R$873C.
@ $8F5F label=draw_everything_else
C $8F5F,3 Point #REGhl at (something above the stack)
C $8F62,3 Self modify 'LD HL' @ #R$A9E2 to load the stack? address
C $8F65,6 Self modify 'LD HL' @ #R$AECF to load $E900
N $8F6B Add 32 to the first 21 entries of tables $E301 and $E336. If meddled with this affects the height of elements.
C $8F6B,3 HL = $E301
C $8F6E,3 DE = $E336
C $8F71,3 B = 21 iterations, C = 32 (added to table entries)
@ $8F74 label=dee_loop
C $8F74,3 *HL += C
C $8F77,3 *DE += C
C $8F7A,1 E++
C $8F7B,1 L++
C $8F7C,2 Loop to #R$8F74 while #REGb > 0
C $8F7E,4 IY = $E315
C $8F82,3 Self modified: either CALL draw_tunnel, or NOPs
C $8F85,2 IY--
C $8F87,3 Load road_buffer_offset into #REGa
C $8F8A,2 Add 115 so it's the right side objects data offset + 19
C $8F8C,3 Point #REGhl at road buffer right side objects data
C $8F8F,4 IX = $EAB0
C $8F93,3 BC = $1420
C $8F96,4 Preserve IX, HL, BC
C $8F9A,7 Call (somewhere in draw_hazards) if n_hazards is set
C $8FA1,3 Call dust_stones_stuff (dust/stones stuff)
C $8FA4,3 Self modified: either CALL draw_helicopter, or NOPs
C $8FA7,3 Self modified: either CALL draw_tunnel, or NOPs
C $8FAA,4 Restore IX, HL, BC
C $8FAE,1 A = *HL
C $8FAF,1 Set flags
C $8FB2,4 IX += 2
C $8FB6,3 L += C
C $8FB9,1 A = *HL
C $8FBA,1 Set flags
C $8FBB,2 Exit via #R$9023 if non-zero
N $8FBD This entry point is used by the routine at #R$9023.
C $8FBD,4 IX += 2
C $8FC1,4 L -= $21
C $8FC5,2 IY--
C $8FC7,2 Loop to $8F96 while #REGb > 0
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
C $8FF5,3 Exit via #R$B6DD
C $8FF8,1 E = A
C $8FF9,3 A = IX[1]
C $8FFC,1 Set flags
C $9003,8 DE = E * 7
C $900B,3 Push return address onto stack
N $9012 $5E42 sampled DE = $23, $1C, $3F, $38, $31, $2A -- multiples of 7
C $9012,1 HL += DE
C $9013,4 DE = wordat(HL); HL += 2
C $9017,4 HL = wordat(HL); HL += 1
C $901B,1 Jump to HL
C $9020,3 Jump to #R$8FB2
c $9023 Routine at 9023
D $9023 Used by the routine at #R$8F5F.
R $9023 I:A ?
R $9023 I:BC ? Sampled: $0D20, $0520, $0120
R $9023 I:HL ? Sampled: $EEAC, $EEA4, $EEA0
R $9023 I:IX ? Sampled: $EACE, $EAEE, $EAFE
C $9023,1 E = A  -- sampled = 5 (only)
C $9024,4 Jump if A == 2
N $9028 A != 2
C $9028,3 A = IX[1]
C $902B,1 Set flags
C $902C,2 Loop?
C $902E,4 Preserve IX, HL, BC
C $9032,8 DE = E * 7
C $903A,4 Set return address
C $903E,3 turn sign lods
C $9041,1 HL += DE
N $9042 sampled HL = $5EA4 (only) which is "Entry 14"
C $9042,4 DE = wordat(HL); HL += 2  -- arg for routine
C $9046,4 HL = wordat(HL)  -- address of routine
N $904A #REGb is used in the routines, what is it here? It must be passed in.
C $904A,1 Indirect jump  -- Call #R$916C #R$9171 #R$924D #R$9252 #R$92E1 etc.
@ $904B label=return_904B
C $904B,4 Restore IX, HL, BC
C $904F,3 Loop?
c $9052 Draws overhead graphics.
D $9052 This gets used on stage 3 when drawing the overhead structure graphics.
D $9052 sampled IX=$EAB2 DE=$6F26 HL=$9052 BC=$1420 (when in stage3!)
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
C $9088,3 Self modify 'SUB x' at $90F1
C $908B,2 A = B - 1
C $9091,2 A = $09
@ $9093 label=do_9093
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
@ $90C4 label=do_90c4
C $90C4,4 IX -= 2
C $90C8,2 E = $1F
C $90CA,3 A = IX[1]
C $90CD,1 Set flags
C $90D1,4 A = IX[0] + C
C $90DD,6 A >>= 3
C $90E3,1 E = A
@ $90E4 label=do_90e4
C $90E4,1 C = D
C $90E5,3 A = E * 2 - C
C $90E8,3 A = ~A + $3D
C $90EB,3 *$9116 = A   -- Self modify jump table target
C $90EE,3 A = IY[$35]
C $90F1,2 A -= <self modified>  -- self modified by $9088
C $90F5,1 A++
C $90F6,1 B = *HL
C $90F7,1 A -= B
C $90FA,1 A += B
C $90FB,1 B = A
@ $90FC label=do_90fc
C $90FC,1 HL++
C $90FD,3 DE = wordat(HL); HL++
C $9101,1 L = A
C $9102,5 H = (A & $0F) + $F0
C $9107,6 L = (L & $70) * 2 + C
@ $910F label=do_continue
C $910F,1 DE++
C $9112,1 Return
@ $9113 label=do_9113
C $9113,1 A = *DE
C $9114,1 C = L
@ $9115 label=do_9115
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
c $916C Draws stretchy objects, such as trees.
D $916C Used by the routine at #R$9052.
R $916C I:B ? value that affects scaling
R $916C I:DE Address of graphic data [structure type yet to be named] (e.g. stretchy_shortpole/#R$7E05)
N $916C Entry point for left hand objects.
@ $916C label=draw_stretchy_object_left
C $916C,3 HL = $9293  -- callback address
N $9171 Entry point for right hand objects.
@ $9171 label=draw_stretchy_object_right
C $9171,3 HL = $92FC  -- callback address
@ $9174 label=dso_common
C $9174,3 Self modify CALL at $91CD to call #REGhl
C $9177,3 Self modify CALL at $9243 to call #REGhl
C $917A,5 A = fast_counter & $E0
N $917F Reduces A by 31.25%. A = 0..224 (in steps of 16) becomes 0..154.
C $917F,1 L = A  -- nothing will rotate out due to AND $E0
C $9180,4 L >>= 2
C $9184,1 A -= L
C $9185,4 L >>= 2
C $9189,1 A -= L
C $918A,1 B is the value passed in
N $918B Look up that value in $E600[].
C $918B,3 #REGhl = $E600 + #REGl
C $918E,1 A = *HL
C $918F,3 Self modify 'LD A,x' at #R$91DB to load A
C $9192,1 Swap
C $9193,7 A = min(B,10)
C $919A,5 DE = A * 2 - 1  -- the -1 accounts for later pointer position
C $919F,1 C = 0
C $91A0,4 Self modify 'LD BC,xxxx' at #R$91BA to load DE
@ $91A4 label=dso_loop
C $91A4,3 A = -C
C $91A7,3 Self modify 'LD D,x' @ #R$933D to load A
C $91AA,2 B = *HL - 1  -- read count byte from graphic stream, e.g. stretchy_shortpole + 0
C $91AC,1 Return if count byte was 1 (terminator)
C $91AD,1 Advance past count byte
C $91AE,4 Read address of ? data to #REGde, e.g. shortpole_7e0c
C $91B2,4 Preserve HL, IX, BC
C $91B6,1 Swap
C $91B7,3 DE = wordat(HL); HL++  -- this is the LOD pointer e.g. streetlampbody_lods
N $91BA This seems to be a 0..9 value that selects from the table.
C $91BA,4 HL += <self modified>  -- by #R$91A0, this is the table offset from earlier
N $91BE HL now points to an entry in the table. Load it.
C $91BE,2 A = *HL++  -- e.g. 0
C $91C0,3 HL = *HL  -- e.g. $56
C $91C3,1 HL += DE  -- so HL's an offset from streetlampbody_lods
C $91C4,1 Restore BC
C $91C5,1 B--   -- e.g. cmd byte 2 would be 0 after this
C $91C6,2 Jump to dispatch for cmd bytes 3+
N $91C8 Zero path => command byte was 2
C $91C8,1 B = *HL
C $91C9,2 HL -= 2
C $91CB,1 Preserve BC
C $91CC,1 B = A
C $91CD,3 Call <self modified>  -- callback
@ $91D0 label=dso_loop_perhaps
C $91D0,1 Restore BC
C $91D1,3 C += B
C $91D4,3 Restore IX, HL
C $91D7,3 Loop dso_loop
N $91DA Non-zero path
@ $91DA label=dso_dispatch
C $91DA,1 Bank <byte[0] of table entry, loaded at #R$91BE>
C $91DB,2 A = <self modified>
N $91DD Dispatch ladder
C $91DD,3 Jump if 3
C $91E0,3 Jump if 4
C $91E3,3 Jump if 5
C $91E6,3 Jump if 6
C $91E9,3 Jump if 7
C $91EC,3 Jump if 8
C $91EF,3 Jump to continue if 9
C $91F2,1 A += A
C $91F3,2 Continue
@ $91F5 label=dso_case_8
C $91F5,4 A >>= 2
C $91F9,2 Continue
@ $91FB label=dso_case_7
C $91FD,1 B = A
C $9200,1 A += B
C $9201,2 Continue
@ $9203 label=dso_case_6
C $9205,1 B = A
C $9206,4 B >>= 2
C $920A,1 A -= B
C $920B,2 Continue
@ $920D label=dso_case_5
C $920D,1 B = A
C $920E,6 B >>= 3
C $9214,1 A += B
C $9215,2 Continue
@ $9217 label=dso_case_4
C $9219,2 Continue
@ $921B label=dso_case_3
C $921B,1 B = A
C $921E,1 A += B
@ $921F label=dso_continue
C $921F,1 A -= C
C $9222,2 C was >= A
C $9224,2 A = 1
C $9226,1 B = A
C $9228,6 A = IY[$35] + 1 - C - B
C $9230,2 B += A
C $9232,1 A = B
C $9233,3 *$9405 = A
C $9236,1 A = *HL
C $9237,2 HL -= 2
C $9239,3 *$9416 = A
C $923C,2 A = $02
C $923E,3 Self modify 'LD A,x' @ #R$93C0 to load 2
C $9242,1 B = A
C $9243,3 Call <self modified>
C $9246,4 Self modify 'LD A,x' @ #R$93C0 to load 0
C $924A,3 Loop to dso_loop_perhaps
c $924D Draws tunnel lights (and possibly other bitmaps).
R $924D R:B Offset added to $E6xx address. Routine skipped if it's >= 16.
@ $924D label=draw_tunnel_light_left
C $924D,3 HL = $9279
@ $9252 label=draw_tunnel_light_right
C $9252,3 HL = $92E2
C $9255,4 Return if B >= 16
C $9259,1 Push return address
C $925A,5 A = fast_counter & $E0
N $925F Reduces A by 31.25%
C $925F,1 L = A
C $9260,4 L >>= 2
C $9264,1 A -= L
C $9265,4 L >>= 2
C $9269,1 A -= L
C $926A,1 A += B   -- B was passed in
C $926B,1 L = A
C $926C,2 H = $E6   -- $E6xx data
C $926E,1 A = *HL
C $926F,8 A = (A>>2) - A
C $9277,1 Return via earlier PUSH
c $9278 Draws objects (left hand version).
D $9278 Called via dispatch at #R$901B.
D $9278 Used to draw turn signs, tunnel lights, ... what else?
R $9278 I:B ?
R $9278 I:IX ?
R $9278 I:DE Address of arg, e.g. $6B0C
@ $9278 label=draw_object_left
C $9278,1 A = 0
C $9279,3 Self modify 'LD D,x' @ #R$933D to load #REGa
C $927C,7 A = min(B,10)
C $9283,1 Put arg address in HL
C $9284,3 DE = wordat(HL); HL++   -- loads address of turn_sign_lods
C $9287,6 HL += A * 2 - 1   -- index the table
C $928D,2 B = *HL++   -- this reads #R$6AF8
C $928F,4 HL = *HL + DE   -- read as byte
N $9293 This entry point is used by the routine at #R$916C.
@ $9293 label=draw_object_left_stretchy_entrypt
C $9293,6 A = IX[0] + 16 - B
C $9299,1 Return if carry (if IX[0] + 16 < B)
N $929A This entry point is used by the routines at #R$A9DE, #R$AA38 and #R$ADA0.
@ $929A label=draw_object_left_helicopter_entrypt
C $929A,3 Return if A < 8
C $929D,2 C = 0
C $929F,1 E = *HL
C $92A0,6 likely E << 3
C $92A6,1 A -= E
C $92A9,4 Jump if A >= 8
C $92AD,1 E = *HL
C $92B0,3 Self modify 'LD A,x' at $9395 to load A
C $92B3,2 A = E - 1
C $92B5,3 BC = $0101
C $92BB,1 E = *HL
C $92BC,2 A &= $FC
C $92C0,3 Self modify 'LD A,x' at $9395 to load A
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
C $92DE,3 Exit via draw_object_common
c $92E1 Draws objects (right hand version).
R $92E1 I:B ?
R $92E1 I:IX ?
R $92E1 I:IY ?
R $92E1 I:DE Address of arg, e.g. $6B0C
@ $92E1 label=draw_object_right
C $92E1,1 A = 0
C $92E2,3 Self modify 'LD D,x' @ #R$933D to load #REGa
C $92E5,7 A = min(B,10)
C $92EC,1 Put arg address in HL
C $92ED,3 DE = wordat(HL); HL++   -- loads address of turn_sign_lods
C $92F0,6 HL += A * 2 - 1   -- index the table
C $92F6,2 B = *HL++
C $92F8,4 HL = *HL + DE   -- read as byte
@ $92FC label=draw_object_right_stretchy_entrypt
C $92FC,3 A = IX[0]
C $92FF,2 Test B's sign
C $9301,2 Jump if +ve
C $9303,1 A += B
C $9304,2 Jump
C $9306,1 A += B
C $9307,1 Return if carry
C $9308,1 Return if zero
N $9309 This entry point is used by the routines at #R$A9DE, #R$AA38 and #R$ADA0.
@ $9309 label=draw_object_right_helicopter_entrypt
C $9309,3 Return if A >= $F7
C $930C,2 C = 0
N $930E This entry point is used by the routine at #R$9278.
C $930E,4 A = (A & $FC) >> 2
C $9312,3 Self modify 'LD A,x' at $9395 to load A
C $9315,1 A >>= 1
C $9316,1 B = A
C $9317,1 E = *HL
C $9318,3 A = ~A + 32  -- (31 - A)
C $931B,1 Is E < A?
C $931C,2 Jump if so
C $931E,1 A = E
C $931F,1 HL++
C $9320,3 D = *HL >> 1
C $9323,3 Jump if zero
C $9326,1 C = A
C $9328,3 C = E - C
N $932B This entry point is used by the routine at #R$9278.
@ $932B label=draw_object_common
C $932B,7 Self modify 'LD A,x' at #R$9395 to be (~x)
N $9333 This entry point is used by the routine at #R$9278.
C $933C,1 HL++
C $933D,2 D = <self modified>
C $933F,6 A = IY[0] - IY[$35]
C $9347,4 A = IY[$35] + D
C $934D,1 A++
C $934E,1 D = *HL
C $934F,1 A -= D
C $9352,1 A = 0
C $9353,1 A += D
C $9354,1 HL++
C $9355,2 D = 1
C $9359,4 Jump if D is -ve
C $935D,1 A -= D
C $9361,1 A += D
C $9362,1 D = A
C $9369,3 A = IY[$35]
C $936D,3 A = *HL - 1 - D
C $9370,3 Jump if no carry
C $9374,3 Read A from 'LD A,x' @ #R$93C0 below
C $9377,1 Set flags
C $9378,1 Return if zero
C $9379,3 Read A from 'LD D,x' @ #R$933D above
C $937C,1 D = *HL
C $937D,1 A -= D
C $937E,1 Return if positive
C $937F,3 *$933E = A
C $9382,4 A = *$9405 - D
C $9386,2 Return if carry or zero
C $9388,3 *$9405 = A
C $938B,2 Loop?
C $938D,1 A++
C $938E,1 D++
C $938F,1 HL++
C $9392,1 D = A
C $9393,2 B = 0  -- not self modified AFAICT
C $9395,2 A = <self modified>
C $939A,2 HL += 2
C $939C,5 HL = wordat(HL) + BC
C $93A1,3 Self modify 'LD HL,xxxx' @ #R$9412 to load HL
C $93A4,1 C = E
C $93A6,1 A--
C $93A9,1 HL += BC
C $93AA,1 A--
C $93AE,1 B = D
C $93AF,2 D = 0  -- not self modified AFAICT
C $93B4,1 D = A
C $93B5,5 H = (A & $0F) + $F0
C $93BA,1 A = D
C $93BB,5 L = (A & $70) * 2 + B
C $93C0,2 A = <self modified>
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
C $93FB,4 Self modify CALL at $940F
C $93FF,4 Self modify JP at $941D
C $9404,3 A = <self modified> - B
C $940B,3 Self modify 'LD A,x' @ #R$9404
C $940F,3 Call <self modified> [e.g. plot_sprite_entry]
C $9412,3 HL = <self modified by $93A1>
C $9415,2 B = <self modified by $9239>
C $941A,1 A += B
C $941B,1 B = A
C $941D,3 Exit via <self modified> [e.g. plot_sprite_entry]
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
C $946C,3 HL = <self modified>  -- Self modified by $9446
C $946F,2 B = $00
C $9474,2 B += A
C $9476,3 Exit via plot_masked_sprite
C $947B,2 D = 0  -- not self modified AFAICT
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
c $949C Sprite plotter for back buffer, up to 64px wide, 15px high, no mask, no flip
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
C $94B1,4 Save #REGsp to restore on exit (self modify)
C $94B5,3 #REGb = 15 rows to draw, #REGc = 16, an increment value used later
C $94B8,1 Bank
C $94B9,2 Jump into body of loop
@ $94BB label=ps_even_loop
C $94BB,1 Bank
@ $94BC label=ps_even_continue
C $94BC,2 Next scanline
C $94BE,3 Restore original #REGsp (self modified)
C $94C1,1 Return
@ $94C2 label=ps_even_next
C $94C2,1 Calculate address of next bitmap scanline
@ $94C3 label=ps_even_body
C $94C3,1 Put it in #REGsp (so we can use POP for speed)
C $94C4,2 Unbank
C $94C6,2 Jump table
@ $94C8 label=ps_jumptable
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
C $9503,4 Save #REGsp to restore on exit (self modify)
C $9507,3 #REGb = 15 rows to draw, #REGc = 16, an increment value used later
C $950A,1 Bank
C $950B,2 Jump into body of loop
@ $950D label=ps_odd_loop
C $950D,1 Bank
@ $950E label=ps_odd_continue
C $950E,2 Next scanline
C $9510,3 Restore original #REGsp (self modified)
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
N $952C Decrement the screen address.
C $952C,1 Save H in A
C $952D,1 Decrement row address
C $952E,1 Extract low four bits of row address
C $952F,3 If zero we'll need to handle it below, otherwise just loop
C $9532,3 H += 16
C $9535,4 Decrement high three bits of the row address
C $9539,3 No carry, so finish scanline
C $953C,3 H -= 16
C $953F,3 Loop back to ps_odd_loop
c $9542 Sprite plotter for back buffer, up to 64px wide, 15px high, no mask, flips
D $9542 Used by the routines at #R$92E1 and #R$B58E.
R $9542 I:A Plot (#REGa + 1) * 8 pixels
R $9542 I:DE' Stride of bitmap data in bytes
R $9542 I:HL Address in back buffer to plot at
R $9542 I:HL' Address of bitmap data
@ $9542 label=plot_sprite_flipped
C $9542,4 Advance the back buffer pointer to the end of the first line, so we can draw in reverse
C $9546,5 Use psf_odd if the bottom bit is set (odd widths)
C $954B,4 Save #REGsp to restore on exit (self modify)
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
C $956A,3 Restore original #REGsp (self modified)
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
C $95B4,4 Save #REGsp to restore on exit (self modify) [in a different position to others]
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
C $95D3,3 Restore original #REGsp (self modified)
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
R $9618 O:A Random byte
R $9618 O:HL Corrupted
@ $9618 label=rng_seed
B $9618,3,3 Seed / initial state
N $961B This entry point is used by the routines at #R$860F, #R$8A0F, #R$8DF9, #R$99EC, #R$A637, #R$A7F3 and #R$A955.
@ $961B label=rng
C $961B,3 Point at rng_seed / state bytes
C $961E,4 rng_seed[0] -= 141
C $9622,4 rng_seed[1] += 3
C $9626,2 A = rng_seed[0] + rng_seed[1]
C $9628,1 Rotate A by 1 through carry
C $9629,2 Rotate rng_seed[2] by 1 through carry
C $962B,2 rng_seed[2] += A
C $962D,1 Return
g $962E In-game message variables
@ $962E label=next_character
W $962E,2,2 Address of the next character in the current message
@ $9630 label=message_set
W $9630,2,2 Address of current message set
@ $9632 label=message_x
B $9632,1,1 Index of next character in the message bar
@ $9633 label=chatter_delay
B $9633,1,1 Counted down while waiting to display the next line of chatter
@ $9634 label=noise_bytes
B $9634,5,5 Five bytes used for noise when character pictures 'noise in'
B $9639,3,3
@ $963C label=noise_counter
B $963C,1,1 Noise effect counter (4..0)
@ $963D label=chatter_state
B $963D,1,1 0 => idle, 1 => starting, 2 => displaying, 3 => stopping
@ $963E label=chatter_priority
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
@ $98D0 label=wrong_way_chatter
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
@ $9941 label=lets_go_mr_driver
B $9941,1,1 Tony ($03)
W $9942,2,2 -> "LET'S GO. MR. DRIVER."
B $9944,1,1 <STOP>
c $9945 Start chatter
D $9945 Used by the routines at #R$8401, #R$858C, #R$873C, #R$8876, #R$9BCF, #R$A637, #R$A8CD, #R$AB33, #R$B063, #R$B4F0 and #R$B9F4.
R $9945 I:A Message priority: must be higher than the stored priority for the message to take effect
R $9945 I:HL Address of message set
@ $9945 label=start_chatter
C $9945,1 Copy priority value to #REGb
C $9946,3 Load chatter_state into #REGa
C $9949,3 If chatter_state == 0 (chatter idle) jump forward to #R$9955
C $994C,4 If chatter_state >= 3 (chatter end) jump forward to #R$9955
C $9950,5 If the current chatter_priority >= priority given then return
@ $9955 label=chatter_set
C $9955,4 Update chatter_priority
C $9959,3 Set message set pointer
C $995C,4 chatter_delay = 0
C $9960,4 chatter_state = 1 (start chatter)
C $9964,1 Return
c $9965 Runs chatter, mugshots and noise effect
D $9965 Used by the routines at #R$8401, #R$858C and #R$873C.
@ $9965 label=drive_chatter
C $9965,3 Load chatter_state
C $9968,3 If chatter_state was 1 (starting) then jump to drive_chatter_clear
C $996B,3 If chatter_state was 2 (displaying) then jump to drive_chatter_do_noise_effect
C $996E,3 If chatter_state was 0 (idle) then jump to drive_chatter_idle
N $9971 Otherwise chatter_state is 3 (stopping).
C $9971,4 Decrement noise_counter in-place
C $9975,1 Load noise_counter
C $9976,3 Jump into noise_effect if it was non-zero
C $9979,3 New chatter_state is 0 (idle)
C $997C,3 Call noise_plot_attrs to clear to black
N $997F This is the flashing cursor.
@ $997F label=drive_chatter_idle
C $997F,2 Plot a space character
C $9981,6 Alternating bit generator: $AA rotated 1 becomes $55, $55 rotated 1 becomes $AA, and so on, result in carry flag
C $9987,2 (flag?)
C $9989,3 Exit via plot_mini_font_2 if carry set
C $998C,3 Otherwise exit via plot_mini_font_1
@ $998F label=drive_chatter_do_noise_effect
C $998F,7 If noise_counter > 0 exit via noise_effect
C $9996,6 If chatter_delay is zero jump to drive_chatter_clear_line
C $999C,4 Otherwise decrement chatter_delay
C $99A0,1 Copy chatter_delay for later
C $99A1,2 If chatter_delay was zero jump to drive_chatter_99bd
C $99A3,3 Load address of next character
C $99A6,1 Go back 1
C $99A7,1 Load the character for when we call plot_mini_font*
C $99A8,2 Clear string terminator bit
C $99AA,4 A = message_x - 1
C $99AE,2 Rotate chatter_delay (testing the bottom bit but why?)
N $99B0 Could JP $9989 here instead.
C $99B0,3 Exit via plot_mini_font_2 if carry set
C $99B3,3 Otherwise exit via plot_mini_font_1
@ $99B6 label=drive_chatter_clear_line
C $99B6,7 If message_x != 0 exit via pc_clear_line
@ $99BD label=drive_chatter_99bd
C $99BD,3 Load message set address
C $99C0,1 Read a byte
C $99C1,4 Is it a message set terminator? Jump to drive_chatter_stop if so
N $99C5 An $FE byte means the next two bytes are the address of another message set.
C $99C5,2 Is it introducing a message set address?
C $99C7,2 Exit via pc_chatter_message if not
N $99C9 The next two bytes hold a message set address.
C $99C9,1 Step over the $FE byte
C $99CA,7 message_set = wordat(HL)
C $99D1,2 Jump to drive_chatter_clear
N $99D3 This entry point is used by the routines at #R$858C and #R$8876.
@ $99D3 label=drive_chatter_stop
C $99D3,5 Set noise_counter to 4
C $99D8,4 Set chatter_state to 3 (stopping)
C $99DC,3 Exit via clear_message_line
@ $99DF label=drive_chatter_starting
C $99DF,5 chatter_state = 2 (displaying)
@ $99E4 label=drive_chatter_clear
C $99E4,3 Call clear_message_line
C $99E7,2 Set noise effect counter to 4
C $99E9,3 Exit via noise_effect
c $99EC Shows the chatter - the alerts and remarks from the game's characters
D $99EC Used by the routine at #R$9A55.
@ $99EC label=print_chatter
C $99EC,3 Load current message set address
@ $99EF label=pc_loop
C $99EF,2 Read a byte and advance
N $99F1 $FC indicates a three-way random choice. Otherwise it's the index of the speaking character (1/2/3 for Nancy/Raymond/Tony) or 0 for the helicopter pilot.
C $99F1,4 If byte != $FC then jump to pc_plot_character
N $99F5 We have a three-way choice here.
@ $99F5 label=pc_random_choice
C $99F5,1 Preserve message data address
C $99F6,3 Call rng - returns a random byte in #REGa
C $99F9,1 Restore message data address
C $99FA,4 If random value < $55 jump to pc_load_message_ptr -- a 33.3% chance
C $99FE,2 Skip first option message
C $9A00,4 If random value < $AA jump to pc_load_message_ptr -- a 66.6% chance
C $9A04,2 Skip second option message
@ $9A06 label=pc_load_message_ptr
C $9A06,4 Load the address of the chosen message set
C $9A0A,2 Loop back - it could be another random choice
N $9A0C #REGa is the index of the face to show.
@ $9A0C label=pc_plot_character
C $9A0C,1 Is it zero? (=> the pilot character's face)
C $9A0D,1 Preserve message pointer
C $9A0E,6 Plot whatever face #R$5CF2 points to, otherwise calculate the face graphic's address
@ $9A14 ssub=LD HL,(bitmap_nancy - 180)
C $9A14,3 Load base address of face graphics (BUT it's actually 180 bytes earlier than the real base because we're 1-indexed)
C $9A17,3 Length of face graphic (bitmap + attrs = 4*8*5 + 4*5)
@ $9A1A label=pc_multiply
C $9A1A,3 Get face graphic address
@ $9A1D label=pc_do_plot
C $9A1D,3 Set plot address to (176,8)
C $9A20,3 Call plot_face
C $9A23,1 Restore message pointer
N $9A24 This entry point is used by the routine at #R$9965.
@ $9A24 label=pc_chatter_message
C $9A24,4 Load address of message to start showing
C $9A28,3 Save current message set address
C $9A2B,4 Save next character address
C $9A2F,1 Cause a clear_message_line and fall through
N $9A30 This entry point is used by the routine at #R$9965. #REGa is message_x.
@ $9A30 label=pc_clear_line
C $9A30,6 If (A == 0) clear_message_line
C $9A36,3 Load next_character
C $9A39,1 Load the character itself
C $9A3A,2 Clear any string terminator bit
C $9A3C,1 Preserve message_x
C $9A3D,1 Preserve string address
C $9A3E,3 Call plot_mini_font_2, with #REGd as ASCII character to plot
C $9A41,1 Restore string address
C $9A42,2 Test string terminator bit
C $9A44,1 Move to the next character in the string
C $9A45,2 If not terminated then jump over
C $9A47,5 Set chatter_delay to 10
@ $9A4C label=pc_exit
C $9A4C,1 Restore message_x
C $9A4D,4 Increment and store message_x
C $9A51,3 Update next_character
C $9A54,1 Return
c $9A55 Noise in/out effect used for mugshots
D $9A55 Used by the routine at #R$9965.
R $9A55 I:A Noise effect counter
@ $9A55 label=noise_effect
C $9A55,4 Decrement noise_counter
C $9A59,3 Jump to print_chatter if it's zero, otherwise fallthrough
N $9A5C This entry point is used by the routine at #R$9965.
@ $9A5C label=ne_9a5c
C $9A5C,1 Shift A's bottom bit into carry
C $9A5D,2 A = 255  -- plot_mini_font_1 flag?
C $9A5F,2 D = 32  -- ASCII character for plot_mini_font*
C $9A61,5 If A on input was odd then jump
C $9A66,2 else call plot_mini_font_2 and continue at $9a6b
@ $9A68 label=ne_9a68
C $9A68,3 Call plot_mini_font_1
@ $9A6B label=ne_9a6b
C $9A6B,3 Set plot address to (176,8)
@ $9A70 label=ne_9a70
C $9A6E,4 B,C = 4 columns/bytes, 40 rows
C $9A72,1 Preserve row ptr
C $9A73,3 Point #REGhl at noise_bytes
@ $9A76 label=ne_plot_loop
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
@ $9A98 label=ne_plot_attrs
C $9A98,3 Screen attribute position (22,1)
C $9A9B,2 5 rows
C $9A9D,3 32 - 3 = row skip
@ $9AA0 label=ne_9a00_loop
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
R $9AEC I:A flag?
R $9AEC I:D The character to plot (ASCII)
@ $9AEC label=plot_mini_font_1
C $9AEF,2 Jump to pmf_go
N $9AF1 This entry point is used by the routines at #R$9965, #R$99EC and #R$9A55.
@ $9AF1 label=plot_mini_font_2
C $9AF1,3 values to self modify with
@ $9AF4 label=pmf_go
C $9AF4,1 Preserve A
C $9AF5,8 self modify LD C and OR 7 later
C $9AFD,1 Restore A
C $9AFE,4 If not string terminator, goto pmf_regular_char  -- or is this a flag?
N $9B02 String terminator?
C $9B02,2 $BF / 8?
@ $9B08 label=pmf_regular_char
C $9B08,6 A = A*5+2
@ $9B10 label=pmf_9b10
C $9B0E,4 8 - $C0 / 8 ??
@ $9B12 label=pmf_9b12
C $9B12,7 divider / rounding?
C $9B19,8 Modify jump table - A * 4 - 4 bytes per sequence
@ $9B21 label=pmf_9b21
C $9B21,3 General multiplier by A
C $9B24,3 Self modify #R$9B88 which is a mask
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
@ $9B5F label=pmf_9b5f
C $9B58,8 Point #REGhl at minifont
C $9B60,3 Self modified earlier
C $9B63,3 Self modified earlier
C $9B66,2 Jump table (self modified)
C $9B88,2 This must be self modified
C $9B8A,4 *scr++ = (*scr AND A) OR B
C $9B8E,2 *scr-- = C
C $9B90,1 Next source byte ?
C $9B91,16 Move to next scanline
@ $9BA1 label=pmf_next_row
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
C $9BCF,5 Return if perp_caught_phase > 0
C $9BD4,6 Return if transition_control == 4 -- don't tick while transitions are running
C $9BDA,3 Point #REGhl at time_bcd
C $9BDD,6 Jump to #R$9C0D if time_up_state was 1 - out of time
C $9BE3,3 Jump to #R$9C31 if time_up_state was 2 - show "TIME UP" message
C $9BE6,3 Jump to #R$9C50 if time_up_state was 3 - show "CONTINUE" message & run countdown
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
C $9C0A,3 Exit via start_chatter (priority 21)
C $9C0D,4 Is time_bcd zero? Jump to time_up if so
C $9C11,4 time_up_state = 0
C $9C15,4 user_input_mask = $FF (allow all keys)
C $9C19,2 Loop back to #R$9BF9
@ $9C1B label=tick_time_up
C $9C1B,3 Point at "TIME UP" message
C $9C1E,3 Call setup_overlay_messages
C $9C21,6 Return if speed > 0
C $9C27,5 time_up_state = 2
C $9C2C,2 Index of "Your time's up" speech sample
C $9C2E,3 Exit via play_speech_hook
C $9C31,5 Return if transition_control is non-zero
C $9C36,8 Exit via quit_key if no credits remain
C $9C3E,1 Decrement credits [POKE $9C3E for Infinite credits]
@ $9C41 ssub=LD (credit_n + 7),A
C $9C3F,5 Turn it into ASCII and poke it into the "CREDIT x" string
C $9C44,5 time_up_state = 3
C $9C49,3 HL = $0115
C $9C4C,3 Self modify 'LD HL' @ #R$9C84 to load HL
C $9C4F,1 Return
C $9C50,5 Is fire pressed?
C $9C55,2 Jump if not (?)
N $9C57 Resetting mission code.
C $9C57,4 Reset time_up_state to zero
C $9C5B,5 Reset smash_factor to zero
C $9C60,2 Reset smash_counter to zero
C $9C62,5 Set user input mask to allow everything through
C $9C67,3 gear_lockout = 3
C $9C6A,3 transition_control = 3 -- Flag set to zero when attributes have been set
C $9C6D,3 turbos = 3
C $9C70,5 Set remaining time to 60 (BCD) [doesn't affect stuff if altered?!]
C $9C75,4 Increment retry_count
N $9C79 This entry point is used by the routines at #R$858C and #R$F220.
@ $9C79 label=play_start_noise
C $9C79,2 Index of start noise sample
C $9C7B,3 Exit via play_speech_hook
@ $9C7E label=tick_print_continue
C $9C7E,3 Print "CONTINUE THIS MISSION" messages
C $9C81,3 Call setup_overlay_messages
C $9C84,3 <self modified>
C $9C87,1 H--
C $9C8A,2 H = 6
C $9C8C,1 L--
C $9C8D,1 A = L
C $9C8F,3 Effect 8 (bip), Priority 1
C $9C94,1 Effect 9 (bow)
C $9C95,3 Call start_sfx
C $9C98,1 A = L
C $9C99,1 Set flags
C $9C9C,1 A++
C $9C9D,3 quit_state = A
C $9CA0,5 time_up_state = 4
C $9CA5,3 Self modify 'LD HL' @ #R$9C84 to load HL
C $9CA8,1 A = L
C $9CA9,2 A >>= 1
@ $9CAB ssub=LD DE,time_n + 5
C $9CAB,3 Address of nn in "TIME nn"
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
C $9CC2,3 Load speed into #REGhl
N $9CC5 This code makes little sense.
C $9CC5,1 Speed low byte
C $9CC6,2 Bottom bit of #REGh moves to carry (#REGh now unused)
C $9CC8,1 Merge carry into LSB of speed [Wrong end?]
C $9CC9,4 Divide by four
C $9CCD,1 [Needless move]
C $9CCE,2 Add carry in, perhaps for rounding?
C $9CD0,1 BCD correction
C $9CD1,3 Zero high part of score increment
C $9CD4,2 Exit via increment_score
c $9CD6 Add a bonus
D $9CD6 The bonus value is passed as five BCD digits in #REGde and #REGa like: 0bDDDDddddEEEEeeeeAAAAxxxxx, where x is not used.
D $9CD6 The bonus value must have a single sequence of zeroes, e.g. "55000" is okay, but "50500" is not.
D $9CD6 Used by the routines at #R$9D2E, #R$A637 and #R$B9F4.
R $9CD6 I:A Low nibble of bonus
R $9CD6 I:DE High four nibbles of bonus
@ $9CD6 label=bonus
C $9CD6,1 Preserve #REGa
C $9CD7,3 -> Byte after bonus digits buffer
C $9CDA,2 Flag, set to $FF while digits are zero
C $9CDC,1 Needless move
C $9CDD,3 Call bonus_digit  -- final digit first
C $9CE0,2 Top-bit terminate the bonus string
C $9CE2,1 Load lowest digit
C $9CE3,3 Call bonus_high_nibble
C $9CE6,1 Load middle digits
C $9CE7,3 Call bonus_digit
C $9CEA,1 Load middle digits
C $9CEB,3 Call bonus_high_nibble
C $9CEE,1 Load high digits
C $9CEF,3 Call bonus_digit
C $9CF2,1 Load high digits
C $9CF3,3 Call bonus_high_nibble
C $9CF6,2 Exit via bonus_exit
@ $9CF8 label=bonus_high_nibble
C $9CF8,4 Shift the high digit down
@ $9CFC label=bonus_digit
C $9CFC,2 Mask off low nibble
C $9CFE,2 If #REGa is not zero goto bonus_non_zero
N $9D00 Digit is zero.
C $9D00,4 Jump if the flag in #REGc is set
N $9D04 We saw a non-zero-to-zero transition, so terminate.
C $9D04,1 Discard return address
C $9D05,2 Exit via bonus_exit
@ $9D07 label=bonus_non_zero
C $9D07,2 Set flag to zero now we've seen a non-zero digit
@ $9D09 label=bonus_store
C $9D09,2 Turn digit to ASCII
C $9D0B,2 Write digit out in reverse
C $9D0D,1 Return
@ $9D0E label=bonus_exit
C $9D0E,3 Self modify 'LD HL,xxxx' @ #R$9D9B  -- bonus score pointer
C $9D11,5 Set the trigger_bonus_flag
C $9D16,1 A = B, then fall into increment_score with the bonus preserved
c $9D17 Increments the score by (D,E,A)
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
D $9D2E Bonus is 200 for each overtaken car, reset on crashes.
R $9D2E Used by the routine at #R$8401.
N $9D2E I:A Iterations (number of sequential overtakes to consider)
@ $9D2E label=calc_overtake_bonus
C $9D2E,5 If allow_overtake_bonus is zero then return
C $9D33,1 B = A -- iterations / no. of overtakes
C $9D34,3 Point #REGhl at overtake_bonus
N $9D37 Increment bonus by 2 up to a max of 128.
@ $9D37 label=cob_overtake_loop
C $9D37,3 A = *HL + 2
C $9D3A,1 BCD correction
C $9D3B,6 If A >= 128 then A = 128  [80 in BCD]
C $9D41,1 *HL = A
C $9D42,1 Bank
N $9D43 Set bonus to N * 100.
C $9D43,1 Set middle digits
C $9D44,1 Clear low digit
C $9D45,1 Clear top two digits
C $9D46,3 Call bonus
C $9D49,1 Unbank
C $9D4A,2 Loop while #REGb > 0
C $9D4C,4 Clear allow_overtake_bonus
C $9D50,1 Return
c $9D51 Update scoreboard and flashing lights
@ $9D51 label=bonus_string
T $9D51,6,5:n1 Bonus digits buffer
@ $9D57 label=hi
T $9D57,2,1:n1 "HI" gear string
@ $9D59 label=lo
T $9D59,2,1:n1 "LO" gear string
@ $9D5B label=stage
T $9D5B,6,6 "STAGE n" message shown in the score area
@ $9D61 label=stage_n
B $9D61,1,1 #R$9D61 writes the current stage number here in ASCII.
N $9D62 This entry point is used by the routines at #R$8401, #R$873C and #R$87DC.
@ $9D62 label=update_scoreboard
C $9D62,3 Check if stage has changed  -- perhaps reset to zero by stage loads?
C $9D65,3 Avoid work if previously updated
C $9D68,3 Load <wanted_stage_number>
C $9D6B,2 Add 48 to make it a digit then $80 to terminate the string
C $9D6D,6 Overwrite the N in "STAGE N"
C $9D73,3 Screen pixel (48,36)
C $9D76,3 Point at stage text
C $9D79,3 Draw it
@ $9D7C label=us_bonus_start
C $9D7C,3 Load <trigger_bonus_flag>
C $9D7F,3 If it's zero jump to attribute setting
C $9D82,4 Clear it
C $9D86,3 Screen pixel (64,24)
N $9D89 Clear the area - 5x7 bytes
C $9D89,2 7 iterations
C $9D8B,2 DE = HL
C $9D8D,1 Preserve column
C $9D8E,9 Update five bytes
C $9D97,1 Restore column
C $9D98,1 Advance row
C $9D99,2 Loop
C $9D9B,3 HL = <self modified>  -- load address of score digits
C $9D9E,3 Draw it
C $9DA1,2 Set bonus counter to 8
C $9DA3,2 Jump
@ $9DA5 label=us_bonus_countdown
C $9DA5,3 A = <bonus_counter>
C $9DA8,3 Jump if counter is zero
C $9DAB,1 Decrement the bonus counter
@ $9DAC label=us_bonus_set_counter
C $9DAC,3 <bonus_counter> = A
C $9DAF,3 Shift out LSB to carry
C $9DB2,2 Jump if #REGc is zero, using zero for attributes
C $9DB4,2 42 => BRIGHT + red over black
C $9DB6,2 Jump if odd
C $9DB8,2 Otherwise 46 => BRIGHT + yellow over black
C $9DBA,3 Point at attributes (8,3)
C $9DBD,6 Set them all to #REGc
@ $9DC3 label=us_gear
C $9DC3,3 Point HL at <displayed_gear>
C $9DC6,4 Compare to <gear>
C $9DCA,2 Avoid work if they're equal
C $9DCC,1 Update <displayed_gear>
C $9DCD,3 Screen pixel (118,36)
C $9DD0,3 Point HL at "LO"
C $9DD3,1 Low gear?
C $9DD4,2 Jump to draw string if so
C $9DD6,3 Point HL at "HI"
C $9DD9,3 Draw string
@ $9DDC label=us_lights
C $9DDC,4 C = sighted_flag
C $9DE0,3 A = <counter_B>
C $9DE3,3 If (<sighted> AND <counter_B>) is zero jump to plot_turbos_and_scores
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
c $9E11 Draws the turbo sprites and updates the displayed scores
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
C $9E35,3 Self modify #R$9E44 to load #REGsp with address of frame
C $9E38,4 Self modify #R$9E78 to restore #REGsp once drawing is done
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
C $9E78,3 Restore original #REGsp (self modified)
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
@ $9EC8 ssub=LD HL,time_digits + 1
C $9EC8,3 Point #REGhl at time_digits + 1
C $9ECB,2 One pair of digits
C $9ECD,3 Call plot_led_digits
N $9ED0 Distance.
@ $9ED0 ssub=LD DE,distance_bcd + 1
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
@ $9F07 ssub=LD DE,distance_bcd + 1
C $9F07,3 Point #REGde at distance_bcd + 1 (two BCD bytes)
@ $9F0A ssub=LD HL,distance_digits + 3
C $9F0A,3 Point #REGhl at distance_digits + 3
C $9F0D,2 Two pairs of digits (4 digits)
C $9F0F,3 Call ptas_led_digits
C $9F12,3 Point #REGde at score digits screen position
C $9F15,1 Bank
@ $9F16 ssub=LD DE,score_bcd + 3
C $9F16,3 Point #REGde at score (four BCD bytes)
@ $9F19 ssub=LD HL,score_digits + 7
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
C $9FF3,4 If 1 then jump #R$A0AD
C $9FF7,5 If 2 then jump #R$A083
C $9FFC,3 If 3 then jump #R$A03D -- double height
C $9FFF,3 If 4 then jump #R$A031 -- single height inverted
C $A002,2 If 5 then jump #R$A023 -- double height + inverted
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
B $A0D5,1,1 User input: QPBFUDLR - Quit Pause Boost Fire/Gear Up Down Left Right. Note that attract mode is driven through this var.
N $A0D6 This entry point is used by the routines at #R$8014, #R$8258, #R$8401, #R$858C, #R$8876 and #R$F220.
@ $A0D6 label=keyscan
C $A0D6,6 If not Kempston then goto #R$A0F3
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
@ $A139 label=mode_128k
B $A139,1,1 0/1 => 48K/128K mode
@ $A13A label=current_stage_number
B $A13A,1,1 Current stage number
@ $A13B label=start_speech_cycle
B $A13B,1,1 Used by #R$8424  -- seems to start at 4 then cycle 3/2/1 with each restart of the game, another random factor?
@ $A13C label=overtake_bonus
B $A13C,1,1 Overtake combo bonus counter. BCD. This increases by 2 for each overtake and is reset on a crash.
@ $A13D label=credits
B $A13D,1,1 Number of credits remaining (2 for a new game)
N $A13E Data copied to $A16D+.
@ $A13E label=saved_game_state
B $A13E,1,1 Copied to var_a16d
B $A13F,1,1 Copied to idle_timer
B $A140,1,1 Copied to user_input_mask
B $A141,1,1 Copied to turbos
W $A142,2,2 Copied to horizon_level
B $A144,1,1 Copied to perp_halt_counter
B $A145,1,1 Copied to displayed_gear
B $A146,8,8 Copied to score_digits
B $A14E,1,1 Copied to time_sixteenths
B $A14F,1,1 Copied to time_digits
B $A150,1,1
B $A151,4,4 Copied to distance_digits
B $A155,1,1
B $A156,1,1 Copied to no_objects_counter
W $A157,2,2 Copied to horizon_attribute
B $A159,1,1 Copied to hazards[0].0
B $A15A,1,1 Copied to hazards[0].1
B $A15B,1,1 Copied to hazards[0].2
B $A15C,1,1 Copied to hazards[0].3
B $A15D,1,1 Copied to hazards[0].4
B $A15E,1,1 Copied to hazards[0].5
B $A15F,1,1 Copied to hazards[0].6
B $A160,1,1 Copied to hazards[0].7
B $A161,1,1 Copied to hazards[0].8
W $A162,2,2 Copied to hazards[0].9 (car LOD)
W $A164,2,2 Copied to hazards[0].11 (== smash_handler)
W $A166,2,2 Copied to hazards[0].13 (horz pos)
B $A168,1,1 Copied to hazards[0].15
B $A169,1,1 Copied to hazards[0].16
B $A16A,1,1 Copied to hazards[0].17
B $A16B,1,1 Copied to hazards[0].18
B $A16C,1,1 Copied to hazards[0].19
N $A16D Oscillates when the road forks. Adjusts horizontal position of the untaken road.
@ $A16D label=var_a16d
B $A16D,1,1 Used by #R$BC36
@ $A16E label=idle_timer
B $A16E,1,1 Idle timer. Counts down from 100. Recommences whenever the hero car is stopped. When it hits zero Raymond will say "LET'S GET MOVIN' MAN!" and it will be reset to 100.
@ $A16F label=user_input_mask
B $A16F,1,1 User input mask, set to $C0 (Quit+Pause) when perp is fully smashed, or $FF otherwise
@ $A170 label=turbos
B $A170,1,1 Number of turbo boosts remaining (3 for a new game)
@ $A171 label=horizon_level
W $A171,2,2 seems to be the horizon level, possibly relative (used during attract mode)
@ $A173 label=perp_halt_counter
B $A173,1,1 This is set to 20 by #$87ED then decremented by #R$8BC6 which uses it. It's a counter decremented while slowing down to catch the perp.
@ $A174 label=displayed_gear
B $A174,1,1 Low/high gear flag.
@ $A175 label=score_digits
B $A175,8,8 Score digits. One digit per byte, least significant first. This seems to be recording what's on screen so digit plotting can be bypassed.
@ $A17D label=time_sixteenths
B $A17D,1,1 Seems to be a 1/16ths second counter. Counts from $F to $0. #R$A17E is decremented when it hits zero.
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
N $A188 Table of spawned vehicles and/or objects
N $A188 Each entry is 20 bytes long. The first entry is the perp.
N $A188 Hazard structure layout:
N $A188 +0 (byte) is $FF if this hazard is used, $00 otherwise
N $A188 +1 (byte) is the low byte of the distance from 'camera'. increments when the perp is getting further away, decrements otherwise.
N $A188 +2 (byte) is horizontal position
N $A188 +3 (byte) TBD
N $A188 +4 (byte) TBD
N $A188 +5 (byte) is horizontal position
N $A188 +6 (byte) TBD
N $A188 +7 (byte) TBD used by hazard_hit, used in plotting
N $A188 +8 (byte) gets copied from the hazards table
N $A188 +9 (word) address of LOD
N $A188 +11 (word) address of routine
N $A188 +13 (word) horizontal position, e.g. $190
N $A188 +15 (byte) TBD used by hazard_hit, counter which gets set to 2 then reduced
N $A188 +16 (byte) TBD
N $A188 +17 (byte) TBD used by hazard_hit, indexes table #R$ACDB
N $A188 +18 (byte) TBD used by hazard_hit
N $A188 +19 (byte) TBD used by hazard_hit
@ $A188 label=hazard_0
@ $A19C label=hazard_1
@ $A1B0 label=hazard_2
@ $A1C4 label=hazard_3
@ $A1D8 label=hazard_4
@ $A1EC label=hazard_5
S $A188,120,$14
N $A200 Unknown - not enough space for another hazard
S $A200,19,$13
N $A213 AY registers 0..11 [128K]
@ $A213 label=ay_chan_a_pitch
W $A213,2,2 0,1: Channel A pitch (fine,course=lo,hi)
@ $A215 label=ay_chan_b_pitch
W $A215,2,2 2,3: Channel B pitch
@ $A217 label=ay_chan_c_pitch
W $A217,2,2 4,5: Channel C pitch - used for engine tone?
@ $A219 label=ay_noise_pitch
B $A219,1,1 6: Noise pitch
@ $A21A label=ay_mixer
B $A21A,1,1 7: Mixer
@ $A21B label=ay_chan_a_vol
B $A21B,1,1 8: Channel A volume
@ $A21C label=ay_chan_b_vol
B $A21C,1,1 9: Channel B volume
@ $A21D label=ay_chan_c_vol
B $A21D,1,1 10: Channel C volume
@ $A21E label=ay_env_fine
B $A21E,1,1 11: Envelope fine duration
N $A21F End of AY registers
B $A21F,1,1 Unused?
@ $A220 label=dont_draw_screen_attrs
B $A220,1,1 If this is non-zero then draw_screen_attributes won't run.
N $A221 Affects collision detection on the left hand side.
@ $A221 label=inhibit_collision_detection
B $A221,1,1 #R$ABCE, #R$AD0D reads
@ $A222 label=n_hazards
B $A222,1,1 Seems to be a count of visible cars+hazards.
@ $A223 label=displayed_stage
B $A223,1,1 Stage number as shown on the scoreboard. Stored as ASCII.
N $A224 0 => no helicopter 1 => moves to left 2 => bobs around in the air 3/4 => moves from left 5 => bobs around in the air
@ $A224 label=helicopter_control
B $A224,1,1 Set to 1 -> helicopter moves out to the left, gets set to zero. Set to 3/4 -> Helicopter moves in from left, gets set to five. #R$AAC6, #R$AB33 reads  #R$AB96, #R$C013 writes
@ $A225 label=stop_car_spawning
B $A225,1,1 Inhibits cars from spawning.
@ $A226 label=correct_fork
B $A226,1,1 Holds the correct direction to take at forks (1 => left, 2 => right).
@ $A227 label=floating_arrow
B $A227,1,1 Shows the floating left/right arrow (0 => off, 1 => left, 2 => right).
@ $A228 label=cherry_light
B $A228,1,1 Shows the flashing cherry light on top of the car (0 => off, else on).
@ $A229 label=time_up_state
B $A229,1,1 1 => out of time, 2 => "TIME UP" message is printed; 3 => "CONTINUE THIS MISSION" message is printed and a countdown runs 4 => countdown elapsed; 0 otherwise
@ $A22A label=var_a22a
B $A22A,1,1 Used by #R$B6D6 and others
@ $A22B label=allow_overtake_bonus
B $A22B,1,1 Enables overtake bonus. Used by #R$9D2E and others.
@ $A22C label=trigger_bonus_flag
B $A22C,1,1 Bonus flag (1 triggers the effect)
@ $A22D label=bonus_counter
B $A22D,1,1 Counter for bonus flash effect (counts 8..0)
@ $A22E label=sighted_flag
B $A22E,1,1 Set to 1 when the perp has been sighted. Enables flashing lights and the smash bar.
@ $A22F label=hand_flag
B $A22F,1,1 Set to 1 by perp_sighted. This starts the anim where the cherry light is put on the roof. Set to 2 by fully_smashed. This shows the "stop" hand.
@ $A230 label=perp_caught_phase
B $A230,1,1 Set to > 0 by fully_smashed when the perp has been caught. #R$8A57 progresses this through stages 1..6 until the cars are slowed to a halt and the bonuses are printed. Used by draw_screen. Inhibits car spawning.
@ $A231 label=transition_control
B $A231,1,1 Transition/fade control/counter. Set to zero when fill_attributes has run. Set to 4 while transitioning. Also 2 sometimes. Set to 1 when the perp has been caught and we're drawing mugshots.
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
@ $A237 label=sfx_index
B $A237,1,1 Current sound effect index
@ $A238 label=sfx_priority
B $A238,1,1 Current sound effect priority
@ $A239 label=var_a239
B $A239,1,1 Used by #R$F265 [128K]  Siren related.
@ $A23A label=var_a23a
B $A23A,1,1 Used by #R$F2F6 [128K]  Copy of noise pitch.
@ $A23B label=tunnel_sfx
B $A23B,1,1 Set to 5 when we're in a tunnel. Used to modulate sfx.
@ $A23C label=var_a23c
B $A23C,1,1 #R$8909, #R$BE28 reads  #R$A3FA, #R$BDFF, #R$BE2C writes
@ $A23D label=var_a23d
B $A23D,1,1 #R$BE33 reads  #R$890F, #R$A3D2, #R$BDFC, #R$BE37 writes
@ $A23E label=off_road
B $A23E,1,1 0 => Fully on-road, 1 => One wheel off-road, 2 => Both wheels off-road [samples: #R$B104, #R$B40C reads  #R$A3FD, #R$A52A, #R$B07D, #R$B322, #R$B404, #R$B443 writes]
@ $A23F label=fast_counter
B $A23F,1,1 This is a very fast counter related to level position. Typically the top three bits are masked off and used to index other tables.
@ $A240 label=road_buffer_offset
W $A240,2,2 Current offset (in bytes) into the road buffer at $EE00..$EEFF. Much of the code treats this as a byte. Used by #R$B8F6, #R$BBF7, #R$87E0.
@ $A242 label=curvature_byte
B $A242,1,1 Current curvature byte (minus 16) [#R$BE3E reads #R$BC17,#R$BE7D writes]
@ $A243 label=height_byte
B $A243,1,1 Current height byte (minus 16) [#R$BE90 reads #R$BC1A,#R$BECF writes]
@ $A244 label=leftside_byte
B $A244,1,1 [#R$BF9E reads #R$BC1D,#R$BFDD writes]
@ $A245 label=rightside_byte
B $A245,1,1 Current rightside byte [#R$BF55 reads #R$BC20,#R$BF94 writes]
@ $A246 label=hazards_byte
B $A246,1,1 Current hazards byte (minus one) [#R$BFE7 reads #R$BC23,#R$C055 writes]
@ $A247 label=lanes_counter_byte
B $A247,1,1 [#R$BEDB reads #R$BC26,#R$BF15,#R$BF29 writes]
@ $A248 label=on_dirt_track
B $A248,1,1 Set to 1 when on dirt track. [#R$A955 reads #R$C457,#R$C526 writes]
@ $A249 label=fork_taken
B $A249,1,1 Set to 0 if no fork, or the left fork was taken, or 1 if the right fork was taken. [$A539,$B988,$BAE0,$BB6E reads #R$BA89,#R$BC2C writes]
@ $A24A label=speed
W $A24A,2,2 Speed (0..511). Max when in low gear =~ $E6, high gear =~ $168, turbo =~ $1FF.
@ $A24C label=var_a24c
B $A24C,1,1 #R$B1B7 reads  #R$B1E4 writes
@ $A24D label=cornering
B $A24D,1,1 Likely a cornering force flag. Used to trigger smoke. #R$B3B4, #R$B432 reads  #R$B2E5, #R$B314, #R$B32A writes
@ $A24E label=boost
B $A24E,1,1 Turbo boost time remaining (60..0)
@ $A24F label=smoke
B $A24F,1,1 Smoke time remaining. This is set to 4 on low-to-high gear changes and to 3 when the hero car lands after a jump. This isn't set for turbo boosts however.
@ $A250 label=turn_speed
B $A250,1,1 Turn speed (0/1/2 => straight/turn/turn hard) ignoring direction
@ $A251 label=flip_car
B $A251,1,1 1 => Horizontally flip the hero car, 0 => Don't
@ $A252 label=gear_lockout
B $A252,1,1 Locks out repeated gear changes. Counts down 3-2-1-0
@ $A253 label=gear
B $A253,1,1 0 => Low gear, 1 => High gear
@ $A254 label=allow_spawning
B $A254,1,1 If non-zero this permits cars, hazards and dust/stones to spawn. It can be 0, 1 or 2, depending on the hero car's speed. Returns to zero when the hero car is stopped.
@ $A255 label=distance_bcd
B $A255,2,2 Distance as BCD (2 bytes / 4 digits, little endian)
@ $A257 label=var_a257
B $A257,1,1 Unused
@ $A258 label=var_a258
B $A258,1,1 Used by #R$B8D8
@ $A259 label=var_a259
B $A259,1,1 Used by #R$B92B
@ $A25A label=var_a25a
B $A25A,1,1 Used by #R$B8D2
@ $A25B label=var_a25b
B $A25B,1,1
@ $A25C label=current_curvature
B $A25C,1,1 This holds the road curvature byte at the position of the hero car. -ve when curving left or +ve when curving right. $FA..$06 in multiples of two.
@ $A25D label=horizon_a25d
B $A25D,1,1 Seems to be added to the index for the b828 table. Saw: 8/16/24.
@ $A25E label=horizon_a25e
B $A25E,1,1 Seems to cycle 4-3-2-1 / 3-2-1 / 2-1 when the roads are curving. Must be the horizon scroll/shift/roll value.
@ $A25F label=var_a25f
W $A25F,2,2 Repeatedly set to zero in mhc_straight_road. If altered this changes the car's position on the road.
@ $A261 label=var_a261
B $A261,1,1 Used by #R$B297
@ $A262 label=var_a262
B $A262,1,1
@ $A263 label=var_a263
B $A263,1,1 Used by #R$B2AE
@ $A264 label=var_a264
B $A264,1,1 Used by #R$B2B2 -- together might be a 16-bit qty
@ $A265 label=fork_visible
B $A265,1,1 Used by #R$BC2C -- set to $60 when the forked road becomes visible, zero otherwise
@ $A266 label=fork_countdown
B $A266,1,1 Used by #R$A3A6 -- counts down (from 16?) when the forked road approaches, zero when the fork actually starts
@ $A267 label=fork_distance
W $A267,2,2 Counts 0..255 during road forks
@ $A269 label=fork_in_progress
B $A269,1,1 Used by #R$BC29. Set to 1 while the road forks.
@ $A26A label=quit_state
B $A26A,1,1 0 if not quitting, or 1/2 depending on quit state
@ $A26B label=start_speech
B $A26B,1,1 Index of speech sample to play preceded by N zero bits and a one bit.
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
@ $A27A label=font
B $A27A,7,7 Exclamation mark
B $A281,7,7 Open bracket
B $A288,7,7 Close bracket
B $A28F,7,7 Comma
B $A296,7,7 Full stop
B $A29D,70,7 0..9
B $A2E3,182,7 A-Z
c $A399 Checks for collisions
D $A399 Used by the routines at #R$8401 and #R$852A.
@ $A399 label=check_collisions
C $A399,3 HL = $0048
C $A39C,3 DE = $01D8
C $A3A0,3 A = fork_visible
C $A3A3,1 Set flags
C $A3A4,2 Jump to cc_at_fork if zero
C $A3A6,3 A = fork_countdown
C $A3A9,1 Set flags
C $A3AA,3 Jump to cc_not_fork if zero
C $A3AD,1 A--
C $A3AE,1 Return if zero  [can't collide immediately before fork?]
N $A3AF Check left hand side of car.
@ $A3AF label=cc_at_fork
C $A3AF,3 HL = *$EAFE
C $A3B2,1 A = H (0 or 255)
C $A3B3,1 Set flags
C $A3B4,2 Jump to #R$A3D1 if non-zero
C $A3B6,9 Jump to #R$A3D1 if HL < $40
C $A3BF,8 Jump to #R$A3D5 if HL < $6A
C $A3C7,4 HL -= $85
C $A3CB,1 A++  -- A == 1 => partially off-road
C $A3CC,2 Jump to cc_fork_set if A was 255
C $A3CE,1 A++  -- A == 2 => fully off-road
C $A3CF,2 Jump to cc_fork_set
@ $A3D1 label=cc_a3d1
C $A3D1,4 var_a23d = 0
N $A3D5 Check right hand side of car.
@ $A3D5 label=cc_a3d5
C $A3D5,3 HL = *$EAFC
C $A3D8,1 A = H
C $A3D9,1 Set flags
C $A3DA,2 A = 0  (not self modified)
C $A3DC,2 Jump to #R$A3FA if non-zero
C $A3DE,9 Jump to #R$A3FA if HL >= $BE
C $A3E7,9 Jump to #R$A3FD if HL >= $8E
C $A3F0,4 HL -= $7C
C $A3F4,1 A++  -- A == 1 => partially off-road
C $A3F5,2 Jump to cc_fork_set if no carry
C $A3F7,1 A++  -- A == 2 => fully off-road
C $A3F8,2 Jump to cc_fork_set
@ $A3FA label=cc_a3fa
C $A3FA,3 var_a23c = A
@ $A3FD label=cc_fork_set
C $A3FD,3 off_road = A  -- 0/1/2 => on-road/one wheel off-road/both wheels off-road
C $A400,1 Set flags
C $A401,2 C = 0  (not self modified)
C $A403,2 not off road?
C $A405,3 Load road_buffer_offset into #REGa
C $A408,2 Add 64 so it's the lanes data offset
C $A40A,3 Point #REGhl at road buffer lanes data
C $A40D,1 A = *HL
C $A40E,2 Test bit 6  -- tunnel bits perhaps?
C $A410,2 Jump if clear
C $A412,1 Test bit 7
C $A413,2 Jump if set
C $A415,2 Test bit 3 (was bit 2 before RLA)
C $A417,3 A = road_pos.hi
C $A41A,2 Jump if clear
C $A41C,1 C = A
C $A41D,2 A = 20
C $A420,3 A = C & 1
C $A423,3 Jump to cc_hit_scenery2
@ $A426 label=cc_a426
C $A426,3 HL = $00D1
C $A429,3 DE = $0195
C $A42D,1 Set flags [A is ?]
C $A42E,2 C = 1
C $A432,1 C = 2
@ $A433 label=cc_hit_tunnel_wall
C $A433,1 Preserve BC
C $A434,3 Effect 6 (tunnel wall hit), Priority 4
C $A437,3 Call start_sfx
C $A43A,1 Restore BC
@ $A43B label=cc_a43b
C $A43B,1 A = C
C $A43C,3 ($B3DC) = A
C $A440,3 ($B396) = HL
C $A443,4 ($B3A4) = DE
C $A447,1 Set flags
C $A448,1 Return if non-zero
C $A449,3 Load road_buffer_offset into #REGa
N $A44C -- RIGHT SIDE OBJECT HIT CHECKING --
C $A44C,2 Add 96 so it's the right side objects data offset
C $A44E,1 Point #REGhl at road buffer right side objects data
C $A44F,1 Bank road buffer offset
C $A450,2 (cont.)
C $A452,3 Load road_buffer_offset into #REGa
C $A455,1 Top bit -> carry
C $A456,1 Read a right side object data byte
C $A457,2 Jump if top bit was clear
C $A459,1 L++  -- so if the top bit of the road buffer offset is set we take next byte?
@ $A45A label=cc_a45a
C $A45A,1 OR in the (next) byte
C $A45B,2 Jump if it's zero -- no object
N $A45D Build a graphics_defs address.
C $A45D,12 HL = (*#R$5CFA)[A * 7]
N $A469 Read collision values.
C $A469,2 C = *HL++  -- e.g. $5E5B. a higher value
C $A46B,2 E = *HL++  -- e.g. $5E5C. a lower value
C $A46D,1 A = *HL     -- e.g. $5E5D. TBD
C $A46E,1 D = B       -- B is zero here, so BC = C and DE = E
N $A46F Check for collisions with scenery (right hand side).
C $A46F,3 HL = *$EAFC  -- near end of road drawing words (centre)... must be position of car
C $A472,6 Jump to cc_a480 if HL >= BC
C $A478,4 Jump to cc_a480 if HL < DE
C $A47C,1 Unbank road buffer offset
C $A47D,1 A = 0  -- perhaps a right hand flag
C $A47E,2 Jump to cc_hit_scenery
@ $A480 label=cc_a480
C $A480,1 Unbank road buffer offset
N $A481 -- LEFT SIDE OBJECT HIT CHECKING --
C $A481,2 Add 32 so it's the left side objects data offset
C $A483,3 Point #REGhl at road buffer left side objects data
C $A486,3 Load road_buffer_offset into #REGa
C $A489,1 Top bit -> carry
C $A48A,1 Read a left side object data byte
C $A48B,2 Jump if top bit was clear
C $A48D,1 L++
@ $A48E label=cc_a48e
C $A48E,1 OR in the (next) byte
C $A48F,1 Return if zero -- no object
C $A490,12 HL = (*#R$5D00)[A * 7]
N $A49C Read collision values.
C $A49C,2 C = *HL++
C $A49E,2 E = *HL++
C $A4A0,1 A = *HL
C $A4A1,1 D = B  must be zero?
N $A4A2 Check for collisions with scenery (left hand side).
C $A4A2,3 HL = *$EAFE  -- suspected position of car
C $A4A5,5 Return if HL < BC
C $A4AA,3 Return if HL >= DE
C $A4AD,1 Unbank road buffer offset
C $A4AE,2 A = 1  -- perhaps a left hand flag
N $A4B0 Arrive here if hit scenery, e.g. drove a tree or a lamp post.
@ $A4B0 label=cc_hit_scenery
C $A4B0,1 Preserve AF  suspected flag
C $A4B1,3 Effect 4 (scenery crash), Priority 3
C $A4B4,3 Call start_sfx
C $A4B7,1 Restore AF
N $A4B8 This entry point is used by the routines at #R$A637 and #R$A8CD.
@ $A4B8 label=cc_hit_scenery2
C $A4B8,3 HL = &<crashed flag>
C $A4BB,2 Set flags
C $A4BD,1 Return if already crashed
C $A4BE,2 Set crashed flag
C $A4C0,4 *$B36F = A++
C $A4C4,5 *$B38E = A
C $A4C9,3 *$B385 = 5
C $A4CC,3 Load speed into #REGhl
C $A4CF,1 Preserve HL
C $A4D0,2 H >>= 1
C $A4D2,1 A = L
C $A4D5,8 A = (A << 3) + 16
C $A4DD,2 L = 24
C $A4E2,1 L = A
@ $A4E3 label=cc_a4e3
C $A4E3,3 *$B357 = HL
C $A4E7,3 HL = A
C $A4EC,2 HL -= DE
@ $A4F2 label=cc_a4f2
C $A4F2,3 Self modify 'LD BC' @ #R$B32E to load HL
C $A4F5,1 Return
@ $A4F6 label=cc_not_fork
C $A4F6,3 HL = *$E8FE  -- checking the final word of the road drawing (left) table
C $A4F9,1 A = H  (0 or 255)
C $A4FA,1 Set flags
C $A4FB,2 Jump to cc_a510 if A isn't zero
C $A4FD,9 Jump to #R$A510 if HL < $6A
C $A506,4 HL -= $85
C $A50A,1 A++  -- A == 1 => partially off-road
C $A50B,2 Jump to cc_not_fork_set if HL < $85
C $A50D,1 A++  -- A == 2 => fully off-road
C $A50E,2 Jump to cc_not_fork_set
@ $A510 label=cc_a510
C $A510,3 HL = *$EDFC
C $A513,1 A = H
C $A514,1 Set flags
C $A515,2 A = 0
C $A517,2 JR NZ,cc_not_fork_set
C $A519,9 Jump to cc_not_fork_set if HL >= $8E
C $A522,4 HL -= $7C
C $A526,1 A++  -- A == 1 => partially off-road
C $A527,2 Jump to cc_not_fork_set if HL >= $7C
C $A529,1 A++  -- A == 2 => fully off-road
N $A52A Set the off-road flag etc.
@ $A52A label=cc_not_fork_set
C $A52A,3 off_road = A
C $A52D,1 A = 0
C $A52E,3 *$B3DC = A
C $A532,3 *$B396 = HL
C $A535,4 *$B3A4 = DE
C $A539,6 Jump to #R$A55C if fork_taken was 1 (right fork taken)
C $A53F,3 HL = *$5CFC
C $A542,2 C = *HL++  -- max object boundary
C $A544,2 E = *HL++  -- min object boundary
C $A546,1 A = *HL
C $A547,2 B = 0
C $A549,1 D = B
C $A54A,3 HL = *$EAFE
C $A54D,5 Return if HL >= BC
C $A552,3 Return if HL < DE
C $A555,2 A = $8C
C $A558,1 A = 0
C $A559,3 Exit via cc_hit_scenery
@ $A55C label=cc_a55c
C $A55C,3 HL = *$5D02
C $A55F,2 C = *HL++
C $A561,2 E = *HL++
C $A563,2 B = 0
C $A565,1 D = B
C $A566,3 HL = *$EAFC
C $A569,5 Return if HL < BC
C $A56E,3 Return if HL >= DE
C $A571,2 A = $8C
C $A573,1 Bank
C $A574,2 A = 1
C $A576,3 Exit via cc_hit_scenery
c $A579 Lays out roadside objects
D $A579 Used by the routines at #R$8401, #R$852A and #R$873C.
@ $A579 label=layout_objects
C $A579,3 Address of object_positions
C $A57C,2 21 iterations [max no of objects on-screen?]
N $A57E Turn a run of sizes into accumulating values.
C $A57E,1 total = 0
@ $A57F label=lo_loop1
C $A57F,1 total += *HL
C $A580,2 *HL++ = total
C $A582,2 Loop lo_loop1 while #REGb > 0
C $A584,4 Save #REGsp to restore on exit (self modify)
C $A588,3 SP = $EB00
C $A58B,2 Point #REGde at road buffer (somewhere)
C $A58D,3 Load road_buffer_offset into #REGa
C $A590,2 Add 64 so it's the lanes data offset
C $A592,1 E = calculated offset
C $A593,4 Address of object_positions
C $A597,2 B = 21
C $A599,3 A = fork_visible
C $A59C,1 Set flags
C $A59D,3 Jump to lo_loop2 if zero [not in forked road]
C $A5A0,3 A = fork_countdown
C $A5A3,1 Set flags
C $A5A4,2 [not about to fork the road]
C $A5A6,1 B = A
@ $A5A7 label=lo_loop2
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
C $A5BD,2 Jump to lo_a5da
C $A5BF,3 H = A + $E7
N $A5C2 Sampled HL = $E869 $E865 $E863 $E861 (road drawing left)
C $A5C2,1 B = *HL
C $A5C3,1 L--
C $A5C4,1 C = *HL
C $A5CF,2 A = 3
C $A5D3,2 A = 3
C $A5D7,1 A--
C $A5D8,2 H += A
@ $A5DA label=lo_a5da
C $A5DA,1 C = *HL
C $A5DB,1 L++
C $A5DC,1 B = *HL
C $A5DF,2 IY++
C $A5E1,1 E++
C $A5E2,2 Loop to lo_loop2 while #REGb > 0
C $A5E4,3 A = fork_countdown
C $A5E7,1 Set flags
C $A5E8,3 Jump to lo_return if zero
C $A5EB,6 A = ~fork_countdown + 22
C $A5F1,2 Jump to lo_return if zero
C $A5F3,1 B = A
C $A5F4,2 H = $EB
@ $A5F6 label=lo_loop3
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
C $A608,2 Loop lo_loop3 while #REGb > 0
@ $A60A label=lo_return
C $A60A,3 Restore original stack pointer (self modified above)
C $A60D,1 Return
c $A60E Counters
D $A60E Used by the routines at #R$8401 and #R$852A.
@ $A60E label=cycle_counters
C $A60E,8 Cycle #R$A234 0-1-2-3
C $A616,5 Cycle #R$A235 0-1
C $A61B,1 Return if it's zero
C $A61C,6 Cycle #R$A236 0-1-2-3 at half rate
C $A622,1 Return
b $A623 Hazard template
D $A623 $A845 copies these 20 bytes to hazards array.
@ $A623 label=hazard_template
B $A623,1,1 Used flag
B $A624,8,8
W $A62C,2,2
W $A62E,2,2 Routine at #R$A8CD
W $A630,2,2
B $A632,5,5
c $A637 Smash/chase handling
D $A637 This gets called whenever the perp is within sight of the hero car.
@ $A637 label=smash_handler
C $A637,5 Return if perp_caught_phase > 0
C $A63C,7 Call perp_sighted if sighted_flag is zero
C $A643,3 A = IX[7]
C $A646,1 Set flags
C $A647,2 Jump if zero
C $A649,3 Jump if positive
N $A64C Otherwise A is negative.
C $A64C,3 IX[7]++
C $A64F,1 Return if non-zero
N $A650 A must be zero to arrive here.
@ $A650 label=sh_a650
C $A650,2 Preserve IY
C $A652,3 C = IX[1]
C $A655,2 5 iterations
C $A657,4 IY = &hazards[1]
C $A65B,3 DE = 20  -- stride of hazards
@ $A65E label=sh_hazards_loop
C $A65E,6 If the hazard is active then jump to #R$A66C
@ $A664 label=sh_a664
C $A664,2 Move to next hazard
C $A666,2 Loop while #REGb
C $A668,2 Restore IY
C $A66A,2 Jump to #R$A68F
@ $A66C label=sh_a66c
C $A66C,3 A = IY[15]
C $A66F,1 Check top bit
C $A670,3 Jump if clear
C $A673,4 A = IY[1] - C
C $A677,3 Jump to sh_a67e if IY[1] > c
C $A67A,2 A += 2
C $A67C,2 Jump to sh_a680
@ $A67E label=sh_a67e
C $A67E,2 A -= 3
@ $A680 label=sh_a680
C $A682,3 A = IY[17]
C $A685,3 Compare to IX[18]
C $A68B,2 Restore IY $A650 pushed
C $A68D,2 Jump to sh_a6d8
@ $A68F label=sh_a68f
C $A68F,2 A = <self modified>
C $A691,3 Jump if non-zero
C $A694,3 A = IX[1]
C $A697,4 Jump if A >= 7
N $A69B I'm failing to understand what the following section does. It's a countdown that, when it hits zero, picks a new random countdown value summed with smash_5d1b. I can only think that it's a delay loop.
N $A69B In-place decrementing counter.
C $A69B,3 A = <self modified> - 1  -- Self modified below
C $A69E,2 Jump to sh_update_counter if non-zero
N $A6A0 When it hits zero we pick a random number...
C $A6A0,1 [C incremented but overwritten in a moment - no effect?]
C $A6A1,6 C = rng() & 31
N $A6A7 This gets hit at some point during the smash process.
C $A6A7,4 A = smash_5d1b + C
@ $A6AB label=sh_update_counter
C $A6AB,3 Self modify 'LD A,x' at #R$A69B (above) to load A
N $A6AE This smells like it's detecting position and turning that into lanes. The values are like those used by get_spawn_lanes.
C $A6AE,8 HL = road_pos - 164
C $A6B6,3 BC = $0404
C $A6B9,2 Jump to #R$A6CF if HL < 164
C $A6BB,2 DE = $0046
C $A6BD,1 BC = $0304
C $A6BE,2 HL -= DE  -- includes previous
C $A6C0,2 Jump to #R$A6CF if HL < 70
C $A6C2,2 BC = $0203
C $A6C4,2 HL -= DE  -- includes previous
C $A6C6,2 Jump to #R$A6CF if HL < 70
C $A6C8,2 BC = $0102
C $A6CA,2 HL -= DE  -- includes previous
C $A6CC,2 Jump to #R$A6CF if HL < 70
C $A6CE,1 BC = $0101
@ $A6CF label=sh_a6cf
C $A6CF,3 A = IX[18]
C $A6D2,3 Jump to sh_a6d8 if A == C
C $A6D5,3 Jump to sh_a6f6 if A == B
@ $A6D8 label=sh_a6d8
C $A6D8,3 C = IX[18]
C $A6DB,3 Call rng
C $A6DE,1 C++
C $A6DF,4 Jump if C positive
C $A6E3,2 C -= 2
@ $A6E5 label=sh_a6e5
C $A6E5,1 A = C
C $A6E6,1 Set flags
C $A6E7,2 C = 2
C $A6E9,3 Jump if A is zero
C $A6EC,4 Jump if A < 5
C $A6F0,2 C = $FE
@ $A6F2 label=sh_a6f2
C $A6F2,1 A += C
@ $A6F3 label=sh_a6f3
C $A6F3,3 IX[18] = A
@ $A6F6 label=sh_a6f6
C $A6F6,3 C = IX[1]
C $A6F9,3 Call get_spawn_lanes
C $A6FC,3 A = IX[18]
C $A6FF,3 Jump to sh_a707 if A >= B  -- upper boundary?
C $A702,2 A += 2
C $A704,3 IX[18] = A
@ $A707 label=sh_a707
C $A707,5 Jump to #R$A711 if A <= C  -- lower boundary?
C $A70C,2 A -= 2
C $A70E,3 IX[18] = A
@ $A711 label=sh_a711
C $A711,3 A = IX[18]
@ $A714 ssub=LD HL,table_a7e7 - 1
C $A714,3 HL = $A7E6 -> #R$A7E7 data block
C $A717,2 L += A
C $A719,3 A = IX[5]
C $A71C,1 A == *HL ?
C $A71D,2 C = 1
C $A71F,2 Jump if A == *HL
C $A721,2 Jump if A < *HL
C $A723,2 A -= 10
C $A725,2 Jump if A was < 10
C $A727,1 A == *HL ?
C $A728,2 Jump if A >= *HL
@ $A72A label=sh_a72a
C $A72A,1 C--
C $A72B,1 A = *HL
C $A72C,2 Jump sh_a737
@ $A72E label=sh_a72e
C $A72E,2 A += 10
C $A730,2 Jump if A+10 carried
C $A732,1 A == *HL ?
C $A733,2 Jump if A < *HL
@ $A735 label=sh_a735
C $A735,1 C--
C $A736,1 A = *HL
@ $A737 label=sh_a737
C $A737,3 IX[5] = A
C $A73A,4 Self modify 'LD A' @ #R$A68F to load C
C $A73E,2 A = <self modified>
C $A740,1 Set flags
C $A741,3 DE = $1E
C $A744,3 HL = $E6
C $A747,2 Jump sh_a762 if non-zero
N $A749 Countdown+rng stuff again... as at #R$A69B
N $A749 In-place decrementing counter.
C $A749,3 A = <self modified> - 1  -- Self modified below
C $A74C,3 Self modify 'LD A,x' @ #R$A749 (above) to load A
C $A74F,2 Jump to sh_a776 if non-zero
N $A751 When it hits zero we pick a random number...
C $A751,1 Preserve HL [what's in it?]
C $A752,3 Call rng
C $A755,1 Restore HL
C $A756,3 C = (result of rng) & 15
C $A759,4 A = smash_5d1c + C
C $A75D,3 Self modify 'LD A,x' @ #R$A749 (above) to load A
C $A760,2 A = 10
@ $A762 label=sh_a762
C $A762,1 A--
C $A763,3 Self modify 'LD A,x' @ #R$A73E (above) to load A
C $A766,2 Jump to sh_a776 if zero
N $A768 sampled IX = $A188 (hazards)
C $A768,3 A = IX[1]  -- load hazard_1 distance byte
C $A76B,4 Jump if A >= 13 -- too far
N $A76F Distance to perp is 12 or less.
N $A76F HL += (15 - A) * DE
N $A76F This seems to be using the distance to the perp as a scale by which to adjust its horizontal position.
C $A76F,4 B = (15 - A)  -- iterations
@ $A773 label=sh_a773_loop
C $A773,1 HL += DE
C $A774,2 Loop to loop_a773 while #REGb > 0
@ $A776 label=sh_a776
C $A776,5 A = IX[1] - 6  -- load hazard_1 distance byte again
C $A77B,2 Jump to sh_store_exit if A >= 6
N $A77D Distance to perp is 5 or less.
C $A77D,2 Put back most of what we just subtracted
C $A77F,3 Multiply by 8
N $A782 And then we do nothing with #REGa?
C $A782,1 HL += DE
@ $A783 label=sh_store_exit
C $A783,6 wordat(IX + 13) = HL  -- store horizontal position
C $A789,1 Return
@ $A78A label=sh_a78a
C $A78A,4 IX[7] = $FC
C $A78E,5 Jump if A < 3  -- preserve A
C $A793,2 A -= 3  -- 0..
@ $A795 label=sh_check_boost
C $A795,1 Bank
C $A796,3 Load turbo boost time remaining (60..0)
C $A799,1 Set flags
C $A79A,2 200 when not turbo boosting
C $A79C,2 Jump if no turbo boost
C $A79E,2 230 when turbo boosting
@ $A7A0 label=sh_a7a0
C $A7A0,1 Bank value chosen from boost; Unbank other
C $A7A1,3 Call cc_hit_scenery2
C $A7A4,3 Read #REGhl from 'LD BC,x' @ #R$B32E
C $A7A7,4 HL += 40
C $A7AB,3 Self modify 'LD BC,x' @ #R$B32E
C $A7AE,2 D = 0
C $A7B0,1 -- restore A which holds the IX[7] flags from earlier AND FLAGS TOO
C $A7B1,3 HL = #R$B4F0  == smash routine
C $A7B4,1 Preserve HL or push addr to stack?
C $A7B5,2 Jump if no carry
C $A7B7,4 Jump if A == 2
C $A7BB,1 Push smash again?
N $A7BC Break?
C $A7BC,2 D = 4
@ $A7BE label=sh_a7be
C $A7BE,5 D = wanted_stage_number + D
C $A7C3,2 E = 0
C $A7C5,6 Jump if retry_count is zero
C $A7CB,1 Middle digit(s) of bonus
C $A7CC,1 Set top two digits of bonus
C $A7CD,4 Move middle digit into position
C $A7D1,1 Set middle digits of bonus
@ $A7D2 label=sh_retry_was_zero
C $A7D2,1 Clear low digits of bonus
C $A7D3,3 Call bonus
C $A7D6,2 A = 5
C $A7D8,3 Self modify 'LD A' @ #R$A73E to load A
C $A7DB,3 Point #REGhl at smash_chatter ("BEAR DOWN" / "OH MAN" / etc.)
C $A7DE,3 Call start_chatter (priority 5)
C $A7E1,3 Effect 3 (car crash), Priority 1
C $A7E4,3 Exit via start_sfx
b $A7E7 Data block at A7E7
@ $A7E7 label=table_a7e7
B $A7E7,12,8,4
c $A7F3 Spawns cars
D $A7F3 Used by the routines at #R$8401 and #R$852A.
@ $A7F3 label=spawn_cars
C $A7F3,9 Return if perp_caught_phase or stop_car_spawning flags are set
C $A7FC,3 Load allow_spawning
C $A7FF,2 Return if allow_spawning was zero
C $A801,4 A = <self modified> - allow_spawning
C $A805,3 Self modify above
C $A808,1 Return if carry?
N $A809 Start spawning cars.
C $A809,3 Call rng
C $A80C,3 C = A & 15
C $A80F,3 Load sighted_flag
C $A812,1 Set flags
C $A813,3 Load car_spawn_rate
C $A816,2 Jump to #R$A81A if sighted_flag was zero
C $A818,2 A += 25  -- boost factor/rate when perp sighted
C $A81A,1 A += C  -- add random factor (0..15)
C $A81B,3 Self modify above
C $A81E,3 BC = $0500
C $A821,4 IX = &hazards[1]
C $A825,3 DE = 20  -- stride of hazards
@ $A828 label=sc_loop
C $A828,6 If the hazard is not active jump to sc_fill_in
C $A82E,3 A = IX[15]
C $A837,2 IX += DE
C $A839,2 Loop to sc_loop while #REGb > 0
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
@ $A85F ssub=LD HL,table_a7e7 - 1
C $A85F,8 A = $A7E6[A]
C $A867,3 IX[5] = A
C $A86A,3 Load sighted_flag
C $A86D,1 Set flags
C $A86E,2 A = 4
C $A872,1 A <<= 1
C $A873,2 HL += A
C $A875,4 IX[13] = *HL
C $A879,3 Call rng
C $A87C,3 C = A & 6
C $A87F,3 Load sighted_flag
C $A882,1 Set flags
C $A885,2 A = 6
C $A88A,2 C--
C $A88C,2 B = 0
C $A88E,3 HL = &lods_vehicles (first of the generic car LODs)
C $A891,1 HL += BC
C $A892,9 wordat(IX+9) = HL
C $A89B,1 Return
c $A89C Returns the lanes that cars can spawn in
D $A89C Return $0101 and cars only appear in the leftmost lane. $0102 (1st and 2nd, mainly 2nd). $0104 first three lanes. $0304 - third and fourth lanes.
R $A89C I:C Buffer offset
R $A89C O:BC TBD
@ $A89C label=get_spawn_lanes
C $A89C,6 A = [current buffer offset] + 66 + C  (wrapping around)
C $A8A2,3 Point #REGde at road buffer lanes data (+2 bytes)
C $A8A5,1 Read lanes byte
C $A8A6,1 Set flags
C $A8A7,3 BC = $0104
C $A8AA,1 Return if zero
C $A8AB,1 E = A  -- preserve E
C $A8AC,2 A &= $C1
C $A8AE,3 Return if A == $C1
C $A8B1,2 CP $41
C $A8B3,3 BC = $0103
C $A8B6,1 Return if zero
C $A8B7,4 A = (E & $82) << 1
C $A8BB,2 Jump to #R$A8C5 if didn't carry
C $A8BD,3 BC = $0204
C $A8C0,1 Return if non-zero
C $A8C1,3 BC = $0103
C $A8C4,1 Return
@ $A8C5 label=gsl_a8c5
C $A8C5,3 BC = $0102
C $A8C8,1 Return if zero
C $A8C9,3 BC = $0304
C $A8CC,1 Return
c $A8CD Hazard handler routine? Triggered at road fork
@ $A8CD label=hazard_handler_a8cd
C $A8CD,6 Return if perp_caught_phase > 0
C $A8D3,6 Check stop_car_spawning flag
C $A8D9,4 IX[14] = $01  -- horizontal position
C $A8DD,4 IX[13] = $FF
C $A8E1,3 C = IX[1]  -- buffer offset
C $A8E4,3 Call get_spawn_lanes
C $A8E7,3 A = IX[17]
C $A8EA,6 If A < B IX[18] = B
C $A8F0,8 If A > C IX[18] = C
C $A8F8,3 A = IX[18]
C $A8FB,5 Jump to #R$A926 if A == IX[17]
@ $A902 ssub=LD HL,table_a7e7 - 1
C $A902,3 HL = $A7E6 -> #R$A7E7 data block
C $A905,1 C = A
C $A906,2 L += A
C $A908,3 A = IX[5]
C $A90B,2 test bottom bit?
C $A90F,2 A -= 5
C $A911,1 CP *HL
C $A914,1 A = *HL
C $A915,3 IX[17] = C
C $A918,2 Jump to #R$A923
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
N $A935 #REGa is zero.
C $A935,3 IX[0] = 0  -- unused hazard
C $A938,3 overtake_bonus = 0
C $A93B,2 A = $96
C $A93D,1 Restore AF
C $A93E,4 Jump if A < 3
C $A942,2 A -= 3
C $A944,3 Call cc_hit_scenery2
C $A947,3 HL = ouch_chatter
C $A94A,5 Call start_chatter (priority 3)
C $A94F,3 Effect 3 (car crash), Priority 2
C $A952,3 Call start_sfx
c $A955 Chooses random dirt and stones (not tumbleweeds though)
D $A955 Used by the routines at #R$8401 and #R$852A.
@ $A955 label=choose_dirt_and_stones
C $A955,5 Return if on_dirt_track is zero
C $A95A,5 Return if allow_spawning is zero
C $A95F,3 Point #REGde at (something above the stack)
C $A962,3 Call rng
@ $A96C label=cdas_store
C $A965,8 Store 1 (stone) if it's +ve or zero, or 2 (dirt) if it's -ve
C $A96D,3 Call rng [could be a position?]
C $A970,2 Store that random byte
C $A972,5 Self modify 'LD A' @ #R$A97E to load 1
C $A977,3 Self modify 'LD A' @ #R$C0BB to load 1
C $A97A,3 Self modify 'LD A' @ #R$A9DE to load 1
C $A97D,1 Return
c $A97E Lays out dirt and stones
D $A97E This seems to calculate the position of stones on the dirt track section.
D $A97E Used by the routines at #R$8401 and #R$852A.
@ $A97E label=layout_dirt_and_stones
C $A97E,2 A = <self modified>  -- Self modified by #R$A974
C $A980,2 Return if #REGa is zero
C $A982,6 #REGiy = $E361
C $A988,3 BC = $1400
C $A98B,3 Point #REGhl at (something above the stack)
N $A98E Scan through whatever-it-is for a non-zero byte. Move 4 bytes per iteration.
@ $A98E label=ldas_loop1
C $A98E,1 A = *HL++
C $A98F,2 Set flags
C $A991,2 Jump to A9A7 if non-zero
C $A993,3 HL += 3
@ $A996 label=ldas_loop1_continue
C $A996,2 IY--
C $A998,2 Loop to ldas_loop1 while #REGb > 0
C $A99A,1 A = C
C $A99B,1 Set flags
C $A99C,1 Return if non-zero
C $A99D,3 Self modify LD A @ #R$A97E
C $A9A0,3 Self modify LD A @ #R$C0BB
C $A9A3,3 Self modify LD A @ #R$A9DE
C $A9A6,1 Return
C $A9A7,2 A = *HL++
C $A9A9,1 C++
C $A9AB,1 Bank A
C $A9AC,6 L = ~(IY[1] * 2)
C $A9B2,2 H = $E8  -- road drawing (left)
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
C $A9C2,3 HL = $0000 [not self modified]
C $A9C5,1 Unbank A
N $A9C6 Multiplier.
C $A9C6,2 B = 8
@ $A9C8 label=ldas_loop2
C $A9C9,3 If carry HL += DE
C $A9CC,1 HL <<= 1
C $A9CD,2 Loop to ldas_loop2 while #REGb > 0
C $A9CF,1 A = H
C $A9D1,1 C = A
C $A9D3,1 HL += BC
C $A9D7,4 DE = wordat(HL); HL += 2
C $A9DB,3 Jump to ldas_loop1_continue
c $A9DE Dust/Stones stuff
D $A9DE If disabled this stops stones and dirt from rendering.
D $A9DE Used by the routine at #R$8F5F.
@ $A9DE label=dust_stones_stuff
C $A9DE,2 A = <self modified>  Self modified by #R$A97A, #R$A9A3
C $A9E0,1 Set flags
C $A9E1,1 Return if zero
C $A9E2,3 Point #REGhl at <self modified>
C $A9E5,3 A = *HL; HL += 2
C $A9E8,1 Set flags
C $A9E9,2 Jump to dss_lods if non-zero
N $A9EB Otherwise skip the next two bytes.
C $A9EB,2 HL += 2
C $A9ED,3 Self modify 'LD HL,x' @ #R$A9E2 (above) to load HL
C $A9F0,1 Return
@ $A9F1 label=dss_lods
C $A9F1,4 Load contents of stones_lods
C $A9F5,1 A--
C $A9F6,6 If non-zero load contents of dust_lods
@ $A9FC label=dss_a9fc
C $A9FC,2 C = *HL++
C $A9FE,1 A = *HL
C $AA00,1 HL++
C $AA01,3 Self modify 'LD HL,x' @ #R$A9E2 to load HL
C $AA04,1 A = 0
C $AA05,3 Self modify 'LD D,x' @ #R$933D to load zero
C $AA08,1 H = A
C $AA09,2 A = B - 1
C $AA0B,6 If A > 10 A = 10
@ $AA11 label=dss_aa11
C $AA11,2 A >>= 1
C $AA13,6 L = A * 7
C $AA19,1 HL += DE
C $AA1A,7 E = *HL * 8
C $AA22,1 Set flags
C $AA23,1 A = C
C $AA24,2 C = $00 [not self modified]
C $AA26,3 Jump to #R$AA33 if negative
C $AA29,1 Return if non-zero
C $AA2A,5 Jump to draw_object_right_helicopter_entrypt if A >= 128
C $AA2F,1 A += E
C $AA30,3 Exit via draw_object_left_helicopter_entrypt
@ $AA33 label=dss_aa33
C $AA33,1 A += E
C $AA34,1 Return if no carry
C $AA35,3 Exit via draw_object_left_helicopter_entrypt
c $AA38 Draw the helicopter
D $AA38 #R$AB89 self modifies #R$8FA4 to call this.
@ $AA38 label=draw_helicopter
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
N $AA4E Multiplier
@ $AA4E label=dhl_loop1
C $AA4E,1 Shift top bit out
C $AA4F,3 If carry HL += DE
@ $AA52 label=dhl_aa52
C $AA52,1 HL <<= 1
C $AA53,2 Loop #R$AA4E while #REGb > 0
C $AA55,2 A = H / 2
C $AA57,3 Self modify 'LD A' @ #R$AA8C
C $AA5A,2 A = <self modified> -- Self modified by #R$AAEE
C $AA5C,3 A -= IY[$4E]
C $AA5F,3 Self modify 'ADD A' @ #R$AA76
C $AA62,2 B = 5  iterations
C $AA64,5 A = counter_A & 1  -- counter used for turbo smoke
C $AA69,3 HL = *$5D08
C $AA6C,2 If A was set
C $AA6E,3 HL = *$5D0A
@ $AA71 label=dhl_aa71
C $AA71,4 DE = wordat(HL); HL += 2
C $AA75,1 A = *DE
C $AA76,2 A += <self modified> -- Self modified by #R$AA5F
C $AA78,1 DE++
C $AA79,4 Preserve IY, HL, BC
C $AA7D,3 Call dhl_aa94
C $AA80,4 Restore IY, HL, BC
C $AA84,2 Loop loop_aa71 while #REGb > 0
C $AA86,3 DE = wordat(HL); HL++
C $AA89,1 A = 0
C $AA8A,2 Preserve IY
C $AA8C,2 A = <self modified> -- Self modified by #R$AA57
C $AA8E,3 Call dhl_aa94
C $AA91,2 Restore IY
C $AA93,1 Return
@ $AA94 label=dhl_aa94
C $AA94,3 BC = <self modified> -- Self modified by #R$AB00, #R$AB2F + others
C $AA97,2 A = -A
C $AA99,3 Self modify 'LD D,x' @ #R$933D to load A
C $AA9C,5 HL = *DE++
C $AAA1,1 Set flags
C $AAA5,1 H--
@ $AAA6 label=dhl_aaa6
C $AAA6,1 HL += BC
C $AAA8,1 B = *HL
C $AAA9,6 B <<= 3
C $AAAF,1 A = D
C $AAB0,1 Set flags
C $AAB1,1 A = E
C $AAB2,2 C = 0
C $AAB7,1 Return if non-zero
C $AAB8,5 Jump to draw_object_right_helicopter_entrypt if A >= 128
C $AABD,1 A += B
C $AABE,3 Exit via draw_object_left_helicopter_entrypt
@ $AAC1 label=dhl_aac1
C $AAC1,1 A += B
C $AAC2,1 Return if no carry
C $AAC3,3 Exit via draw_object_left_helicopter_entrypt
c $AAC6 Move the helicopter
D $AAC6 Used by the routine at #R$8401.
@ $AAC6 label=move_helicopter
C $AAC6,5 Return if helicopter_control is zero
N $AACB Helicopter descends while moving to height 97 (smaller = lower).
C $AACB,2 A = <self modified>  -- Self modified by #R$AAD3 below [target vert pos?]
C $AACD,4 Jump to #R$AAD6 if A == 97
C $AAD1,2 Descend by 2 pixels
C $AAD3,3 Self modify #R$AACB above
@ $AAD6 label=mh_descended
C $AAD6,1 C = A
C $AAD7,2 A = <self modified>  -- Self modified below [counts 0..3]
C $AAD9,3 A = (A + 1) & 3
C $AADC,3 Self modify #R$AAD7 above
C $AADF,2 A = <self modified> -- Self modified by #R$AAE5 below [direction 1 or $FF]
C $AAE1,2 what are we testing here?
C $AAE3,2 A = -A
C $AAE5,3 Self modify #R$AADF above
C $AAE8,2 A += <self modified>  -- Increment by old amount -- Self modified below
C $AAEA,3 Self modify above with new value of A
C $AAED,1 A += C
C $AAEE,3 Self modify 'LD A' @ #R$AA5A
C $AAF1,3 HL = road_pos
C $AAF4,2 DE = HL
C $AAF6,3 BC = <self modified>  -- Self modified by #R$AAFC below, #R$AB60  -- prev road pos?
C $AAF9,1 Set flags
C $AAFA,2 HL -= BC
C $AAFC,4 Self modify 'LD BC' above to load DE
C $AB00,4 Load DE from 'LD BC' @ #R$AA94
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
C $AB2F,3 Self modify 'LD BC' @ #R$AA94
C $AB32,1 Return
c $AB33 Helicopter
D $AB33 Used by the routine at #R$8401.
@ $AB33 label=drive_helicopter
C $AB33,5 Return if helicopter_control is zero
C $AB38,3 Jump to hc_1 if helicopter_control is one
C $AB3B,3 Jump to hc_pick_direction if helicopter_control >= 3
N $AB3E Otherwise helicopter_control is 2.
C $AB3E,3 Read from 'LD BC' @ #R$AA94
C $AB41,1 Set flags
C $AB42,1 Return if zero
C $AB43,1 A = 0  -- NOP
C $AB44,2 DE = 0  -- NOPs
C $AB46,2 New value for helicopter_control is 0
C $AB48,2 Jump to hc_exit
@ $AB4A label=hc_1
C $AB4A,3 HL = $FFC8 (-56)
C $AB4D,2 New value for helicopter_control is 2
C $AB4F,2 Jump to hc_set_draw
@ $AB51 label=hc_pick_direction
C $AB51,1 A--
C $AB52,3 Point #REGhl at pilot "turn left" messages
C $AB55,2 Jump to hc_chatter if A now zero (helicopter_control == 3)
C $AB57,1 A--
C $AB58,1 Return if A now non-zero
C $AB59,3 Point #REGhl at pilot "turn right" messages (helicopter_control == 4)
@ $AB5C label=hc_chatter
C $AB5C,8 Self modify 'LD BC' @ #R$AAF6 to load road_pos
C $AB64,7 Self modify 'LD BC' @ #R$AA94 to load $FFE8
C $AB6B,4 Self modify 'LD A' @ #R$AAD7 to load zero
C $AB6F,3 Self modify 'ADD A' @ #R$AAE8 to load zero
C $AB72,4 Self modify 'LD A' @ #R$AADF to load one
C $AB76,5 Call start_chatter (priority 15)
C $AB7B,3 HL = $0070
N $AB7E Set starting vertical position of the helicopter.
C $AB7E,5 Self modify 'LD A' @ #R$AACB to load 133
C $AB83,2 New value for helicopter_control is 5
@ $AB85 label=hc_set_draw
C $AB85,1 Preserve new value for helicopter_control
C $AB86,3 Self modify 'LD DE' @ #R$AB06 to load $0070, or $FFC8
C $AB89,3 Address of draw_helicopter
C $AB8C,2 Opcode for CALL
@ $AB8E label=hc_exit
C $AB8E,7 Self modify #R$8FA4 to be CALL draw_helicopter, or NOPs
C $AB95,1 Restore new value for helicopter_control
C $AB96,3 helicopter_control = A
C $AB99,1 Return
c $AB9A Spawn hazards
D $AB9A Spawns hittable hazards in the road, like barriers and tumbleweeds.
D $AB9A Used by the routines at #R$8401, #R$852A and #R$873C.
@ $AB9A label=spawn_barriers
C $AB9A,5 Return if allow_spawning is zero
C $AB9F,3 A = 22 - A
C $ABA2,1 C = A
C $ABA3,3 Load road_buffer_offset into #REGa
C $ABA6,2 Add 160 so it's the hazards data offset
C $ABA8,1 A += C
C $ABA9,3 Point #REGhl at road buffer hazards data
C $ABAC,1 A = *HL   -- load the hazard byte
C $ABAD,1 Set flags
C $ABAE,1 Return if zero
C $ABAF,2 Is it 4?
C $ABB1,3 DE = 0
C $ABB4,2 Jump if < 4   -- stop spawning barriers etc? jump/fork too?
C $ABB6,2 E = 3
C $ABB8,1 A -= E
C $ABB9,2 B = 50
C $ABBB,1 A--
C $ABBC,2 Jump if zero  [A == 1]
C $ABBE,2 B = $DC
C $ABC0,1 A--
C $ABC1,2 Jump if zero  [A == 2]
C $ABC3,2 L += 2
C $ABC5,1 *HL = D
C $ABC6,2 L += 2
C $ABC8,1 *HL = D
C $ABC9,1 A = E
C $ABCA,2 Is it 3?
C $ABCC,2 Jump if non-zero
C $ABCE,3 Load inhibit_collision_detection
C $ABD1,1 Set flags
C $ABD2,2 Jump if zero
N $ABD4 Flag was set.
C $ABD4,2 B = 32
C $ABD6,3 Call sb_spawn
C $ABD9,2 B = 86
C $ABDB,3 Call sb_spawn
C $ABDE,2 B = 140
C $ABE0,2 Calls sb_spawn then returns
C $ABE2,2 B = 80
C $ABE4,3 Call sb_spawn
C $ABE7,2 B = 160
C $ABE9,2 Calls sb_spawn then returns
C $ABEB,2 B = 70
C $ABED,3 Call sb_spawn
C $ABF0,2 B = 180
C $ABF2,3 Call sb_spawn
C $ABF5,1 Return
N $ABF6 I:B Stored at entry+5  e.g. $20/$46/$56/$50/$B4
N $ABF6 I:C Stored at entry+1  e.g. $13
@ $ABF6 label=sb_spawn
C $ABF6,1 Bank entry registers
C $ABF7,2 6 iterations
C $ABF9,3 Point #REGhl at hazards table
C $ABFC,3 Stride of 20 bytes
@ $ABFF label=sh_loop
C $ABFF,2 Check the flag byte to see if the entry is used (it's either $00 if empty, or $FF if used, so rotating it in place is not a problem)
C $AC01,2 Jump to sb_found_spare if it didn't carry
C $AC03,1 Move to the next entry
C $AC04,2 Loop while iterations remain
C $AC07,1 Return
N $AC08 #REGhl points to the unused entry
@ $AC08 label=sb_found_spare
C $AC08,3 #REGix = #REGhl
C $AC0B,10 Zero 20 bytes at #REGhl
C $AC15,1 Restore entry registers
C $AC16,9 wordat(IX + 11) = Address of #R$AC3C routine
N $AC1F Gets hit when on the dirt track. sampled DE = $3 (tumbleweed), $0 (barrier)
C $AC1F,4 Point #REGhl at the hazards data table entry (*$5CF6 -> #R$5E40 + 3)
C $AC23,5 Copy flag TBD
C $AC28,9 Copy lod address (e.g. tumbleweed_lods)
C $AC31,3 IX[5] = B TBD -- passed in
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
C $AC53,3 Point #REGhl at #R$AD03 table
N $AC56 This must be mapping the speed into the 5-entry array.
C $AC56,1 A = D     top part of speed
C $AC57,2 E ROL 1   double speed
C $AC59,1 A ROL 1
C $AC5A,2 A &= 3    mask top part to 3 (for ref. turbo speed = $1FF)
C $AC5C,2 RR D      halve D to carry?
C $AC5E,2 D = 0
C $AC60,2 A = (A + D + carry) * 2  scale for element size of 2
C $AC62,1 E = A   it's an offset in #R$AD03
C $AC63,1 HL += DE
C $AC64,9 Copy (two bytes?) to IX+17 from table
C $AC6D,4 double BC?  adjusted speed retained from earlier
C $AC71,8 If B >= 2 BC = 350  so this is if we hit the object very fast it gets put on the left?
C $AC79,3 Set bottom byte of horizontal position
C $AC7C,3 Increment IX[7]
C $AC7F,2 if B is zero then don't store it
C $AC81,3 Set top byte of horizontal position
C $AC84,3 Increment IX[1]
C $AC87,3 Effect 5 (hazard hit), Priority 3
C $AC8A,3 Call start_sfx
C $AC8D,2 A = 2        [set this to 1 and it drives off like a car]
C $AC8F,3 Set IX[15] to 2
@ $AC92 label=hh_ac92
C $AC92,1 A--
C $AC93,1 If A == 0 return
C $AC94,13 IX[16] = #R$ACDB[IX[17]]
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
@ $ACDB label=table_acdb
B $ACDB,40,8 #R$AC94 uses this
@ $AD03 label=table_ad03
W $AD03,10,2 #R$AC53 uses this
c $AD0D Routine at AD0D
D $AD0D Used by the routine at #R$BDFB.
@ $AD0D label=sub_ad0d
C $AD0D,5 Return if the inhibit_collision_detection flag is set
C $AD12,4 IX = &hazards[0]
C $AD16,3 DE = 20  -- stride of hazards
C $AD19,2 B = 6
C $AD1B,6 If the hazard is not active continue to next one
C $AD21,1 Bank
C $AD22,3 Address of ad0d_unbank_continue
C $AD25,1 Push  -- this makes RETs below jump to ad0d_unbank_continue
C $AD26,4 A = IX[15] + 1
C $AD2A,2 Jump if non-zero
C $AD2C,3 A = IX[17]
C $AD2F,1 Set flags
C $AD30,1 UNBANK_CONTINUE if non-zero
C $AD31,3 A = IX[1]  -- distance related
C $AD34,3 UNBANK_CONTINUE if >= 20
C $AD37,2 D = 0  -- not SM
C $AD39,3 Call sub_ad51
C $AD3C,1 A = D
C $AD3D,1 Set flags
C $AD3E,1 UNBANK_CONTINUE if zero
C $AD3F,4 A = IX[15] + 1
C $AD43,1 UNBANK_CONTINUE if zero
C $AD44,6 HL = wordat(IX + 11)  -- presumably the hit handler
C $AD4A,1 Jump to it (then return to the next instr)
@ $AD4B label=ad0d_unbank_continue
C $AD4B,1 Unbank
@ $AD4C label=ad0d_continue
C $AD4C,2 Move to next hazard
C $AD4E,2 Loop
C $AD50,1 Return
c $AD51 Routine at AD51
D $AD51 Used by the routines at #R$AD0D and #R$ADA0.
R $AD51 I:IX Address of a hazard structure
R $AD51 O:D Return value
@ $AD51 label=sub_ad51
C $AD51,5 Return if IX[7] is non-zero
C $AD56,6 HL = wordat(IX + 2)
C $AD5C,3 Return if top byte of #REGhl was set
C $AD5F,4 A = IX[15] + 1 and set flags
C $AD63,3 A = IX[1]
C $AD66,2 C = 3
C $AD68,2 Jump if A /was/ non-zero
C $AD6A,2 C = 2
C $AD6C,2 Return if A >= C
C $AD6E,2 E = 4
C $AD70,1 A--
C $AD71,3 A = fast_counter
C $AD74,2 Jump if A /was/ non-zero
C $AD76,1 Set flags
C $AD77,3 Jump if positive
C $AD7A,2 E = 1
C $AD7C,2 Jump
C $AD7E,1 Set flags
C $AD7F,1 Return if positive
C $AD80,3 A = L & $F8
C $AD83,3 Return if A >= $90
C $AD86,3 A += IX[8]
C $AD89,3 Return if A <= $70
C $AD8C,3 A -= IX[8]
C $AD8F,4 Jump if A < $68
C $AD93,1 E--
C $AD94,4 Jump if A < $78
C $AD98,2 E += 2
C $AD9A,3 IX[7] = E
C $AD9D,2 D = 1
C $AD9F,1 Return
c $ADA0 Draws hazards
D $ADA0 This includes all cars, barriers, tumbleweeds, etc.
D $ADA0 Used by the routines at #R$8401, #R$852A and #R$873C.
@ $ADA0 label=draw_hazards
C $ADA0,4 n_hazards = 0
C $ADA4,3 IY = $E3xx
C $ADA7,4 IX = &hazards[0]
C $ADAB,3 Stride of hazards entry in bytes
C $ADAE,2 6 iterations
@ $ADB0 label=dh_loop
C $ADB0,4 Set carry if the hazard is active
C $ADB4,1 Bank
C $ADB5,3 Call dh_draw_one_hazard if hazard active
C $ADB8,1 Unbank
C $ADB9,2 Advance to next hazard
C $ADBB,2 Loop while iterations remain -- #REGb > 0
C $ADBD,1 Return
@ $ADBE label=dh_draw_one_hazard
C $ADBE,3 C = IX[14]  -- top byte of horz position?
C $ADC1,9 IX[4] -= IX[13]  -- (TBD -= (bottom byte of horz position?))
C $ADCA,3 If IX[4] was < IX[13] Then C++
C $ADCD,5 C += IX[1]  -- distance related?
C $ADD2,4 A = IX[15] + 1  -- counter?
C $ADD6,2 Jump to dh_adf0 if non-zero
C $ADD8,3 A = IX[17]  -- byte that indexes #R$ACDB
C $ADDB,2 Jump if no carry (from earlier)
C $ADDD,5 Jump if ++A < 4
C $ADE2,1 A--
C $ADE3,2 C = $FF  -- gets put in distance related field
C $ADE5,3 IX[17] = A
C $ADE8,1 Set flags
C $ADE9,1 A = C
C $ADEA,2 Jump if zero
C $ADEC,3 IX[1] = A  -- update distance?
C $ADEF,1 Return
@ $ADF0 label=dh_adf0
C $ADF0,1 A = C
C $ADF1,4 Jump if A < 23  -- still visible?
N $ADF5 Wipe the hazard because it's gone?
C $ADF5,4 IX[0] = 0  -- hazard slot now spare
@ $ADF9 label=just_ret
C $ADF9,1 Return
C $ADFA,3 IX[1] = A  -- distance related
C $ADFD,3 Return if A >= 20
C $AE00,1 A--
C $AE01,2 Jump if non-zero
C $AE03,6 A = ~(fast_counter & $E0)
C $AE09,5 Jump if A >= IX[4]
C $AE0E,4 B = IX[15] + 1
C $AE12,2 Jump if zero
N $AE14 Wipe the hazard because car overtaken?
C $AE14,4 IX[0] = 0  -- hazard slot now spare
C $AE1A,1 Return if no carry
C $AE1B,4 Increment allow_overtake_bonus
C $AE1F,1 Return
C $AE20,3 IX[4] = A
C $AE23,1 A = 0
C $AE24,2 A += $4E
N $AE26 $E300..$E316 is regularly accessed. $E300 is always $60, $E316 is always $A0
C $AE26,2 IY.low = A  -- i.e. $E300 | A
C $AE28,3 A = IY[1]
C $AE2B,1 C = A
C $AE2C,3 A -= IY[0]  -- A is now the delta between adjacent elements
N $AE2F Multiplier. DE = multiplier. A = multiplicand. HL = result.
C $AE2F,5 DE = A; HL = 0
C $AE34,3 A = IX[4]
C $AE37,2 B = 8  -- iterations of multiply loop
C $AE39,1 Test top bit
C $AE3A,3 If it carried out then add
C $AE3D,1 Double
C $AE3E,2 Loop
C $AE40,1 A = H  -- high part of result
C $AE42,3 IX[6] = A
C $AE45,5 A = ~((C - A) << 1)
C $AE4A,3 HL = $E800 | A (road drawing left)
C $AE4D,1 D = *HL
C $AE4E,1 HL--
C $AE4F,1 E = *HL
C $AE50,2 H = $EC
C $AE52,1 A = *HL
C $AE53,1 L++
C $AE54,1 H = *HL
C $AE55,1 L = A
C $AE56,1 Set flags
C $AE57,4 Self modify 'LD HL,xxxx' @ #R$AE70
C $AE5B,2 HL -= DE
C $AE5D,1 Swap
C $AE5E,3 HL = 0
C $AE61,3 A = IX[5]
N $AE64 Another multiplier.
C $AE64,2 B = 8
C $AE66,1 A <<= 1
C $AE67,3 If it carried out then add
C $AE6A,1 Double
C $AE6B,2 Loop
C $AE6D,1 A = H
C $AE6F,1 C = A
C $AE70,4 HL = <self modified> + BC
C $AE74,6 wordat(IX + 2) = HL
C $AE7A,3 Call sub_ad51
C $AE7D,3 D = IX[1]
C $AE80,3 E = IX[4]
C $AE83,3 HL = &n_hazards
C $AE86,1 A = *HL
C $AE87,1 (*HL)++
C $AE88,3 HL -> table
C $AE8B,1 Set flags
C $AE8C,2 Jump to dh_no_hazards if zero
C $AE8E,1 Iterations
N $AE8F Loop starts (loop for all hazards)
C $AE8F,1 A = D
C $AE90,1 Compare to *HL
C $AE91,1 L++
C $AE92,2 Jump if A < *HL
C $AE94,2 Jump if A != *HL
C $AE96,1 A = E
C $AE97,1 Compare to *HL
C $AE98,2 Jump if A < *HL
C $AE9A,3 L += 3
C $AE9D,2 Loop
@ $AE9F label=dh_no_hazards
C $AE9F,4 wordat(HL) = DE; HL += 2
C $AEA3,3 DE = IX
C $AEA6,3 wordat(HL) = DE; HL++
C $AEA9,2 Jump
C $AEAB,1 Preserve DE
C $AEAC,3 A = B * 4
C $AEAF,3 BC = A
C $AEB2,3 A += 2 + L
C $AEB5,1 E = A
C $AEB6,2 A -= 4
C $AEB8,1 L = A
C $AEB9,1 D = H
C $AEBA,2 Do *HL-- = *DE-- until BC == 0
C $AEBD,3 DE = IX
C $AEC0,4 wordat(HL) = DE; HL -= 2
C $AEC4,1 Restore DE
C $AEC5,1 *HL = E
C $AEC6,1 L--
C $AEC7,1 *HL = D
C $AEC8,6 HL = wordat(IX + 11)
C $AECE,1 Jump there
N $AECF This entry point is used by the routine at #R$8F5F.
C $AECF,3 HL = <self modified>
C $AED2,1 A = B
C $AED3,2 Return if A != *HL
C $AED5,1 A--
C $AED6,4 Jump if A < 11
C $AEDA,2 A = 10
C $AEDC,2 A >>= 1
C $AEDE,3 Self modify 'LD A,x' @ #R$AFFB
C $AEE1,1 E = A
C $AEE2,3 A <<= 3
C $AEE5,1 A -= E
C $AEE6,3 DE = A
C $AEE9,2 L += 2
C $AEEB,8 IX = wordat(HL); HL += 2
C $AEF3,3 Preserve HL, BC, DE
C $AEF6,6 HL = wordat(IX + 9)
C $AEFC,1 HL += DE
C $AEFD,1 E = *HL
C $AEFE,6 E <<= 3 ?
C $AF04,6 A = IX[6] - IX[16]
C $AF0A,3 Self modify 'LD D,x' @ #R$933D to load A
C $AF0D,3 A = IX[19]
C $AF10,3 Self modify 'LD A,x' @ #R$93C0 to load A
C $AF13,4 A = IX[15] + 1
C $AF17,2 Jump if zero
C $AF19,3 A = IX[3]
C $AF1C,1 Set flags
C $AF1D,3 A = IX[2]
C $AF20,3 Jump if negative
C $AF23,3 Jump if non-zero
C $AF26,5 Jump if A >= 128
C $AF2B,1 A += E
C $AF2C,3 Jump
C $AF2F,1 A += E
C $AF30,2 Jump if no carry
C $AF32,3 Call draw_object_left_helicopter_entrypt
C $AF35,3 Jump over next CALL
C $AF38,3 Call draw_object_right_helicopter_entrypt
C $AF3B,2 Restore DE, BC
C $AF3D,4 Self modify 'LD A,x' @ #R$93C0 to load 0
C $AF41,3 HL = &n_hazards
C $AF44,1 (*HL)--
C $AF45,1 Restore HL
C $AF46,1 Return if zero
C $AF47,1 A = *HL
C $AF48,4 Jump if A == B
C $AF4C,3 Self modify 'LD HL' @ #R$AECF to load HL
C $AF4F,1 Return
C $AF50,3 A = IX[3]
C $AF53,3 Self modify 'LD A,x' @ #R$B029 to load A
C $AF56,1 Set flags
C $AF57,3 A = IX[2]
C $AF5A,3 Self modify 'LD A,x' @ #R$B02C to load A
C $AF5D,3 Jump if negative
C $AF60,3 Jump if non-zero
C $AF63,5 Jump if A >= 128
C $AF68,1 A = E
C $AF69,3 Jump
C $AF6C,1 A = E
C $AF6D,2 -- checking result of test at #R$AF56?
C $AF6F,3 Call draw_object_left_helicopter_entrypt
C $AF72,3 Jump
C $AF75,3 Call draw_object_right_helicopter_entrypt
C $AF78,3 Read A from 'LD D,x' @ #R$933D
C $AF7B,3 Self modify 'LD A,x' @ #R$B023
C $AF7E,3 Get smash_factor
C $AF81,4 Jump if A >= 5
C $AF85,3 A = x in 'LD A,x' @ #R$AFFB
C $AF88,5 Jump if A >= 4
C $AF8D,1 Double A
C $AF8E,3 BC = A
C $AF91,4 HL = #R$CDEC + BC
N $AF95 I see this getting hit only when in smash mode.
C $AF95,3 BC = wordat(HL)
C $AF9E,3 Get smash_factor
C $AFA1,4 Jump if it's < 4
C $AFA5,4 A = (A - 4) * 4 ?
C $AFA9,1 Bank
C $AFAA,3 A = x in 'LD A,x' @ #R$AFFB
C $AFAD,1 *= 2
C $AFAE,3 DE = A
C $AFB1,4 HL = #R$CE00 + DE  -- table?
C $AFB5,3 BC = wordat(HL); HL++
C $AFB8,1 Unbank
C $AFB9,1 E = A
C $AFBA,3 another table?
C $AFBD,5 Load counter_C AND 1
C $AFC2,1 A <<= 1?
C $AFC3,1 A += E
C $AFC4,1 E = A
C $AFC5,1 HL += DE
C $AFC6,4 HL = wordat(HL)
C $AFCA,2 Retrieve DE from stack
C $AFCF,3 Get smash_factor
C $AFD2,4 Jump if zero
C $AFD6,1 A--
C $AFD7,2 Jump if zero
C $AFD9,1 A--
C $AFDA,2 Jump if zero
C $AFDC,3 table?
C $AFDF,3 Call aff1
C $AFE5,3 Call aff1
C $AFEB,3 Call aff1
C $AFF1,2 A = *HL - 1
C $AFF3,3 Jump if +ve
C $AFF6,2 A = 5  -- it became zero, reset to 5
C $AFF8,1 *HL = A
C $AFF9,1 E = A
C $AFFA,1 HL++
C $AFFB,2 A = <self modified>
C $AFFD,1 C = A
C $AFFE,1 D = A
C $AFFF,1 A += E
C $B000,3 Return if A >= 6
C $B003,1 E = A
C $B004,1 A -= D
C $B005,1 D = A
C $B006,2 C <<= 1
C $B008,2 B = 0
C $B00A,1 HL += BC
C $B00B,1 A = *HL
C $B00C,1 A -= D
C $B00D,1 B = A
C $B00E,1 HL++
C $B00F,1 C = *HL
N $B010 #REGe = smoke animation index Select the frame.
C $B010,8 DE = E * 7
C $B018,3 Point #REGhl at smoke_defns
N $B01B Similar code to $AA19.
C $B01B,1 HL += DE
C $B01C,7 E = *HL * 8  -- turn byte width to pixel width
C $B023,3 A = <self modified> + B
C $B026,3 Self modify 'LD D,x' @ #R$933D to load A
C $B029,2 A = <self modified>
C $B02B,1 Set flags
C $B02C,2 A = <self modified>
C $B02E,3 Jump to #R$B03D if negative
C $B031,1 Return if non-zero
C $B032,1 A += C
C $B033,1 Return if carry
C $B034,5 Exit via draw_object_right_helicopter_entrypt if A >= 128
C $B039,1 A += E  -- pixel width
C $B03A,3 Exit via draw_object_left_helicopter_entrypt
C $B03D,1 A += C
C $B03E,2 Loop? if carry
C $B040,1 A += E  -- pixel width
C $B041,3 Exit via draw_object_left_helicopter_entrypt if carry
C $B044,1 Otherwise return
w $B045 Hero car jump table
D $B045 This is a table of 10 values used when the hero car jumps. The first byte of the entry is used to make the car move vertically (by self modifying #R$B5AA). The second byte is used to change the pitch of the car where level/up/down = 0/3/6 (by self modifying #R$B5AF). Used by #R$B968.
@ $B045 label=hero_car_jump_table
W $B045,2,2 Delta  13, Pitch Up
W $B047,2,2 Delta  10, Pitch Up
W $B049,2,2 Delta   7, Pitch Up
W $B04B,2,2 Delta   4, Pitch Up
W $B04D,2,2 Delta   2, Pitch Level
W $B04F,2,2 Delta  -2, Pitch Level
W $B051,2,2 Delta  -4, Pitch Down
W $B053,2,2 Delta  -7, Pitch Down
W $B055,2,2 Delta -10, Pitch Down
W $B057,2,2 Delta -13, Pitch Down
N $B059 Sub-table (another byte pair)
W $B059,10,2
c $B063 Hero car jumps; gear changing; turbos; off road checks; speed adjustment
D $B063 Used by the routines at #R$8401 and #R$852A.
@ $B063 label=move_hero_car
C $B063,2 Load jump counter. Self modified by #R$8827 and #R$B965. Highest is 8.
C $B065,3 Jump to mhc_not_jumping if the jump counter is zero
N $B068 Hero car is jumping.
@ $B068 label=mhc_jumping
C $B068,4 Decrement the jump counter in (self modified) #R$B063
C $B06C,2 Jump to mhc_midair if we're still jumping
N $B06E Hero car has landed.
@ $B06E label=mhc_landed
C $B06E,5 smoke = 3
C $B073,3 Effect 2 (car landing), Priority 3
C $B076,3 Call start_sfx
N $B079 Hero car is in mid-air, or has just landed.
@ $B079 label=mhc_midair
C $B079,3 Point #REGhl at entry in jump table. Self modified by #R$B96C and #R$B092. Default is $B055.
C $B07C,1 off_road = 0
C $B07D,3 }
C $B080,8 Clear up/down/left/right bits of user input (stop the player from turning when in mid-air)
N $B088 The low bytes of the hero_car_jump_table entries are the car's pitch (0/3/6).
C $B088,5 Self modify the 'ADD A,x' at #$B5AF to load the car's pitch
N $B08D The high bytes of the entries are the car's jump offset (-13..13).
C $B08D,5 A = *$B5AB + *HL++  -- Self modified value in draw_car
C $B092,3 Update #REGhl jump table entry address above @ #R$B079
@ $B095 label=mhc_set_jump_offset
C $B095,3 Self modify 'SUB x' @ #R$B5AA to be the new car jump offset
C $B098,3 Address of boost timer
C $B09B,4 Jump to mhc_smoke if a turbo boost is not in effect
C $B09F,1 Decrement boost timer
C $B0A0,2 Jump to mhc_smoke if boost timer is non-zero
C $B0A2,7 Decrement turbo boost count [POKE $B0A5,0 for Infinite turbos]
N $B0A9 Handle smoke effect.
@ $B0A9 label=mhc_smoke
C $B0A9,1 #REGhl now points at smoke
C $B0AA,4 Jump if smoke is zero
C $B0AE,1 Decrement smoke
N $B0AF Handle gear changes.
@ $B0AF label=mhc_gear_change
C $B0AF,3 Read user input
C $B0B2,1 Preserve it in #REGc
C $B0B3,3 Read from the 'LD A,x' @ #R$B325 (crashed flag)
C $B0B6,3 Jump if zero
N $B0B9 Otherwise the crashed flag is set.
C $B0B9,4 Change user input to be ONLY the fire/gear flag (if set)
@ $B0BD label=mhc_b0bd
C $B0BD,1 A = C
C $B0BE,1 Preserve (modified) user input
C $B0BF,2 Test fire/gear flag
C $B0C1,3 Address of gear flag
C $B0C4,3 Load gear_lockout
C $B0C7,2 Jump if fire/gear is unset
N $B0C9 Fire/gear was set.
C $B0C9,3 Jump if gear_lockout is set  -- lock out gear changes while set
C $B0CC,4 Toggle gear flag
C $B0D0,2 Smoke time remaining
C $B0D2,2 Jump if low gear
C $B0D4,3 Set smoke counter
@ $B0D7 label=mhc_b0d7
C $B0D7,2 Decrement A  [why write it as a SUB 1?]
C $B0D9,2 Jump if positive result
C $B0DB,3 gear_lockout = A
@ $B0DE label=mhc_b0de
C $B0DE,1 Read current gear flag
C $B0DF,1 Bank it
C $B0E0,3 Read current speed
C $B0E3,1 Stack it
C $B0E4,5 HL -= 120
C $B0E9,2 Jump if speed >= 120
C $B0EB,6 Jump if perp_caught_phase > 0
N $B0F1 Handle idle timer.
C $B0F1,3 Address of idle timer
C $B0F4,1 Decrement idle timer
C $B0F5,2 Jump if non-zero
C $B0F7,2 Reset idle timer to 100
C $B0F9,2 Set priority to 10
C $B0FB,3 HL = get_moving_chatter
C $B0FE,3 Call start_chatter (priority 10)
@ $B101 label=mhc_b101
C $B101,1 Pop speed
C $B102,2 DE = HL
C $B104,3 Load the off-road flag
C $B107,3 Jump if zero
N $B10A Handle off-road (#REGa can be 1 or 2 here).
C $B10A,3 BC = 120  -- factor for off-road == 1
C $B10D,3 Jump if off-road was one
C $B110,3 BC = 110  -- factor for off-road == 2
@ $B113 label=mhc_b113
C $B113,1 Clear carry
C $B114,2 HL -= BC  -- 16-bit subtract only for result in flags
C $B116,2 HL = D  -- speed
C $B118,2 Jump if HL < BC (result of subtraction)
N $B11A Otherwise do something speed dependent.
C $B11A,1 Speed low byte
C $B11B,2 Bottom bit of #REGh moves to carry
C $B11D,4 A <<= 4  -- 9 bit rotate left through carry
C $B121,6 A = -((A & $0F) | 1)
C $B127,3 BC = $FF00 | A
C $B12A,3 Jump to mhc_b19a
@ $B12D label=mhc_b12d
C $B12D,3 A = boost (time remaining)
C $B130,1 Test then bank the flags
C $B131,1 Bank
C $B132,1 Check previously banked A
C $B135,3 BC = 470
C $B138,1 Unbank boost + flags
C $B139,2 Uses banked flags
C $B13B,3 BC = 230
@ $B13E label=mhc_b13e
C $B13E,2 HL -= BC
C $B140,3 Jump if HL was >= BC
C $B143,7 BC = ~HL + 1
C $B14A,1 A = C
C $B14B,3 Shift a bit out of B into A?
C $B14E,3 and again
C $B151,2 Then divide by 4?
C $B153,5 C = (A & $3F) | 1
C $B158,2 Jump to mhc_b19a
@ $B15A label=mhc_b15a
C $B15A,1 A = L
C $B15B,2 H >>= 1
C $B15E,2 H >>= 1
C $B163,6 A = -((A & $1F) | 1)
C $B169,3 BC = $FF00 | A
C $B16C,2 Jump to mhc_b19a
@ $B16E label=mhc_b16e
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
C $B18C,2 Jump to mhc_b19a
@ $B18E label=mhc_b18e
C $B18E,3 BC = 695
C $B194,3 BC = 360
@ $B19A label=mhc_b19a
C $B19A,2 Get stacked #REGaf
C $B19C,3 Test bit 2?
C $B19F,2 Jump to mhc_b1a6 if no carry
C $B1A1,3 BC = $FFEC
C $B1A4,2 Jump to mhc_b1ac
@ $B1A6 label=mhc_b1a6
@ $B1AC label=mhc_b1ac
C $B1AC,2 HL = DE
C $B1AE,1 Clear carry?
C $B1AF,2 HL += BC
C $B1B4,3 HL = $0000
@ $B1B7 label=mhc_b1b7
C $B1B7,5 A = var_a24c - 1
C $B1BE,3 A = $B5B0  -- self modified value in draw_car
C $B1C1,3 Jump if zero
C $B1C4,1 C = A
C $B1C5,1 A = H
C $B1C6,1 A |= L
C $B1C9,5 A = (C - 5) | 1
C $B1CE,1 C = A
C $B1CF,2 B = 0
C $B1D4,1 B--
@ $B1D5 label=mhc_b1d5
C $B1D5,2 DE = HL
C $B1D7,1 HL += BC
C $B1D8,3 BC = 695
C $B1DC,2 HL -= BC
C $B1DF,2 Jump if HL < BC
@ $B1E2 label=mhc_b1e2
C $B1E2,2 A = 3
@ $B1E4 label=mhc_b1e4
C $B1E4,3 var_a24c = A
C $B1E7,1 A = H
C $B1E8,2 CP 2
C $B1EC,3 Cap speed to $1FF
@ $B1EF label=mhc_b1ef
C $B1EF,3 Set speed to #REGhl
C $B1F2,1 Restore HL  [what is it?]
C $B1F3,4 B = var_a263
C $B1F7,4 C = var_a264
C $B1FB,3 A = *$B064  -- Jump counter [self modified]
C $B1FE,4 Jump to mhc_b253 if non-zero
C $B202,2 Shift LSB out of H
C $B204,2 Jump to mhc_b21a if set
C $B206,2 Shift new LSB out of H
C $B208,2 Jump to mhc_b22c if set
N $B20A A = ((C >= 9) ? C - 9 : 0) = MAX(C - 9, 0) This instr is hit continuously
C $B20A,3 A = C - 9  -- var_a264 from earlier
C $B20D,2 Jump if C >= 9
C $B20F,1 A = 0
@ $B210 label=mhc_b210
C $B210,1 C = A
N $B211 A = ((B >= 9) ? B - 9 : 0) = MAX(B - 9, 0)
C $B211,3 A = B - 9
C $B214,2 Jump to mhc_b217 if B >= 9
C $B216,1 A = 0
@ $B217 label=mhc_b217
C $B217,1 B = A
C $B218,2 Jump to mhc_b23c
N $B21A B = ((B + 4 >= 36) ? 36 : B + 4) = MIN(36, B + 4)
@ $B21A label=mhc_b21a
C $B21A,3 A = B + 4
C $B21D,5 Jump if A >= 36
C $B222,1 B = A
N $B223 C = ((C >= B) ? C - B : 0) = MAX(C - B, 0)
@ $B223 label=mhc_b223
C $B223,3 C -= B
C $B226,2 Jump to mhc_b23c if C >= B
C $B228,2 C = 0
C $B22A,2 Jump to mhc_b23c
N $B22C C = ((C + 4 >= 36) ? 36 : C + 4) = MIN(36, C + 4)
@ $B22C label=mhc_b22c
C $B22C,3 A = C + 4
C $B22F,2 C = 36
C $B231,3 Jump to mhc_b235 if A >= 36
C $B234,1 C = A
N $B235 B = ((B >= C) ? B - C : 0) = MAX(B - C, 0)
@ $B235 label=mhc_b235
C $B235,3 B -= C
C $B238,2 Jump to mhc_b23c if B >= C
C $B23A,2 B = 0
@ $B23C label=mhc_b23c
C $B23C,3 Load speed into #REGhl
C $B23F,1 Speed low byte
C $B240,2 Bottom bit of #REGh moves to carry (#REGh now unused)
C $B242,1 Halve speed, shifting carry in as MSB
C $B243,4 A >>= 2
C $B247,1 L = A
C $B248,2 L >>= 1
C $B24A,1 A += L
C $B24B,3 Jump if A >= B
C $B24E,1 B = A
@ $B24F label=mhc_b24f
C $B24F,3 Jump if A >= C
C $B252,1 C = A
@ $B253 label=mhc_b253
C $B254,3 BC = $0000
C $B257,1 E = B
C $B258,3 Load current_curvature
C $B25B,1 Set flags
C $B25C,3 Jump to mhc_straight_road if zero
C $B25F,3 Jump to mhc_scroll_horizon if positive
N $B262 Negative scroll => scroll horizon right.
C $B262,1 E++
C $B263,2 A = -A
N $B265 Positive scroll => scroll horizon left.
@ $B265 label=mhc_scroll_horizon
@ $B265 ssub=LD HL,table_b828 - 1
C $B265,3 HL -> 32-byte table at #R$B828
C $B268,1 BC = A  -- B is zero at this point
C $B269,1 HL += BC
C $B26A,3 A = var_a261
C $B26D,1 C = A
C $B26F,4 A = fast_counter - C
C $B273,1 Set flags
C $B274,2 Jump if zero
C $B276,1 C = *HL
@ $B277 label=mhc_b277
C $B277,1 A -= C
C $B278,2 Jump if carry
C $B27A,1 B++
C $B27C,1 A += C
C $B27E,2 Jump to mhc_b277 (above)  -- loop?
@ $B280 label=mhc_b280
C $B280,1 A = B
C $B281,3 Jump to mhc_b295 if A is zero
C $B284,3 HL = var_a262
C $B287,1 A += *HL
C $B288,1 *HL = A
C $B289,3 A = B * 3
C $B28C,2 B = 0
C $B28E,2 Bottom bit of E moves to carry
C $B290,2 Jump if clear
C $B292,1 B--
C $B293,2 A = -A
@ $B295 label=mhc_b295
C $B295,1 C = A
C $B297,3 var_a261 = A
N $B29A No curvature - No scroll required?
@ $B29A label=mhc_straight_road
C $B29A,4 HL = var_a25f + BC
C $B29E,7 var_a25f = 0
C $B2A5,3 A = *$B326  -- Read self modified op in animate_hero_car -- crashed flag
C $B2A8,1 Set flags
C $B2A9,2 Jump to mhc_b2ad if zero
C $B2AB,1 DE <> HL
C $B2AC,1 D = H  [must be a delta]
@ $B2AD label=mhc_b2ad
C $B2AE,4 var_a263 = B
C $B2B2,4 var_a264 = C
C $B2B6,1 A -= B
C $B2B7,3 Jump to mhc_b2bb if positive
C $B2BA,1 Otherwise decrement D
@ $B2BB label=mhc_b2bb
C $B2BB,3 E = A >> 1
C $B2BE,1 HL += DE
C $B2BF,1 Set flags
C $B2C0,2 Jump to mhc_set_cornering if zero
C $B2C2,3 Jump if positive
C $B2C5,2 A = -A
@ $B2C7 label=mhc_check_cornering
C $B2C7,2 Compare A to 17
C $B2C9,2 A = 0  -- cornering = 0
C $B2CB,2 Jump to mhc_set_cornering if A < 17
C $B2CD,4 Jump if H bit 7 clear
C $B2D1,4 Jump to mhc_set_cornering if D bit 7 set
C $B2D5,4 Jump to mhc_set_cornering if H bit 7 clear  -- bug? already tested above
C $B2D9,2 Jump to mhc_is_cornering
@ $B2DB label=mhc_b2db
C $B2DB,4 Jump to mhc_set_cornering if D bit 7 clear
C $B2DF,4 Jump to mhc_set_cornering if H bit 7 set
@ $B2E3 label=mhc_is_cornering
C $B2E3,2 A = 1
@ $B2E5 label=mhc_set_cornering
C $B2E5,3 cornering = A
C $B2E8,8 road_pos += HL  [must be a delta]
C $B2F0,2 D = 1  -- flip flag
C $B2F2,1 A = E  -- E is?
C $B2F3,1 Set flags [why are some ORs and some ANDs?]
C $B2F4,3 Jump if positive
C $B2F7,1 D--  -- flip flag -> 0
C $B2F8,2 A = -A
@ $B2FA label=mhc_decide_turn_speed
C $B2FA,2 Compare A to 12
C $B2FC,2 2 => Turn hard
C $B2FE,2 Jump to mhc_exit if A >= 12
C $B300,1 1 => Turn
C $B301,2 Compare A to 6
C $B303,2 Jump to mhc_exit if A >= 6
C $B305,1 0 => Straight
@ $B306 label=mhc_exit
C $B306,4 turn_speed = B  -- should be 0/1/2
C $B30A,4 flip_car = D  -- should be 0/1
C $B30E,3 A = *$B064  -- Jump counter [self modified]
C $B311,2 Return if zero
C $B313,4 cornering = 0  -- reset cornering if not jumping
C $B317,1 Return
c $B318 Animates the hero car.
D $B318 Used by the routines at #R$8401 and #R$852A.
@ $B318 label=animate_hero_car
C $B318,7 Jump to #R$B325 if speed > 0
C $B31F,3 Self modify 'LD A' @ #R$B3DB to load A
C $B322,3 off_road = A
@ $B325 label=ahc_check_crashed
C $B325,2 A = <self modified>  -- crashed flag
C $B327,3 Jump if not crashed
N $B32A Crashed.
C $B32A,3 cornering = A (which is non-zero here)
C $B32D,1 Preserve HL
C $B32E,3 BC = <self modified>
C $B331,2 HL -= BC
C $B333,1 Restore HL
C $B334,2 Jump to #R$B349 if HL < BC
C $B336,1 D = H
C $B337,1 A = L
C $B338,2 D >>= 1
C $B33C,2 A >>= 1
C $B33E,2 A |= 3
C $B340,1 E = A
C $B341,2 HL -= DE
C $B343,2 Jump to #R$B349 if equal
C $B345,2 A = 2
C $B347,2 Jump if #R$B349 if HL > DE
N $B349 Otherwise HL < DE.
@ $B349 label=ahc_b349
C $B349,4 Clear crashed flag
C $B34D,1 A++
C $B34E,2 Jump
@ $B350 label=ahc_b350
C $B350,3 ($A24A) = HL
@ $B353 label=ahc_b353
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
C $B367,3 Self modify 'LD HL,$xxxx' @ #R$B356
C $B36B,3 HL = road_pos
N $B36E This seems to be 1 if I crash on the left, 0 if I crash on the right. So is it flipping the crash graphics?
C $B36E,2 C = <self modified>
C $B370,3 A = C & 1
C $B373,3 flip_car = A
C $B376,3 Jump if C == 1
C $B379,1 C--
C $B37A,2 [I wonder if this jump is ever taken]
C $B37C,1 HL += DE
C $B37F,2 HL -= DE
C $B381,3 road_pos = HL
C $B384,2 A = <self modified>  -- set to 5 on collisions
C $B386,3 Jump if zero
C $B389,1 A--
C $B38A,3 Self modify 'LD A,x' @ #R$B384 (above) to x = A
C $B38D,2 A = <self modified>
C $B38F,3 *$B3DC = A
N $B392 Arrive here if not crashed.
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
C $B3BB,3 Effect 1 (cornering squeal), Priority 5
C $B3BE,3 Call start_sfx
C $B3C1,6 Jump if perp_caught_phase > 0
C $B3C7,1 A--
C $B3C8,2 Jump if zero
N $B3CA New turn speed = MIN(A,2)
C $B3CA,4 Jump if A < 2
C $B3CE,2 Else A = 2
C $B3D0,3 turn_speed = A
C $B3D3,5 flip_car = 1
C $B3D8,3 Draw debris?
C $B3DB,2 A = <self modified>
C $B3DD,3 Jump if zero
C $B3E0,1 C = A
C $B3E2,3 A += C + 24
C $B3E5,1 C = A
C $B3E6,3 Load turn_speed
C $B3E9,4 Jump if < 2
N $B3ED turn_speed is 2.
C $B3ED,3 Load flip_car
C $B3F0,3 Jump if zero
C $B3F3,1 2 -> 3
C $B3F4,1 2/3 -> 3/4
C $B3F5,1 Bank/unbank
C $B3F6,6 B = counter_A & 1
C $B3FD,1 C = A
C $B3FF,1 A = C
C $B403,4 off_road = 0
C $B407,3 Call ahc_check_hand_flag
N $B40A Make the car bounce up and down when it goes off-road.
C $B40A,2 Default bounce of zero to pass to draw_car. It should be either 0 or 3.
C $B40C,6 Add the bounce only when one wheel is off-road (if off_road == 1)
C $B412,9 New bounce = (counter_C & 1) * 3
@ $B41B label=ahc_draw_car
C $B41B,3 Load turn_speed
C $B41E,3 Call draw_car
C $B421,6 Jump if cherry_light is zero
N $B427 Draw the cherry light.
C $B427,1 A = 0
C $B428,3 BC = $0102  -- size?
C $B42B,3 Call draw_cherry
N $B42E Draw smoke?
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
@ $B457 label=ahc_check_hand_flag
C $B457,5 Return if hand_flag is zero
N $B45C Start the animation.
@ $B45C label=ahc_hand_flag_non_zero
C $B45C,3 Jump if hand_flag is one
N $B45F Show the "stop" hand.
@ $B45F label=ahc_hand_flag_gt_one
C $B45F,1 Bank
C $B460,3 BC = A
C $B463,1 Unbank
C $B464,5 Is turn_speed 2?
C $B469,2 A = 36
C $B46B,3 Exit via #R$B69E if turn_speed != 2
N $B46E Otherwise turn_speed is 2 (turn hard).
C $B46E,5 A = flip_car + 37
C $B473,3 Exit via #R$B69E
N $B476 Start the animation.
@ $B476 label=ahc_hand_flag_one
C $B476,2 C = <self modified>
C $B478,2 A = <self modified>  [could be animation frame?]
C $B47A,1 A--
C $B47B,3 Self modify 'LD A' @ #R$B478 (above) to load A
C $B480,2 B = 2
C $B482,1 C++
C $B483,1 A = C
C $B484,4 Jump if >= 4
C $B488,1 A++
C $B489,1 C = A
C $B48A,4 Jump if != 2
C $B48E,1 B++
@ $B48F label=ahc_b48f
C $B48F,1 A = B
C $B490,3 Self modify 'LD A' @ #R$B478 (above) to load A
@ $B493 label=ahc_b493
C $B493,1 A = C
C $B494,3 Self modify 'LD C' @ #R$B476 (above) to load A
C $B497,4 Jump if >= 7
C $B49B,4 hand_flag = 0
C $B49F,1 Return
C $B4A0,3 A = turn_speed
C $B4A3,2 CP 2
C $B4A5,2 A = 6
C $B4A9,10 A = (A << 3) - flip_car + 13
C $B4B3,1 A += C
C $B4B9,1 C = A
C $B4BA,3 Read from 'LD C' @ #R$B476 (above) to load A
C $B4BD,4 Jump to #R$B4C6 if A >= 4
C $B4C1,1 C++
C $B4C2,1 A = C
C $B4C3,3 Exit via #R$B699
@ $B4C6 label=ahc_enable_cherry_light
C $B4C6,5 Enable the cherry_light
C $B4CB,1 Return
c $B4CC Perp sighted
D $B4CC Used by the routine at #R$A637.
@ $B4CC label=perp_sighted
C $B4CC,4 Self modify 'LD C' @ #R$B476 to load 0
C $B4D0,4 hand_flag = 1
C $B4D4,3 sighted_flag = 1
C $B4D7,1 A = 2
C $B4D8,3 Self modify 'LD A' @ #R$B478 to load A
C $B4DB,6 time_sixteenths/$A17D = 15, time_bcd/$A17E = $60
C $B4E1,3 Point #REGhl at left light's attributes
C $B4E4,3 Toggle its brightness
C $B4E7,3 Point at "SIGHTING OF TARGET VEHICLE" message
C $B4EA,3 Call setup_overlay_messages
C $B4ED,3 Exit via start_siren_hook
c $B4F0 Smash handling
@ $B4F0 label=smash
C $B4F0,8 Cycle #REGa one step through 0..3 each time the routine is entered
C $B4F8,14 *$B55C = #R$CE33 + #REGa * 6
C $B506,5 $B54A = 9
C $B50B,4 Load and increment smash_counter
C $B50F,2 20 hits? [POKE $B50F for Single hit capture]
C $B511,3 Exit via fully_smashed if so
C $B514,2 19 hits?
C $B516,2 Jump to smash_b522 if not
N $B518 We have 19 hits
C $B519,2 Set priority to 10
C $B51B,3 HL = raymond_says_one_more_time
C $B51E,3 Call start_chatter (priority 10)
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
@ $B549 label=sub_b549
C $B549,2 A = <self modified>
C $B54B,1 Set flags
C $B54C,1 Return if zero
C $B54D,1 A--
C $B54E,3 Self modify 'LD A' above
C $B551,1 Double A
C $B552,3 DE = A
C $B555,4 Self modify 'LD HL' below
C $B559,2 3 iterations
C $B55B,3 HL = <self modified> by #R$B503
C $B55E,1 Preserve #REGbc
C $B55F,4 DE = wordat(HL); HL += 2
C $B563,1 Preserve #REGhl
C $B564,4 A = (*DE + 1) & 3
C $B568,1 *DE = A
C $B569,2 A <<= 2
C $B56B,1 C = A
C $B56C,1 A <<= 1
C $B56D,2 C += A
C $B56F,1 DE++
C $B570,3 HL = <self modified>
C $B573,1 B = H
C $B574,1 HL += DE
C $B575,3 DE = wordat(HL)
C $B578,4 HL -> bitmap_debris_1/2/3/4
C $B57C,3 B = height, C = ?
C $B57F,1 Bank
C $B580,3 B = 0
C $B583,2 E = 1
C $B585,1 Unbank
C $B586,3 Presumably plotting the debris
C $B589,1 Restore #REGhl
C $B58A,1 Restore #REGbc
C $B58B,2 Loop
C $B58D,1 Return
c $B58E Draws the car
D $B58E Used by the routine at #R$B318.
R $B58E I:A Turn speed. 0/1/2 => Straight/Turning/Turning hard.
R $B58E I:B 0/3 to make the car wobble when off-road.
@ $B58E label=draw_car
C $B58E,6 If #REGa is zero (straight) then flip_car = 0
C $B594,1 C = A
C $B595,1 Preserve #REGc (#REGa from input)
C $B596,9 Point #REGhl at hero_car_shadow then add (#REGa * 4)
C $B59F,3 D = $78 (vertical postion in rows), E = $60 (horizontal position in pixels)
C $B5A2,2 C = 7
C $B5A4,3 Call draw_car_b627 to draw shadow
C $B5A7,1 Restore #REGc
N $B5A8 117 is the car's normal vertical position. Smaller values make it higher.
C $B5A8,4 117 - <self modified value $B5AB>  == car jump offset
C $B5AC,1 D = A
N $B5AD Build an index into hero_car_refs[].
C $B5AD,4 A = C (0/1/2, turn value from input) + B (counter value from input) + <self modified value $B5B0 == up/down facing (0/3/6 => level,up,down)>
C $B5B1,6 If A >= 9 A -= 9  -- clamping
C $B5B7,13 Point #REGhl at hero_car_refs[A] (rows/entries are 20 bytes wide)
C $B5C5,4 A = var_a22a + *HL
C $B5C9,2 A=0-A
C $B5CB,1 A+=D  (aka A=D-A)    D here is (117 - car jump offset) from earlier
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
C $B5EA,6 If flip_car jump to draw_car_perhaps_flipped
C $B5F1,3 Call plot_sprite -- #REGa is (how many pixels to plot - 1) / 8
C $B5F4,2 Jump to draw_car_cont
@ $B5F6 label=draw_car_perhaps_flipped
C $B5F7,1 L--
C $B5F8,3 Call plot_sprite_flipped
@ $B5FB label=draw_car_cont
C $B5FD,1 HL++
C $B5FE,7 E=$68 C=$05 (width) CALL #R$B627  draws the top (windscreen)
C $B605,7 E=$68 C=$05 (width) CALL #R$B627  draws the bottom (wheels)
C $B60C,6 Check flip_car flag
C $B612,4 unflipped
C $B616,2 flipped
C $B618,3 Draws left side of car
C $B61B,2 C = 1
C $B61D,3 A = flip_car
C $B620,1 Set flags
C $B621,2 E = $60
C $B623,2 Jump if flipped
C $B625,2 E = $90
N $B627 C = byte width? D = Y (vertical postion - in rows), E = X (horizontal position in pixels) HL -> address of graphic def (byte, n.rows, data address)
@ $B627 label=draw_car_b627
C $B627,1 A = D
C $B628,1 Preserve X,Y
C $B629,3 D = A - *HL++  -- this byte often zero
C $B62C,2 B = *HL++  -- num rows
C $B62E,5 HL = wordat(HL) while preserving (HL+1)
C $B633,1 Preserve byte width
C $B634,1 Bank
C $B635,1 Restore byte width
C $B636,3 A = flip_car
C $B639,1 B = flip_car
C $B63A,1 E = C
C $B63B,1 C--
C $B63C,4 If A == 0 C = A
C $B640,1 Unbank
C $B641,3 Draws all masked parts of the car
C $B644,1 Restore ptr
C $B645,1 Skip final byte
C $B646,1 Restore X,Y
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
C $B66C,2 Jump to #R$B673 if non-zero
C $B66E,1 C = A
C $B66F,1 Unbank
C $B670,1 A = E
C $B671,2 Jump to #R$B675
C $B673,1 Unbank
C $B674,1 A = D
C $B675,3 E = A + 127 -- horizontal position
C $B678,2 D = 119  -- vertical position
C $B67A,2 Exit via #R$B6D6
c $B67C Routine at B67C
D $B67C Used by the routine at #R$B318.
R $B67C I:A ?
R $B67C I:B ? (gets compared to turn_speed) e.g. 1
R $B67C I:C ? (used wrt flipping)           e.g. 2
@ $B67C label=draw_cherry
C $B67C,7 A += counter_C & 1
C $B683,1 Bank A
C $B684,6 Jump to #R$B698 if turn_speed < B
C $B68A,3 A = flip_car
C $B68D,1 Set flags
C $B68E,2 A = 0
C $B690,2 Jump to sub_b67c_no_flip if flip is zero
C $B692,1 A += C
@ $B693 label=draw_cherry_no_flip
C $B693,1 A += C
C $B694,1 C = A
C $B696,1 A += C
@ $B698 label=draw_cherry_b698
N $B699 This entry point is used by the routine at #R$B318.
@ $B699 label=draw_cherry_b699
C $B69A,3 BC = 0   -- not self modified
N $B69E This entry point is used by the routine at #R$B318.
@ $B69E label=draw_cherry_b69e
C $B69E,2 E = $80
C $B6A0,6 BC = A * 3
C $B6A6,4 HL = #R$CFB2 + BC  -> unknown_cfb2
C $B6AA,5 D = *HL++ + $79
C $B6AF,4 E = *HL++ + E
C $B6B3,1 C = *HL  -- B is still zero
C $B6B4,4 HL = &car_adornments + BC
C $B6B8,2 B = *HL++  -- height
C $B6BA,2 C = *HL++  -- byte width
C $B6BC,4 HL = wordat(HL); HL++   -- masked bitmap data
C $B6C0,1 A = C
C $B6C2,1 E = A  -- bank byte width in E'
C $B6C4,3 Read car jump offset from 'SUB x' @ #R$B5AA
C $B6C7,4 D -= A
C $B6CB,3 A = *$B5B0 -- Self modified value in draw_car
C $B6CE,2 A >>= 1
C $B6D0,4 Jump to #R$B6D6 if zero
C $B6D4,2 D += A - 2
N $B6D6 This entry point is used by the routines at #R$B58E and #R$B648.
@ $B6D6 label=draw_cherry_b6d6
C $B6D6,7 D -= var_a22a
N $B6DD This entry point is used by the routines at #R$8F5F and #R$B549.
@ $B6DD label=draw_cherry_b6dd
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
C $B6FB,1 Is this EX'ing AF to get the carry flag?
C $B6FC,1 HL += BC
N $B701 This entry point is used by the routine at #R$92E1.
@ $B701 label=draw_cherry_b701
C $B701,4 IX = plot_masked_sprite_core_thingy
C $B705,3 A = ~A + 9 == (8 - A)
C $B708,4 This multiplies by six - the length of each load-mask-store step in the plotter core.
C $B70C,5 Add that to #REGix
C $B711,2 B = 15 -- Set loop counter for 15 iterations?
C $B714,2 D = 0 -- fall through into plot_masked_sprite
c $B716 Masked sprite plotter
D $B716 This plots a lot of the game's masked graphics.
D $B716 The stack points to pairs of bitmap and mask bytes and HL must point to the screen buffer. Uses AND-OR type masking. Proceeds left-right. Doesn't flip the bytes so there must be an alternative for that.
R $B716 I:B Rows
R $B716 I:DE Stride (e.g. 10 for 40px wide)
R $B716 I:HL Address of data to plot
@ $B716 label=plot_masked_sprite
@ $B71C label=plot_masked_sprite_loop
C $B716,9 Save #REGsp for restoration on exit
C $B71F,5 Restore original #REGsp (self modified)
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
C $B775,4 Point #REGix at jump table
C $B779,3 Subtract 8
C $B77C,3 Multiply by 8  -- length of jump table sequences
C $B77F,5 Add to IX
C $B784,2 H = $EF
C $B78E,1 Bank
C $B78F,2 Next scanline
C $B791,3 Restore original #REGsp (self modified)
C $B794,1 Return
C $B795,1 Calculate address of next bitmap scanline
C $B796,1 Put it in #REGsp (so we can use POP for speed)
C $B797,1 Unbank
C $B79A,2 Jump
@ $B79C label=pmsf_jumptable
C $B79C,1 Load a bitmap and mask pair (B,C)
C $B79D,1 Set flip table index (assuming table is aligned)
C $B79E,1 Load the screen pixels
C $B79F,1 AND screen pixels with flipped mask
C $B7A0,1 Set flip table index
C $B7A1,1 OR in new flipped pixels
C $B7A2,1 Store back to the screen
C $B7A3,1 Move to next screen pixel (downwards in memory)
C $B7A4,55 <Repeat 9 times>
C $B7E6,2 Loop
C $B7EC,3 Loop
c $B7EF Routine at B7EF
D $B7EF Used by the routine at #R$92E1.
@ $B7EF label=sub_b7ef
C $B7EF,4 self modify #R$B71F - exit of plot_masked_sprite
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
b $B828 Horizon image related
D $B828 breaks/crashes road rendering if messed with
@ $B828 label=table_b828
W $B828,32,8
c $B848 Scroll the horizon
D $B848 Used by the routines at #R$8401, #R$852A and #R$873C.
R $B848 I:A' ?
@ $B848 label=scroll_horizon
C $B848,3 Load speed into #REGhl
C $B84B,3 Return if speed is zero
C $B84E,3 Load current_curvature into #REGa
C $B851,3 Jump if current_curvature is zero
C $B854,1 Bank current_curvature, which is non-zero here
C $B855,2 Bottom bit of #REGh moves to carry (#REGh now unused)
C $B857,5 A = (A << 3) & 6  [+ carry... I can't see where this banked A comes from to start with.]
C $B85C,8 BC = A + horizon_a25d
C $B864,3 16 word table at #R$B828
C $B867,1 HL += BC
C $B868,3 BC = wordat(HL)
N $B86B Decrement horizon_a25e.
C $B86B,3 HL = &horizon_a25e
C $B86E,1 *HL--
C $B86F,2 Jump if non-zero
N $B871 It became zero.
C $B871,1 *HL = B  [bottom byte of table entry]
C $B872,1 Bank [must be]
C $B873,1 A = C
C $B874,3 Jump if positive
C $B877,2 A = -A
@ $B879 label=sh_b879
C $B879,3 Address of operand in 'LD A,x' @ #R$C7E7
C $B87C,1 A += *HL
C $B87D,3 Jump if positive
C $B880,2 A += 20
@ $B882 label=sh_b882
C $B882,4 Jump if A < 20
C $B886,2 A -= 20
@ $B888 label=sh_b888
C $B888,1 *HL = A
@ $B889 label=sh_b889
C $B889,1 A = 0
C $B88A,1 B = 0
C $B88B,1 E = 0
C $B88C,1 Bank
C $B88D,5 Return if var_a258 is zero
C $B892,3 Jump if positive
C $B895,3 E = -(E + 1)
@ $B898 label=sh_b898
@ $B898 ssub=LD HL,table_b828 - 1
C $B898,3 32-byte table at #R$B828
C $B89B,1 C = A
C $B89C,1 HL += BC
C $B89D,4 C = var_a25b
C $B8A1,4 A = fast_counter - C
C $B8A5,1 Set flags
C $B8A6,1 Return if non-zero
C $B8A7,1 C = *HL
@ $B8A8 label=sh_b8a8
C $B8A8,1 A -= C
C $B8A9,2 Jump if carry
C $B8AB,1 B++
C $B8AC,1 Bank
C $B8AD,1 A += C
C $B8AE,1 Bank
C $B8AF,2 Jump
@ $B8B1 label=sh_b8b1
C $B8B1,1 A = B
C $B8B2,1 Set flags
C $B8B3,1 Return if zero
C $B8B4,3 HL = &var_a25a
C $B8B7,1 A += *HL
C $B8B8,1 *HL = A
C $B8B9,1 A = B
C $B8BC,2 B = 0
C $B8BE,2 Jump if no carry
C $B8C0,1 B--
C $B8C1,2 A = -A
@ $B8C3 label=sh_b8c3
C $B8C3,3 HL = horizon_level
C $B8C6,1 C = A
C $B8C7,1 HL += BC
C $B8C8,3 write it back
C $B8CB,1 Bank
C $B8CC,5 var_a25b += A
C $B8D1,1 Return
c $B8D2 Routine at B8D2
D $B8D2 Horizon stuff? Not sure.
R $B8D2 Used by the routine at #R$BDFB.
@ $B8D2 label=sub_b8d2
C $B8D2,3 A = var_a25a
C $B8D5,3 BC = A
C $B8D8,3 A = var_a258
C $B8DB,1 Set flags
C $B8DC,3 Jump if positive
C $B8DF,2 A = -A
C $B8E1,1 C++
C $B8E2,1 B--
C $B8E3,2 Jump if zero
C $B8E5,2 9-bit rotate right through carry
C $B8E7,2 B = 0
C $B8E9,2 Jump if no carry
C $B8EB,2 A = -A
C $B8ED,1 B--
C $B8EE,3 HL = horizon_level
C $B8F1,1 C = A
C $B8F2,1 HL += BC
C $B8F3,3 horizon_level = HL
C $B8F6,3 Load road_buffer_offset into #REGa
C $B8F9,2 Add (32+2) so it's the height data
C $B8FB,3 Point #REGhl at road buffer height data
C $B8FE,1 A = 0
C $B8FF,3 var_a25b = 0
C $B902,3 var_a25a = 0
C $B905,1 A = *HL
C $B908,3 Jump if positive
C $B90B,1 A++
C $B90C,3 var_a258 = A
C $B90F,2 L -= 2
C $B911,1 A = *HL
C $B912,1 C = A
C $B913,1 Set flags
C $B914,2 B = 0
C $B916,2 Jump if zero
C $B918,2 B = 6
C $B91A,3 Jump if positive
C $B91D,2 A = -A
C $B91F,2 B = 3
C $B921,4 Jump if A >= 3
C $B925,2 B = 0
C $B927,1 A = B
C $B928,3 Self modify 'ADD A,x' @ #R$B5AF
C $B92B,3 HL = &var_a259
C $B92E,1 A = *HL
C $B92F,1 Set flags
C $B930,3 Jump if positive
C $B933,2 Test bit 7
C $B935,2 Jump if non-zero
C $B937,4 A = -A - 2
C $B93B,2 Jump if carry
C $B93D,1 B = A
C $B93E,3 Load jump counter
C $B941,1 Set flags
C $B942,2 Jump if non-zero
C $B944,4 DE = speed
C $B948,2 9-bit rotate right via carry
C $B94A,1 A = E
C $B94B,1 9-bit rotate left via carry
C $B94D,6 D = ~(A & 3) + 4
C $B953,2 A = B - D
C $B955,4 Jump if <=
C $B959,1 Preserve HL
C $B95A,8 HL = #R$B057 + A*2  looks early.. might be 1-indexed? [sampled A: 3,2,4]
C $B962,2 E = *HL++  offset into B045
C $B964,1 Load new jump value [sampled: $B060]
C $B965,3 Set jump counter
C $B968,4 HL = #R$B045 + E  points into jump values table
C $B96C,3 Self modify #R$B079
C $B96F,1 Restore HL
C $B970,1 *HL = C
C $B971,3 Load current_curvature into #REGa
C $B974,1 Preserve it
C $B975,3 Load road_buffer_offset into #REGa
C $B978,3 Point #REGhl at road buffer curvature data
C $B97B,3 A = fork_visible
C $B97E,1 Set flags
C $B97F,1 Load a curvature byte
C $B980,2 Jump if fork_visible was zero
N $B982 Forked road is visible.
C $B982,6 Jump if fork_in_progress is zero [set while the road forks]
C $B988,4 Check fork_taken
C $B98C,1 Load a curvature byte again
C $B98D,2 Jump if left fork was taken
C $B98F,2 Negate if right fork was taken
C $B991,3 Store new curvature value to current_curvature
C $B994,1 Set flags
C $B995,1 E = A
C $B996,2 Jump if curvature byte was zero
C $B998,2 E = 1
C $B99A,3 Jump if positive
C $B99F,2 A = -A
C $B9A1,1 D = A
C $B9A2,2 A *= 4
C $B9A4,3 horizon_a25d = A
C $B9A7,1 B = A
C $B9A8,6 Jump if horizon_a25e is non-zero
C $B9AE,3 Load speed into #REGhl
C $B9B1,1 Speed low byte
C $B9B2,2 Bottom bit of #REGh moves to carry (#REGh now unused)
C $B9B4,5 A = (A << 3) & 6
C $B9B9,1 A += B
C $B9BA,3 BC = A
C $B9BD,3 HL -> table of 16 words
C $B9C0,1 HL += BC
C $B9C1,1 A = *HL
C $B9C2,3 horizon_a25e = A
C $B9C5,3 A = var_a262
C $B9C8,3 BC = A
C $B9CC,1 Set flags
C $B9CD,3 Jump if positive
C $B9D0,2 A = -A
C $B9D2,1 C++
C $B9D3,1 A -= B
C $B9D4,4 Jump if <=
C $B9D8,1 B = A
C $B9D9,2 A *= 4
C $B9DD,1 A += B
C $B9E0,2 B = 0
C $B9E2,2 Jump if no carry
C $B9E4,1 B--
C $B9E5,2 A = -A
C $B9E7,1 C = A
C $B9E8,4 var_a25f = BC
C $B9EC,1 A = 0
C $B9ED,3 var_a261 = 0
C $B9F0,3 var_a262 = 0
C $B9F3,1 Return
c $B9F4 Lays out the road
D $B9F4 Used by the routines at #R$8401, #R$852A and #R$873C.
@ $B9F4 label=layout_road
C $B9F4,3 Load road_buffer_offset into #REGa [as byte]
C $B9F7,2 Add 64 so it's the lanes data offset
C $B9F9,3 Point #REGde at road buffer lanes data
N $B9FC Suspect this is counting the distance to the forked road.
C $B9FC,3 #REGb = 20 iterations, #REGc = $E1, a mask
C $B9FF,2 Counter
C $BA01,1 Load a lanes byte
C $BA02,5 Jump to lr_forked_road if (lanes byte & $E1) == $E1
C $BA07,1 Advance to next lanes byte (wrapping around)
C $BA08,1 Increment counter
C $BA09,2 Loop while #REGb
N $BA0B No forked road found.
N $BA0B The road is still rendered if the following call is nopped out, but it's in the wrong position.
@ $BA0B label=lr_no_fork
C $BA0B,3 Call sub_cbce_non_fork
C $BA0E,4 Self modify 'LD SP' @ #R$BA4D to restore #REGsp on exit
C $BA12,3 Put $EC30 (road right) in #REGsp (so we can use POP for speed)
C $BA15,2 Iterate from $30 to $00 in steps of 2 = 104 iterations
N $BA17 Note that the fork case branches back here.
@ $BA17 label=lr_ba17
C $BA17,3 Self modify #R$BA35 to load $EA30 upwards (road centre)
C $BA1A,3 Self modify #R$BA3F to load $EB30 upwards (road centre right)
C $BA1D,3 Self modify #R$BA44 to load $E930 upwards (road centre left)
@ $BA20 label=lr_ba20
C $BA20,3 Self modify the locations setup above
C $BA23,3 Self modify #R$BA27 (below) to load ($E800 | A) (road left)
C $BA26,1 Bank
N $BA27 Centre = Left + (Right - Left) / 2
C $BA27,4 DE = *$E8xx (xx = self modified) (road left)
C $BA2B,1 Read from $ECxx
C $BA2C,2 HL = *$ECxx - *$E8xx  [total width]
C $BA2E,4 Divide HL by 2  [halve total width]
C $BA32,2 BC = HL  [keep a copy]
C $BA34,1 HL += DE  == (*$ECxx - *$E8xx) / 2 + *$E8xx  [calc centre from left]
C $BA35,3 *$EAxx = HL (xx = self modified) (road centre)  [store centre pos]
C $BA38,2 DE = BC  [copy halved width again]
C $BA3A,4 Divide BC by 2   [divide by 2 again for quarter width]
C $BA3E,1 == [centre] + [quarter width]
C $BA3F,3 *$EBxx = HL (xx = self modified) (road centre right) [store centre-right pos]
C $BA42,2 == [[centre] + [quarter width]] - [half total width]
C $BA44,3 *$E9xx = HL (xx = self modified) (road centre left) [store centre-left pos]
C $BA47,2 A += 2 [remember A's not banked by EXX]
C $BA49,1 Unbank
C $BA4A,3 Loop while #REGa
@ $BA4D label=lr_exit
C $BA4D,3 Restore original #REGsp (self modified)
C $BA50,1 Return
N $BA51 Handle a forked road.
@ $BA51 label=lr_forked_road
C $BA52,3 fork_countdown = A
C $BA55,2 HL = $E3xx
C $BA57,5 Set fork_visible
C $BA5C,4 A = 106 - (A - *HL)  -- HL is $E3xx
C $BA60,1 Preserve A
C $BA61,3 Load fork_distance
C $BA64,3 A = *DE & 4  -- DE is the lanes byte ptr
C $BA67,2 Jump if non-zero
N $BA69 Lanes byte & 4 is zero. Hit when the road forks (during the fork itself).
C $BA69,4 A = fork_in_progress - 1
C $BA6D,2 Jump to lr_check_spawning if zero
N $BA6F Otherwise A has 255? NEG turns that back to zero. Why not XOR A which is shorter?
C $BA6F,5 fork_in_progress = -fork_in_progress + 1 = ~fork_in_progress
C $BA74,4 Get road position
C $BA78,2 A = 1
C $BA7A,1 D--
C $BA7B,3 jump with A==1  => right fork
C $BA7E,3 jump with A==1  => left fork
C $BA81,1 A = E  -- E is ?
C $BA82,6 Jump with A==1 if E < 12
@ $BA88 label=lr_left_fork
C $BA88,1 A should be 1, will become zero
@ $BA89 label=lr_check_correct_fork_taken
C $BA89,3 Set fork_taken to #REGa
C $BA8C,1 0/1 -> 1/2
C $BA8D,1 Preserve HL (holds fork_distance)
C $BA8E,3 Address of correct_fork
C $BA91,1 Match?
C $BA92,3 -> Tony: "LET'S GO. MR. DRIVER." <STOP>
C $BA95,2 Jump to lr_chatter if correct fork was taken
N $BA97 Incorrect fork was taken.
@ $BA99 ssub=LD (hazard_0 + 13),A
C $BA97,5 Set the horizontal position of the perp's car to $5F
N $BA9C Add a bonus of (40,000 + 10,000 * level number) for going the wrong way (eh?)
C $BA9C,5 A = wanted_stage_number + 4
C $BAA1,1 Set top digits
C $BAA2,1 Clear low digit
C $BAA3,1 Clear middle digits
C $BAA4,3 Call bonus
C $BAA7,3 -> Raymond: "WHAT ARE YOU DOING MAN!!" / "THE BAD GUYS ARE GOING THE OTHER WAY." <STOP>
@ $BAAA label=lr_chatter
C $BAAA,5 Call start_chatter (with priority 20)
C $BAAF,1 Restore HL (holds fork_distance)
@ $BAB0 label=lr_check_spawning
C $BAB0,3 Load allow_spawning
C $BAB3,3 Jump to lr_bacc if zero
N $BAB6 allow_spawning is non-zero.
C $BAB6,5 A = allow_spawning + var_a16d
C $BABB,1 C = A  -- copy so we can destroy A
C $BABC,2 A -= 2
C $BABE,2 Jump if A < 2
N $BAC0 Otherwise clamp it to 2.
C $BAC0,1 C = A
N $BAC1 Adjusting this delta will cause the forked road effect to proceed faster (higher values) or slower (lower values).
C $BAC1,7 Increase fork_distance by 16
@ $BAC8 label=lr_bac8
C $BAC8,4 var_a16d = C
@ $BACC label=lr_no_car_spawning
C $BACC,3 A = var_a16d
C $BACF,1 Getting carry?
C $BAD0,9 A = (fast_counter >> 4)
C $BAD9,2 A -= 16  -- sets top nibble to $F
C $BADB,3 DE = $FF00 | A
C $BADE,1 HL += DE
N $BADF Lanes byte & 4 is non-zero. Hit when the road forks (as it becomes visible).
@ $BADF label=lr_badf
C $BADF,1 Preserve HL
C $BAE0,6 Jump if fork_taken was 1 (right fork taken)
N $BAE6 Left fork was taken.
@ $BAE6 label=lr_left_fork_taken
C $BAE6,3 Call sub_cbce_non_fork
C $BAE9,1 Restore HL from earlier
C $BAEA,3 Get road position
C $BAED,1 Stack it
C $BAEE,1 DE must be a road position delta?
C $BAEF,3 Set road position
C $BAF2,3 Call sub_cbce_fork
N $BAF7 Right fork was taken.
@ $BAF7 label=lr_right_fork_taken
C $BAF7,3 Call sub_cbce_fork
C $BAFA,1 Pop result?
C $BAFB,3 Get road position
C $BAFE,1 Stack it
C $BAFF,2 DE must be a road position delta?
C $BB01,3 Set road position
C $BB04,3 Call sub_cbce_non_fork
@ $BB07 label=lr_bb07
C $BB07,1 Unstack road position
C $BB08,3 Set road position
C $BB0B,1 B used as a counter in a moment
C $BB0C,4 Self modify 'LD SP' @ #R$BA4D to restore #REGsp
C $BB10,3 Put $EC30 (road right) in #REGsp (so we can use POP for speed)
C $BB13,2 Iterate from $30 to $00 in steps of 2 = 104 iterations [CHECK]
C $BB15,3 Self modify 'LD ($EAxx),HL' @ #R$BB37 to be $EA30 (road centre)
C $BB18,3 Self modify 'LD ($E9xx),HL' @ #R$BB3F to be $E930 (road centre left)
@ $BB1B label=lr_loop
C $BB1B,2 Self modify the locations setup above
C $BB1D,3 Self modify 'LD DE,($E8xx)' @ #R$BB2D to be $E830 (road left)
C $BB20,3 Self modify 'LD DE,($EDxx)' @ #R$BB42 to be $ED30
C $BB23,3 Self modify 'LD HL,($EB00)' @ #R$BB46 to be $EB30 (road centre right)
C $BB26,3 Self modify 'LD ($EB00),HL' @ #R$BB4E to be $EB30 (road centre right)
C $BB29,3 Self modify 'LD ($EC00),HL' @ #R$BB56 to be $EC30 (road right)
C $BB2C,1 Bank during inner loop
C $BB2D,4 DE = *$E8xx (xx = self modified) (road left)
C $BB31,1 Read from $ECxx
C $BB32,5 (Right + Left) / 2  == new road centre
C $BB37,3 *$EAxx = HL (xx = self modified) (store road centre)
C $BB3A,5 (Centre + Left) / 2  == new road centre left
C $BB3F,3 *$E9xx = HL (xx = self modified) store (road centre left)
C $BB42,4 DE = *$EDxx (xx = self modified) (?)
C $BB46,3 HL = *$EBxx (xx = self modified) (road centre right)
C $BB49,5 (Centre-right + ?) / 2  == new road centre right
C $BB4E,3 *$EBxx = HL (xx = self modified) store (road centre right)
C $BB51,5 (Centre-right) + ?) / 2 == new road right
C $BB56,3 *$ECxx = HL (xx = self modified) (road right)
C $BB59,2 A += 2
C $BB5B,1 Unbank
C $BB5C,2 Loop while #REGb > 0
C $BB5E,4 Jump to lr_exit if A is zero
C $BB62,4 Point #REGsp at $EC00 | A (road right)
C $BB66,3 Jump back to #R$BA17
c $BB69 Handle exiting from a road fork
D $BB69 Used by the routine at #R$8401.
N $BB69 When fork_distance exceeds 255 we can proceed.
@ $BB69 label=exit_fork
@ $BB69 ssub=LD A,(fork_distance + 1)
C $BB69,5 Return if (fork_distance & $FF00) is zero
N $BB6E Reset road setup back to a single road now that the forked road has completed.
C $BB6E,4 A = fork_taken - 1
C $BB72,2 Jump if A is zero (right fork taken)
N $BB74 Set up left route.
@ $BB74 label=ef_left
C $BB74,3 D,E = $FC,$01  (curvature byte -4 => super hard (?) left turn, lanes byte = 1)
C $BB77,1 Save for later
@ $BB78 ssub=LD HL,forked_road_leftobjs - 1
C $BB78,6 road_leftside_ptr  = $E2D9
@ $BB7E ssub=LD HL,forked_road_rightobjs - 1
C $BB7E,6 road_rightside_ptr = $E2D5
@ $BB84 ssub=LD HL,forked_road_exit_lanes_left - 1
C $BB84,6 road_lanes_ptr     = $E2E7
C $BB8A,3 Self modified by #R$C035 to be the left route's hazards data
C $BB8D,3 Self modified by #R$BF73 to be the left route's rightside data
C $BB90,3 Self modified by #R$BFBC to be the left route's leftside data
C $BB93,1 Bank
C $BB94,3 Self modified by #R$BE5C to be the left route's curvature data
C $BB97,3 Self modified by #R$BEAE to be the left route's height data
C $BB9A,3 Self modified by #R$BEF9 to be the left route's lanes data
C $BB9D,2 A = 64  -- additional offset into road buffer (presumably left objects)
C $BB9F,2 Jump to ef_set_handlers
N $BBA1 Set up right route.
@ $BBA1 label=ef_right
C $BBA1,3 D,E = $04,$03  (curvature byte +4 => super hard (?) right turn, lanes byte = 3)
C $BBA4,1 Save for later
N $BBA5 Note the switch around here.
@ $BBA5 ssub=LD HL,forked_road_rightobjs - 1
C $BBA5,6 road_leftside_ptr  = $E2D5
@ $BBAB ssub=LD HL,forked_road_leftobjs - 1
C $BBAB,6 road_rightside_ptr = $E2D9
@ $BBB1 ssub=LD HL,forked_road_exit_lanes_right - 1
C $BBB1,6 road_lanes_ptr     = $E2F3
C $BBB7,3 Self modified by #R$C03D to be the right route's hazards data
C $BBBA,3 Self modified by #R$BF73 to be the right route's rightside data
C $BBBD,3 Self modified by #R$BFC4 to be the right route's leftside data
C $BBC0,1 Bank
C $BBC1,3 Self modified by #R$BE62 to be the right route's curvature data
C $BBC4,3 Self modified by #R$BEB6 to be the right route's height data
C $BBC7,3 Self modified by #R$BF01 to be the right route's lanes data
C $BBCA,2 A = 32  -- additional offset into road buffer (presumably right objects)
@ $BBCC label=ef_set_handlers
C $BBCC,3 Self modify rm_curvature_one_command handler
C $BBCF,4 Self modify rm_height_one_command handler
C $BBD3,4 Self modify rm_lanes_one_command handler
C $BBD7,1 Unbank
C $BBD8,4 Self modify rm_leftside_one_command handler
C $BBDC,4 Self modify rm_rightside_one_command handler
C $BBE0,3 Self modify rm_hazards_one_command handler
@ $BBE3 ssub=LD HL,forked_road_exit_curvature - 1
C $BBE3,6 road_curvature_ptr = $E2DD
@ $BBE9 ssub=LD HL,forked_road_exit_height - 1
C $BBE9,6 road_height_ptr    = $E2E2
@ $BBEF ssub=LD HL,forked_road_hazards - 1
C $BBEF,6 road_hazard_ptr    = $E2D2
C $BBF5,1 Suspected road curve type left/right
C $BBF6,1 Copy road buffer additional offset
N $BBF7 The code commonly loads the byte at #R$A240 and merges it with $EE00, yet #R$A240 already contains a whole word with the right value.
C $BBF7,3 Load road_buffer_offset into #REGa [as byte]
C $BBFA,3 Point #REGhl at road buffer curvature data
N $BBFD Set the 32 bytes at #REGhl to the road curvature type in #REGd.
@ $BBFF label=ef_set_curvature_loop
N $BC03 Set the 32 bytes at #REGhl + 64 to the lane type in #REGe.
C $BC03,2 A = road_buffer_offset + 64
@ $BC08 label=ef_set_lanes_loop
N $BC0C Zero the 32 bytes at #REGhl + 64 + #REGc (the additional offset that was setup earlier). This seems to be objects.
C $BC0E,1 E = 0, since B is zero
@ $BC11 label=ef_set_objects_loop
C $BC15,5 curvature_byte     = 0
C $BC1A,3 height_byte         = 0
C $BC1D,3 leftside_byte       = 0
C $BC20,3 rightside_byte      = 0
C $BC23,3 hazards_byte        = 0
C $BC26,3 lanes_counter_byte  = 0
C $BC29,3 fork_in_progress    = 0
C $BC2C,3 fork_taken          = 0
C $BC2F,3 fork_visible        = 0
C $BC32,4 no_objects_counter = 1
C $BC36,3 var_a16d            = 1
C $BC39,4 fork_distance       = 0  [B & C are zero here]
C $BC3D,1 Return
c $BC3E Copies the backbuffer at $F000 to the screen
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
C $BD70,3 Load the address of the first line of ground attributes (+ 31)
C $BD73,3 Set the sky colour screen attributes (always black over bright cyan)
C $BD76,2 If A was zero then jump (Z => sky, NZ => ground)
C $BD78,4 Load the ground colour screen attributes (varies per level)
C $BD7C,2 Point #REGsp at the screen attributes (DE = $FFE0 = -32)
C $BD7E,15 Fill 30 bytes - length of attribute line minus the two blank edges
C $BD8F,1 Move to next line of attributes
C $BD90,3 Save the address of the first line of ground attributes (+ 31)
C $BD93,6 Exit if sighted_flag is zero (flashing lights / smash mode)
C $BD99,7 Jump if perp_caught_phase >= 3 (when car stops)
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
C $BDBD,3 Restore original #REGsp (self modified at start of routine)
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
C $BDFB,4 var_a23d = 0  -- state var TBD
C $BDFF,3 var_a23c = 0  -- state var TBD
C $BE02,3 Clear allow_spawning
C $BE05,3 HL = Address of fast_counter
C $BE08,4 DE = speed
C $BE0C,1 A = Bottom byte of speed
C $BE0D,4 If speed < 255 then jump
C $BE11,2 Preserve
C $BE13,3 Call subfunction #R$BE1F  -- processing something twice as much when at high speed?
C $BE16,2 Restore
@ $BE18 label=rm_be18
C $BE18,2 fast_counter += bottom byte of speed in A
C $BE1A,2 A = 0  -- disallow car spawning
C $BE1C,3 Jump to rm_inc_spawning if no carry
N $BE1F This entry point is used by the routine at #R$87DC.
@ $BE1F label=rm_cycle_buffer_offset
C $BE1F,1 Set #REGhl to address of road_buffer_offset
C $BE20,3 Increment road_buffer_offset
C $BE23,5 HL = $EE00 | (A + 95)  -- final byte of lanes data? or compensating for previous increment?
C $BE28,7 var_a23c |= *HL
C $BE2F,4 L += 32    -- offset 128
C $BE33,7 var_a23d |= *HL
C $BE3A,4 L -= 96    -- offset 32
N $BE3E -- CURVATURE --
N $BE3E The top nibble of each byte is a counter. The bottom nibble is curvature data.
C $BE3E,5 A = curvature_byte - 16
C $BE43,2 Jump to rm_save_curvature_byte if curvature_byte >= 16
N $BE45 Load a new curvature byte.
C $BE45,5 Read and increment address of curvature data
C $BE4A,1 Read a curvature byte
C $BE4B,3 Jump to rm_curvature_regular_byte if non-zero
N $BE4E Handle curvature escape byte.
@ $BE4E label=rm_curvature_escape_byte
C $BE4E,1 #REGhl is now address of escape byte
C $BE4F,3 Increment, read command byte, increment again
C $BE52,3 Jump to rm_curvature_jump_command if command byte is zero
C $BE55,3 Jump to rm_curvature_one_command if command byte is one
N $BE58 Otherwise it must be a fork road command (byte == 2).
C $BE58,4 Load address of left route's curvature data
C $BE5C,4 Self modify 'LD HL,$xxxx' @ #R$BB94 to load the address
C $BE60,4 Load address of right route's curvature data
C $BE64,3 Self modify 'LD HL,$xxxx' @ #R$BBC1 to load the address
C $BE67,3 HL = #R$E2C6 -> forked road left curvature data
C $BE6A,2 Jump to rm_read_curvature
N $BE6C Handle a command byte of one: TBD.
@ $BE6C label=rm_curvature_one_command
C $BE6C,3 HL = <map curvature address> -- Self modified by #R$BBCC
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
C $BE9D,3 Jump to rm_height_regular_byte if non-zero
N $BEA0 Handle height escape byte.
C $BEA0,1 #REGhl is now address of escape byte
C $BEA1,3 Increment, read command byte, increment again
C $BEA4,4 Jump to rm_height_jump_command if command byte is zero
C $BEA8,2 Jump to rm_height_one_command if command byte is one
N $BEAA Otherwise it must be a fork road command (byte == 2).
C $BEAA,4 Load address of left route's height data
C $BEAE,4 Self modify 'LD DE,$xxxx' @ #R$BB97 to load the address
C $BEB2,4 Load address of right route's height data
C $BEB6,3 Self modify 'LD DE,$xxxx' @ #R$BBC4 to load the address
C $BEB9,3 HL = #R$E2CC -> forked road height data
C $BEBC,2 Jump to rm_read_height
@ $BEBE label=rm_height_one_command
C $BEBE,3 HL = <something>  -- Self modified by #R$BBCF
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
C $BEE8,3 Jump to rm_lanes_regular_byte if non-zero
N $BEEB Handle lanes escape byte.
C $BEEB,1 #REGhl is now address of escape byte
C $BEEC,3 Increment, read command byte, increment again
C $BEEF,3 Jump to rm_lanes_jump_command if command byte is zero
C $BEF2,3 Jump to rm_lanes_one_command if command byte is one
N $BEF5 Otherwise it must be a fork road command (byte == 2).
C $BEF5,4 Load address of left route's lanes data
C $BEF9,4 Self modify 'LD BC,$xxxx' @ #R$BB9A to load the address
C $BEFD,4 Load address of right route's lanes data
C $BF01,3 Self modify 'LD BC,$xxxx' @ #R$BBC7 to load the address
C $BF04,3 HL = #R$E2D1 -> forked road lanes data
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
N $BF1D Store (map byte & $F7) to cyclic road buffer (HL).
C $BF1D,4 *HL = *DE & $F7
N $BF21 Store (map byte & $FB) to LD A below.
C $BF21,3 A = *DE & $FB
C $BF24,3 Self modify 'LD A,$xx' @ #R$BF2C (below)
C $BF27,2 Jump to rm_lanes_done
N $BF29 Resume updating lanes data.
@ $BF29 label=rm_lanes_count_resume
C $BF29,3 Store decremented lanes counter
C $BF2C,2 A = <self modified value>  -- Self modified above AND below
C $BF2E,1 C = A  -- Save temporarily
C $BF2F,4 Jump to rm_lanes_bf39 if (A & $0C) is zero
N $BF33 (bottom two bits significant)  $F3 is complement of $0C
C $BF33,3 A = C & $F3  -- discard bit 3 only?
C $BF36,3 Self modify 'LD A,$xx' @ #R$BF2C (above)
@ $BF39 label=rm_lanes_bf39
C $BF39,1 Store to cyclic road buffer (HL)
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
C $BF62,3 Jump to rm_rightside_regular_byte if non-zero
N $BF65 Handle rightside escape byte.
C $BF65,1 #REGhl is now address of escape byte
C $BF66,3 Increment, read command byte, increment again
C $BF69,3 Jump to rm_rightside_jump_command if command byte is zero
C $BF6C,3 Jump to rm_rightside_one_command if command byte is one
N $BF6F Otherwise it must be a fork road command (byte == 2).
C $BF6F,4 Load address of left route's rightside data
C $BF73,4 Set modify 'LD DE,$xxxx' @ #R$BB8D to load the address
C $BF77,4 Load address of right route's rightside data
C $BF7B,3 Self modify 'LD DE,$xxxx' @ #R$BBBA to load the address
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
C $BFAB,3 Jump to rm_leftside_regular_byte if non-zero
N $BFAE Handle leftside escape byte.
C $BFAE,1 #REGhl is now address of escape byte
C $BFAF,3 Increment, read command byte, increment again
C $BFB2,3 Jump to rm_leftside_jump_command if command byte is zero
C $BFB5,3 Jump to rm_leftside_one_command if command byte is one
N $BFB8 Otherwise it must be a fork road command (byte == 2).
C $BFB8,4 Load address of left route's leftside data
C $BFBC,4 Self modify 'LD BC,$xxxx' @ #R$BB90
C $BFC0,4 Load address of right route's leftside data
C $BFC4,3 Self modify 'LD BC,$xxxx' @ #R$BBBD
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
C $BFF4,3 Jump to rm_hazards_regular_byte if non-zero
N $BFF7 Handle hazards escape byte.
C $BFF7,1 #REGhl is now address of escape byte
C $BFF8,3 Increment, read command byte, increment again
C $BFFB,4 Jump to rm_hazards_jump_command if command byte is zero
C $BFFF,2 Jump to rm_hazards_one_command if command byte is one
C $C001,4 Jump to rm_hazards_fork_road_command if command byte is two
C $C005,4 Jump to rm_hazards_set_hazard_command if command byte is < 10 -- 3..9
C $C009,4 Jump to rm_hazards_set_arrow_command if command byte is < 13 -- 10..12
C $C00D,4 Jump to rm_hazards_spawning_command if command byte is < 15 -- 13..14
C $C011,5 Otherwise helicopter_control = A - 11
C $C016,2 Jump to rm_do_restart
N $C018 Handle car spawning commands (13/14 = enable/disable car spawning).
@ $C018 label=rm_hazards_spawning_command
C $C018,5 stop_car_spawning = A - 10  (13/14 in map data)
C $C01D,2 Jump to rm_do_restart
N $C01F Handle floating arrow commands (10/11/12 = off/left/right).
@ $C01F label=rm_hazards_set_arrow_command
C $C01F,5 floating_arrow = A - 7  (10/11/12 in map data)
C $C024,2 Jump to rm_do_restart if zero
C $C026,3 correct_fork = A  (0 or 1)
@ $C029 label=rm_do_restart
C $C02A,2 Jump to rm_restart_hazards_read
N $C02C Handle hazard commands (3 = off, 4/5/6 = tumbleweeds left/right/both, 7/8/9 => barriers)
N $C02C When on left it uses lane 1, on right it uses lane 4, both it uses lane 2 and 3.
@ $C02C label=rm_hazards_set_hazard_command
C $C02C,3 Self modify 'LD (HL),xxxx' at #R$C058 to load A [which is command_no-3]
C $C02F,2 Jump to rm_read_hazards
@ $C031 label=rm_hazards_fork_road_command
C $C031,4 Load address of left route's hazards data  -- Sampled HL: $5F26 (road fork)
C $C035,4 Self modify 'LD HL,$xxxx' @ #R$BB8A
C $C039,4 Load address of right route's hazards data
C $C03D,3 Self modify 'LD HL,$xxxx' @ #R$BBB7
C $C040,3 HL = #R$E2B8
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
C $C05F,3 Call #R$B8D2
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
C $C077,3 HL = &allow_overtake_bonus
C $C07A,3 HL = &allow_overtake_bonus  -- duplicate instruction
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
C $C0A5,3 Jump to rm_c072_continue if non-zero
C $C0A8,4 IX[1] = 1
C $C0AC,4 IX[4] = 255
C $C0B0,2 Jump to rm_c072_continue
@ $C0B2 label=rm_c0b2
C $C0B2,7 IX[17]--
C $C0B9,2 Jump to rm_c072_continue
@ $C0BB label=rm_c0bb
C $C0BB,2 A = <self modified>  -- Self modified by #R$A977 + #R$A9A0 only
C $C0BD,4 Jump to rm_allow_car_spawning if zero
N $C0C1 Some sort of block copy.
C $C0C1,3 HL = $ED73  -- Source+2. This must be a buffer pointer at this point
C $C0C4,3 DE = $ED77  -- Destination+2
C $C0C7,3 BC = 38  -- Bytes?
@ $C0CA label=rm_c0ca_loop
C $C0CA,2 HL -= 2
C $C0CC,2 DE -= 2
C $C0CE,2 *DE-- = *HL--; BC--
C $C0D0,2 *DE-- = *HL--; BC--
N $C0D2 This seems to get hit in the dirt track section.
C $C0D2,3 Loop to rm_c0ca while B != 0 (LDD sets P/V if BC != 0)
C $C0D5,2 *++HL = 0  (since B is zero)
@ $C0D7 label=rm_allow_car_spawning
C $C0D7,2 A = 1
@ $C0D9 label=rm_inc_spawning
C $C0D9,5 Increment allow_spawning by #REGa (which should be 0 or 1)
C $C0DE,3 Exit via #R$AD0D
c $C0E1 Set up the tunnel
D $C0E1 Used by the routines at #R$8401, #R$852A and #R$873C.
@ $C0E1 label=prepare_tunnel
C $C0E1,3 Read 'LD A,x' @ #R$C160 (in draw_tunnel)
C $C0E4,1 Set flags
C $C0E5,3 Read 'LD A,x' @ #R$C88F (tunnel related)
C $C0E8,2 Jump if tunnel has appeared
N $C0EA Tunnel hasn't appeared.
@ $C0EA label=no_tunnel
C $C0EA,1 Set flags
C $C0EB,2 Jump if non-zero
C $C0ED,4 tunnel_sfx = 0
C $C0F1,2 #REGhl = 0
N $C0F3 Remove draw_tunnel calls.
C $C0F3,3 #R$8F82 = NOP (instruction)
C $C0F6,3 #R$8F83 & #R$8F84 = NOP (instruction)
C $C0F9,3 #R$8FA7 = NOP (instruction)
C $C0FC,3 #R$8FA8 & #R$8FA9 = NOP (instruction)
C $C0FF,1 Return
N $C100 Tunnel has appeared.
@ $C100 label=yes_a_tunnel
C $C100,5 tunnel_sfx = 5  -- this quietens noises when in the tunnel
C $C105,3 -- somewhere in road height data table
C $C108,6 #REGbc = wordat(HL); #REGhl -= 4
C $C10E,1 Bank
C $C10F,3 -- somewhere in road height data table
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
C $C12E,1 Swap
C $C12F,2 #REGhl -= #REGbc
C $C131,2 Jump if #REGhl < BC
C $C133,1 Swap
C $C134,2 #REGde = #REGbc
C $C136,1 A--
C $C137,2 Loop to tunnel_loop
C $C139,3 A = 9 - A
C $C13C,3 Self modify 'CP x' @ #R$C15D  [15 when tunnel is small, 6 when fills screen]
C $C13F,2 A = 2
C $C141,3 Self modify 'LD A,x' @ #R$C160 (in draw_tunnel below)
C $C144,2 A = 3
C $C146,3 Self modify 'LD A,x' @ #R$C2B8
N $C149 Self modify #R$8F82 and #R$8FA7 to be CALL draw_tunnel.
C $C15A,1 Return
c $C15B Tunnel entrance/interior/exit drawing code
D $C15B Called by $8F82 etc. being self modified.
R $C15B I:IY e.g. $E315
@ $C15B label=draw_tunnel
C $C15B,2 A = IY.low
C $C15D,2 Compare to <self modified>
C $C15F,1 Return if non-zero
C $C160,2 A = <self modified>
C $C162,2 D = $EE
C $C164,1 A--
C $C165,2 Jump if zero
C $C167,2 D = $FF
C $C169,1 E = D
C $C16A,4 Self modify 'LD DE,x' @ #R$C21C
C $C16E,4 Save #REGsp to restore on exit (self modify)
C $C172,8 L = ~((IY[$4E] - 2) << 1)
C $C17A,2 C = 0
C $C17C,2 H = $EB
C $C17E,1 A = *HL
C $C17F,1 Set flags
C $C180,3 Jump if zero
C $C183,2 D = 16
C $C185,3 Jump if positive
C $C188,2 D = 22
C $C18A,2 L = 31
C $C18C,2 Jump
C $C18E,3 A = HL[-1]
C $C191,5 Divide by 8
C $C196,2 A >>= 1
C $C198,2 Rotate C
C $C19A,1 A++
C $C19B,1 D = A
C $C19C,2 H = $E8
C $C19E,1 A = *HL
C $C19F,1 Set flags
C $C1A0,2 Jump if zero
C $C1A2,2 E = 16
C $C1A4,3 Jump if negative
C $C1A7,3 Jump
C $C1AA,1 L--
C $C1AB,3 A = *HL + 8
C $C1AE,2 Jump if carry
C $C1B0,5 Divide by 8
C $C1B5,1 B = A
C $C1B6,6 E = 16 - (A >> 1)
C $C1BC,3 A = 32 - B
C $C1BF,2 L = 32
C $C1C1,2 Rotate left
C $C1C3,2 Jump if carry
C $C1C5,1 L--
C $C1C6,1 A--
C $C1C7,1 C = A
C $C1C8,1 A = D
C $C1C9,3 Self modify 'JR x' @ #R$C221 -- jump table target
C $C1CC,1 A = E
C $C1CD,3 Self modify 'JR x' @ #R$C236 -- jump table target
C $C1D0,3 A = IY[$35]
C $C1D3,1 B = A
C $C1D4,5 H = (A & 15) + $F0
C $C1D9,5 A = ((B & $70) << 1) + L
C $C1DE,2 Jump if no carry
C $C1E0,1 A--
C $C1E1,1 L = A
C $C1E2,3 A = 128 - B
C $C1E5,1 Bank/unbank
C $C1E6,6 E = fast_counter & $E0
C $C1EC,4 E >>= 2
C $C1F0,1 A -= E
C $C1F1,4 E >>= 2
C $C1F5,1 A -= E
C $C1F6,2 A += IY.low
C $C1F8,2 D = $E6
C $C1FA,1 E = A
C $C1FB,5 B = IY[0] - B
C $C200,1 A = *DE
C $C201,1 E = A
C $C202,6 A = (16 - IY.low) + E
C $C208,1 Bank/unbank
C $C209,1 B = A
C $C20A,1 Bank/unbank
C $C20B,1 A -= B
C $C20C,1 D = A
C $C20D,1 Bank/unbank
C $C20E,1 A = D
C $C20F,3 Jump if positive
C $C212,1 E = A
C $C213,2 A -= $81
C $C215,2 A = -A
C $C217,1 A += D
C $C218,1 D = A
C $C219,1 A = E
C $C21A,1 Bank/unbank
C $C21B,1 B = D
N $C21C Pixels of tunnel loaded here. Top byte, D, seems to affect bottom row? Bottom byte, E, affects whole pattern. The LD E,D later would explain that.
C $C21C,3 DE = <self modified>  -- pixels of tunnel
N $C21F Loop
C $C21F,1 Put it in #REGsp (so we can use PUSH for speed)
C $C220,1 A = L
C $C221,2 Jump table (self modified)
C $C233,1 A -= C
C $C234,1 L = A
C $C235,1 Put it in #REGsp (so we can use PUSH for speed)
C $C236,2 Jump table (self modified)
C $C248,1 A += C
C $C249,1 L = A
C $C24A,1 A = H
C $C24B,1 H--
C $C24C,2 A &= 15
C $C24E,3 Jump if non-zero
C $C251,4 L -= 32
C $C255,2 Jump if carry
C $C257,4 H -= 16
C $C25D,1 E = D
C $C25E,2 Loop
C $C260,1 Bank/unbank
C $C261,2 B >>= 1
C $C263,1 A = B
C $C264,4 B >>= 2
C $C268,1 A -= B
C $C269,1 B = A
C $C26A,1 Bank/unbank
C $C26B,1 Set flags
C $C26C,3 Jump if negative
C $C26F,1 A += B
C $C270,3 Jump if positive
C $C273,1 E = A
C $C274,2 A -= $81
C $C276,2 A = -A
C $C278,1 A = B
C $C279,1 B = A
C $C27A,1 A = E
C $C27B,1 Bank/unbank
C $C27C,1 A = B
C $C27D,1 Bank/unbank
C $C27E,1 B = A
C $C27F,1 A = L
C $C280,2 A &= 15
C $C282,2 Jump if non-zero
C $C284,1 L--
C $C285,2 C = 15
N $C287 Loop
C $C287,1 Put it in #REGsp (so we can use PUSH for speed)
C $C297,1 A = H
C $C298,1 H--
C $C299,1 A &= C
C $C29A,3 Jump if non-zero
C $C29D,4 L -= 32
C $C2A1,2 Jump if carry
C $C2A3,4 H += 16
C $C2A9,1 E = D
C $C2AA,2 Loop
C $C2AD,1 Set flags
C $C2AE,3 Jump if negative
C $C2B1,4 B = ~A + $82
C $C2B5,3 DE = 0
C $C2B8,2 A = <self modified>
C $C2BA,1 Set flags
C $C2BB,2 Jump if zero
C $C2BD,1 A--
C $C2BE,2 Jump if non-zero
C $C2C0,1 DE--
C $C2C1,1 Put it in #REGsp (so we can use PUSH for speed)
C $C2D1,1 A = H
C $C2D2,1 H--
C $C2D3,1 A &= C
C $C2D4,3 Jump if non-zero
N $C2D7 Scanline increment pattern.
C $C2D7,4 L -= 32
C $C2DB,2 Jump if carry
C $C2DD,4 H -= 16
C $C2E1,2 Next scanline ?
C $C2E3,3 Restore original #REGsp (self modified)
C $C2E6,1 Return
c $C2E7 Routine at C2E7
D $C2E7 Used by the routine at #R$C452.
C $C2E7,2 A = IY.low
C $C2E9,2 Compare to 19
C $C2EB,3 Jump if >= 19
C $C2EE,1 Bank
C $C2EF,3 A = IX[0]
C $C2F2,1 L = A
C $C2F3,2 A &= 12
C $C2F5,3 Jump if zero
C $C2F8,2 IY--
C $C2FA,2 Bit 5 of L set?
C $C2FC,2 Jump if clear
C $C2FE,2 Bit 7 of L set?
C $C300,2 H = $EC
C $C302,2 Jump if set
C $C304,1 H = $EB
C $C305,2 Bit 4 of L set?
C $C307,3 Jump if clear
C $C30A,1 Bank
C $C30B,2 Compare to 2
C $C30D,3 Jump if >= 2
C $C310,1 Bank
C $C311,2 Compare to 4
C $C313,2 A = 32
C $C315,3 C = IY[1]
C $C318,2 Jump if != 4
C $C31A,1 A = 0
C $C31B,3 C = IY[2]
C $C31E,3 Self modify 'ADD A,x' @ #R$C345
C $C321,3 A = IY[0]
C $C324,1 A -= C
C $C325,6 Jump if A was <= C
C $C32B,1 C = A
C $C32C,1 A <<= 1
C $C32D,1 B = A
C $C32E,8 L = ~(($60 - IY[0]) << 1)
C $C336,1 -- why are we copying to SP with no push/pop nearby?
C $C337,1 D = *HL
C $C338,1 L--
C $C339,1 E = *HL
C $C33A,1 A -= B
C $C33B,1 L = A
C $C33C,1 H--
C $C33D,3 A = fast_counter
C $C340,3 A ROR 3
C $C343,2 A &= 28
C $C345,2 A += <self modified>
C $C347,3 Self modify 'LD HL,x' @ #R$C351 below
C $C34A,1 Swap
C $C34B,3 DE = A
C $C34E,2 HL -= DE
C $C350,1 Swap
C $C351,3 HL = <self modified>
C $C354,3 Jump
N $C357 Another case.
C $C357,1 Bank
C $C358,2 Compare to 4
C $C35A,3 Jump if non-zero
C $C35D,6 A = IY[0] - IY[2]
C $C363,6 Jump if IY[0] was <= IY[2]
C $C369,1 C = A
C $C36A,1 A <<= 1
C $C36B,1 B = A
C $C36C,8 L = ~(($60 - IY[0]) << 1)
C $C374,1 -- this again
C $C375,1 D = *HL
C $C376,1 L--
C $C377,1 E = *HL
C $C378,1 A -= B
C $C379,1 L = A
C $C37A,1 H--
C $C37B,3 Jump
C $C37E,1 Bank/unbank
C $C37F,2 Compare to 2
N $C381 This entry point is used by the routine at #R$E8CE.
C $C381,3 Jump if A >= 2
C $C384,1 Bank/unbank
C $C385,2 Compare to 4
C $C387,2 A = 32
C $C389,3 C = IY[1]
C $C38C,2 Jump if non-zero
C $C38E,1 X = 0
C $C38F,3 C = IY[2]
C $C392,3 Self modify 'ADD A,x' @ #R$C3BD below
C $C395,3 A = IY[0]
C $C398,1 A -= C
C $C399,6 Jump if A was >= C
C $C39F,2 C = A << 1
C $C3A1,1 B = A
C $C3A2,8 L = ~(($60 - IY[0]) << 1)
C $C3AA,1 -- this again
C $C3AB,1 H--
C $C3AC,1 D = *HL
C $C3AD,1 L--
C $C3AE,1 E = *HL
C $C3AF,1 A -= B
C $C3B0,1 L = A
C $C3B1,1 H++
C $C3B2,3 Self modify 'LD HL,x' @ #R$C3C4 below
C $C3B5,3 A = fast_counter
C $C3B8,3 A ROR 3
C $C3BB,2 A &= 28
C $C3BD,2 A += <self modified>
C $C3BF,3 HL = A
C $C3C2,1 HL += DE
C $C3C3,1 Swap
C $C3C4,3 HL = <self modified>
C $C3C7,3 Jump
C $C3CA,1 Swap
C $C3CB,2 Compare to 4
C $C3CD,2 Jump if non-zero
C $C3CF,6 A = IY[0] - IY[2]
C $C3D5,6 Jump if IY[0] was <= IY[2]
C $C3DB,1 C = A
C $C3DC,1 A <<= 1
C $C3DD,1 B = A
C $C3DE,8 L = ~(($60 - IY[0]) << 1)
C $C3E6,1 -- this again
C $C3E7,1 H--
C $C3E8,1 D = *HL
C $C3E9,1 L--
C $C3EA,1 E = *HL
C $C3EB,1 A -= B
C $C3EC,1 L = A
C $C3ED,1 H++
N $C3EE Continuation point.
C $C3EE,1 A = *HL
C $C3EF,1 L--
C $C3F0,1 L = *HL
C $C3F1,1 H = A
C $C3F2,1 Set flags
C $C3F3,2 HL -= DE
C $C3F5,1 A = L
C $C3F6,3 Jump if negative
C $C3F9,1 Set flags
C $C3FA,3 Jump if positive
C $C3FD,2 A = 127
C $C3FF,2 Jump
N $C401 Negative case.
C $C401,1 Set flags
C $C402,3 Jump if negative
C $C405,2 A = $81
N $C407 Positive case.
C $C407,1 SP++
C $C408,4 Bit 5 of IX[0] set?
C $C40C,2 Jump if clear
C $C40E,3 HL = $FF00
C $C411,1 HL += SP
C $C412,1 SP = HL
C $C413,1 L = A
C $C414,1 B = C
C $C415,1 A = C
C $C416,2 Bit 7 of L set?
C $C418,2 Jump if clear
C $C41A,1 A = L
C $C41B,2 A = -A
C $C41D,1 L = A
C $C41E,1 A = B
C $C41F,1 Compare to L
C $C420,2 A = 'DEC DE'
C $C422,2 Jump if A was < L
C $C424,2 Jump
C $C426,1 Compare to L
C $C427,2 A = 'INC DE'
C $C429,2 Jump if A was < L
C $C42B,3 Self modify instruction at #R$C435
C $C42E,1 A = B
C $C42F,1 A >>= 1
N $C430 Divider?
C $C430,1 A = L
C $C431,1 Compare to C
C $C432,2 Jump if A < C
C $C434,1 A -= C
C $C435,1 <self modified instruction> DE++ or DE--
C $C436,1 Stack DE
C $C437,2 Loop
C $C439,2 IY++
C $C43B,3 Pointer to routine which called function will store at instruction #R$C4B2
C $C43E,3 Jump
C $C441,3 Self modify instruction at #R$C445
C $C444,1 A = 0
C $C445,1 <self modified instruction> DE++ or DE--
C $C446,1 A += C
C $C447,2 Jump if carry
C $C449,1 Compare to L
C $C44A,2 Jump if A < L
C $C44C,1 A -= L
C $C44D,1 Stack DE
C $C44E,2 Loop
C $C450,2 Jump to exit
c $C452 Draws the road
D $C452 Used by the routines at #R$8401, #R$852A and #R$873C.
@ $C452 label=draw_road
C $C452,4 Self modify #REGsp restore instruction
C $C456,4 on_dirt_track = 0
C $C45A,3 Self modify 'LD A,x' @ #R$C160 to be zero (in draw_tunnel)
C $C45D,3 Self modify 'LD A,x' @ #R$C88F to be zero (tunnel related)
C $C460,5 Self modify 'LD A,x' @ #R$C6D8 to be 3
C $C465,4 #REGiy = $E301
C $C469,6 C = $60 - IY[0]
C $C46F,3 Load road_buffer_offset into #REGa
C $C472,2 Add 64 so it's the lanes data offset
C $C474,5 IX = $EE00 | A
C $C479,3 B = A
C $C47C,3 $C6B3 = A & 1
C $C47F,4 RR B twice
N $C483 I'm debating whether $D0 here is an opcode. I'm currently thinking not.
C $C483,3 H = $D0, L = 16
C $C486,1 Copy $D0 to #REGa
C $C487,2 B = $55 -- Zero this and the landscape goes blank but only half the time
C $C489,2 If B carried out (earlier) then jump
C $C48B,3 H = $00, L = 48
C $C48E,1 Copy $00 to #REGb
@ $C48F label=dr_c48f
C $C48F,3 Self modify 'XOR x' @ #R$C6D3 to be x = $D0
C $C492,4 Self modify 'ADD A,x' @ #R$C677 to be x = $D0 or x = $00
C $C496,4 Self modify 'ADD A,x' @ #R$C651 to be x = $10 or x = $30
C $C49A,4 Self modify 'ADD A,x' @ #R$C698 to be x = $11 or x = $31
C $C49E,6 Self modify 'JP Z,x' @ #R$C4B2 to be #R$C534
C $C4A4,2 L = $FF
C $C4A6,3 DE = $0100
C $C4A9,1 A = B
C $C4AA,3 Self modify 'LD A,x' @ #R$C6BC to be zero?
N $C4AD This entry point is used by the routine at #R$C598.
@ $C4AD label=dr_c4ad
C $C4AD,5 A = IX[0] & 3  -- lanes byte & 3
C $C4B2,3 Jump to <self modified> if zero
C $C4B5,1 Bank
C $C4B6,3 L = IX[0] -- sampled IX=$EEFD $EEFE $EEFF $EE00 .. etc.
C $C4B9,2 A += $E7
C $C4BB,3 Self modify 'LD H,x' @ #R$C642
C $C4BE,3 Self modify 'LD H,x' @ #R$C5B3
C $C4C1,1 H = A
C $C4C2,2 L <<= 1
C $C4C4,2 New bit 7 set?
C $C4C6,2 Jump if set  -- Forked road plotting path
C $C4C8,2 Old bit 7 was set?
C $C4CA,2 A &= 3
C $C4CC,2 C = $FD
C $C4CE,3 Jump
@ $C4D1 label=dr_c4d1
C $C4D1,2 A += 2
C $C4D3,2 C = $FE
@ $C4D5 label=dr_c4d5
C $C4D5,3 Self modify 'LD H,x' @ #R$C5D9
C $C4D8,3 Self modify 'LD H,x' @ #R$C68A
C $C4DB,1 A = C
C $C4DC,3 Self modify 'LD B,x' @ #R$C5AC
C $C4DF,3 Jump
@ $C4E2 label=dr_c4e2
C $C4E2,2 -- Forked road plotting path
C $C4E4,2 C = $FF
C $C4E6,2 H = 1
C $C4E8,2 Bit 4 set?
C $C4EA,2 Jump if set
C $C4EC,2 Bit 3 set?
C $C4EE,2 Jump if clear
@ $C4F0 label=dr_c4f0
C $C4F0,2 A = IY.low
C $C4F2,3 Self modify 'CP x' @ #R$C15D
C $C4F5,2 A = 1
C $C4F7,1 C++
C $C4F8,2 Bit 5 set?
C $C4FA,2 Jump if clear
C $C4FC,1 C--
C $C4FD,1 A++
C $C4FE,1 H--
@ $C4FF label=dr_c4ff
C $C4FF,3 Self modify 'LD A,x' @ #R$C160 (in draw_tunnel)
@ $C502 label=dr_c502
C $C502,1 A = H
C $C503,3 Self modify 'LD A,x' @ #R$C88F (tunnel related)
C $C506,2 A = $EB
C $C508,3 Self modify 'LD H,x' @ #R$C5D9
C $C50B,3 Self modify 'LD H,x' @ #R$C68A
C $C50E,2 A = $FF
C $C510,3 Self modify 'LD B,x' @ #R$C5AC
C $C513,1 A = C
C $C514,1 Bank/unbank
C $C515,1 B = A
C $C516,3 Jump
@ $C519 label=dr_c519
C $C519,2 Bit 6 set?
C $C51B,3 Jump to forked_road_plotter if set
N $C51E Non-forked path.
C $C51E,1 A = L
C $C51F,2 A &= 24
C $C521,2 A = 0
C $C523,2 Jump if non-zero
C $C525,1 A++
@ $C526 label=dr_c526
C $C526,3 on_dirt_track = A
C $C529,2 A = $FF
C $C52B,3 Self modify 'LD B,x' @ #R$C5AC
C $C531,3 Jump forward
@ $C534 label=dr_c534
C $C534,1 Bank/unbank
C $C535,2 A = $E8
C $C537,3 Self modify 'LD H,x' @ #R$C642
C $C53A,3 Self modify 'LD H,x' @ #R$C5B3
C $C53D,2 A += 4
C $C53F,3 Self modify 'LD H,x' @ #R$C68A
C $C542,3 Self modify 'LD H,x' @ #R$C5D9
C $C545,2 A = $FC
C $C547,3 Self modify 'LD B,x' @ #R$C5AC
N $C54D This entry point is used by the routine at #R$C2E7.
@ $C54D label=dr_c54d
C $C54D,3 Self modify 'JP Z,x' @ #R$C4B2
C $C550,1 Bank/unbank
@ $C551 label=dr_c551
C $C551,1 A = B
C $C552,1 Set flags
C $C553,3 Jump if non-zero
C $C556,1 Bank/unbank
C $C557,6 Self modify 'JP NZ,x' @ #R$C6AD to be #R$C55F
C $C55D,1 Bank/unbank
C $C55F,1 A = D
C $C560,1 D--
C $C561,2 A &= 15
C $C563,2 Jump if zero
N $C565 This entry point is used by the routine at #R$C58A.
@ $C565 label=dr_c565
C $C565,4 Self modify 'LD DE,x' @ #R$C56C (below)
C $C569,2 B = $FF
C $C56B,1 Bank/unbank
N $C56C Calculate address of next bitmap scanline ??
C $C56C,3 DE = <self modified>
C $C56F,4 L = E + 31
C $C573,1 H = D
C $C574,1 Put it in #REGsp (so we can use PUSH for speed)
C $C575,3 HL = 0  [not self modified apparently]
C $C578,1 C = L
C $C579,3 Jump into a sequence of 15 PUSH HLs
N $C57C smells like scanline/buffer pointer movement
@ $C57C label=dr_c57c
C $C57C,4 E -= 32
C $C580,3 Jump if it went -ve
C $C583,4 D += 16
C $C587,3 jump
N $C58A smells like scanline/buffer pointer movement
@ $C58A label=dr_c58a
C $C58A,4 E -= 32
C $C58E,3 Jump if it went -ve
C $C591,4 D += 16
C $C595,3 jump
N $C598 Road and backdrop plotting
@ $C598 label=dr_c598
C $C598,1 Bank
C $C599,6 Self modify 'JP NZ,x' @ #R$C6AD to be #R$C5A1
C $C59F,1 Unbank
C $C5A1,1 A = D
C $C5A2,1 D--
C $C5A3,2 A &= 15
C $C5A5,2 Jump if zero
N $C5A7 This entry point is used by the routine at #R$C57C.
@ $C5A7 label=dr_c5a7
C $C5A7,4 Self modify 'LD DE,x' @ #R$C5F9
C $C5AB,1 A = L
C $C5AC,2 B = <self modified>
C $C5AE,1 Bank
C $C5AF,1 L = A
C $C5B0,3 B = 16, C = $F8 (mask)
C $C5B3,2 Self modified
C $C5B5,1 A = *HL
C $C5B6,1 Set flags
C $C5B7,3 Jump if zero
C $C5BA,2 A = 0 (not self modified)
C $C5BC,3 Jump if negative
C $C5BF,2 A = 15
C $C5C1,3 Jump
N $C5C4 Pattern
@ $C5C4 label=dr_c5c4
C $C5C4,3 A = HL[-1]
C $C5C7,1 A &= C  -- C is the mask $F8 here
C $C5C8,3 A >>= 3
C $C5CB,1 A >>= 1
C $C5CC,2 Add carry
C $C5CE,4 Jump if A < B
C $C5D2,1 A--
@ $C5D3 label=dr_c5d3
C $C5D3,1 E = A
C $C5D4,2 A = ~A + B
C $C5D6,3 Self modify 'JR x' @ #R$C62C -- jump table target
C $C5D9,2 H = $EC
C $C5DB,1 A = *HL
C $C5DC,1 Set flags
C $C5DD,3 Jump if zero
C $C5E0,2 A = 15
C $C5E2,3 Jump if positive
C $C5E5,1 A = 0
C $C5E6,3 Jump
N $C5E9 Same pattern again
@ $C5E9 label=dr_c5e9
C $C5E9,1 L--
C $C5EA,2 A = *HL & C  -- C is the mask $F8 here
C $C5EC,3 A >>= 3
C $C5EF,1 A >>= 1
@ $C5F0 label=dr_c5f0
C $C5F0,3 Self modify 'JR x' @ #R$C60A -- jump table target
C $C5F3,3 A = ~A + B + E
C $C5F6,3 Self modify 'JR x' @ #R$C61B -- jump table target
C $C5F9,3 DE = <self modified>
C $C5FC,4 L = E + 31
C $C600,1 H = D
C $C601,1 Put it in #REGsp (so we can use PUSH for speed)
C $C602,1 Bank
C $C603,1 A <<= 1
C $C604,1 H = A
C $C605,1 L = A
C $C606,1 Unbank
C $C607,3 BC = 0
@ $C60A refs=:$C60A
C $C60A,2 Jump table (self modified)
@ $C61B refs=:$C61B
C $C61B,2 Jump table (self modified)
@ $C62C refs=:$C62C
C $C62C,2 Jump table (self modified)
N $C62E This entry point is used by the routine at #R$C452.
@ $C62E label=dr_c62e
C $C63D,1 B = E
C $C63E,1 C--
C $C63F,2 H = $E4  -- point at $E4xx - road edge/markings table
C $C641,1 Bank/unbank
C $C642,2 H = <self modified>  -- seems to receive $E8 $E9 $EA (left, centre left, centre tables)
C $C644,1 A = *HL
C $C645,3 Jump if non-zero
C $C648,3 A = HL[-1]
C $C64B,1 Bank/unbank
N $C64C Build address of road edge marking graphic.
N $C64C Bottom three bits select the row.
C $C64C,1 Save index
C $C64D,4 A = (A & 7) * 4
C $C651,2 A += <self modifed>  -- set to one of $10 $30 $50 $70 $90 $B0 - offset of the current edge graphic
C $C653,1 Set road/edge markings address
N $C654 Top five bits select screen buffer addr?
N $C654 If I break this it seems to affect the left hand side only.
C $C654,8 E = ((index >> 3) & 31) + B
N $C65C AND-OR masking here. DE is address of screen buffer byte HL is address of mask byte, followed by bitmap byte [then again since the edges are 16x8]
C $C65C,1 Read a screen buffer byte
C $C65D,1 Apply the mask
C $C65E,1 Advance to bitmap byte
C $C65F,1 Apply the bitmap
C $C660,1 Write the new screen buffer byte
C $C661,2 L += 2
C $C663,1 E++
C $C664,2 *DE++ = *HL++, BC--
C $C666,1 Bank/unbank
@ $C667 label=dr_c667
C $C667,1 B++  -- counter?
C $C668,2 Jump if zero  -- exit?
C $C66A,1 H++
C $C66B,1 A = *HL
C $C66C,1 Set flags
C $C66D,2 Jump if non-zero
C $C66F,3 A = HL[-1]
C $C672,1 Bank/unbank
C $C673,1 E = A  -- save A
C $C674,6 L = ((A & 7) << 1) + <self modified>
C $C67A,8 E = ((E >> 3) & 31) + B
C $C682,2 *DE++ = *HL++, BC--
N $C684 This reads a road edge byte from $E4xx.
C $C684,1 A = *HL
C $C685,1 *DE = A
C $C686,1 Bank/unbank
C $C687,3 Jump  -- looks like a loop
@ $C68A label=dr_c68a
C $C68A,2 H = <self modified>
C $C68C,1 A = *HL
C $C68D,1 L--
C $C68E,1 Set flags
C $C68F,2 Jump if non-zero
C $C691,1 A = *HL
C $C692,1 Bank/unbank
C $C693,1 E = A  -- save A
N $C694 Must be the right hand edge handling.
C $C694,7 L = ((A & 7) << 2) + <self modified>   -- as above but * 4
C $C69B,8 E = ((E >> 3) & 31) + B
C $C6A3,2 *DE++ = *HL++, BC--
N $C6A5 AND-OR masking here. This affects road lines when altered.
C $C6A5,2 A = *DE & *HL
C $C6A7,1 L++
C $C6A8,2 *DE = A | *HL
C $C6AA,1 Bank/unbank
@ $C6AB label=dr_c6ab
C $C6AB,1 L--
C $C6AC,1 C--
C $C6AD,3 Jump to <self modified> if non-zero
C $C6B0,1 Swap
C $C6B1,1 B = A
@ $C6B2 label=dr_c6b2
C $C6B2,2 A = <self modified>
C $C6B4,2 A ^= 1
C $C6B6,3 Self modify above
C $C6B9,3 Jump if non-zero
C $C6BC,4 A = <self modified> ^ 0x55
C $C6C0,3 Self modify above
C $C6C3,1 B = A
N $C6C4 This causes the road edge stripes.
C $C6C4,8 Toggle bit 5 of x in 'ADD A,x' @ #R$C651  -- switch between adjacent edge graphics
C $C6CC,1 A++
C $C6CD,3 Self modify 'ADD A,x' @ #R$C698
C $C6D0,8 Toggle <self modified> bits of 'ADD A,x' @ #R$C677
@ $C6D8 label=dr_c6d8
C $C6D8,2 A = <self modified>  -- self modified below
C $C6DA,1 A--
C $C6DB,3 Self modify 'LD A' @ #R$C6D8 to be new #REGa [above]
C $C6DE,3 Jump if non-zero
C $C6E1,3 Read 'XOR x' @ #R$C6D3
C $C6E4,2 A += 16
C $C6E6,2 Jump if carried
C $C6E8,3 Self modify 'XOR x' @ #R$C6D3 to be new #REGa
C $C6EB,1 C = A
C $C6EC,3 Read 'ADD A,x' @ #R$C677
C $C6EF,1 Set flags
C $C6F0,2 Jump if zero
C $C6F2,1 A = C
C $C6F3,3 Self modify 'ADD A,x' @ #R$C677
@ $C6F6 label=dr_c6f6
C $C6F6,8 Increment x in 'ADD A,x' @ #R$C651 by 64  -- next road edge graphic?
C $C6FE,5 Self modify 'LD A,x' @ #R$C6D8 to be 5
@ $C703 label=dr_c703
C $C703,3 A = IY[0]
C $C706,2 IY++
C $C708,2 IX.low++
C $C70A,3 A -= IY[0]
C $C70D,2 Jump if zero
C $C70F,3 Jump if positive
C $C712,3 Jump
@ $C715 label=dr_c715
C $C715,2 L -= 2
C $C717,3 C = IX[0]
C $C71A,2 Bit 6 of C set?
C $C71C,3 Jump if clear
C $C71F,2 Bit 7 of C set?
C $C721,3 Jump if set
C $C724,2 B = 255
C $C726,2 Bit 2 of C set?
C $C728,2 A = 1
C $C72A,2 Jump if clear
C $C72C,2 A = IY.low
C $C72E,3 Self modify 'CP x' @ #R$C15D  [15 when tunnel is small, 6 when fills screen]
C $C731,2 A = 1
C $C733,1 B++
C $C734,2 Bit 4 of C set?
C $C736,1 C = A
C $C737,2 Jump if clear
C $C739,1 B--
C $C73A,1 A++
C $C73B,1 C--
@ $C73C label=dr_c73c
C $C73C,3 Self modify 'LD A,x' @ #R$C160 (in draw_tunnel)
C $C73F,1 A = C
@ $C740 label=dr_c740
C $C740,3 Self modify 'LD A,x' @ #R$C88F (tunnel related)
C $C743,3 Jump
N $C746 Variation:
@ $C746 label=dr_c746
C $C746,3 C = IX[0]
C $C749,2 Bit 6 of C set?
C $C74B,3 Jump if clear
C $C74E,2 Bit 7 of C set?
C $C750,3 Jump if set
C $C753,1 Swap
C $C754,2 B = 255
C $C756,2 Bit 2 of C set?
C $C758,2 A = 1
C $C75A,2 Jump if clear
C $C75C,2 A = IY.low
C $C75E,3 Self modify 'CP x' @ #R$C15D  [15 when tunnel is small, 6 when fills screen]
C $C761,2 A = 1
C $C763,1 B++
C $C764,2 Bit 4 of C set?
C $C766,1 C = A
C $C767,2 Jump if clear
C $C769,1 B--
C $C76A,1 A++
C $C76B,1 C--
@ $C76C label=dr_c76c
C $C76C,3 Self modify 'LD A,x' @ #R$C160 (in draw_tunnel)
C $C76F,1 A = C
@ $C770 label=dr_c770
C $C770,3 Self modify 'LD A,x' @ #R$C88F (tunnel related)
C $C773,1 Swap
@ $C774 label=dr_c774
C $C774,1 C = A
C $C775,3 A = IY[0]
C $C778,2 IY++
C $C77A,2 IX.low++
C $C77C,3 A -= IY[0]
C $C77F,2 Compare to $E0
C $C781,3 Jump if A >= $E0
C $C784,2 Compare to $50
C $C786,2 Jump if A >= $50
@ $C788 label=dr_c788
C $C788,2 L -= 2
C $C78A,1 A += C
C $C78B,5 Jump if zero or negative
C $C790,1 C = A
C $C791,3 Jump
@ $C794 label=dr_c794
C $C794,1 C = A
C $C795,2 Compare to $50
C $C797,3 Jump if A < $50
N $C79A This entry point is used by the routines at #R$CBA4 and #R$CBC5.
@ $C79A label=dr_c79a
C $C79A,1 E++
C $C79B,3 Address of x in 'LD A,x' @ #R$C160 (in draw_tunnel)
C $C79E,3 Read 'LD A,x' @ #R$C88F (tunnel related)
C $C7A1,1 A |= *HL
C $C7A2,1 A >>= 1
C $C7A3,3 Jump if carry
C $C7A6,1 A = D
C $C7A7,2 A &= 15
C $C7A9,1 C = A
C $C7AA,1 A = E
C $C7AB,2 A >>= 1
C $C7AD,1 A += C
C $C7AE,1 A = ~A
C $C7AF,2 A += $80
C $C7B1,1 B = A
C $C7B2,3 HL = horizon_level
C $C7B5,2 C = 24
C $C7B7,1 A = H
C $C7B8,1 Set flags
C $C7B9,3 Jump if negative
C $C7BC,2 Jump if non-zero
C $C7BE,1 A = L
C $C7BF,1 A -= B
C $C7C0,2 Jump if A was >= B
C $C7C2,1 A += C
C $C7C3,3 Jump if no carry
C $C7C6,3 Jump if zero
C $C7C9,1 C = A
@ $C7CA label=dr_c7ca
C $C7CA,1 A = C
C $C7CB,3 Self modify xx in 'LD BC,$xxyy' @ #R$C80A
C $C7CE,1 A += B
C $C7CF,3 Jump if positive
C $C7D2,2 A -= 127
C $C7D4,2 A = -A
C $C7D6,1 A += C
C $C7D7,3 Self modify xx in 'LD BC,$xxyy' @ #R$C80A
C $C7DA,1 C = A
@ $C7DB label=dr_c7db
C $C7DB,4 A = 24 - C
C $C7DF,1 A <<= 1
C $C7E0,1 C = A
C $C7E1,2 A <<= 2
C $C7E3,1 A += C
C $C7E4,1 C = A
C $C7E5,2 B = 0
C $C7E7,2 A = <self modified>  -- horizon shift value (ranges 0..19)
C $C7E9,1 A >>= 1  -- deciding whether to use the shifted or non shifted backdrop
C $C7EA,3 Point at first hill backdrop (shifted version)
C $C7ED,2 Jump if no carry
C $C7EF,3 Point at second hill backdrop
@ $C7F2 label=dr_c7f2
C $C7F2,1 HL += BC
C $C7F3,1 Bank/unbank
C $C7F4,4 A = 18 - A * 2
C $C7F8,3 Self modify 'JR x' @ #R$C86C -- jump table target
C $C7FB,3 DE = A
C $C7FE,3 Address of backdrop_shift_instrs
C $C801,1 HL += DE
C $C802,3 BC = 18
C $C805,3 Address in instruction stream
C $C808,2 Copy
C $C80A,3 BC = $<self modified>0A
C $C80D,1 Bank/unbank
C $C80E,1 A = L
C $C80F,1 Swap
C $C810,3 Jump
N $C813 Scanline advance pattern.
@ $C813 label=dr_c813
C $C813,4 E -= 32
C $C817,3 Jump if E < 32
C $C81A,4 D -= 16
@ $C821 label=dr_c821
C $C821,1 Bank/unbank
C $C822,1 Swap
C $C823,1 E = A
@ $C824 label=dr_c824
C $C824,1 A = D
C $C825,1 D--
C $C826,2 A &= 15
C $C828,2 Jump if zero
N $C82A This entry point is used by the routine at #R$C813.
@ $C82A label=dr_c82a
C $C82A,1 A = E
C $C82B,1 Swap
C $C82C,1 L = A
C $C82D,18 18 instructions/lines filled in by earlier code
@ $C86C label=dr_c86c
C $C86C,2 Self modified - jump table
C $C881,1 A += C
C $C882,2 Loop?
C $C884,1 Bank/unbank
C $C885,1 Swap
C $C886,1 E = A
N $C887 This entry point is used by the routine at #R$C598.
@ $C887 label=dr_start_sky_fill
C $C887,1 Swap
C $C888,4 L += 30
C $C88C,3 Fill value for blank sky
N $C88F The following value is tunnel related: 1 if in tunnel, 0 if not.
N $C88F It alternates fast when there is a partial tunnel on screen.
C $C88F,2 A = <self modified>
C $C891,1 Set flags
C $C892,3 Jump if zero
N $C895 In tunnel. Draw black scanlines for the sky instead of coloured.
C $C895,1 $0000 -> $FFFF
@ $C896 label=dr_chose_colour
C $C896,2 Mask for later
N $C898 Decrement the screen address.
@ $C898 label=dr_sky_fill_loop
C $C898,1 Save H in A
C $C899,1 Decrement row address
C $C89A,1 Extract low four bits of row address
C $C89B,2 Jump to dr_sky_fill_scanline if non-zero (easy case)
N $C89D Otherwise it was zero so will need extra work.
C $C89D,4 Decrement high three bits of the row address
C $C8A1,2 No carry, so ...
N $C8A3 Exit.
C $C8A3,3 Restore original #REGsp (self modified)
C $C8A6,1 Return
@ $C8A7 label=dr_sky_fill_fix_address
C $C8A7,4 Didn't carry so fix H from earlier DEC H (1110xxxx -> 1111xxxx)
N $C8AB Writes #REGde to #REGhl 15 times filling the scanline. Draws the blank upper part of the sky. DE must always be zero? or what about tunnels?
@ $C8AB label=dr_sky_fill_scanline
C $C8AB,1 Put it in #REGsp (so we can use PUSH for speed)
C $C8AC,15 Write 30 bytes of sky pixels
C $C8BB,3 Loop
c $C8BE Builds a pre-shifted version of the backdrop horizon image
D $C8BE 80x24 pixels = 240 bytes
R $C8BE Used by the routine at #R$87DC.
@ $C8BE label=pre_shift_backdrop
C $C8BE,3 Point at source image
C $C8C1,3 Point at destination image (to be shifted)
C $C8C4,2 Preserve addresses
C $C8C6,5 Copy whole source image to destination
C $C8CB,1 Restore source address
C $C8CC,5 DE = Source address + 9
C $C8D1,1 Pop destination address
C $C8D2,3 Load counters for 10 bytes per row, 24 rows
@ $C8D5 label=psd_row_loop
C $C8D5,1 Preserve counters
C $C8D6,1 A = *DE
@ $C8D7 label=psd_loop
C $C8D7,2 Rotate nibbles from A to *HL
C $C8D9,1 Advance destination address
C $C8DA,1 Advance source address
C $C8DB,2 Loop while rowbytes remain
C $C8DD,1 Restore counters
C $C8DE,1 Decrement row counter
C $C8DF,3 Loop while rows remain
C $C8E2,1 Return
c $C8E3 Forked road plotting
D $C8E3 Used by the routine at #R$C452.
@ $C8E3 label=forked_road_plotter
C $C8E3,1 Bank
N $C8E4 Reset/Update a load of self modified locations.
C $C8E4,3 Read 'LD A,x' @ #R$C6D8
C $C8E7,3 Self modify 'LD A,x' @ #R$CB65
C $C8EA,3 Read 'LD A,x' @ #R$C6B2
C $C8ED,3 Self modify 'LD A,x' @ #R$CB36
C $C8F0,3 Read 'XOR x' @ #R$C6D3
C $C8F3,3 Self modify 'XOR x' @ #R$CB5D
C $C8F6,3 Read 'ADD A,x' @ #R$C677
C $C8F9,3 Self modify 'ADD A,x' @ #R$CA9D
C $C8FC,3 Self modify 'ADD A,x' @ #R$CAFF
C $C8FF,3 Read 'ADD A,x' @ #R$C651
C $C902,3 Self modify 'ADD A,x' @ #R$CA7A
C $C905,3 Self modify 'ADD A,x' @ #R$CADC
C $C908,1 A++
C $C909,3 Self modify 'ADD A,x' @ #R$CB1C
C $C90C,3 Self modify 'ADD A,x' @ #R$CABB
C $C90F,3 Read 'LD A,x' @ #R$C6BC
C $C912,3 Self modify 'LD A,x' @ #R$CB40
N $C915 This entry point is used by the routines at #R$CBA4 and #R$CBC5.
C $C915,1 A = B
C $C916,1 Set flags
C $C917,3 Jump if non-zero
C $C91A,1 Unbank
C $C91B,6 Self modify 'JP NZ' @ #R$CB31 to be #R$C923
C $C921,1 Bank
C $C923,4 A = (D - 1) & 15
C $C927,2 Jump if zero
N $C929 This entry point is used by the routine at #R$C94C.
C $C929,4 Self modify 'LD DE,x' @ #R$C92E (just below)
C $C92D,1 Unbank
N $C92E Calculate address of next bitmap scanline ??
C $C92E,3 DE = <self modified>
C $C931,4 L = E + 31
C $C935,1 H = D
C $C936,1 Put it in #REGsp (so we can use POP for speed)
C $C937,3 HL = 0  [not self modified apparently]
C $C93A,1 C = L
C $C93B,3 Jump into a sequence of 15 PUSH HLs
N $C93E Smells line scanline counting foo.
C $C93E,4 E -= 32
C $C942,3 Jump if E < 32
C $C945,4 D -= 16
N $C94C Smells line scanline counting foo.
C $C94C,4 E -= 32
C $C950,3 Jump if E < 32
C $C953,4 D -= 16
C $C95A,1 Unbank
C $C95B,6 Self modify 'JP NZ,x' @ #R$CB31 to be #R$C963
C $C963,1 A = D
C $C964,1 D--
C $C965,2 A &= $0F
C $C967,2 Jump if zero
C $C969,4 Self modify 'LD DE,x' @ #R$CA00 to load current DE
C $C96D,1 A = L
C $C96E,1 Bank/unbank
C $C96F,1 L = A
C $C970,3 B = 16, C = $F8 (mask)
C $C973,2 HL = $E8xx
C $C975,1 A = *HL
C $C976,1 Set flags
C $C977,3 Jump if zero
C $C97A,2 A = 0 (not self modified)
C $C97C,3 Jump if negative
C $C97F,2 A = 15
C $C981,3 Jump
N $C984 Variation 1?
C $C984,3 A = HL[-1]
C $C987,1 A &= C  -- C is the mask $F8 here
C $C988,3 A >>= 3
C $C98B,1 A >>= 1
C $C98C,2 Add carry
C $C98E,4 Jump if A < B
C $C992,1 A--
C $C993,1 E = A
C $C994,1 A = ~A
C $C995,1 A += B
C $C996,3 Self modify 'JR x' @ #R$CA55 -- jump table target
C $C999,2 H++
C $C99B,1 A = *HL
C $C99C,1 Set flags
C $C99D,3 Jump if zero
C $C9A0,2 A = 0 (not self modified)
C $C9A2,3 Jump if negative
C $C9A5,2 A = 15
C $C9A7,3 Jump
N $C9AA Variation 2?
C $C9AA,3 A = HL[-1]
C $C9AD,1 A &= C  -- C is the mask $F8 here
C $C9AE,3 A >>= 3
C $C9B1,1 A >>= 1
C $C9B2,1 D = A
C $C9B3,3 A = ~(A - E) + B
C $C9B6,3 Self modify jump table target
C $C9B9,1 H++
C $C9BA,1 A = *HL
C $C9BB,1 Set flags
C $C9BC,3 Jump if zero
C $C9BF,2 A = 0 (not self modified)
C $C9C1,3 Jump if negative
C $C9C4,2 A = 15
C $C9C6,3 Jump
N $C9C9 Variation 3?
C $C9C9,3 A = HL[-1]
C $C9CC,1 A &= C  -- C is the mask $F8 here
C $C9CD,3 A >>= 3
C $C9D0,1 A >>= 1
C $C9D1,2 Add carry
C $C9D3,4 Jump if A < B
C $C9D7,1 A--
C $C9D8,1 E = A
C $C9D9,1 A -= D
C $C9DA,1 A = ~A
C $C9DB,1 A += B
C $C9DC,3 Self modify 'JR x' @ #R$CA33 -- jump table target
C $C9DF,2 H += 2
C $C9E1,1 A = *HL
C $C9E2,1 Set flags
C $C9E3,3 Jump if zero
C $C9E6,2 A = 0 (not self modified)
C $C9E8,3 Jump if negative
C $C9EB,2 A = 15
C $C9ED,3 Jump
N $C9F0 Different chunk
C $C9F0,2 A = HL[-1]
C $C9F2,1 A &= C  -- C is the mask $F8 here
C $C9F3,3 A >>= 3
C $C9F6,1 A >>= 1
C $C9F7,3 Self modify 'JR x' @ #R$CA11 -- jump table target
C $C9FA,1 A -= E
C $C9FB,2 A = ~A + B
C $C9FD,3 Self modify 'JR x' @ #R$CA22 -- jump table target
C $CA00,3 Self modified
C $CA03,3 A = E + 31
C $CA06,1 L = A
C $CA07,1 H = D
C $CA08,1 Put it in #REGsp (so we can use POP for speed)
C $CA09,1 Bank A
C $CA0A,1 A <<= 1  -- Rotate the road shading pattern (0x55 <-> 0xAA)
C $CA0B,1 H = A   -- Prepare pattern for storing
C $CA0C,1 L = A
C $CA0D,1 Unbank A
N $CA0E This BC filling the blank parts of the road when drawing the forked road.
C $CA0E,3 BC = 0 [doesn't seem self modified]
C $CA11,17 Jump table (self modified) Write 16 pixels
C $CA22,17 Jump table (self modified)
C $CA33,17 Jump table (self modified)
C $CA44,17 Jump table (self modified)
C $CA55,2 Jump table (self modified)
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
C $CAF2,1 H++
C $CAF3,1 A = *HL
C $CAF4,1 Set flags
C $CAF5,2 Jump if non-zero
C $CAF7,3 A = HL[-1]
C $CAFA,1 Bank/unbank
C $CAFB,1 E = A
C $CAFC,2 A &= 7
C $CAFE,1 A = A ROL 1
C $CAFF,2 A += <self modified>
C $CB01,1 L = A
C $CB02,1 A = E
C $CB03,3 A >>= 3
C $CB06,2 A &= $1F
C $CB08,1 A += B
C $CB09,1 E = A
C $CB0C,1 A = *HL
C $CB0D,1 *DE = A
C $CB0E,1 Bank/unbank
C $CB0F,3 A = HL[-1]
C $CB12,1 Set flags
C $CB13,2 Jump if non-zero
C $CB15,1 A = *HL
C $CB16,1 Bank/unbank
C $CB17,1 E = A
C $CB18,2 A &= 7
C $CB1A,2 A <<= 2
C $CB1C,2 A += 17
C $CB1E,1 L = A
C $CB1F,1 A = E
C $CB20,3 A = A (rotate right through carry) 3
C $CB23,3 A = (A & 31) + B
C $CB26,1 E = A
C $CB27,2 *DE++ = *HL++, BC--
C $CB29,1 A = *DE
C $CB2A,1 A &= *HL    [AND-OR masking?]
C $CB2B,1 L++
C $CB2C,1 A |= *HL
C $CB2D,1 *DE = A
C $CB2E,1 Bank/unbank
C $CB2F,1 L--
C $CB30,1 C--
C $CB31,3 Jump if non-zero to <self modified>
C $CB35,1 B = A
C $CB36,2 A = <self modified>
C $CB38,2 Toggle bit 0
C $CB3A,3 Self modify 'LD A,x' @ #R$CB36 (just above)
C $CB3D,3 Jump if non-zero
C $CB40,2 A = <self modified>
C $CB42,2 Toggle every other bit
C $CB44,3 Self modify 'LD A,x' @ #R$CB40 (just above)
C $CB47,1 B = A
C $CB48,8 Self modify 'LD A,x' @ #R$CA7A
C $CB50,3 Self modify 'ADD A,x' @ #R$CADC
C $CB53,1 A++
C $CB54,3 Self modify 'ADD A,x' @ #R$CB1C
C $CB57,3 Self modify 'ADD A,x' @ #R$CABB
C $CB5A,3 Read 'LD A,x' @ #R$CA9D
C $CB5D,2 [nested self modification - INCEPTION ACHIEVED]
C $CB5F,3 Self modify 'LD A,x' @ #R$CA9D
C $CB62,3 Self modify 'ADD A,x' @ #R$CAFF
C $CB65,3 A = <self modified> - 1
C $CB68,3 Self modify 'LD A,x' @ #R$CB65
C $CB6B,3 Jump if non-zero
C $CB6E,10 Self modify 'XOR x' @ #R$CB5D
C $CB78,1 C = A
C $CB79,3 Read 'LD A,x' @ #R$CA9D
C $CB7C,1 Set flags
C $CB7D,2 Jump if zero
C $CB7F,1 A = C
C $CB80,3 Self modify 'LD A,x' @ #R$CA9D
C $CB83,8 Self modify 'LD A,x' @ #R$CA7A
C $CB8B,5 Self modify 'LD A,x' @ #R$CB65 to load 5
C $CB90,3 A = IY[0]
C $CB93,2 IY++
C $CB95,2 IX.low++
C $CB97,3 A -= IY[0]
C $CB9A,2 Jump if zero
C $CB9C,3 Jump if positive
C $CB9F,2 L -= 2
C $CBA1,3 Loop
c $CBA4 Routine at CBA4
D $CBA4 I can't find what calls this routine. Checked various stages...
C $CBA4,1 C = A
C $CBA5,3 A = IY[0]
C $CBA8,2 IY++
C $CBAA,2 IX.low++
C $CBAC,3 A -= IY[0]
C $CBAF,5 Jump if >= 240
C $CBB4,5 Jump if >= 80
C $CBB9,2 L -= 2
C $CBBB,1 A += C
C $CBBC,5 Jump if zero or negative
C $CBC1,1 C = A
C $CBC2,3 Exit via #R$C915
c $CBC5 Routine at CBC5
D $CBC5 Used by the routine at #R$C95A.
N $CBC5 This gets hit during road forks.
C $CBC5,1 C = A
C $CBC6,5 Jump if A < 80
C $CBCB,3 Exit via #R$C79A
c $CBCE Road drawing - curvature stuff?
D $CBCE Used by the routine at #R$B9F4.
N $CBCE This gets hit during road forks.
@ $CBCE label=sub_cbce_fork
C $CBCE,3 Load two table high-bytes: $EE, $EC
C $CBD1,3 $ED $44 => NEG instruction for #R$CC21 & #R$CC22
N $CBD6 This entry point is used by the routine at #R$B9F4.
N $CBD6 This is the normal entry point?
@ $CBD6 label=sub_cbce_non_fork
C $CBD6,3 Load two table high-bytes: $ED, $E9
C $CBD9,3 Pair of NOP instructions for #R$CC21 & #R$CC22
N $CBDC This entry point is used by the routine at #R$CBCE.
C $CBDC,4 Write instructions in #REGde to #R$CC21 & #R$CC22
C $CBE0,4 Self modify 'LD HL' @ #R$CC70 to load ($<H>00)
C $CBE4,4 Self modify 'LD HL' @ #R$CCA5 to load ($<L>00)
C $CBE8,3 Load road_buffer_offset into #REGhl
C $CBEB,1 Read a curvature data byte
N $CBEC There's similar code at #R$CD47.
C $CBEC,5 A = fast_counter & $E0  -- top three bits
N $CBF2 Reduces A by 31.25% ... unsure why that figure.
C $CBF2,5 Divide A by 4 and subtract
C $CBF7,5 Divide A by 16 and subtract
C $CBFC,10 #REGiy = data_e6b0[#REGa]
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
C $CC2C,3 Reads from the #R$E540 table (distance shift table)
N $CC2F Subtract DE from table entry, result in BC.
C $CC2F,1 A -= E
C $CC30,1 C = A
C $CC31,3 high byte
C $CC34,1 A -= D
C $CC35,1 B = A
N $CC36 Sampled IY = E71E E71F E720 ..
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
N $CC8E Converts the 20 bytes at $E320 to totals.
C $CC8E,3 [points to presumed lanes data in pristine memory map but must be something else]
C $CC91,2 B = 20
C $CC93,1 *DE += *HL
N $CC94 This reads from $E760+ in sequence.
C $CC95,1 }
C $CC96,1 HL++
C $CC97,1 E++
C $CC98,2 Loop while #REGb > 0
C $CC9A,7 HL = road position - 295
C $CCA1,1 swap
C $CCA2,2 B = 0
C $CCA4,1 Unbank
C $CCA5,3 table?
C $CCA8,4 table?
C $CCAC,2 B = 21
C $CCAE,4 Save #REGsp to restore on exit (self modify)
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
C $CCD7,2 Opcode for DEC DE
C $CCD9,2 jump if A < L
C $CCDE,1 A < L ?
C $CCDF,2 Opcode for INC DE
C $CCE1,2 jump if A < L
C $CCE3,3 Self modify instruction below
C $CCE6,1 A = B
C $CCE7,1 A >>= 1
C $CCE8,1 A += L
C $CCE9,1 A < C ?
C $CCEA,2 jump if A < C
C $CCEC,1 A -= C
C $CCED,1 Self modified: INC DE or DEC DE
C $CCEF,2 Loop while #REGb > 0
C $CCF1,1 Unbank
C $CCF2,2 Outer loop?
C $CCF4,3 Restore original #REGsp (must be self modified)
C $CCF7,1 Return
C $CCF8,3 Self modify instruction below
C $CCFB,1 A = 0
C $CCFC,1 Self modified: INC DE or DEC DE
C $CCFD,1 A += C
C $CCFE,2 jump if overflow
C $CD00,1 A < L ?
C $CD01,2 jump if A < L
C $CD03,1 A -= L
C $CD05,2 Loop while #REGb > 0
C $CD07,1 Bank
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
C $CD31,1 push result?
C $CD32,1 Bank
C $CD33,1 B--
C $CD34,3 Outer loop?
C $CD37,3 Exit
c $CD3A Seems to build the height table at $E300 and $E336.
D $CD3A Used by the routines at #R$8401, #R$852A and #R$873C.
N $CD3A Point #REGiy at the road buffer's height data. (The "unpacked map height data"?)
@ $CD3A label=build_height_table
C $CD3A,3 Set the high byte of #REGiy to road buffer memory region ($EExx)
C $CD3D,3 Load road_buffer_offset into #REGa
C $CD40,2 Add 32 so it's the height data offset (wrapping around)
C $CD42,2 #REGiy is now the road buffer height data pointer
C $CD44,3 Read the current height byte into #REGc
N $CD47 The height byte is the byte B from the map data but converted like so ((B & 15) - 8). Heights are therefore 0 for level road, 7 for max downslope, -8 for max upslope.
N $CD47 build A - an index into the road drawing tables.
C $CD47,5 A = fast_counter & $E0  -- top three bits
C $CD4C,1 Copy for reducing
C $CD4D,1 Copy to be a multiplier later
N $CD4E Reduces A by 31.25%. This maps it to the start of rows in data_e600 (multiples of 22 long). So fast_counter indexes the rows of the table.
C $CD4E,5 Divide A by 4 and subtract
C $CD53,5 Divide A by 16 and subtract
C $CD58,4 Point #REGhl into road drawing tables at ($E600 | (A + 1))
C $CD5C,1 Multiplier = (fast_counter & $E0) from above
N $CD5D Multiplicand is the height byte from the map (7 max downslope, 0 level, -8 max upslope).
C $CD5D,3 Call multiply (A = multiplier, C = multiplicand) result in A
C $CD60,3 Negate result and copy to C
C $CD63,1 Bank
N $CD64 This builds the look-up table at $E301. Assuming it's a height table.
C $CD64,2 B = 21
C $CD66,3 DE = $E301
@ $CD69 label=ml10_loop
C $CD69,1 Unbank
C $CD6A,3 E = *HL * 2  -- HL points at $E6xx (road drawing tables)
C $CD6D,1 Preserve HL
C $CD6E,2 DE is now the value from table, widened
C $CD70,2 Initialise result to zero
C $CD72,1 A = negated multiply result from above
C $CD73,3 A += *IY  -- a height byte
C $CD76,1 C = A
C $CD77,2 Jump to ml10_continue if zero (since multiply by zero is a no-op)
C $CD79,3 Jump if positive
N $CD7C Otherwise handle negative case.
C $CD7C,5 E = -E; D = $FF  -- negate multiplier
C $CD81,3 A = -C  -- negate muliplicand
N $CD84 Multiplier.
@ $CD84 label=ml10_multiplier
C $CD84,1 Throw sign bit away?
@ $CD85 label=ml10_bit6
C $CD85,1 Shift out a high bit of multiplicand
C $CD86,4 Copy multiplier (from #REGde) if a bit shifted out
C $CD8A,1 Shift result up to prepare for next bit
@ $CD8B label=ml10_bit5
C $CD8B,1 Shift out a high bit of multiplicand
C $CD8C,3 Add multiplier (from #REGde) if a bit shifted out
@ $CD8F label=ml10_bit4
C $CD8F,1 Shift result up to prepare for next bit
@ $CD94 label=ml10_bit3
C $CD90,5 Repeat
@ $CD99 label=ml10_bit2
C $CD95,5 Repeat
@ $CD9E label=ml10_bit1
C $CD9A,5 Repeat
@ $CDA3 label=ml10_bit0
C $CD9F,5 Repeat
@ $CDA8 label=ml10_cda8
C $CDA4,5 Repeat
C $CDA9,1 A = H  -- top byte of result
@ $CDAA label=ml10_continue
C $CDAA,1 Restore HL
C $CDAB,1 A += *HL  -- HL points at $E6xx (road drawing data)
C $CDAC,1 HL++  (wrapping around)
C $CDAD,1 Bank
C $CDAE,1 Write #REGa to the table at $E3xx
C $CDAF,1 DE++  (wrapping around)
C $CDB0,2 IYl++ (wrapping around)
C $CDB2,2 Loop to ml10_loop while #REGb
C $CDB4,3 Final byte is always $A0
N $CDB7 Copy the table to $E336 while setting -ve values to 96.
C $CDB7,3 destination
C $CDBA,3 source
C $CDBD,3 B = 21 iterations, C = 96 limit
@ $CDC0 label=ml10_loop2
C $CDC0,1 Read from table just built
C $CDC1,4 Jump if A is positive
C $CDC5,1 Otherwise it's negative, so C = 96
C $CDC6,1 Write it
C $CDC7,1 HL++  (wrapping around)
C $CDC8,1 DE++  (wrapping around)
C $CDC9,2 Loop to ml10_loop2 while #REGb
N $CDCB Final bytes.
C $CDCB,6 C = A = (C + 3) & $F8
C $CDD1,1 A -= *HL
C $CDD2,1 *HL = C
C $CDD3,1 HL++  (wrapping around)
C $CDD4,1 *HL = A
C $CDD5,1 Return
c $CDD6 Multiplier
D $CDD6 Multiplies #REGc by the top three bits of #REGa then divides by 8 with rounding, on exit.
D $CDD6 Used by the routines at #R$CBD6 and #R$CD3A.
R $CDD6 I:A Multiplier (number to multiply by)  e.g. $A0, $E7, $20, $C0, $E6
R $CDD6 I:C Multiplicand (value to multiply)    e.g. $05, $02, $05, $03, $FE
R $CDD6 O:A Result e.g. $03, $02, $01, $02, $FE
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
b $CDEC Data block at CDEC these are words, but not ptrs
@ $CDEC label=table_cdec
@ $CDF4 label=table_cdf4
B $CDEC,20,8*2,4
@ $CE00 label=table_ce00
W $CE00,12,12
@ $CE0C label=data_ce0c
@ $CE19 label=data_ce19
@ $CE26 label=data_ce26
B $CE0C,39,13
b $CE33 Used by #R$B4FD - groups of six bytes
@ $CE33 label=table_ce33
W $CE33,24,6
@ $CE4B label=table_ce4b
@ $CE5E label=table_ce5e
@ $CE71 label=table_ce71
@ $CE84 label=table_ce84
@ $CE97 label=table_ce97
B $CE4B,95,8*2,3,8*2,3,8*2,3,8*2,3,8*2,3
b $CEAA Used by #R$B578 4 frames, all 8x6 masked
@ $CEAA label=bitmap_debris_1
@ $CEB6 label=bitmap_debris_2
@ $CEC2 label=bitmap_debris_3
@ $CECE label=bitmap_debris_4
B $CEAA,48,2
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
@ $CF8E label=hero_car_shadow
B $CF8E,1,1
B $CF8F,1,1 rows
W $CF90,2,2 -> shadow graphic
B $CF92,1,1
B $CF93,1,1 rows
W $CF94,2,2 -> shadow graphic
B $CF96,1,1
B $CF97,1,1 rows
W $CF98,2,2 -> shadow graphic
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
B $CFB2,117,3 TBD groups of 3 bytes ref'd by #R$B6A6. 3rd byte is index into car_adornments.
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
@ $D03F label=bitmap_arrow
B $D03F,126,6 Masked bitmap data
N $D0BD "HERE!" graphic (24x7)
N $D0BD #HTML[#CALL:graphic($D0BD,24,7,1,1)]
@ $D0BD label=bitmap_here
B $D0BD,42,6 Masked bitmap data
N $D0E7 Cherry light (sits on roof of car) (8x7)
N $D0E7 #HTML[#CALL:graphic($D0E7,8,7,1,1)]
@ $D0E7 label=bitmap_cherry_light
B $D0E7,14,2 Masked bitmap data
N $D0F5 Illuminated cherry light (24x14)
N $D0F5 #HTML[#CALL:graphic($D0F5,24,14,1,1)]
@ $D0F5 label=bitmap_cherry_light_lit
B $D0F5,84,6 Masked bitmap data
N $D149 Crash/spark (24x20)
N $D149 #HTML[#CALL:graphic($D149,24,20,1,1)]
@ $D149 label=bitmap_spark
B $D149,120,6 Masked bitmap data
N $D1C1 Graphic (16x4)
N $D1C1 #HTML[#CALL:graphic($D1C1,16,4,1,1)] LOOKS WRONG
@ $D1C1 label=bitmap_d1c1
B $D1C1,16,4 Masked bitmap data
N $D1D1 Debris (16x9)
N $D1D1 #HTML[#CALL:graphic($D1D1,16,9,1,1)] LOOKS WRONG
@ $D1D1 label=bitmap_debris
B $D1D1,36,4 Masked bitmap data
N $D1F5 More debris (8x12)
N $D1F5 #HTML[#CALL:graphic($D1F5,8,12,1,1)] LOOKS WRONG
@ $D1F5 label=bitmap_more_debris
B $D1F5,24,2 Masked bitmap data
N $D20D Turbo smoke plume animation
N $D20D (32x16) masked per frame. 4 frames.
N $D20D #HTML[#CALL:anim($D20D,32,16,1,1,4)]
N $D20D Turbo smoke plume data frame 1
N $D20D #HTML[#CALL:graphic($D20D,32,16,1,1)]
@ $D20D label=bitmap_turbo_1
B $D20D,128,8 Masked bitmap data
N $D28D Turbo smoke plume data frame 2
N $D28D #HTML[#CALL:graphic($D28D,32,16,1,1)]
@ $D28D label=bitmap_turbo_2
B $D28D,128,8 Masked bitmap data
N $D30D Turbo smoke plume data frame 3
N $D30D #HTML[#CALL:graphic($D30D,32,16,1,1)]
@ $D30D label=bitmap_turbo_3
B $D30D,128,8 Masked bitmap data
N $D38D Turbo smoke plume data frame 4
N $D38D #HTML[#CALL:graphic($D38D,32,16,1,1)]
@ $D38D label=bitmap_turbo_4
B $D38D,128,8 Masked bitmap data
N $D40D Hero car graphics
N $D40D 9 sets
N $D40D Centre parts
N $D40D Straight (40x14)
N $D40D #HTML[#CALL:graphic($D40D,40,14,0,1)]
@ $D40D label=bitmap_hero_centre_straight
B $D40D,70,5 Non-masked, inverted bitmap data
N $D453 Straight + Turn right (40x17)
N $D453 #HTML[#CALL:graphic($D453,40,17,0,1)]
@ $D453 label=bitmap_hero_centre_straight_right
B $D453,85,5 Non-masked, inverted bitmap data
N $D4A8 Straight + Turn right hard (40x16)
N $D4A8 #HTML[#CALL:graphic($D4A8,40,16,0,1)]
@ $D4A8 label=bitmap_hero_centre_straight_right_hard
B $D4A8,80,5 Non-masked, inverted bitmap data
N $D4F8 Up (40x14)
N $D4F8 #HTML[#CALL:graphic($D4F8,40,14,0,1)]
@ $D4F8 label=bitmap_hero_centre_up
B $D4F8,70,5 Non-masked, inverted bitmap data
N $D53E Up + Turn right (40x17)
N $D53E #HTML[#CALL:graphic($D53E,40,17,0,1)]
@ $D53E label=bitmap_hero_centre_up_right
B $D53E,85,5 Non-masked, inverted bitmap data
N $D593 Up + Turn right hard (40x15)
N $D593 #HTML[#CALL:graphic($D593,40,15,0,1)]
@ $D593 label=bitmap_hero_centre_up_right_hard
B $D593,75,5 Non-masked, inverted bitmap data
N $D5DE Down (40x14)
N $D5DE #HTML[#CALL:graphic($D5DE,40,14,0,1)]
@ $D5DE label=bitmap_hero_centre_down
B $D5DE,70,5 Non-masked, inverted bitmap data
N $D624 Down + Turn right (40x16)
N $D624 #HTML[#CALL:graphic($D624,40,16,0,1)]
@ $D624 label=bitmap_hero_centre_down_right
B $D624,80,5 Non-masked, inverted bitmap data
N $D674 Down + Turn right hard (40x16)
N $D674 #HTML[#CALL:graphic($D674,40,16,0,1)]
@ $D674 label=bitmap_hero_centre_down_right_hard
B $D674,80,5 Non-masked, inverted bitmap data
N $D6C4 Straight parts
N $D6C4 Top of car (40x9)
N $D6C4 #HTML[#CALL:graphic($D6C4,40,9,1,1)]
@ $D6C4 label=bitmap_hero_top_straight
B $D6C4,90,10 Masked, inverted bitmap data
N $D71E Bottom of car (40x6)
N $D71E #HTML[#CALL:graphic($D71E,40,6,1,1)]
@ $D71E label=bitmap_hero_bottom_straight
B $D71E,60,10 Masked, inverted bitmap data
N $D75A Left of car (8x14)
N $D75A #HTML[#CALL:graphic($D75A,8,14,1,1)]
@ $D75A label=bitmap_hero_left_straight
B $D75A,28,2 Masked, inverted bitmap data
N $D776 Right of car (8x14)
N $D776 #HTML[#CALL:graphic($D776,8,14,1,1)]
@ $D776 label=bitmap_hero_right_straight
B $D776,28,2 Masked, inverted bitmap data
N $D792 Straight + Turn right parts
N $D792 Top of car (40x9)
N $D792 #HTML[#CALL:graphic($D792,40,8,1,1)]
@ $D792 label=bitmap_hero_top_straight_right
B $D792,80,10 Masked, inverted bitmap data
N $D7E2 Bottom of car (40x6)
N $D7E2 #HTML[#CALL:graphic($D7E2,40,4,1,1)]
@ $D7E2 label=bitmap_hero_bottom_straight_right
B $D7E2,40,10 Masked, inverted bitmap data
N $D80A Left of car (8x13)
N $D80A #HTML[#CALL:graphic($D80A,8,13,1,1)]
@ $D80A label=bitmap_hero_left_straight_left
B $D80A,26,2 Masked, inverted bitmap data
N $D824 Right of car (8x13)
N $D824 #HTML[#CALL:graphic($D824,8,13,1,1)]
@ $D824 label=bitmap_hero_right_straight_right
B $D824,26,2 Masked, inverted bitmap data
N $D83E Straight + Turn right hard parts
N $D83E Top of car (40x9)
N $D83E #HTML[#CALL:graphic($D83E,40,9,1,1)]
@ $D83E label=bitmap_hero_top_straight_right_hard
B $D83E,90,10 Masked, inverted bitmap data
N $D898 Bottom of car (40x4)
N $D898 #HTML[#CALL:graphic($D898,40,4,1,1)]
@ $D898 label=bitmap_hero_bottom_straight_right_hard
B $D898,40,10 Masked, inverted bitmap data
N $D8C0 Left of car (8x12)
N $D8C0 #HTML[#CALL:graphic($D8C0,8,12,1,1)]
@ $D8C0 label=bitmap_hero_left_straight_right_hard
B $D8C0,24,2 Masked, inverted bitmap data
N $D8D8 Right of car (8x15)
N $D8D8 #HTML[#CALL:graphic($D8D8,8,15,1,1)]
@ $D8D8 label=bitmap_hero_right_straight_right_hard
B $D8D8,30,2 Masked, inverted bitmap data
N $D8F6 Up parts
N $D8F6 Top of car (40x10)
N $D8F6 #HTML[#CALL:graphic($D8F6,40,10,1,1)]
@ $D8F6 label=bitmap_hero_top_up
B $D8F6,100,10 Masked, inverted bitmap data
N $D95A Bottom of car (40x6)
N $D95A #HTML[#CALL:graphic($D95A,40,6,1,1)]
@ $D95A label=bitmap_hero_bottom_up
B $D95A,60,10 Masked, inverted bitmap data
N $D996 Left of car (8x13)
N $D996 #HTML[#CALL:graphic($D996,8,13,1,1)]
@ $D996 label=bitmap_hero_left_up
B $D996,26,2 Masked, inverted bitmap data
N $D9B0 Right of car (8x14)
N $D9B0 #HTML[#CALL:graphic($D9B0,8,14,1,1)]
@ $D9B0 label=bitmap_hero_right_up
B $D9B0,28,2 Masked, inverted bitmap data
N $D9CC Up + Turn right parts
N $D9CC Top of car (40x9)
N $D9CC #HTML[#CALL:graphic($D9CC,40,9,1,1)]
@ $D9CC label=bitmap_hero_top_right
B $D9CC,90,10 Masked, inverted bitmap data
N $DA26 Bottom of car (40x4)
N $DA26 #HTML[#CALL:graphic($DA26,40,4,1,1)]
@ $DA26 label=bitmap_hero_bottom_right
B $DA26,40,10 Masked, inverted bitmap data
N $DA4E Left of car (8x12)
N $DA4E #HTML[#CALL:graphic($DA4E,8,12,1,1)]
@ $DA4E label=bitmap_hero_left_right
B $DA4E,24,2 Masked, inverted bitmap data
N $DA66 Right of car (8x14)
N $DA66 #HTML[#CALL:graphic($DA66,8,14,1,1)]
@ $DA66 label=bitmap_hero_right_right
B $DA66,28,2 Masked, inverted bitmap data
N $DA82 Up + Turn right hard parts
N $DA82 Top of car (40x9)
N $DA82 #HTML[#CALL:graphic($DA82,40,9,1,1)]
@ $DA82 label=bitmap_hero_up_right_hard
B $DA82,90,10 Masked, inverted bitmap data
N $DADC Bottom of car (40x6)
N $DADC #HTML[#CALL:graphic($DADC,40,6,1,1)]
@ $DADC label=bitmap_hero_bottom_right_hard
B $DADC,60,10 Masked, inverted bitmap data
N $DB18 Left of car (8x11)
N $DB18 #HTML[#CALL:graphic($DB18,8,11,1,1)]
@ $DB18 label=bitmap_hero_left_right_hard
B $DB18,22,2 Masked, inverted bitmap data
N $DB2E Right of car (8x15)
N $DB2E #HTML[#CALL:graphic($DB2E,8,15,1,1)]
@ $DB2E label=bitmap_hero_right_right_hard
B $DB2E,30,2 Masked, inverted bitmap data
N $DB4C Down parts
N $DB4C Top of car (40x8)
N $DB4C #HTML[#CALL:graphic($DB4C,40,8,1,1)]
@ $DB4C label=bitmap_hero_top_down
B $DB4C,80,10 Masked, inverted bitmap data
N $DB9C Bottom of car (40x6)
N $DB9C #HTML[#CALL:graphic($DB9C,40,6,1,1)]
@ $DB9C label=bitmap_hero_bottom_down
B $DB9C,60,10 Masked, inverted bitmap data
N $DBD8 Left of car (8x13)
N $DBD8 #HTML[#CALL:graphic($DBD8,8,13,1,1)]
@ $DBD8 label=bitmap_hero_left_down
B $DBD8,26,2 Masked, inverted bitmap data
N $DBF2 Right of car (8x14)
N $DBF2 #HTML[#CALL:graphic($DBF2,8,14,1,1)]
@ $DBF2 label=bitmap_hero_right_down
B $DBF2,28,2 Masked, inverted bitmap data
N $DC0E Down + Turn right parts
N $DC0E Top of car (40x8)
N $DC0E #HTML[#CALL:graphic($DC0E,40,8,1,1)]
@ $DC0E label=bitmap_hero_top_down_right
B $DC0E,80,10 Masked, inverted bitmap data
N $DC5E Bottom of car (40x4)
N $DC5E #HTML[#CALL:graphic($DC5E,40,4,1,1)]
@ $DC5E label=bitmap_hero_bottom_down_right
B $DC5E,40,10 Masked, inverted bitmap data
N $DC86 Left of car (8x13)
N $DC86 #HTML[#CALL:graphic($DC86,8,13,1,1)]
@ $DC86 label=bitmap_hero_left_down_right
B $DC86,26,2 Masked, inverted bitmap data
N $DCA0 Right of car (8x13)
N $DCA0 #HTML[#CALL:graphic($DCA0,8,13,1,1)]
@ $DCA0 label=bitmap_hero_right_down_right
B $DCA0,26,2 Masked, inverted bitmap data
N $DCBA Down + Turn right hard parts
N $DCBA Top of car (40x8)
N $DCBA #HTML[#CALL:graphic($DCBA,40,8,1,1)]
@ $DCBA label=bitmap_hero_top_down_right_hard
B $DCBA,80,10 Masked, inverted bitmap data
N $DD0A Bottom of car (40x4)
N $DD0A #HTML[#CALL:graphic($DD0A,40,4,1,1)]
@ $DD0A label=bitmap_hero_bottom_down_right_hard
B $DD0A,40,10 Masked, inverted bitmap data
N $DD32 Left of car (8x13)
N $DD32 #HTML[#CALL:graphic($DD32,8,13,1,1)]
@ $DD32 label=bitmap_hero_left_down_right_hard
B $DD32,26,2 Masked, inverted bitmap data
N $DD4C Right of car (8x15)
N $DD4C #HTML[#CALL:graphic($DD4C,8,15,1,1)]
@ $DD4C label=bitmap_hero_right_down_right_hard
B $DD4C,30,2 Masked, inverted bitmap data
N $DD6A Shadow parts
N $DD6A Shadow + Straight (56x12)
N $DD6A #HTML[#CALL:graphic($DD6A,56,12,1,1)]
@ $DD6A label=bitmap_shadow_straight
B $DD6A,168,14 Masked, inverted bitmap data
N $DE12 Shadow + Turn right (56x12)
N $DE12 #HTML[#CALL:graphic($DE12,56,12,1,1)]
@ $DE12 label=bitmap_shadow_turn_right
B $DE12,168,14 Masked, inverted bitmap data
N $DEBA Shadow + Turn right hard (56x12)
N $DEBA #HTML[#CALL:graphic($DEBA,56,12,1,1)]
@ $DEBA label=bitmap_shadow_turn_right_hard
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
b $E0B2 Graphics defns <Byte width, Flags, Height, Ptr, Ptr>
N $E0B2 TODO: Ensure these graphic calls don't clash with other emissions.
N $E0B2 Regular: #HTML[#CALL:graphic($7AB1,16,16,0,1)]
B $E0B2,1,1 Width (bytes)
B $E0B3,1,1 Flags
B $E0B4,1,1 Height (pixels)
W $E0B5,2,2 Bitmap
W $E0B7,2,2 Pre-shifted bitmap
N $E0B9 Regular: #HTML[#CALL:graphic($7B31,24,8,1,1)]
B $E0B9,1,1 Width (bytes)
B $E0BA,1,1 Flags
B $E0BB,1,1 Height (pixels)
W $E0BC,2,2 Bitmap
W $E0BE,2,2 Pre-shifted bitmap
N $E0C0 Regular: #HTML[#CALL:graphic($7B91,16,5,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BA5,16,5,1,1)]
B $E0C0,1,1 Width (bytes)
B $E0C1,1,1 Flags
B $E0C2,1,1 Height (pixels)
W $E0C3,2,2 Bitmap
W $E0C5,2,2 Pre-shifted bitmap
N $E0C7 Regular: #HTML[#CALL:graphic($7B95,16,4,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BA9,16,4,1,1)]
B $E0C7,1,1 Width (bytes)
B $E0C8,1,1 Flags
B $E0C9,1,1 Height (pixels)
W $E0CA,2,2 Bitmap
W $E0CC,2,2 Pre-shifted bitmap
N $E0CE Regular: #HTML[#CALL:graphic($7B99,16,3,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BAD,16,3,1,1)]
B $E0CE,1,1 Width (bytes)
B $E0CF,1,1 Flags
B $E0D0,1,1 Height (pixels)
W $E0D1,2,2 Bitmap
W $E0D3,2,2 Pre-shifted bitmap
N $E0D5 Regular: #HTML[#CALL:graphic($7B9D,16,2,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BB1,16,2,1,1)]
B $E0D5,1,1 Width (bytes)
B $E0D6,1,1 Flags
B $E0D7,1,1 Height (pixels)
W $E0D8,2,2 Bitmap
W $E0DA,2,2 Pre-shifted bitmap
N $E0DC Regular: #HTML[#CALL:graphic($7AF1,32,16,0,1)]
B $E0DC,1,1 Width (bytes)
B $E0DD,1,1 Flags
B $E0DE,1,1 Height (pixels)
W $E0DF,2,2 Bitmap
W $E0E1,2,2 Pre-shifted bitmap
N $E0E3 Regular: #HTML[#CALL:graphic($7B61,24,8,1,1)]
B $E0E3,1,1 Width (bytes)
B $E0E4,1,1 Flags
B $E0E5,1,1 Height (pixels)
W $E0E6,2,2 Bitmap
W $E0E8,2,2 Pre-shifted bitmap
N $E0EA Regular: #HTML[#CALL:graphic($7BB9,16,6,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BD1,16,6,1,1)]
B $E0EA,1,1 Width (bytes)
B $E0EB,1,1 Flags
B $E0EC,1,1 Height (pixels)
W $E0ED,2,2 Bitmap
W $E0EF,2,2 Pre-shifted bitmap
N $E0F1 Regular: #HTML[#CALL:graphic($7BBD,16,5,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BD5,16,5,1,1)]
B $E0F1,1,1 Width (bytes)
B $E0F2,1,1 Flags
B $E0F3,1,1 Height (pixels)
W $E0F4,2,2 Bitmap
W $E0F6,2,2 Pre-shifted bitmap
N $E0F8 Regular: #HTML[#CALL:graphic($7BC1,16,4,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BD9,16,4,1,1)]
B $E0F8,1,1 Width (bytes)
B $E0F9,1,1 Flags
B $E0FA,1,1 Height (pixels)
W $E0FB,2,2 Bitmap
W $E0FD,2,2 Pre-shifted bitmap
N $E0FF Regular: #HTML[#CALL:graphic($7BC5,16,3,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BDD,16,3,1,1)]
B $E0FF,1,1 Width (bytes)
B $E100,1,1 Flags
B $E101,1,1 Height (pixels)
W $E102,2,2 Bitmap
W $E104,2,2 Pre-shifted bitmap
N $E106 Regular: #HTML[#CALL:graphic($7AC5,32,11,0,1)]
B $E106,1,1 Width (bytes)
B $E107,1,1 Flags
B $E108,1,1 Height (pixels)
W $E109,2,2 Bitmap
W $E10B,2,2 Pre-shifted bitmap
N $E10D Regular: #HTML[#CALL:graphic($7B3D,24,6,1,1)]
B $E10D,1,1 Width (bytes)
B $E10E,1,1 Flags
B $E10F,1,1 Height (pixels)
W $E110,2,2 Bitmap
W $E112,2,2 Pre-shifted bitmap
N $E114 Regular: #HTML[#CALL:graphic($7B3D,16,4,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BA9,16,4,1,1)]
B $E114,1,1 Width (bytes)
B $E115,1,1 Flags
B $E116,1,1 Height (pixels)
W $E117,2,2 Bitmap
W $E119,2,2 Pre-shifted bitmap
N $E11B Regular: #HTML[#CALL:graphic($7B99,16,3,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BAD,16,3,1,1)]
B $E11B,1,1 Width (bytes)
B $E11C,1,1 Flags
B $E11D,1,1 Height (pixels)
W $E11E,2,2 Bitmap
W $E120,2,2 Pre-shifted bitmap
N $E122 Regular: #HTML[#CALL:graphic($7B9D,16,2,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BB1,16,2,1,1)]
B $E122,1,1 Width (bytes)
B $E123,1,1 Flags
B $E124,1,1 Height (pixels)
W $E125,2,2 Bitmap
W $E127,2,2 Pre-shifted bitmap
N $E129 Regular: #HTML[#CALL:graphic($7BA1,16,1,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BB5,16,1,1,1)]
B $E129,1,1 Width (bytes)
B $E12A,1,1 Flags
B $E12B,1,1 Height (pixels)
W $E12C,2,2 Bitmap
W $E12E,2,2 Pre-shifted bitmap
N $E130 Regular: #HTML[#CALL:graphic($7B05,32,11,0,1)]
B $E130,1,1 Width (bytes)
B $E131,1,1 Flags
B $E132,1,1 Height (pixels)
W $E133,2,2 Bitmap
W $E135,2,2 Pre-shifted bitmap
N $E137 Regular: #HTML[#CALL:graphic($7B6D,24,6,1,1)]
B $E137,1,1 Width (bytes)
B $E138,1,1 Flags
B $E139,1,1 Height (pixels)
W $E13A,2,2 Bitmap
W $E13C,2,2 Pre-shifted bitmap
N $E13E Regular: #HTML[#CALL:graphic($7BA1,16,5,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BB5,16,5,1,1)]
B $E13E,1,1 Width (bytes)
B $E13F,1,1 Flags
B $E140,1,1 Height (pixels)
W $E141,2,2 Bitmap
W $E143,2,2 Pre-shifted bitmap
N $E145 Regular: #HTML[#CALL:graphic($7BC1,16,4,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BD9,16,4,1,1)]
B $E145,1,1 Width (bytes)
B $E146,1,1 Flags
B $E147,1,1 Height (pixels)
W $E148,2,2 Bitmap
W $E14A,2,2 Pre-shifted bitmap
N $E14C Regular: #HTML[#CALL:graphic($7BC5,16,3,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BDD,16,3,1,1)]
B $E14C,1,1 Width (bytes)
B $E14D,1,1 Flags
B $E14E,1,1 Height (pixels)
W $E14F,2,2 Bitmap
W $E151,2,2 Pre-shifted bitmap
N $E153 Regular: #HTML[#CALL:graphic($7BC9,16,2,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BE1,16,2,1,1)]
B $E153,1,1 Width (bytes)
B $E154,1,1 Flags
B $E155,1,1 Height (pixels)
W $E156,2,2 Bitmap
W $E158,2,2 Pre-shifted bitmap
N $E15A Regular: #HTML[#CALL:graphic($7AD9,32,6,0,1)]
B $E15A,1,1 Width (bytes)
B $E15B,1,1 Flags
B $E15C,1,1 Height (pixels)
W $E15D,2,2 Bitmap
W $E15F,2,2 Pre-shifted bitmap
N $E161 Regular: #HTML[#CALL:graphic($7B4F,24,3,1,1)]
B $E161,1,1 Width (bytes)
B $E162,1,1 Flags
B $E163,1,1 Height (pixels)
W $E164,2,2 Bitmap
W $E166,2,2 Pre-shifted bitmap
N $E168 Regular: #HTML[#CALL:graphic($7B99,16,3,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BAD,16,3,1,1)]
B $E168,1,1 Width (bytes)
B $E169,1,1 Flags
B $E16A,1,1 Height (pixels)
W $E16B,2,2 Bitmap
W $E16D,2,2 Pre-shifted bitmap
N $E16F Regular: #HTML[#CALL:graphic($7B9D,16,2,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BB1,16,2,1,1)]
B $E16F,1,1 Width (bytes)
B $E170,1,1 Flags
B $E171,1,1 Height (pixels)
W $E172,2,2 Bitmap
W $E174,2,2 Pre-shifted bitmap
N $E176 Regular: #HTML[#CALL:graphic($7BA1,16,1,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BB5,16,1,1,1)]
B $E176,1,1 Width (bytes)
B $E177,1,1 Flags
B $E178,1,1 Height (pixels)
W $E179,2,2 Bitmap
W $E17B,2,2 Pre-shifted bitmap
N $E17D Regular: #HTML[#CALL:graphic($7BA1,16,1,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BB5,16,1,1,1)]
B $E17D,1,1 Width (bytes)
B $E17E,1,1 Flags
B $E17F,1,1 Height (pixels)
W $E180,2,2 Bitmap
W $E182,2,2 Pre-shifted bitmap
N $E184 Regular: #HTML[#CALL:graphic($7B19,32,6,0,1)]
B $E184,1,1 Width (bytes)
B $E185,1,1 Flags
B $E186,1,1 Height (pixels)
W $E187,2,2 Bitmap
W $E189,2,2 Pre-shifted bitmap
N $E18B Regular: #HTML[#CALL:graphic($7B7F,24,3,1,1)]
B $E18B,1,1 Width (bytes)
B $E18C,1,1 Flags
B $E18D,1,1 Height (pixels)
W $E18E,2,2 Bitmap
W $E190,2,2 Pre-shifted bitmap
N $E192 Regular: #HTML[#CALL:graphic($7BC5,16,3,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BDD,16,3,1,1)]
B $E192,1,1 Width (bytes)
B $E193,1,1 Flags
B $E194,1,1 Height (pixels)
W $E195,2,2 Bitmap
W $E197,2,2 Pre-shifted bitmap
N $E199 Regular: #HTML[#CALL:graphic($7BC9,16,2,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BE1,16,2,1,1)]
B $E199,1,1 Width (bytes)
B $E19A,1,1 Flags
B $E19B,1,1 Height (pixels)
W $E19C,2,2 Bitmap
W $E19E,2,2 Pre-shifted bitmap
N $E1A0 Regular: #HTML[#CALL:graphic($7BCD,16,1,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BE5,16,1,1,1)]
B $E1A0,1,1 Width (bytes)
B $E1A1,1,1 Flags
B $E1A2,1,1 Height (pixels)
W $E1A3,2,2 Bitmap
W $E1A5,2,2 Pre-shifted bitmap
N $E1A7 Regular: #HTML[#CALL:graphic($7BCD,16,1,1,1)] Pre-shifted: #HTML[#CALL:graphic($7BE5,16,1,1,1)]
B $E1A7,1,1 Width (bytes)
B $E1A8,1,1 Flags
B $E1A9,1,1 Height (pixels)
W $E1AA,2,2 Bitmap
W $E1AC,2,2 Pre-shifted bitmap
N $E1AE Smoke graphics (drawn over perp's car when smashed)
N $E1AE #HTML[#CALL:graphic($7A0F,16,13,1,1)]
@ $E1AE label=smoke_defns
B $E1AE,1,1 Width (bytes)
B $E1AF,1,1 Flags
B $E1B0,1,1 Height (pixels)
W $E1B1,2,2 Bitmap
W $E1B3,2,2 Pre-shifted bitmap
N $E1B5 #HTML[#CALL:graphic($7A43,16,11,1,1)]
B $E1B5,1,1 Width (bytes)
B $E1B6,1,1 Flags
B $E1B7,1,1 Height (pixels)
W $E1B8,2,2 Bitmap
W $E1BA,2,2 Pre-shifted bitmap
N $E1BC #HTML[#CALL:graphic($7A6F,16,9,1,1)]
B $E1BC,1,1 Width (bytes)
B $E1BD,1,1 Flags
B $E1BE,1,1 Height (pixels)
W $E1BF,2,2 Bitmap
W $E1C1,2,2 Pre-shifted bitmap
N $E1C3 #HTML[#CALL:graphic($7A93,8,7,1,1)]
B $E1C3,1,1 Width (bytes)
B $E1C4,1,1 Flags
B $E1C5,1,1 Height (pixels)
W $E1C6,2,2 Bitmap
W $E1C8,2,2 Pre-shifted bitmap
N $E1CA #HTML[#CALL:graphic($7AA1,8,5,1,1)]
B $E1CA,1,1 Width (bytes)
B $E1CB,1,1 Flags
B $E1CC,1,1 Height (pixels)
W $E1CD,2,2 Bitmap
W $E1CF,2,2 Pre-shifted bitmap
N $E1D1 #HTML[#CALL:graphic($7AAB,8,3,1,1)]
B $E1D1,1,1 Width (bytes)
B $E1D2,1,1 Flags
B $E1D3,1,1 Height (pixels)
W $E1D4,2,2 Bitmap
W $E1D6,2,2 Pre-shifted bitmap
N $E1D8 Arrow graphic (incl. HERE!)
N $E1D8 #HTML[#CALL:graphic($D03F,24,28,1,1)]
@ $E1D8 label=floating_arrow_big_defn
B $E1D8,1,1 Width (bytes)
B $E1D9,1,1 Flags
B $E1DA,1,1 Height (pixels)
W $E1DB,2,2 Bitmap
W $E1DD,2,2 Pre-shifted bitmap
N $E1DF #HTML[#CALL:graphic($D03F,24,21,1,1)]
@ $E1DF label=floating_arrow_defns
B $E1DF,1,1 Width (bytes)
B $E1E0,1,1 Flags
B $E1E1,1,1 Height (pixels)
W $E1E2,2,2 Bitmap
N $E1E4 #HTML[#CALL:graphic($D03F,24,21,1,1)] (but should be flipped L-R)
B $E1E4,1,1 Width (bytes)
B $E1E5,1,1 Flags
B $E1E6,1,1 Height (pixels)
W $E1E7,2,2 Bitmap
b $E1E9 Referenced by graphic entry 1 and 10
@ $E1E9 label=tunnellight
W $E1E9,2,2
B $E1EB,20,2
@ $E1FF label=tunnellight_lods
B $E1FF,1,1 Width (bytes)
B $E200,1,1 Flags
B $E201,1,1 Height (pixels)
W $E202,2,2 Bitmap
W $E204,2,2 Pre-shifted bitmap
B $E206,1,1 Width (bytes)
B $E207,1,1 Flags
B $E208,1,1 Height (pixels)
W $E209,2,2 Bitmap
W $E20B,2,2 Pre-shifted bitmap
B $E20D,1,1 Width (bytes)
B $E20E,1,1 Flags
B $E20F,1,1 Height (pixels)
W $E210,2,2 Bitmap
W $E212,2,2 Pre-shifted bitmap
B $E214,1,1 Width (bytes)
B $E215,1,1 Flags
B $E216,1,1 Height (pixels)
W $E217,2,2 Bitmap
W $E219,2,2 Pre-shifted bitmap
B $E21B,1,1 Width (bytes)
B $E21C,1,1 Flags
B $E21D,1,1 Height (pixels)
W $E21E,2,2 Bitmap
W $E220,2,2 Pre-shifted bitmap
B $E222,1,1 Width (bytes)
B $E223,1,1 Flags
B $E224,1,1 Height (pixels)
W $E225,2,2 Bitmap
W $E227,2,2 Pre-shifted bitmap
N $E229 Tunnel light (16x16)
N $E229 #HTML[#CALL:graphic($E229,16,16,0,1)]
@ $E229 label=bitmap_tunnellight_1
B $E229,32,8 Bitmap data
N $E249 Tunnel light (16x12)
N $E249 #HTML[#CALL:graphic($E249,16,12,0,1)]
@ $E249 label=bitmap_tunnellight_2
B $E249,24,8 Bitmap data
N $E261 #HTML[#CALL:graphic($E261,8,8,0,1)]
N $E261 Tunnel light (8x8)
@ $E261 label=bitmap_tunnellight_3
B $E261,8,8 Bitmap data
N $E269 #HTML[#CALL:graphic($E269,8,6,0,1)]
N $E269 Tunnel light (8x6)
@ $E269 label=bitmap_tunnellight_4
B $E269,6,6 Bitmap data
N $E26F #HTML[#CALL:graphic($E26F,8,5,0,1)]
N $E26F Tunnel light (8x5)
@ $E26F label=bitmap_tunnellight_5
B $E26F,5,5 Bitmap data
N $E274 #HTML[#CALL:graphic($E274,16,6,1,1)]
N $E274 Tunnel light masked (16x6)
@ $E274 label=tunnellight_6
B $E274,24,8 Masked bitmap data
N $E28C Tunnel light masked shifted (16x6)
N $E28C #HTML[#CALL:graphic($E28C,16,6,1,1)]
@ $E28C label=tunnellight_6s
B $E28C,24,8 Masked bitmap data
b $E2A4 Data for perp escape scene
N $E2A4 Perp escape scene, hazards
@ $E2A4 label=perp_escape_hazards
B $E2A4,6,6
N $E2AA Perp escape scene, curvature
@ $E2AA label=perp_escape_curvature
B $E2AA,1,1 Straight
B $E2AB,2,2 Escape, Command 0 (Continue at <Address>)
W $E2AD,2,2 Loop
N $E2AF Perp escape scene, height
@ $E2AF label=perp_escape_height
B $E2AF,1,1 Level ground
B $E2B0,2,2 Escape, Command 0 (Continue at <Address>)
W $E2B2,2,2 Loop
N $E2B4 Perp escape scene, lanes
@ $E2B4 label=perp_escape_lanes
B $E2B4,1,1 Length byte ?
B $E2B5,1,1 Lanes byte ?
@ $E2B8 label=perp_escape_lanes_loop
B $E2B6,6,1
B $E2BC,2,2 Escape, Command 0 (Continue at <Address>)
W $E2BE,2,2 Loop (partial)
N $E2C0 Perp escape scene, lane objects [not sure now, could be right curve road]
@ $E2C0 label=perp_escape_leftobjs
@ $E2C1 label=perp_escape_leftobjs_loop
B $E2C0,2,1
B $E2C2,2,2 Escape, Command 0 (Continue at <Address>)
W $E2C4,2,2 Loop (partial)
b $E2C6 Data for road forks
N $E2C6 Road fork, curvature
@ $E2C6 label=forked_road_curvature
B $E2C6,1,1
@ $E2C7 label=forked_road_curvature_loop
B $E2C7,1,1 Curve left (67%)
B $E2C8,2,2 Escape, Command 0 (Continue at <Address>)
W $E2CA,2,2 Loop (partial)
N $E2CC Road fork, height
@ $E2CC label=forked_road_height
B $E2CC,1,1
B $E2CD,2,2 Escape, Command 0 (Continue at <Address>)
W $E2CF,2,2 Loop
N $E2D1 Road fork, hazards? Previously thought this was lane data...
@ $E2D1 label=forked_road_hazards
B $E2D1,5,5
@ $E2D6 label=forked_road_rightobjs
B $E2D6,4,4 rightside data  [flips around depending on who's accessing it]
@ $E2DA label=forked_road_leftobjs
B $E2DA,4,4 leftside data   [flips]
b $E2DE Data for road fork exits
D $E2DE Possibly off-by-one here, if the terminator is ($00,$01)
N $E2DE Road fork exit, curvature
@ $E2DE label=forked_road_exit_curvature
B $E2DE,5,5 [ (15x straight), (15x straight), (6x straight), FORK_END ]
N $E2E3 Road fork exit, height
@ $E2E3 label=forked_road_exit_height
B $E2E3,5,5 [ (15x level), (15x level), (6x level), FORK_END ]
N $E2E8 Road fork exit, lanes (left fork)
@ $E2E8 label=forked_road_exit_left_lanes
B $E2E8,7,7 [ (10x Two lanes), (2x $2D?), (10x Three lanes), (2x Widening 3-4 R), (12x Four lanes), FORK_END ]
B $E2EF,5,5
N $E2F4 Road fork exit, lanes (right fork)
@ $E2F4 label=forked_road_exit_right_lanes
B $E2F4,7,7 [ (10x Two lanes), (2x Widening 2-3 L), (10x Three lanes), (2x Widening 3-4 L), (12x Four lanes), FORK_END ]
B $E2FB,5,5
b $E300 Data block at E300
@ $E300 label=table_e300
B $E300,1,1
S $E301,31,$1F
b $E320 Data block at E320
@ $E320 label=table_e320
S $E320,21,$15 21 entries. Used by $CC8E
u $E335 Unused
S $E335,1,$01 padding byte or the start of the next table?
b $E336 Data block at E336
@ $E336 label=table_e336
S $E336,21,$15 21 entries. Used by $8F6E
b $E34B Data block at E34B
D $E34B These bytes all seem to affect the horizon height when meddled with.
B $E34B,3,3 3 bytes set to 8 by #R$880A
B $E34E,1,1
b $E34F Data block at E34F
@ $E34F label=object_positions
S $E34F,21,$15
b $E364 Transition masks
N $E364 Spiral inward animation mask (8x8, 11 frames)
N $E364 #HTML[#CALL:anim($E364,8,8,0,0,11)]
N $E364 #HTML[#CALL:graphic($E364,8,11*8,0,0)]
B $E364,88,8
N $E3BC Circle expanding animation mask (8x8, 7 frames)
N $E3BC #HTML[#CALL:anim($E3BC,8,8,0,0,7)]
N $E3BC #HTML[#CALL:graphic($E3BC,8,7*8,0,0)]
B $E3BC,56,8
u $E3F4 Unused
B $E3F4,28,8*3,4
b $E410 Road edge markings
D $E410 Six sets of 16x8 pixels. Masked. Stored bottom up. 32 bytes each.
N $E410 #HTML[#CALL:graphic($E410,16,8,1,1)]
@ $E410 label=edge_markings
B $E410,8,8 Widest edge. White.
B $E418,24,8
N $E430 #HTML[#CALL:graphic($E430,16,8,1,1)]
B $E430,8,8 Widest edge. Black.
B $E438,24,8
N $E450 #HTML[#CALL:graphic($E450,16,8,1,1)]
B $E450,8,8 Middle edge. White.
B $E458,24,8
N $E470 #HTML[#CALL:graphic($E470,16,8,1,1)]
B $E470,8,8 Middle edge. Black.
B $E478,24,8
N $E490 #HTML[#CALL:graphic($E490,16,8,1,1)]
B $E490,8,8 Thinnest edge. White.
B $E498,24,8
N $E4B0 #HTML[#CALL:graphic($E4B0,16,8,1,1)]
B $E4B0,8,8 Thinnest edge. Black.
B $E4B8,24,8
b $E4D0 Road lane markings
D $E4D0 Three sets of 16x8 pixels. Unmasked. Stored bottom up. 16 bytes each.
N $E4D0 #HTML[#CALL:graphic($E4D0,16,8,0,1)]
@ $E4D0 label=lane_markings
B $E4D0,8,8 Widest marking.
B $E4D8,8,8
N $E4E0 #HTML[#CALL:graphic($E4E0,16,8,0,1)]
B $E4E0,8,8 Middle marking.
B $E4E8,8,8
N $E4F0 #HTML[#CALL:graphic($E4F0,16,8,0,1)]
B $E4F0,8,8 Thinnest marking.
B $E4F8,8,8
b $E500 A table of 32 words being (some function)
D $E500 This is the table, used for forking roads, that bends the road horizontally away from the centre of the screen, as it disappears into the distance.
@ $E500 label=outward_bend_table
W $E500,64,2
w $E540 A table of 96 words being 10^x or similar function
D $E540 This is the table, used for regular roads, that bends the road horizontally towards the centre of the screen as it disappears into the distance.
@ $E540 label=inward_bend_table
W $E540,192,2
b $E600 Data block at E600
D $E600 Seems to be 3x8 groups of 22 bytes. Clearly it's a set of scaling tables. Perhaps drives the stretching of stretchy graphics.
@ $E600 label=data_e600
@ $E6B0 label=data_e6b0
@ $E760 label=data_e760
B $E600,528,22
c $E810 Called once the memory map has been setup
@ $E810 label=entrypt_48k
C $E810,1 Set 128K flag to zero (48K mode)
C $E811,2 3 relocations to do
@ $E816 label=entrypt_128k
C $E816,3 Call clear_game_attrs
C $E819,2 Set 128K flag to one (128K mode)
C $E81B,2 5 relocations to do
C $E81D,3 Store 128K mode flag
C $E820,3 Put stack at end of RAM
C $E823,1 Preserve count in B
C $E824,11 Copy status panel initial pixels to the top of the screen
C $E82F,6 Copy status panel initial attributes to the top of the attribute file
C $E835,4 Call stop_the_tape if in 128K mode  [prob STOP TAPE / PRESS KEY -- does it return?]
C $E839,1 Restore B
C $E83A,3 Point #REGhl at relocations
@ $E83D label=relocate_loop
C $E83D,1 Preserve B
C $E83E,4 DE = wordat(HL); HL += 2
C $E842,1 Stack source pointer
C $E843,4 DE = wordat(HL); HL += 2 -- Destination pointer
C $E847,4 BC = wordat(HL); HL += 2 -- Count
C $E84B,1 Exchange top of stack and HL. HL is now source pointer
C $E84C,2 Copy
C $E84E,2 Restore base pointer and count
C $E850,2 Loop while #REGb > 0
C $E852,3 Set up the stack
C $E855,3 Exit via bootstrap  -- start the game
@ $E858 label=relocations
W $E858,6,2 Copy 24 bytes from data_e88e to $EC00
W $E85E,6,2 Copy 40 bytes from square_zoom_in_mask to $EB00
W $E864,6,2 Copy 48 bytes from diamond_zoom_in_mask to $EA00
W $E86A,6,2 Copy 926 bytes from page_in_stage_128k onwards to $8014 [128K only]
W $E870,6,2 Copy 24 bytes (3 bytes * 8 hooks) from 128k_mode_hooks to hooks at #R$83B5 [128K only]
@ $E876 label=128k_mode_hooks
C $E876,3 Becomes start_siren_hook
C $E879,3 Becomes play_engine_sfx_hook
C $E87C,3 Becomes silence_audio_hook
C $E87F,3 Becomes write_registers_hook
C $E882,3 Becomes play_turbo_sfx_hook
C $E885,3 Becomes setup_engine_sfx_hook
C $E888,3 Becomes play_speech_hook
C $E88B,3 Becomes attract_mode_hook
N $E88E Copied to $EC00
@ $E88E label=data_e88e
B $E88E,1,1
W $E88F,2,2
B $E891,1,1
W $E892,2,2
B $E894,1,1
W $E895,2,2
B $E897,1,1
W $E898,2,2
B $E89A,1,1
W $E89B,2,2
B $E89D,1,1
W $E89E,2,2
B $E8A0,1,1
W $E8A1,2,2
B $E8A3,1,1
W $E8A4,2,2
N $E8A6 Square zoom in animation mask (8x8, 5 frames)
N $E8A6 #HTML[#CALL:anim($E8A6,8,8,0,0,5)]
N $E8A6 #HTML[#CALL:graphic($E8A6,8,5*8,0,0)]
@ $E8A6 label=square_zoom_in_mask
B $E8A6,40,8
N $E8CE Diamond zoom in animation mask (8x8, 6 frames)
N $E8CE #HTML[#CALL:anim($E8CE,8,8,0,0,6)]
N $E8CE #HTML[#CALL:graphic($E8CE,8,6*8,0,0)]
@ $E8CE label=diamond_zoom_in_mask
B $E8CE,48,8
c $E8FE Routine at E8FE
D $E8FE Used by the routine at #R$E810.
@ $E8FE label=stop_the_tape
C $E8FE,3 Call setup_interrupts
C $E901,3 Call music_reset
C $E904,1 Enable interrupts
C $E905,1 Wait for next interrupt
C $E906,3 Call clear_screen
C $E909,3 -> "STOP THE TAPE" + "PRESS ANY KEY" message set
C $E90C,3 Call menu_draw_strings
@ $E90F label=wait_for_keypress
C $E90F,3 Call define_keys
C $E912,6 Was a key pressed?
C $E918,2 Loop if not
N $E91A Debounce.
C $E91A,3 Call define_keys
C $E91D,6 Was a key pressed?
C $E923,2 Loop if it was
C $E925,3 Call clear_screen
C $E928,3 Point #REGhl at input menu messages (NUL terminated)
C $E92B,3 Call menu_draw_strings
C $E92E,3 Call define_keys
C $E931,9 Keyscan for 1, 2, 3, 4, 5
C $E93A,3 Keyscan for 0, 9, 8, 7, 6
C $E93D,3 Keyscan for P, O, I, U, Y
C $E940,3 Keyscan for ENTER, L, K, J, H
C $E943,3 Keyscan for SPACE, SYM SHFT, M, N, B
C $E950,11 Copy 5 bytes at $EE2B to $EE38   -- cursor joystick input scheme
C $E95C,3 Address of three bytes to populate #R$A0CD.. with
C $E95F,4 Set Kempston flag
C $E963,6 Populate #R$A0CD
C $E969,7 Populate $A0D0
C $E970,3 Call clear_screen
C $E973,3 Address of "control options cannot be remodified" text (NUL terminated)
C $E976,3 Call menu_draw_strings
C $E979,3 Call define_keys
C $E97D,7 Keyscan
C $E984,3 Call define_keys
C $E989,7 Keyscan
C $E99C,3 Jump to clear_screen
C $E9AA,3 Call define_keys
b $E9B4 Messages
@ $E9B4 label=messages_stop_the_tape
B $E9B4,1,1 Attribute: Green ink over black
W $E9B5,2,2 Screen position (80,96) (why is this screen, not attrs?)
T $E9B7,13,12:n1 "STOP THE TAPE"
B $E9C4,1,1 Attribute: Cyan ink over black
W $E9C5,2,2 Screen attribute position (4,18)
T $E9C7,25,24:n1 "PRESS ANY KEY TO CONTINUE"
B $E9E0,1,1 Stop marker?
@ $E9E1 label=messages_input_methods
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
@ $EA52 label=messages_redefine_keys
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
@ $EADC label=messages_empty
B $EADC,5,5 Seems to be an empty message structure
@ $EAE1 label=messages_test_mode
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
@ $EB78 label=messages_cannot_be_remodified
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
c $EBFF Renders a string
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
c $EC2C Renders a single character
D $EC2C Compare #R$9FB4
R $EC2C Used by the routine at #R$EBFF.
N $EC2C I:A   The character to plot (ASCII) I:F'  Carry flag set to draw single height characters I:DE' Screen address (UDG aligned) I:HL' Attribute address
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
c $ECDA Clears the screen
D $ECDA Used by the routines at #R$E90F and #R$ECF3.
@ $ECDA label=clear_screen
C $ECDA,12 Wipe bottom 2/3rds of attributes
C $ECE6,12 Wipe bottom 2/3rds of pixels
C $ECF2,1 Return
c $ECF3 Runs the redefine keys screen
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
C $ED0B,3 Read port $00FE
C $ED0E,1 Complement the value returned to change it from active-low to active-high
C $ED0F,2 Discard any non-key flags
C $ED11,2 Jump if any keys are pressed
C $ED16,1 C++
C $ED17,1 HL++
C $ED18,2 Loop rdk_loop_1 while #REGb > 0
C $ED1A,2 B = 20
@ $ED1C label=rdk_loop_2
C $ED1C,1 Preserve
C $ED1D,3 Call define_keys
C $ED20,1 Restore
C $ED21,2 Loop rdk_loop_2 while #REGb > 0
N $ED23 Test keys are "SHOCKED<ENTER>".
C $ED23,2 B = 8
C $ED25,3 DE -> shocked bytes
C $ED28,3 HL -> #R$EE38
@ $ED2B label=rdk_loop_3
C $ED2B,1 A = *DE
C $ED2C,1 CP *HL
C $ED2D,1 HL++
C $ED2E,1 DE++
C $ED2F,1 Return if non-zero -- no match
C $ED30,2 Loop rdk_loop_3 while #REGb > 0
C $ED32,5 Set test mode flag
C $ED37,3 Call clear_screen
C $ED3A,3 Address of TEST MODE strings
C $ED3D,3 Call menu_draw_strings
C $ED40,3 Call define_keys
N $ED43 Debounce?
C $ED43,3 Read port $00FE
C $ED46,1 Complement the value returned to change it from active-low to active-high
C $ED47,2 Discard any non-key flags
C $ED49,2 Loop while no keys were pressed
C $ED4B,2 Loop
c $ED4D Keyscan
D $ED4D Used by the routine at #R$ED6D.
R $ED4D want some examples here
N $ED4D O:D Key half-row number in bits 0..2, key in bits 3+  [or is it inverted?] or $FF if no keys pressed O:F Z clear if keys are pressed 47 = 00101111 = kkkkkrrr (k = key is 5, r = row is 7)
@ $ED4D label=keyscan_all
C $ED4D,3 #REGd = flag/counter? (255 to start), #REGe = initial key and row counters (47 to start)
C $ED50,3 Set #REGb to $FE (initial keyboard half-row selector) and #REGc to $FE (keyboard port number)
N $ED53 Start loop.
@ $ED53 label=ka_loop
C $ED53,2 Read port $<BC>
C $ED55,1 Complement the value returned to change it from active-low to active-high
C $ED56,2 Discard any non-key flags
C $ED58,2 Jump to next iteration if no keys were pressed
N $ED5A Keys were pressed.
C $ED5A,1 ?If #REGd's 255 here we're okay. anything else causes an exit
C $ED5B,1 Return if #REGd is non-zero
C $ED5C,1 Copy key flags to #REGh
C $ED5D,1 Copy key and row counters to #REGa
C $ED5E,2 Decrement key counter bitfield in #REGa
C $ED60,2 Shift a key bit out of #REGh into the carry flag
C $ED62,2 Loop until we hit a set bit (at least one must be set since the zero case is handled earlier)
C $ED64,1 Return if additional bits are set
C $ED65,1 Set return value in #REGd
C $ED66,1 Decrement #REGe
C $ED67,2 Rotate the half-row selector ($FE -> $FD -> $FB -> .. -> $7F)
C $ED69,2 ...loop until the zero bit shifts out (eight iterations)
C $ED6B,2 Set Z
c $ED6D Routine at ED6D
D $ED6D Used by the routine at #R$ECF3.
C $ED6D,2 Preserve #REGde, #REGbc
C $ED6F,3 Call define_keys
C $ED72,3 Call keyscan_all
C $ED75,2 ?Loop while keys are pressed
C $ED77,3 ?No keys were pressed
C $ED7A,1 D--
C $ED7B,1 A = D
C $ED7C,2 Retrieve #REGbc
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
@ $EE26 label=sinclair_joy
B $EE26,5,5
b $EE2B Cursor joystick input scheme
@ $EE2B label=cursor_joy
B $EE2B,5,5
b $EE30 "SHOCKED<ENTER>" ?
@ $EE30 label=shocked
B $EE30,8,8
b $EE38 temp input scheme buffer?
@ $EE38 label=input_tmp
B $EE38,5,5
b $EE3D Bytes copied to $A0CD
@ $EE3D label=data_ee3d
B $EE3D,3,3
c $EE40 Interrupt setup
D $EE40 Used by the routine at #R$E8CE.
@ $EE40 label=setup_interrupts
C $EE40,1 Disable interrupts
C $EE41,3 HL = $FD00
C $EE44,1 A = $FD
N $EE45 Set the 128 words at $FD00 to $FEFE.
C $EE45,2 B = 0 -- 256 iterations, or 255?
C $EE47,2 C = $FE
C $EE49,2 *HL++ = C
C $EE4B,2 Loop
C $EE4D,1 Store final $FE
C $EE4E,2 Set interrupt vector base to $FD00
C $EE50,2 Set interrupt mode 2
N $EE52 Set $FEFE to be "JP $EF19".
C $EE52,5 $FEFE = $C3  -- opcode for JP
C $EE57,6 $FEFF = #R$EF19
C $EE5D,1 Return
c $EE5E Routine at $EE5E  -- suspect a music reset routine
D $EE5E Used by the routine at #R$E8CE.
@ $EE5E label=music_reset
C $EE5E,1 A = 0
C $EE5F,3 Self modify 'LD A,x' @ #R$EF0D  -- clear drum flag?
C $EE62,3 Self modify 'LD A,x' @ #R$EF00  -- in define_keys
C $EE65,3 Self modify 'LD A,x' @ #R$EEA2  -- in define_keys
C $EE68,3 -> music data
C $EE6B,3 Jump to j_ee78
c $EE6E Routine at EE6E
D $EE6E Used by the routine at #R$EE9E.
@ $EE6E label=sub_EE6E
C $EE6E,2 Self modified by #R$EE7E
C $EE70,1 A--
C $EE71,3 Self modify #R$EE6E
C $EE74,1 Return if non-zero  -- so it's a delay?
C $EE75,3 Self modified by #R$EE83 [sampled: F10C, F10E, F102, F104, F106, F108, F10A, ]
N $EE78 This entry point is used by the routine at #R$EE40.
@ $EE78 label=j_ee78
C $EE78,2 A = *HL++
C $EE7A,4 Jump to #R$EE98 if it's $FF
C $EE7E,3 Self modify #R$EE6E
C $EE81,2 C = *HL++
C $EE83,3 Self modify #R$EE75
C $EE86,2 B = 0
C $EE88,3 Address of music data
C $EE8B,1 HL += BC
C $EE8C,2 A = *HL++
C $EE8E,3 Self modify #R$EEB9
C $EE91,3 Self modify #R$EEAD
C $EE94,3 Self modify #R$EEC9
C $EE97,1 Return
C $EE98,4 HL = wordat(HL); HL++
C $EE9C,2 Goto j_ee78
c $EE9E Define keys
D $EE9E Loads of self modifying hopping around... likely to need a better name.
R $EE9E Used by the routines at #R$E90F, #R$ECF3 and #R$ED6D.
@ $EE9E label=define_keys
C $EE9E,4 Enable wait/spinlock at #R$EF13
C $EEA2,2 Self modified by #R$EEA8
C $EEA4,1 Set flags
C $EEA5,2 Jump to #R$EEAD if non-zero
C $EEA7,1 A++
C $EEA8,3 Self modify #R$EEA2
C $EEAB,2 Jump to dk_eec9
C $EEAD,2 Self modified by #R$EEBB, cycles 5,3,2,1
C $EEAF,1 A--
C $EEB0,3 Jump to dk_eeb9 if zero
C $EEB3,3 Self modified by #R$EE8E
C $EEB6,3 Jump to dk_ef00
@ $EEB9 label=dk_eeb9
C $EEB9,2 Self modified by #R$EE8E
C $EEBB,3 Self modify 'LD A' @ #R$EEAD
C $EEBE,3 Self modified, cycles $F12x .. $F2xx ish
@ $EEC1 label=dk_eec1
C $EEC1,2 A = *HL - 1
C $EEC3,3 Jump to dk_eed2 if non-zero
C $EEC6,3 Call sub_EE6E
@ $EEC9 label=dk_eec9
C $EEC9,3 HL = xxxx     Self modified by #R$EE94
C $EECC,3 *$EEBF = HL
C $EECF,3 Goto dk_eec1
@ $EED2 label=dk_eed2
C $EED2,1 HL++
C $EED3,3 *$EEBF = HL
C $EED6,1 A++
C $EED7,2 >= 128?
C $EEDB,2 A &= $7F   note
C $EEDE,5 Self modify 'LD A' @ #R$EEAD
C $EEE3,3 Self modify 'LD A' @ #R$EF00
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
C $EEFD,3 Jump to #R$F0C6 if zero
@ $EF00 label=dk_ef00
C $EF00,2 Self modified by #R$EEE3, #R$EF09
C $EF02,1 Set flags
C $EF03,2 Jump to dk_ef0d if zero
C $EF05,3 Self modify 'LD A' @ #R$EEAD
C $EF08,1 (*HL)--
C $EF09,3 Self modify 'LD A' @ #R$EF00
C $EF0C,1 (*HL)--
@ $EF0D label=dk_ef0d
C $EF0D,2 Self modified by #R$EF33
C $EF0F,1 A--
C $EF10,3 Jump to playdrum_go if zero
N $EF13 This entry point is used by the routines at #R$EF22 and #R$F0C6.
@ $EF13 label=dk_wait
C $EF13,5 Wait/spinlock.  Self modified
C $EF18,1 Return
c $EF19 Routine at $EF19
D $EF19 How does this get entered? $EE40 builds a JP $EF19 that's interrupt driven.
@ $EF19 label=sub_ef19
C $EF19,1 Preserve registers
C $EF1A,5 Unlock the wait/spinlock  -- Self modify 'LD A,x' @ #R$EF13
C $EF1F,1 Restore registers
C $EF20,1 Enable interrupts
C $EF21,1 Return
c $EF22 Drum sample player
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
C $EF2E,3 Self modify 'LD B' @ #R$EF39 to be A  -- speed value passed in?
C $EF31,5 Self modify 'LD A' @ #R$EF0D to be 1  -- set to 1 while playing?
C $EF36,2 Jump to playdrum_go
N $EF38 This entry point is used by the routine at #R$EE9E.
@ $EF39 label=playdrum_go
C $EF39,2 Self modified by #R$EF2E (sampled: 8, 3, 1)
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
C $EF4E,3 Read A from 'LD A' @ #R$EF13  -- wait/spinlock
C $EF51,1 Set flags
C $EF52,3 Jump to playdrum_go if zero
C $EF56,1 Return
@ $EF57 label=playdrum_end_of_sample
C $EF57,4 Self modify 'LD A' @ #R$EF0D to be 0  -- set to 0 when playing stops?
C $EF5B,3 Jump to dk_playdrum_finished
@ $EF5E label=drum1
B $EF5E,252,8*31,4 Drum 1 sample/data
@ $F05A label=drum2
B $F05A,108,8*13,4 Drum 2 sample/data
c $F0C6 White noise generator?
D $F0C6 Used by the routine at #R$EE9E.
@ $F0C6 label=noise
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
C $F0DE,2 Jump to #R$F0F0 if zero
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
C $F0F3,3 Read A from 'LD A' @ #R$EF13  -- spinlock/wait
C $F0F6,1 Set flags
C $F0F7,1 Return if carry set
C $F0F8,1 Decrement outer-outer counter
C $F0F9,2 Jump to noise_outer_loop if non-zero
C $F0FB,3 Jump to dk_wait
b $F0FE Music data
@ $F0FE label=music_patterns
B $F0FE,4,4
W $F102,14,2 Patterns (offset, repetitions?)
B $F110,1,1
B $F111,271,8*33,7 Music
c $F220 128K mode routines and data
D $F220 This is relocated to $8014/load_stage onwards during init (926 bytes long).
@ $F220 label=page_in_stage_128k
C $F220,3 Load wanted_stage_number
C $F223,3 Address of current_stage_number
C $F226,2 Return if the stage is already loaded
C $F228,1 wanted_stage_number = current_stage_number
N $F229 Copy the stage data from the correct bank and address to $5C00..$76EF.
C $F229,8 Point #REGhl at stage_data_locations[current_stage_number]
C $F231,1 Load paging flags byte
C $F232,1 HL++
C $F233,2 #REGhl = Source data address (#REGb is zero)
C $F235,5 128K: Page in required bank
C $F23A,3 Destination $5C00..$76EF (from the horizon backdrop to just before the turbo icons)
C $F23D,3 Bytes to copy (worst case)
C $F240,2 Copy
C $F242,3 Exit via relocated reset_paging_128k
N $F245 Pairs of (top byte of source data address, paging flags).
@ $F245 label=stage_data_locations
W $F245,2,2 Level 1. Source = $C000, Paging = bank 1
W $F247,2,2 Level 2. Source = $E000, Paging = bank 1
W $F249,2,2 Level 3. Source = $C000, Paging = bank 6
W $F24B,2,2 Level 4. Source = $E000, Paging = bank 6
W $F24D,2,2 Level 5. Source = $C000, Paging = bank 7
W $F24F,2,2 Level 6. Source = $E000, Paging = bank 7
N $F251 $8045 once relocated.
@ $F251 label=start_siren_hook_128k
C $F251,5 Store $8C to channel A fine pitch
C $F256,5 Store 14 to channel A volume (4-bit)
C $F25B,5 Store 12 to channel B volume
C $F260,5 Self modify 'LD A,x' @ #R$F271 (in this position)
C $F265,3 var_a239 = $AA
C $F268,1 Return
@ $F269 label=engine_sfx_play_hook_128k
C $F269,3 var_a239 (initialised to $AA above)
C $F26C,2 Return if zero
C $F26E,3 Load channel A fine pitch
C $F271,6 jump on 50-50 alternating pattern? -- could be flipping between string sets?
N $F277 Decreasing case
C $F277,2 A -= 3
C $F279,4 Jump if A >= $5A
C $F27D,2 Jump
N $F27F Increasing case
@ $F27F label=f27f_128k
C $F27F,2 A += 3
C $F281,4 Jump if A < $8C
N $F285 Arrive here if A is outside of $5A..$8B
@ $F285 label=f285_128k
C $F286,4 Store B (rotating pattern)
N $F28B Arrive here if A is $5A..$8B
@ $F28B label=f28b_128k
C $F28B,3 Update channel A fine pitch
C $F28E,5 and store 4 less to channel B fine pitch
C $F293,8 Set mixer to enable tone A & B
C $F29B,2 Jump
@ $F29D label=silence_audio_hook_128k
C $F29D,5 Initialise mixer to $3F (all noise and tone off)
N $F2A2 writing the full register set?
@ $F2A2 label=write_registers_hook_128k
C $F2A2,3 Address of sound register value(s) -- other values must be earlier
@ $F2A9 label=write_registers_hook_loop
C $F2A5,8 Select AY-3-8912 sound chip register 11: envelope fine duration
C $F2AD,4 Write to the register from (HL), then decrement B and HL
C $F2B1,1 Next register down
C $F2B2,3 Loop to #R$F2A9 while +ve
C $F2B5,1 Return
@ $F2B6 label=f2b6_128k
C $F2B6,3 Load speed into #REGhl
C $F2B9,2 Bottom bit of #REGh moves to carry
C $F2BB,1 Speed low byte
C $F2BC,1 Halve speed, shifting carry in as MSB
C $F2BD,2 L = ~A
C $F2BF,6 Jump if in low gear
C $F2D1,3 A = tunnel_sfx
C $F2D4,1 Set flags
C $F2D5,3 -- base tone value?
C $F2DA,2 Jump if tunnel_sfx was zero
C $F2DC,3 -- base tone value?
C $F2DF,2 Volume = 12
C $F2E1,1 -- speed value + base tone?
C $F2E2,3 Set channel C pitch (both fine and course)
C $F2E5,3 Set channel C volume
C $F2E8,8 Set mixer to enable tone C
C $F2F0,1 Return
@ $F2F1 label=turbo_sfx_play_hook_128k
C $F2F1,5 Set noise pitch (5-bit) [$3C is > 5-bit...]
C $F2F6,3 var_a23a = $3C  -- Copy of it?
C $F2F9,1 Return
@ $F2FA label=engine_sfx_setup_hook_128k
C $F2FA,6 Jump if var_a23a is zero
C $F300,1 Decrement noise pitch
C $F301,1 Return if zero
C $F302,3 Address of noise pitch register soft copy
C $F305,1 Decrement in-place
C $F306,2 Jump if zero
C $F308,6 Increment noise pitch register soft copy by 10
C $F30E,3 Set channel C pitch (both fine and course)
C $F311,8 Set mixer to enable tone C and noise C
C $F319,5 Set channel C volume to 13
C $F31E,1 Return
@ $F31F label=f31f_128k
C $F31F,8 Set mixer to disable tone C and noise C
C $F327,4 var_a23a = 0
C $F32B,3 Exit via f2b6_128k
N $F32E Relocated to $8122
N $F32E "Giddy up boy!"
@ $F32E label=speech_samples_table
W $F32E,2,2 length
W $F330,2,2 address
N $F332 "Let's go Mr. Driver!"
W $F332,2,2 length
W $F334,2,2 address
N $F336 "Hold on man!"
W $F336,2,2 length
W $F338,2,2 address
N $F33A "Your time's up"
W $F33A,2,2 length
W $F33C,2,2 address
N $F33E Start noise
W $F33E,2,2 length
W $F340,2,2 address
@ $F342 label=play_speech_128k
C $F342,1 Bank input index (sample indices are 1..5)
C $F343,3 Call silence_audio_hook_128k
C $F346,7 128K: Map RAM bank 4 to $C000; Map normal screen; Map ROM 0
C $F355,1 Unbank input index
C $F356,9 HL = $F32A + A*4  -- i.e. it's 1-indexed speech_samples_table
C $F35F,4 DE = wordat(HL); HL += 2  -- read length
C $F363,4 HL = wordat(HL)  -- read address
@ $F367 label=plsp_f367_128k
C $F367,2 C = 2  -- iterations (two nibbles)
C $F369,1 A = *HL  -- read a sample (or two?)
C $F36A,4 A = A ROR 4
@ $F36E label=plsp_f36e_loop
C $F36E,2 A &= 15
C $F372,1 B = H which is $FF
C $F373,1 A = D which is 8  -- register 8: Channel A volume
C $F374,2 C is $FD
C $F376,1 B = L which is $BF
C $F377,1 A = value loaded above
C $F378,2 Write to register
C $F37A,1 Bank it again
C $F37B,1 8 -> 9  -- register 9: Channel B volume
C $F37C,1 B = $FF
C $F37D,2 C is $FD
C $F37F,1 B = L which is $BF
C $F380,1 A = value loaded above
C $F381,2 Write to register
C $F383,1 Bank it again
C $F384,1 9 -> 10  -- register 9: Channel C volume
C $F385,1 B = $FF
C $F386,2 C is $FD
C $F388,1 B = L which is $BF
C $F389,1 A = value loaded above
C $F38A,2 Write to register
C $F38D,4 Delay loop (lower value => higher frequency)
C $F391,1 Load next nibble (same byte, but next nibble)
C $F392,1 Decrement nibble counter
C $F393,2 Loop plsp_f36e_loop
C $F395,1 Advance to next byte of sample data
C $F396,1 Decrement sample data counter
C $F397,5 Loop to plsp_f367_128k while sample data remains
C $F39C,3 Exit via relocated reset_paging_128k
@ $F39F label=plsp_f39f_128k
C $F39F,3 -- frame delay?
C $F3A2,3 Return if A < 42
C $F3A5,3 Call silence_audio_hook
C $F3A8,4 var_a239 = 0
C $F3AC,1 A = 1
C $F3AD,3 Load var_a23a  -- copy of noise pitch
C $F3B0,3 Self modify #R$8E49
C $F3B3,3 (an address)
N $F3B6 Starts the animated title screen?
@ $F3B6 label=plsp_f3b6_128k
C $F3B6,3 Self modify 'CALL xxxx' @ $81C5 ($F3D1 before relocation - below)
C $F3B9,14 Copy 4096 bytes from $B000 to $F000 (preserving registers for later)
C $F3C7,4 Self modify 'LD SP,xxxx' @ #R$81CD ($F3D9 here - below)
C $F3CB,3 Set stack pointer
C $F3CE,3 Call relocated f3e2_128k
C $F3D1,3 Self modified by #R$F3B6
C $F3D4,1 Preserve ?
C $F3D5,3 Call relocated f3e2_128k
C $F3D8,1 Restore ?
C $F3D9,3 Restore original #REGsp (self modified by #R$F3C7 above)
C $F3DC,3 Copy 4096 bytes from $B000 to $F000
C $F3DF,2 Copy
C $F3E1,1 Return
@ $F3E2 label=f3e2_128k
C $F3E2,3 HL = $C000
C $F3E5,2 4 iterations
C $F3E7,1 E = 0
@ $F3E8 label=f3e8_loop
C $F3E8,1 Preserve iterations
C $F3E9,2 DE = $B000
C $F3EB,3 4096 bytes
C $F3EE,2 Preserve
C $F3F0,2 Copy
C $F3F2,2 A = 3
C $F3F4,3 Jump into setup_engine_sfx with engine speed setup
C $F3F7,4 Retrieve from stack
C $F3FB,2 DE = $B000
@ $F3FD label=f3fd_loop
C $F3FD,1 A = *DE
C $F3FE,2 Transfer a byte
C $F400,3 HL[-1] = A
C $F403,3 Loop if (parity even) to f3fd_loop
C $F406,3 Call relocated reset_paging_128k
C $F409,2 Restore
C $F40B,2 HL = $B000
C $F40D,2 Copy
C $F40F,1 H = D
C $F410,1 Restore iterations
C $F411,2 Loop to f3e8_loop
C $F413,1 Return
@ $F414 label=reset_paging_128k
C $F414,6 128K: Set paging register to default
C $F41A,1 Return
@ $F41B label=attract_mode_hook_128k
C $F41B,3 (an address)
C $F41E,3 Call relocated plsp_f3b6_128k (animated title screen?)
C $F421,2 Return if A is zero
C $F423,3 HL -> attract_data
C $F426,3 Call set_up_stage
C $F429,5 var or self modify or ..?
C $F42E,6 Set speed to $190
@ $F434 label=f434_128k_loop
C $F434,3 Call cpu_driver
C $F437,3 Load attract_cycle
C $F43A,1 Set flags
C $F43B,3 -> enter_for_options_messages
C $F43E,2 Jump to f44b_key_check
C $F440,3 Call keyscan
C $F443,2 Was FIRE pressed?
C $F445,3 Jump to play_start_noise if so
C $F448,3 -> press_gear_messages
@ $F44B label=f44b_key_check
C $F44B,4 Read port $BFFE -- ENTER, L, K, J, H
C $F44F,1 Complement the value returned to change it from to active-high
C $F450,1 ?Shift out lsb
C $F454,2 ?Jump if ENTER was pressed
C $F459,1 -- why load A then shift? self modified?
C $F460,3 Call print_message
C $F463,6 If transition_control != 0 jump
C $F469,2 -- smells like self modified
C $F46F,1 A--
C $F470,3 self modifying?
C $F476,2 Forward transition
C $F478,3 Call setup_transition
@ $F47D label=f47d_128k
C $F47D,3 -- must be messages ptr below ($8208+165 means ?)
C $F482,3 -- must be messages ptr below ($8208+263 means ?)
@ $F485 label=f485_128k
C $F485,3 Call setup_overlay_messages
@ $F488 label=f488_128k
C $F488,3 Call transition
C $F48B,3 Call draw_screen
C $F48E,3 Loop to f434_128k_loop
b $F491 Credits / Score messages show in attract mode
@ $F491 label=press_gear_messages
B $F491,6,6
T $F497,11,10:n1 "PRESS GEAR" (blinks)
@ $F4A2 label=enter_for_options_messages
B $F4A2,6,6
T $F4A8,17,16:n1 "ENTER FOR OPTIONS" (blinks)
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
N $F537 scores and names are updated somewhere
T $F537,28,27:n1 "RANK  SCORE  STAGE PLAY NAME"
B $F553,7,7
T $F55A,28,27:n1 "1ST  56784010  ALL    1  JOB"
B $F576,7,7
T $F57D,28,27:n1 "2ND  35678000   4     1  ABC"
B $F599,7,7
T $F5A0,28,27:n1 "3RD   4340300   3     2  DEF"
B $F5BC,2,2
b $F5BE Status panel initial image
@ $F5BE label=status_panel
B $F5BE,2048,32
b $FDBE Status panel initial attributes
@ $FDBE label=status_panel_attrs
B $FDBE,256,8
u $FEBE Unused
B $FEBE,322,8*40,2
