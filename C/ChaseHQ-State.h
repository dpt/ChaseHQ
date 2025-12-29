// ChaseHQ-State.h
//
// Chase H.Q. code model
//
// by dpt

// vim: ts=8 sts=2 sw=2 et

#ifndef CHASEHQ_STATE_H
#define CHASEHQ_STATE_H

#include <stdint.h>

#include "Types.h"
#include "Spectrum.h"
#include "ChaseHQ.h"

struct hazard_s {
  u8        used;
  u8        distance;
  u8        horz_pos;
  u8        TBD3;    // distance related
  u8        TBD4;
  u8        horz_pos_on_road;
  u8        TBD6;
  u8        TBD7;
  u8        TBD8;
  const u8 *lod_addr; // Conv: u16 becomes pointer
  u16       hit_handler;
  u16       speed;
  u8        TBD15;
  u8        TBD16;
  u8        TBD17; // perp distance high byte
  u8        TBD18;
  u8        TBD19;
};

// crap name
struct stagevars_s {
  // $A16D
  u8        var_a16d;
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
  u8        time_sixteenths;
  // $A17E
  u8        time_bcd;
  // $A17F
  u8        time_digits[2];
  // $A181
  u8        distance_digits[4];
  // $A185
  u8        no_objects_counter;
  // $A186
  u16       horizon_attribute;
};

struct chqstate_s {
  // $4000
  u8        screen[SCREEN_LENGTH];

  // $5B00
  u8        pre_shifted_backdrop[10 * 24]; // occupies 256 bytes in real game?

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
  u8        stagedata[STAGEDATA_LENGTH];

  // $8277
  u8        SM_8277;

  // $8ABE
  u8        SM_8ABE;

#define SCORE_MESSAGES_BASE   (0x8C58)
#define SCORE_MESSAGES_LENGTH (0x8CB2 - SCORE_MESSAGES_BASE)
  // $8C58
  u8        score_messages[SCORE_MESSAGES_LENGTH];

  // $8D77
  char      time_nn[7]; // initialised to "TIME 10"
  // $8D85
  char      credit_n[8]; // initialised to "CREDIT  "

  // $8DA1 (SM)
  u8        transition_nframes;
  // $8DB1 (SM)
  s16       transition_frame_stride; // fade step value: 8 or -8
  // $8DBB (SM)
  const u8 *transition_mask;

  // $8E43 (SM)
  const u8 *overlay_message; // in draw_overlay_messages
  // $8E46 (SM)
  u8        overlay_count; // in draw_overlay_messages
  // $8E49 (SM)
  u8        overlay_delay; // in draw_overlay_messages

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

  // $9C85
  u8        SM_9c85;
  // $9C86
  u8        SM_9c86;

  // $9D51
  char      bonus_string[6];

  // $9D9B
  char     *SM_address_of_score_digits; // was self modified

  // $9E22
  u8        SM_9e22; // in plot_turbos_and_scores

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
  stagevars_t st;

  // $A188
  hazard_t  hazards[MAXHAZARDS];

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
  u8        counter_A;
  // $A235
  u8        counter_B;
  // $A236
  u8        counter_C;
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
  u8        var_a23c;
  // $A23D
  u8        var_a23d;

  // $A23F
  u8        fast_counter;
  // $A240
  u8       *road_buffer_offset;
  u8       *road_buffer_start; // additional
  u8       *road_buffer_end; // additional
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
  u8        inclined;
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
  u8        incline;
  // $A259
  u8        var_a259;
  // $A25A
  u8        var_a25a;
  // $A25B
  u8        var_a25b;
  // $A25C
  u8        current_curvature;
  // $A25D
  u8        horizon_a25d;
  // $A25E
  u8        horizon_a25e;
  // $A25F
  u8        horizontal_adjust;
  // $A261
  u8        var_a261;
  // $A262
  u8        var_a262;
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
  u8        start_speech;
  // $A26C
  u16       road_pos;
  // $A26E
  const u8 *road_curvature_ptr;
  // $A270
  const u8 *road_height_ptr;
  // $A272
  const u8 *road_lanes_ptr;
  // $A274
  const u8 *road_rightside_ptr;
  // $A276
  const u8 *road_leftside_ptr;
  // $A278
  const u8 *road_hazard_ptr;

  // $BB95
  const u8 *SM_BB95;
  // $BBC2
  const u8 *SM_BBC2;

  // $E300
  u8        table_e300[32]; // note: first byte should be $60
  // $E320
  u8        table_e320[2];
  // $E34B
  u8        horizon_table_e34b[3]; // horizon related
  // $E34F
  u8        object_positions[21];
  // $E800
  u16       table_e800[128];
  // $E900
  u16       table_e900[128];
  // $EA00
  u16       table_ea00[128];
  // $EB00
  u16       table_eb00[128];
  // $EC00
  u16       table_ec00[128];
  // $ED00
  u16       table_ed00[128];
  // $EE00
  u8        road_buffer[256];
  // $EF00
  u8        flipped[256];

  // MUST ALIGN THIS
  u8        padding[3990];
  // $F000
  u8        backbuffer[BACKBUFFER_LENGTH];
};

#endif /* CHASEHQ_STATE_H */

