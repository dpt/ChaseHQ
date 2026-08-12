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

static const pixel_t streetlampbody_shifted_pool[64]; /* defined below with 13s/14s/15s data */

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
  { 2, BITMAPFLAG_MASKED, 1, &bitmap_streetlampbody_13[0], &streetlampbody_shifted_pool[0]  },
  { 2, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_14[0], &streetlampbody_shifted_pool[4]  },
  { 2, BITMAPFLAG_MASKED, 2, &bitmap_streetlampbody_15[0], &streetlampbody_shifted_pool[12] }
};

/** $7F25: bitmap_streetlampbody_1 */
const pixel_t bitmap_streetlampbody_1[4] = {
  X______X, _XXXXXX_,
  _______X, XXX__XX_
};

/** $7F29: bitmap_streetlampbody_2 */
const pixel_t bitmap_streetlampbody_2[4] = {
  _______X, XX____X_,
  _______X, X_X___X_
};

/** $7F2D: bitmap_streetlampbody_3 */
const pixel_t bitmap_streetlampbody_3[4] = {
  X_____XX, _XX__X__,
  X_____XX, _X___X__
};

/** $7F31: bitmap_streetlampbody_4 */
const pixel_t bitmap_streetlampbody_4[4] = {
  XX____XX, __XXXX__,
  X______X, _X_X_XX_
};

/** $7F35: bitmap_streetlampbody_5 */
const pixel_t bitmap_streetlampbody_5[4] = {
  X_____XX, _XX__X__,
  X_____XX, _X___X__
};

/** $7F39: bitmap_streetlampbody_6 */
const pixel_t bitmap_streetlampbody_6[4] = {
  X____XXX, _X__X___,
  X____XXX, _X__X___
};

/** $7F3D: bitmap_streetlampbody_7 */
const pixel_t bitmap_streetlampbody_7[2] = {
  ____XXXX, XXXX____
};

/** $7F3F: bitmap_streetlampbody_8 */
const pixel_t bitmap_streetlampbody_8[4] = {
  ____XXXX, XX_X____,
  ____XXXX, XX_X____
};

/** $7F43: bitmap_streetlampbody_9 */
const pixel_t bitmap_streetlampbody_9[4] = {
  ___XXXXX, X_X_____,
  ___XXXXX, X_X_____
};

/** $7F47: bitmap_streetlampbody_7s */
const pixel_t bitmap_streetlampbody_7s[2] = {
  XXXX____, ____XXXX
};

/** $7F49: bitmap_streetlampbody_8s */
const pixel_t bitmap_streetlampbody_8s[4] = {
  XXXX____, ____XX_X,
  XXXX____, ____XX_X
};

/** $7F4D: bitmap_streetlampbody_9s */
const pixel_t bitmap_streetlampbody_9s[4] = {
  XXXX___X, ____X_X_,
  XXXX___X, ____X_X_
};

/** $7F51: bitmap_streetlampbody_10 */
const pixel_t bitmap_streetlampbody_10[2] = {
  XX___XXX, __XXX___
};

/** $7F53: bitmap_streetlampbody_11 */
const pixel_t bitmap_streetlampbody_11[4] = {
  XX___XXX, __X_X___,
  XX___XXX, __X_X___
};

/** $7F57: bitmap_streetlampbody_12 */
const pixel_t bitmap_streetlampbody_12[4] = {
  XX__XXXX, __XX____,
  XX__XXXX, __XX____
};

/** $7F5B: bitmap_streetlampbody_10s */
const pixel_t bitmap_streetlampbody_10s[2] = {
  XXXX___X, ____XXX_
};

/** $7F5D: bitmap_streetlampbody_11s */
const pixel_t bitmap_streetlampbody_11s[4] = {
  XXXX___X, ____X_X_,
  XXXX___X, ____X_X_
};

/** $7F61: bitmap_streetlampbody_12s */
const pixel_t bitmap_streetlampbody_12s[4] = {
  XXXX__XX, ____XX__,
  XXXX__XX, ____XX__
};

/** $7F65: bitmap_streetlampbody_13 */
const pixel_t bitmap_streetlampbody_13[4] = {
  XXXXXXX_, _______X,
  _XXXXXXX, X_______
};

/** $7F69: bitmap_streetlampbody_14 */
const pixel_t bitmap_streetlampbody_14[8] = {
  XXXXXXX_, _______X, _XXXXXXX, X_______,
  XXXXXXX_, _______X, _XXXXXXX, X_______
};

/** $7F71: bitmap_streetlampbody_15 */
const pixel_t bitmap_streetlampbody_15[8] = {
  XXXXXXXX, ________, _XXXXXXX, X_______,
  XXXXXXXX, ________, _XXXXXXX, X_______
};

/**
 * $7F79: streetlampbody_shifted_pool
 *
 * Pre-shifted bitmaps 13s/14s/15s, followed by unused zeros ($7F8D+). The
 * renderer reads across their boundaries as contiguous Z80 memory, so they
 * must be a single C array. Max read: start_offset=2 (1 row skipped),
 * max_height=30, draw_width=4: 2 + 2*29 + 3 = 63 -- need 64 bytes.
 */
static const pixel_t streetlampbody_shifted_pool[64] = {
  /* $7F79: bitmap_streetlampbody_13s */
  XXXXXXXX, ________, XXX__XXX, ___XX___,

  /* $7F7D: bitmap_streetlampbody_14s */
  XXXXXXXX, ________, XXX__XXX, ___XX___,
  XXXXXXXX, ________, XXX__XXX, ___XX___,

  /* $7F85: bitmap_streetlampbody_15s */
  XXXXXXXX, ________, XXXX_XXX, ____X___,
  XXXXXXXX, ________, XXXX_XXX, ____X___,

  /* $7F8D: unused zeros */
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
  0,0,0,0, 0,0,0,0, 0,0,0,0
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

/* ----------------------------------------------------------------------- */

// Conv: Made these bitmap_t's and used NULL for the pre-shifted bitmap field.

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
const pixel_t bitmap_tunnellight_1[2 * 16] = {
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
const pixel_t bitmap_tunnellight_2[2 * 12] = {
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
const pixel_t bitmap_tunnellight_3[1 * 8] = {
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
const pixel_t bitmap_tunnellight_4[1 * 6] = {
  XXXXXXXX,
  _X_X_XXX,
  X___XXXX,
  _____XXX,
  X___XXXX,
  _X_X_XXX,
};

/** $E26F: bitmap_tunnellight_5 */
const pixel_t bitmap_tunnellight_5[1 * 5] = {
  XX_XXXXX,
  X___XXXX,
  _____XXX,
  X___XXXX,
  XX_XXXXX,
};

/** $E274: bitmap_tunnellight_6 */
const pixel_t bitmap_tunnellight_6[2 * 2 * 6] = {
  XX__XXXX, __XX____, XXXXXXXX, ________,
  X____XXX, _X__X___, XXXXXXXX, ________,
  ______XX, X____X__, XXXXXXXX, ________,
  ______XX, X____X__, XXXXXXXX, ________,
  X____XXX, _X__X___, XXXXXXXX, ________,
  XX__XXXX, __XX____, XXXXXXXX, ________,
};

/** $E28C: bitmap_tunnellight_6s */
const pixel_t bitmap_tunnellight_6s[2 * 2 * 6] = {
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
const pixel_t bitmap_heli_part2_frame0[7 * 16] = {
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
const pixel_t bitmap_heli_part1_frame0[3 * 7] = {
  X___X__X, XXXXXXXX, X_XXX___,
  X___X__X, XXXXXXXX, _X_XX___,
  X___XX__, XXXXXXXX, X_X_X___,
  X___X_X_, _XXXXXXX, _X_XX___,
  XXXXX_X_, _XXXXXX_, X_X_XXXX,
  ___X_X_X, __XXXX_X, _XXXX_XX,
  ___X_X__, ___XX_XX, X_XXX_X_,
};

/** $ED6C: bitmap_heli_part2_frame1 */
const pixel_t bitmap_heli_part2_frame1[7 * 16] = {
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
const pixel_t bitmap_heli_part1_frame1[3 * 7] = {
  X___X__X, XXXXXXXX, X_XXX___,
  X___X__X, XXXXXXXX, _X_XX___,
  X___XX__, XXXXXXXX, X_X_X___,
  X___X_X_, _XXXXXXX, _X_XX___,
  XXXXX_X_, _XXXXXX_, X_X_XXXX,
  X__X_X_X, __XXXX_X, _XXXX___,
  _X_X_X__, ___XX_XX, X_XXX___,
};

/** $EDF1: bitmap_heli_part0 (masked: 3 visual columns, 6 raw bytes/row) */
const pixel_t bitmap_heli_part0[2 * 3 * 10] = {
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
const pixel_t bitmap_heli_part3_frame0[2 * 2 * 3] = {
  XXXXXXX_, ________, X____XX_, ___X____,
  XXXXXX__, _______X, ____XXXX, _XX_____,
  XXXXX___, _______X, _____XXX, XX_X____,
};

/** $EE39: bitmap_heli_part4_frame0 (masked: 1 visual column, 2 raw bytes/row) */
const pixel_t bitmap_heli_part4_frame0[2 * 1 * 7] = {
  __XXXXXX, X_______,
  __XXXXXX, X_______,
  ____XXXX, X_______,
  ______XX, XXXX____,
  _______X, XXX_XX__,
  ________, _X_X__X_,
  ____XX_X, X_X_____,
};

/** $EE47: bitmap_heli_part3_frame1 (masked: 1 visual column, 2 raw bytes/row) */
const pixel_t bitmap_heli_part3_frame1[2 * 1 * 5] = {
  XXXXX___, ________,
  XXX_____, _____XXX,
  XX______, ___XXX_X,
  X_______, __X_X_X_,
  XX_X____, _____X_X,
};

/** $EE51: bitmap_heli_part4_frame1 (masked: 2 visual columns, 4 raw bytes/row) */
const pixel_t bitmap_heli_part4_frame1[2 * 2 * 7] = {
  __XXXXXX, X_______, XXXXXXXX, ________,
  __XXXXXX, X_______, XXXXXXXX, ________,
  __XXXXXX, X_______, XXXXXXXX, ________,
  __XXX__X, X_______, XXXXXXXX, ________,
  _XXX____, _____XX_, _XXXXXXX, ________,
  XXXXX___, _______X, __XXXXXX, X_______,
  XXXX_X__, ________, ___XXXXX, XX______,
};

/** $EE6D: bitmap_heli_rotor_frame0 (masked: 5 visual columns, 10 raw bytes/row) */
const pixel_t bitmap_heli_rotor_frame0[2 * 5 * 10] = {
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
const pixel_t bitmap_heli_rotor_frame1[2 * 5 * 10] = {
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

/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */

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
  KEYDEF(4, 3), // 0
  KEYDEF(3, 3), // 9
  KEYDEF(2, 3), // 8
  KEYDEF(0, 3), // 6
  KEYDEF(1, 3)  // 7
};

/** $EE38: temp_keydefs_template -- the assembled contents of the 48K version's
 * scratch keydef buffer, i.e. the keyboard scheme's default key assignments.
 * Layout is [0..4] = gear/accelerate/brake/left/right, [5..7] =
 * quit/pause/turbo. Byte-for-byte identical to the 128K version's
 * default_control_keys[] ($FFF7, Bank3.c); stop_the_tape_48k copies these into
 * state->keydefs when the player picks a scheme that does not redefine them. */
const u8 temp_keydefs_template[8] = {
  KEYDEF(1, 0), // N      -- gear
  KEYDEF(4, 6), // A      -- accelerate
  KEYDEF(3, 7), // Z      -- brake
  KEYDEF(2, 1), // K      -- left
  KEYDEF(3, 1), // L      -- right
  KEYDEF(4, 5), // Q      -- quit
  KEYDEF(4, 2), // P      -- pause
  KEYDEF(4, 0)  // SPACE  -- turbo
};

/* ----------------------------------------------------------------------- */

// clang-format on
