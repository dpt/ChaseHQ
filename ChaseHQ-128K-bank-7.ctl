> $C000 ; ChaseHQ-128K-bank-7.skool
> $C000 ;
> $C000 ; Bank 7 holds the data for stage 5 and the end screen.
> $C000 ;
> $C000
@ $C000 org
b $C000 [Stage 5] Horizon graphic
b $C0F0 [Stage 5] Per-stage data
W $C0F0,2 Address of PERP's mugshot attributes
W $C0F2,2 Address of PERP's mugshot bitmap
W $C0F4,2 Screen attributes used for the ground colour (a pair of matching bytes)
W $C0F6,2 Address of table of LODs for tumbleweeds, barriers.
W $C0F8,2 points to objects?
W $C0FA,2 points to objects?
W $C0FC,2 points to objects?
W $C0FE,2 Address of turn sign arg and handler address
W $C100,2 Address of graphics entry 10
W $C102,2 Address of graphics entry 12
W $C104,2 Address of Nancy's perp description
W $C106,2 Address of arrest messages
W $C108,2 Helicopter related 1
W $C10A,2 Helicopter related 2
w $C10C [Stage 5] Table of addresses of LODs
W $C10C,2 Address of LOD of stone/dust?
W $C10E,2 Address of LOD of stone/dust?
W $C110,2 Address of LOD of car (perp's car)
W $C112,2 Address of LOD of lambo?
W $C114,2 Address of LOD of truck?
W $C116,2 Address of LOD of lambo again?
W $C118,2 Address of LOD of car (generic car)?
b $C11A [Stage 5] Per-stage difficulty settings
B $C11A,1 How often cars spawn. Lower values spawn cars more often.
B $C11B,1 smash related
B $C11C,1 smash related
w $C11D [Stage 5] Per-stage setup data
W $C11D,2 road_pos
W $C11F,2 -> Start stretch, curvature
W $C121,2 -> Start stretch, height
W $C123,2 -> Start stretch, lanes
W $C125,2 -> Start stretch, right-side objects
W $C127,2 -> Start stretch, left-side objects
W $C129,2 -> Start stretch, hazards
w $C12B [Stage 5] Per-stage attract mode data
W $C12B,2 road_pos
W $C12D,2 -> Loop section, curvature
W $C12F,2 -> Loop section, height
W $C131,2 -> Loop section, lanes
W $C133,2 -> Loop section, right-side objects
W $C135,2 -> Loop section, left-side objects
W $C137,2 -> Loop section, hazards
b $C139 [Stage 5] character id, e.g. nancy
w $C13A [Stage 5] perp desc ptr
w $C13C [Stage 5] perp desc ptr
w $C13E [Stage 5] perp desc ptr
w $C140 [Stage 5] perp desc ptr
b $C142 [Stage 5] terminator?
T $C145 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $C16D "EMERGENCY HERE. THE EASTERN BLOC SPY FROM"
T $C196 "WASHINGTON IS FLEEING TOWARDS THE SUBURBS."
T $C1C0 "THE TARGET VEHICLE IS UNKNOWN... OVER."
b $C1E6 [Stage 5] frame delay until first message?
B $C1E7,1 frame delay until next message?
B $C1E8,1 flags
B $C1E9,1 attribute
W $C1EA,2 back buffer addr
W $C1EC,2 attribute addr
T $C1EE "OK! YOU ARE UNDER ARREST ON"
B $C209,1 frame delay until next message?
B $C20A,1 flags
B $C20B,1 attribute
W $C20C,2 back buffer addr
W $C20E,2 attribute addr
T $C210 "SUSPICION OF ESPIONAGE AND"
B $C22A,1 frame delay until next message?
B $C22B,1 flags
B $C22C,1 attribute
W $C22D,2 back buffer addr
W $C22F,2 attribute addr
T $C231 "MURDER"
B $C237,1 frame delay until next message?
N $C239 Hittable hazards
b $C2A1 [Stage 5] Map curvature data
B $C2A1,1 Curve Straight for 50 units
B $C2A5,1 Curve Left Hard for 21 units
B $C2A7,1 Curve Left for 14 units
B $C2A8,1 Curve Straight for 35 units
B $C2AB,1 Curve Right Hard for 21 units
B $C2AD,1 Curve Right Very Hard for 15 units
B $C2AE,1 Curve Right Hard for 25 units
B $C2B0,1 Curve Right for 10 units
B $C2B1,1 Curve Straight for 79 units
B $C2B7,1 Curve Left Hard for 31 units
B $C2BA,1 Curve Left Very Hard for 20 units
B $C2BC,1 Curve Left Hard for 33 units
B $C2BF,1 Curve Left for 7 units
B $C2C0,1 Curve Straight for 39 units
B $C2C3,1 <Esc> Split
W $C2C5,2 Left target
W $C2C7,2 Right target
b $C2C9 [Stage 5] Map height data
B $C2C9,1 Level Road for 40 units
B $C2CC,1 Going Up 3 for 2 units
B $C2CD,1 Going Up 5 for 2 units
B $C2CE,1 Going Up 7 for 1 units
B $C2CF,1 Going Up 5 for 1 units
B $C2D0,1 Going Up 3 for 5 units
B $C2D1,1 Level Road for 20 units
B $C2D3,1 Going Down 3 for 1 units
B $C2D4,1 Going Down 5 for 3 units
B $C2D5,1 Going Down 3 for 4 units
B $C2D6,1 Going Down 7 for 1 units
B $C2D7,1 Going Down 5 for 1 units
B $C2D8,1 Going Down 3 for 1 units
B $C2D9,1 Level Road for 19 units
B $C2DB,1 Going Up 3 for 2 units
B $C2DC,1 Going Up 5 for 1 units
B $C2DD,1 Going Up 7 for 3 units
B $C2DE,1 Going Down 7 for 3 units
B $C2DF,1 Going Down 5 for 1 units
B $C2E0,1 Going Down 3 for 1 units
B $C2E1,1 Level Road for 288 units
B $C2F5,1 <Esc> Split
W $C2F7,2 Left target
W $C2F9,2 Right target
b $C2FB [Stage 5] Map lanes data
B $C2FB,1 2 Lanes R              [||] {03} for 36 units
B $C2FD,1 2-3 Widening R        [\||] {1F} for 2 units
B $C2FF,1 3 Lanes R             [|||] {82} for 52 units
B $C301,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C303,1 4 Lanes              [||||] {00} for 308 units
B $C307,1 <Esc> Split
W $C309,2 Left target
W $C30B,2 Right target
b $C30D [Stage 5] Map hazards data
B $C30D,1 Wait for 20 units
B $C30E,1 Start Spawning Barriers Right
B $C310,1 Wait for 2 units
B $C311,1 Stop Spawning Barriers 3?
B $C313,1 Wait for 45 units
B $C314,1 Start Spawning Barriers Left
B $C316,1 Wait for 2 units
B $C317,1 Stop Spawning Barriers 3?
B $C319,1 Wait for 43 units
B $C31A,1 Start Spawning Two Barriers
B $C31C,1 Wait for 4 units
B $C31D,1 Stop Spawning Barriers 3?
B $C31F,1 Wait for 76 units
B $C320,1 Disable Car Spawning
B $C322,1 Wait for 6 units
B $C323,1 Set Floating Arrow to Right
B $C325,1 Wait for 2 units
B $C326,1 <Esc> Split
W $C328,2 Left target
W $C32A,2 Right target
b $C32C [Stage 5] Map left object data
B $C32C,1 Tree for 1 units
B $C32D,1 Nothing for 1 units
B $C32E,1 Tree for 1 units
B $C32F,1 Nothing for 1 units
B $C330,1 Tree for 1 units
B $C331,1 Nothing for 1 units
B $C332,1 Tree for 1 units
B $C333,1 Nothing for 1 units
B $C334,1 Tree for 1 units
B $C335,1 Nothing for 1 units
B $C336,1 Tree for 1 units
B $C337,1 Nothing for 1 units
B $C338,1 Tree for 1 units
B $C339,1 Nothing for 1 units
B $C33A,1 Tree for 1 units
B $C33B,1 Nothing for 1 units
B $C33C,1 Tree for 1 units
B $C33D,1 Nothing for 3 units
B $C33E,1 Tree for 1 units
B $C33F,1 Telegraph Pole for 1 units
B $C340,1 Tree for 1 units
B $C341,1 Telegraph Pole for 1 units
B $C342,1 Tree for 1 units
B $C343,1 Telegraph Pole for 1 units
B $C344,1 Tree for 1 units
B $C345,1 Nothing for 3 units
B $C346,1 Telegraph Pole for 1 units
B $C347,1 Nothing for 1 units
B $C348,1 Telegraph Pole for 1 units
B $C349,1 Nothing for 1 units
B $C34A,1 Telegraph Pole for 1 units
B $C34B,1 Nothing for 1 units
B $C34C,1 Telegraph Pole for 1 units
B $C34D,1 Nothing for 1 units
B $C34E,1 Telegraph Pole for 1 units
B $C34F,1 Nothing for 1 units
B $C350,1 Telegraph Pole for 1 units
B $C351,1 Nothing for 1 units
B $C352,1 Telegraph Pole for 1 units
B $C353,1 Nothing for 1 units
B $C354,1 Telegraph Pole for 1 units
B $C355,1 Nothing for 1 units
B $C356,1 Telegraph Pole for 1 units
B $C357,1 Nothing for 1 units
B $C358,1 Tree for 1 units
B $C359,1 Nothing for 1 units
B $C35A,1 Tree for 1 units
B $C35B,1 Nothing for 1 units
B $C35C,1 Tree for 1 units
B $C35D,1 Nothing for 1 units
B $C35E,1 Tree for 1 units
B $C35F,1 Nothing for 3 units
B $C360,1 Bush for 1 units
B $C361,1 Nothing for 1 units
B $C362,1 Bush for 1 units
B $C363,1 Nothing for 1 units
B $C364,1 Bush for 1 units
B $C365,1 Nothing for 1 units
B $C366,1 Bush for 1 units
B $C367,1 Nothing for 1 units
B $C368,1 Bush for 1 units
B $C369,1 Nothing for 1 units
B $C36A,1 Bush for 1 units
B $C36B,1 Nothing for 1 units
B $C36C,1 Bush for 1 units
B $C36D,1 Nothing for 1 units
B $C36E,1 Bush for 1 units
B $C36F,1 Nothing for 1 units
B $C370,1 Bush for 1 units
B $C371,1 Nothing for 1 units
B $C372,1 Bush for 1 units
B $C373,1 Nothing for 1 units
B $C374,1 Bush for 1 units
B $C375,1 Nothing for 1 units
B $C376,1 Bush for 1 units
B $C377,1 Nothing for 1 units
B $C378,1 Bush for 1 units
B $C379,1 Nothing for 1 units
B $C37A,1 Bush for 1 units
B $C37B,1 Nothing for 1 units
B $C37C,1 Bush for 1 units
B $C37D,1 Nothing for 1 units
B $C37E,1 Bush for 1 units
B $C37F,1 Nothing for 1 units
B $C380,1 Bush for 1 units
B $C381,1 Nothing for 1 units
B $C382,1 Bush for 1 units
B $C383,1 Nothing for 1 units
B $C384,1 Bush for 1 units
B $C385,1 Nothing for 1 units
B $C386,1 Bush for 1 units
B $C387,1 Nothing for 3 units
B $C388,1 Unknown (2) for 1 units
B $C389,1 Nothing for 1 units
B $C38A,1 Unknown (2) for 1 units
B $C38B,1 Nothing for 5 units
B $C38C,1 Unknown (2) for 1 units
B $C38D,1 Nothing for 1 units
B $C38E,1 Unknown (2) for 1 units
B $C38F,1 Nothing for 5 units
B $C390,1 Unknown (2) for 1 units
B $C391,1 Nothing for 1 units
B $C392,1 Unknown (2) for 1 units
B $C393,1 Nothing for 5 units
B $C394,1 Unknown (2) for 1 units
B $C395,1 Nothing for 1 units
B $C396,1 Unknown (2) for 1 units
B $C397,1 Nothing for 9 units
B $C398,1 Tree for 1 units
B $C399,1 Nothing for 1 units
B $C39A,1 Tree for 1 units
B $C39B,1 Nothing for 1 units
B $C39C,1 Tree for 1 units
B $C39D,1 Nothing for 1 units
B $C39E,1 Tree for 1 units
B $C39F,1 Nothing for 1 units
B $C3A0,1 Tree for 1 units
B $C3A1,1 Nothing for 1 units
B $C3A2,1 Tree for 1 units
B $C3A3,1 Nothing for 1 units
B $C3A4,1 Tree for 1 units
B $C3A5,1 Nothing for 1 units
B $C3A6,1 Tree for 1 units
B $C3A7,1 Nothing for 1 units
B $C3A8,1 Tree for 1 units
B $C3A9,1 Nothing for 1 units
B $C3AA,1 Tree for 1 units
B $C3AB,1 Nothing for 1 units
B $C3AC,1 Tree for 1 units
B $C3AD,1 Nothing for 3 units
B $C3AE,1 Tree for 1 units
B $C3AF,1 Nothing for 3 units
B $C3B0,1 Tree for 1 units
B $C3B1,1 Nothing for 1 units
B $C3B2,1 Tree for 1 units
B $C3B3,1 Nothing for 3 units
B $C3B4,1 Tree for 1 units
B $C3B5,1 Nothing for 3 units
B $C3B6,1 Tree for 1 units
B $C3B7,1 Nothing for 3 units
B $C3B8,1 Telegraph Pole for 1 units
B $C3B9,1 Nothing for 1 units
B $C3BA,1 Telegraph Pole for 1 units
B $C3BB,1 Nothing for 1 units
B $C3BC,1 Telegraph Pole for 1 units
B $C3BD,1 Nothing for 1 units
B $C3BE,1 Telegraph Pole for 1 units
B $C3BF,1 Nothing for 1 units
B $C3C0,1 Telegraph Pole for 1 units
B $C3C1,1 Nothing for 1 units
B $C3C2,1 Telegraph Pole for 1 units
B $C3C3,1 Nothing for 1 units
B $C3C4,1 Telegraph Pole for 1 units
B $C3C5,1 Nothing for 1 units
B $C3C6,1 Telegraph Pole for 1 units
B $C3C7,1 Nothing for 3 units
B $C3C8,1 Telegraph Pole for 1 units
B $C3C9,1 Nothing for 1 units
B $C3CA,1 Telegraph Pole for 1 units
B $C3CB,1 Nothing for 1 units
B $C3CC,1 <Esc> Split
W $C3CE,2 Left target
W $C3D0,2 Right target
b $C3D2 [Stage 5] Map right object data
B $C3D2,1 Nothing for 1 units
B $C3D3,1 Bush for 1 units
B $C3D4,1 Nothing for 3 units
B $C3D5,1 Bush for 1 units
B $C3D6,1 Nothing for 1 units
B $C3D7,1 Bush for 1 units
B $C3D8,1 Nothing for 4 units
B $C3D9,1 Tree for 1 units
B $C3DA,1 Nothing for 1 units
B $C3DB,1 Tree for 1 units
B $C3DC,1 Nothing for 1 units
B $C3DD,1 Tree for 1 units
B $C3DE,1 Nothing for 1 units
B $C3DF,1 Tree for 1 units
B $C3E0,1 Nothing for 13 units
B $C3E1,1 Tree for 1 units
B $C3E2,1 Nothing for 1 units
B $C3E3,1 Tree for 1 units
B $C3E4,1 Nothing for 1 units
B $C3E5,1 Tree for 1 units
B $C3E6,1 Nothing for 3 units
B $C3E7,1 Tree for 1 units
B $C3E8,1 Nothing for 3 units
B $C3E9,1 Tree for 1 units
B $C3EA,1 Nothing for 1 units
B $C3EB,1 Tree for 1 units
B $C3EC,1 Nothing for 1 units
B $C3ED,1 Tree for 1 units
B $C3EE,1 Nothing for 3 units
B $C3EF,1 Tree for 1 units
B $C3F0,1 Nothing for 3 units
B $C3F1,1 Telegraph Pole for 1 units
B $C3F2,1 Nothing for 1 units
B $C3F3,1 Telegraph Pole for 1 units
B $C3F4,1 Nothing for 3 units
B $C3F5,1 Telegraph Pole for 1 units
B $C3F6,1 Nothing for 3 units
B $C3F7,1 Telegraph Pole for 1 units
B $C3F8,1 Nothing for 3 units
B $C3F9,1 Telegraph Pole for 1 units
B $C3FA,1 Nothing for 3 units
B $C3FB,1 Telegraph Pole for 1 units
B $C3FC,1 Nothing for 3 units
B $C3FD,1 Telegraph Pole for 1 units
B $C3FE,1 Nothing for 3 units
B $C3FF,1 Telegraph Pole for 1 units
B $C400,1 Nothing for 3 units
B $C401,1 Telegraph Pole for 1 units
B $C402,1 Nothing for 1 units
B $C403,1 Telegraph Pole for 1 units
B $C404,1 Nothing for 11 units
B $C405,1 Unknown (2) for 1 units
B $C406,1 Nothing for 1 units
B $C407,1 Unknown (2) for 1 units
B $C408,1 Nothing for 5 units
B $C409,1 Unknown (2) for 1 units
B $C40A,1 Nothing for 1 units
B $C40B,1 Unknown (2) for 1 units
B $C40C,1 Nothing for 5 units
B $C40D,1 Unknown (2) for 1 units
B $C40E,1 Nothing for 1 units
B $C40F,1 Unknown (2) for 1 units
B $C410,1 Nothing for 5 units
B $C411,1 Unknown (2) for 1 units
B $C412,1 Nothing for 1 units
B $C413,1 Unknown (2) for 1 units
B $C414,1 Nothing for 5 units
B $C415,1 Telegraph Pole for 1 units
B $C416,1 Nothing for 1 units
B $C417,1 Telegraph Pole for 1 units
B $C418,1 Nothing for 1 units
B $C419,1 Telegraph Pole for 1 units
B $C41A,1 Nothing for 3 units
B $C41B,1 Telegraph Pole for 1 units
B $C41C,1 Nothing for 1 units
B $C41D,1 Telegraph Pole for 1 units
B $C41E,1 Nothing for 3 units
B $C41F,1 Telegraph Pole for 1 units
B $C420,1 Nothing for 1 units
B $C421,1 Telegraph Pole for 1 units
B $C422,1 Nothing for 1 units
B $C423,1 Telegraph Pole for 1 units
B $C424,1 Nothing for 1 units
B $C425,1 Telegraph Pole for 1 units
B $C426,1 Nothing for 1 units
B $C427,1 Telegraph Pole for 1 units
B $C428,1 Nothing for 1 units
B $C429,1 Telegraph Pole for 1 units
B $C42A,1 Nothing for 1 units
B $C42B,1 Telegraph Pole for 1 units
B $C42C,1 Nothing for 1 units
B $C42D,1 Tree for 1 units
B $C42E,1 Nothing for 1 units
B $C42F,1 Tree for 1 units
B $C430,1 Nothing for 1 units
B $C431,1 Tree for 1 units
B $C432,1 Nothing for 1 units
B $C433,1 Tree for 1 units
B $C434,1 Nothing for 1 units
B $C435,1 Tree for 1 units
B $C436,1 Nothing for 1 units
B $C437,1 Tree for 1 units
B $C438,1 Nothing for 1 units
B $C439,1 Tree for 1 units
B $C43A,1 Nothing for 1 units
B $C43B,1 Tree for 1 units
B $C43C,1 Nothing for 3 units
B $C43D,1 Telegraph Pole for 1 units
B $C43E,1 Tree for 1 units
B $C43F,1 Telegraph Pole for 1 units
B $C440,1 Tree for 1 units
B $C441,1 Telegraph Pole for 1 units
B $C442,1 Nothing for 1 units
B $C443,1 Telegraph Pole for 1 units
B $C444,1 Nothing for 1 units
B $C445,1 Tree for 1 units
B $C446,1 Nothing for 1 units
B $C447,1 Tree for 1 units
B $C448,1 Nothing for 1 units
B $C449,1 Tree for 1 units
B $C44A,1 Nothing for 3 units
B $C44B,1 Tree for 1 units
B $C44C,1 Nothing for 1 units
B $C44D,1 Tree for 1 units
B $C44E,1 Nothing for 3 units
B $C44F,1 <Esc> Split
W $C451,2 Left target
W $C453,2 Right target
b $C65C [Stage 5] Map curvature data
B $C65C,1 Curve Straight for 40 units
B $C65F,1 Curve Left Hard for 41 units
B $C662,1 Curve Straight for 19 units
B $C664,1 Curve Right Hard for 41 units
B $C667,1 Curve Straight for 39 units
B $C66A,1 Curve Left Hard for 21 units
B $C66C,1 Curve Right Hard for 25 units
B $C66E,1 Curve Straight for 54 units
B $C672,1 Curve Right Hard for 24 units
B $C674,1 Curve Right Very Hard for 17 units
B $C676,1 Curve Right Hard for 62 units
B $C67B,1 Curve Straight for 61 units
B $C680,1 <Esc> Loop
W $C682,2 Target
b $C684 [Stage 5] Map height data
B $C684,1 Level Road for 132 units
B $C68D,1 Going Down 3 for 2 units
B $C68E,1 Going Down 5 for 4 units
B $C68F,1 Going Down 7 for 2 units
B $C690,1 Going Down 5 for 1 units
B $C691,1 Going Down 3 for 2 units
B $C692,1 Level Road for 2 units
B $C693,1 Going Up 3 for 1 units
B $C694,1 Going Up 5 for 2 units
B $C695,1 Going Up 7 for 8 units
B $C696,1 Going Down 7 for 4 units
B $C697,1 Going Down 5 for 1 units
B $C698,1 Going Up 5 for 1 units
B $C699,1 Going Up 7 for 6 units
B $C69A,1 Going Down 7 for 4 units
B $C69B,1 Going Down 5 for 1 units
B $C69C,1 Going Down 3 for 1 units
B $C69D,1 Going Down 1 for 3 units
B $C69E,1 Level Road for 111 units
B $C6A6,1 Going Up 3 for 1 units
B $C6A7,1 Going Up 5 for 1 units
B $C6A8,1 Going Up 7 for 1 units
B $C6A9,1 Going Up 5 for 1 units
B $C6AA,1 Going Up 3 for 1 units
B $C6AB,1 Going Down 3 for 1 units
B $C6AC,1 Going Down 5 for 1 units
B $C6AD,1 Going Down 7 for 1 units
B $C6AE,1 Going Down 5 for 1 units
B $C6AF,1 Going Down 3 for 1 units
B $C6B0,1 Level Road for 6 units
B $C6B1,1 Going Up 5 for 1 units
B $C6B2,1 Going Up 7 for 2 units
B $C6B3,1 Going Down 3 for 8 units
B $C6B4,1 Level Road for 43 units
B $C6B7,1 <Esc> Loop
W $C6B9,2 Target
b $C6BB [Stage 5] Map lanes data
B $C6BB,1 4 Lanes              [||||] {00} for 368 units
B $C6BF,1 4-3 Narrowing R      [/|||] {8E} for 36 units
B $C6C1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C6C3,1 4 Lanes              [||||] {00} for 4 units
B $C6C5,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C6C7,1 3 Lanes L            [|||]  {81} for 8 units
B $C6C9,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C6CB,1 4 Lanes              [||||] {00} for 22 units
B $C6CD,1 <Esc> Loop
W $C6CF,2 Target
b $C6D1 [Stage 5] Map hazards data
B $C6D1,1 Wait for 13 units
B $C6D2,1 Start Spawning Two Barriers
B $C6D4,1 Wait for 4 units
B $C6D5,1 Stop Spawning Barriers 3?
B $C6D7,1 Wait for 40 units
B $C6D8,1 Start Spawning Barriers Left
B $C6DA,1 Wait for 2 units
B $C6DB,1 Stop Spawning Barriers 3?
B $C6DD,1 Wait for 91 units
B $C6DE,1 Start Spawning Barriers Left
B $C6E0,1 Wait for 2 units
B $C6E1,1 Stop Spawning Barriers 3?
B $C6E3,1 Wait for 28 units
B $C6E4,1 Start Spawning Barriers Left
B $C6E6,1 Wait for 2 units
B $C6E7,1 Stop Spawning Barriers 3?
B $C6E9,1 Wait for 40 units
B $C6EA,1 <Esc> Loop
W $C6EC,2 Target
b $C6EE [Stage 5] Map left object data
B $C6EE,1 Bush for 1 units
B $C6EF,1 Nothing for 1 units
B $C6F0,1 Bush for 1 units
B $C6F1,1 Nothing for 1 units
B $C6F2,1 Bush for 1 units
B $C6F3,1 Nothing for 1 units
B $C6F4,1 Bush for 1 units
B $C6F5,1 Nothing for 1 units
B $C6F6,1 Bush for 1 units
B $C6F7,1 Nothing for 1 units
B $C6F8,1 Bush for 1 units
B $C6F9,1 Nothing for 1 units
B $C6FA,1 Bush for 1 units
B $C6FB,1 Nothing for 5 units
B $C6FC,1 Tree for 1 units
B $C6FD,1 Nothing for 1 units
B $C6FE,1 Tree for 1 units
B $C6FF,1 Nothing for 1 units
B $C700,1 Tree for 1 units
B $C701,1 Nothing for 3 units
B $C702,1 Tree for 1 units
B $C703,1 Nothing for 1 units
B $C704,1 Tree for 1 units
B $C705,1 Nothing for 3 units
B $C706,1 Tree for 1 units
B $C707,1 Nothing for 3 units
B $C708,1 Tree for 1 units
B $C709,1 Nothing for 3 units
B $C70A,1 Street Lamp for 1 units
B $C70B,1 Nothing for 1 units
B $C70C,1 Street Lamp for 1 units
B $C70D,1 Nothing for 1 units
B $C70E,1 Street Lamp for 1 units
B $C70F,1 Nothing for 1 units
B $C710,1 Street Lamp for 1 units
B $C711,1 Nothing for 1 units
B $C712,1 Street Lamp for 1 units
B $C713,1 Nothing for 1 units
B $C714,1 Street Lamp for 1 units
B $C715,1 Nothing for 1 units
B $C716,1 Street Lamp for 1 units
B $C717,1 Nothing for 1 units
B $C718,1 Street Lamp for 1 units
B $C719,1 Nothing for 1 units
B $C71A,1 Street Lamp for 1 units
B $C71B,1 Nothing for 1 units
B $C71C,1 Street Lamp for 1 units
B $C71D,1 Nothing for 1 units
B $C71E,1 Street Lamp for 1 units
B $C71F,1 Nothing for 1 units
B $C720,1 Street Lamp for 1 units
B $C721,1 Nothing for 1 units
B $C722,1 Street Lamp for 1 units
B $C723,1 Nothing for 1 units
B $C724,1 Street Lamp for 1 units
B $C725,1 Nothing for 1 units
B $C726,1 Street Lamp for 1 units
B $C727,1 Nothing for 1 units
B $C728,1 Street Lamp for 1 units
B $C729,1 Nothing for 1 units
B $C72A,1 Street Lamp for 1 units
B $C72B,1 Nothing for 1 units
B $C72C,1 Street Lamp for 1 units
B $C72D,1 Nothing for 3 units
B $C72E,1 Street Lamp for 1 units
B $C72F,1 Nothing for 3 units
B $C730,1 Street Lamp for 1 units
B $C731,1 Nothing for 17 units
B $C733,1 Street Lamp for 1 units
B $C734,1 Nothing for 1 units
B $C735,1 Street Lamp for 1 units
B $C736,1 Nothing for 1 units
B $C737,1 Street Lamp for 1 units
B $C738,1 Nothing for 1 units
B $C739,1 Street Lamp for 1 units
B $C73A,1 Nothing for 1 units
B $C73B,1 Street Lamp for 1 units
B $C73C,1 Nothing for 1 units
B $C73D,1 Street Lamp for 1 units
B $C73E,1 Nothing for 1 units
B $C73F,1 Street Lamp for 1 units
B $C740,1 Nothing for 1 units
B $C741,1 Street Lamp for 1 units
B $C742,1 Nothing for 1 units
B $C743,1 Street Lamp for 1 units
B $C744,1 Nothing for 1 units
B $C745,1 Street Lamp for 1 units
B $C746,1 Nothing for 1 units
B $C747,1 Street Lamp for 1 units
B $C748,1 Nothing for 1 units
B $C749,1 Street Lamp for 1 units
B $C74A,1 Nothing for 1 units
B $C74B,1 Street Lamp for 1 units
B $C74C,1 Nothing for 1 units
B $C74D,1 Street Lamp for 1 units
B $C74E,1 Nothing for 1 units
B $C74F,1 Street Lamp for 1 units
B $C750,1 Nothing for 1 units
B $C751,1 Street Lamp for 1 units
B $C752,1 Nothing for 9 units
B $C753,1 Street Lamp for 1 units
B $C754,1 Nothing for 1 units
B $C755,1 Street Lamp for 1 units
B $C756,1 Nothing for 1 units
B $C757,1 Street Lamp for 1 units
B $C758,1 Nothing for 1 units
B $C759,1 Street Lamp for 1 units
B $C75A,1 Nothing for 1 units
B $C75B,1 Street Lamp for 1 units
B $C75C,1 Nothing for 1 units
B $C75D,1 Street Lamp for 1 units
B $C75E,1 Nothing for 1 units
B $C75F,1 Street Lamp for 1 units
B $C760,1 Nothing for 1 units
B $C761,1 Street Lamp for 1 units
B $C762,1 Nothing for 1 units
B $C763,1 Street Lamp for 1 units
B $C764,1 Nothing for 1 units
B $C765,1 Street Lamp for 1 units
B $C766,1 Nothing for 1 units
B $C767,1 Street Lamp for 1 units
B $C768,1 Nothing for 1 units
B $C769,1 Street Lamp for 1 units
B $C76A,1 Nothing for 1 units
B $C76B,1 Street Lamp for 1 units
B $C76C,1 Nothing for 1 units
B $C76D,1 Street Lamp for 1 units
B $C76E,1 Nothing for 1 units
B $C76F,1 Street Lamp for 1 units
B $C770,1 Nothing for 1 units
B $C771,1 Street Lamp for 1 units
B $C772,1 Nothing for 1 units
B $C773,1 Street Lamp for 1 units
B $C774,1 Nothing for 1 units
B $C775,1 Street Lamp for 1 units
B $C776,1 Nothing for 1 units
B $C777,1 Street Lamp for 1 units
B $C778,1 Nothing for 1 units
B $C779,1 Street Lamp for 1 units
B $C77A,1 Nothing for 1 units
B $C77B,1 Street Lamp for 1 units
B $C77C,1 Nothing for 1 units
B $C77D,1 Street Lamp for 1 units
B $C77E,1 Nothing for 1 units
B $C77F,1 Street Lamp for 1 units
B $C780,1 Nothing for 1 units
B $C781,1 Street Lamp for 1 units
B $C782,1 Nothing for 1 units
B $C783,1 Street Lamp for 1 units
B $C784,1 Nothing for 1 units
B $C785,1 Street Lamp for 1 units
B $C786,1 Nothing for 1 units
B $C787,1 Street Lamp for 1 units
B $C788,1 Nothing for 1 units
B $C789,1 Street Lamp for 1 units
B $C78A,1 Nothing for 1 units
B $C78B,1 Street Lamp for 1 units
B $C78C,1 Nothing for 1 units
B $C78D,1 Street Lamp for 1 units
B $C78E,1 Nothing for 1 units
B $C78F,1 Street Lamp for 1 units
B $C790,1 Nothing for 5 units
B $C791,1 Tree for 1 units
B $C792,1 Nothing for 1 units
B $C793,1 Tree for 1 units
B $C794,1 Nothing for 1 units
B $C795,1 Tree for 1 units
B $C796,1 Nothing for 1 units
B $C797,1 Tree for 1 units
B $C798,1 Nothing for 1 units
B $C799,1 Bush for 1 units
B $C79A,1 Nothing for 1 units
B $C79B,1 Bush for 1 units
B $C79C,1 Nothing for 1 units
B $C79D,1 Bush for 1 units
B $C79E,1 Nothing for 1 units
B $C79F,1 Bush for 1 units
B $C7A0,1 Nothing for 1 units
B $C7A1,1 <Esc> Loop
W $C7A3,2 Target
b $C7A5 [Stage 5] Map right object data
B $C7A5,1 Nothing for 6 units
B $C7A6,1 Tree for 1 units
B $C7A7,1 Nothing for 1 units
B $C7A8,1 Tree for 1 units
B $C7A9,1 Nothing for 1 units
B $C7AA,1 Tree for 1 units
B $C7AB,1 Nothing for 1 units
B $C7AC,1 Tree for 1 units
B $C7AD,1 Nothing for 5 units
B $C7AE,1 Street Lamp for 1 units
B $C7AF,1 Nothing for 1 units
B $C7B0,1 Street Lamp for 1 units
B $C7B1,1 Nothing for 1 units
B $C7B2,1 Street Lamp for 1 units
B $C7B3,1 Nothing for 1 units
B $C7B4,1 Street Lamp for 1 units
B $C7B5,1 Nothing for 1 units
B $C7B6,1 Street Lamp for 1 units
B $C7B7,1 Nothing for 1 units
B $C7B8,1 Street Lamp for 1 units
B $C7B9,1 Nothing for 1 units
B $C7BA,1 Street Lamp for 1 units
B $C7BB,1 Nothing for 1 units
B $C7BC,1 Street Lamp for 1 units
B $C7BD,1 Nothing for 1 units
B $C7BE,1 Street Lamp for 1 units
B $C7BF,1 Nothing for 1 units
B $C7C0,1 Street Lamp for 1 units
B $C7C1,1 Nothing for 1 units
B $C7C2,1 Street Lamp for 1 units
B $C7C3,1 Nothing for 1 units
B $C7C4,1 Street Lamp for 1 units
B $C7C5,1 Nothing for 1 units
B $C7C6,1 Street Lamp for 1 units
B $C7C7,1 Nothing for 1 units
B $C7C8,1 Street Lamp for 1 units
B $C7C9,1 Nothing for 1 units
B $C7CA,1 Street Lamp for 1 units
B $C7CB,1 Nothing for 1 units
B $C7CC,1 Street Lamp for 1 units
B $C7CD,1 Nothing for 1 units
B $C7CE,1 Street Lamp for 1 units
B $C7CF,1 Nothing for 27 units
B $C7D1,1 Street Lamp for 1 units
B $C7D2,1 Nothing for 1 units
B $C7D3,1 Street Lamp for 1 units
B $C7D4,1 Nothing for 1 units
B $C7D5,1 Street Lamp for 1 units
B $C7D6,1 Nothing for 1 units
B $C7D7,1 Street Lamp for 1 units
B $C7D8,1 Nothing for 1 units
B $C7D9,1 Street Lamp for 1 units
B $C7DA,1 Nothing for 1 units
B $C7DB,1 Street Lamp for 1 units
B $C7DC,1 Nothing for 1 units
B $C7DD,1 Street Lamp for 1 units
B $C7DE,1 Nothing for 1 units
B $C7DF,1 Street Lamp for 1 units
B $C7E0,1 Nothing for 1 units
B $C7E1,1 Street Lamp for 1 units
B $C7E2,1 Nothing for 1 units
B $C7E3,1 Street Lamp for 1 units
B $C7E4,1 Nothing for 1 units
B $C7E5,1 Street Lamp for 1 units
B $C7E6,1 Nothing for 1 units
B $C7E7,1 Street Lamp for 1 units
B $C7E8,1 Nothing for 1 units
B $C7E9,1 Street Lamp for 1 units
B $C7EA,1 Nothing for 1 units
B $C7EB,1 Street Lamp for 1 units
B $C7EC,1 Nothing for 1 units
B $C7ED,1 Street Lamp for 1 units
B $C7EE,1 Nothing for 1 units
B $C7EF,1 Street Lamp for 1 units
B $C7F0,1 Nothing for 1 units
B $C7F1,1 Street Lamp for 1 units
B $C7F2,1 Nothing for 1 units
B $C7F3,1 Street Lamp for 1 units
B $C7F4,1 Nothing for 1 units
B $C7F5,1 Street Lamp for 1 units
B $C7F6,1 Nothing for 1 units
B $C7F7,1 Street Lamp for 1 units
B $C7F8,1 Nothing for 1 units
B $C7F9,1 Street Lamp for 1 units
B $C7FA,1 Nothing for 1 units
B $C7FB,1 Street Lamp for 1 units
B $C7FC,1 Nothing for 1 units
B $C7FD,1 Street Lamp for 1 units
B $C7FE,1 Nothing for 1 units
B $C7FF,1 Street Lamp for 1 units
B $C800,1 Nothing for 1 units
B $C801,1 Street Lamp for 1 units
B $C802,1 Nothing for 1 units
B $C803,1 Street Lamp for 1 units
B $C804,1 Nothing for 47 units
B $C808,1 Tree for 1 units
B $C809,1 Nothing for 1 units
B $C80A,1 Tree for 1 units
B $C80B,1 Nothing for 3 units
B $C80C,1 Tree for 1 units
B $C80D,1 Nothing for 5 units
B $C80E,1 Street Lamp for 1 units
B $C80F,1 Nothing for 1 units
B $C810,1 Street Lamp for 1 units
B $C811,1 Nothing for 1 units
B $C812,1 Street Lamp for 1 units
B $C813,1 Nothing for 1 units
B $C814,1 Street Lamp for 1 units
B $C815,1 Nothing for 1 units
B $C816,1 Street Lamp for 1 units
B $C817,1 Nothing for 1 units
B $C818,1 Street Lamp for 1 units
B $C819,1 Nothing for 1 units
B $C81A,1 Street Lamp for 1 units
B $C81B,1 Nothing for 1 units
B $C81C,1 Street Lamp for 1 units
B $C81D,1 Nothing for 1 units
B $C81E,1 Street Lamp for 1 units
B $C81F,1 Nothing for 1 units
B $C820,1 Street Lamp for 1 units
B $C821,1 Nothing for 1 units
B $C822,1 Street Lamp for 1 units
B $C823,1 Nothing for 5 units
B $C824,1 Tree for 1 units
B $C825,1 Nothing for 1 units
B $C826,1 Tree for 1 units
B $C827,1 Nothing for 3 units
B $C828,1 Tree for 1 units
B $C829,1 Nothing for 1 units
B $C82A,1 <Esc> Loop
W $C82C,2 Target
N $C8E2 LOD
B $C8E2,1 Width (bytes)
B $C8E3,1 Flags
B $C8E4,1 Height (pixels)
W $C8E5,2 Bitmap address
W $C8E7,2 Pre-shifted bitmap address
N $C8E9 LOD
B $C8E9,1 Width (bytes)
B $C8EA,1 Flags
B $C8EB,1 Height (pixels)
W $C8EC,2 Bitmap address
W $C8EE,2 Pre-shifted bitmap address
N $C8F0 LOD
B $C8F0,1 Width (bytes)
B $C8F1,1 Flags
B $C8F2,1 Height (pixels)
W $C8F3,2 Bitmap address
W $C8F5,2 Pre-shifted bitmap address
N $C8F7 LOD
B $C8F7,1 Width (bytes)
B $C8F8,1 Flags
B $C8F9,1 Height (pixels)
W $C8FA,2 Bitmap address
W $C8FC,2 Pre-shifted bitmap address
N $C8FE LOD
B $C8FE,1 Width (bytes)
B $C8FF,1 Flags
B $C900,1 Height (pixels)
W $C901,2 Bitmap address
W $C903,2 Pre-shifted bitmap address
N $C905 LOD
B $C905,1 Width (bytes)
B $C906,1 Flags
B $C907,1 Height (pixels)
W $C908,2 Bitmap address
W $C90A,2 Pre-shifted bitmap address
N $C90C LOD
B $C90C,1 Width (bytes)
B $C90D,1 Flags
B $C90E,1 Height (pixels)
W $C90F,2 Bitmap address
W $C911,2 Pre-shifted bitmap address
N $C913 LOD
B $C913,1 Width (bytes)
B $C914,1 Flags
B $C915,1 Height (pixels)
W $C916,2 Bitmap address
W $C918,2 Pre-shifted bitmap address
N $C91A LOD
B $C91A,1 Width (bytes)
B $C91B,1 Flags
B $C91C,1 Height (pixels)
W $C91D,2 Bitmap address
W $C91F,2 Pre-shifted bitmap address
N $C921 LOD
B $C921,1 Width (bytes)
B $C922,1 Flags
B $C923,1 Height (pixels)
W $C924,2 Bitmap address
W $C926,2 Pre-shifted bitmap address
N $C928 LOD
B $C928,1 Width (bytes)
B $C929,1 Flags
B $C92A,1 Height (pixels)
W $C92B,2 Bitmap address
W $C92D,2 Pre-shifted bitmap address
N $C92F LOD
B $C92F,1 Width (bytes)
B $C930,1 Flags
B $C931,1 Height (pixels)
W $C932,2 Bitmap address
W $C934,2 Pre-shifted bitmap address
N $C936 LOD
B $C936,1 Width (bytes)
B $C937,1 Flags
B $C938,1 Height (pixels)
W $C939,2 Bitmap address
W $C93B,2 Pre-shifted bitmap address
N $C93D LOD
B $C93D,1 Width (bytes)
B $C93E,1 Flags
B $C93F,1 Height (pixels)
W $C940,2 Bitmap address
W $C942,2 Pre-shifted bitmap address
N $C944 LOD
B $C944,1 Width (bytes)
B $C945,1 Flags
B $C946,1 Height (pixels)
W $C947,2 Bitmap address
W $C949,2 Pre-shifted bitmap address
N $C94B LOD
B $C94B,1 Width (bytes)
B $C94C,1 Flags
B $C94D,1 Height (pixels)
W $C94E,2 Bitmap address
W $C950,2 Pre-shifted bitmap address
N $C952 LOD
B $C952,1 Width (bytes)
B $C953,1 Flags
B $C954,1 Height (pixels)
W $C955,2 Bitmap address
W $C957,2 Pre-shifted bitmap address
N $C959 LOD
B $C959,1 Width (bytes)
B $C95A,1 Flags
B $C95B,1 Height (pixels)
W $C95C,2 Bitmap address
W $C95E,2 Pre-shifted bitmap address
B $C960,180,6 Bitmap data 6 bytes x 30
B $CA14,110,5 Bitmap data 5 bytes x 22
B $CA82,45,3 Bitmap data 3 bytes x 15
B $CA82,45,3 Bitmap data 3 bytes x 15
B $CAAF,24,3 Bitmap data 3 bytes x 8
B $CAAF,24,3 Bitmap data 3 bytes x 8
B $CADF,24,3 Pre-shifted bitmap data 3 bytes x 8
B $CB0F,234,6 Bitmap data 6 bytes x 39
B $CBF9,145,5 Bitmap data 5 bytes x 29
B $CC8A,60,3 Bitmap data 3 bytes x 20
B $CC8A,60,3 Bitmap data 3 bytes x 20
B $CCC6,24,2 Bitmap data 2 bytes x 12
B $CCC6,24,2 Bitmap data 2 bytes x 12
B $CCF6,24,2 Pre-shifted bitmap data 2 bytes x 12
B $CCF6,24,2 Pre-shifted bitmap data 2 bytes x 12
B $CD26,180,6 Bitmap data 6 bytes x 30
B $CDDA,80,4 Bitmap data 4 bytes x 20
B $CE2A,39,3 Bitmap data 3 bytes x 13
B $CE2A,39,3 Bitmap data 3 bytes x 13
B $CE51,16,2 Bitmap data 2 bytes x 8
B $CE51,16,2 Bitmap data 2 bytes x 8
B $CE71,16,2 Pre-shifted bitmap data 2 bytes x 8
b $E000 End screen.
