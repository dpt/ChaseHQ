> $C000 ; ChaseHQ-128K-bank-7.skool
> $C000 ;
> $C000 ; Bank 7 holds the data for stage 5 and the end screen.
> $C000 ;
> $C000
@ $C000 org
b $C000 [Stage 5] Horizon graphic
B $C000,240,8
b $C0F0 [Stage 5] Per-stage data
W $C0F0,2,2 [$C8CE] Address of perp's mugshot attributes
W $C0F2,2,2 [$6400] Address of perp's mugshot bitmap
W $C0F4,2,2 [$D470] Screen attributes used for the ground colour (a pair of matching bytes)
W $C0F6,2,2 [$C239] Address of table of LODs for tumbleweeds, barriers.
W $C0F8,2,2 [$C23B] (points at a handler address)
W $C0FA,2,2 [$C238] Address of right hand graphics entry/entries (-7 bytes)
W $C0FC,2,2 [$C24D] (points at a handler address)
W $C0FE,2,2 [$C26C] (points at a handler address)
W $C100,2,2 [$C269] Address of left hand graphics entry/entries (-7 bytes)
W $C102,2,2 [$C27E] (points at a handler address)
W $C104,2,2 [$C139] Address of Nancy's perp description
W $C106,2,2 [$C1E6] Address of arrest messages
W $C108,2,2 [$6400] Helicopter related 1
W $C10A,2,2 [$640C] Helicopter related 2
w $C10C [Stage 5] Table of addresses of LODs
W $C10C,2,2 [$6400] Address of LOD of stone/dust?
W $C10E,2,2 [$6400] Address of LOD of stone/dust?
W $C110,2,2 [$C8E2] Address of LOD of car (the perp's car)
W $C112,2,2 [$C936] Address of LOD of car (a Lambo in S1)
W $C114,2,2 [$C90C] Address of LOD of car (a truck in S1)
W $C116,2,2 [$C936] Address of LOD of car (a Lambo in S1)
W $C118,2,2 [$C8E2] Address of LOD of car (a generic car in S1)
b $C11A [Stage 5] Per-stage difficulty settings
B $C11A,1,1 How often cars spawn. Lower values spawn cars more often.
B $C11B,1,1 Smash related parameter
B $C11C,1,1 Smash related parameter
w $C11D [Stage 5] Per-stage setup data
W $C11D,2,2 road_pos
W $C11F,2,2 [$C2A0] Address of start stretch, curvature
W $C121,2,2 [$C2C8] Address of start stretch, height
W $C123,2,2 [$C2FA] Address of start stretch, lanes
W $C125,2,2 [$C3D1] Address of start stretch, right-side objects
W $C127,2,2 [$C32B] Address of start stretch, left-side objects
W $C129,2,2 [$C30C] Address of start stretch, hazards
w $C12B [Stage 5] Per-stage attract mode data
W $C12B,2,2 road_pos
W $C12D,2,2 [$C65B] Address of loop section, curvature
W $C12F,2,2 [$C683] Address of loop section, height
W $C131,2,2 [$C6BA] Address of loop section, lanes
W $C133,2,2 [$C7A4] Address of loop section, right-side objects
W $C135,2,2 [$C6ED] Address of loop section, left-side objects
W $C137,2,2 [$C6D0] Address of loop section, hazards
b $C139 [Stage 5] Nancy's perp description
B $C139,1,1 Character identifier (0/1/2/3 = Pilot/Nancy/Raymond/Tony)
W $C13A,2,2 [$C145] Perp description pointer
W $C13C,2,2 [$C16D] Perp description pointer
W $C13E,2,2 [$C196] Perp description pointer
W $C140,2,2 [$C1C0] Perp description pointer
B $C142,1,1 terminator?
B $C143,2,2
T $C145,40,39:n1 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $C16D,41,40:n1 "EMERGENCY HERE. THE EASTERN BLOC SPY FROM"
T $C196,42,41:n1 "WASHINGTON IS FLEEING TOWARDS THE SUBURBS."
T $C1C0,38,37:n1 "THE TARGET VEHICLE IS UNKNOWN... OVER."
b $C1E6 [Stage 5] Arrest messages
B $C1E6,1,1 ?frame delay until first message
B $C1E7,1,1 ?frame delay until next message
B $C1E8,1,1 Flags
B $C1E9,1,1 Attribute
W $C1EA,2,2 Back buffer address
W $C1EC,2,2 Attribute address
T $C1EE,27,26:n1 "OK! YOU ARE UNDER ARREST ON"
B $C209,1,1 ?frame delay until next message
B $C20A,1,1 Flags
B $C20B,1,1 Attribute
W $C20C,2,2 Back buffer address
W $C20E,2,2 Attribute address
T $C210,26,25:n1 "SUSPICION OF ESPIONAGE AND"
B $C22A,1,1 ?frame delay until next message
B $C22B,1,1 Flags
B $C22C,1,1 Attribute
W $C22D,2,2 Back buffer address
W $C22F,2,2 Attribute address
T $C231,6,5:n1 "MURDER"
B $C237,1,1 ?frame delay until next message
B $C238,1,1 Stop
b $C239 [Stage 5] Hittable hazards
B $C239,1,1 ?id
W $C23A,2,2 [$D620] Address of LODs
B $C23C,1,1 ?id
W $C23D,2,2 [$D620] Address of LODs
b $C23F [Stage 5] Graphic definition
N $C23F Definition
B $C23F,1,1 Hit coord max/min (R/L)
B $C240,1,1 Hit coord min/max (R/L)
B $C241,1,1 ?how far to push hero car away if hit
W $C242,2,2 Argument for routine passed in #REGde
W $C244,2,2 Address of routine draw_tunnel_light_right
N $C246 Definition
B $C246,1,1 Hit coord max/min (R/L)
B $C247,1,1 Hit coord min/max (R/L)
B $C248,1,1 ?how far to push hero car away if hit
W $C249,2,2 Argument for routine passed in #REGde
W $C24B,2,2 Address of routine draw_stretchy_object_right
N $C24D Definition
B $C24D,1,1 Hit coord max/min (R/L)
B $C24E,1,1 Hit coord min/max (R/L)
B $C24F,1,1 ?how far to push hero car away if hit
W $C250,2,2 Argument for routine passed in #REGde
W $C252,2,2 Address of routine draw_stretchy_object_right
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
b $C270 [Stage 5] Graphic definition
N $C270 Definition
B $C270,1,1 Hit coord max/min (R/L)
B $C271,1,1 Hit coord min/max (R/L)
B $C272,1,1 ?how far to push hero car away if hit
W $C273,2,2 Argument for routine passed in #REGde
W $C275,2,2 Address of routine draw_tunnel_light_left
N $C277 Definition
B $C277,1,1 Hit coord max/min (R/L)
B $C278,1,1 Hit coord min/max (R/L)
B $C279,1,1 ?how far to push hero car away if hit
W $C27A,2,2 Argument for routine passed in #REGde
W $C27C,2,2 Address of routine draw_overhead
N $C27E Definition
B $C27E,1,1 Hit coord max/min (R/L)
B $C27F,1,1 Hit coord min/max (R/L)
B $C280,1,1 ?how far to push hero car away if hit
W $C281,2,2 Argument for routine passed in #REGde
W $C283,2,2 Address of routine draw_stretchy_object_left
N $C285 Definition
B $C285,1,1 Hit coord max/min (R/L)
B $C286,1,1 Hit coord min/max (R/L)
B $C287,1,1 ?how far to push hero car away if hit
W $C288,2,2 Argument for routine passed in #REGde
W $C28A,2,2 Address of routine draw_stretchy_object_left
N $C28C Definition
B $C28C,1,1 Hit coord max/min (R/L)
B $C28D,1,1 Hit coord min/max (R/L)
B $C28E,1,1 ?how far to push hero car away if hit
W $C28F,2,2 Argument for routine passed in #REGde
W $C291,2,2 Address of routine draw_stretchy_object_left
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
b $C2A1 [Stage 5] Map curvature data
B $C2A1,1,1 Curve Straight for 50 units
B $C2A2,3,3
B $C2A5,1,1 Curve Left Hard for 21 units
B $C2A6,1,1
B $C2A7,1,1 Curve Left for 14 units
B $C2A8,1,1 Curve Straight for 35 units
B $C2A9,2,2
B $C2AB,1,1 Curve Right Hard for 21 units
B $C2AC,1,1
B $C2AD,1,1 Curve Right Very Hard for 15 units
B $C2AE,1,1 Curve Right Hard for 25 units
B $C2AF,1,1
B $C2B0,1,1 Curve Right for 10 units
B $C2B1,1,1 Curve Straight for 79 units
B $C2B2,5,5
B $C2B7,1,1 Curve Left Hard for 31 units
B $C2B8,2,2
B $C2BA,1,1 Curve Left Very Hard for 20 units
B $C2BB,1,1
B $C2BC,1,1 Curve Left Hard for 33 units
B $C2BD,2,2
B $C2BF,1,1 Curve Left for 7 units
B $C2C0,1,1 Curve Straight for 39 units
B $C2C1,2,2
B $C2C3,1,1 <Esc> Split
B $C2C4,1,1
W $C2C5,2,2 [$C513] Left target
W $C2C7,2,2 [$C455] Right target
b $C2C9 [Stage 5] Map height data
B $C2C9,1,1 Level Road for 40 units
B $C2CA,2,2
B $C2CC,1,1 Going Up 3 for 2 units
B $C2CD,1,1 Going Up 5 for 2 units
B $C2CE,1,1 Going Up 7 for 1 units
B $C2CF,1,1 Going Up 5 for 1 units
B $C2D0,1,1 Going Up 3 for 5 units
B $C2D1,1,1 Level Road for 20 units
B $C2D2,1,1
B $C2D3,1,1 Going Down 3 for 1 units
B $C2D4,1,1 Going Down 5 for 3 units
B $C2D5,1,1 Going Down 3 for 4 units
B $C2D6,1,1 Going Down 7 for 1 units
B $C2D7,1,1 Going Down 5 for 1 units
B $C2D8,1,1 Going Down 3 for 1 units
B $C2D9,1,1 Level Road for 19 units
B $C2DA,1,1
B $C2DB,1,1 Going Up 3 for 2 units
B $C2DC,1,1 Going Up 5 for 1 units
B $C2DD,1,1 Going Up 7 for 3 units
B $C2DE,1,1 Going Down 7 for 3 units
B $C2DF,1,1 Going Down 5 for 1 units
B $C2E0,1,1 Going Down 3 for 1 units
B $C2E1,1,1 Level Road for 288 units
B $C2E2,19,8*2,3
B $C2F5,1,1 <Esc> Split
B $C2F6,1,1
W $C2F7,2,2 [$C526] Left target
W $C2F9,2,2 [$C468] Right target
b $C2FB [Stage 5] Map lanes data
B $C2FB,1,1 2 Lanes R              [||] {03} for 36 units
B $C2FC,1,1
B $C2FD,1,1 2-3 Widening R        [\||] {1F} for 2 units
B $C2FE,1,1
B $C2FF,1,1 3 Lanes R             [|||] {82} for 52 units
B $C300,1,1
B $C301,1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C302,1,1
B $C303,1,1 4 Lanes              [||||] {00} for 308 units
B $C304,3,3
B $C307,1,1 <Esc> Split
B $C308,1,1
W $C309,2,2 [$C53E] Left target
W $C30B,2,2 [$C47A] Right target
b $C30D [Stage 5] Map hazards data
B $C30D,1,1 Wait for 20 units
B $C30E,1,1 Start Spawning Barriers Right
B $C30F,1,1
B $C310,1,1 Wait for 2 units
B $C311,1,1 Stop Spawning Barriers 3?
B $C312,1,1
B $C313,1,1 Wait for 45 units
B $C314,1,1 Start Spawning Barriers Left
B $C315,1,1
B $C316,1,1 Wait for 2 units
B $C317,1,1 Stop Spawning Barriers 3?
B $C318,1,1
B $C319,1,1 Wait for 43 units
B $C31A,1,1 Start Spawning Two Barriers
B $C31B,1,1
B $C31C,1,1 Wait for 4 units
B $C31D,1,1 Stop Spawning Barriers 3?
B $C31E,1,1
B $C31F,1,1 Wait for 76 units
B $C320,1,1 Disable Car Spawning
B $C321,1,1
B $C322,1,1 Wait for 6 units
B $C323,1,1 Set Floating Arrow to Right
B $C324,1,1
B $C325,1,1 Wait for 2 units
B $C326,1,1 <Esc> Split
B $C327,1,1
W $C328,2,2 [$C552] Left target
W $C32A,2,2 [$C492] Right target
b $C32C [Stage 5] Map left object data
B $C32C,1,1 CACTUS for 1 units
B $C32D,1,1 (nothing) for 1 units
B $C32E,1,1 CACTUS for 1 units
B $C32F,1,1 (nothing) for 1 units
B $C330,1,1 CACTUS for 1 units
B $C331,1,1 (nothing) for 1 units
B $C332,1,1 CACTUS for 1 units
B $C333,1,1 (nothing) for 1 units
B $C334,1,1 CACTUS for 1 units
B $C335,1,1 (nothing) for 1 units
B $C336,1,1 CACTUS for 1 units
B $C337,1,1 (nothing) for 1 units
B $C338,1,1 CACTUS for 1 units
B $C339,1,1 (nothing) for 1 units
B $C33A,1,1 CACTUS for 1 units
B $C33B,1,1 (nothing) for 1 units
B $C33C,1,1 CACTUS for 1 units
B $C33D,1,1 (nothing) for 3 units
B $C33E,1,1 CACTUS for 1 units
B $C33F,1,1 TELEGRAPH_POLE for 1 units
B $C340,1,1 CACTUS for 1 units
B $C341,1,1 TELEGRAPH_POLE for 1 units
B $C342,1,1 CACTUS for 1 units
B $C343,1,1 TELEGRAPH_POLE for 1 units
B $C344,1,1 CACTUS for 1 units
B $C345,1,1 (nothing) for 3 units
B $C346,1,1 TELEGRAPH_POLE for 1 units
B $C347,1,1 (nothing) for 1 units
B $C348,1,1 TELEGRAPH_POLE for 1 units
B $C349,1,1 (nothing) for 1 units
B $C34A,1,1 TELEGRAPH_POLE for 1 units
B $C34B,1,1 (nothing) for 1 units
B $C34C,1,1 TELEGRAPH_POLE for 1 units
B $C34D,1,1 (nothing) for 1 units
B $C34E,1,1 TELEGRAPH_POLE for 1 units
B $C34F,1,1 (nothing) for 1 units
B $C350,1,1 TELEGRAPH_POLE for 1 units
B $C351,1,1 (nothing) for 1 units
B $C352,1,1 TELEGRAPH_POLE for 1 units
B $C353,1,1 (nothing) for 1 units
B $C354,1,1 TELEGRAPH_POLE for 1 units
B $C355,1,1 (nothing) for 1 units
B $C356,1,1 TELEGRAPH_POLE for 1 units
B $C357,1,1 (nothing) for 1 units
B $C358,1,1 CACTUS for 1 units
B $C359,1,1 (nothing) for 1 units
B $C35A,1,1 CACTUS for 1 units
B $C35B,1,1 (nothing) for 1 units
B $C35C,1,1 CACTUS for 1 units
B $C35D,1,1 (nothing) for 1 units
B $C35E,1,1 CACTUS for 1 units
B $C35F,1,1 (nothing) for 3 units
B $C360,1,1 DOUBLE_STREET_LAMP for 1 units
B $C361,1,1 (nothing) for 1 units
B $C362,1,1 DOUBLE_STREET_LAMP for 1 units
B $C363,1,1 (nothing) for 1 units
B $C364,1,1 DOUBLE_STREET_LAMP for 1 units
B $C365,1,1 (nothing) for 1 units
B $C366,1,1 DOUBLE_STREET_LAMP for 1 units
B $C367,1,1 (nothing) for 1 units
B $C368,1,1 DOUBLE_STREET_LAMP for 1 units
B $C369,1,1 (nothing) for 1 units
B $C36A,1,1 DOUBLE_STREET_LAMP for 1 units
B $C36B,1,1 (nothing) for 1 units
B $C36C,1,1 DOUBLE_STREET_LAMP for 1 units
B $C36D,1,1 (nothing) for 1 units
B $C36E,1,1 DOUBLE_STREET_LAMP for 1 units
B $C36F,1,1 (nothing) for 1 units
B $C370,1,1 DOUBLE_STREET_LAMP for 1 units
B $C371,1,1 (nothing) for 1 units
B $C372,1,1 DOUBLE_STREET_LAMP for 1 units
B $C373,1,1 (nothing) for 1 units
B $C374,1,1 DOUBLE_STREET_LAMP for 1 units
B $C375,1,1 (nothing) for 1 units
B $C376,1,1 DOUBLE_STREET_LAMP for 1 units
B $C377,1,1 (nothing) for 1 units
B $C378,1,1 DOUBLE_STREET_LAMP for 1 units
B $C379,1,1 (nothing) for 1 units
B $C37A,1,1 DOUBLE_STREET_LAMP for 1 units
B $C37B,1,1 (nothing) for 1 units
B $C37C,1,1 DOUBLE_STREET_LAMP for 1 units
B $C37D,1,1 (nothing) for 1 units
B $C37E,1,1 DOUBLE_STREET_LAMP for 1 units
B $C37F,1,1 (nothing) for 1 units
B $C380,1,1 DOUBLE_STREET_LAMP for 1 units
B $C381,1,1 (nothing) for 1 units
B $C382,1,1 DOUBLE_STREET_LAMP for 1 units
B $C383,1,1 (nothing) for 1 units
B $C384,1,1 DOUBLE_STREET_LAMP for 1 units
B $C385,1,1 (nothing) for 1 units
B $C386,1,1 DOUBLE_STREET_LAMP for 1 units
B $C387,1,1 (nothing) for 3 units
B $C388,1,1 OVERHEAD_BRIDGE for 1 units
B $C389,1,1 (nothing) for 1 units
B $C38A,1,1 OVERHEAD_BRIDGE for 1 units
B $C38B,1,1 (nothing) for 5 units
B $C38C,1,1 OVERHEAD_BRIDGE for 1 units
B $C38D,1,1 (nothing) for 1 units
B $C38E,1,1 OVERHEAD_BRIDGE for 1 units
B $C38F,1,1 (nothing) for 5 units
B $C390,1,1 OVERHEAD_BRIDGE for 1 units
B $C391,1,1 (nothing) for 1 units
B $C392,1,1 OVERHEAD_BRIDGE for 1 units
B $C393,1,1 (nothing) for 5 units
B $C394,1,1 OVERHEAD_BRIDGE for 1 units
B $C395,1,1 (nothing) for 1 units
B $C396,1,1 OVERHEAD_BRIDGE for 1 units
B $C397,1,1 (nothing) for 9 units
B $C398,1,1 CACTUS for 1 units
B $C399,1,1 (nothing) for 1 units
B $C39A,1,1 CACTUS for 1 units
B $C39B,1,1 (nothing) for 1 units
B $C39C,1,1 CACTUS for 1 units
B $C39D,1,1 (nothing) for 1 units
B $C39E,1,1 CACTUS for 1 units
B $C39F,1,1 (nothing) for 1 units
B $C3A0,1,1 CACTUS for 1 units
B $C3A1,1,1 (nothing) for 1 units
B $C3A2,1,1 CACTUS for 1 units
B $C3A3,1,1 (nothing) for 1 units
B $C3A4,1,1 CACTUS for 1 units
B $C3A5,1,1 (nothing) for 1 units
B $C3A6,1,1 CACTUS for 1 units
B $C3A7,1,1 (nothing) for 1 units
B $C3A8,1,1 CACTUS for 1 units
B $C3A9,1,1 (nothing) for 1 units
B $C3AA,1,1 CACTUS for 1 units
B $C3AB,1,1 (nothing) for 1 units
B $C3AC,1,1 CACTUS for 1 units
B $C3AD,1,1 (nothing) for 3 units
B $C3AE,1,1 CACTUS for 1 units
B $C3AF,1,1 (nothing) for 3 units
B $C3B0,1,1 CACTUS for 1 units
B $C3B1,1,1 (nothing) for 1 units
B $C3B2,1,1 CACTUS for 1 units
B $C3B3,1,1 (nothing) for 3 units
B $C3B4,1,1 CACTUS for 1 units
B $C3B5,1,1 (nothing) for 3 units
B $C3B6,1,1 CACTUS for 1 units
B $C3B7,1,1 (nothing) for 3 units
B $C3B8,1,1 TELEGRAPH_POLE for 1 units
B $C3B9,1,1 (nothing) for 1 units
B $C3BA,1,1 TELEGRAPH_POLE for 1 units
B $C3BB,1,1 (nothing) for 1 units
B $C3BC,1,1 TELEGRAPH_POLE for 1 units
B $C3BD,1,1 (nothing) for 1 units
B $C3BE,1,1 TELEGRAPH_POLE for 1 units
B $C3BF,1,1 (nothing) for 1 units
B $C3C0,1,1 TELEGRAPH_POLE for 1 units
B $C3C1,1,1 (nothing) for 1 units
B $C3C2,1,1 TELEGRAPH_POLE for 1 units
B $C3C3,1,1 (nothing) for 1 units
B $C3C4,1,1 TELEGRAPH_POLE for 1 units
B $C3C5,1,1 (nothing) for 1 units
B $C3C6,1,1 TELEGRAPH_POLE for 1 units
B $C3C7,1,1 (nothing) for 3 units
B $C3C8,1,1 TELEGRAPH_POLE for 1 units
B $C3C9,1,1 (nothing) for 1 units
B $C3CA,1,1 TELEGRAPH_POLE for 1 units
B $C3CB,1,1 (nothing) for 1 units
B $C3CC,1,1 <Esc> Split
B $C3CD,1,1
W $C3CE,2,2 [$C560] Left target
W $C3D0,2,2 [$C49A] Right target
b $C3D2 [Stage 5] Map right object data
B $C3D2,1,1 (nothing) for 1 units
B $C3D3,1,1 DOUBLE_STREET_LAMP for 1 units
B $C3D4,1,1 (nothing) for 3 units
B $C3D5,1,1 DOUBLE_STREET_LAMP for 1 units
B $C3D6,1,1 (nothing) for 1 units
B $C3D7,1,1 DOUBLE_STREET_LAMP for 1 units
B $C3D8,1,1 (nothing) for 4 units
B $C3D9,1,1 CACTUS for 1 units
B $C3DA,1,1 (nothing) for 1 units
B $C3DB,1,1 CACTUS for 1 units
B $C3DC,1,1 (nothing) for 1 units
B $C3DD,1,1 CACTUS for 1 units
B $C3DE,1,1 (nothing) for 1 units
B $C3DF,1,1 CACTUS for 1 units
B $C3E0,1,1 (nothing) for 13 units
B $C3E1,1,1 CACTUS for 1 units
B $C3E2,1,1 (nothing) for 1 units
B $C3E3,1,1 CACTUS for 1 units
B $C3E4,1,1 (nothing) for 1 units
B $C3E5,1,1 CACTUS for 1 units
B $C3E6,1,1 (nothing) for 3 units
B $C3E7,1,1 CACTUS for 1 units
B $C3E8,1,1 (nothing) for 3 units
B $C3E9,1,1 CACTUS for 1 units
B $C3EA,1,1 (nothing) for 1 units
B $C3EB,1,1 CACTUS for 1 units
B $C3EC,1,1 (nothing) for 1 units
B $C3ED,1,1 CACTUS for 1 units
B $C3EE,1,1 (nothing) for 3 units
B $C3EF,1,1 CACTUS for 1 units
B $C3F0,1,1 (nothing) for 3 units
B $C3F1,1,1 TELEGRAPH_POLE for 1 units
B $C3F2,1,1 (nothing) for 1 units
B $C3F3,1,1 TELEGRAPH_POLE for 1 units
B $C3F4,1,1 (nothing) for 3 units
B $C3F5,1,1 TELEGRAPH_POLE for 1 units
B $C3F6,1,1 (nothing) for 3 units
B $C3F7,1,1 TELEGRAPH_POLE for 1 units
B $C3F8,1,1 (nothing) for 3 units
B $C3F9,1,1 TELEGRAPH_POLE for 1 units
B $C3FA,1,1 (nothing) for 3 units
B $C3FB,1,1 TELEGRAPH_POLE for 1 units
B $C3FC,1,1 (nothing) for 3 units
B $C3FD,1,1 TELEGRAPH_POLE for 1 units
B $C3FE,1,1 (nothing) for 3 units
B $C3FF,1,1 TELEGRAPH_POLE for 1 units
B $C400,1,1 (nothing) for 3 units
B $C401,1,1 TELEGRAPH_POLE for 1 units
B $C402,1,1 (nothing) for 1 units
B $C403,1,1 TELEGRAPH_POLE for 1 units
B $C404,1,1 (nothing) for 11 units
B $C405,1,1 OVERHEAD_BRIDGE for 1 units
B $C406,1,1 (nothing) for 1 units
B $C407,1,1 OVERHEAD_BRIDGE for 1 units
B $C408,1,1 (nothing) for 5 units
B $C409,1,1 OVERHEAD_BRIDGE for 1 units
B $C40A,1,1 (nothing) for 1 units
B $C40B,1,1 OVERHEAD_BRIDGE for 1 units
B $C40C,1,1 (nothing) for 5 units
B $C40D,1,1 OVERHEAD_BRIDGE for 1 units
B $C40E,1,1 (nothing) for 1 units
B $C40F,1,1 OVERHEAD_BRIDGE for 1 units
B $C410,1,1 (nothing) for 5 units
B $C411,1,1 OVERHEAD_BRIDGE for 1 units
B $C412,1,1 (nothing) for 1 units
B $C413,1,1 OVERHEAD_BRIDGE for 1 units
B $C414,1,1 (nothing) for 5 units
B $C415,1,1 TELEGRAPH_POLE for 1 units
B $C416,1,1 (nothing) for 1 units
B $C417,1,1 TELEGRAPH_POLE for 1 units
B $C418,1,1 (nothing) for 1 units
B $C419,1,1 TELEGRAPH_POLE for 1 units
B $C41A,1,1 (nothing) for 3 units
B $C41B,1,1 TELEGRAPH_POLE for 1 units
B $C41C,1,1 (nothing) for 1 units
B $C41D,1,1 TELEGRAPH_POLE for 1 units
B $C41E,1,1 (nothing) for 3 units
B $C41F,1,1 TELEGRAPH_POLE for 1 units
B $C420,1,1 (nothing) for 1 units
B $C421,1,1 TELEGRAPH_POLE for 1 units
B $C422,1,1 (nothing) for 1 units
B $C423,1,1 TELEGRAPH_POLE for 1 units
B $C424,1,1 (nothing) for 1 units
B $C425,1,1 TELEGRAPH_POLE for 1 units
B $C426,1,1 (nothing) for 1 units
B $C427,1,1 TELEGRAPH_POLE for 1 units
B $C428,1,1 (nothing) for 1 units
B $C429,1,1 TELEGRAPH_POLE for 1 units
B $C42A,1,1 (nothing) for 1 units
B $C42B,1,1 TELEGRAPH_POLE for 1 units
B $C42C,1,1 (nothing) for 1 units
B $C42D,1,1 CACTUS for 1 units
B $C42E,1,1 (nothing) for 1 units
B $C42F,1,1 CACTUS for 1 units
B $C430,1,1 (nothing) for 1 units
B $C431,1,1 CACTUS for 1 units
B $C432,1,1 (nothing) for 1 units
B $C433,1,1 CACTUS for 1 units
B $C434,1,1 (nothing) for 1 units
B $C435,1,1 CACTUS for 1 units
B $C436,1,1 (nothing) for 1 units
B $C437,1,1 CACTUS for 1 units
B $C438,1,1 (nothing) for 1 units
B $C439,1,1 CACTUS for 1 units
B $C43A,1,1 (nothing) for 1 units
B $C43B,1,1 CACTUS for 1 units
B $C43C,1,1 (nothing) for 3 units
B $C43D,1,1 TELEGRAPH_POLE for 1 units
B $C43E,1,1 CACTUS for 1 units
B $C43F,1,1 TELEGRAPH_POLE for 1 units
B $C440,1,1 CACTUS for 1 units
B $C441,1,1 TELEGRAPH_POLE for 1 units
B $C442,1,1 (nothing) for 1 units
B $C443,1,1 TELEGRAPH_POLE for 1 units
B $C444,1,1 (nothing) for 1 units
B $C445,1,1 CACTUS for 1 units
B $C446,1,1 (nothing) for 1 units
B $C447,1,1 CACTUS for 1 units
B $C448,1,1 (nothing) for 1 units
B $C449,1,1 CACTUS for 1 units
B $C44A,1,1 (nothing) for 3 units
B $C44B,1,1 CACTUS for 1 units
B $C44C,1,1 (nothing) for 1 units
B $C44D,1,1 CACTUS for 1 units
B $C44E,1,1 (nothing) for 3 units
B $C44F,1,1 <Esc> Split
B $C450,1,1
W $C451,2,2 [$C5A3] Left target
W $C453,2,2 [$C4DD] Right target
b $C455 [Stage 5] Map curvature data
B $C455,1,1 Curve Straight for 40 units
B $C456,2,2
B $C458,1,1 Curve Left Hard for 41 units
B $C459,2,2
B $C45B,1,1 Curve Right Hard for 40 units
B $C45C,2,2
B $C45E,1,1 Curve Left Hard for 60 units
B $C45F,3,3
B $C462,1,1 Curve Straight for 17 units
B $C463,1,1
B $C464,1,1 <Esc> Jump
B $C465,1,1
W $C466,2,2 [$C5E3] Target
b $C468 [Stage 5] Map height data
B $C468,1,1 Level Road for 198 units
B $C469,13,8,5
B $C476,1,1 <Esc> Jump
B $C477,1,1
W $C478,2,2 [$C5F8] Target
b $C47A [Stage 5] Map lanes data
B $C47A,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C47B,1,1
B $C47C,1,1 Tunnel start                {45} for 20 units
B $C47D,1,1
B $C47E,1,1 Tunnel cont/end?            {59} for 2 units
B $C47F,1,1
B $C480,1,1 3 Lanes L            [|||]  {81} for 4 units
B $C481,1,1
B $C482,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C483,1,1
B $C484,1,1 4 Lanes              [||||] {00} for 6 units
B $C485,1,1
B $C486,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C487,1,1
B $C488,1,1 3 Lanes L            [|||]  {81} for 74 units
B $C489,1,1
B $C48A,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C48B,1,1
B $C48C,1,1 4 Lanes              [||||] {00} for 84 units
B $C48D,1,1
B $C48E,1,1 <Esc> Jump
B $C48F,1,1
W $C490,2,2 [$C614] Target
b $C492 [Stage 5] Map hazards data
B $C492,1,1 Wait for 10 units
B $C493,1,1 Enable Car Spawning
B $C494,1,1
B $C495,1,1 Wait for 89 units
B $C496,1,1 <Esc> Jump
B $C497,1,1
W $C498,2,2 [$C628] Target
b $C49A [Stage 5] Map left object data
B $C49A,1,1 (nothing) for 2 units
B $C49B,1,1 TUNNEL_LIGHT for 10 units
B $C49C,1,1 (nothing) for 2 units
B $C49D,1,1 CACTUS for 1 units
B $C49E,1,1 (nothing) for 1 units
B $C49F,1,1 CACTUS for 1 units
B $C4A0,1,1 (nothing) for 1 units
B $C4A1,1,1 CACTUS for 1 units
B $C4A2,1,1 (nothing) for 1 units
B $C4A3,1,1 CACTUS for 1 units
B $C4A4,1,1 (nothing) for 1 units
B $C4A5,1,1 CACTUS for 1 units
B $C4A6,1,1 (nothing) for 1 units
B $C4A7,1,1 CACTUS for 1 units
B $C4A8,1,1 (nothing) for 1 units
B $C4A9,1,1 CACTUS for 1 units
B $C4AA,1,1 (nothing) for 1 units
B $C4AB,1,1 CACTUS for 1 units
B $C4AC,1,1 (nothing) for 3 units
B $C4AD,1,1 CACTUS for 1 units
B $C4AE,1,1 (nothing) for 3 units
B $C4AF,1,1 CACTUS for 1 units
B $C4B0,1,1 (nothing) for 3 units
B $C4B1,1,1 TELEGRAPH_POLE for 1 units
B $C4B2,1,1 (nothing) for 1 units
B $C4B3,1,1 TELEGRAPH_POLE for 1 units
B $C4B4,1,1 (nothing) for 1 units
B $C4B5,1,1 TELEGRAPH_POLE for 1 units
B $C4B6,1,1 (nothing) for 1 units
B $C4B7,1,1 TELEGRAPH_POLE for 1 units
B $C4B8,1,1 (nothing) for 1 units
B $C4B9,1,1 TELEGRAPH_POLE for 1 units
B $C4BA,1,1 (nothing) for 1 units
B $C4BB,1,1 TELEGRAPH_POLE for 1 units
B $C4BC,1,1 (nothing) for 1 units
B $C4BD,1,1 TELEGRAPH_POLE for 1 units
B $C4BE,1,1 (nothing) for 1 units
B $C4BF,1,1 TELEGRAPH_POLE for 1 units
B $C4C0,1,1 (nothing) for 1 units
B $C4C1,1,1 TELEGRAPH_POLE for 1 units
B $C4C2,1,1 (nothing) for 1 units
B $C4C3,1,1 TELEGRAPH_POLE for 1 units
B $C4C4,1,1 (nothing) for 1 units
B $C4C5,1,1 TELEGRAPH_POLE for 1 units
B $C4C6,1,1 (nothing) for 9 units
B $C4C7,1,1 CACTUS for 1 units
B $C4C8,1,1 (nothing) for 1 units
B $C4C9,1,1 CACTUS for 1 units
B $C4CA,1,1 (nothing) for 1 units
B $C4CB,1,1 CACTUS for 1 units
B $C4CC,1,1 (nothing) for 1 units
B $C4CD,1,1 CACTUS for 1 units
B $C4CE,1,1 (nothing) for 5 units
B $C4CF,1,1 CACTUS for 1 units
B $C4D0,1,1 (nothing) for 1 units
B $C4D1,1,1 CACTUS for 1 units
B $C4D2,1,1 (nothing) for 5 units
B $C4D3,1,1 CACTUS for 1 units
B $C4D4,1,1 (nothing) for 1 units
B $C4D5,1,1 CACTUS for 1 units
B $C4D6,1,1 (nothing) for 3 units
B $C4D7,1,1 CACTUS for 1 units
B $C4D8,1,1 (nothing) for 2 units
B $C4D9,1,1 <Esc> Jump
B $C4DA,1,1
W $C4DB,2,2 [$C62D] Target
b $C4DD [Stage 5] Map right object data
B $C4DD,1,1 (nothing) for 2 units
B $C4DE,1,1 TUNNEL_LIGHT for 10 units
B $C4DF,1,1 (nothing) for 18 units
B $C4E0,1,1
B $C4E1,1,1 CACTUS for 1 units
B $C4E2,1,1 (nothing) for 1 units
B $C4E3,1,1 CACTUS for 1 units
B $C4E4,1,1 (nothing) for 1 units
B $C4E5,1,1 CACTUS for 1 units
B $C4E6,1,1 (nothing) for 1 units
B $C4E7,1,1 CACTUS for 1 units
B $C4E8,1,1 (nothing) for 1 units
B $C4E9,1,1 CACTUS for 1 units
B $C4EA,1,1 (nothing) for 7 units
B $C4EB,1,1 CACTUS for 1 units
B $C4EC,1,1 (nothing) for 3 units
B $C4ED,1,1 CACTUS for 1 units
B $C4EE,1,1 (nothing) for 1 units
B $C4EF,1,1 CACTUS for 1 units
B $C4F0,1,1 (nothing) for 11 units
B $C4F1,1,1 TELEGRAPH_POLE for 1 units
B $C4F2,1,1 (nothing) for 1 units
B $C4F3,1,1 TELEGRAPH_POLE for 1 units
B $C4F4,1,1 (nothing) for 1 units
B $C4F5,1,1 TELEGRAPH_POLE for 1 units
B $C4F6,1,1 (nothing) for 1 units
B $C4F7,1,1 TELEGRAPH_POLE for 1 units
B $C4F8,1,1 (nothing) for 1 units
B $C4F9,1,1 TELEGRAPH_POLE for 1 units
B $C4FA,1,1 (nothing) for 1 units
B $C4FB,1,1 TELEGRAPH_POLE for 1 units
B $C4FC,1,1 (nothing) for 1 units
B $C4FD,1,1 TELEGRAPH_POLE for 1 units
B $C4FE,1,1 (nothing) for 1 units
B $C4FF,1,1 TELEGRAPH_POLE for 1 units
B $C500,1,1 (nothing) for 1 units
B $C501,1,1 TELEGRAPH_POLE for 1 units
B $C502,1,1 (nothing) for 1 units
B $C503,1,1 TELEGRAPH_POLE for 1 units
B $C504,1,1 (nothing) for 1 units
B $C505,1,1 TELEGRAPH_POLE for 1 units
B $C506,1,1 (nothing) for 1 units
B $C507,1,1 TELEGRAPH_POLE for 1 units
B $C508,1,1 (nothing) for 1 units
B $C509,1,1 TELEGRAPH_POLE for 1 units
B $C50A,1,1 (nothing) for 3 units
B $C50B,1,1 CACTUS for 1 units
B $C50C,1,1 (nothing) for 3 units
B $C50D,1,1 CACTUS for 1 units
B $C50E,1,1 (nothing) for 2 units
B $C50F,1,1 <Esc> Jump
B $C510,1,1
W $C511,2,2 [$C651] Target
b $C513 [Stage 5] Map curvature data
B $C513,1,1 Curve Straight for 12 units
B $C514,1,1 Curve Left Hard for 43 units
B $C515,2,2
B $C517,1,1 Curve Straight for 25 units
B $C518,1,1
B $C519,1,1 Curve Right Hard for 68 units
B $C51A,4,4
B $C51E,1,1 Curve Straight for 50 units
B $C51F,3,3
B $C522,1,1 <Esc> Jump
B $C523,1,1
W $C524,2,2 [$C5E3] Target
b $C526 [Stage 5] Map height data
B $C526,1,1 Level Road for 168 units
B $C527,11,8,3
B $C532,1,1 Going Up 3 for 1 units
B $C533,1,1 Going Up 5 for 3 units
B $C534,1,1 Going Up 3 for 1 units
B $C535,1,1 Going Down 3 for 1 units
B $C536,1,1 Going Down 5 for 3 units
B $C537,1,1 Going Down 3 for 1 units
B $C538,1,1 Level Road for 20 units
B $C539,1,1
B $C53A,1,1 <Esc> Jump
B $C53B,1,1
W $C53C,2,2 [$C5F8] Target
b $C53E [Stage 5] Map lanes data
B $C53E,1,1 4 Lanes              [||||] {00} for 40 units
B $C53F,1,1
B $C540,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C541,1,1
B $C542,1,1 3 Lanes L            [|||]  {81} for 124 units
B $C543,1,1
B $C544,1,1 Tunnel start                {45} for 20 units
B $C545,1,1
B $C546,1,1 Tunnel cont/end?            {59} for 2 units
B $C547,1,1
B $C548,1,1 3 Lanes L            [|||]  {81} for 2 units
B $C549,1,1
B $C54A,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C54B,1,1
B $C54C,1,1 4 Lanes              [||||] {00} for 6 units
B $C54D,1,1
B $C54E,1,1 <Esc> Jump
B $C54F,1,1
W $C550,2,2 [$C614] Target
b $C552 [Stage 5] Map hazards data
B $C552,1,1 Wait for 10 units
B $C553,1,1 Enable Car Spawning
B $C554,1,1
B $C555,1,1 Wait for 8 units
B $C556,1,1 Start Spawning Barriers Right
B $C557,1,1
B $C558,1,1 Wait for 2 units
B $C559,1,1 Stop Spawning Barriers 3?
B $C55A,1,1
B $C55B,1,1 Wait for 79 units
B $C55C,1,1 <Esc> Jump
B $C55D,1,1
W $C55E,2,2 [$C628] Target
b $C560 [Stage 5] Map left object data
B $C560,1,1 (nothing) for 2 units
B $C561,1,1 OVERHEAD_BRIDGE for 1 units
B $C562,1,1 (nothing) for 1 units
B $C563,1,1 OVERHEAD_BRIDGE for 1 units
B $C564,1,1 (nothing) for 1 units
B $C565,1,1 OVERHEAD_BRIDGE for 1 units
B $C566,1,1 (nothing) for 1 units
B $C567,1,1 OVERHEAD_BRIDGE for 1 units
B $C568,1,1 (nothing) for 1 units
B $C569,1,1 OVERHEAD_BRIDGE for 1 units
B $C56A,1,1 (nothing) for 1 units
B $C56B,1,1 OVERHEAD_BRIDGE for 1 units
B $C56C,1,1 (nothing) for 1 units
B $C56D,1,1 OVERHEAD_BRIDGE for 1 units
B $C56E,1,1 (nothing) for 1 units
B $C56F,1,1 OVERHEAD_BRIDGE for 1 units
B $C570,1,1 (nothing) for 1 units
B $C571,1,1 OVERHEAD_BRIDGE for 1 units
B $C572,1,1 (nothing) for 1 units
B $C573,1,1 OVERHEAD_BRIDGE for 1 units
B $C574,1,1 (nothing) for 1 units
B $C575,1,1 OVERHEAD_BRIDGE for 1 units
B $C576,1,1 (nothing) for 1 units
B $C577,1,1 OVERHEAD_BRIDGE for 1 units
B $C578,1,1 (nothing) for 21 units
B $C579,1,1
B $C57A,1,1 OVERHEAD_BRIDGE for 1 units
B $C57B,1,1 (nothing) for 1 units
B $C57C,1,1 OVERHEAD_BRIDGE for 1 units
B $C57D,1,1 (nothing) for 1 units
B $C57E,1,1 OVERHEAD_BRIDGE for 1 units
B $C57F,1,1 (nothing) for 1 units
B $C580,1,1 OVERHEAD_BRIDGE for 1 units
B $C581,1,1 (nothing) for 1 units
B $C582,1,1 OVERHEAD_BRIDGE for 1 units
B $C583,1,1 (nothing) for 1 units
B $C584,1,1 OVERHEAD_BRIDGE for 1 units
B $C585,1,1 (nothing) for 1 units
B $C586,1,1 OVERHEAD_BRIDGE for 1 units
B $C587,1,1 (nothing) for 1 units
B $C588,1,1 OVERHEAD_BRIDGE for 1 units
B $C589,1,1 (nothing) for 1 units
B $C58A,1,1 OVERHEAD_BRIDGE for 1 units
B $C58B,1,1 (nothing) for 1 units
B $C58C,1,1 OVERHEAD_BRIDGE for 1 units
B $C58D,1,1 (nothing) for 1 units
B $C58E,1,1 OVERHEAD_BRIDGE for 1 units
B $C58F,1,1 (nothing) for 1 units
B $C590,1,1 OVERHEAD_BRIDGE for 1 units
B $C591,1,1 (nothing) for 1 units
B $C592,1,1 OVERHEAD_BRIDGE for 1 units
B $C593,1,1 (nothing) for 1 units
B $C594,1,1 OVERHEAD_BRIDGE for 1 units
B $C595,1,1 (nothing) for 1 units
B $C596,1,1 OVERHEAD_BRIDGE for 1 units
B $C597,1,1 (nothing) for 1 units
B $C598,1,1 OVERHEAD_BRIDGE for 1 units
B $C599,1,1 (nothing) for 7 units
B $C59A,1,1 TUNNEL_LIGHT for 10 units
B $C59B,1,1 (nothing) for 2 units
B $C59C,1,1 DOUBLE_STREET_LAMP for 1 units
B $C59D,1,1 (nothing) for 1 units
B $C59E,1,1 DOUBLE_STREET_LAMP for 1 units
B $C59F,1,1 <Esc> Jump
B $C5A0,1,1
W $C5A1,2,2 [$C62D] Target
b $C5A3 [Stage 5] Map right object data
B $C5A3,1,1 (nothing) for 2 units
B $C5A4,1,1 OVERHEAD_BRIDGE for 1 units
B $C5A5,1,1 (nothing) for 1 units
B $C5A6,1,1 OVERHEAD_BRIDGE for 1 units
B $C5A7,1,1 (nothing) for 1 units
B $C5A8,1,1 OVERHEAD_BRIDGE for 1 units
B $C5A9,1,1 (nothing) for 1 units
B $C5AA,1,1 OVERHEAD_BRIDGE for 1 units
B $C5AB,1,1 (nothing) for 1 units
B $C5AC,1,1 OVERHEAD_BRIDGE for 1 units
B $C5AD,1,1 (nothing) for 1 units
B $C5AE,1,1 OVERHEAD_BRIDGE for 1 units
B $C5AF,1,1 (nothing) for 1 units
B $C5B0,1,1 OVERHEAD_BRIDGE for 1 units
B $C5B1,1,1 (nothing) for 1 units
B $C5B2,1,1 OVERHEAD_BRIDGE for 1 units
B $C5B3,1,1 (nothing) for 1 units
B $C5B4,1,1 OVERHEAD_BRIDGE for 1 units
B $C5B5,1,1 (nothing) for 1 units
B $C5B6,1,1 OVERHEAD_BRIDGE for 1 units
B $C5B7,1,1 (nothing) for 1 units
B $C5B8,1,1 OVERHEAD_BRIDGE for 1 units
B $C5B9,1,1 (nothing) for 1 units
B $C5BA,1,1 OVERHEAD_BRIDGE for 1 units
B $C5BB,1,1 (nothing) for 21 units
B $C5BC,1,1
B $C5BD,1,1 OVERHEAD_BRIDGE for 1 units
B $C5BE,1,1 (nothing) for 1 units
B $C5BF,1,1 OVERHEAD_BRIDGE for 1 units
B $C5C0,1,1 (nothing) for 1 units
B $C5C1,1,1 OVERHEAD_BRIDGE for 1 units
B $C5C2,1,1 (nothing) for 1 units
B $C5C3,1,1 OVERHEAD_BRIDGE for 1 units
B $C5C4,1,1 (nothing) for 1 units
B $C5C5,1,1 OVERHEAD_BRIDGE for 1 units
B $C5C6,1,1 (nothing) for 1 units
B $C5C7,1,1 OVERHEAD_BRIDGE for 1 units
B $C5C8,1,1 (nothing) for 1 units
B $C5C9,1,1 OVERHEAD_BRIDGE for 1 units
B $C5CA,1,1 (nothing) for 1 units
B $C5CB,1,1 OVERHEAD_BRIDGE for 1 units
B $C5CC,1,1 (nothing) for 1 units
B $C5CD,1,1 OVERHEAD_BRIDGE for 1 units
B $C5CE,1,1 (nothing) for 1 units
B $C5CF,1,1 OVERHEAD_BRIDGE for 1 units
B $C5D0,1,1 (nothing) for 1 units
B $C5D1,1,1 OVERHEAD_BRIDGE for 1 units
B $C5D2,1,1 (nothing) for 1 units
B $C5D3,1,1 OVERHEAD_BRIDGE for 1 units
B $C5D4,1,1 (nothing) for 1 units
B $C5D5,1,1 OVERHEAD_BRIDGE for 1 units
B $C5D6,1,1 (nothing) for 1 units
B $C5D7,1,1 OVERHEAD_BRIDGE for 1 units
B $C5D8,1,1 (nothing) for 1 units
B $C5D9,1,1 OVERHEAD_BRIDGE for 1 units
B $C5DA,1,1 (nothing) for 1 units
B $C5DB,1,1 OVERHEAD_BRIDGE for 1 units
B $C5DC,1,1 (nothing) for 7 units
B $C5DD,1,1 TUNNEL_LIGHT for 10 units
B $C5DE,1,1 (nothing) for 5 units
B $C5DF,1,1 <Esc> Jump
B $C5E0,1,1
W $C5E1,2,2 [$C651] Target
b $C5E3 [Stage 5] Map curvature data
B $C5E3,1,1 Curve Straight for 35 units
B $C5E4,2,2
B $C5E6,1,1 Curve Left Hard for 12 units
B $C5E7,1,1 Curve Right for 1 units
B $C5E8,1,1 Curve Right Hard for 10 units
B $C5E9,1,1 Curve Right for 6 units
B $C5EA,1,1 Curve Right Hard for 64 units
B $C5EB,4,4
B $C5EF,1,1 Curve Right for 26 units
B $C5F0,1,1
B $C5F1,1,1 Curve Straight for 32 units
B $C5F2,2,2
B $C5F4,1,1 <Esc> Jump
B $C5F5,1,1
W $C5F6,2,2 [$C65C] Target
b $C5F8 [Stage 5] Map height data
B $C5F8,1,1 Going Down 5 for 15 units
B $C5F9,1,1 Going Down 3 for 3 units
B $C5FA,1,1 Level Road for 4 units
B $C5FB,1,1 Going Up 7 for 15 units
B $C5FC,1,1 Going Up 5 for 1 units
B $C5FD,1,1 Level Road for 28 units
B $C5FE,1,1
B $C5FF,1,1 Going Up 3 for 20 units
B $C600,1,1
B $C601,1,1 Going Up 5 for 6 units
B $C602,1,1 Going Up 7 for 8 units
B $C603,1,1 Going Up 5 for 2 units
B $C604,1,1 Going Up 3 for 2 units
B $C605,1,1 Level Road for 1 units
B $C606,1,1 Going Down 1 for 2 units
B $C607,1,1 Going Down 3 for 4 units
B $C608,1,1 Going Down 5 for 4 units
B $C609,1,1 Going Down 7 for 11 units
B $C60A,1,1 Going Down 5 for 10 units
B $C60B,1,1 Going Down 3 for 11 units
B $C60C,1,1 Going Down 1 for 2 units
B $C60D,1,1 Level Road for 37 units
B $C60E,2,2
B $C610,1,1 <Esc> Jump
B $C611,1,1
W $C612,2,2 [$C684] Target
b $C614 [Stage 5] Map lanes data
B $C614,1,1 4 Lanes              [||||] {00} for 44 units
B $C615,1,1
B $C616,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C617,1,1
B $C618,1,1 3 Lanes L            [|||]  {81} for 12 units
B $C619,1,1
B $C61A,1,1 Tunnel start                {45} for 102 units
B $C61B,1,1
B $C61C,1,1 Tunnel cont/end?            {59} for 2 units
B $C61D,1,1
B $C61E,1,1 3 Lanes L            [|||]  {81} for 18 units
B $C61F,1,1
B $C620,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C621,1,1
B $C622,1,1 4 Lanes              [||||] {00} for 4 units
B $C623,1,1
B $C624,1,1 <Esc> Jump
B $C625,1,1
W $C626,2,2 [$C6BB] Target
b $C628 [Stage 5] Map hazards data
B $C628,1,1 Wait for 93 units
B $C629,1,1 <Esc> Jump
B $C62A,1,1
W $C62B,2,2 [$C6D1] Target
b $C62D [Stage 5] Map left object data
B $C62D,1,1 (nothing) for 2 units
B $C62E,1,1 TELEGRAPH_POLE for 1 units
B $C62F,1,1 (nothing) for 1 units
B $C630,1,1 TELEGRAPH_POLE for 1 units
B $C631,1,1 (nothing) for 1 units
B $C632,1,1 TELEGRAPH_POLE for 1 units
B $C633,1,1 (nothing) for 1 units
B $C634,1,1 TELEGRAPH_POLE for 1 units
B $C635,1,1 (nothing) for 1 units
B $C636,1,1 TELEGRAPH_POLE for 1 units
B $C637,1,1 (nothing) for 1 units
B $C638,1,1 TELEGRAPH_POLE for 1 units
B $C639,1,1 (nothing) for 1 units
B $C63A,1,1 TELEGRAPH_POLE for 1 units
B $C63B,1,1 (nothing) for 1 units
B $C63C,1,1 TELEGRAPH_POLE for 1 units
B $C63D,1,1 (nothing) for 1 units
B $C63E,1,1 TELEGRAPH_POLE for 1 units
B $C63F,1,1 (nothing) for 1 units
B $C640,1,1 TELEGRAPH_POLE for 1 units
B $C641,1,1 (nothing) for 1 units
B $C642,1,1 TELEGRAPH_POLE for 1 units
B $C643,1,1 (nothing) for 1 units
B $C644,1,1 TELEGRAPH_POLE for 1 units
B $C645,1,1 (nothing) for 1 units
B $C646,1,1 TELEGRAPH_POLE for 1 units
B $C647,1,1 (nothing) for 3 units
B $C648,1,1 TUNNEL_LIGHT for 51 units
B $C649,3,3
B $C64C,1,1 (nothing) for 12 units
B $C64D,1,1 <Esc> Jump
B $C64E,1,1
W $C64F,2,2 [$C6EE] Target
b $C651 [Stage 5] Map right object data
B $C651,1,1 (nothing) for 93 units
B $C652,6,6
B $C658,1,1 <Esc> Jump
B $C659,1,1
W $C65A,2,2 [$C7A5] Target
b $C65C [Stage 5] Map curvature data
B $C65C,1,1 Curve Straight for 40 units
B $C65D,2,2
B $C65F,1,1 Curve Left Hard for 41 units
B $C660,2,2
B $C662,1,1 Curve Straight for 19 units
B $C663,1,1
B $C664,1,1 Curve Right Hard for 41 units
B $C665,2,2
B $C667,1,1 Curve Straight for 39 units
B $C668,2,2
B $C66A,1,1 Curve Left Hard for 21 units
B $C66B,1,1
B $C66C,1,1 Curve Right Hard for 25 units
B $C66D,1,1
B $C66E,1,1 Curve Straight for 54 units
B $C66F,3,3
B $C672,1,1 Curve Right Hard for 24 units
B $C673,1,1
B $C674,1,1 Curve Right Very Hard for 17 units
B $C675,1,1
B $C676,1,1 Curve Right Hard for 62 units
B $C677,4,4
B $C67B,1,1 Curve Straight for 61 units
B $C67C,4,4
B $C680,1,1 <Esc> Loop
B $C681,1,1
W $C682,2,2 [$C65C] Target
b $C684 [Stage 5] Map height data
B $C684,1,1 Level Road for 132 units
B $C685,8,8
B $C68D,1,1 Going Down 3 for 2 units
B $C68E,1,1 Going Down 5 for 4 units
B $C68F,1,1 Going Down 7 for 2 units
B $C690,1,1 Going Down 5 for 1 units
B $C691,1,1 Going Down 3 for 2 units
B $C692,1,1 Level Road for 2 units
B $C693,1,1 Going Up 3 for 1 units
B $C694,1,1 Going Up 5 for 2 units
B $C695,1,1 Going Up 7 for 8 units
B $C696,1,1 Going Down 7 for 4 units
B $C697,1,1 Going Down 5 for 1 units
B $C698,1,1 Going Up 5 for 1 units
B $C699,1,1 Going Up 7 for 6 units
B $C69A,1,1 Going Down 7 for 4 units
B $C69B,1,1 Going Down 5 for 1 units
B $C69C,1,1 Going Down 3 for 1 units
B $C69D,1,1 Going Down 1 for 3 units
B $C69E,1,1 Level Road for 111 units
B $C69F,7,7
B $C6A6,1,1 Going Up 3 for 1 units
B $C6A7,1,1 Going Up 5 for 1 units
B $C6A8,1,1 Going Up 7 for 1 units
B $C6A9,1,1 Going Up 5 for 1 units
B $C6AA,1,1 Going Up 3 for 1 units
B $C6AB,1,1 Going Down 3 for 1 units
B $C6AC,1,1 Going Down 5 for 1 units
B $C6AD,1,1 Going Down 7 for 1 units
B $C6AE,1,1 Going Down 5 for 1 units
B $C6AF,1,1 Going Down 3 for 1 units
B $C6B0,1,1 Level Road for 6 units
B $C6B1,1,1 Going Up 5 for 1 units
B $C6B2,1,1 Going Up 7 for 2 units
B $C6B3,1,1 Going Down 3 for 8 units
B $C6B4,1,1 Level Road for 43 units
B $C6B5,2,2
B $C6B7,1,1 <Esc> Loop
B $C6B8,1,1
W $C6B9,2,2 [$C684] Target
b $C6BB [Stage 5] Map lanes data
B $C6BB,1,1 4 Lanes              [||||] {00} for 368 units
B $C6BC,3,3
B $C6BF,1,1 4-3 Narrowing R      [/|||] {8E} for 36 units
B $C6C0,1,1
B $C6C1,1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C6C2,1,1
B $C6C3,1,1 4 Lanes              [||||] {00} for 4 units
B $C6C4,1,1
B $C6C5,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C6C6,1,1
B $C6C7,1,1 3 Lanes L            [|||]  {81} for 8 units
B $C6C8,1,1
B $C6C9,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C6CA,1,1
B $C6CB,1,1 4 Lanes              [||||] {00} for 22 units
B $C6CC,1,1
B $C6CD,1,1 <Esc> Loop
B $C6CE,1,1
W $C6CF,2,2 [$C6BB] Target
b $C6D1 [Stage 5] Map hazards data
B $C6D1,1,1 Wait for 13 units
B $C6D2,1,1 Start Spawning Two Barriers
B $C6D3,1,1
B $C6D4,1,1 Wait for 4 units
B $C6D5,1,1 Stop Spawning Barriers 3?
B $C6D6,1,1
B $C6D7,1,1 Wait for 40 units
B $C6D8,1,1 Start Spawning Barriers Left
B $C6D9,1,1
B $C6DA,1,1 Wait for 2 units
B $C6DB,1,1 Stop Spawning Barriers 3?
B $C6DC,1,1
B $C6DD,1,1 Wait for 91 units
B $C6DE,1,1 Start Spawning Barriers Left
B $C6DF,1,1
B $C6E0,1,1 Wait for 2 units
B $C6E1,1,1 Stop Spawning Barriers 3?
B $C6E2,1,1
B $C6E3,1,1 Wait for 28 units
B $C6E4,1,1 Start Spawning Barriers Left
B $C6E5,1,1
B $C6E6,1,1 Wait for 2 units
B $C6E7,1,1 Stop Spawning Barriers 3?
B $C6E8,1,1
B $C6E9,1,1 Wait for 40 units
B $C6EA,1,1 <Esc> Loop
B $C6EB,1,1
W $C6EC,2,2 [$C6D1] Target
b $C6EE [Stage 5] Map left object data
B $C6EE,1,1 DOUBLE_STREET_LAMP for 1 units
B $C6EF,1,1 (nothing) for 1 units
B $C6F0,1,1 DOUBLE_STREET_LAMP for 1 units
B $C6F1,1,1 (nothing) for 1 units
B $C6F2,1,1 DOUBLE_STREET_LAMP for 1 units
B $C6F3,1,1 (nothing) for 1 units
B $C6F4,1,1 DOUBLE_STREET_LAMP for 1 units
B $C6F5,1,1 (nothing) for 1 units
B $C6F6,1,1 DOUBLE_STREET_LAMP for 1 units
B $C6F7,1,1 (nothing) for 1 units
B $C6F8,1,1 DOUBLE_STREET_LAMP for 1 units
B $C6F9,1,1 (nothing) for 1 units
B $C6FA,1,1 DOUBLE_STREET_LAMP for 1 units
B $C6FB,1,1 (nothing) for 5 units
B $C6FC,1,1 CACTUS for 1 units
B $C6FD,1,1 (nothing) for 1 units
B $C6FE,1,1 CACTUS for 1 units
B $C6FF,1,1 (nothing) for 1 units
B $C700,1,1 CACTUS for 1 units
B $C701,1,1 (nothing) for 3 units
B $C702,1,1 CACTUS for 1 units
B $C703,1,1 (nothing) for 1 units
B $C704,1,1 CACTUS for 1 units
B $C705,1,1 (nothing) for 3 units
B $C706,1,1 CACTUS for 1 units
B $C707,1,1 (nothing) for 3 units
B $C708,1,1 CACTUS for 1 units
B $C709,1,1 (nothing) for 3 units
B $C70A,1,1 HUGE_ROCK for 1 units
B $C70B,1,1 (nothing) for 1 units
B $C70C,1,1 HUGE_ROCK for 1 units
B $C70D,1,1 (nothing) for 1 units
B $C70E,1,1 HUGE_ROCK for 1 units
B $C70F,1,1 (nothing) for 1 units
B $C710,1,1 HUGE_ROCK for 1 units
B $C711,1,1 (nothing) for 1 units
B $C712,1,1 HUGE_ROCK for 1 units
B $C713,1,1 (nothing) for 1 units
B $C714,1,1 HUGE_ROCK for 1 units
B $C715,1,1 (nothing) for 1 units
B $C716,1,1 HUGE_ROCK for 1 units
B $C717,1,1 (nothing) for 1 units
B $C718,1,1 HUGE_ROCK for 1 units
B $C719,1,1 (nothing) for 1 units
B $C71A,1,1 HUGE_ROCK for 1 units
B $C71B,1,1 (nothing) for 1 units
B $C71C,1,1 HUGE_ROCK for 1 units
B $C71D,1,1 (nothing) for 1 units
B $C71E,1,1 HUGE_ROCK for 1 units
B $C71F,1,1 (nothing) for 1 units
B $C720,1,1 HUGE_ROCK for 1 units
B $C721,1,1 (nothing) for 1 units
B $C722,1,1 HUGE_ROCK for 1 units
B $C723,1,1 (nothing) for 1 units
B $C724,1,1 HUGE_ROCK for 1 units
B $C725,1,1 (nothing) for 1 units
B $C726,1,1 HUGE_ROCK for 1 units
B $C727,1,1 (nothing) for 1 units
B $C728,1,1 HUGE_ROCK for 1 units
B $C729,1,1 (nothing) for 1 units
B $C72A,1,1 HUGE_ROCK for 1 units
B $C72B,1,1 (nothing) for 1 units
B $C72C,1,1 HUGE_ROCK for 1 units
B $C72D,1,1 (nothing) for 3 units
B $C72E,1,1 HUGE_ROCK for 1 units
B $C72F,1,1 (nothing) for 3 units
B $C730,1,1 HUGE_ROCK for 1 units
B $C731,1,1 (nothing) for 17 units
B $C732,1,1
B $C733,1,1 HUGE_ROCK for 1 units
B $C734,1,1 (nothing) for 1 units
B $C735,1,1 HUGE_ROCK for 1 units
B $C736,1,1 (nothing) for 1 units
B $C737,1,1 HUGE_ROCK for 1 units
B $C738,1,1 (nothing) for 1 units
B $C739,1,1 HUGE_ROCK for 1 units
B $C73A,1,1 (nothing) for 1 units
B $C73B,1,1 HUGE_ROCK for 1 units
B $C73C,1,1 (nothing) for 1 units
B $C73D,1,1 HUGE_ROCK for 1 units
B $C73E,1,1 (nothing) for 1 units
B $C73F,1,1 HUGE_ROCK for 1 units
B $C740,1,1 (nothing) for 1 units
B $C741,1,1 HUGE_ROCK for 1 units
B $C742,1,1 (nothing) for 1 units
B $C743,1,1 HUGE_ROCK for 1 units
B $C744,1,1 (nothing) for 1 units
B $C745,1,1 HUGE_ROCK for 1 units
B $C746,1,1 (nothing) for 1 units
B $C747,1,1 HUGE_ROCK for 1 units
B $C748,1,1 (nothing) for 1 units
B $C749,1,1 HUGE_ROCK for 1 units
B $C74A,1,1 (nothing) for 1 units
B $C74B,1,1 HUGE_ROCK for 1 units
B $C74C,1,1 (nothing) for 1 units
B $C74D,1,1 HUGE_ROCK for 1 units
B $C74E,1,1 (nothing) for 1 units
B $C74F,1,1 HUGE_ROCK for 1 units
B $C750,1,1 (nothing) for 1 units
B $C751,1,1 HUGE_ROCK for 1 units
B $C752,1,1 (nothing) for 9 units
B $C753,1,1 HUGE_ROCK for 1 units
B $C754,1,1 (nothing) for 1 units
B $C755,1,1 HUGE_ROCK for 1 units
B $C756,1,1 (nothing) for 1 units
B $C757,1,1 HUGE_ROCK for 1 units
B $C758,1,1 (nothing) for 1 units
B $C759,1,1 HUGE_ROCK for 1 units
B $C75A,1,1 (nothing) for 1 units
B $C75B,1,1 HUGE_ROCK for 1 units
B $C75C,1,1 (nothing) for 1 units
B $C75D,1,1 HUGE_ROCK for 1 units
B $C75E,1,1 (nothing) for 1 units
B $C75F,1,1 HUGE_ROCK for 1 units
B $C760,1,1 (nothing) for 1 units
B $C761,1,1 HUGE_ROCK for 1 units
B $C762,1,1 (nothing) for 1 units
B $C763,1,1 HUGE_ROCK for 1 units
B $C764,1,1 (nothing) for 1 units
B $C765,1,1 HUGE_ROCK for 1 units
B $C766,1,1 (nothing) for 1 units
B $C767,1,1 HUGE_ROCK for 1 units
B $C768,1,1 (nothing) for 1 units
B $C769,1,1 HUGE_ROCK for 1 units
B $C76A,1,1 (nothing) for 1 units
B $C76B,1,1 HUGE_ROCK for 1 units
B $C76C,1,1 (nothing) for 1 units
B $C76D,1,1 HUGE_ROCK for 1 units
B $C76E,1,1 (nothing) for 1 units
B $C76F,1,1 HUGE_ROCK for 1 units
B $C770,1,1 (nothing) for 1 units
B $C771,1,1 HUGE_ROCK for 1 units
B $C772,1,1 (nothing) for 1 units
B $C773,1,1 HUGE_ROCK for 1 units
B $C774,1,1 (nothing) for 1 units
B $C775,1,1 HUGE_ROCK for 1 units
B $C776,1,1 (nothing) for 1 units
B $C777,1,1 HUGE_ROCK for 1 units
B $C778,1,1 (nothing) for 1 units
B $C779,1,1 HUGE_ROCK for 1 units
B $C77A,1,1 (nothing) for 1 units
B $C77B,1,1 HUGE_ROCK for 1 units
B $C77C,1,1 (nothing) for 1 units
B $C77D,1,1 HUGE_ROCK for 1 units
B $C77E,1,1 (nothing) for 1 units
B $C77F,1,1 HUGE_ROCK for 1 units
B $C780,1,1 (nothing) for 1 units
B $C781,1,1 HUGE_ROCK for 1 units
B $C782,1,1 (nothing) for 1 units
B $C783,1,1 HUGE_ROCK for 1 units
B $C784,1,1 (nothing) for 1 units
B $C785,1,1 HUGE_ROCK for 1 units
B $C786,1,1 (nothing) for 1 units
B $C787,1,1 HUGE_ROCK for 1 units
B $C788,1,1 (nothing) for 1 units
B $C789,1,1 HUGE_ROCK for 1 units
B $C78A,1,1 (nothing) for 1 units
B $C78B,1,1 HUGE_ROCK for 1 units
B $C78C,1,1 (nothing) for 1 units
B $C78D,1,1 HUGE_ROCK for 1 units
B $C78E,1,1 (nothing) for 1 units
B $C78F,1,1 HUGE_ROCK for 1 units
B $C790,1,1 (nothing) for 5 units
B $C791,1,1 CACTUS for 1 units
B $C792,1,1 (nothing) for 1 units
B $C793,1,1 CACTUS for 1 units
B $C794,1,1 (nothing) for 1 units
B $C795,1,1 CACTUS for 1 units
B $C796,1,1 (nothing) for 1 units
B $C797,1,1 CACTUS for 1 units
B $C798,1,1 (nothing) for 1 units
B $C799,1,1 DOUBLE_STREET_LAMP for 1 units
B $C79A,1,1 (nothing) for 1 units
B $C79B,1,1 DOUBLE_STREET_LAMP for 1 units
B $C79C,1,1 (nothing) for 1 units
B $C79D,1,1 DOUBLE_STREET_LAMP for 1 units
B $C79E,1,1 (nothing) for 1 units
B $C79F,1,1 DOUBLE_STREET_LAMP for 1 units
B $C7A0,1,1 (nothing) for 1 units
B $C7A1,1,1 <Esc> Loop
B $C7A2,1,1
W $C7A3,2,2 [$C6EE] Target
b $C7A5 [Stage 5] Map right object data
B $C7A5,1,1 (nothing) for 6 units
B $C7A6,1,1 CACTUS for 1 units
B $C7A7,1,1 (nothing) for 1 units
B $C7A8,1,1 CACTUS for 1 units
B $C7A9,1,1 (nothing) for 1 units
B $C7AA,1,1 CACTUS for 1 units
B $C7AB,1,1 (nothing) for 1 units
B $C7AC,1,1 CACTUS for 1 units
B $C7AD,1,1 (nothing) for 5 units
B $C7AE,1,1 HUGE_ROCK for 1 units
B $C7AF,1,1 (nothing) for 1 units
B $C7B0,1,1 HUGE_ROCK for 1 units
B $C7B1,1,1 (nothing) for 1 units
B $C7B2,1,1 HUGE_ROCK for 1 units
B $C7B3,1,1 (nothing) for 1 units
B $C7B4,1,1 HUGE_ROCK for 1 units
B $C7B5,1,1 (nothing) for 1 units
B $C7B6,1,1 HUGE_ROCK for 1 units
B $C7B7,1,1 (nothing) for 1 units
B $C7B8,1,1 HUGE_ROCK for 1 units
B $C7B9,1,1 (nothing) for 1 units
B $C7BA,1,1 HUGE_ROCK for 1 units
B $C7BB,1,1 (nothing) for 1 units
B $C7BC,1,1 HUGE_ROCK for 1 units
B $C7BD,1,1 (nothing) for 1 units
B $C7BE,1,1 HUGE_ROCK for 1 units
B $C7BF,1,1 (nothing) for 1 units
B $C7C0,1,1 HUGE_ROCK for 1 units
B $C7C1,1,1 (nothing) for 1 units
B $C7C2,1,1 HUGE_ROCK for 1 units
B $C7C3,1,1 (nothing) for 1 units
B $C7C4,1,1 HUGE_ROCK for 1 units
B $C7C5,1,1 (nothing) for 1 units
B $C7C6,1,1 HUGE_ROCK for 1 units
B $C7C7,1,1 (nothing) for 1 units
B $C7C8,1,1 HUGE_ROCK for 1 units
B $C7C9,1,1 (nothing) for 1 units
B $C7CA,1,1 HUGE_ROCK for 1 units
B $C7CB,1,1 (nothing) for 1 units
B $C7CC,1,1 HUGE_ROCK for 1 units
B $C7CD,1,1 (nothing) for 1 units
B $C7CE,1,1 HUGE_ROCK for 1 units
B $C7CF,1,1 (nothing) for 27 units
B $C7D0,1,1
B $C7D1,1,1 HUGE_ROCK for 1 units
B $C7D2,1,1 (nothing) for 1 units
B $C7D3,1,1 HUGE_ROCK for 1 units
B $C7D4,1,1 (nothing) for 1 units
B $C7D5,1,1 HUGE_ROCK for 1 units
B $C7D6,1,1 (nothing) for 1 units
B $C7D7,1,1 HUGE_ROCK for 1 units
B $C7D8,1,1 (nothing) for 1 units
B $C7D9,1,1 HUGE_ROCK for 1 units
B $C7DA,1,1 (nothing) for 1 units
B $C7DB,1,1 HUGE_ROCK for 1 units
B $C7DC,1,1 (nothing) for 1 units
B $C7DD,1,1 HUGE_ROCK for 1 units
B $C7DE,1,1 (nothing) for 1 units
B $C7DF,1,1 HUGE_ROCK for 1 units
B $C7E0,1,1 (nothing) for 1 units
B $C7E1,1,1 HUGE_ROCK for 1 units
B $C7E2,1,1 (nothing) for 1 units
B $C7E3,1,1 HUGE_ROCK for 1 units
B $C7E4,1,1 (nothing) for 1 units
B $C7E5,1,1 HUGE_ROCK for 1 units
B $C7E6,1,1 (nothing) for 1 units
B $C7E7,1,1 HUGE_ROCK for 1 units
B $C7E8,1,1 (nothing) for 1 units
B $C7E9,1,1 HUGE_ROCK for 1 units
B $C7EA,1,1 (nothing) for 1 units
B $C7EB,1,1 HUGE_ROCK for 1 units
B $C7EC,1,1 (nothing) for 1 units
B $C7ED,1,1 HUGE_ROCK for 1 units
B $C7EE,1,1 (nothing) for 1 units
B $C7EF,1,1 HUGE_ROCK for 1 units
B $C7F0,1,1 (nothing) for 1 units
B $C7F1,1,1 HUGE_ROCK for 1 units
B $C7F2,1,1 (nothing) for 1 units
B $C7F3,1,1 HUGE_ROCK for 1 units
B $C7F4,1,1 (nothing) for 1 units
B $C7F5,1,1 HUGE_ROCK for 1 units
B $C7F6,1,1 (nothing) for 1 units
B $C7F7,1,1 HUGE_ROCK for 1 units
B $C7F8,1,1 (nothing) for 1 units
B $C7F9,1,1 HUGE_ROCK for 1 units
B $C7FA,1,1 (nothing) for 1 units
B $C7FB,1,1 HUGE_ROCK for 1 units
B $C7FC,1,1 (nothing) for 1 units
B $C7FD,1,1 HUGE_ROCK for 1 units
B $C7FE,1,1 (nothing) for 1 units
B $C7FF,1,1 HUGE_ROCK for 1 units
B $C800,1,1 (nothing) for 1 units
B $C801,1,1 HUGE_ROCK for 1 units
B $C802,1,1 (nothing) for 1 units
B $C803,1,1 HUGE_ROCK for 1 units
B $C804,1,1 (nothing) for 47 units
B $C805,3,3
B $C808,1,1 CACTUS for 1 units
B $C809,1,1 (nothing) for 1 units
B $C80A,1,1 CACTUS for 1 units
B $C80B,1,1 (nothing) for 3 units
B $C80C,1,1 CACTUS for 1 units
B $C80D,1,1 (nothing) for 5 units
B $C80E,1,1 HUGE_ROCK for 1 units
B $C80F,1,1 (nothing) for 1 units
B $C810,1,1 HUGE_ROCK for 1 units
B $C811,1,1 (nothing) for 1 units
B $C812,1,1 HUGE_ROCK for 1 units
B $C813,1,1 (nothing) for 1 units
B $C814,1,1 HUGE_ROCK for 1 units
B $C815,1,1 (nothing) for 1 units
B $C816,1,1 HUGE_ROCK for 1 units
B $C817,1,1 (nothing) for 1 units
B $C818,1,1 HUGE_ROCK for 1 units
B $C819,1,1 (nothing) for 1 units
B $C81A,1,1 HUGE_ROCK for 1 units
B $C81B,1,1 (nothing) for 1 units
B $C81C,1,1 HUGE_ROCK for 1 units
B $C81D,1,1 (nothing) for 1 units
B $C81E,1,1 HUGE_ROCK for 1 units
B $C81F,1,1 (nothing) for 1 units
B $C820,1,1 HUGE_ROCK for 1 units
B $C821,1,1 (nothing) for 1 units
B $C822,1,1 HUGE_ROCK for 1 units
B $C823,1,1 (nothing) for 5 units
B $C824,1,1 CACTUS for 1 units
B $C825,1,1 (nothing) for 1 units
B $C826,1,1 CACTUS for 1 units
B $C827,1,1 (nothing) for 3 units
B $C828,1,1 CACTUS for 1 units
B $C829,1,1 (nothing) for 1 units
B $C82A,1,1 <Esc> Loop
B $C82B,1,1
W $C82C,2,2 [$C7A5] Target
B $C82E,180,8*22,4
N $C8E2 LOD
B $C8E2,1,1 Width (bytes)
B $C8E3,1,1 Flags
B $C8E4,1,1 Height (pixels)
W $C8E5,2,2 [$C960] Bitmap address
W $C8E7,2,2 [$C960] Pre-shifted bitmap address
N $C8E9 LOD
B $C8E9,1,1 Width (bytes)
B $C8EA,1,1 Flags
B $C8EB,1,1 Height (pixels)
W $C8EC,2,2 [$CA14] Bitmap address
W $C8EE,2,2 [$CA14] Pre-shifted bitmap address
N $C8F0 LOD
B $C8F0,1,1 Width (bytes)
B $C8F1,1,1 Flags
B $C8F2,1,1 Height (pixels)
W $C8F3,2,2 [$CA82] Bitmap address
W $C8F5,2,2 [$CA82] Pre-shifted bitmap address
N $C8F7 LOD
B $C8F7,1,1 Width (bytes)
B $C8F8,1,1 Flags
B $C8F9,1,1 Height (pixels)
W $C8FA,2,2 [$CA82] Bitmap address
W $C8FC,2,2 [$CA82] Pre-shifted bitmap address
N $C8FE LOD
B $C8FE,1,1 Width (bytes)
B $C8FF,1,1 Flags
B $C900,1,1 Height (pixels)
W $C901,2,2 [$CAAF] Bitmap address
W $C903,2,2 [$CAAF] Pre-shifted bitmap address
N $C905 LOD
B $C905,1,1 Width (bytes)
B $C906,1,1 Flags
B $C907,1,1 Height (pixels)
W $C908,2,2 [$CAAF] Bitmap address
W $C90A,2,2 [$CADF] Pre-shifted bitmap address
N $C90C LOD
B $C90C,1,1 Width (bytes)
B $C90D,1,1 Flags
B $C90E,1,1 Height (pixels)
W $C90F,2,2 [$CB0F] Bitmap address
W $C911,2,2 [$CB0F] Pre-shifted bitmap address
N $C913 LOD
B $C913,1,1 Width (bytes)
B $C914,1,1 Flags
B $C915,1,1 Height (pixels)
W $C916,2,2 [$CBF9] Bitmap address
W $C918,2,2 [$CBF9] Pre-shifted bitmap address
N $C91A LOD
B $C91A,1,1 Width (bytes)
B $C91B,1,1 Flags
B $C91C,1,1 Height (pixels)
W $C91D,2,2 [$CC8A] Bitmap address
W $C91F,2,2 [$CC8A] Pre-shifted bitmap address
N $C921 LOD
B $C921,1,1 Width (bytes)
B $C922,1,1 Flags
B $C923,1,1 Height (pixels)
W $C924,2,2 [$CC8A] Bitmap address
W $C926,2,2 [$CC8A] Pre-shifted bitmap address
N $C928 LOD
B $C928,1,1 Width (bytes)
B $C929,1,1 Flags
B $C92A,1,1 Height (pixels)
W $C92B,2,2 [$CCC6] Bitmap address
W $C92D,2,2 [$CCF6] Pre-shifted bitmap address
N $C92F LOD
B $C92F,1,1 Width (bytes)
B $C930,1,1 Flags
B $C931,1,1 Height (pixels)
W $C932,2,2 [$CCC6] Bitmap address
W $C934,2,2 [$CCF6] Pre-shifted bitmap address
N $C936 LOD
B $C936,1,1 Width (bytes)
B $C937,1,1 Flags
B $C938,1,1 Height (pixels)
W $C939,2,2 [$CD26] Bitmap address
W $C93B,2,2 [$CD26] Pre-shifted bitmap address
N $C93D LOD
B $C93D,1,1 Width (bytes)
B $C93E,1,1 Flags
B $C93F,1,1 Height (pixels)
W $C940,2,2 [$CDDA] Bitmap address
W $C942,2,2 [$CDDA] Pre-shifted bitmap address
N $C944 LOD
B $C944,1,1 Width (bytes)
B $C945,1,1 Flags
B $C946,1,1 Height (pixels)
W $C947,2,2 [$CE2A] Bitmap address
W $C949,2,2 [$CE2A] Pre-shifted bitmap address
N $C94B LOD
B $C94B,1,1 Width (bytes)
B $C94C,1,1 Flags
B $C94D,1,1 Height (pixels)
W $C94E,2,2 [$CE2A] Bitmap address
W $C950,2,2 [$CE2A] Pre-shifted bitmap address
N $C952 LOD
B $C952,1,1 Width (bytes)
B $C953,1,1 Flags
B $C954,1,1 Height (pixels)
W $C955,2,2 [$CE51] Bitmap address
W $C957,2,2 [$CE51] Pre-shifted bitmap address
N $C959 LOD
B $C959,1,1 Width (bytes)
B $C95A,1,1 Flags
B $C95B,1,1 Height (pixels)
W $C95C,2,2 [$CE51] Bitmap address
W $C95E,2,2 [$CE71] Pre-shifted bitmap address
B $C960,180,6 Bitmap data 6 bytes x 30
B $CA14,110,5 Bitmap data 5 bytes x 22
B $CA82,45,3 Bitmap data 3 bytes x 15
B $CAAF,48,3 Bitmap data (masked) 3 bytes x 8
B $CADF,48,3 Pre-shifted bitmap data (masked) 3 bytes x 8
B $CB0F,234,6 Bitmap data 6 bytes x 39
B $CBF9,145,5 Bitmap data 5 bytes x 29
B $CC8A,60,3 Bitmap data 3 bytes x 20
B $CCC6,48,2 Bitmap data (masked) 2 bytes x 12
B $CCF6,48,2 Pre-shifted bitmap data (masked) 2 bytes x 12
B $CD26,180,6 Bitmap data 6 bytes x 30
B $CDDA,80,4 Bitmap data 4 bytes x 20
B $CE2A,39,3 Bitmap data 3 bytes x 13
B $CE51,32,2 Bitmap data (masked) 2 bytes x 8
B $CE71,32,2 Pre-shifted bitmap data (masked) 2 bytes x 8
B $CE91,1935,8*241,7
N $D620 LOD
B $D620,1,1 Width (bytes)
B $D621,1,1 Flags
B $D622,1,1 Height (pixels)
W $D623,2,2 [$D64A] Bitmap address
W $D625,2,2 [$D64A] Pre-shifted bitmap address
N $D627 LOD
B $D627,1,1 Width (bytes)
B $D628,1,1 Flags
B $D629,1,1 Height (pixels)
W $D62A,2,2 [$D64A] Bitmap address
W $D62C,2,2 [$D64A] Pre-shifted bitmap address
N $D62E LOD
B $D62E,1,1 Width (bytes)
B $D62F,1,1 Flags
B $D630,1,1 Height (pixels)
W $D631,2,2 [$D68E] Bitmap address
W $D633,2,2 [$D68E] Pre-shifted bitmap address
N $D635 LOD
B $D635,1,1 Width (bytes)
B $D636,1,1 Flags
B $D637,1,1 Height (pixels)
W $D638,2,2 [$D6B5] Bitmap address
W $D63A,2,2 [$D6B5] Pre-shifted bitmap address
N $D63C LOD
B $D63C,1,1 Width (bytes)
B $D63D,1,1 Flags
B $D63E,1,1 Height (pixels)
W $D63F,2,2 [$D6B5] Bitmap address
W $D641,2,2 [$D6B5] Pre-shifted bitmap address
N $D643 LOD
B $D643,1,1 Width (bytes)
B $D644,1,1 Flags
B $D645,1,1 Height (pixels)
W $D646,2,2 [$D6C7] Bitmap address
W $D648,2,2 [$D6E3] Pre-shifted bitmap address
B $D64A,68,4 Bitmap data 4 bytes x 17
B $D68E,39,3 Bitmap data 3 bytes x 13
B $D6B5,18,2 Bitmap data 2 bytes x 9
B $D6C7,28,2 Bitmap data (masked) 2 bytes x 7
B $D6E3,28,2 Pre-shifted bitmap data (masked) 2 bytes x 7
B $D6FF,10497,8*1312,1
