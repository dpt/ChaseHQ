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
 * $E256 (stub): Render the final score text
 *
 * TODO: not yet ported. Formats two BCD-ish numbers (bonus count and total)
 * via $9D17/$9F12/$8A36 and writes the digits into the script text buffer
 * at $5DFB, then falls through to routine_e2d9-style drawing. Runs
 * immediately (loops back into run_script rather than returning).
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_render_score(chqstate_t *state)
{
  NOT_USED(state);
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
  if (HLscript == NULL) {
    /* TODO: script_data ($E0FE) not yet ported; nothing to dispatch. */
    state->bank7->es_frame_count = 0xFF;
    return;
  }

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
  int Auser_input_mask;   /* 0 until fire first consumed, then sticky (was A16F) */
  int outer_count;        /* per-keyscan frame divider, reloads to 5/6 (was A171) */

  es_clear(state);
  bank7_setup_interrupts(state);
  play_turbo_sfx_128k(state);

  state->bank7->es_script_ptr  = NULL; /* TODO: point at ported script_data ($E0FE) */
  state->bank7->es_frame_count = 1;
  state->bank7->es_handler     = es_handler_idle;

  Auser_input_mask = 0;
  outer_count       = 6;

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

    if (Auser_input_mask == 0) {
      /* First fire press: skip ahead to the congratulations script. */
      Auser_input_mask = 1;
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
