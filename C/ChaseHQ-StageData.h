// ChaseHQ-StageData.h
//
// Chase H.Q. code model
//
// by dpt

// vim: ts=8 sts=2 sw=2 et

#ifndef CHASEHQ_STAGEDATA_H
#define CHASEHQ_STAGEDATA_H

#include "Types.h"
#include "ChaseHQ.h"

#define LOD_NOMASK (0 << 0)
#define LOD_MASKED (1 << 0)

typedef struct lod {
  u8        width_bytes;
  u8        flags;
  u8        height;
  const u8 *bitmap;
  const u8 *shifted;
} lod_t;

typedef struct stage {
  u8            backdrop[BACKDROP_LENGTH];
  const u8     *addrof_perp_mugshot_attributes;
  const u8     *addrof_perp_mugshot_bitmap;
  u16           ground_colour;
  const u8     *addrof_hittable_objects;
  const u8     *addrof_right_hand_handlers;
  const u8     *addrof_right_hand_objects;
  const u8     *addrof_right_hand_short_pole_object;
  const u8     *addrof_left_hand_handlers;
  const u8     *addrof_left_hand_objects;
  const u8     *addrof_left_hand_short_pole_object;
  const u8     *addrof_perp_description;
  const u8     *addrof_arrest_messages;
  const u8     *addrof_helicopter_stuff_1;
  const u8     *addrof_helicopter_stuff_2;

  const lod_t (*lods_stones)[6];
  const lod_t (*lods_dust)[6];
  const lod_t  *lods_perp_car;
  const lod_t  *lods_vehicles[4];

  u8            car_spawn_delay;
  u8            smash_5d1b;
  u8            smash_perp_delay;

  scenedata_t   stage_data;
  scenedata_t   attract_data;
} stage_t;

#define MAX_STAGEDATA (5)

const stage_t *stages[MAX_STAGEDATA];

#endif /* CHASEHQ_STAGEDATA_H */

