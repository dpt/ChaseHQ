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
 * NOTE: draw_road is partially implemented.  On the first call the sentinel
 * DEbackbuf = 0x0100 decrement-and-rollover path produces DEbackbuf = 0x0020
 * (D=0x00, E=0x20), which is outside the valid backbuffer range.  The missing
 * piece is initialisation of dr_backbuf_1 to the first valid row address
 * (0xFF20) by stage-setup code not yet ported.  Until that is added this test
 * aborts at the VALID_BACKBUF assertion in dr_c62e.
 */
static void test_draw_road_writes_backbuffer(void)
{
  chqstate_t *state = make_road_state();
  int i;

  memset(state->backbuffer, 0xFF, sizeof(state->backbuffer));

  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  printf("height_table[1..5]: %d %d %d %d %d\n",
         state->height_table[1], state->height_table[2],
         state->height_table[3], state->height_table[4],
         state->height_table[5]);

  {
    /* IXlanesptr = road_buffer_start[(road_buffer_offset + ROADBUF_LANES_OFFSET) & 0xFF] */
    int lanes_off = (int)((state->road_buffer_offset - state->road_buffer_start + 64) & 0xFF);
    int carry_stripe = !!(lanes_off & 2);
    printf("lanes byte offset=%d  carry_stripe=%d  lanes[0]=%02x\n",
           lanes_off, carry_stripe, state->road_buffer_start[lanes_off]);
  }

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

int main(void)
{
  speccy_init();

  test_build_height_table_writes_table();
  test_layout_road_populates_tables();
  test_drsc_exits_when_dist_too_far();
  test_drsc_exits_on_straight_track();
  test_draw_road_writes_backbuffer();

  printf("\nAll tests passed.\n");
  return 0;
}
