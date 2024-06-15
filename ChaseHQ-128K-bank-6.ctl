> $C000 ; ChaseHQ-128K-bank-6.skool
> $C000 ;
> $C000 ; Bank 6 holds the data for stages 3 and 4.
> $C000 ;
> $C000
@ $C000 org
b $C000 [Stage 3] Horizon graphic
b $C0F0 [Stage 3] Per-stage data
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
w $C10C [Stage 3] Table of addresses of LODs
W $C10C,2 Address of LOD of stone/dust?
W $C10E,2 Address of LOD of stone/dust?
W $C110,2 Address of LOD of car (perp's car)
W $C112,2 Address of LOD of lambo?
W $C114,2 Address of LOD of truck?
W $C116,2 Address of LOD of lambo again?
W $C118,2 Address of LOD of car (generic car)?
b $C11A [Stage 3] Per-stage difficulty settings
B $C11A,1 How often cars spawn. Lower values spawn cars more often.
B $C11B,1 smash related
B $C11C,1 smash related
w $C11D [Stage 3] Per-stage setup data
W $C11D,2 road_pos
W $C11F,2 -> Start stretch, curvature
W $C121,2 -> Start stretch, height
W $C123,2 -> Start stretch, lanes
W $C125,2 -> Start stretch, right-side objects
W $C127,2 -> Start stretch, left-side objects
W $C129,2 -> Start stretch, hazards
w $C12B [Stage 3] Per-stage attract mode data
W $C12B,2 road_pos
W $C12D,2 -> Loop section, curvature
W $C12F,2 -> Loop section, height
W $C131,2 -> Loop section, lanes
W $C133,2 -> Loop section, right-side objects
W $C135,2 -> Loop section, left-side objects
W $C137,2 -> Loop section, hazards
b $C139 [Stage 3] character id, e.g. nancy
w $C13A [Stage 3] perp desc ptr
w $C13C [Stage 3] perp desc ptr
w $C13E [Stage 3] perp desc ptr
w $C140 [Stage 3] perp desc ptr
b $C142 [Stage 3] terminator?
T $C145 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $C16D "EMERGENCY HERE. A GANG OF CHICAGO PUSHERS"
T $C196 "ARE FLEEING TOWARDS THE SUBURBS. THE TARGET"
T $C1C1 "VEHICLE IS A GERMAN SPORTS CAR... OVER."
b $C1E8 [Stage 3] frame delay until first message?
B $C1E9,1 frame delay until next message?
B $C1EA,1 flags
B $C1EB,1 attribute
W $C1EC,2 back buffer addr
W $C1EE,2 attribute addr
T $C1F0 "OK! YOU ARE UNDER ARREST ON"
B $C20B,1 frame delay until next message?
B $C20C,1 flags
B $C20D,1 attribute
W $C20E,2 back buffer addr
W $C210,2 attribute addr
T $C212 "SUSPICION OF SELLING DRUGS"
B $C22C,1 frame delay until next message?
B $C22D,1 flags
N $C22E Hittable hazards
B $C22E,1 attribute
W $C22F,2 back buffer addr
W $C231,2 attribute addr
T $C233 "ro)Pi"
B $C238,1 frame delay until next message?
b $C288 [Stage 3] Map curvature data
B $C288,1 Curve Straight for 65 units
B $C28D,1 Curve Right for 136 units
B $C297,1 Curve Straight for 39 units
B $C29A,1 Curve Left Hard for 51 units
B $C29E,1 Curve Straight for 25 units
B $C2A0,1 Curve Right Hard for 55 units
B $C2A4,1 Curve Straight for 29 units
B $C2A6,1 <Esc> Split
W $C2A8,2 Left target
W $C2AA,2 Right target
b $C2AC [Stage 3] Map height data
B $C2AC,1 Level Road for 400 units
B $C2C7,1 <Esc> Split
W $C2C9,2 Left target
W $C2CB,2 Right target
b $C2CD [Stage 3] Map lanes data
B $C2CD,1 3 Lanes R             [|||] {82} for 60 units
B $C2CF,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C2D1,1 4 Lanes              [||||] {00} for 338 units
B $C2D5,1 <Esc> Split
W $C2D7,2 Left target
W $C2D9,2 Right target
b $C2DB [Stage 3] Map hazards data
B $C2DB,1 Wait for 193 units
B $C2DC,1 Disable Car Spawning
B $C2DE,1 Wait for 3 units
B $C2DF,1 Set Floating Arrow to Left
B $C2E1,1 Wait for 4 units
B $C2E2,1 <Esc> Split
W $C2E4,2 Left target
W $C2E6,2 Right target
b $C2E8 [Stage 3] Map left object data
B $C2E8,1 Street Lamp for 1 units
B $C2E9,1 Nothing for 1 units
B $C2EA,1 Street Lamp for 1 units
B $C2EB,1 Nothing for 1 units
B $C2EC,1 Street Lamp for 1 units
B $C2ED,1 Nothing for 1 units
B $C2EE,1 Street Lamp for 1 units
B $C2EF,1 Nothing for 1 units
B $C2F0,1 Street Lamp for 1 units
B $C2F1,1 Nothing for 1 units
B $C2F2,1 Street Lamp for 1 units
B $C2F3,1 Nothing for 1 units
B $C2F4,1 Street Lamp for 1 units
B $C2F5,1 Nothing for 1 units
B $C2F6,1 Street Lamp for 1 units
B $C2F7,1 Nothing for 1 units
B $C2F8,1 Street Lamp for 1 units
B $C2F9,1 Nothing for 1 units
B $C2FA,1 Street Lamp for 1 units
B $C2FB,1 Nothing for 1 units
B $C2FC,1 Street Lamp for 1 units
B $C2FD,1 Nothing for 1 units
B $C2FE,1 Street Lamp for 1 units
B $C2FF,1 Nothing for 1 units
B $C300,1 Street Lamp for 1 units
B $C301,1 Nothing for 1 units
B $C302,1 Street Lamp for 1 units
B $C303,1 Nothing for 1 units
B $C304,1 Street Lamp for 1 units
B $C305,1 Nothing for 9 units
B $C306,1 Tree for 1 units
B $C307,1 Nothing for 1 units
B $C308,1 Tree for 1 units
B $C309,1 Nothing for 1 units
B $C30A,1 Tree for 1 units
B $C30B,1 Nothing for 1 units
B $C30C,1 Tree for 1 units
B $C30D,1 Nothing for 1 units
B $C30E,1 Tree for 1 units
B $C30F,1 Nothing for 1 units
B $C310,1 Tree for 1 units
B $C311,1 Nothing for 1 units
B $C312,1 Tree for 1 units
B $C313,1 Nothing for 1 units
B $C314,1 Tree for 1 units
B $C315,1 Nothing for 1 units
B $C316,1 Tree for 1 units
B $C317,1 Nothing for 1 units
B $C318,1 Tree for 1 units
B $C319,1 Nothing for 3 units
B $C31A,1 Tree for 1 units
B $C31B,1 Nothing for 3 units
B $C31C,1 Tree for 1 units
B $C31D,1 Nothing for 3 units
B $C31E,1 Tree for 1 units
B $C31F,1 Nothing for 3 units
B $C320,1 Tree for 1 units
B $C321,1 Nothing for 1 units
B $C322,1 Tree for 1 units
B $C323,1 Nothing for 3 units
B $C324,1 Street Lamp for 1 units
B $C325,1 Nothing for 1 units
B $C326,1 Street Lamp for 1 units
B $C327,1 Nothing for 3 units
B $C328,1 Street Lamp for 1 units
B $C329,1 Nothing for 7 units
B $C32A,1 Tree for 1 units
B $C32B,1 Nothing for 1 units
B $C32C,1 Tree for 1 units
B $C32D,1 Nothing for 1 units
B $C32E,1 Tree for 1 units
B $C32F,1 Nothing for 7 units
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
B $C33D,1 Nothing for 1 units
B $C33E,1 Tree for 1 units
B $C33F,1 Nothing for 1 units
B $C340,1 Tree for 1 units
B $C341,1 Nothing for 1 units
B $C342,1 Tree for 1 units
B $C343,1 Nothing for 3 units
B $C344,1 Street Lamp for 1 units
B $C345,1 Nothing for 3 units
B $C346,1 Street Lamp for 1 units
B $C347,1 Nothing for 1 units
B $C348,1 Street Lamp for 1 units
B $C349,1 Nothing for 3 units
B $C34A,1 Street Lamp for 1 units
B $C34B,1 Nothing for 1 units
B $C34C,1 Street Lamp for 1 units
B $C34D,1 Nothing for 7 units
B $C34E,1 Tree for 1 units
B $C34F,1 Nothing for 3 units
B $C350,1 Tree for 1 units
B $C351,1 Nothing for 1 units
B $C352,1 Tree for 1 units
B $C353,1 Nothing for 3 units
B $C354,1 Tree for 1 units
B $C355,1 Nothing for 3 units
B $C356,1 Tree for 1 units
B $C357,1 Nothing for 5 units
B $C358,1 Tree for 1 units
B $C359,1 Nothing for 3 units
B $C35A,1 Bush for 1 units
B $C35B,1 Nothing for 5 units
B $C35C,1 Bush for 1 units
B $C35D,1 Nothing for 9 units
B $C35E,1 Bush for 1 units
B $C35F,1 Nothing for 5 units
B $C360,1 Bush for 1 units
B $C361,1 Nothing for 3 units
B $C362,1 Street Lamp for 1 units
B $C363,1 Nothing for 1 units
B $C364,1 Street Lamp for 1 units
B $C365,1 Nothing for 1 units
B $C366,1 <Esc> Split
W $C368,2 Left target
W $C36A,2 Right target
b $C36C [Stage 3] Map right object data
B $C36C,1 Tree for 1 units
B $C36D,1 Nothing for 3 units
B $C36E,1 Tree for 1 units
B $C36F,1 Nothing for 3 units
B $C370,1 Tree for 1 units
B $C371,1 Nothing for 3 units
B $C372,1 Tree for 1 units
B $C373,1 Nothing for 5 units
B $C374,1 Tree for 1 units
B $C375,1 Nothing for 1 units
B $C376,1 Tree for 1 units
B $C377,1 Nothing for 1 units
B $C378,1 Tree for 1 units
B $C379,1 Nothing for 1 units
B $C37A,1 Tree for 1 units
B $C37B,1 Nothing for 1 units
B $C37C,1 Tree for 1 units
B $C37D,1 Nothing for 1 units
B $C37E,1 Tree for 1 units
B $C37F,1 Nothing for 3 units
B $C380,1 Tree for 1 units
B $C381,1 Nothing for 3 units
B $C382,1 Tree for 1 units
B $C383,1 Nothing for 5 units
B $C384,1 Street Lamp for 1 units
B $C385,1 Nothing for 1 units
B $C386,1 Street Lamp for 1 units
B $C387,1 Nothing for 1 units
B $C388,1 Street Lamp for 1 units
B $C389,1 Nothing for 1 units
B $C38A,1 Street Lamp for 1 units
B $C38B,1 Nothing for 1 units
B $C38C,1 Street Lamp for 1 units
B $C38D,1 Nothing for 3 units
B $C38E,1 Street Lamp for 1 units
B $C38F,1 Nothing for 3 units
B $C390,1 Street Lamp for 1 units
B $C391,1 Nothing for 3 units
B $C392,1 Street Lamp for 1 units
B $C393,1 Nothing for 1 units
B $C394,1 Street Lamp for 1 units
B $C395,1 Nothing for 1 units
B $C396,1 Street Lamp for 1 units
B $C397,1 Nothing for 1 units
B $C398,1 Street Lamp for 1 units
B $C399,1 Nothing for 1 units
B $C39A,1 Street Lamp for 1 units
B $C39B,1 Nothing for 1 units
B $C39C,1 Street Lamp for 1 units
B $C39D,1 Nothing for 1 units
B $C39E,1 Street Lamp for 1 units
B $C39F,1 Nothing for 5 units
B $C3A0,1 Street Lamp for 1 units
B $C3A1,1 Nothing for 3 units
B $C3A2,1 Street Lamp for 1 units
B $C3A3,1 Nothing for 3 units
B $C3A4,1 Street Lamp for 1 units
B $C3A5,1 Nothing for 3 units
B $C3A6,1 Street Lamp for 1 units
B $C3A7,1 Nothing for 1 units
B $C3A8,1 Street Lamp for 1 units
B $C3A9,1 Nothing for 1 units
B $C3AA,1 Street Lamp for 1 units
B $C3AB,1 Nothing for 1 units
B $C3AC,1 Street Lamp for 1 units
B $C3AD,1 Nothing for 5 units
B $C3AE,1 Tree for 1 units
B $C3AF,1 Nothing for 1 units
B $C3B0,1 Tree for 1 units
B $C3B1,1 Nothing for 1 units
B $C3B2,1 Tree for 1 units
B $C3B3,1 Nothing for 1 units
B $C3B4,1 Tree for 1 units
B $C3B5,1 Nothing for 1 units
B $C3B6,1 Tree for 1 units
B $C3B7,1 Nothing for 1 units
B $C3B8,1 Tree for 1 units
B $C3B9,1 Nothing for 1 units
B $C3BA,1 Tree for 1 units
B $C3BB,1 Nothing for 1 units
B $C3BC,1 Tree for 1 units
B $C3BD,1 Nothing for 1 units
B $C3BE,1 Tree for 1 units
B $C3BF,1 Nothing for 1 units
B $C3C0,1 Tree for 1 units
B $C3C1,1 Nothing for 1 units
B $C3C2,1 Tree for 1 units
B $C3C3,1 Nothing for 1 units
B $C3C4,1 Tree for 1 units
B $C3C5,1 Nothing for 1 units
B $C3C6,1 Tree for 1 units
B $C3C7,1 Nothing for 5 units
B $C3C8,1 Tree for 1 units
B $C3C9,1 Nothing for 3 units
B $C3CA,1 Tree for 1 units
B $C3CB,1 Nothing for 1 units
B $C3CC,1 Tree for 1 units
B $C3CD,1 Nothing for 3 units
B $C3CE,1 Tree for 1 units
B $C3CF,1 Nothing for 3 units
B $C3D0,1 Tree for 1 units
B $C3D1,1 Nothing for 9 units
B $C3D2,1 Tree for 1 units
B $C3D3,1 Nothing for 3 units
B $C3D4,1 Tree for 1 units
B $C3D5,1 Nothing for 7 units
B $C3D6,1 Bush for 1 units
B $C3D7,1 Nothing for 5 units
B $C3D8,1 Bush for 1 units
B $C3D9,1 Nothing for 3 units
B $C3DA,1 Street Lamp for 1 units
B $C3DB,1 Nothing for 1 units
B $C3DC,1 Street Lamp for 1 units
B $C3DD,1 Nothing for 1 units
B $C3DE,1 Street Lamp for 1 units
B $C3DF,1 Nothing for 1 units
B $C3E0,1 Street Lamp for 1 units
B $C3E1,1 Nothing for 1 units
B $C3E2,1 Street Lamp for 1 units
B $C3E3,1 Nothing for 1 units
B $C3E4,1 Street Lamp for 1 units
B $C3E5,1 Nothing for 1 units
B $C3E6,1 Street Lamp for 1 units
B $C3E7,1 Nothing for 1 units
B $C3E8,1 Street Lamp for 1 units
B $C3E9,1 Nothing for 1 units
B $C3EA,1 Street Lamp for 1 units
B $C3EB,1 Nothing for 1 units
B $C3EC,1 Street Lamp for 1 units
B $C3ED,1 Nothing for 1 units
B $C3EE,1 <Esc> Split
W $C3F0,2 Left target
W $C3F2,2 Right target
b $C677 [Stage 3] Map curvature data
B $C677,1 Curve Straight for 20 units
B $C679,1 Curve Right Hard for 45 units
B $C67C,1 Curve Right for 9 units
B $C67D,1 Curve Straight for 39 units
B $C680,1 Curve Left for 24 units
B $C682,1 Curve Right for 15 units
B $C683,1 Curve Straight for 35 units
B $C686,1 Curve Left for 25 units
B $C688,1 Curve Left Hard for 92 units
B $C68F,1 Curve Left for 56 units
B $C693,1 Curve Straight for 84 units
B $C699,1 <Esc> Loop
W $C69B,2 Target
b $C69D [Stage 3] Map height data
B $C69D,1 Level Road for 14 units
B $C69E,1 Going Up 1 for 1 units
B $C69F,1 Going Up 3 for 40 units
B $C6A2,1 Going Down 5 for 17 units
B $C6A4,1 Going Down 3 for 6 units
B $C6A5,1 Going Down 1 for 5 units
B $C6A6,1 Level Road for 16 units
B $C6A8,1 Going Down 5 for 2 units
B $C6A9,1 Going Down 7 for 5 units
B $C6AA,1 Going Down 5 for 6 units
B $C6AB,1 Going Down 3 for 2 units
B $C6AC,1 Going Down 1 for 2 units
B $C6AD,1 Going Up 1 for 2 units
B $C6AE,1 Going Up 3 for 2 units
B $C6AF,1 Going Up 5 for 4 units
B $C6B0,1 Going Up 7 for 10 units
B $C6B1,1 Going Down 7 for 1 units
B $C6B2,1 Going Down 5 for 1 units
B $C6B3,1 Going Down 3 for 2 units
B $C6B4,1 Going Down 1 for 1 units
B $C6B5,1 Level Road for 77 units
B $C6BB,1 Going Down 7 for 1 units
B $C6BC,1 Level Road for 171 units
B $C6C8,1 Going Up 3 for 3 units
B $C6C9,1 Going Up 5 for 2 units
B $C6CA,1 Going Up 7 for 4 units
B $C6CB,1 Going Up 5 for 1 units
B $C6CC,1 Going Down 5 for 1 units
B $C6CD,1 Going Down 7 for 3 units
B $C6CE,1 Going Down 5 for 1 units
B $C6CF,1 Going Down 3 for 1 units
B $C6D0,1 Going Up 3 for 1 units
B $C6D1,1 Going Up 5 for 1 units
B $C6D2,1 Going Up 7 for 3 units
B $C6D3,1 Going Down 5 for 1 units
B $C6D4,1 Going Down 3 for 14 units
B $C6D5,1 Going Down 1 for 4 units
B $C6D6,1 Level Road for 16 units
B $C6D8,1 <Esc> Loop
W $C6DA,2 Target
b $C6DC [Stage 3] Map lanes data
B $C6DC,1 4 Lanes              [||||] {00} for 82 units
B $C6DE,1 4-3 Narrowing R      [/|||] {8E} for 8 units
B $C6E0,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C6E2,1 4 Lanes              [||||] {00} for 50 units
B $C6E4,1 4-3 Narrowing R      [/|||] {8E} for 4 units
B $C6E6,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C6E8,1 4 Lanes              [||||] {00} for 10 units
B $C6EA,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C6EC,1 3 Lanes L            [|||]  {81} for 2 units
B $C6EE,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C6F0,1 4 Lanes              [||||] {00} for 250 units
B $C6F2,1 3 Lanes L            [|||]  {81} for 26 units
B $C6F4,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C6F6,1 4 Lanes              [||||] {00} for 2 units
B $C6F8,1 <Esc> Loop
W $C6FA,2 Target
b $C6FC [Stage 3] Map hazards data
B $C6FC,1 Wait for 18 units
B $C6FD,1 Start Spawning Barriers Left
B $C6FF,1 Wait for 2 units
B $C700,1 Stop Spawning Barriers 3?
B $C702,1 Wait for 79 units
B $C703,1 Start Spawning Barriers Right
B $C705,1 Wait for 2 units
B $C706,1 Stop Spawning Barriers 3?
B $C708,1 Wait for 19 units
B $C709,1 Start Spawning Barriers Right
B $C70B,1 Wait for 2 units
B $C70C,1 Stop Spawning Barriers 3?
B $C70E,1 Wait for 85 units
B $C70F,1 Start Spawning Barriers Right
B $C711,1 Wait for 1 units
B $C712,1 Stop Spawning Barriers 3?
B $C714,1 Wait for 8 units
B $C715,1 Unknown command 4
B $C717,1 Wait for 3 units
B $C718,1 Stop Spawning Barriers 3?
B $C71A,1 Wait for 3 units
B $C71B,1 <Esc> Loop
W $C71D,2 Target
b $C71F [Stage 3] Map left object data
B $C71F,1 Nothing for 4 units
B $C720,1 Bush for 1 units
B $C721,1 Nothing for 5 units
B $C722,1 Tree for 1 units
B $C723,1 Nothing for 1 units
B $C724,1 Tree for 1 units
B $C725,1 Nothing for 1 units
B $C726,1 Tree for 1 units
B $C727,1 Nothing for 1 units
B $C728,1 Tree for 1 units
B $C729,1 Nothing for 1 units
B $C72A,1 Tree for 1 units
B $C72B,1 Nothing for 1 units
B $C72C,1 Tree for 1 units
B $C72D,1 Nothing for 1 units
B $C72E,1 Tree for 1 units
B $C72F,1 Nothing for 1 units
B $C730,1 Tree for 1 units
B $C731,1 Nothing for 1 units
B $C732,1 Tree for 1 units
B $C733,1 Nothing for 1 units
B $C734,1 Tree for 1 units
B $C735,1 Nothing for 1 units
B $C736,1 Tree for 1 units
B $C737,1 Nothing for 1 units
B $C738,1 Tree for 1 units
B $C739,1 Nothing for 1 units
B $C73A,1 Tree for 1 units
B $C73B,1 Nothing for 3 units
B $C73C,1 Tree for 1 units
B $C73D,1 Nothing for 3 units
B $C73E,1 Tree for 1 units
B $C73F,1 Nothing for 1 units
B $C740,1 Tree for 1 units
B $C741,1 Nothing for 3 units
B $C742,1 Street Lamp for 1 units
B $C743,1 Nothing for 1 units
B $C744,1 Street Lamp for 1 units
B $C745,1 Nothing for 1 units
B $C746,1 Street Lamp for 1 units
B $C747,1 Nothing for 1 units
B $C748,1 Street Lamp for 1 units
B $C749,1 Nothing for 1 units
B $C74A,1 Street Lamp for 1 units
B $C74B,1 Nothing for 1 units
B $C74C,1 Street Lamp for 1 units
B $C74D,1 Nothing for 3 units
B $C74E,1 Street Lamp for 1 units
B $C74F,1 Nothing for 3 units
B $C750,1 Street Lamp for 1 units
B $C751,1 Nothing for 5 units
B $C752,1 Street Lamp for 1 units
B $C753,1 Nothing for 1 units
B $C754,1 Street Lamp for 1 units
B $C755,1 Nothing for 1 units
B $C756,1 Street Lamp for 1 units
B $C757,1 Nothing for 1 units
B $C758,1 Street Lamp for 1 units
B $C759,1 Nothing for 1 units
B $C75A,1 Street Lamp for 1 units
B $C75B,1 Nothing for 3 units
B $C75C,1 Tree for 1 units
B $C75D,1 Nothing for 1 units
B $C75E,1 Tree for 1 units
B $C75F,1 Nothing for 1 units
B $C760,1 Tree for 1 units
B $C761,1 Nothing for 1 units
B $C762,1 Tree for 1 units
B $C763,1 Nothing for 1 units
B $C764,1 Tree for 1 units
B $C765,1 Nothing for 1 units
B $C766,1 Tree for 1 units
B $C767,1 Nothing for 9 units
B $C768,1 Bush for 1 units
B $C769,1 Nothing for 21 units
B $C76B,1 Bush for 1 units
B $C76C,1 Nothing for 11 units
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
B $C77C,1 Nothing for 7 units
B $C77D,1 Tree for 1 units
B $C77E,1 Nothing for 1 units
B $C77F,1 Tree for 1 units
B $C780,1 Nothing for 1 units
B $C781,1 Tree for 1 units
B $C782,1 Nothing for 1 units
B $C783,1 Tree for 1 units
B $C784,1 Nothing for 1 units
B $C785,1 Tree for 1 units
B $C786,1 Nothing for 1 units
B $C787,1 Tree for 1 units
B $C788,1 Nothing for 1 units
B $C789,1 Tree for 1 units
B $C78A,1 Nothing for 1 units
B $C78B,1 Tree for 1 units
B $C78C,1 Nothing for 1 units
B $C78D,1 Tree for 1 units
B $C78E,1 Nothing for 1 units
B $C78F,1 Tree for 1 units
B $C790,1 Nothing for 1 units
B $C791,1 Tree for 1 units
B $C792,1 Nothing for 1 units
B $C793,1 Tree for 1 units
B $C794,1 Nothing for 1 units
B $C795,1 Tree for 1 units
B $C796,1 Nothing for 1 units
B $C797,1 Tree for 1 units
B $C798,1 Nothing for 1 units
B $C799,1 Tree for 1 units
B $C79A,1 Nothing for 1 units
B $C79B,1 Tree for 1 units
B $C79C,1 Nothing for 5 units
B $C79D,1 Bush for 1 units
B $C79E,1 Nothing for 5 units
B $C79F,1 Bush for 1 units
B $C7A0,1 Nothing for 9 units
B $C7A1,1 Street Lamp for 1 units
B $C7A2,1 Nothing for 1 units
B $C7A3,1 Street Lamp for 1 units
B $C7A4,1 Nothing for 1 units
B $C7A5,1 Street Lamp for 1 units
B $C7A6,1 Nothing for 1 units
B $C7A7,1 Street Lamp for 1 units
B $C7A8,1 Nothing for 3 units
B $C7A9,1 <Esc> Loop
W $C7AB,2 Target
b $C7AD [Stage 3] Map right object data
B $C7AD,1 Nothing for 4 units
B $C7AE,1 Bush for 1 units
B $C7AF,1 Nothing for 25 units
B $C7B1,1 Street Lamp for 1 units
B $C7B2,1 Nothing for 1 units
B $C7B3,1 Street Lamp for 1 units
B $C7B4,1 Nothing for 1 units
B $C7B5,1 Street Lamp for 1 units
B $C7B6,1 Nothing for 1 units
B $C7B7,1 Street Lamp for 1 units
B $C7B8,1 Nothing for 1 units
B $C7B9,1 Street Lamp for 1 units
B $C7BA,1 Nothing for 1 units
B $C7BB,1 Street Lamp for 1 units
B $C7BC,1 Nothing for 1 units
B $C7BD,1 Street Lamp for 1 units
B $C7BE,1 Nothing for 1 units
B $C7BF,1 Street Lamp for 1 units
B $C7C0,1 Nothing for 1 units
B $C7C1,1 Street Lamp for 1 units
B $C7C2,1 Nothing for 1 units
B $C7C3,1 Street Lamp for 1 units
B $C7C4,1 Nothing for 1 units
B $C7C5,1 Street Lamp for 1 units
B $C7C6,1 Nothing for 1 units
B $C7C7,1 Street Lamp for 1 units
B $C7C8,1 Nothing for 1 units
B $C7C9,1 Street Lamp for 1 units
B $C7CA,1 Nothing for 9 units
B $C7CB,1 Street Lamp for 1 units
B $C7CC,1 Nothing for 1 units
B $C7CD,1 Street Lamp for 1 units
B $C7CE,1 Nothing for 1 units
B $C7CF,1 Street Lamp for 1 units
B $C7D0,1 Nothing for 1 units
B $C7D1,1 Street Lamp for 1 units
B $C7D2,1 Nothing for 1 units
B $C7D3,1 Street Lamp for 1 units
B $C7D4,1 Nothing for 1 units
B $C7D5,1 Street Lamp for 1 units
B $C7D6,1 Nothing for 1 units
B $C7D7,1 Street Lamp for 1 units
B $C7D8,1 Nothing for 3 units
B $C7D9,1 Street Lamp for 1 units
B $C7DA,1 Nothing for 3 units
B $C7DB,1 Street Lamp for 1 units
B $C7DC,1 Nothing for 17 units
B $C7DE,1 Tree for 1 units
B $C7DF,1 Nothing for 1 units
B $C7E0,1 Tree for 1 units
B $C7E1,1 Nothing for 1 units
B $C7E2,1 Tree for 1 units
B $C7E3,1 Nothing for 1 units
B $C7E4,1 Tree for 1 units
B $C7E5,1 Nothing for 1 units
B $C7E6,1 Tree for 1 units
B $C7E7,1 Nothing for 1 units
B $C7E8,1 Tree for 1 units
B $C7E9,1 Nothing for 1 units
B $C7EA,1 Tree for 1 units
B $C7EB,1 Nothing for 1 units
B $C7EC,1 Tree for 1 units
B $C7ED,1 Nothing for 1 units
B $C7EE,1 Tree for 1 units
B $C7EF,1 Nothing for 1 units
B $C7F0,1 Tree for 1 units
B $C7F1,1 Nothing for 1 units
B $C7F2,1 Tree for 1 units
B $C7F3,1 Nothing for 1 units
B $C7F4,1 Tree for 1 units
B $C7F5,1 Nothing for 1 units
B $C7F6,1 Tree for 1 units
B $C7F7,1 Nothing for 1 units
B $C7F8,1 Tree for 1 units
B $C7F9,1 Nothing for 1 units
B $C7FA,1 Tree for 1 units
B $C7FB,1 Nothing for 1 units
B $C7FC,1 Tree for 1 units
B $C7FD,1 Nothing for 5 units
B $C7FE,1 Tree for 1 units
B $C7FF,1 Nothing for 1 units
B $C800,1 Tree for 1 units
B $C801,1 Nothing for 3 units
B $C802,1 Tree for 1 units
B $C803,1 Nothing for 1 units
B $C804,1 Tree for 1 units
B $C805,1 Nothing for 1 units
B $C806,1 Tree for 1 units
B $C807,1 Nothing for 1 units
B $C808,1 Tree for 1 units
B $C809,1 Nothing for 1 units
B $C80A,1 Tree for 1 units
B $C80B,1 Nothing for 7 units
B $C80C,1 Tree for 1 units
B $C80D,1 Nothing for 1 units
B $C80E,1 Tree for 1 units
B $C80F,1 Nothing for 1 units
B $C810,1 Tree for 1 units
B $C811,1 Nothing for 1 units
B $C812,1 Tree for 1 units
B $C813,1 Nothing for 1 units
B $C814,1 Tree for 1 units
B $C815,1 Nothing for 1 units
B $C816,1 Tree for 1 units
B $C817,1 Nothing for 1 units
B $C818,1 Tree for 1 units
B $C819,1 Nothing for 1 units
B $C81A,1 Tree for 1 units
B $C81B,1 Nothing for 5 units
B $C81C,1 Tree for 1 units
B $C81D,1 Nothing for 9 units
B $C81E,1 Tree for 1 units
B $C81F,1 Nothing for 1 units
B $C820,1 Tree for 1 units
B $C821,1 Nothing for 1 units
B $C822,1 Tree for 1 units
B $C823,1 Nothing for 7 units
B $C824,1 Bush for 1 units
B $C825,1 Nothing for 13 units
B $C826,1 Bush for 1 units
B $C827,1 Nothing for 5 units
B $C828,1 <Esc> Loop
W $C82A,2 Target
N $C8E0 LOD
B $C8E0,1 Width (bytes)
B $C8E1,1 Flags
B $C8E2,1 Height (pixels)
W $C8E3,2 Bitmap address
W $C8E5,2 Pre-shifted bitmap address
N $C8E7 LOD
B $C8E7,1 Width (bytes)
B $C8E8,1 Flags
B $C8E9,1 Height (pixels)
W $C8EA,2 Bitmap address
W $C8EC,2 Pre-shifted bitmap address
N $C8EE LOD
B $C8EE,1 Width (bytes)
B $C8EF,1 Flags
B $C8F0,1 Height (pixels)
W $C8F1,2 Bitmap address
W $C8F3,2 Pre-shifted bitmap address
N $C8F5 LOD
B $C8F5,1 Width (bytes)
B $C8F6,1 Flags
B $C8F7,1 Height (pixels)
W $C8F8,2 Bitmap address
W $C8FA,2 Pre-shifted bitmap address
N $C8FC LOD
B $C8FC,1 Width (bytes)
B $C8FD,1 Flags
B $C8FE,1 Height (pixels)
W $C8FF,2 Bitmap address
W $C901,2 Pre-shifted bitmap address
N $C903 LOD
B $C903,1 Width (bytes)
B $C904,1 Flags
B $C905,1 Height (pixels)
W $C906,2 Bitmap address
W $C908,2 Pre-shifted bitmap address
N $C90A LOD
B $C90A,1 Width (bytes)
B $C90B,1 Flags
B $C90C,1 Height (pixels)
W $C90D,2 Bitmap address
W $C90F,2 Pre-shifted bitmap address
N $C911 LOD
B $C911,1 Width (bytes)
B $C912,1 Flags
B $C913,1 Height (pixels)
W $C914,2 Bitmap address
W $C916,2 Pre-shifted bitmap address
N $C918 LOD
B $C918,1 Width (bytes)
B $C919,1 Flags
B $C91A,1 Height (pixels)
W $C91B,2 Bitmap address
W $C91D,2 Pre-shifted bitmap address
N $C91F LOD
B $C91F,1 Width (bytes)
B $C920,1 Flags
B $C921,1 Height (pixels)
W $C922,2 Bitmap address
W $C924,2 Pre-shifted bitmap address
N $C926 LOD
B $C926,1 Width (bytes)
B $C927,1 Flags
B $C928,1 Height (pixels)
W $C929,2 Bitmap address
W $C92B,2 Pre-shifted bitmap address
N $C92D LOD
B $C92D,1 Width (bytes)
B $C92E,1 Flags
B $C92F,1 Height (pixels)
W $C930,2 Bitmap address
W $C932,2 Pre-shifted bitmap address
N $C934 LOD
B $C934,1 Width (bytes)
B $C935,1 Flags
B $C936,1 Height (pixels)
W $C937,2 Bitmap address
W $C939,2 Pre-shifted bitmap address
N $C93B LOD
B $C93B,1 Width (bytes)
B $C93C,1 Flags
B $C93D,1 Height (pixels)
W $C93E,2 Bitmap address
W $C940,2 Pre-shifted bitmap address
N $C942 LOD
B $C942,1 Width (bytes)
B $C943,1 Flags
B $C944,1 Height (pixels)
W $C945,2 Bitmap address
W $C947,2 Pre-shifted bitmap address
N $C949 LOD
B $C949,1 Width (bytes)
B $C94A,1 Flags
B $C94B,1 Height (pixels)
W $C94C,2 Bitmap address
W $C94E,2 Pre-shifted bitmap address
N $C950 LOD
B $C950,1 Width (bytes)
B $C951,1 Flags
B $C952,1 Height (pixels)
W $C953,2 Bitmap address
W $C955,2 Pre-shifted bitmap address
N $C957 LOD
B $C957,1 Width (bytes)
B $C958,1 Flags
B $C959,1 Height (pixels)
W $C95A,2 Bitmap address
W $C95C,2 Pre-shifted bitmap address
B $C95E,180,6 Bitmap data 6 bytes x 30
B $CA12,80,4 Bitmap data 4 bytes x 20
B $CA62,39,3 Bitmap data 3 bytes x 13
B $CA62,39,3 Bitmap data 3 bytes x 13
B $CA89,16,2 Bitmap data 2 bytes x 8
B $CA89,16,2 Bitmap data 2 bytes x 8
B $CAA9,16,2 Pre-shifted bitmap data 2 bytes x 8
B $CAC9,174,6 Bitmap data 6 bytes x 29
B $CB77,76,4 Bitmap data 4 bytes x 19
B $CBC3,42,3 Bitmap data 3 bytes x 14
B $CBC3,42,3 Bitmap data 3 bytes x 14
B $CBED,16,2 Bitmap data 2 bytes x 8
B $CBED,16,2 Bitmap data 2 bytes x 8
B $CC0D,16,2 Pre-shifted bitmap data 2 bytes x 8
B $CC0D,16,2 Pre-shifted bitmap data 2 bytes x 8
B $CC2D,186,6 Bitmap data 6 bytes x 31
B $CCE7,110,5 Bitmap data 5 bytes x 22
B $CD55,48,3 Bitmap data 3 bytes x 16
B $CD55,48,3 Bitmap data 3 bytes x 16
B $CD85,27,3 Bitmap data 3 bytes x 9
B $CD85,27,3 Bitmap data 3 bytes x 9
B $CDBB,27,3 Pre-shifted bitmap data 3 bytes x 9
B $CDBB,27,3 Pre-shifted bitmap data 3 bytes x 9
N $CDF1 LOD
B $CDF1,1 Width (bytes)
B $CDF2,1 Flags
B $CDF3,1 Height (pixels)
W $CDF4,2 Bitmap address
W $CDF6,2 Pre-shifted bitmap address
N $CDF8 LOD
B $CDF8,1 Width (bytes)
B $CDF9,1 Flags
B $CDFA,1 Height (pixels)
W $CDFB,2 Bitmap address
W $CDFD,2 Pre-shifted bitmap address
N $CDFF LOD
B $CDFF,1 Width (bytes)
B $CE00,1 Flags
B $CE01,1 Height (pixels)
W $CE02,2 Bitmap address
W $CE04,2 Pre-shifted bitmap address
N $CE06 LOD
B $CE06,1 Width (bytes)
B $CE07,1 Flags
B $CE08,1 Height (pixels)
W $CE09,2 Bitmap address
W $CE0B,2 Pre-shifted bitmap address
N $CE0D LOD
B $CE0D,1 Width (bytes)
B $CE0E,1 Flags
B $CE0F,1 Height (pixels)
W $CE10,2 Bitmap address
W $CE12,2 Pre-shifted bitmap address
N $CE14 LOD
B $CE14,1 Width (bytes)
B $CE15,1 Flags
B $CE16,1 Height (pixels)
W $CE17,2 Bitmap address
W $CE19,2 Pre-shifted bitmap address
N $CE1B LOD
B $CE1B,1 Width (bytes)
B $CE1C,1 Flags
B $CE1D,1 Height (pixels)
W $CE1E,2 Bitmap address
W $CE20,2 Pre-shifted bitmap address
N $CE22 LOD
B $CE22,1 Width (bytes)
B $CE23,1 Flags
B $CE24,1 Height (pixels)
W $CE25,2 Bitmap address
W $CE27,2 Pre-shifted bitmap address
N $CE29 LOD
B $CE29,1 Width (bytes)
B $CE2A,1 Flags
B $CE2B,1 Height (pixels)
W $CE2C,2 Bitmap address
W $CE2E,2 Pre-shifted bitmap address
N $CE30 LOD
B $CE30,1 Width (bytes)
B $CE31,1 Flags
B $CE32,1 Height (pixels)
W $CE33,2 Bitmap address
W $CE35,2 Pre-shifted bitmap address
N $CE37 LOD
B $CE37,1 Width (bytes)
B $CE38,1 Flags
B $CE39,1 Height (pixels)
W $CE3A,2 Bitmap address
W $CE3C,2 Pre-shifted bitmap address
N $CE3E LOD
B $CE3E,1 Width (bytes)
B $CE3F,1 Flags
B $CE40,1 Height (pixels)
W $CE41,2 Bitmap address
W $CE43,2 Pre-shifted bitmap address
B $CE45,10,2 Bitmap data 2 bytes x 5
B $CE59,8,2 Bitmap data 2 bytes x 4
B $CE69,8,2 Pre-shifted bitmap data 2 bytes x 4
B $CE79,6,2 Bitmap data 2 bytes x 3
B $CE85,6,2 Pre-shifted bitmap data 2 bytes x 3
B $CE91,2,1 Bitmap data 1 bytes x 2
B $CE91,2,1 Bitmap data 1 bytes x 2
B $CE95,2,1 Pre-shifted bitmap data 1 bytes x 2
B $CE95,2,1 Pre-shifted bitmap data 1 bytes x 2
B $CE99,1,1 Bitmap data 1 bytes x 1
B $CE9B,1,1 Pre-shifted bitmap data 1 bytes x 1
B $CE9D,1,1 Bitmap data 1 bytes x 1
B $CE9D,1,1 Bitmap data 1 bytes x 1
B $CE9D,1,1 Bitmap data 1 bytes x 1
B $CE9D,1,1 Bitmap data 1 bytes x 1
B $CE9D,1,1 Bitmap data 1 bytes x 1
B $CE9D,1,1 Bitmap data 1 bytes x 1
B $CE9F,1,1 Pre-shifted bitmap data 1 bytes x 1
B $CE9F,1,1 Pre-shifted bitmap data 1 bytes x 1
B $CE9F,1,1 Pre-shifted bitmap data 1 bytes x 1
B $CE9F,1,1 Pre-shifted bitmap data 1 bytes x 1
B $CE9F,1,1 Pre-shifted bitmap data 1 bytes x 1
B $CE9F,1,1 Pre-shifted bitmap data 1 bytes x 1
b $E000 [Stage 4] Horizon graphic
b $E0F0 [Stage 4] Per-stage data
W $E0F0,2 Address of PERP's mugshot attributes
W $E0F2,2 Address of PERP's mugshot bitmap
W $E0F4,2 Screen attributes used for the ground colour (a pair of matching bytes)
W $E0F6,2 Address of table of LODs for tumbleweeds, barriers.
W $E0F8,2 points to objects?
W $E0FA,2 points to objects?
W $E0FC,2 points to objects?
W $E0FE,2 Address of turn sign arg and handler address
W $E100,2 Address of graphics entry 10
W $E102,2 Address of graphics entry 12
W $E104,2 Address of Nancy's perp description
W $E106,2 Address of arrest messages
W $E108,2 Helicopter related 1
W $E10A,2 Helicopter related 2
w $E10C [Stage 4] Table of addresses of LODs
W $E10C,2 Address of LOD of stone/dust?
W $E10E,2 Address of LOD of stone/dust?
W $E110,2 Address of LOD of car (perp's car)
W $E112,2 Address of LOD of lambo?
W $E114,2 Address of LOD of truck?
W $E116,2 Address of LOD of lambo again?
W $E118,2 Address of LOD of car (generic car)?
b $E11A [Stage 4] Per-stage difficulty settings
B $E11A,1 How often cars spawn. Lower values spawn cars more often.
B $E11B,1 smash related
B $E11C,1 smash related
w $E11D [Stage 4] Per-stage setup data
W $E11D,2 road_pos
W $E11F,2 -> Start stretch, curvature
W $E121,2 -> Start stretch, height
W $E123,2 -> Start stretch, lanes
W $E125,2 -> Start stretch, right-side objects
W $E127,2 -> Start stretch, left-side objects
W $E129,2 -> Start stretch, hazards
w $E12B [Stage 4] Per-stage attract mode data
W $E12B,2 road_pos
W $E12D,2 -> Loop section, curvature
W $E12F,2 -> Loop section, height
W $E131,2 -> Loop section, lanes
W $E133,2 -> Loop section, right-side objects
W $E135,2 -> Loop section, left-side objects
W $E137,2 -> Loop section, hazards
b $E139 [Stage 4] character id, e.g. nancy
w $E13A [Stage 4] perp desc ptr
w $E13C [Stage 4] perp desc ptr
w $E13E [Stage 4] perp desc ptr
w $E140 [Stage 4] perp desc ptr
b $E142 [Stage 4] terminator?
T $E145 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $E16D "EMERGENCY HERE, THE L.A. KIDNAPPER IS"
T $E192 "SPEEDING TOWARDS THE OUTSKIRTS OF TOWN"
T $E1B8 "IN A BLUE TWO SEATER... OVER."
b $E1D5 [Stage 4] frame delay until first message?
B $E1D6,1 frame delay until next message?
B $E1D7,1 flags
B $E1D8,1 attribute
W $E1D9,2 back buffer addr
W $E1DB,2 attribute addr
T $E1DD "OK! YOU ARE UNDER ARREST ON"
B $E1F8,1 frame delay until next message?
B $E1F9,1 flags
B $E1FA,1 attribute
W $E1FB,2 back buffer addr
W $E1FD,2 attribute addr
T $E1FF "SUSPICION OF KIDNAPPING"
B $E216,1 frame delay until next message?
B $E217,1 flags
N $E218 Hittable hazards
B $E218,1 attribute
W $E219,2 back buffer addr
W $E21B,2 attribute addr
T $E21D "ro)Pi"
B $E222,1 frame delay until next message?
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
W $E2C0,2 Left target
W $E2C2,2 Right target
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
W $E300,2 Left target
W $E302,2 Right target
b $E304 [Stage 4] Map lanes data
B $E304,1 4 Lanes              [||||] {00} for 400 units
B $E308,1 <Esc> Split
W $E30A,2 Left target
W $E30C,2 Right target
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
W $E332,2 Left target
W $E334,2 Right target
b $E336 [Stage 4] Map left object data
B $E336,1 Telegraph Pole for 1 units
B $E337,1 Nothing for 1 units
B $E338,1 Telegraph Pole for 1 units
B $E339,1 Nothing for 1 units
B $E33A,1 Telegraph Pole for 1 units
B $E33B,1 Nothing for 1 units
B $E33C,1 Telegraph Pole for 1 units
B $E33D,1 Nothing for 1 units
B $E33E,1 Telegraph Pole for 1 units
B $E33F,1 Nothing for 1 units
B $E340,1 Telegraph Pole for 1 units
B $E341,1 Nothing for 1 units
B $E342,1 Telegraph Pole for 1 units
B $E343,1 Nothing for 1 units
B $E344,1 Telegraph Pole for 1 units
B $E345,1 Nothing for 1 units
B $E346,1 Telegraph Pole for 1 units
B $E347,1 Nothing for 1 units
B $E348,1 Telegraph Pole for 1 units
B $E349,1 Nothing for 1 units
B $E34A,1 Telegraph Pole for 1 units
B $E34B,1 Bush for 1 units
B $E34C,1 Telegraph Pole for 1 units
B $E34D,1 Bush for 1 units
B $E34E,1 Telegraph Pole for 1 units
B $E34F,1 Nothing for 1 units
B $E350,1 Telegraph Pole for 1 units
B $E351,1 Nothing for 1 units
B $E352,1 Telegraph Pole for 1 units
B $E353,1 Nothing for 2 units
B $E354,1 Bush for 1 units
B $E355,1 Nothing for 2 units
B $E356,1 Telegraph Pole for 1 units
B $E357,1 Nothing for 1 units
B $E358,1 Telegraph Pole for 1 units
B $E359,1 Nothing for 1 units
B $E35A,1 Telegraph Pole for 1 units
B $E35B,1 Nothing for 3 units
B $E35C,1 Telegraph Pole for 1 units
B $E35D,1 Nothing for 3 units
B $E35E,1 Telegraph Pole for 1 units
B $E35F,1 Nothing for 11 units
B $E360,1 Turn Sign, Pointing Left for 1 units
B $E361,1 Nothing for 1 units
B $E362,1 Turn Sign, Pointing Left for 1 units
B $E363,1 Nothing for 1 units
B $E364,1 Turn Sign, Pointing Left for 1 units
B $E365,1 Nothing for 1 units
B $E366,1 Turn Sign, Pointing Left for 1 units
B $E367,1 Nothing for 1 units
B $E368,1 Turn Sign, Pointing Left for 1 units
B $E369,1 Nothing for 3 units
B $E36A,1 Telegraph Pole for 1 units
B $E36B,1 Nothing for 1 units
B $E36C,1 Telegraph Pole for 1 units
B $E36D,1 Nothing for 1 units
B $E36E,1 Telegraph Pole for 1 units
B $E36F,1 Nothing for 1 units
B $E370,1 Telegraph Pole for 1 units
B $E371,1 Nothing for 1 units
B $E372,1 Telegraph Pole for 1 units
B $E373,1 Nothing for 1 units
B $E374,1 Telegraph Pole for 1 units
B $E375,1 Nothing for 1 units
B $E376,1 Telegraph Pole for 1 units
B $E377,1 Nothing for 1 units
B $E378,1 Telegraph Pole for 1 units
B $E379,1 Nothing for 1 units
B $E37A,1 Telegraph Pole for 1 units
B $E37B,1 Nothing for 1 units
B $E37C,1 Telegraph Pole for 1 units
B $E37D,1 Nothing for 3 units
B $E37E,1 Turn Sign, Pointing Right for 1 units
B $E37F,1 Nothing for 1 units
B $E380,1 Turn Sign, Pointing Right for 1 units
B $E381,1 Nothing for 1 units
B $E382,1 Turn Sign, Pointing Right for 1 units
B $E383,1 Nothing for 1 units
B $E384,1 Turn Sign, Pointing Right for 1 units
B $E385,1 Nothing for 1 units
B $E386,1 Turn Sign, Pointing Right for 1 units
B $E387,1 Nothing for 1 units
B $E388,1 Turn Sign, Pointing Right for 1 units
B $E389,1 Nothing for 1 units
B $E38A,1 Turn Sign, Pointing Right for 1 units
B $E38B,1 Nothing for 1 units
B $E38C,1 Turn Sign, Pointing Right for 1 units
B $E38D,1 Nothing for 5 units
B $E38E,1 Street Lamp for 1 units
B $E38F,1 Nothing for 1 units
B $E390,1 Street Lamp for 1 units
B $E391,1 Nothing for 1 units
B $E392,1 Street Lamp for 1 units
B $E393,1 Nothing for 1 units
B $E394,1 Street Lamp for 1 units
B $E395,1 Nothing for 1 units
B $E396,1 Street Lamp for 1 units
B $E397,1 Nothing for 1 units
B $E398,1 Street Lamp for 1 units
B $E399,1 Nothing for 1 units
B $E39A,1 Street Lamp for 1 units
B $E39B,1 Nothing for 5 units
B $E39C,1 Turn Sign, Pointing Left for 1 units
B $E39D,1 Nothing for 1 units
B $E39E,1 Turn Sign, Pointing Left for 1 units
B $E39F,1 Nothing for 1 units
B $E3A0,1 Turn Sign, Pointing Left for 1 units
B $E3A1,1 Nothing for 1 units
B $E3A2,1 Turn Sign, Pointing Left for 1 units
B $E3A3,1 Nothing for 1 units
B $E3A4,1 Turn Sign, Pointing Left for 1 units
B $E3A5,1 Nothing for 13 units
B $E3A6,1 Telegraph Pole for 1 units
B $E3A7,1 Nothing for 1 units
B $E3A8,1 Telegraph Pole for 1 units
B $E3A9,1 Nothing for 1 units
B $E3AA,1 Telegraph Pole for 1 units
B $E3AB,1 Nothing for 1 units
B $E3AC,1 Telegraph Pole for 1 units
B $E3AD,1 Nothing for 1 units
B $E3AE,1 Telegraph Pole for 1 units
B $E3AF,1 Nothing for 3 units
B $E3B0,1 Telegraph Pole for 1 units
B $E3B1,1 Nothing for 1 units
B $E3B2,1 Telegraph Pole for 1 units
B $E3B3,1 Nothing for 3 units
B $E3B4,1 Telegraph Pole for 1 units
B $E3B5,1 Nothing for 1 units
B $E3B6,1 Telegraph Pole for 1 units
B $E3B7,1 Nothing for 3 units
B $E3B8,1 Telegraph Pole for 1 units
B $E3B9,1 Nothing for 1 units
B $E3BA,1 Telegraph Pole for 1 units
B $E3BB,1 Nothing for 3 units
B $E3BC,1 Street Lamp for 1 units
B $E3BD,1 Nothing for 1 units
B $E3BE,1 Street Lamp for 1 units
B $E3BF,1 Nothing for 1 units
B $E3C0,1 Street Lamp for 1 units
B $E3C1,1 Nothing for 1 units
B $E3C2,1 Street Lamp for 1 units
B $E3C3,1 Nothing for 1 units
B $E3C4,1 Street Lamp for 1 units
B $E3C5,1 Nothing for 1 units
B $E3C6,1 Street Lamp for 1 units
B $E3C7,1 Nothing for 7 units
B $E3C8,1 <Esc> Split
W $E3CA,2 Left target
W $E3CC,2 Right target
b $E3CE [Stage 4] Map right object data
B $E3CE,1 Telegraph Pole for 1 units
B $E3CF,1 Nothing for 1 units
B $E3D0,1 Telegraph Pole for 1 units
B $E3D1,1 Nothing for 1 units
B $E3D2,1 Telegraph Pole for 1 units
B $E3D3,1 Nothing for 1 units
B $E3D4,1 Telegraph Pole for 1 units
B $E3D5,1 Bush for 1 units
B $E3D6,1 Telegraph Pole for 1 units
B $E3D7,1 Nothing for 1 units
B $E3D8,1 Telegraph Pole for 1 units
B $E3D9,1 Nothing for 1 units
B $E3DA,1 Telegraph Pole for 1 units
B $E3DB,1 Nothing for 1 units
B $E3DC,1 Telegraph Pole for 1 units
B $E3DD,1 Bush for 1 units
B $E3DE,1 Telegraph Pole for 1 units
B $E3DF,1 Bush for 1 units
B $E3E0,1 Telegraph Pole for 1 units
B $E3E1,1 Nothing for 1 units
B $E3E2,1 Telegraph Pole for 1 units
B $E3E3,1 Nothing for 1 units
B $E3E4,1 Telegraph Pole for 1 units
B $E3E5,1 Nothing for 1 units
B $E3E6,1 Telegraph Pole for 1 units
B $E3E7,1 Nothing for 1 units
B $E3E8,1 Telegraph Pole for 1 units
B $E3E9,1 Nothing for 1 units
B $E3EA,1 Telegraph Pole for 1 units
B $E3EB,1 Nothing for 1 units
B $E3EC,1 Telegraph Pole for 1 units
B $E3ED,1 Nothing for 1 units
B $E3EE,1 Telegraph Pole for 1 units
B $E3EF,1 Nothing for 3 units
B $E3F0,1 Telegraph Pole for 1 units
B $E3F1,1 Nothing for 3 units
B $E3F2,1 Telegraph Pole for 1 units
B $E3F3,1 Nothing for 5 units
B $E3F4,1 Telegraph Pole for 1 units
B $E3F5,1 Nothing for 3 units
B $E3F6,1 Telegraph Pole for 1 units
B $E3F7,1 Nothing for 3 units
B $E3F8,1 Telegraph Pole for 1 units
B $E3F9,1 Nothing for 11 units
B $E3FA,1 Turn Sign, Pointing Left for 1 units
B $E3FB,1 Nothing for 1 units
B $E3FC,1 Turn Sign, Pointing Left for 1 units
B $E3FD,1 Nothing for 1 units
B $E3FE,1 Turn Sign, Pointing Left for 1 units
B $E3FF,1 Nothing for 1 units
B $E400,1 Turn Sign, Pointing Left for 1 units
B $E401,1 Nothing for 1 units
B $E402,1 Turn Sign, Pointing Left for 1 units
B $E403,1 Nothing for 1 units
B $E404,1 Turn Sign, Pointing Left for 1 units
B $E405,1 Nothing for 1 units
B $E406,1 Turn Sign, Pointing Left for 1 units
B $E407,1 Nothing for 1 units
B $E408,1 Turn Sign, Pointing Left for 1 units
B $E409,1 Nothing for 1 units
B $E40A,1 Turn Sign, Pointing Left for 1 units
B $E40B,1 Nothing for 49 units
B $E40F,1 Street Lamp for 1 units
B $E410,1 Nothing for 1 units
B $E411,1 Street Lamp for 1 units
B $E412,1 Nothing for 1 units
B $E413,1 Street Lamp for 1 units
B $E414,1 Nothing for 3 units
B $E415,1 Turn Sign, Pointing Left for 1 units
B $E416,1 Nothing for 1 units
B $E417,1 Turn Sign, Pointing Left for 1 units
B $E418,1 Nothing for 1 units
B $E419,1 Turn Sign, Pointing Left for 1 units
B $E41A,1 Nothing for 1 units
B $E41B,1 Turn Sign, Pointing Left for 1 units
B $E41C,1 Nothing for 1 units
B $E41D,1 Turn Sign, Pointing Left for 1 units
B $E41E,1 Nothing for 1 units
B $E41F,1 Turn Sign, Pointing Left for 1 units
B $E420,1 Nothing for 3 units
B $E421,1 Street Lamp for 1 units
B $E422,1 Nothing for 1 units
B $E423,1 Street Lamp for 1 units
B $E424,1 Nothing for 1 units
B $E425,1 Street Lamp for 1 units
B $E426,1 Nothing for 3 units
B $E427,1 Street Lamp for 1 units
B $E428,1 Nothing for 1 units
B $E429,1 Street Lamp for 1 units
B $E42A,1 Nothing for 1 units
B $E42B,1 Street Lamp for 1 units
B $E42C,1 Nothing for 1 units
B $E42D,1 Street Lamp for 1 units
B $E42E,1 Nothing for 1 units
B $E42F,1 Street Lamp for 1 units
B $E430,1 Nothing for 1 units
B $E431,1 Street Lamp for 1 units
B $E432,1 Nothing for 17 units
B $E434,1 Street Lamp for 1 units
B $E435,1 Nothing for 1 units
B $E436,1 Street Lamp for 1 units
B $E437,1 Nothing for 1 units
B $E438,1 Street Lamp for 1 units
B $E439,1 Nothing for 1 units
B $E43A,1 Street Lamp for 1 units
B $E43B,1 Nothing for 3 units
B $E43C,1 <Esc> Split
W $E43E,2 Left target
W $E440,2 Right target
b $E577 [Stage 4] Map curvature data
B $E577,1 Curve Straight for 67 units
B $E57C,1 Curve Left for 14 units
B $E57D,1 Curve Left Hard for 20 units
B $E57F,1 Curve Right Very Hard for 10 units
B $E580,1 Curve Left Hard for 20 units
B $E582,1 Curve Left for 30 units
B $E584,1 Curve Straight for 69 units
B $E589,1 Curve Right Hard for 31 units
B $E58C,1 Curve Right Very Hard for 10 units
B $E58D,1 Curve Right Hard for 30 units
B $E58F,1 Curve Straight for 101 units
B $E596,1 <Esc> Loop
W $E598,2 Target
b $E59A [Stage 4] Map height data
B $E59A,1 Level Road for 14 units
B $E59B,1 Going Up 3 for 2 units
B $E59C,1 Going Up 5 for 2 units
B $E59D,1 Going Up 7 for 4 units
B $E59E,1 Going Up 5 for 1 units
B $E59F,1 Going Up 3 for 1 units
B $E5A0,1 Going Down 3 for 1 units
B $E5A1,1 Going Down 5 for 2 units
B $E5A2,1 Going Down 7 for 10 units
B $E5A3,1 Going Down 5 for 2 units
B $E5A4,1 Going Down 3 for 1 units
B $E5A5,1 Going Up 3 for 1 units
B $E5A6,1 Going Up 5 for 2 units
B $E5A7,1 Going Up 7 for 7 units
B $E5A8,1 Going Up 5 for 1 units
B $E5A9,1 Going Down 7 for 2 units
B $E5AA,1 Going Down 5 for 1 units
B $E5AB,1 Going Down 3 for 1 units
B $E5AC,1 Going Down 1 for 4 units
B $E5AD,1 Level Road for 119 units
B $E5B5,1 Going Down 3 for 1 units
B $E5B6,1 Going Down 5 for 2 units
B $E5B7,1 Going Down 7 for 3 units
B $E5B8,1 Going Down 5 for 1 units
B $E5B9,1 Going Down 3 for 1 units
B $E5BA,1 Going Up 3 for 1 units
B $E5BB,1 Going Up 5 for 1 units
B $E5BC,1 Going Up 7 for 7 units
B $E5BD,1 Going Down 7 for 1 units
B $E5BE,1 Going Down 5 for 1 units
B $E5BF,1 Going Down 3 for 2 units
B $E5C0,1 Going Down 1 for 2 units
B $E5C1,1 Level Road for 201 units
B $E5CF,1 <Esc> Loop
W $E5D1,2 Target
b $E5D3 [Stage 4] Map lanes data
B $E5D3,1 4 Lanes              [||||] {00} for 306 units
B $E5D7,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E5D9,1 3 Lanes L            [|||]  {81} for 6 units
B $E5DB,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E5DD,1 4 Lanes              [||||] {00} for 8 units
B $E5DF,1 4-3 Narrowing R      [/|||] {8E} for 8 units
B $E5E1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $E5E3,1 4 Lanes              [||||] {00} for 8 units
B $E5E5,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E5E7,1 3 Lanes L            [|||]  {81} for 6 units
B $E5E9,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E5EB,1 4 Lanes              [||||] {00} for 16 units
B $E5ED,1 4-3 Narrowing R      [/|||] {8E} for 2 units
B $E5EF,1 3-2 Narrowing R       [/||] {0F} for 14 units
B $E5F1,1 2-3 Widening R        [\||] {1F} for 2 units
B $E5F3,1 3-4 Widening R       [\|||] {9E} for 2 units
B $E5F5,1 4 Lanes              [||||] {00} for 14 units
B $E5F7,1 <Esc> Loop
W $E5F9,2 Target
b $E5FB [Stage 4] Map hazards data
B $E5FB,1 Wait for 32 units
B $E5FC,1 Unknown command 4
B $E5FE,1 Wait for 2 units
B $E5FF,1 Stop Spawning Barriers 3?
B $E601,1 Wait for 26 units
B $E602,1 Unknown command 5
B $E604,1 Wait for 2 units
B $E605,1 Stop Spawning Barriers 3?
B $E607,1 Wait for 61 units
B $E608,1 Unknown command 4
B $E60A,1 Wait for 2 units
B $E60B,1 Stop Spawning Barriers 3?
B $E60D,1 Wait for 9 units
B $E60E,1 Unknown command 4
B $E610,1 Wait for 2 units
B $E611,1 Stop Spawning Barriers 3?
B $E613,1 Wait for 23 units
B $E614,1 Start Spawning Two Barriers
B $E616,1 Wait for 2 units
B $E617,1 Stop Spawning Barriers 3?
B $E619,1 Wait for 26 units
B $E61A,1 Start Spawning Barriers Right
B $E61C,1 Wait for 2 units
B $E61D,1 Stop Spawning Barriers 3?
B $E61F,1 Wait for 12 units
B $E620,1 <Esc> Loop
W $E622,2 Target
b $E624 [Stage 4] Map left object data
B $E624,1 Telegraph Pole for 1 units
B $E625,1 Nothing for 1 units
B $E626,1 Telegraph Pole for 1 units
B $E627,1 Nothing for 1 units
B $E628,1 Telegraph Pole for 1 units
B $E629,1 Nothing for 1 units
B $E62A,1 Telegraph Pole for 1 units
B $E62B,1 Nothing for 1 units
B $E62C,1 Telegraph Pole for 1 units
B $E62D,1 Nothing for 1 units
B $E62E,1 Telegraph Pole for 1 units
B $E62F,1 Nothing for 1 units
B $E630,1 Telegraph Pole for 1 units
B $E631,1 Nothing for 5 units
B $E632,1 Bush for 1 units
B $E633,1 Nothing for 1 units
B $E634,1 Bush for 1 units
B $E635,1 Nothing for 1 units
B $E636,1 Bush for 1 units
B $E637,1 Nothing for 5 units
B $E638,1 Tree for 1 units
B $E639,1 Nothing for 1 units
B $E63A,1 Tree for 1 units
B $E63B,1 Nothing for 1 units
B $E63C,1 Tree for 1 units
B $E63D,1 Nothing for 1 units
B $E63E,1 Tree for 1 units
B $E63F,1 Nothing for 1 units
B $E640,1 Tree for 1 units
B $E641,1 Nothing for 1 units
B $E642,1 Tree for 1 units
B $E643,1 Nothing for 1 units
B $E644,1 Tree for 1 units
B $E645,1 Nothing for 3 units
B $E646,1 Tree for 1 units
B $E647,1 Bush for 1 units
B $E648,1 Tree for 1 units
B $E649,1 Bush for 1 units
B $E64A,1 Nothing for 4 units
B $E64B,1 Turn Sign, Pointing Right for 1 units
B $E64C,1 Nothing for 1 units
B $E64D,1 Turn Sign, Pointing Right for 1 units
B $E64E,1 Nothing for 1 units
B $E64F,1 Turn Sign, Pointing Right for 1 units
B $E650,1 Nothing for 15 units
B $E651,1 Tree for 1 units
B $E652,1 Nothing for 1 units
B $E653,1 Tree for 1 units
B $E654,1 Nothing for 1 units
B $E655,1 Tree for 1 units
B $E656,1 Nothing for 1 units
B $E657,1 Tree for 1 units
B $E658,1 Nothing for 1 units
B $E659,1 Tree for 1 units
B $E65A,1 Nothing for 1 units
B $E65B,1 Tree for 1 units
B $E65C,1 Nothing for 1 units
B $E65D,1 Tree for 1 units
B $E65E,1 Nothing for 1 units
B $E65F,1 Tree for 1 units
B $E660,1 Nothing for 1 units
B $E661,1 Tree for 1 units
B $E662,1 Nothing for 1 units
B $E663,1 Tree for 1 units
B $E664,1 Nothing for 8 units
B $E665,1 Bush for 1 units
B $E666,1 Tree for 1 units
B $E667,1 Bush for 1 units
B $E668,1 Tree for 1 units
B $E669,1 Nothing for 1 units
B $E66A,1 Tree for 1 units
B $E66B,1 Nothing for 3 units
B $E66C,1 Tree for 1 units
B $E66D,1 Nothing for 1 units
B $E66E,1 Tree for 1 units
B $E66F,1 Nothing for 1 units
B $E670,1 Tree for 1 units
B $E671,1 Nothing for 1 units
B $E672,1 Tree for 1 units
B $E673,1 Nothing for 1 units
B $E674,1 Tree for 1 units
B $E675,1 Nothing for 1 units
B $E676,1 Tree for 1 units
B $E677,1 Nothing for 1 units
B $E678,1 Tree for 1 units
B $E679,1 Nothing for 1 units
B $E67A,1 Tree for 1 units
B $E67B,1 Nothing for 1 units
B $E67C,1 Tree for 1 units
B $E67D,1 Nothing for 1 units
B $E67E,1 Tree for 1 units
B $E67F,1 Nothing for 1 units
B $E680,1 Tree for 1 units
B $E681,1 Nothing for 1 units
B $E682,1 Tree for 1 units
B $E683,1 Nothing for 1 units
B $E684,1 Tree for 1 units
B $E685,1 Nothing for 1 units
B $E686,1 Tree for 1 units
B $E687,1 Nothing for 1 units
B $E688,1 Tree for 1 units
B $E689,1 Nothing for 1 units
B $E68A,1 Tree for 1 units
B $E68B,1 Nothing for 1 units
B $E68C,1 Tree for 1 units
B $E68D,1 Nothing for 1 units
B $E68E,1 Tree for 1 units
B $E68F,1 Nothing for 1 units
B $E690,1 Tree for 1 units
B $E691,1 Nothing for 1 units
B $E692,1 Tree for 1 units
B $E693,1 Nothing for 1 units
B $E694,1 Tree for 1 units
B $E695,1 Nothing for 1 units
B $E696,1 Tree for 1 units
B $E697,1 Nothing for 5 units
B $E698,1 Bush for 1 units
B $E699,1 Nothing for 1 units
B $E69A,1 Bush for 1 units
B $E69B,1 Nothing for 1 units
B $E69C,1 Bush for 1 units
B $E69D,1 Nothing for 3 units
B $E69E,1 Tree for 1 units
B $E69F,1 Nothing for 1 units
B $E6A0,1 Bush for 1 units
B $E6A1,1 Nothing for 3 units
B $E6A2,1 Street Lamp for 1 units
B $E6A3,1 Nothing for 1 units
B $E6A4,1 Street Lamp for 1 units
B $E6A5,1 Nothing for 1 units
B $E6A6,1 Street Lamp for 1 units
B $E6A7,1 Nothing for 1 units
B $E6A8,1 Street Lamp for 1 units
B $E6A9,1 Nothing for 6 units
B $E6AA,1 Turn Sign, Pointing Right for 4 units
B $E6AB,1 Nothing for 3 units
B $E6AC,1 Tree for 1 units
B $E6AD,1 Nothing for 5 units
B $E6AE,1 Tree for 1 units
B $E6AF,1 Nothing for 1 units
B $E6B0,1 Tree for 1 units
B $E6B1,1 Nothing for 1 units
B $E6B2,1 Tree for 1 units
B $E6B3,1 <Esc> Loop
W $E6B5,2 Target
b $E6B7 [Stage 4] Map right object data
B $E6B7,1 Nothing for 18 units
B $E6B9,1 Bush for 1 units
B $E6BA,1 Nothing for 1 units
B $E6BB,1 Bush for 1 units
B $E6BC,1 Nothing for 1 units
B $E6BD,1 Bush for 1 units
B $E6BE,1 Nothing for 4 units
B $E6BF,1 Bush for 1 units
B $E6C0,1 Tree for 1 units
B $E6C1,1 Bush for 1 units
B $E6C2,1 Tree for 1 units
B $E6C3,1 Nothing for 1 units
B $E6C4,1 Tree for 1 units
B $E6C5,1 Nothing for 1 units
B $E6C6,1 Tree for 1 units
B $E6C7,1 Nothing for 3 units
B $E6C8,1 Turn Sign, Pointing Left for 1 units
B $E6C9,1 Nothing for 1 units
B $E6CA,1 Turn Sign, Pointing Left for 1 units
B $E6CB,1 Nothing for 1 units
B $E6CC,1 Turn Sign, Pointing Left for 1 units
B $E6CD,1 Nothing for 1 units
B $E6CE,1 Turn Sign, Pointing Left for 1 units
B $E6CF,1 Nothing for 3 units
B $E6D0,1 Tree for 1 units
B $E6D1,1 Nothing for 1 units
B $E6D2,1 Tree for 1 units
B $E6D3,1 Nothing for 1 units
B $E6D4,1 Tree for 1 units
B $E6D5,1 Nothing for 1 units
B $E6D6,1 Tree for 1 units
B $E6D7,1 Nothing for 1 units
B $E6D8,1 Tree for 1 units
B $E6D9,1 Nothing for 1 units
B $E6DA,1 Tree for 1 units
B $E6DB,1 Nothing for 1 units
B $E6DC,1 Tree for 1 units
B $E6DD,1 Nothing for 1 units
B $E6DE,1 Tree for 1 units
B $E6DF,1 Nothing for 1 units
B $E6E0,1 Tree for 1 units
B $E6E1,1 Nothing for 1 units
B $E6E2,1 Tree for 1 units
B $E6E3,1 Nothing for 1 units
B $E6E4,1 Tree for 1 units
B $E6E5,1 Nothing for 1 units
B $E6E6,1 Tree for 1 units
B $E6E7,1 Nothing for 1 units
B $E6E8,1 Tree for 1 units
B $E6E9,1 Nothing for 1 units
B $E6EA,1 Tree for 1 units
B $E6EB,1 Nothing for 1 units
B $E6EC,1 Tree for 1 units
B $E6ED,1 Nothing for 1 units
B $E6EE,1 Tree for 1 units
B $E6EF,1 Nothing for 1 units
B $E6F0,1 Tree for 1 units
B $E6F1,1 Nothing for 1 units
B $E6F2,1 Tree for 1 units
B $E6F3,1 Nothing for 1 units
B $E6F4,1 Tree for 1 units
B $E6F5,1 Nothing for 1 units
B $E6F6,1 Tree for 1 units
B $E6F7,1 Nothing for 7 units
B $E6F8,1 Street Lamp for 1 units
B $E6F9,1 Nothing for 1 units
B $E6FA,1 Street Lamp for 1 units
B $E6FB,1 Nothing for 1 units
B $E6FC,1 Street Lamp for 1 units
B $E6FD,1 Nothing for 3 units
B $E6FE,1 Street Lamp for 1 units
B $E6FF,1 Nothing for 1 units
B $E700,1 Street Lamp for 1 units
B $E701,1 Nothing for 10 units
B $E702,1 Bush for 1 units
B $E703,1 Tree for 1 units
B $E704,1 Nothing for 1 units
B $E705,1 Tree for 1 units
B $E706,1 Nothing for 1 units
B $E707,1 Tree for 1 units
B $E708,1 Nothing for 1 units
B $E709,1 Tree for 1 units
B $E70A,1 Nothing for 1 units
B $E70B,1 Tree for 1 units
B $E70C,1 Nothing for 1 units
B $E70D,1 Tree for 1 units
B $E70E,1 Nothing for 1 units
B $E70F,1 Tree for 1 units
B $E710,1 Nothing for 1 units
B $E711,1 Tree for 1 units
B $E712,1 Nothing for 1 units
B $E713,1 Tree for 1 units
B $E714,1 Nothing for 1 units
B $E715,1 Tree for 1 units
B $E716,1 Nothing for 1 units
B $E717,1 Tree for 1 units
B $E718,1 Nothing for 1 units
B $E719,1 Tree for 1 units
B $E71A,1 Nothing for 1 units
B $E71B,1 Tree for 1 units
B $E71C,1 Nothing for 1 units
B $E71D,1 Tree for 1 units
B $E71E,1 Nothing for 1 units
B $E71F,1 Tree for 1 units
B $E720,1 Nothing for 1 units
B $E721,1 Tree for 1 units
B $E722,1 Nothing for 1 units
B $E723,1 Tree for 1 units
B $E724,1 Nothing for 1 units
B $E725,1 Tree for 1 units
B $E726,1 Nothing for 3 units
B $E727,1 Tree for 1 units
B $E728,1 Bush for 1 units
B $E729,1 Tree for 1 units
B $E72A,1 Nothing for 14 units
B $E72B,1 Turn Sign, Pointing Left for 2 units
B $E72C,1 Nothing for 7 units
B $E72D,1 Street Lamp for 1 units
B $E72E,1 Nothing for 1 units
B $E72F,1 Street Lamp for 1 units
B $E730,1 Nothing for 1 units
B $E731,1 Street Lamp for 1 units
B $E732,1 Nothing for 1 units
B $E733,1 Street Lamp for 1 units
B $E734,1 Nothing for 9 units
B $E735,1 Tree for 1 units
B $E736,1 Nothing for 1 units
B $E737,1 Tree for 1 units
B $E738,1 Nothing for 1 units
B $E739,1 Tree for 1 units
B $E73A,1 <Esc> Loop
W $E73C,2 Target
N $E7F2 LOD
B $E7F2,1 Width (bytes)
B $E7F3,1 Flags
B $E7F4,1 Height (pixels)
W $E7F5,2 Bitmap address
W $E7F7,2 Pre-shifted bitmap address
N $E7F9 LOD
B $E7F9,1 Width (bytes)
B $E7FA,1 Flags
B $E7FB,1 Height (pixels)
W $E7FC,2 Bitmap address
W $E7FE,2 Pre-shifted bitmap address
N $E800 LOD
B $E800,1 Width (bytes)
B $E801,1 Flags
B $E802,1 Height (pixels)
W $E803,2 Bitmap address
W $E805,2 Pre-shifted bitmap address
N $E807 LOD
B $E807,1 Width (bytes)
B $E808,1 Flags
B $E809,1 Height (pixels)
W $E80A,2 Bitmap address
W $E80C,2 Pre-shifted bitmap address
N $E80E LOD
B $E80E,1 Width (bytes)
B $E80F,1 Flags
B $E810,1 Height (pixels)
W $E811,2 Bitmap address
W $E813,2 Pre-shifted bitmap address
N $E815 LOD
B $E815,1 Width (bytes)
B $E816,1 Flags
B $E817,1 Height (pixels)
W $E818,2 Bitmap address
W $E81A,2 Pre-shifted bitmap address
N $E81C LOD
B $E81C,1 Width (bytes)
B $E81D,1 Flags
B $E81E,1 Height (pixels)
W $E81F,2 Bitmap address
W $E821,2 Pre-shifted bitmap address
N $E823 LOD
B $E823,1 Width (bytes)
B $E824,1 Flags
B $E825,1 Height (pixels)
W $E826,2 Bitmap address
W $E828,2 Pre-shifted bitmap address
N $E82A LOD
B $E82A,1 Width (bytes)
B $E82B,1 Flags
B $E82C,1 Height (pixels)
W $E82D,2 Bitmap address
W $E82F,2 Pre-shifted bitmap address
N $E831 LOD
B $E831,1 Width (bytes)
B $E832,1 Flags
B $E833,1 Height (pixels)
W $E834,2 Bitmap address
W $E836,2 Pre-shifted bitmap address
N $E838 LOD
B $E838,1 Width (bytes)
B $E839,1 Flags
B $E83A,1 Height (pixels)
W $E83B,2 Bitmap address
W $E83D,2 Pre-shifted bitmap address
N $E83F LOD
B $E83F,1 Width (bytes)
B $E840,1 Flags
B $E841,1 Height (pixels)
W $E842,2 Bitmap address
W $E844,2 Pre-shifted bitmap address
N $E846 LOD
B $E846,1 Width (bytes)
B $E847,1 Flags
B $E848,1 Height (pixels)
W $E849,2 Bitmap address
W $E84B,2 Pre-shifted bitmap address
N $E84D LOD
B $E84D,1 Width (bytes)
B $E84E,1 Flags
B $E84F,1 Height (pixels)
W $E850,2 Bitmap address
W $E852,2 Pre-shifted bitmap address
N $E854 LOD
B $E854,1 Width (bytes)
B $E855,1 Flags
B $E856,1 Height (pixels)
W $E857,2 Bitmap address
W $E859,2 Pre-shifted bitmap address
N $E85B LOD
B $E85B,1 Width (bytes)
B $E85C,1 Flags
B $E85D,1 Height (pixels)
W $E85E,2 Bitmap address
W $E860,2 Pre-shifted bitmap address
N $E862 LOD
B $E862,1 Width (bytes)
B $E863,1 Flags
B $E864,1 Height (pixels)
W $E865,2 Bitmap address
W $E867,2 Pre-shifted bitmap address
N $E869 LOD
B $E869,1 Width (bytes)
B $E86A,1 Flags
B $E86B,1 Height (pixels)
W $E86C,2 Bitmap address
W $E86E,2 Pre-shifted bitmap address
B $E870,180,6 Bitmap data 6 bytes x 30
B $E924,88,4 Bitmap data 4 bytes x 22
B $E97C,24,2 Bitmap data 2 bytes x 12
B $E97C,24,2 Bitmap data 2 bytes x 12
B $E994,16,2 Bitmap data 2 bytes x 8
B $E994,16,2 Bitmap data 2 bytes x 8
B $E994,16,2 Bitmap data 2 bytes x 8
B $E994,16,2 Bitmap data 2 bytes x 8
B $E9B4,16,2 Pre-shifted bitmap data 2 bytes x 8
B $E9B4,16,2 Pre-shifted bitmap data 2 bytes x 8
B $E9D4,180,6 Bitmap data 6 bytes x 30
B $EA88,88,4 Bitmap data 4 bytes x 22
B $EAE0,45,3 Bitmap data 3 bytes x 15
B $EAE0,45,3 Bitmap data 3 bytes x 15
B $EB0D,174,6 Bitmap data 6 bytes x 29
B $EBBB,76,4 Bitmap data 4 bytes x 19
B $EC07,39,3 Bitmap data 3 bytes x 13
B $EC07,39,3 Bitmap data 3 bytes x 13
B $EC2E,16,2 Bitmap data 2 bytes x 8
B $EC2E,16,2 Bitmap data 2 bytes x 8

