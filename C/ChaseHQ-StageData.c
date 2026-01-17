// ChaseHQ-StageData.c
//
// Chase H.Q. code model
//
// by dpt

// vim: ts=8 sts=2 sw=2 et

#include <stddef.h>

#include "Types.h"
#include "Pixels.h"
#include "Spectrum.h"
#include "ChaseHQ.h"
#include "ChaseHQ-Data.h"

#include "ChaseHQ-StageData.h"

/* ----------------------------------------------------------------------- */

static const u8 stage1_perp_description[7];
static const char *stage1_chatter_strings[4];
static const hitable_t stage1_hitable_object_defs[2];
static const u8 stage1_perp_face[FACEBYTES];
static const lod_t stage1_car_lods[6];
static const u8 stage1_bitmap_car_1[6 * 31];
static const u8 stage1_bitmap_car_2[5 * 22];
static const u8 stage1_bitmap_car_3[3 * 16];
static const u8 stage1_bitmap_car_4[3 * 2 * 9];
static const u8 stage1_bitmap_car_4s[3 * 2 * 9];
static const lod_t stage1_tumbleweed_lods[6];
static const u8 stage1_bitmap_tumbleweed_1[2 * 16];
static const u8 stage1_bitmap_tumbleweed_2[2 * 11];
static const u8 stage1_bitmap_tumbleweed_3[1 * 9];
static const u8 stage1_bitmap_tumbleweed_4[1 * 7];
static const lod_t stage1_barrier_lods[6];
static const u8 stage1_bitmap_barrier_1[4 * 17];
static const u8 stage1_bitmap_barrier_2[3 * 13];
static const u8 stage1_bitmap_barrier_3[2 * 9];
static const u8 stage1_bitmap_barrier_4[2 * 2 * 7];
static const u8 stage1_bitmap_barrier_4s[2 * 2 * 7];

/* ----------------------------------------------------------------------- */

// $5CF0
static const stage_t stage1 = {
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
  &stage1_hitable_object_defs[0],
  NULL, // &stage1_right_hand_handlers[0],
  NULL, // &stage1_right_hand_objects[0],
  NULL, // &stage1_right_hand_short_pole_object[0],
  NULL, // &stage1_left_hand_handlers[0],
  NULL, // &stage1_left_hand_objects[0],
  NULL, // &stage1_left_hand_short_pole_object[0],
  stage1_perp_description,
  NULL, // addrof_arrest_messages
  NULL, // addrof_helicopter_stuff_1
  NULL, // addrof_helicopter_stuff_2

  NULL, // lods_stones
  NULL, // lods_dust
  &stage1_car_lods[0], // lods_perp_car
  { NULL, NULL, NULL, &stage1_car_lods[0] },

  20, // car_spawn_delay
  0x50, // smash_5d1b
  0x5A, // smash_perp_delay

  {
    0xEA,
    NULL, //&map_start_curvature[-1],
    NULL, //&map_start_height[-1],
    NULL, //&map_start_lanes[-1],
    NULL, //&map_start_rightobjs[-1],
    NULL, //&map_start_leftobjs[-1],
    NULL, //&map_start_hazards[-1]
  },

  {
    0xEA,
    NULL, //&map_loop_curvature[-1],
    NULL, //&map_loop_height[-1],
    NULL, //&map_loop_lanes[-1],
    NULL, //&map_loop_rightobjs[-1],
    NULL, //&map_loop_leftobjs[-1],
    NULL, //&map_loop_hazards[-1]
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

// $5E40
static const hitable_t stage1_hitable_object_defs[2] = {
  { 0x10, &stage1_tumbleweed_lods[0] },
  { 0x20, &stage1_barrier_lods[0] },
};

// $5E46
static const obj_t stage1_right_hand_graphics_defs[9] = {
  { 111, 41, 80, &tunnellight, draw_tunnel_light_right },
  {   0,  0,  0, NULL, NULL },
  //{ 144, 92, 40, stretchy_shortpole, draw_stretchy_object_right },
  //{  94, 36, 60, stretchy_tree_right, draw_stretchy_object_right },
  //{ 110, 49, 80, stretchy_tree_left, draw_stretchy_object_right },
  //{ 110, 53, 80, stretchy_streetlamp_right, draw_stretchy_object_right },
  //{ 110, 53, 80, stretchy_telegraphpole_right, draw_stretchy_object_right },
  //{ 100, 24, 70, turn_sign_left, draw_object_right },
  //{ 100, 24, 70, turn_sign_right, draw_object_right }
};

// $5E85
static const obj_t stage1_left_hand_objects[9] = {
  { 126, 188, 80, &tunnellight, draw_tunnel_light_left },
  {   0,  0,  0,  NULL, NULL },
  //{  96, 144, 40, stretchy_shortpole, draw_stretchy_object_left },
  //{ 140, 196, 60, stretchy_tree_right, draw_stretchy_object_left },
  //{ 124, 208, 80, stretchy_tree_left, draw_stretchy_object_left },
  //{ 132, 182, 80, stretchy_streetlamp_left, draw_stretchy_object_left },
  //{ 132, 182, 80, stretchy_telegraphpole_left, draw_stretchy_object_left },
  //{ 129, 182, 70, turn_sign_left, draw_object_left },
  //{ 129, 200, 70, turn_sign_right, draw_object_left },
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

  0x78, 0x70, 0x30, 0x30,
  0x78, 0x70, 0x70, 0x30,
  0x78, 0x70, 0x70, 0x30,
  0x78, 0x70, 0x30, 0x30,
  0x70, 0x70, 0x30, 0x28
};

/* ----------------------------------------------------------------------- */

// $6492
static const lod_t stage1_car_lods[6] = {
  { 6, LOD_NOMASK, 31, &stage1_bitmap_car_1[0], &stage1_bitmap_car_1[0]  },
  { 5, LOD_NOMASK, 22, &stage1_bitmap_car_2[0], &stage1_bitmap_car_2[0]  },
  { 3, LOD_NOMASK, 16, &stage1_bitmap_car_3[0], &stage1_bitmap_car_3[0]  },
  { 3, LOD_NOMASK, 16, &stage1_bitmap_car_3[0], &stage1_bitmap_car_3[0]  },
  { 3, LOD_MASKED,  9, &stage1_bitmap_car_4[0], &stage1_bitmap_car_4s[0] },
  { 3, LOD_MASKED,  9, &stage1_bitmap_car_4[0], &stage1_bitmap_car_4s[0] },
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

// $6D82
static const lod_t stage1_tumbleweed_lods[6] = {
  { 2, LOD_NOMASK, 16, &stage1_bitmap_tumbleweed_1[0], &stage1_bitmap_tumbleweed_1[0] },
  { 2, LOD_NOMASK, 11, &stage1_bitmap_tumbleweed_2[0], &stage1_bitmap_tumbleweed_2[0] },
  { 1, LOD_NOMASK,  9, &stage1_bitmap_tumbleweed_3[0], &stage1_bitmap_tumbleweed_3[0] },
  { 1, LOD_NOMASK,  9, &stage1_bitmap_tumbleweed_3[0], &stage1_bitmap_tumbleweed_3[0] },
  { 1, LOD_NOMASK,  7, &stage1_bitmap_tumbleweed_4[0], &stage1_bitmap_tumbleweed_4[0] },
  { 1, LOD_NOMASK,  7, &stage1_bitmap_tumbleweed_4[0], &stage1_bitmap_tumbleweed_4[0] }
};

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
static const lod_t stage1_barrier_lods[6] = {
  { 4, LOD_NOMASK, 17, &stage1_bitmap_barrier_1[0], &stage1_bitmap_barrier_1[0] },
  { 4, LOD_NOMASK, 17, &stage1_bitmap_barrier_1[0], &stage1_bitmap_barrier_1[0] },
  { 3, LOD_NOMASK, 13, &stage1_bitmap_barrier_2[0], &stage1_bitmap_barrier_2[0] },
  { 2, LOD_NOMASK,  9, &stage1_bitmap_barrier_3[0], &stage1_bitmap_barrier_3[0] },
  { 2, LOD_MASKED,  9, &stage1_bitmap_barrier_3[0], &stage1_bitmap_barrier_3[0] },
  { 2, LOD_MASKED,  7, &stage1_bitmap_barrier_4[0], &stage1_bitmap_barrier_4s[0] }
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

static const u8 stage1_bitmap_barrier_4[2 * 2 * 7] = {
  X__XXXXX, _XX_____, X__XXXXX, _XX_____,
  X___X__X, _XXX_XX_, ___XXXXX, XXX_____,
  XX______, __XXXXXX, __XXXXXX, XX______,
  ________, XXXXXXXX, ____XXXX, XXXX____,
  ________, X__XX__X, ____XXXX, X__X____,
  ________, XXXXXXXX, ____XXXX, XXXX____,
  XXXX____, ____XXXX, XXXXXXXX, ________,
};

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

const stage_t *stages[MAX_STAGEDATA] = {
  &stage1,
  &stage1,
  &stage1,
  &stage1,
  &stage1
};
