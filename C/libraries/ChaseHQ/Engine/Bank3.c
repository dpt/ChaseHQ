/**
 * Bank3.c
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

/* Notes
 *
 * This file holds the C translation of ZX Spectrum 128K "bank 3" -- the
 * animated title screen, the title/success music, and the keyboard/joystick
 * control-select and key-redefinition screens reached from it. These
 * routines are paged into $C000-$FFFF and dispatched from mainline code
 * (Main.c) via bank3_call(), whose entry point and BANK3_* dispatch
 * constants are declared in Internal.h alongside the handful of low-level
 * helpers (z80addrtoscreen, z80addrtoattrs, setwordat) and the bank-4 title-
 * tune functions (titlescr_start_ay, titlescr_ay_music) shared with this file.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "C99/Types.h"
#include "ZXSpectrum/Macros.h"
#include "ZXSpectrum/Pixels.h"
#include "ZXSpectrum/Spectrum.h"
#include "ZXSpectrum/Z80.h"
#include "ZXSpectrum/slopay-chip.h"

#include "ChaseHQ/ChaseHQ.h"

#include "ChaseHQ/Data/CommonData.h"

#include "Types.h"
#include "Internal.h"
#include "State.h"
#include "Bank3State.h"

#include "Bank3.h"
#include "Tests.h"

/* ----------------------------------------------------------------------- */

#define TITLE_SCENE_COUNT          (5)
#define TITLE_SCENE_DATA_BASE (0xCCB7)
#define TITLE_GLYPH_COUNT        (115)

typedef struct title_glyph
{
  u8        height_pairs; /* number of scanline-pairs to blit */
  u8        width_bytes;  /* glyph width in screen bytes (1-7) */
  const u8 *bitmap;       /* source pixel bytes, OR-blitted verbatim */
} title_glyph_t;

/* Object animation script opcodes (title_object::opcode / script byte-code
 * in title_scene_data above). Bytes with the sign bit clear ($00-$7F) are
 * immediate step deltas, not opcodes -- see oss_op_immediate_step in
 * object_script_step's prologue (Bank3.c).
 *
 * Each opcode has a _VAL form (its raw byte value, used as a switch case
 * label in object_script_step) and a function-like macro of the same name
 * that emits the opcode byte followed by its operand bytes in on-disk
 * order, for use inside title_scene_data's initializer below. */
#define OSS_OP_SET_ROW_VAL        (0xC8) /* set screen-row byte, 1 operand */
#define OSS_OP_VELOCITY_VAL       (0xC9) /* constant velocity, 3 operands: x,y,wait */
#define OSS_OP_DECEL_X_VAL        (0xCA) /* decelerate X, 3 operands */
#define OSS_OP_DECEL_Y_VAL        (0xCB) /* decelerate Y, 3 operands */
#define OSS_OP_ACCEL_X_A_VAL      (0xCC) /* accelerate X variant a, 3 operands */
#define OSS_OP_ACCEL_X_C_VAL      (0xCD) /* accelerate X variant c, 3 operands */
#define OSS_OP_ACCEL_X_B_VAL      (0xCE) /* accelerate X variant b, 3 operands */
#define OSS_OP_WAIT_VAL           (0xCF) /* wait N frames, 1 operand */
#define OSS_OP_JUMP_POSITION_VAL  (0xD0) /* jump to absolute position, 2 operands */
#define OSS_OP_DEAD_VAL           (0xD1) /* unrecognized/dead value -- never explicitly
                                           * emitted as a case, falls to default in both
                                           * switches; see object_script_step's prologue */
#define OSS_OP_END_SCRIPT_VAL     (0xD2) /* end of script -- POP HL; RET stack unwind,
                                           * see object_script_step's own Conv note */

#define OSS_OP_SET_ROW(row)                     OSS_OP_SET_ROW_VAL, (row)
#define OSS_OP_VELOCITY(x_step, y_step, wait)   OSS_OP_VELOCITY_VAL, (x_step), (y_step), (wait)
#define OSS_OP_DECEL_X(y_step_seed, wait, x_step) OSS_OP_DECEL_X_VAL, (y_step_seed), (wait), (x_step)
#define OSS_OP_DECEL_Y(y_step_seed, wait, x_step) OSS_OP_DECEL_Y_VAL, (y_step_seed), (wait), (x_step)
#define OSS_OP_ACCEL_X_A(x_step_seed, wait, y_step) OSS_OP_ACCEL_X_A_VAL, (x_step_seed), (wait), (y_step)
#define OSS_OP_ACCEL_X_C(x_step_seed, wait, y_step) OSS_OP_ACCEL_X_C_VAL, (x_step_seed), (wait), (y_step)
#define OSS_OP_ACCEL_X_B(x_step_seed, wait, y_step) OSS_OP_ACCEL_X_B_VAL, (x_step_seed), (wait), (y_step)
#define OSS_OP_WAIT(wait)                       OSS_OP_WAIT_VAL, (wait)
#define OSS_OP_JUMP_POSITION(x, y)              OSS_OP_JUMP_POSITION_VAL, (x), (y)
#define OSS_OP_DEAD()                           OSS_OP_DEAD_VAL
#define OSS_OP_END_SCRIPT()                     OSS_OP_END_SCRIPT_VAL

/* Index names for note_periods (compute_channel_ay_registers, Bank3.c), in
 * scientific pitch notation (A4 = 440 Hz, C4 = middle C), derived from each
 * entry's AY tone period. */
enum note_index
{
  NOTE_AS0, NOTE_B0,
  NOTE_C1, NOTE_CS1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_F1, NOTE_FS1, NOTE_G1, NOTE_GS1, NOTE_A1, NOTE_AS1, NOTE_B1,
  NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2,
  NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3,
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
  NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
  NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
  NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7,
  NOTE_C8, NOTE_CS8, NOTE_D8, NOTE_DS8, NOTE_E8, NOTE_F8, NOTE_FS8, NOTE_G8, NOTE_GS8, NOTE_A8
};

/* One tune's entry in the tune-select table below: a tempo/speed byte plus
 * the raw Z80 address of each of the 3 channels' pattern-data blocks. */
typedef struct tune
{
  u8  tempo;                   /* tune tempo/speed byte */
  u16 channel_pattern_addr[3]; /* raw Z80 address of each channel's pattern-data block */
} tune_t;

/* Z80 base addresses of the three transcribed pattern-data regions. The
 * tunes table above stores raw addresses in this range; resolve_phrase_addr
 * (Bank3.c) subtracts these to index the arrays below. */
#define TITLE_TUNE0_DATA_ADDR  (0xF241)
#define TITLE_TUNE1_DATA_ADDR  (0xF601)
#define TITLE_TUNE23_DATA_ADDR (0xF6DF)

#define DRUM_CUE_SCRIPT_DATA_BASE (0xFA75)

/* ----------------------------------------------------------------------- */

// clang-format off

/* $D462-$EB9D: title_glyph_bitmap_NNN -- glyph/sprite bitmap data (car, truck, logo
 * letters), one array per title_glyph_table entry below. */

/** $D462: glyph 0 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_000[112] = {
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
};

/** $D4D2: glyph 1 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_001[112] = {
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
};

/** $D542: glyph 2 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_002[112] = {
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
};

/** $D5B2: glyph 3 bitmap (140 bytes, 14 row-pairs x 5 width-bytes). */
static const u8 title_glyph_bitmap_003[140] = {
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
};

/** $D63E: glyph 4 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_004[112] = {
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
};

/** $D6AE: glyph 5 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_005[112] = {
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
};

/** $D71E: glyph 6 bitmap (140 bytes, 14 row-pairs x 5 width-bytes). */
static const u8 title_glyph_bitmap_006[140] = {
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
};

/** $D7AA: glyph 7 bitmap (140 bytes, 14 row-pairs x 5 width-bytes). */
static const u8 title_glyph_bitmap_007[140] = {
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
};

/** $D836: glyph 8 bitmap (84 bytes, 14 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_008[84] = {
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
};

/** $D88A: glyph 9 bitmap (84 bytes, 14 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_009[84] = {
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
};

/** $D8DE: glyph 10 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_010[112] = {
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
};

/** $D94E: glyph 11 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_011[112] = {
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
};

/** $D9BE: glyph 12 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_012[112] = {
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
};

/** $DA2E: glyph 13 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_013[112] = {
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
};

/** $DA9E: glyph 14 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_014[112] = {
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
};

/** $DB0E: glyph 15 bitmap (140 bytes, 14 row-pairs x 5 width-bytes). */
static const u8 title_glyph_bitmap_015[140] = {
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
};

/** $DB9A: glyph 16 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_016[112] = {
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
};

/** $DC0A: glyph 17 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_017[112] = {
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
};

/** $DC7A: glyph 18 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_018[112] = {
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
};

/** $DCEA: glyph 19 bitmap (112 bytes, 14 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_019[112] = {
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
};

/** $DD5A: glyph 20 bitmap (128 bytes, 16 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_020[128] = {
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
};

/** $DDDA: glyph 21 bitmap (128 bytes, 16 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_021[128] = {
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
};

/** $DE5A: glyph 22 bitmap (128 bytes, 16 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_022[128] = {
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
};

/** $DEDA: glyph 23 bitmap (160 bytes, 16 row-pairs x 5 width-bytes). */
static const u8 title_glyph_bitmap_023[160] = {
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
};

/** $DF7A: glyph 24 bitmap (16 bytes, 4 row-pairs x 6 width-bytes). */
static const u8 title_glyph_bitmap_024[16] = {
  __XXXXXX, X_______,
  _X_X_X__, XX______,
  _XX_XXX_, _X______,
  XXXXXXXX, _X______,
  XXXXXXXX, XX______,
  X_X_X_X_, X_______,
  XX_____X, X_______,
  _XXXXXXX, ________,
};

/** $DF8A: glyph 25 bitmap (16 bytes, 4 row-pairs x 6 width-bytes). */
static const u8 title_glyph_bitmap_025[16] = {
  ____XXXX, XXX_____,
  ___X_X_X, __XX____,
  ___XX_XX, X__X____,
  __XXXXXX, XX_X____,
  __XXXXXX, XXXX____,
  __X_X_X_, X_X_____,
  __XX____, _XX_____,
  ___XXXXX, XX______,
};

/** $DF9A: glyph 26 bitmap (16 bytes, 4 row-pairs x 6 width-bytes). */
static const u8 title_glyph_bitmap_026[16] = {
  ______XX, XXXXX___,
  _____X_X, _X__XX__,
  _____XX_, XXX__X__,
  ____XXXX, XXXX_X__,
  ____XXXX, XXXXXX__,
  ____X_X_, X_X_X___,
  ____XX__, ___XX___,
  _____XXX, XXXX____,
};

/** $DFAA: glyph 27 bitmap (16 bytes, 4 row-pairs x 6 width-bytes). */
static const u8 title_glyph_bitmap_027[16] = {
  ________, XXXXXXX_,
  _______X, _X_X__XX,
  _______X, X_XXX__X,
  ______XX, XXXXXX_X,
  ______XX, XXXXXXXX,
  ______X_, X_X_X_X_,
  ______XX, _____XX_,
  _______X, XXXXXX__,
};

/** $DFBA: glyph 28 bitmap (78 bytes, 13 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_028[78] = {
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
};

/** $E008: glyph 29 bitmap (80 bytes, 10 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_029[80] = {
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
};

/** $E058: glyph 30 bitmap (72 bytes, 9 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_030[72] = {
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
};

/** $E0A0: glyph 31 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_031[60] = {
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
};

/** $E0DC: glyph 32 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_032[60] = {
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
};

/** $E118: glyph 33 bitmap (80 bytes, 10 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_033[80] = {
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
};

/** $E168: glyph 34 bitmap (80 bytes, 10 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_034[80] = {
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
};

/** $E1B8: glyph 35 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_035[28] = {
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
};

/** $E1D4: glyph 36 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_036[28] = {
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
};

/** $E1F0: glyph 37 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_037[28] = {
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
};

/** $E20C: glyph 38 bitmap (42 bytes, 7 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_038[42] = {
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
};

/** $E236: glyph 39 bitmap (8 bytes, 4 row-pairs x 1 width-bytes). */
static const u8 title_glyph_bitmap_039[8] = {
  __XXXX__,
  _X__X_X_,
  _X_X_XX_,
  XXX_____,
  X_X_XX__,
  XXXXXX__,
  X_X_XX__,
  _XXXX___,
};

/** $E23E: glyph 40 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_040[16] = {
  ____XXXX, ________,
  ___X__X_, X_______,
  ___X_X_X, X_______,
  __XXX___, ________,
  __X_X_XX, ________,
  __XXXXXX, ________,
  __X_X_XX, ________,
  ___XXXX_, ________,
};

/** $E24E: glyph 41 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_041[16] = {
  ______XX, XX______,
  _____X__, X_X_____,
  _____X_X, _XX_____,
  ____XXX_, ________,
  ____X_X_, XX______,
  ____XXXX, XX______,
  ____X_X_, XX______,
  _____XXX, X_______,
};

/** $E25E: glyph 42 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_042[16] = {
  ________, XXXX____,
  _______X, __X_X___,
  _______X, _X_XX___,
  ______XX, X_______,
  ______X_, X_XX____,
  ______XX, XXXX____,
  ______X_, X_XX____,
  _______X, XXX_____,
};

/** $E26E: glyph 43 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_043[60] = {
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
};

/** $E2AA: glyph 44 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_044[60] = {
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
};

/** $E2E6: glyph 45 bitmap (80 bytes, 10 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_045[80] = {
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
};

/** $E336: glyph 46 bitmap (80 bytes, 10 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_046[80] = {
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
};

/** $E386: glyph 47 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_047[28] = {
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
};

/** $E3A2: glyph 48 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_048[28] = {
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
};

/** $E3BE: glyph 49 bitmap (42 bytes, 7 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_049[42] = {
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
};

/** $E3E8: glyph 50 bitmap (42 bytes, 7 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_050[42] = {
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
};

/** $E412: glyph 51 bitmap (8 bytes, 4 row-pairs x 1 width-bytes). */
static const u8 title_glyph_bitmap_051[8] = {
  __XX__XX,
  __XX_X_X,
  _XX_X_XX,
  _XXXXXXX,
  _X_X_XX_,
  XXX_XXX_,
  XXX_XXX_,
  _X___X__,
};

/** $E41A: glyph 52 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_052[16] = {
  ____XX__, XX______,
  ____XX_X, _X______,
  ___XX_X_, XX______,
  ___XXXXX, XX______,
  ___X_X_X, X_______,
  __XXX_XX, X_______,
  __XXX_XX, X_______,
  ___X___X, ________,
};

/** $E42A: glyph 53 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_053[16] = {
  ______XX, __XX____,
  ______XX, _X_X____,
  _____XX_, X_XX____,
  _____XXX, XXXX____,
  _____X_X, _XX_____,
  ____XXX_, XXX_____,
  ____XXX_, XXX_____,
  _____X__, _X______,
};

/** $E43A: glyph 54 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_054[16] = {
  ________, XX__XX__,
  ________, XX_X_X__,
  _______X, X_X_XX__,
  _______X, XXXXXX__,
  _______X, _X_XX___,
  ______XX, X_XXX___,
  ______XX, X_XXX___,
  _______X, ___X____,
};

/** $E44A: glyph 55 bitmap (40 bytes, 10 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_055[40] = {
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
};

/** $E472: glyph 56 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_056[60] = {
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
};

/** $E4AE: glyph 57 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_057[60] = {
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
};

/** $E4EA: glyph 58 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_058[60] = {
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
};

/** $E526: glyph 59 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_059[28] = {
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
};

/** $E542: glyph 60 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_060[28] = {
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
};

/** $E55E: glyph 61 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_061[28] = {
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
};

/** $E57A: glyph 62 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_062[28] = {
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
};

/** $E596: glyph 63 bitmap (8 bytes, 4 row-pairs x 1 width-bytes). */
static const u8 title_glyph_bitmap_063[8] = {
  ___XX___,
  __XXX___,
  __XXX___,
  _XX_X___,
  _XX_X___,
  X_X_X___,
  XXXXX___,
  XX_XX___,
};

/** $E59E: glyph 64 bitmap (8 bytes, 4 row-pairs x 1 width-bytes). */
static const u8 title_glyph_bitmap_064[8] = {
  _____XX_,
  ____XXX_,
  ____XXX_,
  ___XX_X_,
  ___XX_X_,
  __X_X_X_,
  __XXXXX_,
  __XX_XX_,
};

/** $E5A6: glyph 65 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_065[16] = {
  _______X, X_______,
  ______XX, X_______,
  ______XX, X_______,
  _____XX_, X_______,
  _____XX_, X_______,
  ____X_X_, X_______,
  ____XXXX, X_______,
  ____XX_X, X_______,
};

/** $E5B6: glyph 66 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_066[16] = {
  ________, _XX_____,
  ________, XXX_____,
  ________, XXX_____,
  _______X, X_X_____,
  _______X, X_X_____,
  ______X_, X_X_____,
  ______XX, XXX_____,
  ______XX, _XX_____,
};

/** $E5C6: glyph 67 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_067[60] = {
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
};

/** $E602: glyph 68 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_068[60] = {
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
};

/** $E63E: glyph 69 bitmap (80 bytes, 10 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_069[80] = {
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
};

/** $E68E: glyph 70 bitmap (80 bytes, 10 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_070[80] = {
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
};

/** $E6DE: glyph 71 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_071[28] = {
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
};

/** $E6FA: glyph 72 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_072[28] = {
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
};

/** $E716: glyph 73 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_073[28] = {
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
};

/** $E732: glyph 74 bitmap (42 bytes, 7 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_074[42] = {
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
};

/** $E75C: glyph 75 bitmap (8 bytes, 4 row-pairs x 1 width-bytes). */
static const u8 title_glyph_bitmap_075[8] = {
  __XXXX__,
  __XXXXX_,
  _XXX____,
  _XXXXXX_,
  ____X_X_,
  XXX_XX__,
  XXXXXX__,
  _XXXX___,
};

/** $E764: glyph 76 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_076[16] = {
  ____XXXX, ________,
  ____XXXX, X_______,
  ___XXX__, ________,
  ___XXXXX, X_______,
  ______X_, X_______,
  __XXX_XX, ________,
  __XXXXXX, ________,
  ___XXXX_, ________,
};

/** $E774: glyph 77 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_077[16] = {
  ______XX, XX______,
  ______XX, XXX_____,
  _____XXX, ________,
  _____XXX, XXX_____,
  ________, X_X_____,
  ____XXX_, XX______,
  ____XXXX, XX______,
  _____XXX, X_______,
};

/** $E784: glyph 78 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_078[16] = {
  ________, XXXX____,
  ________, XXXXX___,
  _______X, XX______,
  _______X, XXXXX___,
  ________, __X_X___,
  ______XX, X_XX____,
  ______XX, XXXX____,
  _______X, XXX_____,
};

/** $E794: glyph 79 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_079[60] = {
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
};

/** $E7D0: glyph 80 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_080[60] = {
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
};

/** $E80C: glyph 81 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_081[60] = {
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
};

/** $E848: glyph 82 bitmap (60 bytes, 10 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_082[60] = {
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
};

/** $E884: glyph 83 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_083[28] = {
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
};

/** $E8A0: glyph 84 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_084[28] = {
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
};

/** $E8BC: glyph 85 bitmap (28 bytes, 7 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_085[28] = {
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
};

/** $E8D8: glyph 86 bitmap (42 bytes, 7 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_086[42] = {
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
};

/** $E902: glyph 87 bitmap (8 bytes, 4 row-pairs x 1 width-bytes). */
static const u8 title_glyph_bitmap_087[8] = {
  __XXXX__,
  __XXX___,
  _XX_____,
  _X_XX___,
  _XXX____,
  XX______,
  X_XX____,
  XXX_____,
};

/** $E90A: glyph 88 bitmap (8 bytes, 4 row-pairs x 1 width-bytes). */
static const u8 title_glyph_bitmap_088[8] = {
  ____XXXX,
  ____XXX_,
  ___XX___,
  ___X_XX_,
  ___XXX__,
  __XX____,
  __X_XX__,
  __XXX___,
};

/** $E912: glyph 89 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_089[16] = {
  ______XX, XX______,
  ______XX, X_______,
  _____XX_, ________,
  _____X_X, X_______,
  _____XXX, ________,
  ____XX__, ________,
  ____X_XX, ________,
  ____XXX_, ________,
};

/** $E922: glyph 90 bitmap (16 bytes, 4 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_090[16] = {
  ________, XXXX____,
  ________, XXX_____,
  _______X, X_______,
  _______X, _XX_____,
  _______X, XX______,
  ______XX, ________,
  ______X_, XX______,
  ______XX, X_______,
};

/** $E932: glyph 91 bitmap (72 bytes, 12 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_091[72] = {
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
};

/** $E97A: glyph 92 bitmap (72 bytes, 12 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_092[72] = {
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
};

/** $E9C2: glyph 93 bitmap (72 bytes, 12 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_093[72] = {
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
};

/** $EA0A: glyph 94 bitmap (96 bytes, 12 row-pairs x 4 width-bytes). */
static const u8 title_glyph_bitmap_094[96] = {
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
};

/** $EA6A: glyph 95 bitmap (32 bytes, 8 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_095[32] = {
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
};

/** $EA8A: glyph 96 bitmap (32 bytes, 8 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_096[32] = {
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
};

/** $EAAA: glyph 97 bitmap (48 bytes, 8 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_097[48] = {
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
};

/** $EADA: glyph 98 bitmap (48 bytes, 8 row-pairs x 3 width-bytes). */
static const u8 title_glyph_bitmap_098[48] = {
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
};

/** $EB0A: glyph 99 bitmap (10 bytes, 5 row-pairs x 1 width-bytes). */
static const u8 title_glyph_bitmap_099[10] = {
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
};

/** $EB14: glyph 100 bitmap (20 bytes, 5 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_100[20] = {
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
};

/** $EB28: glyph 101 bitmap (20 bytes, 5 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_101[20] = {
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
};

/** $EB3C: glyph 102 bitmap (20 bytes, 5 row-pairs x 2 width-bytes). */
static const u8 title_glyph_bitmap_102[20] = {
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
};

/** $EB50: glyph 103 bitmap (6 bytes, 3 row-pairs x 7 width-bytes). */
static const u8 title_glyph_bitmap_103[6] = {
  _XXXXX__,
  X_XX__X_,
  XXXXX_X_,
  XXXXXXX_,
  XX_X_XX_,
  _XXXXX__,
};

/** $EB56: glyph 104 bitmap (12 bytes, 3 row-pairs x 6 width-bytes). */
static const u8 title_glyph_bitmap_104[12] = {
  ___XXXXX, ________,
  __X_XX__, X_______,
  __XXXXX_, X_______,
  __XXXXXX, X_______,
  __XX_X_X, X_______,
  ___XXXXX, ________,
};

/** $EB62: glyph 105 bitmap (12 bytes, 3 row-pairs x 6 width-bytes). */
static const u8 title_glyph_bitmap_105[12] = {
  _____XXX, XX______,
  ____X_XX, __X_____,
  ____XXXX, X_X_____,
  ____XXXX, XXX_____,
  ____XX_X, _XX_____,
  _____XXX, XX______,
};

/** $EB6E: glyph 106 bitmap (12 bytes, 3 row-pairs x 6 width-bytes). */
static const u8 title_glyph_bitmap_106[12] = {
  _______X, XXXX____,
  ______X_, XX__X___,
  ______XX, XXX_X___,
  ______XX, XXXXX___,
  ______XX, _X_XX___,
  _______X, XXXX____,
};

/** $EB7A: glyph 107 bitmap (4 bytes, 2 row-pairs x 7 width-bytes). */
static const u8 title_glyph_bitmap_107[4] = {
  _XXXX___,
  X_X_X___,
  XXXXX___,
  _XXX____,
};

/** $EB7E: glyph 108 bitmap (4 bytes, 2 row-pairs x 7 width-bytes). */
static const u8 title_glyph_bitmap_108[4] = {
  ___XXXX_,
  __X_X_X_,
  __XXXXX_,
  ___XXX__,
};

/** $EB82: glyph 109 bitmap (8 bytes, 2 row-pairs x 6 width-bytes). */
static const u8 title_glyph_bitmap_109[8] = {
  _____XXX, X_______,
  ____X_X_, X_______,
  ____XXXX, X_______,
  _____XXX, ________,
};

/** $EB8A: glyph 110 bitmap (8 bytes, 2 row-pairs x 6 width-bytes). */
static const u8 title_glyph_bitmap_110[8] = {
  _______X, XXX_____,
  ______X_, X_X_____,
  ______XX, XXX_____,
  _______X, XX______,
};

/** $EB92: glyph 111 bitmap (2 bytes, 1 row-pairs x 7 width-bytes). */
static const u8 title_glyph_bitmap_111[2] = {
  XXX_____,
  XX______,
};

/** $EB94: glyph 112 bitmap (2 bytes, 1 row-pairs x 7 width-bytes). */
static const u8 title_glyph_bitmap_112[2] = {
  __XXX___,
  __XX____,
};

/** $EB96: glyph 113 bitmap (4 bytes, 1 row-pairs x 6 width-bytes). */
static const u8 title_glyph_bitmap_113[4] = {
  ____XXX_, ________,
  ____XX__, ________,
};

/** $EB9A: glyph 114 bitmap (4 bytes, 1 row-pairs x 6 width-bytes). */
static const u8 title_glyph_bitmap_114[4] = {
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
static const tune_t tunes[4] = {
  { 2, { TITLE_TUNE0_DATA_ADDR, 0xF25A, 0xF265 } },
  { 4, { TITLE_TUNE1_DATA_ADDR, 0xF605, 0xF609 } },
  { 2, { 0xF666, 0xF66A, 0xF66E } },
  { 3, { 0xF6F4, 0xF6F8, 0xF6FE } }
};

/**
 * $F241-$F642: title_tune0_data
 *
 * 128K bank 3: title-tune engine raw pattern-data region for tune 0 (title
 * screen), transcribed byte-exact from bank3.bin, $F241-$F600 (the byte
 * immediately preceding tune 1's own region). Each channel's block within
 * this region holds a 2-byte header pointer, its phrase-pointer table, and
 * its note/command byte stream, all as one contiguous run of real Z80
 * memory -- see advance_channel_phrase ($F1AE@bank3) for how the header and
 * table are walked. Addresses read from the header or phrase table are
 * resolved to a C pointer into this array via simple offset arithmetic from
 * $F241 (see resolve_phrase_addr below). Tunes 2 and 3 are not extracted
 * (unreachable from the code paths wired up so far).
 */
static const u8 title_tune0_data[1026] = {
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

/**
 * $F601-$F6DE: title_tune1_data
 *
 * As title_tune0_data, for tune 1 (perp-caught success jingle) -- covers
 * channel 3's wraparound pattern prefix (156 bytes, see tune_pattern_lens
 * in start_tune) in full, the deepest of the three channels' reach into
 * this region. Tune 2 begins at $F666, inside this range; the rest of tune 2
 * and all of tune 3 are extracted separately as title_tune23_data below.
 */
static const u8 title_tune1_data[222] = {
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
 * $F6DF-$F7A9: title_tune23_data
 *
 * As title_tune0_data/title_tune1_data, for tunes 2 and 3 (both used by the
 * high-score name-entry screen -- tune 3 in name_entry_setup_screen, tune 2
 * in hiscore_finalise). Tune 2's channel headers ($F666/$F66A/$F66E) fall inside
 * title_tune1_data above; this array covers the rest of the opaque
 * $F225-$F7A9 data block, up to $F7A9 -- the last byte before code resumes
 * at $F7AA (setup_im2_interrupt_table).
 *
 * Traced byte-by-byte from bank3.bin by walking each channel's note/command
 * stream and phrase-pointer table (see advance_channel_pattern/
 * advance_channel_phrase): every channel reaches either PCMD_END_OF_TUNE or
 * a genuine PHRASE_TABLE_RESET (loop back to its own header word) without
 * leaving the $F666-$F7A9 range, so this extraction is complete -- no
 * cross-tune jump lands outside it.
 */
static const u8 title_tune23_data[203] = {
  0x20, /* $F6DF: NOTE_FS3 [tune2ch1] */
  0x15, /* $F6E0: NOTE_G2 [tune2ch1] */
  0x21, /* $F6E1: NOTE_G3 [tune2ch1] */
  0x12, /* $F6E2: NOTE_E2 [tune2ch1] */
  0x1E, /* $F6E3: NOTE_E3 [tune2ch1] */
  0x15, /* $F6E4: NOTE_G2 [tune2ch1] */
  0x21, /* $F6E5: NOTE_G3 [tune2ch1] */
  0x14, /* $F6E6: NOTE_FS2 [tune2ch1] */
  0x20, /* $F6E7: NOTE_FS3 [tune2ch1] */
  0x80, /* $F6E8: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune2ch1] */
  0x80, /* $F6E9: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune2ch1] */
  0x8E, /* $F6EA: PCMD_END_OF_TUNE [tune2ch1] */
  0xFF, /* $F6EB: PCMD_SET_ROW_WAIT(32) [tune2ch2] */
  0x80, /* $F6EC: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune2ch2] */
  0x80, /* $F6ED: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune2ch2] */
  0x80, /* $F6EE: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune2ch2] */
  0x80, /* $F6EF: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune2ch2] */
  0x80, /* $F6F0: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune2ch2] */
  0x80, /* $F6F1: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune2ch2] */
  0x80, /* $F6F2: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune2ch2] */
  0x8E, /* $F6F3: PCMD_END_OF_TUNE [tune2ch2] */
  0x02, /* $F6F4: HEADER_PATTERN_PTR [tune3ch0] */
  0xF7, /* $F6F5: (high byte) */
  0x00, /* $F6F6: PHRASE_TABLE_WORD */
  0x00, /* $F6F7: (high byte) */
  0x43, /* $F6F8: HEADER_PATTERN_PTR [tune3ch1] */
  0xF7, /* $F6F9: (high byte) */
  0x6E, /* $F6FA: PHRASE_TABLE_WORD */
  0xF7, /* $F6FB: (high byte) */
  0x00, /* $F6FC: PHRASE_TABLE_WORD */
  0x00, /* $F6FD: (high byte) */
  0x9A, /* $F6FE: HEADER_PATTERN_PTR [tune3ch2] */
  0xF7, /* $F6FF: (high byte) */
  0x00, /* $F700: PHRASE_TABLE_WORD */
  0x00, /* $F701: (high byte) */
  0x8A, /* $F702: PCMD_SET_MIXER_BITS_HIGH3 [tune3ch0] */
  0xD0, /* $F703: PCMD_SELECT_ENVELOPE_SHAPE(0) [tune3ch0] */
  0x91, /* $F704: PCMD_UNMUTE_CHANNEL [tune3ch0] */
  0xB9, /* $F705: PCMD_SELECT_PITCH_OFFSET(1) [tune3ch0] */
  0x88, /* $F706: PCMD_SET_ENVELOPE_PARAMS [tune3ch0] */
  0x02, /* $F707: PCMD_SET_ENVELOPE_PARAMS_OPERAND [tune3ch0] */
  0x02, /* $F708: PCMD_SET_ENVELOPE_PARAMS_OPERAND [tune3ch0] */
  0x82, /* $F709: PCMD_VIBRATO_ON [tune3ch0] */
  0xE3, /* $F70A: PCMD_SET_ROW_WAIT(4) [tune3ch0] */
  0x0C, /* $F70B: NOTE_AS1 [tune3ch0] */
  0x13, /* $F70C: NOTE_F2 [tune3ch0] */
  0x11, /* $F70D: NOTE_DS2 [tune3ch0] */
  0x13, /* $F70E: NOTE_F2 [tune3ch0] */
  0x0C, /* $F70F: NOTE_AS1 [tune3ch0] */
  0xE1, /* $F710: PCMD_SET_ROW_WAIT(2) [tune3ch0] */
  0x13, /* $F711: NOTE_F2 [tune3ch0] */
  0xE3, /* $F712: PCMD_SET_ROW_WAIT(4) [tune3ch0] */
  0x11, /* $F713: NOTE_DS2 [tune3ch0] */
  0xE1, /* $F714: PCMD_SET_ROW_WAIT(2) [tune3ch0] */
  0x11, /* $F715: NOTE_DS2 [tune3ch0] */
  0xE3, /* $F716: PCMD_SET_ROW_WAIT(4) [tune3ch0] */
  0x13, /* $F717: NOTE_F2 [tune3ch0] */
  0xE3, /* $F718: PCMD_SET_ROW_WAIT(4) [tune3ch0] */
  0x0C, /* $F719: NOTE_AS1 [tune3ch0] */
  0x13, /* $F71A: NOTE_F2 [tune3ch0] */
  0x11, /* $F71B: NOTE_DS2 [tune3ch0] */
  0x13, /* $F71C: NOTE_F2 [tune3ch0] */
  0x0C, /* $F71D: NOTE_AS1 [tune3ch0] */
  0xE1, /* $F71E: PCMD_SET_ROW_WAIT(2) [tune3ch0] */
  0x13, /* $F71F: NOTE_F2 [tune3ch0] */
  0xE3, /* $F720: PCMD_SET_ROW_WAIT(4) [tune3ch0] */
  0x11, /* $F721: NOTE_DS2 [tune3ch0] */
  0xE1, /* $F722: PCMD_SET_ROW_WAIT(2) [tune3ch0] */
  0x11, /* $F723: NOTE_DS2 [tune3ch0] */
  0xE3, /* $F724: PCMD_SET_ROW_WAIT(4) [tune3ch0] */
  0x13, /* $F725: NOTE_F2 [tune3ch0] */
  0xE3, /* $F726: PCMD_SET_ROW_WAIT(4) [tune3ch0] */
  0x0C, /* $F727: NOTE_AS1 [tune3ch0] */
  0x13, /* $F728: NOTE_F2 [tune3ch0] */
  0x11, /* $F729: NOTE_DS2 [tune3ch0] */
  0x13, /* $F72A: NOTE_F2 [tune3ch0] */
  0x0C, /* $F72B: NOTE_AS1 [tune3ch0] */
  0xE1, /* $F72C: PCMD_SET_ROW_WAIT(2) [tune3ch0] */
  0x13, /* $F72D: NOTE_F2 [tune3ch0] */
  0xE3, /* $F72E: PCMD_SET_ROW_WAIT(4) [tune3ch0] */
  0x11, /* $F72F: NOTE_DS2 [tune3ch0] */
  0xE1, /* $F730: PCMD_SET_ROW_WAIT(2) [tune3ch0] */
  0x11, /* $F731: NOTE_DS2 [tune3ch0] */
  0xE3, /* $F732: PCMD_SET_ROW_WAIT(4) [tune3ch0] */
  0x13, /* $F733: NOTE_F2 [tune3ch0] */
  0xE3, /* $F734: PCMD_SET_ROW_WAIT(4) [tune3ch0] */
  0x0C, /* $F735: NOTE_AS1 [tune3ch0] */
  0x13, /* $F736: NOTE_F2 [tune3ch0] */
  0x11, /* $F737: NOTE_DS2 [tune3ch0] */
  0x13, /* $F738: NOTE_F2 [tune3ch0] */
  0x0C, /* $F739: NOTE_AS1 [tune3ch0] */
  0xE1, /* $F73A: PCMD_SET_ROW_WAIT(2) [tune3ch0] */
  0x13, /* $F73B: NOTE_F2 [tune3ch0] */
  0x16, /* $F73C: NOTE_GS2 [tune3ch0] */
  0xE3, /* $F73D: PCMD_SET_ROW_WAIT(4) [tune3ch0] */
  0x18, /* $F73E: NOTE_AS2 [tune3ch0] */
  0xE1, /* $F73F: PCMD_SET_ROW_WAIT(2) [tune3ch0] */
  0x13, /* $F740: NOTE_F2 [tune3ch0] */
  0x11, /* $F741: NOTE_DS2 [tune3ch0] */
  0x87, /* $F742: PCMD_ADVANCE_PHRASE [tune3ch0] */
  0x8A, /* $F743: PCMD_SET_MIXER_BITS_HIGH3 [tune3ch1] */
  0x91, /* $F744: PCMD_UNMUTE_CHANNEL [tune3ch1] */
  0xD2, /* $F745: PCMD_SELECT_ENVELOPE_SHAPE(2) [tune3ch1] */
  0x81, /* $F746: PCMD_VIBRATO_OFF [tune3ch1] */
  0xC5, /* $F747: PCMD_SELECT_PITCH_OFFSET(13) [tune3ch1] */
  0xE3, /* $F748: PCMD_SET_ROW_WAIT(4) [tune3ch1] */
  0x80, /* $F749: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0xE1, /* $F74A: PCMD_SET_ROW_WAIT(2) [tune3ch1] */
  0x30, /* $F74B: NOTE_AS4 [tune3ch1] */
  0x30, /* $F74C: NOTE_AS4 [tune3ch1] */
  0x80, /* $F74D: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0xE3, /* $F74E: PCMD_SET_ROW_WAIT(4) [tune3ch1] */
  0x30, /* $F74F: NOTE_AS4 [tune3ch1] */
  0xE1, /* $F750: PCMD_SET_ROW_WAIT(2) [tune3ch1] */
  0x30, /* $F751: NOTE_AS4 [tune3ch1] */
  0xE1, /* $F752: PCMD_SET_ROW_WAIT(2) [tune3ch1] */
  0x80, /* $F753: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0x30, /* $F754: NOTE_AS4 [tune3ch1] */
  0xE3, /* $F755: PCMD_SET_ROW_WAIT(4) [tune3ch1] */
  0x30, /* $F756: NOTE_AS4 [tune3ch1] */
  0x80, /* $F757: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0xE1, /* $F758: PCMD_SET_ROW_WAIT(2) [tune3ch1] */
  0x30, /* $F759: NOTE_AS4 [tune3ch1] */
  0x30, /* $F75A: NOTE_AS4 [tune3ch1] */
  0x30, /* $F75B: NOTE_AS4 [tune3ch1] */
  0x80, /* $F75C: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0x30, /* $F75D: NOTE_AS4 [tune3ch1] */
  0x80, /* $F75E: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0x80, /* $F75F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0x30, /* $F760: NOTE_AS4 [tune3ch1] */
  0x30, /* $F761: NOTE_AS4 [tune3ch1] */
  0x30, /* $F762: NOTE_AS4 [tune3ch1] */
  0xE1, /* $F763: PCMD_SET_ROW_WAIT(2) [tune3ch1] */
  0x80, /* $F764: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0x30, /* $F765: NOTE_AS4 [tune3ch1] */
  0x30, /* $F766: NOTE_AS4 [tune3ch1] */
  0x30, /* $F767: NOTE_AS4 [tune3ch1] */
  0xE3, /* $F768: PCMD_SET_ROW_WAIT(4) [tune3ch1] */
  0x80, /* $F769: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0xE1, /* $F76A: PCMD_SET_ROW_WAIT(2) [tune3ch1] */
  0x30, /* $F76B: NOTE_AS4 [tune3ch1] */
  0x30, /* $F76C: NOTE_AS4 [tune3ch1] */
  0x87, /* $F76D: PCMD_ADVANCE_PHRASE [tune3ch1] */
  0x8A, /* $F76E: PCMD_SET_MIXER_BITS_HIGH3 [tune3ch1] */
  0x91, /* $F76F: PCMD_UNMUTE_CHANNEL [tune3ch1] */
  0xD6, /* $F770: PCMD_SELECT_ENVELOPE_SHAPE(6) [tune3ch1] */
  0x81, /* $F771: PCMD_VIBRATO_OFF [tune3ch1] */
  0xC6, /* $F772: PCMD_SELECT_PITCH_OFFSET(14) [tune3ch1] */
  0xE3, /* $F773: PCMD_SET_ROW_WAIT(4) [tune3ch1] */
  0x80, /* $F774: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0xE1, /* $F775: PCMD_SET_ROW_WAIT(2) [tune3ch1] */
  0x30, /* $F776: NOTE_AS4 [tune3ch1] */
  0x30, /* $F777: NOTE_AS4 [tune3ch1] */
  0x80, /* $F778: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0xE3, /* $F779: PCMD_SET_ROW_WAIT(4) [tune3ch1] */
  0x30, /* $F77A: NOTE_AS4 [tune3ch1] */
  0xE1, /* $F77B: PCMD_SET_ROW_WAIT(2) [tune3ch1] */
  0x30, /* $F77C: NOTE_AS4 [tune3ch1] */
  0xE1, /* $F77D: PCMD_SET_ROW_WAIT(2) [tune3ch1] */
  0x80, /* $F77E: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0x30, /* $F77F: NOTE_AS4 [tune3ch1] */
  0xE3, /* $F780: PCMD_SET_ROW_WAIT(4) [tune3ch1] */
  0x30, /* $F781: NOTE_AS4 [tune3ch1] */
  0x80, /* $F782: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0xE1, /* $F783: PCMD_SET_ROW_WAIT(2) [tune3ch1] */
  0x30, /* $F784: NOTE_AS4 [tune3ch1] */
  0x30, /* $F785: NOTE_AS4 [tune3ch1] */
  0xC7, /* $F786: PCMD_SELECT_PITCH_OFFSET(15) [tune3ch1] */
  0x30, /* $F787: NOTE_AS4 [tune3ch1] */
  0x80, /* $F788: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0x30, /* $F789: NOTE_AS4 [tune3ch1] */
  0x80, /* $F78A: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0x80, /* $F78B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0x30, /* $F78C: NOTE_AS4 [tune3ch1] */
  0x30, /* $F78D: NOTE_AS4 [tune3ch1] */
  0x30, /* $F78E: NOTE_AS4 [tune3ch1] */
  0xE1, /* $F78F: PCMD_SET_ROW_WAIT(2) [tune3ch1] */
  0x80, /* $F790: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0x30, /* $F791: NOTE_AS4 [tune3ch1] */
  0x30, /* $F792: NOTE_AS4 [tune3ch1] */
  0x30, /* $F793: NOTE_AS4 [tune3ch1] */
  0xE3, /* $F794: PCMD_SET_ROW_WAIT(4) [tune3ch1] */
  0x80, /* $F795: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch1] */
  0xE1, /* $F796: PCMD_SET_ROW_WAIT(2) [tune3ch1] */
  0x3C, /* $F797: NOTE_AS5 [tune3ch1] */
  0x30, /* $F798: NOTE_AS4 [tune3ch1] */
  0x87, /* $F799: PCMD_ADVANCE_PHRASE [tune3ch1] */
  0x8A, /* $F79A: PCMD_SET_MIXER_BITS_HIGH3 [tune3ch2] */
  0xD7, /* $F79B: PCMD_SELECT_ENVELOPE_SHAPE(7) [tune3ch2] */
  0x81, /* $F79C: PCMD_VIBRATO_OFF [tune3ch2] */
  0x91, /* $F79D: PCMD_UNMUTE_CHANNEL [tune3ch2] */
  0xC8, /* $F79E: PCMD_SELECT_PITCH_OFFSET(16) [tune3ch2] */
  0xFF, /* $F79F: PCMD_SET_ROW_WAIT(32) [tune3ch2] */
  0x48, /* $F7A0: NOTE_AS6 [tune3ch2] */
  0x8F, /* $F7A1: PCMD_RESET_ROW_COUNTER [tune3ch2] */
  0x80, /* $F7A2: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch2] */
  0x80, /* $F7A3: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch2] */
  0xFF, /* $F7A4: PCMD_SET_ROW_WAIT(32) [tune3ch2] */
  0x3C, /* $F7A5: NOTE_AS5 [tune3ch2] */
  0x8F, /* $F7A6: PCMD_RESET_ROW_COUNTER [tune3ch2] */
  0x80, /* $F7A7: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch2] */
  0x80, /* $F7A8: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune3ch2] */
  0x87, /* $F7A9: PCMD_ADVANCE_PHRASE [tune3ch2] */
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
static const u8 options_menu_text[366] = {
  // $FC29 (offset 0): control-select screen
  attribute_RED_OVER_BLACK,
  ZXSCREEN(0x484A),
  'E', 'N', 'T', 'E', 'R', ' ', 'O', 'P', 'T', 'I', 'O', 'N' | EOS,
  attribute_CYAN_OVER_BLACK,
  ZXSCREEN(0x48C6),
  '1', '.', ' ', 'S', 'I', 'N', 'C', 'L', 'A', 'I', 'R', ' ', 'J', 'O', 'Y', 'S', 'T', 'I', 'C', 'K' | EOS,
  attribute_CYAN_OVER_BLACK,
  ZXSCREEN(0x5006),
  '2', '.', ' ', 'C', 'U', 'R', 'S', 'O', 'R', ' ', 'J', 'O', 'Y', 'S', 'T', 'I', 'C', 'K' | EOS,
  attribute_CYAN_OVER_BLACK,
  ZXSCREEN(0x5046),
  '3', '.', ' ', 'K', 'E', 'M', 'P', 'S', 'T', 'O', 'N', ' ', 'J', 'O', 'Y', 'S', 'T', 'I', 'C', 'K' | EOS,
  attribute_CYAN_OVER_BLACK,
  ZXSCREEN(0x5086),
  '4', '.', ' ', 'K', 'E', 'Y', 'B', 'O', 'A', 'R', 'D' | EOS,
  attribute_CYAN_OVER_BLACK,
  ZXSCREEN(0x50C6),
  '5', '.', ' ', 'D', 'E', 'F', 'I', 'N', 'E', ' ', 'K', 'E', 'Y', 'S' | EOS,
  0, // terminator ($FC9A)

  // $FC9B (offset 114): key-redefinition screen, header + first 3 labels
  attribute_RED_OVER_BLACK,
  ZXSCREEN(0x4849),
  'R', 'E', 'D', 'E', 'F', 'I', 'N', 'E', ' ', ' ', 'K', 'E', 'Y', 'S' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_YELLOW_OVER_BLACK),  // was 0xC6
  ZXSCREEN(0x48C9),
  'G', 'E', 'A', 'R' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_YELLOW_OVER_BLACK),
  ZXSCREEN(0x48E9),
  'A', 'C', 'C', 'E', 'L', 'E', 'R', 'A', 'T', 'E' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_YELLOW_OVER_BLACK),
  ZXSCREEN(0x5009),
  'B', 'R', 'A', 'K', 'E' | EOS,
  0, // terminator ($FCC8)

  // $FCC9 (offset 160): key-redefinition screen, remaining 5 labels
  (SINGLE_HEIGHT | attribute_BRIGHT_YELLOW_OVER_BLACK),
  ZXSCREEN(0x5029),
  'L', 'E', 'F', 'T' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_YELLOW_OVER_BLACK),
  ZXSCREEN(0x5049),
  'R', 'I', 'G', 'H', 'T' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_GREEN_OVER_BLACK),  // was 0xC4
  ZXSCREEN(0x5089),
  'Q', 'U', 'I', 'T' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_GREEN_OVER_BLACK),
  ZXSCREEN(0x50A9),
  'P', 'A', 'U', 'S', 'E' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_GREEN_OVER_BLACK),
  ZXSCREEN(0x50C9),
  'T', 'U', 'R', 'B', 'O' | EOS,
  0, // terminator ($FCEF)

  // $FCF0 (offset 199): hidden test-mode screen
  (SINGLE_HEIGHT | attribute_BRIGHT_BLUE_OVER_BLACK),  // was 0xC1
  ZXSCREEN(0x4000),
  'T', 'E', 'S', 'T' | EOS,
  attribute_RED_OVER_BLACK,
  ZXSCREEN(0x4826),
  'C', 'H', 'A', 'S', 'E', ' ', 'H', '.', 'Q', '.', ' ', 'T', 'E', 'S', 'T', ' ', 'M', 'O', 'D', 'E' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_CYAN_OVER_BLACK),  // was 0xC5
  ZXSCREEN(0x48A2),
  'T', 'I', 'T', 'L', 'E', ' ', 'S', 'C', 'R', 'E', 'E', 'N' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_MAGENTA_OVER_BLACK),  // was 0xC3
  ZXSCREEN(0x48E2),
  '1', ' ', 'T', 'O', ' ', '5', '.', ' ', 'L', 'O', 'G', 'O', ' ', 'A', 'N', 'I', 'M', 'A', 'T', 'I', 'O', 'N' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_MAGENTA_OVER_BLACK),
  ZXSCREEN(0x5007),
  '6', '.', ' ', 'S', 'C', 'O', 'R', 'E', ' ', 'E', 'N', 'T', 'R', 'Y' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_CYAN_OVER_BLACK),
  ZXSCREEN(0x5042),
  'I', 'N', ' ', 'G', 'A', 'M', 'E' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_GREEN_OVER_BLACK),
  ZXSCREEN(0x5087),
  '1', '.', ' ', 'R', 'E', 'S', 'T', 'A', 'R', 'T', ' ', 'L', 'E', 'V', 'E', 'L' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_GREEN_OVER_BLACK),
  ZXSCREEN(0x50A7),
  '2', '.', ' ', 'N', 'E', 'X', 'T', ' ', 'L', 'E', 'V', 'E', 'L' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_GREEN_OVER_BLACK),
  ZXSCREEN(0x50C7),
  '3', '.', ' ', 'E', 'N', 'D', ' ', 'S', 'C', 'R', 'E', 'E', 'N' | EOS,
  (SINGLE_HEIGHT | attribute_BRIGHT_GREEN_OVER_BLACK),
  ZXSCREEN(0x50E7),
  '4', '.', ' ', 'E', 'X', 'T', 'R', 'A', ' ', 'C', 'R', 'E', 'D', 'I', 'T' | EOS,
  0 // terminator / pad byte ($FD96)
};
// clang-format on

/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */

/* 128K bank 3: $C403-$C52C, the 10 static rank-suffix strings printed beside
 * each high-score row (fixed to screen position, never shifted -- see
 * high_score_row_t's own comment in Bank3State.h). Not consumed yet: the
 * high-score screen's rendering is not translated (see
 * insert_high_score_entry's Conv note below); kept here for when it is. */
static const u8 high_score_rank_suffixes[HIGH_SCORE_TABLE_ROWS][5] = {
  "1ST  ", "2ND  ", "3RD  ", "4TH  ", "5TH  ",
  "6TH  ", "7TH  ", "8TH  ", "9TH  ", "10TH ",
};

/* ----------------------------------------------------------------------- */

/* 180 frames, ~3.6s at 50Hz. This is the Z80's own $C627-$C637 tune-4 wait
 * count in titlescr_wait_loop; play_success_music borrows it as a heuristic
 * bound for a loop the Z80 never exits (see its Conv: note). */
#define ATTRACT_TUNE_WAIT_FRAMES (0xB4)

/* titlescr_wait_loop / run_title_screen outcomes. TITLESCR_START_GAME and
 * TITLESCR_ATTRACT are the value the Z80 leaves in A when the title screen
 * hands back to attract_mode_128k ($F421 AND A / RET Z); TITLESCR_RESTART is
 * internal to run_title_screen, standing in for the Z80's `JP $C59E`. */
#define TITLESCR_START_GAME (0) /* credit inserted: leave attract mode entirely */
#define TITLESCR_ATTRACT    (1) /* tune-4 wait tail: run the attract demo */
#define TITLESCR_RESTART    (2) /* re-run the title screen with a new scene */

#define ADDRTOSCREEN(addr) z80addrtoscreen(state, addr)
#define ADDRTOATTRS(addr)  z80addrtoattrs(state, addr)

/* ----------------------------------------------------------------------- */

/**
 * Destination address and glyph-table lookup result shared by
 * compute_glyph_blit_params_fg and compute_glyph_blit_params_bg ($C8C5, $C94F).
 *
 * The address bytes are named H/L but their comments say "(was D)"/"(was E)".
 * That is not a typo: compute_glyph_geometry builds the address in DE, and the
 * Z80 hands it over to the blit dispatcher in HL. The field name is the
 * register the value occupies at the point it is consumed; the "(was X)"
 * comment is the register it was computed in.
 */
typedef struct glyph_blit_geometry
{
  int       H;              /* destination screen address high byte (was D) */
  int       L;              /* destination screen address low byte (was E) */
  const u8 *HL_src;         /* glyph bitmap pointer (was HL) */
  int       B_height_pairs; /* scanline-pairs remaining to draw (was B) */
  int       C_width_select; /* width selector, 1-7 (was C) */
  int       carry_initial;  /* true: Y was within range, nothing to skip (was Carry) */
  u8        A_excess;       /* Y clamp excess; valid only when !carry_initial (was A') */
}
glyph_blit_geometry_t;

/* ----------------------------------------------------------------------- */

static u8 run_title_screen(chqstate_t *state);
static u8 titlescr_wait_loop(chqstate_t *state);
static void titlescr_credit_inserted(chqstate_t *state);
static void titlescr_refresh_name_table(chqstate_t *state);
static u8 titlescr_animate_frame(chqstate_t *state);
static u8 object_script_step(chqstate_t *state);
static void oss_op_velocity(struct title_object *rec);
static void oss_apply_x_step(struct title_object *rec);
static void oss_apply_y_step(struct title_object *rec);
static void oss_op_decel_x(struct title_object *rec);
static u8 oss_lookup_speed(u8 C_idx);
static void oss_op_decel_y(struct title_object *rec);
static void oss_op_accel_x_a(struct title_object *rec);
static void oss_op_accel_x_b(struct title_object *rec);
static void oss_op_accel_x_c(struct title_object *rec);
static void clear_playfield_and_attrs(chqstate_t *state);
static void clear_and_fill_border_attrs(chqstate_t *state);
static void compute_glyph_blit_params_fg(chqstate_t *state,
                                         u8          B_y,
                                         u8          C_x,
                                         u8          L_row);
static void compute_glyph_geometry(u8                     B_y,
                                   u8                     C_x,
                                   u8                     L_row,
                                   glyph_blit_geometry_t *out);
static void blit_masked_sprite_dispatch_fg(chqstate_t *state,
                                           int         H,
                                           int         L,
                                           const u8   *src,
                                           int         B_height_pairs,
                                           int         C_width_select);
static void compute_glyph_blit_params_bg(chqstate_t *state,
                                         u8          B_y,
                                         u8          C_x,
                                         u8          L_row);
static void blit_masked_sprite_dispatch_bg(chqstate_t *state,
                                           int         H,
                                           int         L,
                                           const u8   *src,
                                           int         B_height_pairs,
                                           int         C_width_select);
static void blit_glyph_rows(chqstate_t *state,
                            int         H,
                            int         L,
                            const u8   *src,
                            int         B_height_pairs,
                            int         row_bytes);
typedef void blit_width_fn(chqstate_t *state,
                           int         H,
                           int         L,
                           const u8   *src,
                           int         B_height_pairs);
static blit_width_fn blit_width1;
static void advance_glyph_scanline(int *H, int *L);
static blit_width_fn blit_width2, blit_width3, blit_width4, blit_width5, blit_width6, blit_width7;
static void clear_playfield_buffer(chqstate_t *state);
static void titlescr_start_ay(chqstate_t *state, u8 A_tune);
static void titlescr_ay_music(chqstate_t *state);
static void titlescr_write_ay_registers(chqstate_t *state);
static void titlescr_silence_ay(chqstate_t *state);
static void stop_music_and_silence(chqstate_t *state);
static u8 acp_read_byte(title_tune_channel_t *IX_channel,
                        const u8            **DE_pattern);
static const u8 *resolve_phrase_addr(u16 addr);
static u16 compute_channel_ay_registers(chqstate_t           *state,
                                        title_tune_channel_t *IX_channel,
                                        u8                   *A_volume_out);
static void advance_channel_phrase(chqstate_t           *state,
                                   title_tune_channel_t *IX_channel,
                                   const u8            **DE_pattern);
static u8 advance_channel_pattern(chqstate_t           *state,
                                  title_tune_channel_t *IX_channel);
static void setup_im2_interrupt_table(chqstate_t *state);
static void check_high_score(chqstate_t *state);
static void insert_high_score_entry(chqstate_t *state, int row);
static void name_entry_setup_screen(chqstate_t *state);
static void ihe_flash_loop(chqstate_t *state);
static void name_entry_frame(chqstate_t *state);
static void name_entry_input(chqstate_t *state);
static void name_entry_dispatch(chqstate_t *state, u8 A_input);
static void hiscore_finalise(chqstate_t *state);
static void cycle_and_draw_letter(chqstate_t *state, u8 C_input_bits);
static void hiscore_draw_glyph(chqstate_t *state, u8 D_screen, u8 E_screen);
static void scroll_score_rows(chqstate_t *state);
static void blink_hiscore_row(chqstate_t *state, int do_toggle);
static void draw_score_row_fields(chqstate_t             *state,
                                  u8                      D,
                                  u8                      E,
                                  u8                      row,
                                  const high_score_row_t *entry,
                                  int                     blink,
                                  u8                      attrs);
static void redraw_name_frame(chqstate_t *state, u8 D_screen, u8 E_screen);
static void play_success_music(chqstate_t *state);
static void titlescr_start_tune(chqstate_t *state, u8 tune_no);
static void load_drum_script(chqstate_t *state, u8 A_tune);
static const u8 *resolve_drum_script_addr(u16 addr);
static void load_drum_op(chqstate_t *state, const u8 *HL);
static void titlescr_drum_advance(chqstate_t *state);
static void titlescr_music(chqstate_t *state);
static void frame_interrupt_handler(chqstate_t *state);
static void play_fixed_sample_1(chqstate_t *state, int A_pitch_param);
static void play_fixed_sample_2(chqstate_t *state, int A_pitch_param);
static void play_fixed_sample_start(chqstate_t *state,
                                    int         A_pitch_param,
                                    u8         *HL_data,
                                    int         D_length);
static void play_sample_row(chqstate_t *state, int D_length, u8 *HL_data);
static void finish_sample_playback(chqstate_t *state);
static void play_drum_noise_burst(chqstate_t *state, int E_pitch_param);
static u8 options_menu_driver(chqstate_t *state);
static u8 omd_redraw_and_poll(chqstate_t *state);
static void run_title_tune(chqstate_t *state);
static u8 detect_kempston_joystick(chqstate_t *state);
static void print_string(chqstate_t *state, const u8 *HL_string);
static const u8 *print_character(chqstate_t *state, const u8 *HL_record);
static void clear_options_screen(chqstate_t *state);
static void redefine_keys_screen(chqstate_t *state);
static u8 scan_keyboard_matrix(chqstate_t *state, u8 *D_key_code_out);
static void read_new_key_definition(chqstate_t *state,
                                    u16        *DE_screen,
                                    u8          B_remaining,
                                    u8          C_control_index);
static u16 advance_key_label_column(u16 DE_screen);

/* ----------------------------------------------------------------------- */

/* Single-height BRIGHT WHITE on BLACK -- $C0F4 ("LD (HL),$47 / LDIR") fills
 * the whole table attribute area with this colour before any row scrolls
 * in, contrasting with the flashing yellow headers (name_entry_screen_text's
 * 0xC6 records). Not 0x46 -- that byte belongs to a different routine
 * ($C17B, the selector-cell highlight), not the table rows. */
#define TABLE_ROW_COLOUR (attribute_BRIGHT_WHITE_OVER_BLACK)

/* $C58C/$C58B: the selector row pointer's low byte, as copied from the
 * $C580 template by check_high_score, and the value it must reach to
 * force-finalise via the idle timeout (see name_entry_dispatch). Also the
 * attribute column range of the "BEST OFFICERS" row's slow highlight sweep
 * (see name_entry_dispatch): $590A-$5917, one column per full pass of the
 * fast selector below. */
#define HISCORE_CURSOR_ADDR_INIT      10
#define HISCORE_CURSOR_ADDR_FINALISE  23

/* $5967: attribute address of the first cell of "ENTER YOUR INITIALS"'
 * 20-character row -- shared by name_entry_dispatch's fast selector chase
 * (see hiscore.blink_offset) and, one column per full pass of it, the slow
 * "BEST OFFICERS" highlight sweep at $590A ($59, HISCORE_CURSOR_ADDR_INIT)
 * above. */
#define MARQUEE_ROW_ATTR_H  (0x59)
#define MARQUEE_ROW_ATTR_L  (0x67)

/**
 * $C00C: Format the score and check the high-score table
 *
 * Formats the 8-digit BCD score_bcd into an ASCII digit string with leading
 * zeros blanked to spaces, then scans the 10-row high-score table (best
 * first) for the highest-ranked row the new score beats or ties. Falls
 * through into insert_high_score_entry on a hit.
 *
 * Conv: the Z80 builds the digit string via a nibble-swap loop driven by a
 *       banked "significant digit seen" flag in C (tested via RLC C's
 *       carry-out); modelled directly as a bool-like u8 so the tens/units digit
 *       blocks (which are byte-for-byte identical in the Z80 bar which nibble
 *       they mask) collapse into one loop over both nibbles of each BCD byte.
 *
 * Conv: the 10-row scan is a lexicographic ASCII compare -- identical in effect
 *       to memcmp, since a space ($20) sorts below any digit ($30-$39), so a
 *       blanked leading zero correctly compares as "less than" a real digit of
 *       a longer number. Modelled directly as memcmp rather than the Z80's
 *       digit-by-digit CP/JR ladder.
 *
 * Conv: the Z80's opening block ($C00C-$C017, LDIR copying a 13-byte blank-row
 *       template from $C580 into the $C58D-$C59A name-entry scratch variables)
 *       is not translated. Those scratch bytes are only read by the
 *       joystick-driven letter-selection loop, which insert_high_score_entry's
 *       own Conv note already cuts from scope for the same reason -- see $C0EC
 *       there.
 */
static void check_high_score(chqstate_t *state)
{
  int B_bcd_count;  /* BCD bytes left to convert, 4 down to 1 (was B) */
  u8  DE_bcd_index; /* index into score_bcd, 3 (MSB) down to 0 (was DE) */
  u8 *HL_digit;     /* destination cursor in high_score_digits (was HL) */
  u8  C_seen;       /* "significant (non-zero) digit already seen" flag (was C) */
  u8  A_bcd;        /* current BCD byte (was A) */
  int nibble;       /* 0 = tens nibble, 1 = units nibble (Conv: rolled, no Z80 equivalent) */
  u8  A_digit;      /* extracted BCD digit, 0-9 (was A) */
  int row;          /* high-score table row under test, 0 = 1st place (Conv: rolled, no Z80 equivalent) */

  C_seen       = 0;
  HL_digit     = state->bank3->high_score_digits;
  DE_bcd_index = 3;

  for (B_bcd_count = 4; B_bcd_count > 0; B_bcd_count--)
  {
    A_bcd = state->score_bcd[DE_bcd_index];

    for (nibble = 0; nibble < 2; nibble++)
    {
      A_digit = (u8) (nibble == 0 ? (A_bcd >> 4) : (A_bcd & 0x0F));

      if (A_digit == 0 && !C_seen)
      {
        *HL_digit++ = ' '; /* suppressed leading zero */
      } else
      {
        C_seen      = 1;
        *HL_digit++ = (u8) (A_digit + '0');
      }
    }

    DE_bcd_index--;
  }

  for (row = 0; row < HIGH_SCORE_TABLE_ROWS; row++)
  {
    if (memcmp(state->bank3->high_score_digits,
               state->bank3->high_score_table[row].score, 8) >= 0)
               {
      insert_high_score_entry(state, row);
      return;
    }
  }
}

/**
 * Conv: added -- no direct Z80 address; backs titlescr_refresh_name_table
 * ($C69A)'s 15-byte row copy.
 *
 * Reads one row of the live high-score table into a caller-supplied buffer.
 *
 * Conv: added -- functional equivalent of titlescr_refresh_name_table ($C69A),
 *       which on real hardware copies the top 3 rows of the live table at $C403
 *       into a work buffer so the attract-mode "BEST OFFICERS" overlay reflects
 *       entries just typed in on the name-entry screen. This port exposes the
 *       same data field-by-field instead of replicating the byte-for-byte
 *       15+7+6 segmented copy, since the destination here is Main.c's
 *       best_officers overlay array, not a literal $800A-addressed buffer.
 *
 * \param[in]  row Table row to read: 0 = 1st place .. 9 = 10th place.
 * \param[out] out Buffer receiving the row: 8 score digits, 3 stage-code bytes,
 *                 1 retry digit, 3 name bytes (15 bytes total).
 */
void bank3_read_high_score_row(chqstate_t *state, int row, u8 *out)
{
  const high_score_row_t *entry; /* row being read (Conv: added) */

  entry = &state->bank3->high_score_table[row];

  memcpy(&out[0], entry->score, 8);
  memcpy(&out[8], entry->stage_code, 3);
  out[11] = entry->retry_digit;
  memcpy(&out[12], entry->name, 3);
}

/**
 * $C06E: Shift and write a new high-score table entry
 *
 * Reached from check_high_score when the new score beats or ties the row at
 * [row]. Shifts every row from [row] down to (but not including) the last
 * row down by one place -- discarding the old bottom row -- then writes the
 * new score digits, stage code and retry-attempt number into [row], leaving
 * a placeholder ". . ." name.
 *
 * Falls through into name_entry_setup_screen/ihe_flash_loop ($C0EC-$C154),
 * which handle the screen clear/setup, header text, and the joystick-driven
 * letter-selection loop that lets the player type their 3 initials.
 *
 * \param[in] row Table row to insert at: 0 = 1st place .. 9 = 10th place (was
 *                the row counter C, banked via EX AF,AF' across the shift).
 *
 * Conv: the Z80 shifts rows via LDDR over raw 33-byte-stride row bytes,
 *       carefully skipping the 7 static "next row's rank suffix" bytes tucked
 *       into each row's unused tail (see the data block's own comment at $C400
 *       in the skool). Modelled here as a plain struct-array shift over
 *       high_score_row_t, which holds only the fields that actually move -- the
 *       rank-suffix strings ("1ST ".."10TH") are fixed to their screen
 *       position, never move, and are not stored per-row at all (see
 *       high_score_rank_suffixes above).
 */
static void insert_high_score_entry(chqstate_t *state, int row)
{
  // clang-format off
  /* 128K bank 3: $C567-$C57E, the 6-entry stage-code table read by
   * insert_high_score_entry ($C09F), indexed by wanted_stage_number-1 (state
   * fields are 1-6; the Z80 table is addressed from a base 3 bytes before
   * its first real entry so that a raw 1-based multiply lands correctly).
   * Conv: extended to 7 entries -- with CHQ_ENABLE_TEST_STAGE, MAXSTAGE is 6
   * and the "beat all stages" case sets wanted_stage_number to MAXSTAGE+1 = 7,
   * one past what the original 6-entry table could index. */
  static const u8 high_score_stage_codes[7][3] = {
    " 1 ", " 2 ", " 3 ", " 4 ", " 5 ", " 6 ", "ALL",
  };
  // clang-format on

  int                shift_row;   /* row being overwritten by the one above it, 9 down to row+1 (Conv: rolled, no Z80 equivalent) */
  high_score_row_t  *DE_entry;    /* the row being written (was DE, after $C08B POP DE) */
  u8                 A_stage_idx; /* wanted_stage_number - 1: index into high_score_stage_codes (was A/C) */

  for (shift_row = HIGH_SCORE_TABLE_ROWS - 1; shift_row > row; shift_row--)
    state->bank3->high_score_table[shift_row] = state->bank3->high_score_table[shift_row - 1];

  DE_entry = &state->bank3->high_score_table[row];

  memcpy(DE_entry->score, state->bank3->high_score_digits, 8);

  A_stage_idx = (u8) (state->wanted_stage_number - 1);
  memcpy(DE_entry->stage_code, high_score_stage_codes[A_stage_idx], 3);

  DE_entry->retry_digit = (u8) (state->retry_count + '1');

  DE_entry->name[0] = '.';
  DE_entry->name[1] = '.';
  DE_entry->name[2] = '.'; /* placeholder until the player confirms real
                         * letters via name_entry_input/hiscore_finalise */

  state->bank3->hiscore.row = (u8) row;
  name_entry_setup_screen(state);
  ihe_flash_loop(state);

  clear_playfield_and_attrs(state); /* Conv: added -- clear hiscore table
                                      * before returning to the title screen */
  update_whole_playfield(state);    /* Conv: added */
}

/* $C3AF-$C400: static header/label text for the name-entry screen, packed in
 * print_character record format -- byte0 (style bit7 + colour), TWOBYTES of
 * the screen address, then the character stream with the last character
 * OR'd with EOS. Decoded directly from the raw bytes, not the skool's own
 * "pos=.. attr=.." prose (which mislabels the field order -- see
 * print_character's own unpack order at $FDA4). */
// clang-format off
static const u8 name_entry_screen_text[] = {
  0x02, ZXSCREEN(0x480A), 'B','E','S','T',' ','O','F','F','I','C','E','R', 'S' | EOS,
  0xC6, ZXSCREEN(0x4867), 'E','N','T','E','R',' ','Y','O','U','R',' ','I','N','I','T','I','A','L', 'S' | EOS,
  0x07, ZXSCREEN(0x488E), '.',' ','.',' ', '.' | EOS,
  0xC6, ZXSCREEN(0x48C0),
    'R','A','N','K',' ',' ',' ',' ',
    'S','C','O','R','E',' ',' ',
    'S','T','A','G','E',' ',' ',
    'P','L','A','Y',' ',' ',
    'N','A','M', 'E' | EOS,
  0
};
// clang-format on

/* $C54B-$C55E: (E, D) screen address of each rank's name field in the
 * 10-row table -- consumed by scroll_score_rows' full-table renderer, not
 * the letter-entry cursor (see cursor_cell_addr below: the ". . ." record
 * at $488E is a separate, fixed on-screen slot the player types into,
 * independent of which rank the confirmed name will land in). Ranks 1-2
 * (D < $40) start one character row above the visible screen and scroll
 * down into place in the original. */
static const u8 name_entry_row_offsets[HIGH_SCORE_TABLE_ROWS][2] = {
  { 0x80, 0x38 }, { 0xC0, 0x38 },
  { 0x00, 0x40 }, { 0x40, 0x40 }, { 0x80, 0x40 }, { 0xC0, 0x40 },
  { 0x00, 0x48 }, { 0x40, 0x48 }, { 0x80, 0x48 }, { 0xC0, 0x48 },
};

/**
 * $C0EC: Set up the name-entry screen
 *
 * Clears the playfield, seeds the per-row scroll table, prints the four
 * static header/label strings, starts interrupt-table setup (a no-op on
 * this host, called for fidelity) and tune 3, then resets the hiscore
 * sub-struct's fields ready for ihe_flash_loop to drive the interactive
 * letter-entry loop.
 * state->bank3->hiscore.row must already be set by the caller
 * (insert_high_score_entry) to the row being written.
 *
 * Conv: $C0EC (CALL $C890) is clear_playfield_and_attrs.
 */
static void name_entry_setup_screen(chqstate_t *state)
{
  int row; /* name_entry_row_addr copy index (Conv: rolled, no Z80 equivalent) */

  clear_playfield_and_attrs(state);
  update_whole_playfield_full_width(state); /* Conv: added -- draws right to
                                               * the screen edge, so needs the
                                               * full-width dirty rect, not
                                               * just update_whole_playfield's
                                               * driving-playfield inset */

  for (row = 0; row < HIGH_SCORE_TABLE_ROWS; row++)
  {
    state->bank3->hiscore.row_addr[row][0] = name_entry_row_offsets[row][0];
    state->bank3->hiscore.row_addr[row][1] = name_entry_row_offsets[row][1];
  }

  print_string(state, name_entry_screen_text);
  /* Row contents are drawn by scroll_score_rows/redraw_score_name as each
   * rank's row scrolls into view, not here. */

  setup_im2_interrupt_table(state); /* $F7AA -- no-op on this host */
  titlescr_start_tune(state, 3);

  state->bank3->hiscore.char_index        = 0;
  state->bank3->hiscore.letter_code       = '@'; /* blank/"." marker */
  state->bank3->hiscore.fire_locked       = 0;
  state->bank3->hiscore.flash_phase_a     = 0xF0; /* $C59A ROM-data seed --
                                                     * rotated left one frame
                                                     * at a time, see
                                                     * name_entry_dispatch */
  state->bank3->hiscore.flash_phase_b     = 0xEE; /* $C59B ROM-data seed,
                                                     * rotated the same way */
  state->bank3->hiscore.draw_erase_toggle = 0xF0; /* $C58D ROM-data seed
                                                     * (copied from the $C580
                                                     * template by
                                                     * check_high_score) --
                                                     * rotated left one frame
                                                     * at a time, see
                                                     * name_entry_frame/
                                                     * blink_hiscore_row */
  state->bank3->hiscore.blink_timer       = 0x0C;
  state->bank3->hiscore.blink_offset      = 0;
  state->bank3->hiscore.cursor_addr       = HISCORE_CURSOR_ADDR_INIT;
  state->bank3->hiscore.complete          = 0;
  state->bank3->hiscore.intro_timer       = 0xA0; /* $C13D reset value */
}

/**
 * $C133/$C149: Per-frame driver for the name-entry screen
 *
 * Two phases, matching the two Z80 loops:
 *
 * - ihe_flash_loop ($C133-$C142): the row scroll-in intro. Calls
 *   scroll_score_rows every frame while hiscore.intro_timer is nonzero.
 * - ihe_entry_loop ($C149-$C152): the interactive typing loop, entered once
 *   the intro timer reaches zero. Calls name_entry_frame (the current
 *   letter's blink) instead -- scroll_score_rows is never called again, so
 *   the rows stop scrolling for the rest of name entry, but hiscore.row's
 *   text keeps blinking via a direct blink_hiscore_row call alongside
 *   name_entry_frame (matching the real $C155/$C2FB call, which continues
 *   to redraw hiscore.row's text every frame after the row has parked).
 *
 * Both phases call name_entry_input and run until hiscore_finalise sets
 * hiscore.complete.
 *
 * Conv: the Z80's $C133 loop condition ($C13C-$C142) tests a self-modified
 *       operand ("LD A,$A0 / DEC A / LD ($C13D),A") that counts down once from
 *       160 over 160 frames; the skool's own comment ("always recomputes to a
 *       constant $9F, so JR NZ is always taken") only holds for a single static
 *       read of the bytes and misses the self-modification -- see
 *       hiscore.intro_timer's own comment (Bank3State.h). This port models the
 *       countdown as a plain state field instead of a self-modified immediate.
 *
 * Conv: the Z80's phase transition and hiscore_finalise's early exit are both
 *       stack-discarding jumps (the skool's own comment: "this loop can only
 *       actually end via a side effect... e.g. popping this return address");
 *       this port replaces both with the intro_timer/complete flags checked
 *       here.
 */
static void ihe_flash_loop(chqstate_t *state)
{
  for (;;)
  {
    CHECK_HOST_QUIT(state);

    titlescr_music(state); /* $F82F */

    if (state->bank3->hiscore.intro_timer != 0)
    {
      state->bank3->hiscore.intro_timer--;
      scroll_score_rows(state);
    } else
    {
      name_entry_frame(state);
      blink_hiscore_row(state, 0);
    }

    name_entry_input(state);

    if (state->bank3->hiscore.complete)
    {
      /* Conv: the real $C258 parks here forever servicing tune 2, only
       * leaving via an interrupt-driven scene change elsewhere -- it never
       * returns to its own caller (see hiscore_finalise's prologue). This
       * port must return normally, so instead it services the tune to
       * completion here, using the same title_music.tune_active
       * end-of-pattern signal titlescr_wait_loop polls for its own tune
       * waits, then returns. */
      while (state->bank3->title_music.tune_active)
      {
        CHECK_HOST_QUIT(state);
        titlescr_music(state);
      }
      return;
    }
  }
}

/* Conv: added -- no Z80 equivalent. Returns the on-screen (D, E) address of
 * the [char_index]'th letter cell of the typing cursor. Anchored on the
 * ". . ." text record printed at $488E (name_entry_screen_text, third
 * record): dot 0 is drawn at E=$8E, and each subsequent char in that record
 * (space, dot, space, dot) occupies the next column, so the three dot
 * positions are E=$8E, $90, $92 -- a stride of 2 per letter cell, D=$48
 * throughout. This screen has a single fixed typing cursor shared by every
 * rank; name_entry_row_offsets (above) is unrelated to it. */
static void cursor_cell_addr(u8 char_index, u8 *D_out, u8 *E_out)
{
  *D_out = 0x48;
  *E_out = (u8) (0x8E + char_index * 2);
}

/**
 * $C155: Advance the currently-typed letter's blink and redraw it
 *
 * Rotates the draw/erase phase (see blink_hiscore_row's comment for why this
 * is an 8-bit RLC rotation, not a per-frame flip) and redraws the
 * currently-selected letter cell, called once per frame from ihe_flash_loop.
 *
 * Conv: the Z80 reads a live 2-byte scratch value from $C422 (statically part
 *       of row 1's template padding in ROM data) to locate the cell; its real
 *       runtime purpose could not be recovered from static disassembly. This
 *       computes the same cell directly from hiscore.char_index via
 *       cursor_cell_addr instead.
 */
static void name_entry_frame(chqstate_t *state)
{
  u8 D_screen; /* target cell screen address high byte (was D) */
  u8 E_screen; /* target cell screen address low byte (was E) */

  cursor_cell_addr(state->bank3->hiscore.char_index, &D_screen, &E_screen);

  state->bank3->hiscore.draw_erase_toggle =
    (u8) ((state->bank3->hiscore.draw_erase_toggle << 1) |
           (state->bank3->hiscore.draw_erase_toggle >> 7));
  redraw_name_frame(state, D_screen, E_screen);
}

/* Conv: added -- redraws whichever of the 3 cells hiscore.char_index
 * currently selects, shared by cycle_and_draw_letter and name_entry_dispatch's
 * FIRE handler (both look up the cell's screen address then draw the glyph
 * there). */
static void redraw_letter_cursor(chqstate_t *state)
{
  u8 D_screen; /* target cell screen address high byte (was D) */
  u8 E_screen; /* target cell screen address low byte (was E) */

  cursor_cell_addr(state->bank3->hiscore.char_index, &D_screen, &E_screen);
  hiscore_draw_glyph(state, D_screen, E_screen);
}

/**
 * $C16A: Poll input and drive letter selection for the current cell
 *
 * Reads keyscan() masked to RIGHT/LEFT/FIRE and hands it to
 * name_entry_dispatch. Split out so CHQ_TESTS can drive
 * name_entry_dispatch directly with injected input, bypassing the host
 * keyboard/joystick read.
 */
static void name_entry_input(chqstate_t *state)
{
  u8 A_input; /* keyscan() result, masked to RIGHT/LEFT/FIRE (was A) */

  A_input = (u8) (keyscan(state) &
                  (USERINPUTFLAG_RIGHT | USERINPUTFLAG_LEFT | USERINPUTFLAG_FIRE));
  name_entry_dispatch(state, A_input);
}

/* Conv: added -- writes one attribute cell of the "BEST OFFICERS" marquee row
 * and marks it dirty, replacing the *ADDRTOATTRS(...)=value; update_attrs(...)
 * pair repeated at every marquee attribute write below and in
 * name_entry_dispatch. */
static void set_marquee_attr(chqstate_t *state, u8 L_attr, u8 value)
{
  u16 addr; /* marquee row attribute cell address (Conv: added) */

  addr = (u16) ((MARQUEE_ROW_ATTR_H << 8) | L_attr);

  *ADDRTOATTRS(addr) = value;
  update_attrs(state, addr, 8, 8);
}

/* $C1A8-$C1C2: fast-blinks the "BEST OFFICERS" cell currently being chased
 * into (cursor_addr) between bright red and off, by rotating flash_phase_a
 * and using the bit rotated out as the blink state. Runs every frame,
 * unlike name_entry_dispatch's own once-per-sweep promote-to-solid step;
 * see name_entry_dispatch's prologue for how the two combine. */
static void fast_blink_best_officers_cell(chqstate_t *state)
{
  u8 carry_a;      /* MSB rotated out of flash_phase_a this frame (was Cy after RLC $C59A) */
  u8 C_blink_attr; /* fast-blink colour for the cell in progress (was C) */

  carry_a = (u8) (state->bank3->hiscore.flash_phase_a >> 7);
  state->bank3->hiscore.flash_phase_a =
    (u8) ((state->bank3->hiscore.flash_phase_a << 1) | carry_a);
  C_blink_attr = carry_a ? 0x42 : 0x00;

  set_marquee_attr(state, state->bank3->hiscore.cursor_addr, C_blink_attr);

  /* $C1C0: the paired double-height row below shares the same blink, minus
   * the bright bit. */
  set_marquee_attr(state, (u8) (state->bank3->hiscore.cursor_addr + 0x20),
                    (u8) (C_blink_attr & ~0x40));

  state->bank3->hiscore.flash_phase_b =
    (u8) ((state->bank3->hiscore.flash_phase_b << 1) | (state->bank3->hiscore.flash_phase_b >> 7));
}

/**
 * $C16A: Drive letter selection for the current cell from masked input
 *
 * Runs the blink-timer countdown, then dispatches [A_input] to
 * cycle_and_draw_letter (RIGHT/LEFT) or the confirm path (FIRE): stores the
 * current letter, advances to the next of the 3 cells, or -- on the 3rd
 * cell -- calls hiscore_finalise. fire_locked debounces FIRE so a held key does
 * not repeatedly confirm.
 *
 * \param[in] A_input keyscan() result, masked to RIGHT/LEFT/FIRE (was A).
 *
 * $C172-$C19B also sweeps a highlight along the "ENTER YOUR INITIALS"
 * 20-cell selector row, unrelated to letter selection, and -- once per full
 * sweep -- advances the "BEST OFFICERS" chase by one column; both are
 * rendered here (see hiscore.blink_offset/cursor_addr). Its idle-timeout
 * side effect is also kept: 13 full sweeps (3120 frames, ~62s at 50Hz) with
 * no player input force-finalises the current letter via hiscore_finalise,
 * same as the real hardware.
 *
 * $C1A8-$C1C2 also runs every frame (not gated by the blink timer): it
 * rotates flash_phase_a and uses the bit rotated out to fast-blink the
 * "BEST OFFICERS" cell currently being chased into (cursor_addr) between
 * bright red and off, giving the letter still in progress its flicker --
 * distinct from the once-per-sweep promote-to-solid step above, which is
 * what actually makes the row look like it is "cycling" one letter at a
 * time. Rendered here as fast_blink_best_officers_cell.
 *
 * Conv: $C1C7/$C1D2's JP C,$C291/JP C,$C278 (taken from the flash_phase_a/
 *       flash_phase_b rotations) are alternate entry points into this same
 *       input-read code, an optimisation the real hardware uses to skip
 *       redundant work -- not a distinct visual or behavioural effect. This
 *       port always falls through to the unconditional RIGHT/LEFT/FIRE dispatch
 *       below instead, so flash_phase_b is rotated for fidelity only and has no
 *       visible effect here.
 */
static void name_entry_dispatch(chqstate_t *state, u8 A_input)
{
  if (--state->bank3->hiscore.blink_timer == 0)
  {
    u8 L_attr; /* selector cell column before advancing (was L via $C596) */

    state->bank3->hiscore.blink_timer = 0x0C;

    /* $C172-$C17B: restore the outgoing cell to its base colour (it was
     * blanked below on a previous call). */
    L_attr = (u8) (MARQUEE_ROW_ATTR_L + state->bank3->hiscore.blink_offset);
    set_marquee_attr(state, L_attr, 0x46);

    if (++state->bank3->hiscore.blink_offset >= 20)
    {
      state->bank3->hiscore.blink_offset = 0;

      /* $C183-$C18D: one full pass of the fast selector done -- promote the
       * current "BEST OFFICERS" column to a permanent bright highlight, and
       * its paired double-height row below to the matching non-bright
       * colour. */
      L_attr = state->bank3->hiscore.cursor_addr;
      set_marquee_attr(state, L_attr, 0x42);
      set_marquee_attr(state, (u8) (L_attr + 0x20), 0x02);

      if (++state->bank3->hiscore.cursor_addr == HISCORE_CURSOR_ADDR_FINALISE)
      {
        hiscore_finalise(state);
        return;
      }
    }

    /* $C19E-$C1A6: blank the new current cell -- this is the visible
     * "letter blinks off" step of the chase. */
    L_attr = (u8) (MARQUEE_ROW_ATTR_L + state->bank3->hiscore.blink_offset);
    set_marquee_attr(state, L_attr, 0x00);
  }

  fast_blink_best_officers_cell(state);

  if (A_input & USERINPUTFLAG_FIRE)
  {
    high_score_row_t *DE_entry; /* row being written (was DE) */
    u8                A_code;   /* current candidate letter code (was A) */

    if (state->bank3->hiscore.fire_locked)
      return;
    state->bank3->hiscore.fire_locked = 1;

    if (state->bank3->hiscore.char_index == 2)
    {
      /* hiscore_finalise stores this last letter itself -- no need to write
       * DE_entry->name here first. */
      hiscore_finalise(state);
      return;
    }

    DE_entry = &state->bank3->high_score_table[state->bank3->hiscore.row];
    A_code   = state->bank3->hiscore.letter_code;
    DE_entry->name[state->bank3->hiscore.char_index] = (A_code == '@') ? '.' : A_code;

    state->bank3->hiscore.char_index++;
    state->bank3->hiscore.letter_code = '@';

    redraw_letter_cursor(state);
    return;
  }

  state->bank3->hiscore.fire_locked = 0;

  if (A_input & (USERINPUTFLAG_RIGHT | USERINPUTFLAG_LEFT))
    cycle_and_draw_letter(state, A_input);
}

/**
 * $C212: Store the final confirmed letter and finish name entry
 *
 * Stores the current candidate letter (or '.' for the blank marker) into
 * the row's name[], marks name entry complete, plays the confirm sound,
 * redraws the confirmed cell one last time, then cues the finishing tune.
 *
 * Conv: the Z80 falls into an unbounded "service sound, loop" tail ($C258-
 *       $C25B) reached by discarding its own caller's return address
 *       (hiscore_finalise runs in name_entry_input's stack frame, not a nested
 *       CALL -- see name_entry_input's own prologue), so it never returns to
 *       ihe_flash_loop at all; the whole state stays parked servicing tune 2
 *       until the next interrupt-driven scene change. This returns normally
 *       instead, but hiscore.complete alone is not enough to reproduce the
 *       audible effect: ihe_flash_loop still has to service tune 2 to
 *       completion (see its own Conv comment) before it returns, otherwise the
 *       tune armed here never plays a single note.
 */
static void hiscore_finalise(chqstate_t *state)
{
  high_score_row_t *DE_entry; /* row being finalised (was DE) */
  u8                A_code;   /* final candidate letter code (was A) */

  DE_entry = &state->bank3->high_score_table[state->bank3->hiscore.row];
  A_code   = state->bank3->hiscore.letter_code;
  DE_entry->name[state->bank3->hiscore.char_index] = (A_code == '@') ? '.' : A_code;

  state->bank3->hiscore.complete = 1;

  stop_music_and_silence(state); /* $ED0B -- confirm sound */
  name_entry_frame(state);       /* final redraw of the confirmed cell */
  titlescr_start_tune(state, 2);
}

/**
 * $C25D: Advance the current candidate letter and redraw it
 *
 * Cycles hiscore.letter_code up or down through $41-$5A ('A'-'Z'), wrapping
 * through $40 ('@', the blank/"." marker), then redraws it.
 *
 * \param[in] C_input_bits Masked keyscan() bits; only RIGHT/LEFT are examined
 *                         here (was C).
 */
static void cycle_and_draw_letter(chqstate_t *state, u8 C_input_bits)
{
  if (C_input_bits & USERINPUTFLAG_RIGHT)
  {
    state->bank3->hiscore.letter_code =
      (state->bank3->hiscore.letter_code == 'Z') ? '@' : (u8) (state->bank3->hiscore.letter_code + 1);
  } else if (C_input_bits & USERINPUTFLAG_LEFT)
  {
    state->bank3->hiscore.letter_code =
      (state->bank3->hiscore.letter_code == '@') ? 'Z' : (u8) (state->bank3->hiscore.letter_code - 1);
  }

  redraw_letter_cursor(state);
}

/**
 * $C291/cad_draw_glyph: Draw the current candidate letter's glyph
 *
 * Blits hiscore.letter_code's 7-byte font[] entry double-height at
 * (D_screen, E_screen), each font byte written to two consecutive pixel
 * rows: the ". . ." placeholder this overwrites was itself printed
 * double-height by print_character (name_entry_screen_text's third
 * record, style byte $07, bit 7 clear), so the candidate letter must match
 * it or the cell visibly shrinks to single height when typing starts.
 *
 * No attribute write: the real $C291-$C2B0 never touches attributes either
 * -- the placeholder's shaded bright-top/dim-bottom attrs are set once by
 * print_character and are left alone here.
 *
 * \param[in] D_screen Target cell screen address high byte (was D).
 * \param[in] E_screen Target cell screen address low byte (was E).
 */
static void hiscore_draw_glyph(chqstate_t *state, u8 D_screen, u8 E_screen)
{
  u8         A_code;     /* current candidate glyph code, $40 or $41-$5A (was A) */
  u8         C_idx;      /* font[] entry index -- 4 for blank, else code-$32 (was C) */
  const u8  *HL_font;    /* pointer to this glyph's 7-byte font[] entry (was HL) */
  int        glyph_addr; /* Z80 screen address of this glyph (Conv: added) */
  u8        *DE_screen;  /* pixel destination cursor (was DE) */
  int        row;        /* blit row counter (Conv: rolled) */

  A_code  = state->bank3->hiscore.letter_code;
  C_idx   = (A_code == '@') ? 4 : (u8) (A_code - 0x32);
  HL_font = &font[C_idx * 7];

  glyph_addr = (D_screen << 8) | E_screen;
  DE_screen  = ADDRTOSCREEN(glyph_addr);

  for (row = 0; row < 4; row++)
  {
    *DE_screen  = *HL_font;
    DE_screen  += 256;
    *DE_screen  = *HL_font++;
    DE_screen  += 256;
  }

  /* Conv: crosses the screen-third boundary; see print_character's own
   * comment on the identical -2016 correction for the reasoning. */
  DE_screen -= 2016;
  for (row = 0; row < 3; row++)
  {
    *DE_screen  = *HL_font;
    DE_screen  += 256;
    *DE_screen  = *HL_font++;
    DE_screen  += 256;
  }

  update_screen(state, glyph_addr, 8, 14);
}

/* Conv: added -- advances a (D, E) screen-address pair by one pixel
 * scanline, correcting the carry a bare D++ would otherwise misroute into
 * D's thirds-select bits once every 8 steps. D's low 3 bits hold the pixel
 * line within the current 8-line character cell, so D++ advances the line
 * correctly for 7 out of 8 steps, but on the 8th (low 3 bits wrap 7 -> 0)
 * the carry must land in E's cell-row bits instead -- the same
 * third-boundary-wrap idiom documented in cad_draw_glyph and
 * redraw_name_frame's own draw/erase loops. Shared by scroll_score_rows
 * (one step per row per frame) and draw_table_field_scrolling (up to 8
 * steps per glyph, to walk down one character cell). */
static void advance_screen_scanline(u8 *D, u8 *E)
{
  int H; /* widened copy of *D for advance_glyph_scanline (Conv: added) */
  int L; /* widened copy of *E for advance_glyph_scanline (Conv: added) */

  H = *D + 1;
  L = *E;
  advance_glyph_scanline(&H, &L);
  *D = (u8) H;
  *E = (u8) L;
}

/**
 * Whether (D, E) falls in the table's visible draw window.
 *
 * $C2D3-$C2EB. Not simply "thirds 1-2" -- the real check is asymmetric per
 * third: third 1 ($48-$4F) only draws on its bottom character-row (E >=
 * $E0); third 2 ($50-$57) draws on every character-row except its bottom one
 * (E < $E0). Together these cover one contiguous 8-character-row band
 * (third 1's last row followed by third 2's first seven), not the full 16
 * rows both thirds span. Using the wider "D in $48-$57" range instead (an
 * earlier version of this function did) starts each row drawing a full
 * character-row band too early, overlapping rows already at rest further
 * down the table.
 *
 * \param[in] D Screen address high byte.
 * \param[in] E Screen address low byte.
 *
 * \return      Non-zero if (D, E) is in the visible draw window.
 */
static int table_row_visible(u8 D, u8 E)
{
  if (D < 0x48)
    return 0;
  if (D < 0x50)
    return E >= 0xE0;
  if (D < 0x58)
    return E < 0xE0;
  return 0;
}

/**
 * $C2B1: Animate each rank's name field scrolling into view
 *
 * Advances each of the 10 ranks' screen-address entry (hiscore.row_addr) down
 * by one pixel row per frame, then redraws that row's full contents (rank
 * suffix, score, stage code, retry digit, name) at its new position. Row
 * addresses are free-running u8 counters that are never stopped or reset
 * once scrolling starts (matching the Z80), so a row cycles through the
 * whole 256-line address space and back into view periodically -- with only
 * 16 character-row slots visible for 10 ranks, more than one rank's row can
 * be scrolling through the visible band at a time, and two ranks can
 * briefly overlap the same slot exactly as the original hardware does.
 *
 * The real $C2D3-$C2EB gate (table_row_visible) fires once per row per
 * frame, on the row's just-advanced address -- not an 8-frame throttle (an
 * earlier version of this comment claimed that; it was wrong, confused with
 * the unrelated `AND $07` third-wrap test inside advance_screen_scanline).
 * Every frame a row's address is inside the visible band, $C2F6 redraws it;
 * outside the band nothing touches that row's pixels at all -- no draw, no
 * erase. rsn_char_loop's blit is unclamped (see draw_table_field_scrolling)
 * and leads with its own one-scanline erase ($C350-$C352) before each
 * character's 7-row glyph, one row below the erase point. Because the
 * trigger fires every frame the address is in-band, not just once, the net
 * visual is a "comet": a blank leading edge advancing one scanline per
 * frame with the glyph's 7 rows trailing behind it, each frame re-erasing
 * the row the previous frame's glyph occupied. Only once scrolling stops
 * (the row parks) does the trailing glyph settle into a normal, static
 * character cell.
 *
 * hiscore.row is not exempt from this and does not draw plainly: $C2F7's
 * self-modified `CP $09` (patched by insert_high_score_entry's $C0C0) picks
 * out that one row and routes it through the blink toggle at $C2FB
 * (blink_hiscore_row) instead of a plain draw, every trigger frame -- so it
 * blinks throughout the scroll-in, not just once parked.
 */
static void scroll_score_rows(chqstate_t *state)
{
  int                row;    /* rank index, 0-9 (was B, DJNZ 10 down to 1) */
  u8                 E;      /* row's screen address low byte (was E) */
  u8                 D;      /* row's screen address high byte (was D) */
  high_score_row_t  *DE_entry; /* this row's data (was DE) */

  for (row = 0; row < HIGH_SCORE_TABLE_ROWS; row++)
  {
    E = state->bank3->hiscore.row_addr[row][0];
    D = state->bank3->hiscore.row_addr[row][1];

    advance_screen_scanline(&D, &E);

    state->bank3->hiscore.row_addr[row][0] = E;
    state->bank3->hiscore.row_addr[row][1] = D;

    if (!table_row_visible(D, E))
      continue;

    if (row == state->bank3->hiscore.row)
    {
      blink_hiscore_row(state, 1);
      continue;
    }

    DE_entry = &state->bank3->high_score_table[row];

    draw_score_row_fields(state, D, E, (u8) row, DE_entry, 1, TABLE_ROW_COLOUR);
  }
}

/* Conv: added -- the glyph classification ladder print_character's own
 * $FDDA-$FDFE uses, factored out so draw_table_field_scrolling below can
 * look up a glyph per scanline without re-running print_character's whole
 * record-unpack/blit loop (which assumes a cell-aligned start address --
 * see advance_screen_scanline's comment). Returns NULL for a literal space
 * (caller still advances the column but draws nothing). */
static const u8 *font_glyph_for_char(u8 A_char)
{
  u8 A_diff;  /* char - $20; classification input (was A) */
  u8 C_class; /* width-class index (was C) */

  if (A_char == ' ')
    return NULL;

  A_diff = (u8) (A_char - ' ');

  if (A_diff >= 0x21)
    C_class = (u8) (A_diff - 18);
  else if (A_diff >= 0x10)
    C_class = (u8) (A_diff - 11);
  else if (A_diff == 1)
    C_class = 0;
  else if (A_diff == 8)
    C_class = 1;
  else if (A_diff == 9)
    C_class = 2;
  else if (A_diff == 12)
    C_class = 3;
  else
    C_class = 4;

  return &font[C_class * 7];
}

/**
 * $C2F6: Draw [len] characters of a rank's row at its current scroll position
 *
 * Conv: the real $C2F6 (rsn_char_loop) draws from the (D, E) screen address
 *       scroll_score_rows scrolled to, via its own byte-stream glyph loop -- a
 *       near-clone of cycle_and_draw_letter's classifier -- and does not stop
 *       advancing a row's address once drawn: the row keeps scrolling (and
 *       keeps redrawing) past its first rest line, which is how all 10 ranks
 *       share the character-row slots actually free below the header (one rank
 *       is always mid-scroll, carrying the previous rank's row off past the
 *       header as it arrives). Only called (by scroll_score_rows/
 *       blink_hiscore_row) once the row's starting address has already been
 *       checked against table_row_visible; from there the blit is unclamped.
 *       Each character starts with $C350-$C352's own leading write: a zero
 *       byte at the character's un-advanced (D, E), before any glyph pixels.
 *       Only then does the 7-row glyph loop run, and $C353 (INC D) executes
 *       *before* each row's write, so the glyph's own 7 rows land one
 *       scanline below the just-erased one -- font[0] at D+1, ..., font[6] at
 *       D+7, matching rsn_draw_row's pre-increment order exactly (translated
 *       here via advance_screen_scanline, which already increments before
 *       wrapping). Because the trigger fires every frame a row's address sits
 *       in the visible band (not just once), this leading erase plus
 *       1-row-offset glyph is what makes each row look like a comet: the
 *       blank edge is always one scanline ahead of the glyph trailing it, so
 *       scanlines the window has already passed are actively blanked, not
 *       merely left alone. A version without the leading erase (an earlier
 *       revision of this function) instead leaves every prior frame's glyph
 *       rows in place, which smears the character downward across the whole
 *       transit -- the real hardware never does that; only the final frozen
 *       frame (once scrolling stops) shows a normal, fully-formed glyph.
 *
 * \param[in] x     Pixel column of the field's first character, 0-255.
 * \param[in] D_row Row's current screen address high byte (thirds/pixel-row).
 * \param[in] E_row Row's current screen address low byte; only its cell-row
 *                  bits (0xE0) are used -- the column comes from x/i.
 * \param[in] text  Raw ASCII bytes to draw.
 * \param[in] len   Number of characters in [text].
 * \param[in] attrs Conv: added -- no Z80 equivalent. Attribute byte written for
 *                  this field's cells, in place of the fixed TABLE_ROW_COLOUR.
 *                  Lets scroll_score_rows set the FLASH bit on the rank 1 row
 *                  so it blinks via real ZX hardware FLASH (see Screen.c's
 *                  WRITE8PIX/WRITE8PIX_16) instead of a software toggle.
 * \param[in] erase Zero to draw (rsn_char_loop, $C304): a leading 1-row erase
 *                  then a 7-row glyph blit, offset one row below it. Non-zero
 *                  to erase (rsn_char_loop2, $C37B): 8 rows of zero at the
 *                  character's own row, no offset. Both skip space characters
 *                  entirely (column still advances, nothing is written).
 */
static void draw_table_field_scrolling(chqstate_t *state,
                                       int         x,
                                       u8          D_row,
                                       u8          E_row,
                                       const u8   *text,
                                       int         len,
                                       u8          attrs,
                                       int         erase)
{
  int       i;         /* character index within text (Conv: added) */
  u8        D;         /* this glyph's current scanline address, high byte */
  u8        E;         /* this glyph's current scanline address, low byte */
  const u8 *HL_font;   /* this glyph's 7-byte font[] entry, NULL for space */
  int       row;       /* scanline index within the glyph's rows (Conv: added) */
  u8       *DE_screen; /* pixel destination for this scanline (Conv: added) */
  u16       attr_addr; /* this glyph's attribute address (Conv: added) */

  for (i = 0; i < len; i++)
  {
    D = D_row;
    E = (u8) ((E_row & 0xE0) | ((x >> 3) + i));

    HL_font = font_glyph_for_char(text[i]);

    attr_addr = (u16) (((0x58 + ((D >> 3) & 0x03)) << 8) | E);
    *ADDRTOATTRS(attr_addr) = attrs;
    update_attrs(state, attr_addr, 8, 8);

    if (HL_font == NULL)
      continue;

    if (erase)
    {
      /* $C390-$C3A9 (rsn_char_loop2): 8 rows of zero, write-then-advance,
       * starting at the character's own (D, E) -- no leading byte, no
       * 1-row offset. */
      for (row = 0; row < 8; row++)
      {
        DE_screen  = ADDRTOSCREEN((D << 8) | E);
        *DE_screen = 0;
        update_screen(state, (D << 8) | E, 8, 1);
        advance_screen_scanline(&D, &E);
      }
      continue;
    }

    /* $C350-$C352: leading erase at the un-advanced (D, E), before any
     * glyph row is drawn. */
    DE_screen  = ADDRTOSCREEN((D << 8) | E);
    *DE_screen = 0;
    update_screen(state, (D << 8) | E, 8, 1);

    for (row = 0; row < 7; row++)
    {
      /* $C353: INC D (+ wrap) runs before the row's write, not after. */
      advance_screen_scanline(&D, &E);
      DE_screen  = ADDRTOSCREEN((D << 8) | E);
      *DE_screen = HL_font[row];
      update_screen(state, (D << 8) | E, 8, 1);
    }
  }
}

/* $C2F6-$C2F9: whichever rank is being written this session (hiscore.row)
 * does not just draw plainly like the other nine -- every frame it is
 * inside the table's visible window (table_row_visible), it alternates
 * between drawing its whole row text and blanking it, via the same
 * hiscore.draw_erase_toggle flag name_entry_frame flips for the
 * currently-typed cell (real $C58D, RLC'd at $C2FF; the CP $09 at $C2F7 is
 * self-modified by insert_high_score_entry's $C0C0 to compare against
 * whichever row counter matches hiscore.row). "Whole row" is not an
 * approximation: the real $C401 scratch table's 31-byte-per-row content
 * (copied from the 33-byte preset-row records, see the $C3AF-$C422 skool
 * comment) is rank suffix(5) + score(8) + gap(4) + stage(3) + gap(5) +
 * retry(1) + gap(2) + name(3, bit-7 terminated on its last byte) laid out
 * as ONE continuous string with no terminator before the very end --
 * rsn_char_loop walks straight through all five fields in a single pass,
 * so the whole row (suffix included -- "1ST", "2ND", ...) blinks together,
 * not just the name. This is what gives the newly-inserted row its blink
 * as it scrolls into place -- not a flat colour flash over the row's
 * attribute cells, and not on a fixed rate tied to the "BEST OFFICERS"
 * chase (see the removed flash_rank1_row, this comment's predecessor,
 * which modelled both of those incorrectly).
 *
 * The erase half below passes the row's own text to draw_table_field_scrolling
 * with erase set, matching rsn_char_loop2: it walks the same characters as
 * the draw pass (so its own space-skip lines up), but for each non-space
 * character it blanks all 8 rows instead of blitting a glyph.
 */
/* Conv: added -- the five field draws below (rank suffix, score, stage code,
 * retry digit, name) are shared verbatim between blink_hiscore_row and
 * scroll_score_rows' plain-draw branch. blink is non-zero to draw this
 * frame's text, zero to blank the row (blink_hiscore_row's toggle);
 * scroll_score_rows always passes non-zero since it never blanks. */
static void draw_score_row_fields(chqstate_t             *state,
                                  u8                      D,
                                  u8                      E,
                                  u8                      row,
                                  const high_score_row_t *entry,
                                  int                     blink,
                                  u8                      attrs)
{
  draw_table_field_scrolling(state, 0, D, E, high_score_rank_suffixes[row], 5, attrs, !blink);
  draw_table_field_scrolling(state, 64, D, E, entry->score, NELEMS(entry->score), attrs, !blink);
  draw_table_field_scrolling(state, 120, D, E, entry->stage_code, NELEMS(entry->stage_code), attrs, !blink);
  draw_table_field_scrolling(state, 176, D, E, &entry->retry_digit, 1, attrs, !blink);
  draw_table_field_scrolling(state, 224, D, E, entry->name, NELEMS(entry->name), attrs, !blink);
}

/**
 * $C2F6/$C2FB: redraw hiscore.row's whole row text, using the shared toggle
 *
 * Called once per frame for as long as name entry lasts -- both while the
 * row is still scrolling in (from scroll_score_rows, matching the real
 * $C2F6 boundary-triggered entry) and, once it has settled, every frame of
 * the interactive typing loop (from ihe_flash_loop, matching the real
 * $C155/$C2FB unconditional entry). Both real entry points share the same
 * $C58D toggle and the same row-text draw/erase loop, so the row keeps
 * blinking for the whole time its name is being entered, not just during
 * the scroll-in.
 *
 * hiscore.draw_erase_toggle is not a plain per-frame flip: it is the real
 * $C58D byte, rotated left one bit per call (RLC, matching flash_phase_a's
 * treatment elsewhere in this file) rather than XORed. Bit 0 after the
 * rotate selects draw (0) or erase (1) for that frame. Starting from its
 * ROM-seeded seed value 0xF0, one full 8-bit rotation gives four consecutive
 * draw frames followed by four consecutive erase frames, an 8-frame
 * (~6.25Hz) cycle -- not the 2-frame (~25Hz) alternation a bare toggle would
 * produce, and slow enough to read as distinct pulses rather than a flicker.
 *
 * Uses hiscore.row_addr[hiscore.row] directly: scroll_score_rows keeps this
 * current while the row is moving, and simply stops advancing it once the
 * row is parked, so the same address is still correct after scrolling ends.
 *
 * \param[in] do_toggle non-zero to rotate hiscore.draw_erase_toggle before
 *                      reading it (the intro-phase caller, scroll_score_rows,
 *                      is the only per-frame caller of the toggle at that
 *                      point); zero to read it as-is (the entry-phase caller,
 *                      ihe_flash_loop, calls this immediately after
 *                      name_entry_frame has already rotated the same shared
 *                      flag for the letter cursor -- rotating again here would
 *                      double up the frame's rotation and desync the two
 *                      blinks).
 */
static void blink_hiscore_row(chqstate_t *state, int do_toggle)
{
  u8                row;   /* rank index being written this session (Conv: added) */
  u8                D;     /* row's current screen address high byte (was D) */
  u8                E;     /* row's current screen address low byte (was E) */
  high_score_row_t *DE_entry; /* this row's data (was DE) */
  int               blink; /* non-zero draws this frame's text, zero blanks it (Conv: added) */

  row = state->bank3->hiscore.row;
  D   = state->bank3->hiscore.row_addr[row][1];
  E   = state->bank3->hiscore.row_addr[row][0];

  if (!table_row_visible(D, E))
    return;

  DE_entry = &state->bank3->high_score_table[row];

  if (do_toggle)
    state->bank3->hiscore.draw_erase_toggle =
      (u8) ((state->bank3->hiscore.draw_erase_toggle << 1) |
             (state->bank3->hiscore.draw_erase_toggle >> 7));
  blink = !(state->bank3->hiscore.draw_erase_toggle & 1);

  draw_score_row_fields(state, D, E, row, DE_entry, blink, TABLE_ROW_COLOUR);
}

/**
 * $C2FB: Draw or erase the currently-selected letter cell
 *
 * Toggles hiscore.draw_erase_toggle and redraws the currently-selected letter
 * cell at (D_screen, E_screen) accordingly, giving the letter being typed
 * its blink. draw_erase_toggle is shared with scroll_score_rows/
 * redraw_score_name's own draw/erase loops in the original; this port only
 * needs it for the single cell currently being typed.
 *
 * Conv: reduced scope -- the real $C2FB is an entry point into
 *       redraw_score_name's row loop (rsn_char_loop/rsn_char_loop2), which
 *       alternates every already-confirmed name across all 10 ranks. This only
 *       drives the single cell the player is currently typing, since that is
 *       the only text this port's redraw_score_name (the full-row version,
 *       above) does not already keep in view once revealed.
 *
 * \param[in] D_screen Screen address high byte of the currently-selected cell.
 * \param[in] E_screen Screen address low byte of the currently-selected cell.
 */
static void redraw_name_frame(chqstate_t *state, u8 D_screen, u8 E_screen)
{
  u8 *DE_screen;  /* erase-blit cursor (was DE) */
  int glyph_addr; /* Z80 screen address of this cell (Conv: added) */
  int scan_row;   /* erase-blit row counter (Conv: rolled) */

  if (!(state->bank3->hiscore.draw_erase_toggle & 1))
  {
    hiscore_draw_glyph(state, D_screen, E_screen);
    return;
  }

  /* Conv: blanks 14 scanlines (two character rows), matching the
   * double-height glyph hiscore_draw_glyph now draws -- see its own
   * comment for why this cell is double-height, not single. */
  glyph_addr = (D_screen << 8) | E_screen;
  DE_screen  = ADDRTOSCREEN(glyph_addr);
  for (scan_row = 0; scan_row < 4; scan_row++)
  {
    *DE_screen  = 0;
    DE_screen  += 256;
    *DE_screen  = 0;
    DE_screen  += 256;
  }
  DE_screen -= 2016; /* crosses the screen-third boundary, see hiscore_draw_glyph */
  for (scan_row = 0; scan_row < 3; scan_row++)
  {
    *DE_screen  = 0;
    DE_screen  += 256;
    *DE_screen  = 0;
    DE_screen  += 256;
  }

  update_screen(state, glyph_addr, 8, 14);
}


/**
 * $C59E: Title-screen driver
 *
 * Picks one of 5 pre-scripted animation scenes, populates the 9-entry
 * animated-object array at $BB00 from the chosen scene's object table, draws
 * the overlay text (title/credits, "PRESS ENTER FOR OPTIONS" always, and
 * "PRESS GEAR TO PLAY" once controls have been selected), starts tune 0, then
 * falls into the attract-mode wait loop (titlescr_wait_loop) which animates the
 * scene each frame while polling for credit/fire/keyboard input to start a
 * game. Called from $C000 and $FBC8.
 *
 * Conv: the self-modified scene-selector operand at $C5A2 is modelled as
 *       state->bank3->title_animation; see titlescr_wait_loop where the "any
 *       key" restart path reseeds it.
 *
 * Conv: the $FBA2 (ENTER pressed) branch is not an exit from this function:
 *       titlescr_wait_loop calls options_menu_driver's omd_redraw_and_poll
 *       directly, which in the Z80 ends in `JP $C59E` -- a hand-off back to
 *       this function -- so the ENTER path rejoins this loop exactly like the
 *       "any key" and test-mode restarts.
 *
 * \return The value the Z80 leaves in A for attract_mode_128k's $F421 test:
 * TITLESCR_START_GAME when a credit was inserted (fire pressed with controls
 * selected), TITLESCR_ATTRACT after the tune-4 wait tail. Both come from
 * titlescr_wait_loop's two genuine RET paths; the ordinary polling path (no
 * credit, no key) is unbounded.
 *
 * Conv: the Z80's `$C67E JP $C59E` / `$C693 JP $C59E` restarts are plain jumps
 *       -- they do not grow the Z80 stack. Calling run_title_screen recursively
 *       from titlescr_wait_loop would grow the C stack by one frame per restart
 *       with no bound (every "any key"/test-mode restart during a long
 *       attract-mode session), so instead titlescr_wait_loop returns non-zero
 *       to request a restart and this function loops.
 */
static u8 run_title_screen(chqstate_t *state)
{
  // clang-format off
  /**
   * $CC50 -- copyright/credits text, drawn by title_screen_driver via
   * print_string ($FD9C) before the $CCB7 scene tables.
   */
  static const u8 title_screen_credits_text[56] = {
    (SINGLE_HEIGHT | attribute_BRIGHT_RED_OVER_BLACK),  // was 0xC2
    ZXSCREEN(0x50C3),
    '(', 'C', ')', ' ', '1', '9', '8', '9', ' ', 'O', 'C', 'E', 'A', 'N', ' ', 'S', 'O', 'F', 'T', 'W', 'A', 'R', 'E' | EOS,
    (SINGLE_HEIGHT | attribute_BRIGHT_RED_OVER_BLACK),  // was 0xC2
    ZXSCREEN(0x50E2),
    '(', 'C', ')', ' ', '1', '9', '8', '8', ' ', 'T', 'A', 'I', 'T', 'O', ' ', 'C', 'O', 'R', 'P', 'O', 'R', 'A', 'T', 'I', 'O', 'N' | EOS,
    0
  };

  /**
   * $CC88 -- drawn directly by title_screen_driver via print_character (not
   * walked as a list: no end marker, matching the Z80, which runs straight
   * into the $CCB7 scene tables afterwards).
   */
  static const u8 title_screen_overlay_text[47] = {
    (SINGLE_HEIGHT | attribute_BRIGHT_WHITE_OVER_BLACK),  // was 0xC7
    ZXSCREEN(0x4826),
    'P', 'R', 'E', 'S', 'S', ' ', 'G', 'E', 'A', 'R', ' ', 'T', 'O', ' ', 'P', 'L', 'A', 'Y' | EOS,
    (SINGLE_HEIGHT | attribute_BRIGHT_GREEN_OVER_BLACK),  // was 0xC4
    ZXSCREEN(0x4864),
    'P', 'R', 'E', 'S', 'S', ' ', 'E', 'N', 'T', 'E', 'R', ' ', 'F', 'O', 'R', ' ', 'O', 'P', 'T', 'I', 'O', 'N', 'S' | EOS
  };

  /* Z80 address of the scene 4 object script shared by objects 0, 3, 4, 5 and 8
   * -- referenced from their object records within title_scene_data itself. */
#define SCENE4_SCRIPT_SHARED_ADDR (0xD199)

  /**
   * $CCB7-$D271: title_scene_data
   *
   * 5 scene tables (9x5-byte object records [x,y,row,ptr] + script bytecode).
   */
  static const u8 title_scene_data[1467] = {
    // $CCB7: scene 0 -- 9 object records (x, y, row, script ptr)
    0x46, 0x8F, 0x00, TWOBYTES(0xCCEE), // obj0
    0x5E, 0x8F, 0x04, TWOBYTES(0xCCEC), // obj1
    0x76, 0x8F, 0x08, TWOBYTES(0xCCF3), // obj2
    0x8E, 0x8F, 0x0C, TWOBYTES(0xCCEA), // obj3
    0xA6, 0x8F, 0x10, TWOBYTES(0xCCE8), // obj4
    0xC2, 0x8F, 0x04, TWOBYTES(0xCCE6), // obj5
    0x7C, 0x7D, 0x18, TWOBYTES(0xCD18), // obj6
    0xF0, 0x7B, 0x18, TWOBYTES(0xCD49), // obj7
    0xEA, 0x8F, 0x14, TWOBYTES(0xCCE4), // obj8

    // $CCE4-$CD4E: scene 0 object script byte-code
    OSS_OP_WAIT(5), // $CCE4: [obj8]
    OSS_OP_WAIT(5), // $CCE6: [obj5,obj8]
    OSS_OP_WAIT(5), // $CCE8: [obj4,obj5,obj8]
    OSS_OP_WAIT(10), // $CCEA: [obj3,obj4,obj5,obj8]
    OSS_OP_WAIT(5), // $CCEC: [obj1,obj3,obj4,obj5,obj8]
    OSS_OP_VELOCITY(-2, -4, 23), // $CCEE: [obj0,obj1,obj3,obj4,obj5,obj8]
    OSS_OP_DEAD(), // $CCF2: object frozen here [obj0,obj1,obj3,obj4,obj5,obj8]
    OSS_OP_WAIT(10), // $CCF3: [obj2]
    OSS_OP_VELOCITY(-2, -4, 23), // $CCF5: [obj2]
    OSS_OP_WAIT(47), // $CCF9: [obj2]
    0x28, // $CCFB: immediate step dx=+0 dy=-2 [obj2]
    OSS_OP_SET_ROW(28), // $CCFC: [obj2]
    OSS_OP_WAIT(2), // $CCFE: [obj2]
    0x38, // $CD00: immediate step dx=+0 dy=-6 [obj2]
    OSS_OP_SET_ROW(29), // $CD01: [obj2]
    OSS_OP_WAIT(2), // $CD03: [obj2]
    0x28, // $CD05: immediate step dx=+0 dy=-2 [obj2]
    OSS_OP_SET_ROW(30), // $CD06: [obj2]
    OSS_OP_WAIT(2), // $CD08: [obj2]
    0x08, // $CD0A: immediate step dx=+0 dy=+2 [obj2]
    OSS_OP_SET_ROW(29), // $CD0B: [obj2]
    OSS_OP_WAIT(2), // $CD0D: [obj2]
    0x18, // $CD0F: immediate step dx=+0 dy=+6 [obj2]
    OSS_OP_SET_ROW(28), // $CD10: [obj2]
    OSS_OP_WAIT(2), // $CD12: [obj2]
    0x08, // $CD14: immediate step dx=+0 dy=+2 [obj2]
    OSS_OP_SET_ROW(8), // $CD15: [obj2]
    OSS_OP_DEAD(), // $CD17: object frozen here [obj2]
    OSS_OP_WAIT(63), // $CD18: [obj6]
    OSS_OP_DECEL_X(3, 18, -2), // $CD1A: [obj6]
    OSS_OP_WAIT(1), // $CD1E: [obj6]
    0x38, // $CD20: immediate step dx=+0 dy=-6 [obj6]
    OSS_OP_WAIT(2), // $CD21: [obj6]
    0x30, // $CD23: immediate step dx=+0 dy=-4 [obj6]
    OSS_OP_WAIT(2), // $CD24: [obj6]
    0x10, // $CD26: immediate step dx=+0 dy=+4 [obj6]
    OSS_OP_WAIT(2), // $CD27: [obj6]
    0x18, // $CD29: immediate step dx=+0 dy=+6 [obj6]
    OSS_OP_DECEL_Y(18, 19, 2), // $CD2A: [obj6]
    0x01, // $CD2E: immediate step dx=+2 dy=+0 [obj6]
    OSS_OP_WAIT(1), // $CD2F: [obj6]
    0x01, // $CD31: immediate step dx=+2 dy=+0 [obj6]
    OSS_OP_WAIT(1), // $CD32: [obj6]
    OSS_OP_DECEL_X(0, 22, 2), // $CD34: [obj6]
    OSS_OP_DECEL_Y(12, 1, 0), // $CD38: [obj6]
    OSS_OP_DECEL_Y(11, 9, 2), // $CD3C: [obj6]
    OSS_OP_DECEL_X(1, 11, 2), // $CD40: [obj6]
    OSS_OP_DECEL_X(12, 1, 0), // $CD44: [obj6]
    OSS_OP_END_SCRIPT(), // $CD48: [obj6]
    OSS_OP_WAIT(131), // $CD49: [obj7]
    OSS_OP_JUMP_POSITION(-82, 31), // $CD4B: [obj7]
    OSS_OP_DEAD(), // $CD4E: object frozen here [obj7]

    // $CD4F: scene 1 -- 9 object records (x, y, row, script ptr)
    0x3A, 0x7B, 0x27, TWOBYTES(0xCD7C), // obj0
    0x3A, 0x7B, 0x33, TWOBYTES(0xCDA4), // obj1
    0x3A, 0x7B, 0x3F, TWOBYTES(0xCDD1), // obj2
    0x3A, 0x7B, 0x4B, TWOBYTES(0xCDFE), // obj3
    0x3A, 0x7B, 0x57, TWOBYTES(0xCE2B), // obj4
    0x3A, 0x7B, 0x33, TWOBYTES(0xCE5C), // obj5
    0x3A, 0x75, 0x6F, TWOBYTES(0xCE89), // obj6
    0x3A, 0x75, 0x6F, TWOBYTES(0xCEE3), // obj7
    0x3A, 0x7B, 0x63, TWOBYTES(0xCEB6), // obj8

    // $CD7C-$CF0F: scene 1 object script byte-code
    OSS_OP_VELOCITY(4, -4, 20), // $CD7C: [obj0]
    OSS_OP_WAIT(20), // $CD80: [obj0]
    OSS_OP_VELOCITY(-3, 3, 17), // $CD82: [obj0]
    OSS_OP_VELOCITY(-3, 1, 6), // $CD86: [obj0]
    0x06, // $CD8A: immediate step dx=-4 dy=+0 [obj0]
    OSS_OP_SET_ROW(35), // $CD8B: [obj0]
    OSS_OP_VELOCITY(-3, -4, 3), // $CD8D: [obj0]
    0x06, // $CD91: immediate step dx=-4 dy=+0 [obj0]
    OSS_OP_SET_ROW(31), // $CD92: [obj0]
    OSS_OP_VELOCITY(-3, -4, 3), // $CD94: [obj0]
    0x06, // $CD98: immediate step dx=-4 dy=+0 [obj0]
    OSS_OP_SET_ROW(0), // $CD99: [obj0]
    OSS_OP_VELOCITY(-3, -4, 3), // $CD9B: [obj0]
    OSS_OP_VELOCITY(-2, -4, 3), // $CD9F: [obj0]
    OSS_OP_END_SCRIPT(), // $CDA3: [obj0]
    OSS_OP_WAIT(2), // $CDA4: [obj1]
    OSS_OP_VELOCITY(4, -4, 20), // $CDA6: [obj1]
    OSS_OP_WAIT(18), // $CDAA: [obj1]
    OSS_OP_VELOCITY(-3, 4, 6), // $CDAC: [obj1]
    OSS_OP_VELOCITY(-2, 3, 11), // $CDB0: [obj1]
    OSS_OP_VELOCITY(-2, 1, 6), // $CDB4: [obj1]
    0x06, // $CDB8: immediate step dx=-4 dy=+0 [obj1]
    OSS_OP_SET_ROW(47), // $CDB9: [obj1]
    OSS_OP_VELOCITY(-2, -4, 3), // $CDBB: [obj1]
    0x06, // $CDBF: immediate step dx=-4 dy=+0 [obj1]
    OSS_OP_SET_ROW(43), // $CDC0: [obj1]
    OSS_OP_VELOCITY(-2, -4, 3), // $CDC2: [obj1]
    0x06, // $CDC6: immediate step dx=-4 dy=+0 [obj1]
    OSS_OP_SET_ROW(4), // $CDC7: [obj1]
    OSS_OP_VELOCITY(-3, -5, 2), // $CDC9: [obj1]
    OSS_OP_VELOCITY(-2, -5, 4), // $CDCD: [obj1]
    OSS_OP_WAIT(4), // $CDD1: [obj1,obj2]
    OSS_OP_VELOCITY(4, -4, 20), // $CDD3: [obj1,obj2]
    OSS_OP_WAIT(16), // $CDD7: [obj1,obj2]
    OSS_OP_VELOCITY(-2, 4, 11), // $CDD9: [obj1,obj2]
    OSS_OP_VELOCITY(-1, 3, 6), // $CDDD: [obj1,obj2]
    OSS_OP_VELOCITY(-1, 1, 6), // $CDE1: [obj1,obj2]
    0x06, // $CDE5: immediate step dx=-4 dy=+0 [obj1,obj2]
    OSS_OP_SET_ROW(59), // $CDE6: [obj1,obj2]
    OSS_OP_VELOCITY(-2, -4, 3), // $CDE8: [obj1,obj2]
    0x06, // $CDEC: immediate step dx=-4 dy=+0 [obj1,obj2]
    OSS_OP_SET_ROW(55), // $CDED: [obj1,obj2]
    OSS_OP_VELOCITY(-2, -4, 3), // $CDEF: [obj1,obj2]
    0x06, // $CDF3: immediate step dx=-4 dy=+0 [obj1,obj2]
    OSS_OP_SET_ROW(8), // $CDF4: [obj1,obj2]
    OSS_OP_VELOCITY(-2, -5, 1), // $CDF6: [obj1,obj2]
    OSS_OP_VELOCITY(-1, -6, 5), // $CDFA: [obj1,obj2]
    OSS_OP_WAIT(6), // $CDFE: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(4, -4, 20), // $CE00: [obj1,obj2,obj3]
    OSS_OP_WAIT(14), // $CE04: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(-1, 4, 13), // $CE06: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(-1, 3, 4), // $CE0A: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(-1, 1, 6), // $CE0E: [obj1,obj2,obj3]
    0x06, // $CE12: immediate step dx=-4 dy=+0 [obj1,obj2,obj3]
    OSS_OP_SET_ROW(71), // $CE13: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(-1, -4, 3), // $CE15: [obj1,obj2,obj3]
    0x06, // $CE19: immediate step dx=-4 dy=+0 [obj1,obj2,obj3]
    OSS_OP_SET_ROW(67), // $CE1A: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(-1, -4, 3), // $CE1C: [obj1,obj2,obj3]
    0x06, // $CE20: immediate step dx=-4 dy=+0 [obj1,obj2,obj3]
    OSS_OP_SET_ROW(12), // $CE21: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(0, -6, 5), // $CE23: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(0, -7, 1), // $CE27: [obj1,obj2,obj3]
    OSS_OP_WAIT(8), // $CE2B: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(4, -4, 20), // $CE2D: [obj1,obj2,obj3,obj4]
    OSS_OP_WAIT(12), // $CE31: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(0, 4, 11), // $CE33: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(0, 4, 4), // $CE37: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(0, 2, 2), // $CE3B: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(-1, 1, 6), // $CE3F: [obj1,obj2,obj3,obj4]
    0x06, // $CE43: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4]
    OSS_OP_SET_ROW(83), // $CE44: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(0, -3, 3), // $CE46: [obj1,obj2,obj3,obj4]
    0x06, // $CE4A: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4]
    OSS_OP_SET_ROW(79), // $CE4B: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(0, -4, 3), // $CE4D: [obj1,obj2,obj3,obj4]
    0x06, // $CE51: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4]
    OSS_OP_SET_ROW(16), // $CE52: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(0, -6, 4), // $CE54: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(0, -8, 2), // $CE58: [obj1,obj2,obj3,obj4]
    OSS_OP_WAIT(10), // $CE5C: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(4, -4, 20), // $CE5E: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_WAIT(10), // $CE62: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(1, 4, 13), // $CE64: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(1, 3, 4), // $CE68: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(1, 1, 6), // $CE6C: [obj1,obj2,obj3,obj4,obj5]
    0x06, // $CE70: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_SET_ROW(47), // $CE71: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(0, -4, 3), // $CE73: [obj1,obj2,obj3,obj4,obj5]
    0x06, // $CE77: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_SET_ROW(43), // $CE78: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(0, -4, 3), // $CE7A: [obj1,obj2,obj3,obj4,obj5]
    0x06, // $CE7E: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_SET_ROW(4), // $CE7F: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(0, -6, 5), // $CE81: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(0, -7, 1), // $CE85: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_WAIT(12), // $CE89: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(4, -4, 20), // $CE8B: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_WAIT(8), // $CE8F: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(2, 4, 11), // $CE91: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(1, 3, 6), // $CE95: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(1, 0, 6), // $CE99: [obj1,obj2,obj3,obj4,obj5,obj6]
    0x06, // $CE9D: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_SET_ROW(107), // $CE9E: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(2, -3, 3), // $CEA0: [obj1,obj2,obj3,obj4,obj5,obj6]
    0x06, // $CEA4: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_SET_ROW(103), // $CEA5: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(2, -6, 3), // $CEA7: [obj1,obj2,obj3,obj4,obj5,obj6]
    0x06, // $CEAB: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_SET_ROW(24), // $CEAC: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(1, -6, 2), // $CEAE: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(0, -7, 4), // $CEB2: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_WAIT(14), // $CEB6: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(4, -4, 20), // $CEB8: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_WAIT(6), // $CEBC: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(3, 4, 6), // $CEBE: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(2, 3, 11), // $CEC2: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(1, 1, 6), // $CEC6: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    0x06, // $CECA: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_SET_ROW(95), // $CECB: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(1, -3, 3), // $CECD: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    0x06, // $CED1: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_SET_ROW(91), // $CED2: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(1, -4, 3), // $CED4: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    0x06, // $CED8: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_SET_ROW(20), // $CED9: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(2, -5, 3), // $CEDB: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(1, -6, 3), // $CEDF: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_WAIT(16), // $CEE3: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(4, -4, 20), // $CEE5: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_WAIT(4), // $CEE9: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(3, 3, 17), // $CEEB: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(1, 1, 6), // $CEEF: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    0x06, // $CEF3: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_SET_ROW(107), // $CEF4: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(2, -4, 3), // $CEF6: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    0x06, // $CEFA: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_SET_ROW(103), // $CEFB: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(3, -6, 3), // $CEFD: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    0x06, // $CF01: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_SET_ROW(24), // $CF02: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(3, -6, 3), // $CF04: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(1, -5, 2), // $CF08: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(1, -4, 1), // $CF0C: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]

    // $CF10: scene 2 -- 9 object records (x, y, row, script ptr)
    0xE2, 0x34, 0x00, TWOBYTES(0xCF3D), // obj0
    0x3A, 0x8F, 0x04, TWOBYTES(0xCF4C), // obj1
    0x3A, 0x8F, 0x08, TWOBYTES(0xCF5C), // obj2
    0x3A, 0x8F, 0x0C, TWOBYTES(0xCF6C), // obj3
    0x3A, 0x8F, 0x10, TWOBYTES(0xCF7C), // obj4
    0x3A, 0x8F, 0x04, TWOBYTES(0xCF8C), // obj5
    0x3A, 0x89, 0x18, TWOBYTES(0xCF9C), // obj6
    0x3A, 0x89, 0x18, TWOBYTES(0xCFBD), // obj7
    0x3A, 0x8F, 0x14, TWOBYTES(0xCFAC), // obj8

    // $CF3D-$CFCC: scene 2 object script byte-code
    OSS_OP_ACCEL_X_A(33, 34, 0), // $CF3D: [obj0]
    OSS_OP_WAIT(28), // $CF41: [obj0]
    OSS_OP_ACCEL_X_A(10, 11, 0), // $CF43: [obj0]
    OSS_OP_ACCEL_X_C(0, 11, 0), // $CF47: [obj0]
    OSS_OP_DEAD(), // $CF4B: object frozen here [obj0]
    OSS_OP_WAIT(4), // $CF4C: [obj1]
    OSS_OP_JUMP_POSITION(-36, 52), // $CF4E: [obj1]
    OSS_OP_ACCEL_X_A(32, 33, 0), // $CF51: [obj1]
    OSS_OP_WAIT(47), // $CF55: [obj1]
    OSS_OP_ACCEL_X_B(11, 7, 0), // $CF57: [obj1]
    OSS_OP_DEAD(), // $CF5B: object frozen here [obj1]
    OSS_OP_WAIT(8), // $CF5C: [obj2]
    OSS_OP_JUMP_POSITION(-36, 52), // $CF5E: [obj2]
    OSS_OP_ACCEL_X_A(31, 32, 0), // $CF61: [obj2]
    OSS_OP_WAIT(44), // $CF65: [obj2]
    OSS_OP_ACCEL_X_B(13, 14, 0), // $CF67: [obj2]
    OSS_OP_DEAD(), // $CF6B: object frozen here [obj2]
    OSS_OP_WAIT(12), // $CF6C: [obj3]
    OSS_OP_JUMP_POSITION(-36, 52), // $CF6E: [obj3]
    OSS_OP_ACCEL_X_A(30, 31, 0), // $CF71: [obj3]
    OSS_OP_WAIT(41), // $CF75: [obj3]
    OSS_OP_ACCEL_X_B(16, 12, 0), // $CF77: [obj3]
    OSS_OP_DEAD(), // $CF7B: object frozen here [obj3]
    OSS_OP_WAIT(16), // $CF7C: [obj4]
    OSS_OP_JUMP_POSITION(-36, 52), // $CF7E: [obj4]
    OSS_OP_ACCEL_X_A(29, 30, 0), // $CF81: [obj4]
    OSS_OP_WAIT(38), // $CF85: [obj4]
    OSS_OP_ACCEL_X_B(18, 14, 0), // $CF87: [obj4]
    OSS_OP_DEAD(), // $CF8B: object frozen here [obj4]
    OSS_OP_WAIT(20), // $CF8C: [obj5]
    OSS_OP_JUMP_POSITION(-34, 52), // $CF8E: [obj5]
    OSS_OP_ACCEL_X_A(28, 29, 0), // $CF91: [obj5]
    OSS_OP_WAIT(35), // $CF95: [obj5]
    OSS_OP_ACCEL_X_B(20, 17, 0), // $CF97: [obj5]
    OSS_OP_DEAD(), // $CF9B: object frozen here [obj5]
    OSS_OP_WAIT(24), // $CF9C: [obj6]
    OSS_OP_JUMP_POSITION(-34, 32), // $CF9E: [obj6]
    OSS_OP_ACCEL_X_A(27, 28, 0), // $CFA1: [obj6]
    OSS_OP_WAIT(32), // $CFA5: [obj6]
    OSS_OP_ACCEL_X_B(22, 19, 0), // $CFA7: [obj6]
    OSS_OP_DEAD(), // $CFAB: object frozen here [obj6]
    OSS_OP_WAIT(28), // $CFAC: [obj8]
    OSS_OP_JUMP_POSITION(-34, 52), // $CFAE: [obj8]
    OSS_OP_ACCEL_X_A(26, 27, 0), // $CFB1: [obj8]
    OSS_OP_WAIT(29), // $CFB5: [obj8]
    OSS_OP_ACCEL_X_B(22, 18, 0), // $CFB7: [obj8]
    0x01, // $CFBB: immediate step dx=+2 dy=+0 [obj8]
    OSS_OP_DEAD(), // $CFBC: object frozen here [obj8]
    OSS_OP_WAIT(32), // $CFBD: [obj7]
    OSS_OP_JUMP_POSITION(-34, 32), // $CFBF: [obj7]
    OSS_OP_ACCEL_X_A(25, 26, 0), // $CFC2: [obj7]
    OSS_OP_WAIT(26), // $CFC6: [obj7]
    OSS_OP_ACCEL_X_B(24, 20, 0), // $CFC8: [obj7]
    OSS_OP_END_SCRIPT(), // $CFCC: [obj7]

    // $CFCD: scene 3 -- 9 object records (x, y, row, script ptr)
    0x5A, 0x76, 0x27, TWOBYTES(0xCFFA), // obj0
    0x62, 0x76, 0x33, TWOBYTES(0xD024), // obj1
    0x6A, 0x76, 0x3F, TWOBYTES(0xD04D), // obj2
    0x72, 0x76, 0x4B, TWOBYTES(0xD076), // obj3
    0x7A, 0x76, 0x57, TWOBYTES(0xD09F), // obj4
    0x86, 0x76, 0x33, TWOBYTES(0xD0C8), // obj5
    0x90, 0x70, 0x6F, TWOBYTES(0xD0F1), // obj6
    0x9E, 0x70, 0x6F, TWOBYTES(0xD143), // obj7
    0x94, 0x76, 0x63, TWOBYTES(0xD11A), // obj8

    // $CFFA-$D16B: scene 3 object script byte-code
    OSS_OP_VELOCITY(0, -4, 17), // $CFFA: [obj0]
    OSS_OP_VELOCITY(0, -3, 6), // $CFFE: [obj0]
    OSS_OP_VELOCITY(-2, -2, 4), // $D002: [obj0]
    OSS_OP_SET_ROW(35), // $D006: [obj0]
    0x16, // $D008: immediate step dx=-4 dy=+4 [obj0]
    OSS_OP_VELOCITY(-2, -1, 3), // $D009: [obj0]
    OSS_OP_VELOCITY(-3, 1, 2), // $D00D: [obj0]
    OSS_OP_SET_ROW(31), // $D011: [obj0]
    0x16, // $D013: immediate step dx=-4 dy=+4 [obj0]
    OSS_OP_VELOCITY(-4, 1, 2), // $D014: [obj0]
    OSS_OP_VELOCITY(-4, 2, 3), // $D018: [obj0]
    OSS_OP_SET_ROW(0), // $D01C: [obj0]
    0x16, // $D01E: immediate step dx=-4 dy=+4 [obj0]
    OSS_OP_VELOCITY(-4, 3, 3), // $D01F: [obj0]
    OSS_OP_END_SCRIPT(), // $D023: [obj0]
    OSS_OP_VELOCITY(0, -4, 17), // $D024: [obj1]
    OSS_OP_VELOCITY(0, -3, 6), // $D028: [obj1]
    OSS_OP_VELOCITY(-1, -2, 4), // $D02C: [obj1]
    OSS_OP_SET_ROW(47), // $D030: [obj1]
    0x16, // $D032: immediate step dx=-4 dy=+4 [obj1]
    OSS_OP_VELOCITY(-2, -1, 3), // $D033: [obj1]
    OSS_OP_VELOCITY(-2, 1, 2), // $D037: [obj1]
    OSS_OP_SET_ROW(43), // $D03B: [obj1]
    0x16, // $D03D: immediate step dx=-4 dy=+4 [obj1]
    OSS_OP_VELOCITY(-2, 1, 2), // $D03E: [obj1]
    OSS_OP_VELOCITY(-3, 2, 3), // $D042: [obj1]
    OSS_OP_SET_ROW(4), // $D046: [obj1]
    0x16, // $D048: immediate step dx=-4 dy=+4 [obj1]
    OSS_OP_VELOCITY(-3, 3, 3), // $D049: [obj1]
    OSS_OP_VELOCITY(0, -4, 17), // $D04D: [obj1,obj2]
    OSS_OP_VELOCITY(0, -3, 6), // $D051: [obj1,obj2]
    OSS_OP_VELOCITY(0, -2, 4), // $D055: [obj1,obj2]
    OSS_OP_SET_ROW(59), // $D059: [obj1,obj2]
    0x16, // $D05B: immediate step dx=-4 dy=+4 [obj1,obj2]
    OSS_OP_VELOCITY(-1, -1, 3), // $D05C: [obj1,obj2]
    OSS_OP_VELOCITY(-1, 1, 2), // $D060: [obj1,obj2]
    OSS_OP_SET_ROW(55), // $D064: [obj1,obj2]
    0x16, // $D066: immediate step dx=-4 dy=+4 [obj1,obj2]
    OSS_OP_VELOCITY(-1, 1, 2), // $D067: [obj1,obj2]
    OSS_OP_VELOCITY(-2, 2, 3), // $D06B: [obj1,obj2]
    OSS_OP_SET_ROW(8), // $D06F: [obj1,obj2]
    0x16, // $D071: immediate step dx=-4 dy=+4 [obj1,obj2]
    OSS_OP_VELOCITY(-2, 3, 3), // $D072: [obj1,obj2]
    OSS_OP_VELOCITY(0, -4, 17), // $D076: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(0, -3, 6), // $D07A: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(0, -2, 4), // $D07E: [obj1,obj2,obj3]
    OSS_OP_SET_ROW(71), // $D082: [obj1,obj2,obj3]
    0x16, // $D084: immediate step dx=-4 dy=+4 [obj1,obj2,obj3]
    OSS_OP_VELOCITY(0, -1, 3), // $D085: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(0, 1, 2), // $D089: [obj1,obj2,obj3]
    OSS_OP_SET_ROW(67), // $D08D: [obj1,obj2,obj3]
    0x16, // $D08F: immediate step dx=-4 dy=+4 [obj1,obj2,obj3]
    OSS_OP_VELOCITY(0, 1, 2), // $D090: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(0, 2, 3), // $D094: [obj1,obj2,obj3]
    OSS_OP_SET_ROW(12), // $D098: [obj1,obj2,obj3]
    0x16, // $D09A: immediate step dx=-4 dy=+4 [obj1,obj2,obj3]
    OSS_OP_VELOCITY(-1, 3, 3), // $D09B: [obj1,obj2,obj3]
    OSS_OP_VELOCITY(0, -4, 17), // $D09F: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(0, -3, 6), // $D0A3: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(0, -2, 4), // $D0A7: [obj1,obj2,obj3,obj4]
    OSS_OP_SET_ROW(83), // $D0AB: [obj1,obj2,obj3,obj4]
    0x16, // $D0AD: immediate step dx=-4 dy=+4 [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(1, -1, 3), // $D0AE: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(1, 1, 2), // $D0B2: [obj1,obj2,obj3,obj4]
    OSS_OP_SET_ROW(79), // $D0B6: [obj1,obj2,obj3,obj4]
    0x16, // $D0B8: immediate step dx=-4 dy=+4 [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(1, 1, 2), // $D0B9: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(1, 2, 3), // $D0BD: [obj1,obj2,obj3,obj4]
    OSS_OP_SET_ROW(16), // $D0C1: [obj1,obj2,obj3,obj4]
    0x16, // $D0C3: immediate step dx=-4 dy=+4 [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(1, 3, 3), // $D0C4: [obj1,obj2,obj3,obj4]
    OSS_OP_VELOCITY(0, -4, 17), // $D0C8: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(0, -3, 6), // $D0CC: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(1, -2, 4), // $D0D0: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_SET_ROW(47), // $D0D4: [obj1,obj2,obj3,obj4,obj5]
    0x16, // $D0D6: immediate step dx=-4 dy=+4 [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(1, -1, 3), // $D0D7: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(2, 1, 2), // $D0DB: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_SET_ROW(43), // $D0DF: [obj1,obj2,obj3,obj4,obj5]
    0x16, // $D0E1: immediate step dx=-4 dy=+4 [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(2, 1, 2), // $D0E2: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(2, 2, 3), // $D0E6: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_SET_ROW(4), // $D0EA: [obj1,obj2,obj3,obj4,obj5]
    0x16, // $D0EC: immediate step dx=-4 dy=+4 [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(3, 3, 3), // $D0ED: [obj1,obj2,obj3,obj4,obj5]
    OSS_OP_VELOCITY(0, -4, 17), // $D0F1: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(0, -3, 6), // $D0F5: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(1, -2, 4), // $D0F9: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_SET_ROW(107), // $D0FD: [obj1,obj2,obj3,obj4,obj5,obj6]
    0x06, // $D0FF: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(2, -1, 3), // $D100: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(2, 1, 2), // $D104: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_SET_ROW(103), // $D108: [obj1,obj2,obj3,obj4,obj5,obj6]
    0x05, // $D10A: immediate step dx=-2 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(2, 1, 2), // $D10B: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(3, 2, 3), // $D10F: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_SET_ROW(24), // $D113: [obj1,obj2,obj3,obj4,obj5,obj6]
    0x29, // $D115: immediate step dx=+2 dy=-2 [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(4, 3, 3), // $D116: [obj1,obj2,obj3,obj4,obj5,obj6]
    OSS_OP_VELOCITY(0, -4, 17), // $D11A: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(0, -3, 6), // $D11E: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(2, -2, 4), // $D122: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_SET_ROW(95), // $D126: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    0x16, // $D128: immediate step dx=-4 dy=+4 [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(2, -1, 3), // $D129: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(2, 1, 2), // $D12D: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_SET_ROW(91), // $D131: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    0x16, // $D133: immediate step dx=-4 dy=+4 [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(3, 1, 2), // $D134: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(4, 2, 3), // $D138: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_SET_ROW(20), // $D13C: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    0x16, // $D13E: immediate step dx=-4 dy=+4 [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(6, 3, 3), // $D13F: [obj1,obj2,obj3,obj4,obj5,obj6,obj8]
    OSS_OP_VELOCITY(0, -4, 17), // $D143: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(0, -3, 6), // $D147: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(2, -2, 4), // $D14B: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_SET_ROW(107), // $D14F: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    0x06, // $D151: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(2, -1, 3), // $D152: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(3, 1, 2), // $D156: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_SET_ROW(103), // $D15A: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    0x06, // $D15C: immediate step dx=-4 dy=+0 [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(4, 1, 2), // $D15D: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(5, 2, 3), // $D161: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_SET_ROW(24), // $D165: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    0x2A, // $D167: immediate step dx=+4 dy=-2 [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]
    OSS_OP_VELOCITY(6, 3, 3), // $D168: [obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8]

    // $D16C: scene 4 -- 9 object records (x, y, row, script ptr)
    0x18, 0x8F, 0x00, TWOBYTES(SCENE4_SCRIPT_SHARED_ADDR), // obj0
    0x30, 0x8F, 0x04, TWOBYTES(0xD241), // obj1
    0x48, 0x8F, 0x08, TWOBYTES(0xD1AE), // obj2
    0x60, 0x8F, 0x0C, TWOBYTES(SCENE4_SCRIPT_SHARED_ADDR), // obj3
    0x78, 0x8F, 0x10, TWOBYTES(SCENE4_SCRIPT_SHARED_ADDR), // obj4
    0x94, 0x8F, 0x04, TWOBYTES(SCENE4_SCRIPT_SHARED_ADDR), // obj5
    0x56, 0x7B, 0x18, TWOBYTES(0xD1FF), // obj6
    0x56, 0x7A, 0x18, TWOBYTES(0xD220), // obj7
    0xBA, 0x8F, 0x14, TWOBYTES(SCENE4_SCRIPT_SHARED_ADDR), // obj8

    // $D199-$D271: scene 4 object script byte-code
    OSS_OP_VELOCITY(0, -6, 15), // $D199: [obj0,obj3,obj4,obj5,obj8]
    OSS_OP_DECEL_Y(15, 16, 0), // $D19D: [obj0,obj3,obj4,obj5,obj8]
    OSS_OP_DECEL_X(0, 16, 0), // $D1A1: [obj0,obj3,obj4,obj5,obj8]
    OSS_OP_DECEL_Y(10, 11, 0), // $D1A5: [obj0,obj3,obj4,obj5,obj8]
    OSS_OP_DECEL_X(0, 11, 0), // $D1A9: [obj0,obj3,obj4,obj5,obj8]
    OSS_OP_DEAD(), // $D1AD: object frozen here [obj0,obj3,obj4,obj5,obj8]
    OSS_OP_VELOCITY(0, -6, 15), // $D1AE: [obj2]
    OSS_OP_DECEL_Y(15, 16, 0), // $D1B2: [obj2]
    OSS_OP_DECEL_X(0, 16, 0), // $D1B6: [obj2]
    OSS_OP_DECEL_Y(10, 11, 0), // $D1BA: [obj2]
    OSS_OP_DECEL_X(0, 11, 0), // $D1BE: [obj2]
    OSS_OP_WAIT(12), // $D1C2: [obj2]
    0x28, // $D1C4: immediate step dx=+0 dy=-2 [obj2]
    OSS_OP_SET_ROW(28), // $D1C5: [obj2]
    OSS_OP_WAIT(2), // $D1C7: [obj2]
    0x38, // $D1C9: immediate step dx=+0 dy=-6 [obj2]
    OSS_OP_SET_ROW(29), // $D1CA: [obj2]
    OSS_OP_WAIT(2), // $D1CC: [obj2]
    0x28, // $D1CE: immediate step dx=+0 dy=-2 [obj2]
    OSS_OP_SET_ROW(30), // $D1CF: [obj2]
    OSS_OP_WAIT(2), // $D1D1: [obj2]
    0x08, // $D1D3: immediate step dx=+0 dy=+2 [obj2]
    OSS_OP_SET_ROW(29), // $D1D4: [obj2]
    OSS_OP_WAIT(2), // $D1D6: [obj2]
    0x18, // $D1D8: immediate step dx=+0 dy=+6 [obj2]
    OSS_OP_SET_ROW(28), // $D1D9: [obj2]
    OSS_OP_WAIT(2), // $D1DB: [obj2]
    0x08, // $D1DD: immediate step dx=+0 dy=+2 [obj2]
    OSS_OP_SET_ROW(8), // $D1DE: [obj2]
    OSS_OP_WAIT(35), // $D1E0: [obj2]
    0x28, // $D1E2: immediate step dx=+0 dy=-2 [obj2]
    OSS_OP_SET_ROW(28), // $D1E3: [obj2]
    OSS_OP_WAIT(2), // $D1E5: [obj2]
    0x38, // $D1E7: immediate step dx=+0 dy=-6 [obj2]
    OSS_OP_SET_ROW(29), // $D1E8: [obj2]
    OSS_OP_WAIT(2), // $D1EA: [obj2]
    0x28, // $D1EC: immediate step dx=+0 dy=-2 [obj2]
    OSS_OP_SET_ROW(30), // $D1ED: [obj2]
    OSS_OP_WAIT(2), // $D1EF: [obj2]
    0x08, // $D1F1: immediate step dx=+0 dy=+2 [obj2]
    OSS_OP_SET_ROW(29), // $D1F2: [obj2]
    OSS_OP_WAIT(2), // $D1F4: [obj2]
    0x18, // $D1F6: immediate step dx=+0 dy=+6 [obj2]
    OSS_OP_SET_ROW(28), // $D1F7: [obj2]
    OSS_OP_WAIT(2), // $D1F9: [obj2]
    0x08, // $D1FB: immediate step dx=+0 dy=+2 [obj2]
    OSS_OP_SET_ROW(8), // $D1FC: [obj2]
    OSS_OP_DEAD(), // $D1FE: object frozen here [obj2]
    OSS_OP_WAIT(69), // $D1FF: [obj6]
    OSS_OP_VELOCITY(0, -6, 11), // $D201: [obj6]
    OSS_OP_WAIT(1), // $D205: [obj6]
    0x30, // $D207: immediate step dx=+0 dy=-4 [obj6]
    OSS_OP_WAIT(2), // $D208: [obj6]
    0x30, // $D20A: immediate step dx=+0 dy=-4 [obj6]
    OSS_OP_WAIT(2), // $D20B: [obj6]
    0x28, // $D20D: immediate step dx=+0 dy=-2 [obj6]
    OSS_OP_WAIT(2), // $D20E: [obj6]
    0x08, // $D210: immediate step dx=+0 dy=+2 [obj6]
    OSS_OP_WAIT(2), // $D211: [obj6]
    OSS_OP_DECEL_Y(15, 16, 0), // $D213: [obj6]
    OSS_OP_VELOCITY(6, -4, 13), // $D217: [obj6]
    OSS_OP_VELOCITY(10, -3, 1), // $D21B: [obj6]
    OSS_OP_DEAD(), // $D21F: object frozen here [obj6]
    OSS_OP_WAIT(114), // $D220: [obj7]
    OSS_OP_VELOCITY(0, -6, 11), // $D222: [obj7]
    OSS_OP_WAIT(1), // $D226: [obj7]
    0x30, // $D228: immediate step dx=+0 dy=-4 [obj7]
    OSS_OP_WAIT(2), // $D229: [obj7]
    0x30, // $D22B: immediate step dx=+0 dy=-4 [obj7]
    OSS_OP_WAIT(2), // $D22C: [obj7]
    0x28, // $D22E: immediate step dx=+0 dy=-2 [obj7]
    OSS_OP_WAIT(2), // $D22F: [obj7]
    0x08, // $D231: immediate step dx=+0 dy=+2 [obj7]
    OSS_OP_WAIT(2), // $D232: [obj7]
    OSS_OP_DECEL_Y(15, 16, 0), // $D234: [obj7]
    OSS_OP_VELOCITY(7, -3, 17), // $D238: [obj7]
    OSS_OP_VELOCITY(5, -3, 1), // $D23C: [obj7]
    OSS_OP_END_SCRIPT(), // $D240: [obj7]
    OSS_OP_VELOCITY(0, -6, 15), // $D241: [obj1]
    OSS_OP_DECEL_Y(15, 16, 0), // $D245: [obj1]
    OSS_OP_DECEL_X(0, 16, 0), // $D249: [obj1]
    OSS_OP_DECEL_Y(10, 11, 0), // $D24D: [obj1]
    OSS_OP_DECEL_X(0, 11, 0), // $D251: [obj1]
    OSS_OP_WAIT(23), // $D255: [obj1]
    OSS_OP_VELOCITY(-1, 6, 10), // $D257: [obj1]
    OSS_OP_VELOCITY(8, -4, 5), // $D25B: [obj1]
    OSS_OP_VELOCITY(-3, -4, 10), // $D25F: [obj1]
    OSS_OP_WAIT(20), // $D263: [obj1]
    OSS_OP_VELOCITY(-1, 6, 10), // $D265: [obj1]
    OSS_OP_VELOCITY(8, -4, 5), // $D269: [obj1]
    OSS_OP_VELOCITY(-3, -4, 10), // $D26D: [obj1]
    OSS_OP_DEAD(), // $D271: object frozen here [obj1]
  };

#undef SCENE4_SCRIPT_SHARED_ADDR

  /**
   * title_scene_table_offset
   *
   * Conv: byte offsets into title_scene_data for each scene's object-record
   *       block; not a Z80 table itself, so there is no single originating
   *       address -- each entry's corresponding scene start address is given
   *       inline below.
   */
  static const u16 title_scene_table_offset[TITLE_SCENE_COUNT] = {
    0x0000, /* $CCB7 */
    0x0098, /* $CD4F */
    0x0259, /* $CF10 */
    0x0316, /* $CFCD */
    0x04B5, /* $D16C */
  };
  // clang-format on

  u8        A_anim;         /* rotating anim-selector pseudo-random value (was A) */
  int       carry;          /* required by the RLC/RR macros (carry) */
  int       bit;            /* scene-table bit-test index, 0-3 (Conv: rolled RRA/JR C chain) */
  int       scene_idx;      /* chosen scene table index, 0-4 (Conv: rolled, no Z80 equivalent) */
  const u8 *HL_scene_table; /* chosen scene table's object-record base (was HL) */
  int       obj;            /* object-record loop index, 0-8 (was B) */
  u8        A_outcome;      /* titlescr_wait_loop outcome; restart or return (was A) */

  for (;;)
  {
    clear_and_fill_border_attrs(state);

    /* $C5A1-$C5A9: read the stored animation index, rotate (shift) it left,
     * mask to 5 bits, and force it to 1 if that leaves zero -- then store
     * the new value for the next restart. */
    A_anim = (state->bank3->title_animation << 1) & 0x1F;
    if (A_anim == 0)
      A_anim = 1;
    state->bank3->title_animation = A_anim;

    /* $C5AC-$C5C7: pick one of 5 scene tables by testing successive bits of
     * A via RRA; the first bit found set selects the table, defaulting to
     * the 5th if none of the low 4 bits are set.
     * Conv: the four unrolled "LD HL,addr / RRA / JR C" checks collapse to a
     * loop over the same 4 bit tests; behaviourally identical. */
    scene_idx = 4;
    carry     = 0; // Conv: RR reads carry-in only into the rotated value's
                   // bit 7, never the branch below (which tests bit 0 of
                   // the pre-rotation value); the loop's outcome does not
                   // depend on this, but the read must be well-defined.
    for (bit = 0; bit < 4; bit++)
    {
      RR(A_anim);
      if (carry)
      {
        scene_idx = bit;
        break;
      }
    }
    HL_scene_table = &title_scene_data[title_scene_table_offset[scene_idx]];

    /* $C5C8-$C5CB: draw the copyright/credits text block ($CC50). */
    print_string(state, title_screen_credits_text);

    /* $C5CE-$C602: zero the $BB00-$BB4F object array, then copy the chosen
     * scene's 9 5-byte object records into it, reordering each record's
     * bytes [x, y, row, script_lo, script_hi] into the object fields
     * x/y/row/script (confirmed against object_script_step's own (IX+n)
     * accesses at $C740-$C7AC, not the (misleading) inline comment at
     * $C5DB-$C5E3, which names the wrong offsets for the script pointer). */
    for (obj = 0; obj < 9; obj++)
    {
      state->bank3->title_objects[obj].opcode = 0;
      state->bank3->title_objects[obj].wait   = 0;
      state->bank3->title_objects[obj].x_step = 0;
      state->bank3->title_objects[obj].y_step = 0;

      state->bank3->title_objects[obj].x   = HL_scene_table[0];
      state->bank3->title_objects[obj].y   = HL_scene_table[1];
      state->bank3->title_objects[obj].row = HL_scene_table[2];

      state->bank3->title_objects[obj].script = &title_scene_data[wordat(HL_scene_table + 3) - TITLE_SCENE_DATA_BASE];

      HL_scene_table += 5;
    }

    setup_im2_interrupt_table(state);

    /* $C605 CALL $C6C4: on real hardware this call never returns under
     * normal play -- $C6C4 self-loops, animating every frame, until some
     * object's script hits its $D2 end-of-script opcode, which unwinds the
     * whole call chain straight back here (see object_script_step's and
     * titlescr_animate_frame's own Conv notes). Modelled as an explicit loop
     * rather than the stack-unwind trick itself: the scene animates for as
     * many frames as that takes, then this loop ends and the scene is left
     * on its final frame while the tune starts and the (non-animating) wait
     * loop below takes over. */
    while (titlescr_animate_frame(state))
      CHECK_HOST_QUIT(state);

    /* Show "PRESS ENTER FOR OPTIONS" unconditionally. */
    print_character(state, &title_screen_overlay_text[21]);

    /* Show "PRESS GEAR TO PLAY" once controls have been selected. */
    if (state->controls_selected)
      print_character(state, &title_screen_overlay_text[0]);

    titlescr_start_tune(state, 0);

    /* $C61C EI / $C61D HALT: sync to the next interrupt before entering the
     * wait loop, so the first frame drawn above is actually presented. */

    A_outcome = titlescr_wait_loop(state); /* $C61D falls through to $C61E */
    if (A_outcome != TITLESCR_RESTART)
      return A_outcome;
  }
}

/**
 * $C61E: Title-screen attract-mode wait loop
 *
 * Services sound (titlescr_music, the per-frame sound/music tick) and
 * polls for credit-insert / fire / any-key input to start the game or jump to
 * a fresh title screen. Does NOT animate the scene -- on real hardware this
 * loop body is just `CALL $F82F` with no call to $C6C4; by the time this
 * loop is reached, run_title_screen's own per-frame loop has already run
 * the scene's animation to completion (an object's script hit its $D2
 * end-of-script opcode) and the scene sits frozen on its final frame for
 * the rest of the attract-mode wait. Re-entered every frame via $C61E;
 * run_title_screen ($C59E) is re-run (new scene) when a key other than
 * fire is pressed.
 *
 * Conv: the Z80 has no HALT anywhere in this loop body -- the per-frame pacing
 *       described in the skool ("one $F82F service call per frame") is
 *       informal; the real hardware relies on the background IM2 interrupt
 *       firing asynchronously while this loop spins. The C port makes the frame
 *       boundary explicit with the stamp()/sleep() pair inside titlescr_music
 *       itself, one frame per call, matching every other per-frame loop in this
 *       file (attract_mode_128k, drive_attract_demo, run_pregame_screen_loop).
 *
 * Conv: DI/EI have no C equivalent (SDL owns interrupt delivery, matching
 *       setup_im2_interrupt_table) and are omitted throughout.
 *
 * Conv: the skool's inline comments name the wrong keyboard half-row at two
 *       sites -- $C638 says "SPACE (fire)" but loads $BF
 *       (port_KEYBOARD_ENTERLKJH, the ENTER/L/K/J/H row; bit 0 is ENTER), and
 *       $C681 says "ENTER" but loads $F7 (port_KEYBOARD_12345, the 1/2/3/4/5
 *       row). Both are translated here against the actual operand and its
 *       Spectrum.h port constant, not the skool's prose.
 *
 * \return TITLESCR_RESTART where the Z80 would have done `JP $C59E` to
 * restart the title screen -- the caller re-runs its own setup and calls this
 * again, rather than this function recursing into run_title_screen directly
 * (see its prologue for why). Otherwise this call ended via a genuine Z80 RET
 * and the value is the one the Z80 leaves in A for attract_mode_128k:
 * TITLESCR_ATTRACT from the tune-4-wait tail, TITLESCR_START_GAME from the
 * credit-inserted tail.
 */
static u8 titlescr_wait_loop(chqstate_t *state)
{
  int B_wait;   /* tune-4 wait countdown, 180 frames (was B) */
  u8  A_fire;   /* ENTER/L/K/J/H half-row, tested for fire (was A) */
  u8  A_key6;   /* 0/9/8/7/6 half-row, tested for the "6" key (was A) */
  u8  A_anykey; /* 1/2/3/4/5 half-row, tested for any key (was A) */
  int carry;    /* required by the RRC macro, unused (carry) */

  for (;;)
  {
    CHECK_HOST_QUIT(state);

    titlescr_music(state);

    if (!state->bank3->title_music.tune_active)
    {
      /* $C627-$C637: wait out ~180 frames (one titlescr_music call per
       * iteration) before falling through to the credit/name-table refresh
       * tail at titlescr_refresh_name_table.
       *
       * Conv: $C629 calls $F7DB (load_drum_script), NOT $F7D6
       * (titlescr_start_tune's entry point) -- this is a distinct
       * entry point, used only from here, that skips the
       * PUSH AF/CALL $EB9E/POP AF tune-start prologue entirely and jumps
       * straight into the drum-script-table setup for tune #4's cue table.
       * It must NOT call titlescr_start_ay (that would incorrectly arm
       * state->title_music.tune_active). */

      load_drum_script(state, 4);

      B_wait = ATTRACT_TUNE_WAIT_FRAMES;
      do
        titlescr_music(state);
      while (--B_wait);
      /* $C635 INC B (B wraps 0 -> 1) has no further use of B afterwards --
       * Conv: DJNZ bookkeeping, omitted. */

      titlescr_refresh_name_table(state);

      return TITLESCR_ATTRACT;
      /* $C6C3 RET -- returns to run_title_screen's own caller with A = B,
       * which $C635 INC B has just made 1, so attract_mode_128k's $F421
       * AND A / RET Z falls through into the attract demo. Conv: contrary
       * to the usual framing of this loop as unbounded, this path is a
       * genuine early exit in the Z80 -- see run_title_screen's prologue. */
    }

    /* ts_check_fire ($C638): fire (ENTER) check.
     * Conv: was IN+CPL+RRA; RRA only tests bit 0, so this is collapsed to a
     * direct bit-0 mask (cf. attract_mode_128k's ENTER check). */
    A_fire = ~state->speccy->in(state->speccy, port_KEYBOARD_ENTERLKJH);
    if (A_fire & 1)
    {
      omd_redraw_and_poll(state); /* hands off to the options menu, whose own
        * $FC11 JP $C59E re-runs the title screen. */
      return TITLESCR_RESTART;
    }

    /* $C641-$C64C: credit mode / credit-slot check. Conv: $8001 is the same
     * address as state->controls_selected; the skool's prose calls it a
     * "credit mode" flag here, which may be genuine double duty (arcade
     * credit configuration doubling as "controls selected") or just loose
     * wording -- either way this reuses the existing field rather than
     * inventing a second one. */
    if (state->controls_selected)
    {
      /* Conv: $800E is CALL $A0D6 (keyscan) in disguise -- see the skool's
       * $8008 entry. Bit 4 (USERINPUTFLAG_FIRE) is the credit-insert key,
       * matching the "confirm" bit used the same way by $800E's other
       * caller, name_entry_input. */
      if (keyscan(state) & USERINPUTFLAG_FIRE)
      {
        titlescr_credit_inserted(state);
        /* $C696 pushes $8011 before falling into the shared tail, so that
         * tail's $C6C3 RET lands on $8011 -- three bytes ($C3,$79,$9C) that
         * decode as `JP $9C79`, play_start_noise -- and only then returns to
         * run_title_screen's caller. This is the path that starts a game
         * from the title screen. */
        play_start_noise(state);
        return TITLESCR_START_GAME;
      }
    }

    /* $C64F-$C65C: test-mode "6" key check (skool prose calls these "1"/"2"
     * player-select keys, but the operand $EF is port_KEYBOARD_09876 and the
     * mask is $10 -- bit 4 of that row is the "6" key, not "1" or "2"). */
    /* $C652-$C653: AND A; JR Z,$C61E -- a loop-restart, not a skip-this-
     * block branch: $C61E is the loop head, so test_mode == 0 skips the
     * "any key" check below too, not just this one. */
    if (!state->test_mode)
      continue; /* Conv: no balancing sleep() needed -- already closed out by
                 * the stamp()/sleep() pair at the top of this loop */

    /* was IN+CPL */
    A_key6 = ~state->speccy->in(state->speccy, port_KEYBOARD_09876);
    if (A_key6 & 0x10)
    {
      /* $C65F-$C67B: seed a placeholder score of $87654321, stage 6,
       * 3 retries, check it against the high-score table, then restart. */
      state->score_bcd[0]        = 0x21;
      state->score_bcd[1]        = 0x43;
      state->score_bcd[2]        = 0x65;
      state->score_bcd[3]        = 0x87;
      state->wanted_stage_number = 6; // MAX?
      state->retry_count         = 3;

      stop_music_and_silence(state);

      check_high_score(state);

      return TITLESCR_RESTART; /* $C67E JP $C59E */
    }

    /* $C681-$C693: "any key" (1/2/3/4/5 row) check -- restarts the title
     * screen with a freshly seeded scene selector. */
    /* was IN+CPL */
    A_anykey = ~state->speccy->in(state->speccy, port_KEYBOARD_12345);
    if ((A_anykey & 0x1F) == 0)
      continue; /* Conv: no balancing sleep() needed -- see prologue */

    RRC(A_anykey);
    state->bank3->title_animation = A_anykey; /* $C5A2 (SM) */

    stop_music_and_silence(state);

    return TITLESCR_RESTART; /* $C693 JP $C59E */
  }
}

/**
 * $C696: Credit-inserted entry point
 *
 * Pushes $8011 as an extra "credit awarded" flag/value, then falls through
 * into the shared name-table refresh tail at titlescr_refresh_name_table.
 *
 * The $8011 push is *not* discarded by the shared tail's `POP AF` ($C6C2) --
 * that pops the BC pushed at $C69A. $8011 stays put and becomes the address
 * the tail's `RET` ($C6C3) jumps to: `JP $9C79` (play_start_noise), which then
 * returns to run_title_screen's own caller. Modelled by titlescr_wait_loop's
 * credit branch, which calls play_start_noise and returns TITLESCR_START_GAME
 * after this function returns.
 */
static void titlescr_credit_inserted(chqstate_t *state)
{
  titlescr_refresh_name_table(state); /* $C696-$C69A fallthrough */
}

/**
 * $C69A: Refresh the high-score name table
 *
 * Copies the 3 preset high-score name/rank rows from $C403 into the work
 * buffer pointed to by ($800A), each row split into 15+7+6 byte segments
 * with 2-byte gaps skipped between segments.
 *
 * Conv: stubbed per scope decision -- nothing in the C port yet models the
 *       destination buffer or a $800A-equivalent state field, and this task's
 *       State.h changes are handled separately. Reached both directly from
 *       titlescr_wait_loop's ~180-frame tune wait and via
 *       titlescr_credit_inserted; in the Z80 both paths end in a RET back to
 *       run_title_screen's own caller, which this function models simply by
 *       returning normally.
 */
static void titlescr_refresh_name_table(chqstate_t *state)
{
  /* TODO: copy 3 rows of high-score name/rank data from $C403 into the
   * buffer pointed to by ($800A) -- needs a destination buffer/state field,
   * out of scope for this task. */
  NOT_USED(state);
}

/* $FFE5-$FFE9: Sinclair Interface II joystick key-scan codes,
 * installed into state->control_keys[0..4] when "1. SINCLAIR JOYSTICK" is
 * chosen. Genuine emulation of the classic Interface II wiring (keys 6-0),
 * not arbitrary key choices. */
static const u8 sinclair_joystick_keys[5] = { 0x23, 0x1B, 0x13, 0x03, 0x0B };

/* $FFEA-$FFEE: Cursor/Protek joystick key-scan codes, installed
 * when "2. CURSOR JOYSTICK" is chosen (keys 5,6,7,8,0). */
static const u8 cursor_joystick_keys[5]  = { 0x23, 0x0B, 0x03, 0x04, 0x13 };

/* $FF95-$FFE4: key-name lookup table for the "redefine keys" screen (40
 * 2-byte entries: printable character + space, with SYMBOL SHIFT/SPACE/
 * ENTER/CAPS SHIFT spelled out as two-letter codes). Indexed by
 * read_new_key_definition via the same key/halfrow packing produced by
 * scan_keyboard_matrix. Byte-for-byte identical to the 48K version's key_names[]
 * ($EDD6, CommonData.c) -- kept as a separate array since it is a distinct
 * copy at a distinct bank-3 address in the original. */
static const u8 control_key_names[80] = {
  'B', ' ', 'N', ' ', 'M', ' ', 'S', 'Y',
  'S', 'P', 'H', ' ', 'J', ' ', 'K', ' ',
  'L', ' ', 'E', 'N', 'Y', ' ', 'U', ' ',
  'I', ' ', 'O', ' ', 'P', ' ', '6', ' ',
  '7', ' ', '8', ' ', '9', ' ', '0', ' ',
  '5', ' ', '4', ' ', '3', ' ', '2', ' ',
  '1', ' ', 'T', ' ', 'R', ' ', 'E', ' ',
  'W', ' ', 'Q', ' ', 'G', ' ', 'F', ' ',
  'D', ' ', 'S', ' ', 'A', ' ', 'V', ' ',
  'C', ' ', 'X', ' ', 'Z', ' ', 'C', 'P',
};

/* $FFF7-$FFFE: pristine contents of the live scan-key-code buffer, i.e. the
 * keyboard scheme's default key assignments. Layout matches control_keys[]:
 * [0..4] = gear/accelerate/brake/left/right, [5..7] = quit/pause/turbo. */
static const u8 default_control_keys[8] = {
  0x08, 0x26, 0x1F, 0x11, 0x19, 0x25, 0x22, 0x20
};

/* $FFEF-$FFF6: "SHOCKED"+ENTER secret test-mode-unlock reference sequence,
 * checked by redefine_keys_screen against the 8 keys just chosen. Byte-for-
 * byte identical to the 48K version's shocked_keydefs[] ($EE30, CommonData.c). */
static const u8 shocked_keydef_sequence[8] = {
  0x1E, 0x01, 0x1A, 0x0F, 0x11, 0x15, 0x16, 0x21
};

/**
 * $C6C4: Per-frame title-screen animation driver
 *
 * Syncs to the next interrupt, draws the six foreground objects ($BB00-$BB2C)
 * via the masked blitter ($C8C5), steps every object's animation script by
 * one frame ($C705), clears the playfield bitmap ($CC04), then draws the three
 * background objects ($BB36-$BB4A) via the alternate blitter ($C94F). In the
 * Z80 this block ends with an unconditional jump back to its own top
 * ($C702 JP $C6C4), so it never returns to its caller under normal
 * operation — the only exits are a stack-unwinding trick inside the
 * blitters when a frame overruns the interrupt deadline, or the whole call
 * stack being abandoned elsewhere when fire is pressed to start the game.
 *
 * Conv: the self-looping structure is not modelled directly; this function
 *       draws one frame then returns, matching how other per-frame functions in
 *       this port are called once per iteration from a caller-owned loop (e.g.
 *       drive_attract_demo). Only the EI/HALT frame-pacing point is translated,
 *       via the same stamp/sleep idiom used elsewhere. Continuous animation is
 *       driven by run_title_screen calling this function once per iteration
 *       until it returns 0 — see its own call site.
 *
 * Conv: the stack-unwind escape hatch *is* modelled, via the return value.
 *       object_script_step returns 1 when an object's script hits its $D2
 *       end-of-script opcode, which on real hardware pops straight out of this
 *       whole self-loop back to run_title_screen. When that happens, this
 *       function skips the rest of the frame's work (clear_playfield_buffer,
 *       background-object draw, present, sleep) and returns 0 to tell its
 *       caller to stop animating and fall through to the (non-animating)
 *       attract-mode wait loop — matching the "scene animates, then freezes
 *       once the tune starts" behaviour of the original game.
 *
 * \return 1 if the caller should call this function again next frame, 0 if
 * an object's script ended the self-loop this frame (final frame drawn).
 *
 * Conv: the fg/bg draw loops' EXX pairs ($C6D0/$C6DD, $C6E2, $C6E9/$C6FD) only
 *       protect the loop counter (B) and record-stride (DE) from being
 *       clobbered by the blitter call -- they do not carry any value between
 *       main and shadow sets that survives past the following EXX. This is
 *       register protection around a call, not persistent banking, so it needs
 *       no shadow variables in C: the loop counter is a plain C `for`, and the
 *       blitter is called directly with the fields it needs.
 *
 * Conv: added a speccy->draw() call after the bg objects are blitted. The Z80
 *       has no equivalent -- it draws straight into the real display memory --
 *       but the SDL port renders into an off-screen buffer that must be
 *       explicitly presented every frame, or the host window only ever shows
 *       the single frame drawn by run_title_screen before this function starts
 *       looping.
 */
static u8 titlescr_animate_frame(chqstate_t *state)
{
  int                  obj; /* object index within the fg/bg loop (was B) */
  struct title_object *rec; /* current object record (was IX) */

  state->speccy->stamp(state->speccy);

  clear_playfield_buffer(state);

  /* Draw the six "foreground" objects: C, H, A, S, E, H */
  for (obj = 0; obj < 6; obj++)
  {
    rec = &state->bank3->title_objects[obj];
    compute_glyph_blit_params_fg(state, rec->y, rec->x, rec->row);
  }

  /* Draw the three "background" objects: DOT, Q, DOT */
  for (obj = 6; obj < 9; obj++)
  {
    rec = &state->bank3->title_objects[obj];
    compute_glyph_blit_params_bg(state, rec->y, rec->x, rec->row);
  }

  /* Conv: added */
  update_whole_playfield(state);
  state->speccy->sleep(state->speccy, TITLE_ANIM_TSTATES);

  if (object_script_step(state))
  {
    /* Conv: the frame's remaining work is abandoned, but the sleep still runs. */
    return 0; /* $D2 hit -- abort before clear/bg-draw/present, see prologue */
  } else
  {
    return 1;
  }
}

/**
 * $C705: Object animation script interpreter
 *
 * Advances all 9 title-screen objects (state->title_objects[9]) by one
 * frame. Each object record carries a byte-code cursor into
 * title_scene_data; this is a state machine with two dispatch chains that
 * share six "active movement mode" opcodes ($C9-$CE):
 *
 *  - Active dispatch (oss_object_loop, $C70E): when an object's stored
 *    opcode is non-zero, runs one frame's worth of movement for whichever
 *    mode is active, then ticks its wait counter (oss_countdown, $C731),
 *    going idle (opcode -> 0) once it reaches zero. This tick only happens
 *    for opcodes $C9-$CF: any other stored opcode (the fetch chain below
 *    can store one as a fallback) never reaches oss_countdown, so the
 *    object's wait counter is frozen forever and it can never go idle
 *    again. This is not a hypothetical edge case -- every one of the 5
 *    title-screen scenes has an object whose script emits the dead value
 *    $D1 (see the ctl note at $C705/$CC50), so that object permanently
 *    stops moving partway through the animation. Faithfully reproduced
 *    here, not a bug in this port.
 *  - Fetch dispatch (oss_fetch_opcode/_cont, $C740/$C746): when idle, reads
 *    script bytes until it hits a mode-setting opcode. Bytes with the sign
 *    bit clear ($00-$7F) are immediate 2-axis step deltas applied at once
 *    (oss_op_immediate_step, $C868); opcode $C8 (set row) and $D0 (jump to
 *    absolute position) act immediately too and keep fetching; a
 *    mode-setting opcode ($C9-$CF, or any other byte >= $80 as a fallback)
 *    is stored as the new active opcode, its operand bytes are read, and the
 *    object is re-dispatched through the active chain immediately -- so a
 *    freshly fetched mode runs its first frame of movement in the same call
 *    that fetched it.
 *
 * The six active modes are: constant velocity ($C9, oss_op_velocity);
 * decelerate X/Y ($CA/$CB, oss_op_decel_x/oss_op_decel_y); and three
 * accelerate-X variants ($CC/$CD/$CE, oss_op_accel_x_a/oss_op_accel_x_c/
 * oss_op_accel_x_b -- note the fetch dispatch maps CD to variant c and CE to
 * variant b, not alphabetically). See those functions' own prologues for
 * how the decelerate/accelerate modes overload the x_step/y_step fields as
 * curve-lookup counters.
 *
 * Conv: opcode $D2 ("end of script") is `POP HL; RET` on real hardware -- with
 *       no PUSH anywhere in this call chain, that pops object_script_step's own
 *       return address as data and returns via the frame beneath it, aborting
 *       all the way back into titlescr_animate_frame's *caller*
 *       (run_title_screen's own per-frame loop) and skipping the rest of that
 *       frame's work (clear_playfield_buffer, background-object draw,
 *       stamp/sleep). This is exactly what makes the attract-mode scene animate
 *       for a while and then freeze once the tune starts: whichever object's
 *       script reaches $D2 first is the one that ends the self-loop. Modelled
 *       here with a u8 return (1 = hit $D2, stop processing further objects
 *       this frame) that titlescr_animate_frame propagates to its own caller,
 *       rather than the raw stack-unwind trick itself -- see
 *       titlescr_animate_frame's prologue.
 *
 * Conv: oss_op_immediate_step's Y-magnitude extraction rotates A right through
 *       the carry flag 3 times before masking with AND $03; the carry bit fed
 *       into the first rotation (left over from the preceding ADD A,(IX+$07))
 *       lands in a bit position the following AND discards, so the result is
 *       carry-independent and equals `(byte >> 3) & 0x03`. Translated directly
 *       as a shift rather than modelling the rotate/carry chain.
 *
 * \return 1 if the script hit $D2 ("end of script") and processing of
 * further objects this frame must stop, 0 otherwise.
 *
 * Conv: the "JP $C70E" at the end of oss_save_cursor (re-dispatching a freshly
 *       fetched mode through the active chain in the same call) is a genuine
 *       back-edge, not sequential code -- modelled here as the `continue` of
 *       the outer per-object loop, distinct from the `continue`s inside the
 *       fetch loop that model oss_fetch_opcode_cont's own re-fetch jumps ($C8,
 *       $D0). See the project's "verify back-edges" pitfall.
 */
static u8 object_script_step(chqstate_t *state)
{
  int                  obj;       /* object-loop index, 0-8 (was B, DJNZ counter) */
  struct title_object *rec;       /* current object record (was IX) */
  const u8            *HL_script;  /* script byte-code cursor while fetching (was HL) */
  u8                   A_byte;    /* fetched script byte (was A) */
  s8                   A_x_delta; /* immediate-step X delta (was A) */
  s8                   A_y_delta; /* immediate-step Y delta (was A) */
  int                  recognised; /* true if rec->opcode is $C9-$CF (was Z
                                     * flag out of the $C72E DEC-chain) */

  for (obj = 0; obj < 9; obj++)
  {
    rec = &state->bank3->title_objects[obj];

    for (;;) { /* models the "JP $C70E" re-entry after fetching a new opcode */
      if (rec->opcode == 0)
      {
        /* $C740-$C743 oss_fetch_opcode: idle -- fetch from the script. */
        HL_script = rec->script;

        for (;;) { /* $C746 oss_fetch_opcode_cont: fetch/instant-op loop */
          A_byte = *HL_script++;

          if ((s8) A_byte >= 0)
          {
            /* $C868-$C88D oss_op_immediate_step: immediate 2-axis step. */
            A_x_delta = (A_byte & 0x03) << 1;
            if (A_byte & 0x04)
              A_x_delta = -A_x_delta;
            rec->x += A_x_delta;

            A_y_delta = ((A_byte >> 3) & 0x03) << 1;
            if (A_byte & 0x20)
              A_y_delta = -A_y_delta;
            rec->y += A_y_delta;

            continue;
          }

          /* Sign bit set: a "real" opcode -- store it as the new active
           * opcode, then read its operand bytes (if any). */
          rec->opcode = A_byte;

          switch (A_byte)
          {
          case OSS_OP_SET_ROW_VAL:
            rec->row = *HL_script++;
            continue;

          case OSS_OP_VELOCITY_VAL:
            rec->x_step = (s8) *HL_script++;
            rec->y_step = (s8) *HL_script++;
            rec->wait   = *HL_script++;
            break;                          /* -> save cursor below */

          case OSS_OP_DECEL_X_VAL:
          case OSS_OP_DECEL_Y_VAL:
            /* $C7DC-$C7E8: note the y_step slot is read first here -- it
             * seeds the deceleration-curve counter, not a real Y step; see
             * oss_op_decel_x/oss_op_decel_y. */
            rec->y_step = (s8) *HL_script++;
            rec->wait   = *HL_script++;
            rec->x_step = (s8) *HL_script++;
            break;

          case OSS_OP_ACCEL_X_A_VAL:
          case OSS_OP_ACCEL_X_C_VAL:
          case OSS_OP_ACCEL_X_B_VAL:
            /* $C7CA-$C7D6: the x_step slot seeds the acceleration-curve
             * counter here, not a real X step; see oss_op_accel_x_*. */
            rec->x_step = (s8) *HL_script++;
            rec->wait   = *HL_script++;
            rec->y_step = (s8) *HL_script++;
            break;

          case OSS_OP_WAIT_VAL:
            rec->wait = *HL_script++; /* $C785-$C787 (oss_read_wait_operand) */
            break;

          case OSS_OP_JUMP_POSITION_VAL:
            rec->x = *HL_script++;
            rec->y = *HL_script++;
            continue;

          case OSS_OP_END_SCRIPT_VAL: /* see this function's own Conv note */
            return 1;

          default: /* OSS_OP_DEAD and anything else: no operand bytes,
                    * falls straight to oss_save_cursor ($C76C-$C772) */
            break;
          }

          /* $C772-$C778 oss_save_cursor: persist the advanced cursor. */
          rec->script = HL_script;
          break; /* leave the fetch loop; the outer `continue` below
                  * re-dispatches this object immediately ("JP $C70E") */
        }

        continue; /* re-check rec->opcode (now non-zero) at the top */
      }

      /* $C70E-$C72F oss_object_loop: active-mode dispatch. */
      recognised = 1;
      switch (rec->opcode)
      {
      case OSS_OP_VELOCITY_VAL:  oss_op_velocity(rec);  break;
      case OSS_OP_DECEL_X_VAL:   oss_op_decel_x(rec);   break;
      case OSS_OP_DECEL_Y_VAL:   oss_op_decel_y(rec);   break;
      case OSS_OP_ACCEL_X_A_VAL: oss_op_accel_x_a(rec); break;
      case OSS_OP_ACCEL_X_C_VAL: oss_op_accel_x_c(rec); break;
      case OSS_OP_ACCEL_X_B_VAL: oss_op_accel_x_b(rec); break;
      case OSS_OP_WAIT_VAL: break; /* no per-frame movement of its own --
                                    * falls straight to the countdown below */
      default:
        /* $C72F JR NZ,$C73B: any opcode outside $C9-$CF (e.g. OSS_OP_DEAD
         * emitted by every title scene's object 0 script --
         * see this function's own prologue and the ctl note at $C705)
         * never reaches oss_countdown, so the object's wait counter is
         * never decremented and it can never go idle again. A genuine
         * original-game quirk, faithfully reproduced -- not a bug in
         * this port. */
        recognised = 0;
        break;
      }

      if (recognised)
        /* $C731-$C737 oss_countdown: tick the wait counter; go idle (so the
         * next frame re-fetches) once it reaches 0. */
        if (--rec->wait == 0)
          rec->opcode = 0;

      break; /* $C73B oss_next_object: move on to the next object */
    }
  }

  return 0; /* $C73F RET: all 9 objects stepped, no $D2 hit this frame */
}

/**
 * $C78D: Active mode -- constant velocity
 *
 * Position += velocity every frame; no curve lookup, unlike the other five
 * active modes.
 *
 * \param[in,out] rec Object record to update (was IX).
 */
static void oss_op_velocity(struct title_object *rec)
{
  rec->x += rec->x_step;
  rec->y += rec->y_step;
}

/**
 * $C7A2: Apply the current X step to the X position
 *
 * Adds x_step to x, unconditionally, once per frame.
 *
 * \param[in,out] rec Object record to update (was IX).
 */
static void oss_apply_x_step(struct title_object *rec)
{
  rec->x += rec->x_step;
}

/**
 * $C7AC: Apply the current Y step to the Y position
 *
 * Adds y_step to y, unconditionally, once per frame.
 *
 * \param[in,out] rec Object record to update (was IX).
 */
static void oss_apply_y_step(struct title_object *rec)
{
  rec->y += rec->y_step;
}

/**
 * $C7ED: Active mode -- decelerate X
 *
 * X moves at its constant step (oss_apply_x_step); Y moves by a curve-table
 * magnitude subtracted from Y each frame, looked up from an incrementing
 * counter that overloads the y_step field for the lifetime of this mode
 * (the fetch-side operand order that seeds it is in object_script_step's
 * $CA/$CB case).
 *
 * \param[in,out] rec Object record to update (was IX).
 *
 * Conv: NEG followed by ADD A,(IX+$08) collapses to a single subtraction.
 */
static void oss_op_decel_x(struct title_object *rec)
{
  u8 C_idx;   /* curve counter, aliases the y_step field for this mode (was C, from (IX+3)) */
  u8 A_speed; /* looked-up curve magnitude (was A) */

  oss_apply_x_step(rec);

  C_idx   = (u8) rec->y_step;
  A_speed      = oss_lookup_speed(C_idx);
  rec->y      -= A_speed;
  rec->y_step = (s8) (C_idx + 1);
}

/**
 * $C804: Look up a deceleration/acceleration curve magnitude
 *
 * Shared by all five countdown-driven movement modes (oss_op_decel_x/y and
 * oss_op_accel_x_a/b/c): fetch the curve byte at the given index and halve
 * it twice.
 *
 * \param[in] C_idx Countdown/curve index (was C).
 *
 * \return          Curve magnitude for this index (was A).
 *
 * Conv: the Z80 shuttles the outer object-loop's B (DJNZ counter) through A
 *       around this lookup (LD A,B / LD B,$00 / ... / LD B,A) purely to protect
 *       it from being clobbered by the table-relative ADD HL,BC. With no shared
 *       register file in C the outer loop counter cannot be affected by this
 *       call, so the shuttle has no equivalent and is omitted.
 *
 * Conv: the skool's own commentary on this table disagrees with itself -- one
 *       paragraph calls it a 256-byte table, another documents it as a 36-entry
 *       table "indexed by a 0-35 countdown value" (matching the 36 bytes
 *       actually transcribed into title_speed_curve). A scripted object whose
 *       decel/accel phase runs long enough (or whose curve counter is seeded
 *       from a bad upstream value) can drive C_idx past 35; on real hardware
 *       that would just read whatever byte follows the table in memory, but
 *       this port's table is a 36-byte array, so an unclamped index is a
 *       genuine out-of-bounds read (caught by AddressSanitizer). Clamp to the
 *       last documented entry rather than fabricate data for the disputed
 *       256-byte range.
 */
static u8 oss_lookup_speed(u8 C_idx)
{
  // clang-format off
  /** $D272-$D295: title_speed_curve -- 36-entry deceleration/acceleration speed curve. */
  static const u8 title_speed_curve[36] = {
    0x00,
    0x01,
    0x03,
    0x04,
    0x06,
    0x07,
    0x09,
    0x0A,
    0x0C,
    0x0E,
    0x0F,
    0x11,
    0x12,
    0x14,
    0x15,
    0x17,
    0x18,
    0x1A,
    0x1C,
    0x1D,
    0x1F,
    0x20,
    0x22,
    0x23,
    0x25,
    0x26,
    0x28,
    0x29,
    0x2B,
    0x2C,
    0x2E,
    0x2F,
    0x31,
    0x33,
    0x36,
    0x3A,
  };
  // clang-format on

  if (C_idx >= sizeof(title_speed_curve))
    C_idx = sizeof(title_speed_curve) - 1;

  return title_speed_curve[C_idx] >> 2;
}

/**
 * $C812: Active mode -- decelerate Y
 *
 * Mirrors oss_op_decel_x: X still moves at its constant step; Y moves by the
 * same curve lookup, added (not subtracted) and counted down instead of up.
 *
 * \param[in,out] rec Object record to update (was IX).
 */
static void oss_op_decel_y(struct title_object *rec)
{
  u8 C_idx;   /* curve counter, aliases the y_step field for this mode (was C, from (IX+3)) */
  u8 A_speed; /* looked-up curve magnitude (was A) */

  oss_apply_x_step(rec);

  C_idx   = (u8) rec->y_step;
  A_speed      = oss_lookup_speed(C_idx);
  rec->y      += A_speed;
  rec->y_step = (s8) (C_idx - 1);
}

/**
 * $C827: Active mode -- accelerate X, variant a (negated speed, counts down)
 *
 * Y moves at its constant step (oss_apply_y_step); X moves by a curve-table
 * magnitude negated and subtracted from X each frame, looked up from a
 * counter that overloads the x_step field for the lifetime of this mode
 * (seeded via object_script_step's $CC/$CD/$CE case).
 *
 * \param[in,out] rec Object record to update (was IX).
 *
 * Conv: NEG followed by ADD A,(IX+$07) collapses to a single subtraction.
 */
static void oss_op_accel_x_a(struct title_object *rec)
{
  u8 C_idx;   /* curve counter, aliases the x_step field for this mode (was C, from (IX+2)) */
  u8 A_speed; /* looked-up curve magnitude (was A) */

  oss_apply_y_step(rec);

  C_idx   = (u8) rec->x_step;
  A_speed      = oss_lookup_speed(C_idx);
  rec->x      -= A_speed;
  rec->x_step = (s8) (C_idx - 1);
}

/**
 * $C83E: Active mode -- accelerate X, variant b (positive speed, counts down)
 *
 * Same shape as oss_op_accel_x_a, without the negation.
 *
 * \param[in,out] rec Object record to update (was IX).
 */
static void oss_op_accel_x_b(struct title_object *rec)
{
  u8 C_idx;   /* curve counter, aliases the x_step field for this mode (was C, from (IX+2)) */
  u8 A_speed; /* looked-up curve magnitude (was A) */

  oss_apply_y_step(rec);

  C_idx   = (u8) rec->x_step;
  A_speed      = oss_lookup_speed(C_idx);
  rec->x      += A_speed;
  rec->x_step = (s8) (C_idx - 1);
}

/**
 * $C853: Active mode -- accelerate X, variant c (positive speed, counts up)
 *
 * Same shape as oss_op_accel_x_b, counting the curve index up instead of
 * down.
 *
 * \param[in,out] rec Object record to update (was IX).
 */
static void oss_op_accel_x_c(struct title_object *rec)
{
  u8 C_idx;   /* curve counter, aliases the x_step field for this mode (was C, from (IX+2)) */
  u8 A_speed; /* looked-up curve magnitude (was A) */

  oss_apply_y_step(rec);

  C_idx   = (u8) rec->x_step;
  A_speed      = oss_lookup_speed(C_idx);
  rec->x      += A_speed;
  rec->x_step = (s8) (C_idx + 1);
}

/**
 * $C890: Clear the playfield bitmap and attribute area
 *
 * Zero-fills the attribute area $5900-$5AFF and the bitmap $4800-$57FF --
 * the lower two-thirds of the screen, leaving $4000-$47FF (the top third)
 * untouched. Called by $C0EC and clear_and_fill_border_attrs ($C8A9).
 *
 * Same as clear_screen (which this bank overlaps).
 *
 * Conv: the Z80 self-fills via `LD (HL),L` (both ranges start on a $x00
 *       boundary, so L is already zero) then LDIR; this collapses to two plain
 *       memset calls, per the skool's own Conv note at $C890.
 */
static void clear_playfield_and_attrs(chqstate_t *state)
{
  memset(ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR), 0,
         SCREEN_ATTRIBUTES_ROWBYTES * PLAYFIELD_HEIGHT / 8);
  memset(ADDRTOSCREEN(SCREEN_PLAYFIELD_BITMAP_ADDR), 0,
         SCREEN_BITMAP_ROWBYTES * PLAYFIELD_HEIGHT);
}

/**
 * $C8A9: Clear the playfield then paint the border attribute rows
 *
 * Calls clear_playfield_and_attrs, then overwrites the attribute area
 * $5900-$5AFF with a fixed pattern, 32 bytes at a time (16 times, covering
 * all 512 bytes): 2 bytes of attribute 0 (black), 28 bytes of attribute
 * $45 (flash bit set; paper/ink in bits 0-5), then 2 more bytes of
 * attribute 0.
 */
static void clear_and_fill_border_attrs(chqstate_t *state)
{
  u8 *attrs; /* attribute write cursor (was HL) */
  int c;     /* outer repeat count, 16 (was C) */

  clear_playfield_and_attrs(state);

  attrs = ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR);
  c     = PLAYFIELD_HEIGHT / 8;
  do
  {
    *attrs++ = attribute_BLACK_OVER_BLACK;
    *attrs++ = attribute_BLACK_OVER_BLACK;
    memset(attrs, attribute_BRIGHT_CYAN_OVER_BLACK, 28); attrs += 28; /* Conv: memset replaces loop */
    *attrs++ = attribute_BLACK_OVER_BLACK;
    *attrs++ = attribute_BLACK_OVER_BLACK;
  } while (--c);

  update_whole_playfield_full_width(state); /* Conv: added -- also covers the
                                               * leftmost column, in case the
                                               * hiscore/name-entry screen
                                               * (which draws right to the
                                               * screen edge) left pixels
                                               * behind there */
}

/**
 * $C8C5: Draw a foreground title-screen object's glyph
 *
 * Computes the destination address and glyph-table entry via
 * compute_glyph_geometry, then, if the object's Y position was clamped
 * (partially off the bottom of the drawable window), walks the source
 * pointer forward one row-pair's worth of bytes at a time until the clamp
 * excess is consumed, decrementing the row-pair count in step. If the
 * row-pair count reaches zero first, the object is entirely off-screen and
 * nothing is drawn. Otherwise dispatches to the width-specific OR-blit
 * routine.
 *
 * \param[in] B_y   Object Y screen position (was B).
 * \param[in] C_x   Object X screen position (was C).
 * \param[in] L_row Object row/height byte (was L).
 *
 * Conv: $C93D onwards, the Z80 saves the real SP, repoints SP at the glyph
 *       source so the blit routines can POP bytes from it, then restores the
 *       real SP before returning (or via blit_abort_restore_sp on early abort).
 *       This whole mechanism is a way of getting fast sequential byte reads out
 *       of the Z80's POP instruction; it has no bearing on control flow --
 *       every path still returns cleanly to this function's caller, exactly
 *       like an ordinary nested C call. clear_playfield_buffer above
 *       established the same conclusion for its own "LD SP,HL; PUSH x N"
 *       fast-fill trick. Accordingly the real-SP save/restore is omitted
 *       entirely; blit_glyph_rows above reads the source with plain sequential
 *       `*src++`, and both dispatchers return normally with no simulated stack
 *       juggling. cgb_delay_tail's fixed delay loop is likewise a hardware
 *       frame-timing pad with no C equivalent (there is no frame deadline to
 *       protect) and is not translated.
 *
 * Conv: the row-offset skip loop ($C906-$C916) is a post-test loop that
 *       decrements the row-pair count first and only tests the skip count
 *       afterwards. When the Y clamp excess is exactly 1 (Y = $70 or $71),
 *       excess >> 1 is 0, and the u8 skip counter wraps from 0 to 255 on its
 *       first decrement, effectively running the skip loop until the row-pair
 *       count itself reaches zero -- silently drawing nothing for those two Y
 *       values. This is a latent quirk of the original code
 *       (compute_glyph_blit_params_bg below guards against it explicitly), not
 *       a translation bug, and is preserved via A_skip_pairs' u8 wraparound
 *       rather than "fixed".
 */
static void compute_glyph_blit_params_fg(chqstate_t *state,
                                         u8          B_y,
                                         u8          C_x,
                                         u8          L_row)
{
  glyph_blit_geometry_t g;            /* destination address and glyph lookup, filled below (was D/E/HL/B/C/Carry/A') */
  u8                    A_skip_pairs; /* row-pairs of source to skip (was A) */

  compute_glyph_geometry(B_y, C_x, L_row, &g);

  if (!g.carry_initial)
  {
    A_skip_pairs = (u8) (g.A_excess >> 1);
    do
    {
      g.HL_src += g.C_width_select * 2;
      if (--g.B_height_pairs == 0)
        return; /* entirely off-screen -- draw nothing */
    } while (--A_skip_pairs != 0); /* u8 wrap intentional, see Conv note above */
  }

  blit_masked_sprite_dispatch_fg(state, g.H, g.L, g.HL_src, g.B_height_pairs,
                              g.C_width_select);
}

/**
 * $C8CD-$C902 / $C957-$C98C: Compute glyph destination address and table entry
 *
 * Shared by compute_glyph_blit_params_fg and compute_glyph_blit_params_bg, which
 * are otherwise identical apart from the row-offset walk and dispatch table
 * they feed. Clamps the object's Y screen position to a maximum of $6F
 * (rows below that are off the bottom of the drawable window and must be
 * partially skipped by the caller), builds the destination screen address
 * from the clamped Y and the X position, and looks up the glyph's row-pair
 * count/width/bitmap pointer by index (row + ((x >> 1) & 3)) into
 * title_glyph_table.
 *
 * \param[in]  B_y   Object Y screen position (was B).
 * \param[in]  C_x   Object X screen position (was C).
 * \param[in]  L_row Object row/height byte, selects which of 4 glyph variants
 *                   for this animation row (was L).
 * \param[out] out   Filled with the destination address, glyph pointer,
 *                   row-pair count, width selector and Y-clamp state.
 *
 * Conv: the two "RRA/SCF/RRA/RRA" then "XOR B ; AND mask ; XOR B" sequences
 *       that build D and E are translated literally with the RR/RLC macros from
 *       Z80.h and the replace-bits-under-a-mask idiom, matching the style
 *       already used for the AY register merge in compute_channel_ay_registers.
 */
static void compute_glyph_geometry(u8                     B_y,
                                   u8                     C_x,
                                   u8                     L_row,
                                   glyph_blit_geometry_t *out)
{
  // clang-format off
  /** $D296-$D461: title_glyph_table -- 115 glyph metadata entries. */
  static const title_glyph_t title_glyph_table[115] = {
    { 14, 4, title_glyph_bitmap_000 },
    { 14, 4, title_glyph_bitmap_001 },
    { 14, 4, title_glyph_bitmap_002 },
    { 14, 5, title_glyph_bitmap_003 },
    { 14, 4, title_glyph_bitmap_004 },
    { 14, 4, title_glyph_bitmap_005 },
    { 14, 5, title_glyph_bitmap_006 },
    { 14, 5, title_glyph_bitmap_007 },
    { 14, 3, title_glyph_bitmap_008 },
    { 14, 3, title_glyph_bitmap_009 },
    { 14, 4, title_glyph_bitmap_010 },
    { 14, 4, title_glyph_bitmap_011 },
    { 14, 4, title_glyph_bitmap_012 },
    { 14, 4, title_glyph_bitmap_013 },
    { 14, 4, title_glyph_bitmap_014 },
    { 14, 5, title_glyph_bitmap_015 },
    { 14, 4, title_glyph_bitmap_016 },
    { 14, 4, title_glyph_bitmap_017 },
    { 14, 4, title_glyph_bitmap_018 },
    { 14, 4, title_glyph_bitmap_019 },
    { 16, 4, title_glyph_bitmap_020 },
    { 16, 4, title_glyph_bitmap_021 },
    { 16, 4, title_glyph_bitmap_022 },
    { 16, 5, title_glyph_bitmap_023 },
    { 4, 6, title_glyph_bitmap_024 },
    { 4, 6, title_glyph_bitmap_025 },
    { 4, 6, title_glyph_bitmap_026 },
    { 4, 6, title_glyph_bitmap_027 },
    { 13, 3, title_glyph_bitmap_028 },
    { 10, 4, title_glyph_bitmap_029 },
    { 9, 4, title_glyph_bitmap_030 },
    { 10, 3, title_glyph_bitmap_031 },
    { 10, 3, title_glyph_bitmap_032 },
    { 10, 4, title_glyph_bitmap_033 },
    { 10, 4, title_glyph_bitmap_034 },
    { 7, 2, title_glyph_bitmap_035 },
    { 7, 2, title_glyph_bitmap_036 },
    { 7, 2, title_glyph_bitmap_037 },
    { 7, 3, title_glyph_bitmap_038 },
    { 4, 1, title_glyph_bitmap_039 },
    { 4, 2, title_glyph_bitmap_040 },
    { 4, 2, title_glyph_bitmap_041 },
    { 4, 2, title_glyph_bitmap_042 },
    { 10, 3, title_glyph_bitmap_043 },
    { 10, 3, title_glyph_bitmap_044 },
    { 10, 4, title_glyph_bitmap_045 },
    { 10, 4, title_glyph_bitmap_046 },
    { 7, 2, title_glyph_bitmap_047 },
    { 7, 2, title_glyph_bitmap_048 },
    { 7, 3, title_glyph_bitmap_049 },
    { 7, 3, title_glyph_bitmap_050 },
    { 4, 1, title_glyph_bitmap_051 },
    { 4, 2, title_glyph_bitmap_052 },
    { 4, 2, title_glyph_bitmap_053 },
    { 4, 2, title_glyph_bitmap_054 },
    { 10, 2, title_glyph_bitmap_055 },
    { 10, 3, title_glyph_bitmap_056 },
    { 10, 3, title_glyph_bitmap_057 },
    { 10, 3, title_glyph_bitmap_058 },
    { 7, 2, title_glyph_bitmap_059 },
    { 7, 2, title_glyph_bitmap_060 },
    { 7, 2, title_glyph_bitmap_061 },
    { 7, 2, title_glyph_bitmap_062 },
    { 4, 1, title_glyph_bitmap_063 },
    { 4, 1, title_glyph_bitmap_064 },
    { 4, 2, title_glyph_bitmap_065 },
    { 4, 2, title_glyph_bitmap_066 },
    { 10, 3, title_glyph_bitmap_067 },
    { 10, 3, title_glyph_bitmap_068 },
    { 10, 4, title_glyph_bitmap_069 },
    { 10, 4, title_glyph_bitmap_070 },
    { 7, 2, title_glyph_bitmap_071 },
    { 7, 2, title_glyph_bitmap_072 },
    { 7, 2, title_glyph_bitmap_073 },
    { 7, 3, title_glyph_bitmap_074 },
    { 4, 1, title_glyph_bitmap_075 },
    { 4, 2, title_glyph_bitmap_076 },
    { 4, 2, title_glyph_bitmap_077 },
    { 4, 2, title_glyph_bitmap_078 },
    { 10, 3, title_glyph_bitmap_079 },
    { 10, 3, title_glyph_bitmap_080 },
    { 10, 3, title_glyph_bitmap_081 },
    { 10, 3, title_glyph_bitmap_082 },
    { 7, 2, title_glyph_bitmap_083 },
    { 7, 2, title_glyph_bitmap_084 },
    { 7, 2, title_glyph_bitmap_085 },
    { 7, 3, title_glyph_bitmap_086 },
    { 4, 1, title_glyph_bitmap_087 },
    { 4, 1, title_glyph_bitmap_088 },
    { 4, 2, title_glyph_bitmap_089 },
    { 4, 2, title_glyph_bitmap_090 },
    { 12, 3, title_glyph_bitmap_091 },
    { 12, 3, title_glyph_bitmap_092 },
    { 12, 3, title_glyph_bitmap_093 },
    { 12, 4, title_glyph_bitmap_094 },
    { 8, 2, title_glyph_bitmap_095 },
    { 8, 2, title_glyph_bitmap_096 },
    { 8, 3, title_glyph_bitmap_097 },
    { 8, 3, title_glyph_bitmap_098 },
    { 5, 1, title_glyph_bitmap_099 },
    { 5, 2, title_glyph_bitmap_100 },
    { 5, 2, title_glyph_bitmap_101 },
    { 5, 2, title_glyph_bitmap_102 },
    { 3, 7, title_glyph_bitmap_103 },
    { 3, 6, title_glyph_bitmap_104 },
    { 3, 6, title_glyph_bitmap_105 },
    { 3, 6, title_glyph_bitmap_106 },
    { 2, 7, title_glyph_bitmap_107 },
    { 2, 7, title_glyph_bitmap_108 },
    { 2, 6, title_glyph_bitmap_109 },
    { 2, 6, title_glyph_bitmap_110 },
    { 1, 7, title_glyph_bitmap_111 },
    { 1, 7, title_glyph_bitmap_112 },
    { 1, 6, title_glyph_bitmap_113 },
    { 1, 6, title_glyph_bitmap_114 },
  };
  // clang-format on

  u8                   B_clamped;     /* Y, clamped to a maximum of $6F (was B) */
  u8                   B_screen_rows; /* $AF - B_clamped, reused as the mask-merge operand (was B) */
  u8                   A;             /* working accumulator (was A) */
  int                  carry;         /* Z80 carry flag, used by the RR/RLC macros */
  u8                   D;             /* destination screen address high byte (was D) */
  u8                   E;             /* destination screen address low byte (was E) */
  int                  glyph_index;   /* index into title_glyph_table (was BC, table offset / 4) */
  const title_glyph_t *glyph;         /* looked-up glyph-table entry (was the HL read chain at $C8F5) */

  if (B_y < 0x70)
  {
    B_clamped           = B_y;
    out->A_excess       = 0;    /* unused: carry_initial skips the row-offset walk */
    out->carry_initial  = 1;
  } else
  {
    B_clamped           = 0x6F;
    out->A_excess       = (u8) (B_y - 0x6F);
    out->carry_initial  = 0;
  }

  /* $C8CE-$C8DC / $C958-$C966: D = destination screen address high byte. */
  B_screen_rows = (u8) (0xAF - B_clamped);
  A = B_screen_rows;
  carry = 0; RR(A);
  carry = 1; RR(A);
  carry = 0; RR(A);
  A ^= B_screen_rows;
  A &= 0xF8;
  A ^= B_screen_rows;
  D  = A;

  /* $C8DD-$C8E7 / $C967-$C971: E = destination screen address low byte. */
  A = C_x;
  RLC(A); RLC(A); RLC(A);
  A ^= B_screen_rows;
  A &= 0xC7;
  A ^= B_screen_rows;
  RLC(A); RLC(A);
  E = A;

  out->H = D;
  out->L = E;

  /* $C8E8-$C901 / $C972-$C98B: glyph table lookup. The 4-byte table entry
     (height_pairs, width_bytes, bitmap lo, bitmap hi) is title_glyph_t's
     layout exactly, so the raw byte reads collapse to direct indexing. */
  glyph_index = L_row + ((C_x >> 1) & 0x03);
  assert(glyph_index >= 0 && glyph_index < TITLE_GLYPH_COUNT);

  glyph               = &title_glyph_table[glyph_index];
  out->B_height_pairs = glyph->height_pairs;
  out->C_width_select = glyph->width_bytes;
  out->HL_src          = glyph->bitmap;
}

/**
 * $C917: Masked-sprite blit dispatch (foreground objects)
 *
 * Selects one of the 7 width-specific OR-blit routines by C_width_select,
 * decremented against 1..5 with an explicit test; anything else (6 or more)
 * falls through to blit_width6 -- blit_width7 is unreachable from this
 * dispatcher (see its own Conv note).
 *
 * \param[in] H              Destination screen address high byte.
 * \param[in] L              Destination screen address low byte.
 * \param[in] src            Glyph bitmap source pointer.
 * \param[in] B_height_pairs Number of row-pairs to draw.
 * \param[in] C_width_select Width selector, 1-7.
 */
static void blit_masked_sprite_dispatch_fg(chqstate_t *state,
                                           int         H,
                                           int         L,
                                           const u8   *src,
                                           int         B_height_pairs,
                                           int         C_width_select)
{
  switch (C_width_select)
  {
  case 1:  blit_width1(state, H, L, src, B_height_pairs); break;
  case 2:  blit_width2(state, H, L, src, B_height_pairs); break;
  case 3:  blit_width3(state, H, L, src, B_height_pairs); break;
  case 4:  blit_width4(state, H, L, src, B_height_pairs); break;
  case 5:  blit_width5(state, H, L, src, B_height_pairs); break;
  default: blit_width6(state, H, L, src, B_height_pairs); break;
  }
}

/**
 * $C94F: Draw a background title-screen object's glyph
 *
 * Structurally identical to compute_glyph_blit_params_fg above (see its Conv
 * notes for the SP-as-pointer and frame-timing decisions, which apply here
 * unchanged), but feeds blit_masked_sprite_dispatch_bg, and its row-offset
 * skip loop computes the per-row-pair source stride differently for the
 * "width-6"/"width-7" quirk routines (4 bytes for width 6, 2 bytes for
 * width 7, matching blit_width6/blit_width7's real 2-byte/1-byte-per-row
 * consumption) and explicitly guards the skip count against the u8-wrap
 * quirk noted in compute_glyph_blit_params_fg (forcing a minimum of 1).
 *
 * \param[in] B_y   Object Y screen position (was B).
 * \param[in] C_x   Object X screen position (was C).
 * \param[in] L_row Object row/height byte (was L).
 */
static void compute_glyph_blit_params_bg(chqstate_t *state,
                                         u8          B_y,
                                         u8          C_x,
                                         u8          L_row)
{
  glyph_blit_geometry_t g;            /* destination address and glyph lookup, filled below (was D/E/HL/B/C/Carry/A') */
  u8                    A_skip_pairs; /* row-pairs of source to skip (was A) */
  u8                    E_stride;     /* per-row-pair source advance, bytes (was E) */

  compute_glyph_geometry(B_y, C_x, L_row, &g);

  if (!g.carry_initial)
  {
    if (g.C_width_select < 6)
      E_stride = (u8) (g.C_width_select << 1);
    else
      E_stride = (u8) ((8 - g.C_width_select) << 1); /* width 6 -> 4, width 7 -> 2 */

    A_skip_pairs = (u8) (g.A_excess >> 1);
    if (A_skip_pairs == 0)
      A_skip_pairs = 1; /* $C9A3-$C9A5: guards the wrap quirk noted above */

    do
    {
      g.HL_src += E_stride;
      if (--g.B_height_pairs == 0)
        return; /* entirely off-screen -- draw nothing */
    } while (--A_skip_pairs != 0);
  }

  blit_masked_sprite_dispatch_bg(state, g.H, g.L, g.HL_src, g.B_height_pairs,
                                g.C_width_select);
}

/**
 * $C9AF: Masked-sprite blit dispatch (background objects)
 *
 * Structurally identical to blit_masked_sprite_dispatch_fg but tests widths
 * 1..6 explicitly, so blit_width7 (unreachable from the foreground
 * dispatcher above) is reached here as the default case.
 *
 * \param[in] H              Destination screen address high byte.
 * \param[in] L              Destination screen address low byte.
 * \param[in] src            Glyph bitmap source pointer.
 * \param[in] B_height_pairs Number of row-pairs to draw.
 * \param[in] C_width_select Width selector, 1-7.
 */
static void blit_masked_sprite_dispatch_bg(chqstate_t *state,
                                           int         H,
                                           int         L,
                                           const u8   *src,
                                           int         B_height_pairs,
                                           int         C_width_select)
{
  switch (C_width_select)
  {
  case 1:  blit_width1(state, H, L, src, B_height_pairs); break;
  case 2:  blit_width2(state, H, L, src, B_height_pairs); break;
  case 3:  blit_width3(state, H, L, src, B_height_pairs); break;
  case 4:  blit_width4(state, H, L, src, B_height_pairs); break;
  case 5:  blit_width5(state, H, L, src, B_height_pairs); break;
  case 6:  blit_width6(state, H, L, src, B_height_pairs); break;
  default: blit_width7(state, H, L, src, B_height_pairs); break;
  }
}

/**
 * $C9D5: Masked-sprite OR-blit, row_bytes wide
 *
 * Also covers the identically-shaped copies at $CA17, $CA64, $CABC, $CB17,
 * $CB7B, $CBC5.
 *
 * Draws B_height_pairs row-pairs (2 scanlines each) from src into the
 * screen bitmap starting at (H,L), OR-ing row_bytes source bytes into each
 * scanline so the glyph never overwrites bits already set by an overlapping
 * sprite. Advances the screen address one scanline at a time via
 * advance_glyph_scanline.
 *
 * \param[in] H              Destination screen address high byte.
 * \param[in] L              Destination screen address low byte.
 * \param[in] src            Glyph bitmap source pointer.
 * \param[in] B_height_pairs Number of row-pairs to draw.
 * \param[in] row_bytes      Bytes to OR into each scanline.
 *
 * Conv: the Z80 draws each row-pair via "LD SP,HL; POP DE", i.e. two source
 *       bytes at a time (E first, then D) -- see the project's known "LD SP,HL;
 *       POP x N sprite copy" translation pitfall. Since the bytes are written
 *       verbatim with no mask table or flip, this collapses to a plain
 *       sequential `*src++` per byte, matching the pitfall's documented
 *       equivalence. The 7 width-specific unrolled routines ($C9D5-$CBC5) share
 *       this exact shape (only row_bytes and the reachable dispatch entry
 *       differ), so they are modelled as one parameterised helper rather than 7
 *       near-duplicate bodies. Each width routine's own fixed delay loop (e.g.
 *       $C9E8-$C9EB, present on widths 1-5 only) exists purely to pad out real
 *       hardware frame timing; the C port has no such deadline to protect (see
 *       compute_glyph_blit_params_fg' Conv note on $C93C/$C93D), so none of the
 *       delay loops are translated.
 *
 * Conv: compute_glyph_blit_params_fg only clamps the *top* of the glyph (see
 *       its own Conv note on the $C906-$C916 skip loop); the disassembly has no
 *       symmetric clamp for the bottom, so a fast-moving object (e.g. one
 *       driven by oss_op_velocity) can walk this loop's (H,L) address below
 *       screen third 3 and off the bottom of the physical display. On real
 *       hardware that just pokes stray bytes into attribute memory (or further
 *       afield) -- harmless enough that nobody noticed. This port's screen is a
 *       fixed-size struct, not flat memory, so the equivalent out-of-range
 *       write is skipped instead of performed, rather than asserting or
 *       corrupting adjacent struct fields; the address/source advance below
 *       still runs unconditionally so the timing and any later in-range rows
 *       stay correct.
 */
static void blit_glyph_rows(chqstate_t *state,
                            int         H,
                            int         L,
                            const u8   *src,
                            int         B_height_pairs,
                            int         row_bytes)
{
  u8 *dst;  /* current scanline's destination byte(s) (was HL) */
  int row;  /* 0 or 1: which scanline of the current row-pair (Conv: rolled, no Z80 equivalent) */
  int i;    /* byte offset within the current scanline (Conv: rolled, no Z80 equivalent) */
  int addr; /* destination Z80 screen address for this scanline (Conv: added, for the bounds check below) */

  do
  {
    for (row = 0; row < 2; row++)
    {
      addr = (H << 8) | L;
      if (addr >= SCREEN_START_ADDRESS &&
          addr <  SCREEN_START_ADDRESS + SCREEN_BITMAP_LENGTH)
          {
        dst = ADDRTOSCREEN(addr);
        for (i = 0; i < row_bytes; i++)
          dst[i] |= *src++;
      } else
      {
        src += row_bytes; /* Conv: off-screen scanline, see note above */
      }
      H++;
      advance_glyph_scanline(&H, &L);
    }
  } while (--B_height_pairs);
}

/**
 * $C9D5: Width-1 masked-sprite OR-blit (1 byte per scanline).
 *
 * Thin wrapper around blit_glyph_rows with row_bytes fixed at 1.
 *
 * \param[in] H              Destination screen address high byte.
 * \param[in] L              Destination screen address low byte.
 * \param[in] src            Glyph bitmap source pointer.
 * \param[in] B_height_pairs Number of row-pairs to draw.
 */
static void blit_width1(
    chqstate_t *state, int H, int L, const u8 *src, int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 1);
}

/**
 * $C9F3: Advance a glyph-blit screen address to the next scanline
 *
 * Also covers the 13 further copies of this same sequence at $CA05, $CA40,
 * $CA52, $CA98, $CAAA, $CAF3, $CB05, $CB57, $CB69, $CBA1, $CBB3, $CBE0,
 * $CBF2.
 *
 * The caller increments H before calling this; this function applies the
 * ZX Spectrum screen memory's non-linear "third boundary" correction when a
 * character row completes (H & 7 == 0 after the increment).
 *
 * \param[in,out] H Screen address high byte.
 * \param[in,out] L Screen address low byte.
 *
 * Conv: factored into a shared helper rather than repeating the 14
 *       near-identical inline copies in the disassembly -- the same "Conv:
 *       extracted to function" treatment next_screen_row got in Main.c.
 *       next_screen_row itself is static to Main.c and not visible here;
 *       clear_playfield_buffer above already established this file's own
 *       precedent of modelling this exact address math locally rather than
 *       sharing it across files, so this helper follows that precedent instead
 *       of exposing next_screen_row.
 */
static void advance_glyph_scanline(int *H, int *L)
{
  if ((*H & 0x07) != 0)
    return;

  *H -= 0x08;
  *L += 0x20;
  if (*L > 0xFF)
  {
    *L &= 0xFF;
    *H += 0x08;
  }
}

/**
 * $CA17: Width-2 masked-sprite OR-blit (2 bytes per scanline).
 *
 * Thin wrapper around blit_glyph_rows with row_bytes fixed at 2.
 *
 * \param[in] H              Destination screen address high byte.
 * \param[in] L              Destination screen address low byte.
 * \param[in] src            Glyph bitmap source pointer.
 * \param[in] B_height_pairs Number of row-pairs to draw.
 */
static void blit_width2(
    chqstate_t *state, int H, int L, const u8 *src, int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 2);
}

/**
 * $CA64: Width-3 masked-sprite OR-blit (3 bytes per scanline).
 *
 * Thin wrapper around blit_glyph_rows with row_bytes fixed at 3.
 *
 * \param[in] H              Destination screen address high byte.
 * \param[in] L              Destination screen address low byte.
 * \param[in] src            Glyph bitmap source pointer.
 * \param[in] B_height_pairs Number of row-pairs to draw.
 */
static void blit_width3(
    chqstate_t *state, int H, int L, const u8 *src, int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 3);
}

/**
 * $CABC: Width-4 masked-sprite OR-blit (4 bytes per scanline).
 *
 * Thin wrapper around blit_glyph_rows with row_bytes fixed at 4.
 *
 * \param[in] H              Destination screen address high byte.
 * \param[in] L              Destination screen address low byte.
 * \param[in] src            Glyph bitmap source pointer.
 * \param[in] B_height_pairs Number of row-pairs to draw.
 */
static void blit_width4(
    chqstate_t *state, int H, int L, const u8 *src, int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 4);
}

/**
 * $CB17: Width-5 masked-sprite OR-blit (5 bytes per scanline).
 *
 * Thin wrapper around blit_glyph_rows with row_bytes fixed at 5.
 *
 * \param[in] H              Destination screen address high byte.
 * \param[in] L              Destination screen address low byte.
 * \param[in] src            Glyph bitmap source pointer.
 * \param[in] B_height_pairs Number of row-pairs to draw.
 */
static void blit_width5(
    chqstate_t *state, int H, int L, const u8 *src, int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 5);
}

/**
 * $CB7B: "Width-6" masked-sprite OR-blit
 *
 * Conv/bug preserved literally: despite its position in the dispatch chain
 * (reached when the glyph's width selector is 6 or more), this routine's
 * instructions are byte-for-byte identical in shape to blit_width2 -- it
 * draws only 2 bytes per scanline, not 6. The source pointer only advances
 * 2 bytes per scanline to match. This is verified against the disassembly,
 * not assumed: every LD (HL),A / INC L pair at $CB7B-$CB99 matches
 * $CA17-$CA3B exactly. Any glyph whose real width_bytes is 6 or 7 would
 * therefore be drawn with its rightmost columns missing and its source data
 * under-consumed -- an original-game quirk, not a translation bug, and is
 * not "fixed" here.
 *
 * \param[in] H              Destination screen address high byte.
 * \param[in] L              Destination screen address low byte.
 * \param[in] src            Glyph bitmap source pointer.
 * \param[in] B_height_pairs Number of row-pairs to draw.
 */
static void blit_width6(
    chqstate_t *state, int H, int L, const u8 *src, int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 2);
}

/**
 * $CBC5: "Width-7" masked-sprite OR-blit
 *
 * Conv/bug preserved literally: only reachable via
 * blit_masked_sprite_dispatch_bg's default case. Byte-for-byte identical in
 * shape to blit_width1 -- draws only 1 byte per scanline despite its
 * position at the end of the width-7 dispatch chain. See blit_width6's note
 * above; the same quirk applies here one width class down.
 *
 * \param[in] H              Destination screen address high byte.
 * \param[in] L              Destination screen address low byte.
 * \param[in] src            Glyph bitmap source pointer.
 * \param[in] B_height_pairs Number of row-pairs to draw.
 */
static void blit_width7(
    chqstate_t *state, int H, int L, const u8 *src, int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 1);
}

/**
 * $CC04: Clear the playfield bitmap
 *
 * Zero-fills the bitmap area used by the title-screen object sprites: all
 * of screen third 2 ($4800-$4FFF, all 8 character rows) and the top 5
 * character rows of screen third 3 ($5000-$57FF), leaving third 3's bottom
 * 3 character rows untouched so the fixed overlay text printed once by
 * run_title_screen (print_character) is not wiped out every frame. Only
 * 28 of each scanline's 32 bytes are cleared (screen columns 2-29), leaving
 * a 2-column margin on each edge. Classic "LD SP,HL; PUSH x N" fast-fill
 * trick (see the project's known translation pitfall of the same name): SP
 * is repointed at the bitmap and 14 PUSH DE instructions (DE=0) fill 28
 * bytes backward from HL, DJNZ-looped 8 times per character row, stepping
 * through the $x00 third boundary via the usual ADD A,$20 / carry pattern.
 *
 * Conv: the PUSH-fill collapses to one memset per scanline; the real stack
 *       save/restore at $CC04/$CC4C-$CC4F has no C equivalent (SP is never
 *       repurposed as a data pointer here) and is omitted.
 */
static void clear_playfield_buffer(chqstate_t *state)
{
  int H;          /* screen address high byte (was H) */
  int L;          /* screen address low byte (was L) */
  int B_scanline; /* scanline countdown within one character row, 8 (was B) */

  H = 0x48;
  L = 0x1E;

  do { /* $CC10-$CC29: screen third 2, all 8 character rows */
    B_scanline = 8;
    do
    {
      memset(ADDRTOSCREEN((H << 8) | L) - 28, 0, 28);
      H++;
    } while (--B_scanline);
    H  = 0x48;
    L += 0x20;
  } while (L <= 0xFF);
  L &= 0xFF;
  H  = 0x50;

  do { /* $CC2E-$CC49: screen third 3, top 5 character rows only */
    B_scanline = 8;
    do
    {
      memset(ADDRTOSCREEN((H << 8) | L) - 28, 0, 28);
      H++;
    } while (--B_scanline);
    H  = 0x50;
    L += 0x20;
  } while (L < 0xA0);
}

#define SEQ_END_BIT (0x80) /* bit7: tested via (s8) < 0; cleared via &~ to recover the payload */

/* Pitch-offset sequences: real data from bank3.bin's $F07C table
 * (24 x 1-byte self-referential-displacement entries, each pointing
 * to a byte sequence terminated by a bit-7-set marker byte). See
 * decode_pattern_command's pitch-select branch ($EE55-$EE6C). */
static const u8 title_pitch_offset_seq_00[] = { SEQ_END_BIT };
static const u8 title_pitch_offset_seq_01[] = { 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEQ_END_BIT };
static const u8 title_pitch_offset_seq_02[] = { 0x00, 0x04, SEQ_END_BIT | 0x07 };
static const u8 title_pitch_offset_seq_03[] = { 0x00, 0x03, SEQ_END_BIT | 0x08 };
static const u8 title_pitch_offset_seq_04[] = { 0x00, 0x05, SEQ_END_BIT | 0x09 };
static const u8 title_pitch_offset_seq_05[] = { 0x00, 0x00, 0x00, 0x03, 0x03, SEQ_END_BIT | 0x03 };
static const u8 title_pitch_offset_seq_06[] = { 0x00, 0x00, 0x00, 0x05, 0x05, SEQ_END_BIT | 0x05 };
static const u8 title_pitch_offset_seq_07[] = { 0x00, 0x00, 0x00, 0x04, 0x04, SEQ_END_BIT | 0x04 };
static const u8 title_pitch_offset_seq_08[] = { 0x00, SEQ_END_BIT | 0x0C };
static const u8 title_pitch_offset_seq_09[] = { 0x00, 0x00, 0x00, 0x08, 0x08, SEQ_END_BIT | 0x08 };
static const u8 title_pitch_offset_seq_10[] = { 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, SEQ_END_BIT };
static const u8 title_pitch_offset_seq_11[] = { 0x00, 0x00, 0x03, 0x03, 0x07, SEQ_END_BIT | 0x07 };
static const u8 title_pitch_offset_seq_12[] = { 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEQ_END_BIT };
static const u8 title_pitch_offset_seq_13[] = { 0x00, 0x05, 0x07, SEQ_END_BIT | 0x0C };
static const u8 title_pitch_offset_seq_14[] = { 0x00, 0x05, 0x0A, SEQ_END_BIT | 0x0C };
static const u8 title_pitch_offset_seq_15[] = { 0x00, 0x05, 0x09, SEQ_END_BIT | 0x0C };
static const u8 title_pitch_offset_seq_16[] = { 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x07, 0x07, 0x07, 0x07, 0x0C, 0x0C, 0x0C, SEQ_END_BIT | 0x0C };
static const u8 title_pitch_offset_seq_17[] = { 0x1C, 0x01, 0x0B, 0x01, SEQ_END_BIT | 0x7C };
static const u8 title_pitch_offset_seq_18[] = { SEQ_END_BIT | 0x6F };
static const u8 title_pitch_offset_seq_19[] = { SEQ_END_BIT | 0x48 };
static const u8 title_pitch_offset_seq_20[] = { 0x51, 0x01, 0x3E, 0x01, 0x2C, 0x01, 0x1C, 0x01, 0x0B, 0x01, SEQ_END_BIT | 0x7C };
static const u8 title_pitch_offset_seq_21[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEQ_END_BIT };
static const u8 title_pitch_offset_seq_22[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEQ_END_BIT };
static const u8 title_pitch_offset_seq_23[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEQ_END_BIT };

/* Envelope-shape sequences: real data from bank3.bin's $F123 table
 * (11 x 2-byte pointers; byte at ptr-1 is the envelope speed, sequence
 * runs from ptr until a bit-7-set halt marker). See
 * decode_pattern_command's envelope-select branch ($EE7E-$EE93). */
static const u8 title_envelope_shape_00[] = { 0x0F, 0x0F, 0x0E, 0x0D, 0x0C, 0x08, SEQ_END_BIT | 0x07 };
static const u8 title_envelope_shape_01[] = { 0x0E, 0x0F, 0x0E, SEQ_END_BIT | 0x07 };
static const u8 title_envelope_shape_02[] = { 0x0E, 0x0F, 0x0B, SEQ_END_BIT | 0x07 };
static const u8 title_envelope_shape_03[] = { 0x0F, 0x0F, 0x0F, 0x0E, 0x0D, 0x0A, SEQ_END_BIT | 0x07 };
static const u8 title_envelope_shape_04[] = { 0x0F, 0x09, 0x08, 0x06, 0x05, 0x04, 0x03, SEQ_END_BIT | 0x07 };
static const u8 title_envelope_shape_05[] = { 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x06, SEQ_END_BIT | 0x07 };
static const u8 title_envelope_shape_06[] = { 0x0F, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, SEQ_END_BIT | 0x07 };
static const u8 title_envelope_shape_07[] = { 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, SEQ_END_BIT | 0x07 };
static const u8 title_envelope_shape_08[] = { 0x0F, 0x0E, 0x0C, 0x06, SEQ_END_BIT | 0x07 };
static const u8 title_envelope_shape_09[] = { 0x0F, 0x0D, 0x0C, 0x0A, 0x08, 0x07, 0x06, SEQ_END_BIT | 0x07 };
static const u8 title_envelope_shape_10[] = { 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, SEQ_END_BIT | 0x07 };

static const struct
{
  const u8 *base;
} pitch_offset_table[24] = {
  { &title_pitch_offset_seq_00[0] },
  { &title_pitch_offset_seq_01[0] },
  { &title_pitch_offset_seq_02[0] },
  { &title_pitch_offset_seq_03[0] },
  { &title_pitch_offset_seq_04[0] },
  { &title_pitch_offset_seq_05[0] },
  { &title_pitch_offset_seq_06[0] },
  { &title_pitch_offset_seq_07[0] },
  { &title_pitch_offset_seq_08[0] },
  { &title_pitch_offset_seq_09[0] },
  { &title_pitch_offset_seq_10[0] },
  { &title_pitch_offset_seq_11[0] },
  { &title_pitch_offset_seq_12[0] },
  { &title_pitch_offset_seq_13[0] },
  { &title_pitch_offset_seq_14[0] },
  { &title_pitch_offset_seq_15[0] },
  { &title_pitch_offset_seq_16[0] },
  { &title_pitch_offset_seq_17[0] },
  { &title_pitch_offset_seq_18[0] },
  { &title_pitch_offset_seq_19[0] },
  { &title_pitch_offset_seq_20[0] },
  { &title_pitch_offset_seq_21[0] },
  { &title_pitch_offset_seq_22[0] },
  { &title_pitch_offset_seq_23[0] },
};

// $F123
static const struct
{
  const u8 *base;
  u8        speed;
} envelope_shape_table[16] = {
  { &title_envelope_shape_00[0], 1 },
  { &title_envelope_shape_01[0], 2 },
  { &title_envelope_shape_02[0], 2 },
  { &title_envelope_shape_03[0], 4 },
  { &title_envelope_shape_04[0], 4 },
  { &title_envelope_shape_05[0], 0 },
  { &title_envelope_shape_06[0], 2 },
  { &title_envelope_shape_07[0], 6 },
  { &title_envelope_shape_08[0], 0 },
  { &title_envelope_shape_09[0], 1 },
  { &title_envelope_shape_10[0], 2 },
};

/* title_tune_channel.status bits (see State.h). */
#define CHSTATUS_TOGGLE            (0x01) /* bit0: toggled every compute_channel_ay_registers call */
#define CHSTATUS_BIT1              (0x02) /* bit1: set by pcmd_set_status_bit1; consumer not established */
#define CHSTATUS_SLIDE_ACTIVE      (0x04) /* bit2: portamento/slide countdown active */
#define CHSTATUS_SLIDE_UPKEEP      (0x08) /* bit3: channel_slide_upkeep runs while set */
#define CHSTATUS_ENVELOPE_ACTIVE   (0x20) /* bit5: envelope amplitude advance active */
#define CHSTATUS_SLIDE_UPKEEP_UP   (0x80) /* bit7: slide-upkeep direction, set = ascend */
#define CHSTATUS_SLIDE_UPKEEP_ON   (CHSTATUS_SLIDE_UPKEEP | CHSTATUS_SLIDE_UPKEEP_UP) /* pcmd_set_status_bits_3_7 */

/* title_tune_channel.flags bits (see State.h). */
#define CHFLAGS_VIBRATO_ASCENDING  (0x20) /* bit5: vibrato triangle-wave direction */
#define CHFLAGS_VIBRATO_ENABLE     (0x40) /* bit6: vibrato applied in compute_channel_ay_registers */
#define CHFLAGS_VIBRATO_UPDATE_GATE (0x80) /* bit7: gates whether the phase updates this call */
#define CHFLAGS_VIBRATO_ON_MODE2   (CHFLAGS_VIBRATO_ENABLE | CHFLAGS_VIBRATO_UPDATE_GATE) /* pcmd_vibrato_on_mode2 */

/* title_tune_channel.slide_update_flag bits (see State.h). */
#define CHSLIDE_ECHO_NOTE          (0x01) /* bit0: echo new note to title_music.shared_note_value */

/* title_tune_channel.mixer_mask bits: AY mixer register bit groups. */
#define CHMIXER_TONE_MASK          (0x07) /* bits 0-2: tone-enable bits, per pcmd_set_mixer_bits_low3 */
#define CHMIXER_NOISE_MASK         (0x38) /* bits 3-5: noise-enable bits, per pcmd_set_mixer_bits_high3 */

/* title_tune_channel.mute_pending: one-shot mute-transition gate. */
#define CHMUTE_PENDING             (0xFF) /* set by pcmd_mute_channel and row-counter-reset normalisation */
#define CHMUTE_GATE_BIT            (0x80) /* tested via (s8) < 0; cleared once consumed */

/* advance_channel_pattern's pattern-command dispatch bytes ($EE49
 * decode_pattern_command); see this function's own Conv note for how the
 * mapping below was recovered. */
#define PCMD_RESET_ROW_COUNTER_CLEAR_ENV (0x80) /* pcmd_reset_row_counter_clear13 $EDBC */
#define PCMD_VIBRATO_OFF                 (0x81) /* pcmd_vibrato_off $EDA5 */
#define PCMD_VIBRATO_ON                  (0x82) /* pcmd_vibrato_on $EDAA */
#define PCMD_VIBRATO_ON_MODE2            (0x83) /* pcmd_vibrato_on_mode2 $EDB0 */
#define PCMD_SET_SLIDE_TARGET            (0x84) /* pcmd_set_slide_target $ED6F */
#define PCMD_SET_STATUS_BITS_3_7         (0x86) /* pcmd_set_status_bits_3_7 $ED9B */
#define PCMD_ADVANCE_PHRASE              (0x87) /* advance_channel_phrase $F1AE */
#define PCMD_SET_ENVELOPE_PARAMS         (0x88) /* pcmd_set_envelope_params $ED8C */
#define PCMD_SET_DRIVER_FLAG             (0x89) /* pcmd_set_driver_flag $ED85 */
#define PCMD_SET_MIXER_BITS_HIGH3        (0x8A) /* pcmd_set_mixer_bits_high3 $ED4B */
#define PCMD_SET_MIXER_BITS_LOW3         (0x8B) /* pcmd_set_mixer_bits_low3 $ED36 */
#define PCMD_CLEAR_MIXER_BITS            (0x8C) /* pcmd_clear_mixer_bits $ED5F */
#define PCMD_SET_STATUS_BIT1             (0x8D) /* pcmd_set_status_bit1 $EDB6 */
#define PCMD_END_OF_TUNE                 (0x8E) /* $ED2F POP HL / JP $ED0B -- end of tune */
#define PCMD_RESET_ROW_COUNTER           (0x8F) /* pcmd_reset_row_counter $EDC5 */
#define PCMD_MUTE_CHANNEL                (0x90) /* pcmd_mute_channel $EDCB */
#define PCMD_UNMUTE_CHANNEL              (0x91) /* pcmd_unmute_channel $EDD1 */
#define PCMD_RESET_ROW_COUNTER_ALT       (0xA8) /* lands exactly on acp_reset_row_counter $EE22 */

/* Command-byte range bases: below PCMD_TEMPO_BASE is the switch above;
 * PCMD_TEMPO_BASE..PCMD_PITCH_OFFSET_BASE-1 sets tempo; ..PCMD_ENVELOPE_SHAPE_BASE-1
 * selects a pitch-offset sequence; ..PCMD_ROW_WAIT_BASE-1 selects an envelope
 * shape; PCMD_ROW_WAIT_BASE upward sets the row-wait reload value. */
#define PCMD_TEMPO_BASE            (0xB0)
#define PCMD_PITCH_OFFSET_BASE     (0xB8)
#define PCMD_ENVELOPE_SHAPE_BASE   (0xD0)
#define PCMD_ROW_WAIT_BASE         (0xE0)

/**
 * $EB9E (bank 3): Start playing a tune (AY-3-8912 title-tune driver)
 *
 * Part of the 128K animated title screen's music driver. Looks up the given
 * tune's 7-byte entry (1 tempo/speed byte followed by 3 x 2-byte
 * pattern-data pointers, one per channel) in the tune-select table at
 * $F225, then initialises the 3 channel-tracker records at
 * $EC01/$EC26/$EC4B: resets misc playback state, stores each channel's raw
 * pattern-data pointer, follows it to read a 2-byte header from the start of
 * the pattern data itself (dereferenced into the channel's actual
 * pattern-command read pointer -- every pattern begins with this header),
 * sets the initial speed/divider and counter, and enables the channel.
 * Finally it clears the pattern_driver_flag scratch byte, forces an
 * immediate tempo refresh, and arms the tune-active flag for
 * titlescr_ay_music ($EC71) to pick up on its next call.
 *
 * \param[in] A_tune Tune number to start; index into the 7-byte-stride
 *                   tune-select table at $F225. (was A)
 *
 * Conv: $EBA6-$EBAB computes BC = A_tune * 7 via a repeated doubling/add
 *       sequence (the Z80 has no multiply instruction); C uses a direct
 *       multiply.
 *
 * Conv: the Z80 counts a channel counter down from 3 to 1 in A while IX walks
 *       the 37-byte-stride channel records ($EBBD/$EBF1); C counts
 *       channel_index up from 0 to 2 and indexes state->title_music.channel[]
 *       directly, which is equivalent and matches how advance_channel_pattern
 *       and compute_channel_ay_registers already receive a channel pointer.
 *
 * Conv: the pattern-data blocks tunes's pointers reference have been extracted
 *       from bank3.bin as C data for all 4 tunes -- the title tune, the
 *       perp-caught success jingle, and the two name-entry-screen tunes (see
 *       title_tune0_data/title_tune1_data/title_tune23_data above).
 *       DE_pattern_addr (the raw Z80 pointer read from the table) is resolved
 *       to a C pointer into one of those blobs via resolve_phrase_addr;
 *       pattern_ptr is then seeded by following that pointer to the 2-byte
 *       envelope-pointer header every pattern begins with, exactly as the Z80
 *       does. pattern_base/pattern_len (State.h, Conv fields with no Z80
 *       counterpart) cover only a fixed prefix of the real tune, not the whole
 *       thing, and let advance_channel_pattern wrap back to the start once it
 *       runs off the end rather than reading out of bounds; the lengths come
 *       from tune_pattern_lens below.
 *
 * Conv: pitch_offset_default/_cur and envelope_shape_default/_ptr are set for
 *       real once a note stream issues the pattern-command bytes $B8-$CF
 *       (pitch-offset select) or $D0-$DF (envelope-shape select) -- see
 *       dispatch_pattern_command's pitch_offset_table/envelope_shape_table
 *       handling. Before that first select command runs, though, a note event
 *       still dereferences both unconditionally (advance_channel_pattern's
 *       note-value branch, compute_channel_ay_registers' phase 1/2), so
 *       titlescr_start_ay seeds all 3 channels with
 *       default_pitch_offset_seq/default_envelope_shape (below): synthetic
 *       single-entry tables, not transcribed Z80 data, that decode to "no pitch
 *       offset" / "constant amplitude 15" so playback is audible and stable
 *       rather than crashing before the first select command.
 */
static void titlescr_start_ay(chqstate_t *state, u8 A_tune)
{
  static const u8 default_pitch_offset_seq[] = { SEQ_END_BIT }; /* Conv: marker bit set, payload 0 -- always resets to itself with zero offset */
  static const u8 default_envelope_shape[]   = { 0x0F, SEQ_END_BIT }; /* Conv: constant amplitude 15, then a halt marker */

  /* Conv: byte length of the fixed wraparound prefix extracted for each
   * tune/channel (see pattern_base/pattern_len in the prologue above); not
   * itself Z80 data. */
  static const u16 tune_pattern_lens[4][3] = {
    { 157, 160, 447 },
    { 190, 173, 156 },
    { 75, 129, 134 },
    { 79, 162, 172 }
  };

  const tune_t         *HL_tune_entry;   /* -> this tune's entry in the tune-select table (was HL) */
  int                   channel_index;   /* channel 0..2 (was A, counted down 3..1 in the Z80) */
  title_tune_channel_t *IX_channel;      /* this channel's tracker record (was IX) */
  u16                   DE_pattern_addr; /* raw Z80 address of this channel's pattern-data block, read from the tune-select table (was DE) */

  /* $EB9E-$EBA4: clear the tune-active flag and its companion byte. */
  state->bank3->title_music.tune_active           = 0;
  state->bank3->title_music.tune_active_companion = 0;

  /* $EBA6-$EBB1: HL -> this tune's entry in the tune-select table. Conv:
   * indexes tune_t directly rather than computing a 7-byte
   * stride byte offset (BC = A_tune * 7) into a flat byte array. */
  assert(A_tune < NELEMS(tunes));
  HL_tune_entry = &tunes[A_tune];

  /* $EBB2-$EBB6: tempo/speed byte, saved for later use. */
  state->bank3->title_music.tune_tempo = HL_tune_entry->tempo;

  /* $EBB7-$EBBD: IX -> first channel-tracker record; BC = 37 (record
   * stride, folded into array indexing below). */

  /* $EBBD-$EBF4 stu_channel_loop: initialise all 3 channel-tracker
   * records. */
  for (channel_index = 0; channel_index < 3; channel_index++)
  {
    IX_channel = &state->bank3->title_music.channel[channel_index];

    /* $EBBF-$EBC2: read this channel's pattern-data pointer from the tune
     * table. */
    DE_pattern_addr = HL_tune_entry->channel_pattern_addr[channel_index];

    /* $EBC4-$EBC8: reset misc playback state for this channel. */
    IX_channel->transpose           = 0; /* +$20 */
    IX_channel->phrase_repeat_count = 0; /* +$21 */

    /* Conv: seed the pitch-offset/envelope-shape pointers with the synthetic
     * safe defaults (see prologue) rather than leaving them NULL. */
    IX_channel->pitch_offset_default   = default_pitch_offset_seq; /* +$09/$0A */
    IX_channel->pitch_offset_cur       = default_pitch_offset_seq; /* +$0B/$0C */
    IX_channel->envelope_shape_default = default_envelope_shape;   /* +$14/$15 */
    IX_channel->envelope_shape_ptr     = default_envelope_shape;   /* +$16/$17 */

    /* $EBCC: enable the channel. */
    IX_channel->row_wait = 1; /* +$10 */

    /* $EBD0-$EBD6: clear note/status. */
    IX_channel->status       = 0; /* +$00 */
    IX_channel->flags        = 0; /* +$1D */
    IX_channel->mute_pending = 0; /* +$1F */

    /* $EBD9-$EBDC: store the raw pattern-data pointer. */
    assert(A_tune < NELEMS(tune_pattern_lens));
    IX_channel->pattern_data_ptr = resolve_phrase_addr(DE_pattern_addr); /* +$03/+$04 */

    /* $EBDF-$EBE2: follow the pattern pointer to read a second,
     * effect/envelope pointer from the start of the pattern data itself --
     * every pattern begins with an envelope-pointer header. */
    IX_channel->pattern_ptr  = resolve_phrase_addr(wordat(IX_channel->pattern_data_ptr)); /* +$01/+$02 */
    IX_channel->pattern_base = IX_channel->pattern_ptr; /* Conv: wraparound base, see prologue */
    IX_channel->pattern_len  = tune_pattern_lens[A_tune][channel_index]; /* Conv: wraparound length, see prologue */

    /* $EBE3-$EBE7: phrase-table cursor starts just past the 2-byte header;
     * no phrase is active yet. */
    IX_channel->phrase_table_offset = 2;    /* +$05/$06 */
    IX_channel->phrase_ptr          = NULL; /* +$22/$23: Conv: real Z80 never
                                              * initialises this either; the
                                              * first advance_channel_phrase
                                              * call always underflows
                                              * phrase_repeat_count first
                                              * (see State.h). */
  }

  /* $EBF6: clear a driver-internal flag. */
  state->bank3->title_music.pattern_driver_flag = 0;

  /* $EBF9-$EBFA: mark the tempo counter for an immediate refresh. */
  state->bank3->title_music.tempo_counter = 1;

  /* $EBFD: flag the tune as active. */
  state->bank3->title_music.tune_active = 1;
}

/**
 * $EC71 (bank 3): Per-frame title-tune music service
 *
 * Top-level per-tick driver for the 128K title screen's tune engine. If no
 * tune is active (title_music.tune_active is 0), does nothing. Otherwise:
 * decrements the tempo counter ($EC70) and, once it reaches zero,
 * re-processes all 3 channel-tracker records (advance_channel_pattern) one
 * tracker row further and reloads the counter; then (every frame, tick or
 * not) recomputes the AY tone-period/volume register values for all 3
 * channels from their current tracker state (compute_channel_ay_registers)
 * into the register cache at title_ay_regs; finally flushes the full cached
 * register block to the AY chip via titlescr_write_ay_registers.
 *
 * Called once per frame by the routine at $F82F.
 *
 * Conv: the skool re-tests title_music.tune_active a second time at $ECCA
 *       (tms_output_registers) before flushing, since that label is also
 *       reached directly by the $EC75 JP Z when no tune is active. Nothing
 *       between the two tests can change the flag, so the second check here is
 *       equivalent to the first -- both are kept, matching the two-guard
 *       structure of the original.
 *
 * $EC99 "LD A,$01" is self-modifying: its operand byte at $EC9A is the same
 * storage as title_music.tune_tempo, written by start_tune ($EBB3) and by
 * the PCMD_TEMPO_BASE pattern command ($EE71). The reload therefore uses
 * whatever tempo the current tune (or an in-song tempo command) has set,
 * not a fixed 1 -- confirmed against a real-hardware trace, which showed a
 * period-2 fire/skip alternation for tune 0 (tempo byte 0x02).
 */
static void titlescr_ay_music(chqstate_t *state)
{
  title_tune_channel_t *IX_channel; /* this channel's tracker record (was IX) */
  u16                   HL_period;  /* tone period returned per channel (was HL) */
  u8                    A_volume;   /* volume/mixer byte returned per channel (was A) */

  /* $EC71-$EC75: tune-active flag; skip straight to the output guard when no
   * tune is playing. */
  if (state->bank3->title_music.tune_active)
  {
    /* $EC78-$EC7A: clear a driver-internal flag (consumed elsewhere in the
     * pattern processing, not traced in the skool). */
    state->bank3->title_music.driver_internal_flag = 0; /* $ECC6 (SM) */

    /* $EC7D-$EC81: decrement the tempo counter; only re-process the tracker
     * patterns when it reaches zero. */
    if (--state->bank3->title_music.tempo_counter == 0)
    {
      /* $EC84-$EC88: advance channel 1's pattern by one tracker row.
       *
       * Conv: any of the three may hit the tune's end-of-data command (0x8E),
       * whose Z80 handler unwinds the stack past this function and returns
       * straight to titlescr_music. The tune is stopped and silenced by then,
       * so returning here is equivalent. */
      IX_channel = &state->bank3->title_music.channel[0];
      if (advance_channel_pattern(state, IX_channel))
        return;

      /* $EC8B-$EC8F: channel 2. */
      IX_channel = &state->bank3->title_music.channel[1];
      if (advance_channel_pattern(state, IX_channel))
        return;

      /* $EC92-$EC96: channel 3. */
      IX_channel = &state->bank3->title_music.channel[2];
      if (advance_channel_pattern(state, IX_channel))
        return;

      /* $EC99-$EC9B: reset the tempo counter from the tune's tempo byte --
       * self-modifying code, see prologue note above. */
      state->bank3->title_music.tempo_counter = state->bank3->title_music.tune_tempo;
    }

    /* $EC9E-$ECA8 tms_refresh_registers: recompute the AY register values
     * for channel 1 from its current tracker state. */
    IX_channel = &state->bank3->title_music.channel[0];
    HL_period  = compute_channel_ay_registers(state, IX_channel, &A_volume);
    state->bank3->title_ay_regs.chan_a_pitch = HL_period;
    state->bank3->title_ay_regs.chan_a_vol   = A_volume;

    /* $ECAB-$ECB5: channel 2. */
    IX_channel = &state->bank3->title_music.channel[1];
    HL_period  = compute_channel_ay_registers(state, IX_channel, &A_volume);
    state->bank3->title_ay_regs.chan_b_pitch = HL_period;
    state->bank3->title_ay_regs.chan_b_vol   = A_volume;

    /* $ECB8-$ECC2: channel 3. */
    IX_channel = &state->bank3->title_music.channel[2];
    HL_period  = compute_channel_ay_registers(state, IX_channel, &A_volume);
    state->bank3->title_ay_regs.chan_c_pitch = HL_period;
    state->bank3->title_ay_regs.chan_c_vol   = A_volume;

    /* $ECC5-$ECC7: the operand of this "LD A,$00" is $ECC6 --
     * driver_internal_flag -- the same self-modified byte that
     * compute_channel_ay_registers may have just patched (during any of the
     * three calls above, at $EF82 or $EFA7). So this reads back whatever
     * value was last patched there rather than actually loading a literal 0;
     * see the Conv note above and the driver_internal_flag field comment in
     * State.h. */
    state->bank3->title_ay_regs.noise_pitch = state->bank3->title_music.driver_internal_flag;
  }

  /* $ECCA-$ECCE tms_output_registers: re-check the tune-active flag -- see
   * Conv note above -- registers are only ever written while a tune is
   * active. */
  /* $ECCF-$ECE2: output the cached register block. */
  if (state->bank3->title_music.tune_active)
    titlescr_write_ay_registers(state);

  /* $ECE2: RET back to titlescr_music -- pairs with the CALL $EC71 overhead
   * billed at the call site. */
  state->speccy->logtime(state->speccy, 10);
}

/**
 * $ECD2 (bank 3): Flush the title-tune AY register cache to hardware
 *
 * Writes registers 11 down to 0 from the title-tune AY register soft-copy
 * block (title_ay_regs.env_fine..title_ay_regs.chan_a_pitch) by selecting
 * each register via port $FFFD then writing its value via port $BFFD. Same
 * shape as write_audio_registers_128k's flush of the separate in-game AY
 * block at $A213 -- kept as a distinct function because it walks the
 * title-tune engine's own register cache at $EFAF-$EFBA, not the in-game one.
 *
 * Conv: the Z80 uses the OUTD instruction (LD B,$FF / OUT (C),A / LD B,$BF /
 *       OUTD in sequence); C issues two separate out() calls per register, as
 *       in write_audio_registers_128k.
 */
static void titlescr_write_ay_registers(chqstate_t *state)
{
  zxspectrum_t *speccy; /* ZX Spectrum callbacks (Conv: added) */
  const u8     *values; /* pointer walking title AY soft copies downward (was HL) */
  int           reg;    /* AY register index, 11 down to 0 (was A) */

  speccy = state->speccy;

  /* $ECCF-$ECD4: LD HL,$EFBA / LD A,$0B / LD C,$FD (10+7+7=24). */
  speccy->logtime(speccy, 24);
  values = &state->bank3->title_ay_regs.env_fine;
  reg    = AY_REG_ENVELOPE_FINE_DURATION;
  do
  {
    /* $ECD6-$ECDF: LD B,$FF / OUT (C),A / LD B,$BF / OUTD / DEC A / JP P
     * (7+12+7+16+4+10=56). */
    speccy->logtime(speccy, 56);
    speccy->out(speccy, port_AY_REGISTER, reg);
    speccy->out(speccy, port_AY_DATA, *values--); /* was OUTD */
  } while (--reg >= 0);

  /* $ECE2: RET -- pairs with the CALL overhead billed at the call site
   * inside titlescr_ay_music. */
  speccy->logtime(speccy, 10);
}

/**
 * $ECED (bank 3): Silence every AY-3-8912 register
 *
 * Writes 0 to registers 13 downto 0, then belt-and-braces re-writes
 * register 7 (mixer) with 0 a second time. Same select-then-write shape as
 * titlescr_write_ay_registers.
 *
 * Conv: two speccy->out calls per register, as in titlescr_write_ay_registers.
 */
static void titlescr_silence_ay(chqstate_t *state)
{
  zxspectrum_t *speccy; /* ZX Spectrum callbacks (Conv: added) */
  int           reg;    /* AY register index, 13 down to 0 (was D) */

  speccy = state->speccy;
  reg    = AY_REG_ENVELOPE_SHAPE;
  do
  {
    speccy->out(speccy, port_AY_REGISTER, reg);
    speccy->out(speccy, port_AY_DATA, 0);
  } while (--reg >= 0);

  /* $ECFF-$ED0A: belt-and-braces re-write of register 7 (mixer) = 0. */
  speccy->out(speccy, port_AY_REGISTER, AY_REG_MIXER);
  speccy->out(speccy, port_AY_DATA, 0);
}

/**
 * $ED0B (bank 3): Stop any playing tune and silence the AY chip
 *
 * Clears the tune-active flag, silences every AY register via
 * titlescr_silence_ay, then clears the title-tune engine's own soft copy of the
 * per-channel volume registers ($EFB7-$EFB9: title_ay_regs.chan_a_vol/
 * chan_b_vol/chan_c_vol) so a later titlescr_write_ay_registers flush cannot
 * resurrect the old volumes.
 *
 * Used by run_title_screen's test-mode and "any key" restart paths, and
 * by options_menu_driver.
 *
 * Conv: the skool's own comment at this address describes $EFB7-$EFB9 as "the
 *       per-channel mixer/noise register cache", but those addresses land on
 *       title_ay_regs.chan_a_vol/chan_b_vol/chan_c_vol by the same byte-offset
 *       arithmetic titlescr_write_ay_registers relies on (see that function's
 *       prologue) -- trusting the addresses over the prose, per this project's
 *       established practice for the $FE/$FF opcode discrepancy elsewhere in
 *       this bank.
 */
static void stop_music_and_silence(chqstate_t *state)
{
  state->bank3->title_music.tune_active = 0;

  titlescr_silence_ay(state);

  state->bank3->title_ay_regs.chan_a_vol = 0;
  state->bank3->title_ay_regs.chan_b_vol = 0;
  state->bank3->title_ay_regs.chan_c_vol = 0;
}

/**
 * Read one byte from a channel's pattern stream, advancing the cursor
 *
 * Called throughout advance_channel_pattern (its own $EDE4
 * acp_read_pattern_byte step, and again for each multi-byte command's
 * operand bytes) to fetch the next byte and move DE_pattern past it.
 *
 * \param[in]     IX_channel Pointer to this channel's tracker record. (was IX)
 * \param[in,out] DE_pattern Pattern-stream read cursor; advanced past the byte
 *                           read. (was DE)
 *
 * \return                   The byte at the cursor position before advancing.
 *                           (was A)
 *
 * Conv: pattern_ptr/pattern_base/pattern_len (State.h) are only populated for
 *       tunes 0 and 1 (see titlescr_start_ay) -- a channel with pattern_ptr ==
 *       NULL (tunes 2/3, not extracted) is treated as silent rather than
 *       dereferencing NULL. The wrap check below is against the whole extracted
 *       tune array, not this channel's own initial pattern_base/pattern_len
 *       prefix -- advance_channel_phrase's PCMD_ADVANCE_PHRASE can legitimately
 *       re-point DE_pattern far outside that channel's own header block, into a
 *       region physically owned by another channel's data within the same tune
 *       (see the tune0ch1 header at $F26B jumping to $F4E1). Bounding against
 *       pattern_len there mistook every such jump for running off the end of
 *       the transcribed data and reset the cursor back to pattern_base on the
 *       very next byte, permanently stuck replaying the 3-byte header
 *       (including PCMD_RESET_ROW_COUNTER_CLEAR_ENV, which zeroes volume) --
 *       the channel never spoke again.
 */
static u8 acp_read_byte(title_tune_channel_t *IX_channel, const u8 **DE_pattern)
{
  u8        A_byte;    /* byte read before advancing the cursor (was A) */
  const u8 *array_end; /* end of whichever tune array this channel's data lives in */

  A_byte = *(*DE_pattern)++;

  /* Conv: the wrap must be checked against the whole extracted tune array,
   * not this channel's own initial pattern_base/pattern_len prefix --
   * advance_channel_phrase's PCMD_ADVANCE_PHRASE can legitimately re-point
   * DE_pattern far outside that channel's own header block, into a region
   * physically owned by another channel's data within the same tune (see
   * the tune0ch1 header at $F26B jumping to $F4E1). Bounding against
   * pattern_len there mistook every such jump for running off the end of
   * the transcribed data and reset the cursor back to pattern_base on the
   * very next byte, permanently stuck replaying the 3-byte header
   * (including PCMD_RESET_ROW_COUNTER_CLEAR_ENV, which zeroes volume) --
   * the channel never spoke again.
   *
   * Conv: membership is tested against title_tune0_data's own bounds, not by
   * comparing against title_tune1_data's address -- the two arrays are
   * static, so nothing guarantees the compiler lays them out in declaration
   * order (an address-order check silently broke this way once already). */
  if (IX_channel->pattern_base >= title_tune0_data &&
      IX_channel->pattern_base < &title_tune0_data[NELEMS(title_tune0_data)])
    array_end = &title_tune0_data[NELEMS(title_tune0_data)];
  else if (IX_channel->pattern_base >= title_tune1_data &&
           IX_channel->pattern_base < &title_tune1_data[NELEMS(title_tune1_data)])
    array_end = &title_tune1_data[NELEMS(title_tune1_data)];
  else
    array_end = &title_tune23_data[NELEMS(title_tune23_data)];
  if (*DE_pattern >= array_end)
    *DE_pattern = &IX_channel->pattern_base[0]; /* Conv: wrap to extracted prefix start */
  return A_byte;
}

#define PHRASE_TABLE_RESET            0 /* table exhausted -- restart from this channel's own header */
#define PHRASE_TABLE_TRANSPOSE_PREFIX 1 /* next byte is an inline transpose override */
#define PHRASE_TABLE_REPEATING_ENTRY  2 /* 1-byte repeat count + 2-byte pointer follow */

/**
 * $EDD6: Advance one channel's pattern by one tracker row
 *
 * Decrements the per-row wait counter; while it is still counting down,
 * only the portamento/slide upkeep runs (nudging note_index by one
 * semitone). Once it reaches zero, reads and decodes the next byte(s) from
 * the pattern stream: a note value resets pitch/envelope pointers and
 * volume; a command/effect byte is dispatched via decode_pattern_command.
 *
 * Shared end-of-sequence marker bit applies to the pitch-offset and
 * envelope-shape sequence tables (e.g. title_pitch_offset_seq_* and
 * title_envelope_shape_* tables).
 *
 * Conv: $EE96 dispatch_pattern_command reaches the fixed-length handlers at
 *       $ED36-$EDD1 via a computed jump through a table at $EC9D that stores a
 *       1-byte displacement per command byte (0x80-0xAF) rather than a full
 *       address -- the same space-saving trick used by $EE59's pitch-offset
 *       table. The skool could not resolve this statically ("no entry-point
 *       markers"); the mapping below was recovered by reading bank3.bin
 *       directly and evaluating the displacement arithmetic for every byte
 *       value 0x80-0xAF. Only 17 of the 48 possible values resolve to one of
 *       the named handlers' entry points -- these become the `switch` cases
 *       below (0xA8 lands exactly on acp_reset_row_counter's entry point,
 *       $EE22, even though it isn't one of the primary pcmd_* handlers; 0x87
 *       lands on the orphaned "JP $F1AE" at $ED33, handled below via
 *       advance_channel_phrase). Two further values are reachable but do not
 *       target a handler entry point: 0x85 lands mid-instruction inside
 *       pcmd_set_status_bits_3_7 (skipping its first SET 7, executing only SET
 *       3). The remaining values (0x92-0xA7, 0xA9-0xAF) land on arbitrary bytes
 *       inside the handler block and are assumed unused. Both fall into the
 *       `default` case below, which -- unlike the Z80 -- treats them as a no-op
 *       rather than replicating undefined/crashing behaviour. See Translation
 *       notes for the full derivation.
 *
 * 0x8E is the tune's end-of-data marker: it lands on the bare "POP HL; JP
 * $ED0B" at $ED2F, which discards this function's own return address before
 * stopping the tune, so the RET at the end of stop_music_and_silence returns
 * two levels up -- past the rest of titlescr_ay_music, straight back to
 * titlescr_music at $F832. Confirmed by trace (byte 0x8E dispatched at
 * $EE9D, $ED0B reached, RET landing at $F832), which also settles the
 * question of how the title tune ever stops: nothing clears the tune-active
 * flag on a timer, the tune data ends with this command.
 *
 * \param[in,out] IX_channel Channel tracker record to advance. (was IX)
 *
 * \return                   1 when the tune ended this row (0x8E), so the
 *                           caller must abandon the rest of the frame's driver
 *                           work, as the Z80's stack unwind does. 0 otherwise.
 */
static u8 advance_channel_pattern(chqstate_t           *state,
                                  title_tune_channel_t *IX_channel)
{
  u8        A_status;   /* status byte tested by channel_slide_upkeep (was A, IX+$00) */
  const u8 *DE_pattern; /* pattern-command byte stream cursor (was DE, IX+$01/$02) */
  u8        A_byte;     /* raw byte just read from the pattern stream (was A) */
  u8        A_note;     /* raw note value + transpose, before storing to note_index (was A) */
  const u8 *HL_ptr;     /* scratch pointer, reused for the pitch-offset then envelope-shape reset (was HL) */
  u8        A_env_byte; /* envelope shape byte read at note time (was A) */
  u8        A_operand;  /* second operand byte of a 2-operand pattern command (was A) */
  u8        A_mix;      /* scratch accumulator for the replace-bits-under-mask mixer merge (was A) */

  if (IX_channel->pattern_ptr == NULL)
    return 0; /* Conv: tune not extracted (tunes 2/3) -- channel stays silent */

  /* $EDD6: DEC (IX+$10) (23) -- shared by both outcomes below. */
  state->speccy->logtime(state->speccy, 23);

  /* $EDD6-$EDD9: per-row wait countdown. */
  if (--IX_channel->row_wait != 0)
  {
    /* $EDD9-$EE3B: JR NZ taken (12); LD A,(IX+$00)/BIT 3,A (19+8=27). Total
     * 12+27=39. */
    state->speccy->logtime(state->speccy, 39);

    /* $EE38 channel_slide_upkeep: nudge the note by one semitone per frame
     * while a portamento/slide effect is active. */
    A_status = IX_channel->status;
    if (!(A_status & CHSTATUS_SLIDE_UPKEEP))
    {
      /* $EE3D: RET Z taken (11). */
      state->speccy->logtime(state->speccy, 11);
      return 0;
    }
    /* $EE3D: RET Z not taken (5). */
    state->speccy->logtime(state->speccy, 5);

    if (A_status & CHSTATUS_SLIDE_UPKEEP_UP) { // Conv: RLA + JR NC collapsed to a direct bit-7 test
      /* $EE3E-$EE44: RLA/JR NC not taken/INC (IX+$12)/RET (4+7+23+10=44). */
      state->speccy->logtime(state->speccy, 44);
      IX_channel->note_index++;
    } else
    {
      /* $EE3E-$EE48: RLA/JR NC taken/DEC (IX+$12)/RET (4+12+23+10=49). */
      state->speccy->logtime(state->speccy, 49);
      IX_channel->note_index--;
    }
    return 0;
  }
  /* $EDD9-$EDE1: JR NZ not taken (7); LD (IX+$00),B/LD E,(IX+$01)/
   * LD D,(IX+$02) (19+19+19=57). Total 7+57=64. */
  state->speccy->logtime(state->speccy, 64);

  /* $EDDB-$EDE1: countdown reached zero -- start reading this row's pattern
   * bytes. */
  IX_channel->status = 0;
  DE_pattern         = IX_channel->pattern_ptr;

  for (;;)
  {
    A_byte = acp_read_byte(IX_channel, &DE_pattern);

    /* $EDE4-$EDE7 acp_read_pattern_byte + sign test: LD A,(DE)/INC DE (this
     * pair recurs at every operand-byte read site below, each acp_read_byte
     * call); AND A/JP M (7+6+4+10=27). */
    state->speccy->logtime(state->speccy, 27);
    if (!(A_byte & 0x80))
    {
      /* $EDEA-$EDED: ADD A,(IX+$20)/LD (IX+$12),A (19+19=38). */
      state->speccy->logtime(state->speccy, 38);
      A_note = (u8) (A_byte + IX_channel->transpose);
      IX_channel->note_index = A_note;

      /* $EDF0: BIT 0,(IX+$1E) (20). */
      state->speccy->logtime(state->speccy, 20);
      if (IX_channel->slide_update_flag & CHSLIDE_ECHO_NOTE)
      {
        /* $EDF4: JR Z not taken (7); $EDF6: LD ($EC79),A (13). Total 20. */
        state->speccy->logtime(state->speccy, 20);
        state->bank3->title_music.shared_note_value = A_note; /* $EC79 (SM) */
      } else
      {
        /* $EDF4: JR Z taken (12). */
        state->speccy->logtime(state->speccy, 12);
      }

      /* $EDF9-$EE1E: LD L,(IX+$09)/LD H,(IX+$0A)/LD (IX+$0B),L/
       * LD (IX+$0C),H/LD L,(IX+$14)/LD (IX+$16),L/LD H,(IX+$15)/
       * LD (IX+$17),H (8x19=152); LD A,(HL)/LD (IX+$18),A/LD (IX+$13),A/
       * LD A,(IX+$0F)/LD (IX+$19),A (7+19+19+19+19=83); SET 5,(IX+$00) (23).
       * Total 152+83+23=258. */
      state->speccy->logtime(state->speccy, 258);
      HL_ptr = IX_channel->pitch_offset_default;    /* +$09/$0A */
      IX_channel->pitch_offset_cur = HL_ptr;        /* +$0B/$0C: reset to loop start */

      HL_ptr = IX_channel->envelope_shape_default;  /* +$14/$15 */
      IX_channel->envelope_shape_ptr = HL_ptr;      /* +$16/$17: reset envelope pointer */

      A_env_byte                     = *HL_ptr;
      IX_channel->envelope_amplitude = A_env_byte;  /* +$18 */
      IX_channel->volume             = A_env_byte;  /* +$13 */

      IX_channel->envelope_step_counter = IX_channel->envelope_speed; /* +$19 = +$0F */

      IX_channel->status |= CHSTATUS_ENVELOPE_ACTIVE;

      break; /* fall into reset_row_counter below */
    }

    /* $EE49 decode_pattern_command: command/effect byte. */
    if (A_byte < PCMD_TEMPO_BASE)
    {
      /* $EE49-$EE59 cascade (CP $B0/JR C not taken x3 folded into the
       * enclosing if/else-if chain -- see the else-if branches below) plus
       * $EE96-$EE9D dispatch_pattern_command: ADD HL,BC-style table walk and
       * JP (HL) landing on the handler entry point (47). Combined with this
       * branch's own CP/JR C cascade cost (19) the total dispatch overhead
       * for every switch case below is 66; see the Conv note above the
       * prologue for how the jump table was recovered. */
      state->speccy->logtime(state->speccy, 66);
      switch (A_byte)
      {
      case PCMD_RESET_ROW_COUNTER_CLEAR_ENV:
        /* $EDBC-$EDC3: LD (IX+$13),B/RES 5,(IX+$00)/JR $EE22 (19+23+12=54). */
        state->speccy->logtime(state->speccy, 54);
        IX_channel->volume  = 0;
        IX_channel->status &= (u8) ~CHSTATUS_ENVELOPE_ACTIVE;
        goto reset_row_counter;

      case PCMD_VIBRATO_OFF:
        /* $EDA5-$EDA8: LD (IX+$1D),B/JR $EDE4 (19+12=31). */
        state->speccy->logtime(state->speccy, 31);
        IX_channel->flags = 0;
        continue;

      case PCMD_VIBRATO_ON:
        /* $EDAA-$EDAE: LD (IX+$1D),$40/JR $EDE4 (19+12=31). */
        state->speccy->logtime(state->speccy, 31);
        IX_channel->flags = CHFLAGS_VIBRATO_ENABLE;
        continue;

      case PCMD_VIBRATO_ON_MODE2:
        /* $EDB0-$EDB4: LD (IX+$1D),$C0/JR $EDE4 (19+12=31). */
        state->speccy->logtime(state->speccy, 31);
        IX_channel->flags = CHFLAGS_VIBRATO_ON_MODE2;
        continue;

      case PCMD_SET_SLIDE_TARGET:
        IX_channel->slide_step      = (s8) acp_read_byte(IX_channel, &DE_pattern); /* operand 1 */
        IX_channel->slide_accum     = 0;
        IX_channel->status         |= CHSTATUS_SLIDE_ACTIVE;
        IX_channel->slide_countdown = acp_read_byte(IX_channel, &DE_pattern);      /* operand 2 */

        /* $ED6F-$ED83: LD A,(DE)/INC DE (operand 1 read, 7+6=13); LD (IX+$07),B/
         * LD (IX+$08),B/LD (IX+$0D),A (clear slide_accum + store slide_step,
         * 19+19+19=57); SET 2,(IX+$00) (23); LD A,(DE)/INC DE (operand 2 read,
         * 7+6=13); LD (IX+$0E),A (19); JR $EDE4 (12). Total 13+57+23+13+19+12=137. */
        state->speccy->logtime(state->speccy, 137);
        continue;

      case PCMD_SET_STATUS_BITS_3_7:
        /* $ED9B-$EDA3: SET 7,(IX+$00)/SET 3,(IX+$00)/JR $EDE4 (23+23+12=58). */
        state->speccy->logtime(state->speccy, 58);
        IX_channel->status |= CHSTATUS_SLIDE_UPKEEP_ON;
        continue;

      case PCMD_ADVANCE_PHRASE: /* see advance_channel_phrase's own prologue */
        /* $ED33: JP $F1AE (10). */
        state->speccy->logtime(state->speccy, 10);
        advance_channel_phrase(state, IX_channel, &DE_pattern);
        /* $F1F0: JP $EDE4 (10) -- advance_channel_phrase's own exit back into
         * this loop. */
        state->speccy->logtime(state->speccy, 10);
        continue;

      case PCMD_SET_ENVELOPE_PARAMS:
        IX_channel->vibrato_increment = acp_read_byte(IX_channel, &DE_pattern); /* operand 1 -> +$1B */
        A_operand = acp_read_byte(IX_channel, &DE_pattern);                     /* operand 2, stored twice */
        IX_channel->vibrato_depth = A_operand; /* +$1A */
        IX_channel->vibrato_phase = A_operand; /* +$1C */

        /* $ED8C-$ED99: LD A,(DE)/LD (IX+$1B),A/INC DE (operand 1, 7+19+6=32);
         * LD A,(DE)/INC DE (operand 2 read, 7+6=13); LD (IX+$1A),A/
         * LD (IX+$1C),A/JR $EDE4 (19+19+12=50). Total 32+13+50=95. */
        state->speccy->logtime(state->speccy, 95);
        continue;

      case PCMD_SET_DRIVER_FLAG:
        state->bank3->title_music.pattern_driver_flag = acp_read_byte(IX_channel, &DE_pattern);

        /* $ED85-$ED8A: LD A,(DE)/INC DE (7+6=13) -- operand read;
         * LD ($EED1),A/JR $EDE4 (13+12=25). Total 38. */
        state->speccy->logtime(state->speccy, 38);
        continue;

      case PCMD_SET_MIXER_BITS_HIGH3:
        /* $ED4B-$ED5C: LD A,(IX+$24)/LD C,A/AND $38/LD HL,$EF7A/XOR (HL)/
         * AND C/XOR (HL)/LD (HL),A/XOR A/LD (IX+$1E),A/JP $EDE4
         * (19+4+7+10+7+4+7+7+4+19+10=98). */
        state->speccy->logtime(state->speccy, 98);
        A_mix  = (u8) (IX_channel->mixer_mask & CHMIXER_NOISE_MASK);
        A_mix ^= state->bank3->title_music.pending_mixer_bits;
        A_mix &= IX_channel->mixer_mask;
        A_mix ^= state->bank3->title_music.pending_mixer_bits;
        state->bank3->title_music.pending_mixer_bits = A_mix;
        IX_channel->slide_update_flag                = 0;
        continue;

      case PCMD_SET_MIXER_BITS_LOW3:
        /* $ED36-$ED48: LD A,(IX+$24)/LD C,A/AND $07/LD HL,$EF7A/XOR (HL)/
         * AND C/XOR (HL)/LD (HL),A/LD A,$01/LD (IX+$1E),A/JP $EDE4
         * (19+4+7+10+7+4+7+7+7+19+10=101). */
        state->speccy->logtime(state->speccy, 101);
        A_mix  = (u8) (IX_channel->mixer_mask & CHMIXER_TONE_MASK);
        A_mix ^= state->bank3->title_music.pending_mixer_bits;
        A_mix &= IX_channel->mixer_mask;
        A_mix ^= state->bank3->title_music.pending_mixer_bits;
        state->bank3->title_music.pending_mixer_bits = A_mix;
        IX_channel->slide_update_flag                = CHSLIDE_ECHO_NOTE;
        continue;

      case PCMD_CLEAR_MIXER_BITS:
        /* $ED5F-$ED6D: LD HL,$EF7A/LD A,(IX+$24)/CPL/AND (HL)/LD (HL),A/
         * LD A,$01/LD (IX+$1E),A/JR $EDE4 (10+19+4+7+7+7+19+12=85). */
        state->speccy->logtime(state->speccy, 85);
        state->bank3->title_music.pending_mixer_bits =
          (u8) (~IX_channel->mixer_mask & state->bank3->title_music.pending_mixer_bits);
        IX_channel->slide_update_flag = CHSLIDE_ECHO_NOTE;
        continue;

      case PCMD_SET_STATUS_BIT1:
        /* $EDB6-$EDBA: SET 1,(IX+$00)/JR $EDE4 (23+12=35). */
        state->speccy->logtime(state->speccy, 35);
        IX_channel->status |= CHSTATUS_BIT1;
        continue;

      case PCMD_END_OF_TUNE:
        /* $ED2F-$ED33: POP HL/JP $ED0B (10+10=20). Conv: the Z80's POP HL
         * discards this function's return address so that
         * stop_music_and_silence's own RET unwinds past the rest of the
         * driver; C reports the unwind to the caller instead, so
         * stop_music_and_silence's own cost is billed inside that function,
         * not here (it is outside this task's scope). */
        state->speccy->logtime(state->speccy, 20);
        stop_music_and_silence(state);
        return 1;

      case PCMD_RESET_ROW_COUNTER:
        /* $EDC5-$EDC9: RES 5,(IX+$00)/JR $EE22 (23+12=35). */
        state->speccy->logtime(state->speccy, 35);
        IX_channel->status &= (u8) ~CHSTATUS_ENVELOPE_ACTIVE;
        goto reset_row_counter;

      case PCMD_MUTE_CHANNEL:
        /* $EDCB-$EDCF: LD (IX+$1F),$FF/JR $EDE4 (19+12=31). */
        state->speccy->logtime(state->speccy, 31);
        IX_channel->mute_pending = CHMUTE_PENDING;
        continue;

      case PCMD_UNMUTE_CHANNEL:
        /* $EDD1-$EDD4: LD (IX+$1F),B/JR $EDE4 (19+12=31). */
        state->speccy->logtime(state->speccy, 31);
        IX_channel->mute_pending = 0;
        continue;

      case PCMD_RESET_ROW_COUNTER_ALT:
        /* 0xA8 lands exactly on acp_reset_row_counter's entry point via the
         * dispatch table displacement -- no further Z80 instructions execute
         * before $EE22, so no additional bill beyond the shared dispatch
         * overhead above. */
        goto reset_row_counter;

      default:
        /* Ambiguous case, flagged rather than guessed: 0x85 lands
         * mid-instruction inside pcmd_set_status_bits_3_7, executing only
         * its SET 3,(IX+$00)/JR $EDE4 tail (23+12=35) -- exact. The other
         * values folded into this default (0x92-0xA7, 0xA9-0xAF) land on
         * arbitrary bytes inside unrelated handler blocks; their real cost
         * cannot be determined without disassembling those byte sequences as
         * opcodes, and none of them occur in the extracted tune data, so the
         * branch is dead in practice. Approximated with the same 35 as 0x85
         * -- see the Conv note above the prologue. */
        state->speccy->logtime(state->speccy, 35);
        continue;
      }
    } else if (A_byte < PCMD_PITCH_OFFSET_BASE)
    {
      /* $EE49-$EE74: CP $B0/JR C not taken/ADD A,$20/JR C not taken/
       * ADD A,$10/JR C not taken/ADD A,$18/JR NC taken (cascade,
       * 7+7+7+7+7+7+7+12=61); ADD A,$09/LD ($EC9A),A/JP $EDE4
       * (7+13+10=30). Total 61+30=91. */
      state->speccy->logtime(state->speccy, 91);
      /* $EE6F: set the tune tempo/speed byte. */
      state->bank3->title_music.tune_tempo = (u8) (A_byte - PCMD_TEMPO_BASE + 1);
      continue;
    } else if (A_byte < PCMD_ENVELOPE_SHAPE_BASE)
    {
      /* $EE49-$EE57 cascade (7+7+7+7+7+7+7+7=56); $EE59-$EE6C: LD C,A/
       * LD HL,$F07C/ADD HL,BC/LD C,(HL)/ADD HL,BC/LD (IX+$0B),L/
       * LD (IX+$09),L/LD (IX+$0C),H/LD (IX+$0A),H/JP $EDE4
       * (4+10+11+7+11+19+19+19+19+10=129). Total 56+129=185. */
      state->speccy->logtime(state->speccy, 185);
      /* $EE59-$EE6C: select a pitch-offset sequence via the $F07C table (24
       * entries). Sets both the "current" and "default" pointers immediately
       * (unlike the envelope-shape select below, which only sets the
       * default). */
      HL_ptr = pitch_offset_table[A_byte - PCMD_PITCH_OFFSET_BASE].base;
      IX_channel->pitch_offset_cur     = HL_ptr; /* +$0B/$0C */
      IX_channel->pitch_offset_default = HL_ptr; /* +$09/$0A */
      continue;
    } else if (A_byte < PCMD_ROW_WAIT_BASE)
    {
      /* $EE49-$EE53 cascade (7+7+7+7+7+12=47); $EE7E-$EE93: LD HL,$F123/
       * ADD A,A/LD C,A/ADD HL,BC/LD A,(HL)/INC HL/LD H,(HL)/LD L,A/
       * LD (IX+$14),L/LD (IX+$15),H/DEC HL/LD A,(HL)/LD (IX+$0F),A/JP $EDE4
       * (10+4+4+11+7+6+7+4+19+19+6+7+19+10=133). Total 47+133=180. */
      state->speccy->logtime(state->speccy, 180);
      /* $EE7E-$EE93: select an envelope shape via the $F123 pointer table (16
       * entries). Sets envelope_shape_default and envelope_speed;
       * envelope_shape_ptr is only reset from the default at the next note
       * event (see the note-value branch above). */
      IX_channel->envelope_shape_default = envelope_shape_table[A_byte - PCMD_ENVELOPE_SHAPE_BASE].base;  /* +$14/$15 */
      IX_channel->envelope_speed         = envelope_shape_table[A_byte - PCMD_ENVELOPE_SHAPE_BASE].speed; /* +$0F */
      continue;
    } else
    {
      /* $EE49-$EE4F cascade (7+7+7+12=33); $EE77-$EE7B: INC A/LD (IX+$11),A/
       * JP $EDE4 (4+19+10=33). Total 33+33=66. */
      state->speccy->logtime(state->speccy, 66);
      /* $EE77: set the per-row wait reload value. */
      IX_channel->row_wait_reload = (u8) (A_byte - PCMD_ROW_WAIT_BASE + 1); /* +$11 */
      continue;
    }
  }

reset_row_counter:
  IX_channel->row_wait    = IX_channel->row_wait_reload; /* +$10 = +$11 */
  IX_channel->pattern_ptr = DE_pattern;                  /* +$01/+$02 */

  /* $EE22-$EE31 acp_reset_row_counter: LD A,(IX+$11)/LD (IX+$10),A/
   * LD (IX+$02),D/LD (IX+$01),E (19+19+19+19=76); LD A,(IX+$1F)/AND A
   * (19+4=23). Total 99. */
  state->speccy->logtime(state->speccy, 99);
  if (IX_channel->mute_pending)
  {
    /* $EE32: RET Z not taken (5); $EE33-$EE37: LD (IX+$1F),$FF/RET
     * (19+10=29). Total 34. */
    state->speccy->logtime(state->speccy, 34);
    IX_channel->mute_pending = CHMUTE_PENDING; /* normalise any nonzero value to the one-shot gate */
    return 0;
  }
  /* $EE32: RET Z taken (11). */
  state->speccy->logtime(state->speccy, 11);
  return 0;
}

/**
 * $EE9E (bank 3): Recompute one channel's AY tone-period and mixer values
 *
 * Part of the 128K animated title screen's music driver (distinct from the
 * 48K play_music_48k engine, which happens to share this address in a
 * different skool/bank). Called once per channel per frame by
 * titlescr_ay_music ($EC71@bank3) to refresh the AY register cache. Pure
 * calculation: no I/O, no state beyond the channel's
 * own tracker record and the shared mixer-cache byte. Five phases, in order:
 *
 * 1. $EEA1-$EED0: envelope amplitude advance. If status bit 5 is set,
 *    decrements the envelope-step counter; on underflow, reloads it from the
 *    speed value and reads the next byte from the envelope-shape table
 *    pointer (advanced by 1 unless the byte read has bit 7 set, an
 *    end-of-table marker that halts the pointer), storing that byte as the
 *    current amplitude. Either way, the amplitude is copied into the volume
 *    field returned to the caller.
 * 2. $EED0-$EEFC: tone-period lookup. Note index (set elsewhere from the
 *    pattern's raw note + transpose) plus a pitch-offset byte read from a
 *    per-channel offset sequence (default/loop start on end-of-sequence)
 *    indexes the 2-byte-per-note tone-period table, giving the base tone
 *    period.
 * 3. $EEFC-$EF4B: vibrato. Only runs if flags bit 6 is set. The vibrato
 *    phase counter ping-pongs between 0 and (depth * 2) by +/-increment each
 *    call. The phase is centred, sign-extended, then left-shifted a
 *    note-dependent number of times to scale a linear vibrato depth into the
 *    tone-period domain (pitch and period are inversely related), and the
 *    scaled offset is added onto the phase-2 tone period.
 * 4. $EF4B-$EF76: portamento/slide. Status bit 0 is unconditionally toggled
 *    every call. If status bit 2 was set (slide active), a countdown ticks
 *    down; when it reaches zero it reloads and the signed step is
 *    sign-extended and accumulated, which is then added onto the tone
 *    period.
 * 5. $EF76-$EFAE: mixer/volume finalisation. Every 4th call, XORs bit 3 of a
 *    shared byte into a self-modified operand elsewhere in the driver
 *    (purpose not established). Merges this channel's tone-enable bits into
 *    the shared mixer cache using a replace-bits-under-mask idiom. If a
 *    one-shot mute was just requested, clears this channel's noise-enable
 *    bits in the mixer cache and forces that same self-modified operand to
 *    $41.
 *
 * \param[in]  IX_channel   Pointer to this channel's tracker record. (was IX)
 * \param[out] A_volume_out Receives the phase-1/-2 volume (+$13). (was A)
 *
 * \return                  The phase-3/4 tone period. (was HL)
 *
 * Conv: the Z80 pairs of EX DE,HL ($EF32/$EF4A, $EFAA) exist only to route
 *       operands through ADD HL,DE / RET's register contract; C adds the values
 *       directly and returns via the function's return value instead.
 *
 * Conv: at $EED0-$EED2 the Z80 does `LD A,$00; ADD A,(IX+$12)`, which is a
 *       roundabout way of just reading the note index; collapsed to a direct
 *       assignment here.
 *
 * Conv: at $EF33-$EF3D the Z80 manually sign-extends the centred vibrato phase
 *       into D via the carry flag from the preceding SUB. C's (s8)->(s16) cast
 *       performs the same sign extension natively.
 */
static u16 compute_channel_ay_registers(chqstate_t           *state,
                                        title_tune_channel_t *IX_channel,
                                        u8                   *A_volume_out)
{
  // clang-format off
  /**
   * $EFBC-$F07B: note_periods
   *
   * 128K bank 3: title-tune engine AY tone-period lookup table. 96 entries,
   * one per note, transcribed directly from the skool's DEFB bytes
   * (little-endian pairs). Used by compute_channel_ay_registers ($EE9E@bank3)
   * to convert a note index into an AY tone-period value. $F07C onward is a
   * different, unrelated table (an indexed pointer table, see $EE5A@bank3)
   * -- do not extend this array into it. Indices name-checked against enum
   * note_index above.
   */
  static const u16 note_periods[96] = {
    0x0EF8, /* NOTE_AS0 */
    0x0E10, /* NOTE_B0 */
    0x0D60, /* NOTE_C1 */
    0x0C80, /* NOTE_CS1 */
    0x0BD8, /* NOTE_D1 */
    0x0B28, /* NOTE_DS1 */
    0x0A88, /* NOTE_E1 */
    0x09F0, /* NOTE_F1 */
    0x0960, /* NOTE_FS1 */
    0x08E0, /* NOTE_G1 */
    0x0858, /* NOTE_GS1 */
    0x07E0, /* NOTE_A1 */
    0x077C, /* NOTE_AS1 */
    0x0708, /* NOTE_B1 */
    0x06B0, /* NOTE_C2 */
    0x0640, /* NOTE_CS2 */
    0x05EC, /* NOTE_D2 */
    0x0594, /* NOTE_DS2 */
    0x0544, /* NOTE_E2 */
    0x04F8, /* NOTE_F2 */
    0x04B0, /* NOTE_FS2 */
    0x0470, /* NOTE_G2 */
    0x042C, /* NOTE_GS2 */
    0x03F0, /* NOTE_A2 */
    0x03BE, /* NOTE_AS2 */
    0x0384, /* NOTE_B2 */
    0x0358, /* NOTE_C3 */
    0x0320, /* NOTE_CS3 */
    0x02F6, /* NOTE_D3 */
    0x02CA, /* NOTE_DS3 */
    0x02A2, /* NOTE_E3 */
    0x027C, /* NOTE_F3 */
    0x0258, /* NOTE_FS3 */
    0x0238, /* NOTE_G3 */
    0x0216, /* NOTE_GS3 */
    0x01F8, /* NOTE_A3 */
    0x01DF, /* NOTE_AS3 */
    0x01C2, /* NOTE_B3 */
    0x01AC, /* NOTE_C4 */
    0x0190, /* NOTE_CS4 */
    0x017B, /* NOTE_D4 */
    0x0165, /* NOTE_DS4 */
    0x0151, /* NOTE_E4 */
    0x013E, /* NOTE_F4 */
    0x012C, /* NOTE_FS4 */
    0x011C, /* NOTE_G4 */
    0x010B, /* NOTE_GS4 */
    0x00FC, /* NOTE_A4 */
    0x00EF, /* NOTE_AS4 */
    0x00E1, /* NOTE_B4 */
    0x00D6, /* NOTE_C5 */
    0x00C8, /* NOTE_CS5 */
    0x00BD, /* NOTE_D5 */
    0x00B2, /* NOTE_DS5 */
    0x00A8, /* NOTE_E5 */
    0x009F, /* NOTE_F5 */
    0x0096, /* NOTE_FS5 */
    0x008E, /* NOTE_G5 */
    0x0085, /* NOTE_GS5 */
    0x007E, /* NOTE_A5 */
    0x0077, /* NOTE_AS5 */
    0x0070, /* NOTE_B5 */
    0x006B, /* NOTE_C6 */
    0x0064, /* NOTE_CS6 */
    0x005E, /* NOTE_D6 */
    0x0059, /* NOTE_DS6 */
    0x0054, /* NOTE_E6 */
    0x004F, /* NOTE_F6 */
    0x004B, /* NOTE_FS6 */
    0x0047, /* NOTE_G6 */
    0x0042, /* NOTE_GS6 */
    0x003F, /* NOTE_A6 */
    0x003B, /* NOTE_AS6 */
    0x0038, /* NOTE_B6 */
    0x0035, /* NOTE_C7 */
    0x0032, /* NOTE_CS7 */
    0x002F, /* NOTE_D7 */
    0x002C, /* NOTE_DS7 */
    0x002A, /* NOTE_E7 */
    0x0027, /* NOTE_F7 */
    0x0025, /* NOTE_FS7 */
    0x0023, /* NOTE_G7 */
    0x0021, /* NOTE_GS7 */
    0x001F, /* NOTE_A7 */
    0x001D, /* NOTE_AS7 */
    0x001C, /* NOTE_B7 */
    0x001A, /* NOTE_C8 */
    0x0019, /* NOTE_CS8 */
    0x0017, /* NOTE_D8 */
    0x0016, /* NOTE_DS8 */
    0x0015, /* NOTE_E8 */
    0x0013, /* NOTE_F8 */
    0x0012, /* NOTE_FS8 */
    0x0011, /* NOTE_G8 */
    0x0010, /* NOTE_GS8 */
    0x000F /* NOTE_A8 */
  };
  // clang-format on

  u8         C_status;          /* channel status/flags byte, unchanged through most of the function (was C, IX+$00) */
  u8         A_env_step;        /* envelope-step counter, then reload value (was A, IX+$19) */
  const u8  *HL_env_shape;      /* envelope shape table pointer (was HL, IX+$16/$17) */
  u8         A_env_byte;        /* envelope shape byte just read (was A) */
  u8         B_note;            /* base note index for the tone lookup (was B, IX+$12) */
  const u8  *HL_offset_ptr;     /* pitch-offset sequence pointer (was HL, IX+$0B/$0C) */
  u8         A_offset_byte;     /* pitch-offset byte read from the sequence (was A) */
  u8         A_note_combined;   /* offset byte + note index, before doubling (was A) */
  u8         A_note_lookup;     /* doubled combined index: byte offset into the tone-period table, reused in phase 3 as the vibrato scale seed (was A/H) */
  u16        DE_period;         /* running tone period (was DE) */
  u8         B_vib_range;       /* doubled vibrato depth; clamp bound for the phase counter (was B, IX+$1A after SLA) */
  u8         A_vib_phase;       /* vibrato triangle-wave phase counter (was A, IX+$1C) */
  s16        DE_vib_offset;     /* signed, scaled vibrato pitch offset (was DE) */
  u16        A_shift_test;      /* shift-loop overflow accumulator (was A, tested via carry) */
  u8         A_status_new;      /* status with bit 0 toggled; stored back and re-tested in phase 5 (was A) */
  u8         B_slide_countdown; /* portamento reload countdown (was B, IX+$0E) */
  s8         C_slide_step;      /* signed per-tick portamento step (was C, IX+$0D) */
  u16        HL_slide_accum;    /* accumulated portamento/slide value (was HL, IX+$07/$08) */
  u8         A_mixer_test;      /* ~status & 3; nonzero except every 4th call (was A) */
  u8         A_shared;          /* shared driver byte, XORed and forwarded (was A, $EC79) */
  u8         A_mixer_val;       /* value merged into the shared mixer cache (was A) */
  u8         A_mute_flag;       /* one-shot mute-transition gate (was A, IX+$1F) */
  u8         A_noise_mask;      /* this channel's noise-enable bits, complemented for clearing (was A) */

  /* $EE9E-$EED0: phase 1, envelope amplitude advance. */
  /* $EE9E-$EEA1: LD C,(IX+$00)/BIT 5,C (19+8=27). */
  state->speccy->logtime(state->speccy, 27);
  C_status = IX_channel->status;
  if (C_status & CHSTATUS_ENVELOPE_ACTIVE)
  {
    /* $EEA3-$EEAA: JR Z not taken(7); LD A,(IX+$19)/SUB $01/LD (IX+$19),A
     * (19+7+19=45) -- the Z80 speculatively decrements and stores here
     * regardless of underflow; the reload branch below overwrites this store
     * at $EEB2. Total 7+45=52. */
    state->speccy->logtime(state->speccy, 52);
    A_env_step = IX_channel->envelope_step_counter;
    if (A_env_step == 0)
    {
      /* $EEAD: JR NC not taken (7). */
      state->speccy->logtime(state->speccy, 7);
      /* Underflow: reload the counter and try to advance the envelope shape
       * pointer. */
      A_env_step                        = IX_channel->envelope_speed;
      IX_channel->envelope_step_counter = A_env_step;

      /* $EEAF-$EEBD: LD A,(IX+$0F)/LD (IX+$19),A/LD L,(IX+$16)/
       * LD H,(IX+$17)/INC HL/LD A,(HL)/AND A (19+19+19+19+6+7+4=93). */
      state->speccy->logtime(state->speccy, 93);
      HL_env_shape = IX_channel->envelope_shape_ptr + 1;
      A_env_byte = *HL_env_shape;
      if ((s8) A_env_byte >= 0) { /* SEQ_END_BIT clear */
        /* $EEBE: JP M not taken (10); $EEC1-$EEC7: LD (IX+$16),L/
         * LD (IX+$17),H/LD (IX+$18),A (19+19+19=57). Total 67. */
        state->speccy->logtime(state->speccy, 67);
        /* Not an end-of-table marker: commit the advance. */
        IX_channel->envelope_shape_ptr = HL_env_shape;
        IX_channel->envelope_amplitude = A_env_byte;
      } else
      {
        /* $EEBE: JP M taken (10). */
        state->speccy->logtime(state->speccy, 10);
      }
      /* Else: halt the pointer, keep the previous amplitude. */
    } else
    {
      /* $EEAD: JR NC taken (12). */
      state->speccy->logtime(state->speccy, 12);
      IX_channel->envelope_step_counter = A_env_step - 1;
    }
    /* $EECA-$EECD: LD A,(IX+$18)/LD (IX+$13),A (19+19=38). */
    state->speccy->logtime(state->speccy, 38);
    IX_channel->volume = IX_channel->envelope_amplitude;
  } else
  {
    /* $EEA3: JR Z taken (12) -- envelope not active, phase 1 skipped
     * entirely. */
    state->speccy->logtime(state->speccy, 12);
  }

  /* $EED0-$EEFC: phase 2, tone-period lookup. */
  B_note = IX_channel->note_index; // Conv: LD A,$00; ADD A,(IX+$12) collapsed to a direct read

  /* $EED0-$EEDE: LD A,$00/ADD A,(IX+$12)/LD B,A (7+19+4=30) -- collapsed to a
   * direct read in C (see Conv note above); LD L,(IX+$0B)/LD H,(IX+$0C)/
   * LD A,(HL)/INC HL/AND A (19+19+7+6+4=55). Total 85. */
  state->speccy->logtime(state->speccy, 85);
  HL_offset_ptr = IX_channel->pitch_offset_cur;
  A_offset_byte = *HL_offset_ptr;
  HL_offset_ptr++;
  if ((s8) A_offset_byte < 0) { /* SEQ_END_BIT set */
    /* $EEDF: JP P not taken (10); $EEE2-$EEE8: LD L,(IX+$09)/LD H,(IX+$0A)/
     * AND $7F (19+19+7=45). Total 55. */
    state->speccy->logtime(state->speccy, 55);
    /* End-of-sequence marker: reset to the default/loop-start pointer and
     * strip the marker bit. */
    HL_offset_ptr = IX_channel->pitch_offset_default;
    A_offset_byte &= (u8) ~SEQ_END_BIT;
  } else
  {
    /* $EEDF: JP P taken (10). */
    state->speccy->logtime(state->speccy, 10);
  }
  IX_channel->pitch_offset_cur = HL_offset_ptr;

  /* $EEEA-$EEFB: LD (IX+$0B),L/LD (IX+$0C),H (19+19=38); ADD A,B/LD HL,$EFBC/
   * LD D,$00/ADD A,A/LD E,A/ADD HL,DE/LD E,(HL)/INC HL/LD D,(HL)
   * (4+10+7+4+4+11+7+6+7=60). Total 98. */
  state->speccy->logtime(state->speccy, 98);
  A_note_combined = (u8) (A_offset_byte + B_note);
  A_note_lookup   = (u8) (A_note_combined * 2); // ADD A,A: 8-bit doubling, wraps mod 256

  /* Conv: real hardware ($EEF1-$EEFB) indexes $EFBC+2*note with no bounds
   * check. Tune 1 channel 3's real pattern data ($F666, raw note byte $72)
   * genuinely produces an out-of-range index here, reading into the
   * unrelated indexed-pointer table at $F07C onward instead of
   * note_periods. Verified against bank3.bin: at that index ($F0A0) the
   * real memory is zero, so DE_period comes out as 0 on real hardware.
   * Reproduce that rather than reading past note_periods' bounds in C. */
  DE_period = ((A_note_lookup >> 1) < NELEMS(note_periods))
                  ? note_periods[A_note_lookup >> 1]
                  : 0;

  /* $EEFC-$EF4B: phase 3, vibrato. */
  /* $EEFC-$EEFF: LD L,(IX+$1D)/BIT 6,L (19+8=27). */
  state->speccy->logtime(state->speccy, 27);
  if (IX_channel->flags & CHFLAGS_VIBRATO_ENABLE)
  {
    /* $EF01: JR Z not taken (7); $EF03-$EF0B: LD H,A/LD B,(IX+$1A)/SLA B/
     * BIT 7,L/LD A,(IX+$1C) (4+19+8+8+19=58). Total 7+58=65. */
    state->speccy->logtime(state->speccy, 65);
    B_vib_range   = IX_channel->vibrato_depth;
    B_vib_range <<= 1;

    A_vib_phase = IX_channel->vibrato_phase;

    /* $EF0E-$EF12: JR Z / BIT 0,C / JR NZ gate-check cascade -- costs 12 when
     * the gate bit is clear (always update), 22 when the gate bit is set but
     * the toggle bit is clear (also update), or 27 when both are set (skip
     * update). This re-derives the condition purely to select the billing
     * constant; it does not alter behaviour. */
    if (!(IX_channel->flags & CHFLAGS_VIBRATO_UPDATE_GATE))
    {
      state->speccy->logtime(state->speccy, 12);
    } else if (!(C_status & CHSTATUS_TOGGLE))
    {
      state->speccy->logtime(state->speccy, 22);
    } else
    {
      state->speccy->logtime(state->speccy, 27);
    }
    if (!((IX_channel->flags & CHFLAGS_VIBRATO_UPDATE_GATE) && (C_status & CHSTATUS_TOGGLE)))
    {
      /* Conv: $EF09-$EF12 gate whether the phase updates this call at all
       * (runs every other call); not fully resolved against source, mirrored
       * literally from the skool. */
      if (!(IX_channel->flags & CHFLAGS_VIBRATO_ASCENDING))
      {
        /* $EF14-$EF16: BIT 5,L/JR NZ not taken (8+7=15). */
        state->speccy->logtime(state->speccy, 15);
        /* Descending. */
        if (A_vib_phase >= IX_channel->vibrato_increment)
        {
          /* $EF18-$EF1B: SUB (IX+$1B)/JR NC taken (19+12=31). */
          state->speccy->logtime(state->speccy, 31);
          A_vib_phase -= IX_channel->vibrato_increment;
        } else
        {
          /* $EF18-$EF22: SUB (IX+$1B)/JR NC not taken/SET 5,(IX+$1D)/SUB A/JR
           * (19+7+23+4+12=65). */
          state->speccy->logtime(state->speccy, 65);
          IX_channel->flags |= CHFLAGS_VIBRATO_ASCENDING; // flip to ascending
          A_vib_phase        = 0;
        }
      } else
      {
        /* $EF14-$EF16: BIT 5,L/JR NZ taken (8+12=20). */
        state->speccy->logtime(state->speccy, 20);
        /* Ascending. */
        A_vib_phase += IX_channel->vibrato_increment;
        if (A_vib_phase >= B_vib_range)
        {
          /* $EF24-$EF2A: ADD A,(IX+$1B)/CP B/JR C not taken/RES 5,(IX+$1D)/
           * LD A,B (19+4+7+23+4=57). */
          state->speccy->logtime(state->speccy, 57);
          IX_channel->flags &= ~CHFLAGS_VIBRATO_ASCENDING; // flip to descending
          A_vib_phase = B_vib_range;
        } else
        {
          /* $EF24-$EF28: ADD A,(IX+$1B)/CP B/JR C taken (19+4+12=35). */
          state->speccy->logtime(state->speccy, 35);
        }
      }
      /* $EF2F: LD (IX+$1C),A (19). */
      state->speccy->logtime(state->speccy, 19);
      IX_channel->vibrato_phase = A_vib_phase;
    }

    /* $EF32-$EF38: EX DE,HL(Conv folded)/SRL B/SUB B/LD E,A/LD A,D/LD D,$00
     * (4+8+4+4+4+7=31). */
    state->speccy->logtime(state->speccy, 31);
    // Conv: EX DE,HL ($EF32/$EF4A) folded -- add the scaled offset directly.
    DE_vib_offset = (s8) (A_vib_phase - (B_vib_range >> 1)); // centre phase; (s8)->(s16) sign-extends
    /* $EF3A: JR NC (12 taken/11 not taken) -- re-derives the sign of the
     * expression above purely to select the billing constant; no behaviour
     * change. */
    if (DE_vib_offset >= 0)
    {
      state->speccy->logtime(state->speccy, 12);
    } else
    {
      state->speccy->logtime(state->speccy, 11);
    }

    A_shift_test = (u16) A_note_lookup + 0xA0;
    /* $EF3D: ADD A,$A0 (7). */
    state->speccy->logtime(state->speccy, 7);
    if (A_shift_test <= 0xFF)
    {
      /* $EF3F: JR C not taken (7) -- loop entered. */
      state->speccy->logtime(state->speccy, 7);
      do
      {
        DE_vib_offset <<= 1;
        A_shift_test    = (A_shift_test & 0xFF) + 0x18;
        /* $EF41-$EF47: SLA E/RL D/ADD A,$18/JR NC (8+8+7=23, +12 taken/+7 not
         * taken depending on whether the loop continues). */
        if (A_shift_test <= 0xFF)
          state->speccy->logtime(state->speccy, 23 + 12);
        else
          state->speccy->logtime(state->speccy, 23 + 7);
      } while (A_shift_test <= 0xFF);
    } else
    {
      /* $EF3F: JR C taken (12) -- loop skipped entirely. */
      state->speccy->logtime(state->speccy, 12);
    }

    /* $EF49-$EF4A: ADD HL,DE/EX DE,HL (Conv folded) (11+4=15). */
    state->speccy->logtime(state->speccy, 15);
    DE_period += DE_vib_offset;
  } else
  {
    /* $EF01: JR Z taken (12) -- vibrato disabled, phase 3 skipped entirely. */
    state->speccy->logtime(state->speccy, 12);
  }

  /* $EF4B-$EF76: phase 4, portamento/slide. */
  /* $EF4B-$EF4E: LD A,C/XOR $01/LD (IX+$00),A (4+7+19=30). */
  state->speccy->logtime(state->speccy, 30);
  A_status_new       = C_status ^ CHSTATUS_TOGGLE;
  IX_channel->status = A_status_new;

  /* $EF51: BIT 2,C (8). */
  state->speccy->logtime(state->speccy, 8);
  if (C_status & CHSTATUS_SLIDE_ACTIVE)
  {
    /* $EF53: JR Z not taken (7); $EF55: LD B,(IX+$0E) (19). Total 26. */
    state->speccy->logtime(state->speccy, 26);
    B_slide_countdown = IX_channel->slide_countdown;
    B_slide_countdown--;
    if (B_slide_countdown != 0)
    {
      /* $EF58: DJNZ taken (13); $EF73: LD (IX+$0E),B (19). Total 32. */
      state->speccy->logtime(state->speccy, 32);
      IX_channel->slide_countdown = B_slide_countdown;
    } else
    {
      /* $EF58: DJNZ not taken (8); $EF5A-$EF5D: LD C,(IX+$0D)/BIT 7,C
       * (19+8=27). Total 35. */
      state->speccy->logtime(state->speccy, 35);
      C_slide_step = IX_channel->slide_step;

      /* $EF5F: JR Z (12 taken/11 not taken) -- re-derives the sign of
       * C_slide_step purely to select the billing constant; matches the
       * BIT 7,C test above. */
      if ((s8) C_slide_step >= 0)
      {
        state->speccy->logtime(state->speccy, 12);
      } else
      {
        state->speccy->logtime(state->speccy, 11);
      }

      HL_slide_accum = IX_channel->slide_accum + (s16) C_slide_step; // sign-extended add
      /* $EF62-$EF6C: LD L,(IX+$07)/LD H,(IX+$08)/ADD HL,BC/LD (IX+$07),L/
       * LD (IX+$08),H (19+19+11+19+19=87). */
      state->speccy->logtime(state->speccy, 87);
      IX_channel->slide_accum = HL_slide_accum;

      /* $EF6F-$EF71: ADD HL,DE/EX DE,HL(Conv folded)/JR $EF76 (11+4+12=27). */
      state->speccy->logtime(state->speccy, 27);
      DE_period += HL_slide_accum;
    }
  } else
  {
    /* $EF53: JR Z taken (12). */
    state->speccy->logtime(state->speccy, 12);
  }

  /* $EF76-$EFAE: phase 5, mixer/volume finalisation. */
  /* $EF76-$EF77: CPL/AND $03 (4+7=11). */
  state->speccy->logtime(state->speccy, 11);
  A_mixer_test = (u8) ~A_status_new & (CHSTATUS_TOGGLE | CHSTATUS_BIT1);

  /* $EF79: LD A,$00 (7) -- reads the self-modified pending_mixer_bits operand
   * (see State.h); this is the same load either branch below consumes. */
  state->speccy->logtime(state->speccy, 7);
  if (A_mixer_test == 0)
  {
    /* $EF7B: JR NZ not taken (7); $EF7D-$EF85: LD A,($EC79)/XOR $08/
     * LD ($ECC6),A/LD A,$07 (13+7+13+7=40). Total 47. */
    state->speccy->logtime(state->speccy, 47);
    /* Every 4th call. */
    A_shared = state->bank3->title_music.shared_note_value ^ 0x08; // $EC79
    state->bank3->title_music.driver_internal_flag = A_shared;     // $ECC6 (SM)
    A_mixer_val = CHMIXER_TONE_MASK;
  } else
  {
    /* $EF7B: JR NZ taken (12). */
    state->speccy->logtime(state->speccy, 12);
    /* Conv: $EF79 "LD A,$00" reads its own self-modified operand byte
     * ($EF7A, pending_mixer_bits), not a literal 0 -- see State.h. A literal
     * 0x00 here permanently forces every channel's noise-enable bit on,
     * producing constant harsh noise; the real driver patches this operand
     * via advance_channel_pattern's mixer-bit commands.
     */
    A_mixer_val = state->bank3->title_music.pending_mixer_bits; // $EF7A (SM)
  }

  /* Replace-bits-under-mask: merge this channel's tone-enable bits into the
   * shared mixer cache without disturbing the other channels' bits. */
  A_mixer_val                       ^= state->bank3->title_ay_regs.mixer;
  A_mixer_val                       &= IX_channel->mixer_mask;
  A_mixer_val                       ^= state->bank3->title_ay_regs.mixer;
  state->bank3->title_ay_regs.mixer  = A_mixer_val;

  /* $EF87-$EF93: LD HL,$EFB6/XOR (HL)/AND (IX+$24)/XOR (HL)/LD (HL),A
   * (10+7+19+7+7=50); LD A,(IX+$1F)/AND A (19+4=23). Total 73. */
  state->speccy->logtime(state->speccy, 73);
  A_mute_flag = IX_channel->mute_pending;
  if ((s8) A_mute_flag < 0)
  {
    /* One-shot mute transition. */
    A_mute_flag &= (u8) ~CHMUTE_GATE_BIT;
    IX_channel->mute_pending  = A_mute_flag;
    A_noise_mask = (u8) ~(IX_channel->mixer_mask & CHMIXER_NOISE_MASK);
    state->bank3->title_ay_regs.mixer &= A_noise_mask;

    /* $EF94-$EFA7: JP P not taken (10); AND $7F/LD (IX+$1F),A (7+19=26);
     * LD A,(IX+$24)/AND $38/XOR $FF (19+7+7=33); AND (HL)/LD (HL),A
     * (7+7=14); LD A,$41/LD ($ECC6),A (7+13=20). Total 103. */
    state->speccy->logtime(state->speccy, 103);
    state->bank3->title_music.driver_internal_flag = 0x41; // $ECC6 (SM)
  } else
  {
    /* $EF94: JP P taken (10). */
    state->speccy->logtime(state->speccy, 10);
  }

  *A_volume_out = IX_channel->volume;

  /* $EFAA-$EFAE: EX DE,HL (Conv folded)/LD A,(IX+$13) (4+19=23); RET --
   * pairs with the CALL overhead billed at the call site in
   * titlescr_ay_music (10). Total 33. */
  state->speccy->logtime(state->speccy, 33);
  return DE_period;
}

/**
 * Resolve a phrase-table or pattern-header address to a data pointer
 *
 * Converts a raw Z80 address, as found in the title-tune phrase-pointer
 * table or in a 2-byte pattern-data header, into a C pointer into the
 * transcribed title_tune0_data / title_tune1_data arrays.
 *
 * \param[in] addr Raw Z80 address, as stored little-endian in the table or
 *                 header. (was DE/HL)
 *
 * \return         Pointer into the matching transcribed array. (was DE/HL)
 *
 * Conv: not a Z80 routine of its own -- pattern_data_ptr/pattern_ptr/
 *       phrase_ptr are C pointers into title_tune0_data/title_tune1_data, not
 *       simulated Z80 memory, so an address read out of the pattern stream must
 *       be translated via range/offset arithmetic against those two arrays
 *       rather than dereferenced directly.
 */
static const u8 *resolve_phrase_addr(u16 addr)
{
  if (addr >= TITLE_TUNE0_DATA_ADDR &&
      addr < TITLE_TUNE0_DATA_ADDR + NELEMS(title_tune0_data))
    return &title_tune0_data[addr - TITLE_TUNE0_DATA_ADDR];
  if (addr >= TITLE_TUNE1_DATA_ADDR &&
      addr < TITLE_TUNE1_DATA_ADDR + NELEMS(title_tune1_data))
    return &title_tune1_data[addr - TITLE_TUNE1_DATA_ADDR];
  if (addr >= TITLE_TUNE23_DATA_ADDR &&
      addr < TITLE_TUNE23_DATA_ADDR + NELEMS(title_tune23_data))
    return &title_tune23_data[addr - TITLE_TUNE23_DATA_ADDR];
  assert(0); /* address outside all transcribed raw tune data */
  return NULL;
}

/**
 * $F1AE (bank 3): Walk a channel's phrase-pointer table for command 0x87
 *
 * Reached via advance_channel_pattern's computed dispatch for pattern
 * command byte 0x87 (see that function's Conv note on the dispatch table).
 * Unlike every other command, 0x87 does not fall through to read the next
 * pattern byte, or into acp_reset_row_counter -- it hands the read loop a
 * brand-new cursor, taken from this channel's own phrase-pointer table.
 *
 * Each channel's raw pattern-data block (pattern_data_ptr, +$03/$04) begins
 * with the 2-byte header word titlescr_start_ay dereferences to seed pattern_ptr,
 * immediately followed at offset 2 by the phrase-pointer table proper: a
 * sequence of little-endian words, each either a literal marker (0 or 1) or
 * a Z80 address. phrase_table_offset (+$05/$06) is this channel's current
 * byte offset into that table.
 *
 * phrase_repeat_count (+$21) gates whether a new table word is read at all:
 * while it is still counting down, this call just decrements it and resumes
 * reading from phrase_ptr (+$22/$23, left unchanged) -- the phrase most
 * recently activated by a repeating entry. Once it underflows, a new table
 * word is read and dispatched:
 *
 * - PHRASE_TABLE_RESET: table exhausted -- re-read this channel's own header
 *   word and restart the table at offset 2.
 * - PHRASE_TABLE_TRANSPOSE_PREFIX: the following byte is an inline transpose
 *   override for the next phrase; apply it to transpose (+$20) and re-read
 *   the next table word, 3 bytes further on.
 * - PHRASE_TABLE_REPEATING_ENTRY: a 1-byte repeat count and 2-byte pointer
 *   follow; store both (phrase_repeat_count, phrase_ptr) and use the
 *   pointer as the resume cursor.
 * - anything else: a plain phrase-pointer word -- use it directly as the
 *   resume cursor.
 *
 * Either way, the resulting cursor is written back through DE_pattern, ready
 * for advance_channel_pattern's byte-read loop to resume from (was "JP
 * $EDE4").
 *
 * \param[in,out] IX_channel Pointer to this channel's tracker record. (was IX)
 * \param[out]    DE_pattern Receives the new pattern-read cursor. (was DE)
 *
 * Conv: raw Z80 addresses read from the table (the header word, and any phrase
 *       pointer) are resolved to C pointers via resolve_phrase_addr rather than
 *       simulating a flat address space.
 *
 * Conv: the Z80 also clears B to 0 at every exit ($F1EA/$F1ED "LD B,$00"); this
 *       has no C equivalent since BC is not otherwise modelled here.
 */
static void advance_channel_phrase(chqstate_t           *state,
                                   title_tune_channel_t *IX_channel,
                                   const u8            **DE_pattern)
{
  u16       BC_table_offset; /* byte offset into this channel's phrase table (was BC, +$05/$06) */
  const u8 *HL_entry;        /* -> current phrase-table entry (was HL) */
  s8        A_repeat;        /* decremented repeat count, tested for underflow (was A, +$21) */
  u16       DE_word;         /* word read from the phrase table, or the resolved resume cursor (was DE) */
  u8        A_new_repeat;    /* freshly-read repeat count for a repeating entry (was A) */
  u16       DE_new_ptr;      /* freshly-read pointer for a repeating entry (was DE) */

  /* $F1AE-$F1B1: LD C,(IX+$05)/LD B,(IX+$06) (19+19=38). */
  state->speccy->logtime(state->speccy, 38);
  BC_table_offset = IX_channel->phrase_table_offset;

  for (;;)
  {
    /* $F1B4: HL -> this table position. */
    HL_entry = IX_channel->pattern_data_ptr + BC_table_offset;

    /* $F1B4-$F1BF: LD L,(IX+$03)/LD H,(IX+$04)/ADD HL,BC (19+19+11=49);
     * LD A,(IX+$21)/DEC A/JP M (19+4+10=33). Total 82. */
    state->speccy->logtime(state->speccy, 82);
    /* $F1BB-$F1BF: decrement the repeat count; underflow means this
     * position's repeats are exhausted and a new table word must be read. */
    A_repeat = (s8) (IX_channel->phrase_repeat_count - 1);
    if (A_repeat < 0)
      break; /* -> $F1D1, read a new table word */

    /* $F1C2-$F1CF: repeats remain -- store the decrement, then either
     * resume the already-active phrase (repeats still remain after this
     * one) or, on the exact call that brings the count to 0, fall through
     * to re-examine the next table slot instead. Conv: tests A_repeat, not
     * phrase_ptr -- $F1C2-$F1C8 (the two LD (IX+d),reg / LD reg,(IX+d)
     * stores) do not affect flags, so $F1CB's JR NZ still reads the Z flag
     * left over from $F1BE's DEC A, not phrase_ptr's contents. */
    IX_channel->phrase_repeat_count = (u8) A_repeat;

    /* $F1C2-$F1C8: LD (IX+$21),A (19); LD E,(IX+$22)/LD D,(IX+$23)
     * (19+19=38) -- loaded unconditionally by the Z80 regardless of the
     * JR NZ outcome below; the loaded value is only consumed when
     * A_repeat != 0. Total 57. */
    state->speccy->logtime(state->speccy, 57);
    if (A_repeat != 0)
    {
      /* $F1CB: JR NZ taken (12). */
      state->speccy->logtime(state->speccy, 12);
      *DE_pattern = IX_channel->phrase_ptr;
      goto finalize;
    }
    /* $F1CB-$F1CF: JR NZ not taken (7); INC BC/INC BC/JR $F1B4
     * (6+6+12=24). Total 31. */
    state->speccy->logtime(state->speccy, 31);

    BC_table_offset += 2;
  }

  /* $F1D1: LD (IX+$20),$00 (19). */
  state->speccy->logtime(state->speccy, 19);
  /* $F1D1: a new table word must be read -- reset this phrase's transpose
   * override to none. */
  IX_channel->transpose = 0;

  for (;;)
  {
    /* $F1D5-$F1D9: LD A,(HL)/INC HL/LD D,(HL)/LD E,A (word read, 7+6+7+4=24);
     * $F1D9: OR D (4). Total 28. */
    state->speccy->logtime(state->speccy, 28);
    /* $F1D5-$F1D9: read a little-endian word at HL_entry. */
    DE_word = wordat(HL_entry++);

    switch (DE_word)
    {
    case PHRASE_TABLE_RESET:
      /* $F1DA-$F1E7: JR NZ not taken (7); LD L,(IX+$03)/LD H,(IX+$04)/
       * LD BC,$0002/LD E,(HL)/INC HL/LD D,(HL) (19+19+10+7+6+7=68). Total
       * 7+68=75. */
      state->speccy->logtime(state->speccy, 75);
      /* $F1DC-$F1E7: table exhausted -- restart from this channel's own
       * header word, offset reset to the table's start (2). */
      HL_entry        = IX_channel->pattern_data_ptr;
      DE_word         = wordat(HL_entry);
      BC_table_offset = 2;
      *DE_pattern     = resolve_phrase_addr(DE_word);
      goto finalize;

    case PHRASE_TABLE_TRANSPOSE_PREFIX:
      /* $F1DA-$F202: JR NZ taken(from RESET test, 12); DEC DE/LD A,D/OR E/
       * JR NZ not taken (6+4+4+7=21); INC HL/LD A,(HL)/LD (IX+$20),A/INC HL/
       * INC BC x3/INC DE/JR $F1D5 (6+7+19+6+6+6+6+6+12=74). Total
       * 12+21+74=107. */
      state->speccy->logtime(state->speccy, 107);
      /* $F1F8-$F202: inline transpose override -- apply it and re-read the
       * next word, 3 bytes further into the table. */
      IX_channel->transpose = *++HL_entry;
      HL_entry++;
      BC_table_offset += 3;
      continue;

    case PHRASE_TABLE_REPEATING_ENTRY:
      /* $F1DA-$F21D: JR NZ taken(12); DEC DE/LD A,D/OR E/JR NZ taken
       * (6+4+4+12=26); DEC DE/LD A,D/OR E/INC DE/INC DE/JR NZ not taken
       * (6+4+4+6+6+7=33); INC HL/LD A,(HL)/LD (IX+$21),A/INC HL/LD E,(HL)/
       * LD (IX+$22),E/INC HL/LD D,(HL)/LD (IX+$23),D/INC BC x3/JR $F1E8
       * (6+7+19+6+7+19+6+7+19+6+6+6+12=126). Total 12+26+33+126=197. */
      state->speccy->logtime(state->speccy, 197);
      /* $F20B-$F21D: repeating entry -- repeat count then pointer follow.
       * Leaves BC_table_offset at the pointer's own low byte, so the next
       * lookup at this same table position re-reads it as a plain pointer
       * (see the "else" case below) once the repeats are exhausted. */
      A_new_repeat = *++HL_entry;
      HL_entry++;
      DE_new_ptr = wordat(HL_entry);

      IX_channel->phrase_repeat_count = A_new_repeat;
      IX_channel->phrase_ptr          = resolve_phrase_addr(DE_new_ptr);

      BC_table_offset += 3;
      *DE_pattern      = IX_channel->phrase_ptr;
      goto finalize;

    default:
      /* $F1DA-$F221: JR NZ taken(12); DEC DE/LD A,D/OR E/JR NZ taken
       * (6+4+4+12=26); DEC DE/LD A,D/OR E/INC DE/INC DE/JR NZ taken
       * (6+4+4+6+6+12=38); INC BC x2/JR $F1E8 (6+6+12=24). Total
       * 12+26+38+24=100. */
      state->speccy->logtime(state->speccy, 100);
      /* $F21F-$F221: plain phrase-pointer word -- use it directly. */
      BC_table_offset += 2;
      *DE_pattern      = resolve_phrase_addr(DE_word);
      goto finalize;
    }
  }

finalize:
  /* $F1E8-$F1EE: LD (IX+$05),C/LD (IX+$06),B/LD B,$00 (19+19+7=45). The
   * LD B,$00 has no C equivalent (BC is not otherwise modelled here). */
  state->speccy->logtime(state->speccy, 45);
  /* $F1E8-$F1EB: persist the table cursor for next time. */
  IX_channel->phrase_table_offset = BC_table_offset;
}

/**
 * $F3B6: Page in bank 3 and call a banked routine
 *
 * Patches a CALL instruction at $81C5 with [routine], backs up the 4 KB at
 * $B000 to $F000, sets up a temporary stack, pages in bank 3 via page_128k,
 * executes the patched CALL, then pages bank 3 back out, restores SP and
 * refills $B000 from $F000.
 *
 * In C, a switch on [routine] dispatches each Z80 entry-point address constant
 * to its C implementation. page_128k (relocated to $81D6) still brackets the
 * dispatch, page in before and page out after, matching the Z80 CALL pair at
 * $F3CE/$F3D5, even though it is a no-op in the C port.
 *
 * \param[in] routine Z80 address of the bank 3 routine to invoke. (was HL)
 *
 * \return            The value the Z80 leaves in A. Zero signals an early
 *                    return in the caller's loop -- attract_mode_128k's $F421
 *                    RET Z, i.e. start the game. Only BANK3_TITLE_SCREEN
 *                    returns zero, and only when a credit was inserted; every
 *                    other case returns 1.
 *
 * Conv: Z80 uses self-modification and 128K hardware memory paging; C
 *       dispatches via switch on the [routine] address constants.
 */
u8 bank3_call(chqstate_t *state, int routine)
{
  u8 A_result; /* value to return to caller (was A) */

  page_128k(state); /* $F3CE: page in */

  switch (routine)
  {
  default:
    assert(0);
    A_result = 1;
    break;
  case BANK3_TITLE_SCREEN:
    A_result = run_title_screen(state); /* 0 when a credit started a game */
    break;
  case BANK3_HI_SCORE:
    check_high_score(state);
    A_result = 1;
    break;
  case BANK3_SUCCESS_MUSIC:
    play_success_music(state);
    A_result = 1;
    break;
  case BANK3_INPUT_SELECTION:
    A_result = options_menu_driver(state); /* $C009 JP $FB99; always 1 */
    break;
  }

  page_128k(state); /* $F3D5: page out */

  return A_result;
}

/**
 * $F7AA: Configure the 128K Z80 mode-2 interrupt vector table
 *
 * Fills the 257-byte interrupt vector table at $BC00-$BDBD with $BD (so every
 * vector points to $BDBD), then writes a JP $F8AD at $BDBD and sets I=$BC and
 * IM 2. Under mode 2 all interrupts are routed through $F8AD
 * (frame_interrupt_handler).
 *
 * Conv: Z80 interrupt wiring has no equivalent in C; SDL delivers events on its
 *       own thread. This function is a no-op in the C port, matching the
 *       existing 48K setup_interrupts stub.
 */
static void setup_im2_interrupt_table(chqstate_t *state)
{
  /* Conv: no equivalent in C — SDL owns interrupt delivery. Referencing
   * frame_interrupt_handler's address mirrors the Z80's JP $F8AD vector
   * install this function performs, keeping the symbol used without adding
   * a fake per-frame call it never actually gets in the C port. */
  (void) &frame_interrupt_handler;
  NOT_USED(state);
}

/**
 * $F7C7: Set up interrupts and run the success jingle
 *
 * Calls setup_im2_interrupt_table, starts tune 1 (via titlescr_start_tune,
 * which also arms tune 1's drum-sample cue script), then falls into
 * basl_service_loop ($F7D1), which calls titlescr_music once per 50Hz
 * interrupt via HALT synchronisation. In the Z80 this loop is unconditional
 * (`CALL $F82F` / `JR $F7D1`) and never returns to its caller (was RET never
 * reached).
 *
 * Conv: reached from BANK3_SUCCESS_MUSIC (the perp-caught success jingle) via
 *       bank3_call, whose caller (handle_perp_caught_128k, and in turn its own
 *       caller's phase4 state machine) expects a normal return so scoring and
 *       fading can proceed on the same call — an infinite loop here would
 *       permanently hang the game thread. Per explicit scope decision, the
 *       Z80's unconditional loop is quantised into a bounded run of
 *       ATTRACT_TUNE_WAIT_FRAMES frames (the same 0xB4/180-frame, ~3.6s count
 *       the Z80 uses for the tune-4 wait in titlescr_wait_loop) and then
 *       returns normally. The frame count is a guess at the jingle's real
 *       duration; TODO: tune by ear once pattern data exists to actually hear
 *       it.
 */
static void play_success_music(chqstate_t *state)
{
  int B_wait; /* jingle frame countdown (was B, unbounded in the Z80) */

  setup_im2_interrupt_table(state);
  titlescr_start_tune(state, 1);

  B_wait = ATTRACT_TUNE_WAIT_FRAMES;
  do
  {
    /* Conv: ~3.6s of jingle; without this the window stays up that long after
     * the host asks to quit. */
    CHECK_HOST_QUIT(state);

    titlescr_music(state);
  } while (--B_wait);
}

/**
 * $F7D6: Start a tune and arm its drum-sample trigger table
 *
 * Plays tune [tune_no] (via titlescr_start_ay), then falls into
 * load_drum_script to look up and arm that tune's drum-sample cue
 * script.
 *
 * \param[in] tune_no Tune number to start (was A).
 */
static void titlescr_start_tune(chqstate_t *state, u8 tune_no)
{
  titlescr_start_ay(state, tune_no);
  load_drum_script(state, tune_no);
}

/**
 * Resolve a drum-sample cue-script/trigger-table address to a data pointer
 *
 * Converts a raw Z80 address, as found in the cue-script pointer table, a
 * cue script's own $FF jump target, or the offset arithmetic into the
 * $FAA4 trigger table, into a C pointer into the transcribed
 * drum_cue_script_data array.
 *
 * \param[in] addr Raw Z80 address in the $FA75-$FB98 range. (was HL)
 *
 * \return         Pointer into drum_cue_script_data. (was HL)
 *
 * Conv: not a Z80 routine of its own -- see resolve_phrase_addr's own Conv note
 *       for why raw addresses read out of transcribed data must be resolved
 *       this way rather than dereferenced directly.
 */
static const u8 *resolve_drum_script_addr(u16 addr)
{
  // clang-format off
  /* 128K bank 3: drum-sample cue-script/trigger-table data, $FA75-$FB98,
   * transcribed byte-exact from bank3.bin. Covers the per-tune cue-script
   * pointer table ($FA75-$FA7E), the 5 tunes' cue-script byte-code
   * ($FA7F-$FAA3), and the per-drum-ID trigger table ($FAA4-$FB98) -- see
   * stst_load_sfx_script/ssa_read_opcode prologues (Bank3.c) for the
   * byte-code this drives, and resolve_drum_script_addr for how raw Z80
   * addresses within it are resolved to C pointers. */
  static const u8 drum_cue_script_data[292] = {
    /* $FA75-$FA7E: per-tune cue-script pointer table, indexed by tune*2
     * (stst_load_sfx_script). Tunes actually started: 0, 1, 2, 3, 4.
     */
    TWOBYTES(0xFA7F), /* $FA75: tune 0 cue script */
    TWOBYTES(0xFA86), /* $FA77: tune 1 cue script */
    TWOBYTES(0xFA89), /* $FA79: tune 2 cue script */
    TWOBYTES(0xFA8E), /* $FA7B: tune 3 cue script */
    TWOBYTES(0xFA9D), /* $FA7D: tune 4 cue script */

    /* $FA7F-$FAA3: cue scripts (all 5 tunes' bytecode -- ssa_read_opcode).
     * Each step is a delay byte + a raw offset into the trigger table below
     * ($FAA4 + offset); $FF reads a 2-byte jump target; $FE ends the script.
     */
    0x03, /* $FA7F: delay=3 */
    0x00, /* $FA80: offset -> entry $FAA4 */
    0x01, /* $FA81: delay=1 */
    0x1A, /* $FA82: offset -> entry $FABE */
    0xFF, /* $FA83: FF jump */
    TWOBYTES(0xFA7F), /* $FA84: jump target */
    0x01, /* $FA86: delay=1 */
    0x33, /* $FA87: offset -> entry $FAD7 */
    0xFE, /* $FA88: FE end-of-script */
    0x05, /* $FA89: delay=5 */
    0x4F, /* $FA8A: offset -> entry $FAF3 */
    0x01, /* $FA8B: delay=1 */
    0x61, /* $FA8C: offset -> entry $FB05 */
    0xFE, /* $FA8D: FE end-of-script */
    0x04, /* $FA8E: delay=4 */
    0x73, /* $FA8F: offset -> entry $FB17 */
    0x04, /* $FA90: delay=4 */
    0x76, /* $FA91: offset -> entry $FB1A */
    0x03, /* $FA92: delay=3 */
    0x88, /* $FA93: offset -> entry $FB2C */
    0x01, /* $FA94: delay=1 */
    0x9A, /* $FA95: offset -> entry $FB3E */
    0x03, /* $FA96: delay=3 */
    0xAC, /* $FA97: offset -> entry $FB50 */
    0x01, /* $FA98: delay=1 */
    0xC3, /* $FA99: offset -> entry $FB67 */
    0xFF, /* $FA9A: FF jump */
    TWOBYTES(0xFA96), /* $FA9B: jump target */
    0x01, /* $FA9D: delay=1 */
    0xDA, /* $FA9E: offset -> entry $FB7E */
    0x01, /* $FA9F: delay=1 */
    0x73, /* $FAA0: offset -> entry $FB17 */
    0xFF, /* $FAA1: FF jump */
    TWOBYTES(0xFA9F), /* $FAA2: jump target */

    /* $FAA4-$FB98: per-drum-ID trigger table (ssa_read_opcode/
     * sfx_music_service). Each entry is a selector byte (copied to
     * slot1_countdown/slot1_selector_dup, purpose otherwise unestablished)
     * followed by a stream of per-frame dispatch bytes (bit 7 = also arm
     * slot 2, low 3 bits = engine: 1 = sample1, 2 = sample2, 3 = procedural
     * noise, 0 = nothing; upper 5 bits = pitch/rate param), terminated by a
     * literal 1 (sfx_dispatch_entry's reload sentinel).
     */
    0x08, /* $FAA4: entry selector */
    0x8B, /* $FAA5: slot2+noise pitch=17 */
    0x41, /* $FAA6: sample1 pitch=8 */
    0x1B, /* $FAA7: noise pitch=3 */
    0x42, /* $FAA8: sample2 pitch=8 */
    0x8B, /* $FAA9: slot2+noise pitch=17 */
    0x41, /* $FAAA: sample1 pitch=8 */
    0x1B, /* $FAAB: noise pitch=3 */
    0x8B, /* $FAAC: slot2+noise pitch=17 */
    0x41, /* $FAAD: sample1 pitch=8 */
    0x42, /* $FAAE: sample2 pitch=8 */
    0x8B, /* $FAAF: slot2+noise pitch=17 */
    0x41, /* $FAB0: sample1 pitch=8 */
    0x1B, /* $FAB1: noise pitch=3 */
    0x9A, /* $FAB2: slot2+sample2 pitch=19 */
    0x41, /* $FAB3: sample1 pitch=8 */
    0x42, /* $FAB4: sample2 pitch=8 */
    0x1B, /* $FAB5: noise pitch=3 */
    0x8B, /* $FAB6: slot2+noise pitch=17 */
    0x41, /* $FAB7: sample1 pitch=8 */
    0x9A, /* $FAB8: slot2+sample2 pitch=19 */
    0x41, /* $FAB9: sample1 pitch=8 */
    0x42, /* $FABA: sample2 pitch=8 */
    0x8B, /* $FABB: slot2+noise pitch=17 */
    0x41, /* $FABC: sample1 pitch=8 */
    0x01, /* $FABD: reload sentinel */
    0x08, /* $FABE: entry selector */
    0x8B, /* $FABF: slot2+noise pitch=17 */
    0x41, /* $FAC0: sample1 pitch=8 */
    0x1B, /* $FAC1: noise pitch=3 */
    0x42, /* $FAC2: sample2 pitch=8 */
    0x8B, /* $FAC3: slot2+noise pitch=17 */
    0x41, /* $FAC4: sample1 pitch=8 */
    0x1B, /* $FAC5: noise pitch=3 */
    0x8B, /* $FAC6: slot2+noise pitch=17 */
    0x41, /* $FAC7: sample1 pitch=8 */
    0x42, /* $FAC8: sample2 pitch=8 */
    0x8B, /* $FAC9: slot2+noise pitch=17 */
    0x41, /* $FACA: sample1 pitch=8 */
    0x1B, /* $FACB: noise pitch=3 */
    0x9A, /* $FACC: slot2+sample2 pitch=19 */
    0x41, /* $FACD: sample1 pitch=8 */
    0x42, /* $FACE: sample2 pitch=8 */
    0x1B, /* $FACF: noise pitch=3 */
    0x8B, /* $FAD0: slot2+noise pitch=17 */
    0x41, /* $FAD1: sample1 pitch=8 */
    0x9A, /* $FAD2: slot2+sample2 pitch=19 */
    0x42, /* $FAD3: sample2 pitch=8 */
    0x42, /* $FAD4: sample2 pitch=8 */
    0x42, /* $FAD5: sample2 pitch=8 */
    0x01, /* $FAD6: reload sentinel */
    0x08, /* $FAD7: entry selector */
    0x0A, /* $FAD8: sample2 pitch=1 */
    0x12, /* $FAD9: sample2 pitch=2 */
    0x0A, /* $FADA: sample2 pitch=1 */
    0x12, /* $FADB: sample2 pitch=2 */
    0x0A, /* $FADC: sample2 pitch=1 */
    0x12, /* $FADD: sample2 pitch=2 */
    0x0A, /* $FADE: sample2 pitch=1 */
    0x12, /* $FADF: sample2 pitch=2 */
    0x0A, /* $FAE0: sample2 pitch=1 */
    0x12, /* $FAE1: sample2 pitch=2 */
    0x0A, /* $FAE2: sample2 pitch=1 */
    0x12, /* $FAE3: sample2 pitch=2 */
    0x0A, /* $FAE4: sample2 pitch=1 */
    0x12, /* $FAE5: sample2 pitch=2 */
    0x0A, /* $FAE6: sample2 pitch=1 */
    0x12, /* $FAE7: sample2 pitch=2 */
    0x0A, /* $FAE8: sample2 pitch=1 */
    0x12, /* $FAE9: sample2 pitch=2 */
    0x0A, /* $FAEA: sample2 pitch=1 */
    0x12, /* $FAEB: sample2 pitch=2 */
    0x0A, /* $FAEC: sample2 pitch=1 */
    0x12, /* $FAED: sample2 pitch=2 */
    0x0A, /* $FAEE: sample2 pitch=1 */
    0x12, /* $FAEF: sample2 pitch=2 */
    0x42, /* $FAF0: sample2 pitch=8 */
    0x00, /* $FAF1: none pitch=0 */
    0x01, /* $FAF2: reload sentinel */
    0x04, /* $FAF3: entry selector */
    0x29, /* $FAF4: sample1 pitch=5 */
    0x00, /* $FAF5: none pitch=0 */
    0x1B, /* $FAF6: noise pitch=3 */
    0x1B, /* $FAF7: noise pitch=3 */
    0x29, /* $FAF8: sample1 pitch=5 */
    0x00, /* $FAF9: none pitch=0 */
    0x1B, /* $FAFA: noise pitch=3 */
    0x1B, /* $FAFB: noise pitch=3 */
    0x29, /* $FAFC: sample1 pitch=5 */
    0x00, /* $FAFD: none pitch=0 */
    0x1B, /* $FAFE: noise pitch=3 */
    0x1B, /* $FAFF: noise pitch=3 */
    0x29, /* $FB00: sample1 pitch=5 */
    0x00, /* $FB01: none pitch=0 */
    0x1B, /* $FB02: noise pitch=3 */
    0x1B, /* $FB03: noise pitch=3 */
    0x01, /* $FB04: reload sentinel */
    0x04, /* $FB05: entry selector */
    0x29, /* $FB06: sample1 pitch=5 */
    0x00, /* $FB07: none pitch=0 */
    0x1B, /* $FB08: noise pitch=3 */
    0x1B, /* $FB09: noise pitch=3 */
    0x29, /* $FB0A: sample1 pitch=5 */
    0x00, /* $FB0B: none pitch=0 */
    0x0A, /* $FB0C: sample2 pitch=1 */
    0x0A, /* $FB0D: sample2 pitch=1 */
    0x29, /* $FB0E: sample1 pitch=5 */
    0x00, /* $FB0F: none pitch=0 */
    0x12, /* $FB10: sample2 pitch=2 */
    0x12, /* $FB11: sample2 pitch=2 */
    0x29, /* $FB12: sample1 pitch=5 */
    0x00, /* $FB13: none pitch=0 */
    0x1A, /* $FB14: sample2 pitch=3 */
    0x1A, /* $FB15: sample2 pitch=3 */
    0x01, /* $FB16: reload sentinel */
    0x60, /* $FB17: entry selector */
    0x00, /* $FB18: none pitch=0 */
    0x01, /* $FB19: reload sentinel */
    0x06, /* $FB1A: entry selector */
    0x41, /* $FB1B: sample1 pitch=8 */
    0x1B, /* $FB1C: noise pitch=3 */
    0x1B, /* $FB1D: noise pitch=3 */
    0x1B, /* $FB1E: noise pitch=3 */
    0x00, /* $FB1F: none pitch=0 */
    0x1B, /* $FB20: noise pitch=3 */
    0x1B, /* $FB21: noise pitch=3 */
    0x1B, /* $FB22: noise pitch=3 */
    0x41, /* $FB23: sample1 pitch=8 */
    0x1B, /* $FB24: noise pitch=3 */
    0x41, /* $FB25: sample1 pitch=8 */
    0x1B, /* $FB26: noise pitch=3 */
    0x00, /* $FB27: none pitch=0 */
    0x41, /* $FB28: sample1 pitch=8 */
    0x1B, /* $FB29: noise pitch=3 */
    0x1B, /* $FB2A: noise pitch=3 */
    0x01, /* $FB2B: reload sentinel */
    0x06, /* $FB2C: entry selector */
    0x41, /* $FB2D: sample1 pitch=8 */
    0x1B, /* $FB2E: noise pitch=3 */
    0x1B, /* $FB2F: noise pitch=3 */
    0x1B, /* $FB30: noise pitch=3 */
    0x42, /* $FB31: sample2 pitch=8 */
    0x42, /* $FB32: sample2 pitch=8 */
    0x1B, /* $FB33: noise pitch=3 */
    0x1B, /* $FB34: noise pitch=3 */
    0x41, /* $FB35: sample1 pitch=8 */
    0x1B, /* $FB36: noise pitch=3 */
    0x41, /* $FB37: sample1 pitch=8 */
    0x1B, /* $FB38: noise pitch=3 */
    0x42, /* $FB39: sample2 pitch=8 */
    0x41, /* $FB3A: sample1 pitch=8 */
    0x1B, /* $FB3B: noise pitch=3 */
    0x42, /* $FB3C: sample2 pitch=8 */
    0x01, /* $FB3D: reload sentinel */
    0x06, /* $FB3E: entry selector */
    0x41, /* $FB3F: sample1 pitch=8 */
    0x1B, /* $FB40: noise pitch=3 */
    0x1B, /* $FB41: noise pitch=3 */
    0x1B, /* $FB42: noise pitch=3 */
    0x42, /* $FB43: sample2 pitch=8 */
    0x42, /* $FB44: sample2 pitch=8 */
    0x1B, /* $FB45: noise pitch=3 */
    0x1B, /* $FB46: noise pitch=3 */
    0x41, /* $FB47: sample1 pitch=8 */
    0x1B, /* $FB48: noise pitch=3 */
    0x1B, /* $FB49: noise pitch=3 */
    0x42, /* $FB4A: sample2 pitch=8 */
    0x42, /* $FB4B: sample2 pitch=8 */
    0x1B, /* $FB4C: noise pitch=3 */
    0x42, /* $FB4D: sample2 pitch=8 */
    0x42, /* $FB4E: sample2 pitch=8 */
    0x01, /* $FB4F: reload sentinel */
    0x06, /* $FB50: entry selector */
    0x41, /* $FB51: sample1 pitch=8 */
    0x1B, /* $FB52: noise pitch=3 */
    0x92, /* $FB53: slot2+sample2 pitch=18 */
    0x1B, /* $FB54: noise pitch=3 */
    0x1B, /* $FB55: noise pitch=3 */
    0x42, /* $FB56: sample2 pitch=8 */
    0x42, /* $FB57: sample2 pitch=8 */
    0x9A, /* $FB58: slot2+sample2 pitch=19 */
    0x1B, /* $FB59: noise pitch=3 */
    0x92, /* $FB5A: slot2+sample2 pitch=18 */
    0x1B, /* $FB5B: noise pitch=3 */
    0x41, /* $FB5C: sample1 pitch=8 */
    0x92, /* $FB5D: slot2+sample2 pitch=18 */
    0x1B, /* $FB5E: noise pitch=3 */
    0x41, /* $FB5F: sample1 pitch=8 */
    0x9A, /* $FB60: slot2+sample2 pitch=19 */
    0x1B, /* $FB61: noise pitch=3 */
    0x42, /* $FB62: sample2 pitch=8 */
    0x41, /* $FB63: sample1 pitch=8 */
    0x1B, /* $FB64: noise pitch=3 */
    0x42, /* $FB65: sample2 pitch=8 */
    0x01, /* $FB66: reload sentinel */
    0x06, /* $FB67: entry selector */
    0x41, /* $FB68: sample1 pitch=8 */
    0x1B, /* $FB69: noise pitch=3 */
    0x92, /* $FB6A: slot2+sample2 pitch=18 */
    0x1B, /* $FB6B: noise pitch=3 */
    0x1B, /* $FB6C: noise pitch=3 */
    0x42, /* $FB6D: sample2 pitch=8 */
    0x42, /* $FB6E: sample2 pitch=8 */
    0x9A, /* $FB6F: slot2+sample2 pitch=19 */
    0x1B, /* $FB70: noise pitch=3 */
    0x92, /* $FB71: slot2+sample2 pitch=18 */
    0x1B, /* $FB72: noise pitch=3 */
    0x41, /* $FB73: sample1 pitch=8 */
    0x92, /* $FB74: slot2+sample2 pitch=18 */
    0x1B, /* $FB75: noise pitch=3 */
    0x41, /* $FB76: sample1 pitch=8 */
    0x9A, /* $FB77: slot2+sample2 pitch=19 */
    0x42, /* $FB78: sample2 pitch=8 */
    0x42, /* $FB79: sample2 pitch=8 */
    0x41, /* $FB7A: sample1 pitch=8 */
    0x42, /* $FB7B: sample2 pitch=8 */
    0x42, /* $FB7C: sample2 pitch=8 */
    0x01, /* $FB7D: reload sentinel */
    0x08, /* $FB7E: entry selector */
    0x9B, /* $FB7F: slot2+noise pitch=19 */
    0x41, /* $FB80: sample1 pitch=8 */
    0x1B, /* $FB81: noise pitch=3 */
    0x9B, /* $FB82: slot2+noise pitch=19 */
    0x42, /* $FB83: sample2 pitch=8 */
    0x9B, /* $FB84: slot2+noise pitch=19 */
    0x41, /* $FB85: sample1 pitch=8 */
    0x1B, /* $FB86: noise pitch=3 */
    0x9B, /* $FB87: slot2+noise pitch=19 */
    0x41, /* $FB88: sample1 pitch=8 */
    0x42, /* $FB89: sample2 pitch=8 */
    0x42, /* $FB8A: sample2 pitch=8 */
    0x1B, /* $FB8B: noise pitch=3 */
    0x1B, /* $FB8C: noise pitch=3 */
    0x1A, /* $FB8D: sample2 pitch=3 */
    0x1B, /* $FB8E: noise pitch=3 */
    0x1B, /* $FB8F: noise pitch=3 */
    0x43, /* $FB90: noise pitch=8 */
    0x3B, /* $FB91: noise pitch=7 */
    0x33, /* $FB92: noise pitch=6 */
    0x2B, /* $FB93: noise pitch=5 */
    0x23, /* $FB94: noise pitch=4 */
    0x1B, /* $FB95: noise pitch=3 */
    0x13, /* $FB96: noise pitch=2 */
    0x0B, /* $FB97: noise pitch=1 */
    0x01, /* $FB98: reload sentinel */
  };
  // clang-format on

  assert(addr >= DRUM_CUE_SCRIPT_DATA_BASE &&
         addr < DRUM_CUE_SCRIPT_DATA_BASE + NELEMS(drum_cue_script_data));
  return &drum_cue_script_data[addr - DRUM_CUE_SCRIPT_DATA_BASE];
}

/**
 * $F7DB: Look up and arm a tune's drum-sample cue script
 *
 * Looks up a pointer in the table at $FA75 (indexed by A_tune*2) to a
 * per-tune drum-sample cue script, clears the 3 drum-sample "busy" flags at
 * $F837/$F895/$F8A2, then falls into load_drum_op to read that script's
 * first entry.
 *
 * \param[in] A_tune Tune number whose cue script to arm (was A).
 *
 * Conv: this entry point is also called directly (bypassing
 *       titlescr_start_tune/titlescr_start_ay) by titlescr_wait_loop's tune-4
 *       cue-table setup, matching the Z80's own $C629 CALL $F7DB.
 */
static void load_drum_script(chqstate_t *state, u8 A_tune)
{
  u16 HL_table; /* $FA75 + tune*2 -- this tune's table entry (was HL) */

  HL_table = 0xFA75 + (u16) (A_tune << 1);

  state->bank3->drums.sample_active = 0;
  state->bank3->drums.slot2_busy    = 0;
  state->bank3->drums.slot1_busy    = 0;

  load_drum_op(state, resolve_drum_script_addr(wordat(resolve_drum_script_addr(HL_table)))); /* was FALLTHROUGH */
}

/**
 * $F7F4: Drum-sample cue script re-entry, throttled by script_delay
 *
 * Decrements script_delay and returns early until it reaches 0, then
 * re-enters the script-byte-code reader (load_drum_op) at script_ptr to
 * read the next entry.
 *
 * Called by titlescr_music ($F82F) when the selector stream yields a
 * byte of exactly 1.
 */
static void titlescr_drum_advance(chqstate_t *state)
{
  u8 A_delay; /* script_delay, decremented (was A) */

  A_delay = (u8) (state->bank3->drums.script_delay - 1);
  state->bank3->drums.script_delay = A_delay;
  if (A_delay != 0)
    return;

  load_drum_op(state, state->bank3->drums.script_ptr);
}

/**
 * $F7FE: Drum-sample cue script byte-code reader
 *
 * Reads opcode bytes starting at HL. $FE ends the script (stops the tune
 * via stop_music_and_silence); $FF reads a 2-byte absolute jump target and
 * continues reading from there; any other byte is a delay/repeat value
 * stored to script_delay, followed by a raw byte offset into the $FAA4
 * trigger table -- an entry there is a selector byte (copied to both
 * slot1_selector_dup and slot1_countdown) followed by a stream of per-frame
 * dispatch bytes, whose address (the byte immediately after the selector)
 * is stored to stream_reload_ptr for titlescr_music to read.
 *
 * \param[in] HL Cue-script cursor to start reading from (was HL).
 *
 * Conv: $F823's own two-byte read is inlined into the $FF case below rather
 *       than given its own function, since it does nothing but load a new HL
 *       and loop back to the top of this same reader (`JR $F7FE`).
 *
 * Conv: $F829's `POP HL / POP HL / DI` is omitted -- those unwind Z80
 *       call-stack frames left by the CALL chain that reached this reader
 *       (titlescr_music -> titlescr_drum_advance -> here, or load_drum_script
 *       -> here); this function is an ordinary C call/return, not entered via
 *       pushed return addresses that need discarding, and DI has no host
 *       equivalent (see setup_im2_interrupt_table's own Conv note).
 */
static void load_drum_op(chqstate_t *state, const u8 *HL)
{
  u8        A;        /* opcode byte read from the script (was A) */
  u8        C_offset; /* raw byte offset into the $FAA4 trigger table (was C) */
  const u8 *entry;    /* trigger-table entry: selector + dispatch stream (was HL) */

  for (;;)
  {
    A = *HL;
    HL++;

    /* $F7FE LD A,(HL) / $F7FF INC HL / $F800 CP $FE / $F802 JP Z,$F829
     * (7+6+7+10=30). */
    state->speccy->logtime(state->speccy, 30);

    if (A == 0xFE)
    {
      /* $F829-$F82B POP HL / POP HL / DI have no C equivalent (see prologue
       * Conv note); $F82C JP $ED0B's own cost is billed inside
       * stop_music_and_silence. */
      stop_music_and_silence(state);
      return;
    }

    /* $F805 CP $FF (7). */
    state->speccy->logtime(state->speccy, 7);

    if (A == 0xFF)
    {
      /* $F807 JR Z,$F823 taken (12) + $F823 LD A,(HL) / $F824 INC HL /
       * $F825 LD H,(HL) / $F826 LD L,A / $F827 JR $F7FE (7+6+7+4+12=36). */
      state->speccy->logtime(state->speccy, 12 + 36);
      HL = resolve_drum_script_addr(wordat(HL));
      continue;
    }

    /* $F807 JR Z,$F823 not taken (7) + $F809 LD ($F7F5),A / $F80C LD C,(HL) /
     * $F80D INC HL / $F80E LD ($F7FC),HL / $F811 LD B,$00 / $F813 LD
     * HL,$FAA4 / $F816 ADD HL,BC / $F817 LD A,(HL) / $F818 INC HL / $F819 LD
     * ($F84E),A / $F81C LD ($F842),A / $F81F LD ($F85E),HL / $F822 RET
     * (13+7+6+16+7+10+11+7+6+13+13+16+10=135). */
    state->speccy->logtime(state->speccy, 7 + 135);

    state->bank3->drums.script_delay = A;

    C_offset                       = *HL++;
    state->bank3->drums.script_ptr = HL;

    entry = resolve_drum_script_addr(0xFAA4 + C_offset);

    state->bank3->drums.slot1_selector_dup = *entry;
    state->bank3->drums.slot1_countdown    = *entry;
    state->bank3->drums.stream_reload_ptr  = entry + 1;
    return;
  }
}

/**
 * $F82F: Per-frame drum-sample and music service
 *
 * Runs the AY title-tune driver (CALL $EC71 -> titlescr_ay_music), then drives
 * three digitised drum-sample "busy slot" state machines: slot 1
 * ($F836-$F894, a selector-byte stream read via a countdown at
 * slot1_countdown), slot 2 ($F894-$F8A1, a companion countdown armed by
 * slot 1's bit 7), and a 1-bit sample playback tail ($F8A1-$F8AC).
 *
 * Slot 1: if idle, arms itself and reloads the stream cursor from
 * stream_reload_ptr; if already busy, ticks slot1_countdown and only
 * re-enters the stream-reading loop once it reaches 0. The stream-reading
 * loop reads a byte from stream_ptr: exactly 1 means "pull in a fresh
 * trigger-table entry" (titlescr_drum_advance), reload the stream cursor and
 * retry; anything else is the entry byte to act on this frame. Bit 7 of that
 * entry byte also arms slot 2; its low 3 bits select one of the two fixed
 * 1-bit drum samples or the procedural noise generator (dispatch is a tail
 * call — this function returns immediately once one of those fires, exactly
 * as the Z80's JP does); its upper bits are the pitch/rate parameter passed
 * to whichever fixed-sample player is selected. A low-3-bits value of 0
 * triggers nothing and falls through to slot 2.
 *
 * Slot 2: if busy, ticks both slot1_countdown and slot2_busy down together
 * (companion countdown for whatever slot 1 armed via the bit-7 path).
 *
 * Tail: resumes mid-sample playback via play_sample_row when sample_active is
 * still 1 here -- a sample long enough to have yielded mid-playback on a
 * previous call (see play_sample_row's own Conv note) -- continuing from
 * sample_resume_ptr/sample_resume_rows rather than completing in one go.
 *
 * Called once per frame from $C06E, $C16A, $C59E, $F7C7 and $FBC8.
 *
 * Conv: the frame-flag clear at $F832-$F833 is omitted because nothing in this
 *       port polls the literal $F8A8 field (wait_for_frame_flag has no C
 *       equivalent — every caller here already represents one already-paced
 *       tick, so there is nothing left to wait for). play_sample_row's own
 *       mid-sample yield check uses a local per-call T-state budget instead of
 *       $F8A8, which needs no explicit clear -- see its Conv note.
 *
 * Conv: this function uses goto/labels rather than nested structured loops. The
 *       four labels are genuine Z80 jump targets, each reached from more than
 *       one site: sfx1_reload_pointer ($F85D) from the idle-arm path ($F83F)
 *       and from the stream loop's own back-edge; drum_read_stream_byte ($F855)
 *       by fallthrough from $F85D and from $F863; drum_dispatch_entry ($F866)
 *       from $F857; and sfx2_tick_countdown ($F894) from three places -- the
 *       countdown skip ($F84A), the nothing-to-trigger exit ($F87E) and
 *       fallthrough. A structured rewrite would have to duplicate the
 *       tick-countdown tail at each of those three exits, or introduce flag
 *       variables the Z80 does not have.
 */
static void titlescr_music(chqstate_t *state)
{
  u8        A;             /* general accumulator, reused for each state check (was A) */
  const u8 *HL_stream;     /* selector-stream cursor (was HL) */
  u8        D_entry;       /* dispatch entry byte, masked if bit 7 was set (was D) */
  int       B_instrument;  /* low-3-bits: which 1-bit-sample engine to trigger (was B) */
  int       A_pitch_param; /* pitch/rate parameter passed to the fixed-sample players (was A) */

  /* Conv: one call is one frame, so the frame's stamp/sleep pacing lives here
   * rather than being repeated around every call site. Callers that pace a
   * wider block still stamp/sleep themselves; the outer sleep then finds the
   * frame's time already spent here and returns without waiting again. */
  state->speccy->stamp(state->speccy);

  /* $F82F: CALL $EC71 -- call overhead into titlescr_ay_music. The RET this
   * pairs with is billed at that function's own exit. */
  state->speccy->logtime(state->speccy, 17);

  titlescr_ay_music(state);

  /* $F832-$F833: XOR A / LD ($F8A8),A -- clears the "frame occurred" flag.
   * Conv: functionally omitted (see prologue), since nothing here polls
   * $F8A8, but the two instructions still cost real T-states on hardware. */
  /* $F836-$F839: LD A,(slot1_busy) / AND A -- common prefix before the
   * busy/idle branch. */
  state->speccy->logtime(state->speccy, 4 + 13 + 7 + 4);

  if (!state->bank3->drums.slot1_busy)
  {
    /* $F839 JR NZ,$F841 not taken (7) + $F83B INC A / $F83C LD
     * (slot1_busy),A / $F83F JR $F85D (4+13+12=29) -- idle -> arm slot 1,
     * reload and enter the loop. */
    state->speccy->logtime(state->speccy, 7 + 29);
    state->bank3->drums.slot1_busy = 1;
    goto sfx1_reload_pointer;
  }

  /* $F839 JR NZ,$F841 taken (12) + $F841 LD A,(slot1_countdown) / $F843 DEC
   * A / $F844 JP Z,$F84D (7+4+10=21). */
  state->speccy->logtime(state->speccy, 12 + 21);

  A = (u8)(state->bank3->drums.slot1_countdown - 1);
  if (A != 0)
  {
    /* $F847 LD (slot1_countdown),A / $F84A JP $F894 -- skip slot 1 entirely
     * this frame. */
    state->speccy->logtime(state->speccy, 13 + 10);
    state->bank3->drums.slot1_countdown = A;
    goto sfx2_tick_countdown; // skip slot 1 entirely this frame
  }

  /* $F84D LD A,(SM,$00) / $F84F LD (slot1_countdown),A / $F852 LD
   * HL,(SM,stream_ptr) (7+13+10=30) -- reached the countdown's last tick;
   * reset it and fall into the stream-reading loop. */
  state->speccy->logtime(state->speccy, 30);

  /* Conv: $F84D is self-modifying -- its "LD A,$00" operand ($F84E) is
   * patched by load_drum_op to the current trigger-table entry's selector
   * byte (slot1_selector_dup), so this reload is NOT a literal 0. Using a
   * literal here starves slot1_countdown on every non-bit7 dispatch byte
   * (which never re-arms it), making it wrap to 0xFF on the next tick and
   * stall for ~255 frames instead of the ~selector-byte-frame gap the real
   * trigger-table pacing intends.
   */
  state->bank3->drums.slot1_countdown = state->bank3->drums.slot1_selector_dup;
  HL_stream                           = state->bank3->drums.stream_ptr;
  goto drum_read_stream_byte;

sfx1_reload_pointer:
  /* $F85D LD HL,(SM,stream_reload_ptr) / $F860 LD (stream_ptr),HL / $F863 JP
   * $F855 (10+16+10=36). Reached both from the idle-arm path above and from
   * the stream-retry loop below, so this bill fires once per visit exactly
   * as the real instructions would re-execute each time. */
  state->speccy->logtime(state->speccy, 36);
  HL_stream                      = state->bank3->drums.stream_reload_ptr;
  state->bank3->drums.stream_ptr = HL_stream;

drum_read_stream_byte:
  /* $F855 LD A,(HL) / $F856 DEC A / $F857 JP NZ,$F866 (7+4+10=21). */
  state->speccy->logtime(state->speccy, 21);
  A = *HL_stream - 1;
  if (A != 0)
    goto drum_dispatch_entry;
  /* $F85A CALL $F7F4 -- pull in a fresh trigger-table entry. */
  state->speccy->logtime(state->speccy, 17);
  titlescr_drum_advance(state); // pull in a fresh trigger-table entry
  goto sfx1_reload_pointer;

drum_dispatch_entry:
  /* $F866 INC HL / $F867 LD (stream_ptr),HL / $F86A INC A / $F86B BIT 7,A
   * (6+16+4+8=34). */
  state->speccy->logtime(state->speccy, 34);
  state->bank3->drums.stream_ptr = ++HL_stream;
  A++; // restore the original entry byte (undo the -1 above)
  if (A & 0x80)
  {
    /* $F86D JR Z,$F87B not taken (7) + $F86F AND $7F / $F871 EX AF,AF' /
     * $F872 LD A,$01 / $F874 LD (slot1_countdown),A / $F877 LD
     * (slot2_busy),A / $F87A EX AF,AF' (7+48=55) -- also arm slot 2. */
    state->speccy->logtime(state->speccy, 7 + 48);
    A &= ~0x80;
    state->bank3->drums.slot1_countdown = 1;
    state->bank3->drums.slot2_busy      = 1;
  } else
  {
    /* $F86D JR Z,$F87B taken (12). */
    state->speccy->logtime(state->speccy, 12);
  }

  /* $F87B LD D,A / $F87C AND $07 (4+7=11). */
  state->speccy->logtime(state->speccy, 11);
  D_entry  = A;
  A       &= 0x07;
  if (A == 0)
  {
    /* $F87E JR Z,$F894 taken (12) -- nothing to trigger this frame. */
    state->speccy->logtime(state->speccy, 12);
    goto sfx2_tick_countdown; // nothing to trigger this frame
  }

  /* $F87E JR Z,$F894 not taken (7) + $F880 LD B,A / $F881 LD A,D / $F882-
   * $F886 SRL A x3 / $F888 DEC B / $F889 JP Z,$F8B6 (4+4+24+4+10=46) --
   * common prefix for the selector-1 check; selectors 2 and 3 bill their
   * extra DEC B/JP Z pair below. */
  state->speccy->logtime(state->speccy, 7 + 46);

  B_instrument  = A;
  A_pitch_param = D_entry >> 3;
  switch (B_instrument)
  {
  case 1:
    play_fixed_sample_1(state, A_pitch_param);
    goto tm_exit;
  case 2:
    /* $F88C DEC B / $F88D JP Z,$F8BD (4+10=14). */
    state->speccy->logtime(state->speccy, 14);
    play_fixed_sample_2(state, A_pitch_param);
    goto tm_exit;
  case 3:
    /* $F88C-$F88D and $F890-$F891 DEC B/JP Z pairs, both missed then hit
     * (14+14=28). */
    state->speccy->logtime(state->speccy, 28);
    play_drum_noise_burst(state, A_pitch_param);
    goto tm_exit;
  }

sfx2_tick_countdown:
  /* $F894 LD A,(slot2_busy) / $F896 AND A (7+4=11). */
  state->speccy->logtime(state->speccy, 11);
  if (state->bank3->drums.slot2_busy)
  {
    /* $F897 JR Z,$F8A1 not taken (7) + $F899 LD HL,$F842 / $F89C DEC (HL) /
     * $F89D LD HL,$F895 / $F8A0 DEC (HL) (10+11+10+11=42). */
    state->speccy->logtime(state->speccy, 7 + 42);
    state->bank3->drums.slot1_countdown--;
    state->bank3->drums.slot2_busy--;
  } else
  {
    /* $F897 JR Z,$F8A1 taken (12). */
    state->speccy->logtime(state->speccy, 12);
  }

  /* $F8A1 LD A,(sample_active) / $F8A3 DEC A / $F8A4 JP Z,$F8CC
   * (7+4+10=21). */
  state->speccy->logtime(state->speccy, 21);
  if (state->bank3->drums.sample_active)
  {
    /* $F8CC EXX -- bank into the shadow HL'/D' that play_sample_row resumes
     * from. */
    state->speccy->logtime(state->speccy, 4);
    play_sample_row(state,
                    state->bank3->drums.sample_resume_rows,
                    state->bank3->drums.sample_resume_ptr);
  }

tm_exit:
  state->speccy->sleep(state->speccy, TITLE_MUSIC_TSTATES);
}

/**
 * $F8AD: Frame interrupt handler
 *
 * Z80 IM2 interrupt service routine installed by setup_im2_interrupt_table.
 * Sets the "frame occurred" flag at $F8A8, polled by wait_for_frame_flag, and
 * returns. The actual per-frame music/drum-sample work happens
 * synchronously from the title-screen main loop (titlescr_wait_loop /
 * play_success_music), not here.
 *
 * Conv: no equivalent in C — nothing in this port ever waits on the $F8A8 flag
 *       (the title-screen loops call titlescr_music directly once per paced
 *       iteration instead), so there is no flag to set. Adding one would be
 *       dead state, the same way the 48K irq_flag field was.
 */
static void frame_interrupt_handler(chqstate_t *state)
{
  /* Conv: no equivalent in C — see prologue */
  NOT_USED(state);
}

/**
 * $F8B6: Select fixed sample table 1 and start playback
 *
 * Drum-sample dispatch selector 1's entry point (see titlescr_music's
 * prologue).
 * Points play_fixed_sample_start at the 104-byte sample table
 * state->bank3->drums.sample1 and falls through to arm playback.
 *
 * \param[in] A_pitch_param Playback-rate/pitch parameter: the dispatch byte's
 *                          upper 5 bits (was A).
 */
static void play_fixed_sample_1(chqstate_t *state, int A_pitch_param)
{
  /* $F8B6 LD HL,$F8F2 / $F8B9 LD D,$68 / $F8BB JR $F8C2 (10+7+12=29) --
   * table-pointer load for the 104-byte sample table. */
  state->speccy->logtime(state->speccy, 29);
  play_fixed_sample_start(state,
                          A_pitch_param,
                          &state->bank3->drums.sample1[0],
                          sizeof(state->bank3->drums.sample1));
}

/**
 * $F8BD: Select fixed sample table 2 and start playback
 *
 * Drum-sample dispatch selector 2's entry point (see titlescr_music's
 * prologue).
 * Points play_fixed_sample_start at the 224-byte sample table
 * state->bank3->drums.sample2 and falls through to arm playback.
 *
 * \param[in] A_pitch_param Playback-rate/pitch parameter: the dispatch byte's
 *                          upper 5 bits (was A).
 */
static void play_fixed_sample_2(chqstate_t *state, int A_pitch_param)
{
  /* $F8BD LD HL,$F95A / $F8C0 LD D,$E0 (10+7=17) -- table-pointer load for
   * the 224-byte sample table; falls straight through to $F8C2, no JR here. */
  state->speccy->logtime(state->speccy, 17);
  play_fixed_sample_start(state,
                          A_pitch_param,
                          &state->bank3->drums.sample2[0],
                          sizeof(state->bank3->drums.sample2));
}

/**
 * $F8C2: Start 1-bit sample playback
 *
 * Shared tail for play_fixed_sample_1/play_fixed_sample_2: stashes the
 * pitch/rate parameter, marks sample playback active, then falls into
 * play_sample_row to begin pulsing the sample out over the beeper.
 *
 * \param[in] A_pitch_param Playback-rate/pitch parameter (was A).
 * \param[in] HL_data       Pointer to the first byte of the sample table to
 *                          play (was HL).
 * \param[in] D_length      Number of sample bytes to play (was D).
 *
 * Conv: $F8CE (sample_pitch_param) is the self-modified operand of the "LD
 *       B,$08" at $F8CD -- play_sample_row reloads its row-bit-count from this
 *       field every row, so it is the real playback-rate control, not a dead
 *       write (see play_sample_row's own Conv note).
 */
static void play_fixed_sample_start(chqstate_t *state,
                                    int         A_pitch_param,
                                    u8         *HL_data,
                                    int         D_length)
{
  /* $F8C2 LD ($F8CE),A / $F8C5 LD A,$01 / $F8C7 LD ($F8A2),A / $F8CA JR
   * $F8CD (13+7+13+12=45). */
  state->speccy->logtime(state->speccy, 45);
  state->bank3->drums.sample_pitch_param = A_pitch_param;
  state->bank3->drums.sample_active      = 1;
  play_sample_row(state, D_length, HL_data); /* was FALLTHROUGH */
}

/* T-states one titlescr_music call spends before it can resume sample
 * playback: the AY driver at $EC71 plus the slot-1/slot-2 dispatch above.
 * On real hardware that time comes out of the same interrupt period the
 * bit-bang loop runs in, so the loop never gets a whole frame. */
#define TITLE_MUSIC_TICK_TSTATES (8384)

/* What is left of one real ZX Spectrum 128K interrupt period (3546900 Hz CPU
 * clock / 50.021 Hz frame rate = 70908 T-states) for bit-banging -- the budget
 * play_sample_row's mid-sample yield check compares itself against, standing
 * in for the real hardware's $F8A8 frame flag (see play_sample_row's own Conv
 * note). Both figures are measured, not assumed: an instruction-level trace of
 * the real game's title screen (8096 frames) shows playing frames bit-bang a
 * median 98 rows = 62524 T-states, leaving 8384 for the tick. Not to be
 * confused with TITLE_MUSIC_TSTATES, which paces the outer call cadence in
 * wall-clock terms and is left alone. */
#define SAMPLE_ROW_FRAME_TSTATES (FRAME_TSTATES - TITLE_MUSIC_TICK_TSTATES)

/**
 * $F8CC/$F8CD: Pulse a 1-bit PCM sample out over the beeper
 *
 * Bit-bangs port $FE (border/speaker) from bitmap data at HL_data, one row of
 * 8 bits per sample byte: each bit test (BIT 7,(HL)) selects a full or muted
 * EAR pulse, then RLC (HL) rotates the next bit into position for the
 * following iteration -- the byte doubles as its own 8-iteration counter.
 * After each byte, D_length is decremented; when it reaches 0 the sample is
 * complete and finish_sample_playback clears sample_active.
 *
 * \param[in]     D_length Number of sample bytes remaining to output (was D).
 * \param[in,out] HL_data  Pointer to the next sample byte to play; mutated in
 *                         place by the RLC rotation (was HL).
 *
 * Conv: the Z80 re-enters this loop at $F8CC (with an EXX banking in a shadow
 *       HL'/D' saved by an earlier early exit) when a genuine 50Hz interrupt
 *       fires mid-sample -- tested at $F8E2 via the $F8A8 "frame occurred" flag
 *       -- so that playback resumes on the next titlescr_music call rather than
 *       completing in one go. This matters here: sample2 (224 bytes) at its
 *       usual pitch takes around 143000 T-states to bit-bang, well over one
 *       real 70908 T-state interrupt period, so on real hardware it genuinely
 *       spans several frames. The C port has no background interrupt to set
 *       $F8A8 asynchronously, so SAMPLE_ROW_FRAME_TSTATES below stands in for
 *       it: once this call has spent the bit-bang time a real frame leaves
 *       after the music tick, it yields exactly as $F8E2's check would, saving
 *       position in sample_resume_ptr/sample_resume_rows (the shadow HL'/D'
 *       equivalent) for titlescr_music's tail to resume next call. Playing
 *       every sample to completion in a single call instead (as
 *       playdrum_go/es_playdrum_go still do for the 48K and bank 7 drum
 *       samples) starves the rest of titlescr_music -- the AY tick, and the
 *       drum dispatch stream itself -- of the frames a long sample should
 *       genuinely take, which is audible as increasingly late drum hits ("lazy
 *       drummer").
 *
 * Conv: the inter-OUT delay code is modelled as speccy->logtime so the host can
 *       reconstruct the bit timing -- same accounting as playdrum_go/
 *       es_playdrum_go, whose bit-bang loop is byte-for-byte identical to this
 *       one bar the row-bit-count: theirs is a fixed 8 iterations, this one
 *       reloads B from sample_pitch_param ($F8CE, self-modified by
 *       play_fixed_sample_start) every row, since $F8CD's own "LD B,$08"
 *       operand is that same byte -- the dispatch byte's pitch/rate parameter
 *       is this loop's actual iteration count, not a spectator value.
 */
static void play_sample_row(chqstate_t *state, int D_length, u8 *HL_data)
{
  zxspectrum_t *speccy;        /* game's ZX Spectrum facade (was N/A) */
  int           frame_tstates; /* bit-bang T-states spent so far this call (was N/A) */
  int           i;             /* inner loop counter: row-bit-count from sample_pitch_param (was B) */
  int           bits;          /* speaker output level: port_MASK_EAR or 0 based on sample bit 7 (was A) */
  int           carry;         /* carry from the RLC rotation, unused after (carry) */

  speccy        = state->speccy;
  frame_tstates = 0;
  for (;;)
  {
    /* Conv: DJNZ with B = 0 loops 256 times */
    i = state->bank3->drums.sample_pitch_param
            ? state->bank3->drums.sample_pitch_param
            : 256;
    do
    {
      bits = port_MASK_EAR; // speaker bit
      if ((*HL_data & (1 << 7)) == 0)
        bits = 0;
      speccy->out(speccy, port_BORDER_EAR_MIC, bits);
      RLC(*HL_data); /* rotate sample byte in place */
      /* inter-bit cost 15+13+7+4+12+12 (bit-set path) */
      speccy->logtime(speccy, 63);
      /* The OUT above costs a further 11, billed to the virtual clock by the
       * facade's out() rather than by logtime. The yield budget must count the
       * full 74 or it lets ~16% too many rows through per frame. */
      frame_tstates += 63 + 11;
    } while (--i > 0);
    HL_data++;
    /* inter-byte cost 6+4+7+13+4+10+7, less the DJNZ not-taken saving */
    speccy->logtime(speccy, 46);
    frame_tstates += 46;
    if (--D_length == 0)
    {
      finish_sample_playback(state);
      return;
    }
    if (frame_tstates >= SAMPLE_ROW_FRAME_TSTATES)
    {
      /* Conv: $F8E2's flag check -- one real interrupt period's worth of
       * bit-banging has passed, so yield back to titlescr_music exactly as
       * the Z80 does, saving position for the next call to resume from. */
      state->bank3->drums.sample_resume_ptr  = HL_data;
      state->bank3->drums.sample_resume_rows = (u8) D_length;
      return;
    }
  }
}

/**
 * $F8EB: Finish sample playback
 *
 * Clears the 1-bit sample playback active flag once play_sample_row has
 * output every byte of the armed sample.
 *
 * Conv: the Z80 tail-jumps to wait_for_frame_flag ($F8A7) to busy-wait for the
 *       next interrupt. wait_for_frame_flag has no C equivalent -- like
 *       play_music_48k, this call already represents one already-paced tick
 *       (see titlescr_music's callers), so there is nothing left to wait for;
 *       the function simply returns.
 */
static void finish_sample_playback(chqstate_t *state)
{
  state->bank3->drums.sample_active = 0;
}

/**
 * $FA3A: Play a drum noise burst
 *
 * Drum-sample dispatch selector 3's entry point (see titlescr_music's
 * prologue).
 * Generates a noise burst on the beeper by running an LFSR-like update on the
 * 3-byte noise_phase/noise_accum/noise_rotate state, then toggling the
 * EAR/MIC outputs whenever bit 4 of the result is set. The outer loop runs
 * E_duration ticks; each tick iterates an inner loop of 50 noise steps. On
 * each step the phase/accumulator/rotate bytes are updated, and if bit 4
 * fires, two timed pulses are written to port_BORDER_EAR_MIC: first high
 * after ($18 - E_duration) delay iterations, then low after E_duration
 * iterations. Same update sequence as play_noise (Main.c) and
 * es_play_noise (Bank7.c), operating on this bank's own 3-byte state
 * instead of rng_seed.
 *
 * \param[in] E_pitch_param Noise duration: outer loop count and pulse timing
 *                          parameter (was A, moved to E at entry).
 *
 * Conv: Z80 drives the border port via OUT ($FE); C issues the equivalent write
 *       via speccy->out and models the delay loops as speccy->logtime so the
 *       host can reconstruct the pulse timing -- same accounting as play_noise.
 *
 * Conv: the Z80 polls the $F8A8 "frame occurred" flag after each inner-loop
 *       iteration ($FA67-$FA6B) to bail out early on a genuine 50Hz interrupt.
 *       frame_interrupt_handler is a no-op in this port, so $F8A8 never becomes
 *       non-zero and that early-exit branch is unreachable here: both loops
 *       always run to completion within one call, matching play_noise's own
 *       Conv note.
 */
static void play_drum_noise_burst(chqstate_t *state, int E_pitch_param)
{
  zxspectrum_t *speccy;     /* game's ZX Spectrum facade (was N/A) */
  int           carry;      /* carry from RLC/RRC operations on noise state (carry) */
  int           E_duration; /* outer loop count and pulse high/low timing parameter (was E) */
  int           D_inner;    /* inner loop count: 50 noise steps per tick (was D) */
  int           B_phase;    /* phase byte read after the +3 advance (was B) */
  u8            A;          /* LFSR result byte; bit 4 gates the speaker pulse (was A) */

  speccy     = state->speccy;
  carry      = 0;
  E_duration = E_pitch_param;

  do
  {
    D_inner = 50;
    do
    {
      state->bank3->drums.noise_phase += 3;
      B_phase                          = state->bank3->drums.noise_phase;
      A                                = state->bank3->drums.noise_accum - 0x8D;
      state->bank3->drums.noise_accum  = A;
      A                               += B_phase;
      RLC(A);
      RRC(state->bank3->drums.noise_rotate);
      A                                += state->bank3->drums.noise_rotate;
      state->bank3->drums.noise_rotate  = A;
      /* $FA3D-$FA50: phase/accumulator/rotate advance + AND $10 (127 T-states) */
      speccy->logtime(speccy, 127);
      if (A & (1 << 4))
      {
        /* $FA52: JR Z not taken; LD A,$18; SUB E; LD B,A (7+7+4+4) + DJNZ */
        speccy->logtime(speccy, 22 + DJNZ_LOOP_TSTATES(0x18 - E_duration));
        speccy->out(speccy, port_BORDER_EAR_MIC, port_MASK_EAR | port_MASK_MIC);
        /* $FA5E: LD B,E; DJNZ; XOR A (4 + loop + 4) */
        speccy->logtime(speccy, 8 + DJNZ_LOOP_TSTATES(E_duration));
        speccy->out(speccy, port_BORDER_EAR_MIC, 0);

        speccy->logtime(speccy, 16);
      } else
      {
        /* $FA52: JR Z taken; DEC D; JR NZ (12+4+12) */
        speccy->logtime(speccy, 28);
      }
    } while (--D_inner > 0);
  } while (--E_duration > 0);
}

/**
 * $FB99: Options-menu driver entry point
 *
 * One-time setup for the control-select menu: arms the IM2 interrupt
 * vector table, starts tune 0, and syncs to the next interrupt, then falls
 * into the redraw+poll loop at omd_redraw_and_poll ($FBA2).
 *
 * Called from the cold-boot entry point ($C009, BANK3_INPUT_SELECTION), which
 * attract_mode_128k reaches when ENTER is pressed during the attract demo. The
 * ENTER exit from the title screen's own wait loop (titlescr_wait_loop, $C63E
 * JP C,$FBA2) re-enters at omd_redraw_and_poll directly instead, skipping this
 * one-time setup.
 *
 * \return 1 always -- see omd_redraw_and_poll's return-value doc.
 */
static u8 options_menu_driver(chqstate_t *state)
{
  setup_im2_interrupt_table(state);
  titlescr_start_tune(state, 0);

  /* $FBA0 EI / $FBA1 HALT: sync to the next interrupt before entering the poll
   * loop. Conv: not modelled -- omd_redraw_and_poll's own per-frame stamp/sleep
   * pair covers it. A stamp() here would never be slept out and would leak a
   * host stack entry per options-menu entry (same treatment as run_title_screen's
   * $C61D EI/HALT). */

  return omd_redraw_and_poll(state); /* $FBA2: falls straight in */
}

/**
 * $FBA2: options-menu redraw + poll + dispatch loop
 *
 * Draws the control-select screen text, then polls half-row $F7FE (keys
 * "1"-"5") and dispatches: "1" -> Sinclair joystick key list, "2" -> Cursor
 * joystick key list (both installed via a shared 5-byte copy), "3" ->
 * Kempston-joystick detection, "4" -> keyboard (no key-list copy, keeps
 * whatever is already in state->control_keys), "5" (falls through
 * unbranched, the default) -> "DEFINE KEYS" screen, after which the whole
 * loop redraws and re-polls.
 *
 * Once a scheme is chosen (any path other than "5"), installs the
 * active-control-config header and hands off to the title screen.
 *
 * \return 1 always, at the point corresponding to the Z80's `JP $C59E`
 * ($FC11) -- the caller should now (re-)run title_screen_driver.
 *
 * Conv: this function uses goto/labels rather than nested structured loops.
 *       $FBA2 (full redraw) and $FBAB (poll only, no redraw) are two genuinely
 *       distinct restart points reached from different call sites -- the "no
 *       key" and "key 5" exits target $FBA2; the Kempston-detector's bail-out
 *       targets $FBAB. Modelling both with a single loop would either duplicate
 *       the redraw block or redraw when the Z80 does not.
 *
 * Conv: $FBAE-$FBB3 (`LD A,$F7` / `IN A,($FE)` / `CPL` / `AND $1F`) collapses
 *       to a single inverted, masked port_KEYBOARD_12345 read (same collapse as
 *       titlescr_wait_loop's fire/credit/anykey checks).
 *
 * Conv: $FBB7-$FBC1 (four `RRA` / `JR C` pairs testing bits 0-3 of the 5-bit
 *       mask in turn) collapse to direct bit tests against A_key_mask; key "5"
 *       is whatever remains after all four bits test false, matching the Z80's
 *       unbranched fallthrough default.
 */
static u8 omd_redraw_and_poll(chqstate_t *state)
{
  u8        A_key_mask;   /* keys "1".."5" pressed bitmask, bit0=key"1"..
                           * bit3=key"4"; the exit debounce reuses it as an
                           * any-key mask (was A) */
  const u8 *HL_ctrl_list; /* joystick key-list source, list A or B (was HL) */
  u8        A_flag;       /* input-method flag written to the active-
                           * config header byte: 0 = joystick/keyboard
                           * scheme installed normally, 1 = no Kempston
                           * joystick detected (was A) */

redraw:
  clear_options_screen(state);

  print_string(state, &options_menu_text[0]); /* $FBA5-$FBA8: "ENTER OPTION" /
                                                * P1-P5 control-scheme list. */

  update_whole_playfield(state); /* Conv: added */

poll: /* $FBAB omd_service_and_read_keys */
  do
  {
    run_title_tune(state);

    A_key_mask = (u8) (~state->speccy->in(state->speccy, port_KEYBOARD_12345) & 0x1F);
  } while (A_key_mask == 0);

  if (A_key_mask & 0x01) { /* $FBB7/$FBB8: key "1" -> Sinclair joystick */
    HL_ctrl_list = sinclair_joystick_keys;
    goto install_joystick_keys;
  }
  if (A_key_mask & 0x02) { /* $FBBA/$FBBB: key "2" -> Cursor joystick */
    HL_ctrl_list = cursor_joystick_keys;
    goto install_joystick_keys;
  }
  if (A_key_mask & 0x04) { /* $FBBD/$FBBE: key "3" -> Kempston detect */
    if (!detect_kempston_joystick(state))
      goto poll; /* $FC1C JR NZ,$FBAB: joystick activity seen, poll again */
    A_flag = 1;
    goto shared_tail;
  }
  if (A_key_mask & 0x08) { /* $FBC0/$FBC1: key "4" -> keyboard, inline */
    A_flag = 0;
    goto shared_tail;
  }

  /* $FBC3: key "5" (default, falls through unbranched) -> "DEFINE KEYS" */
  redefine_keys_screen(state);
  goto redraw;

install_joystick_keys:
  memcpy(state->bank3->control_keys, HL_ctrl_list, 5);
  A_flag = 0;

shared_tail:
  /* $FBE5-$FBF8: install the chosen scheme into the 9-byte control-config
   * block at ($8008). Conv: that block is $A0CC in the main binary --
   * state->kempston_flag followed by state->keydefs[] -- so the Z80's two
   * LDIRs into ($8008) become two copies into those fields. The key codes
   * need no translation: scan_keyboard_matrix packs them as key<<3|halfrow,
   * exactly what keyscan_inner unpacks. */
  state->kempston_flag = A_flag;

  memcpy(&state->keydefs[KEYDEF_QUIT], &state->bank3->control_keys[5], 3);
  memcpy(&state->keydefs[KEYDEF_GEAR], &state->bank3->control_keys[0], 5);

  clear_options_screen(state);

  update_whole_playfield(state); /* Conv: added */

  do
  {
    run_title_tune(state);

    /* $FC00 XOR A / IN A,($FE): all half-rows at once, i.e. any key. */
    A_key_mask = (u8) (~state->speccy->in(state->speccy, port_BORDER_EAR_MIC) & 0x1F);
    /* debounce: wait for the selection key to be released before proceeding */
  } while (A_key_mask != 0);

  stop_music_and_silence(state);

  state->controls_selected = 1;

  /* $FC10 DI: omitted -- SDL owns interrupt delivery, matching every other
   * DI/EI site in this file (see titlescr_wait_loop's prologue). */

  return 1; /* $FC11 JP $C59E: hand off to run_title_screen */
}

/**
 * $FBC8: Services sound each frame and keeps the options-menu tune looping
 *
 * Runs one frame of the drum-sample/music service and, if no tune is
 * currently active, restarts tune 0.
 */
static void run_title_tune(chqstate_t *state)
{
  /* Conv: every caller is an unbounded 128K options/redefine poll loop
   * (omd_redraw_and_poll, redefine_keys_screen, read_new_key_definition,
   * detect_kempston_joystick) which paces itself solely by calling here. The
   * host quit check therefore lives in the shared tick rather than being
   * repeated at each of those loops. */
  CHECK_HOST_QUIT(state);

  titlescr_music(state);

  /* restart the tune if it's finished */
  if (!state->bank3->title_music.tune_active)
    titlescr_start_tune(state, 0);
}

/**
 * $FC14: Joystick-present detector for the control-select sub-screen
 *
 * Samples the Kempston port 20 times, servicing sound each iteration, and
 * bails out as soon as the port's value changes (a joystick is moving or
 * present).
 *
 * \return 0 if Kempston port activity was detected within the sample
 * window -- the caller must return to the poll loop without installing any
 * control scheme. 1 if no joystick was detected -- the caller falls into
 * the shared control-install tail with the input-method flag set to 1.
 */
static u8 detect_kempston_joystick(chqstate_t *state)
{
  u8  A_sample;   /* current Kempston sample (was A) */
  u8  C_baseline; /* first Kempston sample (was C) */
  int B_count;    /* sample loop countdown, 20 iterations (was B) */

  A_sample   = state->speccy->in(state->speccy, port_KEMPSTON_JOYSTICK);
  C_baseline = A_sample;

  B_count = 20;
  do
  {
    A_sample = state->speccy->in(state->speccy, port_KEMPSTON_JOYSTICK);
    if (A_sample != C_baseline)
      return 0;

    run_title_tune(state);
  } while (--B_count);

  return 1;
}

/**
 * $FD9C: Print one or more back-to-back packed text records
 *
 * Calls print_character to draw the record at HL_string, then repeats for the
 * next record if the byte immediately following the terminator is non-zero.
 * The final zero byte is a pad, not part of any record (e.g. the $FC29
 * block's "$FD96 pad byte").
 *
 * \param[in] HL_string Pointer to the first record (was HL).
 */
static void print_string(chqstate_t *state, const u8 *HL_string)
{
  for (;;)
  {
    HL_string = print_character(state, HL_string);
    if (*HL_string == 0)
      return;
  }
}

/**
 * $FDA4: Print one packed text record (position + colour + characters)
 *
 * Unpacks a 3-byte record header -- style/colour byte, then a 2-byte screen
 * pixel address -- then draws each following character until one with bit 7
 * set (the record terminator) is printed. Despite the "print a single
 * character" name inherited from the skool, this draws a whole run of
 * characters sharing one position/colour, since its own internal loop
 * (mdc-style) only returns after the terminator; #print_string only calls
 * this again if further records follow in memory.
 *
 * Each character byte in the stream (with bit 7 masked off) is either a
 * literal space ($20, advances the column without drawing) or a character byte
 * mapped through a range ladder to one of 41 glyphs in #font, blitted
 * double-height (7 font bytes -> 15 scanlines across two attribute rows,
 * BRIGHT set on the upper row) or single-height (7 font bytes, one scanline
 * each), selected by the header's style bit.
 *
 * Sister function: menu_draw_char ($EC2C, Main.c). The two are near-clones --
 * the same glyph ladder, the same 4-row/-2016/3-row double-height blit and the
 * same seven-row single-height blit. They are kept separate because they are
 * separate routines at separate addresses in separate banks. The differences
 * are plumbing only: this one loops over a packed record and derives its
 * attribute address from D/E, where menu_draw_char draws a single character
 * and takes the attribute address as a parameter. Fix bugs in both.
 *
 * \param[in] HL_record Pointer to the 3-byte header + character stream
 *                      (was HL).
 * \return              Pointer to the byte following the record's terminator
 *                      (was HL).
 *
 * Conv: $FDA4-$FDB9 (header unpack), the classification ladder ($FDDA-$FDFE)
 *       and the double/single-height blits ($FE16-$FE7E) all follow
 *       menu_draw_char ($EC2C) precedent -- an essentially identical blit for
 *       an essentially identical font -- but this function has no cross-call
 *       persisted state to carry via EXX, so the shadow-register dance the Z80
 *       uses to snapshot the per-character screen pointer ($FE0D-$FE11:
 *       EXX/PUSH DE/INC E/EXX/POP DE, "pop scr addr as-was") collapses to a
 *       plain local: compute DE_screen from the *current* E_screen, then
 *       increment E_screen for the next character. Similarly, $FDBA EXX/$FDBB
 *       EX (SP),HL (banking the char/shape stream pointer while the attribute
 *       address sits in shadow HL') has no observable effect in C beyond naming
 *       which quantity is "the shape cursor" from this point on; modelled as a
 *       plain assignment, not a literal register swap.
 */
static const u8 *print_character(chqstate_t *state, const u8 *HL_record)
{
  u8        C_byte0;      /* packed style-bit + colour byte (was C) */
  u8        C_attrs;      /* colour value, bits 0-6 of byte0 (was C) */
  u8        A_style_bit;  /* byte0 bit 7: 0 = double-height shaded glyph, 1 = single-height flat glyph (was carry via EX AF,AF') */
  u8        E_screen;     /* pixel screen address low byte; advances one per column (was E) */
  u8        D_screen;     /* pixel screen address high byte; constant across the whole call (was D) */
  u8        H_attr;       /* attribute address high byte: $58 + third (was H) */
  u8        L_attr;       /* attribute address low byte; advances one per column (was L) */
  const u8 *HL_shape;     /* char/shape-byte stream cursor (was HL) */
  u8        A_char;       /* current column's character byte, bits 0-6 (was A) */
  u8        A_diff;       /* char - $20; classification input (was A) */
  u8        C_class;      /* width-class index (was C) */
  const u8 *HL_font;      /* pointer to this glyph's 7-byte font[] entry (was HL) */
  u8       *DE_screen;    /* pixel destination for this glyph (was DE) */
  int       row;          /* row loop counter; no Z80 equivalent (Conv: rolled) */
  int       glyph_addr;   /* Z80 screen address of this glyph's top-left pixel; kept for the dirty-region update (Conv: added) */
  int       glyph_height; /* this glyph's height in scanlines (Conv: added) */
  u8        terminator;   /* bit 7 of the character byte: terminates the outer loop (was flags) */

  C_byte0     = *HL_record;
  C_attrs     = C_byte0 & ~SINGLE_HEIGHT;
  A_style_bit = (C_byte0 >> 7) & 1;

  E_screen   = HL_record[1];
  D_screen   = HL_record[2];
  HL_record += 3;

  H_attr = (u8) (0x58 + ((D_screen >> 3) & 0x03));
  L_attr = E_screen;

  HL_shape = HL_record; /* // EXX / EX (SP),HL - bank ($FDBA-$FDBB) */

  do
  {
    A_char = *HL_shape & ~EOS;

    if (A_char == ' ')
    {
      /* $FDD1-$FDD9: space */
      E_screen++;
      L_attr++;
    } else
    {
      A_diff = (u8) (A_char - ' ');

      /* $FDDA-$FDFE classification ladder */
      if (A_diff >= '!')
        C_class = (u8) (A_diff - 18);
      else if (A_diff >= 0x10)
        C_class = (u8) (A_diff - 11);
      else if (A_diff == 1)
        C_class = 0;
      else if (A_diff == 8)
        C_class = 1;
      else if (A_diff == 9)
        C_class = 2;
      else if (A_diff == 12)
        C_class = 3;
      else
        C_class = 4;

      HL_font = &font[C_class * 7];

      /* $FE0D-$FE11: shared destination snapshot for both branches below. */
      glyph_addr = (D_screen << 8) | E_screen;
      DE_screen  = ADDRTOSCREEN(glyph_addr);
      E_screen++;

      if (!A_style_bit)
      {
        /* $FE16-$FE4E: double-height, 7 font bytes -> 15 rows */
        for (row = 0; row < 4; row++) { /* Conv: rolled */
          *DE_screen = *HL_font;
          DE_screen += 256;
          *DE_screen = *HL_font++;
          DE_screen += 256;
        }
        /* $FE30-$FE37: crosses 8-scanline group boundary. Conv: the literal
         * Z80 does E += 0x1F, D -= 7 on the *register* DE (D0+7, E0+1 at
         * this point), landing on (D0, E0+32). The rolled loop above instead
         * advances DE_screen by 256 twice per font byte, so it is already 255
         * bytes further along (D0+8, E0) than the literal register state.
         * -2016 is the byte delta that lands this pointer on the same
         * (D0, E0+32) target; 0xF820 (65536-2016) is only equivalent to that
         * under 16-bit modular register arithmetic, not real pointer
         * arithmetic, so it must not be added directly to DE_screen. */
        DE_screen -= 2016;
        for (row = 0; row < 3; row++) { /* Conv: rolled */
          *DE_screen = *HL_font;
          DE_screen += 256;
          *DE_screen = *HL_font++;
          DE_screen += 256;
        }
        *DE_screen = 0; /* $FE4D-$FE4E: final row always blank */

        *ADDRTOATTRS((H_attr << 8) | L_attr) = C_attrs | ATTR_BRIGHT;
        *ADDRTOATTRS((H_attr << 8) | (u8) (L_attr + 0x20)) = C_attrs & ~ATTR_BRIGHT;
        L_attr++;

        glyph_height = 16; /* two character rows (Conv: added) */
      } else
      {
        /* $FE5F-$FE78: single-height, 7 font bytes, one row each */
        for (row = 0; row < 7; row++) { /* Conv: rolled */
          *DE_screen = *HL_font++;
          DE_screen += 256;
        }

        *ADDRTOATTRS((H_attr << 8) | L_attr) = C_attrs;
        L_attr++;

        glyph_height = 7; /* seven scanlines, one character row (Conv: added) */
      }

      /* Conv: added -- mark this glyph's cell dirty so the host redraws it.
       * Both blits above write a column 8 pixels wide starting at
       * glyph_addr; the attribute writes fall inside the same box. */
      update_screen(state, glyph_addr, 8, glyph_height);
    }

    terminator = *HL_shape++ & EOS;
  } while (!terminator);

  return HL_shape;
}

/**
 * $FE7F: Clears the options-menu screen area (attributes and bitmap)
 *
 * Zero-fills the same $5900-$5AFF attribute range and $4800-$57FF bitmap
 * range as clear_playfield_and_attrs, servicing sound (titlescr_music,
 * via run_title_tune) between passes so the title tune keeps
 * advancing during the fill. Called from omd_redraw_and_poll ($FBA2 and
 * $FBE5) and, once ported, the "define keys" screen ($FEA9/$FEF6).
 *
 * Conv: the Z80 does this as three LDIR chunks (attrs, then bitmap split into
 *       two chunks of $082F and $07D0 bytes) with a sound-service call between
 *       each pair; the bitmap fill collapses to one memset since nothing
 *       observes it mid-way, but all three service calls are kept, in the same
 *       order, so the tune advances by the same number of steps as the Z80.
 *       Falls through into run_title_tune via a tail jump in the Z80 ($FEA6 JP
 *       $FBC8), modelled here as a plain call before returning.
 */
static void clear_options_screen(chqstate_t *state)
{
  memset(ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR), 0, 0x200);
  run_title_tune(state);

  memset(ADDRTOSCREEN(SCREEN_PLAYFIELD_BITMAP_ADDR), 0, 0x1000);
  run_title_tune(state);

  run_title_tune(state); /* tail call */
}

/**
 * $FEA9: "Redefine keys" screen driver
 *
 * Prints the title/prompt text and the 8 control-name labels (gear,
 * accelerate, brake, left, right, quit, pause, turbo), then captures a
 * fresh keypress for each of the 8 controls in turn via
 * read_new_key_definition, waiting out an any-key debounce before each
 * capture. After all 8 keys are set, waits ~20 frames, then compares the 8
 * keys just chosen against shocked_keydef_sequence (the hidden "SHOCKED" +
 * ENTER cheat code): on a match, enables test mode, shows the confirmation
 * screen, waits for any key, then loops back to redisplay this screen; on
 * any mismatch, returns immediately (the ordinary case -- the new mapping
 * is kept).
 *
 * Conv: modelled as an outer for(;;) that only exits via return (mismatch) --
 *       matching the Z80, which has no path back to the caller once the secret
 *       code has been entered other than by looping back to $FEA9 itself.
 *
 * Conv: $FEC1/$FED6 (`PUSH HL` / `INC HL`) walk a pointer that is never read
 *       back before the next iteration's `PUSH HL` overwrites it -- dead code,
 *       as with the identical stray HL increment noted in redefine_keys_48k
 *       ($ECF3). Not modelled.
 */
static void redefine_keys_screen(chqstate_t *state)
{
  u16 DE_screen;       /* current label print position (was DE) */
  int B_remaining;     /* controls remaining, counts down from 8 (was B) */
  u8  C_control_index; /* 1-based control index, counts up from 1 (was C) */
  u8  A_key_mask;      /* any-key-pressed bitmask, all half-rows (was A) */
  int B_wait;          /* ~20-frame post-capture wait counter (was B) */
  int B_shocked_i;     /* "SHOCKED"+ENTER compare loop index (was B) */

  for (;;)
  {
    clear_options_screen(state);

    print_string(state, &options_menu_text[114]); /* $FEAC-$FEAF: header +
                                                    * GEAR/ACCELERATE/BRAKE */
    run_title_tune(state);
    print_string(state, &options_menu_text[160]); /* $FEB5-$FEB8:
                                                    * LEFT/RIGHT/QUIT/PAUSE/TURBO */

    update_whole_playfield(state); /* Conv: added */

    DE_screen       = 0x48D6;
    B_remaining     = 8;      /* $FEBE LD BC,$0801: B half */
    C_control_index = 1;      /* $FEBE LD BC,$0801: C half */

    do
    {
      do
      {
        run_title_tune(state);

        /* $FECA XOR A / IN A,($FE): all half-rows at once, i.e. any key. */
        A_key_mask = (u8) (~state->speccy->in(state->speccy, port_BORDER_EAR_MIC) & 0x1F);
      } while (A_key_mask != 0); /* $FED0 JR NZ,$FEC1: wait for any key to be released */

      read_new_key_definition(state, &DE_screen, B_remaining, C_control_index);

      C_control_index++;
    } while (--B_remaining != 0);

    B_wait = 0x14;
    do
    {
      run_title_tune(state);
    } while (--B_wait != 0);

    for (B_shocked_i = 0; B_shocked_i < 8; B_shocked_i++)
      if (state->bank3->control_keys[B_shocked_i] != shocked_keydef_sequence[B_shocked_i])
        return; /* $FEEE RET NZ: mismatch -- ordinary case, keep the new mapping */

    state->test_mode = 1;

    clear_options_screen(state);
    print_string(state, &options_menu_text[199]); /* $FEF9-$FEFC: test-mode confirmation text */

    update_whole_playfield(state); /* Conv: added */

    do
    {
      run_title_tune(state);

      /* $FF02 XOR A / IN A,($FE): all half-rows at once, i.e. any key. */
      A_key_mask = (u8) (~state->speccy->in(state->speccy, port_BORDER_EAR_MIC) & 0x1F);
    } while (A_key_mask == 0); /* $FF08 JR Z,$FEFF: wait for any key */
  }
}

/**
 * $FF0C: Scans the keyboard matrix for a single currently-held key
 *
 * Walks the eight keyboard half-row ports ($FEFE, $FDFE, $FBFE, $F7FE,
 * $EFFE, $DFFE, $BFFE, $7FFE), rotating the row-select byte through all
 * eight in turn. Whichever row (if any) has a key held has its bit
 * position within that row's 5-bit mask found by repeated halving, and
 * combined with the row number into a single packed code (see
 * read_new_key_definition for how the code is unpacked again).
 *
 * \param[out] D_key_code_out Packed key code: 8*(4-bit) + (7-row). Left at 0xFF
 *                            if no key was held in any row (was D).
 * \return                    1 if more than one row (or more than one bit
 *                            within a row) was held simultaneously -- an
 *                            ambiguous scan the caller should reject and retry.
 *                            0 otherwise (D_key_code_out is 0xFF for "no key",
 *                            or a valid packed code for exactly one key held).
 */
static u8 scan_keyboard_matrix(chqstate_t *state, u8 *D_key_code_out)
{
  u8  D_key_code;     /* sentinel 0xFF at entry; row-found flag/result (was D) */
  int E_row_value;    /* row's contribution to the packed code, decremented per row (was E) */
  u8  B_port_hi;      /* high byte of keyboard IN port; rotated through all eight rows (was B) */
  u8  A_pressed_mask; /* active key bits for the current row: inverted, masked to 5 bits (was A) */
  u8  H_bits;         /* copy of A_pressed_mask, shifted right to find the set bit (was H) */
  u8  A_code;         /* row/bit code accumulator, decremented by 8 per shift (was A) */
  int carry;          /* carry from SRL/RLC operations (carry) */

  D_key_code  = 0xFF; /* $FF0C LD DE,$FF2F: D half */
  E_row_value = 0x2F; /* $FF0C LD DE,$FF2F: E half */
  B_port_hi   = 0xFE; /* $FF0F LD BC,$FEFE: B half */

  do
  {
    A_pressed_mask = (u8) (~state->speccy->in(state->speccy, (u16) ((B_port_hi << 8) | 0xFE)) & 0x1F);

    if (A_pressed_mask != 0)
    {
      D_key_code++;
      if (D_key_code != 0)
        return 1; /* $FF1A RET NZ: a second row is also held -- ambiguous */

      H_bits = A_pressed_mask;
      A_code = (u8) E_row_value;
      do
      {
        A_code -= 8;
        SRL(H_bits);
      } while (!carry);

      if (H_bits != 0)
        return 1; /* $FF23 RET NZ: more than one bit held in this row */

      D_key_code = A_code;
    }

    E_row_value--;
    RLC(B_port_hi);
  } while (carry);

  *D_key_code_out = D_key_code; /* $FF2A-$FF2B CP A / RET (Z always set here) */
  return 0;
}

/**
 * $FF2C: Waits for a fresh single keypress and stores it as one control's key
 *
 * Repeatedly scans the keyboard (scan_keyboard_matrix) until exactly one key
 * is held that is not already assigned to an earlier control in this
 * session (state->control_keys[0..C_control_index-2]), rejecting ambiguous
 * scans, "no key held" scans, and duplicates by looping back to rescan.
 * Stores the accepted key code at state->control_keys[C_control_index-1],
 * looks up its two-character display name in control_key_names[], prints it
 * at *DE_screen via print_character, then advances *DE_screen by one label
 * column (twice, when B_remaining is exactly 4 -- see
 * advance_key_label_column).
 *
 * \param[in,out] DE_screen       Screen address to print the key's name at;
 *                                updated to the next label position on return
 *                                (was DE).
 * \param[in]     B_remaining     Controls remaining in the outer 8-control
 *                                loop, including this one; when exactly 4, an
 *                                extra column advance is applied (was B).
 * \param[in]     C_control_index 1-based index of the control being defined,
 *                                into state->control_keys[] (was C).
 */
static void read_new_key_definition(chqstate_t *state,
                                    u16        *DE_screen,
                                    u8          B_remaining,
                                    u8          C_control_index)
{
  u8  ambiguous;   /* scan_keyboard_matrix ambiguity flag (was flags) */
  u8  D_key_code;  /* packed key code from scan_keyboard_matrix (was D) */
  u8  A_key_code;  /* accepted key code, used for storage/lookup (was A) */
  int B_dup_count; /* duplicate-check count: C_control_index-1 already-
                    * assigned slots (was B) */
  int dup_i;       /* duplicate-check loop index (was HL-$FFF7) */
  int index_bytes; /* byte offset into control_key_names[] (was HL-$FF95) */
  u8  char0;       /* first character of the looked-up key name (was A) */
  u8  char1;       /* second character, with the EOS bit set (was A) */

rescan:
  for (;;)
  {
    run_title_tune(state);

    ambiguous = scan_keyboard_matrix(state, &D_key_code);
    if (ambiguous)
      continue;

    if (D_key_code == 0xFF)
      continue;

    break;
  }
  A_key_code = D_key_code;

  B_dup_count = C_control_index - 1;
  for (dup_i = 0; dup_i < B_dup_count; dup_i++)
    if (A_key_code == state->bank3->control_keys[dup_i])
      goto rescan; /* $FF45 JR Z,$FF2E: duplicate -- rescan */

  state->bank3->control_keys[C_control_index - 1] = A_key_code;

  index_bytes = 10 * (A_key_code & 0x07) + 2 * (A_key_code >> 3);
  char0       = control_key_names[index_bytes];
  char1       = control_key_names[index_bytes + 1] | EOS;

  state->bank3->options_key_string[0] = 0xC7; /* Conv: fixed constant resident at
                                               * $FD97; never rewritten by this
                                               * routine (see key-name-table
                                               * comment in the skool). */
  setwordat(&state->bank3->options_key_string[1], *DE_screen);
  state->bank3->options_key_string[3] = char0;
  state->bank3->options_key_string[4] = char1;
  print_character(state, &state->bank3->options_key_string[0]);

  *DE_screen = advance_key_label_column(*DE_screen);
  if (B_remaining == 4)
    *DE_screen = advance_key_label_column(*DE_screen); /* $FF8A: mid-list row wrap */
}

/**
 * $FF8B: Advance the key-label print position by one label column
 *
 * Adds 32 to the low byte of the screen address, then adds 8 to the high
 * byte only if that addition overflowed -- i.e. only once every 8 columns,
 * when the low byte wraps back round. Same step as dak_move_down ($EDCC),
 * the 48K equivalent.
 *
 * \param[in] DE_screen Z80 screen address (was DE).
 * \return              Screen address advanced by one label column.
 */
static u16 advance_key_label_column(u16 DE_screen)
{
  int carry; /* carry out of the E += 32 addition (carry) */
  int E_sum; /* E + 32 before truncation, to test for overflow (was A) */
  u8  E;     /* low byte of DE_screen: byte column offset + 32 (was E) */
  u8  D;     /* high byte of DE_screen: pixel row within third (was D) */

  E_sum = (DE_screen & 0xFF) + 32;
  carry = E_sum > 0xFF;
  E     = (u8) E_sum;

  D = (u8) (DE_screen >> 8);
  if (carry)
    D += 8;

  return (u16) ((D << 8) | E);
}

/* ----------------------------------------------------------------------- */

/**
 * Allocate and initialise the bank 3 sub-state.
 *
 * Conv: host lifecycle helper; has no Z80 address. Called once from chq_create.
 *
 * \return 0 on success, -1 if allocation failed.
 */
int bank3_state_create(chqstate_t *state)
{
  // clang-format off
  /* 128K bank 3: two fixed 1-bit PCM "digitised sample" tables played by
   * play_sample_row via the drum-sample dispatch in sfx_music_service. Each
   * byte is one playback row of 8 bits, rotated out with RLC so playback
   * mutates the table in place -- state keeps a mutable per-game copy, these
   * are the pristine templates. Same underlying sample content as
   * CommonData.c's drum1_template/drum2_template and Bank7.c's
   * es_drum_sample_1_template/es_drum_sample_2_template (this game's PCM
   * drum/noise assets are duplicated, at slightly different lengths, across
   * every bank that plays them) --
   * transcribed separately here because bank 3's copies are shorter than
   * either. */

  /** $F8F2: drum_sample_1_template (104 bytes, played back with D=$68 rows) */
  static const u8 drum_sample_1_template[104] = {
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
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00
  };

  /** $F95A: drum_sample_2_template (224 bytes, played back with D=$E0 rows) */
  static const u8 drum_sample_2_template[224] = {
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
    0x7E, 0xFC, 0xFE, 0x7F, 0xE7, 0x6F, 0xDE, 0x00
  };

  /* 128K bank 3: preset high-score table rows, $C408-$C552 (33-byte stride
   * in the original; only the fields that move are transcribed here -- see
   * high_score_row_t, Bank3State.h). Row 0 = 1st place .. row 9 = 10th
   * place. Row 0's initials "JOB" are John O'Brien, this game's
   * programmer. */
  static const high_score_row_t high_score_table_template[HIGH_SCORE_TABLE_ROWS] = {
    { "10000000", "ALL", '2', "JOB" }, /* $C408: row 0 (1st place) */
    { " 9888700", " 5 ", '2', "BIL" }, /* $C429: row 1 (2nd place) */
    { " 8900000", " 4 ", '1', "JON" }, /* $C44A: row 2 (3rd place) */
    { " 5789000", " 4 ", '2', "JAM" }, /* $C46B: row 3 (4th place) */
    { " 3500000", " 2 ", '1', "ROB" }, /* $C48C: row 4 (5th place) */
    { " 2456000", " 2 ", '1', "J.B" }, /* $C4AD: row 5 (6th place) */
    { "  956700", " 1 ", '1', "M.L" }, /* $C4CE: row 6 (7th place) */
    { "  340500", " 1 ", '1', "STE" }, /* $C4EF: row 7 (8th place) */
    { "  235050", " 1 ", '1', "PIX" }, /* $C510: row 8 (9th place) */
    { "  123000", " 1 ", '1', "IES" }, /* $C531: row 9 (10th place) */
  };

  // clang-format on

  state->bank3 = calloc(1, sizeof(*state->bank3));
  if (state->bank3 == NULL)
    return -1;

  /* Conv: calloc has already zeroed the sub-state, so only the fields whose
   * pristine value is non-zero are written below.
   */

  /* mixer_mask is static initial RAM content in the Z80 (not written by
   * titlescr_start_ay), confirmed against the skool's DEFB data: channel-tracker
   * $EC01 -> $EC25=$09, $EC26 -> $EC4A=$12, $EC4B -> $EC6F=$24. */
  state->bank3->title_music.channel[0].mixer_mask = AY_MIXER_NO_NOISE_A | AY_MIXER_NO_TONE_A;
  state->bank3->title_music.channel[1].mixer_mask = AY_MIXER_NO_NOISE_B | AY_MIXER_NO_TONE_B;
  state->bank3->title_music.channel[2].mixer_mask = AY_MIXER_NO_NOISE_C | AY_MIXER_NO_TONE_C;

  /* $EFAF-$EFBA (128K bank 3): title-tune AY register cache. Matches the
   * skool's DEFB bytes at $EFAF-$EFBA exactly; per the skool comment these
   * are placeholder start-up defaults, overwritten every frame once a tune
   * is playing (mixer = 0x3F disables all tone/noise channels until then).
   */
  state->bank3->title_ay_regs.mixer      = AY_MIXER_ALL_OFF;
  state->bank3->title_ay_regs.chan_a_vol = 0x0F;
  state->bank3->title_ay_regs.chan_b_vol = 0x0F;
  state->bank3->title_ay_regs.chan_c_vol = 0x0F;

  memcpy(state->bank3->drums.sample1, drum_sample_1_template, sizeof(state->bank3->drums.sample1));
  memcpy(state->bank3->drums.sample2, drum_sample_2_template, sizeof(state->bank3->drums.sample2));

  memcpy(state->bank3->high_score_table, high_score_table_template,
         sizeof(state->bank3->high_score_table));

  /* $FFF7-$FFFE (128K bank 3): pristine scan-key codes for the keyboard
   * scheme. Option "4" copies no key list, so these are what gets installed
   * unless the player redefines the keys first.
   */
  memcpy(state->bank3->control_keys, default_control_keys,
         sizeof(state->bank3->control_keys));

  /* Conv: seed the live keydefs from the same table, in the layout
   * omd_redraw_and_poll installs ($FBE5-$FBF8). The Z80 needs no equivalent:
   * its $A0CD table is assembled with the Sinclair-joystick codes and is
   * always overwritten by the options menu before play. Ours can be reached
   * with the menu skipped while BANK3_INPUT_SELECTION is stubbed, and zeroed
   * keydefs put every action on one key.
   */
  memcpy(&state->keydefs[KEYDEF_QUIT], &default_control_keys[5], 3);
  memcpy(&state->keydefs[KEYDEF_GEAR], &default_control_keys[0], 5);

  return 0;
}

/**
 * Free the bank 3 sub-state.
 *
 * Conv: host lifecycle helper; has no Z80 address. Called once from
 *       chq_destroy.
 */
void bank3_state_destroy(chqstate_t *state)
{
  free(state->bank3);
  state->bank3 = NULL;
}

#ifdef CHQ_TESTS

void chq_test_start_title_tune(chqstate_t *state, u8 A_tune)
{
  titlescr_start_tune(state, A_tune);
}

void chq_test_run_title_tune(chqstate_t *state)
{
  run_title_tune(state);
}

void chq_test_insert_high_score_entry(chqstate_t *state, int row)
{
  insert_high_score_entry(state, row);
}

void chq_test_name_entry_setup_screen(chqstate_t *state, int row)
{
  state->bank3->hiscore.row = (u8) row;
  name_entry_setup_screen(state);
}

void chq_test_hiscore_inject_input(chqstate_t *state, u8 user_input_flags)
{
  name_entry_dispatch(state, user_input_flags);
}

void chq_test_scroll_score_rows(chqstate_t *state)
{
  scroll_score_rows(state);
}

#endif /* CHQ_TESTS */
