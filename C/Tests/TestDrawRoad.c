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
 * draw_road must reset on_dirt_track, dt_tunnel_visible, dr_in_tunnel,
 * and dr_edge_thickness regardless of road content.
 */
static void test_draw_road_resets_state_flags(void)
{
  chqstate_t *state = make_road_state();

  /* Poison flags to ensure draw_road writes them. */
  state->on_dirt_track      = 0xFF;
  state->dt_tunnel_visible  = 0xFF;
  state->dr_in_tunnel       = 0xFF;
  state->dr_edge_thickness  = 0xFF;

  chq_test_draw_road(state);

  assert(state->on_dirt_track     == 0);
  assert(state->dt_tunnel_visible == 0);
  assert(state->dr_in_tunnel      == 0);
  assert(state->dr_edge_thickness == 3);

  chq_destroy(state);
  printf("PASS  draw_road resets state flags\n");
}

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
 * even slots [48..126].  Verify that these differ from their pre-layout
 * values — i.e. layout_road actually did something.
 *
 * Note: SProadright steps by 1 u16 per loop iteration while Aiterations
 * steps by 2, so the "right" source for slot i is xpos_road_right[(i-48)/2+48],
 * not xpos_road_right[i]; a formula test is therefore non-trivial to express
 * here without duplicating build_curve_table logic.  We settle for checking
 * that the written slots are non-zero (road positions for Stage 1 are non-zero)
 * and that centre-left <= centre <= centre-right (internal consistency of the
 * four derived tables).
 */
static void test_layout_road_populates_tables(void)
{
  chqstate_t *state = make_road_state();
  int         nonzero = 0;
  int         i;

  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  for (i = 48; i < 128; i += 2) {
    uint16_t cl = state->xpos_road_centre_left[i];
    uint16_t cr = state->xpos_road_centre_right[i];
    uint16_t c  = state->xpos_road_centre[i];

    if (c != 0) nonzero++;

    /*
     * Road x-coordinates run right-to-left: larger value = further left on screen.
     * xpos_road_centre_left  = centre - quarter_width  (larger: more left)
     * xpos_road_centre_right = centre + quarter_width  (smaller: more right)
     * So numerically: centre_right <= centre <= centre_left.
     */
    if ((uint16_t)(c - cr) > (uint16_t)(cl - cr)) {
      fprintf(stderr, "  slot %d: cr=%u c=%u cl=%u (c not between cr and cl)\n",
              i, cr, c, cl);
    }
    assert((uint16_t)(c - cr) <= (uint16_t)(cl - cr));
  }

  assert(nonzero > 0);

  chq_destroy(state);
  printf("PASS  layout_road: populates centre tables with road geometry\n");
}

/*
 * Full pipeline smoke test: build_height_table → layout_road → draw_road
 * must write some non-zero content into the back buffer.
 *
 * NOTE: draw_road is partially implemented and may trip an assertion in
 * dr_c62e (VALID_BACKBUF) due to the initial DEbackbuf = 0x0100 → 0x0000
 * address computation.  If this test aborts, that assertion is the bug to
 * fix next.
 */
static void test_draw_road_writes_backbuffer(void)
{
  chqstate_t *state = make_road_state();

  memset(state->backbuffer, 0, sizeof(state->backbuffer));

  chq_test_build_height_table(state);
  chq_test_layout_road(state);
  chq_test_draw_road(state);

  assert(backbuf_has_content(state));

  chq_destroy(state);
  printf("PASS  draw_road writes to back buffer\n");
}

/* ----------------------------------------------------------------------- */

int main(void)
{
  speccy_init();

  test_draw_road_resets_state_flags();
  test_build_height_table_writes_table();
  test_layout_road_populates_tables();
  test_draw_road_writes_backbuffer();

  printf("\nAll tests passed.\n");
  return 0;
}
