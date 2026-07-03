/**
 * ChaseHQ-CommonData.c
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

#include <stddef.h>

#include "ZXSpectrum/Pixels.h"

#include "C99/Types.h"
#include "../ChaseHQ.h"
#include "ChaseHQ-CommonData.h"

/* ----------------------------------------------------------------------- */

// [Graphics] Turbo icons
//

// $76F0
const u8 bitmap_turbospin[TURBOFRAMELENGTH * TURBOFRAMES] = {
  ________, ________, ___XXXXX, ________,
  ________, _XXXXXXX, ____XXXX, XXX_____,
  ________, _XXXXXXX, _____XXX, XXXX____,
  ________, ________, ______XX, __XXX___,
  ________, _XXX____, _______X, _X_XXX__,
  ________, _XXX_XX_, _______X, _X__XX__,
  X_______, __XX___X, _______X, X___XX__,
  X_______, __XX___X, _______X, X___XX__,
  X_______, __XX__X_, ________, _XX_XXX_,
  X_______, __XXX_X_, ________, ____XXX_,
  XX______, ___XXX__, ________, ________,
  XXX_____, ____XXXX, ________, XXXXXXX_,
  XXXX____, _____XXX, ________, XXXXXXX_,
  XXXXX___, ________, ________, ________,

  ________, ________, ___XXXXX, ________,
  ________, _XXXXXXX, ____XXXX, XXX_____,
  ________, _XXXXXXX, _____XXX, XXXX____,
  ________, ________, ______XX, __XXX___,
  ________, _XXX__X_, _______X, ___XXX__,
  ________, _XXX___X, _______X, __X_XX__,
  X_______, __XX___X, _______X, XX__XX__,
  X_______, __XX__XX, _______X, X___XX__,
  X_______, __XX_X__, ________, X___XXX_,
  X_______, __XXX___, ________, _X__XXX_,
  XX______, ___XXX__, ________, ________,
  XXX_____, ____XXXX, ________, XXXXXXX_,
  XXXX____, _____XXX, ________, XXXXXXX_,
  XXXXX___, ________, ________, ________,

  ________, ________, ___XXXXX, ________,
  ________, _XXXXXXX, ____XXXX, XXX_____,
  ________, _XXXXXXX, _____XXX, XXXX____,
  ________, ________, ______XX, __XXX___,
  ________, _XXX___X, _______X, ___XXX__,
  ________, _XXX____, _______X, X___XX__,
  X_______, __XX__XX, _______X, X_X_XX__,
  X_______, __XX_X_X, _______X, XX__XX__,
  X_______, __XX___X, ________, ____XXX_,
  X_______, __XXX___, ________, X___XXX_,
  XX______, ___XXX__, ________, ________,
  XXX_____, ____XXXX, ________, XXXXXXX_,
  XXXX____, _____XXX, ________, XXXXXXX_,
  XXXXX___, ________, ________, ________
};

// $7798
//
// Conv: Added unused bytes which original game omitted.
const u8 pregame_messages[68] = {
  0xFF, // unused
  attribute_BRIGHT_BLACK_OVER_WHITE,
  TWOBYTES(0xF802),
  TWOBYTES(0x5922),
  'C', 'H', 'A', 'S', 'E', ' ', 'H', '.', 'Q', '.', ' ', 'M', 'O', 'N', 'I', 'T', 'O', 'R', 'I', 'N', 'G', ' ', 'S', 'Y', 'S', 'T', 'E', 'M' | EOS,

  0xFF, // unused
  0x60, // must be attr but doesn't seem to do what it should
  TWOBYTES(0xF863),
  TWOBYTES(0x59E3),
  'T', 'U', 'N', 'E' | EOS,

  0xFF, // unused
  0x60,
  TWOBYTES(0xF8A2),
  TWOBYTES(0x5A62),
  'V', 'O', 'L', 'U', 'M', 'E' | EOS,

  0xFF, // unused
  0x58,
  TWOBYTES(0xF077),
  TWOBYTES(0x59D7),
  'S', 'I', 'G', 'N', 'A', 'L' | EOS
};

// $77D8
const u8 pregame_data[207] = {
  0xDC, // Set colour 12 (Bright Green)
  0xE1, // Draw horizontally
  TWOBYTES(0x01F0), // Set address to (1,0)
  0x1F, // Plot tile 0
  0x1C, // Repeat 28
  0x25, // Plot tile 6
  0x20, // Plot tile 1
  TWOBYTES(0x21F0), // Set address to (1,0)
  0x21, // Plot tile 2
  0x1C, // Repeat 28
  0x24, // Plot tile 5
  0x22, // Plot tile 3
  TWOBYTES(0x01F8), // Set address to (1,1)
  0x23, // Plot tile 4
  TWOBYTES(0x1EF8), // Set address to (30,1)
  0x26, // Plot tile 7
  0xDD, // Set colour 13 (Bright Cyan)
  TWOBYTES(0x41F0), // Set address to (1,4)
  0x1F, // Plot tile 0
  0x1C, // Repeat 28
  0x25, // Plot tile 6
  0x20, // Plot tile 1
  TWOBYTES(0xE1F8), // Set address to (1,13)
  0x21, // Plot tile 2
  0x1C, // Repeat 28
  0x24, // Plot tile 5
  0x22, // Plot tile 3
  0xE2, // Draw vertically
  TWOBYTES(0x41F8), // Set address to (1,5)
  0x0A, // Repeat 10
  0x23, // Plot tile 4
  TWOBYTES(0x5EF8), // Set address to (30,5)
  0x0A, // Repeat 10
  0x26, // Plot tile 7
  0xD0, // Set colour 0 (Black)
  0xE1, // Draw horizontally
  TWOBYTES(0x97F0), // Set address to (23,9)
  0x07, // Repeat 7
  0x27, // Plot tile 8
  TWOBYTES(0xB7F0), // Set address to (23,9)
  0x07, // Repeat 7
  0x27, // Plot tile 8
  TWOBYTES(0x49F8), // Set address to (9,5)
  0xDF, // Set colour 15 (Bright White)
  0x2A, // Plot tile 11
  0x2B, // Plot tile 12
  0x09, // Repeat 9
  0x2C, // Plot tile 13
  0x2D, // Plot tile 14
  0x2E, // Plot tile 15
  0x2F, // Plot tile 16
  TWOBYTES(0x69F0), // Set address to (9,4)
  0xDF, // Set colour 15 (Bright White)
  0x35, // Plot tile 22
  0xDD, // Set colour 13 (Bright Cyan)
  0x36, // Plot tile 23
  0x09, // Repeat 9
  0x37, // Plot tile 24
  0x38, // Plot tile 25
  0x39, // Plot tile 26
  0xDF, // Set colour 15 (Bright White)
  0x3A, // Plot tile 27
  TWOBYTES(0x6BF8), // Set address to (11,5)
  0xD6, // Set colour 6 (Yellow)
  0x42, // Plot tile 35
  0x30, // Plot tile 17
  0x30, // Plot tile 17
  0x04, // Repeat 4
  0x31, // Plot tile 18
  0x30, // Plot tile 17
  0x30, // Plot tile 17
  0x43, // Plot tile 36
  TWOBYTES(0x8BF0), // Set address to (11,8)
  0xDE, // Set colour 14 (Bright Yellow)
  0x0A, // Repeat 10
  0x4B, // Plot tile 44
  TWOBYTES(0x8BF8), // Set address to (11,9)
  0x0A, // Repeat 10
  0x4B, // Plot tile 44
  TWOBYTES(0xABF0), // Set address to (11,8)
  0x0A, // Repeat 10
  0x4B, // Plot tile 44
  TWOBYTES(0xABF8), // Set address to (11,9)
  0x0A, // Repeat 10
  0x4B, // Plot tile 44
  TWOBYTES(0xCBF0), // Set address to (11,12)
  0x0A, // Repeat 10
  0x4B, // Plot tile 44
  0xE2, // Draw vertically
  TWOBYTES(0x69F8), // Set address to (9,5)
  0xD7, // Set colour 7 (White)
  0x40, // Plot tile 33
  0xDD, // Set colour 13 (Bright Cyan)
  0x40, // Plot tile 33
  0xD5, // Set colour 5 (Cyan)
  0x06, // Repeat 6
  0x40, // Plot tile 33
  TWOBYTES(0x76F8), // Set address to (22,5)
  0xD7, // Set colour 7 (White)
  0x45, // Plot tile 38
  0xDD, // Set colour 13 (Bright Cyan)
  0x45, // Plot tile 38
  0xD5, // Set colour 5 (Cyan)
  0x06, // Repeat 6
  0x45, // Plot tile 38
  TWOBYTES(0x6AF8), // Set address to (10,5)
  0x03, // Repeat 3
  0x41, // Plot tile 34
  TWOBYTES(0x75F8), // Set address to (21,5)
  0x03, // Repeat 3
  0x44, // Plot tile 37
  0xE1, // Draw horizontally
  TWOBYTES(0xCAF8), // Set address to (10,13)
  0xDD, // Set colour 13 (Bright Cyan)
  0x3B, // Plot tile 28
  0xDE, // Set colour 14 (Bright Yellow)
  0x3C, // Plot tile 29
  0x08, // Repeat 8
  0x3D, // Plot tile 30
  0x3E, // Plot tile 31
  0xDD, // Set colour 13 (Bright Cyan)
  0x3F, // Plot tile 32
  TWOBYTES(0xEAF0), // Set address to (10,12)
  0x46, // Plot tile 39
  0x47, // Plot tile 40
  0x08, // Repeat 8
  0x48, // Plot tile 41
  0x49, // Plot tile 42
  0x4A, // Plot tile 43
  TWOBYTES(0xEAF8), // Set address to (10,13)
  0xDD, // Set colour 13 (Bright Cyan)
  0x0C, // Repeat 12
  0x32, // Plot tile 19
  TWOBYTES(0x84F0), // Set address to (4,8)
  0xDF, // Set colour 15 (Bright White)
  0x28, // Plot tile 9
  0xDD, // Set colour 13 (Bright Cyan)
  0x29, // Plot tile 10
  TWOBYTES(0x84F8), // Set address to (4,9)
  0xDE, // Set colour 14 (Bright Yellow)
  0x33, // Plot tile 20
  0xD5, // Set colour 5 (Cyan)
  0x34, // Plot tile 21
  TWOBYTES(0xC4F0), // Set address to (4,12)
  0xDF, // Set colour 15 (Bright White)
  0x28, // Plot tile 9
  0xDD, // Set colour 13 (Bright Cyan)
  0x29, // Plot tile 10
  TWOBYTES(0xC4F8), // Set address to (4,13)
  0xDE, // Set colour 14 (Bright Yellow)
  0x33, // Plot tile 20
  0xD5, // Set colour 5 (Cyan)
  0x34, // Plot tile 21
  TWOBYTES(0xD8F0), // Set address to (24,13)
  0xDF, // Set colour 15 (Bright White)
  0x28, // Plot tile 9
  0xDD, // Set colour 13 (Bright Cyan)
  0x29, // Plot tile 10
  TWOBYTES(0xD8F8), // Set address to (24,13)
  0xDE, // Set colour 14 (Bright Yellow)
  0x33, // Plot tile 20
  0xD5, // Set colour 5 (Cyan)
  0x34, // Plot tile 21
  TWOBYTES(0xDBF0), // Set address to (27,13)
  0xDF, // Set colour 15 (Bright White)
  0x28, // Plot tile 9
  0xDD, // Set colour 13 (Bright Cyan)
  0x29, // Plot tile 10
  TWOBYTES(0xDBF8), // Set address to (27,13)
  0xDE, // Set colour 14 (Bright Yellow)
  0x33, // Plot tile 20
  0xD5, // Set colour 5 (Cyan)
  0x34, // Plot tile 21
  0x00 // Stop
};

// $78A7
//
// 45 tiles used to draw the pre-game screen.
const u8 pregame_tiles[45 * 8] = {
  ________,
  _X_X_X_X,
  __XXXXXX,
  _XXXXXXX,
  __XXXXXX,
  _XXXXXXX,
  __XXXXXX,
  _XXXXXXX,

  ________,
  _X_X_X__,
  XXXXXXX_,
  XXXXXX__,
  XXXXXXX_,
  XXXXXX__,
  XXXXXXX_,
  XXXXXX__,

  __XXXXXX,
  _XXXXXXX,
  __XXXXXX,
  _XXXXXXX,
  __XXXXXX,
  _XXXXXXX,
  __X_X_X_,
  ________,

  XXXXXXX_,
  XXXXXX__,
  XXXXXXX_,
  XXXXXX__,
  XXXXXXX_,
  XXXXXX__,
  X_X_X_X_,
  ________,

  __XXXXXX,
  _XXXXXXX,
  __XXXXXX,
  _XXXXXXX,
  __XXXXXX,
  _XXXXXXX,
  __XXXXXX,
  _XXXXXXX,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  X_X_X_X_,
  ________,

  ________,
  _X_X_X_X,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXXXX_,
  XXXXXX__,
  XXXXXXX_,
  XXXXXX__,
  XXXXXXX_,
  XXXXXX__,
  XXXXXXX_,
  XXXXXX__,

  X_______,
  X_______,
  X_______,
  X_______,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXX___,
  XXX_____,
  XX___XXX,
  X___X_X_,
  X__X___X,
  __X___XX,
  __XX_XXX,
  __X_XXXX,

  ___XXXXX,
  _____XXX,
  XXX_X_XX,
  X_XXXXXX,
  _XXXX_XX,
  XXXXXXXX,
  XXXXXX_X,
  XXXXXXXX,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXX__,
  XXXXXX__,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  ________,
  ________,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  ________,
  ________,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  ________,
  ________,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  ________,
  ________,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  __XXXXXX,
  __XXXXXX,

  XXXXXXXX,
  _X_X_X_X,
  X_X_X_X_,
  ________,
  ________,
  ________,
  ________,
  ________,

  XXXXXXXX,
  _X_X_X_X,
  ________,
  ________,
  ________,
  ________,
  ________,
  ________,

  ________,
  ________,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  X_X_X_X_,
  ________,

  __XX_XXX,
  __X_XXXX,
  __XXXXXX,
  X__XXXXX,
  X___XXXX,
  XX___XXX,
  XXX_____,
  XXXXX___,

  XXXXXX_X,
  XXXXXXXX,
  XXXXXX_X,
  XXXXX_XX,
  XXXXXXXX,
  XXX_X_XX,
  _____XXX,
  ___XXXXX,

  XXXXXX__,
  XXXXXX__,
  XXXXXX__,
  XXXXXX__,
  XXXXXX__,
  XXXXXX__,
  XXXXXX__,
  XXXXXX__,

  _XXXXXXX,
  XXXXXXXX,
  XX_X_X_X,
  XXX_X_X_,
  XX_X_X_X,
  XXX_XXXX,
  XX_X_XXX,
  XXX_XXXX,

  XXXXXXXX,
  XXXXXXXX,
  _X_X_X_X,
  X_X_X_X_,
  _X_X_X_X,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXXXXX,
  XXXXXXXX,
  _X_X_X_X,
  X_X_X_X_,
  _X_X_X_X,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXXXX_,
  XXXXXXXX,
  _X_X_XXX,
  X_X_X_XX,
  _X_X_XXX,
  XXX_X_XX,
  XXXX_XXX,
  XXX_X_XX,

  __XXXXXX,
  __XXXXXX,
  __XXXXXX,
  __XXXXXX,
  __XXXXXX,
  __XXXXXX,
  __XXXXXX,
  __XXXXXX,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXX_,
  XXXXXX_X,
  XXXXXXX_,
  XXXXXX_X,
  XXXXXX__,

  X_______,
  X_______,
  X_______,
  X_______,
  X_______,
  _X______,
  XX______,
  __XX____,

  ________,
  ________,
  ________,
  ________,
  ________,
  ________,
  ________,
  ________,

  _______X,
  _______X,
  _______X,
  _______X,
  _______X,
  ______XX,
  ______X_,
  ____XX__,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  _XXXXXXX,
  X_XXXXXX,
  _XXXXXXX,
  X_XXXXXX,
  __XXXXXX,

  XXXXXX__,
  XXXXXX__,
  XXXXXX__,
  XXXXXX__,
  XXXXXX__,
  XXXXXX__,
  XXXXXX__,
  XXXXXX__,

  XX_X_XXX,
  XXX_XXXX,
  XX_X_XXX,
  XXX_XXXX,
  XX_X_XXX,
  XXX_XXXX,
  XX_X_XXX,
  XXX_XXXX,

  XXXXXXXX,
  XXXX_X_X,
  XX__X_X_,
  XX______,
  X_X_____,
  XX______,
  X_______,
  XX______,

  XXXXXXXX,
  _X_XXXXX,
  X_X_X_XX,
  _____XXX,
  _______X,
  ______XX,
  _______X,
  ______XX,

  XXXX_XXX,
  XXX_X_XX,
  XXXX_XXX,
  XXX_X_XX,
  XXXX_XXX,
  XXX_X_XX,
  XXXX_XXX,
  XXX_X_XX,

  __XXXXXX,
  __XXXXXX,
  __XXXXXX,
  __XXXXXX,
  __XXXXXX,
  __XXXXXX,
  __XXXXXX,
  __XXXXXX,

  XXXXXX__,
  XXXXXX__,
  XXXXXX__,
  XXXXXXX_,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  _XXXXXXX,

  ____XXXX,
  _____X_X,
  __X_X_X_,
  _X_X_X_X,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXXXXX,
  _X_X_X_X,
  X_X_X_X_,
  _X_X_X_X,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXX___,
  _X_X_X__,
  X_X_X___,
  _X_X_X_X,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,

  __XXXXXX,
  __XXXXXX,
  __XXXXXX,
  _XXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXX_,

  ________,
  ________,
  ________,
  ________,
  ________,
  ________,
  ________,
  ________
};

// [Graphics] Smoke and fire graphics
//

// $7A0F
const u8 bitmap_smoke1[4 * 13] = {
  XXXX____, ____XXXX, __XXXXXX, XX______,
  XX______, __XXXXXX, _____XXX, XXXXX___,
  X_______, _XXXXX_X, ______XX, _XXXXX__,
  X_______, _X_X_XXX, _______X, X_X_XXX_,
  X_______, __X_____, _______X, _X_X_XX_,
  X_______, _X______, _______X, _____XX_,
  X_______, ________, ______XX, ____XX__,
  X_______, _X______, ______XX, _____X__,
  X_______, ________, _______X, ______X_,
  X_______, _X______, ______XX, ________,
  XX______, ________, _______X, ______X_,
  XX______, __X____X, ______XX, ___X_X__,
  XXXX____, ____X_X_, ___XXXXX, X_X_____
};

// $7A43
const u8 bitmap_smoke2[4 * 11] = {
  XXXXX___, _____XXX, _XXXXXXX, X_______,
  XXX_____, ___XXXXX, ____XXXX, XXXX____,
  XX______, __XX_XXX, _____XXX, XXXXX___,
  XX______, __X_X_XX, ______XX, XX_XXX__,
  XX______, ________, ______XX, _XX_XX__,
  XX______, ___X____, ______XX, ___X_X__,
  XX______, __X_____, _____XXX, ____X___,
  XX______, ________, _____XXX, ________,
  XX______, ________, _____XXX, ____X___,
  XX______, __X_____, ____XXXX, ________,
  XXX____X, ___XX_X_, ____XXXX, X_XX____
};

// $7A6F
const u8 bitmap_smoke3[4 * 9] = {
  XXXX____, ____XXXX, __XXXXXX, XX______,
  XXX_____, ___XXXXX, ____XXXX, XXXX____,
  XX______, __XXX_XX, _____XXX, XXXXX___,
  XX______, __X____X, _____XXX, _XX_X___,
  XX______, ___X____, _____XXX, ___X____,
  XXX_____, ________, _____XXX, ____X___,
  XXX_____, ___X____, ____XXXX, ________,
  XXX_____, ____X___, ____XXXX, ___X____,
  XXXXX___, ______X_, ___XXXXX, X_X_____
};

// $7A93
const u8 bitmap_smoke4[2 * 7] = {
  X_____XX, _XXXXX__,
  ________, XXXXXXXX,
  ________, X_X_X_XX,
  ________, X____X_X,
  ________, ________,
  ________, X______X,
  XX_____X, __X_X_X_
};

// $7AA1
const u8 bitmap_smoke5[2 * 5] = {
  XX_____X, __XXXXX_,
  X______X, _XX_XXX_,
  X______X, _X___X__,
  X______X, _X____X_,
  XX____XX, ___X_X__
};

// $7AAB
const u8 bitmap_smoke6[2 * 3] = {
  XX____XX, __XXXX__,
  XX____XX, __X__X__,
  XX____XX, __X_X___
};

// $7AB1
const u8 bitmap_fire1[4 * 16] = {
  ___XXXX_, XXXXXXXX, XXXXXXXX, XXX_____,
  _______X, ________, ________, _X_XXX__,
  __X_____, ________, ________, ____X___,
  _X______, ________, ________, ______X_,
  _X______, ________, ________, _______X,
  _XX_____, ________, ________, _______X,
  _X______, ________, ________, _______X,
  X____X__, ________, ________, _____X_X,
  X_______, _X______, X___X_X_, X_____X_,
  X_X__X__, __X____X, ___XX__X, ____X__X,
  _X___XX_, _X_____X, ___X_X_X, _______X,
  __X__X_X, _XX____X, X__X_X_X, ____X__X,
  X_X__X__, X_X___X_, _X_X__X_, X___X__X,
  _XXX__XX, _X____X_, __XXX___, X___X_X_,
  _X_X__X_, X__X__X_, __X__X__, _X_X_X__,
  X__X_X__, ____XX__, ___X_X__, __X_____
};

// $7AF1
const u8 bitmap_fire2[4 * 16] = {
  ___XXXX_, XXXXXXXX, XXXXXXXX, XXX_____,
  __X____X, ________, ________, _X_XXX__,
  _X______, ________, ________, ____X_X_,
  _XX_____, ________, ________, _______X,
  _X______, ________, ________, ______X_,
  X_______, ________, ________, _______X,
  _X______, ________, ________, ______XX,
  X____X__, ________, ________, ________,
  X_____X_, ___X____, ________, ______X_,
  X_X__X__, ________, X_______, ______X_,
  X____XX_, ___X___X, _______X, __X_X__X,
  X_X__XX_, __X____X, X______X, X_XX_X_X,
  X_X__X_X, _X_X__XX, _X__X___, _X_X__X_,
  _X_X__XX, _X_X__X_, _X_X_X_X, X_X__XX_,
  ____XX_X, _X_X_X__, _X_X__XX, _X__X__X,
  ________, X___X___, __X____X, X____X_X
};

// $7B31
const u8 bitmap_fire3[6 * 8] = {
  XX______, __XXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  X_______, _X______, ________, ________, ____XXXX, ___X____,
  ________, X_______, ________, ________, _____XXX, ____X___,
  ________, X__X__X_, ________, ________, _____XXX, ____X___,
  ________, X______X, ________, _X__X__X, _____XXX, _X__X___,
  ________, X_X__X_X, ________, X___X___, _____XXX, X___X___,
  X_____XX, _XX__X__, X_______, _X__X___, ____XXXX, XX_X____,
  XX____XX, __XX_X__, X___X___, _X_X_X_X, X___XXXX, _X_X____
};

// $7B61
const u8 bitmap_fire4[6 * 8] = {
  XX____X_, __XXXX_X, ________, XXXXXXXX, ___XXXXX, XXX_____,
  X_______, _X____X_, ________, ________, ____XXXX, __XX____,
  ________, X_______, ________, ________, _____XXX, ____X___,
  ________, X_______, ________, ________, _____XXX, ___XX___,
  ________, X_______, ________, X_____X_, _____XXX, ____X___,
  ________, X_X_____, ________, _X__X__X, _____XXX, ___XX___,
  ________, X_X__X__, ____X___, XX_X_X_X, ____XXXX, X_XX____,
  X_X_____, _X_X_XX_, ____XX__, XX_X__XX, _XX_XXXX, ___X____
};

// $7B91
const u8 bitmap_fire5[4 * 5] = {
  X_______, _XXXXXXX, ___XXXXX, XXX_____,
  ________, X_______, ____XXXX, ___X____,
  ________, ___X____, ____XXXX, X__X____,
  ________, X_XX_X_X, X___XXXX, _X_X____,
  XXXX_X_X, ____X_X_, XX_XXXXX, __X_____
};

// $7BA5
const u8 bitmap_fire5s[4 * 5] = {
  XXXXX___, _____XXX, _______X, XXXXXXX_,
  XXXX____, ____X___, ________, _______X,
  XXXX____, _______X, ________, ____X__X,
  XXXX____, ____X_XX, ____X___, _X_X_X_X,
  XXXXXXXX, ________, _X_XXX_X, X_X___X_
};

// $7BB9
const u8 bitmap_fire6[4 * 6] = {
  X_______, _XXXXXXX, __XXXXXX, XX______,
  ________, X_______, ___XXXXX, __X_____,
  ________, X_____X_, ____XXXX, _X_X____,
  ________, _X_X__XX, ____XXXX, X__X____,
  _X____XX, X_XX_X__, X_X_XXXX, _X_X____,
  XXXX_XXX, ____X___, XXXXXXXX, ________
};

// $7BD1
const u8 bitmap_fire6s[4 * 6] = {
  XXXXX___, _____XXX, ______XX, XXXXXX__,
  XXXX____, ____X___, _______X, ______X_,
  XXXX____, ____X___, ________, __X__X_X,
  XXXX____, _____X_X, ________, __XXX__X,
  XXXX_X__, ____X_XX, __XXX_X_, _X___X_X,
  XXXXXXXX, ________, _XXXXXXX, X_______
};

// [Graphics] Faces
//

// $7BE9
const u8 bitmap_faces[FACEBYTES * NFACES] = {
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

  0x78, 0x70, 0x30, 0x30,
  0x70, 0x70, 0x70, 0x30,
  0x70, 0x72, 0x70, 0x30,
  0x30, 0x70, 0x70, 0x30,
  0x30, 0x30, 0x28, 0x30,

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

  0x68, 0x38, 0x78, 0x68,
  0x38, 0x38, 0x38, 0x38,
  0x38, 0x38, 0x38, 0x38,
  0x68, 0x38, 0x38, 0x68,
  0x68, 0x38, 0x38, 0x78,

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

  0x38, 0x70, 0x30, 0x30,
  0x38, 0x70, 0x70, 0x30,
  0x38, 0x70, 0x70, 0x30,
  0x38, 0x70, 0x70, 0x30,
  0x30, 0x70, 0x70, 0x28,
};

// [Graphics] Street lamps etc.
//

const stretchy_t stretchy_shortpole[3] = {
  { STRETCHY_TYPE_FIXED, &shortpole_bottom },
  { STRETCHY_TYPE_50PC,  &shortpole_middle },
  { STRETCHY_TYPE_END, NULL } // Conv: NULL added
};

// $7E0C
//
// Why is the first column all zeroes?
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

// $7E22
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

// $7E38
//
// seems to be shared streetlamp/telegraphpole
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

// $7E4E
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

// $7E64
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

// $7E7A
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

// $7E90
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

// $7EA6
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

static const u8 streetlampbody_shifted_pool[64]; /* defined below with 13s/14s/15s data */

// $7EBC
const bitmap_t streetlampbody_bitmaps[15] = {
  { 1, 1, 2, &bitmap_streetlampbody_1[0],  &bitmap_streetlampbody_1[0]   },
  { 1, 1, 2, &bitmap_streetlampbody_2[0],  &bitmap_streetlampbody_2[0]   },
  { 1, 1, 2, &bitmap_streetlampbody_3[0],  &bitmap_streetlampbody_3[0]   },
  { 1, 1, 2, &bitmap_streetlampbody_4[0],  &bitmap_streetlampbody_4[0]   },
  { 1, 1, 2, &bitmap_streetlampbody_5[0],  &bitmap_streetlampbody_5[0]   },
  { 1, 1, 2, &bitmap_streetlampbody_6[0],  &bitmap_streetlampbody_6[0]   },
  { 1, 1, 1, &bitmap_streetlampbody_7[0],  &bitmap_streetlampbody_7s[0]  },
  { 1, 1, 2, &bitmap_streetlampbody_8[0],  &bitmap_streetlampbody_8s[0]  },
  { 1, 1, 2, &bitmap_streetlampbody_9[0],  &bitmap_streetlampbody_9s[0]  },
  { 1, 1, 1, &bitmap_streetlampbody_10[0], &bitmap_streetlampbody_10s[0] },
  { 1, 1, 2, &bitmap_streetlampbody_11[0], &bitmap_streetlampbody_11s[0] },
  { 1, 1, 2, &bitmap_streetlampbody_12[0], &bitmap_streetlampbody_12s[0] },
  { 2, 1, 1, &bitmap_streetlampbody_13[0], &streetlampbody_shifted_pool[0]  },
  { 2, 1, 2, &bitmap_streetlampbody_14[0], &streetlampbody_shifted_pool[4]  },
  { 2, 1, 2, &bitmap_streetlampbody_15[0], &streetlampbody_shifted_pool[12] }
};

// $7F25
const u8 bitmap_streetlampbody_1[4] = {
  X______X, _XXXXXX_,
  _______X, XXX__XX_
};

// $7F29
const u8 bitmap_streetlampbody_2[4] = {
  _______X, XX____X_,
  _______X, X_X___X_
};

// $7F2D
const u8 bitmap_streetlampbody_3[4] = {
  X_____XX, _XX__X__,
  X_____XX, _X___X__
};

// $7F31
const u8 bitmap_streetlampbody_4[4] = {
  XX____XX, __XXXX__,
  X______X, _X_X_XX_
};

// $7F35
const u8 bitmap_streetlampbody_5[4] = {
  X_____XX, _XX__X__,
  X_____XX, _X___X__
};

// $7F39
const u8 bitmap_streetlampbody_6[4] = {
  X____XXX, _X__X___,
  X____XXX, _X__X___
};

// $7F3D
const u8 bitmap_streetlampbody_7[2] = {
  ____XXXX, XXXX____
};

// $7F3F
const u8 bitmap_streetlampbody_8[4] = {
  ____XXXX, XX_X____,
  ____XXXX, XX_X____
};

// $7F43
const u8 bitmap_streetlampbody_9[4] = {
  ___XXXXX, X_X_____,
  ___XXXXX, X_X_____
};

// $7F47
const u8 bitmap_streetlampbody_7s[2] = {
  XXXX____, ____XXXX
};

// $7F49
const u8 bitmap_streetlampbody_8s[4] = {
  XXXX____, ____XX_X,
  XXXX____, ____XX_X
};

// $7F4D
const u8 bitmap_streetlampbody_9s[4] = {
  XXXX___X, ____X_X_,
  XXXX___X, ____X_X_
};

// $7F51
const u8 bitmap_streetlampbody_10[2] = {
  XX___XXX, __XXX___
};

// $7F53
const u8 bitmap_streetlampbody_11[4] = {
  XX___XXX, __X_X___,
  XX___XXX, __X_X___
};

// $7F57
const u8 bitmap_streetlampbody_12[4] = {
  XX__XXXX, __XX____,
  XX__XXXX, __XX____
};

// $7F5B
const u8 bitmap_streetlampbody_10s[2] = {
  XXXX___X, ____XXX_
};

// $7F5D
const u8 bitmap_streetlampbody_11s[4] = {
  XXXX___X, ____X_X_,
  XXXX___X, ____X_X_
};

// $7F61
const u8 bitmap_streetlampbody_12s[4] = {
  XXXX__XX, ____XX__,
  XXXX__XX, ____XX__
};

// $7F65
const u8 bitmap_streetlampbody_13[4] = {
  XXXXXXX_, _______X,
  _XXXXXXX, X_______
};

// $7F69
const u8 bitmap_streetlampbody_14[8] = {
  XXXXXXX_, _______X, _XXXXXXX, X_______,
  XXXXXXX_, _______X, _XXXXXXX, X_______
};

// $7F71
const u8 bitmap_streetlampbody_15[8] = {
  XXXXXXXX, ________, _XXXXXXX, X_______,
  XXXXXXXX, ________, _XXXXXXX, X_______
};

/* $7F79: pre-shifted bitmaps 13s/14s/15s, followed by unused zeros ($7F8D+).
 * The renderer reads across their boundaries as contiguous Z80 memory, so
 * they must be a single C array. Max read: start_offset=2 (1 row skipped),
 * max_height=30, draw_width=4: 2 + 2*29 + 3 = 63 → need 64 bytes. */
static const u8 streetlampbody_shifted_pool[64] = {
  /* $7F79: bitmap_streetlampbody_13s */
  XXXXXXXX, ________, XXX__XXX, ___XX___,
  /* $7F7D: bitmap_streetlampbody_14s */
  XXXXXXXX, ________, XXX__XXX, ___XX___, XXXXXXXX, ________, XXX__XXX, ___XX___,
  /* $7F85: bitmap_streetlampbody_15s */
  XXXXXXXX, ________, XXXX_XXX, ____X___, XXXXXXXX, ________, XXXX_XXX, ____X___,
  /* $7F8D: unused zeros */
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
  0,0,0,0, 0,0,0,0, 0,0,0,0
};

/* ----------------------------------------------------------------------- */

// $81DD
const u8 chatterblk_start_stage[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_TONY_GIDDY_UP,
  CHATTERBLK_TONY_HOLD_ON,
  CHATTERBLK_TONY_LETS_GO
};

// $81E4
const u8 chatterblk_tony_giddy_up[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_GIDDY_UP_BOY,
  CHATTERCMD_STOP
};

// $81E8
const u8 chatterblk_tony_hold_on[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_HOLD_ON_MAN,
  CHATTERCMD_STOP
};

// Conv: Chatter strings from two locations are combined into this table.
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
  "WHAT ARE YOU DOING MAN!\xA1",
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
  "SEE YOU LATER\xAE"
};

// $82A6
const u8 attract_messages[38] = {
  DRAWCHARSTYLE_DOUBLE,
  attribute_BLACK_OVER_BLACK, // zero
  TWOBYTES(0xF02C), // back buffer addr
  TWOBYTES(0x594C), // attr addr
  'C', 'H', 'A', 'S', 'E', ' ', 'H', 'Q' | EOS,

  DRAWCHARSTYLE_SINGLE,
  attribute_BLACK_OVER_BLACK, // zero
  TWOBYTES(0xF847),
  TWOBYTES(0x59A7),
  'P', 'R', 'E', 'S', 'S', ' ', 'G', 'E', 'A', 'R', ' ', 'T', 'O', ' ', 'P', 'L', 'A', 'Y' | EOS
};

// $82CC
const u8 credits_messages[84] = {
  10, // frame delay
  8, // vertical gap?
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK, // 2
  TWOBYTES(0xF086),
  TWOBYTES(0x5A06),
  'P', 'R', 'O', 'G', 'R', 'A', 'M', ' ', ' ', ' ', ' ', ' ', ' ', 'J', 'O', 'B', 'B', 'E', 'E', 'E' | EOS,
  8,
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK, // 2
  TWOBYTES(0xF0A6),
  TWOBYTES(0x5A46),
  'G', 'R', 'A', 'P', 'H', 'I', 'C', 'S', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'I', 'L', 'L' | EOS,
  0x28,
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK, // 2
  TWOBYTES(0xF0C6),
  TWOBYTES(0x5A86),
  'M', 'U', 'S', 'I', 'C', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'J', 'O', 'N', ' ', 'D', 'U', 'N', 'N' | EOS,
  3,
  0
};

// $8320
const u8 copyright_messages[92] = {
  10,
  8,
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK, // 2
  TWOBYTES(0xF084),
  TWOBYTES(0x5A04),
  '(', 'C', ')', ' ', '1', '9', '8', '9', ' ', 'O', 'C', 'E', 'A', 'N', ' ', 'S', 'O', 'F', 'T', 'W', 'A', 'R', 'E' | EOS,
  8,
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK, // 2
  TWOBYTES(0xF0A3),
  TWOBYTES(0x5A43),
  '(', 'C', ')', ' ', '1', '9', '8', '8', ' ', 'T', 'A', 'I', 'T', 'O', ' ', 'C', 'O', 'R', 'P', 'O', 'R', 'A', 'T', 'I', 'O', 'N' | EOS,
  8,
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK, // 2
  TWOBYTES(0xF0C6),
  TWOBYTES(0x5A86),
  'A', 'L', 'L', ' ', 'R', 'I', 'G', 'H', 'T', 'S', ' ', 'R', 'E', 'S', 'E', 'R', 'V', 'E', 'D' | EOS,
  3,
  0
};

// $871A
const scenedata_t escape_scene_data = {
  308, // road_pos
  &perp_escape_curvature[-1],
  &perp_escape_height[-1],
  &perp_escape_lanes[-1],
  &perp_escape_curvature[-1],
  &perp_escape_curvature[-1],
  &perp_escape_hazards[-1],
};

const hazard_t escape_scene_perp = {
  HAZARD_USED,
  1,      // distance
  0,
  0,
  0,
  0x4C,   // horz_pos_on_road
  0,
  0,
  {
    0x2C,
    NULL,
  },
  no_op,  // hit_handler (ptr!)
  250,    // speed
  0,
  0,
  0,
  0,
  0
};

// $897C
// Conv: This is now a template copied into state.
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

// $8C58
// Conv: This is now a template copied into state.
const u8 score_messages_template[90] = {
  5, 4, 3,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0xF025),
  TWOBYTES(0x5945),
  'C', 'L', 'E', 'A', 'R', ' ', 'B', 'O', 'N', 'U', 'S', ' ', ' ', ' ', ' ', ' ', ' ', '0', ',', '0', '0', '0' | EOS,
  4, 3,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0xF065),
  TWOBYTES(0x59C5),
  'T', 'I', 'M', 'E', ' ', 'B', 'O', 'N', 'U', 'S', ' ', ' ', ' ', ' ', ' ', ' ', 'X', ' ', '5', '0', '0', '0' | EOS,
  0x32, 3,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0xF0A5),
  TWOBYTES(0x5A45),
  'S', 'C', 'O', 'R', 'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' | EOS,
  3, 0
};

// $8CB2
const u8 sighting_message[36] = {
  0x01, 0x1E, 0x02,
  attribute_BLACK_OVER_BLACK,
  TWOBYTES(0xF023),
  TWOBYTES(0x5943),
  'S', 'I', 'G', 'H', 'T', 'I', 'N', 'G', ' ', 'O', 'F', ' ', 'T', 'A', 'R', 'G', 'E', 'T', ' ', 'V', 'E', 'H', 'I', 'C', 'L', 'E' | EOS,
  0, 0
};

// $8CD6
const u8 pull_over_message[30] = {
  0x01, 0x1E, 0x02,
  attribute_BLACK_OVER_BLACK,
  TWOBYTES(0xF026),
  TWOBYTES(0x5946),
  'O', 'K', '!', ' ', 'P', 'U', 'L', 'L', ' ', 'O', 'V', 'E', 'R', ' ', 'C', 'R', 'E', 'E', 'P', '!' | EOS,
  0, 0
};

// $8CF4
const u8 game_over_message[20] = {
  0x01, 0x1E, 0x03,
  attribute_BLACK_OVER_BLACK,
  TWOBYTES(0xF02B),
  TWOBYTES(0x594B),
  'G', 'A', 'M', 'E', ' ', 'O', 'V', 'E', 'R' | EOS,
  3, 0
};

// $8D07
const u8 time_up_message[20] = {
  0x01, 0x19, 0x03,
  attribute_BLACK_OVER_BLACK,
  TWOBYTES(0xF02C),
  TWOBYTES(0x594C),
  'T', 'I', 'M', 'E', ' ', 'U', 'P' | EOS,
  3, 0
};

// $8D18
const u8 continue_messages_template[119] = {
  0x01, 0x01, 0x03,
  attribute_BLACK_OVER_BLACK,
  TWOBYTES(0xF02C),
  TWOBYTES(0x594C),
  'C', 'O', 'N', 'T', 'I', 'N', 'U', 'E', ' ' | EOS,
  0x01, 0x03,
  attribute_BLACK_OVER_BLACK,
  TWOBYTES(0xF04A),
  TWOBYTES(0x598A),
  'T', 'H', 'I', 'S', ' ', 'M', 'I', 'S', 'S', 'I', 'O', 'N' | EOS,
  0x01, 0x02,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0xF868),
  TWOBYTES(0x59E8),
  'P', 'U', 'S', 'H', ' ', 'G', 'E', 'A', 'R', ' ', 'B', 'U', 'T', 'T', 'O', 'N' | EOS,
  0x01, 0x02,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0xF086),
  TWOBYTES(0x5A06),
  'B', 'E', 'F', 'O', 'R', 'E', ' ', 'T', 'I', 'M', 'E', 'R', ' ', 'R', 'E', 'A', 'C', 'H', 'E', 'S', ' ', '0' | EOS,
  0x01, 0x02,
  attribute_BLACK_OVER_BLACK,
  TWOBYTES(0xF0AC),
  TWOBYTES(0x5A4C),
  'T', 'I', 'M', 'E', ' ', '1', '0' | EOS,
  0x0A, 0x02,
  attribute_BLACK_OVER_BLACK,
  TWOBYTES(0xF8F6),
  TWOBYTES(0x5AF6),
  'C', 'R', 'E', 'D', 'I', 'T', ' ', ' ' | EOS,
  0x03, 0x00
};

// $9D5B
const u8 stage_n_template[7] = {
  'S', 'T', 'A', 'G', 'E', ' ', ' ' | EOS,
};

// $98A9
const u8 chatterblk_pilot_turn_left[6] = {
  CHATTERCHR_PILOT,
  CHATTERSTR_THIS_IS_AIRBORNE,
  CHATTERSTR_TARGET_VEHICLE_TURNED,
  CHATTERSTR_LEFT_AHEAD_OVER,
  CHATTERCMD_PAUSE,
  CHATTERBLK_TONY_LOUD_CLEAR
};

// $98B3
const u8 chatterblk_pilot_turn_right[6] = {
  CHATTERCHR_PILOT,
  CHATTERSTR_THIS_IS_AIRBORNE,
  CHATTERSTR_TARGET_VEHICLE_TURNED,
  CHATTERSTR_RIGHT_AHEAD_OVER,
  CHATTERCMD_PAUSE,
  CHATTERBLK_TONY_LOUD_CLEAR
};

// $98BD
const u8 chatterblk_heroes_acknowledge[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_TONY_LOUD_CLEAR,
  CHATTERBLK_RAYMOND_ROGER,
  CHATTERBLK_TONY_GOTCHA,
};

// $98C4
const u8 chatterblk_tony_loud_clear[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_READ_LOUD_CLEAR,
  CHATTERCMD_STOP
};

// $98C8
const u8 chatterblk_raymond_roger[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_ROGER,
  CHATTERCMD_STOP
};

// $98CC
const u8 chatterblk_tony_gotcha[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_GOTCHA_NANCY,
  CHATTERCMD_STOP
};

// $98D0
const u8 chatterblk_raymond_wrong_way[4] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_WHAT_YOU_DOING,
  CHATTERSTR_GOING_OTHER_WAY,
  CHATTERCMD_STOP
};

// $98D6
const u8 chatterblk_raymond_smash[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_RAYMOND_BEAR_DOWN,
  CHATTERBLK_RAYMOND_RANDOM_PLEAS,
  CHATTERBLK_RAYMOND_PUSH_IT
};

// $98DD
const u8 chatterblk_raymond_bear_down[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_BEAR_DOWN,
  CHATTERCMD_STOP
};

// $98E1
const u8 chatterblk_raymond_push_it[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_MORE_PUSH_MORE,
  CHATTERCMD_STOP
};

// $98E5
const u8 chatterblk_raymond_harder[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_HARDER,
  CHATTERCMD_STOP
};

// $98E9
const u8 chatterblk_raymond_oh_man[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_OH_MAN,
  CHATTERCMD_STOP
};

// $98ED
const u8 chatterblk_raymond_random_pleas[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_RAYMOND_OH_MAN,
  CHATTERBLK_RAYMOND_HARDER,
  CHATTERBLK_RAYMOND_PLEASE
};

// $98F4
const u8 chatterblk_raymond_please[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_PLEASE,
  CHATTERCMD_STOP
};

// $98F8
const u8 chatterblk_raymond_get_moving[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_GET_MOVIN_MAN,
  CHATTERCMD_STOP
};

// $98FC
const u8 chatterblk_nancy_time_running_out[5] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_THIS_IS_NANCY,
  CHATTERSTR_MESSIN_AROUND,
  CHATTERSTR_TIME_RUN_OUT,
  CHATTERCMD_STOP
};

// $9904
const u8 chatterblk_raymond_random_yelps[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_RAYMOND_OHNO,
  CHATTERBLK_RAYMOND_OUCH,
  CHATTERBLK_RAYMOND_YAOW
};

// $990B
const u8 chatterblk_raymond_ohno[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_OH_NO,
  CHATTERCMD_STOP
};

// $990F
const u8 chatterblk_raymond_ouch[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_OUCH,
  CHATTERCMD_STOP
};

// $9913
const u8 chatterblk_raymond_yaow[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_YAOW,
  CHATTERCMD_STOP
};

// $9917
const u8 chatterblk_turbo[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_TONY_WHOA,
  CHATTERBLK_TONY_GREAT,
  CHATTERBLK_RAYMOND_ONE_MORE_TIME
};

// $991E
const u8 chatterblk_tony_whoa[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_WHOA,
  CHATTERCMD_STOP
};

// $9922
const u8 chatterblk_tony_great[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_GREAT,
  CHATTERCMD_STOP
};

// $9926
const u8 chatterblk_raymond_one_more_time[3] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_ONE_MORE_TIME,
  CHATTERCMD_STOP
};

// $992A
const u8 chatterblk_nancy_berates_hero[4] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_NANCY_WRONG_JOB,
  CHATTERBLK_NANCY_ONE_MORE_TRY,
  CHATTERBLK_NANCY_MEDIOCRE_DRIVER
};

// $9931
const u8 chatterblk_nancy_wrong_job[3] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_PICKED_WRONG_JOB,
  CHATTERCMD_STOP
};

// $9937
const u8 chatterblk_nancy_one_more_try[3] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_ONE_MORE_TRY,
  CHATTERCMD_STOP
};

// $993B
const u8 chatterblk_nancy_mediocre_driver[4] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_MEDIOCRE_DRIVER,
  CHATTERSTR_SEE_YOU_LATER,
  CHATTERCMD_STOP
};

// $9941
const u8 chatterblk_tony_lets_go[3] = {
  CHATTERCHR_TONY,
  CHATTERSTR_LETS_GO,
  CHATTERCMD_STOP
};

// Additional: List of all chatter blocks
// Note: Strictly this only needs to be the blocks that are referred to from
// other blocks.
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

// $A13E
const session_t saved_game_state = {
  1, // spawn_accumulator
  100, // idle_timer
  USERINPUTFLAGMASK_ALLOW_ALL,
  3, // turbos
  0x54, // horizon_level
  20, // perp_halt_counter
  0xFF, // displayed_gear
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, // score digits
  15, // sixteenths
  0x60, // time_bcd
  { 0xFF, 0xFF }, // time_digits
  { 0xFF, 0xFF, 0xFF, 0xFF }, // distance_digits
  1, // no_objects_flag
  0x59BF // horizon_attribute
};

const hazard_t saved_game_state_hazard_0 = {
  0,
  100, // distance
  0,
  0,
  0,
  0x47,
  0,
  0,
  0x2C,
  0x0000,
  perp_behaviour,
  0x003C,
  0xFF,
  0,
  3,
  2,
  0
};

/* ----------------------------------------------------------------------- */

// $A27A
const u8 font[41 * 7] = {
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

// $A623
const hazard_t hazard_template = {
  HAZARD_USED,
  21, // distance
  0,
  0,
  0,
  0,
  0,
  0,
  { 0x28, NULL },
  hazard_handler,
  0, // speed
  0x80, // hazard_flags
  0,
  0,
  0,
  0
};

/* ----------------------------------------------------------------------- */

// $A7E7
const u8 hazard_pos_speed[3 * 4] = {
  5, 80, 148, 216,    // road positions the hazard will align with
  140, 220, 170, 250, // hazard speed used when perp has not been sighted
  60, 140, 90, 170    // hazard speed used when perp is sighted
};

/* ----------------------------------------------------------------------- */

// $B045
const u8 hero_car_jump_table[10 * 2] = {
  0x03, 0x0D, // Pitch Up,    Delta  13
  0x03, 0x0A, // Pitch Up,    Delta  10
  0x03, 0x07, // Pitch Up,    Delta   7
  0x03, 0x04, // Pitch Up,    Delta   4
  0x00, 0x02, // Pitch Level, Delta   2
  0x00, 0xFE, // Pitch Level, Delta  -2
  0x06, 0xFC, // Pitch Down,  Delta  -4
  0x06, 0xF9, // Pitch Down,  Delta  -7
  0x06, 0xF6, // Pitch Down,  Delta -10
  0x06, 0xF3  // Pitch Down,  Delta -13
};

// $B059
//
// Five pairs of (jump_arc_offset, y_height)
const u8 car_jump_params[5 * 2] = {
  0x08, 0x02,
  0x06, 0x04,
  0x04, 0x06,
  0x02, 0x08,
  0x00, 0x0A
};

// $B828
//
// Byte table: scroll_horizon ($B265) reads HL=($B827+curvature) as a byte;
// update_road_level ($B9BD) reads HL=($B828+speed_part+abs_curvature*4) as byte.
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

// $CE0C
const u8 smoke_ce0c_template[13] = {
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// $CE19
const u8 smoke_ce19_template[13] = {
  0x02, 0xF4, 0x10, 0xF8, 0x08, 0xFA, 0x08, 0xFC, 0x08, 0xFD, 0x08, 0xFE, 0x00
};

// $CE26
const u8 smoke_ce26_template[13] = {
  0x05, 0xF8, 0x20, 0xFA, 0x18, 0xFC, 0x10, 0xFE, 0x10, 0xFF, 0x08, 0x00, 0x08
};

/* ----------------------------------------------------------------------- */

// $CE4B
const u8 debris_subtable_1_template[19] = {
  0x00, // counter
  0x32, 0x10, // y,x
  0x28, 0x20, 0x23, 0x30, 0x1E, 0x40, 0x23, 0x50, 0x28, 0x60, 0x32, 0x68, 0x46, 0x70, 0x5A, 0x78
};

// $CE5E
const u8 debris_subtable_2_template[19] = {
  0x01,
  0x1E, 0x48, // y,x
  0x14, 0x50, 0x0A, 0x58, 0x0A, 0x60, 0x14, 0x68, 0x1E, 0x70, 0x2D, 0x70, 0x41, 0x78, 0x5A, 0x78
};

// $CE71
const u8 debris_subtable_3_template[19] = {
  0x03,
  0x0A, 0x80, // y,x
  0x05, 0x80, 0x05, 0x80, 0x0A, 0x80, 0x0F, 0x80, 0x19, 0x80, 0x28, 0x80, 0x3C, 0x80, 0x5A, 0x80
};

// $CE84
const u8 debris_subtable_4_template[19] = {
  0x01,
  0x1E, 0xB0, // y,x
  0x14, 0xA8, 0x0A, 0xA0, 0x0A, 0x98, 0x14, 0x90, 0x1E, 0x88, 0x2D, 0x88, 0x41, 0x80, 0x5A, 0x80
};

// $CE97
const u8 debris_subtable_5_template[19] = {
  0x02,
  0x32, 0xF0, // y,x
  0x28, 0xE8, 0x23, 0xE0, 0x1E, 0xD0, 0x23, 0xC0, 0x28, 0xB0, 0x32, 0xA0, 0x46, 0x90, 0x5A, 0x80
};

/* ----------------------------------------------------------------------- */

// $CEAA
const u8 bitmap_debris_1[2 * 6] = {
  XXXX___X, ____XXX_,
  _______X, XXXX__X_,
  ________, X______X,
  ________, X_____XX,
  X______X, _X__XXX_,
  XX__XXXX, __XX____
};

// $CEB6
const u8 bitmap_debris_2[2 * 6] = {
  XXX___XX, ___XXX__,
  X______X, _XXX_XX_,
  X______X, _X____X_,
  X______X, _X____X_,
  XX_____X, __X_XXX_,
  XX__XXXX, __XX____
};

// $CEC2
const u8 bitmap_debris_3[2 * 6] = {
  XXX__XXX, ___XX___,
  XX___XXX, __X_X___,
  XX____XX, __X__X__,
  XX____XX, __X__X__,
  XXX___XX, ___X_X__,
  XXXX_XXX, ____X___
};

// $CECE
const u8 bitmap_debris_4[2 * 6] = {
  XXX___XX, ___XXX__,
  X______X, _XXX_XX_,
  X______X, _X____X_,
  X______X, _X____X_,
  XX_____X, __X_XXX_,
  XX__XXXX, __XX____
};

/* ----------------------------------------------------------------------- */

// $CEDA
const carpart_t hero_car_parts[9][5] = {
  {
    {  6, 14, bitmap_hero_centre_straight },
    { 20,  9, bitmap_hero_top_straight    },
    {  0,  6, bitmap_hero_bottom_straight },
    {  6, 14, bitmap_hero_left_straight   },
    {  6, 14, bitmap_hero_right_straight  }
  },
  {
    {  4, 17, bitmap_hero_centre_straight_right },
    { 21,  8, bitmap_hero_top_straight_right    },
    {  0,  4, bitmap_hero_bottom_straight_right },
    {  7, 13, bitmap_hero_left_straight_left    },
    {  7, 13, bitmap_hero_right_straight_right  }
  },
  {
    {  4, 16, bitmap_hero_centre_straight_right_hard },
    { 20,  9, bitmap_hero_top_straight_right_hard    },
    {  0,  4, bitmap_hero_bottom_straight_right_hard },
    {  7, 12, bitmap_hero_left_straight_right_hard   },
    {  5, 15, bitmap_hero_right_straight_right_hard  }
  },
  {
    {  6, 14, bitmap_hero_centre_up },
    { 20, 10, bitmap_hero_top_up    },
    {  0,  6, bitmap_hero_bottom_up },
    {  6, 13, bitmap_hero_left_up   },
    {  6, 14, bitmap_hero_right_up  }
  },
  {
    {  4, 17, bitmap_hero_centre_up_right },
    { 21,  9, bitmap_hero_top_right       },
    {  0,  4, bitmap_hero_bottom_right    },
    {  7, 12, bitmap_hero_left_right      },
    {  6, 14, bitmap_hero_right_right     }
  },
  {
    {  6, 15, bitmap_hero_centre_up_right_hard },
    { 21,  9, bitmap_hero_up_right_hard        },
    {  0,  6, bitmap_hero_bottom_right_hard    },
    {  7, 11, bitmap_hero_left_right_hard      },
    {  6, 15, bitmap_hero_right_right_hard     }
  },
  {
    {  6, 14, bitmap_hero_centre_down },
    { 20,  8, bitmap_hero_top_down    },
    {  0,  6, bitmap_hero_bottom_down },
    {  6, 13, bitmap_hero_left_down   },
    {  6, 14, bitmap_hero_right_down  }
  },
  {
    {  4, 16, bitmap_hero_centre_down_right },
    { 20,  8, bitmap_hero_top_down_right    },
    {  0,  4, bitmap_hero_bottom_down_right },
    {  7, 13, bitmap_hero_left_down_right   },
    {  6, 13, bitmap_hero_right_down_right  }
  },
  {
    {  4, 16, bitmap_hero_centre_down_right_hard },
    { 20,  8, bitmap_hero_top_down_right_hard    },
    {  0,  4, bitmap_hero_bottom_down_right_hard },
    {  7, 13, bitmap_hero_left_down_right_hard   },
    {  5, 15, bitmap_hero_right_down_right_hard  }
  }
};

// $CF8E
const carpart_t hero_car_shadow[3] = {
  { 0, 12, bitmap_shadow_straight        },
  { 0, 12, bitmap_shadow_turn_right      },
  { 0, 12, bitmap_shadow_turn_right_hard }
};

// $CF9A
const carsmokeframe_t hero_car_turbo_smoke[4] = {
  {
    16, 4,
    16, 216,
    &bitmap_turbo_1[0]
  },
  {
    16, 4,
    16, 216,
    &bitmap_turbo_2[0]
  },
  {
    16, 4,
    16, 216,
    &bitmap_turbo_3[0]
  },
  {
    16, 4,
    16, 216,
    &bitmap_turbo_4[0]
  }
};

/* ----------------------------------------------------------------------- */

// $CFB2
const carframe_t car_frames[39] = {
  { 0xE1, 0x00, 0x00 }, // Cherry light
  { 0xE4, 0xF8, 0x04 }, // Flashing cherry light
  { 0xE1, 0x00, 0x00 }, // Cherry light
  { 0xE4, 0xF8, 0x04 }, // Flashing cherry light
  { 0xE1, 0xF8, 0x00 }, // Cherry light
  { 0xE4, 0xF0, 0x04 }, // Flashing cherry light
  { 0xE8, 0x00, 0x0C }, // Putting-cherry-on-roof anim frame 1
  { 0xE3, 0x08, 0x00 }, // Cherry light
  { 0xE8, 0x00, 0x10 }, // Putting-cherry-on-roof anim frame 2
  { 0xE0, 0x08, 0x00 }, // Cherry light
  { 0xE8, 0x08, 0x14 }, // Putting-cherry-on-roof anim frame 3
  { 0xE8, 0x00, 0x10 }, // Putting-cherry-on-roof anim frame 2
  { 0xE8, 0x00, 0x0C }, // Putting-cherry-on-roof anim frame 1
  { 0xE8, 0x00, 0x0C }, // Putting-cherry-on-roof anim frame 1
  { 0xE3, 0x08, 0x00 }, // Cherry light
  { 0xE8, 0x00, 0x10 }, // Putting-cherry-on-roof anim frame 2
  { 0xE0, 0x08, 0x00 }, // Cherry light
  { 0xE8, 0x08, 0x14 }, // Putting-cherry-on-roof anim frame 3
  { 0xE8, 0x00, 0x10 }, // Putting-cherry-on-roof anim frame 2
  { 0xE8, 0x00, 0x0C }, // Putting-cherry-on-roof anim frame 1
  { 0xE8, 0xF8, 0x0C }, // Putting-cherry-on-roof anim frame 1
  { 0xE3, 0x00, 0x00 }, // Cherry light
  { 0xE8, 0xF8, 0x10 }, // Putting-cherry-on-roof anim frame 2
  { 0xE0, 0x00, 0x00 }, // Cherry light
  { 0xE8, 0x00, 0x14 }, // Putting-cherry-on-roof anim frame 3
  { 0xE8, 0xF8, 0x10 }, // Putting-cherry-on-roof anim frame 2
  { 0xE8, 0xF8, 0x0C }, // Putting-cherry-on-roof anim frame 1
  { 0xEC, 0x00, 0x08 }, // Crash/spark
  { 0xF0, 0x08, 0x08 }, // Crash/spark
  { 0xEE, 0xF8, 0x08 }, // Crash/spark
  { 0xEC, 0xE0, 0x08 }, // Crash/spark
  { 0xEE, 0xE8, 0x08 }, // Crash/spark
  { 0xF0, 0xD8, 0x08 }, // Crash/spark
  { 0xEC, 0xF0, 0x08 }, // Crash/spark
  { 0xEE, 0xF8, 0x08 }, // Crash/spark
  { 0xEE, 0xE8, 0x08 }, // Crash/spark
  { 0xE8, 0xF0, 0x10 }, // Putting-cherry-on-roof anim frame 2
  { 0xE8, 0xF8, 0x10 }, // Putting-cherry-on-roof anim frame 2
  { 0xE8, 0xF0, 0x10 }  // Putting-cherry-on-roof anim frame 2
};

// $CFB2
const caradornment_t car_adornments[6] = {
  {  7, 1, &bitmap_cherry_light[0] },
  { 14, 3, &bitmap_cherry_light_lit[0] },
  { 20, 3, &bitmap_spark[0] },
  {  4, 2, &bitmap_cherryout_1[0] },
  {  9, 2, &bitmap_cherryout_2[0] },
  { 12, 1, &bitmap_cherryout_3[0] }
};

/* ----------------------------------------------------------------------- */

// $D03F
const u8 bitmap_arrow[3 * 2 * 28] = {
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

// $D0E7
const u8 bitmap_cherry_light[1 * 2 * 7] = {
  ________, _X____X_,
  ________, _XXXXXX_,
  ________, _XX_X_X_,
  ________, _X_X_XX_,
  ________, _XX_X_X_,
  X______X, __XXXX__,
  XX____XX, ________
};

// $D0F5
const u8 bitmap_cherry_light_lit[3 * 2 * 14] = {
  XXXXXXXX, ________, XXXX_XXX, ____X___, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX___XX, ___X_X__, XXXXXXXX, ________,
  XXXXXXXX, ________, XX____XX, __XX_X__, XXXXXXXX, ________,
  XXXXXXXX, ________, X______X, _X_X__X_, XXXXXXXX, ________,
  XXXXXXXX, ________, X______X, _XXX_XX_, XXXXXXXX, ________,
  XXXXX___, _____XXX, ________, XXX____X, ____XXXX, XXXX____,
  XXXX____, ____X_X_, ________, ________, _____XXX, __X_X___,
  XXXXX___, _____XXX, ________, XXX___XX, ____XXXX, XXXX____,
  XXXXXXXX, ________, XX_____X, __XX_XX_, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX___XX, ___X_X__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX___XX, ___X_X__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX___XX, ___XXX__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX___XX, ___X_X__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXX_XXX, ____X___, XXXXXXXX, ________
};

// $D149
const u8 bitmap_spark[3 * 2 * 20] = {
  XXXXXXX_, _______X, XXX___XX, ___X_X__, XX_XXXXX, __X_____,
  XXXXXX__, ______X_, __X___X_, XX_XXX_X, X___XXXX, _X_X____,
  XXXXX_X_, _____X_X, ________, X_X_X_X_, _X_XXXXX, X_X_____,
  XXXX___X, ____X_X_, ________, XX_X_X_X, _X_XX_XX, X_X__X__,
  XXX_X___, ___X_XXX, ________, _X__X_X_, _______X, _X_XX_X_,
  _X______, X_X_XX_X, ________, ________, ___X__XX, X_X_XX__,
  ____X___, _XXX_XX_, ________, _____X__, ______XX, ___X_X__,
  ________, XX_XX_X_, ________, _______X, ___X_X_X, _XX_X_X_,
  XX_X____, __X_XX__, ________, ________, ____X___, ___X_X_X,
  X_X_____, _X_X_X__, ________, X_______, _____X_X, _XX_X_X_,
  _____X__, X_XXX_X_, ________, _____X__, ____XX_X, __XX__X_,
  X_______, _XX_XX__, ________, _X______, ____X___, X__X_X_X,
  XXX_XX__, ___X__X_, ________, X__X___X, _____X_X, _XX_X_X_,
  _XXX_X__, X___X_X_, ________, _XX__X__, __X_X_XX, XX_X_X__,
  __X_____, _X_X_X_X, ________, XX__XX_X, _______X, X_XXX_X_,
  _XXX_X__, X___X_XX, ________, _XX_XXX_, ______XX, XXX_XX__,
  XXX_X___, ___X_X_X, ________, XX_XX_XX, ____XXXX, X_XX____,
  XX___X_X, __X_X_X_, _X__X___, X_XX_XX_, ___XXXXX, XXX_____,
  XXX_XXX_, ___X___X, _____XX_, _XX_X__X, X___XXXX, _X_X____,
  XXXXXXXX, ________, _XX_XXXX, X__X____, XX_XXXXX, __X_____
};

// $D1C1
const u8 bitmap_cherryout_1[2 * 2 * 4] = {
  XXXXXX__, ______XX, ___XXXXX, XXX_____,
  XXXXXXX_, _______X, X___XXXX, _XXX____,
  XXXXXXXX, ________, ____XXXX, XXXX____,
  XXXXXXXX, ________, ___XXXXX, XXX_____
};

// $D1D1
const u8 bitmap_cherryout_2[2 * 2 * 9] = {
  XXXXXXX_, _______X, ___X_XXX, XXX_____,
  XXXXXX_X, ______X_, ______XX, XXXXX___,
  XXXXXXX_, _______X, ______XX, XXXXX___,
  XXXXXXXX, ________, XX_____X, ___XXX__,
  XXXXXXXX, ________, XXX____X, ____XX__,
  XXXXXXXX, ________, XXX_____, ____X_X_,
  XXXXXXXX, ________, XXXX____, ______X_,
  XXXXXXXX, ________, XXXX___X, _____X__,
  XXXXXXXX, ________, XXXXX_XX, ________
};

// $D1F5
const u8 bitmap_cherryout_3[1 * 2 * 12] = {
  _____XXX, XXXXX___,
  ______XX, XXXXX___,
  XX_____X, __XXXX__,
  XX_____X, __XXXX__,
  XX______, ___XXXX_,
  XX______, ___XXXX_,
  XXX____X, ____XX__,
  XX_____X, ____XX__,
  X______X, __XXXX__,
  X_____XX, ___XX___,
  _____XXX, _XXX____,
  X___XXXX, ________
};

// $D20D
const u8 bitmap_turbo_1[4 * 2 * 16] = {
  XX___XXX, __XXX___, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________,
  X_______, _X__XXXX, ___XXXXX, XXX_____, XXXXXXXX, ________, XXXXXXXX, ________,
  ________, X______X, ______XX, _XXXXX__, ____XX__, XXXX__XX, _XXXXXXX, X_______,
  ________, X___X___, ________, __XX_XXX, ________, _XXXXXXX, ___XXXXX, XXX_____,
  X_______, _XXX____, ________, _X__XX__, ________, ____XXX_, ____XXXX, _XXX____,
  XXXX____, ____X_X_, ________, ____X___, ________, ______XX, _____XXX, __X_X___,
  XXXXX___, _____X__, ________, ________, ________, _______X, _____XXX, ____X___,
  XXXXXX__, ______X_, ________, __X_____, ________, _______X, _____XXX, ____X___,
  XXXXXX__, ______X_, ________, _X______, ________, ________, ____XXXX, __XX____,
  XXXXXXX_, _______X, ________, X___X___, ________, ________, ___XXXXX, __X_____,
  XXXXXXXX, ________, X_______, _XXX____, ________, ________, ____XXXX, ___X____,
  XXXXXXXX, ________, XXX_____, ___X____, ________, _______X, ____XXXX, ___X____,
  XXXXXXXX, ________, XXXX____, ____X__X, ________, X____XX_, ___XXXXX, X_X_____,
  XXXXXXXX, ________, XXXXX__X, _____XX_, ________, X__XX___, __XXXXXX, XX______,
  XXXXXXXX, ________, XXXXXXXX, ________, X_______, _XX___XX, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, XXX___XX, ___XXX__, XXXXXXXX, ________
};

// $D28D
const u8 bitmap_turbo_2[4 * 2 * 16] = {
  XX___XXX, __XXX___, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________,
  X_______, _X___XXX, X__XXXXX, _XX_____, XXXX____, ____XXXX, XXXXXXXX, ________,
  ________, X_______, ______XX, X__XXX__, ________, XXXX___X, ____XXXX, XXXX____,
  ________, X____X__, ________, __XX_XXX, ________, XX______, _____XXX, XXXXX___,
  X_______, _XXXX___, ________, __X___XX, ________, ________, ______XX, _X_X_X__,
  XXXXX___, _____XX_, ________, _______X, ________, ________, ______XX, _X___X__,
  XXXXXX__, ______X_, ________, _X_____X, ________, ________, ______XX, _____X__,
  XXXXXXX_, _______X, ________, X_______, ________, X_______, ______XX, ____XX__,
  XXXXXXXX, ________, ________, XX______, ________, ________, _____XXX, __XXX___,
  XXXXXXXX, ________, XX______, __X_____, ________, ________, ____XXXX, ___X____,
  XXXXXXXX, ________, X_______, _X______, ________, ________, ____XXXX, ___X____,
  XXXXXXXX, ________, X_______, _X___X__, ________, _______X, _____XXX, ____X___,
  XXXXXXXX, ________, XX______, __XXX___, _______X, _____XX_, _____XXX, X___X___,
  XXXXXXXX, ________, XXXXX___, _____XX_, ________, X__X___X, ____XXXX, X__X____,
  XXXXXXXX, ________, XXXXXXX_, _______X, X__X___X, _XX_XXX_, ___XXXXX, XXX_____,
  XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________
};

// $D30D
const u8 bitmap_turbo_3[4 * 2 * 16] = {
  XX___XXX, __XXX___, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________,
  X______X, _X___XX_, _X__XXXX, X_XX____, XXXXXXXX, ________, XXXXXXXX, ________,
  ________, X______X, ______XX, XXXXXX__, ___X___X, XXX_XXX_, ___XXXXX, XXX_____,
  ________, X__X____, ________, ___XXXXX, ________, _XXX___X, _____XXX, XXXXX___,
  X__X____, _XX_X_X_, ________, _____X__, ________, __X_____, ______XX, __XX_X__,
  XXXXX___, _____XX_, ________, ____X___, ________, __X_____, _______X, ___X__X_,
  XXXXXX__, ______X_, ________, _X__X___, ________, ________, _______X, ______X_,
  XXXXXXX_, _______X, ________, X_______, ________, ________, ______XX, _____X__,
  XXXXXXXX, ________, X_______, _X______, ________, ________, _______X, _____XX_,
  XXXXXXXX, ________, X_______, _X______, ________, ________, _______X, ______X_,
  XXXXXXXX, ________, XX______, __XX____, ________, ________, _______X, ______X_,
  XXXXXXXX, ________, XXX_____, ___X___X, ________, X__XX__X, ______XX, ___XXX__,
  XXXXXXXX, ________, XXXX____, ____XXXX, ________, _____XX_, ______XX, X_XXXX__,
  XXXXXXXX, ________, XXXXXXXX, ________, ________, XX___X_X, _______X, XX____X_,
  XXXXXXXX, ________, XXXXXXXX, ________, XX___X_X, __XXX_X_, XX_____X, __XX__X_,
  XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXX___XX, ___XXX__
};

// $D38D
const u8 bitmap_turbo_4[4 * 2 * 16] = {
  XX___XXX, __XXX___, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________,
  X_______, _X__XXXX, ___XXXXX, XXX_____, XXXXXXXX, ________, XXXXXXXX, ________,
  ________, X______X, ______XX, _XXXXX__, ____XX__, XXXX__XX, __XXXXXX, XX______,
  ________, X_______, ________, _X___XXX, ________, __XXXXXX, ____XXXX, XXXX____,
  X_______, _XXX_X__, ________, ____XX__, ________, XX_____X, _____XXX, XXX_X___,
  XXXX____, ____X___, ________, ____X___, ________, X_______, ______XX, X____X__,
  XXXXX___, _____XXX, ________, X_______, ________, ________, ______XX, X____X__,
  XXXXXXX_, _______X, ________, ________, ________, ________, ______XX, _____X__,
  XXXXXXXX, ________, ________, X__X____, ________, ________, _____XXX, __XXX___,
  XXXXXXXX, ________, ________, XXX_____, ________, ________, ___XXXXX, __X_____,
  XXXXXXXX, ________, X_______, _XX_X___, ________, ____X___, ____XXXX, ___X____,
  XXXXXXXX, ________, XXX_____, ___XX___, ________, _X_XXX_X, ____XXXX, ___X____,
  XXXXXXXX, ________, XXXXX___, _____X__, ________, X___XXXX, ___XX__X, X_X__XX_,
  XXXXXXXX, ________, XXXXXX__, ______XX, X______X, _XXX__X_, __XX____, XX__X__X,
  XXXXXXXX, ________, XXXXXXXX, ________, XXX___XX, ___XXX__, XXXXX___, _____X_X,
  XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXX_X, ______X_
};

// $D40D
const u8 bitmap_hero_centre_straight[5 * 14] = {
  XXXXXXXX, XXXXXXX_, ________, __XXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXX_, ________, __XXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXX_, ________, __XXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ___XXXXX, XXXXXXXX, XXXXXX__, ________,
  _X_X_XXX, XX_XXXXX, XXXXXXXX, XXXXXX_X, XXXX_X_X,
  X_X_X___, __XXXXXX, XXX_X_XX, XXXXXXX_, ____X_X_,
  _X_XX___, __XXXXXX, XXX___XX, XXXXXXX_, ____XX_X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXXX,
  XXXXX__X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX__XXXX,
  XXXXX___, ________, ________, ________, ____XXXX,
  __XX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_XX_
};

// $D453
const u8 bitmap_hero_centre_straight_right[5 * 17] = {
  XXXXXXX_, XX__XXXX, XXXXXXXX, XX_XX__X, XXXXXXXX,
  XXXXXXXX, ___XX___, ________, XXX___XX, XXXXXXXX,
  XXXXXXXX, XXXXX___, ________, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXX___, ________, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXX___, ________, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  ________, _XXXXXXX, XXXXXXXX, XXXX____, ______XX,
  _X_XXXXX, _XXXXXXX, XXXXXXXX, XXXX_XXX, XX_X_X_X,
  X_X_____, XXXXXXXX, X_X_XXXX, XXXXX___, __X_X_XX,
  _XX_____, XXXXXXXX, X___XXXX, XXXXX___, __XX_X_X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXXXX,
  XXX___XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXXXX,
  XXX_____, ________, ________, ________, __XXXXXX,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  ___XXXXX, _XXXXXXX, XX__X_X_, XXXXXXXX, ___XX___
};

// $D4A8
const u8 bitmap_hero_centre_straight_right_hard[5 * 16] = {
  XXXX_XX_, _XXXXXXX, XXXXXXX_, XX___XXX, XXXXXXXX,
  XXXXX___, XXX_____, ______XX, ____XXXX, XXXXXXXX,
  XXXXXXXX, XXX_____, ______XX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXX_____, ______XX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXX_____, ______XX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  _______X, XXXXXXXX, XXXXXXXX, XX______, ____XXXX,
  _XXXXX_X, XXXXXXXX, XXXXXXXX, XX_XXXXX, _X_X_XXX,
  X_____XX, XXXXXXX_, X_XXXXXX, XXX_____, X_X_XXXX,
  X_____XX, XXXXXXX_, __XXXXXX, XXX_____, XX_X_XXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X__XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, XXXXXXXX,
  X____XXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, _XXXXXXX,
  X_______, ________, ________, ________, _XXXXXXX,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX__XXX
};

// $D4F8
const u8 bitmap_hero_centre_up[5 * 14] = {
  XXXXXXXX, XXXXXXX_, ________, __XXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ___XXXXX, XXXXXXXX, XXXXXX__, ________,
  _X_X_XXX, XX_XXXXX, XXXXXXXX, XXXXXX_X, XXXX_X_X,
  X_X_X___, __XXXXXX, XXX_X_XX, XXXXXXX_, ____X_X_,
  _X_XX___, __XXXXXX, XXX___XX, XXXXXXX_, ____XX_X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXXX,
  XXXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXXX,
  XXXXX___, ________, ________, ________, ____XXXX,
  XXXXXX__, ________, ________, ________, ___XXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_
};

// $D53E
const u8 bitmap_hero_centre_up_right[5 * 17] = {
  XXXXXXXX, X__XX___, ________, XXXX__XX, XXXXXXXX,
  XXXXXXXX, XXXXX___, ________, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXX___, ________, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  ________, _XXXXXXX, XXXXXXXX, XXXX____, ______XX,
  _X_XXXXX, _XXXXXXX, XXXXXXXX, XXXX_XXX, XX_X_X_X,
  X_X_____, XXXXXXXX, X_X_XXXX, XXXXX___, __X_X_XX,
  _XX_____, XXXXXXXX, X___XXXX, XXXXX___, __XX_X_X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXXXX,
  XXX__XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXXXX,
  XXX_____, ________, ________, ________, __XXXXXX,
  XXXX____, ________, ________, ________, __XXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XXX__
};

// $D593
const u8 bitmap_hero_centre_up_right_hard[5 * 15] = {
  XXXXXXXX, XXX_____, ______XX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  _______X, XXXXXXXX, XXXXXXXX, XX______, ____XXXX,
  _XXXXX_X, XXXXXXXX, XXXXXXXX, XX_XXXXX, _X_X_XXX,
  X_____XX, XXXXXXX_, X_XXXXXX, XXX_____, X_X_XXXX,
  X_____XX, XXXXXXX_, __XXXXXX, XXX_____, XX_X_XXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, XXXXXXXX,
  X___XXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, XXXXXXXX,
  X_______, ________, ________, ________, _XXXXXXX,
  XXX_____, ________, ________, ________, _XXXXXXX,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX__XXXX,
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX__XXX_
};

// $D5DE
const u8 bitmap_hero_centre_down[5 * 14] = {
  XXXXXXXX, X____XXX, XXXXXXXX, XXXX____, XXXXXXXX,
  XXXXXXXX, X_XX_XX_, ________, __XX_XX_, XXXXXXXX,
  XXXXXXXX, XX__XXX_, ________, __XXX__X, XXXXXXXX,
  XXXXXXXX, XXXXXXX_, ________, __XXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXX_, ________, __XXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ___XXXXX, XXXXXXXX, XXXXXX__, ________,
  _X_X_XXX, XX_XXXXX, XXXXXXXX, XXXXXX_X, XXXX_X_X,
  X_X_X___, __XXXXXX, XXX_X_XX, XXXXXXX_, ____X_X_,
  _X_XX___, __XXXXXX, XXX___XX, XXXXXXX_, ____XX_X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XXXXX,
  __XXX__X, XXXXXXXX, XXXXXXXX, XXXXXXXX, X__XXXX_
};

// $D624
const u8 bitmap_hero_centre_down_right[5 * 16] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, X__XXXXX, XXXXXXXX, XXXX__XX, XXXXXXXX,
  XXXXXXXX, ____XXXX, XXXXXXXX, XXX____X, XXXXXXXX,
  XXXXXXX_, XX_XX___, ________, XX_XX_XX, XXXXXXXX,
  XXXXXXXX, __XXX___, ________, XXX__XXX, XXXXXXXX,
  XXXXXXXX, XXXXX___, ________, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXX___, ________, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  ________, _XXXXXXX, XXXXXXXX, XXXX____, ______XX,
  _X_XXXXX, _XXXXXXX, XXXXXXXX, XXXX_XXX, XX_X_X_X,
  X_X_____, XXXXXXXX, X_X_XXXX, XXXXX___, __X_X_XX,
  _XX_____, XXXXXXXX, X___XXXX, XXXXX___, __XX_X_X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXXXX,
  _XX___XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXX__
};

// $D674
const u8 bitmap_hero_centre_down_right_hard[5 * 16] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXX_, _XXXXXXX, XXXXXXXX, XX__XXXX, XXXXXXXX,
  XXXXX___, __XXXXXX, XXXXXXXX, _____XXX, XXXXXXXX,
  XXXX_XX_, _XX_____, ______X_, XX__XXXX, XXXXXXXX,
  XXXXX___, XXX_____, ______XX, ___XXXXX, XXXXXXXX,
  XXXXXXXX, XXX_____, ______XX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXX_____, ______XX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  _______X, XXXXXXXX, XXXXXXXX, XX______, ____XXXX,
  _XXXXX_X, XXXXXXXX, XXXXXXXX, XX_XXXXX, _X_X_XXX,
  X_____XX, XXXXXXX_, X_XXXXXX, XXX_____, X_X_XXXX,
  X_____XX, XXXXXXX_, __XXXXXX, XXX_____, XX_X_XXX,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X__XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, _XXXXXXX,
  X____XXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, _XXXXXXX
};

// $D6C4
const u8 bitmap_hero_top_straight[5 * 2 * 9] = {
  XX__XX__, ___X__XX, XX______, __XXXXXX, __X_X_X_, XX_X_X_X, _______X, XXXXXXX_, X__XX__X, _XX__X__,
  XXX__X__, ____X_XX, XXX_____, ___XXXXX, XXXXXXXX, ________, X_____XX, _XXXXX__, X__X__XX, _XX_X___,
  XXX__XX_, ____X__X, _XX_____, X__XXXXX, _XXXXXXX, X_______, ______XX, XXXXXX__, __XX__XX, XX__X___,
  XXXX__X_, _____X_X, _XX_____, X__XXXXX, _X_____X, X_XXXXX_, ______XX, XXXXXX__, __X__XXX, XX_X____,
  XXXXX__X, ______X_, __X_____, XX_XXXXX, _X_____X, X_XXXXX_, ______X_, XXXXXX_X, _X__XXXX, X_X_____,
  XXXXXX__, _______X, ___X____, XXX_XXXX, XXXX_XXX, ____X___, X____X__, _XXXX_XX, ___XXXXX, XX______,
  XXXXXXX_, ________, ________, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, __XXXXXX, ________,
  XXXXXXXX, ________, X_______, ___XXXXX, ________, XXXXXXXX, ________, XXXXXX__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ________, ________, ________, ______XX, ________, XXXXXXXX, ________
};

// $D71E
const u8 bitmap_hero_bottom_straight[5 * 2 * 6] = {
  XXX_____, ___XXXXX, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, X_____XX, _XXXXX__,
  XX______, __X_____, _XXXXXXX, X_______, XXXXXXXX, ________, XXXXXXXX, ________, _______X, X_____X_,
  X_______, _X_X_X_X, ____XXXX, _XXX____, XXXXXXXX, ________, XXXXX___, _____XXX, ________, _X_X_X_X,
  X_______, _XXXXXXX, _____XXX, XX__X___, XXXXXXXX, ________, XXXX____, ____X__X, ________, XXXXXXXX,
  X_______, _XXXXXXX, ________, X_XX_XXX, ________, XXXXXXXX, ________, XXXX_XX_, ________, XXXXXXXX,
  X_______, _XXXXXXX, ________, XX__XXX_, ________, ________, ________, __XXX__X, ________, XXXXXXXX
};

// $D75A
const u8 bitmap_hero_left_straight[1 * 2 * 14] = {
  XXXXXXX_, ________,
  XXXXXXX_, ________,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXXXX_, ________,
  XXXXXXXX, ________
};

// $D776
const u8 bitmap_hero_right_straight[1 * 2 * 14] = {
  __XXXXXX, X_______,
  __XXXXXX, X_______,
  ___XXXXX, XX______,
  ___XXXXX, XX______,
  ____XXXX, XXX_____,
  ____XXXX, XXX_____,
  ____XXXX, _XX_____,
  ____XXXX, XXX_____,
  ____XXXX, _XX_____,
  ____XXXX, XXX_____,
  ___XXXXX, XX______,
  ___XXXXX, XX______,
  __XXXXXX, X_______,
  _XXXXXXX, ________
};

// $D792
const u8 bitmap_hero_top_straight_right[5 * 2 * 8] = {
  XXX___XX, ____XX__, XXX_____, ___XXXXX, _XXXXXXX, X_______, XX____XX, __XXXX__, XXX__XXX, ___X____,
  XXXX__XX, _____X__, XXXX____, ____XXXX, _XXXXXXX, X_______, X______X, _XXXXXX_, XX___XXX, __XX____,
  XXXXX__X, ______X_, XXXX____, ____XXXX, __XX____, XX__XXXX, _______X, XXXXXXX_, XX__XXXX, __X_____,
  XXXXXX__, _______X, XXXX____, ____XXXX, __XX____, XX__XXXX, _______X, XXXXXXX_, X__XXXXX, _X______,
  XXXXXXX_, ________, _XXXX___, X____XXX, _XXXXX_X, X_____X_, XX____XX, __XXXX__, __XXXXXX, X_______,
  XXXXXXXX, ________, ________, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _XXXXXXX, ________,
  XXXXXXXX, ________, X_______, ___XXXXX, ________, XXXXXXXX, ________, XXXXXX__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ________, ________, ________, ______XX, ________, XXXXXXXX, ________
};

// $D7E2
const u8 bitmap_hero_bottom_straight_right[5 * 2 * 4] = {
  XX_____X, __XXXXX_, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, _____XXX, XXXXX___,
  X_______, _X_____X, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, _______X, ______XX, _____X__,
  ________, X_X_X_XX, ___XXXXX, XXX_____, XXXXXXXX, ________, XXX_____, ___XXXX_, _______X, X_X_XXX_,
  ________, XXXXXXXX, ____XXXX, ___X____, XXXXXXXX, ________, XX______, __X___XX, _______X, XXXXXXX_
};

// $D80A
const u8 bitmap_hero_left_straight_left[1 * 2 * 13] = {
  XXXXXXX_, ________,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXX___, ______XX,
  XXXXX___, ______X_,
  XXXXX___, _______X,
  XXXXX___, ______X_,
  XXXXX___, _______X,
  XXXXX___, ______XX,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXXXX_, ________,
  XXXXXXXX, ________
};

// $D824
const u8 bitmap_hero_right_straight_right[1 * 2 * 13] = {
  __XXXXXX, X_______,
  ___XXXXX, XX______,
  ____XXXX, XXX_____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  ____XXXX, XXX_____,
  ____XXXX, XXX_____,
  ___XXXXX, XX______,
  __XXXXXX, ________
};

// $D83E
const u8 bitmap_hero_top_straight_right_hard[5 * 2 * 9] = {
  X_______, ___XXXXX, ________, XXXXXXXX, ______X_, XXXXXX_X, X_X_____, _X_XXXXX, ___XX___, XXX__XX_,
  XXX____X, _____XX_, X_XXXX__, _X____XX, ____XXXX, XXXX____, XXXXX___, _____XXX, __XXX__X, XX___X__,
  XXXXX__X, ______X_, X_XXXXX_, _X_____X, ____XXXX, XXXX____, XXXXX___, _____XXX, __XX__XX, XX__X___,
  XXXXXX__, _______X, XX_XXXX_, __X____X, _____XX_, XXXXX__X, ________, XXXXXXXX, __X__XXX, XX_X____,
  XXXXXXX_, ________, _X_XXXX_, X_X____X, _____XX_, XXXXX__X, ________, XXXXXXXX, _X__XXXX, X_X_____,
  XXXXXXXX, ________, ____XXXX, _XXX____, ____XXXX, XXXX____, X_XXX___, _X___XXX, X__XXXXX, _X______,
  XXXXXXXX, ________, X_______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, __XXXXXX, X_______,
  XXXXXXXX, ________, XX______, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, _XXXXXXX, ________,
  XXXXXXXX, ________, XXXX____, ________, ________, ________, ________, ________, XXXXXXXX, ________
};

// $D898
const u8 bitmap_hero_bottom_straight_right_hard[5 * 2 * 4] = {
  XX____XX, __XXXX__, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, _______X, ____XXXX, XXXX____,
  X______X, _X____X_, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXX__, ______X_, _____XXX, ____X___,
  ________, X_X_XXXX, XXXXXXXX, ________, XXXXXXXX, ________, ____X___, XXXX_X_X, ______XX, _X_XXX__,
  ________, XXXXX___, _XXXXXXX, X_______, XXXXXXX_, _______X, ________, ____XXXX, ______XX, XXXXXX__
};

// $D8C0
const u8 bitmap_hero_left_straight_right_hard[1 * 2 * 12] = {
  XXXXXXX_, ________,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXX___, ______XX,
  XXXXX___, ________,
  XXXXX___, _______X,
  XXXXX___, ______X_,
  XXXXX___, _______X,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXXX__, _______X,
  XXXXXXX_, ________
};

// $D8D8
const u8 bitmap_hero_right_straight_right_hard[1 * 2 * 15] = {
  _XXXXXXX, X_______,
  ___XXXXX, XX______,
  ____XXXX, XXX_____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  _____XXX, XXXX____,
  ____XXXX, XXX_____,
  ___XXXXX, ________
};

// $D8F6
const u8 bitmap_hero_top_up[5 * 2 * 10] = {
  XX___XX_, ___XX__X, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, __XX___X, XX__XX__,
  XXX___X_, _____X_X, _X______, X_XXXXXX, __X_X_X_, XX_X_X_X, _______X, XXXXXXX_, __X___XX, XX_X____,
  XXXX__X_, _____X_X, _X______, X_XXXXXX, XXXXXXXX, ________, X_____X_, _XXXXX_X, _XX__XXX, X__X____,
  XXXXX___, ______XX, _XX_____, X__XXXXX, XXXXXXXX, ________, X_____XX, _XXXXX__, _X__XXXX, X_X_____,
  XXXXX___, ______XX, _XX_____, X__XXXXX, _X_____X, X_XXXXX_, ______XX, XXXXXX__, ____XXXX, XXX_____,
  XXXXXX__, _______X, __X_____, XX_XXXXX, _X_____X, X_XXXXX_, ______XX, XXXXXX__, ___XXXXX, XX______,
  XXXXXXX_, ________, ________, XXXXXXXX, ________, XXXXXXXX, ______X_, XXXXXX_X, __XXXXXX, X_______,
  XXXXXXXX, ________, ________, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _XXXXXXX, ________,
  XXXXXXXX, ________, X_______, ___XXXXX, ________, XXXXXXXX, ________, XXXXXX__, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ________, ________, ________, ______XX, ________, XXXXXXXX, ________
};

// $D95A
const u8 bitmap_hero_bottom_up[5 * 2 * 6] = {
  XXX_____, ___XXXXX, _X__XXXX, X_XX____, XXXXXXXX, ________, XXXXX__X, _____XX_, ______XX, XXXXXX__,
  XX______, __X_____, ______XX, XX__XX__, XXXXXXXX, ________, XXXX____, ____X__X, _______X, X_____X_,
  X_______, _X_X_X_X, ________, X_XX_XXX, ________, XXXXXXXX, ________, XXXX_XX_, ________, XX_X_X_X,
  X_______, _XXXXXXX, ________, X____XX_, ________, ________, ________, __XX____, ________, XXXXXXXX,
  X_______, _XXXXXXX, ________, XX__XXX_, ________, ________, ________, __XXX__X, ________, XXXXXXXX,
  X_______, _XXXXXXX, ________, XXXXXXX_, ________, ________, ________, __XXXXXX, ________, XXXXXXXX
};

// $D996
const u8 bitmap_hero_left_up[1 * 2 * 13] = {
  XXXXXXX_, ________,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXXXX_, ________
};

// $D9B0
const u8 bitmap_hero_right_up[1 * 2 * 14] = {
  __XXXXXX, X_______,
  ___XXXXX, XX______,
  ___XXXXX, XX______,
  ____XXXX, XXX_____,
  ____XXXX, _XX_____,
  ____XXXX, XXX_____,
  ____XXXX, _XX_____,
  ____XXXX, XXX_____,
  ____XXXX, XXX_____,
  ____XXXX, XXX_____,
  ___XXXXX, XX______,
  ___XXXXX, XX______,
  __XXXXXX, X_______,
  _XXXXXXX, ________
};

// $D9CC
const u8 bitmap_hero_top_right[5 * 2 * 9] = {
  XXX_____, _____XXX, ________, XXXXXXXX, ____X_X_, XXXX_X_X, X_______, _XXXXXXX, XXX___XX, ___X____,
  XXXX__XX, _____X__, XXX_____, ___XXXXX, _XXXXXXX, X_______, XX____XX, __XXXX__, XX__XXXX, __X_____,
  XXXXX__X, ______X_, XXXX____, ____XXXX, _XXXXXXX, X_______, X______X, _XXXXXX_, XX__XXXX, __X_____,
  XXXXXX__, _______X, XXXX____, ____XXXX, __XX____, XX__XXXX, _______X, XXXXXXX_, X__XXXXX, _X______,
  XXXXXXX_, ________, _XXX____, X___XXXX, __XX____, XX__XXXX, ________, XXXXXXXX, __XXXXXX, X_______,
  XXXXXXXX, ________, ________, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _XXXXXXX, ________,
  XXXXXXXX, ________, X_______, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXX_, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ______XX, ________, XXXXXXXX, _______X, XXXXX___, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXX__, ________, ________, ________, _____XXX, ________, XXXXXXXX, ________
};

// $DA26
const u8 bitmap_hero_bottom_right[5 * 2 * 4] = {
  XX______, __XXXXXX, ___XXXXX, XXX_____, XXXXXXXX, ________, XXX_____, ___XXXXX, _____XXX, XXXXX___,
  X_______, _X_____X, ____XXXX, ___X____, XXXXXXXX, ________, XX______, __X___XX, ______XX, _____X__,
  ________, X_X_X_X_, ________, XX__XXXX, ________, XXXXXXXX, ________, XX_XX__X, _______X, X_X_XXX_,
  ________, XXXXXXXX, ________, ____X___, ________, ________, ________, XXX____X, _______X, XXXXXXX_
};

// $DA4E
const u8 bitmap_hero_left_right[1 * 2 * 12] = {
  XXXXXXX_, ________,
  XXXXXX__, _______X,
  XXXXXX__, ________,
  XXXXX___, _______X,
  XXXXX___, ______X_,
  XXXXX___, _______X,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXXXX_, ________
};

// $DA66
const u8 bitmap_hero_right_right[1 * 2 * 14] = {
  __XXXXXX, X_______,
  ___XXXXX, XX______,
  ____XXXX, XXX_____,
  ____XXXX, XXX_____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  ____XXXX, XXX_____,
  ____XXXX, XXX_____,
  ___XXXXX, XX______,
  __XXXXXX, ________
};

// $DA82
const u8 bitmap_hero_up_right_hard[5 * 2 * 9] = {
  XXXX____, _____XXX, ________, XXXXXXXX, ______X_, XXXXXX_X, X_X_____, _X_XXXXX, _XXX___X, X___X___,
  XXXXX__X, ______X_, X_XXXX__, _X____XX, ____XXXX, XXXX____, XXXXX___, _____XXX, _XXX__XX, X___X___,
  XXXXXX__, _______X, X_XXXXX_, _X_____X, ____XXXX, XXXX____, XXXXX___, _____XXX, _XX__XXX, X__X____,
  XXXXXXX_, ________, _X_XXXX_, X_X____X, _____XX_, XXXXX__X, ________, XXXXXXXX, XX__XXXX, __X_____,
  XXXXXXXX, ________, ___XXXX_, _XX____X, _____XX_, XXXXX__X, ________, XXXXXXXX, X__XXXXX, _X______,
  XXXXXXXX, ________, X_______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XX______,
  XXXXXXXX, ________, XX______, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, __XXXXXX, ________,
  XXXXXXXX, ________, XXXX____, ________, ________, XXXXXXXX, ________, XXXXX___, XXXXXXXX, ________,
  XXXXXXXX, ________, XXXXXXXX, ________, ________, ________, _____XXX, ________, XXXXXXXX, ________
};

// $DADC
const u8 bitmap_hero_bottom_right_hard[5 * 2 * 6] = {
  XX__X___, __XX_XXX, XXXXXXXX, ________, XXXXXXXX, ________, ___XXXX_, XXX____X, ____XXXX, XXXX____,
  X_______, _X__X___, __XXXXXX, XX______, XXXXXXX_, _______X, ________, ___XXXX_, _____XXX, ____X___,
  ________, X_XX_XX_, ________, __XXXXXX, ________, XXXXXXX_, ________, XX___X_X, ______XX, _X_XXX__,
  ________, XXXXX___, ________, __X_____, ________, ______XX, ________, _____XXX, ______XX, XXXXXX__,
  ________, XXXXXXX_, ________, _XX_____, ________, ______XX, ________, XX__XXXX, _______X, XXXXXXX_,
  ________, XXXXXXXX, ________, XXX_____, ________, ______XX, ________, XXXXXXXX, ________, XXXXXXXX
};

// $DB18
const u8 bitmap_hero_left_right_hard[1 * 2 * 11] = {
  XXXXXXX_, ________,
  XXXXXX__, _______X,
  XXXXXX__, ________,
  XXXXX___, _______X,
  XXXXX___, ______X_,
  XXXXX___, _______X,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXXX__, _______X,
  XXXXXXX_, ________
};

// $DB2E
const u8 bitmap_hero_right_right_hard[1 * 2 * 15] = {
  ___XXXXX, XX______,
  ____XXXX, XXX_____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  _____XXX, XXXX____,
  ____XXXX, XX______,
  __XXXXXX, ________
};

// $DB4C
const u8 bitmap_hero_top_down[5 * 2 * 8] = {
  XX______, ___XXX__, ________, ________, ________, ________, ________, ________, _______X, __XXXX__,
  XXX_____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXX___,
  XXXX__X_, _____X_X, _XX_____, X__XXXXX, _X_____X, X_XXXXX_, _____XXX, XXXXX___, __X__XXX, XX_X____,
  XXXXX__X, ______X_, __XX____, XX__XXXX, XXXX_XXX, ____X___, X___XXX_, _XXX___X, _X__XXXX, X_X_____,
  XXXXXX__, _______X, ___X____, XXX_XXXX, ________, XXXXXXXX, _____X__, XXXXX_XX, ___XXXXX, XX______,
  XXXXXXX_, ________, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, __XXXXXX, X_______,
  XXXXXXXX, ________, ________, ___XXXXX, ________, XXXXXXXX, ________, XXXXXX__, _XXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ________, ________, ________, ______XX, ________, XXXXXXXX, ________
};

// $DB9C
const u8 bitmap_hero_bottom_down[5 * 2 * 6] = {
  XXX_____, ___XXXXX, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, X_____XX, _XXXXX__,
  XX______, __X_____, _XXXXXXX, X_______, XXXXXXXX, ________, XXXXXXXX, ________, _______X, X_____X_,
  X_______, _X_X_X_X, __XXXXXX, _X______, XXXXXXXX, ________, XXXXXXX_, _______X, ________, _X_X_X_X,
  X_______, _XXXXXXX, ____XXXX, XXXX____, XXXXXXXX, ________, XXXXX___, _____XXX, ________, XXXXXXXX,
  X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX,
  X_______, _XXXXXXX, ________, XX__XXXX, ________, XXXXXXXX, ________, XXXXX__X, ________, XXXXXXXX
};

// $DBD8
const u8 bitmap_hero_left_down[1 * 2 * 13] = {
  XXXXXXX_, ________,
  XXXXXXX_, ________,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXXXX_, ________
};

// $DBF2
const u8 bitmap_hero_right_down[1 * 2 * 14] = {
  __XXXXXX, X_______,
  __XXXXXX, X_______,
  ___XXXXX, XX______,
  ___XXXXX, XX______,
  ____XXXX, XXX_____,
  ____XXXX, XXX_____,
  ____XXXX, XXX_____,
  ____XXXX, XXX_____,
  ____XXXX, _XX_____,
  ____XXXX, XXX_____,
  ___XXXXX, _X______,
  ___XXXXX, XX______,
  __XXXXXX, X_______,
  _XXXXXXX, ________
};

// $DC0E
const u8 bitmap_hero_top_down_right[5 * 2 * 8] = {
  X_______, ___X____, ________, ________, ________, ________, ________, ________, ______XX, __XXX___,
  XXX_____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXX_____,
  XXXX___X, ______X_, XXXX____, ____XXXX, __XX____, XX__XXXX, ________, XXXXXXXX, XX__XXXX, __X_____,
  XXXXXX__, _______X, XXXX____, ____XXXX, __XX____, XX__XXXX, _______X, XXXXXXX_, X__XXXXX, _X______,
  XXXXXXX_, ________, _XXXX___, X____XXX, _XXXXX_X, X_____X_, XX_____X, __XXXXX_, __XXXXXX, X_______,
  XXXXXXXX, ________, ________, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _XXXXXXX, ________,
  XXXXXXXX, ________, X_______, ___XXXXX, ________, XXXXXXXX, ________, XXXXX___, XXXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ________, ________, ________, _____XXX, ________, XXXXXXXX, ________
};

// $DC5E
const u8 bitmap_hero_bottom_down_right[5 * 2 * 4] = {
  XX_____X, __XXXXX_, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, _____XXX, XXXXX___,
  X_______, _X_____X, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, _______X, ______XX, _____X__,
  ________, X_X_X_XX, _XXXXXXX, X_______, XXXXXXXX, ________, XXXXXX__, ______X_, _______X, X_X_XXX_,
  ________, XXXXXXXX, ___XXXXX, XXX_____, XXXXXXXX, ________, XXXX____, ____XXXX, _______X, XXXXXXX_
};

// $DC86
const u8 bitmap_hero_left_down_right[1 * 2 * 13] = {
  XXXXXXX_, ________,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______X_,
  XXXXX___, _______X,
  XXXXX___, ______X_,
  XXXXX___, _______X,
  XXXXX___, ______XX,
  XXXXXX__, _______X,
  XXXXXXX_, ________
};

// $DCA0
const u8 bitmap_hero_right_down_right[1 * 2 * 13] = {
  __XXXXXX, ________,
  ___XXXXX, XX______,
  ____XXXX, XXX_____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  _____XXX, XXXX____,
  ____XXXX, XXX_____,
  ____XXXX, XXX_____,
  ___XXXXX, XX______,
  __XXXXXX, ________
};

// $DCBA
const u8 bitmap_hero_top_down_right_hard[5 * 2 * 8] = {
  ________, X_______, ________, ________, ________, ________, ________, ________, ___XX___, XXX__XX_,
  ________, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XX__X, XXX__X__,
  X______X, ______X_, X_XXXXX_, _X_____X, _____XX_, XXXXX__X, ________, XXXXXXXX, _XXX__XX, X___X___,
  XXXXXX__, _______X, XX_XXXX_, __X____X, _____XX_, XXXXX__X, ________, XXXXXXXX, _XX__XXX, X__X____,
  XXXXXXX_, ________, _X_XXXXX, X_X_____, _____XXX, XXXXX___, X_XX____, _X__XXXX, X___XXXX, _XX_____,
  XXXXXXXX, ________, ________, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, X_______,
  XXXXXXXX, ________, X_______, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _XXXXXXX, ________,
  XXXXXXXX, ________, XXX_____, ________, ________, ________, ________, ________, XXXXXXXX, ________
};

// $DD0A
const u8 bitmap_hero_bottom_down_right_hard[5 * 2 * 4] = {
  XX____XX, __XXXX__, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXX_, _______X, ____XXXX, XXXX____,
  X______X, _X____X_, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXX__, ______X_, _____XXX, ____X___,
  _______X, X_X_XXX_, XXXXXXXX, ________, XXXXXXXX, ________, XXXXX___, _____X_X, ______XX, _X_XXX__,
  ________, XXXXXXXX, ___XXXXX, XXX_____, XXXXXXXX, ________, XX______, __XXXXXX, ______XX, XXXXXX__
};

// $DD32
const u8 bitmap_hero_left_down_right_hard[1 * 2 * 13] = {
  XXXXXXX_, ________,
  XXXXXX__, _______X,
  XXXXXX__, _______X,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ______XX,
  XXXXX___, ________,
  XXXXX___, _______X,
  XXXXX___, ______X_,
  XXXXX___, _______X,
  XXXXX___, ______XX,
  XXXXXX__, _______X,
  XXXXXXX_, ________
};

// $DD4C
const u8 bitmap_hero_right_down_right_hard[1 * 2 * 15] = {
  __XXXXXX, XX______,
  ____XXXX, XXX_____,
  _____XXX, XXXX____,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  ______XX, XXXXX___,
  _____XXX, XXXX____,
  ____XXXX, XXX_____,
  ___XXXXX, X_______,
  _XXXXXXX, ________
};

// $DD6A
const u8 bitmap_shadow_straight[7 * 2 * 12] = {
  XXXXXX__, ______XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXXX___, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XXXX____,
  XXXXX___, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XXXX____,
  XXXXXX__, ______XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXXXXX__, ______XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXXXXXX_, _______X, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, __XXXXXX, XX______,
  XXXXXXXX, ________, X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, XXXXXXXX, ________
};

// $DE12
const u8 bitmap_shadow_turn_right[7 * 2 * 12] = {
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XXXX____,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXXX___, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXXXX__, ______XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXXXXX_, _______X, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXXXXXX, ________, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXXXXXX, ________, X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXXXXXX, ________, XX______, __XXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XXXX____,
  XXXXXXXX, ________, XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____
};

// $DEBA
const u8 bitmap_shadow_turn_right_hard[7 * 2 * 12] = {
  XXXXXX__, ______XX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, XXXXXXXX, ________,
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, __XXXXXX, XX______,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ____XXXX, XXXX____,
  XXXX____, ____XXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXXX___, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _____XXX, XXXXX___,
  XXXXXXX_, _______X, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  XXXXXXXX, ________, X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__,
  XXXXXXXX, ________, XXX_____, ___XXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  XXXXXXXX, ________, XXXXX___, _____XXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  XXXXXXXX, ________, XXXXXXX_, _______X, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, _______X, XXXXXXX_,
  XXXXXXXX, ________, XXXXXXXX, ________, X_______, _XXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX, ______XX, XXXXXX__
};

/* ----------------------------------------------------------------------- */

// $DF62
const u8 ledfont[10 * LEDFONT_HEIGHT] = {
  _XXXXX__,
  X_XXX_X_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  X_____X_,
  X_____X_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  X_XXX_X_,
  _XXXXX__,

  ________,
  ______X_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  ______X_,
  ______X_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  ______X_,
  ________,

  _XXXXX__,
  __XXX_X_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  __XXX_X_,
  X_XXX___,
  XX______,
  XX______,
  XX______,
  XX______,
  XX______,
  X_XXX___,
  _XXXXX__,

  _XXXXX__,
  __XXX_X_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  __XXX_X_,
  __XXX_X_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  __XXX_X_,
  _XXXXX__,

  ________,
  X_____X_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  X_XXX_X_,
  __XXX_X_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  ______X_,
  ________,

  _XXXXX__,
  X_XXX___,
  XX______,
  XX______,
  XX______,
  XX______,
  X_XXX___,
  __XXX_X_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  __XXX_X_,
  _XXXXX__,

  _XXXXX__,
  X_XXX___,
  XX______,
  XX______,
  XX______,
  XX______,
  X_XXX___,
  X_XXX_X_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  X_XXX_X_,
  _XXXXX__,

  _XXXXX__,
  __XXX_X_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  ______X_,
  ______X_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  ______X_,
  ________,

  _XXXXX__,
  X_XXX_X_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  X_XXX_X_,
  X_XXX_X_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  X_XXX_X_,
  _XXXXX__,

  _XXXXX__,
  X_XXX_X_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  XX___XX_,
  X_XXX_X_,
  __XXX_X_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  _____XX_,
  __XXX_X_,
  _XXXXX__,
};

// $DFF8
const u8 minifont[31 * MINIFONT_HEIGHT] = {
  _XX_____,
  X__X____,
  X__X____,
  XXXX____,
  X__X____,
  X__X____,

  XXX_____,
  X__X____,
  XXX_____,
  X__X____,
  X__X____,
  XXX_____,

  _XX_____,
  X__X____,
  X_______,
  X_______,
  X__X____,
  _XX_____,

  XXX_____,
  X__X____,
  X__X____,
  X__X____,
  X__X____,
  XXX_____,

  XXXX____,
  X_______,
  XXX_____,
  X_______,
  X_______,
  XXXX____,

  XXXX____,
  X_______,
  XXX_____,
  X_______,
  X_______,
  X_______,

  _XX_____,
  X__X____,
  X_______,
  X_XX____,
  X__X____,
  _XXX____,

  X__X____,
  X__X____,
  XXXX____,
  X__X____,
  X__X____,
  X__X____,

  _XXX____,
  __X_____,
  __X_____,
  __X_____,
  __X_____,
  _XXX____,

  ___X____,
  ___X____,
  ___X____,
  ___X____,
  X__X____,
  _XX_____,

  X__X____,
  X__X____,
  X_X_____,
  XXX_____,
  X__X____,
  X__X____,

  X_______,
  X_______,
  X_______,
  X_______,
  X_______,
  XXXX____,

  X__X____,
  XXXX____,
  X__X____,
  X__X____,
  X__X____,
  X__X____,

  X__X____,
  XX_X____,
  XX_X____,
  X_XX____,
  X_XX____,
  X__X____,

  _XX_____,
  X__X____,
  X__X____,
  X__X____,
  X__X____,
  _XX_____,

  XXX_____,
  X__X____,
  X__X____,
  XXX_____,
  X_______,
  X_______,

  _XX_____,
  X__X____,
  X__X____,
  XX_X____,
  X_XX____,
  _XX_____,

  XXX_____,
  X__X____,
  X__X____,
  XXX_____,
  X__X____,
  X__X____,

  _XX_____,
  X_______,
  _XX_____,
  ___X____,
  X__X____,
  _XX_____,

  _XXX____,
  __X_____,
  __X_____,
  __X_____,
  __X_____,
  __X_____,

  X__X____,
  X__X____,
  X__X____,
  X__X____,
  X__X____,
  _XX_____,

  X__X____,
  X__X____,
  X__X____,
  X__X____,
  _XX_____,
  _XX_____,

  X__X____,
  X__X____,
  X__X____,
  XXXX____,
  XXXX____,
  X__X____,

  X__X____,
  X__X____,
  _XX_____,
  _XX_____,
  X__X____,
  X__X____,

  _X_X____,
  _X_X____,
  _X_X____,
  __X_____,
  __X_____,
  __X_____,

  XXXX____,
  ___X____,
  __X_____,
  _X______,
  X_______,
  XXXX____,

  ________,
  ________,
  ________,
  ________,
  _XX_____,
  _XX_____,

  ________,
  ________,
  ________,
  __X_____,
  __X_____,
  _X______,

  __X_____,
  __X_____,
  __X_____,
  __X_____,
  ________,
  __X_____,

  ________,
  ________,
  ________,
  ________,
  ________,
  ________,

  __X_____,
  __X_____,
  _X______,
  ________,
  ________,
  ________
};

/* ----------------------------------------------------------------------- */

// $E0B2
const bitmap_t fire1_defns[SPRITE_FRAMES] = {
  { 4, BITMAPFLAG_DEFAULT, 16, bitmap_fire1,      bitmap_fire1       },
  { 3, BITMAPFLAG_MASKED,   8, bitmap_fire3,      bitmap_fire3       },
  { 2, BITMAPFLAG_MASKED,   5, bitmap_fire5,      bitmap_fire5s      },
  { 2, BITMAPFLAG_MASKED,   4, bitmap_fire5 + 4,  bitmap_fire5s + 4  },
  { 2, BITMAPFLAG_MASKED,   3, bitmap_fire5 + 8,  bitmap_fire5s + 8  },
  { 2, BITMAPFLAG_MASKED,   2, bitmap_fire5 + 12, bitmap_fire5s + 12 }
};

// $E0DC
const bitmap_t fire2_defns[SPRITE_FRAMES] = {
  { 4, BITMAPFLAG_DEFAULT, 16, bitmap_fire2,      bitmap_fire2       },
  { 3, BITMAPFLAG_MASKED,   8, bitmap_fire4,      bitmap_fire4       },
  { 2, BITMAPFLAG_MASKED,   6, bitmap_fire6,      bitmap_fire6s      },
  { 2, BITMAPFLAG_MASKED,   5, bitmap_fire6 + 4,  bitmap_fire6s + 4  },
  { 2, BITMAPFLAG_MASKED,   4, bitmap_fire6 + 8,  bitmap_fire6s + 8  },
  { 2, BITMAPFLAG_MASKED,   3, bitmap_fire6 + 12, bitmap_fire6s + 12 }
};

// $E106
const bitmap_t fire3_defns[SPRITE_FRAMES] = {
  { 4, BITMAPFLAG_DEFAULT, 11, bitmap_fire1 + 20, bitmap_fire1 + 20  },
  { 3, BITMAPFLAG_MASKED,   6, bitmap_fire3 + 12, bitmap_fire3 + 12  },
  { 2, BITMAPFLAG_MASKED,   4, bitmap_fire5 + 4,  bitmap_fire5s + 4  },
  { 2, BITMAPFLAG_MASKED,   3, bitmap_fire5 + 8,  bitmap_fire5s + 8  },
  { 2, BITMAPFLAG_MASKED,   2, bitmap_fire5 + 12, bitmap_fire5s + 12 },
  { 2, BITMAPFLAG_MASKED,   1, bitmap_fire5 + 16, bitmap_fire5s + 12 }
};

// $E130
const bitmap_t fire4_defns[SPRITE_FRAMES] = {
  { 4, BITMAPFLAG_DEFAULT, 11, bitmap_fire2 + 20, bitmap_fire2 + 20  },
  { 3, BITMAPFLAG_MASKED,   6, bitmap_fire4 + 12, bitmap_fire4 + 12  },
  { 2, BITMAPFLAG_MASKED,   5, bitmap_fire6 + 4,  bitmap_fire6s + 4  },
  { 2, BITMAPFLAG_MASKED,   4, bitmap_fire6 + 8,  bitmap_fire6s + 8  },
  { 2, BITMAPFLAG_MASKED,   3, bitmap_fire6 + 12, bitmap_fire6s + 12 },
  { 2, BITMAPFLAG_MASKED,   2, bitmap_fire6 + 16, bitmap_fire6s + 16 }
};

// $E15A
const bitmap_t fire5_defns[SPRITE_FRAMES] = {
  { 4, BITMAPFLAG_DEFAULT,  6, bitmap_fire1 + 40, bitmap_fire1 + 40  },
  { 3, BITMAPFLAG_MASKED,   3, bitmap_fire3 + 40, bitmap_fire3 + 40  },
  { 2, BITMAPFLAG_MASKED,   3, bitmap_fire5 + 8,  bitmap_fire5s + 8  },
  { 2, BITMAPFLAG_MASKED,   2, bitmap_fire5 + 12, bitmap_fire5s + 12 },
  { 2, BITMAPFLAG_MASKED,   1, bitmap_fire5 + 16, bitmap_fire5s + 16 },
  { 2, BITMAPFLAG_MASKED,   1, bitmap_fire5 + 16, bitmap_fire5s + 16 }
};

// $E184
const bitmap_t fire6_defns[SPRITE_FRAMES] = {
  { 4, BITMAPFLAG_DEFAULT, 6, bitmap_fire2 + 40, bitmap_fire2 + 40  },
  { 3, BITMAPFLAG_MASKED,  3, bitmap_fire4 + 40, bitmap_fire4 + 40  },
  { 2, BITMAPFLAG_MASKED,  3, bitmap_fire6 + 12, bitmap_fire6s + 12 },
  { 2, BITMAPFLAG_MASKED,  2, bitmap_fire6 + 16, bitmap_fire6s + 16 },
  { 2, BITMAPFLAG_MASKED,  1, bitmap_fire6 + 20, bitmap_fire6s + 20 },
  { 2, BITMAPFLAG_MASKED,  1, bitmap_fire6 + 20, bitmap_fire6s + 20 }
};

// $E1AE
const bitmap_t smoke_defns[SPRITE_FRAMES] = {
  { 2, BITMAPFLAG_MASKED, 13, bitmap_smoke1, bitmap_smoke1 },
  { 2, BITMAPFLAG_MASKED, 11, bitmap_smoke2, bitmap_smoke2 },
  { 2, BITMAPFLAG_MASKED,  9, bitmap_smoke3, bitmap_smoke3 },
  { 1, BITMAPFLAG_MASKED,  7, bitmap_smoke4, bitmap_smoke4 },
  { 1, BITMAPFLAG_MASKED,  5, bitmap_smoke5, bitmap_smoke5 },
  { 1, BITMAPFLAG_MASKED,  3, bitmap_smoke6, bitmap_smoke6 }
};

// $E1D8
const bitmap_t floating_arrow_here_defn = {
  3, BITMAPFLAG_MASKED, 28, bitmap_arrow, bitmap_arrow
};

// Conv: Made these bitmap_t's and used NULL for the pre-shifted bitmap field.

// $E1DF
const bitmap_t floating_arrow_left_defn = {
  3, BITMAPFLAG_MASKED, 21, bitmap_arrow, NULL
};

// $E1E4
const bitmap_t floating_arrow_right_defn = {
  3, BITMAPFLAG_MASKED | BITMAPFLAG_FLIPPED, 21, bitmap_arrow, NULL
};

/* ----------------------------------------------------------------------- */

// $E1E9
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

// $E1FF
const bitmap_t tunnellight_bitmaps[SPRITE_FRAMES] = {
  { 2, BITMAPFLAG_DEFAULT, 16, &bitmap_tunnellight_1[0], &bitmap_tunnellight_1[0]  },
  { 2, BITMAPFLAG_DEFAULT, 12, &bitmap_tunnellight_2[0], &bitmap_tunnellight_2[0]  },
  { 1, BITMAPFLAG_DEFAULT,  8, &bitmap_tunnellight_3[0], &bitmap_tunnellight_3[0]  },
  { 1, BITMAPFLAG_DEFAULT,  6, &bitmap_tunnellight_4[0], &bitmap_tunnellight_4[0]  },
  { 1, BITMAPFLAG_DEFAULT,  5, &bitmap_tunnellight_5[0], &bitmap_tunnellight_5[0]  },
  { 2, BITMAPFLAG_MASKED,   6, &bitmap_tunnellight_6[0], &bitmap_tunnellight_6s[0] }
};

// $E229
const u8 bitmap_tunnellight_1[2 * 16] = {
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

// $E249
const u8 bitmap_tunnellight_2[2 * 12] = {
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

// $E261
const u8 bitmap_tunnellight_3[1 * 8] = {
  XXX_XXXX,
  X_X_X_XX,
  XX_____X,
  ______XX,
  XX______,
  X_____XX,
  XX_X_X_X,
  X_XX_XXX,
};

// $E269
const u8 bitmap_tunnellight_4[1 * 6] = {
  XXXXXXXX,
  _X_X_XXX,
  X___XXXX,
  _____XXX,
  X___XXXX,
  _X_X_XXX,
};

// $E26F
const u8 bitmap_tunnellight_5[1 * 5] = {
  XX_XXXXX,
  X___XXXX,
  _____XXX,
  X___XXXX,
  XX_XXXXX,
};

// $E274
const u8 bitmap_tunnellight_6[2 * 2 * 6] = {
  XX__XXXX, __XX____, XXXXXXXX, ________,
  X____XXX, _X__X___, XXXXXXXX, ________,
  ______XX, X____X__, XXXXXXXX, ________,
  ______XX, X____X__, XXXXXXXX, ________,
  X____XXX, _X__X___, XXXXXXXX, ________,
  XX__XXXX, __XX____, XXXXXXXX, ________,
};

// $E28C
const u8 bitmap_tunnellight_6s[2 * 2 * 6] = {
  XXXXXX__, ______XX, XXXXXXXX, ________,
  XXXXX___, _____X__, _XXXXXXX, X_______,
  XXXX____, ____X___, __XXXXXX, _X______,
  XXXX____, ____X___, __XXXXXX, _X______,
  XXXXX___, _____X__, _XXXXXXX, X_______,
  XXXXXX__, ______XX, XXXXXXXX, ________,
};

/* ----------------------------------------------------------------------- */

// $E2A4
const u8 perp_escape_hazards[6] = {
  MAP_HAZARD_WAIT(27),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(1),
  MAP_CMD_STOP_BARRIERS
};

// $E2AA
const u8 perp_escape_curvature[5] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CMD_GOTO(0xE2AA) // loop
};

// $E2AF
const u8 perp_escape_height[5] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_CMD_GOTO(0xE2AF) // loop
};

// $E2B4
const u8 perp_escape_lanes[4] = {
  MAP_LANES_3L(53),
  MAP_LANES_TUNNEL_ENTRY(255) // no explicit stop
};

// $E2B8
const u8 fork_hazards[8] = {
  MAP_HAZARD_WAIT(12),
  MAP_CMD_ARROW_OFF,
  MAP_HAZARD_WAIT(255),
  MAP_CMD_GOTO(0xE2B8) // loop
};

// $E2C0
const u8 fork_leftrightobjs[6] = {
  MAP_OBJ_S1_NONE(1),
  MAP_OBJ_S1_SHORT_POLE(15),
  MAP_CMD_GOTO(0xE2C1) // loop (skipping)
};

// $E2C6
const u8 forked_road_curvature[6] = {
  MAP_CURVE_STRAIGHT(1),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CMD_GOTO(0xE2C7) // loop (skipping)
};

// $E2CC
const u8 forked_road_height[5] = {
  MAP_CURVE_LEFT_HARD(15),
  MAP_CMD_GOTO(0xE2CC) // loop
};

// $E2D1
const u8 forked_road_lanes[2] = {
  MAP_LANES_FORKED(255)
};

// $ED23
const u8 forked_road_exit_hazards[3] = {
  MAP_HAZARD_WAIT(18),
  MAP_CMD_FORK_END
};

// $ED26
const u8 forked_road_exit_rightobjs[4] = {
  MAP_OBJ_S1_SHORT_POLE(5),
  MAP_OBJ_S1_NONE(13),
  MAP_CMD_FORK_END
};

// $E2DA
const u8 forked_road_exit_leftobjs[4] = {
  MAP_OBJ_S1_NONE(5),
  MAP_OBJ_S1_NONE(13),
  MAP_CMD_FORK_END
};

// $E2DE
const u8 forked_road_exit_curvature[5] = {
  MAP_CURVE_STRAIGHT(15), // 15 is max
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(6),
  MAP_CMD_FORK_END
};

// $E2E3
const u8 forked_road_exit_height[5] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(6),
  MAP_CMD_FORK_END
};

// $E2E8
const u8 forked_road_exit_left_lanes[12] = {
  MAP_LANES_2L(10),
  MAP_LANES_2TO3L(2),
  MAP_LANES_3L(10),
  MAP_LANES_3TO4L(2),
  MAP_LANES_4(12),
  MAP_CMD_FORK_END
};

// $E2F4
const u8 forked_road_exit_right_lanes[12] = {
  MAP_LANES_2R(10),
  MAP_LANES_2TO3R(2),
  MAP_LANES_3R(10),
  MAP_LANES_3TO4R(2),
  MAP_LANES_4(12),
  MAP_CMD_FORK_END
};

/* ----------------------------------------------------------------------- */

// $E364
const u8 spiral_transition_mask[13 * 8] = {
  ________,
  ________,
  ________,
  ________,
  ________,
  ________,
  ________,
  ________,

  XXXXXXXX,
  ________,
  ________,
  ________,
  ________,
  ________,
  ________,
  ________,

  XXXXXXXX,
  _______X,
  _______X,
  _______X,
  _______X,
  _______X,
  _______X,
  _______X,

  XXXXXXXX,
  _______X,
  _______X,
  _______X,
  _______X,
  _______X,
  _______X,
  XXXXXXXX,

  XXXXXXXX,
  X______X,
  X______X,
  X______X,
  X______X,
  X______X,
  X______X,
  XXXXXXXX,

  XXXXXXXX,
  XXXXXXXX,
  X______X,
  X______X,
  X______X,
  X______X,
  X______X,
  XXXXXXXX,

  XXXXXXXX,
  XXXXXXXX,
  X_____XX,
  X_____XX,
  X_____XX,
  X_____XX,
  X_____XX,
  XXXXXXXX,

  XXXXXXXX,
  XXXXXXXX,
  X_____XX,
  X_____XX,
  X_____XX,
  X_____XX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXXXXX,
  XXXXXXXX,
  XX____XX,
  XX____XX,
  XX____XX,
  XX____XX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XX____XX,
  XX____XX,
  XX____XX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XX___XXX,
  XX___XXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX
};

// $E3BC
const u8 circle_transition_mask[7 * 8] = {
  ________,
  ________,
  ________,
  ___XX___,
  ___XX___,
  ________,
  ________,
  ________,

  ________,
  ________,
  ___XX___,
  __XXXX__,
  __XXXX__,
  ___XX___,
  ________,
  ________,

  ________,
  ___XX___,
  __XXXX__,
  _XXXXXX_,
  _XXXXXX_,
  __XXXX__,
  ___XX___,
  ________,

  ___XX___,
  __XXXX__,
  _XXXXXX_,
  XXXXXXXX,
  XXXXXXXX,
  _XXXXXX_,
  __XXXX__,
  ___XX___,

  __XXXX__,
  _XXXXXX_,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  _XXXXXX_,
  __XXXX__,

  _XXXXXX_,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  _XXXXXX_,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX
};

/* ----------------------------------------------------------------------- */

// $E400 - Road edge/lane markings ($E4xx page, 256 bytes)
// $E400..$E40F: unused zeros (Z80 "draw nothing" state when stripe offset = 0)
// $E410..$E4CF: edge markings (six 32-byte masked variants)
// $E4D0..$E4FF: lane markings (three 16-byte unmasked variants)
const u8 edge_markings[256] = {
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

// $E500
const u16 outward_bend_table[32] = {
  0x0000,
  0xEC22,
  0xF653,
  0xF9B9,
  0xFB6C,
  0xFC72,
  0xFD21,
  0xFD9E,
  0xFDFD,
  0xFE46,
  0xFE81,
  0xFEB1,
  0xFEDA,
  0xFEFD,
  0xFF1A,
  0xFF34,
  0xFF4B,
  0xFF5F,
  0xFF71,
  0xFF82,
  0xFF91,
  0xFF9E,
  0xFFAA,
  0xFFB6,
  0xFFC0,
  0xFFCA,
  0xFFD3,
  0xFFDC,
  0xFFE4,
  0xFFEC,
  0xFFF3,
  0xFFFA
};

// $E540 - Converts a curvature to a road X position
// Approx? v[i] = round(128 * (1 + tan((i − 32) · π/128)))
const u16 curvature_to_xpos[96] = {
  0x0000,
  0x0006,
  0x000C,
  0x0012,
  0x0017,
  0x001C,
  0x0021,
  0x0026,
  0x002A,
  0x002F,
  0x0033,
  0x0037,
  0x003C,
  0x0040,
  0x0043,
  0x0047,
  0x004B,
  0x004F,
  0x0052,
  0x0056,
  0x0059,
  0x005D,
  0x0060,
  0x0063,
  0x0067,
  0x006A,
  0x006D,
  0x0070,
  0x0073,
  0x0077,
  0x007A,
  0x007D,
  0x0080,
  0x0083,
  0x0086,
  0x0089,
  0x008D,
  0x0090,
  0x0093,
  0x0096,
  0x0099,
  0x009D,
  0x00A0,
  0x00A3,
  0x00A7,
  0x00AA,
  0x00AE,
  0x00B1,
  0x00B5,
  0x00B9,
  0x00BD,
  0x00C0,
  0x00C4,
  0x00C9,
  0x00CD,
  0x00D1,
  0x00D6,
  0x00DA,
  0x00DF,
  0x00E4,
  0x00E9,
  0x00EE,
  0x00F4,
  0x00FA,
  0x0100,
  0x0106,
  0x010D,
  0x0114,
  0x011C,
  0x0124,
  0x012D,
  0x0136,
  0x0140,
  0x014A,
  0x0156,
  0x0162,
  0x016F,
  0x017E,
  0x018F,
  0x01A1,
  0x01B5,
  0x01CC,
  0x01E6,
  0x0203,
  0x0226,
  0x024F,
  0x027F,
  0x02BA,
  0x0303,
  0x0362,
  0x03DF,
  0x048E,
  0x0594,
  0x0747,
  0x0AAD,
  0x14DE
};

// $E600 - Vertical perspective weights — Y scale by speed and distance slot
const u8 persp_y_scale[8][22] = {
  { 0x60, 0x4A, 0x3C, 0x32, 0x2B, 0x25, 0x21, 0x1E, 0x1B, 0x18, 0x16, 0x15, 0x13, 0x12, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x4C, 0x3D, 0x33, 0x2C, 0x26, 0x22, 0x1E, 0x1B, 0x19, 0x17, 0x15, 0x13, 0x12, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x4F, 0x3F, 0x34, 0x2D, 0x27, 0x22, 0x1E, 0x1B, 0x19, 0x17, 0x15, 0x14, 0x12, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x51, 0x41, 0x35, 0x2D, 0x27, 0x23, 0x1F, 0x1C, 0x19, 0x17, 0x15, 0x14, 0x12, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x54, 0x42, 0x37, 0x2E, 0x28, 0x23, 0x1F, 0x1C, 0x1A, 0x17, 0x16, 0x14, 0x13, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x57, 0x44, 0x38, 0x2F, 0x29, 0x24, 0x20, 0x1D, 0x1A, 0x18, 0x16, 0x14, 0x13, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x59, 0x46, 0x39, 0x30, 0x29, 0x24, 0x20, 0x1D, 0x1A, 0x18, 0x16, 0x14, 0x13, 0x12, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A },
  { 0x60, 0x5D, 0x48, 0x3B, 0x31, 0x2A, 0x25, 0x21, 0x1D, 0x1A, 0x18, 0x16, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0A }
};

// $E6B0 - Horizontal perspective multipliers for right road edge curvature
const u8 persp_x_scale_right[8][22] = {
  { 0xEB, 0xC1, 0xAA, 0x8F, 0x8E, 0x6E, 0x5D, 0x66, 0x71, 0x55, 0x2E, 0x61, 0x35, 0x38, 0x3C, 0x40, 0x44, 0x49, 0x4E, 0x55, 0x5D, 0x66 },
  { 0xD5, 0xCA, 0xA7, 0x8C, 0x8B, 0x6B, 0x78, 0x66, 0x4B, 0x51, 0x59, 0x61, 0x35, 0x38, 0x3C, 0x40, 0x44, 0x49, 0x4E, 0x55, 0x5D, 0x66 },
  { 0xB5, 0xCF, 0xB3, 0x89, 0x88, 0x83, 0x78, 0x66, 0x4B, 0x51, 0x59, 0x30, 0x66, 0x38, 0x3C, 0x40, 0x44, 0x49, 0x4E, 0x55, 0x5D, 0x66 },
  { 0xA0, 0xCA, 0xBD, 0x9A, 0x88, 0x69, 0x75, 0x63, 0x6D, 0x51, 0x59, 0x30, 0x66, 0x38, 0x3C, 0x40, 0x44, 0x49, 0x4E, 0x55, 0x5D, 0x66 },
  { 0x80, 0xDB, 0xAA, 0xA7, 0x85, 0x80, 0x75, 0x63, 0x49, 0x76, 0x2C, 0x5D, 0x33, 0x6B, 0x3C, 0x40, 0x44, 0x49, 0x4E, 0x55, 0x5D, 0x66 },
  { 0x60, 0xDF, 0xB5, 0xA4, 0x82, 0x7C, 0x71, 0x60, 0x69, 0x4E, 0x55, 0x5D, 0x33, 0x6B, 0x3C, 0x40, 0x44, 0x49, 0x4E, 0x55, 0x5D, 0x66 },
  { 0x4B, 0xDB, 0xBE, 0xA1, 0x95, 0x7C, 0x71, 0x60, 0x69, 0x4E, 0x55, 0x5D, 0x33, 0x35, 0x71, 0x40, 0x44, 0x49, 0x4E, 0x55, 0x5D, 0x66 },
  { 0x20, 0xE7, 0xB9, 0xAD, 0x92, 0x79, 0x6E, 0x7C, 0x69, 0x4E, 0x55, 0x5D, 0x33, 0x35, 0x38, 0x3C, 0x40, 0x44, 0x49, 0x4E, 0x55, 0x5D }
};

// $E760 - Horizontal position deltas applied to shift left road edge relative to right
const u8 persp_x_delta_left[8][22] = {
  { 0x42, 0x2A, 0x1E, 0x15, 0x12, 0x0C, 0x09, 0x09, 0x09, 0x06, 0x03, 0x06, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00 },
  { 0x3D, 0x2D, 0x1F, 0x15, 0x12, 0x0C, 0x0C, 0x09, 0x06, 0x06, 0x06, 0x06, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00 },
  { 0x34, 0x30, 0x21, 0x15, 0x12, 0x0F, 0x0C, 0x09, 0x06, 0x06, 0x06, 0x03, 0x06, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00 },
  { 0x2E, 0x30, 0x24, 0x18, 0x12, 0x0C, 0x0C, 0x09, 0x09, 0x06, 0x06, 0x03, 0x06, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00 },
  { 0x25, 0x36, 0x21, 0x1C, 0x12, 0x0F, 0x0C, 0x09, 0x06, 0x09, 0x03, 0x06, 0x03, 0x06, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00 },
  { 0x1C, 0x3A, 0x24, 0x1B, 0x12, 0x0F, 0x0C, 0x09, 0x09, 0x06, 0x06, 0x06, 0x03, 0x06, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00 },
  { 0x15, 0x39, 0x27, 0x1B, 0x15, 0x0F, 0x0C, 0x09, 0x09, 0x06, 0x06, 0x06, 0x03, 0x03, 0x06, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00 },
  { 0x0A, 0x3F, 0x27, 0x1E, 0x15, 0x0F, 0x0C, 0x0C, 0x09, 0x06, 0x06, 0x06, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00 }
};

// $E88E
// Conv: Changed to point at fixed data only. Original game copied two of the
// transition frames around during relocation.
const transition_t transitions_e88e[8] = {
  // forward
  {  6, &square_transition_mask[-1 * 8] },
  { 12, &spiral_transition_mask[-1 * 8] },
  {  8, &circle_transition_mask[-1 * 8] },
  {  7, &diamond_transition_mask[-1 * 8] },

  // reverse
  {  6, &square_transition_mask[5 * 8] }, // too far outside?
  { 12, &spiral_transition_mask[11 * 8] },
  {  8, &circle_transition_mask[7 * 8] },
  {  7, &diamond_transition_mask[6 * 8] },
};

// 0xE8A6
const u8 square_transition_mask[5 * 8] = {
  ________,
  ________,
  ________,
  ___X____,
  ________,
  ________,
  ________,
  ________,

  ________,
  ________,
  ________,
  ___XX___,
  ___XX___,
  ________,
  ________,
  ________,

  ________,
  ________,
  __XXXX__,
  __XXXX__,
  __XXXX__,
  __XXXX__,
  ________,
  ________,

  ________,
  _XXXXXX_,
  _XXXXXX_,
  _XXXXXX_,
  _XXXXXX_,
  _XXXXXX_,
  _XXXXXX_,
  ________,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX
};

// 0xE8CE
const u8 diamond_transition_mask[6 * 8] = {
  X______X,
  _X____X_,
  __X__X__,
  ___XX___,
  ___XX___,
  __X__X__,
  _X____X_,
  X______X,

  X______X,
  XX____XX,
  _XX__XX_,
  __XXXX__,
  __XXXX__,
  _XX__XX_,
  XX____XX,
  X______X,

  XX____XX,
  XXX__XXX,
  _XXXXXX_,
  __XXXX__,
  __XXXX__,
  _XXXXXX_,
  XXX__XXX,
  XX____XX,

  XX____XX,
  XXX__XXX,
  XXXXXXXX,
  _XXXXXX_,
  _XXXXXX_,
  XXXXXXXX,
  XXX__XXX,
  XX____XX,

  XXX__XXX,
  XXXXXXXX,
  XXXXXXXX,
  _XXXXXX_,
  _XXXXXX_,
  XXXXXXXX,
  XXXXXXXX,
  XXX__XXX,

  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX,
  XXXXXXXX
};

/* ----------------------------------------------------------------------- */

// $E9B4
const u8 messages_stop_the_tape[45] = {
  attribute_GREEN_OVER_BLACK,
  TWOBYTES(0x488A),
  'S', 'T', 'O', 'P', ' ', 'T', 'H', 'E', ' ', 'T', 'A', 'P', 'E' | EOS,
  attribute_CYAN_OVER_BLACK,
  TWOBYTES(0x5044),
  'P', 'R', 'E', 'S', 'S', ' ', 'A', 'N', 'Y', ' ', 'K', 'E', 'Y', ' ', 'T', 'O', ' ', 'C', 'O', 'N', 'T', 'I', 'N', 'U', 'E' | EOS,
  0 // end marker
};

// $E9E1
const u8 messages_input_methods[112] = {
  attribute_GREEN_OVER_BLACK,
  TWOBYTES(0x484B),
  'C', 'H', 'A', 'S', 'E', ' ', 'H', '.', 'Q', '.' | EOS,
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
  0 // end marker
};

// $EA52
const u8 messages_redefine_keys[138] = {
  attribute_RED_OVER_BLACK,
  TWOBYTES(0x4849),
  'R', 'E', 'D', 'E', 'F', 'I', 'N', 'E', ' ', ' ', 'K', 'E', 'Y', 'S' | EOS,
  0xC6,  //attribute_BRIGHT_YELLOW_OVER_BLACK + single height bit
  TWOBYTES(0x48C9),
  'G', 'E', 'A', 'R', '.', '.', '.', '.', '.', '.', '.', '.' | EOS,
  0xC6,
  TWOBYTES(0x48E9),
  'A', 'C', 'C', 'E', 'L', 'E', 'R', 'A', 'T', 'E', '.', '.' | EOS,
  0xC6,
  TWOBYTES(0x5009),
  'B', 'R', 'A', 'K', 'E', '.', '.', '.', '.', '.', '.', '.' | EOS,
  0xC6,
  TWOBYTES(0x5029),
  'L', 'E', 'F', 'T', '.', '.', '.', '.', '.', '.', '.', '.' | EOS,
  0xC6,
  TWOBYTES(0x5049),
  'R', 'I', 'G', 'H', 'T', '.', '.', '.', '.', '.', '.', '.' | EOS,
  0xC4,
  TWOBYTES(0x5089),
  'Q', 'U', 'I', 'T', '.', '.', '.', '.', '.', '.', '.', '.' | EOS,
  0xC4,
  TWOBYTES(0x50A9),
  'P', 'A', 'U', 'S', 'E', '.', '.', '.', '.', '.', '.', '.' | EOS,
  0xC4,
  TWOBYTES(0x50C9),
  'T', 'U', 'R', 'B', 'O', '.', '.', '.', '.', '.', '.', '.' | EOS,
  0
};

// $EAE1
const u8 messages_test_mode[151] = {
  0xC1,
  TWOBYTES(0x4000),
  'T', 'E', 'S', 'T' | EOS,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0x4824),
  'C', 'H', 'A', 'S', 'E', ' ', 'H', '.', 'Q', '.', ' ', ' ', ' ', ' ', ' ', 'T', 'E', 'S', 'T', ' ', 'M', 'O', 'D', 'E' | EOS,
  0xC5,
  TWOBYTES(0x5000),
  'I', 'N', ' ', 'G', 'A', 'M', 'E', '.', '.', '.' | EOS,
  0xC4,
  TWOBYTES(0x5041),
  'P', 'R', 'E', 'S', 'S', ' ', '1', '.', '.', '.', '.', '.', '.', '.', ' ', 'R', 'E', 'S', 'T', 'A', 'R', 'T', ' ', 'L', 'E', 'V', 'E', 'L', '.' | EOS,
  0xC4,
  TWOBYTES(0x5067),
  '2', '.', '.', '.', '.', '.', '.', '.', ' ', 'N', 'E', 'X', 'T', ' ', 'L', 'E', 'V', 'E', 'L', '.' | EOS,
  0xC4,
  TWOBYTES(0x5087),
  '3', '.', '.', '.', '.', '.', '.', '.', ' ', 'E', 'N', 'D', ' ', 'S', 'C', 'R', 'E', 'E', 'N', '.' | EOS,
  0xC4,
  TWOBYTES(0x50A7),
  '4', '.', '.', '.', '.', '.', '.', '.', ' ', 'E', 'X', 'T', 'R', 'A', ' ', 'C', 'R', 'E', 'D', 'I', 'T', '.' | EOS,
  0
};

// $EB78
const u8 messages_cannot_be_remodified[127] = {
  attribute_RED_OVER_BLACK,
  TWOBYTES(0x484B),
  'C', 'H', 'A', 'S', 'E', ' ', ' ', 'H', '.', 'Q', '.' | EOS,
  0xC6,
  TWOBYTES(0x48C2),
  'P', 'L', 'E', 'A', 'S', 'E', ' ', 'N', 'O', 'T', 'E', ' ', 'C', 'O', 'N', 'T', 'R', 'O', 'L', ' ', 'O', 'P', 'T', 'I', 'O', 'N', 'S' | EOS,
  0xC6,
  TWOBYTES(0x48E5),
  'C', 'A', 'N', 'N', 'O', 'T', ' ', 'B', 'E', ' ', 'R', 'E', 'M', 'O', 'D', 'I', 'F', 'I', 'E', 'D', '.' | EOS,
  0xC5,
  TWOBYTES(0x5041),
  'A', 'R', 'E', ' ', 'Y', 'O', 'U', ' ', 'H', 'A', 'P', 'P', 'Y', ' ', 'W', 'I', 'T', 'H', ' ', 'Y', 'O', 'U', 'R', ' ', 'C', 'H', 'O', 'I', 'C', 'E', '.' | EOS,
  0x07,
  TWOBYTES(0x5085),
  'P', 'R', 'E', 'S', 'S', ' ', 'Y', 'E', 'S', '(', 'Y', ')', ' ', 'O', 'R', ' ', 'N', 'O', '(', 'N', ')' | EOS,
  0
};

/* ----------------------------------------------------------------------- */

// $EDD6
const u8 key_names[10 * 8] =
  "B N M SYSP"
  "H J K L EN"
  "Y U I O P "
  "6 7 8 9 0 "
  "5 4 3 2 1 "
  "T R E W Q "
  "G F D S A "
  "V C X Z CP";

// $EE26
const u8 sinclair_joy_keydefs[5] = {
  KEYDEF(4, 3), // 0
  KEYDEF(3, 3), // 9
  KEYDEF(2, 3), // 8
  KEYDEF(0, 3), // 6
  KEYDEF(1, 3)  // 7
};

// $EE2B
const u8 cursor_joy_keydefs[5] = {
  KEYDEF(4, 3), // 0
  KEYDEF(1, 3), // 7
  KEYDEF(0, 3), // 6
  KEYDEF(0, 4), // 5
  KEYDEF(2, 3)  // 8
};

// $EE30
const u8 shocked_keydefs[8] = {
  KEYDEF(3, 6), // S
  KEYDEF(0, 1), // H
  KEYDEF(3, 2), // O
  KEYDEF(1, 7), // C
  KEYDEF(2, 1), // K
  KEYDEF(2, 5), // E
  KEYDEF(2, 6), // D
  KEYDEF(4, 1)  // <ENTER>
};

/* ----------------------------------------------------------------------- */

const u8 drum1[252] = {
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
  0x38, 0xF0, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xC6, 0xDB, 0x9D, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFE, 0x07, 0x70, 0x60, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x08, 0x80, 0x77, 0xDF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFE, 0xFF, 0xF8, 0xA3, 0xCE, 0x1C, 0x64, 0x02,
  0x20, 0x00, 0x0E, 0x00, 0x00, 0x3F, 0x18, 0x8F,
  0x9F, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xEF, 0xFF, 0xFF, 0xEF, 0xFF, 0x3F, 0xFC,
  0x9C, 0x38, 0xCE, 0x00, 0x03, 0x00, 0x00, 0x00,
  0xC4, 0x7F, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x3E, 0x40, 0x98, 0x10, 0xC0, 0x1F, 0x80, 0x00,
  0xFF, 0x00, 0x1F, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xB8, 0x77,
  0x7E, 0xFC, 0xFE, 0x7F, 0xE7, 0x6F, 0xDE, 0xFF,
  0xFE, 0x07, 0xF9, 0xFF, 0x3B, 0x3F, 0x3F, 0xFE,
  0xDF, 0xFF, 0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0x03, 0xFE, 0xFF, 0x33, 0xA7, 0xFF,
  0x9F, 0xFF, 0xEF, 0x00
};

const u8 drum2[108] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xF8, 0x30,
  0xFF, 0xDE, 0x00, 0xFF, 0x81, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF4, 0x3B, 0x80,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00,
  0x00, 0x00, 0x1B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFD, 0x7E, 0x00
};

/* ----------------------------------------------------------------------- */

// $F0FE
const u8 music_patterns[19] = {
  // (repetitions, offset)
  0x01, 0x29,
  0x01, 0x2C, // restart is here
  0x04, 0x00,
  0x04, 0x71,
  0x04, 0x91,
  0x04, 0xAD,
  0x07, 0xCD,
  0x01, 0xED,
  0xFF, // stop marker
  TWOBYTES(0xF100) // restart address
};

// $F111
const u8 music_data[271] = {
  0x05, 0xC1, 0x1B, 0x1B, 0x9B, 0x1A, 0x1B, 0x42, // A
  0x9B, 0x1A, 0x1B, 0x41, 0xC1, 0x0A, 0x1B, 0x41,
  0x1B, 0x42, 0x1B, 0x1B, 0x1B, 0x41, 0x1B, 0x9B,
  0x1A, 0x1B, 0x42, 0x9B, 0x1A, 0x41, 0x1B, 0xC2,
  0x0A, 0x42, 0x1B, 0x41, 0x42, 0x42, 0x42, 0x41,
  0x01,
  0x05, 0x4B, 0x01, // B
  0x05, 0x1B, 0x1B, 0x1B, 0x4B, 0x1B, 0x1B, 0x4B, // C
  0x1B, 0x1B, 0x1B, 0x1B, 0x4B, 0x1B, 0x4B, 0x1B,
  0x1B, 0x1B, 0x4B, 0x1B, 0x4B, 0x1B, 0x1B, 0x4B,
  0x1B, 0x1B, 0x4B, 0x1B, 0x1B, 0x4B, 0x1B, 0x1B,
  0x4B, 0x1B, 0x1B, 0x1B, 0x4B, 0x1B, 0x1B, 0x4B,
  0x1B, 0x1B, 0x1B, 0x1B, 0x4B, 0x1B, 0x4B, 0x1B,
  0x1B, 0x1B, 0x4B, 0x1B, 0x4B, 0x1B, 0x1B, 0x4B,
  0x1B, 0x1B, 0x4B, 0x1B, 0x9B, 0x42, 0xCB, 0x42,
  0x9B, 0x42, 0x9B, 0x42, 0x01,
  0x05, 0xC1, 0x1A, 0x9B, 0x1A, 0x9B, 0x1A, 0x9A, // D
  0x41, 0x9B, 0x42, 0x9B, 0x0A, 0x1B, 0x9B, 0x0A,
  0x9B, 0x0A, 0x9B, 0x41, 0x9B, 0x1A, 0xC1, 0x1A,
  0x9B, 0x42, 0x1B, 0x9B, 0x42, 0x9B, 0x42, 0x01,
  0x05, 0xC1, 0x1A, 0x9B, 0x1A, 0x1B, 0x8A, 0x41, // E
  0x9B, 0x42, 0x1B, 0x1B, 0x1B, 0x9B, 0x1A, 0x9B,
  0x1A, 0x1B, 0xC1, 0x1B, 0x9B, 0x42, 0x1B, 0x9B,
  0x0A, 0x9B, 0x0A, 0x01,
  0x05, 0xC1, 0x1A, 0x9B, 0x1A, 0x9B, 0x0A, 0x9A, // F
  0x41, 0x9B, 0x42, 0x9B, 0x0A, 0x1B, 0x9B, 0x0A,
  0x9B, 0x1A, 0x9B, 0x1A, 0x1B, 0xC1, 0x0A, 0x9B,
  0x42, 0x9B, 0x0A, 0x9B, 0x42, 0x9B, 0x0A, 0x01,
  0x05, 0xC1, 0x0A, 0x9B, 0x0A, 0x9B, 0x0A, 0xC1, // G
  0x1B, 0x9B, 0x42, 0x1B, 0x8A, 0x1B, 0x9B, 0x0A,
  0x9B, 0x0A, 0x9B, 0x41, 0x9B, 0x1A, 0xC1, 0x1A,
  0x9B, 0x42, 0x1B, 0x9B, 0x42, 0x9B, 0x42, 0x01,
  0x05, 0xC1, 0x0A, 0x9B, 0x0A, 0x9B, 0x0A, 0xC1, // H
  0x1B, 0x9B, 0x42, 0x1B, 0x8A, 0x1B, 0x9B, 0x0A,
  0x9B, 0x42, 0xC1, 0x42, 0x9B, 0x42, 0xC1, 0x42,
  0x9B, 0x42, 0x9B, 0x42, 0x9B, 0x42, 0x9B, 0x42,
  0x42, 0x01
};

/* ----------------------------------------------------------------------- */

// $F491
const u8 press_gear_messages[17] = {
  DRAWCHARSTYLE_SINGLE,
  attribute_BLACK_OVER_BLACK,
  TWOBYTES(0xF84B),
  TWOBYTES(0x59AB),
  'P', 'R', 'E', 'S', 'S', ' ', ' ', 'G', 'E', 'A', 'R' | EOS
};

// $F4A2
const u8 enter_for_options_messages[23] = {
  DRAWCHARSTYLE_SINGLE,
  attribute_BLACK_OVER_BLACK,
  TWOBYTES(0xF848),
  TWOBYTES(0x59A8),
  'E', 'N', 'T', 'E', 'R', ' ', 'F', 'O', 'R', ' ', 'O', 'P', 'T', 'I', 'O', 'N', 'S' | EOS
};

// $F4B9
const u8 credits_messages_128[98] = {
  10,
  8,
  DRAWCHARSTYLE_DOUBLE,
  attribute_BLACK_OVER_BLACK,
  TWOBYTES(0xF02D),
  TWOBYTES(0x594D),
  'C', 'R', 'E', 'D', 'I', 'T', 'S' | EOS,
  8,
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0xF086),
  TWOBYTES(0x5A06),
  'P', 'R', 'O', 'G', 'R', 'A', 'M', ' ', ' ', ' ', ' ', ' ', ' ', 'J', 'O', 'B', 'B', 'E', 'E', 'E' | EOS,
  8,
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0xF0A6),
  TWOBYTES(0x5A46),
  'G', 'R', 'A', 'P', 'H', 'I', 'C', 'S', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'I', 'L', 'L' | EOS,
  0x50,
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0xF0C6),
  TWOBYTES(0x5A86),
  'M', 'U', 'S', 'I', 'C', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'J', 'O', 'N', ' ', 'D', 'U', 'N', 'N' | EOS,
  3,
  0
};

// $F51B
const u8 best_officers[163] = {
  10,
  8,
  DRAWCHARSTYLE_DOUBLE,
  attribute_BLACK_OVER_BLACK,
  TWOBYTES(0xF02A),
  TWOBYTES(0x594A),
  'B', 'E', 'S', 'T', ' ', 'O', 'F', 'F', 'I', 'C', 'E', 'R', 'S' | EOS,
  8,
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0xF082),
  TWOBYTES(0x5A02),
  'R', 'A', 'N', 'K', ' ', ' ', 'S', 'C', 'O', 'R', 'E', ' ', ' ', 'S', 'T', 'A', 'G', 'E', ' ', 'P', 'L', 'A', 'Y', ' ', 'N', 'A', 'M', 'E' | EOS,
  8,
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0xF0A2),
  TWOBYTES(0x5A42),
  '1', 'S', 'T', ' ', ' ', '5', '6', '7', '8', '4', '0', '1', '0', ' ', ' ', 'A', 'L', 'L', ' ', ' ', ' ', ' ', '1', ' ', ' ', 'J', 'O', 'B' | EOS,
  8,
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0xF0C2),
  TWOBYTES(0x5A82),
  '2', 'N', 'D', ' ', ' ', '3', '5', '6', '7', '8', '0', '0', '0', ' ', ' ', ' ', '4', ' ', ' ', ' ', ' ', ' ', '1', ' ', ' ', 'A', 'B', 'C' | EOS,
  0x50,
  DRAWCHARSTYLE_SINGLE,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0xF0E2),
  TWOBYTES(0x5AC2),
  '3', 'R', 'D', ' ', ' ', ' ', '4', '3', '4', '0', '3', '0', '0', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', '2', ' ', ' ', 'D', 'E', 'F' | EOS,
  3,
  0
};

/* ----------------------------------------------------------------------- */

// $F5BE
const u8 marquee_initial[SCREEN_BITMAP_ROWBYTES * MARQUEE_HEIGHT] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x05, 0x00, 0x00, 0xFF, 0x74, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0x00, 0xBA, 0xBA, 0x00, 0xBA, 0xBA, 0xBA, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFF, 0x05, 0x00, 0x00, 0xFF,
  0xFF, 0x1D, 0xDD, 0x06, 0xFF, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFF, 0x1D, 0xDD, 0x06, 0xFF,
  0xFE, 0x6A, 0x80, 0x07, 0x3F, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFE, 0x6A, 0x80, 0x07, 0x3F,
  0xF9, 0x12, 0xA5, 0x48, 0x5F, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBA, 0xBA, 0xBA, 0xBA, 0x06, 0x00, 0x00, 0x00, 0x00, 0x67, 0xF9, 0x12, 0xA5, 0x48, 0x5F,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3B, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x00, 0x7C, 0x7C, 0x00, 0x7C, 0x7C, 0x7C, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x0A, 0x80, 0x08, 0xFF, 0x76, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFF, 0x0A, 0x80, 0x08, 0xFF,
  0xFE, 0x3F, 0x77, 0x0A, 0x7F, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFE, 0x3F, 0x77, 0x0A, 0x7F,
  0xFC, 0x30, 0x00, 0x0A, 0x3F, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x7C, 0x7C, 0x7C, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFC, 0x30, 0x00, 0x0A, 0x3F,
  0xF2, 0x25, 0xC3, 0xA4, 0x2F, 0xEA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x67, 0xF2, 0x25, 0xC3, 0xA4, 0x2F,
  0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFF, 0xE0, 0x00, 0x07, 0xFF, 0x3A, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0x00, 0xBA, 0xBA, 0x00, 0xBA, 0xBA, 0xBA, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6C, 0xFF, 0xE0, 0x00, 0x07, 0xFF,
  0xFF, 0x07, 0x00, 0x04, 0xFF, 0x74, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFF, 0x07, 0x00, 0x04, 0xFF,
  0xFE, 0x1A, 0xAA, 0x04, 0x7F, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFE, 0x1A, 0xAA, 0x04, 0x7F,
  0xFC, 0x6A, 0x80, 0x05, 0x3F, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBA, 0xBA, 0xBA, 0xBA, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFC, 0x6A, 0x80, 0x05, 0x3F,
  0xE4, 0x32, 0xA5, 0xCA, 0x4F, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x67, 0xE4, 0x32, 0xA5, 0xCA, 0x4F,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0xFB, 0x69, 0x06, 0x63, 0x39, 0xC2, 0x6B, 0xDD, 0x91, 0xDD, 0x2E, 0x5A, 0x37, 0x3B, 0xB8, 0x4A, 0xD4, 0x80, 0x96, 0xDF, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFF, 0xC0, 0x00, 0x03, 0xFF, 0x3A, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6C, 0xFF, 0xC0, 0x00, 0x03, 0xFF,
  0xFF, 0x0A, 0xA0, 0x08, 0xFF, 0x76, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFF, 0x0A, 0xA0, 0x08, 0xFF,
  0xFE, 0x35, 0x40, 0x0A, 0x7F, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCE, 0x67, 0x01, 0xC9, 0xB9, 0x92, 0x66, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFE, 0x35, 0x40, 0x0A, 0x7F,
  0xFC, 0x50, 0x00, 0x0A, 0x3F, 0xF6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6F, 0xFC, 0x50, 0x00, 0x0A, 0x3F,
  0xE2, 0x47, 0xC2, 0xA2, 0x27, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x67, 0xE2, 0x47, 0xC2, 0xA2, 0x27,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, 0xD4, 0x00, 0x08, 0x94, 0xA5, 0x00, 0x00, 0x00, 0x00, 0x89, 0xE8, 0x00, 0x44, 0xA2, 0x24, 0x00, 0x00, 0x00, 0x00, 0x2B, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFF, 0xC0, 0x00, 0x03, 0xFF, 0x3A, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6C, 0xFF, 0xC0, 0x00, 0x03, 0xFF,
  0xFF, 0x17, 0x00, 0x04, 0xFF, 0x74, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFF, 0x17, 0x00, 0x04, 0xFF,
  0xFE, 0x3A, 0xA0, 0x06, 0x7F, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x94, 0x81, 0x2A, 0x12, 0x52, 0x94, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFE, 0x3A, 0xA0, 0x06, 0x7F,
  0xFC, 0x6A, 0x80, 0x17, 0x3F, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x67, 0xFC, 0x6A, 0x80, 0x17, 0x3F,
  0xC4, 0x6A, 0x81, 0xF6, 0x17, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x67, 0xC4, 0x6A, 0x81, 0xF6, 0x17,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0xA0, 0x00, 0x04, 0x84, 0xB9, 0x80, 0x00, 0x00, 0x00, 0x89, 0x2C, 0x00, 0x27, 0x33, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFF, 0x80, 0x00, 0x03, 0xFF, 0x3A, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6C, 0xFF, 0x80, 0x00, 0x01, 0xFF,
  0xFF, 0x1A, 0xA0, 0x08, 0xFF, 0x76, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFF, 0x1A, 0xA0, 0x08, 0xFF,
  0xFE, 0x34, 0x00, 0x0A, 0x7F, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x4C, 0xF7, 0x01, 0x29, 0x13, 0xDA, 0x86, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFE, 0x34, 0x00, 0x0A, 0x7F,
  0xFC, 0x30, 0x00, 0x0A, 0x3F, 0xEA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x67, 0xFC, 0x30, 0x00, 0x0A, 0x3F,
  0xCA, 0x47, 0xC2, 0xA2, 0x27, 0xEA, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x67, 0xCA, 0x47, 0xC2, 0xA2, 0x27,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x00, 0x00, 0x02, 0x94, 0xA5, 0x00, 0x00, 0x00, 0x00, 0x89, 0x28, 0x00, 0x14, 0x22, 0x24, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFF, 0x80, 0x00, 0x01, 0xFF, 0x7A, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFF, 0x80, 0x00, 0x01, 0xFF,
  0xFF, 0x15, 0x00, 0x04, 0xFF, 0x74, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0x00, 0xBA, 0xBA, 0x00, 0xBA, 0xBA, 0xBA, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFF, 0x15, 0x00, 0x04, 0xFF,
  0xFE, 0x7A, 0xA0, 0x04, 0x7F, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x28, 0x94, 0x81, 0x28, 0x92, 0x56, 0x94, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFE, 0x7A, 0xA0, 0x04, 0x7F,
  0xFC, 0x6A, 0x80, 0x15, 0x3F, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x67, 0xFC, 0x6A, 0x80, 0x15, 0x3F,
  0xC4, 0x6A, 0x81, 0xF6, 0x13, 0xED, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBA, 0xBA, 0xBA, 0xBA, 0x06, 0x00, 0x00, 0x00, 0x00, 0x67, 0xC4, 0x6A, 0x81, 0xF6, 0x13,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x80, 0x00, 0x0C, 0x63, 0x25, 0xC0, 0x00, 0x00, 0x00, 0x9D, 0x2E, 0x00, 0x64, 0x3B, 0xB8, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xAC, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFF, 0x80, 0x00, 0x01, 0xFF, 0x76, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0x00, 0xBA, 0xBA, 0x00, 0xBA, 0xBA, 0xBA, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFF, 0x80, 0x00, 0x01, 0xFF,
  0xFF, 0x1E, 0xA0, 0x2A, 0xFF, 0x76, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x00, 0x7C, 0x7C, 0x00, 0x7C, 0x7C, 0x7C, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFF, 0x1E, 0xA0, 0x2A, 0xFF,
  0xFE, 0x50, 0x00, 0x0A, 0x7F, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCE, 0x94, 0x81, 0xCB, 0x12, 0x52, 0x66, 0x06, 0x00, 0x00, 0x00, 0x00, 0x6E, 0xFE, 0x50, 0x00, 0x0A, 0x7F,
  0xFC, 0x50, 0x00, 0x2A, 0x3F, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBA, 0xBA, 0xBA, 0xBA, 0x06, 0x00, 0x00, 0x00, 0x00, 0x67, 0xFC, 0x50, 0x00, 0x2A, 0x3F,
  0xCA, 0x47, 0x42, 0xA2, 0x2B, 0xE7, 0xDA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x7C, 0x7C, 0x7C, 0x06, 0x00, 0x00, 0x00, 0x00, 0x67, 0xCA, 0x47, 0x42, 0xA2, 0x2B,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// $FDBE
const u8 marquee_attrs[SCREEN_ATTRIBUTES_WIDTH * MARQUEE_HEIGHT / 8] = {
  0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47,
  0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47,
  0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47,
  0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47,
  0x10, 0x10, 0x10, 0x10, 0x10, 0x47, 0x45, 0x45,
  0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x47, 0x46,
  0x46, 0x47, 0x45, 0x45, 0x45, 0x47, 0x06, 0x06,
  0x06, 0x06, 0x47, 0x08, 0x08, 0x08, 0x08, 0x08,
  0x10, 0x17, 0x17, 0x11, 0x10, 0x47, 0x05, 0x05,
  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x07, 0x06,
  0x06, 0x07, 0x05, 0x05, 0x05, 0x47, 0x06, 0x06,
  0x06, 0x06, 0x47, 0x08, 0x0D, 0x0D, 0x08, 0x08,
  0x10, 0x17, 0x17, 0x11, 0x10, 0x47, 0x44, 0x04,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x47, 0x47, 0x47,
  0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x06, 0x06,
  0x06, 0x06, 0x47, 0x08, 0x0D, 0x0D, 0x08, 0x08,
  0x10, 0x17, 0x17, 0x11, 0x10, 0x47, 0x44, 0x44,
  0x44, 0x44, 0x44, 0x44, 0x04, 0x47, 0x43, 0x43,
  0x05, 0x04, 0x04, 0x04, 0x04, 0x47, 0x06, 0x06,
  0x06, 0x06, 0x47, 0x08, 0x0D, 0x0D, 0x08, 0x08,
  0x28, 0x6C, 0x3E, 0x6C, 0x28, 0x47, 0x04, 0x44,
  0x44, 0x44, 0x44, 0x44, 0x04, 0x47, 0x03, 0x03,
  0x45, 0x44, 0x44, 0x44, 0x44, 0x47, 0x06, 0x06,
  0x06, 0x06, 0x47, 0x28, 0x6C, 0x3E, 0x6C, 0x28,
  0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78,
  0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78,
  0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78,
  0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78,
  0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78,
  0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78,
  0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78,
  0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78
};

/* $86F6: backdrop blit instruction templates (36 bytes, two 18-byte chunks).
 * Chunk 0 (bytes 0–17):  INC L (0x2C) + NOP (0x00) × 9 — skip backdrop bytes.
 * Chunk 1 (bytes 18–35): LDI   (0xED, 0xA0) × 9       — copy backdrop bytes.
 * dr_start_backdrop_fill copies 18 bytes starting at offset dr_backdrop_copy_jump into
 * state->dr_backdrop_copy_instrs, which the blit loop then interprets. */
const u8 backdrop_copy_instrs_template[36] = {
  0x2C, 0x00, 0x2C, 0x00, 0x2C, 0x00, 0x2C, 0x00, 0x2C, 0x00,
  0x2C, 0x00, 0x2C, 0x00, 0x2C, 0x00, 0x2C, 0x00,
  0xED, 0xA0, 0xED, 0xA0, 0xED, 0xA0, 0xED, 0xA0, 0xED, 0xA0,
  0xED, 0xA0, 0xED, 0xA0, 0xED, 0xA0, 0xED, 0xA0
};
