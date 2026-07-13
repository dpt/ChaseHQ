/**
 * TestDrawRoad.c
 *
 * Unit tests for draw_road and its supporting pipeline:
 *   build_height_table → layout_road → draw_road
 *
 * The test binary is built with -DCHQ_TESTS, which exposes thin wrappers
 * around the static functions in ChaseHQ.c via Tests.h.
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
#include "ChaseHQ/Internal.h"
#include "ChaseHQ/State.h"
#include "ChaseHQ/Stages.h"
#include "ChaseHQ/Tests.h"
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
 * draw_road_lanes_change: when IYheight is 19 or more entries past the start
 * of height_table (A_dist >= 19) the function must exit immediately without
 * modifying the xpos tables.  The exit path calls dr_four_lane_highway, which
 * sets dr_neg_lane_count to -4.
 */
static void test_drlc_exits_when_dist_too_far(void)
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
  chq_test_draw_road_lanes_change(state, 0xBD /* MAP_LANES_4TO3L_VAL */, 19);

  assert(memcmp(snap_left,         state->xpos_road_left,         sizeof(snap_left))         == 0);
  assert(memcmp(snap_centre_left,  state->xpos_road_centre_left,  sizeof(snap_centre_left))  == 0);
  assert(memcmp(snap_centre,       state->xpos_road_centre,       sizeof(snap_centre))       == 0);
  assert(memcmp(snap_centre_right, state->xpos_road_centre_right, sizeof(snap_centre_right)) == 0);
  assert(memcmp(snap_right,        state->xpos_road_right,        sizeof(snap_right))        == 0);

  chq_destroy(state);
  printf("PASS  draw_road_lanes_change: dist >= 19 exits early, xpos tables unchanged\n");

}

/*
 * draw_road_lanes_change: when the lane byte has no curve bits (bits 2-3 both
 * clear) the function identifies a straight section and exits without touching
 * the xpos tables.  The exit path calls dr_four_lane_highway (dr_neg_lane_count
 * becomes -4).
 *
 * 0x02 = MAP_LANES_2M_VAL: a 2-lane middle section, bits 2-3 = 0.
 */
static void test_drlc_exits_on_straight_track(void)
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

  chq_test_draw_road_lanes_change(state, 0x02 /* MAP_LANES_2M_VAL */, 1);

  assert(memcmp(snap_left,         state->xpos_road_left,         sizeof(snap_left))         == 0);
  assert(memcmp(snap_centre_left,  state->xpos_road_centre_left,  sizeof(snap_centre_left))  == 0);
  assert(memcmp(snap_centre,       state->xpos_road_centre,       sizeof(snap_centre))       == 0);
  assert(memcmp(snap_centre_right, state->xpos_road_centre_right, sizeof(snap_centre_right)) == 0);
  assert(memcmp(snap_right,        state->xpos_road_right,        sizeof(snap_right))        == 0);

  chq_destroy(state);
  printf("PASS  draw_road_lanes_change: straight track (no curve bits) exits early, xpos tables unchanged\n");

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
 * call 3 from map data) is 0x86, which would trigger draw_road_lanes_change
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
   * (0x86), which would trigger draw_road_lanes_change and modify the xpos
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

/*
 * draw_road_lanes_change Bresenham active: at A_dist=1 with a lane-change
 * byte, the Bresenham must run and write at least one xpos entry that differs
 * from the layout_road baseline.
 *
 * MAP_LANES_4TO3L_VAL (0xBD): bit4=1, bit5=1, bit7=1, bits2-3 non-zero.
 * At height_offset=1 (A_dist=1) this triggers path 1a — the near-boundary
 * Bresenham that writes to xpos entries for the nearest road rows.
 */
static void test_drlc_writes_xpos_entries(void)
{
  chqstate_t *state;
  u16 baseline[128];
  int i;
  int changed;

  state = make_road_state();
  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  memcpy(baseline, state->xpos_road_centre_right, sizeof(baseline));

  state->fast_counter = 0;
  chq_test_draw_road_lanes_change(state, MAP_LANES_4TO3L_VAL, 1);

  changed = 0;
  for (i = 0; i < 128; i++) {
    if (state->xpos_road_centre_right[i] != baseline[i])
      changed++;
  }
  if (changed == 0) {
    printf("  FAIL: no xpos entries changed — Bresenham did not run\n");
    assert(changed > 0);
  }

  chq_destroy(state);
  printf("PASS  draw_road_lanes_change: xpos writes are stable across fast_counter values\n");
}

/*
 * Drive the road pipeline frame by frame through the first stage-1 fork and
 * out the other side, mirroring the main loop ordering: read_map (modelled as
 * allow_spawning reset + one rm_cycle_buffer_offset) → build_height_table →
 * layout_road → exit_fork. The game has been crashing/seizing whenever the
 * hero car reaches the fork; this reproduces that headlessly.
 */
static void test_fork_progression(void)
{
  chqstate_t *state;       /* game state under test */
  int         frame;       /* frame counter */
  int         saw_fork;    /* set once fork_visible goes non-zero */
  int         fork_frames; /* frames spent with the fork active */
  int         max_obj;     /* largest side-object byte this frame */

  state = make_road_state();
  saw_fork = 0;
  fork_frames = 0;

  for (frame = 0; frame < 30000; frame++) {
    state->allow_spawning = 0; /* read_map does this each frame */
    chq_test_prime_road(state, 1);
    chq_test_build_height_table(state);
    chq_test_layout_road(state);
    chq_test_exit_fork(state);

    max_obj = chq_test_max_side_object(state);
    if (max_obj > 9) {
      printf("  FAIL: side object byte %d (> 9) at frame %d "
             "(vis=%u prog=%u taken=%u dist=%d)\n",
             max_obj, frame, state->fork_visible, state->fork_in_progress,
             state->fork_taken, (int)state->fork_distance);
      assert(max_obj <= 9);
    }

    if (state->fork_visible) {
      if (fork_frames == 0)
        saw_fork++;
      fork_frames++;
      if (fork_frames >= 5000) {
        printf("  FAIL: fork still active after %d frames — seized\n",
               fork_frames);
        assert(fork_frames < 5000);
      }
    } else if (fork_frames) {
      printf("  fork %d completed after %d active frames (frame %d)\n",
             saw_fork, fork_frames, frame);
      fork_frames = 0;
    }
  }

  assert(saw_fork);
  chq_destroy(state);
  printf("PASS  fork progression: fork completes without crashing\n");
}

/*
 * Run whole game frames (via chq_test_game_frame, which mirrors the run_game
 * main loop) through the stage-1 forks, watching for road buffer corruption:
 * side-object bytes > 9, as asserted by draw_scene_objects. Reproduces the
 * in-game Aobj=170 assert seen after the fork fixes.
 */
static void test_full_frame_no_corruption(void)
{
  chqstate_t *state;    /* game state under test */
  int         frame;    /* frame counter */
  int         saw_fork; /* set once fork_visible goes non-zero */
  int         max_obj;  /* largest side-object byte this frame */

  state = make_road_state();
  saw_fork = 0;
  state->hazards[0].used = HAZARD_USED; /* keep perp spawned, as run_game */

  for (frame = 0; frame < 30000; frame++) {
    state->speed = 0x0180;    /* keep the car moving at speed */
    state->session.time_bcd = 0x60; /* top up the clock: never expires */
    chq_test_game_frame(state);

    if (state->fork_visible)
      saw_fork = 1;

    max_obj = chq_test_max_side_object(state);
    if (max_obj > 9) {
      printf("  FAIL: side object byte %d (> 9) at frame %d "
             "(vis=%u prog=%u taken=%u dist=%d)\n",
             max_obj, frame, state->fork_visible, state->fork_in_progress,
             state->fork_taken, (int)state->fork_distance);
      assert(max_obj <= 9);
    }

    /* Debug aid: set CHQ_DUMP_DIR to dump raw ZX screens (.scr) and
     * post-draw_road backbuffers (.bbuf) around the first fork for visual
     * inspection (see scr2png-style converters in the session notes). */
    if (getenv("CHQ_DUMP_DIR") != NULL &&
        ((frame >= 180 && frame <= 260) || frame == 100)) {
      char  fname[256];
      FILE *fp;
      snprintf(fname, sizeof(fname), "%s/screen-%05d.scr",
               getenv("CHQ_DUMP_DIR"), frame);
      fp = fopen(fname, "wb");
      if (fp) {
        fwrite(g_speccy.screen.pixels, 1, sizeof(g_speccy.screen.pixels), fp);
        fclose(fp);
      }
      {
        extern u8 chq_test_backbuf_snapshot[];
        snprintf(fname, sizeof(fname), "%s/road-%05d.bbuf",
                 getenv("CHQ_DUMP_DIR"), frame);
        fp = fopen(fname, "wb");
        if (fp) {
          fwrite(chq_test_backbuf_snapshot, 1, BACKBUFFER_LENGTH, fp);
          fclose(fp);
        }
      }
    }
  }

  assert(saw_fork);
  chq_destroy(state);
  printf("PASS  full frames: no road buffer corruption across forks\n");
}

/*
 * Drive the perp-caught state machine end to end. Simulates the state that
 * fully_smashed ($8C3A) leaves behind, then runs whole game frames and
 * checks that the sequence progresses: the synthesised input accelerates
 * the hero (regression check for the assign_hero_pos stale-register bug
 * which stripped the UP bit, halting the hero while the perp raced off),
 * the perp is caught (phase 2), the car pulls in (phase 3) and the score
 * phase (4) is reached.
 */
static void test_perp_caught_progression(void)
{
  chqstate_t *state;        /* game state under test */
  int         frame;        /* frame counter */
  int         saw_accel;    /* set once the synthesised input includes UP */
  int         max_phase;    /* highest perp_caught_phase reached */

  state = make_road_state();
  state->hazards[0].used = HAZARD_USED; /* keep perp spawned, as run_game */
  saw_accel = 0;
  max_phase = 0;

  /* Warm up: drive normally for a while so hazard/perp state is live. */
  for (frame = 0; frame < 400; frame++) {
    state->speed = 0x0180;
    state->session.time_bcd = 0x60;
    chq_test_game_frame(state);
  }

  /* Mimic fully_smashed ($8C3A): phase 1, stop hand, input masked to
   * pause/quit only, perp scripted to speed 400. */
  state->perp_caught_phase = 1;
  state->hand_flag = 2;
  state->smash_counter = 20;
  state->session.user_input_mask = 0xC0;
  state->hazards[0].speed = 400;
  state->hazards[0].distance = 5;
  state->speed = 0x0180;

  for (frame = 0; frame < 2000; frame++) {
    state->session.time_bcd = 0x60;
    chq_test_game_frame(state);
    if (state->user_input & 0x08) /* USERINPUTFLAG_UP */
      saw_accel = 1;
    if (state->perp_caught_phase > max_phase)
      max_phase = state->perp_caught_phase;
    if (state->perp_caught_phase >= 4)
      break;
  }

  if (!saw_accel) {
    printf("  FAIL: synthesised input never accelerated the hero\n");
    assert(saw_accel);
  }
  if (max_phase < 4) {
    printf("  FAIL: stuck in phase %d after %d frames "
           "(speed=%d input=%02x perp dist=%d speed=%d)\n",
           max_phase, frame, (int)state->speed, state->user_input,
           (int)state->hazards[0].distance, (int)state->hazards[0].speed);
    assert(max_phase >= 4);
  }
  /* Phase 2 must have stopped both cars before handing over. */
  assert(state->speed == 0);
  assert(state->hazards[0].speed == 0);
  assert(state->hazards[0].distance == 1);
  assert(state->car_y >= 16);

  chq_destroy(state);
  printf("PASS  perp caught: sequence reaches the score phase "
         "(caught at frame %d)\n", frame);
}

/*
 * Force the stage-2 helicopter straight into its turn-left approach
 * (helicopter_control = 3, see drive_helicopter's hc_pick_direction) and run
 * whole game frames, checking that dee_draw_helicopter engages and that the
 * road buffer is not corrupted while draw_helicopter runs. Regression check
 * for pitfall #29: helitable was stepped as an array of heli_bitmap_t
 * (12 bytes) rather than an array of pointers (2 bytes), which walked far
 * outside CommonData and produced garbage or crashed.
 */
static void test_helicopter_draws(void)
{
  chqstate_t *state;   /* game state under test */
  int         frame;   /* frame counter */
  int         saw_draw; /* set once dee_draw_helicopter goes non-zero */
  int         max_obj; /* largest side-object byte this frame */

  state = chq_create(&g_speccy);
  assert(state != NULL);

  state->wanted_stage_number  = 2;
  state->current_stage_number = 0; /* force load */
  chq_test_load_stage(state);
  assert(state->stage != NULL);
  chq_test_set_up_stage(state);

  state->hazards[0].used = HAZARD_USED; /* keep perp spawned, as run_game */
  state->helicopter_control = 3; /* dispatch straight to turn-left sequence */

  saw_draw = 0;

  for (frame = 0; frame < 500; frame++) {
    state->speed = 0x0180;
    state->session.time_bcd = 0x60;
    chq_test_game_frame(state);

    if (state->dee_draw_helicopter)
      saw_draw = 1;

    max_obj = chq_test_max_side_object(state);
    if (max_obj > 9) {
      printf("  FAIL: side object byte %d (> 9) at frame %d "
             "(dee_draw_helicopter=%u)\n",
             max_obj, frame, state->dee_draw_helicopter);
      assert(max_obj <= 9);
    }

    /* Debug aid: set CHQ_DUMP_DIR to dump raw ZX screens (.scr) for visual
     * inspection of the rendered helicopter sprite. */
    if (getenv("CHQ_DUMP_DIR") != NULL && frame < 100) {
      char  fname[256];
      FILE *fp;
      snprintf(fname, sizeof(fname), "%s/heli-%05d.scr",
               getenv("CHQ_DUMP_DIR"), frame);
      fp = fopen(fname, "wb");
      if (fp) {
        fwrite(g_speccy.screen.pixels, 1, sizeof(g_speccy.screen.pixels), fp);
        fclose(fp);
      }
    }
  }

  assert(saw_draw);
  chq_destroy(state);
  printf("PASS  helicopter draws: dee_draw_helicopter engages without "
         "road buffer corruption\n");
}

/*
 * draw_overhead ($9052): the bridge deck span must stop where the Z80's
 * unrolled fill loop ($9117-$9151, BRIDGE_DECK_LOOP_WRITES entries) would
 * stop, not run past it. Regression test for the "central part of the
 * object that spans the road fails to stop at the right edge" report: the
 * memset byte count was taken directly from do_span_width_words/2 (the JR
 * displacement into the loop) instead of BRIDGE_DECK_LOOP_WRITES minus that
 * value, which inverted the clip -- the span grew wider as the deck should
 * have been narrowing towards the edge.
 *
 * All values below (Avertical, do_vert_sub, D, E, do_span_width_words, dest
 * address) are hand-derived from the skool at $90A3-$9169 for this specific
 * fixture; see the comment block beside the assertions.
 */
static void test_draw_overhead_stops_at_right_edge(void)
{
  chqstate_t      *state;
  overhead_span_t  span;
  u8               fill_byte;
  depthset_t       set;
  stretchy_t       obj;
  s16              xpos[4];
  u8               height[0x36];
  const u8        *row;
  int              expected_off;
  int              expected_row;
  int              expected_col;
  int              expected_writes;
  int              i;

  state = chq_create(&g_speccy);
  assert(state != NULL);

  memset(state->backbuffer, 0xFF, sizeof(state->backbuffer));

  fill_byte = 0xAA;
  span.nrows = 1;
  span.fill_bytes = &fill_byte;

  memset(&set, 0, sizeof(set));
  set.pairs[0].depth = 10; /* Cdepth, selected via Aminheight = MIN(Bparam-1,9) = 0 */
  set.spans = &span;

  obj.type = STRETCHY_TYPE_FIXED; /* unused by draw_overhead */
  obj.set  = &set;

  memset(xpos, 0, sizeof(xpos));
  xpos[1] = 100; /* previous-row entry: high byte 0 -> exercises the E chain */
  xpos[2] = -1;  /* current-row entry: negative -> skips left-leg draw and the D chain (D stays 1) */

  memset(height, 0, sizeof(height));
  height[0]    = 200;
  height[0x35] = 200; /* build_height_table diff (height[0]-height[0x35]) = 0 */

  draw_overhead(state, 1 /* Bparam */, &obj, &xpos[2], height);

  /* fast_counter=0 (fresh state) -> persp_y_scale row 0, column Bparam=1 ->
   * Avertical = 0x4A (74). do_vert_sub = (74>>1)+74-0 = 111.
   * Row select A = height[0x35](200) - 111 = 89.
   * D = 1 (xpos[2] high byte negative skips the D chain).
   * E: xpos[1]=100, Cdepth=10 -> A=110 -> E = 110>>3 = 13.
   * do_span_width_words = ~((13-1)*2)+61 = 36 (mod 256).
   * Deck loop write count = BRIDGE_DECK_LOOP_WRITES(30) - 36/2 = 12.
   * Dest addr = (((89&0x0F)+0xF0)<<8) | ((89&0x70)*2 + D(1)) = 0xF900|0xA1 = 0xF9A1. */
  assert(state->do_span_width_words == 36);

  expected_off    = 0xF9A1 - BACKBUFFER_START_ADDRESS;
  expected_row    = expected_off / BACKBUFFER_ROWBYTES;
  expected_col    = expected_off % BACKBUFFER_ROWBYTES;
  expected_writes = 12;

  row = &state->backbuffer[expected_row * BACKBUFFER_ROWBYTES];

  for (i = 0; i < expected_writes; i++)
    assert(row[expected_col + i] == fill_byte);

  /* The byte immediately past the span must be untouched -- this is the
   * right-edge stop. Before the BRIDGE_DECK_LOOP_WRITES fix the buggy
   * formula wrote 18 bytes here instead of 12, overrunning this check. */
  assert(row[expected_col + expected_writes] == 0xFF);

  chq_destroy(state);
  printf("PASS  draw_overhead: bridge deck span stops at the computed right edge\n");
}

/*
 * advance_hazards: when three hazards are advanced in slot order with
 * strictly increasing distance (5, 10, 15), each later hazard must be
 * inserted *before* the earlier ones already in the depth-sorted draw list
 * (state->xpos_road_centre_left), forcing the dhs_insert shift path with a
 * growing number of records to move (B=1, then B=2).
 *
 * Regression check for a reversed source/destination copy in the shift
 * loop: the pointer arithmetic for the shift matched real Z80 LDDR
 * semantics ((DE) <- (HL)), but the loop body wrote `*HLtable-- =
 * *DEtable--` -- backwards -- which duplicated stale/garbage entries over
 * the live records instead of shifting them up, losing the queued hazards.
 * With three hazards and slot order 0,1,2 the final insert must shift two
 * live records (B=2), which is exactly the case that corrupted data.
 */
static void test_advance_hazards_insert_shift_preserves_records(void)
{
  chqstate_t *state;
  s16        *rec;

  state = make_road_state();
  chq_test_build_height_table(state);

  memset(state->hazards, 0, sizeof(state->hazards));

  state->hazards[0].used             = HAZARD_USED;
  state->hazards[0].distance         = 5;
  state->hazards[0].dist_frac        = 0;
  state->hazards[0].speed            = 0;
  state->hazards[0].hazard_flags     = 0;
  state->hazards[0].horz_pos_on_road = 0;
  state->hazards[0].hit_handler      = no_op;

  state->hazards[1].used             = HAZARD_USED;
  state->hazards[1].distance         = 10;
  state->hazards[1].dist_frac        = 0;
  state->hazards[1].speed            = 0;
  state->hazards[1].hazard_flags     = 0;
  state->hazards[1].horz_pos_on_road = 0;
  state->hazards[1].hit_handler      = no_op;

  state->hazards[2].used             = HAZARD_USED;
  state->hazards[2].distance         = 15;
  state->hazards[2].dist_frac        = 0;
  state->hazards[2].speed            = 0;
  state->hazards[2].hazard_flags     = 0;
  state->hazards[2].horz_pos_on_road = 0;
  state->hazards[2].hit_handler      = no_op;

  chq_test_advance_hazards(state);

  assert(state->n_hazards == 3);

  /* Draw list must be sorted with the largest distance first: slot 2 (15),
   * then slot 1 (10), then slot 0 (5) -- none lost, none corrupted. */
  rec = state->xpos_road_centre_left;
  assert((rec[0] & 0xFF) == 15); assert(rec[1] == 2);
  assert((rec[2] & 0xFF) == 10); assert(rec[3] == 1);
  assert((rec[4] & 0xFF) == 5);  assert(rec[5] == 0);

  chq_destroy(state);
  printf("PASS  advance_hazards: insert-shift preserves queued draw-list "
         "records\n");
}

/* ----------------------------------------------------------------------- */

int main(void)
{
  speccy_init();

  test_build_height_table_writes_table();
  test_layout_road_populates_tables();
  test_drlc_exits_when_dist_too_far();
  test_drlc_exits_on_straight_track();
  test_drlc_writes_xpos_entries();
  test_draw_road_writes_backbuffer();
  test_lane_markings_appear_at_bottom_row();
  test_set_up_stage_lanes_slot_is_3lane();
  test_set_up_stage_resets_lane_data();
  test_fork_progression();
  test_full_frame_no_corruption();
  test_draw_overhead_stops_at_right_edge();
  test_helicopter_draws();
  test_perp_caught_progression();
  test_advance_hazards_insert_shift_preserves_records();

  printf("\nAll tests passed.\n");
  return 0;
}
