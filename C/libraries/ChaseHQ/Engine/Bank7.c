/**
 * Bank7.c
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
 * C translation of ZX Spectrum 128K "bank 7" -- the end-of-game
 * results/credits sequence. Paged into $C000-$FFFF and reached via an inner
 * relocation from $F7EF to $F300, driven by a script interpreter at $E20A.
 * See show_end_screen() below. es_setup_interrupts remains a stub -- see its
 * own prologue. es_play_music_48k drives the beatbox each frame (relocated
 * to $F340 -- the real target of that call, not speech as originally
 * guessed) but its drum/noise instruments (es_playdrum_2/1, es_play_noise)
 * remain unported stubs, so playback is currently silent on instrument
 * notes.
 */

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "C99/Types.h"

#include "ZXSpectrum/Macros.h"
#include "ZXSpectrum/Pixels.h"
#include "ZXSpectrum/Spectrum.h"
#include "ZXSpectrum/Z80.h"

#include "ChaseHQ/ChaseHQ.h"

#include "ChaseHQ/Data/CommonData.h"

#include "Types.h"
#include "Internal.h"
#include "State.h"
#include "Bank7State.h"

#include "Bank7.h"

#include <assert.h>

/* ----------------------------------------------------------------------- */

typedef struct
{
  u8        rows;
  const u8 *image;
} handshake_frame_t;

/* End-screen script command bytes, dispatched by run_script's switch below.
 * Argument-encoding macros (ESCMD_CHATTER, ESCMD_IDLE, etc.) are private to
 * es_script's own definition, where they build the table below. */
#define ESCMD_CLEAR_DRAW_FRAME_VAL    (1) /* -> $E2D9 es_clear_then_draw_frame, runs immediately */
#define ESCMD_DRAW_WORD_VAL           (2) /* -> $E2DE es_handler_draw_word, runs immediately */
#define ESCMD_FADE_IN_A_VAL           (3) /* -> $E42E es_attribute_fade_in via rs_exit, reload 16 */
#define ESCMD_FADE_IN_B_VAL           (4) /* -> $E472 es_handler_glyph_fade_b via rs_exit, reload 16 */
#define ESCMD_HANDSHAKE_VAL           (5) /* -> $E3B7 es_handler_handshake via rs_exit, reload 16 */
#define ESCMD_FADE_IN_C_VAL           (6) /* -> $E46D es_handler_glyph_fade_c via rs_exit, reload 32 */
#define ESCMD_IDLE_VAL                (7) /* -> rs_exit, handler = no-op, reload = script byte */
#define ESCMD_RESET_HANDSHAKE_VAL     (8) /* -> rs_exit, sets $A172, handler = handshake, reload = script byte */
#define ESCMD_HANDSHAKE_AGAIN_VAL     (9) /* -> rs_exit, handler = handshake, reload = script byte */
#define ESCMD_DRAW_TEXT_NO_CLEAR_VAL (10) /* -> $E2F5 render_text_common, runs immediately, no backbuffer clear */
#define ESCMD_DRAW_TEXT_VAL          (11) /* -> $E2F0 es_handler_render_text, runs immediately, clears backbuffer first */
#define ESCMD_CHATTER_VAL            (12) /* -> $E2B2, runs immediately */
#define ESCMD_DRAW_SCORE_VAL         (13) /* -> $E256, runs immediately */

/* Z80 addresses that the end-screen script encodes as literal pointer words.
 * es_script below emits them; z80addrtoendshot/z80addrtochatterblk lookups
 * turn them back into the C arrays elsewhere in this file. */
#define BITMAP_ENDSHOT_1_ADDR               (0x60E1)
#define BITMAP_ENDSHOT_2_ADDR               (0x6489)
#define BITMAP_ENDSHOT_3_ADDR               (0x6831)
#define BITMAP_ENDSHOT_4_ADDR               (0x6BD9)
#define CHATTERBLK_NANCY_CONGRATULATES_ADDR (0x5C6E)
#define CHATTERBLK_PRESS_GEAR_ADDR          (0x5C78)

/* Conv: skool $E251 "LD HL,$5E04 / JR $E20D" -- on an unrecognised command
 * byte the Z80 resets HL to the CHATTER(0x5C78) command three bytes back
 * and re-enters the loop rather than returning, which is what makes
 * "PRESS GEAR TO CONTINUE" blink forever instead of a one-shot draw. */
#define ES_SCRIPT_RESET_OFFSET (sizeof(es_script) - 6)

/* Conv: skool $E052 "LD HL,$5DE3 / LD ($A16D),HL" -- the first fire press
 * jumps the script program counter to the congratulations sequence at
 * $E1E3, which is 0xE5 bytes into the script block based at $E0FE. */
#define ES_SCRIPT_CONGRATS_OFFSET (0xE1E3 - 0xE0FE)

/* ----------------------------------------------------------------------- */

// clang-format off

/* Private argument-encoding macros for es_script below; command values
 * (ESCMD_*_VAL) are shared with run_script's switch elsewhere in this file. */
#define ESCMD_CLEAR_DRAW_FRAME(BMADDR, SCRADDR) ESCMD_CLEAR_DRAW_FRAME_VAL, TWOBYTES(BMADDR), ZXSCREEN(SCRADDR)
#define ESCMD_DRAW_WORD(BMADDR, SCRADDR)        ESCMD_DRAW_WORD_VAL, TWOBYTES(BMADDR), ZXSCREEN(SCRADDR)
#define ESCMD_FADE_IN_A                         ESCMD_FADE_IN_A_VAL
#define ESCMD_FADE_IN_B                         ESCMD_FADE_IN_B_VAL
#define ESCMD_HANDSHAKE                         ESCMD_HANDSHAKE_VAL
#define ESCMD_FADE_IN_C                         ESCMD_FADE_IN_C_VAL
#define ESCMD_IDLE(D)                           ESCMD_IDLE_VAL, (D)
#define ESCMD_RESET_HANDSHAKE(D)                ESCMD_RESET_HANDSHAKE_VAL, (D)
#define ESCMD_HANDSHAKE_AGAIN(D)                ESCMD_HANDSHAKE_AGAIN_VAL, (D)
#define ESCMD_DRAW_TEXT_NO_CLEAR(ATTR, SCRADDR) ESCMD_DRAW_TEXT_NO_CLEAR_VAL, (ATTR), ZXSCREEN(SCRADDR)
#define ESCMD_DRAW_TEXT(ATTR, SCRADDR)          ESCMD_DRAW_TEXT_VAL, (ATTR), ZXSCREEN(SCRADDR)
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
 * decoded against run_script's command dispatch below -- see the inline
 * comments through the tail of the array.
 *
 * Conv: this master copy is const. show_end_screen copies it into
 *       state->bank7->es_script at entry; es_handler_draw_score patches the
 *       "GBP________ PTS" placeholder text in-place (offset 0xFD, matching
 *       $5DFB relocated) in that per-instance copy, not here, exactly as the
 *       original self-modifies its own es_script at that address but without
 *       concurrent game instances trampling each other's score text.
 */
static const u8 es_script[268] = {
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
        * ES_SCRIPT_RESET_OFFSET above) rather than stopping */
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

/* Bank 7's own copy of the 48K music engine's pattern/data tables, played by
 * es_play_music_48k et al below. Same (repeats, offset) / note-stream format
 * as CommonData.c's music_patterns/music_data, but a separate tune and a
 * separate table (relocated base $F53C, not $F0FE).
 *
 * $F53C (relocated; source $FA2B) */
static const u8 es_music_patterns[23] = {
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

/* ----------------------------------------------------------------------- */

#define ADDRTOSCREEN(addr)  z80addrtoscreen(state, addr)
#define ADDRTOATTRS(addr)   z80addrtoattrs(state, addr)
#define ADDRTOBACKBUF(addr) z80addrtobackbuf(state, addr)

#define ENDSHOT_WIDTH  (13 * 8) /* pixel width of an end-screen bitmap */
#define ENDSHOT_HEIGHT (64)     /* pixel height of an end-screen bitmap */

/* ----------------------------------------------------------------------- */

static void es_clear(chqstate_t *state);
static u16 next_screen_row(u16 addr);
static void draw_endshot(chqstate_t *state, const u8 *image, u16 screen_addr);
static const u8 *z80addrtoendshot(u16 addr);
static void es_draw_frame_common(chqstate_t *state, const u8 **script);
static void es_clear_then_draw_frame(chqstate_t *state, const u8 **script);
static void es_attribute_fade_in(chqstate_t *state);
static void es_attribute_fade_out(chqstate_t *state, u8 *flag);
static void es_handler_glyph_fade_b(chqstate_t *state);
static void es_handler_glyph_fade_c(chqstate_t *state);
static void es_handler_handshake(chqstate_t *state);
static void es_handler_handshake_advance(chqstate_t *state);
static void es_handler_idle(chqstate_t *state);
static void es_handler_draw_score(chqstate_t *state);
static const u8 *z80addrtochatterblk(u16 addr);
static void es_chatter(chqstate_t *state);
static void es_set_dispatch(chqstate_t *state,
                            void (*handler)(chqstate_t *state),
                            u8 reload);
static int ascii_to_glyph_id(int character);
static void plot_char(chqstate_t *state,
                      u8          A_char,
                      u8          D_row,
                      u8         *E_col,
                      u8          H_attr,
                      u8         *L_attr,
                      u8          A_attr);
static void render_text_common(chqstate_t *state, const u8 **script);
static void es_handler_render_text(chqstate_t *state, const u8 **script);
static void run_script(chqstate_t *state);
static void es_setup_interrupts(chqstate_t *state);
static void es_next_pattern_at_addr(chqstate_t *state, const u8 *HL_pataddr);
static void es_advance_pattern(chqstate_t *state);
static void es_reset_music(chqstate_t *state);
static void es_playdrum_go(chqstate_t *state, int D_length, u8 *HL_data);
static void es_playdrum_2(chqstate_t *state, int A_speed);
static void es_playdrum_1(chqstate_t *state, int A_speed);
static void es_play_noise(chqstate_t *state, int A_param);
static void es_play_music_48k(chqstate_t *state);

/* ----------------------------------------------------------------------- */
/**
 * $E000: Show the end screen
 *
 * Displays the end-of-game results screen: clears the playfield, starts the
 * beatbox and bank 7's own interrupt-driven music/script engine, then loops
 * driving the script/frame-advance dispatch and drive_chatter each frame
 * until the fire key is pressed twice (once to reach the congratulations
 * script, once more to exit), clearing the queued key each time.
 *
 * Conv: the Z80 entry point LDIRs itself from $E000 to $5C00, then LDIRs a
 *       second, inner 768-byte block from $F7EF to $F300 and runs from there
 *       (see project memory project-endscreen-bank7-double-relocation). Both
 *       relocations are pure ZX paging/self-modification artefacts and are
 *       discarded here, following the load_stage precedent -- the C functions
 *       below are simply called directly.
 */
void show_end_screen(chqstate_t *state)
{
  u8  A_input;     /* keyscan result, tested for the fire bit (was A) */
  int outer_count; /* per-keyscan frame divider, reloads to 5/6 (was A171) */

  es_clear(state);
  es_setup_interrupts(state);
  es_reset_music(state);

  assert(sizeof(es_script) == sizeof(state->bank7->es_script));
  memcpy(state->bank7->es_script, es_script, sizeof(es_script));
  state->bank7->es_script_ptr  = state->bank7->es_script;
  state->bank7->es_frame_count = 1;
  state->bank7->es_handler     = es_handler_idle;
  state->bank7->es_input_mask  = 0;

  outer_count = 6;

  for (;;)
  {
    CHECK_HOST_QUIT(state);

    state->speccy->stamp(state->speccy);

    es_play_music_48k(state);
    if (--state->bank7->es_frame_count == 0)
      run_script(state);
    state->bank7->es_handler(state);

    state->speccy->sleep(state->speccy, END_SCREEN_TSTATES);

    if (--outer_count != 0)
      continue;
    outer_count = 5;

    drive_chatter(state);
    A_input = keyscan(state);
    if (!(A_input & USERINPUTFLAG_FIRE))
      continue;

    if (state->bank7->es_input_mask == 0)
    {
      /* First fire press: skip ahead to the congratulations script. */
      state->bank7->es_input_mask  = 1;
      state->bank7->es_frame_count = 1; /* $E04F: run the script next frame */
      state->bank7->es_script_ptr =
          &state->bank7->es_script[ES_SCRIPT_CONGRATS_OFFSET]; /* $E052 */
      drive_chatter_stop(state);
      /* Conv: the Z80 spins here on the keyboard alone. The C port must pace
       * the wait -- an unpaced poll runs the game thread flat out and the host
       * never gets a frame -- and honour the quit request while the player is
       * still holding fire. */
      while (keyscan(state) & USERINPUTFLAG_FIRE)
      {
        CHECK_HOST_QUIT(state);

        state->speccy->stamp(state->speccy);
        state->speccy->sleep(state->speccy, END_SCREEN_TSTATES);
      }
      continue;
    }

    /* Second fire press: leave the end screen. */
    break;
  }

  drive_chatter_stop(state);
}

/**
 * $E01E: Set up bank 7's own interrupt handler and 48K music engine
 *
 * TODO: not yet ported. Bank 7 carries its own copy of the 48K sound code
 * (es_setup_interrupts onward in the skool), relocated into the copied
 * $F300 buffer.
 */
static void es_setup_interrupts(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $E20A: Run the current end-screen script
 *
 * Reads and dispatches script command bytes from state->es_script_ptr in a
 * DEC A/JP Z chain matching the ESCMD_* constants above. "Immediate" commands
 * (draw frame, call word, draw score) run their handler stub straight away
 * and loop for the next command in the same call; all other commands instead
 * arm state->es_handler/es_frame_count via es_set_dispatch and return,
 * leaving show_end_screen's per-frame loop to invoke the handler on a delay.
 *
 * ESCMD_DRAW_TEXT_NO_CLEAR_VAL/ESCMD_DRAW_TEXT_VAL (render_text/plot_char) also run immediately, drawing
 * their text run within this same call rather than arming a per-frame
 * handler -- see render_text_common and plot_char above.
 *
 * An unrecognised command byte resets HL_script to ES_SCRIPT_RESET_OFFSET
 * and loops rather than returning, matching skool $E251.
 */
static void run_script(chqstate_t *state)
{
  const u8 *HL_script; /* script program counter (was HL) */
  u8        A_cmd;     /* command byte just read (was A) */
  u8        C_reload;  /* frame-delay reload value about to be applied (was C) */

  HL_script = state->bank7->es_script_ptr;

  for (;;)
  {
    A_cmd = *HL_script++;

    /* Conv: es_chatter reads/advances state->bank7->es_script_ptr directly
     * rather than taking &HL_script like the other handlers, so it must see
     * the pointer already advanced past A_cmd before it runs. */
    state->bank7->es_script_ptr = HL_script;

    switch (A_cmd)
    {
    case ESCMD_CLEAR_DRAW_FRAME_VAL:
      es_clear_then_draw_frame(state, &HL_script);
      continue;

    case ESCMD_DRAW_WORD_VAL:
      /* $E2DE is $E2D9's tail half, entered directly for this command
       * (skipping E2D9's own backbuffer-clear prefix). */
      es_draw_frame_common(state, &HL_script);
      continue;

    case ESCMD_FADE_IN_A_VAL:
      es_set_dispatch(state, es_attribute_fade_in, 16);
      goto rs_exit;

    case ESCMD_FADE_IN_B_VAL:
      es_set_dispatch(state, es_handler_glyph_fade_b, 16);
      goto rs_exit;

    case ESCMD_HANDSHAKE_VAL:
      es_set_dispatch(state, es_handler_handshake, 16);
      goto rs_exit;

    case ESCMD_FADE_IN_C_VAL:
      es_set_dispatch(state, es_handler_glyph_fade_c, 32);
      goto rs_exit;

    case ESCMD_IDLE_VAL:
      C_reload = *HL_script++;
      es_set_dispatch(state, es_handler_idle, C_reload);
      goto rs_exit;

    case ESCMD_RESET_HANDSHAKE_VAL:
      C_reload                         = *HL_script++;
      state->bank7->es_handshake_index = 0; /* $E2C0 LD ($A172),A with A=0 */
      es_set_dispatch(state, es_handler_handshake_advance, C_reload);
      goto rs_exit;

    case ESCMD_HANDSHAKE_AGAIN_VAL:
      C_reload = *HL_script++;
      es_set_dispatch(state, es_handler_handshake_advance, C_reload);
      goto rs_exit;

    case ESCMD_DRAW_TEXT_NO_CLEAR_VAL:
      /* $E2F5 entered directly: no backbuffer-clear prefix. */
      render_text_common(state, &HL_script);
      continue;

    case ESCMD_DRAW_TEXT_VAL:
      es_handler_render_text(state, &HL_script);
      continue;

    case ESCMD_CHATTER_VAL:
      es_chatter(state);
      HL_script = state->bank7->es_script_ptr; /* es_chatter advanced it directly */
      continue;

    case ESCMD_DRAW_SCORE_VAL:
      es_handler_draw_score(state);
      continue;

default:
      /* Unrecognised command: skool $E251 resets HL to the CHATTER(0x5C78)
       * command rather than stopping -- see ES_SCRIPT_RESET_OFFSET. */
      HL_script = &state->bank7->es_script[ES_SCRIPT_RESET_OFFSET];
      continue;
    }
  }

rs_exit:
  state->bank7->es_script_ptr = HL_script;
}

/**
 * $E256: Tally the bonus into the score, then draw the final score text
 *
 * First runs a 1000-iteration bonus-tally animation: each iteration adds
 * 5,000 to the score (increment_score), redraws the scoreboard LED digits
 * (ptad_led_digits, same call as the in-game scoreboard update) and plays one
 * "bipbow" tick (sfx_bipbow), giving the classic arcade rising-score sound
 * and visual. Sets es_input_mask so the very next fire press exits the end
 * screen (see Bank7State.h). Then formats the final score as ASCII -- most
 * significant BCD byte first, blanking leading zeros -- directly into the
 * "GBP________ PTS" placeholder already sitting in state->bank7->es_script
 * (offset 0xFD, $5DFB relocated), so the later draw-text command renders it.
 *
 * Conv: the leading-zero blank/print decision (Z80: RLC C carry chain) is
 *       modelled as a sticky "seen a non-blank digit yet" flag: print the digit
 *       if it is non-zero or a digit has already been printed, otherwise print
 *       a space.
 */
static void es_handler_draw_score(chqstate_t *state)
{
  int       tally;    /* bonus-tally animation iteration counter (was BC) */
  const u8 *DE_bcd;   /* packed-BCD score pointer, walked backwards (was DE) */
  u8       *HL_dst;   /* destination ASCII bytes in es_script (was HL) */
  u8        C_seen;   /* sticky "non-blank digit already printed" flag (was C) */
  int       pair;     /* BCD byte-pair iteration counter (was B) */
  u8        A_nibble; /* nibble being converted to ASCII (was A) */

  for (tally = 1000; tally != 0; tally--)
  {
    /* Conv: the tally runs for ~11s; without this the window stays up for the
     * remainder of it after the host asks to quit. */
    CHECK_HOST_QUIT(state);

    state->speccy->stamp(state->speccy);

    increment_score(state, 0, 0x00, 0x50);
    ptad_led_digits(state, 4, &state->score_bcd[3],
                    &state->session.score_digits[7], ADDRTOSCREEN(0x4126));
    sfx_bipbow(state, 2, 2);

    /* Sleeps out the whole per-increment body timed from the stamp() above,
     * not just sfx_bipbow's delay loops (see SCORE_TALLY_TSTATES). Without
     * this the 1000-iteration tally runs in zero real time and the classic
     * rising-score animation is never seen.
     */
    state->speccy->sleep(state->speccy, SCORE_TALLY_TSTATES);
  }

  state->bank7->es_input_mask = 1;

  DE_bcd = &state->score_bcd[3];
  HL_dst = &state->bank7->es_script[0xFD];
  C_seen = 0;

  for (pair = 4; pair != 0; pair--)
  {
    A_nibble = (*DE_bcd >> 4) & 0x0F;
    if (A_nibble != 0 || C_seen != 0)
    {
      C_seen    = 0xFF;
      *HL_dst++ = '0' + A_nibble;
    } else
    {
      *HL_dst++ = ' ';
    }

    A_nibble = *DE_bcd & 0x0F;
    if (A_nibble != 0 || C_seen != 0)
    {
      C_seen    = 0xFF;
      *HL_dst++ = '0' + A_nibble;
    } else
    {
      *HL_dst++ = ' ';
    }

    DE_bcd--;
  }

  HL_dst[-1] |= EOS;
}

/**
 * Resolve a script-embedded argument word to its C data array.
 *
 * Conv: as with z80addrtoendshot, es_script only ever encodes one literal value
 *       here ($5C6E, pre-relocation for data_e06e at post-relocation $E06E via
 *       the bank's uniform +0x8400 rule), so a small lookup replaces pointer
 *       arithmetic into relocated bank memory the C port does not model
 *       byte-for-byte.
 *
 * \param[in] addr Raw address word read from the script (was HL after EX DE,HL
 *                 at $E2B7).
 * \return         Matching data block, or NULL if unrecognised.
 */
static const u8 *z80addrtochatterblk(u16 addr)
{
  // clang-format off
  /** $5C6E: chatterblk_nancy_congratulates */
  static const u8 chatterblk_nancy_congratulates[6] = {
    CHATTERCHR_NANCY,
    CHATTERSTR_THIS_IS_NANCY,
    CHATTERSTR_CONGRATS_1,
    CHATTERSTR_CONGRATS_2,
    CHATTERSTR_CONGRATS_3,
    CHATTERCMD_STOP
  };

  /** $5C78: chatterblk_press_gear */
  static const u8 chatterblk_press_gear[3] = {
    CHATTERCHR_TONY,
    CHATTERSTR_PRESS_GEAR,
    CHATTERCMD_STOP
  };
  // clang-format on

  switch (addr)
  {
  case CHATTERBLK_NANCY_CONGRATULATES_ADDR:
    return &chatterblk_nancy_congratulates[0];
  case CHATTERBLK_PRESS_GEAR_ADDR:
    return &chatterblk_press_gear[0];
default:
    assert(0);
    return NULL;
  }
}

/**
 * $E2B2: Call a script-supplied handler with a literal argument
 *
 * Reads a 2-byte little-endian pointer word from the script, advances the
 * script pointer past it, then calls start_chatter with priority 1 (Z80
 * sets A=0 then INC A immediately before CALL $9945) and the resolved
 * pointer. Runs immediately (loops back into run_script rather than
 * returning).
 *
 * Conv: data_e06e (the only live target) is NOT a standard {CHATTERCHR,
 *       CHATTERSTR, CHATTERCMD} chatterblk -- see
 *       CHATTERBLK_NANCY_CONGRATULATES_ADDR above. Byte 2 of that block ($3F =
 *       63) would be consumed as a CHATTERSTR index by pc_chatter_message
 *       (Main.c, the "assert(*chatterblk < CHATTERSTR__LIMIT)" guard around
 *       line 5871) and fail that bounds check immediately -- CHATTERSTR__LIMIT
 *       is 36. In a release build without asserts this reads
 *       common_chatter_strings[63] out of its 36-entry array and dereferences
 *       whatever garbage pointer turns up, i.e. every single playthrough would
 *       crash on reaching the end screen. The original Z80 has the same
 *       malformed data, so this path is presumed unreached in practice (the
 *       skool marks it "unproven, dead end"); rather than risk that
 *       byte-for-byte here, the start_chatter call is skipped whenever the
 *       resolved target does not look like a well-formed chatterblk.
 *
 * \param[in,out] state Pointer to game state; state->bank7->es_script_ptr is
 *                      read and advanced past the word consumed.
 */
static void es_chatter(chqstate_t *state)
{
  const u8 *HL_script_ptr; /* script read pointer (was HL) */
  u16       target_addr;   /* raw argument word read from the script (was DE/HL) */
  const u8 *chatterblk;    /* resolved data block pointer (was HL after CALL $9945 setup) */

  HL_script_ptr = state->bank7->es_script_ptr;

  target_addr    = wordat(HL_script_ptr);
  HL_script_ptr += 2;

  chatterblk = z80addrtochatterblk(target_addr);
  start_chatter(state, 1, chatterblk);

  state->bank7->es_script_ptr = HL_script_ptr;
}

/**
 * $E2CD: Set the per-frame handler and its frame-delay reload
 *
 * Common tail shared by the run_script commands that hand off to a
 * self-modified per-frame handler rather than running immediately: stores
 * the new handler and reload count, ready for show_end_screen's loop to
 * count down and re-invoke run_script when it reaches zero.
 *
 * \param[in] handler New per-frame handler (was DE).
 * \param[in] reload  New $A170 frame-delay reload count (was C).
 */
static void es_set_dispatch(chqstate_t *state,
                            void (*handler)(chqstate_t *state),
                            u8 reload)
{
  state->bank7->es_handler     = handler;
  state->bank7->es_frame_count = reload;
}

/**
 * Map an ASCII character to its glyph index in font[]
 *
 * $E328-$E356 width-class ladder. Instruction-for-instruction identical to
 * draw_char's ($9FEC, Main.c) dc_have_range/dc_have_single mapping -- see
 * that function for the equivalent structure. Reused here (rather than
 * reinvented) because both routines index the same font[41*7] table.
 *
 * \param[in] character ASCII character, already offset by ' ' (was A after SUB
 *                      $20; space and 0 are handled by the caller).
 * \return              Glyph index into font[] (multiply by 7 for the row
 *                      pointer).
 */
static int ascii_to_glyph_id(int character)
{
  int glyphid; /* glyph index accumulator (was C) */

  glyphid = 0x12;
  if (character >= ('A' - ' ')) goto have_range;
  glyphid = 0x0B;
  if (character >= ('0' - ' ')) goto have_range;
  glyphid = 0;
  character--;
  if (character == 0) goto have_single;
  glyphid++;
  character -= 7;
  if (character == 0) goto have_single;
  glyphid++;
  character--;
  if (character == 0) goto have_single;
  glyphid++;
  character -= 3;
  if (character == 0) goto have_single;
  glyphid++;
  goto have_single;

have_range:
  glyphid = character - glyphid;

have_single:
  return glyphid;
}

/**
 * $E2D8 (stub): Idle per-frame handler (no drawing)
 *
 * Does nothing; dispatched between animation beats when a script step has no
 * per-frame work to do.
 */
static void es_handler_idle(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $E2D9: Clear the backbuffer, then draw an end-screen graphic frame
 *
 * Thin wrapper: es_clear followed by es_draw_frame_common.
 *
 * \param[in,out] script Script read pointer (was HL); advanced past the two
 *                       words consumed.
 */
static void es_clear_then_draw_frame(chqstate_t *state, const u8 **script)
{
  es_clear(state);
  es_draw_frame_common(state, script);
}

/**
 * $E2DE: Read an image+destination pair and blit it
 *
 * Reads a bitmap address word and a destination screen address word from
 * the script, then blits the resolved image there. Does not clear the
 * backbuffer first; see es_clear_then_draw_frame for the variant that does.
 *
 * \param[in,out] script Script read pointer (was HL); advanced past the two
 *                       words consumed.
 */
static void es_draw_frame_common(chqstate_t *state, const u8 **script)
{
  const u8 *HL_script;   /* script read pointer (was HL) */
  u16       image_addr;  /* raw bitmap address word read from script */
  u16       screen_addr; /* destination screen address word read from script */

  HL_script = *script;

  image_addr   = wordat(HL_script);
  screen_addr  = wordat(HL_script + 2);
  HL_script   += 4;

  draw_endshot(state, z80addrtoendshot(image_addr), screen_addr);

  *script = HL_script;
}

/**
 * $E2F0: Clear the backbuffer, then render an end-screen text run
 *
 * Clears the backbuffer before handing off to render_text_common, used for
 * script commands that redraw the whole end-screen text area from scratch.
 *
 * \param[in,out] script Script read pointer (was HL); advanced as per
 *                       render_text_common.
 */
static void es_handler_render_text(chqstate_t *state, const u8 **script)
{
  es_clear(state);
  render_text_common(state, script);
}

/**
 * $E2F5: Parse and draw a script text-render command
 *
 * Reads a colour byte and a screen destination word from the script (3
 * bytes total), derives the attribute-row address exactly as draw_endshot
 * does, then plots each following script character via plot_char until the
 * EOS-terminated (top-bit-set) character has been drawn.
 *
 * \param[in,out] script Script read pointer (was HL); advanced past the 3-byte
 *                       header and the whole character run.
 */
static void render_text_common(chqstate_t *state, const u8 **script)
{
  const u8 *HL_script; /* script read pointer (was HL) */
  u8        C_attr;    /* attribute byte read from the script (was C) */
  u8        E_scr;     /* screen destination column byte (was E) */
  u8        D_scr;     /* screen destination row byte (was D) */
  u8        H_attr;    /* attribute-row address high byte (was H, $E304) */
  u8        L_attr;    /* attribute address column byte (was L, $E305) */
  u8        raw;       /* raw script byte, EOS bit intact (was (HL) at $E312) */
  u8        A_char;    /* script character byte, EOS bit masked off (was A) */

  HL_script = *script;

  C_attr = *HL_script++;
  E_scr  = *HL_script++;
  D_scr  = *HL_script++;

  H_attr = (u8) ((((D_scr >> 3) | (D_scr << 5)) & 0x03) + 0xEF);
  L_attr = E_scr;

  do
  {
    raw    = *HL_script;
    A_char = raw & (u8) ~EOS;
    plot_char(state, A_char, D_scr, &E_scr, H_attr, &L_attr, C_attr);
    HL_script++;
  } while ((raw & EOS) == 0);

  *script = HL_script;
}

/**
 * $E31F: Render one end-screen text character
 *
 * Space ($E323-$E327): advances both persistent cursors by one column and
 * draws nothing.
 *
 * Non-space ($E328-$E3A4): looks up the glyph via ascii_to_glyph_id(), then
 * blits it double-height directly to the screen in two passes (font rows
 * 0-3, then 4-6 plus a trailing blank row) -- the same "double height via
 * two separate 4- and 3-row passes with a column advance mid-glyph"
 * structure as draw_char's style==0 case, except writing to real (bank-7)
 * screen addresses rather than a flat backbuffer offset, so the mid-glyph
 * advance has to reproduce the Z80's raw row/column byte arithmetic.
 * Finally stamps the call's colour byte into both glyph-cell attributes and
 * advances the persistent cursor by one column.
 *
 * Conv: register-banking notes, resolved from the original stalled attempt. The
 *       Z80 threads three logically distinct values through nested EXX/stack
 *       shuffles ($E306, $E307/EX (SP),HL, and plot_char's own
 *       $E35B/$E39A-$E3A3 EXX pairs): the script read cursor (HL throughout the
 *       character loop -- modelled as the caller's script pointer, untouched by
 *       plot_char), the persistent column cursor (screen-dest E and attr-addr
 *       L, both threaded here as [in,out] E_col/L_attr), and this character's
 *       own draw position (screen-dest E's PRE-increment value, borrowed via a
 *       PUSH/EXX/POP shuffle at $E35B-$E35F -- modelled here as the local
 *       E_cur, read from *E_col before it is advanced). Everything else the EXX
 *       dance shuffles (Set S's stale/arbitrary BC and DE, pushed and popped
 *       purely to balance the stack) carries no live data and is correctly
 *       omitted.
 *
 * Conv: A_attr (was C, Set M) is NOT the "row count" the ($E2F5) prologue
 *       naming originally suggested. $E399's EXX switches back to the SAME
 *       physical register set read at $E2F5 -- the ladder's own use of C
 *       ($E328-$E356) is a completely different (Set S) C that plot_char's own
 *       LDI calls decrement into irrelevance and never reads back. Confirmed
 *       against es_script: the byte read here for "CONGRATULATIONS!" is
 *       attribute_BRIGHT_WHITE_OVER_BLACK -- a plausible text colour, not a row
 *       count. It survives unclobbered in Set M across the whole render_text
 *       call and is written verbatim into both glyph-cell attributes at
 *       $E399/$E3A0.
 *
 * Conv: $E37A-$E37F (E += $1F, then the pending LDI increment folds in a
 *       further +1, netting E += $20; D -= 7) is NOT the same computation as
 *       next_screen_row() -- it never checks for, or propagates, a carry out of
 *       the column byte into the row byte, unlike next_screen_row's explicit
 *       "did this cross a screen third" branch. Reproduced literally as two
 *       independent 8-bit adds rather than substituting next_screen_row, since
 *       the two are only equivalent when no such carry occurs -- true for every
 *       script-supplied text position in es_script, but not guaranteed in
 *       general.
 *
 * Conv: the attribute-row address computed from D_row (H_attr, range $EF-$F2)
 *       is resolved via ADDRTOSCREEN, not ADDRTOATTRS, matching the established
 *       precedent in draw_endshot's attribute-row loop above -- this bank-7
 *       memory range is not standard $5800-$5AFF attribute space.
 *
 * \param[in]     A_char Script character byte, EOS bit already masked off by
 *                       the caller (was A).
 * \param[in]     D_row  Screen destination row byte; constant for the whole
 *                       render_text call (was D, Set M).
 * \param[in,out] E_col  Screen destination column byte; the persistent cursor,
 *                       advanced by one per character (was E, Set M).
 * \param[in]     H_attr Attribute-row address high byte; constant for the whole
 *                       call (was H, Set M).
 * \param[in,out] L_attr Attribute address column byte; the persistent cursor,
 *                       mirrors *E_col (was L, Set M).
 * \param[in]     A_attr Attribute/colour byte read once from the script at
 *                       $E2F5 and held constant for the whole call
 *                       (was C, Set M) -- see the Conv note above.
 */
static void plot_char(chqstate_t *state,
                      u8          A_char,
                      u8          D_row,
                      u8         *E_col,
                      u8          H_attr,
                      u8         *L_attr,
                      u8          A_attr)
{
  int       character;     /* character code, offset by ' ' (was A) */
  int       glyphid;       /* glyph index into font[] (was C during the ladder) */
  const u8 *HL_font;       /* current font row pointer, walked forward (was HL) */
  u8        E_cur;         /* this character's draw column (was E, Set S) */
  u16       starting_addr; /* glyph's first screen byte, saved for the dirty-box call (Conv: added) */
  u8        D_cur;         /* current screen row byte during the blit (was D) */
  int       i;             /* pass loop index (Conv: no Z80 register) */
  int       data;          /* font byte read for the current scanline pair (was A) */
  u8        L_cur;         /* this character's attribute column (was L) */

  character = A_char - ' ';
  if (character == 0)
  {
    // Space: $E323-$E327.
    (*E_col)++;
    (*L_attr)++;
    return;
  }

  glyphid = ascii_to_glyph_id(character);
  HL_font  = &font[glyphid * 7];

  E_cur         = *E_col;
  starting_addr = (u16) (((u16) D_row << 8) | E_cur);
  (*E_col)++; // $E35D: persistent cursor advances for the NEXT character now.

  // Pass 1 ($E360-$E378): font bytes 0-3, double height.
  D_cur = D_row;
  for (i = 0; i < 4; i++)
  {
    data                                     = *HL_font++;
    *ADDRTOSCREEN(((u16) D_cur << 8) | E_cur) = (u8) data;
    D_cur++;
    *ADDRTOSCREEN(((u16) D_cur << 8) | E_cur) = (u8) data;
    if (i != 3)
      D_cur++;
  }

  // $E37A-$E381: mid-glyph row-wrap -- see Conv note in the prologue.
  E_cur = (u8) (E_cur + 0x20);
  D_cur = (u8) (D_cur - 7);

  // Pass 2 ($E382-$E398): font bytes 4-6, double height, then a blank row.
  for (i = 0; i < 3; i++)
  {
    data                                     = *HL_font++;
    *ADDRTOSCREEN(((u16) D_cur << 8) | E_cur) = (u8) data;
    D_cur++;
    *ADDRTOSCREEN(((u16) D_cur << 8) | E_cur) = (u8) data;
    D_cur++;
  }
  *ADDRTOSCREEN(((u16) D_cur << 8) | E_cur) = 0;

  update_screen(state, starting_addr, 8, 16); /* Conv: added */

  // $E399-$E3A4: stamp the call's colour into both glyph-cell attributes.
  L_cur                                                   = *L_attr;
  *ADDRTOBACKBUF(((u16) H_attr << 8) | L_cur)               = A_attr;
  *ADDRTOBACKBUF(((u16) H_attr << 8) | (u8) (L_cur + 0x20)) = A_attr;
  *L_attr = (u8) (L_cur + 1);
}

/**
 * $E3B7: Fade the $5C6C attribute band, then advance
 *
 * Fades the attribute band one step (routine_e472's shared tail, called
 * directly rather than duplicated), then runs the handshake animation-advance
 * ($E3BA, es_handler_handshake_advance).
 *
 * This is the entry point ESCMD_HANDSHAKE dispatches to; ESCMD_RESET_HANDSHAKE
 * and ESCMD_HANDSHAKE_AGAIN dispatch to es_handler_handshake_advance directly,
 * skipping this fade-b call ($5FBA vs $5FB7 in the relocated dispatch table).
 */
static void es_handler_handshake(chqstate_t *state)
{
  es_handler_glyph_fade_b(state);
  es_handler_handshake_advance(state);
}

/**
 * $E3BA: Advance the handshake animation frame
 *
 * Rotates the gate byte at $5C6D: when its old top bit was clear, the
 * animation-advance block below is skipped entirely; otherwise the 0-5
 * ping-pong frame index ($A172) advances into handshake_frames, that frame's
 * rows are LDIR'd to screen $48AC (8 bytes/row, wraparound-stepped via
 * next_screen_row), and 3 further 8-byte rows are zero-filled to pad every
 * frame out to a fixed height. Either way, finishes by stamping a fixed
 * 5-group x 8-byte decorative attribute pattern at $59AC.
 *
 * This is the entry point ESCMD_RESET_HANDSHAKE and ESCMD_HANDSHAKE_AGAIN
 * dispatch to directly ($5FBA in the relocated table), skipping the $5C6C
 * fade-b call that only the plain ESCMD_HANDSHAKE entry point runs.
 */
static void es_handler_handshake_advance(chqstate_t *state)
{
  // clang-format off
  /* Handshake animation frames: 8-byte-wide rows only, no attribute data
   * (handshake's own routine_e3b7 supplies attributes via the fixed $59AC
   * fill). Row counts vary per frame (37/35/34/32) -- sizes taken verbatim
   * from skool label boundaries. */
  /** $F381: bitmap_handshake_1 */
  static const u8 bitmap_handshake_1[296] = {
    XX______, ________, ________, ________, ________, ________, ________, ________,
    XXXX____, ________, ________, ________, ________, ________, ________, ________,
    XXXXXX__, ________, ________, ________, ________, ________, ________, _______X,
    XXXXXXXX, X_______, ________, ________, ________, ________, ________, _____XXX,
    XXXXXXXX, XXXX____, ________, ________, ________, ________, ________, ____XXXX,
    XXXXXXXX, XXXXXXX_, ________, ________, ________, ________, ________, ___XXXXX,
    XXXXXXXX, XXXXXXXX, XX______, ________, ________, ________, ________, __XXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXX___, ________, ________, ________, ________, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, ________, ________, _______X, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XX______, ________, ________, ______XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX__XX, XXXXXX__, XXXX____, ____X_XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX_XX, XXXXXX__, ___X_XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XX_XXX, XXXXXXXX, __X_X_XX, XXXXXXXX,
    _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _X__XXXX, XXXXXXXX, XX_X_XXX, XXXXXXXX,
    X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, X___XXX_, X_XXXXXX, XXXXX_XX, XXXXXXXX,
    _X_X_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ___XXX_X, ___X_XXX, XXXXXX_X, XXXXXXXX,
    X_X_X_X_, XXXXXXXX, XXXXXXXX, XXXXXXX_, __XXX_X_, ____XXXX, XXXXXXXX, XXXXXXXX,
    ___X_X_X, _X_XXXXX, XXXXXXXX, XXXXXXXX, _XXXXX__, XXX__XXX, XXXXXXXX, XXXXXXXX,
    ________, X_X_X_X_, XXXXXXXX, XXXXXXX_, __XX___X, XXXX__XX, XXXXXXXX, _XXXXXXX,
    ________, _______X, _X_XXXXX, XXXXXXXX, X_____XX, XXXXX___, XXXXXXXX, X_XXXX_X,
    ________, ________, ____X_XX, XXXXXXXX, XXXXXXXX, XXXXXX__, __XXXXXX, X_X_X_X_,
    ________, ________, _____X_X, XXXXXXXX, XXXXXXXX, XXXXX_X_, X__XXXXX, XX_X____,
    ________, ________, ______XX, XXXXXXXX, XXXXXXXX, XXXXXX_X, _X__XXX_, X_X_____,
    ________, ________, _______X, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_XX_X, ________,
    ________, ________, ______X_, _XXXXXXX, XXXXXXXX, XXXXXXXX, XX__X_X_, ________,
    ________, ________, ______X_, X_XX_X_X, XXXXXXXX, XXX_X_XX, XXXX_X__, ________,
    ________, ________, _______X, ______X_, XXXXXXXX, XXXXX__X, _XXXX___, ________,
    ________, ________, ________, __XXX__X, _XXXXXXX, XXX_X___, X_XXX___, ________,
    ________, ________, ________, _X_XXX__, X_XXXXXX, _X_X_X__, _XXX____, ________,
    ________, ________, ________, X_X_X___, _XXXX_X_, X_X_XXX_, ________, ________,
    ________, ________, ________, _X___XXX, __X_XXXX, __X_XXX_, ________, ________,
    ________, ________, ________, ____X_XX, _XXX_XXX, _X_X_XXX, ________, ________,
    ________, ________, ________, ___X_X_X, __XXXXXX, X___XXXX, ________, ________,
    ________, ________, ________, ____X_X_, _XXX_XXX, X____XX_, ________, ________,
    ________, ________, ________, _____XXX, _XXX_X_X, XX______, ________, ________,
    ________, ________, ________, ______X_, XXXX__X_, XX______, ________, ________,
    ________, ________, ________, ________, _XX____X, X_______, ________, ________,
  };

  /** $F4A9: bitmap_handshake_2 */
  static const u8 bitmap_handshake_2[280] = {
    XX______, ________, ________, ________, ________, ________, ________, _______X,
    XXXXX___, ________, ________, ________, ________, ________, ________, _____XXX,
    XXXXXXXX, ________, ________, ________, ________, ________, ________, ____XXXX,
    XXXXXXXX, XXXX____, ________, ________, ________, ________, ________, ___XXXXX,
    XXXXXXXX, XXXXXXX_, ________, ________, ________, ________, ________, __XXXXXX,
    XXXXXXXX, XXXXXXXX, XXX_____, ________, ________, ________, ________, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXX_, ________, ________, ________, _______X, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XX______, ________, ________, ______XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX__XX, XXXXXX__, XXXX____, ____X_XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX_XX, XXXXXX__, ___X_XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XX_XXX, XXXXXXXX, __X_X_XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _X__XXXX, XXXXXXXX, XX_X_XXX, XXXXXXXX,
    _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, X___XXX_, X_XXXXXX, XXXXX_XX, XXXXXXXX,
    X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ___XXX_X, ___X_XXX, XXXXXX_X, XXXXXXXX,
    _X_X_XXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, __XXX_X_, ____XXXX, XXXXXXXX, XXXXXXXX,
    X_X_X_X_, XXXXXXXX, XXXXXXXX, XXXXXXXX, _XXXXX__, XXX__XXX, XXXXXXXX, XXXXXXXX,
    ___X_X_X, _X_X_XXX, XXXXXXXX, XXXXXXX_, __XX___X, XXXX__XX, XXXXXXXX, _XXXXXXX,
    ________, X_X_X_X_, X_XXXXXX, XXXXXXXX, X_____XX, XXXXX___, XXXXXXXX, X_XXXX_X,
    ________, ________, _X_X_XXX, XXXXXXXX, XXXXXXXX, XXXXXX__, __XXXXXX, X_X_X_X_,
    ________, ________, ____X_XX, XXXXXXXX, XXXXXXXX, XXXXX_X_, X__XXXXX, XX_X____,
    ________, ________, _____XXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, _X__XXX_, X_X_____,
    ________, ________, _______X, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_XX_X, ________,
    ________, ________, ______X_, _XXXXXXX, XXXXXXXX, XXXXXXXX, XX__X_X_, ________,
    ________, ________, ______X_, X_XX_X_X, XXXXXXXX, XXX_X_XX, XXXX_X__, ________,
    ________, ________, _______X, ______X_, XXXXXXXX, XXXXX__X, _XXXX___, ________,
    ________, ________, ________, __XXX__X, _XXXXXXX, XXX_X___, X_XXX___, ________,
    ________, ________, ________, _X_XXX__, X_XXXXXX, _X_X_X__, _XXX____, ________,
    ________, ________, ________, X_X_X___, _XXXX_X_, X_X_XXX_, ________, ________,
    ________, ________, ________, _X___XXX, __X_XXXX, __X_XXX_, ________, ________,
    ________, ________, ________, ____X_XX, _XXX_XXX, _X_X_XXX, ________, ________,
    ________, ________, ________, ___X_X_X, __XXXXXX, X___XXXX, ________, ________,
    ________, ________, ________, ____X_X_, _XXX_XXX, X____XX_, ________, ________,
    ________, ________, ________, _____XXX, _XXX_X_X, XX______, ________, ________,
    ________, ________, ________, ______X_, XXXX__X_, XX______, ________, ________,
    ________, ________, ________, ________, _XX____X, X_______, ________, ________,
  };

  /** $F5C1: bitmap_handshake_3 */
  static const u8 bitmap_handshake_3[272] = {
    XX______, ________, ________, ________, ________, ________, ________, _____XXX,
    XXXXX___, ________, ________, ________, ________, ________, ________, ____XXXX,
    XXXXXXXX, X_______, ________, ________, ________, ________, ________, ___XXXXX,
    XXXXXXXX, XXXXX___, ________, ________, ________, ________, ________, __XXXXXX,
    XXXXXXXX, XXXXXXXX, XX______, ________, ________, ________, ________, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXX__, ________, ________, ________, _______X, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, X_______, ________, ________, ______XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX__XX, XXXXXX__, XXXX____, ____X_XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX_XX, XXXXXX__, ___X_XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XX_XXX, XXXXXXXX, __X_X_XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _X__XXXX, XXXXXXXX, XX_X_XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, X___XXX_, X_XXXXXX, XXXXX_XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ___XXX_X, ___X_XXX, XXXXXX_X, XXXXXXXX,
    _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, __XXX_X_, ____XXXX, XXXXXXXX, XXXXXXXX,
    X_X_X_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _XXXXX__, XXX__XXX, XXXXXXXX, XXXXXXXX,
    _X_X_X_X, _X_XXXXX, XXXXXXXX, XXXXXXX_, __XX___X, XXXX__XX, XXXXXXXX, _XXXXXXX,
    ______X_, X_X_X_X_, X_XXXXXX, XXXXXXXX, X_____XX, XXXXX___, XXXXXXXX, X_XXXX_X,
    ________, ________, _X_X_XXX, XXXXXXXX, XXXXXXXX, XXXXXX__, __XXXXXX, X_X_X_X_,
    ________, ________, ____X_XX, XXXXXXXX, XXXXXXXX, XXXXX_X_, X__XXXXX, XX_X____,
    ________, ________, _____XXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, _X__XXX_, X_X_____,
    ________, ________, _______X, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_XX_X, ________,
    ________, ________, ______X_, _XXXXXXX, XXXXXXXX, XXXXXXXX, XX__X_X_, ________,
    ________, ________, ______X_, X_XX_X_X, XXXXXXXX, XXX_X_XX, XXXX_X__, ________,
    ________, ________, _______X, ______X_, XXXXXXXX, XXXXX__X, _XXXX___, ________,
    ________, ________, ________, __XXX__X, _XXXXXXX, XXX_X___, X_XXX___, ________,
    ________, ________, ________, _X_XXX__, X_XXXXXX, _X_X_X__, _XXX____, ________,
    ________, ________, ________, X_X_X___, _XXXX_X_, X_X_XXX_, ________, ________,
    ________, ________, ________, _X___XXX, __X_XXXX, __X_XXX_, ________, ________,
    ________, ________, ________, ____X_XX, _XXX_XXX, _X_X_XXX, ________, ________,
    ________, ________, ________, ___X_X_X, __XXXXXX, X___XXXX, ________, ________,
    ________, ________, ________, ____X_X_, _XXX_XXX, X____XX_, ________, ________,
    ________, ________, ________, _____XXX, _XXX_X_X, XX______, ________, ________,
    ________, ________, ________, ______X_, XXXX__X_, XX______, ________, ________,
    ________, ________, ________, ________, _XX____X, X_______, ________, ________,
  };

  /** $F6D1: bitmap_handshake_4 */
  static const u8 bitmap_handshake_4[256] = {
    XXXXX___, ________, ________, ________, ________, ________, ________, ___XXXXX,
    XXXXXXXX, XX______, ________, ________, ________, ________, ________, __XXXXXX,
    XXXXXXXX, XXXXXX__, ________, ________, ________, ________, ________, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXX_____, ________, ________, ________, _______X, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXX_, ________, ________, ________, ______XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX__XX, XXXXXX__, XXXX____, ____X_XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX_XX, XXXXXX__, ___X_XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XX_XXX, XXXXXXXX, __X_X_XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _X__XXXX, XXXXXXXX, XX_X_XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, X___XXX_, X_XXXXXX, XXXXX_XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ___XXX_X, ___X_XXX, XXXXXX_X, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, __XXX_X_, ____XXXX, XXXXXXXX, XXXXXXXX,
    _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _XXXXX__, XXX__XXX, XXXXXXXX, XXXXXXXX,
    X_X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, __XX___X, XXXX__XX, XXXXXXXX, _XXXXXXX,
    _X_X_X_X, _XXXXXXX, XXXXXXXX, XXXXXXXX, X_____XX, XXXXX___, XXXXXXXX, X_XXXX_X,
    ____X_X_, X_X_X_X_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__, __XXXXXX, X_X_X_X_,
    ________, _____X_X, _X_X_X_X, XXXXXXXX, XXXXXXXX, XXXXX_X_, X__XXXXX, XX_X____,
    ________, ________, ____X_X_, XXXXXXXX, XXXXXXXX, XXXXXX_X, _X__XXX_, X_X_____,
    ________, ________, _______X, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_XX_X, ________,
    ________, ________, ______X_, _XXXXXXX, XXXXXXXX, XXXXXXXX, XX__X_X_, ________,
    ________, ________, ______X_, X_XX_X_X, XXXXXXXX, XXX_X_XX, XXXX_X__, ________,
    ________, ________, _______X, ______X_, XXXXXXXX, XXXXX__X, _XXXX___, ________,
    ________, ________, ________, __XXX__X, _XXXXXXX, XXX_X___, X_XXX___, ________,
    ________, ________, ________, _X_XXX__, X_XXXXXX, _X_X_X__, _XXX____, ________,
    ________, ________, ________, X_X_X___, _XXXX_X_, X_X_XXX_, ________, ________,
    ________, ________, ________, _X___XXX, __X_XXXX, __X_XXX_, ________, ________,
    ________, ________, ________, ____X_XX, _XXX_XXX, _X_X_XXX, ________, ________,
    ________, ________, ________, ___X_X_X, __XXXXXX, X___XXXX, ________, ________,
    ________, ________, ________, ____X_X_, _XXX_XXX, X____XX_, ________, ________,
    ________, ________, ________, _____XXX, _XXX_X_X, XX______, ________, ________,
    ________, ________, ________, ______X_, XXXX__X_, XX______, ________, ________,
    ________, ________, ________, ________, _XX____X, X_______, ________, ________,
  };

  /**
   * $E3A5: handshake_frames
   *
   * Row-count + source bitmap per animation frame, cycling 1-2-3-4-3-2
   * (es_handler_handshake_advance, Bank7.c).
   */
  static const handshake_frame_t handshake_frames[6] = {
    { 37, &bitmap_handshake_1[0] },
    { 35, &bitmap_handshake_2[0] },
    { 34, &bitmap_handshake_3[0] },
    { 32, &bitmap_handshake_4[0] },
    { 34, &bitmap_handshake_3[0] },
    { 35, &bitmap_handshake_2[0] },
  };
  // clang-format on

  int       carry;     /* carry flag set by RLC (carry) */
  u8        A_index;   /* frame index 0..5, wrapped (was A/B) */
  const u8 *HL_image;  /* handshake bitmap source, walked forward (was HL) */
  u16       DE_screen; /* screen destination address (was DE) */
  int       row;       /* bitmap row counter for this frame (was B) */
  int       blank;     /* blank-row counter, 3 down to 0 (was C) */
  u8       *HL_attr;   /* decorative attribute cell (was HL) */
  int       group;     /* decorative attribute group counter, 5 down to 0 (was C) */

  RLC(state->bank7->es_fade_gate_c);
  if (carry)
  {
    A_index = state->bank7->es_handshake_index;
    state->bank7->es_handshake_index = (u8) ((A_index + 1 == 6) ? 0 : A_index + 1);

    HL_image  = handshake_frames[A_index].image;
    DE_screen = 0x48AC;

    for (row = handshake_frames[A_index].rows; row != 0; row--)
    {
      memcpy(ADDRTOSCREEN(DE_screen), HL_image, 8);
      HL_image  += 8;
      DE_screen  = next_screen_row(DE_screen);
    }

    for (blank = 3; blank != 0; blank--)
    {
      memset(ADDRTOSCREEN(DE_screen), 0, 8);
      DE_screen = next_screen_row(DE_screen);
    }
  }

  HL_attr = ADDRTOATTRS(0x59AC);
  for (group = 5; group != 0; group--)
  {
    memset(HL_attr, attribute_WHITE_OVER_BLACK, 8);
    HL_attr += SCREEN_ATTRIBUTES_WIDTH;
  }

  update_attrs(state, 0x59AC, 8 * 8, 5 * 8); /* Conv: added */
}

/**
 * $E42E: Sweep attributes toward the target colours
 *
 * Gate: only runs every other call (RLC flip-flops $5C6C; returns
 * immediately when the old top bit was set). When it runs, walks all 512
 * attribute cells $5900-$5AFF against the corresponding backbuffer bytes at
 * $F000-$F1FF (the glyph shapes rasterised there by other code): cells with
 * the BRIGHT bit set are left untouched; cells whose masked colour already
 * matches the backbuffer target are copied verbatim; every other cell steps
 * its ink and paper fields one unit toward the target. Called repeatedly
 * this produces a gradual colour reveal as glyphs are plotted into the
 * backbuffer over several frames.
 *
 * Conv: the ink-field increment (`INC C`, $E45D) and paper-field increment
 *       (`ADD A,$08`, $E468) are not masked back into their 3-bit fields -- u8
 *       wraparound reproduces this bug-for-bug.
 */
static void es_attribute_fade_in(chqstate_t *state)
{
  int carry;    /* carry flag set by RLC (carry) */
  u8 *HL_attr;  /* current attribute cell (was HL) */
  u8 *DE_back;  /* current backbuffer cell (was DE) */
  int c;        /* attribute cell counter, 512 down to 0 (was H reaching $5B) */
  u8  A_target; /* masked target colour read from the backbuffer (was A) */
  u8  A_paper;  /* working accumulator (was A) */
  u8  B_target; /* masked target colour read from the backbuffer (was B) */
  u8  C_ink;    /* merged ink field (was C) */

  RLC(state->bank7->es_fade_gate_ab);
  if (carry)
    return;

  HL_attr = ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR);
  DE_back = ADDRTOBACKBUF(BACKBUFFER_START_ADDRESS);

  for (c = SCREEN_ATTRIBUTES_WIDTH * PLAYFIELD_HEIGHT / 8; c != 0; c--, HL_attr++, DE_back++)
  {
    if (*HL_attr & ATTR_BRIGHT) /* BRIGHT set: leave this cell untouched */
      continue;

    A_target = *DE_back & (ATTR_INK_MASK | ATTR_PAPER_MASK);
    if (A_target == *HL_attr)
    {
      *HL_attr = *DE_back;
      continue; // already there
    }

    B_target = A_target;

    C_ink = *HL_attr & ATTR_INK_MASK;
    if ((B_target & ATTR_INK_MASK) != C_ink)
      C_ink++;

    A_paper = *HL_attr & ATTR_PAPER_MASK;
    if ((B_target & ATTR_PAPER_MASK) != A_paper)
      A_paper = (u8) (A_paper + 0x08);

    *HL_attr = (u8) (A_paper | C_ink);
  }

  update_attrs(state, SCREEN_PLAYFIELD_ATTRS_ADDR, SCREEN_WIDTH, PLAYFIELD_HEIGHT); /* Conv: added */
}

/**
 * $E46D: Fade the $5C6D-gated glyph attribute band
 *
 * Thin wrapper: es_attribute_fade_out against es_fade_gate_c.
 */
static void es_handler_glyph_fade_c(chqstate_t *state)
{
  es_attribute_fade_out(state, &state->bank7->es_fade_gate_c);
}

/**
 * $E472: Fade the $5C6C-gated glyph attribute band
 *
 * Thin wrapper: es_attribute_fade_out against es_fade_gate_ab.
 */
static void es_handler_glyph_fade_b(chqstate_t *state)
{
  es_attribute_fade_out(state, &state->bank7->es_fade_gate_ab);
}

/**
 * $E475: Shared fade-to-black tail for routine_e472/routine_e46d
 *
 * Sweeps the same 512-cell attribute band as es_attribute_fade_in,
 * decrementing each cell's ink field by 1 (floor 0) and paper field by one
 * unit (floor 0) every call it runs. Gated by rlc8 on *flag -- $5C6C for
 * routine_e472 (GLYPH_B, also called directly by the handshake handler),
 * $5C6D for routine_e46d (GLYPH_C).
 *
 * Conv: unlike es_attribute_fade_in, BRIGHT/FLASH are never tested here -- the
 *       original ANDs each byte down to its ink/paper fields before OR-ing them
 *       back together, which drops those bits on every write. Matched
 *       bug-for-bug.
 *
 * \param[in] flag Flip-flop gate byte to rotate (was HL -> $5C6C/$5C6D).
 */
static void es_attribute_fade_out(chqstate_t *state, u8 *flag)
{
  int carry;     /* carry flag set by RLC (carry) */
  u8 *HL_pattrs; /* current attribute cell (was HL) */
  int c;         /* attribute cell counter, 512 down to 0 (was D pages) */
  u8  A_attr;    /* attribute cell (was A) */
  u8  B_ink;     /* new ink field (was B) */
  u8  A_paper;   /* working accumulator (was A) */

  RLC(*flag);
  if (!carry)
    return;

  HL_pattrs = ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR);

  for (c = SCREEN_ATTRIBUTES_WIDTH * PLAYFIELD_HEIGHT / 8; c != 0; c--, HL_pattrs++)
  {
    A_attr = *HL_pattrs;
    if (A_attr == 0)
      continue;

    B_ink = A_attr & ATTR_INK_MASK;
    if (B_ink != 0)
      B_ink--;

    A_paper = A_attr & ATTR_PAPER_MASK;
    if (A_paper != 0)
      A_paper = (u8) (A_paper - 0x08);

    *HL_pattrs = (u8) (A_paper | B_ink);
  }

  update_attrs(state, SCREEN_PLAYFIELD_ATTRS_ADDR, SCREEN_WIDTH, PLAYFIELD_HEIGHT); /* Conv: added */
}

/**
 * $E499: Clear the playfield ready for the end screen
 *
 * Zeros the on-screen playfield (attributes and bitmap) plus the first 512
 * bytes of the "backbuffer" area, which alias the attribute portion of the
 * end-screen montage/glyph drawing area ($F000 = &state->backbuffer[0]).
 */
static void es_clear(chqstate_t *state)
{
  clear_playfield(state);
  memset(&state->backbuffer[0], 0, 512);
}

/* Script command bytes, $E20D's DEC A/JP Z chain (1-based, in read order). */

/**
 * Advance a raw Z80 screen address by one character-cell row.
 *
 * Common wraparound arithmetic shared by draw_endshot ($E4A9) and
 * routine_e3b7's handshake blit ($E3E9-$E3F8, $E407-$E416): within a
 * character row D climbs through its low 3 bits (one pixel scanline per
 * call); when that wraps, E jumps on by 32 (next character row) and D drops
 * back by 8 unless E itself carried into the next screen third.
 *
 * \param[in] addr Current screen address (was DE).
 * \return         Screen address one row down.
 */
static u16 next_screen_row(u16 addr)
{
  u8 D_hi; /* screen address high byte after +1 scanline (was D via A) */
  u8 E_lo; /* screen address low byte after +32 column step (was E via A) */

  D_hi = (u8) ((addr >> 8) + 1);
  if ((D_hi & 0x07) != 0)
    return (u16) ((D_hi << 8) | (addr & 0xFF));

  E_lo = (u8) ((addr & 0xFF) + 0x20);
  if (E_lo < 0x20) /* carry out of E: stay in the next screen third */
    return (u16) ((D_hi << 8) | E_lo);
  else
    return (u16) (((D_hi - 0x08) << 8) | E_lo);
}

/**
 * $E4A9: Blit an end-game montage shot to the screen
 *
 * Copies a 13-byte-wide bitmap, 64 rows tall, from image into the screen at
 * screen_addr, then 8 rows of attribute bytes into the corresponding
 * attribute third. Row addressing mimics the Z80's raw D/E screen-address
 * increment: within a character row D climbs through its low 3 bits: when
 * that wraps, E jumps on by 32 (next character column pair... actually next
 * character row) and D drops back by 8 unless E itself carried into the next
 * screen third.
 *
 * Conv: operates on the raw 16-bit Z80 screen address (screen_addr) and calls
 *       ADDRTOSCREEN per row, rather than walking a pre-resolved C pointer, so
 *       the row-wrap arithmetic can mirror the Z80 exactly.
 *
 * \param[in] image       Bitmap+attribute source blob (was HL).
 * \param[in] screen_addr Top-left destination screen address (was DE).
 */
static void draw_endshot(chqstate_t *state, const u8 *image, u16 screen_addr)
{
  int row;            /* bitmap row counter, 64 down to 1 (was B) */
  u16 DE_screen_addr; /* current screen row address (was DE) */
  u8  D_attr;         /* attribute row address high byte (was D after RRCA x3) */
  u16 attraddr;       /* current attribute row address (was DE in the attr loop) */
  int attrrow;        /* attribute row counter, 8 down to 1 (was A) */

  DE_screen_addr = screen_addr;

  for (row = ENDSHOT_HEIGHT; row != 0; row--)
  {
    memcpy(ADDRTOSCREEN(DE_screen_addr), image, ENDSHOT_WIDTH / 8);
    image          += ENDSHOT_WIDTH / 8;
    DE_screen_addr  = next_screen_row(DE_screen_addr);
  }

  /* Conv: skool POPs DE here, restoring the original destination pushed at
   * function entry -- NOT the row loop's final DE_screen_addr. Must use the
   * screen_addr parameter, which the loop above never mutates. */
  D_attr    = (u8) (screen_addr >> 8); /* original D, pre-rotate */
  D_attr    = (u8) ((((D_attr >> 3) | (D_attr << 5)) & 0x03) + 0xEF);
  attraddr = (u16) ((D_attr << 8) | (screen_addr & 0xFF));

  for (attrrow = 8; attrrow != 0; attrrow--)
  {
    memcpy(ADDRTOBACKBUF(attraddr), image, 13);
    image += 13;
    /* Conv: the skool's LDIR ($E4D6) advances DE by 13 as a side effect of
     * the copy itself, then adds a further 19 to skip the remaining columns
     * of the 32-byte attribute row (skool comment at $E4D8: "13+19 = 32").
     * memcpy has no such side effect on attraddr, so both parts of that
     * total must be added explicitly here. */
    attraddr = (u16) (attraddr + 13 + 19);
  }

  update_screen(state, screen_addr, ENDSHOT_WIDTH, ENDSHOT_HEIGHT); /* Conv: added */
}

/**
 * Resolve a script-embedded end-shot bitmap address to its C data array.
 *
 * Conv: the original walks a real (relocated) Z80 pointer; es_script only ever
 *       encodes these four literal addresses (see the
 *       ESCMD_CLEAR_DRAW_FRAME_VAL entries above), so a small lookup replaces
 *       pointer arithmetic into relocated bank memory the C port does not model
 *       byte-for-byte.
 *
 * \param[in] addr Raw address word read from the script (was HL).
 * \return         Matching bitmap_endshot_N array.
 */
static const u8 *z80addrtoendshot(u16 addr)
{
  // clang-format off
  /* End-game montage shots: 64 bitmap rows (13 bytes each) followed by 8
   * attribute rows (13 bytes each), consumed as one contiguous blob by
   * draw_endshot ($E4A9, Bank7.c). Row/attribute counts are not exactly
   * 64*13+8*13 in every case -- sizes here are taken verbatim from the
   * skool's label boundaries, not recomputed from the nominal 104x64
   * dimensions. */
  /** $60E1: bitmap_endshot_1 */
  static const u8 bitmap_endshot_1[936] = {
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_X___, _______X, ___X_X_X, _XXXXXXX, XXXXXXXX,
    X_X_X_XX, _X_X_XXX, XXX__XX_, XXX_XXXX, _X_X__XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _X_X_X__, ________, X_X_X_XX, XXXXXXXX, XXXXXXXX,
    X__X_XX_, X_XX_XXX, ___XXXXX, ___XXXXX, _X_X__X_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX_X_, X______X, _X_X_XXX, XXXXXX_X, _XXXXXXX,
    X___XX_X, _X_XX_XX, XXXXXXXX, XXXXXXX_, X_X___X_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XXXXX, _X______, X_XXXX_X, XXXXX_X_, X_X_XXXX,
    X__XX_X_, X_X_XX__, X__XXXXX, X_XXX___, __X___X_, _X_XXXXX, XXXXXXXX, XXXXXXXX, X_X_XXXX, X_X_____, _XXX_XXX, XXXXXXXX, _X_XXXXX,
    X_XX_X_X, _X_X_XXX, _XXXXXXX, _______X, _X___X__, X_X_X_XX, XXXXXXXX, XXXXXXXX, ____X_XX, XX_X_X__, X_XXX_XX, XXXXXXXX, XXX_XXXX,
    XXX_X_X_, X_X_XX__, XXXXXX_X, _______X, _____X_X, _XXX_X_X, _X_XXXXX, XXXXXXXX, _____X_X, X_X_X___, _XXXXXXX, XXX_X_XX, XXXXXXXX,
    XX_X___X, _XXXXXXX, X__X___X, ______X_, ___X____, XXX_X_X_, X_XXXXXX, XXXXXXXX, ____XXXX, XX_X_X__, __XXXXXX, XX_X_X_X, _XXXXXXX,
    X_X___X_, X_XX_XX_, _XXX____, ______XX, ___X_X_X, _X_X_X_X, _X_XXXXX, XXXXXXXX, X__X____, _XXXX___, _X_XXXXX, XX____X_, XXXXXXXX,
    XX_X_XXX, _X_XX___, XXX__X_X, ______X_, ___XX_X_, X_XXXXXX, X_XXXXXX, XXXXXXXX, X___XXXX, __XXXX__, __X_XXXX, X___XXXX, XXXXXXXX,
    X_X_XXX_, X_XXXXXX, X_X_XXXX, ______X_, ____XX_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XX__X, XXXXXXX_, ___X_XXX, X_XXX__X, XXXXXXXX,
    XX_X_X_X, _X_X_XXX, _XXXXXXX, XX_XX__X, ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX__XX, XXXXXX__, __X_XXXX, XXXX__XX, XXXXXXXX,
    X_X_X_X_, X_X_XXXX, _XX__X_X, XXXXXX_X, ____X_XX, XXXXX_XX, XXXXXXXX, XXXXXXXX, XXX_____, XXXXXXX_, ___X_XXX, XXX____X, XXXXXXXX,
    XX_X_X_X, _X_X_XX_, XXX____X, _XXXX___, ______XX, XXXX_X_X, _XXXXXXX, XXXXXXXX, XXX___X_, XXXXXX__, __X_XXXX, XXX___XX, XXXXXXXX,
    X_X_X_X_, X_X_X_X_, XXX___X_, XXXXXX__, __X__XXX, XXX___X_, XXXXXXXX, XXXXXXXX, XX_X___X, XX_XX___, _X_XXXXX, XXXXXX_X, _XXXXXXX,
    XX_X_X_X, _X_X_XXX, X_X__XXX, XXXX__X_, __XXXXXX, X___XXXX, XXXXXXXX, XXXXXXXX, XX______, __XXX___, __X_XXXX, X_XXXXXX, XXXXXXXX,
    X_X_X_X_, X_X_XXXX, _XX_XXX_, X_X_X_X_, ___XXXXX, X_XXX_X_, XXXXXXXX, XXXXXXXX, XX______, _X_X____, _X_XXXXX, XX_X_XXX, XXX_XXXX,
    XX_X_X_X, _X_XXXXX, __X_XX__, X___X___, ____XXXX, _XX_X___, XXXXXXXX, XXXXXXXX, XX_____X, X__XX___, X_XXXXXX, XXX_X_XX, XX_XXXXX,
    X_X_X_X_, X_XXXXX_, X_XX_X__, _X______, ___XXXXX, XX___X_X, __XXXXXX, XXXXXXXX, X_______, __XX___X, _XXXXXXX, XXXXXXX_, X_X_XXXX,
    XX_X_X_X, _XXXXXXX, XX_X__X_, ____X___, __X_XXXX, XXX_____, _XXX_XXX, XXXXXXXX, X_______, __X_____, X_XXXXXX, XXXX_X_X, _X_XXXXX,
    X_X_X_X_, XXXXXXX_, X_XX____, ___XXX__, ___XXXXX, XXXX__X_, XXX_X_X_, XXXXXXXX, X_______, _X_____X, _XXXXXXX, XXXXXXX_, X_XXXXXX,
    XX_X_X_X, XXXXXXXX, _X_X____, __X_X___, __X_XXXX, XXX_X__X, _XXX_X_X, XXXXXXXX, X_______, X_____X_, XXXXXXXX, XXXXXXXX, _XXXXXXX,
    X_X_X_XX, XXXXXXXX, _X_X____, ___X____, ___XXXXX, XXXX____, __X___X_, XXXXXXXX, X_______, _X_____X, _XXXXXXX, XXXXXXXX, X_XXXXXX,
    XX_X_XXX, XXXXXXXX, XX_X____, ________, __XXXXXX, XXXXX___, _______X, XXXXXXXX, X_______, X_____X_, XXXXXXXX, XXXXXXXX, _XXXXXXX,
    X_X_XXXX, XXXXXXXX, X_XX____, ________, ___XXXXX, XXXX_X__, ______X_, XXXXXXXX, X______X, _____X_X, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XX_XXXXX, XXXXXXXX, X_XX____, ________, __XXXXXX, XXX_X___, _______X, XXXXXXXX, X_______, X_____XX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    X_XXXXXX, XXXXXXXX, _XXX____, ________, ___XXXXX, XXXX_X__, ______X_, XXXXXXXX, X______X, _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXX____, ________, __X_XXXX, X_X_X___, _____X_X, XXXXXXXX, X_____X_, ____X_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXX____, ________, ___XXXXX, XX_X__X_, ____X_XX, XXXXXXXX, X____X_X, _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXX___, ________, __X_XXXX, X_X_XX_X, _____XXX, XXXXXXXX, X_______, ____X_XX, XXX_XXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX_X____, ________, _X_XXXX_, XX_X_XX_, ____X_XX, XXXXXXXX, X____X__, ___X_XXX, _X_X_XXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXX_, X_X_X___, ____X___, __XXXXXX, _XX_XX_X, ___X_XXX, XXXXXXXX, XX____XX, _X__XXXX, XXX_XXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXX_X_X, _X_X____, ___X____, _XXXXXX_, XXXX_XX_, X_X_XXXX, XXXXXXXX, XX____XX, X_XXXXXX, XXXX_XXX, XX_XXXXX, XXXXXXXX,
    XXXXXXXX, XXX_X_X_, X_X_X___, ________, X_XXXXXX, _XXXXXXX, _X_XXXXX, XXXXXXXX, XX_____X, _XXXXXXX, XXX_XXXX, X_X_XXXX, XXXXXXXX,
    XXXXXXXX, XX_X_X_X, _X_X____, ___X_X_X, _XX_X_X_, XXXX_XXX, XXXXXXXX, XXXXXXXX, XX____X_, ____X_XX, XXXXXXXX, X__X_XXX, XXXXXXXX,
    XXXXX_X_, X_XXXXXX, X_X_XX__, __X___XX, XX_XXX_X, XXXXXX_X, XXXXXXXX, XXXXXXXX, XX______, _____X_X, XXXXXXXX, XX__X_XX, XXXXXXXX,
    XXXX_X_X, XXXXXXXX, _XXXXX__, _____XX_, _XXXXXXX, XXX_X_XX, _XXXXXXX, XXXXXXXX, XX____X_, ______XX, XXXXXXXX, XX_X_XXX, XXXXXXXX,
    XXXXXXX_, XXXXXXX_, XX_X_X__, ___XXXXX, XXXXXXXX, XX_X_X_X, X_XXXXXX, XXXXXXXX, XXX__X__, _____XXX, XXXXXXXX, XXX_X_XX, XXXXXXXX,
    XXXXXXXX, _XXXXXXX, X_X_XX__, __XXXXXX, X_XXXXXX, XXX_X_X_, X_XXXXXX, XXXXXXXX, XXX_____, __X___XX, XXXXXXXX, XXXX_XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, _XXXXX__, __X_XXXX, XX_XXXXX, XXXXXX_X, XX_XXXXX, XXXXXXXX, XXX__X__, _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, _XXXXXXX, XXXXXX__, _X_XXXXX, X_XXXXXX, XXXXXXX_, XXXXXXXX, XXXXXXXX, XXX_____, __X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, _XXXXX__, _XXX_XXX, XX_XXXXX, XXXXXXXX, _XX_XXXX, XXXXXXXX, XXX_____, _XXX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXX_, XXXXXXX_, _X_XXXXX, __X__XXX, XXXXXXX_, X_XXXXXX, XXXXXXXX, XXX_____, X______X, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXX_, __XXX___, ________, XXXXXXXX, _XXXXXXX, XXXXXXXX, XXXX____, ______X_, X_XXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXX_, XXXXXXX_, ________, _______X, _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXX____, ____X__X, _X_X_XXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXX_, ________, ____X_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX__X_, __X_XXXX, XXX_X_XX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, _______X, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX_X_, _X_XXXX_, __XXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, X_XXX_X_, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXX__X, X___X___, _X_XXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, X_______, _X__XX_X, _X_XXXXX, XXXXXXXX, X_XXXXXX, XXXXXX_X, ________, X_XXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, X_______, ___X__X_, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, ________, _XXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, X_______, _______X, _X_XXXXX, XXXXXXXX, X_XXXXXX, XXXXXX__, ________, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XX______, ________, X_XXXXXX, XXXXXXXX, _XXXXXXX, XXXXXX__, _______X, _XXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XX______, ________, _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, ____X_X_, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____, ________, X_XXXXXX, XXXXXXXX, _XXXXXXX, XXXXXXX_, _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____, ________, _X_XXXXX, XXXXXXX_, XXXXXXXX, XXXXXXX_, ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____, ________, X_XXXXXX, XXXX_X_X, XXXXXXXX, XXXXXXX_, ___X_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____, _______X, _XXXXXXX, XXX_XXXX, XXXXXXXX, XXXXXXXX, __X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___, ________, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _____X_X, _XXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___, _____X_X, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, X_XXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_XX_, ____X_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, ___XXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_______, __XXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX__XX, XXX___X_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_______, _X_XXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX___XX, XXX_____, _X_X_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X___X_X_, X_XXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX___X_X, _X_XXXXX, XXXXXXXX, XXXXXXXX,
    _XX_X___, _XX_X___, _XXX____, _XXXX___, __XX____, __XX____, __XX____, _XX_X___, __XXX___, _XXXX___, __X_X___, __X_X___, __X_X___,
    _XX_X___, _XX_X___, _XXXX___, _XXXX___, _XXX____, __XX____, __XX____, _XX_X___, _XXXX___, __XXX___, __X_X___, __X_X___, __X_X___,
    _XX_X___, _XX_X___, _XXXX___, _XXXX___, _XXX____, __XX____, __XX____, _XX_X___, _XXXX___, __XXX___, __X_X___, __X_X___, __X_X___,
    _XX_X___, _XX_X___, _XXXX___, _XXXX___, _XXX____, __XX____, __XX____, _XX_X___, _XXXX___, __XXX___, __X_X___, __X_X___, __X_X___,
    _XX_X___, _XX_X___, _XXXX___, _XXXX___, _XXX____, __XX____, __XX____, _XX_X___, _XXXX___, __XXX___, __X_X___, __X_X___, __X_X___,
    _XX_X___, _XX_X___, _XX_X___, _XXX____, _XXXX___, __XX____, __XX____, _XX_X___, _XXXX___, __XXX___, __X_X___, _XX_X___, _XX_X___,
    _XX_X___, _XX_X___, _XX_X___, _XXX____, _XXXX___, __XX____, _XXX____, _XX_X___, __XXX___, _XXXX___, __X_X___, _XX_X___, _XX_X___,
    _XX_X___, _XX_X___, _XX_X___, _XXX____, _XXX____, __XX____, _XXX____, _XX_X___, _XX_X___, _XXXX___, __X_X___, _XX_X___, _XX_X___,
  };

  /** $6489: bitmap_endshot_2 */
  static const u8 bitmap_endshot_2[936] = {
    XXXXXXXX, XXX_X_X_, X_X_____, X_X_____, ________, ________, ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _X______, ___X____,
    XXXXXXXX, XX_X_X_X, _X______, _X_X____, ________, ________, ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_____, X____X_X,
    XXXXXXXX, XXX_XXXX, X_X_X___, X_XX____, ________, ________, ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX____X_, ____X_X_,
    XXXXXXXX, XX_XXXX_, XXXX____, _XX_X___, ________, ________, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_X___, _X_X_X_X,
    XXXXXXXX, XXXXX___, __XXX___, X_______, ________, ________, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, X_X_XXXX, XX____X_, X_X_X_X_,
    XXXXXXXX, XX_X_X__, _X_XXX_X, _X______, ________, ________, __XXXXXX, XXXXXXXX, XXXXXX_X, XXXXX_X_, XXXX_XXX, X_X__X_X, _X_X_X_X,
    XXXXXXXX, XXX_XXXX, X_XXX_X_, X_______, ________, ________, __XXXXXX, XXXXXXXX, XXXXXXX_, XXXXXX_X, __X_XXXX, XX__X_X_, X_X_X_X_,
    XXXXXXXX, XX_XXX_X, XXXXXX__, X__X____, ________, ________, X_XXXXXX, XXXXXXXX, XXXXXX_X, XXXXX_XX, ___X_XXX, X__X_X_X, _X_X_X_X,
    XXXXXXXX, XXX_X_X_, __XXXXX_, _XXXX___, ________, ________, _XXXXXXX, XXXXXXXX, XXXXXXXX, _XXX___X, ___X__XX, XX__X_X_, X_X_X_X_,
    XXXXXXXX, XX_X_X_X, XX_XXX__, ____X___, ________, ________, _XXXXXXX, XXXXXXXX, XX_X_X__, XXXX__X_, ______XX, X__X_X_X, _X_X_X_X,
    XXXXXXXX, XXX_X_X_, X_X_X_X_, ________, ________, ______X_, XXXXXXXX, XXXXXXXX, X_X_XXXX, X_X___X_, _______X, ____X_X_, X_X_X_X_,
    XXXXXXXX, XXXX_X_X, _X_X_X__, _______X, _______X, ____X___, XXXXXXXX, XXXXXXXX, XX_XXX_X, _X___X__, _______X, X__X_X_X, _X_X_X_X,
    XXXXXXXX, XXX_X_X_, __X_X_X_, ________, __X_____, __X____X, _XXXXXXX, XXXXXXXX, XXX_X_X_, X_______, _______X, __X_X_X_, X_X_X_X_,
    XXXXXXXX, XXXX_X_X, __XX_X_X, ________, X____X__, X_____X_, XXXXXXXX, XXXXXXXX, XX_XXX_X, _X______, ________, _____X_X, _X_X_X_X,
    XXXXXXXX, XXXXX_X_, _XX_X_X_, _____X_X, _X_X____, ___X_X_X, _XXXXXXX, XXXXXXXX, X_X_XXXX, XXXX____, _______X, ___X____, X_X_X_X_,
    XXXXXXXX, XXXXXX_X, _XXXXX_X, _X______, X_X_X_X_, X_X_X_X_, XXXXXXXX, XXXXXXXX, XX_XXXXX, _XXXXX__, _____XXX, ______X_, ___X_X_X,
    _XXXXXXX, XXXXX_X_, XXXXXXXX, XXX___X_, ___X_X_X, _X_X_X_X, XXXXXXXX, XXXXXXXX, X_X_X_X_, X_XXXXXX, ___XXXXX, ________, _X______,
    _XXXXXXX, XXXXXX_X, XX_XXXXX, _X______, _X____X_, X_X_X_XX, XXXXXXXX, XXXXXXXX, _X_XXX_X, XXXXXXX_, __XXXXXX, ________, _____X__,
    _XXXXXXX, XXXXXXX_, XXX_X_X_, __X_____, ___X____, _X_X_X_X, _XXXXXXX, XXXXXXXX, X_X_XXXX, __XX_XX_, __X__XX_, ________, ___X___X,
    _XXXXXXX, XXXXXX_X, X_X_____, ________, ________, X_X_X_X_, XXXXXXXX, XX_XXXXX, _X_X_XXX, ______XX, _______X, ________, _____X__,
    _XXXXXXX, XXXXX_XX, _X_XX___, __X_____, ________, ___X_X_X, XXXXXXX_, XXXXXXXX, X_X_X__X, XX____XX, ____X_X_, ________, _______X,
    __XXXXXX, XXXXXX_X, XXXXXXXX, X_______, ________, ______XX, XXXXXXXX, _X_XXXXX, _X_X____, ______XX, ________, ________, ______X_,
    __XXXXXX, XXXXX_X_, XXXX_XXX, XX______, ________, _X_X_X_X, XXXXXXXX, XXX_XXXX, X_X_____, ______XX, X_______, ________, ________,
    __XXXXXX, XXXXXX_X, __X_____, ________, ________, ______XX, XXXXXXXX, _XXXXX_X, _X_X____, _____XXX, X_______, ________, _______X,
    __XXXXXX, XXXXX_X_, _X_X____, ________, ________, __X__XXX, XXXXXXXX, X_X_XXX_, X_X_X___, _____XXX, X_______, X_______, ________,
    __XXXXXX, XXXXXX_X, __XXX___, _X______, ________, X___XXXX, XXXXXXXX, XX_X_X_X, _X_X____, _____XXX, X_______, X_______, _______X,
    _XXXXXXX, XXXXX_X_, X__X_XXX, X_______, ________, _____X_X, XXXXXXXX, XXX_X_X_, X_X_X___, _____XXX, XX______, X_______, ________,
    XXXXXXXX, XXXXXX_X, ____X_X_, ________, ________, X_X__XXX, XXXXXXXX, XXXX_X_X, _X_X_X__, ____XXXX, XX______, X_______, ________,
    XX_XXXXX, XXXXXXX_, X_______, ________, ________, ____X_XX, XXXXXXXX, XX_XXXXX, X_X_X___, ____XXXX, XX______, X_______, ______X_,
    XX_XXXXX, XXXXXX_X, _X______, ________, X______X, _____XXX, XXXXXXXX, XXX_XX_X, _X_X_X__, ____XXXX, XXXX____, X_______, _______X,
    XX__XXXX, XXXXXXXX, X_X_X___, ________, ____X___, __X_XXXX, XXXXXXXX, XX_XXXXX, X_X_X_X_, _____XXX, XXX_____, X_______, ________,
    XX___XXX, XXXXXXXX, XXXX_X_X, _XX___X_, _X______, X____XXX, XXXXXXXX, XXX_XXXX, XX_X_X__, ____X_XX, _X______, ________, ______X_,
    XXX____X, XXXXXXXX, XXXXXXXX, X__XX___, ___X_X_X, _X_XXXXX, XXXXXXXX, XX_X_XXX, X_X_X_X_, ___XXXXX, XXXX____, ________, ________,
    XXX_____, _XXXXXXX, XXXXX__X, XX__XXX_, X_X_X_X_, X_X_XXXX, XXXXXXXX, XXX_XXXX, XX_X_X__, __XXXXXX, XXXXX___, _X_X____, _______X,
    XXX_____, ___XXXXX, XXXX___X, X____X_X, _X_X_X_X, _X_XXXXX, XXXXXXXX, XX_X_XXX, X_X_X_X_, _XXXXX_X, _X_XXX__, X_X_X_X_, ____X_X_,
    XXXX____, ______XX, XX_____X, ____XX__, __X_X_X_, X_X_X_XX, XXXXXXXX, XXX_XXXX, XX_X_X_X, _XX_X_XX, XXX__X__, _X_X_X_X, _X_X_X_X,
    _XXX____, _____XX_, _X_____X, X____X__, _____X_X, _X_X_XXX, XXXXXXXX, XXXX_XXX, XXX_X_X_, ___XXXXX, _XXX___X, X_X_X_X_, X_X_X_X_,
    _XXXX___, _____XXX, XX____XX, _____X__, ________, X_X_X_XX, XXXXXXXX, XXX_X_X_, XX_X_X_X, __X_X___, __X____X, _X_X_X_X, _X_X_X_X,
    _XX_X___, _____XXX, XX____X_, X_____X_, ________, _X_X_X_X, _XXXXXXX, XXXX_XXX, XXX_X_X_, ________, _______X, X_X_X_X_, X_XXXXXX,
    _XXX_X__, _____XXX, XX____XX, ______X_, ________, X_X_X_X_, X_XXXXXX, XXX_X_X_, XXXX_X_X, ___XXX__, ___X__XX, XX_X_XXX, _XXXXXXX,
    _XX_____, _____XXX, XX___XX_, X_____X_, ______X_, X__X_X_X, _XXXXXXX, XXXX_XXX, XXXXX_X_, X_XXXXXX, _XX___XX, XXXXXXXX, XXXXXXXX,
    XXXX____, ______X_, XX___XXX, ______X_, ________, X__XX_XX, XXXXXXXX, XXX_X_X_, XXXX_X_X, _X_XXXXX, XX____XX, XXXXXXXX, XXXXXXXX,
    _XX_____, X_____XX, XX____XX, X_____X_, _____X_X, __XXXXXX, XXXX_XXX, XXXX_X_X, _XXXX_X_, X_X_X_X_, ______XX, XXXXXXXX, XXXXXXXX,
    XXXX____, _X_____X, X_____XX, ____X___, ______XX, XXXXXXXX, XXX_XXXX, XX_XX_X_, XXXXXX_X, _X_X_X__, _____XXX, XXXXXXXX, XXXXXXXX,
    _XX_____, ______XX, X_____XX, X____X_X, ___XXXXX, XXXXXXXX, XXX__XXX, X_XX_X_X, _XXXXXX_, X_X_X_X_, _____XX_, _____XXX, XXXXXXXX,
    XXX_____, ______XX, X_____XX, ______X_, __XXXXXX, XXXXXXXX, XX__X_XX, ___XX_X_, X_XXXXXX, _X_X_X_X, ____XX__, X_XX____, __XXXXXX,
    _XX_____, _____XXX, X_____XX, X______X, _XXXXXXX, XXXXXXXX, XX_X_XX_, ___X_X_X, _XXXXXXX, XXX_X_X_, ___XXXX_, XXX__XXX, X_____XX,
    XX______, _____XXX, X_____XX, X_____X_, XXXXXXXX, XXXXXXXX, XX___XXX, __XXX_X_, X_XXXXXX, XXXXXXXX, XXXXXX_X, _X___XXX, XX______,
    XXX_____, _____XXX, XX____XX, X_______, XXXXXXXX, XXXXXXXX, XXX___X_, __XXXX_X, _X_X_XXX, XXXXXXXX, XXXXXXX_, X___XXXX, X__X_X__,
    XX____X_, _____XXX, XX____XX, X______X, XXXXXXXX, XXXXXXXX, XXX___XX, _XXXX_X_, X_X_X_X_, XXXXXXXX, XXXXXXXX, _X_X_X_X, _X__X_X_,
    XXX____X, _____XXX, XX_____X, XX_____X, XXXXXXXX, XXXXXXXX, XXX___X_, __XXXX_X, _X_X_X_X, XXXXXXXX, XXXXXXX_, X___X___, XX___X_X,
    XX______, X____XXX, XX_____X, XX____XX, XXXXXXXX, XXXXXXXX, XXXX_XXX, ___XX_X_, X_X_XXXX, XXXXXXXX, X_X_XXXX, ___X___X, XX__X_X_,
    XX______, _X___XXX, XX_____X, XX____XX, XXXXXXXX, XXXXXXXX, XXXX_XXX, X_XXXX_X, _X_XXXXX, X__XXX_X, _X_XXXX_, X_X_X_X_, X__X_X_X,
    XX______, X____XXX, XX_____X, XX____XX, XXXXXXXX, XXXXXXXX, _XXX_XXX, ___XX_X_, X_XXXX_X, _X_X_XX_, X_X_XXXX, _X_X_X_X, X___X_X_,
    XX______, _X__X_XX, X______X, XX___XXX, XXXXXXXX, XXXXXXXX, XXX_XXXX, X___XX_X, _X_XX_X_, X_X_X_XX, _X_X_XX_, X_X_X_X_, X__X_X_X,
    X_______, __XX_XXX, XX_____X, XX___XXX, XXXXXXXX, XXXXXXXX, _XX_XXXX, XX_XXXX_, X_XX_X_X, _______X, X_X_XXXX, _X_X_X_X, __X_X_X_,
    XX_X_X_X, XXX_XXXX, X______X, XX__XXXX, XXXXXXXX, XXXXXXXX, X_XXXXXX, X___XX_X, _XXX__X_, X_______, XX_X_XXX, X_X_X_XX, ___X_X__,
    XXXXXXXX, X_XXXXXX, _X_____X, XX__XXXX, XXXXXXXX, XXXXXXXX, X__XXXXX, XX___XX_, X_XX___X, ____X_X_, XXX_X_XX, _X_X_X_X, __X_X___,
    _X_X_X_X, XXXXXXXX, XX_____X, XX_XXXXX, XXXXXXXX, XXXXXXXX, XX_XXXXX, XX___X_X, _XXX__X_, ___XXXXX, XX_X_XXX, X_X_X_XX, _X_X_X__,
    XXXXXXXX, XXXXXXXX, _X_____X, XX_XXXXX, XXXXXXXX, XXXXXXXX, X___XXXX, XXX_XXX_, X_X____X, __X_XXXX, XXX_X_XX, _X_X_X_X, X_X_X___,
    XXXXXXXX, XXXXXXX_, XX_____X, XX_XXXXX, XXXXXXXX, XXXXXXXX, XX___XXX, XXX__XXX, _XX___X_, ___XXXXX, XXXXXX_X, X_X_X_XX, _X_X_X__,
    XXXXXXXX, XXXX_X_X, _X_____X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX__XXXX, XXXX__X_, X_X_____, __XXXXXX, XXXXXXX_, XX_X_XX_, X_X_X__X,
    XXXXXXXX, XX__XXX_, XX_____X, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X__XXX, XXXXX_XX, _XX_____, ___XXXXX, XXXXXXXX, X_X_X_XX, _X_X_X__,
    XXXXXXX_, ___X_X__, _X_____X, XXXXXXXX, XXXXXXXX, XXXXXXXX, _X__XXXX, XXXX__XX, X_X_____, __XXXXXX, XXXXXXX_, XX_X_XXX, X_X_X__X,
    _XX_X___, __X_X___, __X_X___, _XX_X___, _XX_X___, _XX_X___, _XX_X___, _XX_X___, __XX____, _XXX____, _XXX____, _XX_X___, _XX_X___,
    _XX_X___, __X_X___, __X_X___, _XX_X___, _XX_X___, _XX_X___, _XX_X___, __XX____, __XX____, _XXX____, _XXX____, _XX_X___, _XX_X___,
    _XX_X___, __X_X___, __X_X___, _XX_X___, _XX_X___, _XX_X___, _XX_X___, __XX____, __XX____, __XX____, _XXX____, _XX_XXX_, _XX_X___,
    _XX_X___, __X_X___, __X_X___, _XX_X___, _XX_X___, _XX_X___, _XX_X___, __XX____, __XX____, __XX____, _XXX____, _XX_XXX_, _XX_X___,
    _XXXX___, _XXXX___, _XXXX___, __X_X___, __X_X___, _XX_X___, _XX_X___, __XX____, __XX____, __XX____, _XXX____, _XX_X___, _XX_X___,
    _XXXX___, _XXXX___, _XXXX___, __X_X___, __X_X___, _XX_X___, _XX_X___, __XX____, __XX____, _XXX____, _XXX____, __XXX___, __XXX___,
    _XXXX___, _XXXX___, _XXXX___, __X_X___, _XX_X___, _XX_X___, _XX_X___, __XX____, __XX____, __XXX___, __XXX___, __XXX___, __XXX___,
    _XXXX___, _XXXX___, _XXXX___, __X_X___, _XX_X___, _XX_X___, _XX_X___, __XX____, __XXX___, __XXX___, __XXX___, __XXX___, __XXX___,
  };

  /** $6831: bitmap_endshot_3 */
  static const u8 bitmap_endshot_3[936] = {
    ________, ________, ________, ________, ________, ________, ________, ________, ________, ________, ________, ________, ________,
    ________, ________, ________, ________, ________, ________, ________, ________, ________, ________, ________, ________, ________,
    ________, ________, ________, ________, ________, ________, ________, ________, ________, ________, ________, ________, ________,
    ________, ________, ________, ________, ________, ________, ________, ________, ________, ________, ________, ________, ________,
    ________, ________, ________, ________, ________, _____X__, ________, ________, ________, ________, ________, ________, ________,
    ________, ________, ________, ________, ________, ____X__X, _X_XXXX_, ________, ________, ________, ________, ________, ________,
    ________, ________, ________, ________, ________, ________, X_X_X_XX, XXXXX___, ________, ________, ________, ________, ________,
    ________, ________, ________, ________, ________, ________, _____X_X, _XXXXXXX, XX______, ________, ________, ________, ________,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, X_X_X_XX, XXXXXX__, ________, ________, ________, ________,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, _____X_X, _X_XXXXX, ________, ________, ________, ________,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, ________, __X_X_XX, X_______, ________, ________, ________,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, ________, _____X_X, XX______, ________, ________, ________,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, ________, ______X_, XXX_____, ________, ________, ________,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, ________, _______X, _XX_____, ________, ________, ________,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, ________, ________, X_XX____, ________, ________, ________,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, ________, ________, _XXX____, ________, ________, ________,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, __X_X___, __X_____, __X_____, X_XXX___, ________, ________, ________,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _X_X_X_X, XXX_____, __XX____, _X_XX___, ________, ________, ________,
    XXXXXXXX, XXXX_X_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, __X_XXXX, XXX_____, _XXX____, __X_XX__, ________, ________, ________,
    XXXXXXXX, XXXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _____X_X, _X______, _XXXX___, _X_XXX__, ________, ________, ____X_X_,
    XXXXXXXX, XXX_XX_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, ________, ____X___, __X_XXX_, _____X_X, ________, _X_X_X_X,
    XXXXXXXX, XXXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, ________, ____X___, ___X_XX_, __X_X_X_, X_X___X_, X_X_X_X_,
    XXXXXXXX, XXXX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________, ____XXXX, XXX_____, __X_X_XX, _X_X_X_X, _X_X_X_X, _X_XXXXX,
    XXXXXXXX, X_XXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____, ________, _____X_X, _X_XXXXX, X_______, __XXXXXX, X_X_X_XX, XXXXXXXX,
    XXXXXXX_, XXXX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_______, __XXXXXX, XXXXXX_X, _X_X_X__, ________, __XXXXXX, XXXXXXXX, ________,
    XXXXXXXX, X_X_X_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_______, ___XXXXX, XXXXXXXX, X_X_X_X_, X_X_X_X_, _______X, XXXXXXXX, ________,
    XXXXXXXX, _X_X_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX______, ___XX_XX, XXXXXXXX, XX_X_X_X, _X_X_X_X, _X_X____, XXXXXXXX, ________,
    XXXXXX_X, X_X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____, __XXX_XX, XXXXXXXX, XXXXXXX_, X_X_X___, __XXXX__, XXXXXXXX, ________,
    XXXXXXXX, _X_X_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _XXX_XXX, XXXXX_X_, X_XXXXXX, XXXX___X, _X_X_XX_, _XXXXXXX, ________,
    XXXXXXX_, X___XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_X_XX, XXXXXXXX, XX_X_X_X, _XXXXX__, ____X_X_, ___XXXXX, ________,
    XXXX_XXX, ___X_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, _X_XXXXX, XXXXX_X_, X_X_X___, ______XX, ______XX, ________,
    XXXXXXX_, X___X_XX, XXXXXXXX, XXXXXXXX, XXXXXX__, _XXXXXXX, XXXXXXXX, X_X_X_X_, XXXXXXXX, XX_XX___, _______X, X______X, ________,
    X_XXXX_X, _____XXX, XXXXXXXX, XXXXXXXX, XXXXXX__, __XX_XXX, XXXXXXXX, XXXXXX_X, _X_X_XXX, XXXXXX__, _______X, ________, X_______,
    XXX_X_X_, ____X_XX, XXXXXXXX, XXXXXXXX, XXXXXX__, __XX____, _XXXXXXX, XXXXXXXX, XXX_X_X_, X_X_X_XX, X_____XX, X_X_____, __X_____,
    XXX____X, _____XXX, XXXXXXXX, XXXXXXXX, XXXXX_X_, XXX_____, _____XXX, XXXXXXXX, XXXXXX_X, _X_X_X_X, XXXXXXXX, _XX__XX_, X_______,
    XX______, ____X_XX, XXXXXXXX, XXXXXXXX, XXXXXX_X, _XX_____, ________, _XXX___X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX__XXXX, _X______,
    X_XX____, _____XXX, XXXXXX_X, XXXXXXXX, XXXXXXXX, XXXXXX__, ________, __XX____, ___XXXXX, XXXXXXXX, XXXXXXXX, XX__XXXX, X__X____,
    XXXXX_X_, ____XXXX, XXXXXX_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____, _XXX_X_X, _______X, X___XXXX, XXXXXXXX, X__XXXXX, _X______,
    XXXXXX_X, _X_X_XXX, XXXXXX_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, _XX___X_, X_X_X__X, X_______, __XXXXXX, XXXXXXXX, X_X_____,
    XXXXXXX_, X_X_XXXX, XXXXXX_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX____X, _X_X__XX, ________, ______XX, __XXXXXX, _X__X___,
    XXXXXXXX, XX_X_XXX, XXXXX_XX, _X_X_X_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___, ______XX, ________, _____XXX, __XXXXXX, X_X_____,
    XXXXXXXX, XXXXXXXX, XXXXX_XX, XXX_X_X_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX__XX, XXXXX___, _____XX_, _X_XXXXX, _X___X__,
    XXXXXXXX, XXXXXXXX, XXXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX___X_, __XXXXXX, X__X____,
    XXXXXXXX, XXXXXXXX, XXXXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XXXXX, _X___X_X,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_X___,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _X_X_X_X,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_X_X_,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _X_X_X_X,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, X_X_X_X_,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _X_X__X_, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, _X_X_X_X,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_X___, ____X_X_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, X_X_X_X_,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_X__, ________, ____XXXX, XXXXXXXX, XX_XXXXX, XXXXXX_X, _X_X_X_X,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, ________, ___X_X__, XXXXXXX_, X_XXXXXX, XXXXX_X_, X_X_X_X_,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX______, ____X___, XXXXXX__, ___XXXXX, XXXX_X_X, _X_X_X_X,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, _X_X_X_X, XXXXXX__, ___XXXXX, XXXXX_X_, X_X_X_X_,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX_X_, XXXXXX_X, _X_XXXXX, XXXX_X_X, _X_X_X_X,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, X_XXXXXX, XXXXX_X_, X_X_X_X_,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, _XXXXXXX, XXXX_X_X, _X_X_X_X,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_X_X_, X_X_X_X_,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_X_X, _X_X_X_X,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, X_X_X_X_,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_X_X,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    _X___XX_, _X___XX_, _X___XX_, _X___XX_, _____XX_, _____XX_, _____XX_, _____XX_, _____XX_, _X___XX_, _X___XX_, _X___XX_, _X___XX_,
    _X_X____, _X_X____, _X_X____, _XX_X___, _XX_X___, _XX_X___, _X___XXX, _____XX_, _____XX_, _X___XX_, _X___XXX, _X___XXX, _X___XXX,
    _X_X____, _X_X____, _X_X____, _XX_X___, _XX_X___, _XX_X___, _X____X_, _X____X_, _X____X_, _X___XX_, _X____X_, _X____X_, _X____X_,
    _X_X____, _X_X____, _X_X____, _XX_X___, _X_XX___, _XX_X___, _XX_X___, _XX_X___, _XXX____, _XXXX___, _XXXX___, _XXXX_X_, _X_X____,
    _X_X____, _X_X____, _X_X____, _X_X____, _X_XX___, __XX____, __XX____, _XXX____, _XXX____, _XXX____, _XXXX___, _XXXX___, _X_X____,
    _X_X____, _X_X____, _X_X____, _X_X____, _X_X____, _XX_X___, _XX_X___, _XXX____, _XXX____, _XXX____, _XX_X___, _X_XX___, _X_X____,
    _XX_X___, _XX_X___, _XX_X___, _XX_X___, _XX_X___, _XX_X___, _X_X____, _X_X____, _X_X____, _X_X____, _X_X____, _X_X____, _X_X____,
    _XX_X___, _XX_X___, _XX_X___, _XX_X___, _XX_X___, _XX_X___, _XX_X___, _XX_X___, _X_X____, _X_X____, _X_X____, _X_X____, _X_X____,
    //_X_X____, X_______, ________, ________, _XXXX___, ________, ________, ___X_X_X,
  };

  /**
   * $6BD9: bitmap_endshot_4
   *
   * Conv: the original binary's table for bitmap_endshot_4 is only 928 bytes;
   *       draw_endshot's fixed 72-row (64 bitmap + 8 attribute) loop always
   *       consumes 936 bytes, so the original spills 8 bytes into the next
   *       label (handshake_1, $F381: $C0,$00,$00,$00,$00,$00,$00,$00).
   *       Reproduced verbatim below rather than zero-padding.
   */
  static const u8 bitmap_endshot_4[936] = {
    XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_X_X_, X_XXX_XX, __XX____, X__XXXXX, XXXXXXXX, __XXXXXX, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, _X_X_X_X, XXXXX_XX, _X_X____, XX_XXXXX, XXXXXXXX, __XXXXXX, _XXXX___, _____XXX, XXXX____, ____X___,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_XXXX, XXXXX_XX, __XX____, X_X_X_X_, X_X_X_XX, ___X_X_X, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXXXXXXX, _XXXXXXX, XXXXX_XX, _X_X____, XX_XXXXX, XXXXXXXX, __XXXXXX, _XXXX___, __XXXXXX, XXXXXXXX, ___X_X_X,
    XXXXXXXX, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXX_XX, __XX____, X_X_X_X_, X_X_X_XX, ___X_X_X, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXXX_XX, _X_X____, XX__XXXX, XXXXXXXX, __XXXXXX, _XXXX___, _XXXXXXX, X_XXXXXX, X___X___,
    XXXXXXX_, X_X_X_XX, XXXXXXXX, XXXXXXXX, XXXXX_X_, __XX____, ____XXXX, _______X, ________, _XXXXXXX, XXXXXXX_, X__XXXXX, XXXXXXXX,
    XXXXXXXX, _X___X_X, XXXXXXXX, XXXXXXXX, XXXXX_XX, _X______, ____XXXX, XXXXXXXX, __XXXXXX, _XXXX___, _XXXXX__, X__XX_XX, XX_X_X_X,
    XXXXXXX_, XXX___XX, XXXXXXXX, XXXXXXXX, XXX_X_X_, X_______, _____XXX, _______X, ________, _XXXXXXX, XXXXX___, _X_XXXXX, XXXXXXXX,
    XXXXXXXX, _XXX_X_X, XXXXXXXX, XXXXXXXX, XXXXX_XX, _XXXXXX_, ______XX, XXXXXXXX, __XXXXXX, _XXXX___, _XXXX___, ____X_XX, XX__X___,
    XXXXXXXX, XXX___XX, XX_XXXXX, XX_XXXXX, XXX_X_X_, XXXXXXXX, _____XXX, _______X, ________, _XXXXXXX, XXXXX___, ___X_X_X, XXXXXXXX,
    XXXXXXX_, XX_X___X, X_X_XXXX, XXX___XX, XX_XX_XX, XXXXXXXX, ______XX, XXXXXXXX, __XXXXXX, _XXXX___, __XXXXX_, ___XXXXX, XX_X_X_X,
    XXXXXX__, X_X___X_, _XXXXXXX, XX_____X, XXX_X_XX, XXXXXXXX, X____XXX, _______X, ________, _XXXXXXX, XXXXXXXX, __XXXX_X, XXXXXXXX,
    XXXXXX_X, _X___X_X, X_X_XXXX, XXX___XX, XX_XX_X_, XXXXXXXX, X___X_XX, XXXXXXXX, __XXXXXX, _XXXX___, __XXX_X_, __XXXXXX, XX__X___,
    XXXXX_X_, X_____X_, XX_XXXXX, XX_____X, XXX_X_XX, XXXXXXXX, X____XXX, _______X, ________, _XXXXXXX, XXXXXX_X, __XXXX_X, XXXXXXXX,
    XXXXX__X, _X___X_X, XXXXXXXX, XXX___XX, XX_XX_X_, XXXXXXXX, ____XXXX, XXXXXXXX, __XXXXXX, _XXXX___, ___XX___, __XXX_XX, X__X_X_X,
    XXXXXXXX, X___X_X_, XXXXXXXX, XX_____X, XXX_X_XX, XXX__XXX, __XXXX_X, _______X, ________, _XXXXXXX, XXXXX__X, __XX_X_X, XXXXXXXX,
    XXXXX_XX, _X___X_X, XXXXX_XX, XX____XX, XX_XX_X_, XXX__XX_, X____X__, XXXXXXXX, __XXXXXX, _XXXX___, ___XX___, XXXXX_XX, ____X___,
    XXXX_XX_, X_X_X_X_, XXXX_X_X, X______X, XXX_X_XX, _XX___XX, ________, _______X, ________, _XXXXXXX, XXXXXX_X, XXXX_X_X, XXXXXXXX,
    XXXXX_XX, XX_X_X_X, XXXXX_XX, X_____X_, XX_XX_X_, _XXX____, X_______, XXXXXXXX, __XXXXXX, _XXXX___, ___XXXXX, XXXXX_XX, ___X_X_X,
    XXXX__X_, X_X_X_XX, XXXX_X_X, XX_X_XXX, XXX_X_XX, _XXX___X, _X____XX, _______X, ________, _XXXXXXX, XXXXXXX_, ___XXXXX, XXXXXXXX,
    XXXXX_XX, XX_X_X_X, XXXXX_XX, _XXXXXXX, XXXXXXXX, __X_____, X____XXX, XXXXXXXX, __XXXXXX, _XXXX___, __XXXX__, __X_X_X_, ____X___,
    XXXX___X, X_X_X_XX, XXXX_X_X, _XXXXXXX, XXXXXXXX, _XX____X, _X_X_XX_, _______X, ________, _XXXXXXX, XXXXXX_X, _X_XXXXX, XXXXXXXX,
    XXXXX__X, _X_X_XXX, XXX_X_X_, _XXXXXXX, XXXXXXXX, XXXX____, X_X_XXXX, XXXXXXXX, __XXXXXX, _XXXXXXX, XXXXXXX_, X_XXXXXX, XX_X_X_X,
    XXXX____, X_X_XXXX, XX_X_XX_, X_XXXXXX, XXXXXXXX, XXXXX__X, _X_X_X_X, _______X, ________, _XXXXXXX, XXXXXX_X, XXXXXXXX, XXXXXXXX,
    XXXXX___, XXXXXXXX, X_X_XX__, __XXXXXX, XXXXXXXX, XXXXXX_X, X_X_X_XX, XXXXXXXX, __XXXXXX, XXXXXXXX, XXXX_X__, __XXXX_X, XXXXXXX_,
    XXXX_X_X, _X_XXXXX, _XXX____, X_XXXXXX, XXXXXXXX, XXXXXX_X, XX_X_X_X, _______X, _______X, XXXXXXXX, XXXXX_X_, __XXX_XX, XXXXXXXX,
    XXXXX_X_, X____XXX, XXX_____, __XXXXXX, XXXXXXXX, XXXXXXX_, XXXXXXXX, XXXXXXXX, __XXXXXX, XXXXXXXX, XXXX__X_, ___XXX_X, XXXXXXXX,
    XXXX____, _X____XX, ___XX___, __XXXXXX, XXXXXXXX, XXXXXXX_, XXXX__XX, _______X, _______X, XXXXXXXX, XXXXX__X, _X_XX_X_, XXXXXXXX,
    XXXXX___, X___XXXX, XXX_X___, __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, __XXXXXX, XXXXXXXX, XXXX___X, X_XX___X, XXXXXXXX,
    XXXX_X_X, _XXXXXXX, _XX_____, _XXXXXXX, XXXXXXXX, XXXXXXXX, _X____XX, _______X, _______X, XXXXXXXX, XXXXX___, XXXX____, XXXXXXXX,
    XXXXX_XX, XXXXXXXX, XXX_____, __XXXXXX, XXXXXXXX, XXXXXXXX, X_XXXXXX, XXXXXXXX, __XXXXXX, XXXXXXXX, XXXX____, _XX____X, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, _X______, _XXXXXXX, XXXXXXXX, XXXXXXXX, X_X___XX, _______X, _______X, XXXXXXXX, XXX___X_, XX___X__, XXXXXXXX,
    XXXXXXXX, XXXXXXX_, XX______, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, __XXXXXX, XXXXXXXX, XXXX____, XXX_X__X, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX______, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_X__XX, _______X, ______XX, XXXXXXXX, XXX____X, XXX____X, XXXXXXXX,
    XXXXXXXX, XXXXXXX_, XX______, _XXXXXXX, XXXXXXXX, XXXXXXXX, XX_XXXXX, XXXXXXXX, __XXXXXX, XXXXXXXX, XXXX____, XXX____X, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX______, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX__XX, _______X, ______XX, XXXXXXXX, XXX____X, XX_____X, XXXXXXXX,
    XXXXXXXX, XXXXXXX_, XX______, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, __XXXXXX, XXXXXXXX, XXXX__X_, XX____XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX______, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX_XX, _______X, ______XX, XXXXXXXX, XXX__X_X, XX____XX, XXXXXXXX,
    XXXXXXXX, XXXXXXX_, XX______, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, __XXXXXX, XXXXXXXX, XXXX____, XX____XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX______, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _______X, _____XXX, XXXXXXXX, XXXX____, XX____XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX______, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, __XXXXXX, XXXXXX__, X_XX____, XX____XX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX______, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _______X, _____XXX, XXXX__XX, _X_X____, XX___XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX______, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, __XXXXXX, XXXXXXXX, XXXX___X, X____XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX______, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X______X, ____XXXX, XXXXXXX_, _XXX___X, X____XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX__X___, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, __XXXXXX, XXXXXX__, ___XXX_X, X____XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX_X___X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_____X, ____XXXX, XXXXX___, __XXXXXX, X____XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXX_____, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _XXXXXX_, _X______, ___XXXXX, _XXX_XXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX_X___X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX____X, XXXXXXXX, __X_X_X_, __XXXXX_, __XXXXXX, _X_XXXXX,
    XXXXXXXX, XXXXXXXX, XXX_____, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, __XX_X_X, _X_XXXX_, _XXXXXXX, X_XXX_X_,
    XXXXXXXX, XXXXXXXX, XX_____X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, __X_X_X_, X_XXXXX_, _XXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXX_____, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XX_XXX, XXXXXXX_, _XXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX_____X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X__XXXXX, XXXX_X__, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XXX_____, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X__XXXXX, XXXXX___, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX_____X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_XXXXXX, XXXXXX_X, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, XX______, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, X______X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, X_______, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, X______X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, ________, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, _______X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, ________, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, _______X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    XXXXXXXX, XXXXXXXX, ________, XXXXXXXX, XXXXXXXX, _XX_____, _XX_____, _XX_____, _XX_____, _XX_____, _XX_____, _XX_____, _XX_____,
    _XXXX___, _XX_X___, _XX_X___, _XXX____, _XXX____, _XXX____, _XXX____, _XX_____, _XX_____, _XX_____, _XX_____, _XXX____, _XX_____,
    _XXXX___, _XX_X___, _XX_X___, _XXX____, _XXX____, _XXX____, _XXX____, _XX_____, _XX_____, _XX_____, _XX_____, _XXX____, _XX_____,
    _XXX____, __X_X___, _XX_X___, _XXX____, _XXX____, _XXX____, _XX_____, _XX_____, _XX_____, _XXXX___, _XXXX___, _XXX____, _XX_____,
    _XXX____, _XXX____, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XX_____, _XX_____, _XX_____, _XXXX___, _XXXX___, _XXXX___, _XXXX___,
    _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XX_____, _XX_____, _XX_____, _XXXX___, _XXXX___, _XXXX___, _XXXX___,
    _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XX_____, _XX_____, _XXXX___, _XXX____, _XXX____, _XXXX___, _XXXX___,
    _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___,
    _XXXX___, _XXXX___, _XXXX___, _XXXX___, _XXXX___, XX______, ________, ________, ________, ________, ________, ________, ________,
  };
  // clang-format on

  switch (addr)
  {
  case BITMAP_ENDSHOT_1_ADDR: return &bitmap_endshot_1[0];
  case BITMAP_ENDSHOT_2_ADDR: return &bitmap_endshot_2[0];
  case BITMAP_ENDSHOT_3_ADDR: return &bitmap_endshot_3[0];
  case BITMAP_ENDSHOT_4_ADDR: return &bitmap_endshot_4[0];
  default:                    assert(0); return NULL;
  }
}

/**
 * $F300: Start the end-screen beatbox
 *
 * Clears the three playback flags that carry state across ticks
 * (drum_active, extra_delay, started) then loads the first pattern in
 * es_music_patterns, exactly as Main.c's reset_music does for the shared
 * in-game engine.
 */
static void es_reset_music(chqstate_t *state)
{
  state->bank7->es_music.drum_active = 0;
  state->bank7->es_music.extra_delay = 0;
  state->bank7->es_music.started     = 0;
  es_next_pattern_at_addr(state, &es_music_patterns[0]);
}

/**
 * $F310: Advance to the next pattern once the current one's repeats expire
 *
 * Decrements pattern_repeats and returns immediately while repeats remain;
 * once it reaches zero, loads the pattern whose address follows the one
 * just played. Same structure as Main.c's next_pattern, operating on bank
 * 7's own es_music state.
 */
static void es_advance_pattern(chqstate_t *state)
{
  if (--state->bank7->es_music.pattern_repeats)
    return;
  es_next_pattern_at_addr(state, state->bank7->es_music.pattern_addr);
}

/**
 * $F318: Load the pattern at pattern_addr into bank 7 music state
 *
 * Reads the pattern's repeat count; $FF marks the end of the pattern list
 * and restarts from the word-pointer that follows it (an offset from
 * es_music_patterns' own base, $F53C when relocated -- see
 * z80addrtoendshot for the same "es_script only ever encodes one literal
 * table base" reasoning). Otherwise stores the repeat count and advances
 * pattern_addr past it, reads the one-byte offset into es_music_data,
 * and primes note_delay/note_delay_reload/pattern_start_ptr from the note
 * stream at that offset.
 *
 * Same structure as Main.c's next_pattern_at_addr, operating on bank 7's own
 * es_music_patterns/es_music_data tables and es_music state instead of the
 * shared in-game music engine's.
 *
 * \param[in] HL_pataddr Pattern-list read pointer (was HL).
 */
static void es_next_pattern_at_addr(chqstate_t *state, const u8 *HL_pataddr)
{
  // clang-format off
  /**
   * $F553 (relocated; source $FA42)
   *
   * Layout: each block is a delay-reload byte (ticks per note) followed by a
   * note stream, terminated by NOTE_END. NOTE_* macros are defined in
   * Internal.h. See es_play_music_48k (Bank7.c) for the byte-level decode this
   * is built from.
   */
  static const u8 es_music_data[172] = {
    // 0x00 (delay=6) - noise-only hi-hat pattern
    NOTE_DELAY(6),
    NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3),
    NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3),
    NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(3),
    NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3),
    NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3),
    NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(9),
    NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(9),
    NOTE_NOISE(9), NOTE_NOISE(3), NOTE_NOISE(9), NOTE_NOISE(3),
    NOTE_END,

    // 0x22 (delay=6) - drum2/drum1/noise beat
    NOTE_DELAY(6),
    NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3),
    NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3),
    NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_DRUM2(8),
    NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_NOISE(3),
    NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3),
    NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3),
    NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_DRUM2(8),
    NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_DRUM1(8), NOTE_DRUM1(8),
    NOTE_END,

    // 0x44 (delay=6) - drum2/drum1/noise beat
    NOTE_DELAY(6),
    NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_NOISE(3),
    NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_DRUM2(8),
    NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_DRUM2(8),
    NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_DRUM1(8), NOTE_NOISE(3),
    NOTE_END,

    // 0x56 (delay=6) - drum2/drum1/noise beat
    NOTE_DELAY(6),
    NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_NOISE(3),
    NOTE_DRUM1(8), NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_DRUM2(8),
    NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_NOISE(3),
    NOTE_DRUM1(8), NOTE_DRUM2(8), NOTE_DRUM1(8), NOTE_NOISE(3),
    NOTE_END,

    // 0x68 (delay=6) - drum2/drum1/noise beat
    NOTE_DELAY(6),
    NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_NOISE(3),
    NOTE_DRUM1(8), NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_DRUM2(8),
    NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_DRUM1(8), NOTE_NOISE(3),
    NOTE_DRUM1(8), NOTE_DRUM1(8), NOTE_DRUM1(8), NOTE_NOISE(3),
    NOTE_END,

    // 0x7A (delay=6) - drum2/noise beat
    NOTE_DELAY(6),
    NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3),
    NOTE_NOISE(3), NOTE_NOISE(3), NOTE_NOISE(3), NOTE_DRUM2(8),
    NOTE_DRUM2(8), NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_DRUM2(8),
    NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_DRUM2(8),
    NOTE_END,

    /* 0x8C (delay=6) - drum2/drum1 beat, a silent note, then a descending
     * noise fade-out sweep (param 11..1) before END
     */
    NOTE_DELAY(6),
    NOTE_DRUM2(8), NOTE_NOISE(3), NOTE_DRUM2(8), NOTE_NOISE(3),
    NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_DRUM1(8), NOTE_DRUM2(8),
    NOTE_DRUM1(8), NOTE_DRUM1(8), NOTE_NOISE(3), NOTE_DRUM1(8),
    NOTE_DRUM1(8), NOTE_DRUM2(8), NOTE_DRUM1(8), NOTE_SILENCE,
    NOTE_NOISE(11), NOTE_NOISE(10), NOTE_NOISE(9), NOTE_NOISE(8),
    NOTE_NOISE(7), NOTE_NOISE(6), NOTE_NOISE(5), NOTE_NOISE(4),
    NOTE_NOISE(3), NOTE_NOISE(2), NOTE_NOISE(1),
    NOTE_END,

    // 0xA9 (delay=0xFF) - single silent note, long pause before restart
    NOTE_DELAY(0xFF),
    NOTE_SILENCE,
    NOTE_END,
  };
  // clang-format on

  int       A_n_repeats; /* pattern repeat count just read, or 0xFF end marker (was A) */
  int       C_offset;    /* offset into es_music_data for this pattern's notes (was C) */
  const u8 *HL_data;     /* es_music_data read pointer, walked past the note-delay byte (was HL) */

  for (;;)
  {
    A_n_repeats = *HL_pataddr++;
    if (A_n_repeats != 0xFF)
    {
      // Not end of pattern(s)
      state->bank7->es_music.pattern_repeats = (u8) A_n_repeats;
      C_offset                               = *HL_pataddr++;
      state->bank7->es_music.pattern_addr    = HL_pataddr;

      // Calculate address of music data
      HL_data = &es_music_data[C_offset];
      state->bank7->es_music.note_delay_reload = state->bank7->es_music.note_delay = *HL_data++;
      state->bank7->es_music.pattern_start_ptr = HL_data;
      return;
    } else
    {
      // Restart
      HL_pataddr = &es_music_patterns[wordat(HL_pataddr) - 0xF53C];
    }
  }
}

/**
 * $F340: Service the end-screen beatbox for the current frame
 *
 * Per-tick music driver for bank 7's own 48K music engine, almost identical
 * to Main.c's play_music_48k operating on es_music/es_music_patterns/
 * es_music_data instead of the shared in-game engine's tables. Either
 * initialises playback on the first call or decrements the note delay
 * counter; when the delay expires, reads the next byte from the pattern
 * stream (a terminating byte of 1 advances to the next pattern via
 * es_advance_pattern). Bytes with bit 7 set carry a one-tick extra delay
 * flag; the lower three bits of the remaining byte select the instrument
 * (0 = silence, 1 = drum 2, 2 = drum 1, 3 = noise).
 *
 * Conv: the Z80 clears an interrupt flag ($F3BC) on entry then, once its own
 *       processing is done, busy-waits on that flag in a loop
 *       (b7pm_wait_for_interrupt) until the next interrupt sets it -- this is
 *       how the routine paces itself to one call per frame. show_end_screen's
 *       loop already paces each call via state->speccy->sleep, so both the flag
 *       and the wait loop are omitted; this function represents one
 *       already-paced tick, same as Main.c's play_music_48k.
 *
 * Conv: the Z80 checks es_input_mask ($A16F) first and, if it is non-zero (the
 *       player has pressed fire once already, skipping ahead to the
 *       congratulations script), jumps straight to the wait-for-interrupt loop
 *       -- i.e. does no music processing at all that tick. C returns
 *       immediately in that case.
 */
static void es_play_music_48k(chqstate_t *state)
{
  int       A_delay;      /* note_delay-1; tests whether the current note's delay has expired (was A) */
  const u8 *HL_data;      /* pattern byte-stream read pointer (was HL) */
  int       A_n_note;     /* raw music byte minus 1; zero marks the end-of-pattern sentinel (was A) */
  int       D_note;       /* adjusted music byte: delay bit consumed, upper bits = param, lower 3 = instrument (was D) */
  int       B_instrument; /* instrument index: lower 3 bits of D_note (was B) */
  int       A_param;      /* pitch/parameter value passed to the instrument handler (was A) */

  if (state->bank7->es_input_mask != 0)
    return; // Conv: wait-for-interrupt loop is a no-op here (see prologue)

  if (state->bank7->es_music.started == 0)
  {
    state->bank7->es_music.started = 1;
    goto pm_reset_pattern;
  }

  A_delay = state->bank7->es_music.note_delay - 1;
  if (A_delay)
  {
    state->bank7->es_music.note_delay = A_delay;
  } else
  {
    state->bank7->es_music.note_delay = state->bank7->es_music.note_delay_reload;
    HL_data = state->bank7->es_music.data_ptr;

    for (;;)
    {
      A_n_note = *HL_data - 1;
      if (A_n_note)
        break;

      es_advance_pattern(state);

pm_reset_pattern:
      HL_data = state->bank7->es_music.pattern_start_ptr;
      state->bank7->es_music.data_ptr = HL_data;
    }

    state->bank7->es_music.data_ptr = ++HL_data;
    if (++A_n_note > NOTE_XDELAY_FLAG)
    {
      // A byte of the form 0b1aaaaiii (1 is the delay flag bit)
      A_n_note                           &= ~NOTE_XDELAY_FLAG;
      state->bank7->es_music.note_delay   = 1;
      state->bank7->es_music.extra_delay  = 1;
    }

    D_note       = A_n_note;
    B_instrument = D_note & NOTE_INST_MASK;
    if (B_instrument)
    {
      A_param = D_note >> 3;
      switch (B_instrument)
      {
      case NOTE_DRUM2_VAL: es_playdrum_2(state, A_param); return;
      case NOTE_DRUM1_VAL: es_playdrum_1(state, A_param); return;
      case NOTE_NOISE_VAL: es_play_noise(state, A_param); return;
      }
    }
  }

  if (state->bank7->es_music.extra_delay)
  {
    state->bank7->es_music.note_delay--;
    state->bank7->es_music.extra_delay--;
  }

  /* Conv: b7pm_start_drums/b7pm_wait_for_interrupt -- resuming a drum sample
   * suspended by a real Z80 interrupt has no equivalent here; playdrum
   * playback (once ported) will always run to completion within one call.
   */
}

/**
 * $F3CA: Play bank 7 drum sample 2 for the current tick
 *
 * Starts playback of drum sample 2 (94 bytes). Records the drum speed and
 * marks the drum as active, then calls es_playdrum_go to output it.
 * Analogous to Main.c's playdrum_2/playdrum_start.
 *
 * \param[in] A_speed Playback speed: inner loop count per sample byte (was A).
 */
static void es_playdrum_2(chqstate_t *state, int A_speed)
{
  state->bank7->es_music.drum_speed  = A_speed;
  state->bank7->es_music.drum_active = 1;
  es_playdrum_go(state, sizeof(state->bank7->es_drum2), &state->bank7->es_drum2[0]);
}

/**
 * $F3D1: Play bank 7 drum sample 1 for the current tick
 *
 * Starts playback of drum sample 1 (160 bytes). Records the drum speed and
 * marks the drum as active, then calls es_playdrum_go to output it.
 * Analogous to Main.c's playdrum_1/playdrum_start.
 *
 * \param[in] A_speed Playback speed: inner loop count per sample byte (was A).
 */
static void es_playdrum_1(chqstate_t *state, int A_speed)
{
  state->bank7->es_music.drum_speed  = A_speed;
  state->bank7->es_music.drum_active = 1;
  es_playdrum_go(state, sizeof(state->bank7->es_drum1), &state->bank7->es_drum1[0]);
}

/**
 * $F504: Play bank 7's noise instrument for the current tick
 *
 * Bank 7's own copy of Main.c's play_noise routine, decoded by hand from
 * the raw bytes at source $F9F3-$FA2A (56 bytes) after the disassembler
 * mislabelled the region as data. Confirmed identical to the shared
 * routine, operating on the same fixed-address state->rng_seed -- so
 * rather than duplicate it, this calls the shared implementation directly.
 *
 * \param[in] A_param Noise duration: outer loop count and pulse timing (was A).
 */
static void es_play_noise(chqstate_t *state, int A_param)
{
  play_noise(state, A_param); /* tail call */
}

/**
 * $F8C5: Output bank 7 PCM drum sample
 *
 * Only the es_playdrum_2/es_playdrum_1 entry points at $F3CA/$F3D1 and the
 * sample tables were decoded this pass.
 *
 * Outputs a PCM drum sample byte-by-byte to the speaker port. For each
 * sample byte, an inner loop runs drum_speed iterations; each iteration
 * writes bit 7 of the current sample byte to the EAR bit of
 * port_BORDER_EAR_MIC, then rotates the sample byte left in-place (RLC) so
 * successive iterations output successive bits -- 1-bit PCM at drum_speed
 * bits per byte. When all [D_length] bytes have been output, drum_active is
 * cleared. Identical in structure to Main.c's playdrum_go, operating on
 * bank 7's own es_music state and es_drum2/es_drum1 buffers.
 *
 * \param[in] D_length Number of sample bytes remaining to output (was D).
 * \param[in] HL_data  Pointer to the next sample byte in
 *                     state->bank7->es_drum2[] or state->bank7->es_drum1[]
 *                     (was HL).
 *
 * Conv: the Z80 uses RLC (HL) to walk bit 7 through all 8 bit positions across
 *       8 iterations -- the byte doubles as its own iteration counter, no
 *       separate bit-index register needed. This rotation mutates the sample
 *       data in place (only a full 8-bit rotation restores it), so the drum
 *       samples live in bank7 state as mutable copies of
 *       es_drum_sample_2_template/es_drum_sample_1_template.
 *
 * Conv: the inter-OUT delay code is modelled as speccy->logtime so the host can
 *       reconstruct the bit timing.
 *
 * Conv: C has no mid-sample interrupts, so the early-return resume path never
 *       triggers and the sample always plays to completion in one call.
 */
static void es_playdrum_go(chqstate_t *state, int D_length, u8 *HL_data)
{
  zxspectrum_t *speccy; /* game's ZX Spectrum facade (was N/A) */
  int           carry;  /* carry flag used by RLC (carry) */
  int           i;      /* inner loop counter: drum_speed ticks per sample byte (was B') */
  int           bits;   /* speaker output level: port_MASK_EAR or 0 based on sample bit 7 (was A) */

  speccy = state->speccy;
  carry  = 0;
  for (;;)
  {
    i = state->bank7->es_music.drum_speed;
    do
    {
      bits = port_MASK_EAR; // speaker bit
      if ((*HL_data & (1 << 7)) == 0)
        bits = 0;
      speccy->out(speccy, port_BORDER_EAR_MIC, bits);
      RLC(*HL_data); /* rotate sample byte in place */
      /* inter-bit cost 15+13+7+4+12+12 (bit-set path) */
      speccy->logtime(speccy, 63);
    } while (--i > 0);
    HL_data++;
    /* inter-byte cost 6+4+7+13+4+10+7, less the DJNZ not-taken saving */
    speccy->logtime(speccy, 46);
    if (--D_length == 0)
      goto pd_end_of_sample;
  }
  // EXX unbank
  return;

pd_end_of_sample:
  state->bank7->es_music.drum_active = 0;
}

/* ----------------------------------------------------------------------- */

/**
 * Allocate and initialise the bank 7 sub-state.
 *
 * Conv: host lifecycle helper; has no Z80 address. Called once from chq_create.
 *       Sets the fade-gate bytes and drum-sample copies that calloc's zero fill
 *       would otherwise leave wrong (see the comments below).
 *
 * \return 0 on success, -1 if allocation failed.
 */
int bank7_state_create(chqstate_t *state)
{
  // clang-format off
  /* Bank 7's own drum sample templates, played by es_playdrum_2/
   * es_playdrum_1 (Bank7.c). Byte-for-byte identical to CommonData.c's
   * drum2_template/drum1_template for all but the final byte -- not aliases
   * of those tables, since that last byte differs (0x00 here vs 0xFF
   * there), and bank 7 uses shorter lengths (94/160 vs 108/252)
   * throughout. */
  /** $F8F5 (source; used by es_playdrum_2) */
  static const u8 es_drum_sample_2_template[94] = {
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
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00
  };

  /** $F953 (source; used by es_playdrum_1) */
  static const u8 es_drum_sample_1_template[160] = {
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
    0x9F, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0x00
  };
  // clang-format on

  state->bank7 = calloc(1, sizeof(*state->bank7));
  if (state->bank7 == NULL)
    return -1;

  /* $E06C/$E06D initial bytes (relocated to $5C6C/$5C6D): these flip-flop
   * gate bytes live in what was code space before relocation, so calloc's
   * zero is wrong -- RLC(0) is always 0 with no carry, which permanently
   * disables the GLYPH_B fade and the handshake animation's advance/draw
   * block. */
  state->bank7->es_fade_gate_ab = 0xAA;
  state->bank7->es_fade_gate_c  = 0x88;

  /* es_playdrum_go rotates each sample byte in place during playback (RLC),
   * so bank 7 needs its own mutable copies of the drum templates, refreshed
   * per instance exactly like chqstate_t's own music.drum1/music.drum2
   * (Create.c). */
  memcpy(state->bank7->es_drum2, es_drum_sample_2_template,
         sizeof(es_drum_sample_2_template));
  memcpy(state->bank7->es_drum1, es_drum_sample_1_template,
         sizeof(es_drum_sample_1_template));

  return 0;
}

/**
 * Free the bank 7 sub-state.
 *
 * Conv: host lifecycle helper; has no Z80 address. Called once from
 *       chq_destroy.
 */
void bank7_state_destroy(chqstate_t *state)
{
  free(state->bank7);
  state->bank7 = NULL;
}
