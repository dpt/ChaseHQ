/**
 * State.h
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

#ifndef CHASEHQ_STATE_H
#define CHASEHQ_STATE_H

#include <setjmp.h>

#include "C99/Types.h"
#include "ZXSpectrum/Spectrum.h"

#include "ChaseHQ/ChaseHQ.h"
#include "ChaseHQ/Data/Stages.h"

/* ----------------------------------------------------------------------- */

typedef void (dr_callback_t)(chqstate_t *state, int Bfill_pattern, int Chorizon, int DEscreen_ptr, int Lrow, u8 **IXlanesptr, const u8 **IYheightptr);

/* Returns the back-buffer address advanced past the rows just drawn: on the
 * Z80 this value survives in the shadow HL' register across the repeat
 * loop's CALLs; the C translation must pass it back explicitly instead. */
typedef u8 *(plot_sprite_cb_t)(chqstate_t *state,
                               int         IXjump_offset,
                               u8         *HLbackbuf_addr,
                               int         Bdash_height,
                               int         DEdash_bitmap_stride,
                               const u8   *HLdash_bitmap_data);

/// A hazard in Chase HQ is something that's on the road. It might be a moving
/// object like the perp or NPC cars, or a fixed item like a barrier or a
/// tumbleweed.
struct hazard {
  u8                used;              // HAZARD_USED (0xFF) or HAZARD_UNUSED (0x00)
  u8                distance;          // approach counter (21..0); also reused as road-col low byte during draw
  u8                horz_pos;          // horizontal screen position
  s8                horz_clip;         // high byte of computed road X: 0=on screen, <0=clip left, >0=clip right
  u8                dist_frac;         // fixed-point fractional distance; decremented by speed, carry advances distance
  u8                horz_pos_on_road;  // lane/road position (0..255 across road width)
  u8                persp_col;         // perspective-scaled column: (dist_frac * scale) >> 8; used for sprite column and road-edge row lookup
  s8                hit_timer;         // hit sequence timer: 0=clear, >0=vehicle hit in progress, <0=perp hit cooldown ($FC=-4, counts to 0)
  hittable_t        hittable;
  hazard_handler_t *hit_handler;
  u16               speed;             // fixed-point approach rate: high byte = whole distance units/frame added to distance counter; low byte = fractional units/frame subtracted from dist_frac (carry advances distance)
  u8                hazard_flags;      // 0x80=spawned vehicle; 0xFF=perp car; 1/2=post-hit damage state; bit 7 = is vehicle
  u8                hit_wobble;        // horizontal wobble offset from hit animation table (table_acdb), subtracted from persp_col
  u8                hazard_lane_OR_perp_dist_hi; // perp: high byte of distance; hazard: current lane index
  u8                current_lane;      // target lane (counts down to 0 during lane-change animation)
  u8                inverted;          // sprite plot mode: 0=normal, 1=inverted
};

struct session {
  // $A16D
  u8        spawn_accumulator;
  // $A16E
  u8        idle_timer;
  // $A16F
  u8        user_input_mask;
  // $A170
  u8        turbos;
  // $A171
  u16       horizon_level;
  // $A173
  u8        perp_halt_counter;
  // $A174
  u8        displayed_gear;
  // $A175
  u8        score_digits[8];
  // $A17D
  u8        subsecond_ticks;
  // $A17E
  u8        time_bcd;
  // $A17F
  u8        time_digits[2];
  // $A181
  u8        distance_digits[4];
  // $A185
  u8        no_objects_flag; // 1 (default) or 2 (don't spawn objects or hazards)
  // $A186
  u16       horizon_attribute; // Z80 address
};

/// One 37-byte channel-tracker record used by the 128K bank-3 title-tune
/// engine ($EC01/$EC26/$EC4B). Offsets read/written by
/// compute_channel_ay_registers ($EE9E), advance_channel_pattern ($EDD6) and
/// start_tune ($EB9E) are modelled. Offsets $22/$23 are not written by
/// start_tune either, and are not referenced anywhere else in bank 3 (checked
/// exhaustively) -- most likely unused padding in the 37-byte record. Do not
/// invent their layout.
struct title_tune_channel {
  u8         status;                // +$00 note/status; bit0 toggled every call, bit1 set by pcmd_set_status_bit1, bit2 slide active, bit3 slide direction/upkeep gate, bit5 envelope active, bits 3&7 set by pcmd_set_status_bits_3_7
  const u8  *pattern_ptr;           // +$01/+$02 current read position in the pattern-command byte stream; initialised by start_tune from the first 2 bytes of the pattern-data block that pattern_data_ptr points to (an "envelope-pointer header")
  const u8  *pattern_data_ptr;      // +$03/+$04 raw pattern-data block pointer for this channel, read from the tune-select table by start_tune; never read elsewhere in bank 3
  const u8  *pattern_base;          // Conv: start of the extracted pattern_ptr array; not a Z80 field. Lets advance_channel_pattern wrap pattern_ptr back to the start once it runs off the end of the finite extracted prefix, since the real Z80 data (and its true loop point) is not fully transcribed into C
  u16        pattern_len;           // Conv: byte length of the array pattern_base points to; paired with pattern_base for the same reason
  u8         speed_divider;         // +$05 initial speed/divider value, reset to 2 by start_tune; never read elsewhere in bank 3 (purpose beyond initialisation not established)
  u8         counter;               // +$06 counter, reset to 0 by start_tune; never read elsewhere in bank 3 (purpose beyond initialisation not established)
  u16        slide_accum;           // +$07/+$08 accumulated portamento/slide value
  const u8  *pitch_offset_default;  // +$09/+$0A default/loop-start pitch-offset sequence pointer
  const u8  *pitch_offset_cur;      // +$0B/+$0C current pitch-offset sequence pointer
  s8         slide_step;            // +$0D signed per-tick portamento step
  u8         slide_countdown;       // +$0E portamento reload countdown
  u8         envelope_speed;        // +$0F envelope-step reload value
  u8         row_wait;              // +$10 per-row wait countdown; also doubles as the channel enable flag (start_tune sets it to 1)
  u8         row_wait_reload;       // +$11 reload value for row_wait, set by the row-duration pattern command
  u8         note_index;            // +$12 current note index (post-transpose)
  u8         volume;                // +$13 volume/envelope amplitude; returned to caller
  const u8  *envelope_shape_default; // +$14/$15 default/base envelope-shape table pointer, reloaded into envelope_shape_ptr on every note
  const u8  *envelope_shape_ptr;    // +$16/+$17 envelope shape table pointer
  u8         envelope_amplitude;    // +$18 current envelope amplitude
  u8         envelope_step_counter; // +$19 envelope-step counter; 0 = due for reload
  u8         vibrato_depth;         // +$1A vibrato depth * 2
  u8         vibrato_increment;     // +$1B vibrato per-tick increment
  u8         vibrato_phase;         // +$1C vibrato triangle-wave phase counter
  u8         flags;                 // +$1D bit5 vibrato direction, bit6 vibrato enable, bit7 vibrato update gate
  u8         slide_update_flag;     // +$1E bit0 gates whether a new note is echoed to title_music.shared_note_value; set/cleared by the mixer-bit pattern commands
  u8         mute_pending;          // +$1F bit7 = one-shot mute-transition gate
  u8         transpose;             // +$20 added to each raw note value read from the pattern stream before storing to note_index
  u8         misc_playback_state;   // +$21 write-only; reset to 0 alongside transpose by start_tune ("reset misc playback state for this channel"); never read elsewhere in bank 3; purpose not established
  u8         mixer_mask;            // +$24 mask applied when merging into the shared mixer cache
};

/// One 9-byte animated-object record used by the 128K bank-3 title screen
/// ($BB00-$BB4F, 9 records). Populated from a scene table by
/// title_screen_driver, drawn each frame by ts_animate_frame, and advanced
/// by object_script_step ($C705).
struct title_object {
  u8         opcode;  // +$00 active movement-mode opcode, or 0 (idle: fetch next script opcode)
  u8         wait;    // +$01 "wait N frames" countdown, also reused as the decel/accel countdown
  s8         x_step;  // +$02 X velocity/step
  s8         y_step;  // +$03 Y velocity/step
  const u8  *script;  // +$04/+$05 script byte-code cursor
  u8         row;     // +$06 screen row/height byte consumed by the blitters
  u8         x;       // +$07 current X screen position
  u8         y;       // +$08 current Y screen position
};

/* ----------------------------------------------------------------------- */

/**
 * Holds the current state of the game.
 */
struct chqstate {
  /* ------------------------------------------------------------------------
   * State variables additional to the original game.
   * --------------------------------------------------------------------- */

  /**
   * Virtual ZX Spectrum hardware we're driving.
   */
  zxspectrum_t *speccy;

  jmp_buf      host_quit_jmp;
  volatile int host_quit;

  /* ------------------------------------------------------------------------
   * State variables as per the original, ordered by memory location.
   * --------------------------------------------------------------------- */

  // $5B00
  u8        pre_shifted_backdrop[BACKDROP_LENGTH]; // occupies 256 bytes in real game?

  // $8000
  u8        test_mode;
  // $8001
  u8        controls_selected;
  // $8002
  u8        score_bcd[4];
  // $8006
  u8        retry_count;
  // $8007
  u8        wanted_stage_number;

  // $5C00..$76EF
  const stage_t *stage;

  // $8066 (SM) in play_siren_sfx_128k (relocated)
  u8        siren_pattern;

  // $823B (SM) in play_engine_sfx_48k
  u8        engine_sfx_counter;
  // $8244 (SM) in play_engine_sfx_48k
  u8        engine_sfx_nloops;
  // $8249 (SM) in play_engine_sfx_48k
  u8        engine_sfx_off_cycle;
  // $8251 (SM) in play_engine_sfx_48k
  u8        engine_sfx_on_cycle;

  // $824B (SM) in attract_mode_128k (relocated)
  u8        attract_mode_128k_blink;
  // $825D (SM) in attract_mode_128k (relocated)
  s8        attract_mode_128k_countdown;

  // $8277 (SM) in attract_mode
  u8        attract_blinker;

  // $85EB (SM) in reveal_perp_car
  u8        pregame_car_revealed_height;

  // $8614 (SM) in animate_meters
  s8        meter_1_level;
  // $8631 (SM) in animate_meters
  s8        meter_2_level;

  // $86C3 (SM) in draw_pregame
  u8        draw_pregame_background;
  // $86C9 (SM) in draw_pregame
  u8        draw_pregame_direction;

  // $897C
  u8        sfx_crash_table[93];

  // $8A0F (SM) in sfx_cornering
  u8        sfx_cornering_toggle;

  // $8ABE (SM) in handle_perp_caught
  u8        handle_perp_caught_delay;

#define SCORE_MESSAGES_BASE         (0x8C58)
#define SCORE_MESSAGES_LENGTH       (0x8CB2 - SCORE_MESSAGES_BASE)
  // $8C58
  u8        score_messages[SCORE_MESSAGES_LENGTH];

#define CONTINUE_MESSAGES_BASE      (0x8D18)
#define CONTINUE_MESSAGES_LENGTH    (0x8D8F - CONTINUE_MESSAGES_BASE)
#define CONTINUE_MESSAGES_TIME_NN   (0x8D7C - CONTINUE_MESSAGES_BASE)
#define CONTINUE_MESSAGES_CREDIT_N  (0x8D8C - CONTINUE_MESSAGES_BASE)
  // $8D18
  u8        continue_messages[CONTINUE_MESSAGES_LENGTH];

  // $8DA1 (SM) in transition
  u8        transition_nframes;
  // $8DB1 (SM) in transition
  s16       transition_frame_stride; // fade step value: 8 or -8
  // $8DBB (SM) in transition
  const u8 *transition_mask;

  // $8E43 (SM) in draw_overlay_messages
  const u8 *overlay_message;
  // $8E46 (SM) in draw_overlay_messages
  u8        overlay_count;
  // $8E49 (SM) in draw_overlay_messages
  u8        overlay_delay;

  // $8F82 (SM) in draw_everything_else
  u8        dee_draw_tunnel_1;
  // $8FA4 (SM) in draw_everything_else
  u8        dee_draw_helicopter;
  // $8FA7 (SM) in draw_everything_else
  u8        dee_draw_tunnel_2;

  // $90F1 (SM) in draw_overhead
  u8        do_vert_sub;
  // $9115 (SM) in draw_overhead
  u8        do_span_width_words;

  // $9395 (SM) in draw_object_common
  u8        doc_shift_select;
  // $933D (SM) in draw_object_common
  s8        doc_col_pos;
  // $93C0 (SM) in draw_object_common
  // Dispatch selector at $93B4 in draw_object_common: 0 = normal plot;
  // 1 = vertically-inverted plot (hazard hit-wobble flip, e.g. barriers);
  // 2 = multi-section column plot (draw_stretchy_object_common's single-
  // pixel-column case for poles/signs); unrelated to inversion despite
  // sharing this field.
  u8        doc_plot_mode;
  // $9404 (SM) in draw_object_common
  u8        doc_rows_main;
  // $940F (SM) in draw_object_common
  plot_sprite_cb_t *doc_plot_fn;
  // $9412 (SM) in draw_object_common
  const u8 *doc_bitmap_ptr;
  // $9415 (SM) in draw_object_common
  u8        doc_rows_2nd;
  // $941D (SM) in draw_object_common
  plot_sprite_cb_t *doc_plot_fn_2;
  // $945F (SM) in draw_object_common
  u8       doc_mask_rows_main;
  // $946C (SM) in draw_object_common
  const u8 *doc_mask_bitmap_ptr; // bitmap data ptr
  // $946F (SM) in draw_object_common
  u8       doc_mask_rows_2nd;

  // $9618
  u8        rng_seed[3];

  // $962E
  const char *next_character;
  // $9630
  const u8 *chatterblk_ptr; // current chatter block byte
  // $9632
  u8        message_x;
  // $9633
  u8        chatter_delay;
  // $9634
  u8        noise_bytes[5];

  // $963C
  u8        noise_counter;
  // $963D
  u8        chatter_state;
  // $963E
  chatterpriority_t chatter_priority;

  // $9982 (SM)
  u8        chatter_cursor_blink;

  // $9C85 (SM) in tick
  u8        tick_remaining_seconds_x2; // seconds, doubled // Conv: 16-bit load was split
  // $9C86 (SM) in tick
  u8        tick_remaining_subseconds; // delay factor - approx sixths of a second // Conv: 16-bit load was split

  // $9D51
  char      bonus_string[6];
  // $9D5B
  u8        stage_n[7];

  // $9D9B
  const u8 *SM_address_of_score_digits;

  // $9E22 (SM) in plot_turbos_and_scores
  u8        turbo_spin_frame;

  // $A0CC
  u8        kempston_flag;
  // $A0CD
  u8        keydefs[8]; // TODO add symbols for these
  // $A0D5
  u8        user_input;

  // $A139
  u8        mode_128k;
  // $A13A
  u8        current_stage_number; // init to 1
  // $A13B
  u8        start_speech_cycle; // init to 4
  // $A13C
  u8        overtake_bonus_bcd;
  // $A13D
  u8        credits;

  // $A16D
  session_t session;

  // $A188
  hazard_t  hazards[MAXHAZARDS];

  // $A213
  u16       ay_chan_a_pitch;
  u16       ay_chan_b_pitch;
  u16       ay_chan_c_pitch;
  u8        ay_noise_pitch;
  u8        ay_mixer;
  u8        ay_chan_a_vol;
  u8        ay_chan_b_vol;
  u8        ay_chan_c_vol;
  u8        ay_env_fine;

  // $A220
  u8        dont_draw_screen_attrs;
  // $A221
  u8        inhibit_collision_detection;
  // $A222
  u8        n_hazards;
  // $A223
  u8        displayed_stage;
  // $A224
  u8        helicopter_control;
  // $A225
  u8        dont_spawn_cars;
  // $A226
  u8        correct_fork;
  // $A227
  u8        floating_arrow;
  // $A228
  u8        cherry_light;
  // $A229
  u8        time_up_state;
  // $A22A
  u8        car_y;
  // $A22B
  u8        overtake_bonus_counter;
  // $A22C
  u8        trigger_bonus_flag;
  // $A22D
  u8        bonus_counter;
  // $A22E
  u8        sighted_flag;
  // $A22F
  u8        hand_flag;
  // $A230
  u8        perp_caught_phase;
  // $A231
  u8        transition_control;
  // $A232
  u8        smash_level;
  // $A233
  u8        smash_counter;
  // $A234
  u8        anim_counter;
  // $A235
  u8        frame_toggle;
  // $A236
  u8        slow_anim_counter;
  // $A237
  u8        sfx_index;
  // $A238
  u8        sfx_priority;
  // $A239
  u8        siren_enabled;
  // $A23A
  u8        turbo_sfx_pitch;
  // $A23B
  u8        tunnel_sfx;
  // $A23C
  u8        trigger_righthand_sfx;
  // $A23D
  u8        trigger_lefthand_sfx;
  // $A23E
  u8        off_road; // 0/1/2 => on-road/one wheel off-road/both wheels off-road
  // $A23F
  u8        fast_counter;
  // $A240
  u8       *roadbufptr;
  u8       *roadbuf_start; // Conv: additional
  u8       *roadbuf_end; // Conv: additional
  // $A242
  u8        curvature_byte;
  // $A243
  u8        height_byte;
  // $A244
  u8        leftside_byte;
  // $A245
  u8        rightside_byte;
  // $A246
  u8        hazards_counter;
  // $A247
  u8        lanes_counter;
  // $A248
  u8        on_dirt_track;
  // $A249
  u8        fork_taken;
  // $A24A
  u16       speed;
  // $A24C
  u8        inclined_counter;
  // $A24D
  u8        cornering;
  // $A24E
  u8        boost;
  // $A24F
  u8        smoke;
  // $A250
  u8        turn_speed;
  // $A251
  u8        flip_car;
  // $A252
  u8        gear_lockout;
  // $A253
  u8        gear;
  // $A254
  u8        allow_spawning;
  // $A255
  u8        distance_bcd[2];

  // $A258
  // TODO check signed use of incline
  s8        incline; // $FD..$03 = climbing/level/descending
  // $A259
  s8        prev_road_height;
  // $A25A
  u8        horizon_y_accum; // related to changes in incline (goes 0/1/2)
  // $A25B
  u8        horizon_y_step;
  // $A25C
  u8        current_curvature;
  // $A25D
  u8        horizon_curve_index;
  // $A25E
  u8        horizon_x_scroll; // cycles 4..1 or similar when roads curve
  // $A25F
  s16       horizontal_adjust;
  // $A261
  u8        horizon_scroll_sub;
  // $A262
  u8        curvature_ticks;
  // $A263
  u8        right_turn;
  // $A264
  u8        left_turn;
  // $A265
  u8        fork_visible;
  // $A266
  u8        fork_countdown;
  // $A267
  u16       fork_distance;
  // $A269
  u8        fork_in_progress;
  // $A26A
  u8        quit_state;
  // $A26B
  u8        start_speech; // delay
  // $A26C
  scenedata_t scenedata;

  // $A68F (SM) in perp_behaviour
  u8        pb_changing_lane; // changing lane flag
  // $A69B (SM) in perp_behaviour
  u8        pb_lane_change_timer;
  // $A73E (SM) in perp_behaviour
  u8        pb_delay;
  // $A749 (SM) in perp_behaviour
  u8        pb_approach_timer; // delay

  // $A804 (SM) in spawn_cars
  u8        sc_spawn_counter;

  // $A97E (SM) in layout_dirt_and_stones
  u8        ldas_enabled; // makes layout_dirt_and_stones run

  // $A9DE (SM) in draw_dirt_and_stones
  u8        ddas_enabled; // makes draw_dirt_and_stones run
  // $A9E2 (SM) in draw_dirt_and_stones
  u8       *ddas_particle; // byte ptr into the $ED28 particle table

  // $AA5A (SM) in draw_helicopter
  u8        dhs_heli_vert_base; // y position?
  // $AA76 (SM) in draw_helicopter
  u8        dhs_heli_y_offset;
  // $AA8C (SM) in draw_helicopter
  u8        dhs_heli_rotor_pos;

  // $AA94 (SM) in dhl_aa94
  s16       dhl_helipos;

  // $AACB (SM) in move_helicopter
  u8        mh_height; // height
  // $AAD7 (SM) in move_helicopter
  u8        mh_animframe; // animation counter (0..3)
  // $AADF (SM) in move_helicopter
  s8        mh_direction; // direction (-1 or 1)
  // $AAE8 (SM) in move_helicopter
  u8        mh_offset; // horizontal pos/offset
  // $AAF6 (SM) in move_helicopter
  u16       mh_prevroadpos; // previous road pos
  // $AB06 (SM) in move_helicopter
  s16       mh_heli_centre_y; // 112 normally; −56 during the exit sequence

  // $AE70 (SM) in advance_hazard
  u16       dhs_road_left_xpos;

  // $AED0 (SM) in draw_hazards
  s16      *dhs_xpos_table; // points to table e900 for example

  // $AFFB (SM) in dhs_aecf
  u8        smoke_bitmap_index; // (smoke) speed factor?

  // $B023 (SM) in ...
  u8        dhs_col_pos;
  // $B029 (SM) in ...
  s8        dhs_SM_B029_horz_clip;
  // $B02C (SM) in ...
  u8        dhs_SM_B02C_horz_pos;

  // $B063 (SM) in move_hero_car
  u8        mhc_y_offset; // jump counter
  // $B079 (SM) in move_hero_car
  const u8 *mhc_jump_data; // jump data table entry

  // $B325 (SM) in animate_hero_car
  u16       ahc_crashed_flag; // crashed flag
  // $B32E (SM) in animate_hero_car
  u16       ahc_crash_speed_threshold; // set when crashed (a speed)
  // $B356 (SM) in animate_hero_car
  u16       ahc_crash_spin_speed; // perhaps a speed
  // $B36E (SM) in animate_hero_car
  u8        ahc_flip_flag; // flip flag
  // $B384 (SM) in animate_hero_car
  u8        ahc_delay; // delay counter, set to 5
  // $B38D (SM) in animate_hero_car
  u8        ahc_crash_flip_count; // (flip flag + 1)
  // $B395 (SM) in animate_hero_car
  u16       ahc_road_pos_a; // a road position
  // $B3A3 (SM) in animate_hero_car
  u16       ahc_road_pos_b; // another road position
  // $B3DB (SM) in animate_hero_car
  u8        ahc_crash_spin; // controls flipping
  // $B476 (SM) in animate_hero_car
  u8        ahc_hand_step;
  // $B478 (SM) in animate_hero_car
  u8        ahc_hand_delay; // hand animation frame?

  // $B4F0 (SM) in smash
  u8        smash_cycling_counter;

  // $B549 (SM) in draw_debris
  u8        dd_SM_B549_frame_counter; // frame counter, set to 9 by smash
  // $B55B (SM) in draw_debris
  u8      **dd_debris_subtables_start;
  // $B570 (SM) in draw_debris
  u16       dd_frame_offset; // (might not need to be a state var)

  // $B5AA (SM) in draw_car
  u8        dhc_jump_y; // height of car in the air - leaving shadow on the ground
  // $B5AF (SM) in draw_car
  u8        dhc_pitch; // car's pitch (0/3/6 = level/up/down)

  // $BB8B (SM) in rm_cycle_buffer_offset
  u16       rm_leftfork_hazards; // hazards left-fork Z80 addr
  // $BB8E (SM) in rm_cycle_buffer_offset
  u16       rm_leftfork_rightside; // rightside left-fork Z80 addr
  // $BB91 (SM) in rm_cycle_buffer_offset
  u16       rm_leftfork_leftside; // leftside left-fork Z80 addr
  // $BB95 (SM) in rm_cycle_buffer_offset
  u16       rm_leftfork_curve; // curvature left-fork Z80 addr
  // $BB98 (SM) in rm_cycle_buffer_offset
  u16       rm_leftfork_height; // height left-fork Z80 addr
  // $BB9B (SM) in rm_cycle_buffer_offset
  u16       rm_leftfork_lanes; // lanes left-fork Z80 addr
  // $BBB8 (SM) in rm_cycle_buffer_offset
  u16       rm_rightfork_hazards; // hazards right-fork Z80 addr
  // $BBBB (SM) in rm_cycle_buffer_offset
  u16       rm_rightfork_rightside; // rightside right-fork Z80 addr
  // $BBBE (SM) in rm_cycle_buffer_offset
  u16       rm_rightfork_leftside; // leftside right-fork Z80 addr
  // $BBC2 (SM) in rm_cycle_buffer_offset
  u16       rm_rightfork_curve; // curvature right-fork Z80 addr
  // $BBC5 (SM) in rm_cycle_buffer_offset
  u16       rm_rightfork_height; // height right-fork Z80 addr
  // $BBC8 (SM) in rm_cycle_buffer_offset
  u16       rm_rightfork_lanes; // lanes right-fork Z80 addr
  // $BE6D (SM) in rm_cycle_buffer_offset
  const u8 *rm_curvature_fork_end_ptr; // curvature one_command ptr
  // $BEBF (SM) in rm_cycle_buffer_offset
  const u8 *rm_height_fork_end_ptr; // height one_command ptr
  // $BF0A (SM) in rm_cycle_buffer_offset
  const u8 *rm_lanes_fork_end_ptr; // lanes one_command ptr
  // $BF2D (SM) in rm_cycle_buffer_offset
  u8        rm_lanes_byte; // lanes current value
  // $BF84 (SM) in rm_cycle_buffer_offset
  const u8 *rm_rightside_fork_end_ptr; // rightside one_command ptr
  // $BFCD (SM) in rm_cycle_buffer_offset
  const u8 *rm_leftside_fork_end_ptr; // leftside one_command ptr
  // $C046 (SM) in rm_cycle_buffer_offset
  const u8 *rm_hazards_fork_end_ptr; // hazards one_command ptr
  // $C058 (SM) in read_map
  u8        rm_hazard_byte; // current hazard command
  // $C0BB (SM) in read_map
  u8        rm_scroll_dirt_particles;

  // $C15D (SM) in draw_tunnel
  u8        dt_tunnel_distance; // size related [15 when tunnel is small, 6 when fills screen]
  // $C160 (SM) in draw_tunnel
  u8        dt_tunnel_visible; // 0 if not visible; vibrates 1 if visible; 2 if in tunnel
  // $C21C (SM) in draw_tunnel
  u16       dt_fill_pattern;
  // $C221 (SM) in draw_tunnel
  u8        dt_fill_start_a; // jump table target
  // $C236 (SM) in draw_tunnel
  u8        dt_fill_start_b; // jump table target
  // $C2B8 (SM) in draw_tunnel
  u8        dt_far_wall_mode;

  // $C4B2 (SM) in draw_road
  dr_callback_t *dr_callback;
  // $C56C (SM) in draw_road
  u16       dr_backbuf_1;
  // $C5AC (SM) in draw_road
  s8        dr_neg_lane_count;
  // $C5B3 (SM) in draw_road
  u8        dr_left_table_hi_2;
  // $C5D9 (SM) in draw_road
  u8        dr_right_table_hi_2;
  // $C5F9 (SM) in draw_road
  u16       dr_backbuf_2;
  // $C60A (SM) in draw_road
  u8        dr_right_stripe_width;
  // $C61B (SM) in draw_road
  u8        dr_road_width;
  // $C62C (SM) in draw_road
  u8        dr_left_stripe_width;
  // $C642 (SM) in draw_road
  u8        dr_left_table_hi_1;
  // $C651 (SM) in draw_road
  u8        dr_edge_graphic_offset;
  // $C677 (SM) in draw_road
  u8        dr_stripe_table_offset;
  // $C68A (SM) in draw_road
  u8        dr_right_table_hi_1;
  // $C698 (SM) in draw_road
  u8        dr_right_edge_offset;
  // $C6AD (SM) in draw_road
  void    (*dr_fill_fn)(chqstate_t *state, int Ccounter, int DEbackbuf, int L, int Adash_fill, u8 **IXlanesptr, const u8 **IYheightptr);
  // $C6B2 (SM) in draw_road
  u8        dr_initial_stripe_state; // inital road stripe state
  // $C6BC (SM) in draw_road
  u8        dr_fill_pattern;
  // $C6D3 (SM) in draw_road
  u8        dr_stripe_xor_base;
  // $C6D8 (SM) in draw_road
  u8        dr_edge_thickness; // road edge line thickness
  // $C7E7 (SM) in draw_road
  u8        dr_horizon_x_scroll;
  // $C80A (SM) in draw_road
  u8        dr_sky_rows;
  // $C86C (SM) in dr_start_backdrop_fill: offset of first LDI in blit stream (0..18)
  u8        dr_backdrop_copy_jump;
  // $C82D in dr_start_backdrop_fill: runtime-selected 18-byte blit instruction stream
  u8        dr_backdrop_copy_instrs[18];
  // $C88F (SM) in draw_road
  u8        dr_in_tunnel;

  // $CE0C–$CE32
  u8        smokes[3][13]; // three 13-byte smoke animation buffers, indexed 0..2

  // $CE33
  u8       *debris_table[12];
  // $CE4B–$CEAA
  u8        debris_subtables[5][19]; // five 19-byte subtables, indexed 0..4

  // $E300
  u8        height_table[32]; // [0]=$60 sentinel, [1..21]=perspective heights, [22]=$A0 terminal, [23..31]=unused
  // $E320
  u8        curvature_table[22]; // per-depth curvature adjustment (22 entries); built by build_curve_table
  // $E336
  u8        clamped_heights[21]; // running-minimum clamp of height_table[1..21]; built by build_height_table
  // $E34B
  u8        horizon_attr[3]; // horizon attribute scroll state: [0]=initial delta, [1]=current level, [2]=previous level
  // $E34E - unused pad byte; kept so pointer arithmetic ending at $E34E/$E34F (advance_hazard, draw_tunnel) lands correctly
  u8        horizon_attr_pad;
  // $E34F
  u8        object_positions[21];
  // $E800 - Left outer edge (verge/road boundary)
  s16       xpos_road_left[128];
  // $E900 - Left inner edge / centre-left
  s16       xpos_road_centre_left[128];
  // $EA00 - Road centre line
  s16       xpos_road_centre[128];
  // $EB00 - Right inner edge / centre-right
  s16       xpos_road_centre_right[128];
  // $EC00 - Right outer edge (road/verge boundary)
  s16       xpos_road_right[128];
  // $ED00 - Right outer edge of right fork (forked roads only); also reused as dirt/stone layout data on dirt tracks
  s16       xpos_road_fork_right[128];
  // $EE00
  u8        road_buffer[256];
  // $EF00
  u8        flipped[256];

  // $EADC
  u8        messages_key_string[5];
  // $EE38
  u8        temp_keydefs[8];

  // $EE6E
  struct {
    u8        pattern_repeats; // (SM) in next_pattern; pattern repeat counter
    const u8 *pattern_addr;   // current pattern address
    u8        started;
    u8        note_delay;
    u8        note_delay_reload;
    const u8 *data_ptr;
    const u8 *pattern_start_ptr;
    u8        extra_delay;
    u8        drum_active; // drum playing flag
    u8        drum_speed;
  } music;

  // $EC01-$F224 (128K bank 3 only) -- title-screen tune engine channel
  // trackers and self-modifying scratch bytes. Same numeric address range as
  // `music` above but a different bank/context; kept as a distinct struct
  // rather than aliased onto it (see compute_channel_ay_registers,
  // $EE9E@bank3).
  struct {
    struct title_tune_channel channel[3]; // $EC01/$EC26/$EC4B

    // $EC70 (128K bank 3): per-tick tempo countdown, decremented by
    // ts_music_service ($EC71) each call; the 3
    // channels' patterns only advance one row when it reaches zero, after
    // which it always reloads to a fixed 1 -- per the skool's own comment at
    // $EC99, *not* the tune's stored tempo/speed byte (tune_tempo, $EC9A),
    // so the driver appears to always tick every other call regardless of
    // the selected tune. Set to 1 by start_tune ($EBF9-$EBFA) so the very
    // first service call after a tune starts always advances.
    u8        tempo_counter;

    // $EC79 (SM): operand of "LD A,$00" at $EC78 in ts_music_service
    // ($EC71@bank3); written by advance_channel_pattern
    // ($EDF6@bank3); read by compute_channel_ay_registers.
    // Purpose not established elsewhere in bank 3 (see skool comment at $EE9E).
    u8        shared_note_value;

    // $EC9A (128K bank 3): tune tempo/speed byte. Written by start_tune
    // ($EBB3, not yet translated) and by advance_channel_pattern's
    // decode_pattern_command cascade ($EE71); not currently read anywhere in
    // bank 3 (see the skool comment at $EC9B in ts_music_service).
    u8        tune_tempo;

    // $ECC6 (SM): operand of "LD A,$00" at $ECC5 in ts_music_service
    // ($EC71@bank3). Written within compute_channel_ay_registers (forced to
    // a computed value or to $41 on the mute transition); also written
    // directly to 0 by ts_music_service itself at $EC7A on entry. Read back
    // by ts_music_service at $ECC5 -- the "LD A,$00" instruction there
    // executes with whatever value was last patched into its own operand
    // byte, so it is not actually loading a literal 0 once
    // compute_channel_ay_registers has run -- and stored into the
    // noise_pitch AY register cache ($EFB5). Purpose beyond that plumbing
    // not established.
    u8        driver_internal_flag;

    // $EED1 (128K bank 3): driver-internal flag. Write-only, set by
    // advance_channel_pattern's pcmd_set_driver_flag handler ($ED87) from a
    // pattern-stream operand byte; cleared to 0 by start_tune ($EBF6, not yet
    // translated); never read anywhere in bank 3. Purpose not established.
    u8        pattern_driver_flag;

    // $EF7A (SM): operand of "LD A,$00" at $EF79 in
    // compute_channel_ay_registers ($EE9E@bank3), phase 5. Merged into by
    // advance_channel_pattern's mixer-bit pattern-command handlers
    // ($ED36/$ED4B/$ED5F) using the same replace-bits-under-mask idiom as
    // the $EFB6 mixer cache. Read back every frame at $EF79 -- the
    // "LD A,$00" instruction there executes with whatever value was last
    // patched into its own operand byte, so on any frame where the JR NZ at
    // $EF7B is taken (skipping the $EC79-derived $07 path), the mixer merge
    // uses this self-modified value rather than a literal 0. Confirmed
    // against a genuine ChaseHQ.ay dump (SlopAY project corpus): the
    // pristine snapshot's static operand byte is 0x00 (the pre-pattern-data
    // startup default), but the real tune patches it via the pattern
    // commands above, driving the AY mixer's noise-enable bits.
    u8        pending_mixer_bits;

    // $F223 (128K bank 3): tune-active flag. Tested by ts_music_service
    // ($EC71/$ECCA) to decide whether to advance
    // patterns / recompute and flush AY registers at all; also tested
    // directly by the title-screen driver at $C621. Armed to 1 by start_tune
    // ($EBFD) once the 3 channel-tracker records are initialised; cleared to
    // 0 both by start_tune's own entry (so a tune restart is briefly
    // inactive while re-initialising) and by the stop routine at $ED0B (not
    // yet translated).
    u8        tune_active;

    // $F224 (128K bank 3): cleared to 0 alongside tune_active by start_tune
    // ($EBA1, "its companion byte"); never read anywhere in bank 3. Purpose
    // not established beyond being cleared in lockstep with tune_active.
    u8        tune_active_companion;
  } title_music;

  // $EFAF-$EFBA (128K bank 3 only) -- per-frame AY register cache for the
  // title-tune engine, refreshed by compute_channel_ay_registers ($EE9E) and
  // flushed to the AY chip by ts_music_service ($EC71) via
  // write_title_ay_registers.
  // Separate from the in-game AY block at $A213 above (same numeric address
  // range, different bank/context) -- do not alias the two.
  struct {
    u16       chan_a_pitch; // $EFAF/$EFB0
    u16       chan_b_pitch; // $EFB1/$EFB2
    u16       chan_c_pitch; // $EFB3/$EFB4
    u8        noise_pitch;  // $EFB5
    u8        mixer;        // $EFB6 -- read-modify-written by compute_channel_ay_registers
    u8        chan_a_vol;   // $EFB7
    u8        chan_b_vol;   // $EFB8
    u8        chan_c_vol;   // $EFB9
    u8        env_fine;     // $EFBA
  } title_ay_regs;

  // $C5A2 (SM, 128K bank 3 only): title-screen scene selector, rotated and
  // tested bit-by-bit by title_screen_driver to pick one of the 5 scene
  // tables each time the title screen restarts.
  u8                 title_scene_selector;

  // $BB00-$BB4F (128K bank 3 only): the 9 animated-object records populated
  // from the chosen scene table by title_screen_driver, drawn each frame by
  // ts_animate_frame, and advanced by object_script_step ($C705).
  struct title_object title_objects[9];

  // $EF5E — drum 1 PCM sample; playdrum_go's RLC (HL) rotates the bytes in
  // place, so like sfx_crash_table this is a mutable copy of a template
  u8        drum1[252];
  // $F05A — drum 2 PCM sample; as drum1
  u8        drum2[108];

  // $F000
  u8        backbuffer[BACKBUFFER_LENGTH + BACKBUFFER_OVERFLOW];

  // $FD97-$FD9B (128K bank 3): print_character scratch record built by
  // read_new_key_definition ($FF2C) each time a control's key name is
  // drawn. Not fixed ROM data -- same role as messages_key_string for the
  // 48K equivalent (define_a_key).
  u8        options_key_string[5];

  // $FFF7-$FFFE (128K bank 3): live scan-key-code buffer for the currently
  // active control scheme. Not fixed ROM data -- installed from the
  // Sinclair/Cursor joystick key lists by options_menu_driver ($FBDC), or
  // written key-by-key by read_new_key_definition ($FF2C).
  // Layout: [0..4] = gear/accelerate/brake/left/right (joystick-mappable),
  // [5..7] = quit/pause/turbo (keyboard-only).
  u8        control_keys[8];
};

#endif /* CHASEHQ_STATE_H */

