/**
 * ChaseHQ-Stage1Data.c
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
 * The original game and design is copyright (c) 1988 Taito Corporation
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited
 * The recreated version is copyright (c) 2023-2026 David Thomas
 */

#include <assert.h>
#include <stddef.h>

#include "C99/Types.h"
#include "ZXSpectrum/Pixels.h"
#include "ZXSpectrum/Spectrum.h"

#include "../ChaseHQ.h"
#include "ChaseHQ-CommonData.h"

#include "ChaseHQ-Stage1Data.h"

/* ----------------------------------------------------------------------- */

static const u8 stage1_perp_description[7];
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
static const u8 stage1_map_left_curvature[];
static const u8 stage1_map_left_height[];
static const u8 stage1_map_left_lanes[];
static const u8 stage1_map_left_hazards[];
static const u8 stage1_map_left_leftobjs[];
static const u8 stage1_map_left_rightobjs[];
static const u8 stage1_map_right_curvature[];
static const u8 stage1_map_right_height[];
static const u8 stage1_map_right_lanes[];
static const u8 stage1_map_right_hazards[];
static const u8 stage1_map_right_leftobjs[];
static const u8 stage1_map_right_rightobjs[];
static const u8 stage1_map_tunnel_curvature[];
static const u8 stage1_map_tunnel_height[];
static const u8 stage1_map_tunnel_lanes[];
static const u8 stage1_map_tunnel_hazards[];
static const u8 stage1_map_tunnel_leftobjs[];
static const u8 stage1_map_tunnel_rightobjs[];
static const u8 stage1_map_loop_curvature[];
static const u8 stage1_map_loop_height[];
static const u8 stage1_map_loop_lanes[];
static const u8 stage1_map_loop_hazards[];
static const u8 stage1_map_loop_leftobjs[];
static const u8 stage1_map_loop_rightobjs[];
static const u8 stage1_perp_face[FACEBYTES];
static const bitmap_t stage1_lambo_bitmaps[SPRITE_FRAMES];
static const bitmap_t stage1_truck_bitmaps[SPRITE_FRAMES];
static const bitmap_t stage1_car_bitmaps[SPRITE_FRAMES];
static const u8 stage1_bitmap_lambo_1[6 * 30];
static const u8 stage1_bitmap_lambo_2[5 * 22];
static const u8 stage1_bitmap_lambo_3[3 * 15];
static const u8 stage1_bitmap_truck_1[6 * 39];
static const u8 stage1_bitmap_truck_2[5 * 29];
static const u8 stage1_bitmap_truck_3[3 * 20];
static const u8 stage1_bitmap_car_1[6 * 31];
static const u8 stage1_bitmap_car_2[5 * 22];
static const u8 stage1_bitmap_car_3[3 * 16];
static const u8 stage1_bitmap_lambo_4[3 * 2 * 8];
static const u8 stage1_bitmap_lambo_4s[3 * 2 * 8];
static const u8 stage1_bitmap_truck_4[2 * 2 * 12];
static const u8 stage1_bitmap_truck_4s[2 * 2 * 12];
static const u8 stage1_bitmap_car_4[3 * 2 * 9];
static const u8 stage1_bitmap_car_4s[3 * 2 * 9];
static const bitmap_t stage1_stones_bitmaps[SPRITE_FRAMES];
static const bitmap_t stage1_dust_bitmaps[SPRITE_FRAMES];
static const u8 stage1_bitmap_stones_1[2 * 2 * 5];
static const u8 stage1_bitmap_stones_2[2 * 2 * 4];
static const u8 stage1_bitmap_stones_2s[2 * 2 * 4];
static const u8 stage1_bitmap_stones_3[2 * 2 * 3];
static const u8 stage1_bitmap_stones_3s[2 * 2 * 3];
static const u8 stage1_bitmap_stones_4[1 * 2 * 2];
static const u8 stage1_bitmap_stones_4s[1 * 2 * 2];
static const u8 stage1_bitmap_stones_5[1 * 2 * 1];
static const u8 stage1_bitmap_stones_5s[1 * 2 * 1];
static const u8 stage1_bitmap_dust_1[1 * 2 * 1];
static const u8 stage1_bitmap_dust_1s[1 * 2 * 1];
static const depthset_t stage1_turn_sign_right;
static const depthset_t stage1_turn_sign_left;
static const bitmap_t stage1_turn_sign_bitmaps[];
static const u8 stage1_bitmap_turnsign_1[4 * 40];
static const u8 stage1_bitmap_turnsign_2[3 * 30];
static const u8 stage1_bitmap_turnsign_3[2 * 20];
static const u8 stage1_bitmap_turnsign_4[2 * 2 * 16];
static const u8 stage1_bitmap_turnsign_5[2 * 2 * 13];
static const u8 stage1_bitmap_turnsign_5s[2 * 2 * 13];
static const bitmap_t stage1_tumbleweed_bitmaps[SPRITE_FRAMES];
static const u8 stage1_bitmap_tumbleweed_1[2 * 16];
static const u8 stage1_bitmap_tumbleweed_2[2 * 11];
static const u8 stage1_bitmap_tumbleweed_3[1 * 9];
static const u8 stage1_bitmap_tumbleweed_4[1 * 7];
static const bitmap_t stage1_barrier_bitmaps[SPRITE_FRAMES];
static const u8 stage1_bitmap_barrier_1[4 * 17];
static const u8 stage1_bitmap_barrier_2[3 * 13];
static const u8 stage1_bitmap_barrier_3[2 * 9];
static const u8 stage1_bitmap_barrier_4[2 * 2 * 7];
static const u8 stage1_bitmap_barrier_4s[2 * 2 * 7];
static const stretchy_t stage1_stretchy_streetlamp_right[5];
static const stretchy_t stage1_stretchy_streetlamp_left[5];
static const depthset_t stage1_streetlamptop_right;
static const depthset_t stage1_streetlamptop_left;
static const bitmap_t stage1_streetlamptop_bitmaps[5];
static const bitmap_t stage1_streetlamptop_flipped_bitmaps[5];
static const u8 stage1_bitmap_streetlamptop_1[4 * 8];
static const u8 stage1_bitmap_streetlamptop_2[3 * 5];
static const u8 stage1_bitmap_streetlamptop_3[3 * 4];
static const u8 stage1_bitmap_streetlamptop_3s[3 * 4];
static const u8 stage1_bitmap_streetlamptop_4[2 * 2 * 4];
static const u8 stage1_bitmap_streetlamptop_4s[2 * 2 * 4];
static const u8 stage1_bitmap_streetlamptop_5[2 * 2 * 4];
static const u8 stage1_bitmap_streetlamptop_5s[2 * 2 * 3];
static const stretchy_t stage1_stretchy_telegraphpole_right[5];
static const stretchy_t stage1_stretchy_telegraphpole_left[5];
static const depthset_t stage1_telegraphpoletop_left;
static const depthset_t stage1_telegraphpoletop_right;
static const bitmap_t stage1_telegraphpoletop_bitmaps[5];
static const u8 stage1_bitmap_telegraphpoletop_1[3 * 13];
static const u8 stage1_bitmap_telegraphpoletop_2[3 * 10];
static const u8 stage1_bitmap_telegraphpoletop_3[3 * 7];
static const u8 stage1_bitmap_telegraphpoletop_3s[3 * 7];
static const u8 stage1_bitmap_telegraphpoletop_4[2 * 2 * 5];
static const u8 stage1_bitmap_telegraphpoletop_4s[2 * 2 * 5];
static const u8 stage1_bitmap_telegraphpoletop_5[2 * 2 * 4];
static const u8 stage1_bitmap_telegraphpoletop_5s[2 * 2 * 4];
static const stretchy_t stage1_stretchy_tree_right[SPRITE_FRAMES];
static const stretchy_t stage1_stretchy_tree_left[4];
static const depthset_t stage1_tree_bottom;
static const depthset_t stage1_tree_bottommiddle;
static const depthset_t stage1_tree_middle;
static const depthset_t stage1_tree_topmiddle;
static const depthset_t stage1_tree_top;
static const bitmap_t stage1_tree_bitmaps[25];
static const u8 stage1_bitmap_tree_middle_64x16[8 * 16];
static const u8 stage1_bitmap_tree_bottom_64x5[8 * 5];
static const u8 stage1_bitmap_tree_trunk_16x8[2 * 8];
static const u8 stage1_bitmap_tree_shadow_64x5[8 * 5];
static const u8 stage1_bitmap_tree_middle_48x12[6 * 12];
static const u8 stage1_bitmap_tree_bottom_48x4[6 * 4];
static const u8 stage1_bitmap_tree_shadow_48x4[6 * 4];
static const u8 stage1_bitmap_tree_middle_32x8[4 * 8];
static const u8 stage1_bitmap_tree_bottom_32x3[4 * 3];
static const u8 stage1_bitmap_tree_shadow_32x2[4 * 2];
static const u8 stage1_bitmap_tree_middle_24x7[3 * 7];
static const u8 stage1_bitmap_tree_bottom_24x2[3 * 2];
static const u8 stage1_bitmap_tree_trunk_8x4[1 * 4];
static const u8 stage1_bitmap_tree_shadow_24x2[3 * 2];
static const u8 stage1_bitmap_tree_top_64x13[8 * 2 * 13];
static const u8 stage1_bitmap_tree_top_48x10[6 * 2 * 10];
static const u8 stage1_bitmap_tree_top_32x5[4 * 2 * 5];
static const u8 stage1_bitmap_tree_top_24x4[3 * 2 * 4];
static const u8 stage1_bitmap_tree_top_24x3[3 * 2 * 3];
static const u8 stage1_bitmap_tree_top_24x3s[3 * 2 * 3];
static const u8 stage1_bitmap_tree_trunk_16x6[2 * 2 * 6];
static const u8 stage1_bitmap_tree_trunk_16x4[2 * 2 * 4];
static const u8 stage1_bitmap_tree_middle_24x5[3 * 2 * 5];
static const u8 stage1_bitmap_tree_bottom_24x2_another[3 * 2 * 2];
static const u8 stage1_bitmap_tree_trunk_24x3[3 * 2 * 3];
static const u8 stage1_bitmap_tree_shadow_24x1[3 * 2 * 1];
static const u8 stage1_bitmap_tree_middle_24x5s[3 * 2 * 5];
static const u8 stage1_bitmap_tree_bottom_24x2s[3 * 2 * 2];
static const u8 stage1_bitmap_tree_trunk_24x3s[3 * 2 * 3];
static const u8 stage1_bitmap_tree_shadow_24x1s[3 * 2 * 1];

/* ----------------------------------------------------------------------- */

// $5CF0
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
  NULL, // no bitmap given on this level
  attribute_BRIGHT_YELLOW_OVER_BLACK | (attribute_BRIGHT_YELLOW_OVER_BLACK << 8),
  &stage1_hittable_object_defs[0],
  &stage1_right_hand_graphics_defs[-1].arg,
  &stage1_right_hand_graphics_defs[-1],
  &stage1_right_hand_graphics_defs[2], // short pole
  &stage1_left_hand_graphics_defs[-1].arg,
  &stage1_left_hand_graphics_defs[-1],
  &stage1_left_hand_graphics_defs[2], // short pole
  &stage1_perp_description[0],
  &stage1_arrest_messages[0],
  NULL, // unused on this level - addrof_helicopter_stuff_1
  NULL, // unused on this level - addrof_helicopter_stuff_2 - Conv: was &000C

  &stage1_stones_bitmaps,
  &stage1_dust_bitmaps,
  &stage1_car_bitmaps[0],
  {
    &stage1_lambo_bitmaps[0],
    &stage1_truck_bitmaps[0],
    &stage1_lambo_bitmaps[0],
    &stage1_car_bitmaps[0]
  },

  20, // car_spawn_delay
  80, // perp_lane_change_base
  90, // perp_approach_base

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

// $5D39
static const u8 stage1_perp_description[7] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_PERP_DESC_1,
  CHATTERSTR_PERP_DESC_2,
  CHATTERSTR_PERP_DESC_3,
  CHATTERSTR_PERP_DESC_4,
  CHATTERCMD_PAUSE,
  CHATTERBLK_HEROES_ACKNOWLEDGE
};

// Conv: Additional
static const char *stage1_chatter_strings[4] = {
  "THIS IS NANCY AT CHASE H.Q. WE'VE GOT A\xCE",
  "EMERGENCY HERE. RALPH THE IDAHO SLASHER\xAC",
  "IS FLEEING TOWARDS THE SUBURBS. THE TARGE\xD4",
  "VEHICLE IS A WHITE BRITISH SPORTS CAR... OVER\xAE"
};

/* ----------------------------------------------------------------------- */

// $5DED
static const u8 stage1_arrest_messages[83] = {
  6, // initial delay

  6, // delay
  DRAWCHARSTYLE_SINGLE,
  0, // partial attribute?
  TWOBYTES(0xF802), // backbuf (16,8)
  TWOBYTES(0x5922), // attr (2,9)
  'O', 'K', '!', ' ', 'Y', 'O', 'U', ' ', 'A', 'R', 'E', ' ', 'U', 'N', 'D', 'E', 'R', ' ', 'A', 'R', 'R', 'E', 'S', 'T', ' ', 'O', 'N' | EOS,

  6, // delay
  DRAWCHARSTYLE_SINGLE,
  0, // partial attribute?
  TWOBYTES(0xF023), // backbuf (24,16)
  TWOBYTES(0x5943), // attr (3,10)
  'S', 'U', 'S', 'P', 'I', 'C', 'I', 'O', 'N', ' ', 'O', 'F', ' ', 'F', 'I', 'R', 'S', 'T', ' ', 'D', 'E', 'G', 'R', 'E', 'E', ' ' | EOS,

  45, // delay
  DRAWCHARSTYLE_SINGLE,
  0, // partial attribute?
  TWOBYTES(0xF82C), // backbuf (96,24)
  TWOBYTES(0x596C), // attr (12,11)
  'M', 'U', 'R', 'D', 'E', 'R' | EOS,

  TRANSITIONCONTROL_FILL_ATTRIBUTES, // new value for transition_control
  DRAWOVERLAY_STOP // terminator
};

/* ----------------------------------------------------------------------- */

// $5E40
static const hittable_t stage1_hittable_object_defs[2] = {
  { 16, &stage1_tumbleweed_bitmaps[0] },
  { 32, &stage1_barrier_bitmaps[0]    },
};

// $5E46
static const obj_t stage1_right_hand_graphics_defs[9] = {
  { 111, 41, 80, &tunnellight,                         draw_tunnel_light_right    },
  {   0,  0,  0, NULL,                                 NULL },
  { 144, 92, 40, &stretchy_shortpole,                  draw_stretchy_object_right },
  {  94, 36, 60, &stage1_stretchy_tree_right,          draw_stretchy_object_right },
  { 110, 49, 80, &stage1_stretchy_tree_left,           draw_stretchy_object_right },
  { 110, 53, 80, &stage1_stretchy_streetlamp_right,    draw_stretchy_object_right },
  { 110, 53, 80, &stage1_stretchy_telegraphpole_right, draw_stretchy_object_right },
  { 100, 24, 70, &stage1_turn_sign_left,               draw_object_right          },
  { 100, 24, 70, &stage1_turn_sign_right,              draw_object_right          }
};

// $5E85
static const obj_t stage1_left_hand_graphics_defs[9] = {
  { 126, 188, 80, &tunnellight,                        draw_tunnel_light_left    },
  {   0,  0,  0,  NULL,                                NULL },
  {  96, 144, 40, &stretchy_shortpole,                 draw_stretchy_object_left },
  { 140, 196, 60, &stage1_stretchy_tree_right,         draw_stretchy_object_left },
  { 124, 208, 80, &stage1_stretchy_tree_left,          draw_stretchy_object_left },
  { 132, 182, 80, &stage1_stretchy_streetlamp_left,    draw_stretchy_object_left },
  { 132, 182, 80, &stage1_stretchy_telegraphpole_left, draw_stretchy_object_left },
  { 129, 182, 70, &stage1_turn_sign_left,              draw_object_left          },
  { 129, 200, 70, &stage1_turn_sign_right,             draw_object_left          },
};

/* ----------------------------------------------------------------------- */

// $5EC4
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
  MAP_CMD_SPLIT(0x5FD2, 0x6088)
};

// 0x5EE4
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
  MAP_CMD_SPLIT(0x5FE6, 0x60A3)
};

// 0x5F0F
static const u8 stage1_map_start_lanes[14] = {
  MAP_LANES_3L(30),
  MAP_LANES_3TO4L(2),
  MAP_LANES_4(254),
  MAP_LANES_4(34),
  MAP_CMD_SPLIT(0x6003, 0x60E5)
};

// 0x5F1D
static const u8 stage1_map_start_hazards[13] = {
  MAP_HAZARD_WAIT(147),
  MAP_CMD_STOP_CARS,
  MAP_HAZARD_WAIT(12),
  MAP_CMD_ARROW_R,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_SPLIT(0x6009, 0x60EF)
};

// 0x5F2A
static const u8 stage1_map_start_leftobjs[78] = {
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(11),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_CMD_SPLIT(0x6011, 0x6109)
};

// 0x5F78
static const u8 stage1_map_start_rightobjs[90] = {
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(9),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_CMD_SPLIT(0x6052, 0x6143)
};

/* ----------------------------------------------------------------------- */

// $5FD2
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
  MAP_CMD_GOTO(0x6173) // map_tunnel_curvature
};

// 0x5FE6
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
  MAP_CMD_GOTO(0x6189) // map_tunnel_height
};

// 0x6003
static const u8 stage1_map_left_lanes[] = {
  MAP_LANES_4(210),
  MAP_CMD_GOTO(0x61A5) // map_tunnel_lanes
};

// 0x6009
static const u8 stage1_map_left_hazards[] = {
  MAP_HAZARD_WAIT(10),
  MAP_CMD_START_CARS,
  MAP_HAZARD_WAIT(95),
  MAP_CMD_GOTO(0x61B9) // map_tunnel_hazards
};

// 0x6011
static const u8 stage1_map_left_leftobjs[] = {
  MAP_OBJ_S1_NONE(4),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(15),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(2),
  MAP_CMD_GOTO(0x61BE)
};

// 0x6052
static const u8 stage1_map_left_rightobjs[] = {
  MAP_OBJ_S1_NONE(15),
  MAP_OBJ_S1_NONE(15),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(10),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_CMD_GOTO(0x61C9)
};

/* ----------------------------------------------------------------------- */

// 0x6088
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
  MAP_CMD_GOTO(0x6173)
};

// $60A3
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
  MAP_CMD_GOTO(0x6189)
};

// $60E5
static const u8 stage1_map_right_lanes[] = {
  MAP_LANES_4(20),
  MAP_LANES_DIRTTRACK(178),
  MAP_LANES_4(12),
  MAP_CMD_GOTO(0x61A5)
};

// $60EF
static const u8 stage1_map_right_hazards[] = {
  MAP_HAZARD_WAIT(10),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_UNKNOWN_HAZARD_6,
  MAP_HAZARD_WAIT(41),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(4),
  MAP_CMD_UNKNOWN_HAZARD_6,
  MAP_HAZARD_WAIT(42),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_START_CARS,
  MAP_HAZARD_WAIT(4),
  MAP_CMD_GOTO(0x61B9)
};

// $6109
static const u8 stage1_map_right_leftobjs[] = {
  MAP_OBJ_S1_NONE(10),
  MAP_OBJ_S1_SHORT_POLE(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(13),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_CMD_GOTO(0x61BE)
};

// $6143
static const u8 stage1_map_right_rightobjs[] = {
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_SHORT_POLE(1),
  MAP_OBJ_S1_NONE(11),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(9),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(15),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(13),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(8),
  MAP_CMD_GOTO(0x61C9)
};

/* ----------------------------------------------------------------------- */

// $6173
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
  MAP_CMD_GOTO(0x61D4)
};

// $6189
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
  MAP_CMD_GOTO(0x6209)
};

// $61A5
static const u8 stage1_map_tunnel_lanes[] = {
  MAP_LANES_4(44),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(12),
  MAP_LANES_TUNNEL(102),
  MAP_LANES_TUNNEL_EXIT(2),
  MAP_LANES_3L(18),
  MAP_LANES_3TO4L(2),
  MAP_LANES_4(8),
  MAP_CMD_GOTO(0x625D)
};

// $61B9
static const u8 stage1_map_tunnel_hazards[] = {
  MAP_HAZARD_WAIT(95),
  MAP_CMD_GOTO(0x6277)
};

// $61BE
static const u8 stage1_map_tunnel_leftobjs[] = {
  MAP_OBJ_S1_NONE(15),
  MAP_OBJ_S1_NONE(15),
  MAP_OBJ_S1_TUNNEL_LIGHT(15),
  MAP_OBJ_S1_TUNNEL_LIGHT(15),
  MAP_OBJ_S1_TUNNEL_LIGHT(15),
  MAP_OBJ_S1_TUNNEL_LIGHT(6),
  MAP_OBJ_S1_NONE(14),
  MAP_CMD_GOTO(0x6294)
};

// $61C9
static const u8 stage1_map_tunnel_rightobjs[] = {
  MAP_OBJ_S1_NONE(15),
  MAP_OBJ_S1_NONE(15),
  MAP_OBJ_S1_TUNNEL_LIGHT(15),
  MAP_OBJ_S1_TUNNEL_LIGHT(15),
  MAP_OBJ_S1_TUNNEL_LIGHT(15),
  MAP_OBJ_S1_TUNNEL_LIGHT(6),
  MAP_OBJ_S1_NONE(14),
  MAP_CMD_GOTO(0x6314)
};

/* ----------------------------------------------------------------------- */

// $61D4
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
  MAP_CMD_GOTO(0x61D4)
};

// $6209
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
  MAP_CMD_GOTO(0x6209)
};

// $625D
static const u8 stage1_map_loop_lanes[] = {
  MAP_LANES_4(254),
  MAP_LANES_4(36),
  MAP_LANES_4TO3R(34),
  MAP_LANES_3TO4R(2),
  MAP_LANES_4(30),
  MAP_LANES_3L(10),
  MAP_LANES_3TO4L(2),
  MAP_LANES_4(22),
  MAP_LANES_4TO3R(6),
  MAP_LANES_3TO4R(2),
  MAP_LANES_4(52),
  MAP_CMD_GOTO(0x625D)
};

// 6277
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
  MAP_CMD_GOTO(0x6277)
};

// $6294
static const u8 stage1_map_loop_leftobjs[] = {
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(6),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(9),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_S1_NONE(15),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(11),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(9),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_CMD_GOTO(0x6294)
};

// $6314
static const u8 stage1_map_loop_rightobjs[] = {
  MAP_OBJ_S1_NONE(2),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(15),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TELEGRAPH_POLE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_L(1),
  MAP_OBJ_S1_NONE(7),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(9),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(15),
  MAP_OBJ_S1_NONE(6),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_BUSH(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(11),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TREE(1),
  MAP_OBJ_S1_NONE(9),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(3),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_TURN_SIGN_R(1),
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_STREET_LAMP(1),
  MAP_OBJ_S1_NONE(2),
  MAP_CMD_GOTO(0x6314)
};

/* ----------------------------------------------------------------------- */

// $642A
static const u8 stage1_perp_face[FACEBYTES] = {
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

// clang-format off
  attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_CYAN
// clang-format on
};

/* ----------------------------------------------------------------------- */

// $643E
static const bitmap_t stage1_lambo_bitmaps[SPRITE_FRAMES] = {
  { 6, BITMAPFLAG_DEFAULT, 30, &stage1_bitmap_lambo_1[0], &stage1_bitmap_lambo_1[0]  },
  { 5, BITMAPFLAG_DEFAULT, 22, &stage1_bitmap_lambo_2[0], &stage1_bitmap_lambo_2[0]  },
  { 3, BITMAPFLAG_DEFAULT, 15, &stage1_bitmap_lambo_3[0], &stage1_bitmap_lambo_3[0]  },
  { 3, BITMAPFLAG_DEFAULT, 15, &stage1_bitmap_lambo_3[0], &stage1_bitmap_lambo_3[0]  },
  { 3, BITMAPFLAG_MASKED,   8, &stage1_bitmap_lambo_4[0], &stage1_bitmap_lambo_4[0]  },
  { 3, BITMAPFLAG_MASKED,   8, &stage1_bitmap_lambo_4[0], &stage1_bitmap_lambo_4s[0] },
};

// $6468
static const bitmap_t stage1_truck_bitmaps[SPRITE_FRAMES] = {
  { 6, BITMAPFLAG_DEFAULT, 39, &stage1_bitmap_truck_1[0], &stage1_bitmap_truck_1[0]  },
  { 5, BITMAPFLAG_DEFAULT, 29, &stage1_bitmap_truck_2[0], &stage1_bitmap_truck_2[0]  },
  { 3, BITMAPFLAG_DEFAULT, 20, &stage1_bitmap_truck_3[0], &stage1_bitmap_truck_3[0]  },
  { 3, BITMAPFLAG_DEFAULT, 20, &stage1_bitmap_truck_3[0], &stage1_bitmap_truck_3[0]  },
  { 2, BITMAPFLAG_MASKED,  12, &stage1_bitmap_truck_4[0], &stage1_bitmap_truck_4s[0] },
  { 2, BITMAPFLAG_MASKED,  12, &stage1_bitmap_truck_4[0], &stage1_bitmap_truck_4s[0] },
};

// $6492
static const bitmap_t stage1_car_bitmaps[SPRITE_FRAMES] = {
  { 6, BITMAPFLAG_DEFAULT, 31, &stage1_bitmap_car_1[0], &stage1_bitmap_car_1[0]  },
  { 5, BITMAPFLAG_DEFAULT, 22, &stage1_bitmap_car_2[0], &stage1_bitmap_car_2[0]  },
  { 3, BITMAPFLAG_DEFAULT, 16, &stage1_bitmap_car_3[0], &stage1_bitmap_car_3[0]  },
  { 3, BITMAPFLAG_DEFAULT, 16, &stage1_bitmap_car_3[0], &stage1_bitmap_car_3[0]  },
  { 3, BITMAPFLAG_MASKED,   9, &stage1_bitmap_car_4[0], &stage1_bitmap_car_4s[0] },
  { 3, BITMAPFLAG_MASKED,   9, &stage1_bitmap_car_4[0], &stage1_bitmap_car_4s[0] },
};

// $64BC
static const u8 stage1_bitmap_lambo_1[6 * 30] = {
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  _XXXXXX_, _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, __XXXXXX,
  _XXXX__X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX__XXXX,
  __XXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXX_,
  ___XXXX_, X_X_X_XX, X_X_X_X_, X_X_X_X_, XXX_X_X_, X_XXXX__,
  ___X_X__, _____XXX, XXXXXXXX, XXXXXXXX, XXXX____, ___X_X__,
  __X___X_, X_XXXXXX, _XX_XXXX, XXXXXXX_, XX_XXXX_, X_X___X_,
  __X_XXXX, XXXXXXX_, _X__XXX_, X_X_XX__, X__XXXXX, XXXXX_X_,
  __XXXXXX, XXXXXXXX, XXXXXX_X, _X_X_XXX, XXXXXXXX, XXXXXXX_,
  __XXXXXX, XXXX_XXX, X_X__X__, ____X_X_, XXXXXXXX, XXXXXXX_,
  __XX_XXX, __X_X_X_, X_______, ________, X_XXXXXX, XXXXXXX_,
  __X_X_X_, _X_X_XXX, ________, ________, _XX____X, XXXXXXX_,
  __X___X_, XXXXXXXX, _X_X_X_X, _X_X_X_X, _X_XXXX_, _XXXXXX_,
  __X__X_X, XX____XX, X_X_X_X_, X_X_X_X_, X_X____X, X__XX_X_,
  _X_X_XXX, __X_X__X, XXXXXXXX, XXXXXXXX, XX__X_X_, _XX_XX_X,
  _XX_XX__, _______X, __X_X_X_, X_X_X_X_, _X______, ___XX_XX,
  _XXX_X__, _______X, __XX_X_X, _X_X_XX_, _X______, ___XXXXX,
  _XXX_XXX, _X_X_X_X, XXXXXXXX, XXXXXXXX, XX_X_X_X, _XXXXXXX,
  __XXX__X, XX____XX, X_______, ________, XXX____X, XX_XXXX_,
  ____X___, _XXXXXXX, XXX_X_X_, X_X_X_XX, XXXXXXXX, ____X___,
  ____XX_X, ______XX, XXXXXXXX, XXXXXXXX, XXX_____, _X_XX___,
  ______XX, X_XX___X, _X_X_X_X, _X_X_X_X, _X___XX_, XXX_____,
  __XXX___, XXXXXXXX, XXX_____, ______XX, XXXXXXXX, X___XXX_,
  _X__XX__, _XXX_X_X, _XXXXXXX, XXXXXXXX, _X_X_XXX, ___XX__X,
  _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X,
  _X__X___, ________, ________, ________, ________, ____X__X,
  _XX__XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX__XX,
  ___XXX__, ________, ________, ________, ________, ___XXX__,
};

// $6570
static const u8 stage1_bitmap_lambo_2[5 * 22] = {
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  __XX__X_, XXXXXXXX, XXXXXXXX, XXXXXXX_, X__XXX__,
  __XX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XX___,
  ___XX___, ___XXXXX, XXXXXXXX, XXXX____, __XX____,
  __X__X_X, _XXXX_XX, _XXXXXXX, _XX_XX_X, _X__X___,
  __X_XXXX, XXXX__X_, _XX__XX_, _X__XXXX, XXX_X___,
  __XXXXXX, _X_XXXXX, XX____XX, XXXXXXXX, XXXXX___,
  __XX_XX_, __X__X_X, _______X, _X_XXXXX, XXXXX___,
  __X_X___, _XXXX___, ________, __XXXXXX, XXXXX___,
  __X____X, X___XX_X, _X_X_X_X, _XX___XX, X_X_X___,
  ___X_XX_, __X_XXXX, XXXXXXXX, XXX_X___, XX_X_X__,
  __XX_X__, ____X__X, _X_X_X_X, __X_____, _XXXXX__,
  __XX_XXX, ___XXXXX, XXXXXXXX, XXXX___X, XXXXXX__,
  ____X___, XXXXXXX_, X_X_X_X_, XXXXXXX_, ___X____,
  _____XX_, ____XXXX, XXXXXXXX, XXXX____, _XX_____,
  ______XX, _XX_XXXX, _X__X__X, _XXX_XX_, XX______,
  __X__X__, XX_XX_XX, XXXXXXXX, XX_XX_XX, __X__X__,
  __X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_X__,
  __XX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XX__,
  ___XX___, ________, ________, ________, __XX____,
};

// $65DE
static const u8 stage1_bitmap_lambo_3[3 * 15] = {
  _XXXXXXX, XXXXXXXX, XXXXXXX_,
  XXX__XXX, XXXXXXXX, XXX__XXX,
  XX_XXXXX, XXXXXXXX, XXXXX_XX,
  _XX___XX, XXXXXXXX, XX___XX_,
  XX_XXXXX, _X_XX_X_, XXXXX_X_,
  XXXXXXXX, XXX__XXX, XXXXXXXX,
  XX__XXXX, ________, _XXXXXXX,
  X__XX__X, XXXXXXXX, X__XXXXX,
  X_X____X, ________, X____XXX,
  X_XXX_XX, XXXXXXXX, XX_XXXXX,
  _XX__XX_, XXXXXXXX, _XXX_XX_,
  ___X___X, X_X__X_X, X_XXX___,
  XXX_X_X_, XXXXXXXX, XXXX_XXX,
  X_XXXXXX, XXXXXXXX, XXXXXX_X,
  _XX_____, ________, _____XX_,
};

// $660B
static const u8 stage1_bitmap_truck_1[6 * 39] = {
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  _XXXXX__, X_X_XXXX, XXXXXXXX, XXXXXXXX, XXXX_X_X, __XXXXX_,
  _XXXX__X, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, X__XXXX_,
  _XXXX_XX, XXXXXXXX, XX______, _______X, XXXXXXXX, XX_XXXX_,
  __XXX_XX, XXXXXXXX, XX______, _______X, XXXXXXXX, XX_XXX__,
  __XXX_XX, XXXXX___, XXX_____, ______XX, X___XXXX, XX_XXX__,
  ___XX_XX, XXXX_X_X, XX_X_X_X, _X_X_X_X, XX_X_XXX, XX_XX___,
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  ___X____, ________, ________, ________, ________, ____X___,
  __X_X_X_, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, _X_X_X__,
  __XX_X_X, _X___X_X, _X_X_X_X, _X_X_X_X, _X_X__X_, X_X_XX__,
  __X_XXX_, ________, ________, ________, ________, _XXX_X__,
  __XXXX__, ________, ________, ________, ________, __XXXX__,
  _X___XX_, ________, ________, ________, ________, _XX___X_,
  _X___X__, ________, ________, ________, ________, __X___X_,
  _X___XX_, ________, ________, ________, ________, _XX___X_,
  __XXXXX_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_XXXXX_,
  _X___XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX___X_,
  _X______, ________, ________, ________, ________, ______X_,
  _X______, ________, ________, ________, ________, ______X_,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  ___XXXX_, _XX_X_X_, X_X_X_XX, X_X_X_X_, X_X_X_X_, _XXXX___,
  ______XX, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, XX______,
  ______XX, XX______, XX_____X, X_____XX, ______XX, XX______,
  _____XX_, __XXXXXX, __XXXXX_, _XXXXX__, XXXXXX__, _XX_____,
  ____X_XX, X_X_X_XX, __X_X_X_, _X_X_X__, XX_X_X_X, XX_X____,
  ____X__X, _X_XXXXX, __XXXXX_, _XXXXX__, XXXXX_X_, X__X____,
  ___X___X, _X_XXXXX, __XXXXX_, _XXXXX__, XXXXX_X_, X___X___,
  ___X___X, _X_XXXXX, __XXXXX_, _XXXXX__, XXXXX_X_, X___X___,
  ___X___X, __X_XXXX, __XXXXX_, _XXXXX__, XXXX_X__, X___X___,
  ___X___X, X_X_XXXX, __XXXXX_, _XXXXX__, XXXX_X_X, X___X___,
  ____X_X_, _XX_XXXX, __XXXXX_, _XXXXX__, XXXX_XX_, _X_X____,
  _____XXX, X__X_XXX, __XXXXX_, _XXXXX__, XXX_X__X, XXX_____,
  ________, _XXX____, ________, ________, ____XXX_, ________,
  ________, ____X___, ________, ________, ___X____, ________,
  ________, _____XXX, XXXXXXXX, XXXXXXXX, XXX_____, ________,
};

// $66F5
static const u8 stage1_bitmap_truck_2[5 * 29] = {
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  ___XXX_X, _XXXXXXX, XXXXXXXX, XXXXXXX_, X_XXXX__,
  ___XX_XX, XXXXXXXX, ________, _XXXXXXX, XX_XXX__,
  ___XX_XX, XXXX__XX, ________, _XX__XXX, XX_XX___,
  ____X_XX, XXX_XXXX, X_X___X_, XXXXX_XX, XX_X____,
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  ____X___, ________, ________, ________, ___X____,
  ___X_XX_, X_XXXXXX, XXXXXXXX, XXXXXX_X, _XX_X___,
  ___X_X_X, _X______, ________, ______X_, X_X_X___,
  ___XXX__, ________, ________, ________, __XXX___,
  __X___X_, ________, ________, ________, _X___X__,
  __X___X_, ________, ________, ________, _X___X__,
  __XXXXX_, X_X_X_X_, X_X__X_X, _X_X_X_X, __XXXX__,
  __X_____, ________, ________, ________, _____X__,
  __X_____, ________, ________, ________, _____X__,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  ______XX, X_XXXXXX, XXXXXXXX, XXXXXX_X, XX______,
  ______XX, XX____X_, ___XX___, _X____XX, XX______,
  _____XXX, X__X_X_X, X_X__X_X, X_X_X__X, XXX_____,
  _____X_X, __XXXX_X, XXX__XXX, X_XXXX__, X_X_____,
  ____X__X, __XXXX_X, XXX__XXX, X_XXXX__, X__X____,
  ____X__X, _X_XXX_X, XXX__XXX, X_XXX_X_, X__X____,
  ____X__X, XX_XXX_X, XXX__XXX, X_XXX_XX, X__X____,
  ______XX, X_X_XX_X, XXX__XXX, X_XX_X_X, XX______,
  ________, _XX_____, ________, _____XX_, ________,
  ________, ___X____, ________, ____X___, ________,
  ________, ____XXXX, XXXXXXXX, XXXX____, ________,
};

// $6786
static const u8 stage1_bitmap_truck_3[3 * 20] = {
  _XXXXXXX, XXXXXXXX, XXXXXXX_,
  XXX_XXXX, XXXXXXXX, XXXX_XXX,
  XX_XXXXX, X______X, XXXXX_XX,
  _X_XXXX_, XX____XX, _XXXX_X_,
  __XXXXXX, XXXXXXXX, XXXXXX__,
  _X______, ________, ______X_,
  _XXXXXXX, XXXXXXXX, XXXXXXXX,
  X_X_X___, ________, ___X_X_X,
  X_X_____, ________, _____X_X,
  _XX_X_X_, X_X_X_X_, X_X_XXX_,
  X_______, ________, _______X,
  _XXXXXXX, XXXXXXXX, XXXXXXX_,
  ___XXX__, ________, ___XX___,
  ___X___X, X_XX_XX_, XX___X__,
  __X_XX_X, X_XX_XX_, XX_XX_X_,
  __X_X__X, X_XX_XX_, XX__X_X_,
  __X_X__X, X_XX_XX_, XX__X_X_,
  ___XXXX_, X_XX_XX_, X_XXXX__,
  _______X, ________, _X______,
  ________, XXXXXXXX, X_______,
};

// $67C2
static const u8 stage1_bitmap_car_1[6 * 31] = {
  ________, __XXXXXX, XXXXXXXX, XXXXXXXX, XXXX____, ________,
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  _XXXXX__, X_X_XXXX, XXXXXXXX, XXXXXXXX, XXX_X_X_, _XXXXX__,
  XXXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXXX_,
  XXXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXXX_,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  _XX_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_XXX_,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  _X______, ________, ________, ________, ________, _____X__,
  _X______, ________, ________, ________, ________, _____X__,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  __X_____, ________, ________, ________, ________, ____X___,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  _X__XXXX, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, XXX__X__,
  _X__XXXX, _X_X_X_X, _X______, _____X_X, _X_X_X_X, XXX__X__,
  _X__XXXX, __X_X_X_, XX______, _____XX_, X_X_X_X_, XXX__X__,
  __XXXXX_, _X_X_X_X, _X______, _____X_X, _X_X_X__, XXXXXX__,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  __X_XX__, ________, ________, ________, ________, _XX_X___,
  ___X_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_X____,
  __X_XX__, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, _XX_X___,
  _X___XX_, ________, ________, ________, ________, XX___X__,
  _X____XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X____X__,
  __XXXXXX, X_X_____, ________, ___XXXXX, ____X_XX, XXXXX___,
  _______X, XX_X____, ________, __XXXXXX, ___X_XXX, ________,
  ________, XX__X___, ________, __XXXXXX, __X__XX_, ________,
  ________, _XX__XX_, ________, ___XXXX_, XX__XX__, ________,
  ________, ___X___X, XXXXXXXX, XXXXXXXX, ___XX___, ________,
  ________, ____XX__, ________, ________, _XX_____, ________,
  ________, ______XX, XXXXXXXX, XXXXXXXX, X_______, ________
};

// $687C
static const u8 stage1_bitmap_car_2[5 * 22] = {
  ________, _XXXXXXX, XXXXXXXX, XXXXX___, ________,
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  __XXXX__, X_XXXXXX, XXXXXXXX, XXXXX_X_, _XXXX___,
  _XXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXX__,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  __XX_X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, _X_XXX__,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  __X_____, ________, ________, ________, ____X___,
  ___X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, _X_X____,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  __X__XX_, XXXXXXXX, XXXXXXXX, XXXXXXX_, XX__X___,
  __X__XX_, _X_X_X__, ________, XX_X_X__, XX__X___,
  ___XXXX_, X_X_XX__, ________, X_X_X_X_, XXXXX___,
  ___XXX_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, _XXX____,
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  ___X_X__, XXXXXXXX, XXXXXXXX, XXXXXXX_, _X_X____,
  __X__XXX, X_______, ________, ______XX, XX__X___,
  ___XXXXX, _X______, ______XX, XX___X_X, XXXX____,
  _______X, X_X_____, _____XXX, XX__X_XX, ________,
  ________, XX_XX___, ______XX, X_XX_XX_, ________,
  ________, __X__XXX, XXXXXXXX, XX__X___, ________,
  ________, ___XXXXX, XXXXXXXX, XXXX____, ________,
};

// $68EA
static const u8 stage1_bitmap_car_3[3 * 16] = {
  _____XXX, XXXXXXXX, XXX_____,
  _XXXXXXX, XXXXXXXX, XXXXXXX_,
  XXX__XXX, XXXXXXXX, XXX__XXX,
  XX_XXXXX, XXXXXXXX, XXXXX_XX,
  _XX_X_X_, X_XX_X_X, _X_X_XX_,
  X_______, ________, _______X,
  _XX_XXXX, XXXXXXXX, XXXX_XX_,
  X_XXX_X_, X______X, _X_XXX_X,
  X_XX_X_X, _X_XX_X_, X_X_XX_X,
  _XXXXXXX, XXXXXXXX, XXXXXXX_,
  _XX_XXXX, XXXXXXXX, XXXXX_X_,
  _X_X____, ________, _____X_X,
  __XXX_X_, ______XX, __X_XXX_,
  ____XX_X, _____XXX, _X_XX___,
  _____XX_, _XXXXXXX, __XX____,
  _______X, XXXXXXXX, XX______,
};

// $691A
static const u8 stage1_bitmap_lambo_4[3 * 2 * 8] = {
  ________, XXXXXXXX, _______X, XXXXXXX_, XXXXXXXX, ________,
  X_______, _XXXXXXX, ______XX, XXXXXX__, XXXXXXXX, ________,
  ________, XX_XXX_X, _______X, _X_XXXX_, XXXXXXXX, ________,
  ________, X_XX_XXX, _______X, XXX_XXX_, XXXXXXXX, ________,
  ________, X_X__XX_, _______X, X_X__XX_, XXXXXXXX, ________,
  X_______, _X_XXXXX, ______XX, XXXXXX__, XXXXXXXX, ________,
  _X______, X_XXX_X_, _____X_X, X_XXX_X_, XXXXXXXX, ________,
  ________, XXXXXXXX, _______X, XXXXXXX_, XXXXXXXX, ________,
};

// $694A
static const u8 stage1_bitmap_lambo_4s[3 * 2 * 8] = {
  XXXX____, ____XXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXXXX___, _____XXX, ________, XXXXXXXX, __XXXXXX, XX______,
  XXXX____, ____XX_X, ________, XX_X_X_X, ___XXXXX, XXX_____,
  XXXX____, ____X_XX, ________, _XXXXXX_, ___XXXXX, XXX_____,
  XXXX____, ____X_X_, ________, _XX_X_X_, ___XXXXX, _XX_____,
  XXXXX___, _____X_X, ________, XXXXXXXX, __XXXXXX, XX______,
  XXXX_X__, ____X_XX, ________, X_X_X_XX, _X_XXXXX, X_X_____,
  XXXX____, ____XXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
};

// $697A
static const u8 stage1_bitmap_truck_4[2 * 2 * 12] = {
  ________, XXXXXXXX, _____XXX, XXXXX___,
  ________, XX_XXXXX, _____XXX, XX_XX___,
  ________, XXXXX___, _____XXX, XXXXX___,
  ________, X_X_X_X_, _____XXX, X_X_X___,
  ________, X_______, _____XXX, ____X___,
  ________, X_______, _____XXX, ____X___,
  ________, XXX_X_X_, _____XXX, X_XXX___,
  X_______, _X_X_X_X, ____XXXX, _X_X____,
  ________, X__X_X_X, _____XXX, _X__X___,
  ________, XX_X_X_X, _____XXX, _X_XX___,
  X_______, _XX_____, ____XXXX, __XX____,
  XXX_____, ___XXXXX, __XXXXXX, XX______,
};

// $69AA
static const u8 stage1_bitmap_truck_4s[2 * 2 * 12] = {
  XXX_____, ___XXXXX, ________, XXXXXXXX,
  XXX_____, ___XX_XX, ________, XXXXX_XX,
  XXX_____, ___XXXXX, ________, ___XXXXX,
  XXX_____, ___X_X_X, ________, _X_X_X_X,
  XXX_____, ___X____, ________, _______X,
  XXX_____, ___X____, ________, _______X,
  XXX_____, ___XXX_X, ________, _X_X_XXX,
  XXXX____, ____X_X_, _______X, X_X_X_X_,
  XXX_____, ___X__X_, ________, X_X_X__X,
  XXX_____, ___XX_X_, ________, X_X_X_XX,
  XXXX____, ____XX__, _______X, _____XX_,
  XXXXXX__, ______XX, _____XXX, XXXXX___,
};

// $69DA
static const u8 stage1_bitmap_car_4[3 * 2 * 9] = {
  ________, XXXXXXXX, _______X, XXXXXXX_, XXXXXXXX, ________,
  X_______, _XXXXXXX, ______XX, XXXXXX__, XXXXXXXX, ________,
  ________, XX_XXX_X, _______X, _X_XXXX_, XXXXXXXX, ________,
  ________, X_XX_XXX, _______X, XXX_XXX_, XXXXXXXX, ________,
  ________, X_X__XX_, _______X, X_X__XX_, XXXXXXXX, ________,
  X_______, _X_XXXXX, ______XX, XXXXXX__, XXXXXXXX, ________,
  _X______, X_XXX_X_, _____X_X, X_XXX_X_, XXXXXXXX, ________,
  ________, XXXXXXXX, _______X, XXXXXXX_, XXXXXXXX, ________,
};

// $6A10
static const u8 stage1_bitmap_car_4s[3 * 2 * 9] = {
  XXXX____, ____XXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXXXX___, _____XXX, ________, XXXXXXXX, __XXXXXX, XX______,
  XXXX____, ____XX_X, ________, XX_X_X_X, ___XXXXX, XXX_____,
  XXXX____, ____X_XX, ________, _XXXXXX_, ___XXXXX, XXX_____,
  XXXX____, ____X_X_, ________, _XX_X_X_, ___XXXXX, _XX_____,
  XXXXX___, _____X_X, ________, XXXXXXXX, __XXXXXX, XX______,
  XXXX_X__, ____X_XX, ________, X_X_X_XX, _X_XXXXX, X_X_____,
  XXXX____, ____XXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
};

/* ----------------------------------------------------------------------- */

static const bitmap_t stage1_stones_bitmaps[SPRITE_FRAMES] = {
  { 2, BITMAPFLAG_MASKED, 5, &stage1_bitmap_stones_1[0], &stage1_bitmap_stones_1[0] },
  { 2, BITMAPFLAG_MASKED, 4, &stage1_bitmap_stones_2[0], &stage1_bitmap_stones_2s[0] },
  { 2, BITMAPFLAG_MASKED, 3, &stage1_bitmap_stones_3[0], &stage1_bitmap_stones_3s[0] },
  { 1, BITMAPFLAG_MASKED, 2, &stage1_bitmap_stones_4[0], &stage1_bitmap_stones_4s[0] },
  { 1, BITMAPFLAG_MASKED, 2, &stage1_bitmap_stones_4[0], &stage1_bitmap_stones_4s[0] },
  { 1, BITMAPFLAG_MASKED, 1, &stage1_bitmap_stones_5[0], &stage1_bitmap_stones_5s[0] }
};

static const bitmap_t stage1_dust_bitmaps[SPRITE_FRAMES] = {
  { 1, BITMAPFLAG_MASKED, 1, &stage1_bitmap_dust_1[0], &stage1_bitmap_dust_1s[0] },
  { 1, BITMAPFLAG_MASKED, 1, &stage1_bitmap_dust_1[0], &stage1_bitmap_dust_1s[0] },
  { 1, BITMAPFLAG_MASKED, 1, &stage1_bitmap_dust_1[0], &stage1_bitmap_dust_1s[0] },
  { 1, BITMAPFLAG_MASKED, 1, &stage1_bitmap_dust_1[0], &stage1_bitmap_dust_1s[0] },
  { 1, BITMAPFLAG_MASKED, 1, &stage1_bitmap_dust_1[0], &stage1_bitmap_dust_1s[0] },
  { 1, BITMAPFLAG_MASKED, 1, &stage1_bitmap_dust_1[0], &stage1_bitmap_dust_1s[0] }
};

// $6A9A
static const u8 stage1_bitmap_stones_1[2 * 2 * 5] = {
  ________, XXXXXXXX, ___XXXXX, XXX_____,
  ________, X_____X_, ____XXXX, XXXX____,
  ________, X______X, ___XXXXX, _XX_____,
  X_______, _XX___XX, __XXXXXX, XX______,
  XXX_____, ___XXXXX, XXXXXXXX, ________,
};

// $6AAE
static const u8 stage1_bitmap_stones_2[2 * 2 * 4] = {
  ________, XXXXXXXX, XXXXXXXX, ________,
  ________, X____XXX, _XXXXXXX, X_______,
  X_______, _X____XX, XXXXXXXX, ________,
  XX_____X, __XX_XX_, XXXXXXXX, ________,
};

// $6ABE
static const u8 stage1_bitmap_stones_2s[2 * 2 * 4] = {
  XXXX____, ____XXXX, ____XXXX, XXXX____,
  XXXX____, ____X___, _____XXX, _XXXX___,
  XXXXX___, _____X__, ____XXXX, __XX____,
  XXXXXX__, ______XX, ___XXXXX, _XX_____,
};

// $6ACE
static const u8 stage1_bitmap_stones_3[2 * 2 * 3] = {
  _____XXX, XXXXX___, XXXXXXXX, ________,
  ______XX, X__X_X__, XXXXXXXX, ________,
  X____XXX, _X_XX___, XXXXXXXX, ________,
};

// $6ADA
static const u8 stage1_bitmap_stones_3s[2 * 2 * 3] = {
  XXXX____, ____XXXX, _XXXXXXX, X_______,
  XXXX____, ____X__X, __XXXXXX, _X______,
  XXXXX___, _____X_X, _XXXXXXX, X_______,
};

// $6AE6
static const u8 stage1_bitmap_stones_4[1 * 2 * 2] = {
  ____XXXX, XXXX____,
  ___XXXXX, X_X_____,
};

// $6AEA
static const u8 stage1_bitmap_stones_4s[1 * 2 * 2] = {
  XXXX____, ____XXXX,
  XXXX___X, ____X_X_,
};

// $6AEE
static const u8 stage1_bitmap_stones_5[1 * 2 * 1] = {
  __XXXXXX, XX______,
};

// $6AF0
static const u8 stage1_bitmap_stones_5s[1 * 2 * 1] = {
  XXXX__XX, ____XX__,
};

// $6AF2
static const u8 stage1_bitmap_dust_1[1 * 2 * 1] = {
  ___XXXXX, _X______,
};

// $6AF4
static const u8 stage1_bitmap_dust_1s[1 * 2 * 1] = {
  XXXX___X, _____X__,
};

/* ----------------------------------------------------------------------- */

// $6AF6
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

// $6B0C
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

// $6B22
static const bitmap_t stage1_turn_sign_bitmaps[] = {
  { 4, BITMAPFLAG_DEFAULT,                  40, stage1_bitmap_turnsign_1, stage1_bitmap_turnsign_1  },
  { 3, BITMAPFLAG_DEFAULT,                  30, stage1_bitmap_turnsign_2, stage1_bitmap_turnsign_2  },
  { 2, BITMAPFLAG_DEFAULT,                  20, stage1_bitmap_turnsign_3, stage1_bitmap_turnsign_3  },
  { 2, BITMAPFLAG_MASKED,                   16, stage1_bitmap_turnsign_4, stage1_bitmap_turnsign_4  },
  { 2, BITMAPFLAG_MASKED,                   13, stage1_bitmap_turnsign_5, stage1_bitmap_turnsign_5s },
  { 4, BITMAPFLAG_FLIPPED,                  40, stage1_bitmap_turnsign_1, stage1_bitmap_turnsign_1  },
  { 3, BITMAPFLAG_FLIPPED,                  30, stage1_bitmap_turnsign_2, stage1_bitmap_turnsign_2  },
  { 2, BITMAPFLAG_FLIPPED,                  20, stage1_bitmap_turnsign_3, stage1_bitmap_turnsign_3  },
  { 2, BITMAPFLAG_FLIPPED,                  16, stage1_bitmap_turnsign_4, stage1_bitmap_turnsign_4  },
  { 2, BITMAPFLAG_FLIPPED | BITMAPFLAG_MASKED, 13, stage1_bitmap_turnsign_5, stage1_bitmap_turnsign_5s }
};

// $6B68
static const u8 stage1_bitmap_turnsign_1[4 * 40] = {
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXX____,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  _XX____X, XXXXXXXX, XXXXXXXX, X_X_XXX_,
  __X__X_X, ________, ________, XX_X_X__,
  __X_X_XX, ________, ________, X_X_XX__,
  __XXXXXX, ________, ________, XXXXXX__,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX_____, _______X, _X_X_X_X, _X_X_XXX,
  XXX_____, ________, ______X_, X_X_XXXX,
  XXX_____, ___XXX__, ________, ___X_XXX,
  XXX_____, ___XXX__, ________, ____XXXX,
  XXX_____, ___XXX__, ________, ___X_XXX,
  XXX_____, ___XXX__, ________, ____XXXX,
  XXX_____, ___XXX__, ________, ___X_XXX,
  XXX_____, ___XXX__, ________, ____XXXX,
  XXX_____, ___XXX__, ________, ___X_XXX,
  XXX_____, ___XXX__, ________, ____XXXX,
  XXX_____, ___XXX__, ________, _____XXX,
  XXX_____, ___XXX__, ________, ____XXXX,
  X_X_____, ___XXX__, ________, _____XXX,
  XXX_____, ____XXX_, ________, ____XXXX,
  XXX_____, ____XXX_, ________, _____XXX,
  X_X_____, ____XXX_, ________, _____XXX,
  XXX_____, _____XXX, ________, _____XXX,
  X_X_____, _____XXX, __XX____, _____XXX,
  X_X_X___, ______XX, XXXX____, _____XXX,
  XXX_____, _______X, XXXX____, _____XXX,
  X_X_X___, ______XX, XXXXX___, _____XXX,
  X_X_____, _____XXX, XXXXX___, _____XXX,
  X_X_X___, ________, XXXXXX__, _____XXX,
  XXX_____, ________, ____XXX_, _____XXX,
  X_X_X_X_, X_X_____, ________, _____XXX,
  X_X_____, ________, ________, _____XXX,
  X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X____X__, _X__X_XX, _XXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
};

// $6C08
static const u8 stage1_bitmap_turnsign_2[3 * 30] = {
  __XXXXXX, XXXXXXXX, XXXXXX__,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  _XX__XXX, XXXXXXXX, XXXX_XX_,
  __X_XX__, ________, __X_XX__,
  __XXXX__, ________, __XX_X__,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XX______, ____X_X_, X_X_X_XX,
  XX______, ________, _X_X_XXX,
  XX______, XX______, ____X_XX,
  XX______, XX______, _____XXX,
  XX______, XX______, ____X_XX,
  XX______, XX______, _____XXX,
  XX______, XX______, ____X_XX,
  XX______, XX______, _____XXX,
  XX______, XX______, ______XX,
  XX______, XXX_____, _____XXX,
  XX______, _XX_____, ______XX,
  XX______, _XX_____, ______XX,
  XX______, _XXX____, ______XX,
  XX______, __XX__X_, ______XX,
  XX_X____, ___XXXX_, ______XX,
  XX______, ___XXXXX, ______XX,
  X__X____, __XXXXXX, ______XX,
  XX______, _____XXX, X_____XX,
  X__X_X_X, _______X, XX____XX,
  XX______, ________, ______XX,
  X_X_X_XX, _XXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
};

// $6C62
static const u8 stage1_bitmap_turnsign_3[2 * 20] = {
  _XXXXXXX, XXXXXXX_,
  XXXXXXXX, XXXXXXXX,
  _X__XXXX, XXXXX_X_,
  _X_X____, ____XXX_,
  XXXXXXXX, XXXXXXXX,
  X______X, _X_X_X_X,
  X_______, ______XX,
  X_____X_, _______X,
  X_____X_, ______XX,
  X_____X_, _______X,
  X_____X_, ______XX,
  X_____X_, _______X,
  X______X, ______XX,
  X______X, _______X,
  X_______, XXX____X,
  XX_____X, XXX____X,
  X_______, XXX____X,
  XX______, __XX___X,
  X_X_X___, _______X,
  XXXXXXXX, XXXXXXXX,
};

// $6C8A
static const u8 stage1_bitmap_turnsign_4[2 * 2 * 16] = {
  X_______, _XXXXXXX, ____XXXX, XXXX____,
  ________, XXXXXXXX, _____XXX, XXXXX___,
  ________, X_XXXXXX, _____XXX, XXXXX___,
  ________, XXXXXXXX, _____XXX, XXXXX___,
  ________, X_____X_, _____XXX, X_X_X___,
  ________, X___X___, _____XXX, ___XX___,
  ________, X___X___, _____XXX, ____X___,
  ________, X___X___, _____XXX, ___XX___,
  ________, X___X___, _____XXX, ____X___,
  ________, X____X__, _____XXX, ___XX___,
  ________, XX___X__, _____XXX, ____X___,
  ________, X_____XX, _____XXX, ____X___,
  ________, X____XXX, _____XXX, X___X___,
  ________, XX_____X, _____XXX, X___X___,
  ________, X_X__X__, _____XXX, ____X___,
  ________, XXXXXXXX, _____XXX, XXXXX___,
};

// $6CCA
static const u8 stage1_bitmap_turnsign_5[2 * 2 * 13] = {
  X_______, _XXXXXXX,
  _XXXXXXX, X_______,
  ________, XXXXXXXX,
  __XXXXXX, XX______,
  ________, XXXXXXXX,
  __XXXXXX, XX______,
  ________, XXXXXXXX,
  __XXXXXX, XX______,
  ________, X___X_X_,
  __XXXXXX, XX______,
  ________, X__X____,
  __XXXXXX, _X______,
  ________, X__X____,
  __XXXXXX, XX______,
  ________, X__X____,
  __XXXXXX, _X______,
  ________, X___X___,
  __XXXXXX, XX______,
  ________, X___XX__,
  __XXXXXX, _X______,
  ________, X____XX_,
  __XXXXXX, _X______,
  ________, X_______,
  __XXXXXX, _X______,
  ________, XXXXXXXX,
  __XXXXXX, XX______,
};

// $6CFE
static const u8 stage1_bitmap_turnsign_5s[2 * 2 * 13] = {
  XXXXX___, _____XXX,
  _____XXX, XXXXX___,
  XXXX____, ____XXXX,
  ______XX, XXXXXX__,
  XXXX____, ____XXXX,
  ______XX, XXXXXX__,
  XXXX____, ____XXXX,
  ______XX, XXXXXX__,
  XXXX____, ____X___,
  ______XX, X_X_XX__,
  XXXX____, ____X__X,
  ______XX, _____X__,
  XXXX____, ____X__X,
  ______XX, ____XX__,
  XXXX____, ____X__X,
  ______XX, _____X__,
  XXXX____, ____X___,
  ______XX, X___XX__,
  XXXX____, ____X___,
  ______XX, XX___X__,
  XXXX____, ____X___,
  ______XX, _XX__X__,
  XXXX____, ____X___,
  ______XX, _____X__,
  XXXX____, ____XXXX,
  ______XX, XXXXXX__,
};

/* ----------------------------------------------------------------------- */

// $6D82
static const bitmap_t stage1_tumbleweed_bitmaps[SPRITE_FRAMES] = {
  { 2, BITMAPFLAG_DEFAULT, 16, &stage1_bitmap_tumbleweed_1[0], &stage1_bitmap_tumbleweed_1[0] },
  { 2, BITMAPFLAG_DEFAULT, 11, &stage1_bitmap_tumbleweed_2[0], &stage1_bitmap_tumbleweed_2[0] },
  { 1, BITMAPFLAG_DEFAULT,  9, &stage1_bitmap_tumbleweed_3[0], &stage1_bitmap_tumbleweed_3[0] },
  { 1, BITMAPFLAG_DEFAULT,  9, &stage1_bitmap_tumbleweed_3[0], &stage1_bitmap_tumbleweed_3[0] },
  { 1, BITMAPFLAG_DEFAULT,  7, &stage1_bitmap_tumbleweed_4[0], &stage1_bitmap_tumbleweed_4[0] },
  { 1, BITMAPFLAG_DEFAULT,  7, &stage1_bitmap_tumbleweed_4[0], &stage1_bitmap_tumbleweed_4[0] }
};

// $6DAC
static const u8 stage1_bitmap_tumbleweed_1[2 * 16] = {
  ___XX___, _X_XX___,
  ____XXXX, XXXX____,
  ___XXXXX, XXXXX___,
  __XXXXXX, XXXXXX__,
  __XXXXXX, XX_XXX__,
  _XXXXXXX, XXXXXXX_,
  _X_XX_XX, X_XXX_X_,
  __XX_X__, XX_XXX__,
  __X_X_X_, X_X_XXX_,
  _X_X_X_X, X_X_X_X_,
  __XX_XX_, X_X_XX__,
  ___XX_X_, __X_XX__,
  ___XX_XX, _XX_X___,
  __XX_XXX, XX_XX___,
  _____X__, X__X____,
  ________, X_XX____,
};

// $6DCC
static const u8 stage1_bitmap_tumbleweed_2[2 * 11] = {
  ____X__X, __XX____,
  _____XXX, XXX_____,
  ____XXXX, XXXX____,
  ____XXXX, X_XXX___,
  ____XXX_, XXXXX___,
  ___XXXX_, X_X_X___,
  ___X_X_X, _X_X____,
  _____XX_, X_XX____,
  ______XX, X_X_____,
  ____XX_X, _XX_____,
  ________, X_X_____,
};

// $6DE2
static const u8 stage1_bitmap_tumbleweed_3[1 * 9] = {
  __XXXXX_,
  _XXXXXXX,
  XXXXXXXX,
  XXX__XXX,
  XXX_X_XX,
  _X_X__X_,
  _XXX_XX_,
  __X_X_X_,
  __X__X__,
};

// $6DEB
static const u8 stage1_bitmap_tumbleweed_4[1 * 7] = {
  __XXXX__,
  _XXXXXX_,
  _XXX_XX_,
  _XX__XX_,
  __X_XX__,
  __XX_X__,
  ___XX___,
};

/* ----------------------------------------------------------------------- */

// $6DF2
static const bitmap_t stage1_barrier_bitmaps[SPRITE_FRAMES] = {
  { 4, BITMAPFLAG_DEFAULT, 17, &stage1_bitmap_barrier_1[0], &stage1_bitmap_barrier_1[0]  },
  { 4, BITMAPFLAG_DEFAULT, 17, &stage1_bitmap_barrier_1[0], &stage1_bitmap_barrier_1[0]  },
  { 3, BITMAPFLAG_DEFAULT, 13, &stage1_bitmap_barrier_2[0], &stage1_bitmap_barrier_2[0]  },
  { 2, BITMAPFLAG_DEFAULT,  9, &stage1_bitmap_barrier_3[0], &stage1_bitmap_barrier_3[0]  },
  { 2, BITMAPFLAG_MASKED,   9, &stage1_bitmap_barrier_3[0], &stage1_bitmap_barrier_3[0]  },
  { 2, BITMAPFLAG_MASKED,   7, &stage1_bitmap_barrier_4[0], &stage1_bitmap_barrier_4s[0] }
};

// $6E1C
static const u8 stage1_bitmap_barrier_1[4 * 17] = {
  ___XXXXX, XX______, ______XX, XXXXX___,
  ___XXXX_, _X____XX, XX____XX, _X__X___,
  ____XX_X, __X___XX, _X___XX_, X__X____,
  ____XXX_, X_X___XX, XX___XXX, _X_X____,
  _____XXX, _X_X__XX, _X__XXX_, X_X_____,
  _____XX_, X__X_XX_, X_X_XX_X, __X_____,
  ______XX, _X__XXXX, __XXX_X_, _X______,
  ______XX, X_X_XXX_, X_XXXX_X, _X______,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXX_X, _XXXXXX_, _XXXXXX_, X_XXXXXX,
  XXXXX___, _XXXXX__, __XXXXX_, ___XXXXX,
  XXXXX___, _XXXXX__, __XXXXX_, ___XXXXX,
  XXXXX___, _XXXXX__, __XXXXX_, ___XXXXX,
  XXXXX___, _XXXXX__, __XXXXX_, ___XXXXX,
  X_X_X___, _X_X_X__, __X_X_X_, ___X_X_X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  ________, ____XXXX, XXXXX___, ________,
};

// $6E60
static const u8 stage1_bitmap_barrier_2[3 * 13] = {
  __XXXXXX, ________, XXXXXX__,
  __XXX_XX, ___XXX__, XX_X_X__,
  ___XXX_X, X__X_X_X, X_X_X___,
  ___XX_X_, X_XX_X_X, XX__X___,
  ____XX_X, X_X_X_XX, X__X____,
  ____XXX_, _XXXXXXX, XX_X____,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXX_XX, XXXX_XXX, X_X_XXXX,
  XXXX___X, XXX__XXX, X___XXXX,
  XXXX___X, XXX__XXX, X___XXXX,
  X__X___X, __X__X__, X___X__X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  ________, _XXXXXX_, ________,
};

// $6E87
static const u8 stage1_bitmap_barrier_3[2 * 9] = {
  _XXXX___, ___XXXX_,
  _XX_X__X, X__XX_X_,
  __XX_X_X, X_XX_X__,
  __XXXX_X, X_XXXX__,
  XXXXXXXX, XXXXXXXX,
  XXX__XXX, __XXX__X,
  X_X__X_X, __X_X__X,
  XXXXXXXX, XXXXXXXX,
  ______XX, XX______,
};

// $6E99
static const u8 stage1_bitmap_barrier_4[2 * 2 * 7] = {
  X__XXXXX, _XX_____, X__XXXXX, _XX_____,
  X___X__X, _XXX_XX_, ___XXXXX, XXX_____,
  XX______, __XXXXXX, __XXXXXX, XX______,
  ________, XXXXXXXX, ____XXXX, XXXX____,
  ________, X__XX__X, ____XXXX, X__X____,
  ________, XXXXXXXX, ____XXXX, XXXX____,
  XXXX____, ____XXXX, XXXXXXXX, ________,
};

// $6EB5
static const u8 stage1_bitmap_barrier_4s[2 * 2 * 7] = {
  XXXXX__X, _____XX_, XXXXX__X, _____XX_,
  XXXXX___, _____XXX, X__X___X, _XX_XXX_,
  XXXXXX__, ______XX, ______XX, XXXXXX__,
  XXXX____, ____XXXX, ________, XXXXXXXX,
  XXXX____, ____X__X, ________, X__XX__X,
  XXXX____, ____XXXX, ________, XXXXXXXX,
  XXXXXXXX, ________, ____XXXX, XXXX____,
};

/* ----------------------------------------------------------------------- */

// $6ED1
static const stretchy_t stage1_stretchy_streetlamp_right[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_right     },
  { STRETCHY_TYPE_50PC, &streetlampmiddle2_right    },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_right     },
  { STRETCHY_TYPE_FIXED, &stage1_streetlamptop_right },
  { STRETCHY_TYPE_END, NULL }
};

// $6EDE
static const stretchy_t stage1_stretchy_streetlamp_left[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_left     },
  { STRETCHY_TYPE_50PC, &streetlampmiddle2_left    },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_left     },
  { STRETCHY_TYPE_FIXED, &stage1_streetlamptop_left },
  { STRETCHY_TYPE_END, NULL }
};

// $6EEB
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

// $6F01
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

// $6F17
static const bitmap_t stage1_streetlamptop_bitmaps[5] = {
  { 4, BITMAPFLAG_DEFAULT, 8, stage1_bitmap_streetlamptop_1, stage1_bitmap_streetlamptop_1  },
  { 3, BITMAPFLAG_DEFAULT, 5, stage1_bitmap_streetlamptop_2, stage1_bitmap_streetlamptop_2  },
  { 3, BITMAPFLAG_DEFAULT, 4, stage1_bitmap_streetlamptop_3, stage1_bitmap_streetlamptop_3s },
  { 2, BITMAPFLAG_MASKED,  4, stage1_bitmap_streetlamptop_4, stage1_bitmap_streetlamptop_4s },
  { 2, BITMAPFLAG_MASKED,  3, stage1_bitmap_streetlamptop_5, stage1_bitmap_streetlamptop_5s }
};

// $6F3A
static const bitmap_t stage1_streetlamptop_flipped_bitmaps[5] = {
  { 4, BITMAPFLAG_FLIPPED, 8, stage1_bitmap_streetlamptop_1, stage1_bitmap_streetlamptop_1  },
  { 3, BITMAPFLAG_FLIPPED, 5, stage1_bitmap_streetlamptop_2, stage1_bitmap_streetlamptop_2  },
  { 3, BITMAPFLAG_FLIPPED, 4, stage1_bitmap_streetlamptop_3, stage1_bitmap_streetlamptop_3s },
  { 2, BITMAPFLAG_MASKED | BITMAPFLAG_FLIPPED, 4, stage1_bitmap_streetlamptop_4, stage1_bitmap_streetlamptop_4s },
  { 2, BITMAPFLAG_MASKED | BITMAPFLAG_FLIPPED, 3, stage1_bitmap_streetlamptop_5, stage1_bitmap_streetlamptop_5s }
};

// $6F5D
static const u8 stage1_bitmap_streetlamptop_1[4 * 8] = {
  ________, ________, ________, XXX__X__,
  ___XXXXX, XXXXXXX_, _______X, X_X__X__,
  __X_____, _______X, _____XXX, __X__X__,
  __X_____, _______X, __XXXX__, XX___X__,
  _XX_X_X_, X_X_X_XX, XXX___XX, _X___X__,
  X_XXXXXX, XXXXXXX_, ___XXX__, _XX_XX__,
  XX______, _______X, XXX_____, __XXX___,
  __XXXXXX, XXXXXXX_, ________, ________
};

// $6F7D
static const u8 stage1_bitmap_streetlamptop_2[3 * 5] = {
  _XXXXXXX, XX_____X, X_X_X___,
  X_______, __X__XX_, _X__X___,
  X_______, _XXXX__X, XX__X___,
  X_X_X_X_, X____XX_, _X__X___,
  _XXXXXXX, XXXXX___, __XX____
};

// $6F8C
static const u8 stage1_bitmap_streetlamptop_3[3 * 4] = {
  ______XX, XXXX___X, _XX_____,
  _____X__, ____XXX_, X_X_____,
  ____X_X_, _____X_X, _XX_____,
  _____XXX, XXXXXXX_, ________
};

// $6F98
static const u8 stage1_bitmap_streetlamptop_3s[3 * 4] = {
  ________, __XXXXXX, ___X_XX_,
  ________, _X______, XXX_X_X_,
  ________, X_X_____, _X_X_XX_,
  ________, _XXXXXXX, XXX_____
};

// $6FA4
static const u8 stage1_bitmap_streetlamptop_4[2 * 2 * 4] = {
  XXXXXXXX, ________,
  X___XXXX, _XXX____,
  ______X_, XX_XXX_X,
  ____XXXX, X_XX____,
  ________, X____XX_,
  _X__XXXX, X_XX____,
  X____XXX, _XXXX___,
  XXXXXXXX, ________
};

// $6FB4
static const u8 stage1_bitmap_streetlamptop_4s[2 * 2 * 4] = {
  XXXXXXXX, ________,
  XXX___XX, ___XXX__,
  XX______, __XX_XXX,
  X_____XX, _XX_XX__,
  XX______, __X____X,
  ___X__XX, X_X_XX__,
  XXX____X, ___XXXX_,
  XXXXXXXX, ________
};

// $6FC4
static const u8 stage1_bitmap_streetlamptop_5[2 * 2 * 4] = {
  XXXXXXX_, _______X,
  _XXXXXXX, X_______,
  ________, X___XXXX,
  _XXXXXXX, X_______,
  ____XXXX, XXXX____,
  XXXXXXXX, ________
};

// $6FD0
static const u8 stage1_bitmap_streetlamptop_5s[2 * 2 * 3] = {
  XXXXXXXX, ________,
  XXX__XXX, ___XX___,
  XXXX____, ____X___,
  _____XXX, XXXXX___,
  XXXX____, ____XXXX,
  XXXXXXXX, ________
};

/* ----------------------------------------------------------------------- */

// $6FDC
static const stretchy_t stage1_stretchy_telegraphpole_right[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_right },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_right },
  { STRETCHY_TYPE_FIXED, &stage1_telegraphpoletop_right },
  { STRETCHY_TYPE_END, NULL }
};

// $6FE6
static const stretchy_t stage1_stretchy_telegraphpole_left[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_left },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_left },
  { STRETCHY_TYPE_FIXED, &stage1_telegraphpoletop_left },
  { STRETCHY_TYPE_END, NULL }
};

// $6FF0
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

// $7006
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

// $701C
static const bitmap_t stage1_telegraphpoletop_bitmaps[5] = {
  { 3, BITMAPFLAG_DEFAULT, 13, stage1_bitmap_telegraphpoletop_1, stage1_bitmap_telegraphpoletop_1  },
  { 3, BITMAPFLAG_DEFAULT, 10, stage1_bitmap_telegraphpoletop_2, stage1_bitmap_telegraphpoletop_2  },
  { 3, BITMAPFLAG_DEFAULT,  7, stage1_bitmap_telegraphpoletop_3, stage1_bitmap_telegraphpoletop_3s },
  { 2, BITMAPFLAG_MASKED,   5, stage1_bitmap_telegraphpoletop_4, stage1_bitmap_telegraphpoletop_4s },
  { 2, BITMAPFLAG_MASKED,   4, stage1_bitmap_telegraphpoletop_5, stage1_bitmap_telegraphpoletop_5s }
};

// $703F
static const u8 stage1_bitmap_telegraphpoletop_1[3 * 13] = {
  ____XXXX, XXX__XXX, XXX_____,
  ___X____, ___X_XX_, ___X____,
  ___XXXXX, XXX__XXX, XXXX____,
  ____X_X_, _X___X__, X_X_____,
  ____X_X_, _XX__X__, X_X_____,
  _____X__, _XX__X__, _X______,
  _XXXXXXX, XXXX_XXX, XXXXXX__,
  X_______, ___X_XX_, X_____X_,
  XXXXXXXX, XXX__XXX, XXXXXXX_,
  _X_XX_X_, _X___X__, X_XX_X__,
  _X_XX_X_, _XX__X__, X_XX_X__,
  __X__X__, _X___X__, _X__X___,
  ________, _XXXXX__, ________
};

// $7066
static const u8 stage1_bitmap_telegraphpoletop_2[3 * 10] = {
  ______XX, XX__XXXX, X_______,
  _____X__, __X_XX__, X_______,
  _____X_X, XX__XXX_, X_______,
  ______XX, _X__X__X, ________,
  ________, _XX_X___, ________,
  ____XXXX, XXX_XXXX, XX______,
  ___X____, __X_XX__, __X_____,
  ___X_X_X, XX__X_X_, X_X_____,
  ___X_X_X, _X__X_X_, X_X_____,
  ____X_X_, _XXXX__X, _X______
};

// $7084
static const u8 stage1_bitmap_telegraphpoletop_3[3 * 7] = {
  ______XX, XXXXXX__, ________,
  _____X__, __X__X__, ________,
  _____X_X, X_XX_X__, ________,
  _____XXX, XXXXXXX_, ________,
  ____X___, __XXX__X, ________,
  ____X_XX, X_XXXX_X, ________,
  _____X__, XXX___X_, ________
};

// $7099
static const u8 stage1_bitmap_telegraphpoletop_3s[3 * 7] = {
  ________, __XXXXXX, XX______,
  ________, _X____X_, _X______,
  ________, _X_XX_XX, _X______,
  ________, _XXXXXXX, XXX_____,
  ________, X_____XX, X__X____,
  ________, X_XXX_XX, XX_X____,
  ________, _X__XXX_, __X_____
};

// $70AE
static const u8 stage1_bitmap_telegraphpoletop_4[2 * 2 * 5] = {
  XXXXXXX_, _______X, ______XX, XX_XXX__,
  XXXXXXX_, _______X, ______XX, _X_X_X__,
  XXXXXXXX, ________, XX__XXXX, ___X____,
  XXXXXX__, ______XX, ______XX, XX_XXX__,
  XXXXXX__, ______X_, ______XX, X___X___
};

// $70C2
static const u8 stage1_bitmap_telegraphpoletop_4s[2 * 2 * 5] = {
  XXXXXXXX, ________, X_______, _XXX_XXX,
  XXXXXXXX, ________, X_______, _X_X_X_X,
  XXXXXXXX, ________, XXXX__XX, _____X__,
  XXXXXXXX, ________, ________, XXXX_XXX,
  XXXXXXXX, ________, ________, X_X___X_
};

// $70D6
static const u8 stage1_bitmap_telegraphpoletop_5[2 * 2 * 4] = {
  XXXXXX__, ______XX, ___XXXXX, XXX_____,
  XXXXXX__, ______XX, ___XXXXX, __X_____,
  XXXXX___, _____XXX, ____XXXX, _XXX____,
  XXXXX___, _____X_X, ____XXXX, __XX____
};

// $70E6
static const u8 stage1_bitmap_telegraphpoletop_5s[2 * 2 * 4] = {
  XXXXXXXX, ________, XX_____X, __XXXXX_,
  XXXXXXXX, ________, XX_____X, __XX__X_,
  XXXXXXXX, ________, X_______, _XXX_XXX,
  XXXXXXXX, ________, X_______, _X_X__XX
};

/* ----------------------------------------------------------------------- */

// $70F6
static const stretchy_t stage1_stretchy_tree_right[SPRITE_FRAMES] = {
  { STRETCHY_TYPE_FIXED, &stage1_tree_bottom       },
  { STRETCHY_TYPE_38PC, &stage1_tree_bottommiddle },
  { STRETCHY_TYPE_FIXED, &stage1_tree_middle       },
  { STRETCHY_TYPE_113PC, &stage1_tree_topmiddle    },
  { STRETCHY_TYPE_FIXED, &stage1_tree_top          },
  { STRETCHY_TYPE_END, NULL }
};

// $7106
//
// Note: Two fewer elements than the right version (smaller/bush?)
static const stretchy_t stage1_stretchy_tree_left[4] = {
  { STRETCHY_TYPE_FIXED, &stage1_tree_middle    },
  { STRETCHY_TYPE_38PC, &stage1_tree_topmiddle },
  { STRETCHY_TYPE_FIXED, &stage1_tree_top       },
  { STRETCHY_TYPE_END, NULL }
};

// $7110
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

// $7126
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

// $713C
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

// $7152
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

// $7168
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

// $717E
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

// $722D
static const u8 stage1_bitmap_tree_middle_64x16[8 * 16] = {
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

// $72AD
static const u8 stage1_bitmap_tree_bottom_64x5[8 * 5] = {
  ________, ____X__X, __X__X_X, XXX_X_X_, X_X____X, _X___X__, X___X__X, ________,
  ______XX, X_XXXX_X, _XX__XXX, XXXX_XXX, _XX_X___, X__X___X, _X_XX_X_, X_X_____,
  ____X_X_, XXXX_XX_, XX_XX_X_, XXXXXXXX, X_XXXXX_, _XX_XXXX, _XXX_XXX, X__X____,
  _X__XXXX, XXXXXXXX, XXXXXXXX, X_XXXX_X, XXXXXXXX, ___XXXXX, XXXXXXX_, XXXX_X__,
  _X_XXXX_, XX_X_XXX, X_XXXXX_, XXX_XXXX, XXXX_XXX, XXXXX_XX, X_XXX_XX, XX_XXXX_,
};

// $72D5
static const u8 stage1_bitmap_tree_trunk_16x8[2 * 8] = {
  X_X__X_X, _X_XX__X,
  XX__XX_X, __X__X_X,
  _XX__X__, X_X_X__X,
  _X_X_X__, __X__XX_,
  X_XX__X_, X___X_X_,
  XX_X_X__, X_X__XX_,
  X_X__X__, X_X_X__X,
  XX_X_X_X, __X__X_X,
};

// $72E5
static const u8 stage1_bitmap_tree_shadow_64x5[8 * 5] = {
  ________, __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_______,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XX_XXXXX, XXXX__XX, XXXXXXXX, XXXXXXXX, XXXXX___,
  XXXXXXXX, XXXXXXXX, XXXXX__X, XX__XXX_, X_X__X_X, X_XXXXXX, XXXXXXXX, XXXXXXXX,
  ______XX, XXXXXXXX, XXXXXX__, XXX___XX, _X__X_X_, _XXXXXXX, XXXXXXXX, XXX_____,
  ________, ___XXXXX, XXXXXXXX, ___X__X_, X_______, XXXXXXXX, XXXX____, ________,
};

// $730D
static const u8 stage1_bitmap_tree_middle_48x12[6 * 12] = {
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

// $7355
static const u8 stage1_bitmap_tree_bottom_48x4[6 * 4] = {
  ______X_, _X_X_X__, _XXX_X__, X____X__, __X____X, _X______,
  ____XXXX, XX_XXX__, XXXXX_XX, _XXX__X_, X__X__XX, X_XX____,
  ___XX_XX, X_XXX_XX, X_XXXXXX, XXXXX__X, _XXX_XX_, XXXXX___,
  __XXXXX_, XXXXXXXX, X_XXX_XX, XXX_XXXX, X_XXXXXX, XXX_XXX_,
};

// $736D
static const u8 stage1_bitmap_tree_shadow_48x4[6 * 4] = {
  _______X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  _XXXXXXX, XXXXXXXX, XXX_XXXX, XXX_XXXX, XXXXXXXX, XXXXXX__,
  ____XXXX, XXXXXXXX, ___X__X_, ___XX__X, XXXXXXXX, XXXX____,
  ________, __XXXXXX, XXX____X, _____XXX, XXXXXXX_, ________,
};

// $7385
static const u8 stage1_bitmap_tree_middle_32x8[4 * 8] = {
  XXXXXXXX, __X_XX__, X__X_XXX, XXXX_XXX,
  XXX_X_XX, X_XXX__X, X_XXXXXX, X_XXXXX_,
  XXXXXXXX, _XXXXXX_, XXXX_X_X, XXX_XXXX,
  _X_XX_X_, __XX_XXX, X_XXX__X, XX_XX_X_,
  XXXX__XX, _XXXXXX_, _XXXX_XX, XXX_XX_X,
  __XXXXX_, XXX_XX_X, __X_XXXX, _XXXXXX_,
  _XXX_XXX, _XX__XX_, XXXXXXX_, XXXXXX_X,
  __XXXXXX, XX_XXXXX, X_XXX_XX, _XX_XXX_,
};

// $73A5
static const u8 stage1_bitmap_tree_bottom_32x3[4 * 3] = {
  _X_X__X_, _X__XX_X, XX_XX___, X_X_____,
  __XXXX_X, X_XXXXXX, XXXX_XX_, XX_XX___,
  _XXXX__X, XXXXXXXX, XX_XXXXX, XXXXX_XX,
};

// $73B1
static const u8 stage1_bitmap_tree_shadow_32x2[4 * 2] = {
  _XXXXXXX, XXXXX_XX, XXX_XXXX, XXXXXXX_,
  ___XXXXX, XXX_XX_X, _X_X_XXX, XXXXXX__,
};

// $73B9
static const u8 stage1_bitmap_tree_middle_24x7[3 * 7] = {
  _XXXXXX_, XXXXX__X, XX_XXXX_,
  XXX_X_XX, XX_XX_XX, _XXX_XXX,
  _XXXXXXX, X_XXXXX_, _XXXXX_X,
  XXX_XX_X, XX_XX_XX, XX__XXXX,
  XX_XX_XX, _XXXXXX_, X_X_XX_X,
  _X__XXXX, __XX_X__, XXXXXXXX,
  _XXX_XX_, _X_XX_XX, XX_XX_XX,
};

// $73CE
static const u8 stage1_bitmap_tree_bottom_24x2[3 * 2] = {
  ___X_XX_, XX_X__XX, __X_XX__,
  _XXXXXXX, X_X_XXX_, XXXXX_X_,
};

// $73D4
static const u8 stage1_bitmap_tree_trunk_8x4[1 * 4] = {
  _X_X_XX_,
  _XX_X_X_,
  _X__X_X_,
  _XX___X_,
};

// $73D8
static const u8 stage1_bitmap_tree_shadow_24x2[3 * 2] = {
  XXXXXXXX, X_XXXX_X, XXXXXXXX,
  __XXXXXX, _X_X__X_, XXXXXXX_,
};

// $73DE
static const u8 stage1_bitmap_tree_top_64x13[8 * 2 * 13] = {
  XXXX____, ____XX_X, ________, X___XXXX, ________, XX_XXXXX, ________, XX_X_XXX,
  ________, XX_X_XXX, ________, XXXXX_X_, ________, XX_XXX_X, _____XXX, XXXXX___,
  XXXX____, ____XXXX, ________, XX_X_X_X, ________, XXXXXX_X, ________, XXXXXX_X,
  ________, XXXXXX_X, ________, XXX_XX_X, ________, XXXXX_X_, ____XXXX, XXXX____,
  XXXXX___, _____XXX, ________, X_X_XXXX, ________, _XX_X_X_, ________, X_X_XXXX,
  ________, XX_XXXXX, ________, _X_X__X_, ________, XXX_XX_X, ___XXXXX, X_X_____,
  XXXXXXXX, ________, ________, XXXXXXX_, ________, __XXXX_X, ________, _XXXX_XX,
  ________, _XX_X_X_, ________, XXX____X, ________, _XXXX_X_, __XXXXXX, XX______,
  XXXXXXXX, ________, XXX_____, ___XXX_X, ________, _X_XX_X_, ________, __XX_X_X,
  ________, XX_XXXXX, ________, XX_X__X_, ________, XXXX_XXX, _XXXXXXX, X_______,
  XXXXXXXX, ________, XXXX____, ____XXX_, ________, X_X_XX_X, ________, _XXXX_XX,
  ________, X_X_XXXX, ________, _XXX_X_X, ______XX, XX_XXX__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXX____, ____XX_X, ________, _XXXXXXX, ________, XXXXXXXX,
  ________, ___XX_XX, ________, XX_XXXXX, ______XX, X_XXXX__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXX___, _____XX_, ________, XXXX_XX_, ________, XX_X_XXX,
  ________, X_XX_X_X, ________, _XXXXXXX, ____XXXX, _X_X____, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXX__, ______XX, ________, X_X_XX_X, ________, __X_XX_X,
  ________, XX_XX___, ________, X_X_XX__, ___XXXXX, XXX_____, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, ________, XXXXXXX_, ________, X__X_XX_,
  ________, XXXX_X__, ________, _X_XXXXX, _XXXXXXX, X_______, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, XXXXXX__, ______X_, ________, __X_XXXX,
  _X______, X_XXX_X_, _____XXX, X_XXX___, XXXXXXXX, ________, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, _______X, ______XX, _X_XXX__,
  XXXX____, ____XXXX, ____XXXX, XXXX____, XXXXXXXX, ________, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, _____XXX, XXXXX___,
  XXXXXX__, ______XX, ___XXXXX, XXX_____, XXXXXXXX, ________, XXXXXXXX, ________,
};

// $74AE
static const u8 stage1_bitmap_tree_top_48x10[6 * 2 * 10] = {
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

// $7526
static const u8 stage1_bitmap_tree_top_32x5[4 * 2 * 5] = {
  XXXXX___, _____X_X, ________, _XXXXX_X, ________, XXXX_X_X, ____XXXX, X_XX____,
  XXXXX___, _____XX_, ________, X_X_XXXX, ________, X_XXXXXX, __XXXXXX, XX______,
  XXXXXX__, ______XX, ________, XXXX_X_X, ________, XX_X___X, _XXXXXXX, X_______,
  XXXXXXXX, ________, XX______, __X_X_XX, _______X, XXX__XX_, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX___XX, ___XXX__, XX___XXX, __XXX___, XXXXXXXX, ________,
};

// $754E
static const u8 stage1_bitmap_tree_top_24x4[3 * 2 * 4] = {
  XX______, __XXXXXX, ________, _XXX_XXX, _______X, XXXXX_X_,
  XXXX____, ____X_X_, ________, XXXXXX_X, ______XX, _XX_XX__,
  XXXXX___, _____XXX, ________, X_X_XX__, ____XXXX, XXXX____,
  XXXXXXX_, _______X, ____X___, XXXX_XXX, _XXXXXXX, X_______,
};

// $7566
static const u8 stage1_bitmap_tree_top_24x3[3 * 2 * 3] = {
  XXX_____, ___XX_XX, ________, _XXX_XXX, XXXXXXXX, ________,
  XXXX____, ____XXXX, _______X, XX_XXXX_, XXXXXXXX, ________,
  XXXXXX__, ______XX, XX___XXX, __XXX___, XXXXXXXX, ________,
};

// $7578
static const u8 stage1_bitmap_tree_top_24x3s[3 * 2 * 3] = {
  XXXXXXX_, _______X, ________, X_XX_XXX, ____XXXX, _XXX____,
  XXXXXXXX, ________, ________, XXXXXX_X, ___XXXXX, XXX_____,
  XXXXXXXX, ________, XX__XX__, __XX__XX, _XXXXXXX, X_______,
};

// $758A
static const u8 stage1_bitmap_tree_trunk_16x6[2 * 2 * 6] = {
  XXX_____, ___X___X, _____XXX, __XXX___,
  XXX_____, ___X_X_X, _____XXX, _X__X___,
  XXX_____, ___XX_X_, ____XXXX, _X_X____,
  XX______, __X_X_X_, ____XXXX, X__X____,
  XX______, __X_X___, _____XXX, X_X_X___,
  XX______, __X_X__X, _____XXX, _X__X___,
};

// $75A2
static const u8 stage1_bitmap_tree_trunk_16x4[2 * 2 * 4] = {
  XXXX____, ____XX_X, ____XXXX, _X_X____,
  XXXX____, ____XX_X, ____XXXX, ___X____,
  XXXX____, ____X___, ____XXXX, _X_X____,
  XXXX____, ____XX_X, ____XXXX, _X_X____,
};

// $75B2
static const u8 stage1_bitmap_tree_middle_24x5[3 * 2 * 5] = {
  X_______, _XX_XXXX, ________, X_XXXXXX, ___XXXXX, _XX_____,
  ________, XX_XX_X_, ________, XXX_XX_X, __XXXXXX, XX______,
  ________, XXXXXX_X, ________, X_XXXXXX, ___XXXXX, XXX_____,
  X_______, _X_XXXXX, ________, XXXX_XX_, ___XXXXX, XXX_____,
  X_______, _XXX_XXX, ________, _XX_XXXX, __XXXXXX, XX______,
};

// $75D0
static const u8 stage1_bitmap_tree_bottom_24x2_another[3 * 2 * 2] = {
  XX_X_XX_, __X_X__X, ____XX_X, XX_X__X_, _XXXXXXX, X_______,
  X_______, _X_XXXX_, ________, XXXX_XXX, __XXXXXX, _X______,
};

// $75DC
static const u8 stage1_bitmap_tree_trunk_24x3[3 * 2 * 3] = {
  XXXXXXXX, ________, ____XXXX, X__X____, XXXXXXXX, ________,
  XXXXXXX_, _______X, ____XXXX, X_XX____, XXXXXXXX, ________,
  XXXXXXX_, _______X, ____XXXX, _X_X____, XXXXXXXX, ________,
};

// $75EE
static const u8 stage1_bitmap_tree_shadow_24x1[3 * 2 * 1] = {
  ________, XXXXXXX_, ________, _XX_XXXX, ____XXXX, XXXX____,
};

// $75F4
static const u8 stage1_bitmap_tree_middle_24x5s[3 * 2 * 5] = {
  XXXXX___, _____XX_, ________, XXXXX_XX, _______X, XXXX_XX_,
  XXXX____, ____XX_X, ________, X_X_XXX_, ______XX, XX_XXX__,
  XXXX____, ____XXXX, ________, XX_XX_XX, _______X, XXXXXXX_,
  XXXXX___, _____X_X, ________, XXXXXXXX, _______X, _XX_XXX_,
  XXXXX___, _____XXX, ________, _XXX_XX_, ______XX, XXXXXX__,
};

// $7612
static const u8 stage1_bitmap_tree_bottom_24x2s[3 * 2 * 2] = {
  XXXXXX_X, ______X_, _XX_____, X__XXX_X, XX_X_XXX, __X_X___,
  XXXXX___, _____X_X, ________, XXX_XXXX, ______XX, _XXX_X__,
};

// $761E
static const u8 stage1_bitmap_tree_trunk_24x3s[3 * 2 * 3] = {
  XXXXXXXX, ________, XXXX____, ____X__X, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ___XX_XX, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ___X_X_X, XXXXXXXX, ________,
};

// $7630
static const u8 stage1_bitmap_tree_shadow_24x1s[3 * 2 * 1] = {
  XXXX____, ____XXXX, ________, XXX__XX_, ________, XXXXXXXX,
};

/* ----------------------------------------------------------------------- */

static const struct { u16 z80; const void *ptr; } stage1_map_goto_table[] = {
  { 0x5FD2, &stage1_map_left_curvature[0]    },
  { 0x5FE6, &stage1_map_left_height[0]       },
  { 0x6003, &stage1_map_left_lanes[0]        },
  { 0x6009, &stage1_map_left_hazards[0]      },
  { 0x6011, &stage1_map_left_leftobjs[0]     },
  { 0x6052, &stage1_map_left_rightobjs[0]    },
  { 0x6088, &stage1_map_right_curvature[0]   },
  { 0x60A3, &stage1_map_right_height[0]      },
  { 0x60E5, &stage1_map_right_lanes[0]       },
  { 0x60EF, &stage1_map_right_hazards[0]     },
  { 0x6109, &stage1_map_right_leftobjs[0]    },
  { 0x6143, &stage1_map_right_rightobjs[0]   },
  { 0x6173, &stage1_map_tunnel_curvature[0]  },
  { 0x6189, &stage1_map_tunnel_height[0]     },
  { 0x61A5, &stage1_map_tunnel_lanes[0]      },
  { 0x61B9, &stage1_map_tunnel_hazards[0]    },
  { 0x61BE, &stage1_map_tunnel_leftobjs[0]   },
  { 0x61C9, &stage1_map_tunnel_rightobjs[0]  },
  { 0x61D4, &stage1_map_loop_curvature[0]    },
  { 0x6209, &stage1_map_loop_height[0]       },
  { 0x625D, &stage1_map_loop_lanes[0]        },
  { 0x6277, &stage1_map_loop_hazards[0]      },
  { 0x6294, &stage1_map_loop_leftobjs[0]     },
  { 0x6314, &stage1_map_loop_rightobjs[0]    },
};

const void *stage1_lookup_map_goto(chqstate_t *state, u16 z80)
{
  size_t i;

  for (i = 0; i < sizeof(stage1_map_goto_table) / sizeof(stage1_map_goto_table[0]); i++)
    if (stage1_map_goto_table[i].z80 == z80)
      return stage1_map_goto_table[i].ptr;
  assert("Unknown Z80 address (stage 1)" == NULL);
  return NULL;
}
