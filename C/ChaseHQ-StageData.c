// ChaseHQ-StageData.c
//
// Chase H.Q. code model
//
// by dpt

// vim: ts=8 sts=2 sw=2 et

#include <stddef.h>
#include <stdint.h>

#include "Types.h"
#include "Pixels.h"
#include "ChaseHQ.h"

#include "ChaseHQ-StageData.h"

static const u8 face_ralph[FACEBYTES];

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
  &face_ralph[FACEBITMAPBYTES],
  NULL,
  0x7070,
  NULL, // addrof_hittable_objects
  NULL, // addrof_right_hand_handlers
  NULL, // addrof_right_hand_objects
  NULL, // addrof_right_hand_short_pole_object
  NULL, // addrof_left_hand_handlers
  NULL, // addrof_left_hand_objects
  NULL, // addrof_left_hand_short_pole_object
  NULL, // addrof_perp_description
  NULL, // addrof_arrest_messages
  NULL, // addrof_helicopter_stuff_1
  NULL, // addrof_helicopter_stuff_2
};

// $642A
static const u8 face_ralph[FACEBYTES] = {
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

const stage_t *stages[MAX_STAGEDATA] = {
  &stage1,
  &stage1,
  &stage1,
  &stage1,
  &stage1
};

