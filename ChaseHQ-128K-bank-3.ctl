> $C000 ; ChaseHQ-128K-bank-3.skool
> $C000 ;
> $C000 ; Bank 3 holds the animated title screen.
> $C000 ;
> $C000
@ $C000 org
c $C000 Entry points
C $C000,3 Title animations
C $C003,3 Called from bootstrap
C $C006,3 Plays success music
C $C009,3 Entry point for keyboard/joystick selection menu
c $C00C Routine at C00C
D $C00C Used by the routines at #R$C000 and #R$C59E.
c $C06E Routine at C06E
D $C06E Used by the routine at #R$C00C.
N $C155 This entry point is used by the routine at #R$C16A.
c $C16A Routine at C16A
D $C16A Used by the routine at #R$C06E.
c $C25D Routine at C25D
D $C25D Used by the routine at #R$C16A.
N $C274 This entry point is used by the routine at #R$C16A.
N $C275 This entry point is used by the routine at #R$C16A.
N $C278 This entry point is used by the routine at #R$C16A.
N $C291 This entry point is used by the routine at #R$C16A.
c $C2B1 Routine at C2B1
D $C2B1 Used by the routine at #R$C06E.
N $C2F2 This entry point is used by the routine at #R$C2F6.
c $C2F6 Routine at C2F6
D $C2F6 Used by the routine at #R$C2B1.
N $C2FB This entry point is used by the routine at #R$C06E.
b $C3AF Data block at C3AF
B $C3AF,495,8*61,7
c $C59E Routine at C59E
D $C59E Used by the routines at #R$C000 and #R$FBC8.
N $C890 This entry point is used by the routine at #R$C06E.
b $CC50 Data block at CC50
B $CC50,1606,8*200,6
b $D296 Character graphic data table
@ $D296 label=glyphs
B $D296,1,1 Height/2
B $D297,1,1 Width in bytes
W $D298,2,2 Bitmap pointer
B $D29A,1,1 Height/2
B $D29B,1,1 Width in bytes
W $D29C,2,2 Bitmap pointer
B $D29E,1,1 Height/2
B $D29F,1,1 Width in bytes
W $D2A0,2,2 Bitmap pointer
B $D2A2,1,1 Height/2
B $D2A3,1,1 Width in bytes
W $D2A4,2,2 Bitmap pointer
B $D2A6,1,1 Height/2
B $D2A7,1,1 Width in bytes
W $D2A8,2,2 Bitmap pointer
B $D2AA,1,1 Height/2
B $D2AB,1,1 Width in bytes
W $D2AC,2,2 Bitmap pointer
B $D2AE,1,1 Height/2
B $D2AF,1,1 Width in bytes
W $D2B0,2,2 Bitmap pointer
B $D2B2,1,1 Height/2
B $D2B3,1,1 Width in bytes
W $D2B4,2,2 Bitmap pointer
B $D2B6,1,1 Height/2
B $D2B7,1,1 Width in bytes
W $D2B8,2,2 Bitmap pointer
B $D2BA,1,1 Height/2
B $D2BB,1,1 Width in bytes
W $D2BC,2,2 Bitmap pointer
B $D2BE,1,1 Height/2
B $D2BF,1,1 Width in bytes
W $D2C0,2,2 Bitmap pointer
B $D2C2,1,1 Height/2
B $D2C3,1,1 Width in bytes
W $D2C4,2,2 Bitmap pointer
B $D2C6,1,1 Height/2
B $D2C7,1,1 Width in bytes
W $D2C8,2,2 Bitmap pointer
B $D2CA,1,1 Height/2
B $D2CB,1,1 Width in bytes
W $D2CC,2,2 Bitmap pointer
B $D2CE,1,1 Height/2
B $D2CF,1,1 Width in bytes
W $D2D0,2,2 Bitmap pointer
B $D2D2,1,1 Height/2
B $D2D3,1,1 Width in bytes
W $D2D4,2,2 Bitmap pointer
B $D2D6,1,1 Height/2
B $D2D7,1,1 Width in bytes
W $D2D8,2,2 Bitmap pointer
B $D2DA,1,1 Height/2
B $D2DB,1,1 Width in bytes
W $D2DC,2,2 Bitmap pointer
B $D2DE,1,1 Height/2
B $D2DF,1,1 Width in bytes
W $D2E0,2,2 Bitmap pointer
B $D2E2,1,1 Height/2
B $D2E3,1,1 Width in bytes
W $D2E4,2,2 Bitmap pointer
B $D2E6,1,1 Height/2
B $D2E7,1,1 Width in bytes
W $D2E8,2,2 Bitmap pointer
B $D2EA,1,1 Height/2
B $D2EB,1,1 Width in bytes
W $D2EC,2,2 Bitmap pointer
B $D2EE,1,1 Height/2
B $D2EF,1,1 Width in bytes
W $D2F0,2,2 Bitmap pointer
B $D2F2,1,1 Height/2
B $D2F3,1,1 Width in bytes
W $D2F4,2,2 Bitmap pointer
B $D2F6,1,1 Height/2
B $D2F7,1,1 Width in bytes
W $D2F8,2,2 Bitmap pointer
B $D2FA,1,1 Height/2
B $D2FB,1,1 Width in bytes
W $D2FC,2,2 Bitmap pointer
B $D2FE,1,1 Height/2
B $D2FF,1,1 Width in bytes
W $D300,2,2 Bitmap pointer
B $D302,1,1 Height/2
B $D303,1,1 Width in bytes
W $D304,2,2 Bitmap pointer
B $D306,1,1 Height/2
B $D307,1,1 Width in bytes
W $D308,2,2 Bitmap pointer
B $D30A,1,1 Height/2
B $D30B,1,1 Width in bytes
W $D30C,2,2 Bitmap pointer
B $D30E,1,1 Height/2
B $D30F,1,1 Width in bytes
W $D310,2,2 Bitmap pointer
B $D312,1,1 Height/2
B $D313,1,1 Width in bytes
W $D314,2,2 Bitmap pointer
B $D316,1,1 Height/2
B $D317,1,1 Width in bytes
W $D318,2,2 Bitmap pointer
B $D31A,1,1 Height/2
B $D31B,1,1 Width in bytes
W $D31C,2,2 Bitmap pointer
B $D31E,1,1 Height/2
B $D31F,1,1 Width in bytes
W $D320,2,2 Bitmap pointer
B $D322,1,1 Height/2
B $D323,1,1 Width in bytes
W $D324,2,2 Bitmap pointer
B $D326,1,1 Height/2
B $D327,1,1 Width in bytes
W $D328,2,2 Bitmap pointer
B $D32A,1,1 Height/2
B $D32B,1,1 Width in bytes
W $D32C,2,2 Bitmap pointer
B $D32E,1,1 Height/2
B $D32F,1,1 Width in bytes
W $D330,2,2 Bitmap pointer
B $D332,1,1 Height/2
B $D333,1,1 Width in bytes
W $D334,2,2 Bitmap pointer
B $D336,1,1 Height/2
B $D337,1,1 Width in bytes
W $D338,2,2 Bitmap pointer
B $D33A,1,1 Height/2
B $D33B,1,1 Width in bytes
W $D33C,2,2 Bitmap pointer
B $D33E,1,1 Height/2
B $D33F,1,1 Width in bytes
W $D340,2,2 Bitmap pointer
B $D342,1,1 Height/2
B $D343,1,1 Width in bytes
W $D344,2,2 Bitmap pointer
B $D346,1,1 Height/2
B $D347,1,1 Width in bytes
W $D348,2,2 Bitmap pointer
B $D34A,1,1 Height/2
B $D34B,1,1 Width in bytes
W $D34C,2,2 Bitmap pointer
B $D34E,1,1 Height/2
B $D34F,1,1 Width in bytes
W $D350,2,2 Bitmap pointer
B $D352,1,1 Height/2
B $D353,1,1 Width in bytes
W $D354,2,2 Bitmap pointer
B $D356,1,1 Height/2
B $D357,1,1 Width in bytes
W $D358,2,2 Bitmap pointer
B $D35A,1,1 Height/2
B $D35B,1,1 Width in bytes
W $D35C,2,2 Bitmap pointer
B $D35E,1,1 Height/2
B $D35F,1,1 Width in bytes
W $D360,2,2 Bitmap pointer
B $D362,1,1 Height/2
B $D363,1,1 Width in bytes
W $D364,2,2 Bitmap pointer
B $D366,1,1 Height/2
B $D367,1,1 Width in bytes
W $D368,2,2 Bitmap pointer
B $D36A,1,1 Height/2
B $D36B,1,1 Width in bytes
W $D36C,2,2 Bitmap pointer
B $D36E,1,1 Height/2
B $D36F,1,1 Width in bytes
W $D370,2,2 Bitmap pointer
B $D372,1,1 Height/2
B $D373,1,1 Width in bytes
W $D374,2,2 Bitmap pointer
B $D376,1,1 Height/2
B $D377,1,1 Width in bytes
W $D378,2,2 Bitmap pointer
B $D37A,1,1 Height/2
B $D37B,1,1 Width in bytes
W $D37C,2,2 Bitmap pointer
B $D37E,1,1 Height/2
B $D37F,1,1 Width in bytes
W $D380,2,2 Bitmap pointer
B $D382,1,1 Height/2
B $D383,1,1 Width in bytes
W $D384,2,2 Bitmap pointer
B $D386,1,1 Height/2
B $D387,1,1 Width in bytes
W $D388,2,2 Bitmap pointer
B $D38A,1,1 Height/2
B $D38B,1,1 Width in bytes
W $D38C,2,2 Bitmap pointer
B $D38E,1,1 Height/2
B $D38F,1,1 Width in bytes
W $D390,2,2 Bitmap pointer
B $D392,1,1 Height/2
B $D393,1,1 Width in bytes
W $D394,2,2 Bitmap pointer
B $D396,1,1 Height/2
B $D397,1,1 Width in bytes
W $D398,2,2 Bitmap pointer
B $D39A,1,1 Height/2
B $D39B,1,1 Width in bytes
W $D39C,2,2 Bitmap pointer
B $D39E,1,1 Height/2
B $D39F,1,1 Width in bytes
W $D3A0,2,2 Bitmap pointer
B $D3A2,1,1 Height/2
B $D3A3,1,1 Width in bytes
W $D3A4,2,2 Bitmap pointer
B $D3A6,1,1 Height/2
B $D3A7,1,1 Width in bytes
W $D3A8,2,2 Bitmap pointer
B $D3AA,1,1 Height/2
B $D3AB,1,1 Width in bytes
W $D3AC,2,2 Bitmap pointer
B $D3AE,1,1 Height/2
B $D3AF,1,1 Width in bytes
W $D3B0,2,2 Bitmap pointer
B $D3B2,1,1 Height/2
B $D3B3,1,1 Width in bytes
W $D3B4,2,2 Bitmap pointer
B $D3B6,1,1 Height/2
B $D3B7,1,1 Width in bytes
W $D3B8,2,2 Bitmap pointer
B $D3BA,1,1 Height/2
B $D3BB,1,1 Width in bytes
W $D3BC,2,2 Bitmap pointer
B $D3BE,1,1 Height/2
B $D3BF,1,1 Width in bytes
W $D3C0,2,2 Bitmap pointer
B $D3C2,1,1 Height/2
B $D3C3,1,1 Width in bytes
W $D3C4,2,2 Bitmap pointer
B $D3C6,1,1 Height/2
B $D3C7,1,1 Width in bytes
W $D3C8,2,2 Bitmap pointer
B $D3CA,1,1 Height/2
B $D3CB,1,1 Width in bytes
W $D3CC,2,2 Bitmap pointer
B $D3CE,1,1 Height/2
B $D3CF,1,1 Width in bytes
W $D3D0,2,2 Bitmap pointer
B $D3D2,1,1 Height/2
B $D3D3,1,1 Width in bytes
W $D3D4,2,2 Bitmap pointer
B $D3D6,1,1 Height/2
B $D3D7,1,1 Width in bytes
W $D3D8,2,2 Bitmap pointer
B $D3DA,1,1 Height/2
B $D3DB,1,1 Width in bytes
W $D3DC,2,2 Bitmap pointer
B $D3DE,1,1 Height/2
B $D3DF,1,1 Width in bytes
W $D3E0,2,2 Bitmap pointer
B $D3E2,1,1 Height/2
B $D3E3,1,1 Width in bytes
W $D3E4,2,2 Bitmap pointer
B $D3E6,1,1 Height/2
B $D3E7,1,1 Width in bytes
W $D3E8,2,2 Bitmap pointer
B $D3EA,1,1 Height/2
B $D3EB,1,1 Width in bytes
W $D3EC,2,2 Bitmap pointer
B $D3EE,1,1 Height/2
B $D3EF,1,1 Width in bytes
W $D3F0,2,2 Bitmap pointer
B $D3F2,1,1 Height/2
B $D3F3,1,1 Width in bytes
W $D3F4,2,2 Bitmap pointer
B $D3F6,1,1 Height/2
B $D3F7,1,1 Width in bytes
W $D3F8,2,2 Bitmap pointer
B $D3FA,1,1 Height/2
B $D3FB,1,1 Width in bytes
W $D3FC,2,2 Bitmap pointer
B $D3FE,1,1 Height/2
B $D3FF,1,1 Width in bytes
W $D400,2,2 Bitmap pointer
B $D402,1,1 Height/2
B $D403,1,1 Width in bytes
W $D404,2,2 Bitmap pointer
B $D406,1,1 Height/2
B $D407,1,1 Width in bytes
W $D408,2,2 Bitmap pointer
B $D40A,1,1 Height/2
B $D40B,1,1 Width in bytes
W $D40C,2,2 Bitmap pointer
B $D40E,1,1 Height/2
B $D40F,1,1 Width in bytes
W $D410,2,2 Bitmap pointer
B $D412,1,1 Height/2
B $D413,1,1 Width in bytes
W $D414,2,2 Bitmap pointer
B $D416,1,1 Height/2
B $D417,1,1 Width in bytes
W $D418,2,2 Bitmap pointer
B $D41A,1,1 Height/2
B $D41B,1,1 Width in bytes
W $D41C,2,2 Bitmap pointer
B $D41E,1,1 Height/2
B $D41F,1,1 Width in bytes
W $D420,2,2 Bitmap pointer
B $D422,1,1 Height/2
B $D423,1,1 Width in bytes
W $D424,2,2 Bitmap pointer
B $D426,1,1 Height/2
B $D427,1,1 Width in bytes
W $D428,2,2 Bitmap pointer
B $D42A,1,1 Height/2
B $D42B,1,1 Width in bytes
W $D42C,2,2 Bitmap pointer
B $D42E,1,1 Height/2
B $D42F,1,1 Width in bytes
W $D430,2,2 Bitmap pointer
B $D432,1,1 Height/2
B $D433,1,1 Width in bytes
W $D434,2,2 Bitmap pointer
B $D436,1,1 Height/2
B $D437,1,1 Width in bytes
W $D438,2,2 Bitmap pointer
B $D43A,1,1 Height/2
B $D43B,1,1 Width in bytes
W $D43C,2,2 Bitmap pointer
B $D43E,1,1 Height/2
B $D43F,1,1 Width in bytes
W $D440,2,2 Bitmap pointer
B $D442,1,1 Height/2
B $D443,1,1 Width in bytes
W $D444,2,2 Bitmap pointer
B $D446,1,1 Height/2
B $D447,1,1 Width in bytes
W $D448,2,2 Bitmap pointer
B $D44A,1,1 Height/2
B $D44B,1,1 Width in bytes
W $D44C,2,2 Bitmap pointer
B $D44E,1,1 Height/2
B $D44F,1,1 Width in bytes
W $D450,2,2 Bitmap pointer
B $D452,1,1 Height/2
B $D453,1,1 Width in bytes
W $D454,2,2 Bitmap pointer
B $D456,1,1 Height/2
B $D457,1,1 Width in bytes
W $D458,2,2 Bitmap pointer
B $D45A,1,1 Height/2
B $D45B,1,1 Width in bytes
W $D45C,2,2 Bitmap pointer
B $D45E,1,1 Height/2
B $D45F,1,1 Width in bytes
W $D460,2,2 Bitmap pointer
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
B $DB9A,112,5*22,2 Bitmap: "E" 32x28
B $DC0A,112,5*22,2 Bitmap: "E" 32x28 shifted right
B $DC7A,112,5*22,2 Bitmap: "E" 32x28 shifted right again
B $DCEA,112,5*22,2 Bitmap: "E" 32x28 shifted right again again
B $DD5A,128,4 Bitmap: "Q" 32x32
B $DDDA,128,4 Bitmap: "Q" 32x32 shifted right
B $DE5A,128,4 Bitmap: "Q" 32x32 shifted right again
B $DEDA,160,5 Bitmap: "Q" 40x32
B $DF7A,16,2 Bitmap: "." 16x8
B $DF8A,16,2 Bitmap: "." 16x8
B $DF9A,16,2 Bitmap: "." 16x8
B $DFAA,16,2 Bitmap: "." 16x8
B $DFBA,78,3 Bitmap: "A" 24x26
B $E008,80,8 Bitmap
B $E058,72,8 Bitmap
B $E0A0,60,8*7,4 Bitmap
B $E0DC,60,8*7,4 Bitmap
B $E118,80,8 Bitmap
B $E168,80,8 Bitmap
B $E1B8,28,8*3,4 Bitmap
B $E1D4,28,8*3,4 Bitmap
B $E1F0,28,8*3,4 Bitmap
B $E20C,42,8*5,2 Bitmap
B $E236,8,8 Bitmap
B $E23E,16,8 Bitmap
B $E24E,16,8 Bitmap
B $E25E,16,8 Bitmap
B $E26E,60,8*7,4 Bitmap
B $E2AA,60,8*7,4 Bitmap
B $E2E6,80,8 Bitmap
B $E336,80,8 Bitmap
B $E386,28,8*3,4 Bitmap
B $E3A2,28,8*3,4 Bitmap
B $E3BE,42,8*5,2 Bitmap
B $E3E8,42,8*5,2 Bitmap
B $E412,8,8 Bitmap
B $E41A,16,8 Bitmap
B $E42A,16,8 Bitmap
B $E43A,16,8 Bitmap
B $E44A,40,8 Bitmap
B $E472,60,8*7,4 Bitmap
B $E4AE,60,8*7,4 Bitmap
B $E4EA,60,8*7,4 Bitmap
B $E526,28,8*3,4 Bitmap
B $E542,28,8*3,4 Bitmap
B $E55E,28,8*3,4 Bitmap
B $E57A,28,8*3,4 Bitmap
B $E596,8,8 Bitmap
B $E59E,8,8 Bitmap
B $E5A6,16,8 Bitmap
B $E5B6,16,8 Bitmap
B $E5C6,60,8*7,4 Bitmap
B $E602,60,8*7,4 Bitmap
B $E63E,80,8 Bitmap
B $E68E,80,8 Bitmap
B $E6DE,28,8*3,4 Bitmap
B $E6FA,28,8*3,4 Bitmap
B $E716,28,8*3,4 Bitmap
B $E732,42,8*5,2 Bitmap
B $E75C,8,8 Bitmap
B $E764,16,8 Bitmap
B $E774,16,8 Bitmap
B $E784,16,8 Bitmap
B $E794,60,8*7,4 Bitmap
B $E7D0,60,8*7,4 Bitmap
B $E80C,60,8*7,4 Bitmap
B $E848,60,8*7,4 Bitmap
B $E884,28,8*3,4 Bitmap
B $E8A0,28,8*3,4 Bitmap
B $E8BC,28,8*3,4 Bitmap
B $E8D8,42,8*5,2 Bitmap
B $E902,8,8 Bitmap
B $E90A,8,8 Bitmap
B $E912,16,8 Bitmap
B $E922,16,8 Bitmap
B $E932,72,8 Bitmap
B $E97A,72,8 Bitmap
B $E9C2,72,8 Bitmap
B $EA0A,96,8 Bitmap
B $EA6A,32,8 Bitmap
B $EA8A,32,8 Bitmap
B $EAAA,48,8 Bitmap
B $EADA,48,8 Bitmap
B $EB0A,10,8,2 Bitmap
B $EB14,20,8*2,4 Bitmap
B $EB28,20,8*2,4 Bitmap
B $EB3C,20,8*2,4 Bitmap
B $EB50,6,6 Bitmap
B $EB56,12,8,4 Bitmap
B $EB62,12,8,4 Bitmap
B $EB6E,12,8,4 Bitmap
B $EB7A,4,4 Bitmap
B $EB7E,4,4 Bitmap
B $EB82,8,8 Bitmap
B $EB8A,8,8 Bitmap
B $EB92,2,2 Bitmap
B $EB94,2,2 Bitmap
B $EB96,4,4 Bitmap
B $EB9A,4,4 Bitmap
c $EB9E Routine at EB9E
D $EB9E Used by the routine at #R$F7D6.
b $EC01 Data block at EC01
B $EC01,112,8
c $EC71 Routine at EC71
D $EC71 Used by the routine at #R$F82F.
N $ED0B This entry point is used by the routines at #R$C16A, #R$C59E, #R$F7D6 and #R$FBC8.
b $EFAF Data block at EFAF
B $EFAF,2043,8*255,3
c $F7AA Routine at F7AA
D $F7AA Used by the routines at #R$C06E, #R$C59E, #R$F7C7 and #R$FB99.
c $F7C7 Routine at F7C7
D $F7C7 Used by the routine at #R$C000.
c $F7D6 Routine at F7D6
D $F7D6 Used by the routines at #R$C06E, #R$C16A, #R$C59E, #R$F7C7, #R$FB99 and #R$FBC8.
N $F7DB This entry point is used by the routine at #R$C59E.
N $F7F4 This entry point is used by the routine at #R$F82F.
c $F82F Routine at F82F
D $F82F Used by the routines at #R$C06E, #R$C16A, #R$C59E, #R$F7C7 and #R$FBC8.
N $F8A7 This entry point is used by the routines at #R$F8EB and #R$FA3A.
c $F8EB Routine at F8EB
D $F8EB Used by the routine at #R$F82F.
b $F8F2 Data block at F8F2
B $F8F2,104,8
b $F95A Data block at F95A
B $F95A,224,8
c $FA3A Routine at FA3A
D $FA3A Used by the routine at #R$F82F.
b $FA72 Data block at FA72
B $FA72,295,8*36,7
c $FB99 Routine at FB99
D $FB99 Used by the routine at #R$C000.
N $FBA2 This entry point is used by the routine at #R$C59E.
N $FBAB This entry point is used by the routine at #R$FC14.
c $FBC8 Routine at FBC8
D $FBC8 Used by the routines at #R$FB99, #R$FC14, #R$FEA9 and #R$FF2C.
N $FBD4 This entry point is used by the routine at #R$FB99.
N $FBD9 This entry point is used by the routine at #R$FB99.
N $FBE4 This entry point is used by the routine at #R$FB99.
N $FBE5 This entry point is used by the routine at #R$FC14.
c $FC14 Routine at FC14
D $FC14 Used by the routine at #R$FB99.
b $FC29 Data block at FC29
B $FC29,640,8
c $FEA9 Routine at FEA9
D $FEA9 Used by the routine at #R$FB99.
c $FF0C Routine at FF0C
D $FF0C Used by the routine at #R$FF2C.
c $FF2C Routine at FF2C
D $FF2C Used by the routine at #R$FEA9.
b $FF95 Data block at FF95
B $FF95,107,8*13,3
