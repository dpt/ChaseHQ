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
 * The original game and design is copyright (c) 1988 Taito Corporation
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited
 * The recreated version is copyright (c) 2023-2026 David Thomas
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ChaseHQ/ChaseHQ.h"

#include "ChaseHQ/Data/CommonData.h"
#include "ChaseHQ/Engine/State.h"

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
  state->wanted_stage_number   = 1;
  state->current_stage_number  = 1;

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
  state->ahc_road_pos_a =  72; /* lower clamp bound */
  state->ahc_road_pos_b = 472; /* upper clamp bound */

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

  // $EE76/$EEBF/$EECA: music driver SM pointer operands. The Z80 assembles
  // all three as $0000 — harmless there (a stray read lands in ROM) but a
  // crash as NULL in C — so point them at the starts of the pattern table
  // and music data instead. pattern_start_ptr skips music_data's leading
  // per-pattern delay byte, preserving the driver's invariant that it never
  // points at a delay byte.
  state->music.pattern_addr      = &music_patterns[0]; // $F0FE
  state->music.data_ptr          = &music_data[1];
  state->music.pattern_start_ptr = &music_data[1];     // $F112

  // $EC01/$EC26/$EC4B (128K bank 3): title-tune channel-tracker records.
  // start_tune ($EB9E) properly initialises these once a tune is selected,
  // but compute_channel_ay_registers ($EE9E) can in principle run before
  // that, so give every field a defined value here rather than leaving
  // calloc's zero fill as the only guarantee. pattern_ptr/pattern_data_ptr
  // and the Conv-only pattern_base/pattern_len stay NULL/0 here; start_tune
  // resolves them for real from tune_patterns (tunes 0/1 only -- see
  // start_tune's Translation notes). pitch_offset_default/_cur and
  // envelope_shape_default/_ptr are set by decode_pattern_command's
  // as-yet-unported pitch-offset/envelope-shape select tables, not by
  // start_tune.
  {
    int titlechan;

    for (titlechan = 0; titlechan < 3; titlechan++) {
      state->title_music.channel[titlechan].status                 = 0;
      state->title_music.channel[titlechan].pattern_ptr            = NULL; // set for real by start_tune
      state->title_music.channel[titlechan].pattern_data_ptr       = NULL; // never read elsewhere in bank 3
      state->title_music.channel[titlechan].pattern_base           = NULL; // Conv: set for real by start_tune
      state->title_music.channel[titlechan].pattern_len            = 0;    // Conv: set for real by start_tune
      state->title_music.channel[titlechan].speed_divider          = 0;
      state->title_music.channel[titlechan].counter                = 0;
      state->title_music.channel[titlechan].slide_accum            = 0;
      state->title_music.channel[titlechan].pitch_offset_default   = NULL; // TODO: set once decode_pattern_command's pitch-offset table exists
      state->title_music.channel[titlechan].pitch_offset_cur       = NULL; // TODO: as above
      state->title_music.channel[titlechan].slide_step             = 0;
      state->title_music.channel[titlechan].slide_countdown        = 0;
      state->title_music.channel[titlechan].envelope_speed         = 0;
      state->title_music.channel[titlechan].row_wait               = 0;
      state->title_music.channel[titlechan].row_wait_reload        = 0;
      state->title_music.channel[titlechan].note_index             = 0;
      state->title_music.channel[titlechan].volume                 = 0;
      state->title_music.channel[titlechan].envelope_shape_default = NULL; // TODO: set once decode_pattern_command's envelope-shape table exists
      state->title_music.channel[titlechan].envelope_shape_ptr     = NULL; // TODO: as above
      state->title_music.channel[titlechan].envelope_amplitude     = 0;
      state->title_music.channel[titlechan].envelope_step_counter  = 0;
      state->title_music.channel[titlechan].vibrato_depth          = 0;
      state->title_music.channel[titlechan].vibrato_increment      = 0;
      state->title_music.channel[titlechan].vibrato_phase          = 0;
      state->title_music.channel[titlechan].flags                  = 0;
      state->title_music.channel[titlechan].slide_update_flag      = 0;
      state->title_music.channel[titlechan].mute_pending           = 0;
      state->title_music.channel[titlechan].transpose              = 0;
      state->title_music.channel[titlechan].misc_playback_state    = 0;
      /* mixer_mask is static initial RAM content in the Z80 (not written by
       * start_tune), confirmed against the skool's DEFB data: channel-tracker
       * $EC01 -> $EC25=$09, $EC26 -> $EC4A=$12, $EC4B -> $EC6F=$24. */
      switch (titlechan) {
      case 0:
        state->title_music.channel[titlechan].mixer_mask = 0x09;
        break;
      case 1:
        state->title_music.channel[titlechan].mixer_mask = 0x12;
        break;
      case 2:
        state->title_music.channel[titlechan].mixer_mask = 0x24;
        break;
      }
    }
  }

  // $EC70 (128K bank 3): per-tick tempo countdown; start_tune sets this to 1
  // on tune start, so 0 here is just a harmless pre-tune-start default (the
  // tune-active flag being clear means ts_music_service never reads it
  // before then).
  state->title_music.tempo_counter = 0x00; // $EC70

  // $EC79/$ECC6 (SM, 128K bank 3): scratch bytes are the operands of
  // "LD A,$00" instructions in ts_music_service, so 0 is the assembled
  // reset value for both.
  state->title_music.shared_note_value    = 0x00; // $EC79
  state->title_music.driver_internal_flag = 0x00; // $ECC6

  // $EC9A/$EED1/$EF7A (128K bank 3): further scratch bytes written by
  // start_tune/advance_channel_pattern; 0 matches start_tune's explicit
  // clear of $EED1 and is the natural power-on state of the other two.
  state->title_music.tune_tempo         = 0x00; // $EC9A
  state->title_music.pattern_driver_flag = 0x00; // $EED1
  state->title_music.pending_mixer_bits  = 0x00; // $EF7A

  // $F223/$F224 (128K bank 3): tune-active flag and its companion byte.
  // start_tune clears both on entry and arms tune_active=1 once a tune's
  // channel-tracker records are initialised; 0 here matches the power-on
  // state (no tune active) before start_tune is ever called.
  state->title_music.tune_active           = 0x00; // $F223
  state->title_music.tune_active_companion = 0x00; // $F224

  // $EFAF-$EFBA (128K bank 3): title-tune AY register cache. Matches the
  // skool's DEFB bytes at $EFAF-$EFBA exactly; per the skool comment these
  // are placeholder start-up defaults, overwritten every frame once a tune
  // is playing (mixer = 0x3F disables all tone/noise channels until then).
  state->title_ay_regs.chan_a_pitch = 0x0000;
  state->title_ay_regs.chan_b_pitch = 0x0000;
  state->title_ay_regs.chan_c_pitch = 0x0000;
  state->title_ay_regs.noise_pitch  = 0x00;
  state->title_ay_regs.mixer        = 0x3F;
  state->title_ay_regs.chan_a_vol   = 0x0F;
  state->title_ay_regs.chan_b_vol   = 0x0F;
  state->title_ay_regs.chan_c_vol   = 0x0F;
  state->title_ay_regs.env_fine     = 0x00;

  // $C5A2 (SM): pristine operand value for the "LD A,$00" self-modified by
  // title_screen_driver; rotates/increments on each restart.
  state->title_animation = 0x00;

  // $BB00-$BB4F (128K bank 3): title-screen animated-object array, repopulated
  // from a scene table on every title_screen_driver restart; zeroed here so an
  // object drawn before the first restart (should never happen) is inert.
  {
    int titleobj;

    for (titleobj = 0; titleobj < 9; titleobj++) {
      state->title_objects[titleobj].opcode = 0x00;
      state->title_objects[titleobj].wait   = 0x00;
      state->title_objects[titleobj].x_step = 0;
      state->title_objects[titleobj].y_step = 0;
      state->title_objects[titleobj].script = NULL;
      state->title_objects[titleobj].row    = 0x00;
      state->title_objects[titleobj].x      = 0x00;
      state->title_objects[titleobj].y      = 0x00;
    }
  }

  // Temp until the 128K input code is ported.
  state->kempston_flag = 0;
  state->keydefs[KEYDEF_QUIT      ] = KEYDEF(4,3); // 0
  state->keydefs[KEYDEF_PAUSE     ] = KEYDEF(4,4); // 1
  state->keydefs[KEYDEF_BOOST     ] = KEYDEF(4,0); // SPACE
  state->keydefs[KEYDEF_GEAR      ] = KEYDEF(2,0); // M
  state->keydefs[KEYDEF_ACCELERATE] = KEYDEF(4,5); // Q
  state->keydefs[KEYDEF_BRAKE     ] = KEYDEF(4,6); // A
  state->keydefs[KEYDEF_LEFT      ] = KEYDEF(3,2); // O
  state->keydefs[KEYDEF_RIGHT     ] = KEYDEF(4,2); // P
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
