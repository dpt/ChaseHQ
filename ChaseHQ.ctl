; Chase H.Q. by Ocean/Taito. Spectrum 48K version.
;
; Strings are top bit set terminated ASCII.
; $F000 is a screen buffer. There are a column spare either side of the screen...
;

; AUTHORS
; -------
; Code = JOBBEEE aka John O'Brien  https://www.mobygames.com/person/28679/john-obrien/
; Graphics = BILL aka Bill Harbison
; Music = JON DUNN aka Jonathan Dunn

; OTHER VERSIONS
; --------------
; The Amstrad CPC version is by the same authors.

; RELATED GAMES
; -------------
; "WEC Le Mans" - earlier game by the same authors.
; https://www.mobygames.com/game/15167/wec-le-mans-24/
;
; "Batman The Movie" - same authors worked on the driving sequences for that ST/Amiga game.
;
; "Burnin' Rubber" (1990) a later Amstrad game by JOBBEEE.
; https://www.mobygames.com/game/70894/burnin-rubber/

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

; SECRETS
; -------
; Redefine keys to "SHOCKED<ENTER>" to activate test/cheat mode.


; THINGS TO LOCATE
; ----------------
; Considering this is just level 1 in 48K:
; - input handlers
; - music data and player
; - most of car sprites
; - car smoke sprites
; - lamp post sprites
; - tree sprites
; - car's gravity / jump handling
; - sound effects
; - tunnel handling
; - route/map
; - road drawing code
; - road split handling
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
; - level format
; - helicopter (later level)
; - flashing light on car
; - ok creep! auto driving
; - Chase HQ monitoring system

@ $4000 start
@ $4000 org
b $4000 Screen memory

b $5B00 Graphics
B $5B00,260 Hill backdrop (80x26?)
B $5C04,260 Another hill backdrop (80x26?) looks similar but pre-shifted

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
W $5DF1,2 Backbuffer address
W $5DF3,2 Attribute address
T $5DF5,27 "OK! YOU ARE UNDER ARREST ON"
;
B $5E10,1 frame delay until next message?
B $5E11,1 Flags (?)
B $5E12,1 Attribute (black)
W $5E13,2 Backbuffer address
W $5E15,2 Attribute address
T $5E17,26 "SUSPICION OF FIRST DEGREE "
;
B $5E31,1 frame delay until next message?
B $5E32,1 Flags (?)
B $5E33,1 Attribute (black)
W $5E34,2 Backbuffer address
W $5E36,2 Attribute address
T $5E38,6 "MURDER"

b $5E3E Graphics. All are stored inverted except where noted.
;
W $5E41,2 Pointer to tumbleweed LODs table
W $5E44,2 Pointer to another LOD table

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
B $6B68,160 Turn sign (32x40)
B $6C08,90 Turn sign (24x30)
B $6C62,40 Turn sign (16x20)
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



L $6D82,7,6
@ $7798 label=pre_game_messages
b $7798 Pre-game screen messages
;
B $7798,1 flags? attrs?   text is white
W $7799,2 Backbuffer address
W $779B,2 Attributes address
T $779D,28 "CHASE H.Q. MONITORING SYSTEM"   text is white
;
B $77B9,1 flags? attrs?   text is yellow
W $77BA,2 Backbuffer address
W $77BC,2 Attributes address
T $77BE,4 "TUNE"
;
B $77C2,1 flags? attrs?   text is yellow
W $77C3,2 Backbuffer address
W $77C5,2 Attributes address
T $77C7,6 "VOLUME"
;
B $77CD,1 flags? attrs?   text is white
W $77CE,2 Backbuffer address
W $77D0,2 Attributes address
T $77D2,6 "SIGNAL"

b $77D8
B $78B6,64 8 tiles used for borders on the pregame screen

B $7BE9,160 Graphic: Nancy's face (32x40). Stored top-down.
B $7C89 unknown
B $7C9D,160 Graphic: Raymond's face (32x40). Stored top-down.
B $7D3D unknown
B $7D52,160 Graphic: Tony's face (32x40). Stored top-down.
B $7DF2 unknown

c $8014
c $8088
c $80B9
c $80C0
c $814B Tape level loader

b $8169 Mainly tape loading messages
B $8169,1 flags? attrs?
W $816A,2 Backbuffer address
W $816C,2 Attribute address
T $816E,30 "REWIND TAPE TO START OF SIDE 2"
T $8191,10 "START TAPE"
T $81A0,15 "SEARCHING FOR 1"
T $81B4,7 "FOUND 1"
T $81C0,13 "STOP THE TAPE"
T $81D2,11 "PRESS  GEAR"
T $81EC,13 "GIDDY UP BOY!"
T $81F9,11 "HOLD ON MAN"

c $8204
c $8258

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
W $82A8,2 Backbuffer address
W $82AA,2 Attribute address
T $82AC,8 "CHASE HQ"
;
B $82B4,1 Flags (regular)
B $82B5,1 Attribute (black)
W $82B6,2 Backbuffer address
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
W $82D0,2 Backbuffer address
W $82D2,2 Attribute address
T $82D4,20 "PROGRAM      JOBBEEE"
;
B $82E8,1 Frame delay?
;
B $82E9,1 Flags (regular)
B $82EA,1 Attribute (red)
W $82EB,2 Backbuffer address
W $82ED,2 Attribute address
T $82EF,20 "GRAPHICS        BILL"
;
B $8303,1 Delay + stop flags?  $28
;
B $8304,1 Flags (regular)
B $8305,1 Attribute (red)
W $8306,2 Backbuffer address
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
W $8324,2 Backbuffer address
W $8326,2 Attribute address
T $8328,23 "(C) 1989 OCEAN SOFTWARE"
;
B $833F Frame delay?
;
B $8340,1 Flags (regular)
B $8341,1 Attribute (red)
W $8342,2 Backbuffer address
W $8344,2 Attribute address
T $8346,26 "(C) 1988 TAITO CORPORATION"
;
B $8360 Delay + stop flags?  $28
;
B $8361,1 Flags (regular)
B $8362,1 Attribute (red)
W $8363,2 Backbuffer address
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

c $8860 this clears the flashing lights
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
c $8A0F
c $8A36
c $8A57
c $8C3A

b $8C58 End of level messages
B $8C5B,1 Attribute (red)
W $8C5C,2 Backbuffer address
W $8C5E,2 Attribute address
T $8C60,22 "CLEAR BONUS      0,000" -- double height
;
B $8C78,1 Attribute (red)
W $8C79,2 Backbuffer address
W $8C7B,2 Attribute address
T $8C7D,22 "TIME BONUS      X 5000" -- double height
;
B $8C95,1 Attribute (red)
W $8C96,2 Backbuffer address
W $8C98,2 Attribute address
T $8C9A,22 "SCORE                 " -- double height
;
B $8CB5,1 Attribute (black)
W $8CB6,2 Backbuffer address
W $8CB8,2 Attribute address
T $8CBA,26 "SIGHTING OF TARGET VEHICLE"
;
B $8CD9,1 Attribute (black)
W $8CDA,2 Backbuffer address
W $8CDC,2 Attribute address
T $8CDE,20 "OK! PULL OVER CREEP!"
;
B $8CF7,1 Attribute (black)
W $8CF8,2 Backbuffer address
W $8CFA,2 Attribute address
T $8CFC,9 "GAME OVER"
;
B $8D0A,1 Attribute (black)
W $8D0B,2 Backbuffer address
W $8D0D,2 Attribute address
T $8D0F,7 "TIME UP"
;
B $8D1B,1 Attribute (black)
W $8D1C,2 Backbuffer address
W $8D1E,2 Attribute address
T $8D20,9 "CONTINUE "
;
B $8D2B,1 Attribute (black)
W $8D2C,2 Backbuffer address
W $8D2E,2 Attribute address
T $8D30,12 "THIS MISSION"
;
B $8D3E,1 Attribute (red)
W $8D3F,2 Backbuffer address
W $8D41,2 Attribute address
T $8D43,16 "PUSH GEAR BUTTON"
;
B $8D55,1 Attribute (red)
W $8D56,2 Backbuffer address
W $8D58,2 Attribute address
T $8D5A,22 "BEFORE TIMER REACHES 0"
;
B $8D72,1 Attribute (black)
W $8D73,2 Backbuffer address
W $8D75,2 Attribute address
T $8D77,7 "TIME 10"
;
B $8D80,1 Attribute (black)
W $8D81,2 Backbuffer address
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
c $94BB
c $94C2
c $94C8
c $94F2
c $950D
c $9514
c $951A
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
b $9618
c $961B
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
c $9CC2
c $9CD6
c $9CF8
c $9D2E
t $9D51
b $9D56
t $9D5B
b $9D61
c $9D62
c $9DF4
c $9E11
c $9F47
c $9F99
c $9FA3
c $9FB4

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


b $A139

b $A231 flag set to zero when attributes have been set

b $A27A Font: 8x7 bitmap of symbols(!(),.) then 0-9A-Z

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
c $B4F0
c $B549
c $B58E
c $B648
c $B67C
c $B71C
c $B723
c $B729
c $B76C
c $B78B
c $B795
b $B79C
c $B7A4
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


c $C813
c $C821
c $C86E
c $C8BE
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

b $D20E Smoke plume. Perhaps 64x64 in total, but includes masks.
b $DD6A Components of the car graphic or its shadow?. Straight on. Byte pairs of value and mask. three sets of 14 x 8 UDGs.
b $DE12 Same, but turning.
b $DEBA Same again, but turning hard.

b $DF62 LED numeric font used for scores
B $DF62,150 8x15 pixels digits 0..9 only

b $DFF8 Mini font used for in-game messages
B $DFF8,174 8x6 pixels (though the digits are thinner than 8) A-Z + (probably 3 symbols)

b $E1B8

b $E35D (roughly) spiral inward animation mask used for transitions
b $E3BD circle expanding animation mask used for transitions
b $E540 looks like a table of flipped bytes (but not quite)

b $E601
c $E839
b $E858
c $E90F

b $E9B4 Messages
;
B $E9B4,1 flag/attr?
W $E9B5,2 Screen position 80,96
T $E9B7,13 "STOP THE TAPE"
;
T $E9C7,25 "PRESS ANY KEY TO CONTINUE"
;
T $E9E4,11 "CHASE  H.Q."
;
T $E9F2,20 "1. SINCLAIR JOYSTICK"
;
T $EA09,18 "2. CURSOR JOYSTICK"
;
T $EA1E,20 "3. KEMPSTON JOYSTICK"
;
T $EA35,11 "4. KEYBOARD"
;
T $EA43,14 "5. DEFINE KEYS"
;
T $EA55,14 "REDEFINE  KEYS"
;
T $EA66,12 "GEAR........"
;
T $EA75,12 "ACCELERATE.."
;
T $EA84,12 "BRAKE......."
;
T $EA93,12 "LEFT........"
;
T $EAA2,12 "RIGHT......."
;
T $EAB1,12 "QUIT........"
;
T $EAC0,12 "PAUSE......."
;
T $EACF,12 "TURBO......."
;
T $EAE4,4 "TEST"
;
T $EAEB,24 "CHASE H.Q.     TEST MODE"
;
T $EB06,10 "IN GAME..."
;
T $EB13,29 "PRESS 1....... RESTART LEVEL."
;
T $EB33,20 "2....... NEXT LEVEL."
;
T $EB4A,20 "3....... END SCREEN."
;
T $EB61,22 "4....... EXTRA CREDIT."
;
T $EB7B,11 "CHASE  H.Q."
;
T $EB89,27 "PLEASE NOTE CONTROL OPTIONS"
;
T $EBA7,21 "CANNOT BE REMODIFIED."
;
T $EBBF,31 "ARE YOU HAPPY WITH YOUR CHOICE."
;
T $EBE1,21 "PRESS YES(Y) OR NO(N)"

c $EBF7
c $EBFF
c $EC2C

@ $ECDA label=clear_screen
c $ECDA Clears the screen.

c $ECF3
b $ED4D
t $EDD6
b $EE26
t $EE3D
b $EE40
c $EE6E
c $EE9E
c $EF19
c $EF22
c $EF29
c $EF38
b $EF5E
c $F0C6
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
