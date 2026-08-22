/**
 * Stage1Data.c
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

#include "Stage1Data.h"

/* Z80 addresses of the map sections, as referenced by the map
 * GOTO/SPLIT commands and stage1_map_goto_table[]. */
#define STAGE1_MAP_LEFT_CURVATURE_ADDR   (0x5FD2)
#define STAGE1_MAP_LEFT_HEIGHT_ADDR      (0x5FE6)
#define STAGE1_MAP_LEFT_LANES_ADDR       (0x6003)
#define STAGE1_MAP_LEFT_HAZARDS_ADDR     (0x6009)
#define STAGE1_MAP_LEFT_LEFTOBJS_ADDR    (0x6011)
#define STAGE1_MAP_LEFT_RIGHTOBJS_ADDR   (0x6052)
#define STAGE1_MAP_RIGHT_CURVATURE_ADDR  (0x6088)
#define STAGE1_MAP_RIGHT_HEIGHT_ADDR     (0x60A3)
#define STAGE1_MAP_RIGHT_LANES_ADDR      (0x60E5)
#define STAGE1_MAP_RIGHT_HAZARDS_ADDR    (0x60EF)
#define STAGE1_MAP_RIGHT_LEFTOBJS_ADDR   (0x6109)
#define STAGE1_MAP_RIGHT_RIGHTOBJS_ADDR  (0x6143)
#define STAGE1_MAP_TUNNEL_CURVATURE_ADDR (0x6173)
#define STAGE1_MAP_TUNNEL_HEIGHT_ADDR    (0x6189)
#define STAGE1_MAP_TUNNEL_LANES_ADDR     (0x61A5)
#define STAGE1_MAP_TUNNEL_HAZARDS_ADDR   (0x61B9)
#define STAGE1_MAP_TUNNEL_LEFTOBJS_ADDR  (0x61BE)
#define STAGE1_MAP_TUNNEL_RIGHTOBJS_ADDR (0x61C9)
#define STAGE1_MAP_LOOP_CURVATURE_ADDR   (0x61D4)
#define STAGE1_MAP_LOOP_HEIGHT_ADDR      (0x6209)
#define STAGE1_MAP_LOOP_LANES_ADDR       (0x625D)
#define STAGE1_MAP_LOOP_HAZARDS_ADDR     (0x6277)
#define STAGE1_MAP_LOOP_LEFTOBJS_ADDR    (0x6294)
#define STAGE1_MAP_LOOP_RIGHTOBJS_ADDR   (0x6314)

/* ----------------------------------------------------------------------- */

/* Stage 1 object type macros */

#define MAP_OBJ_S1_TREE_VAL           (4)
#define MAP_OBJ_S1_BUSH_VAL           (5)
#define MAP_OBJ_S1_STREET_LAMP_VAL    (6)
#define MAP_OBJ_S1_TELEGRAPH_POLE_VAL (7)
#define MAP_OBJ_S1_TURN_SIGN_L_VAL    (8)
#define MAP_OBJ_S1_TURN_SIGN_R_VAL    (9)

#define MAP_OBJ_S1_TREE(D)            ((D << 4) | MAP_OBJ_S1_TREE_VAL)
#define MAP_OBJ_S1_BUSH(D)            ((D << 4) | MAP_OBJ_S1_BUSH_VAL)
#define MAP_OBJ_S1_STREET_LAMP(D)     ((D << 4) | MAP_OBJ_S1_STREET_LAMP_VAL)
#define MAP_OBJ_S1_TELEGRAPH_POLE(D)  ((D << 4) | MAP_OBJ_S1_TELEGRAPH_POLE_VAL)
#define MAP_OBJ_S1_TURN_SIGN_L(D)     ((D << 4) | MAP_OBJ_S1_TURN_SIGN_L_VAL)
#define MAP_OBJ_S1_TURN_SIGN_R(D)     ((D << 4) | MAP_OBJ_S1_TURN_SIGN_R_VAL)

/* ----------------------------------------------------------------------- */

/* Forward declarations */

static const char *stage1_chatter_strings[4];
static const u8 stage1_arrest_messages[83];
static const hittable_t stage1_hittable_object_defs[2];
static const obj_t stage1_right_hand_graphics_defs[9];
static const obj_t stage1_left_hand_graphics_defs[9];
static const u8 stage1_map_start_curvature[32];
static const u8 stage1_map_start_height[43];
static const u8 stage1_map_start_lanes[14];
static const u8 stage1_map_start_hazards[13];
static const u8 stage1_map_start_leftobjs[78];
static const u8 stage1_map_start_rightobjs[90];
static const u8 stage1_map_left_curvature[20];
static const u8 stage1_map_left_height[29];
static const u8 stage1_map_left_lanes[6];
static const u8 stage1_map_left_hazards[8];
static const u8 stage1_map_left_leftobjs[65];
static const u8 stage1_map_left_rightobjs[54];
static const u8 stage1_map_right_curvature[27];
static const u8 stage1_map_right_height[66];
static const u8 stage1_map_right_lanes[10];
static const u8 stage1_map_right_hazards[26];
static const u8 stage1_map_right_leftobjs[58];
static const u8 stage1_map_right_rightobjs[48];
static const u8 stage1_map_tunnel_curvature[22];
static const u8 stage1_map_tunnel_height[28];
static const u8 stage1_map_tunnel_lanes[20];
static const u8 stage1_map_tunnel_hazards[5];
static const u8 stage1_map_tunnel_leftobjs[11];
static const u8 stage1_map_tunnel_rightobjs[11];
static const u8 stage1_map_loop_curvature[53];
static const u8 stage1_map_loop_height[84];
static const u8 stage1_map_loop_lanes[26];
static const u8 stage1_map_loop_hazards[29];
static const u8 stage1_map_loop_leftobjs[128];
static const u8 stage1_map_loop_rightobjs[118];
static const pixel_t stage1_perp_face[FACEBYTES];
static const bitmap_t stage1_lambo_bitmaps[SPRITE_FRAMES];
static const bitmap_t stage1_truck_bitmaps[SPRITE_FRAMES];
static const bitmap_t stage1_car_bitmaps[SPRITE_FRAMES];
static const bitmap_t stage1_stones_bitmaps[SPRITE_FRAMES];
static const bitmap_t stage1_dust_bitmaps[SPRITE_FRAMES];
static const depthset_t stage1_turn_sign_right;
static const depthset_t stage1_turn_sign_left;
static const bitmap_t stage1_turn_sign_bitmaps[10];
static const pixel_t stage1_bitmap_turnsign_5s[2 * 2 * 13 * 1];
static const bitmap_t stage1_tumbleweed_bitmaps[SPRITE_FRAMES];
static const bitmap_t stage1_barrier_bitmaps[SPRITE_FRAMES];
static const stretchy_t stage1_stretchy_streetlamp_right[5];
static const stretchy_t stage1_stretchy_streetlamp_left[5];
static const depthset_t stage1_streetlamptop_right;
static const depthset_t stage1_streetlamptop_left;
static const bitmap_t stage1_streetlamptop_bitmaps[5];
static const bitmap_t stage1_streetlamptop_flipped_bitmaps[5];
static const stretchy_t stage1_stretchy_telegraphpole_right[5];
static const stretchy_t stage1_stretchy_telegraphpole_left[5];
static const depthset_t stage1_telegraphpoletop_left;
static const depthset_t stage1_telegraphpoletop_right;
static const bitmap_t stage1_telegraphpoletop_bitmaps[5];
static const stretchy_t stage1_stretchy_tree[SPRITE_FRAMES];
static const stretchy_t stage1_stretchy_bush[4];
static const depthset_t stage1_tree_bottom;
static const depthset_t stage1_tree_bottommiddle;
static const depthset_t stage1_tree_middle;
static const depthset_t stage1_tree_topmiddle;
static const depthset_t stage1_tree_top;
static const bitmap_t stage1_tree_bitmaps[25];
static const pixel_t stage1_bitmap_tree_middle_64x16[8 * 1 * 16 * 1];
static const pixel_t stage1_bitmap_tree_bottom_64x5[8 * 1 * 5 * 1];
static const pixel_t stage1_bitmap_tree_trunk_16x8[2 * 1 * 8 * 1];
static const pixel_t stage1_bitmap_tree_shadow_64x5[8 * 1 * 5 * 1];
static const pixel_t stage1_bitmap_tree_middle_48x12[6 * 1 * 12 * 1];
static const pixel_t stage1_bitmap_tree_bottom_48x4[6 * 1 * 4 * 1];
static const pixel_t stage1_bitmap_tree_shadow_48x4[6 * 1 * 4 * 1];
static const pixel_t stage1_bitmap_tree_middle_32x8[4 * 1 * 8 * 1];
static const pixel_t stage1_bitmap_tree_bottom_32x3[4 * 1 * 3 * 1];
static const pixel_t stage1_bitmap_tree_shadow_32x2[4 * 1 * 2 * 1];
static const pixel_t stage1_bitmap_tree_middle_24x7[3 * 1 * 7 * 1];
static const pixel_t stage1_bitmap_tree_bottom_24x2[3 * 1 * 2 * 1];
static const pixel_t stage1_bitmap_tree_trunk_8x4[1 * 1 * 4 * 1];
static const pixel_t stage1_bitmap_tree_shadow_24x2[3 * 1 * 2 * 1];
static const pixel_t stage1_bitmap_tree_top_64x13[8 * 2 * 13 * 1];
static const pixel_t stage1_bitmap_tree_top_48x10[6 * 2 * 10 * 1];
static const pixel_t stage1_bitmap_tree_top_32x5[4 * 2 * 5 * 1];
static const pixel_t stage1_bitmap_tree_top_24x4[3 * 2 * 4 * 1];
static const pixel_t stage1_bitmap_tree_top_24x3[3 * 2 * 3 * 1];
static const pixel_t stage1_bitmap_tree_top_24x3s[3 * 2 * 3 * 1];
static const pixel_t stage1_bitmap_tree_trunk_16x6[2 * 2 * 6 * 1];
static const pixel_t stage1_bitmap_tree_trunk_16x4[2 * 2 * 4 * 1];
static const pixel_t stage1_bitmap_tree_middle_24x5[3 * 2 * 5 * 1];
static const pixel_t stage1_bitmap_tree_bottom_24x2_another[3 * 2 * 2 * 1];
static const pixel_t stage1_bitmap_tree_trunk_24x3[3 * 2 * 3 * 1];
static const pixel_t stage1_bitmap_tree_shadow_24x1[3 * 2 * 1 * 1];
static const pixel_t stage1_bitmap_tree_middle_24x5s[3 * 2 * 5 * 1];
static const pixel_t stage1_bitmap_tree_bottom_24x2s[3 * 2 * 2 * 1];
static const pixel_t stage1_bitmap_tree_trunk_24x3s[3 * 2 * 3 * 1];
static const pixel_t stage1_bitmap_tree_shadow_24x1s[3 * 2 * 1 * 1];

/* ----------------------------------------------------------------------- */

/** $5CF0: stage1 */
// clang-format off
const stage_t stage1 = {
  {
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXX____, __X__X_X, _X_XXX_X, _XXXXXXX, _X_XXXXX, XXXXXXXX, XXXXXXXX, X_X_XXXX, ____XXX_, X_X__X_X,
    X_XXX___, ____X___, __X_X_XX, X_XXX_X_, X____XXX, XX_X_XXX, XXXX_X_X, _X_X_X_X, XXX___XX, ______X_,
    _X_X_XXX, ______X_, ___X_X_X, XXXXXX__, ______X_, X_X_X_XX, X_X_X___, ______X_, X_XX____, X_______,
    ____X_XX, X_______, X_XXXXXX, XXX_XXX_, _______X, _X_X_XX_, __XXX___, ________, _X_XX___, ________,
    _____XXX, _X_X___X, _XXXXX__, XX_X_XX_, ______X_, X_X_XX__, XXX___X_, X_X_____, ____XXX_, ____X___,
    ____X_X_, X_X_XXXX, XXXX____, _XX_X__X, _X_____X, _X_X___X, X____X_X, _X_XX___, ______XX, X_____X_,
    _______X, XX_XXXXX, XX______, __XX_X__, ____X_X_, ______X_, __X_X_X_, XXXXXXXX, ________, XXX_____,
    X_______, X_XXX_X_, XXXXX___, ____XXX_, ________, _XX_X___, _X_X_X_X, XXXXXXXX, X_______, ___X____,
    _X_X__XX, XX___X__, _X_X_XXX, X_______, ______XX, XX_X__X_, X_X_XXXX, XXXXXXXX, XXXX____, ________,
    XXXXXXX_, ________, X___X_X_, XXXXX___, ____XXX_, X_X__X_X, _XXXXXXX, XXXXXXXX, XXXXX_X_, _____X__,
    XXXXXXXX, _X______, _______X, _X_XXX_X, _X___XXX, ____X___, XXXXXXXX, _X_XXX_X, XX_XXX_X, _XXXXXXX,
    XXXXXXXX, XXX_____, ____X_X_, __X___XX, XXX____X, XX____XX, XXXXX_X_, X_X_X_X_, X_X_X_X_, X_X_XXXX,
    _X_X_XXX, XXXX_X__, _______X, _____X__, _XXX_X__, _____XXX, XXXX_X_X, _X_X_X_X, ___X___X, _X_X_X_X,
    X_X_X_X_, XXXXXXX_, X_______, ________, X_X_XXXX, X_XXXXXX, X_X_X_X_, ________, ______X_, __X_X_X_,
    _X_X_XXX, _XXXXXXX, XX_X____, ________, ___X_X_X, XXXXXX_X, _XXXXXXX, XX_X_X__, ________, _X___X_X,
    _X__X_X_, XXXXXXXX, XXXXX_X_, X_X_____, __X_X_X_, X_X_X_XX, XXXXX___, ______X_, X_X_X___, ____X___,
    X__X_X_X, XXXXXXXX, XXXXXX__, ___X_X__, ___X___X, _XXXXXXX, X_______, ________, _____X__, ________,
    _______X, _XXXXXXX, XXX_____, ______X_, ________, _XXXX___, ________, ________, ______X_, X_______,
    ____X_X_, XXXXXXXX, ________, _______X, _X______, XX______, ________, ________, ________, _X_X____,
    _______X, XXXXXXX_, ________, ________, __X_X_XX, ________, ________, ________, ________, ____X___,
    ______X_, X_XX____, ________, ________, ________, ________, ________, ________, ________, _____X_X,
    X_X____X, XXX_____, ________, ________, ________, ________, ________, ________, ________, ________,
    ___X_XXX, ________, ________, ________, ________, ________, ________, ________, ________, ________
  },
  &stage1_perp_face[FACEBITMAPBYTES],
  NULL,  /* no pilot mugshot on this stage */
  attribute_BRIGHT_BLACK_OVER_YELLOW * 0x0101,
  &stage1_hittable_object_defs[0],
  &stage1_right_hand_graphics_defs[-1].arg,
  &stage1_right_hand_graphics_defs[-1],
  &stage1_right_hand_graphics_defs[2], // short pole
  &stage1_left_hand_graphics_defs[-1].arg,
  &stage1_left_hand_graphics_defs[-1],
  &stage1_left_hand_graphics_defs[2], // short pole
  &perp_description[0],
  &stage1_arrest_messages[0],
  { NULL, NULL }, // unused on this level - addrof_helicopter_frames - Conv: was &000C

  &stage1_stones_bitmaps,
  &stage1_dust_bitmaps,
  &stage1_car_bitmaps[0],
  {
    &stage1_lambo_bitmaps[0],
    &stage1_truck_bitmaps[0],
    &stage1_lambo_bitmaps[0],
    &stage1_car_bitmaps[0]
  },

  20,  /* car_spawn_delay */
  80,  /* perp_lane_change_base */
  90,  /* perp_approach_base */

  {
    234,
    &stage1_map_start_curvature[-1],
    &stage1_map_start_height[-1],
    &stage1_map_start_lanes[-1],
    &stage1_map_start_rightobjs[-1],
    &stage1_map_start_leftobjs[-1],
    &stage1_map_start_hazards[-1]
  },

  {
    234,
    &stage1_map_loop_curvature[-1],
    &stage1_map_loop_height[-1],
    &stage1_map_loop_lanes[-1],
    &stage1_map_loop_rightobjs[-1],
    &stage1_map_loop_leftobjs[-1],
    &stage1_map_loop_hazards[-1]
  },

  stage1_chatter_strings
};

/* ----------------------------------------------------------------------- */

/* $5D39: perp_description - Conv: Deduped to CommonData.c (shared with stage 2/3/4/5) */

/**
 * $C145 (bank 1)
 * Conv: Additional
 */
static const char *stage1_chatter_strings[4] = {
  "THIS IS NANCY AT CHASE H.Q. WE'VE GOT A\xCE",
  "EMERGENCY HERE. RALPH THE IDAHO SLASHER\xAC",
  "IS FLEEING TOWARDS THE SUBURBS. THE TARGE\xD4",
  "VEHICLE IS A WHITE BRITISH SPORTS CAR... OVER\xAE"
};

/* ----------------------------------------------------------------------- */

/** $5DED: stage1_arrest_messages */
static const u8 stage1_arrest_messages[83] = {
  6, // initial delay

  6, // delay
  DRAWCHARSTYLE_SINGLE,
  0, // attribute: black on black, revealed by the FILL_ATTRIBUTES transition
  CHQBACKBUF(0xF802), // (16,8)
  ZXATTRS(0x5922), // (2,9)
  'O', 'K', '!', ' ', 'Y', 'O', 'U', ' ', 'A', 'R', 'E', ' ', 'U', 'N', 'D', 'E', 'R', ' ', 'A', 'R', 'R', 'E', 'S', 'T', ' ', 'O', 'N' | EOS,

  6, // delay
  DRAWCHARSTYLE_SINGLE,
  0, // attribute: black on black, revealed by the FILL_ATTRIBUTES transition
  CHQBACKBUF(0xF023), // (24,16)
  ZXATTRS(0x5943), // (3,10)
  'S', 'U', 'S', 'P', 'I', 'C', 'I', 'O', 'N', ' ', 'O', 'F', ' ', 'F', 'I', 'R', 'S', 'T', ' ', 'D', 'E', 'G', 'R', 'E', 'E', ' ' | EOS,

  45, // delay
  DRAWCHARSTYLE_SINGLE,
  0, // attribute: black on black, revealed by the FILL_ATTRIBUTES transition
  CHQBACKBUF(0xF82C), // (96,24)
  ZXATTRS(0x596C), // (12,11)
  'M', 'U', 'R', 'D', 'E', 'R' | EOS,

  TRANSITIONCONTROL_FILL_ATTRIBUTES,
  DRAWOVERLAY_STOP
};

/* ----------------------------------------------------------------------- */

/** $5E40: stage1_hittable_object_defs */
static const hittable_t stage1_hittable_object_defs[2] = {
  { 16, &stage1_tumbleweed_bitmaps[0] },
  { 32, &stage1_barrier_bitmaps[0]    },
};

/** $5E46: stage1_right_hand_graphics_defs */
static const obj_t stage1_right_hand_graphics_defs[9] = {
  { 111, 41, 80, &tunnellight,                         draw_tunnel_light_right    },
  {   0,  0,  0, NULL,                                 NULL                       },
  { 144, 92, 40, &stretchy_shortpole,                  draw_stretchy_object_right },
  {  94, 36, 60, &stage1_stretchy_tree,                draw_stretchy_object_right },
  { 110, 49, 80, &stage1_stretchy_bush,                draw_stretchy_object_right },
  { 110, 53, 80, &stage1_stretchy_streetlamp_right,    draw_stretchy_object_right },
  { 110, 53, 80, &stage1_stretchy_telegraphpole_right, draw_stretchy_object_right },
  { 100, 24, 70, &stage1_turn_sign_left,               draw_object_right          },
  { 100, 24, 70, &stage1_turn_sign_right,              draw_object_right          }
};

/** $5E85: stage1_left_hand_graphics_defs */
static const obj_t stage1_left_hand_graphics_defs[9] = {
  { 126, 188, 80, &tunnellight,                        draw_tunnel_light_left    },
  {   0,  0,  0,  NULL,                                NULL                      },
  {  96, 144, 40, &stretchy_shortpole,                 draw_stretchy_object_left },
  { 140, 196, 60, &stage1_stretchy_tree,               draw_stretchy_object_left },
  { 124, 208, 80, &stage1_stretchy_bush,               draw_stretchy_object_left },
  { 132, 182, 80, &stage1_stretchy_streetlamp_left,    draw_stretchy_object_left },
  { 132, 182, 80, &stage1_stretchy_telegraphpole_left, draw_stretchy_object_left },
  { 129, 182, 70, &stage1_turn_sign_left,              draw_object_left          },
  { 129, 200, 70, &stage1_turn_sign_right,             draw_object_left          },
};

/* ----------------------------------------------------------------------- */

/** $5EC4: stage1_map_start_curvature */
static const u8 stage1_map_start_curvature[32] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(14),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(5),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_LEFT_HARD(14),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(1),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(1),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(9),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(5),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(10),
  MAP_CURVE_RIGHT(11),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(10),
  MAP_CMD_SPLIT(STAGE1_MAP_LEFT_CURVATURE_ADDR, STAGE1_MAP_RIGHT_CURVATURE_ADDR)
};

/** 0x5EE4: stage1_map_start_height */
static const u8 stage1_map_start_height[43] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(7),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_UP3(13),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_DOWN3(15),
  MAP_HEIGHT_DOWN3(9),
  MAP_HEIGHT_DOWN1(3),
  MAP_HEIGHT_LEVEL(2),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_UP5(8),
  MAP_HEIGHT_DOWN5(8),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_LEVEL(2),
  MAP_HEIGHT_UP1(3),
  MAP_HEIGHT_UP3(7),
  MAP_HEIGHT_UP5(8),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_UP1(2),
  MAP_HEIGHT_LEVEL(6),
  MAP_HEIGHT_DOWN1(2),
  MAP_HEIGHT_DOWN3(14),
  MAP_HEIGHT_DOWN1(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(6),
  MAP_CMD_SPLIT(STAGE1_MAP_LEFT_HEIGHT_ADDR, STAGE1_MAP_RIGHT_HEIGHT_ADDR)
};

/** 0x5F0F: stage1_map_start_lanes */
static const u8 stage1_map_start_lanes[14] = {
  MAP_LANES_3L(30),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(254),
  MAP_LANES_4(34),
  MAP_CMD_SPLIT(STAGE1_MAP_LEFT_LANES_ADDR, STAGE1_MAP_RIGHT_LANES_ADDR)
};

/** 0x5F1D: stage1_map_start_hazards */
static const u8 stage1_map_start_hazards[13] = {
  MAP_HAZARD_WAIT(147),
  MAP_CMD_STOP_CARS,
  MAP_HAZARD_WAIT(12),
  MAP_CMD_ARROW_R,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_SPLIT(STAGE1_MAP_LEFT_HAZARDS_ADDR, STAGE1_MAP_RIGHT_HAZARDS_ADDR)
};

/** 0x5F2A: stage1_map_start_leftobjs */
static const u8 stage1_map_start_leftobjs[78] = {
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(11),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_CMD_SPLIT(STAGE1_MAP_LEFT_LEFTOBJS_ADDR, STAGE1_MAP_RIGHT_LEFTOBJS_ADDR)
};

/** 0x5F78: stage1_map_start_rightobjs */
static const u8 stage1_map_start_rightobjs[90] = {
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_CMD_SPLIT(STAGE1_MAP_LEFT_RIGHTOBJS_ADDR, STAGE1_MAP_RIGHT_RIGHTOBJS_ADDR)
};

/* ----------------------------------------------------------------------- */

/** $5FD2: stage1_map_left_curvature */
static const u8 stage1_map_left_curvature[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(6),
  MAP_CURVE_RIGHT(12),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(2),
  MAP_CURVE_RIGHT_VERY_HARD(15),
  MAP_CURVE_RIGHT_VERY_HARD(15),
  MAP_CURVE_RIGHT_HARD(12),
  MAP_CURVE_RIGHT(14),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(14),
  MAP_CMD_GOTO(STAGE1_MAP_TUNNEL_CURVATURE_ADDR) // map_tunnel_curvature
};

/** 0x5FE6: stage1_map_left_height */
static const u8 stage1_map_left_height[] = {
  MAP_HEIGHT_LEVEL(13),
  MAP_HEIGHT_UP1(4),
  MAP_HEIGHT_UP3(4),
  MAP_HEIGHT_UP5(6),
  MAP_HEIGHT_UP7(11),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_UP3(3),
  MAP_HEIGHT_UP1(2),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(7),
  MAP_HEIGHT_DOWN1(3),
  MAP_HEIGHT_DOWN3(4),
  MAP_HEIGHT_DOWN5(6),
  MAP_HEIGHT_DOWN7(7),
  MAP_HEIGHT_DOWN5(7),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_DOWN1(3),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(3),
  MAP_CMD_GOTO(STAGE1_MAP_TUNNEL_HEIGHT_ADDR) // map_tunnel_height
};

/** 0x6003: stage1_map_left_lanes */
static const u8 stage1_map_left_lanes[] = {
  MAP_LANES_4(210),
  MAP_CMD_GOTO(STAGE1_MAP_TUNNEL_LANES_ADDR) // map_tunnel_lanes
};

/** 0x6009: stage1_map_left_hazards */
static const u8 stage1_map_left_hazards[] = {
  MAP_HAZARD_WAIT(10),
  MAP_CMD_START_CARS,
  MAP_HAZARD_WAIT(95),
  MAP_CMD_GOTO(STAGE1_MAP_TUNNEL_HAZARDS_ADDR) // map_tunnel_hazards
};

/** 0x6011: stage1_map_left_leftobjs */
static const u8 stage1_map_left_leftobjs[] = {
  MAP_OBJ_NONE(4),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(2),
  MAP_CMD_GOTO(STAGE1_MAP_TUNNEL_LEFTOBJS_ADDR)
};

/** 0x6052: stage1_map_left_rightobjs */
static const u8 stage1_map_left_rightobjs[] = {
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(10),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_CMD_GOTO(STAGE1_MAP_TUNNEL_RIGHTOBJS_ADDR)
};

/* ----------------------------------------------------------------------- */

/** 0x6088: stage1_map_right_curvature */
static const u8 stage1_map_right_curvature[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(8),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(2),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(11),
  MAP_CURVE_LEFT(6),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CURVE_RIGHT(7),
  MAP_CURVE_RIGHT_HARD(11),
  MAP_CURVE_RIGHT(4),
  MAP_CURVE_LEFT(4),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(4),
  MAP_CURVE_LEFT_VERY_HARD(15),
  MAP_CURVE_LEFT_HARD(8),
  MAP_CURVE_RIGHT_HARD(9),
  MAP_CURVE_RIGHT_VERY_HARD(7),
  MAP_CURVE_RIGHT(2),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(2),
  MAP_CMD_GOTO(STAGE1_MAP_TUNNEL_CURVATURE_ADDR)
};

/** $60A3: stage1_map_right_height */
static const u8 stage1_map_right_height[] = {
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
  MAP_CMD_GOTO(STAGE1_MAP_TUNNEL_HEIGHT_ADDR)
};

/** $60E5: stage1_map_right_lanes */
static const u8 stage1_map_right_lanes[] = {
  MAP_LANES_4(20),
  MAP_LANES_DIRTTRACK(178),
  MAP_LANES_4(12),
  MAP_CMD_GOTO(STAGE1_MAP_TUNNEL_LANES_ADDR)
};

/** $60EF: stage1_map_right_hazards */
static const u8 stage1_map_right_hazards[] = {
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
  MAP_CMD_GOTO(STAGE1_MAP_TUNNEL_HAZARDS_ADDR)
};

/** $6109: stage1_map_right_leftobjs */
static const u8 stage1_map_right_leftobjs[] = {
  MAP_OBJ_NONE(10),
  MAP_OBJ_SHORT_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(13),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_CMD_GOTO(STAGE1_MAP_TUNNEL_LEFTOBJS_ADDR)
};

/** $6143: stage1_map_right_rightobjs */
static const u8 stage1_map_right_rightobjs[] = {
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_SHORT_POLE(1),
  MAP_OBJ_NONE(11),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(13),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(8),
  MAP_CMD_GOTO(STAGE1_MAP_TUNNEL_RIGHTOBJS_ADDR)
};

/* ----------------------------------------------------------------------- */

/** $6173: stage1_map_tunnel_curvature */
static const u8 stage1_map_tunnel_curvature[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CURVE_LEFT_HARD(12),
  MAP_CURVE_RIGHT(1),
  MAP_CURVE_RIGHT_HARD(10),
  MAP_CURVE_RIGHT(6),
  MAP_CURVE_STRAIGHT(8),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(1),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(10),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(11),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(6),
  MAP_CMD_GOTO(STAGE1_MAP_LOOP_CURVATURE_ADDR)
};

/** $6189: stage1_map_tunnel_height */
static const u8 stage1_map_tunnel_height[] = {
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
  MAP_HEIGHT_LEVEL(11),
  MAP_CMD_GOTO(STAGE1_MAP_LOOP_HEIGHT_ADDR)
};

/** $61A5: stage1_map_tunnel_lanes */
static const u8 stage1_map_tunnel_lanes[] = {
  MAP_LANES_4(44),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(12),
  MAP_LANES_TUNNEL_ENTRY(102),
  MAP_LANES_TUNNEL_EXIT(2),
  MAP_LANES_3L(18),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(8),
  MAP_CMD_GOTO(STAGE1_MAP_LOOP_LANES_ADDR)
};

/** $61B9: stage1_map_tunnel_hazards */
static const u8 stage1_map_tunnel_hazards[] = {
  MAP_HAZARD_WAIT(95),
  MAP_CMD_GOTO(STAGE1_MAP_LOOP_HAZARDS_ADDR)
};

/** $61BE: stage1_map_tunnel_leftobjs */
static const u8 stage1_map_tunnel_leftobjs[] = {
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(6),
  MAP_OBJ_NONE(14),
  MAP_CMD_GOTO(STAGE1_MAP_LOOP_LEFTOBJS_ADDR)
};

/** $61C9: stage1_map_tunnel_rightobjs */
static const u8 stage1_map_tunnel_rightobjs[] = {
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(6),
  MAP_OBJ_NONE(14),
  MAP_CMD_GOTO(STAGE1_MAP_LOOP_RIGHTOBJS_ADDR)
};

/* ----------------------------------------------------------------------- */

/** $61D4: stage1_map_loop_curvature */
static const u8 stage1_map_loop_curvature[] = {
  MAP_CURVE_STRAIGHT(3),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(8),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(5),
  MAP_CURVE_STRAIGHT(9),
  MAP_CURVE_LEFT(11),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(5),
  MAP_CURVE_RIGHT_HARD(11),
  MAP_CURVE_STRAIGHT(1),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(8),
  MAP_CURVE_RIGHT(9),
  MAP_CURVE_LEFT(9),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(7),
  MAP_CURVE_STRAIGHT(1),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(3),
  MAP_CURVE_LEFT(12),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(2),
  MAP_CURVE_LEFT_HARD(5),
  MAP_CURVE_LEFT_VERY_HARD(2),
  MAP_CURVE_LEFT_HARD(2),
  MAP_CURVE_LEFT(2),
  MAP_CURVE_RIGHT(2),
  MAP_CURVE_RIGHT_HARD(2),
  MAP_CURVE_RIGHT_VERY_HARD(10),
  MAP_CURVE_RIGHT_HARD(5),
  MAP_CURVE_RIGHT(10),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(3),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(4),
  MAP_CURVE_RIGHT_HARD(12),
  MAP_CURVE_RIGHT(6),
  MAP_CURVE_LEFT(5),
  MAP_CURVE_STRAIGHT(6),
  MAP_CMD_GOTO(STAGE1_MAP_LOOP_CURVATURE_ADDR)
};

/** $6209: stage1_map_loop_height */
static const u8 stage1_map_loop_height[] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(10),
  MAP_HEIGHT_UP3(4),
  MAP_HEIGHT_UP5(10),
  MAP_HEIGHT_DOWN5(12),
  MAP_HEIGHT_DOWN3(4),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(6),
  MAP_HEIGHT_UP1(4),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_UP5(15),
  MAP_HEIGHT_UP5(3),
  MAP_HEIGHT_UP7(15),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN7(6),
  MAP_HEIGHT_DOWN5(15),
  MAP_HEIGHT_DOWN5(7),
  MAP_HEIGHT_DOWN3(15),
  MAP_HEIGHT_DOWN3(15),
  MAP_HEIGHT_DOWN3(15),
  MAP_HEIGHT_DOWN1(4),
  MAP_HEIGHT_LEVEL(3),
  MAP_HEIGHT_UP1(5),
  MAP_HEIGHT_UP3(4),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(1),
  MAP_HEIGHT_UP5(12),
  MAP_HEIGHT_UP7(2),
  MAP_HEIGHT_DOWN7(6),
  MAP_HEIGHT_DOWN5(3),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_LEVEL(2),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_UP5(3),
  MAP_HEIGHT_UP7(5),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_DOWN5(3),
  MAP_HEIGHT_DOWN3(5),
  MAP_HEIGHT_DOWN1(2),
  MAP_HEIGHT_LEVEL(14),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_DOWN5(2),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_LEVEL(10),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN5(2),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN1(1),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP1(1),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(7),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_LEVEL(3),
  MAP_CMD_GOTO(STAGE1_MAP_LOOP_HEIGHT_ADDR)
};

/** $625D: stage1_map_loop_lanes */
static const u8 stage1_map_loop_lanes[] = {
  MAP_LANES_4(254),
  MAP_LANES_4(36),
  MAP_LANES_4TO3R(34),
  MAP_LANES_3RTO4(2),
  MAP_LANES_4(30),
  MAP_LANES_3L(10),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(22),
  MAP_LANES_4TO3R(6),
  MAP_LANES_3RTO4(2),
  MAP_LANES_4(52),
  MAP_CMD_GOTO(STAGE1_MAP_LOOP_LANES_ADDR)
};

/** $6277: stage1_map_loop_hazards */
static const u8 stage1_map_loop_hazards[] = {
  MAP_HAZARD_WAIT(42),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(4),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(27),
  MAP_CMD_START_BARRIERS_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(99),
  MAP_CMD_START_BARRIERS_R,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(46),
  MAP_CMD_GOTO(STAGE1_MAP_LOOP_HAZARDS_ADDR)
};

/** $6294: stage1_map_loop_leftobjs */
static const u8 stage1_map_loop_leftobjs[] = {
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(6),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(11),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_CMD_GOTO(STAGE1_MAP_LOOP_LEFTOBJS_ADDR)
};

/** $6314: stage1_map_loop_rightobjs */
static const u8 stage1_map_loop_rightobjs[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(6),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(11),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_NONE(2),
  MAP_CMD_GOTO(STAGE1_MAP_LOOP_RIGHTOBJS_ADDR)
};

/* ----------------------------------------------------------------------- */

/** $642A: stage1_perp_face */
static const pixel_t stage1_perp_face[FACEBYTES] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X__XXXXX, X__XXXX_, __XXX___, ___XXX_X,
  X__XXXXX, XXX___XX, XXX_____, ____XX_X,
  X_XXXXX_, __XXXXX_, __X_____, ____XXXX,
  X_XXXXXX, XXX_____, ________, ____XXXX,
  X_XXX_X_, ________, __X_____, ____XXXX,
  X_XXXXX_, __X_X_X_, ___X____, __X_XXXX,
  XXXX_XX_, _X_____X, ___X___X, _X___XXX,
  XXXXXX__, ________, _X_X____, _____XXX,
  XXXXX___, __XXXXX_, _____X__, __XXXXXX,
  XXX_XX__, XX__XXXX, ____X__X, XX__XXXX,
  XXXXXXXX, XXXXXXXX, XX_XX_XX, XXXXXXXX,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XX_XXXX_, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXX_X, XXXXXXXX, XXX__XXX, XXXXXXXX,
  XXX_X_X_, XXXXXXXX, XXX__XXX, XXXXXXXX,
  XX_____X, XX_XXX_X, _X____XX, _XXXXX_X,
  XX__X_X_, X_X_X_X_, XX____X_, X_X_X_XX,
  X_X_X_XX, XX_X_X_X, X_X___XX, _X_X_XXX,
  X_X_X_X_, _XXXXXXX, XX______, XXXXXX_X,
  X_X___XX, _______X, __X___X_, _______X,
  X_X__XX_, X_____XX, _______X, _____X_X,
  X__X_XXX, _____XXX, XXX___XX, ____X__X,
  X__X_XXX, X____XXX, XXXXXX__, X__X_X_X,
  X___XXXX, _X__XX_X, XXXX____, ____X__X,
  X____XXX, XXX_X___, _X_____X, ___X___X,
  X____XXX, XX_X____, XXXXXXXX, ____X__X,
  X____XX_, XXX_X__X, XXXXX___, ___XX__X,
  X____XXX, _X_X____, ________, X_X_X__X,
  X____XXX, X_X_X_XX, _____XXX, ___X___X,
  X___XXXX, XX_X___X, XXXXXX__, __XX___X,
  X___XX_X, X_X_X___, XXXX____, _X_X___X,
  X__XXXX_, XX_X____, ________, __XX___X,
  X_XXXX_X, XXXXX_X_, ___X___X, _XX_XX_X,
  XX__X_X_, XXXXXX_X, __XXX_X_, XXX___XX,
  X___X___, _XXXXXXX, XXXXXXXX, X_X__X_X,
  X__XX___, X_XXXXXX, XXXXXXXX, __X___XX,
  X__XX___, _X_X_XXX, XXXXXXXX, __X__X_X,
  XX_XX___, ____X_X_, XXX_X_XX, X_X_X_XX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,

  attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_CYAN
};

/* ----------------------------------------------------------------------- */

/** $643E: stage1_lambo_bitmaps */
static const bitmap_t stage1_lambo_bitmaps[SPRITE_FRAMES] = {
  { 6, BITMAPFLAG_DEFAULT, 30, &bitmap_lambo_1[0], &bitmap_lambo_1[0]  },
  { 5, BITMAPFLAG_DEFAULT, 22, &bitmap_lambo_2[0], &bitmap_lambo_2[0]  },
  { 3, BITMAPFLAG_DEFAULT, 15, &bitmap_lambo_3[0], &bitmap_lambo_3[0]  },
  { 3, BITMAPFLAG_DEFAULT, 15, &bitmap_lambo_3[0], &bitmap_lambo_3[0]  },
  { 3, BITMAPFLAG_MASKED,   8, &bitmap_lambo_4[0], &bitmap_lambo_4[0]  },
  { 3, BITMAPFLAG_MASKED,   8, &bitmap_lambo_4[0], &bitmap_lambo_4s[0] },
};

/** $6468: stage1_truck_bitmaps */
static const bitmap_t stage1_truck_bitmaps[SPRITE_FRAMES] = {
  { 6, BITMAPFLAG_DEFAULT, 39, &bitmap_truck_1[0], &bitmap_truck_1[0]  },
  { 5, BITMAPFLAG_DEFAULT, 29, &bitmap_truck_2[0], &bitmap_truck_2[0]  },
  { 3, BITMAPFLAG_DEFAULT, 20, &bitmap_truck_3[0], &bitmap_truck_3[0]  },
  { 3, BITMAPFLAG_DEFAULT, 20, &bitmap_truck_3[0], &bitmap_truck_3[0]  },
  { 2, BITMAPFLAG_MASKED,  12, &bitmap_truck_4[0], &bitmap_truck_4s[0] },
  { 2, BITMAPFLAG_MASKED,  12, &bitmap_truck_4[0], &bitmap_truck_4s[0] },
};

/** $6492: stage1_car_bitmaps */
static const bitmap_t stage1_car_bitmaps[SPRITE_FRAMES] = {
  { 6, BITMAPFLAG_DEFAULT, 31, &bitmap_car_1[0], &bitmap_car_1[0]  },
  { 5, BITMAPFLAG_DEFAULT, 22, &bitmap_car_2[0], &bitmap_car_2[0]  },
  { 3, BITMAPFLAG_DEFAULT, 16, &bitmap_car_3[0], &bitmap_car_3[0]  },
  { 3, BITMAPFLAG_DEFAULT, 16, &bitmap_car_3[0], &bitmap_car_3[0]  },
  { 3, BITMAPFLAG_MASKED,   9, &bitmap_car_4[0], &bitmap_car_4s[0] },
  { 3, BITMAPFLAG_MASKED,   9, &bitmap_car_4[0], &bitmap_car_4s[0] },
};

/* $64BC: bitmap_lambo_1 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $6570: bitmap_lambo_2 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $65DE: bitmap_lambo_3 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $660B: bitmap_truck_1 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $66F5: bitmap_truck_2 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $6786: bitmap_truck_3 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $67C2: bitmap_car_1 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $687C: bitmap_car_2 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $68EA: bitmap_car_3 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $691A: bitmap_lambo_4 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $694A: bitmap_lambo_4s - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $697A: bitmap_truck_4 - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $69AA: bitmap_truck_4s - Conv: Deduped to CommonData.c (shared with stage 5) */

/* $69DA: bitmap_car_4 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6A10: bitmap_car_4s - Conv: Deduped to CommonData.c (shared with stage 3) */

/* ----------------------------------------------------------------------- */

/** $CE46: stage1_stones_bitmaps */
static const bitmap_t stage1_stones_bitmaps[SPRITE_FRAMES] = {
  { 2, BITMAPFLAG_MASKED, 5, &bitmap_stones_1[0], &bitmap_stones_1[0] },
  { 2, BITMAPFLAG_MASKED, 4, &bitmap_stones_2[0], &bitmap_stones_2s[0] },
  { 2, BITMAPFLAG_MASKED, 3, &bitmap_stones_3[0], &bitmap_stones_3s[0] },
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_stones_4[0], &bitmap_stones_4s[0] },
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_stones_4[0], &bitmap_stones_4s[0] },
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_stones_5[0], &bitmap_stones_5s[0] }
};

/** $CE70: stage1_dust_bitmaps */
static const bitmap_t stage1_dust_bitmaps[SPRITE_FRAMES] = {
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_dust_1[0], &bitmap_dust_1s[0] },
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_dust_1[0], &bitmap_dust_1s[0] },
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_dust_1[0], &bitmap_dust_1s[0] },
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_dust_1[0], &bitmap_dust_1s[0] },
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_dust_1[0], &bitmap_dust_1s[0] },
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_dust_1[0], &bitmap_dust_1s[0] }
};

/* $6A9A: bitmap_stones_1 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6AAE: bitmap_stones_2 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6ABE: bitmap_stones_2s - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6ACE: bitmap_stones_3 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6ADA: bitmap_stones_3s - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6AE6: bitmap_stones_4 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6AEA: bitmap_stones_4s - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6AEE: bitmap_stones_5 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6AF0: bitmap_stones_5s - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6AF2: bitmap_dust_1 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6AF4: bitmap_dust_1s - Conv: Deduped to CommonData.c (shared with stage 3) */

/* ----------------------------------------------------------------------- */

/** $6AF6: stage1_turn_sign_right */
static const depthset_t stage1_turn_sign_right = {
  &stage1_turn_sign_bitmaps[0],
  {
    0x24, 0x00,
    0x1C, 0x00,
    0x18, 0x07,
    0x18, 0x07,
    0x14, 0x0E,
    0x14, 0x0E,
    0x14, 0x15,
    0x10, 0x15,
    0x10, 0x1C,
    0x10, 0x1C
  }
};

/** $6B0C: stage1_turn_sign_left */
static const depthset_t stage1_turn_sign_left = {
  &stage1_turn_sign_bitmaps[0],
  {
    0x24, 0x23,
    0x1C, 0x23,
    0x18, 0x2A,
    0x18, 0x2A,
    0x14, 0x31,
    0x14, 0x31,
    0x14, 0x38,
    0x10, 0x38,
    0x10, 0x3F,
    0x10, 0x3F
  }
};

/** $6B22: stage1_turn_sign_bitmaps */
static const bitmap_t stage1_turn_sign_bitmaps[] = {
  { 4, BITMAPFLAG_DEFAULT,                  40, bitmap_turnsign_1, bitmap_turnsign_1  },
  { 3, BITMAPFLAG_DEFAULT,                  30, bitmap_turnsign_2, bitmap_turnsign_2  },
  { 2, BITMAPFLAG_DEFAULT,                  20, bitmap_turnsign_3, bitmap_turnsign_3  },
  { 2, BITMAPFLAG_MASKED,                   16, bitmap_turnsign_4, bitmap_turnsign_4  },
  { 2, BITMAPFLAG_MASKED,                   13, bitmap_turnsign_5, stage1_bitmap_turnsign_5s },
  { 4, BITMAPFLAG_FLIPPED,                  40, bitmap_turnsign_1, bitmap_turnsign_1  },
  { 3, BITMAPFLAG_FLIPPED,                  30, bitmap_turnsign_2, bitmap_turnsign_2  },
  { 2, BITMAPFLAG_FLIPPED,                  20, bitmap_turnsign_3, bitmap_turnsign_3  },
  { 2, BITMAPFLAG_FLIPPED | BITMAPFLAG_MASKED, 16, bitmap_turnsign_4, bitmap_turnsign_4  },
  { 2, BITMAPFLAG_FLIPPED | BITMAPFLAG_MASKED, 13, bitmap_turnsign_5, stage1_bitmap_turnsign_5s }
};

/* $6B68: bitmap_turnsign_1 - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $6C08: bitmap_turnsign_2 - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $6C62: bitmap_turnsign_3 - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $6C8A: bitmap_turnsign_4 - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $6CCA: bitmap_turnsign_5 - Conv: Deduped to CommonData.c (shared with stage 4) */

/** $6CFE: stage1_bitmap_turnsign_5s */
static const pixel_t stage1_bitmap_turnsign_5s[2 * 2 * 13 * 1] = {
  XXXXX___, _____XXX, _____XXX, XXXXX___,
  XXXX____, ____XXXX, ______XX, XXXXXX__,
  XXXX____, ____XXXX, ______XX, XXXXXX__,
  XXXX____, ____XXXX, ______XX, XXXXXX__,
  XXXX____, ____X___, ______XX, X_X_XX__,
  XXXX____, ____X__X, ______XX, _____X__,
  XXXX____, ____X__X, ______XX, ____XX__,
  XXXX____, ____X__X, ______XX, _____X__,
  XXXX____, ____X___, ______XX, X___XX__,
  XXXX____, ____X___, ______XX, XX___X__,
  XXXX____, ____X___, ______XX, _XX__X__,
  XXXX____, ____X___, ______XX, _____X__,
  XXXX____, ____XXXX, ______XX, XXXXXX__,
};

/* ----------------------------------------------------------------------- */

/** $6D82: stage1_tumbleweed_bitmaps */
static const bitmap_t stage1_tumbleweed_bitmaps[SPRITE_FRAMES] = {
  { 2, BITMAPFLAG_DEFAULT, 16, &bitmap_tumbleweed_1[0], &bitmap_tumbleweed_1[0] },
  { 2, BITMAPFLAG_DEFAULT, 11, &bitmap_tumbleweed_2[0], &bitmap_tumbleweed_2[0] },
  { 1, BITMAPFLAG_DEFAULT,  9, &bitmap_tumbleweed_3[0], &bitmap_tumbleweed_3[0] },
  { 1, BITMAPFLAG_DEFAULT,  9, &bitmap_tumbleweed_3[0], &bitmap_tumbleweed_3[0] },
  { 1, BITMAPFLAG_DEFAULT,  7, &bitmap_tumbleweed_4[0], &bitmap_tumbleweed_4[0] },
  { 1, BITMAPFLAG_DEFAULT,  7, &bitmap_tumbleweed_4[0], &bitmap_tumbleweed_4[0] }
};

/* $6DAC: bitmap_tumbleweed_1 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6DCC: bitmap_tumbleweed_2 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6DE2: bitmap_tumbleweed_3 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* $6DEB: bitmap_tumbleweed_4 - Conv: Deduped to CommonData.c (shared with stage 3) */

/* ----------------------------------------------------------------------- */

/** $6DF2: stage1_barrier_bitmaps */
static const bitmap_t stage1_barrier_bitmaps[SPRITE_FRAMES] = {
  { 4, BITMAPFLAG_DEFAULT, 17, &bitmap_barrier_1[0], &bitmap_barrier_1[0]  },
  { 4, BITMAPFLAG_DEFAULT, 17, &bitmap_barrier_1[0], &bitmap_barrier_1[0]  },
  { 3, BITMAPFLAG_DEFAULT, 13, &bitmap_barrier_2[0], &bitmap_barrier_2[0]  },
  { 2, BITMAPFLAG_DEFAULT,  9, &bitmap_barrier_3[0], &bitmap_barrier_3[0]  },
  { 2, BITMAPFLAG_DEFAULT,  9, &bitmap_barrier_3[0], &bitmap_barrier_3[0]  },
  { 2, BITMAPFLAG_MASKED,   7, &bitmap_barrier_4[0], &bitmap_barrier_4s[0] }
};

/* $6E1C: bitmap_barrier_1 - Conv: Deduped to CommonData.c (shared with stage 2/3/4/5) */

/* $6E60: bitmap_barrier_2 - Conv: Deduped to CommonData.c (shared with stage 2/3/4/5) */

/* $6E87: bitmap_barrier_3 - Conv: Deduped to CommonData.c (shared with stage 2/3/4/5) */

/* $6E99: bitmap_barrier_4 - Conv: Deduped to CommonData.c (shared with stage 2/3/4/5) */

/* $6EB5: bitmap_barrier_4s - Conv: Deduped to CommonData.c (shared with stage 2/3/4/5) */

/* ----------------------------------------------------------------------- */

/** $6ED1: stage1_stretchy_streetlamp_right */
static const stretchy_t stage1_stretchy_streetlamp_right[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_right     },
  { STRETCHY_TYPE_50PC,  &streetlampmiddle2_right    },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_right     },
  { STRETCHY_TYPE_FIXED, &stage1_streetlamptop_right },
  { STRETCHY_TYPE_END,   NULL }
};

/** $6EDE: stage1_stretchy_streetlamp_left */
static const stretchy_t stage1_stretchy_streetlamp_left[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_left     },
  { STRETCHY_TYPE_50PC,  &streetlampmiddle2_left    },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_left     },
  { STRETCHY_TYPE_FIXED, &stage1_streetlamptop_left },
  { STRETCHY_TYPE_END,   NULL }
};

/** $6EEB: stage1_streetlamptop_right */
static const depthset_t stage1_streetlamptop_right = {
  &stage1_streetlamptop_bitmaps[0],
  {
    0x10, 0x02,
    0x08, 0x02,
    0x08, 0x09,
    0x08, 0x09,
    0x04, 0x10,
    0x04, 0x10,
    0x08, 0x17,
    0x08, 0x17,
    0x04, 0x1E,
    0x04, 0x1E
  }
};

/** $6F01: stage1_streetlamptop_left */
static const depthset_t stage1_streetlamptop_left = {
  &stage1_streetlamptop_flipped_bitmaps[0],
  {
    0x18, 0x02,
    0x10, 0x02,
    0x10, 0x09,
    0x10, 0x09,
    0x0C, 0x10,
    0x0C, 0x10,
    0x10, 0x17,
    0x10, 0x17,
    0x0C, 0x1E,
    0x0C, 0x1E
  }
};

/** $6F17: stage1_streetlamptop_bitmaps */
static const bitmap_t stage1_streetlamptop_bitmaps[5] = {
  { 4, BITMAPFLAG_DEFAULT, 8, bitmap_streetlamptop_1, bitmap_streetlamptop_1  },
  { 3, BITMAPFLAG_DEFAULT, 5, bitmap_streetlamptop_2, bitmap_streetlamptop_2  },
  { 3, BITMAPFLAG_DEFAULT, 4, bitmap_streetlamptop_3, bitmap_streetlamptop_3s },
  { 2, BITMAPFLAG_MASKED,  4, bitmap_streetlamptop_4, bitmap_streetlamptop_4s },
  { 2, BITMAPFLAG_MASKED,  3, bitmap_streetlamptop_5, bitmap_streetlamptop_5s }
};

/** $6F3A: stage1_streetlamptop_flipped_bitmaps */
static const bitmap_t stage1_streetlamptop_flipped_bitmaps[5] = {
  { 4, BITMAPFLAG_FLIPPED, 8, bitmap_streetlamptop_1, bitmap_streetlamptop_1  },
  { 3, BITMAPFLAG_FLIPPED, 5, bitmap_streetlamptop_2, bitmap_streetlamptop_2  },
  { 3, BITMAPFLAG_FLIPPED, 4, bitmap_streetlamptop_3, bitmap_streetlamptop_3s },
  { 2, BITMAPFLAG_MASKED | BITMAPFLAG_FLIPPED, 4, bitmap_streetlamptop_4, bitmap_streetlamptop_4s },
  { 2, BITMAPFLAG_MASKED | BITMAPFLAG_FLIPPED, 3, bitmap_streetlamptop_5, bitmap_streetlamptop_5s }
};

/* $6F5D: bitmap_streetlamptop_1 - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $6F7D: bitmap_streetlamptop_2 - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $6F8C: bitmap_streetlamptop_3 - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $6F98: bitmap_streetlamptop_3s - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $6FA4: bitmap_streetlamptop_4 - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $6FB4: bitmap_streetlamptop_4s - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $6FC4: bitmap_streetlamptop_5 - Conv: Deduped to CommonData.c (shared with stage 4) */

/* $6FD0: bitmap_streetlamptop_5s - Conv: Deduped to CommonData.c (shared with stage 4) */

/* ----------------------------------------------------------------------- */

/** $6FDC: stage1_stretchy_telegraphpole_right */
static const stretchy_t stage1_stretchy_telegraphpole_right[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_right },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_right },
  { STRETCHY_TYPE_FIXED, &stage1_telegraphpoletop_right },
  { STRETCHY_TYPE_END, NULL }
};

/** $6FE6: stage1_stretchy_telegraphpole_left */
static const stretchy_t stage1_stretchy_telegraphpole_left[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_left },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_left },
  { STRETCHY_TYPE_FIXED, &stage1_telegraphpoletop_left },
  { STRETCHY_TYPE_END, NULL }
};

/** $6FF0: stage1_telegraphpoletop_left */
static const depthset_t stage1_telegraphpoletop_left = {
  &stage1_telegraphpoletop_bitmaps[0],
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
    0x0C, 0x1E
  }
};

/** $7006: stage1_telegraphpoletop_right */
static const depthset_t stage1_telegraphpoletop_right = {
  &stage1_telegraphpoletop_bitmaps[0],
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
    0x04, 0x1E
  }
};

/** $701C: stage1_telegraphpoletop_bitmaps */
static const bitmap_t stage1_telegraphpoletop_bitmaps[5] = {
  { 3, BITMAPFLAG_DEFAULT, 13, bitmap_telegraphpoletop_1, bitmap_telegraphpoletop_1  },
  { 3, BITMAPFLAG_DEFAULT, 10, bitmap_telegraphpoletop_2, bitmap_telegraphpoletop_2  },
  { 3, BITMAPFLAG_DEFAULT,  7, bitmap_telegraphpoletop_3, bitmap_telegraphpoletop_3s },
  { 2, BITMAPFLAG_MASKED,   5, bitmap_telegraphpoletop_4, bitmap_telegraphpoletop_4s },
  { 2, BITMAPFLAG_MASKED,   4, bitmap_telegraphpoletop_5, bitmap_telegraphpoletop_5s }
};

/* $703F: bitmap_telegraphpoletop_1 - Conv: Deduped to CommonData.c (shared with stage 3/5) */

/* $7066: bitmap_telegraphpoletop_2 - Conv: Deduped to CommonData.c (shared with stage 3/5) */

/* $7084: bitmap_telegraphpoletop_3 - Conv: Deduped to CommonData.c (shared with stage 3/5) */

/* $7099: bitmap_telegraphpoletop_3s - Conv: Deduped to CommonData.c (shared with stage 3/5) */

/* $70AE: bitmap_telegraphpoletop_4 - Conv: Deduped to CommonData.c (shared with stage 3/5) */

/* $70C2: bitmap_telegraphpoletop_4s - Conv: Deduped to CommonData.c (shared with stage 3/5) */

/* $70D6: bitmap_telegraphpoletop_5 - Conv: Deduped to CommonData.c (shared with stage 3/5) */

/* $70E6: bitmap_telegraphpoletop_5s - Conv: Deduped to CommonData.c (shared with stage 3) */

/* ----------------------------------------------------------------------- */

/** $70F6: stage1_stretchy_tree */
static const stretchy_t stage1_stretchy_tree[SPRITE_FRAMES] = {
  { STRETCHY_TYPE_FIXED, &stage1_tree_bottom       },
  { STRETCHY_TYPE_38PC,  &stage1_tree_bottommiddle },
  { STRETCHY_TYPE_FIXED, &stage1_tree_middle       },
  { STRETCHY_TYPE_113PC, &stage1_tree_topmiddle    },
  { STRETCHY_TYPE_FIXED, &stage1_tree_top          },
  { STRETCHY_TYPE_END, NULL }
};

/** $7106: stage1_stretchy_bush */
static const stretchy_t stage1_stretchy_bush[4] = {
  { STRETCHY_TYPE_FIXED, &stage1_tree_middle    },
  { STRETCHY_TYPE_38PC,  &stage1_tree_topmiddle },
  { STRETCHY_TYPE_FIXED, &stage1_tree_top       },
  { STRETCHY_TYPE_END, NULL }
};

/** $7110: stage1_tree_bottom */
static const depthset_t stage1_tree_bottom = {
  &stage1_tree_bitmaps[0],
  {
    0x1C, 0x17,
    0x14, 0x17,
    0x10, 0x2C,
    0x10, 0x2C,
    0x0C, 0x41,
    0x0C, 0x41,
    0x0C, 0x5D,
    0x08, 0x5D,
    0x08, 0xAA,
    0x08, 0xAA
  }
};

/** $7126: stage1_tree_bottommiddle */
static const depthset_t stage1_tree_bottommiddle = {
  &stage1_tree_bitmaps[0],
  {
    0x34, 0x10,
    0x2C, 0x10,
    0x20, 0x87,
    0x20, 0x87,
    0x14, 0x8E,
    0x14, 0x8E,
    0x14, 0x56,
    0x10, 0x56,
    0x08, 0xA3,
    0x08, 0xA3
  }
};

/** $713C: stage1_tree_middle */
static const depthset_t stage1_tree_middle = {
  &stage1_tree_bitmaps[0],
  {
    0x1C, 0x09,
    0x14, 0x09,
    0x10, 0x25,
    0x10, 0x25,
    0x0C, 0x3A,
    0x0C, 0x3A,
    0x0C, 0x4F,
    0x08, 0x4F,
    0x08, 0x9C,
    0x08, 0x9C
  }
};

/** $7152: stage1_tree_topmiddle */
static const depthset_t stage1_tree_topmiddle = {
  &stage1_tree_bitmaps[0],
  {
    0x1C, 0x02,
    0x14, 0x02,
    0x10, 0x1E,
    0x10, 0x1E,
    0x0C, 0x33,
    0x0C, 0x33,
    0x0C, 0x48,
    0x08, 0x48,
    0x08, 0x95,
    0x08, 0x95
  }
};

/** $7168: stage1_tree_top */
static const depthset_t stage1_tree_top = {
  &stage1_tree_bitmaps[0],
  {
    0x1C, 0x64,
    0x14, 0x64,
    0x10, 0x6B,
    0x10, 0x6B,
    0x0C, 0x72,
    0x0C, 0x72,
    0x0C, 0x79,
    0x08, 0x79,
    0x08, 0x80,
    0x08, 0x80
  }
};

/** $717E: stage1_tree_bitmaps */
static const bitmap_t stage1_tree_bitmaps[25] = {
  { 8, BITMAPFLAG_DEFAULT, 16, &stage1_bitmap_tree_middle_64x16[0],        &stage1_bitmap_tree_middle_64x16[0] },
  { 8, BITMAPFLAG_DEFAULT,  5, &stage1_bitmap_tree_bottom_64x5[0],         &stage1_bitmap_tree_bottom_64x5[0]  },
  { 2, BITMAPFLAG_DEFAULT,  8, &stage1_bitmap_tree_trunk_16x8[0],          &stage1_bitmap_tree_trunk_16x8[0]   },
  { 8, BITMAPFLAG_DEFAULT,  5, &stage1_bitmap_tree_shadow_64x5[0],         &stage1_bitmap_tree_shadow_64x5[0]  },
  { 6, BITMAPFLAG_DEFAULT, 12, &stage1_bitmap_tree_middle_48x12[0],        &stage1_bitmap_tree_middle_48x12[0] },
  { 6, BITMAPFLAG_DEFAULT,  4, &stage1_bitmap_tree_bottom_48x4[0],         &stage1_bitmap_tree_bottom_48x4[0]  },
  { 6, BITMAPFLAG_DEFAULT,  4, &stage1_bitmap_tree_shadow_48x4[0],         &stage1_bitmap_tree_shadow_48x4[0]  },
  { 4, BITMAPFLAG_DEFAULT,  8, &stage1_bitmap_tree_middle_32x8[0],         &stage1_bitmap_tree_middle_32x8[0]  },
  { 4, BITMAPFLAG_DEFAULT,  3, &stage1_bitmap_tree_bottom_32x3[0],         &stage1_bitmap_tree_bottom_32x3[0]  },
  { 4, BITMAPFLAG_DEFAULT,  2, &stage1_bitmap_tree_shadow_32x2[0],         &stage1_bitmap_tree_shadow_32x2[0]  },
  { 3, BITMAPFLAG_DEFAULT,  7, &stage1_bitmap_tree_middle_24x7[0],         &stage1_bitmap_tree_middle_24x7[0]  },
  { 3, BITMAPFLAG_DEFAULT,  2, &stage1_bitmap_tree_bottom_24x2[0],         &stage1_bitmap_tree_bottom_24x2[0]  },
  { 1, BITMAPFLAG_DEFAULT,  4, &stage1_bitmap_tree_trunk_8x4[0],           &stage1_bitmap_tree_trunk_8x4[0]    },
  { 3, BITMAPFLAG_DEFAULT,  2, &stage1_bitmap_tree_shadow_24x2[0],         &stage1_bitmap_tree_shadow_24x2[0]  },
  { 8, BITMAPFLAG_MASKED,  13, &stage1_bitmap_tree_top_64x13[0],           &stage1_bitmap_tree_top_64x13[0]    },
  { 6, BITMAPFLAG_MASKED,  10, &stage1_bitmap_tree_top_48x10[0],           &stage1_bitmap_tree_top_48x10[0]    },
  { 4, BITMAPFLAG_MASKED,   5, &stage1_bitmap_tree_top_32x5[0],            &stage1_bitmap_tree_top_32x5[0]     },
  { 3, BITMAPFLAG_MASKED,   4, &stage1_bitmap_tree_top_24x4[0],            &stage1_bitmap_tree_top_24x4[0]     },
  { 3, BITMAPFLAG_MASKED,   3, &stage1_bitmap_tree_top_24x3[0],            &stage1_bitmap_tree_top_24x3s[0]    },
  { 2, BITMAPFLAG_MASKED,   6, &stage1_bitmap_tree_trunk_16x6[0],          &stage1_bitmap_tree_trunk_16x6[0]   },
  { 2, BITMAPFLAG_MASKED,   4, &stage1_bitmap_tree_trunk_16x4[0],          &stage1_bitmap_tree_trunk_16x4[0]   },
  { 3, BITMAPFLAG_MASKED,   5, &stage1_bitmap_tree_middle_24x5[0],         &stage1_bitmap_tree_middle_24x5s[0] },
  { 3, BITMAPFLAG_MASKED,   2, &stage1_bitmap_tree_bottom_24x2_another[0], &stage1_bitmap_tree_bottom_24x2s[0] },
  { 3, BITMAPFLAG_MASKED,   3, &stage1_bitmap_tree_trunk_24x3[0],          &stage1_bitmap_tree_trunk_24x3s[0]  },
  { 3, BITMAPFLAG_MASKED,   1, &stage1_bitmap_tree_shadow_24x1[0],         &stage1_bitmap_tree_shadow_24x1s[0] }
};

/** $722D: stage1_bitmap_tree_middle_64x16 */
static const pixel_t stage1_bitmap_tree_middle_64x16[8 * 1 * 16 * 1] = {
  _XXXXXX_, X_XXXXXX, XXXXXXXX, XXX_X_XX, XX__X_XX, _X_XXXX_, X_X_XXXX, XXXXXX__,
  XXXX_XXX, XXX_XXXX, _X_XXX_X, XX_X_X_X, X____XX_, X_X_X_XX, _X_XXXXX, X_XXX___,
  _XX_XXX_, XX_X_XX_, X_X_XXXX, _XX___XX, XXX_XXXX, _X_XXXXX, XXXX_XXX, XX_XXXX_,
  XXXXXX_X, X___XXXX, XX_XXXXX, XX_X_XXX, XX_X_XXX, XXXXX_X_, XX_XXX_X, XXXXXXX_,
  XXXXXXXX, XX_X_X_X, XXXXXX_X, XXXXXX_X, XXXXXX_X, XXX_XX_X, XXXXX_X_, XXXX_XXX,
  XXX_XXXX, X_X_XXXX, _XX_X_X_, X_X_XXXX, XX_XXXXX, _X_X__X_, XXX_XX_X, X_XXXXX_,
  _XXXXXX_, XXXXXXX_, __XXXX_X, _XXXX_XX, _XX_X_X_, XXX____X, _XXXX_X_, XXXX_XX_,
  __XX_XXX, XX_XXX_X, _X_XX_X_, __XX_X_X, XX_XXXXX, XX_X__X_, XXXX_X_X, XX_XXX__,
  ___XXXX_, X_XXXXX_, X_X_XX_X, _XXXX_XX, X_X_XXXX, _XXX_X_X, XX_XXXXX, XXXXXXX_,
  __XXX_XX, _X_XXX_X, _XXXXXXX, XXXXXXXX, ___XX_XX, XX_XXXXX, X_XXXXX_, XXXX_XX_,
  ___XXXX_, __X_XXX_, XXXXXXX_, XX_X_XXX, X_XX_X_X, _XXXXXXX, XXXX_XXX, X_X_XX__,
  ____X_X_, X_XXX_XX, XXX_XX_X, __X_XX_X, XX_XX___, X_X_XX_X, _XX_XXXX, XXXXXX__,
  ____XXXX, _X_XXX__, X_XXXXX_, X__X_XX_, XXXX_X__, _X_XX_X_, XXXX_XXX, _XX_XX__,
  ___XXXXX, X_XXX_X_, __XXX_X_, __X_XXXX, X_XXX_X_, X_XXX__X, _XXXXXXX, XXXX_XX_,
  ___XXXX_, XXXX_X__, _X_XXXXX, _X_XXX_X, _X_XXXXX, XXXX_X__, XX_X_XX_, XXXXXXX_,
  ____XXXX, XXXXX_X_, X_XX_X_X, XXXXXXXX, X_XX_XXX, XXXXX_XX, X_X_X_XX, XX_XXX__,
};

/** $72AD: stage1_bitmap_tree_bottom_64x5 */
static const pixel_t stage1_bitmap_tree_bottom_64x5[8 * 1 * 5 * 1] = {
  ________, ____X__X, __X__X_X, XXX_X_X_, X_X____X, _X___X__, X___X__X, ________,
  ______XX, X_XXXX_X, _XX__XXX, XXXX_XXX, _XX_X___, X__X___X, _X_XX_X_, X_X_____,
  ____X_X_, XXXX_XX_, XX_XX_X_, XXXXXXXX, X_XXXXX_, _XX_XXXX, _XXX_XXX, X__X____,
  _X__XXXX, XXXXXXXX, XXXXXXXX, X_XXXX_X, XXXXXXXX, ___XXXXX, XXXXXXX_, XXXX_X__,
  _X_XXXX_, XX_X_XXX, X_XXXXX_, XXX_XXXX, XXXX_XXX, XXXXX_XX, X_XXX_XX, XX_XXXX_,
};

/** $72D5: stage1_bitmap_tree_trunk_16x8 */
static const pixel_t stage1_bitmap_tree_trunk_16x8[2 * 1 * 8 * 1] = {
  X_X__X_X, _X_XX__X,
  XX__XX_X, __X__X_X,
  _XX__X__, X_X_X__X,
  _X_X_X__, __X__XX_,
  X_XX__X_, X___X_X_,
  XX_X_X__, X_X__XX_,
  X_X__X__, X_X_X__X,
  XX_X_X_X, __X__X_X,
};

/** $72E5: stage1_bitmap_tree_shadow_64x5 */
static const pixel_t stage1_bitmap_tree_shadow_64x5[8 * 1 * 5 * 1] = {
  ________, __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_______,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XX_XXXXX, XXXX__XX, XXXXXXXX, XXXXXXXX, XXXXX___,
  XXXXXXXX, XXXXXXXX, XXXXX__X, XX__XXX_, X_X__X_X, X_XXXXXX, XXXXXXXX, XXXXXXXX,
  ______XX, XXXXXXXX, XXXXXX__, XXX___XX, _X__X_X_, _XXXXXXX, XXXXXXXX, XXX_____,
  ________, ___XXXXX, XXXXXXXX, ___X__X_, X_______, XXXXXXXX, XXXX____, ________,
};

/** $730D: stage1_bitmap_tree_middle_48x12 */
static const pixel_t stage1_bitmap_tree_middle_48x12[6 * 1 * 12 * 1] = {
  _XXXX_XX, XXXXXXXX, XXXX_XXX, XX_XXX__, XXX_XX_X, XXXXXX__,
  XXX_XXX_, XX_X__XX, _XX____X, X___X_XX, _XXX__XX, XXX_XX__,
  XX_X_XXX, __XXX_XX, XXX_X_XX, X_X_XXXX, XXX_X_XX, _XXXXXX_,
  XXXXXXX_, X__XXXXX, _XXX_X_X, XXXX_XXX, _X_XXXXX, X_XXX_XX,
  XX_XXXXX, _XXX_X_X, X_XXXXXX, X_XXXX__, X_X_XX_X, _XXXXXX_,
  _XX_XX_X, XX_X__XX, X__XX_XX, X_XXXXX_, XX__XXX_, _XX_XX__,
  __XXX_XX, XXX_XX_X, _X_XXX_X, XX_XXXXX, X_XXX_X_, XXXXXXX_,
  _XXXXXX_, XX_X_XXX, XXXX_XXX, __XXXXX_, XXXXXXXX, X_XXX_X_,
  ___XX_XX, XXXXXXXX, _X_XXX_X, X_XX__XX, _X_X_X_X, XXXXXX__,
  ___XXXXX, XX__XXXX, X_X_XXX_, XXX__X__, XXX_XXX_, XX_X_X__,
  __XX_XXX, _XX__XXX, ___XXX_X, XXXXX_XX, XX_X_XXX, XXXXX_X_,
  ___XXXXX, XXX_XXX_, _XXXXXXX, XXX_XXXX, XXXXXX_X, XXX_XX__,
};

/** $7355: stage1_bitmap_tree_bottom_48x4 */
static const pixel_t stage1_bitmap_tree_bottom_48x4[6 * 1 * 4 * 1] = {
  ______X_, _X_X_X__, _XXX_X__, X____X__, __X____X, _X______,
  ____XXXX, XX_XXX__, XXXXX_XX, _XXX__X_, X__X__XX, X_XX____,
  ___XX_XX, X_XXX_XX, X_XXXXXX, XXXXX__X, _XXX_XX_, XXXXX___,
  __XXXXX_, XXXXXXXX, X_XXX_XX, XXX_XXXX, X_XXXXXX, XXX_XXX_,
};

/** $736D: stage1_bitmap_tree_shadow_48x4 */
static const pixel_t stage1_bitmap_tree_shadow_48x4[6 * 1 * 4 * 1] = {
  _______X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  _XXXXXXX, XXXXXXXX, XXX_XXXX, XXX_XXXX, XXXXXXXX, XXXXXX__,
  ____XXXX, XXXXXXXX, ___X__X_, ___XX__X, XXXXXXXX, XXXX____,
  ________, __XXXXXX, XXX____X, _____XXX, XXXXXXX_, ________,
};

/** $7385: stage1_bitmap_tree_middle_32x8 */
static const pixel_t stage1_bitmap_tree_middle_32x8[4 * 1 * 8 * 1] = {
  XXXXXXXX, __X_XX__, X__X_XXX, XXXX_XXX,
  XXX_X_XX, X_XXX__X, X_XXXXXX, X_XXXXX_,
  XXXXXXXX, _XXXXXX_, XXXX_X_X, XXX_XXXX,
  _X_XX_X_, __XX_XXX, X_XXX__X, XX_XX_X_,
  XXXX__XX, _XXXXXX_, _XXXX_XX, XXX_XX_X,
  __XXXXX_, XXX_XX_X, __X_XXXX, _XXXXXX_,
  _XXX_XXX, _XX__XX_, XXXXXXX_, XXXXXX_X,
  __XXXXXX, XX_XXXXX, X_XXX_XX, _XX_XXX_,
};

/** $73A5: stage1_bitmap_tree_bottom_32x3 */
static const pixel_t stage1_bitmap_tree_bottom_32x3[4 * 1 * 3 * 1] = {
  _X_X__X_, _X__XX_X, XX_XX___, X_X_____,
  __XXXX_X, X_XXXXXX, XXXX_XX_, XX_XX___,
  _XXXX__X, XXXXXXXX, XX_XXXXX, XXXXX_XX,
};

/** $73B1: stage1_bitmap_tree_shadow_32x2 */
static const pixel_t stage1_bitmap_tree_shadow_32x2[4 * 1 * 2 * 1] = {
  _XXXXXXX, XXXXX_XX, XXX_XXXX, XXXXXXX_,
  ___XXXXX, XXX_XX_X, _X_X_XXX, XXXXXX__,
};

/** $73B9: stage1_bitmap_tree_middle_24x7 */
static const pixel_t stage1_bitmap_tree_middle_24x7[3 * 1 * 7 * 1] = {
  _XXXXXX_, XXXXX__X, XX_XXXX_,
  XXX_X_XX, XX_XX_XX, _XXX_XXX,
  _XXXXXXX, X_XXXXX_, _XXXXX_X,
  XXX_XX_X, XX_XX_XX, XX__XXXX,
  XX_XX_XX, _XXXXXX_, X_X_XX_X,
  _X__XXXX, __XX_X__, XXXXXXXX,
  _XXX_XX_, _X_XX_XX, XX_XX_XX,
};

/** $73CE: stage1_bitmap_tree_bottom_24x2 */
static const pixel_t stage1_bitmap_tree_bottom_24x2[3 * 1 * 2 * 1] = {
  ___X_XX_, XX_X__XX, __X_XX__,
  _XXXXXXX, X_X_XXX_, XXXXX_X_,
};

/** $73D4: stage1_bitmap_tree_trunk_8x4 */
static const pixel_t stage1_bitmap_tree_trunk_8x4[1 * 1 * 4 * 1] = {
  _X_X_XX_,
  _XX_X_X_,
  _X__X_X_,
  _XX___X_,
};

/** $73D8: stage1_bitmap_tree_shadow_24x2 */
static const pixel_t stage1_bitmap_tree_shadow_24x2[3 * 1 * 2 * 1] = {
  XXXXXXXX, X_XXXX_X, XXXXXXXX,
  __XXXXXX, _X_X__X_, XXXXXXX_,
};

/** $73DE: stage1_bitmap_tree_top_64x13 */
static const pixel_t stage1_bitmap_tree_top_64x13[8 * 2 * 13 * 1] = {
  XXXX____, ____XX_X, ________, X___XXXX, ________, XX_XXXXX, ________, XX_X_XXX, ________, XX_X_XXX, ________, XXXXX_X_, ________, XX_XXX_X, _____XXX, XXXXX___,
  XXXX____, ____XXXX, ________, XX_X_X_X, ________, XXXXXX_X, ________, XXXXXX_X, ________, XXXXXX_X, ________, XXX_XX_X, ________, XXXXX_X_, ____XXXX, XXXX____,
  XXXXX___, _____XXX, ________, X_X_XXXX, ________, _XX_X_X_, ________, X_X_XXXX, ________, XX_XXXXX, ________, _X_X__X_, ________, XXX_XX_X, ___XXXXX, X_X_____,
  XXXXXXXX, ________, ________, XXXXXXX_, ________, __XXXX_X, ________, _XXXX_XX, ________, _XX_X_X_, ________, XXX____X, ________, _XXXX_X_, __XXXXXX, XX______,
  XXXXXXXX, ________, XXX_____, ___XXX_X, ________, _X_XX_X_, ________, __XX_X_X, ________, XX_XXXXX, ________, XX_X__X_, ________, XXXX_XXX, _XXXXXXX, X_______,
  XXXXXXXX, ________, XXXX____, ____XXX_, ________, X_X_XX_X, ________, _XXXX_XX, ________, X_X_XXXX, ________, _XXX_X_X, ______XX, XX_XXX__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXX____, ____XX_X, ________, _XXXXXXX, ________, XXXXXXXX, ________, ___XX_XX, ________, XX_XXXXX, ______XX, X_XXXX__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXX___, _____XX_, ________, XXXX_XX_, ________, XX_X_XXX, ________, X_XX_X_X, ________, _XXXXXXX, ____XXXX, _X_X____, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXX__, ______XX, ________, X_X_XX_X, ________, __X_XX_X, ________, XX_XX___, ________, X_X_XX__, ___XXXXX, XXX_____, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, ________, XXXXXXX_, ________, X__X_XX_, ________, XXXX_X__, ________, _X_XXXXX, _XXXXXXX, X_______, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, XXXXXX__, ______X_, ________, __X_XXXX, _X______, X_XXX_X_, _____XXX, X_XXX___, XXXXXXXX, ________, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, _______X, ______XX, _X_XXX__, XXXX____, ____XXXX, ____XXXX, XXXX____, XXXXXXXX, ________, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, _____XXX, XXXXX___, XXXXXX__, ______XX, ___XXXXX, XXX_____, XXXXXXXX, ________, XXXXXXXX, ________,
};

/** $74AE: stage1_bitmap_tree_top_48x10 */
static const pixel_t stage1_bitmap_tree_top_48x10[6 * 2 * 10 * 1] = {
  XXX_____, ___XXXXX, ________, __X_X__X, ________, XXXX__XX, ________, __X_XXXX, ________, XXX_X_XX, ______XX, _XXXXX__,
  XXX_____, ___XX_X_, ________, X__XXXXX, ________, _XX_XX_X, ________, XXXX_XXX, ________, _X_XXXXX, _____XXX, X_X_X___,
  XXXX____, ____XXXX, ________, _XXX_X_X, ________, X_XXXXXX, ________, X_XXXX_X, ________, __X_XX_X, ____XXXX, _XXX____,
  XXXXXX__, ______XX, ________, XX_X__XX, ________, X__XX_XX, ________, X__XXXX_, ________, _X_XXXX_, ___XXXXX, XXX_____,
  XXXXXXXX, ________, ________, XXX_XX_X, ________, _X_XXX_X, ________, XX_X_X_X, ________, X_XXXXXX, XXXXXXXX, ________,
  XXXXXXXX, ________, X_______, _X_X_XXX, ________, XXXXXXXX, ________, __XXX_XX, ________, XXXX_XXX, XXXXXXXX, ________,
  XXXXXXXX, ________, XX______, __XXXX_X, ________, XX_X_XXX, ________, X_XXXXXX, ______XX, _X__XX__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXX____, ____XXXX, ________, X___X_X_, ________, XXX__X__, _____XXX, XXXXX___, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, ________, XX_X_XXX, ________, XXXXX__X, __XXXXXX, XX______, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, XX____XX, __XXXX__, XXXX____, ____XXXX, XXXXXXXX, ________, XXXXXXXX, ________,
};

/** $7526: stage1_bitmap_tree_top_32x5 */
static const pixel_t stage1_bitmap_tree_top_32x5[4 * 2 * 5 * 1] = {
  XXXXX___, _____X_X, ________, _XXXXX_X, ________, XXXX_X_X, ____XXXX, X_XX____,
  XXXXX___, _____XX_, ________, X_X_XXXX, ________, X_XXXXXX, __XXXXXX, XX______,
  XXXXXX__, ______XX, ________, XXXX_X_X, ________, XX_X___X, _XXXXXXX, X_______,
  XXXXXXXX, ________, XX______, __X_X_XX, _______X, XXX__XX_, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX___XX, ___XXX__, XX___XXX, __XXX___, XXXXXXXX, ________,
};

/** $754E: stage1_bitmap_tree_top_24x4 */
static const pixel_t stage1_bitmap_tree_top_24x4[3 * 2 * 4 * 1] = {
  XX______, __XXXXXX, ________, _XXX_XXX, _______X, XXXXX_X_,
  XXXX____, ____X_X_, ________, XXXXXX_X, ______XX, _XX_XX__,
  XXXXX___, _____XXX, ________, X_X_XX__, ____XXXX, XXXX____,
  XXXXXXX_, _______X, ____X___, XXXX_XXX, _XXXXXXX, X_______,
};

/** $7566: stage1_bitmap_tree_top_24x3 */
static const pixel_t stage1_bitmap_tree_top_24x3[3 * 2 * 3 * 1] = {
  XXX_____, ___XX_XX, ________, _XXX_XXX, XXXXXXXX, ________,
  XXXX____, ____XXXX, _______X, XX_XXXX_, XXXXXXXX, ________,
  XXXXXX__, ______XX, XX___XXX, __XXX___, XXXXXXXX, ________,
};

/** $7578: stage1_bitmap_tree_top_24x3s */
static const pixel_t stage1_bitmap_tree_top_24x3s[3 * 2 * 3 * 1] = {
  XXXXXXX_, _______X, ________, X_XX_XXX, ____XXXX, _XXX____,
  XXXXXXXX, ________, ________, XXXXXX_X, ___XXXXX, XXX_____,
  XXXXXXXX, ________, XX__XX__, __XX__XX, _XXXXXXX, X_______,
};

/** $758A: stage1_bitmap_tree_trunk_16x6 */
static const pixel_t stage1_bitmap_tree_trunk_16x6[2 * 2 * 6 * 1] = {
  XXX_____, ___X___X, _____XXX, __XXX___,
  XXX_____, ___X_X_X, _____XXX, _X__X___,
  XXX_____, ___XX_X_, ____XXXX, _X_X____,
  XX______, __X_X_X_, ____XXXX, X__X____,
  XX______, __X_X___, _____XXX, X_X_X___,
  XX______, __X_X__X, _____XXX, _X__X___,
};

/** $75A2: stage1_bitmap_tree_trunk_16x4 */
static const pixel_t stage1_bitmap_tree_trunk_16x4[2 * 2 * 4 * 1] = {
  XXXX____, ____XX_X, ____XXXX, _X_X____,
  XXXX____, ____XX_X, ____XXXX, ___X____,
  XXXX____, ____X___, ____XXXX, _X_X____,
  XXXX____, ____XX_X, ____XXXX, _X_X____,
};

/** $75B2: stage1_bitmap_tree_middle_24x5 */
static const pixel_t stage1_bitmap_tree_middle_24x5[3 * 2 * 5 * 1] = {
  X_______, _XX_XXXX, ________, X_XXXXXX, ___XXXXX, _XX_____,
  ________, XX_XX_X_, ________, XXX_XX_X, __XXXXXX, XX______,
  ________, XXXXXX_X, ________, X_XXXXXX, ___XXXXX, XXX_____,
  X_______, _X_XXXXX, ________, XXXX_XX_, ___XXXXX, XXX_____,
  X_______, _XXX_XXX, ________, _XX_XXXX, __XXXXXX, XX______,
};

/** $75D0: stage1_bitmap_tree_bottom_24x2_another */
static const pixel_t stage1_bitmap_tree_bottom_24x2_another[3 * 2 * 2 * 1] = {
  XX_X_XX_, __X_X__X, ____XX_X, XX_X__X_, _XXXXXXX, X_______,
  X_______, _X_XXXX_, ________, XXXX_XXX, __XXXXXX, _X______,
};

/** $75DC: stage1_bitmap_tree_trunk_24x3 */
static const pixel_t stage1_bitmap_tree_trunk_24x3[3 * 2 * 3 * 1] = {
  XXXXXXXX, ________, ____XXXX, X__X____, XXXXXXXX, ________,
  XXXXXXX_, _______X, ____XXXX, X_XX____, XXXXXXXX, ________,
  XXXXXXX_, _______X, ____XXXX, _X_X____, XXXXXXXX, ________,
};

/** $75EE: stage1_bitmap_tree_shadow_24x1 */
static const pixel_t stage1_bitmap_tree_shadow_24x1[3 * 2 * 1 * 1] = {
  ________, XXXXXXX_, ________, _XX_XXXX, ____XXXX, XXXX____,
};

/** $75F4: stage1_bitmap_tree_middle_24x5s */
static const pixel_t stage1_bitmap_tree_middle_24x5s[3 * 2 * 5 * 1] = {
  XXXXX___, _____XX_, ________, XXXXX_XX, _______X, XXXX_XX_,
  XXXX____, ____XX_X, ________, X_X_XXX_, ______XX, XX_XXX__,
  XXXX____, ____XXXX, ________, XX_XX_XX, _______X, XXXXXXX_,
  XXXXX___, _____X_X, ________, XXXXXXXX, _______X, _XX_XXX_,
  XXXXX___, _____XXX, ________, _XXX_XX_, ______XX, XXXXXX__,
};

/** $7612: stage1_bitmap_tree_bottom_24x2s */
static const pixel_t stage1_bitmap_tree_bottom_24x2s[3 * 2 * 2 * 1] = {
  XXXXXX_X, ______X_, _XX_____, X__XXX_X, XX_X_XXX, __X_X___,
  XXXXX___, _____X_X, ________, XXX_XXXX, ______XX, _XXX_X__,
};

/** $761E: stage1_bitmap_tree_trunk_24x3s */
static const pixel_t stage1_bitmap_tree_trunk_24x3s[3 * 2 * 3 * 1] = {
  XXXXXXXX, ________, XXXX____, ____X__X, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ___XX_XX, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ___X_X_X, XXXXXXXX, ________,
};

/** $7630: stage1_bitmap_tree_shadow_24x1s */
static const pixel_t stage1_bitmap_tree_shadow_24x1s[3 * 2 * 1 * 1] = {
  XXXX____, ____XXXX, ________, XXX__XX_, ________, XXXXXXXX,
};

/* ----------------------------------------------------------------------- */

const map_goto_entry_t stage1_map_goto_table[24] = {
  { STAGE1_MAP_LEFT_CURVATURE_ADDR,   &stage1_map_left_curvature[0]   },
  { STAGE1_MAP_LEFT_HEIGHT_ADDR,      &stage1_map_left_height[0]      },
  { STAGE1_MAP_LEFT_LANES_ADDR,       &stage1_map_left_lanes[0]       },
  { STAGE1_MAP_LEFT_HAZARDS_ADDR,     &stage1_map_left_hazards[0]     },
  { STAGE1_MAP_LEFT_LEFTOBJS_ADDR,    &stage1_map_left_leftobjs[0]    },
  { STAGE1_MAP_LEFT_RIGHTOBJS_ADDR,   &stage1_map_left_rightobjs[0]   },
  { STAGE1_MAP_RIGHT_CURVATURE_ADDR,  &stage1_map_right_curvature[0]  },
  { STAGE1_MAP_RIGHT_HEIGHT_ADDR,     &stage1_map_right_height[0]     },
  { STAGE1_MAP_RIGHT_LANES_ADDR,      &stage1_map_right_lanes[0]      },
  { STAGE1_MAP_RIGHT_HAZARDS_ADDR,    &stage1_map_right_hazards[0]    },
  { STAGE1_MAP_RIGHT_LEFTOBJS_ADDR,   &stage1_map_right_leftobjs[0]   },
  { STAGE1_MAP_RIGHT_RIGHTOBJS_ADDR,  &stage1_map_right_rightobjs[0]  },
  { STAGE1_MAP_TUNNEL_CURVATURE_ADDR, &stage1_map_tunnel_curvature[0] },
  { STAGE1_MAP_TUNNEL_HEIGHT_ADDR,    &stage1_map_tunnel_height[0]    },
  { STAGE1_MAP_TUNNEL_LANES_ADDR,     &stage1_map_tunnel_lanes[0]     },
  { STAGE1_MAP_TUNNEL_HAZARDS_ADDR,   &stage1_map_tunnel_hazards[0]   },
  { STAGE1_MAP_TUNNEL_LEFTOBJS_ADDR,  &stage1_map_tunnel_leftobjs[0]  },
  { STAGE1_MAP_TUNNEL_RIGHTOBJS_ADDR, &stage1_map_tunnel_rightobjs[0] },
  { STAGE1_MAP_LOOP_CURVATURE_ADDR,   &stage1_map_loop_curvature[0]   },
  { STAGE1_MAP_LOOP_HEIGHT_ADDR,      &stage1_map_loop_height[0]      },
  { STAGE1_MAP_LOOP_LANES_ADDR,       &stage1_map_loop_lanes[0]       },
  { STAGE1_MAP_LOOP_HAZARDS_ADDR,     &stage1_map_loop_hazards[0]     },
  { STAGE1_MAP_LOOP_LEFTOBJS_ADDR,    &stage1_map_loop_leftobjs[0]    },
  { STAGE1_MAP_LOOP_RIGHTOBJS_ADDR,   &stage1_map_loop_rightobjs[0]   },
};

// clang-format on
