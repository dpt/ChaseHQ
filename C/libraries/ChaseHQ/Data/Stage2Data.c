/**
 * Stage2Data.c
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

#include "Stage2Data.h"

/* Z80 addresses of the map sections, as referenced by the map
 * GOTO/SPLIT commands and stage2_map_goto_table[]. */
#define STAGE2_MAP_CURV_E439_ADDR    (0x6039)
#define STAGE2_MAP_HEIGHT_E44F_ADDR  (0x604F)
#define STAGE2_MAP_LANES_E474_ADDR   (0x6074)
#define STAGE2_MAP_HAZARDS_E488_ADDR (0x6088)
#define STAGE2_MAP_LOBJS_E4A8_ADDR   (0x60A8)
#define STAGE2_MAP_ROBJS_E4F9_ADDR   (0x60F9)
#define STAGE2_MAP_CURV_E53C_ADDR    (0x613C)
#define STAGE2_MAP_HEIGHT_E54F_ADDR  (0x614F)
#define STAGE2_MAP_LANES_E561_ADDR   (0x6161)
#define STAGE2_MAP_HAZARDS_E57B_ADDR (0x617B)
#define STAGE2_MAP_LOBJS_E592_ADDR   (0x6192)
#define STAGE2_MAP_ROBJS_E5D0_ADDR   (0x61D0)
#define STAGE2_MAP_CURV_E60B_ADDR    (0x620B)
#define STAGE2_MAP_HEIGHT_E61E_ADDR  (0x621E)
#define STAGE2_MAP_LANES_E62F_ADDR   (0x622F)
#define STAGE2_MAP_HAZARDS_E643_ADDR (0x6243)
#define STAGE2_MAP_LOBJS_E648_ADDR   (0x6248)
#define STAGE2_MAP_ROBJS_E668_ADDR   (0x6268)
#define STAGE2_MAP_CURV_E674_ADDR    (0x6274)
#define STAGE2_MAP_HEIGHT_E69C_ADDR  (0x629C)
#define STAGE2_MAP_LANES_E6E0_ADDR   (0x62E0)
#define STAGE2_MAP_HAZARDS_E6F0_ADDR (0x62F0)
#define STAGE2_MAP_LOBJS_E713_ADDR   (0x6313)
#define STAGE2_MAP_ROBJS_E79B_ADDR   (0x639B)

/* ----------------------------------------------------------------------- */

/* Stage 2 object type macros */

#define MAP_OBJ_S2_HUGE_ROCK_VAL   (4)
#define MAP_OBJ_S2_PALM_TREE_VAL   (5)
#define MAP_OBJ_S2_LEAVES_VAL      (6)
#define MAP_OBJ_S2_DOUBLE_LAMP_VAL (7)

#define MAP_OBJ_S2_HUGE_ROCK(D)    (((D) << 4) | MAP_OBJ_S2_HUGE_ROCK_VAL)
#define MAP_OBJ_S2_PALM_TREE(D)    (((D) << 4) | MAP_OBJ_S2_PALM_TREE_VAL)
#define MAP_OBJ_S2_LEAVES(D)       (((D) << 4) | MAP_OBJ_S2_LEAVES_VAL)
#define MAP_OBJ_S2_DOUBLE_LAMP(D)  (((D) << 4) | MAP_OBJ_S2_DOUBLE_LAMP_VAL)

/* ----------------------------------------------------------------------- */

/* Forward declarations */

static const char *stage2_chatter_strings[4];
static const u8 stage2_arrest_messages_E1DD[71];
static const hittable_t stage2_hittable_objects_E224[2];
static const obj_t stage2_right_obj_defs_E22A[7];
static const obj_t stage2_left_obj_defs_E25B[7];
static const u8 stage2_map_curv_E28C[36];
static const u8 stage2_map_height_E2B0[54];
static const u8 stage2_map_lanes_E2E6[22];
static const u8 stage2_map_hazards_E2FC[52];
static const u8 stage2_map_lobjs_E330[146];
static const u8 stage2_map_robjs_E3C2[119];
static const u8 stage2_map_curv_E439[22];
static const u8 stage2_map_height_E44F[37];
static const u8 stage2_map_lanes_E474[20];
static const u8 stage2_map_hazards_E488[32];
static const u8 stage2_map_lobjs_E4A8[81];
static const u8 stage2_map_robjs_E4F9[67];
static const u8 stage2_map_curv_E53C[19];
static const u8 stage2_map_height_E54F[18];
static const u8 stage2_map_lanes_E561[26];
static const u8 stage2_map_hazards_E57B[23];
static const u8 stage2_map_lobjs_E592[62];
static const u8 stage2_map_robjs_E5D0[59];
static const u8 stage2_map_curv_E60B[19];
static const u8 stage2_map_height_E61E[17];
static const u8 stage2_map_lanes_E62F[20];
static const u8 stage2_map_hazards_E643[5];
static const u8 stage2_map_lobjs_E648[32];
static const u8 stage2_map_robjs_E668[12];
static const u8 stage2_map_curv_E674[40];
static const u8 stage2_map_height_E69C[68];
static const u8 stage2_map_lanes_E6E0[16];
static const u8 stage2_map_hazards_E6F0[35];
static const u8 stage2_map_lobjs_E713[136];
static const u8 stage2_map_robjs_E79B[176];
static const pixel_t stage2_perp_face[180];
static const bitmap_t stage2_lods_E8FF[6];
static const bitmap_t stage2_lods_E929[6];
static const bitmap_t stage2_lods_E953[6];
static const stretchy_t stage2_stretchy_F10E[5];
static const depthset_t stage2_depthset_F11B;
static const depthset_t stage2_depthset_F131;
static const depthset_t stage2_depthset_F147;
static const depthset_t stage2_depthset_F15D;
static const stretchy_t stage2_stretchy_F173[5];
static const depthset_t stage2_depthset_F180;
static const depthset_t stage2_depthset_F196;
static const depthset_t stage2_depthset_F1AC;
static const depthset_t stage2_depthset_F1C2;
static const bitmap_t stage2_lods_F1D8[20];
static const bitmap_t stage2_lods_F264[20];
static const stretchy_t stage2_stretchy_F509[10];
static const stretchy_t stage2_stretchy_F525[10];
static const stretchy_t stage2_stretchy_F541[4];
static const stretchy_t stage2_stretchy_F54B[4];
static const depthset_t stage2_depthset_F555;
static const depthset_t stage2_depthset_F56B;
static const depthset_t stage2_depthset_F581;
static const depthset_t stage2_depthset_F597;
static const depthset_t stage2_depthset_F5AD;
static const depthset_t stage2_depthset_F5C3;
static const depthset_t stage2_depthset_F5D9;
static const depthset_t stage2_depthset_F5EF;
static const depthset_t stage2_depthset_F605;
static const depthset_t stage2_depthset_F61B;
static const depthset_t stage2_depthset_F631;
static const depthset_t stage2_depthset_F647;
static const depthset_t stage2_depthset_F65D;
static const bitmap_t stage2_lods_F673[35];
static const stretchy_t stage2_stretchy_F912[5];
static const stretchy_t stage2_stretchy_F91F[5];
static const depthset_t stage2_depthset_F92C;
static const depthset_t stage2_depthset_F942;
static const bitmap_t stage2_lods_F958[5];
static const bitmap_t stage2_lods_FA0F[6];
static const pixel_t stage2_bitmap_EA9E[2 * 2 * 8 * 1];
static const pixel_t stage2_bitmap_EABE[2 * 2 * 8 * 1];
static const pixel_t stage2_bitmap_EADE[6 * 1 * 30 * 1];
static const pixel_t stage2_bitmap_EB92[4 * 1 * 22 * 1];
static const pixel_t stage2_bitmap_EBEA[3 * 1 * 15 * 1];
static const pixel_t stage2_bitmap_EC57[6 * 1 * 30 * 1];
static const pixel_t stage2_bitmap_ED0B[4 * 1 * 22 * 1];
static const pixel_t stage2_bitmap_ED63[3 * 1 * 16 * 1];
static const pixel_t stage2_bitmap_F768[5 * 1 * 4 * 1];
static const pixel_t stage2_bitmap_F77C[4 * 1 * 11 * 1];
static const pixel_t stage2_bitmap_F7A8[5 * 1 * 8 * 1];
static const pixel_t stage2_bitmap_F7D0[4 * 1 * 6 * 1];
static const pixel_t stage2_bitmap_F7E8[3 * 1 * 4 * 1];
static const pixel_t stage2_bitmap_F7F4[3 * 1 * 4 * 1];
static const pixel_t stage2_bitmap_F800[4 * 1 * 11 * 1];
static const pixel_t stage2_bitmap_F82C[3 * 1 * 4 * 1];
static const pixel_t stage2_bitmap_F838[2 * 1 * 2 * 1];
static const pixel_t stage2_bitmap_F83C[2 * 1 * 5 * 1];
static const pixel_t stage2_bitmap_F846[3 * 1 * 4 * 1];
static const pixel_t stage2_bitmap_F854[2 * 1 * 4 * 1];
static const pixel_t stage2_bitmap_F85C[2 * 1 * 2 * 1];
static const pixel_t stage2_bitmap_F860[2 * 1 * 4 * 1];
static const pixel_t stage2_bitmap_F868[2 * 1 * 4 * 1];
static const pixel_t stage2_bitmap_F870[2 * 1 * 2 * 1];
static const pixel_t stage2_bitmap_F874[1 * 2 * 3 * 1];
static const pixel_t stage2_bitmap_F87A[1 * 2 * 3 * 1];
static const pixel_t stage2_bitmap_F880[1 * 2 * 3 * 1];
static const pixel_t stage2_bitmap_F886[1 * 2 * 2 * 1];
static const pixel_t stage2_bitmap_F88A[1 * 2 * 2 * 1];
static const pixel_t stage2_bitmap_F88E[1 * 2 * 2 * 1];
static const pixel_t stage2_bitmap_F892[1 * 2 * 2 * 1];
static const pixel_t stage2_bitmap_F896[1 * 2 * 2 * 1];
static const pixel_t stage2_bitmap_F89A[1 * 2 * 2 * 1];
static const pixel_t stage2_bitmap_F89E[1 * 2 * 2 * 1];
static const pixel_t stage2_bitmap_F8A6[1 * 2 * 2 * 1];
static const pixel_t stage2_bitmap_F8AA[2 * 2 * 1 * 1];
static const pixel_t stage2_bitmap_F8AE[2 * 2 * 1 * 1];
static const pixel_t stage2_bitmap_F8B2[2 * 2 * 2 * 1];
static const pixel_t stage2_bitmap_F8BA[2 * 2 * 1 * 1];
static const pixel_t stage2_bitmap_F8BE[2 * 2 * 3 * 1];
static const pixel_t stage2_bitmap_F8CA[2 * 2 * 3 * 1];
static const pixel_t stage2_bitmap_F8D6[2 * 2 * 2 * 1];
static const pixel_t stage2_bitmap_F8DE[2 * 2 * 1 * 1];
static const pixel_t stage2_bitmap_F8E2[2 * 2 * 1 * 1];
static const pixel_t stage2_bitmap_F8E6[2 * 2 * 2 * 1];
static const pixel_t stage2_bitmap_F8EE[2 * 2 * 1 * 1];
static const pixel_t stage2_bitmap_F8F2[2 * 2 * 3 * 1];
static const pixel_t stage2_bitmap_F8FE[2 * 2 * 3 * 1];
static const pixel_t stage2_bitmap_F90A[2 * 2 * 2 * 1];
static const pixel_t stage2_bitmap_F9F7[2 * 2 * 2 + 4];
static const pixel_t stage2_bitmap_FA03[2 * 2 * 2 + 4];

/* ----------------------------------------------------------------------- */

/** $E000: stage2 */
// clang-format off
const stage_t stage2 = {
  /* $E000 backdrop */
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
  /* $E0F0 perstage */
  &stage2_perp_face[FACEBITMAPBYTES],
  &pilot_mugshot[0],
  attribute_BLACK_OVER_WHITE * 0x0101,
  &stage2_hittable_objects_E224[0],  /* addrof_hittable_objects */
  &stage2_right_obj_defs_E22A[-1].arg,  /* addrof_right_hand_handlers */
  &stage2_right_obj_defs_E22A[-1],  /* addrof_right_hand_objects */
  &stage2_right_obj_defs_E22A[2],  /* addrof_right_hand_short_pole_object */
  &stage2_left_obj_defs_E25B[-1].arg,  /* addrof_left_hand_handlers */
  &stage2_left_obj_defs_E25B[-1],  /* addrof_left_hand_objects */
  &stage2_left_obj_defs_E25B[2],  /* addrof_left_hand_short_pole_object */
  &perp_description[0],  /* addrof_perp_description */
  &stage2_arrest_messages_E1DD[0],  /* addrof_arrest_messages */
  { heli_table_frame0, heli_table_frame1 },  /* addrof_helicopter_frames ($ED93, $ED9F) */

  NULL,  /* bitmaps_stones */
  NULL,  /* bitmaps_dust */
  &stage2_lods_E8FF[0],  /* bitmaps_perp_car */
  {
    &stage2_lods_E953[0],
    &stage2_lods_E929[0],
    &stage2_lods_E953[0],
    &stage2_lods_E8FF[0]
  },  /* bitmaps_vehicles */

  /* $E11A difficulty */
  15,  /* car_spawn_delay */
  50,  /* perp_lane_change_base */
  40,  /* perp_approach_base */

  /* $E11D setupdata */
  {
    378,
    &stage2_map_curv_E28C[-1],
    &stage2_map_height_E2B0[-1],
    &stage2_map_lanes_E2E6[-1],
    &stage2_map_robjs_E3C2[-1],
    &stage2_map_lobjs_E330[-1],
    &stage2_map_hazards_E2FC[-1],
  },

  /* $E12B attractdata */
  {
    234,
    &stage2_map_curv_E674[-1],
    &stage2_map_height_E69C[-1],
    &stage2_map_lanes_E6E0[-1],
    &stage2_map_robjs_E79B[-1],
    &stage2_map_lobjs_E713[-1],
    &stage2_map_hazards_E6F0[-1],
  },

  stage2_chatter_strings
};

/* ----------------------------------------------------------------------- */

/* $E139: perp_description - Conv: Deduped to CommonData.c (shared with stage 1/3/4/5) */

/** $E145 (bank 1) */
static const char *stage2_chatter_strings[4] = {
  "THIS IS NANCY AT CHASE H.Q. WE'VE GOT A\xCE",
  "EMERGENCY HERE. CARLOS, THE NEW YOR\xCB",
  "ARMED ROBBER, HAS BEEN SPOTTED IN \xC1",
  "YELLOW SPORTS CAR ON THE FREEWAY... OVER\xAE",
};

/* ----------------------------------------------------------------------- */

/**
 * $E1DD: stage2_arrest_messages_E1DD
 */
static const u8 stage2_arrest_messages_E1DD[71] = {
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
  CHQBACKBUF(0xF022),
  ZXATTRS(0x5942),
  'S', 'U', 'S', 'P', 'I', 'C', 'I', 'O', 'N', ' ', 'O', 'F', ' ', 'A', 'R', 'M', 'E', 'D', ' ', 'R', 'O', 'B', 'B', 'E', 'R', 'Y', '.' | EOS,

  TRANSITIONCONTROL_FILL_ATTRIBUTES,
  DRAWOVERLAY_STOP
};

/* ----------------------------------------------------------------------- */

/** $E224: stage2_hittable_objects_E224 */
static const hittable_t stage2_hittable_objects_E224[2] = {
  { 32, &stage2_lods_FA0F[0] },
  { 32, &stage2_lods_FA0F[0] },
};

/** $E22A: stage2_right_obj_defs_E22A */
static const obj_t stage2_right_obj_defs_E22A[7] = {
  { 111, 41, 80, &tunnellight, draw_tunnel_light_right },
  { 0, 0, 0, NULL, NULL },
  { 144, 92, 40, &stretchy_shortpole, draw_stretchy_object_right },
  { 127, 16, 60, &stage2_stretchy_F10E[0], draw_stretchy_object_right },
  { 107, 53, 70, &stage2_stretchy_F509[0], draw_stretchy_object_right },
  { 116, 33, 80, &stage2_stretchy_F541[0], draw_stretchy_object_right },
  { 110, 53, 80, &stage2_stretchy_F912[0], draw_stretchy_object_right },
};

/** $E25B: stage2_left_obj_defs_E25B */
static const obj_t stage2_left_obj_defs_E25B[7] = {
  { 126, 188, 80, &tunnellight, draw_tunnel_light_left },
  { 0, 0, 0, NULL, NULL },
  { 96, 144, 40, &stretchy_shortpole, draw_stretchy_object_left },
  { 112, 224, 60, &stage2_stretchy_F173[0], draw_stretchy_object_left },
  { 136, 187, 80, &stage2_stretchy_F525[0], draw_stretchy_object_left },
  { 124, 200, 80, &stage2_stretchy_F54B[0], draw_stretchy_object_left },
  { 132, 182, 80, &stage2_stretchy_F91F[0], draw_stretchy_object_left },
};

/* ----------------------------------------------------------------------- */

/** $E28C: stage2_map_curv_E28C */
static const u8 stage2_map_curv_E28C[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(13),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(9),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(12),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(5),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(14),
  MAP_CURVE_LEFT(12),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(14),
  MAP_CURVE_LEFT(14),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(14),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(1),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(7),
  MAP_CMD_SPLIT(STAGE2_MAP_CURV_E439_ADDR, STAGE2_MAP_CURV_E53C_ADDR)
};

/** $E2B0: stage2_map_height_E2B0 */
static const u8 stage2_map_height_E2B0[] = {
  MAP_HEIGHT_LEVEL(2),
  MAP_HEIGHT_UP1(7),
  MAP_HEIGHT_UP3(12),
  MAP_HEIGHT_UP1(3),
  MAP_HEIGHT_LEVEL(3),
  MAP_HEIGHT_DOWN1(3),
  MAP_HEIGHT_DOWN3(15),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_DOWN1(6),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(10),
  MAP_HEIGHT_UP1(4),
  MAP_HEIGHT_UP3(4),
  MAP_HEIGHT_UP5(12),
  MAP_HEIGHT_UP3(4),
  MAP_HEIGHT_UP1(3),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(14),
  MAP_HEIGHT_DOWN1(2),
  MAP_HEIGHT_DOWN3(8),
  MAP_HEIGHT_DOWN5(7),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_DOWN1(3),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(8),
  MAP_HEIGHT_DOWN1(3),
  MAP_HEIGHT_DOWN3(6),
  MAP_HEIGHT_DOWN5(15),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(3),
  MAP_HEIGHT_LEVEL(3),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_UP5(4),
  MAP_HEIGHT_UP7(14),
  MAP_HEIGHT_UP5(4),
  MAP_HEIGHT_DOWN5(4),
  MAP_HEIGHT_DOWN3(14),
  MAP_HEIGHT_DOWN1(3),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(4),
  MAP_CMD_SPLIT(STAGE2_MAP_HEIGHT_E44F_ADDR, STAGE2_MAP_HEIGHT_E54F_ADDR)
};

/** $E2E6: stage2_map_lanes_E2E6 */
static const u8 stage2_map_lanes_E2E6[] = {
  MAP_LANES_4(2),
  MAP_LANES_4TO3R(30),
  MAP_LANES_3RTO4(2),
  MAP_LANES_4(10),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(130),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(222),
  MAP_CMD_SPLIT(STAGE2_MAP_LANES_E474_ADDR, STAGE2_MAP_LANES_E561_ADDR)
};

/** $E2FC: stage2_map_hazards_E2FC */
static const u8 stage2_map_hazards_E2FC[] = {
  MAP_HAZARD_WAIT(38),
  MAP_CMD_START_OBSTACLE_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(10),
  MAP_CMD_START_OBSTACLE_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(45),
  MAP_CMD_START_OBSTACLE_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(16),
  MAP_CMD_START_OBSTACLE_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(8),
  MAP_CMD_HELI_TURN_L,
  MAP_HAZARD_WAIT(19),
  MAP_CMD_START_OBSTACLE_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(29),
  MAP_CMD_START_OBSTACLE_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(13),
  MAP_CMD_STOP_CARS,
  MAP_HAZARD_WAIT(9),
  MAP_CMD_ARROW_L,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_SPLIT(STAGE2_MAP_HAZARDS_E488_ADDR, STAGE2_MAP_HAZARDS_E57B_ADDR)
};

/** $E330: stage2_map_lobjs_E330 */
static const u8 stage2_map_lobjs_E330[] = {
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(13),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_CMD_SPLIT(STAGE2_MAP_LOBJS_E4A8_ADDR, STAGE2_MAP_LOBJS_E592_ADDR)
};

/** $E3C2: stage2_map_robjs_E3C2 */
static const u8 stage2_map_robjs_E3C2[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(6),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(13),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(6),
  MAP_CMD_SPLIT(STAGE2_MAP_ROBJS_E4F9_ADDR, STAGE2_MAP_ROBJS_E5D0_ADDR)
};

/** $E439: stage2_map_curv_E439 */
static const u8 stage2_map_curv_E439[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(9),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(3),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(14),
  MAP_CURVE_RIGHT(13),
  MAP_CURVE_RIGHT_HARD(8),
  MAP_CURVE_RIGHT(8),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(6),
  MAP_CURVE_LEFT(14),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(6),
  MAP_CURVE_STRAIGHT(9),
  MAP_CMD_GOTO(STAGE2_MAP_CURV_E60B_ADDR)
};

/** $E44F: stage2_map_height_E44F */
static const u8 stage2_map_height_E44F[] = {
  MAP_HEIGHT_UP3(8),
  MAP_HEIGHT_UP5(6),
  MAP_HEIGHT_UP7(7),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_UP3(3),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_DOWN1(2),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_DOWN5(15),
  MAP_HEIGHT_DOWN5(15),
  MAP_HEIGHT_DOWN5(5),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_DOWN1(2),
  MAP_HEIGHT_UP1(2),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_UP5(15),
  MAP_HEIGHT_UP5(15),
  MAP_HEIGHT_UP5(15),
  MAP_HEIGHT_UP5(3),
  MAP_HEIGHT_UP3(15),
  MAP_HEIGHT_UP3(15),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN5(15),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN7(11),
  MAP_HEIGHT_DOWN5(15),
  MAP_HEIGHT_DOWN5(5),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN1(1),
  MAP_CMD_GOTO(STAGE2_MAP_HEIGHT_E61E_ADDR)
};

/** $E474: stage2_map_lanes_E474 */
static const u8 stage2_map_lanes_E474[] = {
  MAP_LANES_4(78),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(10),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(42),
  MAP_LANES_4TO3R(4),
  MAP_LANES_3RTO4(2),
  MAP_LANES_4(70),
  MAP_CMD_GOTO(STAGE2_MAP_LANES_E62F_ADDR)
};

/** $E488: stage2_map_hazards_E488 */
static const u8 stage2_map_hazards_E488[] = {
  MAP_HAZARD_WAIT(10),
  MAP_CMD_START_CARS,
  MAP_HAZARD_WAIT(11),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(6),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(9),
  MAP_CMD_START_OBSTACLE_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(50),
  MAP_CMD_HELI_LEAVE,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(14),
  MAP_CMD_GOTO(STAGE2_MAP_HAZARDS_E643_ADDR)
};

/** $E4A8: stage2_map_lobjs_E4A8 */
static const u8 stage2_map_lobjs_E4A8[] = {
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_CMD_GOTO(STAGE2_MAP_LOBJS_E648_ADDR)
};

/** $E4F9: stage2_map_robjs_E4F9 */
static const u8 stage2_map_robjs_E4F9[] = {
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_CMD_GOTO(STAGE2_MAP_ROBJS_E668_ADDR)
};

/** $E53C: stage2_map_curv_E53C */
static const u8 stage2_map_curv_E53C[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(9),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(7),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(14),
  MAP_CMD_GOTO(STAGE2_MAP_CURV_E60B_ADDR)
};

/** $E54F: stage2_map_height_E54F */
static const u8 stage2_map_height_E54F[] = {
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
  MAP_CMD_GOTO(STAGE2_MAP_HEIGHT_E61E_ADDR)
};

/** $E561: stage2_map_lanes_E561 */
static const u8 stage2_map_lanes_E561[] = {
  MAP_LANES_4(6),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(8),
  MAP_LANES_TUNNEL_ENTRY(44),
  MAP_LANES_TUNNEL_EXIT(2),
  MAP_LANES_3L(40),
  MAP_LANES_3LTO2L(2),
  MAP_LANES_2L(38),
  MAP_LANES_2LTO3L(2),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(64),
  MAP_CMD_GOTO(STAGE2_MAP_LANES_E62F_ADDR)
};

/** $E57B: stage2_map_hazards_E57B */
static const u8 stage2_map_hazards_E57B[] = {
  MAP_HAZARD_WAIT(5),
  MAP_CMD_HELI_LEAVE,
  MAP_HAZARD_WAIT(7),
  MAP_CMD_START_CARS,
  MAP_HAZARD_WAIT(59),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(22),
  MAP_CMD_START_TWO_OBSTACLES,
  MAP_HAZARD_WAIT(4),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(7),
  MAP_CMD_GOTO(STAGE2_MAP_HAZARDS_E643_ADDR)
};

/** $E592: stage2_map_lobjs_E592 */
static const u8 stage2_map_lobjs_E592[] = {
  MAP_OBJ_NONE(9),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(7),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(2),
  MAP_CMD_GOTO(STAGE2_MAP_LOBJS_E648_ADDR)
};

/** $E5D0: stage2_map_robjs_E5D0 */
static const u8 stage2_map_robjs_E5D0[] = {
  MAP_OBJ_NONE(9),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(7),
  MAP_OBJ_NONE(13),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_CMD_GOTO(STAGE2_MAP_ROBJS_E668_ADDR)
};

/** $E60B: stage2_map_curv_E60B */
static const u8 stage2_map_curv_E60B[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(13),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(7),
  MAP_CURVE_STRAIGHT(8),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(11),
  MAP_CURVE_STRAIGHT(12),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(12),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(7),
  MAP_CMD_GOTO(STAGE2_MAP_CURV_E674_ADDR)
};

/** $E61E: stage2_map_height_E61E */
static const u8 stage2_map_height_E61E[] = {
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
  MAP_CMD_GOTO(STAGE2_MAP_HEIGHT_E69C_ADDR)
};

/** $E62F: stage2_map_lanes_E62F */
static const u8 stage2_map_lanes_E62F[] = {
  MAP_LANES_4(8),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(10),
  MAP_LANES_TUNNEL_ENTRY(122),
  MAP_LANES_TUNNEL_EXIT(2),
  MAP_LANES_3L(10),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(34),
  MAP_CMD_GOTO(STAGE2_MAP_LANES_E6E0_ADDR)
};

/** $E643: stage2_map_hazards_E643 */
static const u8 stage2_map_hazards_E643[] = {
  MAP_HAZARD_WAIT(95),
  MAP_CMD_GOTO(STAGE2_MAP_HAZARDS_E6F0_ADDR)
};

/** $E648: stage2_map_lobjs_E648 */
static const u8 stage2_map_lobjs_E648[] = {
  MAP_OBJ_NONE(11),
  MAP_OBJ_TUNNEL_LIGHT(14),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(6),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(11),
  MAP_OBJ_NONE(2),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_CMD_GOTO(STAGE2_MAP_LOBJS_E713_ADDR)
};

/** $E668: stage2_map_robjs_E668 */
static const u8 stage2_map_robjs_E668[] = {
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(11),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(4),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(5),
  MAP_CMD_GOTO(STAGE2_MAP_ROBJS_E79B_ADDR)
};

/** $E674: stage2_map_curv_E674 */
static const u8 stage2_map_curv_E674[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(13),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(7),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(8),
  MAP_CURVE_LEFT(8),
  MAP_CURVE_LEFT_HARD(14),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(1),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(1),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(4),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(14),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(4),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(12),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(14),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(11),
  MAP_CURVE_STRAIGHT(9),
  MAP_CMD_GOTO(STAGE2_MAP_CURV_E674_ADDR)
};

/** $E69C: stage2_map_height_E69C */
static const u8 stage2_map_height_E69C[] = {
  MAP_HEIGHT_LEVEL(11),
  MAP_HEIGHT_UP3(14),
  MAP_HEIGHT_DOWN5(15),
  MAP_HEIGHT_DOWN5(5),
  MAP_HEIGHT_UP3(15),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_UP5(3),
  MAP_HEIGHT_UP7(4),
  MAP_HEIGHT_DOWN5(5),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(7),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN5(4),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(10),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_DOWN3(4),
  MAP_HEIGHT_DOWN5(11),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(4),
  MAP_HEIGHT_UP3(9),
  MAP_HEIGHT_DOWN3(15),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_UP3(7),
  MAP_HEIGHT_UP7(3),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_LEVEL(13),
  MAP_HEIGHT_DOWN1(2),
  MAP_HEIGHT_DOWN3(12),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(7),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(3),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN7(2),
  MAP_HEIGHT_DOWN5(2),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(8),
  MAP_CMD_GOTO(STAGE2_MAP_HEIGHT_E69C_ADDR)
};

/** $E6E0: stage2_map_lanes_E6E0 */
static const u8 stage2_map_lanes_E6E0[] = {
  MAP_LANES_4(92),
  MAP_LANES_3L(46),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(102),
  MAP_LANES_3R(6),
  MAP_LANES_4(202),
  MAP_CMD_GOTO(STAGE2_MAP_LANES_E6E0_ADDR)
};

/** $E6F0: stage2_map_hazards_E6F0 */
static const u8 stage2_map_hazards_E6F0[] = {
  MAP_HAZARD_WAIT(19),
  MAP_CMD_START_OBSTACLE_L,
  MAP_HAZARD_WAIT(3),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(23),
  MAP_CMD_START_BARRIERS_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(54),
  MAP_CMD_START_OBSTACLE_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(18),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(100),
  MAP_CMD_GOTO(STAGE2_MAP_HAZARDS_E6F0_ADDR)
};

/** $E713: stage2_map_lobjs_E713 */
static const u8 stage2_map_lobjs_E713[] = {
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(8),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(2),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(2),
  MAP_CMD_GOTO(STAGE2_MAP_LOBJS_E713_ADDR)
};

/** $E79B: stage2_map_robjs_E79B */
static const u8 stage2_map_robjs_E79B[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_LEAVES(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_PALM_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S2_HUGE_ROCK(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S2_DOUBLE_LAMP(1),
  MAP_CMD_GOTO(STAGE2_MAP_ROBJS_E79B_ADDR)
};

/**
 * $E84B: stage2_perp_face
 */
static const pixel_t stage2_perp_face[FACEBYTES] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXX__, ____XX_X, XX___X_X, XXXXXXXX,
  XXXXX___, ______XX, X___X_XX, XXXXXXXX,
  XXXXX___, ________, _____X_X, XXXXXXXX,
  XXXX____, ________, ______X_, XXXXXXXX,
  XXXX____, ________, _____X_X, XXXXXXXX,
  XXXX____, ________, __X___X_, XXXXXXXX,
  XXX_____, ________, ___X_X_X, XXXXXXXX,
  XXX___XX, XXX_____, __X_XXXX, XXXXXXXX,
  XXX__XXX, _XXXX___, ___XXXXX, XXXXXXXX,
  XXX_X___, __XXX__X, X_XXXXXX, XXXXXXXX,
  XXX_____, ___XXXXX, XXXXXX_X, _XXXXXXX,
  XXXX___X, XXXXXXXX, XXXXXXXX, X_XXXXXX,
  XXXX_XX_, XX_XXX__, _XXX_XX_, XXXXXXXX,
  XXXX____, __XXXX__, __XXX___, __XXXXXX,
  XXXX_X__, _XXXX___, _XXXX___, _X__XXXX,
  XXX___XX, __X_____, _XXXXX_X, X_X__XXX,
  XXX_____, ________, __XXX_X_, ____XXXX,
  XX______, __XX____, __XXXX__, _____XXX,
  XXX_____, XX______, __XXXXX_, ____XXXX,
  XX_X____, XX______, ___XXX_X, ___X_XXX,
  XXX____X, XX_XX___, __XXXXX_, ____XXXX,
  XX_X___X, _XXXXX__, _XXXXXXX, _X_XXXXX,
  XXX___X_, XXXXXXXX, XXXXXXXX, X_X_XXXX,
  XXXX_XX_, XXXXX_XX, XXXXXXXX, _XXXXXXX,
  XXX_X_XX, XX_X_X_X, _XX_X_XX, X_XXXXXX,
  XXXX_XXX, X_X_____, _______X, XXXXXXXX,
  XXXXX_XX, ____XXXX, XXX___X_, XXXXXXXX,
  XXXXXXX_, X_XX____, _______X, XXXXXXXX,
  XXXXXXXX, ________, ___XX___, XXXXXXXX,
  XXXXXXX_, X____XXX, XXXX___X, XXXXXXXX,
  XXXXXXXX, __X_X_XX, X_X_X_X_, XXXXXXXX,
  XXXXXXX_, X__X_X_X, _X_X_X_X, XXXXXXXX,
  XXXXXXXX, _XX_XXXX, XXX_X_XX, XXXXXXXX,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX__XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX___X_, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XX_____X, _X_XXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,

  attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
};

/* ----------------------------------------------------------------------- */

/** $E8FF: stage2_lods_E8FF */
static const bitmap_t stage2_lods_E8FF[6] = {
  { 6, BITMAPFLAG_DEFAULT, 29, &bitmap_E97D[0], &bitmap_E97D[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 19, &bitmap_EA2B[0], &bitmap_EA2B[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 13, &bitmap_EA77[0], &bitmap_EA77[0] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 13, &bitmap_EA77[0], &bitmap_EA77[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 8, &stage2_bitmap_EA9E[0], &stage2_bitmap_EA9E[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 8, &stage2_bitmap_EA9E[0], &stage2_bitmap_EABE[0] },  // [5]
};

/** $E929: stage2_lods_E929 */
static const bitmap_t stage2_lods_E929[6] = {
  { 6, BITMAPFLAG_DEFAULT, 30, &stage2_bitmap_EADE[0], &stage2_bitmap_EADE[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 22, &stage2_bitmap_EB92[0], &stage2_bitmap_EB92[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 15, &stage2_bitmap_EBEA[0], &stage2_bitmap_EBEA[0] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 15, &stage2_bitmap_EBEA[0], &stage2_bitmap_EBEA[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 8, &bitmap_EC17[0], &bitmap_EC37[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 8, &bitmap_EC17[0], &bitmap_EC37[0] },  // [5]
};

/** $E953: stage2_lods_E953 */
static const bitmap_t stage2_lods_E953[6] = {
  { 6, BITMAPFLAG_DEFAULT, 30, &stage2_bitmap_EC57[0], &stage2_bitmap_EC57[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 22, &stage2_bitmap_ED0B[0], &stage2_bitmap_ED0B[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 16, &stage2_bitmap_ED63[0], &stage2_bitmap_ED63[0] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 16, &stage2_bitmap_ED63[0], &stage2_bitmap_ED63[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 8, &bitmap_EC17[0], &bitmap_EC37[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 8, &bitmap_EC17[0], &bitmap_EC37[0] },  // [5]
};

/* $E97D: bitmap_E97D - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $EA2B: bitmap_EA2B - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $EA77: bitmap_EA77 - Conv: Deduped to CommonData.c (shared with stage 4) */

/**
 * $EA9E: stage2_bitmap_EA9E
 */
static const pixel_t stage2_bitmap_EA9E[2 * 2 * 8 * 1] = {
  ________, XXXXXXXX, _____XXX, XXXXX___,
  ________, XX__XXXX, _____XXX, X__XX___,
  X_______, _XX_X_X_, ____XXXX, X_XX____,
  ________, X_XXXX_X, _____XXX, XXX_X___,
  ________, XXXXXXXX, _____XXX, XXXXX___,
  ________, X_X_X_X_, _____XXX, X_X_X___,
  X_______, _X___XXX, ____XXXX, ___X____,
  XX______, __XXXXXX, ___XXXXX, XXX_____,
};

/**
 * $EABE: stage2_bitmap_EABE
 */
static const pixel_t stage2_bitmap_EABE[2 * 2 * 8 * 1] = {
  XXX_____, ___XXXXX, ________, XXXXXXXX,
  XXX_____, ___XX__X, ________, XXXX__XX,
  XXXX____, ____XX_X, _______X, _X_X_XX_,
  XXX_____, ___X_XXX, ________, X_XXXX_X,
  XXX_____, ___XXXXX, ________, XXXXXXXX,
  XXX_____, ___X_X_X, ________, _X_X_X_X,
  XXXX____, ____X___, _______X, XXX___X_,
  XXXXX___, _____XXX, ______XX, XXXXXX__,
};

/**
 * $EADE: stage2_bitmap_EADE
 */
static const pixel_t stage2_bitmap_EADE[6 * 1 * 30 * 1] = {
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  _XXX__X_, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXX_X_, X__XXX__,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXX_,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXX_,
  XXXXX_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_XXXXX_,
  _XX_____, ________, ________, ________, ________, ____XX__,
  _XX_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_XX__,
  XX_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_XX_,
  X_XX_XX_, XX_XX_XX, _XX_XX_X, X_XX_XX_, XX_XX_XX, _XX_XXX_,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  X_X_X_X_, ___X_XXX, X_X_X_X_, X_X_X_XX, XX_X____, X_X_X_X_,
  XX_X_X_X, __X_XX_X, ________, _______X, _XX_X__X, _X_X_XX_,
  X_____X_, ___X_XXX, ________, _______X, XX_X____, X_____X_,
  XX___X_X, __X_XX_X, ________, _______X, _XX_X__X, _X___XX_,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  X_______, ________, ________, ________, ________, ______X_,
  _X______, ________, ________, ________, ________, _____X__,
  _X__X___, ________, ________, ________, ________, __X__X__,
  __XX__X_, X_______, ________, ________, ______X_, X__XX___,
  __X_XX__, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__, _XX_X___,
  _X___XX_, _X___XXX, XXXXXX__, _XXXXXXX, XX___X__, XX___X__,
  _X____XX, __X___XX, XXXXXX__, _XXXXXXX, X___X__X, X____X__,
  __XXXXXX, X__X____, ________, __XXXXXX, ___X__XX, XXXXX___,
  _______X, XX_XX___, ________, __XXXXXX, __XX_XXX, ________,
  ________, XX__XX__, ________, ___XXXX_, _XX__XX_, ________,
  ________, _XX__XXX, XXXXXXXX, XXXXXXXX, XX__XX__, ________,
  ________, ___X____, ________, ________, ___X____, ________,
  ________, ____XX__, ________, ________, _XX_____, ________,
  ________, ______XX, XXXXXXXX, XXXXXXXX, X_______, ________,
};

/**
 * $EB92: stage2_bitmap_EB92
 */
static const pixel_t stage2_bitmap_EB92[4 * 1 * 22 * 1] = {
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  _XX_X_XX, XXXXXXXX, XXXXXXXX, XX_X_XX_,
  XX_XXXXX, XXXXXXXX, XXXXXXXX, XXXXX_XX,
  XX_XXXXX, XXXXXXXX, XXXXXXXX, XXXXX_XX,
  _XXX____, ________, ________, ____XXX_,
  _X_X_X_X, _X_X_X_X, X_X_X_X_, X_X_X_X_,
  X_X_X_X_, X_X_X_X_, _X_X_X_X, _X_X_X_X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X__X_X_X, X_XX_X_X, X_X_XX_X, X_X_X__X,
  X_X_X__X, _XXX____, ____XXX_, X__X_X_X,
  XX___X_X, X_XX____, ____XX_X, X_X___XX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ________, ________, _______X,
  _X_X____, ________, ________, ____X_X_,
  _XX_X_X_, ________, ________, _X_X_XX_,
  _X_X___X, XXXXXXXX, XXXXXXXX, X___X_X_,
  _X__XX_X, __XXXXX_, XXXXXX__, X_XX__X_,
  __XXXXX_, X_______, XXXXX__X, _XXXXX__,
  _____XX_, XX______, _XXXX_XX, _XX_____,
  _______X, __XXXXXX, XXXXXX__, X_______,
  ________, X_______, _______X, ________,
  ________, _XXXXXXX, XXXXXXX_, ________,
};

/**
 * $EBEA: stage2_bitmap_EBEA
 */
static const pixel_t stage2_bitmap_EBEA[3 * 1 * 15 * 1] = {
  __XXXXXX, XXXXXXXX, XXXXX___,
  _X_XXXXX, XXXXXXXX, XXXX_X__,
  XXXXXXXX, XXXXXXXX, XXXXXXX_,
  _X______, ________, _____X__,
  X_X_X_X_, X_XX_X_X, _X_X__X_,
  XXXXXXXX, XXXXXXXX, XXXXXXX_,
  X_X_XXX_, X_XX_XX_, XXX_X_X_,
  X__X_XXX, X_____XX, XX_X__X_,
  XXXXXXXX, XXXXXXXX, XXXXXXX_,
  _XX_____, ________, ____XX__,
  _XX__XXX, XXXXXXXX, XX__XX__,
  X__X_X_X, XXX_XXXX, _X_X__X_,
  _XX_X_X_, ____XXX_, X_X_XX__,
  _____X_X, XXXXXXXX, _X______,
  ______XX, XXXXXXXX, X_______,
};

/* $EC17: bitmap_EC17 - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $EC37: bitmap_EC37 - Conv: Deduped to CommonData.c (shared with stage 4) */

/**
 * $EC57: stage2_bitmap_EC57
 */
static const pixel_t stage2_bitmap_EC57[6 * 1 * 30 * 1] = {
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  _XXX__X_, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXX_X_, X__XXX__,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXX_,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXX_,
  XXXXX_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_XXXXX_,
  _XX_____, ________, ________, ________, ________, ____XX__,
  _XX_____, ________, ________, ________, ________, ____XX__,
  XX_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_XX_,
  X_XX_XX_, XX_XX_XX, _XX_XX_X, X_XX_XX_, XX_XX_XX, _XX_XXX_,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  XXX_X_X_, __XXX_XX, X_X_X_X_, X_X_X_XX, X_XXX___, X_X_XXX_,
  XX_X_X_X, ___X_X_X, ________, _______X, _X_X___X, _X_X_XX_,
  XX____X_, ___X___X, ________, _______X, ___X____, X____XX_,
  XXX__X_X, __XX___X, ________, _______X, ___XX__X, _X__XXX_,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  X__X_X_X, _X______, ________, ________, ________, ______X_,
  _X__X_X_, ________, ________, ________, ________, _____X__,
  _XXX_X_X, ________, ________, ________, ________, __X__X__,
  __XX__X_, X_X__X__, ________, ________, ______X_, X__XX___,
  __X_XX__, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__, _XX_X___,
  _X___XX_, _X___XXX, XXXXXX__, _XXXXXXX, XX___X__, XX___X__,
  _X____XX, __X___XX, XXXXXX__, _XXXXXXX, X___X__X, X____X__,
  __XXXXXX, X__X____, ________, __XXXXXX, ___X__XX, XXXXX___,
  _______X, XX_XX___, ________, __XXXXXX, __XX_XXX, ________,
  ________, XX__XX__, ________, ___XXXX_, _XX__XX_, ________,
  ________, _XX__XXX, XXXXXXXX, XXXXXXXX, XX__XX__, ________,
  ________, ___X____, ________, ________, ___X____, ________,
  ________, ____XX__, ________, ________, _XX_____, ________,
  ________, ______XX, XXXXXXXX, XXXXXXXX, X_______, ________,
};

/**
 * $ED0B: stage2_bitmap_ED0B
 */
static const pixel_t stage2_bitmap_ED0B[4 * 1 * 22 * 1] = {
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  _XX_X_XX, XXXXXXXX, XXXXXXXX, XX_X_XX_,
  XX_XXXXX, XXXXXXXX, XXXXXXXX, XXXXX_XX,
  XX_X_X_X, _X_X_X_X, __X_X_X_, X_X_X_XX,
  _XX_____, ________, ________, ______X_,
  _X______, ________, ________, ______X_,
  X_X_X_X_, X_X_X_XX, XX_X_X_X, _X_X_X_X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XX__X__X, XXXXX_XX, XX_XXXXX, X__X__XX,
  X__X____, X__X____, ____X__X, ____X__X,
  XX__X__X, X__X____, ____X__X, X__X__XX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X___X_X_, X_______, ________, _______X,
  X__X_X__, ________, ________, _______X,
  _XX_X_X_, ________, ________, _X_X_XX_,
  _X_X___X, XXXXXXXX, XXXXXXXX, X___X_X_,
  X___XX_X, __XXXXXX, _XXXXX__, X_XX___X,
  _XXXXXX_, X_______, _XXXX__X, _XXXXXX_,
  _____XX_, XX______, _XXXX_XX, _XX_____,
  _______X, __XXXXXX, XXXXXX__, X_______,
  ________, XX______, ______XX, ________,
  ________, __XXXXXX, XXXXXX__, ________,
};

/**
 * $ED63: stage2_bitmap_ED63
 */
static const pixel_t stage2_bitmap_ED63[3 * 1 * 16 * 1] = {
  __XXXXXX, XXXXXXXX, XXXXXX__,
  _X_XXXXX, XXXXXXXX, XXXXX_X_,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  _X______, ________, ______X_,
  X_X_X_X_, X_X_X_X_, X_X_X__X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_X_XXXX, X_X_X_XX, XXXX_X_X,
  XX_X_XX_, X______X, _XX_X_XX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  _XX_____, ________, _____XX_,
  _XX__XXX, XXXXXXXX, XXX__XX_,
  X__X_X_X, XXX__XXX, X_X_X__X,
  _XX_X_X_, _____XXX, _X_X_XX_,
  _____X_X, XXXXXXXX, X_X_____,
  ______X_, ________, _X______,
  _______X, XXXXXXXX, X_______,
};

/* $F05A: pilot_mugshot - Conv: Deduped to CommonData.c (shared with stage 4) */

/** $F10E: stage2_stretchy_F10E */
static const stretchy_t stage2_stretchy_F10E[5] = {
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F11B },
  { STRETCHY_TYPE_113PC, &stage2_depthset_F131 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F147 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F15D },
  { STRETCHY_TYPE_END, NULL },
};

/** $F11B: stage2_depthset_F11B */
static const depthset_t stage2_depthset_F11B = {
  &stage2_lods_F1D8[0],
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

/** $F131: stage2_depthset_F131 */
static const depthset_t stage2_depthset_F131 = {
  &stage2_lods_F1D8[0],
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

/** $F147: stage2_depthset_F147 */
static const depthset_t stage2_depthset_F147 = {
  &stage2_lods_F1D8[0],
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

/** $F15D: stage2_depthset_F15D */
static const depthset_t stage2_depthset_F15D = {
  &stage2_lods_F1D8[0],
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

/** $F173: stage2_stretchy_F173 */
static const stretchy_t stage2_stretchy_F173[5] = {
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F180 },
  { STRETCHY_TYPE_113PC, &stage2_depthset_F196 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F1AC },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F1C2 },
  { STRETCHY_TYPE_END, NULL },
};

/** $F180: stage2_depthset_F180 */
static const depthset_t stage2_depthset_F180 = {
  &stage2_lods_F264[0],
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

/** $F196: stage2_depthset_F196 */
static const depthset_t stage2_depthset_F196 = {
  &stage2_lods_F264[0],
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

/** $F1AC: stage2_depthset_F1AC */
static const depthset_t stage2_depthset_F1AC = {
  &stage2_lods_F264[0],
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

/** $F1C2: stage2_depthset_F1C2 */
static const depthset_t stage2_depthset_F1C2 = {
  &stage2_lods_F264[0],
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

/** $F1D8: stage2_lods_F1D8 */
static const bitmap_t stage2_lods_F1D8[20] = {
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

/** $F264: stage2_lods_F264 */
static const bitmap_t stage2_lods_F264[20] = {
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

/* $F2F0: bitmap_F2F0 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F318: bitmap_F318 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F350: bitmap_F350 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F388: bitmap_F388 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F3C8: bitmap_F3C8 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F3D4: bitmap_F3D4 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F3E9: bitmap_F3E9 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F407: bitmap_F407 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F42B: bitmap_F42B - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F433: bitmap_F433 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F43C: bitmap_F43C - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F448: bitmap_F448 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F458: bitmap_F458 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F45B: bitmap_F45B - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F461: bitmap_F461 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F467: bitmap_F467 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F46D: bitmap_F46D - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F4A5: bitmap_F4A5 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F4C3: bitmap_F4C3 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F4CF: bitmap_F4CF - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F4D5: bitmap_F4D5 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F4D7: bitmap_F4D7 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F4DB: bitmap_F4DB - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F4E3: bitmap_F4E3 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F4EF: bitmap_F4EF - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F4F1: bitmap_F4F1 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F4F5: bitmap_F4F5 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F4FD: bitmap_F4FD - Conv: Deduped to CommonData.c (shared with stage 5) */

/** $F509: stage2_stretchy_F509 */
static const stretchy_t stage2_stretchy_F509[10] = {
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F555 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F56B },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F56B },
  { STRETCHY_TYPE_50PC, &stage2_depthset_F581 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F597 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F597 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F5AD },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F5C3 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F5D9 },
  { STRETCHY_TYPE_END, NULL },
};

/** $F525: stage2_stretchy_F525 */
static const stretchy_t stage2_stretchy_F525[10] = {
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F555 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F5EF },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F5EF },
  { STRETCHY_TYPE_50PC, &stage2_depthset_F605 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F61B },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F61B },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F631 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F647 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F65D },
  { STRETCHY_TYPE_END, NULL },
};

/** $F541: stage2_stretchy_F541 */
static const stretchy_t stage2_stretchy_F541[4] = {
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F5AD },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F5C3 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F5D9 },
  { STRETCHY_TYPE_END, NULL },
};

/** $F54B: stage2_stretchy_F54B */
static const stretchy_t stage2_stretchy_F54B[4] = {
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F631 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F647 },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F65D },
  { STRETCHY_TYPE_END, NULL },
};

/** $F555: stage2_depthset_F555 */
static const depthset_t stage2_depthset_F555 = {
  &stage2_lods_F673[0],
  {
    0x1C, 0x02,
    0x14, 0x02,
    0x10, 0x1E,
    0x10, 0x1E,
    0x0C, 0x3A,
    0x0C, 0x3A,
    0x0C, 0x56,
    0x08, 0x56,
    0x08, 0xC6,
    0x08, 0xC6,
  }
};

/** $F56B: stage2_depthset_F56B */
static const depthset_t stage2_depthset_F56B = {
  &stage2_lods_F673[0],
  {
    0x2C, 0x72,
    0x24, 0x72,
    0x18, 0x87,
    0x18, 0x87,
    0x0C, 0x9C,
    0x0C, 0x9C,
    0x0C, 0xB1,
    0x08, 0xB1,
    0x08, 0xCD,
    0x08, 0xCD,
  }
};

/** $F581: stage2_depthset_F581 */
static const depthset_t stage2_depthset_F581 = {
  &stage2_lods_F673[0],
  {
    0x2C, 0x79,
    0x24, 0x79,
    0x18, 0x8E,
    0x18, 0x8E,
    0x0C, 0xA3,
    0x0C, 0xA3,
    0x0C, 0xB8,
    0x08, 0xB8,
    0x08, 0xD4,
    0x08, 0xD4,
  }
};

/** $F597: stage2_depthset_F597 */
static const depthset_t stage2_depthset_F597 = {
  &stage2_lods_F673[0],
  {
    0x2C, 0x80,
    0x24, 0x80,
    0x18, 0x95,
    0x18, 0x95,
    0x0C, 0xAA,
    0x0C, 0xAA,
    0x0C, 0xBF,
    0x08, 0xBF,
    0x08, 0xDB,
    0x08, 0xDB,
  }
};

/** $F5AD: stage2_depthset_F5AD */
static const depthset_t stage2_depthset_F5AD = {
  &stage2_lods_F673[0],
  {
    0x24, 0x09,
    0x1C, 0x09,
    0x18, 0x25,
    0x18, 0x25,
    0x0C, 0x41,
    0x0C, 0x41,
    0x0C, 0x5D,
    0x08, 0x5D,
    0x08, 0xE2,
    0x08, 0xE2,
  }
};

/** $F5C3: stage2_depthset_F5C3 */
static const depthset_t stage2_depthset_F5C3 = {
  &stage2_lods_F673[0],
  {
    0x1C, 0x10,
    0x14, 0x10,
    0x10, 0x2C,
    0x10, 0x2C,
    0x04, 0x48,
    0x04, 0x48,
    0x0C, 0x64,
    0x08, 0x64,
    0x08, 0xE9,
    0x08, 0xE9,
  }
};

/** $F5D9: stage2_depthset_F5D9 */
static const depthset_t stage2_depthset_F5D9 = {
  &stage2_lods_F673[0],
  {
    0x24, 0x17,
    0x1C, 0x17,
    0x18, 0x33,
    0x18, 0x33,
    0x0C, 0x4F,
    0x0C, 0x4F,
    0x0C, 0x6B,
    0x08, 0x6B,
    0x08, 0xF0,
    0x08, 0xF0,
  }
};

/** $F5EF: stage2_depthset_F5EF */
static const depthset_t stage2_depthset_F5EF = {
  &stage2_lods_F673[0],
  {
    0x2C, 0x72,
    0x24, 0x72,
    0x18, 0x87,
    0x18, 0x87,
    0x14, 0x9C,
    0x14, 0x9C,
    0x14, 0xB1,
    0x10, 0xB1,
    0x08, 0xCD,
    0x08, 0xCD,
  }
};

/** $F605: stage2_depthset_F605 */
static const depthset_t stage2_depthset_F605 = {
  &stage2_lods_F673[0],
  {
    0x2C, 0x79,
    0x24, 0x79,
    0x18, 0x8E,
    0x18, 0x8E,
    0x14, 0xA3,
    0x14, 0xA3,
    0x14, 0xB8,
    0x10, 0xB8,
    0x08, 0xD4,
    0x08, 0xD4,
  }
};

/** $F61B: stage2_depthset_F61B */
static const depthset_t stage2_depthset_F61B = {
  &stage2_lods_F673[0],
  {
    0x2C, 0x80,
    0x24, 0x80,
    0x18, 0x95,
    0x18, 0x95,
    0x14, 0xAA,
    0x14, 0xAA,
    0x14, 0xBF,
    0x10, 0xBF,
    0x08, 0xDB,
    0x08, 0xDB,
  }
};

/** $F631: stage2_depthset_F631 */
static const depthset_t stage2_depthset_F631 = {
  &stage2_lods_F673[0],
  {
    0x1C, 0x09,
    0x14, 0x09,
    0x08, 0x25,
    0x08, 0x25,
    0x0C, 0x41,
    0x0C, 0x41,
    0x0C, 0x5D,
    0x08, 0x5D,
    0x08, 0xE2,
    0x08, 0xE2,
  }
};

/** $F647: stage2_depthset_F647 */
static const depthset_t stage2_depthset_F647 = {
  &stage2_lods_F673[0],
  {
    0x1C, 0x10,
    0x14, 0x10,
    0x08, 0x2C,
    0x08, 0x2C,
    0x0C, 0x48,
    0x0C, 0x48,
    0x0C, 0x64,
    0x08, 0x64,
    0x08, 0xE9,
    0x08, 0xE9,
  }
};

/** $F65D: stage2_depthset_F65D */
static const depthset_t stage2_depthset_F65D = {
  &stage2_lods_F673[0],
  {
    0x1C, 0x17,
    0x14, 0x17,
    0x08, 0x33,
    0x08, 0x33,
    0x0C, 0x4F,
    0x0C, 0x4F,
    0x0C, 0x6B,
    0x08, 0x6B,
    0x08, 0xF0,
    0x08, 0xF0,
  }
};

/** $F673: stage2_lods_F673 */
static const bitmap_t stage2_lods_F673[35] = {
  { 5, BITMAPFLAG_DEFAULT, 4, &stage2_bitmap_F768[0], &stage2_bitmap_F768[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 11, &stage2_bitmap_F77C[0], &stage2_bitmap_F77C[0] },  // [1]
  { 5, BITMAPFLAG_DEFAULT, 8, &stage2_bitmap_F7A8[0], &stage2_bitmap_F7A8[0] },  // [2]
  { 4, BITMAPFLAG_DEFAULT, 6, &stage2_bitmap_F7D0[0], &stage2_bitmap_F7D0[0] },  // [3]
  { 3, BITMAPFLAG_DEFAULT, 4, &stage2_bitmap_F7E8[0], &stage2_bitmap_F7E8[0] },  // [4]
  { 3, BITMAPFLAG_DEFAULT, 4, &stage2_bitmap_F7F4[0], &stage2_bitmap_F7F4[0] },  // [5]
  { 4, BITMAPFLAG_DEFAULT, 11, &stage2_bitmap_F800[0], &stage2_bitmap_F800[0] },  // [6]
  { 3, BITMAPFLAG_DEFAULT, 4, &stage2_bitmap_F82C[0], &stage2_bitmap_F82C[0] },  // [7]
  { 2, BITMAPFLAG_DEFAULT, 2, &stage2_bitmap_F838[0], &stage2_bitmap_F838[0] },  // [8]
  { 2, BITMAPFLAG_DEFAULT, 5, &stage2_bitmap_F83C[0], &stage2_bitmap_F83C[0] },  // [9]
  { 3, BITMAPFLAG_DEFAULT, 4, &stage2_bitmap_F846[0], &stage2_bitmap_F846[0] },  // [10]
  { 2, BITMAPFLAG_DEFAULT, 4, &stage2_bitmap_F854[0], &stage2_bitmap_F854[0] },  // [11]
  { 2, BITMAPFLAG_DEFAULT, 2, &stage2_bitmap_F85C[0], &stage2_bitmap_F85C[0] },  // [12]
  { 2, BITMAPFLAG_DEFAULT, 4, &stage2_bitmap_F860[0], &stage2_bitmap_F860[0] },  // [13]
  { 2, BITMAPFLAG_DEFAULT, 4, &stage2_bitmap_F868[0], &stage2_bitmap_F868[0] },  // [14]
  { 2, BITMAPFLAG_DEFAULT, 2, &stage2_bitmap_F870[0], &stage2_bitmap_F870[0] },  // [15]
  { 1, BITMAPFLAG_MASKED, 3, &stage2_bitmap_F874[0], &stage2_bitmap_F874[0] },  // [16]
  { 1, BITMAPFLAG_MASKED, 3, &stage2_bitmap_F87A[0], &stage2_bitmap_F87A[0] },  // [17]
  { 1, BITMAPFLAG_MASKED, 3, &stage2_bitmap_F880[0], &stage2_bitmap_F880[0] },  // [18]
  { 1, BITMAPFLAG_MASKED, 2, &stage2_bitmap_F886[0], &stage2_bitmap_F886[0] },  // [19]
  { 1, BITMAPFLAG_MASKED, 2, &stage2_bitmap_F88A[0], &stage2_bitmap_F88A[0] },  // [20]
  { 1, BITMAPFLAG_MASKED, 2, &stage2_bitmap_F88E[0], &stage2_bitmap_F88E[0] },  // [21]
  { 1, BITMAPFLAG_MASKED, 2, &stage2_bitmap_F892[0], &stage2_bitmap_F892[0] },  // [22]
  { 1, BITMAPFLAG_MASKED, 2, &stage2_bitmap_F896[0], &stage2_bitmap_F896[0] },  // [23]
  { 1, BITMAPFLAG_MASKED, 2, &stage2_bitmap_F89A[0], &stage2_bitmap_F89A[0] },  // [24]
  { 1, BITMAPFLAG_MASKED, 2, &stage2_bitmap_F89E[0], &stage2_bitmap_F89E[0] },  // [25]
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_F8A2[0], &bitmap_F8A2[0] },  // [26]
  { 1, BITMAPFLAG_MASKED, 2, &stage2_bitmap_F8A6[0], &stage2_bitmap_F8A6[0] },  // [27]
  { 2, BITMAPFLAG_MASKED, 1, &stage2_bitmap_F8AA[0], &stage2_bitmap_F8DE[0] },  // [28]
  { 2, BITMAPFLAG_MASKED, 1, &stage2_bitmap_F8AE[0], &stage2_bitmap_F8E2[0] },  // [29]
  { 2, BITMAPFLAG_MASKED, 2, &stage2_bitmap_F8B2[0], &stage2_bitmap_F8E6[0] },  // [30]
  { 2, BITMAPFLAG_MASKED, 1, &stage2_bitmap_F8BA[0], &stage2_bitmap_F8EE[0] },  // [31]
  { 2, BITMAPFLAG_MASKED, 3, &stage2_bitmap_F8BE[0], &stage2_bitmap_F8F2[0] },  // [32]
  { 2, BITMAPFLAG_MASKED, 3, &stage2_bitmap_F8CA[0], &stage2_bitmap_F8FE[0] },  // [33]
  { 2, BITMAPFLAG_MASKED, 2, &stage2_bitmap_F8D6[0], &stage2_bitmap_F90A[0] },  // [34]
};

/**
 * $F768: stage2_bitmap_F768
 */
static const pixel_t stage2_bitmap_F768[5 * 1 * 4 * 1] = {
  _X_X_X_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_X_X_,
  XXXXXXXX, XXXXXXXX, _X_XXXXX, XXXXXXXX, XXXXXXXX,
  __X__XXX, XXXXXXXX, _____XXX, XXXXXXXX, XX_X____,
  _______X, _X_X__XX, _XX_XX_X, _X_X_X__, ________,
};

/**
 * $F77C: stage2_bitmap_F77C
 */
static const pixel_t stage2_bitmap_F77C[4 * 1 * 11 * 1] = {
  ____X___, __X_XXX_, XXX_____, ____XXX_,
  ___X_X__, __XX__XX, X_XX____, __XXX_X_,
  ___X__XX, X__XXXXX, _X_XXXXX, XXX__XX_,
  ___XX_X_, XXXX_XX_, XX_XXX__, X__X_X__,
  _XXXXX__, X_XX_XX_, _X_XXXX_, X___XX__,
  X_XX_X_X, __XXXX_X, _XX_XXXX, XX_XXXXX,
  XX_XXXXX, __XXXX__, XX__XXXX, XXXXXX_X,
  _X__X_X_, _X_XXX__, _X_XXX_X, XXX_X_XX,
  _XXX___X, X__XX_X_, _XXXX_X_, _X___XX_,
  __X__X_X, XX_XX__X, XXX_X_X_, __XXXX__,
  __XXX__X, _X__X___, X___X__X, _XX_X___,
};

/**
 * $F7A8: stage2_bitmap_F7A8
 */
static const pixel_t stage2_bitmap_F7A8[5 * 1 * 8 * 1] = {
  _XXXX__X, _XXX_XX_, XX_XXX_X, X__XX_XX, X__X__X_,
  _XX__XXX, XXXXX__X, XXXXXXXX, __XXXX__, _XXX_X_X,
  __X__X_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X,
  __XXX___, X_X_X_XX, XX_XXXXX, X_X_XXXX, XXX_X_X_,
  ___XX__X, ___X__XX, X___XXXX, __X___X_, _X__XX__,
  ____XXX_, ___X__XX, X___X_XX, X__X__X_, __XX____,
  _____XXX, __X__XXX, _XXXX_XX, _XXXXX_X, _XX_____,
  _______X, XXX_XXXX, ___X_XXX, X___X_XX, X___XX__,
};

/**
 * $F7D0: stage2_bitmap_F7D0
 */
static const pixel_t stage2_bitmap_F7D0[4 * 1 * 6 * 1] = {
  _XXXXXXX, ___X_XXX, XXXXXXX_, __XX_X__,
  X_XXXXX_, X_X__XXX, _XXXXXXX, XX__XX__,
  X__XXX__, _XX_XXXX, __XX_XX_, _X__X___,
  _X_X__X_, XX_X___X, X__X__X_, __XX____,
  __XX_XXX, _XX_____, XX__X__X, _XX_____,
  ____XXXX, X_______, __XXXXXX, X_______,
};

/**
 * $F7E8: stage2_bitmap_F7E8
 */
static const pixel_t stage2_bitmap_F7E8[3 * 1 * 4 * 1] = {
  ____XXXX, XXXXXXXX, XXXX____,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  __XXXXXX, XX____XX, XXXXXXX_,
  ___XX_X_, XX_X_X_X, _X__X___,
};

/**
 * $F7F4: stage2_bitmap_F7F4
 */
static const pixel_t stage2_bitmap_F7F4[3 * 1 * 4 * 1] = {
  XX__XX_X, XXXX____, __XX____,
  X_XXX_XX, XX__XXXX, XX__X___,
  X_XXXX_X, X_X_XX__, X_X_X___,
  X___XXXX, X_X_XXX_, X__XX___,
};

/**
 * $F800: stage2_bitmap_F800
 */
static const pixel_t stage2_bitmap_F800[4 * 1 * 11 * 1] = {
  _____XXX, XX_X_XXX, _XXX_XXX, XXXX_X__,
  _____X_X, _XX___XX, __X_XX_X, XX__XX__,
  _XX__XX_, _X_XX_X_, XXXXX__X, __X_X___,
  X_XX_XXX, X_XX___X, __X_XX__, XX_X____,
  X__XXXX_, ___XX_XX, _X_XXXXX, __X_X___,
  XX__XXXX, ___XXXXX, XXXXXXXX, XXXX_X__,
  XX_X__XX, _XXXX_XX, XX_X_XXX, XX_XX___,
  _XX__X__, X_XXX__X, X__X__X_, X__X____,
  __XXX___, X_XXX__X, XXX_XXX_, _XX_____,
  ____XX_X, _XXX_XX_, XX____XX, X__XX___,
  _____XXX, XXXX__X_, XXXXXXX_, _XX_X___,
};

/**
 * $F82C: stage2_bitmap_F82C
 */
static const pixel_t stage2_bitmap_F82C[3 * 1 * 4 * 1] = {
  XXX_XX_X, XX_XXXXX, X__XX___,
  X_X_X_X_, _XX_X_X_, _X_X____,
  XXXX_X__, __X__X_X, __X_____,
  _XXXX___, ___XXXXX, XX______,
};

/**
 * $F838: stage2_bitmap_F838
 */
static const pixel_t stage2_bitmap_F838[2 * 1 * 2 * 1] = {
  XXXXXXXX, XXXXXX__,
  _XXX___X, XXX_X___,
};

/**
 * $F83C: stage2_bitmap_F83C
 */
static const pixel_t stage2_bitmap_F83C[2 * 1 * 5 * 1] = {
  __X__XXX, XX____XX,
  _X_XX_XX, __XXXX_X,
  XX_XXXXX, _XXXX_X_,
  _XX__XX_, X_XXXXX_,
  X_X_X_XX, __XXXX_X,
};

/**
 * $F846: stage2_bitmap_F846
 */
static const pixel_t stage2_bitmap_F846[3 * 1 * 4 * 1] = {
  _____XX_, X__X__X_, X_X___XX,
  _____X_X, XX__XXXX, _XX__XX_,
  ______X_, _XXXX_XX, XXXXXX_X,
  _______X, ___XX_XX, X__X__X_,
};

/**
 * $F854: stage2_bitmap_F854
 */
static const pixel_t stage2_bitmap_F854[2 * 1 * 4 * 1] = {
  XX_XX__X, X_X_XXX_,
  XXXX_X_X, _XXXX_X_,
  _X__X_X_, X__X_X__,
  __XXXX__, _XXXX___,
};

/**
 * $F85C: stage2_bitmap_F85C
 */
static const pixel_t stage2_bitmap_F85C[2 * 1 * 2 * 1] = {
  XXXXXXXX, XXXXX___,
  _XXX_X_X, XXXX____,
};

/**
 * $F860: stage2_bitmap_F860
 */
static const pixel_t stage2_bitmap_F860[2 * 1 * 4 * 1] = {
  __XXX_XX, _XXXXX__,
  _X_X_XXX, _XXXXX__,
  X_XXX_XX, X_XXXX__,
  _X_XX__X, _XXX_X__,
};

/**
 * $F868: stage2_bitmap_F868
 */
static const pixel_t stage2_bitmap_F868[2 * 1 * 4 * 1] = {
  XX__XX_X, XXX__X__,
  _XXXXXXX, X__XX_X_,
  X___X_XX, XXX_XXX_,
  _XXXX__X, X__XX_X_,
};

/**
 * $F870: stage2_bitmap_F870
 */
static const pixel_t stage2_bitmap_F870[2 * 1 * 2 * 1] = {
  _X_X__XX, _X___X__,
  __XXXX__, XXXXX___,
};

/**
 * $F874: stage2_bitmap_F874
 */
static const pixel_t stage2_bitmap_F874[1 * 2 * 3 * 1] = {
  ______XX, X____X__,
  ______XX, X_XXXX__,
  ______XX, XX___X__,
};

/**
 * $F87A: stage2_bitmap_F87A
 */
static const pixel_t stage2_bitmap_F87A[1 * 2 * 3 * 1] = {
  X_____XX, _X___X__,
  X_____XX, _X_XXX__,
  X_____XX, _XX__X__,
};

/**
 * $F880: stage2_bitmap_F880
 */
static const pixel_t stage2_bitmap_F880[1 * 2 * 3 * 1] = {
  XX_____X, __X___X_,
  XX_____X, __XX__X_,
  XX_____X, __X_XXX_,
};

/**
 * $F886: stage2_bitmap_F886
 */
static const pixel_t stage2_bitmap_F886[1 * 2 * 2 * 1] = {
  X_____XX, _X___X__,
  X_____XX, _X_XXX__,
};

/**
 * $F88A: stage2_bitmap_F88A
 */
static const pixel_t stage2_bitmap_F88A[1 * 2 * 2 * 1] = {
  XX____XX, __X_XX__,
  XX____XX, __XX_X__,
};

/**
 * $F88E: stage2_bitmap_F88E
 */
static const pixel_t stage2_bitmap_F88E[1 * 2 * 2 * 1] = {
  XXX____X, ___X_XX_,
  XXX____X, ___XX_X_,
};

/**
 * $F892: stage2_bitmap_F892
 */
static const pixel_t stage2_bitmap_F892[1 * 2 * 2 * 1] = {
  XXX____X, ___X__X_,
  XXX____X, ___X__X_,
};

/**
 * $F896: stage2_bitmap_F896
 */
static const pixel_t stage2_bitmap_F896[1 * 2 * 2 * 1] = {
  XXXX___X, ____X_X_,
  XXXX___X, ____X_X_,
};

/**
 * $F89A: stage2_bitmap_F89A
 */
static const pixel_t stage2_bitmap_F89A[1 * 2 * 2 * 1] = {
  XXXXX___, _____X_X,
  XXXXX___, _____X_X,
};

/**
 * $F89E: stage2_bitmap_F89E
 */
static const pixel_t stage2_bitmap_F89E[1 * 2 * 2 * 1] = {
  XXXX___X, ____X_X_,
  XXXX___X, ____X_X_,
};

/* $F8A2: bitmap_F8A2 - Conv: Deduped to CommonData.c (shared with stage 5) */

/**
 * $F8A6: stage2_bitmap_F8A6
 */
static const pixel_t stage2_bitmap_F8A6[1 * 2 * 2 * 1] = {
  XXXXXX__, ______XX,
  XXXXXX__, ______XX,
};

/**
 * $F8AA: stage2_bitmap_F8AA
 */
static const pixel_t stage2_bitmap_F8AA[2 * 2 * 1 * 1] = {
  ________, XXXXXXXX, ____XXXX, XXXX____,
};

/**
 * $F8AE: stage2_bitmap_F8AE
 */
static const pixel_t stage2_bitmap_F8AE[2 * 2 * 1 * 1] = {
  XXXX___X, ____XXX_, XXXXXXXX, ________,
};

/**
 * $F8B2: stage2_bitmap_F8B2
 */
static const pixel_t stage2_bitmap_F8B2[2 * 2 * 2 * 1] = {
  XXXXX__X, _____XX_, XXXXXXXX, ________,
  XXXXX__X, _____XX_, XXXXXXXX, ________,
};

/**
 * $F8BA: stage2_bitmap_F8BA
 */
static const pixel_t stage2_bitmap_F8BA[2 * 2 * 1 * 1] = {
  XXXXX___, _____XXX, XXXXXXXX, ________,
};

/**
 * $F8BE: stage2_bitmap_F8BE
 */
static const pixel_t stage2_bitmap_F8BE[2 * 2 * 3 * 1] = {
  XX______, __XXXXX_, ___XXXXX, XXX_____,
  XX______, __XX_XXX, ____XXXX, _XXX____,
  X_______, _X_XX_XX, ____XXXX, XXXX____,
};

/**
 * $F8CA: stage2_bitmap_F8CA
 */
static const pixel_t stage2_bitmap_F8CA[2 * 2 * 3 * 1] = {
  ________, XXX_X__X, ____XXXX, _X_X____,
  ________, X_XXXXXX, ____XXXX, XXX_____,
  X_______, _X__XX_X, ____XXXX, _X_X____,
};

/**
 * $F8D6: stage2_bitmap_F8D6
 */
static const pixel_t stage2_bitmap_F8D6[2 * 2 * 2 * 1] = {
  XX______, __XXX__X, ___XXXXX, __X_____,
  XXX_____, ___XXXXX, __XXXXXX, XX______,
};

/**
 * $F8DE: stage2_bitmap_F8DE
 */
static const pixel_t stage2_bitmap_F8DE[2 * 2 * 1 * 1] = {
  XXXX____, ____XXXX, ________, XXXXXXXX,
};

/**
 * $F8E2: stage2_bitmap_F8E2
 */
static const pixel_t stage2_bitmap_F8E2[2 * 2 * 1 * 1] = {
  XXXXXXXX, ________, ___XXXXX, XXX_____,
};

/**
 * $F8E6: stage2_bitmap_F8E6
 */
static const pixel_t stage2_bitmap_F8E6[2 * 2 * 2 * 1] = {
  XXXXXXXX, ________, X__XXXXX, _XX_____,
  XXXXXXXX, ________, X__XXXXX, _XX_____,
};

/**
 * $F8EE: stage2_bitmap_F8EE
 */
static const pixel_t stage2_bitmap_F8EE[2 * 2 * 1 * 1] = {
  XXXXXXXX, ________, X___XXXX, _XXX____,
};

/**
 * $F8F2: stage2_bitmap_F8F2
 */
static const pixel_t stage2_bitmap_F8F2[2 * 2 * 3 * 1] = {
  XXXXXX__, ______XX, _______X, XXX_XXX_,
  XXXXXX__, ______XX, ________, _XXX_XXX,
  XXXXX___, _____X_X, ________, X_XXXXXX,
};

/**
 * $F8FE: stage2_bitmap_F8FE
 */
static const pixel_t stage2_bitmap_F8FE[2 * 2 * 3 * 1] = {
  XXXX____, ____XXX_, ________, X__X_X_X,
  XXXX____, ____X_XX, ________, XXXXXXX_,
  XXXXX___, _____X__, ________, XX_X_X_X,
};

/**
 * $F90A: stage2_bitmap_F90A
 */
static const pixel_t stage2_bitmap_F90A[2 * 2 * 2 * 1] = {
  XXXXXX__, ______XX, _______X, X__X__X_,
  XXXXXXX_, _______X, ______XX, XXXXXX__,
};

/** $F912: stage2_stretchy_F912 */
static const stretchy_t stage2_stretchy_F912[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_right },
  { STRETCHY_TYPE_50PC, &streetlampmiddle2_right },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_right },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F92C },
  { STRETCHY_TYPE_END, NULL },
};

/** $F91F: stage2_stretchy_F91F */
static const stretchy_t stage2_stretchy_F91F[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_left },
  { STRETCHY_TYPE_50PC, &streetlampmiddle2_left },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_left },
  { STRETCHY_TYPE_FIXED, &stage2_depthset_F942 },
  { STRETCHY_TYPE_END, NULL },
};

/** $F92C: stage2_depthset_F92C */
static const depthset_t stage2_depthset_F92C = {
  &stage2_lods_F958[0],
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

/** $F942: stage2_depthset_F942 */
static const depthset_t stage2_depthset_F942 = {
  &stage2_lods_F958[0],
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

/** $F958: stage2_lods_F958 */
static const bitmap_t stage2_lods_F958[5] = {
  { 5, BITMAPFLAG_DEFAULT, 8, &bitmap_F97B[0], &bitmap_F97B[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 6, &bitmap_F9A3[0], &bitmap_F9A3[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 4, &bitmap_F9BB[0], &bitmap_F9C7[0] },  // [2]
  { 3, BITMAPFLAG_MASKED, 3, &bitmap_F9D3[0], &bitmap_F9E5[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 2, &stage2_bitmap_F9F7[0], &stage2_bitmap_FA03[0] },  // [4]
};

/* $F97B: bitmap_F97B - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F9A3: bitmap_F9A3 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F9BB: bitmap_F9BB - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F9C7: bitmap_F9C7 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F9D3: bitmap_F9D3 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $F9E5: bitmap_F9E5 - Conv: Deduped to CommonData.c (shared with stage 5) */

/**
 * $F9F7: stage2_bitmap_F9F7
 */
static const pixel_t stage2_bitmap_F9F7[2 * 2 * 2 + 4] = {
  X_XXXX_X, _X____X_, XXX_XXXX, ___X____,
  ______X_, X_XXXX_X, _____XXX, XXX_X___,
  // $F9FF: 4 further bytes, not reached by any LOD entry
  ___XXXXX, XXX_____, XX___XXX, __XXX___,
};

/**
 * $FA03: stage2_bitmap_FA03
 */
static const pixel_t stage2_bitmap_FA03[2 * 2 * 2 + 4] = {
  XXXXX_XX, _____X__, XX_XXX_X, __X___X_,
  XXXX____, ____X_XX, __X_____, XX_XXX_X,
  // $FA0B: 4 further bytes, not reached by any LOD entry
  XXXX___X, ____XXX_, XXXXXX__, ______XX,
};

/** $FA0F: stage2_lods_FA0F */
static const bitmap_t stage2_lods_FA0F[6] = {
  { 4, BITMAPFLAG_DEFAULT, 17, &bitmap_barrier_1[0], &bitmap_barrier_1[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 17, &bitmap_barrier_1[0], &bitmap_barrier_1[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 13, &bitmap_barrier_2[0], &bitmap_barrier_2[0] },  // [2]
  { 2, BITMAPFLAG_DEFAULT, 9, &bitmap_barrier_3[0], &bitmap_barrier_3[0] },  // [3]
  { 2, BITMAPFLAG_DEFAULT, 9, &bitmap_barrier_3[0], &bitmap_barrier_3[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 7, &bitmap_barrier_4[0], &bitmap_barrier_4s[0] },  // [5]
};

/* $FA39: bitmap_barrier_1 - Conv: Deduped to CommonData.c (shared with stage 1/3/4/5) */

/* $FA7D: bitmap_barrier_2 - Conv: Deduped to CommonData.c (shared with stage 1/3/4/5) */

/* $FAA4: bitmap_barrier_3 - Conv: Deduped to CommonData.c (shared with stage 1/3/4/5) */

/* $FAB6: bitmap_barrier_4 - Conv: Deduped to CommonData.c (shared with stage 1/3/4/5) */

/* $FAD2: bitmap_barrier_4s - Conv: Deduped to CommonData.c (shared with stage 1/3/4/5) */

const map_goto_entry_t stage2_map_goto_table[24] = {
  { STAGE2_MAP_CURV_E439_ADDR,    &stage2_map_curv_E439[0]    },
  { STAGE2_MAP_HEIGHT_E44F_ADDR,  &stage2_map_height_E44F[0]  },
  { STAGE2_MAP_LANES_E474_ADDR,   &stage2_map_lanes_E474[0]   },
  { STAGE2_MAP_HAZARDS_E488_ADDR, &stage2_map_hazards_E488[0] },
  { STAGE2_MAP_LOBJS_E4A8_ADDR,   &stage2_map_lobjs_E4A8[0]   },
  { STAGE2_MAP_ROBJS_E4F9_ADDR,   &stage2_map_robjs_E4F9[0]   },
  { STAGE2_MAP_CURV_E53C_ADDR,    &stage2_map_curv_E53C[0]    },
  { STAGE2_MAP_HEIGHT_E54F_ADDR,  &stage2_map_height_E54F[0]  },
  { STAGE2_MAP_LANES_E561_ADDR,   &stage2_map_lanes_E561[0]   },
  { STAGE2_MAP_HAZARDS_E57B_ADDR, &stage2_map_hazards_E57B[0] },
  { STAGE2_MAP_LOBJS_E592_ADDR,   &stage2_map_lobjs_E592[0]   },
  { STAGE2_MAP_ROBJS_E5D0_ADDR,   &stage2_map_robjs_E5D0[0]   },
  { STAGE2_MAP_CURV_E60B_ADDR,    &stage2_map_curv_E60B[0]    },
  { STAGE2_MAP_HEIGHT_E61E_ADDR,  &stage2_map_height_E61E[0]  },
  { STAGE2_MAP_LANES_E62F_ADDR,   &stage2_map_lanes_E62F[0]   },
  { STAGE2_MAP_HAZARDS_E643_ADDR, &stage2_map_hazards_E643[0] },
  { STAGE2_MAP_LOBJS_E648_ADDR,   &stage2_map_lobjs_E648[0]   },
  { STAGE2_MAP_ROBJS_E668_ADDR,   &stage2_map_robjs_E668[0]   },
  { STAGE2_MAP_CURV_E674_ADDR,    &stage2_map_curv_E674[0]    },
  { STAGE2_MAP_HEIGHT_E69C_ADDR,  &stage2_map_height_E69C[0]  },
  { STAGE2_MAP_LANES_E6E0_ADDR,   &stage2_map_lanes_E6E0[0]   },
  { STAGE2_MAP_HAZARDS_E6F0_ADDR, &stage2_map_hazards_E6F0[0] },
  { STAGE2_MAP_LOBJS_E713_ADDR,   &stage2_map_lobjs_E713[0]   },
  { STAGE2_MAP_ROBJS_E79B_ADDR,   &stage2_map_robjs_E79B[0]   },
};

// clang-format on
