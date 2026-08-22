/**
 * CommonData.c
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

#include "ChaseHQ/ChaseHQ.h"
#include "ChaseHQ/Engine/Types.h"

#include "CommonData.h"

/* ----------------------------------------------------------------------- */

// clang-format off

/* [Graphics] Faces
 *
 */

/** $7BE9: bitmap_faces */
const pixel_t bitmap_faces[FACEBYTES * NFACES] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X__XX__X, X_______, ________, ____X_XX,
  X_XX___X, ________, ________, _____X_X,
  XX____XX, ________, ________, _____X_X,
  X___XXXX, ____XXXX, XX______, __XX_X_X,
  X_XX_XX_, ___X____, _XXX____, XX__XXXX,
  XXX_XXX_, ________, __XX___X, X____XXX,
  XXX_XXX_, ___X_XXX, X_X____X, X____X__,
  XX_XXXX_, ___XXX_X, XXX____X, XXXX_XXX,
  XX_XXX__, ____XXXX, __XX___X, X_XXXXX_,
  X_XXXX__, ______XX, X______X, XXX_XXX_,
  XXXXXXX_, ________, ________, XX___XX_,
  XXXXXX__, ________, ____X___, _____XXX,
  XXXXX_X_, ________, ________, _____XXX,
  XXXX_X_X, ________, __X__X__, _____X_X,
  XXX_X_X_, ________, __X_XXXX, X____X_X,
  XXXX_X_X, _X______, _X_XXXXX, _____X__,
  _XXXX_X_, X_X_____, _____XX_, _____X__,
  _X_____X, _X_X_X__, ______X_, _____X__,
  ___XX___, __X_X___, ________, _____X__,
  X__X____, _X_X____, ___XXX_X, X___XX__,
  XXXX____, __X_____, _XXXXXXX, X___XX__,
  XXXX____, _X_X____, XXX____X, XX__XXX_,
  XXXX____, __X_____, ________, ____X_X_,
  XXXX____, ________, _X______, X__XX_X_,
  XXXXX___, ________, __XXXXXX, ___XX_X_,
  XXXXX___, ________, ___XXXXX, ___XX_X_,
  XXXXXX__, ________, ________, __XXX_X_,
  _XXXX_X_, X_______, ________, __XXX_X_,
  _XXX_X_X, XXXX_X__, _____XX_, __XXX_X_,
  _XXXX_X_, XXXXX_XX, _X_XXXXX, _XXXX_XX,
  __XX___X, XXXXXXXX, XXXXXXXX, XXXXX_XX,
  __XXX___, _XXXXXXX, XXXXXX_X, XXXXX__X,
  __XX____, ___XXXXX, XXXXX_XX, XXXXXX_X,
  __X_X___, _____X_X, XXXX_XXX, XXXXXX_X,
  __XX____, ______X_, XXXXXXXX, XXX_XX_X,
  _XX_____, _______X, XXXX_XXX, XXX_XX_X,
  _X_X____, _X____X_, XXX_XXXX, XXX_XX_X,
  _XX____X, __X____X, XXXX_XXX, XXX_XX_X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,

  attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_YELLOW, MKATTR(0, 1, ATTR_YELLOW, ATTR_RED), attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_CYAN, attribute_BLACK_OVER_YELLOW,

  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX__X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X,
  XXXXXXXX, XXXXXXXX, XX_X_XXX, XXXXXX_X,
  XXXXXXXX, XXXXXXXX, XXXXX_X_, XXXXXXXX,
  XXXXXXXX, XXXXXXX_, _X_X_X_X, XXXXXX_X,
  XXXXXXXX, XX_X_X__, ______X_, XXXXXXXX,
  XXXXXXXX, X_X_X___, _____X_X, _XXXXXXX,
  XXXXXXXX, XX______, __X_X_X_, XXXXXXXX,
  XXXXXXXX, X_X__X__, _X_X_XXX, _XXXXXXX,
  XXXXXXXX, XX______, _______X, XXXXXXXX,
  XXXXXXXX, _XX__X__, _X_XXX_X, _XXXXXXX,
  XXXXXXXX, XXXX_X_X, XXXXXXX_, XXXXXXXX,
  XXXXXXXX, XXXX__X_, XXX____X, _XXXXXXX,
  XXXXXXX_, ___XX___, _XXXXX__, __XXXXXX,
  XXXXXX__, XXXXXX__, _X_XXXX_, _XXXXXXX,
  XXXXXXXX, XXXXXX__, _XX_____, ___XXX_X,
  XXXXXX_X, __XXX___, __X_X___, __XXXXXX,
  XXXXXX__, ___XX___, X_______, ___XXXXX,
  XXXXX___, __XXX___, ____X___, __XXXX_X,
  XXXXXX_X, _XXXXX__, _XXXX___, ___XXX_X,
  XXXXXXX_, X_XXXXXX, X___XX__, __XXX__X,
  XXXXXXXX, XXXXXXXX, ____XXX_, X_XXX__X,
  X_XXXXXX, XXXXXXX_, _____XXX, _XXXXX_X,
  X_XXXXXX, XXXXXX_X, ____XXXX, XXXXX__X,
  XX_XXXXX, XXXXXXXX, XXX___XX, _XXXX__X,
  X__XXXXX, XXXXX___, ______XX, XXXX___X,
  X_X_XXXX, XXXXXX__, ___XX_X_, XXXX___X,
  X__XXXXX, XXXXXXXX, XXX__X_X, XXX_X__X,
  X___XXXX, XXXXXXXX, X_X_X_X_, X______X,
  X___XXXX, XXXXXX__, _____X_X, XX_____X,
  X____XXX, XXXXX___, ____X_XX, X_XXX__X,
  X____XXX, XXXXX___, _____XXX, X_XXXXXX,
  X___XX_X, XXXXXX_X, _X_XXXX_, X_XXXXXX,
  X_XX_X_X, XXXXXXXX, XXXXXXX_, X__XXXXX,
  XX___X__, XXXXXXXX, XXXXXX_X, ___XXXXX,
  X____X__, _XXXXXXX, XXXXX__X, ___XXXXX,
  X_XX_X__, _XXXXXXX, XXX___X_, ___XXXXX,
  XX__XX__, __XXX_X_, X_____X_, __XXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,

  attribute_BRIGHT_BLACK_OVER_CYAN, attribute_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_CYAN,
  attribute_BLACK_OVER_WHITE, attribute_BLACK_OVER_WHITE, attribute_BLACK_OVER_WHITE, attribute_BLACK_OVER_WHITE,
  attribute_BLACK_OVER_WHITE, attribute_BLACK_OVER_WHITE, attribute_BLACK_OVER_WHITE, attribute_BLACK_OVER_WHITE,
  attribute_BRIGHT_BLACK_OVER_CYAN, attribute_BLACK_OVER_WHITE, attribute_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_CYAN,
  attribute_BRIGHT_BLACK_OVER_CYAN, attribute_BLACK_OVER_WHITE, attribute_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_WHITE,

  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X,
  X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXX_XX_, _X_XXXXX, XXXXXXXX,
  XXXXXXXX, XX__XX__, _X_XXX_X, XXXXXXXX,
  XXXXXXXX, XXXX_X__, _X_X__XX, _XXXXXXX,
  XXXXXXXX, XXXX_X__, _X_X__XX, _XXXXXXX,
  XXXXXXXX, XXXXXX__, ___X__XX, _XX_XXXX,
  XXXXXXXX, X_XX_X__, __X___X_, _XX_XXXX,
  XXXXXXXX, XX______, __X_____, ____XXXX,
  XXXXXXXX, XX_XXX__, ________, XXXX_XXX,
  XXXXXXXX, __XXXXXX, XX____XX, XXXXXXXX,
  XXXXXXXX, _X__XXXX, XX___XXX, X___XXXX,
  XXXXXXXX, _X____XX, X____XXX, ___X_XXX,
  XXXXXXXX, ____XXXX, XX___XXX, XXX_XXXX,
  XXXXXXX_, X_XXXX_X, XX____XX, XX_XXXXX,
  XXXXXXXX, _XX_____, _X____XX, ____XXXX,
  XXX_XXX_, X__X_X__, _XX_____, X_X__XXX,
  XXX_XXX_, ________, XX______, _____X_X,
  XXX__XXX, _______X, __X___X_, _______X,
  XXX__XX_, X_____XX, _______X, _____X_X,
  XXXX__XX, _____XXX, XXX___XX, _______X,
  XXXX_XXX, X____XXX, XXXXXX_X, X____X_X,
  XXXXXXXX, _X_X_XXX, XXX_X_X_, XXX_X__X,
  X_XXXXXX, X_X_XXX_, X_X__X_X, _XXX_X_X,
  X__XXXXX, XX_XXXXX, _______X, XXX_X__X,
  X_XXXXXX, XXX_XX__, XXXXXXX_, ___XX__X,
  X_XXXXXX, XX_XXXX_, ________, __X_X__X,
  X_XXXX_X, XXXX_XXX, ______XX, _X_X___X,
  X_XXXXX_, XX_XX_X_, __XXXXX_, __XX___X,
  X__XXX_X, XXXX_X__, ________, _X_X___X,
  X___XXX_, XXXXX_X_, ________, X_XX___X,
  X_XXXX_X, _XXXXX_X, _X_X_X_X, _XX_XX_X,
  XX__X_X_, X_XXXXX_, X_XXX_X_, XXX___XX,
  X___X___, _XXXXXXX, XXXXXXXX, X_X__X_X,
  X__XX___, X_XXXXXX, XXXXXXXX, __X___XX,
  X__XX___, _X_X_XXX, XXXXXXXX, __X__X_X,
  XX_XX___, ____X_X_, XXX_X_XX, X_X_X_XX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,

  attribute_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_CYAN,
};

/* [Graphics] Street lamps etc.
 *
 */

/**
 * $7E0C: shortpole_bottom
 *
 * The first column of each pair is the depth, which
 * draw_object_left/right_stretchy_entrypt subtracts from the x-position table
 * value: a horizontal inset from the road edge. The streetlamp depthsets taper
 * theirs with distance ($28, $20, $18 ... $0C) because the lamp hangs out on an
 * arm. A short pole is a plain post standing flush at the edge, so it insets by
 * nothing at every depth.
 */
const depthset_t shortpole_bottom = {
  &streetlampbody_bitmaps[0],
  {
    { 0x00, DEPTHSET_OFFSET(0, 2) },
    { 0x00, DEPTHSET_OFFSET(0, 2) },
    { 0x00, DEPTHSET_OFFSET(3, 2) },
    { 0x00, DEPTHSET_OFFSET(3, 2) },
    { 0x00, DEPTHSET_OFFSET(6, 2) },
    { 0x00, DEPTHSET_OFFSET(6, 2) },
    { 0x00, DEPTHSET_OFFSET(9, 2) },
    { 0x00, DEPTHSET_OFFSET(9, 2) },
    { 0x00, DEPTHSET_OFFSET(12, 2) },
    { 0x00, DEPTHSET_OFFSET(12, 2) }
  }
};

/** $7E22: shortpole_middle */
const depthset_t shortpole_middle = {
  &streetlampbody_bitmaps[0],
  {
    { 0x00, DEPTHSET_OFFSET(1, 2) },
    { 0x00, DEPTHSET_OFFSET(1, 2) },
    { 0x00, DEPTHSET_OFFSET(4, 2) },
    { 0x00, DEPTHSET_OFFSET(4, 2) },
    { 0x00, DEPTHSET_OFFSET(7, 2) },
    { 0x00, DEPTHSET_OFFSET(7, 2) },
    { 0x00, DEPTHSET_OFFSET(10, 2) },
    { 0x00, DEPTHSET_OFFSET(10, 2) },
    { 0x00, DEPTHSET_OFFSET(13, 2) },
    { 0x00, DEPTHSET_OFFSET(13, 2) }
  }
};

/**
 * stretchy_shortpole
 *
 * Conv: host-side aggregate wrapping shortpole_bottom/shortpole_middle into the
 *       stretchy_t list format render_stretchy_object walks; not a Z80 table
 *       itself, so there is no single originating address -- see the
 *       components' own addresses below.
 */
const stretchy_t stretchy_shortpole[3] = {
  { STRETCHY_TYPE_FIXED, &shortpole_bottom },
  { STRETCHY_TYPE_50PC,  &shortpole_middle },
  { STRETCHY_TYPE_END, NULL } // Conv: NULL added
};

/**
 * $7E38: streetlampbottom_left
 *
 * seems to be shared streetlamp/telegraphpole
 */
const depthset_t streetlampbottom_left = {
  &streetlampbody_bitmaps[0],
  {
    { 0x28, DEPTHSET_OFFSET(0, 2) },
    { 0x20, DEPTHSET_OFFSET(0, 2) },
    { 0x18, DEPTHSET_OFFSET(3, 2) },
    { 0x18, DEPTHSET_OFFSET(3, 2) },
    { 0x14, DEPTHSET_OFFSET(6, 2) },
    { 0x14, DEPTHSET_OFFSET(6, 2) },
    { 0x10, DEPTHSET_OFFSET(9, 2) },
    { 0x10, DEPTHSET_OFFSET(9, 2) },
    { 0x0C, DEPTHSET_OFFSET(12, 2) },
    { 0x0C, DEPTHSET_OFFSET(12, 2) }
  }
};

/** $7E4E: streetlampbottom_right */
const depthset_t streetlampbottom_right = {
  &streetlampbody_bitmaps[0],
  {
    { 0x28, DEPTHSET_OFFSET(0, 2) },
    { 0x20, DEPTHSET_OFFSET(0, 2) },
    { 0x18, DEPTHSET_OFFSET(3, 2) },
    { 0x18, DEPTHSET_OFFSET(3, 2) },
    { 0x14, DEPTHSET_OFFSET(6, 2) },
    { 0x14, DEPTHSET_OFFSET(6, 2) },
    { 0x10, DEPTHSET_OFFSET(9, 2) },
    { 0x10, DEPTHSET_OFFSET(9, 2) },
    { 0x04, DEPTHSET_OFFSET(12, 2) },
    { 0x04, DEPTHSET_OFFSET(12, 2) }
  }
};

/** $7E64: streetlampmiddle2_left */
const depthset_t streetlampmiddle2_left = {
  &streetlampbody_bitmaps[0],
  {
    { 0x28, DEPTHSET_OFFSET(1, 2) },
    { 0x20, DEPTHSET_OFFSET(1, 2) },
    { 0x18, DEPTHSET_OFFSET(4, 2) },
    { 0x18, DEPTHSET_OFFSET(4, 2) },
    { 0x14, DEPTHSET_OFFSET(7, 2) },
    { 0x14, DEPTHSET_OFFSET(7, 2) },
    { 0x10, DEPTHSET_OFFSET(10, 2) },
    { 0x10, DEPTHSET_OFFSET(10, 2) },
    { 0x0C, DEPTHSET_OFFSET(13, 2) },
    { 0x0C, DEPTHSET_OFFSET(13, 2) }
  }
};

/** $7E7A: streetlampmiddle2_right */
const depthset_t streetlampmiddle2_right = {
  &streetlampbody_bitmaps[0],
  {
    { 0x28, DEPTHSET_OFFSET(1, 2) },
    { 0x20, DEPTHSET_OFFSET(1, 2) },
    { 0x18, DEPTHSET_OFFSET(4, 2) },
    { 0x18, DEPTHSET_OFFSET(4, 2) },
    { 0x14, DEPTHSET_OFFSET(7, 2) },
    { 0x14, DEPTHSET_OFFSET(7, 2) },
    { 0x10, DEPTHSET_OFFSET(10, 2) },
    { 0x10, DEPTHSET_OFFSET(10, 2) },
    { 0x04, DEPTHSET_OFFSET(13, 2) },
    { 0x04, DEPTHSET_OFFSET(13, 2) }
  }
};

/** $7E90: streetlampmiddle_left */
const depthset_t streetlampmiddle_left = {
  &streetlampbody_bitmaps[0],
  {
    { 0x28, DEPTHSET_OFFSET(2, 2) },
    { 0x20, DEPTHSET_OFFSET(2, 2) },
    { 0x18, DEPTHSET_OFFSET(5, 2) },
    { 0x18, DEPTHSET_OFFSET(5, 2) },
    { 0x14, DEPTHSET_OFFSET(8, 2) },
    { 0x14, DEPTHSET_OFFSET(8, 2) },
    { 0x10, DEPTHSET_OFFSET(11, 2) },
    { 0x10, DEPTHSET_OFFSET(11, 2) },
    { 0x0C, DEPTHSET_OFFSET(14, 2) },
    { 0x0C, DEPTHSET_OFFSET(14, 2) }
  }
};

/** $7EA6: streetlampmiddle_right */
const depthset_t streetlampmiddle_right = {
  &streetlampbody_bitmaps[0],
  {
    { 0x28, DEPTHSET_OFFSET(2, 2) },
    { 0x20, DEPTHSET_OFFSET(2, 2) },
    { 0x18, DEPTHSET_OFFSET(5, 2) },
    { 0x18, DEPTHSET_OFFSET(5, 2) },
    { 0x14, DEPTHSET_OFFSET(8, 2) },
    { 0x14, DEPTHSET_OFFSET(8, 2) },
    { 0x10, DEPTHSET_OFFSET(11, 2) },
    { 0x10, DEPTHSET_OFFSET(11, 2) },
    { 0x04, DEPTHSET_OFFSET(14, 2) },
    { 0x04, DEPTHSET_OFFSET(14, 2) }
  }
};

/** $7EBC: streetlampbody_bitmaps */
const bitmap_t streetlampbody_bitmaps[15] = {
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_1[0],  &bitmap_streetlampbody_1[0]   },
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_2[0],  &bitmap_streetlampbody_2[0]   },
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_3[0],  &bitmap_streetlampbody_3[0]   },
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_4[0],  &bitmap_streetlampbody_4[0]   },
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_5[0],  &bitmap_streetlampbody_5[0]   },
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_6[0],  &bitmap_streetlampbody_6[0]   },
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_streetlampbody_7[0],  &bitmap_streetlampbody_7s[0]  },
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_8[0],  &bitmap_streetlampbody_8s[0]  },
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_9[0],  &bitmap_streetlampbody_9s[0]  },
  { 1, BITMAPFLAG_MASKED, 1, &bitmap_streetlampbody_10[0], &bitmap_streetlampbody_10s[0] },
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_11[0], &bitmap_streetlampbody_11s[0] },
  { 1, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_12[0], &bitmap_streetlampbody_12s[0] },
  { 2, BITMAPFLAG_MASKED, 1, &bitmap_streetlampbody_13[0], &bitmap_streetlampbody_13s[0] },
  { 2, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_14[0], &bitmap_streetlampbody_14s[0] },
  { 2, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_15[0], &bitmap_streetlampbody_15s[0] }
};

/** $7F25: bitmap_streetlampbody_1 */
const pixel_t bitmap_streetlampbody_1[1 * 2 * 2 * 1] = {
  X______X, _XXXXXX_,
  _______X, XXX__XX_
};

/** $7F29: bitmap_streetlampbody_2 */
const pixel_t bitmap_streetlampbody_2[1 * 2 * 2 * 1] = {
  _______X, XX____X_,
  _______X, X_X___X_
};

/** $7F2D: bitmap_streetlampbody_3 */
const pixel_t bitmap_streetlampbody_3[1 * 2 * 2 * 1] = {
  X_____XX, _XX__X__,
  X_____XX, _X___X__
};

/** $7F31: bitmap_streetlampbody_4 */
const pixel_t bitmap_streetlampbody_4[1 * 2 * 2 * 1] = {
  XX____XX, __XXXX__,
  X______X, _X_X_XX_
};

/** $7F35: bitmap_streetlampbody_5 */
const pixel_t bitmap_streetlampbody_5[1 * 2 * 2 * 1] = {
  X_____XX, _XX__X__,
  X_____XX, _X___X__
};

/** $7F39: bitmap_streetlampbody_6 */
const pixel_t bitmap_streetlampbody_6[1 * 2 * 2 * 1] = {
  X____XXX, _X__X___,
  X____XXX, _X__X___
};

/** $7F3D: bitmap_streetlampbody_7 */
const pixel_t bitmap_streetlampbody_7[1 * 2 * 1 * 1] = {
  ____XXXX, XXXX____
};

/** $7F3F: bitmap_streetlampbody_8 */
const pixel_t bitmap_streetlampbody_8[1 * 2 * 2 * 1] = {
  ____XXXX, XX_X____,
  ____XXXX, XX_X____
};

/** $7F43: bitmap_streetlampbody_9 */
const pixel_t bitmap_streetlampbody_9[1 * 2 * 2 * 1] = {
  ___XXXXX, X_X_____,
  ___XXXXX, X_X_____
};

/** $7F47: bitmap_streetlampbody_7s */
const pixel_t bitmap_streetlampbody_7s[1 * 2 * 1 * 1] = {
  XXXX____, ____XXXX
};

/** $7F49: bitmap_streetlampbody_8s */
const pixel_t bitmap_streetlampbody_8s[1 * 2 * 2 * 1] = {
  XXXX____, ____XX_X,
  XXXX____, ____XX_X
};

/** $7F4D: bitmap_streetlampbody_9s */
const pixel_t bitmap_streetlampbody_9s[1 * 2 * 2 * 1] = {
  XXXX___X, ____X_X_,
  XXXX___X, ____X_X_
};

/** $7F51: bitmap_streetlampbody_10 */
const pixel_t bitmap_streetlampbody_10[1 * 2 * 1 * 1] = {
  XX___XXX, __XXX___
};

/** $7F53: bitmap_streetlampbody_11 */
const pixel_t bitmap_streetlampbody_11[1 * 2 * 2 * 1] = {
  XX___XXX, __X_X___,
  XX___XXX, __X_X___
};

/** $7F57: bitmap_streetlampbody_12 */
const pixel_t bitmap_streetlampbody_12[1 * 2 * 2 * 1] = {
  XX__XXXX, __XX____,
  XX__XXXX, __XX____
};

/** $7F5B: bitmap_streetlampbody_10s */
const pixel_t bitmap_streetlampbody_10s[1 * 2 * 1 * 1] = {
  XXXX___X, ____XXX_
};

/** $7F5D: bitmap_streetlampbody_11s */
const pixel_t bitmap_streetlampbody_11s[1 * 2 * 2 * 1] = {
  XXXX___X, ____X_X_,
  XXXX___X, ____X_X_
};

/** $7F61: bitmap_streetlampbody_12s */
const pixel_t bitmap_streetlampbody_12s[1 * 2 * 2 * 1] = {
  XXXX__XX, ____XX__,
  XXXX__XX, ____XX__
};

/** $7F65: bitmap_streetlampbody_13 */
const pixel_t bitmap_streetlampbody_13[2 * 2 * 1 * 1] = {
  XXXXXXX_, _______X,
  _XXXXXXX, X_______
};

/** $7F69: bitmap_streetlampbody_14 */
const pixel_t bitmap_streetlampbody_14[2 * 2 * 2 * 1] = {
  XXXXXXX_, _______X, _XXXXXXX, X_______,
  XXXXXXX_, _______X, _XXXXXXX, X_______
};

/** $7F71: bitmap_streetlampbody_15 */
const pixel_t bitmap_streetlampbody_15[2 * 2 * 2 * 1] = {
  XXXXXXXX, ________, _XXXXXXX, X_______,
  XXXXXXXX, ________, _XXXXXXX, X_______
};

/** $7F79: bitmap_streetlampbody_13s */
const pixel_t bitmap_streetlampbody_13s[2 * 2 * 1 * 1] = {
  XXXXXXXX, ________, XXX__XXX, ___XX___
};

/** $7F7D: bitmap_streetlampbody_14s */
const pixel_t bitmap_streetlampbody_14s[2 * 2 * 2 * 1] = {
  XXXXXXXX, ________, XXX__XXX, ___XX___,
  XXXXXXXX, ________, XXX__XXX, ___XX___
};

/** $7F85: bitmap_streetlampbody_15s */
const pixel_t bitmap_streetlampbody_15s[2 * 2 * 2 * 1] = {
  XXXXXXXX, ________, XXXX_XXX, ____X___,
  XXXXXXXX, ________, XXXX_XXX, ____X___
};

/* ----------------------------------------------------------------------- */

/** $81DD: chatterblk_start_stage */
const u8 chatterblk_start_stage[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_TONY_GIDDY_UP,
  CHATTERBLK_TONY_HOLD_ON,
  CHATTERBLK_TONY_LETS_GO
};

/** $81E4: chatterblk_tony_giddy_up */
const u8 chatterblk_tony_giddy_up[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_GIDDY_UP_BOY,
  CHATTERCMD_STOP
};

/** $81E8: chatterblk_tony_hold_on */
const u8 chatterblk_tony_hold_on[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_HOLD_ON_MAN,
  CHATTERCMD_STOP
};

/**
 * common_chatter_strings
 *
 * Conv: chatter strings from two locations ($81EC and $97B5, marked inline
 *       below) are combined into this single table; not a Z80 table itself, so
 *       there is no single originating address.
 */
const char *common_chatter_strings[CHATTERSTR__LIMIT] = {
  // $81EC
  "GIDDY UP BOY\xA1",
  "HOLD ON MA\xCE",
  // $97B5
  "THIS IS NANCY AT CHASE H.Q\xAE",
  "THIS IS SPECIAL INVESTIGATION AIRBORNE\xAE",
  "THE TARGET VEHICLE HAS TURNE\xC4",
  "RIGHT UP AHEAD... OVER\xAE",
  "LEFT UP AHEAD... OVER\xAE",
  "WE READ LOUD AND CLEAR! OVER\xAE",
  "ROGER\xA1",
  "GOTCHA NANCY BABY\xA1",
  "WHAT ARE YOU DOING MAN\xA1",
  "THE BAD GUYS ARE GOING THE OTHER WAY\xAE",
  "IF YOU KEEP MESSIN' AROUND LIKE THA\xD4",
  "YOUR TIME IS GOING TO RUN OUT... OVER\xAE",
  "LET'S GET MOVIN' MAN\xA1",
  "OH, NO\xA1",
  "PLEASE\xA1",
  "GREAT\xA1",
  "OUCH\xA1",
  "LET'S GO. MR. DRIVER\xAE",
  "YAOOOOOW\xA1",
  "BEAR DOWN\xAE",
  "MORE, PUSH IT MORE\xA1",
  "ONE MORE TIME\xAE",
  "OH MAN\xAE",
  "WHOAAAAA\xA1",
  "HARDER\xA1",
  "WE THINK YOU PICKED THE WRONG JOB\xAE",
  "BETTER CHECK THE CLASSIFIED ADS\xAE",
  "ONE MORE TRY FOR BEING A GOOD BOY\xA1",
  "YOU'RE A MEDIOCRE DRIVER, BROTHER\xA1",
  "SEE YOU LATER\xAE",
  "I WOULD LIKE TO CONGRATULATE YOU FOR YOU\xD2",
  "HEROISM AND BRAVERY. YOU WILL BE REMEMBERE\xC4",
  "FOR A LONG TIME TO COME\xA1",
  "PRESS GEAR TO CONTINU\xC5"
  // CHATTERSTR_PERP_DESC_1..4 (36..39) -- these vary with stage
};

/**
 * $897C: sfx_crash_table
 *
 * Conv: This is now a template copied into state.
 */
const u8 sfx_crash_table[93] = {
  0xFF, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF,
  0xEF, 0xFF, 0xFF, 0xFF, 0xBE, 0x36, 0x10, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x78, 0x80,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xC0, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0xF3, 0x9E,
  0x70, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x9E, 0xFF, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xFF, 0x3F,
  0x38, 0xF0, 0x03, 0x80, 0x00
};

/** $98A9: chatterblk_pilot_turn_left */
const u8 chatterblk_pilot_turn_left[6] = {
  CHATTERCHR_PILOT,
  CHATTERSTR_THIS_IS_AIRBORNE,
  CHATTERSTR_TARGET_VEHICLE_TURNED,
  CHATTERSTR_LEFT_AHEAD_OVER,
  CHATTERCMD_PAUSE,
  CHATTERBLK_TONY_LOUD_CLEAR
};

/** $98B3: chatterblk_pilot_turn_right */
const u8 chatterblk_pilot_turn_right[6] = {
  CHATTERCHR_PILOT,
  CHATTERSTR_THIS_IS_AIRBORNE,
  CHATTERSTR_TARGET_VEHICLE_TURNED,
  CHATTERSTR_RIGHT_AHEAD_OVER,
  CHATTERCMD_PAUSE,
  CHATTERBLK_TONY_LOUD_CLEAR
};

/** $98BD: chatterblk_heroes_acknowledge */
const u8 chatterblk_heroes_acknowledge[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_TONY_LOUD_CLEAR,
  CHATTERBLK_RAYMOND_ROGER,
  CHATTERBLK_TONY_GOTCHA,
};

/** $98C4: chatterblk_tony_loud_clear */
const u8 chatterblk_tony_loud_clear[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_READ_LOUD_CLEAR,
  CHATTERCMD_STOP
};

/** $98C8: chatterblk_raymond_roger */
const u8 chatterblk_raymond_roger[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_ROGER,
  CHATTERCMD_STOP
};

/** $98CC: chatterblk_tony_gotcha */
const u8 chatterblk_tony_gotcha[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_GOTCHA_NANCY,
  CHATTERCMD_STOP
};

/** $98D0: chatterblk_raymond_wrong_way */
const u8 chatterblk_raymond_wrong_way[4] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_WHAT_YOU_DOING,
  CHATTERSTR_GOING_OTHER_WAY,
  CHATTERCMD_STOP
};

/** $98D6: chatterblk_raymond_smash */
const u8 chatterblk_raymond_smash[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_RAYMOND_BEAR_DOWN,
  CHATTERBLK_RAYMOND_RANDOM_PLEAS,
  CHATTERBLK_RAYMOND_PUSH_IT
};

/** $98DD: chatterblk_raymond_bear_down */
const u8 chatterblk_raymond_bear_down[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_BEAR_DOWN,
  CHATTERCMD_STOP
};

/** $98E1: chatterblk_raymond_push_it */
const u8 chatterblk_raymond_push_it[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_MORE_PUSH_MORE,
  CHATTERCMD_STOP
};

/** $98E5: chatterblk_raymond_harder */
const u8 chatterblk_raymond_harder[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_HARDER,
  CHATTERCMD_STOP
};

/** $98E9: chatterblk_raymond_oh_man */
const u8 chatterblk_raymond_oh_man[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_OH_MAN,
  CHATTERCMD_STOP
};

/** $98ED: chatterblk_raymond_random_pleas */
const u8 chatterblk_raymond_random_pleas[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_RAYMOND_OH_MAN,
  CHATTERBLK_RAYMOND_HARDER,
  CHATTERBLK_RAYMOND_PLEASE
};

/** $98F4: chatterblk_raymond_please */
const u8 chatterblk_raymond_please[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_PLEASE,
  CHATTERCMD_STOP
};

/** $98F8: chatterblk_raymond_get_moving */
const u8 chatterblk_raymond_get_moving[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_GET_MOVIN_MAN,
  CHATTERCMD_STOP
};

/** $98FC: chatterblk_nancy_time_running_out */
const u8 chatterblk_nancy_time_running_out[5] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_THIS_IS_NANCY,
  CHATTERSTR_MESSIN_AROUND,
  CHATTERSTR_TIME_RUN_OUT,
  CHATTERCMD_STOP
};

/** $9904: chatterblk_raymond_random_yelps */
const u8 chatterblk_raymond_random_yelps[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_RAYMOND_OHNO,
  CHATTERBLK_RAYMOND_OUCH,
  CHATTERBLK_RAYMOND_YAOW
};

/** $990B: chatterblk_raymond_ohno */
const u8 chatterblk_raymond_ohno[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_OH_NO,
  CHATTERCMD_STOP
};

/** $990F: chatterblk_raymond_ouch */
const u8 chatterblk_raymond_ouch[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_OUCH,
  CHATTERCMD_STOP
};

/** $9913: chatterblk_raymond_yaow */
const u8 chatterblk_raymond_yaow[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_YAOW,
  CHATTERCMD_STOP
};

/** $9917: chatterblk_turbo */
const u8 chatterblk_turbo[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_TONY_WHOA,
  CHATTERBLK_TONY_GREAT,
  CHATTERBLK_RAYMOND_ONE_MORE_TIME
};

/** $991E: chatterblk_tony_whoa */
const u8 chatterblk_tony_whoa[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_WHOA,
  CHATTERCMD_STOP
};

/** $9922: chatterblk_tony_great */
const u8 chatterblk_tony_great[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_GREAT,
  CHATTERCMD_STOP
};

/** $9926: chatterblk_raymond_one_more_time */
const u8 chatterblk_raymond_one_more_time[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_ONE_MORE_TIME,
  CHATTERCMD_STOP
};

/** $992A: chatterblk_nancy_berates_hero */
const u8 chatterblk_nancy_berates_hero[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_NANCY_WRONG_JOB,
  CHATTERBLK_NANCY_ONE_MORE_TRY,
  CHATTERBLK_NANCY_MEDIOCRE_DRIVER
};

/** $9931: chatterblk_nancy_wrong_job */
const u8 chatterblk_nancy_wrong_job[3] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_PICKED_WRONG_JOB,
  CHATTERCMD_STOP
};

/** $9937: chatterblk_nancy_one_more_try */
const u8 chatterblk_nancy_one_more_try[3] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_ONE_MORE_TRY,
  CHATTERCMD_STOP
};

/** $993B: chatterblk_nancy_mediocre_driver */
const u8 chatterblk_nancy_mediocre_driver[4] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_MEDIOCRE_DRIVER,
  CHATTERSTR_SEE_YOU_LATER,
  CHATTERCMD_STOP
};

/** $9941: chatterblk_tony_lets_go */
const u8 chatterblk_tony_lets_go[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_LETS_GO,
  CHATTERCMD_STOP
};

/**
 * chatter_blocks
 *
 * Conv: host-side index of all chatter blocks, added so callers can look one up
 *       by enum instead of address; not a Z80 table itself, so there is no
 *       single originating address -- see each chatterblk_* array's own address
 *       above. Strictly this only needs to hold the blocks that are referred to
 *       from other blocks.
 */
const u8 *chatter_blocks[CHATTERBLK__LIMIT] = {
  &chatterblk_start_stage[0],
  &chatterblk_tony_giddy_up[0],
  &chatterblk_tony_hold_on[0],
  &chatterblk_pilot_turn_left[0],
  &chatterblk_pilot_turn_right[0],
  &chatterblk_heroes_acknowledge[0],
  &chatterblk_tony_loud_clear[0],
  &chatterblk_raymond_roger[0],
  &chatterblk_tony_gotcha[0],
  &chatterblk_raymond_wrong_way[0],
  &chatterblk_raymond_smash[0],
  &chatterblk_raymond_bear_down[0],
  &chatterblk_raymond_push_it[0],
  &chatterblk_raymond_harder[0],
  &chatterblk_raymond_oh_man[0],
  &chatterblk_raymond_random_pleas[0],
  &chatterblk_raymond_please[0],
  &chatterblk_raymond_get_moving[0],
  &chatterblk_nancy_time_running_out[0],
  &chatterblk_raymond_random_yelps[0],
  &chatterblk_raymond_ohno[0],
  &chatterblk_raymond_ouch[0],
  &chatterblk_raymond_yaow[0],
  &chatterblk_turbo[0],
  &chatterblk_tony_whoa[0],
  &chatterblk_tony_great[0],
  &chatterblk_raymond_one_more_time[0],
  &chatterblk_nancy_berates_hero[0],
  &chatterblk_nancy_wrong_job[0],
  &chatterblk_nancy_one_more_try[0],
  &chatterblk_nancy_mediocre_driver[0],
  &chatterblk_tony_lets_go[0]
};

/* ----------------------------------------------------------------------- */

/** $A27A: font */
const pixel_t font[41 * 7] = {
  __XX____,
  _XXXX___,
  _XXXX___,
  __XX____,
  ________,
  __XX____,
  __XX____,

  _____XX_,
  ____XX__,
  ___XX___,
  ___XX___,
  ___XX___,
  ____XX__,
  _____XX_,

  XX______,
  _XX_____,
  __XX____,
  __XX____,
  __XX____,
  _XX_____,
  XX______,

  ________,
  ________,
  ________,
  ________,
  ________,
  __XX____,
  _XX_____,

  ________,
  ________,
  ________,
  ________,
  ________,
  __XX____,
  __XX____,

  __XXXX__,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  __XXXX__,

  ___XX___,
  __XXX___,
  ___XX___,
  ___XX___,
  ___XX___,
  ___XX___,
  __XXXX__,

  __XXXX__,
  _XX__XX_,
  _____XX_,
  ____XX__,
  ___XX___,
  __XX____,
  _XXXXXX_,

  __XXXX__,
  _XX__XX_,
  _____XX_,
  ___XXX__,
  _____XX_,
  _XX__XX_,
  __XXXX__,

  ____XXX_,
  ___XXXX_,
  __XX_XX_,
  _XX__XX_,
  _XXXXXXX,
  _____XX_,
  _____XX_,

  _XXXXXX_,
  _XX_____,
  _XXXXX__,
  _____XX_,
  _____XX_,
  _XX__XX_,
  __XXXX__,

  ____XX__,
  ___XX___,
  __XX____,
  _XXXXX__,
  _XX__XX_,
  _XX__XX_,
  __XXXX__,

  _XXXXXX_,
  _____XX_,
  ____XX__,
  ___XX___,
  __XX____,
  __XX____,
  __XX____,

  __XXXX__,
  _XX__XX_,
  _XX__XX_,
  __XXXX__,
  _XX__XX_,
  _XX__XX_,
  __XXXX__,

  __XXXX__,
  _XX__XX_,
  _XX__XX_,
  __XXXXX_,
  ____XX__,
  ___XX___,
  __XX____,

  __XXXX__,
  _XX__XX_,
  _XX__XX_,
  _XXXXXX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,

  _XXXXX__,
  _XX__XX_,
  _XX__XX_,
  _XXXXX__,
  _XX__XX_,
  _XX__XX_,
  _XXXXX__,

  __XXXX__,
  _XX__XX_,
  _XX_____,
  _XX_____,
  _XX_____,
  _XX__XX_,
  __XXXX__,

  _XXXXX__,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XXXXX__,

  _XXXXXX_,
  _XX_____,
  _XX_____,
  _XXXX___,
  _XX_____,
  _XX_____,
  _XXXXXX_,

  _XXXXXX_,
  _XX_____,
  _XX_____,
  _XXXX___,
  _XX_____,
  _XX_____,
  _XX_____,

  __XXXX__,
  _XX__XX_,
  _XX_____,
  _XX_XXX_,
  _XX__XX_,
  _XX__XX_,
  __XXXXX_,

  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XXXXXX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,

  __XXXX__,
  ___XX___,
  ___XX___,
  ___XX___,
  ___XX___,
  ___XX___,
  __XXXX__,

  __XXXXX_,
  _____XX_,
  _____XX_,
  _____XX_,
  _XX__XX_,
  _XX__XX_,
  __XXXX__,

  _XX__XX_,
  _XX_XX__,
  _XXXX___,
  _XXX____,
  _XXXX___,
  _XX_XX__,
  _XX__XX_,

  _XX_____,
  _XX_____,
  _XX_____,
  _XX_____,
  _XX_____,
  _XX_____,
  _XXXXXX_,

  XX___XX_,
  XXX_XXX_,
  XXXXXXX_,
  XX_X_XX_,
  XX___XX_,
  XX___XX_,
  XX___XX_,

  _XX__XX_,
  _XXX_XX_,
  _XXXXXX_,
  _XX_XXX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,

  __XXXX__,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  __XXXX__,

  _XXXXX__,
  _XX__XX_,
  _XX__XX_,
  _XXXXX__,
  _XX_____,
  _XX_____,
  _XX_____,

  __XXXX__,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XX_XXX_,
  _XX_XX__,
  __XX_XX_,

  _XXXXX__,
  _XX__XX_,
  _XX__XX_,
  _XXXXX__,
  _XX_XX__,
  _XX__XX_,
  _XX__XX_,

  __XXXX__,
  _XX__XX_,
  _XX_____,
  __XXXX__,
  _____XX_,
  _XX__XX_,
  __XXXX__,

  _XXXXXX_,
  ___XX___,
  ___XX___,
  ___XX___,
  ___XX___,
  ___XX___,
  ___XX___,

  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  __XXXX__,

  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  __XXXX__,
  ___XX___,

  XX___XX_,
  XX___XX_,
  XX___XX_,
  XX_X_XX_,
  XXXXXXX_,
  XXX_XXX_,
  XX___XX_,

  _XX__XX_,
  _XX__XX_,
  __XXXX__,
  ___XX___,
  __XXXX__,
  _XX__XX_,
  _XX__XX_,

  _XX__XX_,
  _XX__XX_,
  _XX__XX_,
  __XXXX__,
  ___XX___,
  ___XX___,
  ___XX___,

  _XXXXXX_,
  _____XX_,
  ____XX__,
  ___XX___,
  __XX____,
  _XX_____,
  _XXXXXX_
};

/* ----------------------------------------------------------------------- */

/** $A7E7: hazard_pos_speed */
const u8 hazard_pos_speed[3 * 4] = {
  5, 80, 148, 216,    // road positions the hazard will align with
  140, 220, 170, 250, // hazard speed used when perp has not been sighted
  60, 140, 90, 170    // hazard speed used when perp is sighted
};

/* ----------------------------------------------------------------------- */

/** $B045: hero_car_jump_table */
const s8 hero_car_jump_table[10 * 2] = {
  3,  13,  // Pitch Up,    Delta  13
  3,  10,  // Pitch Up,    Delta  10
  3,  7,   // Pitch Up,    Delta   7
  3,  4,   // Pitch Up,    Delta   4
  0,  2,   // Pitch Level, Delta   2
  0,  -2,  // Pitch Level, Delta  -2
  6,  -4,  // Pitch Down,  Delta  -4
  6,  -7,  // Pitch Down,  Delta  -7
  6,  -10, // Pitch Down,  Delta -10
  6,  -13  // Pitch Down,  Delta -13
};

/**
 * $B828: horizon_table
 *
 * Byte table: scroll_horizon ($B265) reads HL=($B827+curvature) as a byte;
 * update_road_level ($B9BD) reads HL=($B828+speed_part+abs_curvature*4) as byte.
 */
const u8 horizon_table[32] = {
  0xFF, 0x7F,  /* $B828: from DEFW $7FFF */
  0x55, 0x3F,  /* $B82A: from DEFW $3F55 */
  0x33, 0x2A,  /* $B82C: from DEFW $2A33 */
  0x24, 0x1F,  /* $B82E: from DEFW $1F24 */
  0x04, 0x01,  /* $B830: from DEFW $0104 */
  0x03, 0x01,  /* $B832: from DEFW $0103 */
  0x02, 0x01,  /* $B834: from DEFW $0102 */
  0x01, 0x01,  /* $B836: from DEFW $0101 */
  0x03, 0x01,  /* $B838: from DEFW $0103 */
  0x02, 0x01,  /* $B83A: from DEFW $0102 */
  0x01, 0x01,  /* $B83C: from DEFW $0101 */
  0x01, 0x02,  /* $B83E: from DEFW $0201 */
  0x03, 0x01,  /* $B840: from DEFW $0103 */
  0x02, 0x01,  /* $B842: from DEFW $0102 */
  0x01, 0x01,  /* $B844: from DEFW $0101 */
  0x01, 0x02   /* $B846: from DEFW $0201 */
};

/* ----------------------------------------------------------------------- */

/** $D03F: bitmap_arrow */
const pixel_t bitmap_arrow[3 * 2 * 28] = {
  XXXXX___, _____XXX, _______X, XXXXXXX_, XXXXXXXX, ________,
  XX______, _____XXX, ______XX, XXXXXX__, XXXXXXXX, ________,
  XX______, ___XXXXX, ________, XXXXXXXX, XXXXXXXX, ________,
  XX______, ___X____, _______X, _____XX_, XXXXXXXX, ________,
  XX______, ___X____, ______XX, ____XX__, XXXXXXXX, ________,
  XX______, ___X____, _______X, ___XXXX_, XXXXXXXX, ________,
  XX______, ___X____, ________, __XXXXXX, XXXXXXXX, ________,
  XX______, ___X____, ________, ___XXXXX, _XXXXXXX, X_______,
  XX______, ___X____, ________, ____XXXX, __XXXXXX, XX______,
  XX______, ___X___X, ________, _____XXX, ___XXXXX, XXX_____,
  XX______, ___X__XX, ________, X_____XX, ____XXXX, XXXX____,
  XX______, ___X_XX_, ________, XX_____X, _____XXX, XXXXX___,
  XX_____X, ___XXX__, ________, _XX_____, ____XXXX, XXXX____,
  XX____XX, ___XX___, X_______, __XX____, ____XXXX, _XX_____,
  XX___XXX, ___X____, XX______, ___XX___, _____XXX, __XX____,
  XX__XXXX, ________, XXX_____, ____XX__, ______XX, ___XX___,
  XX_XXXXX, ________, XXXX____, _____XX_, _______X, ____XX__,
  XXXXXXXX, ________, XXXXX___, ______XX, ______XX, ___XX___,
  XXXXXXXX, ________, XXXXXX__, _______X, _____XXX, X_XX____,
  XXXXXXXX, ________, XXXXXXX_, ________, ____XXXX, XXX_____,
  XXXXXXXX, ________, XXXXXXXX, ________, ___XXXXX, _X______,
  XX__X_X_, ________, __X__X_X, ________, ___X_XXX, ________,
  X_______, __XX_X_X, ________, XX_XX_X_, ______XX, XXX_X___,
  X_______, __XX_X_X, ________, X__XXXX_, ___X__XX, XX______,
  X_______, __XXXX_X, ________, XX_XXX__, _______X, XXX__X__,
  X_______, __XX_X_X, ________, X__XX_X_, ___X____, XX___XX_,
  X_______, __XX_X_X, ________, XX_XXXX_, ________, XXX__XX_,
  XX__X_X_, ________, __X____X, ________, ___XX__X, ________
};

/* ----------------------------------------------------------------------- */

/** $E1E9: tunnellight */
const depthset_t tunnellight = {
  &tunnellight_bitmaps[0],
  0x1C, DEPTHSET_OFFSET(0, 0),
  0x14, DEPTHSET_OFFSET(1, 0),
  0x10, DEPTHSET_OFFSET(1, 0),
  0x10, DEPTHSET_OFFSET(2, 0),
  0x0C, DEPTHSET_OFFSET(2, 0),
  0x0C, DEPTHSET_OFFSET(3, 0),
  0x0C, DEPTHSET_OFFSET(3, 0),
  0x08, DEPTHSET_OFFSET(4, 0),
  0x08, DEPTHSET_OFFSET(4, 0),
  0x08, DEPTHSET_OFFSET(5, 0)
};

/** $E1FF: tunnellight_bitmaps */
const bitmap_t tunnellight_bitmaps[SPRITE_FRAMES] = {
  { 2, BITMAPFLAG_DEFAULT, 16, &bitmap_tunnellight_1[0], &bitmap_tunnellight_1[0]  },
  { 2, BITMAPFLAG_DEFAULT, 12, &bitmap_tunnellight_2[0], &bitmap_tunnellight_2[0]  },
  { 1, BITMAPFLAG_DEFAULT,  8, &bitmap_tunnellight_3[0], &bitmap_tunnellight_3[0]  },
  { 1, BITMAPFLAG_DEFAULT,  6, &bitmap_tunnellight_4[0], &bitmap_tunnellight_4[0]  },
  { 1, BITMAPFLAG_DEFAULT,  5, &bitmap_tunnellight_5[0], &bitmap_tunnellight_5[0]  },
  { 2, BITMAPFLAG_MASKED,   6, &bitmap_tunnellight_6[0], &bitmap_tunnellight_6s[0] }
};

/** $E229: bitmap_tunnellight_1 */
const pixel_t bitmap_tunnellight_1[2 * 1 * 16 * 1] = {
  XXXXXXXX, _XXXXXXX,
  XX_XXXX_, _XXXXXXX,
  XXX_X_X_, _X_XXXXX,
  XXX__X__, __XXX__X,
  XXXX____, _X_X__XX,
  X_X_X___, __X__XXX,
  XX_X____, ____X_XX,
  ________, _____XXX,
  XX______, ________,
  X_X_____, ____X_XX,
  XX_X____, ___X_XXX,
  X_X_____, ____X_XX,
  XX___X__, __X__XXX,
  X__XX_X_, _X_X__XX,
  XXXXXXX_, XXXXX_XX,
  XXXXXXX_, XXXXXXXX,
};

/** $E249: bitmap_tunnellight_2 */
const pixel_t bitmap_tunnellight_2[2 * 1 * 12 * 1] = {
  XXXXX_XX, XXXXXXXX,
  X_XXX_XX, X_XXXXXX,
  XX_X___X, _XXXXXXX,
  XXX_____, _X_XXXXX,
  X_______, __XXXXXX,
  XX______, ____XXXX,
  ________, __XXXXXX,
  XX______, ___XXXXX,
  XX______, __XXXXXX,
  X_XX____, XX_XXXXX,
  XXX_X_XX, _XXXXXXX,
  XXXXX_XX, XXXXXXXX,
};

/** $E261: bitmap_tunnellight_3 */
const pixel_t bitmap_tunnellight_3[1 * 1 * 8 * 1] = {
  XXX_XXXX,
  X_X_X_XX,
  XX_____X,
  ______XX,
  XX______,
  X_____XX,
  XX_X_X_X,
  X_XX_XXX,
};

/** $E269: bitmap_tunnellight_4 */
const pixel_t bitmap_tunnellight_4[1 * 1 * 6 * 1] = {
  XXXXXXXX,
  _X_X_XXX,
  X___XXXX,
  _____XXX,
  X___XXXX,
  _X_X_XXX,
};

/** $E26F: bitmap_tunnellight_5 */
const pixel_t bitmap_tunnellight_5[1 * 1 * 5 * 1] = {
  XX_XXXXX,
  X___XXXX,
  _____XXX,
  X___XXXX,
  XX_XXXXX,
};

/** $E274: bitmap_tunnellight_6 */
const pixel_t bitmap_tunnellight_6[2 * 2 * 6 * 1] = {
  XX__XXXX, __XX____, XXXXXXXX, ________,
  X____XXX, _X__X___, XXXXXXXX, ________,
  ______XX, X____X__, XXXXXXXX, ________,
  ______XX, X____X__, XXXXXXXX, ________,
  X____XXX, _X__X___, XXXXXXXX, ________,
  XX__XXXX, __XX____, XXXXXXXX, ________,
};

/** $E28C: bitmap_tunnellight_6s */
const pixel_t bitmap_tunnellight_6s[2 * 2 * 6 * 1] = {
  XXXXXX__, ______XX, XXXXXXXX, ________,
  XXXXX___, _____X__, _XXXXXXX, X_______,
  XXXX____, ____X___, __XXXXXX, _X______,
  XXXX____, ____X___, __XXXXXX, _X______,
  XXXXX___, _____X__, _XXXXXXX, X_______,
  XXXXXX__, ______XX, XXXXXXXX, ________,
};

/* ----------------------------------------------------------------------- */

/**
 * Helicopter sprite ($AA38 draw_helicopter). Stages 2 and 4 both reference
 * this identical part/bitmap data from their own banks (only the two frame
 * table addresses differ in the original); stages 1, 3 and 5 have no
 * helicopter (addrof_helicopter_frames is { NULL, NULL }).
 *
 * Part 0 is shared by both animation frames; parts 1-4 and the rotor have a
 * distinct bitmap per frame, selected by anim_counter bit 0.
 */
/** $ECE7: bitmap_heli_part2_frame0 */
const pixel_t bitmap_heli_part2_frame0[7 * 1 * 16 * 1] = {
  _____X__, X_X_X___, ____X_X_, __XXXX_X, _X_X_X_X, ________, ________,
  ________, _X_XXXX_, ____X_X_, ___XX_X_, XXXXX_X_, ________, ___X____,
  ________, ___XXX_X, _X___X_X, __XXXX_X, _XX__X__, ________, ____X___,
  ________, ____X_XX, XX_X_X__, ___XX_X_, X_X_____, ________, ___X_X__,
  ________, _______X, _XX_XXX_, __XXXX_X, _XX_____, _____X_X, X_X_XX__,
  ________, ________, ___XXXXX, __X__XXX, XX______, ___XX_X_, XXXXXXX_,
  ________, ________, ________, X__XX__X, ___X_XX_, X_XXXXXX, X_X_X_X_,
  _X______, ________, ______XX, X_XXXX_X, XXXXXXXX, XX_X_X_X, _X______,
  ________, ________, ______X_, X_XXXX_X, _XX_X_X_, X_X_X___, ______X_,
  _X______, ___X_X_X, _X_XXXXX, X_XXXX_X, XXX_____, ________, ________,
  __X_X_X_, XX_XXXXX, X_X_XXX_, X__XX__X, X__XX___, ________, ________,
  _XXXXXXX, XXX_X_X_, X_____XX, X_X____X, _XXX_X__, ________, ________,
  _XXX_X_X, _X_X____, ____XXX_, _X_X_XX_, X__XX_X_, X_______, ________,
  __X_X_X_, ________, ___XXX__, _X____XX, _____XXX, X_XX____, ________,
  ________, ________, __X_X___, _X____X_, ______X_, XXX_X___, ________,
  ____X___, ________, _X_X_X__, _X____X_, ________, _X_XXX_X, __X_____,
};

/** $ED57: bitmap_heli_part1_frame0 */
const pixel_t bitmap_heli_part1_frame0[3 * 1 * 7 * 1] = {
  X___X__X, XXXXXXXX, X_XXX___,
  X___X__X, XXXXXXXX, _X_XX___,
  X___XX__, XXXXXXXX, X_X_X___,
  X___X_X_, _XXXXXXX, _X_XX___,
  XXXXX_X_, _XXXXXX_, X_X_XXXX,
  ___X_X_X, __XXXX_X, _XXXX_XX,
  ___X_X__, ___XX_XX, X_XXX_X_,
};

/** $ED6C: bitmap_heli_part2_frame1 */
const pixel_t bitmap_heli_part2_frame1[7 * 1 * 16 * 1] = {
  ________, ________, X_XXX_X_, __XXXX_X, _X_X____, ____X_XX, XXX_____,
  _____X__, ________, _X_XX_X_, ___XX_X_, XXXX____, _X_XXX_X, _X_X____,
  ____X___, ________, __X__X_X, __XXXX_X, _XX_____, X_XXX_X_, ________,
  ___X_X__, ________, ___XXX__, ___XX_X_, X_X__X_X, XX_X____, ________,
  ___XX_X_, X_______, _____XX_, __XXXX_X, _XXX_XX_, X_______, ________,
  __XXXXXX, _X_X_X_X, _______X, __X__XXX, XX_XX___, ________, ________,
  __XX_X_X, XXXXXXXX, XX_XXX__, X__XX__X, ________, ________, _____X__,
  __X_X_X_, X_X_X_X_, XXXXXXXX, X_XXXX_X, XXX_____, ________, ________,
  __XX____, ________, ___X_XX_, X_XXXX_X, _XXX_X__, ________, _____X__,
  ___X____, ________, ______XX, X_XXXX_X, X_X_X_XX, X_X_X_X_, X_X_X_X_,
  ________, ________, __XX_XX_, X__XX__X, _____X_X, _XXXXX_X, _X_X_XX_,
  ________, _______X, _X_XXXXX, X_X____X, ________, X_X_X_XX, XXXXXX__,
  ________, ______X_, XXXX_X__, XX_X_XX_, X_______, _______X, _X_X_XX_,
  ____X___, _X_X_XXX, _X__X__X, XX____XX, _XX_____, ________, X_X__X__,
  _____XX_, X_XXX_X_, X_______, XX____X_, __XX____, ________, ____X___,
  _____XXX, XXXX_X__, _______X, _X____X_, _X_XX_X_, ________, ________,
};

/** $EDDC: bitmap_heli_part1_frame1 */
const pixel_t bitmap_heli_part1_frame1[3 * 1 * 7 * 1] = {
  X___X__X, XXXXXXXX, X_XXX___,
  X___X__X, XXXXXXXX, _X_XX___,
  X___XX__, XXXXXXXX, X_X_X___,
  X___X_X_, _XXXXXXX, _X_XX___,
  XXXXX_X_, _XXXXXX_, X_X_XXXX,
  X__X_X_X, __XXXX_X, _XXXX___,
  _X_X_X__, ___XX_XX, X_XXX___,
};

/** $EDF1: bitmap_heli_part0 (masked: 3 visual columns, 6 raw bytes/row) */
const pixel_t bitmap_heli_part0[3 * 2 * 10 * 1] = {
  XXXXX__X, ________, XX_____X, ________, X__XXXXX, ________,
  XXXX____, _____XX_, X_______, __XXXXX_, ____XXXX, _XX_____,
  XXX_____, ____X_XX, ________, _XXXXXXX, _____XXX, _XXX____,
  XXX_____, ____X__X, ________, XXXXXXXX, ______XX, XXXXX___,
  XX______, ___X__XX, ________, XXXXXXXX, ______XX, XXXXX___,
  XX______, ___X_XXX, ________, XXXXXXXX, _______X, XXXXXX__,
  XX______, ___X__XX, ________, XXXXXXXX, _______X, XXXXXX__,
  X_______, __X__XXX, ________, X_______, ________, XXXX_XX_,
  X_______, __X_XX_X, ________, _XXXXXXX, ________, _X_XX_X_,
  ________, _X_X_XXX, ________, XXXXXXXX, ________, XXXX_X_X,
};

/** $EE2D: bitmap_heli_part3_frame0 (masked: 2 visual columns, 4 raw bytes/row) */
const pixel_t bitmap_heli_part3_frame0[2 * 2 * 3 * 1] = {
  XXXXXXX_, ________, X____XX_, ___X____,
  XXXXXX__, _______X, ____XXXX, _XX_____,
  XXXXX___, _______X, _____XXX, XX_X____,
};

/** $EE39: bitmap_heli_part4_frame0 (masked: 1 visual column, 2 raw bytes/row) */
const pixel_t bitmap_heli_part4_frame0[1 * 2 * 7 * 1] = {
  __XXXXXX, X_______,
  __XXXXXX, X_______,
  ____XXXX, X_______,
  ______XX, XXXX____,
  _______X, XXX_XX__,
  ________, _X_X__X_,
  ____XX_X, X_X_____,
};

/** $EE47: bitmap_heli_part3_frame1 (masked: 1 visual column, 2 raw bytes/row) */
const pixel_t bitmap_heli_part3_frame1[1 * 2 * 5 * 1] = {
  XXXXX___, ________,
  XXX_____, _____XXX,
  XX______, ___XXX_X,
  X_______, __X_X_X_,
  XX_X____, _____X_X,
};

/** $EE51: bitmap_heli_part4_frame1 (masked: 2 visual columns, 4 raw bytes/row) */
const pixel_t bitmap_heli_part4_frame1[2 * 2 * 7 * 1] = {
  __XXXXXX, X_______, XXXXXXXX, ________,
  __XXXXXX, X_______, XXXXXXXX, ________,
  __XXXXXX, X_______, XXXXXXXX, ________,
  __XXX__X, X_______, XXXXXXXX, ________,
  _XXX____, _____XX_, _XXXXXXX, ________,
  XXXXX___, _______X, __XXXXXX, X_______,
  XXXX_X__, ________, ___XXXXX, XX______,
};

/** $EE6D: bitmap_heli_rotor_frame0 (masked: 5 visual columns, 10 raw bytes/row) */
const pixel_t bitmap_heli_rotor_frame0[5 * 2 * 10 * 1] = {
  XXXXXXXX, ________, XXXXXXXX, ________, _X_X__XX, X_X_XX__, XXX_X_XX, ___X_X__, XXXXXXXX, ________,
  XXXXXXX_, _______X, X_XXXXXX, _X______, X_X____X, _X_XXXX_, XX_X_X_X, __X_X_X_, _XXXXXXX, X_______,
  XXXX_X_X, ____X_X_, _X_X_XXX, X_X_X___, XX_X__XX, __X_XX__, X_X_X_XX, _X_X_X__, XXXXXXXX, ________,
  XXX_X_X_, ___X_X_X, X_X_X_X_, _X_X_X_X, XXX____X, ___XXXX_, _X_XXXXX, X_X_____, XXXXXXXX, ________,
  XXXX_X_X, ____X_X_, _X_X_X_X, X_X_X_X_, _X______, X_XXXXXX, XXXXXXXX, ________, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, X_______, _XXXXXXX, _X_X_X_X, X_X_X_X_, _X_X_X_X, X_X_X_X_,
  XXXXXXXX, ________, XXXXXX_X, ______X_, _X______, X_XXXXXX, X_X_X_X_, _X_X_X_X, X_X_X_XX, _X_X_X__,
  XXXXXXXX, ________, XXX_X_X_, ___X_X_X, X_______, _XXXXXXX, _XXXXX_X, X_____X_, _X_X_XXX, X_X_X___,
  XXXXXXXX, ________, _X_X_X_X, X_X_X_X_, XX______, __XXXXXX, X_XXXXXX, _X______, X_XXXXXX, _X______,
  XXXXXXXX, ________, XXXXX_XX, _____X__, XXX____X, ___XXXX_, _X_XXXXX, X_X_____, XXXXXXXX, ________,
};

/** $EED1: bitmap_heli_rotor_frame1 (masked: 5 visual columns, 10 raw bytes/row) */
const pixel_t bitmap_heli_rotor_frame1[5 * 2 * 10 * 1] = {
  XXXXXXXX, ________, XXXX_X_X, ____X_X_, XXXX__X_, ____XX_X, X_X_XXXX, _X_X____, XXXXXXXX, ________,
  XXXXXXXX, ________, X_X_X_X_, _X_X_X_X, XXX____X, ___XXXX_, _X_XXXXX, X_X_____, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXX_X_X, ____X_X_, _X_X__X_, X_X_XX_X, X_XXXXXX, _X______, XXX_X_XX, ___X_X__,
  XXXXXXXX, ________, XXXXXXXX, ________, X_X____X, _X_XXXX_, _XXXXX_X, X_____X_, _X_X_X_X, X_X_X_X_,
  XXXXXXXX, ________, XXXXXXXX, ________, XX______, __XXXXXX, X_X_X_X_, _X_X_X_X, X_X_X_XX, _X_X_X__,
  XXX_X_X_, ___X_X_X, X_X_X_X_, _X_X_X_X, X_______, _XXXXXXX, _X_X_X_X, X_X_X_X_, _X_X_X_X, X_X_X_X_,
  XXXX_X_X, ____X_X_, _X_X_X_X, X_X_X_X_, _X______, X_XXXXXX, XXXXXXXX, ________, XXXXXXXX, ________,
  XXXXXXX_, _______X, X_X_X_XX, _X_X_X__, XX______, __XXXXXX, _X_XXXXX, X_X_____, XXXXXXXX, ________,
  XXXXXXXX, ________, _XXXXXXX, X_______, _X______, X_XXXXXX, X_X_X_X_, _X_X_X_X, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXX_, _______X, X_X____X, _X_XXXX_, XX_X_XXX, __X_X___, XXXXXXXX, ________,
};

/** $ECAA: heli_part0 */
const heli_bitmap_t heli_part0 = {
  0x00, { 0, { 3, BITMAPFLAG_MASKED, 10, &bitmap_heli_part0[0], &bitmap_heli_part0[0] } }
};

/** $EC8F: heli_part1_frame0 */
const heli_bitmap_t heli_part1_frame0 = {
  0x0A, { 0, { 3, BITMAPFLAG_DEFAULT, 7, &bitmap_heli_part1_frame0[0], &bitmap_heli_part1_frame0[0] } }
};
/** $ECA1: heli_part1_frame1 */
const heli_bitmap_t heli_part1_frame1 = {
  0x0A, { 0, { 3, BITMAPFLAG_DEFAULT, 7, &bitmap_heli_part1_frame1[0], &bitmap_heli_part1_frame1[0] } }
};

/** $EC86: heli_part2_frame0 */
const heli_bitmap_t heli_part2_frame0 = {
  0x11, { -16, { 7, BITMAPFLAG_DEFAULT, 16, &bitmap_heli_part2_frame0[0], &bitmap_heli_part2_frame0[0] } }
};
/** $EC98: heli_part2_frame1 */
const heli_bitmap_t heli_part2_frame1 = {
  0x11, { -16, { 7, BITMAPFLAG_DEFAULT, 16, &bitmap_heli_part2_frame1[0], &bitmap_heli_part2_frame1[0] } }
};

/** $ECB3: heli_part3_frame0 */
const heli_bitmap_t heli_part3_frame0 = {
  0x0E, { -16, { 2, BITMAPFLAG_MASKED, 3, &bitmap_heli_part3_frame0[0], &bitmap_heli_part3_frame0[0] } }
};
/** $ECC5: heli_part3_frame1 */
const heli_bitmap_t heli_part3_frame1 = {
  0x0C, { -8, { 1, BITMAPFLAG_MASKED, 5, &bitmap_heli_part3_frame1[0], &bitmap_heli_part3_frame1[0] } }
};

/** $ECBC: heli_part4_frame0 */
const heli_bitmap_t heli_part4_frame0 = {
  0x0A, { 24, { 1, BITMAPFLAG_MASKED, 7, &bitmap_heli_part4_frame0[0], &bitmap_heli_part4_frame0[0] } }
};
/** $ECCE: heli_part4_frame1 */
const heli_bitmap_t heli_part4_frame1 = {
  0x0A, { 24, { 2, BITMAPFLAG_MASKED, 7, &bitmap_heli_part4_frame1[0], &bitmap_heli_part4_frame1[0] } }
};

/** $ECD7: heli_rotor_frame0 */
const heli_bitmap_xonly_t heli_rotor_frame0 = {
  -8, { 5, BITMAPFLAG_MASKED, 10, &bitmap_heli_rotor_frame0[0], &bitmap_heli_rotor_frame0[0] }
};
/** $ECDF: heli_rotor_frame1 */
const heli_bitmap_xonly_t heli_rotor_frame1 = {
  -8, { 5, BITMAPFLAG_MASKED, 10, &bitmap_heli_rotor_frame1[0], &bitmap_heli_rotor_frame1[0] }
};

/** $ED93 (stage 2) / $EC6E (stage 4) in the original banks. */
const heli_part_ptr_t heli_table_frame0[SPRITE_FRAMES] = {
  { &heli_part0 }, { &heli_part1_frame0 }, { &heli_part2_frame0 },
  { &heli_part3_frame0 }, { &heli_part4_frame0 },
  { (const heli_bitmap_t *) &heli_rotor_frame0 },
};
/** $ED9F (stage 2) / $EC7A (stage 4) in the original banks. */
const heli_part_ptr_t heli_table_frame1[SPRITE_FRAMES] = {
  { &heli_part0 }, { &heli_part1_frame1 }, { &heli_part2_frame1 },
  { &heli_part3_frame1 }, { &heli_part4_frame1 },
  { (const heli_bitmap_t *) &heli_rotor_frame1 },
};

/* ----------------------------------------------------------------------- */

/** $E2AA: perp_escape_curvature */
const u8 perp_escape_curvature[5] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CMD_GOTO(PERP_ESCAPE_CURVATURE_ADDR) // loop
};

/** $E2AF: perp_escape_height */
const u8 perp_escape_height[5] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_CMD_GOTO(PERP_ESCAPE_HEIGHT_ADDR) // loop
};

/** $E2B8: fork_hazards */
const u8 fork_hazards[8] = {
  MAP_HAZARD_WAIT(12),
  MAP_CMD_ARROW_OFF,
  MAP_HAZARD_WAIT(255),
  MAP_CMD_GOTO(FORK_HAZARDS_ADDR) // loop
};

/** $E2C0: fork_leftrightobjs */
const u8 fork_leftrightobjs[6] = {
  MAP_OBJ_NONE(1),
  MAP_OBJ_SHORT_POLE(15),
  MAP_CMD_GOTO(FORK_LEFTRIGHTOBJS_ADDR + 1) // loop (skipping)
};

/** $E2C6: forked_road_curvature */
const u8 forked_road_curvature[6] = {
  MAP_CURVE_STRAIGHT(1),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CMD_GOTO(FORKED_ROAD_CURVATURE_ADDR + 1) // loop (skipping)
};

/** $E2CC: forked_road_height */
const u8 forked_road_height[5] = {
  MAP_CURVE_LEFT_HARD(15),
  MAP_CMD_GOTO(FORKED_ROAD_HEIGHT_ADDR) // loop
};

/** $E2D1: forked_road_lanes */
const u8 forked_road_lanes[2] = {
  MAP_LANES_FORKED(255)
};

/* ----------------------------------------------------------------------- */

/** $E364: spiral_transition_frames */
const pixel_t spiral_transition_frames[11 * 8] = {
  XXXXXXXX, // 1
  ________,
  ________,
  ________,
  ________,
  ________,
  ________,
  ________,

  XXXXXXXX, // 2
  _______X,
  _______X,
  _______X,
  _______X,
  _______X,
  _______X,
  _______X,

  XXXXXXXX, // 3
  _______X,
  _______X,
  _______X,
  _______X,
  _______X,
  _______X,
  XXXXXXXX,

  XXXXXXXX, // 4
  X______X,
  X______X,
  X______X,
  X______X,
  X______X,
  X______X,
  XXXXXXXX,

  XXXXXXXX,// 5
  XXXXXXXX,
  X______X,
  X______X,
  X______X,
  X______X,
  X______X,
  XXXXXXXX,

  XXXXXXXX, // 6
  XXXXXXXX,
  X_____XX,
  X_____XX,
  X_____XX,
  X_____XX,
  X_____XX,
  XXXXXXXX,

  XXXXXXXX, // 7
  XXXXXXXX,
  X_____XX,
  X_____XX,
  X_____XX,
  X_____XX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXXXXX, // 8
  XXXXXXXX,
  XX____XX,
  XX____XX,
  XX____XX,
  XX____XX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXXXXX, // 9
  XXXXXXXX,
  XXXXXXXX,
  XX____XX,
  XX____XX,
  XX____XX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXXXXX, // 10
  XXXXXXXX,
  XXXXXXXX,
  XX___XXX,
  XX___XXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXXXXX, // 11
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX
};

/** $E3BC: circle_transition_frames */
const pixel_t circle_transition_frames[7 * 8] = {
  ________, // 1
  ________,
  ________,
  ___XX___,
  ___XX___,
  ________,
  ________,
  ________,

  ________, // 2
  ________,
  ___XX___,
  __XXXX__,
  __XXXX__,
  ___XX___,
  ________,
  ________,

  ________, // 3
  ___XX___,
  __XXXX__,
  _XXXXXX_,
  _XXXXXX_,
  __XXXX__,
  ___XX___,
  ________,

  ___XX___, // 4
  __XXXX__,
  _XXXXXX_,
  XXXXXXXX,
  XXXXXXXX,
  _XXXXXX_,
  __XXXX__,
  ___XX___,

  __XXXX__, // 5
  _XXXXXX_,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  _XXXXXX_,
  __XXXX__,

  _XXXXXX_, // 6
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  _XXXXXX_,

  XXXXXXXX, // 7
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX
};

/* ----------------------------------------------------------------------- */

/**
 * $E400 - Road edge/lane markings ($E4xx page, 256 bytes)
 * $E400..$E40F: unused zeros (Z80 "draw nothing" state when stripe offset = 0)
 * $E410..$E4CF: edge markings (six 32-byte masked variants)
 * $E4D0..$E4FF: lane markings (three 16-byte unmasked variants)
 */
const pixel_t edge_markings[256] = {
  // $E400..$E40F: unused zeros
  ________,________,________,________,
  ________,________,________,________,
  ________,________,________,________,
  ________,________,________,________,

  // $E410: Widest edge. White.
  ________,_X______, __XXXXXX,X_______,
  X_______,__X_____, ___XXXXX,_X______,
  XX______,___X____, ____XXXX,__X_____,
  XXX_____,____X___, _____XXX,___X____,
  XXXX____,_____X__, ______XX,____X___,
  XXXXX___,______X_, _______X,_____X__,
  XXXXXX__,_______X, ________,______X_,
  XXXXXXX_,________, ________,X______X,

  // $E430: Widest edge. Black.
  ________,XXXXXXXX, __XXXXXX,X_______,
  ________,_XXXXXXX, ___XXXXX,XX______,
  X_______,__XXXXXX, ____XXXX,XXX_____,
  XX______,___XXXXX, _____XXX,XXXX____,
  XXX_____,____XXXX, ______XX,XXXXX___,
  XXXX____,_____XXX, _______X,XXXXXX__,
  XXXXX___,______XX, ________,XXXXXXX_,
  XXXXXX__,_______X, ________,XXXXXXXX,

  // $E450: Middle edge. White.
  ________,_X______, _XXXXXXX,________,
  X_______,__X_____, __XXXXXX,X_______,
  XX______,___X____, ___XXXXX,_X______,
  XXX_____,____X___, ____XXXX,__X_____,
  XXXX____,_____X__, _____XXX,___X____,
  XXXXX___,______X_, ______XX,____X___,
  XXXXXX__,_______X, _______X,_____X__,
  XXXXXXX_,________, ________,X_____X_,

  // $E470: Middle edge. Black.
  ________,_XXXXXXX, _XXXXXXX,________,
  X_______,__XXXXXX, __XXXXXX,X_______,
  XX______,___XXXXX, ___XXXXX,XX______,
  XXX_____,____XXXX, ____XXXX,XXX_____,
  XXXX____,_____XXX, _____XXX,XXXX____,
  XXXXX___,______XX, ______XX,XXXXX___,
  XXXXXX__,_______X, _______X,XXXXXX__,
  XXXXXXX_,________, ________,XXXXXXX_,

  // $E490: Thinnest edge. White.
  _______X,_X___X__, XXXXXXXX,________,
  X_______,__X___X_, XXXXXXXX,________,
  XX______,___X___X, _XXXXXXX,________,
  XXX_____,____X___, __XXXXXX,X_______,
  XXXX____,_____X__, ___XXXXX,_X______,
  XXXXX___,______X_, ____XXXX,__X_____,
  XXXXXX__,_______X, _____XXX,___X____,
  XXXXXXX_,________, ______XX,X___X___,

  // $E4B0: Thinnest edge. Black.
  _______X,_XXXXX__, XXXXXXXX,________,
  X_______,__XXXXX_, XXXXXXXX,________,
  XX______,___XXXXX, _XXXXXXX,________,
  XXX_____,____XXXX, __XXXXXX,X_______,
  XXXX____,_____XXX, ___XXXXX,XX______,
  XXXXX___,______XX, ____XXXX,XXX_____,
  XXXXXX__,_______X, _____XXX,XXXX____,
  XXXXXXX_,________, ______XX,XXXXX___,

  // $E4D0: Widest lane marking.
  __XXX___,________,
  ___XXX__,________,
  ____XXX_,________,
  _____XXX,________,
  ______XX,X_______,
  _______X,XX______,
  ________,XXX_____,
  ________,_XXX____,

  // $E4E0: Middle lane marking.
  ___XX___,________,
  ____XX__,________,
  _____XX_,________,
  ______XX,________,
  _______X,X_______,
  ________,XX______,
  ________,_XX_____,
  ________,__XX____,

  // $E4F0: Thinnest lane marking.
  ____X___,________,
  _____X__,________,
  ______X_,________,
  _______X,________,
  ________,X_______,
  ________,_X______,
  ________,__X_____,
  ________,___X____,
};

/* ----------------------------------------------------------------------- */

/** $E600 - Vertical perspective weights — Y scale by speed and distance slot */
const u8 persp_y_scale[8][PERSP_TABLE_COLS] = {
  { 0x60, 0x4A, 0x3C, 0x32, 0x2B, 0x25, 0x21, 0x1E, 0x1B, 0x18, 0x16, 0x15, 0x13, 0x12, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x4C, 0x3D, 0x33, 0x2C, 0x26, 0x22, 0x1E, 0x1B, 0x19, 0x17, 0x15, 0x13, 0x12, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x4F, 0x3F, 0x34, 0x2D, 0x27, 0x22, 0x1E, 0x1B, 0x19, 0x17, 0x15, 0x14, 0x12, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x51, 0x41, 0x35, 0x2D, 0x27, 0x23, 0x1F, 0x1C, 0x19, 0x17, 0x15, 0x14, 0x12, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x54, 0x42, 0x37, 0x2E, 0x28, 0x23, 0x1F, 0x1C, 0x1A, 0x17, 0x16, 0x14, 0x13, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x57, 0x44, 0x38, 0x2F, 0x29, 0x24, 0x20, 0x1D, 0x1A, 0x18, 0x16, 0x14, 0x13, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x59, 0x46, 0x39, 0x30, 0x29, 0x24, 0x20, 0x1D, 0x1A, 0x18, 0x16, 0x14, 0x13, 0x12, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x5D, 0x48, 0x3B, 0x31, 0x2A, 0x25, 0x21, 0x1D, 0x1A, 0x18, 0x16, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0A }
};

/**
 * $E88E: transitions
 *
 * Conv: Changed to point at fixed data only. Original game copied two of the
 *       transition frames around during relocation.
 */
const transition_t transitions[8] = {
  /* Forward animations */
  {  6, &square_transition_frames[-1 * 8] },
  { 12, &spiral_transition_frames[-1 * 8] },
  {  8, &circle_transition_frames[-1 * 8] },
  {  7, &diamond_transition_frames[-1 * 8] },

  /* Reverse animations */
  {  6, &square_transition_frames[5 * 8] },
  { 12, &spiral_transition_frames[11 * 8] },
  {  8, &circle_transition_frames[7 * 8] },
  {  7, &diamond_transition_frames[6 * 8] },
};

/** 0xE8A6: square_transition_frames */
const pixel_t square_transition_frames[5 * 8] = {
  ________, // 1
  ________,
  ________,
  ___X____,
  ________,
  ________,
  ________,
  ________,

  ________, // 2
  ________,
  ________,
  ___XX___,
  ___XX___,
  ________,
  ________,
  ________,

  ________, // 3
  ________,
  __XXXX__,
  __XXXX__,
  __XXXX__,
  __XXXX__,
  ________,
  ________,

  ________, // 4
  _XXXXXX_,
  _XXXXXX_,
  _XXXXXX_,
  _XXXXXX_,
  _XXXXXX_,
  _XXXXXX_,
  ________,

  XXXXXXXX, // 5
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX
};

/** 0xE8CE: diamond_transition_frames */
const pixel_t diamond_transition_frames[6 * 8] = {
  X______X, // 1
  _X____X_,
  __X__X__,
  ___XX___,
  ___XX___,
  __X__X__,
  _X____X_,
  X______X,

  X______X, // 2
  XX____XX,
  _XX__XX_,
  __XXXX__,
  __XXXX__,
  _XX__XX_,
  XX____XX,
  X______X,

  XX____XX, // 3
  XXX__XXX,
  _XXXXXX_,
  __XXXX__,
  __XXXX__,
  _XXXXXX_,
  XXX__XXX,
  XX____XX,

  XX____XX, // 4
  XXX__XXX,
  XXXXXXXX,
  _XXXXXX_,
  _XXXXXX_,
  XXXXXXXX,
  XXX__XXX,
  XX____XX,

  XXX__XXX, // 5
  XXXXXXXX,
  XXXXXXXX,
  _XXXXXX_,
  _XXXXXX_,
  XXXXXXXX,
  XXXXXXXX,
  XXX__XXX,

  XXXXXXXX, // 6
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX
};

/* ----------------------------------------------------------------------- */

/** $F0FE: music_patterns */
const u8 music_patterns[19] = {
  // (repetitions, offset)
  0x01, 0x29, // single noise hit
  0x01, 0x2C, // restart is here: noise-only hi-hat pattern, ending in a drum1/noise flourish
  0x04, 0x00, // drum2/drum1/noise beat
  0x04, 0x71, // drum2/drum1/noise beat
  0x04, 0x91, // drum2/drum1/noise beat
  0x04, 0xAD, // drum2/drum1/noise beat
  0x07, 0xCD, // drum2/drum1/noise beat
  0x01, 0xED, // drum2/drum1/noise beat
  0xFF, // stop marker
  TWOBYTES(0xF100) // restart address
};

/**
 * $F111: music_data
 *
 * Layout: each block is a delay-reload byte (ticks per note) followed by a
 * note stream, terminated by NOTE_END. NOTE_* macros are defined in
 * Internal.h. Same format as es_music_data (Bank7.c), but this table also
 * uses bit7 "extra delay" notes (NOTE_XDELAY). See play_music_48k (Main.c)
 * for the byte-level decode this is built from.
 */
const u8 music_data[271] = {
  // 0x00 (delay=5) - drum2/drum1/noise beat
  NOTE_DELAY(5),
  NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(3), NOTE_NOISE(3), NOTE_DRUM1(8), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(3), NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_XDELAY(NOTE_DRUM2(8)),
  NOTE_DRUM1(1), NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_NOISE(3),
  NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3),
  NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(3),
  NOTE_NOISE(3), NOTE_DRUM1(8), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(3),
  NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_XDELAY(NOTE_DRUM1(8)), NOTE_DRUM1(1),
  NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_DRUM1(8),
  NOTE_DRUM1(8), NOTE_DRUM1(8), NOTE_DRUM2(8),
  NOTE_END,

  // 0x29 (delay=5) - single noise hit
  NOTE_DELAY(5),
  NOTE_NOISE(9),
  NOTE_END,

  // 0x2C (delay=5) - noise-only hi-hat pattern, ending in a drum1/noise flourish
  NOTE_DELAY(5),
  NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(9),
  NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3),
  NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(9),
  NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(3),
  NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(9),
  NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3),
  NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(3),
  NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(9),
  NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(9),
  NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3),
  NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(9),
  NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(3),
  NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(9),
  NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3),
  NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(8), NOTE_XDELAY(NOTE_NOISE(9)), NOTE_DRUM1(8), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(8), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8),
  NOTE_END,

  // 0x71 (delay=5) - drum2/drum1/noise beat
  NOTE_DELAY(5),
  NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_DRUM1(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(3),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(3), NOTE_XDELAY(NOTE_DRUM1(3)), NOTE_DRUM2(8),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1),
  NOTE_NOISE(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(1), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM2(8), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(3), NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_DRUM1(3), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8),
  NOTE_END,

  // 0x91 (delay=5) - drum2/drum1/noise beat
  NOTE_DELAY(5),
  NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_DRUM1(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(3),
  NOTE_NOISE(3), NOTE_XDELAY(NOTE_DRUM1(1)), NOTE_DRUM2(8), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(3),
  NOTE_NOISE(3), NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_NOISE(3), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1),
  NOTE_END,

  // 0xAD (delay=5) - drum2/drum1/noise beat
  NOTE_DELAY(5),
  NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_DRUM1(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(3),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1), NOTE_XDELAY(NOTE_DRUM1(3)), NOTE_DRUM2(8),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1),
  NOTE_NOISE(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(3), NOTE_NOISE(3),
  NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_DRUM1(1), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1),
  NOTE_END,

  // 0xCD (delay=5) - drum2/drum1/noise beat
  NOTE_DELAY(5),
  NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_DRUM1(1), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1), NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_NOISE(3),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_XDELAY(NOTE_DRUM1(1)),
  NOTE_NOISE(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(1), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM2(8), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(3), NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_DRUM1(3), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8),
  NOTE_END,

  // 0xED (delay=5) - drum2/drum1/noise beat
  NOTE_DELAY(5),
  NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_DRUM1(1), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1), NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_NOISE(3),
  NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_XDELAY(NOTE_DRUM1(1)),
  NOTE_NOISE(3), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(1), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(8), NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_DRUM1(8), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(8), NOTE_XDELAY(NOTE_DRUM2(8)), NOTE_DRUM1(8), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(8), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8), NOTE_XDELAY(NOTE_NOISE(3)),
  NOTE_DRUM1(8), NOTE_XDELAY(NOTE_NOISE(3)), NOTE_DRUM1(8), NOTE_DRUM1(8),
  NOTE_END,
};

/* ----------------------------------------------------------------------- */

/** $EE26: sinclair_joy_keydefs */
const u8 sinclair_joy_keydefs[5] = {
  KEYDEF(zxkey_0),
  KEYDEF(zxkey_9),
  KEYDEF(zxkey_8),
  KEYDEF(zxkey_6),
  KEYDEF(zxkey_7)
};

/** $EE38: temp_keydefs_template -- the assembled contents of the 48K version's
 * scratch keydef buffer, i.e. the keyboard scheme's default key assignments.
 * Layout is [0..4] = gear/accelerate/brake/left/right, [5..7] =
 * quit/pause/turbo. Byte-for-byte identical to the 128K version's
 * default_control_keys[] ($FFF7, Bank3.c); stop_the_tape_48k copies these into
 * state->keydefs when the player picks a scheme that does not redefine them. */
const u8 temp_keydefs_template[8] = {
  KEYDEF(zxkey_N),     // gear
  KEYDEF(zxkey_A),     // accelerate
  KEYDEF(zxkey_Z),     // brake
  KEYDEF(zxkey_K),     // left
  KEYDEF(zxkey_L),     // right
  KEYDEF(zxkey_Q),     // quit
  KEYDEF(zxkey_P),     // pause
  KEYDEF(zxkey_SPACE)  // turbo
};

/* ----------------------------------------------------------------------- */

/* Deduplicated graphics: byte-identical across two or more of
 * Stage1Data.c-Stage5Data.c in the original binary; kept once here. */

/** $5D39 (stage1) / $E139 (stage2) / $C139 (stage3) / $E139 (stage4) / $C139 (stage5): perp_description */
const u8 perp_description[7] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_PERP_DESC_1,
  CHATTERSTR_PERP_DESC_2,
  CHATTERSTR_PERP_DESC_3,
  CHATTERSTR_PERP_DESC_4,
  CHATTERCMD_PAUSE,
  CHATTERBLK_HEROES_ACKNOWLEDGE
};

/** $64BC (stage1) / $C960 (stage5): bitmap_lambo_1 */
const pixel_t bitmap_lambo_1[6 * 1 * 30 * 1] = {
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

/** $6570 (stage1) / $CA14 (stage5): bitmap_lambo_2 */
const pixel_t bitmap_lambo_2[5 * 1 * 22 * 1] = {
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

/** $65DE (stage1) / $CA82 (stage5): bitmap_lambo_3 */
const pixel_t bitmap_lambo_3[3 * 1 * 15 * 1] = {
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

/* Conv: port-added masked variants of bitmap_lambo_{1,2,3}, built under
   CHQ_ENABLE_MASKED_VEHICLES. Hand-authored silhouette mask painted onto the
   exported sheet via scripts/graphics_png.py, see docs/graphics-png-format.md. */
#ifdef CHQ_ENABLE_MASKED_VEHICLES
const pixel_t bitmap_lambo_1_masked[6 * 2 * 30 * 1] = {
  XXXXX___, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XXXX____,
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX,
  X_______, _XXXXXX_, ________, _X_XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXX_X, ________, __XXXXXX,
  X_______, _XXXX__X, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XX__XXXX,
  XX______, __XXX_XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXX_XXX_,
  XXX_____, ___XXXX_, ________, X_X_X_XX, ________, X_X_X_X_, ________, X_X_X_X_, ________, XXX_X_X_, ______XX, X_XXXX__,
  XXX_____, ___X_X__, ________, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXX____, ______XX, ___X_X__,
  XX______, __X___X_, ________, X_XXXXXX, ________, _XX_XXXX, ________, XXXXXXX_, ________, XX_XXXX_, _______X, X_X___X_,
  XX______, __X_XXXX, ________, XXXXXXX_, ________, _X__XXX_, ________, X_X_XX__, ________, X__XXXXX, _______X, XXXXX_X_,
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXX_X, ________, _X_X_XXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  XX______, __XXXXXX, ________, XXXX_XXX, ________, X_X__X__, ________, ____X_X_, ________, XXXXXXXX, _______X, XXXXXXX_,
  XX______, __XX_XXX, ________, __X_X_X_, ________, X_______, ________, ________, ________, X_XXXXXX, _______X, XXXXXXX_,
  XX______, __X_X_X_, ________, _X_X_XXX, ________, ________, ________, ________, ________, _XX____X, _______X, XXXXXXX_,
  XX______, __X___X_, ________, XXXXXXXX, ________, _X_X_X_X, ________, _X_X_X_X, ________, _X_XXXX_, _______X, _XXXXXX_,
  XX______, __X__X_X, ________, XX____XX, ________, X_X_X_X_, ________, X_X_X_X_, ________, X_X____X, _______X, X__XX_X_,
  X_______, _X_X_XXX, ________, __X_X__X, ________, XXXXXXXX, ________, XXXXXXXX, ________, XX__X_X_, ________, _XX_XX_X,
  X_______, _XX_XX__, ________, _______X, ________, __X_X_X_, ________, X_X_X_X_, ________, _X______, ________, ___XX_XX,
  X_______, _XXX_X__, ________, _______X, ________, __XX_X_X, ________, _X_X_XX_, ________, _X______, ________, ___XXXXX,
  X_______, _XXX_XXX, ________, _X_X_X_X, ________, XXXXXXXX, ________, XXXXXXXX, ________, XX_X_X_X, ________, _XXXXXXX,
  XX______, __XXX__X, ________, XX____XX, ________, X_______, ________, ________, ________, XXX____X, _______X, XX_XXXX_,
  XXXX____, ____X___, ________, _XXXXXXX, ________, XXX_X_X_, ________, X_X_X_XX, ________, XXXXXXXX, _____XXX, ____X___,
  XXXX____, ____XX_X, ________, ______XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXX_____, _____XXX, _X_XX___,
  XXXXXX__, ______XX, ________, X_XX___X, ________, _X_X_X_X, ________, _X_X_X_X, ________, _X___XX_, ___XXXXX, XXX_____,
  XX___XXX, __XXX___, ________, XXXXXXXX, ________, XXX_____, ________, ______XX, ________, XXXXXXXX, _XXX___X, X___XXX_,
  X_____XX, _X__XX__, X_______, _XXX_X_X, ________, _XXXXXXX, ________, XXXXXXXX, ________, _X_X_XXX, XXX_____, ___XX__X,
  X_______, _X_XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXX_X,
  X_______, _X__X___, ________, ________, ________, ________, ________, ________, ________, ________, ________, ____X__X,
  X_______, _XX__XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXX__XX,
  XXX___XX, ___XXX__, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXX___XX, ___XXX__,
};

const pixel_t bitmap_lambo_2_masked[5 * 2 * 22 * 1] = {
  XXXXX___, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  XX______, __XX__X_, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, ______XX, X__XXX__,
  XX______, __XX_XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XX_XX___,
  XXX_____, ___XX___, ________, ___XXXXX, ________, XXXXXXXX, ________, XXXX____, ____XXXX, __XX____,
  XX______, __X__X_X, ________, _XXXX_XX, ________, _XXXXXXX, ________, _XX_XX_X, _____XXX, _X__X___,
  XX______, __X_XXXX, ________, XXXX__X_, ________, _XX__XX_, ________, _X__XXXX, _____XXX, XXX_X___,
  XX______, __XXXXXX, ________, _X_XXXXX, ________, XX____XX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XX______, __XX_XX_, ________, __X__X_X, ________, _______X, ________, _X_XXXXX, _____XXX, XXXXX___,
  XX______, __X_X___, ________, _XXXX___, ________, ________, ________, __XXXXXX, _____XXX, XXXXX___,
  XX______, __X____X, ________, X___XX_X, ________, _X_X_X_X, ________, _XX___XX, _____XXX, X_X_X___,
  XXX_____, ___X_XX_, ________, __X_XXXX, ________, XXXXXXXX, ________, XXX_X___, ______XX, XX_X_X__,
  XX______, __XX_X__, ________, ____X__X, ________, _X_X_X_X, ________, __X_____, ______XX, _XXXXX__,
  XX______, __XX_XXX, ________, ___XXXXX, ________, XXXXXXXX, ________, XXXX___X, ______XX, XXXXXX__,
  XXXX____, ____X___, ________, XXXXXXX_, ________, X_X_X_X_, ________, XXXXXXX_, ____XXXX, ___X____,
  XXXXX___, _____XX_, ________, ____XXXX, ________, XXXXXXXX, ________, XXXX____, ___XXXXX, _XX_____,
  XXXXXX__, ______XX, ________, _XX_XXXX, ________, _X__X__X, ________, _XXX_XX_, __XXXXXX, XX______,
  XX_XX___, __X__X__, ________, XX_XX_XX, ________, XXXXXXXX, ________, XX_XX_XX, ___XX_XX, __X__X__,
  XX_X____, __X_XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____X_XX, XXXX_X__,
  XX______, __XX_XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXX_XX__,
  XXX__XXX, ___XX___, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XX__XXXX, __XX____,
};

const pixel_t bitmap_lambo_3_masked[3 * 2 * 15 * 1] = {
  X_______, _XXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  ________, XXX__XXX, ________, XXXXXXXX, ________, XXX__XXX,
  ________, XX_XXXXX, ________, XXXXXXXX, ________, XXXXX_XX,
  X_______, _XX___XX, ________, XXXXXXXX, _______X, XX___XX_,
  ________, XX_XXXXX, ________, _X_XX_X_, _______X, XXXXX_X_,
  ________, XXXXXXXX, ________, XXX__XXX, ________, XXXXXXXX,
  ________, XX__XXXX, ________, ________, ________, _XXXXXXX,
  ________, X__XX__X, ________, XXXXXXXX, ________, X__XXXXX,
  ________, X_X____X, ________, ________, ________, X____XXX,
  ________, X_XXX_XX, ________, XXXXXXXX, ________, XX_XXXXX,
  X_______, _XX__XX_, ________, XXXXXXXX, _______X, _XXX_XX_,
  XXX_____, ___X___X, ________, X_X__X_X, _____XXX, X_XXX___,
  ________, XXX_X_X_, ________, XXXXXXXX, ________, XXXX_XXX,
  ________, X_XXXXXX, ________, XXXXXXXX, ________, XXXXXX_X,
  X__XXXXX, _XX_____, XXXXXXXX, ________, XXXXX__X, _____XX_,
};
#endif

/** $660B (stage1) / $CB0F (stage5): bitmap_truck_1 */
const pixel_t bitmap_truck_1[6 * 1 * 39 * 1] = {
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

/** $66F5 (stage1) / $CBF9 (stage5): bitmap_truck_2 */
const pixel_t bitmap_truck_2[5 * 1 * 29 * 1] = {
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

/** $6786 (stage1) / $CC8A (stage5): bitmap_truck_3 */
const pixel_t bitmap_truck_3[3 * 1 * 20 * 1] = {
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

/* Conv: port-added masked variants of bitmap_truck_{1,2,3}, built under
   CHQ_ENABLE_MASKED_VEHICLES. Hand-authored silhouette mask painted onto the
   exported sheet via scripts/graphics_png.py, see docs/graphics-png-format.md. */
#ifdef CHQ_ENABLE_MASKED_VEHICLES
const pixel_t bitmap_truck_1_masked[6 * 2 * 39 * 1] = {
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XXXX____,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  X_______, _XXXXX__, ________, X_X_XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXX_X_X, _______X, __XXXXX_,
  X_______, _XXXX__X, ________, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, _______X, X__XXXX_,
  X_______, _XXXX_XX, ________, XXXXXXXX, ________, XX______, ________, _______X, ________, XXXXXXXX, _______X, XX_XXXX_,
  XX______, __XXX_XX, ________, XXXXXXXX, ________, XX______, ________, _______X, ________, XXXXXXXX, ______XX, XX_XXX__,
  XX______, __XXX_XX, ________, XXXXX___, ________, XXX_____, ________, ______XX, ________, X___XXXX, ______XX, XX_XXX__,
  XXX_____, ___XX_XX, ________, XXXX_X_X, ________, XX_X_X_X, ________, _X_X_X_X, ________, XX_X_XXX, _____XXX, XX_XX___,
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XXXX____,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXX_____, ___X____, ________, ________, ________, ________, ________, ________, ________, ________, _____XXX, ____X___,
  XX______, __X_X_X_, ________, X_XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXX_X, ______XX, _X_X_X__,
  XX______, __XX_X_X, ________, _X___X_X, ________, _X_X_X_X, ________, _X_X_X_X, ________, _X_X__X_, ______XX, X_X_XX__,
  XX______, __X_XXX_, ________, ________, ________, ________, ________, ________, ________, ________, ______XX, _XXX_X__,
  XX______, __XXXX__, ________, ________, ________, ________, ________, ________, ________, ________, ______XX, __XXXX__,
  X_______, _X___XX_, ________, ________, ________, ________, ________, ________, ________, ________, _______X, _XX___X_,
  X_______, _X___X__, ________, ________, ________, ________, ________, ________, ________, ________, _______X, __X___X_,
  X_______, _X___XX_, ________, ________, ________, ________, ________, ________, ________, ________, _______X, _XX___X_,
  XX______, __XXXXX_, ________, X_X_X_X_, ________, X_X_X_X_, ________, X_X_X_X_, ________, X_X_X_X_, _______X, X_XXXXX_,
  X_______, _X___XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXX___X_,
  X_______, _X______, ________, ________, ________, ________, ________, ________, ________, ________, _______X, ______X_,
  X_______, _X______, ________, ________, ________, ________, ________, ________, ________, ________, _______X, ______X_,
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  XXX_____, ___XXXX_, ________, _XX_X_X_, ________, X_X_X_XX, ________, X_X_X_X_, ________, X_X_X_X_, _____XXX, _XXXX___,
  XXXXXX__, ______XX, ________, X_XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXX_X, __XXXXXX, XX______,
  XXXXXX__, ______XX, ________, XX______, ________, XX_____X, ________, X_____XX, ________, ______XX, __XXXXXX, XX______,
  XXXXX___, _____XX_, ________, __XXXXXX, ________, __XXXXX_, ________, _XXXXX__, ________, XXXXXX__, ___XXXXX, _XX_____,
  XXXX____, ____X_XX, ________, X_X_X_XX, ________, __X_X_X_, ________, _X_X_X__, ________, XX_X_X_X, ____XXXX, XX_X____,
  XXXX____, ____X__X, ________, _X_XXXXX, ________, __XXXXX_, ________, _XXXXX__, ________, XXXXX_X_, ____XXXX, X__X____,
  XXX_____, ___X___X, ________, _X_XXXXX, ________, __XXXXX_, ________, _XXXXX__, ________, XXXXX_X_, _____XXX, X___X___,
  XXX_____, ___X___X, ________, _X_XXXXX, ________, __XXXXX_, ________, _XXXXX__, ________, XXXXX_X_, _____XXX, X___X___,
  XXX_____, ___X___X, ________, __X_XXXX, ________, __XXXXX_, ________, _XXXXX__, ________, XXXX_X__, _____XXX, X___X___,
  XXX_____, ___X___X, ________, X_X_XXXX, ________, __XXXXX_, ________, _XXXXX__, ________, XXXX_X_X, _____XXX, X___X___,
  XXXX____, ____X_X_, ________, _XX_XXXX, ________, __XXXXX_, ________, _XXXXX__, ________, XXXX_XX_, ____XXXX, _X_X____,
  XXXXX___, _____XXX, ________, X__X_XXX, ________, __XXXXX_, ________, _XXXXX__, ________, XXX_X__X, ___XXXXX, XXX_____,
  XXXXXXXX, ________, X_______, _XXX____, ________, ________, ________, ________, _______X, ____XXX_, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXX____, ____X___, ________, ________, ________, ________, ____XXXX, ___X____, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXX___, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____, XXXXXXXX, ________,
};

const pixel_t bitmap_truck_2_masked[5 * 2 * 29 * 1] = {
  XXXXX___, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XXXX____,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  XXX_____, ___XXX_X, ________, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, ______XX, X_XXXX__,
  XXX_____, ___XX_XX, ________, XXXXXXXX, ________, ________, ________, _XXXXXXX, ______XX, XX_XXX__,
  XXX_____, ___XX_XX, ________, XXXX__XX, ________, ________, ________, _XX__XXX, _____XXX, XX_XX___,
  XXXX____, ____X_XX, ________, XXX_XXXX, ________, X_X___X_, ________, XXXXX_XX, ____XXXX, XX_X____,
  XXXXX___, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXXX____, ____X___, ________, ________, ________, ________, ________, ________, ____XXXX, ___X____,
  XXX_____, ___X_XX_, ________, X_XXXXXX, ________, XXXXXXXX, ________, XXXXXX_X, _____XXX, _XX_X___,
  XXX_____, ___X_X_X, ________, _X______, ________, ________, ________, ______X_, _____XXX, X_X_X___,
  XXX_____, ___XXX__, ________, ________, ________, ________, ________, ________, _____XXX, __XXX___,
  XX______, __X___X_, ________, ________, ________, ________, ________, ________, ______XX, _X___X__,
  XX______, __X___X_, ________, ________, ________, ________, ________, ________, ______XX, _X___X__,
  XX______, __XXXXX_, ________, X_X_X_X_, ________, X_X__X_X, ________, _X_X_X_X, ______XX, __XXXX__,
  XX______, __X_____, ________, ________, ________, ________, ________, ________, ______XX, _____X__,
  XX______, __X_____, ________, ________, ________, ________, ________, ________, ______XX, _____X__,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXXXX__, ______XX, ________, X_XXXXXX, ________, XXXXXXXX, ________, XXXXXX_X, __XXXXXX, XX______,
  XXXXXX__, ______XX, ________, XX____X_, ________, ___XX___, ________, _X____XX, __XXXXXX, XX______,
  XXXXX___, _____XXX, ________, X__X_X_X, ________, X_X__X_X, ________, X_X_X__X, ___XXXXX, XXX_____,
  XXXXX___, _____X_X, ________, __XXXX_X, ________, XXX__XXX, ________, X_XXXX__, ___XXXXX, X_X_____,
  XXXX____, ____X__X, ________, __XXXX_X, ________, XXX__XXX, ________, X_XXXX__, ____XXXX, X__X____,
  XXXX____, ____X__X, ________, _X_XXX_X, ________, XXX__XXX, ________, X_XXX_X_, ____XXXX, X__X____,
  XXXX____, ____X__X, ________, XX_XXX_X, ________, XXX__XXX, ________, X_XXX_XX, ____XXXX, X__X____,
  XXXXXX__, ______XX, ________, X_X_XX_X, ________, XXX__XXX, ________, X_XX_X_X, __XXXXXX, XX______,
  XXXXXXXX, ________, X_______, _XX_____, ________, ________, _______X, _____XX_, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ___X____, ________, ________, _____XXX, ____X___, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXX____, ____XXXX, ________, XXXXXXXX, ____XXXX, XXXX____, XXXXXXXX, ________,
};

const pixel_t bitmap_truck_3_masked[3 * 2 * 20 * 1] = {
  X_______, _XXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  ________, XXX_XXXX, ________, XXXXXXXX, ________, XXXX_XXX,
  ________, XX_XXXXX, ________, X______X, ________, XXXXX_XX,
  X_______, _X_XXXX_, ________, XX____XX, _______X, _XXXX_X_,
  XX______, __XXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  X_______, _X______, ________, ________, _______X, ______X_,
  X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX,
  ________, X_X_X___, ________, ________, ________, ___X_X_X,
  ________, X_X_____, ________, ________, ________, _____X_X,
  ________, _XX_X_X_, ________, X_X_X_X_, ________, X_X_XXX_,
  ________, X_______, ________, ________, ________, _______X,
  X_______, _XXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  XXX_____, ___XXX__, ________, ________, _____XXX, ___XX___,
  XXX_____, ___X___X, ________, X_XX_XX_, ______XX, XX___X__,
  XX______, __X_XX_X, ________, X_XX_XX_, _______X, XX_XX_X_,
  XX______, __X_X__X, ________, X_XX_XX_, _______X, XX__X_X_,
  XX______, __X_X__X, ________, X_XX_XX_, _______X, XX__X_X_,
  XXX_____, ___XXXX_, ________, X_XX_XX_, ______XX, X_XXXX__,
  XXXXXXX_, _______X, ________, ________, __XXXXXX, _X______,
  XXXXXXXX, ________, ________, XXXXXXXX, _XXXXXXX, X_______,
};
#endif

/** $67C2 (stage1) / $CC2D (stage3): bitmap_car_1 */
const pixel_t bitmap_car_1[6 * 1 * 31 * 1] = {
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

/** $687C (stage1) / $CCE7 (stage3): bitmap_car_2 */
const pixel_t bitmap_car_2[5 * 1 * 22 * 1] = {
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

/** $68EA (stage1) / $CD55 (stage3): bitmap_car_3 */
const pixel_t bitmap_car_3[3 * 1 * 16 * 1] = {
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

/* Conv: port-added masked variants of bitmap_car_{1,2,3}, built under
   CHQ_ENABLE_MASKED_VEHICLES. Hand-authored silhouette mask painted onto the
   exported sheet via scripts/graphics_png.py, see docs/graphics-png-format.md. */
#ifdef CHQ_ENABLE_MASKED_VEHICLES
const pixel_t bitmap_car_1_masked[6 * 2 * 31 * 1] = {
  XXXXXXXX, ________, XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XXXX____, XXXXXXXX, ________,
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  X_______, _XXXXX__, ________, X_X_XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXX_X_X_, ______XX, _XXXXX__,
  ________, XXXXX_XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, X_XXXXX_,
  ________, XXXXX_XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, X_XXXXX_,
  ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  X_______, _XX_X_X_, ________, X_X_X_X_, ________, X_X_X_X_, ________, X_X_X_X_, ________, X_X_X_X_, _______X, X_X_XXX_,
  X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  X_______, _X______, ________, ________, ________, ________, ________, ________, ________, ________, ______XX, _____X__,
  X_______, _X______, ________, ________, ________, ________, ________, ________, ________, ________, ______XX, _____X__,
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XX______, __X_____, ________, ________, ________, ________, ________, ________, ________, ________, _____XXX, ____X___,
  X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  X_______, _X__XXXX, ________, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXX_X, ______XX, XXX__X__,
  X_______, _X__XXXX, ________, _X_X_X_X, ________, _X______, ________, _____X_X, ________, _X_X_X_X, ______XX, XXX__X__,
  X_______, _X__XXXX, ________, __X_X_X_, ________, XX______, ________, _____XX_, ________, X_X_X_X_, ______XX, XXX__X__,
  XX______, __XXXXX_, ________, _X_X_X_X, ________, _X______, ________, _____X_X, ________, _X_X_X__, ______XX, XXXXXX__,
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XX______, __X_XX__, ________, ________, ________, ________, ________, ________, ________, ________, _____XXX, _XX_X___,
  XXX_____, ___X_XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XX_X____,
  XX______, __X_XX__, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, _____XXX, _XX_X___,
  X_______, _X___XX_, ________, ________, ________, ________, ________, ________, ________, ________, ______XX, XX___X__,
  X_______, _X____XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, X____X__,
  XX______, __XXXXXX, ________, X_X_____, ________, ________, ________, ___XXXXX, ________, ____X_XX, _____XXX, XXXXX___,
  XXXXXXX_, _______X, ________, XX_X____, ________, ________, ________, __XXXXXX, ________, ___X_XXX, XXXXXXXX, ________,
  XXXXXXXX, ________, ________, XX__X___, ________, ________, ________, __XXXXXX, _______X, __X__XX_, XXXXXXXX, ________,
  XXXXXXXX, ________, X_______, _XX__XX_, ________, ________, ________, ___XXXX_, ______XX, XX__XX__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ___X___X, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, ___XX___, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXX____, ____XX__, ________, ________, ________, ________, ___XXXXX, _XX_____, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXX__, ______XX, ________, XXXXXXXX, ________, XXXXXXXX, _XXXXXXX, X_______, XXXXXXXX, ________,
};

const pixel_t bitmap_car_2_masked[5 * 2 * 22 * 1] = {
  XXXXXXXX, ________, X_______, _XXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___, XXXXXXXX, ________,
  XXXXX___, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XX______, __XXXX__, ________, X_XXXXXX, ________, XXXXXXXX, ________, XXXXX_X_, _____XXX, _XXXX___,
  X_______, _XXXX_XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, X_XXXX__,
  X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  XX______, __XX_X_X, ________, _X_X_X_X, ________, _X_X_X_X, ________, _X_X_X_X, ______XX, _X_XXX__,
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XX______, __X_____, ________, ________, ________, ________, ________, ________, _____XXX, ____X___,
  XXX_____, ___X_X_X, ________, _X_X_X_X, ________, _X_X_X_X, ________, _X_X_X_X, ____XXXX, _X_X____,
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XX______, __X__XX_, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, _____XXX, XX__X___,
  XX______, __X__XX_, ________, _X_X_X__, ________, ________, ________, XX_X_X__, _____XXX, XX__X___,
  XXX_____, ___XXXX_, ________, X_X_XX__, ________, ________, ________, X_X_X_X_, _____XXX, XXXXX___,
  XXX_____, ___XXX_X, ________, _X_X_X_X, ________, _X_X_X_X, ________, _X_X_X_X, ____XXXX, _XXX____,
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXX_____, ___X_X__, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, ____XXXX, _X_X____,
  XX______, __X__XXX, ________, X_______, ________, ________, ________, ______XX, _____XXX, XX__X___,
  XXX_____, ___XXXXX, ________, _X______, ________, ______XX, ________, XX___X_X, ____XXXX, XXXX____,
  XXXXXXX_, _______X, ________, X_X_____, ________, _____XXX, ________, XX__X_XX, XXXXXXXX, ________,
  XXXXXXXX, ________, ________, XX_XX___, ________, ______XX, _______X, X_XX_XX_, XXXXXXXX, ________,
  XXXXXXXX, ________, XX______, __X__XXX, ________, XXXXXXXX, _____XXX, XX__X___, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ___XXXXX, ________, XXXXXXXX, ____XXXX, XXXX____, XXXXXXXX, ________,
};

const pixel_t bitmap_car_3_masked[3 * 2 * 16 * 1] = {
  XXXXX___, _____XXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  X_______, _XXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  ________, XXX__XXX, ________, XXXXXXXX, ________, XXX__XXX,
  ________, XX_XXXXX, ________, XXXXXXXX, ________, XXXXX_XX,
  ________, _XX_X_X_, ________, X_XX_X_X, ________, _X_X_XX_,
  ________, X_______, ________, ________, ________, _______X,
  ________, _XX_XXXX, ________, XXXXXXXX, ________, XXXX_XX_,
  ________, X_XXX_X_, ________, X______X, ________, _X_XXX_X,
  ________, X_XX_X_X, ________, _X_XX_X_, ________, X_X_XX_X,
  X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_,
  X_______, _XX_XXXX, ________, XXXXXXXX, ________, XXXXX_X_,
  X_______, _X_X____, ________, ________, ________, _____X_X,
  XX______, __XXX_X_, ________, ______XX, _______X, __X_XXX_,
  XXXX____, ____XX_X, ________, _____XXX, _____XXX, _X_XX___,
  XXXXX___, _____XX_, ________, _XXXXXXX, ____XXXX, __XX____,
  XXXXXXX_, _______X, ________, XXXXXXXX, __XXXXXX, XX______,
};
#endif

/** $691A (stage1) / $CAAF (stage5): bitmap_lambo_4 */
const pixel_t bitmap_lambo_4[3 * 2 * 8 * 1] = {
  ________, XXXXXXXX, _______X, XXXXXXX_, XXXXXXXX, ________,
  X_______, _XXXXXXX, ______XX, XXXXXX__, XXXXXXXX, ________,
  ________, XX_XXX_X, _______X, _X_XXXX_, XXXXXXXX, ________,
  ________, X_XX_XXX, _______X, XXX_XXX_, XXXXXXXX, ________,
  ________, X_X__XX_, _______X, X_X__XX_, XXXXXXXX, ________,
  X_______, _X_XXXXX, ______XX, XXXXXX__, XXXXXXXX, ________,
  _X______, X_XXX_X_, _____X_X, X_XXX_X_, XXXXXXXX, ________,
  ________, XXXXXXXX, _______X, XXXXXXX_, XXXXXXXX, ________,
};

/** $694A (stage1) / $CADF (stage5): bitmap_lambo_4s */
const pixel_t bitmap_lambo_4s[3 * 2 * 8 * 1] = {
  XXXX____, ____XXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXXXX___, _____XXX, ________, XXXXXXXX, __XXXXXX, XX______,
  XXXX____, ____XX_X, ________, XX_X_X_X, ___XXXXX, XXX_____,
  XXXX____, ____X_XX, ________, _XXXXXX_, ___XXXXX, XXX_____,
  XXXX____, ____X_X_, ________, _XX_X_X_, ___XXXXX, _XX_____,
  XXXXX___, _____X_X, ________, XXXXXXXX, __XXXXXX, XX______,
  XXXX_X__, ____X_XX, ________, X_X_X_XX, _X_XXXXX, X_X_____,
  XXXX____, ____XXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
};

/** $697A (stage1) / $CCC6 (stage5): bitmap_truck_4 */
const pixel_t bitmap_truck_4[2 * 2 * 12 * 1] = {
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

/** $69AA (stage1) / $CCF6 (stage5): bitmap_truck_4s */
const pixel_t bitmap_truck_4s[2 * 2 * 12 * 1] = {
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

/** $69DA (stage1) / $CD85 (stage3): bitmap_car_4 */
const pixel_t bitmap_car_4[3 * 2 * 9 * 1] = {
  X_______, _XXXXXXX, _______X, XXXXXXX_, XXXXXXXX, ________,
  ________, XXX_XXXX, ________, XXXX_XXX, XXXXXXXX, ________,
  ________, XX_X_X_X, ________, _X_X__XX, XXXXXXXX, ________,
  X_______, _X__X_X_, _______X, X_X___X_, XXXXXXXX, ________,
  X_______, _XXX_XXX, _______X, XX_XXXX_, XXXXXXXX, ________,
  X_______, _XX_____, _______X, _____XX_, XXXXXXXX, ________,
  X_______, _X_XXXXX, _______X, XXXXX_X_, XXXXXXXX, ________,
  XX___XXX, __X_X___, X_X___XX, _X_X_X__, XXXXXXXX, ________,
  XXXX____, ____XXXX, ____XXXX, XXXX____, XXXXXXXX, ________,
};

/** $6A10 (stage1) / $CDBB (stage3): bitmap_car_4s */
const pixel_t bitmap_car_4s[3 * 2 * 9 * 1] = {
  XXXXX___, _____XXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXXX____, ____XXX_, ________, XXXXXXXX, ____XXXX, _XXX____,
  XXXX____, ____XX_X, ________, _X_X_X_X, ____XXXX, __XX____,
  XXXXX___, _____X__, ________, X_X_X_X_, ___XXXXX, __X_____,
  XXXXX___, _____XXX, ________, _XXXXX_X, ___XXXXX, XXX_____,
  XXXXX___, _____XX_, ________, ________, ___XXXXX, _XX_____,
  XXXXX___, _____X_X, ________, XXXXXXXX, ___XXXXX, X_X_____,
  XXXXXX__, ______X_, _XXXX_X_, X____X_X, __XXXXXX, _X______,
  XXXXXXXX, ________, ________, XXXXXXXX, XXXXXXXX, ________,
};

/** $6A9A (stage1) / $CE45 (stage3): bitmap_stones_1 */
const pixel_t bitmap_stones_1[2 * 2 * 5 * 1] = {
  ________, XXXXXXXX, ___XXXXX, XXX_____,
  ________, X_____X_, ____XXXX, XXXX____,
  ________, X______X, ___XXXXX, _XX_____,
  X_______, _XX___XX, __XXXXXX, XX______,
  XXX_____, ___XXXXX, XXXXXXXX, ________,
};

/** $6AAE (stage1) / $CE59 (stage3): bitmap_stones_2 */
const pixel_t bitmap_stones_2[2 * 2 * 4 * 1] = {
  ________, XXXXXXXX, XXXXXXXX, ________,
  ________, X____XXX, _XXXXXXX, X_______,
  X_______, _X____XX, XXXXXXXX, ________,
  XX_____X, __XX_XX_, XXXXXXXX, ________,
};

/** $6ABE (stage1) / $CE69 (stage3): bitmap_stones_2s */
const pixel_t bitmap_stones_2s[2 * 2 * 4 * 1] = {
  XXXX____, ____XXXX, ____XXXX, XXXX____,
  XXXX____, ____X___, _____XXX, _XXXX___,
  XXXXX___, _____X__, ____XXXX, __XX____,
  XXXXXX__, ______XX, ___XXXXX, _XX_____,
};

/** $6ACE (stage1) / $CE79 (stage3): bitmap_stones_3 */
const pixel_t bitmap_stones_3[2 * 2 * 3 * 1] = {
  _____XXX, XXXXX___, XXXXXXXX, ________,
  ______XX, X__X_X__, XXXXXXXX, ________,
  X____XXX, _X_XX___, XXXXXXXX, ________,
};

/** $6ADA (stage1) / $CE85 (stage3): bitmap_stones_3s */
const pixel_t bitmap_stones_3s[2 * 2 * 3 * 1] = {
  XXXX____, ____XXXX, _XXXXXXX, X_______,
  XXXX____, ____X__X, __XXXXXX, _X______,
  XXXXX___, _____X_X, _XXXXXXX, X_______,
};

/** $6AE6 (stage1) / $CE91 (stage3): bitmap_stones_4 */
const pixel_t bitmap_stones_4[1 * 2 * 2 * 1] = {
  ____XXXX, XXXX____,
  ___XXXXX, X_X_____,
};

/** $6AEA (stage1) / $CE95 (stage3): bitmap_stones_4s */
const pixel_t bitmap_stones_4s[1 * 2 * 2 * 1] = {
  XXXX____, ____XXXX,
  XXXX___X, ____X_X_,
};

/** $6AEE (stage1) / $CE99 (stage3): bitmap_stones_5 */
const pixel_t bitmap_stones_5[1 * 2 * 1 * 1] = {
  __XXXXXX, XX______,
};

/** $6AF0 (stage1) / $CE9B (stage3): bitmap_stones_5s */
const pixel_t bitmap_stones_5s[1 * 2 * 1 * 1] = {
  XXXX__XX, ____XX__,
};

/** $6AF2 (stage1) / $CE9D (stage3): bitmap_dust_1 */
const pixel_t bitmap_dust_1[1 * 2 * 1 * 1] = {
  ___XXXXX, _X______,
};

/** $6AF4 (stage1) / $CE9F (stage3): bitmap_dust_1s */
const pixel_t bitmap_dust_1s[1 * 2 * 1 * 1] = {
  XXXX___X, _____X__,
};

/** $6B68 (stage1) / $F871 (stage4): bitmap_turnsign_1 */
const pixel_t bitmap_turnsign_1[4 * 1 * 40 * 1] = {
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

/** $6C08 (stage1) / $F911 (stage4): bitmap_turnsign_2 */
const pixel_t bitmap_turnsign_2[3 * 1 * 30 * 1] = {
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

/** $6C62 (stage1) / $F96B (stage4): bitmap_turnsign_3 */
const pixel_t bitmap_turnsign_3[2 * 1 * 20 * 1] = {
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

/** $6C8A (stage1) / $F993 (stage4): bitmap_turnsign_4 */
const pixel_t bitmap_turnsign_4[2 * 2 * 16 * 1] = {
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

/** $6CCA (stage1) / $F9D3 (stage4): bitmap_turnsign_5 */
const pixel_t bitmap_turnsign_5[2 * 2 * 13 * 1] = {
  X_______, _XXXXXXX, _XXXXXXX, X_______,
  ________, XXXXXXXX, __XXXXXX, XX______,
  ________, XXXXXXXX, __XXXXXX, XX______,
  ________, XXXXXXXX, __XXXXXX, XX______,
  ________, X___X_X_, __XXXXXX, XX______,
  ________, X__X____, __XXXXXX, _X______,
  ________, X__X____, __XXXXXX, XX______,
  ________, X__X____, __XXXXXX, _X______,
  ________, X___X___, __XXXXXX, XX______,
  ________, X___XX__, __XXXXXX, _X______,
  ________, X____XX_, __XXXXXX, _X______,
  ________, X_______, __XXXXXX, _X______,
  ________, XXXXXXXX, __XXXXXX, XX______,
};

/** $6DAC (stage1) / $D679 (stage3): bitmap_tumbleweed_1 */
const pixel_t bitmap_tumbleweed_1[2 * 1 * 16 * 1] = {
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

/** $6DCC (stage1) / $D699 (stage3): bitmap_tumbleweed_2 */
const pixel_t bitmap_tumbleweed_2[2 * 1 * 11 * 1] = {
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

/** $6DE2 (stage1) / $D6AF (stage3): bitmap_tumbleweed_3 */
const pixel_t bitmap_tumbleweed_3[1 * 1 * 9 * 1] = {
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

/** $6DEB (stage1) / $D6B8 (stage3): bitmap_tumbleweed_4 */
const pixel_t bitmap_tumbleweed_4[1 * 1 * 7 * 1] = {
  __XXXX__,
  _XXXXXX_,
  _XXX_XX_,
  _XX__XX_,
  __X_XX__,
  __XX_X__,
  ___XX___,
};

/** $6E1C (stage1) / $FA39 (stage2) / $D6E9 (stage3) / $F63F (stage4) / $D64A (stage5): bitmap_barrier_1 */
const pixel_t bitmap_barrier_1[4 * 1 * 17 * 1] = {
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

/** $6E60 (stage1) / $FA7D (stage2) / $D72D (stage3) / $F683 (stage4) / $D68E (stage5): bitmap_barrier_2 */
const pixel_t bitmap_barrier_2[3 * 1 * 13 * 1] = {
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

/** $6E87 (stage1) / $FAA4 (stage2) / $D754 (stage3) / $F6AA (stage4) / $D6B5 (stage5): bitmap_barrier_3 */
const pixel_t bitmap_barrier_3[2 * 1 * 9 * 1] = {
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

/** $6E99 (stage1) / $FAB6 (stage2) / $D766 (stage3) / $F6BC (stage4) / $D6C7 (stage5): bitmap_barrier_4 */
const pixel_t bitmap_barrier_4[2 * 2 * 7 * 1] = {
  X__XXXXX, _XX_____, X__XXXXX, _XX_____,
  X___X__X, _XXX_XX_, ___XXXXX, XXX_____,
  XX______, __XXXXXX, __XXXXXX, XX______,
  ________, XXXXXXXX, ____XXXX, XXXX____,
  ________, X__XX__X, ____XXXX, X__X____,
  ________, XXXXXXXX, ____XXXX, XXXX____,
  XXXX____, ____XXXX, XXXXXXXX, ________,
};

/** $6EB5 (stage1) / $FAD2 (stage2) / $D782 (stage3) / $F6D8 (stage4) / $D6E3 (stage5): bitmap_barrier_4s */
const pixel_t bitmap_barrier_4s[2 * 2 * 7 * 1] = {
  XXXXX__X, _____XX_, XXXXX__X, _____XX_,
  XXXXX___, _____XXX, X__X___X, _XX_XXX_,
  XXXXXX__, ______XX, ______XX, XXXXXX__,
  XXXX____, ____XXXX, ________, XXXXXXXX,
  XXXX____, ____X__X, ________, X__XX__X,
  XXXX____, ____XXXX, ________, XXXXXXXX,
  XXXXXXXX, ________, ____XXXX, XXXX____,
};

/** $6F5D (stage1) / $F780 (stage4): bitmap_streetlamptop_1 */
const pixel_t bitmap_streetlamptop_1[4 * 1 * 8 * 1] = {
  ________, ________, ________, XXX__X__,
  ___XXXXX, XXXXXXX_, _______X, X_X__X__,
  __X_____, _______X, _____XXX, __X__X__,
  __X_____, _______X, __XXXX__, XX___X__,
  _XX_X_X_, X_X_X_XX, XXX___XX, _X___X__,
  X_XXXXXX, XXXXXXX_, ___XXX__, _XX_XX__,
  XX______, _______X, XXX_____, __XXX___,
  __XXXXXX, XXXXXXX_, ________, ________
};

/** $6F7D (stage1) / $F7A0 (stage4): bitmap_streetlamptop_2 */
const pixel_t bitmap_streetlamptop_2[3 * 1 * 5 * 1] = {
  _XXXXXXX, XX_____X, X_X_X___,
  X_______, __X__XX_, _X__X___,
  X_______, _XXXX__X, XX__X___,
  X_X_X_X_, X____XX_, _X__X___,
  _XXXXXXX, XXXXX___, __XX____
};

/** $6F8C (stage1) / $F7AF (stage4): bitmap_streetlamptop_3 */
const pixel_t bitmap_streetlamptop_3[3 * 1 * 4 * 1] = {
  ______XX, XXXX___X, _XX_____,
  _____X__, ____XXX_, X_X_____,
  ____X_X_, _____X_X, _XX_____,
  _____XXX, XXXXXXX_, ________
};

/** $6F98 (stage1) / $F7BB (stage4): bitmap_streetlamptop_3s */
const pixel_t bitmap_streetlamptop_3s[3 * 1 * 4 * 1] = {
  ________, __XXXXXX, ___X_XX_,
  ________, _X______, XXX_X_X_,
  ________, X_X_____, _X_X_XX_,
  ________, _XXXXXXX, XXX_____
};

/** $6FA4 (stage1) / $F7C7 (stage4): bitmap_streetlamptop_4 */
const pixel_t bitmap_streetlamptop_4[2 * 2 * 4 * 1] = {
  XXXXXXXX, ________, X___XXXX, _XXX____,
  ______X_, XX_XXX_X, ____XXXX, X_XX____,
  ________, X____XX_, _X__XXXX, X_XX____,
  X____XXX, _XXXX___, XXXXXXXX, ________
};

/** $6FB4 (stage1) / $F7D7 (stage4): bitmap_streetlamptop_4s */
const pixel_t bitmap_streetlamptop_4s[2 * 2 * 4 * 1] = {
  XXXXXXXX, ________, XXX___XX, ___XXX__,
  XX______, __XX_XXX, X_____XX, _XX_XX__,
  XX______, __X____X, ___X__XX, X_X_XX__,
  XXX____X, ___XXXX_, XXXXXXXX, ________
};

/** $6FC4 (stage1) / $F7E7 (stage4): bitmap_streetlamptop_5 */
const pixel_t bitmap_streetlamptop_5[2 * 2 * 3 * 1] = {
  XXXXXXX_, _______X, _XXXXXXX, X_______,
  ________, X___XXXX, _XXXXXXX, X_______,
  ____XXXX, XXXX____, XXXXXXXX, ________
};

/** $6FD0 (stage1) / $F7F3 (stage4): bitmap_streetlamptop_5s */
const pixel_t bitmap_streetlamptop_5s[2 * 2 * 3 * 1] = {
  XXXXXXXX, ________, XXX__XXX, ___XX___,
  XXXX____, ____X___, _____XXX, XXXXX___,
  XXXX____, ____XXXX, XXXXXXXX, ________
};

/** $703F (stage1) / $DA39 (stage3) / $DA32 (stage5): bitmap_telegraphpoletop_1 */
const pixel_t bitmap_telegraphpoletop_1[3 * 1 * 13 * 1] = {
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

/** $7066 (stage1) / $DA60 (stage3) / $DA59 (stage5): bitmap_telegraphpoletop_2 */
const pixel_t bitmap_telegraphpoletop_2[3 * 1 * 10 * 1] = {
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

/** $7084 (stage1) / $DA7E (stage3) / $DA77 (stage5): bitmap_telegraphpoletop_3 */
const pixel_t bitmap_telegraphpoletop_3[3 * 1 * 7 * 1] = {
  ______XX, XXXXXX__, ________,
  _____X__, __X__X__, ________,
  _____X_X, X_XX_X__, ________,
  _____XXX, XXXXXXX_, ________,
  ____X___, __XXX__X, ________,
  ____X_XX, X_XXXX_X, ________,
  _____X__, XXX___X_, ________
};

/** $7099 (stage1) / $DA93 (stage3) / $DA8C (stage5): bitmap_telegraphpoletop_3s */
const pixel_t bitmap_telegraphpoletop_3s[3 * 1 * 7 * 1] = {
  ________, __XXXXXX, XX______,
  ________, _X____X_, _X______,
  ________, _X_XX_XX, _X______,
  ________, _XXXXXXX, XXX_____,
  ________, X_____XX, X__X____,
  ________, X_XXX_XX, XX_X____,
  ________, _X__XXX_, __X_____
};

/** $70AE (stage1) / $DAA8 (stage3) / $DAA1 (stage5): bitmap_telegraphpoletop_4 */
const pixel_t bitmap_telegraphpoletop_4[2 * 2 * 5 * 1] = {
  XXXXXXX_, _______X, ______XX, XX_XXX__,
  XXXXXXX_, _______X, ______XX, _X_X_X__,
  XXXXXXXX, ________, XX__XXXX, ___X____,
  XXXXXX__, ______XX, ______XX, XX_XXX__,
  XXXXXX__, ______X_, ______XX, X___X___
};

/** $70C2 (stage1) / $DABC (stage3) / $DAB5 (stage5): bitmap_telegraphpoletop_4s */
const pixel_t bitmap_telegraphpoletop_4s[2 * 2 * 5 * 1] = {
  XXXXXXXX, ________, X_______, _XXX_XXX,
  XXXXXXXX, ________, X_______, _X_X_X_X,
  XXXXXXXX, ________, XXXX__XX, _____X__,
  XXXXXXXX, ________, ________, XXXX_XXX,
  XXXXXXXX, ________, ________, X_X___X_
};

/** $70D6 (stage1) / $DAD0 (stage3) / $DAC9 (stage5): bitmap_telegraphpoletop_5 */
const pixel_t bitmap_telegraphpoletop_5[2 * 2 * 4 * 1] = {
  XXXXXX__, ______XX, ___XXXXX, XXX_____,
  XXXXXX__, ______XX, ___XXXXX, __X_____,
  XXXXX___, _____XXX, ____XXXX, _XXX____,
  XXXXX___, _____X_X, ____XXXX, __XX____
};

/** $70E6 (stage1) / $DAE0 (stage3): bitmap_telegraphpoletop_5s */
const pixel_t bitmap_telegraphpoletop_5s[2 * 2 * 4 * 1] = {
  XXXXXXXX, ________, XX_____X, __XXXXX_,
  XXXXXXXX, ________, XX_____X, __XX__X_,
  XXXXXXXX, ________, X_______, _XXX_XXX,
  XXXXXXXX, ________, X_______, _X_X__XX
};

/** $F05A (stage2) / $EF35 (stage4): pilot_mugshot */
const pixel_t pilot_mugshot[180] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXX__, _X_X_XX_, XXXXXXXX, XXX_X_XX,
  XXXX___X, XXXXXXXX, XXXXXXXX, XXXXXX_X,
  XXXX__X_, XXXXX_X_, X_X_XXXX, XXXXXXXX,
  XXX__XXX, XXXXXXXX, _X_XXXXX, XXXXXXXX,
  XXX_X_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX__XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X,
  XXX_X_XX, XXXXXXXX, XXXX_XXX, XXXXXXXX,
  XXX__XXX, XXXXXXXX, ___XXXXX, XXXXXX_X,
  XXX_X_XX, XXXXXXX_, XXXX_XXX, XXXXXXXX,
  XXX__XXX, XXXXXX_X, X__XX_XX, XXXXXX_X,
  XXX_XXXX, XXXXXX_X, X___XX_X, XXXXXXXX,
  XXX__XXX, XXXXX_XX, ____X_XX, XXXX_X_X,
  XXXX__XX, XX_X_XX_, X___XXXX, X_X_X_XX,
  XXXXXXXX, XXXXXX__, _____X_X, XXXXXXXX,
  X_XXXXXX, X_X_XX__, X_______, _XXXXXXX,
  XXXXXXXX, _____X_X, XX__XX__, __X_XXXX,
  X_XXXXX_, ____X_XX, XXXXXX__, ___XXXXX,
  XXXXXXXX, ____XXXX, XXXXXXX_, ____XXXX,
  X_XXXXX_, ___XXXXX, XXXXXXXX, ___XXXXX,
  XX_XXXXX, __XXXXXX, XX_XXXXX, X_X_XXXX,
  X_XXXXXX, X_XXXXXX, XXXXXXXX, X__XXXXX,
  XX_XXXXX, XX_XXXX_, _____XXX, XX_XXXXX,
  X_X_XXXX, X_XXXX__, XXXX__XX, X_XXXXXX,
  XX__XXXX, XX_X_X_X, XXXXXX_X, _X_XXXXX,
  X_X__XXX, XXXXX_X_, XXXXX_X_, XXXXXX_X,
  XXXX_X__, XXXXXXXX, XXXXXXXX, XXX_XX_X,
  XXXXXXX_, _XXXXXXX, _____XXX, XX__XX_X,
  X__XXXXX, ___XXX__, _______X, X__XX__X,
  X____XXX, XX_XX_X_, _______X, X_XX___X,
  X___XXXX, XXX_XX_X, _X_X__XX, XXXX___X,
  X_XXXXXX, XXXXXXXX, XXXXXXXX, XXX_XX_X,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXXX__XX,
  XX_XXXXX, _XXXXXXX, XXXXXXXX, X_XXXX_X,
  XX_XXXX_, X_XXX_XX, XXXXXXXX, XXXXXXXX,
  X_XXXXXX, _X_XXX_X, _X_X_XXX, X_XXXXXX,
  X_XXXXXX, X_XXX_X_, ____X_XX, _XXXXXXX,
  XXXXXXXX, _X_XXX_X, ___X_XX_, X_XXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,

  attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_GREEN, attribute_BRIGHT_BLACK_OVER_GREEN,
  attribute_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_WHITE,
  attribute_BLACK_OVER_GREEN, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW,
  attribute_BRIGHT_BLACK_OVER_CYAN, attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_CYAN,
  attribute_BLACK_OVER_WHITE, attribute_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_CYAN, attribute_BLACK_OVER_CYAN,
};

/** $E97D (stage2) / $EB0D (stage4): bitmap_E97D */
const pixel_t bitmap_E97D[6 * 1 * 29 * 1] = {
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  _XXXXX__, X_X_XXXX, XXXXXXXX, XXXXXXXX, XXX_X_X_, _XXXXX__,
  XXXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXXX_,
  XXXXX_X_, _X_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X__, X_XXXXX_,
  XXXXX___, X_X_____, __XXXXXX, XXXXXX__, ____X_X_, __XXXXX_,
  _XXX_XXX, XXXXXXXX, XXX_____, _____XXX, XXXXXXXX, XX_XXX__,
  _XX_X_X_, X_X_X_X_, X_X_____, _____XX_, X_X_X_X_, X_X_XX__,
  XX_X_X__, ________, __X_____, _____X__, ________, _X_X_XX_,
  X_______, ________, __XXXXXX, XXXXXX__, ________, ______X_,
  X_______, ________, ________, ________, ________, ______X_,
  X____X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, _X____X_,
  _X_XXXXX, XXXXXXX_, ______XX, X_______, XXXXXXXX, XXXX_X__,
  _X_X__XX, XXX_XXX_, _____XXX, ________, XXX_XXXX, X__X_X__,
  _X_X_XXX, _X_X_XX_, ______XX, X_______, XX_X_X_X, XX_X_X__,
  _X_X__XX, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_XX, X__X_X__,
  _X__X_XX, _X___XXX, XXXXXXXX, XXXXXXXX, XX___X_X, X_X__X__,
  __X__XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX__X___,
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

/** $EA2B (stage2) / $EBBB (stage4): bitmap_EA2B */
const pixel_t bitmap_EA2B[4 * 1 * 19 * 1] = {
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  _XXX_X_X, XXXXXXXX, XXXXXXXX, X_X_XXX_,
  XXX_XXXX, _X__X_XX, _X_XX_X_, XXXX_XXX,
  XXX__X__, ____XXXX, XXXX____, __X__XXX,
  XX__XXXX, X_XXX___, ___XXX_X, XXXX__XX,
  X__X____, ____X___, ___X____, ____X__X,
  X_______, ________, ________, _______X,
  X____XX_, XX_X_XXX, X_XXXX_X, X_X____X,
  X_X_XXXX, XXX___XX, ____XXXX, XXXX_X_X,
  X_X_X_X_, _XX____X, ____XX__, X_XX_X_X,
  X__XX_X_, _XXXXXXX, XXXXXX__, X_XXX__X,
  _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXX__X_,
  __XX____, ________, ________, ___XXX__,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  _X___X_X, X_____X_, X_____XX, _X___X__,
  __XXXX_X, X____X__, _X____XX, _XXXX___,
  _____XX_, XXXXX___, __XXXXX_, XX______,
  _______X, XXXXXXXX, XXXXXXXX, ________,
  _______X, XXXXXXXX, XXXXXXXX, ________,
};

/** $EA77 (stage2) / $EC07 (stage4): bitmap_EA77 */
const pixel_t bitmap_EA77[3 * 1 * 13 * 1] = {
  ___XXXXX, XXXXXXXX, XXXXXX__,
  __XXX_X_, XXXXXXXX, X_X_XXX_,
  _XXX_XXX, X__XXX__, XXXX_XXX,
  __XXX___, __X___X_, ____XXX_,
  _X______, __X___X_, _______X,
  _X_XXXXX, X___X___, XXXXXX_X,
  _X__X___, X___X___, X___X__X,
  _X_XX___, XXXXXXXX, X___XX_X,
  __XXXXXX, XXXXXXXX, XXXXXXX_,
  __X__X_X, _X_XXX_X, _X_X__X_,
  ___XX_XX, ____X___, _XX_XX__,
  ______XX, XXXXXXXX, XXX_____,
  ______XX, XXXXXXXX, XXX_____,
};

/* Conv: port-added masked variants of bitmap_E97D/EA2B/EA77, built under
   CHQ_ENABLE_MASKED_VEHICLES. Hand-authored silhouette mask painted onto the
   exported sheet via scripts/graphics_png.py, see docs/graphics-png-format.md. */
#ifdef CHQ_ENABLE_MASKED_VEHICLES
const pixel_t bitmap_E97D_masked[6 * 2 * 29 * 1] = {
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  X_______, _XXXXX__, ________, X_X_XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXX_X_X_, ______XX, _XXXXX__,
  ________, XXXXX_XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, X_XXXXX_,
  ________, XXXXX_X_, ________, _X_X_X_X, ________, _X_X_X_X, ________, _X_X_X_X, ________, _X_X_X__, _______X, X_XXXXX_,
  ________, XXXXX___, ________, X_X_____, ________, __XXXXXX, ________, XXXXXX__, ________, ____X_X_, _______X, __XXXXX_,
  X_______, _XXX_XXX, ________, XXXXXXXX, ________, XXX_____, ________, _____XXX, ________, XXXXXXXX, ______XX, XX_XXX__,
  X_______, _XX_X_X_, ________, X_X_X_X_, ________, X_X_____, ________, _____XX_, ________, X_X_X_X_, ______XX, X_X_XX__,
  ________, XX_X_X__, ________, ________, ________, __X_____, ________, _____X__, ________, ________, _______X, _X_X_XX_,
  ________, X_______, ________, ________, ________, __XXXXXX, ________, XXXXXX__, ________, ________, _______X, ______X_,
  ________, X_______, ________, ________, ________, ________, ________, ________, ________, ________, _______X, ______X_,
  ________, X____X_X, ________, _X_X_X_X, ________, _X_X_X_X, ________, _X_X_X_X, ________, _X_X_X_X, _______X, _X____X_,
  X_______, _X_XXXXX, ________, XXXXXXX_, ________, ______XX, ________, X_______, ________, XXXXXXXX, ______XX, XXXX_X__,
  X_______, _X_X__XX, ________, XXX_XXX_, ________, _____XXX, ________, ________, ________, XXX_XXXX, ______XX, X__X_X__,
  X_______, _X_X_XXX, ________, _X_X_XX_, ________, ______XX, ________, X_______, ________, XX_X_X_X, ______XX, XX_X_X__,
  X_______, _X_X__XX, ________, X_X_X_X_, ________, X_X_X_X_, ________, X_X_X_X_, ________, X_X_X_XX, ______XX, X__X_X__,
  X_______, _X__X_XX, ________, _X___XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XX___X_X, ______XX, X_X__X__,
  XX______, __X__XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XX__X___,
  XX______, __X_X___, ________, ________, ________, ________, ________, ________, ________, ________, _____XXX, __X_X___,
  XXX_____, ___XXX_X, ________, ________, ________, ________, ________, ________, ________, ________, ____XXXX, X_XX____,
  XXX_____, ___X_XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XX_X____,
  XX______, __X_____, ________, X__XX_X_, ________, X_X_X___, ________, __X_X_X_, ________, X_XX__X_, _____XXX, ____X___,
  XX______, __X_____, ________, X__XX___, ________, _____X__, ________, _X______, ________, __XX__X_, _____XXX, ____X___,
  XXX_____, ___XXXXX, X_______, _X_XX___, ________, _____X__, ________, _X______, ______X_, __XX_X_X, ____XXXX, XXXX____,
  XXXXXXXX, ________, X_______, _X__XXXX, ________, XXXXX___, ________, __XXXXXX, ______XX, XXX__X__, XXXXXXXX, ________,
  XXXXXXXX, ________, XX______, __X_XXXX, ________, XXX_____, ________, ________, _____XXX, __X_X___, XXXXXXXX, ________,
  XXXXXXXX, ________, X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__, XXXXXXXX, ________,
  XXXXXXXX, ________, ________, X______X, ________, XX______, ________, ________, _______X, ______X_, XXXXXXXX, ________,
  XXXXXXXX, ________, X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__, XXXXXXXX, ________,
};

const pixel_t bitmap_EA2B_masked[4 * 2 * 19 * 1] = {
  X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  X_______, _XXX_X_X, ________, XXXXXXXX, ________, XXXXXXXX, _______X, X_X_XXX_,
  ________, XXX_XXXX, ________, _X__X_XX, ________, _X_XX_X_, ________, XXXX_XXX,
  ________, XXX__X__, ________, ____XXXX, ________, XXXX____, ________, __X__XXX,
  ________, XX__XXXX, ________, X_XXX___, ________, ___XXX_X, ________, XXXX__XX,
  ________, X__X____, ________, ____X___, ________, ___X____, ________, ____X__X,
  ________, X_______, ________, ________, ________, ________, ________, _______X,
  ________, X____XX_, ________, XX_X_XXX, ________, X_XXXX_X, ________, X_X____X,
  ________, X_X_XXXX, ________, XXX___XX, ________, ____XXXX, ________, XXXX_X_X,
  ________, X_X_X_X_, ________, _XX____X, ________, ____XX__, ________, X_XX_X_X,
  ________, X__XX_X_, ________, _XXXXXXX, ________, XXXXXX__, ________, X_XXX__X,
  X_______, _X_XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXXX__X_,
  XX______, __XX____, ________, ________, ________, ________, ______XX, ___XXX__,
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  X_______, _X___X_X, ________, X_____X_, ________, X_____XX, ______XX, _X___X__,
  XX______, __XXXX_X, ________, X____X__, ________, _X____XX, _____XXX, _XXXX___,
  XXXXX___, _____XX_, ________, XXXXX___, ________, __XXXXX_, __XXXXXX, XX______,
  XXXXXXX_, _______X, ________, XXXXXXXX, ________, XXXXXXXX, XXXXXXXX, ________,
  XXXXXXX_, _______X, ________, XXXXXXXX, ________, XXXXXXXX, XXXXXXXX, ________,
};

const pixel_t bitmap_EA77_masked[3 * 2 * 13 * 1] = {
  XXX_____, ___XXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  XX______, __XXX_X_, ________, XXXXXXXX, _______X, X_X_XXX_,
  X_______, _XXX_XXX, ________, X__XXX__, ________, XXXX_XXX,
  XX______, __XXX___, ________, __X___X_, _______X, ____XXX_,
  X_______, _X______, ________, __X___X_, ________, _______X,
  X_______, _X_XXXXX, ________, X___X___, ________, XXXXXX_X,
  X_______, _X__X___, ________, X___X___, ________, X___X__X,
  X_______, _X_XX___, ________, XXXXXXXX, ________, X___XX_X,
  XX______, __XXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  XX______, __X__X_X, ________, _X_XXX_X, _______X, _X_X__X_,
  XXX__X__, ___XX_XX, ________, ____X___, ___X__XX, _XX_XX__,
  XXXXXX__, ______XX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXXXXX__, ______XX, ________, XXXXXXXX, ___XXXXX, XXX_____,
};
#endif

/** $EC17 (stage2) / $E994 (stage4): bitmap_EC17 */
const pixel_t bitmap_EC17[2 * 2 * 8 * 1] = {
  ________, XXXXXXXX, _____XXX, XXXXX___,
  ________, XX__XXXX, _____XXX, X__XX___,
  X_______, _X_X_XXX, ____XXXX, _X_X____,
  ________, X_XXXX_X, _____XXX, XXX_X___,
  ________, X_XXXXXX, _____XXX, XXX_X___,
  X_______, _X_XXXXX, ____XXXX, XX_X____,
  XX______, __X_____, ___XXXXX, __X_____,
  XXX_____, ___XXXXX, __XXXXXX, XX______,
};

/** $EC37 (stage2) / $E9B4 (stage4): bitmap_EC37 */
const pixel_t bitmap_EC37[2 * 2 * 8 * 1] = {
  XXX_____, ___XXXXX, ________, XXXXXXXX,
  XXX_____, ___XX__X, ________, XXXX__XX,
  XXXX____, ____X_X_, _______X, XXX_X_X_,
  XXX_____, ___X_XXX, ________, X_XXXX_X,
  XXX_____, ___X_XXX, ________, XXXXXX_X,
  XXXX____, ____X_XX, _______X, XXXXX_X_,
  XXXXX___, _____X__, ______XX, _____X__,
  XXXXXX__, ______XX, _____XXX, XXXXX___,
};

/** $F2F0 (stage2) / $D073 (stage5): bitmap_F2F0 */
const pixel_t bitmap_F2F0[5 * 1 * 8 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXX_, XXXXXXXX, _X_XXXX_,
  XX_XXXXX, X_XXXXXX, _XXXXXXX, XXXXX_XX, __XX____,
  XXXX_XXX, _XXXXXX_, XXXXXX_X, XXXX_X__, _XX_____,
  X_X_X_XX, XXXXXXXX, _XXXX__X, XX_XXXXX, XX______,
  XXXX_XXX, XX_XXXX_, XXXX__XX, __XX____, ________,
  XX_X__X_, XXXXXX_X, X_X_XXXX, XX______, ________,
  XXXXX___, _XXX___X, XXXXX___, ________, ________,
  XXXXXXXX, XXXXXXX_, ________, ________, ________,
};

/** $F318 (stage2) / $D09B (stage5): bitmap_F318 */
const pixel_t bitmap_F318[7 * 1 * 8 * 1] = {
  X_X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXXX, _XXXX___, XX__XX__,
  X__XXXXX, _XXXXX_X, XXXXXXX_, XXXXXXX_, XXXX_XXX, _XX_XX_X, X__X____,
  X_XXXXX_, XXXXXXXX, XXXXX_XX, XXXXXXXX, XXX_XXX_, _XXX___X, XXX_____,
  X__XXXXX, _XXXXXXX, _XXXXXX_, XXXXXXX_, XXXX_X_X, _XX__XX_, ________,
  XX__XXX_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXX_, XXXXX___, ________,
  XX_XXXXX, XXXXXX_X, XXXXXX_X, XXXXX_XX, XXX_XX__, XX______, ________,
  X_X_XXXX, X_XXXXXX, _XXXXXXX, XXXXXXX_, XX_XXXX_, XX______, ________,
  XX_XXXX_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XX_X, X_______, ________,
};

/** $F350 (stage2) / $D0D3 (stage5): bitmap_F350 */
const pixel_t bitmap_F350[7 * 1 * 8 * 1] = {
  X_X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXXX, _XXXXXX_, _XXXX_XX,
  X__XXXXX, _XXXXX_X, XXXXXXX_, XXXXXXX_, XXXX_XXX, _XX_XX__, _XXX___X,
  X_XXXXX_, XXXXXXXX, XXXXX_XX, XXXXXXXX, XXX_XXX_, _XXXXXX_, _XX_X__X,
  X__XXXXX, _XXXXXXX, _XXXXXX_, XXXXXXX_, XXXX_X_X, _XXXX_X_, _XXX___X,
  XX__XXX_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXX_, XXXX____, _XX____X,
  XX_XXXXX, XXXXXX_X, XXXXXX_X, XXXXX_XX, XXX_XX__, XX_XXX__, XXX_X_X_,
  X_X_XXXX, X_XXXXXX, _XXXXXXX, XXXXXXX_, XX_XXXX_, XXXXX_X_, XX_X__X_,
  XX_XXXX_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XX_X, XX_X_X__, XXX___X_,
};

/** $F388 (stage2) / $D10B (stage5): bitmap_F388 */
const pixel_t bitmap_F388[8 * 1 * 8 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X__XXXXX, X_XXXXXX, XXX_XXX_, X_XXX_X_, X_XX_X_X, ____XXXX, __XX__XX, _XX_X__X,
  X__XXX_X, XX_XXXX_, XXXXXXXX, _XXXXX__, XXXXX_X_, ___XXXX_, _XX_X_XX, XX_X___X,
  XX__X_XX, XXXXXXXX, XXXXXXXX, XXXXX_X_, XX_X____, _XXX_X__, XXXX__XX, _XX___X_,
  XX___X_X, XX_XX_XX, _XXXXXX_, XXXX_X_X, XXXXX___, XXXXX_X_, XX___XX_, X_XXX_X_,
  X_X___XX, X_XXXXXX, XXXXXX_X, XXXXX__X, XX_X___X, X_XXXX__, XXX__XXX, _____XX_,
  X_XXXXXX, XXXXXXX_, XXXX_XXX, XXXX__XX, XXX___XX, _XX_X__X, __XXXX__, __XXXX__,
  X__XXXXX, XXXXXXXX, XXXXXXXX, XX__XXXX, XXXXXXX_, X_XX__X_, _XXX___X, XX______,
};

/** $F3C8 (stage2) / $D14B (stage5): bitmap_F3C8 */
const pixel_t bitmap_F3C8[3 * 1 * 4 * 1] = {
  XX_XXXXX, XXXX_XXX, _XX_XXXX,
  XX__XXX_, XX__XX__, XX_XX___,
  X_X___XX, XXXXXXXX, XXX_____,
  XXXXXXXX, XXX_____, ________,
};

/** $F3D4 (stage2) / $D157 (stage5): bitmap_F3D4 */
const pixel_t bitmap_F3D4[4 * 1 * 4 * 1] = {
  X_XXXXXX, XX_XXXXX, _XXXXXXX, XX_XX_XX,
  X_XXX_XX, XXXXXX_X, XXXXXXXX, X__X_XX_,
  XXXXXXXX, XXXXXXXX, X_XXXXXX, __XXX___,
  X_X_XXXX, _XXX__XX, XXXXXX__, _XX_____,
};

/** $F3E9 (stage2) / $D16C (stage5): bitmap_F3E9 */
const pixel_t bitmap_F3E9[5 * 1 * 6 * 1] = {
  XX_XX_XX, XXXXXXXX, XXXXXXXX, XX_X_X_X, X_X__XX_,
  X_XXX_XX, XX_XXXXX, XXXXXXX_, XXX__X_X, _X___X_X,
  X_XXXX_X, XXXXXXXX, X_XXXXX_, XXX_XX_X, XXX__X_X,
  X__XX_XX, XX_XXXXX, XXXXXXXX, XX_XX_XX, X____X_X,
  X_XXXX_X, XXX_XXXX, _XXXXX_X, XX_X__X_, XX__XXX_,
  X_XXXX_X, XXXXXXXX, XXXXXXXX, XX_X_XX_, XX_XXX__,
};

/** $F407 (stage2) / $D18A (stage5): bitmap_F407 */
const pixel_t bitmap_F407[6 * 1 * 6 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_XXXXXX, XXXXXX_X, X_XXXXX_, _XXX_X__, XXX__XX_, XXXXX__X,
  X__XXXXX, XXXXXXXX, XXXXXX__, X_X____X, _X_XXX__, XX_X__X_,
  XX_X_XX_, XXXX_XXX, X_XXX__X, XX_X__X_, __X_X_X_, X_X_X_X_,
  XXX___XX, XXXXXXXX, _XXXXX_X, X___XXXX, XX__XX_X, ___XXX__,
  X_XXXXXX, XXXXXXXX, XXXX__XX, XXXXX_XX, X_X__XX_, _XX_____,
};

/** $F42B (stage2) / $D1AE (stage5): bitmap_F42B */
const pixel_t bitmap_F42B[2 * 1 * 4 * 1] = {
  X_XXX_XX, _XXXXX_X,
  XX_XXXXX, _XX_XXX_,
  XX__XXX_, XXXXX___,
  XXXXXXXX, ________,
};

/** $F433 (stage2) / $D1B6 (stage5): bitmap_F433 */
const pixel_t bitmap_F433[3 * 1 * 3 * 1] = {
  X_XX_XX_, XXXXXXXX, _X_XXX__,
  X_XX_XXX, _XXXXX__, XX_XX___,
  X_XXXXX_, XXX_XXX_, X_XX____,
};

/** $F43C (stage2) / $D1BF (stage5): bitmap_F43C */
const pixel_t bitmap_F43C[3 * 1 * 4 * 1] = {
  X_XX_XX_, XXXXXXXX, XX___X_X,
  X_XX_XXX, _XXXXXX_, X__X_X_X,
  X_XXX_XX, _XX_XX_X, X_X_XX_X,
  X_XXX_XX, XXXXXXXX, X_X_X_X_,
};

/** $F448 (stage2) / $D1CB (stage5): bitmap_F448 */
const pixel_t bitmap_F448[4 * 1 * 4 * 1] = {
  X_XXXXXX, XXXXXXX_, XX_X__XX, __X_X_X_,
  X__XXX_X, XXXXXX_X, X_X__X__, _XXXXXX_,
  XX__XXXX, _XX_XXX_, X___XXX_, XXX__X__,
  X_XXXXXX, XXXXX_XX, XXXXXX_X, _X__X___,
};

/** $F458 (stage2) / $D1DB (stage5): bitmap_F458 */
const pixel_t bitmap_F458[1 * 1 * 3 * 1] = {
  XXX_XX_X,
  XXXX_XX_,
  XX______,
};

/** $F45B (stage2) / $D1DE (stage5): bitmap_F45B */
const pixel_t bitmap_F45B[2 * 1 * 3 * 1] = {
  XXXXXXXX, XXXX_X__,
  XXXXX_X_, X_XXX___,
  XX_XXXXX, XXX_____,
};

/** $F461 (stage2) / $D1E4 (stage5): bitmap_F461 */
const pixel_t bitmap_F461[2 * 1 * 3 * 1] = {
  XXXXXXXX, XXXXXX_X,
  XXXXX_X_, XXX__X_X,
  XX_XXXXX, XX_XX_X_,
};

/** $F467 (stage2) / $D1EA (stage5): bitmap_F467 */
const pixel_t bitmap_F467[3 * 1 * 2 * 1] = {
  XXX_XXXX, XX_XX_XX, _X_X____,
  X_XXXX_X, X_X__XX_, XXX_____,
};

/** $F46D (stage2) / $D1F0 (stage5): bitmap_F46D */
const pixel_t bitmap_F46D[7 * 1 * 8 * 1] = {
  XX_XXXX_, _XXXXXX_, XXXX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_X_X,
  X___XXX_, __XX_XX_, XXX_XXXX, _XXXXXXX, _XXXXXXX, X_XXXXX_, XXXXX__X,
  X__X_XX_, _XXXXXX_, _XXX_XXX, XXXXXXXX, XX_XXXXX, XXXXXXXX, _XXXXX_X,
  X___XXX_, _X_XXXX_, X_X_XXXX, _XXXXXXX, _XXXXXX_, XXXXXXX_, XXXXX__X,
  X____XX_, ____XXXX, _XXX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _XXX__XX,
  _X_X_XXX, __XXX_XX, __XX_XXX, XX_XXXXX, X_XXXXXX, X_XXXXXX, XXXXX_XX,
  _X__X_XX, _X_XXXXX, _XXXX_XX, _XXXXXXX, XXXXXXX_, XXXXXX_X, XXXX_X_X,
  _X___XXX, __X_X_XX, X_XX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _XXXX_XX,
};

/** $F4A5 (stage2) / $D228 (stage5): bitmap_F4A5 */
const pixel_t bitmap_F4A5[5 * 1 * 6 * 1] = {
  _XX__X_X, X_X_X_XX, XXXXXXXX, XXXXXXXX, XX_XX_XX,
  X_X___X_, X_X__XXX, _XXXXXXX, XXXXX_XX, XX_XXX_X,
  X_X__XXX, X_XX_XXX, _XXXXX_X, XXXXXXXX, X_XXXX_X,
  X_X____X, XX_XX_XX, XXXXXXXX, XXXXX_XX, XX_XX__X,
  _XXX__XX, _X__X_XX, X_XXXXX_, XXXX_XXX, X_XXXX_X,
  __XXX_XX, _XX_X_XX, XXXXXXXX, XXXXXXXX, X_XXXX_X,
};

/** $F4C3 (stage2) / $D246 (stage5): bitmap_F4C3 */
const pixel_t bitmap_F4C3[3 * 1 * 4 * 1] = {
  X_X___XX, XXXXXXXX, _XX_XX_X,
  X_X_X__X, _XXXXXX_, XXX_XX_X,
  X_XX_X_X, X_XX_XX_, XX_XXX_X,
  _X_X_X_X, XXXXXXXX, XX_XXX_X,
};

/** $F4CF (stage2) / $D252 (stage5): bitmap_F4CF */
const pixel_t bitmap_F4CF[2 * 1 * 3 * 1] = {
  X_XXXXXX, XXXXXXXX,
  X_X__XXX, _X_XXXXX,
  _X_XX_XX, XXXXX_XX,
};

/** $F4D5 (stage2) / $D258 (stage5): bitmap_F4D5 */
const pixel_t bitmap_F4D5[1 * 2 * 1 * 1] = {
  ____XXXX, XXXX____,
};

/** $F4D7 (stage2) / $D25A (stage5): bitmap_F4D7 */
const pixel_t bitmap_F4D7[2 * 2 * 1 * 1] = {
  ________, XX_X_XXX, XXXXXXXX, ________,
};

/** $F4DB (stage2) / $D25E (stage5): bitmap_F4DB */
const pixel_t bitmap_F4DB[2 * 2 * 2 * 1] = {
  ________, X_XXX_XX, ___XXXXX, X_X_____,
  ________, XXXXXXXX, __XXXXXX, _X______,
};

/** $F4E3 (stage2) / $D266 (stage5): bitmap_F4E3 */
const pixel_t bitmap_F4E3[3 * 2 * 2 * 1] = {
  ________, XX_XXXXX, ______XX, XX_X_X__, XXXXXXXX, ________,
  ________, XXXXXXX_, _____XXX, X_X_X___, XXXXXXXX, ________,
};

/** $F4EF (stage2) / $D272 (stage5): bitmap_F4EF */
const pixel_t bitmap_F4EF[1 * 2 * 1 * 1] = {
  XXXX____, ____XXXX,
};

/** $F4F1 (stage2) / $D274 (stage5): bitmap_F4F1 */
const pixel_t bitmap_F4F1[2 * 2 * 1 * 1] = {
  XXXX____, ____XX_X, ____XXXX, _XXX____,
};

/** $F4F5 (stage2) / $D278 (stage5): bitmap_F4F5 */
const pixel_t bitmap_F4F5[2 * 2 * 2 * 1] = {
  XXXX____, ____X_XX, _______X, X_XXX_X_,
  XXXX____, ____XXXX, ______XX, XXXX_X__,
};

/** $F4FD (stage2) / $D280 (stage5): bitmap_F4FD */
const pixel_t bitmap_F4FD[3 * 2 * 2 * 1] = {
  XXXX____, ____XX_X, ________, XXXXXX_X, __XXXXXX, _X______,
  XXXX____, ____XXXX, ________, XXX_X_X_, _XXXXXXX, X_______,
};

/** $F8A2 (stage2) / $D8C2 (stage5): bitmap_F8A2 */
const pixel_t bitmap_F8A2[1 * 2 * 2 * 1] = {
  XXXXX__X, _____XX_,
  XXXXX__X, _____XX_,
};

/** $F97B (stage2) / $D93B (stage5): bitmap_F97B */
const pixel_t bitmap_F97B[5 * 1 * 8 * 1] = {
  __XXXXX_, ________, XXX_XXX_, ________, _XXXXX__,
  _XX___XX, _______X, X_X_X_XX, ________, XX___XX_,
  _X_____X, _____XXX, __X_X__X, XX______, X_____X_,
  _X_____X, __XXXX__, XX___XX_, _XXXX___, X_____X_,
  _XX_X_XX, XXX___XX, _X___X_X, X___XXXX, XX_X_XX_,
  XXXXXXX_, ___XXX__, _XX_XX__, _XXX____, XXXXXXXX,
  X______X, XXX_____, __XXX___, ____XXXX, _______X,
  _XXXXXX_, ________, ________, ________, XXXXXXX_,
};

/** $F9A3 (stage2) / $D963 (stage5): bitmap_F9A3 */
const pixel_t bitmap_F9A3[4 * 1 * 6 * 1] = {
  ____XXXX, _______X, X_XX____, ___XXXX_,
  ___XX__X, X____XX_, X_X_XX__, __XX__XX,
  ___X____, X_XXX__X, X__X__XX, X_X____X,
  ____X_XX, XXX__XX_, X__XXX__, XXXXX_X_,
  ___X_X_X, ___XX___, _XX___XX, ___X_X_X,
  ____XXXX, XXX_____, ________, XXXXXXX_,
};

/** $F9BB (stage2) / $D97B (stage5): bitmap_F9BB */
const pixel_t bitmap_F9BB[3 * 1 * 4 * 1] = {
  _X_X___X, XXXXX___, X_X_____,
  X___XXX_, X__X_XXX, ___X____,
  XXXX____, _XX_____, XXXX____,
  _XXX____, ________, XXX_____,
};

/** $F9C7 (stage2) / $D987 (stage5): bitmap_F9C7 */
const pixel_t bitmap_F9C7[3 * 1 * 4 * 1] = {
  _____X_X, ___XXXXX, X___X_X_,
  ____X___, XXX_X__X, _XXX___X,
  ____XXXX, _____XX_, ____XXXX,
  _____XXX, ________, ____XXX_,
};

/** $F9D3 (stage2) / $D993 (stage5): bitmap_F9D3 */
const pixel_t bitmap_F9D3[3 * 2 * 3 * 1] = {
  XX___XX_, __X_X__X, ____XX__, XXXX__X_, _XXXXXXX, X_______,
  XX______, ____XXX_, _____X__, X_X_X_X_, _XXXXXXX, ________,
  XX_____X, __XXX___, XXXX____, ______XX, _XXXXXXX, X_______,
};

/** $F9E5 (stage2) / $D9A5 (stage5): bitmap_F9E5 */
const pixel_t bitmap_F9E5[3 * 2 * 3 * 1] = {
  XXXXXX__, ______X_, _XX_____, X__XXXXX, XX___XXX, __X_X___,
  XXXXXX__, ________, ________, XXX_X_X_, _X___XXX, X_X_____,
  XXXXXX__, ______XX, ___XXXXX, X_______, _____XXX, __XXX___,
};

/** $C95E (stage3) / $CD26 (stage5): bitmap_C95E */
const pixel_t bitmap_C95E[6 * 1 * 30 * 1] = {
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  __XXXXX_, _X_X_XXX, XXXXXXXX, XXXXXXXX, XXXX_X_X, __XXXXX_,
  _XXXXX_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XXXXX,
  _XXXXX_X, __X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, _X_XXXXX,
  _XXXXX__, _X_X____, ___XXXXX, XXXXXXX_, _____X_X, ___XXXXX,
  __XXX_XX, XXXXXXXX, XXXX____, ______XX, XXXXXXXX, XXX_XXX_,
  __XX_X_X, _X_X_X_X, _X_X____, ______XX, _X_X_X_X, _X_X_XX_,
  _XX_X_X_, ________, ___X____, ______X_, ________, __X_X_XX,
  _X______, ________, ___XXXXX, XXXXXXX_, ________, _______X,
  _X______, ________, ________, ________, ________, _______X,
  _X____X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X____X,
  __X_XXXX, XXXXXXXX, _______X, XX______, _XXXXXXX, XXXXX_X_,
  __X_X__X, XXXX_XXX, ______XX, X_______, _XXX_XXX, XX__X_X_,
  __X_X_XX, X_X_X_XX, _______X, XX______, _XX_X_X_, XXX_X_X_,
  __X_X__X, XX_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, XX__X_X_,
  __X__X_X, X_X___XX, XXXXXXXX, XXXXXXXX, XXX___X_, XX_X__X_,
  ___X__XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX__X__,
  ___X____, ________, ________, ________, ________, _____X__,
  ___X__X_, X_______, ________, ________, ________, __X__X__,
  ___X_X__, __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, ___X_X__,
  ___X_XXX, __X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, _XXX_X__,
  ___XXX_X, X__X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X__, XX_XXX__,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  ___X____, ________, ________, ________, ________, _____X__,
  ___X____, ________, ________, ________, ________, _____X__,
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  ________, ____X___, ________, ________, ____X___, ________,
  ________, _____XX_, ________, ________, __XX____, ________,
  ________, _______X, XXXXXXXX, XXXXXXXX, XX______, ________,
};

/** $CA12 (stage3) / $CDDA (stage5): bitmap_CA12 */
const pixel_t bitmap_CA12[4 * 1 * 20 * 1] = {
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  _XXX__X_, XXXXXXXX, XXXXXXX_, X__XXXX_,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXX_XXXX,
  XXXXX_X_, X_X_XXXX, XXX_X_X_, X_X_XXXX,
  _X_X_X_X, _X_XX___, __XX_X_X, _X_X__X_,
  X_______, ____X___, __X_____, _______X,
  X_______, ________, ________, _______X,
  X__XXX_X, X_XX_XX_, XX_XX_XX, _XX_X__X,
  X_X_XXXX, XXX_____, ____XXXX, XXXX_X_X,
  X_X_X_X_, _XX_____, ____XX__, X_XX_X_X,
  X__XX_X_, _XXXXXXX, XXXXXX__, X_XXX__X,
  _X__XXXX, XXXXXXXX, XXXXXXXX, XXXX__X_,
  _X______, ________, ________, ______X_,
  _XX___XX, XXXXXXXX, XXXXXXXX, X____XX_,
  _XXXX__X, X_XX_XX_, XX_XX_XX, X__XXXX_,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  _X______, ________, ________, ______X_,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  ________, XX______, _____XX_, ________,
  ________, __XXXXXX, XXXXX___, ________,
};

/** $CA62 (stage3) / $CE2A (stage5): bitmap_CA62 */
const pixel_t bitmap_CA62[3 * 1 * 13 * 1] = {
  ___XXXXX, XXXXXXXX, XXXXXX__,
  _XXX___X, XXXXXXXX, XX___XX_,
  _XX_X_X_, X_XXXXX_, X_X_X_XX,
  _X______, ________, _______X,
  _X___XXX, XXXXXXXX, XXXX___X,
  _X_XXXXX, X___X___, XXXXXX_X,
  _X__X___, X___X___, X___X__X,
  _X_XXXXX, XXXXXXXX, XXXXXX_X,
  __XX____, ________, ______X_,
  __XXX__X, XXXXXXXX, XX__XXX_,
  __X__XXX, _X_X_X_X, _XXX__X_,
  ___XXXXX, XXXXXXXX, XXXXXX__,
  _______X, XXXXXXXX, XX______,
};

/* Conv: port-added masked variants of bitmap_C95E/CA12/CA62, built under
   CHQ_ENABLE_MASKED_VEHICLES. Hand-authored silhouette mask painted onto the
   exported sheet via scripts/graphics_png.py, see docs/graphics-png-format.md. */
#ifdef CHQ_ENABLE_MASKED_VEHICLES
const pixel_t bitmap_C95E_masked[6 * 2 * 30 * 1] = {
  XXXXX___, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XXXX____,
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  XX______, __XXXXX_, ________, _X_X_XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXX_X_X, _______X, __XXXXX_,
  X_______, _XXXXX_X, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XX_XXXXX,
  X_______, _XXXXX_X, ________, __X_X_X_, ________, X_X_X_X_, ________, X_X_X_X_, ________, X_X_X_X_, ________, _X_XXXXX,
  X_______, _XXXXX__, ________, _X_X____, ________, ___XXXXX, ________, XXXXXXX_, ________, _____X_X, ________, ___XXXXX,
  XX______, __XXX_XX, ________, XXXXXXXX, ________, XXXX____, ________, ______XX, ________, XXXXXXXX, _______X, XXX_XXX_,
  XX______, __XX_X_X, ________, _X_X_X_X, ________, _X_X____, ________, ______XX, ________, _X_X_X_X, _______X, _X_X_XX_,
  X_______, _XX_X_X_, ________, ________, ________, ___X____, ________, ______X_, ________, ________, ________, __X_X_XX,
  X_______, _X______, ________, ________, ________, ___XXXXX, ________, XXXXXXX_, ________, ________, ________, _______X,
  X_______, _X______, ________, ________, ________, ________, ________, ________, ________, ________, ________, _______X,
  X_______, _X____X_, ________, X_X_X_X_, ________, X_X_X_X_, ________, X_X_X_X_, ________, X_X_X_X_, ________, X_X____X,
  XX______, __X_XXXX, ________, XXXXXXXX, ________, _______X, ________, XX______, ________, _XXXXXXX, _______X, XXXXX_X_,
  XX______, __X_X__X, ________, XXXX_XXX, ________, ______XX, ________, X_______, ________, _XXX_XXX, _______X, XX__X_X_,
  XX______, __X_X_XX, ________, X_X_X_XX, ________, _______X, ________, XX______, ________, _XX_X_X_, _______X, XXX_X_X_,
  XX______, __X_X__X, ________, XX_X_X_X, ________, _X_X_X_X, ________, _X_X_X_X, ________, _X_X_X_X, _______X, XX__X_X_,
  XX______, __X__X_X, ________, X_X___XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXX___X_, _______X, XX_X__X_,
  XXX_____, ___X__XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXX__X__,
  XXX_____, ___X____, ________, ________, ________, ________, ________, ________, ________, ________, ______XX, _____X__,
  XXX_____, ___X__X_, ________, X_______, ________, ________, ________, ________, ________, ________, ______XX, __X__X__,
  XXX_____, ___X_X__, ________, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, ______XX, ___X_X__,
  XXX_____, ___X_XXX, ________, __X_X_X_, ________, X_X_X_X_, ________, X_X_X_X_, ________, X_X_X_X_, ______XX, _XXX_X__,
  XXX_____, ___XXX_X, ________, X__X_X_X, ________, _X_X_X_X, ________, _X_X_X_X, ________, _X_X_X__, ______XX, XX_XXX__,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  XXX_____, ___X____, ________, ________, ________, ________, ________, ________, ________, ________, ______XX, _____X__,
  XXX_____, ___X____, ________, ________, ________, ________, ________, ________, ________, ________, ______XX, _____X__,
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXXXXXX, ________, XXXX____, ____X___, ________, ________, ________, ________, _____XXX, ____X___, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXX___, _____XX_, ________, ________, ________, ________, ____XXXX, __XX____, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXX_, _______X, ________, XXXXXXXX, ________, XXXXXXXX, __XXXXXX, XX______, XXXXXXXX, ________,
};

const pixel_t bitmap_CA12_masked[4 * 2 * 20 * 1] = {
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  X_______, _XXX__X_, ________, XXXXXXXX, ________, XXXXXXX_, _______X, X__XXXX_,
  ________, XXX_XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXX_XXXX,
  ________, XXXXX_X_, ________, X_X_XXXX, ________, XXX_X_X_, ________, X_X_XXXX,
  X_______, _X_X_X_X, ________, _X_XX___, ________, __XX_X_X, _______X, _X_X__X_,
  ________, X_______, ________, ____X___, ________, __X_____, ________, _______X,
  ________, X_______, ________, ________, ________, ________, ________, _______X,
  ________, X__XXX_X, ________, X_XX_XX_, ________, XX_XX_XX, ________, _XX_X__X,
  ________, X_X_XXXX, ________, XXX_____, ________, ____XXXX, ________, XXXX_X_X,
  ________, X_X_X_X_, ________, _XX_____, ________, ____XX__, ________, X_XX_X_X,
  ________, X__XX_X_, ________, _XXXXXXX, ________, XXXXXX__, ________, X_XXX__X,
  X_______, _X__XXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXXX__X_,
  X_______, _X______, ________, ________, ________, ________, _______X, ______X_,
  X_______, _XX___XX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, X____XX_,
  X_______, _XXXX__X, ________, X_XX_XX_, ________, XX_XX_XX, _______X, X__XXXX_,
  X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  X_______, _X______, ________, ________, ________, ________, _______X, ______X_,
  XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  XXXXXXXX, ________, ________, XX______, _______X, _____XX_, XXXXXXXX, ________,
  XXXXXXXX, ________, XX______, __XXXXXX, _____XXX, XXXXX___, XXXXXXXX, ________,
};

const pixel_t bitmap_CA62_masked[3 * 2 * 13 * 1] = {
  XXX_____, ___XXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  X_______, _XXX___X, ________, XXXXXXXX, _______X, XX___XX_,
  X_______, _XX_X_X_, ________, X_XXXXX_, ________, X_X_X_XX,
  X_______, _X______, ________, ________, ________, _______X,
  X_______, _X___XXX, ________, XXXXXXXX, ________, XXXX___X,
  X_______, _X_XXXXX, ________, X___X___, ________, XXXXXX_X,
  X_______, _X__X___, ________, X___X___, ________, X___X__X,
  X_______, _X_XXXXX, ________, XXXXXXXX, ________, XXXXXX_X,
  XX______, __XX____, ________, ________, _______X, ______X_,
  XX______, __XXX__X, ________, XXXXXXXX, _______X, XX__XXX_,
  XX______, __X__XXX, ________, _X_X_X_X, _______X, _XXX__X_,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  XXXXXXX_, _______X, ________, XXXXXXXX, __XXXXXX, XX______,
};
#endif

/** $CA89 (stage3) / $CBED (stage3) / $CE51 (stage5): bitmap_CA89 */
const pixel_t bitmap_CA89[2 * 2 * 8 * 1] = {
  ________, XXXXXXXX, _____XXX, XXXXX___,
  ________, XX__XXXX, _____XXX, X__XX___,
  X_______, _XX_X_X_, ____XXXX, X_XX____,
  ________, X_XXX___, _____XXX, XXX_X___,
  ________, XXXXXXXX, _____XXX, XXXXX___,
  ________, X_______, _____XXX, ____X___,
  X_______, _XX_X_X_, ____XXXX, X_XX____,
  XX______, __XXXXXX, __XXXXXX, XX______,
};

/** $CAA9 (stage3) / $CC0D (stage3) / $CE71 (stage5): bitmap_CAA9 */
const pixel_t bitmap_CAA9[2 * 2 * 8 * 1] = {
  XXX_____, ___XXXXX, ________, XXXXXXXX,
  XXX_____, ___XX__X, ________, XXXX__XX,
  XXXX____, ____XX_X, _______X, _X_X_XX_,
  XXX_____, ___X_XXX, ________, ___XXX_X,
  XXX_____, ___XXXXX, ________, XXXXXXXX,
  XXX_____, ___X____, ________, _______X,
  XXXX____, ____XX_X, _______X, _X_X_XX_,
  XXXXX___, _____XXX, _____XXX, XXXXX___,
};

/** $D3AF (stage3) / $D380 (stage5): bitmap_D3AF */
const pixel_t bitmap_D3AF[4 * 1 * 16 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ________, ________, __X__XXX,
  X____XXX, ________, ___XXX__, __X____X,
  X__XX___, XXXXXXXX, XXX___XX, __X_X__X,
  X__X_X__, _XXX____, _X___XXX, __X_XX_X,
  X__X_XXX, __XXX__X, X__XXXXX, __X__X_X,
  X__X_XXX, X___XXX_, __XXXX_X, __X__X_X,
  X__X_XXX, _XX__X__, XXXXX__X, __X_XX_X,
  X__X_XXX, ___X___X, XXXX___X, __X_XX_X,
  X__X_XXX, _XX__X__, XXXXX__X, __X_XX_X,
  X__X_XXX, X___XXX_, __XXXX_X, __X__X_X,
  X__X_XXX, __XXXX_X, X__XXXXX, __X__X_X,
  X__X_X__, _XXXX___, _X___XXX, __X_XX_X,
  X__XX___, XXXXXXXX, XXX___XX, __X__X_X,
  X_______, ________, ________, __X____X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX__X,
};

/** $D3EF (stage3) / $D3C0 (stage5): bitmap_D3EF */
const pixel_t bitmap_D3EF[4 * 1 * 24 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ________, ________, __X__XXX,
  X____XXX, ________, ___XXX__, __X___XX,
  X__XX___, XXXXXXXX, XXX___XX, __X_XXXX,
  X__X_X__, _XXX____, _X___XXX, __X_XXXX,
  X__X_XXX, __XXX__X, X__XXXXX, __X_X_X_,
  X__X_XXX, X___XXX_, __XXXX_X, __X_XXXX,
  X__X_XXX, _XX__X__, XXXXX__X, __X_XX_X,
  X__X_XXX, ___X___X, XXXX___X, __X_X___,
  X__X_XXX, _XX__X__, XXXXX__X, __X_XXXX,
  X__X_XXX, X___XXX_, __XXXX_X, __X_X_X_,
  X__X_XXX, __XXXX_X, X__XXXXX, __X_X___,
  X__X_X__, _XXXX___, _X___XXX, __X_X___,
  X__XX___, XXXXXXXX, XXX___XX, __X_XX__,
  X____XXX, ________, ___XXX__, __XX_XX_,
  X_______, ________, ________, ____XX_X,
  X______X, XXXXXXXX, XXXXXXXX, XX____XX,
  _XXXX___, ___XXXXX, XXXX____, __X_____,
  _____XXX, X______X, XXXXXXXX, X_X_____,
  ________, _XXXX___, ___XXXXX, XXX____X,
  ________, _____XXX, X______X, XXX____X,
  ________, ________, _XXXX___, __X____X,
  ________, ________, _____XXX, X_______,
  ________, ________, ________, _XXXXXXX,
};

/** $D44F (stage3) / $D420 (stage5): bitmap_D44F */
const pixel_t bitmap_D44F[24] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_X_X_, XXXXXXXX, _X_X_X_X,
  ________, XXXXXXXX, X_X_X_X_, ________, ________, ________, ________, ________,
  XXXXXXXX, ________, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX,
};

/** $D467 (stage3) / $D438 (stage5): bitmap_D467 */
const pixel_t bitmap_D467[4 * 1 * 16 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX__X__, ________, ________, _______X,
  X____X__, __XXX___, ________, XXX____X,
  X__X_X__, XX___XXX, XXXXXXXX, ___XX__X,
  X_XX_X__, XXX___X_, ____XXX_, __X_X__X,
  X_X__X__, XXXXX__X, X__XXX__, XXX_X__X,
  X_X__X__, X_XXXX__, _XXX___X, XXX_X__X,
  X_XX_X__, X__XXXXX, __X__XX_, XXX_X__X,
  X_XX_X__, X___XXXX, X___X___, XXX_X__X,
  X_XX_X__, X__XXXXX, __X__XX_, XXX_X__X,
  X_X__X__, X_XXXX__, _XXX___X, XXX_X__X,
  X_X__X__, XXXXX__X, X_XXXX__, XXX_X__X,
  X_XX_X__, XXX___X_, ___XXXX_, __X_X__X,
  X_X__X__, XX___XXX, XXXXXXXX, ___XX__X,
  X____X__, ________, ________, _______X,
  X__XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
};

/** $D4A7 (stage3) / $D478 (stage5): bitmap_D4A7 */
const pixel_t bitmap_D4A7[4 * 1 * 24 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX__X__, ________, ________, _______X,
  XX___X__, __XXX___, ________, XXX____X,
  XXXX_X__, XX___XXX, XXXXXXXX, ___XX__X,
  XXXX_X__, XXX___X_, ____XXX_, __X_X__X,
  X_XX_X__, XXXXX__X, X__XXX__, XXX_X__X,
  XXXX_X__, X_XXXX__, _XXX___X, XXX_X__X,
  _X_X_X__, X__XXXXX, __X__XX_, XXX_X__X,
  ___X_X__, X___XXXX, X___X___, XXX_X__X,
  XXXX_X__, X__XXXXX, __X__XX_, XXX_X__X,
  X_XX_X__, X_XXXX__, _XXX___X, XXX_X__X,
  ___X_X__, XXXXX__X, X_XXXX__, XXX_X__X,
  ___X_X__, XXX___X_, ___XXXX_, __X_X__X,
  __XX_X__, XX___XXX, XXXXXXXX, ___XX__X,
  _XX_XX__, __XXX___, ________, XXX____X,
  X_XX____, ________, ________, _______X,
  XX____XX, XXXXXXXX, XXXXXXXX, X______X,
  _____X__, ____XXXX, XXXXX___, ___XXXX_,
  _____X_X, XXXXXXXX, X______X, XXX_____,
  X____XXX, XXXXX___, ___XXXX_, ________,
  X____XXX, X______X, XXX_____, ________,
  X____X__, ___XXXX_, ________, ________,
  _______X, XXX_____, ________, ________,
  XXXXXXX_, ________, ________, ________,
};

/** $D507 (stage3) / $D4D8 (stage5): bitmap_D507 */
const pixel_t bitmap_D507[3 * 1 * 12 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ________, ___X__XX,
  X_XX__XX, XXXXXX__, XX_X___X,
  X_X_X__X, X___X__X, XX_X_X_X,
  X_X_XX__, XXXX__XX, XX_X___X,
  X_X_XXXX, ____XXXX, _X_X_X_X,
  X_X_XX_X, X__XXXX_, _X_X_X_X,
  X_X_XXXX, ____XXXX, _X_X_X_X,
  X_X_XX__, XXXX__XX, XX_X___X,
  X_XXX__X, XX__X__X, XX_X_X_X,
  X_XX__XX, XXXXXX__, XX_X___X,
  XXXXXXXX, XXXXXXXX, XXXXXX_X,
};

/** $D52B (stage3) / $D4FC (stage5): bitmap_D52B */
const pixel_t bitmap_D52B[3 * 1 * 18 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ________, ___X__XX,
  X_XX__XX, XXXXXX__, XX_X_XXX,
  X_X_X__X, X___X__X, XX_X_XXX,
  X_X_XX__, XXXX__XX, XX_X_XX_,
  X_X_XXXX, ____XXXX, _X_X_X_X,
  X_X_XX_X, X__XXXX_, _X_X_X__,
  X_X_XXXX, ____XXXX, _X_X_XX_,
  X_X_XX__, XXXX__XX, XX_X_X__,
  X_X_X__X, XX__X__X, XX_X_X__,
  X_XX__XX, XXXXXX__, XX_X_X__,
  X_______, ________, ___X_XX_,
  X____XXX, XXXXXXXX, XXX_X_XX,
  _XXXX___, XXXXXXX_, ___X____,
  ____XXXX, ____XXXX, XXXX___X,
  ________, XXXX____, XXXX___X,
  ________, ____XXXX, ___X____,
  ________, ________, XXXXXXXX,
};

/** $D564 (stage3) / $D535 (stage5): bitmap_D564 */
const pixel_t bitmap_D564[18] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_X_X_, _X_X_X_X, ________, X_X_X_X_,
  ________, ________, ________, ________, XXXXXXXX, ________, XXXXXXXX, XXXXXXXX,
  ________, XXXXXXXX,
};

/** $D576 (stage3) / $D547 (stage5): bitmap_D576 */
const pixel_t bitmap_D576[3 * 1 * 12 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XX__X___, ________, _______X,
  X___X_XX, __XXXXXX, XX__XX_X,
  X_X_X_XX, X__X___X, X__X_X_X,
  X___X_XX, XX__XXXX, __XX_X_X,
  X_X_X_X_, XXXX____, XXXX_X_X,
  X_X_X_X_, _XXXX__X, X_XX_X_X,
  X_X_X_X_, XXXX____, XXXX_X_X,
  X___X_XX, XX__XXXX, __XX_X_X,
  X_X_X_XX, X__X__XX, X__XXX_X,
  X___X_XX, __XXXXXX, XX__XX_X,
  X_XXXXXX, XXXXXXXX, XXXXXXXX,
};

/** $D59A (stage3) / $D56B (stage5): bitmap_D59A */
const pixel_t bitmap_D59A[3 * 1 * 18 * 1] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XX__X___, ________, _______X,
  XXX_X_XX, __XXXXXX, XX__XX_X,
  XXX_X_XX, X__X___X, X__X_X_X,
  X_X_X_XX, XX__XXXX, __XX_X_X,
  _XX_X_X_, XXXX____, XXXX_X_X,
  __X_X_X_, _XXXX__X, X_XX_X_X,
  X_X_X_X_, XXXX____, XXXX_X_X,
  __X_X_XX, XX__XXXX, __XX_X_X,
  __X_X_XX, X__X__XX, X__X_X_X,
  __X_X_XX, __XXXXXX, XX__XX_X,
  _XX_X___, ________, _______X,
  XX_X_XXX, XXXXXXXX, XXX____X,
  ____X___, _XXXXXXX, ___XXXX_,
  X___XXXX, XXXX____, XXXX____,
  X___XXXX, ____XXXX, ________,
  ____X___, XXXX____, ________,
  XXXXXXXX, ________, ________,
};

/** $D5D0 (stage3) / $D5A1 (stage5): bitmap_D5D0 */
const pixel_t bitmap_D5D0[2 * 1 * 8 * 1] = {
  X_______, _____X_X,
  X_X_XXXX, XX_X_XX_,
  X_XX__XX, __XX_X__,
  X_XXXX__, XXXX_XX_,
  X_XXXX__, XXXX_XX_,
  X_XX__XX, __XX_X__,
  X_X_XXXX, XX_X_XX_,
  XXXXXXXX, XXXXXXXX,
};

/** $D5E0 (stage3) / $D5B1 (stage5): bitmap_D5E0 */
const pixel_t bitmap_D5E0[2 * 1 * 12 * 1] = {
  X_______, _____X_X,
  X_X_XXXX, XX_X_X_X,
  X_XX__XX, __XX_X_X,
  X_XXXX__, XXXX_X_X,
  X_XXXX__, XXXX_X__,
  X_XX__XX, __XX_X_X,
  X_X_XXXX, XX_X_X__,
  X__XXXXX, XXXX__X_,
  _XX___XX, XX__XX__,
  ___XXX__, __XXXX_X,
  ______XX, XX___X__,
  ________, __XXXXXX,
};

/** $D5F8 (stage3) / $D5C9 (stage5): bitmap_D5F8 */
const pixel_t bitmap_D5F8[12] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, _X_X_X_X, ________, _X_X_X_X, ________, ________,
  ________, XXXXXXXX, ________, XXXXXXXX,
};

/** $D604 (stage3) / $D5D5 (stage5): bitmap_D604 */
const pixel_t bitmap_D604[2 * 1 * 8 * 1] = {
  X_X_____, _______X,
  _XX_X_XX, XXXX_X_X,
  __X_XX__, XX__XX_X,
  _XX_XXXX, __XXXX_X,
  _XX_XXXX, __XXXX_X,
  __X_XX__, XX__XX_X,
  _XX_X_XX, XXXX_X_X,
  XXXXXXXX, XXXXXXXX,
};

/** $D614 (stage3) / $D5E5 (stage5): bitmap_D614 */
const pixel_t bitmap_D614[2 * 1 * 12 * 1] = {
  X_X_____, _______X,
  X_X_X_XX, XXXX_X_X,
  X_X_XX__, XX__XX_X,
  _XX_XXXX, __XXXX_X,
  __X_XXXX, __XXXX_X,
  _XX_XX__, XX__XX_X,
  __X_X_XX, XXXX_X_X,
  _X__XXXX, XXXXX__X,
  __XX__XX, XX___XX_,
  X_XXXX__, __XXX___,
  __X___XX, XX______,
  XXXXXX__, ________,
};

/** $D62C (stage3) / $D5FD (stage5): bitmap_D62C */
const pixel_t bitmap_D62C[1 * 1 * 4 * 1] = {
  X__XX___,
  XXX__XX_,
  X__XX___,
  XXXXXXXX,
};

/** $D630 (stage3) / $D601 (stage5): bitmap_D630 */
const pixel_t bitmap_D630[1 * 1 * 6 * 1] = {
  X__XX___,
  XXX__XX_,
  X__XX___,
  X__XXX__,
  _XX___X_,
  __XXXXXX,
};

/** $D636 (stage3) / $D607 (stage5): bitmap_D636 */
const pixel_t bitmap_D636[7] = {
  ________, XXXXXXXX, XXXXXXXX, _X_X_X_X, X_X_X_X_, ________, XXXXXXXX,
};

/** $D63D (stage3) / $D60E (stage5): bitmap_D63D */
const pixel_t bitmap_D63D[1 * 1 * 4 * 1] = {
  ___XX__X,
  _XX__XXX,
  ___XX__X,
  XXXXXXXX,
};

/** $D641 (stage3) / $D612 (stage5): bitmap_D641 */
const pixel_t bitmap_D641[1 * 1 * 6 * 1] = {
  ___XX__X,
  _XX__XXX,
  ___XX__X,
  __XXX__X,
  _X___XX_,
  XXXXXX__,
};

/* ----------------------------------------------------------------------- */

// clang-format on
