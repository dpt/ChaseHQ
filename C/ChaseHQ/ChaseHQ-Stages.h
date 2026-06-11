/**
 * ChaseHQ-Stages.h
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

#ifndef CHASEHQ_STAGES_H
#define CHASEHQ_STAGES_H

#include "C99/Types.h"
#include "ChaseHQ-Internal.h"

/* ----------------------------------------------------------------------- */

#define STREND                (1<<7) // string terminating top bit

/* ----------------------------------------------------------------------- */

#define BACKDROP_WIDTH        (10) // bytes
#define BACKDROP_HEIGHT       (24) // rows
#define BACKDROP_LENGTH       (BACKDROP_WIDTH * BACKDROP_HEIGHT)

/* ----------------------------------------------------------------------- */

// Conv: The C version uses IDs for strings and blocks rather than inline
// addresses.

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

#define MAP_ESC                         (0)

#define MAP_CMD_GOTO(ADDR)              MAP_ESC, (0), (ADDR) & 0xFF, (ADDR) >> 8
#define MAP_CMD_FORK_END                MAP_ESC, (1)
#define MAP_CMD_SPLIT(LADDR,RADDR)      MAP_ESC, (2), (LADDR) & 0xFF, (LADDR) >> 8, (RADDR) & 0xFF, (RADDR) >> 8
#define MAP_CMD_STOP_BARRIERS           MAP_ESC, (3)
#define MAP_CMD_UNKNOWN_HAZARD_6        MAP_ESC, (6)
#define MAP_CMD_START_BARRIERS_L        MAP_ESC, (7)
#define MAP_CMD_START_BARRIERS_R        MAP_ESC, (8)
#define MAP_CMD_START_TWO_BARRIERS      MAP_ESC, (9)
#define MAP_CMD_ARROW_OFF               MAP_ESC, (10)
#define MAP_CMD_ARROW_L                 MAP_ESC, (11)
#define MAP_CMD_ARROW_R                 MAP_ESC, (12)
#define MAP_CMD_START_CARS              MAP_ESC, (13)
#define MAP_CMD_STOP_CARS               MAP_ESC, (14)

// Curvature
#define MAP_CURVE_STRAIGHT(D)           (((D) << 4) | 0)
#define MAP_CURVE_RIGHT(D)              (((D) << 4) | 1)
#define MAP_CURVE_RIGHT_HARD(D)         (((D) << 4) | 2)
#define MAP_CURVE_RIGHT_VERY_HARD(D)    (((D) << 4) | 3)
#define MAP_CURVE_LEFT(D)               (((D) << 4) | 9)
#define MAP_CURVE_LEFT_HARD(D)          (((D) << 4) | 10)
#define MAP_CURVE_LEFT_VERY_HARD(D)     (((D) << 4) | 11)

// Height
#define MAP_HEIGHT_UP7(D)               (((D) << 4) | 1)
#define MAP_HEIGHT_UP5(D)               (((D) << 4) | 3)
#define MAP_HEIGHT_UP3(D)               (((D) << 4) | 5)
#define MAP_HEIGHT_UP1(D)               (((D) << 4) | 7)
#define MAP_HEIGHT_LEVEL(D)             (((D) << 4) | 8)
#define MAP_HEIGHT_DOWN1(D)             (((D) << 4) | 9)
#define MAP_HEIGHT_DOWN3(D)             (((D) << 4) | 11)
#define MAP_HEIGHT_DOWN5(D)             (((D) << 4) | 13)
#define MAP_HEIGHT_DOWN7(D)             (((D) << 4) | 15)

// Lanes
#define MAP_LANES_4_VAL                 (0x00)
#define MAP_LANES_3L_VAL                (0x81)
#define MAP_LANES_3R_VAL                (0x82)
#define MAP_LANES_2L_VAL                (0x01)
#define MAP_LANES_2M_VAL                (0x02)
#define MAP_LANES_2R_VAL                (0x03)
#define MAP_LANES_4TO3L_VAL             (0xBD)
#define MAP_LANES_4TO3R_VAL             (0x8E)
#define MAP_LANES_3TO4L_VAL             (0xAD)
#define MAP_LANES_3TO4R_VAL             (0x9E)
#define MAP_LANES_3TO2L_VAL             (0x06)
#define MAP_LANES_3TO2R_VAL             (0x0F)
#define MAP_LANES_2TO3L_VAL             (0x2D)
#define MAP_LANES_2TO3R_VAL             (0x1F)
#define MAP_LANES_TUNNEL_VAL            (0x45)
#define MAP_LANES_TUNNEL_EXIT_VAL       (0x59)
#define MAP_LANES_DIRTTRACK_VAL         (0xC1)
#define MAP_LANES_FORKED_VAL            (0xED)

#define MAP_LANES_LEFT_OFFSET_MASK      (0x03)

#define MAP_LANES_4(D)                  (D), (MAP_LANES_4_VAL)
#define MAP_LANES_3L(D)                 (D), (MAP_LANES_3L_VAL)
#define MAP_LANES_3R(D)                 (D), (MAP_LANES_3R_VAL)
#define MAP_LANES_2L(D)                 (D), (MAP_LANES_2L_VAL)
#define MAP_LANES_2M(D)                 (D), (MAP_LANES_2M_VAL)
#define MAP_LANES_2R(D)                 (D), (MAP_LANES_2R_VAL)
#define MAP_LANES_4TO3L(D)              (D), (MAP_LANES_4TO3L_VAL)
#define MAP_LANES_4TO3R(D)              (D), (MAP_LANES_4TO3R_VAL)
#define MAP_LANES_3TO4L(D)              (D), (MAP_LANES_3TO4L_VAL)
#define MAP_LANES_3TO4R(D)              (D), (MAP_LANES_3TO4R_VAL)
#define MAP_LANES_3TO2L(D)              (D), (MAP_LANES_3TO2L_VAL)
#define MAP_LANES_3TO2R(D)              (D), (MAP_LANES_3TO2R_VAL)
#define MAP_LANES_2TO3L(D)              (D), (MAP_LANES_2TO3L_VAL)
#define MAP_LANES_2TO3R(D)              (D), (MAP_LANES_2TO3R_VAL)
#define MAP_LANES_TUNNEL(D)             (D), (MAP_LANES_TUNNEL_VAL)
#define MAP_LANES_TUNNEL_EXIT(D)        (D), (MAP_LANES_TUNNEL_EXIT_VAL)
#define MAP_LANES_DIRTTRACK(D)          (D), (MAP_LANES_DIRTTRACK_VAL)
#define MAP_LANES_FORKED(D)             (D), (MAP_LANES_FORKED_VAL)

#define MAP_HAZARD_WAIT(T)              (T)

// Objects (names are valid for Stage 1)

#define MAP_OBJ_NONE_VAL                (0) // common to all stages?
#define MAP_OBJ_SHORT_POLE_VAL          (3) // common to all stages?

#define MAP_OBJ_S1_NONE_VAL             (0)
#define MAP_OBJ_S1_TUNNEL_LIGHT_VAL     (1)
#define MAP_OBJ_S1_UNKNOWN_2_VAL        (2)
#define MAP_OBJ_S1_SHORT_POLE_VAL       (3)
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

// Are bitmaps always given in groups of five or six? Or ten?
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
  u8       horz_offset; // might be s8
  bitmap_t bm;
} heli_bitmap_inner_t;

typedef struct heli_bitmap {
  u8                  y_offset;
  heli_bitmap_inner_t inner;
} heli_bitmap_t;

/// Depth Set offset
/// (7 is original game sizeof(bitmap_t))
/// M is a bodge factor since the streetlamp values seem to be +2.
#define DEPTHSET_OFFSET(N,M) ((N) * 7 + (M))

#define DEPTHSET_MAX (10)

typedef struct depthset_pair {
  u8 depth;
  u8 offset; // byte offset from 'bitmaps'
} depthset_pair_t;

typedef struct depthset {
  const bitmap_t *bitmaps; // -> array of bitmaps
  depthset_pair_t pairs[DEPTHSET_MAX]; // maps depths to offsets
} depthset_t;

// root objects (an array of these) used with routine draw_stretchy_object_left/right
// bottom-most object is given first
typedef struct stretchy {
  // 1 => end
  // 2 =>
  // 3 => repeats?
  // otherwise not sure. the value affects height.
  u8                type;
  const depthset_t *set; // Conv: this is always present, can be NULL for
  // final entry
} stretchy_t;

typedef struct obj {
  u8             hit_max_or_min; // max for RIGHT? min for LEFT?
  u8             hit_min_or_max;
  u8             hit_something;
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
  const heli_bitmap_t (*addrof_helicopter_stuff_1)[6];
  const heli_bitmap_t (*addrof_helicopter_stuff_2)[6];

  const bitmap_t  (*bitmaps_stones)[6];
  const bitmap_t  (*bitmaps_dust)[6];
  const bitmap_t   *bitmaps_perp_car;
  const bitmap_t   *bitmaps_vehicles[4];

  u8                car_spawn_delay;
  u8                smash_5d1b;
  u8                smash_perp_delay;

  scenedata_t       stage_data;
  scenedata_t       attract_data;

  const char      **chatter_strings; // Conv: additional
} stage_t;

/* ----------------------------------------------------------------------- */

#define MAX_STAGES (5)
const stage_t *stages[MAX_STAGES];

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_STAGES_H */

