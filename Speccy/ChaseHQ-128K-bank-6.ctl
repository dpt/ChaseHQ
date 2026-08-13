b $C000 [Stage 3] Horizon graphic
B $C000,240,8
b $C0F0 [Stage 3] Per-stage data
@ $C0F0 label=stage3
W $C0F0,2,2 [$C8CC] Address of perp's mugshot (attributes)
W $C0F2,2,2 [out-of-bounds] Address of pilot's mugshot (bitmap)
W $C0F4,2,2 [$D470] Screen attributes used for the ground colour (a pair of matching bytes)
W $C0F6,2,2 [$C22E] Address of table of LODs for tumbleweeds, barriers.
W $C0F8,2,2 [$C230] (points at a handler address)
W $C0FA,2,2 [$C22D] Address of right hand graphics entry/entries (-7 bytes)
W $C0FC,2,2 [$C242] (points at a handler address)
W $C0FE,2,2 [$C25A] (points at a handler address)
W $C100,2,2 [$C257] Address of left hand graphics entry/entries (-7 bytes)
W $C102,2,2 [$C26C] (points at a handler address)
W $C104,2,2 [$C139] Address of Nancy's perp description
W $C106,2,2 [$C1E8] Address of arrest messages
W $C108,2,2 [out-of-bounds] Helicopter data 1
W $C10A,2,2 [out-of-bounds] Helicopter data 2
w $C10C [Stage 3] Table of addresses of LODs
W $C10C,2,2 [$CDF1] Address of LOD of Hazard (stone/dust)
W $C10E,2,2 [$CE1B] Address of LOD of Hazard (stone/dust)
W $C110,2,2 [$C8E0] Address of LOD of Car A (the perp's car)
W $C112,2,2 [$C934] Address of LOD of Car B (a Lambo in S1)
W $C114,2,2 [$C90A] Address of LOD of Car C (a truck in S1)
W $C116,2,2 [$C934] Address of LOD of Car D (a Lambo in S1)
W $C118,2,2 [$C8E0] Address of LOD of Car E (a generic car in S1)
b $C11A [Stage 3] Per-stage difficulty settings
B $C11A,1,1 How often cars spawn. Lower values spawn cars more often.
B $C11B,1,1 Smash config parameter TBD
B $C11C,1,1 Smash config parameter TBD
w $C11D [Stage 3] Per-stage setup data
W $C11D,2,2 road_pos
W $C11F,2,2 [$C287] Address of start stretch, curvature
W $C121,2,2 [$C2AB] Address of start stretch, height
W $C123,2,2 [$C2CC] Address of start stretch, lanes
W $C125,2,2 [$C36B] Address of start stretch, right-side objects
W $C127,2,2 [$C2E7] Address of start stretch, left-side objects
W $C129,2,2 [$C2DA] Address of start stretch, hazards
w $C12B [Stage 3] Per-stage attract mode data
W $C12B,2,2 road_pos
W $C12D,2,2 [$C676] Address of loop section, curvature
W $C12F,2,2 [$C69C] Address of loop section, height
W $C131,2,2 [$C6DB] Address of loop section, lanes
W $C133,2,2 [$C7AC] Address of loop section, right-side objects
W $C135,2,2 [$C71E] Address of loop section, left-side objects
W $C137,2,2 [$C6FB] Address of loop section, hazards
b $C139 [Stage 3] Nancy's perp description
@ $C139 label=stage3_perp_description
B $C139,1,1 Character identifier (0/1/2/3 = Pilot/Nancy/Raymond/Tony)
W $C13A,2,2 [$C145] Perp description pointer
W $C13C,2,2 [$C16D] Perp description pointer
W $C13E,2,2 [$C196] Perp description pointer
W $C140,2,2 [$C1C1] Perp description pointer
B $C142,1,1 Escape: Jump
W $C143,2,2 Address of next message (always $98BD)
T $C145,40,39:n1 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $C16D,41,40:n1 "EMERGENCY HERE. A GANG OF CHICAGO PUSHERS"
T $C196,43,42:n1 "ARE FLEEING TOWARDS THE SUBURBS. THE TARGET"
T $C1C1,39,38:n1 "VEHICLE IS A GERMAN SPORTS CAR... OVER."
b $C1E8 [Stage 3] Arrest messages
B $C1E8,1,1 ?frame delay until first message
B $C1E9,1,1 ?frame delay until next message
B $C1EA,1,1 Flags
B $C1EB,1,1 Attribute
W $C1EC,2,2 Back buffer address
W $C1EE,2,2 Attribute address
T $C1F0,27,26:n1 "OK! YOU ARE UNDER ARREST ON"
B $C20B,1,1 ?frame delay until next message
B $C20C,1,1 Flags
B $C20D,1,1 Attribute
W $C20E,2,2 Back buffer address
W $C210,2,2 Attribute address
T $C212,26,25:n1 "SUSPICION OF SELLING DRUGS"
B $C22C,1,1 ?frame delay until next message
B $C22D,1,1 Stop
b $C22E [Stage 3] Hittable hazards
B $C22E,1,1 ?id
W $C22F,2,2 [$D64F] Address of LODs
B $C231,1,1 ?id
W $C232,2,2 [$D6BF] Address of LODs
b $C234 [Stage 3] Object graphic definitions (right)
N $C234 Graphic definition for object 1 - TUNNEL_LIGHT
B $C234,1,1 Hit coord max (furthest)
B $C235,1,1 Hit coord min (nearest)
B $C236,1,1 How far to push hero car away if hit
W $C237,2,2 Argument - tunnellight
W $C239,2,2 [out-of-bounds] Address of routine draw_tunnel_light_right
N $C23B Graphic definition for object 2 - OVERHEAD_BRIDGE
B $C23B,1,1 Hit coord max (furthest)
B $C23C,1,1 Hit coord min (nearest)
B $C23D,1,1 How far to push hero car away if hit
W $C23E,2,2 [$D2BB] Argument for routine passed in #REGde
W $C240,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $C242 Graphic definition for object 3 - SHORT_POLE
B $C242,1,1 Hit coord max (furthest)
B $C243,1,1 Hit coord min (nearest)
B $C244,1,1 How far to push hero car away if hit
W $C245,2,2 Argument - stretchy_shortpole
W $C247,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $C249 Graphic definition for object 4 - TOWER_BLOCK
B $C249,1,1 Hit coord max (furthest)
B $C24A,1,1 Hit coord min (nearest)
B $C24B,1,1 How far to push hero car away if hit
W $C24C,2,2 [$CEA1] Argument for routine passed in #REGde
W $C24E,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $C250 Graphic definition for object 5 - SPEED_LIMIT_SIGN
B $C250,1,1 Hit coord max (furthest)
B $C251,1,1 Hit coord min (nearest)
B $C252,1,1 How far to push hero car away if hit
W $C253,2,2 [$D79E] Argument for routine passed in #REGde
W $C255,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $C257 Graphic definition for object 6 - TELEGRAPH_POLE
B $C257,1,1 Hit coord max (furthest)
B $C258,1,1 Hit coord min (nearest)
B $C259,1,1 How far to push hero car away if hit
W $C25A,2,2 [$D9D6] Argument for routine passed in #REGde
W $C25C,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
b $C25E [Stage 3] Object graphic definitions (left)
N $C25E Graphic definition for object 1 - TUNNEL_LIGHT
B $C25E,1,1 Hit coord min (furthest)
B $C25F,1,1 Hit coord max (nearest)
B $C260,1,1 How far to push hero car away if hit
W $C261,2,2 Argument - tunnellight
W $C263,2,2 [out-of-bounds] Address of routine draw_tunnel_light_left
N $C265 Graphic definition for object 2 - OVERHEAD_BRIDGE
B $C265,1,1 Hit coord min (furthest)
B $C266,1,1 Hit coord max (nearest)
B $C267,1,1 How far to push hero car away if hit
W $C268,2,2 [$D326] Argument for routine passed in #REGde
W $C26A,2,2 [out-of-bounds] Address of routine draw_overhead
N $C26C Graphic definition for object 3 - SHORT_POLE
B $C26C,1,1 Hit coord min (furthest)
B $C26D,1,1 Hit coord max (nearest)
B $C26E,1,1 How far to push hero car away if hit
W $C26F,2,2 Argument - stretchy_shortpole
W $C271,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
N $C273 Graphic definition for object 4 - TOWER_BLOCK
B $C273,1,1 Hit coord min (furthest)
B $C274,1,1 Hit coord max (nearest)
B $C275,1,1 How far to push hero car away if hit
W $C276,2,2 [$CEA1] Argument for routine passed in #REGde
W $C278,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
N $C27A Graphic definition for object 5 - SPEED_LIMIT_SIGN
B $C27A,1,1 Hit coord min (furthest)
B $C27B,1,1 Hit coord max (nearest)
B $C27C,1,1 How far to push hero car away if hit
W $C27D,2,2 [$D7A8] Argument for routine passed in #REGde
W $C27F,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
N $C281 Graphic definition for object 6 - TELEGRAPH_POLE
B $C281,1,1 Hit coord min (furthest)
B $C282,1,1 Hit coord max (nearest)
B $C283,1,1 How far to push hero car away if hit
W $C284,2,2 [$D9E0] Argument for routine passed in #REGde
W $C286,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
b $C288 [Stage 3] Map curvature data
B $C288,1,1 Curve Straight for 65 units
B $C289,4,4
B $C28D,1,1 Curve Right for 136 units
B $C28E,9,8,1
B $C297,1,1 Curve Straight for 39 units
B $C298,2,2
B $C29A,1,1 Curve Left Hard for 51 units
B $C29B,3,3
B $C29E,1,1 Curve Straight for 25 units
B $C29F,1,1
B $C2A0,1,1 Curve Right Hard for 55 units
B $C2A1,3,3
B $C2A4,1,1 Curve Straight for 29 units
B $C2A5,1,1
B $C2A6,1,1 <Esc> Split
B $C2A7,1,1
W $C2A8,2,2 [$C3F4] Left target
W $C2AA,2,2 [$C4D8] Right target
b $C2AC [Stage 3] Map height data
B $C2AC,1,1 Level Road for 400 units
B $C2AD,26,8*3,2
B $C2C7,1,1 <Esc> Split
B $C2C8,1,1
W $C2C9,2,2 [$C409] Left target
W $C2CB,2,2 [$C4EF] Right target
b $C2CD [Stage 3] Map lanes data
B $C2CD,1,1 3 Lanes R             [|||] {82} for 60 units
B $C2CE,1,1
B $C2CF,1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C2D0,1,1
B $C2D1,1,1 4 Lanes              [||||] {00} for 338 units
B $C2D2,3,3
B $C2D5,1,1 <Esc> Split
B $C2D6,1,1
W $C2D7,2,2 [$C44B] Left target
W $C2D9,2,2 [$C504] Right target
b $C2DB [Stage 3] Map hazards data
B $C2DB,1,1 Wait for 193 units
B $C2DC,1,1 Disable Car Spawning
B $C2DD,1,1
B $C2DE,1,1 Wait for 3 units
B $C2DF,1,1 Set Floating Arrow to Left
B $C2E0,1,1
B $C2E1,1,1 Wait for 4 units
B $C2E2,1,1 <Esc> Split
B $C2E3,1,1
W $C2E4,2,2 [$C455] Left target
W $C2E6,2,2 [$C510] Right target
b $C2E8 [Stage 3] Map left object data
B $C2E8,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 28 units
B $C2E9,27,8*3,3
B $C304,1,1 TELEGRAPH_POLE for 1 units
B $C305,1,1 Alternating (TOWER_BLOCK, EMPTY) for 18 units
B $C306,18,8*2,2
B $C318,1,1 TOWER_BLOCK for 10 units
B $C319,1,1 EMPTY for 3 units
B $C31A,1,1 TOWER_BLOCK for 1 units
B $C31B,1,1 EMPTY for 3 units
B $C31C,1,1 TOWER_BLOCK for 1 units
B $C31D,1,1 EMPTY for 3 units
B $C31E,1,1 TOWER_BLOCK for 1 units
B $C31F,1,1 EMPTY for 3 units
B $C320,1,1 TOWER_BLOCK for 1 units
B $C321,1,1 EMPTY for 1 units
B $C322,1,1 TOWER_BLOCK for 1 units
B $C323,1,1 EMPTY for 3 units
B $C324,1,1 TELEGRAPH_POLE for 1 units
B $C325,1,1 EMPTY for 1 units
B $C326,1,1 TELEGRAPH_POLE for 1 units
B $C327,1,1 EMPTY for 3 units
B $C328,1,1 TELEGRAPH_POLE for 1 units
B $C329,1,1 Alternating (TOWER_BLOCK, EMPTY) for 4 units
B $C32A,4,4
B $C32E,1,1 TOWER_BLOCK for 8 units
B $C32F,1,1 Alternating (TOWER_BLOCK, EMPTY) for 18 units
B $C330,18,8*2,2
B $C342,1,1 TOWER_BLOCK for 8 units
B $C343,1,1 EMPTY for 3 units
B $C344,1,1 TELEGRAPH_POLE for 1 units
B $C345,1,1 EMPTY for 3 units
B $C346,1,1 TELEGRAPH_POLE for 1 units
B $C347,1,1 EMPTY for 1 units
B $C348,1,1 TELEGRAPH_POLE for 1 units
B $C349,1,1 EMPTY for 3 units
B $C34A,1,1 TELEGRAPH_POLE for 1 units
B $C34B,1,1 EMPTY for 1 units
B $C34C,1,1 TELEGRAPH_POLE for 1 units
B $C34D,1,1 EMPTY for 7 units
B $C34E,1,1 TOWER_BLOCK for 1 units
B $C34F,1,1 EMPTY for 3 units
B $C350,1,1 TOWER_BLOCK for 1 units
B $C351,1,1 EMPTY for 1 units
B $C352,1,1 TOWER_BLOCK for 1 units
B $C353,1,1 EMPTY for 3 units
B $C354,1,1 TOWER_BLOCK for 1 units
B $C355,1,1 EMPTY for 3 units
B $C356,1,1 TOWER_BLOCK for 1 units
B $C357,1,1 EMPTY for 5 units
B $C358,1,1 TOWER_BLOCK for 1 units
B $C359,1,1 EMPTY for 3 units
B $C35A,1,1 SPEED_LIMIT_SIGN for 1 units
B $C35B,1,1 EMPTY for 5 units
B $C35C,1,1 SPEED_LIMIT_SIGN for 1 units
B $C35D,1,1 EMPTY for 9 units
B $C35E,1,1 SPEED_LIMIT_SIGN for 1 units
B $C35F,1,1 EMPTY for 5 units
B $C360,1,1 SPEED_LIMIT_SIGN for 1 units
B $C361,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 4 units
B $C362,4,4
B $C366,1,1 <Esc> Split
B $C367,1,1
W $C368,2,2 [$C46F] Left target
W $C36A,2,2 [$C518] Right target
b $C36C [Stage 3] Map right object data
B $C36C,1,1 TOWER_BLOCK for 1 units
B $C36D,1,1 EMPTY for 3 units
B $C36E,1,1 TOWER_BLOCK for 1 units
B $C36F,1,1 EMPTY for 3 units
B $C370,1,1 TOWER_BLOCK for 1 units
B $C371,1,1 EMPTY for 3 units
B $C372,1,1 TOWER_BLOCK for 1 units
B $C373,1,1 Alternating (TOWER_BLOCK, EMPTY) for 10 units
B $C374,10,8,2
B $C37E,1,1 TOWER_BLOCK for 6 units
B $C37F,1,1 EMPTY for 3 units
B $C380,1,1 TOWER_BLOCK for 1 units
B $C381,1,1 EMPTY for 3 units
B $C382,1,1 TOWER_BLOCK for 1 units
B $C383,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 8 units
B $C384,8,8
B $C38C,1,1 TELEGRAPH_POLE for 6 units
B $C38D,1,1 EMPTY for 3 units
B $C38E,1,1 TELEGRAPH_POLE for 1 units
B $C38F,1,1 EMPTY for 3 units
B $C390,1,1 TELEGRAPH_POLE for 1 units
B $C391,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 12 units
B $C392,12,8,4
B $C39E,1,1 TELEGRAPH_POLE for 4 units
B $C39F,1,1 EMPTY for 5 units
B $C3A0,1,1 TELEGRAPH_POLE for 1 units
B $C3A1,1,1 EMPTY for 3 units
B $C3A2,1,1 TELEGRAPH_POLE for 1 units
B $C3A3,1,1 EMPTY for 3 units
B $C3A4,1,1 TELEGRAPH_POLE for 1 units
B $C3A5,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 6 units
B $C3A6,6,6
B $C3AC,1,1 TELEGRAPH_POLE for 4 units
B $C3AD,1,1 Alternating (TOWER_BLOCK, EMPTY) for 24 units
B $C3AE,24,8
B $C3C6,1,1 TOWER_BLOCK for 6 units
B $C3C7,1,1 EMPTY for 5 units
B $C3C8,1,1 TOWER_BLOCK for 1 units
B $C3C9,1,1 EMPTY for 3 units
B $C3CA,1,1 TOWER_BLOCK for 1 units
B $C3CB,1,1 EMPTY for 1 units
B $C3CC,1,1 TOWER_BLOCK for 1 units
B $C3CD,1,1 EMPTY for 3 units
B $C3CE,1,1 TOWER_BLOCK for 1 units
B $C3CF,1,1 EMPTY for 3 units
B $C3D0,1,1 TOWER_BLOCK for 1 units
B $C3D1,1,1 EMPTY for 9 units
B $C3D2,1,1 TOWER_BLOCK for 1 units
B $C3D3,1,1 EMPTY for 3 units
B $C3D4,1,1 TOWER_BLOCK for 1 units
B $C3D5,1,1 EMPTY for 7 units
B $C3D6,1,1 SPEED_LIMIT_SIGN for 1 units
B $C3D7,1,1 EMPTY for 5 units
B $C3D8,1,1 SPEED_LIMIT_SIGN for 1 units
B $C3D9,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 20 units
B $C3DA,20,8*2,4
B $C3EE,1,1 <Esc> Split
B $C3EF,1,1
W $C3F0,2,2 [$C4A9] Left target
W $C3F2,2,2 [$C54B] Right target
b $C3F4 [Stage 3] Map curvature data
B $C3F4,1,1 Curve Straight for 51 units
B $C3F5,3,3
B $C3F8,1,1 Curve Right for 49 units
B $C3F9,3,3
B $C3FC,1,1 Curve Right Hard for 30 units
B $C3FD,1,1
B $C3FE,1,1 Curve Right Very Hard for 11 units
B $C3FF,1,1 Curve Right Hard for 10 units
B $C400,1,1 Curve Right for 42 units
B $C401,2,2
B $C403,1,1 Curve Straight for 17 units
B $C404,1,1
B $C405,1,1 <Esc> Jump
B $C406,1,1
W $C407,2,2 [$C58A] Target
b $C409 [Stage 3] Map height data
B $C409,1,1 Level Road for 20 units
B $C40A,1,1
B $C40B,1,1 Going Up 3 for 5 units
B $C40C,1,1 Level Road for 1 units
B $C40D,1,1 Going Down 3 for 5 units
B $C40E,1,1 Level Road for 1 units
B $C40F,1,1 Going Up 3 for 5 units
B $C410,1,1 Level Road for 1 units
B $C411,1,1 Going Down 3 for 5 units
B $C412,1,1 Level Road for 1 units
B $C413,1,1 Going Up 3 for 5 units
B $C414,1,1 Level Road for 1 units
B $C415,1,1 Going Down 3 for 5 units
B $C416,1,1 Level Road for 1 units
B $C417,1,1 Going Up 3 for 5 units
B $C418,1,1 Level Road for 1 units
B $C419,1,1 Going Down 3 for 5 units
B $C41A,1,1 Level Road for 1 units
B $C41B,1,1 Going Up 3 for 5 units
B $C41C,1,1 Level Road for 1 units
B $C41D,1,1 Going Down 3 for 5 units
B $C41E,1,1 Level Road for 1 units
B $C41F,1,1 Going Up 3 for 5 units
B $C420,1,1 Level Road for 1 units
B $C421,1,1 Going Down 3 for 5 units
B $C422,1,1 Level Road for 1 units
B $C423,1,1 Going Up 3 for 5 units
B $C424,1,1 Level Road for 1 units
B $C425,1,1 Going Down 3 for 5 units
B $C426,1,1 Level Road for 1 units
B $C427,1,1 Going Up 3 for 5 units
B $C428,1,1 Level Road for 1 units
B $C429,1,1 Going Down 3 for 5 units
B $C42A,1,1 Level Road for 1 units
B $C42B,1,1 Going Up 3 for 5 units
B $C42C,1,1 Level Road for 1 units
B $C42D,1,1 Going Down 3 for 5 units
B $C42E,1,1 Level Road for 1 units
B $C42F,1,1 Going Up 3 for 5 units
B $C430,1,1 Level Road for 1 units
B $C431,1,1 Going Down 3 for 5 units
B $C432,1,1 Level Road for 1 units
B $C433,1,1 Going Up 3 for 5 units
B $C434,1,1 Level Road for 1 units
B $C435,1,1 Going Down 3 for 5 units
B $C436,1,1 Level Road for 1 units
B $C437,1,1 Going Up 3 for 5 units
B $C438,1,1 Level Road for 1 units
B $C439,1,1 Going Down 3 for 5 units
B $C43A,1,1 Level Road for 1 units
B $C43B,1,1 Going Up 3 for 5 units
B $C43C,1,1 Level Road for 1 units
B $C43D,1,1 Going Down 3 for 5 units
B $C43E,1,1 Level Road for 1 units
B $C43F,1,1 Going Up 3 for 5 units
B $C440,1,1 Level Road for 1 units
B $C441,1,1 Going Down 3 for 5 units
B $C442,1,1 Level Road for 1 units
B $C443,1,1 Going Up 3 for 5 units
B $C444,1,1 Level Road for 1 units
B $C445,1,1 Going Down 3 for 5 units
B $C446,1,1 Level Road for 11 units
B $C447,1,1 <Esc> Jump
B $C448,1,1
W $C449,2,2 [$C5A0] Target
b $C44B [Stage 3] Map lanes data
B $C44B,1,1 4 Lanes              [||||] {00} for 20 units
B $C44C,1,1
B $C44D,1,1 4 Lanes dirt track   [||||] {C1} for 178 units
B $C44E,1,1
B $C44F,1,1 4 Lanes              [||||] {00} for 12 units
B $C450,1,1
B $C451,1,1 <Esc> Jump
B $C452,1,1
W $C453,2,2 [$C5B6] Target
b $C455 [Stage 3] Map hazards data
B $C455,1,1 Wait for 10 units
B $C456,1,1 Start Spawning HAZARD_2 Both Sides
B $C457,1,1
B $C458,1,1 Wait for 1 units
B $C459,1,1 Start Spawning HAZARD_1 Both Sides
B $C45A,1,1
B $C45B,1,1 Wait for 41 units
B $C45C,1,1 Start Spawning HAZARD_2 Both Sides
B $C45D,1,1
B $C45E,1,1 Wait for 4 units
B $C45F,1,1 Start Spawning HAZARD_1 Both Sides
B $C460,1,1
B $C461,1,1 Wait for 42 units
B $C462,1,1 Start Spawning HAZARD_2 Both Sides
B $C463,1,1
B $C464,1,1 Wait for 2 units
B $C465,1,1 Stop Spawning Hazards
B $C466,1,1
B $C467,1,1 Wait for 1 units
B $C468,1,1 Enable Car Spawning
B $C469,1,1
B $C46A,1,1 Wait for 4 units
B $C46B,1,1 <Esc> Jump
B $C46C,1,1
W $C46D,2,2 [$C5C4] Target
b $C46F [Stage 3] Map left object data
B $C46F,1,1 EMPTY for 10 units
B $C470,1,1 SHORT_POLE for 1 units
B $C471,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 10 units
B $C472,10,8,2
B $C47C,1,1 TELEGRAPH_POLE for 8 units
B $C47D,1,1 EMPTY for 5 units
B $C47E,1,1 TELEGRAPH_POLE for 1 units
B $C47F,1,1 EMPTY for 1 units
B $C480,1,1 TELEGRAPH_POLE for 1 units
B $C481,1,1 EMPTY for 5 units
B $C482,1,1 TELEGRAPH_POLE for 1 units
B $C483,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 6 units
B $C484,6,6
B $C48A,1,1 TELEGRAPH_POLE for 4 units
B $C48B,1,1 EMPTY for 3 units
B $C48C,1,1 TELEGRAPH_POLE for 1 units
B $C48D,1,1 EMPTY for 1 units
B $C48E,1,1 TELEGRAPH_POLE for 1 units
B $C48F,1,1 EMPTY for 3 units
B $C490,1,1 TELEGRAPH_POLE for 1 units
B $C491,1,1 EMPTY for 1 units
B $C492,1,1 TELEGRAPH_POLE for 1 units
B $C493,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 6 units
B $C494,6,6
B $C49A,1,1 TELEGRAPH_POLE for 8 units
B $C49B,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 4 units
B $C49C,4,4
B $C4A0,1,1 TELEGRAPH_POLE for 14 units
B $C4A1,1,1 EMPTY for 5 units
B $C4A2,1,1 TELEGRAPH_POLE for 1 units
B $C4A3,1,1 EMPTY for 1 units
B $C4A4,1,1 TELEGRAPH_POLE for 1 units
B $C4A5,1,1 <Esc> Jump
B $C4A6,1,1
W $C4A7,2,2 [$C5C9] Target
b $C4A9 [Stage 3] Map right object data
B $C4A9,1,1 EMPTY for 2 units
B $C4AA,1,1 SPEED_LIMIT_SIGN for 1 units
B $C4AB,1,1 EMPTY for 5 units
B $C4AC,1,1 SPEED_LIMIT_SIGN for 1 units
B $C4AD,1,1 EMPTY for 1 units
B $C4AE,1,1 SHORT_POLE for 1 units
B $C4AF,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 6 units
B $C4B0,6,6
B $C4B6,1,1 TELEGRAPH_POLE for 12 units
B $C4B7,1,1 EMPTY for 9 units
B $C4B8,1,1 TELEGRAPH_POLE for 1 units
B $C4B9,1,1 EMPTY for 1 units
B $C4BA,1,1 TELEGRAPH_POLE for 1 units
B $C4BB,1,1 EMPTY for 7 units
B $C4BC,1,1 TELEGRAPH_POLE for 1 units
B $C4BD,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 18 units
B $C4BE,18,8*2,2
B $C4D0,1,1 TELEGRAPH_POLE for 16 units
B $C4D1,1,1 EMPTY for 13 units
B $C4D2,1,1 TELEGRAPH_POLE for 1 units
B $C4D3,1,1 EMPTY for 8 units
B $C4D4,1,1 <Esc> Jump
B $C4D5,1,1
W $C4D6,2,2 [$C620] Target
b $C4D8 [Stage 3] Map curvature data
B $C4D8,1,1 Curve Straight for 8 units
B $C4D9,1,1 Curve Left for 7 units
B $C4DA,1,1 Curve Left Hard for 37 units
B $C4DB,2,2
B $C4DD,1,1 Curve Straight for 29 units
B $C4DE,1,1
B $C4DF,1,1 Curve Right Hard for 30 units
B $C4E0,1,1
B $C4E1,1,1 Curve Right for 9 units
B $C4E2,1,1 Curve Straight for 11 units
B $C4E3,1,1 Curve Left for 9 units
B $C4E4,1,1 Curve Left Hard for 26 units
B $C4E5,1,1
B $C4E6,1,1 Curve Left for 5 units
B $C4E7,1,1 Curve Straight for 23 units
B $C4E8,1,1
B $C4E9,1,1 Curve Right for 16 units
B $C4EA,1,1
B $C4EB,1,1 <Esc> Jump
B $C4EC,1,1
W $C4ED,2,2 [$C58A] Target
b $C4EF [Stage 3] Map height data
B $C4EF,1,1 Going Up 5 for 42 units
B $C4F0,2,2
B $C4F2,1,1 Going Up 3 for 1 units
B $C4F3,1,1 Going Down 3 for 1 units
B $C4F4,1,1 Going Down 5 for 41 units
B $C4F5,2,2
B $C4F7,1,1 Level Road for 125 units
B $C4F8,8,8
B $C500,1,1 <Esc> Jump
B $C501,1,1
W $C502,2,2 [$C5A0] Target
b $C504 [Stage 3] Map lanes data
B $C504,1,1 4 Lanes              [||||] {00} for 56 units
B $C505,1,1
B $C506,1,1 4-3 Narrowing R      [/|||] {8E} for 134 units
B $C507,1,1
B $C508,1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C509,1,1
B $C50A,1,1 4 Lanes              [||||] {00} for 18 units
B $C50B,1,1
B $C50C,1,1 <Esc> Jump
B $C50D,1,1
W $C50E,2,2 [$C5B6] Target
b $C510 [Stage 3] Map hazards data
B $C510,1,1 Wait for 10 units
B $C511,1,1 Enable Car Spawning
B $C512,1,1
B $C513,1,1 Wait for 95 units
B $C514,1,1 <Esc> Jump
B $C515,1,1
W $C516,2,2 [$C5C4] Target
b $C518 [Stage 3] Map left object data
B $C518,1,1 EMPTY for 6 units
B $C519,1,1 SPEED_LIMIT_SIGN for 1 units
B $C51A,1,1 Alternating (TOWER_BLOCK, EMPTY) for 10 units
B $C51B,10,8,2
B $C525,1,1 TOWER_BLOCK for 8 units
B $C526,1,1 EMPTY for 2 units
B $C527,1,1 SHORT_POLE for 4 units
B $C528,1,1 EMPTY for 5 units
B $C529,1,1 TOWER_BLOCK for 1 units
B $C52A,1,1 EMPTY for 3 units
B $C52B,1,1 TOWER_BLOCK for 1 units
B $C52C,1,1 EMPTY for 3 units
B $C52D,1,1 TOWER_BLOCK for 1 units
B $C52E,1,1 Alternating (TOWER_BLOCK, EMPTY) for 6 units
B $C52F,6,6
B $C535,1,1 TOWER_BLOCK for 4 units
B $C536,1,1 Alternating (TOWER_BLOCK, EMPTY) for 12 units
B $C537,12,8,4
B $C543,1,1 TOWER_BLOCK for 4 units
B $C544,1,1 EMPTY for 34 units
B $C545,2,2
B $C547,1,1 <Esc> Jump
B $C548,1,1
W $C549,2,2 [$C5C9] Target
b $C54B [Stage 3] Map right object data
B $C54B,1,1 EMPTY for 6 units
B $C54C,1,1 SPEED_LIMIT_SIGN for 1 units
B $C54D,1,1 Alternating (TOWER_BLOCK, EMPTY) for 14 units
B $C54E,14,8,6
B $C55C,1,1 TOWER_BLOCK for 8 units
B $C55D,1,1 EMPTY for 3 units
B $C55E,1,1 TOWER_BLOCK for 1 units
B $C55F,1,1 EMPTY for 3 units
B $C560,1,1 TOWER_BLOCK for 1 units
B $C561,1,1 EMPTY for 3 units
B $C562,1,1 TOWER_BLOCK for 1 units
B $C563,1,1 EMPTY for 3 units
B $C564,1,1 TOWER_BLOCK for 1 units
B $C565,1,1 EMPTY for 3 units
B $C566,1,1 TOWER_BLOCK for 1 units
B $C567,1,1 EMPTY for 3 units
B $C568,1,1 TOWER_BLOCK for 1 units
B $C569,1,1 EMPTY for 3 units
B $C56A,1,1 TOWER_BLOCK for 1 units
B $C56B,1,1 EMPTY for 3 units
B $C56C,1,1 TOWER_BLOCK for 1 units
B $C56D,1,1 EMPTY for 3 units
B $C56E,1,1 TOWER_BLOCK for 1 units
B $C56F,1,1 EMPTY for 3 units
B $C570,1,1 TOWER_BLOCK for 1 units
B $C571,1,1 EMPTY for 3 units
B $C572,1,1 TOWER_BLOCK for 1 units
B $C573,1,1 EMPTY for 3 units
B $C574,1,1 TOWER_BLOCK for 1 units
B $C575,1,1 EMPTY for 3 units
B $C576,1,1 TOWER_BLOCK for 1 units
B $C577,1,1 EMPTY for 3 units
B $C578,1,1 TOWER_BLOCK for 1 units
B $C579,1,1 EMPTY for 3 units
B $C57A,1,1 TOWER_BLOCK for 1 units
B $C57B,1,1 Alternating (TOWER_BLOCK, EMPTY) for 8 units
B $C57C,8,8
B $C584,1,1 TOWER_BLOCK for 4 units
B $C585,1,1 EMPTY for 4 units
B $C586,1,1 <Esc> Jump
B $C587,1,1
W $C588,2,2 [$C620] Target
b $C58A [Stage 3] Map curvature data
B $C58A,1,1 Curve Straight for 61 units
B $C58B,4,4
B $C58F,1,1 Curve Left for 31 units
B $C590,2,2
B $C592,1,1 Curve Left Hard for 20 units
B $C593,1,1
B $C594,1,1 Curve Left for 4 units
B $C595,1,1 Curve Right for 29 units
B $C596,1,1
B $C597,1,1 Curve Right Hard for 34 units
B $C598,2,2
B $C59A,1,1 Curve Right for 7 units
B $C59B,1,1 Curve Straight for 6 units
B $C59C,1,1 <Esc> Jump
B $C59D,1,1
W $C59E,2,2 [$C677] Target
b $C5A0 [Stage 3] Map height data
B $C5A0,1,1 Going Up 3 for 1 units
B $C5A1,1,1 Going Up 5 for 1 units
B $C5A2,1,1 Going Up 7 for 6 units
B $C5A3,1,1 Going Up 5 for 1 units
B $C5A4,1,1 Going Down 5 for 9 units
B $C5A5,1,1 Level Road for 9 units
B $C5A6,1,1 Going Up 5 for 8 units
B $C5A7,1,1 Going Up 3 for 2 units
B $C5A8,1,1 Going Up 1 for 1 units
B $C5A9,1,1 Going Down 1 for 1 units
B $C5AA,1,1 Going Down 3 for 1 units
B $C5AB,1,1 Going Down 5 for 6 units
B $C5AC,1,1 Going Down 3 for 3 units
B $C5AD,1,1 Going Down 1 for 3 units
B $C5AE,1,1 Level Road for 55 units
B $C5AF,3,3
B $C5B2,1,1 <Esc> Jump
B $C5B3,1,1
W $C5B4,2,2 [$C69D] Target
b $C5B6 [Stage 3] Map lanes data
B $C5B6,1,1 4 Lanes              [||||] {00} for 8 units
B $C5B7,1,1
B $C5B8,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C5B9,1,1
B $C5BA,1,1 3 Lanes L            [|||]  {81} for 178 units
B $C5BB,1,1
B $C5BC,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C5BD,1,1
B $C5BE,1,1 4 Lanes              [||||] {00} for 2 units
B $C5BF,1,1
B $C5C0,1,1 <Esc> Jump
B $C5C1,1,1
W $C5C2,2,2 [$C6DC] Target
b $C5C4 [Stage 3] Map hazards data
B $C5C4,1,1 Wait for 96 units
B $C5C5,1,1 <Esc> Jump
B $C5C6,1,1
W $C5C7,2,2 [$C6FC] Target
b $C5C9 [Stage 3] Map left object data
B $C5C9,1,1 Alternating (OVERHEAD_BRIDGE, EMPTY) for 80 units
B $C5CA,80,8
B $C61A,1,1 OVERHEAD_BRIDGE for 13 units
B $C61B,1,1 EMPTY for 3 units
B $C61C,1,1 <Esc> Jump
B $C61D,1,1
W $C61E,2,2 [$C71F] Target
b $C620 [Stage 3] Map right object data
B $C620,1,1 Alternating (OVERHEAD_BRIDGE, EMPTY) for 80 units
B $C621,80,8
B $C671,1,1 OVERHEAD_BRIDGE for 13 units
B $C672,1,1 EMPTY for 3 units
B $C673,1,1 <Esc> Jump
B $C674,1,1
W $C675,2,2 [$C7AD] Target
b $C677 [Stage 3] Map curvature data
B $C677,1,1 Curve Straight for 20 units
B $C678,1,1
B $C679,1,1 Curve Right Hard for 45 units
B $C67A,2,2
B $C67C,1,1 Curve Right for 9 units
B $C67D,1,1 Curve Straight for 39 units
B $C67E,2,2
B $C680,1,1 Curve Left for 24 units
B $C681,1,1
B $C682,1,1 Curve Right for 15 units
B $C683,1,1 Curve Straight for 35 units
B $C684,2,2
B $C686,1,1 Curve Left for 25 units
B $C687,1,1
B $C688,1,1 Curve Left Hard for 92 units
B $C689,6,6
B $C68F,1,1 Curve Left for 56 units
B $C690,3,3
B $C693,1,1 Curve Straight for 84 units
B $C694,5,5
B $C699,1,1 <Esc> Loop
B $C69A,1,1
W $C69B,2,2 [$C677] Target
b $C69D [Stage 3] Map height data
B $C69D,1,1 Level Road for 14 units
B $C69E,1,1 Going Up 1 for 1 units
B $C69F,1,1 Going Up 3 for 40 units
B $C6A0,2,2
B $C6A2,1,1 Going Down 5 for 17 units
B $C6A3,1,1
B $C6A4,1,1 Going Down 3 for 6 units
B $C6A5,1,1 Going Down 1 for 5 units
B $C6A6,1,1 Level Road for 16 units
B $C6A7,1,1
B $C6A8,1,1 Going Down 5 for 2 units
B $C6A9,1,1 Going Down 7 for 5 units
B $C6AA,1,1 Going Down 5 for 6 units
B $C6AB,1,1 Going Down 3 for 2 units
B $C6AC,1,1 Going Down 1 for 2 units
B $C6AD,1,1 Going Up 1 for 2 units
B $C6AE,1,1 Going Up 3 for 2 units
B $C6AF,1,1 Going Up 5 for 4 units
B $C6B0,1,1 Going Up 7 for 10 units
B $C6B1,1,1 Going Down 7 for 1 units
B $C6B2,1,1 Going Down 5 for 1 units
B $C6B3,1,1 Going Down 3 for 2 units
B $C6B4,1,1 Going Down 1 for 1 units
B $C6B5,1,1 Level Road for 77 units
B $C6B6,5,5
B $C6BB,1,1 Going Down 7 for 1 units
B $C6BC,1,1 Level Road for 171 units
B $C6BD,11,8,3
B $C6C8,1,1 Going Up 3 for 3 units
B $C6C9,1,1 Going Up 5 for 2 units
B $C6CA,1,1 Going Up 7 for 4 units
B $C6CB,1,1 Going Up 5 for 1 units
B $C6CC,1,1 Going Down 5 for 1 units
B $C6CD,1,1 Going Down 7 for 3 units
B $C6CE,1,1 Going Down 5 for 1 units
B $C6CF,1,1 Going Down 3 for 1 units
B $C6D0,1,1 Going Up 3 for 1 units
B $C6D1,1,1 Going Up 5 for 1 units
B $C6D2,1,1 Going Up 7 for 3 units
B $C6D3,1,1 Going Down 5 for 1 units
B $C6D4,1,1 Going Down 3 for 14 units
B $C6D5,1,1 Going Down 1 for 4 units
B $C6D6,1,1 Level Road for 16 units
B $C6D7,1,1
B $C6D8,1,1 <Esc> Loop
B $C6D9,1,1
W $C6DA,2,2 [$C69D] Target
b $C6DC [Stage 3] Map lanes data
B $C6DC,1,1 4 Lanes              [||||] {00} for 82 units
B $C6DD,1,1
B $C6DE,1,1 4-3 Narrowing R      [/|||] {8E} for 8 units
B $C6DF,1,1
B $C6E0,1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C6E1,1,1
B $C6E2,1,1 4 Lanes              [||||] {00} for 50 units
B $C6E3,1,1
B $C6E4,1,1 4-3 Narrowing R      [/|||] {8E} for 4 units
B $C6E5,1,1
B $C6E6,1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $C6E7,1,1
B $C6E8,1,1 4 Lanes              [||||] {00} for 10 units
B $C6E9,1,1
B $C6EA,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C6EB,1,1
B $C6EC,1,1 3 Lanes L            [|||]  {81} for 2 units
B $C6ED,1,1
B $C6EE,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C6EF,1,1
B $C6F0,1,1 4 Lanes              [||||] {00} for 250 units
B $C6F1,1,1
B $C6F2,1,1 3 Lanes L            [|||]  {81} for 26 units
B $C6F3,1,1
B $C6F4,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $C6F5,1,1
B $C6F6,1,1 4 Lanes              [||||] {00} for 2 units
B $C6F7,1,1
B $C6F8,1,1 <Esc> Loop
B $C6F9,1,1
W $C6FA,2,2 [$C6DC] Target
b $C6FC [Stage 3] Map hazards data
B $C6FC,1,1 Wait for 18 units
B $C6FD,1,1 Start Spawning HAZARD_2 Left
B $C6FE,1,1
B $C6FF,1,1 Wait for 2 units
B $C700,1,1 Stop Spawning Hazards
B $C701,1,1
B $C702,1,1 Wait for 79 units
B $C703,1,1 Start Spawning HAZARD_2 Right
B $C704,1,1
B $C705,1,1 Wait for 2 units
B $C706,1,1 Stop Spawning Hazards
B $C707,1,1
B $C708,1,1 Wait for 19 units
B $C709,1,1 Start Spawning HAZARD_2 Right
B $C70A,1,1
B $C70B,1,1 Wait for 2 units
B $C70C,1,1 Stop Spawning Hazards
B $C70D,1,1
B $C70E,1,1 Wait for 85 units
B $C70F,1,1 Start Spawning HAZARD_2 Right
B $C710,1,1
B $C711,1,1 Wait for 1 units
B $C712,1,1 Stop Spawning Hazards
B $C713,1,1
B $C714,1,1 Wait for 8 units
B $C715,1,1 Start Spawning HAZARD_1 Left
B $C716,1,1
B $C717,1,1 Wait for 3 units
B $C718,1,1 Stop Spawning Hazards
B $C719,1,1
B $C71A,1,1 Wait for 3 units
B $C71B,1,1 <Esc> Loop
B $C71C,1,1
W $C71D,2,2 [$C6FC] Target
b $C71F [Stage 3] Map left object data
B $C71F,1,1 EMPTY for 4 units
B $C720,1,1 SPEED_LIMIT_SIGN for 1 units
B $C721,1,1 Alternating (TOWER_BLOCK, EMPTY) for 24 units
B $C722,24,8
B $C73A,1,1 TOWER_BLOCK for 6 units
B $C73B,1,1 EMPTY for 3 units
B $C73C,1,1 TOWER_BLOCK for 1 units
B $C73D,1,1 EMPTY for 3 units
B $C73E,1,1 TOWER_BLOCK for 1 units
B $C73F,1,1 EMPTY for 1 units
B $C740,1,1 TOWER_BLOCK for 1 units
B $C741,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 10 units
B $C742,10,8,2
B $C74C,1,1 TELEGRAPH_POLE for 4 units
B $C74D,1,1 EMPTY for 3 units
B $C74E,1,1 TELEGRAPH_POLE for 1 units
B $C74F,1,1 EMPTY for 3 units
B $C750,1,1 TELEGRAPH_POLE for 1 units
B $C751,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 8 units
B $C752,8,8
B $C75A,1,1 TELEGRAPH_POLE for 6 units
B $C75B,1,1 Alternating (TOWER_BLOCK, EMPTY) for 10 units
B $C75C,10,8,2
B $C766,1,1 TOWER_BLOCK for 4 units
B $C767,1,1 EMPTY for 9 units
B $C768,1,1 SPEED_LIMIT_SIGN for 1 units
B $C769,1,1 EMPTY for 21 units
B $C76A,1,1
B $C76B,1,1 SPEED_LIMIT_SIGN for 1 units
B $C76C,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 14 units
B $C76D,14,8,6
B $C77B,1,1 TELEGRAPH_POLE for 12 units
B $C77C,1,1 Alternating (TOWER_BLOCK, EMPTY) for 30 units
B $C77D,30,8*3,6
B $C79B,1,1 TOWER_BLOCK for 8 units
B $C79C,1,1 EMPTY for 5 units
B $C79D,1,1 SPEED_LIMIT_SIGN for 1 units
B $C79E,1,1 EMPTY for 5 units
B $C79F,1,1 SPEED_LIMIT_SIGN for 1 units
B $C7A0,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 6 units
B $C7A1,6,6
B $C7A7,1,1 TELEGRAPH_POLE for 10 units
B $C7A8,1,1 EMPTY for 3 units
B $C7A9,1,1 <Esc> Loop
B $C7AA,1,1
W $C7AB,2,2 [$C71F] Target
b $C7AD [Stage 3] Map right object data
B $C7AD,1,1 EMPTY for 4 units
B $C7AE,1,1 SPEED_LIMIT_SIGN for 1 units
B $C7AF,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 24 units
B $C7B0,25,8*3,1
B $C7C9,1,1 TELEGRAPH_POLE for 26 units
B $C7CA,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 12 units
B $C7CB,12,8,4
B $C7D7,1,1 TELEGRAPH_POLE for 10 units
B $C7D8,1,1 EMPTY for 3 units
B $C7D9,1,1 TELEGRAPH_POLE for 1 units
B $C7DA,1,1 EMPTY for 3 units
B $C7DB,1,1 TELEGRAPH_POLE for 1 units
B $C7DC,1,1 Alternating (TOWER_BLOCK, EMPTY) for 30 units
B $C7DD,31,8*3,7
B $C7FC,1,1 TOWER_BLOCK for 18 units
B $C7FD,1,1 EMPTY for 5 units
B $C7FE,1,1 TOWER_BLOCK for 1 units
B $C7FF,1,1 EMPTY for 1 units
B $C800,1,1 TOWER_BLOCK for 1 units
B $C801,1,1 Alternating (TOWER_BLOCK, EMPTY) for 8 units
B $C802,8,8
B $C80A,1,1 TOWER_BLOCK for 4 units
B $C80B,1,1 Alternating (TOWER_BLOCK, EMPTY) for 14 units
B $C80C,14,8,6
B $C81A,1,1 TOWER_BLOCK for 8 units
B $C81B,1,1 EMPTY for 5 units
B $C81C,1,1 TOWER_BLOCK for 1 units
B $C81D,1,1 Alternating (TOWER_BLOCK, EMPTY) for 4 units
B $C81E,4,4
B $C822,1,1 TOWER_BLOCK for 10 units
B $C823,1,1 EMPTY for 7 units
B $C824,1,1 SPEED_LIMIT_SIGN for 1 units
B $C825,1,1 EMPTY for 13 units
B $C826,1,1 SPEED_LIMIT_SIGN for 1 units
B $C827,1,1 EMPTY for 5 units
B $C828,1,1 <Esc> Loop
B $C829,1,1
W $C82A,2,2 [$C7AD] Target
b $C82C [Stage 3] Perp's mugshot
B $C82C,160,4 Bitmap data for the perp's mugshot (32x40). Stored top-down.
B $C8CC,20,4 Attribute data for the perp's mugshot (4x5). Stored top-down.
N $C8E0 LOD table for "Car A (the perp's car)"
N $C8E0 LOD
B $C8E0,1,1 Width (bytes)
B $C8E1,1,1 Flags
B $C8E2,1,1 Height (pixels)
W $C8E3,2,2 [$C95E] Bitmap address
W $C8E5,2,2 [$C95E] Pre-shifted bitmap address
N $C8E7 LOD
B $C8E7,1,1 Width (bytes)
B $C8E8,1,1 Flags
B $C8E9,1,1 Height (pixels)
W $C8EA,2,2 [$CA12] Bitmap address
W $C8EC,2,2 [$CA12] Pre-shifted bitmap address
N $C8EE LOD
B $C8EE,1,1 Width (bytes)
B $C8EF,1,1 Flags
B $C8F0,1,1 Height (pixels)
W $C8F1,2,2 [$CA62] Bitmap address
W $C8F3,2,2 [$CA62] Pre-shifted bitmap address
N $C8F5 LOD
B $C8F5,1,1 Width (bytes)
B $C8F6,1,1 Flags
B $C8F7,1,1 Height (pixels)
W $C8F8,2,2 [$CA62] Bitmap address
W $C8FA,2,2 [$CA62] Pre-shifted bitmap address
N $C8FC LOD
B $C8FC,1,1 Width (bytes)
B $C8FD,1,1 Flags
B $C8FE,1,1 Height (pixels)
W $C8FF,2,2 [$CA89] Bitmap address
W $C901,2,2 [$CA89] Pre-shifted bitmap address
N $C903 LOD
B $C903,1,1 Width (bytes)
B $C904,1,1 Flags
B $C905,1,1 Height (pixels)
W $C906,2,2 [$CA89] Bitmap address
W $C908,2,2 [$CAA9] Pre-shifted bitmap address
N $C90A LOD table for "Car C (a truck in S1)"
N $C90A LOD
B $C90A,1,1 Width (bytes)
B $C90B,1,1 Flags
B $C90C,1,1 Height (pixels)
W $C90D,2,2 [$CAC9] Bitmap address
W $C90F,2,2 [$CAC9] Pre-shifted bitmap address
N $C911 LOD
B $C911,1,1 Width (bytes)
B $C912,1,1 Flags
B $C913,1,1 Height (pixels)
W $C914,2,2 [$CB77] Bitmap address
W $C916,2,2 [$CB77] Pre-shifted bitmap address
N $C918 LOD
B $C918,1,1 Width (bytes)
B $C919,1,1 Flags
B $C91A,1,1 Height (pixels)
W $C91B,2,2 [$CBC3] Bitmap address
W $C91D,2,2 [$CBC3] Pre-shifted bitmap address
N $C91F LOD
B $C91F,1,1 Width (bytes)
B $C920,1,1 Flags
B $C921,1,1 Height (pixels)
W $C922,2,2 [$CBC3] Bitmap address
W $C924,2,2 [$CBC3] Pre-shifted bitmap address
N $C926 LOD
B $C926,1,1 Width (bytes)
B $C927,1,1 Flags
B $C928,1,1 Height (pixels)
W $C929,2,2 [$CBED] Bitmap address
W $C92B,2,2 [$CC0D] Pre-shifted bitmap address
N $C92D LOD
B $C92D,1,1 Width (bytes)
B $C92E,1,1 Flags
B $C92F,1,1 Height (pixels)
W $C930,2,2 [$CBED] Bitmap address
W $C932,2,2 [$CC0D] Pre-shifted bitmap address
N $C934 LOD table for "Car B (a Lambo in S1)"
N $C934 LOD
B $C934,1,1 Width (bytes)
B $C935,1,1 Flags
B $C936,1,1 Height (pixels)
W $C937,2,2 [$CC2D] Bitmap address
W $C939,2,2 [$CC2D] Pre-shifted bitmap address
N $C93B LOD
B $C93B,1,1 Width (bytes)
B $C93C,1,1 Flags
B $C93D,1,1 Height (pixels)
W $C93E,2,2 [$CCE7] Bitmap address
W $C940,2,2 [$CCE7] Pre-shifted bitmap address
N $C942 LOD
B $C942,1,1 Width (bytes)
B $C943,1,1 Flags
B $C944,1,1 Height (pixels)
W $C945,2,2 [$CD55] Bitmap address
W $C947,2,2 [$CD55] Pre-shifted bitmap address
N $C949 LOD
B $C949,1,1 Width (bytes)
B $C94A,1,1 Flags
B $C94B,1,1 Height (pixels)
W $C94C,2,2 [$CD55] Bitmap address
W $C94E,2,2 [$CD55] Pre-shifted bitmap address
N $C950 LOD
B $C950,1,1 Width (bytes)
B $C951,1,1 Flags
B $C952,1,1 Height (pixels)
W $C953,2,2 [$CD85] Bitmap address
W $C955,2,2 [$CDBB] Pre-shifted bitmap address
N $C957 LOD
B $C957,1,1 Width (bytes)
B $C958,1,1 Flags
B $C959,1,1 Height (pixels)
W $C95A,2,2 [$CD85] Bitmap address
W $C95C,2,2 [$CDBB] Pre-shifted bitmap address
B $C95E,180,6 Bitmap data 6 bytes x 30
B $CA12,80,4 Bitmap data 4 bytes x 20
B $CA62,39,3 Bitmap data 3 bytes x 13
B $CA89,32,4 Bitmap data (masked) 4 bytes x 8
B $CAA9,32,4 Pre-shifted bitmap data (masked) 4 bytes x 8
B $CAC9,174,6 Bitmap data 6 bytes x 29
B $CB77,76,4 Bitmap data 4 bytes x 19
B $CBC3,42,3 Bitmap data 3 bytes x 14
B $CBED,32,4 Bitmap data (masked) 4 bytes x 8
B $CC0D,32,4 Pre-shifted bitmap data (masked) 4 bytes x 8
B $CC2D,186,6 Bitmap data 6 bytes x 31
B $CCE7,110,5 Bitmap data 5 bytes x 22
B $CD55,48,3 Bitmap data 3 bytes x 16
B $CD85,54,6 Bitmap data (masked) 6 bytes x 9
B $CDBB,54,6 Pre-shifted bitmap data (masked) 6 bytes x 9
N $CDF1 LOD table for "Hazard (stone/dust)"
N $CDF1 LOD
B $CDF1,1,1 Width (bytes)
B $CDF2,1,1 Flags
B $CDF3,1,1 Height (pixels)
W $CDF4,2,2 [$CE45] Bitmap address
W $CDF6,2,2 [$CE45] Pre-shifted bitmap address
N $CDF8 LOD
B $CDF8,1,1 Width (bytes)
B $CDF9,1,1 Flags
B $CDFA,1,1 Height (pixels)
W $CDFB,2,2 [$CE59] Bitmap address
W $CDFD,2,2 [$CE69] Pre-shifted bitmap address
N $CDFF LOD
B $CDFF,1,1 Width (bytes)
B $CE00,1,1 Flags
B $CE01,1,1 Height (pixels)
W $CE02,2,2 [$CE79] Bitmap address
W $CE04,2,2 [$CE85] Pre-shifted bitmap address
N $CE06 LOD
B $CE06,1,1 Width (bytes)
B $CE07,1,1 Flags
B $CE08,1,1 Height (pixels)
W $CE09,2,2 [$CE91] Bitmap address
W $CE0B,2,2 [$CE95] Pre-shifted bitmap address
N $CE0D LOD
B $CE0D,1,1 Width (bytes)
B $CE0E,1,1 Flags
B $CE0F,1,1 Height (pixels)
W $CE10,2,2 [$CE91] Bitmap address
W $CE12,2,2 [$CE95] Pre-shifted bitmap address
N $CE14 LOD
B $CE14,1,1 Width (bytes)
B $CE15,1,1 Flags
B $CE16,1,1 Height (pixels)
W $CE17,2,2 [$CE99] Bitmap address
W $CE19,2,2 [$CE9B] Pre-shifted bitmap address
N $CE1B LOD table for "Hazard (stone/dust)"
N $CE1B LOD
B $CE1B,1,1 Width (bytes)
B $CE1C,1,1 Flags
B $CE1D,1,1 Height (pixels)
W $CE1E,2,2 [$CE9D] Bitmap address
W $CE20,2,2 [$CE9F] Pre-shifted bitmap address
N $CE22 LOD
B $CE22,1,1 Width (bytes)
B $CE23,1,1 Flags
B $CE24,1,1 Height (pixels)
W $CE25,2,2 [$CE9D] Bitmap address
W $CE27,2,2 [$CE9F] Pre-shifted bitmap address
N $CE29 LOD
B $CE29,1,1 Width (bytes)
B $CE2A,1,1 Flags
B $CE2B,1,1 Height (pixels)
W $CE2C,2,2 [$CE9D] Bitmap address
W $CE2E,2,2 [$CE9F] Pre-shifted bitmap address
N $CE30 LOD
B $CE30,1,1 Width (bytes)
B $CE31,1,1 Flags
B $CE32,1,1 Height (pixels)
W $CE33,2,2 [$CE9D] Bitmap address
W $CE35,2,2 [$CE9F] Pre-shifted bitmap address
N $CE37 LOD
B $CE37,1,1 Width (bytes)
B $CE38,1,1 Flags
B $CE39,1,1 Height (pixels)
W $CE3A,2,2 [$CE9D] Bitmap address
W $CE3C,2,2 [$CE9F] Pre-shifted bitmap address
N $CE3E LOD
B $CE3E,1,1 Width (bytes)
B $CE3F,1,1 Flags
B $CE40,1,1 Height (pixels)
W $CE41,2,2 [$CE9D] Bitmap address
W $CE43,2,2 [$CE9F] Pre-shifted bitmap address
B $CE45,20,4 Bitmap data (masked) 4 bytes x 5
B $CE59,16,4 Bitmap data (masked) 4 bytes x 4
B $CE69,16,4 Pre-shifted bitmap data (masked) 4 bytes x 4
B $CE79,12,4 Bitmap data (masked) 4 bytes x 3
B $CE85,12,4 Pre-shifted bitmap data (masked) 4 bytes x 3
B $CE91,4,2 Bitmap data (masked) 2 bytes x 2
B $CE95,4,2 Pre-shifted bitmap data (masked) 2 bytes x 2
B $CE99,2,2 Bitmap data (masked) 2 bytes x 1
B $CE9B,2,2 Pre-shifted bitmap data (masked) 2 bytes x 1
B $CE9D,2,2 Bitmap data (masked) 2 bytes x 1
B $CE9F,2,2 Pre-shifted bitmap data (masked) 2 bytes x 1
N $CEA1 Stretchy graphic
B $CEA1,1,1 ?index
W $CEA2,2,2 [$CEAB] Pointer to stretchy_graphic_part
B $CEA4,1,1 ?index
W $CEA5,2,2 [$CEC1] Pointer to stretchy_graphic_part
B $CEA7,1,1 ?index
W $CEA8,2,2 [$CED7] Pointer to stretchy_graphic_part
B $CEAA,1,1 Terminator
N $CEAB Stretchy graphic part
W $CEAB,2,2 [$CEED] LOD ptr
W $CEAD,2,2 TBD
W $CEAF,2,2 TBD
W $CEB1,2,2 TBD
W $CEB3,2,2 TBD
W $CEB5,2,2 TBD
W $CEB7,2,2 TBD
W $CEB9,2,2 TBD
W $CEBB,2,2 TBD
W $CEBD,2,2 TBD
W $CEBF,2,2 TBD
N $CEC1 Stretchy graphic part
W $CEC1,2,2 [$CEED] LOD ptr
W $CEC3,2,2 TBD
W $CEC5,2,2 TBD
W $CEC7,2,2 TBD
W $CEC9,2,2 TBD
W $CECB,2,2 TBD
W $CECD,2,2 TBD
W $CECF,2,2 TBD
W $CED1,2,2 TBD
W $CED3,2,2 TBD
W $CED5,2,2 TBD
N $CED7 Stretchy graphic part
W $CED7,2,2 [$CEED] LOD ptr
W $CED9,2,2 TBD
W $CEDB,2,2 TBD
W $CEDD,2,2 TBD
W $CEDF,2,2 TBD
W $CEE1,2,2 TBD
W $CEE3,2,2 TBD
W $CEE5,2,2 TBD
W $CEE7,2,2 TBD
W $CEE9,2,2 TBD
W $CEEB,2,2 TBD
N $CEED LOD table for "stretchy"
N $CEED LOD
B $CEED,1,1 Width (bytes)
B $CEEE,1,1 Flags
B $CEEF,1,1 Height (pixels)
W $CEF0,2,2 [$CF6B] Bitmap address
W $CEF2,2,2 [$CF6B] Pre-shifted bitmap address
N $CEF4 LOD
B $CEF4,1,1 Width (bytes)
B $CEF5,1,1 Flags
B $CEF6,1,1 Height (pixels)
W $CEF7,2,2 [$D0AB] Bitmap address
W $CEF9,2,2 [$D0AB] Pre-shifted bitmap address
N $CEFB LOD
B $CEFB,1,1 Width (bytes)
B $CEFC,1,1 Flags
B $CEFD,1,1 Height (pixels)
W $CEFE,2,2 [$D17D] Bitmap address
W $CF00,2,2 [$D17D] Pre-shifted bitmap address
N $CF02 LOD
B $CF02,1,1 Width (bytes)
B $CF03,1,1 Flags
B $CF04,1,1 Height (pixels)
W $CF05,2,2 [$D20E] Bitmap address
W $CF07,2,2 [$D20E] Pre-shifted bitmap address
N $CF09 LOD
B $CF09,1,1 Width (bytes)
B $CF0A,1,1 Flags
B $CF0B,1,1 Height (pixels)
W $CF0C,2,2 [$D262] Bitmap address
W $CF0E,2,2 [$D262] Pre-shifted bitmap address
B $CF10,91,8*11,3
B $CF6B,256,8 Bitmap data 8 bytes x 32
B $D06B,64,8
B $D0AB,150,6 Bitmap data 6 bytes x 25
B $D141,60,8*7,4
B $D17D,100,5 Bitmap data 5 bytes x 20
B $D1E1,45,8*5,5
B $D20E,56,4 Bitmap data 4 bytes x 14
B $D246,28,8*3,4
B $D262,33,3 Bitmap data 3 bytes x 11
B $D283,56,8
N $D2BB Stretchy graphic
B $D2BB,1,1 ?index
W $D2BC,2,2 [$D2C2] Pointer to stretchy_graphic_part
B $D2BE,1,1 ?index
W $D2BF,2,2 [$D2D8] Pointer to stretchy_graphic_part
B $D2C1,1,1 Terminator
N $D2C2 Stretchy graphic part
W $D2C2,2,2 [$D2EE] LOD ptr
W $D2C4,2,2 TBD
W $D2C6,2,2 TBD
W $D2C8,2,2 TBD
W $D2CA,2,2 TBD
W $D2CC,2,2 TBD
W $D2CE,2,2 TBD
W $D2D0,2,2 TBD
W $D2D2,2,2 TBD
W $D2D4,2,2 TBD
W $D2D6,2,2 TBD
N $D2D8 Stretchy graphic part
W $D2D8,2,2 [$D2EE] LOD ptr
W $D2DA,2,2 TBD
W $D2DC,2,2 TBD
W $D2DE,2,2 TBD
W $D2E0,2,2 TBD
W $D2E2,2,2 TBD
W $D2E4,2,2 TBD
W $D2E6,2,2 TBD
W $D2E8,2,2 TBD
W $D2EA,2,2 TBD
W $D2EC,2,2 TBD
N $D2EE LOD table for "stretchy"
N $D2EE LOD
B $D2EE,1,1 Width (bytes)
B $D2EF,1,1 Flags
B $D2F0,1,1 Height (pixels)
W $D2F1,2,2 [$D467] Bitmap address
W $D2F3,2,2 [$D467] Pre-shifted bitmap address
N $D2F5 LOD
B $D2F5,1,1 Width (bytes)
B $D2F6,1,1 Flags
B $D2F7,1,1 Height (pixels)
W $D2F8,2,2 [$D576] Bitmap address
W $D2FA,2,2 [$D576] Pre-shifted bitmap address
N $D2FC LOD
B $D2FC,1,1 Width (bytes)
B $D2FD,1,1 Flags
B $D2FE,1,1 Height (pixels)
W $D2FF,2,2 [$D604] Bitmap address
W $D301,2,2 [$D604] Pre-shifted bitmap address
N $D303 LOD
B $D303,1,1 Width (bytes)
B $D304,1,1 Flags
B $D305,1,1 Height (pixels)
W $D306,2,2 [$D63D] Bitmap address
W $D308,2,2 [$D63D] Pre-shifted bitmap address
N $D30A LOD
B $D30A,1,1 Width (bytes)
B $D30B,1,1 Flags
B $D30C,1,1 Height (pixels)
W $D30D,2,2 [$D4A7] Bitmap address
W $D30F,2,2 [$D4A7] Pre-shifted bitmap address
N $D311 LOD
B $D311,1,1 Width (bytes)
B $D312,1,1 Flags
B $D313,1,1 Height (pixels)
W $D314,2,2 [$D59A] Bitmap address
W $D316,2,2 [$D59A] Pre-shifted bitmap address
N $D318 LOD
B $D318,1,1 Width (bytes)
B $D319,1,1 Flags
B $D31A,1,1 Height (pixels)
W $D31B,2,2 [$D614] Bitmap address
W $D31D,2,2 [$D614] Pre-shifted bitmap address
N $D31F LOD
B $D31F,1,1 Width (bytes)
B $D320,1,1 Flags
B $D321,1,1 Height (pixels)
W $D322,2,2 [$D641] Bitmap address
W $D324,2,2 [$D641] Pre-shifted bitmap address
N $D326 Stretchy graphic
B $D326,1,1 ?index
W $D327,2,2 [$D32D] Pointer to stretchy_graphic_part
B $D329,1,1 ?index
W $D32A,2,2 [$D361] Pointer to stretchy_graphic_part
B $D32C,1,1 Terminator
N $D32D Stretchy graphic part
W $D32D,2,2 [$D377] LOD ptr
W $D32F,2,2 TBD
W $D331,2,2 TBD
W $D333,2,2 TBD
W $D335,2,2 TBD
W $D337,2,2 TBD
W $D339,2,2 TBD
W $D33B,2,2 TBD
W $D33D,2,2 TBD
W $D33F,2,2 TBD
W $D341,2,2 TBD
N $D343 Bitmap data 3 bytes x 10
B $D343,30,8*3,6
N $D361 Stretchy graphic part
W $D361,2,2 [$D377] LOD ptr
W $D363,2,2 TBD
W $D365,2,2 TBD
W $D367,2,2 TBD
W $D369,2,2 TBD
W $D36B,2,2 TBD
W $D36D,2,2 TBD
W $D36F,2,2 TBD
W $D371,2,2 TBD
W $D373,2,2 TBD
W $D375,2,2 TBD
N $D377 LOD table for "stretchy"
N $D377 LOD
B $D377,1,1 Width (bytes)
B $D378,1,1 Flags
B $D379,1,1 Height (pixels)
W $D37A,2,2 [$D3AF] Bitmap address
W $D37C,2,2 [$D3AF] Pre-shifted bitmap address
N $D37E LOD
B $D37E,1,1 Width (bytes)
B $D37F,1,1 Flags
B $D380,1,1 Height (pixels)
W $D381,2,2 [$D507] Bitmap address
W $D383,2,2 [$D507] Pre-shifted bitmap address
N $D385 LOD
B $D385,1,1 Width (bytes)
B $D386,1,1 Flags
B $D387,1,1 Height (pixels)
W $D388,2,2 [$D5D0] Bitmap address
W $D38A,2,2 [$D5D0] Pre-shifted bitmap address
N $D38C LOD
B $D38C,1,1 Width (bytes)
B $D38D,1,1 Flags
B $D38E,1,1 Height (pixels)
W $D38F,2,2 [$D62C] Bitmap address
W $D391,2,2 [$D62C] Pre-shifted bitmap address
N $D393 LOD
B $D393,1,1 Width (bytes)
B $D394,1,1 Flags
B $D395,1,1 Height (pixels)
W $D396,2,2 [$D3EF] Bitmap address
W $D398,2,2 [$D3EF] Pre-shifted bitmap address
N $D39A LOD
B $D39A,1,1 Width (bytes)
B $D39B,1,1 Flags
B $D39C,1,1 Height (pixels)
W $D39D,2,2 [$D52B] Bitmap address
W $D39F,2,2 [$D52B] Pre-shifted bitmap address
N $D3A1 LOD
B $D3A1,1,1 Width (bytes)
B $D3A2,1,1 Flags
B $D3A3,1,1 Height (pixels)
W $D3A4,2,2 [$D5E0] Bitmap address
W $D3A6,2,2 [$D5E0] Pre-shifted bitmap address
N $D3A8 LOD
B $D3A8,1,1 Width (bytes)
B $D3A9,1,1 Flags
B $D3AA,1,1 Height (pixels)
W $D3AB,2,2 [$D630] Bitmap address
W $D3AD,2,2 [$D630] Pre-shifted bitmap address
B $D3AF,64,4 Bitmap data 4 bytes x 16
B $D3EF,96,4 Bitmap data 4 bytes x 24
B $D44F,24,8
B $D467,64,4 Bitmap data 4 bytes x 16
B $D4A7,96,4 Bitmap data 4 bytes x 24
B $D507,111,8*13,7
B $D576,36,3 Bitmap data 3 bytes x 12
B $D59A,106,8*13,2
B $D604,16,2 Bitmap data 2 bytes x 8
B $D614,41,8*5,1
B $D63D,4,1 Bitmap data 1 bytes x 4
B $D641,14,8,6
N $D64F LOD table for "hittable hazard"
N $D64F LOD
B $D64F,1,1 Width (bytes)
B $D650,1,1 Flags
B $D651,1,1 Height (pixels)
W $D652,2,2 [$D679] Bitmap address
W $D654,2,2 [$D679] Pre-shifted bitmap address
N $D656 LOD
B $D656,1,1 Width (bytes)
B $D657,1,1 Flags
B $D658,1,1 Height (pixels)
W $D659,2,2 [$D699] Bitmap address
W $D65B,2,2 [$D699] Pre-shifted bitmap address
N $D65D LOD
B $D65D,1,1 Width (bytes)
B $D65E,1,1 Flags
B $D65F,1,1 Height (pixels)
W $D660,2,2 [$D6AF] Bitmap address
W $D662,2,2 [$D6AF] Pre-shifted bitmap address
N $D664 LOD
B $D664,1,1 Width (bytes)
B $D665,1,1 Flags
B $D666,1,1 Height (pixels)
W $D667,2,2 [$D6AF] Bitmap address
W $D669,2,2 [$D6AF] Pre-shifted bitmap address
N $D66B LOD
B $D66B,1,1 Width (bytes)
B $D66C,1,1 Flags
B $D66D,1,1 Height (pixels)
W $D66E,2,2 [$D6B8] Bitmap address
W $D670,2,2 [$D6B8] Pre-shifted bitmap address
N $D672 LOD
B $D672,1,1 Width (bytes)
B $D673,1,1 Flags
B $D674,1,1 Height (pixels)
W $D675,2,2 [$D6B8] Bitmap address
W $D677,2,2 [$D6B8] Pre-shifted bitmap address
B $D679,32,2 Bitmap data 2 bytes x 16
B $D699,22,2 Bitmap data 2 bytes x 11
B $D6AF,9,1 Bitmap data 1 bytes x 9
B $D6B8,7,1 Bitmap data 1 bytes x 7
N $D6BF LOD table for "hittable hazard"
N $D6BF LOD
B $D6BF,1,1 Width (bytes)
B $D6C0,1,1 Flags
B $D6C1,1,1 Height (pixels)
W $D6C2,2,2 [$D6E9] Bitmap address
W $D6C4,2,2 [$D6E9] Pre-shifted bitmap address
N $D6C6 LOD
B $D6C6,1,1 Width (bytes)
B $D6C7,1,1 Flags
B $D6C8,1,1 Height (pixels)
W $D6C9,2,2 [$D6E9] Bitmap address
W $D6CB,2,2 [$D6E9] Pre-shifted bitmap address
N $D6CD LOD
B $D6CD,1,1 Width (bytes)
B $D6CE,1,1 Flags
B $D6CF,1,1 Height (pixels)
W $D6D0,2,2 [$D72D] Bitmap address
W $D6D2,2,2 [$D72D] Pre-shifted bitmap address
N $D6D4 LOD
B $D6D4,1,1 Width (bytes)
B $D6D5,1,1 Flags
B $D6D6,1,1 Height (pixels)
W $D6D7,2,2 [$D754] Bitmap address
W $D6D9,2,2 [$D754] Pre-shifted bitmap address
N $D6DB LOD
B $D6DB,1,1 Width (bytes)
B $D6DC,1,1 Flags
B $D6DD,1,1 Height (pixels)
W $D6DE,2,2 [$D754] Bitmap address
W $D6E0,2,2 [$D754] Pre-shifted bitmap address
N $D6E2 LOD
B $D6E2,1,1 Width (bytes)
B $D6E3,1,1 Flags
B $D6E4,1,1 Height (pixels)
W $D6E5,2,2 [$D766] Bitmap address
W $D6E7,2,2 [$D782] Pre-shifted bitmap address
B $D6E9,68,4 Bitmap data 4 bytes x 17
B $D72D,39,3 Bitmap data 3 bytes x 13
B $D754,18,2 Bitmap data 2 bytes x 9
B $D766,28,4 Bitmap data (masked) 4 bytes x 7
B $D782,28,4 Pre-shifted bitmap data (masked) 4 bytes x 7
N $D79E Stretchy graphic
B $D79E,1,1 ?index
W $D79F,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D7A1,1,1 ?index
W $D7A2,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D7A4,1,1 ?index
W $D7A5,2,2 [$D7C8] Pointer to stretchy_graphic_part
B $D7A7,1,1 Terminator
N $D7A8 Stretchy graphic
B $D7A8,1,1 ?index
W $D7A9,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D7AB,1,1 ?index
W $D7AC,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D7AE,1,1 ?index
W $D7AF,2,2 [$D7B2] Pointer to stretchy_graphic_part
B $D7B1,1,1 Terminator
N $D7B2 Stretchy graphic part
W $D7B2,2,2 [$D7DE] LOD ptr
W $D7B4,2,2 TBD
W $D7B6,2,2 TBD
W $D7B8,2,2 TBD
W $D7BA,2,2 TBD
W $D7BC,2,2 TBD
W $D7BE,2,2 TBD
W $D7C0,2,2 TBD
W $D7C2,2,2 TBD
W $D7C4,2,2 TBD
W $D7C6,2,2 TBD
N $D7C8 Stretchy graphic part
W $D7C8,2,2 [$D7DE] LOD ptr
W $D7CA,2,2 TBD
W $D7CC,2,2 TBD
W $D7CE,2,2 TBD
W $D7D0,2,2 TBD
W $D7D2,2,2 TBD
W $D7D4,2,2 TBD
W $D7D6,2,2 TBD
W $D7D8,2,2 TBD
W $D7DA,2,2 TBD
W $D7DC,2,2 TBD
N $D7DE LOD table for "stretchy"
N $D7DE LOD
B $D7DE,1,1 Width (bytes)
B $D7DF,1,1 Flags
B $D7E0,1,1 Height (pixels)
W $D7E1,2,2 [$D801] Bitmap address
W $D7E3,2,2 [$D801] Pre-shifted bitmap address
N $D7E5 LOD
B $D7E5,1,1 Width (bytes)
B $D7E6,1,1 Flags
B $D7E7,1,1 Height (pixels)
W $D7E8,2,2 [$D852] Bitmap address
W $D7EA,2,2 [$D852] Pre-shifted bitmap address
N $D7EC LOD
B $D7EC,1,1 Width (bytes)
B $D7ED,1,1 Flags
B $D7EE,1,1 Height (pixels)
W $D7EF,2,2 [$D87A] Bitmap address
W $D7F1,2,2 [$D8D4] Pre-shifted bitmap address
N $D7F3 LOD
B $D7F3,1,1 Width (bytes)
B $D7F4,1,1 Flags
B $D7F5,1,1 Height (pixels)
W $D7F6,2,2 [$D92E] Bitmap address
W $D7F8,2,2 [$D95E] Pre-shifted bitmap address
N $D7FA LOD
B $D7FA,1,1 Width (bytes)
B $D7FB,1,1 Flags
B $D7FC,1,1 Height (pixels)
W $D7FD,2,2 [$D98E] Bitmap address
W $D7FF,2,2 [$D9B2] Pre-shifted bitmap address
B $D801,81,3 Bitmap data 3 bytes x 27
B $D852,40,2 Bitmap data 2 bytes x 20
B $D87A,90,6 Bitmap data (masked) 6 bytes x 15
B $D8D4,90,6 Pre-shifted bitmap data (masked) 6 bytes x 15
B $D92E,48,4 Bitmap data (masked) 4 bytes x 12
B $D95E,48,4 Pre-shifted bitmap data (masked) 4 bytes x 12
B $D98E,36,4 Bitmap data (masked) 4 bytes x 9
B $D9B2,36,4 Pre-shifted bitmap data (masked) 4 bytes x 9
N $D9D6 Stretchy graphic
B $D9D6,1,1 ?index
W $D9D7,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D9D9,1,1 ?index
W $D9DA,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D9DC,1,1 ?index
W $D9DD,2,2 [$DA00] Pointer to stretchy_graphic_part
B $D9DF,1,1 Terminator
N $D9E0 Stretchy graphic
B $D9E0,1,1 ?index
W $D9E1,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D9E3,1,1 ?index
W $D9E4,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D9E6,1,1 ?index
W $D9E7,2,2 [$D9EA] Pointer to stretchy_graphic_part
B $D9E9,1,1 Terminator
N $D9EA Stretchy graphic part
W $D9EA,2,2 [$DA16] LOD ptr
W $D9EC,2,2 TBD
W $D9EE,2,2 TBD
W $D9F0,2,2 TBD
W $D9F2,2,2 TBD
W $D9F4,2,2 TBD
W $D9F6,2,2 TBD
W $D9F8,2,2 TBD
W $D9FA,2,2 TBD
W $D9FC,2,2 TBD
W $D9FE,2,2 TBD
N $DA00 Stretchy graphic part
W $DA00,2,2 [$DA16] LOD ptr
W $DA02,2,2 TBD
W $DA04,2,2 TBD
W $DA06,2,2 TBD
W $DA08,2,2 TBD
W $DA0A,2,2 TBD
W $DA0C,2,2 TBD
W $DA0E,2,2 TBD
W $DA10,2,2 TBD
W $DA12,2,2 TBD
W $DA14,2,2 TBD
N $DA16 LOD table for "stretchy"
N $DA16 LOD
B $DA16,1,1 Width (bytes)
B $DA17,1,1 Flags
B $DA18,1,1 Height (pixels)
W $DA19,2,2 [$DA39] Bitmap address
W $DA1B,2,2 [$DA39] Pre-shifted bitmap address
N $DA1D LOD
B $DA1D,1,1 Width (bytes)
B $DA1E,1,1 Flags
B $DA1F,1,1 Height (pixels)
W $DA20,2,2 [$DA60] Bitmap address
W $DA22,2,2 [$DA60] Pre-shifted bitmap address
N $DA24 LOD
B $DA24,1,1 Width (bytes)
B $DA25,1,1 Flags
B $DA26,1,1 Height (pixels)
W $DA27,2,2 [$DA7E] Bitmap address
W $DA29,2,2 [$DA93] Pre-shifted bitmap address
N $DA2B LOD
B $DA2B,1,1 Width (bytes)
B $DA2C,1,1 Flags
B $DA2D,1,1 Height (pixels)
W $DA2E,2,2 [$DAA8] Bitmap address
W $DA30,2,2 [$DABC] Pre-shifted bitmap address
N $DA32 LOD
B $DA32,1,1 Width (bytes)
B $DA33,1,1 Flags
B $DA34,1,1 Height (pixels)
W $DA35,2,2 [$DAD0] Bitmap address
W $DA37,2,2 [$DAE0] Pre-shifted bitmap address
B $DA39,39,3 Bitmap data 3 bytes x 13
B $DA60,30,3 Bitmap data 3 bytes x 10
B $DA7E,21,3 Bitmap data 3 bytes x 7
B $DA93,21,3 Pre-shifted bitmap data 3 bytes x 7
B $DAA8,20,4 Bitmap data (masked) 4 bytes x 5
B $DABC,20,4 Pre-shifted bitmap data (masked) 4 bytes x 5
B $DAD0,16,4 Bitmap data (masked) 4 bytes x 4
B $DAE0,16,4 Pre-shifted bitmap data (masked) 4 bytes x 4
B $DAF0,1296,8
b $E000 [Stage 4] Horizon graphic
B $E000,240,8
b $E0F0 [Stage 4] Per-stage data
@ $E0F0 label=stage4
W $E0F0,2,2 [$E7DE] Address of perp's mugshot (attributes)
W $E0F2,2,2 [$EF35] Address of pilot's mugshot (bitmap)
W $E0F4,2,2 [out-of-bounds] Screen attributes used for the ground colour (a pair of matching bytes)
W $E0F6,2,2 [$E218] Address of table of LODs for tumbleweeds, barriers.
W $E0F8,2,2 [$E21A] (points at a handler address)
W $E0FA,2,2 [$E217] Address of right hand graphics entry/entries (-7 bytes)
W $E0FC,2,2 [$E22C] (points at a handler address)
W $E0FE,2,2 [$E259] (points at a handler address)
W $E100,2,2 [$E256] Address of left hand graphics entry/entries (-7 bytes)
W $E102,2,2 [$E26B] (points at a handler address)
W $E104,2,2 [$E139] Address of Nancy's perp description
W $E106,2,2 [$E1D5] Address of arrest messages
W $E108,2,2 [$EC6E] Helicopter data 1
W $E10A,2,2 [$EC7A] Helicopter data 2
w $E10C [Stage 4] Table of addresses of LODs
W $E10C,2,2 [out-of-bounds] Address of LOD of Hazard (stone/dust)
W $E10E,2,2 [out-of-bounds] Address of LOD of Hazard (stone/dust)
W $E110,2,2 [$E7F2] Address of LOD of Car A (the perp's car)
W $E112,2,2 [$E846] Address of LOD of Car B (a Lambo in S1)
W $E114,2,2 [$E81C] Address of LOD of Car C (a truck in S1)
W $E116,2,2 [$E846] Address of LOD of Car D (a Lambo in S1)
W $E118,2,2 [$E7F2] Address of LOD of Car E (a generic car in S1)
b $E11A [Stage 4] Per-stage difficulty settings
B $E11A,1,1 How often cars spawn. Lower values spawn cars more often.
B $E11B,1,1 Smash config parameter TBD
B $E11C,1,1 Smash config parameter TBD
w $E11D [Stage 4] Per-stage setup data
W $E11D,2,2 road_pos
W $E11F,2,2 [$E29B] Address of start stretch, curvature
W $E121,2,2 [$E2C3] Address of start stretch, height
W $E123,2,2 [$E303] Address of start stretch, lanes
W $E125,2,2 [$E3CD] Address of start stretch, right-side objects
W $E127,2,2 [$E335] Address of start stretch, left-side objects
W $E129,2,2 [$E30D] Address of start stretch, hazards
w $E12B [Stage 4] Per-stage attract mode data
W $E12B,2,2 road_pos
W $E12D,2,2 [$E576] Address of loop section, curvature
W $E12F,2,2 [$E599] Address of loop section, height
W $E131,2,2 [$E5D2] Address of loop section, lanes
W $E133,2,2 [$E6B6] Address of loop section, right-side objects
W $E135,2,2 [$E623] Address of loop section, left-side objects
W $E137,2,2 [$E5FA] Address of loop section, hazards
b $E139 [Stage 4] Nancy's perp description
@ $E139 label=stage4_perp_description
B $E139,1,1 Character identifier (0/1/2/3 = Pilot/Nancy/Raymond/Tony)
W $E13A,2,2 [$E145] Perp description pointer
W $E13C,2,2 [$E16D] Perp description pointer
W $E13E,2,2 [$E192] Perp description pointer
W $E140,2,2 [$E1B8] Perp description pointer
B $E142,1,1 Escape: Jump
W $E143,2,2 Address of next message (always $98BD)
T $E145,40,39:n1 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
T $E16D,37,36:n1 "EMERGENCY HERE, THE L.A. KIDNAPPER IS"
T $E192,38,37:n1 "SPEEDING TOWARDS THE OUTSKIRTS OF TOWN"
T $E1B8,29,28:n1 "IN A BLUE TWO SEATER... OVER."
b $E1D5 [Stage 4] Arrest messages
B $E1D5,1,1 ?frame delay until first message
B $E1D6,1,1 ?frame delay until next message
B $E1D7,1,1 Flags
B $E1D8,1,1 Attribute
W $E1D9,2,2 Back buffer address
W $E1DB,2,2 Attribute address
T $E1DD,27,26:n1 "OK! YOU ARE UNDER ARREST ON"
B $E1F8,1,1 ?frame delay until next message
B $E1F9,1,1 Flags
B $E1FA,1,1 Attribute
W $E1FB,2,2 Back buffer address
W $E1FD,2,2 Attribute address
T $E1FF,23,22:n1 "SUSPICION OF KIDNAPPING"
B $E216,1,1 ?frame delay until next message
B $E217,1,1 Stop
b $E218 [Stage 4] Hittable hazards
B $E218,1,1 ?id
W $E219,2,2 [$F4D8] Address of LODs
B $E21B,1,1 ?id
W $E21C,2,2 [$F615] Address of LODs
b $E21E [Stage 4] Object graphic definitions (right)
N $E21E Graphic definition for object 1 - TUNNEL_LIGHT
B $E21E,1,1 Hit coord max (furthest)
B $E21F,1,1 Hit coord min (nearest)
B $E220,1,1 How far to push hero car away if hit
W $E221,2,2 Argument - tunnellight
W $E223,2,2 [out-of-bounds] Address of routine draw_tunnel_light_right
N $E225 Graphic definition for object 2 - (object 2 - unused)
B $E225,1,1 Hit coord max (furthest)
B $E226,1,1 Hit coord min (nearest)
B $E227,1,1 How far to push hero car away if hit
W $E228,2,2 Argument - (null)
W $E22A,2,2 [out-of-bounds] Address of routine (null)
N $E22C Graphic definition for object 3 - SHORT_POLE
B $E22C,1,1 Hit coord max (furthest)
B $E22D,1,1 Hit coord min (nearest)
B $E22E,1,1 How far to push hero car away if hit
W $E22F,2,2 Argument - stretchy_shortpole
W $E231,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $E233 Graphic definition for object 4 - NEAR_COLUMN
B $E233,1,1 Hit coord max (furthest)
B $E234,1,1 Hit coord min (nearest)
B $E235,1,1 How far to push hero car away if hit
W $E236,2,2 [$EFE9] Argument for routine passed in #REGde
W $E238,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $E23A Graphic definition for object 5 - FAR_COLUMN
B $E23A,1,1 Hit coord max (furthest)
B $E23B,1,1 Hit coord min (nearest)
B $E23C,1,1 How far to push hero car away if hit
W $E23D,2,2 [$F035] Argument for routine passed in #REGde
W $E23F,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $E241 Graphic definition for object 6 - PILE_OF_ROCKS
B $E241,1,1 Hit coord max (furthest)
B $E242,1,1 Hit coord min (nearest)
B $E243,1,1 How far to push hero car away if hit
W $E244,2,2 [$F25A] Argument for routine passed in #REGde
W $E246,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $E248 Graphic definition for object 7 - STREET_LAMP
B $E248,1,1 Hit coord max (furthest)
B $E249,1,1 Hit coord min (nearest)
B $E24A,1,1 How far to push hero car away if hit
W $E24B,2,2 [$F6F4] Argument for routine passed in #REGde
W $E24D,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $E24F Graphic definition for object 8 - TURN_SIGN_POINTING_LEFT
B $E24F,1,1 Hit coord max (furthest)
B $E250,1,1 Hit coord min (nearest)
B $E251,1,1 How far to push hero car away if hit
W $E252,2,2 [$F815] Argument for routine passed in #REGde
W $E254,2,2 [out-of-bounds] Address of routine draw_object_right
N $E256 Graphic definition for object 9 - TURN_SIGN_POINTING_RIGHT
B $E256,1,1 Hit coord max (furthest)
B $E257,1,1 Hit coord min (nearest)
B $E258,1,1 How far to push hero car away if hit
W $E259,2,2 [$F7FF] Argument for routine passed in #REGde
W $E25B,2,2 [out-of-bounds] Address of routine draw_object_right
b $E25D [Stage 4] Object graphic definitions (left)
N $E25D Graphic definition for object 1 - TUNNEL_LIGHT
B $E25D,1,1 Hit coord min (furthest)
B $E25E,1,1 Hit coord max (nearest)
B $E25F,1,1 How far to push hero car away if hit
W $E260,2,2 Argument - tunnellight
W $E262,2,2 [out-of-bounds] Address of routine draw_tunnel_light_left
N $E264 Graphic definition for object 2 - (object 2 - unused)
B $E264,1,1 Hit coord min (furthest)
B $E265,1,1 Hit coord max (nearest)
B $E266,1,1 How far to push hero car away if hit
W $E267,2,2 Argument - (null)
W $E269,2,2 [out-of-bounds] Address of routine (null)
N $E26B Graphic definition for object 3 - SHORT_POLE
B $E26B,1,1 Hit coord min (furthest)
B $E26C,1,1 Hit coord max (nearest)
B $E26D,1,1 How far to push hero car away if hit
W $E26E,2,2 Argument - stretchy_shortpole
W $E270,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
N $E272 Graphic definition for object 4 - NEAR_COLUMN
B $E272,1,1 Hit coord min (furthest)
B $E273,1,1 Hit coord max (nearest)
B $E274,1,1 How far to push hero car away if hit
W $E275,2,2 [$EFE9] Argument for routine passed in #REGde
W $E277,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
N $E279 Graphic definition for object 5 - FAR_COLUMN
B $E279,1,1 Hit coord min (furthest)
B $E27A,1,1 Hit coord max (nearest)
B $E27B,1,1 How far to push hero car away if hit
W $E27C,2,2 [$F035] Argument for routine passed in #REGde
W $E27E,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
N $E280 Graphic definition for object 6 - PILE_OF_ROCKS
B $E280,1,1 Hit coord min (furthest)
B $E281,1,1 Hit coord max (nearest)
B $E282,1,1 How far to push hero car away if hit
W $E283,2,2 [$F267] Argument for routine passed in #REGde
W $E285,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
N $E287 Graphic definition for object 7 - STREET_LAMP
B $E287,1,1 Hit coord min (furthest)
B $E288,1,1 Hit coord max (nearest)
B $E289,1,1 How far to push hero car away if hit
W $E28A,2,2 [$F701] Argument for routine passed in #REGde
W $E28C,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
N $E28E Graphic definition for object 8 - TURN_SIGN_POINTING_LEFT
B $E28E,1,1 Hit coord min (furthest)
B $E28F,1,1 Hit coord max (nearest)
B $E290,1,1 How far to push hero car away if hit
W $E291,2,2 [$F815] Argument for routine passed in #REGde
W $E293,2,2 [out-of-bounds] Address of routine draw_object_left
N $E295 Graphic definition for object 9 - TURN_SIGN_POINTING_RIGHT
B $E295,1,1 Hit coord min (furthest)
B $E296,1,1 Hit coord max (nearest)
B $E297,1,1 How far to push hero car away if hit
W $E298,2,2 [$F7FF] Argument for routine passed in #REGde
W $E29A,2,2 [out-of-bounds] Address of routine draw_object_left
b $E29C [Stage 4] Map curvature data
B $E29C,1,1 Curve Straight for 54 units
B $E29D,3,3
B $E2A0,1,1 Curve Right Hard for 11 units
B $E2A1,1,1 Curve Right Very Hard for 6 units
B $E2A2,1,1 Curve Right Hard for 10 units
B $E2A3,1,1 Curve Straight for 40 units
B $E2A4,2,2
B $E2A6,1,1 Curve Left for 50 units
B $E2A7,3,3
B $E2AA,1,1 Curve Right for 50 units
B $E2AB,3,3
B $E2AE,1,1 Curve Straight for 39 units
B $E2AF,2,2
B $E2B1,1,1 Curve Left Hard for 12 units
B $E2B2,1,1 Curve Left Very Hard for 7 units
B $E2B3,1,1 Curve Left Hard for 22 units
B $E2B4,1,1
B $E2B5,1,1 Curve Straight for 34 units
B $E2B6,2,2
B $E2B8,1,1 Curve Right for 46 units
B $E2B9,3,3
B $E2BC,1,1 Curve Straight for 19 units
B $E2BD,1,1
B $E2BE,1,1 <Esc> Split
B $E2BF,1,1
W $E2C0,2,2 [$E442] Left target
W $E2C2,2,2 [$E4C4] Right target
b $E2C4 [Stage 4] Map height data
B $E2C4,1,1 Level Road for 40 units
B $E2C5,2,2
B $E2C7,1,1 Going Up 3 for 2 units
B $E2C8,1,1 Going Up 5 for 5 units
B $E2C9,1,1 Going Up 7 for 1 units
B $E2CA,1,1 Going Down 5 for 2 units
B $E2CB,1,1 Going Down 3 for 11 units
B $E2CC,1,1 Going Down 1 for 5 units
B $E2CD,1,1 Level Road for 27 units
B $E2CE,1,1
B $E2CF,1,1 Going Down 3 for 1 units
B $E2D0,1,1 Going Down 5 for 1 units
B $E2D1,1,1 Going Down 7 for 3 units
B $E2D2,1,1 Going Down 5 for 1 units
B $E2D3,1,1 Going Down 3 for 1 units
B $E2D4,1,1 Going Down 1 for 1 units
B $E2D5,1,1 Level Road for 5 units
B $E2D6,1,1 Going Up 1 for 1 units
B $E2D7,1,1 Going Up 3 for 1 units
B $E2D8,1,1 Going Up 5 for 1 units
B $E2D9,1,1 Going Up 7 for 2 units
B $E2DA,1,1 Going Up 5 for 2 units
B $E2DB,1,1 Going Up 3 for 2 units
B $E2DC,1,1 Going Up 1 for 1 units
B $E2DD,1,1 Level Road for 95 units
B $E2DE,6,6
B $E2E4,1,1 Going Up 1 for 1 units
B $E2E5,1,1 Going Up 3 for 1 units
B $E2E6,1,1 Going Up 5 for 1 units
B $E2E7,1,1 Going Up 7 for 8 units
B $E2E8,1,1 Going Up 5 for 2 units
B $E2E9,1,1 Going Up 3 for 1 units
B $E2EA,1,1 Going Up 1 for 1 units
B $E2EB,1,1 Level Road for 1 units
B $E2EC,1,1 Going Down 1 for 1 units
B $E2ED,1,1 Going Down 3 for 2 units
B $E2EE,1,1 Going Down 5 for 1 units
B $E2EF,1,1 Going Down 7 for 8 units
B $E2F0,1,1 Going Down 5 for 1 units
B $E2F1,1,1 Going Down 3 for 2 units
B $E2F2,1,1 Going Down 1 for 5 units
B $E2F3,1,1 Level Road for 153 units
B $E2F4,10,8,2
B $E2FE,1,1 <Esc> Split
B $E2FF,1,1
W $E300,2,2 [$E45A] Left target
W $E302,2,2 [$E4DA] Right target
b $E304 [Stage 4] Map lanes data
B $E304,1,1 4 Lanes              [||||] {00} for 400 units
B $E305,3,3
B $E308,1,1 <Esc> Split
B $E309,1,1
W $E30A,2,2 [$E46C] Left target
W $E30C,2,2 [$E4EC] Right target
b $E30E [Stage 4] Map hazards data
B $E30E,1,1 Wait for 31 units
B $E30F,1,1 Start Spawning HAZARD_1 Both Sides
B $E310,1,1
B $E311,1,1 Wait for 4 units
B $E312,1,1 Stop Spawning Hazards
B $E313,1,1
B $E314,1,1 Wait for 29 units
B $E315,1,1 Start Spawning HAZARD_1 Right
B $E316,1,1
B $E317,1,1 Wait for 2 units
B $E318,1,1 Stop Spawning Hazards
B $E319,1,1
B $E31A,1,1 Wait for 29 units
B $E31B,1,1 Start Spawning HAZARD_1 Left
B $E31C,1,1
B $E31D,1,1 Wait for 2 units
B $E31E,1,1 Stop Spawning Hazards
B $E31F,1,1
B $E320,1,1 Wait for 21 units
B $E321,1,1 Start helicopter (pilot says turn right)
B $E322,1,1
B $E323,1,1 Wait for 20 units
B $E324,1,1 Start Spawning HAZARD_1 Right
B $E325,1,1
B $E326,1,1 Wait for 2 units
B $E327,1,1 Stop Spawning Hazards
B $E328,1,1
B $E329,1,1 Wait for 53 units
B $E32A,1,1 Disable Car Spawning
B $E32B,1,1
B $E32C,1,1 Wait for 6 units
B $E32D,1,1 Set Floating Arrow to Right
B $E32E,1,1
B $E32F,1,1 Wait for 1 units
B $E330,1,1 <Esc> Split
B $E331,1,1
W $E332,2,2 [$E480] Left target
W $E334,2,2 [$E500] Right target
b $E336 [Stage 4] Map left object data
B $E336,1,1 Alternating (STREET_LAMP, EMPTY) for 20 units
B $E337,19,8*2,3
B $E34A,1,1 Alternating (STREET_LAMP, FAR_COLUMN) for 4 units
B $E34B,3,3
B $E34E,1,1 Alternating (STREET_LAMP, EMPTY) for 4 units
B $E34F,3,3
B $E352,1,1 STREET_LAMP for 1 units
B $E353,1,1 EMPTY for 2 units
B $E354,1,1 FAR_COLUMN for 1 units
B $E355,1,1 Alternating (STREET_LAMP, EMPTY) for 4 units
B $E356,4,4
B $E35A,1,1 STREET_LAMP for 3 units
B $E35B,1,1 EMPTY for 3 units
B $E35C,1,1 STREET_LAMP for 1 units
B $E35D,1,1 EMPTY for 3 units
B $E35E,1,1 STREET_LAMP for 1 units
B $E35F,1,1 Alternating (TURN_SIGN_POINTING_LEFT, EMPTY) for 8 units
B $E360,8,8
B $E368,1,1 TURN_SIGN_POINTING_LEFT for 12 units
B $E369,1,1 Alternating (STREET_LAMP, EMPTY) for 18 units
B $E36A,18,8*2,2
B $E37C,1,1 STREET_LAMP for 4 units
B $E37D,1,1 Alternating (TURN_SIGN_POINTING_RIGHT, EMPTY) for 14 units
B $E37E,14,8,6
B $E38C,1,1 TURN_SIGN_POINTING_RIGHT for 4 units
B $E38D,1,1 Alternating (PILE_OF_ROCKS, EMPTY) for 12 units
B $E38E,12,8,4
B $E39A,1,1 PILE_OF_ROCKS for 6 units
B $E39B,1,1 Alternating (TURN_SIGN_POINTING_LEFT, EMPTY) for 8 units
B $E39C,8,8
B $E3A4,1,1 TURN_SIGN_POINTING_LEFT for 6 units
B $E3A5,1,1 Alternating (STREET_LAMP, EMPTY) for 8 units
B $E3A6,8,8
B $E3AE,1,1 STREET_LAMP for 14 units
B $E3AF,1,1 EMPTY for 3 units
B $E3B0,1,1 STREET_LAMP for 1 units
B $E3B1,1,1 EMPTY for 1 units
B $E3B2,1,1 STREET_LAMP for 1 units
B $E3B3,1,1 EMPTY for 3 units
B $E3B4,1,1 STREET_LAMP for 1 units
B $E3B5,1,1 EMPTY for 1 units
B $E3B6,1,1 STREET_LAMP for 1 units
B $E3B7,1,1 EMPTY for 3 units
B $E3B8,1,1 STREET_LAMP for 1 units
B $E3B9,1,1 EMPTY for 1 units
B $E3BA,1,1 STREET_LAMP for 1 units
B $E3BB,1,1 Alternating (PILE_OF_ROCKS, EMPTY) for 10 units
B $E3BC,10,8,2
B $E3C6,1,1 PILE_OF_ROCKS for 4 units
B $E3C7,1,1 EMPTY for 7 units
B $E3C8,1,1 <Esc> Split
B $E3C9,1,1
W $E3CA,2,2 [$E48B] Left target
W $E3CC,2,2 [$E50B] Right target
b $E3CE [Stage 4] Map right object data
B $E3CE,1,1 Alternating (STREET_LAMP, EMPTY) for 6 units
B $E3CF,5,5
B $E3D4,1,1 STREET_LAMP for 1 units
B $E3D5,1,1 Alternating (STREET_LAMP, EMPTY) for 6 units
B $E3D6,6,6
B $E3DC,1,1 Alternating (STREET_LAMP, FAR_COLUMN) for 4 units
B $E3DD,3,3
B $E3E0,1,1 Alternating (STREET_LAMP, EMPTY) for 14 units
B $E3E1,13,8,5
B $E3EE,1,1 STREET_LAMP for 2 units
B $E3EF,1,1 EMPTY for 3 units
B $E3F0,1,1 STREET_LAMP for 1 units
B $E3F1,1,1 EMPTY for 3 units
B $E3F2,1,1 STREET_LAMP for 1 units
B $E3F3,1,1 EMPTY for 5 units
B $E3F4,1,1 STREET_LAMP for 1 units
B $E3F5,1,1 EMPTY for 3 units
B $E3F6,1,1 STREET_LAMP for 1 units
B $E3F7,1,1 EMPTY for 3 units
B $E3F8,1,1 STREET_LAMP for 1 units
B $E3F9,1,1 Alternating (TURN_SIGN_POINTING_LEFT, EMPTY) for 16 units
B $E3FA,16,8
B $E40A,1,1 TURN_SIGN_POINTING_LEFT for 12 units
B $E40B,1,1 Alternating (PILE_OF_ROCKS, EMPTY) for 4 units
B $E40C,7,7
B $E413,1,1 PILE_OF_ROCKS for 50 units
B $E414,1,1 Alternating (TURN_SIGN_POINTING_LEFT, EMPTY) for 10 units
B $E415,10,8,2
B $E41F,1,1 TURN_SIGN_POINTING_LEFT for 4 units
B $E420,1,1 Alternating (PILE_OF_ROCKS, EMPTY) for 4 units
B $E421,4,4
B $E425,1,1 PILE_OF_ROCKS for 4 units
B $E426,1,1 Alternating (PILE_OF_ROCKS, EMPTY) for 10 units
B $E427,10,8,2
B $E431,1,1 PILE_OF_ROCKS for 4 units
B $E432,1,1 Alternating (PILE_OF_ROCKS, EMPTY) for 6 units
B $E433,7,7
B $E43A,1,1 PILE_OF_ROCKS for 18 units
B $E43B,1,1 EMPTY for 3 units
B $E43C,1,1 <Esc> Split
B $E43D,1,1
W $E43E,2,2 [$E4B4] Left target
W $E440,2,2 [$E54D] Right target
b $E442 [Stage 4] Map curvature data
B $E442,1,1 Curve Straight for 35 units
B $E443,2,2
B $E445,1,1 Curve Left Hard for 12 units
B $E446,1,1 Curve Right for 1 units
B $E447,1,1 Curve Right Hard for 10 units
B $E448,1,1 Curve Right for 6 units
B $E449,1,1 Curve Straight for 8 units
B $E44A,1,1 Curve Left for 16 units
B $E44B,1,1
B $E44C,1,1 Curve Right for 25 units
B $E44D,1,1
B $E44E,1,1 Curve Right Hard for 15 units
B $E44F,1,1 Curve Right for 26 units
B $E450,1,1
B $E451,1,1 Curve Straight for 14 units
B $E452,1,1 Curve Right Hard for 10 units
B $E453,1,1 Curve Right Very Hard for 8 units
B $E454,1,1 Curve Right Hard for 7 units
B $E455,1,1 Curve Straight for 11 units
B $E456,1,1 <Esc> Jump
B $E457,1,1
W $E458,2,2 [$E577] Target
b $E45A [Stage 4] Map height data
B $E45A,1,1 Level Road for 204 units
B $E45B,13,8,5
B $E468,1,1 <Esc> Jump
B $E469,1,1
W $E46A,2,2 [$E59A] Target
b $E46C [Stage 4] Map lanes data
B $E46C,1,1 4 Lanes              [||||] {00} for 2 units
B $E46D,1,1
B $E46E,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E46F,1,1
B $E470,1,1 3 Lanes L            [|||]  {81} for 8 units
B $E471,1,1
B $E472,1,1 Tunnel start                {45} for 102 units
B $E473,1,1
B $E474,1,1 Tunnel cont/end?            {59} for 2 units
B $E475,1,1
B $E476,1,1 3 Lanes L            [|||]  {81} for 18 units
B $E477,1,1
B $E478,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E479,1,1
B $E47A,1,1 4 Lanes              [||||] {00} for 68 units
B $E47B,1,1
B $E47C,1,1 <Esc> Jump
B $E47D,1,1
W $E47E,2,2 [$E5D3] Target
b $E480 [Stage 4] Map hazards data
B $E480,1,1 Wait for 10 units
B $E481,1,1 Stop helicopter
B $E482,1,1
B $E483,1,1 Wait for 2 units
B $E484,1,1 Enable Car Spawning
B $E485,1,1
B $E486,1,1 Wait for 90 units
B $E487,1,1 <Esc> Jump
B $E488,1,1
W $E489,2,2 [$E5FB] Target
b $E48B [Stage 4] Map left object data
B $E48B,1,1 EMPTY for 2 units
B $E48C,1,1 STREET_LAMP for 1 units
B $E48D,1,1 EMPTY for 1 units
B $E48E,1,1 STREET_LAMP for 1 units
B $E48F,1,1 EMPTY for 2 units
B $E490,1,1 TUNNEL_LIGHT for 51 units
B $E491,3,3
B $E494,1,1 Alternating (EMPTY, PILE_OF_ROCKS) for 22 units
B $E495,22,8*2,6
B $E4AB,1,1 Alternating (PILE_OF_ROCKS, EMPTY) for 4 units
B $E4AC,4,4
B $E4B0,1,1 <Esc> Jump
B $E4B1,1,1
W $E4B2,2,2 [$E624] Target
b $E4B4 [Stage 4] Map right object data
B $E4B4,1,1 EMPTY for 2 units
B $E4B5,1,1 STREET_LAMP for 1 units
B $E4B6,1,1 EMPTY for 1 units
B $E4B7,1,1 STREET_LAMP for 1 units
B $E4B8,1,1 EMPTY for 2 units
B $E4B9,1,1 TUNNEL_LIGHT for 51 units
B $E4BA,3,3
B $E4BD,1,1 EMPTY for 44 units
B $E4BE,2,2
B $E4C0,1,1 <Esc> Jump
B $E4C1,1,1
W $E4C2,2,2 [$E6B7] Target
b $E4C4 [Stage 4] Map curvature data
B $E4C4,1,1 Curve Straight for 12 units
B $E4C5,1,1 Curve Left for 8 units
B $E4C6,1,1 Curve Left Hard for 16 units
B $E4C7,1,1
B $E4C8,1,1 Curve Left for 18 units
B $E4C9,1,1
B $E4CA,1,1 Curve Straight for 30 units
B $E4CB,1,1
B $E4CC,1,1 Curve Right Hard for 12 units
B $E4CD,1,1 Curve Right Very Hard for 13 units
B $E4CE,1,1 Curve Right Hard for 12 units
B $E4CF,1,1 Curve Right for 33 units
B $E4D0,2,2
B $E4D2,1,1 Curve Straight for 48 units
B $E4D3,3,3
B $E4D6,1,1 <Esc> Jump
B $E4D7,1,1
W $E4D8,2,2 [$E577] Target
b $E4DA [Stage 4] Map height data
B $E4DA,1,1 Level Road for 202 units
B $E4DB,13,8,5
B $E4E8,1,1 <Esc> Jump
B $E4E9,1,1
W $E4EA,2,2 [$E59A] Target
b $E4EC [Stage 4] Map lanes data
B $E4EC,1,1 4 Lanes              [||||] {00} for 88 units
B $E4ED,1,1
B $E4EE,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E4EF,1,1
B $E4F0,1,1 3 Lanes L            [|||]  {81} for 20 units
B $E4F1,1,1
B $E4F2,1,1 Tunnel start                {45} for 52 units
B $E4F3,1,1
B $E4F4,1,1 Tunnel cont/end?            {59} for 2 units
B $E4F5,1,1
B $E4F6,1,1 3 Lanes L            [|||]  {81} for 10 units
B $E4F7,1,1
B $E4F8,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E4F9,1,1
B $E4FA,1,1 4 Lanes              [||||] {00} for 26 units
B $E4FB,1,1
B $E4FC,1,1 <Esc> Jump
B $E4FD,1,1
W $E4FE,2,2 [$E5D3] Target
b $E500 [Stage 4] Map hazards data
B $E500,1,1 Wait for 10 units
B $E501,1,1 Enable Car Spawning
B $E502,1,1
B $E503,1,1 Wait for 36 units
B $E504,1,1 Stop helicopter
B $E505,1,1
B $E506,1,1 Wait for 55 units
B $E507,1,1 <Esc> Jump
B $E508,1,1
W $E509,2,2 [$E5FB] Target
b $E50B [Stage 4] Map left object data
B $E50B,1,1 Alternating (PILE_OF_ROCKS, EMPTY) for 20 units
B $E50C,19,8*2,3
B $E51F,1,1 PILE_OF_ROCKS for 1 units
B $E520,1,1 EMPTY for 3 units
B $E521,1,1 PILE_OF_ROCKS for 1 units
B $E522,1,1 EMPTY for 3 units
B $E523,1,1 PILE_OF_ROCKS for 1 units
B $E524,1,1 EMPTY for 3 units
B $E525,1,1 PILE_OF_ROCKS for 1 units
B $E526,1,1 EMPTY for 1 units
B $E527,1,1 PILE_OF_ROCKS for 1 units
B $E528,1,1 Alternating (TURN_SIGN_POINTING_RIGHT, EMPTY) for 10 units
B $E529,10,8,2
B $E533,1,1 STREET_LAMP for 8 units
B $E534,1,1 EMPTY for 3 units
B $E535,1,1 TUNNEL_LIGHT for 26 units
B $E536,1,1
B $E537,1,1 Alternating (STREET_LAMP, EMPTY) for 16 units
B $E538,16,8
B $E548,1,1 STREET_LAMP for 3 units
B $E549,1,1 <Esc> Jump
B $E54A,1,1
W $E54B,2,2 [$E624] Target
b $E54D [Stage 4] Map right object data
B $E54D,1,1 Alternating (TURN_SIGN_POINTING_LEFT, EMPTY) for 10 units
B $E54E,10,8,2
B $E558,1,1 TURN_SIGN_POINTING_LEFT for 13 units
B $E559,1,1 EMPTY for 13 units
B $E55A,1,1 STREET_LAMP for 1 units
B $E55B,1,1 EMPTY for 3 units
B $E55C,1,1 STREET_LAMP for 1 units
B $E55D,1,1 EMPTY for 3 units
B $E55E,1,1 TURN_SIGN_POINTING_LEFT for 1 units
B $E55F,1,1 Alternating (STREET_LAMP, EMPTY) for 16 units
B $E560,18,8*2,2
B $E572,1,1 STREET_LAMP for 40 units
B $E573,1,1 <Esc> Jump
B $E574,1,1
W $E575,2,2 [$E6B7] Target
b $E577 [Stage 4] Map curvature data
B $E577,1,1 Curve Straight for 67 units
B $E578,4,4
B $E57C,1,1 Curve Left for 14 units
B $E57D,1,1 Curve Left Hard for 20 units
B $E57E,1,1
B $E57F,1,1 Curve Right Very Hard for 10 units
B $E580,1,1 Curve Left Hard for 20 units
B $E581,1,1
B $E582,1,1 Curve Left for 30 units
B $E583,1,1
B $E584,1,1 Curve Straight for 69 units
B $E585,4,4
B $E589,1,1 Curve Right Hard for 31 units
B $E58A,2,2
B $E58C,1,1 Curve Right Very Hard for 10 units
B $E58D,1,1 Curve Right Hard for 30 units
B $E58E,1,1
B $E58F,1,1 Curve Straight for 101 units
B $E590,6,6
B $E596,1,1 <Esc> Loop
B $E597,1,1
W $E598,2,2 [$E577] Target
b $E59A [Stage 4] Map height data
B $E59A,1,1 Level Road for 14 units
B $E59B,1,1 Going Up 3 for 2 units
B $E59C,1,1 Going Up 5 for 2 units
B $E59D,1,1 Going Up 7 for 4 units
B $E59E,1,1 Going Up 5 for 1 units
B $E59F,1,1 Going Up 3 for 1 units
B $E5A0,1,1 Going Down 3 for 1 units
B $E5A1,1,1 Going Down 5 for 2 units
B $E5A2,1,1 Going Down 7 for 10 units
B $E5A3,1,1 Going Down 5 for 2 units
B $E5A4,1,1 Going Down 3 for 1 units
B $E5A5,1,1 Going Up 3 for 1 units
B $E5A6,1,1 Going Up 5 for 2 units
B $E5A7,1,1 Going Up 7 for 7 units
B $E5A8,1,1 Going Up 5 for 1 units
B $E5A9,1,1 Going Down 7 for 2 units
B $E5AA,1,1 Going Down 5 for 1 units
B $E5AB,1,1 Going Down 3 for 1 units
B $E5AC,1,1 Going Down 1 for 4 units
B $E5AD,1,1 Level Road for 119 units
B $E5AE,7,7
B $E5B5,1,1 Going Down 3 for 1 units
B $E5B6,1,1 Going Down 5 for 2 units
B $E5B7,1,1 Going Down 7 for 3 units
B $E5B8,1,1 Going Down 5 for 1 units
B $E5B9,1,1 Going Down 3 for 1 units
B $E5BA,1,1 Going Up 3 for 1 units
B $E5BB,1,1 Going Up 5 for 1 units
B $E5BC,1,1 Going Up 7 for 7 units
B $E5BD,1,1 Going Down 7 for 1 units
B $E5BE,1,1 Going Down 5 for 1 units
B $E5BF,1,1 Going Down 3 for 2 units
B $E5C0,1,1 Going Down 1 for 2 units
B $E5C1,1,1 Level Road for 201 units
B $E5C2,13,8,5
B $E5CF,1,1 <Esc> Loop
B $E5D0,1,1
W $E5D1,2,2 [$E59A] Target
b $E5D3 [Stage 4] Map lanes data
B $E5D3,1,1 4 Lanes              [||||] {00} for 306 units
B $E5D4,3,3
B $E5D7,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E5D8,1,1
B $E5D9,1,1 3 Lanes L            [|||]  {81} for 6 units
B $E5DA,1,1
B $E5DB,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E5DC,1,1
B $E5DD,1,1 4 Lanes              [||||] {00} for 8 units
B $E5DE,1,1
B $E5DF,1,1 4-3 Narrowing R      [/|||] {8E} for 8 units
B $E5E0,1,1
B $E5E1,1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $E5E2,1,1
B $E5E3,1,1 4 Lanes              [||||] {00} for 8 units
B $E5E4,1,1
B $E5E5,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $E5E6,1,1
B $E5E7,1,1 3 Lanes L            [|||]  {81} for 6 units
B $E5E8,1,1
B $E5E9,1,1 3-4 Widening L       [|||/] {AD} for 2 units
B $E5EA,1,1
B $E5EB,1,1 4 Lanes              [||||] {00} for 16 units
B $E5EC,1,1
B $E5ED,1,1 4-3 Narrowing R      [/|||] {8E} for 2 units
B $E5EE,1,1
B $E5EF,1,1 3-2 Narrowing R       [/||] {0F} for 14 units
B $E5F0,1,1
B $E5F1,1,1 2-3 Widening R        [\||] {1F} for 2 units
B $E5F2,1,1
B $E5F3,1,1 3-4 Widening R       [\|||] {9E} for 2 units
B $E5F4,1,1
B $E5F5,1,1 4 Lanes              [||||] {00} for 14 units
B $E5F6,1,1
B $E5F7,1,1 <Esc> Loop
B $E5F8,1,1
W $E5F9,2,2 [$E5D3] Target
b $E5FB [Stage 4] Map hazards data
B $E5FB,1,1 Wait for 32 units
B $E5FC,1,1 Start Spawning HAZARD_1 Left
B $E5FD,1,1
B $E5FE,1,1 Wait for 2 units
B $E5FF,1,1 Stop Spawning Hazards
B $E600,1,1
B $E601,1,1 Wait for 26 units
B $E602,1,1 Start Spawning HAZARD_1 Right
B $E603,1,1
B $E604,1,1 Wait for 2 units
B $E605,1,1 Stop Spawning Hazards
B $E606,1,1
B $E607,1,1 Wait for 61 units
B $E608,1,1 Start Spawning HAZARD_1 Left
B $E609,1,1
B $E60A,1,1 Wait for 2 units
B $E60B,1,1 Stop Spawning Hazards
B $E60C,1,1
B $E60D,1,1 Wait for 9 units
B $E60E,1,1 Start Spawning HAZARD_1 Left
B $E60F,1,1
B $E610,1,1 Wait for 2 units
B $E611,1,1 Stop Spawning Hazards
B $E612,1,1
B $E613,1,1 Wait for 23 units
B $E614,1,1 Start Spawning HAZARD_2 Both Sides
B $E615,1,1
B $E616,1,1 Wait for 2 units
B $E617,1,1 Stop Spawning Hazards
B $E618,1,1
B $E619,1,1 Wait for 26 units
B $E61A,1,1 Start Spawning HAZARD_2 Right
B $E61B,1,1
B $E61C,1,1 Wait for 2 units
B $E61D,1,1 Stop Spawning Hazards
B $E61E,1,1
B $E61F,1,1 Wait for 12 units
B $E620,1,1 <Esc> Loop
B $E621,1,1
W $E622,2,2 [$E5FB] Target
b $E624 [Stage 4] Map left object data
B $E624,1,1 Alternating (STREET_LAMP, EMPTY) for 12 units
B $E625,11,8,3
B $E630,1,1 STREET_LAMP for 1 units
B $E631,1,1 Alternating (FAR_COLUMN, EMPTY) for 4 units
B $E632,4,4
B $E636,1,1 FAR_COLUMN for 6 units
B $E637,1,1 Alternating (NEAR_COLUMN, EMPTY) for 12 units
B $E638,12,8,4
B $E644,1,1 NEAR_COLUMN for 6 units
B $E645,1,1 Alternating (NEAR_COLUMN, FAR_COLUMN) for 4 units
B $E646,4,4
B $E64A,1,1 Alternating (TURN_SIGN_POINTING_RIGHT, EMPTY) for 4 units
B $E64B,4,4
B $E64F,1,1 TURN_SIGN_POINTING_RIGHT for 8 units
B $E650,1,1 Alternating (NEAR_COLUMN, EMPTY) for 18 units
B $E651,18,8*2,2
B $E663,1,1 NEAR_COLUMN for 16 units
B $E664,1,1 Alternating (FAR_COLUMN, NEAR_COLUMN) for 4 units
B $E665,4,4
B $E669,1,1 EMPTY for 9 units
B $E66A,1,1 NEAR_COLUMN for 1 units
B $E66B,1,1 Alternating (NEAR_COLUMN, EMPTY) for 42 units
B $E66C,42,8*5,2
B $E696,1,1 NEAR_COLUMN for 4 units
B $E697,1,1 Alternating (FAR_COLUMN, EMPTY) for 4 units
B $E698,4,4
B $E69C,1,1 FAR_COLUMN for 6 units
B $E69D,1,1 EMPTY for 3 units
B $E69E,1,1 NEAR_COLUMN for 1 units
B $E69F,1,1 EMPTY for 1 units
B $E6A0,1,1 FAR_COLUMN for 1 units
B $E6A1,1,1 Alternating (PILE_OF_ROCKS, EMPTY) for 6 units
B $E6A2,6,6
B $E6A8,1,1 PILE_OF_ROCKS for 4 units
B $E6A9,1,1 EMPTY for 6 units
B $E6AA,1,1 TURN_SIGN_POINTING_RIGHT for 4 units
B $E6AB,1,1 EMPTY for 3 units
B $E6AC,1,1 NEAR_COLUMN for 1 units
B $E6AD,1,1 Alternating (NEAR_COLUMN, EMPTY) for 4 units
B $E6AE,4,4
B $E6B2,1,1 NEAR_COLUMN for 6 units
B $E6B3,1,1 <Esc> Loop
B $E6B4,1,1
W $E6B5,2,2 [$E624] Target
b $E6B7 [Stage 4] Map right object data
B $E6B7,1,1 Alternating (FAR_COLUMN, EMPTY) for 4 units
B $E6B8,5,5
B $E6BD,1,1 FAR_COLUMN for 19 units
B $E6BE,1,1 Alternating (FAR_COLUMN, NEAR_COLUMN) for 4 units
B $E6BF,4,4
B $E6C3,1,1 Alternating (EMPTY, NEAR_COLUMN) for 4 units
B $E6C4,3,3
B $E6C7,1,1 Alternating (TURN_SIGN_POINTING_LEFT, EMPTY) for 6 units
B $E6C8,6,6
B $E6CE,1,1 TURN_SIGN_POINTING_LEFT for 8 units
B $E6CF,1,1 Alternating (NEAR_COLUMN, EMPTY) for 38 units
B $E6D0,38,8*4,6
B $E6F6,1,1 NEAR_COLUMN for 4 units
B $E6F7,1,1 Alternating (PILE_OF_ROCKS, EMPTY) for 4 units
B $E6F8,4,4
B $E6FC,1,1 PILE_OF_ROCKS for 8 units
B $E6FD,1,1 EMPTY for 3 units
B $E6FE,1,1 PILE_OF_ROCKS for 1 units
B $E6FF,1,1 EMPTY for 1 units
B $E700,1,1 PILE_OF_ROCKS for 1 units
B $E701,1,1 EMPTY for 10 units
B $E702,1,1 Alternating (NEAR_COLUMN, EMPTY) for 34 units
B $E703,34,8*4,2
B $E725,1,1 NEAR_COLUMN for 2 units
B $E726,1,1 EMPTY for 3 units
B $E727,1,1 NEAR_COLUMN for 1 units
B $E728,1,1 FAR_COLUMN for 1 units
B $E729,1,1 NEAR_COLUMN for 1 units
B $E72A,1,1 EMPTY for 14 units
B $E72B,1,1 TURN_SIGN_POINTING_LEFT for 2 units
B $E72C,1,1 Alternating (PILE_OF_ROCKS, EMPTY) for 6 units
B $E72D,6,6
B $E733,1,1 PILE_OF_ROCKS for 8 units
B $E734,1,1 Alternating (NEAR_COLUMN, EMPTY) for 4 units
B $E735,4,4
B $E739,1,1 NEAR_COLUMN for 10 units
B $E73A,1,1 <Esc> Loop
B $E73B,1,1
W $E73C,2,2 [$E6B7] Target
b $E73E [Stage 4] Perp's mugshot
B $E73E,160,4 Bitmap data for the perp's mugshot (32x40). Stored top-down.
B $E7DE,20,4 Attribute data for the perp's mugshot (4x5). Stored top-down.
N $E7F2 LOD table for "Car A (the perp's car)"
N $E7F2 LOD
B $E7F2,1,1 Width (bytes)
B $E7F3,1,1 Flags
B $E7F4,1,1 Height (pixels)
W $E7F5,2,2 [$E870] Bitmap address
W $E7F7,2,2 [$E870] Pre-shifted bitmap address
N $E7F9 LOD
B $E7F9,1,1 Width (bytes)
B $E7FA,1,1 Flags
B $E7FB,1,1 Height (pixels)
W $E7FC,2,2 [$E924] Bitmap address
W $E7FE,2,2 [$E924] Pre-shifted bitmap address
N $E800 LOD
B $E800,1,1 Width (bytes)
B $E801,1,1 Flags
B $E802,1,1 Height (pixels)
W $E803,2,2 [$E97C] Bitmap address
W $E805,2,2 [$E97C] Pre-shifted bitmap address
N $E807 LOD
B $E807,1,1 Width (bytes)
B $E808,1,1 Flags
B $E809,1,1 Height (pixels)
W $E80A,2,2 [$E97C] Bitmap address
W $E80C,2,2 [$E97C] Pre-shifted bitmap address
N $E80E LOD
B $E80E,1,1 Width (bytes)
B $E80F,1,1 Flags
B $E810,1,1 Height (pixels)
W $E811,2,2 [$E994] Bitmap address
W $E813,2,2 [$E994] Pre-shifted bitmap address
N $E815 LOD
B $E815,1,1 Width (bytes)
B $E816,1,1 Flags
B $E817,1,1 Height (pixels)
W $E818,2,2 [$E994] Bitmap address
W $E81A,2,2 [$E994] Pre-shifted bitmap address
N $E81C LOD table for "Car C (a truck in S1)"
N $E81C LOD
B $E81C,1,1 Width (bytes)
B $E81D,1,1 Flags
B $E81E,1,1 Height (pixels)
W $E81F,2,2 [$E9D4] Bitmap address
W $E821,2,2 [$E9D4] Pre-shifted bitmap address
N $E823 LOD
B $E823,1,1 Width (bytes)
B $E824,1,1 Flags
B $E825,1,1 Height (pixels)
W $E826,2,2 [$EA88] Bitmap address
W $E828,2,2 [$EA88] Pre-shifted bitmap address
N $E82A LOD
B $E82A,1,1 Width (bytes)
B $E82B,1,1 Flags
B $E82C,1,1 Height (pixels)
W $E82D,2,2 [$EAE0] Bitmap address
W $E82F,2,2 [$EAE0] Pre-shifted bitmap address
N $E831 LOD
B $E831,1,1 Width (bytes)
B $E832,1,1 Flags
B $E833,1,1 Height (pixels)
W $E834,2,2 [$EAE0] Bitmap address
W $E836,2,2 [$EAE0] Pre-shifted bitmap address
N $E838 LOD
B $E838,1,1 Width (bytes)
B $E839,1,1 Flags
B $E83A,1,1 Height (pixels)
W $E83B,2,2 [$E994] Bitmap address
W $E83D,2,2 [$E9B4] Pre-shifted bitmap address
N $E83F LOD
B $E83F,1,1 Width (bytes)
B $E840,1,1 Flags
B $E841,1,1 Height (pixels)
W $E842,2,2 [$E994] Bitmap address
W $E844,2,2 [$E9B4] Pre-shifted bitmap address
N $E846 LOD table for "Car B (a Lambo in S1)"
N $E846 LOD
B $E846,1,1 Width (bytes)
B $E847,1,1 Flags
B $E848,1,1 Height (pixels)
W $E849,2,2 [$EB0D] Bitmap address
W $E84B,2,2 [$EB0D] Pre-shifted bitmap address
N $E84D LOD
B $E84D,1,1 Width (bytes)
B $E84E,1,1 Flags
B $E84F,1,1 Height (pixels)
W $E850,2,2 [$EBBB] Bitmap address
W $E852,2,2 [$EBBB] Pre-shifted bitmap address
N $E854 LOD
B $E854,1,1 Width (bytes)
B $E855,1,1 Flags
B $E856,1,1 Height (pixels)
W $E857,2,2 [$EC07] Bitmap address
W $E859,2,2 [$EC07] Pre-shifted bitmap address
N $E85B LOD
B $E85B,1,1 Width (bytes)
B $E85C,1,1 Flags
B $E85D,1,1 Height (pixels)
W $E85E,2,2 [$EC07] Bitmap address
W $E860,2,2 [$EC07] Pre-shifted bitmap address
N $E862 LOD
B $E862,1,1 Width (bytes)
B $E863,1,1 Flags
B $E864,1,1 Height (pixels)
W $E865,2,2 [$EC2E] Bitmap address
W $E867,2,2 [$EC2E] Pre-shifted bitmap address
N $E869 LOD
B $E869,1,1 Width (bytes)
B $E86A,1,1 Flags
B $E86B,1,1 Height (pixels)
W $E86C,2,2 [$EC2E] Bitmap address
W $E86E,2,2 [$EC2E] Pre-shifted bitmap address
B $E870,180,6 Bitmap data 6 bytes x 30
B $E924,88,4 Bitmap data 4 bytes x 22
B $E97C,24,2 Bitmap data 2 bytes x 12
B $E994,32,4 Bitmap data (masked) 4 bytes x 8
B $E9B4,32,4 Pre-shifted bitmap data (masked) 4 bytes x 8
B $E9D4,180,6 Bitmap data 6 bytes x 30
B $EA88,88,4 Bitmap data 4 bytes x 22
B $EAE0,45,3 Bitmap data 3 bytes x 15
B $EB0D,174,6 Bitmap data 6 bytes x 29
B $EBBB,76,4 Bitmap data 4 bytes x 19
B $EC07,39,3 Bitmap data 3 bytes x 13
B $EC2E,32,4 Bitmap data (masked) 4 bytes x 8
B $EC4E,32,8
b $EC6E [Stage 4] Helicopter data 1
W $EC6E,2,2 [$ECAA] ptr
W $EC70,2,2 [$EC8F] ptr
W $EC72,2,2 [$EC86] ptr
W $EC74,2,2 [$ECB3] ptr
W $EC76,2,2 [$ECBC] ptr
W $EC78,2,2 [$ECD7] ptr
b $EC7A [Stage 4] Helicopter data 2
W $EC7A,2,2 [$ECAA] ptr
W $EC7C,2,2 [$ECA1] ptr
W $EC7E,2,2 [$EC98] ptr
W $EC80,2,2 [$ECC5] ptr
W $EC82,2,2 [$ECCE] ptr
W $EC84,2,2 [$ECDF] ptr
B $EC86,1,1 Pointed to by helicopter data 1
B $EC87,1,1
N $EC88 LOD
B $EC88,1,1 Width (bytes)
B $EC89,1,1 Flags
B $EC8A,1,1 Height (pixels)
W $EC8B,2,2 [$ECE7] Bitmap address
W $EC8D,2,2 [$ECE7] Pre-shifted bitmap address
B $EC8F,1,1 Pointed to by helicopter data 1
B $EC90,1,1
N $EC91 LOD
B $EC91,1,1 Width (bytes)
B $EC92,1,1 Flags
B $EC93,1,1 Height (pixels)
W $EC94,2,2 [$ED57] Bitmap address
W $EC96,2,2 [$ED57] Pre-shifted bitmap address
B $EC98,1,1 Pointed to by helicopter data 2
B $EC99,1,1
N $EC9A LOD
B $EC9A,1,1 Width (bytes)
B $EC9B,1,1 Flags
B $EC9C,1,1 Height (pixels)
W $EC9D,2,2 [$ED6C] Bitmap address
W $EC9F,2,2 [$ED6C] Pre-shifted bitmap address
B $ECA1,1,1 Pointed to by helicopter data 2
B $ECA2,1,1
N $ECA3 LOD
B $ECA3,1,1 Width (bytes)
B $ECA4,1,1 Flags
B $ECA5,1,1 Height (pixels)
W $ECA6,2,2 [$EDDC] Bitmap address
W $ECA8,2,2 [$EDDC] Pre-shifted bitmap address
B $ECAA,1,1 Pointed to by helicopter data 2
B $ECAB,1,1
N $ECAC LOD
B $ECAC,1,1 Width (bytes)
B $ECAD,1,1 Flags
B $ECAE,1,1 Height (pixels)
W $ECAF,2,2 [$EDF1] Bitmap address
W $ECB1,2,2 [$EDF1] Pre-shifted bitmap address
B $ECB3,1,1 Pointed to by helicopter data 1
B $ECB4,1,1
N $ECB5 LOD
B $ECB5,1,1 Width (bytes)
B $ECB6,1,1 Flags
B $ECB7,1,1 Height (pixels)
W $ECB8,2,2 [$EE2D] Bitmap address
W $ECBA,2,2 [$EE2D] Pre-shifted bitmap address
B $ECBC,1,1 Pointed to by helicopter data 1
B $ECBD,1,1
N $ECBE LOD
B $ECBE,1,1 Width (bytes)
B $ECBF,1,1 Flags
B $ECC0,1,1 Height (pixels)
W $ECC1,2,2 [$EE39] Bitmap address
W $ECC3,2,2 [$EE39] Pre-shifted bitmap address
B $ECC5,1,1 Pointed to by helicopter data 2
B $ECC6,1,1
N $ECC7 LOD
B $ECC7,1,1 Width (bytes)
B $ECC8,1,1 Flags
B $ECC9,1,1 Height (pixels)
W $ECCA,2,2 [$EE47] Bitmap address
W $ECCC,2,2 [$EE47] Pre-shifted bitmap address
B $ECCE,1,1 Pointed to by helicopter data 2
B $ECCF,1,1
N $ECD0 LOD
B $ECD0,1,1 Width (bytes)
B $ECD1,1,1 Flags
B $ECD2,1,1 Height (pixels)
W $ECD3,2,2 [$EE51] Bitmap address
W $ECD5,2,2 [$EE51] Pre-shifted bitmap address
B $ECD7,1,1 Pointed to by helicopter data 1
N $ECD8 LOD
B $ECD8,1,1 Width (bytes)
B $ECD9,1,1 Flags
B $ECDA,1,1 Height (pixels)
W $ECDB,2,2 [$EE6D] Bitmap address
W $ECDD,2,2 [$EE6D] Pre-shifted bitmap address
B $ECDF,1,1 Pointed to by helicopter data 2
N $ECE0 LOD
B $ECE0,1,1 Width (bytes)
B $ECE1,1,1 Flags
B $ECE2,1,1 Height (pixels)
W $ECE3,2,2 [$EED1] Bitmap address
W $ECE5,2,2 [$EED1] Pre-shifted bitmap address
B $ECE7,112,7 Bitmap data 7 bytes x 16
B $ED57,21,3 Bitmap data 3 bytes x 7
B $ED6C,112,7 Bitmap data 7 bytes x 16
B $EDDC,21,3 Bitmap data 3 bytes x 7
B $EDF1,60,6 Bitmap data (masked) 6 bytes x 10
B $EE2D,12,4 Bitmap data (masked) 4 bytes x 3
B $EE39,14,2 Bitmap data (masked) 2 bytes x 7
B $EE47,10,2 Bitmap data (masked) 2 bytes x 5
B $EE51,28,4 Bitmap data (masked) 4 bytes x 7
B $EE6D,100,10 Bitmap data (masked) 10 bytes x 10
B $EED1,100,10 Bitmap data (masked) 10 bytes x 10
b $EF35 [Stage 4] Pilot's mugshot
B $EF35,160,4 Bitmap data for the pilot's mugshot (32x40). Stored top-down.
B $EFD5,20,4 Attribute data for the pilot's mugshot (4x5). Stored top-down.
N $EFE9 Stretchy graphic
B $EFE9,1,1 ?index
W $EFEA,2,2 [$EFF3] Pointer to stretchy_graphic_part
B $EFEC,1,1 ?index
W $EFED,2,2 [$F009] Pointer to stretchy_graphic_part
B $EFEF,1,1 ?index
W $EFF0,2,2 [$F01F] Pointer to stretchy_graphic_part
B $EFF2,1,1 Terminator
N $EFF3 Stretchy graphic part
W $EFF3,2,2 [$F081] LOD ptr
W $EFF5,2,2 TBD
W $EFF7,2,2 TBD
W $EFF9,2,2 TBD
W $EFFB,2,2 TBD
W $EFFD,2,2 TBD
W $EFFF,2,2 TBD
W $F001,2,2 TBD
W $F003,2,2 TBD
W $F005,2,2 TBD
W $F007,2,2 TBD
N $F009 Stretchy graphic part
W $F009,2,2 [$F081] LOD ptr
W $F00B,2,2 TBD
W $F00D,2,2 TBD
W $F00F,2,2 TBD
W $F011,2,2 TBD
W $F013,2,2 TBD
W $F015,2,2 TBD
W $F017,2,2 TBD
W $F019,2,2 TBD
W $F01B,2,2 TBD
W $F01D,2,2 TBD
N $F01F Stretchy graphic part
W $F01F,2,2 [$F081] LOD ptr
W $F021,2,2 TBD
W $F023,2,2 TBD
W $F025,2,2 TBD
W $F027,2,2 TBD
W $F029,2,2 TBD
W $F02B,2,2 TBD
W $F02D,2,2 TBD
W $F02F,2,2 TBD
W $F031,2,2 TBD
W $F033,2,2 TBD
N $F035 Stretchy graphic
B $F035,1,1 ?index
W $F036,2,2 [$F03F] Pointer to stretchy_graphic_part
B $F038,1,1 ?index
W $F039,2,2 [$F055] Pointer to stretchy_graphic_part
B $F03B,1,1 ?index
W $F03C,2,2 [$F06B] Pointer to stretchy_graphic_part
B $F03E,1,1 Terminator
N $F03F Stretchy graphic part
W $F03F,2,2 [$F081] LOD ptr
W $F041,2,2 TBD
W $F043,2,2 TBD
W $F045,2,2 TBD
W $F047,2,2 TBD
W $F049,2,2 TBD
W $F04B,2,2 TBD
W $F04D,2,2 TBD
W $F04F,2,2 TBD
W $F051,2,2 TBD
W $F053,2,2 TBD
N $F055 Stretchy graphic part
W $F055,2,2 [$F081] LOD ptr
W $F057,2,2 TBD
W $F059,2,2 TBD
W $F05B,2,2 TBD
W $F05D,2,2 TBD
W $F05F,2,2 TBD
W $F061,2,2 TBD
W $F063,2,2 TBD
W $F065,2,2 TBD
W $F067,2,2 TBD
W $F069,2,2 TBD
N $F06B Stretchy graphic part
W $F06B,2,2 [$F081] LOD ptr
W $F06D,2,2 TBD
W $F06F,2,2 TBD
W $F071,2,2 TBD
W $F073,2,2 TBD
W $F075,2,2 TBD
W $F077,2,2 TBD
W $F079,2,2 TBD
W $F07B,2,2 TBD
W $F07D,2,2 TBD
W $F07F,2,2 TBD
N $F081 LOD table for "stretchy"
N $F081 LOD
B $F081,1,1 Width (bytes)
B $F082,1,1 Flags
B $F083,1,1 Height (pixels)
W $F084,2,2 [$F0EA] Bitmap address
W $F086,2,2 [$F0EA] Pre-shifted bitmap address
N $F088 LOD
B $F088,1,1 Width (bytes)
B $F089,1,1 Flags
B $F08A,1,1 Height (pixels)
W $F08B,2,2 [$F130] Bitmap address
W $F08D,2,2 [$F130] Pre-shifted bitmap address
N $F08F LOD
B $F08F,1,1 Width (bytes)
B $F090,1,1 Flags
B $F091,1,1 Height (pixels)
W $F092,2,2 [$F158] Bitmap address
W $F094,2,2 [$F158] Pre-shifted bitmap address
N $F096 LOD
B $F096,1,1 Width (bytes)
B $F097,1,1 Flags
B $F098,1,1 Height (pixels)
W $F099,2,2 [$F174] Bitmap address
W $F09B,2,2 [$F174] Pre-shifted bitmap address
N $F09D LOD
B $F09D,1,1 Width (bytes)
B $F09E,1,1 Flags
B $F09F,1,1 Height (pixels)
W $F0A0,2,2 [$F19C] Bitmap address
W $F0A2,2,2 [$F19C] Pre-shifted bitmap address
B $F0A4,70,8*8,6
B $F0EA,70,7 Bitmap data 7 bytes x 10
B $F130,40,5 Bitmap data 5 bytes x 8
B $F158,28,7 Bitmap data 7 bytes x 4
B $F174,40,5 Bitmap data 5 bytes x 8
B $F19C,15,5 Bitmap data 5 bytes x 3
B $F1AB,175,8*21,7
N $F25A Stretchy graphic
B $F25A,1,1 ?index
W $F25B,2,2 [$F274] Pointer to stretchy_graphic_part
B $F25D,1,1 ?index
W $F25E,2,2 [$F28A] Pointer to stretchy_graphic_part
B $F260,1,1 ?index
W $F261,2,2 [$F2A0] Pointer to stretchy_graphic_part
B $F263,1,1 ?index
W $F264,2,2 [$F2B6] Pointer to stretchy_graphic_part
B $F266,1,1 Terminator
N $F267 Stretchy graphic
B $F267,1,1 ?index
W $F268,2,2 [$F274] Pointer to stretchy_graphic_part
B $F26A,1,1 ?index
W $F26B,2,2 [$F2CC] Pointer to stretchy_graphic_part
B $F26D,1,1 ?index
W $F26E,2,2 [$F2E2] Pointer to stretchy_graphic_part
B $F270,1,1 ?index
W $F271,2,2 [$F2F8] Pointer to stretchy_graphic_part
B $F273,1,1 Terminator
N $F274 Stretchy graphic part
W $F274,2,2 [$F30E] LOD ptr
W $F276,2,2 TBD
W $F278,2,2 TBD
W $F27A,2,2 TBD
W $F27C,2,2 TBD
W $F27E,2,2 TBD
W $F280,2,2 TBD
W $F282,2,2 TBD
W $F284,2,2 TBD
W $F286,2,2 TBD
W $F288,2,2 TBD
N $F28A Stretchy graphic part
W $F28A,2,2 [$F30E] LOD ptr
W $F28C,2,2 TBD
W $F28E,2,2 TBD
W $F290,2,2 TBD
W $F292,2,2 TBD
W $F294,2,2 TBD
W $F296,2,2 TBD
W $F298,2,2 TBD
W $F29A,2,2 TBD
W $F29C,2,2 TBD
W $F29E,2,2 TBD
N $F2A0 Stretchy graphic part
W $F2A0,2,2 [$F30E] LOD ptr
W $F2A2,2,2 TBD
W $F2A4,2,2 TBD
W $F2A6,2,2 TBD
W $F2A8,2,2 TBD
W $F2AA,2,2 TBD
W $F2AC,2,2 TBD
W $F2AE,2,2 TBD
W $F2B0,2,2 TBD
W $F2B2,2,2 TBD
W $F2B4,2,2 TBD
N $F2B6 Stretchy graphic part
W $F2B6,2,2 [$F30E] LOD ptr
W $F2B8,2,2 TBD
W $F2BA,2,2 TBD
W $F2BC,2,2 TBD
W $F2BE,2,2 TBD
W $F2C0,2,2 TBD
W $F2C2,2,2 TBD
W $F2C4,2,2 TBD
W $F2C6,2,2 TBD
W $F2C8,2,2 TBD
W $F2CA,2,2 TBD
N $F2CC Stretchy graphic part
W $F2CC,2,2 [$F30E] LOD ptr
W $F2CE,2,2 TBD
W $F2D0,2,2 TBD
W $F2D2,2,2 TBD
W $F2D4,2,2 TBD
W $F2D6,2,2 TBD
W $F2D8,2,2 TBD
W $F2DA,2,2 TBD
W $F2DC,2,2 TBD
W $F2DE,2,2 TBD
W $F2E0,2,2 TBD
N $F2E2 Stretchy graphic part
W $F2E2,2,2 [$F30E] LOD ptr
W $F2E4,2,2 TBD
W $F2E6,2,2 TBD
W $F2E8,2,2 TBD
W $F2EA,2,2 TBD
W $F2EC,2,2 TBD
W $F2EE,2,2 TBD
W $F2F0,2,2 TBD
W $F2F2,2,2 TBD
W $F2F4,2,2 TBD
W $F2F6,2,2 TBD
N $F2F8 Stretchy graphic part
W $F2F8,2,2 [$F30E] LOD ptr
W $F2FA,2,2 TBD
W $F2FC,2,2 TBD
W $F2FE,2,2 TBD
W $F300,2,2 TBD
W $F302,2,2 TBD
W $F304,2,2 TBD
W $F306,2,2 TBD
W $F308,2,2 TBD
W $F30A,2,2 TBD
W $F30C,2,2 TBD
N $F30E LOD table for "stretchy"
N $F30E LOD
B $F30E,1,1 Width (bytes)
B $F30F,1,1 Flags
B $F310,1,1 Height (pixels)
W $F311,2,2 [$F39A] Bitmap address
W $F313,2,2 [$F39A] Pre-shifted bitmap address
N $F315 LOD
B $F315,1,1 Width (bytes)
B $F316,1,1 Flags
B $F317,1,1 Height (pixels)
W $F318,2,2 [$F3E2] Bitmap address
W $F31A,2,2 [$F3E2] Pre-shifted bitmap address
N $F31C LOD
B $F31C,1,1 Width (bytes)
B $F31D,1,1 Flags
B $F31E,1,1 Height (pixels)
W $F31F,2,2 [$F3FB] Bitmap address
W $F321,2,2 [$F3FB] Pre-shifted bitmap address
N $F323 LOD
B $F323,1,1 Width (bytes)
B $F324,1,1 Flags
B $F325,1,1 Height (pixels)
W $F326,2,2 [$F40B] Bitmap address
W $F328,2,2 [$F40B] Pre-shifted bitmap address
N $F32A LOD
B $F32A,1,1 Width (bytes)
B $F32B,1,1 Flags
B $F32C,1,1 Height (pixels)
W $F32D,2,2 [$F420] Bitmap address
W $F32F,2,2 [$F420] Pre-shifted bitmap address
B $F331,105,8*13,1
B $F39A,72,6 Bitmap data 6 bytes x 12
B $F3E2,25,5 Bitmap data 5 bytes x 5
B $F3FB,16,4 Bitmap data 4 bytes x 4
B $F40B,21,3 Bitmap data 3 bytes x 7
B $F420,48,4 Bitmap data 4 bytes x 12
B $F450,136,8
N $F4D8 LOD table for "hittable hazard"
N $F4D8 LOD
B $F4D8,1,1 Width (bytes)
B $F4D9,1,1 Flags
B $F4DA,1,1 Height (pixels)
W $F4DB,2,2 [$F502] Bitmap address
W $F4DD,2,2 [$F502] Pre-shifted bitmap address
N $F4DF LOD
B $F4DF,1,1 Width (bytes)
B $F4E0,1,1 Flags
B $F4E1,1,1 Height (pixels)
W $F4E2,2,2 [$F541] Bitmap address
W $F4E4,2,2 [$F541] Pre-shifted bitmap address
N $F4E6 LOD
B $F4E6,1,1 Width (bytes)
B $F4E7,1,1 Flags
B $F4E8,1,1 Height (pixels)
W $F4E9,2,2 [$F561] Bitmap address
W $F4EB,2,2 [$F58D] Pre-shifted bitmap address
N $F4ED LOD
B $F4ED,1,1 Width (bytes)
B $F4EE,1,1 Flags
B $F4EF,1,1 Height (pixels)
W $F4F0,2,2 [$F5B9] Bitmap address
W $F4F2,2,2 [$F5DD] Pre-shifted bitmap address
N $F4F4 LOD
B $F4F4,1,1 Width (bytes)
B $F4F5,1,1 Flags
B $F4F6,1,1 Height (pixels)
W $F4F7,2,2 [$F5B9] Bitmap address
W $F4F9,2,2 [$F5DD] Pre-shifted bitmap address
N $F4FB LOD
B $F4FB,1,1 Width (bytes)
B $F4FC,1,1 Flags
B $F4FD,1,1 Height (pixels)
W $F4FE,2,2 [$F601] Bitmap address
W $F500,2,2 [$F60B] Pre-shifted bitmap address
B $F502,63,3 Bitmap data 3 bytes x 21
B $F541,32,2 Bitmap data 2 bytes x 16
B $F561,44,4 Bitmap data (masked) 4 bytes x 11
B $F58D,44,4 Pre-shifted bitmap data (masked) 4 bytes x 11
B $F5B9,36,4 Bitmap data (masked) 4 bytes x 9
B $F5DD,36,4 Pre-shifted bitmap data (masked) 4 bytes x 9
B $F601,10,2 Bitmap data (masked) 2 bytes x 5
B $F60B,10,2 Pre-shifted bitmap data (masked) 2 bytes x 5
N $F615 LOD table for "hittable hazard"
N $F615 LOD
B $F615,1,1 Width (bytes)
B $F616,1,1 Flags
B $F617,1,1 Height (pixels)
W $F618,2,2 [$F63F] Bitmap address
W $F61A,2,2 [$F63F] Pre-shifted bitmap address
N $F61C LOD
B $F61C,1,1 Width (bytes)
B $F61D,1,1 Flags
B $F61E,1,1 Height (pixels)
W $F61F,2,2 [$F63F] Bitmap address
W $F621,2,2 [$F63F] Pre-shifted bitmap address
N $F623 LOD
B $F623,1,1 Width (bytes)
B $F624,1,1 Flags
B $F625,1,1 Height (pixels)
W $F626,2,2 [$F683] Bitmap address
W $F628,2,2 [$F683] Pre-shifted bitmap address
N $F62A LOD
B $F62A,1,1 Width (bytes)
B $F62B,1,1 Flags
B $F62C,1,1 Height (pixels)
W $F62D,2,2 [$F6AA] Bitmap address
W $F62F,2,2 [$F6AA] Pre-shifted bitmap address
N $F631 LOD
B $F631,1,1 Width (bytes)
B $F632,1,1 Flags
B $F633,1,1 Height (pixels)
W $F634,2,2 [$F6AA] Bitmap address
W $F636,2,2 [$F6AA] Pre-shifted bitmap address
N $F638 LOD
B $F638,1,1 Width (bytes)
B $F639,1,1 Flags
B $F63A,1,1 Height (pixels)
W $F63B,2,2 [$F6BC] Bitmap address
W $F63D,2,2 [$F6D8] Pre-shifted bitmap address
B $F63F,68,4 Bitmap data 4 bytes x 17
B $F683,39,3 Bitmap data 3 bytes x 13
B $F6AA,18,2 Bitmap data 2 bytes x 9
B $F6BC,28,4 Bitmap data (masked) 4 bytes x 7
B $F6D8,28,4 Pre-shifted bitmap data (masked) 4 bytes x 7
N $F6F4 Stretchy graphic
B $F6F4,1,1 ?index
W $F6F5,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $F6F7,1,1 ?index
W $F6F8,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $F6FA,1,1 ?index
W $F6FB,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $F6FD,1,1 ?index
W $F6FE,2,2 [$F70E] Pointer to stretchy_graphic_part
B $F700,1,1 Terminator
N $F701 Stretchy graphic
B $F701,1,1 ?index
W $F702,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $F704,1,1 ?index
W $F705,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $F707,1,1 ?index
W $F708,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $F70A,1,1 ?index
W $F70B,2,2 [$F724] Pointer to stretchy_graphic_part
B $F70D,1,1 Terminator
N $F70E Stretchy graphic part
W $F70E,2,2 [$F73A] LOD ptr
W $F710,2,2 TBD
W $F712,2,2 TBD
W $F714,2,2 TBD
W $F716,2,2 TBD
W $F718,2,2 TBD
W $F71A,2,2 TBD
W $F71C,2,2 TBD
W $F71E,2,2 TBD
W $F720,2,2 TBD
W $F722,2,2 TBD
N $F724 Stretchy graphic part
W $F724,2,2 [$F75D] LOD ptr
W $F726,2,2 TBD
W $F728,2,2 TBD
W $F72A,2,2 TBD
W $F72C,2,2 TBD
W $F72E,2,2 TBD
W $F730,2,2 TBD
W $F732,2,2 TBD
W $F734,2,2 TBD
W $F736,2,2 TBD
W $F738,2,2 TBD
N $F73A LOD table for "stretchy"
N $F73A LOD
B $F73A,1,1 Width (bytes)
B $F73B,1,1 Flags
B $F73C,1,1 Height (pixels)
W $F73D,2,2 [$F780] Bitmap address
W $F73F,2,2 [$F780] Pre-shifted bitmap address
N $F741 LOD
B $F741,1,1 Width (bytes)
B $F742,1,1 Flags
B $F743,1,1 Height (pixels)
W $F744,2,2 [$F7A0] Bitmap address
W $F746,2,2 [$F7A0] Pre-shifted bitmap address
N $F748 LOD
B $F748,1,1 Width (bytes)
B $F749,1,1 Flags
B $F74A,1,1 Height (pixels)
W $F74B,2,2 [$F7AF] Bitmap address
W $F74D,2,2 [$F7BB] Pre-shifted bitmap address
N $F74F LOD
B $F74F,1,1 Width (bytes)
B $F750,1,1 Flags
B $F751,1,1 Height (pixels)
W $F752,2,2 [$F7C7] Bitmap address
W $F754,2,2 [$F7D7] Pre-shifted bitmap address
N $F756 LOD
B $F756,1,1 Width (bytes)
B $F757,1,1 Flags
B $F758,1,1 Height (pixels)
W $F759,2,2 [$F7E7] Bitmap address
W $F75B,2,2 [$F7F3] Pre-shifted bitmap address
N $F75D LOD table for "stretchy"
N $F75D LOD
B $F75D,1,1 Width (bytes)
B $F75E,1,1 Flags
B $F75F,1,1 Height (pixels)
W $F760,2,2 [$F780] Bitmap address
W $F762,2,2 [$F780] Pre-shifted bitmap address
N $F764 LOD
B $F764,1,1 Width (bytes)
B $F765,1,1 Flags
B $F766,1,1 Height (pixels)
W $F767,2,2 [$F7A0] Bitmap address
W $F769,2,2 [$F7A0] Pre-shifted bitmap address
N $F76B LOD
B $F76B,1,1 Width (bytes)
B $F76C,1,1 Flags
B $F76D,1,1 Height (pixels)
W $F76E,2,2 [$F7AF] Bitmap address
W $F770,2,2 [$F7BB] Pre-shifted bitmap address
N $F772 LOD
B $F772,1,1 Width (bytes)
B $F773,1,1 Flags
B $F774,1,1 Height (pixels)
W $F775,2,2 [$F7C7] Bitmap address
W $F777,2,2 [$F7D7] Pre-shifted bitmap address
N $F779 LOD
B $F779,1,1 Width (bytes)
B $F77A,1,1 Flags
B $F77B,1,1 Height (pixels)
W $F77C,2,2 [$F7E7] Bitmap address
W $F77E,2,2 [$F7F3] Pre-shifted bitmap address
B $F780,32,4 Bitmap data 4 bytes x 8
B $F7A0,15,3 Bitmap data 3 bytes x 5
B $F7AF,12,3 Bitmap data 3 bytes x 4
B $F7BB,12,3 Pre-shifted bitmap data 3 bytes x 4
B $F7C7,16,4 Bitmap data (masked) 4 bytes x 4
B $F7D7,16,4 Pre-shifted bitmap data (masked) 4 bytes x 4
B $F7E7,12,4 Bitmap data (masked) 4 bytes x 3
B $F7F3,12,4 Pre-shifted bitmap data (masked) 4 bytes x 3
N $F7FF draw_object_left/right graphic data
W $F7FF,2,2 [$F82B] LOD ptr
W $F801,2,2 TBD
W $F803,2,2 TBD
W $F805,2,2 TBD
W $F807,2,2 TBD
W $F809,2,2 TBD
W $F80B,2,2 TBD
W $F80D,2,2 TBD
W $F80F,2,2 TBD
W $F811,2,2 TBD
W $F813,2,2 TBD
N $F815 draw_object_left/right graphic data
W $F815,2,2 [$F82B] LOD ptr
W $F817,2,2 TBD
W $F819,2,2 TBD
W $F81B,2,2 TBD
W $F81D,2,2 TBD
W $F81F,2,2 TBD
W $F821,2,2 TBD
W $F823,2,2 TBD
W $F825,2,2 TBD
W $F827,2,2 TBD
W $F829,2,2 TBD
N $F82B LOD table for "non-stretchy"
N $F82B LOD
B $F82B,1,1 Width (bytes)
B $F82C,1,1 Flags
B $F82D,1,1 Height (pixels)
W $F82E,2,2 [$F871] Bitmap address
W $F830,2,2 [$F871] Pre-shifted bitmap address
N $F832 LOD
B $F832,1,1 Width (bytes)
B $F833,1,1 Flags
B $F834,1,1 Height (pixels)
W $F835,2,2 [$F911] Bitmap address
W $F837,2,2 [$F911] Pre-shifted bitmap address
N $F839 LOD
B $F839,1,1 Width (bytes)
B $F83A,1,1 Flags
B $F83B,1,1 Height (pixels)
W $F83C,2,2 [$F96B] Bitmap address
W $F83E,2,2 [$F96B] Pre-shifted bitmap address
N $F840 LOD
B $F840,1,1 Width (bytes)
B $F841,1,1 Flags
B $F842,1,1 Height (pixels)
W $F843,2,2 [$F993] Bitmap address
W $F845,2,2 [$F993] Pre-shifted bitmap address
N $F847 LOD
B $F847,1,1 Width (bytes)
B $F848,1,1 Flags
B $F849,1,1 Height (pixels)
W $F84A,2,2 [$F9D3] Bitmap address
W $F84C,2,2 [$FA07] Pre-shifted bitmap address
N $F84E LOD
B $F84E,1,1 Width (bytes)
B $F84F,1,1 Flags
B $F850,1,1 Height (pixels)
W $F851,2,2 [$F871] Bitmap address
W $F853,2,2 [$F871] Pre-shifted bitmap address
N $F855 LOD
B $F855,1,1 Width (bytes)
B $F856,1,1 Flags
B $F857,1,1 Height (pixels)
W $F858,2,2 [$F911] Bitmap address
W $F85A,2,2 [$F911] Pre-shifted bitmap address
N $F85C LOD
B $F85C,1,1 Width (bytes)
B $F85D,1,1 Flags
B $F85E,1,1 Height (pixels)
W $F85F,2,2 [$F96B] Bitmap address
W $F861,2,2 [$F96B] Pre-shifted bitmap address
N $F863 LOD
B $F863,1,1 Width (bytes)
B $F864,1,1 Flags
B $F865,1,1 Height (pixels)
W $F866,2,2 [$F993] Bitmap address
W $F868,2,2 [$F993] Pre-shifted bitmap address
N $F86A LOD
B $F86A,1,1 Width (bytes)
B $F86B,1,1 Flags
B $F86C,1,1 Height (pixels)
W $F86D,2,2 [$F9D3] Bitmap address
W $F86F,2,2 [$FA07] Pre-shifted bitmap address
B $F871,160,4 Bitmap data 4 bytes x 40
B $F911,90,3 Bitmap data 3 bytes x 30
B $F96B,40,2 Bitmap data 2 bytes x 20
B $F993,64,4 Bitmap data (masked) 4 bytes x 16
B $F9D3,52,4 Bitmap data (masked) 4 bytes x 13
B $FA07,52,4 Pre-shifted bitmap data (masked) 4 bytes x 13
B $FA3B,1477,8*184,5
