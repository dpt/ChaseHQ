/**
 * ChaseHQ-State.h
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

#include "ChaseHQ.h"
#include "ChaseHQ-Stages.h"

/* ----------------------------------------------------------------------- */

typedef void (dr_callback_t)(chqstate_t *state, u8 Bfill_pattern, u16 DEscreen_ptr, u8 L);

typedef void (plot_sprite_cb_t)(chqstate_t *state,
                                int         IXjump_offset,
                                u8         *HLbackbuf_addr,
                                u8          Bdash_height,
                                u16         DEdash_bitmap_stride,
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
  u8                inverted;          // sprite plot mode: 0=normal, 1=stopped/neutral, 2=inverted
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
  u8        time_sixteenths; // is this sixteenths or fifteenths since we reset to 15?
  // $A17E
  u8        time_bcd;
  // $A17F
  u8        time_digits[2];
  // $A181
  u8        distance_digits[4];
  // $A185
  u8        no_objects_counter;
  // $A186
  u16       horizon_attribute; // Z80 address
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
  u8        attract_cycle;
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
  u8        attract_mode_128k_countdown;

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

#define SCORE_MESSAGES_BASE   (0x8C58)
#define SCORE_MESSAGES_LENGTH (0x8CB2 - SCORE_MESSAGES_BASE)
  // $8C58
  u8        score_messages[SCORE_MESSAGES_LENGTH];

  // $8D77
  char      time_nn[7]; // initialised to "TIME 10"
  // $8D85
  char      credit_n[8]; // initialised to "CREDIT  "

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
  u8        dee_tunnel_1;
  // $8FA4 (SM) in draw_everything_else
  u8        dee_helicopter;
  // $8FA7 (SM) in draw_everything_else
  u8        dee_tunnel_2;

  // $90F1 (SM) in draw_overhead
  u8        do_vert_sub;
  // $9115 (SM) in draw_overhead
  u8        do_span_width_words;

  // $9396 (SM) in draw_object_common
  u8        doc_shift_select;
  // $933D (SM) in draw_object_common
  u8        doc_col_pos;
  // $93C0 (SM) in draw_object_common
  u8        doc_inverted; // controls sprite plotting (2 => inverted, 1 => ?, 0 => ?)
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

  // $9D9B
  char     *SM_address_of_score_digits; // was self modified

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
  u8        turbo_sfx_noise_pitch;
  // $A23B
  u8        tunnel_sfx;
  // $A23C
  u8        trigger_passed_object_sfx;
  // $A23D
  u8        trigger_lane_change_sfx;
  // $A23E
  u8        off_road;
  // $A23F
  u8        fast_counter;
  // $A240
  u8       *road_buffer_offset;
  u8       *road_buffer_start; // Conv: additional
  u8       *road_buffer_end; // Conv: additional
  // $A242
  u8        curvature_byte;
  // $A243
  u8        height_byte;
  // $A244
  u8        leftside_byte;
  // $A245
  u8        rightside_byte;
  // $A246
  u8        hazards_byte;
  // $A247
  u8        lanes_counter_byte;
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
  u8        prev_road_height;
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

  // $A9DE (SM) in dust_stones_stuff
  u8        dss_enabled; // makes dust_stones_stuff run
  // $A9E2 (SM) in dust_stones_stuff
  u16      *dss_fork_xpos_ptr; // a table ptr e.g. $ED28

  // $AA5A (SM) in draw_helicopter
  u8        dh_heli_vert_base; // y position?
  // $AA76 (SM) in draw_helicopter
  u8        dh_heli_body_y_offset;
  // $AA8C (SM) in draw_helicopter
  u8        dh_heli_rotor_pos;

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
  u16       mh_heli_centre_y;

  // $AE70 (SM) in dh_draw_one_hazard
  u16       dh_road_left_xpos;

  // $AED0 (SM) in draw_hazards
  u16      *dh_xpos_table; // points to table e900 for example

  // $AFFB (SM) in dh_aecf
  u8        smoke_bitmap_index; // (smoke) speed factor?

  // $B023 (SM) in ...
  u8        dh_col_pos;
  // $B029 (SM) in ...
  s8        dh_SM_B029_horz_clip;
  // $B02C (SM) in ...
  u8        dh_SM_B02C_horz_pos;

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
  u8        ahc_road_pos_b; // another road position
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
  u16       rm_SM_BB8B_leftfork_hazards; // hazards left-fork Z80 addr
  // $BB8E (SM) in rm_cycle_buffer_offset
  u16       rm_SM_BB8E_leftfork_rightside; // rightside left-fork Z80 addr
  // $BB91 (SM) in rm_cycle_buffer_offset
  u16       rm_SM_BB91_leftfork_leftside; // leftside left-fork Z80 addr
  // $BB95 (SM) in rm_cycle_buffer_offset
  u16       rm_SM_BB95_leftfork_curve; // curvature left-fork Z80 addr
  // $BB98 (SM) in rm_cycle_buffer_offset
  u16       rm_SM_BB98_leftfork_height; // height left-fork Z80 addr
  // $BB9B (SM) in rm_cycle_buffer_offset
  u16       rm_SM_BB9B_leftfork_lanes; // lanes left-fork Z80 addr
  // $BBB8 (SM) in rm_cycle_buffer_offset
  u16       rm_SM_BBB8_rightfork_hazards; // hazards right-fork Z80 addr
  // $BBBB (SM) in rm_cycle_buffer_offset
  u16       rm_SM_BBBB_rightfork_rightside; // rightside right-fork Z80 addr
  // $BBBE (SM) in rm_cycle_buffer_offset
  u16       rm_SM_BBBE_rightfork_leftside; // leftside right-fork Z80 addr
  // $BBC2 (SM) in rm_cycle_buffer_offset
  u16       rm_SM_BBC2_rightfork_curve; // curvature right-fork Z80 addr
  // $BBC5 (SM) in rm_cycle_buffer_offset
  u16       rm_SM_BBC5_rightfork_height; // height right-fork Z80 addr
  // $BBC8 (SM) in rm_cycle_buffer_offset
  u16       rm_SM_BBC8_rightfork_lanes; // lanes right-fork Z80 addr
  // $BE6D (SM) in rm_cycle_buffer_offset
  const u8 *rm_SM_BE6D_curvature_one_command_ptr; // curvature one_command ptr
  // $BEBF (SM) in rm_cycle_buffer_offset
  const u8 *rm_SM_BEBF_height_one_command_ptr; // height one_command ptr
  // $BF0A (SM) in rm_cycle_buffer_offset
  const u8 *rm_SM_BF0A_lanes_one_command_ptr; // lanes one_command ptr
  // $BF2D (SM) in rm_cycle_buffer_offset
  u8        rm_lanes_byte; // lanes current value
  // $BF84 (SM) in rm_cycle_buffer_offset
  const u8 *rm_SM_BF84_rightside_one_command_ptr; // rightside one_command ptr
  // $BFCD (SM) in rm_cycle_buffer_offset
  const u8 *rm_SM_BFCD_leftside_one_command_ptr; // leftside one_command ptr
  // $C046 (SM) in rm_cycle_buffer_offset
  const u8 *rm_SM_C046_hazards_one_command_ptr; // hazards one_command ptr

  // $C058 (SM) in read_map
  u8        rm_SM_C058_hazard_type; // current hazard command
  // $C0BB (SM) in read_map
  u8        rm_SM_C0BB_fork_copy_pending; // hazard related

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
  // $C56D (SM) in draw_road
  u16       dr_backbuf_1;
  // $C5AC (SM) in draw_road
  s8        dr_neg_lane_count;
  // $C5B3 (SM) in draw_road
  u8        dr_left_table_hi_2; // table hi byte
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
  u8        dr_left_table_hi; // table hi byte
  // $C651 (SM) in draw_road
  u8        dr_edge_graphic_offset;
  // $C677 (SM) in draw_road
  u8        dr_stripe_table_offset;
  // $C68B (SM) in draw_road
  u8        dr_right_table_hi;
  // $C698 (SM) in draw_road
  u8        dr_right_edge_offset;
  // $C6AD (SM) in draw_road
  void    (*dr_fill_fn)(chqstate_t *state, u16 DEbackbuf, u8 L, u8 Adash_fill);
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
  // $C88F (SM) in draw_road
  u8        dr_in_tunnel;

  // $CE0C
  u8        smoke_1[13];
  // $CE19
  u8        smoke_2[13];
  // $CE26
  u8        smoke_3[13];

  // $CE33
  u8       *debris_table[12];
  // TODO Make these an array?
  // $CE4B
  u8        debris_subtable_1[19];
  // $CE5E
  u8        debris_subtable_2[19];
  // $CE71
  u8        debris_subtable_3[19];
  // $CE84
  u8        debris_subtable_4[19];
  // $CE97
  u8        debris_subtable_5[19];

  // $E300
  u8        table_e300[32]; // height table: [0]=$60 sentinel, [1..21]=heights, [22]=$A0 terminal, [23..31]=unused
  // $E320
  u8        table_e320[22]; // road curvature/position table (CTL: 22 entries; confirmed by 22-iteration write loop)
  // $E336
  u8        table_e336[21]; // clamped copy of height table
  // $E34B
  u8        horizon_table_e34b[3]; // horizon level related
  // $E34F
  u8        object_positions[21];
  // Conv: Z80 gap $E364-$E7FF (1180 bytes). build_curve_table_sub_cca8 writes
  // backward from table_e800 and can overflow past [0]; in Z80 this landed in
  // the gap. Replicate that gap so the overrun stays harmless.
  u8        _gap_e364[0x800 - 0x364]; // 1180 bytes
  // $E800 - Left outer edge (verge/road boundary)
  u16       xpos_road_left[128];
  // $E900 - Left inner edge / centre-left
  u16       xpos_road_centre_left[128];
  // $EA00 - Road centre line
  u16       xpos_road_centre[128]; // perhaps s16
  // $EB00 - Right inner edge / centre-right
  u16       xpos_road_centre_right[128];
  // $EC00 - Right outer edge (road/verge boundary)
  u16       xpos_road_right[128];
  // $ED00 - Right outer edge of right fork (forked roads only); also reused as dirt/stone layout data on dirt tracks
  u16       xpos_road_fork_right[128];
  // $EE00
  u8        road_buffer[256];
  // $EF00
  u8        flipped[256];

  // $EADC
  u8        messages_key_string[5];
  // $EE38
  u8        temp_keydefs[8];

  // $EE6E (SM) in next_pattern
  u8        music_pattern_repeats; // pattern repeat counter
  const u8 *music_pattern_addr; // current pattern address

  u8        music_started;
  u8        music_note_delay;
  u8        music_note_delay_reload;
  const u8 *music_data_ptr;
  const u8 *music_pattern_start_ptr;
  u8        music_extra_delay;
  u8        music_drum_active; // drum playing flag
  u8        music_irq_flag; // interrupt flag
  u8        music_drum_speed;

  // $F000
  u8        backbuffer[BACKBUFFER_LENGTH + BACKBUFFER_OVERFLOW];
};

#endif /* CHASEHQ_STATE_H */

