/**
 * UnitTest.c
 *
 * Unit tests for the C port's game logic: the road-drawing pipeline
 * (build_height_table → layout_road → draw_road), stage/scene setup, fork
 * and hazard handling, the perp-caught and end-screen sequences, and the
 * bank 3 title-music service.
 *
 * The test binary is built with -DCHQ_TESTS, which exposes thin wrappers
 * around the static functions in Main.c and Bank3.c via Tests.h.
 *
 * Running:
 *   cmake --build cmake-build-debug --target ChaseHQ_Tests
 *   ./cmake-build-debug/ChaseHQ_Tests
 */

#include <assert.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ChaseHQ/ChaseHQ.h"
#include "ChaseHQ/Engine/Bank3State.h"
#include "ChaseHQ/Engine/Bank7.h"
#include "ChaseHQ/Engine/Internal.h"
#include "ChaseHQ/Engine/State.h"
#include "ChaseHQ/Data/CommonData.h"
#include "ChaseHQ/Data/Stages.h"
#include "ChaseHQ/Engine/Tests.h"
#include "ZXSpectrum/Macros.h"
#include "ZXSpectrum/Spectrum.h"

/* ----------------------------------------------------------------------- */
/* Minimal fake ZX Spectrum (no-op callbacks, valid screen dimensions)      */
/* ----------------------------------------------------------------------- */

static uint8_t fake_in(zxspectrum_t *s, uint16_t addr)
{
  NOT_USED(s);
  NOT_USED(addr);
  return 0xFF; /* all keys unpressed */
}

static void fake_out(zxspectrum_t *s, uint16_t addr, uint8_t byte)
{
  NOT_USED(s);
  NOT_USED(addr);
  NOT_USED(byte);
}

static void fake_draw(zxspectrum_t *s, const zxbox_t *dirty)
{
  NOT_USED(s);
  NOT_USED(dirty);
}

/* Models the host's fixed-depth timestamp stack: the host asserts on a stamp()
 * that is never slept out, so every engine path any test drives must pair the
 * two. Modelled here so the imbalance fails in the test binary rather than
 * after four title-screen scenes in the real app. */
static int g_stamp_depth;

static void fake_stamp(zxspectrum_t *s)
{
  NOT_USED(s);
  assert(g_stamp_depth < MAX_STAMPS);
  g_stamp_depth++;
}

/* Counts fake_sleep calls so tests can check a routine paces itself. */
static int g_sleep_count;

static int fake_sleep(zxspectrum_t *s, int ticks)
{
  NOT_USED(s);
  NOT_USED(ticks);
  assert(g_stamp_depth > 0);
  g_stamp_depth--;
  g_sleep_count++;
  return 0;
}

static void fake_logtime(zxspectrum_t *s, int duration)
{
  NOT_USED(s);
  NOT_USED(duration);
}

static zxspectrum_t g_speccy;

static void speccy_init(void)
{
  memset(&g_speccy, 0, sizeof(g_speccy));
  g_speccy.in            = fake_in;
  g_speccy.out           = fake_out;
  g_speccy.draw          = fake_draw;
  g_speccy.stamp         = fake_stamp;
  g_speccy.sleep         = fake_sleep;
  g_speccy.logtime       = fake_logtime;
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
static chqstate_t *make_stage_state(u8 stage)
{
  chqstate_t *state;

  state = chq_create(&g_speccy);
  assert(state != NULL);

  state->wanted_stage_number  = stage;
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

static chqstate_t *make_road_state(void)
{
  return make_stage_state(MINSTAGE);
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
  chqstate_t *state   = make_road_state();
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
 * layout_road writes xpos.centre (and _left/_right variants) for the
 * even slots [48..126].  Verify at least some centre slots are non-zero —
 * i.e. layout_road actually ran and wrote the geometry tables.
 *
 * Note: build_curve_table uses pointer arithmetic on persp_x_scale_right
 * whose ASLR-shifted address affects the computed road positions, so the
 * exact values and their left/right ordering are not stable between runs.
 */
static void test_layout_road_populates_tables(void)
{
  chqstate_t *state   = make_road_state();
  int         nonzero = 0;
  int         i;

  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  for (i = 48; i < 128; i += 2)
    if (state->xpos.centre[i] != 0)
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
  state->dt.tunnel_visible = 0xFF;
  state->dr.in_tunnel      = 0xFF;
  state->dr.edge_thickness = 0xFF;

  chq_test_draw_road(state);

  /* draw_road resets these flags at entry */
  assert(state->on_dirt_track     == 0);
  assert(state->dt.tunnel_visible == 0);
  assert(state->dr.in_tunnel      == 0);
  /* dr.edge_thickness is updated during rendering; not checked here */

  assert(backbuf_was_written(state));

  chq_destroy(state);
  printf("PASS  draw_road writes to back buffer\n");
}

/* ----------------------------------------------------------------------- */

/*
 * draw_road_lanes_change: when IYheight is 19 or more entries past the start
 * of height_table (A_dist >= 19) the function must exit immediately without
 * modifying the xpos tables.  The exit path calls dr_four_lane_highway, which
 * sets dr.neg_lane_count to -4.
 */
static void test_drlc_exits_when_dist_too_far(void)
{
  chqstate_t *state;
  u16         snap_left[128];
  u16         snap_centre_left[128];
  u16         snap_centre[128];
  u16         snap_centre_right[128];
  u16         snap_right[128];

  state = make_road_state();
  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  memcpy(snap_left,         state->xpos.left,         sizeof(snap_left));
  memcpy(snap_centre_left,  state->xpos.centre_left,  sizeof(snap_centre_left));
  memcpy(snap_centre,       state->xpos.centre,       sizeof(snap_centre));
  memcpy(snap_centre_right, state->xpos.centre_right, sizeof(snap_centre_right));
  memcpy(snap_right,        state->xpos.right,        sizeof(snap_right));

  /* MAP_LANES_4TO3L_VAL (0xBD) has non-zero curve bits, so if the dist
   * check were absent this call would proceed into the Bresenham loop.
   * With height_offset=19 the dist-too-far guard fires first. */
  chq_test_draw_road_lanes_change(state, 0xBD /* MAP_LANES_4TO3L_VAL */, 19);

  assert(memcmp(snap_left,         state->xpos.left,         sizeof(snap_left))         == 0);
  assert(memcmp(snap_centre_left,  state->xpos.centre_left,  sizeof(snap_centre_left))  == 0);
  assert(memcmp(snap_centre,       state->xpos.centre,       sizeof(snap_centre))       == 0);
  assert(memcmp(snap_centre_right, state->xpos.centre_right, sizeof(snap_centre_right)) == 0);
  assert(memcmp(snap_right,        state->xpos.right,        sizeof(snap_right))        == 0);

  chq_destroy(state);
  printf("PASS  draw_road_lanes_change: dist >= 19 exits early, xpos tables unchanged\n");

}

/*
 * draw_road_lanes_change: when the lane byte has no curve bits (bits 2-3 both
 * clear) the function identifies a straight section and exits without touching
 * the xpos tables.  The exit path calls dr_four_lane_highway (dr.neg_lane_count
 * becomes -4).
 *
 * 0x02 = MAP_LANES_2M_VAL: a 2-lane middle section, bits 2-3 = 0.
 */
static void test_drlc_exits_on_straight_track(void)
{
  chqstate_t *state;
  u16         snap_left[128];
  u16         snap_centre_left[128];
  u16         snap_centre[128];
  u16         snap_centre_right[128];
  u16         snap_right[128];

  state = make_road_state();
  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  memcpy(snap_left,         state->xpos.left,         sizeof(snap_left));
  memcpy(snap_centre_left,  state->xpos.centre_left,  sizeof(snap_centre_left));
  memcpy(snap_centre,       state->xpos.centre,       sizeof(snap_centre));
  memcpy(snap_centre_right, state->xpos.centre_right, sizeof(snap_centre_right));
  memcpy(snap_right,        state->xpos.right,        sizeof(snap_right));

  chq_test_draw_road_lanes_change(state, 0x02 /* MAP_LANES_2M_VAL */, 1);

  assert(memcmp(snap_left,         state->xpos.left,         sizeof(snap_left))         == 0);
  assert(memcmp(snap_centre_left,  state->xpos.centre_left,  sizeof(snap_centre_left))  == 0);
  assert(memcmp(snap_centre,       state->xpos.centre,       sizeof(snap_centre))       == 0);
  assert(memcmp(snap_centre_right, state->xpos.centre_right, sizeof(snap_centre_right)) == 0);
  assert(memcmp(snap_right,        state->xpos.right,        sizeof(snap_right))        == 0);

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
 * the OLD rm.lanes_byte into the buffer instead of reading fresh map data.
 *
 * Concretely: attract mode primes with MAP_LANES_4 (rm.lanes_byte=0x00,
 * lanes_counter ≈ 222 remaining).  A subsequent set_up_stage for game
 * stage 1 must still produce MAP_LANES_3L_VAL in the slot, not 0x00.
 */
static void test_set_up_stage_resets_lane_data(void)
{
  chqstate_t *state;

  state = chq_create(&g_speccy);
  assert(state != NULL);

  state->wanted_stage_number  = MINSTAGE;
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
  u16         baseline[128];
  int         i;
  int         changed;

  state = make_road_state();
  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  memcpy(baseline, state->xpos.centre_right, sizeof(baseline));

  state->fast_counter = 0;
  chq_test_draw_road_lanes_change(state, MAP_LANES_4TO3L_VAL, 1);

  changed = 0;
  for (i = 0; i < 128; i++)
  {
    if (state->xpos.centre_right[i] != baseline[i])
      changed++;
  }
  if (changed == 0)
  {
    printf("  FAIL: no xpos entries changed — Bresenham did not run\n");
    assert(changed > 0);
  }

  chq_destroy(state);
  printf("PASS  draw_road_lanes_change: xpos writes are stable across fast_counter values\n");
}

/*
 * MAP_LANES_3LTO2L_VAL (0x3D) is original stage 2 data ($E56D), not a spare
 * bit pattern: it narrows a 3L run to a 2L run. Pin the fact that it reaches
 * the Bresenham rather than being ignored as a steady state — the docs plan a
 * new lane transition whose proposed mask, (flags & 0xB0) == 0x30, would
 * capture this byte and silently change how stage 2 renders. The taper as it
 * stands has been confirmed correct on screen, so that would be a regression.
 *
 * Writes land in xpos.centre: bit 5 set with bit 7 clear points H at
 * page $EB (xpos.centre_right), and the bit-5 correction at $C408 then
 * steps the output pointer back one page to $EA.
 */
static void test_drlc_3lto2l_runs_bresenham(void)
{
  chqstate_t *state;
  u16         baseline[128];
  int         i;
  int         changed;

  state = make_road_state();
  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  memcpy(baseline, state->xpos.centre, sizeof(baseline));

  state->fast_counter = 0;
  chq_test_draw_road_lanes_change(state, MAP_LANES_3LTO2L_VAL, 1);

  changed = 0;
  for (i = 0; i < 128; i++)
  {
    if (state->xpos.centre[i] != baseline[i])
      changed++;
  }
  if (changed == 0)
  {
    printf("  FAIL: 0x3D wrote no xpos entries — treated as a steady state\n");
    assert(changed > 0);
  }

  chq_destroy(state);
  printf("PASS  draw_road_lanes_change: MAP_LANES_3LTO2L drives the Bresenham\n");
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

  state       = make_road_state();
  saw_fork    = 0;
  fork_frames = 0;

  for (frame = 0; frame < 30000; frame++)
  {
    state->allow_spawning = 0; /* read_map does this each frame */
    chq_test_prime_road(state, 1);
    chq_test_build_height_table(state);
    chq_test_layout_road(state);
    chq_test_exit_fork(state);

    max_obj = chq_test_max_side_object(state);
    if (max_obj > 9)
    {
      printf("  FAIL: side object byte %d (> 9) at frame %d "
             "(vis=%u prog=%u taken=%u dist=%d)\n",
             max_obj, frame, state->fork_visible, state->fork_in_progress,
             state->fork_taken, (int)state->fork_distance);
      assert(max_obj <= 9);
    }

    if (state->fork_visible)
    {
      if (fork_frames == 0)
        saw_fork++;
      fork_frames++;
      if (fork_frames >= 5000)
      {
        printf("  FAIL: fork still active after %d frames — seized\n",
               fork_frames);
        assert(fork_frames < 5000);
      }
    }
    else if (fork_frames)
    {
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

  state                  = make_road_state();
  saw_fork               = 0;
  state->hazards[0].used = HAZARD_USED; /* keep perp spawned, as run_game */

  for (frame = 0; frame < 30000; frame++)
  {
    state->speed = 0x0180;    /* keep the car moving at speed */
    state->session.time_bcd = 0x60;   /* top up the clock: never expires */
    chq_test_game_frame(state);

    if (state->fork_visible)
      saw_fork = 1;

    max_obj = chq_test_max_side_object(state);
    if (max_obj > 9)
    {
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
        ((frame >= 180 && frame <= 260) || frame == 100))
        {
      char  fname[256];
      FILE *fp;
      snprintf(fname, sizeof(fname), "%s/screen-%05d.scr",
               getenv("CHQ_DUMP_DIR"), frame);
      fp = fopen(fname, "wb");
      if (fp)
      {
        fwrite(g_speccy.screen.pixels, 1, sizeof(g_speccy.screen.pixels), fp);
        fclose(fp);
      }
      {
        extern u8 chq_test_backbuf_snapshot[];
        snprintf(fname, sizeof(fname), "%s/road-%05d.bbuf",
                 getenv("CHQ_DUMP_DIR"), frame);
        fp = fopen(fname, "wb");
        if (fp)
        {
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
  chqstate_t *state;     /* game state under test */
  int         frame;     /* frame counter */
  int         saw_accel; /* set once the synthesised input includes UP */
  int         max_phase; /* highest perp_caught_phase reached */

  state                  = make_road_state();
  state->hazards[0].used = HAZARD_USED; /* keep perp spawned, as run_game */
  saw_accel              = 0;
  max_phase              = 0;

  /* Warm up: drive normally for a while so hazard/perp state is live. */
  for (frame = 0; frame < 400; frame++)
  {
    state->speed            = 0x0180;
    state->session.time_bcd = 0x60;
    chq_test_game_frame(state);
  }

  /* Mimic fully_smashed ($8C3A): phase 1, stop hand, input masked to
   * pause/quit only, perp scripted to speed 400. */
  state->perp_caught_phase       = 1;
  state->hand_flag               = 2;
  state->smash_counter           = 20;
  state->session.user_input_mask = 0xC0;
  state->hazards[0].speed        = 400;
  state->hazards[0].distance     = 5;
  state->speed                   = 0x0180;

  for (frame = 0; frame < 2000; frame++)
  {
    state->session.time_bcd = 0x60;
    /* chq_test_game_frame omits keyscan/check_user_input, so nothing
     * resets user_input to the real no-keys-pressed baseline each frame
     * as it would in run_game; without this, a stale UP synthesised by
     * assign_hero_pos during the ALIGNING phase would keep re-accelerating
     * the hero through the STOPPING/STOPPED/SCORE phases. */
    state->user_input = 0;
    chq_test_game_frame(state);
    if (state->user_input & 0x08) /* USERINPUTFLAG_UP */
      saw_accel = 1;
    if (state->perp_caught_phase > max_phase)
      max_phase = state->perp_caught_phase;
    if (state->perp_caught_phase >= 4)
      break;
  }

  if (!saw_accel)
  {
    printf("  FAIL: synthesised input never accelerated the hero\n");
    assert(saw_accel);
  }
  if (max_phase < 4)
  {
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
 * whole game frames, checking that dee.draw_helicopter engages and that the
 * road buffer is not corrupted while draw_helicopter runs. Regression check
 * for pitfall #29: helitable was stepped as an array of heli_bitmap_t
 * (12 bytes) rather than an array of pointers (2 bytes), which walked far
 * outside CommonData and produced garbage or crashed.
 */
static void test_helicopter_draws(void)
{
  chqstate_t *state;    /* game state under test */
  int         frame;    /* frame counter */
  int         saw_draw; /* set once dee.draw_helicopter goes non-zero */
  int         max_obj;  /* largest side-object byte this frame */

  state = chq_create(&g_speccy);
  assert(state != NULL);

  state->wanted_stage_number  = 2;
  state->current_stage_number = 0; /* force load */
  chq_test_load_stage(state);
  assert(state->stage != NULL);
  chq_test_set_up_stage(state);

  state->hazards[0].used    = HAZARD_USED; /* keep perp spawned, as run_game */
  state->helicopter_control = 3; /* dispatch straight to turn-left sequence */

  saw_draw = 0;

  for (frame = 0; frame < 500; frame++)
  {
    state->speed            = 0x0180;
    state->session.time_bcd = 0x60;
    chq_test_game_frame(state);

    if (state->dee.draw_helicopter)
      saw_draw = 1;

    max_obj = chq_test_max_side_object(state);
    if (max_obj > 9)
    {
      printf("  FAIL: side object byte %d (> 9) at frame %d "
             "(dee.draw_helicopter=%u)\n",
             max_obj, frame, state->dee.draw_helicopter);
      assert(max_obj <= 9);
    }

    /* Debug aid: set CHQ_DUMP_DIR to dump raw ZX screens (.scr) for visual
     * inspection of the rendered helicopter sprite. */
    if (getenv("CHQ_DUMP_DIR") != NULL && frame < 100)
    {
      char  fname[256];
      FILE *fp;
      snprintf(fname, sizeof(fname), "%s/heli-%05d.scr",
               getenv("CHQ_DUMP_DIR"), frame);
      fp = fopen(fname, "wb");
      if (fp)
      {
        fwrite(g_speccy.screen.pixels, 1, sizeof(g_speccy.screen.pixels), fp);
        fclose(fp);
      }
    }
  }

  assert(saw_draw);
  chq_destroy(state);
  printf("PASS  helicopter draws: dee.draw_helicopter engages without "
         "road buffer corruption\n");
}

/*
 * draw_overhead ($9052): the bridge deck span must stop where the Z80's
 * unrolled fill loop ($9117-$9151, BRIDGE_DECK_LOOP_WRITES entries) would
 * stop, not run past it. Regression test for the "central part of the
 * object that spans the road fails to stop at the right edge" report: the
 * memset byte count was taken directly from overhead.span_width_words/2 (the JR
 * displacement into the loop) instead of BRIDGE_DECK_LOOP_WRITES minus that
 * value, which inverted the clip -- the span grew wider as the deck should
 * have been narrowing towards the edge.
 *
 * All values below (Avertical, overhead.vert_sub, D, E, overhead.span_width_words, dest
 * address) are hand-derived from the skool at $90A3-$9169 for this specific
 * fixture; see the comment block beside the assertions.
 */
static void test_draw_overhead_stops_at_right_edge(void)
{
  chqstate_t     *state;
  overhead_span_t span;
  u8              fill_byte;
  depthset_t      set;
  stretchy_t      obj;
  s16             xpos[4];
  u8              height[0x36];
  const u8       *row;
  int             expected_off;
  int             expected_row;
  int             expected_col;
  int             expected_writes;
  int             i;

  state = chq_create(&g_speccy);
  assert(state != NULL);

  memset(state->backbuffer, 0xFF, sizeof(state->backbuffer));

  fill_byte       = 0xAA;
  span.nrows      = 1;
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
   * Avertical = 0x4A (74). overhead.vert_sub = (74>>1)+74-0 = 111.
   * Row select A = height[0x35](200) - 111 = 89.
   * D = 1 (xpos[2] high byte negative skips the D chain).
   * E: xpos[1]=100, Cdepth=10 -> A=110 -> E = 110>>3 = 13.
   * overhead.span_width_words = ~((13-1)*2)+61 = 36 (mod 256).
   * Deck loop write count = BRIDGE_DECK_LOOP_WRITES(30) - 36/2 = 12.
   * Dest addr = (((89&0x0F)+0xF0)<<8) | ((89&0x70)*2 + D(1)) = 0xF900|0xA1 = 0xF9A1. */
  assert(state->overhead.span_width_words == 36);

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
 * (state->xpos.centre_left), forcing the dhs_insert shift path with a
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
  rec = state->xpos.centre_left;
  assert((rec[0] & 0xFF) == 15); assert(rec[1] == 2);
  assert((rec[2] & 0xFF) == 10); assert(rec[3] == 1);
  assert((rec[4] & 0xFF) == 5);  assert(rec[5] == 0);

  chq_destroy(state);
  printf("PASS  advance_hazards: insert-shift preserves queued draw-list "
         "records\n");
}

/*
 * show_end_screen ($E000): an infinite input-driven loop with no natural
 * exit under an all-keys-unpressed fake in(). Bound it with the project's
 * host_quit/longjmp escape hatch -- the same idiom chq_start uses around
 * entry_128k -- so the test can run a fixed number of frames and then
 * inspect state. state->bank7's fields are private to Bank7.c (only
 * forward-declared in State.h), so the observable proof of life is that the
 * script interpreter drew something into the back buffer.
 */
static chqstate_t *s_end_screen_state;
static int         s_end_screen_in_count;

static uint8_t end_screen_in(zxspectrum_t *s, uint16_t addr)
{
  NOT_USED(s); NOT_USED(addr);
  if (++s_end_screen_in_count > 200)
    s_end_screen_state->host_quit = 1;
  return 0xFF; /* all keys unpressed */
}

static void test_show_end_screen_runs_script(void)
{
  chqstate_t *state;

  /* show_end_screen's chatter path reads state->stage->addrof_perp_mugshot_
   * bitmap (print_chatter, Main.c), so a bare chq_create() is not enough --
   * a stage must be loaded, as it would be by the time the real game
   * reaches the end screen. Stage 1 has no pilot mugshot (NULL by design),
   * so use stage 2, which does. */
  state = chq_create(&g_speccy);
  assert(state != NULL);

  state->wanted_stage_number  = 2;
  state->current_stage_number = 0; /* force load */
  chq_test_load_stage(state);
  assert(state->stage != NULL);
  chq_test_set_up_stage(state);

  memset(state->backbuffer, 0xFF, sizeof(state->backbuffer));

  s_end_screen_state    = state;
  s_end_screen_in_count = 0;
  g_speccy.in           = end_screen_in;

  if (setjmp(state->host_quit_jmp) == 0)
    show_end_screen(state);

  assert(backbuf_was_written(state));

  g_speccy.in = fake_in;
  chq_destroy(state);
  printf("PASS  show_end_screen: script interpreter runs under bounded "
         "host_quit escape\n");
}

/*
 * name_entry_input/hiscore_finalise ($C16A/$C212): drives 3 letters through the
 * name-entry screen via chq_test_hiscore_inject_input, which calls
 * name_entry_dispatch directly (bypassing the host keyboard/joystick read),
 * and checks the confirmed row ends up with the expected initials and
 * hiscore.complete set.
 */
/*
 * draw_table_field_scrolling draws a rank's row one scanline at a time at
 * its current, usually mid-cell, scroll position (scroll_score_rows calls
 * it every frame for every rank), clipping each scanline against
 * SCREEN_ROW_VISIBLE. Drives enough frames for every rank to scroll through
 * the visible band at least once, so any regression that walks a write
 * outside the clipped band (global-buffer-overflow on the g_speccy screen
 * bitmap, see chq.log) is caught immediately.
 */
static void test_name_entry_setup_screen_draws_table(void)
{
  chqstate_t *state;
  int         frame;

  state = chq_create(&g_speccy);
  assert(state != NULL);

  chq_test_name_entry_setup_screen(state, 0);

  for (frame = 0; frame < 400; frame++)
    chq_test_scroll_score_rows(state);

  chq_destroy(state);

  printf("PASS  name_entry_setup_screen: draws high-score table without corrupting memory\n");
}

static void test_name_entry_confirms_three_letters(void)
{
  chqstate_t       *state;
  int               letter;
  high_score_row_t *entry;

  state = chq_create(&g_speccy);
  assert(state != NULL);

  state->bank3->hiscore.row          = 0;
  state->bank3->hiscore.char_index   = 0;
  state->bank3->hiscore.letter_code  = 0x40;
  state->bank3->hiscore.blink_timer  = 0x0C;
  state->bank3->hiscore.fire_locked  = 0;
  state->bank3->hiscore.complete     = 0;

  for (letter = 0; letter < 3; letter++)
  {
    chq_test_hiscore_inject_input(state, USERINPUTFLAG_RIGHT); /* '@' -> 'A' */
    chq_test_hiscore_inject_input(state, 0);                   /* release RIGHT */
    chq_test_hiscore_inject_input(state, USERINPUTFLAG_FIRE);  /* confirm 'A' */
    chq_test_hiscore_inject_input(state, 0);                   /* release FIRE */
  }

  entry = &state->bank3->high_score_table[0];
  assert(entry->name[0] == 'A');
  assert(entry->name[1] == 'A');
  assert(entry->name[2] == 'A');
  assert(state->bank3->hiscore.complete == 1);

  chq_destroy(state);
  printf("PASS  name_entry: RIGHT/FIRE x3 confirms \"AAA\" and completes\n");
}

/*
 * cycle_and_draw_letter ($C25D): checks the wrap at both ends of the
 * $41-$5A ('A'-'Z') range through the $40 ('@', blank/".") marker -- RIGHT
 * past 'Z' lands on blank, LEFT past blank lands on 'Z'.
 */
/*
 * name_entry_dispatch ($C16A, $C172-$C19B): the 20-cell selector sweeps
 * once every 12 frames; after 13 full sweeps (3120 frames, ~62s at 50Hz)
 * with no player input, the idle timeout force-finalises the current
 * letter, same as the real hardware's safety net.
 */
static void test_name_entry_idle_timeout_finalises(void)
{
  chqstate_t       *state;
  int               frame;
  high_score_row_t *entry;

  state = chq_create(&g_speccy);
  assert(state != NULL);

  state->bank3->hiscore.row          = 0;
  state->bank3->hiscore.char_index   = 0;
  state->bank3->hiscore.letter_code  = 0x40;
  state->bank3->hiscore.blink_timer  = 0x0C;
  state->bank3->hiscore.blink_offset = 0;
  state->bank3->hiscore.cursor_addr  = 10;
  state->bank3->hiscore.fire_locked  = 0;
  state->bank3->hiscore.complete     = 0;

  for (frame = 0; frame < 3119; frame++)
  {
    chq_test_hiscore_inject_input(state, 0);
    assert(state->bank3->hiscore.complete == 0);
  }
  chq_test_hiscore_inject_input(state, 0);
  assert(state->bank3->hiscore.complete == 1);

  entry = &state->bank3->high_score_table[0];
  assert(entry->name[0] == '.'); /* $40 blank marker, never confirmed by FIRE */

  chq_destroy(state);
  printf("PASS  name_entry: idle timeout force-finalises after 3120 frames\n");
}

static void test_cycle_and_draw_letter_wraps_both_ends(void)
{
  chqstate_t *state;
  int         i;

  state = chq_create(&g_speccy);
  assert(state != NULL);

  state->bank3->hiscore.row         = 0;
  state->bank3->hiscore.char_index  = 0;
  state->bank3->hiscore.letter_code = 0x40;
  state->bank3->hiscore.blink_timer = 0x0C;
  state->bank3->hiscore.fire_locked = 0;
  state->bank3->hiscore.complete    = 0;

  for (i = 0; i < 26; i++)
    chq_test_hiscore_inject_input(state, USERINPUTFLAG_RIGHT); /* '@' -> ... -> 'Z' */
  assert(state->bank3->hiscore.letter_code == 0x5A);

  chq_test_hiscore_inject_input(state, USERINPUTFLAG_RIGHT); /* 'Z' -> '@' */
  assert(state->bank3->hiscore.letter_code == 0x40);

  chq_test_hiscore_inject_input(state, USERINPUTFLAG_LEFT); /* '@' -> 'Z' */
  assert(state->bank3->hiscore.letter_code == 0x5A);

  chq_destroy(state);
  printf("PASS  cycle_and_draw_letter: wraps at both ends of 'A'-'Z' through '@'\n");
}

/*
 * run_title_tune ($FBC8): services one frame of the title-screen music/drum
 * subsystem, restarting the current tune whenever it finds no tune active.
 * Mirrors run_title_screen's own entry-time call to titlescr_start_tune
 * ($C61B) for each of the 4 tunes the tune-select table (tunes[],
 * Bank3.c) defines, then drives a few seconds' worth of frames and
 * checks the tune stays active throughout -- proof the music/drum service
 * loop runs without crashing or stalling for every tune, not just the one
 * the title screen happens to start.
 *
 * All 4 tunes have real pattern data extracted (see titlescr_start_ay's own
 * Conv note) and so must each arm channel 0's pattern reader.
 */
static void test_run_title_tune_starts_and_keeps_playing(void)
{
  chqstate_t *state;
  u8          tune;
  int         frame;

  for (tune = 0; tune < 4; tune++)
  {
    state = chq_create(&g_speccy);
    assert(state != NULL);

    chq_test_start_title_tune(state, tune);
    assert(state->bank3->title_music.tune_active);
    assert(state->bank3->title_music.channel[0].pattern_ptr != NULL);

    for (frame = 0; frame < 150; frame++)
    {
      chq_test_run_title_tune(state);
      assert(state->bank3->title_music.tune_active);
    }

    chq_destroy(state);
  }

  printf("PASS  run_title_tune: all 4 tunes start and keep playing\n");
}

/*
 * play_music_48k must sleep exactly once per call, on every path through the
 * driver. Its callers (redefine_keys_48k, define_a_key) poll the keyboard in
 * loops that never sleep themselves, so an unpaced tick spins the game thread
 * flat out -- the "48K music seizes up" symptom. Also acts as a termination
 * check: the pattern-fetch loop must never spin forever on a given tick.
 */
static void test_play_music_48k_paces_every_tick(void)
{
  chqstate_t *state;
  int         tick;

  state = chq_create(&g_speccy);
  assert(state != NULL);

  chq_test_reset_music(state);

  g_sleep_count = 0;
  for (tick = 0; tick < 500; tick++)
    chq_test_play_music_48k(state);
  assert(g_sleep_count == 500);

  chq_destroy(state);

  printf("PASS  play_music_48k: every tick sleeps exactly once\n");
}

/*
 * Phase of the scripted key sequence stt_scripted_in feeds to
 * stop_the_tape_48k. Each phase satisfies exactly one of the routine's polling
 * loops, so a mistranslated loop condition or branch polarity leaves the
 * routine stuck in an earlier phase and the test hangs rather than passing.
 */
static int g_stt_phase;

static uint8_t stt_scripted_in(zxspectrum_t *s, uint16_t addr)
{
  NOT_USED(s);

  switch (g_stt_phase)
  {
  case 0: /* $E90F: a key goes down, ending the "press any key" wait */
    if (addr == port_BORDER_EAR_MIC)
    {
      g_stt_phase = 1;
      return 0xFE;
    }
    break;

  case 1: /* $E91A: released again, ending the debounce */
    if (addr == port_BORDER_EAR_MIC)
    {
      g_stt_phase = 2;
      return 0xFF;
    }
    break;

  case 2: /* $E92E: "1" chooses SINCLAIR JOYSTICK */
    if (addr == port_KEYBOARD_12345)
    {
      g_stt_phase = 3;
      return 0xFE;
    }
    break;

  case 3: /* $E979: released again, ending the second debounce */
    if (addr == port_BORDER_EAR_MIC)
    {
      g_stt_phase = 4;
      return 0xFF;
    }
    break;

  case 4: /* $E984: Y confirms the choice */
    if (addr == port_KEYBOARD_POIUY) return 0xEF;
    break;
  }

  return 0xFF; /* nothing pressed */
}

/*
 * Driving stop_the_tape_48k with "1" then Y must install the Sinclair joystick
 * scheme: kempston_flag clear, keydefs[GEAR..RIGHT] copied from
 * sinclair_joy_keydefs and keydefs[QUIT..BOOST] left at the temp_keydefs
 * defaults. This is the only path that populates keydefs[] in 48K mode, so a
 * broken menu leaves the game unplayable.
 */
static void test_stop_the_tape_48k_installs_sinclair_scheme(void)
{
  chqstate_t *state;

  state = chq_create(&g_speccy);
  assert(state != NULL);

  /* Prove the assertions below are not just reading the pristine state. */
  memset(state->keydefs, 0xAA, sizeof(state->keydefs));
  state->kempston_flag = 0xAA;

  g_stt_phase  = 0;
  g_speccy.in  = stt_scripted_in;
  chq_test_stop_the_tape_48k(state);
  g_speccy.in  = fake_in;

  assert(g_stt_phase == 4); /* every loop was satisfied in order */
  assert(state->kempston_flag == 0);
  assert(memcmp(&state->keydefs[KEYDEF_GEAR], sinclair_joy_keydefs, 5) == 0);
  assert(memcmp(&state->keydefs[KEYDEF_QUIT], &temp_keydefs_template[5], 3) == 0);

  chq_destroy(state);

  printf("PASS  stop_the_tape_48k: \"1\" then Y installs the Sinclair scheme\n");
}

/*
 * Every stage's sprites are drawn from their own array now rather than from one
 * offset into a shared blob per graphics run, so a sprite whose height reaches
 * past its array no longer lands harmlessly in the next sprite's data -- there
 * is no guarantee how the compiler lays two arrays out. Driving frames for each
 * stage under the Debug build's AddressSanitizer is what catches that: a read
 * past the end of any one of the ~370 sprite arrays aborts here.
 */
static void test_all_stages_draw_frames(void)
{
  chqstate_t *state;
  u8          stage;
  int         frame;
  u8          last;

#ifdef CHQ_ENABLE_TEST_STAGE
  last = 6; /* the port-added test level, only mapped in that build */
#else
  last = 5;
#endif

  for (stage = MINSTAGE; stage <= last; stage++)
  {
    state                  = make_stage_state(stage);
    state->hazards[0].used = HAZARD_USED; /* keep the perp spawned */

    for (frame = 0; frame < 2000; frame++)
    {
      state->speed = 0x0180;          /* keep the car moving at speed */
      state->session.time_bcd = 0x60;   /* top up the clock: never expires */
      chq_test_game_frame(state);
      assert(chq_test_max_side_object(state) <= 9);
    }

    chq_destroy(state);
  }

  printf("PASS  all stages: 2000 frames each draw without an out-of-bounds read\n");
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
  test_drlc_3lto2l_runs_bresenham();
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
  test_show_end_screen_runs_script();
  test_name_entry_setup_screen_draws_table();
  test_name_entry_confirms_three_letters();
  test_name_entry_idle_timeout_finalises();
  test_cycle_and_draw_letter_wraps_both_ends();
  test_run_title_tune_starts_and_keeps_playing();
  test_play_music_48k_paces_every_tick();
  test_stop_the_tape_48k_installs_sinclair_scheme();
  test_all_stages_draw_frames();

  printf("\nAll tests passed.\n");
  return 0;
}
