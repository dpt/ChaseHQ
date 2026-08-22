/**
 * Stage5Data.c
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

#include "Stage5Data.h"

/* Z80 addresses of the map sections, as referenced by the map
 * GOTO/SPLIT commands and stage5_map_goto_table[]. */
#define STAGE5_MAP_CURV_C455_ADDR    (0x6055)
#define STAGE5_MAP_HEIGHT_C468_ADDR  (0x6068)
#define STAGE5_MAP_LANES_C47A_ADDR   (0x607A)
#define STAGE5_MAP_HAZARDS_C492_ADDR (0x6092)
#define STAGE5_MAP_LOBJS_C49A_ADDR   (0x609A)
#define STAGE5_MAP_ROBJS_C4DD_ADDR   (0x60DD)
#define STAGE5_MAP_CURV_C513_ADDR    (0x6113)
#define STAGE5_MAP_HEIGHT_C526_ADDR  (0x6126)
#define STAGE5_MAP_LANES_C53E_ADDR   (0x613E)
#define STAGE5_MAP_HAZARDS_C552_ADDR (0x6152)
#define STAGE5_MAP_LOBJS_C560_ADDR   (0x6160)
#define STAGE5_MAP_ROBJS_C5A3_ADDR   (0x61A3)
#define STAGE5_MAP_CURV_C5E3_ADDR    (0x61E3)
#define STAGE5_MAP_HEIGHT_C5F8_ADDR  (0x61F8)
#define STAGE5_MAP_LANES_C614_ADDR   (0x6214)
#define STAGE5_MAP_HAZARDS_C628_ADDR (0x6228)
#define STAGE5_MAP_LOBJS_C62D_ADDR   (0x622D)
#define STAGE5_MAP_ROBJS_C651_ADDR   (0x6251)
#define STAGE5_MAP_CURV_C65C_ADDR    (0x625C)
#define STAGE5_MAP_HEIGHT_C684_ADDR  (0x6284)
#define STAGE5_MAP_LANES_C6BB_ADDR   (0x62BB)
#define STAGE5_MAP_HAZARDS_C6D1_ADDR (0x62D1)
#define STAGE5_MAP_LOBJS_C6EE_ADDR   (0x62EE)
#define STAGE5_MAP_ROBJS_C7A5_ADDR   (0x63A5)

/* ----------------------------------------------------------------------- */

/* Stage 5 object type macros */

#define MAP_OBJ_S5_OVERHEAD_BRIDGE_VAL    (2)
#define MAP_OBJ_S5_CACTUS_VAL             (4)
#define MAP_OBJ_S5_DOUBLE_STREET_LAMP_VAL (5)
#define MAP_OBJ_S5_HUGE_ROCK_VAL          (6)
#define MAP_OBJ_S5_TELEGRAPH_POLE_VAL     (7)

#define MAP_OBJ_S5_OVERHEAD_BRIDGE(D)     (((D) << 4) | MAP_OBJ_S5_OVERHEAD_BRIDGE_VAL)
#define MAP_OBJ_S5_CACTUS(D)              (((D) << 4) | MAP_OBJ_S5_CACTUS_VAL)
#define MAP_OBJ_S5_DOUBLE_STREET_LAMP(D)  (((D) << 4) | MAP_OBJ_S5_DOUBLE_STREET_LAMP_VAL)
#define MAP_OBJ_S5_HUGE_ROCK(D)           (((D) << 4) | MAP_OBJ_S5_HUGE_ROCK_VAL)
#define MAP_OBJ_S5_TELEGRAPH_POLE(D)      (((D) << 4) | MAP_OBJ_S5_TELEGRAPH_POLE_VAL)

/* ----------------------------------------------------------------------- */

/* Forward declarations */

static const char *stage5_chatter_strings[4];
static const u8 stage5_arrest_messages_C1E6[83];
static const hittable_t stage5_hittable_objects_C239[2];
static const obj_t stage5_right_obj_defs_C23F[7];
static const obj_t stage5_left_obj_defs_C270[7];
static const u8 stage5_map_curv_C2A1[40];
static const u8 stage5_map_height_C2C9[50];
static const u8 stage5_map_lanes_C2FB[18];
static const u8 stage5_map_hazards_C30D[31];
static const u8 stage5_map_lobjs_C32C[166];
static const u8 stage5_map_robjs_C3D2[131];
static const u8 stage5_map_curv_C455[19];
static const u8 stage5_map_height_C468[18];
static const u8 stage5_map_lanes_C47A[24];
static const u8 stage5_map_hazards_C492[8];
static const u8 stage5_map_lobjs_C49A[67];
static const u8 stage5_map_robjs_C4DD[54];
static const u8 stage5_map_curv_C513[19];
static const u8 stage5_map_height_C526[24];
static const u8 stage5_map_lanes_C53E[20];
static const u8 stage5_map_hazards_C552[14];
static const u8 stage5_map_lobjs_C560[67];
static const u8 stage5_map_robjs_C5A3[64];
static const u8 stage5_map_curv_C5E3[21];
static const u8 stage5_map_height_C5F8[28];
static const u8 stage5_map_lanes_C614[20];
static const u8 stage5_map_hazards_C628[5];
static const u8 stage5_map_lobjs_C62D[36];
static const u8 stage5_map_robjs_C651[11];
static const u8 stage5_map_curv_C65C[40];
static const u8 stage5_map_height_C684[55];
static const u8 stage5_map_lanes_C6BB[22];
static const u8 stage5_map_hazards_C6D1[29];
static const u8 stage5_map_lobjs_C6EE[183];
static const u8 stage5_map_robjs_C7A5[137];
static const pixel_t stage5_perp_face[180];
static const bitmap_t stage5_lambo_bitmaps[6];
static const bitmap_t stage5_pickup_bitmaps[6];
static const bitmap_t stage5_f40_bitmaps[6];
static const stretchy_t stage5_stretchy_CE91[5];
static const depthset_t stage5_depthset_CE9E;
static const depthset_t stage5_depthset_CEB4;
static const depthset_t stage5_depthset_CECA;
static const depthset_t stage5_depthset_CEE0;
static const stretchy_t stage5_stretchy_CEF6[5];
static const depthset_t stage5_depthset_CF03;
static const depthset_t stage5_depthset_CF19;
static const depthset_t stage5_depthset_CF2F;
static const depthset_t stage5_depthset_CF45;
static const bitmap_t stage5_lods_CF5B[20];
static const bitmap_t stage5_lods_CFE7[20];
static const stretchy_t stage5_stretchy_D28C[3];
static const depthset_t stage5_depthset_D293;
static const depthset_t stage5_depthset_D2A9;
static const bitmap_t stage5_lods_D2BF[8];
static const stretchy_t stage5_stretchy_D2F7[3];
static const depthset_t stage5_depthset_D2FE;
static const overhead_span_t stage5_overhead_spans_D314[10];
static const depthset_t stage5_depthset_D332;
static const bitmap_t stage5_lods_D348[8];
static const bitmap_t stage5_lods_D620[6];
static const stretchy_t stage5_stretchy_D6FF[5];
static const stretchy_t stage5_stretchy_D70C[5];
static const depthset_t stage5_depthset_D719;
static const depthset_t stage5_depthset_D72F;
static const depthset_t stage5_depthset_D745;
static const depthset_t stage5_depthset_D75B;
static const depthset_t stage5_depthset_D771;
static const depthset_t stage5_depthset_D787;
static const depthset_t stage5_depthset_D79D;
static const bitmap_t stage5_lods_D7B3[15];
static const stretchy_t stage5_stretchy_D8D2[5];
static const stretchy_t stage5_stretchy_D8DF[5];
static const depthset_t stage5_depthset_D8EC;
static const depthset_t stage5_depthset_D902;
static const bitmap_t stage5_lods_D918[5];
static const stretchy_t stage5_stretchy_D9CF[4];
static const stretchy_t stage5_stretchy_D9D9[4];
static const depthset_t stage5_depthset_D9E3;
static const depthset_t stage5_depthset_D9F9;
static const bitmap_t stage5_lods_DA0F[5];
static const pixel_t stage5_bitmap_D81C[3 * 1 * 4 * 1];
static const pixel_t stage5_bitmap_D828[3 * 1 * 13 * 1];
static const pixel_t stage5_bitmap_D84F[1 * 1 * 7 * 1];
static const pixel_t stage5_bitmap_D856[2 * 1 * 3 * 1];
static const pixel_t stage5_bitmap_D85C[1 * 1 * 2 * 1];
static const pixel_t stage5_bitmap_D85E[1 * 1 * 6 * 1];
static const pixel_t stage5_bitmap_D864[1 * 1 * 2 * 1];
static const pixel_t stage5_bitmap_D866[1 * 2 + 4];
static const pixel_t stage5_bitmap_D86C[6];
static const pixel_t stage5_bitmap_D872[2 * 2 * 3 * 1];
static const pixel_t stage5_bitmap_D87E[2 * 2 * 9 * 1];
static const pixel_t stage5_bitmap_D8A2[2 * 2 * 2 * 1];
static const pixel_t stage5_bitmap_D8AA[1 * 2 * 2 * 1];
static const pixel_t stage5_bitmap_D8AE[1 * 2 * 6 * 1];
static const pixel_t stage5_bitmap_D8BA[2 * 2 * 2 * 1];
static const pixel_t stage5_bitmap_D8C6[1 * 2 * 6 * 1];
static const pixel_t stage5_bitmap_D9B7[2 * 2 * 2 + 4];
static const pixel_t stage5_bitmap_D9C3[2 * 2 * 2 + 4];
static const pixel_t stage5_bitmap_DAD9[2 * 2 * 4 + 7];

/* ----------------------------------------------------------------------- */

/** $C000: stage5 */
// clang-format off
const stage_t stage5 = {
  /* $C000 backdrop */
  {
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    ________, ________, ____X_X_, X_XX_XXX, ___XXX_X, _X______, X_X_X_X_, X_XXXXXX, XXXXXXXX, XXXXXXXX,
    ________, ________, ________, _X_XXXX_, X_XXX_X_, X_______, _X_X_XXX, XXXXXXXX, XXXXXXXX, XXXXX_X_,
    X_______, ______X_, X_X__X_X, _XX_XXXX, __XX_X__, ______X_, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X,
    _XXX____, ________, ________, _X_XX_X_, X_XXX___, _____X_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_X_X_,
    _X_XXX__, ________, ________, X_XXXX__, __X__X_X, X_XXXXX_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_XXX,
    X_X_XXX_, ________, ________, _XX_XXX_, X_X_____, _____XXX, XXX__XXX, XXXXXXXX, XXXXXXXX, XXXXX_X_,
    _XXX_XXX, ________, __X_X_X_, X_XX_X__, ________, __X_X_XX, XX_____X, XXXXXXXX, XXXXXXXX, XX_X_X_X,
    X_X_X__X, XX______, _______X, _XXXX___, ___X_X_X, XXXXXXXX, XX_X___X, XXXXXXXX, XXXXX___, ________,
    ___X_X__, _XXX____, ________, XX_XX_XX, ________, X_X_XXXX, X_______, XXX___XX, ________, ________,
    _______X, __XXX___, _______X, X_XXXX_X, _X_____X, XX_XXXXX, X____X__, X_______, _X__X___, _X______,
    ____X___, ___XXX__, __X_X_X_, XXXX____, ______X_, XXXXXXXX, X__X____, _____X__, ______X_, ___X__X_,
    ______X_, _X__XXXX, ____XXXX, XX______, ___XXX_X, XXXXXXXX, X_______, __X_____, ___X____, ________,
    ________, _____X__, _______X, XXXXX_X_, __XX_XX_, X_XXXXXX, X_______, X_______, ________, ____X_X_,
    X_______, __X_XXX_, X_XX_XXX, XX_X____, ______XX, XXXXXXXX, ________, ________, _____X__, _X______,
    ________, _____XXX, __X_X_X_, XXX_____, _______X, _X_XXXXX, ________, ______X_, ________, ______X_,
    ________, _X____X_, _______X, _XX_X___, ________, __XXXXXX, ________, _______X, ________, X__X____,
    ________, ___X_XXX, ________, XXXX_X__, ________, ___XXXX_, ________, ________, ________, ________,
    ________, ______XX, X______X, _XXXXX__, _____XX_, __XXXX_X, ________, _______X, ________, __X_X___,
    ________, ___X_X_X, XX______, XXX__XXX, _X_XXXXX, _XXX____, _X______, ________, X_______, ________,
    _X______, ______X_, XXXX___X, XX_____X, X_XX___X, X_______, ___X____, __X_X___, ________, X____XXX,
    ___X____, _____X_X, ___XXXXX, ________, ________, ________, ____XX__, _X___X__, ________, ___XX___,
    _____X_X, ____XXX_, ________, ________, ________, ________, _______X, X______X, _X______, _XX_____,
    ________, XXXXX___, ________, ________, ________, ________, ________, ________, __XXXXXX, ________,
  },
  /* $C0F0 perstage */
  &stage5_perp_face[FACEBITMAPBYTES],
  NULL,  /* no pilot mugshot on this stage */
  attribute_BRIGHT_BLACK_OVER_YELLOW * 0x0101,
  &stage5_hittable_objects_C239[0],  /* addrof_hittable_objects */
  &stage5_right_obj_defs_C23F[-1].arg,  /* addrof_right_hand_handlers */
  &stage5_right_obj_defs_C23F[-1],  /* addrof_right_hand_objects */
  &stage5_right_obj_defs_C23F[2],  /* addrof_right_hand_short_pole_object */
  &stage5_left_obj_defs_C270[-1].arg,  /* addrof_left_hand_handlers */
  &stage5_left_obj_defs_C270[-1],  /* addrof_left_hand_objects */
  &stage5_left_obj_defs_C270[2],  /* addrof_left_hand_short_pole_object */
  &perp_description[0],  /* addrof_perp_description */
  &stage5_arrest_messages_C1E6[0],  /* addrof_arrest_messages */
  { NULL, NULL },

  NULL,  /* bitmaps_stones */
  NULL,  /* bitmaps_dust */
  &stage5_lambo_bitmaps[0],  /* bitmaps_perp_car */
  {
    &stage5_f40_bitmaps[0],
    &stage5_pickup_bitmaps[0],
    &stage5_f40_bitmaps[0],
    &stage5_lambo_bitmaps[0]
  },  /* bitmaps_vehicles */

  /* $C11A difficulty */
  20,  /* car_spawn_delay */
  40,  /* perp_lane_change_base */
  40,  /* perp_approach_base */

  /* $C11D setupdata */
  {
    158,
    &stage5_map_curv_C2A1[-1],
    &stage5_map_height_C2C9[-1],
    &stage5_map_lanes_C2FB[-1],
    &stage5_map_robjs_C3D2[-1],
    &stage5_map_lobjs_C32C[-1],
    &stage5_map_hazards_C30D[-1],
  },

  /* $C12B attractdata */
  {
    234,
    &stage5_map_curv_C65C[-1],
    &stage5_map_height_C684[-1],
    &stage5_map_lanes_C6BB[-1],
    &stage5_map_robjs_C7A5[-1],
    &stage5_map_lobjs_C6EE[-1],
    &stage5_map_hazards_C6D1[-1],
  },

  stage5_chatter_strings
};

/* ----------------------------------------------------------------------- */

/* $C139: perp_description - Conv: Deduped to CommonData.c (shared with stage 1/2/3/4) */

/** $C145 (bank 7) */
static const char *stage5_chatter_strings[4] = {
  "THIS IS NANCY AT CHASE H.Q. WE'VE GOT A\xCE",
  "EMERGENCY HERE. THE EASTERN BLOC SPY FRO\xCD",
  "WASHINGTON IS FLEEING TOWARDS THE SUBURBS\xAE",
  "THE TARGET VEHICLE IS UNKNOWN... OVER\xAE",
};

/* ----------------------------------------------------------------------- */

/**
 * $C1E6: stage5_arrest_messages_C1E6
 */
static const u8 stage5_arrest_messages_C1E6[83] = {
  6,  // initial delay

  6,  // delay
  DRAWCHARSTYLE_SINGLE,
  0,  // attribute
  CHQBACKBUF(0xF802),
  ZXATTRS(0x5922),
  'O', 'K', '!', ' ', 'Y', 'O', 'U', ' ', 'A', 'R', 'E', ' ', 'U', 'N', 'D', 'E', 'R', ' ', 'A', 'R', 'R', 'E', 'S', 'T', ' ', 'O', 'N' | EOS,

  6,  // delay
  DRAWCHARSTYLE_SINGLE,
  0,  // attribute
  CHQBACKBUF(0xF023),
  ZXATTRS(0x5943),
  'S', 'U', 'S', 'P', 'I', 'C', 'I', 'O', 'N', ' ', 'O', 'F', ' ', 'E', 'S', 'P', 'I', 'O', 'N', 'A', 'G', 'E', ' ', 'A', 'N', 'D' | EOS,

  45,  // delay
  DRAWCHARSTYLE_SINGLE,
  0,  // attribute
  CHQBACKBUF(0xF82C),
  ZXATTRS(0x596C),
  'M', 'U', 'R', 'D', 'E', 'R' | EOS,

  TRANSITIONCONTROL_FILL_ATTRIBUTES,
  DRAWOVERLAY_STOP
};

/* ----------------------------------------------------------------------- */

/** $C239: stage5_hittable_objects_C239 */
static const hittable_t stage5_hittable_objects_C239[2] = {
  { 32, &stage5_lods_D620[0] },
  { 32, &stage5_lods_D620[0] },
};

/** $C23F: stage5_right_obj_defs_C23F */
static const obj_t stage5_right_obj_defs_C23F[7] = {
  { 111, 41, 80, &tunnellight, draw_tunnel_light_right },
  { 111, 53, 60, &stage5_stretchy_D28C[0], draw_stretchy_object_right },
  { 144, 92, 40, &stretchy_shortpole, draw_stretchy_object_right },
  { 116, 56, 80, &stage5_stretchy_D6FF[0], draw_stretchy_object_right },
  { 110, 53, 70, &stage5_stretchy_D8D2[0], draw_stretchy_object_right },
  { 127, 16, 50, &stage5_stretchy_CE91[0], draw_stretchy_object_right },
  { 110, 53, 70, &stage5_stretchy_D9CF[0], draw_stretchy_object_right },
};

/** $C270: stage5_left_obj_defs_C270 */
static const obj_t stage5_left_obj_defs_C270[7] = {
  { 126, 188, 80, &tunnellight, draw_tunnel_light_left },
  { 126, 154, 60, &stage5_stretchy_D2F7[0], draw_overhead },
  { 96, 144, 40, &stretchy_shortpole, draw_stretchy_object_left },
  { 126, 188, 80, &stage5_stretchy_D70C[0], draw_stretchy_object_left },
  { 132, 182, 70, &stage5_stretchy_D8DF[0], draw_stretchy_object_left },
  { 112, 224, 50, &stage5_stretchy_CEF6[0], draw_stretchy_object_left },
  { 132, 182, 70, &stage5_stretchy_D9D9[0], draw_stretchy_object_left },
};

/* ----------------------------------------------------------------------- */

/** $C2A1: stage5_map_curv_C2A1 */
static const u8 stage5_map_curv_C2A1[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(6),
  MAP_CURVE_LEFT(14),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(6),
  MAP_CURVE_RIGHT_VERY_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(10),
  MAP_CURVE_RIGHT(10),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(4),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(1),
  MAP_CURVE_LEFT_VERY_HARD(15),
  MAP_CURVE_LEFT_VERY_HARD(5),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(3),
  MAP_CURVE_LEFT(7),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(9),
  MAP_CMD_SPLIT(STAGE5_MAP_CURV_C513_ADDR, STAGE5_MAP_CURV_C455_ADDR)
};

/** $C2C9: stage5_map_height_C2C9 */
static const u8 stage5_map_height_C2C9[] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(10),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_UP7(1),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(5),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN5(3),
  MAP_HEIGHT_DOWN3(4),
  MAP_HEIGHT_DOWN7(1),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(4),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(3),
  MAP_HEIGHT_DOWN7(3),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(1),
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
  MAP_HEIGHT_LEVEL(3),
  MAP_CMD_SPLIT(STAGE5_MAP_HEIGHT_C526_ADDR, STAGE5_MAP_HEIGHT_C468_ADDR)
};

/** $C2FB: stage5_map_lanes_C2FB */
static const u8 stage5_map_lanes_C2FB[] = {
  MAP_LANES_2R(36),
  MAP_LANES_2RTO3R(2),
  MAP_LANES_3R(52),
  MAP_LANES_3RTO4(2),
  MAP_LANES_4(254),
  MAP_LANES_4(54),
  MAP_CMD_SPLIT(STAGE5_MAP_LANES_C53E_ADDR, STAGE5_MAP_LANES_C47A_ADDR)
};

/** $C30D: stage5_map_hazards_C30D */
static const u8 stage5_map_hazards_C30D[] = {
  MAP_HAZARD_WAIT(20),
  MAP_CMD_START_BARRIERS_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(45),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(43),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(4),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(76),
  MAP_CMD_STOP_CARS,
  MAP_HAZARD_WAIT(6),
  MAP_CMD_ARROW_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_SPLIT(STAGE5_MAP_HAZARDS_C552_ADDR, STAGE5_MAP_HAZARDS_C492_ADDR)
};

/** $C32C: stage5_map_lobjs_C32C */
static const u8 stage5_map_lobjs_C32C[] = {
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_CMD_SPLIT(STAGE5_MAP_LOBJS_C560_ADDR, STAGE5_MAP_LOBJS_C49A_ADDR)
};

/** $C3D2: stage5_map_robjs_C3D2 */
static const u8 stage5_map_robjs_C3D2[] = {
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(4),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(13),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(11),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_CMD_SPLIT(STAGE5_MAP_ROBJS_C5A3_ADDR, STAGE5_MAP_ROBJS_C4DD_ADDR)
};

/** $C455: stage5_map_curv_C455 */
static const u8 stage5_map_curv_C455[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(10),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(11),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(10),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(2),
  MAP_CMD_GOTO(STAGE5_MAP_CURV_C5E3_ADDR)
};

/** $C468: stage5_map_height_C468 */
static const u8 stage5_map_height_C468[] = {
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
  MAP_HEIGHT_LEVEL(3),
  MAP_CMD_GOTO(STAGE5_MAP_HEIGHT_C5F8_ADDR)
};

/** $C47A: stage5_map_lanes_C47A */
static const u8 stage5_map_lanes_C47A[] = {
  MAP_LANES_4TO3L(2),
  MAP_LANES_TUNNEL_ENTRY(20),
  MAP_LANES_TUNNEL_EXIT(2),
  MAP_LANES_3L(4),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(6),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(74),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(84),
  MAP_CMD_GOTO(STAGE5_MAP_LANES_C614_ADDR)
};

/** $C492: stage5_map_hazards_C492 */
static const u8 stage5_map_hazards_C492[] = {
  MAP_HAZARD_WAIT(10),
  MAP_CMD_START_CARS,
  MAP_HAZARD_WAIT(89),
  MAP_CMD_GOTO(STAGE5_MAP_HAZARDS_C628_ADDR)
};

/** $C49A: stage5_map_lobjs_C49A */
static const u8 stage5_map_lobjs_C49A[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_TUNNEL_LIGHT(10),
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(2),
  MAP_CMD_GOTO(STAGE5_MAP_LOBJS_C62D_ADDR)
};

/** $C4DD: stage5_map_robjs_C4DD */
static const u8 stage5_map_robjs_C4DD[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_TUNNEL_LIGHT(10),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(11),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(2),
  MAP_CMD_GOTO(STAGE5_MAP_ROBJS_C651_ADDR)
};

/** $C513: stage5_map_curv_C513 */
static const u8 stage5_map_curv_C513[] = {
  MAP_CURVE_STRAIGHT(12),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(13),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(10),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(8),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CMD_GOTO(STAGE5_MAP_CURV_C5E3_ADDR)
};

/** $C526: stage5_map_height_C526 */
static const u8 stage5_map_height_C526[] = {
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
  MAP_HEIGHT_LEVEL(3),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP5(3),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN5(3),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(5),
  MAP_CMD_GOTO(STAGE5_MAP_HEIGHT_C5F8_ADDR)
};

/** $C53E: stage5_map_lanes_C53E */
static const u8 stage5_map_lanes_C53E[] = {
  MAP_LANES_4(40),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(124),
  MAP_LANES_TUNNEL_ENTRY(20),
  MAP_LANES_TUNNEL_EXIT(2),
  MAP_LANES_3L(2),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(6),
  MAP_CMD_GOTO(STAGE5_MAP_LANES_C614_ADDR)
};

/** $C552: stage5_map_hazards_C552 */
static const u8 stage5_map_hazards_C552[] = {
  MAP_HAZARD_WAIT(10),
  MAP_CMD_START_CARS,
  MAP_HAZARD_WAIT(8),
  MAP_CMD_START_BARRIERS_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(79),
  MAP_CMD_GOTO(STAGE5_MAP_HAZARDS_C628_ADDR)
};

/** $C560: stage5_map_lobjs_C560 */
static const u8 stage5_map_lobjs_C560[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(6),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_TUNNEL_LIGHT(10),
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_CMD_GOTO(STAGE5_MAP_LOBJS_C62D_ADDR)
};

/** $C5A3: stage5_map_robjs_C5A3 */
static const u8 stage5_map_robjs_C5A3[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(6),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_TUNNEL_LIGHT(10),
  MAP_OBJ_NONE(5),
  MAP_CMD_GOTO(STAGE5_MAP_ROBJS_C651_ADDR)
};

/** $C5E3: stage5_map_curv_C5E3 */
static const u8 stage5_map_curv_C5E3[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CURVE_LEFT_HARD(12),
  MAP_CURVE_RIGHT(1),
  MAP_CURVE_RIGHT_HARD(10),
  MAP_CURVE_RIGHT(6),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(4),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(11),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(2),
  MAP_CMD_GOTO(STAGE5_MAP_CURV_C65C_ADDR)
};

/** $C5F8: stage5_map_height_C5F8 */
static const u8 stage5_map_height_C5F8[] = {
  MAP_HEIGHT_DOWN5(15),
  MAP_HEIGHT_DOWN3(3),
  MAP_HEIGHT_LEVEL(4),
  MAP_HEIGHT_UP7(15),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(13),
  MAP_HEIGHT_UP3(15),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_UP5(6),
  MAP_HEIGHT_UP7(8),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN1(2),
  MAP_HEIGHT_DOWN3(4),
  MAP_HEIGHT_DOWN5(4),
  MAP_HEIGHT_DOWN7(11),
  MAP_HEIGHT_DOWN5(10),
  MAP_HEIGHT_DOWN3(11),
  MAP_HEIGHT_DOWN1(2),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(7),
  MAP_CMD_GOTO(STAGE5_MAP_HEIGHT_C684_ADDR)
};

/** $C614: stage5_map_lanes_C614 */
static const u8 stage5_map_lanes_C614[] = {
  MAP_LANES_4(44),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(12),
  MAP_LANES_TUNNEL_ENTRY(102),
  MAP_LANES_TUNNEL_EXIT(2),
  MAP_LANES_3L(18),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(4),
  MAP_CMD_GOTO(STAGE5_MAP_LANES_C6BB_ADDR)
};

/** $C628: stage5_map_hazards_C628 */
static const u8 stage5_map_hazards_C628[] = {
  MAP_HAZARD_WAIT(93),
  MAP_CMD_GOTO(STAGE5_MAP_HAZARDS_C6D1_ADDR)
};

/** $C62D: stage5_map_lobjs_C62D */
static const u8 stage5_map_lobjs_C62D[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(6),
  MAP_OBJ_NONE(12),
  MAP_CMD_GOTO(STAGE5_MAP_LOBJS_C6EE_ADDR)
};

/** $C651: stage5_map_robjs_C651 */
static const u8 stage5_map_robjs_C651[] = {
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(3),
  MAP_CMD_GOTO(STAGE5_MAP_ROBJS_C7A5_ADDR)
};

/** $C65C: stage5_map_curv_C65C */
static const u8 stage5_map_curv_C65C[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(10),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(11),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(4),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(11),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(9),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(6),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(10),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(9),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(9),
  MAP_CURVE_RIGHT_VERY_HARD(15),
  MAP_CURVE_RIGHT_VERY_HARD(2),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(2),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(1),
  MAP_CMD_GOTO(STAGE5_MAP_CURV_C65C_ADDR)
};

/** $C684: stage5_map_height_C684 */
static const u8 stage5_map_height_C684[] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(12),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_DOWN5(4),
  MAP_HEIGHT_DOWN7(2),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_LEVEL(2),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_UP7(8),
  MAP_HEIGHT_DOWN7(4),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(6),
  MAP_HEIGHT_DOWN7(4),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN1(3),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(6),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(1),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN7(1),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_LEVEL(6),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(2),
  MAP_HEIGHT_DOWN3(8),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(13),
  MAP_CMD_GOTO(STAGE5_MAP_HEIGHT_C684_ADDR)
};

/** $C6BB: stage5_map_lanes_C6BB */
static const u8 stage5_map_lanes_C6BB[] = {
  MAP_LANES_4(254),
  MAP_LANES_4(114),
  MAP_LANES_4TO3R(36),
  MAP_LANES_3RTO4(2),
  MAP_LANES_4(4),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(8),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(22),
  MAP_CMD_GOTO(STAGE5_MAP_LANES_C6BB_ADDR)
};

/** $C6D1: stage5_map_hazards_C6D1 */
static const u8 stage5_map_hazards_C6D1[] = {
  MAP_HAZARD_WAIT(13),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(4),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(40),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(91),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(28),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(40),
  MAP_CMD_GOTO(STAGE5_MAP_HAZARDS_C6D1_ADDR)
};

/** $C6EE: stage5_map_lobjs_C6EE */
static const u8 stage5_map_lobjs_C6EE[] = {
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_CMD_GOTO(STAGE5_MAP_LOBJS_C6EE_ADDR)
};

/** $C7A5: stage5_map_robjs_C7A5 */
static const u8 stage5_map_robjs_C7A5[] = {
  MAP_OBJ_NONE(6),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(12),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_CMD_GOTO(STAGE5_MAP_ROBJS_C7A5_ADDR)
};

/**
 * $C82E: stage5_perp_face
 */
static const pixel_t stage5_perp_face[FACEBYTES] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, X_XXXXX_, XXX_XXX_, X_X_X__X,
  XXXXXXXX, X_XX_XX_, XX__XX_X, XXXX_X_X,
  XXXXXXXX, _XXXXX_X, XX_X_XXX, _XX__X_X,
  XXXXXXXX, XXX__XX_, _XXXX_X_, _XXXX__X,
  XXXXXXXX, XX______, _______X, XXX_XX_X,
  XXXXXXXX, X_X_____, ________, XXXXXX_X,
  XXXXXXXX, XX______, _______X, _XXXXX_X,
  XXXXXXXX, X_______, ________, X_XXX__X,
  XXXXXXXX, XX______, ________, _XXXX__X,
  XXXXXXXX, X_X_____, ________, X_XXX__X,
  XXXXXXXX, XX______, __XXXX__, _XXXX__X,
  XXXXXXXX, XXXX____, XXXXXXX_, _XXX___X,
  XXXXXXXX, XXXX____, XXX____X, _XXX___X,
  XXXXXXX_, ___XX___, XXXXXX__, _XXXX__X,
  XXXXXX__, XXXXXX__, XX_XX_X_, __XX_X_X,
  XXXXXXXX, XX_XXX__, _X______, ___X_X_X,
  XXXXXX_X, __XXX___, _XXX____, __XX_X_X,
  XXXXXX__, XX_XXX__, _XX_____, ___X_X_X,
  X_XXX___, __XXX___, ___X____, __XX_X_X,
  X_XXXX_X, _XXX____, ___XX___, ___XX__X,
  X_XXXXX_, X_XXXX__, _XXXXX__, ___XX__X,
  XXXXXXXX, XXXXXXXX, XX__XX__, ___X___X,
  X_XXXXXX, XXXXXXX_, _____XX_, ___X___X,
  X_XXXXXX, XXXXXX_X, _____XX_, X__X___X,
  X__XXXXX, XXXXXXXX, XXX___X_, X_X____X,
  X__XXXXX, XXXXXX__, ___XX_X_, X_X____X,
  X__XXXXX, XXXXX___, ______X_, __X____X,
  X__XXXXX, XXXXXXXX, XXX_____, __X____X,
  X___XXXX, XXXX_XXX, X_XX____, __X____X,
  X___XXXX, XXX_X___, ________, __X____X,
  X____XXX, XXXX_X__, ________, __XX___X,
  X____XXX, XXXXX___, _____X__, _XXXX__X,
  X___XXXX, XXXXXX_X, _X_XX___, _X_XXX_X,
  X__XXXXX, XXXXXXXX, XXX_____, X_XXXX_X,
  X_XXXXX_, XXXXXXX_, X_____XX, X_XXXXXX,
  X_XX_XXX, XXXXXX_X, _X_XXX_X, X_X_XXXX,
  XXXXXXXX, _XXXXXXX, XXX____X, _XXXXXXX,
  XXXXXX_X, X_XXX___, ______X_, XXXXXX_X,
  XXXX_XXX, XXXXXXXX, XXXXXXXX, XX_XXXXX,

  attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_CYAN,
  attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_WHITE,
  attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_WHITE,
  attribute_BRIGHT_BLACK_OVER_CYAN, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_CYAN,
  attribute_BRIGHT_BLACK_OVER_CYAN, attribute_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_WHITE,
};

/* ----------------------------------------------------------------------- */

/** $C8E2: stage5_lambo_bitmaps */
static const bitmap_t stage5_lambo_bitmaps[6] = {
#ifdef CHQ_ENABLE_MASKED_VEHICLES
  { 6, BITMAPFLAG_MASKED, 30, &bitmap_lambo_1_masked[0], &bitmap_lambo_1_masked[0] },  // [0]
  { 5, BITMAPFLAG_MASKED, 22, &bitmap_lambo_2_masked[0], &bitmap_lambo_2_masked[0] },  // [1]
  { 3, BITMAPFLAG_MASKED, 15, &bitmap_lambo_3_masked[0], &bitmap_lambo_3_masked[0] },  // [2]
  { 3, BITMAPFLAG_MASKED, 15, &bitmap_lambo_3_masked[0], &bitmap_lambo_3_masked[0] },  // [3]
#else
  { 6, BITMAPFLAG_DEFAULT, 30, &bitmap_lambo_1[0], &bitmap_lambo_1[0] },  // [0]
  { 5, BITMAPFLAG_DEFAULT, 22, &bitmap_lambo_2[0], &bitmap_lambo_2[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 15, &bitmap_lambo_3[0], &bitmap_lambo_3[0] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 15, &bitmap_lambo_3[0], &bitmap_lambo_3[0] },  // [3]
#endif
  { 3, BITMAPFLAG_MASKED, 8, &bitmap_lambo_4[0], &bitmap_lambo_4[0] },  // [4]
  { 3, BITMAPFLAG_MASKED, 8, &bitmap_lambo_4[0], &bitmap_lambo_4s[0] },  // [5]
};

/** $C90C: stage5_pickup_bitmaps */
static const bitmap_t stage5_pickup_bitmaps[6] = {
#ifdef CHQ_ENABLE_MASKED_VEHICLES
  { 6, BITMAPFLAG_MASKED, 39, &bitmap_truck_1_masked[0], &bitmap_truck_1_masked[0] },  // [0]
  { 5, BITMAPFLAG_MASKED, 29, &bitmap_truck_2_masked[0], &bitmap_truck_2_masked[0] },  // [1]
  { 3, BITMAPFLAG_MASKED, 20, &bitmap_truck_3_masked[0], &bitmap_truck_3_masked[0] },  // [2]
  { 3, BITMAPFLAG_MASKED, 20, &bitmap_truck_3_masked[0], &bitmap_truck_3_masked[0] },  // [3]
#else
  { 6, BITMAPFLAG_DEFAULT, 39, &bitmap_truck_1[0], &bitmap_truck_1[0] },  // [0]
  { 5, BITMAPFLAG_DEFAULT, 29, &bitmap_truck_2[0], &bitmap_truck_2[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 20, &bitmap_truck_3[0], &bitmap_truck_3[0] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 20, &bitmap_truck_3[0], &bitmap_truck_3[0] },  // [3]
#endif
  { 2, BITMAPFLAG_MASKED, 12, &bitmap_truck_4[0], &bitmap_truck_4s[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 12, &bitmap_truck_4[0], &bitmap_truck_4s[0] },  // [5]
};

/** $C936: stage5_f40_bitmaps */
static const bitmap_t stage5_f40_bitmaps[6] = {
#ifdef CHQ_ENABLE_MASKED_VEHICLES
  { 6, BITMAPFLAG_MASKED, 30, &bitmap_C95E_masked[0], &bitmap_C95E_masked[0] },  // [0]
  { 4, BITMAPFLAG_MASKED, 20, &bitmap_CA12_masked[0], &bitmap_CA12_masked[0] },  // [1]
  { 3, BITMAPFLAG_MASKED, 13, &bitmap_CA62_masked[0], &bitmap_CA62_masked[0] },  // [2]
  { 3, BITMAPFLAG_MASKED, 13, &bitmap_CA62_masked[0], &bitmap_CA62_masked[0] },  // [3]
#else
  { 6, BITMAPFLAG_DEFAULT, 30, &bitmap_C95E[0], &bitmap_C95E[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 20, &bitmap_CA12[0], &bitmap_CA12[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 13, &bitmap_CA62[0], &bitmap_CA62[0] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 13, &bitmap_CA62[0], &bitmap_CA62[0] },  // [3]
#endif
  { 2, BITMAPFLAG_MASKED, 8, &bitmap_CA89[0], &bitmap_CA89[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 8, &bitmap_CA89[0], &bitmap_CAA9[0] },  // [5]
};

/* $C960: bitmap_lambo_1 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CA14: bitmap_lambo_2 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CA82: bitmap_lambo_3 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CAAF: bitmap_lambo_4 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CADF: bitmap_lambo_4s - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CB0F: bitmap_truck_1 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CBF9: bitmap_truck_2 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CC8A: bitmap_truck_3 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CCC6: bitmap_truck_4 - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CCF6: bitmap_truck_4s - Conv: Deduped to CommonData.c (shared with stage 1) */

/* $CD26: bitmap_C95E - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $CDDA: bitmap_CA12 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $CE2A: bitmap_CA62 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $CE51: bitmap_CA89 - Conv: Deduped to CommonData.c (shared with stage 3/3) */

/* $CE71: bitmap_CAA9 - Conv: Deduped to CommonData.c (shared with stage 3/3) */

/** $CE91: stage5_stretchy_CE91 */
static const stretchy_t stage5_stretchy_CE91[5] = {
  { STRETCHY_TYPE_FIXED, &stage5_depthset_CE9E },
  { STRETCHY_TYPE_113PC, &stage5_depthset_CEB4 },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_CECA },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_CEE0 },
  { STRETCHY_TYPE_END, NULL },
};

/** $CE9E: stage5_depthset_CE9E */
static const depthset_t stage5_depthset_CE9E = {
  &stage5_lods_CF5B[0],
  {
    0x00, 0x17,
    0x00, 0x17,
    0x00, 0x33,
    0x00, 0x33,
    0x00, 0x4F,
    0x00, 0x4F,
    0xF8, 0x6B,
    0xF8, 0x6B,
    0xF8, 0x87,
    0xF8, 0x87,
  }
};

/** $CEB4: stage5_depthset_CEB4 */
static const depthset_t stage5_depthset_CEB4 = {
  &stage5_lods_CF5B[0],
  {
    0x10, 0x10,
    0x10, 0x10,
    0x10, 0x2C,
    0x10, 0x2C,
    0x10, 0x48,
    0x10, 0x48,
    0x08, 0x64,
    0x08, 0x64,
    0x08, 0x80,
    0x08, 0x80,
  }
};

/** $CECA: stage5_depthset_CECA */
static const depthset_t stage5_depthset_CECA = {
  &stage5_lods_CF5B[0],
  {
    0x08, 0x09,
    0x08, 0x09,
    0x10, 0x25,
    0x10, 0x25,
    0x08, 0x41,
    0x08, 0x41,
    0x00, 0x5D,
    0x00, 0x5D,
    0x00, 0x79,
    0x00, 0x79,
  }
};

/** $CEE0: stage5_depthset_CEE0 */
static const depthset_t stage5_depthset_CEE0 = {
  &stage5_lods_CF5B[0],
  {
    0x18, 0x02,
    0x18, 0x02,
    0x18, 0x1E,
    0x18, 0x1E,
    0x10, 0x3A,
    0x10, 0x3A,
    0x08, 0x56,
    0x08, 0x56,
    0x08, 0x72,
    0x08, 0x72,
  }
};

/** $CEF6: stage5_stretchy_CEF6 */
static const stretchy_t stage5_stretchy_CEF6[5] = {
  { STRETCHY_TYPE_FIXED, &stage5_depthset_CF03 },
  { STRETCHY_TYPE_113PC, &stage5_depthset_CF19 },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_CF2F },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_CF45 },
  { STRETCHY_TYPE_END, NULL },
};

/** $CF03: stage5_depthset_CF03 */
static const depthset_t stage5_depthset_CF03 = {
  &stage5_lods_CFE7[0],
  {
    0x08, 0x17,
    0x08, 0x17,
    0x08, 0x33,
    0x08, 0x33,
    0x00, 0x4F,
    0x00, 0x4F,
    0x00, 0x6B,
    0x00, 0x6B,
    0x00, 0x87,
    0x00, 0x87,
  }
};

/** $CF19: stage5_depthset_CF19 */
static const depthset_t stage5_depthset_CF19 = {
  &stage5_lods_CFE7[0],
  {
    0x10, 0x10,
    0x10, 0x10,
    0x10, 0x2C,
    0x10, 0x2C,
    0x08, 0x48,
    0x08, 0x48,
    0x08, 0x64,
    0x08, 0x64,
    0x08, 0x80,
    0x08, 0x80,
  }
};

/** $CF2F: stage5_depthset_CF2F */
static const depthset_t stage5_depthset_CF2F = {
  &stage5_lods_CFE7[0],
  {
    0x10, 0x09,
    0x10, 0x09,
    0x18, 0x25,
    0x18, 0x25,
    0x08, 0x41,
    0x08, 0x41,
    0x08, 0x5D,
    0x08, 0x5D,
    0x08, 0x79,
    0x08, 0x79,
  }
};

/** $CF45: stage5_depthset_CF45 */
static const depthset_t stage5_depthset_CF45 = {
  &stage5_lods_CFE7[0],
  {
    0x20, 0x02,
    0x20, 0x02,
    0x20, 0x1E,
    0x20, 0x1E,
    0x10, 0x3A,
    0x10, 0x3A,
    0x10, 0x56,
    0x10, 0x56,
    0x10, 0x72,
    0x10, 0x72,
  }
};

/** $CF5B: stage5_lods_CF5B */
static const bitmap_t stage5_lods_CF5B[20] = {
  { 5, BITMAPFLAG_FLIPPED, 8, &bitmap_F2F0[0], &bitmap_F2F0[0] },  // [0]
  { 7, BITMAPFLAG_FLIPPED, 8, &bitmap_F318[0], &bitmap_F318[0] },  // [1]
  { 7, BITMAPFLAG_DEFAULT, 8, &bitmap_F46D[0], &bitmap_F46D[0] },  // [2]
  { 8, BITMAPFLAG_FLIPPED, 8, &bitmap_F388[0], &bitmap_F388[0] },  // [3]
  { 3, BITMAPFLAG_FLIPPED, 4, &bitmap_F3C8[0], &bitmap_F3C8[0] },  // [4]
  { 4, BITMAPFLAG_FLIPPED, 4, &bitmap_F3D4[0], &bitmap_F3D4[0] },  // [5]
  { 5, BITMAPFLAG_DEFAULT, 6, &bitmap_F4A5[0], &bitmap_F4A5[0] },  // [6]
  { 6, BITMAPFLAG_FLIPPED, 6, &bitmap_F407[0], &bitmap_F407[0] },  // [7]
  { 2, BITMAPFLAG_FLIPPED, 4, &bitmap_F42B[0], &bitmap_F42B[0] },  // [8]
  { 3, BITMAPFLAG_FLIPPED, 3, &bitmap_F433[0], &bitmap_F433[0] },  // [9]
  { 3, BITMAPFLAG_DEFAULT, 4, &bitmap_F4C3[0], &bitmap_F4C3[0] },  // [10]
  { 4, BITMAPFLAG_FLIPPED, 4, &bitmap_F448[0], &bitmap_F448[0] },  // [11]
  { 1, BITMAPFLAG_FLIPPED, 3, &bitmap_F458[0], &bitmap_F458[0] },  // [12]
  { 2, BITMAPFLAG_FLIPPED, 3, &bitmap_F45B[0], &bitmap_F45B[0] },  // [13]
  { 2, BITMAPFLAG_DEFAULT, 3, &bitmap_F4CF[0], &bitmap_F4CF[0] },  // [14]
  { 3, BITMAPFLAG_FLIPPED, 2, &bitmap_F467[0], &bitmap_F467[0] },  // [15]
  { 1, BITMAPFLAG_MASKED|BITMAPFLAG_FLIPPED, 1, &bitmap_F4D5[0], &bitmap_F4EF[0] },  // [16]
  { 2, BITMAPFLAG_MASKED|BITMAPFLAG_FLIPPED, 1, &bitmap_F4D7[0], &bitmap_F4F1[0] },  // [17]
  { 2, BITMAPFLAG_MASKED, 2, &bitmap_F4DB[0], &bitmap_F4F5[0] },  // [18]
  { 3, BITMAPFLAG_MASKED|BITMAPFLAG_FLIPPED, 2, &bitmap_F4E3[0], &bitmap_F4FD[0] },  // [19]
};

/** $CFE7: stage5_lods_CFE7 */
static const bitmap_t stage5_lods_CFE7[20] = {
  { 5, BITMAPFLAG_DEFAULT, 8, &bitmap_F2F0[0], &bitmap_F2F0[0] },  // [0]
  { 7, BITMAPFLAG_DEFAULT, 8, &bitmap_F318[0], &bitmap_F318[0] },  // [1]
  { 7, BITMAPFLAG_DEFAULT, 8, &bitmap_F350[0], &bitmap_F350[0] },  // [2]
  { 8, BITMAPFLAG_DEFAULT, 8, &bitmap_F388[0], &bitmap_F388[0] },  // [3]
  { 3, BITMAPFLAG_DEFAULT, 4, &bitmap_F3C8[0], &bitmap_F3C8[0] },  // [4]
  { 4, BITMAPFLAG_DEFAULT, 4, &bitmap_F3D4[0], &bitmap_F3D4[0] },  // [5]
  { 5, BITMAPFLAG_DEFAULT, 6, &bitmap_F3E9[0], &bitmap_F3E9[0] },  // [6]
  { 6, BITMAPFLAG_DEFAULT, 6, &bitmap_F407[0], &bitmap_F407[0] },  // [7]
  { 2, BITMAPFLAG_DEFAULT, 4, &bitmap_F42B[0], &bitmap_F42B[0] },  // [8]
  { 3, BITMAPFLAG_DEFAULT, 3, &bitmap_F433[0], &bitmap_F433[0] },  // [9]
  { 3, BITMAPFLAG_DEFAULT, 4, &bitmap_F43C[0], &bitmap_F43C[0] },  // [10]
  { 4, BITMAPFLAG_DEFAULT, 4, &bitmap_F448[0], &bitmap_F448[0] },  // [11]
  { 1, BITMAPFLAG_DEFAULT, 3, &bitmap_F458[0], &bitmap_F458[0] },  // [12]
  { 2, BITMAPFLAG_DEFAULT, 3, &bitmap_F45B[0], &bitmap_F45B[0] },  // [13]
  { 2, BITMAPFLAG_DEFAULT, 3, &bitmap_F461[0], &bitmap_F461[0] },  // [14]
  { 3, BITMAPFLAG_DEFAULT, 2, &bitmap_F467[0], &bitmap_F467[0] },  // [15]
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_F4D5[0], &bitmap_F4EF[0] },  // [16]
  { 2, BITMAPFLAG_MASKED, 1, &bitmap_F4D7[0], &bitmap_F4F1[0] },  // [17]
  { 2, BITMAPFLAG_MASKED, 2, &bitmap_F4DB[0], &bitmap_F4F5[0] },  // [18]
  { 3, BITMAPFLAG_MASKED, 2, &bitmap_F4E3[0], &bitmap_F4FD[0] },  // [19]
};

/* $D073: bitmap_F2F0 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D09B: bitmap_F318 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D0D3: bitmap_F350 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D10B: bitmap_F388 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D14B: bitmap_F3C8 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D157: bitmap_F3D4 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D16C: bitmap_F3E9 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D18A: bitmap_F407 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D1AE: bitmap_F42B - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D1B6: bitmap_F433 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D1BF: bitmap_F43C - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D1CB: bitmap_F448 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D1DB: bitmap_F458 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D1DE: bitmap_F45B - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D1E4: bitmap_F461 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D1EA: bitmap_F467 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D1F0: bitmap_F46D - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D228: bitmap_F4A5 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D246: bitmap_F4C3 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D252: bitmap_F4CF - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D258: bitmap_F4D5 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D25A: bitmap_F4D7 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D25E: bitmap_F4DB - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D266: bitmap_F4E3 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D272: bitmap_F4EF - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D274: bitmap_F4F1 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D278: bitmap_F4F5 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D280: bitmap_F4FD - Conv: Deduped to CommonData.c (shared with stage 2) */

/** $D28C: stage5_stretchy_D28C */
static const stretchy_t stage5_stretchy_D28C[3] = {
  { STRETCHY_TYPE_150PC, &stage5_depthset_D293 },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D2A9 },
  { STRETCHY_TYPE_END, NULL },
};

/** $D293: stage5_depthset_D293 */
static const depthset_t stage5_depthset_D293 = {
  &stage5_lods_D2BF[0],
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

/** $D2A9: stage5_depthset_D2A9 */
static const depthset_t stage5_depthset_D2A9 = {
  &stage5_lods_D2BF[0],
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

/** $D2BF: stage5_lods_D2BF */
static const bitmap_t stage5_lods_D2BF[8] = {
  { 4, BITMAPFLAG_DEFAULT, 16, &bitmap_D467[0], &bitmap_D467[0] },  // [0]
  { 3, BITMAPFLAG_DEFAULT, 12, &bitmap_D576[0], &bitmap_D576[0] },  // [1]
  { 2, BITMAPFLAG_DEFAULT, 8, &bitmap_D604[0], &bitmap_D604[0] },  // [2]
  { 1, BITMAPFLAG_DEFAULT, 4, &bitmap_D63D[0], &bitmap_D63D[0] },  // [3]
  { 4, BITMAPFLAG_DEFAULT, 24, &bitmap_D4A7[0], &bitmap_D4A7[0] },  // [4]
  { 3, BITMAPFLAG_DEFAULT, 18, &bitmap_D59A[0], &bitmap_D59A[0] },  // [5]
  { 2, BITMAPFLAG_DEFAULT, 12, &bitmap_D614[0], &bitmap_D614[0] },  // [6]
  { 1, BITMAPFLAG_DEFAULT, 6, &bitmap_D641[0], &bitmap_D641[0] },  // [7]
};

/** $D2F7: stage5_stretchy_D2F7 */
static const stretchy_t stage5_stretchy_D2F7[3] = {
  { STRETCHY_TYPE_150PC, &stage5_depthset_D2FE },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D332 },
  { STRETCHY_TYPE_END, NULL },
};

/** $D2FE: stage5_depthset_D2FE */
static const depthset_t stage5_depthset_D2FE = {
  &stage5_lods_D348[0],
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
  &stage5_overhead_spans_D314[0]
};

/**
 * $D314: overhead-bridge span table (Conv: modelled explicitly; see
 * overhead_span_t in Stages.h). Each entry's fill_bytes points into
 * bitmap_D3AF, which is defined further below.
 */
static const overhead_span_t stage5_overhead_spans_D314[10] = {
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

/** $D332: stage5_depthset_D332 */
static const depthset_t stage5_depthset_D332 = {
  &stage5_lods_D348[0],
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

/** $D348: stage5_lods_D348 */
static const bitmap_t stage5_lods_D348[8] = {
  { 4, BITMAPFLAG_DEFAULT, 16, &bitmap_D3AF[0], &bitmap_D3AF[0] },  // [0]
  { 3, BITMAPFLAG_DEFAULT, 12, &bitmap_D507[0], &bitmap_D507[0] },  // [1]
  { 2, BITMAPFLAG_DEFAULT, 8, &bitmap_D5D0[0], &bitmap_D5D0[0] },  // [2]
  { 1, BITMAPFLAG_DEFAULT, 4, &bitmap_D62C[0], &bitmap_D62C[0] },  // [3]
  { 4, BITMAPFLAG_DEFAULT, 24, &bitmap_D3EF[0], &bitmap_D3EF[0] },  // [4]
  { 3, BITMAPFLAG_DEFAULT, 18, &bitmap_D52B[0], &bitmap_D52B[0] },  // [5]
  { 2, BITMAPFLAG_DEFAULT, 12, &bitmap_D5E0[0], &bitmap_D5E0[0] },  // [6]
  { 1, BITMAPFLAG_DEFAULT, 6, &bitmap_D630[0], &bitmap_D630[0] },  // [7]
};

/* $D380: bitmap_D3AF - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D3C0: bitmap_D3EF - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D420: bitmap_D44F - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D438: bitmap_D467 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D478: bitmap_D4A7 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D4D8: bitmap_D507 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D4FC: bitmap_D52B - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D535: bitmap_D564 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D547: bitmap_D576 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D56B: bitmap_D59A - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D5A1: bitmap_D5D0 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D5B1: bitmap_D5E0 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D5C9: bitmap_D5F8 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D5D5: bitmap_D604 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D5E5: bitmap_D614 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D5FD: bitmap_D62C - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D601: bitmap_D630 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D607: bitmap_D636 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D60E: bitmap_D63D - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $D612: bitmap_D641 - Conv: Deduped to CommonData.c (shared with stage 3) */

/** $D620: stage5_lods_D620 */
static const bitmap_t stage5_lods_D620[6] = {
  { 4, BITMAPFLAG_DEFAULT, 17, &bitmap_barrier_1[0], &bitmap_barrier_1[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 17, &bitmap_barrier_1[0], &bitmap_barrier_1[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 13, &bitmap_barrier_2[0], &bitmap_barrier_2[0] },  // [2]
  { 2, BITMAPFLAG_DEFAULT, 9, &bitmap_barrier_3[0], &bitmap_barrier_3[0] },  // [3]
  { 2, BITMAPFLAG_DEFAULT, 9, &bitmap_barrier_3[0], &bitmap_barrier_3[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 7, &bitmap_barrier_4[0], &bitmap_barrier_4s[0] },  // [5]
};

/* $D64A: bitmap_barrier_1 - Conv: Deduped to CommonData.c (shared with stage 1/2/3/4) */

/* $D68E: bitmap_barrier_2 - Conv: Deduped to CommonData.c (shared with stage 1/2/3/4) */

/* $D6B5: bitmap_barrier_3 - Conv: Deduped to CommonData.c (shared with stage 1/2/3/4) */

/* $D6C7: bitmap_barrier_4 - Conv: Deduped to CommonData.c (shared with stage 1/2/3/4) */

/* $D6E3: bitmap_barrier_4s - Conv: Deduped to CommonData.c (shared with stage 1/2/3/4) */

/** $D6FF: stage5_stretchy_D6FF */
static const stretchy_t stage5_stretchy_D6FF[5] = {
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D719 },
  { STRETCHY_TYPE_25PC, &stage5_depthset_D72F },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D75B },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D787 },
  { STRETCHY_TYPE_END, NULL },
};

/** $D70C: stage5_stretchy_D70C */
static const stretchy_t stage5_stretchy_D70C[5] = {
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D719 },
  { STRETCHY_TYPE_25PC, &stage5_depthset_D745 },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D771 },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D79D },
  { STRETCHY_TYPE_END, NULL },
};

/** $D719: stage5_depthset_D719 */
static const depthset_t stage5_depthset_D719 = {
  &stage5_lods_D7B3[0],
  {
    0x1C, 0x02,
    0x14, 0x02,
    0x10, 0x17,
    0x10, 0x17,
    0x0C, 0x2C,
    0x0C, 0x2C,
    0x0C, 0x56,
    0x08, 0x56,
    0x08, 0x56,
    0x08, 0x56,
  }
};

/** $D72F: stage5_depthset_D72F */
static const depthset_t stage5_depthset_D72F = {
  &stage5_lods_D7B3[0],
  {
    0x1C, 0x48,
    0x14, 0x48,
    0x10, 0x1E,
    0x10, 0x1E,
    0x0C, 0x33,
    0x0C, 0x33,
    0x0C, 0x5D,
    0x08, 0x5D,
    0x08, 0x5D,
    0x08, 0x5D,
  }
};

/** $D745: stage5_depthset_D745 */
static const depthset_t stage5_depthset_D745 = {
  &stage5_lods_D7B3[0],
  {
    0x24, 0x48,
    0x1C, 0x48,
    0x18, 0x1E,
    0x18, 0x1E,
    0x0C, 0x33,
    0x0C, 0x33,
    0x14, 0x5D,
    0x10, 0x5D,
    0x10, 0x5D,
    0x10, 0x5D,
  }
};

/** $D75B: stage5_depthset_D75B */
static const depthset_t stage5_depthset_D75B = {
  &stage5_lods_D7B3[0],
  {
    0x1C, 0x09,
    0x14, 0x09,
    0x10, 0x4F,
    0x10, 0x4F,
    0x0C, 0x3A,
    0x0C, 0x3A,
    0x0C, 0x5D,
    0x08, 0x5D,
    0x08, 0x5D,
    0x08, 0x5D,
  }
};

/** $D771: stage5_depthset_D771 */
static const depthset_t stage5_depthset_D771 = {
  &stage5_lods_D7B3[0],
  {
    0x1C, 0x09,
    0x14, 0x09,
    0x10, 0x4F,
    0x10, 0x4F,
    0x0C, 0x3A,
    0x0C, 0x3A,
    0x14, 0x5D,
    0x10, 0x5D,
    0x10, 0x5D,
    0x10, 0x5D,
  }
};

/** $D787: stage5_depthset_D787 */
static const depthset_t stage5_depthset_D787 = {
  &stage5_lods_D7B3[0],
  {
    0x24, 0x10,
    0x1C, 0x10,
    0x10, 0x25,
    0x10, 0x25,
    0x0C, 0x41,
    0x0C, 0x41,
    0x0C, 0x64,
    0x08, 0x64,
    0x08, 0x64,
    0x08, 0x64,
  }
};

/** $D79D: stage5_depthset_D79D */
static const depthset_t stage5_depthset_D79D = {
  &stage5_lods_D7B3[0],
  {
    0x24, 0x10,
    0x1C, 0x10,
    0x18, 0x25,
    0x18, 0x25,
    0x0C, 0x41,
    0x0C, 0x41,
    0x14, 0x64,
    0x10, 0x64,
    0x10, 0x64,
    0x10, 0x64,
  }
};

/** $D7B3: stage5_lods_D7B3 */
static const bitmap_t stage5_lods_D7B3[15] = {
  { 3, BITMAPFLAG_DEFAULT, 4, &stage5_bitmap_D81C[0], &stage5_bitmap_D81C[0] },  // [0]
  { 3, BITMAPFLAG_DEFAULT, 13, &stage5_bitmap_D828[0], &stage5_bitmap_D828[0] },  // [1]
  { 1, BITMAPFLAG_DEFAULT, 7, &stage5_bitmap_D84F[0], &stage5_bitmap_D84F[0] },  // [2]
  { 2, BITMAPFLAG_DEFAULT, 3, &stage5_bitmap_D856[0], &stage5_bitmap_D856[0] },  // [3]
  { 1, BITMAPFLAG_DEFAULT, 2, &stage5_bitmap_D85C[0], &stage5_bitmap_D85C[0] },  // [4]
  { 1, BITMAPFLAG_DEFAULT, 6, &stage5_bitmap_D85E[0], &stage5_bitmap_D85E[0] },  // [5]
  { 1, BITMAPFLAG_DEFAULT, 2, &stage5_bitmap_D864[0], &stage5_bitmap_D864[0] },  // [6]
  { 1, BITMAPFLAG_DEFAULT, 2, &stage5_bitmap_D866[0], &stage5_bitmap_D866[0] },  // [7]
  { 1, BITMAPFLAG_DEFAULT, 5, &stage5_bitmap_D86C[0], &stage5_bitmap_D86C[0] },  // [8]
  { 1, BITMAPFLAG_DEFAULT, 2, &stage5_bitmap_D86C[4], &stage5_bitmap_D86C[4] },  // [9]
  { 2, BITMAPFLAG_MASKED, 3, &stage5_bitmap_D872[0], &stage5_bitmap_D872[0] },  // [10]
  { 2, BITMAPFLAG_MASKED, 9, &stage5_bitmap_D87E[0], &stage5_bitmap_D87E[0] },  // [11]
  { 2, BITMAPFLAG_MASKED, 2, &stage5_bitmap_D8A2[0], &stage5_bitmap_D8BA[0] },  // [12]
  { 1, BITMAPFLAG_MASKED, 2, &stage5_bitmap_D8AA[0], &bitmap_F8A2[0] },  // [13]
  { 1, BITMAPFLAG_MASKED, 6, &stage5_bitmap_D8AE[0], &stage5_bitmap_D8C6[0] },  // [14]
};

/**
 * $D81C: stage5_bitmap_D81C
 */
static const pixel_t stage5_bitmap_D81C[3 * 1 * 4 * 1] = {
  ________, XXXXXX__, XX______,
  ____XXXX, XXXXXXXX, XXXXXX__,
  __XXXX_X, _XXXXXXX, XXXX____,
  ____XX__, __X_XXXX, X_______,
};

/**
 * $D828: stage5_bitmap_D828
 */
static const pixel_t stage5_bitmap_D828[3 * 1 * 13 * 1] = {
  __X_X_XX, _X_XX_XX, XXX_____,
  ___X_XXX, ___X_X_X, _XXX____,
  ___X__XX, __XX____, X_XX____,
  ___X_XXX, ___XX_X_, __XXX___,
  __X___XX, X__XXXX_, X__XX___,
  __XX_XXX, __XXXXXX, ___XX___,
  _XX___XX, X__XX_X_, X_X_XX__,
  __XX_XXX, ___XXX__, X__XX___,
  _XX__XXX, _X_X_X_X, X_XXXX__,
  __X__XXX, ___XXX__, X__XX___,
  __XX_XX_, __XXXXXX, X_XXX___,
  _X_X_XXX, ___XXXX_, _XXX____,
  __X_XXX_, X_X_X_XX, __X_____,
};

/**
 * $D84F: stage5_bitmap_D84F
 */
static const pixel_t stage5_bitmap_D84F[1 * 1 * 7 * 1] = {
  X___XXX_,
  X___XXXX,
  XX_X_XX_,
  X___XXX_,
  X___XX__,
  XX_XXX__,
  _XXXX_X_,
};

/**
 * $D856: stage5_bitmap_D856
 */
static const pixel_t stage5_bitmap_D856[2 * 1 * 3 * 1] = {
  _XXXXXXX, _XX_____,
  XXXX_XXX, XXXXXX__,
  _XX____X, XX______,
};

/**
 * $D85C: stage5_bitmap_D85C
 */
static const pixel_t stage5_bitmap_D85C[1 * 1 * 2 * 1] = {
  __X__XXX,
  _XX_X_XX,
};

/**
 * $D85E: stage5_bitmap_D85E
 */
static const pixel_t stage5_bitmap_D85E[1 * 1 * 6 * 1] = {
  XXX__XXX,
  _XX_X_XX,
  __X__XXX,
  _X_X__X_,
  ___X_XX_,
  __X_XX__,
};

/**
 * $D864: stage5_bitmap_D864
 */
static const pixel_t stage5_bitmap_D864[1 * 1 * 2 * 1] = {
  _XXXX_XX,
  XXXXXXX_,
};

/**
 * $D866: stage5_bitmap_D866
 */
static const pixel_t stage5_bitmap_D866[1 * 2 + 4] = {
  __X_XX__,
  __XX_X__,
  // $D868: 4 further bytes, not reached by any LOD entry
  _XX__XX_, _XX_X_XX, _XX_X__X, _XX_XX_X,
};

/**
 * $D86C: stage5_bitmap_D86C
 */
static const pixel_t stage5_bitmap_D86C[6] = {
  _XX_XX_X, _XX_XXX_, _XX_XX__, __X_XX__, __XX_X__, ___XX___,
};

/**
 * $D872: stage5_bitmap_D872
 */
static const pixel_t stage5_bitmap_D872[2 * 2 * 3 * 1] = {
  XXXXX___, _____XX_, ______X_, _X__XX_X,
  XXXX_X__, ____X_X_, _______X, ___XXXX_,
  XXXXX___, _____XXX, ______XX, __X_XX__,
};

/**
 * $D87E: stage5_bitmap_D87E
 */
static const pixel_t stage5_bitmap_D87E[2 * 2 * 9 * 1] = {
  X_______, _XX_XXXX, __XXXXXX, XX______,
  ________, XXX__XX_, ___XXXXX, X_X_____,
  ________, XXX_XX__, ____XXXX, _XXX____,
  ________, XXX__XX_, ____XXXX, __XX____,
  ________, XXX___XX, _____XXX, X__XX___,
  ________, XXX___XX, ____XXXX, X_XX____,
  ________, X_X_X_XX, _____XXX, ___XX___,
  ________, X_X__XXX, ____XXXX, __XX____,
  ________, X_X_X_XX, X__XXXXX, _XX_____,
};

/**
 * $D8A2: stage5_bitmap_D8A2
 */
static const pixel_t stage5_bitmap_D8A2[2 * 2 * 2 * 1] = {
  ______X_, XXXXXX_X, XXXXXXXX, ________,
  ____XXXX, XXXX____, XXXXXXXX, ________,
};

/**
 * $D8AA: stage5_bitmap_D8AA
 */
static const pixel_t stage5_bitmap_D8AA[1 * 2 * 2 * 1] = {
  X__XXXXX, _XX_____,
  X__XXXXX, _XX_____,
};

/**
 * $D8AE: stage5_bitmap_D8AE
 */
static const pixel_t stage5_bitmap_D8AE[1 * 2 * 6 * 1] = {
  ___XXXXX, XXX_____,
  ____XXXX, XXXX____,
  ____XXXX, XXXX____,
  ___XXXXX, XXX_____,
  X__XXXXX, _XX_____,
  X__XXXXX, _XX_____,
};

/**
 * $D8BA: stage5_bitmap_D8BA
 */
static const pixel_t stage5_bitmap_D8BA[2 * 2 * 2 * 1] = {
  XXXX____, ____XXXX, __X_XXXX, XX_X____,
  XXXX____, ____XXXX, XXXXXXXX, ________,
};

/* $D8C2: bitmap_F8A2 - Conv: Deduped to CommonData.c (shared with stage 2) */

/**
 * $D8C6: stage5_bitmap_D8C6
 */
static const pixel_t stage5_bitmap_D8C6[1 * 2 * 6 * 1] = {
  XXXX___X, ____XXX_,
  XXXX____, ____XXXX,
  XXXX____, ____XXXX,
  XXXX___X, ____XXX_,
  XXXXX__X, _____XX_,
  XXXXX__X, _____XX_,
};

/** $D8D2: stage5_stretchy_D8D2 */
static const stretchy_t stage5_stretchy_D8D2[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_right },
  { STRETCHY_TYPE_50PC, &streetlampmiddle2_right },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_right },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D8EC },
  { STRETCHY_TYPE_END, NULL },
};

/** $D8DF: stage5_stretchy_D8DF */
static const stretchy_t stage5_stretchy_D8DF[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_left },
  { STRETCHY_TYPE_50PC, &streetlampmiddle2_left },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_left },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D902 },
  { STRETCHY_TYPE_END, NULL },
};

/** $D8EC: stage5_depthset_D8EC */
static const depthset_t stage5_depthset_D8EC = {
  &stage5_lods_D918[0],
  {
    0x18, 0x02,
    0x10, 0x02,
    0x08, 0x09,
    0x08, 0x09,
    0x0C, 0x10,
    0x0C, 0x10,
    0x08, 0x17,
    0x08, 0x17,
    0x04, 0x1E,
    0x04, 0x1E,
  }
};

/** $D902: stage5_depthset_D902 */
static const depthset_t stage5_depthset_D902 = {
  &stage5_lods_D918[0],
  {
    0x18, 0x02,
    0x10, 0x02,
    0x10, 0x09,
    0x10, 0x09,
    0x0C, 0x10,
    0x0C, 0x10,
    0x08, 0x17,
    0x08, 0x17,
    0x0C, 0x1E,
    0x0C, 0x1E,
  }
};

/** $D918: stage5_lods_D918 */
static const bitmap_t stage5_lods_D918[5] = {
  { 5, BITMAPFLAG_DEFAULT, 8, &bitmap_F97B[0], &bitmap_F97B[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 6, &bitmap_F9A3[0], &bitmap_F9A3[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 4, &bitmap_F9BB[0], &bitmap_F9C7[0] },  // [2]
  { 3, BITMAPFLAG_MASKED, 3, &bitmap_F9D3[0], &bitmap_F9E5[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 2, &stage5_bitmap_D9B7[0], &stage5_bitmap_D9C3[0] },  // [4]
};

/* $D93B: bitmap_F97B - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D963: bitmap_F9A3 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D97B: bitmap_F9BB - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D987: bitmap_F9C7 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D993: bitmap_F9D3 - Conv: Deduped to CommonData.c (shared with stage 2) */

/* $D9A5: bitmap_F9E5 - Conv: Deduped to CommonData.c (shared with stage 2) */

/**
 * $D9B7: stage5_bitmap_D9B7
 */
static const pixel_t stage5_bitmap_D9B7[2 * 2 * 2 + 4] = {
  X_XXXX_X, _X____X_, XXX_XXXX, ___X____,
  ______X_, X_XXXX_X, _____XXX, XXX_X___,
  // $D9BF: 4 further bytes, not reached by any LOD entry
  ___XXXXX, XXX_____, XX___XXX, __XXX___,
};

/**
 * $D9C3: stage5_bitmap_D9C3
 */
static const pixel_t stage5_bitmap_D9C3[2 * 2 * 2 + 4] = {
  XXXXX_XX, _____X__, XX_XXX_X, __X___X_,
  XXXX____, ____X_XX, __X_____, XX_XXX_X,
  // $D9CB: 4 further bytes, not reached by any LOD entry
  XXXX___X, ____XXX_, XXXXXX__, ______XX,
};

/** $D9CF: stage5_stretchy_D9CF */
static const stretchy_t stage5_stretchy_D9CF[4] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_right },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_right },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D9F9 },
  { STRETCHY_TYPE_END, NULL },
};

/** $D9D9: stage5_stretchy_D9D9 */
static const stretchy_t stage5_stretchy_D9D9[4] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_left },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_left },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D9E3 },
  { STRETCHY_TYPE_END, NULL },
};

/** $D9E3: stage5_depthset_D9E3 */
static const depthset_t stage5_depthset_D9E3 = {
  &stage5_lods_DA0F[0],
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

/** $D9F9: stage5_depthset_D9F9 */
static const depthset_t stage5_depthset_D9F9 = {
  &stage5_lods_DA0F[0],
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

/** $DA0F: stage5_lods_DA0F */
static const bitmap_t stage5_lods_DA0F[5] = {
  { 3, BITMAPFLAG_DEFAULT, 13, &bitmap_telegraphpoletop_1[0], &bitmap_telegraphpoletop_1[0] },  // [0]
  { 3, BITMAPFLAG_DEFAULT, 10, &bitmap_telegraphpoletop_2[0], &bitmap_telegraphpoletop_2[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 7, &bitmap_telegraphpoletop_3[0], &bitmap_telegraphpoletop_3s[0] },  // [2]
  { 2, BITMAPFLAG_MASKED, 5, &bitmap_telegraphpoletop_4[0], &bitmap_telegraphpoletop_4s[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 4, &bitmap_telegraphpoletop_5[0], &stage5_bitmap_DAD9[0] },  // [4]
};

/* $DA32: bitmap_telegraphpoletop_1 - Conv: Deduped to CommonData.c (shared with stage 1/3) */

/* $DA59: bitmap_telegraphpoletop_2 - Conv: Deduped to CommonData.c (shared with stage 1/3) */

/* $DA77: bitmap_telegraphpoletop_3 - Conv: Deduped to CommonData.c (shared with stage 1/3) */

/* $DA8C: bitmap_telegraphpoletop_3s - Conv: Deduped to CommonData.c (shared with stage 1/3) */

/* $DAA1: bitmap_telegraphpoletop_4 - Conv: Deduped to CommonData.c (shared with stage 1/3) */

/* $DAB5: bitmap_telegraphpoletop_4s - Conv: Deduped to CommonData.c (shared with stage 1/3) */

/* $DAC9: bitmap_telegraphpoletop_5 - Conv: Deduped to CommonData.c (shared with stage 1/3) */

/**
 * $DAD9: stage5_bitmap_DAD9
 */
static const pixel_t stage5_bitmap_DAD9[2 * 2 * 4 + 7] = {
  XXXXXXXX, ________, XX_____X, __XXXXX_,
  XXXXXXXX, ________, XX_____X, __XX__X_,
  XXXXXXXX, ________, X_______, _XXX_XXX,
  XXXXXXXX, ________, X_______, _X_X__XX,
  // $DAE9: 7 further bytes, not reached by any LOD entry
  ________, X_______, _XXX_XXX, XXXXXXXX, ________, X_______, _X_X__XX,
};

const map_goto_entry_t stage5_map_goto_table[24] = {
  { STAGE5_MAP_CURV_C455_ADDR,    &stage5_map_curv_C455[0]    },
  { STAGE5_MAP_HEIGHT_C468_ADDR,  &stage5_map_height_C468[0]  },
  { STAGE5_MAP_LANES_C47A_ADDR,   &stage5_map_lanes_C47A[0]   },
  { STAGE5_MAP_HAZARDS_C492_ADDR, &stage5_map_hazards_C492[0] },
  { STAGE5_MAP_LOBJS_C49A_ADDR,   &stage5_map_lobjs_C49A[0]   },
  { STAGE5_MAP_ROBJS_C4DD_ADDR,   &stage5_map_robjs_C4DD[0]   },
  { STAGE5_MAP_CURV_C513_ADDR,    &stage5_map_curv_C513[0]    },
  { STAGE5_MAP_HEIGHT_C526_ADDR,  &stage5_map_height_C526[0]  },
  { STAGE5_MAP_LANES_C53E_ADDR,   &stage5_map_lanes_C53E[0]   },
  { STAGE5_MAP_HAZARDS_C552_ADDR, &stage5_map_hazards_C552[0] },
  { STAGE5_MAP_LOBJS_C560_ADDR,   &stage5_map_lobjs_C560[0]   },
  { STAGE5_MAP_ROBJS_C5A3_ADDR,   &stage5_map_robjs_C5A3[0]   },
  { STAGE5_MAP_CURV_C5E3_ADDR,    &stage5_map_curv_C5E3[0]    },
  { STAGE5_MAP_HEIGHT_C5F8_ADDR,  &stage5_map_height_C5F8[0]  },
  { STAGE5_MAP_LANES_C614_ADDR,   &stage5_map_lanes_C614[0]   },
  { STAGE5_MAP_HAZARDS_C628_ADDR, &stage5_map_hazards_C628[0] },
  { STAGE5_MAP_LOBJS_C62D_ADDR,   &stage5_map_lobjs_C62D[0]   },
  { STAGE5_MAP_ROBJS_C651_ADDR,   &stage5_map_robjs_C651[0]   },
  { STAGE5_MAP_CURV_C65C_ADDR,    &stage5_map_curv_C65C[0]    },
  { STAGE5_MAP_HEIGHT_C684_ADDR,  &stage5_map_height_C684[0]  },
  { STAGE5_MAP_LANES_C6BB_ADDR,   &stage5_map_lanes_C6BB[0]   },
  { STAGE5_MAP_HAZARDS_C6D1_ADDR, &stage5_map_hazards_C6D1[0] },
  { STAGE5_MAP_LOBJS_C6EE_ADDR,   &stage5_map_lobjs_C6EE[0]   },
  { STAGE5_MAP_ROBJS_C7A5_ADDR,   &stage5_map_robjs_C7A5[0]   },
};

// clang-format on
