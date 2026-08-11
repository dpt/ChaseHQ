/**
 * State.h
 *
 * This file is part of "Chase H.Q. in C".
 *
 * This project recreates the ZX Spectrum version of the chase-and-smash game
 * "Chase H.Q." in portable C code. It is free software provided without
 * warranty in the interests of education and software preservation.
 *
 * The arcade original was created by Taito Corporation in 1988. It was then
 * ported to the ZX Spectrum by Ocean Software Limited and released in 1989.
 *
 * The original game and design is copyright (c) 1988 Taito Corporation.
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited.
 * The recreated version is copyright (c) 2023-2026 David Thomas.
 */

#ifndef CHASEHQ_STATE_H
#define CHASEHQ_STATE_H

#include <setjmp.h>

#include "C99/Types.h"
#include "ZXSpectrum/Spectrum.h"

#include "ChaseHQ/ChaseHQ.h"
#include "ChaseHQ/Data/Stages.h"

/* ----------------------------------------------------------------------- */

/* Returns the back-buffer address advanced past the rows just drawn: on the
 * Z80 this value survives in the shadow HL' register across the repeat
 * loop's CALLs; the C translation must pass it back explicitly instead. */
typedef u8 *(plot_sprite_cb_t)(chqstate_t *state,
                               int         IX_jump_offset,
                               u8         *HL_backbuf_addr,
                               int         Bdash_height,
                               int         DEdash_bitmap_stride,
                               const u8   *HLdash_bitmap_data);

/**
 * A hazard in Chase H.Q. is something that's on the road. It might be a moving
 * object like the perp or NPC cars, or a fixed item like a barrier or a
 * tumbleweed.
 *
 * $A188-$A1FF: six slots of 20 bytes each, at $A188, $A19C, $A1B0, $A1C4,
 * $A1D8 and $A1EC. Slot 0 is always the perp car; spawn_cars and
 * spawn_hazards allocate from slots 1-5 by scanning for the first with
 * [used] clear. Field comments give the byte offset within the record,
 * which is how the Z80 reaches them -- always as (IX+n), never by absolute
 * address.
 */
struct hazard
{
  /* +0: Slot allocation flag
   *
   * HAZARD_USED ($FF) or HAZARD_UNUSED ($00). The spawn routines scan the
   * five non-perp slots for the first unused one; update_hazard clears the
   * flag when the object has passed the camera.
   */
  u8                used;

  /* +1: Approach counter, counting 21 down to 0 as the object nears
   *
   * Advanced by the whole-units part of [speed] each frame. update_hazard
   * retires the slot once this reaches 23 and only draws the object below
   * 20. It doubles as the low byte of the road-buffer column while the
   * object is being drawn, and perp_behaviour reads the perp's copy as a
   * road-buffer offset when comparing hazard positions.
   */
  u8                distance;

  /* +2: Horizontal screen position, low byte of the computed road X */
  u8                horz_pos;

  /* +3: Horizontal clip flag, high byte of the computed road X
   *
   * 0 means on screen, negative means clipped off the left edge, positive
   * means clipped off the right. check_collision rejects any hazard whose
   * clip byte is non-zero, and draw_hazard picks its plot entry point from
   * the sign.
   */
  s8                horz_clip;

  /* +4: Fractional part of the approach distance
   *
   * Decremented each frame by the low byte of [speed]; each borrow carries
   * into [distance]. Also the multiplicand for the perspective column
   * calculation, so it doubles as the sub-row depth within the current
   * distance step.
   */
  u8                dist_frac;

  /* +5: Position across the road, 0-255 left to right
   *
   * Observed values run 5..216. hazard_handler slides this +/-5 a frame
   * toward the lane position in hazard_pos_speed, which is what makes a
   * traffic car drift between lanes rather than jump.
   */
  u8                horz_pos_on_road;

  /* +6: Perspective-scaled column, ([dist_frac] * height delta) >> 8
   *
   * Recomputed each frame by update_hazard. Selects the sprite's column and
   * the road-edge row the object is drawn against; draw_hazard passes it on
   * as doc.col_pos after subtracting [hit_wobble].
   */
  u8                persp_col;

  /* +7: Hit sequence timer
   *
   * Zero when nothing is happening. Positive means a vehicle hit is in
   * progress. Negative is the perp's post-hit cooldown: perp_behaviour sets
   * it to $FC (-4) and counts back up to zero, ignoring input meanwhile.
   */
  s8                hit_timer;

  /* +8: Collision box width and sprite set for this object
   *
   * +8 is the width used for the bounding-box overlap test in
   * check_collision; +9 is the address of the object's bitmap table (the
   * skool calls it the LOD). spawn_cars overwrites the bitmaps pointer with
   * a random vehicle from the stage's table.
   */
  hittable_t        hittable;

  /* +11: Called once per frame by update_hazard after the object is drawn
   *
   * perp_behaviour for the perp car, hazard_handler for traffic and
   * hazard_hit for barriers and tumbleweeds.
   *
   * Conv: the Z80 reaches the handler with JP (HL); the C port calls
   *       through the function pointer.
   */
  hazard_handler_t *hit_handler;

  /* +13: Fixed-point approach rate
   *
   * High byte is whole distance units per frame, added to [distance]. Low
   * byte is fractional units per frame, subtracted from [dist_frac], where
   * each borrow carries a further unit into [distance]. hazard_hit decays
   * it by 1/32 a frame while an object wobbles away from a collision.
   */
  u16               speed;

  /* +15: What kind of object this is, and how far through a hit it is
   *
   * $80 marks a spawned vehicle and $FF the perp car -- bit 7 set means
   * "is a vehicle", which is how spawn_cars counts the traffic already on
   * screen. For a static hazard it is the hit state machine instead: 0 is
   * untouched, 2 is wobbling and 1 is finished.
   */
  u8                hazard_flags;

  /* +16: Horizontal wobble offset during a hit
   *
   * Indexed out of the wobble_amplitudes table in hazard_hit and subtracted
   * from [persp_col] as the object is drawn, so a struck barrier shudders
   * and settles.
   */
  u8                hit_wobble;

  /* +17: Lane index, or the perp's distance high byte
   *
   * For the perp this is the high byte of [distance]. For a traffic car it
   * is the lane the object currently occupies, which perp_behaviour compares
   * against its own [current_lane] to decide whether to swerve. While a
   * static hazard is being hit it is reused again, as the running index into
   * wobble_amplitudes.
   */
  u8                lane_or_perp_dist_hi;

  /* +18: Target lane, or the wobble countdown
   *
   * hazard_handler slides [horz_pos_on_road] toward this lane's position
   * until the two agree. For a hazard mid-wobble it is instead the frame
   * countdown; when it reaches zero the effect ends.
   */
  u8                current_lane;

  /* +19: Sprite plot mode: 0 normal, 1 inverted
   *
   * hazard_hit toggles it every frame of the wobble, flipping the sprite
   * vertically to fake a barrier tumbling.
   */
  u8                inverted;
};

/**
 * $A16D-$A187: Counters and on-screen digit caches for the run in progress.
 * All of it is reset between stages by load_scene.
 */
struct session
{
  /* $A16D: Object-spawning accumulator, usually 1
   *
   * layout_road adds allow_spawning to it each slice and subtracts 2 when it
   * reaches that; every carry advances fork_distance by 16. Bit 0 also feeds
   * the horizontal position of the untaken road, so it visibly oscillates
   * 0/1 while the road forks.
   */
  u8        spawn_accumulator;

  /* $A16E: Frames left before Raymond complains about not moving
   *
   * Counts down from 100 and restarts whenever the hero car is stopped. At
   * zero Raymond says "LET'S GET MOVIN' MAN!" and it reloads with 100.
   */
  u8        idle_timer;

  /* $A16F: Mask ANDed with the player's controls each frame
   *
   * USERINPUTFLAGMASK_ALLOW_ALL ($FF) in normal play. Set to Pause+Quit only
   * once the perp is fully smashed, and to nothing at all while a cut scene
   * plays, which is how the game locks out steering without a separate flag.
   */
  u8        user_input_mask;

  /* $A170: Turbo boosts remaining; RESTART_BOOSTS (3) for a new game */
  u8        turbos;

  /* $A171: Height of the horizon, accumulated from the road's incline
   *
   * scroll_horizon adds the per-slice incline to it each frame, signed by
   * the direction of travel. Read a byte at a time: the high byte gives the
   * horizon's row, the low byte its sub-row phase.
   */
  u16       horizon_level;

  /* $A173: Frames left while the caught perp is brought to a halt
   *
   * Set to 20 when the arrest begins and decremented each frame of the
   * slow-down.
   */
  u8        perp_halt_counter;

  /* $A174: Gear currently shown on the status panel, low or high
   *
   * A cache of what was last drawn: the panel is only redrawn when the car's
   * actual gear differs from this.
   */
  u8        displayed_gear;

  /* $A175: Score as currently shown, one digit per byte, least significant
   * first
   *
   * A cache of what is on screen so that unchanged digits are not replotted.
   */
  u8        score_digits[8];

  /* $A17D: Frames left in the current second of the countdown
   *
   * Counts down from SUBSECOND_TICKS_PER_SECOND (15); reaching zero
   * decrements [time_bcd] and reloads.
   */
  u8        subsecond_ticks;

  /* $A17E: Time remaining, packed BCD */
  u8        time_bcd;

  /* $A17F: Time remaining as shown, one digit per byte
   *
   * The same replot-avoiding cache as [score_digits].
   */
  u8        time_digits[2];

  /* $A181: Distance remaining as shown, one digit per byte
   *
   * The same replot-avoiding cache as [score_digits].
   */
  u8        distance_digits[4];

  /* $A185: Alternates object emission between road slices
   *
   * layout_road emits the right-side, left-side and hazard object columns
   * when this is 1 and then sets it to 2; on the next slice it finds 2,
   * blanks those three columns instead and sets it back to 1. Objects
   * therefore appear on every other slice of road.
   */
  u8        no_objects_flag;

  /* $A186: Z80 address of the last attribute cell on the horizon row
   *
   * Byte 31 of the row where sky meets ground, e.g. $59DF. ds_attributes
   * fills the 30 cells before it and walks the pointer up or down as the
   * horizon moves -- see the $E34B-$E34D notes in CLAUDE.md.
   */
  u16       horizon_attribute;
};

/**
 * Private state for the 128K bank 3 title-screen / title-tune engine.
 * Fully defined in Bank3State.h, included only by Bank3.c -- chqstate only
 * ever sees this as an opaque pointer, so its fields aren't reachable from
 * any other translation unit.
 */
struct chq_bank3_state;
struct chq_bank7_state;

/**
 * Per-frame AY-3-8912 register soft-copy cache, shared by the in-game audio
 * engine (chqstate::ay_regs) and the 128K bank 3 title-tune engine
 * (chq_bank3_state::title_ay_regs -- see Bank3State.h). write_audio_registers_128k
 * (and its title-tune equivalent) walk this struct backwards as raw bytes
 * from env_fine to chan_a_pitch's low byte, one AY register per byte, so
 * field order and the absence of padding are correctness-critical -- do not
 * reorder, insert, or remove fields.
 */
typedef struct
{
  u16       chan_a_pitch;
  u16       chan_b_pitch;
  u16       chan_c_pitch;
  u8        noise_pitch;
  u8        mixer;
  u8        chan_a_vol;
  u8        chan_b_vol;
  u8        chan_c_vol;
  u8        env_fine;
} ay_register_cache_t;

/* ----------------------------------------------------------------------- */

/**
 * Holds the current state of the game.
 */
struct chqstate
{
  /* ------------------------------------------------------------------------
   * State variables additional to the original game.
   * --------------------------------------------------------------------- */

  /* Virtual ZX Spectrum hardware we're driving
   *
   * Conv: port-added. The Z80 ran on the real machine; the C port reaches the
   *       screen, keyboard and speaker through this facade.
   */
  zxspectrum_t *speccy;

  /* Unwind target for a host-requested quit
   *
   * chq_start sets this up; every long-running loop unwinds to it via
   * CHECK_HOST_QUIT.
   *
   * Conv: port-added. The Z80 has no clean-exit path -- the game only ever
   *       stops when the machine is switched off.
   */
  jmp_buf      host_quit_jmp;

  /* Set by the host to ask the game thread to quit
   *
   * Conv: port-added. Written by chq_stop from the host thread, hence
   *       volatile; read by CHECK_HOST_QUIT.
   */
  volatile int host_quit;

  /* ------------------------------------------------------------------------
   * State variables as per the original, ordered by memory location.
   * --------------------------------------------------------------------- */

  /* $5B00: Backdrop artwork pre-shifted to each of the eight pixel phases
   *
   * $5B00..$5BFF is reserved for this in the original; only the leading 240
   * bytes (10x24) are used.
   */
  u8        pre_shifted_backdrop[BACKDROP_LENGTH];

  /* $8000: Test mode flag; set by the "SHOCKED" cheat on the redefine-keys
   * screen */
  u8        test_mode;

  /* $8001: Set once the player has chosen a control scheme */
  u8        controls_selected;

  /* $8002: Player's score, packed BCD, most significant byte first */
  u8        score_bcd[4];

  /* $8006: Number of times the player has continued */
  u8        retry_count;

  /* $8007: Stage the player is about to play; MAXSTAGE + 1 triggers the end
   * screen */
  u8        wanted_stage_number;

  /* $5C00-$76EF: Read-only data tables for the stage being played
   *
   * Conv: the Z80 paged the stage's tables into this address range; the C
   *       port points at the compiled-in stage_t instead.
   */
  const stage_t *stage;

  /* $8066: (SM in play_siren_sfx_128k, relocated) Current step through the
   * siren's pitch pattern */
  u8        siren_pattern;

  /* $823B-$8251: (SM in play_engine_sfx_48k) Engine tone generator state
   *
   * The engine note is bit-banged as a square wave whose on and off phases,
   * and the number of times they repeat, are patched per frame from the car's
   * speed.
   */
  struct
  {
    /* $823B: (SM in play_engine_sfx_48k) Countdown within the current engine
     * tone phase */
    u8        counter;

    /* $8244: (SM in play_engine_sfx_48k) Number of speaker toggle loops per
     * engine tone burst */
    u8        nloops;

    /* $8249: (SM in play_engine_sfx_48k) Length of the engine tone's off phase */
    u8        off_cycle;

    /* $8251: (SM in play_engine_sfx_48k) Length of the engine tone's on phase */
    u8        on_cycle;
  } engine_sfx;

  /* $824B-$825D: (SM in attract_mode_128k, relocated) Attract mode screen
   * state */
  struct
  {
    /* $824B: (SM in attract_mode_128k, relocated) Blink phase for the attract
     * screen's flashing message */
    u8        blink;

    /* $825D: (SM in attract_mode_128k, relocated) Frames left before the attract
     * demo gives way to the title screen */
    s8        countdown;

    /* $F51B: best_officers overlay message data, initialised from
     * best_officers_template (Create.c) by chq_initialise and patched in
     * place by refresh_best_officers so the attract-mode "BEST OFFICERS"
     * rows track the live high-score table. */
    u8        best_officers[163];
  } attract_mode_128k;

  /* $8277: (SM in attract_mode_48k) Rotating bit pattern deciding which attract
   * messages are drawn this frame */
  u8        attract_blinker;

  /* $85EB: (SM in reveal_perp_car) Height of the perp's car revealed so far
   * during the pregame wipe */
  u8        pregame_car_revealed_height;

  /* $8614: (SM in animate_meters) Current fill level of each pregame meter,
   * 0-7
   *
   * [0] is the upper meter, [1] the lower.
   *
   * Conv: the Z80 keeps the two levels in separate self-modifying LD A,n
   *       operands at $8614 and $8631, one per unrolled copy of the update
   *       code. The C port merges them into one array; animate_meters still
   *       keeps both copies of the update, as the Z80 does.
   */
  s8        meter_level[2];

  /* $86C3-$86C9: (SM in draw_pregame) Pregame screen artwork selection */
  struct
  {
    /* $86C3: (SM in draw_pregame) Backdrop variant drawn behind the pregame
     * screen */
    u8        background;

    /* $86C9: (SM in draw_pregame) Direction the pregame artwork slides in from */
    u8        direction;
  } draw_pregame;

  /* $897C: Crash sound effect waveform
   *
   * A mutable copy of the template: the effect rotates the bytes in place as
   * it plays.
   */
  u8        sfx_crash_table[93];

  /* $8A0F: (SM in sfx_cornering) Speaker level toggle for the tyre-squeal
   * effect */
  u8        sfx_cornering_toggle;

  /* $8ABE: (SM in handle_perp_caught) Frames left in the current phase of the
   * arrest sequence */
  u8        handle_perp_caught_delay;

#define SCORE_MESSAGES_BASE         (0x8C58)
#define SCORE_MESSAGES_LENGTH       (0x8CB2 - SCORE_MESSAGES_BASE)
  /* $8C58: Score-screen message records, patched at runtime with the current
   * figures */
  u8        score_messages[SCORE_MESSAGES_LENGTH];

#define CONTINUE_MESSAGES_BASE      (0x8D18)
#define CONTINUE_MESSAGES_LENGTH    (0x8D8F - CONTINUE_MESSAGES_BASE)
#define CONTINUE_MESSAGES_TIME_NN   (0x8D7C - CONTINUE_MESSAGES_BASE)
#define CONTINUE_MESSAGES_CREDIT_N  (0x8D8C - CONTINUE_MESSAGES_BASE)
  /* $8D18: Continue-screen message records, patched at runtime with the
   * countdown and credit count */
  u8        continue_messages[CONTINUE_MESSAGES_LENGTH];

  /* $8DA1-$8DBB: (SM in transition) Screen fade state */
  struct
  {
    /* $8DA1: (SM in transition) Number of frames the current screen fade runs
     * for */
    u8        nframes;

    /* $8DB1: (SM in transition) Fade step value: 8 or -8 */
    s16       frame_stride;

    /* $8DBB: (SM in transition) Dither mask table driving the current fade */
    const u8 *mask;
  } transition;

  /* $8E43-$8E49: (SM in draw_overlay_messages) Overlay message reveal state
   *
   * Overlay messages are typed on a character at a time; these track how far
   * through the current message the reveal has got.
   */
  struct
  {
    /* $8E43: (SM in draw_overlay_messages) Message record currently being
     * revealed */
    const u8 *message;

    /* $8E46: (SM in draw_overlay_messages) Characters of the message revealed so
     * far */
    u8        count;

    /* $8E49: (SM in draw_overlay_messages) Frames left before the next character
     * is revealed */
    u8        delay;
  } overlay;

  /* $8F82-$8FA7: (SM in draw_scene_objects) Per-frame scenery draw enables */
  struct
  {
    /* $8F82: (SM in draw_scene_objects) Enables drawing of the first tunnel
     * mouth */
    u8        draw_tunnel_1;

    /* $8FA4: (SM in draw_scene_objects) Enables drawing of the helicopter */
    u8        draw_helicopter;

    /* $8FA7: (SM in draw_scene_objects) Enables drawing of the second tunnel
     * mouth */
    u8        draw_tunnel_2;
  } dee;

  /* $90F1-$9115: (SM in draw_overhead) Geometry of the overhead structure
   * being drawn */
  struct
  {
    /* $90F1: (SM in draw_overhead) Vertical offset subtracted from the overhead
     * structure's screen position */
    u8        vert_sub;

    /* $9115: (SM in draw_overhead) Width of the overhead structure's deck span,
     * in words */
    u8        span_width_words;
  } overhead;

  /* $933D-$946F: (SM in draw_object_clipped) Sprite plot parameters
   *
   * draw_object_common patches its own plot calls rather than passing
   * arguments: a sprite is drawn as up to two runs of rows, each with its own
   * row count, bitmap pointer and plot routine, and a mask may be plotted the
   * same way beforehand.
   */
  struct
  {
    /* $9395: (SM in draw_object_clipped) Selects which pre-shifted copy of the
     * sprite to plot */
    u8        shift_select;

    /* $933D: (SM in draw_object_clipped) Signed column adjustment applied to the
     * sprite's screen position */
    s8        col_pos;

    /* $93C0: (SM in draw_object_clipped) Plot dispatch selector
     *
     * Dispatch selector at $93B4: 0 = normal plot; 1 = vertically-inverted plot
     * (hazard hit-wobble flip, e.g. barriers); 2 = multi-section column plot
     * (draw_stretchy_object_common's single-pixel-column case for poles and
     * signs) -- unrelated to inversion despite sharing this field.
     */
    u8        plot_mode;

    /* $9404: (SM in draw_object_clipped) Row count for the sprite's first plot
     * call */
    u8        rows_main;

    /* $940F: (SM in draw_object_clipped) Plot routine for the sprite's first plot
     * call */
    plot_sprite_cb_t *plot_fn;

    /* $9412: (SM in draw_object_clipped) Bitmap data for the sprite's second plot
     * call */
    const u8 *bitmap_ptr;

    /* $9415: (SM in draw_object_clipped) Row count for the sprite's second plot
     * call */
    u8        rows_2nd;

    /* $941D: (SM in draw_object_clipped) Plot routine for the sprite's second
     * plot call */
    plot_sprite_cb_t *plot_fn_2;

    /* $945F: (SM in draw_object_clipped) Row count for the mask's first plot
     * call */
    u8        mask_rows_main;

    /* $946C: (SM in draw_object_clipped) Bitmap data for the mask's second plot
     * call */
    const u8 *mask_bitmap_ptr;

    /* $946F: (SM in draw_object_clipped) Row count for the mask's second plot
     * call */
    u8        mask_rows_2nd;
  } doc;

  /* $9618: Pseudo-random number generator state */
  u8        rng_seed[3];

  /* $962E: Next character to type out of the current chatter string */
  const char *next_character;

  /* $9630: Current byte of the chatter block being interpreted */
  const u8 *chatterblk_ptr;

  /* $9632: Column the next chatter character is typed at */
  u8        message_x;

  /* $9633: Frames left before the next chatter character is typed */
  u8        chatter_delay;

  /* $9634-$963C: Noise/static effect drawn over the mugshot area
   *
   * While a character's mugshot is on screen the face area is filled with
   * animated static, drawn by draw_noise_effect and wound down over four
   * frames by drive_noise_effect.
   */
  struct
  {
    /* $9634: Rotating state the pseudo-random pixel bytes are derived from */
    u8        bytes[5];

    /* $963C: Frames left on the effect; bit 0 also selects the cursor style */
    u8        counter;
  } noise;

  /* $963D: Chatter state machine phase */
  u8        chatter_state;

  /* $963E: Priority of the chatter currently playing; a lower-priority message
   * cannot interrupt it */
  chatterpriority_t chatter_priority;

  /* $9982: (SM) Blink phase of the cursor trailing the chatter text */
  u8        chatter_cursor_blink;

  /* $9C85-$9C86: (SM) Countdown timer remainder */
  struct
  {
    /* $9C85: (SM in tick_check_credits) Seconds left on the countdown, doubled
     *
     * Conv: the Z80's 16-bit load was split into this field and
     *       tick.remaining_subseconds.
     */
    u8        remaining_seconds_x2;

    /* $9C86: (SM in tick_update_remaining_time) Sub-second delay factor,
     * roughly sixths of a second
     *
     * Conv: the Z80's 16-bit load was split into tick.remaining_seconds_x2 and
     *       this field.
     */
    u8        remaining_subseconds;
  } tick;

  /* $9D51: Bonus figure rendered as text for the score screen */
  char      bonus_string[6];

  /* $9D5B: Stage number rendered as text for the pregame screen */
  u8        stage_n[7];

  /* $9D9B: First digit of the score to redraw this frame */
  const u8 *score_digits_start;

  /* $9E22: (SM in plot_turbos_and_digits) Animation frame of the spinning
   * turbo indicator */
  u8        turbo_spin_frame;

  /* $A0CC: Set when the Kempston joystick is the chosen control scheme */
  u8        kempston_flag;

  /* $A0CD: Packed keyboard row/column codes for each game control */
  u8        keydefs[KEYDEF__LIMIT];

  /* $A0D5: Controls held this frame, as USERINPUTFLAG_* bits */
  u8        user_input;

  /* $A139: Set when running on a 128K machine */
  u8        mode_128k;

  /* $A13A: Stage currently being played; initialised to 1 */
  u8        current_stage_number;

  /* $A13B: Cycles 3, 2, 1 to vary the speech sample played at stage start;
   * initialised to 4 */
  u8        start_speech_cycle;

  /* $A13C: Bonus awarded per overtake, packed BCD */
  u8        overtake_bonus_bcd;

  /* $A13D: Credits remaining */
  u8        credits;

  /* $A16D: Per-session counters and on-screen digit caches */
  session_t session;

  /* $A188: Every object currently on the road, moving or fixed */
  hazard_t  hazards[MAXHAZARDS];

  /* $A213: Soft copy of the AY-3-8912 registers for this frame */
  ay_register_cache_t ay_regs;

  /* $A220: Suppresses the per-frame attribute update */
  u8        dont_draw_screen_attrs;

  /* $A221: Suppresses collision detection, e.g. during the arrest sequence */
  u8        inhibit_collision_detection;

  /* $A222: Number of hazard slots currently in use */
  u8        n_hazards;

  /* $A223: Stage number shown on the status panel */
  u8        displayed_stage;

  /* $A224: Drives the helicopter's scripted behaviour */
  u8        helicopter_control;

  /* $A225: Suppresses spawning of NPC cars */
  u8        dont_spawn_cars;

  /* $A226: Which branch of a fork leads to the perp */
  u8        correct_fork;

  /* $A227: Animation phase of the arrow pointing to the correct fork */
  u8        floating_arrow;

  /* $A228: Warning light shown as the perp comes into range */
  u8        cherry_light;

  /* $A229: Phase of the out-of-time sequence */
  u8        time_up_state;

  /* $A22A: Hero car's vertical screen position */
  u8        car_y;

  /* $A22B: Overtakes counted since the last bonus was awarded */
  u8        overtake_bonus_counter;

  /* $A22C: Set when a bonus is due to be awarded */
  u8        trigger_bonus_flag;

  /* $A22D: Frames left showing the awarded bonus */
  u8        bonus_counter;

  /* $A22E: Set once the perp has been sighted */
  u8        sighted_flag;

  /* $A22F: Drives the pointing-hand animation */
  u8        hand_flag;

  /* $A230: Phase of the perp-caught sequence */
  u8        perp_caught_phase;

  /* $A231: Drives the screen fade between scenes */
  u8        transition_control;

  /* $A232: Damage inflicted on the perp's car so far */
  u8        smash_level;

  /* $A233: Frames left in the current smash animation */
  u8        smash_counter;

  /* $A234: General per-frame animation counter */
  u8        anim_counter;

  /* $A235: Alternates each frame, used to halve animation rates */
  u8        frame_toggle;

  /* $A236: Animation counter advancing slower than anim_counter */
  u8        slow_anim_counter;

  /* $A237: Sound effect currently playing */
  u8        sfx_index;

  /* $A238: Priority of the sound effect currently playing */
  u8        sfx_priority;

  /* $A239: Set while the siren is sounding */
  u8        siren_enabled;

  /* $A23A: Pitch of the turbo boost effect */
  u8        turbo_sfx_pitch;

  /* $A23B: Set while the in-tunnel engine echo is sounding */
  u8        tunnel_sfx;

  /* $A23C: Requests the right-hand cornering effect */
  u8        trigger_righthand_sfx;

  /* $A23D: Requests the left-hand cornering effect */
  u8        trigger_lefthand_sfx;

  /* $A23E: Wheels off the road: 0 = on-road, 1 = one wheel off, 2 = both
   * wheels off */
  u8        off_road;

  /* $A23F: Counter advancing once per frame, used to pace fast animations */
  u8        fast_counter;

  /* $A240: Cursor into the circular road buffer */
  u8       *roadbufptr;

  /* First byte of the road buffer
   *
   * Conv: port-added. The Z80 wrapped the cursor by masking its low byte,
   *       which relies on the buffer being page-aligned at $EE00.
   */
  u8       *roadbuf_start;

  /* One past the last byte of the road buffer
   *
   * Conv: port-added, as roadbuf_start.
   */
  u8       *roadbuf_end;

  /* $A242: Curvature value read from the map this frame */
  u8        curvature_byte;

  /* $A243: Height value read from the map this frame */
  u8        height_byte;

  /* $A244: Left roadside value read from the map this frame */
  u8        leftside_byte;

  /* $A245: Right roadside value read from the map this frame */
  u8        rightside_byte;

  /* $A246: Slices left before the next hazard map command is read */
  u8        hazards_counter;

  /* $A247: Slices left before the next lane map command is read */
  u8        lanes_counter;

  /* $A248: Set while the road surface is dirt rather than tarmac */
  u8        on_dirt_track;

  /* $A249: Which branch of the fork the player took */
  u8        fork_taken;

  /* $A24A: Hero car's speed */
  u16       speed;

  /* $A24C: Frames left on the current incline */
  u8        inclined_counter;

  /* $A24D: Set while the car is cornering hard enough to squeal */
  u8        cornering;

  /* $A24E: Turbo boosts remaining */
  u8        boost;

  /* $A24F: Frames left on the exhaust smoke effect */
  u8        smoke;

  /* $A250: Rate the car moves across the road while steering */
  u8        turn_speed;

  /* $A251: Selects the mirrored hero car sprite */
  u8        flip_car;

  /* $A252: Blocks further gear changes until the control is released */
  u8        gear_lockout;

  /* $A253: Gear the car is actually in */
  u8        gear;

  /* $A254: Permits hazards to be spawned */
  u8        allow_spawning;

  /* $A255: Distance travelled, packed BCD */
  u8        distance_bcd[2];

  /* $A258: Road gradient: $FD..$03 = climbing, level, descending */
  s8        incline;

  /* $A259: Previous frame's road height, used to derive the incline */
  s8        prev_road_height;

  /* $A25A: Accumulated horizon movement from changes in incline; goes 0, 1,
   * 2 */
  u8        horizon_y_accum;

  /* $A25B: Amount the horizon moves vertically per frame */
  u8        horizon_y_step;

  /* $A25C: Curvature of the road at the car's position */
  u8        current_curvature;

  /* $A25D: Index into the curvature table for the horizon */
  u8        horizon_curve_index;

  /* $A25E: Horizon scroll phase; cycles 4..1 or similar when roads curve */
  u8        horizon_x_scroll;

  /* $A25F: Lateral offset applied to the whole road as it curves */
  s16       horizontal_adjust;

  /* $A261: Fractional part of the horizon's horizontal scroll */
  u8        horizon_scroll_sub;

  /* $A262: Frames elapsed on the current curve, paces the horizon scroll */
  u8        curvature_ticks;

  /* $A263: Set while the road bends right */
  u8        right_turn;

  /* $A264: Set while the road bends left */
  u8        left_turn;

  /* $A265: Set while a fork is on screen */
  u8        fork_visible;

  /* $A266: Frames left before the fork must be committed to */
  u8        fork_countdown;

  /* $A267: Distance remaining to the fork */
  u16       fork_distance;

  /* $A269: Set while the road is laid out as two diverging branches */
  u8        fork_in_progress;

  /* $A26A: Phase of the player's quit request: QUITSTATE_IDLE, _START or
   * _DONE */
  u8        quit_state;

  /* $A26B: Speech sample queued at stage start, and its delay */
  u8        start_speech;

  /* $A26C: Scenery layout for the stage being played */
  scenedata_t scenedata;

  /* $A68F-$A749: (SM in perp_behaviour) Perp AI timers */
  struct
  {
    /* $A68F: (SM in perp_behaviour) Set while the perp is changing lane */
    u8        changing_lane;

    /* $A69B: (SM in perp_behaviour) Frames left in the perp's lane change */
    u8        lane_change_timer;

    /* $A73E: (SM in perp_behaviour) Frames left before the perp next reacts */
    u8        delay;

    /* $A749: (SM in perp_behaviour) Delay before the perp lets the player close
     * in */
    u8        approach_timer;
  } pb;

  /* $A804: (SM in spawn_cars) Frames left before the next NPC car is spawned */
  u8        sc_spawn_counter;

  /* $A97E: (SM in layout_dirt_and_stones) Enables layout_dirt_and_stones */
  u8        ldas_enabled;

  /* $A9DE-$A9E2: (SM in draw_dirt_and_stones) Dirt and stone particle drawing
   * state */
  struct
  {
    /* $A9DE: (SM in draw_dirt_and_stones) Enables draw_dirt_and_stones */
    u8        enabled;

    /* $A9E2: (SM in draw_dirt_and_stones) Byte cursor into the $ED28 particle
     * table */
    u8       *particle;
  } ddas;

  /* $AA5A-$AA8C: (SM in draw_helicopter) Helicopter drawing position */
  struct
  {
    /* $AA5A: (SM in draw_helicopter) Helicopter's vertical base position
     *
     * Swing offset plus height, minus the per-distance object position when
     * drawn.
     */
    u8        vert_base;

    /* $AA76: (SM in draw_helicopter) Vertical offset of the helicopter's swing */
    u8        y_offset;

    /* $AA8C: (SM in draw_helicopter) Animation phase of the rotor */
    u8        rotor_pos;
  } dheli;

  /* $AA94: (SM in draw_helicopter) Helicopter's horizontal position
   *
   * Read by draw_helicopter and drive_helicopter.
   */
  s16       helicopter_x;

  /* $AACB-$AB06: (SM in move_helicopter) Helicopter flight state */
  struct
  {
    /* $AACB: (SM in move_helicopter) Helicopter's height above the road */
    u8        height;

    /* $AAD7: (SM in move_helicopter) Animation counter, 0..3 */
    u8        anim_frame;

    /* $AADF: (SM in move_helicopter) Direction of travel: -1 or 1 */
    s8        direction;

    /* $AAE8: (SM in move_helicopter) Horizontal offset from the flight path's
     * centre */
    u8        offset;

    /* $AAF6: (SM in move_helicopter) Previous frame's road position */
    u16       prev_road_pos;

    /* $AB06: (SM in move_helicopter) Vertical centre of the flight path: 112
     * normally, -56 during the exit sequence */
    s16       centre_y;
  } mh;

  /* $AE70: (SM in advance_hazard) Left edge of the road at the hazard's
   * distance */
  u16       ah_road_left_xpos;

  /* $AED0-$B02C: (SM in draw_hazard_sprites) Hazard sprite drawing state */
  struct
  {
    /* $AED0: (SM in draw_hazard_sprites) Cursor into the depth-sorted draw list
     *
     * The list of (distance, hazard) pairs is built over xpos.centre_left
     * ($E900).
     */
    s16      *xpos_table;

    /* $AFFB: (SM in draw_hazard_sprites) Level-of-detail index 0..5
     *
     * Derived from the hazard's draw-list depth; picks the arrow, smoke and fire
     * sprite sizes.
     */
    u8        lod_index;

    /* $B023: (SM in draw_hazard_sprites) Column adjustment for the hazard being
     * drawn */
    u8        col_pos;

    /* $B029: (SM in draw_hazard_sprites) Clip side for the hazard being drawn:
     * 0 = on screen, <0 = clip left, >0 = clip right */
    s8        horz_clip;

    /* $B02C: (SM in draw_hazard_sprites) Screen column of the hazard being
     * drawn */
    u8        horz_pos;
  } dhs;

  /* $B063-$B079: (SM in move_hero_car) Hero car jump state */
  struct
  {
    /* $B063: (SM in move_hero_car) Height of the car through a jump */
    u8        y_offset;

    /* $B079: (SM in move_hero_car) Current entry in the jump trajectory table */
    const u8 *jump_data;
  } mhc;

  /* $B325-$B478: (SM in animate_hero_car) Hero car crash and hand animation
   * state */
  struct
  {
    /* $B325: (SM in animate_hero_car) Non-zero while the crash sequence runs */
    u16       crashed_flag;

    /* $B32E: (SM in animate_hero_car) Speed the car must drop below before the
     * crash ends */
    u16       crash_speed_threshold;

    /* $B356: (SM in animate_hero_car) Lateral drift added to road_pos each frame
     * during a crash spin; decays by 1/16 per frame */
    u16       crash_spin_speed;

    /* $B36E: (SM in animate_hero_car) Drift direction selector: 1 or 2; bit 0
     * also drives flip_car */
    u8        flip_flag;

    /* $B384: (SM in animate_hero_car) Delay counter, set to 5 */
    u8        delay;

    /* $B38D: (SM in animate_hero_car) Flips left in the crash animation: flip
     * flag plus one */
    u8        crash_flip_count;

    /* $B395: (SM in animate_hero_car) Lowest road_pos the car may reach: 72
     * normally, 209 inside a tunnel */
    u16       road_pos_min;

    /* $B3A3: (SM in animate_hero_car) Highest road_pos the car may reach: 472
     * normally, 405 inside a tunnel */
    u16       road_pos_max;

    /* $B3DB: (SM in animate_hero_car) Non-zero while the car is still spinning;
     * gates the flip animation */
    u8        crash_spin;

    /* $B476: (SM in animate_hero_car) Step of the pointing-hand animation */
    u8        hand_step;

    /* $B478: (SM in animate_hero_car) Frames left on the current hand animation
     * step; reloads to 2, or 3 on step 2, as ahc.hand_step advances */
    u8        hand_delay;
  } ahc;

  /* $B4F0: (SM in smash) Cycles the damage sprite shown as the perp's car is
   * smashed */
  u8        smash_cycling_counter;

  /* $B549-$B570: (SM in draw_debris) Debris animation state */
  struct
  {
    /* $B549: (SM in draw_debris) Frames left on the debris animation; set to 9
     * by smash */
    u8        frame_counter;

    /* $B55B: (SM in draw_debris) Subtable the current debris frame is drawn
     * from */
    u8      **subtables_start;

    /* $B570: (SM in draw_debris) Offset of the current frame within the debris
     * subtable */
    u16       frame_offset;
  } dd;

  /* $B5AA-$B5AF: (SM in draw_hero_car) Hero car sprite selection */
  struct
  {
    /* $B5AA: (SM in draw_hero_car) Height of the car in the air, leaving its
     * shadow on the ground */
    u8        jump_y;

    /* $B5AF: (SM in draw_hero_car) Car's pitch: 0, 3 or 6 = level, nose up, nose
     * down */
    u8        pitch;
  } dhc;

  /* $BB8B-$C0BB: (SM in rm_cycle_buffer_offset and read_map) Map stream
   * bookkeeping
   *
   * A fork splits each of the six map streams in two. The leftfork_* and
   * rightfork_* addresses are where each stream's branch begins, and the
   * *_fork_end_ptr pointers are where it rejoins.
   */
  struct
  {
    /* $BB8B: (SM in rm_cycle_buffer_offset) Z80 address of the hazard stream's
     * left-fork branch */
    u16       leftfork_hazards;

    /* $BB8E: (SM in rm_cycle_buffer_offset) Z80 address of the rightside
     * stream's left-fork branch */
    u16       leftfork_rightside;

    /* $BB91: (SM in rm_cycle_buffer_offset) Z80 address of the leftside stream's
     * left-fork branch */
    u16       leftfork_leftside;

    /* $BB95: (SM in rm_cycle_buffer_offset) Z80 address of the curvature
     * stream's left-fork branch */
    u16       leftfork_curve;

    /* $BB98: (SM in rm_cycle_buffer_offset) Z80 address of the height stream's
     * left-fork branch */
    u16       leftfork_height;

    /* $BB9B: (SM in rm_cycle_buffer_offset) Z80 address of the lanes stream's
     * left-fork branch */
    u16       leftfork_lanes;

    /* $BBB8: (SM in rm_cycle_buffer_offset) Z80 address of the hazard stream's
     * right-fork branch */
    u16       rightfork_hazards;

    /* $BBBB: (SM in rm_cycle_buffer_offset) Z80 address of the rightside
     * stream's right-fork branch */
    u16       rightfork_rightside;

    /* $BBBE: (SM in rm_cycle_buffer_offset) Z80 address of the leftside stream's
     * right-fork branch */
    u16       rightfork_leftside;

    /* $BBC2: (SM in rm_cycle_buffer_offset) Z80 address of the curvature
     * stream's right-fork branch */
    u16       rightfork_curve;

    /* $BBC5: (SM in rm_cycle_buffer_offset) Z80 address of the height stream's
     * right-fork branch */
    u16       rightfork_height;

    /* $BBC8: (SM in rm_cycle_buffer_offset) Z80 address of the lanes stream's
     * right-fork branch */
    u16       rightfork_lanes;

    /* $BE6D: (SM in rm_cycle_buffer_offset) Curvature stream's one_command
     * pointer where the fork rejoins */
    const u8 *curvature_fork_end_ptr;

    /* $BEBF: (SM in rm_cycle_buffer_offset) Height stream's one_command pointer
     * where the fork rejoins */
    const u8 *height_fork_end_ptr;

    /* $BF0A: (SM in rm_cycle_buffer_offset) Lanes stream's one_command pointer
     * where the fork rejoins */
    const u8 *lanes_fork_end_ptr;

    /* $BF2D: (SM in rm_cycle_buffer_offset) Lane layout currently in force */
    u8        lanes_byte;

    /* $BF84: (SM in rm_cycle_buffer_offset) Rightside stream's one_command
     * pointer where the fork rejoins */
    const u8 *rightside_fork_end_ptr;

    /* $BFCD: (SM in rm_cycle_buffer_offset) Leftside stream's one_command
     * pointer where the fork rejoins */
    const u8 *leftside_fork_end_ptr;

    /* $C046: (SM in rm_cycle_buffer_offset) Hazard stream's one_command pointer
     * where the fork rejoins */
    const u8 *hazards_fork_end_ptr;

    /* $C058: (SM in read_map) Hazard command currently being acted on */
    u8        hazard_byte;

    /* $C0BB: (SM in read_map) Enables scrolling of the dirt particle layer */
    u8        scroll_dirt_particles;
  } rm;

  /* $C15D-$C2B8: (SM in draw_tunnel) Tunnel drawing state */
  struct
  {
    /* $C15D: (SM in draw_tunnel) Distance to the tunnel mouth: 15 when small, 6
     * when it fills the screen */
    u8        tunnel_distance;

    /* $C160: (SM in draw_tunnel) Tunnel visibility: 0 = not visible, 1 =
     * approaching, 2 = inside */
    u8        tunnel_visible;

    /* $C21C: (SM in draw_tunnel) Byte pair the tunnel interior is filled with */
    u16       fill_pattern;

    /* $C221: (SM in draw_tunnel) Jump table entry the first fill run starts at */
    u8        fill_start_a;

    /* $C236: (SM in draw_tunnel) Jump table entry the second fill run starts
     * at */
    u8        fill_start_b;

    /* $C2B8: (SM in draw_tunnel) Selects how the far wall of the tunnel is
     * drawn */
    u8        far_wall_mode;
  } dt;

  /* $C56C-$C88F: (SM in draw_road) Road drawing state
   *
   * draw_road patches the widths, fill patterns, marking variants and buffer
   * addresses for the row it is about to draw, then runs a common inner loop.
   */
  struct
  {
    /* $C56C: (SM in draw_road) Back buffer address the first road span is drawn
     * to */
    u16       backbuf_1;

    /* $C5AC: (SM in draw_road) Negated count of lanes to draw */
    s8        neg_lane_count;

    /* $C5B3: (SM in draw_road) Memory page holding the left verge fill pattern */
    u8        left_fill_page;

    /* $C5D9: (SM in draw_road) Memory page holding the right verge fill
     * pattern */
    u8        right_fill_page;

    /* $C5F9: (SM in draw_road) Back buffer address the second road span is drawn
     * to */
    u16       backbuf_2;

    /* $C60A: (SM in draw_road) Width of the right-hand road stripe */
    u8        right_stripe_width;

    /* $C61B: (SM in draw_road) Width of the road at the row being drawn */
    u8        road_width;

    /* $C62C: (SM in draw_road) Width of the left-hand road stripe */
    u8        left_stripe_width;

    /* $C642: (SM in draw_road) Memory page holding the left edge markings */
    u8        left_markings_page;

    /* $C651: (SM in draw_road) Offset of the edge marking variant within
     * edge_markings */
    u8        edge_graphic_offset;

    /* $C677: (SM in draw_road) Offset of the lane marking variant within
     * edge_markings */
    u8        stripe_table_offset;

    /* $C68A: (SM in draw_road) Memory page holding the right edge markings */
    u8        right_markings_page;

    /* $C698: (SM in draw_road) Offset applied to the right edge marking */
    u8        right_edge_offset;

    /* $C6B2: (SM in draw_road) Initial road stripe state */
    u8        initial_stripe_state;

    /* $C6BC: (SM in draw_road) Byte the road surface is filled with */
    u8        fill_pattern;

    /* $C6D3: (SM in draw_road) Base value the stripe pattern is XORed against */
    u8        stripe_xor_base;

    /* $C6D8: (SM in draw_road) Road edge line thickness */
    u8        edge_thickness;

    /* $C7E7: (SM in draw_road) Horizon scroll phase applied to the backdrop */
    u8        horizon_x_scroll;

    /* $C80A: (SM in draw_road) Number of sky rows above the horizon */
    u8        sky_rows;

    /* $C86C: (SM in dr_start_backdrop_fill) Offset of the first LDI in the blit
     * instruction stream, 0..18 */
    u8        backdrop_copy_jump;

    /* $C82D: Runtime-selected 18-byte blit instruction stream, built by
     * dr_start_backdrop_fill */
    u8        backdrop_copy_instrs[18];

    /* $C88F: (SM in draw_road) Set while the car is inside a tunnel */
    u8        in_tunnel;
  } dr;

  /* $CE0C-$CE32: Three 13-byte smoke animation buffers, indexed 0..2 */
  u8        smokes[3][13];

  /* $CE33: Pointers to the debris animation subtables */
  u8       *debris_table[12];

  /* $CE4B-$CEAA: Five 19-byte debris subtables, indexed 0..4 */
  u8        debris_subtables[5][19];

  /* $E300: Perspective height of each road slice
   *
   * [0] = $60 sentinel, [1..21] = perspective heights, [22] = $A0 terminal,
   * [23..31] unused.
   */
  u8        height_table[32];

  /* $E320: Per-depth curvature adjustment; built by build_curve_table */
  u8        curvature_table[PERSP_TABLE_COLS];

  /* $E336: Running-minimum clamp of height_table[1..21]; built by
   * build_height_table */
  u8        clamped_heights[ROAD_SLOT_COUNT];

  /* $E34B: Horizon attribute scroll state
   *
   * [0] is the previous rounded Cmin, [1] the current frame's delta (new minus
   * old, a multiple of 8) and [2] the previous frame's delta, which
   * ds_attributes reads one frame late. build_height_table writes [0] and [1]
   * each frame; see CLAUDE.md for the full sequence.
   */
  u8        horizon_attr[3];

  /* $E34E: Unused pad byte
   *
   * Kept so pointer arithmetic ending at $E34E/$E34F (advance_hazard,
   * draw_tunnel) lands correctly.
   */
  u8        horizon_attr_pad;

  /* $E34F: Screen position of the object at each road slice */
  u8        object_positions[ROAD_SLOT_COUNT];

  /* $E800-$ED00: Horizontal pixel position of each road boundary
   *
   * One table per boundary, all indexed by screen row, 0..127; only the
   * visible depth range is populated per frame. 0..255 is on-screen; values
   * outside that range are off-screen left or right during curves and are
   * clamped when consumed (see docs/draw-road-internals.md).
   */
  struct
  {
    /* $E800: Left outer edge, the verge/road boundary */
    s16       left[128];

    /* $E900: Left inner edge, centre-left */
    s16       centre_left[128];

    /* $EA00: Road centre line */
    s16       centre[128];

    /* $EB00: Right inner edge, centre-right */
    s16       centre_right[128];

    /* $EC00: Right outer edge, the road/verge boundary */
    s16       right[128];

    /* $ED00: Right fork's right outer edge
     *
     * Forked roads only; also reused as dirt and stone layout data on dirt
     * tracks.
     */
    s16       fork_right[128];
  } xpos;

  /* $EE00: Circular buffer of road slices between the car and the horizon */
  u8        road_buffer[256];

  /* $EF00: Table mapping each byte to its bit-reversed value */
  u8        flipped[256];

  /* $EADC: Name of the key being redefined, as characters */
  u8        messages_key_string[5];

  /* $EE38: Key definitions being assembled on the redefine-keys screen */
  u8        temp_keydefs[8];

  /* $EE6E-$F0C5: 48K music engine state, including the two drum samples */
  struct
  {
    /* (SM in next_pattern) Repeats left of the current pattern */
    u8        pattern_repeats;

    /* Current pattern address */
    const u8 *pattern_addr;

    /* Set once playback has begun */
    u8        started;

    /* Ticks left before the next note is fetched */
    u8        note_delay;

    /* Value note_delay reloads to */
    u8        note_delay_reload;

    /* Next byte of the pattern to be fetched */
    const u8 *data_ptr;

    /* First byte of the current pattern */
    const u8 *pattern_start_ptr;

    /* Extra ticks added to the current note's length */
    u8        extra_delay;

    /* Set while a drum sample is playing */
    u8        drum_active;

    /* Playback rate of the current drum sample */
    u8        drum_speed;

    /* $EF5E: Drum 1 PCM sample
     *
     * playdrum_go's RLC (HL) rotates the bytes in place, so like
     * sfx_crash_table this is a mutable copy of a template.
     */
    u8        drum1[252];

    /* $F05A: Drum 2 PCM sample
     *
     * A mutable copy of a template, as drum1.
     */
    u8        drum2[108];
  } music;

  /* $F000: Off-screen buffer the playfield is composed in before being sent to
   * the screen */
  u8        backbuffer[BACKBUFFER_LENGTH + BACKBUFFER_OVERFLOW];

  /* $EC01-$F224: Title-screen tune engine and options-menu state (128K bank 3
   * only)
   *
   * Channel trackers and self-modifying scratch bytes, plus title-screen
   * animation and options-menu state. Opaque pointer: the fields live in
   * struct chq_bank3_state (Bank3State.h), which only Bank3.c includes, so no
   * other translation unit can read or write them. Allocated and freed by
   * bank3_state_create/bank3_state_destroy (Bank3.h), called from Create.c.
   */
  struct chq_bank3_state *bank3;

  /* $A16D/$A170/$5C31-$5C32: End-screen script interpreter state (128K bank 7
   * only)
   *
   * Drives run_script ($E20A). Opaque pointer: the fields live in
   * struct chq_bank7_state (Bank7State.h), which only Bank7.c includes, so no
   * other translation unit can read or write them. Allocated and freed by
   * bank7_state_create/bank7_state_destroy (Bank7.h), called from Create.c.
   */
  struct chq_bank7_state *bank7;

  /* Port-added Z80 shadow-register state
   *
   * The C port has no shadow register bank; values the original code carries
   * across calls via EX AF,AF' / EXX are modelled here instead.
   */
  struct
  {
    /* Curvature scroll amount banked in the AF' shadow register.
     *
     * Models the Z80 AF' shadow register as banked by move_hero_car's
     * EX AF,AF' at $B296 (holding BC_count_scaled) and read back by
     * scroll_horizon's EX AF,AF' at $B854. Only written when move_hero_car
     * actually reaches $B296 (current_curvature != 0 and ticks elapsed).
     */
    u8        curvature_scroll;
  } shadow;
};

#endif /* CHASEHQ_STATE_H */
