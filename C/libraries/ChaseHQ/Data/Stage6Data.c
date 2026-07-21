/**
 * Stage6Data.c
 */

#include <assert.h>
#include <stddef.h>

#include "C99/Types.h"
#include "ZXSpectrum/Pixels.h"
#include "ZXSpectrum/Spectrum.h"

#include "ChaseHQ/Engine/Types.h"
#include "CommonData.h"

#include "Stage6Data.h"

/* ----------------------------------------------------------------------- */

/* Stage 6 object type macros */
#define MAP_OBJ_S6_TUNNEL_LIGHT_VAL  (1)

#define MAP_OBJ_S6_NONE(D)           (((D) << 4) | MAP_OBJ_NONE_VAL)
#define MAP_OBJ_S6_TUNNEL_LIGHT(D)   (((D) << 4) | MAP_OBJ_S6_TUNNEL_LIGHT_VAL)

/* ----------------------------------------------------------------------- */

/* Forward declarations */
static const u8 stage6_perp_description[7];
static const char *stage6_chatter_strings[4];
static const u8 stage6_arrest_messages[83];
static const hittable_t stage6_hittable_objects[2];
static const obj_t stage6_right_hand_objects[7];
static const obj_t stage6_left_hand_objects[7];

static const u8 curvature[];
static const u8 height[];
static const u8 lanes[];
static const u8 hazards[];
static const u8 leftside[];
static const u8 rightside[];

static const u8 stage6_perp_face[180];
static const bitmap_t stage6_veh3[6];
static const bitmap_t stage6_veh2[6];
static const bitmap_t stage6_veh1[6];

static const bitmap_t stage6_lods_D620[6];

static const u8 stage6_bitmap_C960[1329];
static const u8 stage6_bitmap_D64A[181];

/* ----------------------------------------------------------------------- */

const stage_t stage6 = {
  // clang-format off
  {
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    X_XXXXXX, X_X___XX, XX______, X_X_X_X_, X_X_XXX_, X_X_X___, X___XXX_, X_X_X_X_, X_X_X_X_, XXXXXXX_,
    XXXXX___, _X_X_X_X, _XX__X__, _____X_X, _X_X_XX_, _X______, _____X__, _X_X_X__, XX______, _X_XXX_X,
    XXX_____, X_X_X_XX, X_____X_, X_X_X_X_, X_X_X_XX, X_X_X_X_, X_X_____, __X_X_X_, _XX___X_, X_XXX_XX,
    X_______, ___X_X_X, XX__X___, ___X_X__, _X_X_X_X, XX_X____, ________, ___X___X, __XX____, ___XXXXX,
    __X_____, X_X_XXXX, _____XX_, X_X_X_X_, XXX_XXX_, XXXXX_X_, X_X_X_X_, X___X___, XX_XX___, X_X_XXXX,
    ________, _X_X_X_X, X_______, _X_X_X_X, _X_X_X_X, _X_XXX_X, _X___X_X, ________, __XXXX__, _X_XXXX_,
    X_____X_, X_X_XXX_, __X_X_X_, X_X___XX, XXX_XXXX, X_X_XXX_, X_X_X_X_, X_X___X_, ___XXXX_, __XXXX__,
    ________, ___XX_XX, __XXX__X, ___X_X_X, _X_XXX_X, _X_XXXXX, ___X___X, _X_X_X__, __X____X, _X_XXX_X,
    ____X_X_, XXX_XX__, ___X__X_, __X_XXXX, XXXXXXXX, XXXXX_X_, X___X_XX, X_X_X_X_, X_X_X___, X_XXX_X_,
    ________, _X_XX_XX, _X___X__, _X_X_X_X, XX_XXXXX, _X_X_X__, ________, XX______, _X______, _XXX_X_X,
    ______X_, X_X_XX__, __X_X___, X_XXX_XX, X_XXXXXX, XXXXX_X_, X___X__X, XXX_X___, X_X_X_X_, X_X___X_,
    ________, _X_XXXX_, _X______, _X_X_XXX, XXX_XXXX, _X_X____, X__X___X, XXXX____, _X_XX___, ________,
    ____X_X_, XXXXX_XX, X_X_____, X_X_XXX_, XXXXXX_X, X_X_X_X_, XX_X__XX, XXXXXX__, ____XX__, X_X_X_X_,
    __X____X, XXXX_X_X, _X__X__X, _X_XXX_X, X_XXXXXX, _X_X_X__, XX_X_XXX, XX_XXXXX, __X__XXX, _X___X__,
    ____X_X_, XXXXXXX_, X_X_____, __XXXXXX, XXXXX___, XXX_X_X_, XXX__XXX, XXXXX_X_, XX__X_X_, X_X_X_X_,
    __X__X_X, XX____XX, XX_X___X, _XXX_XXX, XXX_____, ___X_XX_, _XX_XXXX, _X_XXXXX, XXXXX__X, XX_X_X_X,
    X_XXXXXX, X_______, __XXXX__, XX_XXXXX, ________, ____XXXX, ____XXXX, XXXXX___, _____XXX, XXX_XXX_,
    _XXXXXX_, ________, ____XXX_, _XXXXX__, ________, ______XX, X__XXX_X, XX______, ________, _XXXXXXX,
    XXXXX___, ________, ______XX, XXXXX___, ________, _______X, XXX_XXXX, ________, ________, __XXX_XX,
    XXX_____, ________, ________, XXX_____, ________, ________, _XXXXX__, ________, ________, _______X,
    ________, ________, ________, ________, ________, ________, __XX____, ________, ________, ________,
    ________, ________, ________, ________, ________, ________, ________, ________, ________, ________,
    ________, ________, ________, ________, ________, ________, ________, ________, ________, ________,
  },
  // clang-format on

  &stage6_perp_face[FACEBITMAPBYTES],
  NULL,
  attribute_BRIGHT_BLACK_OVER_GREEN * 0x0101,
  &stage6_hittable_objects[0],
  &stage6_right_hand_objects[-1].arg,
  &stage6_right_hand_objects[-1],
  &stage6_right_hand_objects[2],
  &stage6_left_hand_objects[-1].arg,
  &stage6_left_hand_objects[-1],
  &stage6_left_hand_objects[2],
  &stage6_perp_description[0],
  &stage6_arrest_messages[0],
  NULL,
  NULL,
  NULL,
  NULL,
  &stage6_veh3[0],
  { &stage6_veh1[0], &stage6_veh2[0], &stage6_veh1[0], &stage6_veh3[0] },

  /* $C11A difficulty */
  20,  /* car_spawn_delay */
  40,  /* perp_lane_change_base */
  40,  /* perp_approach_base */

  /* $C11D setupdata */
  {
    158,
    &curvature[-1],
    &height[-1],
    &lanes[-1],
    &rightside[-1],
    &leftside[-1],
    &hazards[-1],
  },

  /* $C12B attractdata */
  {
    234,
    &curvature[-1],
    &height[-1],
    &lanes[-1],
    &rightside[-1],
    &leftside[-1],
    &hazards[-1],
  },

  stage6_chatter_strings
};

/* ----------------------------------------------------------------------- */

// $C139
static const u8 stage6_perp_description[7] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_PERP_DESC_1,
  CHATTERCMD_PAUSE,
  CHATTERBLK_HEROES_ACKNOWLEDGE
};

static const char *stage6_chatter_strings[4] = {
  "THIS IS A TEST LEVEL\xAE",
};

// $C1E6
// clang-format off
static const u8 stage6_arrest_messages[83] = {
  6,  // initial delay

  6,  // delay
  DRAWCHARSTYLE_SINGLE,
  0,  // attribute
  TWOBYTES(0xF802),  // backbuf
  TWOBYTES(0x5922),  // attr
  'O', 'K', '!', ' ', 'Y', 'O', 'U', ' ', 'A', 'R', 'E', ' ', 'U', 'N', 'D', 'E', 'R', ' ', 'A', 'R', 'R', 'E', 'S', 'T' | EOS,

  TRANSITIONCONTROL_FILL_ATTRIBUTES,  // transition_control
  DRAWOVERLAY_STOP
};
// clang-format on

// $C239
static const hittable_t stage6_hittable_objects[2] = {
  { 32, &stage6_lods_D620[0] },
  { 32, &stage6_lods_D620[0] },
};

// $C23F
static const obj_t stage6_right_hand_objects[7] = {
  { 111, 41, 80, &tunnellight, draw_tunnel_light_right },
};

// $C270
static const obj_t stage6_left_hand_objects[7] = {
  { 126, 188, 80, &tunnellight, draw_tunnel_light_left },
};

// $C65C
static const u8 curvature[] = {
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_LEFT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(15),
  MAP_CMD_GOTO(0xC65C)
};

// $C684
static const u8 height[] = {
  MAP_HEIGHT_UP7(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_DOWN7(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_CMD_GOTO(0xC684)
};

// $C6BB
static const u8 lanes[] = {
  MAP_LANES_2L(15),

  MAP_LANES_2LTO3L(2),
  MAP_LANES_3L(13),

  MAP_LANES_3LTO4(2),
  MAP_LANES_4(13),

  MAP_LANES_4TO3R(2),
  MAP_LANES_3R(13),

  MAP_LANES_3RTO2R(2),
  MAP_LANES_2R(13),

  MAP_LANES_2RTO3R(2),

  MAP_CMD_GOTO(0xC6BB)
};

// $C6D1
static const u8 hazards[] = {
  MAP_HAZARD_WAIT(252),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_CMD_GOTO(0xC6D1)
};

// $C6EE
static const u8 leftside[] = {
  MAP_OBJ_S6_NONE(15),
  MAP_CMD_GOTO(0xC6EE)
};

// $C7A5
static const u8 rightside[] = {
  MAP_OBJ_S6_NONE(15),
  MAP_CMD_GOTO(0xC7A5)
};

// $C82E
// clang-format off
static const u8 stage6_perp_face[180] = {
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
// clang-format on

// $C8E2
static const bitmap_t stage6_veh3[6] = {
  { 6, BITMAPFLAG_DEFAULT, 30, &stage6_bitmap_C960[0], &stage6_bitmap_C960[0] },  // [0]
  { 5, BITMAPFLAG_DEFAULT, 22, &stage6_bitmap_C960[180], &stage6_bitmap_C960[180] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 15, &stage6_bitmap_C960[290], &stage6_bitmap_C960[290] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 15, &stage6_bitmap_C960[290], &stage6_bitmap_C960[290] },  // [3]
  { 3, BITMAPFLAG_MASKED, 8, &stage6_bitmap_C960[335], &stage6_bitmap_C960[335] },  // [4]
  { 3, BITMAPFLAG_MASKED, 8, &stage6_bitmap_C960[335], &stage6_bitmap_C960[383] },  // [5]
};

// $C90C
static const bitmap_t stage6_veh2[6] = {
  { 6, BITMAPFLAG_DEFAULT, 39, &stage6_bitmap_C960[431], &stage6_bitmap_C960[431] },  // [0]
  { 5, BITMAPFLAG_DEFAULT, 29, &stage6_bitmap_C960[665], &stage6_bitmap_C960[665] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 20, &stage6_bitmap_C960[810], &stage6_bitmap_C960[810] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 20, &stage6_bitmap_C960[810], &stage6_bitmap_C960[810] },  // [3]
  { 2, BITMAPFLAG_MASKED, 12, &stage6_bitmap_C960[870], &stage6_bitmap_C960[918] },  // [4]
  { 2, BITMAPFLAG_MASKED, 12, &stage6_bitmap_C960[870], &stage6_bitmap_C960[918] },  // [5]
};

// $C936
static const bitmap_t stage6_veh1[6] = {
  { 6, BITMAPFLAG_DEFAULT, 30, &stage6_bitmap_C960[966], &stage6_bitmap_C960[966] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 20, &stage6_bitmap_C960[1146], &stage6_bitmap_C960[1146] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 13, &stage6_bitmap_C960[1226], &stage6_bitmap_C960[1226] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 13, &stage6_bitmap_C960[1226], &stage6_bitmap_C960[1226] },  // [3]
  { 2, BITMAPFLAG_MASKED, 8, &stage6_bitmap_C960[1265], &stage6_bitmap_C960[1265] },  // [4]
  { 2, BITMAPFLAG_MASKED, 8, &stage6_bitmap_C960[1265], &stage6_bitmap_C960[1297] },  // [5]
};

// $C960
// clang-format off
static const u8 stage6_bitmap_C960[1329] = {
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____, __XXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, _XXXXXX_, _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, __XXXXXX,
  _XXXX__X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX__XXXX, __XXX_XX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXX_, ___XXXX_, X_X_X_XX, X_X_X_X_, X_X_X_X_,
  XXX_X_X_, X_XXXX__, ___X_X__, _____XXX, XXXXXXXX, XXXXXXXX, XXXX____, ___X_X__,
  __X___X_, X_XXXXXX, _XX_XXXX, XXXXXXX_, XX_XXXX_, X_X___X_, __X_XXXX, XXXXXXX_,
  _X__XXX_, X_X_XX__, X__XXXXX, XXXXX_X_, __XXXXXX, XXXXXXXX, XXXXXX_X, _X_X_XXX,
  XXXXXXXX, XXXXXXX_, __XXXXXX, XXXX_XXX, X_X__X__, ____X_X_, XXXXXXXX, XXXXXXX_,
  __XX_XXX, __X_X_X_, X_______, ________, X_XXXXXX, XXXXXXX_, __X_X_X_, _X_X_XXX,
  ________, ________, _XX____X, XXXXXXX_, __X___X_, XXXXXXXX, _X_X_X_X, _X_X_X_X,
  _X_XXXX_, _XXXXXX_, __X__X_X, XX____XX, X_X_X_X_, X_X_X_X_, X_X____X, X__XX_X_,
  _X_X_XXX, __X_X__X, XXXXXXXX, XXXXXXXX, XX__X_X_, _XX_XX_X, _XX_XX__, _______X,
  __X_X_X_, X_X_X_X_, _X______, ___XX_XX, _XXX_X__, _______X, __XX_X_X, _X_X_XX_,
  _X______, ___XXXXX, _XXX_XXX, _X_X_X_X, XXXXXXXX, XXXXXXXX, XX_X_X_X, _XXXXXXX,
  __XXX__X, XX____XX, X_______, ________, XXX____X, XX_XXXX_, ____X___, _XXXXXXX,
  XXX_X_X_, X_X_X_XX, XXXXXXXX, ____X___, ____XX_X, ______XX, XXXXXXXX, XXXXXXXX,
  XXX_____, _X_XX___, ______XX, X_XX___X, _X_X_X_X, _X_X_X_X, _X___XX_, XXX_____,
  __XXX___, XXXXXXXX, XXX_____, ______XX, XXXXXXXX, X___XXX_, _X__XX__, _XXX_X_X,
  _XXXXXXX, XXXXXXXX, _X_X_XXX, ___XX__X, _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXX_X, _X__X___, ________, ________, ________, ________, ____X__X,
  _XX__XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX__XX, ___XXX__, ________,
  ________, ________, ________, ___XXX__, _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX_____, __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___, __XXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXX__, __XX__X_, XXXXXXXX, XXXXXXXX, XXXXXXX_, X__XXX__,
  __XX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XX___, ___XX___, ___XXXXX, XXXXXXXX,
  XXXX____, __XX____, __X__X_X, _XXXX_XX, _XXXXXXX, _XX_XX_X, _X__X___, __X_XXXX,
  XXXX__X_, _XX__XX_, _X__XXXX, XXX_X___, __XXXXXX, _X_XXXXX, XX____XX, XXXXXXXX,
  XXXXX___, __XX_XX_, __X__X_X, _______X, _X_XXXXX, XXXXX___, __X_X___, _XXXX___,
  ________, __XXXXXX, XXXXX___, __X____X, X___XX_X, _X_X_X_X, _XX___XX, X_X_X___,
  ___X_XX_, __X_XXXX, XXXXXXXX, XXX_X___, XX_X_X__, __XX_X__, ____X__X, _X_X_X_X,
  __X_____, _XXXXX__, __XX_XXX, ___XXXXX, XXXXXXXX, XXXX___X, XXXXXX__, ____X___,
  XXXXXXX_, X_X_X_X_, XXXXXXX_, ___X____, _____XX_, ____XXXX, XXXXXXXX, XXXX____,
  _XX_____, ______XX, _XX_XXXX, _X__X__X, _XXX_XX_, XX______, __X__X__, XX_XX_XX,
  XXXXXXXX, XX_XX_XX, __X__X__, __X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_X__,
  __XX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XX__, ___XX___, ________, ________,
  ________, __XX____, _XXXXXXX, XXXXXXXX, XXXXXXX_, XXX__XXX, XXXXXXXX, XXX__XXX,
  XX_XXXXX, XXXXXXXX, XXXXX_XX, _XX___XX, XXXXXXXX, XX___XX_, XX_XXXXX, _X_XX_X_,
  XXXXX_X_, XXXXXXXX, XXX__XXX, XXXXXXXX, XX__XXXX, ________, _XXXXXXX, X__XX__X,
  XXXXXXXX, X__XXXXX, X_X____X, ________, X____XXX, X_XXX_XX, XXXXXXXX, XX_XXXXX,
  _XX__XX_, XXXXXXXX, _XXX_XX_, ___X___X, X_X__X_X, X_XXX___, XXX_X_X_, XXXXXXXX,
  XXXX_XXX, X_XXXXXX, XXXXXXXX, XXXXXX_X, _XX_____, ________, _____XX_, ________,
  XXXXXXXX, _______X, XXXXXXX_, XXXXXXXX, ________, X_______, _XXXXXXX, ______XX,
  XXXXXX__, XXXXXXXX, ________, ________, XX_XXX_X, _______X, _X_XXXX_, XXXXXXXX,
  ________, ________, X_XX_XXX, _______X, XXX_XXX_, XXXXXXXX, ________, ________,
  X_X__XX_, _______X, X_X__XX_, XXXXXXXX, ________, X_______, _X_XXXXX, ______XX,
  XXXXXX__, XXXXXXXX, ________, _X______, X_XXX_X_, _____X_X, X_XXX_X_, XXXXXXXX,
  ________, ________, XXXXXXXX, _______X, XXXXXXX_, XXXXXXXX, ________, XXXX____,
  ____XXXX, ________, XXXXXXXX, ___XXXXX, XXX_____, XXXXX___, _____XXX, ________,
  XXXXXXXX, __XXXXXX, XX______, XXXX____, ____XX_X, ________, XX_X_X_X, ___XXXXX,
  XXX_____, XXXX____, ____X_XX, ________, _XXXXXX_, ___XXXXX, XXX_____, XXXX____,
  ____X_X_, ________, _XX_X_X_, ___XXXXX, _XX_____, XXXXX___, _____X_X, ________,
  XXXXXXXX, __XXXXXX, XX______, XXXX_X__, ____X_XX, ________, X_X_X_XX, _X_XXXXX,
  X_X_____, XXXX____, ____XXXX, ________, XXXXXXXX, ___XXXXX, XXX_____, ____XXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____, ___XXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXX___, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXX_, _XXXXX__, X_X_XXXX, XXXXXXXX, XXXXXXXX, XXXX_X_X, __XXXXX_, _XXXX__X,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, X__XXXX_, _XXXX_XX, XXXXXXXX, XX______,
  _______X, XXXXXXXX, XX_XXXX_, __XXX_XX, XXXXXXXX, XX______, _______X, XXXXXXXX,
  XX_XXX__, __XXX_XX, XXXXX___, XXX_____, ______XX, X___XXXX, XX_XXX__, ___XX_XX,
  XXXX_X_X, XX_X_X_X, _X_X_X_X, XX_X_XXX, XX_XX___, ____XXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXX____, ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXX___, ___X____, ________, ________, ________, ________, ____X___, __X_X_X_,
  X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, _X_X_X__, __XX_X_X, _X___X_X, _X_X_X_X,
  _X_X_X_X, _X_X__X_, X_X_XX__, __X_XXX_, ________, ________, ________, ________,
  _XXX_X__, __XXXX__, ________, ________, ________, ________, __XXXX__, _X___XX_,
  ________, ________, ________, ________, _XX___X_, _X___X__, ________, ________,
  ________, ________, __X___X_, _X___XX_, ________, ________, ________, ________,
  _XX___X_, __XXXXX_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_XXXXX_, _X___XXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX___X_, _X______, ________, ________,
  ________, ________, ______X_, _X______, ________, ________, ________, ________,
  ______X_, __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__, ___XXXX_,
  _XX_X_X_, X_X_X_XX, X_X_X_X_, X_X_X_X_, _XXXX___, ______XX, X_XXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXX_X, XX______, ______XX, XX______, XX_____X, X_____XX, ______XX,
  XX______, _____XX_, __XXXXXX, __XXXXX_, _XXXXX__, XXXXXX__, _XX_____, ____X_XX,
  X_X_X_XX, __X_X_X_, _X_X_X__, XX_X_X_X, XX_X____, ____X__X, _X_XXXXX, __XXXXX_,
  _XXXXX__, XXXXX_X_, X__X____, ___X___X, _X_XXXXX, __XXXXX_, _XXXXX__, XXXXX_X_,
  X___X___, ___X___X, _X_XXXXX, __XXXXX_, _XXXXX__, XXXXX_X_, X___X___, ___X___X,
  __X_XXXX, __XXXXX_, _XXXXX__, XXXX_X__, X___X___, ___X___X, X_X_XXXX, __XXXXX_,
  _XXXXX__, XXXX_X_X, X___X___, ____X_X_, _XX_XXXX, __XXXXX_, _XXXXX__, XXXX_XX_,
  _X_X____, _____XXX, X__X_XXX, __XXXXX_, _XXXXX__, XXX_X__X, XXX_____, ________,
  _XXX____, ________, ________, ____XXX_, ________, ________, ____X___, ________,
  ________, ___X____, ________, ________, _____XXX, XXXXXXXX, XXXXXXXX, XXX_____,
  ________, _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____, ____XXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXX____, ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  ___XXX_X, _XXXXXXX, XXXXXXXX, XXXXXXX_, X_XXXX__, ___XX_XX, XXXXXXXX, ________,
  _XXXXXXX, XX_XXX__, ___XX_XX, XXXX__XX, ________, _XX__XXX, XX_XX___, ____X_XX,
  XXX_XXXX, X_X___X_, XXXXX_XX, XX_X____, _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX_____, ____X___, ________, ________, ________, ___X____, ___X_XX_, X_XXXXXX,
  XXXXXXXX, XXXXXX_X, _XX_X___, ___X_X_X, _X______, ________, ______X_, X_X_X___,
  ___XXX__, ________, ________, ________, __XXX___, __X___X_, ________, ________,
  ________, _X___X__, __X___X_, ________, ________, ________, _X___X__, __XXXXX_,
  X_X_X_X_, X_X__X_X, _X_X_X_X, __XXXX__, __X_____, ________, ________, ________,
  _____X__, __X_____, ________, ________, ________, _____X__, ___XXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXX___, ______XX, X_XXXXXX, XXXXXXXX, XXXXXX_X, XX______,
  ______XX, XX____X_, ___XX___, _X____XX, XX______, _____XXX, X__X_X_X, X_X__X_X,
  X_X_X__X, XXX_____, _____X_X, __XXXX_X, XXX__XXX, X_XXXX__, X_X_____, ____X__X,
  __XXXX_X, XXX__XXX, X_XXXX__, X__X____, ____X__X, _X_XXX_X, XXX__XXX, X_XXX_X_,
  X__X____, ____X__X, XX_XXX_X, XXX__XXX, X_XXX_XX, X__X____, ______XX, X_X_XX_X,
  XXX__XXX, X_XX_X_X, XX______, ________, _XX_____, ________, _____XX_, ________,
  ________, ___X____, ________, ____X___, ________, ________, ____XXXX, XXXXXXXX,
  XXXX____, ________, _XXXXXXX, XXXXXXXX, XXXXXXX_, XXX_XXXX, XXXXXXXX, XXXX_XXX,
  XX_XXXXX, X______X, XXXXX_XX, _X_XXXX_, XX____XX, _XXXX_X_, __XXXXXX, XXXXXXXX,
  XXXXXX__, _X______, ________, ______X_, _XXXXXXX, XXXXXXXX, XXXXXXXX, X_X_X___,
  ________, ___X_X_X, X_X_____, ________, _____X_X, _XX_X_X_, X_X_X_X_, X_X_XXX_,
  X_______, ________, _______X, _XXXXXXX, XXXXXXXX, XXXXXXX_, ___XXX__, ________,
  ___XX___, ___X___X, X_XX_XX_, XX___X__, __X_XX_X, X_XX_XX_, XX_XX_X_, __X_X__X,
  X_XX_XX_, XX__X_X_, __X_X__X, X_XX_XX_, XX__X_X_, ___XXXX_, X_XX_XX_, X_XXXX__,
  _______X, ________, _X______, ________, XXXXXXXX, X_______, ________, XXXXXXXX,
  _____XXX, XXXXX___, ________, XX_XXXXX, _____XXX, XX_XX___, ________, XXXXX___,
  _____XXX, XXXXX___, ________, X_X_X_X_, _____XXX, X_X_X___, ________, X_______,
  _____XXX, ____X___, ________, X_______, _____XXX, ____X___, ________, XXX_X_X_,
  _____XXX, X_XXX___, X_______, _X_X_X_X, ____XXXX, _X_X____, ________, X__X_X_X,
  _____XXX, _X__X___, ________, XX_X_X_X, _____XXX, _X_XX___, X_______, _XX_____,
  ____XXXX, __XX____, XXX_____, ___XXXXX, __XXXXXX, XX______, XXX_____, ___XXXXX,
  ________, XXXXXXXX, XXX_____, ___XX_XX, ________, XXXXX_XX, XXX_____, ___XXXXX,
  ________, ___XXXXX, XXX_____, ___X_X_X, ________, _X_X_X_X, XXX_____, ___X____,
  ________, _______X, XXX_____, ___X____, ________, _______X, XXX_____, ___XXX_X,
  ________, _X_X_XXX, XXXX____, ____X_X_, _______X, X_X_X_X_, XXX_____, ___X__X_,
  ________, X_X_X__X, XXX_____, ___XX_X_, ________, X_X_X_XX, XXXX____, ____XX__,
  _______X, _____XX_, XXXXXX__, ______XX, _____XXX, XXXXX___, _____XXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____, __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXX_, __XXXXX_, _X_X_XXX, XXXXXXXX, XXXXXXXX, XXXX_X_X, __XXXXX_,
  _XXXXX_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XXXXX, _XXXXX_X, __X_X_X_,
  X_X_X_X_, X_X_X_X_, X_X_X_X_, _X_XXXXX, _XXXXX__, _X_X____, ___XXXXX, XXXXXXX_,
  _____X_X, ___XXXXX, __XXX_XX, XXXXXXXX, XXXX____, ______XX, XXXXXXXX, XXX_XXX_,
  __XX_X_X, _X_X_X_X, _X_X____, ______XX, _X_X_X_X, _X_X_XX_, _XX_X_X_, ________,
  ___X____, ______X_, ________, __X_X_XX, _X______, ________, ___XXXXX, XXXXXXX_,
  ________, _______X, _X______, ________, ________, ________, ________, _______X,
  _X____X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X____X, __X_XXXX, XXXXXXXX,
  _______X, XX______, _XXXXXXX, XXXXX_X_, __X_X__X, XXXX_XXX, ______XX, X_______,
  _XXX_XXX, XX__X_X_, __X_X_XX, X_X_X_XX, _______X, XX______, _XX_X_X_, XXX_X_X_,
  __X_X__X, XX_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, XX__X_X_, __X__X_X, X_X___XX,
  XXXXXXXX, XXXXXXXX, XXX___X_, XX_X__X_, ___X__XX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXX__X__, ___X____, ________, ________, ________, ________, _____X__,
  ___X__X_, X_______, ________, ________, ________, __X__X__, ___X_X__, __XXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXX_, ___X_X__, ___X_XXX, __X_X_X_, X_X_X_X_, X_X_X_X_,
  X_X_X_X_, _XXX_X__, ___XXX_X, X__X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X__, XX_XXX__,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__, ___X____, ________,
  ________, ________, ________, _____X__, ___X____, ________, ________, ________,
  ________, _____X__, ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  ________, ____X___, ________, ________, ____X___, ________, ________, _____XX_,
  ________, ________, __XX____, ________, ________, _______X, XXXXXXXX, XXXXXXXX,
  XX______, ________, __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__, _XXX__X_, XXXXXXXX,
  XXXXXXX_, X__XXXX_, XXX_XXXX, XXXXXXXX, XXXXXXXX, XXX_XXXX, XXXXX_X_, X_X_XXXX,
  XXX_X_X_, X_X_XXXX, _X_X_X_X, _X_XX___, __XX_X_X, _X_X__X_, X_______, ____X___,
  __X_____, _______X, X_______, ________, ________, _______X, X__XXX_X, X_XX_XX_,
  XX_XX_XX, _XX_X__X, X_X_XXXX, XXX_____, ____XXXX, XXXX_X_X, X_X_X_X_, _XX_____,
  ____XX__, X_XX_X_X, X__XX_X_, _XXXXXXX, XXXXXX__, X_XXX__X, _X__XXXX, XXXXXXXX,
  XXXXXXXX, XXXX__X_, _X______, ________, ________, ______X_, _XX___XX, XXXXXXXX,
  XXXXXXXX, X____XX_, _XXXX__X, X_XX_XX_, XX_XX_XX, X__XXXX_, _XXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXX_, _X______, ________, ________, ______X_, __XXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXX__, ________, XX______, _____XX_, ________, ________, __XXXXXX,
  XXXXX___, ________, ___XXXXX, XXXXXXXX, XXXXXX__, _XXX___X, XXXXXXXX, XX___XX_,
  _XX_X_X_, X_XXXXX_, X_X_X_XX, _X______, ________, _______X, _X___XXX, XXXXXXXX,
  XXXX___X, _X_XXXXX, X___X___, XXXXXX_X, _X__X___, X___X___, X___X__X, _X_XXXXX,
  XXXXXXXX, XXXXXX_X, __XX____, ________, ______X_, __XXX__X, XXXXXXXX, XX__XXX_,
  __X__XXX, _X_X_X_X, _XXX__X_, ___XXXXX, XXXXXXXX, XXXXXX__, _______X, XXXXXXXX,
  XX______, ________, XXXXXXXX, _____XXX, XXXXX___, ________, XX__XXXX, _____XXX,
  X__XX___, X_______, _XX_X_X_, ____XXXX, X_XX____, ________, X_XXX___, _____XXX,
  XXX_X___, ________, XXXXXXXX, _____XXX, XXXXX___, ________, X_______, _____XXX,
  ____X___, X_______, _XX_X_X_, ____XXXX, X_XX____, XX______, __XXXXXX, __XXXXXX,
  XX______, XXX_____, ___XXXXX, ________, XXXXXXXX, XXX_____, ___XX__X, ________,
  XXXX__XX, XXXX____, ____XX_X, _______X, _X_X_XX_, XXX_____, ___X_XXX, ________,
  ___XXX_X, XXX_____, ___XXXXX, ________, XXXXXXXX, XXX_____, ___X____, ________,
  _______X, XXXX____, ____XX_X, _______X, _X_X_XX_, XXXXX___, _____XXX, _____XXX,
  XXXXX___,
};
// clang-format on

// $D620
static const bitmap_t stage6_lods_D620[6] = {
  { 4, BITMAPFLAG_DEFAULT, 17, &stage6_bitmap_D64A[0], &stage6_bitmap_D64A[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 17, &stage6_bitmap_D64A[0], &stage6_bitmap_D64A[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 13, &stage6_bitmap_D64A[68], &stage6_bitmap_D64A[68] },  // [2]
  { 2, BITMAPFLAG_DEFAULT, 9, &stage6_bitmap_D64A[107], &stage6_bitmap_D64A[107] },  // [3]
  { 2, BITMAPFLAG_DEFAULT, 9, &stage6_bitmap_D64A[107], &stage6_bitmap_D64A[107] },  // [4]
  { 2, BITMAPFLAG_MASKED, 7, &stage6_bitmap_D64A[125], &stage6_bitmap_D64A[153] },  // [5]
};

// $D64A
// clang-format off
static const u8 stage6_bitmap_D64A[181] = {
  ___XXXXX, XX______, ______XX, XXXXX___, ___XXXX_, _X____XX, XX____XX, _X__X___,
  ____XX_X, __X___XX, _X___XX_, X__X____, ____XXX_, X_X___XX, XX___XXX, _X_X____,
  _____XXX, _X_X__XX, _X__XXX_, X_X_____, _____XX_, X__X_XX_, X_X_XX_X, __X_____,
  ______XX, _X__XXXX, __XXX_X_, _X______, ______XX, X_X_XXX_, X_XXXX_X, _X______,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, _XXXXXX_, _XXXXXX_, X_XXXXXX,
  XXXXX___, _XXXXX__, __XXXXX_, ___XXXXX, XXXXX___, _XXXXX__, __XXXXX_, ___XXXXX,
  XXXXX___, _XXXXX__, __XXXXX_, ___XXXXX, XXXXX___, _XXXXX__, __XXXXX_, ___XXXXX,
  X_X_X___, _X_X_X__, __X_X_X_, ___X_X_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  ________, ____XXXX, XXXXX___, ________, __XXXXXX, ________, XXXXXX__, __XXX_XX,
  ___XXX__, XX_X_X__, ___XXX_X, X__X_X_X, X_X_X___, ___XX_X_, X_XX_X_X, XX__X___,
  ____XX_X, X_X_X_XX, X__X____, ____XXX_, _XXXXXXX, XX_X____, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXX_XX, XXXX_XXX, X_X_XXXX, XXXX___X, XXX__XXX, X___XXXX, XXXX___X,
  XXX__XXX, X___XXXX, X__X___X, __X__X__, X___X__X, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  ________, _XXXXXX_, ________, _XXXX___, ___XXXX_, _XX_X__X, X__XX_X_, __XX_X_X,
  X_XX_X__, __XXXX_X, X_XXXX__, XXXXXXXX, XXXXXXXX, XXX__XXX, __XXX__X, X_X__X_X,
  __X_X__X, XXXXXXXX, XXXXXXXX, ______XX, XX______, X__XXXXX, _XX_____, X__XXXXX,
  _XX_____, X___X__X, _XXX_XX_, ___XXXXX, XXX_____, XX______, __XXXXXX, __XXXXXX,
  XX______, ________, XXXXXXXX, ____XXXX, XXXX____, ________, X__XX__X, ____XXXX,
  X__X____, ________, XXXXXXXX, ____XXXX, XXXX____, XXXX____, ____XXXX, XXXXXXXX,
  ________, XXXXX__X, _____XX_, XXXXX__X, _____XX_, XXXXX___, _____XXX, X__X___X,
  _XX_XXX_, XXXXXX__, ______XX, ______XX, XXXXXX__, XXXX____, ____XXXX, ________,
  XXXXXXXX, XXXX____, ____X__X, ________, X__XX__X, XXXX____, ____XXXX, ________,
  XXXXXXXX, XXXXXXXX, ________, ____XXXX, XXXX____,
};


static const struct { u16 z80; const void *ptr; } stage6_map_goto_table[] = {
  { 0xC65C, &curvature[0] },
  { 0xC684, &height[0] },
  { 0xC6BB, &lanes[0] },
  { 0xC6D1, &hazards[0] },
  { 0xC6EE, &leftside[0] },
  { 0xC7A5, &rightside[0] },
};

const void *stage6_lookup_map_goto(u16 z80)
{
  int lo, hi, mid;

  lo  = 0;
  hi  = (int)NELEMS(stage6_map_goto_table) - 1;
  while (lo <= hi) {
    mid = lo + (hi - lo) / 2;
    if (stage6_map_goto_table[mid].z80 == z80) return stage6_map_goto_table[mid].ptr;
    if (stage6_map_goto_table[mid].z80 < z80)  lo = mid + 1;
    else                          hi = mid - 1;
  }
  assert("Unknown Z80 address (stage 6)" == NULL);
  return NULL;
}
