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
 * This file will hold the C translation of ZX Spectrum 128K "bank 7" -- the
 * end-of-game results/credits sequence. Paged into $C000-$FFFF and reached
 * via an inner relocation from $F7EF to $F300, driven by a script
 * interpreter at $E20A. Not yet ported; see show_end_screen() in Main.c.
 */

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "C99/Types.h"

#include "ZXSpectrum/Macros.h"

#include "ChaseHQ/ChaseHQ.h"

#include "Internal.h"
#include "State.h"
#include "Bank7State.h"

#include "Bank7.h"

/* ----------------------------------------------------------------------- */

u8 call_bank_7_128k(chqstate_t *state, int HLroutine)
{
  NOT_USED(state);
  NOT_USED(HLroutine);

  return 0;
}

/**
 * $E499: Clear the playfield ready for the end screen
 *
 * Zeros the on-screen playfield (attributes and bitmap) plus the first 512
 * bytes of the backbuffer, which alias the attribute portion of the
 * end-screen montage/glyph drawing area ($F000 = &state->backbuffer[0]).
 *
 * \param[in] state Pointer to game state.
 */
static void es_clear(chqstate_t *state)
{
  clear_playfield(state);
  memset(&state->backbuffer[0], 0, 512);
}

/* Script command bytes, $E20D's DEC A/JP Z chain (1-based, in read order). */
#define ESCMD_DRAW_FRAME      (1)  /* -> $E2D9 routine_e2d9, runs immediately */
#define ESCMD_DRAW_WORD       (2)  /* -> $E2DE routine_e2de, runs immediately */
#define ESCMD_GLYPH_A         (3)  /* -> $E42E routine_e42e via rs_exit, reload 16 */
#define ESCMD_GLYPH_B         (4)  /* -> $E472 routine_e472 via rs_exit, reload 16 */
#define ESCMD_HANDSHAKE       (5)  /* -> $E3B7 routine_e3b7 via rs_exit, reload 16 */
#define ESCMD_GLYPH_C         (6)  /* -> $E46D routine_e46d via rs_exit, reload 32 */
#define ESCMD_IDLE            (7)  /* -> rs_exit, handler = no-op, reload = script byte */
#define ESCMD_SET_A172        (8)  /* -> rs_exit, sets $A172, handler = handshake, reload = script byte */
#define ESCMD_HANDSHAKE_AGAIN (9)  /* -> rs_exit, handler = handshake, reload = script byte */
#define ESCMD_10              (10) /* -> $E2F0 (undecoded) */
#define ESCMD_11              (11) /* -> $E2C0-style entry (undecoded) */
#define ESCMD_CALL_WORD       (12) /* -> $E2B2, runs immediately */
#define ESCMD_RENDER_SCORE    (13) /* -> $E256, runs immediately */

#define ADDRTOSCREEN(addr) z80addrtoscreen(state, addr, 0, 0)

/**
 * $E0FE-$E209: Raw end-screen script bytes.
 *
 * Verbatim transcription of the skool's script_data block (268 bytes):
 * command/argument bytes interleaved with bitmap addresses (as raw
 * little-endian DEFW pairs) and embedded high-bit-terminated ASCII text
 * ("CONGRATULATIONS", "WELL DONE", etc). run_script's command dispatch
 * (below) has not yet been checked against every byte here -- this is
 * the raw data only, not yet validated end-to-end against a real script
 * run.
 *
 * Conv: not const -- es_handler_render_score patches the "GBP________ PTS"
 * placeholder text in-place (offset 0xFD, matching $5DFB relocated) with the
 * player's formatted score, exactly as the original self-modifies its own
 * script_data at that address.
 */
static u8 script_data[] = {
  0x0C,
  0x6E, 0x5C,
  0x07, 0xC0,
  0x01,
  0xE1, 0x60,
  0x89, 0x48, 0x03, 0x07, 0xA0, 0x04, 0x01,
  0x89, 0x64,
  0x89, 0x48, 0x03, 0x07, 0xA0, 0x04, 0x01,
  0x31, 0x68,
  0x89, 0x48, 0x03, 0x07, 0xA0, 0x04, 0x01,
  0xD9, 0x6B,
  0x89, 0x48, 0x03, 0x07, 0xA0, 0x04, 0x01,
  0xE1, 0x60,
  0x02, 0x48, 0x03, 0x07, 0x50, 0x02,
  0x89, 0x64,
  0x11, 0x48, 0x03, 0x07, 0x50, 0x02,
  0x31, 0x68,
  0x02, 0x50, 0x03, 0x07, 0x50, 0x02,
  0xD9, 0x6B,
  0x11, 0x50, 0x03, 0x07, 0x50, 0x08, 0xC0, 0x09, 0xB0, 0x05, 0x09, 0xB0, 0x04,
  0x0B, 0x47, 0x48, 0x48, 0x43, 0x4F, 0x4E, 0x47,
  0x52, 0x41, 0x54, 0x55, 0x4C, 0x41, 0x54, 0x49,
  0x4F, 0x4E, 0x53, 0xA1, 0x03, 0x07, 0x08, 0x0A,
  0x47, 0xCB, 0x48, 0x41, 0x4C, 0x4C, 0x20, 0x20,
  0x43, 0x4C, 0x45, 0x41, 0xD2, 0x03, 0x07, 0x08,
  0x0A, 0x47, 0x49, 0x50, 0x35, 0x2C, 0x30, 0x30,
  0x30, 0x2C, 0x30, 0x30, 0x30, 0x20, 0x20, 0x50,
  0x54, 0x53, 0xAE, 0x03, 0x07, 0xC0, 0x06, 0x07,
  0x60, 0x0B, 0x47, 0x84, 0x48, 0x28, 0x43, 0x29,
  0x20, 0x31, 0x39, 0x38, 0x39, 0x20, 0x4F, 0x43,
  0x45, 0x41, 0x4E, 0x20, 0x53, 0x4F, 0x46, 0x54,
  0x57, 0x41, 0x52, 0xC5, 0x0A, 0x47, 0x43, 0x50,
  0x28, 0x43, 0x29, 0x20, 0x31, 0x39, 0x38, 0x38,
  0x20, 0x54, 0x41, 0x49, 0x54, 0x4F, 0x20, 0x43,
  0x4F, 0x52, 0x50, 0x4F, 0x52, 0x41, 0x54, 0x49,
  0x4F, 0xCE, 0x03, 0x07, 0xF0, 0x06, 0x07, 0x60,
  0x0B, 0x47, 0xCC, 0x48, 0x54, 0x48, 0x45, 0x20,
  0x20, 0x45, 0x4E, 0xC4, 0x03, 0x07, 0xF0, 0x07,
  0x60, 0x06, 0x07, 0x60,
  0x0B, 0x45, 0xAA, 0x48,
  0x46, 0x49, 0x4E, 0x41, 0x4C, 0x20, 0x20, 0x53,
  0x43, 0x4F, 0x52, 0xC5, 0x03, 0x07, 0x1E, 0x0D,
  0x0A, 0x47, 0x2C, 0x50, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0xA0, 0x03, 0x0C, 0x78, 0x5C,
  0x07, 0x00, 0x0E
};

/**
 * $E2D9/$E2DE (stub): Draw an end-screen graphic frame
 *
 * TODO: not yet ported (bitmap/glyph blit phase). Reads a byte then a word
 * pointer from the script (e.g. one of the bitmap_endshot_N pointers) and
 * blits it via draw_endshot ($E4A9).
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_draw_frame(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $E42E/$E472/$E46D (stub): Plot end-screen glyphs
 *
 * TODO: not yet ported (bitmap/glyph blit phase). Three related entry points
 * into the same glyph-plot routine, called repeatedly (every reload-count
 * frames) while $A16D points past the command byte at the text to plot.
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_glyph_plot(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $E3B7/$E3BA (stub): Advance the handshake animation frame
 *
 * TODO: not yet ported (bitmap/glyph blit phase). Ping-pongs through the
 * four handshake_N bitmaps via the $E3A5 table, LDIR'd to screen $48AC.
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_handshake(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $E2D8 (stub): Idle per-frame handler (no drawing)
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_idle(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $E256: Tally the bonus into the score, then render the final score text
 *
 * First runs a 1000-iteration bonus-tally animation: each iteration adds
 * 5,000 to the score (increment_score), redraws the scoreboard LED digits
 * (ptad_led_digits, same call as the in-game scoreboard update) and plays one
 * "bipbow" tick (sfx_bipbow), giving the classic arcade rising-score sound
 * and visual. Sets es_input_mask so the very next fire press exits the end
 * screen (see Bank7State.h). Then formats the final score as ASCII -- most
 * significant BCD byte first, blanking leading zeros -- directly into the
 * "GBP________ PTS" placeholder already sitting in script_data (offset
 * 0xFD, $5DFB relocated), so the later draw-word command renders it.
 *
 * Conv: the leading-zero blank/print decision (Z80: RLC C carry chain) is
 * modelled as a sticky "seen a non-blank digit yet" flag: print the digit if
 * it is non-zero or a digit has already been printed, otherwise print a
 * space.
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_render_score(chqstate_t *state)
{
  int       tally;   /* bonus-tally animation iteration counter (was BC) */
  const u8 *DEbcd;    /* packed-BCD score pointer, walked backwards (was DE) */
  u8       *HLdst;    /* destination ASCII bytes in script_data (was HL) */
  u8        Cseen;    /* sticky "non-blank digit already printed" flag (was C) */
  int       pair;     /* BCD byte-pair iteration counter (was B) */
  u8        Anibble;  /* nibble being converted to ASCII (was A) */

  for (tally = 1000; tally != 0; tally--) {
    increment_score(state, 0, 0x00, 0x50);
    ptad_led_digits(4, &state->score_bcd[3], &state->session.score_digits[7],
                     ADDRTOSCREEN(0x4126));
    sfx_bipbow(state, 2, 2);
  }

  state->bank7->es_input_mask = 1;

  DEbcd = &state->score_bcd[3];
  HLdst = &script_data[0xFD];
  Cseen = 0;

  for (pair = 4; pair != 0; pair--) {
    Anibble = (*DEbcd >> 4) & 0x0F;
    if (Anibble != 0 || Cseen != 0) {
      Cseen    = 0xFF;
      *HLdst++ = 0x30 + Anibble;
    } else {
      *HLdst++ = 0x20;
    }

    Anibble = *DEbcd & 0x0F;
    if (Anibble != 0 || Cseen != 0) {
      Cseen    = 0xFF;
      *HLdst++ = 0x30 + Anibble;
    } else {
      *HLdst++ = 0x20;
    }

    DEbcd--;
  }

  HLdst[-1] |= 0x80; /* SET 7,(HL): mark this text run's terminator byte */
}

/**
 * $E2B2 (stub): Call a script-supplied handler with a literal argument
 *
 * TODO: not yet ported ($E2B9 CALL $9945, target and purpose unconfirmed).
 * Runs immediately (loops back into run_script rather than returning).
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_call_word(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $E2CD rs_exit: Set the per-frame handler and its frame-delay reload
 *
 * Common tail shared by the run_script commands that hand off to a
 * self-modified per-frame handler rather than running immediately: stores
 * the new handler and reload count, ready for show_end_screen's loop to
 * count down and re-invoke run_script when it reaches zero.
 *
 * \param[in] state Pointer to game state.
 * \param[in] handler New per-frame handler (was DE).
 * \param[in] reload New $A170 frame-delay reload count (was C).
 */
static void es_set_dispatch(chqstate_t *state, void (*handler)(chqstate_t *),
                             u8 reload)
{
  state->bank7->es_handler     = handler;
  state->bank7->es_frame_count = reload;
}

/**
 * $E20A: Run the current end-screen script
 *
 * Reads and dispatches script command bytes from state->es_script_ptr in a
 * DEC A/JP Z chain matching the ESCMD_* constants above. "Immediate" commands
 * (draw frame, call word, render score) run their handler stub straight away
 * and loop for the next command in the same call; all other commands instead
 * arm state->es_handler/es_frame_count via es_set_dispatch and return,
 * leaving show_end_screen's per-frame loop to invoke the handler on a delay.
 *
 * Conv: the handler bodies themselves (draw/glyph-plot/handshake/score) are
 * not yet ported -- see the TODO stubs above. This function faithfully
 * reproduces the command dispatch and byte consumption only.
 *
 * \param[in] state Pointer to game state.
 */
static void run_script(chqstate_t *state)
{
  const u8 *HLscript; /* script program counter (was HL) */
  u8        Acmd;     /* command byte just read (was A) */
  u8        Creload;  /* frame-delay reload value about to be applied (was C) */

  HLscript = state->bank7->es_script_ptr;

  for (;;) {
    Acmd = *HLscript++;

    switch (Acmd) {
    case ESCMD_DRAW_FRAME:
      es_handler_draw_frame(state);
      continue;

    case ESCMD_DRAW_WORD:
      /* Conv: $E2DE is $E2D9's tail half, entered directly for this command
       * (skipping E2D9's own backbuffer-clear prefix). Same stub for now. */
      es_handler_draw_frame(state);
      continue;

    case ESCMD_GLYPH_A:
    case ESCMD_GLYPH_B:
    case ESCMD_HANDSHAKE:
      es_set_dispatch(state, es_handler_glyph_plot, 16);
      goto rs_exit;

    case ESCMD_GLYPH_C:
      es_set_dispatch(state, es_handler_glyph_plot, 32);
      goto rs_exit;

    case ESCMD_IDLE:
      Creload = *HLscript++;
      es_set_dispatch(state, es_handler_idle, Creload);
      goto rs_exit;

    case ESCMD_SET_A172:
      Creload = *HLscript++;
      es_set_dispatch(state, es_handler_handshake, Creload);
      goto rs_exit;

    case ESCMD_HANDSHAKE_AGAIN:
      Creload = *HLscript++;
      es_set_dispatch(state, es_handler_handshake, Creload);
      goto rs_exit;

    case ESCMD_10:
    case ESCMD_11:
      /* TODO: $E2F0/$E2C0 region not yet decoded. */
      goto rs_exit;

    case ESCMD_CALL_WORD:
      es_handler_call_word(state);
      continue;

    case ESCMD_RENDER_SCORE:
      es_handler_render_score(state);
      continue;

    default:
      /* Unrecognised command: matches the Z80 fallback (reset to a fixed
       * script offset) closely enough for a defensive stop. */
      goto rs_exit;
    }
  }

rs_exit:
  state->bank7->es_script_ptr = HLscript;
}

/**
 * $E01E (stub): Set up bank 7's own interrupt handler and 48K music engine
 *
 * TODO: not yet ported. Bank 7 carries its own copy of the 48K sound code
 * (bank7_setup_interrupts onward in the skool), relocated into the copied
 * $F300 buffer.
 *
 * \param[in] state Pointer to game state.
 */
static void bank7_setup_interrupts(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $F300 (stub): Start the end-screen fanfare
 *
 * TODO: not yet ported. This is bank 7's own copy of the turbo-siren sound
 * code, relocated into the copied $F300 buffer -- distinct from Main.c's
 * play_engine_or_turbo_sfx_128k.
 *
 * \param[in] state Pointer to game state.
 */
static void play_turbo_sfx_128k(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $F340 (stub): Service end-screen speech playback for the current frame
 *
 * TODO: not yet ported. Skool comment marks this as living two bytes before
 * the shared play_speech_128k in the relocated buffer; not confirmed to be
 * the same routine.
 *
 * \param[in] state Pointer to game state.
 */
static void es_service_speech(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $E000: Show the end screen
 *
 * Displays the end-of-game results screen: clears the playfield, starts the
 * fanfare and bank 7's own interrupt-driven music/script engine, then loops
 * driving the script/frame-advance dispatch and drive_chatter each frame
 * until the fire key is pressed twice (once to reach the congratulations
 * script, once more to exit), clearing the queued key each time.
 *
 * Conv: the Z80 entry point LDIRs itself from $E000 to $5C00, then LDIRs a
 * second, inner 768-byte block from $F7EF to $F300 and runs from there (see
 * project memory project-endscreen-bank7-double-relocation). Both
 * relocations are pure ZX paging/self-modification artefacts and are
 * discarded here, following the load_stage precedent -- the C functions
 * below are simply called directly.
 *
 * \param[in] state Pointer to game state.
 */
void show_end_screen(chqstate_t *state)
{
  u8  Ainput;             /* keyscan result, tested for the fire bit (was A) */
  int outer_count;        /* per-keyscan frame divider, reloads to 5/6 (was A171) */

  es_clear(state);
  bank7_setup_interrupts(state);
  play_turbo_sfx_128k(state);

  state->bank7->es_script_ptr  = script_data;
  state->bank7->es_frame_count = 1;
  state->bank7->es_handler     = es_handler_idle;
  state->bank7->es_input_mask  = 0;

  outer_count = 6;

  for (;;) {
    if (state->host_quit)
      longjmp(state->host_quit_jmp, 1);

    es_service_speech(state);
    if (--state->bank7->es_frame_count == 0)
      run_script(state);
    state->bank7->es_handler(state);

    if (--outer_count != 0)
      continue;
    outer_count = 5;

    drive_chatter(state);
    Ainput = keyscan(state);
    if (!(Ainput & USERINPUTFLAG_FIRE))
      continue;

    if (state->bank7->es_input_mask == 0) {
      /* First fire press: skip ahead to the congratulations script. */
      state->bank7->es_input_mask = 1;
      drive_chatter_stop(state);
      while (keyscan(state) & USERINPUTFLAG_FIRE)
        ;
      continue;
    }

    /* Second fire press: leave the end screen. */
    break;
  }

  drive_chatter_stop(state);
}

int bank7_state_create(chqstate_t *state)
{
  state->bank7 = calloc(1, sizeof(*state->bank7));
  if (state->bank7 == NULL)
    return -1;

  return 0;
}

void bank7_state_destroy(chqstate_t *state)
{
  free(state->bank7);
  state->bank7 = NULL;
}
