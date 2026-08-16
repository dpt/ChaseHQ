b $C000 [Stage 5] Horizon graphic
D $C000 The stage's skyline: 10 bytes wide by 24 rows, 240 bytes in all. It arrives at $5C00 with the rest of the per-stage data, and pre_shift_backdrop makes a copy at $5B00 rotated right by one nibble. Bit 0 of the horizontal scroll then picks between the two in draw_road, so the four pixel shift comes free.
N $C000 #HTML[#CALL(graphic($C000,80,24,0,1))]
@ $C000 label=stage5_backdrop
B $C000,240,8
b $C0F0 [Stage 5] Per-stage data
@ $C0F0 label=stage5
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
B $C11B,1,1 Perp lane-change base delay: added to (rng() & 31) to reset the lane-change timer
B $C11C,1,1 Perp approach base delay: added to (rng() & 15) to reset the approach timer
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
@ $C139 label=stage5_perp_description
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
D $C1E6 An overlay message list: a leading delay byte, then one block per line made of a delay, a character style (2 = single height, 3 = double), an attribute, a back buffer address and an attribute address, followed by the text with bit 7 set on its last character. The text is drawn black on black and revealed by the attribute transition, which is why every attribute byte here is zero. A delay followed by $00 ends the list.
B $C1E6,1,1 Frame delay until first message
B $C1E7,1,1 Frame delay until next message
B $C1E8,1,1 Character style (single height)
B $C1E9,1,1 Attribute
W $C1EA,2,2 Back buffer address
W $C1EC,2,2 Attribute address
T $C1EE,27,26:n1 "OK! YOU ARE UNDER ARREST ON"
B $C209,1,1 Frame delay until next message
B $C20A,1,1 Character style (single height)
B $C20B,1,1 Attribute
W $C20C,2,2 Back buffer address
W $C20E,2,2 Attribute address
T $C210,26,25:n1 "SUSPICION OF ESPIONAGE AND"
B $C22A,1,1 Frame delay until next message
B $C22B,1,1 Character style (single height)
B $C22C,1,1 Attribute
W $C22D,2,2 Back buffer address
W $C22F,2,2 Attribute address
T $C231,6,5:n1 "MURDER"
B $C237,1,1 Frame delay until next message
B $C238,1,1 Stop
b $C239 [Stage 5] Hittable hazards
B $C239,1,1 Collision width
W $C23A,2,2 [$D620] Address of LODs
B $C23C,1,1 Collision width
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
b $C2A1 [Stage 5] Map curvature data (start section)
@ $C2A1 label=stage5_map_start_curvature
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
b $C2C9 [Stage 5] Map height data (start section)
@ $C2C9 label=stage5_map_start_height
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
b $C2FB [Stage 5] Map lanes data (start section)
@ $C2FB label=stage5_map_start_lanes
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
b $C30D [Stage 5] Map hazards data (start section)
@ $C30D label=stage5_map_start_hazards
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
b $C32C [Stage 5] Map left object data (start section)
@ $C32C label=stage5_map_start_leftobjs
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
b $C3D2 [Stage 5] Map right object data (start section)
@ $C3D2 label=stage5_map_start_rightobjs
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
b $C455 [Stage 5] Map curvature data (right section)
@ $C455 label=stage5_map_right_curvature
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
b $C468 [Stage 5] Map height data (right section)
@ $C468 label=stage5_map_right_height
B $C468,1,1 Level Road for 198 units
B $C469,13,8,5
B $C476,1,1 <Esc> Jump
B $C477,1,1
W $C478,2,2 [$C5F8] Target
b $C47A [Stage 5] Map lanes data (right section)
@ $C47A label=stage5_map_right_lanes
B $C47A,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C47B,1,1
B $C47C,1,1 Tunnel start                {45} for 20 units
B $C47D,1,1
B $C47E,1,1 Tunnel exit                 {59} for 2 units
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
b $C492 [Stage 5] Map hazards data (right section)
@ $C492 label=stage5_map_right_hazards
B $C492,1,1 Wait for 10 units
B $C493,1,1 Enable Car Spawning
B $C494,1,1
B $C495,1,1 Wait for 89 units
B $C496,1,1 <Esc> Jump
B $C497,1,1
W $C498,2,2 [$C628] Target
b $C49A [Stage 5] Map left object data (right section)
@ $C49A label=stage5_map_right_leftobjs
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
b $C4DD [Stage 5] Map right object data (right section)
@ $C4DD label=stage5_map_right_rightobjs
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
b $C513 [Stage 5] Map curvature data (left section)
@ $C513 label=stage5_map_left_curvature
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
b $C526 [Stage 5] Map height data (left section)
@ $C526 label=stage5_map_left_height
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
b $C53E [Stage 5] Map lanes data (left section)
@ $C53E label=stage5_map_left_lanes
B $C53E,1,1 4 Lanes              [||||] {00} for 40 units
B $C53F,1,1
B $C540,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C541,1,1
B $C542,1,1 3 Lanes L            [|||]  {81} for 124 units
B $C543,1,1
B $C544,1,1 Tunnel start                {45} for 20 units
B $C545,1,1
B $C546,1,1 Tunnel exit                 {59} for 2 units
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
b $C552 [Stage 5] Map hazards data (left section)
@ $C552 label=stage5_map_left_hazards
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
b $C560 [Stage 5] Map left object data (left section)
@ $C560 label=stage5_map_left_leftobjs
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
b $C5A3 [Stage 5] Map right object data (left section)
@ $C5A3 label=stage5_map_left_rightobjs
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
b $C5E3 [Stage 5] Map curvature data (merge section)
@ $C5E3 label=stage5_map_merge_curvature
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
b $C5F8 [Stage 5] Map height data (merge section)
@ $C5F8 label=stage5_map_merge_height
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
b $C614 [Stage 5] Map lanes data (merge section)
@ $C614 label=stage5_map_merge_lanes
B $C614,1,1 4 Lanes              [||||] {00} for 44 units
B $C615,1,1
B $C616,1,1 4-3 Narrowing L      [|||\] {BD} for 2 units
B $C617,1,1
B $C618,1,1 3 Lanes L            [|||]  {81} for 12 units
B $C619,1,1
B $C61A,1,1 Tunnel start                {45} for 102 units
B $C61B,1,1
B $C61C,1,1 Tunnel exit                 {59} for 2 units
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
b $C628 [Stage 5] Map hazards data (merge section)
@ $C628 label=stage5_map_merge_hazards
B $C628,1,1 Wait for 93 units
B $C629,1,1 <Esc> Jump
B $C62A,1,1
W $C62B,2,2 [$C6D1] Target
b $C62D [Stage 5] Map left object data (merge section)
@ $C62D label=stage5_map_merge_leftobjs
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
b $C651 [Stage 5] Map right object data (merge section)
@ $C651 label=stage5_map_merge_rightobjs
B $C651,1,1 EMPTY for 93 units
B $C652,6,6
B $C658,1,1 <Esc> Jump
B $C659,1,1
W $C65A,2,2 [$C7A5] Target
b $C65C [Stage 5] Map curvature data (loop section)
@ $C65C label=stage5_map_loop_curvature
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
b $C684 [Stage 5] Map height data (loop section)
@ $C684 label=stage5_map_loop_height
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
b $C6BB [Stage 5] Map lanes data (loop section)
@ $C6BB label=stage5_map_loop_lanes
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
b $C6D1 [Stage 5] Map hazards data (loop section)
@ $C6D1 label=stage5_map_loop_hazards
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
b $C6EE [Stage 5] Map left object data (loop section)
@ $C6EE label=stage5_map_loop_leftobjs
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
b $C7A5 [Stage 5] Map right object data (loop section)
@ $C7A5 label=stage5_map_loop_rightobjs
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
N $C82E #HTML[#CALL(face($C82E))]
@ $C82E label=stage5_perp_face
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
N $C960 #HTML[#CALL(graphic($C960,48,30,0,1))]
B $C960,180,6 Bitmap data 6 bytes x 30
N $CA14 #HTML[#CALL(graphic($CA14,40,22,0,1))]
B $CA14,110,5 Bitmap data 5 bytes x 22
N $CA82 #HTML[#CALL(graphic($CA82,24,15,0,1))]
B $CA82,45,3 Bitmap data 3 bytes x 15
N $CAAF #HTML[#CALL(graphic($CAAF,24,8,1,1))]
B $CAAF,48,6 Bitmap data (masked) 6 bytes x 8
N $CADF #HTML[#CALL(graphic($CADF,24,8,1,1))]
B $CADF,48,6 Pre-shifted bitmap data (masked) 6 bytes x 8
N $CB0F #HTML[#CALL(graphic($CB0F,48,39,0,1))]
B $CB0F,234,6 Bitmap data 6 bytes x 39
N $CBF9 #HTML[#CALL(graphic($CBF9,40,29,0,1))]
B $CBF9,145,5 Bitmap data 5 bytes x 29
N $CC8A #HTML[#CALL(graphic($CC8A,24,20,0,1))]
B $CC8A,60,3 Bitmap data 3 bytes x 20
N $CCC6 #HTML[#CALL(graphic($CCC6,16,12,1,1))]
B $CCC6,48,4 Bitmap data (masked) 4 bytes x 12
N $CCF6 #HTML[#CALL(graphic($CCF6,16,12,1,1))]
B $CCF6,48,4 Pre-shifted bitmap data (masked) 4 bytes x 12
N $CD26 #HTML[#CALL(graphic($CD26,48,30,0,1))]
B $CD26,180,6 Bitmap data 6 bytes x 30
N $CDDA #HTML[#CALL(graphic($CDDA,32,20,0,1))]
B $CDDA,80,4 Bitmap data 4 bytes x 20
N $CE2A #HTML[#CALL(graphic($CE2A,24,13,0,1))]
B $CE2A,39,3 Bitmap data 3 bytes x 13
N $CE51 #HTML[#CALL(graphic($CE51,16,8,1,1))]
B $CE51,32,4 Bitmap data (masked) 4 bytes x 8
N $CE71 #HTML[#CALL(graphic($CE71,16,8,1,1))]
B $CE71,32,4 Pre-shifted bitmap data (masked) 4 bytes x 8
N $CE91 Stretchy graphic
B $CE91,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $CE92,2,2 [$CE9E] Pointer to stretchy_graphic_part
B $CE94,1,1 Height 112.5% of the perspective scale
W $CE95,2,2 [$CEB4] Pointer to stretchy_graphic_part
B $CE97,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $CE98,2,2 [$CECA] Pointer to stretchy_graphic_part
B $CE9A,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $CE9B,2,2 [$CEE0] Pointer to stretchy_graphic_part
B $CE9D,1,1 Terminator
N $CE9E Stretchy graphic part
W $CE9E,2,2 [$CF5B] LOD ptr
W $CEA0,2,2 Depth $00, bitmap at +$17
W $CEA2,2,2 Depth $00, bitmap at +$17
W $CEA4,2,2 Depth $00, bitmap at +$33
W $CEA6,2,2 Depth $00, bitmap at +$33
W $CEA8,2,2 Depth $00, bitmap at +$4F
W $CEAA,2,2 Depth $00, bitmap at +$4F
W $CEAC,2,2 Depth $F8, bitmap at +$6B
W $CEAE,2,2 Depth $F8, bitmap at +$6B
W $CEB0,2,2 Depth $F8, bitmap at +$87
W $CEB2,2,2 Depth $F8, bitmap at +$87
N $CEB4 Stretchy graphic part
W $CEB4,2,2 [$CF5B] LOD ptr
W $CEB6,2,2 Depth $10, bitmap at +$10
W $CEB8,2,2 Depth $10, bitmap at +$10
W $CEBA,2,2 Depth $10, bitmap at +$2C
W $CEBC,2,2 Depth $10, bitmap at +$2C
W $CEBE,2,2 Depth $10, bitmap at +$48
W $CEC0,2,2 Depth $10, bitmap at +$48
W $CEC2,2,2 Depth $08, bitmap at +$64
W $CEC4,2,2 Depth $08, bitmap at +$64
W $CEC6,2,2 Depth $08, bitmap at +$80
W $CEC8,2,2 Depth $08, bitmap at +$80
N $CECA Stretchy graphic part
W $CECA,2,2 [$CF5B] LOD ptr
W $CECC,2,2 Depth $08, bitmap at +$09
W $CECE,2,2 Depth $08, bitmap at +$09
W $CED0,2,2 Depth $10, bitmap at +$25
W $CED2,2,2 Depth $10, bitmap at +$25
W $CED4,2,2 Depth $08, bitmap at +$41
W $CED6,2,2 Depth $08, bitmap at +$41
W $CED8,2,2 Depth $00, bitmap at +$5D
W $CEDA,2,2 Depth $00, bitmap at +$5D
W $CEDC,2,2 Depth $00, bitmap at +$79
W $CEDE,2,2 Depth $00, bitmap at +$79
N $CEE0 Stretchy graphic part
W $CEE0,2,2 [$CF5B] LOD ptr
W $CEE2,2,2 Depth $18, bitmap at +$02
W $CEE4,2,2 Depth $18, bitmap at +$02
W $CEE6,2,2 Depth $18, bitmap at +$1E
W $CEE8,2,2 Depth $18, bitmap at +$1E
W $CEEA,2,2 Depth $10, bitmap at +$3A
W $CEEC,2,2 Depth $10, bitmap at +$3A
W $CEEE,2,2 Depth $08, bitmap at +$56
W $CEF0,2,2 Depth $08, bitmap at +$56
W $CEF2,2,2 Depth $08, bitmap at +$72
W $CEF4,2,2 Depth $08, bitmap at +$72
N $CEF6 Stretchy graphic
B $CEF6,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $CEF7,2,2 [$CF03] Pointer to stretchy_graphic_part
B $CEF9,1,1 Height 112.5% of the perspective scale
W $CEFA,2,2 [$CF19] Pointer to stretchy_graphic_part
B $CEFC,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $CEFD,2,2 [$CF2F] Pointer to stretchy_graphic_part
B $CEFF,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $CF00,2,2 [$CF45] Pointer to stretchy_graphic_part
B $CF02,1,1 Terminator
N $CF03 Stretchy graphic part
W $CF03,2,2 [$CFE7] LOD ptr
W $CF05,2,2 Depth $08, bitmap at +$17
W $CF07,2,2 Depth $08, bitmap at +$17
W $CF09,2,2 Depth $08, bitmap at +$33
W $CF0B,2,2 Depth $08, bitmap at +$33
W $CF0D,2,2 Depth $00, bitmap at +$4F
W $CF0F,2,2 Depth $00, bitmap at +$4F
W $CF11,2,2 Depth $00, bitmap at +$6B
W $CF13,2,2 Depth $00, bitmap at +$6B
W $CF15,2,2 Depth $00, bitmap at +$87
W $CF17,2,2 Depth $00, bitmap at +$87
N $CF19 Stretchy graphic part
W $CF19,2,2 [$CFE7] LOD ptr
W $CF1B,2,2 Depth $10, bitmap at +$10
W $CF1D,2,2 Depth $10, bitmap at +$10
W $CF1F,2,2 Depth $10, bitmap at +$2C
W $CF21,2,2 Depth $10, bitmap at +$2C
W $CF23,2,2 Depth $08, bitmap at +$48
W $CF25,2,2 Depth $08, bitmap at +$48
W $CF27,2,2 Depth $08, bitmap at +$64
W $CF29,2,2 Depth $08, bitmap at +$64
W $CF2B,2,2 Depth $08, bitmap at +$80
W $CF2D,2,2 Depth $08, bitmap at +$80
N $CF2F Stretchy graphic part
W $CF2F,2,2 [$CFE7] LOD ptr
W $CF31,2,2 Depth $10, bitmap at +$09
W $CF33,2,2 Depth $10, bitmap at +$09
W $CF35,2,2 Depth $18, bitmap at +$25
W $CF37,2,2 Depth $18, bitmap at +$25
W $CF39,2,2 Depth $08, bitmap at +$41
W $CF3B,2,2 Depth $08, bitmap at +$41
W $CF3D,2,2 Depth $08, bitmap at +$5D
W $CF3F,2,2 Depth $08, bitmap at +$5D
W $CF41,2,2 Depth $08, bitmap at +$79
W $CF43,2,2 Depth $08, bitmap at +$79
N $CF45 Stretchy graphic part
W $CF45,2,2 [$CFE7] LOD ptr
W $CF47,2,2 Depth $20, bitmap at +$02
W $CF49,2,2 Depth $20, bitmap at +$02
W $CF4B,2,2 Depth $20, bitmap at +$1E
W $CF4D,2,2 Depth $20, bitmap at +$1E
W $CF4F,2,2 Depth $10, bitmap at +$3A
W $CF51,2,2 Depth $10, bitmap at +$3A
W $CF53,2,2 Depth $10, bitmap at +$56
W $CF55,2,2 Depth $10, bitmap at +$56
W $CF57,2,2 Depth $10, bitmap at +$72
W $CF59,2,2 Depth $10, bitmap at +$72
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
N $D073 #HTML[#CALL(graphic($D073,40,8,0,1))]
B $D073,40,5 Bitmap data 5 bytes x 8
N $D09B #HTML[#CALL(graphic($D09B,56,8,0,1))]
B $D09B,56,7 Bitmap data 7 bytes x 8
N $D0D3 #HTML[#CALL(graphic($D0D3,56,8,0,1))]
B $D0D3,56,7 Bitmap data 7 bytes x 8
N $D10B #HTML[#CALL(graphic($D10B,64,8,0,1))]
B $D10B,64,8 Bitmap data 8 bytes x 8
N $D14B #HTML[#CALL(graphic($D14B,24,4,0,1))]
B $D14B,12,3 Bitmap data 3 bytes x 4
B $D157,153,8*19,1
N $D1F0 #HTML[#CALL(graphic($D1F0,56,8,0,1))]
B $D1F0,56,7 Bitmap data 7 bytes x 8
B $D228,100,8*12,4
N $D28C Stretchy graphic
B $D28C,1,1 Height 150% of the perspective scale
W $D28D,2,2 [$D293] Pointer to stretchy_graphic_part
B $D28F,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D290,2,2 [$D2A9] Pointer to stretchy_graphic_part
B $D292,1,1 Terminator
N $D293 Stretchy graphic part
W $D293,2,2 [$D2BF] LOD ptr
W $D295,2,2 Depth $24, bitmap at +$02
W $D297,2,2 Depth $1C, bitmap at +$02
W $D299,2,2 Depth $18, bitmap at +$09
W $D29B,2,2 Depth $18, bitmap at +$09
W $D29D,2,2 Depth $14, bitmap at +$10
W $D29F,2,2 Depth $14, bitmap at +$10
W $D2A1,2,2 Depth $14, bitmap at +$17
W $D2A3,2,2 Depth $10, bitmap at +$17
W $D2A5,2,2 Depth $10, bitmap at +$17
W $D2A7,2,2 Depth $10, bitmap at +$17
N $D2A9 Stretchy graphic part
W $D2A9,2,2 [$D2BF] LOD ptr
W $D2AB,2,2 Depth $24, bitmap at +$1E
W $D2AD,2,2 Depth $1C, bitmap at +$1E
W $D2AF,2,2 Depth $18, bitmap at +$25
W $D2B1,2,2 Depth $18, bitmap at +$25
W $D2B3,2,2 Depth $14, bitmap at +$2C
W $D2B5,2,2 Depth $14, bitmap at +$2C
W $D2B7,2,2 Depth $14, bitmap at +$33
W $D2B9,2,2 Depth $10, bitmap at +$33
W $D2BB,2,2 Depth $10, bitmap at +$33
W $D2BD,2,2 Depth $10, bitmap at +$33
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
B $D2F7,1,1 Height 150% of the perspective scale
W $D2F8,2,2 [$D2FE] Pointer to stretchy_graphic_part
B $D2FA,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D2FB,2,2 [$D332] Pointer to stretchy_graphic_part
B $D2FD,1,1 Terminator
N $D2FE Stretchy graphic part
W $D2FE,2,2 [$D348] LOD ptr
W $D300,2,2 Depth $24, bitmap at +$02
W $D302,2,2 Depth $1C, bitmap at +$02
W $D304,2,2 Depth $18, bitmap at +$09
W $D306,2,2 Depth $18, bitmap at +$09
W $D308,2,2 Depth $14, bitmap at +$10
W $D30A,2,2 Depth $14, bitmap at +$10
W $D30C,2,2 Depth $14, bitmap at +$17
W $D30E,2,2 Depth $10, bitmap at +$17
W $D310,2,2 Depth $10, bitmap at +$17
W $D312,2,2 Depth $10, bitmap at +$17
N $D314 Bitmap data 3 bytes x 10
B $D314,30,8*3,6
N $D332 Stretchy graphic part
W $D332,2,2 [$D348] LOD ptr
W $D334,2,2 Depth $24, bitmap at +$1E
W $D336,2,2 Depth $1C, bitmap at +$1E
W $D338,2,2 Depth $18, bitmap at +$25
W $D33A,2,2 Depth $18, bitmap at +$25
W $D33C,2,2 Depth $14, bitmap at +$2C
W $D33E,2,2 Depth $14, bitmap at +$2C
W $D340,2,2 Depth $14, bitmap at +$33
W $D342,2,2 Depth $10, bitmap at +$33
W $D344,2,2 Depth $10, bitmap at +$33
W $D346,2,2 Depth $10, bitmap at +$33
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
N $D380 #HTML[#CALL(graphic($D380,32,16,0,1))]
B $D380,64,4 Bitmap data 4 bytes x 16
N $D3C0 #HTML[#CALL(graphic($D3C0,32,24,0,1))]
B $D3C0,96,4 Bitmap data 4 bytes x 24
B $D420,24,8
N $D438 #HTML[#CALL(graphic($D438,32,16,0,1))]
B $D438,64,4 Bitmap data 4 bytes x 16
N $D478 #HTML[#CALL(graphic($D478,32,24,0,1))]
B $D478,96,4 Bitmap data 4 bytes x 24
B $D4D8,111,8*13,7
N $D547 #HTML[#CALL(graphic($D547,24,12,0,1))]
B $D547,36,3 Bitmap data 3 bytes x 12
B $D56B,106,8*13,2
N $D5D5 #HTML[#CALL(graphic($D5D5,16,8,0,1))]
B $D5D5,16,2 Bitmap data 2 bytes x 8
B $D5E5,41,8*5,1
N $D60E #HTML[#CALL(graphic($D60E,8,4,0,1))]
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
N $D64A #HTML[#CALL(graphic($D64A,32,17,0,1))]
B $D64A,68,4 Bitmap data 4 bytes x 17
N $D68E #HTML[#CALL(graphic($D68E,24,13,0,1))]
B $D68E,39,3 Bitmap data 3 bytes x 13
N $D6B5 #HTML[#CALL(graphic($D6B5,16,9,0,1))]
B $D6B5,18,2 Bitmap data 2 bytes x 9
N $D6C7 #HTML[#CALL(graphic($D6C7,16,7,1,1))]
B $D6C7,28,4 Bitmap data (masked) 4 bytes x 7
N $D6E3 #HTML[#CALL(graphic($D6E3,16,7,1,1))]
B $D6E3,28,4 Pre-shifted bitmap data (masked) 4 bytes x 7
N $D6FF Stretchy graphic
B $D6FF,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D700,2,2 [$D719] Pointer to stretchy_graphic_part
B $D702,1,1 Height 25% of the perspective scale
W $D703,2,2 [$D72F] Pointer to stretchy_graphic_part
B $D705,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D706,2,2 [$D75B] Pointer to stretchy_graphic_part
B $D708,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D709,2,2 [$D787] Pointer to stretchy_graphic_part
B $D70B,1,1 Terminator
N $D70C Stretchy graphic
B $D70C,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D70D,2,2 [$D719] Pointer to stretchy_graphic_part
B $D70F,1,1 Height 25% of the perspective scale
W $D710,2,2 [$D745] Pointer to stretchy_graphic_part
B $D712,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D713,2,2 [$D771] Pointer to stretchy_graphic_part
B $D715,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D716,2,2 [$D79D] Pointer to stretchy_graphic_part
B $D718,1,1 Terminator
N $D719 Stretchy graphic part
W $D719,2,2 [$D7B3] LOD ptr
W $D71B,2,2 Depth $1C, bitmap at +$02
W $D71D,2,2 Depth $14, bitmap at +$02
W $D71F,2,2 Depth $10, bitmap at +$17
W $D721,2,2 Depth $10, bitmap at +$17
W $D723,2,2 Depth $0C, bitmap at +$2C
W $D725,2,2 Depth $0C, bitmap at +$2C
W $D727,2,2 Depth $0C, bitmap at +$56
W $D729,2,2 Depth $08, bitmap at +$56
W $D72B,2,2 Depth $08, bitmap at +$56
W $D72D,2,2 Depth $08, bitmap at +$56
N $D72F Stretchy graphic part
W $D72F,2,2 [$D7B3] LOD ptr
W $D731,2,2 Depth $1C, bitmap at +$48
W $D733,2,2 Depth $14, bitmap at +$48
W $D735,2,2 Depth $10, bitmap at +$1E
W $D737,2,2 Depth $10, bitmap at +$1E
W $D739,2,2 Depth $0C, bitmap at +$33
W $D73B,2,2 Depth $0C, bitmap at +$33
W $D73D,2,2 Depth $0C, bitmap at +$5D
W $D73F,2,2 Depth $08, bitmap at +$5D
W $D741,2,2 Depth $08, bitmap at +$5D
W $D743,2,2 Depth $08, bitmap at +$5D
N $D745 Stretchy graphic part
W $D745,2,2 [$D7B3] LOD ptr
W $D747,2,2 Depth $24, bitmap at +$48
W $D749,2,2 Depth $1C, bitmap at +$48
W $D74B,2,2 Depth $18, bitmap at +$1E
W $D74D,2,2 Depth $18, bitmap at +$1E
W $D74F,2,2 Depth $0C, bitmap at +$33
W $D751,2,2 Depth $0C, bitmap at +$33
W $D753,2,2 Depth $14, bitmap at +$5D
W $D755,2,2 Depth $10, bitmap at +$5D
W $D757,2,2 Depth $10, bitmap at +$5D
W $D759,2,2 Depth $10, bitmap at +$5D
N $D75B Stretchy graphic part
W $D75B,2,2 [$D7B3] LOD ptr
W $D75D,2,2 Depth $1C, bitmap at +$09
W $D75F,2,2 Depth $14, bitmap at +$09
W $D761,2,2 Depth $10, bitmap at +$4F
W $D763,2,2 Depth $10, bitmap at +$4F
W $D765,2,2 Depth $0C, bitmap at +$3A
W $D767,2,2 Depth $0C, bitmap at +$3A
W $D769,2,2 Depth $0C, bitmap at +$5D
W $D76B,2,2 Depth $08, bitmap at +$5D
W $D76D,2,2 Depth $08, bitmap at +$5D
W $D76F,2,2 Depth $08, bitmap at +$5D
N $D771 Stretchy graphic part
W $D771,2,2 [$D7B3] LOD ptr
W $D773,2,2 Depth $1C, bitmap at +$09
W $D775,2,2 Depth $14, bitmap at +$09
W $D777,2,2 Depth $10, bitmap at +$4F
W $D779,2,2 Depth $10, bitmap at +$4F
W $D77B,2,2 Depth $0C, bitmap at +$3A
W $D77D,2,2 Depth $0C, bitmap at +$3A
W $D77F,2,2 Depth $14, bitmap at +$5D
W $D781,2,2 Depth $10, bitmap at +$5D
W $D783,2,2 Depth $10, bitmap at +$5D
W $D785,2,2 Depth $10, bitmap at +$5D
N $D787 Stretchy graphic part
W $D787,2,2 [$D7B3] LOD ptr
W $D789,2,2 Depth $24, bitmap at +$10
W $D78B,2,2 Depth $1C, bitmap at +$10
W $D78D,2,2 Depth $10, bitmap at +$25
W $D78F,2,2 Depth $10, bitmap at +$25
W $D791,2,2 Depth $0C, bitmap at +$41
W $D793,2,2 Depth $0C, bitmap at +$41
W $D795,2,2 Depth $0C, bitmap at +$64
W $D797,2,2 Depth $08, bitmap at +$64
W $D799,2,2 Depth $08, bitmap at +$64
W $D79B,2,2 Depth $08, bitmap at +$64
N $D79D Stretchy graphic part
W $D79D,2,2 [$D7B3] LOD ptr
W $D79F,2,2 Depth $24, bitmap at +$10
W $D7A1,2,2 Depth $1C, bitmap at +$10
W $D7A3,2,2 Depth $18, bitmap at +$25
W $D7A5,2,2 Depth $18, bitmap at +$25
W $D7A7,2,2 Depth $0C, bitmap at +$41
W $D7A9,2,2 Depth $0C, bitmap at +$41
W $D7AB,2,2 Depth $14, bitmap at +$64
W $D7AD,2,2 Depth $10, bitmap at +$64
W $D7AF,2,2 Depth $10, bitmap at +$64
W $D7B1,2,2 Depth $10, bitmap at +$64
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
N $D81C #HTML[#CALL(graphic($D81C,24,4,0,1))]
B $D81C,12,3 Bitmap data 3 bytes x 4
N $D828 #HTML[#CALL(graphic($D828,24,13,0,1))]
B $D828,39,3 Bitmap data 3 bytes x 13
N $D84F #HTML[#CALL(graphic($D84F,8,7,0,1))]
B $D84F,7,1 Bitmap data 1 bytes x 7
N $D856 #HTML[#CALL(graphic($D856,16,3,0,1))]
B $D856,6,2 Bitmap data 2 bytes x 3
N $D85C #HTML[#CALL(graphic($D85C,8,2,0,1))]
B $D85C,2,1 Bitmap data 1 bytes x 2
B $D85E,116,8*14,4
N $D8D2 Stretchy graphic
B $D8D2,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D8D3,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D8D5,1,1 Height 50% of the perspective scale
W $D8D6,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D8D8,1,1 Height 112.5% of the perspective scale
W $D8D9,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D8DB,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D8DC,2,2 [$D8EC] Pointer to stretchy_graphic_part
B $D8DE,1,1 Terminator
N $D8DF Stretchy graphic
B $D8DF,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D8E0,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D8E2,1,1 Height 50% of the perspective scale
W $D8E3,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D8E5,1,1 Height 112.5% of the perspective scale
W $D8E6,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D8E8,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D8E9,2,2 [$D902] Pointer to stretchy_graphic_part
B $D8EB,1,1 Terminator
N $D8EC Stretchy graphic part
W $D8EC,2,2 [$D918] LOD ptr
W $D8EE,2,2 Depth $18, bitmap at +$02
W $D8F0,2,2 Depth $10, bitmap at +$02
W $D8F2,2,2 Depth $08, bitmap at +$09
W $D8F4,2,2 Depth $08, bitmap at +$09
W $D8F6,2,2 Depth $0C, bitmap at +$10
W $D8F8,2,2 Depth $0C, bitmap at +$10
W $D8FA,2,2 Depth $08, bitmap at +$17
W $D8FC,2,2 Depth $08, bitmap at +$17
W $D8FE,2,2 Depth $04, bitmap at +$1E
W $D900,2,2 Depth $04, bitmap at +$1E
N $D902 Stretchy graphic part
W $D902,2,2 [$D918] LOD ptr
W $D904,2,2 Depth $18, bitmap at +$02
W $D906,2,2 Depth $10, bitmap at +$02
W $D908,2,2 Depth $10, bitmap at +$09
W $D90A,2,2 Depth $10, bitmap at +$09
W $D90C,2,2 Depth $0C, bitmap at +$10
W $D90E,2,2 Depth $0C, bitmap at +$10
W $D910,2,2 Depth $08, bitmap at +$17
W $D912,2,2 Depth $08, bitmap at +$17
W $D914,2,2 Depth $0C, bitmap at +$1E
W $D916,2,2 Depth $0C, bitmap at +$1E
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
N $D93B #HTML[#CALL(graphic($D93B,40,8,0,1))]
B $D93B,40,5 Bitmap data 5 bytes x 8
N $D963 #HTML[#CALL(graphic($D963,32,6,0,1))]
B $D963,24,4 Bitmap data 4 bytes x 6
N $D97B #HTML[#CALL(graphic($D97B,24,4,0,1))]
B $D97B,12,3 Bitmap data 3 bytes x 4
N $D987 #HTML[#CALL(graphic($D987,24,4,0,1))]
B $D987,12,3 Pre-shifted bitmap data 3 bytes x 4
N $D993 #HTML[#CALL(graphic($D993,24,3,1,1))]
B $D993,18,6 Bitmap data (masked) 6 bytes x 3
N $D9A5 #HTML[#CALL(graphic($D9A5,24,3,1,1))]
B $D9A5,18,6 Pre-shifted bitmap data (masked) 6 bytes x 3
N $D9B7 #HTML[#CALL(graphic($D9B7,16,2,1,1))]
B $D9B7,8,4 Bitmap data (masked) 4 bytes x 2
B $D9BF,4,4
N $D9C3 #HTML[#CALL(graphic($D9C3,16,2,1,1))]
B $D9C3,8,4 Pre-shifted bitmap data (masked) 4 bytes x 2
B $D9CB,4,4
N $D9CF Stretchy graphic
B $D9CF,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D9D0,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D9D2,1,1 Height 112.5% of the perspective scale
W $D9D3,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D9D5,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D9D6,2,2 [$D9F9] Pointer to stretchy_graphic_part
B $D9D8,1,1 Terminator
N $D9D9 Stretchy graphic
B $D9D9,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D9DA,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D9DC,1,1 Height 112.5% of the perspective scale
W $D9DD,2,2 [out-of-bounds] Pointer to stretchy_graphic_part
B $D9DF,1,1 Fixed height (bitmap width - 2), no perspective scaling
W $D9E0,2,2 [$D9E3] Pointer to stretchy_graphic_part
B $D9E2,1,1 Terminator
N $D9E3 Stretchy graphic part
W $D9E3,2,2 [$DA0F] LOD ptr
W $D9E5,2,2 Depth $20, bitmap at +$02
W $D9E7,2,2 Depth $18, bitmap at +$02
W $D9E9,2,2 Depth $10, bitmap at +$09
W $D9EB,2,2 Depth $10, bitmap at +$09
W $D9ED,2,2 Depth $0C, bitmap at +$10
W $D9EF,2,2 Depth $0C, bitmap at +$10
W $D9F1,2,2 Depth $10, bitmap at +$17
W $D9F3,2,2 Depth $10, bitmap at +$17
W $D9F5,2,2 Depth $0C, bitmap at +$1E
W $D9F7,2,2 Depth $0C, bitmap at +$1E
N $D9F9 Stretchy graphic part
W $D9F9,2,2 [$DA0F] LOD ptr
W $D9FB,2,2 Depth $20, bitmap at +$02
W $D9FD,2,2 Depth $18, bitmap at +$02
W $D9FF,2,2 Depth $10, bitmap at +$09
W $DA01,2,2 Depth $10, bitmap at +$09
W $DA03,2,2 Depth $0C, bitmap at +$10
W $DA05,2,2 Depth $0C, bitmap at +$10
W $DA07,2,2 Depth $08, bitmap at +$17
W $DA09,2,2 Depth $08, bitmap at +$17
W $DA0B,2,2 Depth $04, bitmap at +$1E
W $DA0D,2,2 Depth $04, bitmap at +$1E
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
N $DA32 #HTML[#CALL(graphic($DA32,24,13,0,1))]
B $DA32,39,3 Bitmap data 3 bytes x 13
N $DA59 #HTML[#CALL(graphic($DA59,24,10,0,1))]
B $DA59,30,3 Bitmap data 3 bytes x 10
N $DA77 #HTML[#CALL(graphic($DA77,24,7,0,1))]
B $DA77,21,3 Bitmap data 3 bytes x 7
N $DA8C #HTML[#CALL(graphic($DA8C,24,7,0,1))]
B $DA8C,21,3 Pre-shifted bitmap data 3 bytes x 7
N $DAA1 #HTML[#CALL(graphic($DAA1,16,5,1,1))]
B $DAA1,20,4 Bitmap data (masked) 4 bytes x 5
N $DAB5 #HTML[#CALL(graphic($DAB5,16,5,1,1))]
B $DAB5,20,4 Pre-shifted bitmap data (masked) 4 bytes x 5
N $DAC9 #HTML[#CALL(graphic($DAC9,16,4,1,1))]
B $DAC9,16,4 Bitmap data (masked) 4 bytes x 4
N $DAD9 #HTML[#CALL(graphic($DAD9,16,4,1,1))]
B $DAD9,16,4 Pre-shifted bitmap data (masked) 4 bytes x 4
b $DAE9 [Stage 5] Unidentified data (gap before end screen)
B $DAE9,1303,8*162,7
c $E000 Show the end screen
D $E000 Runs the end-of-game results screen: clears the playfield, starts the beatbox and this bank's own interrupt-driven music and script engine, then loops driving the script, the current frame-advance handler and drive_chatter until the fire key has been pressed twice -- once to skip ahead to the congratulations script, once more to leave.
D $E000 The whole bank has already been paged in and copied down to $5C00..$7BFF, so every address here reads $8400 lower at run time ($E000 runs at $5C00). This routine then performs a second, inner relocation of its own: the 768 bytes at $F7EF (bank7_reset_music onwards) are copied to $F300, which is why the calls below target $F300 and $F340 rather than the bank addresses the code was assembled at.
D $E000 The five bytes at $A16D..$A171 are the engine's state: the script pointer ($A16D/$A16E), the input mask ($A16F), the script frame countdown ($A170) and the keyscan divider ($A171).
@ $E000 label=show_end_screen
C $E000,3 Call es_clear
C $E003,3 Source $F7EF, the reset_music/play_music_48k block
C $E006,3 Destination of the inner relocation
C $E009,3 768 bytes
C $E00C,2 Copy
N $E00E $A16D = $5CFE [$E0FE here]
C $E00E,9 $A16D -> script_data [$5CFE]
C $E017,1 $A16F C is zero
C $E019,2 $A170
C $E01C,2 $A171
C $E01E,3 Call bank7_setup_interrupts
C $E021,3 Call reset_music (relocated copy of bank7_reset_music, #R$F7EF, copied here by the LDIR above)
C $E024,1 Enable interrupts
C $E025,1 Wait for an interrupt
@ $E026 label=es_loop
C $E026,3 Call play_music_48k (relocated copy of b7_play_music_48k, #R$F82F)
C $E029,4 Decrement $A170
C $E02D,3 Call run_script
C $E030,3 Call <self modified>
C $E033,4 Decrement $A171
C $E037,2 Loop while > 0
C $E039,2 Reset $A171 to 5
C $E03B,3 Call drive_chatter
C $E03E,3 Call keyscan
C $E041,4 Loop while key not pressed
C $E045,4 AND user input with user input mask
C $E049,2 Jump if masked key remains pressed
C $E04B,1 A is zero here, so this sets it to 1
C $E04C,3 Mark the first fire press as consumed
C $E04F,3 Run the script on the next frame
C $E052,6 Set script data pointer to $5DE3 [$E1E3]
C $E058,3 Call drive_chatter_stop
C $E05B,3 Call keyscan
C $E05E,4 Loop while key pressed
C $E062,2 Loop
C $E065,4 Set interrupt vector table base to $8000
C $E069,3 Call drive_chatter_stop
b $E06C Data block at E06C
N $E06C $E06C ($5C6C) and $E06D ($5C6D) are NOT part of the pointer table below -- they are two independent 1-bit trigger flags, each tested/consumed via RLC (HL)+RET C/NC: $5C6C by es_attribute_fade_in ($E42E), $5C6D by the handshake frame-advance code at $E472/handshake_table. RLC both tests bit 7 (into carry) and rotates it into bit 0, so each read self-clears/advances the flag -- confirmed via skool cross-reference (LD HL,$5C6C at $E42E and $E472; LD HL,$5C6D at $E3BA and $E46D)
@ $E06C label=data_e06c
B $E06C,2,1 Trigger flags (see note above)
N $E06E Still unidentified (3 bytes, values $01,$3F,$96) -- confirmed dead end: grepped for literal $5C6E/$5C6F/$5C70 operands across the full static bank-7 disassembly and the entire trace-end-screen.log execution capture, zero hits either way. Not reached by any direct-addressed instruction in the code disassembled so far; either accessed only via computed/indexed addressing not yet spotted, read by common-RAM code outside bank 7, or genuinely unused padding
@ $E06E label=data_e06e
B $E06E,3,1
N $E071 Pointer table for the congratulations text: three pointers packed tight ($E071-$E076), then a 2-byte gap, then a fourth pointer ($E079-$E07A), then a trailing byte. Bytes at $E071-$E072, $E073-$E074, $E075-$E076, $E079-$E07A are little-endian relocated pointers $5C7C/$5CA5/$5CD0/$5CE8 -- relocating back (+$8400) gives $E07C/$E0A5/$E0D0/$E0E8, an exact match to the four text strings below
W $E071,6,2 Pointers 1-3 (tight-packed)
N $E077 Gap between pointer 3 and pointer 4 ($FF,$03) -- same confirmed dead end as $E06E: no literal $5C77/$5C78 operand anywhere in bank 7's static disassembly or trace-end-screen.log. $FF matches the terminator convention seen elsewhere (see $F8F5 SFX table), possibly a record separator/duration byte rather than table padding, but unproven
B $E077,2,1
W $E079,2,2 Pointer 4
B $E07B,1,1 Terminator
T $E07C,41,40:n1 "I WOULD LIKE TO CONGRATULATE YOU FOR YOUR"
T $E0A5,43,42:n1 "HEROISM AND BRAVERY. YOU WILL BE REMEMBERED"
T $E0D0,24,23:n1 "FOR A LONG TIME TO COME."
T $E0E8,22,21:n1 "PRESS GEAR TO CONTINUE"
b $E0FE Script data
@ $E0FE label=script_data
B $E0FE,1,1
W $E0FF,2,2 -> data_e06e
B $E101,3,2,1
W $E104,2,2 bitmap_endshot_1
B $E106,7,7
W $E10D,2,2 bitmap_endshot_2
B $E10F,7,7
W $E116,2,2 bitmap_endshot_3
B $E118,7,7
W $E11F,2,2 bitmap_endshot_4
B $E121,7,7
W $E128,2,2 bitmap_endshot_1
B $E12A,6,6
W $E130,2,2 bitmap_endshot_2
B $E132,6,6
W $E138,2,2 bitmap_endshot_3
B $E13A,6,6
W $E140,2,2 bitmap_endshot_4
@ $E1E3 label=possible_data_block_e1e3
B $E142,200,13,8*18,4*2,8*4,3
c $E20A Command interpreter
D $E20A Confirmed live by trace-end-screen.log: reads script bytes via $A16D and dispatches on a DEC A/JP Z chain. Self-modifies the CALL operand at $E030 (live address $5C31, patched by LD ($5C31),DE at $E2D4) to repoint the main loop's per-frame handler call based on which command last ran.
@ $E20A label=run_script
C $E20A,3 Load script pointer
@ $E20D label=rs_loop
C $E20D,1 Load the next script command
C $E20E,1 Advance script program counter
C $E20F,4 If command == 1 goto es_clear_then_draw_frame
C $E213,4 If command == 2 goto es_draw_frame_common
C $E217,3 -> es_attribute_fade_in
C $E21A,2 16
C $E21C,4 If command == 3 goto $E2CD/es_set_dispatch
C $E220,3 -> es_attribute_fade_out
C $E223,4 If command == 4 goto $E2CD/es_set_dispatch
C $E227,3 -> es_handler_handshake
C $E22A,4 If command == 5 goto $E2CD/es_set_dispatch
C $E22E,2 32
C $E230,3 -> es_handler_glyph_fade_c
C $E233,4 If command == 6 goto ...
C $E237,4 If command == 7 goto ...
C $E23B,4 If command == 8 goto ...
C $E23F,4 If command == 9 goto ...
C $E243,4 If command == 10 goto ...
C $E247,4 If command == 11 goto ...
C $E24B,3 If command == 12 goto ...
C $E24E,3 If command == 13 goto $E256
C $E254,2 Loop
@ $E256 label=es_handler_draw_score
N $E2B2 Command 12: start chatter using a block pointer taken from the script.
@ $E2B2 label=es_handler_chatter
@ $E2CD label=es_set_dispatch
C $E2CD,3 Update script pointer
C $E2D4,4 Store func ptr in E031
c $E2D9 Interpreter handler: draw graphic frame
D $E2D9 Reads a byte then a word pointer from the script -- e.g. one of the bitmap_endshot_N pointers at $E104 onward -- calls draw_endshot to blit it, then rejoins run_script's loop.
D $E2D9 draw_endshot ($E4A9) is the montage-shot blitter, not a generic tile blit -- 64-row bitmap copy plus attribute copy.
@ $E2D9 label=es_clear_then_draw_frame
C $E2DA,3 Call $E49C (buffer zeroing thing)
@ $E2DE label=es_draw_frame_common
N $E2F0 Command 11: clear the backbuffer, then render a run of text.
@ $E2F0 label=es_handler_render_text
N $E2F5 Command 10: render a run of text without clearing first. Reads a colour byte and a destination word from the script, then plots characters until one is found with its top bit set.
@ $E2F5 label=es_render_text_common
N $E31F Render one end-screen character at double height. A space advances both cursors by a column and draws nothing.
@ $E31F label=es_plot_char
@ $E328 label=es_plot_char_glyph
N $E381 This entry point is used by the routine at #R$E3A5.
b $E3A5 Handshake animation frame-advance table
D $E3A5 Looked up as (row_count,source_ptr) using $A172 mod 6 as frame index, LDIRed to screen $48AC with third-boundary row-wrap arithmetic; $A172 then increments/wraps for the next call. Code resumes at $E3B7 after the table.
D $E3A5 18-byte table, 6 entries x 3 bytes (row_count:1, source_ptr:2 LE); auto-disassembled as bogus instructions by earlier passes because it sits inline in the code stream. Decoded and verified byte-for-byte -- a ping-pong sequence (grip closes 1->4, reopens 4->2) confirming the handshake_N row counts exactly.
@ $E3A5 label=handshake_table
B $E3A5,1,1
W $E3A6,2,2 37, handshake_1
B $E3A8,1,1
W $E3A9,2,2 35, handshake_2
B $E3AB,1,1
W $E3AC,2,2 34, handshake_3
B $E3AE,1,1
W $E3AF,2,2 32, handshake_4
B $E3B1,1,1
W $E3B2,2,2 34, handshake_3
B $E3B4,1,1
W $E3B5,2,2 35, handshake_2
c $E3B7 Fade the glyph attribute band, then advance the handshake
D $E3B7 The entry point the HANDSHAKE script command dispatches to ($5FB7 in the relocated dispatch table). It fades the band one step by calling es_handler_glyph_fade_b at #R$E472 -- $6072 once relocated -- then falls into es_handler_handshake_advance. The RESET HANDSHAKE and HANDSHAKE AGAIN commands enter at $5FBA instead, skipping the fade.
@ $E3B7 label=es_handler_handshake
C $E3B7,3 Call es_handler_glyph_fade_b [#R$E472]
@ $E3BA label=es_handler_handshake_advance
c $E42E Sweep the playfield attributes toward their target colours
D $E42E Called via the interpreter's self-modified $E030 dispatch. The gate byte at $E06C only lets it run on alternate calls. When it does run it walks all 512 attribute cells of $5900..$5AFF against the matching backbuffer bytes at $F000..$F1FF, which other code has rasterised the glyph shapes into: cells with BRIGHT set are left alone, cells whose masked colour already matches the backbuffer are copied verbatim, and the rest step their ink and paper fields one unit toward the target. Repeated over several frames this reveals the glyphs as a gradual colour change.
R $E42E The ink increment at #R$E45D and the paper increment at #R$E468 are not
R $E42E masked back into their 3-bit fields, so a field that reaches its target
R $E42E exactly on the last step carries into the next one.
@ $E42E label=es_attribute_fade_in
C $E42E,3 HL -> data_e06c [$E06C]
C $E431,2 50-50 pattern, rotate in place
C $E433,1 Return when bit set
C $E434,3 Middle band of attributes
C $E437,3 Backbuffer
C $E43A,4 Leave the cell alone if BRIGHT is set
@ $E452 label=e452
c $E46D Fade the glyph attribute band out
D $E46D Two thin wrappers over one tail: #R$E46D gates on $5C6D and #R$E472 on $5C6C. The tail rotates the gate byte and returns unless the bit rotated out was set, so each gate runs the fade on alternate calls. When it does run it sweeps the same 512 attribute cells es_attribute_fade_in covers, taking one off each cell's ink and one unit off its paper, both stopping at zero.
D $E46D The bytes are masked down to their ink and paper fields and re-combined, so BRIGHT and FLASH are dropped from every cell the fade touches.
@ $E46D label=es_handler_glyph_fade_c
C $E46D,3 Gate on $5C6D
C $E470,2 Jump to es_attribute_fade_out
@ $E472 label=es_handler_glyph_fade_b
C $E472,3 Gate on $5C6C
@ $E475 label=es_attribute_fade_out
C $E475,2 Rotate the gate byte
@ $E499 label=es_clear
C $E499,3 Call clear_playfield
C $E49C,12 Zero first 512 bytes of the (backbuffer)
C $E4A8,1 Return
c $E4A9 Draws an end-game montage shot to the screen (attrs -> screen)
D $E4A9 I:HL Address of image to plot DE Destination address in the screen
@ $E4A9 label=draw_endshot
C $E4A9,1 Preserve destination in screen
C $E4AA,2 Counter = 64 rows
C $E4AC,1 Preserve counter
C $E4AD,3 13 bytes to transfer
C $E4B0,1 Preserve destination
C $E4B1,2 Copy
C $E4B3,1 Restore destination
C $E4B4,16 Scanline increment
C $E4C4,1 Restore counter
C $E4C5,2 Loop while counter
C $E4C7,1 Restore destination
C $E4C8,9 Turn the destination's high byte into the matching backbuffer attribute address
C $E4D1,2 8 rows of attributes
@ $E4D3 label=e4d3_loop
C $E4D3,5 Copy 13 attribute bytes
C $E4D8,5 DE += 19  (a gap value - 13+19 = 32)
C $E4DD,1 A--
C $E4DE,2 Loop
C $E4E0,1 Return
b $E4E1 [Graphics] End-game montage shots
D $E4E1 $60E1 once relocated.
N $E4E1 End-game montage shot 1 (104x64)
N $E4E1 #HTML[#CALL(endshot($E4E1))]
@ $E4E1 label=bitmap_endshot_1
B $E4E1,832,8 Bitmap data
B $E821,104,8 Attribute bytes
N $E889 End-game montage shot 2 (104x64)
N $E889 #HTML[#CALL(endshot($E889))]
@ $E889 label=bitmap_endshot_2
B $E889,832,8 Bitmap data
B $EBC9,104,8 Attribute bytes
N $EC31 End-game montage shot 3 (104x64)
N $EC31 #HTML[#CALL(endshot($EC31))]
@ $EC31 label=bitmap_endshot_3
B $EC31,832,8 Bitmap data
B $EF71,112,8 Attribute bytes
N $EFE1 End-game montage shot 4 (104x64)
N $EFE1 #HTML[#CALL(endshot($EFE1))]
@ $EFE1 label=bitmap_endshot_4
B $EFE1,824,8 Bitmap data
B $F319,104,8 Attribute bytes
N $F381 End-game handshake frame 1 (64x37)
N $F381 #HTML[#CALL(graphic($F381,64,37,0,0))]
@ $F381 label=handshake_1
B $F381,296,8 Bitmap data
N $F4A9 End-game handshake frame 2 (64x35)
N $F4A9 #HTML[#CALL(graphic($F4A9,64,35,0,0))]
@ $F4A9 label=handshake_2
B $F4A9,280,8 Bitmap data {
N $F5C1 End-game handshake frame 3 (64x34)
N $F5C1 #HTML[#CALL(graphic($F5C1,64,34,0,0))]
@ $F5C1 label=handshake_3
B $F5C1,272,8 Bitmap data
N $F6D1 End-game handshake frame 4 (64x32)
N $F6D1 #HTML[#CALL(graphic($F6D1,64,32,0,0))]
@ $F6D1 label=handshake_4
B $F6D1,256,8 Bitmap data
c $F7D1 Interrupt setup
D $F7D1 Almost the same as #R$EE40@main
@ $F7D1 label=bank7_setup_interrupts
C $F7D1,1 Disable interrupts
C $F7D2,3 Load address of interrupt vector table
C $F7D5,1 Interrupt vector table high byte
N $F7D6 Point the 128 interrupt vector table words at $FD00 to $FEFE.
C $F7D6,2 256 iterations
C $F7D8,2 #REGc = $FE
@ $F7DA label=b7si_fill
C $F7DA,2 *HL++ = $FE
C $F7DC,2 Loop
N $F7DE Now store an additional byte since the IVT needs to be 257 bytes long.
C $F7DE,1 Store final $FE
C $F7DF,2 Set interrupt vector table base to $FD00
C $F7E1,2 Set interrupt mode 2
N $F7E3 Set $FEFE to be "JP $F3C1".
C $F7E3,5 $FEFE = Opcode for JP
C $F7E8,6 $FEFF = $F3C1
C $F7EE,1 Return
c $F7EF Reset music
D $F7EF Almost the same as #R$EE5E@main
@ $F7EF label=bank7_reset_music
C $F7EF,1 A = 0
C $F7F0,3 Self modify #R$F8A4 -- clear the <drum is playing> flag
C $F7F3,3 Self modify #R$F897 -- clear the extra delay counter
C $F7F6,3 Self modify #R$F839 -- clear the <playback started> flag
C $F7F9,3 Load address of music patterns
C $F7FC,3 Jump to (np_start_at_hl)
c $F7FF Setup the next music pattern
D $F7FF Almost the same as #R$EE6E@main
N $F7FF Keep playing current pattern until this counter becomes zero.
@ $F7FF label=bank7_next_pattern
C $F7FF,2 Load number of pattern repetitions. Self modified by #R$F80F below.
C $F801,1 Decrease
C $F802,3 Self modify #R$F7FF above
C $F805,1 Return if non-zero
@ $F806 label=b7np_next
C $F806,3 Load address of current pattern. Self modified by #R$F814 below.
@ $F809 label=b7np_start_at_hl
C $F809,2 Read new repetition count
C $F80B,4 Jump to #R$F829 if it's $FF (end of patterns)
C $F80F,3 Self modify #R$F7FF above with new repetition count
C $F812,2 Read music data offset
C $F814,3 Self modify #R$F806 above (pattern addr)
N $F817 Calculate address of music data.
C $F817,2 #REGbc = #REGc
C $F819,3 Load address of base of music data
C $F81C,1 Combine with offset
C $F81D,2 A = *HL++ -- load first byte of music data
C $F81F,3 Self modify #R$F850 (delay reload, first byte of pattern)
C $F822,3 Self modify #R$F844 (delay counter, first byte of pattern)
C $F825,3 Self modify #R$F860 (addr of second music data byte in pattern)
C $F828,1 Return
@ $F829 label=b7np_restart
C $F829,4 HL = wordat(HL); HL++
C $F82D,2 Jump to b7np_start_at_hl
c $F82F Play menu music (48K mode only)
D $F82F Almost the same as #R$EE9E@main.
@ $F82F label=b7_play_music_48k
C $F82F,4 Clear the <interrupt flag> at #R$F8AA
C $F833,3 Load the end screen's input mask
C $F836,1 Set flags
C $F837,2 Play nothing more once the player has pressed fire
C $F839,2 <Playback started> flag, self modified by #R$F83F below
C $F83B,1 Set flags
C $F83C,2 Jump to b7pm_delay_1 if playback is already running
C $F83E,4 Otherwise mark playback started by self modifying #R$F839 above
C $F842,2 Jump to b7pm_reset_pattern
@ $F844 label=b7pm_delay_1
C $F844,2 Note delay counter. Self modified by #R$F822 above and by #R$F84A, #R$F852, #R$F877 and #R$F89F below
C $F846,1 Decrement and set flags
C $F847,3 Jump to b7pm_delay_complete if zero
C $F84A,3 Self modify #R$F844 above (store the decremented value)
C $F84D,3 Jump to b7pm_zero_or_456
@ $F850 label=b7pm_delay_complete
C $F850,2 Delay reload value. Self modified by #R$F81F above with the first byte of the pattern
C $F852,3 Self modify #R$F844 above (reload the delay counter)
C $F855,3 Self modified below, cycles $F12x .. $F2xx ish  <addr of next music byte>
N $F858 Fetch a byte of the form 0bdaaaaiii (d is delay bit, aaaa is argument, iii is instrument index)
@ $F858 label=b7pm_loop
C $F858,1 Fetch a music byte
C $F859,1 Temporarily decrement for testing (will undo later)
C $F85A,3 Jump to pm_continue_pattern if the byte is NOT 1 - the terminating byte of the music data
C $F85D,3 Call bank7_next_pattern ($F310 when reloc, $F7FF here)
@ $F860 label=b7pm_reset_pattern
C $F860,3 Self modified by #R$F825 above with the address of the second music data byte
C $F863,3 Self modify #R$F855 above, restarting the pattern
C $F866,3 Loop
@ $F869 label=b7pm_continue_pattern
C $F869,1 Advance
C $F86A,3 Update <addr of next music byte> above
C $F86D,1 Compensate for earlier decrement
C $F86E,4 Jump if music byte < 128
N $F872 A byte of the form 0b1aaaaiii (1 is delay bit)
C $F872,2 Isolate delay bit
C $F874,1 Bank
C $F875,5 Self modify #R$F844 above, setting the note delay counter to 1
C $F87A,3 Self modify #R$F897 below, flagging the extra delay
C $F87D,1 Unbank
N $F87E A byte now of the form 0b0aaaaiii
@ $F87E label=b7pm_play_inst
C $F87E,1 Save a copy of the byte
C $F87F,2 Extract bottom 3 instrument bits  -- must be the command
C $F881,2 Jump to pm_zero_or_456 if they're zero
C $F883,1 Save the instrument
C $F884,7 Extract the four argument bits
C $F88B,4 Jump to b7_playdrum_X (#R$F8B9) if instrument is 1 -- drum 2
C $F88F,4 Jump to b7_playdrum_Y (#R$F8C0) if instrument is 2 -- drum 1
C $F893,4 Jump to b7_play_noise (#R$F9F3) if instrument is 3 -- noise
@ $F897 label=b7pm_zero_or_456
C $F897,2 Extra delay counter. Self modified by #R$F7F3 and #R$F87A above and by #R$F8A3 below
C $F899,1 Set flags
C $F89A,2 Jump to b7pm_start_drums if zero (no delay)
C $F89C,4 Self modify #R$F844 above, decrementing the note delay counter
C $F8A0,4 Self modify #R$F897 above, decrementing the extra delay counter
@ $F8A4 label=b7pm_start_drums
C $F8A4,2 <Drum is playing> flag. Self modified by #R$F7F0 above and by #R$F8CA and #R$F8EF below
C $F8A6,1 Decrement
C $F8A7,3 Jump to b7pd_bank_go if a sample was interrupted part way through, resuming it
@ $F8AA label=b7pm_wait_for_interrupt
C $F8AA,5 Loop while waiting for this <interrupt flag> to be set
C $F8AF,1 Return
c $F8B0 Interrupt entry point
D $F8B0 bank7_setup_interrupts (#R$F7D1) builds a table at $FD00 containing 257 occurrences of $FE, then plants a JP $F3C1 at $FEFE -- $F3C1 being this routine's address once the bank has been relocated.
@ $F8B0 label=b7_interrupt_entry
C $F8B0,1 Preserve registers
C $F8B1,5 Set the <interrupt flag> to $FF by self modifying #R$F8AA
C $F8B6,1 Restore registers
C $F8B7,1 Enable interrupts
C $F8B8,1 Return
c $F8B9 Drum sample players
D $F8B9 Used by #R$F82F.
R $F8B9 I:A Calling this <speed value> (8/3/1 seem to be the used values in practice)
@ $F8B9 label=b7_playdrum_X
C $F8B9,3 Load address of drum X data
C $F8BC,2 94 sample bytes
C $F8BE,2 Jump to es_playdrum_go
@ $F8C0 label=b7_playdrum_Y
C $F8C0,3 Load address of drum Y data
C $F8C3,2 160 sample bytes
@ $F8C5 label=es_playdrum_go
C $F8C5,3 Self modify #R$F8D0 below, setting the <speed value> to #REGa as passed in
C $F8C8,5 Self modify #R$F8A4 above, setting the <drum is playing> flag to 1
C $F8CD,2 Jump to b7pd_go
@ $F8CF label=b7pd_bank_go
C $F8CF,1 Bank
@ $F8D0 label=b7pd_go
C $F8D0,2 <speed value> iterations. Self modified by #R$F8C5 above
@ $F8D2 label=b7pd_loop
C $F8D2,2 Set speaker flag
C $F8D4,1 Delay
C $F8D5,2 Test a sample bit
C $F8D7,4 Set speaker flag to match sample bit
@ $F8DB label=b7pd_output_bit
C $F8DB,2 Output it
C $F8DD,2 Rotate sample byte in-place
C $F8DF,2 Loop to pd_loop while #REGb
C $F8E1,1 Move to next sample byte
C $F8E2,1 Decrement sample bytes remaining
C $F8E3,2 Jump to pd_end_of_sample if no bytes remain
C $F8E5,3 Read A from the <interrupt flag> at #R$F8AA
C $F8E8,1 Set flags
C $F8E9,3 Loop to pd_go if clear
C $F8EC,1 Otherwise unbank
C $F8ED,1 Return
@ $F8EE label=b7pd_end_of_sample
C $F8EE,4 Self modify #R$F8A4 above, clearing the <drum is playing> flag now that the sample has finished
C $F8F2,3 Jump to pm_wait_for_interrupt
b $F8F5 Drum 2 sample
D $F8F5 94 bytes of 1-bit PCM, played by b7_playdrum_X (#R$F8B9). The player rotates each byte in place with RLC (HL), so a sample interrupted part way through is left rotated until eight further rotations bring it back round.
@ $F8F5 label=b7_drum2
B $F8F5,94,8*11,6 Drum 2 sample
b $F953 Drum 1 sample
D $F953 160 bytes of 1-bit PCM, played by b7_playdrum_Y (#R$F8C0) and rotated in place the same way as #R$F8F5.
@ $F953 label=b7_drum1
B $F953,160,2,8*19,6 Drum 1 sample
c $F9F3 White noise generator
D $F9F3 Bank 7's own copy of #R$F0C6@main, byte for byte identical to it apart from the two references to the <interrupt flag>, which point at this bank's copy at #R$F8AA rather than the main bank's. Reached as instrument 3 from #R$F893.
D $F9F3 The disassembler took these 56 bytes for data; they are code.
R $F9F3 I:A Duration (3 or 9 in practice)
@ $F9F3 label=b7_play_noise
C $F9F3,1 Set #REGe to duration counter
@ $F9F4 label=b7n_outer_loop
C $F9F4,2 Set #REGd to inner counter 50
@ $F9F6 label=b7n_loop
C $F9F6,3 Point at rng_seed
C $F9F9,3 Increment first byte of rng_seed by 3
C $F9FC,1 Load it into #REGb
C $F9FD,1 Advance to second byte of rng_seed
N $F9FE Note that this is a different order of operations than in rng/#R$961B@main.
C $F9FE,4 Subtract 141 from second byte of rng_seed
C $FA02,1 Add first and second rng_seed bytes together
C $FA03,1 Advance to third byte of seed
C $FA04,1 Rotate #REGa left by 1
C $FA05,2 Rotate third byte of seed right by 1
C $FA07,1 Add it to #REGa
C $FA08,1 Write it back
C $FA09,2 Take a tap off at bit 4
C $FA0B,2 Jump to b7n_continue if zero
@ $FA0D label=b7n_make_noise
C $FA0D,4 Delay for (24 - #REGe) iterations
@ $FA11 label=b7n_delay_loop_1
C $FA11,2 Delay
C $FA13,4 Set EAR + MIC bits
C $FA17,1 Delay for #REGe iterations
@ $FA18 label=b7n_delay_loop_2
C $FA18,2 Delay
C $FA1A,3 Clear EAR + MIC bits
@ $FA1D label=b7n_continue
C $FA1D,1 Decrement inner counter
C $FA1E,2 Jump to b7n_loop if non-zero
N $FA20 The same dead interrupt check the main bank's copy carries: AND A clears carry, so the RET C below is never taken.
C $FA20,3 Read A from the <interrupt flag> at #R$F8AA
C $FA23,1 Set flags
C $FA24,1 Bug: Carry is cleared by AND A so this makes no sense
C $FA25,1 Decrement duration counter
C $FA26,2 Jump to b7n_outer_loop if non-zero
C $FA28,3 Exit via b7pm_wait_for_interrupt (#R$F8AA)
b $FA2B End screen music patterns
D $FA2B Ten (repetition count, data offset) pairs terminated by $FF, then a restart pointer. #R$F7EF points the engine at this table and #R$F7FF walks it; each offset indexes #R$FA42. The restart pointer holds $F54E, which is #R$FA3D once the bank has been relocated -- the last pair -- so the tune plays through once and then repeats its final pattern.
@ $FA2B label=b7_music_patterns
B $FA2B,20,2,8*2,2 Repetition count, data offset
B $FA3F,1,1 End of patterns
W $FA40,2,2 Restart at #R$FA3D
b $FA42 End screen music data
D $FA42 The byte streams the patterns index into, in the same 0bdaaaaiii form the in-game engine uses: bit 7 is an extra delay flag, bits 6-3 are the instrument argument and bits 2-0 the instrument (0 = silence, 1 = drum 2, 2 = drum 1, 3 = noise). A byte of 1 ends a pattern. The first byte of each stream is the note delay, not a note.
@ $FA42 label=b7_music_data
B $FA42,174,3,8*21,3
b $FAF0 Bank 7 tail (unidentified)
B $FAF0,1296,8
