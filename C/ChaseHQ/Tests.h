/**
 * Tests.h
 *
 * Test hook declarations — only compiled when CHQ_TESTS is defined.
 *
 * These thin wrappers expose static functions in ChaseHQ.c so that
 * the test binary can call them without modifying the production API.
 */

#ifndef CHASEHQ_TESTS_H
#define CHASEHQ_TESTS_H

#ifdef CHQ_TESTS

#include "ChaseHQ/State.h"

/** Force a stage load for the current wanted_stage_number. */
void chq_test_load_stage(chqstate_t *state);

/**
 * Initialise state->scenedata and road buffer from stage_data, matching
 * the work set_up_stage does before the game loop starts.
 */
void chq_test_set_up_stage(chqstate_t *state);

/**
 * Prime the road buffer by running rm_cycle_buffer_offset the given number
 * of times (set_up_stage uses 32 iterations).
 */
void chq_test_prime_road(chqstate_t *state, int iterations);

/** Run build_height_table in isolation. */
void chq_test_build_height_table(chqstate_t *state);

/** Run layout_road in isolation (builds road geometry tables). */
void chq_test_layout_road(chqstate_t *state);

/** Run exit_fork in isolation (fork-to-single-road transition). */
void chq_test_exit_fork(chqstate_t *state);

/** Return the largest right/left-side object byte visible to
 *  draw_scene_objects (values > 9 indicate road buffer corruption). */
int chq_test_max_side_object(chqstate_t *state);

/** Run one full game frame (run_game main-loop body minus host-dependent
 *  calls: input, timing, audio, scoring, screen copy). */
void chq_test_game_frame(chqstate_t *state);

/** Set up stage from attract_data rather than stage_data. */
void chq_test_set_up_stage_attract(chqstate_t *state);

/** Return a pointer to the current IXlanesptr slot — the lane byte that
 *  draw_road will read on its first dr_read_lanes call. */
u8 *chq_test_lanes_slot(chqstate_t *state);

/** Run draw_road in isolation (renders road to back buffer). */
void chq_test_draw_road(chqstate_t *state);

/**
 * Call draw_road_lanes_change with a synthesised single-byte lanes buffer and
 * the given height_table index for IYheight.  Uses standard draw_road
 * parameters for fill_pattern (0), horizon (0), DEbackbuf (0x0100), and Lrow
 * (0xFF).
 */
void chq_test_draw_road_lanes_change(chqstate_t *state, u8 lane_flags,
                                     int height_offset);

/*
 * draw_stretchy_object_left/right already have external linkage in
 * ChaseHQ.c (they are the real game entry points, not static helpers); they
 * just have no declaration outside it. Declared here, not in ChaseHQ.h,
 * because only test/tool code needs to call them directly.
 */
void draw_stretchy_object_left(chqstate_t *state, int Bdepth,
                               const void *DEarg, const s16 *IXxpos,
                               const u8 *IYheight);
void draw_stretchy_object_right(chqstate_t *state, int Bdepth,
                                const void *DEarg, const s16 *IXxpos,
                                const u8 *IYheight);

#endif /* CHQ_TESTS */

#endif /* CHASEHQ_TESTS_H */
