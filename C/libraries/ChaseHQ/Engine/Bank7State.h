/**
 * Bank7State.h
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
 *
 * -----------------------------------------------------------------------
 *
 * Full definition of chqstate's opaque bank7 pointer (State.h only forward
 * declares struct chq_bank7_state). Private to the 128K bank 7 end-screen
 * sequence -- only Bank7.c includes this header, so no other translation
 * unit can see or touch these fields directly.
 */

#ifndef CHASEHQ_BANK7STATE_H
#define CHASEHQ_BANK7STATE_H

#include "C99/Types.h"

#include "State.h"

/* ----------------------------------------------------------------------- */

/**
 * State private to the 128K bank 7 end-screen script interpreter.
 * chqstate_t only holds a pointer to this (see State.h); the fields below
 * are reachable only from within Bank7.c.
 */
struct chq_bank7_state {
  // $A16D (SM): run_script's script program counter. Points at script_data
  // (Bank7.c).
  const u8    *es_script_ptr;

  // $A170 (SM): per-command frame-delay countdown; show_end_screen's loop
  // calls run_script when this reaches zero, then always invokes es_handler.
  u8           es_frame_count;

  // $5C31-$5C32 (SM): the per-frame draw handler, self-modified by
  // run_script's rs_exit tail ($E2D4 LD ($5C31),DE) each time a command hands
  // off to a delayed handler instead of running immediately.
  void       (*es_handler)(chqstate_t *state);

  // $A16F: dual-purpose fire-button/tally-done flag. show_end_screen's loop
  // uses it as a 0/1 "has the first fire press been consumed" latch;
  // es_handler_render_score unconditionally sets it to 1 once the score
  // tally finishes, so the very next fire press ends the end screen instead
  // of merely arming the exit (confirmed via skool cross-reference: $E017
  // clears it, $E045/$E04C are show_end_screen's own mask logic, $E272 is
  // render_score's unconditional set).
  u8           es_input_mask;
};

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK7STATE_H */
