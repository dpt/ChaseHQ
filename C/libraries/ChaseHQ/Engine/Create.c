/**
 * Create.c
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
#include <stdlib.h>
#include <string.h>

#include "ChaseHQ/ChaseHQ.h"

#include "ChaseHQ/Data/CommonData.h"
#include "ChaseHQ/Engine/State.h"
#include "ChaseHQ/Engine/Bank3.h"
#include "ChaseHQ/Engine/Bank7.h"
#include "ChaseHQ/Engine/Types.h"

/* ----------------------------------------------------------------------- */

/**
 * Copy read-only game tables into state and set original-game defaults.
 *
 * Conv: host lifecycle helper; has no single Z80 address (individual field
 *       defaults are marked with the Z80 address they correspond to inline
 *       below). Called once from chq_create.
 */
static void chq_initialise(chqstate_t *state)
{
  // clang-format off
  /**
   * $8C58: score_messages_template
   *
   * Conv: This is now a template copied into state.
   */
  static const u8 score_messages_template[90] = {
    5, 4, 3,
    attribute_RED_OVER_BLACK,
    Z80BACKBUF(0xF025),
    Z80ATTRS(0x5945),
    'C', 'L', 'E', 'A', 'R', ' ', 'B', 'O', 'N', 'U', 'S', ' ', ' ', ' ', ' ', ' ', ' ', '0', ',', '0', '0', '0' | EOS,
    4, 3,
    attribute_RED_OVER_BLACK,
    Z80BACKBUF(0xF065),
    Z80ATTRS(0x59C5),
    'T', 'I', 'M', 'E', ' ', 'B', 'O', 'N', 'U', 'S', ' ', ' ', ' ', ' ', ' ', ' ', 'X', ' ', '5', '0', '0', '0' | EOS,
    0x32, 3,
    attribute_RED_OVER_BLACK,
    Z80BACKBUF(0xF0A5),
    Z80ATTRS(0x5A45),
    'S', 'C', 'O', 'R', 'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' | EOS,
    3, 0
  };

  /** $8D18: continue_messages_template */
  static const u8 continue_messages_template[119] = {
    0x01, 0x01, 0x03,
    attribute_BLACK_OVER_BLACK,
    Z80BACKBUF(0xF02C),
    Z80ATTRS(0x594C),
    'C', 'O', 'N', 'T', 'I', 'N', 'U', 'E', ' ' | EOS,
    0x01, 0x03,
    attribute_BLACK_OVER_BLACK,
    Z80BACKBUF(0xF04A),
    Z80ATTRS(0x598A),
    'T', 'H', 'I', 'S', ' ', 'M', 'I', 'S', 'S', 'I', 'O', 'N' | EOS,
    0x01, 0x02,
    attribute_RED_OVER_BLACK,
    Z80BACKBUF(0xF868),
    Z80ATTRS(0x59E8),
    'P', 'U', 'S', 'H', ' ', 'G', 'E', 'A', 'R', ' ', 'B', 'U', 'T', 'T', 'O', 'N' | EOS,
    0x01, 0x02,
    attribute_RED_OVER_BLACK,
    Z80BACKBUF(0xF086),
    Z80ATTRS(0x5A06),
    'B', 'E', 'F', 'O', 'R', 'E', ' ', 'T', 'I', 'M', 'E', 'R', ' ', 'R', 'E', 'A', 'C', 'H', 'E', 'S', ' ', '0' | EOS,
    0x01, 0x02,
    attribute_BLACK_OVER_BLACK,
    Z80BACKBUF(0xF0AC),
    Z80ATTRS(0x5A4C),
    'T', 'I', 'M', 'E', ' ', '1', '0' | EOS,
    0x0A, 0x02,
    attribute_BLACK_OVER_BLACK,
    Z80BACKBUF(0xF8F6),
    Z80ATTRS(0x5AF6),
    'C', 'R', 'E', 'D', 'I', 'T', ' ', ' ' | EOS,
    0x03, 0x00
  };

  /** $9D5B: stage_n_template */
  static const u8 stage_n_template[7] = {
    'S', 'T', 'A', 'G', 'E', ' ', ' ' | EOS,
  };

  /** $CE0C: smoke_ce0c_template */
  static const u8 smoke_ce0c_template[13] = {
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  /** $CE19: smoke_ce19_template */
  static const u8 smoke_ce19_template[13] = {
    0x02, 0xF4, 0x10, 0xF8, 0x08, 0xFA, 0x08, 0xFC, 0x08, 0xFD, 0x08, 0xFE, 0x00
  };

  /** $CE26: smoke_ce26_template */
  static const u8 smoke_ce26_template[13] = {
    0x05, 0xF8, 0x20, 0xFA, 0x18, 0xFC, 0x10, 0xFE, 0x10, 0xFF, 0x08, 0x00, 0x08
  };

  /** $CE4B: debris_subtable_1_template */
  static const u8 debris_subtable_1_template[19] = {
    0x00, // counter
    0x32, 0x10, // y,x
    0x28, 0x20, 0x23, 0x30, 0x1E, 0x40, 0x23, 0x50, 0x28, 0x60, 0x32, 0x68, 0x46, 0x70, 0x5A, 0x78
  };

  /** $CE5E: debris_subtable_2_template */
  static const u8 debris_subtable_2_template[19] = {
    0x01,
    0x1E, 0x48, // y,x
    0x14, 0x50, 0x0A, 0x58, 0x0A, 0x60, 0x14, 0x68, 0x1E, 0x70, 0x2D, 0x70, 0x41, 0x78, 0x5A, 0x78
  };

  /** $CE71: debris_subtable_3_template */
  static const u8 debris_subtable_3_template[19] = {
    0x03,
    0x0A, 0x80, // y,x
    0x05, 0x80, 0x05, 0x80, 0x0A, 0x80, 0x0F, 0x80, 0x19, 0x80, 0x28, 0x80, 0x3C, 0x80, 0x5A, 0x80
  };

  /** $CE84: debris_subtable_4_template */
  static const u8 debris_subtable_4_template[19] = {
    0x01,
    0x1E, 0xB0, // y,x
    0x14, 0xA8, 0x0A, 0xA0, 0x0A, 0x98, 0x14, 0x90, 0x1E, 0x88, 0x2D, 0x88, 0x41, 0x80, 0x5A, 0x80
  };

  /** $CE97: debris_subtable_5_template */
  static const u8 debris_subtable_5_template[19] = {
    0x02,
    0x32, 0xF0, // y,x
    0x28, 0xE8, 0x23, 0xE0, 0x1E, 0xD0, 0x23, 0xC0, 0x28, 0xB0, 0x32, 0xA0, 0x46, 0x90, 0x5A, 0x80
  };

  /** $EF5E: drum1_template */
  static const u8 drum1_template[252] = {
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

  /** $F05A: drum2_template */
  static const u8 drum2_template[108] = {
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
  // clang-format on

  static const struct {
    size_t      dstoff;
    const void *src;
    size_t      n;
  } copies[] = {
    // $897C
    { offsetof(chqstate_t, sfx_crash_table), &sfx_crash_table[0], sizeof(sfx_crash_table) },
    // $8C58
    { offsetof(chqstate_t, score_messages), &score_messages_template[0], sizeof(score_messages_template) },
    // $8D18
    { offsetof(chqstate_t, continue_messages), &continue_messages_template[0], sizeof(continue_messages_template) },
    // $9D5B
    { offsetof(chqstate_t, stage_n), &stage_n_template[0], sizeof(stage_n_template) },
    // $CE0C
    { offsetof(chqstate_t, smokes[0]), &smoke_ce0c_template[0], sizeof(smoke_ce0c_template) },
    // $CE19
    { offsetof(chqstate_t, smokes[1]), &smoke_ce19_template[0], sizeof(smoke_ce19_template) },
    // $CE26
    { offsetof(chqstate_t, smokes[2]), &smoke_ce26_template[0], sizeof(smoke_ce26_template) },
    // $CE4B
    { offsetof(chqstate_t, debris_subtables[0]), &debris_subtable_1_template[0], sizeof(debris_subtable_1_template) },
    // $CE5E
    { offsetof(chqstate_t, debris_subtables[1]), &debris_subtable_2_template[0], sizeof(debris_subtable_2_template) },
    // $CE71
    { offsetof(chqstate_t, debris_subtables[2]), &debris_subtable_3_template[0], sizeof(debris_subtable_3_template) },
    // $CE84
    { offsetof(chqstate_t, debris_subtables[3]), &debris_subtable_4_template[0], sizeof(debris_subtable_4_template) },
    // $CE97
    { offsetof(chqstate_t, debris_subtables[4]), &debris_subtable_5_template[0], sizeof(debris_subtable_5_template) },
    // $EE38
    { offsetof(chqstate_t, temp_keydefs), &temp_keydefs_template[0], sizeof(temp_keydefs_template) },
    // $EF5E
    { offsetof(chqstate_t, music.drum1), &drum1_template[0], sizeof(drum1_template) },
    // $F05A
    { offsetof(chqstate_t, music.drum2), &drum2_template[0], sizeof(drum2_template) },
  };

  size_t i;

  // Copy various blocks into place in state
  for (i = 0; i < NELEMS(copies); i++)
    memcpy((char *) state + copies[i].dstoff, copies[i].src, copies[i].n);

  // $8000
  state->test_mode = 1;

  // $8007
  state->wanted_stage_number   = MINSTAGE;
  state->current_stage_number  = MINSTAGE;

  /* $8244, $8249, $8251 — engine sfx SM operands as assembled: 3 iterations
   * with zero (= 256 via DJNZ) delay counts until setup_engine_sfx_48k
   * stores real values
   */
  state->engine_sfx.nloops    = 3;
  state->engine_sfx.off_cycle = 0;
  state->engine_sfx.on_cycle  = 0;

  // $824B
  state->attract_mode_128k.blink = 0xF0;

  // $825D
  state->attract_mode_128k.countdown = 0;

  // $8277
  state->attract_blinker = 0xF0;

  // $8ABE: SM operand in handle_perp_caught, assembled as zero
  state->handle_perp_caught_delay = 0;

  // $9618
  state->rng_seed[0] = 0x7B;
  state->rng_seed[1] = 0x2D;
  state->rng_seed[2] = 0xE9;

  // $9982
  state->chatter_cursor_blink = 0xAA;

  // $A13B
  state->start_speech_cycle = 4;

  // $A240
  state->roadbufptr    = &state->road_buffer[0];   // $EE00
  state->roadbuf_start = &state->road_buffer[0];   // $EE00
  state->roadbuf_end   = &state->road_buffer[256]; // $EF00

  // $A68F (SM), $A69C (SM), $A73E (SM), $A74A (SM)
  state->pb.changing_lane     = 0;
  state->pb.lane_change_timer = 20;
  state->pb.delay             = 0;
  state->pb.approach_timer    = 20;

  // $A804
  state->sc_spawn_counter = 20;

  // $A97F/$A9DF/$C0BC: dirt-and-stones SM flags, assembled as zero
  state->ldas_enabled = 0;
  state->ddas.enabled = 0;
  state->rm.scroll_dirt_particles = 0;

  // $B395/$B3A3: SM operands (default LD DE operands) in animate_hero_car
  state->ahc.road_pos_min =  72; /* lower clamp bound */
  state->ahc.road_pos_max = 472; /* upper clamp bound */

  // $CE33
  state->debris_table[0]  = state->debris_subtables[0];
  state->debris_table[1]  = state->debris_subtables[1];
  state->debris_table[2]  = state->debris_subtables[2];
  state->debris_table[3]  = state->debris_subtables[4];
  state->debris_table[4]  = state->debris_subtables[2];
  state->debris_table[5]  = state->debris_subtables[3];
  state->debris_table[6]  = state->debris_subtables[0];
  state->debris_table[7]  = state->debris_subtables[1];
  state->debris_table[8]  = state->debris_subtables[4];
  state->debris_table[9]  = state->debris_subtables[2];
  state->debris_table[10] = state->debris_subtables[3];
  state->debris_table[11] = state->debris_subtables[0];

  // $C534/$C68A: SM operands in dr_four_lane_highway / dr_fill_left_stripe
  state->dr.left_markings_page  = 0xE8; // xpos.left page
  state->dr.left_fill_page  = 0xE8;
  state->dr.right_markings_page = 0xEC; // xpos.right page
  state->dr.right_fill_page = 0xEC;
  state->dr.neg_lane_count   = -4;   // four-lane default

  // $E300
  state->height_table[0] = 0x60; // sentinel, hardcoded in Z80 RAM

  /* $EE76/$EEBF/$EECA: music driver SM pointer operands. The Z80 assembles
   * all three as $0000 — harmless there (a stray read lands in ROM) but a
   * crash as NULL in C — so point them at the starts of the pattern table
   * and music data instead. pattern_start_ptr skips music_data's leading
   * per-pattern delay byte, preserving the driver's invariant that it never
   * points at a delay byte.
   */
  state->music.pattern_addr      = &music_patterns[0]; // $F0FE
  state->music.data_ptr          = &music_data[1];
  state->music.pattern_start_ptr = &music_data[1];     // $F112

  /* Conv: keydefs[] is installed by the options menu ($FBE5). chq_bank3_create
   * seeds it with the same pristine defaults the Z80 holds at $FFF7, so the
   * stubbed BANK3_INPUT_SELECTION path still gets the original key layout.
   */
  state->kempston_flag = 0;
}

/* ----------------------------------------------------------------------- */

/**
 * Allocate and initialise a new game state.
 *
 * Conv: host lifecycle entry point; has no Z80 address. Allocates chqstate_t,
 *       wires up the ZX facade, runs chq_initialise and creates the bank 3/7
 *       sub-states.
 *
 * \param[in] speccy ZX Spectrum facade to attach to the new state.
 * \return           Newly allocated state, or NULL on allocation failure.
 */
CHQ_API chqstate_t *chq_create(zxspectrum_t *speccy)
{
  chqstate_t *state = NULL;

  /* Allocate state structure. */

  state = calloc(1, sizeof(*state));
  if (state == NULL)
    goto failure;

  /* Initialise additional variables. */

  state->speccy = speccy;

  /* Initialise original game variables. */

  chq_initialise(state);

  if (bank3_state_create(state) < 0)
    goto failure;

  if (bank7_state_create(state) < 0)
    goto failure;

  return state;


failure:

  free(state);

  return NULL;
}

/**
 * Free a game state created by chq_create.
 *
 * Conv: host lifecycle entry point; has no Z80 address. Tolerates a NULL state.
 */
CHQ_API void chq_destroy(chqstate_t *state)
{
  if (state == NULL)
    return;

  bank3_state_destroy(state);
  bank7_state_destroy(state);

  free(state);
}
