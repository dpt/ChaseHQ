/**
 * Stage3Data.c
 *
 * This file is part of "Chase H.Q. in C".
 *
 * This project recreates the ZX Spectrum version of the chase-and-smash game
 * "Chase H.Q." in portable C code. It is free software provided without
 * warranty in the interests of education and software preservation.
 *
 * The arcade original was created by Taito Corporation in 1988. It was then
 * ported to the ZX Spectrum by Ocean Software Limited and released in 1989.
 *
 * The original game and design is copyright (c) 1988 Taito Corporation.
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited.
 * The recreated version is copyright (c) 2023-2026 David Thomas.
 */

#include <stddef.h>

#include "C99/Types.h"
#include "ZXSpectrum/Pixels.h"
#include "ZXSpectrum/Spectrum.h"

#include "ChaseHQ/Engine/Types.h"
#include "CommonData.h"

#include "Stage3Data.h"

/* Z80 addresses of the map sections, as referenced by the map
 * GOTO/SPLIT commands and stage3_map_goto_table[]. */
#define STAGE3_MAP_CURV_C3F4_ADDR    (0x5FF4)
#define STAGE3_MAP_HEIGHT_C409_ADDR  (0x6009)
#define STAGE3_MAP_LANES_C44B_ADDR   (0x604B)
#define STAGE3_MAP_HAZARDS_C455_ADDR (0x6055)
#define STAGE3_MAP_LOBJS_C46F_ADDR   (0x606F)
#define STAGE3_MAP_ROBJS_C4A9_ADDR   (0x60A9)
#define STAGE3_MAP_CURV_C4D8_ADDR    (0x60D8)
#define STAGE3_MAP_HEIGHT_C4EF_ADDR  (0x60EF)
#define STAGE3_MAP_LANES_C504_ADDR   (0x6104)
#define STAGE3_MAP_HAZARDS_C510_ADDR (0x6110)
#define STAGE3_MAP_LOBJS_C518_ADDR   (0x6118)
#define STAGE3_MAP_ROBJS_C54B_ADDR   (0x614B)
#define STAGE3_MAP_CURV_C58A_ADDR    (0x618A)
#define STAGE3_MAP_HEIGHT_C5A0_ADDR  (0x61A0)
#define STAGE3_MAP_LANES_C5B6_ADDR   (0x61B6)
#define STAGE3_MAP_HAZARDS_C5C4_ADDR (0x61C4)
#define STAGE3_MAP_LOBJS_C5C9_ADDR   (0x61C9)
#define STAGE3_MAP_ROBJS_C620_ADDR   (0x6220)
#define STAGE3_MAP_CURV_C677_ADDR    (0x6277)
#define STAGE3_MAP_HEIGHT_C69D_ADDR  (0x629D)
#define STAGE3_MAP_LANES_C6DC_ADDR   (0x62DC)
#define STAGE3_MAP_HAZARDS_C6FC_ADDR (0x62FC)
#define STAGE3_MAP_LOBJS_C71F_ADDR   (0x631F)
#define STAGE3_MAP_ROBJS_C7AD_ADDR   (0x63AD)

/* ----------------------------------------------------------------------- */

/* Stage 3 object type macros */

#define MAP_OBJ_S3_OVERHEAD_BRIDGE_VAL  (2)
#define MAP_OBJ_S3_TOWER_BLOCK_VAL      (4)
#define MAP_OBJ_S3_SPEED_LIMIT_SIGN_VAL (5)
#define MAP_OBJ_S3_TELEGRAPH_POLE_VAL   (6)

#define MAP_OBJ_S3_OVERHEAD_BRIDGE(D)   (((D) << 4) | MAP_OBJ_S3_OVERHEAD_BRIDGE_VAL)
#define MAP_OBJ_S3_TOWER_BLOCK(D)       (((D) << 4) | MAP_OBJ_S3_TOWER_BLOCK_VAL)
#define MAP_OBJ_S3_SPEED_LIMIT_SIGN(D)  (((D) << 4) | MAP_OBJ_S3_SPEED_LIMIT_SIGN_VAL)
#define MAP_OBJ_S3_TELEGRAPH_POLE(D)    (((D) << 4) | MAP_OBJ_S3_TELEGRAPH_POLE_VAL)

/* ----------------------------------------------------------------------- */

/* Forward declarations */

static const char *stage3_chatter_strings[4];
static const u8 stage3_arrest_messages_C1E8[70];
static const hittable_t stage3_hittable_objects_C22E[2];
static const obj_t stage3_right_obj_defs_C234[6];
static const obj_t stage3_left_obj_defs_C25E[6];
static const u8 stage3_map_curv_C288[36];
static const u8 stage3_map_height_C2AC[33];
static const u8 stage3_map_lanes_C2CD[14];
static const u8 stage3_map_hazards_C2DB[13];
static const u8 stage3_map_lobjs_C2E8[132];
static const u8 stage3_map_robjs_C36C[136];
static const u8 stage3_map_curv_C3F4[21];
static const u8 stage3_map_height_C409[66];
static const u8 stage3_map_lanes_C44B[10];
static const u8 stage3_map_hazards_C455[26];
static const u8 stage3_map_lobjs_C46F[58];
static const u8 stage3_map_robjs_C4A9[47];
static const u8 stage3_map_curv_C4D8[23];
static const u8 stage3_map_height_C4EF[21];
static const u8 stage3_map_lanes_C504[12];
static const u8 stage3_map_hazards_C510[8];
static const u8 stage3_map_lobjs_C518[51];
static const u8 stage3_map_robjs_C54B[63];
static const u8 stage3_map_curv_C58A[22];
static const u8 stage3_map_height_C5A0[22];
static const u8 stage3_map_lanes_C5B6[14];
static const u8 stage3_map_hazards_C5C4[5];
static const u8 stage3_map_lobjs_C5C9[87];
static const u8 stage3_map_robjs_C620[87];
static const u8 stage3_map_curv_C677[38];
static const u8 stage3_map_height_C69D[63];
static const u8 stage3_map_lanes_C6DC[32];
static const u8 stage3_map_hazards_C6FC[35];
static const u8 stage3_map_lobjs_C71F[142];
static const u8 stage3_map_robjs_C7AD[127];
static const pixel_t stage3_perp_face[180];
static const bitmap_t stage3_lods_C8E0[6];
static const bitmap_t stage3_lods_C90A[6];
static const bitmap_t stage3_lods_C934[6];
static const bitmap_t stage3_lods_CDF1[6];
static const bitmap_t stage3_lods_CE1B[6];
static const stretchy_t stage3_stretchy_CEA1[4];
static const depthset_t stage3_depthset_CEAB;
static const depthset_t stage3_depthset_CEC1;
static const depthset_t stage3_depthset_CED7;
static const bitmap_t stage3_lods_CEED[18];
static const stretchy_t stage3_stretchy_D2BB[3];
static const depthset_t stage3_depthset_D2C2;
static const depthset_t stage3_depthset_D2D8;
static const bitmap_t stage3_lods_D2EE[8];
static const stretchy_t stage3_stretchy_D326[3];
static const depthset_t stage3_depthset_D32D;
static const overhead_span_t stage3_overhead_spans_D343[10];
static const depthset_t stage3_depthset_D361;
static const bitmap_t stage3_lods_D377[8];
static const bitmap_t stage3_lods_D64F[6];
static const bitmap_t stage3_lods_D6BF[6];
static const stretchy_t stage3_stretchy_D79E[4];
static const stretchy_t stage3_stretchy_D7A8[4];
static const depthset_t stage3_depthset_D7B2;
static const depthset_t stage3_depthset_D7C8;
static const bitmap_t stage3_lods_D7DE[5];
static const stretchy_t stage3_stretchy_D9D6[4];
static const stretchy_t stage3_stretchy_D9E0[4];
static const depthset_t stage3_depthset_D9EA;
static const depthset_t stage3_depthset_DA00;
static const bitmap_t stage3_lods_DA16[5];
static const pixel_t stage3_bitmap_CAC9[6 * 1 * 29 * 1];
static const pixel_t stage3_bitmap_CB77[4 * 1 * 19 * 1];
static const pixel_t stage3_bitmap_CBC3[3 * 1 * 14 * 1];
static const pixel_t stage3_bitmap_CF6B[8 * 1 * 32 * 1];
static const pixel_t stage3_bitmap_D06B[8 * 1 * 8 * 1];
static const pixel_t stage3_bitmap_D0AB[6 * 1 * 25 * 1];
static const pixel_t stage3_bitmap_D141[6 * 1 * 6 * 1];
static const pixel_t stage3_bitmap_D165[6 * 1 * 4 * 1];
static const pixel_t stage3_bitmap_D17D[5 * 1 * 20 * 1];
static const pixel_t stage3_bitmap_D1E1[5 * 1 * 5 * 1];
static const pixel_t stage3_bitmap_D1FA[5 * 1 * 4 * 1];
static const pixel_t stage3_bitmap_D20E[4 * 1 * 14 * 1];
static const pixel_t stage3_bitmap_D246[4 * 1 * 4 * 1];
static const pixel_t stage3_bitmap_D256[4 * 1 * 3 * 1];
static const pixel_t stage3_bitmap_D262[3 * 1 * 11 * 1];
static const pixel_t stage3_bitmap_D283[3 * 1 * 4 * 1];
static const pixel_t stage3_bitmap_D28F[3 * 1 * 2 * 1];
static const pixel_t stage3_bitmap_D295[2 * 1 * 8 * 1];
static const pixel_t stage3_bitmap_D2A5[2 * 1 * 3 * 1];
static const pixel_t stage3_bitmap_D2AB[2 * 1 * 2 * 1];
static const pixel_t stage3_bitmap_D801[3 * 1 * 27 * 1];
static const pixel_t stage3_bitmap_D852[2 * 1 * 20 * 1];
static const pixel_t stage3_bitmap_D87A[3 * 2 * 15 * 1];
static const pixel_t stage3_bitmap_D8D4[3 * 2 * 15 * 1];
static const pixel_t stage3_bitmap_D92E[2 * 2 * 12 * 1];
static const pixel_t stage3_bitmap_D95E[2 * 2 * 12 * 1];
static const pixel_t stage3_bitmap_D98E[2 * 2 * 9 * 1];
static const pixel_t stage3_bitmap_D9B2[2 * 2 * 9 * 1];

/* ----------------------------------------------------------------------- */

/** $C000: stage3 */
// clang-format off
const stage_t stage3 = {
  /* $C000 backdrop */
  {
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    X_XXXXXX, XXXX_X_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, _XXXXXXX, XXXXX___, ________, ____XXXX, XXXXXXXX,
    XXXX_X_X, XXX_X_XX, XXXXXXXX, X_XXXXXX, XXXXXXXX, XXXXXXXX, X_XXX_X_, X_X_X_X_, X_X_XXXX, XXX_X__X,
    X_XXXXXX, XXXXXX_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, __XX_XXX, XXXXX___, ________, ____XXXX, XXXXXXXX,
    XXXX_X_X, XXXXX_XX, XXXXXX_X, _XXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXX_X, _X_X_X_X, _X_XXX_X, XXX_X__X,
    X_XXXXXX, XXXXXX_X, XXXXXXXX, XXX_XXXX, XXXXXXXX, _____XXX, XXXXXXXX, XXXXXXXX, XXXXX___, XXXXXXXX,
    XXXX_X_X, XXX_X_XX, XXXXXX_X, _XXXX_XX, XXXXXXXX, XXXXXXXX, X_XXXXXX, ___XXXXX, XX_XX___, _X_X_X_X,
    X_XXXXXX, XXXX_X_X, XXXXXXXX, XXX_X__X, XXXXXXXX, _____XXX, XXXXXXXX, X__XXXXX, X_X_X___, _XXXXXXX,
    XXXX_X_X, XXXXXXXX, X_XX_X_X, XX_XX__X, XXXXXXXX, XXXXXXXX, X_XXXXXX, X__X___X, _X_XX___, _XXXXXXX,
    X_XXXXXX, XXXXXXXX, X_XXXXXX, XXX_X__X, XXXXXXXX, _____XXX, XX_XXXX_, X__XX_X_, X_X_X___, _XXXX_X_,
    XXXX_X_X, X__X_XXX, __XX_X_X, XX_XX___, __XXXXXX, XXXXXXXX, X__XX_XX, XXXX___X, _X_XXXXX, XXXXXXXX,
    X_XXXXXX, X__X_XXX, __XXXXXX, X_X_X___, __XXXXXX, _____XXX, XXXXXXXX, XXXXX_X_, X_X_X__X, XXXXX_X_,
    XXXX_X_X, X__X____, __XX_X_X, XXXXX___, __XXXXXX, XXXXXXXX, X_____XX, XXXX____, ___XX__X, XXXXXXXX,
    X_XXXXXX, X__XXXXX, XXXXXXXX, X___X___, __X__XX_, XXXXXXXX, XX______, XXXXX_X_, X_XX___X, XXXXX_X_,
    XXXX_X_X, X______X, _X_X_X_X, X_______, __X__X_X, XXXXXXXX, X______X, XXXXXXXX, XXXX___X, XXXXXXXX,
    X_XXXXXX, X______X, XXXXXXXX, X___X___, __X__XX_, XXXXXXXX, XX______, XXXX__X_, _______X, XXXXX_X_,
    XXXX____, _______X, _X_X_X_X, X_______, __X__X_X, _X_X__X_, X______X, XXXX__X_, _______X, X___XXXX,
    X_XX____, _______X, XXXXXXXX, X___XXXX, XXX__XX_, X_XX__X_, XXXXXXXX, XXX___X_, _______X, X___X_X_,
    XXXX____, _______X, _X_X_X_X, X_____X_, X____XXX, XXXX__X_, X_X_X_XX, XXX___X_, _______X, X___XXXX,
    XXXX____, _______X, XXXXXXXX, X_____X_, X_______, ______X_, _X_X_XXX, XXX___XX, XXXXXXXX, ____XXXX,
    ________, _______X, _X_X_X_X, X_____XX, X_______, ______X_, X_X_X_X_, ________, ________, ________,
    ________, _______X, XXXXXXXX, X_______, ________, ______X_, _X_X_XX_, ________, ________, ________,
    ________, _______X, _X_X_X_X, ________, ________, ______X_, ______X_, ________, ________, ________,
    ________, _______X, XXXXXXXX, ________, ________, ______XX, XXXXXXX_, ________, ________, ________,
  },
  /* $C0F0 perstage */
  &stage3_perp_face[FACEBITMAPBYTES],
  NULL,  /* no pilot mugshot on this stage */
  attribute_BRIGHT_BLACK_OVER_YELLOW * 0x0101,
  &stage3_hittable_objects_C22E[0],  /* addrof_hittable_objects */
  &stage3_right_obj_defs_C234[-1].arg,  /* addrof_right_hand_handlers */
  &stage3_right_obj_defs_C234[-1],  /* addrof_right_hand_objects */
  &stage3_right_obj_defs_C234[2],  /* addrof_right_hand_short_pole_object */
  &stage3_left_obj_defs_C25E[-1].arg,  /* addrof_left_hand_handlers */
  &stage3_left_obj_defs_C25E[-1],  /* addrof_left_hand_objects */
  &stage3_left_obj_defs_C25E[2],  /* addrof_left_hand_short_pole_object */
  &perp_description[0],  /* addrof_perp_description */
  &stage3_arrest_messages_C1E8[0],  /* addrof_arrest_messages */
  { NULL, NULL },

  (const bitmap_t (*)[SPRITE_FRAMES])&stage3_lods_CDF1[0],  /* bitmaps_stones */
  (const bitmap_t (*)[SPRITE_FRAMES])&stage3_lods_CE1B[0],  /* bitmaps_dust */
  &stage3_lods_C8E0[0],  /* bitmaps_perp_car */
  {
    &stage3_lods_C934[0],
    &stage3_lods_C90A[0],
    &stage3_lods_C934[0],
    &stage3_lods_C8E0[0]
  },  /* bitmaps_vehicles */

  /* $C11A difficulty */
  15,  /* car_spawn_delay */
  70,  /* perp_lane_change_base */
  85,  /* perp_approach_base */

  /* $C11D setupdata */
  {
    234,
    &stage3_map_curv_C288[-1],
    &stage3_map_height_C2AC[-1],
    &stage3_map_lanes_C2CD[-1],
    &stage3_map_robjs_C36C[-1],
    &stage3_map_lobjs_C2E8[-1],
    &stage3_map_hazards_C2DB[-1],
  },

  /* $C12B attractdata */
  {
    234,
    &stage3_map_curv_C677[-1],
    &stage3_map_height_C69D[-1],
    &stage3_map_lanes_C6DC[-1],
    &stage3_map_robjs_C7AD[-1],
    &stage3_map_lobjs_C71F[-1],
    &stage3_map_hazards_C6FC[-1],
  },

  stage3_chatter_strings
};

/* ----------------------------------------------------------------------- */

/* $C139: perp_description - Conv: Deduped to CommonData.c (shared with stage 1/2/4/5) */

/** $C145 (bank 6) */
static const char *stage3_chatter_strings[4] = {
  "THIS IS NANCY AT CHASE H.Q. WE'VE GOT A\xCE",
  "EMERGENCY HERE. A GANG OF CHICAGO PUSHER\xD3",
  "ARE FLEEING TOWARDS THE SUBURBS. THE TARGE\xD4",
  "VEHICLE IS A GERMAN SPORTS CAR... OVER\xAE",
};

/* ----------------------------------------------------------------------- */

/**
 * $C1E8: stage3_arrest_messages_C1E8
 */
static const u8 stage3_arrest_messages_C1E8[70] = {
  6,  // initial delay

  6,  // delay
  DRAWCHARSTYLE_SINGLE,
  0,  // attribute
  CHQBACKBUF(0xF802),
  ZXATTRS(0x5922),
  'O', 'K', '!', ' ', 'Y', 'O', 'U', ' ', 'A', 'R', 'E', ' ', 'U', 'N', 'D', 'E', 'R', ' ', 'A', 'R', 'R', 'E', 'S', 'T', ' ', 'O', 'N' | EOS,

  45,  // delay
  DRAWCHARSTYLE_SINGLE,
  0,  // attribute
  CHQBACKBUF(0xF023),
  ZXATTRS(0x5943),
  'S', 'U', 'S', 'P', 'I', 'C', 'I', 'O', 'N', ' ', 'O', 'F', ' ', 'S', 'E', 'L', 'L', 'I', 'N', 'G', ' ', 'D', 'R', 'U', 'G', 'S' | EOS,

  TRANSITIONCONTROL_FILL_ATTRIBUTES,
  DRAWOVERLAY_STOP
};

/* ----------------------------------------------------------------------- */

/** $C22E: stage3_hittable_objects_C22E */
static const hittable_t stage3_hittable_objects_C22E[2] = {
  { 16, &stage3_lods_D64F[0] },
  { 32, &stage3_lods_D6BF[0] },
};

/** $C234: stage3_right_obj_defs_C234 */
static const obj_t stage3_right_obj_defs_C234[6] = {
  { 111, 41, 80, &tunnellight, draw_tunnel_light_right },
  { 111, 53, 70, &stage3_stretchy_D2BB[0], draw_stretchy_object_right },
  { 144, 92, 40, &stretchy_shortpole, draw_stretchy_object_right },
  { 80, 16, 60, &stage3_stretchy_CEA1[0], draw_stretchy_object_right },
  { 110, 53, 80, &stage3_stretchy_D79E[0], draw_stretchy_object_right },
  { 110, 53, 80, &stage3_stretchy_D9D6[0], draw_stretchy_object_right },
};

/** $C25E: stage3_left_obj_defs_C25E */
static const obj_t stage3_left_obj_defs_C25E[6] = {
  { 126, 188, 80, &tunnellight, draw_tunnel_light_left },
  { 120, 154, 70, &stage3_stretchy_D326[0], draw_overhead },
  { 96, 144, 40, &stretchy_shortpole, draw_stretchy_object_left },
  { 168, 240, 60, &stage3_stretchy_CEA1[0], draw_stretchy_object_left },
  { 132, 182, 80, &stage3_stretchy_D7A8[0], draw_stretchy_object_left },
  { 132, 182, 80, &stage3_stretchy_D9E0[0], draw_stretchy_object_left },
};

/* ----------------------------------------------------------------------- */

/** $C288: stage3_map_curv_C288 */
static const u8 stage3_map_curv_C288[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(1),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(9),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(6),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(10),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(10),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(14),
  MAP_CMD_SPLIT(STAGE3_MAP_CURV_C3F4_ADDR, STAGE3_MAP_CURV_C4D8_ADDR)
};

/** $C2AC: stage3_map_height_C2AC */
static const u8 stage3_map_height_C2AC[] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(10),
  MAP_CMD_SPLIT(STAGE3_MAP_HEIGHT_C409_ADDR, STAGE3_MAP_HEIGHT_C4EF_ADDR)
};

/** $C2CD: stage3_map_lanes_C2CD */
static const u8 stage3_map_lanes_C2CD[] = {
  MAP_LANES_3R(60),
  MAP_LANES_3RTO4(2),
  MAP_LANES_4(254),
  MAP_LANES_4(84),
  MAP_CMD_SPLIT(STAGE3_MAP_LANES_C44B_ADDR, STAGE3_MAP_LANES_C504_ADDR)
};

/** $C2DB: stage3_map_hazards_C2DB */
static const u8 stage3_map_hazards_C2DB[] = {
  MAP_HAZARD_WAIT(193),
  MAP_CMD_STOP_CARS,
  MAP_HAZARD_WAIT(3),
  MAP_CMD_ARROW_L,
  MAP_HAZARD_WAIT(4),
  MAP_CMD_SPLIT(STAGE3_MAP_HAZARDS_C455_ADDR, STAGE3_MAP_HAZARDS_C510_ADDR)
};

/** $C2E8: stage3_map_lobjs_C2E8 */
static const u8 stage3_map_lobjs_C2E8[] = {
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_CMD_SPLIT(STAGE3_MAP_LOBJS_C46F_ADDR, STAGE3_MAP_LOBJS_C518_ADDR)
};

/** $C36C: stage3_map_robjs_C36C */
static const u8 stage3_map_robjs_C36C[] = {
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_CMD_SPLIT(STAGE3_MAP_ROBJS_C4A9_ADDR, STAGE3_MAP_ROBJS_C54B_ADDR)
};

/** $C3F4: stage3_map_curv_C3F4 */
static const u8 stage3_map_curv_C3F4[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(6),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(4),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_VERY_HARD(11),
  MAP_CURVE_RIGHT_HARD(10),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(12),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(2),
  MAP_CMD_GOTO(STAGE3_MAP_CURV_C58A_ADDR)
};

/** $C409: stage3_map_height_C409 */
static const u8 stage3_map_height_C409[] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(5),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_LEVEL(11),
  MAP_CMD_GOTO(STAGE3_MAP_HEIGHT_C5A0_ADDR)
};

/** $C44B: stage3_map_lanes_C44B */
static const u8 stage3_map_lanes_C44B[] = {
  MAP_LANES_4(20),
  MAP_LANES_DIRTTRACK(178),
  MAP_LANES_4(12),
  MAP_CMD_GOTO(STAGE3_MAP_LANES_C5B6_ADDR)
};

/** $C455: stage3_map_hazards_C455 */
static const u8 stage3_map_hazards_C455[] = {
  MAP_HAZARD_WAIT(10),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_START_TWO_OBSTACLES,
  MAP_HAZARD_WAIT(41),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(4),
  MAP_CMD_START_TWO_OBSTACLES,
  MAP_HAZARD_WAIT(42),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_START_CARS,
  MAP_HAZARD_WAIT(4),
  MAP_CMD_GOTO(STAGE3_MAP_HAZARDS_C5C4_ADDR)
};

/** $C46F: stage3_map_lobjs_C46F */
static const u8 stage3_map_lobjs_C46F[] = {
  MAP_OBJ_NONE(10),
  MAP_OBJ_SHORT_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(13),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_CMD_GOTO(STAGE3_MAP_LOBJS_C5C9_ADDR)
};

/** $C4A9: stage3_map_robjs_C4A9 */
static const u8 stage3_map_robjs_C4A9[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_SHORT_POLE(1),
  MAP_OBJ_NONE(11),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(13),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(8),
  MAP_CMD_GOTO(STAGE3_MAP_ROBJS_C620_ADDR)
};

/** $C4D8: stage3_map_curv_C4D8 */
static const u8 stage3_map_curv_C4D8[] = {
  MAP_CURVE_STRAIGHT(8),
  MAP_CURVE_LEFT(7),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(7),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(14),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT(9),
  MAP_CURVE_STRAIGHT(11),
  MAP_CURVE_LEFT(9),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(11),
  MAP_CURVE_LEFT(5),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(8),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(1),
  MAP_CMD_GOTO(STAGE3_MAP_CURV_C58A_ADDR)
};

/** $C4EF: stage3_map_height_C4EF */
static const u8 stage3_map_height_C4EF[] = {
  MAP_HEIGHT_UP5(15),
  MAP_HEIGHT_UP5(15),
  MAP_HEIGHT_UP5(12),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN5(15),
  MAP_HEIGHT_DOWN5(15),
  MAP_HEIGHT_DOWN5(11),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(5),
  MAP_CMD_GOTO(STAGE3_MAP_HEIGHT_C5A0_ADDR)
};

/** $C504: stage3_map_lanes_C504 */
static const u8 stage3_map_lanes_C504[] = {
  MAP_LANES_4(56),
  MAP_LANES_4TO3R(134),
  MAP_LANES_3RTO4(2),
  MAP_LANES_4(18),
  MAP_CMD_GOTO(STAGE3_MAP_LANES_C5B6_ADDR)
};

/** $C510: stage3_map_hazards_C510 */
static const u8 stage3_map_hazards_C510[] = {
  MAP_HAZARD_WAIT(10),
  MAP_CMD_START_CARS,
  MAP_HAZARD_WAIT(95),
  MAP_CMD_GOTO(STAGE3_MAP_HAZARDS_C5C4_ADDR)
};

/** $C518: stage3_map_lobjs_C518 */
static const u8 stage3_map_lobjs_C518[] = {
  MAP_OBJ_NONE(6),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(2),
  MAP_OBJ_SHORT_POLE(4),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(4),
  MAP_CMD_GOTO(STAGE3_MAP_LOBJS_C5C9_ADDR)
};

/** $C54B: stage3_map_robjs_C54B */
static const u8 stage3_map_robjs_C54B[] = {
  MAP_OBJ_NONE(6),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(4),
  MAP_CMD_GOTO(STAGE3_MAP_ROBJS_C620_ADDR)
};

/** $C58A: stage3_map_curv_C58A */
static const u8 stage3_map_curv_C58A[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(1),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(1),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(5),
  MAP_CURVE_LEFT(4),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(14),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(4),
  MAP_CURVE_RIGHT(7),
  MAP_CURVE_STRAIGHT(6),
  MAP_CMD_GOTO(STAGE3_MAP_CURV_C677_ADDR)
};

/** $C5A0: stage3_map_height_C5A0 */
static const u8 stage3_map_height_C5A0[] = {
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(6),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_DOWN5(9),
  MAP_HEIGHT_LEVEL(9),
  MAP_HEIGHT_UP5(8),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN5(6),
  MAP_HEIGHT_DOWN3(3),
  MAP_HEIGHT_DOWN1(3),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(10),
  MAP_CMD_GOTO(STAGE3_MAP_HEIGHT_C69D_ADDR)
};

/** $C5B6: stage3_map_lanes_C5B6 */
static const u8 stage3_map_lanes_C5B6[] = {
  MAP_LANES_4(8),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(178),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(2),
  MAP_CMD_GOTO(STAGE3_MAP_LANES_C6DC_ADDR)
};

/** $C5C4: stage3_map_hazards_C5C4 */
static const u8 stage3_map_hazards_C5C4[] = {
  MAP_HAZARD_WAIT(96),
  MAP_CMD_GOTO(STAGE3_MAP_HAZARDS_C6FC_ADDR)
};

/** $C5C9: stage3_map_lobjs_C5C9 */
static const u8 stage3_map_lobjs_C5C9[] = {
  MAP_OBJ_NONE(12),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(3),
  MAP_CMD_GOTO(STAGE3_MAP_LOBJS_C71F_ADDR)
};

/** $C620: stage3_map_robjs_C620 */
static const u8 stage3_map_robjs_C620[] = {
  MAP_OBJ_NONE(12),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(3),
  MAP_CMD_GOTO(STAGE3_MAP_ROBJS_C7AD_ADDR)
};

/** $C677: stage3_map_curv_C677 */
static const u8 stage3_map_curv_C677[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT(9),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(9),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(9),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(10),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(2),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(11),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(9),
  MAP_CMD_GOTO(STAGE3_MAP_CURV_C677_ADDR)
};

/** $C69D: stage3_map_height_C69D */
static const u8 stage3_map_height_C69D[] = {
  MAP_HEIGHT_LEVEL(14),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_UP3(15),
  MAP_HEIGHT_UP3(15),
  MAP_HEIGHT_UP3(10),
  MAP_HEIGHT_DOWN5(15),
  MAP_HEIGHT_DOWN5(2),
  MAP_HEIGHT_DOWN3(6),
  MAP_HEIGHT_DOWN1(5),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN5(2),
  MAP_HEIGHT_DOWN7(5),
  MAP_HEIGHT_DOWN5(6),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_DOWN1(2),
  MAP_HEIGHT_UP1(2),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_UP5(4),
  MAP_HEIGHT_UP7(10),
  MAP_HEIGHT_DOWN7(1),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(2),
  MAP_HEIGHT_DOWN7(1),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(6),
  MAP_HEIGHT_UP3(3),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_UP7(4),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN7(3),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(3),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(14),
  MAP_HEIGHT_DOWN1(4),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(1),
  MAP_CMD_GOTO(STAGE3_MAP_HEIGHT_C69D_ADDR)
};

/** $C6DC: stage3_map_lanes_C6DC */
static const u8 stage3_map_lanes_C6DC[] = {
  MAP_LANES_4(82),
  MAP_LANES_4TO3R(8),
  MAP_LANES_3RTO4(2),
  MAP_LANES_4(50),
  MAP_LANES_4TO3R(4),
  MAP_LANES_3RTO4(2),
  MAP_LANES_4(10),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(2),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(250),
  MAP_LANES_3L(26),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(2),
  MAP_CMD_GOTO(STAGE3_MAP_LANES_C6DC_ADDR)
};

/** $C6FC: stage3_map_hazards_C6FC */
static const u8 stage3_map_hazards_C6FC[] = {
  MAP_HAZARD_WAIT(18),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(79),
  MAP_CMD_START_BARRIERS_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(19),
  MAP_CMD_START_BARRIERS_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(85),
  MAP_CMD_START_BARRIERS_R,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(8),
  MAP_CMD_START_OBSTACLE_L,
  MAP_HAZARD_WAIT(3),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(3),
  MAP_CMD_GOTO(STAGE3_MAP_HAZARDS_C6FC_ADDR)
};

/** $C71F: stage3_map_lobjs_C71F */
static const u8 stage3_map_lobjs_C71F[] = {
  MAP_OBJ_NONE(4),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(6),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(11),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_CMD_GOTO(STAGE3_MAP_LOBJS_C71F_ADDR)
};

/** $C7AD: stage3_map_robjs_C7AD */
static const u8 stage3_map_robjs_C7AD[] = {
  MAP_OBJ_NONE(4),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(10),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(2),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S3_TOWER_BLOCK(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(13),
  MAP_OBJ_S3_SPEED_LIMIT_SIGN(1),
  MAP_OBJ_NONE(5),
  MAP_CMD_GOTO(STAGE3_MAP_ROBJS_C7AD_ADDR)
};

/**
 * $C82C: stage3_perp_face
 */
static const pixel_t stage3_perp_face[FACEBYTES] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, X_XXXXX_, XXX_XXX_, XXX__XXX,
  XXXXXXXX, X_XX_XX_, XX__XX_X, XX_XXXXX,
  XXXXXXXX, _XXXXX_X, XX_X_XXX, XXXXXXXX,
  XXXXXXXX, XXX__XX_, _XXXX_XX, XXXXX_XX,
  XXXXXXXX, XX______, _____X_X, XXX__X_X,
  XXXXXXXX, X_X_____, ______XX, XXX_XX_X,
  XXXXXXXX, XX______, _______X, XXXX__XX,
  X_X_XXXX, X_______, ________, XXXXX__X,
  XXXXXXXX, XX______, _______X, XXXX_X_X,
  XX_XXXXX, X_X_____, _______X, XX_XXX_X,
  X_XXXXXX, XX______, ____XX__, XXX__X_X,
  XX_XXXXX, XXX_____, XXXXXXX_, XX__X_XX,
  XX__XXXX, XXXXX___, XXXX___X, XXX___XX,
  X___XXX_, _XXXX___, XXXXX___, X_X____X,
  X__XXX__, XXXXXX__, XX_X_X__, X__X___X,
  X_XXXX_X, _X_XXX__, _XX_____, XX_X___X,
  X_XXXX__, __XXX___, _XXXX___, XX_X___X,
  X_XXXXX_, XX_XXX__, _XX_____, _X_X___X,
  X__XXX__, __XXXX__, __XX____, _XX____X,
  X__XX_X_, ____X___, __XX____, _XX____X,
  X__XXX__, ___XXX__, _XXXX___, _X_X___X,
  X___XXX_, ___X_XXX, X___X___, X_X____X,
  X___XX__, __X_X___, _____X__, X_X____X,
  X____XX_, __XX____, _____X__, X_X____X,
  X_____XX, _XXXX_XX, XXX_____, X______X,
  X_____X_, __XXXX__, ___XX___, X______X,
  X_____XX, _XXXX_XX, XX_____X, _______X,
  X______X, X_XXXXXX, XXX____X, _______X,
  X______X, _X_XXXXX, X_XX___X, _______X,
  X_______, X_XXX_X_, ______XX, _______X,
  X_______, XX_XXX__, ______XX, X______X,
  X_______, _XXX_XX_, _____X_X, _XXXX__X,
  X_______, XXXXXX_X, _X__X__X, _____XXX,
  X______X, XXXXXXXX, XXXX____, X______X,
  X____XXX, XXXXXXXX, XXX_____, X______X,
  X__XXXXX, XXXXXX_X, ________, X______X,
  XXX_XXXX, XXXXXXX_, _______X, _______X,
  XX_X_X_X, XXX_X_X_, X_X____X, _______X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,

  attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_CYAN,
  attribute_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_CYAN,
  attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_CYAN,
  attribute_BRIGHT_BLACK_OVER_CYAN, attribute_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_CYAN,
  attribute_BRIGHT_BLACK_OVER_CYAN, attribute_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_WHITE,
};

/* ----------------------------------------------------------------------- */

/** $C8E0: stage3_lods_C8E0 */
static const bitmap_t stage3_lods_C8E0[6] = {
  { 6, BITMAPFLAG_DEFAULT, 30, &bitmap_C95E[0], &bitmap_C95E[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 20, &bitmap_CA12[0], &bitmap_CA12[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 13, &bitmap_CA62[0], &bitmap_CA62[0] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 13, &bitmap_CA62[0], &bitmap_CA62[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 8, &bitmap_CA89[0], &bitmap_CA89[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 8, &bitmap_CA89[0], &bitmap_CAA9[0] },  // [5]
};

/** $C90A: stage3_lods_C90A */
static const bitmap_t stage3_lods_C90A[6] = {
  { 6, BITMAPFLAG_DEFAULT, 29, &stage3_bitmap_CAC9[0], &stage3_bitmap_CAC9[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 19, &stage3_bitmap_CB77[0], &stage3_bitmap_CB77[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 14, &stage3_bitmap_CBC3[0], &stage3_bitmap_CBC3[0] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 14, &stage3_bitmap_CBC3[0], &stage3_bitmap_CBC3[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 8, &bitmap_CA89[0], &bitmap_CAA9[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 8, &bitmap_CA89[0], &bitmap_CAA9[0] },  // [5]
};

/** $C934: stage3_lods_C934 */
static const bitmap_t stage3_lods_C934[6] = {
  { 6, BITMAPFLAG_DEFAULT, 31, &bitmap_car_1[0], &bitmap_car_1[0] },  // [0]
  { 5, BITMAPFLAG_DEFAULT, 22, &bitmap_car_2[0], &bitmap_car_2[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 16, &bitmap_car_3[0], &bitmap_car_3[0] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 16, &bitmap_car_3[0], &bitmap_car_3[0] },  // [3]
  { 3, BITMAPFLAG_MASKED, 9, &bitmap_car_4[0], &bitmap_car_4s[0] },  // [4]
  { 3, BITMAPFLAG_MASKED, 9, &bitmap_car_4[0], &bitmap_car_4s[0] },  // [5]
};

/* $C95E: bitmap_C95E - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $CA12: bitmap_CA12 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $CA62: bitmap_CA62 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $CA89: bitmap_CA89 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $CAA9: bitmap_CAA9 - Conv: Deduped to CommonData.c (shared with stage 5) */

/**
 * $CAC9: stage3_bitmap_CAC9
 */
static const pixel_t stage3_bitmap_CAC9[6 * 1 * 29 * 1] = {
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  _XXXXX__, X_X_XXXX, XXXXXXXX, XXXXXXXX, XXX_X_X_, _XXXXX__,
  XXXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXXX_,
  XXXXX_X_, _X_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X__, X_XXXXX_,
  XXXXX___, X_X_____, ______X_, X_X_____, ____X_X_, __XXXXX_,
  _XXX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XXX__,
  _XX_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_XX__,
  XX_X_X__, ________, ________, ________, ________, _X_X_XX_,
  X_______, ________, ________, ________, ________, ______X_,
  X_______, ________, ________, ________, ________, ______X_,
  X____X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, _X____X_,
  _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_X__,
  _X_X____, __X_X___, _X______, _____X__, __X_X___, ___X_X__,
  _X_X_X_X, _X_XX___, _X______, _____X__, __XX_X_X, _X_X_X__,
  _X_XX_X_, X_X_X_X_, XX______, _____XX_, X_X_X_X_, X_XX_X__,
  _X_X____, _X_XXXXX, XX______, _____XXX, XXXX_X__, ___X_X__,
  __X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_X___,
  __X_X___, ________, ________, ________, ________, __X_X___,
  ___XXX_X, ________, ________, ________, ________, X_XX____,
  ___X_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_X____,
  __X_____, X__XX_X_, X_X_X___, __X_X_X_, X_XX__X_, ____X___,
  __X_____, X__XX___, _____X__, _X______, __XX__X_, ____X___,
  ___XXXXX, _X_XX___, _____X__, _X______, __XX_X_X, XXXX____,
  ________, _X__XXXX, XXXXX___, __XXXXXX, XXX__X__, ________,
  ________, __X_XXXX, XXX_____, ________, __X_X___, ________,
  ________, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__, ________,
  ________, X______X, XX______, ________, ______X_, ________,
  ________, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__, ________,
};

/**
 * $CB77: stage3_bitmap_CB77
 */
static const pixel_t stage3_bitmap_CB77[4 * 1 * 19 * 1] = {
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  XXXX_X_X, XXXXXXXX, XXXXXXXX, X_X_XXXX,
  XXX_XXXX, XX_X_X__, __X_X_XX, XXXX_XXX,
  XXX___X_, X_X_X_XX, XX_X_X_X, _X___XXX,
  _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXXX_X_,
  XXX_____, ________, ________, _____X_X,
  X____X_X, _X_X_X_X, _X_X_X_X, _X_X___X,
  X__XXXXX, XXXXXXXX, XXXXXXXX, XXXXX__X,
  X_X_X_X_, X___X___, ___X___X, _XX_XX_X,
  X_XX_XXX, X___X___, ___X___X, XX_X_X_X,
  X_X___XX, XXXXX___, ___XXXXX, XX___X_X,
  _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXXX_X_,
  __XX____, ________, ________, ____XX__,
  __X_XXXX, XXXXXXXX, XXXXXXXX, XXXX_X__,
  _X___X__, X_____X_, _X_____X, __X___X_,
  __XXX_X_, X_____X_, _X_____X, _X_XXX__,
  _______X, XXXXXX__, __XXXXXX, X_______,
  ______X_, __XXX___, ________, _X______,
  ______XX, XXXXXXXX, XXXXXXXX, XX______,
};

/**
 * $CBC3: stage3_bitmap_CBC3
 */
static const pixel_t stage3_bitmap_CBC3[3 * 1 * 14 * 1] = {
  _XXXXXXX, XXXXXXXX, XXXXXX__,
  XX__XXXX, XXXXXXXX, XXX__XX_,
  X_XXXXXX, XXXXXXXX, XXXXX_X_,
  X__X_X_X, _X_X_X_X, _X_X__X_,
  X_______, ________, ______X_,
  XXXXXXXX, XXXXXXXX, XXXXXXX_,
  X_X_XX__, X____X__, _XX_X_X_,
  XX_X_XXX, X____XXX, XX_X_XX_,
  _XX_____, ________, ____XX__,
  X__XXXXX, XXXXXXXX, XXXX__X_,
  _XXXXX__, __X_X___, _XXXXX__,
  ____XXXX, XXXXXXXX, XXX_____,
  ____X__X, X_______, __X_____,
  _____XXX, XXXXXXXX, XX______,
};

/* $CA89: bitmap_CA89 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $CAA9: bitmap_CAA9 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $CC2D: bitmap_car_1 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CCE7: bitmap_car_2 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CD55: bitmap_car_3 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CD85: bitmap_car_4 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CDBB: bitmap_car_4s - Conv: Deduped to CommonData.c (shared with stage 1) */

/** $CDF1: stage3_lods_CDF1 */
static const bitmap_t stage3_lods_CDF1[SPRITE_FRAMES] = {
  { 2, BITMAPFLAG_MASKED, 5, &bitmap_stones_1[0], &bitmap_stones_1[0] },  // [0]
  { 2, BITMAPFLAG_MASKED, 4, &bitmap_stones_2[0], &bitmap_stones_2s[0] },  // [1]
  { 2, BITMAPFLAG_MASKED, 3, &bitmap_stones_3[0], &bitmap_stones_3s[0] },  // [2]
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_stones_4[0], &bitmap_stones_4s[0] },  // [3]
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_stones_4[0], &bitmap_stones_4s[0] },  // [4]
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_stones_5[0], &bitmap_stones_5s[0] },  // [5]
};

/** $CE1B: stage3_lods_CE1B */
static const bitmap_t stage3_lods_CE1B[SPRITE_FRAMES] = {
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_dust_1[0], &bitmap_dust_1s[0] },  // [0]
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_dust_1[0], &bitmap_dust_1s[0] },  // [1]
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_dust_1[0], &bitmap_dust_1s[0] },  // [2]
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_dust_1[0], &bitmap_dust_1s[0] },  // [3]
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_dust_1[0], &bitmap_dust_1s[0] },  // [4]
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_dust_1[0], &bitmap_dust_1s[0] },  // [5]
};

/* $CE45: bitmap_stones_1 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CE59: bitmap_stones_2 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CE69: bitmap_stones_2s - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CE79: bitmap_stones_3 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CE85: bitmap_stones_3s - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CE91: bitmap_stones_4 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CE95: bitmap_stones_4s - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CE99: bitmap_stones_5 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CE9B: bitmap_stones_5s - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CE9D: bitmap_dust_1 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CE9F: bitmap_dust_1s - Conv: Deduped to CommonData.c (shared with stage 1) */

/** $CEA1: stage3_stretchy_CEA1 */
static const stretchy_t stage3_stretchy_CEA1[4] = {
  { STRETCHY_TYPE_FIXED, &stage3_depthset_CEAB },
  { STRETCHY_TYPE_200PC, &stage3_depthset_CEC1 },
  { STRETCHY_TYPE_FIXED, &stage3_depthset_CED7 },
  { STRETCHY_TYPE_END, NULL },
};

/** $CEAB: stage3_depthset_CEAB */
static const depthset_t stage3_depthset_CEAB = {
  &stage3_lods_CEED[0],
  {
    0x48, 0x02,
    0x38, 0x09,
    0x30, 0x09,
    0x28, 0x10,
    0x20, 0x10,
    0x20, 0x17,
    0x1C, 0x17,
    0x18, 0x1E,
    0x14, 0x1E,
    0x10, 0x25,
  }
};

/** $CEC1: stage3_depthset_CEC1 */
static const depthset_t stage3_depthset_CEC1 = {
  &stage3_lods_CEED[0],
  {
    0x48, 0x2C,
    0x38, 0x33,
    0x30, 0x33,
    0x28, 0x3A,
    0x20, 0x3A,
    0x20, 0x41,
    0x1C, 0x41,
    0x18, 0x48,
    0x14, 0x48,
    0x10, 0x4F,
  }
};

/** $CED7: stage3_depthset_CED7 */
static const depthset_t stage3_depthset_CED7 = {
  &stage3_lods_CEED[0],
  {
    0x48, 0x56,
    0x38, 0x5D,
    0x30, 0x5D,
    0x28, 0x64,
    0x20, 0x64,
    0x20, 0x6B,
    0x1C, 0x6B,
    0x18, 0x72,
    0x14, 0x72,
    0x10, 0x79,
  }
};

/** $CEED: stage3_lods_CEED */
static const bitmap_t stage3_lods_CEED[18] = {
  { 8, BITMAPFLAG_DEFAULT, 32, &stage3_bitmap_CF6B[0], &stage3_bitmap_CF6B[0] },  // [0]
  { 6, BITMAPFLAG_DEFAULT, 25, &stage3_bitmap_D0AB[0], &stage3_bitmap_D0AB[0] },  // [1]
  { 5, BITMAPFLAG_DEFAULT, 20, &stage3_bitmap_D17D[0], &stage3_bitmap_D17D[0] },  // [2]
  { 4, BITMAPFLAG_DEFAULT, 14, &stage3_bitmap_D20E[0], &stage3_bitmap_D20E[0] },  // [3]
  { 3, BITMAPFLAG_DEFAULT, 11, &stage3_bitmap_D262[0], &stage3_bitmap_D262[0] },  // [4]
  { 2, BITMAPFLAG_DEFAULT, 8, &stage3_bitmap_D295[0], &stage3_bitmap_D295[0] },  // [5]
  { 8, BITMAPFLAG_DEFAULT, 8, &stage3_bitmap_D06B[0], &stage3_bitmap_D06B[0] },  // [6]
  { 6, BITMAPFLAG_DEFAULT, 6, &stage3_bitmap_D141[0], &stage3_bitmap_D141[0] },  // [7]
  { 5, BITMAPFLAG_DEFAULT, 5, &stage3_bitmap_D1E1[0], &stage3_bitmap_D1E1[0] },  // [8]
  { 4, BITMAPFLAG_DEFAULT, 4, &stage3_bitmap_D246[0], &stage3_bitmap_D246[0] },  // [9]
  { 3, BITMAPFLAG_DEFAULT, 4, &stage3_bitmap_D283[0], &stage3_bitmap_D283[0] },  // [10]
  { 2, BITMAPFLAG_DEFAULT, 3, &stage3_bitmap_D2A5[0], &stage3_bitmap_D2A5[0] },  // [11]
  { 8, BITMAPFLAG_DEFAULT, 8, &stage3_bitmap_D06B[0], &stage3_bitmap_D06B[0] },  // [12]
  { 6, BITMAPFLAG_DEFAULT, 4, &stage3_bitmap_D165[0], &stage3_bitmap_D165[0] },  // [13]
  { 5, BITMAPFLAG_DEFAULT, 4, &stage3_bitmap_D1FA[0], &stage3_bitmap_D1FA[0] },  // [14]
  { 4, BITMAPFLAG_DEFAULT, 3, &stage3_bitmap_D256[0], &stage3_bitmap_D256[0] },  // [15]
  { 3, BITMAPFLAG_DEFAULT, 2, &stage3_bitmap_D28F[0], &stage3_bitmap_D28F[0] },  // [16]
  { 2, BITMAPFLAG_DEFAULT, 2, &stage3_bitmap_D2AB[0], &stage3_bitmap_D2AB[0] },  // [17]
};

/**
 * $CF6B: stage3_bitmap_CF6B
 */
static const pixel_t stage3_bitmap_CF6B[8 * 1 * 32 * 1] = {
  __X___X_, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X____X__, ___X___X,
  ____X__X, _XX___XX, _X_X_XXX, XXX___XX, _X_X_XXX, XXX___XX, XXXX___X, ________,
  _X______, _XX___XX, X_X_X_X_, XXX___XX, X_X_X_X_, XXX___XX, XX_XXXX_, _XX_X___,
  ____X_XX, XX_XXXXX, _X_X_X_X, _XXX__XX, _X_X_X_X, _XXXXXXX, X_X_X_XX, ____X_X_,
  X__XXX__, X_XX___X, XXXXXXXX, XXXX___X, XXXXXXXX, XXXX___X, XX_X_X_X, XX______,
  __XX____, XX_X___X, X_X_X_XX, XXXX___X, X_X_X_XX, XXXX___X, X_X_X_X_, X_XX_X__,
  _X_X____, X_XX___X, _X_X_X_X, _XXX___X, _X_X_X_X, _XXX___X, XX_X_X_X, _XXXX__X,
  X__X____, XX_XX__X, XXXXXXXX, XXXXX__X, XXXXXXXX, XXXXX___, X_X_X_X_, X_X_XX__,
  X__X____, X_X_X___, XX_X_X_X, _X_XX___, XX_X_X_X, _X_XX___, XX_X_X_X, _XXXXXX_,
  X__X____, XX_XX___, XXX_X_X_, X_X_X___, XXX_X_X_, X_X_X___, X_X_X_X_, X_X_X_X_,
  X__X____, X_X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_X_X_X, _XXXXXXX,
  X__X____, XX_X_X__, X_XXX_X_, X_X_XX__, X_XXX_X_, X_X_XX__, X_XXX_X_, X_X_X_XX,
  X__X____, XXXXXX__, X_XXXX__, XX_X_X__, X_XXXX__, XX_X_X__, XXXXXXXX, _XXXXXXX,
  X__X__XX, XX_X_X__, X_XXXXX_, X_X_XX__, X_XXXXX_, X_X_XX__, X_XXXXX_, X_X_X_XX,
  X__XXXX_, _XXXXX__, X_XXXX__, XX_X_X__, X_XXXX__, XX_X_X__, XXX_XXXX, _XXXXXXX,
  X__X___X, XX_X_X__, X_XXXXX_, XXX_XX__, X_XXXXX_, XXX_XX__, X_XX__X_, X_X_X_XX,
  X__XXXX_, __X_XX__, X_XXXXX_, XXXX_X__, X_XXXXX_, XX_X_X__, XXXXX_XX, _XXXXXXX,
  X__X_X_X, _X_X_X__, X_XXXXX_, XXX_XX__, X_XXXXX_, XXX_XX__, X_XXX_X_, X_X_X_XX,
  X__X____, __X_XX__, X_XXXXX_, XXXX_X__, X_XXXXX_, XX_X_X__, XXX__XXX, _XXXXXXX,
  X__X_X_X, _X_X_X__, X_XXXXX_, XXXXXX__, X_XXXXX_, XXXXXX__, X_XXX_X_, X_X_X_XX,
  X__X____, __X_XX__, X_XXXXX_, XXXX_X__, X_XXXXX_, XXXX_X__, XXX__XXX, _XXXXXXX,
  X__X_X_X, _X_X_X__, X_XXXXX_, XXXXXX__, X_XXXXX_, XXXXXX__, X_XXX_X_, X_X_X_XX,
  X__X____, __X_XX__, X_XXXXX_, XXXXXX__, X_XXXXX_, XXXXXX__, XX_X_XXX, _XXXXXXX,
  X__X_X_X, _X_X_X__, X______X, _X_X_X__, X______X, _X_X_X__, X_X_X_X_, X_X_X_XX,
  X__X____, __X_XX__, XXXXXXXX, XXXXXX__, XXXXXXXX, XXXXXX__, XX_X_X_X, _XXXXXXX,
  X__X_X_X, _X_X_X__, XXXXXXXX, XXXXXX__, XXXXXXXX, XXXXXX__, X_X_X_X_, X_X_X_XX,
  X__X____, __X_XX__, XX_X_X_X, _X_X_X__, XX_X_X_X, _X_X_X__, XX_X_X_X, _XXXXXXX,
  X__X_X_X, _X_X_X__, XXXXXXXX, XXXXXX__, XXXXXXXX, XXXXXX__, X_X_X_X_, X_X_X_XX,
  X__X____, __X_XX__, X_______, _____X__, X_______, _____X__, XX_X_X_X, _XXXXXXX,
  X__X_X_X, _X_X_X__, X_______, _____X__, X_______, _____X__, X_X_X_X_, X_X_X_XX,
  X__X____, __X_XX__, X_______, _____X__, X_______, _____X__, XX_X_X_X, _XXXXXXX,
  X__X_X_X, _X_X_X__, XXXXXXXX, XXXXXX__, XXXXXXXX, XXXXXX__, X_X_X_X_, X_X_X_XX,
};

/**
 * $D06B: stage3_bitmap_D06B
 */
static const pixel_t stage3_bitmap_D06B[8 * 1 * 8 * 1] = {
  X__X____, __X_XX__, XXXXXXXX, XXXXXX__, XXXXXXXX, XXXXXX__, XX_X_X_X, _XXXXXXX,
  X__X_X_X, XXXX_X__, XX_____X, _X_X_X__, XX_____X, _X_X_X__, X_X_X_X_, X_X_X_XX,
  X__X_XX_, __XXXX__, XX_XXXX_, XXXXXX__, XX_XXXX_, XXXXXX__, XXXXXX_X, _XXXXXXX,
  X__X_X_X, XXXX_X__, XX_XXXX_, XXXX_X__, XX_XXXX_, XXXX_X__, X_XXXXXX, X_X_X_XX,
  X__X_X_X, XXXXXX__, XX_XXXX_, XXXXXX__, XX_XXXX_, XXXXXX__, XXXXXXXX, _XXXXXXX,
  X__X_X_X, XXXX_X__, XX_XXXX_, XXXX_X__, XX_XXXX_, XXXX_X__, X_XXXXXX, X_X_X_XX,
  X__X_X__, _XXXXX__, XX____X_, X_X_XX__, XX____X_, X_X_XX__, XXXXXXXX, _XXXXXXX,
  X__XXXXX, X__X_X__, XXXXXXXX, XXXXXX__, XXXXXXXX, XXXXXX__, X_X_X_XX, X_X_X_XX,
};

/**
 * $D0AB: stage3_bitmap_D0AB
 */
static const pixel_t stage3_bitmap_D0AB[6 * 1 * 25 * 1] = {
  _X__X__X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___, ____X__X,
  ___X_X_X, __X_X_X_, X_XX__X_, X_X_X_XX, __XXXXX_, _X______,
  ____XXXX, __XX_X_X, _X_X__XX, _X_X_X_X, __XXXXXX, X__X_X__,
  X_XX__X_, X__XXXXX, XXXXX__X, XXXXXXXX, X__XXX_X, _XX_____,
  _XX___X_, X__X_X_X, X_XXX__X, _X_XX_XX, X__XXXXX, XXXXX___,
  X_X___X_, X__X__X_, _X__X__X, __X__X__, X__XXX_X, _X_XXX_X,
  X_X___X_, XX__XXXX, XXXXXX_X, XXXXXXXX, XX__XXXX, XXXXXX__,
  X_X___X_, XX__XX_X, X_XX_X__, XX_XX_XX, _X__XX_X, _X_XXXX_,
  X_X___X_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_X___X_, XX__XXXX, X_X_X___, XXXXX_X_, X___XX_X, _X_XXXXX,
  X_X_XXX_, X___XXXX, X_XX_X__, XXXXX_XX, _X__XXXX, XXXXXXXX,
  X_XXX__X, XX__XXXX, __X_X___, XXXX__X_, X___XX_X, _X_XXXXX,
  X_X__XX_, X___XXXX, X_XX_X__, XXXXX_XX, _X__XXX_, _XXXXXXX,
  X_XXX__X, _X__XXXX, X_XXX___, XXXXX_XX, X___XXXX, X__XXXXX,
  X_X__X__, X___XXXX, X_XXXX__, XXXXX_XX, XX__XXXX, X_XXXXXX,
  X_X____X, _X__XXXX, X_XXX___, XXXXX_XX, X___XXXX, X__XXXXX,
  X_X__X__, X___XXXX, X_XXXX__, XXXXX_XX, XX__XXXX, X_XXXXXX,
  X_X____X, _X__XXXX, X_XXX___, XXXXX_XX, X___XXXX, ___XXXXX,
  X_X__X__, X___X___, _X_X_X__, X____X_X, _X__XXXX, XXXXXXXX,
  X_X____X, _X__XXXX, XXXXXX__, XXXXXXXX, XX__XX_X, _X_XXXXX,
  X_X__X__, X___XXXX, XXXXXX__, XXXXXXXX, XX__XXXX, XXXXXXXX,
  X_X____X, _X__XXXX, XXXXXX__, XXXXXXXX, XX__XX_X, _X_XXXXX,
  X_X__X__, X___X___, ________, X_______, ____XXXX, XXXXXXXX,
  X_X____X, _X__X___, ________, X_______, ____XX_X, _X_XXXXX,
  X_X__X__, X___XXXX, XXXXXX__, XXXXXXXX, XX__XXXX, XXXXXXXX,
};

/**
 * $D141: stage3_bitmap_D141
 */
static const pixel_t stage3_bitmap_D141[6 * 1 * 6 * 1] = {
  X_X___XX, X___X___, _X__X___, X____X__, X___XX_X, _X_XXXXX,
  X_X_XXXX, XX__X_XX, X_XXXX__, X_XXX_XX, XX__XXXX, XXXX_XXX,
  X_X_X_XX, X___X_XX, X_XXXX__, X_XXX_XX, XX__XX_X, XXXXXXXX,
  X_X_X___, XX__X_XX, X_XXX___, X_XXX_XX, X___XXXX, XXXX_XXX,
  X_X_XXXX, ____X___, X_XX_X__, X___X_XX, _X__XX_X, _X_XXXXX,
  X_X_____, _X__XXXX, XXXXXX__, XXXXXXXX, XX__XXXX, XXXX_XXX,
};

/**
 * $D165: stage3_bitmap_D165
 */
static const pixel_t stage3_bitmap_D165[6 * 1 * 4 * 1] = {
  X_X_____, ________, ________, ________, ____XXXX, XXXXXXXX,
  X__XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
};

/**
 * $D17D: stage3_bitmap_D17D
 */
static const pixel_t stage3_bitmap_D17D[5 * 1 * 20 * 1] = {
  X____XXX, XXXXXXXX, XXXXXXXX, XXXXXX__, _X__X__X,
  ___XX__X, _X_X_X_X, __X_X_X_, XX__XXXX, X_____X_,
  _XX_X__X, XXXXXXXX, __XXXXXX, XX__XXXX, XXXX____,
  X___X__X, X_X_X_XX, __XX_X_X, _X__XXXX, XXXXXX_X,
  X___XX__, XXXXXXXX, X__XXXXX, XXX__XXX, XXXXXXX_,
  X___XX__, XX_X_X__, X__X_X_X, __X__XXX, XXXXXXX_,
  X___X_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X___XX_X, _XXXX_X_, X__XXXX_, XX_X_XXX, XXXXXXXX,
  X__XX_XX, _XXXX_XX, _X_XXXX_, XXX__XXX, XXXXXXXX,
  XXX__X_X, _XXXX_XX, X__XXXX_, XX_X_XXX, XXXXXXXX,
  X_____XX, _XXXX_XX, _X_XXXX_, XXX__XXX, XXXXXXXX,
  X____X_X, _XXXX_XX, X__XXXX_, XXXX_XXX, XXXXXXXX,
  X_____XX, _XXXX_XX, XX_XXXX_, XXX__XXX, XXXXXXXX,
  X____X_X, _XXXX_XX, XX_XXXX_, XXXX_XXX, XXXXXXXX,
  X_____XX, _X___X_X, _X_X___X, _X_X_XXX, XXXXXXXX,
  X____X_X, _XXXXXXX, XX_XXXXX, XXXX_XXX, XXXXXXXX,
  X_____XX, _XXXXXXX, XX_XXXXX, XXXX_XXX, XXXXXXXX,
  X____X_X, _X______, ___X____, _____XXX, XXXXXXXX,
  X_____XX, _X______, ___X____, _____XXX, XXXXXXXX,
  X____X_X, _XXXXXXX, XX_XXXXX, XXXX_XXX, XXXXXXXX,
};

/**
 * $D1E1: stage3_bitmap_D1E1
 */
static const pixel_t stage3_bitmap_D1E1[5 * 1 * 5 * 1] = {
  X___XXXX, _X_____X, XX_X____, _XXX_XXX, XXXXXXXX,
  X_XXXXXX, _X_XXXXX, XX_X_XXX, XXXX_XXX, XXXXXXXX,
  X_XXXXXX, _X_XXXXX, XX_X_XXX, XXXX_XXX, XXXXXXXX,
  X_XXX___, _X___XX_, _X_X___X, X__X_XXX, XXXXXXXX,
  X_______, _XXXXXXX, XX_XXXXX, XXXX_XXX, XXXXXXXX,
};

/**
 * $D1FA: stage3_bitmap_D1FA
 */
static const pixel_t stage3_bitmap_D1FA[5 * 1 * 4 * 1] = {
  X_______, ________, ________, _____XXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ________, ________, ________, _______X,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
};

/**
 * $D20E: stage3_bitmap_D20E
 */
static const pixel_t stage3_bitmap_D20E[4 * 1 * 14 * 1] = {
  _____X_X, XXXXXX_X, XXXXXX_X, XX______,
  __XXXX_X, _X_X_X_X, _X_X_X_X, XXXXX__X,
  _X___X__, XXXXXX__, XXXXXX__, XXXXXXX_,
  X____XX_, X_X_X_X_, X_X_X_X_, XXXXXXXX,
  X____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X____X__, XXX_XXX_, XXX_XXX_, XXXXXXXX,
  X__XX_X_, XXX_XX__, XXX_XX__, XXXXXXXX,
  X_______, XXX_XXX_, XXX_XXX_, XXXXXXXX,
  X_____X_, XXX_XX__, XXX_XX__, XXXXXXXX,
  X_______, X___X_X_, X___X_X_, XXXXXXXX,
  X_____X_, XXXXXXX_, XXXXXXX_, XXXXXXXX,
  X_______, X_______, X_______, XXXXXXXX,
  X_____X_, X_______, X_______, XXXXXXXX,
  X_______, XXXXXXX_, XXXXXXX_, XXXXXXXX,
};

/**
 * $D246: stage3_bitmap_D246
 */
static const pixel_t stage3_bitmap_D246[4 * 1 * 4 * 1] = {
  X__XXXX_, X__XXXX_, X__XXXX_, XXXXXXXX,
  X_XXXXX_, X_XXXX__, X_XXXX__, XXXXXXXX,
  X_XXXX__, XXXXXXX_, XXXXXXX_, XXXXXXXX,
  X_____X_, ________, ________, XXXXXXXX,
};

/**
 * $D256: stage3_bitmap_D256
 */
static const pixel_t stage3_bitmap_D256[4 * 1 * 3 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ________, ________, _______X,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
};

/**
 * $D262: stage3_bitmap_D262
 */
static const pixel_t stage3_bitmap_D262[3 * 1 * 11 * 1] = {
  ___X_XXX, XX_XXXXX, _XXXX___,
  _XXX_XX_, XX_XX_XX, _XXXXXX_,
  XX_XX_XX, __X_XX__, X_XXXXXX,
  XX_X__XX, XXX_XXXX, X_XXXXXX,
  XX_XX_XX, XXX_XXXX, X_XXXXXX,
  XXX___XX, XXX_XXXX, X_XXXXXX,
  XX__X_XX, XXX_XXXX, X_XXXXXX,
  XX____XX, XXX_XXXX, X_XXXXXX,
  XX__X_X_, ____X___, __XXXXXX,
  XX____XX, XXX_XXXX, X_XXXXXX,
  XX__X_XX, XXX_XXXX, X_XXXXXX,
};

/**
 * $D283: stage3_bitmap_D283
 */
static const pixel_t stage3_bitmap_D283[3 * 1 * 4 * 1] = {
  X___X___, ________, __XXXXXX,
  X_X_X_XX, XX__XXXX, __XXXXXX,
  XXX_X_X_, ____X___, __XXXXXX,
  XX__X_XX, XXX_XXXX, X_XXXXXX,
};

/**
 * $D28F: stage3_bitmap_D28F
 */
static const pixel_t stage3_bitmap_D28F[3 * 1 * 2 * 1] = {
  XX______, ________, __XX__XX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
};

/**
 * $D295: stage3_bitmap_D295
 */
static const pixel_t stage3_bitmap_D295[2 * 1 * 8 * 1] = {
  __XX____, ___XXX__,
  XXXXX__X, X__XXXXX,
  X_X_XXXX, XXXXXXXX,
  X__XXXX_, XXX_XXXX,
  X___XXX_, XXX_XXXX,
  X__XXXX_, XXX_XXXX,
  X_X_X___, X___XXXX,
  X__XXXXX, XXXXXXXX,
};

/**
 * $D2A5: stage3_bitmap_D2A5
 */
static const pixel_t stage3_bitmap_D2A5[2 * 1 * 3 * 1] = {
  X__X____, ____XXXX,
  X__XXXX_, XXX_XXXX,
  X__XXXXX, XXXXXXXX,
};

/**
 * $D2AB: stage3_bitmap_D2AB
 */
static const pixel_t stage3_bitmap_D2AB[2 * 1 * 2 * 1] = {
  X_______, _______X,
  _XXXXXXX, XXXXXXX_,
};

/** $D2BB: stage3_stretchy_D2BB */
static const stretchy_t stage3_stretchy_D2BB[3] = {
  { STRETCHY_TYPE_150PC, &stage3_depthset_D2C2 },
  { STRETCHY_TYPE_FIXED, &stage3_depthset_D2D8 },
  { STRETCHY_TYPE_END, NULL },
};

/** $D2C2: stage3_depthset_D2C2 */
static const depthset_t stage3_depthset_D2C2 = {
  &stage3_lods_D2EE[0],
  {
    0x24, 0x02,
    0x1C, 0x02,
    0x18, 0x09,
    0x18, 0x09,
    0x14, 0x10,
    0x14, 0x10,
    0x14, 0x17,
    0x10, 0x17,
    0x10, 0x17,
    0x10, 0x17,
  }
};

/** $D2D8: stage3_depthset_D2D8 */
static const depthset_t stage3_depthset_D2D8 = {
  &stage3_lods_D2EE[0],
  {
    0x24, 0x1E,
    0x1C, 0x1E,
    0x18, 0x25,
    0x18, 0x25,
    0x14, 0x2C,
    0x14, 0x2C,
    0x14, 0x33,
    0x10, 0x33,
    0x10, 0x33,
    0x10, 0x33,
  }
};

/** $D2EE: stage3_lods_D2EE */
static const bitmap_t stage3_lods_D2EE[8] = {
  { 4, BITMAPFLAG_DEFAULT, 16, &bitmap_D467[0], &bitmap_D467[0] },  // [0]
  { 3, BITMAPFLAG_DEFAULT, 12, &bitmap_D576[0], &bitmap_D576[0] },  // [1]
  { 2, BITMAPFLAG_DEFAULT, 8, &bitmap_D604[0], &bitmap_D604[0] },  // [2]
  { 1, BITMAPFLAG_DEFAULT, 4, &bitmap_D63D[0], &bitmap_D63D[0] },  // [3]
  { 4, BITMAPFLAG_DEFAULT, 24, &bitmap_D4A7[0], &bitmap_D4A7[0] },  // [4]
  { 3, BITMAPFLAG_DEFAULT, 18, &bitmap_D59A[0], &bitmap_D59A[0] },  // [5]
  { 2, BITMAPFLAG_DEFAULT, 12, &bitmap_D614[0], &bitmap_D614[0] },  // [6]
  { 1, BITMAPFLAG_DEFAULT, 6, &bitmap_D641[0], &bitmap_D641[0] },  // [7]
};

/** $D326: stage3_stretchy_D326 */
static const stretchy_t stage3_stretchy_D326[3] = {
  { STRETCHY_TYPE_150PC, &stage3_depthset_D32D },
  { STRETCHY_TYPE_FIXED, &stage3_depthset_D361 },
  { STRETCHY_TYPE_END, NULL },
};

/** $D32D: stage3_depthset_D32D */
static const depthset_t stage3_depthset_D32D = {
  &stage3_lods_D377[0],
  {
    0x24, 0x02,
    0x1C, 0x02,
    0x18, 0x09,
    0x18, 0x09,
    0x14, 0x10,
    0x14, 0x10,
    0x14, 0x17,
    0x10, 0x17,
    0x10, 0x17,
    0x10, 0x17,
  },
  &stage3_overhead_spans_D343[0]
};

/**
 * $D343: overhead-bridge span table (Conv: modelled explicitly; see
 * overhead_span_t in Stages.h). Each entry's fill_bytes points into
 * bitmap_D3AF, which is defined further below.
 */
static const overhead_span_t stage3_overhead_spans_D343[10] = {
  { 0x18, &bitmap_D44F[0] },
  { 0x18, &bitmap_D44F[0] },
  { 0x12, &bitmap_D564[0] },
  { 0x12, &bitmap_D564[0] },
  { 0x0C, &bitmap_D5F8[0] },
  { 0x0C, &bitmap_D5F8[0] },
  { 0x07, &bitmap_D636[0] },
  { 0x07, &bitmap_D636[0] },
  { 0x07, &bitmap_D636[0] },
  { 0x07, &bitmap_D636[0] },
};

/** $D361: stage3_depthset_D361 */
static const depthset_t stage3_depthset_D361 = {
  &stage3_lods_D377[0],
  {
    0x24, 0x1E,
    0x1C, 0x1E,
    0x18, 0x25,
    0x18, 0x25,
    0x14, 0x2C,
    0x14, 0x2C,
    0x14, 0x33,
    0x10, 0x33,
    0x10, 0x33,
    0x10, 0x33,
  }
};

/** $D377: stage3_lods_D377 */
static const bitmap_t stage3_lods_D377[8] = {
  { 4, BITMAPFLAG_DEFAULT, 16, &bitmap_D3AF[0], &bitmap_D3AF[0] },  // [0]
  { 3, BITMAPFLAG_DEFAULT, 12, &bitmap_D507[0], &bitmap_D507[0] },  // [1]
  { 2, BITMAPFLAG_DEFAULT, 8, &bitmap_D5D0[0], &bitmap_D5D0[0] },  // [2]
  { 1, BITMAPFLAG_DEFAULT, 4, &bitmap_D62C[0], &bitmap_D62C[0] },  // [3]
  { 4, BITMAPFLAG_DEFAULT, 24, &bitmap_D3EF[0], &bitmap_D3EF[0] },  // [4]
  { 3, BITMAPFLAG_DEFAULT, 18, &bitmap_D52B[0], &bitmap_D52B[0] },  // [5]
  { 2, BITMAPFLAG_DEFAULT, 12, &bitmap_D5E0[0], &bitmap_D5E0[0] },  // [6]
  { 1, BITMAPFLAG_DEFAULT, 6, &bitmap_D630[0], &bitmap_D630[0] },  // [7]
};

/* $D3AF: bitmap_D3AF - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D3EF: bitmap_D3EF - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D44F: bitmap_D44F - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D467: bitmap_D467 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D4A7: bitmap_D4A7 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D507: bitmap_D507 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D52B: bitmap_D52B - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D564: bitmap_D564 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D576: bitmap_D576 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D59A: bitmap_D59A - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D5D0: bitmap_D5D0 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D5E0: bitmap_D5E0 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D5F8: bitmap_D5F8 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D604: bitmap_D604 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D614: bitmap_D614 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D62C: bitmap_D62C - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D630: bitmap_D630 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D636: bitmap_D636 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D63D: bitmap_D63D - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $D641: bitmap_D641 - Conv: Deduped to CommonData.c (shared with stage 5) */

/** $D64F: stage3_lods_D64F */
static const bitmap_t stage3_lods_D64F[6] = {
  { 2, BITMAPFLAG_DEFAULT, 16, &bitmap_tumbleweed_1[0], &bitmap_tumbleweed_1[0] },  // [0]
  { 2, BITMAPFLAG_DEFAULT, 11, &bitmap_tumbleweed_2[0], &bitmap_tumbleweed_2[0] },  // [1]
  { 1, BITMAPFLAG_DEFAULT, 9, &bitmap_tumbleweed_3[0], &bitmap_tumbleweed_3[0] },  // [2]
  { 1, BITMAPFLAG_DEFAULT, 9, &bitmap_tumbleweed_3[0], &bitmap_tumbleweed_3[0] },  // [3]
  { 1, BITMAPFLAG_DEFAULT, 7, &bitmap_tumbleweed_4[0], &bitmap_tumbleweed_4[0] },  // [4]
  { 1, BITMAPFLAG_DEFAULT, 7, &bitmap_tumbleweed_4[0], &bitmap_tumbleweed_4[0] },  // [5]
};

/* $D679: bitmap_tumbleweed_1 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $D699: bitmap_tumbleweed_2 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $D6AF: bitmap_tumbleweed_3 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $D6B8: bitmap_tumbleweed_4 - Conv: Deduped to CommonData.c (shared with stage 1) */

/** $D6BF: stage3_lods_D6BF */
static const bitmap_t stage3_lods_D6BF[6] = {
  { 4, BITMAPFLAG_DEFAULT, 17, &bitmap_barrier_1[0], &bitmap_barrier_1[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 17, &bitmap_barrier_1[0], &bitmap_barrier_1[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 13, &bitmap_barrier_2[0], &bitmap_barrier_2[0] },  // [2]
  { 2, BITMAPFLAG_DEFAULT, 9, &bitmap_barrier_3[0], &bitmap_barrier_3[0] },  // [3]
  { 2, BITMAPFLAG_DEFAULT, 9, &bitmap_barrier_3[0], &bitmap_barrier_3[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 7, &bitmap_barrier_4[0], &bitmap_barrier_4s[0] },  // [5]
};

/* $D6E9: bitmap_barrier_1 - Conv: Deduped to CommonData.c (shared with stage 1/2/4/5) */

/* $D72D: bitmap_barrier_2 - Conv: Deduped to CommonData.c (shared with stage 1/2/4/5) */

/* $D754: bitmap_barrier_3 - Conv: Deduped to CommonData.c (shared with stage 1/2/4/5) */

/* $D766: bitmap_barrier_4 - Conv: Deduped to CommonData.c (shared with stage 1/2/4/5) */

/* $D782: bitmap_barrier_4s - Conv: Deduped to CommonData.c (shared with stage 1/2/4/5) */

/** $D79E: stage3_stretchy_D79E */
static const stretchy_t stage3_stretchy_D79E[4] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_right },
  { STRETCHY_TYPE_38PC, &streetlampmiddle_right },
  { STRETCHY_TYPE_FIXED, &stage3_depthset_D7C8 },
  { STRETCHY_TYPE_END, NULL },
};

/** $D7A8: stage3_stretchy_D7A8 */
static const stretchy_t stage3_stretchy_D7A8[4] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_left },
  { STRETCHY_TYPE_50PC, &streetlampmiddle_left },
  { STRETCHY_TYPE_FIXED, &stage3_depthset_D7B2 },
  { STRETCHY_TYPE_END, NULL },
};

/** $D7B2: stage3_depthset_D7B2 */
static const depthset_t stage3_depthset_D7B2 = {
  &stage3_lods_D7DE[0],
  {
    0x20, 0x02,
    0x18, 0x02,
    0x10, 0x09,
    0x10, 0x09,
    0x0C, 0x10,
    0x0C, 0x10,
    0x10, 0x17,
    0x10, 0x17,
    0x0C, 0x1E,
    0x0C, 0x1E,
  }
};

/** $D7C8: stage3_depthset_D7C8 */
static const depthset_t stage3_depthset_D7C8 = {
  &stage3_lods_D7DE[0],
  {
    0x20, 0x02,
    0x18, 0x02,
    0x10, 0x09,
    0x10, 0x09,
    0x0C, 0x10,
    0x0C, 0x10,
    0x08, 0x17,
    0x08, 0x17,
    0x04, 0x1E,
    0x04, 0x1E,
  }
};

/** $D7DE: stage3_lods_D7DE */
static const bitmap_t stage3_lods_D7DE[5] = {
  { 3, BITMAPFLAG_DEFAULT, 27, &stage3_bitmap_D801[0], &stage3_bitmap_D801[0] },  // [0]
  { 2, BITMAPFLAG_DEFAULT, 20, &stage3_bitmap_D852[0], &stage3_bitmap_D852[0] },  // [1]
  { 3, BITMAPFLAG_MASKED, 15, &stage3_bitmap_D87A[0], &stage3_bitmap_D8D4[0] },  // [2]
  { 2, BITMAPFLAG_MASKED, 12, &stage3_bitmap_D92E[0], &stage3_bitmap_D95E[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 9, &stage3_bitmap_D98E[0], &stage3_bitmap_D9B2[0] },  // [4]
};

/**
 * $D801: stage3_bitmap_D801
 */
static const pixel_t stage3_bitmap_D801[3 * 1 * 27 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ________, _______X,
  X____XXX, XX____XX, XXX____X,
  X___XXXX, XXX__XXX, XXXX___X,
  X___XX__, _XX__XX_, __XX___X,
  X_______, _XX_____, __XX___X,
  X_______, _XX_____, __XX___X,
  X___XXXX, XXX__XXX, XXXX___X,
  X___XXXX, XX___XXX, XXX____X,
  X___XX__, _____XX_, _______X,
  X___XX__, _____XX_, _______X,
  X___XXXX, XXX__XXX, XXXX___X,
  X___XXXX, XXX__XXX, XXXX___X,
  X_______, ________, _______X,
  X_XXXX_X, _X___X_X, ___X___X,
  X_X____X, _X_X_X_X, ___X___X,
  X_X____X, _XX_XX_X, ___X___X,
  X_X____X, _X___X_X, _XXXXX_X,
  X_______, ________, _______X,
  X_XXX_X_, __XXX_XX, X_XXX__X,
  X____XX_, __X___X_, __X__X_X,
  X__XX_XX, X_XXX_XX, X_X__X_X,
  X_X___X_, _XX___X_, __X__X_X,
  X__XXXXX, X_XXX_XX, X_XXX__X,
  X_______, ________, _______X,
  XX_X_X_X, _X_X_X_X, _X_X_X_X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
};

/**
 * $D852: stage3_bitmap_D852
 */
static const pixel_t stage3_bitmap_D852[2 * 1 * 20 * 1] = {
  XXXXXXXX, XXXXXXXX,
  X_______, _______X,
  X___XXX_, __XXX__X,
  X__XX_XX, _XX_XX_X,
  X______X, _____X_X,
  X_____XX, ____XX_X,
  X__XXXX_, _XXXX__X,
  X__X____, _X_____X,
  X__XXXXX, _XXXXX_X,
  X_______, _______X,
  XXXX_X_X, X_X__X_X,
  XX___X_X, X_X__X_X,
  XX___X_X, X_X_XXXX,
  X_______, _______X,
  XXXXX__X, XXXXXXXX,
  X__XX__X, _X__X__X,
  XX__X__X, _X__X__X,
  X_XXXXXX, XXXXXXXX,
  X_______, _______X,
  XXXXXXXX, XXXXXXXX,
};

/**
 * $D87A: stage3_bitmap_D87A
 */
static const pixel_t stage3_bitmap_D87A[3 * 2 * 15 * 1] = {
  XXX_____, ___XXXXX, ________, XXXXXXXX, XXXXXXXX, ________,
  XXX_____, ___X____, ________, _______X, XXXXXXXX, ________,
  XXX_____, ___X_XXX, ________, __XXX__X, XXXXXXXX, ________,
  XXX_____, ___X____, ________, X____X_X, XXXXXXXX, ________,
  XXX_____, ___X_XXX, ________, __XXX__X, XXXXXXXX, ________,
  XXX_____, ___X_X__, ________, __X____X, XXXXXXXX, ________,
  XXX_____, ___X_XXX, ________, X_XXXX_X, XXXXXXXX, ________,
  XXX_____, ___X____, ________, _______X, XXXXXXXX, ________,
  XXX_____, ___X_XXX, ________, _X_X_X_X, XXXXXXXX, ________,
  XXX_____, ___X_X_X, ________, XXXXXX_X, XXXXXXXX, ________,
  XXX_____, ___X____, ________, _______X, XXXXXXXX, ________,
  XXX_____, ___X_XX_, ________, XXXXXX_X, XXXXXXXX, ________,
  XXX_____, ___X__XX, ________, X_X_XX_X, XXXXXXXX, ________,
  XXX_____, ___X____, ________, _______X, XXXXXXXX, ________,
  XXX_____, ___XXXXX, ________, XXXXXXXX, XXXXXXXX, ________,
};

/**
 * $D8D4: stage3_bitmap_D8D4
 */
static const pixel_t stage3_bitmap_D8D4[3 * 2 * 15 * 1] = {
  XXXXXXX_, _______X, ________, XXXXXXXX, ____XXXX, XXXX____,
  XXXXXXX_, _______X, ________, ________, ____XXXX, ___X____,
  XXXXXXX_, _______X, ________, _XXX__XX, ____XXXX, X__X____,
  XXXXXXX_, _______X, ________, ____X___, ____XXXX, _X_X____,
  XXXXXXX_, _______X, ________, _XXX__XX, ____XXXX, X__X____,
  XXXXXXX_, _______X, ________, _X____X_, ____XXXX, ___X____,
  XXXXXXX_, _______X, ________, _XXXX_XX, ____XXXX, XX_X____,
  XXXXXXX_, _______X, ________, ________, ____XXXX, ___X____,
  XXXXXXX_, _______X, ________, _XXX_X_X, ____XXXX, _X_X____,
  XXXXXXX_, _______X, ________, _X_XXXXX, ____XXXX, XX_X____,
  XXXXXXX_, _______X, ________, ________, ____XXXX, ___X____,
  XXXXXXX_, _______X, ________, _XX_XXXX, ____XXXX, XX_X____,
  XXXXXXX_, _______X, ________, __XXX_X_, ____XXXX, XX_X____,
  XXXXXXX_, _______X, ________, ________, ____XXXX, ___X____,
  XXXXXXX_, _______X, ________, XXXXXXXX, ____XXXX, XXXX____,
};

/**
 * $D92E: stage3_bitmap_D92E
 */
static const pixel_t stage3_bitmap_D92E[2 * 2 * 12 * 1] = {
  XXX_____, ___XXXXX, ____XXXX, XXXX____,
  XXX_____, ___X____, ____XXXX, ___X____,
  XXX_____, ___XXX__, ____XXXX, XX_X____,
  XXX_____, ___X__X_, ____XXXX, __XX____,
  XXX_____, ___XXXX_, ____XXXX, XXXX____,
  XXX_____, ___XX___, ____XXXX, X__X____,
  XXX_____, ___XXXX_, ____XXXX, XXXX____,
  XXX_____, ___X____, ____XXXX, ___X____,
  XXX_____, ___XX_X_, ____XXXX, X_XX____,
  XXX_____, ___X__XX, ____XXXX, XXXX____,
  XXX_____, ___XXXXX, ____XXXX, _XXX____,
  XXX_____, ___XXXXX, ____XXXX, XXXX____,
};

/**
 * $D95E: stage3_bitmap_D95E
 */
static const pixel_t stage3_bitmap_D95E[2 * 2 * 12 * 1] = {
  XXXXXXX_, _______X, ________, XXXXXXXX,
  XXXXXXX_, _______X, ________, _______X,
  XXXXXXX_, _______X, ________, XX__XX_X,
  XXXXXXX_, _______X, ________, __X___XX,
  XXXXXXX_, _______X, ________, XXX_XXXX,
  XXXXXXX_, _______X, ________, X___X__X,
  XXXXXXX_, _______X, ________, XXX_XXXX,
  XXXXXXX_, _______X, ________, _______X,
  XXXXXXX_, _______X, ________, X_X_X_XX,
  XXXXXXX_, _______X, ________, __XXXXXX,
  XXXXXXX_, _______X, ________, XXXX_XXX,
  XXXXXXX_, _______X, ________, XXXXXXXX,
};

/**
 * $D98E: stage3_bitmap_D98E
 */
static const pixel_t stage3_bitmap_D98E[2 * 2 * 9 * 1] = {
  XXXXX___, _____XXX, ____XXXX, XXXX____,
  XXXXX___, _____XXX, ____XXXX, _XXX____,
  XXXXX___, _____X_X, ____XXXX, __XX____,
  XXXXX___, _____XXX, ____XXXX, _XXX____,
  XXXXX___, _____XXX, ____XXXX, _XXX____,
  XXXXX___, _____X__, ____XXXX, XX_X____,
  XXXXX___, _____X_X, ____XXXX, XXXX____,
  XXXXX___, _____XX_, ____XXXX, X_XX____,
  XXXXX___, _____XXX, ____XXXX, XXXX____,
};

/**
 * $D9B2: stage3_bitmap_D9B2
 */
static const pixel_t stage3_bitmap_D9B2[2 * 2 * 9 * 1] = {
  XXXXXXXX, ________, X_______, _XXXXXXX,
  XXXXXXXX, ________, X_______, _XXX_XXX,
  XXXXXXXX, ________, X_______, _X_X__XX,
  XXXXXXXX, ________, X_______, _XXX_XXX,
  XXXXXXXX, ________, X_______, _XXX_XXX,
  XXXXXXXX, ________, X_______, _X__XX_X,
  XXXXXXXX, ________, X_______, _X_XXXXX,
  XXXXXXXX, ________, X_______, _XX_X_XX,
  XXXXXXXX, ________, X_______, _XXXXXXX,
};

/** $D9D6: stage3_stretchy_D9D6 */
static const stretchy_t stage3_stretchy_D9D6[4] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_right },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_right },
  { STRETCHY_TYPE_FIXED, &stage3_depthset_DA00 },
  { STRETCHY_TYPE_END, NULL },
};

/** $D9E0: stage3_stretchy_D9E0 */
static const stretchy_t stage3_stretchy_D9E0[4] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_left },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_left },
  { STRETCHY_TYPE_FIXED, &stage3_depthset_D9EA },
  { STRETCHY_TYPE_END, NULL },
};

/** $D9EA: stage3_depthset_D9EA */
static const depthset_t stage3_depthset_D9EA = {
  &stage3_lods_DA16[0],
  {
    0x20, 0x02,
    0x18, 0x02,
    0x10, 0x09,
    0x10, 0x09,
    0x0C, 0x10,
    0x0C, 0x10,
    0x10, 0x17,
    0x10, 0x17,
    0x0C, 0x1E,
    0x0C, 0x1E,
  }
};

/** $DA00: stage3_depthset_DA00 */
static const depthset_t stage3_depthset_DA00 = {
  &stage3_lods_DA16[0],
  {
    0x20, 0x02,
    0x18, 0x02,
    0x10, 0x09,
    0x10, 0x09,
    0x0C, 0x10,
    0x0C, 0x10,
    0x08, 0x17,
    0x08, 0x17,
    0x04, 0x1E,
    0x04, 0x1E,
  }
};

/** $DA16: stage3_lods_DA16 */
static const bitmap_t stage3_lods_DA16[5] = {
  { 3, BITMAPFLAG_DEFAULT, 13, &bitmap_telegraphpoletop_1[0], &bitmap_telegraphpoletop_1[0] },  // [0]
  { 3, BITMAPFLAG_DEFAULT, 10, &bitmap_telegraphpoletop_2[0], &bitmap_telegraphpoletop_2[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 7, &bitmap_telegraphpoletop_3[0], &bitmap_telegraphpoletop_3s[0] },  // [2]
  { 2, BITMAPFLAG_MASKED, 5, &bitmap_telegraphpoletop_4[0], &bitmap_telegraphpoletop_4s[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 4, &bitmap_telegraphpoletop_5[0], &bitmap_telegraphpoletop_5s[0] },  // [4]
};

/* $DA39: bitmap_telegraphpoletop_1 - Conv: Deduped to CommonData.c (shared with stage 1/5) */

/* $DA60: bitmap_telegraphpoletop_2 - Conv: Deduped to CommonData.c (shared with stage 1/5) */

/* $DA7E: bitmap_telegraphpoletop_3 - Conv: Deduped to CommonData.c (shared with stage 1/5) */

/* $DA93: bitmap_telegraphpoletop_3s - Conv: Deduped to CommonData.c (shared with stage 1/5) */

/* $DAA8: bitmap_telegraphpoletop_4 - Conv: Deduped to CommonData.c (shared with stage 1/5) */

/* $DABC: bitmap_telegraphpoletop_4s - Conv: Deduped to CommonData.c (shared with stage 1/5) */

/* $DAD0: bitmap_telegraphpoletop_5 - Conv: Deduped to CommonData.c (shared with stage 1/5) */

/* $DAE0: bitmap_telegraphpoletop_5s - Conv: Deduped to CommonData.c (shared with stage 1) */

const map_goto_entry_t stage3_map_goto_table[24] = {
  { STAGE3_MAP_CURV_C3F4_ADDR,    &stage3_map_curv_C3F4[0]    },
  { STAGE3_MAP_HEIGHT_C409_ADDR,  &stage3_map_height_C409[0]  },
  { STAGE3_MAP_LANES_C44B_ADDR,   &stage3_map_lanes_C44B[0]   },
  { STAGE3_MAP_HAZARDS_C455_ADDR, &stage3_map_hazards_C455[0] },
  { STAGE3_MAP_LOBJS_C46F_ADDR,   &stage3_map_lobjs_C46F[0]   },
  { STAGE3_MAP_ROBJS_C4A9_ADDR,   &stage3_map_robjs_C4A9[0]   },
  { STAGE3_MAP_CURV_C4D8_ADDR,    &stage3_map_curv_C4D8[0]    },
  { STAGE3_MAP_HEIGHT_C4EF_ADDR,  &stage3_map_height_C4EF[0]  },
  { STAGE3_MAP_LANES_C504_ADDR,   &stage3_map_lanes_C504[0]   },
  { STAGE3_MAP_HAZARDS_C510_ADDR, &stage3_map_hazards_C510[0] },
  { STAGE3_MAP_LOBJS_C518_ADDR,   &stage3_map_lobjs_C518[0]   },
  { STAGE3_MAP_ROBJS_C54B_ADDR,   &stage3_map_robjs_C54B[0]   },
  { STAGE3_MAP_CURV_C58A_ADDR,    &stage3_map_curv_C58A[0]    },
  { STAGE3_MAP_HEIGHT_C5A0_ADDR,  &stage3_map_height_C5A0[0]  },
  { STAGE3_MAP_LANES_C5B6_ADDR,   &stage3_map_lanes_C5B6[0]   },
  { STAGE3_MAP_HAZARDS_C5C4_ADDR, &stage3_map_hazards_C5C4[0] },
  { STAGE3_MAP_LOBJS_C5C9_ADDR,   &stage3_map_lobjs_C5C9[0]   },
  { STAGE3_MAP_ROBJS_C620_ADDR,   &stage3_map_robjs_C620[0]   },
  { STAGE3_MAP_CURV_C677_ADDR,    &stage3_map_curv_C677[0]    },
  { STAGE3_MAP_HEIGHT_C69D_ADDR,  &stage3_map_height_C69D[0]  },
  { STAGE3_MAP_LANES_C6DC_ADDR,   &stage3_map_lanes_C6DC[0]   },
  { STAGE3_MAP_HAZARDS_C6FC_ADDR, &stage3_map_hazards_C6FC[0] },
  { STAGE3_MAP_LOBJS_C71F_ADDR,   &stage3_map_lobjs_C71F[0]   },
  { STAGE3_MAP_ROBJS_C7AD_ADDR,   &stage3_map_robjs_C7AD[0]   },
};

// clang-format on
