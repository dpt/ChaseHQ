> $C000 ; ChaseHQ-128K-bank-1.skool
> $C000 ;
> $C000 ; Bank 1 holds the data for stages 1 and 2.
> $C000 ;
> $C000
@ $C000 org
b $C000 [Stage 1] Horizon graphic
B $C000,240,8
b $C0F0 [Stage 1] Per-stage data
W $C0F0,2,2 [$C82A] Address of perp's mugshot attributes
W $C0F2,2,2 [$6400] Address of perp's mugshot bitmap
W $C0F4,2,2 [$D470] Screen attributes used for the ground colour (a pair of matching bytes)
W $C0F6,2,2 [$C240] Address of table of LODs for tumbleweeds, barriers.
W $C0F8,2,2 [$C242] (points at a handler address)
W $C0FA,2,2 [$C23F] Address of right hand graphics entry/entries (-7 bytes)
W $C0FC,2,2 [$C254] (points at a handler address)
W $C0FE,2,2 [$C281] (points at a handler address)
W $C100,2,2 [$C27E] Address of left hand graphics entry/entries (-7 bytes)
W $C102,2,2 [$C293] (points at a handler address)
W $C104,2,2 [$C139] Address of Nancy's perp description
W $C106,2,2 [$C1ED] Address of arrest messages
W $C108,2,2 [$6400] Helicopter related 1
W $C10A,2,2 [$640C] Helicopter related 2
w $C10C [Stage 1] Table of addresses of LODs
W $C10C,2,2 [$CE46] Address of LOD of stone/dust?
W $C10E,2,2 [$CE70] Address of LOD of stone/dust?
W $C110,2,2 [$C892] Address of LOD of car (the perp's car)
W $C112,2,2 [$C83E] Address of LOD of car (a Lambo in S1)
W $C114,2,2 [$C868] Address of LOD of car (a truck in S1)
W $C116,2,2 [$C83E] Address of LOD of car (a Lambo in S1)
W $C118,2,2 [$C892] Address of LOD of car (a generic car in S1)
b $C11A [Stage 1] Per-stage difficulty settings
B $C11A,1,1 How often cars spawn. Lower values spawn cars more often.
B $C11B,1,1 Smash related parameter
B $C11C,1,1 Smash related parameter
w $C11D [Stage 1] Per-stage setup data
W $C11D,2,2 road_pos
W $C11F,2,2 [$C2C3] Address of start stretch, curvature
W $C121,2,2 [$C2E3] Address of start stretch, height
W $C123,2,2 [$C30E] Address of start stretch, lanes
W $C125,2,2 [$C377] Address of start stretch, right-side objects
W $C127,2,2 [$C329] Address of start stretch, left-side objects
W $C129,2,2 [$C31C] Address of start stretch, hazards
w $C12B [Stage 1] Per-stage attract mode data
W $C12B,2,2 road_pos
W $C12D,2,2 [$C5D3] Address of loop section, curvature
W $C12F,2,2 [$C608] Address of loop section, height
W $C131,2,2 [$C65C] Address of loop section, lanes
W $C133,2,2 [$C713] Address of loop section, right-side objects
W $C135,2,2 [$C693] Address of loop section, left-side objects
W $C137,2,2 [$C676] Address of loop section, hazards
b $C139 [Stage 1] Nancy's perp description
B $C139,1,1 Character identifier (0/1/2/3 = Pilot/Nancy/Raymond/Tony)
W $C13A,2,2 [$C145] Perp description pointer
W $C13C,2,2 [$C16D] Perp description pointer
W $C13E,2,2 [$C195] Perp description pointer
W $C140,2,2 [$C1BF] Perp description pointer
B $C142,1,1 terminator?
B $C143,2,2
T $C145,40,39:n1 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $C16D,40,39:n1 "EMERGENCY HERE. RALPH THE IDAHO SLASHER,"
T $C195,42,41:n1 "IS FLEEING TOWARDS THE SUBURBS. THE TARGET"
T $C1BF,46,45:n1 "VEHICLE IS A WHITE BRITISH SPORTS CAR... OVER."
b $C1ED [Stage 1] Arrest messages
B $C1ED,1,1 ?frame delay until first message
B $C1EE,1,1 ?frame delay until next message
B $C1EF,1,1 Flags
B $C1F0,1,1 Attribute
W $C1F1,2,2 Back buffer address
W $C1F3,2,2 Attribute address
T $C1F5,27,26:n1 "OK! YOU ARE UNDER ARREST ON"
B $C210,1,1 ?frame delay until next message
B $C211,1,1 Flags
B $C212,1,1 Attribute
W $C213,2,2 Back buffer address
W $C215,2,2 Attribute address
T $C217,26,25:n1 "SUSPICION OF FIRST DEGREE "
B $C231,1,1 ?frame delay until next message
B $C232,1,1 Flags
B $C233,1,1 Attribute
W $C234,2,2 Back buffer address
W $C236,2,2 Attribute address
T $C238,6,5:n1 "MURDER"
B $C23E,1,1 ?frame delay until next message
B $C23F,1,1 Stop
b $C240 [Stage 1] Hittable hazards
B $C240,1,1 ?id
W $C241,2,2 [$D182] Address of LODs
B $C243,1,1 ?id
W $C244,2,2 [$D1F2] Address of LODs
b $C246 [Stage 1] Graphic definition
N $C246 Definition
B $C246,1,1 Hit coord max/min (R/L)
B $C247,1,1 Hit coord min/max (R/L)
B $C248,1,1 ?how far to push hero car away if hit
W $C249,2,2 Argument for routine passed in #REGde
W $C24B,2,2 Address of routine draw_tunnel_light_right
N $C24D Definition
B $C24D,1,1 Hit coord max/min (R/L)
B $C24E,1,1 Hit coord min/max (R/L)
B $C24F,1,1 ?how far to push hero car away if hit
W $C250,2,2 Argument for routine passed in #REGde
W $C252,2,2 Address of routine TBD
N $C254 Definition
B $C254,1,1 Hit coord max/min (R/L)
B $C255,1,1 Hit coord min/max (R/L)
B $C256,1,1 ?how far to push hero car away if hit
W $C257,2,2 Argument for routine passed in #REGde
W $C259,2,2 Address of routine draw_stretchy_object_right
N $C25B Definition
B $C25B,1,1 Hit coord max/min (R/L)
B $C25C,1,1 Hit coord min/max (R/L)
B $C25D,1,1 ?how far to push hero car away if hit
W $C25E,2,2 Argument for routine passed in #REGde
W $C260,2,2 Address of routine draw_stretchy_object_right
N $C262 Definition
B $C262,1,1 Hit coord max/min (R/L)
B $C263,1,1 Hit coord min/max (R/L)
B $C264,1,1 ?how far to push hero car away if hit
W $C265,2,2 Argument for routine passed in #REGde
W $C267,2,2 Address of routine draw_stretchy_object_right
N $C269 Definition
B $C269,1,1 Hit coord max/min (R/L)
B $C26A,1,1 Hit coord min/max (R/L)
B $C26B,1,1 ?how far to push hero car away if hit
W $C26C,2,2 Argument for routine passed in #REGde
W $C26E,2,2 Address of routine draw_stretchy_object_right
N $C270 Definition
B $C270,1,1 Hit coord max/min (R/L)
B $C271,1,1 Hit coord min/max (R/L)
B $C272,1,1 ?how far to push hero car away if hit
W $C273,2,2 Argument for routine passed in #REGde
W $C275,2,2 Address of routine draw_stretchy_object_right
B $C277,14,8,6
b $C285 [Stage 1] Graphic definition
N $C285 Definition
B $C285,1,1 Hit coord max/min (R/L)
B $C286,1,1 Hit coord min/max (R/L)
B $C287,1,1 ?how far to push hero car away if hit
W $C288,2,2 Argument for routine passed in #REGde
W $C28A,2,2 Address of routine draw_tunnel_light_left
N $C28C Definition
B $C28C,1,1 Hit coord max/min (R/L)
B $C28D,1,1 Hit coord min/max (R/L)
B $C28E,1,1 ?how far to push hero car away if hit
W $C28F,2,2 Argument for routine passed in #REGde
W $C291,2,2 Address of routine TBD
N $C293 Definition
B $C293,1,1 Hit coord max/min (R/L)
B $C294,1,1 Hit coord min/max (R/L)
B $C295,1,1 ?how far to push hero car away if hit
W $C296,2,2 Argument for routine passed in #REGde
W $C298,2,2 Address of routine draw_stretchy_object_left
N $C29A Definition
B $C29A,1,1 Hit coord max/min (R/L)
B $C29B,1,1 Hit coord min/max (R/L)
B $C29C,1,1 ?how far to push hero car away if hit
W $C29D,2,2 Argument for routine passed in #REGde
W $C29F,2,2 Address of routine draw_stretchy_object_left
N $C2A1 Definition
B $C2A1,1,1 Hit coord max/min (R/L)
B $C2A2,1,1 Hit coord min/max (R/L)
B $C2A3,1,1 ?how far to push hero car away if hit
W $C2A4,2,2 Argument for routine passed in #REGde
W $C2A6,2,2 Address of routine draw_stretchy_object_left
N $C2A8 Definition
B $C2A8,1,1 Hit coord max/min (R/L)
B $C2A9,1,1 Hit coord min/max (R/L)
B $C2AA,1,1 ?how far to push hero car away if hit
W $C2AB,2,2 Argument for routine passed in #REGde
W $C2AD,2,2 Address of routine draw_stretchy_object_left
N $C2AF Definition
B $C2AF,1,1 Hit coord max/min (R/L)
B $C2B0,1,1 Hit coord min/max (R/L)
B $C2B1,1,1 ?how far to push hero car away if hit
W $C2B2,2,2 Argument for routine passed in #REGde
W $C2B4,2,2 Address of routine draw_stretchy_object_left
B $C2B6,14,8,6
b $C2C4 [Stage 1] Map curvature data
B $C2C4,1,1 Curve Straight for 30 units
B $C2C5,1,1
B $C2C6,1,1 Curve Left for 29 units
B $C2C7,1,1
B $C2C8,1,1 Curve Right for 20 units
B $C2C9,1,1
B $C2CA,1,1 Curve Straight for 75 units
B $C2CB,4,4
B $C2CF,1,1 Curve Left Hard for 14 units
B $C2D0,1,1 Curve Left for 16 units
B $C2D1,1,1
B $C2D2,1,1 Curve Right for 31 units
B $C2D3,2,2
B $C2D5,1,1 Curve Straight for 24 units
B $C2D6,1,1
B $C2D7,1,1 Curve Left for 20 units
B $C2D8,1,1
B $C2D9,1,1 Curve Right Hard for 25 units
B $C2DA,1,1
B $C2DB,1,1 Curve Right for 11 units
B $C2DC,1,1 Curve Straight for 25 units
B $C2DD,1,1
B $C2DE,1,1 <Esc> Split
B $C2DF,1,1
W $C2E0,2,2 [$C3D2] Left target
W $C2E2,2,2 [$C488] Right target
b $C2E4 [Stage 1] Map height data
B $C2E4,1,1 Level Road for 82 units
B $C2E5,5,5
B $C2EA,1,1 Going Up 1 for 1 units
B $C2EB,1,1 Going Up 3 for 13 units
B $C2EC,1,1 Going Up 1 for 1 units
B $C2ED,1,1 Level Road for 1 units
B $C2EE,1,1 Going Down 1 for 1 units
B $C2EF,1,1 Going Down 3 for 24 units
B $C2F0,1,1
B $C2F1,1,1 Going Down 1 for 3 units
B $C2F2,1,1 Level Road for 2 units
B $C2F3,1,1 Going Up 3 for 2 units
B $C2F4,1,1 Going Up 5 for 8 units
B $C2F5,1,1 Going Down 5 for 8 units
B $C2F6,1,1 Going Down 3 for 2 units
B $C2F7,1,1 Level Road for 2 units
B $C2F8,1,1 Going Up 1 for 3 units
B $C2F9,1,1 Going Up 3 for 7 units
B $C2FA,1,1 Going Up 5 for 8 units
B $C2FB,1,1 Going Up 3 for 2 units
B $C2FC,1,1 Going Up 1 for 2 units
B $C2FD,1,1 Level Road for 6 units
B $C2FE,1,1 Going Down 1 for 2 units
B $C2FF,1,1 Going Down 3 for 14 units
B $C300,1,1 Going Down 1 for 15 units
B $C301,1,1 Level Road for 111 units
B $C302,7,7
B $C309,1,1 <Esc> Split
B $C30A,1,1
W $C30B,2,2 [$C3E6] Left target
W $C30D,2,2 [$C4A3] Right target
b $C30F [Stage 1] Map lanes data
B $C30F,1,1 3 Lanes L            [|||]  {81} for 30 units
B $C310,1,1
B $C311,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C312,1,1
B $C313,1,1 4 Lanes              [||||] {00} for 288 units
B $C314,3,3
B $C317,1,1 <Esc> Split
B $C318,1,1
W $C319,2,2 [$C403] Left target
W $C31B,2,2 [$C4E5] Right target
b $C31D [Stage 1] Map hazards data
B $C31D,1,1 Wait for 147 units
B $C31E,1,1 Disable Car Spawning
B $C31F,1,1
B $C320,1,1 Wait for 12 units
B $C321,1,1 Set Floating Arrow to Right
B $C322,1,1
B $C323,1,1 Wait for 1 units
B $C324,1,1 <Esc> Split
B $C325,1,1
W $C326,2,2 [$C409] Left target
W $C328,2,2 [$C4EF] Right target
b $C32A [Stage 1] Map left object data
B $C32A,1,1 BUSH for 1 units
B $C32B,1,1 (nothing) for 1 units
B $C32C,1,1 BUSH for 1 units
B $C32D,1,1 (nothing) for 3 units
B $C32E,1,1 BUSH for 1 units
B $C32F,1,1 (nothing) for 3 units
B $C330,1,1 BUSH for 1 units
B $C331,1,1 (nothing) for 3 units
B $C332,1,1 BUSH for 1 units
B $C333,1,1 (nothing) for 5 units
B $C334,1,1 BUSH for 1 units
B $C335,1,1 (nothing) for 1 units
B $C336,1,1 BUSH for 1 units
B $C337,1,1 (nothing) for 3 units
B $C338,1,1 BUSH for 1 units
B $C339,1,1 (nothing) for 7 units
B $C33A,1,1 TREE for 1 units
B $C33B,1,1 (nothing) for 3 units
B $C33C,1,1 TREE for 1 units
B $C33D,1,1 (nothing) for 5 units
B $C33E,1,1 TREE for 1 units
B $C33F,1,1 (nothing) for 1 units
B $C340,1,1 TREE for 1 units
B $C341,1,1 (nothing) for 1 units
B $C342,1,1 TREE for 1 units
B $C343,1,1 (nothing) for 11 units
B $C344,1,1 TREE for 1 units
B $C345,1,1 (nothing) for 7 units
B $C346,1,1 BUSH for 1 units
B $C347,1,1 (nothing) for 3 units
B $C348,1,1 BUSH for 1 units
B $C349,1,1 (nothing) for 3 units
B $C34A,1,1 TREE for 1 units
B $C34B,1,1 (nothing) for 3 units
B $C34C,1,1 TREE for 1 units
B $C34D,1,1 (nothing) for 3 units
B $C34E,1,1 TREE for 1 units
B $C34F,1,1 (nothing) for 5 units
B $C350,1,1 TREE for 1 units
B $C351,1,1 (nothing) for 5 units
B $C352,1,1 TREE for 1 units
B $C353,1,1 (nothing) for 5 units
B $C354,1,1 TREE for 1 units
B $C355,1,1 (nothing) for 3 units
B $C356,1,1 TREE for 1 units
B $C357,1,1 (nothing) for 3 units
B $C358,1,1 TREE for 1 units
B $C359,1,1 (nothing) for 3 units
B $C35A,1,1 BUSH for 1 units
B $C35B,1,1 (nothing) for 1 units
B $C35C,1,1 BUSH for 1 units
B $C35D,1,1 (nothing) for 5 units
B $C35E,1,1 STREET_LAMP for 1 units
B $C35F,1,1 (nothing) for 3 units
B $C360,1,1 STREET_LAMP for 1 units
B $C361,1,1 (nothing) for 3 units
B $C362,1,1 STREET_LAMP for 1 units
B $C363,1,1 (nothing) for 3 units
B $C364,1,1 STREET_LAMP for 1 units
B $C365,1,1 (nothing) for 3 units
B $C366,1,1 STREET_LAMP for 1 units
B $C367,1,1 (nothing) for 3 units
B $C368,1,1 STREET_LAMP for 1 units
B $C369,1,1 (nothing) for 3 units
B $C36A,1,1 STREET_LAMP for 1 units
B $C36B,1,1 (nothing) for 3 units
B $C36C,1,1 STREET_LAMP for 1 units
B $C36D,1,1 (nothing) for 3 units
B $C36E,1,1 STREET_LAMP for 1 units
B $C36F,1,1 (nothing) for 3 units
B $C370,1,1 STREET_LAMP for 1 units
B $C371,1,1 (nothing) for 1 units
B $C372,1,1 <Esc> Split
B $C373,1,1
W $C374,2,2 [$C411] Left target
W $C376,2,2 [$C509] Right target
b $C378 [Stage 1] Map right object data
B $C378,1,1 TREE for 1 units
B $C379,1,1 (nothing) for 3 units
B $C37A,1,1 TREE for 1 units
B $C37B,1,1 (nothing) for 3 units
B $C37C,1,1 TREE for 1 units
B $C37D,1,1 (nothing) for 1 units
B $C37E,1,1 TREE for 1 units
B $C37F,1,1 (nothing) for 9 units
B $C380,1,1 TREE for 1 units
B $C381,1,1 (nothing) for 1 units
B $C382,1,1 TREE for 1 units
B $C383,1,1 (nothing) for 1 units
B $C384,1,1 TREE for 1 units
B $C385,1,1 (nothing) for 3 units
B $C386,1,1 TREE for 1 units
B $C387,1,1 (nothing) for 3 units
B $C388,1,1 TREE for 1 units
B $C389,1,1 (nothing) for 7 units
B $C38A,1,1 TREE for 1 units
B $C38B,1,1 (nothing) for 3 units
B $C38C,1,1 TREE for 1 units
B $C38D,1,1 (nothing) for 1 units
B $C38E,1,1 TREE for 1 units
B $C38F,1,1 (nothing) for 1 units
B $C390,1,1 TREE for 1 units
B $C391,1,1 (nothing) for 7 units
B $C392,1,1 BUSH for 1 units
B $C393,1,1 (nothing) for 3 units
B $C394,1,1 BUSH for 1 units
B $C395,1,1 (nothing) for 1 units
B $C396,1,1 TREE for 1 units
B $C397,1,1 (nothing) for 3 units
B $C398,1,1 TREE for 1 units
B $C399,1,1 (nothing) for 1 units
B $C39A,1,1 TREE for 1 units
B $C39B,1,1 (nothing) for 1 units
B $C39C,1,1 TREE for 1 units
B $C39D,1,1 (nothing) for 1 units
B $C39E,1,1 TREE for 1 units
B $C39F,1,1 (nothing) for 1 units
B $C3A0,1,1 BUSH for 1 units
B $C3A1,1,1 (nothing) for 1 units
B $C3A2,1,1 BUSH for 1 units
B $C3A3,1,1 (nothing) for 3 units
B $C3A4,1,1 TREE for 1 units
B $C3A5,1,1 (nothing) for 3 units
B $C3A6,1,1 TREE for 1 units
B $C3A7,1,1 (nothing) for 3 units
B $C3A8,1,1 TREE for 1 units
B $C3A9,1,1 (nothing) for 7 units
B $C3AA,1,1 TREE for 1 units
B $C3AB,1,1 (nothing) for 1 units
B $C3AC,1,1 TREE for 1 units
B $C3AD,1,1 (nothing) for 1 units
B $C3AE,1,1 TREE for 1 units
B $C3AF,1,1 (nothing) for 1 units
B $C3B0,1,1 TREE for 1 units
B $C3B1,1,1 (nothing) for 5 units
B $C3B2,1,1 TREE for 1 units
B $C3B3,1,1 (nothing) for 1 units
B $C3B4,1,1 TREE for 1 units
B $C3B5,1,1 (nothing) for 5 units
B $C3B6,1,1 TREE for 1 units
B $C3B7,1,1 (nothing) for 5 units
B $C3B8,1,1 STREET_LAMP for 1 units
B $C3B9,1,1 (nothing) for 3 units
B $C3BA,1,1 STREET_LAMP for 1 units
B $C3BB,1,1 (nothing) for 3 units
B $C3BC,1,1 STREET_LAMP for 1 units
B $C3BD,1,1 (nothing) for 3 units
B $C3BE,1,1 STREET_LAMP for 1 units
B $C3BF,1,1 (nothing) for 3 units
B $C3C0,1,1 STREET_LAMP for 1 units
B $C3C1,1,1 (nothing) for 3 units
B $C3C2,1,1 STREET_LAMP for 1 units
B $C3C3,1,1 (nothing) for 3 units
B $C3C4,1,1 STREET_LAMP for 1 units
B $C3C5,1,1 (nothing) for 3 units
B $C3C6,1,1 STREET_LAMP for 1 units
B $C3C7,1,1 (nothing) for 3 units
B $C3C8,1,1 STREET_LAMP for 1 units
B $C3C9,1,1 (nothing) for 3 units
B $C3CA,1,1 STREET_LAMP for 1 units
B $C3CB,1,1 (nothing) for 1 units
B $C3CC,1,1 <Esc> Split
B $C3CD,1,1
W $C3CE,2,2 [$C452] Left target
W $C3D0,2,2 [$C543] Right target
b $C3D2 [Stage 1] Map curvature data
B $C3D2,1,1 Curve Straight for 51 units
B $C3D3,3,3
B $C3D6,1,1 Curve Right for 12 units
B $C3D7,1,1 Curve Right Hard for 17 units
B $C3D8,1,1
B $C3D9,1,1 Curve Right Very Hard for 30 units
B $C3DA,1,1
B $C3DB,1,1 Curve Right Hard for 12 units
B $C3DC,1,1 Curve Right for 14 units
B $C3DD,1,1 Curve Straight for 74 units
B $C3DE,4,4
B $C3E2,1,1 <Esc> Jump
B $C3E3,1,1
W $C3E4,2,2 [$C573] Target
b $C3E6 [Stage 1] Map height data
B $C3E6,1,1 Level Road for 13 units
B $C3E7,1,1 Going Up 1 for 4 units
B $C3E8,1,1 Going Up 3 for 4 units
B $C3E9,1,1 Going Up 5 for 6 units
B $C3EA,1,1 Going Up 7 for 11 units
B $C3EB,1,1 Going Up 5 for 2 units
B $C3EC,1,1 Going Up 3 for 3 units
B $C3ED,1,1 Going Up 1 for 2 units
B $C3EE,1,1 Level Road for 97 units
B $C3EF,6,6
B $C3F5,1,1 Going Down 1 for 3 units
B $C3F6,1,1 Going Down 3 for 4 units
B $C3F7,1,1 Going Down 5 for 6 units
B $C3F8,1,1 Going Down 7 for 7 units
B $C3F9,1,1 Going Down 5 for 7 units
B $C3FA,1,1 Going Down 3 for 5 units
B $C3FB,1,1 Going Down 1 for 3 units
B $C3FC,1,1 Level Road for 33 units
B $C3FD,2,2
B $C3FF,1,1 <Esc> Jump
B $C400,1,1
W $C401,2,2 [$C589] Target
b $C403 [Stage 1] Map lanes data
B $C403,1,1 4 Lanes              [||||] {00} for 210 units
B $C404,1,1
B $C405,1,1 <Esc> Jump
B $C406,1,1
W $C407,2,2 [$C5A5] Target
b $C409 [Stage 1] Map hazards data
B $C409,1,1 Wait for 10 units
B $C40A,1,1 Enable Car Spawning
B $C40B,1,1
B $C40C,1,1 Wait for 95 units
B $C40D,1,1 <Esc> Jump
B $C40E,1,1
W $C40F,2,2 [$C5B9] Target
b $C411 [Stage 1] Map left object data
B $C411,1,1 (nothing) for 4 units
B $C412,1,1 TREE for 1 units
B $C413,1,1 (nothing) for 1 units
B $C414,1,1 TREE for 1 units
B $C415,1,1 (nothing) for 1 units
B $C416,1,1 TREE for 1 units
B $C417,1,1 (nothing) for 3 units
B $C418,1,1 TREE for 1 units
B $C419,1,1 (nothing) for 1 units
B $C41A,1,1 TREE for 1 units
B $C41B,1,1 (nothing) for 3 units
B $C41C,1,1 TREE for 1 units
B $C41D,1,1 (nothing) for 1 units
B $C41E,1,1 TREE for 1 units
B $C41F,1,1 (nothing) for 1 units
B $C420,1,1 TREE for 1 units
B $C421,1,1 (nothing) for 3 units
B $C422,1,1 TELEGRAPH_POLE for 1 units
B $C423,1,1 (nothing) for 1 units
B $C424,1,1 TELEGRAPH_POLE for 1 units
B $C425,1,1 (nothing) for 1 units
B $C426,1,1 TELEGRAPH_POLE for 1 units
B $C427,1,1 (nothing) for 3 units
B $C428,1,1 TELEGRAPH_POLE for 1 units
B $C429,1,1 (nothing) for 1 units
B $C42A,1,1 TELEGRAPH_POLE for 1 units
B $C42B,1,1 (nothing) for 1 units
B $C42C,1,1 TELEGRAPH_POLE for 1 units
B $C42D,1,1 (nothing) for 3 units
B $C42E,1,1 TELEGRAPH_POLE for 1 units
B $C42F,1,1 (nothing) for 3 units
B $C430,1,1 TELEGRAPH_POLE for 1 units
B $C431,1,1 (nothing) for 3 units
B $C432,1,1 TELEGRAPH_POLE for 1 units
B $C433,1,1 (nothing) for 3 units
B $C434,1,1 TELEGRAPH_POLE for 1 units
B $C435,1,1 (nothing) for 1 units
B $C436,1,1 TELEGRAPH_POLE for 1 units
B $C437,1,1 (nothing) for 1 units
B $C438,1,1 TELEGRAPH_POLE for 1 units
B $C439,1,1 (nothing) for 1 units
B $C43A,1,1 TELEGRAPH_POLE for 1 units
B $C43B,1,1 (nothing) for 1 units
B $C43C,1,1 TELEGRAPH_POLE for 1 units
B $C43D,1,1 (nothing) for 7 units
B $C43E,1,1 TELEGRAPH_POLE for 1 units
B $C43F,1,1 (nothing) for 3 units
B $C440,1,1 TELEGRAPH_POLE for 1 units
B $C441,1,1 (nothing) for 15 units
B $C442,1,1 STREET_LAMP for 1 units
B $C443,1,1 (nothing) for 1 units
B $C444,1,1 STREET_LAMP for 1 units
B $C445,1,1 (nothing) for 1 units
B $C446,1,1 STREET_LAMP for 1 units
B $C447,1,1 (nothing) for 3 units
B $C448,1,1 STREET_LAMP for 1 units
B $C449,1,1 (nothing) for 1 units
B $C44A,1,1 STREET_LAMP for 1 units
B $C44B,1,1 (nothing) for 1 units
B $C44C,1,1 STREET_LAMP for 1 units
B $C44D,1,1 (nothing) for 2 units
B $C44E,1,1 <Esc> Jump
B $C44F,1,1
W $C450,2,2 [$C5BE] Target
b $C452 [Stage 1] Map right object data
B $C452,1,1 (nothing) for 31 units
B $C453,2,2
B $C455,1,1 TELEGRAPH_POLE for 1 units
B $C456,1,1 (nothing) for 1 units
B $C457,1,1 TELEGRAPH_POLE for 1 units
B $C458,1,1 (nothing) for 1 units
B $C459,1,1 TELEGRAPH_POLE for 1 units
B $C45A,1,1 (nothing) for 10 units
B $C45B,1,1 TELEGRAPH_POLE for 1 units
B $C45C,1,1 (nothing) for 3 units
B $C45D,1,1 TELEGRAPH_POLE for 1 units
B $C45E,1,1 (nothing) for 3 units
B $C45F,1,1 TELEGRAPH_POLE for 1 units
B $C460,1,1 (nothing) for 1 units
B $C461,1,1 TELEGRAPH_POLE for 1 units
B $C462,1,1 (nothing) for 3 units
B $C463,1,1 TELEGRAPH_POLE for 1 units
B $C464,1,1 (nothing) for 3 units
B $C465,1,1 TELEGRAPH_POLE for 1 units
B $C466,1,1 (nothing) for 3 units
B $C467,1,1 TELEGRAPH_POLE for 1 units
B $C468,1,1 (nothing) for 7 units
B $C469,1,1 STREET_LAMP for 1 units
B $C46A,1,1 (nothing) for 1 units
B $C46B,1,1 STREET_LAMP for 1 units
B $C46C,1,1 (nothing) for 1 units
B $C46D,1,1 STREET_LAMP for 1 units
B $C46E,1,1 (nothing) for 1 units
B $C46F,1,1 STREET_LAMP for 1 units
B $C470,1,1 (nothing) for 1 units
B $C471,1,1 STREET_LAMP for 1 units
B $C472,1,1 (nothing) for 1 units
B $C473,1,1 STREET_LAMP for 1 units
B $C474,1,1 (nothing) for 1 units
B $C475,1,1 STREET_LAMP for 1 units
B $C476,1,1 (nothing) for 1 units
B $C477,1,1 STREET_LAMP for 1 units
B $C478,1,1 (nothing) for 1 units
B $C479,1,1 STREET_LAMP for 1 units
B $C47A,1,1 (nothing) for 1 units
B $C47B,1,1 STREET_LAMP for 1 units
B $C47C,1,1 (nothing) for 3 units
B $C47D,1,1 STREET_LAMP for 1 units
B $C47E,1,1 (nothing) for 1 units
B $C47F,1,1 STREET_LAMP for 1 units
B $C480,1,1 (nothing) for 1 units
B $C481,1,1 STREET_LAMP for 1 units
B $C482,1,1 (nothing) for 1 units
B $C483,1,1 STREET_LAMP for 1 units
B $C484,1,1 <Esc> Jump
B $C485,1,1
W $C486,2,2 [$C5C9] Target
b $C488 [Stage 1] Map curvature data
B $C488,1,1 Curve Straight for 23 units
B $C489,1,1
B $C48A,1,1 Curve Left for 17 units
B $C48B,1,1
B $C48C,1,1 Curve Left Hard for 41 units
B $C48D,2,2
B $C48F,1,1 Curve Left for 6 units
B $C490,1,1 Curve Straight for 20 units
B $C491,1,1
B $C492,1,1 Curve Right for 7 units
B $C493,1,1 Curve Right Hard for 11 units
B $C494,1,1 Curve Right for 4 units
B $C495,1,1 Curve Left for 4 units
B $C496,1,1 Curve Left Hard for 19 units
B $C497,1,1
B $C498,1,1 Curve Left Very Hard for 15 units
B $C499,1,1 Curve Left Hard for 8 units
B $C49A,1,1 Curve Right Hard for 9 units
B $C49B,1,1 Curve Right Very Hard for 7 units
B $C49C,1,1 Curve Right for 2 units
B $C49D,1,1 Curve Straight for 17 units
B $C49E,1,1
B $C49F,1,1 <Esc> Jump
B $C4A0,1,1
W $C4A1,2,2 [$C573] Target
b $C4A3 [Stage 1] Map height data
B $C4A3,1,1 Level Road for 20 units
B $C4A4,1,1
B $C4A5,1,1 Going Up 3 for 5 units
B $C4A6,1,1 Level Road for 1 units
B $C4A7,1,1 Going Down 3 for 5 units
B $C4A8,1,1 Level Road for 1 units
B $C4A9,1,1 Going Up 3 for 5 units
B $C4AA,1,1 Level Road for 1 units
B $C4AB,1,1 Going Down 3 for 5 units
B $C4AC,1,1 Level Road for 1 units
B $C4AD,1,1 Going Up 3 for 5 units
B $C4AE,1,1 Level Road for 1 units
B $C4AF,1,1 Going Down 3 for 5 units
B $C4B0,1,1 Level Road for 1 units
B $C4B1,1,1 Going Up 3 for 5 units
B $C4B2,1,1 Level Road for 1 units
B $C4B3,1,1 Going Down 3 for 5 units
B $C4B4,1,1 Level Road for 1 units
B $C4B5,1,1 Going Up 3 for 5 units
B $C4B6,1,1 Level Road for 1 units
B $C4B7,1,1 Going Down 3 for 5 units
B $C4B8,1,1 Level Road for 1 units
B $C4B9,1,1 Going Up 3 for 5 units
B $C4BA,1,1 Level Road for 1 units
B $C4BB,1,1 Going Down 3 for 5 units
B $C4BC,1,1 Level Road for 1 units
B $C4BD,1,1 Going Up 3 for 5 units
B $C4BE,1,1 Level Road for 1 units
B $C4BF,1,1 Going Down 3 for 5 units
B $C4C0,1,1 Level Road for 1 units
B $C4C1,1,1 Going Up 3 for 5 units
B $C4C2,1,1 Level Road for 1 units
B $C4C3,1,1 Going Down 3 for 5 units
B $C4C4,1,1 Level Road for 1 units
B $C4C5,1,1 Going Up 3 for 5 units
B $C4C6,1,1 Level Road for 1 units
B $C4C7,1,1 Going Down 3 for 5 units
B $C4C8,1,1 Level Road for 1 units
B $C4C9,1,1 Going Up 3 for 5 units
B $C4CA,1,1 Level Road for 1 units
B $C4CB,1,1 Going Down 3 for 5 units
B $C4CC,1,1 Level Road for 1 units
B $C4CD,1,1 Going Up 3 for 5 units
B $C4CE,1,1 Level Road for 1 units
B $C4CF,1,1 Going Down 3 for 5 units
B $C4D0,1,1 Level Road for 1 units
B $C4D1,1,1 Going Up 3 for 5 units
B $C4D2,1,1 Level Road for 1 units
B $C4D3,1,1 Going Down 3 for 5 units
B $C4D4,1,1 Level Road for 1 units
B $C4D5,1,1 Going Up 3 for 5 units
B $C4D6,1,1 Level Road for 1 units
B $C4D7,1,1 Going Down 3 for 5 units
B $C4D8,1,1 Level Road for 1 units
B $C4D9,1,1 Going Up 3 for 5 units
B $C4DA,1,1 Level Road for 1 units
B $C4DB,1,1 Going Down 3 for 5 units
B $C4DC,1,1 Level Road for 1 units
B $C4DD,1,1 Going Up 3 for 5 units
B $C4DE,1,1 Level Road for 1 units
B $C4DF,1,1 Going Down 3 for 5 units
B $C4E0,1,1 Level Road for 11 units
B $C4E1,1,1 <Esc> Jump
B $C4E2,1,1
W $C4E3,2,2 [$C589] Target
b $C4E5 [Stage 1] Map lanes data
B $C4E5,1,1 4 Lanes              [||||] {00} for 20 units
B $C4E6,1,1
B $C4E7,1,1 4 Lanes dirt track   [||||] {C1} for 178 units
B $C4E8,1,1
B $C4E9,1,1 4 Lanes              [||||] {00} for 12 units
B $C4EA,1,1
B $C4EB,1,1 <Esc> Jump
B $C4EC,1,1
W $C4ED,2,2 [$C5A5] Target
b $C4EF [Stage 1] Map hazards data
B $C4EF,1,1 Wait for 10 units
B $C4F0,1,1 Start Spawning Two Barriers
B $C4F1,1,1
B $C4F2,1,1 Wait for 1 units
B $C4F3,1,1 Stop Spawning Barriers 6?
B $C4F4,1,1
B $C4F5,1,1 Wait for 41 units
B $C4F6,1,1 Start Spawning Two Barriers
B $C4F7,1,1
B $C4F8,1,1 Wait for 4 units
B $C4F9,1,1 Stop Spawning Barriers 6?
B $C4FA,1,1
B $C4FB,1,1 Wait for 42 units
B $C4FC,1,1 Start Spawning Two Barriers
B $C4FD,1,1
B $C4FE,1,1 Wait for 2 units
B $C4FF,1,1 Stop Spawning Barriers 3?
B $C500,1,1
B $C501,1,1 Wait for 1 units
B $C502,1,1 Enable Car Spawning
B $C503,1,1
B $C504,1,1 Wait for 4 units
B $C505,1,1 <Esc> Jump
B $C506,1,1
W $C507,2,2 [$C5B9] Target
b $C509 [Stage 1] Map left object data
B $C509,1,1 (nothing) for 10 units
B $C50A,1,1 SHORT_POLE for 1 units
B $C50B,1,1 (nothing) for 7 units
B $C50C,1,1 TELEGRAPH_POLE for 1 units
B $C50D,1,1 (nothing) for 1 units
B $C50E,1,1 TELEGRAPH_POLE for 1 units
B $C50F,1,1 (nothing) for 1 units
B $C510,1,1 TELEGRAPH_POLE for 1 units
B $C511,1,1 (nothing) for 1 units
B $C512,1,1 TELEGRAPH_POLE for 1 units
B $C513,1,1 (nothing) for 1 units
B $C514,1,1 TELEGRAPH_POLE for 1 units
B $C515,1,1 (nothing) for 1 units
B $C516,1,1 TELEGRAPH_POLE for 1 units
B $C517,1,1 (nothing) for 5 units
B $C518,1,1 TELEGRAPH_POLE for 1 units
B $C519,1,1 (nothing) for 1 units
B $C51A,1,1 TELEGRAPH_POLE for 1 units
B $C51B,1,1 (nothing) for 5 units
B $C51C,1,1 TELEGRAPH_POLE for 1 units
B $C51D,1,1 (nothing) for 3 units
B $C51E,1,1 TELEGRAPH_POLE for 1 units
B $C51F,1,1 (nothing) for 1 units
B $C520,1,1 TELEGRAPH_POLE for 1 units
B $C521,1,1 (nothing) for 1 units
B $C522,1,1 TELEGRAPH_POLE for 1 units
B $C523,1,1 (nothing) for 1 units
B $C524,1,1 TELEGRAPH_POLE for 1 units
B $C525,1,1 (nothing) for 3 units
B $C526,1,1 TELEGRAPH_POLE for 1 units
B $C527,1,1 (nothing) for 1 units
B $C528,1,1 TELEGRAPH_POLE for 1 units
B $C529,1,1 (nothing) for 3 units
B $C52A,1,1 TELEGRAPH_POLE for 1 units
B $C52B,1,1 (nothing) for 1 units
B $C52C,1,1 TELEGRAPH_POLE for 1 units
B $C52D,1,1 (nothing) for 7 units
B $C52E,1,1 TELEGRAPH_POLE for 1 units
B $C52F,1,1 (nothing) for 1 units
B $C530,1,1 TELEGRAPH_POLE for 1 units
B $C531,1,1 (nothing) for 1 units
B $C532,1,1 TELEGRAPH_POLE for 1 units
B $C533,1,1 (nothing) for 1 units
B $C534,1,1 TELEGRAPH_POLE for 1 units
B $C535,1,1 (nothing) for 13 units
B $C536,1,1 TELEGRAPH_POLE for 1 units
B $C537,1,1 (nothing) for 1 units
B $C538,1,1 TELEGRAPH_POLE for 1 units
B $C539,1,1 (nothing) for 1 units
B $C53A,1,1 TELEGRAPH_POLE for 1 units
B $C53B,1,1 (nothing) for 5 units
B $C53C,1,1 STREET_LAMP for 1 units
B $C53D,1,1 (nothing) for 1 units
B $C53E,1,1 STREET_LAMP for 1 units
B $C53F,1,1 <Esc> Jump
B $C540,1,1
W $C541,2,2 [$C5BE] Target
b $C543 [Stage 1] Map right object data
B $C543,1,1 STREET_LAMP for 1 units
B $C544,1,1 (nothing) for 3 units
B $C545,1,1 STREET_LAMP for 1 units
B $C546,1,1 (nothing) for 3 units
B $C547,1,1 STREET_LAMP for 1 units
B $C548,1,1 (nothing) for 1 units
B $C549,1,1 SHORT_POLE for 1 units
B $C54A,1,1 (nothing) for 11 units
B $C54B,1,1 TELEGRAPH_POLE for 1 units
B $C54C,1,1 (nothing) for 1 units
B $C54D,1,1 TELEGRAPH_POLE for 1 units
B $C54E,1,1 (nothing) for 1 units
B $C54F,1,1 TELEGRAPH_POLE for 1 units
B $C550,1,1 (nothing) for 1 units
B $C551,1,1 TELEGRAPH_POLE for 1 units
B $C552,1,1 (nothing) for 9 units
B $C553,1,1 TELEGRAPH_POLE for 1 units
B $C554,1,1 (nothing) for 1 units
B $C555,1,1 TELEGRAPH_POLE for 1 units
B $C556,1,1 (nothing) for 7 units
B $C557,1,1 TELEGRAPH_POLE for 1 units
B $C558,1,1 (nothing) for 15 units
B $C559,1,1 TELEGRAPH_POLE for 1 units
B $C55A,1,1 (nothing) for 1 units
B $C55B,1,1 TELEGRAPH_POLE for 1 units
B $C55C,1,1 (nothing) for 1 units
B $C55D,1,1 TELEGRAPH_POLE for 1 units
B $C55E,1,1 (nothing) for 1 units
B $C55F,1,1 TELEGRAPH_POLE for 1 units
B $C560,1,1 (nothing) for 1 units
B $C561,1,1 TELEGRAPH_POLE for 1 units
B $C562,1,1 (nothing) for 1 units
B $C563,1,1 TELEGRAPH_POLE for 1 units
B $C564,1,1 (nothing) for 1 units
B $C565,1,1 TELEGRAPH_POLE for 1 units
B $C566,1,1 (nothing) for 1 units
B $C567,1,1 TELEGRAPH_POLE for 1 units
B $C568,1,1 (nothing) for 1 units
B $C569,1,1 TELEGRAPH_POLE for 1 units
B $C56A,1,1 (nothing) for 1 units
B $C56B,1,1 TELEGRAPH_POLE for 1 units
B $C56C,1,1 (nothing) for 13 units
B $C56D,1,1 TELEGRAPH_POLE for 1 units
B $C56E,1,1 (nothing) for 8 units
B $C56F,1,1 <Esc> Jump
B $C570,1,1
W $C571,2,2 [$C5C9] Target
b $C573 [Stage 1] Map curvature data
B $C573,1,1 Curve Straight for 35 units
B $C574,2,2
B $C576,1,1 Curve Left Hard for 12 units
B $C577,1,1 Curve Right for 1 units
B $C578,1,1 Curve Right Hard for 10 units
B $C579,1,1 Curve Right for 6 units
B $C57A,1,1 Curve Straight for 8 units
B $C57B,1,1 Curve Left for 16 units
B $C57C,1,1
B $C57D,1,1 Curve Right for 25 units
B $C57E,1,1
B $C57F,1,1 Curve Right Hard for 15 units
B $C580,1,1 Curve Right for 26 units
B $C581,1,1
B $C582,1,1 Curve Straight for 36 units
B $C583,2,2
B $C585,1,1 <Esc> Jump
B $C586,1,1
W $C587,2,2 [$C5D4] Target
b $C589 [Stage 1] Map height data
B $C589,1,1 Going Down 5 for 15 units
B $C58A,1,1 Going Down 3 for 3 units
B $C58B,1,1 Level Road for 4 units
B $C58C,1,1 Going Up 7 for 15 units
B $C58D,1,1 Going Up 5 for 1 units
B $C58E,1,1 Level Road for 28 units
B $C58F,1,1
B $C590,1,1 Going Up 3 for 20 units
B $C591,1,1
B $C592,1,1 Going Up 5 for 6 units
B $C593,1,1 Going Up 7 for 8 units
B $C594,1,1 Going Up 5 for 2 units
B $C595,1,1 Going Up 3 for 2 units
B $C596,1,1 Level Road for 1 units
B $C597,1,1 Going Down 1 for 2 units
B $C598,1,1 Going Down 3 for 4 units
B $C599,1,1 Going Down 5 for 4 units
B $C59A,1,1 Going Down 7 for 11 units
B $C59B,1,1 Going Down 5 for 10 units
B $C59C,1,1 Going Down 3 for 11 units
B $C59D,1,1 Going Down 1 for 2 units
B $C59E,1,1 Level Road for 41 units
B $C59F,2,2
B $C5A1,1,1 <Esc> Jump
B $C5A2,1,1
W $C5A3,2,2 [$C609] Target
b $C5A5 [Stage 1] Map lanes data
B $C5A5,1,1 4 Lanes              [||||] {00} for 44 units
B $C5A6,1,1
B $C5A7,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C5A8,1,1
B $C5A9,1,1 3 Lanes L            [|||]  {81} for 12 units
B $C5AA,1,1
B $C5AB,1,1 Tunnel start                {45} for 102 units
B $C5AC,1,1
B $C5AD,1,1 Tunnel cont/end?            {59} for 2 units
B $C5AE,1,1
B $C5AF,1,1 3 Lanes L            [|||]  {81} for 18 units
B $C5B0,1,1
B $C5B1,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C5B2,1,1
B $C5B3,1,1 4 Lanes              [||||] {00} for 8 units
B $C5B4,1,1
B $C5B5,1,1 <Esc> Jump
B $C5B6,1,1
W $C5B7,2,2 [$C65D] Target
b $C5B9 [Stage 1] Map hazards data
B $C5B9,1,1 Wait for 95 units
B $C5BA,1,1 <Esc> Jump
B $C5BB,1,1
W $C5BC,2,2 [$C677] Target
b $C5BE [Stage 1] Map left object data
B $C5BE,1,1 (nothing) for 30 units
B $C5BF,1,1
B $C5C0,1,1 TUNNEL_LIGHT for 51 units
B $C5C1,3,3
B $C5C4,1,1 (nothing) for 14 units
B $C5C5,1,1 <Esc> Jump
B $C5C6,1,1
W $C5C7,2,2 [$C694] Target
b $C5C9 [Stage 1] Map right object data
B $C5C9,1,1 (nothing) for 30 units
B $C5CA,1,1
B $C5CB,1,1 TUNNEL_LIGHT for 51 units
B $C5CC,3,3
B $C5CF,1,1 (nothing) for 14 units
B $C5D0,1,1 <Esc> Jump
B $C5D1,1,1
W $C5D2,2,2 [$C714] Target
b $C5D4 [Stage 1] Map curvature data
B $C5D4,1,1 Curve Straight for 3 units
B $C5D5,1,1 Curve Left for 23 units
B $C5D6,1,1
B $C5D7,1,1 Curve Right for 20 units
B $C5D8,1,1
B $C5D9,1,1 Curve Straight for 9 units
B $C5DA,1,1 Curve Left for 11 units
B $C5DB,1,1 Curve Right for 20 units
B $C5DC,1,1
B $C5DD,1,1 Curve Right Hard for 11 units
B $C5DE,1,1 Curve Straight for 1 units
B $C5DF,1,1 Curve Right Hard for 23 units
B $C5E0,1,1
B $C5E1,1,1 Curve Right for 9 units
B $C5E2,1,1 Curve Left for 9 units
B $C5E3,1,1 Curve Left Hard for 22 units
B $C5E4,1,1
B $C5E5,1,1 Curve Straight for 1 units
B $C5E6,1,1 Curve Left Hard for 18 units
B $C5E7,1,1
B $C5E8,1,1 Curve Left for 12 units
B $C5E9,1,1 Curve Straight for 17 units
B $C5EA,1,1
B $C5EB,1,1 Curve Left Hard for 5 units
B $C5EC,1,1 Curve Left Very Hard for 2 units
B $C5ED,1,1 Curve Left Hard for 2 units
B $C5EE,1,1 Curve Left for 2 units
B $C5EF,1,1 Curve Right for 2 units
B $C5F0,1,1 Curve Right Hard for 2 units
B $C5F1,1,1 Curve Right Very Hard for 10 units
B $C5F2,1,1 Curve Right Hard for 5 units
B $C5F3,1,1 Curve Right for 10 units
B $C5F4,1,1 Curve Straight for 153 units
B $C5F5,10,8,2
B $C5FF,1,1 Curve Right for 19 units
B $C600,1,1
B $C601,1,1 Curve Right Hard for 12 units
B $C602,1,1 Curve Right for 6 units
B $C603,1,1 Curve Left for 5 units
B $C604,1,1 Curve Straight for 6 units
B $C605,1,1 <Esc> Loop
B $C606,1,1
W $C607,2,2 [$C5D4] Target
b $C609 [Stage 1] Map height data
B $C609,1,1 Level Road for 55 units
B $C60A,3,3
B $C60D,1,1 Going Up 3 for 4 units
B $C60E,1,1 Going Up 5 for 10 units
B $C60F,1,1 Going Down 5 for 12 units
B $C610,1,1 Going Down 3 for 4 units
B $C611,1,1 Level Road for 51 units
B $C612,3,3
B $C615,1,1 Going Up 1 for 4 units
B $C616,1,1 Going Up 3 for 5 units
B $C617,1,1 Going Up 5 for 18 units
B $C618,1,1
B $C619,1,1 Going Up 7 for 15 units
B $C61A,1,1 Going Down 3 for 1 units
B $C61B,1,1 Going Down 7 for 6 units
B $C61C,1,1 Going Down 5 for 22 units
B $C61D,1,1
B $C61E,1,1 Going Down 3 for 45 units
B $C61F,2,2
B $C621,1,1 Going Down 1 for 4 units
B $C622,1,1 Level Road for 3 units
B $C623,1,1 Going Up 1 for 5 units
B $C624,1,1 Going Up 3 for 4 units
B $C625,1,1 Going Up 5 for 1 units
B $C626,1,1 Going Up 7 for 1 units
B $C627,1,1 Going Up 5 for 12 units
B $C628,1,1 Going Up 7 for 2 units
B $C629,1,1 Going Down 7 for 6 units
B $C62A,1,1 Going Down 5 for 3 units
B $C62B,1,1 Going Down 3 for 2 units
B $C62C,1,1 Going Down 1 for 1 units
B $C62D,1,1 Level Road for 2 units
B $C62E,1,1 Going Up 1 for 1 units
B $C62F,1,1 Going Up 3 for 2 units
B $C630,1,1 Going Up 5 for 3 units
B $C631,1,1 Going Up 7 for 5 units
B $C632,1,1 Going Up 5 for 2 units
B $C633,1,1 Going Up 3 for 1 units
B $C634,1,1 Going Up 1 for 1 units
B $C635,1,1 Level Road for 1 units
B $C636,1,1 Going Down 1 for 1 units
B $C637,1,1 Going Down 3 for 2 units
B $C638,1,1 Going Down 5 for 3 units
B $C639,1,1 Going Down 3 for 5 units
B $C63A,1,1 Going Down 1 for 2 units
B $C63B,1,1 Level Road for 14 units
B $C63C,1,1 Going Up 1 for 1 units
B $C63D,1,1 Going Up 3 for 1 units
B $C63E,1,1 Going Up 5 for 2 units
B $C63F,1,1 Going Up 3 for 2 units
B $C640,1,1 Going Up 1 for 1 units
B $C641,1,1 Going Down 1 for 1 units
B $C642,1,1 Going Down 3 for 2 units
B $C643,1,1 Going Down 5 for 2 units
B $C644,1,1 Going Down 3 for 1 units
B $C645,1,1 Going Down 1 for 1 units
B $C646,1,1 Level Road for 10 units
B $C647,1,1 Going Down 1 for 1 units
B $C648,1,1 Going Down 3 for 1 units
B $C649,1,1 Going Down 5 for 2 units
B $C64A,1,1 Going Down 3 for 1 units
B $C64B,1,1 Going Down 1 for 1 units
B $C64C,1,1 Level Road for 1 units
B $C64D,1,1 Going Up 1 for 1 units
B $C64E,1,1 Going Up 3 for 1 units
B $C64F,1,1 Going Up 5 for 2 units
B $C650,1,1 Going Up 3 for 1 units
B $C651,1,1 Going Up 1 for 1 units
B $C652,1,1 Level Road for 67 units
B $C653,4,4
B $C657,1,1 Going Down 3 for 2 units
B $C658,1,1 Level Road for 3 units
B $C659,1,1 <Esc> Loop
B $C65A,1,1
W $C65B,2,2 [$C609] Target
b $C65D [Stage 1] Map lanes data
B $C65D,1,1 4 Lanes              [||||] {00} for 290 units
B $C65E,3,3
B $C661,1,1 4-3 Narrowing R      [/|||] {8E} for 34 units
B $C662,1,1
B $C663,1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C664,1,1
B $C665,1,1 4 Lanes              [||||] {00} for 30 units
B $C666,1,1
B $C667,1,1 3 Lanes L            [|||]  {81} for 10 units
B $C668,1,1
B $C669,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C66A,1,1
B $C66B,1,1 4 Lanes              [||||] {00} for 22 units
B $C66C,1,1
B $C66D,1,1 4-3 Narrowing R      [/|||] {8E} for 6 units
B $C66E,1,1
B $C66F,1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C670,1,1
B $C671,1,1 4 Lanes              [||||] {00} for 52 units
B $C672,1,1
B $C673,1,1 <Esc> Loop
B $C674,1,1
W $C675,2,2 [$C65D] Target
b $C677 [Stage 1] Map hazards data
B $C677,1,1 Wait for 42 units
B $C678,1,1 Start Spawning Barriers Left
B $C679,1,1
B $C67A,1,1 Wait for 2 units
B $C67B,1,1 Stop Spawning Barriers 3?
B $C67C,1,1
B $C67D,1,1 Wait for 4 units
B $C67E,1,1 Start Spawning Barriers Left
B $C67F,1,1
B $C680,1,1 Wait for 2 units
B $C681,1,1 Stop Spawning Barriers 3?
B $C682,1,1
B $C683,1,1 Wait for 27 units
B $C684,1,1 Start Spawning Barriers Right
B $C685,1,1
B $C686,1,1 Wait for 2 units
B $C687,1,1 Stop Spawning Barriers 3?
B $C688,1,1
B $C689,1,1 Wait for 99 units
B $C68A,1,1 Start Spawning Barriers Right
B $C68B,1,1
B $C68C,1,1 Wait for 1 units
B $C68D,1,1 Stop Spawning Barriers 3?
B $C68E,1,1
B $C68F,1,1 Wait for 46 units
B $C690,1,1 <Esc> Loop
B $C691,1,1
W $C692,2,2 [$C677] Target
b $C694 [Stage 1] Map left object data
B $C694,1,1 (nothing) for 3 units
B $C695,1,1 STREET_LAMP for 1 units
B $C696,1,1 (nothing) for 1 units
B $C697,1,1 STREET_LAMP for 1 units
B $C698,1,1 (nothing) for 1 units
B $C699,1,1 STREET_LAMP for 1 units
B $C69A,1,1 (nothing) for 6 units
B $C69B,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C69C,1,1 (nothing) for 1 units
B $C69D,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C69E,1,1 (nothing) for 1 units
B $C69F,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C6A0,1,1 (nothing) for 5 units
B $C6A1,1,1 TELEGRAPH_POLE for 1 units
B $C6A2,1,1 (nothing) for 1 units
B $C6A3,1,1 TELEGRAPH_POLE for 1 units
B $C6A4,1,1 (nothing) for 1 units
B $C6A5,1,1 TELEGRAPH_POLE for 1 units
B $C6A6,1,1 (nothing) for 1 units
B $C6A7,1,1 TELEGRAPH_POLE for 1 units
B $C6A8,1,1 (nothing) for 3 units
B $C6A9,1,1 TELEGRAPH_POLE for 1 units
B $C6AA,1,1 (nothing) for 1 units
B $C6AB,1,1 TELEGRAPH_POLE for 1 units
B $C6AC,1,1 (nothing) for 3 units
B $C6AD,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C6AE,1,1 (nothing) for 1 units
B $C6AF,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C6B0,1,1 (nothing) for 1 units
B $C6B1,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C6B2,1,1 (nothing) for 1 units
B $C6B3,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C6B4,1,1 (nothing) for 9 units
B $C6B5,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C6B6,1,1 (nothing) for 1 units
B $C6B7,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C6B8,1,1 (nothing) for 1 units
B $C6B9,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C6BA,1,1 (nothing) for 3 units
B $C6BB,1,1 TELEGRAPH_POLE for 1 units
B $C6BC,1,1 (nothing) for 1 units
B $C6BD,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $C6BE,1,1 (nothing) for 1 units
B $C6BF,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $C6C0,1,1 (nothing) for 3 units
B $C6C1,1,1 TELEGRAPH_POLE for 1 units
B $C6C2,1,1 (nothing) for 1 units
B $C6C3,1,1 TELEGRAPH_POLE for 1 units
B $C6C4,1,1 (nothing) for 7 units
B $C6C5,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $C6C6,1,1 (nothing) for 1 units
B $C6C7,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $C6C8,1,1 (nothing) for 15 units
B $C6C9,1,1 STREET_LAMP for 1 units
B $C6CA,1,1 (nothing) for 1 units
B $C6CB,1,1 STREET_LAMP for 1 units
B $C6CC,1,1 (nothing) for 1 units
B $C6CD,1,1 STREET_LAMP for 1 units
B $C6CE,1,1 (nothing) for 3 units
B $C6CF,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C6D0,1,1 (nothing) for 1 units
B $C6D1,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C6D2,1,1 (nothing) for 1 units
B $C6D3,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C6D4,1,1 (nothing) for 3 units
B $C6D5,1,1 STREET_LAMP for 1 units
B $C6D6,1,1 (nothing) for 1 units
B $C6D7,1,1 STREET_LAMP for 1 units
B $C6D8,1,1 (nothing) for 1 units
B $C6D9,1,1 STREET_LAMP for 1 units
B $C6DA,1,1 (nothing) for 11 units
B $C6DB,1,1 TREE for 1 units
B $C6DC,1,1 (nothing) for 3 units
B $C6DD,1,1 TREE for 1 units
B $C6DE,1,1 (nothing) for 3 units
B $C6DF,1,1 TREE for 1 units
B $C6E0,1,1 (nothing) for 1 units
B $C6E1,1,1 BUSH for 1 units
B $C6E2,1,1 (nothing) for 1 units
B $C6E3,1,1 BUSH for 1 units
B $C6E4,1,1 (nothing) for 5 units
B $C6E5,1,1 TREE for 1 units
B $C6E6,1,1 (nothing) for 1 units
B $C6E7,1,1 TREE for 1 units
B $C6E8,1,1 (nothing) for 5 units
B $C6E9,1,1 TREE for 1 units
B $C6EA,1,1 (nothing) for 1 units
B $C6EB,1,1 TREE for 1 units
B $C6EC,1,1 (nothing) for 5 units
B $C6ED,1,1 BUSH for 1 units
B $C6EE,1,1 (nothing) for 1 units
B $C6EF,1,1 BUSH for 1 units
B $C6F0,1,1 (nothing) for 1 units
B $C6F1,1,1 BUSH for 1 units
B $C6F2,1,1 (nothing) for 3 units
B $C6F3,1,1 TREE for 1 units
B $C6F4,1,1 (nothing) for 3 units
B $C6F5,1,1 TREE for 1 units
B $C6F6,1,1 (nothing) for 3 units
B $C6F7,1,1 TREE for 1 units
B $C6F8,1,1 (nothing) for 1 units
B $C6F9,1,1 TREE for 1 units
B $C6FA,1,1 (nothing) for 3 units
B $C6FB,1,1 STREET_LAMP for 1 units
B $C6FC,1,1 (nothing) for 1 units
B $C6FD,1,1 STREET_LAMP for 1 units
B $C6FE,1,1 (nothing) for 1 units
B $C6FF,1,1 STREET_LAMP for 1 units
B $C700,1,1 (nothing) for 3 units
B $C701,1,1 STREET_LAMP for 1 units
B $C702,1,1 (nothing) for 9 units
B $C703,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C704,1,1 (nothing) for 1 units
B $C705,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C706,1,1 (nothing) for 1 units
B $C707,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C708,1,1 (nothing) for 1 units
B $C709,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C70A,1,1 (nothing) for 1 units
B $C70B,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C70C,1,1 (nothing) for 7 units
B $C70D,1,1 STREET_LAMP for 1 units
B $C70E,1,1 (nothing) for 1 units
B $C70F,1,1 STREET_LAMP for 1 units
B $C710,1,1 <Esc> Loop
B $C711,1,1
W $C712,2,2 [$C694] Target
b $C714 [Stage 1] Map right object data
B $C714,1,1 (nothing) for 2 units
B $C715,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $C716,1,1 (nothing) for 1 units
B $C717,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $C718,1,1 (nothing) for 1 units
B $C719,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $C71A,1,1 (nothing) for 1 units
B $C71B,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $C71C,1,1 (nothing) for 3 units
B $C71D,1,1 TELEGRAPH_POLE for 1 units
B $C71E,1,1 (nothing) for 1 units
B $C71F,1,1 TELEGRAPH_POLE for 1 units
B $C720,1,1 (nothing) for 1 units
B $C721,1,1 TELEGRAPH_POLE for 1 units
B $C722,1,1 (nothing) for 7 units
B $C723,1,1 TELEGRAPH_POLE for 1 units
B $C724,1,1 (nothing) for 1 units
B $C725,1,1 TELEGRAPH_POLE for 1 units
B $C726,1,1 (nothing) for 1 units
B $C727,1,1 TELEGRAPH_POLE for 1 units
B $C728,1,1 (nothing) for 1 units
B $C729,1,1 TELEGRAPH_POLE for 1 units
B $C72A,1,1 (nothing) for 1 units
B $C72B,1,1 TELEGRAPH_POLE for 1 units
B $C72C,1,1 (nothing) for 5 units
B $C72D,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C72E,1,1 (nothing) for 1 units
B $C72F,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C730,1,1 (nothing) for 1 units
B $C731,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C732,1,1 (nothing) for 1 units
B $C733,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C734,1,1 (nothing) for 1 units
B $C735,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C736,1,1 (nothing) for 1 units
B $C737,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C738,1,1 (nothing) for 15 units
B $C739,1,1 TELEGRAPH_POLE for 1 units
B $C73A,1,1 (nothing) for 3 units
B $C73B,1,1 TELEGRAPH_POLE for 1 units
B $C73C,1,1 (nothing) for 3 units
B $C73D,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $C73E,1,1 (nothing) for 1 units
B $C73F,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $C740,1,1 (nothing) for 1 units
B $C741,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $C742,1,1 (nothing) for 1 units
B $C743,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $C744,1,1 (nothing) for 7 units
B $C745,1,1 STREET_LAMP for 1 units
B $C746,1,1 (nothing) for 1 units
B $C747,1,1 STREET_LAMP for 1 units
B $C748,1,1 (nothing) for 1 units
B $C749,1,1 STREET_LAMP for 1 units
B $C74A,1,1 (nothing) for 1 units
B $C74B,1,1 STREET_LAMP for 1 units
B $C74C,1,1 (nothing) for 3 units
B $C74D,1,1 STREET_LAMP for 1 units
B $C74E,1,1 (nothing) for 3 units
B $C74F,1,1 STREET_LAMP for 1 units
B $C750,1,1 (nothing) for 9 units
B $C751,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C752,1,1 (nothing) for 1 units
B $C753,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C754,1,1 (nothing) for 1 units
B $C755,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C756,1,1 (nothing) for 1 units
B $C757,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C758,1,1 (nothing) for 21 units
B $C759,1,1
B $C75A,1,1 BUSH for 1 units
B $C75B,1,1 (nothing) for 1 units
B $C75C,1,1 BUSH for 1 units
B $C75D,1,1 (nothing) for 1 units
B $C75E,1,1 BUSH for 1 units
B $C75F,1,1 (nothing) for 5 units
B $C760,1,1 BUSH for 1 units
B $C761,1,1 (nothing) for 3 units
B $C762,1,1 BUSH for 1 units
B $C763,1,1 (nothing) for 3 units
B $C764,1,1 TREE for 1 units
B $C765,1,1 (nothing) for 3 units
B $C766,1,1 TREE for 1 units
B $C767,1,1 (nothing) for 5 units
B $C768,1,1 TREE for 1 units
B $C769,1,1 (nothing) for 3 units
B $C76A,1,1 TREE for 1 units
B $C76B,1,1 (nothing) for 3 units
B $C76C,1,1 TREE for 1 units
B $C76D,1,1 (nothing) for 11 units
B $C76E,1,1 TREE for 1 units
B $C76F,1,1 (nothing) for 1 units
B $C770,1,1 TREE for 1 units
B $C771,1,1 (nothing) for 1 units
B $C772,1,1 TREE for 1 units
B $C773,1,1 (nothing) for 9 units
B $C774,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C775,1,1 (nothing) for 1 units
B $C776,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C777,1,1 (nothing) for 1 units
B $C778,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C779,1,1 (nothing) for 1 units
B $C77A,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C77B,1,1 (nothing) for 1 units
B $C77C,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C77D,1,1 (nothing) for 3 units
B $C77E,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C77F,1,1 (nothing) for 1 units
B $C780,1,1 TURN_SIGN_POINTING_RIGHT for 1 units
B $C781,1,1 (nothing) for 5 units
B $C782,1,1 STREET_LAMP for 1 units
B $C783,1,1 (nothing) for 1 units
B $C784,1,1 STREET_LAMP for 1 units
B $C785,1,1 (nothing) for 2 units
B $C786,1,1 <Esc> Loop
B $C787,1,1
W $C788,2,2 [$C714] Target
B $C78A,180,8*22,4
N $C83E LOD
B $C83E,1,1 Width (bytes)
B $C83F,1,1 Flags
B $C840,1,1 Height (pixels)
W $C841,2,2 [$C8BC] Bitmap address
W $C843,2,2 [$C8BC] Pre-shifted bitmap address
N $C845 LOD
B $C845,1,1 Width (bytes)
B $C846,1,1 Flags
B $C847,1,1 Height (pixels)
W $C848,2,2 [$C970] Bitmap address
W $C84A,2,2 [$C970] Pre-shifted bitmap address
N $C84C LOD
B $C84C,1,1 Width (bytes)
B $C84D,1,1 Flags
B $C84E,1,1 Height (pixels)
W $C84F,2,2 [$C9DE] Bitmap address
W $C851,2,2 [$C9DE] Pre-shifted bitmap address
N $C853 LOD
B $C853,1,1 Width (bytes)
B $C854,1,1 Flags
B $C855,1,1 Height (pixels)
W $C856,2,2 [$C9DE] Bitmap address
W $C858,2,2 [$C9DE] Pre-shifted bitmap address
N $C85A LOD
B $C85A,1,1 Width (bytes)
B $C85B,1,1 Flags
B $C85C,1,1 Height (pixels)
W $C85D,2,2 [$CD1A] Bitmap address
W $C85F,2,2 [$CD1A] Pre-shifted bitmap address
N $C861 LOD
B $C861,1,1 Width (bytes)
B $C862,1,1 Flags
B $C863,1,1 Height (pixels)
W $C864,2,2 [$CD1A] Bitmap address
W $C866,2,2 [$CD4A] Pre-shifted bitmap address
N $C868 LOD
B $C868,1,1 Width (bytes)
B $C869,1,1 Flags
B $C86A,1,1 Height (pixels)
W $C86B,2,2 [$CA0B] Bitmap address
W $C86D,2,2 [$CA0B] Pre-shifted bitmap address
N $C86F LOD
B $C86F,1,1 Width (bytes)
B $C870,1,1 Flags
B $C871,1,1 Height (pixels)
W $C872,2,2 [$CAF5] Bitmap address
W $C874,2,2 [$CAF5] Pre-shifted bitmap address
N $C876 LOD
B $C876,1,1 Width (bytes)
B $C877,1,1 Flags
B $C878,1,1 Height (pixels)
W $C879,2,2 [$CB86] Bitmap address
W $C87B,2,2 [$CB86] Pre-shifted bitmap address
N $C87D LOD
B $C87D,1,1 Width (bytes)
B $C87E,1,1 Flags
B $C87F,1,1 Height (pixels)
W $C880,2,2 [$CB86] Bitmap address
W $C882,2,2 [$CB86] Pre-shifted bitmap address
N $C884 LOD
B $C884,1,1 Width (bytes)
B $C885,1,1 Flags
B $C886,1,1 Height (pixels)
W $C887,2,2 [$CD7A] Bitmap address
W $C889,2,2 [$CDAA] Pre-shifted bitmap address
N $C88B LOD
B $C88B,1,1 Width (bytes)
B $C88C,1,1 Flags
B $C88D,1,1 Height (pixels)
W $C88E,2,2 [$CD7A] Bitmap address
W $C890,2,2 [$CDAA] Pre-shifted bitmap address
N $C892 LOD
B $C892,1,1 Width (bytes)
B $C893,1,1 Flags
B $C894,1,1 Height (pixels)
W $C895,2,2 [$CBC2] Bitmap address
W $C897,2,2 [$CBC2] Pre-shifted bitmap address
N $C899 LOD
B $C899,1,1 Width (bytes)
B $C89A,1,1 Flags
B $C89B,1,1 Height (pixels)
W $C89C,2,2 [$CC7C] Bitmap address
W $C89E,2,2 [$CC7C] Pre-shifted bitmap address
N $C8A0 LOD
B $C8A0,1,1 Width (bytes)
B $C8A1,1,1 Flags
B $C8A2,1,1 Height (pixels)
W $C8A3,2,2 [$CCEA] Bitmap address
W $C8A5,2,2 [$CCEA] Pre-shifted bitmap address
N $C8A7 LOD
B $C8A7,1,1 Width (bytes)
B $C8A8,1,1 Flags
B $C8A9,1,1 Height (pixels)
W $C8AA,2,2 [$CCEA] Bitmap address
W $C8AC,2,2 [$CCEA] Pre-shifted bitmap address
N $C8AE LOD
B $C8AE,1,1 Width (bytes)
B $C8AF,1,1 Flags
B $C8B0,1,1 Height (pixels)
W $C8B1,2,2 [$CDDA] Bitmap address
W $C8B3,2,2 [$CE10] Pre-shifted bitmap address
N $C8B5 LOD
B $C8B5,1,1 Width (bytes)
B $C8B6,1,1 Flags
B $C8B7,1,1 Height (pixels)
W $C8B8,2,2 [$CDDA] Bitmap address
W $C8BA,2,2 [$CE10] Pre-shifted bitmap address
B $C8BC,180,6 Bitmap data 6 bytes x 30
B $C970,110,5 Bitmap data 5 bytes x 22
B $C9DE,45,3 Bitmap data 3 bytes x 15
B $CA0B,234,6 Bitmap data 6 bytes x 39
B $CAF5,145,5 Bitmap data 5 bytes x 29
B $CB86,60,3 Bitmap data 3 bytes x 20
B $CBC2,186,6 Bitmap data 6 bytes x 31
B $CC7C,110,5 Bitmap data 5 bytes x 22
B $CCEA,48,3 Bitmap data 3 bytes x 16
B $CD1A,48,3 Bitmap data (masked) 3 bytes x 8
B $CD4A,48,3 Pre-shifted bitmap data (masked) 3 bytes x 8
B $CD7A,48,2 Bitmap data (masked) 2 bytes x 12
B $CDAA,48,2 Pre-shifted bitmap data (masked) 2 bytes x 12
B $CDDA,54,3 Bitmap data (masked) 3 bytes x 9
B $CE10,54,3 Pre-shifted bitmap data (masked) 3 bytes x 9
N $CE46 LOD
B $CE46,1,1 Width (bytes)
B $CE47,1,1 Flags
B $CE48,1,1 Height (pixels)
W $CE49,2,2 [$CE9A] Bitmap address
W $CE4B,2,2 [$CE9A] Pre-shifted bitmap address
N $CE4D LOD
B $CE4D,1,1 Width (bytes)
B $CE4E,1,1 Flags
B $CE4F,1,1 Height (pixels)
W $CE50,2,2 [$CEAE] Bitmap address
W $CE52,2,2 [$CEBE] Pre-shifted bitmap address
N $CE54 LOD
B $CE54,1,1 Width (bytes)
B $CE55,1,1 Flags
B $CE56,1,1 Height (pixels)
W $CE57,2,2 [$CECE] Bitmap address
W $CE59,2,2 [$CEDA] Pre-shifted bitmap address
N $CE5B LOD
B $CE5B,1,1 Width (bytes)
B $CE5C,1,1 Flags
B $CE5D,1,1 Height (pixels)
W $CE5E,2,2 [$CEE6] Bitmap address
W $CE60,2,2 [$CEEA] Pre-shifted bitmap address
N $CE62 LOD
B $CE62,1,1 Width (bytes)
B $CE63,1,1 Flags
B $CE64,1,1 Height (pixels)
W $CE65,2,2 [$CEE6] Bitmap address
W $CE67,2,2 [$CEEA] Pre-shifted bitmap address
N $CE69 LOD
B $CE69,1,1 Width (bytes)
B $CE6A,1,1 Flags
B $CE6B,1,1 Height (pixels)
W $CE6C,2,2 [$CEEE] Bitmap address
W $CE6E,2,2 [$CEF0] Pre-shifted bitmap address
N $CE70 LOD
B $CE70,1,1 Width (bytes)
B $CE71,1,1 Flags
B $CE72,1,1 Height (pixels)
W $CE73,2,2 [$CEF2] Bitmap address
W $CE75,2,2 [$CEF4] Pre-shifted bitmap address
N $CE77 LOD
B $CE77,1,1 Width (bytes)
B $CE78,1,1 Flags
B $CE79,1,1 Height (pixels)
W $CE7A,2,2 [$CEF2] Bitmap address
W $CE7C,2,2 [$CEF4] Pre-shifted bitmap address
N $CE7E LOD
B $CE7E,1,1 Width (bytes)
B $CE7F,1,1 Flags
B $CE80,1,1 Height (pixels)
W $CE81,2,2 [$CEF2] Bitmap address
W $CE83,2,2 [$CEF4] Pre-shifted bitmap address
N $CE85 LOD
B $CE85,1,1 Width (bytes)
B $CE86,1,1 Flags
B $CE87,1,1 Height (pixels)
W $CE88,2,2 [$CEF2] Bitmap address
W $CE8A,2,2 [$CEF4] Pre-shifted bitmap address
N $CE8C LOD
B $CE8C,1,1 Width (bytes)
B $CE8D,1,1 Flags
B $CE8E,1,1 Height (pixels)
W $CE8F,2,2 [$CEF2] Bitmap address
W $CE91,2,2 [$CEF4] Pre-shifted bitmap address
N $CE93 LOD
B $CE93,1,1 Width (bytes)
B $CE94,1,1 Flags
B $CE95,1,1 Height (pixels)
W $CE96,2,2 [$CEF2] Bitmap address
W $CE98,2,2 [$CEF4] Pre-shifted bitmap address
B $CE9A,20,2 Bitmap data (masked) 2 bytes x 5
B $CEAE,16,2 Bitmap data (masked) 2 bytes x 4
B $CEBE,16,2 Pre-shifted bitmap data (masked) 2 bytes x 4
B $CECE,12,2 Bitmap data (masked) 2 bytes x 3
B $CEDA,12,2 Pre-shifted bitmap data (masked) 2 bytes x 3
B $CEE6,4,1 Bitmap data (masked) 1 bytes x 2
B $CEEA,4,1 Pre-shifted bitmap data (masked) 1 bytes x 2
B $CEEE,2,1 Bitmap data (masked) 1 bytes x 1
B $CEF0,2,1 Pre-shifted bitmap data (masked) 1 bytes x 1
B $CEF2,2,1 Bitmap data (masked) 1 bytes x 1
B $CEF4,2,1 Pre-shifted bitmap data (masked) 1 bytes x 1
B $CEF6,652,8*81,4
N $D182 LOD
B $D182,1,1 Width (bytes)
B $D183,1,1 Flags
B $D184,1,1 Height (pixels)
W $D185,2,2 [$D1AC] Bitmap address
W $D187,2,2 [$D1AC] Pre-shifted bitmap address
N $D189 LOD
B $D189,1,1 Width (bytes)
B $D18A,1,1 Flags
B $D18B,1,1 Height (pixels)
W $D18C,2,2 [$D1CC] Bitmap address
W $D18E,2,2 [$D1CC] Pre-shifted bitmap address
N $D190 LOD
B $D190,1,1 Width (bytes)
B $D191,1,1 Flags
B $D192,1,1 Height (pixels)
W $D193,2,2 [$D1E2] Bitmap address
W $D195,2,2 [$D1E2] Pre-shifted bitmap address
N $D197 LOD
B $D197,1,1 Width (bytes)
B $D198,1,1 Flags
B $D199,1,1 Height (pixels)
W $D19A,2,2 [$D1E2] Bitmap address
W $D19C,2,2 [$D1E2] Pre-shifted bitmap address
N $D19E LOD
B $D19E,1,1 Width (bytes)
B $D19F,1,1 Flags
B $D1A0,1,1 Height (pixels)
W $D1A1,2,2 [$D1EB] Bitmap address
W $D1A3,2,2 [$D1EB] Pre-shifted bitmap address
N $D1A5 LOD
B $D1A5,1,1 Width (bytes)
B $D1A6,1,1 Flags
B $D1A7,1,1 Height (pixels)
W $D1A8,2,2 [$D1EB] Bitmap address
W $D1AA,2,2 [$D1EB] Pre-shifted bitmap address
B $D1AC,32,2 Bitmap data 2 bytes x 16
B $D1CC,22,2 Bitmap data 2 bytes x 11
B $D1E2,9,1 Bitmap data 1 bytes x 9
B $D1EB,7,1 Bitmap data 1 bytes x 7
N $D1F2 LOD
B $D1F2,1,1 Width (bytes)
B $D1F3,1,1 Flags
B $D1F4,1,1 Height (pixels)
W $D1F5,2,2 [$D21C] Bitmap address
W $D1F7,2,2 [$D21C] Pre-shifted bitmap address
N $D1F9 LOD
B $D1F9,1,1 Width (bytes)
B $D1FA,1,1 Flags
B $D1FB,1,1 Height (pixels)
W $D1FC,2,2 [$D21C] Bitmap address
W $D1FE,2,2 [$D21C] Pre-shifted bitmap address
N $D200 LOD
B $D200,1,1 Width (bytes)
B $D201,1,1 Flags
B $D202,1,1 Height (pixels)
W $D203,2,2 [$D260] Bitmap address
W $D205,2,2 [$D260] Pre-shifted bitmap address
N $D207 LOD
B $D207,1,1 Width (bytes)
B $D208,1,1 Flags
B $D209,1,1 Height (pixels)
W $D20A,2,2 [$D287] Bitmap address
W $D20C,2,2 [$D287] Pre-shifted bitmap address
N $D20E LOD
B $D20E,1,1 Width (bytes)
B $D20F,1,1 Flags
B $D210,1,1 Height (pixels)
W $D211,2,2 [$D287] Bitmap address
W $D213,2,2 [$D287] Pre-shifted bitmap address
N $D215 LOD
B $D215,1,1 Width (bytes)
B $D216,1,1 Flags
B $D217,1,1 Height (pixels)
W $D218,2,2 [$D299] Bitmap address
W $D21A,2,2 [$D2B5] Pre-shifted bitmap address
B $D21C,68,4 Bitmap data 4 bytes x 17
B $D260,39,3 Bitmap data 3 bytes x 13
B $D287,18,2 Bitmap data 2 bytes x 9
B $D299,28,2 Bitmap data (masked) 2 bytes x 7
B $D2B5,28,2 Pre-shifted bitmap data (masked) 2 bytes x 7
B $D2D1,3375,8*421,7
b $E000 [Stage 2] Horizon graphic
B $E000,240,8
b $E0F0 [Stage 2] Per-stage data
W $E0F0,2,2 [$C8EB] Address of perp's mugshot attributes
W $E0F2,2,2 [$D05A] Address of perp's mugshot bitmap
W $E0F4,2,2 [$9C38] Screen attributes used for the ground colour (a pair of matching bytes)
W $E0F6,2,2 [$C224] Address of table of LODs for tumbleweeds, barriers.
W $E0F8,2,2 [$C226] (points at a handler address)
W $E0FA,2,2 [$C223] Address of right hand graphics entry/entries (-7 bytes)
W $E0FC,2,2 [$C238] (points at a handler address)
W $E0FE,2,2 [$C257] (points at a handler address)
W $E100,2,2 [$C254] Address of left hand graphics entry/entries (-7 bytes)
W $E102,2,2 [$C269] (points at a handler address)
W $E104,2,2 [$C139] Address of Nancy's perp description
W $E106,2,2 [$C1DD] Address of arrest messages
W $E108,2,2 [$CD93] Helicopter related 1
W $E10A,2,2 [$CD9F] Helicopter related 2
w $E10C [Stage 2] Table of addresses of LODs
W $E10C,2,2 [$6400] Address of LOD of stone/dust?
W $E10E,2,2 [$6400] Address of LOD of stone/dust?
W $E110,2,2 [$C8FF] Address of LOD of car (the perp's car)
W $E112,2,2 [$C953] Address of LOD of car (a Lambo in S1)
W $E114,2,2 [$C929] Address of LOD of car (a truck in S1)
W $E116,2,2 [$C953] Address of LOD of car (a Lambo in S1)
W $E118,2,2 [$C8FF] Address of LOD of car (a generic car in S1)
b $E11A [Stage 2] Per-stage difficulty settings
B $E11A,1,1 How often cars spawn. Lower values spawn cars more often.
B $E11B,1,1 Smash related parameter
B $E11C,1,1 Smash related parameter
w $E11D [Stage 2] Per-stage setup data
W $E11D,2,2 road_pos
W $E11F,2,2 [$C28B] Address of start stretch, curvature
W $E121,2,2 [$C2AF] Address of start stretch, height
W $E123,2,2 [$C2E5] Address of start stretch, lanes
W $E125,2,2 [$C3C1] Address of start stretch, right-side objects
W $E127,2,2 [$C32F] Address of start stretch, left-side objects
W $E129,2,2 [$C2FB] Address of start stretch, hazards
w $E12B [Stage 2] Per-stage attract mode data
W $E12B,2,2 road_pos
W $E12D,2,2 [$C673] Address of loop section, curvature
W $E12F,2,2 [$C69B] Address of loop section, height
W $E131,2,2 [$C6DF] Address of loop section, lanes
W $E133,2,2 [$C79A] Address of loop section, right-side objects
W $E135,2,2 [$C712] Address of loop section, left-side objects
W $E137,2,2 [$C6EF] Address of loop section, hazards
b $E139 [Stage 2] Nancy's perp description
B $E139,1,1 Character identifier (0/1/2/3 = Pilot/Nancy/Raymond/Tony)
W $E13A,2,2 [$C145] Perp description pointer
W $E13C,2,2 [$C16D] Perp description pointer
W $E13E,2,2 [$C191] Perp description pointer
W $E140,2,2 [$C1B4] Perp description pointer
B $E142,1,1 terminator?
B $E143,2,2
T $E145,40,39:n1 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $E16D,36,35:n1 "EMERGENCY HERE. CARLOS, THE NEW YORK"
T $E191,35,34:n1 "ARMED ROBBER, HAS BEEN SPOTTED IN A"
T $E1B4,41,40:n1 "YELLOW SPORTS CAR ON THE FREEWAY... OVER."
b $E1DD [Stage 2] Arrest messages
B $E1DD,1,1 ?frame delay until first message
B $E1DE,1,1 ?frame delay until next message
B $E1DF,1,1 Flags
B $E1E0,1,1 Attribute
W $E1E1,2,2 Back buffer address
W $E1E3,2,2 Attribute address
T $E1E5,27,26:n1 "OK! YOU ARE UNDER ARREST ON"
B $E200,1,1 ?frame delay until next message
B $E201,1,1 Flags
B $E202,1,1 Attribute
W $E203,2,2 Back buffer address
W $E205,2,2 Attribute address
T $E207,27,26:n1 "SUSPICION OF ARMED ROBBERY."
B $E222,1,1 ?frame delay until next message
B $E223,1,1 Stop
b $E224 [Stage 2] Hittable hazards
B $E224,1,1 ?id
W $E225,2,2 [$DA0F] Address of LODs
B $E227,1,1 ?id
W $E228,2,2 [$DA0F] Address of LODs
b $E22A [Stage 2] Graphic definition
N $E22A Definition
B $E22A,1,1 Hit coord max/min (R/L)
B $E22B,1,1 Hit coord min/max (R/L)
B $E22C,1,1 ?how far to push hero car away if hit
W $E22D,2,2 Argument for routine passed in #REGde
W $E22F,2,2 Address of routine draw_tunnel_light_right
N $E231 Definition
B $E231,1,1 Hit coord max/min (R/L)
B $E232,1,1 Hit coord min/max (R/L)
B $E233,1,1 ?how far to push hero car away if hit
W $E234,2,2 Argument for routine passed in #REGde
W $E236,2,2 Address of routine TBD
N $E238 Definition
B $E238,1,1 Hit coord max/min (R/L)
B $E239,1,1 Hit coord min/max (R/L)
B $E23A,1,1 ?how far to push hero car away if hit
W $E23B,2,2 Argument for routine passed in #REGde
W $E23D,2,2 Address of routine draw_stretchy_object_right
N $E23F Definition
B $E23F,1,1 Hit coord max/min (R/L)
B $E240,1,1 Hit coord min/max (R/L)
B $E241,1,1 ?how far to push hero car away if hit
W $E242,2,2 Argument for routine passed in #REGde
W $E244,2,2 Address of routine draw_stretchy_object_right
N $E246 Definition
B $E246,1,1 Hit coord max/min (R/L)
B $E247,1,1 Hit coord min/max (R/L)
B $E248,1,1 ?how far to push hero car away if hit
W $E249,2,2 Argument for routine passed in #REGde
W $E24B,2,2 Address of routine draw_stretchy_object_right
N $E24D Definition
B $E24D,1,1 Hit coord max/min (R/L)
B $E24E,1,1 Hit coord min/max (R/L)
B $E24F,1,1 ?how far to push hero car away if hit
W $E250,2,2 Argument for routine passed in #REGde
W $E252,2,2 Address of routine draw_stretchy_object_right
N $E254 Definition
B $E254,1,1 Hit coord max/min (R/L)
B $E255,1,1 Hit coord min/max (R/L)
B $E256,1,1 ?how far to push hero car away if hit
W $E257,2,2 Argument for routine passed in #REGde
W $E259,2,2 Address of routine draw_stretchy_object_right
b $E25B [Stage 2] Graphic definition
N $E25B Definition
B $E25B,1,1 Hit coord max/min (R/L)
B $E25C,1,1 Hit coord min/max (R/L)
B $E25D,1,1 ?how far to push hero car away if hit
W $E25E,2,2 Argument for routine passed in #REGde
W $E260,2,2 Address of routine draw_tunnel_light_left
N $E262 Definition
B $E262,1,1 Hit coord max/min (R/L)
B $E263,1,1 Hit coord min/max (R/L)
B $E264,1,1 ?how far to push hero car away if hit
W $E265,2,2 Argument for routine passed in #REGde
W $E267,2,2 Address of routine TBD
N $E269 Definition
B $E269,1,1 Hit coord max/min (R/L)
B $E26A,1,1 Hit coord min/max (R/L)
B $E26B,1,1 ?how far to push hero car away if hit
W $E26C,2,2 Argument for routine passed in #REGde
W $E26E,2,2 Address of routine draw_stretchy_object_left
N $E270 Definition
B $E270,1,1 Hit coord max/min (R/L)
B $E271,1,1 Hit coord min/max (R/L)
B $E272,1,1 ?how far to push hero car away if hit
W $E273,2,2 Argument for routine passed in #REGde
W $E275,2,2 Address of routine draw_stretchy_object_left
N $E277 Definition
B $E277,1,1 Hit coord max/min (R/L)
B $E278,1,1 Hit coord min/max (R/L)
B $E279,1,1 ?how far to push hero car away if hit
W $E27A,2,2 Argument for routine passed in #REGde
W $E27C,2,2 Address of routine draw_stretchy_object_left
N $E27E Definition
B $E27E,1,1 Hit coord max/min (R/L)
B $E27F,1,1 Hit coord min/max (R/L)
B $E280,1,1 ?how far to push hero car away if hit
W $E281,2,2 Argument for routine passed in #REGde
W $E283,2,2 Address of routine draw_stretchy_object_left
N $E285 Definition
B $E285,1,1 Hit coord max/min (R/L)
B $E286,1,1 Hit coord min/max (R/L)
B $E287,1,1 ?how far to push hero car away if hit
W $E288,2,2 Argument for routine passed in #REGde
W $E28A,2,2 Address of routine draw_stretchy_object_left
b $E28C [Stage 2] Map curvature data
B $E28C,1,1 Curve Straight for 73 units
B $E28D,4,4
B $E291,1,1 Curve Right for 24 units
B $E292,1,1
B $E293,1,1 Curve Right Hard for 27 units
B $E294,1,1
B $E295,1,1 Curve Right for 20 units
B $E296,1,1
B $E297,1,1 Curve Straight for 59 units
B $E298,3,3
B $E29B,1,1 Curve Left for 12 units
B $E29C,1,1 Curve Left Hard for 59 units
B $E29D,3,3
B $E2A0,1,1 Curve Left for 14 units
B $E2A1,1,1 Curve Straight for 59 units
B $E2A2,3,3
B $E2A5,1,1 Curve Right for 16 units
B $E2A6,1,1
B $E2A7,1,1 Curve Straight for 37 units
B $E2A8,2,2
B $E2AA,1,1 <Esc> Split
B $E2AB,1,1
W $E2AC,2,2 [$C439] Left target
W $E2AE,2,2 [$C53C] Right target
b $E2B0 [Stage 2] Map height data
B $E2B0,1,1 Level Road for 2 units
B $E2B1,1,1 Going Up 1 for 7 units
B $E2B2,1,1 Going Up 3 for 12 units
B $E2B3,1,1 Going Up 1 for 3 units
B $E2B4,1,1 Level Road for 3 units
B $E2B5,1,1 Going Down 1 for 3 units
B $E2B6,1,1 Going Down 3 for 20 units
B $E2B7,1,1
B $E2B8,1,1 Going Down 1 for 6 units
B $E2B9,1,1 Level Road for 55 units
B $E2BA,3,3
B $E2BD,1,1 Going Up 1 for 4 units
B $E2BE,1,1 Going Up 3 for 4 units
B $E2BF,1,1 Going Up 5 for 12 units
B $E2C0,1,1 Going Up 3 for 4 units
B $E2C1,1,1 Going Up 1 for 3 units
B $E2C2,1,1 Level Road for 44 units
B $E2C3,2,2
B $E2C5,1,1 Going Down 1 for 2 units
B $E2C6,1,1 Going Down 3 for 8 units
B $E2C7,1,1 Going Down 5 for 7 units
B $E2C8,1,1 Going Down 3 for 2 units
B $E2C9,1,1 Going Down 1 for 3 units
B $E2CA,1,1 Level Road for 68 units
B $E2CB,4,4
B $E2CF,1,1 Going Down 1 for 3 units
B $E2D0,1,1 Going Down 3 for 6 units
B $E2D1,1,1 Going Down 5 for 16 units
B $E2D2,1,1
B $E2D3,1,1 Going Down 3 for 3 units
B $E2D4,1,1 Level Road for 3 units
B $E2D5,1,1 Going Up 3 for 5 units
B $E2D6,1,1 Going Up 5 for 4 units
B $E2D7,1,1 Going Up 7 for 14 units
B $E2D8,1,1 Going Up 5 for 4 units
B $E2D9,1,1 Going Down 5 for 4 units
B $E2DA,1,1 Going Down 3 for 14 units
B $E2DB,1,1 Going Down 1 for 3 units
B $E2DC,1,1 Level Road for 49 units
B $E2DD,3,3
B $E2E0,1,1 <Esc> Split
B $E2E1,1,1
W $E2E2,2,2 [$C44F] Left target
W $E2E4,2,2 [$C54F] Right target
b $E2E6 [Stage 2] Map lanes data
B $E2E6,1,1 4 Lanes              [||||] {00} for 2 units
B $E2E7,1,1
B $E2E8,1,1 4-3 Narrowing R      [/|||] {8E} for 30 units
B $E2E9,1,1
B $E2EA,1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $E2EB,1,1
B $E2EC,1,1 4 Lanes              [||||] {00} for 10 units
B $E2ED,1,1
B $E2EE,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E2EF,1,1
B $E2F0,1,1 3 Lanes L            [|||]  {81} for 130 units
B $E2F1,1,1
B $E2F2,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E2F3,1,1
B $E2F4,1,1 4 Lanes              [||||] {00} for 222 units
B $E2F5,1,1
B $E2F6,1,1 <Esc> Split
B $E2F7,1,1
W $E2F8,2,2 [$C474] Left target
W $E2FA,2,2 [$C561] Right target
b $E2FC [Stage 2] Map hazards data
B $E2FC,1,1 Wait for 38 units
B $E2FD,1,1 Unknown command 4
B $E2FE,1,1
B $E2FF,1,1 Wait for 2 units
B $E300,1,1 Stop Spawning Barriers 3?
B $E301,1,1
B $E302,1,1 Wait for 10 units
B $E303,1,1 Unknown command 4
B $E304,1,1
B $E305,1,1 Wait for 2 units
B $E306,1,1 Stop Spawning Barriers 3?
B $E307,1,1
B $E308,1,1 Wait for 45 units
B $E309,1,1 Unknown command 5
B $E30A,1,1
B $E30B,1,1 Wait for 2 units
B $E30C,1,1 Stop Spawning Barriers 3?
B $E30D,1,1
B $E30E,1,1 Wait for 16 units
B $E30F,1,1 Unknown command 5
B $E310,1,1
B $E311,1,1 Wait for 2 units
B $E312,1,1 Stop Spawning Barriers 3?
B $E313,1,1
B $E314,1,1 Wait for 8 units
B $E315,1,1 Unknown command 11
B $E316,1,1
B $E317,1,1 Wait for 19 units
B $E318,1,1 Unknown command 4
B $E319,1,1
B $E31A,1,1 Wait for 2 units
B $E31B,1,1 Stop Spawning Barriers 3?
B $E31C,1,1
B $E31D,1,1 Wait for 29 units
B $E31E,1,1 Unknown command 4
B $E31F,1,1
B $E320,1,1 Wait for 2 units
B $E321,1,1 Stop Spawning Barriers 3?
B $E322,1,1
B $E323,1,1 Wait for 13 units
B $E324,1,1 Disable Car Spawning
B $E325,1,1
B $E326,1,1 Wait for 9 units
B $E327,1,1 Set Floating Arrow to Left
B $E328,1,1
B $E329,1,1 Wait for 1 units
B $E32A,1,1 <Esc> Split
B $E32B,1,1
W $E32C,2,2 [$C488] Left target
W $E32E,2,2 [$C57B] Right target
b $E330 [Stage 2] Map left object data
B $E330,1,1 LEAVES for 1 units
B $E331,1,1 (nothing) for 1 units
B $E332,1,1 LEAVES for 1 units
B $E333,1,1 (nothing) for 1 units
B $E334,1,1 LEAVES for 1 units
B $E335,1,1 (nothing) for 1 units
B $E336,1,1 LEAVES for 1 units
B $E337,1,1 (nothing) for 3 units
B $E338,1,1 LEAVES for 1 units
B $E339,1,1 (nothing) for 3 units
B $E33A,1,1 LEAVES for 1 units
B $E33B,1,1 (nothing) for 3 units
B $E33C,1,1 LEAVES for 1 units
B $E33D,1,1 (nothing) for 1 units
B $E33E,1,1 LEAVES for 1 units
B $E33F,1,1 (nothing) for 1 units
B $E340,1,1 LEAVES for 1 units
B $E341,1,1 (nothing) for 13 units
B $E342,1,1 PALM_TREE for 1 units
B $E343,1,1 (nothing) for 1 units
B $E344,1,1 PALM_TREE for 1 units
B $E345,1,1 (nothing) for 1 units
B $E346,1,1 PALM_TREE for 1 units
B $E347,1,1 (nothing) for 1 units
B $E348,1,1 PALM_TREE for 1 units
B $E349,1,1 (nothing) for 9 units
B $E34A,1,1 PALM_TREE for 1 units
B $E34B,1,1 (nothing) for 1 units
B $E34C,1,1 PALM_TREE for 1 units
B $E34D,1,1 (nothing) for 1 units
B $E34E,1,1 PALM_TREE for 1 units
B $E34F,1,1 (nothing) for 1 units
B $E350,1,1 PALM_TREE for 1 units
B $E351,1,1 (nothing) for 1 units
B $E352,1,1 PALM_TREE for 1 units
B $E353,1,1 (nothing) for 3 units
B $E354,1,1 PALM_TREE for 1 units
B $E355,1,1 (nothing) for 1 units
B $E356,1,1 PALM_TREE for 1 units
B $E357,1,1 (nothing) for 1 units
B $E358,1,1 PALM_TREE for 1 units
B $E359,1,1 (nothing) for 3 units
B $E35A,1,1 PALM_TREE for 1 units
B $E35B,1,1 (nothing) for 1 units
B $E35C,1,1 PALM_TREE for 1 units
B $E35D,1,1 (nothing) for 1 units
B $E35E,1,1 PALM_TREE for 1 units
B $E35F,1,1 (nothing) for 7 units
B $E360,1,1 PALM_TREE for 1 units
B $E361,1,1 (nothing) for 1 units
B $E362,1,1 PALM_TREE for 1 units
B $E363,1,1 (nothing) for 1 units
B $E364,1,1 PALM_TREE for 1 units
B $E365,1,1 (nothing) for 1 units
B $E366,1,1 PALM_TREE for 1 units
B $E367,1,1 (nothing) for 3 units
B $E368,1,1 PALM_TREE for 1 units
B $E369,1,1 (nothing) for 1 units
B $E36A,1,1 PALM_TREE for 1 units
B $E36B,1,1 (nothing) for 3 units
B $E36C,1,1 PALM_TREE for 1 units
B $E36D,1,1 (nothing) for 1 units
B $E36E,1,1 PALM_TREE for 1 units
B $E36F,1,1 (nothing) for 3 units
B $E370,1,1 PALM_TREE for 1 units
B $E371,1,1 (nothing) for 3 units
B $E372,1,1 PALM_TREE for 1 units
B $E373,1,1 (nothing) for 1 units
B $E374,1,1 PALM_TREE for 1 units
B $E375,1,1 (nothing) for 1 units
B $E376,1,1 LEAVES for 1 units
B $E377,1,1 (nothing) for 1 units
B $E378,1,1 LEAVES for 1 units
B $E379,1,1 (nothing) for 1 units
B $E37A,1,1 LEAVES for 1 units
B $E37B,1,1 (nothing) for 1 units
B $E37C,1,1 LEAVES for 1 units
B $E37D,1,1 (nothing) for 3 units
B $E37E,1,1 PALM_TREE for 1 units
B $E37F,1,1 (nothing) for 1 units
B $E380,1,1 LEAVES for 1 units
B $E381,1,1 (nothing) for 1 units
B $E382,1,1 PALM_TREE for 1 units
B $E383,1,1 (nothing) for 1 units
B $E384,1,1 LEAVES for 1 units
B $E385,1,1 (nothing) for 1 units
B $E386,1,1 LEAVES for 1 units
B $E387,1,1 (nothing) for 1 units
B $E388,1,1 PALM_TREE for 1 units
B $E389,1,1 (nothing) for 7 units
B $E38A,1,1 DOUBLE_STREET_LAMP for 1 units
B $E38B,1,1 (nothing) for 1 units
B $E38C,1,1 DOUBLE_STREET_LAMP for 1 units
B $E38D,1,1 (nothing) for 1 units
B $E38E,1,1 DOUBLE_STREET_LAMP for 1 units
B $E38F,1,1 (nothing) for 1 units
B $E390,1,1 DOUBLE_STREET_LAMP for 1 units
B $E391,1,1 (nothing) for 1 units
B $E392,1,1 DOUBLE_STREET_LAMP for 1 units
B $E393,1,1 (nothing) for 1 units
B $E394,1,1 DOUBLE_STREET_LAMP for 1 units
B $E395,1,1 (nothing) for 1 units
B $E396,1,1 DOUBLE_STREET_LAMP for 1 units
B $E397,1,1 (nothing) for 1 units
B $E398,1,1 DOUBLE_STREET_LAMP for 1 units
B $E399,1,1 (nothing) for 1 units
B $E39A,1,1 DOUBLE_STREET_LAMP for 1 units
B $E39B,1,1 (nothing) for 1 units
B $E39C,1,1 DOUBLE_STREET_LAMP for 1 units
B $E39D,1,1 (nothing) for 1 units
B $E39E,1,1 DOUBLE_STREET_LAMP for 1 units
B $E39F,1,1 (nothing) for 1 units
B $E3A0,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3A1,1,1 (nothing) for 1 units
B $E3A2,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3A3,1,1 (nothing) for 1 units
B $E3A4,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3A5,1,1 (nothing) for 1 units
B $E3A6,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3A7,1,1 (nothing) for 1 units
B $E3A8,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3A9,1,1 (nothing) for 1 units
B $E3AA,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3AB,1,1 (nothing) for 1 units
B $E3AC,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3AD,1,1 (nothing) for 1 units
B $E3AE,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3AF,1,1 (nothing) for 5 units
B $E3B0,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3B1,1,1 (nothing) for 1 units
B $E3B2,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3B3,1,1 (nothing) for 1 units
B $E3B4,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3B5,1,1 (nothing) for 3 units
B $E3B6,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3B7,1,1 (nothing) for 1 units
B $E3B8,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3B9,1,1 (nothing) for 3 units
B $E3BA,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3BB,1,1 (nothing) for 1 units
B $E3BC,1,1 <Esc> Split
B $E3BD,1,1
W $E3BE,2,2 [$C4A8] Left target
W $E3C0,2,2 [$C592] Right target
b $E3C2 [Stage 2] Map right object data
B $E3C2,1,1 (nothing) for 2 units
B $E3C3,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3C4,1,1 (nothing) for 1 units
B $E3C5,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3C6,1,1 (nothing) for 1 units
B $E3C7,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3C8,1,1 (nothing) for 1 units
B $E3C9,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3CA,1,1 (nothing) for 1 units
B $E3CB,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3CC,1,1 (nothing) for 7 units
B $E3CD,1,1 PALM_TREE for 1 units
B $E3CE,1,1 (nothing) for 1 units
B $E3CF,1,1 PALM_TREE for 1 units
B $E3D0,1,1 (nothing) for 1 units
B $E3D1,1,1 PALM_TREE for 1 units
B $E3D2,1,1 (nothing) for 3 units
B $E3D3,1,1 PALM_TREE for 1 units
B $E3D4,1,1 (nothing) for 1 units
B $E3D5,1,1 PALM_TREE for 1 units
B $E3D6,1,1 (nothing) for 3 units
B $E3D7,1,1 PALM_TREE for 1 units
B $E3D8,1,1 (nothing) for 3 units
B $E3D9,1,1 PALM_TREE for 1 units
B $E3DA,1,1 (nothing) for 1 units
B $E3DB,1,1 PALM_TREE for 1 units
B $E3DC,1,1 (nothing) for 3 units
B $E3DD,1,1 LEAVES for 1 units
B $E3DE,1,1 (nothing) for 1 units
B $E3DF,1,1 LEAVES for 1 units
B $E3E0,1,1 (nothing) for 1 units
B $E3E1,1,1 LEAVES for 1 units
B $E3E2,1,1 (nothing) for 1 units
B $E3E3,1,1 LEAVES for 1 units
B $E3E4,1,1 (nothing) for 3 units
B $E3E5,1,1 LEAVES for 1 units
B $E3E6,1,1 (nothing) for 3 units
B $E3E7,1,1 LEAVES for 1 units
B $E3E8,1,1 (nothing) for 1 units
B $E3E9,1,1 LEAVES for 1 units
B $E3EA,1,1 (nothing) for 3 units
B $E3EB,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3EC,1,1 (nothing) for 1 units
B $E3ED,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3EE,1,1 (nothing) for 1 units
B $E3EF,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3F0,1,1 (nothing) for 3 units
B $E3F1,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3F2,1,1 (nothing) for 3 units
B $E3F3,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3F4,1,1 (nothing) for 3 units
B $E3F5,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3F6,1,1 (nothing) for 1 units
B $E3F7,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3F8,1,1 (nothing) for 1 units
B $E3F9,1,1 DOUBLE_STREET_LAMP for 1 units
B $E3FA,1,1 (nothing) for 21 units
B $E3FB,1,1
B $E3FC,1,1 PALM_TREE for 1 units
B $E3FD,1,1 (nothing) for 1 units
B $E3FE,1,1 PALM_TREE for 1 units
B $E3FF,1,1 (nothing) for 1 units
B $E400,1,1 PALM_TREE for 1 units
B $E401,1,1 (nothing) for 3 units
B $E402,1,1 PALM_TREE for 1 units
B $E403,1,1 (nothing) for 1 units
B $E404,1,1 PALM_TREE for 1 units
B $E405,1,1 (nothing) for 13 units
B $E406,1,1 PALM_TREE for 1 units
B $E407,1,1 (nothing) for 1 units
B $E408,1,1 PALM_TREE for 1 units
B $E409,1,1 (nothing) for 1 units
B $E40A,1,1 PALM_TREE for 1 units
B $E40B,1,1 (nothing) for 3 units
B $E40C,1,1 PALM_TREE for 1 units
B $E40D,1,1 (nothing) for 3 units
B $E40E,1,1 PALM_TREE for 1 units
B $E40F,1,1 (nothing) for 1 units
B $E410,1,1 DOUBLE_STREET_LAMP for 1 units
B $E411,1,1 (nothing) for 1 units
B $E412,1,1 DOUBLE_STREET_LAMP for 1 units
B $E413,1,1 (nothing) for 1 units
B $E414,1,1 DOUBLE_STREET_LAMP for 1 units
B $E415,1,1 (nothing) for 1 units
B $E416,1,1 DOUBLE_STREET_LAMP for 1 units
B $E417,1,1 (nothing) for 1 units
B $E418,1,1 DOUBLE_STREET_LAMP for 1 units
B $E419,1,1 (nothing) for 1 units
B $E41A,1,1 DOUBLE_STREET_LAMP for 1 units
B $E41B,1,1 (nothing) for 1 units
B $E41C,1,1 DOUBLE_STREET_LAMP for 1 units
B $E41D,1,1 (nothing) for 1 units
B $E41E,1,1 DOUBLE_STREET_LAMP for 1 units
B $E41F,1,1 (nothing) for 1 units
B $E420,1,1 DOUBLE_STREET_LAMP for 1 units
B $E421,1,1 (nothing) for 1 units
B $E422,1,1 DOUBLE_STREET_LAMP for 1 units
B $E423,1,1 (nothing) for 1 units
B $E424,1,1 DOUBLE_STREET_LAMP for 1 units
B $E425,1,1 (nothing) for 1 units
B $E426,1,1 DOUBLE_STREET_LAMP for 1 units
B $E427,1,1 (nothing) for 1 units
B $E428,1,1 DOUBLE_STREET_LAMP for 1 units
B $E429,1,1 (nothing) for 1 units
B $E42A,1,1 DOUBLE_STREET_LAMP for 1 units
B $E42B,1,1 (nothing) for 1 units
B $E42C,1,1 DOUBLE_STREET_LAMP for 1 units
B $E42D,1,1 (nothing) for 5 units
B $E42E,1,1 DOUBLE_STREET_LAMP for 1 units
B $E42F,1,1 (nothing) for 1 units
B $E430,1,1 DOUBLE_STREET_LAMP for 1 units
B $E431,1,1 (nothing) for 21 units
B $E432,1,1
B $E433,1,1 <Esc> Split
B $E434,1,1
W $E435,2,2 [$C4F9] Left target
W $E437,2,2 [$C5D0] Right target
b $E439 [Stage 2] Map curvature data
B $E439,1,1 Curve Straight for 39 units
B $E43A,2,2
B $E43C,1,1 Curve Right for 33 units
B $E43D,2,2
B $E43F,1,1 Curve Straight for 29 units
B $E440,1,1
B $E441,1,1 Curve Right for 13 units
B $E442,1,1 Curve Right Hard for 8 units
B $E443,1,1 Curve Right for 8 units
B $E444,1,1 Curve Straight for 21 units
B $E445,1,1
B $E446,1,1 Curve Left for 14 units
B $E447,1,1 Curve Straight for 15 units
B $E448,1,1 Curve Right for 21 units
B $E449,1,1
B $E44A,1,1 Curve Straight for 9 units
B $E44B,1,1 <Esc> Jump
B $E44C,1,1
W $E44D,2,2 [$C60B] Target
b $E44F [Stage 2] Map height data
B $E44F,1,1 Going Up 3 for 8 units
B $E450,1,1 Going Up 5 for 6 units
B $E451,1,1 Going Up 7 for 7 units
B $E452,1,1 Going Up 5 for 2 units
B $E453,1,1 Going Up 3 for 3 units
B $E454,1,1 Going Up 1 for 1 units
B $E455,1,1 Going Down 1 for 2 units
B $E456,1,1 Going Down 3 for 2 units
B $E457,1,1 Going Down 5 for 35 units
B $E458,2,2
B $E45A,1,1 Going Down 3 for 2 units
B $E45B,1,1 Going Down 1 for 2 units
B $E45C,1,1 Going Up 1 for 2 units
B $E45D,1,1 Going Up 3 for 2 units
B $E45E,1,1 Going Up 5 for 48 units
B $E45F,3,3
B $E462,1,1 Going Up 3 for 35 units
B $E463,2,2
B $E465,1,1 Going Up 1 for 1 units
B $E466,1,1 Level Road for 1 units
B $E467,1,1 Going Down 1 for 1 units
B $E468,1,1 Going Down 3 for 1 units
B $E469,1,1 Going Down 5 for 16 units
B $E46A,1,1
B $E46B,1,1 Going Down 7 for 11 units
B $E46C,1,1 Going Down 5 for 20 units
B $E46D,1,1
B $E46E,1,1 Going Down 3 for 1 units
B $E46F,1,1 Going Down 1 for 1 units
B $E470,1,1 <Esc> Jump
B $E471,1,1
W $E472,2,2 [$C61E] Target
b $E474 [Stage 2] Map lanes data
B $E474,1,1 4 Lanes              [||||] {00} for 78 units
B $E475,1,1
B $E476,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E477,1,1
B $E478,1,1 3 Lanes L            [|||]  {81} for 10 units
B $E479,1,1
B $E47A,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E47B,1,1
B $E47C,1,1 4 Lanes              [||||] {00} for 42 units
B $E47D,1,1
B $E47E,1,1 4-3 Narrowing R      [/|||] {8E} for 4 units
B $E47F,1,1
B $E480,1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $E481,1,1
B $E482,1,1 4 Lanes              [||||] {00} for 70 units
B $E483,1,1
B $E484,1,1 <Esc> Jump
B $E485,1,1
W $E486,2,2 [$C62F] Target
b $E488 [Stage 2] Map hazards data
B $E488,1,1 Wait for 10 units
B $E489,1,1 Enable Car Spawning
B $E48A,1,1
B $E48B,1,1 Wait for 11 units
B $E48C,1,1 Start Spawning Two Barriers
B $E48D,1,1
B $E48E,1,1 Wait for 1 units
B $E48F,1,1 Stop Spawning Barriers 3?
B $E490,1,1
B $E491,1,1 Wait for 6 units
B $E492,1,1 Start Spawning Two Barriers
B $E493,1,1
B $E494,1,1 Wait for 1 units
B $E495,1,1 Stop Spawning Barriers 3?
B $E496,1,1
B $E497,1,1 Wait for 9 units
B $E498,1,1 Unknown command 5
B $E499,1,1
B $E49A,1,1 Wait for 2 units
B $E49B,1,1 Stop Spawning Barriers 3?
B $E49C,1,1
B $E49D,1,1 Wait for 50 units
B $E49E,1,1 Unknown command F
B $E49F,1,1
B $E4A0,1,1 Wait for 1 units
B $E4A1,1,1 Stop Spawning Barriers 3?
B $E4A2,1,1
B $E4A3,1,1 Wait for 14 units
B $E4A4,1,1 <Esc> Jump
B $E4A5,1,1
W $E4A6,2,2 [$C643] Target
b $E4A8 [Stage 2] Map left object data
B $E4A8,1,1 PALM_TREE for 1 units
B $E4A9,1,1 (nothing) for 1 units
B $E4AA,1,1 PALM_TREE for 1 units
B $E4AB,1,1 (nothing) for 1 units
B $E4AC,1,1 PALM_TREE for 1 units
B $E4AD,1,1 (nothing) for 1 units
B $E4AE,1,1 PALM_TREE for 1 units
B $E4AF,1,1 (nothing) for 1 units
B $E4B0,1,1 PALM_TREE for 1 units
B $E4B1,1,1 (nothing) for 1 units
B $E4B2,1,1 PALM_TREE for 1 units
B $E4B3,1,1 (nothing) for 1 units
B $E4B4,1,1 PALM_TREE for 1 units
B $E4B5,1,1 (nothing) for 1 units
B $E4B6,1,1 PALM_TREE for 1 units
B $E4B7,1,1 (nothing) for 1 units
B $E4B8,1,1 PALM_TREE for 1 units
B $E4B9,1,1 (nothing) for 5 units
B $E4BA,1,1 PALM_TREE for 1 units
B $E4BB,1,1 (nothing) for 1 units
B $E4BC,1,1 PALM_TREE for 1 units
B $E4BD,1,1 (nothing) for 3 units
B $E4BE,1,1 PALM_TREE for 1 units
B $E4BF,1,1 (nothing) for 1 units
B $E4C0,1,1 PALM_TREE for 1 units
B $E4C1,1,1 (nothing) for 1 units
B $E4C2,1,1 PALM_TREE for 1 units
B $E4C3,1,1 (nothing) for 1 units
B $E4C4,1,1 PALM_TREE for 1 units
B $E4C5,1,1 (nothing) for 3 units
B $E4C6,1,1 PALM_TREE for 1 units
B $E4C7,1,1 (nothing) for 1 units
B $E4C8,1,1 PALM_TREE for 1 units
B $E4C9,1,1 (nothing) for 1 units
B $E4CA,1,1 PALM_TREE for 1 units
B $E4CB,1,1 (nothing) for 1 units
B $E4CC,1,1 PALM_TREE for 1 units
B $E4CD,1,1 (nothing) for 3 units
B $E4CE,1,1 PALM_TREE for 1 units
B $E4CF,1,1 (nothing) for 3 units
B $E4D0,1,1 PALM_TREE for 1 units
B $E4D1,1,1 (nothing) for 3 units
B $E4D2,1,1 PALM_TREE for 1 units
B $E4D3,1,1 (nothing) for 3 units
B $E4D4,1,1 PALM_TREE for 1 units
B $E4D5,1,1 (nothing) for 3 units
B $E4D6,1,1 LEAVES for 1 units
B $E4D7,1,1 (nothing) for 1 units
B $E4D8,1,1 LEAVES for 1 units
B $E4D9,1,1 (nothing) for 1 units
B $E4DA,1,1 LEAVES for 1 units
B $E4DB,1,1 (nothing) for 3 units
B $E4DC,1,1 LEAVES for 1 units
B $E4DD,1,1 (nothing) for 3 units
B $E4DE,1,1 LEAVES for 1 units
B $E4DF,1,1 (nothing) for 1 units
B $E4E0,1,1 LEAVES for 1 units
B $E4E1,1,1 (nothing) for 1 units
B $E4E2,1,1 LEAVES for 1 units
B $E4E3,1,1 (nothing) for 3 units
B $E4E4,1,1 LEAVES for 1 units
B $E4E5,1,1 (nothing) for 1 units
B $E4E6,1,1 LEAVES for 1 units
B $E4E7,1,1 (nothing) for 1 units
B $E4E8,1,1 LEAVES for 1 units
B $E4E9,1,1 (nothing) for 1 units
B $E4EA,1,1 PALM_TREE for 1 units
B $E4EB,1,1 (nothing) for 1 units
B $E4EC,1,1 PALM_TREE for 1 units
B $E4ED,1,1 (nothing) for 1 units
B $E4EE,1,1 LEAVES for 1 units
B $E4EF,1,1 (nothing) for 1 units
B $E4F0,1,1 PALM_TREE for 1 units
B $E4F1,1,1 (nothing) for 3 units
B $E4F2,1,1 LEAVES for 1 units
B $E4F3,1,1 (nothing) for 3 units
B $E4F4,1,1 PALM_TREE for 1 units
B $E4F5,1,1 <Esc> Jump
B $E4F6,1,1
W $E4F7,2,2 [$C648] Target
b $E4F9 [Stage 2] Map right object data
B $E4F9,1,1 (nothing) for 16 units
B $E4FA,1,1
B $E4FB,1,1 DOUBLE_STREET_LAMP for 1 units
B $E4FC,1,1 (nothing) for 3 units
B $E4FD,1,1 DOUBLE_STREET_LAMP for 1 units
B $E4FE,1,1 (nothing) for 3 units
B $E4FF,1,1 DOUBLE_STREET_LAMP for 1 units
B $E500,1,1 (nothing) for 3 units
B $E501,1,1 DOUBLE_STREET_LAMP for 1 units
B $E502,1,1 (nothing) for 1 units
B $E503,1,1 DOUBLE_STREET_LAMP for 1 units
B $E504,1,1 (nothing) for 1 units
B $E505,1,1 DOUBLE_STREET_LAMP for 1 units
B $E506,1,1 (nothing) for 3 units
B $E507,1,1 DOUBLE_STREET_LAMP for 1 units
B $E508,1,1 (nothing) for 3 units
B $E509,1,1 DOUBLE_STREET_LAMP for 1 units
B $E50A,1,1 (nothing) for 3 units
B $E50B,1,1 DOUBLE_STREET_LAMP for 1 units
B $E50C,1,1 (nothing) for 3 units
B $E50D,1,1 DOUBLE_STREET_LAMP for 1 units
B $E50E,1,1 (nothing) for 1 units
B $E50F,1,1 DOUBLE_STREET_LAMP for 1 units
B $E510,1,1 (nothing) for 1 units
B $E511,1,1 DOUBLE_STREET_LAMP for 1 units
B $E512,1,1 (nothing) for 1 units
B $E513,1,1 DOUBLE_STREET_LAMP for 1 units
B $E514,1,1 (nothing) for 3 units
B $E515,1,1 DOUBLE_STREET_LAMP for 1 units
B $E516,1,1 (nothing) for 3 units
B $E517,1,1 LEAVES for 1 units
B $E518,1,1 (nothing) for 1 units
B $E519,1,1 DOUBLE_STREET_LAMP for 1 units
B $E51A,1,1 (nothing) for 1 units
B $E51B,1,1 LEAVES for 1 units
B $E51C,1,1 (nothing) for 1 units
B $E51D,1,1 LEAVES for 1 units
B $E51E,1,1 (nothing) for 1 units
B $E51F,1,1 LEAVES for 1 units
B $E520,1,1 (nothing) for 3 units
B $E521,1,1 LEAVES for 1 units
B $E522,1,1 (nothing) for 1 units
B $E523,1,1 DOUBLE_STREET_LAMP for 1 units
B $E524,1,1 (nothing) for 1 units
B $E525,1,1 DOUBLE_STREET_LAMP for 1 units
B $E526,1,1 (nothing) for 1 units
B $E527,1,1 DOUBLE_STREET_LAMP for 1 units
B $E528,1,1 (nothing) for 3 units
B $E529,1,1 DOUBLE_STREET_LAMP for 1 units
B $E52A,1,1 (nothing) for 1 units
B $E52B,1,1 DOUBLE_STREET_LAMP for 1 units
B $E52C,1,1 (nothing) for 1 units
B $E52D,1,1 DOUBLE_STREET_LAMP for 1 units
B $E52E,1,1 (nothing) for 3 units
B $E52F,1,1 DOUBLE_STREET_LAMP for 1 units
B $E530,1,1 (nothing) for 3 units
B $E531,1,1 DOUBLE_STREET_LAMP for 1 units
B $E532,1,1 (nothing) for 1 units
B $E533,1,1 DOUBLE_STREET_LAMP for 1 units
B $E534,1,1 (nothing) for 1 units
B $E535,1,1 DOUBLE_STREET_LAMP for 1 units
B $E536,1,1 (nothing) for 3 units
B $E537,1,1 DOUBLE_STREET_LAMP for 1 units
B $E538,1,1 <Esc> Jump
B $E539,1,1
W $E53A,2,2 [$C668] Target
b $E53C [Stage 2] Map curvature data
B $E53C,1,1 Curve Straight for 84 units
B $E53D,5,5
B $E542,1,1 Curve Left for 97 units
B $E543,6,6
B $E549,1,1 Curve Straight for 29 units
B $E54A,1,1
B $E54B,1,1 <Esc> Jump
B $E54C,1,1
W $E54D,2,2 [$C60B] Target
b $E54F [Stage 2] Map height data
B $E54F,1,1 Level Road for 210 units
B $E550,13,8,5
B $E55D,1,1 <Esc> Jump
B $E55E,1,1
W $E55F,2,2 [$C61E] Target
b $E561 [Stage 2] Map lanes data
B $E561,1,1 4 Lanes              [||||] {00} for 6 units
B $E562,1,1
B $E563,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E564,1,1
B $E565,1,1 3 Lanes L            [|||]  {81} for 8 units
B $E566,1,1
B $E567,1,1 Tunnel start                {45} for 44 units
B $E568,1,1
B $E569,1,1 Tunnel cont/end?            {59} for 2 units
B $E56A,1,1
B $E56B,1,1 3 Lanes L            [|||]  {81} for 40 units
B $E56C,1,1
B $E56D,1,1 [3D] for 2 units
B $E56E,1,1
B $E56F,1,1 2 Lanes L            [||]   {01} for 38 units
B $E570,1,1
B $E571,1,1 2-3 Widening L       [\||]  {2D} for 2 units
B $E572,1,1
B $E573,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E574,1,1
B $E575,1,1 4 Lanes              [||||] {00} for 64 units
B $E576,1,1
B $E577,1,1 <Esc> Jump
B $E578,1,1
W $E579,2,2 [$C62F] Target
b $E57B [Stage 2] Map hazards data
B $E57B,1,1 Wait for 5 units
B $E57C,1,1 Unknown command F
B $E57D,1,1
B $E57E,1,1 Wait for 7 units
B $E57F,1,1 Enable Car Spawning
B $E580,1,1
B $E581,1,1 Wait for 59 units
B $E582,1,1 Start Spawning Barriers Left
B $E583,1,1
B $E584,1,1 Wait for 1 units
B $E585,1,1 Stop Spawning Barriers 3?
B $E586,1,1
B $E587,1,1 Wait for 22 units
B $E588,1,1 Stop Spawning Barriers 6?
B $E589,1,1
B $E58A,1,1 Wait for 4 units
B $E58B,1,1 Stop Spawning Barriers 3?
B $E58C,1,1
B $E58D,1,1 Wait for 7 units
B $E58E,1,1 <Esc> Jump
B $E58F,1,1
W $E590,2,2 [$C643] Target
b $E592 [Stage 2] Map left object data
B $E592,1,1 (nothing) for 9 units
B $E593,1,1 TUNNEL_LIGHT for 22 units
B $E594,1,1
B $E595,1,1 (nothing) for 5 units
B $E596,1,1 DOUBLE_STREET_LAMP for 1 units
B $E597,1,1 (nothing) for 1 units
B $E598,1,1 DOUBLE_STREET_LAMP for 1 units
B $E599,1,1 (nothing) for 1 units
B $E59A,1,1 DOUBLE_STREET_LAMP for 1 units
B $E59B,1,1 (nothing) for 1 units
B $E59C,1,1 DOUBLE_STREET_LAMP for 1 units
B $E59D,1,1 (nothing) for 1 units
B $E59E,1,1 DOUBLE_STREET_LAMP for 1 units
B $E59F,1,1 (nothing) for 1 units
B $E5A0,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5A1,1,1 (nothing) for 3 units
B $E5A2,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5A3,1,1 (nothing) for 3 units
B $E5A4,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5A5,1,1 (nothing) for 1 units
B $E5A6,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5A7,1,1 (nothing) for 1 units
B $E5A8,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5A9,1,1 (nothing) for 1 units
B $E5AA,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5AB,1,1 (nothing) for 1 units
B $E5AC,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5AD,1,1 (nothing) for 1 units
B $E5AE,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5AF,1,1 (nothing) for 1 units
B $E5B0,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5B1,1,1 (nothing) for 5 units
B $E5B2,1,1 LEAVES for 1 units
B $E5B3,1,1 (nothing) for 1 units
B $E5B4,1,1 LEAVES for 1 units
B $E5B5,1,1 (nothing) for 1 units
B $E5B6,1,1 LEAVES for 1 units
B $E5B7,1,1 (nothing) for 1 units
B $E5B8,1,1 LEAVES for 1 units
B $E5B9,1,1 (nothing) for 3 units
B $E5BA,1,1 LEAVES for 1 units
B $E5BB,1,1 (nothing) for 3 units
B $E5BC,1,1 LEAVES for 1 units
B $E5BD,1,1 (nothing) for 1 units
B $E5BE,1,1 LEAVES for 1 units
B $E5BF,1,1 (nothing) for 1 units
B $E5C0,1,1 LEAVES for 1 units
B $E5C1,1,1 (nothing) for 1 units
B $E5C2,1,1 LEAVES for 1 units
B $E5C3,1,1 (nothing) for 1 units
B $E5C4,1,1 LEAVES for 1 units
B $E5C5,1,1 (nothing) for 3 units
B $E5C6,1,1 LEAVES for 1 units
B $E5C7,1,1 (nothing) for 1 units
B $E5C8,1,1 LEAVES for 1 units
B $E5C9,1,1 (nothing) for 1 units
B $E5CA,1,1 LEAVES for 1 units
B $E5CB,1,1 (nothing) for 2 units
B $E5CC,1,1 <Esc> Jump
B $E5CD,1,1
W $E5CE,2,2 [$C648] Target
b $E5D0 [Stage 2] Map right object data
B $E5D0,1,1 (nothing) for 9 units
B $E5D1,1,1 TUNNEL_LIGHT for 22 units
B $E5D2,1,1
B $E5D3,1,1 (nothing) for 13 units
B $E5D4,1,1 PALM_TREE for 1 units
B $E5D5,1,1 (nothing) for 1 units
B $E5D6,1,1 PALM_TREE for 1 units
B $E5D7,1,1 (nothing) for 1 units
B $E5D8,1,1 PALM_TREE for 1 units
B $E5D9,1,1 (nothing) for 1 units
B $E5DA,1,1 PALM_TREE for 1 units
B $E5DB,1,1 (nothing) for 3 units
B $E5DC,1,1 PALM_TREE for 1 units
B $E5DD,1,1 (nothing) for 1 units
B $E5DE,1,1 PALM_TREE for 1 units
B $E5DF,1,1 (nothing) for 3 units
B $E5E0,1,1 PALM_TREE for 1 units
B $E5E1,1,1 (nothing) for 1 units
B $E5E2,1,1 PALM_TREE for 1 units
B $E5E3,1,1 (nothing) for 1 units
B $E5E4,1,1 PALM_TREE for 1 units
B $E5E5,1,1 (nothing) for 1 units
B $E5E6,1,1 PALM_TREE for 1 units
B $E5E7,1,1 (nothing) for 3 units
B $E5E8,1,1 PALM_TREE for 1 units
B $E5E9,1,1 (nothing) for 1 units
B $E5EA,1,1 PALM_TREE for 1 units
B $E5EB,1,1 (nothing) for 1 units
B $E5EC,1,1 PALM_TREE for 1 units
B $E5ED,1,1 (nothing) for 3 units
B $E5EE,1,1 PALM_TREE for 1 units
B $E5EF,1,1 (nothing) for 3 units
B $E5F0,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5F1,1,1 (nothing) for 1 units
B $E5F2,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5F3,1,1 (nothing) for 1 units
B $E5F4,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5F5,1,1 (nothing) for 1 units
B $E5F6,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5F7,1,1 (nothing) for 1 units
B $E5F8,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5F9,1,1 (nothing) for 1 units
B $E5FA,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5FB,1,1 (nothing) for 1 units
B $E5FC,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5FD,1,1 (nothing) for 1 units
B $E5FE,1,1 DOUBLE_STREET_LAMP for 1 units
B $E5FF,1,1 (nothing) for 1 units
B $E600,1,1 DOUBLE_STREET_LAMP for 1 units
B $E601,1,1 (nothing) for 1 units
B $E602,1,1 DOUBLE_STREET_LAMP for 1 units
B $E603,1,1 (nothing) for 1 units
B $E604,1,1 DOUBLE_STREET_LAMP for 1 units
B $E605,1,1 (nothing) for 1 units
B $E606,1,1 DOUBLE_STREET_LAMP for 1 units
B $E607,1,1 <Esc> Jump
B $E608,1,1
W $E609,2,2 [$C668] Target
b $E60B [Stage 2] Map curvature data
B $E60B,1,1 Curve Straight for 28 units
B $E60C,1,1
B $E60D,1,1 Curve Right Hard for 22 units
B $E60E,1,1
B $E60F,1,1 Curve Straight for 8 units
B $E610,1,1 Curve Left Hard for 26 units
B $E611,1,1
B $E612,1,1 Curve Straight for 12 units
B $E613,1,1 Curve Right for 42 units
B $E614,2,2
B $E616,1,1 Curve Straight for 52 units
B $E617,3,3
B $E61A,1,1 <Esc> Jump
B $E61B,1,1
W $E61C,2,2 [$C674] Target
b $E61E [Stage 2] Map height data
B $E61E,1,1 Level Road for 190 units
B $E61F,12,8,4
B $E62B,1,1 <Esc> Jump
B $E62C,1,1
W $E62D,2,2 [$C69C] Target
b $E62F [Stage 2] Map lanes data
B $E62F,1,1 4 Lanes              [||||] {00} for 8 units
B $E630,1,1
B $E631,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E632,1,1
B $E633,1,1 3 Lanes L            [|||]  {81} for 10 units
B $E634,1,1
B $E635,1,1 Tunnel start                {45} for 122 units
B $E636,1,1
B $E637,1,1 Tunnel cont/end?            {59} for 2 units
B $E638,1,1
B $E639,1,1 3 Lanes L            [|||]  {81} for 10 units
B $E63A,1,1
B $E63B,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E63C,1,1
B $E63D,1,1 4 Lanes              [||||] {00} for 34 units
B $E63E,1,1
B $E63F,1,1 <Esc> Jump
B $E640,1,1
W $E641,2,2 [$C6E0] Target
b $E643 [Stage 2] Map hazards data
B $E643,1,1 Wait for 95 units
B $E644,1,1 <Esc> Jump
B $E645,1,1
W $E646,2,2 [$C6F0] Target
b $E648 [Stage 2] Map left object data
B $E648,1,1 (nothing) for 11 units
B $E649,1,1 TUNNEL_LIGHT for 14 units
B $E64A,1,1 (nothing) for 21 units
B $E64B,1,1
B $E64C,1,1 TUNNEL_LIGHT for 26 units
B $E64D,1,1
B $E64E,1,1 (nothing) for 2 units
B $E64F,1,1 DOUBLE_STREET_LAMP for 1 units
B $E650,1,1 (nothing) for 1 units
B $E651,1,1 DOUBLE_STREET_LAMP for 1 units
B $E652,1,1 (nothing) for 1 units
B $E653,1,1 DOUBLE_STREET_LAMP for 1 units
B $E654,1,1 (nothing) for 1 units
B $E655,1,1 DOUBLE_STREET_LAMP for 1 units
B $E656,1,1 (nothing) for 1 units
B $E657,1,1 DOUBLE_STREET_LAMP for 1 units
B $E658,1,1 (nothing) for 1 units
B $E659,1,1 DOUBLE_STREET_LAMP for 1 units
B $E65A,1,1 (nothing) for 1 units
B $E65B,1,1 DOUBLE_STREET_LAMP for 1 units
B $E65C,1,1 (nothing) for 1 units
B $E65D,1,1 DOUBLE_STREET_LAMP for 1 units
B $E65E,1,1 (nothing) for 1 units
B $E65F,1,1 DOUBLE_STREET_LAMP for 1 units
B $E660,1,1 (nothing) for 1 units
B $E661,1,1 DOUBLE_STREET_LAMP for 1 units
B $E662,1,1 (nothing) for 1 units
B $E663,1,1 DOUBLE_STREET_LAMP for 1 units
B $E664,1,1 <Esc> Jump
B $E665,1,1
W $E666,2,2 [$C713] Target
b $E668 [Stage 2] Map right object data
B $E668,1,1 (nothing) for 26 units
B $E669,1,1
B $E66A,1,1 TUNNEL_LIGHT for 19 units
B $E66B,1,1
B $E66C,1,1 (nothing) for 50 units
B $E66D,3,3
B $E670,1,1 <Esc> Jump
B $E671,1,1
W $E672,2,2 [$C79B] Target
b $E674 [Stage 2] Map curvature data
B $E674,1,1 Curve Straight for 28 units
B $E675,1,1
B $E676,1,1 Curve Right for 37 units
B $E677,2,2
B $E679,1,1 Curve Straight for 23 units
B $E67A,1,1
B $E67B,1,1 Curve Left for 8 units
B $E67C,1,1 Curve Left Hard for 14 units
B $E67D,1,1 Curve Left for 31 units
B $E67E,2,2
B $E680,1,1 Curve Straight for 31 units
B $E681,2,2
B $E683,1,1 Curve Left for 49 units
B $E684,3,3
B $E687,1,1 Curve Straight for 59 units
B $E688,3,3
B $E68B,1,1 Curve Right Hard for 34 units
B $E68C,2,2
B $E68E,1,1 Curve Right for 27 units
B $E68F,1,1
B $E690,1,1 Curve Straight for 59 units
B $E691,3,3
B $E694,1,1 Curve Left for 41 units
B $E695,2,2
B $E697,1,1 Curve Straight for 9 units
B $E698,1,1 <Esc> Loop
B $E699,1,1
W $E69A,2,2 [$C674] Target
b $E69C [Stage 2] Map height data
B $E69C,1,1 Level Road for 11 units
B $E69D,1,1 Going Up 3 for 14 units
B $E69E,1,1 Going Down 5 for 20 units
B $E69F,1,1
B $E6A0,1,1 Going Up 3 for 20 units
B $E6A1,1,1
B $E6A2,1,1 Going Up 5 for 3 units
B $E6A3,1,1 Going Up 7 for 4 units
B $E6A4,1,1 Going Down 5 for 5 units
B $E6A5,1,1 Going Down 3 for 2 units
B $E6A6,1,1 Level Road for 67 units
B $E6A7,4,4
B $E6AB,1,1 Going Down 1 for 1 units
B $E6AC,1,1 Going Down 3 for 1 units
B $E6AD,1,1 Going Down 5 for 4 units
B $E6AE,1,1 Going Down 3 for 1 units
B $E6AF,1,1 Going Up 3 for 1 units
B $E6B0,1,1 Going Up 5 for 1 units
B $E6B1,1,1 Going Up 7 for 10 units
B $E6B2,1,1 Going Up 5 for 2 units
B $E6B3,1,1 Going Down 3 for 4 units
B $E6B4,1,1 Going Down 5 for 11 units
B $E6B5,1,1 Going Down 3 for 1 units
B $E6B6,1,1 Going Down 1 for 1 units
B $E6B7,1,1 Level Road for 34 units
B $E6B8,2,2
B $E6BA,1,1 Going Up 3 for 9 units
B $E6BB,1,1 Going Down 3 for 16 units
B $E6BC,1,1
B $E6BD,1,1 Going Up 3 for 7 units
B $E6BE,1,1 Going Up 7 for 3 units
B $E6BF,1,1 Going Up 5 for 1 units
B $E6C0,1,1 Going Up 3 for 1 units
B $E6C1,1,1 Level Road for 13 units
B $E6C2,1,1 Going Down 1 for 2 units
B $E6C3,1,1 Going Down 3 for 12 units
B $E6C4,1,1 Level Road for 82 units
B $E6C5,5,5
B $E6CA,1,1 Going Up 3 for 1 units
B $E6CB,1,1 Going Up 5 for 1 units
B $E6CC,1,1 Going Up 7 for 3 units
B $E6CD,1,1 Going Up 5 for 1 units
B $E6CE,1,1 Going Up 3 for 1 units
B $E6CF,1,1 Going Up 1 for 1 units
B $E6D0,1,1 Going Down 1 for 1 units
B $E6D1,1,1 Going Down 3 for 1 units
B $E6D2,1,1 Going Down 5 for 1 units
B $E6D3,1,1 Going Down 7 for 2 units
B $E6D4,1,1 Going Down 5 for 2 units
B $E6D5,1,1 Going Down 3 for 2 units
B $E6D6,1,1 Going Down 1 for 1 units
B $E6D7,1,1 Level Road for 68 units
B $E6D8,4,4
B $E6DC,1,1 <Esc> Loop
B $E6DD,1,1
W $E6DE,2,2 [$C69C] Target
b $E6E0 [Stage 2] Map lanes data
B $E6E0,1,1 4 Lanes              [||||] {00} for 92 units
B $E6E1,1,1
B $E6E2,1,1 3 Lanes L            [|||]  {81} for 46 units
B $E6E3,1,1
B $E6E4,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E6E5,1,1
B $E6E6,1,1 4 Lanes              [||||] {00} for 102 units
B $E6E7,1,1
B $E6E8,1,1 3 Lanes R             [|||] {82} for 6 units
B $E6E9,1,1
B $E6EA,1,1 4 Lanes              [||||] {00} for 202 units
B $E6EB,1,1
B $E6EC,1,1 <Esc> Loop
B $E6ED,1,1
W $E6EE,2,2 [$C6E0] Target
b $E6F0 [Stage 2] Map hazards data
B $E6F0,1,1 Wait for 19 units
B $E6F1,1,1 Unknown command 4
B $E6F2,1,1
B $E6F3,1,1 Wait for 3 units
B $E6F4,1,1 Stop Spawning Barriers 3?
B $E6F5,1,1
B $E6F6,1,1 Wait for 23 units
B $E6F7,1,1 Start Spawning Barriers Right
B $E6F8,1,1
B $E6F9,1,1 Wait for 2 units
B $E6FA,1,1 Stop Spawning Barriers 3?
B $E6FB,1,1
B $E6FC,1,1 Wait for 54 units
B $E6FD,1,1 Unknown command 5
B $E6FE,1,1
B $E6FF,1,1 Wait for 2 units
B $E700,1,1 Stop Spawning Barriers 3?
B $E701,1,1
B $E702,1,1 Wait for 18 units
B $E703,1,1 Start Spawning Barriers Left
B $E704,1,1
B $E705,1,1 Wait for 1 units
B $E706,1,1 Stop Spawning Barriers 3?
B $E707,1,1
B $E708,1,1 Wait for 2 units
B $E709,1,1 Start Spawning Barriers Left
B $E70A,1,1
B $E70B,1,1 Wait for 1 units
B $E70C,1,1 Stop Spawning Barriers 3?
B $E70D,1,1
B $E70E,1,1 Wait for 100 units
B $E70F,1,1 <Esc> Loop
B $E710,1,1
W $E711,2,2 [$C6F0] Target
b $E713 [Stage 2] Map left object data
B $E713,1,1 DOUBLE_STREET_LAMP for 1 units
B $E714,1,1 (nothing) for 1 units
B $E715,1,1 DOUBLE_STREET_LAMP for 1 units
B $E716,1,1 (nothing) for 1 units
B $E717,1,1 DOUBLE_STREET_LAMP for 1 units
B $E718,1,1 (nothing) for 5 units
B $E719,1,1 HUGE_ROCK for 1 units
B $E71A,1,1 (nothing) for 1 units
B $E71B,1,1 HUGE_ROCK for 1 units
B $E71C,1,1 (nothing) for 1 units
B $E71D,1,1 HUGE_ROCK for 1 units
B $E71E,1,1 (nothing) for 1 units
B $E71F,1,1 HUGE_ROCK for 1 units
B $E720,1,1 (nothing) for 1 units
B $E721,1,1 HUGE_ROCK for 1 units
B $E722,1,1 (nothing) for 1 units
B $E723,1,1 HUGE_ROCK for 1 units
B $E724,1,1 (nothing) for 1 units
B $E725,1,1 HUGE_ROCK for 1 units
B $E726,1,1 (nothing) for 1 units
B $E727,1,1 HUGE_ROCK for 1 units
B $E728,1,1 (nothing) for 1 units
B $E729,1,1 HUGE_ROCK for 1 units
B $E72A,1,1 (nothing) for 1 units
B $E72B,1,1 HUGE_ROCK for 1 units
B $E72C,1,1 (nothing) for 1 units
B $E72D,1,1 HUGE_ROCK for 1 units
B $E72E,1,1 (nothing) for 1 units
B $E72F,1,1 HUGE_ROCK for 1 units
B $E730,1,1 (nothing) for 1 units
B $E731,1,1 HUGE_ROCK for 1 units
B $E732,1,1 (nothing) for 1 units
B $E733,1,1 HUGE_ROCK for 1 units
B $E734,1,1 (nothing) for 1 units
B $E735,1,1 HUGE_ROCK for 1 units
B $E736,1,1 (nothing) for 1 units
B $E737,1,1 HUGE_ROCK for 1 units
B $E738,1,1 (nothing) for 3 units
B $E739,1,1 PALM_TREE for 1 units
B $E73A,1,1 (nothing) for 1 units
B $E73B,1,1 PALM_TREE for 1 units
B $E73C,1,1 (nothing) for 1 units
B $E73D,1,1 PALM_TREE for 1 units
B $E73E,1,1 (nothing) for 1 units
B $E73F,1,1 PALM_TREE for 1 units
B $E740,1,1 (nothing) for 1 units
B $E741,1,1 PALM_TREE for 1 units
B $E742,1,1 (nothing) for 3 units
B $E743,1,1 LEAVES for 1 units
B $E744,1,1 (nothing) for 1 units
B $E745,1,1 LEAVES for 1 units
B $E746,1,1 (nothing) for 3 units
B $E747,1,1 LEAVES for 1 units
B $E748,1,1 (nothing) for 9 units
B $E749,1,1 LEAVES for 1 units
B $E74A,1,1 (nothing) for 1 units
B $E74B,1,1 LEAVES for 1 units
B $E74C,1,1 (nothing) for 1 units
B $E74D,1,1 LEAVES for 1 units
B $E74E,1,1 (nothing) for 1 units
B $E74F,1,1 LEAVES for 1 units
B $E750,1,1 (nothing) for 23 units
B $E751,1,1
B $E752,1,1 PALM_TREE for 1 units
B $E753,1,1 (nothing) for 1 units
B $E754,1,1 PALM_TREE for 1 units
B $E755,1,1 (nothing) for 1 units
B $E756,1,1 PALM_TREE for 1 units
B $E757,1,1 (nothing) for 1 units
B $E758,1,1 PALM_TREE for 1 units
B $E759,1,1 (nothing) for 1 units
B $E75A,1,1 PALM_TREE for 1 units
B $E75B,1,1 (nothing) for 1 units
B $E75C,1,1 PALM_TREE for 1 units
B $E75D,1,1 (nothing) for 1 units
B $E75E,1,1 PALM_TREE for 1 units
B $E75F,1,1 (nothing) for 1 units
B $E760,1,1 PALM_TREE for 1 units
B $E761,1,1 (nothing) for 47 units
B $E762,3,3
B $E765,1,1 HUGE_ROCK for 1 units
B $E766,1,1 (nothing) for 1 units
B $E767,1,1 HUGE_ROCK for 1 units
B $E768,1,1 (nothing) for 1 units
B $E769,1,1 HUGE_ROCK for 1 units
B $E76A,1,1 (nothing) for 1 units
B $E76B,1,1 HUGE_ROCK for 1 units
B $E76C,1,1 (nothing) for 1 units
B $E76D,1,1 HUGE_ROCK for 1 units
B $E76E,1,1 (nothing) for 1 units
B $E76F,1,1 HUGE_ROCK for 1 units
B $E770,1,1 (nothing) for 1 units
B $E771,1,1 HUGE_ROCK for 1 units
B $E772,1,1 (nothing) for 1 units
B $E773,1,1 HUGE_ROCK for 1 units
B $E774,1,1 (nothing) for 1 units
B $E775,1,1 HUGE_ROCK for 1 units
B $E776,1,1 (nothing) for 1 units
B $E777,1,1 HUGE_ROCK for 1 units
B $E778,1,1 (nothing) for 1 units
B $E779,1,1 HUGE_ROCK for 1 units
B $E77A,1,1 (nothing) for 1 units
B $E77B,1,1 HUGE_ROCK for 1 units
B $E77C,1,1 (nothing) for 1 units
B $E77D,1,1 HUGE_ROCK for 1 units
B $E77E,1,1 (nothing) for 1 units
B $E77F,1,1 HUGE_ROCK for 1 units
B $E780,1,1 (nothing) for 3 units
B $E781,1,1 HUGE_ROCK for 1 units
B $E782,1,1 (nothing) for 3 units
B $E783,1,1 HUGE_ROCK for 1 units
B $E784,1,1 (nothing) for 3 units
B $E785,1,1 HUGE_ROCK for 1 units
B $E786,1,1 (nothing) for 1 units
B $E787,1,1 HUGE_ROCK for 1 units
B $E788,1,1 (nothing) for 1 units
B $E789,1,1 HUGE_ROCK for 1 units
B $E78A,1,1 (nothing) for 1 units
B $E78B,1,1 HUGE_ROCK for 1 units
B $E78C,1,1 (nothing) for 1 units
B $E78D,1,1 HUGE_ROCK for 1 units
B $E78E,1,1 (nothing) for 1 units
B $E78F,1,1 HUGE_ROCK for 1 units
B $E790,1,1 (nothing) for 1 units
B $E791,1,1 HUGE_ROCK for 1 units
B $E792,1,1 (nothing) for 3 units
B $E793,1,1 HUGE_ROCK for 1 units
B $E794,1,1 (nothing) for 3 units
B $E795,1,1 HUGE_ROCK for 1 units
B $E796,1,1 (nothing) for 2 units
B $E797,1,1 <Esc> Loop
B $E798,1,1
W $E799,2,2 [$C713] Target
b $E79B [Stage 2] Map right object data
B $E79B,1,1 (nothing) for 2 units
B $E79C,1,1 LEAVES for 1 units
B $E79D,1,1 (nothing) for 3 units
B $E79E,1,1 LEAVES for 1 units
B $E79F,1,1 (nothing) for 3 units
B $E7A0,1,1 LEAVES for 1 units
B $E7A1,1,1 (nothing) for 1 units
B $E7A2,1,1 LEAVES for 1 units
B $E7A3,1,1 (nothing) for 3 units
B $E7A4,1,1 LEAVES for 1 units
B $E7A5,1,1 (nothing) for 3 units
B $E7A6,1,1 LEAVES for 1 units
B $E7A7,1,1 (nothing) for 7 units
B $E7A8,1,1 PALM_TREE for 1 units
B $E7A9,1,1 (nothing) for 3 units
B $E7AA,1,1 PALM_TREE for 1 units
B $E7AB,1,1 (nothing) for 1 units
B $E7AC,1,1 PALM_TREE for 1 units
B $E7AD,1,1 (nothing) for 1 units
B $E7AE,1,1 PALM_TREE for 1 units
B $E7AF,1,1 (nothing) for 9 units
B $E7B0,1,1 HUGE_ROCK for 1 units
B $E7B1,1,1 (nothing) for 1 units
B $E7B2,1,1 HUGE_ROCK for 1 units
B $E7B3,1,1 (nothing) for 1 units
B $E7B4,1,1 HUGE_ROCK for 1 units
B $E7B5,1,1 (nothing) for 1 units
B $E7B6,1,1 HUGE_ROCK for 1 units
B $E7B7,1,1 (nothing) for 1 units
B $E7B8,1,1 HUGE_ROCK for 1 units
B $E7B9,1,1 (nothing) for 1 units
B $E7BA,1,1 HUGE_ROCK for 1 units
B $E7BB,1,1 (nothing) for 1 units
B $E7BC,1,1 HUGE_ROCK for 1 units
B $E7BD,1,1 (nothing) for 1 units
B $E7BE,1,1 HUGE_ROCK for 1 units
B $E7BF,1,1 (nothing) for 1 units
B $E7C0,1,1 HUGE_ROCK for 1 units
B $E7C1,1,1 (nothing) for 1 units
B $E7C2,1,1 HUGE_ROCK for 1 units
B $E7C3,1,1 (nothing) for 1 units
B $E7C4,1,1 HUGE_ROCK for 1 units
B $E7C5,1,1 (nothing) for 1 units
B $E7C6,1,1 HUGE_ROCK for 1 units
B $E7C7,1,1 (nothing) for 1 units
B $E7C8,1,1 HUGE_ROCK for 1 units
B $E7C9,1,1 (nothing) for 1 units
B $E7CA,1,1 HUGE_ROCK for 1 units
B $E7CB,1,1 (nothing) for 1 units
B $E7CC,1,1 HUGE_ROCK for 1 units
B $E7CD,1,1 (nothing) for 1 units
B $E7CE,1,1 HUGE_ROCK for 1 units
B $E7CF,1,1 (nothing) for 1 units
B $E7D0,1,1 HUGE_ROCK for 1 units
B $E7D1,1,1 (nothing) for 1 units
B $E7D2,1,1 HUGE_ROCK for 1 units
B $E7D3,1,1 (nothing) for 1 units
B $E7D4,1,1 HUGE_ROCK for 1 units
B $E7D5,1,1 (nothing) for 1 units
B $E7D6,1,1 HUGE_ROCK for 1 units
B $E7D7,1,1 (nothing) for 1 units
B $E7D8,1,1 HUGE_ROCK for 1 units
B $E7D9,1,1 (nothing) for 1 units
B $E7DA,1,1 HUGE_ROCK for 1 units
B $E7DB,1,1 (nothing) for 1 units
B $E7DC,1,1 HUGE_ROCK for 1 units
B $E7DD,1,1 (nothing) for 1 units
B $E7DE,1,1 HUGE_ROCK for 1 units
B $E7DF,1,1 (nothing) for 1 units
B $E7E0,1,1 HUGE_ROCK for 1 units
B $E7E1,1,1 (nothing) for 5 units
B $E7E2,1,1 HUGE_ROCK for 1 units
B $E7E3,1,1 (nothing) for 1 units
B $E7E4,1,1 HUGE_ROCK for 1 units
B $E7E5,1,1 (nothing) for 1 units
B $E7E6,1,1 HUGE_ROCK for 1 units
B $E7E7,1,1 (nothing) for 1 units
B $E7E8,1,1 HUGE_ROCK for 1 units
B $E7E9,1,1 (nothing) for 1 units
B $E7EA,1,1 HUGE_ROCK for 1 units
B $E7EB,1,1 (nothing) for 1 units
B $E7EC,1,1 HUGE_ROCK for 1 units
B $E7ED,1,1 (nothing) for 1 units
B $E7EE,1,1 HUGE_ROCK for 1 units
B $E7EF,1,1 (nothing) for 1 units
B $E7F0,1,1 HUGE_ROCK for 1 units
B $E7F1,1,1 (nothing) for 1 units
B $E7F2,1,1 HUGE_ROCK for 1 units
B $E7F3,1,1 (nothing) for 1 units
B $E7F4,1,1 HUGE_ROCK for 1 units
B $E7F5,1,1 (nothing) for 1 units
B $E7F6,1,1 HUGE_ROCK for 1 units
B $E7F7,1,1 (nothing) for 1 units
B $E7F8,1,1 HUGE_ROCK for 1 units
B $E7F9,1,1 (nothing) for 1 units
B $E7FA,1,1 HUGE_ROCK for 1 units
B $E7FB,1,1 (nothing) for 1 units
B $E7FC,1,1 HUGE_ROCK for 1 units
B $E7FD,1,1 (nothing) for 1 units
B $E7FE,1,1 HUGE_ROCK for 1 units
B $E7FF,1,1 (nothing) for 1 units
B $E800,1,1 HUGE_ROCK for 1 units
B $E801,1,1 (nothing) for 1 units
B $E802,1,1 HUGE_ROCK for 1 units
B $E803,1,1 (nothing) for 1 units
B $E804,1,1 HUGE_ROCK for 1 units
B $E805,1,1 (nothing) for 1 units
B $E806,1,1 HUGE_ROCK for 1 units
B $E807,1,1 (nothing) for 1 units
B $E808,1,1 HUGE_ROCK for 1 units
B $E809,1,1 (nothing) for 1 units
B $E80A,1,1 HUGE_ROCK for 1 units
B $E80B,1,1 (nothing) for 1 units
B $E80C,1,1 HUGE_ROCK for 1 units
B $E80D,1,1 (nothing) for 1 units
B $E80E,1,1 HUGE_ROCK for 1 units
B $E80F,1,1 (nothing) for 1 units
B $E810,1,1 HUGE_ROCK for 1 units
B $E811,1,1 (nothing) for 1 units
B $E812,1,1 HUGE_ROCK for 1 units
B $E813,1,1 (nothing) for 1 units
B $E814,1,1 HUGE_ROCK for 1 units
B $E815,1,1 (nothing) for 1 units
B $E816,1,1 HUGE_ROCK for 1 units
B $E817,1,1 (nothing) for 3 units
B $E818,1,1 PALM_TREE for 1 units
B $E819,1,1 (nothing) for 1 units
B $E81A,1,1 PALM_TREE for 1 units
B $E81B,1,1 (nothing) for 1 units
B $E81C,1,1 PALM_TREE for 1 units
B $E81D,1,1 (nothing) for 3 units
B $E81E,1,1 HUGE_ROCK for 1 units
B $E81F,1,1 (nothing) for 1 units
B $E820,1,1 HUGE_ROCK for 1 units
B $E821,1,1 (nothing) for 1 units
B $E822,1,1 HUGE_ROCK for 1 units
B $E823,1,1 (nothing) for 1 units
B $E824,1,1 HUGE_ROCK for 1 units
B $E825,1,1 (nothing) for 1 units
B $E826,1,1 HUGE_ROCK for 1 units
B $E827,1,1 (nothing) for 7 units
B $E828,1,1 HUGE_ROCK for 1 units
B $E829,1,1 (nothing) for 1 units
B $E82A,1,1 HUGE_ROCK for 1 units
B $E82B,1,1 (nothing) for 1 units
B $E82C,1,1 HUGE_ROCK for 1 units
B $E82D,1,1 (nothing) for 1 units
B $E82E,1,1 HUGE_ROCK for 1 units
B $E82F,1,1 (nothing) for 1 units
B $E830,1,1 HUGE_ROCK for 1 units
B $E831,1,1 (nothing) for 1 units
B $E832,1,1 HUGE_ROCK for 1 units
B $E833,1,1 (nothing) for 1 units
B $E834,1,1 HUGE_ROCK for 1 units
B $E835,1,1 (nothing) for 3 units
B $E836,1,1 HUGE_ROCK for 1 units
B $E837,1,1 (nothing) for 3 units
B $E838,1,1 HUGE_ROCK for 1 units
B $E839,1,1 (nothing) for 3 units
B $E83A,1,1 HUGE_ROCK for 1 units
B $E83B,1,1 (nothing) for 9 units
B $E83C,1,1 DOUBLE_STREET_LAMP for 1 units
B $E83D,1,1 (nothing) for 1 units
B $E83E,1,1 DOUBLE_STREET_LAMP for 1 units
B $E83F,1,1 (nothing) for 1 units
B $E840,1,1 DOUBLE_STREET_LAMP for 1 units
B $E841,1,1 (nothing) for 1 units
B $E842,1,1 DOUBLE_STREET_LAMP for 1 units
B $E843,1,1 (nothing) for 1 units
B $E844,1,1 DOUBLE_STREET_LAMP for 1 units
B $E845,1,1 (nothing) for 1 units
B $E846,1,1 DOUBLE_STREET_LAMP for 1 units
B $E847,1,1 <Esc> Loop
B $E848,1,1
W $E849,2,2 [$C79B] Target
B $E84B,180,8*22,4
N $E8FF LOD
B $E8FF,1,1 Width (bytes)
B $E900,1,1 Flags
B $E901,1,1 Height (pixels)
W $E902,2,2 [$C97D] Bitmap address
W $E904,2,2 [$C97D] Pre-shifted bitmap address
N $E906 LOD
B $E906,1,1 Width (bytes)
B $E907,1,1 Flags
B $E908,1,1 Height (pixels)
W $E909,2,2 [$CA2B] Bitmap address
W $E90B,2,2 [$CA2B] Pre-shifted bitmap address
N $E90D LOD
B $E90D,1,1 Width (bytes)
B $E90E,1,1 Flags
B $E90F,1,1 Height (pixels)
W $E910,2,2 [$CA77] Bitmap address
W $E912,2,2 [$CA77] Pre-shifted bitmap address
N $E914 LOD
B $E914,1,1 Width (bytes)
B $E915,1,1 Flags
B $E916,1,1 Height (pixels)
W $E917,2,2 [$CA77] Bitmap address
W $E919,2,2 [$CA77] Pre-shifted bitmap address
N $E91B LOD
B $E91B,1,1 Width (bytes)
B $E91C,1,1 Flags
B $E91D,1,1 Height (pixels)
W $E91E,2,2 [$CA9E] Bitmap address
W $E920,2,2 [$CA9E] Pre-shifted bitmap address
N $E922 LOD
B $E922,1,1 Width (bytes)
B $E923,1,1 Flags
B $E924,1,1 Height (pixels)
W $E925,2,2 [$CA9E] Bitmap address
W $E927,2,2 [$CABE] Pre-shifted bitmap address
N $E929 LOD
B $E929,1,1 Width (bytes)
B $E92A,1,1 Flags
B $E92B,1,1 Height (pixels)
W $E92C,2,2 [$CADE] Bitmap address
W $E92E,2,2 [$CADE] Pre-shifted bitmap address
N $E930 LOD
B $E930,1,1 Width (bytes)
B $E931,1,1 Flags
B $E932,1,1 Height (pixels)
W $E933,2,2 [$CB92] Bitmap address
W $E935,2,2 [$CB92] Pre-shifted bitmap address
N $E937 LOD
B $E937,1,1 Width (bytes)
B $E938,1,1 Flags
B $E939,1,1 Height (pixels)
W $E93A,2,2 [$CBEA] Bitmap address
W $E93C,2,2 [$CBEA] Pre-shifted bitmap address
N $E93E LOD
B $E93E,1,1 Width (bytes)
B $E93F,1,1 Flags
B $E940,1,1 Height (pixels)
W $E941,2,2 [$CBEA] Bitmap address
W $E943,2,2 [$CBEA] Pre-shifted bitmap address
N $E945 LOD
B $E945,1,1 Width (bytes)
B $E946,1,1 Flags
B $E947,1,1 Height (pixels)
W $E948,2,2 [$CC17] Bitmap address
W $E94A,2,2 [$CC37] Pre-shifted bitmap address
N $E94C LOD
B $E94C,1,1 Width (bytes)
B $E94D,1,1 Flags
B $E94E,1,1 Height (pixels)
W $E94F,2,2 [$CC17] Bitmap address
W $E951,2,2 [$CC37] Pre-shifted bitmap address
N $E953 LOD
B $E953,1,1 Width (bytes)
B $E954,1,1 Flags
B $E955,1,1 Height (pixels)
W $E956,2,2 [$CC57] Bitmap address
W $E958,2,2 [$CC57] Pre-shifted bitmap address
N $E95A LOD
B $E95A,1,1 Width (bytes)
B $E95B,1,1 Flags
B $E95C,1,1 Height (pixels)
W $E95D,2,2 [$CD0B] Bitmap address
W $E95F,2,2 [$CD0B] Pre-shifted bitmap address
N $E961 LOD
B $E961,1,1 Width (bytes)
B $E962,1,1 Flags
B $E963,1,1 Height (pixels)
W $E964,2,2 [$CD63] Bitmap address
W $E966,2,2 [$CD63] Pre-shifted bitmap address
N $E968 LOD
B $E968,1,1 Width (bytes)
B $E969,1,1 Flags
B $E96A,1,1 Height (pixels)
W $E96B,2,2 [$CD63] Bitmap address
W $E96D,2,2 [$CD63] Pre-shifted bitmap address
N $E96F LOD
B $E96F,1,1 Width (bytes)
B $E970,1,1 Flags
B $E971,1,1 Height (pixels)
W $E972,2,2 [$CC17] Bitmap address
W $E974,2,2 [$CC37] Pre-shifted bitmap address
N $E976 LOD
B $E976,1,1 Width (bytes)
B $E977,1,1 Flags
B $E978,1,1 Height (pixels)
W $E979,2,2 [$CC17] Bitmap address
W $E97B,2,2 [$CC37] Pre-shifted bitmap address
B $E97D,174,6 Bitmap data 6 bytes x 29
B $EA2B,76,4 Bitmap data 4 bytes x 19
B $EA77,39,3 Bitmap data 3 bytes x 13
B $EA9E,32,2 Bitmap data (masked) 2 bytes x 8
B $EABE,32,2 Pre-shifted bitmap data (masked) 2 bytes x 8
B $EADE,180,6 Bitmap data 6 bytes x 30
B $EB92,88,4 Bitmap data 4 bytes x 22
B $EBEA,45,3 Bitmap data 3 bytes x 15
B $EC17,32,2 Bitmap data (masked) 2 bytes x 8
B $EC37,32,2 Pre-shifted bitmap data (masked) 2 bytes x 8
B $EC57,180,6 Bitmap data 6 bytes x 30
B $ED0B,88,4 Bitmap data 4 bytes x 22
B $ED63,48,3 Bitmap data 3 bytes x 16
B $ED93,3196,8*399,4
N $FA0F LOD
B $FA0F,1,1 Width (bytes)
B $FA10,1,1 Flags
B $FA11,1,1 Height (pixels)
W $FA12,2,2 [$DA39] Bitmap address
W $FA14,2,2 [$DA39] Pre-shifted bitmap address
N $FA16 LOD
B $FA16,1,1 Width (bytes)
B $FA17,1,1 Flags
B $FA18,1,1 Height (pixels)
W $FA19,2,2 [$DA39] Bitmap address
W $FA1B,2,2 [$DA39] Pre-shifted bitmap address
N $FA1D LOD
B $FA1D,1,1 Width (bytes)
B $FA1E,1,1 Flags
B $FA1F,1,1 Height (pixels)
W $FA20,2,2 [$DA7D] Bitmap address
W $FA22,2,2 [$DA7D] Pre-shifted bitmap address
N $FA24 LOD
B $FA24,1,1 Width (bytes)
B $FA25,1,1 Flags
B $FA26,1,1 Height (pixels)
W $FA27,2,2 [$DAA4] Bitmap address
W $FA29,2,2 [$DAA4] Pre-shifted bitmap address
N $FA2B LOD
B $FA2B,1,1 Width (bytes)
B $FA2C,1,1 Flags
B $FA2D,1,1 Height (pixels)
W $FA2E,2,2 [$DAA4] Bitmap address
W $FA30,2,2 [$DAA4] Pre-shifted bitmap address
N $FA32 LOD
B $FA32,1,1 Width (bytes)
B $FA33,1,1 Flags
B $FA34,1,1 Height (pixels)
W $FA35,2,2 [$DAB6] Bitmap address
W $FA37,2,2 [$DAD2] Pre-shifted bitmap address
B $FA39,68,4 Bitmap data 4 bytes x 17
B $FA7D,39,3 Bitmap data 3 bytes x 13
B $FAA4,18,2 Bitmap data 2 bytes x 9
B $FAB6,28,2 Bitmap data (masked) 2 bytes x 7
B $FAD2,28,2 Pre-shifted bitmap data (masked) 2 bytes x 7
B $FAEE,1298,8*162,2
