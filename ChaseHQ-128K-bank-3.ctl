> $C000 ; ChaseHQ-128K-bank-3.skool
> $C000 ;
> $C000 ; Bank 3 holds the animated title screen.
> $C000 ;
> $C000
@ $C000 org
c $C000 Entry points
C $C000 Title animations
C $C003 Called from bootstrap
C $C006 Plays success? music
C $C009 Entry point for keyboard/joystick selection menu
c $C00C
c $C06E
c $C16A
c $C25D
c $C2B1
c $C2F6
b $C3AF
c $C59E
b $CC50
@ $D296 label=glyphs
b $D296 Character graphic data table
B $D296,1 Height/2
B $D297,1 Width in bytes
W $D298,2 Bitmap pointer
L $D296,4,115
b $D462 Graphics
B $D462,112,4 Bitmap: "C" 32x28
B $D4D2,112,4 Bitmap: "C" 32x28 shifted right 4?
B $D542,112,4 Bitmap: "C" 32x28 shifted right 8?
B $D5B2,140,5 Bitmap: "C" 40x28
B $D63E,112,4 Bitmap: "H" 32x28
B $D6AE,112,4 Bitmap: "H" 32x28 shifted right
B $D71E,140,5 Bitmap: "H" 40x28
B $D7AA,140,5 Bitmap: "H" 40x28 shifted right
B $D836,84,3 Bitmap: "A" 24x28
B $D88A,84,3 Bitmap: "A" 24x28 shifted right
B $D8DE,112,4 Bitmap: "A" 32x28
B $D94E,112,4 Bitmap: "A" 32x28 shifted right
B $D9BE,112,4 Bitmap: "S" 32x28
B $DA2E,112,4 Bitmap: "S" 32x28 shifted right
B $DA9E,112,4 Bitmap: "S" 32x28 shifted right again
B $DB0E,140,5 Bitmap: "S" 40x28
B $DB9A,112,5 Bitmap: "E" 32x28
B $DC0A,112,5 Bitmap: "E" 32x28 shifted right
B $DC7A,112,5 Bitmap: "E" 32x28 shifted right again
B $DCEA,112,5 Bitmap: "E" 32x28 shifted right again again
B $DD5A,128,4 Bitmap: "Q" 32x32
B $DDDA,128,4 Bitmap: "Q" 32x32 shifted right
B $DE5A,128,4 Bitmap: "Q" 32x32 shifted right again
B $DEDA,160,5 Bitmap: "Q" 40x32
B $DF7A,,2 Bitmap: "." 16x8
B $DF8A,,2 Bitmap: "." 16x8
B $DF9A,,2 Bitmap: "." 16x8
B $DFAA,,2 Bitmap: "." 16x8
B $DFBA,78,3 Bitmap: "A" 24x26
B $E008 Bitmap
B $E058 Bitmap
B $E0A0 Bitmap
B $E0DC Bitmap
B $E118 Bitmap
B $E168 Bitmap
B $E1B8 Bitmap
B $E1D4 Bitmap
B $E1F0 Bitmap
B $E20C Bitmap
B $E236 Bitmap
B $E23E Bitmap
B $E24E Bitmap
B $E25E Bitmap
B $E26E Bitmap
B $E2AA Bitmap
B $E2E6 Bitmap
B $E336 Bitmap
B $E386 Bitmap
B $E3A2 Bitmap
B $E3BE Bitmap
B $E3E8 Bitmap
B $E412 Bitmap
B $E41A Bitmap
B $E42A Bitmap
B $E43A Bitmap
B $E44A Bitmap
B $E472 Bitmap
B $E4AE Bitmap
B $E4EA Bitmap
B $E526 Bitmap
B $E542 Bitmap
B $E55E Bitmap
B $E57A Bitmap
B $E596 Bitmap
B $E59E Bitmap
B $E5A6 Bitmap
B $E5B6 Bitmap
B $E5C6 Bitmap
B $E602 Bitmap
B $E63E Bitmap
B $E68E Bitmap
B $E6DE Bitmap
B $E6FA Bitmap
B $E716 Bitmap
B $E732 Bitmap
B $E75C Bitmap
B $E764 Bitmap
B $E774 Bitmap
B $E784 Bitmap
B $E794 Bitmap
B $E7D0 Bitmap
B $E80C Bitmap
B $E848 Bitmap
B $E884 Bitmap
B $E8A0 Bitmap
B $E8BC Bitmap
B $E8D8 Bitmap
B $E902 Bitmap
B $E90A Bitmap
B $E912 Bitmap
B $E922 Bitmap
B $E932 Bitmap
B $E97A Bitmap
B $E9C2 Bitmap
B $EA0A Bitmap
B $EA6A Bitmap
B $EA8A Bitmap
B $EAAA Bitmap
B $EADA Bitmap
B $EB0A Bitmap
B $EB14 Bitmap
B $EB28 Bitmap
B $EB3C Bitmap
B $EB50 Bitmap
B $EB56 Bitmap
B $EB62 Bitmap
B $EB6E Bitmap
B $EB7A Bitmap
B $EB7E Bitmap
B $EB82 Bitmap
B $EB8A Bitmap
B $EB92 Bitmap
B $EB94 Bitmap
B $EB96 Bitmap
B $EB9A Bitmap
c $EB9E
b $EC01
c $EC71
b $EFAF
c $F7AA
c $F7C7
c $F7D6
c $F82F
c $F8EB
b $F8F2
b $F95A
c $FA3A
b $FA72
c $FB99
c $FBC8
c $FC14
b $FC29
c $FEA9
c $FF0C
c $FF2C
b $FF95
