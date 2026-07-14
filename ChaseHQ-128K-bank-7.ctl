b $C000 [Stage 5] Horizon graphic
B $C000,240,8
b $C0F0 [Stage 5] Per-stage data
W $C0F0,2,2 [$C8CE] Address of perp's mugshot (attributes)
W $C0F2,2,2 [out-of-bounds] Address of pilot's mugshot (bitmap)
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
W $C108,2,2 [out-of-bounds] Helicopter data 1
W $C10A,2,2 [out-of-bounds] Helicopter data 2
w $C10C [Stage 5] Table of addresses of LODs
W $C10C,2,2 [out-of-bounds] Address of LOD of Hazard (stone/dust)
W $C10E,2,2 [out-of-bounds] Address of LOD of Hazard (stone/dust)
W $C110,2,2 [$C8E2] Address of LOD of Car A (the perp's car)
W $C112,2,2 [$C936] Address of LOD of Car B (a Lambo in S1)
W $C114,2,2 [$C90C] Address of LOD of Car C (a truck in S1)
W $C116,2,2 [$C936] Address of LOD of Car D (a Lambo in S1)
W $C118,2,2 [$C8E2] Address of LOD of Car E (a generic car in S1)
b $C11A [Stage 5] Per-stage difficulty settings
B $C11A,1,1 How often cars spawn. Lower values spawn cars more often.
B $C11B,1,1 Smash config parameter TBD
B $C11C,1,1 Smash config parameter TBD
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
B $C142,1,1 Escape: Jump
W $C143,2,2 Address of next message (always $98BD)
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
b $C23F [Stage 5] Object graphic definitions (right)
N $C23F Graphic definition for object 1 - TUNNEL_LIGHT
B $C23F,1,1 Hit coord max (furthest)
B $C240,1,1 Hit coord min (nearest)
B $C241,1,1 How far to push hero car away if hit
W $C242,2,2 Argument - tunnellight
W $C244,2,2 [out-of-bounds] Address of routine draw_tunnel_light_right
N $C246 Graphic definition for object 2 - OVERHEAD_BRIDGE
B $C246,1,1 Hit coord max (furthest)
B $C247,1,1 Hit coord min (nearest)
B $C248,1,1 How far to push hero car away if hit
W $C249,2,2 [$D28C] Argument for routine passed in #REGde
W $C24B,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $C24D Graphic definition for object 3 - (object 3 - unused)
B $C24D,1,1 Hit coord max (furthest)
B $C24E,1,1 Hit coord min (nearest)
B $C24F,1,1 How far to push hero car away if hit
W $C250,2,2 Argument - stretchy_shortpole
W $C252,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $C254 Graphic definition for object 4 - CACTUS
B $C254,1,1 Hit coord max (furthest)
B $C255,1,1 Hit coord min (nearest)
B $C256,1,1 How far to push hero car away if hit
W $C257,2,2 [$D6FF] Argument for routine passed in #REGde
W $C259,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $C25B Graphic definition for object 5 - DOUBLE_STREET_LAMP
B $C25B,1,1 Hit coord max (furthest)
B $C25C,1,1 Hit coord min (nearest)
B $C25D,1,1 How far to push hero car away if hit
W $C25E,2,2 [$D8D2] Argument for routine passed in #REGde
W $C260,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $C262 Graphic definition for object 6 - HUGE_ROCK
B $C262,1,1 Hit coord max (furthest)
B $C263,1,1 Hit coord min (nearest)
B $C264,1,1 How far to push hero car away if hit
W $C265,2,2 [$CE91] Argument for routine passed in #REGde
W $C267,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
N $C269 Graphic definition for object 7 - TELEGRAPH_POLE
B $C269,1,1 Hit coord max (furthest)
B $C26A,1,1 Hit coord min (nearest)
B $C26B,1,1 How far to push hero car away if hit
W $C26C,2,2 [$D9CF] Argument for routine passed in #REGde
W $C26E,2,2 [out-of-bounds] Address of routine draw_stretchy_object_right
b $C270 [Stage 5] Object graphic definitions (left)
N $C270 Graphic definition for object 1 - TUNNEL_LIGHT
B $C270,1,1 Hit coord min (furthest)
B $C271,1,1 Hit coord max (nearest)
B $C272,1,1 How far to push hero car away if hit
W $C273,2,2 Argument - tunnellight
W $C275,2,2 [out-of-bounds] Address of routine draw_tunnel_light_left
N $C277 Graphic definition for object 2 - OVERHEAD_BRIDGE
B $C277,1,1 Hit coord min (furthest)
B $C278,1,1 Hit coord max (nearest)
B $C279,1,1 How far to push hero car away if hit
W $C27A,2,2 [$D2F7] Argument for routine passed in #REGde
W $C27C,2,2 [out-of-bounds] Address of routine draw_overhead
N $C27E Graphic definition for object 3 - (object 3 - unused)
B $C27E,1,1 Hit coord min (furthest)
B $C27F,1,1 Hit coord max (nearest)
B $C280,1,1 How far to push hero car away if hit
W $C281,2,2 Argument - stretchy_shortpole
W $C283,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
N $C285 Graphic definition for object 4 - CACTUS
B $C285,1,1 Hit coord min (furthest)
B $C286,1,1 Hit coord max (nearest)
B $C287,1,1 How far to push hero car away if hit
W $C288,2,2 [$D70C] Argument for routine passed in #REGde
W $C28A,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
N $C28C Graphic definition for object 5 - DOUBLE_STREET_LAMP
B $C28C,1,1 Hit coord min (furthest)
B $C28D,1,1 Hit coord max (nearest)
B $C28E,1,1 How far to push hero car away if hit
W $C28F,2,2 [$D8DF] Argument for routine passed in #REGde
W $C291,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
N $C293 Graphic definition for object 6 - HUGE_ROCK
B $C293,1,1 Hit coord min (furthest)
B $C294,1,1 Hit coord max (nearest)
B $C295,1,1 How far to push hero car away if hit
W $C296,2,2 [$CEF6] Argument for routine passed in #REGde
W $C298,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
N $C29A Graphic definition for object 7 - TELEGRAPH_POLE
B $C29A,1,1 Hit coord min (furthest)
B $C29B,1,1 Hit coord max (nearest)
B $C29C,1,1 How far to push hero car away if hit
W $C29D,2,2 [$D9D9] Argument for routine passed in #REGde
W $C29F,2,2 [out-of-bounds] Address of routine draw_stretchy_object_left
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
B $C30E,1,1 Start Spawning HAZARD_2 Right
B $C30F,1,1
B $C310,1,1 Wait for 2 units
B $C311,1,1 Stop Spawning Hazards
B $C312,1,1
B $C313,1,1 Wait for 45 units
B $C314,1,1 Start Spawning HAZARD_2 Left
B $C315,1,1
B $C316,1,1 Wait for 2 units
B $C317,1,1 Stop Spawning Hazards
B $C318,1,1
B $C319,1,1 Wait for 43 units
B $C31A,1,1 Start Spawning HAZARD_2 Both Sides
B $C31B,1,1
B $C31C,1,1 Wait for 4 units
B $C31D,1,1 Stop Spawning Hazards
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
B $C32C,1,1 Alternating (CACTUS, EMPTY) for 16 units
B $C32D,15,8,7
B $C33C,1,1 CACTUS for 1 units
B $C33D,1,1 Alternating (CACTUS, TELEGRAPH_POLE) for 6 units
B $C33E,6,6
B $C344,1,1 CACTUS for 4 units
B $C345,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 18 units
B $C346,18,8*2,2
B $C358,1,1 Alternating (CACTUS, EMPTY) for 6 units
B $C359,5,5
B $C35E,1,1 CACTUS for 4 units
B $C35F,1,1 Alternating (DOUBLE_STREET_LAMP, EMPTY) for 38 units
B $C360,38,8*4,6
B $C386,1,1 DOUBLE_STREET_LAMP for 4 units
B $C387,1,1 EMPTY for 3 units
B $C388,1,1 OVERHEAD_BRIDGE for 1 units
B $C389,1,1 EMPTY for 1 units
B $C38A,1,1 OVERHEAD_BRIDGE for 1 units
B $C38B,1,1 EMPTY for 5 units
B $C38C,1,1 OVERHEAD_BRIDGE for 1 units
B $C38D,1,1 EMPTY for 1 units
B $C38E,1,1 OVERHEAD_BRIDGE for 1 units
B $C38F,1,1 EMPTY for 5 units
B $C390,1,1 OVERHEAD_BRIDGE for 1 units
B $C391,1,1 EMPTY for 1 units
B $C392,1,1 OVERHEAD_BRIDGE for 1 units
B $C393,1,1 EMPTY for 5 units
B $C394,1,1 OVERHEAD_BRIDGE for 1 units
B $C395,1,1 EMPTY for 1 units
B $C396,1,1 OVERHEAD_BRIDGE for 1 units
B $C397,1,1 Alternating (CACTUS, EMPTY) for 20 units
B $C398,20,8*2,4
B $C3AC,1,1 CACTUS for 10 units
B $C3AD,1,1 EMPTY for 3 units
B $C3AE,1,1 CACTUS for 1 units
B $C3AF,1,1 EMPTY for 3 units
B $C3B0,1,1 CACTUS for 1 units
B $C3B1,1,1 EMPTY for 1 units
B $C3B2,1,1 CACTUS for 1 units
B $C3B3,1,1 EMPTY for 3 units
B $C3B4,1,1 CACTUS for 1 units
B $C3B5,1,1 EMPTY for 3 units
B $C3B6,1,1 CACTUS for 1 units
B $C3B7,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 14 units
B $C3B8,14,8,6
B $C3C6,1,1 TELEGRAPH_POLE for 4 units
B $C3C7,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 4 units
B $C3C8,4,4
B $C3CC,1,1 <Esc> Split
B $C3CD,1,1
W $C3CE,2,2 [$C560] Left target
W $C3D0,2,2 [$C49A] Right target
b $C3D2 [Stage 5] Map right object data
B $C3D2,1,1 EMPTY for 1 units
B $C3D3,1,1 DOUBLE_STREET_LAMP for 1 units
B $C3D4,1,1 EMPTY for 3 units
B $C3D5,1,1 DOUBLE_STREET_LAMP for 1 units
B $C3D6,1,1 EMPTY for 1 units
B $C3D7,1,1 DOUBLE_STREET_LAMP for 1 units
B $C3D8,1,1 Alternating (CACTUS, EMPTY) for 6 units
B $C3D9,6,6
B $C3DF,1,1 CACTUS for 5 units
B $C3E0,1,1 Alternating (CACTUS, EMPTY) for 4 units
B $C3E1,4,4
B $C3E5,1,1 CACTUS for 14 units
B $C3E6,1,1 EMPTY for 3 units
B $C3E7,1,1 CACTUS for 1 units
B $C3E8,1,1 Alternating (CACTUS, EMPTY) for 4 units
B $C3E9,4,4
B $C3ED,1,1 CACTUS for 4 units
B $C3EE,1,1 EMPTY for 3 units
B $C3EF,1,1 CACTUS for 1 units
B $C3F0,1,1 EMPTY for 3 units
B $C3F1,1,1 TELEGRAPH_POLE for 1 units
B $C3F2,1,1 EMPTY for 1 units
B $C3F3,1,1 TELEGRAPH_POLE for 1 units
B $C3F4,1,1 EMPTY for 3 units
B $C3F5,1,1 TELEGRAPH_POLE for 1 units
B $C3F6,1,1 EMPTY for 3 units
B $C3F7,1,1 TELEGRAPH_POLE for 1 units
B $C3F8,1,1 EMPTY for 3 units
B $C3F9,1,1 TELEGRAPH_POLE for 1 units
B $C3FA,1,1 EMPTY for 3 units
B $C3FB,1,1 TELEGRAPH_POLE for 1 units
B $C3FC,1,1 EMPTY for 3 units
B $C3FD,1,1 TELEGRAPH_POLE for 1 units
B $C3FE,1,1 EMPTY for 3 units
B $C3FF,1,1 TELEGRAPH_POLE for 1 units
B $C400,1,1 EMPTY for 3 units
B $C401,1,1 TELEGRAPH_POLE for 1 units
B $C402,1,1 EMPTY for 1 units
B $C403,1,1 TELEGRAPH_POLE for 1 units
B $C404,1,1 EMPTY for 11 units
B $C405,1,1 OVERHEAD_BRIDGE for 1 units
B $C406,1,1 EMPTY for 1 units
B $C407,1,1 OVERHEAD_BRIDGE for 1 units
B $C408,1,1 EMPTY for 5 units
B $C409,1,1 OVERHEAD_BRIDGE for 1 units
B $C40A,1,1 EMPTY for 1 units
B $C40B,1,1 OVERHEAD_BRIDGE for 1 units
B $C40C,1,1 EMPTY for 5 units
B $C40D,1,1 OVERHEAD_BRIDGE for 1 units
B $C40E,1,1 EMPTY for 1 units
B $C40F,1,1 OVERHEAD_BRIDGE for 1 units
B $C410,1,1 EMPTY for 5 units
B $C411,1,1 OVERHEAD_BRIDGE for 1 units
B $C412,1,1 EMPTY for 1 units
B $C413,1,1 OVERHEAD_BRIDGE for 1 units
B $C414,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 4 units
B $C415,4,4
B $C419,1,1 TELEGRAPH_POLE for 6 units
B $C41A,1,1 EMPTY for 3 units
B $C41B,1,1 TELEGRAPH_POLE for 1 units
B $C41C,1,1 EMPTY for 1 units
B $C41D,1,1 TELEGRAPH_POLE for 1 units
B $C41E,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 14 units
B $C41F,14,8,6
B $C42D,1,1 Alternating (CACTUS, EMPTY) for 14 units
B $C42E,13,8,5
B $C43B,1,1 CACTUS for 4 units
B $C43C,1,1 Alternating (TELEGRAPH_POLE, CACTUS) for 4 units
B $C43D,4,4
B $C441,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 4 units
B $C442,3,3
B $C445,1,1 Alternating (CACTUS, EMPTY) for 4 units
B $C446,3,3
B $C449,1,1 CACTUS for 4 units
B $C44A,1,1 EMPTY for 3 units
B $C44B,1,1 CACTUS for 1 units
B $C44C,1,1 EMPTY for 1 units
B $C44D,1,1 CACTUS for 1 units
B $C44E,1,1 EMPTY for 3 units
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
B $C49A,1,1 EMPTY for 2 units
B $C49B,1,1 TUNNEL_LIGHT for 10 units
B $C49C,1,1 Alternating (CACTUS, EMPTY) for 14 units
B $C49D,14,8,6
B $C4AB,1,1 CACTUS for 3 units
B $C4AC,1,1 EMPTY for 3 units
B $C4AD,1,1 CACTUS for 1 units
B $C4AE,1,1 EMPTY for 3 units
B $C4AF,1,1 CACTUS for 1 units
B $C4B0,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 20 units
B $C4B1,20,8*2,4
B $C4C5,1,1 TELEGRAPH_POLE for 4 units
B $C4C6,1,1 Alternating (CACTUS, EMPTY) for 6 units
B $C4C7,6,6
B $C4CD,1,1 CACTUS for 10 units
B $C4CE,1,1 EMPTY for 5 units
B $C4CF,1,1 CACTUS for 1 units
B $C4D0,1,1 EMPTY for 1 units
B $C4D1,1,1 CACTUS for 1 units
B $C4D2,1,1 EMPTY for 5 units
B $C4D3,1,1 CACTUS for 1 units
B $C4D4,1,1 EMPTY for 1 units
B $C4D5,1,1 CACTUS for 1 units
B $C4D6,1,1 EMPTY for 3 units
B $C4D7,1,1 CACTUS for 1 units
B $C4D8,1,1 EMPTY for 2 units
B $C4D9,1,1 <Esc> Jump
B $C4DA,1,1
W $C4DB,2,2 [$C62D] Target
b $C4DD [Stage 5] Map right object data
B $C4DD,1,1 EMPTY for 2 units
B $C4DE,1,1 TUNNEL_LIGHT for 10 units
B $C4DF,1,1 Alternating (CACTUS, EMPTY) for 8 units
B $C4E0,9,8,1
B $C4E9,1,1 CACTUS for 19 units
B $C4EA,1,1 EMPTY for 7 units
B $C4EB,1,1 CACTUS for 1 units
B $C4EC,1,1 EMPTY for 3 units
B $C4ED,1,1 CACTUS for 1 units
B $C4EE,1,1 EMPTY for 1 units
B $C4EF,1,1 CACTUS for 1 units
B $C4F0,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 24 units
B $C4F1,24,8
B $C509,1,1 TELEGRAPH_POLE for 12 units
B $C50A,1,1 EMPTY for 3 units
B $C50B,1,1 CACTUS for 1 units
B $C50C,1,1 EMPTY for 3 units
B $C50D,1,1 CACTUS for 1 units
B $C50E,1,1 EMPTY for 2 units
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
B $C556,1,1 Start Spawning HAZARD_2 Right
B $C557,1,1
B $C558,1,1 Wait for 2 units
B $C559,1,1 Stop Spawning Hazards
B $C55A,1,1
B $C55B,1,1 Wait for 79 units
B $C55C,1,1 <Esc> Jump
B $C55D,1,1
W $C55E,2,2 [$C628] Target
b $C560 [Stage 5] Map left object data
B $C560,1,1 Alternating (OVERHEAD_BRIDGE, EMPTY) for 22 units
B $C561,22,8*2,6
B $C577,1,1 OVERHEAD_BRIDGE for 3 units
B $C578,1,1 Alternating (OVERHEAD_BRIDGE, EMPTY) for 30 units
B $C579,31,8*3,7
B $C598,1,1 OVERHEAD_BRIDGE for 22 units
B $C599,1,1 EMPTY for 7 units
B $C59A,1,1 TUNNEL_LIGHT for 10 units
B $C59B,1,1 EMPTY for 2 units
B $C59C,1,1 DOUBLE_STREET_LAMP for 1 units
B $C59D,1,1 EMPTY for 1 units
B $C59E,1,1 DOUBLE_STREET_LAMP for 1 units
B $C59F,1,1 <Esc> Jump
B $C5A0,1,1
W $C5A1,2,2 [$C62D] Target
b $C5A3 [Stage 5] Map right object data
B $C5A3,1,1 Alternating (OVERHEAD_BRIDGE, EMPTY) for 22 units
B $C5A4,22,8*2,6
B $C5BA,1,1 OVERHEAD_BRIDGE for 3 units
B $C5BB,1,1 Alternating (OVERHEAD_BRIDGE, EMPTY) for 30 units
B $C5BC,31,8*3,7
B $C5DB,1,1 OVERHEAD_BRIDGE for 22 units
B $C5DC,1,1 EMPTY for 7 units
B $C5DD,1,1 TUNNEL_LIGHT for 10 units
B $C5DE,1,1 EMPTY for 5 units
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
B $C62D,1,1 Alternating (TELEGRAPH_POLE, EMPTY) for 24 units
B $C62E,24,8
B $C646,1,1 TELEGRAPH_POLE for 3 units
B $C647,1,1 EMPTY for 3 units
B $C648,1,1 TUNNEL_LIGHT for 51 units
B $C649,3,3
B $C64C,1,1 EMPTY for 12 units
B $C64D,1,1 <Esc> Jump
B $C64E,1,1
W $C64F,2,2 [$C6EE] Target
b $C651 [Stage 5] Map right object data
B $C651,1,1 EMPTY for 93 units
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
B $C6D2,1,1 Start Spawning HAZARD_2 Both Sides
B $C6D3,1,1
B $C6D4,1,1 Wait for 4 units
B $C6D5,1,1 Stop Spawning Hazards
B $C6D6,1,1
B $C6D7,1,1 Wait for 40 units
B $C6D8,1,1 Start Spawning HAZARD_2 Left
B $C6D9,1,1
B $C6DA,1,1 Wait for 2 units
B $C6DB,1,1 Stop Spawning Hazards
B $C6DC,1,1
B $C6DD,1,1 Wait for 91 units
B $C6DE,1,1 Start Spawning HAZARD_2 Left
B $C6DF,1,1
B $C6E0,1,1 Wait for 2 units
B $C6E1,1,1 Stop Spawning Hazards
B $C6E2,1,1
B $C6E3,1,1 Wait for 28 units
B $C6E4,1,1 Start Spawning HAZARD_2 Left
B $C6E5,1,1
B $C6E6,1,1 Wait for 2 units
B $C6E7,1,1 Stop Spawning Hazards
B $C6E8,1,1
B $C6E9,1,1 Wait for 40 units
B $C6EA,1,1 <Esc> Loop
B $C6EB,1,1
W $C6EC,2,2 [$C6D1] Target
b $C6EE [Stage 5] Map left object data
B $C6EE,1,1 Alternating (DOUBLE_STREET_LAMP, EMPTY) for 12 units
B $C6EF,11,8,3
B $C6FA,1,1 DOUBLE_STREET_LAMP for 1 units
B $C6FB,1,1 Alternating (CACTUS, EMPTY) for 4 units
B $C6FC,4,4
B $C700,1,1 CACTUS for 6 units
B $C701,1,1 EMPTY for 3 units
B $C702,1,1 CACTUS for 1 units
B $C703,1,1 EMPTY for 1 units
B $C704,1,1 CACTUS for 1 units
B $C705,1,1 EMPTY for 3 units
B $C706,1,1 CACTUS for 1 units
B $C707,1,1 EMPTY for 3 units
B $C708,1,1 CACTUS for 1 units
B $C709,1,1 Alternating (HUGE_ROCK, EMPTY) for 34 units
B $C70A,34,8*4,2
B $C72C,1,1 HUGE_ROCK for 4 units
B $C72D,1,1 EMPTY for 3 units
B $C72E,1,1 HUGE_ROCK for 1 units
B $C72F,1,1 EMPTY for 3 units
B $C730,1,1 HUGE_ROCK for 1 units
B $C731,1,1 Alternating (HUGE_ROCK, EMPTY) for 30 units
B $C732,31,8*3,7
B $C751,1,1 HUGE_ROCK for 18 units
B $C752,1,1 Alternating (HUGE_ROCK, EMPTY) for 60 units
B $C753,60,8*7,4
B $C78F,1,1 HUGE_ROCK for 10 units
B $C790,1,1 Alternating (CACTUS, EMPTY) for 8 units
B $C791,8,8
B $C799,1,1 Alternating (DOUBLE_STREET_LAMP, EMPTY) for 8 units
B $C79A,7,7
B $C7A1,1,1 <Esc> Loop
B $C7A2,1,1
W $C7A3,2,2 [$C6EE] Target
b $C7A5 [Stage 5] Map right object data
B $C7A5,1,1 Alternating (CACTUS, EMPTY) for 6 units
B $C7A6,6,6
B $C7AC,1,1 CACTUS for 7 units
B $C7AD,1,1 Alternating (HUGE_ROCK, EMPTY) for 32 units
B $C7AE,32,8
B $C7CE,1,1 HUGE_ROCK for 6 units
B $C7CF,1,1 Alternating (HUGE_ROCK, EMPTY) for 50 units
B $C7D0,51,8*6,3
B $C803,1,1 HUGE_ROCK for 28 units
B $C804,1,1 EMPTY for 47 units
B $C805,3,3
B $C808,1,1 CACTUS for 1 units
B $C809,1,1 EMPTY for 1 units
B $C80A,1,1 CACTUS for 1 units
B $C80B,1,1 EMPTY for 3 units
B $C80C,1,1 CACTUS for 1 units
B $C80D,1,1 Alternating (HUGE_ROCK, EMPTY) for 20 units
B $C80E,20,8*2,4
B $C822,1,1 HUGE_ROCK for 6 units
B $C823,1,1 EMPTY for 5 units
B $C824,1,1 CACTUS for 1 units
B $C825,1,1 EMPTY for 1 units
B $C826,1,1 CACTUS for 1 units
B $C827,1,1 EMPTY for 3 units
B $C828,1,1 CACTUS for 1 units
B $C829,1,1 EMPTY for 1 units
B $C82A,1,1 <Esc> Loop
B $C82B,1,1
W $C82C,2,2 [$C7A5] Target
b $C82E [Stage 5] Perp's mugshot
B $C82E,160,4 Bitmap data for the perp's mugshot (32x40). Stored top-down.
B $C8CE,20,4 Attribute data for the perp's mugshot (4x5). Stored top-down.
N $C8E2 LOD table for "Car A (the perp's car)"
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
N $C90C LOD table for "Car C (a truck in S1)"
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
N $C936 LOD table for "Car B (a Lambo in S1)"
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
B $CAAF,48,6 Bitmap data (masked) 6 bytes x 8
B $CADF,48,6 Pre-shifted bitmap data (masked) 6 bytes x 8
B $CB0F,234,6 Bitmap data 6 bytes x 39
B $CBF9,145,5 Bitmap data 5 bytes x 29
B $CC8A,60,3 Bitmap data 3 bytes x 20
B $CCC6,48,4 Bitmap data (masked) 4 bytes x 12
B $CCF6,48,4 Pre-shifted bitmap data (masked) 4 bytes x 12
B $CD26,180,6 Bitmap data 6 bytes x 30
B $CDDA,80,4 Bitmap data 4 bytes x 20
B $CE2A,39,3 Bitmap data 3 bytes x 13
B $CE51,32,4 Bitmap data (masked) 4 bytes x 8
B $CE71,32,4 Pre-shifted bitmap data (masked) 4 bytes x 8
N $CE91 Stretchy graphic
B $CE91,1,1 ?index
W $CE92,2,2 [$CE9E] Pointer to stretchy_graphic_part
B $CE94,1,1 ?index
W $CE95,2,2 [$CEB4] Pointer to stretchy_graphic_part
B $CE97,1,1 ?index
W $CE98,2,2 [$CECA] Pointer to stretchy_graphic_part
B $CE9A,1,1 ?index
W $CE9B,2,2 [$CEE0] Pointer to stretchy_graphic_part
B $CE9D,1,1 Terminator
N $CE9E Stretchy graphic part
W $CE9E,2,2 [$CF5B] LOD ptr
W $CEA0,2,2 TBD
W $CEA2,2,2 TBD
W $CEA4,2,2 TBD
W $CEA6,2,2 TBD
W $CEA8,2,2 TBD
W $CEAA,2,2 TBD
W $CEAC,2,2 TBD
W $CEAE,2,2 TBD
W $CEB0,2,2 TBD
W $CEB2,2,2 TBD
N $CEB4 Stretchy graphic part
W $CEB4,2,2 [$CF5B] LOD ptr
W $CEB6,2,2 TBD
W $CEB8,2,2 TBD
W $CEBA,2,2 TBD
W $CEBC,2,2 TBD
W $CEBE,2,2 TBD
W $CEC0,2,2 TBD
W $CEC2,2,2 TBD
W $CEC4,2,2 TBD
W $CEC6,2,2 TBD
W $CEC8,2,2 TBD
N $CECA Stretchy graphic part
W $CECA,2,2 [$CF5B] LOD ptr
W $CECC,2,2 TBD
W $CECE,2,2 TBD
W $CED0,2,2 TBD
W $CED2,2,2 TBD
W $CED4,2,2 TBD
W $CED6,2,2 TBD
W $CED8,2,2 TBD
W $CEDA,2,2 TBD
W $CEDC,2,2 TBD
W $CEDE,2,2 TBD
N $CEE0 Stretchy graphic part
W $CEE0,2,2 [$CF5B] LOD ptr
W $CEE2,2,2 TBD
W $CEE4,2,2 TBD
W $CEE6,2,2 TBD
W $CEE8,2,2 TBD
W $CEEA,2,2 TBD
W $CEEC,2,2 TBD
W $CEEE,2,2 TBD
W $CEF0,2,2 TBD
W $CEF2,2,2 TBD
W $CEF4,2,2 TBD
N $CEF6 Stretchy graphic
B $CEF6,1,1 ?index
W $CEF7,2,2 [$CF03] Pointer to stretchy_graphic_part
B $CEF9,1,1 ?index
W $CEFA,2,2 [$CF19] Pointer to stretchy_graphic_part
B $CEFC,1,1 ?index
W $CEFD,2,2 [$CF2F] Pointer to stretchy_graphic_part
B $CEFF,1,1 ?index
W $CF00,2,2 [$CF45] Pointer to stretchy_graphic_part
B $CF02,1,1 Terminator
N $CF03 Stretchy graphic part
W $CF03,2,2 [$CFE7] LOD ptr
W $CF05,2,2 TBD
W $CF07,2,2 TBD
W $CF09,2,2 TBD
W $CF0B,2,2 TBD
W $CF0D,2,2 TBD
W $CF0F,2,2 TBD
W $CF11,2,2 TBD
W $CF13,2,2 TBD
W $CF15,2,2 TBD
W $CF17,2,2 TBD
N $CF19 Stretchy graphic part
W $CF19,2,2 [$CFE7] LOD ptr
W $CF1B,2,2 TBD
W $CF1D,2,2 TBD
W $CF1F,2,2 TBD
W $CF21,2,2 TBD
W $CF23,2,2 TBD
W $CF25,2,2 TBD
W $CF27,2,2 TBD
W $CF29,2,2 TBD
W $CF2B,2,2 TBD
W $CF2D,2,2 TBD
N $CF2F Stretchy graphic part
W $CF2F,2,2 [$CFE7] LOD ptr
W $CF31,2,2 TBD
W $CF33,2,2 TBD
W $CF35,2,2 TBD
W $CF37,2,2 TBD
W $CF39,2,2 TBD
W $CF3B,2,2 TBD
W $CF3D,2,2 TBD
W $CF3F,2,2 TBD
W $CF41,2,2 TBD
W $CF43,2,2 TBD
N $CF45 Stretchy graphic part
W $CF45,2,2 [$CFE7] LOD ptr
W $CF47,2,2 TBD
W $CF49,2,2 TBD
W $CF4B,2,2 TBD
W $CF4D,2,2 TBD
W $CF4F,2,2 TBD
W $CF51,2,2 TBD
W $CF53,2,2 TBD
W $CF55,2,2 TBD
W $CF57,2,2 TBD
W $CF59,2,2 TBD
N $CF5B LOD table for "stretchy"
N $CF5B LOD
B $CF5B,1,1 Width (bytes)
B $CF5C,1,1 Flags
B $CF5D,1,1 Height (pixels)
W $CF5E,2,2 [$D073] Bitmap address
W $CF60,2,2 [$D073] Pre-shifted bitmap address
N $CF62 LOD
B $CF62,1,1 Width (bytes)
B $CF63,1,1 Flags
B $CF64,1,1 Height (pixels)
W $CF65,2,2 [$D09B] Bitmap address
W $CF67,2,2 [$D09B] Pre-shifted bitmap address
N $CF69 LOD
B $CF69,1,1 Width (bytes)
B $CF6A,1,1 Flags
B $CF6B,1,1 Height (pixels)
W $CF6C,2,2 [$D1F0] Bitmap address
W $CF6E,2,2 [$D1F0] Pre-shifted bitmap address
N $CF70 LOD
B $CF70,1,1 Width (bytes)
B $CF71,1,1 Flags
B $CF72,1,1 Height (pixels)
W $CF73,2,2 [$D10B] Bitmap address
W $CF75,2,2 [$D10B] Pre-shifted bitmap address
N $CF77 LOD
B $CF77,1,1 Width (bytes)
B $CF78,1,1 Flags
B $CF79,1,1 Height (pixels)
W $CF7A,2,2 [$D14B] Bitmap address
W $CF7C,2,2 [$D14B] Pre-shifted bitmap address
B $CF7E,105,8*13,1
N $CFE7 LOD table for "stretchy"
N $CFE7 LOD
B $CFE7,1,1 Width (bytes)
B $CFE8,1,1 Flags
B $CFE9,1,1 Height (pixels)
W $CFEA,2,2 [$D073] Bitmap address
W $CFEC,2,2 [$D073] Pre-shifted bitmap address
N $CFEE LOD
B $CFEE,1,1 Width (bytes)
B $CFEF,1,1 Flags
B $CFF0,1,1 Height (pixels)
W $CFF1,2,2 [$D09B] Bitmap address
W $CFF3,2,2 [$D09B] Pre-shifted bitmap address
N $CFF5 LOD
B $CFF5,1,1 Width (bytes)
B $CFF6,1,1 Flags
B $CFF7,1,1 Height (pixels)
W $CFF8,2,2 [$D0D3] Bitmap address
W $CFFA,2,2 [$D0D3] Pre-shifted bitmap address
N $CFFC LOD
B $CFFC,1,1 Width (bytes)
B $CFFD,1,1 Flags
B $CFFE,1,1 Height (pixels)
W $CFFF,2,2 [$D10B] Bitmap address
W $D001,2,2 [$D10B] Pre-shifted bitmap address
N $D003 LOD
B $D003,1,1 Width (bytes)
B $D004,1,1 Flags
B $D005,1,1 Height (pixels)
W $D006,2,2 [$D14B] Bitmap address
W $D008,2,2 [$D14B] Pre-shifted bitmap address
B $D00A,105,8*13,1
B $D073,40,5 Bitmap data 5 bytes x 8
B $D09B,56,7 Bitmap data 7 bytes x 8
B $D0D3,56,7 Bitmap data 7 bytes x 8
B $D10B,64,8 Bitmap data 8 bytes x 8
B $D14B,12,3 Bitmap data 3 bytes x 4
B $D157,153,8*19,1
B $D1F0,56,7 Bitmap data 7 bytes x 8
B $D228,100,8*12,4
N $D28C Stretchy graphic
B $D28C,1,1 ?index
W $D28D,2,2 [$D293] Pointer to stretchy_graphic_part
B $D28F,1,1 ?index
W $D290,2,2 [$D2A9] Pointer to stretchy_graphic_part
B $D292,1,1 Terminator
N $D293 Stretchy graphic part
W $D293,2,2 [$D2BF] LOD ptr
W $D295,2,2 TBD
W $D297,2,2 TBD
W $D299,2,2 TBD
W $D29B,2,2 TBD
W $D29D,2,2 TBD
W $D29F,2,2 TBD
W $D2A1,2,2 TBD
W $D2A3,2,2 TBD
W $D2A5,2,2 TBD
W $D2A7,2,2 TBD
N $D2A9 Stretchy graphic part
W $D2A9,2,2 [$D2BF] LOD ptr
W $D2AB,2,2 TBD
W $D2AD,2,2 TBD
W $D2AF,2,2 TBD
W $D2B1,2,2 TBD
W $D2B3,2,2 TBD
W $D2B5,2,2 TBD
W $D2B7,2,2 TBD
W $D2B9,2,2 TBD
W $D2BB,2,2 TBD
W $D2BD,2,2 TBD
N $D2BF LOD table for "stretchy"
N $D2BF LOD
B $D2BF,1,1 Width (bytes)
B $D2C0,1,1 Flags
B $D2C1,1,1 Height (pixels)
W $D2C2,2,2 [$D438] Bitmap address
W $D2C4,2,2 [$D438] Pre-shifted bitmap address
N $D2C6 LOD
B $D2C6,1,1 Width (bytes)
B $D2C7,1,1 Flags
B $D2C8,1,1 Height (pixels)
W $D2C9,2,2 [$D547] Bitmap address
W $D2CB,2,2 [$D547] Pre-shifted bitmap address
N $D2CD LOD
B $D2CD,1,1 Width (bytes)
B $D2CE,1,1 Flags
B $D2CF,1,1 Height (pixels)
W $D2D0,2,2 [$D5D5] Bitmap address
W $D2D2,2,2 [$D5D5] Pre-shifted bitmap address
N $D2D4 LOD
B $D2D4,1,1 Width (bytes)
B $D2D5,1,1 Flags
B $D2D6,1,1 Height (pixels)
W $D2D7,2,2 [$D60E] Bitmap address
W $D2D9,2,2 [$D60E] Pre-shifted bitmap address
N $D2DB LOD
B $D2DB,1,1 Width (bytes)
B $D2DC,1,1 Flags
B $D2DD,1,1 Height (pixels)
W $D2DE,2,2 [$D478] Bitmap address
W $D2E0,2,2 [$D478] Pre-shifted bitmap address
N $D2E2 LOD
B $D2E2,1,1 Width (bytes)
B $D2E3,1,1 Flags
B $D2E4,1,1 Height (pixels)
W $D2E5,2,2 [$D56B] Bitmap address
W $D2E7,2,2 [$D56B] Pre-shifted bitmap address
N $D2E9 LOD
B $D2E9,1,1 Width (bytes)
B $D2EA,1,1 Flags
B $D2EB,1,1 Height (pixels)
W $D2EC,2,2 [$D5E5] Bitmap address
W $D2EE,2,2 [$D5E5] Pre-shifted bitmap address
N $D2F0 LOD
B $D2F0,1,1 Width (bytes)
B $D2F1,1,1 Flags
B $D2F2,1,1 Height (pixels)
W $D2F3,2,2 [$D612] Bitmap address
W $D2F5,2,2 [$D612] Pre-shifted bitmap address
N $D2F7 Stretchy graphic
B $D2F7,1,1 ?index
W $D2F8,2,2 [$D2FE] Pointer to stretchy_graphic_part
B $D2FA,1,1 ?index
W $D2FB,2,2 [$D332] Pointer to stretchy_graphic_part
B $D2FD,1,1 Terminator
N $D2FE Stretchy graphic part
W $D2FE,2,2 [$D348] LOD ptr
W $D300,2,2 TBD
W $D302,2,2 TBD
W $D304,2,2 TBD
W $D306,2,2 TBD
W $D308,2,2 TBD
W $D30A,2,2 TBD
W $D30C,2,2 TBD
W $D30E,2,2 TBD
W $D310,2,2 TBD
W $D312,2,2 TBD
N $D314 Bitmap data 3 bytes x 10
B $D314,30,8*3,6
N $D332 Stretchy graphic part
W $D332,2,2 [$D348] LOD ptr
W $D334,2,2 TBD
W $D336,2,2 TBD
W $D338,2,2 TBD
W $D33A,2,2 TBD
W $D33C,2,2 TBD
W $D33E,2,2 TBD
W $D340,2,2 TBD
W $D342,2,2 TBD
W $D344,2,2 TBD
W $D346,2,2 TBD
N $D348 LOD table for "stretchy"
N $D348 LOD
B $D348,1,1 Width (bytes)
B $D349,1,1 Flags
B $D34A,1,1 Height (pixels)
W $D34B,2,2 [$D380] Bitmap address
W $D34D,2,2 [$D380] Pre-shifted bitmap address
N $D34F LOD
B $D34F,1,1 Width (bytes)
B $D350,1,1 Flags
B $D351,1,1 Height (pixels)
W $D352,2,2 [$D4D8] Bitmap address
W $D354,2,2 [$D4D8] Pre-shifted bitmap address
N $D356 LOD
B $D356,1,1 Width (bytes)
B $D357,1,1 Flags
B $D358,1,1 Height (pixels)
W $D359,2,2 [$D5A1] Bitmap address
W $D35B,2,2 [$D5A1] Pre-shifted bitmap address
N $D35D LOD
B $D35D,1,1 Width (bytes)
B $D35E,1,1 Flags
B $D35F,1,1 Height (pixels)
W $D360,2,2 [$D5FD] Bitmap address
W $D362,2,2 [$D5FD] Pre-shifted bitmap address
N $D364 LOD
B $D364,1,1 Width (bytes)
B $D365,1,1 Flags
B $D366,1,1 Height (pixels)
W $D367,2,2 [$D3C0] Bitmap address
W $D369,2,2 [$D3C0] Pre-shifted bitmap address
N $D36B LOD
B $D36B,1,1 Width (bytes)
B $D36C,1,1 Flags
B $D36D,1,1 Height (pixels)
W $D36E,2,2 [$D4FC] Bitmap address
W $D370,2,2 [$D4FC] Pre-shifted bitmap address
N $D372 LOD
B $D372,1,1 Width (bytes)
B $D373,1,1 Flags
B $D374,1,1 Height (pixels)
W $D375,2,2 [$D5B1] Bitmap address
W $D377,2,2 [$D5B1] Pre-shifted bitmap address
N $D379 LOD
B $D379,1,1 Width (bytes)
B $D37A,1,1 Flags
B $D37B,1,1 Height (pixels)
W $D37C,2,2 [$D601] Bitmap address
W $D37E,2,2 [$D601] Pre-shifted bitmap address
B $D380,64,4 Bitmap data 4 bytes x 16
B $D3C0,96,4 Bitmap data 4 bytes x 24
B $D420,24,8
B $D438,64,4 Bitmap data 4 bytes x 16
B $D478,96,4 Bitmap data 4 bytes x 24
B $D4D8,111,8*13,7
B $D547,36,3 Bitmap data 3 bytes x 12
B $D56B,106,8*13,2
B $D5D5,16,2 Bitmap data 2 bytes x 8
B $D5E5,41,8*5,1
B $D60E,4,1 Bitmap data 1 bytes x 4
B $D612,14,8,6
N $D620 LOD table for "hittable hazard"
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
B $D6C7,28,4 Bitmap data (masked) 4 bytes x 7
B $D6E3,28,4 Pre-shifted bitmap data (masked) 4 bytes x 7
N $D6FF Stretchy graphic
B $D6FF,1,1 ?index
W $D700,2,2 [$D719] Pointer to stretchy_graphic_part
B $D702,1,1 ?index
W $D703,2,2 [$D72F] Pointer to stretchy_graphic_part
B $D705,1,1 ?index
W $D706,2,2 [$D75B] Pointer to stretchy_graphic_part
B $D708,1,1 ?index
W $D709,2,2 [$D787] Pointer to stretchy_graphic_part
B $D70B,1,1 Terminator
N $D70C Stretchy graphic
B $D70C,1,1 ?index
W $D70D,2,2 [$D719] Pointer to stretchy_graphic_part
B $D70F,1,1 ?index
W $D710,2,2 [$D745] Pointer to stretchy_graphic_part
B $D712,1,1 ?index
W $D713,2,2 [$D771] Pointer to stretchy_graphic_part
B $D715,1,1 ?index
W $D716,2,2 [$D79D] Pointer to stretchy_graphic_part
B $D718,1,1 Terminator
N $D719 Stretchy graphic part
W $D719,2,2 [$D7B3] LOD ptr
W $D71B,2,2 TBD
W $D71D,2,2 TBD
W $D71F,2,2 TBD
W $D721,2,2 TBD
W $D723,2,2 TBD
W $D725,2,2 TBD
W $D727,2,2 TBD
W $D729,2,2 TBD
W $D72B,2,2 TBD
W $D72D,2,2 TBD
N $D72F Stretchy graphic part
W $D72F,2,2 [$D7B3] LOD ptr
W $D731,2,2 TBD
W $D733,2,2 TBD
W $D735,2,2 TBD
W $D737,2,2 TBD
W $D739,2,2 TBD
W $D73B,2,2 TBD
W $D73D,2,2 TBD
W $D73F,2,2 TBD
W $D741,2,2 TBD
W $D743,2,2 TBD
N $D745 Stretchy graphic part
W $D745,2,2 [$D7B3] LOD ptr
W $D747,2,2 TBD
W $D749,2,2 TBD
W $D74B,2,2 TBD
W $D74D,2,2 TBD
W $D74F,2,2 TBD
W $D751,2,2 TBD
W $D753,2,2 TBD
W $D755,2,2 TBD
W $D757,2,2 TBD
W $D759,2,2 TBD
N $D75B Stretchy graphic part
W $D75B,2,2 [$D7B3] LOD ptr
W $D75D,2,2 TBD
W $D75F,2,2 TBD
W $D761,2,2 TBD
W $D763,2,2 TBD
W $D765,2,2 TBD
W $D767,2,2 TBD
W $D769,2,2 TBD
W $D76B,2,2 TBD
W $D76D,2,2 TBD
W $D76F,2,2 TBD
N $D771 Stretchy graphic part
W $D771,2,2 [$D7B3] LOD ptr
W $D773,2,2 TBD
W $D775,2,2 TBD
W $D777,2,2 TBD
W $D779,2,2 TBD
W $D77B,2,2 TBD
W $D77D,2,2 TBD
W $D77F,2,2 TBD
W $D781,2,2 TBD
W $D783,2,2 TBD
W $D785,2,2 TBD
N $D787 Stretchy graphic part
W $D787,2,2 [$D7B3] LOD ptr
W $D789,2,2 TBD
W $D78B,2,2 TBD
W $D78D,2,2 TBD
W $D78F,2,2 TBD
W $D791,2,2 TBD
W $D793,2,2 TBD
W $D795,2,2 TBD
W $D797,2,2 TBD
W $D799,2,2 TBD
W $D79B,2,2 TBD
N $D79D Stretchy graphic part
W $D79D,2,2 [$D7B3] LOD ptr
W $D79F,2,2 TBD
W $D7A1,2,2 TBD
W $D7A3,2,2 TBD
W $D7A5,2,2 TBD
W $D7A7,2,2 TBD
W $D7A9,2,2 TBD
W $D7AB,2,2 TBD
W $D7AD,2,2 TBD
W $D7AF,2,2 TBD
W $D7B1,2,2 TBD
N $D7B3 LOD table for "stretchy"
N $D7B3 LOD
B $D7B3,1,1 Width (bytes)
B $D7B4,1,1 Flags
B $D7B5,1,1 Height (pixels)
W $D7B6,2,2 [$D81C] Bitmap address
W $D7B8,2,2 [$D81C] Pre-shifted bitmap address
N $D7BA LOD
B $D7BA,1,1 Width (bytes)
B $D7BB,1,1 Flags
B $D7BC,1,1 Height (pixels)
W $D7BD,2,2 [$D828] Bitmap address
W $D7BF,2,2 [$D828] Pre-shifted bitmap address
N $D7C1 LOD
B $D7C1,1,1 Width (bytes)
B $D7C2,1,1 Flags
B $D7C3,1,1 Height (pixels)
W $D7C4,2,2 [$D84F] Bitmap address
W $D7C6,2,2 [$D84F] Pre-shifted bitmap address
N $D7C8 LOD
B $D7C8,1,1 Width (bytes)
B $D7C9,1,1 Flags
B $D7CA,1,1 Height (pixels)
W $D7CB,2,2 [$D856] Bitmap address
W $D7CD,2,2 [$D856] Pre-shifted bitmap address
N $D7CF LOD
B $D7CF,1,1 Width (bytes)
B $D7D0,1,1 Flags
B $D7D1,1,1 Height (pixels)
W $D7D2,2,2 [$D85C] Bitmap address
W $D7D4,2,2 [$D85C] Pre-shifted bitmap address
B $D7D6,70,8*8,6
B $D81C,12,3 Bitmap data 3 bytes x 4
B $D828,39,3 Bitmap data 3 bytes x 13
B $D84F,7,1 Bitmap data 1 bytes x 7
B $D856,6,2 Bitmap data 2 bytes x 3
B $D85C,2,1 Bitmap data 1 bytes x 2
B $D85E,116,8*14,4
N $D8D2 Stretchy graphic
B $D8D2,1,1 ?index
W $D8D3,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D8D5,1,1 ?index
W $D8D6,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D8D8,1,1 ?index
W $D8D9,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D8DB,1,1 ?index
W $D8DC,2,2 [$D8EC] Pointer to stretchy_graphic_part
B $D8DE,1,1 Terminator
N $D8DF Stretchy graphic
B $D8DF,1,1 ?index
W $D8E0,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D8E2,1,1 ?index
W $D8E3,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D8E5,1,1 ?index
W $D8E6,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D8E8,1,1 ?index
W $D8E9,2,2 [$D902] Pointer to stretchy_graphic_part
B $D8EB,1,1 Terminator
N $D8EC Stretchy graphic part
W $D8EC,2,2 [$D918] LOD ptr
W $D8EE,2,2 TBD
W $D8F0,2,2 TBD
W $D8F2,2,2 TBD
W $D8F4,2,2 TBD
W $D8F6,2,2 TBD
W $D8F8,2,2 TBD
W $D8FA,2,2 TBD
W $D8FC,2,2 TBD
W $D8FE,2,2 TBD
W $D900,2,2 TBD
N $D902 Stretchy graphic part
W $D902,2,2 [$D918] LOD ptr
W $D904,2,2 TBD
W $D906,2,2 TBD
W $D908,2,2 TBD
W $D90A,2,2 TBD
W $D90C,2,2 TBD
W $D90E,2,2 TBD
W $D910,2,2 TBD
W $D912,2,2 TBD
W $D914,2,2 TBD
W $D916,2,2 TBD
N $D918 LOD table for "stretchy"
N $D918 LOD
B $D918,1,1 Width (bytes)
B $D919,1,1 Flags
B $D91A,1,1 Height (pixels)
W $D91B,2,2 [$D93B] Bitmap address
W $D91D,2,2 [$D93B] Pre-shifted bitmap address
N $D91F LOD
B $D91F,1,1 Width (bytes)
B $D920,1,1 Flags
B $D921,1,1 Height (pixels)
W $D922,2,2 [$D963] Bitmap address
W $D924,2,2 [$D963] Pre-shifted bitmap address
N $D926 LOD
B $D926,1,1 Width (bytes)
B $D927,1,1 Flags
B $D928,1,1 Height (pixels)
W $D929,2,2 [$D97B] Bitmap address
W $D92B,2,2 [$D987] Pre-shifted bitmap address
N $D92D LOD
B $D92D,1,1 Width (bytes)
B $D92E,1,1 Flags
B $D92F,1,1 Height (pixels)
W $D930,2,2 [$D993] Bitmap address
W $D932,2,2 [$D9A5] Pre-shifted bitmap address
N $D934 LOD
B $D934,1,1 Width (bytes)
B $D935,1,1 Flags
B $D936,1,1 Height (pixels)
W $D937,2,2 [$D9B7] Bitmap address
W $D939,2,2 [$D9C3] Pre-shifted bitmap address
B $D93B,40,5 Bitmap data 5 bytes x 8
B $D963,24,4 Bitmap data 4 bytes x 6
B $D97B,12,3 Bitmap data 3 bytes x 4
B $D987,12,3 Pre-shifted bitmap data 3 bytes x 4
B $D993,18,6 Bitmap data (masked) 6 bytes x 3
B $D9A5,18,6 Pre-shifted bitmap data (masked) 6 bytes x 3
B $D9B7,8,4 Bitmap data (masked) 4 bytes x 2
B $D9BF,4,4
B $D9C3,8,4 Pre-shifted bitmap data (masked) 4 bytes x 2
B $D9CB,4,4
N $D9CF Stretchy graphic
B $D9CF,1,1 ?index
W $D9D0,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D9D2,1,1 ?index
W $D9D3,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D9D5,1,1 ?index
W $D9D6,2,2 [$D9F9] Pointer to stretchy_graphic_part
B $D9D8,1,1 Terminator
N $D9D9 Stretchy graphic
B $D9D9,1,1 ?index
W $D9DA,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D9DC,1,1 ?index
W $D9DD,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D9DF,1,1 ?index
W $D9E0,2,2 [$D9E3] Pointer to stretchy_graphic_part
B $D9E2,1,1 Terminator
N $D9E3 Stretchy graphic part
W $D9E3,2,2 [$DA0F] LOD ptr
W $D9E5,2,2 TBD
W $D9E7,2,2 TBD
W $D9E9,2,2 TBD
W $D9EB,2,2 TBD
W $D9ED,2,2 TBD
W $D9EF,2,2 TBD
W $D9F1,2,2 TBD
W $D9F3,2,2 TBD
W $D9F5,2,2 TBD
W $D9F7,2,2 TBD
N $D9F9 Stretchy graphic part
W $D9F9,2,2 [$DA0F] LOD ptr
W $D9FB,2,2 TBD
W $D9FD,2,2 TBD
W $D9FF,2,2 TBD
W $DA01,2,2 TBD
W $DA03,2,2 TBD
W $DA05,2,2 TBD
W $DA07,2,2 TBD
W $DA09,2,2 TBD
W $DA0B,2,2 TBD
W $DA0D,2,2 TBD
N $DA0F LOD table for "stretchy"
N $DA0F LOD
B $DA0F,1,1 Width (bytes)
B $DA10,1,1 Flags
B $DA11,1,1 Height (pixels)
W $DA12,2,2 [$DA32] Bitmap address
W $DA14,2,2 [$DA32] Pre-shifted bitmap address
N $DA16 LOD
B $DA16,1,1 Width (bytes)
B $DA17,1,1 Flags
B $DA18,1,1 Height (pixels)
W $DA19,2,2 [$DA59] Bitmap address
W $DA1B,2,2 [$DA59] Pre-shifted bitmap address
N $DA1D LOD
B $DA1D,1,1 Width (bytes)
B $DA1E,1,1 Flags
B $DA1F,1,1 Height (pixels)
W $DA20,2,2 [$DA77] Bitmap address
W $DA22,2,2 [$DA8C] Pre-shifted bitmap address
N $DA24 LOD
B $DA24,1,1 Width (bytes)
B $DA25,1,1 Flags
B $DA26,1,1 Height (pixels)
W $DA27,2,2 [$DAA1] Bitmap address
W $DA29,2,2 [$DAB5] Pre-shifted bitmap address
N $DA2B LOD
B $DA2B,1,1 Width (bytes)
B $DA2C,1,1 Flags
B $DA2D,1,1 Height (pixels)
W $DA2E,2,2 [$DAC9] Bitmap address
W $DA30,2,2 [$DAD9] Pre-shifted bitmap address
B $DA32,39,3 Bitmap data 3 bytes x 13
B $DA59,30,3 Bitmap data 3 bytes x 10
B $DA77,21,3 Bitmap data 3 bytes x 7
B $DA8C,21,3 Pre-shifted bitmap data 3 bytes x 7
B $DAA1,20,4 Bitmap data (masked) 4 bytes x 5
B $DAB5,20,4 Pre-shifted bitmap data (masked) 4 bytes x 5
B $DAC9,16,4 Bitmap data (masked) 4 bytes x 4
B $DAD9,16,4 Pre-shifted bitmap data (masked) 4 bytes x 4
B $DAE9,9495,8*1186,7
