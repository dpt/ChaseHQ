/**
 * ChaseHQ-Tests.h
 *
 * Test hook declarations — only compiled when CHQ_TESTS is defined.
 *
 * These thin wrappers expose static functions in ChaseHQ.c so that
 * the test binary can call them without modifying the production API.
 */

#ifndef CHASEHQ_TESTS_H
#define CHASEHQ_TESTS_H

#ifdef CHQ_TESTS

#include "ChaseHQ/ChaseHQ-State.h"

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

/** Run draw_road in isolation (renders road to back buffer). */
void chq_test_draw_road(chqstate_t *state);

#endif /* CHQ_TESTS */

#endif /* CHASEHQ_TESTS_H */
