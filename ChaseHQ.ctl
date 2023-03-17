; Chase H.Q. by Ocean/Taito. Spectrum 48K version.
;
; Strings are top bit set terminated ASCII.
; $F000 is a screen buffer. There are a column spare either side of the screen...
;

; RELATED GAMES
; -------------
; "WEC Le Mans" - earlier game by the same authors.
; https://www.mobygames.com/game/15167/wec-le-mans-24/
;
; "Batman The Movie" - same authors worked on the driving sequences for that ST/Amiga game.
;
; "Burnin' Rubber" (1990) a later Amstrad game by JOBBEEE.
; https://www.mobygames.com/game/70894/burnin-rubber/

; OTHER VERSIONS
; --------------
; Amstrad CPC version is by the same authors.
;
; Code = JOBBEEE aka John O'Brien  https://www.mobygames.com/person/28679/john-obrien/
; Graphics = BILL aka Bill Harbison
; Music = Jon Dunn
;
; http://reassembler.blogspot.com/2012/06/interview-with-spectrum-legend-bill.html

; RESEARCH
; --------
; https://www.mobygames.com/game/9832/chase-hq/
;
; https://spectrumcomputing.co.uk/entry/903/ZX-Spectrum/Chase_HQ
; Known Errors:
; Bugfix provided by Russell Marks:
; When you finish a stage, a hidden bonus is sometimes given randomly. At
; address $8b3d, instructions EX AF,AF' and LD B,A are in the wrong order.
; Therefore calculation of time bonus is using random value from A' instead of
; low digit from remaining time. Fixed with POKE 35645,71: POKE 35646,8.


@ $4000 start
@ $4000 org
b $4000 Screen memory

b $5B00 Graphics
B $5B00,260 Hill backdrop (80x26?)
B $5C04,260 Another hill backdrop (80x26?) looks similar but pre-shifted

b $5D3A
W $5D3A,2 Points at "THIS IS NANCY..."
W $5D3C,2 Points at "EMERGENCY HERE..."
W $5D3E,2 Points at "IS FLEEING ..."
W $5D40,2 Points at "VEHICLE IS..."

b $5D45 Messages
T $5D45,40 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $5D6D,40 "EMERGENCY HERE. RALPH THE IDAHO SLASHER,"
T $5D95,42 "IS FLEEING TOWARDS THE SUBURBS. THE TARGET"
T $5DBF,46 "VEHICLE IS A WHITE BRITISH SPORTS CAR... OVER."
T $5DF4,27 "OK! YOU ARE UNDER ARREST ON"
T $5E17,26 "SUSPICION OF FIRST DEGREE "
T $5E38,6 "MURDER"

b $5E3E Graphics. All are stored inverted.
B $638A,160 Perp w/ sunglasses (32x40)
B $64BC Lamborghini 48x30?
B $6570 Lamborghini small 40x20?
B $660B Truck 48x40?
B $66F5 Truck 40x30?
B $6804 Car 48x20?
B $6B88,32 Turn sign (32x32)

b $779D Pre-game screen messages
T $779D,28 "CHASE H.Q. MONITORING SYSTEM"
T $77BE,4 "TUNE"
T $77C7,6 "VOLUME"
T $77D2,6 "SIGNAL"

b $77D8
t $7812
b $7816
t $781D
b $7820
t $7825
b $7828
t $7829
b $782D
t $786E
b $7871
t $7947
b $794A
t $797F
b $7987
t $79D7
b $79DF
t $79FF
b $7A02
t $7AE5
b $7AE8
t $7B22
b $7B28
t $7B2A

b $7B2D
B $7BE9,160 Graphic: Nancy's face (32x40)
B $7C9D,160 Graphic: Raymond's face (32x40)
B $7D52,160 Graphic: Tony's face (32x40)




c $8014
c $8088
c $80B9
c $80C0
b $8112
c $814B

b $8169 Mainly tape loading messages
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
T $82AC,8 "CHASE HQ"  Note that this is double height in-game
T $82BA,18 "PRESS GEAR TO PLAY"  this blinks
T $82D4,20 "PROGRAM      JOBBEEE"  all the rest are red and appear in sequence in
T $82EF,20 "GRAPHICS        BILL"
T $830A,20 "MUSIC       JON DUNN"
T $8328,23 "(C) 1989 OCEAN SOFTWARE"
T $8346,26 "(C) 1988 TAITO CORPORATION"
T $8367,19 "ALL RIGHTS RESERVED"

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
c $88D5
c $88E2
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
b $8C58

b $8C60 End of level messages
T $8C60,22 "CLEAR BONUS      0,000"
T $8C7D,22 "TIME BONUS      X 5000"
T $8C9A,22 "SCORE                 "
T $8CBA,26 "SIGHTING OF TARGET VEHICLE"
T $8CDE,20 "OK! PULL OVER CREEP!"
T $8CFC,9 "GAME OVER"
T $8D0F,7 "TIME UP"
T $8D20,8 "CONTINUE"
T $8D30,12 "THIS MISSION"
T $8D43,16 "PUSH GEAR BUTTON"
T $8D5A,22 "BEFORE TIMER REACHES 0"
T $8D77,7 "TIME 10"
T $8D85,8 "CREDIT  "

c $8D8F
c $8DD8
c $8DF9
c $8E29
;$8E35 writes attrs to screen

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
t $9117
b $9153
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
t $9578
b $957B
t $9581
b $9584
c $9587
c $95B3
c $95D0
c $95D7
b $95DE
t $95E1
b $95E4
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
W $98AB
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

s $A0CC
b $A0CD
c $A0D6
c $A112
c $A11E
b $A139

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
t $ACDC
b $ACE3
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
T $E9B7,13 "STOP THE TAPE"
T $E9C7,25 "PRESS ANY KEY TO CONTINUE"
T $E9E4,11 "CHASE  H.Q."
T $E9F2,20 "1. SINCLAIR JOYSTICK"
T $EA09,18 "2. CURSOR JOYSTICK"
T $EA1E,20 "3. KEMPSTON JOYSTICK"
T $EA35,11 "4. KEYBOARD"
T $EA43,14 "5. DEFINE KEYS"
T $EA55,14 "REDEFINE  KEYS"
T $EA66,12 "GEAR........"
T $EA75,12 "ACCELERATE.."
T $EA84,12 "BRAKE......."
T $EA93,12 "LEFT........"
T $EAA2,12 "RIGHT......."
T $EAB1,12 "QUIT........"
T $EAC0,12 "PAUSE......."
T $EACF,12 "TURBO......."
T $EAE4,4 "TEST"
T $EAEB,24 "CHASE H.Q.     TEST MODE"  ooh!
T $EB06,10 "IN GAME..."
T $EB13,29 "PRESS 1....... RESTART LEVEL."
T $EB33,20 "2....... NEXT LEVEL."
T $EB4A,20 "3....... END SCREEN."
T $EB61,22 "4....... EXTRA CREDIT."
T $EB7B,11 "CHASE  H.Q."
T $EB89,27 "PLEASE NOTE CONTROL OPTIONS"
T $EBA7,21 "CANNOT BE REMODIFIED."
T $EBBF,31 "ARE YOU HAPPY WITH YOUR CHOICE."
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
