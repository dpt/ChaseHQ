> $C000 ; ChaseHQ-128K-bank-1.ctl
> $C000 ;
> $C000 ; Bank 1 holds the data for stages 1 and 2.
> $C000 ;
> $C000
@ $C000 org
b $C000 [Stage 1] Horizon graphic
D $C000 #HTML[#CALL:graphic($C000,80,24,0,1)]
@ $C000 label=bitmap_horizon
B $C000,240,8 Horizon backdrop (80x24). Non-masked, inverted, bitmap data. Varies per stage.
b $C0F0 [Stage 1] Per-stage data
@ $C0F0 label=addrof_perp_mugshot_attributes
W $C0F0,2,2 Address of Ralph's mugshot attributes
@ $C0F2 label=addrof_perp_mugshot_bitmap
W $C0F2,2,2 Address of Ralph's mugshot bitmap
@ $C0F4 label=ground_colour
W $C0F4,2,2 Screen attributes used for the ground colour (a pair of matching bytes)
@ $C0F6 label=addrof_hittable_objects
W $C0F6,2,2 Loaded by #R$AC1F. Address of table of LODs for tumbleweeds, barriers.
@ $C0F8 label=addrof_right_hand_handlers
@ $C0F8 ssub=DEFW right_hand_handlers - 7
W $C0F8,2,2 Loaded by #R$900F. Address of an array of 7 byte entries. Points 7 bytes earlier to permit 1-indexing.
@ $C0FA label=addrof_right_hand_objects
@ $C0FA ssub=DEFW right_hand_graphics_defs - 7
W $C0FA,2,2 Loaded by #R$A465. Address of graphics entry 1. Points 7 bytes earlier to permit 1-indexing.
@ $C0FC label=addrof_right_hand_short_pole_object
W $C0FC,2,2 Loaded by #R$A53F. Address of graphics entry 3.
@ $C0FE label=addrof_turn_sign_handlers
W $C0FE,2,2 Address of turn sign arg and handler address.
@ $C100 label=addrof_left_hand_objects
@ $C100 ssub=DEFW left_hand_graphics_defs - 7
W $C100,2,2 Loaded by #R$A490. Address of graphics entry 10. Points 7 bytes earlier to permit 1-indexing.
@ $C102 label=addrof_left_hand_short_pole_object
W $C102,2,2 Loaded by #R$A55C. Address of graphics entry 12.
@ $C104 label=addrof_perp_description
W $C104,2,2 Address of Nancy's perp description.
@ $C106 label=addrof_arrest_messages
W $C106,2,2 Address of arrest messages.
@ $C108 label=addrof_helicopter_stuff_1
W $C108,2,2 Loaded by #R$AA69. Helicopter related.
@ $C10A label=addrof_helicopter_stuff_2
W $C10A,2,2 Loaded by #R$AA6E. Helicopter related.
w $C10C [Stage 1] Table of addresses of LODs
D $C10C LODs = Level Of Detail - a set of sprites of various sizes representing the same object.
@ $C10C label=lods_stones
W $C10C,2,2 Address of LODs for stones.
@ $C10E label=lods_dust
W $C10E,2,2 Address of LODs for dust.
@ $C110 label=lods_perp_car
W $C110,2,2 Address of LODs for car (perp's car).
@ $C112 label=lods_vehicles
W $C112,2,2 Address of LODs for lambo.
W $C114,2,2 Address of LODs for truck.
W $C116,2,2 Address of LODs for lambo (again).
W $C118,2,2 Address of LODs for car (generic car).
b $C11A [Stage 1] Per-stage difficulty settings
@ $C11A label=car_spawn_rate
B $C11A,1,1 How often cars spawn. Lower values spawn cars more often.
@ $C11B label=smash_5d1b
B $C11B,1,1 Loaded by #R$A6A7. Used by smash_handler.
@ $C11C label=smash_5d1c
B $C11C,1,1 Loaded by #R$A759.
w $C11D [Stage 1] Per-stage setup data
@ $C11D label=stage_set_up_data
W $C11D,2,2 road_pos
N $C11F The following all point a byte earlier than the real data's start.
@ $C11F ssub=DEFW map_start_curvature - 1
W $C11F,2,2 -> Start stretch, curvature
@ $C121 ssub=DEFW map_start_height - 1
W $C121,2,2 -> Start stretch, height
@ $C123 ssub=DEFW map_start_lanes - 1
W $C123,2,2 -> Start stretch, lanes
@ $C125 ssub=DEFW map_start_rightobjs - 1
W $C125,2,2 -> Start stretch, right-side objects
@ $C127 ssub=DEFW map_start_leftobjs - 1
W $C127,2,2 -> Start stretch, left-side objects
@ $C129 ssub=DEFW map_start_hazards - 1
W $C129,2,2 -> Start stretch, hazards
w $C12B [Stage 1] Per-stage attract mode data
@ $C12B label=attract_data
W $C12B,2,2 road_pos
N $C12D These all point a byte earlier than the real data start point.
@ $C12D ssub=DEFW map_loop_curvature - 1
W $C12D,2,2 -> Loop section, curvature
@ $C12F ssub=DEFW map_loop_height - 1
W $C12F,2,2 -> Loop section, height
@ $C131 ssub=DEFW map_loop_lanes - 1
W $C131,2,2 -> Loop section, lanes
@ $C133 ssub=DEFW map_loop_rightobjs - 1
W $C133,2,2 -> Loop section, right-side objects
@ $C135 ssub=DEFW map_loop_leftobjs - 1
W $C135,2,2 -> Loop section, left-side objects
@ $C137 ssub=DEFW map_loop_hazards - 1
W $C137,2,2 -> Loop section, hazards
b $C139 [Stage 1] Nancy's perp description
@ $C139 label=perp_description
B $C139,1,1 Nancy ($01)
W $C13A,2,2 Points at "THIS IS NANCY..."
W $C13C,2,2 Points at "EMERGENCY HERE..."
W $C13E,2,2 Points at "IS FLEEING ..."
W $C140,2,2 Points at "VEHICLE IS..."
B $C142,1,1 (pause?)
W $C143,2,2 -> Random choice of ("WE READ..." / "ROGER!" / "GOTCHA...")
@ $C145 label=perp_description_1
T $C145,40,39:n1 "THIS IS NANCY AT CHASE H.Q. WE'VE GOT AN"
@ $C16D label=perp_description_2
T $C16D,40,39:n1 "EMERGENCY HERE. RALPH THE IDAHO SLASHER,"
@ $C195 label=perp_description_3
T $C195,42,41:n1 "IS FLEEING TOWARDS THE SUBURBS. THE TARGET"
@ $C1BF label=perp_description_4
T $C1BF,46,45:n1 "VEHICLE IS A WHITE BRITISH SPORTS CAR... OVER."
b $C1ED [Stage 1] Arrest messages
@ $C1ED label=arrest_messages
B $C1ED,1,1 Frame delay until first message?
B $C1EE,1,1 Frame delay until next message?
B $C1EF,1,1 Flags (?)
B $C1F0,1,1 Attribute (black)
W $C1F1,2,2 Back buffer address
W $C1F3,2,2 Attribute address
T $C1F5,27,26:n1 "OK! YOU ARE UNDER ARREST ON"
B $C210,1,1 Frame delay until next message?
B $C211,1,1 Flags (?)
B $C212,1,1 Attribute (black)
W $C213,2,2 Back buffer address
W $C215,2,2 Attribute address
T $C217,26,25:n1 "SUSPICION OF FIRST DEGREE "
B $C231,1,1 Frame delay until next message?
B $C232,1,1 Flags (?)
B $C233,1,1 Attribute (black)
W $C234,2,2 Back buffer address
W $C236,2,2 Attribute address
T $C238,6,5:n1 "MURDER"
B $C23E,1,1 Frame delay until next message?
b $C23F [Stage 1] Graphics definitions
D $C23F All are stored inverted except where noted.
@ $C23F label=graphics_defs
B $C23F,1,1 Pointed to by #R$5CFA (when relocated)
N $C240 Hittable hazards: tumbleweed and barrier.
@ $C240 label=hittable_objects_defs
B $C240,1,1 Pointed to by #R$5CF6 (when relocated)
W $C241,2,2 Address of tumbleweed_lods table
B $C243,1,1 ?
W $C244,2,2 Address of barrier_lods table
N $C246 Right hand side objects.
N $C246 Entry 1 (tunnel light)
@ $C246 label=right_hand_graphics_defs
B $C246,3,3 (hit coord max, hit coord min, ?)
@ $C249 label=right_hand_handlers
W $C249,2,2 Arg for routine passed in DE
W $C24B,2,2 -> Routine at #R$9252
N $C24D Entry 2 (empty)
B $C24D,3,3
W $C250,4,2
N $C254 Entry 3 (short pole)
@ $C254 label=right_hand_short_pole_object
B $C254,3,3
W $C257,2,2 Arg for routine passed in DE
W $C259,2,2 -> Routine at #R$9171
N $C25B Entry 4 (tree, right hand side)
B $C25B,3,3
W $C25E,2,2 Arg for routine passed in DE
W $C260,2,2 -> Routine at #R$9171.
N $C262 Entry 5 (bush)
B $C262,3,3
W $C265,2,2 Arg for routine passed in DE
W $C267,2,2 -> Routine at #R$9171
N $C269 Entry 6 (street lamp)
B $C269,3,3
W $C26C,2,2 Arg for routine passed in DE
W $C26E,2,2 -> Routine at #R$9171
N $C270 Entry 7 (telegraph pole)
B $C270,3,3
W $C273,2,2 Arg for routine passed in DE
W $C275,2,2 -> Routine at #R$9171
N $C277 Entry 8 (turn sign, pointing left)
B $C277,3,3
W $C27A,2,2 Arg for routine passed in DE
W $C27C,2,2 -> Routine at #R$92E1
N $C27E Entry 9 (turn sign, pointing right)
B $C27E,3,3
@ $C281 label=right_hand_turn_sign_handlers
W $C281,2,2 -> -> turn_sign_lods
W $C283,2,2 -> Routine at #R$92E1
N $C285 Left hand side objects.
N $C285 Entry 10 (tunnel light)
@ $C285 label=left_hand_graphics_defs
B $C285,3,3
W $C288,2,2 Arg for routine passed in DE
W $C28A,2,2 -> Routine at #R$924D
N $C28C Entry 11 (empty)
B $C28C,3,3
W $C28F,4,2
N $C293 Entry 12 (short pole)
@ $C293 label=left_hand_short_pole_object
B $C293,3,3
W $C296,2,2 Arg for routine passed in DE
W $C298,2,2 -> Routine at #R$916C
N $C29A Entry 13 (tree, left hand side)
B $C29A,3,3
W $C29D,2,2 Arg for routine passed in DE
W $C29F,2,2 -> Routine at #R$916C
N $C2A1 Entry 14 (bush)
B $C2A1,3,3
W $C2A4,2,2 Arg for routine passed in DE
W $C2A6,2,2 -> Routine at #R$916C
N $C2A8 Entry 15 (street lamp)
B $C2A8,3,3
W $C2AB,2,2 Arg for routine passed in DE
W $C2AD,2,2 -> Routine at #R$916C
N $C2AF Entry 16 (telegraph pole)
B $C2AF,3,3
W $C2B2,2,2 Arg for routine passed in DE
W $C2B4,2,2 -> Routine at #R$916C
N $C2B6 Entry 17 (turn sign, pointing left)
B $C2B6,3,3
W $C2B9,2,2 Arg for routine passed in DE
W $C2BB,2,2 -> Routine at #R$9278
N $C2BD Entry 18 (turn sign, pointing right)
B $C2BD,3,3
W $C2C0,2,2 Arg for routine passed in DE
W $C2C2,2,2 -> Routine at #R$9278
b $C2C4 [Stage 1] Map: Start stretch
N $C2C4 Start stretch
N $C2C4 Start stretch, curvature
@ $C2C4 label=map_start_curvature
B $C2C4,32,8
N $C2E4 Start stretch, height
@ $C2E4 label=map_start_height
B $C2E4,43,8*5,3
N $C30F Start stretch, lanes
@ $C30F label=map_start_lanes
B $C30F,14,8,6
N $C31D Start stretch, hazards
@ $C31D label=map_start_hazards
B $C31D,13,8,5
N $C32A Start stretch, left-side objects
@ $C32A label=map_start_leftobjs
B $C32A,78,8*9,6
N $C378 Start stretch, right-side objects
@ $C378 label=map_start_rightobjs
B $C378,90,8*11,2
b $C3D2 [Stage 1] Map: Left fork
N $C3D2 Left fork
N $C3D2 Left fork, curvature
@ $C3D2 label=map_left_curvature
B $C3D2,20,8*2,4
N $C3E6 Left fork, height
@ $C3E6 label=map_left_height
B $C3E6,29,8*3,5
N $C403 Left fork, lanes
@ $C403 label=map_left_lanes
B $C403,6,6
N $C409 Left fork, hazards
@ $C409 label=map_left_hazards
B $C409,8,8
N $C411 Left fork, left-side objects
@ $C411 label=map_left_leftobjs
B $C411,65,8*8,1
N $C452 Left fork, right-side objects
@ $C452 label=map_left_rightobjs
B $C452,54,8*6,6
b $C488 [Stage 1] Map: Right fork (dirt track)
N $C488 Right fork (dirt track), curvature
@ $C488 label=map_right_curvature
B $C488,27,8*3,3
N $C4A3 Right fork (dirt track), height
@ $C4A3 label=map_right_height
B $C4A3,66,8*8,2
N $C4E5 Right fork (dirt track), lanes
@ $C4E5 label=map_right_lanes
B $C4E5,10,8,2
N $C4EF Right fork (dirt track), hazards
@ $C4EF label=map_right_hazards
B $C4EF,26,8*3,2
N $C509 Right fork (dirt track), left-side objects
@ $C509 label=map_right_leftobjs
B $C509,58,8*7,2
N $C543 Right fork (dirt track), right-side objects
@ $C543 label=map_right_rightobjs
B $C543,48,8
b $C573 [Stage 1] Map: Tunnel section
N $C573 Tunnel section, curvature
@ $C573 label=map_tunnel_curvature
B $C573,22,8*2,6
N $C589 Tunnel section, height
@ $C589 label=map_tunnel_height
B $C589,28,8*3,4
N $C5A5 Tunnel section, lanes
@ $C5A5 label=map_tunnel_lanes
B $C5A5,20,8*2,4
N $C5B9 Tunnel section, hazards
@ $C5B9 label=map_tunnel_hazards
B $C5B9,5,5
N $C5BE Tunnel section, left-side objects
@ $C5BE label=map_tunnel_leftobjs
B $C5BE,11,8,3
N $C5C9 Tunnel section, right-side objects
@ $C5C9 label=map_tunnel_rightobjs
B $C5C9,11,8,3
b $C5D4 [Stage 1] Map: Loop section
N $C5D4 Loop section, curvature
@ $C5D4 label=map_loop_curvature
B $C5D4,53,8*6,5
N $C609 Loop section, height
@ $C609 label=map_loop_height
B $C609,84,8*10,4
N $C65D Loop section, lanes
@ $C65D label=map_loop_lanes
B $C65D,26,8*3,2
N $C677 Loop section, hazards
@ $C677 label=map_loop_hazards
B $C677,29,8*3,5
N $C694 Loop section, left-side objects
@ $C694 label=map_loop_leftobjs
B $C694,128,8
N $C714 Loop section, right-side objects
@ $C714 label=map_loop_rightobjs
B $C714,118,8*14,6
b $C78A [Stage 1] Ralph the Idaho Slasher's mugshot
N $C78A Bitmap data for Ralph the Idaho Slasher's mugshot (32x40). Stored top-down.
N $C78A #HTML[#CALL:face($C78A)]
@ $C78A label=bitmap_ralph
B $C78A,160,4
N $C82A Attribute data for Ralph the Idaho Slasher's mugshot (4x5). Stored top-down.
@ $C82A label=attrs_ralph
B $C82A,20,4
b $C83E [Stage 1] LODs
D $C83E LOD = Level of Detail. These structures collect together the variously sized versions of the same game object.
N $C83E Lamborghini LOD 1
@ $C83E label=lambo_lods
B $C83E,1,1 Width (bytes)
B $C83F,1,1 Flags
B $C840,1,1 Height (pixels)
W $C841,2,2 Bitmap
W $C843,2,2 Pre-shifted bitmap
N $C845 Lamborghini LOD 2
B $C845,1,1 Width (bytes)
B $C846,1,1 Flags
B $C847,1,1 Height (pixels)
W $C848,2,2 Bitmap
W $C84A,2,2 Pre-shifted bitmap
N $C84C Lamborghini LOD 3
B $C84C,1,1 Width (bytes)
B $C84D,1,1 Flags
B $C84E,1,1 Height (pixels)
W $C84F,2,2 Bitmap
W $C851,2,2 Pre-shifted bitmap
N $C853 Lamborghini LOD 4
B $C853,1,1 Width (bytes)
B $C854,1,1 Flags
B $C855,1,1 Height (pixels)
W $C856,2,2 Bitmap
W $C858,2,2 Pre-shifted bitmap
N $C85A Lamborghini LOD 5
B $C85A,1,1 Width (bytes)
B $C85B,1,1 Flags
B $C85C,1,1 Height (pixels)
W $C85D,2,2 Bitmap
W $C85F,2,2 Pre-shifted bitmap
N $C861 Lamborghini LOD 6
B $C861,1,1 Width (bytes)
B $C862,1,1 Flags
B $C863,1,1 Height (pixels)
W $C864,2,2 Bitmap
W $C866,2,2 Pre-shifted bitmap
N $C868 Truck LOD 1
@ $C868 label=truck_lods
B $C868,1,1 Width (bytes)
B $C869,1,1 Flags
B $C86A,1,1 Height (pixels)
W $C86B,2,2 Bitmap
W $C86D,2,2 Pre-shifted bitmap
N $C86F Truck LOD 2
B $C86F,1,1 Width (bytes)
B $C870,1,1 Flags
B $C871,1,1 Height (pixels)
W $C872,2,2 Bitmap
W $C874,2,2 Pre-shifted bitmap
N $C876 Truck LOD 3
B $C876,1,1 Width (bytes)
B $C877,1,1 Flags
B $C878,1,1 Height (pixels)
W $C879,2,2 Bitmap
W $C87B,2,2 Pre-shifted bitmap
N $C87D Truck LOD 4
B $C87D,1,1 Width (bytes)
B $C87E,1,1 Flags
B $C87F,1,1 Height (pixels)
W $C880,2,2 Bitmap
W $C882,2,2 Pre-shifted bitmap
N $C884 Truck LOD 5
B $C884,1,1 Width (bytes)
B $C885,1,1 Flags
B $C886,1,1 Height (pixels)
W $C887,2,2 Bitmap
W $C889,2,2 Pre-shifted bitmap
N $C88B Truck LOD 6
B $C88B,1,1 Width (bytes)
B $C88C,1,1 Flags
B $C88D,1,1 Height (pixels)
W $C88E,2,2 Bitmap
W $C890,2,2 Pre-shifted bitmap
N $C892 Car LOD 1
@ $C892 label=car_lods
B $C892,1,1 Width (bytes)
B $C893,1,1 Flags
B $C894,1,1 Height (pixels)
W $C895,2,2 Bitmap
W $C897,2,2 Pre-shifted bitmap
N $C899 Car LOD 2
B $C899,1,1 Width (bytes)
B $C89A,1,1 Flags
B $C89B,1,1 Height (pixels)
W $C89C,2,2 Bitmap
W $C89E,2,2 Pre-shifted bitmap
N $C8A0 Car LOD 3
B $C8A0,1,1 Width (bytes)
B $C8A1,1,1 Flags
B $C8A2,1,1 Height (pixels)
W $C8A3,2,2 Bitmap
W $C8A5,2,2 Pre-shifted bitmap
N $C8A7 Car LOD 4
B $C8A7,1,1 Width (bytes)
B $C8A8,1,1 Flags
B $C8A9,1,1 Height (pixels)
W $C8AA,2,2 Bitmap
W $C8AC,2,2 Pre-shifted bitmap
N $C8AE Car LOD 5
B $C8AE,1,1 Width (bytes)
B $C8AF,1,1 Flags
B $C8B0,1,1 Height (pixels)
W $C8B1,2,2 Bitmap
W $C8B3,2,2 Pre-shifted bitmap
N $C8B5 Car LOD 6
B $C8B5,1,1 Width (bytes)
B $C8B6,1,1 Flags
B $C8B7,1,1 Height (pixels)
W $C8B8,2,2 Bitmap
W $C8BA,2,2 Pre-shifted bitmap
N $C8BC Bitmap: Lamborghini_1 (48x30)
N $C8BC #HTML[#CALL:graphic($C8BC,48,30,0,1)]
@ $C8BC label=bitmap_lamborghini_1
B $C8BC,180,6 Bitmap data
N $C970 Bitmap: Lamborghini_2 (40x22)
N $C970 #HTML[#CALL:graphic($C970,40,22,0,1)]
@ $C970 label=bitmap_lamborghini_2
B $C970,110,5 Bitmap data
N $C9DE Bitmap: Lamborghini_3 (24x15)
N $C9DE #HTML[#CALL:graphic($C9DE,24,15,0,1)]
@ $C9DE label=bitmap_lamborghini_3
B $C9DE,45,3 Bitmap data
N $CA0B Bitmap: Truck_1 (48x39)
N $CA0B #HTML[#CALL:graphic($CA0B,48,39,0,1)]
@ $CA0B label=bitmap_truck_1
B $CA0B,234,6 Bitmap data
N $CAF5 Bitmap: Truck_2 (40x29)
N $CAF5 #HTML[#CALL:graphic($CAF5,40,29,0,1)]
@ $CAF5 label=bitmap_truck_2
B $CAF5,145,5 Bitmap data
N $CB86 Bitmap: Truck_3 (24x20)
N $CB86 #HTML[#CALL:graphic($CB86,24,20,0,1)]
@ $CB86 label=bitmap_truck_3
B $CB86,60,3 Bitmap data
N $CBC2 Bitmap: Car_1 (48x31)
N $CBC2 #HTML[#CALL:graphic($CBC2,48,31,0,1)]
@ $CBC2 label=bitmap_car_1
B $CBC2,186,6 Bitmap data
N $CC7C Bitmap: Car_2 (40x22)
N $CC7C #HTML[#CALL:graphic($CC7C,40,22,0,1)]
@ $CC7C label=bitmap_car_2
B $CC7C,110,5 Bitmap data
N $CCEA Bitmap: Car_3 (24x16)
N $CCEA #HTML[#CALL:graphic($CCEA,24,16,0,1)]
@ $CCEA label=bitmap_car_3
B $CCEA,48,3 Bitmap data
N $CD1A Bitmap: Lamborghini_4 (24x8)
N $CD1A #HTML[#CALL:graphic($CD1A,24,8,1,1)]
@ $CD1A label=bitmap_lamborghini_4
B $CD1A,48,6 Masked bitmap data
N $CD4A Bitmap: Lamborghini_4 (24x8) pre-shifted
N $CD4A #HTML[#CALL:graphic($CD4A,24,8,1,1)]
@ $CD4A label=bitmap_lamborghini_4s
B $CD4A,48,6 Masked bitmap data
N $CD7A Bitmap: Truck_4 (16x12)
N $CD7A #HTML[#CALL:graphic($CD7A,16,12,1,1)]
@ $CD7A label=bitmap_truck_4
B $CD7A,48,4 Masked bitmap data
N $CDAA Bitmap: Truck_4 (16x12) pre-shifted
N $CDAA #HTML[#CALL:graphic($CDAA,16,12,1,1)]
@ $CDAA label=bitmap_truck_4s
B $CDAA,48,4 Masked bitmap data
N $CDDA Bitmap: Car_4 (24x9)
N $CDDA #HTML[#CALL:graphic($CDDA,24,9,1,1)]
@ $CDDA label=bitmap_car_4
B $CDDA,54,6 Masked bitmap data
N $CE10 Bitmap: Car_4 (24x9) pre-shifted
N $CE10 #HTML[#CALL:graphic($CE10,24,9,1,1)]
@ $CE10 label=bitmap_car_4s
B $CE10,54,6 Masked bitmap data
N $CE46 Stones LOD 1
@ $CE46 label=stones_lods
B $CE46,1,1 Width (bytes)
B $CE47,1,1 Flags
B $CE48,1,1 Height (pixels)
W $CE49,2,2 Bitmap
W $CE4B,2,2 Pre-shifted bitmap
N $CE4D Stones LOD 2
B $CE4D,1,1 Width (bytes)
B $CE4E,1,1 Flags
B $CE4F,1,1 Height (pixels)
W $CE50,2,2 Bitmap
W $CE52,2,2 Pre-shifted bitmap
N $CE54 Stones LOD 3
B $CE54,1,1 Width (bytes)
B $CE55,1,1 Flags
B $CE56,1,1 Height (pixels)
W $CE57,2,2 Bitmap
W $CE59,2,2 Pre-shifted bitmap
N $CE5B Stones LOD 4
B $CE5B,1,1 Width (bytes)
B $CE5C,1,1 Flags
B $CE5D,1,1 Height (pixels)
W $CE5E,2,2 Bitmap
W $CE60,2,2 Pre-shifted bitmap
N $CE62 Stones LOD 5
B $CE62,1,1 Width (bytes)
B $CE63,1,1 Flags
B $CE64,1,1 Height (pixels)
W $CE65,2,2 Bitmap
W $CE67,2,2 Pre-shifted bitmap
N $CE69 Stones LOD 6
B $CE69,1,1 Width (bytes)
B $CE6A,1,1 Flags
B $CE6B,1,1 Height (pixels)
W $CE6C,2,2 Bitmap
W $CE6E,2,2 Pre-shifted bitmap
N $CE70 Dust LOD 1
@ $CE70 label=dust_lods
B $CE70,1,1 Width (bytes)
B $CE71,1,1 Flags
B $CE72,1,1 Height (pixels)
W $CE73,2,2 Bitmap
W $CE75,2,2 Pre-shifted bitmap
N $CE77 Dust LOD 2
B $CE77,1,1 Width (bytes)
B $CE78,1,1 Flags
B $CE79,1,1 Height (pixels)
W $CE7A,2,2 Bitmap
W $CE7C,2,2 Pre-shifted bitmap
N $CE7E Dust LOD 3
B $CE7E,1,1 Width (bytes)
B $CE7F,1,1 Flags
B $CE80,1,1 Height (pixels)
W $CE81,2,2 Bitmap
W $CE83,2,2 Pre-shifted bitmap
N $CE85 Dust LOD 4
B $CE85,1,1 Width (bytes)
B $CE86,1,1 Flags
B $CE87,1,1 Height (pixels)
W $CE88,2,2 Bitmap
W $CE8A,2,2 Pre-shifted bitmap
N $CE8C Dust LOD 5
B $CE8C,1,1 Width (bytes)
B $CE8D,1,1 Flags
B $CE8E,1,1 Height (pixels)
W $CE8F,2,2 Bitmap
W $CE91,2,2 Pre-shifted bitmap
N $CE93 Dust LOD 6
B $CE93,1,1 Width (bytes)
B $CE94,1,1 Flags
B $CE95,1,1 Height (pixels)
W $CE96,2,2 Bitmap
W $CE98,2,2 Pre-shifted bitmap
N $CE9A Bitmap: Stones (16x5)
N $CE9A #HTML[#CALL:graphic($CE9A,16,5,1,1)]
@ $CE9A label=bitmap_stones_1
B $CE9A,20,4 Masked bitmap data
N $CEAE Bitmap: Stones (16x4)
N $CEAE #HTML[#CALL:graphic($CEAE,16,4,1,1)]
@ $CEAE label=bitmap_stones_2
B $CEAE,16,4 Masked bitmap data
N $CEBE Bitmap: Stones (16x4) pre-shifted
N $CEBE #HTML[#CALL:graphic($CEBE,16,4,1,1)]
@ $CEBE label=bitmap_stones_2s
B $CEBE,16,4 Masked bitmap data
N $CECE Bitmap: Stones (16x3)
N $CECE #HTML[#CALL:graphic($CECE,16,3,1,1)]
@ $CECE label=bitmap_stones_3
B $CECE,12,4 Masked bitmap data
N $CEDA Bitmap: Stones (16x3) pre-shifted
N $CEDA #HTML[#CALL:graphic($CEDA,16,3,1,1)]
@ $CEDA label=bitmap_stones_3s
B $CEDA,12,4 Masked bitmap data
N $CEE6 Bitmap: Stones (8x2)
N $CEE6 #HTML[#CALL:graphic($CEE6,8,2,1,1)]
@ $CEE6 label=bitmap_stones_4
B $CEE6,4,2 Masked bitmap data
N $CEEA Bitmap: Stones (8x2) pre-shifted
N $CEEA #HTML[#CALL:graphic($CEEA,8,2,1,1)]
@ $CEEA label=bitmap_stones_4s
B $CEEA,4,2 Masked bitmap data
N $CEEE Bitmap: Stones (8x1)
N $CEEE #HTML[#CALL:graphic($CEEE,8,1,1,1)]
@ $CEEE label=bitmap_stones_5
B $CEEE,2,2 Masked bitmap data
N $CEF0 Bitmap: Stones (8x1) pre-shifted
N $CEF0 #HTML[#CALL:graphic($CEF0,8,1,1,1)]
@ $CEF0 label=bitmap_stones_5s
B $CEF0,2,2 Masked bitmap data
N $CEF2 Bitmap: Dust (8x1)
N $CEF2 #HTML[#CALL:graphic($CEF2,8,1,1,1)]
@ $CEF2 label=bitmap_dust_1
B $CEF2,2,2 Masked bitmap data
N $CEF4 Bitmap: Dust (8x1) pre-shifted
N $CEF4 #HTML[#CALL:graphic($CEF4,8,1,1,1)]
@ $CEF4 label=bitmap_dust_1s
B $CEF4,2,2 Masked bitmap data
@ $CEF6 label=turn_sign_right
W $CEF6,2,2 -> turn_sign_lods
B $CEF8,20,2 Read by #R$928D and #R$92F6
N $CF0C Referenced by graphic entry 8 and 17
@ $CF0C label=turn_sign_left
W $CF0C,2,2 -> turn_sign_lods
B $CF0E,20,2
N $CF22 Turn sign LOD 1
@ $CF22 label=turn_sign_lods
B $CF22,1,1 Width (bytes)
B $CF23,1,1 Flags
B $CF24,1,1 Height (pixels)
W $CF25,2,2 Bitmap
W $CF27,2,2 Pre-shifted bitmap
N $CF29 Turn sign LOD 2
B $CF29,1,1 Width (bytes)
B $CF2A,1,1 Flags
B $CF2B,1,1 Height (pixels)
W $CF2C,2,2 Bitmap
W $CF2E,2,2 Pre-shifted bitmap
N $CF30 Turn sign LOD 3
B $CF30,1,1 Width (bytes)
B $CF31,1,1 Flags
B $CF32,1,1 Height (pixels)
W $CF33,2,2 Bitmap
W $CF35,2,2 Pre-shifted bitmap
N $CF37 Turn sign LOD 4
B $CF37,1,1 Width (bytes)
B $CF38,1,1 Flags
B $CF39,1,1 Height (pixels)
W $CF3A,2,2 Bitmap
W $CF3C,2,2 Pre-shifted bitmap
N $CF3E Turn sign LOD 5
B $CF3E,1,1 Width (bytes)
B $CF3F,1,1 Flags
B $CF40,1,1 Height (pixels)
W $CF41,2,2 Bitmap
W $CF43,2,2 Pre-shifted bitmap
N $CF45 Turn sign LOD 1 (flipped)
B $CF45,1,1 Width (bytes)
B $CF46,1,1 Flags
B $CF47,1,1 Height (pixels)
W $CF48,2,2 Bitmap
W $CF4A,2,2 Pre-shifted bitmap
N $CF4C Turn sign LOD 2 (flipped)
B $CF4C,1,1 Width (bytes)
B $CF4D,1,1 Flags
B $CF4E,1,1 Height (pixels)
W $CF4F,2,2 Bitmap
W $CF51,2,2 Pre-shifted bitmap
N $CF53 Turn sign LOD 3 (flipped)
B $CF53,1,1 Width (bytes)
B $CF54,1,1 Flags
B $CF55,1,1 Height (pixels)
W $CF56,2,2 Bitmap
W $CF58,2,2 Pre-shifted bitmap
N $CF5A Turn sign LOD 4 (flipped)
B $CF5A,1,1 Width (bytes)
B $CF5B,1,1 Flags
B $CF5C,1,1 Height (pixels)
W $CF5D,2,2 Bitmap
W $CF5F,2,2 Pre-shifted bitmap
N $CF61 Turn sign LOD 5 (flipped)
B $CF61,1,1 Width (bytes)
B $CF62,1,1 Flags
B $CF63,1,1 Height (pixels)
W $CF64,2,2 Bitmap
W $CF66,2,2 Pre-shifted bitmap
N $CF68 Turn right sign (32x40)
N $CF68 #HTML[#CALL:graphic($CF68,32,40,0,1)]
@ $CF68 label=bitmap_turnsign_1
B $CF68,160,4 Bitmap data
N $D008 Turn right sign (24x30)
N $D008 #HTML[#CALL:graphic($D008,24,30,0,1)]
@ $D008 label=bitmap_turnsign_2
B $D008,90,3 Bitmap data
N $D062 Turn right sign (16x20)
N $D062 #HTML[#CALL:graphic($D062,16,20,0,1)]
@ $D062 label=bitmap_turnsign_3
B $D062,40,2 Bitmap data
N $D08A Turn right sign (16x16)
N $D08A #HTML[#CALL:graphic($D08A,16,16,1,1)]
@ $D08A label=bitmap_turnsign_4
B $D08A,64,4 Masked bitmap data
N $D0CA Turn right sign (16x13)
N $D0CA #HTML[#CALL:graphic($D0CA,16,13,1,1)]
@ $D0CA label=bitmap_turnsign_5
B $D0CA,52,2 Masked bitmap data
N $D0FE Turn right sign (16x13) pre-shifted
N $D0FE #HTML[#CALL:graphic($D0FE,16,13,1,1)]
@ $D0FE label=bitmap_turnsign_5s
B $D0FE,52,2 Masked bitmap data
N $D132 Turn right sign (16x10)
N $D132 #HTML[#CALL:graphic($D132,16,10,1,1)]
@ $D132 label=bitmap_turnsign_6
B $D132,40,4 Masked bitmap data
N $D15A Turn right sign (16x10) pre-shifted
N $D15A #HTML[#CALL:graphic($D15A,16,10,1,1)]
@ $D15A label=bitmap_turnsign_6s
B $D15A,40,4 Masked bitmap data
N $D182 Tumbleweed LOD 1
@ $D182 label=tumbleweed_lods
B $D182,1,1 Width (bytes)
B $D183,1,1 Flags
B $D184,1,1 Height (pixels)
W $D185,2,2 Bitmap
W $D187,2,2 Pre-shifted bitmap
N $D189 Tumbleweed LOD 2
B $D189,1,1 Width (bytes)
B $D18A,1,1 Flags
B $D18B,1,1 Height (pixels)
W $D18C,2,2 Bitmap
W $D18E,2,2 Pre-shifted bitmap
N $D190 Tumbleweed LOD 3
B $D190,1,1 Width (bytes)
B $D191,1,1 Flags
B $D192,1,1 Height (pixels)
W $D193,2,2 Bitmap
W $D195,2,2 Pre-shifted bitmap
N $D197 Tumbleweed LOD 4
B $D197,1,1 Width (bytes)
B $D198,1,1 Flags
B $D199,1,1 Height (pixels)
W $D19A,2,2 Bitmap
W $D19C,2,2 Pre-shifted bitmap
N $D19E Tumbleweed LOD 5
B $D19E,1,1 Width (bytes)
B $D19F,1,1 Flags
B $D1A0,1,1 Height (pixels)
W $D1A1,2,2 Bitmap
W $D1A3,2,2 Pre-shifted bitmap
N $D1A5 Tumbleweed LOD 6
B $D1A5,1,1 Width (bytes)
B $D1A6,1,1 Flags
B $D1A7,1,1 Height (pixels)
W $D1A8,2,2 Bitmap
W $D1AA,2,2 Pre-shifted bitmap
N $D1AC Bitmap: Tumbleweed_1 (16x16)
N $D1AC #HTML[#CALL:graphic($D1AC,16,16,0,1)]
@ $D1AC label=bitmap_tumbleweed_1
B $D1AC,32,2 Bitmap data
N $D1CC Bitmap: Tumbleweed_2 (16x11)
N $D1CC #HTML[#CALL:graphic($D1CC,16,11,0,1)]
@ $D1CC label=bitmap_tumbleweed_2
B $D1CC,22,2 Bitmap data
N $D1E2 Bitmap: Tumbleweed_3 (8x9)
N $D1E2 #HTML[#CALL:graphic($D1E2,8,9,0,1)]
@ $D1E2 label=bitmap_tumbleweed_3
B $D1E2,9,1 Bitmap data
N $D1EB Bitmap: Tumbleweed_4 (8x7)
N $D1EB #HTML[#CALL:graphic($D1EB,8,7,0,1)]
@ $D1EB label=bitmap_tumbleweed_4
B $D1EB,7,1 Bitmap data
N $D1F2 Barrier LOD 1
@ $D1F2 label=barrier_lods
B $D1F2,1,1 Width (bytes)
B $D1F3,1,1 Flags
B $D1F4,1,1 Height (pixels)
W $D1F5,2,2 Bitmap
W $D1F7,2,2 Pre-shifted bitmap
N $D1F9 Barrier LOD 2
B $D1F9,1,1 Width (bytes)
B $D1FA,1,1 Flags
B $D1FB,1,1 Height (pixels)
W $D1FC,2,2 Bitmap
W $D1FE,2,2 Pre-shifted bitmap
N $D200 Barrier LOD 3
B $D200,1,1 Width (bytes)
B $D201,1,1 Flags
B $D202,1,1 Height (pixels)
W $D203,2,2 Bitmap
W $D205,2,2 Pre-shifted bitmap
N $D207 Barrier LOD 4
B $D207,1,1 Width (bytes)
B $D208,1,1 Flags
B $D209,1,1 Height (pixels)
W $D20A,2,2 Bitmap
W $D20C,2,2 Pre-shifted bitmap
N $D20E Barrier LOD 5
B $D20E,1,1 Width (bytes)
B $D20F,1,1 Flags
B $D210,1,1 Height (pixels)
W $D211,2,2 Bitmap
W $D213,2,2 Pre-shifted bitmap
N $D215 Barrier LOD 6
B $D215,1,1 Width (bytes)
B $D216,1,1 Flags
B $D217,1,1 Height (pixels)
W $D218,2,2 Bitmap
W $D21A,2,2 Pre-shifted bitmap
N $D21C Bitmap: Barrier (32x17)
N $D21C #HTML[#CALL:graphic($D21C,32,17,0,1)]
@ $D21C label=bitmap_barrier_1
B $D21C,68,4 Bitmap data
N $D260 Bitmap: Barrier (24x13)
N $D260 #HTML[#CALL:graphic($D260,24,13,0,1)]
@ $D260 label=bitmap_barrier_2
B $D260,39,3 Bitmap data
N $D287 Bitmap: Barrier (16x9)
N $D287 #HTML[#CALL:graphic($D287,16,9,0,1)]
@ $D287 label=bitmap_barrier_3
B $D287,18,2 Bitmap data
N $D299 Bitmap: Barrier (16x7)
N $D299 #HTML[#CALL:graphic($D299,16,7,1,1)]
@ $D299 label=bitmap_barrier_4
B $D299,28,4 Masked bitmap data
N $D2B5 Bitmap: Barrier (16x7) pre-shifted
N $D2B5 #HTML[#CALL:graphic($D2B5,16,7,1,1)]
@ $D2B5 label=bitmap_barrier_4s
B $D2B5,28,4 Masked bitmap data
@ $D2D1 label=stretchy_streetlamp_right
B $D2D1,1,1
W $D2D2,2,2
B $D2D4,1,1
W $D2D5,2,2
B $D2D7,1,1
W $D2D8,2,2
N $D2DA 0 => draws loads 1 => draws none  (1 is terminator) 2 => draws it once 3 => draws at least 3/4 4+ => draws none
B $D2DA,1,1 flag meaning draw once, or repeat, or?
W $D2DB,2,2 top
B $D2DD,1,1 terminator
@ $D2DE label=stretchy_streetlamp_left
B $D2DE,1,1
W $D2DF,2,2
B $D2E1,1,1
W $D2E2,2,2
B $D2E4,1,1
W $D2E5,2,2
B $D2E7,1,1
W $D2E8,2,2
B $D2EA,1,1
@ $D2EB label=streetlamptop_6eeb
W $D2EB,2,2 Address of another LOD table
B $D2ED,20,2
@ $D301 label=streetlamptop_flipped_6f01
W $D301,2,2 Address of another LOD table
B $D303,20,2
N $D317 Street lamp LOD 1
@ $D317 label=streetlamptop_lods
B $D317,1,1 Width (bytes)
B $D318,1,1 Flags
B $D319,1,1 Height
W $D31A,2,2 Bitmap
W $D31C,2,2 Pre-shifted bitmap
N $D31E Street lamp LOD 2
B $D31E,1,1 Width (bytes)
B $D31F,1,1 Flags
B $D320,1,1 Height
W $D321,2,2 Bitmap
W $D323,2,2 Pre-shifted bitmap
N $D325 Street lamp LOD 3
B $D325,1,1 Width (bytes)
B $D326,1,1 Flags
B $D327,1,1 Height
W $D328,2,2 Bitmap
W $D32A,2,2 Pre-shifted bitmap
N $D32C Street lamp LOD 4
B $D32C,1,1 Width (bytes)
B $D32D,1,1 Flags
B $D32E,1,1 Height
W $D32F,2,2 Bitmap
W $D331,2,2 Pre-shifted bitmap
N $D333 Street lamp LOD 5
B $D333,1,1 Width (bytes)
B $D334,1,1 Flags
B $D335,1,1 Height
W $D336,2,2 Bitmap
W $D338,2,2 Pre-shifted bitmap
N $D33A Street lamp LOD 1 (flipped)
@ $D33A label=streetlamptop_flipped_lods
B $D33A,1,1 Width (bytes)
B $D33B,1,1 Flags
B $D33C,1,1 Height
W $D33D,2,2 Bitmap
W $D33F,2,2 Pre-shifted bitmap
N $D341 Street lamp LOD 2 (flipped)
B $D341,1,1 Width (bytes)
B $D342,1,1 Flags
B $D343,1,1 Height
W $D344,2,2 Bitmap
W $D346,2,2 Pre-shifted bitmap
N $D348 Street lamp LOD 3 (flipped)
B $D348,1,1 Width (bytes)
B $D349,1,1 Flags
B $D34A,1,1 Height
W $D34B,2,2 Bitmap
W $D34D,2,2 Pre-shifted bitmap
N $D34F Street lamp LOD 4 (flipped)
B $D34F,1,1 Width (bytes)
B $D350,1,1 Flags
B $D351,1,1 Height
W $D352,2,2 Bitmap
W $D354,2,2 Pre-shifted bitmap
N $D356 Street lamp LOD 5 (flipped)
B $D356,1,1 Width (bytes)
B $D357,1,1 Flags
B $D358,1,1 Height
W $D359,2,2 Bitmap
W $D35B,2,2 Pre-shifted bitmap
N $D35D Street lamp top (32x8)
N $D35D #HTML[#CALL:graphic($D35D,32,8,0,1)]
@ $D35D label=bitmap_streetlamptop_1
B $D35D,32,4 Bitmap data
N $D37D Street lamp top (24x5)
N $D37D #HTML[#CALL:graphic($D37D,24,5,0,1)]
@ $D37D label=bitmap_streetlamptop_2
B $D37D,15,3 Bitmap data
N $D38C Street lamp top (24x4)
N $D38C #HTML[#CALL:graphic($D38C,24,4,0,1)]
@ $D38C label=bitmap_streetlamptop_3
B $D38C,12,3 Bitmap data
N $D398 Street lamp top (24x4) pre-shifted
N $D398 #HTML[#CALL:graphic($D398,24,4,0,1)]
@ $D398 label=bitmap_streetlamptop_3s
B $D398,12,3 Bitmap data
N $D3A4 Street lamp top (16x4)
N $D3A4 #HTML[#CALL:graphic($D3A4,16,4,1,1)]
@ $D3A4 label=bitmap_streetlamptop_4
B $D3A4,16,2 Masked bitmap data
N $D3B4 Street lamp top (16x4) pre-shifted
N $D3B4 #HTML[#CALL:graphic($D3B4,16,4,1,1)]
@ $D3B4 label=bitmap_streetlamptop_4s
B $D3B4,16,2 Masked bitmap data
N $D3C4 Street lamp top (16x3)
N $D3C4 #HTML[#CALL:graphic($D3C4,16,3,1,1)]
@ $D3C4 label=bitmap_streetlamptop_5
B $D3C4,12,2 Masked bitmap data
N $D3D0 Street lamp top (16x3) pre-shifted
N $D3D0 #HTML[#CALL:graphic($D3D0,16,3,1,1)]
@ $D3D0 label=bitmap_streetlamptop_5s
B $D3D0,12,2 Masked bitmap data
N $D3DC Referenced by graphic entry 7
@ $D3DC label=stretchy_telegraphpole_right
B $D3DC,1,1
W $D3DD,2,2
B $D3DF,1,1
W $D3E0,2,2
B $D3E2,1,1
W $D3E3,2,2
B $D3E5,1,1
N $D3E6 Referenced by graphic entry 16
@ $D3E6 label=stretchy_telegraphpole_left
B $D3E6,1,1
W $D3E7,2,2
B $D3E9,1,1
W $D3EA,2,2
B $D3EC,1,1
W $D3ED,2,2
B $D3EF,1,1
@ $D3F0 label=telegraphpoletop_6ff0
W $D3F0,2,2
B $D3F2,20,2
@ $D406 label=telegraphpoletop_D406
W $D406,2,2
B $D408,20,2
@ $D41C label=telegraphpoletop_lods
B $D41C,1,1 Width (bytes)
B $D41D,1,1 Flags
B $D41E,1,1 Height
W $D41F,2,2 Bitmap
W $D421,2,2 Pre-shifted bitmap
B $D423,1,1 Width (bytes)
B $D424,1,1 Flags
B $D425,1,1 Height
W $D426,2,2 Bitmap
W $D428,2,2 Pre-shifted bitmap
B $D42A,1,1 Width (bytes)
B $D42B,1,1 Flags
B $D42C,1,1 Height
W $D42D,2,2 Bitmap
W $D42F,2,2 Pre-shifted bitmap
B $D431,1,1 Width (bytes)
B $D432,1,1 Flags
B $D433,1,1 Height
W $D434,2,2 Bitmap
W $D436,2,2 Pre-shifted bitmap
B $D438,1,1 Width (bytes)
B $D439,1,1 Flags
B $D43A,1,1 Height
W $D43B,2,2 Bitmap
W $D43D,2,2 Pre-shifted bitmap
N $D43F Top of telegraph pole (24x13)
N $D43F #HTML[#CALL:graphic($D43F,24,13,0,1)]
@ $D43F label=bitmap_telegraphpoletop_1
B $D43F,8,8 Bitmap data
B $D447,31,8*3,7
N $D466 Top of telegraph pole (24x10)
N $D466 #HTML[#CALL:graphic($D466,24,10,0,1)]
@ $D466 label=bitmap_telegraphpoletop_2
B $D466,8,8 Bitmap data
B $D46E,22,8*2,6
N $D484 Top of telegraph pole (24x7)
N $D484 #HTML[#CALL:graphic($D484,24,7,0,1)]
@ $D484 label=bitmap_telegraphpoletop_3
B $D484,8,8 Bitmap data
B $D48C,13,8,5
N $D499 Top of telegraph pole (24x7) pre-shifted
N $D499 #HTML[#CALL:graphic($D499,24,7,0,1)]
@ $D499 label=bitmap_telegraphpoletop_3s
B $D499,8,8 Bitmap data
B $D4A1,13,8,5
N $D4AE Top of telegraph pole (16x5) masked
N $D4AE #HTML[#CALL:graphic($D4AE,16,5,1,1)]
@ $D4AE label=bitmap_telegraphpoletop_4
B $D4AE,8,8 Masked bitmap data
B $D4B6,12,8,4
N $D4C2 Top of telegraph pole (16x5) pre-shifted and masked
N $D4C2 #HTML[#CALL:graphic($D4C2,16,5,1,1)]
@ $D4C2 label=bitmap_telegraphpoletop_4s
B $D4C2,8,8 Masked bitmap data
B $D4CA,12,8,4
N $D4D6 Top of telegraph pole (16x4)
N $D4D6 #HTML[#CALL:graphic($D4D6,16,4,1,1)]
@ $D4D6 label=bitmap_telegraphpoletop_5
B $D4D6,8,8 Masked bitmap data
B $D4DE,8,8
N $D4E6 Top of telegraph pole (16x4) pre-shifted and masked
N $D4E6 #HTML[#CALL:graphic($D4E6,16,4,1,1)]
@ $D4E6 label=bitmap_telegraphpoletop_5s
B $D4E6,8,8 Masked bitmap data
@ $D4F6 label=stretchy_tree_right
B $D4EE,9,8,1
W $D4F7,2,2
B $D4F9,1,1
W $D4FA,2,2
B $D4FC,1,1
W $D4FD,2,2
B $D4FF,1,1
W $D500,2,2
B $D502,1,1
W $D503,2,2
@ $D506 label=stretchy_tree_left
B $D505,2,1
W $D507,2,2
B $D509,1,1
W $D50A,2,2
B $D50C,1,1
W $D50D,2,2
B $D50F,1,1
@ $D510 label=tree_D510
W $D510,2,2 -> tree_lods
B $D512,20,2
@ $D526 label=tree_D526
W $D526,2,2 -> tree_lods
B $D528,20,2
@ $D53C label=tree_713c
W $D53C,2,2 -> tree_lods
B $D53E,20,2
@ $D552 label=tree_D552
W $D552,2,2 -> tree_lods
B $D554,20,2
@ $D568 label=tree_D568
W $D568,2,2 -> tree_lods
B $D56A,20,2
N $D57E LOD
@ $D57E label=tree_lods
B $D57E,1,1 Width (bytes)
B $D57F,1,1 Flags
B $D580,1,1 Height
W $D581,2,2 Bitmap
W $D583,2,2 Pre-shifted bitmap
N $D585 LOD
B $D585,1,1 Width (bytes)
B $D586,1,1 Flags
B $D587,1,1 Height
W $D588,2,2 Bitmap
W $D58A,2,2 Pre-shifted bitmap
N $D58C LOD
B $D58C,1,1 Width (bytes)
B $D58D,1,1 Flags
B $D58E,1,1 Height
W $D58F,2,2 Bitmap
W $D591,2,2 Pre-shifted bitmap
N $D593 LOD
B $D593,1,1 Width (bytes)
B $D594,1,1 Flags
B $D595,1,1 Height
W $D596,2,2 Bitmap
W $D598,2,2 Pre-shifted bitmap
N $D59A LOD
B $D59A,1,1 Width (bytes)
B $D59B,1,1 Flags
B $D59C,1,1 Height
W $D59D,2,2 Bitmap
W $D59F,2,2 Pre-shifted bitmap
N $D5A1 LOD
B $D5A1,1,1 Width (bytes)
B $D5A2,1,1 Flags
B $D5A3,1,1 Height
W $D5A4,2,2 Bitmap
W $D5A6,2,2 Pre-shifted bitmap
N $D5A8 LOD
B $D5A8,1,1 Width (bytes)
B $D5A9,1,1 Flags
B $D5AA,1,1 Height
W $D5AB,2,2 Bitmap
W $D5AD,2,2 Pre-shifted bitmap
N $D5AF LOD
B $D5AF,1,1 Width (bytes)
B $D5B0,1,1 Flags
B $D5B1,1,1 Height
W $D5B2,2,2 Bitmap
W $D5B4,2,2 Pre-shifted bitmap
N $D5B6 LOD
B $D5B6,1,1 Width (bytes)
B $D5B7,1,1 Flags
B $D5B8,1,1 Height
W $D5B9,2,2 Bitmap
W $D5BB,2,2 Pre-shifted bitmap
N $D5BD LOD
B $D5BD,1,1 Width (bytes)
B $D5BE,1,1 Flags
B $D5BF,1,1 Height
W $D5C0,2,2 Bitmap
W $D5C2,2,2 Pre-shifted bitmap
N $D5C4 LOD
B $D5C4,1,1 Width (bytes)
B $D5C5,1,1 Flags
B $D5C6,1,1 Height
W $D5C7,2,2 Bitmap
W $D5C9,2,2 Pre-shifted bitmap
N $D5CB LOD
B $D5CB,1,1 Width (bytes)
B $D5CC,1,1 Flags
B $D5CD,1,1 Height
W $D5CE,2,2 Bitmap
W $D5D0,2,2 Pre-shifted bitmap
N $D5D2 LOD
B $D5D2,1,1 Width (bytes)
B $D5D3,1,1 Flags
B $D5D4,1,1 Height
W $D5D5,2,2 Bitmap
W $D5D7,2,2 Pre-shifted bitmap
N $D5D9 LOD
B $D5D9,1,1 Width (bytes)
B $D5DA,1,1 Flags
B $D5DB,1,1 Height
W $D5DC,2,2 Bitmap
W $D5DE,2,2 Pre-shifted bitmap
N $D5E0 LOD - Tree top (64x13)
B $D5E0,1,1 Width (bytes)
B $D5E1,1,1 Flags
B $D5E2,1,1 Height
W $D5E3,2,2 Bitmap
W $D5E5,2,2 Pre-shifted bitmap
N $D5E7 LOD
B $D5E7,1,1 Width (bytes)
B $D5E8,1,1 Flags
B $D5E9,1,1 Height
W $D5EA,2,2 Bitmap
W $D5EC,2,2 Pre-shifted bitmap
N $D5EE LOD
B $D5EE,1,1 Width (bytes)
B $D5EF,1,1 Flags
B $D5F0,1,1 Height
W $D5F1,2,2 Bitmap
W $D5F3,2,2 Pre-shifted bitmap
N $D5F5 LOD
B $D5F5,1,1 Width (bytes)
B $D5F6,1,1 Flags
B $D5F7,1,1 Height
W $D5F8,2,2 Bitmap
W $D5FA,2,2 Pre-shifted bitmap
N $D5FC LOD
B $D5FC,1,1 Width (bytes)
B $D5FD,1,1 Flags
B $D5FE,1,1 Height
W $D5FF,2,2 Bitmap
W $D601,2,2 Pre-shifted bitmap
N $D603 LOD
B $D603,1,1 Width (bytes)
B $D604,1,1 Flags
B $D605,1,1 Height
W $D606,2,2 Bitmap
W $D608,2,2 Pre-shifted bitmap
N $D60A LOD
B $D60A,1,1 Width (bytes)
B $D60B,1,1 Flags
B $D60C,1,1 Height
W $D60D,2,2 Bitmap
W $D60F,2,2 Pre-shifted bitmap
N $D611 LOD
B $D611,1,1 Width (bytes)
B $D612,1,1 Flags
B $D613,1,1 Height
W $D614,2,2 Bitmap
W $D616,2,2 Pre-shifted bitmap
N $D618 LOD
B $D618,1,1 Width (bytes)
B $D619,1,1 Flags
B $D61A,1,1 Height
W $D61B,2,2 Bitmap
W $D61D,2,2 Pre-shifted bitmap
N $D61F LOD
B $D61F,1,1 Width (bytes)
B $D620,1,1 Flags
B $D621,1,1 Height
W $D622,2,2 Bitmap
W $D624,2,2 Pre-shifted bitmap
N $D626 LOD
B $D626,1,1 Width (bytes)
B $D627,1,1 Flags
B $D628,1,1 Height
W $D629,2,2 Bitmap
W $D62B,2,2 Pre-shifted bitmap
N $D62D Tree middle (64x16)
N $D62D #HTML[#CALL:graphic($D62D,64,16,0,1)]
@ $D62D label=bitmap_tree_middle_64x16
B $D62D,128,8
N $D6AD Tree bottom (64x5)
N $D6AD #HTML[#CALL:graphic($D6AD,64,5,0,1)]
@ $D6AD label=bitmap_tree_bottom_64x5
B $D6AD,40,8
N $D6D5 Tree trunk (16x8)
N $D6D5 #HTML[#CALL:graphic($D6D5,16,8,0,1)]
@ $D6D5 label=bitmap_tree_trunk_16x8
B $D6D5,16,8
N $D6E5 Tree shadow (64x5)
N $D6E5 #HTML[#CALL:graphic($D6E5,64,5,0,1)]
@ $D6E5 label=bitmap_tree_shadow_64x5
B $D6E5,40,8
N $D70D Tree middle (48x12)
N $D70D #HTML[#CALL:graphic($D70D,48,12,0,1)]
@ $D70D label=bitmap_tree_middle_48x12
B $D70D,72,8
N $D755 Tree bottom (48x4)
N $D755 #HTML[#CALL:graphic($D755,48,4,0,1)]
@ $D755 label=bitmap_tree_bottom_48x4
B $D755,24,8
N $D76D Tree shadow (48x4)
N $D76D #HTML[#CALL:graphic($D76D,48,4,0,1)]
@ $D76D label=bitmap_tree_shadow_48x4
B $D76D,24,8
N $D785 Tree middle (32x8)
N $D785 #HTML[#CALL:graphic($D785,32,8,0,1)]
@ $D785 label=bitmap_tree_middle_32x8
B $D785,32,8
N $D7A5 Tree bottom (32x3)
N $D7A5 #HTML[#CALL:graphic($D7A5,32,3,0,1)]
@ $D7A5 label=bitmap_tree_bottom_32x3
B $D7A5,12,8,4
N $D7B1 Tree shadow (16x2)
N $D7B1 #HTML[#CALL:graphic($D7B1,16,2,0,1)]
@ $D7B1 label=bitmap_tree_shadow_16x2
B $D7B1,8,8
N $D7B9 Tree middle (24x7)
N $D7B9 #HTML[#CALL:graphic($D7B9,24,7,0,1)]
@ $D7B9 label=bitmap_tree_middle_24x7
B $D7B9,21,8*2,5
N $D7CE Tree bottom (24x2)
N $D7CE #HTML[#CALL:graphic($D7CE,24,2,0,1)]
@ $D7CE label=bitmap_tree_bottom_24x2
B $D7CE,6,6
N $D7D4 Tree trunk (8x4)
N $D7D4 #HTML[#CALL:graphic($D7D4,8,4,0,1)]
@ $D7D4 label=bitmap_tree_trunk_8x4
B $D7D4,4,4
N $D7D8 Tree shadow (24x2)
N $D7D8 #HTML[#CALL:graphic($D7D8,24,2,0,1)]
@ $D7D8 label=bitmap_tree_shadow_24x2
B $D7D8,6,6
N $D7DE Tree top (64x13)
N $D7DE #HTML[#CALL:graphic($D7DE,64,13,1,1)]
@ $D7DE label=bitmap_tree_top_64x13
B $D7DE,208,8 Masked bitmap data
N $D8AE Tree top (48x10)
N $D8AE #HTML[#CALL:graphic($D8AE,48,10,1,1)]
@ $D8AE label=bitmap_tree_top_48x10
B $D8AE,120,12 Masked bitmap data
N $D926 Tree top (32x5)
N $D926 #HTML[#CALL:graphic($D926,32,5,1,1)]
@ $D926 label=bitmap_tree_top_32x5
B $D926,40,8 Masked bitmap data
N $D94E Tree top (24x4)
N $D94E #HTML[#CALL:graphic($D94E,24,4,1,1)]
@ $D94E label=bitmap_tree_top_24x4
B $D94E,24,6 Masked bitmap data
N $D966 Tree top (24x3)
N $D966 #HTML[#CALL:graphic($D966,24,3,1,1)]
@ $D966 label=bitmap_tree_top_24x3
B $D966,18,6 Masked bitmap data
N $D978 Tree top (24x3) pre-shifted
N $D978 #HTML[#CALL:graphic($D978,24,3,1,1)]
@ $D978 label=bitmap_tree_top_24x3s
B $D978,18,6 Masked bitmap data
N $D98A Tree trunk (16x6)
N $D98A #HTML[#CALL:graphic($D98A,16,6,1,1)]
@ $D98A label=bitmap_tree_trunk_16x6
B $D98A,24,4 Masked bitmap data
N $D9A2 Tree trunk (16x4)
N $D9A2 #HTML[#CALL:graphic($D9A2,16,4,1,1)]
@ $D9A2 label=bitmap_tree_trunk_16x4
B $D9A2,16,4 Masked bitmap data
N $D9B2 Tree middle (24x5)
N $D9B2 #HTML[#CALL:graphic($D9B2,24,5,1,1)]
@ $D9B2 label=bitmap_tree_middle_24x5
B $D9B2,30,6 Masked bitmap data
N $D9D0 Tree bottom (24x2)
N $D9D0 #HTML[#CALL:graphic($D9D0,24,2,1,1)]
@ $D9D0 label=bitmap_tree_bottom_24x2_another
B $D9D0,12,6 Masked bitmap data
N $D9DC Tree trunk (24x3)
N $D9DC #HTML[#CALL:graphic($D9DC,24,3,1,1)]
@ $D9DC label=bitmap_tree_trunk_24x3
B $D9DC,18,6 Masked bitmap data
N $D9EE Tree shadow (24x1)
N $D9EE #HTML[#CALL:graphic($D9EE,24,1,1,1)]
@ $D9EE label=bitmap_tree_shadow_24x1
B $D9EE,6,6 Masked bitmap data
N $D9F4 Tree middle (24x5) pre-shifted
N $D9F4 #HTML[#CALL:graphic($D9F4,24,5,1,1)]
@ $D9F4 label=bitmap_tree_middle_24x5s
B $D9F4,30,6 Masked bitmap data
N $DA12 Tree bottom (24x2) pre-shifted
N $DA12 #HTML[#CALL:graphic($DA12,24,2,1,1)]
@ $DA12 label=bitmap_tree_bottom_24x2s
B $DA12,12,6 Masked bitmap data
N $DA1E Tree trunk (24x3) pre-shifted
N $DA1E #HTML[#CALL:graphic($DA1E,24,3,1,1)]
@ $DA1E label=bitmap_tree_trunk_24x3s
B $DA1E,18,6 Masked bitmap data
N $DA30 Tree shadow (24x1) pre-shifted
N $DA30 #HTML[#CALL:graphic($DA30,24,1,1,1)]
@ $DA30 label=bitmap_tree_shadow_24x1s
B $DA30,6,6 Masked bitmap data
u $DA36 [Stage 1] Spare space
D $DA36 This is the end of the per-stage data.
S $DA36,186,$BA
b $E000 [Stage 2] Horizon graphic
b $E0F0 [Stage 2] Per-stage data
W $E0F0,28,2 vars
w $E10C [Stage 2] Table of addresses of LODs
b $E11A [Stage 2] Per-stage difficulty settings
w $E11D [Stage 2] Per-stage setup data
W $E11D,14,2 vars
w $E12B [Stage 2] Per-stage attract mode data
W $E12B,14,2 vars
t $E139 [Stage 2] Nancy's perp description
t $E1DD [Stage 2] Arrest messages
b $E223 [Stage 2] Graphics definitions
B $E223,126,7
b $E2A1 [Stage 2] Map: Start stretch
u $FAF0
