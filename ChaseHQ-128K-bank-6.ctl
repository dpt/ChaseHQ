> $C000 ; ChaseHQ-128K-bank-6.skool
> $C000 ;
> $C000 ; Bank 6 holds the data for stages 3 and 4.
> $C000 ;
> $C000
@ $C000 org
b $C000 [Stage 3] Horizon graphic
b $C0F0 [Stage 3] Per-stage data
W $C0F0,2 [$C8CC] Address of perp's mugshot attributes
W $C0F2,2 [$6400] Address of perp's mugshot bitmap
W $C0F4,2 [$D470] Screen attributes used for the ground colour (a pair of matching bytes)
W $C0F6,2 [$C22E] Address of table of LODs for tumbleweeds, barriers.
W $C0F8,2 [$C230] (points at a handler address)
W $C0FA,2 [$C22D] Address of right hand graphics entry/entries (-7 bytes)
W $C0FC,2 [$C242] (points at a handler address)
W $C0FE,2 [$C25A] (points at a handler address)
W $C100,2 [$C257] Address of left hand graphics entry/entries (-7 bytes)
W $C102,2 [$C26C] (points at a handler address)
W $C104,2 [$C139] Address of Nancy's perp description
W $C106,2 [$C1E8] Address of arrest messages
W $C108,2 [$6400] Helicopter related 1
W $C10A,2 [$640C] Helicopter related 2
w $C10C [Stage 3] Table of addresses of LODs
W $C10C,2 [$CDF1] Address of LOD of stone/dust?
W $C10E,2 [$CE1B] Address of LOD of stone/dust?
W $C110,2 [$C8E0] Address of LOD of car (the perp's car)
W $C112,2 [$C934] Address of LOD of car (a Lambo in S1)
W $C114,2 [$C90A] Address of LOD of car (a truck in S1)
W $C116,2 [$C934] Address of LOD of car (a Lambo in S1)
W $C118,2 [$C8E0] Address of LOD of car (a generic car in S1)
b $C11A [Stage 3] Per-stage difficulty settings
B $C11A,1 How often cars spawn. Lower values spawn cars more often.
B $C11B,1 Smash related parameter
B $C11C,1 Smash related parameter
w $C11D [Stage 3] Per-stage setup data
W $C11D,2 road_pos
W $C11F,2 [$C287] Address of start stretch, curvature
W $C121,2 [$C2AB] Address of start stretch, height
W $C123,2 [$C2CC] Address of start stretch, lanes
W $C125,2 [$C36B] Address of start stretch, right-side objects
W $C127,2 [$C2E7] Address of start stretch, left-side objects
W $C129,2 [$C2DA] Address of start stretch, hazards
w $C12B [Stage 3] Per-stage attract mode data
W $C12B,2 road_pos
W $C12D,2 [$C676] Address of loop section, curvature
W $C12F,2 [$C69C] Address of loop section, height
W $C131,2 [$C6DB] Address of loop section, lanes
W $C133,2 [$C7AC] Address of loop section, right-side objects
W $C135,2 [$C71E] Address of loop section, left-side objects
W $C137,2 [$C6FB] Address of loop section, hazards
b $C139 [Stage 3] Nancy's perp description
B $C139,1 Character identifier (0/1/2/3 = Pilot/Nancy/Raymond/Tony)
W $C13A,2 [$C145] Perp description pointer
W $C13C,2 [$C16D] Perp description pointer
W $C13E,2 [$C196] Perp description pointer
W $C140,2 [$C1C1] Perp description pointer
B $C142,1 terminator?
T $C145 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $C16D "EMERGENCY HERE. A GANG OF CHICAGO PUSHERS"
T $C196 "ARE FLEEING TOWARDS THE SUBURBS. THE TARGET"
T $C1C1 "VEHICLE IS A GERMAN SPORTS CAR... OVER."
b $C1E8 [Stage 3] Arrest messages
B $C1E8,1 ?frame delay until first message
B $C1E9,1 ?frame delay until next message
B $C1EA,1 Flags
B $C1EB,1 Attribute
W $C1EC,2 Back buffer address
W $C1EE,2 Attribute address
T $C1F0 "OK! YOU ARE UNDER ARREST ON"
B $C20B,1 ?frame delay until next message
B $C20C,1 Flags
B $C20D,1 Attribute
W $C20E,2 Back buffer address
W $C210,2 Attribute address
T $C212 "SUSPICION OF SELLING DRUGS"
B $C22C,1 ?frame delay until next message
B $C22D,1 Stop
b $C22E [Stage 3] Hittable hazards
B $C22E,1 ?id
W $C22F,2 [$D64F] Address of LODs
B $C231,1 ?id
W $C232,2 [$D6BF] Address of LODs
b $C234 [Stage 3] Graphic definition
N $C234 Definition
B $C234,1 Hit coord max/min (R/L)
B $C235,1 Hit coord min/max (R/L)
B $C236,1 ?how far to push hero car away if hit
W $C237,2 Argument for routine passed in #REGde
W $C239,2 Address of routine draw_tunnel_light_right
N $C23B Definition
B $C23B,1 Hit coord max/min (R/L)
B $C23C,1 Hit coord min/max (R/L)
B $C23D,1 ?how far to push hero car away if hit
W $C23E,2 Argument for routine passed in #REGde
W $C240,2 Address of routine draw_stretchy_object_right
N $C242 Definition
B $C242,1 Hit coord max/min (R/L)
B $C243,1 Hit coord min/max (R/L)
B $C244,1 ?how far to push hero car away if hit
W $C245,2 Argument for routine passed in #REGde
W $C247,2 Address of routine draw_stretchy_object_right
N $C249 Definition
B $C249,1 Hit coord max/min (R/L)
B $C24A,1 Hit coord min/max (R/L)
B $C24B,1 ?how far to push hero car away if hit
W $C24C,2 Argument for routine passed in #REGde
W $C24E,2 Address of routine draw_stretchy_object_right
N $C250 Definition
B $C250,1 Hit coord max/min (R/L)
B $C251,1 Hit coord min/max (R/L)
B $C252,1 ?how far to push hero car away if hit
W $C253,2 Argument for routine passed in #REGde
W $C255,2 Address of routine draw_stretchy_object_right
N $C257 Definition
B $C257,1 Hit coord max/min (R/L)
B $C258,1 Hit coord min/max (R/L)
B $C259,1 ?how far to push hero car away if hit
W $C25A,2 Argument for routine passed in #REGde
W $C25C,2 Address of routine draw_stretchy_object_right
N $C25E Definition
B $C25E,1 Hit coord max/min (R/L)
b $C25E [Stage 3] Graphic definition
N $C25E Definition
B $C25E,1 Hit coord max/min (R/L)
B $C25F,1 Hit coord min/max (R/L)
B $C25F,1 Hit coord min/max (R/L)
B $C260,1 ?how far to push hero car away if hit
B $C260,1 ?how far to push hero car away if hit
W $C261,2 Argument for routine passed in #REGde
W $C261,2 Argument for routine passed in #REGde
W $C263,2 Address of routine draw_tunnel_light_left
W $C263,2 Address of routine draw_tunnel_light_left
N $C265 Definition
B $C265,1 Hit coord max/min (R/L)
B $C266,1 Hit coord min/max (R/L)
B $C267,1 ?how far to push hero car away if hit
W $C268,2 Argument for routine passed in #REGde
W $C26A,2 Address of routine draw_overhead
N $C26C Definition
B $C26C,1 Hit coord max/min (R/L)
B $C26D,1 Hit coord min/max (R/L)
B $C26E,1 ?how far to push hero car away if hit
W $C26F,2 Argument for routine passed in #REGde
W $C271,2 Address of routine draw_stretchy_object_left
N $C273 Definition
B $C273,1 Hit coord max/min (R/L)
B $C274,1 Hit coord min/max (R/L)
B $C275,1 ?how far to push hero car away if hit
W $C276,2 Argument for routine passed in #REGde
W $C278,2 Address of routine draw_stretchy_object_left
N $C27A Definition
B $C27A,1 Hit coord max/min (R/L)
B $C27B,1 Hit coord min/max (R/L)
B $C27C,1 ?how far to push hero car away if hit
W $C27D,2 Argument for routine passed in #REGde
W $C27F,2 Address of routine draw_stretchy_object_left
N $C281 Definition
B $C281,1 Hit coord max/min (R/L)
B $C282,1 Hit coord min/max (R/L)
B $C283,1 ?how far to push hero car away if hit
W $C284,2 Argument for routine passed in #REGde
W $C286,2 Address of routine draw_stretchy_object_left
N $C288 Definition
B $C288,1 Hit coord max/min (R/L)
b $C288 [Stage 3] Map curvature data
B $C288,1 Curve Straight for 65 units
B $C289,1 Hit coord min/max (R/L)
B $C28A,1 ?how far to push hero car away if hit
W $C28B,2 Argument for routine passed in #REGde
W $C28D,2 Address of routine TBD
B $C28D,1 Curve Right for 136 units
B $C297,1 Curve Straight for 39 units
B $C29A,1 Curve Left Hard for 51 units
B $C29E,1 Curve Straight for 25 units
B $C2A0,1 Curve Right Hard for 55 units
B $C2A4,1 Curve Straight for 29 units
B $C2A6,1 <Esc> Split
W $C2A8,2 [$C3F4] Left target
W $C2AA,2 [$C4D8] Right target
b $C2AC [Stage 3] Map height data
B $C2AC,1 Level Road for 400 units
B $C2C7,1 <Esc> Split
W $C2C9,2 [$C409] Left target
W $C2CB,2 [$C4EF] Right target
b $C2CD [Stage 3] Map lanes data
B $C2CD,1 3 Lanes R             [|||] {82} for 60 units
B $C2CF,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C2D1,1 4 Lanes              [||||] {00} for 338 units
B $C2D5,1 <Esc> Split
W $C2D7,2 [$C44B] Left target
W $C2D9,2 [$C504] Right target
b $C2DB [Stage 3] Map hazards data
B $C2DB,1 Wait for 193 units
B $C2DC,1 Disable Car Spawning
B $C2DE,1 Wait for 3 units
B $C2DF,1 Set Floating Arrow to Left
B $C2E1,1 Wait for 4 units
B $C2E2,1 <Esc> Split
W $C2E4,2 [$C455] Left target
W $C2E6,2 [$C510] Right target
b $C2E8 [Stage 3] Map left object data
B $C2E8,1 TELEGRAPH_POLE for 1 units
B $C2E9,1 (nothing) for 1 units
B $C2EA,1 TELEGRAPH_POLE for 1 units
B $C2EB,1 (nothing) for 1 units
B $C2EC,1 TELEGRAPH_POLE for 1 units
B $C2ED,1 (nothing) for 1 units
B $C2EE,1 TELEGRAPH_POLE for 1 units
B $C2EF,1 (nothing) for 1 units
B $C2F0,1 TELEGRAPH_POLE for 1 units
B $C2F1,1 (nothing) for 1 units
B $C2F2,1 TELEGRAPH_POLE for 1 units
B $C2F3,1 (nothing) for 1 units
B $C2F4,1 TELEGRAPH_POLE for 1 units
B $C2F5,1 (nothing) for 1 units
B $C2F6,1 TELEGRAPH_POLE for 1 units
B $C2F7,1 (nothing) for 1 units
B $C2F8,1 TELEGRAPH_POLE for 1 units
B $C2F9,1 (nothing) for 1 units
B $C2FA,1 TELEGRAPH_POLE for 1 units
B $C2FB,1 (nothing) for 1 units
B $C2FC,1 TELEGRAPH_POLE for 1 units
B $C2FD,1 (nothing) for 1 units
B $C2FE,1 TELEGRAPH_POLE for 1 units
B $C2FF,1 (nothing) for 1 units
B $C300,1 TELEGRAPH_POLE for 1 units
B $C301,1 (nothing) for 1 units
B $C302,1 TELEGRAPH_POLE for 1 units
B $C303,1 (nothing) for 1 units
B $C304,1 TELEGRAPH_POLE for 1 units
B $C305,1 (nothing) for 9 units
B $C306,1 TOWER_BLOCK for 1 units
B $C307,1 (nothing) for 1 units
B $C308,1 TOWER_BLOCK for 1 units
B $C309,1 (nothing) for 1 units
B $C30A,1 TOWER_BLOCK for 1 units
B $C30B,1 (nothing) for 1 units
B $C30C,1 TOWER_BLOCK for 1 units
B $C30D,1 (nothing) for 1 units
B $C30E,1 TOWER_BLOCK for 1 units
B $C30F,1 (nothing) for 1 units
B $C310,1 TOWER_BLOCK for 1 units
B $C311,1 (nothing) for 1 units
B $C312,1 TOWER_BLOCK for 1 units
B $C313,1 (nothing) for 1 units
B $C314,1 TOWER_BLOCK for 1 units
B $C315,1 (nothing) for 1 units
B $C316,1 TOWER_BLOCK for 1 units
B $C317,1 (nothing) for 1 units
B $C318,1 TOWER_BLOCK for 1 units
B $C319,1 (nothing) for 3 units
B $C31A,1 TOWER_BLOCK for 1 units
B $C31B,1 (nothing) for 3 units
B $C31C,1 TOWER_BLOCK for 1 units
B $C31D,1 (nothing) for 3 units
B $C31E,1 TOWER_BLOCK for 1 units
B $C31F,1 (nothing) for 3 units
B $C320,1 TOWER_BLOCK for 1 units
B $C321,1 (nothing) for 1 units
B $C322,1 TOWER_BLOCK for 1 units
B $C323,1 (nothing) for 3 units
B $C324,1 TELEGRAPH_POLE for 1 units
B $C325,1 (nothing) for 1 units
B $C326,1 TELEGRAPH_POLE for 1 units
B $C327,1 (nothing) for 3 units
B $C328,1 TELEGRAPH_POLE for 1 units
B $C329,1 (nothing) for 7 units
B $C32A,1 TOWER_BLOCK for 1 units
B $C32B,1 (nothing) for 1 units
B $C32C,1 TOWER_BLOCK for 1 units
B $C32D,1 (nothing) for 1 units
B $C32E,1 TOWER_BLOCK for 1 units
B $C32F,1 (nothing) for 7 units
B $C330,1 TOWER_BLOCK for 1 units
B $C331,1 (nothing) for 1 units
B $C332,1 TOWER_BLOCK for 1 units
B $C333,1 (nothing) for 1 units
B $C334,1 TOWER_BLOCK for 1 units
B $C335,1 (nothing) for 1 units
B $C336,1 TOWER_BLOCK for 1 units
B $C337,1 (nothing) for 1 units
B $C338,1 TOWER_BLOCK for 1 units
B $C339,1 (nothing) for 1 units
B $C33A,1 TOWER_BLOCK for 1 units
B $C33B,1 (nothing) for 1 units
B $C33C,1 TOWER_BLOCK for 1 units
B $C33D,1 (nothing) for 1 units
B $C33E,1 TOWER_BLOCK for 1 units
B $C33F,1 (nothing) for 1 units
B $C340,1 TOWER_BLOCK for 1 units
B $C341,1 (nothing) for 1 units
B $C342,1 TOWER_BLOCK for 1 units
B $C343,1 (nothing) for 3 units
B $C344,1 TELEGRAPH_POLE for 1 units
B $C345,1 (nothing) for 3 units
B $C346,1 TELEGRAPH_POLE for 1 units
B $C347,1 (nothing) for 1 units
B $C348,1 TELEGRAPH_POLE for 1 units
B $C349,1 (nothing) for 3 units
B $C34A,1 TELEGRAPH_POLE for 1 units
B $C34B,1 (nothing) for 1 units
B $C34C,1 TELEGRAPH_POLE for 1 units
B $C34D,1 (nothing) for 7 units
B $C34E,1 TOWER_BLOCK for 1 units
B $C34F,1 (nothing) for 3 units
B $C350,1 TOWER_BLOCK for 1 units
B $C351,1 (nothing) for 1 units
B $C352,1 TOWER_BLOCK for 1 units
B $C353,1 (nothing) for 3 units
B $C354,1 TOWER_BLOCK for 1 units
B $C355,1 (nothing) for 3 units
B $C356,1 TOWER_BLOCK for 1 units
B $C357,1 (nothing) for 5 units
B $C358,1 TOWER_BLOCK for 1 units
B $C359,1 (nothing) for 3 units
B $C35A,1 SPEED_LIMIT_SIGN for 1 units
B $C35B,1 (nothing) for 5 units
B $C35C,1 SPEED_LIMIT_SIGN for 1 units
B $C35D,1 (nothing) for 9 units
B $C35E,1 SPEED_LIMIT_SIGN for 1 units
B $C35F,1 (nothing) for 5 units
B $C360,1 SPEED_LIMIT_SIGN for 1 units
B $C361,1 (nothing) for 3 units
B $C362,1 TELEGRAPH_POLE for 1 units
B $C363,1 (nothing) for 1 units
B $C364,1 TELEGRAPH_POLE for 1 units
B $C365,1 (nothing) for 1 units
B $C366,1 <Esc> Split
W $C368,2 [$C46F] Left target
W $C36A,2 [$C518] Right target
b $C36C [Stage 3] Map right object data
B $C36C,1 TOWER_BLOCK for 1 units
B $C36D,1 (nothing) for 3 units
B $C36E,1 TOWER_BLOCK for 1 units
B $C36F,1 (nothing) for 3 units
B $C370,1 TOWER_BLOCK for 1 units
B $C371,1 (nothing) for 3 units
B $C372,1 TOWER_BLOCK for 1 units
B $C373,1 (nothing) for 5 units
B $C374,1 TOWER_BLOCK for 1 units
B $C375,1 (nothing) for 1 units
B $C376,1 TOWER_BLOCK for 1 units
B $C377,1 (nothing) for 1 units
B $C378,1 TOWER_BLOCK for 1 units
B $C379,1 (nothing) for 1 units
B $C37A,1 TOWER_BLOCK for 1 units
B $C37B,1 (nothing) for 1 units
B $C37C,1 TOWER_BLOCK for 1 units
B $C37D,1 (nothing) for 1 units
B $C37E,1 TOWER_BLOCK for 1 units
B $C37F,1 (nothing) for 3 units
B $C380,1 TOWER_BLOCK for 1 units
B $C381,1 (nothing) for 3 units
B $C382,1 TOWER_BLOCK for 1 units
B $C383,1 (nothing) for 5 units
B $C384,1 TELEGRAPH_POLE for 1 units
B $C385,1 (nothing) for 1 units
B $C386,1 TELEGRAPH_POLE for 1 units
B $C387,1 (nothing) for 1 units
B $C388,1 TELEGRAPH_POLE for 1 units
B $C389,1 (nothing) for 1 units
B $C38A,1 TELEGRAPH_POLE for 1 units
B $C38B,1 (nothing) for 1 units
B $C38C,1 TELEGRAPH_POLE for 1 units
B $C38D,1 (nothing) for 3 units
B $C38E,1 TELEGRAPH_POLE for 1 units
B $C38F,1 (nothing) for 3 units
B $C390,1 TELEGRAPH_POLE for 1 units
B $C391,1 (nothing) for 3 units
B $C392,1 TELEGRAPH_POLE for 1 units
B $C393,1 (nothing) for 1 units
B $C394,1 TELEGRAPH_POLE for 1 units
B $C395,1 (nothing) for 1 units
B $C396,1 TELEGRAPH_POLE for 1 units
B $C397,1 (nothing) for 1 units
B $C398,1 TELEGRAPH_POLE for 1 units
B $C399,1 (nothing) for 1 units
B $C39A,1 TELEGRAPH_POLE for 1 units
B $C39B,1 (nothing) for 1 units
B $C39C,1 TELEGRAPH_POLE for 1 units
B $C39D,1 (nothing) for 1 units
B $C39E,1 TELEGRAPH_POLE for 1 units
B $C39F,1 (nothing) for 5 units
B $C3A0,1 TELEGRAPH_POLE for 1 units
B $C3A1,1 (nothing) for 3 units
B $C3A2,1 TELEGRAPH_POLE for 1 units
B $C3A3,1 (nothing) for 3 units
B $C3A4,1 TELEGRAPH_POLE for 1 units
B $C3A5,1 (nothing) for 3 units
B $C3A6,1 TELEGRAPH_POLE for 1 units
B $C3A7,1 (nothing) for 1 units
B $C3A8,1 TELEGRAPH_POLE for 1 units
B $C3A9,1 (nothing) for 1 units
B $C3AA,1 TELEGRAPH_POLE for 1 units
B $C3AB,1 (nothing) for 1 units
B $C3AC,1 TELEGRAPH_POLE for 1 units
B $C3AD,1 (nothing) for 5 units
B $C3AE,1 TOWER_BLOCK for 1 units
B $C3AF,1 (nothing) for 1 units
B $C3B0,1 TOWER_BLOCK for 1 units
B $C3B1,1 (nothing) for 1 units
B $C3B2,1 TOWER_BLOCK for 1 units
B $C3B3,1 (nothing) for 1 units
B $C3B4,1 TOWER_BLOCK for 1 units
B $C3B5,1 (nothing) for 1 units
B $C3B6,1 TOWER_BLOCK for 1 units
B $C3B7,1 (nothing) for 1 units
B $C3B8,1 TOWER_BLOCK for 1 units
B $C3B9,1 (nothing) for 1 units
B $C3BA,1 TOWER_BLOCK for 1 units
B $C3BB,1 (nothing) for 1 units
B $C3BC,1 TOWER_BLOCK for 1 units
B $C3BD,1 (nothing) for 1 units
B $C3BE,1 TOWER_BLOCK for 1 units
B $C3BF,1 (nothing) for 1 units
B $C3C0,1 TOWER_BLOCK for 1 units
B $C3C1,1 (nothing) for 1 units
B $C3C2,1 TOWER_BLOCK for 1 units
B $C3C3,1 (nothing) for 1 units
B $C3C4,1 TOWER_BLOCK for 1 units
B $C3C5,1 (nothing) for 1 units
B $C3C6,1 TOWER_BLOCK for 1 units
B $C3C7,1 (nothing) for 5 units
B $C3C8,1 TOWER_BLOCK for 1 units
B $C3C9,1 (nothing) for 3 units
B $C3CA,1 TOWER_BLOCK for 1 units
B $C3CB,1 (nothing) for 1 units
B $C3CC,1 TOWER_BLOCK for 1 units
B $C3CD,1 (nothing) for 3 units
B $C3CE,1 TOWER_BLOCK for 1 units
B $C3CF,1 (nothing) for 3 units
B $C3D0,1 TOWER_BLOCK for 1 units
B $C3D1,1 (nothing) for 9 units
B $C3D2,1 TOWER_BLOCK for 1 units
B $C3D3,1 (nothing) for 3 units
B $C3D4,1 TOWER_BLOCK for 1 units
B $C3D5,1 (nothing) for 7 units
B $C3D6,1 SPEED_LIMIT_SIGN for 1 units
B $C3D7,1 (nothing) for 5 units
B $C3D8,1 SPEED_LIMIT_SIGN for 1 units
B $C3D9,1 (nothing) for 3 units
B $C3DA,1 TELEGRAPH_POLE for 1 units
B $C3DB,1 (nothing) for 1 units
B $C3DC,1 TELEGRAPH_POLE for 1 units
B $C3DD,1 (nothing) for 1 units
B $C3DE,1 TELEGRAPH_POLE for 1 units
B $C3DF,1 (nothing) for 1 units
B $C3E0,1 TELEGRAPH_POLE for 1 units
B $C3E1,1 (nothing) for 1 units
B $C3E2,1 TELEGRAPH_POLE for 1 units
B $C3E3,1 (nothing) for 1 units
B $C3E4,1 TELEGRAPH_POLE for 1 units
B $C3E5,1 (nothing) for 1 units
B $C3E6,1 TELEGRAPH_POLE for 1 units
B $C3E7,1 (nothing) for 1 units
B $C3E8,1 TELEGRAPH_POLE for 1 units
B $C3E9,1 (nothing) for 1 units
B $C3EA,1 TELEGRAPH_POLE for 1 units
B $C3EB,1 (nothing) for 1 units
B $C3EC,1 TELEGRAPH_POLE for 1 units
B $C3ED,1 (nothing) for 1 units
B $C3EE,1 <Esc> Split
W $C3F0,2 [$C4A9] Left target
W $C3F2,2 [$C54B] Right target
b $C3F4 [Stage 3] Map curvature data
B $C3F4,1 Curve Straight for 51 units
B $C3F8,1 Curve Right for 49 units
B $C3FC,1 Curve Right Hard for 30 units
B $C3FE,1 Curve Right Very Hard for 11 units
B $C3FF,1 Curve Right Hard for 10 units
B $C400,1 Curve Right for 42 units
B $C403,1 Curve Straight for 17 units
B $C405,1 <Esc> Jump
W $C407,2 [$C58A] Target
b $C409 [Stage 3] Map height data
B $C409,1 Level Road for 20 units
B $C40B,1 Going Up 3 for 5 units
B $C40C,1 Level Road for 1 units
B $C40D,1 Going Down 3 for 5 units
B $C40E,1 Level Road for 1 units
B $C40F,1 Going Up 3 for 5 units
B $C410,1 Level Road for 1 units
B $C411,1 Going Down 3 for 5 units
B $C412,1 Level Road for 1 units
B $C413,1 Going Up 3 for 5 units
B $C414,1 Level Road for 1 units
B $C415,1 Going Down 3 for 5 units
B $C416,1 Level Road for 1 units
B $C417,1 Going Up 3 for 5 units
B $C418,1 Level Road for 1 units
B $C419,1 Going Down 3 for 5 units
B $C41A,1 Level Road for 1 units
B $C41B,1 Going Up 3 for 5 units
B $C41C,1 Level Road for 1 units
B $C41D,1 Going Down 3 for 5 units
B $C41E,1 Level Road for 1 units
B $C41F,1 Going Up 3 for 5 units
B $C420,1 Level Road for 1 units
B $C421,1 Going Down 3 for 5 units
B $C422,1 Level Road for 1 units
B $C423,1 Going Up 3 for 5 units
B $C424,1 Level Road for 1 units
B $C425,1 Going Down 3 for 5 units
B $C426,1 Level Road for 1 units
B $C427,1 Going Up 3 for 5 units
B $C428,1 Level Road for 1 units
B $C429,1 Going Down 3 for 5 units
B $C42A,1 Level Road for 1 units
B $C42B,1 Going Up 3 for 5 units
B $C42C,1 Level Road for 1 units
B $C42D,1 Going Down 3 for 5 units
B $C42E,1 Level Road for 1 units
B $C42F,1 Going Up 3 for 5 units
B $C430,1 Level Road for 1 units
B $C431,1 Going Down 3 for 5 units
B $C432,1 Level Road for 1 units
B $C433,1 Going Up 3 for 5 units
B $C434,1 Level Road for 1 units
B $C435,1 Going Down 3 for 5 units
B $C436,1 Level Road for 1 units
B $C437,1 Going Up 3 for 5 units
B $C438,1 Level Road for 1 units
B $C439,1 Going Down 3 for 5 units
B $C43A,1 Level Road for 1 units
B $C43B,1 Going Up 3 for 5 units
B $C43C,1 Level Road for 1 units
B $C43D,1 Going Down 3 for 5 units
B $C43E,1 Level Road for 1 units
B $C43F,1 Going Up 3 for 5 units
B $C440,1 Level Road for 1 units
B $C441,1 Going Down 3 for 5 units
B $C442,1 Level Road for 1 units
B $C443,1 Going Up 3 for 5 units
B $C444,1 Level Road for 1 units
B $C445,1 Going Down 3 for 5 units
B $C446,1 Level Road for 11 units
B $C447,1 <Esc> Jump
W $C449,2 [$C5A0] Target
b $C44B [Stage 3] Map lanes data
B $C44B,1 4 Lanes              [||||] {00} for 20 units
B $C44D,1 4 Lanes dirt track   [||||] {C1} for 178 units
B $C44F,1 4 Lanes              [||||] {00} for 12 units
B $C451,1 <Esc> Jump
W $C453,2 [$C5B6] Target
b $C455 [Stage 3] Map hazards data
B $C455,1 Wait for 10 units
B $C456,1 Start Spawning Two Barriers
B $C458,1 Wait for 1 units
B $C459,1 Stop Spawning Barriers 6?
B $C45B,1 Wait for 41 units
B $C45C,1 Start Spawning Two Barriers
B $C45E,1 Wait for 4 units
B $C45F,1 Stop Spawning Barriers 6?
B $C461,1 Wait for 42 units
B $C462,1 Start Spawning Two Barriers
B $C464,1 Wait for 2 units
B $C465,1 Stop Spawning Barriers 3?
B $C467,1 Wait for 1 units
B $C468,1 Enable Car Spawning
B $C46A,1 Wait for 4 units
B $C46B,1 <Esc> Jump
W $C46D,2 [$C5C4] Target
b $C46F [Stage 3] Map left object data
B $C46F,1 (nothing) for 10 units
B $C470,1 SHORT_POLE for 1 units
B $C471,1 (nothing) for 7 units
B $C472,1 TELEGRAPH_POLE for 1 units
B $C473,1 (nothing) for 1 units
B $C474,1 TELEGRAPH_POLE for 1 units
B $C475,1 (nothing) for 1 units
B $C476,1 TELEGRAPH_POLE for 1 units
B $C477,1 (nothing) for 1 units
B $C478,1 TELEGRAPH_POLE for 1 units
B $C479,1 (nothing) for 1 units
B $C47A,1 TELEGRAPH_POLE for 1 units
B $C47B,1 (nothing) for 1 units
B $C47C,1 TELEGRAPH_POLE for 1 units
B $C47D,1 (nothing) for 5 units
B $C47E,1 TELEGRAPH_POLE for 1 units
B $C47F,1 (nothing) for 1 units
B $C480,1 TELEGRAPH_POLE for 1 units
B $C481,1 (nothing) for 5 units
B $C482,1 TELEGRAPH_POLE for 1 units
B $C483,1 (nothing) for 3 units
B $C484,1 TELEGRAPH_POLE for 1 units
B $C485,1 (nothing) for 1 units
B $C486,1 TELEGRAPH_POLE for 1 units
B $C487,1 (nothing) for 1 units
B $C488,1 TELEGRAPH_POLE for 1 units
B $C489,1 (nothing) for 1 units
B $C48A,1 TELEGRAPH_POLE for 1 units
B $C48B,1 (nothing) for 3 units
B $C48C,1 TELEGRAPH_POLE for 1 units
B $C48D,1 (nothing) for 1 units
B $C48E,1 TELEGRAPH_POLE for 1 units
B $C48F,1 (nothing) for 3 units
B $C490,1 TELEGRAPH_POLE for 1 units
B $C491,1 (nothing) for 1 units
B $C492,1 TELEGRAPH_POLE for 1 units
B $C493,1 (nothing) for 7 units
B $C494,1 TELEGRAPH_POLE for 1 units
B $C495,1 (nothing) for 1 units
B $C496,1 TELEGRAPH_POLE for 1 units
B $C497,1 (nothing) for 1 units
B $C498,1 TELEGRAPH_POLE for 1 units
B $C499,1 (nothing) for 1 units
B $C49A,1 TELEGRAPH_POLE for 1 units
B $C49B,1 (nothing) for 13 units
B $C49C,1 TELEGRAPH_POLE for 1 units
B $C49D,1 (nothing) for 1 units
B $C49E,1 TELEGRAPH_POLE for 1 units
B $C49F,1 (nothing) for 1 units
B $C4A0,1 TELEGRAPH_POLE for 1 units
B $C4A1,1 (nothing) for 5 units
B $C4A2,1 TELEGRAPH_POLE for 1 units
B $C4A3,1 (nothing) for 1 units
B $C4A4,1 TELEGRAPH_POLE for 1 units
B $C4A5,1 <Esc> Jump
W $C4A7,2 [$C5C9] Target
b $C4A9 [Stage 3] Map right object data
B $C4A9,1 (nothing) for 2 units
B $C4AA,1 SPEED_LIMIT_SIGN for 1 units
B $C4AB,1 (nothing) for 5 units
B $C4AC,1 SPEED_LIMIT_SIGN for 1 units
B $C4AD,1 (nothing) for 1 units
B $C4AE,1 SHORT_POLE for 1 units
B $C4AF,1 (nothing) for 11 units
B $C4B0,1 TELEGRAPH_POLE for 1 units
B $C4B1,1 (nothing) for 1 units
B $C4B2,1 TELEGRAPH_POLE for 1 units
B $C4B3,1 (nothing) for 1 units
B $C4B4,1 TELEGRAPH_POLE for 1 units
B $C4B5,1 (nothing) for 1 units
B $C4B6,1 TELEGRAPH_POLE for 1 units
B $C4B7,1 (nothing) for 9 units
B $C4B8,1 TELEGRAPH_POLE for 1 units
B $C4B9,1 (nothing) for 1 units
B $C4BA,1 TELEGRAPH_POLE for 1 units
B $C4BB,1 (nothing) for 7 units
B $C4BC,1 TELEGRAPH_POLE for 1 units
B $C4BD,1 (nothing) for 15 units
B $C4BE,1 TELEGRAPH_POLE for 1 units
B $C4BF,1 (nothing) for 1 units
B $C4C0,1 TELEGRAPH_POLE for 1 units
B $C4C1,1 (nothing) for 1 units
B $C4C2,1 TELEGRAPH_POLE for 1 units
B $C4C3,1 (nothing) for 1 units
B $C4C4,1 TELEGRAPH_POLE for 1 units
B $C4C5,1 (nothing) for 1 units
B $C4C6,1 TELEGRAPH_POLE for 1 units
B $C4C7,1 (nothing) for 1 units
B $C4C8,1 TELEGRAPH_POLE for 1 units
B $C4C9,1 (nothing) for 1 units
B $C4CA,1 TELEGRAPH_POLE for 1 units
B $C4CB,1 (nothing) for 1 units
B $C4CC,1 TELEGRAPH_POLE for 1 units
B $C4CD,1 (nothing) for 1 units
B $C4CE,1 TELEGRAPH_POLE for 1 units
B $C4CF,1 (nothing) for 1 units
B $C4D0,1 TELEGRAPH_POLE for 1 units
B $C4D1,1 (nothing) for 13 units
B $C4D2,1 TELEGRAPH_POLE for 1 units
B $C4D3,1 (nothing) for 8 units
B $C4D4,1 <Esc> Jump
W $C4D6,2 [$C620] Target
b $C4D8 [Stage 3] Map curvature data
B $C4D8,1 Curve Straight for 8 units
B $C4D9,1 Curve Left for 7 units
B $C4DA,1 Curve Left Hard for 37 units
B $C4DD,1 Curve Straight for 29 units
B $C4DF,1 Curve Right Hard for 30 units
B $C4E1,1 Curve Right for 9 units
B $C4E2,1 Curve Straight for 11 units
B $C4E3,1 Curve Left for 9 units
B $C4E4,1 Curve Left Hard for 26 units
B $C4E6,1 Curve Left for 5 units
B $C4E7,1 Curve Straight for 23 units
B $C4E9,1 Curve Right for 16 units
B $C4EB,1 <Esc> Jump
W $C4ED,2 [$C58A] Target
b $C4EF [Stage 3] Map height data
B $C4EF,1 Going Up 5 for 42 units
B $C4F2,1 Going Up 3 for 1 units
B $C4F3,1 Going Down 3 for 1 units
B $C4F4,1 Going Down 5 for 41 units
B $C4F7,1 Level Road for 125 units
B $C500,1 <Esc> Jump
W $C502,2 [$C5A0] Target
b $C504 [Stage 3] Map lanes data
B $C504,1 4 Lanes              [||||] {00} for 56 units
B $C506,1 4-3 Narrowing R      [/|||] {8E} for 134 units
B $C508,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C50A,1 4 Lanes              [||||] {00} for 18 units
B $C50C,1 <Esc> Jump
W $C50E,2 [$C5B6] Target
b $C510 [Stage 3] Map hazards data
B $C510,1 Wait for 10 units
B $C511,1 Enable Car Spawning
B $C513,1 Wait for 95 units
B $C514,1 <Esc> Jump
W $C516,2 [$C5C4] Target
b $C518 [Stage 3] Map left object data
B $C518,1 (nothing) for 6 units
B $C519,1 SPEED_LIMIT_SIGN for 1 units
B $C51A,1 (nothing) for 7 units
B $C51B,1 TOWER_BLOCK for 1 units
B $C51C,1 (nothing) for 1 units
B $C51D,1 TOWER_BLOCK for 1 units
B $C51E,1 (nothing) for 1 units
B $C51F,1 TOWER_BLOCK for 1 units
B $C520,1 (nothing) for 1 units
B $C521,1 TOWER_BLOCK for 1 units
B $C522,1 (nothing) for 1 units
B $C523,1 TOWER_BLOCK for 1 units
B $C524,1 (nothing) for 1 units
B $C525,1 TOWER_BLOCK for 1 units
B $C526,1 (nothing) for 2 units
B $C527,1 SHORT_POLE for 4 units
B $C528,1 (nothing) for 5 units
B $C529,1 TOWER_BLOCK for 1 units
B $C52A,1 (nothing) for 3 units
B $C52B,1 TOWER_BLOCK for 1 units
B $C52C,1 (nothing) for 3 units
B $C52D,1 TOWER_BLOCK for 1 units
B $C52E,1 (nothing) for 3 units
B $C52F,1 TOWER_BLOCK for 1 units
B $C530,1 (nothing) for 1 units
B $C531,1 TOWER_BLOCK for 1 units
B $C532,1 (nothing) for 1 units
B $C533,1 TOWER_BLOCK for 1 units
B $C534,1 (nothing) for 1 units
B $C535,1 TOWER_BLOCK for 1 units
B $C536,1 (nothing) for 3 units
B $C537,1 TOWER_BLOCK for 1 units
B $C538,1 (nothing) for 1 units
B $C539,1 TOWER_BLOCK for 1 units
B $C53A,1 (nothing) for 1 units
B $C53B,1 TOWER_BLOCK for 1 units
B $C53C,1 (nothing) for 1 units
B $C53D,1 TOWER_BLOCK for 1 units
B $C53E,1 (nothing) for 1 units
B $C53F,1 TOWER_BLOCK for 1 units
B $C540,1 (nothing) for 1 units
B $C541,1 TOWER_BLOCK for 1 units
B $C542,1 (nothing) for 1 units
B $C543,1 TOWER_BLOCK for 1 units
B $C544,1 (nothing) for 34 units
B $C547,1 <Esc> Jump
W $C549,2 [$C5C9] Target
b $C54B [Stage 3] Map right object data
B $C54B,1 (nothing) for 6 units
B $C54C,1 SPEED_LIMIT_SIGN for 1 units
B $C54D,1 (nothing) for 7 units
B $C54E,1 TOWER_BLOCK for 1 units
B $C54F,1 (nothing) for 1 units
B $C550,1 TOWER_BLOCK for 1 units
B $C551,1 (nothing) for 1 units
B $C552,1 TOWER_BLOCK for 1 units
B $C553,1 (nothing) for 1 units
B $C554,1 TOWER_BLOCK for 1 units
B $C555,1 (nothing) for 1 units
B $C556,1 TOWER_BLOCK for 1 units
B $C557,1 (nothing) for 1 units
B $C558,1 TOWER_BLOCK for 1 units
B $C559,1 (nothing) for 1 units
B $C55A,1 TOWER_BLOCK for 1 units
B $C55B,1 (nothing) for 1 units
B $C55C,1 TOWER_BLOCK for 1 units
B $C55D,1 (nothing) for 3 units
B $C55E,1 TOWER_BLOCK for 1 units
B $C55F,1 (nothing) for 3 units
B $C560,1 TOWER_BLOCK for 1 units
B $C561,1 (nothing) for 3 units
B $C562,1 TOWER_BLOCK for 1 units
B $C563,1 (nothing) for 3 units
B $C564,1 TOWER_BLOCK for 1 units
B $C565,1 (nothing) for 3 units
B $C566,1 TOWER_BLOCK for 1 units
B $C567,1 (nothing) for 3 units
B $C568,1 TOWER_BLOCK for 1 units
B $C569,1 (nothing) for 3 units
B $C56A,1 TOWER_BLOCK for 1 units
B $C56B,1 (nothing) for 3 units
B $C56C,1 TOWER_BLOCK for 1 units
B $C56D,1 (nothing) for 3 units
B $C56E,1 TOWER_BLOCK for 1 units
B $C56F,1 (nothing) for 3 units
B $C570,1 TOWER_BLOCK for 1 units
B $C571,1 (nothing) for 3 units
B $C572,1 TOWER_BLOCK for 1 units
B $C573,1 (nothing) for 3 units
B $C574,1 TOWER_BLOCK for 1 units
B $C575,1 (nothing) for 3 units
B $C576,1 TOWER_BLOCK for 1 units
B $C577,1 (nothing) for 3 units
B $C578,1 TOWER_BLOCK for 1 units
B $C579,1 (nothing) for 3 units
B $C57A,1 TOWER_BLOCK for 1 units
B $C57B,1 (nothing) for 3 units
B $C57C,1 TOWER_BLOCK for 1 units
B $C57D,1 (nothing) for 1 units
B $C57E,1 TOWER_BLOCK for 1 units
B $C57F,1 (nothing) for 1 units
B $C580,1 TOWER_BLOCK for 1 units
B $C581,1 (nothing) for 1 units
B $C582,1 TOWER_BLOCK for 1 units
B $C583,1 (nothing) for 1 units
B $C584,1 TOWER_BLOCK for 1 units
B $C585,1 (nothing) for 4 units
B $C586,1 <Esc> Jump
W $C588,2 [$C620] Target
b $C58A [Stage 3] Map curvature data
B $C58A,1 Curve Straight for 61 units
B $C58F,1 Curve Left for 31 units
B $C592,1 Curve Left Hard for 20 units
B $C594,1 Curve Left for 4 units
B $C595,1 Curve Right for 29 units
B $C597,1 Curve Right Hard for 34 units
B $C59A,1 Curve Right for 7 units
B $C59B,1 Curve Straight for 6 units
B $C59C,1 <Esc> Jump
W $C59E,2 [$C677] Target
b $C5A0 [Stage 3] Map height data
B $C5A0,1 Going Up 3 for 1 units
B $C5A1,1 Going Up 5 for 1 units
B $C5A2,1 Going Up 7 for 6 units
B $C5A3,1 Going Up 5 for 1 units
B $C5A4,1 Going Down 5 for 9 units
B $C5A5,1 Level Road for 9 units
B $C5A6,1 Going Up 5 for 8 units
B $C5A7,1 Going Up 3 for 2 units
B $C5A8,1 Going Up 1 for 1 units
B $C5A9,1 Going Down 1 for 1 units
B $C5AA,1 Going Down 3 for 1 units
B $C5AB,1 Going Down 5 for 6 units
B $C5AC,1 Going Down 3 for 3 units
B $C5AD,1 Going Down 1 for 3 units
B $C5AE,1 Level Road for 55 units
B $C5B2,1 <Esc> Jump
W $C5B4,2 [$C69D] Target
b $C5B6 [Stage 3] Map lanes data
B $C5B6,1 4 Lanes              [||||] {00} for 8 units
B $C5B8,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C5BA,1 3 Lanes L            [|||]  {81} for 178 units
B $C5BC,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C5BE,1 4 Lanes              [||||] {00} for 2 units
B $C5C0,1 <Esc> Jump
W $C5C2,2 [$C6DC] Target
b $C5C4 [Stage 3] Map hazards data
B $C5C4,1 Wait for 96 units
B $C5C5,1 <Esc> Jump
W $C5C7,2 [$C6FC] Target
b $C5C9 [Stage 3] Map left object data
B $C5C9,1 (nothing) for 12 units
B $C5CA,1 OVERHEAD_BRIDGE for 1 units
B $C5CB,1 (nothing) for 1 units
B $C5CC,1 OVERHEAD_BRIDGE for 1 units
B $C5CD,1 (nothing) for 1 units
B $C5CE,1 OVERHEAD_BRIDGE for 1 units
B $C5CF,1 (nothing) for 1 units
B $C5D0,1 OVERHEAD_BRIDGE for 1 units
B $C5D1,1 (nothing) for 1 units
B $C5D2,1 OVERHEAD_BRIDGE for 1 units
B $C5D3,1 (nothing) for 1 units
B $C5D4,1 OVERHEAD_BRIDGE for 1 units
B $C5D5,1 (nothing) for 1 units
B $C5D6,1 OVERHEAD_BRIDGE for 1 units
B $C5D7,1 (nothing) for 1 units
B $C5D8,1 OVERHEAD_BRIDGE for 1 units
B $C5D9,1 (nothing) for 1 units
B $C5DA,1 OVERHEAD_BRIDGE for 1 units
B $C5DB,1 (nothing) for 1 units
B $C5DC,1 OVERHEAD_BRIDGE for 1 units
B $C5DD,1 (nothing) for 1 units
B $C5DE,1 OVERHEAD_BRIDGE for 1 units
B $C5DF,1 (nothing) for 1 units
B $C5E0,1 OVERHEAD_BRIDGE for 1 units
B $C5E1,1 (nothing) for 1 units
B $C5E2,1 OVERHEAD_BRIDGE for 1 units
B $C5E3,1 (nothing) for 1 units
B $C5E4,1 OVERHEAD_BRIDGE for 1 units
B $C5E5,1 (nothing) for 1 units
B $C5E6,1 OVERHEAD_BRIDGE for 1 units
B $C5E7,1 (nothing) for 1 units
B $C5E8,1 OVERHEAD_BRIDGE for 1 units
B $C5E9,1 (nothing) for 1 units
B $C5EA,1 OVERHEAD_BRIDGE for 1 units
B $C5EB,1 (nothing) for 1 units
B $C5EC,1 OVERHEAD_BRIDGE for 1 units
B $C5ED,1 (nothing) for 1 units
B $C5EE,1 OVERHEAD_BRIDGE for 1 units
B $C5EF,1 (nothing) for 1 units
B $C5F0,1 OVERHEAD_BRIDGE for 1 units
B $C5F1,1 (nothing) for 1 units
B $C5F2,1 OVERHEAD_BRIDGE for 1 units
B $C5F3,1 (nothing) for 1 units
B $C5F4,1 OVERHEAD_BRIDGE for 1 units
B $C5F5,1 (nothing) for 1 units
B $C5F6,1 OVERHEAD_BRIDGE for 1 units
B $C5F7,1 (nothing) for 1 units
B $C5F8,1 OVERHEAD_BRIDGE for 1 units
B $C5F9,1 (nothing) for 1 units
B $C5FA,1 OVERHEAD_BRIDGE for 1 units
B $C5FB,1 (nothing) for 1 units
B $C5FC,1 OVERHEAD_BRIDGE for 1 units
B $C5FD,1 (nothing) for 1 units
B $C5FE,1 OVERHEAD_BRIDGE for 1 units
B $C5FF,1 (nothing) for 1 units
B $C600,1 OVERHEAD_BRIDGE for 1 units
B $C601,1 (nothing) for 1 units
B $C602,1 OVERHEAD_BRIDGE for 1 units
B $C603,1 (nothing) for 1 units
B $C604,1 OVERHEAD_BRIDGE for 1 units
B $C605,1 (nothing) for 1 units
B $C606,1 OVERHEAD_BRIDGE for 1 units
B $C607,1 (nothing) for 1 units
B $C608,1 OVERHEAD_BRIDGE for 1 units
B $C609,1 (nothing) for 1 units
B $C60A,1 OVERHEAD_BRIDGE for 1 units
B $C60B,1 (nothing) for 1 units
B $C60C,1 OVERHEAD_BRIDGE for 1 units
B $C60D,1 (nothing) for 1 units
B $C60E,1 OVERHEAD_BRIDGE for 1 units
B $C60F,1 (nothing) for 1 units
B $C610,1 OVERHEAD_BRIDGE for 1 units
B $C611,1 (nothing) for 1 units
B $C612,1 OVERHEAD_BRIDGE for 1 units
B $C613,1 (nothing) for 1 units
B $C614,1 OVERHEAD_BRIDGE for 1 units
B $C615,1 (nothing) for 1 units
B $C616,1 OVERHEAD_BRIDGE for 1 units
B $C617,1 (nothing) for 1 units
B $C618,1 OVERHEAD_BRIDGE for 1 units
B $C619,1 (nothing) for 1 units
B $C61A,1 OVERHEAD_BRIDGE for 1 units
B $C61B,1 (nothing) for 3 units
B $C61C,1 <Esc> Jump
W $C61E,2 [$C71F] Target
b $C620 [Stage 3] Map right object data
B $C620,1 (nothing) for 12 units
B $C621,1 OVERHEAD_BRIDGE for 1 units
B $C622,1 (nothing) for 1 units
B $C623,1 OVERHEAD_BRIDGE for 1 units
B $C624,1 (nothing) for 1 units
B $C625,1 OVERHEAD_BRIDGE for 1 units
B $C626,1 (nothing) for 1 units
B $C627,1 OVERHEAD_BRIDGE for 1 units
B $C628,1 (nothing) for 1 units
B $C629,1 OVERHEAD_BRIDGE for 1 units
B $C62A,1 (nothing) for 1 units
B $C62B,1 OVERHEAD_BRIDGE for 1 units
B $C62C,1 (nothing) for 1 units
B $C62D,1 OVERHEAD_BRIDGE for 1 units
B $C62E,1 (nothing) for 1 units
B $C62F,1 OVERHEAD_BRIDGE for 1 units
B $C630,1 (nothing) for 1 units
B $C631,1 OVERHEAD_BRIDGE for 1 units
B $C632,1 (nothing) for 1 units
B $C633,1 OVERHEAD_BRIDGE for 1 units
B $C634,1 (nothing) for 1 units
B $C635,1 OVERHEAD_BRIDGE for 1 units
B $C636,1 (nothing) for 1 units
B $C637,1 OVERHEAD_BRIDGE for 1 units
B $C638,1 (nothing) for 1 units
B $C639,1 OVERHEAD_BRIDGE for 1 units
B $C63A,1 (nothing) for 1 units
B $C63B,1 OVERHEAD_BRIDGE for 1 units
B $C63C,1 (nothing) for 1 units
B $C63D,1 OVERHEAD_BRIDGE for 1 units
B $C63E,1 (nothing) for 1 units
B $C63F,1 OVERHEAD_BRIDGE for 1 units
B $C640,1 (nothing) for 1 units
B $C641,1 OVERHEAD_BRIDGE for 1 units
B $C642,1 (nothing) for 1 units
B $C643,1 OVERHEAD_BRIDGE for 1 units
B $C644,1 (nothing) for 1 units
B $C645,1 OVERHEAD_BRIDGE for 1 units
B $C646,1 (nothing) for 1 units
B $C647,1 OVERHEAD_BRIDGE for 1 units
B $C648,1 (nothing) for 1 units
B $C649,1 OVERHEAD_BRIDGE for 1 units
B $C64A,1 (nothing) for 1 units
B $C64B,1 OVERHEAD_BRIDGE for 1 units
B $C64C,1 (nothing) for 1 units
B $C64D,1 OVERHEAD_BRIDGE for 1 units
B $C64E,1 (nothing) for 1 units
B $C64F,1 OVERHEAD_BRIDGE for 1 units
B $C650,1 (nothing) for 1 units
B $C651,1 OVERHEAD_BRIDGE for 1 units
B $C652,1 (nothing) for 1 units
B $C653,1 OVERHEAD_BRIDGE for 1 units
B $C654,1 (nothing) for 1 units
B $C655,1 OVERHEAD_BRIDGE for 1 units
B $C656,1 (nothing) for 1 units
B $C657,1 OVERHEAD_BRIDGE for 1 units
B $C658,1 (nothing) for 1 units
B $C659,1 OVERHEAD_BRIDGE for 1 units
B $C65A,1 (nothing) for 1 units
B $C65B,1 OVERHEAD_BRIDGE for 1 units
B $C65C,1 (nothing) for 1 units
B $C65D,1 OVERHEAD_BRIDGE for 1 units
B $C65E,1 (nothing) for 1 units
B $C65F,1 OVERHEAD_BRIDGE for 1 units
B $C660,1 (nothing) for 1 units
B $C661,1 OVERHEAD_BRIDGE for 1 units
B $C662,1 (nothing) for 1 units
B $C663,1 OVERHEAD_BRIDGE for 1 units
B $C664,1 (nothing) for 1 units
B $C665,1 OVERHEAD_BRIDGE for 1 units
B $C666,1 (nothing) for 1 units
B $C667,1 OVERHEAD_BRIDGE for 1 units
B $C668,1 (nothing) for 1 units
B $C669,1 OVERHEAD_BRIDGE for 1 units
B $C66A,1 (nothing) for 1 units
B $C66B,1 OVERHEAD_BRIDGE for 1 units
B $C66C,1 (nothing) for 1 units
B $C66D,1 OVERHEAD_BRIDGE for 1 units
B $C66E,1 (nothing) for 1 units
B $C66F,1 OVERHEAD_BRIDGE for 1 units
B $C670,1 (nothing) for 1 units
B $C671,1 OVERHEAD_BRIDGE for 1 units
B $C672,1 (nothing) for 3 units
B $C673,1 <Esc> Jump
W $C675,2 [$C7AD] Target
b $C677 [Stage 3] Map curvature data
B $C677,1 Curve Straight for 20 units
b $C677 [Stage 3] Map curvature data
B $C677,1 Curve Straight for 20 units
B $C679,1 Curve Right Hard for 45 units
B $C679,1 Curve Right Hard for 45 units
B $C67C,1 Curve Right for 9 units
B $C67C,1 Curve Right for 9 units
B $C67D,1 Curve Straight for 39 units
B $C67D,1 Curve Straight for 39 units
B $C680,1 Curve Left for 24 units
B $C680,1 Curve Left for 24 units
B $C682,1 Curve Right for 15 units
B $C682,1 Curve Right for 15 units
B $C683,1 Curve Straight for 35 units
B $C683,1 Curve Straight for 35 units
B $C686,1 Curve Left for 25 units
B $C686,1 Curve Left for 25 units
B $C688,1 Curve Left Hard for 92 units
B $C688,1 Curve Left Hard for 92 units
B $C68F,1 Curve Left for 56 units
B $C68F,1 Curve Left for 56 units
B $C693,1 Curve Straight for 84 units
B $C693,1 Curve Straight for 84 units
B $C699,1 <Esc> Loop
B $C699,1 <Esc> Loop
W $C69B,2 [$C677] Target
W $C69B,2 [$C677] Target
b $C69D [Stage 3] Map height data
B $C69D,1 Level Road for 14 units
b $C69D [Stage 3] Map height data
B $C69D,1 Level Road for 14 units
B $C69E,1 Going Up 1 for 1 units
B $C69E,1 Going Up 1 for 1 units
B $C69F,1 Going Up 3 for 40 units
B $C69F,1 Going Up 3 for 40 units
B $C6A2,1 Going Down 5 for 17 units
B $C6A2,1 Going Down 5 for 17 units
B $C6A4,1 Going Down 3 for 6 units
B $C6A4,1 Going Down 3 for 6 units
B $C6A5,1 Going Down 1 for 5 units
B $C6A5,1 Going Down 1 for 5 units
B $C6A6,1 Level Road for 16 units
B $C6A6,1 Level Road for 16 units
B $C6A8,1 Going Down 5 for 2 units
B $C6A8,1 Going Down 5 for 2 units
B $C6A9,1 Going Down 7 for 5 units
B $C6A9,1 Going Down 7 for 5 units
B $C6AA,1 Going Down 5 for 6 units
B $C6AA,1 Going Down 5 for 6 units
B $C6AB,1 Going Down 3 for 2 units
B $C6AB,1 Going Down 3 for 2 units
B $C6AC,1 Going Down 1 for 2 units
B $C6AC,1 Going Down 1 for 2 units
B $C6AD,1 Going Up 1 for 2 units
B $C6AD,1 Going Up 1 for 2 units
B $C6AE,1 Going Up 3 for 2 units
B $C6AE,1 Going Up 3 for 2 units
B $C6AF,1 Going Up 5 for 4 units
B $C6AF,1 Going Up 5 for 4 units
B $C6B0,1 Going Up 7 for 10 units
B $C6B0,1 Going Up 7 for 10 units
B $C6B1,1 Going Down 7 for 1 units
B $C6B1,1 Going Down 7 for 1 units
B $C6B2,1 Going Down 5 for 1 units
B $C6B2,1 Going Down 5 for 1 units
B $C6B3,1 Going Down 3 for 2 units
B $C6B3,1 Going Down 3 for 2 units
B $C6B4,1 Going Down 1 for 1 units
B $C6B4,1 Going Down 1 for 1 units
B $C6B5,1 Level Road for 77 units
B $C6B5,1 Level Road for 77 units
B $C6BB,1 Going Down 7 for 1 units
B $C6BB,1 Going Down 7 for 1 units
B $C6BC,1 Level Road for 171 units
B $C6BC,1 Level Road for 171 units
B $C6C8,1 Going Up 3 for 3 units
B $C6C8,1 Going Up 3 for 3 units
B $C6C9,1 Going Up 5 for 2 units
B $C6C9,1 Going Up 5 for 2 units
B $C6CA,1 Going Up 7 for 4 units
B $C6CA,1 Going Up 7 for 4 units
B $C6CB,1 Going Up 5 for 1 units
B $C6CB,1 Going Up 5 for 1 units
B $C6CC,1 Going Down 5 for 1 units
B $C6CC,1 Going Down 5 for 1 units
B $C6CD,1 Going Down 7 for 3 units
B $C6CD,1 Going Down 7 for 3 units
B $C6CE,1 Going Down 5 for 1 units
B $C6CE,1 Going Down 5 for 1 units
B $C6CF,1 Going Down 3 for 1 units
B $C6CF,1 Going Down 3 for 1 units
B $C6D0,1 Going Up 3 for 1 units
B $C6D0,1 Going Up 3 for 1 units
B $C6D1,1 Going Up 5 for 1 units
B $C6D1,1 Going Up 5 for 1 units
B $C6D2,1 Going Up 7 for 3 units
B $C6D2,1 Going Up 7 for 3 units
B $C6D3,1 Going Down 5 for 1 units
B $C6D3,1 Going Down 5 for 1 units
B $C6D4,1 Going Down 3 for 14 units
B $C6D4,1 Going Down 3 for 14 units
B $C6D5,1 Going Down 1 for 4 units
B $C6D5,1 Going Down 1 for 4 units
B $C6D6,1 Level Road for 16 units
B $C6D6,1 Level Road for 16 units
B $C6D8,1 <Esc> Loop
B $C6D8,1 <Esc> Loop
W $C6DA,2 [$C69D] Target
W $C6DA,2 [$C69D] Target
b $C6DC [Stage 3] Map lanes data
B $C6DC,1 4 Lanes              [||||] {00} for 82 units
b $C6DC [Stage 3] Map lanes data
B $C6DC,1 4 Lanes              [||||] {00} for 82 units
B $C6DE,1 4-3 Narrowing R      [/|||] {8E} for 8 units
B $C6DE,1 4-3 Narrowing R      [/|||] {8E} for 8 units
B $C6E0,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C6E0,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C6E2,1 4 Lanes              [||||] {00} for 50 units
B $C6E2,1 4 Lanes              [||||] {00} for 50 units
B $C6E4,1 4-3 Narrowing R      [/|||] {8E} for 4 units
B $C6E4,1 4-3 Narrowing R      [/|||] {8E} for 4 units
B $C6E6,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C6E6,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C6E8,1 4 Lanes              [||||] {00} for 10 units
B $C6E8,1 4 Lanes              [||||] {00} for 10 units
B $C6EA,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C6EA,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C6EC,1 3 Lanes L            [|||]  {81} for 2 units
B $C6EC,1 3 Lanes L            [|||]  {81} for 2 units
B $C6EE,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C6EE,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C6F0,1 4 Lanes              [||||] {00} for 250 units
B $C6F0,1 4 Lanes              [||||] {00} for 250 units
B $C6F2,1 3 Lanes L            [|||]  {81} for 26 units
B $C6F2,1 3 Lanes L            [|||]  {81} for 26 units
B $C6F4,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C6F4,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C6F6,1 4 Lanes              [||||] {00} for 2 units
B $C6F6,1 4 Lanes              [||||] {00} for 2 units
B $C6F8,1 <Esc> Loop
B $C6F8,1 <Esc> Loop
W $C6FA,2 [$C6DC] Target
W $C6FA,2 [$C6DC] Target
b $C6FC [Stage 3] Map hazards data
B $C6FC,1 Wait for 18 units
b $C6FC [Stage 3] Map hazards data
B $C6FC,1 Wait for 18 units
B $C6FD,1 Start Spawning Barriers Left
B $C6FD,1 Start Spawning Barriers Left
B $C6FF,1 Wait for 2 units
B $C6FF,1 Wait for 2 units
B $C700,1 Stop Spawning Barriers 3?
B $C700,1 Stop Spawning Barriers 3?
B $C702,1 Wait for 79 units
B $C702,1 Wait for 79 units
B $C703,1 Start Spawning Barriers Right
B $C703,1 Start Spawning Barriers Right
B $C705,1 Wait for 2 units
B $C705,1 Wait for 2 units
B $C706,1 Stop Spawning Barriers 3?
B $C706,1 Stop Spawning Barriers 3?
B $C708,1 Wait for 19 units
B $C708,1 Wait for 19 units
B $C709,1 Start Spawning Barriers Right
B $C709,1 Start Spawning Barriers Right
B $C70B,1 Wait for 2 units
B $C70B,1 Wait for 2 units
B $C70C,1 Stop Spawning Barriers 3?
B $C70C,1 Stop Spawning Barriers 3?
B $C70E,1 Wait for 85 units
B $C70E,1 Wait for 85 units
B $C70F,1 Start Spawning Barriers Right
B $C70F,1 Start Spawning Barriers Right
B $C711,1 Wait for 1 units
B $C711,1 Wait for 1 units
B $C712,1 Stop Spawning Barriers 3?
B $C712,1 Stop Spawning Barriers 3?
B $C714,1 Wait for 8 units
B $C714,1 Wait for 8 units
B $C715,1 Unknown command 4
B $C715,1 Unknown command 4
B $C717,1 Wait for 3 units
B $C717,1 Wait for 3 units
B $C718,1 Stop Spawning Barriers 3?
B $C718,1 Stop Spawning Barriers 3?
B $C71A,1 Wait for 3 units
B $C71A,1 Wait for 3 units
B $C71B,1 <Esc> Loop
B $C71B,1 <Esc> Loop
W $C71D,2 [$C6FC] Target
W $C71D,2 [$C6FC] Target
b $C71F [Stage 3] Map left object data
B $C71F,1 (nothing) for 4 units
b $C71F [Stage 3] Map left object data
B $C71F,1 (nothing) for 4 units
B $C720,1 SPEED_LIMIT_SIGN for 1 units
B $C720,1 SPEED_LIMIT_SIGN for 1 units
B $C721,1 (nothing) for 5 units
B $C721,1 (nothing) for 5 units
B $C722,1 TOWER_BLOCK for 1 units
B $C722,1 TOWER_BLOCK for 1 units
B $C723,1 (nothing) for 1 units
B $C723,1 (nothing) for 1 units
B $C724,1 TOWER_BLOCK for 1 units
B $C724,1 TOWER_BLOCK for 1 units
B $C725,1 (nothing) for 1 units
B $C725,1 (nothing) for 1 units
B $C726,1 TOWER_BLOCK for 1 units
B $C726,1 TOWER_BLOCK for 1 units
B $C727,1 (nothing) for 1 units
B $C727,1 (nothing) for 1 units
B $C728,1 TOWER_BLOCK for 1 units
B $C728,1 TOWER_BLOCK for 1 units
B $C729,1 (nothing) for 1 units
B $C729,1 (nothing) for 1 units
B $C72A,1 TOWER_BLOCK for 1 units
B $C72A,1 TOWER_BLOCK for 1 units
B $C72B,1 (nothing) for 1 units
B $C72B,1 (nothing) for 1 units
B $C72C,1 TOWER_BLOCK for 1 units
B $C72C,1 TOWER_BLOCK for 1 units
B $C72D,1 (nothing) for 1 units
B $C72D,1 (nothing) for 1 units
B $C72E,1 TOWER_BLOCK for 1 units
B $C72E,1 TOWER_BLOCK for 1 units
B $C72F,1 (nothing) for 1 units
B $C72F,1 (nothing) for 1 units
B $C730,1 TOWER_BLOCK for 1 units
B $C730,1 TOWER_BLOCK for 1 units
B $C731,1 (nothing) for 1 units
B $C731,1 (nothing) for 1 units
B $C732,1 TOWER_BLOCK for 1 units
B $C732,1 TOWER_BLOCK for 1 units
B $C733,1 (nothing) for 1 units
B $C733,1 (nothing) for 1 units
B $C734,1 TOWER_BLOCK for 1 units
B $C734,1 TOWER_BLOCK for 1 units
B $C735,1 (nothing) for 1 units
B $C735,1 (nothing) for 1 units
B $C736,1 TOWER_BLOCK for 1 units
B $C736,1 TOWER_BLOCK for 1 units
B $C737,1 (nothing) for 1 units
B $C737,1 (nothing) for 1 units
B $C738,1 TOWER_BLOCK for 1 units
B $C738,1 TOWER_BLOCK for 1 units
B $C739,1 (nothing) for 1 units
B $C739,1 (nothing) for 1 units
B $C73A,1 TOWER_BLOCK for 1 units
B $C73A,1 TOWER_BLOCK for 1 units
B $C73B,1 (nothing) for 3 units
B $C73B,1 (nothing) for 3 units
B $C73C,1 TOWER_BLOCK for 1 units
B $C73C,1 TOWER_BLOCK for 1 units
B $C73D,1 (nothing) for 3 units
B $C73D,1 (nothing) for 3 units
B $C73E,1 TOWER_BLOCK for 1 units
B $C73E,1 TOWER_BLOCK for 1 units
B $C73F,1 (nothing) for 1 units
B $C73F,1 (nothing) for 1 units
B $C740,1 TOWER_BLOCK for 1 units
B $C740,1 TOWER_BLOCK for 1 units
B $C741,1 (nothing) for 3 units
B $C741,1 (nothing) for 3 units
B $C742,1 TELEGRAPH_POLE for 1 units
B $C742,1 TELEGRAPH_POLE for 1 units
B $C743,1 (nothing) for 1 units
B $C743,1 (nothing) for 1 units
B $C744,1 TELEGRAPH_POLE for 1 units
B $C744,1 TELEGRAPH_POLE for 1 units
B $C745,1 (nothing) for 1 units
B $C745,1 (nothing) for 1 units
B $C746,1 TELEGRAPH_POLE for 1 units
B $C746,1 TELEGRAPH_POLE for 1 units
B $C747,1 (nothing) for 1 units
B $C747,1 (nothing) for 1 units
B $C748,1 TELEGRAPH_POLE for 1 units
B $C748,1 TELEGRAPH_POLE for 1 units
B $C749,1 (nothing) for 1 units
B $C749,1 (nothing) for 1 units
B $C74A,1 TELEGRAPH_POLE for 1 units
B $C74A,1 TELEGRAPH_POLE for 1 units
B $C74B,1 (nothing) for 1 units
B $C74B,1 (nothing) for 1 units
B $C74C,1 TELEGRAPH_POLE for 1 units
B $C74C,1 TELEGRAPH_POLE for 1 units
B $C74D,1 (nothing) for 3 units
B $C74D,1 (nothing) for 3 units
B $C74E,1 TELEGRAPH_POLE for 1 units
B $C74E,1 TELEGRAPH_POLE for 1 units
B $C74F,1 (nothing) for 3 units
B $C74F,1 (nothing) for 3 units
B $C750,1 TELEGRAPH_POLE for 1 units
B $C750,1 TELEGRAPH_POLE for 1 units
B $C751,1 (nothing) for 5 units
B $C751,1 (nothing) for 5 units
B $C752,1 TELEGRAPH_POLE for 1 units
B $C752,1 TELEGRAPH_POLE for 1 units
B $C753,1 (nothing) for 1 units
B $C753,1 (nothing) for 1 units
B $C754,1 TELEGRAPH_POLE for 1 units
B $C754,1 TELEGRAPH_POLE for 1 units
B $C755,1 (nothing) for 1 units
B $C755,1 (nothing) for 1 units
B $C756,1 TELEGRAPH_POLE for 1 units
B $C756,1 TELEGRAPH_POLE for 1 units
B $C757,1 (nothing) for 1 units
B $C757,1 (nothing) for 1 units
B $C758,1 TELEGRAPH_POLE for 1 units
B $C758,1 TELEGRAPH_POLE for 1 units
B $C759,1 (nothing) for 1 units
B $C759,1 (nothing) for 1 units
B $C75A,1 TELEGRAPH_POLE for 1 units
B $C75A,1 TELEGRAPH_POLE for 1 units
B $C75B,1 (nothing) for 3 units
B $C75B,1 (nothing) for 3 units
B $C75C,1 TOWER_BLOCK for 1 units
B $C75C,1 TOWER_BLOCK for 1 units
B $C75D,1 (nothing) for 1 units
B $C75D,1 (nothing) for 1 units
B $C75E,1 TOWER_BLOCK for 1 units
B $C75E,1 TOWER_BLOCK for 1 units
B $C75F,1 (nothing) for 1 units
B $C75F,1 (nothing) for 1 units
B $C760,1 TOWER_BLOCK for 1 units
B $C760,1 TOWER_BLOCK for 1 units
B $C761,1 (nothing) for 1 units
B $C761,1 (nothing) for 1 units
B $C762,1 TOWER_BLOCK for 1 units
B $C762,1 TOWER_BLOCK for 1 units
B $C763,1 (nothing) for 1 units
B $C763,1 (nothing) for 1 units
B $C764,1 TOWER_BLOCK for 1 units
B $C764,1 TOWER_BLOCK for 1 units
B $C765,1 (nothing) for 1 units
B $C765,1 (nothing) for 1 units
B $C766,1 TOWER_BLOCK for 1 units
B $C766,1 TOWER_BLOCK for 1 units
B $C767,1 (nothing) for 9 units
B $C767,1 (nothing) for 9 units
B $C768,1 SPEED_LIMIT_SIGN for 1 units
B $C768,1 SPEED_LIMIT_SIGN for 1 units
B $C769,1 (nothing) for 21 units
B $C769,1 (nothing) for 21 units
B $C76B,1 SPEED_LIMIT_SIGN for 1 units
B $C76B,1 SPEED_LIMIT_SIGN for 1 units
B $C76C,1 (nothing) for 11 units
B $C76C,1 (nothing) for 11 units
B $C76D,1 TELEGRAPH_POLE for 1 units
B $C76D,1 TELEGRAPH_POLE for 1 units
B $C76E,1 (nothing) for 1 units
B $C76E,1 (nothing) for 1 units
B $C76F,1 TELEGRAPH_POLE for 1 units
B $C76F,1 TELEGRAPH_POLE for 1 units
B $C770,1 (nothing) for 1 units
B $C770,1 (nothing) for 1 units
B $C771,1 TELEGRAPH_POLE for 1 units
B $C771,1 TELEGRAPH_POLE for 1 units
B $C772,1 (nothing) for 1 units
B $C772,1 (nothing) for 1 units
B $C773,1 TELEGRAPH_POLE for 1 units
B $C773,1 TELEGRAPH_POLE for 1 units
B $C774,1 (nothing) for 1 units
B $C774,1 (nothing) for 1 units
B $C775,1 TELEGRAPH_POLE for 1 units
B $C775,1 TELEGRAPH_POLE for 1 units
B $C776,1 (nothing) for 1 units
B $C776,1 (nothing) for 1 units
B $C777,1 TELEGRAPH_POLE for 1 units
B $C777,1 TELEGRAPH_POLE for 1 units
B $C778,1 (nothing) for 1 units
B $C778,1 (nothing) for 1 units
B $C779,1 TELEGRAPH_POLE for 1 units
B $C779,1 TELEGRAPH_POLE for 1 units
B $C77A,1 (nothing) for 1 units
B $C77A,1 (nothing) for 1 units
B $C77B,1 TELEGRAPH_POLE for 1 units
B $C77B,1 TELEGRAPH_POLE for 1 units
B $C77C,1 (nothing) for 7 units
B $C77C,1 (nothing) for 7 units
B $C77D,1 TOWER_BLOCK for 1 units
B $C77D,1 TOWER_BLOCK for 1 units
B $C77E,1 (nothing) for 1 units
B $C77E,1 (nothing) for 1 units
B $C77F,1 TOWER_BLOCK for 1 units
B $C77F,1 TOWER_BLOCK for 1 units
B $C780,1 (nothing) for 1 units
B $C780,1 (nothing) for 1 units
B $C781,1 TOWER_BLOCK for 1 units
B $C781,1 TOWER_BLOCK for 1 units
B $C782,1 (nothing) for 1 units
B $C782,1 (nothing) for 1 units
B $C783,1 TOWER_BLOCK for 1 units
B $C783,1 TOWER_BLOCK for 1 units
B $C784,1 (nothing) for 1 units
B $C784,1 (nothing) for 1 units
B $C785,1 TOWER_BLOCK for 1 units
B $C785,1 TOWER_BLOCK for 1 units
B $C786,1 (nothing) for 1 units
B $C786,1 (nothing) for 1 units
B $C787,1 TOWER_BLOCK for 1 units
B $C787,1 TOWER_BLOCK for 1 units
B $C788,1 (nothing) for 1 units
B $C788,1 (nothing) for 1 units
B $C789,1 TOWER_BLOCK for 1 units
B $C789,1 TOWER_BLOCK for 1 units
B $C78A,1 (nothing) for 1 units
B $C78A,1 (nothing) for 1 units
B $C78B,1 TOWER_BLOCK for 1 units
B $C78B,1 TOWER_BLOCK for 1 units
B $C78C,1 (nothing) for 1 units
B $C78C,1 (nothing) for 1 units
B $C78D,1 TOWER_BLOCK for 1 units
B $C78D,1 TOWER_BLOCK for 1 units
B $C78E,1 (nothing) for 1 units
B $C78E,1 (nothing) for 1 units
B $C78F,1 TOWER_BLOCK for 1 units
B $C78F,1 TOWER_BLOCK for 1 units
B $C790,1 (nothing) for 1 units
B $C790,1 (nothing) for 1 units
B $C791,1 TOWER_BLOCK for 1 units
B $C791,1 TOWER_BLOCK for 1 units
B $C792,1 (nothing) for 1 units
B $C792,1 (nothing) for 1 units
B $C793,1 TOWER_BLOCK for 1 units
B $C793,1 TOWER_BLOCK for 1 units
B $C794,1 (nothing) for 1 units
B $C794,1 (nothing) for 1 units
B $C795,1 TOWER_BLOCK for 1 units
B $C795,1 TOWER_BLOCK for 1 units
B $C796,1 (nothing) for 1 units
B $C796,1 (nothing) for 1 units
B $C797,1 TOWER_BLOCK for 1 units
B $C797,1 TOWER_BLOCK for 1 units
B $C798,1 (nothing) for 1 units
B $C798,1 (nothing) for 1 units
B $C799,1 TOWER_BLOCK for 1 units
B $C799,1 TOWER_BLOCK for 1 units
B $C79A,1 (nothing) for 1 units
B $C79A,1 (nothing) for 1 units
B $C79B,1 TOWER_BLOCK for 1 units
B $C79B,1 TOWER_BLOCK for 1 units
B $C79C,1 (nothing) for 5 units
B $C79C,1 (nothing) for 5 units
B $C79D,1 SPEED_LIMIT_SIGN for 1 units
B $C79D,1 SPEED_LIMIT_SIGN for 1 units
B $C79E,1 (nothing) for 5 units
B $C79E,1 (nothing) for 5 units
B $C79F,1 SPEED_LIMIT_SIGN for 1 units
B $C79F,1 SPEED_LIMIT_SIGN for 1 units
B $C7A0,1 (nothing) for 9 units
B $C7A0,1 (nothing) for 9 units
B $C7A1,1 TELEGRAPH_POLE for 1 units
B $C7A1,1 TELEGRAPH_POLE for 1 units
B $C7A2,1 (nothing) for 1 units
B $C7A2,1 (nothing) for 1 units
B $C7A3,1 TELEGRAPH_POLE for 1 units
B $C7A3,1 TELEGRAPH_POLE for 1 units
B $C7A4,1 (nothing) for 1 units
B $C7A4,1 (nothing) for 1 units
B $C7A5,1 TELEGRAPH_POLE for 1 units
B $C7A5,1 TELEGRAPH_POLE for 1 units
B $C7A6,1 (nothing) for 1 units
B $C7A6,1 (nothing) for 1 units
B $C7A7,1 TELEGRAPH_POLE for 1 units
B $C7A7,1 TELEGRAPH_POLE for 1 units
B $C7A8,1 (nothing) for 3 units
B $C7A8,1 (nothing) for 3 units
B $C7A9,1 <Esc> Loop
B $C7A9,1 <Esc> Loop
W $C7AB,2 [$C71F] Target
W $C7AB,2 [$C71F] Target
b $C7AD [Stage 3] Map right object data
B $C7AD,1 (nothing) for 4 units
b $C7AD [Stage 3] Map right object data
B $C7AD,1 (nothing) for 4 units
B $C7AE,1 SPEED_LIMIT_SIGN for 1 units
B $C7AE,1 SPEED_LIMIT_SIGN for 1 units
B $C7AF,1 (nothing) for 25 units
B $C7AF,1 (nothing) for 25 units
B $C7B1,1 TELEGRAPH_POLE for 1 units
B $C7B1,1 TELEGRAPH_POLE for 1 units
B $C7B2,1 (nothing) for 1 units
B $C7B2,1 (nothing) for 1 units
B $C7B3,1 TELEGRAPH_POLE for 1 units
B $C7B3,1 TELEGRAPH_POLE for 1 units
B $C7B4,1 (nothing) for 1 units
B $C7B4,1 (nothing) for 1 units
B $C7B5,1 TELEGRAPH_POLE for 1 units
B $C7B5,1 TELEGRAPH_POLE for 1 units
B $C7B6,1 (nothing) for 1 units
B $C7B6,1 (nothing) for 1 units
B $C7B7,1 TELEGRAPH_POLE for 1 units
B $C7B7,1 TELEGRAPH_POLE for 1 units
B $C7B8,1 (nothing) for 1 units
B $C7B8,1 (nothing) for 1 units
B $C7B9,1 TELEGRAPH_POLE for 1 units
B $C7B9,1 TELEGRAPH_POLE for 1 units
B $C7BA,1 (nothing) for 1 units
B $C7BA,1 (nothing) for 1 units
B $C7BB,1 TELEGRAPH_POLE for 1 units
B $C7BB,1 TELEGRAPH_POLE for 1 units
B $C7BC,1 (nothing) for 1 units
B $C7BC,1 (nothing) for 1 units
B $C7BD,1 TELEGRAPH_POLE for 1 units
B $C7BD,1 TELEGRAPH_POLE for 1 units
B $C7BE,1 (nothing) for 1 units
B $C7BE,1 (nothing) for 1 units
B $C7BF,1 TELEGRAPH_POLE for 1 units
B $C7BF,1 TELEGRAPH_POLE for 1 units
B $C7C0,1 (nothing) for 1 units
B $C7C0,1 (nothing) for 1 units
B $C7C1,1 TELEGRAPH_POLE for 1 units
B $C7C1,1 TELEGRAPH_POLE for 1 units
B $C7C2,1 (nothing) for 1 units
B $C7C2,1 (nothing) for 1 units
B $C7C3,1 TELEGRAPH_POLE for 1 units
B $C7C3,1 TELEGRAPH_POLE for 1 units
B $C7C4,1 (nothing) for 1 units
B $C7C4,1 (nothing) for 1 units
B $C7C5,1 TELEGRAPH_POLE for 1 units
B $C7C5,1 TELEGRAPH_POLE for 1 units
B $C7C6,1 (nothing) for 1 units
B $C7C6,1 (nothing) for 1 units
B $C7C7,1 TELEGRAPH_POLE for 1 units
B $C7C7,1 TELEGRAPH_POLE for 1 units
B $C7C8,1 (nothing) for 1 units
B $C7C8,1 (nothing) for 1 units
B $C7C9,1 TELEGRAPH_POLE for 1 units
B $C7C9,1 TELEGRAPH_POLE for 1 units
B $C7CA,1 (nothing) for 9 units
B $C7CA,1 (nothing) for 9 units
B $C7CB,1 TELEGRAPH_POLE for 1 units
B $C7CB,1 TELEGRAPH_POLE for 1 units
B $C7CC,1 (nothing) for 1 units
B $C7CC,1 (nothing) for 1 units
B $C7CD,1 TELEGRAPH_POLE for 1 units
B $C7CD,1 TELEGRAPH_POLE for 1 units
B $C7CE,1 (nothing) for 1 units
B $C7CE,1 (nothing) for 1 units
B $C7CF,1 TELEGRAPH_POLE for 1 units
B $C7CF,1 TELEGRAPH_POLE for 1 units
B $C7D0,1 (nothing) for 1 units
B $C7D0,1 (nothing) for 1 units
B $C7D1,1 TELEGRAPH_POLE for 1 units
B $C7D1,1 TELEGRAPH_POLE for 1 units
B $C7D2,1 (nothing) for 1 units
B $C7D2,1 (nothing) for 1 units
B $C7D3,1 TELEGRAPH_POLE for 1 units
B $C7D3,1 TELEGRAPH_POLE for 1 units
B $C7D4,1 (nothing) for 1 units
B $C7D4,1 (nothing) for 1 units
B $C7D5,1 TELEGRAPH_POLE for 1 units
B $C7D5,1 TELEGRAPH_POLE for 1 units
B $C7D6,1 (nothing) for 1 units
B $C7D6,1 (nothing) for 1 units
B $C7D7,1 TELEGRAPH_POLE for 1 units
B $C7D7,1 TELEGRAPH_POLE for 1 units
B $C7D8,1 (nothing) for 3 units
B $C7D8,1 (nothing) for 3 units
B $C7D9,1 TELEGRAPH_POLE for 1 units
B $C7D9,1 TELEGRAPH_POLE for 1 units
B $C7DA,1 (nothing) for 3 units
B $C7DA,1 (nothing) for 3 units
B $C7DB,1 TELEGRAPH_POLE for 1 units
B $C7DB,1 TELEGRAPH_POLE for 1 units
B $C7DC,1 (nothing) for 17 units
B $C7DC,1 (nothing) for 17 units
B $C7DE,1 TOWER_BLOCK for 1 units
B $C7DE,1 TOWER_BLOCK for 1 units
B $C7DF,1 (nothing) for 1 units
B $C7DF,1 (nothing) for 1 units
B $C7E0,1 TOWER_BLOCK for 1 units
B $C7E0,1 TOWER_BLOCK for 1 units
B $C7E1,1 (nothing) for 1 units
B $C7E1,1 (nothing) for 1 units
B $C7E2,1 TOWER_BLOCK for 1 units
B $C7E2,1 TOWER_BLOCK for 1 units
B $C7E3,1 (nothing) for 1 units
B $C7E3,1 (nothing) for 1 units
B $C7E4,1 TOWER_BLOCK for 1 units
B $C7E4,1 TOWER_BLOCK for 1 units
B $C7E5,1 (nothing) for 1 units
B $C7E5,1 (nothing) for 1 units
B $C7E6,1 TOWER_BLOCK for 1 units
B $C7E6,1 TOWER_BLOCK for 1 units
B $C7E7,1 (nothing) for 1 units
B $C7E7,1 (nothing) for 1 units
B $C7E8,1 TOWER_BLOCK for 1 units
B $C7E8,1 TOWER_BLOCK for 1 units
B $C7E9,1 (nothing) for 1 units
B $C7E9,1 (nothing) for 1 units
B $C7EA,1 TOWER_BLOCK for 1 units
B $C7EA,1 TOWER_BLOCK for 1 units
B $C7EB,1 (nothing) for 1 units
B $C7EB,1 (nothing) for 1 units
B $C7EC,1 TOWER_BLOCK for 1 units
B $C7EC,1 TOWER_BLOCK for 1 units
B $C7ED,1 (nothing) for 1 units
B $C7ED,1 (nothing) for 1 units
B $C7EE,1 TOWER_BLOCK for 1 units
B $C7EE,1 TOWER_BLOCK for 1 units
B $C7EF,1 (nothing) for 1 units
B $C7EF,1 (nothing) for 1 units
B $C7F0,1 TOWER_BLOCK for 1 units
B $C7F0,1 TOWER_BLOCK for 1 units
B $C7F1,1 (nothing) for 1 units
B $C7F1,1 (nothing) for 1 units
B $C7F2,1 TOWER_BLOCK for 1 units
B $C7F2,1 TOWER_BLOCK for 1 units
B $C7F3,1 (nothing) for 1 units
B $C7F3,1 (nothing) for 1 units
B $C7F4,1 TOWER_BLOCK for 1 units
B $C7F4,1 TOWER_BLOCK for 1 units
B $C7F5,1 (nothing) for 1 units
B $C7F5,1 (nothing) for 1 units
B $C7F6,1 TOWER_BLOCK for 1 units
B $C7F6,1 TOWER_BLOCK for 1 units
B $C7F7,1 (nothing) for 1 units
B $C7F7,1 (nothing) for 1 units
B $C7F8,1 TOWER_BLOCK for 1 units
B $C7F8,1 TOWER_BLOCK for 1 units
B $C7F9,1 (nothing) for 1 units
B $C7F9,1 (nothing) for 1 units
B $C7FA,1 TOWER_BLOCK for 1 units
B $C7FA,1 TOWER_BLOCK for 1 units
B $C7FB,1 (nothing) for 1 units
B $C7FB,1 (nothing) for 1 units
B $C7FC,1 TOWER_BLOCK for 1 units
B $C7FC,1 TOWER_BLOCK for 1 units
B $C7FD,1 (nothing) for 5 units
B $C7FD,1 (nothing) for 5 units
B $C7FE,1 TOWER_BLOCK for 1 units
B $C7FE,1 TOWER_BLOCK for 1 units
B $C7FF,1 (nothing) for 1 units
B $C7FF,1 (nothing) for 1 units
B $C800,1 TOWER_BLOCK for 1 units
B $C800,1 TOWER_BLOCK for 1 units
B $C801,1 (nothing) for 3 units
B $C801,1 (nothing) for 3 units
B $C802,1 TOWER_BLOCK for 1 units
B $C802,1 TOWER_BLOCK for 1 units
B $C803,1 (nothing) for 1 units
B $C803,1 (nothing) for 1 units
B $C804,1 TOWER_BLOCK for 1 units
B $C804,1 TOWER_BLOCK for 1 units
B $C805,1 (nothing) for 1 units
B $C805,1 (nothing) for 1 units
B $C806,1 TOWER_BLOCK for 1 units
B $C806,1 TOWER_BLOCK for 1 units
B $C807,1 (nothing) for 1 units
B $C807,1 (nothing) for 1 units
B $C808,1 TOWER_BLOCK for 1 units
B $C808,1 TOWER_BLOCK for 1 units
B $C809,1 (nothing) for 1 units
B $C809,1 (nothing) for 1 units
B $C80A,1 TOWER_BLOCK for 1 units
B $C80A,1 TOWER_BLOCK for 1 units
B $C80B,1 (nothing) for 7 units
B $C80B,1 (nothing) for 7 units
B $C80C,1 TOWER_BLOCK for 1 units
B $C80C,1 TOWER_BLOCK for 1 units
B $C80D,1 (nothing) for 1 units
B $C80D,1 (nothing) for 1 units
B $C80E,1 TOWER_BLOCK for 1 units
B $C80E,1 TOWER_BLOCK for 1 units
B $C80F,1 (nothing) for 1 units
B $C80F,1 (nothing) for 1 units
B $C810,1 TOWER_BLOCK for 1 units
B $C810,1 TOWER_BLOCK for 1 units
B $C811,1 (nothing) for 1 units
B $C811,1 (nothing) for 1 units
B $C812,1 TOWER_BLOCK for 1 units
B $C812,1 TOWER_BLOCK for 1 units
B $C813,1 (nothing) for 1 units
B $C813,1 (nothing) for 1 units
B $C814,1 TOWER_BLOCK for 1 units
B $C814,1 TOWER_BLOCK for 1 units
B $C815,1 (nothing) for 1 units
B $C815,1 (nothing) for 1 units
B $C816,1 TOWER_BLOCK for 1 units
B $C816,1 TOWER_BLOCK for 1 units
B $C817,1 (nothing) for 1 units
B $C817,1 (nothing) for 1 units
B $C818,1 TOWER_BLOCK for 1 units
B $C818,1 TOWER_BLOCK for 1 units
B $C819,1 (nothing) for 1 units
B $C819,1 (nothing) for 1 units
B $C81A,1 TOWER_BLOCK for 1 units
B $C81A,1 TOWER_BLOCK for 1 units
B $C81B,1 (nothing) for 5 units
B $C81B,1 (nothing) for 5 units
B $C81C,1 TOWER_BLOCK for 1 units
B $C81C,1 TOWER_BLOCK for 1 units
B $C81D,1 (nothing) for 9 units
B $C81D,1 (nothing) for 9 units
B $C81E,1 TOWER_BLOCK for 1 units
B $C81E,1 TOWER_BLOCK for 1 units
B $C81F,1 (nothing) for 1 units
B $C81F,1 (nothing) for 1 units
B $C820,1 TOWER_BLOCK for 1 units
B $C820,1 TOWER_BLOCK for 1 units
B $C821,1 (nothing) for 1 units
B $C821,1 (nothing) for 1 units
B $C822,1 TOWER_BLOCK for 1 units
B $C822,1 TOWER_BLOCK for 1 units
B $C823,1 (nothing) for 7 units
B $C823,1 (nothing) for 7 units
B $C824,1 SPEED_LIMIT_SIGN for 1 units
B $C824,1 SPEED_LIMIT_SIGN for 1 units
B $C825,1 (nothing) for 13 units
B $C825,1 (nothing) for 13 units
B $C826,1 SPEED_LIMIT_SIGN for 1 units
B $C826,1 SPEED_LIMIT_SIGN for 1 units
B $C827,1 (nothing) for 5 units
B $C827,1 (nothing) for 5 units
B $C828,1 <Esc> Loop
B $C828,1 <Esc> Loop
W $C82A,2 [$C7AD] Target
W $C82A,2 [$C7AD] Target
N $C8E0 LOD
B $C8E0,1 Width (bytes)
B $C8E1,1 Flags
B $C8E2,1 Height (pixels)
W $C8E3,2 [$C95E] Bitmap address
W $C8E5,2 [$C95E] Pre-shifted bitmap address
N $C8E7 LOD
B $C8E7,1 Width (bytes)
B $C8E8,1 Flags
B $C8E9,1 Height (pixels)
W $C8EA,2 [$CA12] Bitmap address
W $C8EC,2 [$CA12] Pre-shifted bitmap address
N $C8EE LOD
B $C8EE,1 Width (bytes)
B $C8EF,1 Flags
B $C8F0,1 Height (pixels)
W $C8F1,2 [$CA62] Bitmap address
W $C8F3,2 [$CA62] Pre-shifted bitmap address
N $C8F5 LOD
B $C8F5,1 Width (bytes)
B $C8F6,1 Flags
B $C8F7,1 Height (pixels)
W $C8F8,2 [$CA62] Bitmap address
W $C8FA,2 [$CA62] Pre-shifted bitmap address
N $C8FC LOD
B $C8FC,1 Width (bytes)
B $C8FD,1 Flags
B $C8FE,1 Height (pixels)
W $C8FF,2 [$CA89] Bitmap address
W $C901,2 [$CA89] Pre-shifted bitmap address
N $C903 LOD
B $C903,1 Width (bytes)
B $C904,1 Flags
B $C905,1 Height (pixels)
W $C906,2 [$CA89] Bitmap address
W $C908,2 [$CAA9] Pre-shifted bitmap address
N $C90A LOD
B $C90A,1 Width (bytes)
B $C90B,1 Flags
B $C90C,1 Height (pixels)
W $C90D,2 [$CAC9] Bitmap address
W $C90F,2 [$CAC9] Pre-shifted bitmap address
N $C911 LOD
B $C911,1 Width (bytes)
B $C912,1 Flags
B $C913,1 Height (pixels)
W $C914,2 [$CB77] Bitmap address
W $C916,2 [$CB77] Pre-shifted bitmap address
N $C918 LOD
B $C918,1 Width (bytes)
B $C919,1 Flags
B $C91A,1 Height (pixels)
W $C91B,2 [$CBC3] Bitmap address
W $C91D,2 [$CBC3] Pre-shifted bitmap address
N $C91F LOD
B $C91F,1 Width (bytes)
B $C920,1 Flags
B $C921,1 Height (pixels)
W $C922,2 [$CBC3] Bitmap address
W $C924,2 [$CBC3] Pre-shifted bitmap address
N $C926 LOD
B $C926,1 Width (bytes)
B $C927,1 Flags
B $C928,1 Height (pixels)
W $C929,2 [$CBED] Bitmap address
W $C92B,2 [$CC0D] Pre-shifted bitmap address
N $C92D LOD
B $C92D,1 Width (bytes)
B $C92E,1 Flags
B $C92F,1 Height (pixels)
W $C930,2 [$CBED] Bitmap address
W $C932,2 [$CC0D] Pre-shifted bitmap address
N $C934 LOD
B $C934,1 Width (bytes)
B $C935,1 Flags
B $C936,1 Height (pixels)
W $C937,2 [$CC2D] Bitmap address
W $C939,2 [$CC2D] Pre-shifted bitmap address
N $C93B LOD
B $C93B,1 Width (bytes)
B $C93C,1 Flags
B $C93D,1 Height (pixels)
W $C93E,2 [$CCE7] Bitmap address
W $C940,2 [$CCE7] Pre-shifted bitmap address
N $C942 LOD
B $C942,1 Width (bytes)
B $C943,1 Flags
B $C944,1 Height (pixels)
W $C945,2 [$CD55] Bitmap address
W $C947,2 [$CD55] Pre-shifted bitmap address
N $C949 LOD
B $C949,1 Width (bytes)
B $C94A,1 Flags
B $C94B,1 Height (pixels)
W $C94C,2 [$CD55] Bitmap address
W $C94E,2 [$CD55] Pre-shifted bitmap address
N $C950 LOD
B $C950,1 Width (bytes)
B $C951,1 Flags
B $C952,1 Height (pixels)
W $C953,2 [$CD85] Bitmap address
W $C955,2 [$CDBB] Pre-shifted bitmap address
N $C957 LOD
B $C957,1 Width (bytes)
B $C958,1 Flags
B $C959,1 Height (pixels)
W $C95A,2 [$CD85] Bitmap address
W $C95C,2 [$CDBB] Pre-shifted bitmap address
B $C95E,180,6 Bitmap data 6 bytes x 30
B $CA12,80,4 Bitmap data 4 bytes x 20
B $CA62,39,3 Bitmap data 3 bytes x 13
B $CA62,39,3 Bitmap data 3 bytes x 13
B $CA89,32,2 Bitmap data (masked) 2 bytes x 8
B $CA89,32,2 Bitmap data (masked) 2 bytes x 8
B $CAA9,32,2 Pre-shifted bitmap data (masked) 2 bytes x 8
B $CAC9,174,6 Bitmap data 6 bytes x 29
B $CB77,76,4 Bitmap data 4 bytes x 19
B $CBC3,42,3 Bitmap data 3 bytes x 14
B $CBC3,42,3 Bitmap data 3 bytes x 14
B $CBED,32,2 Bitmap data (masked) 2 bytes x 8
B $CBED,32,2 Bitmap data (masked) 2 bytes x 8
B $CC0D,32,2 Pre-shifted bitmap data (masked) 2 bytes x 8
B $CC0D,32,2 Pre-shifted bitmap data (masked) 2 bytes x 8
B $CC2D,186,6 Bitmap data 6 bytes x 31
B $CCE7,110,5 Bitmap data 5 bytes x 22
B $CD55,48,3 Bitmap data 3 bytes x 16
B $CD55,48,3 Bitmap data 3 bytes x 16
B $CD85,54,3 Bitmap data (masked) 3 bytes x 9
B $CD85,54,3 Bitmap data (masked) 3 bytes x 9
B $CDBB,54,3 Pre-shifted bitmap data (masked) 3 bytes x 9
B $CDBB,54,3 Pre-shifted bitmap data (masked) 3 bytes x 9
N $CDF1 LOD
B $CDF1,1 Width (bytes)
B $CDF2,1 Flags
B $CDF3,1 Height (pixels)
W $CDF4,2 [$CE45] Bitmap address
W $CDF6,2 [$CE45] Pre-shifted bitmap address
N $CDF8 LOD
B $CDF8,1 Width (bytes)
B $CDF9,1 Flags
B $CDFA,1 Height (pixels)
W $CDFB,2 [$CE59] Bitmap address
W $CDFD,2 [$CE69] Pre-shifted bitmap address
N $CDFF LOD
B $CDFF,1 Width (bytes)
B $CE00,1 Flags
B $CE01,1 Height (pixels)
W $CE02,2 [$CE79] Bitmap address
W $CE04,2 [$CE85] Pre-shifted bitmap address
N $CE06 LOD
B $CE06,1 Width (bytes)
B $CE07,1 Flags
B $CE08,1 Height (pixels)
W $CE09,2 [$CE91] Bitmap address
W $CE0B,2 [$CE95] Pre-shifted bitmap address
N $CE0D LOD
B $CE0D,1 Width (bytes)
B $CE0E,1 Flags
B $CE0F,1 Height (pixels)
W $CE10,2 [$CE91] Bitmap address
W $CE12,2 [$CE95] Pre-shifted bitmap address
N $CE14 LOD
B $CE14,1 Width (bytes)
B $CE15,1 Flags
B $CE16,1 Height (pixels)
W $CE17,2 [$CE99] Bitmap address
W $CE19,2 [$CE9B] Pre-shifted bitmap address
N $CE1B LOD
B $CE1B,1 Width (bytes)
B $CE1C,1 Flags
B $CE1D,1 Height (pixels)
W $CE1E,2 [$CE9D] Bitmap address
W $CE20,2 [$CE9F] Pre-shifted bitmap address
N $CE22 LOD
B $CE22,1 Width (bytes)
B $CE23,1 Flags
B $CE24,1 Height (pixels)
W $CE25,2 [$CE9D] Bitmap address
W $CE27,2 [$CE9F] Pre-shifted bitmap address
N $CE29 LOD
B $CE29,1 Width (bytes)
B $CE2A,1 Flags
B $CE2B,1 Height (pixels)
W $CE2C,2 [$CE9D] Bitmap address
W $CE2E,2 [$CE9F] Pre-shifted bitmap address
N $CE30 LOD
B $CE30,1 Width (bytes)
B $CE31,1 Flags
B $CE32,1 Height (pixels)
W $CE33,2 [$CE9D] Bitmap address
W $CE35,2 [$CE9F] Pre-shifted bitmap address
N $CE37 LOD
B $CE37,1 Width (bytes)
B $CE38,1 Flags
B $CE39,1 Height (pixels)
W $CE3A,2 [$CE9D] Bitmap address
W $CE3C,2 [$CE9F] Pre-shifted bitmap address
N $CE3E LOD
B $CE3E,1 Width (bytes)
B $CE3F,1 Flags
B $CE40,1 Height (pixels)
W $CE41,2 [$CE9D] Bitmap address
W $CE43,2 [$CE9F] Pre-shifted bitmap address
B $CE45,20,2 Bitmap data (masked) 2 bytes x 5
B $CE59,16,2 Bitmap data (masked) 2 bytes x 4
B $CE69,16,2 Pre-shifted bitmap data (masked) 2 bytes x 4
B $CE79,12,2 Bitmap data (masked) 2 bytes x 3
B $CE85,12,2 Pre-shifted bitmap data (masked) 2 bytes x 3
B $CE91,4,1 Bitmap data (masked) 1 bytes x 2
B $CE91,4,1 Bitmap data (masked) 1 bytes x 2
B $CE95,4,1 Pre-shifted bitmap data (masked) 1 bytes x 2
B $CE95,4,1 Pre-shifted bitmap data (masked) 1 bytes x 2
B $CE99,2,1 Bitmap data (masked) 1 bytes x 1
B $CE9B,2,1 Pre-shifted bitmap data (masked) 1 bytes x 1
B $CE9D,2,1 Bitmap data (masked) 1 bytes x 1
B $CE9D,2,1 Bitmap data (masked) 1 bytes x 1
B $CE9D,2,1 Bitmap data (masked) 1 bytes x 1
B $CE9D,2,1 Bitmap data (masked) 1 bytes x 1
B $CE9D,2,1 Bitmap data (masked) 1 bytes x 1
B $CE9D,2,1 Bitmap data (masked) 1 bytes x 1
B $CE9F,2,1 Pre-shifted bitmap data (masked) 1 bytes x 1
B $CE9F,2,1 Pre-shifted bitmap data (masked) 1 bytes x 1
B $CE9F,2,1 Pre-shifted bitmap data (masked) 1 bytes x 1
B $CE9F,2,1 Pre-shifted bitmap data (masked) 1 bytes x 1
B $CE9F,2,1 Pre-shifted bitmap data (masked) 1 bytes x 1
B $CE9F,2,1 Pre-shifted bitmap data (masked) 1 bytes x 1
N $D64F LOD
B $D64F,1 Width (bytes)
B $D650,1 Flags
B $D651,1 Height (pixels)
W $D652,2 [$D679] Bitmap address
W $D654,2 [$D679] Pre-shifted bitmap address
N $D656 LOD
B $D656,1 Width (bytes)
B $D657,1 Flags
B $D658,1 Height (pixels)
W $D659,2 [$D699] Bitmap address
W $D65B,2 [$D699] Pre-shifted bitmap address
N $D65D LOD
B $D65D,1 Width (bytes)
B $D65E,1 Flags
B $D65F,1 Height (pixels)
W $D660,2 [$D6AF] Bitmap address
W $D662,2 [$D6AF] Pre-shifted bitmap address
N $D664 LOD
B $D664,1 Width (bytes)
B $D665,1 Flags
B $D666,1 Height (pixels)
W $D667,2 [$D6AF] Bitmap address
W $D669,2 [$D6AF] Pre-shifted bitmap address
N $D66B LOD
B $D66B,1 Width (bytes)
B $D66C,1 Flags
B $D66D,1 Height (pixels)
W $D66E,2 [$D6B8] Bitmap address
W $D670,2 [$D6B8] Pre-shifted bitmap address
N $D672 LOD
B $D672,1 Width (bytes)
B $D673,1 Flags
B $D674,1 Height (pixels)
W $D675,2 [$D6B8] Bitmap address
W $D677,2 [$D6B8] Pre-shifted bitmap address
B $D679,32,2 Bitmap data 2 bytes x 16
B $D699,22,2 Bitmap data 2 bytes x 11
B $D6AF,9,1 Bitmap data 1 bytes x 9
B $D6AF,9,1 Bitmap data 1 bytes x 9
B $D6B8,7,1 Bitmap data 1 bytes x 7
B $D6B8,7,1 Bitmap data 1 bytes x 7
N $D6BF LOD
B $D6BF,1 Width (bytes)
B $D6C0,1 Flags
B $D6C1,1 Height (pixels)
W $D6C2,2 [$D6E9] Bitmap address
W $D6C4,2 [$D6E9] Pre-shifted bitmap address
N $D6C6 LOD
B $D6C6,1 Width (bytes)
B $D6C7,1 Flags
B $D6C8,1 Height (pixels)
W $D6C9,2 [$D6E9] Bitmap address
W $D6CB,2 [$D6E9] Pre-shifted bitmap address
N $D6CD LOD
B $D6CD,1 Width (bytes)
B $D6CE,1 Flags
B $D6CF,1 Height (pixels)
W $D6D0,2 [$D72D] Bitmap address
W $D6D2,2 [$D72D] Pre-shifted bitmap address
N $D6D4 LOD
B $D6D4,1 Width (bytes)
B $D6D5,1 Flags
B $D6D6,1 Height (pixels)
W $D6D7,2 [$D754] Bitmap address
W $D6D9,2 [$D754] Pre-shifted bitmap address
N $D6DB LOD
B $D6DB,1 Width (bytes)
B $D6DC,1 Flags
B $D6DD,1 Height (pixels)
W $D6DE,2 [$D754] Bitmap address
W $D6E0,2 [$D754] Pre-shifted bitmap address
N $D6E2 LOD
B $D6E2,1 Width (bytes)
B $D6E3,1 Flags
B $D6E4,1 Height (pixels)
W $D6E5,2 [$D766] Bitmap address
W $D6E7,2 [$D782] Pre-shifted bitmap address
B $D6E9,68,4 Bitmap data 4 bytes x 17
B $D6E9,68,4 Bitmap data 4 bytes x 17
B $D72D,39,3 Bitmap data 3 bytes x 13
B $D754,18,2 Bitmap data 2 bytes x 9
B $D754,18,2 Bitmap data 2 bytes x 9
B $D766,28,2 Bitmap data (masked) 2 bytes x 7
B $D782,28,2 Pre-shifted bitmap data (masked) 2 bytes x 7
b $E000 [Stage 4] Horizon graphic
b $E0F0 [Stage 4] Per-stage data
W $E0F0,2 [$C7DE] Address of perp's mugshot attributes
W $E0F2,2 [$CF35] Address of perp's mugshot bitmap
W $E0F4,2 [$9C38] Screen attributes used for the ground colour (a pair of matching bytes)
W $E0F6,2 [$C218] Address of table of LODs for tumbleweeds, barriers.
W $E0F8,2 [$C21A] (points at a handler address)
W $E0FA,2 [$C217] Address of right hand graphics entry/entries (-7 bytes)
W $E0FC,2 [$C22C] (points at a handler address)
W $E0FE,2 [$C259] (points at a handler address)
W $E100,2 [$C256] Address of left hand graphics entry/entries (-7 bytes)
W $E102,2 [$C26B] (points at a handler address)
W $E104,2 [$C139] Address of Nancy's perp description
W $E106,2 [$C1D5] Address of arrest messages
W $E108,2 [$CC6E] Helicopter related 1
W $E10A,2 [$CC7A] Helicopter related 2
w $E10C [Stage 4] Table of addresses of LODs
W $E10C,2 [$6400] Address of LOD of stone/dust?
W $E10E,2 [$6400] Address of LOD of stone/dust?
W $E110,2 [$C7F2] Address of LOD of car (the perp's car)
W $E112,2 [$C846] Address of LOD of car (a Lambo in S1)
W $E114,2 [$C81C] Address of LOD of car (a truck in S1)
W $E116,2 [$C846] Address of LOD of car (a Lambo in S1)
W $E118,2 [$C7F2] Address of LOD of car (a generic car in S1)
b $E11A [Stage 4] Per-stage difficulty settings
B $E11A,1 How often cars spawn. Lower values spawn cars more often.
B $E11B,1 Smash related parameter
B $E11C,1 Smash related parameter
w $E11D [Stage 4] Per-stage setup data
W $E11D,2 road_pos
W $E11F,2 [$C29B] Address of start stretch, curvature
W $E121,2 [$C2C3] Address of start stretch, height
W $E123,2 [$C303] Address of start stretch, lanes
W $E125,2 [$C3CD] Address of start stretch, right-side objects
W $E127,2 [$C335] Address of start stretch, left-side objects
W $E129,2 [$C30D] Address of start stretch, hazards
w $E12B [Stage 4] Per-stage attract mode data
W $E12B,2 road_pos
W $E12D,2 [$C576] Address of loop section, curvature
W $E12F,2 [$C599] Address of loop section, height
W $E131,2 [$C5D2] Address of loop section, lanes
W $E133,2 [$C6B6] Address of loop section, right-side objects
W $E135,2 [$C623] Address of loop section, left-side objects
W $E137,2 [$C5FA] Address of loop section, hazards
b $E139 [Stage 4] Nancy's perp description
B $E139,1 Character identifier (0/1/2/3 = Pilot/Nancy/Raymond/Tony)
W $E13A,2 [$C145] Perp description pointer
W $E13C,2 [$C16D] Perp description pointer
W $E13E,2 [$C192] Perp description pointer
W $E140,2 [$C1B8] Perp description pointer
B $E142,1 terminator?
T $E145 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $E16D "EMERGENCY HERE, THE L.A. KIDNAPPER IS"
T $E192 "SPEEDING TOWARDS THE OUTSKIRTS OF TOWN"
T $E1B8 "IN A BLUE TWO SEATER... OVER."
b $E1D5 [Stage 4] Arrest messages
B $E1D5,1 ?frame delay until first message
B $E1D6,1 ?frame delay until next message
B $E1D7,1 Flags
B $E1D8,1 Attribute
W $E1D9,2 Back buffer address
W $E1DB,2 Attribute address
T $E1DD "OK! YOU ARE UNDER ARREST ON"
B $E1F8,1 ?frame delay until next message
B $E1F9,1 Flags
B $E1FA,1 Attribute
W $E1FB,2 Back buffer address
W $E1FD,2 Attribute address
T $E1FF "SUSPICION OF KIDNAPPING"
B $E216,1 ?frame delay until next message
B $E217,1 Stop
b $E218 [Stage 4] Hittable hazards
B $E218,1 ?id
W $E219,2 [$D4D8] Address of LODs
B $E21B,1 ?id
W $E21C,2 [$D615] Address of LODs
b $E21E [Stage 4] Graphic definition
N $E21E Definition
B $E21E,1 Hit coord max/min (R/L)
B $E21F,1 Hit coord min/max (R/L)
B $E220,1 ?how far to push hero car away if hit
W $E221,2 Argument for routine passed in #REGde
W $E223,2 Address of routine draw_tunnel_light_right
N $E225 Definition
B $E225,1 Hit coord max/min (R/L)
B $E226,1 Hit coord min/max (R/L)
B $E227,1 ?how far to push hero car away if hit
W $E228,2 Argument for routine passed in #REGde
W $E22A,2 Address of routine TBD
N $E22C Definition
B $E22C,1 Hit coord max/min (R/L)
B $E22D,1 Hit coord min/max (R/L)
B $E22E,1 ?how far to push hero car away if hit
W $E22F,2 Argument for routine passed in #REGde
W $E231,2 Address of routine draw_stretchy_object_right
N $E233 Definition
B $E233,1 Hit coord max/min (R/L)
B $E234,1 Hit coord min/max (R/L)
B $E235,1 ?how far to push hero car away if hit
W $E236,2 Argument for routine passed in #REGde
W $E238,2 Address of routine draw_stretchy_object_right
N $E23A Definition
B $E23A,1 Hit coord max/min (R/L)
B $E23B,1 Hit coord min/max (R/L)
B $E23C,1 ?how far to push hero car away if hit
W $E23D,2 Argument for routine passed in #REGde
W $E23F,2 Address of routine draw_stretchy_object_right
N $E241 Definition
B $E241,1 Hit coord max/min (R/L)
B $E242,1 Hit coord min/max (R/L)
B $E243,1 ?how far to push hero car away if hit
W $E244,2 Argument for routine passed in #REGde
W $E246,2 Address of routine draw_stretchy_object_right
N $E248 Definition
B $E248,1 Hit coord max/min (R/L)
B $E249,1 Hit coord min/max (R/L)
B $E24A,1 ?how far to push hero car away if hit
W $E24B,2 Argument for routine passed in #REGde
W $E24D,2 Address of routine draw_stretchy_object_right
b $E25D [Stage 4] Graphic definition
N $E25D Definition
B $E25D,1 Hit coord max/min (R/L)
B $E25E,1 Hit coord min/max (R/L)
B $E25F,1 ?how far to push hero car away if hit
W $E260,2 Argument for routine passed in #REGde
W $E262,2 Address of routine draw_tunnel_light_left
N $E264 Definition
B $E264,1 Hit coord max/min (R/L)
B $E265,1 Hit coord min/max (R/L)
B $E266,1 ?how far to push hero car away if hit
W $E267,2 Argument for routine passed in #REGde
W $E269,2 Address of routine TBD
N $E26B Definition
B $E26B,1 Hit coord max/min (R/L)
B $E26C,1 Hit coord min/max (R/L)
B $E26D,1 ?how far to push hero car away if hit
W $E26E,2 Argument for routine passed in #REGde
W $E270,2 Address of routine draw_stretchy_object_left
N $E272 Definition
B $E272,1 Hit coord max/min (R/L)
B $E273,1 Hit coord min/max (R/L)
B $E274,1 ?how far to push hero car away if hit
W $E275,2 Argument for routine passed in #REGde
W $E277,2 Address of routine draw_stretchy_object_left
N $E279 Definition
B $E279,1 Hit coord max/min (R/L)
B $E27A,1 Hit coord min/max (R/L)
B $E27B,1 ?how far to push hero car away if hit
W $E27C,2 Argument for routine passed in #REGde
W $E27E,2 Address of routine draw_stretchy_object_left
N $E280 Definition
B $E280,1 Hit coord max/min (R/L)
B $E281,1 Hit coord min/max (R/L)
B $E282,1 ?how far to push hero car away if hit
W $E283,2 Argument for routine passed in #REGde
W $E285,2 Address of routine draw_stretchy_object_left
N $E287 Definition
B $E287,1 Hit coord max/min (R/L)
B $E288,1 Hit coord min/max (R/L)
B $E289,1 ?how far to push hero car away if hit
W $E28A,2 Argument for routine passed in #REGde
W $E28C,2 Address of routine draw_stretchy_object_left
b $E29C [Stage 4] Map curvature data
B $E29C,1 Curve Straight for 54 units
B $E2A0,1 Curve Right Hard for 11 units
B $E2A1,1 Curve Right Very Hard for 6 units
B $E2A2,1 Curve Right Hard for 10 units
B $E2A3,1 Curve Straight for 40 units
B $E2A6,1 Curve Left for 50 units
B $E2AA,1 Curve Right for 50 units
B $E2AE,1 Curve Straight for 39 units
B $E2B1,1 Curve Left Hard for 12 units
B $E2B2,1 Curve Left Very Hard for 7 units
B $E2B3,1 Curve Left Hard for 22 units
B $E2B5,1 Curve Straight for 34 units
B $E2B8,1 Curve Right for 46 units
B $E2BC,1 Curve Straight for 19 units
B $E2BE,1 <Esc> Split
W $E2C0,2 [$C442] Left target
W $E2C2,2 [$C4C4] Right target
b $E2C4 [Stage 4] Map height data
B $E2C4,1 Level Road for 40 units
B $E2C7,1 Going Up 3 for 2 units
B $E2C8,1 Going Up 5 for 5 units
B $E2C9,1 Going Up 7 for 1 units
B $E2CA,1 Going Down 5 for 2 units
B $E2CB,1 Going Down 3 for 11 units
B $E2CC,1 Going Down 1 for 5 units
B $E2CD,1 Level Road for 27 units
B $E2CF,1 Going Down 3 for 1 units
B $E2D0,1 Going Down 5 for 1 units
B $E2D1,1 Going Down 7 for 3 units
B $E2D2,1 Going Down 5 for 1 units
B $E2D3,1 Going Down 3 for 1 units
B $E2D4,1 Going Down 1 for 1 units
B $E2D5,1 Level Road for 5 units
B $E2D6,1 Going Up 1 for 1 units
B $E2D7,1 Going Up 3 for 1 units
B $E2D8,1 Going Up 5 for 1 units
B $E2D9,1 Going Up 7 for 2 units
B $E2DA,1 Going Up 5 for 2 units
B $E2DB,1 Going Up 3 for 2 units
B $E2DC,1 Going Up 1 for 1 units
B $E2DD,1 Level Road for 95 units
B $E2E4,1 Going Up 1 for 1 units
B $E2E5,1 Going Up 3 for 1 units
B $E2E6,1 Going Up 5 for 1 units
B $E2E7,1 Going Up 7 for 8 units
B $E2E8,1 Going Up 5 for 2 units
B $E2E9,1 Going Up 3 for 1 units
B $E2EA,1 Going Up 1 for 1 units
B $E2EB,1 Level Road for 1 units
B $E2EC,1 Going Down 1 for 1 units
B $E2ED,1 Going Down 3 for 2 units
B $E2EE,1 Going Down 5 for 1 units
B $E2EF,1 Going Down 7 for 8 units
B $E2F0,1 Going Down 5 for 1 units
B $E2F1,1 Going Down 3 for 2 units
B $E2F2,1 Going Down 1 for 5 units
B $E2F3,1 Level Road for 153 units
B $E2FE,1 <Esc> Split
W $E300,2 [$C45A] Left target
W $E302,2 [$C4DA] Right target
b $E304 [Stage 4] Map lanes data
B $E304,1 4 Lanes              [||||] {00} for 400 units
B $E308,1 <Esc> Split
W $E30A,2 [$C46C] Left target
W $E30C,2 [$C4EC] Right target
b $E30E [Stage 4] Map hazards data
B $E30E,1 Wait for 31 units
B $E30F,1 Stop Spawning Barriers 6?
B $E311,1 Wait for 4 units
B $E312,1 Stop Spawning Barriers 3?
B $E314,1 Wait for 29 units
B $E315,1 Unknown command 5
B $E317,1 Wait for 2 units
B $E318,1 Stop Spawning Barriers 3?
B $E31A,1 Wait for 29 units
B $E31B,1 Unknown command 4
B $E31D,1 Wait for 2 units
B $E31E,1 Stop Spawning Barriers 3?
B $E320,1 Wait for 21 units
B $E321,1 Unknown command 12
B $E323,1 Wait for 20 units
B $E324,1 Unknown command 5
B $E326,1 Wait for 2 units
B $E327,1 Stop Spawning Barriers 3?
B $E329,1 Wait for 53 units
B $E32A,1 Disable Car Spawning
B $E32C,1 Wait for 6 units
B $E32D,1 Set Floating Arrow to Right
B $E32F,1 Wait for 1 units
B $E330,1 <Esc> Split
W $E332,2 [$C480] Left target
W $E334,2 [$C500] Right target
b $E336 [Stage 4] Map left object data
B $E336,1 STREET_LAMP for 1 units
B $E337,1 (nothing) for 1 units
B $E338,1 STREET_LAMP for 1 units
B $E339,1 (nothing) for 1 units
B $E33A,1 STREET_LAMP for 1 units
B $E33B,1 (nothing) for 1 units
B $E33C,1 STREET_LAMP for 1 units
B $E33D,1 (nothing) for 1 units
B $E33E,1 STREET_LAMP for 1 units
B $E33F,1 (nothing) for 1 units
B $E340,1 STREET_LAMP for 1 units
B $E341,1 (nothing) for 1 units
B $E342,1 STREET_LAMP for 1 units
B $E343,1 (nothing) for 1 units
B $E344,1 STREET_LAMP for 1 units
B $E345,1 (nothing) for 1 units
B $E346,1 STREET_LAMP for 1 units
B $E347,1 (nothing) for 1 units
B $E348,1 STREET_LAMP for 1 units
B $E349,1 (nothing) for 1 units
B $E34A,1 STREET_LAMP for 1 units
B $E34B,1 FAR_COLUMN for 1 units
B $E34C,1 STREET_LAMP for 1 units
B $E34D,1 FAR_COLUMN for 1 units
B $E34E,1 STREET_LAMP for 1 units
B $E34F,1 (nothing) for 1 units
B $E350,1 STREET_LAMP for 1 units
B $E351,1 (nothing) for 1 units
B $E352,1 STREET_LAMP for 1 units
B $E353,1 (nothing) for 2 units
B $E354,1 FAR_COLUMN for 1 units
B $E355,1 (nothing) for 2 units
B $E356,1 STREET_LAMP for 1 units
B $E357,1 (nothing) for 1 units
B $E358,1 STREET_LAMP for 1 units
B $E359,1 (nothing) for 1 units
B $E35A,1 STREET_LAMP for 1 units
B $E35B,1 (nothing) for 3 units
B $E35C,1 STREET_LAMP for 1 units
B $E35D,1 (nothing) for 3 units
B $E35E,1 STREET_LAMP for 1 units
B $E35F,1 (nothing) for 11 units
B $E360,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E361,1 (nothing) for 1 units
B $E362,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E363,1 (nothing) for 1 units
B $E364,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E365,1 (nothing) for 1 units
B $E366,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E367,1 (nothing) for 1 units
B $E368,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E369,1 (nothing) for 3 units
B $E36A,1 STREET_LAMP for 1 units
B $E36B,1 (nothing) for 1 units
B $E36C,1 STREET_LAMP for 1 units
B $E36D,1 (nothing) for 1 units
B $E36E,1 STREET_LAMP for 1 units
B $E36F,1 (nothing) for 1 units
B $E370,1 STREET_LAMP for 1 units
B $E371,1 (nothing) for 1 units
B $E372,1 STREET_LAMP for 1 units
B $E373,1 (nothing) for 1 units
B $E374,1 STREET_LAMP for 1 units
B $E375,1 (nothing) for 1 units
B $E376,1 STREET_LAMP for 1 units
B $E377,1 (nothing) for 1 units
B $E378,1 STREET_LAMP for 1 units
B $E379,1 (nothing) for 1 units
B $E37A,1 STREET_LAMP for 1 units
B $E37B,1 (nothing) for 1 units
B $E37C,1 STREET_LAMP for 1 units
B $E37D,1 (nothing) for 3 units
B $E37E,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E37F,1 (nothing) for 1 units
B $E380,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E381,1 (nothing) for 1 units
B $E382,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E383,1 (nothing) for 1 units
B $E384,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E385,1 (nothing) for 1 units
B $E386,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E387,1 (nothing) for 1 units
B $E388,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E389,1 (nothing) for 1 units
B $E38A,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E38B,1 (nothing) for 1 units
B $E38C,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E38D,1 (nothing) for 5 units
B $E38E,1 PILE_OF_ROCKS for 1 units
B $E38F,1 (nothing) for 1 units
B $E390,1 PILE_OF_ROCKS for 1 units
B $E391,1 (nothing) for 1 units
B $E392,1 PILE_OF_ROCKS for 1 units
B $E393,1 (nothing) for 1 units
B $E394,1 PILE_OF_ROCKS for 1 units
B $E395,1 (nothing) for 1 units
B $E396,1 PILE_OF_ROCKS for 1 units
B $E397,1 (nothing) for 1 units
B $E398,1 PILE_OF_ROCKS for 1 units
B $E399,1 (nothing) for 1 units
B $E39A,1 PILE_OF_ROCKS for 1 units
B $E39B,1 (nothing) for 5 units
B $E39C,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E39D,1 (nothing) for 1 units
B $E39E,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E39F,1 (nothing) for 1 units
B $E3A0,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E3A1,1 (nothing) for 1 units
B $E3A2,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E3A3,1 (nothing) for 1 units
B $E3A4,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E3A5,1 (nothing) for 13 units
B $E3A6,1 STREET_LAMP for 1 units
B $E3A7,1 (nothing) for 1 units
B $E3A8,1 STREET_LAMP for 1 units
B $E3A9,1 (nothing) for 1 units
B $E3AA,1 STREET_LAMP for 1 units
B $E3AB,1 (nothing) for 1 units
B $E3AC,1 STREET_LAMP for 1 units
B $E3AD,1 (nothing) for 1 units
B $E3AE,1 STREET_LAMP for 1 units
B $E3AF,1 (nothing) for 3 units
B $E3B0,1 STREET_LAMP for 1 units
B $E3B1,1 (nothing) for 1 units
B $E3B2,1 STREET_LAMP for 1 units
B $E3B3,1 (nothing) for 3 units
B $E3B4,1 STREET_LAMP for 1 units
B $E3B5,1 (nothing) for 1 units
B $E3B6,1 STREET_LAMP for 1 units
B $E3B7,1 (nothing) for 3 units
B $E3B8,1 STREET_LAMP for 1 units
B $E3B9,1 (nothing) for 1 units
B $E3BA,1 STREET_LAMP for 1 units
B $E3BB,1 (nothing) for 3 units
B $E3BC,1 PILE_OF_ROCKS for 1 units
B $E3BD,1 (nothing) for 1 units
B $E3BE,1 PILE_OF_ROCKS for 1 units
B $E3BF,1 (nothing) for 1 units
B $E3C0,1 PILE_OF_ROCKS for 1 units
B $E3C1,1 (nothing) for 1 units
B $E3C2,1 PILE_OF_ROCKS for 1 units
B $E3C3,1 (nothing) for 1 units
B $E3C4,1 PILE_OF_ROCKS for 1 units
B $E3C5,1 (nothing) for 1 units
B $E3C6,1 PILE_OF_ROCKS for 1 units
B $E3C7,1 (nothing) for 7 units
B $E3C8,1 <Esc> Split
W $E3CA,2 [$C48B] Left target
W $E3CC,2 [$C50B] Right target
b $E3CE [Stage 4] Map right object data
B $E3CE,1 STREET_LAMP for 1 units
B $E3CF,1 (nothing) for 1 units
B $E3D0,1 STREET_LAMP for 1 units
B $E3D1,1 (nothing) for 1 units
B $E3D2,1 STREET_LAMP for 1 units
B $E3D3,1 (nothing) for 1 units
B $E3D4,1 STREET_LAMP for 1 units
B $E3D5,1 FAR_COLUMN for 1 units
B $E3D6,1 STREET_LAMP for 1 units
B $E3D7,1 (nothing) for 1 units
B $E3D8,1 STREET_LAMP for 1 units
B $E3D9,1 (nothing) for 1 units
B $E3DA,1 STREET_LAMP for 1 units
B $E3DB,1 (nothing) for 1 units
B $E3DC,1 STREET_LAMP for 1 units
B $E3DD,1 FAR_COLUMN for 1 units
B $E3DE,1 STREET_LAMP for 1 units
B $E3DF,1 FAR_COLUMN for 1 units
B $E3E0,1 STREET_LAMP for 1 units
B $E3E1,1 (nothing) for 1 units
B $E3E2,1 STREET_LAMP for 1 units
B $E3E3,1 (nothing) for 1 units
B $E3E4,1 STREET_LAMP for 1 units
B $E3E5,1 (nothing) for 1 units
B $E3E6,1 STREET_LAMP for 1 units
B $E3E7,1 (nothing) for 1 units
B $E3E8,1 STREET_LAMP for 1 units
B $E3E9,1 (nothing) for 1 units
B $E3EA,1 STREET_LAMP for 1 units
B $E3EB,1 (nothing) for 1 units
B $E3EC,1 STREET_LAMP for 1 units
B $E3ED,1 (nothing) for 1 units
B $E3EE,1 STREET_LAMP for 1 units
B $E3EF,1 (nothing) for 3 units
B $E3F0,1 STREET_LAMP for 1 units
B $E3F1,1 (nothing) for 3 units
B $E3F2,1 STREET_LAMP for 1 units
B $E3F3,1 (nothing) for 5 units
B $E3F4,1 STREET_LAMP for 1 units
B $E3F5,1 (nothing) for 3 units
B $E3F6,1 STREET_LAMP for 1 units
B $E3F7,1 (nothing) for 3 units
B $E3F8,1 STREET_LAMP for 1 units
B $E3F9,1 (nothing) for 11 units
B $E3FA,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E3FB,1 (nothing) for 1 units
B $E3FC,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E3FD,1 (nothing) for 1 units
B $E3FE,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E3FF,1 (nothing) for 1 units
B $E400,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E401,1 (nothing) for 1 units
B $E402,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E403,1 (nothing) for 1 units
B $E404,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E405,1 (nothing) for 1 units
B $E406,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E407,1 (nothing) for 1 units
B $E408,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E409,1 (nothing) for 1 units
B $E40A,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E40B,1 (nothing) for 49 units
B $E40F,1 PILE_OF_ROCKS for 1 units
B $E410,1 (nothing) for 1 units
B $E411,1 PILE_OF_ROCKS for 1 units
B $E412,1 (nothing) for 1 units
B $E413,1 PILE_OF_ROCKS for 1 units
B $E414,1 (nothing) for 3 units
B $E415,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E416,1 (nothing) for 1 units
B $E417,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E418,1 (nothing) for 1 units
B $E419,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E41A,1 (nothing) for 1 units
B $E41B,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E41C,1 (nothing) for 1 units
B $E41D,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E41E,1 (nothing) for 1 units
B $E41F,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E420,1 (nothing) for 3 units
B $E421,1 PILE_OF_ROCKS for 1 units
B $E422,1 (nothing) for 1 units
B $E423,1 PILE_OF_ROCKS for 1 units
B $E424,1 (nothing) for 1 units
B $E425,1 PILE_OF_ROCKS for 1 units
B $E426,1 (nothing) for 3 units
B $E427,1 PILE_OF_ROCKS for 1 units
B $E428,1 (nothing) for 1 units
B $E429,1 PILE_OF_ROCKS for 1 units
B $E42A,1 (nothing) for 1 units
B $E42B,1 PILE_OF_ROCKS for 1 units
B $E42C,1 (nothing) for 1 units
B $E42D,1 PILE_OF_ROCKS for 1 units
B $E42E,1 (nothing) for 1 units
B $E42F,1 PILE_OF_ROCKS for 1 units
B $E430,1 (nothing) for 1 units
B $E431,1 PILE_OF_ROCKS for 1 units
B $E432,1 (nothing) for 17 units
B $E434,1 PILE_OF_ROCKS for 1 units
B $E435,1 (nothing) for 1 units
B $E436,1 PILE_OF_ROCKS for 1 units
B $E437,1 (nothing) for 1 units
B $E438,1 PILE_OF_ROCKS for 1 units
B $E439,1 (nothing) for 1 units
B $E43A,1 PILE_OF_ROCKS for 1 units
B $E43B,1 (nothing) for 3 units
B $E43C,1 <Esc> Split
W $E43E,2 [$C4B4] Left target
W $E440,2 [$C54D] Right target
b $E442 [Stage 4] Map curvature data
B $E442,1 Curve Straight for 35 units
B $E445,1 Curve Left Hard for 12 units
B $E446,1 Curve Right for 1 units
B $E447,1 Curve Right Hard for 10 units
B $E448,1 Curve Right for 6 units
B $E449,1 Curve Straight for 8 units
B $E44A,1 Curve Left for 16 units
B $E44C,1 Curve Right for 25 units
B $E44E,1 Curve Right Hard for 15 units
B $E44F,1 Curve Right for 26 units
B $E451,1 Curve Straight for 14 units
B $E452,1 Curve Right Hard for 10 units
B $E453,1 Curve Right Very Hard for 8 units
B $E454,1 Curve Right Hard for 7 units
B $E455,1 Curve Straight for 11 units
B $E456,1 <Esc> Jump
W $E458,2 [$C577] Target
b $E45A [Stage 4] Map height data
B $E45A,1 Level Road for 204 units
B $E468,1 <Esc> Jump
W $E46A,2 [$C59A] Target
b $E46C [Stage 4] Map lanes data
B $E46C,1 4 Lanes              [||||] {00} for 2 units
B $E46E,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E470,1 3 Lanes L            [|||]  {81} for 8 units
B $E472,1 Tunnel start                {45} for 102 units
B $E474,1 Tunnel cont/end?            {59} for 2 units
B $E476,1 3 Lanes L            [|||]  {81} for 18 units
B $E478,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E47A,1 4 Lanes              [||||] {00} for 68 units
B $E47C,1 <Esc> Jump
W $E47E,2 [$C5D3] Target
b $E480 [Stage 4] Map hazards data
B $E480,1 Wait for 10 units
B $E481,1 Unknown command F
B $E483,1 Wait for 2 units
B $E484,1 Enable Car Spawning
B $E486,1 Wait for 90 units
B $E487,1 <Esc> Jump
W $E489,2 [$C5FB] Target
b $E48B [Stage 4] Map left object data
B $E48B,1 (nothing) for 2 units
B $E48C,1 STREET_LAMP for 1 units
B $E48D,1 (nothing) for 1 units
B $E48E,1 STREET_LAMP for 1 units
B $E48F,1 (nothing) for 2 units
B $E490,1 TUNNEL_LIGHT for 51 units
B $E494,1 (nothing) for 16 units
B $E496,1 PILE_OF_ROCKS for 1 units
B $E497,1 (nothing) for 1 units
B $E498,1 PILE_OF_ROCKS for 1 units
B $E499,1 (nothing) for 1 units
B $E49A,1 PILE_OF_ROCKS for 1 units
B $E49B,1 (nothing) for 1 units
B $E49C,1 PILE_OF_ROCKS for 1 units
B $E49D,1 (nothing) for 1 units
B $E49E,1 PILE_OF_ROCKS for 1 units
B $E49F,1 (nothing) for 1 units
B $E4A0,1 PILE_OF_ROCKS for 1 units
B $E4A1,1 (nothing) for 1 units
B $E4A2,1 PILE_OF_ROCKS for 1 units
B $E4A3,1 (nothing) for 1 units
B $E4A4,1 PILE_OF_ROCKS for 1 units
B $E4A5,1 (nothing) for 1 units
B $E4A6,1 PILE_OF_ROCKS for 1 units
B $E4A7,1 (nothing) for 1 units
B $E4A8,1 PILE_OF_ROCKS for 1 units
B $E4A9,1 (nothing) for 1 units
B $E4AA,1 PILE_OF_ROCKS for 1 units
B $E4AB,1 (nothing) for 3 units
B $E4AC,1 PILE_OF_ROCKS for 1 units
B $E4AD,1 (nothing) for 1 units
B $E4AE,1 PILE_OF_ROCKS for 1 units
B $E4AF,1 (nothing) for 1 units
B $E4B0,1 <Esc> Jump
W $E4B2,2 [$C624] Target
b $E4B4 [Stage 4] Map right object data
B $E4B4,1 (nothing) for 2 units
B $E4B5,1 STREET_LAMP for 1 units
B $E4B6,1 (nothing) for 1 units
B $E4B7,1 STREET_LAMP for 1 units
B $E4B8,1 (nothing) for 2 units
B $E4B9,1 TUNNEL_LIGHT for 51 units
B $E4BD,1 (nothing) for 44 units
B $E4C0,1 <Esc> Jump
W $E4C2,2 [$C6B7] Target
b $E4C4 [Stage 4] Map curvature data
B $E4C4,1 Curve Straight for 12 units
B $E4C5,1 Curve Left for 8 units
B $E4C6,1 Curve Left Hard for 16 units
B $E4C8,1 Curve Left for 18 units
B $E4CA,1 Curve Straight for 30 units
B $E4CC,1 Curve Right Hard for 12 units
B $E4CD,1 Curve Right Very Hard for 13 units
B $E4CE,1 Curve Right Hard for 12 units
B $E4CF,1 Curve Right for 33 units
B $E4D2,1 Curve Straight for 48 units
B $E4D6,1 <Esc> Jump
W $E4D8,2 [$C577] Target
b $E4DA [Stage 4] Map height data
B $E4DA,1 Level Road for 202 units
B $E4E8,1 <Esc> Jump
W $E4EA,2 [$C59A] Target
b $E4EC [Stage 4] Map lanes data
B $E4EC,1 4 Lanes              [||||] {00} for 88 units
B $E4EE,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E4F0,1 3 Lanes L            [|||]  {81} for 20 units
B $E4F2,1 Tunnel start                {45} for 52 units
B $E4F4,1 Tunnel cont/end?            {59} for 2 units
B $E4F6,1 3 Lanes L            [|||]  {81} for 10 units
B $E4F8,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E4FA,1 4 Lanes              [||||] {00} for 26 units
B $E4FC,1 <Esc> Jump
W $E4FE,2 [$C5D3] Target
b $E500 [Stage 4] Map hazards data
B $E500,1 Wait for 10 units
B $E501,1 Enable Car Spawning
B $E503,1 Wait for 36 units
B $E504,1 Unknown command F
B $E506,1 Wait for 55 units
B $E507,1 <Esc> Jump
W $E509,2 [$C5FB] Target
b $E50B [Stage 4] Map left object data
B $E50B,1 PILE_OF_ROCKS for 1 units
B $E50C,1 (nothing) for 1 units
B $E50D,1 PILE_OF_ROCKS for 1 units
B $E50E,1 (nothing) for 1 units
B $E50F,1 PILE_OF_ROCKS for 1 units
B $E510,1 (nothing) for 1 units
B $E511,1 PILE_OF_ROCKS for 1 units
B $E512,1 (nothing) for 1 units
B $E513,1 PILE_OF_ROCKS for 1 units
B $E514,1 (nothing) for 1 units
B $E515,1 PILE_OF_ROCKS for 1 units
B $E516,1 (nothing) for 1 units
B $E517,1 PILE_OF_ROCKS for 1 units
B $E518,1 (nothing) for 1 units
B $E519,1 PILE_OF_ROCKS for 1 units
B $E51A,1 (nothing) for 1 units
B $E51B,1 PILE_OF_ROCKS for 1 units
B $E51C,1 (nothing) for 1 units
B $E51D,1 PILE_OF_ROCKS for 1 units
B $E51E,1 (nothing) for 1 units
B $E51F,1 PILE_OF_ROCKS for 1 units
B $E520,1 (nothing) for 3 units
B $E521,1 PILE_OF_ROCKS for 1 units
B $E522,1 (nothing) for 3 units
B $E523,1 PILE_OF_ROCKS for 1 units
B $E524,1 (nothing) for 3 units
B $E525,1 PILE_OF_ROCKS for 1 units
B $E526,1 (nothing) for 1 units
B $E527,1 PILE_OF_ROCKS for 1 units
B $E528,1 (nothing) for 7 units
B $E529,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E52A,1 (nothing) for 1 units
B $E52B,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E52C,1 (nothing) for 1 units
B $E52D,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E52E,1 (nothing) for 1 units
B $E52F,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E530,1 (nothing) for 1 units
B $E531,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E532,1 (nothing) for 1 units
B $E533,1 STREET_LAMP for 1 units
B $E534,1 (nothing) for 3 units
B $E535,1 TUNNEL_LIGHT for 26 units
B $E537,1 (nothing) for 2 units
B $E538,1 STREET_LAMP for 1 units
B $E539,1 (nothing) for 1 units
B $E53A,1 STREET_LAMP for 1 units
B $E53B,1 (nothing) for 1 units
B $E53C,1 STREET_LAMP for 1 units
B $E53D,1 (nothing) for 1 units
B $E53E,1 STREET_LAMP for 1 units
B $E53F,1 (nothing) for 1 units
B $E540,1 STREET_LAMP for 1 units
B $E541,1 (nothing) for 1 units
B $E542,1 STREET_LAMP for 1 units
B $E543,1 (nothing) for 1 units
B $E544,1 STREET_LAMP for 1 units
B $E545,1 (nothing) for 1 units
B $E546,1 STREET_LAMP for 1 units
B $E547,1 (nothing) for 1 units
B $E548,1 STREET_LAMP for 1 units
B $E549,1 <Esc> Jump
W $E54B,2 [$C624] Target
b $E54D [Stage 4] Map right object data
B $E54D,1 (nothing) for 12 units
B $E54E,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E54F,1 (nothing) for 1 units
B $E550,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E551,1 (nothing) for 1 units
B $E552,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E553,1 (nothing) for 1 units
B $E554,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E555,1 (nothing) for 1 units
B $E556,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E557,1 (nothing) for 1 units
B $E558,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E559,1 (nothing) for 13 units
B $E55A,1 STREET_LAMP for 1 units
B $E55B,1 (nothing) for 3 units
B $E55C,1 STREET_LAMP for 1 units
B $E55D,1 (nothing) for 3 units
B $E55E,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E55F,1 (nothing) for 39 units
B $E562,1 STREET_LAMP for 1 units
B $E563,1 (nothing) for 1 units
B $E564,1 STREET_LAMP for 1 units
B $E565,1 (nothing) for 1 units
B $E566,1 STREET_LAMP for 1 units
B $E567,1 (nothing) for 1 units
B $E568,1 STREET_LAMP for 1 units
B $E569,1 (nothing) for 1 units
B $E56A,1 STREET_LAMP for 1 units
B $E56B,1 (nothing) for 1 units
B $E56C,1 STREET_LAMP for 1 units
B $E56D,1 (nothing) for 1 units
B $E56E,1 STREET_LAMP for 1 units
B $E56F,1 (nothing) for 1 units
B $E570,1 STREET_LAMP for 1 units
B $E571,1 (nothing) for 1 units
B $E572,1 STREET_LAMP for 1 units
B $E573,1 <Esc> Jump
W $E575,2 [$C6B7] Target
b $E577 [Stage 4] Map curvature data
B $E577,1 Curve Straight for 67 units
b $E577 [Stage 4] Map curvature data
B $E577,1 Curve Straight for 67 units
B $E57C,1 Curve Left for 14 units
B $E57C,1 Curve Left for 14 units
B $E57D,1 Curve Left Hard for 20 units
B $E57D,1 Curve Left Hard for 20 units
B $E57F,1 Curve Right Very Hard for 10 units
B $E57F,1 Curve Right Very Hard for 10 units
B $E580,1 Curve Left Hard for 20 units
B $E580,1 Curve Left Hard for 20 units
B $E582,1 Curve Left for 30 units
B $E582,1 Curve Left for 30 units
B $E584,1 Curve Straight for 69 units
B $E584,1 Curve Straight for 69 units
B $E589,1 Curve Right Hard for 31 units
B $E589,1 Curve Right Hard for 31 units
B $E58C,1 Curve Right Very Hard for 10 units
B $E58C,1 Curve Right Very Hard for 10 units
B $E58D,1 Curve Right Hard for 30 units
B $E58D,1 Curve Right Hard for 30 units
B $E58F,1 Curve Straight for 101 units
B $E58F,1 Curve Straight for 101 units
B $E596,1 <Esc> Loop
B $E596,1 <Esc> Loop
W $E598,2 [$C577] Target
W $E598,2 [$C577] Target
b $E59A [Stage 4] Map height data
B $E59A,1 Level Road for 14 units
b $E59A [Stage 4] Map height data
B $E59A,1 Level Road for 14 units
B $E59B,1 Going Up 3 for 2 units
B $E59B,1 Going Up 3 for 2 units
B $E59C,1 Going Up 5 for 2 units
B $E59C,1 Going Up 5 for 2 units
B $E59D,1 Going Up 7 for 4 units
B $E59D,1 Going Up 7 for 4 units
B $E59E,1 Going Up 5 for 1 units
B $E59E,1 Going Up 5 for 1 units
B $E59F,1 Going Up 3 for 1 units
B $E59F,1 Going Up 3 for 1 units
B $E5A0,1 Going Down 3 for 1 units
B $E5A0,1 Going Down 3 for 1 units
B $E5A1,1 Going Down 5 for 2 units
B $E5A1,1 Going Down 5 for 2 units
B $E5A2,1 Going Down 7 for 10 units
B $E5A2,1 Going Down 7 for 10 units
B $E5A3,1 Going Down 5 for 2 units
B $E5A3,1 Going Down 5 for 2 units
B $E5A4,1 Going Down 3 for 1 units
B $E5A4,1 Going Down 3 for 1 units
B $E5A5,1 Going Up 3 for 1 units
B $E5A5,1 Going Up 3 for 1 units
B $E5A6,1 Going Up 5 for 2 units
B $E5A6,1 Going Up 5 for 2 units
B $E5A7,1 Going Up 7 for 7 units
B $E5A7,1 Going Up 7 for 7 units
B $E5A8,1 Going Up 5 for 1 units
B $E5A8,1 Going Up 5 for 1 units
B $E5A9,1 Going Down 7 for 2 units
B $E5A9,1 Going Down 7 for 2 units
B $E5AA,1 Going Down 5 for 1 units
B $E5AA,1 Going Down 5 for 1 units
B $E5AB,1 Going Down 3 for 1 units
B $E5AB,1 Going Down 3 for 1 units
B $E5AC,1 Going Down 1 for 4 units
B $E5AC,1 Going Down 1 for 4 units
B $E5AD,1 Level Road for 119 units
B $E5AD,1 Level Road for 119 units
B $E5B5,1 Going Down 3 for 1 units
B $E5B5,1 Going Down 3 for 1 units
B $E5B6,1 Going Down 5 for 2 units
B $E5B6,1 Going Down 5 for 2 units
B $E5B7,1 Going Down 7 for 3 units
B $E5B7,1 Going Down 7 for 3 units
B $E5B8,1 Going Down 5 for 1 units
B $E5B8,1 Going Down 5 for 1 units
B $E5B9,1 Going Down 3 for 1 units
B $E5B9,1 Going Down 3 for 1 units
B $E5BA,1 Going Up 3 for 1 units
B $E5BA,1 Going Up 3 for 1 units
B $E5BB,1 Going Up 5 for 1 units
B $E5BB,1 Going Up 5 for 1 units
B $E5BC,1 Going Up 7 for 7 units
B $E5BC,1 Going Up 7 for 7 units
B $E5BD,1 Going Down 7 for 1 units
B $E5BD,1 Going Down 7 for 1 units
B $E5BE,1 Going Down 5 for 1 units
B $E5BE,1 Going Down 5 for 1 units
B $E5BF,1 Going Down 3 for 2 units
B $E5BF,1 Going Down 3 for 2 units
B $E5C0,1 Going Down 1 for 2 units
B $E5C0,1 Going Down 1 for 2 units
B $E5C1,1 Level Road for 201 units
B $E5C1,1 Level Road for 201 units
B $E5CF,1 <Esc> Loop
B $E5CF,1 <Esc> Loop
W $E5D1,2 [$C59A] Target
W $E5D1,2 [$C59A] Target
b $E5D3 [Stage 4] Map lanes data
B $E5D3,1 4 Lanes              [||||] {00} for 306 units
b $E5D3 [Stage 4] Map lanes data
B $E5D3,1 4 Lanes              [||||] {00} for 306 units
B $E5D7,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E5D7,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E5D9,1 3 Lanes L            [|||]  {81} for 6 units
B $E5D9,1 3 Lanes L            [|||]  {81} for 6 units
B $E5DB,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E5DB,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E5DD,1 4 Lanes              [||||] {00} for 8 units
B $E5DD,1 4 Lanes              [||||] {00} for 8 units
B $E5DF,1 4-3 Narrowing R      [/|||] {8E} for 8 units
B $E5DF,1 4-3 Narrowing R      [/|||] {8E} for 8 units
B $E5E1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $E5E1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $E5E3,1 4 Lanes              [||||] {00} for 8 units
B $E5E3,1 4 Lanes              [||||] {00} for 8 units
B $E5E5,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E5E5,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E5E7,1 3 Lanes L            [|||]  {81} for 6 units
B $E5E7,1 3 Lanes L            [|||]  {81} for 6 units
B $E5E9,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E5E9,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E5EB,1 4 Lanes              [||||] {00} for 16 units
B $E5EB,1 4 Lanes              [||||] {00} for 16 units
B $E5ED,1 4-3 Narrowing R      [/|||] {8E} for 2 units
B $E5ED,1 4-3 Narrowing R      [/|||] {8E} for 2 units
B $E5EF,1 3-2 Narrowing R       [/||] {0F} for 14 units
B $E5EF,1 3-2 Narrowing R       [/||] {0F} for 14 units
B $E5F1,1 2-3 Widening R        [\||] {1F} for 2 units
B $E5F1,1 2-3 Widening R        [\||] {1F} for 2 units
B $E5F3,1 3-4 Widening R       [\|||] {9E} for 2 units
B $E5F3,1 3-4 Widening R       [\|||] {9E} for 2 units
B $E5F5,1 4 Lanes              [||||] {00} for 14 units
B $E5F5,1 4 Lanes              [||||] {00} for 14 units
B $E5F7,1 <Esc> Loop
B $E5F7,1 <Esc> Loop
W $E5F9,2 [$C5D3] Target
W $E5F9,2 [$C5D3] Target
b $E5FB [Stage 4] Map hazards data
B $E5FB,1 Wait for 32 units
b $E5FB [Stage 4] Map hazards data
B $E5FB,1 Wait for 32 units
B $E5FC,1 Unknown command 4
B $E5FC,1 Unknown command 4
B $E5FE,1 Wait for 2 units
B $E5FE,1 Wait for 2 units
B $E5FF,1 Stop Spawning Barriers 3?
B $E5FF,1 Stop Spawning Barriers 3?
B $E601,1 Wait for 26 units
B $E601,1 Wait for 26 units
B $E602,1 Unknown command 5
B $E602,1 Unknown command 5
B $E604,1 Wait for 2 units
B $E604,1 Wait for 2 units
B $E605,1 Stop Spawning Barriers 3?
B $E605,1 Stop Spawning Barriers 3?
B $E607,1 Wait for 61 units
B $E607,1 Wait for 61 units
B $E608,1 Unknown command 4
B $E608,1 Unknown command 4
B $E60A,1 Wait for 2 units
B $E60A,1 Wait for 2 units
B $E60B,1 Stop Spawning Barriers 3?
B $E60B,1 Stop Spawning Barriers 3?
B $E60D,1 Wait for 9 units
B $E60D,1 Wait for 9 units
B $E60E,1 Unknown command 4
B $E60E,1 Unknown command 4
B $E610,1 Wait for 2 units
B $E610,1 Wait for 2 units
B $E611,1 Stop Spawning Barriers 3?
B $E611,1 Stop Spawning Barriers 3?
B $E613,1 Wait for 23 units
B $E613,1 Wait for 23 units
B $E614,1 Start Spawning Two Barriers
B $E614,1 Start Spawning Two Barriers
B $E616,1 Wait for 2 units
B $E616,1 Wait for 2 units
B $E617,1 Stop Spawning Barriers 3?
B $E617,1 Stop Spawning Barriers 3?
B $E619,1 Wait for 26 units
B $E619,1 Wait for 26 units
B $E61A,1 Start Spawning Barriers Right
B $E61A,1 Start Spawning Barriers Right
B $E61C,1 Wait for 2 units
B $E61C,1 Wait for 2 units
B $E61D,1 Stop Spawning Barriers 3?
B $E61D,1 Stop Spawning Barriers 3?
B $E61F,1 Wait for 12 units
B $E61F,1 Wait for 12 units
B $E620,1 <Esc> Loop
B $E620,1 <Esc> Loop
W $E622,2 [$C5FB] Target
W $E622,2 [$C5FB] Target
b $E624 [Stage 4] Map left object data
B $E624,1 STREET_LAMP for 1 units
b $E624 [Stage 4] Map left object data
B $E624,1 STREET_LAMP for 1 units
B $E625,1 (nothing) for 1 units
B $E625,1 (nothing) for 1 units
B $E626,1 STREET_LAMP for 1 units
B $E626,1 STREET_LAMP for 1 units
B $E627,1 (nothing) for 1 units
B $E627,1 (nothing) for 1 units
B $E628,1 STREET_LAMP for 1 units
B $E628,1 STREET_LAMP for 1 units
B $E629,1 (nothing) for 1 units
B $E629,1 (nothing) for 1 units
B $E62A,1 STREET_LAMP for 1 units
B $E62A,1 STREET_LAMP for 1 units
B $E62B,1 (nothing) for 1 units
B $E62B,1 (nothing) for 1 units
B $E62C,1 STREET_LAMP for 1 units
B $E62C,1 STREET_LAMP for 1 units
B $E62D,1 (nothing) for 1 units
B $E62D,1 (nothing) for 1 units
B $E62E,1 STREET_LAMP for 1 units
B $E62E,1 STREET_LAMP for 1 units
B $E62F,1 (nothing) for 1 units
B $E62F,1 (nothing) for 1 units
B $E630,1 STREET_LAMP for 1 units
B $E630,1 STREET_LAMP for 1 units
B $E631,1 (nothing) for 5 units
B $E631,1 (nothing) for 5 units
B $E632,1 FAR_COLUMN for 1 units
B $E632,1 FAR_COLUMN for 1 units
B $E633,1 (nothing) for 1 units
B $E633,1 (nothing) for 1 units
B $E634,1 FAR_COLUMN for 1 units
B $E634,1 FAR_COLUMN for 1 units
B $E635,1 (nothing) for 1 units
B $E635,1 (nothing) for 1 units
B $E636,1 FAR_COLUMN for 1 units
B $E636,1 FAR_COLUMN for 1 units
B $E637,1 (nothing) for 5 units
B $E637,1 (nothing) for 5 units
B $E638,1 NEAR_COLUMN for 1 units
B $E638,1 NEAR_COLUMN for 1 units
B $E639,1 (nothing) for 1 units
B $E639,1 (nothing) for 1 units
B $E63A,1 NEAR_COLUMN for 1 units
B $E63A,1 NEAR_COLUMN for 1 units
B $E63B,1 (nothing) for 1 units
B $E63B,1 (nothing) for 1 units
B $E63C,1 NEAR_COLUMN for 1 units
B $E63C,1 NEAR_COLUMN for 1 units
B $E63D,1 (nothing) for 1 units
B $E63D,1 (nothing) for 1 units
B $E63E,1 NEAR_COLUMN for 1 units
B $E63E,1 NEAR_COLUMN for 1 units
B $E63F,1 (nothing) for 1 units
B $E63F,1 (nothing) for 1 units
B $E640,1 NEAR_COLUMN for 1 units
B $E640,1 NEAR_COLUMN for 1 units
B $E641,1 (nothing) for 1 units
B $E641,1 (nothing) for 1 units
B $E642,1 NEAR_COLUMN for 1 units
B $E642,1 NEAR_COLUMN for 1 units
B $E643,1 (nothing) for 1 units
B $E643,1 (nothing) for 1 units
B $E644,1 NEAR_COLUMN for 1 units
B $E644,1 NEAR_COLUMN for 1 units
B $E645,1 (nothing) for 3 units
B $E645,1 (nothing) for 3 units
B $E646,1 NEAR_COLUMN for 1 units
B $E646,1 NEAR_COLUMN for 1 units
B $E647,1 FAR_COLUMN for 1 units
B $E647,1 FAR_COLUMN for 1 units
B $E648,1 NEAR_COLUMN for 1 units
B $E648,1 NEAR_COLUMN for 1 units
B $E649,1 FAR_COLUMN for 1 units
B $E649,1 FAR_COLUMN for 1 units
B $E64A,1 (nothing) for 4 units
B $E64A,1 (nothing) for 4 units
B $E64B,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E64B,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E64C,1 (nothing) for 1 units
B $E64C,1 (nothing) for 1 units
B $E64D,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E64D,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E64E,1 (nothing) for 1 units
B $E64E,1 (nothing) for 1 units
B $E64F,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E64F,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $E650,1 (nothing) for 15 units
B $E650,1 (nothing) for 15 units
B $E651,1 NEAR_COLUMN for 1 units
B $E651,1 NEAR_COLUMN for 1 units
B $E652,1 (nothing) for 1 units
B $E652,1 (nothing) for 1 units
B $E653,1 NEAR_COLUMN for 1 units
B $E653,1 NEAR_COLUMN for 1 units
B $E654,1 (nothing) for 1 units
B $E654,1 (nothing) for 1 units
B $E655,1 NEAR_COLUMN for 1 units
B $E655,1 NEAR_COLUMN for 1 units
B $E656,1 (nothing) for 1 units
B $E656,1 (nothing) for 1 units
B $E657,1 NEAR_COLUMN for 1 units
B $E657,1 NEAR_COLUMN for 1 units
B $E658,1 (nothing) for 1 units
B $E658,1 (nothing) for 1 units
B $E659,1 NEAR_COLUMN for 1 units
B $E659,1 NEAR_COLUMN for 1 units
B $E65A,1 (nothing) for 1 units
B $E65A,1 (nothing) for 1 units
B $E65B,1 NEAR_COLUMN for 1 units
B $E65B,1 NEAR_COLUMN for 1 units
B $E65C,1 (nothing) for 1 units
B $E65C,1 (nothing) for 1 units
B $E65D,1 NEAR_COLUMN for 1 units
B $E65D,1 NEAR_COLUMN for 1 units
B $E65E,1 (nothing) for 1 units
B $E65E,1 (nothing) for 1 units
B $E65F,1 NEAR_COLUMN for 1 units
B $E65F,1 NEAR_COLUMN for 1 units
B $E660,1 (nothing) for 1 units
B $E660,1 (nothing) for 1 units
B $E661,1 NEAR_COLUMN for 1 units
B $E661,1 NEAR_COLUMN for 1 units
B $E662,1 (nothing) for 1 units
B $E662,1 (nothing) for 1 units
B $E663,1 NEAR_COLUMN for 1 units
B $E663,1 NEAR_COLUMN for 1 units
B $E664,1 (nothing) for 8 units
B $E664,1 (nothing) for 8 units
B $E665,1 FAR_COLUMN for 1 units
B $E665,1 FAR_COLUMN for 1 units
B $E666,1 NEAR_COLUMN for 1 units
B $E666,1 NEAR_COLUMN for 1 units
B $E667,1 FAR_COLUMN for 1 units
B $E667,1 FAR_COLUMN for 1 units
B $E668,1 NEAR_COLUMN for 1 units
B $E668,1 NEAR_COLUMN for 1 units
B $E669,1 (nothing) for 1 units
B $E669,1 (nothing) for 1 units
B $E66A,1 NEAR_COLUMN for 1 units
B $E66A,1 NEAR_COLUMN for 1 units
B $E66B,1 (nothing) for 3 units
B $E66B,1 (nothing) for 3 units
B $E66C,1 NEAR_COLUMN for 1 units
B $E66C,1 NEAR_COLUMN for 1 units
B $E66D,1 (nothing) for 1 units
B $E66D,1 (nothing) for 1 units
B $E66E,1 NEAR_COLUMN for 1 units
B $E66E,1 NEAR_COLUMN for 1 units
B $E66F,1 (nothing) for 1 units
B $E66F,1 (nothing) for 1 units
B $E670,1 NEAR_COLUMN for 1 units
B $E670,1 NEAR_COLUMN for 1 units
B $E671,1 (nothing) for 1 units
B $E671,1 (nothing) for 1 units
B $E672,1 NEAR_COLUMN for 1 units
B $E672,1 NEAR_COLUMN for 1 units
B $E673,1 (nothing) for 1 units
B $E673,1 (nothing) for 1 units
B $E674,1 NEAR_COLUMN for 1 units
B $E674,1 NEAR_COLUMN for 1 units
B $E675,1 (nothing) for 1 units
B $E675,1 (nothing) for 1 units
B $E676,1 NEAR_COLUMN for 1 units
B $E676,1 NEAR_COLUMN for 1 units
B $E677,1 (nothing) for 1 units
B $E677,1 (nothing) for 1 units
B $E678,1 NEAR_COLUMN for 1 units
B $E678,1 NEAR_COLUMN for 1 units
B $E679,1 (nothing) for 1 units
B $E679,1 (nothing) for 1 units
B $E67A,1 NEAR_COLUMN for 1 units
B $E67A,1 NEAR_COLUMN for 1 units
B $E67B,1 (nothing) for 1 units
B $E67B,1 (nothing) for 1 units
B $E67C,1 NEAR_COLUMN for 1 units
B $E67C,1 NEAR_COLUMN for 1 units
B $E67D,1 (nothing) for 1 units
B $E67D,1 (nothing) for 1 units
B $E67E,1 NEAR_COLUMN for 1 units
B $E67E,1 NEAR_COLUMN for 1 units
B $E67F,1 (nothing) for 1 units
B $E67F,1 (nothing) for 1 units
B $E680,1 NEAR_COLUMN for 1 units
B $E680,1 NEAR_COLUMN for 1 units
B $E681,1 (nothing) for 1 units
B $E681,1 (nothing) for 1 units
B $E682,1 NEAR_COLUMN for 1 units
B $E682,1 NEAR_COLUMN for 1 units
B $E683,1 (nothing) for 1 units
B $E683,1 (nothing) for 1 units
B $E684,1 NEAR_COLUMN for 1 units
B $E684,1 NEAR_COLUMN for 1 units
B $E685,1 (nothing) for 1 units
B $E685,1 (nothing) for 1 units
B $E686,1 NEAR_COLUMN for 1 units
B $E686,1 NEAR_COLUMN for 1 units
B $E687,1 (nothing) for 1 units
B $E687,1 (nothing) for 1 units
B $E688,1 NEAR_COLUMN for 1 units
B $E688,1 NEAR_COLUMN for 1 units
B $E689,1 (nothing) for 1 units
B $E689,1 (nothing) for 1 units
B $E68A,1 NEAR_COLUMN for 1 units
B $E68A,1 NEAR_COLUMN for 1 units
B $E68B,1 (nothing) for 1 units
B $E68B,1 (nothing) for 1 units
B $E68C,1 NEAR_COLUMN for 1 units
B $E68C,1 NEAR_COLUMN for 1 units
B $E68D,1 (nothing) for 1 units
B $E68D,1 (nothing) for 1 units
B $E68E,1 NEAR_COLUMN for 1 units
B $E68E,1 NEAR_COLUMN for 1 units
B $E68F,1 (nothing) for 1 units
B $E68F,1 (nothing) for 1 units
B $E690,1 NEAR_COLUMN for 1 units
B $E690,1 NEAR_COLUMN for 1 units
B $E691,1 (nothing) for 1 units
B $E691,1 (nothing) for 1 units
B $E692,1 NEAR_COLUMN for 1 units
B $E692,1 NEAR_COLUMN for 1 units
B $E693,1 (nothing) for 1 units
B $E693,1 (nothing) for 1 units
B $E694,1 NEAR_COLUMN for 1 units
B $E694,1 NEAR_COLUMN for 1 units
B $E695,1 (nothing) for 1 units
B $E695,1 (nothing) for 1 units
B $E696,1 NEAR_COLUMN for 1 units
B $E696,1 NEAR_COLUMN for 1 units
B $E697,1 (nothing) for 5 units
B $E697,1 (nothing) for 5 units
B $E698,1 FAR_COLUMN for 1 units
B $E698,1 FAR_COLUMN for 1 units
B $E699,1 (nothing) for 1 units
B $E699,1 (nothing) for 1 units
B $E69A,1 FAR_COLUMN for 1 units
B $E69A,1 FAR_COLUMN for 1 units
B $E69B,1 (nothing) for 1 units
B $E69B,1 (nothing) for 1 units
B $E69C,1 FAR_COLUMN for 1 units
B $E69C,1 FAR_COLUMN for 1 units
B $E69D,1 (nothing) for 3 units
B $E69D,1 (nothing) for 3 units
B $E69E,1 NEAR_COLUMN for 1 units
B $E69E,1 NEAR_COLUMN for 1 units
B $E69F,1 (nothing) for 1 units
B $E69F,1 (nothing) for 1 units
B $E6A0,1 FAR_COLUMN for 1 units
B $E6A0,1 FAR_COLUMN for 1 units
B $E6A1,1 (nothing) for 3 units
B $E6A1,1 (nothing) for 3 units
B $E6A2,1 PILE_OF_ROCKS for 1 units
B $E6A2,1 PILE_OF_ROCKS for 1 units
B $E6A3,1 (nothing) for 1 units
B $E6A3,1 (nothing) for 1 units
B $E6A4,1 PILE_OF_ROCKS for 1 units
B $E6A4,1 PILE_OF_ROCKS for 1 units
B $E6A5,1 (nothing) for 1 units
B $E6A5,1 (nothing) for 1 units
B $E6A6,1 PILE_OF_ROCKS for 1 units
B $E6A6,1 PILE_OF_ROCKS for 1 units
B $E6A7,1 (nothing) for 1 units
B $E6A7,1 (nothing) for 1 units
B $E6A8,1 PILE_OF_ROCKS for 1 units
B $E6A8,1 PILE_OF_ROCKS for 1 units
B $E6A9,1 (nothing) for 6 units
B $E6A9,1 (nothing) for 6 units
B $E6AA,1 TURN_SIGN_POINTING_RIGHT for 4 units
B $E6AA,1 TURN_SIGN_POINTING_RIGHT for 4 units
B $E6AB,1 (nothing) for 3 units
B $E6AB,1 (nothing) for 3 units
B $E6AC,1 NEAR_COLUMN for 1 units
B $E6AC,1 NEAR_COLUMN for 1 units
B $E6AD,1 (nothing) for 5 units
B $E6AD,1 (nothing) for 5 units
B $E6AE,1 NEAR_COLUMN for 1 units
B $E6AE,1 NEAR_COLUMN for 1 units
B $E6AF,1 (nothing) for 1 units
B $E6AF,1 (nothing) for 1 units
B $E6B0,1 NEAR_COLUMN for 1 units
B $E6B0,1 NEAR_COLUMN for 1 units
B $E6B1,1 (nothing) for 1 units
B $E6B1,1 (nothing) for 1 units
B $E6B2,1 NEAR_COLUMN for 1 units
B $E6B2,1 NEAR_COLUMN for 1 units
B $E6B3,1 <Esc> Loop
B $E6B3,1 <Esc> Loop
W $E6B5,2 [$C624] Target
W $E6B5,2 [$C624] Target
b $E6B7 [Stage 4] Map right object data
B $E6B7,1 (nothing) for 18 units
b $E6B7 [Stage 4] Map right object data
B $E6B7,1 (nothing) for 18 units
B $E6B9,1 FAR_COLUMN for 1 units
B $E6B9,1 FAR_COLUMN for 1 units
B $E6BA,1 (nothing) for 1 units
B $E6BA,1 (nothing) for 1 units
B $E6BB,1 FAR_COLUMN for 1 units
B $E6BB,1 FAR_COLUMN for 1 units
B $E6BC,1 (nothing) for 1 units
B $E6BC,1 (nothing) for 1 units
B $E6BD,1 FAR_COLUMN for 1 units
B $E6BD,1 FAR_COLUMN for 1 units
B $E6BE,1 (nothing) for 4 units
B $E6BE,1 (nothing) for 4 units
B $E6BF,1 FAR_COLUMN for 1 units
B $E6BF,1 FAR_COLUMN for 1 units
B $E6C0,1 NEAR_COLUMN for 1 units
B $E6C0,1 NEAR_COLUMN for 1 units
B $E6C1,1 FAR_COLUMN for 1 units
B $E6C1,1 FAR_COLUMN for 1 units
B $E6C2,1 NEAR_COLUMN for 1 units
B $E6C2,1 NEAR_COLUMN for 1 units
B $E6C3,1 (nothing) for 1 units
B $E6C3,1 (nothing) for 1 units
B $E6C4,1 NEAR_COLUMN for 1 units
B $E6C4,1 NEAR_COLUMN for 1 units
B $E6C5,1 (nothing) for 1 units
B $E6C5,1 (nothing) for 1 units
B $E6C6,1 NEAR_COLUMN for 1 units
B $E6C6,1 NEAR_COLUMN for 1 units
B $E6C7,1 (nothing) for 3 units
B $E6C7,1 (nothing) for 3 units
B $E6C8,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E6C8,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E6C9,1 (nothing) for 1 units
B $E6C9,1 (nothing) for 1 units
B $E6CA,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E6CA,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E6CB,1 (nothing) for 1 units
B $E6CB,1 (nothing) for 1 units
B $E6CC,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E6CC,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E6CD,1 (nothing) for 1 units
B $E6CD,1 (nothing) for 1 units
B $E6CE,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E6CE,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E6CF,1 (nothing) for 3 units
B $E6CF,1 (nothing) for 3 units
B $E6D0,1 NEAR_COLUMN for 1 units
B $E6D0,1 NEAR_COLUMN for 1 units
B $E6D1,1 (nothing) for 1 units
B $E6D1,1 (nothing) for 1 units
B $E6D2,1 NEAR_COLUMN for 1 units
B $E6D2,1 NEAR_COLUMN for 1 units
B $E6D3,1 (nothing) for 1 units
B $E6D3,1 (nothing) for 1 units
B $E6D4,1 NEAR_COLUMN for 1 units
B $E6D4,1 NEAR_COLUMN for 1 units
B $E6D5,1 (nothing) for 1 units
B $E6D5,1 (nothing) for 1 units
B $E6D6,1 NEAR_COLUMN for 1 units
B $E6D6,1 NEAR_COLUMN for 1 units
B $E6D7,1 (nothing) for 1 units
B $E6D7,1 (nothing) for 1 units
B $E6D8,1 NEAR_COLUMN for 1 units
B $E6D8,1 NEAR_COLUMN for 1 units
B $E6D9,1 (nothing) for 1 units
B $E6D9,1 (nothing) for 1 units
B $E6DA,1 NEAR_COLUMN for 1 units
B $E6DA,1 NEAR_COLUMN for 1 units
B $E6DB,1 (nothing) for 1 units
B $E6DB,1 (nothing) for 1 units
B $E6DC,1 NEAR_COLUMN for 1 units
B $E6DC,1 NEAR_COLUMN for 1 units
B $E6DD,1 (nothing) for 1 units
B $E6DD,1 (nothing) for 1 units
B $E6DE,1 NEAR_COLUMN for 1 units
B $E6DE,1 NEAR_COLUMN for 1 units
B $E6DF,1 (nothing) for 1 units
B $E6DF,1 (nothing) for 1 units
B $E6E0,1 NEAR_COLUMN for 1 units
B $E6E0,1 NEAR_COLUMN for 1 units
B $E6E1,1 (nothing) for 1 units
B $E6E1,1 (nothing) for 1 units
B $E6E2,1 NEAR_COLUMN for 1 units
B $E6E2,1 NEAR_COLUMN for 1 units
B $E6E3,1 (nothing) for 1 units
B $E6E3,1 (nothing) for 1 units
B $E6E4,1 NEAR_COLUMN for 1 units
B $E6E4,1 NEAR_COLUMN for 1 units
B $E6E5,1 (nothing) for 1 units
B $E6E5,1 (nothing) for 1 units
B $E6E6,1 NEAR_COLUMN for 1 units
B $E6E6,1 NEAR_COLUMN for 1 units
B $E6E7,1 (nothing) for 1 units
B $E6E7,1 (nothing) for 1 units
B $E6E8,1 NEAR_COLUMN for 1 units
B $E6E8,1 NEAR_COLUMN for 1 units
B $E6E9,1 (nothing) for 1 units
B $E6E9,1 (nothing) for 1 units
B $E6EA,1 NEAR_COLUMN for 1 units
B $E6EA,1 NEAR_COLUMN for 1 units
B $E6EB,1 (nothing) for 1 units
B $E6EB,1 (nothing) for 1 units
B $E6EC,1 NEAR_COLUMN for 1 units
B $E6EC,1 NEAR_COLUMN for 1 units
B $E6ED,1 (nothing) for 1 units
B $E6ED,1 (nothing) for 1 units
B $E6EE,1 NEAR_COLUMN for 1 units
B $E6EE,1 NEAR_COLUMN for 1 units
B $E6EF,1 (nothing) for 1 units
B $E6EF,1 (nothing) for 1 units
B $E6F0,1 NEAR_COLUMN for 1 units
B $E6F0,1 NEAR_COLUMN for 1 units
B $E6F1,1 (nothing) for 1 units
B $E6F1,1 (nothing) for 1 units
B $E6F2,1 NEAR_COLUMN for 1 units
B $E6F2,1 NEAR_COLUMN for 1 units
B $E6F3,1 (nothing) for 1 units
B $E6F3,1 (nothing) for 1 units
B $E6F4,1 NEAR_COLUMN for 1 units
B $E6F4,1 NEAR_COLUMN for 1 units
B $E6F5,1 (nothing) for 1 units
B $E6F5,1 (nothing) for 1 units
B $E6F6,1 NEAR_COLUMN for 1 units
B $E6F6,1 NEAR_COLUMN for 1 units
B $E6F7,1 (nothing) for 7 units
B $E6F7,1 (nothing) for 7 units
B $E6F8,1 PILE_OF_ROCKS for 1 units
B $E6F8,1 PILE_OF_ROCKS for 1 units
B $E6F9,1 (nothing) for 1 units
B $E6F9,1 (nothing) for 1 units
B $E6FA,1 PILE_OF_ROCKS for 1 units
B $E6FA,1 PILE_OF_ROCKS for 1 units
B $E6FB,1 (nothing) for 1 units
B $E6FB,1 (nothing) for 1 units
B $E6FC,1 PILE_OF_ROCKS for 1 units
B $E6FC,1 PILE_OF_ROCKS for 1 units
B $E6FD,1 (nothing) for 3 units
B $E6FD,1 (nothing) for 3 units
B $E6FE,1 PILE_OF_ROCKS for 1 units
B $E6FE,1 PILE_OF_ROCKS for 1 units
B $E6FF,1 (nothing) for 1 units
B $E6FF,1 (nothing) for 1 units
B $E700,1 PILE_OF_ROCKS for 1 units
B $E700,1 PILE_OF_ROCKS for 1 units
B $E701,1 (nothing) for 10 units
B $E701,1 (nothing) for 10 units
B $E702,1 FAR_COLUMN for 1 units
B $E702,1 FAR_COLUMN for 1 units
B $E703,1 NEAR_COLUMN for 1 units
B $E703,1 NEAR_COLUMN for 1 units
B $E704,1 (nothing) for 1 units
B $E704,1 (nothing) for 1 units
B $E705,1 NEAR_COLUMN for 1 units
B $E705,1 NEAR_COLUMN for 1 units
B $E706,1 (nothing) for 1 units
B $E706,1 (nothing) for 1 units
B $E707,1 NEAR_COLUMN for 1 units
B $E707,1 NEAR_COLUMN for 1 units
B $E708,1 (nothing) for 1 units
B $E708,1 (nothing) for 1 units
B $E709,1 NEAR_COLUMN for 1 units
B $E709,1 NEAR_COLUMN for 1 units
B $E70A,1 (nothing) for 1 units
B $E70A,1 (nothing) for 1 units
B $E70B,1 NEAR_COLUMN for 1 units
B $E70B,1 NEAR_COLUMN for 1 units
B $E70C,1 (nothing) for 1 units
B $E70C,1 (nothing) for 1 units
B $E70D,1 NEAR_COLUMN for 1 units
B $E70D,1 NEAR_COLUMN for 1 units
B $E70E,1 (nothing) for 1 units
B $E70E,1 (nothing) for 1 units
B $E70F,1 NEAR_COLUMN for 1 units
B $E70F,1 NEAR_COLUMN for 1 units
B $E710,1 (nothing) for 1 units
B $E710,1 (nothing) for 1 units
B $E711,1 NEAR_COLUMN for 1 units
B $E711,1 NEAR_COLUMN for 1 units
B $E712,1 (nothing) for 1 units
B $E712,1 (nothing) for 1 units
B $E713,1 NEAR_COLUMN for 1 units
B $E713,1 NEAR_COLUMN for 1 units
B $E714,1 (nothing) for 1 units
B $E714,1 (nothing) for 1 units
B $E715,1 NEAR_COLUMN for 1 units
B $E715,1 NEAR_COLUMN for 1 units
B $E716,1 (nothing) for 1 units
B $E716,1 (nothing) for 1 units
B $E717,1 NEAR_COLUMN for 1 units
B $E717,1 NEAR_COLUMN for 1 units
B $E718,1 (nothing) for 1 units
B $E718,1 (nothing) for 1 units
B $E719,1 NEAR_COLUMN for 1 units
B $E719,1 NEAR_COLUMN for 1 units
B $E71A,1 (nothing) for 1 units
B $E71A,1 (nothing) for 1 units
B $E71B,1 NEAR_COLUMN for 1 units
B $E71B,1 NEAR_COLUMN for 1 units
B $E71C,1 (nothing) for 1 units
B $E71C,1 (nothing) for 1 units
B $E71D,1 NEAR_COLUMN for 1 units
B $E71D,1 NEAR_COLUMN for 1 units
B $E71E,1 (nothing) for 1 units
B $E71E,1 (nothing) for 1 units
B $E71F,1 NEAR_COLUMN for 1 units
B $E71F,1 NEAR_COLUMN for 1 units
B $E720,1 (nothing) for 1 units
B $E720,1 (nothing) for 1 units
B $E721,1 NEAR_COLUMN for 1 units
B $E721,1 NEAR_COLUMN for 1 units
B $E722,1 (nothing) for 1 units
B $E722,1 (nothing) for 1 units
B $E723,1 NEAR_COLUMN for 1 units
B $E723,1 NEAR_COLUMN for 1 units
B $E724,1 (nothing) for 1 units
B $E724,1 (nothing) for 1 units
B $E725,1 NEAR_COLUMN for 1 units
B $E725,1 NEAR_COLUMN for 1 units
B $E726,1 (nothing) for 3 units
B $E726,1 (nothing) for 3 units
B $E727,1 NEAR_COLUMN for 1 units
B $E727,1 NEAR_COLUMN for 1 units
B $E728,1 FAR_COLUMN for 1 units
B $E728,1 FAR_COLUMN for 1 units
B $E729,1 NEAR_COLUMN for 1 units
B $E729,1 NEAR_COLUMN for 1 units
B $E72A,1 (nothing) for 14 units
B $E72A,1 (nothing) for 14 units
B $E72B,1 TURN_SIGN_POINTING_LEFT for 2 units
B $E72B,1 TURN_SIGN_POINTING_LEFT for 2 units
B $E72C,1 (nothing) for 7 units
B $E72C,1 (nothing) for 7 units
B $E72D,1 PILE_OF_ROCKS for 1 units
B $E72D,1 PILE_OF_ROCKS for 1 units
B $E72E,1 (nothing) for 1 units
B $E72E,1 (nothing) for 1 units
B $E72F,1 PILE_OF_ROCKS for 1 units
B $E72F,1 PILE_OF_ROCKS for 1 units
B $E730,1 (nothing) for 1 units
B $E730,1 (nothing) for 1 units
B $E731,1 PILE_OF_ROCKS for 1 units
B $E731,1 PILE_OF_ROCKS for 1 units
B $E732,1 (nothing) for 1 units
B $E732,1 (nothing) for 1 units
B $E733,1 PILE_OF_ROCKS for 1 units
B $E733,1 PILE_OF_ROCKS for 1 units
B $E734,1 (nothing) for 9 units
B $E734,1 (nothing) for 9 units
B $E735,1 NEAR_COLUMN for 1 units
B $E735,1 NEAR_COLUMN for 1 units
B $E736,1 (nothing) for 1 units
B $E736,1 (nothing) for 1 units
B $E737,1 NEAR_COLUMN for 1 units
B $E737,1 NEAR_COLUMN for 1 units
B $E738,1 (nothing) for 1 units
B $E738,1 (nothing) for 1 units
B $E739,1 NEAR_COLUMN for 1 units
B $E739,1 NEAR_COLUMN for 1 units
B $E73A,1 <Esc> Loop
B $E73A,1 <Esc> Loop
W $E73C,2 [$C6B7] Target
W $E73C,2 [$C6B7] Target
N $E7F2 LOD
B $E7F2,1 Width (bytes)
B $E7F3,1 Flags
B $E7F4,1 Height (pixels)
W $E7F5,2 [$C870] Bitmap address
W $E7F7,2 [$C870] Pre-shifted bitmap address
N $E7F9 LOD
B $E7F9,1 Width (bytes)
B $E7FA,1 Flags
B $E7FB,1 Height (pixels)
W $E7FC,2 [$C924] Bitmap address
W $E7FE,2 [$C924] Pre-shifted bitmap address
N $E800 LOD
B $E800,1 Width (bytes)
B $E801,1 Flags
B $E802,1 Height (pixels)
W $E803,2 [$C97C] Bitmap address
W $E805,2 [$C97C] Pre-shifted bitmap address
N $E807 LOD
B $E807,1 Width (bytes)
B $E808,1 Flags
B $E809,1 Height (pixels)
W $E80A,2 [$C97C] Bitmap address
W $E80C,2 [$C97C] Pre-shifted bitmap address
N $E80E LOD
B $E80E,1 Width (bytes)
B $E80F,1 Flags
B $E810,1 Height (pixels)
W $E811,2 [$C994] Bitmap address
W $E813,2 [$C994] Pre-shifted bitmap address
N $E815 LOD
B $E815,1 Width (bytes)
B $E816,1 Flags
B $E817,1 Height (pixels)
W $E818,2 [$C994] Bitmap address
W $E81A,2 [$C994] Pre-shifted bitmap address
N $E81C LOD
B $E81C,1 Width (bytes)
B $E81D,1 Flags
B $E81E,1 Height (pixels)
W $E81F,2 [$C9D4] Bitmap address
W $E821,2 [$C9D4] Pre-shifted bitmap address
N $E823 LOD
B $E823,1 Width (bytes)
B $E824,1 Flags
B $E825,1 Height (pixels)
W $E826,2 [$CA88] Bitmap address
W $E828,2 [$CA88] Pre-shifted bitmap address
N $E82A LOD
B $E82A,1 Width (bytes)
B $E82B,1 Flags
B $E82C,1 Height (pixels)
W $E82D,2 [$CAE0] Bitmap address
W $E82F,2 [$CAE0] Pre-shifted bitmap address
N $E831 LOD
B $E831,1 Width (bytes)
B $E832,1 Flags
B $E833,1 Height (pixels)
W $E834,2 [$CAE0] Bitmap address
W $E836,2 [$CAE0] Pre-shifted bitmap address
N $E838 LOD
B $E838,1 Width (bytes)
B $E839,1 Flags
B $E83A,1 Height (pixels)
W $E83B,2 [$C994] Bitmap address
W $E83D,2 [$C9B4] Pre-shifted bitmap address
N $E83F LOD
B $E83F,1 Width (bytes)
B $E840,1 Flags
B $E841,1 Height (pixels)
W $E842,2 [$C994] Bitmap address
W $E844,2 [$C9B4] Pre-shifted bitmap address
N $E846 LOD
B $E846,1 Width (bytes)
B $E847,1 Flags
B $E848,1 Height (pixels)
W $E849,2 [$CB0D] Bitmap address
W $E84B,2 [$CB0D] Pre-shifted bitmap address
N $E84D LOD
B $E84D,1 Width (bytes)
B $E84E,1 Flags
B $E84F,1 Height (pixels)
W $E850,2 [$CBBB] Bitmap address
W $E852,2 [$CBBB] Pre-shifted bitmap address
N $E854 LOD
B $E854,1 Width (bytes)
B $E855,1 Flags
B $E856,1 Height (pixels)
W $E857,2 [$CC07] Bitmap address
W $E859,2 [$CC07] Pre-shifted bitmap address
N $E85B LOD
B $E85B,1 Width (bytes)
B $E85C,1 Flags
B $E85D,1 Height (pixels)
W $E85E,2 [$CC07] Bitmap address
W $E860,2 [$CC07] Pre-shifted bitmap address
N $E862 LOD
B $E862,1 Width (bytes)
B $E863,1 Flags
B $E864,1 Height (pixels)
W $E865,2 [$CC2E] Bitmap address
W $E867,2 [$CC2E] Pre-shifted bitmap address
N $E869 LOD
B $E869,1 Width (bytes)
B $E86A,1 Flags
B $E86B,1 Height (pixels)
W $E86C,2 [$CC2E] Bitmap address
W $E86E,2 [$CC2E] Pre-shifted bitmap address
B $E870,180,6 Bitmap data 6 bytes x 30
B $E924,88,4 Bitmap data 4 bytes x 22
B $E97C,24,2 Bitmap data 2 bytes x 12
B $E97C,24,2 Bitmap data 2 bytes x 12
B $E994,32,2 Bitmap data (masked) 2 bytes x 8
B $E994,32,2 Bitmap data (masked) 2 bytes x 8
B $E994,32,2 Bitmap data (masked) 2 bytes x 8
B $E994,32,2 Bitmap data (masked) 2 bytes x 8
B $E9B4,32,2 Pre-shifted bitmap data (masked) 2 bytes x 8
B $E9B4,32,2 Pre-shifted bitmap data (masked) 2 bytes x 8
B $E9D4,180,6 Bitmap data 6 bytes x 30
B $EA88,88,4 Bitmap data 4 bytes x 22
B $EAE0,45,3 Bitmap data 3 bytes x 15
B $EAE0,45,3 Bitmap data 3 bytes x 15
B $EB0D,174,6 Bitmap data 6 bytes x 29
B $EBBB,76,4 Bitmap data 4 bytes x 19
B $EC07,39,3 Bitmap data 3 bytes x 13
B $EC07,39,3 Bitmap data 3 bytes x 13
B $EC2E,32,2 Bitmap data (masked) 2 bytes x 8
B $EC2E,32,2 Bitmap data (masked) 2 bytes x 8
N $F4D8 LOD
B $F4D8,1 Width (bytes)
B $F4D9,1 Flags
B $F4DA,1 Height (pixels)
W $F4DB,2 [$D502] Bitmap address
W $F4DD,2 [$D502] Pre-shifted bitmap address
N $F4DF LOD
B $F4DF,1 Width (bytes)
B $F4E0,1 Flags
B $F4E1,1 Height (pixels)
W $F4E2,2 [$D541] Bitmap address
W $F4E4,2 [$D541] Pre-shifted bitmap address
N $F4E6 LOD
B $F4E6,1 Width (bytes)
B $F4E7,1 Flags
B $F4E8,1 Height (pixels)
W $F4E9,2 [$D561] Bitmap address
W $F4EB,2 [$D58D] Pre-shifted bitmap address
N $F4ED LOD
B $F4ED,1 Width (bytes)
B $F4EE,1 Flags
B $F4EF,1 Height (pixels)
W $F4F0,2 [$D5B9] Bitmap address
W $F4F2,2 [$D5DD] Pre-shifted bitmap address
N $F4F4 LOD
B $F4F4,1 Width (bytes)
B $F4F5,1 Flags
B $F4F6,1 Height (pixels)
W $F4F7,2 [$D5B9] Bitmap address
W $F4F9,2 [$D5DD] Pre-shifted bitmap address
N $F4FB LOD
B $F4FB,1 Width (bytes)
B $F4FC,1 Flags
B $F4FD,1 Height (pixels)
W $F4FE,2 [$D601] Bitmap address
W $F500,2 [$D60B] Pre-shifted bitmap address
B $F502,63,3 Bitmap data 3 bytes x 21
B $F541,32,2 Bitmap data 2 bytes x 16
B $F561,44,2 Bitmap data (masked) 2 bytes x 11
B $F58D,44,2 Pre-shifted bitmap data (masked) 2 bytes x 11
B $F5B9,36,2 Bitmap data (masked) 2 bytes x 9
B $F5B9,36,2 Bitmap data (masked) 2 bytes x 9
B $F5DD,36,2 Pre-shifted bitmap data (masked) 2 bytes x 9
B $F5DD,36,2 Pre-shifted bitmap data (masked) 2 bytes x 9
B $F601,10,1 Bitmap data (masked) 1 bytes x 5
B $F60B,10,1 Pre-shifted bitmap data (masked) 1 bytes x 5
N $F615 LOD
B $F615,1 Width (bytes)
B $F616,1 Flags
B $F617,1 Height (pixels)
W $F618,2 [$D63F] Bitmap address
W $F61A,2 [$D63F] Pre-shifted bitmap address
N $F61C LOD
B $F61C,1 Width (bytes)
B $F61D,1 Flags
B $F61E,1 Height (pixels)
W $F61F,2 [$D63F] Bitmap address
W $F621,2 [$D63F] Pre-shifted bitmap address
N $F623 LOD
B $F623,1 Width (bytes)
B $F624,1 Flags
B $F625,1 Height (pixels)
W $F626,2 [$D683] Bitmap address
W $F628,2 [$D683] Pre-shifted bitmap address
N $F62A LOD
B $F62A,1 Width (bytes)
B $F62B,1 Flags
B $F62C,1 Height (pixels)
W $F62D,2 [$D6AA] Bitmap address
W $F62F,2 [$D6AA] Pre-shifted bitmap address
N $F631 LOD
B $F631,1 Width (bytes)
B $F632,1 Flags
B $F633,1 Height (pixels)
W $F634,2 [$D6AA] Bitmap address
W $F636,2 [$D6AA] Pre-shifted bitmap address
N $F638 LOD
B $F638,1 Width (bytes)
B $F639,1 Flags
B $F63A,1 Height (pixels)
W $F63B,2 [$D6BC] Bitmap address
W $F63D,2 [$D6D8] Pre-shifted bitmap address
B $F63F,68,4 Bitmap data 4 bytes x 17
B $F63F,68,4 Bitmap data 4 bytes x 17
B $F683,39,3 Bitmap data 3 bytes x 13
B $F6AA,18,2 Bitmap data 2 bytes x 9
B $F6AA,18,2 Bitmap data 2 bytes x 9
B $F6BC,28,2 Bitmap data (masked) 2 bytes x 7
B $F6D8,28,2 Pre-shifted bitmap data (masked) 2 bytes x 7
