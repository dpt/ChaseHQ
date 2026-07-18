/**
 * Stages.h
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

#ifndef CHASEHQ_STAGES_H
#define CHASEHQ_STAGES_H

#include "C99/Types.h"
#include "ChaseHQ/Engine/Internal.h"

/* ----------------------------------------------------------------------- */

#define BACKDROP_WIDTH  (10) // bytes
#define BACKDROP_HEIGHT (24) // rows
#define BACKDROP_LENGTH (BACKDROP_WIDTH * BACKDROP_HEIGHT)

/* ----------------------------------------------------------------------- */

// Conv: The C version uses IDs for strings and blocks rather than inline
// addresses.

/// Chatter commands
#define CHATTERCMD_RANDOM                   (0xFC) // Followed by three chatterblock indices
#define CHATTERCMD_PAUSE                    (0xFE) // Followed by a single chatterblock index
#define CHATTERCMD_STOP                     (0xFF)

/// Chatter characters
#define CHATTERCHR_PILOT                       (0)
#define CHATTERCHR_NANCY                       (1)
#define CHATTERCHR_RAYMOND                     (2)
#define CHATTERCHR_TONY                        (3)

/// Chatter string indices
#define CHATTERSTR_GIDDY_UP_BOY                (0)
#define CHATTERSTR_HOLD_ON_MAN                 (1)
#define CHATTERSTR_THIS_IS_NANCY               (2)
#define CHATTERSTR_THIS_IS_AIRBORNE            (3)
#define CHATTERSTR_TARGET_VEHICLE_TURNED       (4)
#define CHATTERSTR_RIGHT_AHEAD_OVER            (5)
#define CHATTERSTR_LEFT_AHEAD_OVER             (6)
#define CHATTERSTR_READ_LOUD_CLEAR             (7)
#define CHATTERSTR_ROGER                       (8)
#define CHATTERSTR_GOTCHA_NANCY                (9)
#define CHATTERSTR_WHAT_YOU_DOING             (10)
#define CHATTERSTR_GOING_OTHER_WAY            (11)
#define CHATTERSTR_MESSIN_AROUND              (12)
#define CHATTERSTR_TIME_RUN_OUT               (13)
#define CHATTERSTR_GET_MOVIN_MAN              (14)
#define CHATTERSTR_OH_NO                      (15)
#define CHATTERSTR_PLEASE                     (16)
#define CHATTERSTR_GREAT                      (17)
#define CHATTERSTR_OUCH                       (18)
#define CHATTERSTR_LETS_GO                    (19)
#define CHATTERSTR_YAOW                       (20)
#define CHATTERSTR_BEAR_DOWN                  (21)
#define CHATTERSTR_MORE_PUSH_MORE             (22)
#define CHATTERSTR_ONE_MORE_TIME              (23)
#define CHATTERSTR_OH_MAN                     (24)
#define CHATTERSTR_WHOA                       (25)
#define CHATTERSTR_HARDER                     (26)
#define CHATTERSTR_PICKED_WRONG_JOB           (27)
#define CHATTERSTR_CHECK_CLASSIFIED_ADS       (28)
#define CHATTERSTR_ONE_MORE_TRY               (29)
#define CHATTERSTR_MEDIOCRE_DRIVER            (30)
#define CHATTERSTR_SEE_YOU_LATER              (31)
#define CHATTERSTR_PERP_DESC_1                (32)
#define CHATTERSTR_PERP_DESC_2                (33)
#define CHATTERSTR_PERP_DESC_3                (34)
#define CHATTERSTR_PERP_DESC_4                (35)
#define CHATTERSTR__LIMIT                     (36)

/// Chatter block indices
#define CHATTERBLK_START_STAGE                 (0)
#define CHATTERBLK_TONY_GIDDY_UP               (1)
#define CHATTERBLK_TONY_HOLD_ON                (2)
#define CHATTERBLK_PILOT_TURN_LEFT             (3)
#define CHATTERBLK_PILOT_TURN_RIGHT            (4)
#define CHATTERBLK_HEROES_ACKNOWLEDGE          (5)
#define CHATTERBLK_TONY_LOUD_CLEAR             (6)
#define CHATTERBLK_RAYMOND_ROGER               (7)
#define CHATTERBLK_TONY_GOTCHA                 (8)
#define CHATTERBLK_RAYMOND_WRONG_WAY           (9)
#define CHATTERBLK_RAYMOND_SMASH              (10)
#define CHATTERBLK_RAYMOND_BEAR_DOWN          (10)
#define CHATTERBLK_RAYMOND_PUSH_IT            (12)
#define CHATTERBLK_RAYMOND_HARDER             (13)
#define CHATTERBLK_RAYMOND_OH_MAN             (14)
#define CHATTERBLK_RAYMOND_RANDOM_PLEAS       (15)
#define CHATTERBLK_RAYMOND_PLEASE             (16)
#define CHATTERBLK_RAYMOND_GET_MOVING         (17)
#define CHATTERBLK_NANCY_TIME_RUNNING_OUT     (18)
#define CHATTERBLK_RAYMOND_RANDOM_YELPS       (19)
#define CHATTERBLK_RAYMOND_OHNO               (20)
#define CHATTERBLK_RAYMOND_OUCH               (21)
#define CHATTERBLK_RAYMOND_YAOW               (22)
#define CHATTERBLK_TURBO                      (23)
#define CHATTERBLK_TONY_WHOA                  (24)
#define CHATTERBLK_TONY_GREAT                 (25)
#define CHATTERBLK_RAYMOND_ONE_MORE_TIME      (26)
#define CHATTERBLK_NANCY_BERATES              (27)
#define CHATTERBLK_NANCY_WRONG_JOB            (28)
#define CHATTERBLK_NANCY_ONE_MORE_TRY         (29)
#define CHATTERBLK_NANCY_MEDIOCRE_DRIVER      (30)
#define CHATTERBLK_TONY_LETS_GO               (31)
#define CHATTERBLK__LIMIT                     (32)

/* ----------------------------------------------------------------------- */

// Map escape byte
#define MAP_ESC                         (0)

// Map commands
#define MAP_CMDCODE_GOTO                (0)
#define MAP_CMDCODE_FORK_END            (1)
#define MAP_CMDCODE_SPLIT               (2)
#define MAP_CMDCODE_STOP_BARRIERS       (3)
#define MAP_CMDCODE_UNKNOWN_HAZARD_6    (6)
#define MAP_CMDCODE_START_BARRIERS_L    (7)
#define MAP_CMDCODE_START_BARRIERS_R    (8)
#define MAP_CMDCODE_START_TWO_BARRIERS  (9)
#define MAP_CMDCODE_ARROW_OFF           (10)
#define MAP_CMDCODE_ARROW_L             (11)
#define MAP_CMDCODE_ARROW_R             (12)
#define MAP_CMDCODE_START_CARS          (13)
#define MAP_CMDCODE_STOP_CARS           (14)

// Map command sequences
#define MAP_CMD_GOTO(ADDR)              MAP_ESC, MAP_CMDCODE_GOTO, (ADDR) & 0xFF, (ADDR) >> 8
#define MAP_CMD_FORK_END                MAP_ESC, MAP_CMDCODE_FORK_END
#define MAP_CMD_SPLIT(LADDR,RADDR)      MAP_ESC, MAP_CMDCODE_SPLIT, (LADDR) & 0xFF, (LADDR) >> 8, (RADDR) & 0xFF, (RADDR) >> 8
#define MAP_CMD_STOP_BARRIERS           MAP_ESC, MAP_CMDCODE_STOP_BARRIERS
#define MAP_CMD_UNKNOWN_HAZARD_6        MAP_ESC, MAP_CMDCODE_UNKNOWN_HAZARD_6
#define MAP_CMD_START_BARRIERS_L        MAP_ESC, MAP_CMDCODE_START_BARRIERS_L
#define MAP_CMD_START_BARRIERS_R        MAP_ESC, MAP_CMDCODE_START_BARRIERS_R
#define MAP_CMD_START_TWO_BARRIERS      MAP_ESC, MAP_CMDCODE_START_TWO_BARRIERS
#define MAP_CMD_ARROW_OFF               MAP_ESC, MAP_CMDCODE_ARROW_OFF
#define MAP_CMD_ARROW_L                 MAP_ESC, MAP_CMDCODE_ARROW_L
#define MAP_CMD_ARROW_R                 MAP_ESC, MAP_CMDCODE_ARROW_R
#define MAP_CMD_START_CARS              MAP_ESC, MAP_CMDCODE_START_CARS
#define MAP_CMD_STOP_CARS               MAP_ESC, MAP_CMDCODE_STOP_CARS

// Map curvatures
#define MAP_CURVE_STRAIGHT(D)           (((D) << 4) | 0)
#define MAP_CURVE_RIGHT(D)              (((D) << 4) | 1)
#define MAP_CURVE_RIGHT_HARD(D)         (((D) << 4) | 2)
#define MAP_CURVE_RIGHT_VERY_HARD(D)    (((D) << 4) | 3)
#define MAP_CURVE_LEFT(D)               (((D) << 4) | 9)
#define MAP_CURVE_LEFT_HARD(D)          (((D) << 4) | 10)
#define MAP_CURVE_LEFT_VERY_HARD(D)     (((D) << 4) | 11)

// Map heights
#define MAP_HEIGHT_UP7(D)               (((D) << 4) | 1)
#define MAP_HEIGHT_UP5(D)               (((D) << 4) | 3)
#define MAP_HEIGHT_UP3(D)               (((D) << 4) | 5)
#define MAP_HEIGHT_UP1(D)               (((D) << 4) | 7)
#define MAP_HEIGHT_LEVEL(D)             (((D) << 4) | 8)
#define MAP_HEIGHT_DOWN1(D)             (((D) << 4) | 9)
#define MAP_HEIGHT_DOWN3(D)             (((D) << 4) | 11)
#define MAP_HEIGHT_DOWN5(D)             (((D) << 4) | 13)
#define MAP_HEIGHT_DOWN7(D)             (((D) << 4) | 15)

// Map lanes
#define MAP_LANES_4_VAL                 (0x00) // 0000_0000
#define MAP_LANES_3L_VAL                (0x81) // 1000_0001
#define MAP_LANES_3R_VAL                (0x82) // 1000_0010
#define MAP_LANES_2L_VAL                (0x01) // 0000_0001
#define MAP_LANES_2M_VAL                (0x02) // 0000_0010
#define MAP_LANES_2R_VAL                (0x03) // 0000_0011
#define MAP_LANES_4TO3L_VAL             (0xBD) // 1011_1101
#define MAP_LANES_4TO3R_VAL             (0x8E) // 1000_1110
#define MAP_LANES_3LTO4_VAL             (0xAD) // 1010_1101
#define MAP_LANES_3RTO4_VAL             (0x9E) // 1001_1110
#define MAP_LANES_3LTO2M_VAL            (0x06) // 0000_0110
#define MAP_LANES_3RTO2R_VAL            (0x0F) // 0000_1111
#define MAP_LANES_2LTO3L_VAL            (0x2D) // 0010_1101
#define MAP_LANES_2RTO3R_VAL            (0x1F) // 0001_1111
#define MAP_LANES_TUNNEL_VAL            (0x41) // 0100_0001 // engine-derived tunnel body value; used in runtime comparisons
#define MAP_LANES_TUNNEL_ENTRY_VAL      (0x45) // 0100_0101
#define MAP_LANES_TUNNEL_EXIT_VAL       (0x59) // 0101_1001
#define MAP_LANES_DIRTTRACK_VAL         (0xC1) // 1100_0001
#define MAP_LANES_FORKED_VAL            (0xED) // 1110_1101

// Bottom two bits are the left hand offset.

// Decoding lanes bits:
//
// if all clear => four lane road
// - if bit 6 clear => normal road
//   - if bit 7 set => 3 / 4-to-3 / 3-to-4 lanes
//     else if bit 7 clear => 2 / 3-to-2 / 2-to-3 lanes
// - else if bit 6 set => tunnel, dirt track or forked road
//   - if bit 7 clear => in tunnel
//     - if bits 2 or 3 set => tunnel transition
//       - if bit 4 set => tunnel exit
//       - else if bit 4 clear => tunnel entrance
//     - else bits 2 or 3 clear => tunnel body
//   - else if bit 7 set => forked road or dirt track
//     - if bit 5 set => forked road
//     - else if bit 5 clear => dirt track or (unknown)
//       - if bits 2 or 3 set => (unknown)
//       - else if bits 2 or 3 clear => dirt track
//
// Testing for bits 2 or 3 is used by draw_road_lanes_change to detect lane
// changing sections, but there must be some other test involved there since
// tunnel/forked would be included.
//

#define MAP_LANES_LEFT_OFFSET_MASK      (0x03)

#define MAP_LANES_4(D)                  (D), (MAP_LANES_4_VAL)
#define MAP_LANES_3L(D)                 (D), (MAP_LANES_3L_VAL)
#define MAP_LANES_3R(D)                 (D), (MAP_LANES_3R_VAL)
#define MAP_LANES_2L(D)                 (D), (MAP_LANES_2L_VAL)
#define MAP_LANES_2M(D)                 (D), (MAP_LANES_2M_VAL)
#define MAP_LANES_2R(D)                 (D), (MAP_LANES_2R_VAL)
#define MAP_LANES_4TO3L(D)              (D), (MAP_LANES_4TO3L_VAL)
#define MAP_LANES_4TO3R(D)              (D), (MAP_LANES_4TO3R_VAL)
#define MAP_LANES_3LTO4(D)              (D), (MAP_LANES_3LTO4_VAL)
#define MAP_LANES_3RTO4(D)              (D), (MAP_LANES_3RTO4_VAL)
#define MAP_LANES_3LTO2M(D)             (D), (MAP_LANES_3LTO2M_VAL)
#define MAP_LANES_3RTO2R(D)             (D), (MAP_LANES_3RTO2R_VAL)
#define MAP_LANES_2LTO3L(D)             (D), (MAP_LANES_2LTO3L_VAL)
#define MAP_LANES_2RTO3R(D)             (D), (MAP_LANES_2RTO3R_VAL)
#define MAP_LANES_TUNNEL_ENTRY(D)       (D), (MAP_LANES_TUNNEL_ENTRY_VAL)
#define MAP_LANES_TUNNEL_EXIT(D)        (D), (MAP_LANES_TUNNEL_EXIT_VAL)
#define MAP_LANES_DIRTTRACK(D)          (D), (MAP_LANES_DIRTTRACK_VAL)
#define MAP_LANES_FORKED(D)             (D), (MAP_LANES_FORKED_VAL)

// Map hazards
#define MAP_HAZARD_WAIT(T)              (T)

// Objects
//
// These names are valid for Stage 1 but I'm not yet sure which ones are the
// same across all stages.
#define MAP_OBJ_NONE_VAL                (0) // common to all stages?
#define MAP_OBJ_SHORT_POLE_VAL          (3) // common to all stages?

#define MAP_OBJ_S1_NONE_VAL             (0) // perhaps a dupe/common across all levels
#define MAP_OBJ_S1_TUNNEL_LIGHT_VAL     (1)
#define MAP_OBJ_S1_UNKNOWN_2_VAL        (2) // unused
#define MAP_OBJ_S1_SHORT_POLE_VAL       (3) // perhaps a dupe/common
#define MAP_OBJ_S1_TREE_VAL             (4)
#define MAP_OBJ_S1_BUSH_VAL             (5)
#define MAP_OBJ_S1_STREET_LAMP_VAL      (6)
#define MAP_OBJ_S1_TELEGRAPH_POLE_VAL   (7)
#define MAP_OBJ_S1_TURN_SIGN_L_VAL      (8)
#define MAP_OBJ_S1_TURN_SIGN_R_VAL      (9)

#define MAP_OBJ_S1_NONE(D)              ((D << 4) | MAP_OBJ_S1_NONE_VAL)
#define MAP_OBJ_S1_TUNNEL_LIGHT(D)      ((D << 4) | MAP_OBJ_S1_TUNNEL_LIGHT_VAL)
#define MAP_OBJ_S1_UNKNOWN_2(D)         ((D << 4) | MAP_OBJ_S1_UNKNOWN_2_VAL)
#define MAP_OBJ_S1_SHORT_POLE(D)        ((D << 4) | MAP_OBJ_S1_SHORT_POLE_VAL)
#define MAP_OBJ_S1_TREE(D)              ((D << 4) | MAP_OBJ_S1_TREE_VAL)
#define MAP_OBJ_S1_BUSH(D)              ((D << 4) | MAP_OBJ_S1_BUSH_VAL)
#define MAP_OBJ_S1_STREET_LAMP(D)       ((D << 4) | MAP_OBJ_S1_STREET_LAMP_VAL)
#define MAP_OBJ_S1_TELEGRAPH_POLE(D)    ((D << 4) | MAP_OBJ_S1_TELEGRAPH_POLE_VAL)
#define MAP_OBJ_S1_TURN_SIGN_L(D)       ((D << 4) | MAP_OBJ_S1_TURN_SIGN_L_VAL)
#define MAP_OBJ_S1_TURN_SIGN_R(D)       ((D << 4) | MAP_OBJ_S1_TURN_SIGN_R_VAL)

/* ----------------------------------------------------------------------- */

// No. 5 per depth level for depthset objects (×sub-components: 15, 25 etc.).
// 6 for directly-indexed objects: four size bands where 2–3 share data, 4–5
// are an unshifted/shifted pair. Turn signs: 10 = 5 unflipped + 5 flipped.
typedef struct bitmap {
  u8        width_bytes;
  u8        flags;
  u8        height;
  const u8 *data;
  const u8 *shifted;
} bitmap_t;

typedef struct hittable {
  u8              width;
  const bitmap_t *bitmaps;
} hittable_t;

typedef struct heli_bitmap_inner {
  s8       horz_offset;
  bitmap_t bm;
} heli_bitmap_inner_t;

typedef struct heli_bitmap {
  u8                  y_offset;
  heli_bitmap_inner_t inner;
} heli_bitmap_t;

// Table of 6 part pointers used by draw_helicopter: entries 0-4 point to a
// heli_bitmap_t (body parts, which carry a y_offset); entry 5 points to a
// bare heli_bitmap_inner_t (the rotor, whose Z80 block has no y_offset
// byte). The Z80 never type-checks these, so the C table is a flat array of
// untyped pointers, cast to the right type at each of the two use sites.
typedef const void *heli_part_ptr_t;

/// Depth Set offset
/// (7 is original game sizeof(bitmap_t))
/// M is a bodge factor since the streetlamp values seem to be +2.
#define DEPTHSET_OFFSET(N,M) ((N) * 7 + (M))

#define DEPTHSET_MAX (10)

typedef struct depthset_pair {
  u8 depth;
  u8 offset; // byte offset from 'bitmaps' in parent structure
} depthset_pair_t;

// Conv: In the Z80 data, overhead-spanning objects (bridges) pack an extra
// 10-entry table directly after 'pairs' in ROM: draw_overhead reads past
// pairs[10] with raw pointer arithmetic to reach it. Modelled explicitly
// here rather than as an out-of-bounds read. Only overhead-bridge depthsets
// populate 'spans'; all other depthset_t instances leave it NULL.
typedef struct overhead_span {
  u8        nrows;      // number of scanlines in the span
  const u8 *fill_bytes; // one fill byte per scanline, nrows entries
} overhead_span_t;

typedef struct depthset {
  const bitmap_t        *bitmaps; // -> array of bitmaps
  depthset_pair_t        pairs[DEPTHSET_MAX]; // maps depths to offsets
  const overhead_span_t *spans; // overhead-bridge span table, or NULL
} depthset_t;

// root objects (an array of these) used with routine draw_stretchy_object_left/right
// bottom-most object is given first
typedef struct stretchy {
  // STRETCHY_TYPE_END:   terminator
  // STRETCHY_TYPE_FIXED: draw at height = bitmap->width_bytes - 2 (no perspective scaling)
  // STRETCHY_TYPE_*PC:   draw at given % of the perspective height
  u8                type;
  const depthset_t *set; // Conv: this is always present, can be NULL for
  // final entry
} stretchy_t;

typedef struct obj {
  // Together these define the hit zone [lo, hi). Which field is lo and which
  // is hi swaps between sides: for right-hand objects field1=hi, field2=lo;
  // for left-hand objects field1=lo, field2=hi.
  u8             hit_max_or_min;
  u8             hit_min_or_max;
  u8             impact_speed_cap; // capped with speed → ahc_crash_speed_threshold
  const void    *arg;
  obj_handler_t *handler;
} obj_t;

typedef struct scenedata {
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
} scenedata_t;

/* ----------------------------------------------------------------------- */

#define SPRITE_FRAMES 6   /* bitmaps per sprite sequence (distance LODs / animation frames) */

typedef struct stage {
  u8                backdrop[BACKDROP_LENGTH];
  const u8         *addrof_perp_mugshot_attributes;
  const u8         *addrof_perp_mugshot_bitmap;
  u16               ground_colour;
  const hittable_t *addrof_hittable_objects;
  const void       *addrof_right_hand_handlers;
  const obj_t      *addrof_right_hand_objects;
  const obj_t      *addrof_right_hand_short_pole_object;
  const void       *addrof_left_hand_handlers;
  const obj_t      *addrof_left_hand_objects;
  const obj_t      *addrof_left_hand_short_pole_object;
  const u8         *addrof_perp_description;
  const u8         *addrof_arrest_messages;
  const heli_part_ptr_t *addrof_helicopter_stuff_1;
  const heli_part_ptr_t *addrof_helicopter_stuff_2;

  const bitmap_t  (*bitmaps_stones)[SPRITE_FRAMES];
  const bitmap_t  (*bitmaps_dust)[SPRITE_FRAMES];
  const bitmap_t   *bitmaps_perp_car;
  const bitmap_t   *bitmaps_vehicles[4];

  u8                car_spawn_delay;
  u8                perp_lane_change_base;
  u8                perp_approach_base;

  scenedata_t       stage_data;
  scenedata_t       attract_data;

  const char      **chatter_strings; // Conv: additional
} stage_t;

/* ----------------------------------------------------------------------- */

const stage_t *stages[MAXSTAGE + 2]; /* [0]=pregame, [1..5]=game stages, [6]=end-sequence */

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_STAGES_H */

