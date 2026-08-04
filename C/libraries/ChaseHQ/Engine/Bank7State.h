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
  /* Per-instance copy of Bank7.c's static const script_data, made at
   * show_end_screen entry so es_handler_draw_score's in-place score patch
   * (offset 0xFD) cannot trample, or be trampled by, another concurrently
   * running game instance sharing the same process.
   */
  u8           es_script[268];

  /* $A16D (SM): run_script's script program counter. Points into es_script
   * above (Bank7.c).
   */
  const u8    *es_script_ptr;

  /* $A170 (SM): per-command frame-delay countdown; show_end_screen's loop
   * calls run_script when this reaches zero, then always invokes es_handler.
   */
  u8           es_frame_count;

  /* $5C31-$5C32 (SM): the per-frame draw handler, self-modified by
   * run_script's rs_exit tail ($E2D4 LD ($5C31),DE) each time a command hands
   * off to a delayed handler instead of running immediately.
   */
  void       (*es_handler)(chqstate_t *state);

  /* $A16F: dual-purpose fire-button/tally-done flag. show_end_screen's loop
   * uses it as a 0/1 "has the first fire press been consumed" latch;
   * es_handler_render_score unconditionally sets it to 1 once the score
   * tally finishes, so the very next fire press ends the end screen instead
   * of merely arming the exit (confirmed via skool cross-reference: $E017
   * clears it, $E045/$E04C are show_end_screen's own mask logic, $E272 is
   * render_score's unconditional set).
   */
  u8           es_input_mask;

  /* $5C6C (SM): GLYPH_A/GLYPH_B flip-flop cadence gate, rotated (RLC) once
   * per call by both routine_e42e (GLYPH_A) and routine_e472 (GLYPH_B, also
   * called by routine_e3b7's handshake handler before its own animation
   * logic).
   */
  u8           es_fade_gate_ab;

  /* $5C6D (SM): GLYPH_C flip-flop cadence gate, rotated (RLC) once per call
   * by both routine_e46d (GLYPH_C) and routine_e3b7 (HANDSHAKE) itself, where
   * it also gates the animation-advance block.
   */
  u8           es_fade_gate_c;

  /* $A172 (SM): handshake animation frame index (0..5), advanced and
   * wrapped by routine_e3b7 each call; indexes handshake_table.
   */
  u8           es_handshake_index;

  /* Bank 7's own copy of the 48K music engine's playback state (SM fields at
   * $F311/$F318/$F34B/$F356/$F362/$F372/$F3A9/$F3B6 in the relocated $F300
   * buffer), driving es_music_patterns/es_music_data via
   * es_play_music_48k. Same shape as chqstate_t's own "music" struct
   * (State.h) -- a separate instance because bank 7 pages in its own copy of
   * the engine rather than sharing the in-game one.
   */
  struct {
    u8        pattern_repeats; // (SM) $F311: pattern repeat counter
    const u8 *pattern_addr;    // (SM) $F318: current pattern address
    u8        started;         // (SM) $F34B
    u8        note_delay;      // (SM) $F356
    u8        note_delay_reload; // (SM) $F362
    const u8 *data_ptr;        // (SM) $F372
    const u8 *pattern_start_ptr;
    u8        extra_delay;     // (SM) $F3A9
    u8        drum_active;     // (SM) $F3B6: drum playing flag
    u8        drum_speed;      // (SM): inner loop count per sample byte
  } es_music;

  /* Mutable per-instance copies of es_drum_sample_2_template/es_drum_sample_1_template
   * (Bank7.c), played by es_playdrum_2/es_playdrum_1 (Bank7.c). Mutable
   * because playdrum_go's RLC rotates each sample byte in place during
   * playback, matching the same pattern chqstate_t uses for its own
   * music.drum1/music.drum2 (State.h).
   */
  u8           es_drum2[94];
  u8           es_drum1[160];
};

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK7STATE_H */
