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
    { offsetof(chqstate_t, drum1), &drum1_template[0], sizeof(drum1_template) },
    // $F05A
    { offsetof(chqstate_t, drum2), &drum2_template[0], sizeof(drum2_template) },
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

  // $8244, $8249, $8251 — engine sfx SM operands as assembled: 3 iterations
  // with zero (= 256 via DJNZ) delay counts until setup_engine_sfx_48k
  // stores real values
  state->engine_sfx_nloops    = 3;
  state->engine_sfx_off_cycle = 0;
  state->engine_sfx_on_cycle  = 0;

  // $824B
  state->attract_mode_128k_blink = 0xF0;

  // $825D
  state->attract_mode_128k_countdown = 0;

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
  state->pb_changing_lane     = 0;
  state->pb_lane_change_timer = 20;
  state->pb_delay             = 0;
  state->pb_approach_timer    = 20;

  // $A804
  state->sc_spawn_counter = 20;

  // $A97F/$A9DF/$C0BC: dirt-and-stones SM flags, assembled as zero
  state->ldas_enabled = 0;
  state->ddas_enabled = 0;
  state->rm_scroll_dirt_particles = 0;

  // $B395/$B3A3: SM operands (default LD DE operands) in animate_hero_car
  state->ahc_road_pos_min =  72; /* lower clamp bound */
  state->ahc_road_pos_max = 472; /* upper clamp bound */

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
  state->dr_left_markings_page  = 0xE8; // xpos_road_left page
  state->dr_left_fill_page  = 0xE8;
  state->dr_right_markings_page = 0xEC; // xpos_road_right page
  state->dr_right_fill_page = 0xEC;
  state->dr_neg_lane_count   = -4;   // four-lane default

  // $E300
  state->height_table[0] = 0x60; // sentinel, hardcoded in Z80 RAM

  // $EE76/$EEBF/$EECA: music driver SM pointer operands. The Z80 assembles
  // all three as $0000 — harmless there (a stray read lands in ROM) but a
  // crash as NULL in C — so point them at the starts of the pattern table
  // and music data instead. pattern_start_ptr skips music_data's leading
  // per-pattern delay byte, preserving the driver's invariant that it never
  // points at a delay byte.
  state->music.pattern_addr      = &music_patterns[0]; // $F0FE
  state->music.data_ptr          = &music_data[1];
  state->music.pattern_start_ptr = &music_data[1];     // $F112

  // Conv: keydefs[] is installed by the options menu ($FBE5). chq_bank3_create
  // seeds it with the same pristine defaults the Z80 holds at $FFF7, so the
  // stubbed BANK3_INPUT_SELECTION path still gets the original key layout.
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
