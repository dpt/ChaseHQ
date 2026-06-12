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

static int backbuf_has_content(const chqstate_t *state)
{
  int i;
  for (i = 0; i < BACKBUFFER_LENGTH; i++)
    if (state->backbuffer[i] != 0)
      return 1;
  return 0;
}

/* ----------------------------------------------------------------------- */
/* Tests                                                                    */
/* ----------------------------------------------------------------------- */

/*
 * build_height_table must write non-sentinel values into table_e300[1..21]
 * for a road with non-zero height data, and leave table_e300[0] unchanged.
 */
static void test_build_height_table_writes_table(void)
{
  chqstate_t *state = make_road_state();
  int         changed = 0;
  int         i;

  /* Sentinel must not be overwritten. */
  uint8_t sentinel = state->table_e300[0];

  chq_test_build_height_table(state);

  assert(state->table_e300[0] == sentinel);

  /* At least some entries should differ from zero. */
  for (i = 1; i <= 21; i++)
    if (state->table_e300[i] != 0)
      changed++;

  assert(changed > 0);

  chq_destroy(state);
  printf("PASS  build_height_table writes table_e300\n");
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
 * NOTE: draw_road is partially implemented.  The Ahi-ordering bug in
 * dr_c55f_unfilled_path (reads high byte before HI_DEC instead of after)
 * causes an invalid backbuffer address when called with the initial sentinel
 * DEbackbuf = 0x0100.  This test will abort at the VALID_BACKBUF assertion
 * in dr_c62e until that is fixed.
 */
static void test_draw_road_writes_backbuffer(void)
{
  chqstate_t *state = make_road_state();

  memset(state->backbuffer, 0, sizeof(state->backbuffer));

  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  /* Poison flags to ensure draw_road writes them. */
  state->on_dirt_track     = 0xFF;
  state->dt_tunnel_visible = 0xFF;
  state->dr_in_tunnel      = 0xFF;
  state->dr_edge_thickness = 0xFF;

  chq_test_draw_road(state);

  assert(state->on_dirt_track     == 0);
  assert(state->dt_tunnel_visible == 0);
  assert(state->dr_in_tunnel      == 0);
  assert(state->dr_edge_thickness == 3);

  assert(backbuf_has_content(state));

  chq_destroy(state);
  printf("PASS  draw_road writes to back buffer\n");
}

/* ----------------------------------------------------------------------- */

int main(void)
{
  speccy_init();

  test_build_height_table_writes_table();
  test_layout_road_populates_tables();
  test_draw_road_writes_backbuffer();

  printf("\nAll tests passed.\n");
  return 0;
}
