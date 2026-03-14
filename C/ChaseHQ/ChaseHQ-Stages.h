// ChaseHQ-Stages.h
//
// Chase H.Q. code model
//
// by dpt

#ifndef CHASEHQ_STAGES_H
#define CHASEHQ_STAGES_H

#include "C99/Types.h"
#include "ChaseHQ.h"

struct hittable {
  u8                width;
  const lod_t      *lods;
};

struct obj {
  u8                hit_max_or_min; // max for RIGHT? min for LEFT?
  u8                hit_min_or_max;
  u8                hit_something;
  const void       *arg;
  obj_handler_t    *handler;
};

struct heli_lod_inner {
  u8                tbd2; // might be s8
  lod_t             lod;
};

struct heli_lod {
  u8                tbd1;
  heli_lod_inner_t  inner;
};

typedef struct stage {
  u8                backdrop[BACKDROP_LENGTH];
  const u8         *addrof_perp_mugshot_attributes;
  const u8         *addrof_perp_mugshot_bitmap;
  u16               ground_colour;
  const hittable_t  *addrof_hittable_objects;
  const void       *addrof_right_hand_handlers;
  const obj_t      *addrof_right_hand_objects;
  const obj_t      *addrof_right_hand_short_pole_object;
  const void       *addrof_left_hand_handlers;
  const obj_t      *addrof_left_hand_objects;
  const obj_t      *addrof_left_hand_short_pole_object;
  const u8         *addrof_perp_description;
  const u8         *addrof_arrest_messages;
  const heli_lod_t (*addrof_helicopter_stuff_1)[6];
  const heli_lod_t (*addrof_helicopter_stuff_2)[6];

  const lod_t     (*lods_stones)[6];
  const lod_t     (*lods_dust)[6];
  const lod_t      *lods_perp_car;
  const lod_t      *lods_vehicles[4];

  u8                car_spawn_delay;
  u8                smash_5d1b;
  u8                smash_perp_delay;

  scenedata_t       stage_data;
  scenedata_t       attract_data;

  const char      **chatter_strings; // Conv: additional
} stage_t;

#define MAX_STAGES (5)

const stage_t *stages[MAX_STAGES];

#endif /* CHASEHQ_STAGES_H */

