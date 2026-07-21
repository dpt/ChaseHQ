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
#include <string.h>

#include "C99/Types.h"

#include "ZXSpectrum/Macros.h"

#include "ChaseHQ/ChaseHQ.h"

#include "Internal.h"
#include "State.h"

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

/**
 * $E20A (stub): Run the current end-screen script
 *
 * TODO: not yet ported. Drives the visual-effect/text sequence: two scripts
 * back to back (the intro montage commands, then the congratulations text),
 * self-modifying the per-frame dispatch call at $5ED8 as it goes. See
 * project memory project-endscreen-bank7-double-relocation for the traced
 * behaviour.
 *
 * \param[in] state Pointer to game state.
 */
static void run_script(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $5ED8 (stub): Advance one frame of the current script command
 *
 * TODO: not yet ported. The Z80 self-modifies this call's operand each time
 * run_script dispatches a new command, repointing it at one of the
 * draw-frame/glyph-plot handlers ($E2D9/$E42E/$E3B7/$E46D).
 *
 * \param[in] state Pointer to game state.
 */
static void advance_script_frame(chqstate_t *state)
{
  NOT_USED(state);
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

  Auser_input_mask = 0;
  outer_count       = 6;

  for (;;) {
    if (state->host_quit)
      longjmp(state->host_quit_jmp, 1);

    es_service_speech(state);
    run_script(state);
    advance_script_frame(state);

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
