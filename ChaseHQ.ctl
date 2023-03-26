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
; $F000..$FFFF is a 4KB back buffer (once the game's up and running)
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

b $5B00 Graphics
B $5B00,240 Hill backdrop (80x24) seems to be pre-shifted by #R$C8BE
B $5C00,240 Hill backdrop (80x24)

b $5D08
W $5D10,2 -> car_lods
W $5D12,2 -> lambo_lods
W $5D14,2 -> truck_lods
W $5D16,2 -> lambo_lods
W $5D18,2 -> car_lods

b $5D3A
W $5D3A,2 Points at "THIS IS NANCY..."
W $5D3C,2 Points at "EMERGENCY HERE..."
W $5D3E,2 Points at "IS FLEEING ..."
W $5D40,2 Points at "VEHICLE IS..."

b $5D45 Messages - Nancy
T $5D45,40 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $5D6D,40 "EMERGENCY HERE. RALPH THE IDAHO SLASHER,"
T $5D95,42 "IS FLEEING TOWARDS THE SUBURBS. THE TARGET"
T $5DBF,46 "VEHICLE IS A WHITE BRITISH SPORTS CAR... OVER."

b $5DED Messages - arrest
B $5DED,1 frame delay until first message?
B $5DEE,1 frame delay until next message?
B $5DEF,1 Flags (?)
B $5DF0,1 Attribute (black)
W $5DF1,2 Back buffer address
W $5DF3,2 Attribute address
T $5DF5,27 "OK! YOU ARE UNDER ARREST ON"
;
B $5E10,1 frame delay until next message?
B $5E11,1 Flags (?)
B $5E12,1 Attribute (black)
W $5E13,2 Back buffer address
W $5E15,2 Attribute address
T $5E17,26 "SUSPICION OF FIRST DEGREE "
;
B $5E31,1 frame delay until next message?
B $5E32,1 Flags (?)
B $5E33,1 Attribute (black)
W $5E34,2 Back buffer address
W $5E36,2 Attribute address
T $5E38,6 "MURDER"

b $5E3E Graphics. All are stored inverted except where noted.
;
W $5E41,2 Address of tumbleweed LODs table
W $5E44,2 Address of another LOD table

;
B $638A,160 Perp w/ sunglasses (32x40). Stored top-down.
B $642A TBD
;
; Flags in these LOD structures: 1 seems to indicate
; interleaved masks, 2 is probably to flip
;
@ $643E label=lambo_lods
B $643E Width (bytes)
B $643F Flags
B $6440 Height (pixels)
W $6441,2 Bitmap
W $6443,2 Mask
L $643E,7,6
;
@ $6468 label=truck_lods
B $6468 Width (bytes)
B $6469 Flags
B $646A Height (pixels)
W $646B,2 Bitmap
W $646D,2 Mask
L $6468,7,6
;
@ $6492 label=car_lods
B $6492 Width (bytes)
B $6493 Flags
B $6494 Height (pixels)
W $6495,2 Bitmap
W $6497,2 Mask
L $6492,7,6
;
B $64BC,180 Lamborghini_1 (48x30)
B $6570,110 Lamborghini_2 (40x22)
B $65DE,45 Lamborghini_3 (24x15)
B $660B,234 Truck_1 (48x39)
B $66F5,145 Truck_2 (40x29)
B $6786,60 Truck_3 (24x20)
B $67C2,186 Car_1 (48x31)
B $687C,110 Car_2 (40x22)
B $68EA,48 Car_3 (24x16)
B $691A,24 Lamborghini_4 perhaps (24x8)
B $694A,24 Lamborghini_5 mask (24x8)
B $697A,24 Truck_4/5
B $69AA,24 Truck_4/5 mask
B $69DA,27 Car_4/5
B $6A10,27 Car_4/5 mask
B $6932 TBD
B $6962 TBD
B $6992 TBD
B $69C2 TBD
B $69C2 TBD
B $69F5 TBD
B $6A2B mystery block
;
B $6A46 Width (bytes)
B $6A47 Flags
B $6A48 Height (pixels)
W $6A49,2 Bitmap
W $6A4B,2 Mask
L $6A46,7,12
;
B $6A9A,10 bitmap interleaved
B $6AA4 TBD
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
B $6B22 Width (bytes)
B $6B23 Flags
B $6B24 Height (pixels)
W $6B25,2 Bitmap
W $6B27,2 Mask
L $6B22,7,10
;
B $6B68,160 Turn right sign (32x40)
B $6C08,90 Turn right sign (24x30)
B $6C62,40 Turn right sign (16x20)
B $6C8A,32 TBD 16x16
B $6CAA TBD
B $6CCA,26 TBD 16x13
B $6CE4 TBD
B $6CFE,26 TBD 16x13
;
B $6D18 TBD looks like graphics
;
B $6D82 Width (bytes)
B $6D83 Flags
B $6D84 Height (pixels)
W $6D85,2 Bitmap
W $6D87,2 Mask
L $6D82,7,6
;
B $6DAC,32 Tumbleweed_1 (16x16)
B $6DCC,22 Tumbleweed_2 (16x11)
B $6DE2,9 Tumbleweed_3 (8x9)
B $6DEB,7 Tumbleweed_4 (8x7)
;
B $6DF2 Width (bytes)
B $6DF3 Flags
B $6DF4 Height (pixels)
W $6DF5,2 Bitmap
W $6DF7,2 Mask
L $6DF2,7,6
;
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
B $7860 Seems to be tiles pointed at by car rendering code
B $78B6,64 8 Tiles used for borders on the pre-game screen

B $7BE9,160 Graphic: Nancy's face (32x40). Stored top-down.
B $7C89 unknown
B $7C9D,160 Graphic: Raymond's face (32x40). Stored top-down.
B $7D3D unknown
B $7D52,160 Graphic: Tony's face (32x40). Stored top-down.
B $7DF2 unknown

b $8000 temporaries?
B $8002,4 Score digits as BCD (4 bytes / 8 digits)
B $8007,1 Current stage number (1...)

c $8014

c $8088

c $80B9 Tape loading
  $80B9 Setup to load a block ???
  $80C0 Routine
  $814B Subroutine

t $8169 Mainly tape loading messages
;
B $8169,1 ?
W $816A,2 Screen position (8,64)
W $816C,2 Screen attribute position (1,8)
  $816E,30 "REWIND TAPE TO START OF SIDE 2"
;
B $818C,1 ?
W $818D,2 Screen position (88,96)
W $818F,2 Screen attribute position (11,12)
  $8191,10 "START TAPE"
;
B $819B,1 ?
W $819C,2 Screen position (72,128)
W $819E,2 Screen attribute position (9,16)
  $81A0,15 "SEARCHING FOR 1"
;
B $81AF,1 ?
W $81B0,2 Screen position (104,160)
W $81B2,2 Screen attribute position (13,20)
  $81B4,7 "FOUND 1"
;
B $81BB,1 ?
W $81BC,2 Screen position (80,96)
W $81BE,2 Screen attribute position (10,12)
  $81C0,13 "STOP THE TAPE"
;
B $81CD,1 ?
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
C $8246 Produces engine sound effect
C $824E Produces the steering squeak

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
c $8401 Possible main game loop?
c $852A
c $858C
b $85DD
c $85E4
c $860F
c $865A
b $86F6

c $87DC
C $8807 Pre-shift the backdrop image
C $8857 Point #REGhl at left light's attributes
C $885D Point #REGhl at right light's attributes then FALL THROUGH
@ $8860 label=clear_lights
C $8860 Clear the lights' BRIGHT bit
; flashing lights are 5 attrs wide, 4 high

c $8876

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
c $8C3A

b $8C58 End of level messages
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
B $8CB5,1 Attribute (black)
W $8CB6,2 Back buffer address
W $8CB8,2 Attribute address
T $8CBA,26 "SIGHTING OF TARGET VEHICLE"
;
B $8CD9,1 Attribute (black)
W $8CDA,2 Back buffer address
W $8CDC,2 Attribute address
T $8CDE,20 "OK! PULL OVER CREEP!"
;
B $8CF7,1 Attribute (black)
W $8CF8,2 Back buffer address
W $8CFA,2 Attribute address
T $8CFC,9 "GAME OVER"
;
B $8D0A,1 Attribute (black)
W $8D0B,2 Back buffer address
W $8D0D,2 Attribute address
T $8D0F,7 "TIME UP"
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

c $8D8F
c $8DD8
c $8DF9
c $8E29 Spreads attribute bytes from left, filling the attrs.

c $8E42
c $8E6C Message printing related. Increments HL then loads A,E,D,C,B from where HL points.
c $8E7E
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
D $952E Decrement the screen address
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
c $9618 PRNG - nobble this and only Lambos spawn on the left
B $9618,3
s $962E

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

w $98A9 Block containing some pointers to the above strings
W $98A9
W $98AB
W $98AD
W $98AF
W $98B1
W $98B3
W $98B5
W $98B7
W $98B9
W $98BB
W $98BD
W $98BF
W $98C1
W $98C3
W $98C5 -> "WE READ LOUD AND CLEAR! OVER."
W $98C7
W $98C9 -> "ROGER!"
W $98CB
W $98CD -> "GOTCHA NANCY BABY!"
W $98CF
W $98D1 -> "WHAT ARE YOU DOING MAN!!"
W $98D3 -> "THE BAD GUYS ARE GOING THE OTHER WAY."
W $98D5
W $98D7
W $98D9
W $98DB
W $98DD
W $98DF
W $98E1
W $98E3
W $98E5
W $98E7
W $98E9
W $98EB
W $98ED
W $98EF
W $98F1
W $98F3
W $98F5 -> "PLEASE!"
W $98F7
W $98F9 -> "LET'S GET MOVIN' MAN!"
W $98FB
W $98FD -> "THIS IS NANCY AT CHASE H.Q."
W $98FF -> "IF YOU KEEP MESSIN' AROUND LIKE THAT"
W $9901 -> "YOUR TIME IS GOING TO RUN OUT... OVER."
W $9903
W $9905
W $9907
W $9909
W $990B
W $990D
W $990F
W $9911
W $9913
W $9915
W $9917
W $9919
W $991B
W $991D
W $991F -> "WHOAAAAA!"
W $9921
W $9923 -> "GREAT!"
W $9925
W $9927 -> "ONE MORE TIME."
W $9929
W $992B
W $992D
W $992F
W $9931
W $9933
W $9935
W $9937
W $9939
W $993B
W $993D
W $993F
W $9941
W $9943

c $9945
c $9965
c $99EC
c $9A55
c $9AAB
c $9AEC
c $9AF1
b $9B68
c $9B6C
c $9BA7

c $9BCF
C $9C70 Set remaining time to 60 (BCD)

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
C $9D62 Check if stage has changed?
C $9D6B Adding 48 to make it a digit then $80 to terminate the string.
C $9D6D Update "STAGE N"
C $9D73 Screen pixel (48,36)
C $9D76 Point at stage text
C $9D79 Draw it
C $9D7C Is the bonus flag set?
C $9D82 Clear it
C $9D86 Screen pixel (64,24)
C $9D89 Clear the area 5*7 bytes
C $9D9E Draw it
C $9DB4 42 => BRIGHT + red over black
C $9DB8 46 => BRIGHT + yellow over black
C $9DBA Point at attributes (8,3)
C $9DBD Set them to #REGc
C $9DC3
C $9DCD Screen pixel (118,36)
C $9DD0 "LO"
C $9DD6 "HI"
C $9DD9 Draw it

C $9DE6 Point #REGhl at left light's attributes
C $9DE9 Toggle its brightness
C $9DEC Point #REGhl at right light's attributes
C $9DEF Toggle its brightness

@ $9DF4 label=toggle_light_brightness
c $9DF4 Toggle the light's BRIGHT bit (#REGhl -> attrs)
C $9DF4 B = 4, C = $40 (BRIGHT bit)
C $9DF7 Toggle attribute byte on five successive locations
C $9E0A Move to next attribute row
C $9E0E Repeat for four rows

@ $9E11 label=plot_scores_etc
c $9E11
@ $9E7B label=plot_scores_only
C $9E7B Point #REGde at speed digits screen position
C $9EB6 Plot a digit
C $9EBA Plot a digit
C $9EBE Plot a digit
C $9F03 Point #REGde at distance digits screen position (136,33)
C $9F12 Point #REGde at score digits screen position
C $9F16 Point #REGde at score (four BCD bytes)
C $9F19 Point #REGhl at score digits (eight bytes)
C $9F1E C = A = *DE; A >>= 4; if (A == *HL) ...
C $9F1F Keep for later
C $9F26 If different than stored then plot
C $9F2D Examine next digit
C $9F30 If different than stored then plot
C $9EC1 Point #REGde at time digits position
C $9F3B Update new digit
C $9F3C Done
C $9F3C Plot a digit
C $9F45 Plot a digit
C $9F3F Next digit (second half of a pair)
C $9F45 Next digit (next pair)

@ $9F47 label=ledfont_plot
c $9F47 Plots an LED font digit
;(A = index, DE -> scr ptr)  vertical would need to be 1, 9, 17, ..
C $9F48 HL = &ledfont[A * 15]
C $9F55 save the scr ptr
C $9F56 save D
C $9F57 transfer a byte
C $9F59 next row
C $9F5A step back
C $9F5B transfer 7 rows
C $9F71 restore D
C $9F73 E += 31
C $9F77 transfer another 8 rows
C $9F96 move to next column

c $9F99

@ $9FAE label=draw_text
c $9FA3 Draws a string, bit 7 terminated
C $9FA6 Load a byte, mask off the text part
C $9FAE Was bit 7 set?, quit if so, otherwise loop

@ $9FB4 label=draw_char
c $9FB4 Draws a character
C $9FEC Point #REGhl at 8x7 font

@ $A03D label=double_height_glyph
c $A03D Plots double-height glyphs. DE->screen HL->font def

c $A0CC
B $A0CC
@ $A0D6 label=keyscan
C $A0D6 Main keyboard handler.
C $A112 Outer keyboard loop.
C $A11E Inner keyboard loop. #REGa = ?
C $A11F ...B=(A&7)+1, C=5-(A>>3)
; rotate $FE by B
; IN that port
; rotate the result by C

g $A139
B $A16E,1 used during attract mode, road gradient/angle or something?
B $A170 used in plot_scores_etc. set to 3 by $9C65.
W $A171,2 seems to be the horizon level, possibly relative (used during attract mode)
;
B $A174,1 Low/high gear flag?
B $A175,8 Score digits. One digit per byte, least significant first. This seems to be recording what's on screen so digit plotting can be bypassed.
@ $A175 label=score_digits
B $A175,8 Score. Stored as one digit per byte.
@ $A17E label=time_bcd
B $A17E,1 Time remaining. Stored as BCD.
@ $A17F label=time_digits
B $A17F,2 Time remaining. Stored as one digit per byte.
@ $A181 label=distance_digits
B $A181,4 Distance. Stored as one digit per byte.
B $A223,1 Stage number as shown. Stored as ASCII.
B $A22A,1 Used by $B6D6 and others
B $A22C,1 Bonus flag (1 triggers the effect)
B $A22D,1 Counter for bonus effect (goes up to 7?)
B $A22E,1 ... light toggle flashing related
B $A231,1 Flag set to zero when attributes have been set
B $A232,1
B $A233,1
B $A234,1 Cycles 0/1/2/3 as the game runs.
B $A235,1 ... light toggle flashing related? seems to cycle 0/1 as the game runs.
B $A240,1 Used by $B8F6, $BBF7
B $A249,1 Counter used by $BB6E
B $A24A,1 Used by $B848
B $A24E used in plot_scores_etc
B $A253,1 Low/high gear flag? or visual state?
B $A258,1 Used by $B8D8
B $A259,1 Used by $B92B
B $A256,1 Distance related perhaps?
B $A25A,1 Used by $B8D2
B $A25C,1 Used by $B84E
B $A25D,1 Used by $B85D
B $A268,1 Used by $BB69 - skips routine if not set

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
C $B4E1 Point #REGhl at left light's attributes
C $B4E4 Toggle its brightness
C $B4E7
C $B4EA
C $B4ED

c $B4F0
c $B549

c $B58E Draws the car
C $B5FE Hit while drawing car. loads values then calls $B627
C $B596 Point HL at ? then add (A * 4)
C $B605 Hit while drawing car. draws wheels
C $B610 Draws left side of car
C $B627 Subroutine
C $B641 Draws right side of car

c $B648
C $B6DE,5 Divide by 8

c $B67C

C $B701 Select a plotter? Could be clipping stuff too.
C $B708 This multiplies by six - the length of each load-mask-store step in the plotter core.
C $B70C Add that to #REGix.
C $B711 Set loop counter for 15 iterations?
C $B714

@ $B716 label=masked_tile_plotter
c $B716 Save #REGsp for restoration on exit
@ $B71C label=masked_tile_plotter_loop
C $B71F Restore original #REGsp
@ $B724 label=masked_tile_plotter_entry
C $B729 Masked tile plotter. The stack points to pairs of bitmap and mask bytes and HL must point to the screen buffer. Uses AND-OR type masking. Proceeds left-right. Doesn't flip the bytes so there must be an alternative for that.
@ $B729 label=masked_tile_plotter_core_thingy
C $B729 Load a bitmap and mask pair (D,E)
C $B72A Load the screen pixels
C $B72B AND screen pixels with mask
C $B72C OR in new pixels
C $B72D Store back to the screen
C $B72E Move to next screen pixel
C $B72F Repeat 8 times
C $B758 Handle end of row. This must be adjusting the screen pointer.

c $B76C
C $B771 Save #REGsp for restoration on exit
C $B775 Point #REGix at ??? table
C $B779 Subtract 8 ???
C $B77C,3 Multiply by 8
C $B77F Add to IX
C $B784,2 HEF
B $B79C
D $B7A4 Masked tile plotter with flipping
C $B7A4 Load a bitmap and mask pair (B,C)
C $B7A5 Set flip table index (assuming table is aligned)
C $B7A6 Load the screen pixels
C $B7A7 AND screen pixels with flipped mask
C $B7A8 Set flip table index
C $B7A9 OR in new flipped pixels
C $B7AA Store back to the screen
C $B7AB Move to next screen pixel (downwards in memory)
C $B7AC Repeat 8 times
C $B7DB Handle end of row. This must be adjusting the screen pointer.

c $B7EF
b $B828
c $B848
c $B8D2
c $B9F4
c $BB69

@ $BC3E label=draw_screen
c $BC3E Copies the backbuffer at $F000 to the screen.
C $BC3E Point #REGhl at screen.
C $BC42 Point #REGhl' at backbuffer.

c $BDC1
c $BDFB
c $C0E1
c $C15B
b $C223
c $C22A
b $C238
c $C23F
c $C2E7
c $C452
c $C57C
c $C58A
c $C598
c $C60C
c $C61D

c $C62E
;$C635 does the road plotting
C $C7EA,3 Point at first hill backdrop
C $C7EF,3 Point at second hill backdrop

c $C813
c $C821
c $C86E

@ $C8BE label=pre_shift_backdrop
c $C8BE Builds the pre-shifted first hill backdrop
; Q. How much is it shifting it by?
C $C8BE Point at second hill backdrop
C $C8C1 Point at first hill backdrop
C $C8C4
C $C8C5
C $C8C6 Each is 250 bytes long
C $C8C9 Copy 2nd to first
C $C8CB
C $C8CC Jump 10 bytes fwd
C $C8CF
C $C8D0
C $C8D1 pop old DE
C $C8D2 10 x 24
C $C8D5
C $C8D6
C $C8D7 Nibble swap thing
C $C8D9
C $C8DA
C $C8DB
C $C8DD
C $C8DE
C $C8DF
C $C8E2

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
B $DF62,150 8x15 pixels digits 0..9 only

b $DFF8 Mini font used for in-game messages
B $DFF8,174 8x6 pixels (though the digits are thinner than 8) A-Z + (probably 3 symbols)

b $E1B8

b $E2D2
b $E2D5
b $E2D9
b $E2DD
b $E2E2
b $E2E7
b $E2F3

b $E34B poked by $880A

b $E35D (roughly) spiral inward animation mask used for transitions
b $E3BD circle expanding animation mask used for transitions
b $E540 looks like a table of flipped bytes (but not quite)

b $E601
c $E839

b $E858
W $E90A,2 -> "STOP THE TAPE" + "PRESS ANY KEY" message set

c $E90F
C $E928 Point #REGhl at input menu messages
C $E931 Keyscan for 1, 2, 3, 4, 5
C $E93A Keyscan for 0, 9, 8, 7, 6
C $E93D Keyscan for P, O, I, U, Y
C $E940 Keyscan for ENTER, L, K, J, H
C $E943 Keyscan for SPACE, SYM SHFT, M, N, B

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
C $ED94,3 -> "B N M ..."
C $EDBD,3 -> test mode strings

t $EDD6 Names of keys

b $EE26
b $EE2B
b $EE30
b $EE3D
b $EE40

c $EE6E

c $EE9E Define keys
C $EEAD Self modified, cycles 5,3,2,1
C $EEBE Self modified, cycles $F12x .. $F2xx ish

c $EF19
c $EF22
c $EF29
c $EF38

b $EF5E Drum related

c $F0C6 White noise generator?
C $F0C9 loop: Point at random bytes
C $F0CC Increment first byte by 3, Load it into #REGb

b $F0FE

b $F490 Credits / Score messages
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

b $F5BC
b $F8D2
b $FAD3
b $FE34
b $FE4A
b $FE4E
b $FE64
b $FE6A
b $FE74
s $FEBE
c $FEFE
b $FF01
