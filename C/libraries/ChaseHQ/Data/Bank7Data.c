/**
 * Bank7Data.c
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

#include "C99/Types.h"

#include "ZXSpectrum/Macros.h"
#include "ZXSpectrum/Pixels.h"
#include "ZXSpectrum/Spectrum.h"
#include "ChaseHQ/Engine/Types.h"

#include "ChaseHQ/Data/Bank7Data.h"

#include "Stages.h"

/* ----------------------------------------------------------------------- */

// clang-format off

/* Private argument-encoding macros for es_script below; command values
 * (ESCMD_*_VAL) are shared with Bank7.c's run_script switch via
 * Bank7Data.h. */
#define ESCMD_CLEAR_DRAW_FRAME(BMADDR, SCRADDR) ESCMD_CLEAR_DRAW_FRAME_VAL, TWOBYTES(BMADDR), TWOBYTES(SCRADDR)
#define ESCMD_DRAW_WORD(BMADDR, SCRADDR)        ESCMD_DRAW_WORD_VAL, TWOBYTES(BMADDR), TWOBYTES(SCRADDR)
#define ESCMD_FADE_IN_A                         ESCMD_FADE_IN_A_VAL
#define ESCMD_FADE_IN_B                         ESCMD_FADE_IN_B_VAL
#define ESCMD_HANDSHAKE                         ESCMD_HANDSHAKE_VAL
#define ESCMD_FADE_IN_C                         ESCMD_FADE_IN_C_VAL
#define ESCMD_IDLE(D)                           ESCMD_IDLE_VAL, (D)
#define ESCMD_RESET_HANDSHAKE(D)                ESCMD_RESET_HANDSHAKE_VAL, (D)
#define ESCMD_HANDSHAKE_AGAIN(D)                ESCMD_HANDSHAKE_AGAIN_VAL, (D)
#define ESCMD_DRAW_TEXT_NO_CLEAR(ATTR, SCRADDR) ESCMD_DRAW_TEXT_NO_CLEAR_VAL, (ATTR), TWOBYTES(SCRADDR)
#define ESCMD_DRAW_TEXT(ATTR, SCRADDR)          ESCMD_DRAW_TEXT_VAL, (ATTR), TWOBYTES(SCRADDR)
#define ESCMD_CHATTER(ADDR)                     ESCMD_CHATTER_VAL, TWOBYTES(ADDR)
#define ESCMD_DRAW_SCORE                        ESCMD_DRAW_SCORE_VAL

/* ----------------------------------------------------------------------- */

/**
 * $E0FE-$E209: End-screen script bytecode.
 *
 * Verbatim transcription of the skool's es_script block (268 bytes):
 * command/argument bytes interleaved with bitmap addresses (as raw
 * little-endian DEFW pairs) and embedded high-bit-terminated ASCII text
 * ("CONGRATULATIONS!", "ALL  CLEAR", "(C) 1989 OCEAN SOFTWARE", "(C) 1988
 * TAITO CORPORATION", "THE  END", "FINAL  SCORE"). Every byte has now been
 * decoded against run_script's command dispatch (Bank7.c) -- see the inline
 * comments through the tail of the array.
 *
 * Conv: this master copy is const. show_end_screen copies it into
 * state->bank7->es_script at entry; es_handler_draw_score patches the
 * "GBP________ PTS" placeholder text in-place (offset 0xFD, matching $5DFB
 * relocated) in that per-instance copy, not here, exactly as the original
 * self-modifies its own es_script at that address but without concurrent
 * game instances trampling each other's score text.
 */
// clang-format off
const u8 es_script[268] = {
  ESCMD_CHATTER(CHATTERBLK_NANCY_CONGRATULATES_ADDR),
  ESCMD_IDLE(0xC0),
  ESCMD_CLEAR_DRAW_FRAME(BITMAP_ENDSHOT_1_ADDR, XYTOSCREEN(72, 96)), // screen dst
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0xA0),
  ESCMD_FADE_IN_B,
  ESCMD_CLEAR_DRAW_FRAME(BITMAP_ENDSHOT_2_ADDR, XYTOSCREEN(72, 96)), // screen dst
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0xA0),
  ESCMD_FADE_IN_B,
  ESCMD_CLEAR_DRAW_FRAME(BITMAP_ENDSHOT_3_ADDR, XYTOSCREEN(72, 96)), // screen dst
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0xA0),
  ESCMD_FADE_IN_B,
  ESCMD_CLEAR_DRAW_FRAME(BITMAP_ENDSHOT_4_ADDR, XYTOSCREEN(72, 96)), // screen dst
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0xA0),
  ESCMD_FADE_IN_B,
  ESCMD_CLEAR_DRAW_FRAME(BITMAP_ENDSHOT_1_ADDR, XYTOSCREEN(16, 64)), // screen dst
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0x50),
  ESCMD_DRAW_WORD(BITMAP_ENDSHOT_2_ADDR, XYTOSCREEN(136, 64)), // screen dst
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0x50),
  ESCMD_DRAW_WORD(BITMAP_ENDSHOT_3_ADDR, XYTOSCREEN(16, 128)), // screen dst
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0x50),
  ESCMD_DRAW_WORD(BITMAP_ENDSHOT_4_ADDR, XYTOSCREEN(136, 128)), // screen dst
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0x50),
  ESCMD_RESET_HANDSHAKE(0xC0),
  ESCMD_HANDSHAKE_AGAIN(0xB0),
  ESCMD_HANDSHAKE,
  ESCMD_HANDSHAKE_AGAIN(0xB0),
  ESCMD_FADE_IN_B,
  ESCMD_DRAW_TEXT(attribute_BRIGHT_WHITE_OVER_BLACK, XYTOSCREEN(64, 80)), /* clear */
  'C', 'O', 'N', 'G', 'R', 'A', 'T', 'U', 'L', 'A', 'T', 'I', 'O', 'N', 'S', '!' | EOS,
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0x08),
  ESCMD_DRAW_TEXT_NO_CLEAR(attribute_BRIGHT_WHITE_OVER_BLACK, XYTOSCREEN(88, 112)),
  'A', 'L', 'L', ' ', ' ', 'C', 'L', 'E', 'A', 'R' | EOS,
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0x08),
  ESCMD_DRAW_TEXT_NO_CLEAR(attribute_BRIGHT_WHITE_OVER_BLACK, XYTOSCREEN(72, 144)),
  '5', ',', '0', '0', '0', ',', '0', '0', '0', ' ', ' ', 'P', 'T', 'S', '.' | EOS,
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0xC0),
  ESCMD_FADE_IN_C,
  ESCMD_IDLE(0x60),
  ESCMD_DRAW_TEXT(attribute_BRIGHT_WHITE_OVER_BLACK, XYTOSCREEN(32, 96)), /* clear */
  '(', 'C', ')', ' ', '1', '9', '8', '9', ' ', 'O', 'C', 'E', 'A', 'N', ' ', 'S', 'O', 'F', 'T', 'W', 'A', 'R', 'E' | EOS,
  ESCMD_DRAW_TEXT_NO_CLEAR(attribute_BRIGHT_WHITE_OVER_BLACK, XYTOSCREEN(24, 144)),
  '(', 'C', ')', ' ', '1', '9', '8', '8', ' ', 'T', 'A', 'I', 'T', 'O', ' ', 'C', 'O', 'R', 'P', 'O', 'R', 'A', 'T', 'I', 'O', 'N' | EOS,
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0xF0),
  ESCMD_FADE_IN_C,
  ESCMD_IDLE(0x60),
  ESCMD_DRAW_TEXT(attribute_BRIGHT_WHITE_OVER_BLACK, XYTOSCREEN(96, 112)), /* clear */
  'T', 'H', 'E', ' ', ' ', 'E', 'N', 'D' | EOS,
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0xF0),
  ESCMD_IDLE(0x60),
  ESCMD_FADE_IN_C,
  ESCMD_IDLE(0x60),
  ESCMD_DRAW_TEXT(attribute_BRIGHT_CYAN_OVER_BLACK, XYTOSCREEN(80, 104)), /* clear */
  'F', 'I', 'N', 'A', 'L', ' ', ' ', 'S', 'C', 'O', 'R', 'E' | EOS,
  ESCMD_FADE_IN_A,
  ESCMD_IDLE(0x1E),
  ESCMD_DRAW_SCORE, /* tallies bonus, patches offset 0xFD below with score ASCII */
  ESCMD_DRAW_TEXT_NO_CLEAR(attribute_BRIGHT_WHITE_OVER_BLACK, XYTOSCREEN(96, 136)), /* "GBP________ PTS" placeholder, digits patched at offset 0xFD by es_handler_draw_score */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' | EOS,
  ESCMD_FADE_IN_A,
  ESCMD_CHATTER(CHATTERBLK_PRESS_GEAR_ADDR),
  ESCMD_IDLE(0x00),
  0x0E /* unrecognised command: run_script's default case resets HL here (see
        * ES_SCRIPT_RESET_OFFSET in Bank7Data.h) rather than stopping */
};
// clang-format on

#undef ESCMD_CLEAR_DRAW_FRAME
#undef ESCMD_DRAW_WORD
#undef ESCMD_FADE_IN_A
#undef ESCMD_FADE_IN_B
#undef ESCMD_HANDSHAKE
#undef ESCMD_FADE_IN_C
#undef ESCMD_IDLE
#undef ESCMD_RESET_HANDSHAKE
#undef ESCMD_HANDSHAKE_AGAIN
#undef ESCMD_DRAW_TEXT_NO_CLEAR
#undef ESCMD_DRAW_TEXT
#undef ESCMD_CHATTER
#undef ESCMD_DRAW_SCORE

/* ----------------------------------------------------------------------- */

// clang-format off

/** $F53C (relocated; source $FA2B) */
const u8 es_music_patterns[23] = {
  // (repetitions, offset)
  0x01, 0x00,
  0x04, 0x22,
  0x04, 0x44,
  0x05, 0x56,
  0x01, 0x68,
  0x04, 0x7A, // weird separate repeat of same part
  0x04, 0x7A,
  0x03, 0x7A,
  0x01, 0x8C,
  0x3C, 0xA9, // silence: 60x repeat of the single silent note at 0xA9
  0xFF, // stop marker
  TWOBYTES(0xF54E) // restart address
};

// clang-format on
