/**
* Bank3Data.c
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

#include "ZXSpectrum/Pixels.h"

#include "C99/Types.h"

#include "ChaseHQ/ChaseHQ.h"
#include "ChaseHQ/Engine/Types.h"

#include "Bank3Data.h"

/* ----------------------------------------------------------------------- */

// clang-format off

/** $D462-$EB9D: title_glyph_bitmaps -- glyph/sprite bitmap data (car, truck, logo letters). */
const u8 title_glyph_bitmaps[5948] = {
  ________, ______XX, XXXXXX__, ________,
  ________, __XXXX__, ______XX, ________,
  ________, XX______, ________, XX______,
  _______X, _____X_X, _X_X_X__, __X_____,
  _______X, __X_X_X_, X_X_X_X_, X_X_____,
  ______X_, _X_X_X_X, _X_X_X_X, ___X____,
  ______X_, X_X_X_X_, X_X_X_X_, X__X____,
  _____XXX, _X_X_XXX, XX_X_X_X, _XX_____,
  _____XX_, X_X_XXX_, XXX_X_X_, X_X_____,
  _____XXX, XXXXXX__, _XXXXXXX, XXX_____,
  ____X_X_, X_X_X___, X_X_X_X_, XX______,
  ____XXXX, XXXXX___, XXXXXXXX, XX______,
  ____X_X_, X_X_X___, X_X_X_X_, X_______,
  ___XXXXX, XXXX___X, XXXXXXXX, X_______,
  ___XXXXX, XXXX____, ________, ________,
  ___XXXXX, XXXX____, ________, ________,
  __XXXXXX, XXX___XX, XXXXXXXX, ________,
  __X_X_X_, X_X___XX, _X_X_X_X, ________,
  __X_____, __X___X_, _______X, ________,
  _XX_X_X_, XX___X_X, _X_X_XX_, ________,
  _X_X_X_X, _XX_XXX_, X_X_X_X_, ________,
  _XXXXXXX, XXXXXXXX, XXXXXXX_, ________,
  XXXXXXXX, XXXXXXXX, XXXXXX__, ________,
  XXXXXXXX, XXXXXXXX, XXXXXX__, ________,
  _X_XXXXX, XXXXXXXX, XXXXX___, ________,
  _X__X_X_, X_X_X_X_, X__X____, ________,
  __XX____, ________, _XX_____, ________,
  ____XXXX, XXXXXXXX, X_______, ________,

  ________, ________, XXXXXXXX, ________,
  ________, ____XXXX, ________, XX______,
  ________, __XX____, ________, __XX____,
  ________, _X_____X, _X_X_X_X, ____X___,
  ________, _X__X_X_, X_X_X_X_, X_X_X___,
  ________, X__X_X_X, _X_X_X_X, _X___X__,
  ________, X_X_X_X_, X_X_X_X_, X_X__X__,
  _______X, XX_X_X_X, XXXX_X_X, _X_XX___,
  _______X, X_X_X_XX, X_XXX_X_, X_X_X___,
  _______X, XXXXXXXX, ___XXXXX, XXXXX___,
  ______X_, X_X_X_X_, __X_X_X_, X_XX____,
  ______XX, XXXXXXX_, __XXXXXX, XXXX____,
  ______X_, X_X_X_X_, __X_X_X_, X_X_____,
  _____XXX, XXXXXX__, _XXXXXXX, XXX_____,
  _____XXX, XXXXXX__, ________, ________,
  _____XXX, XXXXXX__, ________, ________,
  ____XXXX, XXXXX___, XXXXXXXX, XX______,
  ____X_X_, X_X_X___, XX_X_X_X, _X______,
  ____X___, ____X___, X_______, _X______,
  ___XX_X_, X_XX___X, _X_X_X_X, X_______,
  ___X_X_X, _X_XX_XX, X_X_X_X_, X_______,
  ___XXXXX, XXXXXXXX, XXXXXXXX, X_______,
  __XXXXXX, XXXXXXXX, XXXXXXXX, ________,
  __XXXXXX, XXXXXXXX, XXXXXXXX, ________,
  ___X_XXX, XXXXXXXX, XXXXXXX_, ________,
  ___X__X_, X_X_X_X_, X_X__X__, ________,
  ____XX__, ________, ___XX___, ________,
  ______XX, XXXXXXXX, XXX_____, ________,

  ________, ________, __XXXXXX, XX______,
  ________, ______XX, XX______, __XX____,
  ________, ____XX__, ________, ____XX__,
  ________, ___X____, _X_X_X_X, _X____X_,
  ________, ___X__X_, X_X_X_X_, X_X_X_X_,
  ________, __X__X_X, _X_X_X_X, _X_X___X,
  ________, __X_X_X_, X_X_X_X_, X_X_X__X,
  ________, _XXX_X_X, _XXXXX_X, _X_X_XX_,
  ________, _XX_X_X_, XXX_XXX_, X_X_X_X_,
  ________, _XXXXXXX, XX___XXX, XXXXXXX_,
  ________, X_X_X_X_, X___X_X_, X_X_XX__,
  ________, XXXXXXXX, X___XXXX, XXXXXX__,
  ________, X_X_X_X_, X___X_X_, X_X_X___,
  _______X, XXXXXXXX, ___XXXXX, XXXXX___,
  _______X, XXXXXXXX, ________, ________,
  _______X, XXXXXXXX, ________, ________,
  ______XX, XXXXXXX_, __XXXXXX, XXXX____,
  ______X_, X_X_X_X_, __XX_X_X, _X_X____,
  ______X_, ______X_, __X_____, ___X____,
  _____XX_, X_X_XX__, _X_X_X_X, _XX_____,
  _____X_X, _X_X_XX_, XXX_X_X_, X_X_____,
  _____XXX, XXXXXXXX, XXXXXXXX, XXX_____,
  ____XXXX, XXXXXXXX, XXXXXXXX, XX______,
  ____XXXX, XXXXXXXX, XXXXXXXX, XX______,
  _____X_X, XXXXXXXX, XXXXXXXX, X_______,
  _____X__, X_X_X_X_, X_X_X__X, ________,
  ______XX, ________, _____XX_, ________,
  ________, XXXXXXXX, XXXXX___, ________,

  ________, ________, ____XXXX, XXXX____, ________,
  ________, ________, XXXX____, ____XX__, ________,
  ________, ______XX, ________, ______XX, ________,
  ________, _____X__, ___X_X_X, _X_X____, X_______,
  ________, _____X__, X_X_X_X_, X_X_X_X_, X_______,
  ________, ____X__X, _X_X_X_X, _X_X_X__, _X______,
  ________, ____X_X_, X_X_X_X_, X_X_X_X_, _X______,
  ________, ___XXX_X, _X_XXXXX, _X_X_X_X, X_______,
  ________, ___XX_X_, X_XXX_XX, X_X_X_X_, X_______,
  ________, ___XXXXX, XXXX___X, XXXXXXXX, X_______,
  ________, __X_X_X_, X_X___X_, X_X_X_XX, ________,
  ________, __XXXXXX, XXX___XX, XXXXXXXX, ________,
  ________, __X_X_X_, X_X___X_, X_X_X_X_, ________,
  ________, _XXXXXXX, XX___XXX, XXXXXXX_, ________,
  ________, _XXXXXXX, XX______, ________, ________,
  ________, _XXXXXXX, XX______, ________, ________,
  ________, XXXXXXXX, X___XXXX, XXXXXX__, ________,
  ________, X_X_X_X_, X___XX_X, _X_X_X__, ________,
  ________, X_______, X___X___, _____X__, ________,
  _______X, X_X_X_XX, ___X_X_X, _X_XX___, ________,
  _______X, _X_X_X_X, X_XXX_X_, X_X_X___, ________,
  _______X, XXXXXXXX, XXXXXXXX, XXXXX___, ________,
  ______XX, XXXXXXXX, XXXXXXXX, XXXX____, ________,
  ______XX, XXXXXXXX, XXXXXXXX, XXXX____, ________,
  _______X, _XXXXXXX, XXXXXXXX, XXX_____, ________,
  _______X, __X_X_X_, X_X_X_X_, _X______, ________,
  ________, XX______, _______X, X_______, ________,
  ________, __XXXXXX, XXXXXXX_, ________, ________,

  ________, _XXXXXXX, X____XXX, XXXXX___,
  ________, XX______, _X__X___, ____XX__,
  ________, X_______, X___X___, _____X__,
  ________, X_____X_, X___X_X_, _____X__,
  _______X, ___X_X_X, ___X_X_X, _X__X___,
  _______X, __X_X_XX, ___XX_X_, X_X_X___,
  _______X, _X_X_X_X, ___X_X_X, _X_XX___,
  ______X_, X_X_X_X_, __X_X_X_, X_XX____,
  ______XX, _X_X_XX_, __XX_X_X, _X_X____,
  ______XX, XXXXXXX_, __XXXXXX, XXXX____,
  _____X_X, _X_X_X__, _XXX_X_X, _XX_____,
  _____XXX, XXXXXX__, _XXXXXXX, XXX_____,
  _____X_X, _X_X_X__, _X_X_X_X, _XX_____,
  ____XXXX, XXXXXXXX, XXXXXXXX, XX______,
  ____XXXX, XXXXXXXX, XXXXXXXX, XX______,
  ____XXXX, XXXXXXXX, XXXXXXXX, XX______,
  ___XXXXX, XXXXXXXX, XXXXXXXX, X_______,
  ___X_X_X, _X_X___X, X_X_X_X_, X_______,
  ___X____, ___X___X, ________, X_______,
  __XX_X_X, _XX___X_, X_X_X_XX, ________,
  __X_X_X_, X_X___XX, _X_X_X_X, ________,
  __XXXXXX, XXX___XX, XXXXXXXX, ________,
  _XXXXXXX, XX___XXX, XXXXXXX_, ________,
  _X_XXXXX, XX___X_X, XXXXXXX_, ________,
  X__XXXXX, X___X_XX, XXXXXX__, ________,
  X_X_X_X_, X___X__X, _X_X_X__, ________,
  XX_____X, X___XX__, ____XX__, ________,
  _XXXXXXX, _____XXX, XXXXX___, ________,

  ________, ___XXXXX, XXX____X, XXXXXXX_,
  ________, __XX____, ___X__X_, ______XX,
  ________, __X_____, __X___X_, _______X,
  ________, __X_____, X_X___X_, X______X,
  ________, _X___X_X, _X___X_X, _X_X__X_,
  ________, _X__X_X_, XX___XX_, X_X_X_X_,
  ________, _X_X_X_X, _X___X_X, _X_X_XX_,
  ________, X_X_X_X_, X___X_X_, X_X_XX__,
  ________, XX_X_X_X, X___XX_X, _X_X_X__,
  ________, XXXXXXXX, X___XXXX, XXXXXX__,
  _______X, _X_X_X_X, ___XXX_X, _X_XX___,
  _______X, XXXXXXXX, ___XXXXX, XXXXX___,
  _______X, _X_X_X_X, ___X_X_X, _X_XX___,
  ______XX, XXXXXXXX, XXXXXXXX, XXXX____,
  ______XX, XXXXXXXX, XXXXXXXX, XXXX____,
  ______XX, XXXXXXXX, XXXXXXXX, XXXX____,
  _____XXX, XXXXXXXX, XXXXXXXX, XXX_____,
  _____X_X, _X_X_X__, _XX_X_X_, X_X_____,
  _____X__, _____X__, _X______, __X_____,
  ____XX_X, _X_XX___, X_X_X_X_, XX______,
  ____X_X_, X_X_X___, XX_X_X_X, _X______,
  ____XXXX, XXXXX___, XXXXXXXX, XX______,
  ___XXXXX, XXXX___X, XXXXXXXX, X_______,
  ___X_XXX, XXXX___X, _XXXXXXX, X_______,
  __X__XXX, XXX___X_, XXXXXXXX, ________,
  __X_X_X_, X_X___X_, _X_X_X_X, ________,
  __XX____, _XX___XX, ______XX, ________,
  ___XXXXX, XX_____X, XXXXXXX_, ________,

  ________, _____XXX, XXXXX___, _XXXXXXX, X_______,
  ________, ____XX__, _____X__, X_______, XX______,
  ________, ____X___, ____X___, X_______, _X______,
  ________, ____X___, __X_X___, X_X_____, _X______,
  ________, ___X___X, _X_X___X, _X_X_X__, X_______,
  ________, ___X__X_, X_XX___X, X_X_X_X_, X_______,
  ________, ___X_X_X, _X_X___X, _X_X_X_X, X_______,
  ________, __X_X_X_, X_X___X_, X_X_X_XX, ________,
  ________, __XX_X_X, _XX___XX, _X_X_X_X, ________,
  ________, __XXXXXX, XXX___XX, XXXXXXXX, ________,
  ________, _X_X_X_X, _X___XXX, _X_X_XX_, ________,
  ________, _XXXXXXX, XX___XXX, XXXXXXX_, ________,
  ________, _X_X_X_X, _X___X_X, _X_X_XX_, ________,
  ________, XXXXXXXX, XXXXXXXX, XXXXXX__, ________,
  ________, XXXXXXXX, XXXXXXXX, XXXXXX__, ________,
  ________, XXXXXXXX, XXXXXXXX, XXXXXX__, ________,
  _______X, XXXXXXXX, XXXXXXXX, XXXXX___, ________,
  _______X, _X_X_X_X, ___XX_X_, X_X_X___, ________,
  _______X, _______X, ___X____, ____X___, ________,
  ______XX, _X_X_XX_, __X_X_X_, X_XX____, ________,
  ______X_, X_X_X_X_, __XX_X_X, _X_X____, ________,
  ______XX, XXXXXXX_, __XXXXXX, XXXX____, ________,
  _____XXX, XXXXXX__, _XXXXXXX, XXX_____, ________,
  _____X_X, XXXXXX__, _X_XXXXX, XXX_____, ________,
  ____X__X, XXXXX___, X_XXXXXX, XX______, ________,
  ____X_X_, X_X_X___, X__X_X_X, _X______, ________,
  ____XX__, ___XX___, XX______, XX______, ________,
  _____XXX, XXXX____, _XXXXXXX, X_______, ________,

  ________, _______X, XXXXXXX_, ___XXXXX, XXX_____,
  ________, ______XX, _______X, __X_____, __XX____,
  ________, ______X_, ______X_, __X_____, ___X____,
  ________, ______X_, ____X_X_, __X_X___, ___X____,
  ________, _____X__, _X_X_X__, _X_X_X_X, __X_____,
  ________, _____X__, X_X_XX__, _XX_X_X_, X_X_____,
  ________, _____X_X, _X_X_X__, _X_X_X_X, _XX_____,
  ________, ____X_X_, X_X_X___, X_X_X_X_, XX______,
  ________, ____XX_X, _X_XX___, XX_X_X_X, _X______,
  ________, ____XXXX, XXXXX___, XXXXXXXX, XX______,
  ________, ___X_X_X, _X_X___X, XX_X_X_X, X_______,
  ________, ___XXXXX, XXXX___X, XXXXXXXX, X_______,
  ________, ___X_X_X, _X_X___X, _X_X_X_X, X_______,
  ________, __XXXXXX, XXXXXXXX, XXXXXXXX, ________,
  ________, __XXXXXX, XXXXXXXX, XXXXXXXX, ________,
  ________, __XXXXXX, XXXXXXXX, XXXXXXXX, ________,
  ________, _XXXXXXX, XXXXXXXX, XXXXXXX_, ________,
  ________, _X_X_X_X, _X___XX_, X_X_X_X_, ________,
  ________, _X______, _X___X__, ______X_, ________,
  ________, XX_X_X_X, X___X_X_, X_X_XX__, ________,
  ________, X_X_X_X_, X___XX_X, _X_X_X__, ________,
  ________, XXXXXXXX, X___XXXX, XXXXXX__, ________,
  _______X, XXXXXXXX, ___XXXXX, XXXXX___, ________,
  _______X, _XXXXXXX, ___X_XXX, XXXXX___, ________,
  ______X_, _XXXXXX_, __X_XXXX, XXXX____, ________,
  ______X_, X_X_X_X_, __X__X_X, _X_X____, ________,
  ______XX, _____XX_, __XX____, __XX____, ________,
  _______X, XXXXXX__, ___XXXXX, XXX_____, ________,

  ________, ______XX, XXXXXX__,
  ________, ______X_, ____XX__,
  ________, _____X__, _____X__,
  ________, _____X__, X_X__X__,
  ________, ____X__X, _X_X_X__,
  ________, ____X_X_, X_X__X__,
  ________, ___X_X_X, _X_X_X__,
  ________, ___XX_X_, X_X_XX__,
  ________, __XX_X_X, _X_X_X__,
  ________, __XXXXX_, XXXXXX__,
  ________, _X_X_XX_, _X_X_X__,
  ________, _XXXXX__, XXXXXX__,
  ________, XX_X_X_X, _X_X_X__,
  ________, XXXXXX_X, XXXXXX__,
  _______X, XXXXX__X, XXXXXX__,
  ______XX, XXXXXXXX, XXXXX___,
  _____XX_, X_X_X_X_, X_X_X___,
  _____X__, ________, ____X___,
  ____X_X_, X_X_X_X_, X_X_X___,
  ____XX_X, _X_X_X_X, _X_XX___,
  ___XXXXX, XXXXXXXX, XXXXX___,
  ___XXXXX, XX___XXX, XXXXX___,
  __X_XXXX, XX___XXX, XXXXX___,
  __X_XXXX, X____XXX, XXXXX___,
  _X__XXXX, X____X_X, _X_XX___,
  _X_X_X_X, _____X__, X_X_X___,
  XXX___XX, _____XX_, ___XX___,
  XXXXXXX_, _____XXX, XXXXX___,

  ________, ________, XXXXXXXX,
  ________, ________, X_____XX,
  ________, _______X, _______X,
  ________, _______X, __X_X__X,
  ________, ______X_, _X_X_X_X,
  ________, ______X_, X_X_X__X,
  ________, _____X_X, _X_X_X_X,
  ________, _____XX_, X_X_X_XX,
  ________, ____XX_X, _X_X_X_X,
  ________, ____XXXX, X_XXXXXX,
  ________, ___X_X_X, X__X_X_X,
  ________, ___XXXXX, __XXXXXX,
  ________, __XX_X_X, _X_X_X_X,
  ________, __XXXXXX, _XXXXXXX,
  ________, _XXXXXX_, _XXXXXXX,
  ________, XXXXXXXX, XXXXXXX_,
  _______X, X_X_X_X_, X_X_X_X_,
  _______X, ________, ______X_,
  ______X_, X_X_X_X_, X_X_X_X_,
  ______XX, _X_X_X_X, _X_X_XX_,
  _____XXX, XXXXXXXX, XXXXXXX_,
  _____XXX, XXXX___X, XXXXXXX_,
  ____X_XX, XXXX___X, XXXXXXX_,
  ____X_XX, XXX____X, XXXXXXX_,
  ___X__XX, XXX____X, _X_X_XX_,
  ___X_X_X, _X_____X, __X_X_X_,
  __XXX___, XX_____X, X____XX_,
  __XXXXXX, X______X, XXXXXXX_,

  ________, ________, __XXXXXX, XX______,
  ________, ________, __X_____, XX______,
  ________, ________, _X______, _X______,
  ________, ________, _X__X_X_, _X______,
  ________, ________, X__X_X_X, _X______,
  ________, ________, X_X_X_X_, _X______,
  ________, _______X, _X_X_X_X, _X______,
  ________, _______X, X_X_X_X_, XX______,
  ________, ______XX, _X_X_X_X, _X______,
  ________, ______XX, XXX_XXXX, XX______,
  ________, _____X_X, _XX__X_X, _X______,
  ________, _____XXX, XX__XXXX, XX______,
  ________, ____XX_X, _X_X_X_X, _X______,
  ________, ____XXXX, XX_XXXXX, XX______,
  ________, ___XXXXX, X__XXXXX, XX______,
  ________, __XXXXXX, XXXXXXXX, X_______,
  ________, _XX_X_X_, X_X_X_X_, X_______,
  ________, _X______, ________, X_______,
  ________, X_X_X_X_, X_X_X_X_, X_______,
  ________, XX_X_X_X, _X_X_X_X, X_______,
  _______X, XXXXXXXX, XXXXXXXX, X_______,
  _______X, XXXXXX__, _XXXXXXX, X_______,
  ______X_, XXXXXX__, _XXXXXXX, X_______,
  ______X_, XXXXX___, _XXXXXXX, X_______,
  _____X__, XXXXX___, _X_X_X_X, X_______,
  _____X_X, _X_X____, _X__X_X_, X_______,
  ____XXX_, __XX____, _XX____X, X_______,
  ____XXXX, XXX_____, _XXXXXXX, X_______,

  ________, ________, ____XXXX, XXXX____,
  ________, ________, ____X___, __XX____,
  ________, ________, ___X____, ___X____,
  ________, ________, ___X__X_, X__X____,
  ________, ________, __X__X_X, _X_X____,
  ________, ________, __X_X_X_, X__X____,
  ________, ________, _X_X_X_X, _X_X____,
  ________, ________, _XX_X_X_, X_XX____,
  ________, ________, XX_X_X_X, _X_X____,
  ________, ________, XXXXX_XX, XXXX____,
  ________, _______X, _X_XX__X, _X_X____,
  ________, _______X, XXXX__XX, XXXX____,
  ________, ______XX, _X_X_X_X, _X_X____,
  ________, ______XX, XXXX_XXX, XXXX____,
  ________, _____XXX, XXX__XXX, XXXX____,
  ________, ____XXXX, XXXXXXXX, XXX_____,
  ________, ___XX_X_, X_X_X_X_, X_X_____,
  ________, ___X____, ________, __X_____,
  ________, __X_X_X_, X_X_X_X_, X_X_____,
  ________, __XX_X_X, _X_X_X_X, _XX_____,
  ________, _XXXXXXX, XXXXXXXX, XXX_____,
  ________, _XXXXXXX, ___XXXXX, XXX_____,
  ________, X_XXXXXX, ___XXXXX, XXX_____,
  ________, X_XXXXX_, ___XXXXX, XXX_____,
  _______X, __XXXXX_, ___X_X_X, _XX_____,
  _______X, _X_X_X__, ___X__X_, X_X_____,
  ______XX, X___XX__, ___XX___, _XX_____,
  ______XX, XXXXX___, ___XXXXX, XXX_____,

  ________, ______XX, XXXXXX__, ________,
  ________, __XXXX__, ______XX, ________,
  ________, XX______, ________, XX______,
  _______X, _____X_X, _X_X_X__, __X_____,
  _______X, __X_X_X_, X_X_X_X_, X_X_____,
  ______X_, _X_X_X_X, _X_X_X_X, ___X____,
  ______X_, X_X_X_X_, X_X_X_X_, X__X____,
  _____XXX, _X_X_XXX, XX_X_X_X, _XX_____,
  _____XX_, X_X_XXX_, XXX_X_X_, X_X_____,
  _____XXX, XXXXXX__, _XXXXXXX, XXX_____,
  ____X_X_, X_X_X___, _XXXXXXX, XXX_____,
  ____XXXX, XXXXX___, ________, ________,
  ____X_X_, X_X_XXXX, XXXXXX__, ________,
  ____XXXX, XXXXXXXX, XXXXXXXX, ________,
  ____XXXX, XXXXXXXX, XXXXXXXX, X_______,
  _____XXX, XXXXXXXX, XXXXXXXX, X_______,
  ______XX, XXXXXXXX, XXXXXXXX, ________,
  ________, _____X_X, _X_X_X_X, ________,
  _XXXXXXX, XXX__X__, _______X, ________,
  _XX_X_X_, XX___X_X, _X_X_XX_, ________,
  _X_X_X_X, _XX_XXX_, X_X_X_X_, ________,
  _XXXXXXX, XXXXXXXX, XXXXXXX_, ________,
  XXXXXXXX, XXXXXXXX, XXXXXX__, ________,
  XXXXXXXX, XXXXXXXX, XXXXX___, ________,
  _X_XXXXX, XXXXXXXX, XXXXX___, ________,
  _X__X_X_, X_X_X_X_, X__X____, ________,
  __XX____, ________, _XX_____, ________,
  ____XXXX, XXXXXXXX, X_______, ________,

  ________, ________, XXXXXXXX, ________,
  ________, ____XXXX, ________, XX______,
  ________, __XX____, ________, __XX____,
  ________, _X_____X, _X_X_X_X, ____X___,
  ________, _X__X_X_, X_X_X_X_, X_X_X___,
  ________, X__X_X_X, _X_X_X_X, _X___X__,
  ________, X_X_X_X_, X_X_X_X_, X_X__X__,
  _______X, XX_X_X_X, XXXX_X_X, _X_XX___,
  _______X, X_X_X_XX, X_XXX_X_, X_X_X___,
  _______X, XXXXXXXX, ___XXXXX, XXXXX___,
  ______X_, X_X_X_X_, ___XXXXX, XXXXX___,
  ______XX, XXXXXXX_, ________, ________,
  ______X_, X_X_X_XX, XXXXXXXX, ________,
  ______XX, XXXXXXXX, XXXXXXXX, XX______,
  ______XX, XXXXXXXX, XXXXXXXX, XXX_____,
  _______X, XXXXXXXX, XXXXXXXX, XXX_____,
  ________, XXXXXXXX, XXXXXXXX, XX______,
  ________, _______X, _X_X_X_X, _X______,
  ___XXXXX, XXXXX__X, ________, _X______,
  ___XX_X_, X_XX___X, _X_X_X_X, X_______,
  ___X_X_X, _X_XX_XX, X_X_X_X_, X_______,
  ___XXXXX, XXXXXXXX, XXXXXXXX, X_______,
  __XXXXXX, XXXXXXXX, XXXXXXXX, ________,
  __XXXXXX, XXXXXXXX, XXXXXXX_, ________,
  ___X_XXX, XXXXXXXX, XXXXXXX_, ________,
  ___X__X_, X_X_X_X_, X_X__X__, ________,
  ____XX__, ________, ___XX___, ________,
  ______XX, XXXXXXXX, XXX_____, ________,

  ________, ________, __XXXXXX, XX______,
  ________, ______XX, XX______, __XX____,
  ________, ____XX__, ________, ____XX__,
  ________, ___X____, _X_X_X_X, _X____X_,
  ________, ___X__X_, X_X_X_X_, X_X_X_X_,
  ________, __X__X_X, _X_X_X_X, _X_X___X,
  ________, __X_X_X_, X_X_X_X_, X_X_X__X,
  ________, _XXX_X_X, _XXXXX_X, _X_X_XX_,
  ________, _XX_X_X_, XXX_XXX_, X_X_X_X_,
  ________, _XXXXXXX, XX___XXX, XXXXXXX_,
  ________, X_X_X_X_, X____XXX, XXXXXXX_,
  ________, XXXXXXXX, X_______, ________,
  ________, X_X_X_X_, XXXXXXXX, XX______,
  ________, XXXXXXXX, XXXXXXXX, XXXX____,
  ________, XXXXXXXX, XXXXXXXX, XXXXX___,
  ________, _XXXXXXX, XXXXXXXX, XXXXX___,
  ________, __XXXXXX, XXXXXXXX, XXXX____,
  ________, ________, _X_X_X_X, _X_X____,
  _____XXX, XXXXXXX_, _X______, ___X____,
  _____XX_, X_X_XX__, _X_X_X_X, _XX_____,
  _____X_X, _X_X_XX_, XXX_X_X_, X_X_____,
  _____XXX, XXXXXXXX, XXXXXXXX, XXX_____,
  ____XXXX, XXXXXXXX, XXXXXXXX, XX______,
  ____XXXX, XXXXXXXX, XXXXXXXX, X_______,
  _____X_X, XXXXXXXX, XXXXXXXX, X_______,
  _____X__, X_X_X_X_, X_X_X__X, ________,
  ______XX, ________, _____XX_, ________,
  ________, XXXXXXXX, XXXXX___, ________,

  ________, ________, ____XXXX, XXXX____, ________,
  ________, ________, XXXX____, ____XX__, ________,
  ________, ______XX, ________, ______XX, ________,
  ________, _____X__, ___X_X_X, _X_X____, X_______,
  ________, _____X__, X_X_X_X_, X_X_X_X_, X_______,
  ________, ____X__X, _X_X_X_X, _X_X_X__, _X______,
  ________, ____X_X_, X_X_X_X_, X_X_X_X_, _X______,
  ________, ___XXX_X, _X_XXXXX, _X_X_X_X, X_______,
  ________, ___XX_X_, X_XXX_XX, X_X_X_X_, X_______,
  ________, ___XXXXX, XXXX___X, XXXXXXXX, X_______,
  ________, __X_X_X_, X_X____X, XXXXXXXX, X_______,
  ________, __XXXXXX, XXX_____, ________, ________,
  ________, __X_X_X_, X_XXXXXX, XXXX____, ________,
  ________, __XXXXXX, XXXXXXXX, XXXXXX__, ________,
  ________, __XXXXXX, XXXXXXXX, XXXXXXX_, ________,
  ________, ___XXXXX, XXXXXXXX, XXXXXXX_, ________,
  ________, ____XXXX, XXXXXXXX, XXXXXX__, ________,
  ________, ________, ___X_X_X, _X_X_X__, ________,
  _______X, XXXXXXXX, X__X____, _____X__, ________,
  _______X, X_X_X_XX, ___X_X_X, _X_XX___, ________,
  _______X, _X_X_X_X, X_XXX_X_, X_X_X___, ________,
  _______X, XXXXXXXX, XXXXXXXX, XXXXX___, ________,
  ______XX, XXXXXXXX, XXXXXXXX, XXXX____, ________,
  ______XX, XXXXXXXX, XXXXXXXX, XXX_____, ________,
  _______X, _XXXXXXX, XXXXXXXX, XXX_____, ________,
  _______X, __X_X_X_, X_X_X_X_, _X______, ________,
  ________, XX______, _______X, X_______, ________,
  ________, __XXXXXX, XXXXXXX_, ________, ________,

  ________, _XXXXXXX, XXXXXXXX, XX______,
  ________, XX______, _______X, X_______,
  ________, X_______, ________, X_______,
  ________, X___X_X_, X_X_X___, X_______,
  _______X, ___X_X_X, _X_X_X_X, ________,
  _______X, __X_X_XX, XXXXXXXX, ________,
  _______X, _X_X_X_X, XXXXXXXX, ________,
  ______X_, X_X_X_X_, ________, ________,
  ______XX, _X_X_XX_, ________, ________,
  ______XX, XXXXXXX_, ________, ________,
  _____XXX, _X_X_X__, ________, ________,
  _____XXX, XXXXXX__, ________, ________,
  _____X_X, _X_X_XXX, XXXXXX__, ________,
  ____XXXX, XXXXXXXX, XXXXX___, ________,
  ____XXXX, XXXXXXXX, XXXXX___, ________,
  ____XXXX, XXXXXXXX, XXXXX___, ________,
  ___XXXXX, XXXXXXXX, XXXX____, ________,
  ___X_X_X, _X_X____, ________, ________,
  ___X____, ___X____, ________, ________,
  __XX_X_X, _XX_____, ________, ________,
  __X_X_X_, X_X_____, ________, ________,
  __XXXXXX, XXX_____, ________, ________,
  _XXXXXXX, XXXXXXXX, XX______, ________,
  _XXXXXXX, XXXXXXXX, XX______, ________,
  X_XXXXXX, XXXXXXXX, X_______, ________,
  X__X_X_X, _X_X_X__, X_______, ________,
  XX______, _______X, X_______, ________,
  _XXXXXXX, XXXXXXXX, ________, ________,

  ________, ___XXXXX, XXXXXXXX, XXXX____,
  ________, __XX____, ________, _XX_____,
  ________, __X_____, ________, __X_____,
  ________, __X___X_, X_X_X_X_, __X_____,
  ________, _X___X_X, _X_X_X_X, _X______,
  ________, _X__X_X_, XXXXXXXX, XX______,
  ________, _X_X_X_X, _XXXXXXX, XX______,
  ________, X_X_X_X_, X_______, ________,
  ________, XX_X_X_X, X_______, ________,
  ________, XXXXXXXX, X_______, ________,
  _______X, XX_X_X_X, ________, ________,
  _______X, XXXXXXXX, ________, ________,
  _______X, _X_X_X_X, XXXXXXXX, ________,
  ______XX, XXXXXXXX, XXXXXXX_, ________,
  ______XX, XXXXXXXX, XXXXXXX_, ________,
  ______XX, XXXXXXXX, XXXXXXX_, ________,
  _____XXX, XXXXXXXX, XXXXXX__, ________,
  _____X_X, _X_X_X__, ________, ________,
  _____X__, _____X__, ________, ________,
  ____XX_X, _X_XX___, ________, ________,
  ____X_X_, X_X_X___, ________, ________,
  ____XXXX, XXXXX___, ________, ________,
  ___XXXXX, XXXXXXXX, XXXX____, ________,
  ___XXXXX, XXXXXXXX, XXXX____, ________,
  __X_XXXX, XXXXXXXX, XXX_____, ________,
  __X__X_X, _X_X_X_X, __X_____, ________,
  __XX____, ________, _XX_____, ________,
  ___XXXXX, XXXXXXXX, XX______, ________,

  ________, _____XXX, XXXXXXXX, XXXXXX__,
  ________, ____XX__, ________, ___XX___,
  ________, ____X___, ________, ____X___,
  ________, ____X___, X_X_X_X_, X___X___,
  ________, ___X___X, _X_X_X_X, _X_X____,
  ________, ___X__X_, X_XXXXXX, XXXX____,
  ________, ___X_X_X, _X_XXXXX, XXXX____,
  ________, __X_X_X_, X_X_____, ________,
  ________, __XX_X_X, _XX_____, ________,
  ________, __XXXXXX, XXX_____, ________,
  ________, _XXX_X_X, _X______, ________,
  ________, _XXXXXXX, XX______, ________,
  ________, _X_X_X_X, _XXXXXXX, XX______,
  ________, XXXXXXXX, XXXXXXXX, X_______,
  ________, XXXXXXXX, XXXXXXXX, X_______,
  ________, XXXXXXXX, XXXXXXXX, X_______,
  _______X, XXXXXXXX, XXXXXXXX, ________,
  _______X, _X_X_X_X, ________, ________,
  _______X, _______X, ________, ________,
  ______XX, _X_X_XX_, ________, ________,
  ______X_, X_X_X_X_, ________, ________,
  ______XX, XXXXXXX_, ________, ________,
  _____XXX, XXXXXXXX, XXXXXX__, ________,
  _____XXX, XXXXXXXX, XXXXXX__, ________,
  ____X_XX, XXXXXXXX, XXXXX___, ________,
  ____X__X, _X_X_X_X, _X__X___, ________,
  ____XX__, ________, ___XX___, ________,
  _____XXX, XXXXXXXX, XXXX____, ________,

  ________, _______X, XXXXXXXX, XXXXXXXX,
  ________, ______XX, ________, _____XX_,
  ________, ______X_, ________, ______X_,
  ________, ______X_, __X_X_X_, X_X___X_,
  ________, _____X__, _X_X_X_X, _X_X_X__,
  ________, _____X__, X_X_XXXX, XXXXXX__,
  ________, _____X_X, _X_X_XXX, XXXXXX__,
  ________, ____X_X_, X_X_X___, ________,
  ________, ____XX_X, _X_XX___, ________,
  ________, ____XXXX, XXXXX___, ________,
  ________, ___XXX_X, _X_X____, ________,
  ________, ___XXXXX, XXXX____, ________,
  ________, ___X_X_X, _X_XXXXX, XXXX____,
  ________, __XXXXXX, XXXXXXXX, XXX_____,
  ________, __XXXXXX, XXXXXXXX, XXX_____,
  ________, __XXXXXX, XXXXXXXX, XXX_____,
  ________, _XXXXXXX, XXXXXXXX, XX______,
  ________, _X_X_X_X, _X______, ________,
  ________, _X______, _X______, ________,
  ________, XX_X_X_X, X_______, ________,
  ________, X_X_X_X_, X_______, ________,
  ________, XXXXXXXX, X_______, ________,
  _______X, XXXXXXXX, XXXXXXXX, ________,
  _______X, XXXXXXXX, XXXXXXXX, ________,
  ______X_, XXXXXXXX, XXXXXXX_, ________,
  ______X_, _X_X_X_X, _X_X__X_, ________,
  ______XX, ________, _____XX_, ________,
  _______X, XXXXXXXX, XXXXXX__, ________,

  ________, ______XX, XXXXXX__, ________,
  ________, __XXXX__, ______XX, ________,
  ________, XX______, ________, XX______,
  _______X, _____X_X, _X_X_X__, __X_____,
  _______X, __X_X_X_, X_X_X_X_, X_X_____,
  ______X_, _X_X_X_X, _X_X_X_X, ___X____,
  ______X_, X_X_X_X_, X_X_X_X_, X__X____,
  _____XXX, _X_X_XXX, XX_X_X_X, _XX_____,
  _____XX_, X_X_XXX_, XXX_X_X_, X_X_____,
  _____XXX, XXXXXX__, _XXXXXXX, XXX_____,
  ____X_X_, X_X_X___, X_X_X_X_, XX______,
  ____XXXX, XXXXX___, XXXXXXXX, XX______,
  ____X_X_, X_X_X___, X_X_X_X_, XX______,
  ___XXXXX, XXXX___X, XXXXXXXX, X_______,
  ___XXXXX, XXXX___X, XXXXXXXX, X_______,
  ___XXXXX, XXXX___X, XXXXXXXX, X_______,
  __XXXXXX, XXX___XX, XXXXXXXX, ________,
  __X_X_X_, X_X___XX, _X_X_X_X, ________,
  __X_____, __X___X_, _______X, ________,
  _XX_X_X_, XX___X_X, _X_X_XX_, ________,
  _X_X_X_X, _XX_XXX_, X_X_X_X_, ________,
  _XXXXXXX, XXXXXXXX, XXXXXXX_, ________,
  XXXXXXXX, XXXXXXXX, XXXXXX__, ________,
  XXXXXXXX, XXXXXXXX, XXXXX___, ________,
  _X_XXXXX, XXXXXXXX, XXXXX___, ________,
  _X__X_X_, X_X_XXXX, XXXX____, ________,
  __XX____, _____XXX, XXX_____, ________,
  ____XXXX, XXXX__XX, X_______, ________,
  ________, ____X__X, X_______, ________,
  ________, ____X_XX, XX______, ________,
  ________, _____X_X, _XX_____, ________,
  ________, _____XXX, XXXX____, ________,

  ________, ________, XXXXXXXX, ________,
  ________, ____XXXX, ________, XX______,
  ________, __XX____, ________, __XX____,
  ________, _X_____X, _X_X_X_X, ____X___,
  ________, _X__X_X_, X_X_X_X_, X_X_X___,
  ________, X__X_X_X, _X_X_X_X, _X___X__,
  ________, X_X_X_X_, X_X_X_X_, X_X__X__,
  _______X, XX_X_X_X, XXXX_X_X, _X_XX___,
  _______X, X_X_X_XX, X_XXX_X_, X_X_X___,
  _______X, XXXXXXXX, ___XXXXX, XXXXX___,
  ______X_, X_X_X_X_, __X_X_X_, X_XX____,
  ______XX, XXXXXXX_, __XXXXXX, XXXX____,
  ______X_, X_X_X_X_, __X_X_X_, X_XX____,
  _____XXX, XXXXXX__, _XXXXXXX, XXX_____,
  _____XXX, XXXXXX__, _XXXXXXX, XXX_____,
  _____XXX, XXXXXX__, _XXXXXXX, XXX_____,
  ____XXXX, XXXXX___, XXXXXXXX, XX______,
  ____X_X_, X_X_X___, XX_X_X_X, _X______,
  ____X___, ____X___, X_______, _X______,
  ___XX_X_, X_XX___X, _X_X_X_X, X_______,
  ___X_X_X, _X_XX_XX, X_X_X_X_, X_______,
  ___XXXXX, XXXXXXXX, XXXXXXXX, X_______,
  __XXXXXX, XXXXXXXX, XXXXXXXX, ________,
  __XXXXXX, XXXXXXXX, XXXXXXX_, ________,
  ___X_XXX, XXXXXXXX, XXXXXXX_, ________,
  ___X__X_, X_X_X_XX, XXXXXX__, ________,
  ____XX__, _______X, XXXXX___, ________,
  ______XX, XXXXXX__, XXX_____, ________,
  ________, ______X_, _XX_____, ________,
  ________, ______X_, XXXX____, ________,
  ________, _______X, _X_XX___, ________,
  ________, _______X, XXXXXX__, ________,

  ________, ________, __XXXXXX, XX______,
  ________, ______XX, XX______, __XX____,
  ________, ____XX__, ________, ____XX__,
  ________, ___X____, _X_X_X_X, _X____X_,
  ________, ___X__X_, X_X_X_X_, X_X_X_X_,
  ________, __X__X_X, _X_X_X_X, _X_X___X,
  ________, __X_X_X_, X_X_X_X_, X_X_X__X,
  ________, _XXX_X_X, _XXXXX_X, _X_X_XX_,
  ________, _XX_X_X_, XXX_XXX_, X_X_X_X_,
  ________, _XXXXXXX, XX___XXX, XXXXXXX_,
  ________, X_X_X_X_, X___X_X_, X_X_XX__,
  ________, XXXXXXXX, X___XXXX, XXXXXX__,
  ________, X_X_X_X_, X___X_X_, X_X_XX__,
  _______X, XXXXXXXX, ___XXXXX, XXXXX___,
  _______X, XXXXXXXX, ___XXXXX, XXXXX___,
  _______X, XXXXXXXX, ___XXXXX, XXXXX___,
  ______XX, XXXXXXX_, __XXXXXX, XXXX____,
  ______X_, X_X_X_X_, __XX_X_X, _X_X____,
  ______X_, ______X_, __X_____, ___X____,
  _____XX_, X_X_XX__, _X_X_X_X, _XX_____,
  _____X_X, _X_X_XX_, XXX_X_X_, X_X_____,
  _____XXX, XXXXXXXX, XXXXXXXX, XXX_____,
  ____XXXX, XXXXXXXX, XXXXXXXX, XX______,
  ____XXXX, XXXXXXXX, XXXXXXXX, X_______,
  _____X_X, XXXXXXXX, XXXXXXXX, X_______,
  _____X__, X_X_X_X_, XXXXXXXX, ________,
  ______XX, ________, _XXXXXX_, ________,
  ________, XXXXXXXX, __XXX___, ________,
  ________, ________, X__XX___, ________,
  ________, ________, X_XXXX__, ________,
  ________, ________, _X_X_XX_, ________,
  ________, ________, _XXXXXXX, ________,

  ________, ________, ____XXXX, XXXX____, ________,
  ________, ________, XXXX____, ____XX__, ________,
  ________, ______XX, ________, ______XX, ________,
  ________, _____X__, ___X_X_X, _X_X____, X_______,
  ________, _____X__, X_X_X_X_, X_X_X_X_, X_______,
  ________, ____X__X, _X_X_X_X, _X_X_X__, _X______,
  ________, ____X_X_, X_X_X_X_, X_X_X_X_, _X______,
  ________, ___XXX_X, _X_XXXXX, _X_X_X_X, X_______,
  ________, ___XX_X_, X_XXX_XX, X_X_X_X_, X_______,
  ________, ___XXXXX, XXXX___X, XXXXXXXX, X_______,
  ________, __X_X_X_, X_X___X_, X_X_X_XX, ________,
  ________, __XXXXXX, XXX___XX, XXXXXXXX, ________,
  ________, __X_X_X_, X_X___X_, X_X_X_XX, ________,
  ________, _XXXXXXX, XX___XXX, XXXXXXX_, ________,
  ________, _XXXXXXX, XX___XXX, XXXXXXX_, ________,
  ________, _XXXXXXX, XX___XXX, XXXXXXX_, ________,
  ________, XXXXXXXX, X___XXXX, XXXXXX__, ________,
  ________, X_X_X_X_, X___XX_X, _X_X_X__, ________,
  ________, X_______, X___X___, _____X__, ________,
  _______X, X_X_X_XX, ___X_X_X, _X_XX___, ________,
  _______X, _X_X_X_X, X_XXX_X_, X_X_X___, ________,
  _______X, XXXXXXXX, XXXXXXXX, XXXXX___, ________,
  ______XX, XXXXXXXX, XXXXXXXX, XXXX____, ________,
  ______XX, XXXXXXXX, XXXXXXXX, XXX_____, ________,
  _______X, _XXXXXXX, XXXXXXXX, XXX_____, ________,
  _______X, __X_X_X_, X_XXXXXX, XX______, ________,
  ________, XX______, ___XXXXX, X_______, ________,
  ________, __XXXXXX, XX__XXX_, ________, ________,
  ________, ________, __X__XX_, ________, ________,
  ________, ________, __X_XXXX, ________, ________,
  ________, ________, ___X_X_X, X_______, ________,
  ________, ________, ___XXXXX, XX______, ________,

  __XXXXXX, X_______,
  _X_X_X__, XX______,
  _XX_XXX_, _X______,
  XXXXXXXX, _X______,
  XXXXXXXX, XX______,
  X_X_X_X_, X_______,
  XX_____X, X_______,
  _XXXXXXX, ________,

  ____XXXX, XXX_____,
  ___X_X_X, __XX____,
  ___XX_XX, X__X____,
  __XXXXXX, XX_X____,
  __XXXXXX, XXXX____,
  __X_X_X_, X_X_____,
  __XX____, _XX_____,
  ___XXXXX, XX______,

  ______XX, XXXXX___,
  _____X_X, _X__XX__,
  _____XX_, XXX__X__,
  ____XXXX, XXXX_X__,
  ____XXXX, XXXXXX__,
  ____X_X_, X_X_X___,
  ____XX__, ___XX___,
  _____XXX, XXXX____,

  ________, XXXXXXX_,
  _______X, _X_X__XX,
  _______X, X_XXX__X,
  ______XX, XXXXXX_X,
  ______XX, XXXXXXXX,
  ______X_, X_X_X_X_,
  ______XX, _____XX_,
  _______X, XXXXXX__,

  ________, ________, _XXXXXXX,
  ________, ________, _X_____X,
  ________, ________, X______X,
  ________, _______X, __X_X__X,
  ________, _______X, _X_X_XX_,
  ________, ______X_, X_X_X_X_,
  ________, _____X_X, _X_X_XX_,
  ________, ____X_X_, XXX_X_X_,
  ________, ____XXXX, X_XXXXX_,
  ________, ___XX_X_, X_X_X_X_,
  ________, __XXXXXX, _XXXXXX_,
  ________, __XX_X_X, _X_X_XX_,
  ________, _XXXXXX_, _XXXXX__,
  ________, XXXXXXXX, XXXXXX__,
  ________, XXXXXXXX, XXXXXX__,
  _______X, _X_X_X_X, _X_X_X__,
  ______X_, ________, _____X__,
  ______X_, X_X_X_X_, X_X_XX__,
  _____XXX, _X_X_X_X, _X_X_X__,
  ____XXXX, XXXXXXXX, XXXXXX__,
  ___XXXXX, XXX___XX, XXXXXX__,
  ___X_XXX, XX____XX, XXXXX___,
  __X_XXXX, X_____XX, XXXXX___,
  _X_X_X__, X____X_X, _X__X___,
  _X_____X, _____X__, ___XX___,
  XXXXXXX_, _____XXX, XXXXX___,

  ________, ________, _______X, XXXXXX__,
  ________, ________, ______X_, ____XX__,
  ________, ________, ____XX__, ____X___,
  ________, ________, ___XX_X_, X_X_X___,
  ________, ________, __XX_X_X, _X_X____,
  ________, ________, _XX_X_X_, X_XX____,
  ________, ________, XX_X_X_X, _XX_____,
  ________, ______XX, XXXXX_XX, XXX_____,
  ________, _____XXX, _X_X_XXX, _X______,
  ________, ____XXXX, XXX_XXXX, XX______,
  ________, ___XXXXX, XXXXXXXX, X_______,
  ________, __X_X_X_, X_X_X_X_, X_______,
  ________, XX_X_X_X, _X_X_X_X, ________,
  _______X, XXXXXXXX, XXXXXXXX, ________,
  ______XX, XXXXXX__, __XXXXX_, ________,
  _____XXX, XXXX____, _XXXXXX_, ________,
  ____X_XX, XXX_____, XXXXXX__, ________,
  __XX_X__, XX_____X, XX_X_X__, ________,
  _X_____X, _______X, ____X___, ________,
  XXXXXXX_, ______XX, XXXXX___, ________,

  ________, ________, ________, _XXXXXX_,
  ________, ________, _______X, X____XX_,
  ________, ________, ______X_, _____X__,
  ________, ________, ____XX_X, _X_X_X__,
  ________, ________, ___XX_X_, X_X_X___,
  ________, ________, __XX_XXX, _X_X____,
  ________, ________, XXXXX_XX, XXXX____,
  ________, _______X, _X_X_XX_, X_X_____,
  ________, _____XXX, XXXX_XXX, XX______,
  ________, ____XXXX, XXXXXXXX, XX______,
  ________, __XX_X_X, _X_X_X__, X_______,
  ________, _XX_X_X_, X_X_X_XX, X_______,
  _______X, XXXXXXXX, XXXXXXXX, ________,
  ______XX, XXXXXX__, _XXXXXX_, ________,
  _____X_X, XXXX____, XXXXXXX_, ________,
  ___XX___, XX_____X, X_X_XX__, ________,
  __X____X, _______X, _____X__, ________,
  XXXXXXX_, ______XX, XXXXX___, ________,

  ________, _XXXXXXX, X_______,
  _______X, X_______, _XX_____,
  ______X_, __X_X_X_, X__X____,
  _____X_X, _X_X_X_X, _X_X____,
  _____X__, X_X_X_X_, X_X_X___,
  ____X__X, _X_XXX_X, _X_XX___,
  ____XXXX, XXX__XXX, XXXXX___,
  ___X_X_X, _X___X_X, _X_X____,
  ___XXXXX, XX___XXX, XXXX____,
  ___X_X_X, X_______, ________,
  __XXXXXX, X_______, ________,
  __XXXXXX, X_______, ________,
  _XXXXXXX, ___XXXXX, XX______,
  _X_____X, ___X____, _X______,
  _X_X_X_X, __X_X_X_, XX______,
  X_X_X_X_, XX_X_X_X, X_______,
  XXXXXXXX, XXXXXXXX, X_______,
  XX_XXXXX, XXXXX_XX, ________,
  _XX_____, _____XX_, ________,
  ___XXXXX, XXXXX___, ________,

  ________, ___XXXXX, XXX_____,
  ________, _XX_____, ___XX___,
  ________, X___X_X_, X_X__X__,
  _______X, _X_X_X_X, _X_X_X__,
  _______X, __X_X_X_, X_X_X_X_,
  ______X_, _X_X_XXX, _X_X_XX_,
  ______XX, XXXXX__X, XXXXXXX_,
  _____X_X, _X_X___X, _X_X_X__,
  _____XXX, XXXX___X, XXXXXX__,
  _____X_X, _XX_____, ________,
  ____XXXX, XXX_____, ________,
  ____XXXX, XXX_____, ________,
  ___XXXXX, XX___XXX, XXXX____,
  ___X____, _X___X__, ___X____,
  ___X_X_X, _X__X_X_, X_XX____,
  __X_X_X_, X_XX_X_X, _XX_____,
  __XXXXXX, XXXXXXXX, XXX_____,
  __XX_XXX, XXXXXXX_, XX______,
  ___XX___, _______X, X_______,
  _____XXX, XXXXXXX_, ________,

  ________, _____XXX, XXXXX___, ________,
  ________, ___XX___, _____XX_, ________,
  ________, __X___X_, X_X_X__X, ________,
  ________, _X_X_X_X, _X_X_X_X, ________,
  ________, _X__X_X_, X_X_X_X_, X_______,
  ________, X__X_X_X, XX_X_X_X, X_______,
  ________, XXXXXXX_, _XXXXXXX, X_______,
  _______X, _X_X_X__, _X_X_X_X, ________,
  _______X, XXXXXX__, _XXXXXXX, ________,
  _______X, _X_XX___, ________, ________,
  ______XX, XXXXX___, ________, ________,
  ______XX, XXXXX___, ________, ________,
  _____XXX, XXXX___X, XXXXXX__, ________,
  _____X__, ___X___X, _____X__, ________,
  _____X_X, _X_X__X_, X_X_XX__, ________,
  ____X_X_, X_X_XX_X, _X_XX___, ________,
  ____XXXX, XXXXXXXX, XXXXX___, ________,
  ____XX_X, XXXXXXXX, X_XX____, ________,
  _____XX_, ________, _XX_____, ________,
  _______X, XXXXXXXX, X_______, ________,

  ________, _______X, XXXXXXX_, ________,
  ________, _____XX_, _______X, X_______,
  ________, ____X___, X_X_X_X_, _X______,
  ________, ___X_X_X, _X_X_X_X, _X______,
  ________, ___X__X_, X_X_X_X_, X_X_____,
  ________, __X__X_X, _XXX_X_X, _XX_____,
  ________, __XXXXXX, X__XXXXX, XXX_____,
  ________, _X_X_X_X, ___X_X_X, _X______,
  ________, _XXXXXXX, ___XXXXX, XX______,
  ________, _X_X_XX_, ________, ________,
  ________, XXXXXXX_, ________, ________,
  ________, XXXXXXX_, ________, ________,
  _______X, XXXXXX__, _XXXXXXX, ________,
  _______X, _____X__, _X_____X, ________,
  _______X, _X_X_X__, X_X_X_XX, ________,
  ______X_, X_X_X_XX, _X_X_XX_, ________,
  ______XX, XXXXXXXX, XXXXXXX_, ________,
  ______XX, _XXXXXXX, XXX_XX__, ________,
  _______X, X_______, ___XX___, ________,
  ________, _XXXXXXX, XXX_____, ________,

  _____XXX, XXX_____,
  ____X___, __XX____,
  ___X_X_X, _X_X____,
  __X_X_XX, X_XX____,
  __XXXX__, XXXX____,
  _X_XX___, XXXX____,
  _XXXX___, ________,
  _XXX____, ________,
  XXXX___X, XXX_____,
  X__X__X_, __X_____,
  X_X_XX_X, _X______,
  XXXXXXXX, XX______,
  XX_X_X_X, X_______,
  _XXXXXXX, ________,

  _______X, XXXXX___,
  ______X_, ____XX__,
  _____X_X, _X_X_X__,
  ____X_X_, XXX_XX__,
  ____XXXX, __XXXX__,
  ___X_XX_, __XXXX__,
  ___XXXX_, ________,
  ___XXX__, ________,
  __XXXX__, _XXXX___,
  __X__X__, X___X___,
  __X_X_XX, _X_X____,
  __XXXXXX, XXXX____,
  __XX_X_X, _XX_____,
  ___XXXXX, XX______,

  ________, _XXXXXX_,
  ________, X_____XX,
  _______X, _X_X_X_X,
  ______X_, X_XXX_XX,
  ______XX, XX__XXXX,
  _____X_X, X___XXXX,
  _____XXX, X_______,
  _____XXX, ________,
  ____XXXX, ___XXXX_,
  ____X__X, __X___X_,
  ____X_X_, XX_X_X__,
  ____XXXX, XXXXXX__,
  ____XX_X, _X_XX___,
  _____XXX, XXXX____,

  ________, ___XXXXX, X_______,
  ________, __X_____, XX______,
  ________, _X_X_X_X, _X______,
  ________, X_X_XXX_, XX______,
  ________, XXXX__XX, XX______,
  _______X, _XX___XX, XX______,
  _______X, XXX_____, ________,
  _______X, XX______, ________,
  ______XX, XX___XXX, X_______,
  ______X_, _X__X___, X_______,
  ______X_, X_XX_X_X, ________,
  ______XX, XXXXXXXX, ________,
  ______XX, _X_X_XX_, ________,
  _______X, XXXXXX__, ________,

  __XXXX__,
  _X__X_X_,
  _X_X_XX_,
  XXX_____,
  X_X_XX__,
  XXXXXX__,
  X_X_XX__,
  _XXXX___,

  ____XXXX, ________,
  ___X__X_, X_______,
  ___X_X_X, X_______,
  __XXX___, ________,
  __X_X_XX, ________,
  __XXXXXX, ________,
  __X_X_XX, ________,
  ___XXXX_, ________,

  ______XX, XX______,
  _____X__, X_X_____,
  _____X_X, _XX_____,
  ____XXX_, ________,
  ____X_X_, XX______,
  ____XXXX, XX______,
  ____X_X_, XX______,
  _____XXX, X_______,

  ________, XXXX____,
  _______X, __X_X___,
  _______X, _X_XX___,
  ______XX, X_______,
  ______X_, X_XX____,
  ______XX, XXXX____,
  ______X_, X_XX____,
  _______X, XXX_____,

  ______XX, XXXX____, XXXXX___,
  ______X_, ___X___X, _____X__,
  _____X_X, _X_X___X, _X_X_X__,
  _____XX_, X_X___X_, X_X_X___,
  _____X_X, _XX___XX, _X_XX___,
  ____XXXX, XXX___XX, XXXXX___,
  ____XX_X, _X___X_X, _X_X____,
  ____XXXX, XXX_XXXX, XXXX____,
  ___X_X_X, _X_X_X_X, _X_X____,
  ___XXXXX, XXXXXXXX, XXX_____,
  __XXXXXX, XXXXXXXX, XXX_____,
  __XXXXXX, XXXXXXXX, XX______,
  __X_____, X_X_____, _X______,
  _X_X_X_X, ___X_X_X, _X______,
  _XX_X_XX, __X_X_X_, X_______,
  XXXXXXX_, __XXXXXX, X_______,
  XXXXXXX_, _XXXXXXX, X_______,
  XXXX_X__, _XXXXX_X, ________,
  X____X__, _X____XX, ________,
  _XXXX___, __XXXXX_, ________,

  ________, XXXXXX__, __XXXXX_,
  ________, X____X__, _X_____X,
  _______X, _X_X_X__, _X_X_X_X,
  _______X, X_X_X___, X_X_X_X_,
  _______X, _X_XX___, XX_X_XX_,
  ______XX, XXXXX___, XXXXXXX_,
  ______XX, _X_X___X, _X_X_X__,
  ______XX, XXXXX_XX, XXXXXX__,
  _____X_X, _X_X_X_X, _X_X_X__,
  _____XXX, XXXXXXXX, XXXXX___,
  ____XXXX, XXXXXXXX, XXXXX___,
  ____XXXX, XXXXXXXX, XXXX____,
  ____X___, __X_X___, ___X____,
  ___X_X_X, _X___X_X, _X_X____,
  ___XX_X_, XX__X_X_, X_X_____,
  __XXXXXX, X___XXXX, XXX_____,
  __XXXXXX, X__XXXXX, XXX_____,
  __XXXX_X, ___XXXXX, _X______,
  __X____X, ___X____, XX______,
  ___XXXX_, ____XXXX, X_______,

  ________, __XXXXXX, ____XXXX, X_______,
  ________, __X____X, ___X____, _X______,
  ________, _X_X_X_X, ___X_X_X, _X______,
  ________, _XX_X_X_, __X_X_X_, X_______,
  ________, _X_X_XX_, __XX_X_X, X_______,
  ________, XXXXXXX_, __XXXXXX, X_______,
  ________, XX_X_X__, _X_X_X_X, ________,
  ________, XXXXXXX_, XXXXXXXX, ________,
  _______X, _X_X_X_X, _X_X_X_X, ________,
  _______X, XXXXXXXX, XXXXXXX_, ________,
  ______XX, XXXXXXXX, XXXXXXX_, ________,
  ______XX, XXXXXXXX, XXXXXX__, ________,
  ______X_, ____X_X_, _____X__, ________,
  _____X_X, _X_X___X, _X_X_X__, ________,
  _____XX_, X_XX__X_, X_X_X___, ________,
  ____XXXX, XXX___XX, XXXXX___, ________,
  ____XXXX, XXX__XXX, XXXXX___, ________,
  ____XXXX, _X___XXX, XX_X____, ________,
  ____X___, _X___X__, __XX____, ________,
  _____XXX, X_____XX, XXX_____, ________,

  ________, ____XXXX, XX____XX, XXX_____,
  ________, ____X___, _X___X__, ___X____,
  ________, ___X_X_X, _X___X_X, _X_X____,
  ________, ___XX_X_, X___X_X_, X_X_____,
  ________, ___X_X_X, X___XX_X, _XX_____,
  ________, __XXXXXX, X___XXXX, XXX_____,
  ________, __XX_X_X, ___X_X_X, _X______,
  ________, __XXXXXX, X_XXXXXX, XX______,
  ________, _X_X_X_X, _X_X_X_X, _X______,
  ________, _XXXXXXX, XXXXXXXX, X_______,
  ________, XXXXXXXX, XXXXXXXX, X_______,
  ________, XXXXXXXX, XXXXXXXX, ________,
  ________, X_____X_, X______X, ________,
  _______X, _X_X_X__, _X_X_X_X, ________,
  _______X, X_X_XX__, X_X_X_X_, ________,
  ______XX, XXXXX___, XXXXXXX_, ________,
  ______XX, XXXXX__X, XXXXXXX_, ________,
  ______XX, XX_X___X, XXXX_X__, ________,
  ______X_, ___X___X, ____XX__, ________,
  _______X, XXX_____, XXXXX___, ________,

  ____XXX_, __XXX___,
  ____X_XX, _XXX_X__,
  ___X_XX_, _XX_XX__,
  ___XX_X_, _X_XX___,
  ___XXXX_, X_X_X___,
  __XX_X__, XX_XX___,
  __XXXXXX, XXXX____,
  __XXXXXX, XXXX____,
  _X___X_X, ___X____,
  _X__X__X, X_X_____,
  _XXXX__X, XXX_____,
  XXXXX_XX, XXX_____,
  XXXX__XX, XX______,
  _XX____X, X_______,

  ______XX, X___XXX_,
  ______X_, XX_XXX_X,
  _____X_X, X__XX_XX,
  _____XX_, X__X_XX_,
  _____XXX, X_X_X_X_,
  ____XX_X, __XX_XX_,
  ____XXXX, XXXXXX__,
  ____XXXX, XXXXXX__,
  ___X___X, _X___X__,
  ___X__X_, _XX_X___,
  ___XXXX_, _XXXX___,
  __XXXXX_, XXXXX___,
  __XXXX__, XXXX____,
  ___XX___, _XX_____,

  ________, XXX___XX, X_______,
  ________, X_XX_XXX, _X______,
  _______X, _XX__XX_, XX______,
  _______X, X_X__X_X, X_______,
  _______X, XXX_X_X_, X_______,
  ______XX, _X__XX_X, X_______,
  ______XX, XXXXXXXX, ________,
  ______XX, XXXXXXXX, ________,
  _____X__, _X_X___X, ________,
  _____X__, X__XX_X_, ________,
  _____XXX, X__XXXX_, ________,
  ____XXXX, X_XXXXX_, ________,
  ____XXXX, __XXXX__, ________,
  _____XX_, ___XX___, ________,

  ________, __XXX___, XXX_____,
  ________, __X_XX_X, XX_X____,
  ________, _X_XX__X, X_XX____,
  ________, _XX_X__X, _XX_____,
  ________, _XXXX_X_, X_X_____,
  ________, XX_X__XX, _XX_____,
  ________, XXXXXXXX, XX______,
  ________, XXXXXXXX, XX______,
  _______X, ___X_X__, _X______,
  _______X, __X__XX_, X_______,
  _______X, XXX__XXX, X_______,
  ______XX, XXX_XXXX, X_______,
  ______XX, XX__XXXX, ________,
  _______X, X____XX_, ________,

  __XX__XX,
  __XX_X_X,
  _XX_X_XX,
  _XXXXXXX,
  _X_X_XX_,
  XXX_XXX_,
  XXX_XXX_,
  _X___X__,

  ____XX__, XX______,
  ____XX_X, _X______,
  ___XX_X_, XX______,
  ___XXXXX, XX______,
  ___X_X_X, X_______,
  __XXX_XX, X_______,
  __XXX_XX, X_______,
  ___X___X, ________,

  ______XX, __XX____,
  ______XX, _X_X____,
  _____XX_, X_XX____,
  _____XXX, XXXX____,
  _____X_X, _XX_____,
  ____XXX_, XXX_____,
  ____XXX_, XXX_____,
  _____X__, _X______,

  ________, XX__XX__,
  ________, XX_X_X__,
  _______X, X_X_XX__,
  _______X, XXXXXX__,
  _______X, _X_XX___,
  ______XX, X_XXX___,
  ______XX, X_XXX___,
  _______X, ___X____,

  ________, __XXXXXX,
  ________, _XX____X,
  ________, _X_X_X_X,
  ________, X_X_X__X,
  ________, X__X_X_X,
  _______X, __X_X_XX,
  _______X, XXXXXXXX,
  ______X_, X_X_X_XX,
  ______XX, XX_XXXXX,
  _____XX_, XX_XX_XX,
  _____XXX, X__XXXXX,
  ____XXXX, XXXXXXX_,
  ____XXXX, XXXXXXX_,
  ___X____, ______X_,
  ___X_X_X, _X_X_XX_,
  __X_X_XX, XXX_X_X_,
  _XXXXX__, _XXXXXX_,
  _X_X_X__, _X_X_XX_,
  X___X___, _XX___X_,
  XXXXX___, _XXXXXX_,

  ________, ____XXXX, XX______,
  ________, ___XX___, _X______,
  ________, ___X_X_X, _X______,
  ________, __X_X_X_, _X______,
  ________, __X__X_X, _X______,
  ________, _X__X_X_, XX______,
  ________, _XXXXXXX, XX______,
  ________, X_X_X_X_, XX______,
  ________, XXXX_XXX, XX______,
  _______X, X_XX_XX_, XX______,
  _______X, XXX__XXX, XX______,
  ______XX, XXXXXXXX, X_______,
  ______XX, XXXXXXXX, X_______,
  _____X__, ________, X_______,
  _____X_X, _X_X_X_X, X_______,
  ____X_X_, XXXXX_X_, X_______,
  ___XXXXX, ___XXXXX, X_______,
  ___X_X_X, ___X_X_X, X_______,
  __X___X_, ___XX___, X_______,
  __XXXXX_, ___XXXXX, X_______,

  ________, ______XX, XXXX____,
  ________, _____XX_, ___X____,
  ________, _____X_X, _X_X____,
  ________, ____X_X_, X__X____,
  ________, ____X__X, _X_X____,
  ________, ___X__X_, X_XX____,
  ________, ___XXXXX, XXXX____,
  ________, __X_X_X_, X_XX____,
  ________, __XXXX_X, XXXX____,
  ________, _XX_XX_X, X_XX____,
  ________, _XXXX__X, XXXX____,
  ________, XXXXXXXX, XXX_____,
  ________, XXXXXXXX, XXX_____,
  _______X, ________, __X_____,
  _______X, _X_X_X_X, _XX_____,
  ______X_, X_XXXXX_, X_X_____,
  _____XXX, XX___XXX, XXX_____,
  _____X_X, _X___X_X, _XX_____,
  ____X___, X____XX_, __X_____,
  ____XXXX, X____XXX, XXX_____,

  ________, ________, XXXXXX__,
  ________, _______X, X____X__,
  ________, _______X, _X_X_X__,
  ________, ______X_, X_X__X__,
  ________, ______X_, _X_X_X__,
  ________, _____X__, X_X_XX__,
  ________, _____XXX, XXXXXX__,
  ________, ____X_X_, X_X_XX__,
  ________, ____XXXX, _XXXXX__,
  ________, ___XX_XX, _XX_XX__,
  ________, ___XXXX_, _XXXXX__,
  ________, __XXXXXX, XXXXX___,
  ________, __XXXXXX, XXXXX___,
  ________, _X______, ____X___,
  ________, _X_X_X_X, _X_XX___,
  ________, X_X_XXXX, X_X_X___,
  _______X, XXXX___X, XXXXX___,
  _______X, _X_X___X, _X_XX___,
  ______X_, __X____X, X___X___,
  ______XX, XXX____X, XXXXX___,

  ______XX, XX______,
  ______X_, _X______,
  _____X_X, _X______,
  _____XX_, _X______,
  ____XXXX, _X______,
  ____XX_X, XX______,
  ___XXX_X, XX______,
  ___XXXXX, XX______,
  __X_____, _X______,
  __XX_X_X, _X______,
  _XXXXXXX, XX______,
  _XXX__XX, XX______,
  XXXX__XX, XX______,
  XXX___XX, X_______,

  ________, XXXX____,
  ________, X__X____,
  _______X, _X_X____,
  _______X, X__X____,
  ______XX, XX_X____,
  ______XX, _XXX____,
  _____XXX, _XXX____,
  _____XXX, XXXX____,
  ____X___, ___X____,
  ____XX_X, _X_X____,
  ___XXXXX, XXXX____,
  ___XXX__, XXXX____,
  __XXXX__, XXXX____,
  __XXX___, XXX_____,

  ________, __XXXX__,
  ________, __X__X__,
  ________, _X_X_X__,
  ________, _XX__X__,
  ________, XXXX_X__,
  ________, XX_XXX__,
  _______X, XX_XXX__,
  _______X, XXXXXX__,
  ______X_, _____X__,
  ______XX, _X_X_X__,
  _____XXX, XXXXXX__,
  _____XXX, __XXXX__,
  ____XXXX, __XXXX__,
  ____XXX_, __XXX___,

  ________, ____XXXX,
  ________, ____X__X,
  ________, ___X_X_X,
  ________, ___XX__X,
  ________, __XXXX_X,
  ________, __XX_XXX,
  ________, _XXX_XXX,
  ________, _XXXXXXX,
  ________, X______X,
  ________, XX_X_X_X,
  _______X, XXXXXXXX,
  _______X, XX__XXXX,
  ______XX, XX__XXXX,
  ______XX, X___XXX_,

  ___XX___,
  __XXX___,
  __XXX___,
  _XX_X___,
  _XX_X___,
  X_X_X___,
  XXXXX___,
  XX_XX___,

  _____XX_,
  ____XXX_,
  ____XXX_,
  ___XX_X_,
  ___XX_X_,
  __X_X_X_,
  __XXXXX_,
  __XX_XX_,

  _______X, X_______,
  ______XX, X_______,
  ______XX, X_______,
  _____XX_, X_______,
  _____XX_, X_______,
  ____X_X_, X_______,
  ____XXXX, X_______,
  ____XX_X, X_______,

  ________, _XX_____,
  ________, XXX_____,
  ________, XXX_____,
  _______X, X_X_____,
  _______X, X_X_____,
  ______X_, X_X_____,
  ______XX, XXX_____,
  ______XX, _XX_____,

  ________, XXXXXXXX, XX______,
  ______XX, ________, __XX____,
  ______X_, _X_X_X_X, _X__X___,
  _____X__, X_X_X_X_, X_X_X___,
  _____X_X, _X_X_X_X, _X_XX___,
  ____XXX_, X_XXXXX_, X_XXX___,
  ____XX_X, _XX__X_X, _X_X____,
  ____XXXX, XX___XXX, XXXX____,
  ___X_X_X, _X______, ________,
  ___XXXXX, XXXXXXXX, XX______,
  ___XXXXX, XXXXXXXX, XXX_____,
  ____XXXX, XXXXXXXX, XXX_____,
  ________, __X_____, _XX_____,
  ________, __X_X_X_, XX______,
  _XXXXXX_, __XX_X_X, _X______,
  _XXXXXXX, XXXXXXXX, XX______,
  XXXXXXXX, XXXXXXXX, X_______,
  X__XXXXX, XXXXX__X, ________,
  _XX_____, _____XX_, ________,
  ___XXXXX, XXXXX___, ________,

  ________, __XXXXXX, XXXX____,
  ________, XX______, ____XX__,
  ________, X__X_X_X, _X_X__X_,
  _______X, __X_X_X_, X_X_X_X_,
  _______X, _X_X_X_X, _X_X_XX_,
  ______XX, X_X_XXXX, X_X_XXX_,
  ______XX, _X_XX__X, _X_X_X__,
  ______XX, XXXX___X, XXXXXX__,
  _____X_X, _X_X____, ________,
  _____XXX, XXXXXXXX, XXXX____,
  _____XXX, XXXXXXXX, XXXXX___,
  ______XX, XXXXXXXX, XXXXX___,
  ________, ____X___, ___XX___,
  ________, ____X_X_, X_XX____,
  ___XXXXX, X___XX_X, _X_X____,
  ___XXXXX, XXXXXXXX, XXXX____,
  __XXXXXX, XXXXXXXX, XXX_____,
  __X__XXX, XXXXXXX_, _X______,
  ___XX___, _______X, X_______,
  _____XXX, XXXXXXX_, ________,

  ________, ____XXXX, XXXXXX__, ________,
  ________, __XX____, ______XX, ________,
  ________, __X__X_X, _X_X_X__, X_______,
  ________, _X__X_X_, X_X_X_X_, X_______,
  ________, _X_X_X_X, _X_X_X_X, X_______,
  ________, XXX_X_XX, XXX_X_XX, X_______,
  ________, XX_X_XX_, _X_X_X_X, ________,
  ________, XXXXXX__, _XXXXXXX, ________,
  _______X, _X_X_X__, ________, ________,
  _______X, XXXXXXXX, XXXXXX__, ________,
  _______X, XXXXXXXX, XXXXXXX_, ________,
  ________, XXXXXXXX, XXXXXXX_, ________,
  ________, ______X_, _____XX_, ________,
  ________, ______X_, X_X_XX__, ________,
  _____XXX, XXX___XX, _X_X_X__, ________,
  _____XXX, XXXXXXXX, XXXXXX__, ________,
  ____XXXX, XXXXXXXX, XXXXX___, ________,
  ____X__X, XXXXXXXX, X__X____, ________,
  _____XX_, ________, _XX_____, ________,
  _______X, XXXXXXXX, X_______, ________,

  ________, ______XX, XXXXXXXX, ________,
  ________, ____XX__, ________, XX______,
  ________, ____X__X, _X_X_X_X, __X_____,
  ________, ___X__X_, X_X_X_X_, X_X_____,
  ________, ___X_X_X, _X_X_X_X, _XX_____,
  ________, __XXX_X_, XXXXX_X_, XXX_____,
  ________, __XX_X_X, X__X_X_X, _X______,
  ________, __XXXXXX, ___XXXXX, XX______,
  ________, _X_X_X_X, ________, ________,
  ________, _XXXXXXX, XXXXXXXX, ________,
  ________, _XXXXXXX, XXXXXXXX, X_______,
  ________, __XXXXXX, XXXXXXXX, X_______,
  ________, ________, X______X, X_______,
  ________, ________, X_X_X_XX, ________,
  _______X, XXXXX___, XX_X_X_X, ________,
  _______X, XXXXXXXX, XXXXXXXX, ________,
  ______XX, XXXXXXXX, XXXXXXX_, ________,
  ______X_, _XXXXXXX, XXX__X__, ________,
  _______X, X_______, ___XX___, ________,
  ________, _XXXXXXX, XXX_____, ________,

  ______XX, XX______,
  ___XXX_X, _XXX____,
  __X_X_X_, X_X_____,
  __XX_XXX, XX_X____,
  __X_X___, XXXX____,
  _X_X_X__, ________,
  _XXXXXXX, XXX_____,
  __XXXXXX, XXX_____,
  ______X_, __X_____,
  XXXX__X_, _X______,
  XXXXXXXX, XX______,
  XXXXXXXX, XX______,
  X_X_X_X_, X_______,
  _XXXXXXX, ________,

  ________, XXXX____,
  _____XXX, _X_XXX__,
  ____X_X_, X_X_X___,
  ____XX_X, XXXX_X__,
  ____X_X_, __XXXX__,
  ___X_X_X, ________,
  ___XXXXX, XXXXX___,
  ____XXXX, XXXXX___,
  ________, X___X___,
  __XXXX__, X__X____,
  __XXXXXX, XXXX____,
  __XXXXXX, XXXX____,
  __X_X_X_, X_X_____,
  ___XXXXX, XX______,

  ________, __XXXX__,
  _______X, XX_X_XXX,
  ______X_, X_X_X_X_,
  ______XX, _XXXXX_X,
  ______X_, X___XXXX,
  _____X_X, _X______,
  _____XXX, XXXXXXX_,
  ______XX, XXXXXXX_,
  ________, __X___X_,
  ____XXXX, __X__X__,
  ____XXXX, XXXXXX__,
  ____XXXX, XXXXXX__,
  ____X_X_, X_X_X___,
  _____XXX, XXXX____,

  ________, ____XXXX, ________,
  ________, _XXX_X_X, XX______,
  ________, X_X_X_X_, X_______,
  ________, XX_XXXXX, _X______,
  ________, X_X___XX, XX______,
  _______X, _X_X____, ________,
  _______X, XXXXXXXX, X_______,
  ________, XXXXXXXX, X_______,
  ________, ____X___, X_______,
  ______XX, XX__X__X, ________,
  ______XX, XXXXXXXX, ________,
  ______XX, XXXXXXXX, ________,
  ______X_, X_X_X_X_, ________,
  _______X, XXXXXX__, ________,

  __XXXX__,
  __XXXXX_,
  _XXX____,
  _XXXXXX_,
  ____X_X_,
  XXX_XX__,
  XXXXXX__,
  _XXXX___,

  ____XXXX, ________,
  ____XXXX, X_______,
  ___XXX__, ________,
  ___XXXXX, X_______,
  ______X_, X_______,
  __XXX_XX, ________,
  __XXXXXX, ________,
  ___XXXX_, ________,

  ______XX, XX______,
  ______XX, XXX_____,
  _____XXX, ________,
  _____XXX, XXX_____,
  ________, X_X_____,
  ____XXX_, XX______,
  ____XXXX, XX______,
  _____XXX, X_______,

  ________, XXXX____,
  ________, XXXXX___,
  _______X, XX______,
  _______X, XXXXX___,
  ________, __X_X___,
  ______XX, X_XX____,
  ______XX, XXXX____,
  _______X, XXX_____,

  _______X, XXXXXXXX, XX______,
  ______X_, ________, _X______,
  ______X_, _X_X_X_X, _X______,
  _____X__, X_X_X_X_, X_______,
  _____X_X, _X_XXXXX, X_______,
  _____XX_, X_X_____, ________,
  ____XX_X, _X______, ________,
  ____XXXX, XX______, ________,
  ___XX_X_, X_______, ________,
  ___XXXXX, XXXXXXXX, ________,
  ___XXXXX, XXXXXXX_, ________,
  __XXXXXX, XXXXXXX_, ________,
  __X___X_, ________, ________,
  __X_X_X_, ________, ________,
  _X_X_X__, ________, ________,
  _XXXXX__, ________, ________,
  XXXXXXXX, XXXXX___, ________,
  X_XXXXXX, XXXX____, ________,
  X_______, ___X____, ________,
  XXXXXXXX, XXXX____, ________,

  ________, _XXXXXXX, XXXX____,
  ________, X_______, ___X____,
  ________, X__X_X_X, _X_X____,
  _______X, __X_X_X_, X_X_____,
  _______X, _X_X_XXX, XXX_____,
  _______X, X_X_X___, ________,
  ______XX, _X_X____, ________,
  ______XX, XXXX____, ________,
  _____XX_, X_X_____, ________,
  _____XXX, XXXXXXXX, XX______,
  _____XXX, XXXXXXXX, X_______,
  ____XXXX, XXXXXXXX, X_______,
  ____X___, X_______, ________,
  ____X_X_, X_______, ________,
  ___X_X_X, ________, ________,
  ___XXXXX, ________, ________,
  __XXXXXX, XXXXXXX_, ________,
  __X_XXXX, XXXXXX__, ________,
  __X_____, _____X__, ________,
  __XXXXXX, XXXXXX__, ________,

  ________, ___XXXXX, XXXXXX__,
  ________, __X_____, _____X__,
  ________, __X__X_X, _X_X_X__,
  ________, _X__X_X_, X_X_X___,
  ________, _X_X_X_X, XXXXX___,
  ________, _XX_X_X_, ________,
  ________, XX_X_X__, ________,
  ________, XXXXXX__, ________,
  _______X, X_X_X___, ________,
  _______X, XXXXXXXX, XXXX____,
  _______X, XXXXXXXX, XXX_____,
  ______XX, XXXXXXXX, XXX_____,
  ______X_, __X_____, ________,
  ______X_, X_X_____, ________,
  _____X_X, _X______, ________,
  _____XXX, XX______, ________,
  ____XXXX, XXXXXXXX, X_______,
  ____X_XX, XXXXXXXX, ________,
  ____X___, _______X, ________,
  ____XXXX, XXXXXXXX, ________,

  ________, _____XXX, XXXXXXXX,
  ________, ____X___, _______X,
  ________, ____X__X, _X_X_X_X,
  ________, ___X__X_, X_X_X_X_,
  ________, ___X_X_X, _XXXXXX_,
  ________, ___XX_X_, X_______,
  ________, __XX_X_X, ________,
  ________, __XXXXXX, ________,
  ________, _XX_X_X_, ________,
  ________, _XXXXXXX, XXXXXX__,
  ________, _XXXXXXX, XXXXX___,
  ________, XXXXXXXX, XXXXX___,
  ________, X___X___, ________,
  ________, X_X_X___, ________,
  _______X, _X_X____, ________,
  _______X, XXXX____, ________,
  ______XX, XXXXXXXX, XXX_____,
  ______X_, XXXXXXXX, XX______,
  ______X_, ________, _X______,
  ______XX, XXXXXXXX, XX______,

  ____XXXX, XXXX____,
  ____X_X_, X_X_____,
  ___X_X_X, _XX_____,
  ___XX_X_, ________,
  ___X_X__, ________,
  __XXXX__, ________,
  __XXXXXX, XX______,
  __XXXXXX, X_______,
  _X__X___, ________,
  _X_X____, ________,
  _XXX____, ________,
  XXXXXXXX, ________,
  X_X_X_XX, ________,
  XXXXXXX_, ________,

  ______XX, XXXXXX__,
  ______X_, X_X_X___,
  _____X_X, _X_XX___,
  _____XX_, X_______,
  _____X_X, ________,
  ____XXXX, ________,
  ____XXXX, XXXX____,
  ____XXXX, XXX_____,
  ___X__X_, ________,
  ___X_X__, ________,
  ___XXX__, ________,
  __XXXXXX, XX______,
  __X_X_X_, XX______,
  __XXXXXX, X_______,

  ________, XXXXXXXX,
  ________, X_X_X_X_,
  _______X, _X_X_XX_,
  _______X, X_X_____,
  _______X, _X______,
  ______XX, XX______,
  ______XX, XXXXXX__,
  ______XX, XXXXX___,
  _____X__, X_______,
  _____X_X, ________,
  _____XXX, ________,
  ____XXXX, XXXX____,
  ____X_X_, X_XX____,
  ____XXXX, XXX_____,

  ________, __XXXXXX, XX______,
  ________, __X_X_X_, X_______,
  ________, _X_X_X_X, X_______,
  ________, _XX_X___, ________,
  ________, _X_X____, ________,
  ________, XXXX____, ________,
  ________, XXXXXXXX, ________,
  ________, XXXXXXX_, ________,
  _______X, __X_____, ________,
  _______X, _X______, ________,
  _______X, XX______, ________,
  ______XX, XXXXXX__, ________,
  ______X_, X_X_XX__, ________,
  ______XX, XXXXX___, ________,

  __XXXX__,
  __XXX___,
  _XX_____,
  _X_XX___,
  _XXX____,
  XX______,
  X_XX____,
  XXX_____,

  ____XXXX,
  ____XXX_,
  ___XX___,
  ___X_XX_,
  ___XXX__,
  __XX____,
  __X_XX__,
  __XXX___,

  ______XX, XX______,
  ______XX, X_______,
  _____XX_, ________,
  _____X_X, X_______,
  _____XXX, ________,
  ____XX__, ________,
  ____X_XX, ________,
  ____XXX_, ________,

  ________, XXXX____,
  ________, XXX_____,
  _______X, X_______,
  _______X, _XX_____,
  _______X, XX______,
  ______XX, ________,
  ______X_, XX______,
  ______XX, X_______,

  ________, XXXXXXXX, X_______,
  ______XX, ________, _XX_____,
  _____X__, X_X_X_X_, X__X____,
  ____X__X, _X_X_X_X, _X_X____,
  ____X_X_, X_X_X_X_, X__X____,
  ___X_X_X, _XXX_X_X, _X_X____,
  ___XX_X_, XX__X_X_, X_X_____,
  ___XXXXX, X___XXXX, XXX_____,
  __XXX_X_, X___X_X_, X_X_____,
  __XXXXXX, ___XXXXX, XX______,
  __XXXXXX, ___XXXXX, XX______,
  _XXXXXXX, ___XXXXX, XX______,
  _XXXXXX_, __XXXXXX, X_______,
  _X____X_, __X_____, X_______,
  X_X_X_XX, _X_X_X_X, ________,
  XXXXXXXX, XXXXXXXX, ________,
  XXXXXXXX, XXXXXXX_, ________,
  XX_XXXXX, XXXXXXX_, ________,
  _XX_____, XXXXXX__, ________,
  ___XXXXX, _XXXX___, ________,
  _______X, ___X____, ________,
  ________, X___X___, ________,
  ________, _XXXXX__, ________,
  ________, __XXXXX_, ________,

  ________, __XXXXXX, XXX_____,
  ________, XX______, ___XX___,
  _______X, __X_X_X_, X_X__X__,
  ______X_, _X_X_X_X, _X_X_X__,
  ______X_, X_X_X_X_, X_X__X__,
  _____X_X, _X_XXX_X, _X_X_X__,
  _____XX_, X_XX__X_, X_X_X___,
  _____XXX, XXX___XX, XXXXX___,
  ____XXX_, X_X___X_, X_X_X___,
  ____XXXX, XX___XXX, XXXX____,
  ____XXXX, XX___XXX, XXXX____,
  ___XXXXX, XX___XXX, XXXX____,
  ___XXXXX, X___XXXX, XXX_____,
  ___X____, X___X___, __X_____,
  __X_X_X_, XX_X_X_X, _X______,
  __XXXXXX, XXXXXXXX, XX______,
  __XXXXXX, XXXXXXXX, X_______,
  __XX_XXX, XXXXXXXX, X_______,
  ___XX___, __XXXXXX, ________,
  _____XXX, XX_XXXX_, ________,
  ________, _X___X__, ________,
  ________, __X___X_, ________,
  ________, ___XXXXX, ________,
  ________, ____XXXX, X_______,

  ________, ____XXXX, XXXXX___,
  ________, __XX____, _____XX_,
  ________, _X__X_X_, X_X_X__X,
  ________, X__X_X_X, _X_X_X_X,
  ________, X_X_X_X_, X_X_X__X,
  _______X, _X_X_XXX, _X_X_X_X,
  _______X, X_X_XX__, X_X_X_X_,
  _______X, XXXXX___, XXXXXXX_,
  ______XX, X_X_X___, X_X_X_X_,
  ______XX, XXXX___X, XXXXXX__,
  ______XX, XXXX___X, XXXXXX__,
  _____XXX, XXXX___X, XXXXXX__,
  _____XXX, XXX___XX, XXXXX___,
  _____X__, __X___X_, ____X___,
  ____X_X_, X_XX_X_X, _X_X____,
  ____XXXX, XXXXXXXX, XXXX____,
  ____XXXX, XXXXXXXX, XXX_____,
  ____XX_X, XXXXXXXX, XXX_____,
  _____XX_, ____XXXX, XX______,
  _______X, XXXX_XXX, X_______,
  ________, ___X___X, ________,
  ________, ____X___, X_______,
  ________, _____XXX, XX______,
  ________, ______XX, XXX_____,

  ________, ______XX, XXXXXXX_, ________,
  ________, ____XX__, _______X, X_______,
  ________, ___X__X_, X_X_X_X_, _X______,
  ________, __X__X_X, _X_X_X_X, _X______,
  ________, __X_X_X_, X_X_X_X_, _X______,
  ________, _X_X_X_X, XX_X_X_X, _X______,
  ________, _XX_X_XX, __X_X_X_, X_______,
  ________, _XXXXXX_, __XXXXXX, X_______,
  ________, XXX_X_X_, __X_X_X_, X_______,
  ________, XXXXXX__, _XXXXXXX, ________,
  ________, XXXXXX__, _XXXXXXX, ________,
  _______X, XXXXXX__, _XXXXXXX, ________,
  _______X, XXXXX___, XXXXXXX_, ________,
  _______X, ____X___, X_____X_, ________,
  ______X_, X_X_XX_X, _X_X_X__, ________,
  ______XX, XXXXXXXX, XXXXXX__, ________,
  ______XX, XXXXXXXX, XXXXX___, ________,
  ______XX, _XXXXXXX, XXXXX___, ________,
  _______X, X_____XX, XXXX____, ________,
  ________, _XXXXX_X, XXX_____, ________,
  ________, _____X__, _X______, ________,
  ________, ______X_, __X_____, ________,
  ________, _______X, XXXX____, ________,
  ________, ________, XXXXX___, ________,

  _____XXX, XXX_____,
  ____X___, ___XX___,
  ___X__X_, X_X_X___,
  __X_X_XX, _X_XX___,
  __XX_X__, X_X_X___,
  __X_XX_X, _X_X____,
  _XXXX__X, XXXX____,
  _XXXX__X, XXXX____,
  _XXXX_XX, XXX_____,
  X__X__X_, __X_____,
  X___XX__, _XX_____,
  XXXXXXXX, XX______,
  XXXXXXXX, XX______,
  _XXXX_XX, X_______,
  _____X_X, ________,
  ______XX, X_______,

  _______X, XXXXX___,
  ______X_, _____XX_,
  _____X__, X_X_X_X_,
  ____X_X_, XX_X_XX_,
  ____XX_X, __X_X_X_,
  ____X_XX, _X_X_X__,
  ___XXXX_, _XXXXX__,
  ___XXXX_, _XXXXX__,
  ___XXXX_, XXXXX___,
  __X__X__, X___X___,
  __X___XX, ___XX___,
  __XXXXXX, XXXX____,
  __XXXXXX, XXXX____,
  ___XXXX_, XXX_____,
  _______X, _X______,
  ________, XXX_____,

  ________, _XXXXXX_, ________,
  ________, X______X, X_______,
  _______X, __X_X_X_, X_______,
  ______X_, X_XX_X_X, X_______,
  ______XX, _X__X_X_, X_______,
  ______X_, XX_X_X_X, ________,
  _____XXX, X__XXXXX, ________,
  _____XXX, X__XXXXX, ________,
  _____XXX, X_XXXXX_, ________,
  ____X__X, __X___X_, ________,
  ____X___, XX___XX_, ________,
  ____XXXX, XXXXXX__, ________,
  ____XXXX, XXXXXX__, ________,
  _____XXX, X_XXX___, ________,
  ________, _X_X____, ________,
  ________, __XXX___, ________,

  ________, ___XXXXX, X_______,
  ________, __X_____, _XX_____,
  ________, _X__X_X_, X_X_____,
  ________, X_X_XX_X, _XX_____,
  ________, XX_X__X_, X_X_____,
  ________, X_XX_X_X, _X______,
  _______X, XXX__XXX, XX______,
  _______X, XXX__XXX, XX______,
  _______X, XXX_XXXX, X_______,
  ______X_, _X__X___, X_______,
  ______X_, __XX___X, X_______,
  ______XX, XXXXXXXX, ________,
  ______XX, XXXXXXXX, ________,
  _______X, XXX_XXX_, ________,
  ________, ___X_X__, ________,
  ________, ____XXX_, ________,

  __XXXXX_,
  __XX_XXX,
  _XX___XX,
  _XX__XX_,
  XX_XXXX_,
  X_X_XX__,
  XX_X_X__,
  _XXXX___,
  ___XXX__,
  ____XX__,

  ____XXXX, X_______,
  ____XX_X, XX______,
  ___XX___, XX______,
  ___XX__X, X_______,
  __XX_XXX, X_______,
  __X_X_XX, ________,
  __XX_X_X, ________,
  ___XXXX_, ________,
  _____XXX, ________,
  ______XX, ________,

  ______XX, XXX_____,
  ______XX, _XXX____,
  _____XX_, __XX____,
  _____XX_, _XX_____,
  ____XX_X, XXX_____,
  ____X_X_, XX______,
  ____XX_X, _X______,
  _____XXX, X_______,
  _______X, XX______,
  ________, XX______,

  ________, XXXXX___,
  ________, XX_XXX__,
  _______X, X___XX__,
  _______X, X__XX___,
  ______XX, _XXXX___,
  ______X_, X_XX____,
  ______XX, _X_X____,
  _______X, XXX_____,
  ________, _XXX____,
  ________, __XX____,

  _XXXXX__,
  X_XX__X_,
  XXXXX_X_,
  XXXXXXX_,
  XX_X_XX_,
  _XXXXX__,

  ___XXXXX, ________,
  __X_XX__, X_______,
  __XXXXX_, X_______,
  __XXXXXX, X_______,
  __XX_X_X, X_______,
  ___XXXXX, ________,

  _____XXX, XX______,
  ____X_XX, __X_____,
  ____XXXX, X_X_____,
  ____XXXX, XXX_____,
  ____XX_X, _XX_____,
  _____XXX, XX______,

  _______X, XXXX____,
  ______X_, XX__X___,
  ______XX, XXX_X___,
  ______XX, XXXXX___,
  ______XX, _X_XX___,
  _______X, XXXX____,

  _XXXX___,
  X_X_X___,
  XXXXX___,
  _XXX____,

  ___XXXX_,
  __X_X_X_,
  __XXXXX_,
  ___XXX__,

  _____XXX, X_______,
  ____X_X_, X_______,
  ____XXXX, X_______,
  _____XXX, ________,

  _______X, XXX_____,
  ______X_, X_X_____,
  ______XX, XXX_____,
  _______X, XX______,

  XXX_____,
  XX______,

  __XXX___,
  __XX____,

  ____XXX_, ________,
  ____XX__, ________,

  ______XX, X_______,
  ______XX, ________,
};

/**
 * $F225-$F240: tunes
 *
 * 128K bank 3: title-tune engine tune-select table, transcribed directly
 * from the skool's DEFB bytes. 4 entries, 7 bytes each: tempo byte, then
 * 3 x 2-byte little-endian pattern-data pointers (channels 1-3). Used by
 * start_tune ($EB9E@bank3). Tune 0's channel-1 pointer ($F241) lands exactly
 * on the byte immediately following this table, confirming its 4-entry
 * extent. The pattern-data blocks these pointers reference are extracted as
 * title_tune0_data/title_tune1_data below, for tunes 0 and 1 -- see
 * start_tune's Translation notes.
 */
const tune_t tunes[4] = {
  { 2, { TITLE_TUNE0_DATA_ADDR, 0xF25A, 0xF265 } },
  { 4, { TITLE_TUNE1_DATA_ADDR, 0xF605, 0xF609 } },
  { 2, { 0xF666, 0xF66A, 0xF66E } },
  { 3, { 0xF6F4, 0xF6F8, 0xF6FE } }
};

/**
 * $F241-$F642: title_tune0_data
 *
 * 128K bank 3: title-tune engine raw pattern-data region for tune 0 (title
 * screen), transcribed byte-exact from bank3.bin. See declaration comments
 * in Bank3Data.h. Tunes 2 and 3 are not extracted (unreachable from the
 * code paths wired up so far).
 */
const u8 title_tune0_data[1026] = {
  0x6E, /* $F241: HEADER_PATTERN_PTR [tune0ch0] */
  0xF2, /* $F242: (high byte) */
  0x02, /* $F243: PHRASE_TABLE_WORD */
  0x00, /* $F244: (high byte) */
  0x08, /* $F245: PHRASE_TABLE_REPEAT_COUNT */
  0x6E, /* $F246: PHRASE_TABLE_REPEAT_PTR */
  0xF2, /* $F247: (high byte) */
  0x97, /* $F248: PHRASE_TABLE_WORD */
  0xF2, /* $F249: (high byte) */
  0x88, /* $F24A: PHRASE_TABLE_WORD */
  0xF2, /* $F24B: (high byte) */
  0xA7, /* $F24C: PHRASE_TABLE_WORD */
  0xF2, /* $F24D: (high byte) */
  0x88, /* $F24E: PHRASE_TABLE_WORD */
  0xF2, /* $F24F: (high byte) */
  0x88, /* $F250: PHRASE_TABLE_WORD */
  0xF2, /* $F251: (high byte) */
  0x97, /* $F252: PHRASE_TABLE_WORD */
  0xF2, /* $F253: (high byte) */
  0x88, /* $F254: PHRASE_TABLE_WORD */
  0xF2, /* $F255: (high byte) */
  0x39, /* $F256: PHRASE_TABLE_WORD */
  0xF3, /* $F257: (high byte) */
  0x00, /* $F258: PHRASE_TABLE_WORD */
  0x00, /* $F259: (high byte) */
  0x6B, /* $F25A: HEADER_PATTERN_PTR [tune0ch1] */
  0xF2, /* $F25B: (high byte) */
  0x02, /* $F25C: PHRASE_TABLE_WORD */
  0x00, /* $F25D: (high byte) */
  0x07, /* $F25E: PHRASE_TABLE_REPEAT_COUNT */
  0x6B, /* $F25F: PHRASE_TABLE_REPEAT_PTR */
  0xF2, /* $F260: (high byte) */
  0xE1, /* $F261: PHRASE_TABLE_WORD */
  0xF4, /* $F262: (high byte) */
  0x00, /* $F263: PHRASE_TABLE_WORD */
  0x00, /* $F264: (high byte) */
  0x50, /* $F265: HEADER_PATTERN_PTR [tune0ch2] */
  0xF3, /* $F266: (high byte) */
  0x9F, /* $F267: PHRASE_TABLE_WORD */
  0xF3, /* $F268: (high byte) */
  0x00, /* $F269: PHRASE_TABLE_WORD */
  0x00, /* $F26A: (high byte) */
  0xFF, /* $F26B: PCMD_SET_ROW_WAIT(32) [tune0ch1] */
  0x80, /* $F26C: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch1] */
  0x87, /* $F26D: PCMD_ADVANCE_PHRASE [tune0ch1] */
  0x8A, /* $F26E: PCMD_SET_MIXER_BITS_HIGH3 [tune0ch0] */
  0x90, /* $F26F: PCMD_MUTE_CHANNEL [tune0ch0] */
  0xD0, /* $F270: PCMD_SELECT_ENVELOPE_SHAPE(0) [tune0ch0] */
  0xB9, /* $F271: PCMD_SELECT_PITCH_OFFSET(1) [tune0ch0] */
  0x88, /* $F272: PCMD_SET_ENVELOPE_PARAMS [tune0ch0] */
  0x02, /* $F273: PCMD_SET_ENVELOPE_PARAMS_OPERAND [tune0ch0] */
  0x01, /* $F274: PCMD_SET_ENVELOPE_PARAMS_OPERAND [tune0ch0] */
  0x82, /* $F275: PCMD_VIBRATO_ON [tune0ch0] */
  0xE3, /* $F276: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x15, /* $F277: NOTE_G2 [tune0ch0] */
  0x15, /* $F278: NOTE_G2 [tune0ch0] */
  0x21, /* $F279: NOTE_G3 [tune0ch0] */
  0x15, /* $F27A: NOTE_G2 [tune0ch0] */
  0x15, /* $F27B: NOTE_G2 [tune0ch0] */
  0x21, /* $F27C: NOTE_G3 [tune0ch0] */
  0x15, /* $F27D: NOTE_G2 [tune0ch0] */
  0x15, /* $F27E: NOTE_G2 [tune0ch0] */
  0x21, /* $F27F: NOTE_G3 [tune0ch0] */
  0x15, /* $F280: NOTE_G2 [tune0ch0] */
  0x15, /* $F281: NOTE_G2 [tune0ch0] */
  0x21, /* $F282: NOTE_G3 [tune0ch0] */
  0x15, /* $F283: NOTE_G2 [tune0ch0] */
  0x15, /* $F284: NOTE_G2 [tune0ch0] */
  0x12, /* $F285: NOTE_E2 [tune0ch0] */
  0x14, /* $F286: NOTE_FS2 [tune0ch0] */
  0x87, /* $F287: PCMD_ADVANCE_PHRASE [tune0ch0] */
  0x8A, /* $F288: PCMD_SET_MIXER_BITS_HIGH3 [tune0ch0] */
  0x90, /* $F289: PCMD_MUTE_CHANNEL [tune0ch0] */
  0xD0, /* $F28A: PCMD_SELECT_ENVELOPE_SHAPE(0) [tune0ch0] */
  0xB9, /* $F28B: PCMD_SELECT_PITCH_OFFSET(1) [tune0ch0] */
  0x81, /* $F28C: PCMD_VIBRATO_OFF [tune0ch0] */
  0xE3, /* $F28D: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x15, /* $F28E: NOTE_G2 [tune0ch0] */
  0x15, /* $F28F: NOTE_G2 [tune0ch0] */
  0x15, /* $F290: NOTE_G2 [tune0ch0] */
  0x15, /* $F291: NOTE_G2 [tune0ch0] */
  0x15, /* $F292: NOTE_G2 [tune0ch0] */
  0x15, /* $F293: NOTE_G2 [tune0ch0] */
  0x15, /* $F294: NOTE_G2 [tune0ch0] */
  0x15, /* $F295: NOTE_G2 [tune0ch0] */
  0x87, /* $F296: PCMD_ADVANCE_PHRASE [tune0ch0] */
  0x1A, /* $F297: NOTE_C3 [tune0ch0] */
  0x1A, /* $F298: NOTE_C3 [tune0ch0] */
  0x1A, /* $F299: NOTE_C3 [tune0ch0] */
  0x1A, /* $F29A: NOTE_C3 [tune0ch0] */
  0x1A, /* $F29B: NOTE_C3 [tune0ch0] */
  0x1A, /* $F29C: NOTE_C3 [tune0ch0] */
  0x1A, /* $F29D: NOTE_C3 [tune0ch0] */
  0xE7, /* $F29E: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x18, /* $F29F: NOTE_AS2 [tune0ch0] */
  0x17, /* $F2A0: NOTE_A2 [tune0ch0] */
  0x15, /* $F2A1: NOTE_G2 [tune0ch0] */
  0xE3, /* $F2A2: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x13, /* $F2A3: NOTE_F2 [tune0ch0] */
  0x12, /* $F2A4: NOTE_E2 [tune0ch0] */
  0x10, /* $F2A5: NOTE_D2 [tune0ch0] */
  0x87, /* $F2A6: PCMD_ADVANCE_PHRASE [tune0ch0] */
  0x13, /* $F2A7: NOTE_F2 [tune0ch0] */
  0x13, /* $F2A8: NOTE_F2 [tune0ch0] */
  0x13, /* $F2A9: NOTE_F2 [tune0ch0] */
  0x13, /* $F2AA: NOTE_F2 [tune0ch0] */
  0x13, /* $F2AB: NOTE_F2 [tune0ch0] */
  0x13, /* $F2AC: NOTE_F2 [tune0ch0] */
  0x13, /* $F2AD: NOTE_F2 [tune0ch0] */
  0x13, /* $F2AE: NOTE_F2 [tune0ch0] */
  0x12, /* $F2AF: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B0: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B1: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B2: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B3: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B4: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B5: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B6: NOTE_E2 [tune0ch0] */
  0x10, /* $F2B7: NOTE_D2 [tune0ch0] */
  0x10, /* $F2B8: NOTE_D2 [tune0ch0] */
  0x12, /* $F2B9: NOTE_E2 [tune0ch0] */
  0x10, /* $F2BA: NOTE_D2 [tune0ch0] */
  0x13, /* $F2BB: NOTE_F2 [tune0ch0] */
  0x10, /* $F2BC: NOTE_D2 [tune0ch0] */
  0x15, /* $F2BD: NOTE_G2 [tune0ch0] */
  0x13, /* $F2BE: NOTE_F2 [tune0ch0] */
  0x10, /* $F2BF: NOTE_D2 [tune0ch0] */
  0x10, /* $F2C0: NOTE_D2 [tune0ch0] */
  0x12, /* $F2C1: NOTE_E2 [tune0ch0] */
  0x10, /* $F2C2: NOTE_D2 [tune0ch0] */
  0xE1, /* $F2C3: PCMD_SET_ROW_WAIT(2) [tune0ch0] */
  0x10, /* $F2C4: NOTE_D2 [tune0ch0] */
  0x12, /* $F2C5: NOTE_E2 [tune0ch0] */
  0x13, /* $F2C6: NOTE_F2 [tune0ch0] */
  0x15, /* $F2C7: NOTE_G2 [tune0ch0] */
  0x16, /* $F2C8: NOTE_GS2 [tune0ch0] */
  0x17, /* $F2C9: NOTE_A2 [tune0ch0] */
  0x1A, /* $F2CA: NOTE_C3 [tune0ch0] */
  0x19, /* $F2CB: NOTE_B2 [tune0ch0] */
  0xE3, /* $F2CC: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x15, /* $F2CD: NOTE_G2 [tune0ch0] */
  0x14, /* $F2CE: NOTE_FS2 [tune0ch0] */
  0x13, /* $F2CF: NOTE_F2 [tune0ch0] */
  0xE7, /* $F2D0: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x12, /* $F2D1: NOTE_E2 [tune0ch0] */
  0xE3, /* $F2D2: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x12, /* $F2D3: NOTE_E2 [tune0ch0] */
  0x12, /* $F2D4: NOTE_E2 [tune0ch0] */
  0x12, /* $F2D5: NOTE_E2 [tune0ch0] */
  0x17, /* $F2D6: NOTE_A2 [tune0ch0] */
  0x16, /* $F2D7: NOTE_GS2 [tune0ch0] */
  0x15, /* $F2D8: NOTE_G2 [tune0ch0] */
  0xE7, /* $F2D9: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x14, /* $F2DA: NOTE_FS2 [tune0ch0] */
  0xE3, /* $F2DB: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x14, /* $F2DC: NOTE_FS2 [tune0ch0] */
  0x14, /* $F2DD: NOTE_FS2 [tune0ch0] */
  0x14, /* $F2DE: NOTE_FS2 [tune0ch0] */
  0x19, /* $F2DF: NOTE_B2 [tune0ch0] */
  0x18, /* $F2E0: NOTE_AS2 [tune0ch0] */
  0x17, /* $F2E1: NOTE_A2 [tune0ch0] */
  0xE7, /* $F2E2: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x15, /* $F2E3: NOTE_G2 [tune0ch0] */
  0xE3, /* $F2E4: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x15, /* $F2E5: NOTE_G2 [tune0ch0] */
  0x15, /* $F2E6: NOTE_G2 [tune0ch0] */
  0xE7, /* $F2E7: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x19, /* $F2E8: NOTE_B2 [tune0ch0] */
  0x1A, /* $F2E9: NOTE_C3 [tune0ch0] */
  0xE3, /* $F2EA: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x1B, /* $F2EB: NOTE_CS3 [tune0ch0] */
  0x1C, /* $F2EC: NOTE_D3 [tune0ch0] */
  0x10, /* $F2ED: NOTE_D2 [tune0ch0] */
  0x10, /* $F2EE: NOTE_D2 [tune0ch0] */
  0x10, /* $F2EF: NOTE_D2 [tune0ch0] */
  0xE3, /* $F2F0: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x15, /* $F2F1: NOTE_G2 [tune0ch0] */
  0x14, /* $F2F2: NOTE_FS2 [tune0ch0] */
  0x13, /* $F2F3: NOTE_F2 [tune0ch0] */
  0xE7, /* $F2F4: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x12, /* $F2F5: NOTE_E2 [tune0ch0] */
  0xE3, /* $F2F6: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x12, /* $F2F7: NOTE_E2 [tune0ch0] */
  0x12, /* $F2F8: NOTE_E2 [tune0ch0] */
  0x12, /* $F2F9: NOTE_E2 [tune0ch0] */
  0x17, /* $F2FA: NOTE_A2 [tune0ch0] */
  0x16, /* $F2FB: NOTE_GS2 [tune0ch0] */
  0x15, /* $F2FC: NOTE_G2 [tune0ch0] */
  0xE7, /* $F2FD: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x14, /* $F2FE: NOTE_FS2 [tune0ch0] */
  0xE3, /* $F2FF: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x14, /* $F300: NOTE_FS2 [tune0ch0] */
  0x14, /* $F301: NOTE_FS2 [tune0ch0] */
  0x14, /* $F302: NOTE_FS2 [tune0ch0] */
  0x19, /* $F303: NOTE_B2 [tune0ch0] */
  0x18, /* $F304: NOTE_AS2 [tune0ch0] */
  0x17, /* $F305: NOTE_A2 [tune0ch0] */
  0xE7, /* $F306: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x15, /* $F307: NOTE_G2 [tune0ch0] */
  0xE3, /* $F308: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x15, /* $F309: NOTE_G2 [tune0ch0] */
  0x15, /* $F30A: NOTE_G2 [tune0ch0] */
  0xE7, /* $F30B: -- unreached by tune 0/1 playback -- */
  0x16, /* $F30C: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F30D: -- unreached by tune 0/1 playback -- */
  0x80, /* $F30E: -- unreached by tune 0/1 playback -- */
  0xEF, /* $F30F: -- unreached by tune 0/1 playback -- */
  0x18, /* $F310: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F311: -- unreached by tune 0/1 playback -- */
  0x80, /* $F312: -- unreached by tune 0/1 playback -- */
  0x80, /* $F313: -- unreached by tune 0/1 playback -- */
  0x12, /* $F314: -- unreached by tune 0/1 playback -- */
  0x13, /* $F315: -- unreached by tune 0/1 playback -- */
  0x12, /* $F316: -- unreached by tune 0/1 playback -- */
  0x14, /* $F317: -- unreached by tune 0/1 playback -- */
  0x12, /* $F318: -- unreached by tune 0/1 playback -- */
  0x16, /* $F319: -- unreached by tune 0/1 playback -- */
  0x13, /* $F31A: -- unreached by tune 0/1 playback -- */
  0x12, /* $F31B: -- unreached by tune 0/1 playback -- */
  0x12, /* $F31C: -- unreached by tune 0/1 playback -- */
  0x13, /* $F31D: -- unreached by tune 0/1 playback -- */
  0x12, /* $F31E: -- unreached by tune 0/1 playback -- */
  0x14, /* $F31F: -- unreached by tune 0/1 playback -- */
  0x12, /* $F320: -- unreached by tune 0/1 playback -- */
  0x16, /* $F321: -- unreached by tune 0/1 playback -- */
  0x13, /* $F322: -- unreached by tune 0/1 playback -- */
  0x80, /* $F323: -- unreached by tune 0/1 playback -- */
  0x12, /* $F324: -- unreached by tune 0/1 playback -- */
  0x13, /* $F325: -- unreached by tune 0/1 playback -- */
  0x12, /* $F326: -- unreached by tune 0/1 playback -- */
  0x14, /* $F327: -- unreached by tune 0/1 playback -- */
  0x12, /* $F328: -- unreached by tune 0/1 playback -- */
  0x16, /* $F329: -- unreached by tune 0/1 playback -- */
  0x13, /* $F32A: -- unreached by tune 0/1 playback -- */
  0x12, /* $F32B: -- unreached by tune 0/1 playback -- */
  0x12, /* $F32C: -- unreached by tune 0/1 playback -- */
  0x13, /* $F32D: -- unreached by tune 0/1 playback -- */
  0x12, /* $F32E: -- unreached by tune 0/1 playback -- */
  0xE1, /* $F32F: -- unreached by tune 0/1 playback -- */
  0x10, /* $F330: -- unreached by tune 0/1 playback -- */
  0x12, /* $F331: -- unreached by tune 0/1 playback -- */
  0x13, /* $F332: -- unreached by tune 0/1 playback -- */
  0x15, /* $F333: -- unreached by tune 0/1 playback -- */
  0x16, /* $F334: -- unreached by tune 0/1 playback -- */
  0x17, /* $F335: -- unreached by tune 0/1 playback -- */
  0x1A, /* $F336: -- unreached by tune 0/1 playback -- */
  0x19, /* $F337: -- unreached by tune 0/1 playback -- */
  0x87, /* $F338: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F339: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x13, /* $F33A: -- unreached by tune 0/1 playback -- */
  0x13, /* $F33B: -- unreached by tune 0/1 playback -- */
  0x13, /* $F33C: -- unreached by tune 0/1 playback -- */
  0x13, /* $F33D: -- unreached by tune 0/1 playback -- */
  0x13, /* $F33E: -- unreached by tune 0/1 playback -- */
  0x13, /* $F33F: -- unreached by tune 0/1 playback -- */
  0x13, /* $F340: -- unreached by tune 0/1 playback -- */
  0x13, /* $F341: -- unreached by tune 0/1 playback -- */
  0x12, /* $F342: -- unreached by tune 0/1 playback -- */
  0x12, /* $F343: -- unreached by tune 0/1 playback -- */
  0x12, /* $F344: -- unreached by tune 0/1 playback -- */
  0x12, /* $F345: -- unreached by tune 0/1 playback -- */
  0xE1, /* $F346: -- unreached by tune 0/1 playback -- */
  0x10, /* $F347: -- unreached by tune 0/1 playback -- */
  0x12, /* $F348: -- unreached by tune 0/1 playback -- */
  0x13, /* $F349: -- unreached by tune 0/1 playback -- */
  0x15, /* $F34A: -- unreached by tune 0/1 playback -- */
  0x16, /* $F34B: -- unreached by tune 0/1 playback -- */
  0x17, /* $F34C: -- unreached by tune 0/1 playback -- */
  0x1A, /* $F34D: -- unreached by tune 0/1 playback -- */
  0x19, /* $F34E: -- unreached by tune 0/1 playback -- */
  0x87, /* $F34F: -- unreached by tune 0/1 playback -- */
  0x8A, /* $F350: PCMD_SET_MIXER_BITS_HIGH3 [tune0ch2] */
  0x91, /* $F351: PCMD_UNMUTE_CHANNEL [tune0ch2] */
  0xD1, /* $F352: PCMD_SELECT_ENVELOPE_SHAPE(1) [tune0ch2] */
  0x81, /* $F353: PCMD_VIBRATO_OFF [tune0ch2] */
  0xBA, /* $F354: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0xE3, /* $F355: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x39, /* $F356: NOTE_G5 [tune0ch2] */
  0xEB, /* $F357: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F358: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F359: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x37, /* $F35A: NOTE_F5 [tune0ch2] */
  0xE3, /* $F35B: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F35C: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F35D: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F35E: NOTE_E5 [tune0ch2] */
  0xE7, /* $F35F: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F360: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBA, /* $F361: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0xE3, /* $F362: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x37, /* $F363: NOTE_F5 [tune0ch2] */
  0x80, /* $F364: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F365: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F366: NOTE_E5 [tune0ch2] */
  0x80, /* $F367: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBA, /* $F368: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0x39, /* $F369: NOTE_G5 [tune0ch2] */
  0xEB, /* $F36A: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F36B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F36C: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x37, /* $F36D: NOTE_F5 [tune0ch2] */
  0xE3, /* $F36E: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F36F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F370: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F371: NOTE_E5 [tune0ch2] */
  0xE7, /* $F372: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F373: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F374: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0xBA, /* $F375: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0x37, /* $F376: NOTE_F5 [tune0ch2] */
  0x80, /* $F377: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F378: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F379: NOTE_E5 [tune0ch2] */
  0x80, /* $F37A: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBA, /* $F37B: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0xE3, /* $F37C: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x39, /* $F37D: NOTE_G5 [tune0ch2] */
  0xEB, /* $F37E: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F37F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F380: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x37, /* $F381: NOTE_F5 [tune0ch2] */
  0xE3, /* $F382: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F383: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F384: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F385: NOTE_E5 [tune0ch2] */
  0xE7, /* $F386: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F387: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBA, /* $F388: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0xE3, /* $F389: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x37, /* $F38A: NOTE_F5 [tune0ch2] */
  0x80, /* $F38B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F38C: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F38D: NOTE_E5 [tune0ch2] */
  0x80, /* $F38E: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x80, /* $F38F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBA, /* $F390: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0x39, /* $F391: NOTE_G5 [tune0ch2] */
  0xE7, /* $F392: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F393: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F394: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x37, /* $F395: NOTE_F5 [tune0ch2] */
  0x80, /* $F396: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F397: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F398: NOTE_E5 [tune0ch2] */
  0x80, /* $F399: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F39A: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0xBC, /* $F39B: PCMD_SELECT_PITCH_OFFSET(4) [tune0ch2] */
  0x34, /* $F39C: NOTE_D5 [tune0ch2] */
  0x80, /* $F39D: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x87, /* $F39E: PCMD_ADVANCE_PHRASE [tune0ch2] */
  0x8A, /* $F39F: PCMD_SET_MIXER_BITS_HIGH3 [tune0ch2] */
  0x91, /* $F3A0: PCMD_UNMUTE_CHANNEL [tune0ch2] */
  0xD2, /* $F3A1: PCMD_SELECT_ENVELOPE_SHAPE(2) [tune0ch2] */
  0x81, /* $F3A2: PCMD_VIBRATO_OFF [tune0ch2] */
  0xBD, /* $F3A3: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F3A4: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F3A5: NOTE_B5 [tune0ch2] */
  0xEB, /* $F3A6: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F3A7: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F3A8: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x3B, /* $F3A9: NOTE_A5 [tune0ch2] */
  0xE3, /* $F3AA: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F3AB: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3AC: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3AD: NOTE_G5 [tune0ch2] */
  0xE7, /* $F3AE: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F3AF: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F3B0: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F3B1: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F3B2: NOTE_A5 [tune0ch2] */
  0x80, /* $F3B3: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3B4: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3B5: NOTE_G5 [tune0ch2] */
  0x80, /* $F3B6: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F3B7: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3D, /* $F3B8: NOTE_B5 [tune0ch2] */
  0xEB, /* $F3B9: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F3BA: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F3BB: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x3B, /* $F3BC: NOTE_A5 [tune0ch2] */
  0xE3, /* $F3BD: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F3BE: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3BF: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3C0: NOTE_G5 [tune0ch2] */
  0xE7, /* $F3C1: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F3C2: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F3C3: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0xBD, /* $F3C4: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3B, /* $F3C5: NOTE_A5 [tune0ch2] */
  0x80, /* $F3C6: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3C7: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3C8: NOTE_G5 [tune0ch2] */
  0x80, /* $F3C9: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F3CA: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F3CB: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F3CC: NOTE_B5 [tune0ch2] */
  0xEB, /* $F3CD: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F3CE: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F3CF: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x3B, /* $F3D0: NOTE_A5 [tune0ch2] */
  0xE3, /* $F3D1: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F3D2: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3D3: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3D4: NOTE_G5 [tune0ch2] */
  0xE7, /* $F3D5: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F3D6: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F3D7: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F3D8: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F3D9: NOTE_A5 [tune0ch2] */
  0x80, /* $F3DA: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3DB: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3DC: NOTE_G5 [tune0ch2] */
  0x80, /* $F3DD: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x80, /* $F3DE: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F3DF: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3D, /* $F3E0: NOTE_B5 [tune0ch2] */
  0xE7, /* $F3E1: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F3E2: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F3E3: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F3E4: NOTE_A5 [tune0ch2] */
  0x80, /* $F3E5: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3E6: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3E7: NOTE_G5 [tune0ch2] */
  0x80, /* $F3E8: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F3E9: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0xBF, /* $F3EA: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0x39, /* $F3EB: NOTE_G5 [tune0ch2] */
  0x80, /* $F3EC: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F3ED: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F3EE: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F3EF: NOTE_B5 [tune0ch2] */
  0xE7, /* $F3F0: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F3F1: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F3F2: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F3F3: NOTE_B5 [tune0ch2] */
  0xEB, /* $F3F4: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F3F5: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F3F6: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F3F7: NOTE_A5 [tune0ch2] */
  0xF7, /* $F3F8: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x8F, /* $F3F9: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE7, /* $F3FA: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F3FB: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3FC: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0xF7, /* $F3FD: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x39, /* $F3FE: NOTE_G5 [tune0ch2] */
  0xE3, /* $F3FF: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F400: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F401: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3D, /* $F402: NOTE_B5 [tune0ch2] */
  0x80, /* $F403: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F404: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3B, /* $F405: NOTE_A5 [tune0ch2] */
  0x80, /* $F406: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x39, /* $F407: NOTE_G5 [tune0ch2] */
  0x80, /* $F408: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBF, /* $F409: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0x37, /* $F40A: NOTE_F5 [tune0ch2] */
  0xE7, /* $F40B: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F40C: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F40D: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F40E: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F40F: NOTE_B5 [tune0ch2] */
  0xEB, /* $F410: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F411: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F412: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F413: NOTE_B5 [tune0ch2] */
  0xEB, /* $F414: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F415: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F416: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F417: NOTE_A5 [tune0ch2] */
  0xF7, /* $F418: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x8F, /* $F419: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F41A: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F41B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F41C: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F41D: NOTE_G5 [tune0ch2] */
  0xF7, /* $F41E: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x8F, /* $F41F: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F420: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F421: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xC1, /* $F422: PCMD_SELECT_PITCH_OFFSET(9) [tune0ch2] */
  0x38, /* $F423: NOTE_FS5 [tune0ch2] */
  0x80, /* $F424: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x38, /* $F425: NOTE_FS5 [tune0ch2] */
  0xE7, /* $F426: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F427: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F428: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x38, /* $F429: NOTE_FS5 [tune0ch2] */
  0xEB, /* $F42A: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F42B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F42C: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x38, /* $F42D: NOTE_FS5 [tune0ch2] */
  0xE7, /* $F42E: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F42F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F430: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x38, /* $F431: NOTE_FS5 [tune0ch2] */
  0xEF, /* $F432: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x80, /* $F433: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBF, /* $F434: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0xE3, /* $F435: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x39, /* $F436: NOTE_G5 [tune0ch2] */
  0xE7, /* $F437: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F438: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F439: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xEB, /* $F43A: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3A, /* $F43B: NOTE_GS5 [tune0ch2] */
  0xE7, /* $F43C: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F43D: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBF, /* $F43E: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0xE3, /* $F43F: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F440: NOTE_A5 [tune0ch2] */
  0xE7, /* $F441: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F442: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F443: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xEB, /* $F444: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3C, /* $F445: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F446: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F447: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F448: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F449: NOTE_B5 [tune0ch2] */
  0xE7, /* $F44A: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F44B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEB, /* $F44C: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3D, /* $F44D: NOTE_B5 [tune0ch2] */
  0xE3, /* $F44E: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F44F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3D, /* $F450: NOTE_B5 [tune0ch2] */
  0xBF, /* $F451: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0x3E, /* $F452: NOTE_C6 [tune0ch2] */
  0x80, /* $F453: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3F, /* $F454: NOTE_CS6 [tune0ch2] */
  0x40, /* $F455: NOTE_D6 [tune0ch2] */
  0x80, /* $F456: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F457: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x44, /* $F458: NOTE_FS6 [tune0ch2] */
  0x80, /* $F459: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x80, /* $F45A: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBF, /* $F45B: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0xE3, /* $F45C: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x39, /* $F45D: NOTE_G5 [tune0ch2] */
  0xE7, /* $F45E: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F45F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F460: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xEB, /* $F461: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3A, /* $F462: NOTE_GS5 [tune0ch2] */
  0xE7, /* $F463: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F464: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBF, /* $F465: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0xE3, /* $F466: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F467: NOTE_A5 [tune0ch2] */
  0xE7, /* $F468: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F469: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F46A: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xEB, /* $F46B: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3C, /* $F46C: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F46D: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F46E: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F46F: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F470: NOTE_B5 [tune0ch2] */
  0xE7, /* $F471: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F472: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEB, /* $F473: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3D, /* $F474: NOTE_B5 [tune0ch2] */
  0xE3, /* $F475: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0xBF, /* $F476: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0x80, /* $F477: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3C, /* $F478: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F479: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x8F, /* $F47A: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F47B: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F47C: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEB, /* $F47D: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3E, /* $F47E: NOTE_C6 [tune0ch2] */
  0xE3, /* $F47F: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F480: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3C, /* $F481: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F482: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x8F, /* $F483: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F484: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F485: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEB, /* $F486: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3E, /* $F487: NOTE_C6 [tune0ch2] */
  0xE3, /* $F488: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F489: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3C, /* $F48A: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F48B: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x8F, /* $F48C: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F48D: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F48E: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEB, /* $F48F: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3E, /* $F490: NOTE_C6 [tune0ch2] */
  0xE3, /* $F491: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F492: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3C, /* $F493: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F494: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x8F, /* $F495: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F496: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F497: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEB, /* $F498: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3E, /* $F499: NOTE_C6 [tune0ch2] */
  0xE3, /* $F49A: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F49B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3C, /* $F49C: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F49D: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x8F, /* $F49E: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE7, /* $F49F: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x3E, /* $F4A0: NOTE_C6 [tune0ch2] */
  0xE3, /* $F4A1: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F4A2: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE7, /* $F4A3: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0xBE, /* $F4A4: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x3B, /* $F4A5: NOTE_A5 [tune0ch2] */
  0xBD, /* $F4A6: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F4A7: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F4A8: NOTE_B5 [tune0ch2] */
  0xE7, /* $F4A9: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F4AA: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F4AB: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F4AC: NOTE_B5 [tune0ch2] */
  0xEB, /* $F4AD: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F4AE: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F4AF: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F4B0: NOTE_A5 [tune0ch2] */
  0xF7, /* $F4B1: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x8F, /* $F4B2: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE7, /* $F4B3: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F4B4: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F4B5: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0xF7, /* $F4B6: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x39, /* $F4B7: NOTE_G5 [tune0ch2] */
  0xE3, /* $F4B8: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F4B9: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F4BA: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3D, /* $F4BB: NOTE_B5 [tune0ch2] */
  0x80, /* $F4BC: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F4BD: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3B, /* $F4BE: NOTE_A5 [tune0ch2] */
  0x80, /* $F4BF: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x39, /* $F4C0: NOTE_G5 [tune0ch2] */
  0x80, /* $F4C1: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBF, /* $F4C2: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0x37, /* $F4C3: NOTE_F5 [tune0ch2] */
  0xE7, /* $F4C4: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F4C5: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F4C6: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F4C7: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F4C8: NOTE_B5 [tune0ch2] */
  0xE7, /* $F4C9: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F4CA: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F4CB: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F4CC: NOTE_B5 [tune0ch2] */
  0xEB, /* $F4CD: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F4CE: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F4CF: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F4D0: NOTE_A5 [tune0ch2] */
  0xF7, /* $F4D1: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x8F, /* $F4D2: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F4D3: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0xBD, /* $F4D4: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x80, /* $F4D5: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x80, /* $F4D6: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F4D7: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F4D8: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE7, /* $F4D9: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F4DA: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F4DB: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0xC1, /* $F4DC: PCMD_SELECT_PITCH_OFFSET(9) [tune0ch2] */
  0x38, /* $F4DD: NOTE_FS5 [tune0ch2] */
  0xEF, /* $F4DE: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x80, /* $F4DF: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x87, /* $F4E0: PCMD_ADVANCE_PHRASE [tune0ch2] */
  0x8A, /* $F4E1: PCMD_SET_MIXER_BITS_HIGH3 [tune0ch1] */
  0xC0, /* $F4E2: -- unreached by tune 0/1 playback -- */
  0x81, /* $F4E3: -- unreached by tune 0/1 playback -- */
  0xD3, /* $F4E4: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F4E5: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4E6: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F4E7: -- unreached by tune 0/1 playback -- */
  0x39, /* $F4E8: -- unreached by tune 0/1 playback -- */
  0x39, /* $F4E9: -- unreached by tune 0/1 playback -- */
  0x39, /* $F4EA: -- unreached by tune 0/1 playback -- */
  0x37, /* $F4EB: -- unreached by tune 0/1 playback -- */
  0x36, /* $F4EC: -- unreached by tune 0/1 playback -- */
  0x37, /* $F4ED: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4EE: -- unreached by tune 0/1 playback -- */
  0x39, /* $F4EF: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4F0: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F4F1: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4F2: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F4F3: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4F4: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F4F5: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4F6: -- unreached by tune 0/1 playback -- */
  0x39, /* $F4F7: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4F8: -- unreached by tune 0/1 playback -- */
  0x34, /* $F4F9: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4FA: -- unreached by tune 0/1 playback -- */
  0x32, /* $F4FB: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4FC: -- unreached by tune 0/1 playback -- */
  0x31, /* $F4FD: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4FE: -- unreached by tune 0/1 playback -- */
  0x39, /* $F4FF: -- unreached by tune 0/1 playback -- */
  0xEB, /* $F500: -- unreached by tune 0/1 playback -- */
  0x80, /* $F501: -- unreached by tune 0/1 playback -- */
  0xE1, /* $F502: -- unreached by tune 0/1 playback -- */
  0x31, /* $F503: -- unreached by tune 0/1 playback -- */
  0x32, /* $F504: -- unreached by tune 0/1 playback -- */
  0x34, /* $F505: -- unreached by tune 0/1 playback -- */
  0x36, /* $F506: -- unreached by tune 0/1 playback -- */
  0x37, /* $F507: -- unreached by tune 0/1 playback -- */
  0x38, /* $F508: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F509: -- unreached by tune 0/1 playback -- */
  0x39, /* $F50A: -- unreached by tune 0/1 playback -- */
  0x80, /* $F50B: -- unreached by tune 0/1 playback -- */
  0x39, /* $F50C: -- unreached by tune 0/1 playback -- */
  0x39, /* $F50D: -- unreached by tune 0/1 playback -- */
  0x39, /* $F50E: -- unreached by tune 0/1 playback -- */
  0x37, /* $F50F: -- unreached by tune 0/1 playback -- */
  0x36, /* $F510: -- unreached by tune 0/1 playback -- */
  0x37, /* $F511: -- unreached by tune 0/1 playback -- */
  0x80, /* $F512: -- unreached by tune 0/1 playback -- */
  0x39, /* $F513: -- unreached by tune 0/1 playback -- */
  0x80, /* $F514: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F515: -- unreached by tune 0/1 playback -- */
  0x80, /* $F516: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F517: -- unreached by tune 0/1 playback -- */
  0x80, /* $F518: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F519: -- unreached by tune 0/1 playback -- */
  0x80, /* $F51A: -- unreached by tune 0/1 playback -- */
  0x39, /* $F51B: -- unreached by tune 0/1 playback -- */
  0x80, /* $F51C: -- unreached by tune 0/1 playback -- */
  0x34, /* $F51D: -- unreached by tune 0/1 playback -- */
  0x80, /* $F51E: -- unreached by tune 0/1 playback -- */
  0x32, /* $F51F: -- unreached by tune 0/1 playback -- */
  0x80, /* $F520: -- unreached by tune 0/1 playback -- */
  0x31, /* $F521: -- unreached by tune 0/1 playback -- */
  0x80, /* $F522: -- unreached by tune 0/1 playback -- */
  0x39, /* $F523: -- unreached by tune 0/1 playback -- */
  0xF7, /* $F524: -- unreached by tune 0/1 playback -- */
  0x80, /* $F525: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F526: -- unreached by tune 0/1 playback -- */
  0x80, /* $F527: -- unreached by tune 0/1 playback -- */
  0xB8, /* $F528: -- unreached by tune 0/1 playback -- */
  0x88, /* $F529: -- unreached by tune 0/1 playback -- */
  0x02, /* $F52A: -- unreached by tune 0/1 playback -- */
  0x03, /* $F52B: -- unreached by tune 0/1 playback -- */
  0x82, /* $F52C: -- unreached by tune 0/1 playback -- */
  0xEB, /* $F52D: -- unreached by tune 0/1 playback -- */
  0x40, /* $F52E: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F52F: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F530: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F531: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F532: -- unreached by tune 0/1 playback -- */
  0xFF, /* $F533: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F534: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F535: -- unreached by tune 0/1 playback -- */
  0x80, /* $F536: -- unreached by tune 0/1 playback -- */
  0xEB, /* $F537: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F538: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F539: -- unreached by tune 0/1 playback -- */
  0x36, /* $F53A: -- unreached by tune 0/1 playback -- */
  0x39, /* $F53B: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F53C: -- unreached by tune 0/1 playback -- */
  0x80, /* $F53D: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F53E: -- unreached by tune 0/1 playback -- */
  0x80, /* $F53F: -- unreached by tune 0/1 playback -- */
  0x39, /* $F540: -- unreached by tune 0/1 playback -- */
  0x80, /* $F541: -- unreached by tune 0/1 playback -- */
  0x37, /* $F542: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F543: -- unreached by tune 0/1 playback -- */
  0x80, /* $F544: -- unreached by tune 0/1 playback -- */
  0x81, /* $F545: -- unreached by tune 0/1 playback -- */
  0xC0, /* $F546: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F547: -- unreached by tune 0/1 playback -- */
  0x80, /* $F548: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F549: -- unreached by tune 0/1 playback -- */
  0x40, /* $F54A: -- unreached by tune 0/1 playback -- */
  0x40, /* $F54B: -- unreached by tune 0/1 playback -- */
  0x40, /* $F54C: -- unreached by tune 0/1 playback -- */
  0x42, /* $F54D: -- unreached by tune 0/1 playback -- */
  0x43, /* $F54E: -- unreached by tune 0/1 playback -- */
  0x43, /* $F54F: -- unreached by tune 0/1 playback -- */
  0x80, /* $F550: -- unreached by tune 0/1 playback -- */
  0x42, /* $F551: -- unreached by tune 0/1 playback -- */
  0x80, /* $F552: -- unreached by tune 0/1 playback -- */
  0x40, /* $F553: -- unreached by tune 0/1 playback -- */
  0x80, /* $F554: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F555: -- unreached by tune 0/1 playback -- */
  0x80, /* $F556: -- unreached by tune 0/1 playback -- */
  0x42, /* $F557: -- unreached by tune 0/1 playback -- */
  0x80, /* $F558: -- unreached by tune 0/1 playback -- */
  0x40, /* $F559: -- unreached by tune 0/1 playback -- */
  0x80, /* $F55A: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F55B: -- unreached by tune 0/1 playback -- */
  0x80, /* $F55C: -- unreached by tune 0/1 playback -- */
  0x40, /* $F55D: -- unreached by tune 0/1 playback -- */
  0x42, /* $F55E: -- unreached by tune 0/1 playback -- */
  0x40, /* $F55F: -- unreached by tune 0/1 playback -- */
  0xFF, /* $F560: -- unreached by tune 0/1 playback -- */
  0x80, /* $F561: -- unreached by tune 0/1 playback -- */
  0x80, /* $F562: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F563: -- unreached by tune 0/1 playback -- */
  0x39, /* $F564: -- unreached by tune 0/1 playback -- */
  0x38, /* $F565: -- unreached by tune 0/1 playback -- */
  0x37, /* $F566: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F567: -- unreached by tune 0/1 playback -- */
  0x36, /* $F568: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F569: -- unreached by tune 0/1 playback -- */
  0x37, /* $F56A: -- unreached by tune 0/1 playback -- */
  0x38, /* $F56B: -- unreached by tune 0/1 playback -- */
  0x39, /* $F56C: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F56D: -- unreached by tune 0/1 playback -- */
  0x3A, /* $F56E: -- unreached by tune 0/1 playback -- */
  0x39, /* $F56F: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F570: -- unreached by tune 0/1 playback -- */
  0x38, /* $F571: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F572: -- unreached by tune 0/1 playback -- */
  0x39, /* $F573: -- unreached by tune 0/1 playback -- */
  0x3A, /* $F574: -- unreached by tune 0/1 playback -- */
  0x80, /* $F575: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F576: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F577: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F578: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F579: -- unreached by tune 0/1 playback -- */
  0x39, /* $F57A: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F57B: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F57C: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F57D: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F57E: -- unreached by tune 0/1 playback -- */
  0x80, /* $F57F: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F580: -- unreached by tune 0/1 playback -- */
  0x80, /* $F581: -- unreached by tune 0/1 playback -- */
  0x3F, /* $F582: -- unreached by tune 0/1 playback -- */
  0x40, /* $F583: -- unreached by tune 0/1 playback -- */
  0x34, /* $F584: -- unreached by tune 0/1 playback -- */
  0x34, /* $F585: -- unreached by tune 0/1 playback -- */
  0x80, /* $F586: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F587: -- unreached by tune 0/1 playback -- */
  0x39, /* $F588: -- unreached by tune 0/1 playback -- */
  0x38, /* $F589: -- unreached by tune 0/1 playback -- */
  0x37, /* $F58A: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F58B: -- unreached by tune 0/1 playback -- */
  0x36, /* $F58C: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F58D: -- unreached by tune 0/1 playback -- */
  0x37, /* $F58E: -- unreached by tune 0/1 playback -- */
  0x38, /* $F58F: -- unreached by tune 0/1 playback -- */
  0x39, /* $F590: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F591: -- unreached by tune 0/1 playback -- */
  0x3A, /* $F592: -- unreached by tune 0/1 playback -- */
  0x39, /* $F593: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F594: -- unreached by tune 0/1 playback -- */
  0x38, /* $F595: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F596: -- unreached by tune 0/1 playback -- */
  0x39, /* $F597: -- unreached by tune 0/1 playback -- */
  0x3A, /* $F598: -- unreached by tune 0/1 playback -- */
  0x80, /* $F599: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F59A: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F59B: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F59C: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F59D: -- unreached by tune 0/1 playback -- */
  0x39, /* $F59E: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F59F: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F5A0: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F5A1: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F5A2: -- unreached by tune 0/1 playback -- */
  0xEB, /* $F5A3: -- unreached by tune 0/1 playback -- */
  0x8F, /* $F5A4: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5A5: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5A6: -- unreached by tune 0/1 playback -- */
  0xFF, /* $F5A7: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5A8: -- unreached by tune 0/1 playback -- */
  0xEF, /* $F5A9: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5AA: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5AB: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5AC: -- unreached by tune 0/1 playback -- */
  0x39, /* $F5AD: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5AE: -- unreached by tune 0/1 playback -- */
  0x45, /* $F5AF: -- unreached by tune 0/1 playback -- */
  0x42, /* $F5B0: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5B1: -- unreached by tune 0/1 playback -- */
  0x39, /* $F5B2: -- unreached by tune 0/1 playback -- */
  0xEF, /* $F5B3: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F5B4: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5B5: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5B6: -- unreached by tune 0/1 playback -- */
  0xEF, /* $F5B7: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5B8: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5B9: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5BA: -- unreached by tune 0/1 playback -- */
  0x39, /* $F5BB: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5BC: -- unreached by tune 0/1 playback -- */
  0x45, /* $F5BD: -- unreached by tune 0/1 playback -- */
  0x42, /* $F5BE: -- unreached by tune 0/1 playback -- */
  0xE1, /* $F5BF: -- unreached by tune 0/1 playback -- */
  0x31, /* $F5C0: -- unreached by tune 0/1 playback -- */
  0x32, /* $F5C1: -- unreached by tune 0/1 playback -- */
  0x34, /* $F5C2: -- unreached by tune 0/1 playback -- */
  0x36, /* $F5C3: -- unreached by tune 0/1 playback -- */
  0x37, /* $F5C4: -- unreached by tune 0/1 playback -- */
  0x38, /* $F5C5: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F5C6: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5C7: -- unreached by tune 0/1 playback -- */
  0xB8, /* $F5C8: -- unreached by tune 0/1 playback -- */
  0x88, /* $F5C9: -- unreached by tune 0/1 playback -- */
  0x02, /* $F5CA: -- unreached by tune 0/1 playback -- */
  0x03, /* $F5CB: -- unreached by tune 0/1 playback -- */
  0x82, /* $F5CC: -- unreached by tune 0/1 playback -- */
  0xEB, /* $F5CD: -- unreached by tune 0/1 playback -- */
  0x40, /* $F5CE: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5CF: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5D0: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F5D1: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5D2: -- unreached by tune 0/1 playback -- */
  0xFF, /* $F5D3: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5D4: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F5D5: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5D6: -- unreached by tune 0/1 playback -- */
  0xEB, /* $F5D7: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5D8: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5D9: -- unreached by tune 0/1 playback -- */
  0x36, /* $F5DA: -- unreached by tune 0/1 playback -- */
  0x39, /* $F5DB: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F5DC: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5DD: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F5DE: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5DF: -- unreached by tune 0/1 playback -- */
  0x39, /* $F5E0: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5E1: -- unreached by tune 0/1 playback -- */
  0x37, /* $F5E2: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F5E3: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5E4: -- unreached by tune 0/1 playback -- */
  0x81, /* $F5E5: -- unreached by tune 0/1 playback -- */
  0xC0, /* $F5E6: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F5E7: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5E8: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5E9: -- unreached by tune 0/1 playback -- */
  0x40, /* $F5EA: -- unreached by tune 0/1 playback -- */
  0x40, /* $F5EB: -- unreached by tune 0/1 playback -- */
  0x40, /* $F5EC: -- unreached by tune 0/1 playback -- */
  0x42, /* $F5ED: -- unreached by tune 0/1 playback -- */
  0x43, /* $F5EE: -- unreached by tune 0/1 playback -- */
  0x43, /* $F5EF: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5F0: -- unreached by tune 0/1 playback -- */
  0x42, /* $F5F1: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5F2: -- unreached by tune 0/1 playback -- */
  0x40, /* $F5F3: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5F4: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5F5: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5F6: -- unreached by tune 0/1 playback -- */
  0x42, /* $F5F7: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5F8: -- unreached by tune 0/1 playback -- */
  0x40, /* $F5F9: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5FA: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5FB: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5FC: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F5FD: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F5FE: -- unreached by tune 0/1 playback -- */
  0x39, /* $F5FF: -- unreached by tune 0/1 playback -- */
  0x8E, /* $F600: -- unreached by tune 0/1 playback -- */
  /* Conv: extension past the original $F600 cut -- ch1's real command
   * stream runs on into what the original transcription window called
   * "$F601-$F6DE" (tune 1's own header block), because bank3.bin packs both
   * tunes' data back-to-back with no tune0/tune1 boundary in the byte
   * stream itself; only the phrase-table entries that resolve_phrase_addr
   * is asked to follow define which tune "owns" which bytes. Duplicated
   * verbatim from bank3.bin here so ch1's PHRASE_TABLE_RESET loop
   * ($F25C-$F263) reaches its real PCMD_ADVANCE_PHRASE at $F642 instead of
   * hitting acp_read_byte's finite-array wrap guard early and restarting
   * from the header ($F26B) 350+ bytes too soon -- see the sync-drift fix.
   */
  0x0D, /* $F601: NOTE(0x0d) [tune0ch1] */
  0xF6, /* $F602: PCMD_SET_ROW_WAIT(23) [tune0ch1] */
  0x00, /* $F603: NOTE(0x00) [tune0ch1] */
  0x00, /* $F604: NOTE(0x00) [tune0ch1] */
  0x2E, /* $F605: NOTE(0x2e) [tune0ch1] */
  0xF6, /* $F606: PCMD_SET_ROW_WAIT(23) [tune0ch1] */
  0x00, /* $F607: NOTE(0x00) [tune0ch1] */
  0x00, /* $F608: NOTE(0x00) [tune0ch1] */
  0x43, /* $F609: NOTE(0x43) [tune0ch1] */
  0xF6, /* $F60A: PCMD_SET_ROW_WAIT(23) [tune0ch1] */
  0x00, /* $F60B: NOTE(0x00) [tune0ch1] */
  0x00, /* $F60C: NOTE(0x00) [tune0ch1] */
  0x8A, /* $F60D: PCMD_SET_MIXER_BITS_HIGH3 [tune0ch1] */
  0xC0, /* $F60E: PCMD_SELECT_PITCH_OFFSET(8) [tune0ch1] */
  0x81, /* $F60F: PCMD_VIBRATO_OFF [tune0ch1] */
  0xD4, /* $F610: PCMD_SELECT_ENVELOPE_SHAPE(4) [tune0ch1] */
  0x91, /* $F611: PCMD_UNMUTE_CHANNEL [tune0ch1] */
  0xE1, /* $F612: PCMD_SET_ROW_WAIT(2) [tune0ch1] */
  0x3C, /* $F613: NOTE(0x3c) [tune0ch1] */
  0x3C, /* $F614: NOTE(0x3c) [tune0ch1] */
  0x3C, /* $F615: NOTE(0x3c) [tune0ch1] */
  0xE3, /* $F616: PCMD_SET_ROW_WAIT(4) [tune0ch1] */
  0x3B, /* $F617: NOTE(0x3b) [tune0ch1] */
  0x37, /* $F618: NOTE(0x37) [tune0ch1] */
  0x3A, /* $F619: NOTE(0x3a) [tune0ch1] */
  0x3A, /* $F61A: NOTE(0x3a) [tune0ch1] */
  0xE1, /* $F61B: PCMD_SET_ROW_WAIT(2) [tune0ch1] */
  0x3A, /* $F61C: NOTE(0x3a) [tune0ch1] */
  0xE3, /* $F61D: PCMD_SET_ROW_WAIT(4) [tune0ch1] */
  0x39, /* $F61E: NOTE(0x39) [tune0ch1] */
  0x35, /* $F61F: NOTE(0x35) [tune0ch1] */
  0xE1, /* $F620: PCMD_SET_ROW_WAIT(2) [tune0ch1] */
  0x38, /* $F621: NOTE(0x38) [tune0ch1] */
  0x38, /* $F622: NOTE(0x38) [tune0ch1] */
  0x38, /* $F623: NOTE(0x38) [tune0ch1] */
  0xE3, /* $F624: PCMD_SET_ROW_WAIT(4) [tune0ch1] */
  0x37, /* $F625: NOTE(0x37) [tune0ch1] */
  0x33, /* $F626: NOTE(0x33) [tune0ch1] */
  0xE7, /* $F627: PCMD_SET_ROW_WAIT(8) [tune0ch1] */
  0x35, /* $F628: NOTE(0x35) [tune0ch1] */
  0xE1, /* $F629: PCMD_SET_ROW_WAIT(2) [tune0ch1] */
  0x80, /* $F62A: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch1] */
  0xE7, /* $F62B: PCMD_SET_ROW_WAIT(8) [tune0ch1] */
  0x80, /* $F62C: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch1] */
  0x8E, /* $F62D: unmapped no-op [tune0ch1] */
  0x8A, /* $F62E: PCMD_SET_MIXER_BITS_HIGH3 [tune0ch1] */
  0x90, /* $F62F: PCMD_MUTE_CHANNEL [tune0ch1] */
  0xD3, /* $F630: PCMD_SELECT_ENVELOPE_SHAPE(3) [tune0ch1] */
  0xC2, /* $F631: PCMD_SELECT_PITCH_OFFSET(10) [tune0ch1] */
  0x81, /* $F632: PCMD_VIBRATO_OFF [tune0ch1] */
  0xEB, /* $F633: PCMD_SET_ROW_WAIT(12) [tune0ch1] */
  0x18, /* $F634: NOTE(0x18) [tune0ch1] */
  0xE3, /* $F635: PCMD_SET_ROW_WAIT(4) [tune0ch1] */
  0x80, /* $F636: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch1] */
  0xEB, /* $F637: PCMD_SET_ROW_WAIT(12) [tune0ch1] */
  0x16, /* $F638: NOTE(0x16) [tune0ch1] */
  0xE3, /* $F639: PCMD_SET_ROW_WAIT(4) [tune0ch1] */
  0x80, /* $F63A: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch1] */
  0xEB, /* $F63B: PCMD_SET_ROW_WAIT(12) [tune0ch1] */
  0x14, /* $F63C: NOTE(0x14) [tune0ch1] */
  0xE3, /* $F63D: PCMD_SET_ROW_WAIT(4) [tune0ch1] */
  0x80, /* $F63E: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch1] */
  0xE7, /* $F63F: PCMD_SET_ROW_WAIT(8) [tune0ch1] */
  0x11, /* $F640: NOTE(0x11) [tune0ch1] */
  0x80, /* $F641: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch1] */
  0x87, /* $F642: PCMD_ADVANCE_PHRASE [tune0ch1] */
};

/** $F601-$F6DE: title_tune1_data */
const u8 title_tune1_data[222] = {
  0x0D, /* $F601: HEADER_PATTERN_PTR [tune1ch0] */
  0xF6, /* $F602: (high byte) */
  0x00, /* $F603: PHRASE_TABLE_WORD */
  0x00, /* $F604: (high byte) */
  0x2E, /* $F605: HEADER_PATTERN_PTR [tune1ch1] */
  0xF6, /* $F606: (high byte) */
  0x00, /* $F607: PHRASE_TABLE_WORD */
  0x00, /* $F608: (high byte) */
  0x43, /* $F609: HEADER_PATTERN_PTR [tune1ch2] */
  0xF6, /* $F60A: (high byte) */
  0x00, /* $F60B: -- unreached by tune 0/1 playback -- */
  0x00, /* $F60C: -- unreached by tune 0/1 playback -- */
  0x8A, /* $F60D: PCMD_SET_MIXER_BITS_HIGH3 [tune1ch0] */
  0xC0, /* $F60E: PCMD_SELECT_PITCH_OFFSET(8) [tune1ch0] */
  0x81, /* $F60F: PCMD_VIBRATO_OFF [tune1ch0] */
  0xD4, /* $F610: PCMD_SELECT_ENVELOPE_SHAPE(4) [tune1ch0] */
  0x91, /* $F611: PCMD_UNMUTE_CHANNEL [tune1ch0] */
  0xE1, /* $F612: PCMD_SET_ROW_WAIT(2) [tune1ch0] */
  0x3C, /* $F613: NOTE_AS5 [tune1ch0] */
  0x3C, /* $F614: NOTE_AS5 [tune1ch0] */
  0x3C, /* $F615: NOTE_AS5 [tune1ch0] */
  0xE3, /* $F616: PCMD_SET_ROW_WAIT(4) [tune1ch0] */
  0x3B, /* $F617: NOTE_A5 [tune1ch0] */
  0x37, /* $F618: NOTE_F5 [tune1ch0] */
  0x3A, /* $F619: NOTE_GS5 [tune1ch0] */
  0x3A, /* $F61A: NOTE_GS5 [tune1ch0] */
  0xE1, /* $F61B: PCMD_SET_ROW_WAIT(2) [tune1ch0] */
  0x3A, /* $F61C: NOTE_GS5 [tune1ch0] */
  0xE3, /* $F61D: PCMD_SET_ROW_WAIT(4) [tune1ch0] */
  0x39, /* $F61E: NOTE_G5 [tune1ch0] */
  0x35, /* $F61F: NOTE_DS5 [tune1ch0] */
  0xE1, /* $F620: PCMD_SET_ROW_WAIT(2) [tune1ch0] */
  0x38, /* $F621: NOTE_FS5 [tune1ch0] */
  0x38, /* $F622: NOTE_FS5 [tune1ch0] */
  0x38, /* $F623: NOTE_FS5 [tune1ch0] */
  0xE3, /* $F624: PCMD_SET_ROW_WAIT(4) [tune1ch0] */
  0x37, /* $F625: NOTE_F5 [tune1ch0] */
  0x33, /* $F626: NOTE_CS5 [tune1ch0] */
  0xE7, /* $F627: PCMD_SET_ROW_WAIT(8) [tune1ch0] */
  0x35, /* $F628: NOTE_DS5 [tune1ch0] */
  0xE1, /* $F629: PCMD_SET_ROW_WAIT(2) [tune1ch0] */
  0x80, /* $F62A: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch0] */
  0xE7, /* $F62B: PCMD_SET_ROW_WAIT(8) [tune1ch0] */
  0x80, /* $F62C: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch0] */
  0x8E, /* $F62D: PCMD_UNUSED_8E [tune1ch0] */
  0x8A, /* $F62E: PCMD_SET_MIXER_BITS_HIGH3 [tune1ch0] */
  0x90, /* $F62F: PCMD_MUTE_CHANNEL [tune1ch0] */
  0xD3, /* $F630: PCMD_SELECT_ENVELOPE_SHAPE(3) [tune1ch0] */
  0xC2, /* $F631: PCMD_SELECT_PITCH_OFFSET(10) [tune1ch0] */
  0x81, /* $F632: PCMD_VIBRATO_OFF [tune1ch0] */
  0xEB, /* $F633: PCMD_SET_ROW_WAIT(12) [tune1ch0] */
  0x18, /* $F634: NOTE_AS2 [tune1ch0] */
  0xE3, /* $F635: PCMD_SET_ROW_WAIT(4) [tune1ch0] */
  0x80, /* $F636: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch0] */
  0xEB, /* $F637: PCMD_SET_ROW_WAIT(12) [tune1ch0] */
  0x16, /* $F638: NOTE_GS2 [tune1ch0] */
  0xE3, /* $F639: PCMD_SET_ROW_WAIT(4) [tune1ch0] */
  0x80, /* $F63A: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch0] */
  0xEB, /* $F63B: PCMD_SET_ROW_WAIT(12) [tune1ch0] */
  0x14, /* $F63C: NOTE_FS2 [tune1ch0] */
  0xE3, /* $F63D: PCMD_SET_ROW_WAIT(4) [tune1ch0] */
  0x80, /* $F63E: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch0] */
  0xE7, /* $F63F: PCMD_SET_ROW_WAIT(8) [tune1ch0] */
  0x11, /* $F640: NOTE_DS2 [tune1ch0] */
  0x80, /* $F641: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch0] */
  0x87, /* $F642: PCMD_ADVANCE_PHRASE [tune1ch0] */
  0xD5, /* $F643: PCMD_SELECT_ENVELOPE_SHAPE(5) [tune1ch2] */
  0x8A, /* $F644: PCMD_SET_MIXER_BITS_HIGH3 [tune1ch2] */
  0xB8, /* $F645: PCMD_SELECT_PITCH_OFFSET(0) [tune1ch2] */
  0x81, /* $F646: PCMD_VIBRATO_OFF [tune1ch2] */
  0x90, /* $F647: PCMD_MUTE_CHANNEL [tune1ch2] */
  0xE1, /* $F648: PCMD_SET_ROW_WAIT(2) [tune1ch2] */
  0x4D, /* $F649: NOTE_DS7 [tune1ch2] */
  0x4C, /* $F64A: NOTE_D7 [tune1ch2] */
  0x4B, /* $F64B: NOTE_CS7 [tune1ch2] */
  0x4A, /* $F64C: NOTE_C7 [tune1ch2] */
  0x49, /* $F64D: NOTE_B6 [tune1ch2] */
  0x48, /* $F64E: NOTE_AS6 [tune1ch2] */
  0x47, /* $F64F: NOTE_A6 [tune1ch2] */
  0x46, /* $F650: NOTE_GS6 [tune1ch2] */
  0x45, /* $F651: NOTE_G6 [tune1ch2] */
  0x44, /* $F652: NOTE_FS6 [tune1ch2] */
  0x43, /* $F653: NOTE_F6 [tune1ch2] */
  0x42, /* $F654: NOTE_E6 [tune1ch2] */
  0x41, /* $F655: NOTE_DS6 [tune1ch2] */
  0x40, /* $F656: NOTE_D6 [tune1ch2] */
  0x3F, /* $F657: NOTE_CS6 [tune1ch2] */
  0x3E, /* $F658: NOTE_C6 [tune1ch2] */
  0x3D, /* $F659: NOTE_B5 [tune1ch2] */
  0x3C, /* $F65A: NOTE_AS5 [tune1ch2] */
  0x3B, /* $F65B: NOTE_A5 [tune1ch2] */
  0x3A, /* $F65C: NOTE_GS5 [tune1ch2] */
  0x39, /* $F65D: NOTE_G5 [tune1ch2] */
  0x38, /* $F65E: NOTE_FS5 [tune1ch2] */
  0x37, /* $F65F: NOTE_F5 [tune1ch2] */
  0x36, /* $F660: NOTE_E5 [tune1ch2] */
  0xEF, /* $F661: PCMD_SET_ROW_WAIT(16) [tune1ch2] */
  0x8B, /* $F662: PCMD_SET_MIXER_BITS_LOW3 [tune1ch2] */
  0xD6, /* $F663: PCMD_SELECT_ENVELOPE_SHAPE(6) [tune1ch2] */
  0x00, /* $F664: NOTE_AS0 [tune1ch2] */
  0x8E, /* $F665: PCMD_UNUSED_8E [tune1ch2] */
  0x72, /* $F666: NOTE_RAW_0x72(OUT_OF_RANGE) [tune1ch2] */
  0xF6, /* $F667: PCMD_SET_ROW_WAIT(23) [tune1ch2] */
  0x00, /* $F668: NOTE_AS0 [tune1ch2] */
  0x00, /* $F669: NOTE_AS0 [tune1ch2] */
  0xB1, /* $F66A: PCMD_SET_TEMPO(2) [tune1ch2] */
  0xF6, /* $F66B: PCMD_SET_ROW_WAIT(23) [tune1ch2] */
  0x00, /* $F66C: NOTE_AS0 [tune1ch2] */
  0x00, /* $F66D: NOTE_AS0 [tune1ch2] */
  0xEB, /* $F66E: PCMD_SET_ROW_WAIT(12) [tune1ch2] */
  0xF6, /* $F66F: PCMD_SET_ROW_WAIT(23) [tune1ch2] */
  0x00, /* $F670: NOTE_AS0 [tune1ch2] */
  0x00, /* $F671: NOTE_AS0 [tune1ch2] */
  0x8A, /* $F672: PCMD_SET_MIXER_BITS_HIGH3 [tune1ch2] */
  0x81, /* $F673: PCMD_VIBRATO_OFF [tune1ch2] */
  0xC0, /* $F674: PCMD_SELECT_PITCH_OFFSET(8) [tune1ch2] */
  0x91, /* $F675: PCMD_UNMUTE_CHANNEL [tune1ch2] */
  0xD4, /* $F676: PCMD_SELECT_ENVELOPE_SHAPE(4) [tune1ch2] */
  0xE7, /* $F677: PCMD_SET_ROW_WAIT(8) [tune1ch2] */
  0x37, /* $F678: NOTE_F5 [tune1ch2] */
  0xE3, /* $F679: PCMD_SET_ROW_WAIT(4) [tune1ch2] */
  0x39, /* $F67A: NOTE_G5 [tune1ch2] */
  0x37, /* $F67B: NOTE_F5 [tune1ch2] */
  0x36, /* $F67C: NOTE_E5 [tune1ch2] */
  0x34, /* $F67D: NOTE_D5 [tune1ch2] */
  0x32, /* $F67E: NOTE_C5 [tune1ch2] */
  0x80, /* $F67F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch2] */
  0xE7, /* $F680: PCMD_SET_ROW_WAIT(8) [tune1ch2] */
  0x34, /* $F681: NOTE_D5 [tune1ch2] */
  0xE3, /* $F682: PCMD_SET_ROW_WAIT(4) [tune1ch2] */
  0x37, /* $F683: NOTE_F5 [tune1ch2] */
  0x36, /* $F684: NOTE_E5 [tune1ch2] */
  0x34, /* $F685: NOTE_D5 [tune1ch2] */
  0x31, /* $F686: NOTE_B4 [tune1ch2] */
  0x2D, /* $F687: NOTE_G4 [tune1ch2] */
  0x80, /* $F688: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch2] */
  0xE7, /* $F689: PCMD_SET_ROW_WAIT(8) [tune1ch2] */
  0x2B, /* $F68A: NOTE_F4 [tune1ch2] */
  0xE3, /* $F68B: PCMD_SET_ROW_WAIT(4) [tune1ch2] */
  0x2D, /* $F68C: NOTE_G4 [tune1ch2] */
  0x2F, /* $F68D: NOTE_A4 [tune1ch2] */
  0x31, /* $F68E: NOTE_B4 [tune1ch2] */
  0x33, /* $F68F: NOTE_CS5 [tune1ch2] */
  0x35, /* $F690: NOTE_DS5 [tune1ch2] */
  0x37, /* $F691: NOTE_F5 [tune1ch2] */
  0x39, /* $F692: NOTE_G5 [tune1ch2] */
  0x33, /* $F693: NOTE_CS5 [tune1ch2] */
  0x35, /* $F694: NOTE_DS5 [tune1ch2] */
  0x37, /* $F695: NOTE_F5 [tune1ch2] */
  0x39, /* $F696: NOTE_G5 [tune1ch2] */
  0x35, /* $F697: NOTE_DS5 [tune1ch2] */
  0x37, /* $F698: NOTE_F5 [tune1ch2] */
  0x39, /* $F699: NOTE_G5 [tune1ch2] */
  0x3B, /* $F69A: NOTE_A5 [tune1ch2] */
  0x37, /* $F69B: NOTE_F5 [tune1ch2] */
  0x39, /* $F69C: NOTE_G5 [tune1ch2] */
  0x3B, /* $F69D: NOTE_A5 [tune1ch2] */
  0x3D, /* $F69E: NOTE_B5 [tune1ch2] */
  0x39, /* $F69F: NOTE_G5 [tune1ch2] */
  0x3B, /* $F6A0: NOTE_A5 [tune1ch2] */
  0x3D, /* $F6A1: NOTE_B5 [tune1ch2] */
  0x3F, /* $F6A2: NOTE_CS6 [tune1ch2] */
  0x3B, /* $F6A3: NOTE_A5 [tune1ch2] */
  0x3D, /* $F6A4: NOTE_B5 [tune1ch2] */
  0x3F, /* $F6A5: NOTE_CS6 [tune1ch2] */
  0x41, /* $F6A6: NOTE_DS6 [tune1ch2] */
  0xE1, /* $F6A7: PCMD_SET_ROW_WAIT(2) [tune1ch2] */
  0x43, /* $F6A8: NOTE_F6 [tune1ch2] */
  0x41, /* $F6A9: NOTE_DS6 [tune1ch2] */
  0x3F, /* $F6AA: NOTE_CS6 [tune1ch2] */
  0x3D, /* $F6AB: NOTE_B5 [tune1ch2] */
  0x3B, /* $F6AC: NOTE_A5 [tune1ch2] */
  0x39, /* $F6AD: NOTE_G5 [tune1ch2] */
  0x37, /* $F6AE: NOTE_F5 [tune1ch2] */
  0x35, /* $F6AF: NOTE_DS5 [tune1ch2] */
  0x8E, /* $F6B0: PCMD_UNUSED_8E [tune1ch2] */
  0x8A, /* $F6B1: PCMD_SET_MIXER_BITS_HIGH3 [tune1ch2] */
  0x90, /* $F6B2: PCMD_MUTE_CHANNEL [tune1ch2] */
  0xD0, /* $F6B3: PCMD_SELECT_ENVELOPE_SHAPE(0) [tune1ch2] */
  0xB9, /* $F6B4: PCMD_SELECT_PITCH_OFFSET(1) [tune1ch2] */
  0x81, /* $F6B5: PCMD_VIBRATO_OFF [tune1ch2] */
  0xE3, /* $F6B6: PCMD_SET_ROW_WAIT(4) [tune1ch2] */
  0x21, /* $F6B7: NOTE_G3 [tune1ch2] */
  0x15, /* $F6B8: NOTE_G2 [tune1ch2] */
  0x21, /* $F6B9: NOTE_G3 [tune1ch2] */
  0x15, /* $F6BA: NOTE_G2 [tune1ch2] */
  0x21, /* $F6BB: NOTE_G3 [tune1ch2] */
  0x15, /* $F6BC: NOTE_G2 [tune1ch2] */
  0x21, /* $F6BD: NOTE_G3 [tune1ch2] */
  0x15, /* $F6BE: NOTE_G2 [tune1ch2] */
  0x21, /* $F6BF: NOTE_G3 [tune1ch2] */
  0x15, /* $F6C0: NOTE_G2 [tune1ch2] */
  0x21, /* $F6C1: NOTE_G3 [tune1ch2] */
  0x15, /* $F6C2: NOTE_G2 [tune1ch2] */
  0x21, /* $F6C3: NOTE_G3 [tune1ch2] */
  0x15, /* $F6C4: NOTE_G2 [tune1ch2] */
  0x21, /* $F6C5: NOTE_G3 [tune1ch2] */
  0x15, /* $F6C6: NOTE_G2 [tune1ch2] */
  0x21, /* $F6C7: NOTE_G3 [tune1ch2] */
  0x15, /* $F6C8: NOTE_G2 [tune1ch2] */
  0x21, /* $F6C9: NOTE_G3 [tune1ch2] */
  0x15, /* $F6CA: NOTE_G2 [tune1ch2] */
  0x21, /* $F6CB: NOTE_G3 [tune1ch2] */
  0x15, /* $F6CC: NOTE_G2 [tune1ch2] */
  0x21, /* $F6CD: NOTE_G3 [tune1ch2] */
  0x15, /* $F6CE: NOTE_G2 [tune1ch2] */
  0x21, /* $F6CF: NOTE_G3 [tune1ch2] */
  0x15, /* $F6D0: NOTE_G2 [tune1ch2] */
  0x21, /* $F6D1: NOTE_G3 [tune1ch2] */
  0x12, /* $F6D2: NOTE_E2 [tune1ch2] */
  0x1E, /* $F6D3: NOTE_E3 [tune1ch2] */
  0x15, /* $F6D4: NOTE_G2 [tune1ch2] */
  0x21, /* $F6D5: NOTE_G3 [tune1ch2] */
  0x14, /* $F6D6: NOTE_FS2 [tune1ch2] */
  0x20, /* $F6D7: NOTE_FS3 [tune1ch2] */
  0x15, /* $F6D8: NOTE_G2 [tune1ch2] */
  0x21, /* $F6D9: NOTE_G3 [tune1ch2] */
  0x12, /* $F6DA: NOTE_E2 [tune1ch2] */
  0x1E, /* $F6DB: NOTE_E3 [tune1ch2] */
  0x15, /* $F6DC: NOTE_G2 [tune1ch2] */
  0x21, /* $F6DD: NOTE_G3 [tune1ch2] */
  0x14, /* $F6DE: NOTE_FS2 [tune1ch2] */
};

/**
 * $FC29-$FD96 -- 128K control-select, key-redefinition and hidden test-mode
 * screen text, printed via print_string/print_character. Unlike the
 * messages_* lists above, print_string does not walk an end-marker-terminated
 * list: each 0x00 below terminates whichever call is in progress, so this one
 * data block actually holds four independent entry points, each reached via
 * its own literal HL constant in the original:
 *   offset   0 ($FC29): control-select screen -- wired into
 *                       omd_redraw_and_poll below.
 *   offset 114 ($FC9B): key-redefinition screen, header + GEAR/ACCELERATE/
 *                       BRAKE -- not yet wired up (needs redefine_keys_screen,
 *                       $FEA9).
 *   offset 160 ($FCC9): key-redefinition screen continued, LEFT/RIGHT/QUIT/
 *                       PAUSE/TURBO -- not yet wired up (see above).
 *   offset 199 ($FCF0): hidden test-mode screen -- not yet wired up (needs a
 *                       128K test-mode driver, $C06E).
 * "P1."-"P4."/"P5." labels in the skool comments are missing their leading
 * "P" in the actual data (confirmed byte-for-byte against the skool) --
 * presumably drawn as a separate fixed graphic; transcribed faithfully as-is.
 */
const u8 options_menu_text[366] = {
  // $FC29 (offset 0): control-select screen
  attribute_RED_OVER_BLACK,
  TWOBYTES(0x484A),
  'E', 'N', 'T', 'E', 'R', ' ', 'O', 'P', 'T', 'I', 'O', 'N' | EOS,
  attribute_CYAN_OVER_BLACK,
  TWOBYTES(0x48C6),
  '1', '.', ' ', 'S', 'I', 'N', 'C', 'L', 'A', 'I', 'R', ' ', 'J', 'O', 'Y', 'S', 'T', 'I', 'C', 'K' | EOS,
  attribute_CYAN_OVER_BLACK,
  TWOBYTES(0x5006),
  '2', '.', ' ', 'C', 'U', 'R', 'S', 'O', 'R', ' ', 'J', 'O', 'Y', 'S', 'T', 'I', 'C', 'K' | EOS,
  attribute_CYAN_OVER_BLACK,
  TWOBYTES(0x5046),
  '3', '.', ' ', 'K', 'E', 'M', 'P', 'S', 'T', 'O', 'N', ' ', 'J', 'O', 'Y', 'S', 'T', 'I', 'C', 'K' | EOS,
  attribute_CYAN_OVER_BLACK,
  TWOBYTES(0x5086),
  '4', '.', ' ', 'K', 'E', 'Y', 'B', 'O', 'A', 'R', 'D' | EOS,
  attribute_CYAN_OVER_BLACK,
  TWOBYTES(0x50C6),
  '5', '.', ' ', 'D', 'E', 'F', 'I', 'N', 'E', ' ', 'K', 'E', 'Y', 'S' | EOS,
  0, // terminator ($FC9A)

  // $FC9B (offset 114): key-redefinition screen, header + first 3 labels
  attribute_RED_OVER_BLACK,
  TWOBYTES(0x4849),
  'R', 'E', 'D', 'E', 'F', 'I', 'N', 'E', ' ', ' ', 'K', 'E', 'Y', 'S' | EOS,
  0xC6, // attribute_BRIGHT_YELLOW_OVER_BLACK + single height bit
  TWOBYTES(0x48C9),
  'G', 'E', 'A', 'R' | EOS,
  0xC6,
  TWOBYTES(0x48E9),
  'A', 'C', 'C', 'E', 'L', 'E', 'R', 'A', 'T', 'E' | EOS,
  0xC6,
  TWOBYTES(0x5009),
  'B', 'R', 'A', 'K', 'E' | EOS,
  0, // terminator ($FCC8)

  // $FCC9 (offset 160): key-redefinition screen, remaining 5 labels
  0xC6,
  TWOBYTES(0x5029),
  'L', 'E', 'F', 'T' | EOS,
  0xC6,
  TWOBYTES(0x5049),
  'R', 'I', 'G', 'H', 'T' | EOS,
  0xC4, // attribute_BRIGHT_GREEN_OVER_BLACK + single height bit
  TWOBYTES(0x5089),
  'Q', 'U', 'I', 'T' | EOS,
  0xC4,
  TWOBYTES(0x50A9),
  'P', 'A', 'U', 'S', 'E' | EOS,
  0xC4,
  TWOBYTES(0x50C9),
  'T', 'U', 'R', 'B', 'O' | EOS,
  0, // terminator ($FCEF)

  // $FCF0 (offset 199): hidden test-mode screen
  0xC1, // attribute_BRIGHT_BLUE_OVER_BLACK + single height bit
  TWOBYTES(0x4000),
  'T', 'E', 'S', 'T' | EOS,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0x4826),
  'C', 'H', 'A', 'S', 'E', ' ', 'H', '.', 'Q', '.', ' ', 'T', 'E', 'S', 'T', ' ', 'M', 'O', 'D', 'E' | EOS,
  0xC5, // attribute_BRIGHT_CYAN_OVER_BLACK + single height bit
  TWOBYTES(0x48A2),
  'T', 'I', 'T', 'L', 'E', ' ', 'S', 'C', 'R', 'E', 'E', 'N' | EOS,
  0xC3, // attribute_BRIGHT_MAGENTA_OVER_BLACK + single height bit
  TWOBYTES(0x48E2),
  '1', ' ', 'T', 'O', ' ', '5', '.', ' ', 'L', 'O', 'G', 'O', ' ', 'A', 'N', 'I', 'M', 'A', 'T', 'I', 'O', 'N' | EOS,
  0xC3,
  TWOBYTES(0x5007),
  '6', '.', ' ', 'S', 'C', 'O', 'R', 'E', ' ', 'E', 'N', 'T', 'R', 'Y' | EOS,
  0xC5,
  TWOBYTES(0x5042),
  'I', 'N', ' ', 'G', 'A', 'M', 'E' | EOS,
  0xC4,
  TWOBYTES(0x5087),
  '1', '.', ' ', 'R', 'E', 'S', 'T', 'A', 'R', 'T', ' ', 'L', 'E', 'V', 'E', 'L' | EOS,
  0xC4,
  TWOBYTES(0x50A7),
  '2', '.', ' ', 'N', 'E', 'X', 'T', ' ', 'L', 'E', 'V', 'E', 'L' | EOS,
  0xC4,
  TWOBYTES(0x50C7),
  '3', '.', ' ', 'E', 'N', 'D', ' ', 'S', 'C', 'R', 'E', 'E', 'N' | EOS,
  0xC4,
  TWOBYTES(0x50E7),
  '4', '.', ' ', 'E', 'X', 'T', 'R', 'A', ' ', 'C', 'R', 'E', 'D', 'I', 'T' | EOS,
  0 // terminator / pad byte ($FD96)
};
// clang-format on

/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */

const u8 high_score_rank_suffixes[HIGH_SCORE_TABLE_ROWS][5] = {
  "1ST  ", "2ND  ", "3RD  ", "4TH  ", "5TH  ",
  "6TH  ", "7TH  ", "8TH  ", "9TH  ", "10TH ",
};

