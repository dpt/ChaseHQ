/**
 * Tests.h
 *
 * Test hook declarations — only compiled when CHQ_TESTS is defined.
 *
 * These thin wrappers expose static functions in Main.c so that
 * the test binary can call them without modifying the production API.
 */

#ifndef CHASEHQ_TESTS_H
#define CHASEHQ_TESTS_H

#ifdef CHQ_TESTS

#include "ChaseHQ/Engine/State.h"

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

/** Run advance_hazards in isolation (advances and depth-sorts all active
 *  hazard slots into the draw list at xpos.centre_left). */
void chq_test_advance_hazards(chqstate_t *state);

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
 * the given height_table index for IY_height.  Uses standard draw_road
 * parameters for fill_pattern (0), horizon (0), DEbackbuf (0x0100), and Lrow
 * (0xFF).
 */
void chq_test_draw_road_lanes_change(chqstate_t *state, u8 lane_flags,
                                     int height_offset);

/*
 * draw_stretchy_object_left/right already have external linkage in
 * Main.c (they are the real game entry points, not static helpers); they
 * just have no declaration outside it. Declared here, not in ChaseHQ.h,
 * because only test/tool code needs to call them directly.
 */
void draw_stretchy_object_left(chqstate_t *state, int B_depth,
                               const void *DE_arg, const s16 *IX_xpos,
                               const u8 *IY_height);
void draw_stretchy_object_right(chqstate_t *state, int B_depth,
                                const void *DE_arg, const s16 *IX_xpos,
                                const u8 *IY_height);

/** Start bank 3 title tune A_tune (titlescr_start_tune) -- must be called
 *  once before chq_test_run_title_tune, matching run_title_screen's own
 *  entry-time call ($C61B). */
void chq_test_start_title_tune(chqstate_t *state, u8 A_tune);

/** Reset the 48K music driver to the start of the pattern list
 *  (reset_music). */
void chq_test_reset_music(chqstate_t *state);

/** Run one tick of the 48K music driver (play_music_48k). Every tick must
 *  end in exactly one sleep -- that is what paces the driver's callers. */
void chq_test_play_music_48k(chqstate_t *state);

/** Run the 48K "STOP THE TAPE" prompt and controller menu
 *  (stop_the_tape_48k). Returns once the player has confirmed a scheme, so
 *  the caller's in() handler must script a key sequence that reaches the
 *  confirmation. */
void chq_test_stop_the_tape_48k(chqstate_t *state);

/** Run one frame of bank 3's title-music service (run_title_tune):
 *  services the drum/music tick and restarts tune 0 once it finishes. */
void chq_test_run_title_tune(chqstate_t *state);

/** Run insert_high_score_entry for [row], including the name-entry screen
 *  setup and its (host-quit-bounded) input loop. */
void chq_test_insert_high_score_entry(chqstate_t *state, int row);

/** Drive one call of name_entry_dispatch with [user_input_flags] (masked to
 *  USERINPUTFLAG_RIGHT/LEFT/FIRE), bypassing the host keyboard/joystick
 *  read so a test can script a RIGHT/LEFT/FIRE sequence frame by frame. */
void chq_test_hiscore_inject_input(chqstate_t *state, u8 user_input_flags);

/** Run name_entry_setup_screen for [row] in isolation, without the
 *  interactive (host-quit-bounded) input loop insert_high_score_entry
 *  also drives. Draws the header text and seeds the per-row scroll-in
 *  table; row contents appear later as chq_test_scroll_score_rows is
 *  driven forward. */
void chq_test_name_entry_setup_screen(chqstate_t *state, int row);

/** Run one frame of scroll_score_rows in isolation -- advances each rank's
 *  row-scroll animation by one pixel and redraws its row at the new
 *  position, clipped to the visible table. */
void chq_test_scroll_score_rows(chqstate_t *state);

#endif /* CHQ_TESTS */

#endif /* CHASEHQ_TESTS_H */
