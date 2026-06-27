/**
 * ChaseHQ-Create.c
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
 * The original game and design is copyright (c) 1988 Taito Corporation
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited
 * The recreated version is copyright (c) 2023-2026 David Thomas
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ChaseHQ/ChaseHQ.h"

#include "ChaseHQ/Data/ChaseHQ-CommonData.h"
#include "ChaseHQ/ChaseHQ-State.h"

/* ----------------------------------------------------------------------- */

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
    // memcpy(&state->continue_messages[0], &continue_messages[0], sizeof(continue_messages));
    // $8D77
    { offsetof(chqstate_t, time_nn), "TIME 1\xB0", 7 },
    // $8D85
    { offsetof(chqstate_t, credit_n), "CREDIT \xA0", 8 },
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
  };

  int i;

  // Copy various blocks into place in state
  for (i = 0; i < NELEMS(copies); i++)
    memcpy((char *) state + copies[i].dstoff, copies[i].src, copies[i].n);

  // $8007
  state->wanted_stage_number   = 1;
  state->current_stage_number  = 1;

  // $824B
  state->attract_mode_128k_blink = 0xF0;

  // $825D
  state->attract_mode_128k_countdown = 0;

  // $8277
  state->attract_blinker = 0xF0;

  // $9618
  state->rng_seed[0] = 0x7B;
  state->rng_seed[1] = 0x2D;
  state->rng_seed[2] = 0xE9;

  // $A13B
  state->start_speech_cycle = 4;

  // $A240
  state->roadbufptr = &state->road_buffer[0];   // $EE00
  state->roadbuf_start  = &state->road_buffer[0];   // $EE00
  state->roadbuf_end    = &state->road_buffer[256]; // $EF00

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
  state->dr_left_table_hi_1  = 0xE8; // xpos_road_left page
  state->dr_left_table_hi_2  = 0xE8;
  state->dr_right_table_hi_1 = 0xEC; // xpos_road_right page
  state->dr_right_table_hi_2 = 0xEC;
  state->dr_neg_lane_count   = -4;   // four-lane default

  // $E300
  state->height_table[0] = 0x60; // sentinel, hardcoded in Z80 RAM

  /* $EF00: Build bit-reversal lookup table (done in bootstrap() in the full game) */
  {
    int i;
    for (i = 0; i < 256; i++) {
      u8 v, r;
      int b;
      v = (u8) i;
      r = 0;
      for (b = 0; b < 8; b++) {
        r = (u8) ((r << 1) | (v & 1));
        v >>= 1;
      }
      state->flipped[i] = r;
    }
  }
}

/* ----------------------------------------------------------------------- */

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

  return state;


failure:

  free(state);

  return NULL;
}

CHQ_API void chq_destroy(chqstate_t *state)
{
  if (state == NULL)
    return;

  free(state);
}
