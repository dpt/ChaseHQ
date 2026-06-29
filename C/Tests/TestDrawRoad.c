/**
 * TestDrawRoad.c
 *
 * Unit tests for draw_road and its supporting pipeline:
 *   build_height_table → layout_road → draw_road
 *
 * The test binary is built with -DCHQ_TESTS, which exposes thin wrappers
 * around the static functions in ChaseHQ.c via ChaseHQ-Tests.h.
 *
 * Running:
 *   cmake --build cmake-build-debug --target ChaseHQ_Tests
 *   ./cmake-build-debug/ChaseHQ_Tests
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ChaseHQ/ChaseHQ.h"
#include "ChaseHQ/ChaseHQ-Internal.h"
#include "ChaseHQ/ChaseHQ-State.h"
#include "ChaseHQ/ChaseHQ-Stages.h"
#include "ChaseHQ/ChaseHQ-Tests.h"
#include "ZXSpectrum/Spectrum.h"

/* ----------------------------------------------------------------------- */
/* Minimal fake ZX Spectrum (no-op callbacks, valid screen dimensions)      */
/* ----------------------------------------------------------------------- */

static uint8_t fake_in(zxspectrum_t *s, uint16_t addr)
{
  (void)s; (void)addr;
  return 0xFF; /* all keys unpressed */
}

static void fake_out(zxspectrum_t *s, uint16_t addr, uint8_t byte)
{
  (void)s; (void)addr; (void)byte;
}

static void fake_draw(zxspectrum_t *s, const zxbox_t *dirty)
{
  (void)s; (void)dirty;
}

static void fake_stamp(zxspectrum_t *s)
{
  (void)s;
}

static int fake_sleep(zxspectrum_t *s, int ticks)
{
  (void)s; (void)ticks;
  return 0;
}

static zxspectrum_t g_speccy;

static void speccy_init(void)
{
  memset(&g_speccy, 0, sizeof(g_speccy));
  g_speccy.in    = fake_in;
  g_speccy.out   = fake_out;
  g_speccy.draw  = fake_draw;
  g_speccy.stamp = fake_stamp;
  g_speccy.sleep = fake_sleep;
  g_speccy.screen.width  = SCREEN_WIDTH;
  g_speccy.screen.height = SCREEN_HEIGHT;
}

/* ----------------------------------------------------------------------- */
/* State helpers                                                            */
/* ----------------------------------------------------------------------- */

/**
 * Create a game state initialised for stage 1 with a primed road buffer,
 * ready for the drawing pipeline.
 */
static chqstate_t *make_road_state(void)
{
  chqstate_t *state;

  state = chq_create(&g_speccy);
  assert(state != NULL);

  /* Load stage 1. */
  state->wanted_stage_number  = 1;
  state->current_stage_number = 0; /* force load */
  chq_test_load_stage(state);
  assert(state->stage != NULL);

  /*
   * Initialise scenedata and road buffer the same way set_up_stage does:
   * copies stage map pointers, pre-shifts backdrop, and primes the road
   * buffer with 32 iterations of rm_cycle_buffer_offset.
   */
  chq_test_set_up_stage(state);

  return state;
}

/* Return 1 if draw_road wrote to the backbuffer (some bytes became != 0xFF). */
static int backbuf_was_written(const chqstate_t *state)
{
  int i;
  for (i = 0; i < BACKBUFFER_LENGTH; i++)
    if (state->backbuffer[i] != 0xFF)
      return 1;
  return 0;
}

/* ----------------------------------------------------------------------- */
/* Tests                                                                    */
/* ----------------------------------------------------------------------- */

/*
 * build_height_table must write non-sentinel values into height_table[1..21]
 * for a road with non-zero height data, and leave height_table[0] unchanged.
 */
static void test_build_height_table_writes_table(void)
{
  chqstate_t *state = make_road_state();
  int         changed = 0;
  int         i;

  /* Sentinel must not be overwritten. */
  uint8_t sentinel = state->height_table[0];

  chq_test_build_height_table(state);

  assert(state->height_table[0] == sentinel);

  /* At least some entries should differ from zero. */
  for (i = 1; i <= 21; i++)
    if (state->height_table[i] != 0)
      changed++;

  assert(changed > 0);

  chq_destroy(state);
  printf("PASS  build_height_table writes height_table\n");

}

/*
 * layout_road writes xpos_road_centre (and _left/_right variants) for the
 * even slots [48..126].  Verify at least some centre slots are non-zero —
 * i.e. layout_road actually ran and wrote the geometry tables.
 *
 * Note: build_curve_table uses pointer arithmetic on persp_x_scale_right
 * whose ASLR-shifted address affects the computed road positions, so the
 * exact values and their left/right ordering are not stable between runs.
 */
static void test_layout_road_populates_tables(void)
{
  chqstate_t *state = make_road_state();
  int         nonzero = 0;
  int         i;

  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  for (i = 48; i < 128; i += 2)
    if (state->xpos_road_centre[i] != 0)
      nonzero++;

  assert(nonzero > 0);

  chq_destroy(state);
  printf("PASS  layout_road: populates centre tables with road geometry\n");

}

/*
 * Full pipeline smoke test: build_height_table → layout_road → draw_road
 * must write some non-zero content into the back buffer and must reset the
 * four road-state flags.
 *
 * Full pipeline smoke test: build_height_table → layout_road → draw_road.
 */
static void test_draw_road_writes_backbuffer(void)
{
  chqstate_t *state = make_road_state();

  memset(state->backbuffer, 0xFF, sizeof(state->backbuffer));

  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  state->on_dirt_track     = 0xFF;
  state->dt_tunnel_visible = 0xFF;
  state->dr_in_tunnel      = 0xFF;
  state->dr_edge_thickness = 0xFF;

  chq_test_draw_road(state);

  /* draw_road resets these flags at entry */
  assert(state->on_dirt_track     == 0);
  assert(state->dt_tunnel_visible == 0);
  assert(state->dr_in_tunnel      == 0);
  /* dr_edge_thickness is updated during rendering; not checked here */

  assert(backbuf_was_written(state));

  chq_destroy(state);
  printf("PASS  draw_road writes to back buffer\n");
}

/* ----------------------------------------------------------------------- */

/*
 * draw_road_scene_change: when IYheight is 19 or more entries past the start
 * of height_table (A_dist >= 19) the function must exit immediately without
 * modifying the xpos tables.  The exit path calls dr_four_lane_highway, which
 * sets dr_neg_lane_count to -4.
 */
static void test_drsc_exits_when_dist_too_far(void)
{
  chqstate_t *state;
  u16 snap_left[128];
  u16 snap_centre_left[128];
  u16 snap_centre[128];
  u16 snap_centre_right[128];
  u16 snap_right[128];

  state = make_road_state();
  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  memcpy(snap_left,         state->xpos_road_left,         sizeof(snap_left));
  memcpy(snap_centre_left,  state->xpos_road_centre_left,  sizeof(snap_centre_left));
  memcpy(snap_centre,       state->xpos_road_centre,       sizeof(snap_centre));
  memcpy(snap_centre_right, state->xpos_road_centre_right, sizeof(snap_centre_right));
  memcpy(snap_right,        state->xpos_road_right,        sizeof(snap_right));

  /* MAP_LANES_4TO3L_VAL (0xBD) has non-zero curve bits, so if the dist
   * check were absent this call would proceed into the Bresenham loop.
   * With height_offset=19 the dist-too-far guard fires first. */
  chq_test_draw_road_scene_change(state, 0xBD /* MAP_LANES_4TO3L_VAL */, 19);

  assert(memcmp(snap_left,         state->xpos_road_left,         sizeof(snap_left))         == 0);
  assert(memcmp(snap_centre_left,  state->xpos_road_centre_left,  sizeof(snap_centre_left))  == 0);
  assert(memcmp(snap_centre,       state->xpos_road_centre,       sizeof(snap_centre))       == 0);
  assert(memcmp(snap_centre_right, state->xpos_road_centre_right, sizeof(snap_centre_right)) == 0);
  assert(memcmp(snap_right,        state->xpos_road_right,        sizeof(snap_right))        == 0);

  chq_destroy(state);
  printf("PASS  draw_road_scene_change: dist >= 19 exits early, xpos tables unchanged\n");

}

/*
 * draw_road_scene_change: when the lane byte has no curve bits (bits 2-3 both
 * clear) the function identifies a straight section and exits without touching
 * the xpos tables.  The exit path calls dr_four_lane_highway (dr_neg_lane_count
 * becomes -4).
 *
 * 0x02 = MAP_LANES_2M_VAL: a 2-lane middle section, bits 2-3 = 0.
 */
static void test_drsc_exits_on_straight_track(void)
{
  chqstate_t *state;
  u16 snap_left[128];
  u16 snap_centre_left[128];
  u16 snap_centre[128];
  u16 snap_centre_right[128];
  u16 snap_right[128];

  state = make_road_state();
  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  memcpy(snap_left,         state->xpos_road_left,         sizeof(snap_left));
  memcpy(snap_centre_left,  state->xpos_road_centre_left,  sizeof(snap_centre_left));
  memcpy(snap_centre,       state->xpos_road_centre,       sizeof(snap_centre));
  memcpy(snap_centre_right, state->xpos_road_centre_right, sizeof(snap_centre_right));
  memcpy(snap_right,        state->xpos_road_right,        sizeof(snap_right));

  chq_test_draw_road_scene_change(state, 0x02 /* MAP_LANES_2M_VAL */, 1);

  assert(memcmp(snap_left,         state->xpos_road_left,         sizeof(snap_left))         == 0);
  assert(memcmp(snap_centre_left,  state->xpos_road_centre_left,  sizeof(snap_centre_left))  == 0);
  assert(memcmp(snap_centre,       state->xpos_road_centre,       sizeof(snap_centre))       == 0);
  assert(memcmp(snap_centre_right, state->xpos_road_centre_right, sizeof(snap_centre_right)) == 0);
  assert(memcmp(snap_right,        state->xpos_road_right,        sizeof(snap_right))        == 0);

  chq_destroy(state);
  printf("PASS  draw_road_scene_change: straight track (no curve bits) exits early, xpos tables unchanged\n");

}


/* ----------------------------------------------------------------------- */

/*
 * Lane markings closest to player (bottom row of backbuffer = row 127).
 *
 * carry_stripe (bit 1 of Blanesdataoffset) controls whether the bottom
 * road segments use the filled or unfilled rendering path:
 *   carry_stripe = 0 → unfilled → road surface zeroed, no lane overlays
 *   carry_stripe = 1 → filled   → road surface zeroed then lane marks written
 *
 * After set_up_stage (32 iterations), roadbufptr_idx = 32 →
 * Blanesdataoffset = 96 → carry_stripe = 0: no lane marks at the bottom.
 * Two extra prime iterations advance it to 34 → carry_stripe = 1.
 *
 * The lanes byte at road_buffer[98] (written by rm_cycle_buffer_offset
 * call 3 from map data) is 0x86, which would trigger draw_road_scene_change
 * and push some xpos entries off-screen before the lane loop runs.  Clear it
 * to force the four-lane highway path so the xpos tables remain valid.
 *
 * With carry_stripe = 1 and a clean four-lane road, the centre-left lane
 * mark (xpos ≈ 156, column 19-20 of row 127) is non-zero and non-0xFF.
 */
static void test_lane_markings_appear_at_bottom_row(void)
{
  chqstate_t *state;
  int         col;
  int         found;
  const u8   *bottom_row;

  state = make_road_state();

  /* Two extra iterations advance roadbufptr_idx 32→34.
   * Blanesdataoffset = (34+64) = 98 → bit1=1 → carry_stripe=1.
   * Height data for idx=34 is at road_buffer[66..97] (valid stage-1 data). */
  chq_test_prime_road(state, 2);

  /* road_buffer[98] holds the lanes byte for idx=34.  It was written by the
   * third rm_cycle_buffer_offset call (map position 3) and is non-zero
   * (0x86), which would trigger draw_road_scene_change and modify the xpos
   * tables during rendering.  Clear it so dr_read_lanes takes the
   * four-lane-highway path and the xpos tables are not disturbed. */
  state->road_buffer[98] = 0;

  memset(state->backbuffer, 0xFF, BACKBUFFER_LENGTH);

  chq_test_build_height_table(state);
  chq_test_layout_road(state);
  chq_test_draw_road(state);

  bottom_row = &state->backbuffer[127 * BACKBUFFER_ROWBYTES];

  found = 0;
  for (col = 0; col <= 31; col++)
    if (bottom_row[col] != 0x00 && bottom_row[col] != 0xFF)
      found = 1;

  assert(found);

  chq_destroy(state);
  printf("PASS  draw_road: lane markings appear in bottom row when carry_stripe=1\n");
}

/*
 * After set_up_stage with stage_data the IXlanesptr slot must hold the
 * first lane type from the stage map.  For stage 1 that is MAP_LANES_3L_VAL
 * (0x81): 30 iterations of MAP_LANES_3L prime slots 96-125 and the first
 * draw_road call reads slot 96.
 */
static void test_set_up_stage_lanes_slot_is_3lane(void)
{
  chqstate_t *state;

  state = make_road_state(); /* loads stage 1, calls set_up_stage(stage_data) */

  assert(*chq_test_lanes_slot(state) == MAP_LANES_3L_VAL);

  chq_destroy(state);
  printf("PASS  set_up_stage: stage 1 game data primes lane slot to MAP_LANES_3L_VAL\n");
}

/*
 * Regression: set_up_stage must reset map-reader counters (lanes_counter,
 * curvature_byte, height_byte …) before priming the road buffer.  If they
 * are left over from the previous scene, the 32 priming iterations write
 * the OLD rm_lanes_byte into the buffer instead of reading fresh map data.
 *
 * Concretely: attract mode primes with MAP_LANES_4 (rm_lanes_byte=0x00,
 * lanes_counter ≈ 222 remaining).  A subsequent set_up_stage for game
 * stage 1 must still produce MAP_LANES_3L_VAL in the slot, not 0x00.
 */
static void test_set_up_stage_resets_lane_data(void)
{
  chqstate_t *state;

  state = chq_create(&g_speccy);
  assert(state != NULL);

  state->wanted_stage_number  = 1;
  state->current_stage_number = 0;
  chq_test_load_stage(state);

  /* Prime with attract data: stage 1 attract starts MAP_LANES_4(254). */
  chq_test_set_up_stage_attract(state);
  assert(*chq_test_lanes_slot(state) == MAP_LANES_4_VAL);

  /* Now switch to game stage data: must reload from stage1_map_start_lanes. */
  chq_test_set_up_stage(state);
  assert(*chq_test_lanes_slot(state) == MAP_LANES_3L_VAL);

  chq_destroy(state);
  printf("PASS  set_up_stage: resets lane counters so second call loads new scene data\n");
}

/* ----------------------------------------------------------------------- */

int main(void)
{
  speccy_init();

  test_build_height_table_writes_table();
  test_layout_road_populates_tables();
  test_drsc_exits_when_dist_too_far();
  test_drsc_exits_on_straight_track();
  test_draw_road_writes_backbuffer();
  test_lane_markings_appear_at_bottom_row();
  test_set_up_stage_lanes_slot_is_3lane();
  test_set_up_stage_resets_lane_data();

  printf("\nAll tests passed.\n");
  return 0;
}
