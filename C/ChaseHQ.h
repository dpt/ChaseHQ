// ChaseHQ.h
//
// Chase H.Q. code model
//
// by dpt

// vim: ts=8 sts=2 sw=2 et

#ifndef CHASEHQ_H
#define CHASEHQ_H

#include "Types.h"
#include "Spectrum.h"

/* ----------------------------------------------------------------------- */

#define MARQUEE_HEIGHT        (8 * 8) // rows
#define PLAYFIELD_HEIGHT      (16 * 8) // rows

#define BACKBUFFER_WIDTH      (256)
#define BACKBUFFER_ROWBYTES   (BACKBUFFER_WIDTH / 8)
#define BACKBUFFER_HEIGHT     (128)
#define BACKBUFFER_LENGTH     (BACKBUFFER_ROWBYTES * BACKBUFFER_HEIGHT)
#define BACKBUFFER_START_ADDRESS ((u16) 0xF000)
#define BACKBUFFER_END_ADDRESS (BACKBUFFER_START_ADDRESS + BACKBUFFER_LENGTH)

// Return screen[] pointer given a Z80 address.
#define ADDRTOSCREEN(addr)    (&state->screen[(addr) - SCREEN_START_ADDRESS])
// Return backbuffer[] pointer given a Z80 address.
#define ADDRTOBACKBUF(addr)   (&state->backbuffer[(addr) - BACKBUFFER_START_ADDRESS])

// Return byte offset of screen[] pointer.
#define SCREENTOOFFSET(ptr)   ((ptr) - &state->screen[0])
// Return byte offset of backbuffer[] pointer.
#define BACKBUFTOOFFSET(ptr)  ((ptr) - &state->backbuffer[0])

// Return screen[] pointer given byte offset.
#define OFFSETTOSCREEN(off)   (&state->screen[off])
// Return backbuffer[] pointer given byte offset.
#define OFFSETTOBACKBUF(off)  (&state->backbuffer[off])

// Return ptr incremented modulo 256.
#define WRAPPINGINCREMENT(ptr, base) &base[((ptr) + 1 - (base)) & 0xFF]

#define STAGEDATA_BASE        (0x5C00)
#define STAGEDATA_END         (0x7FFF) // inclusive
#define STAGEDATA_LENGTH      (STAGEDATA_END + 1 - STAGEDATA_BASE)

#define MAXHAZARDS            (6)

#define MARQUEELIGHTWIDTH     (5)
#define MARQUEELIGHTHEIGHT    (4)

#define STREND                (1<<7) // string terminating top bit

#define TURBOWIDTH            (16) // pixels
#define TURBOROWBYTES         (TURBOWIDTH / 8)
#define TURBOHEIGHT           (14)
#define TURBOFRAMELENGTH      (TURBOROWBYTES * TURBOHEIGHT * 2) // masked
#define TURBOFRAMES           (3)

#define FACEWIDTH             (32)
#define FACEROWBYTES          (FACEWIDTH / 8)
#define FACEHEIGHT            (40)
#define FACEBITMAPBYTES       (FACEROWBYTES * FACEHEIGHT)
#define FACEATTRBYTES         (4 * 5)
#define FACEBYTES             (FACEBITMAPBYTES + FACEATTRBYTES)
#define NFACES                (3)

#define MINSTAGE              (1)
#define MAXSTAGE              (5)

/* ----------------------------------------------------------------------- */

#define DRAWCHARSTYLE_DUNNO                    (0)
#define DRAWCHARSTYLE_GENERIC                  (1)
#define DRAWCHARSTYLE_SINGLE                   (2)
#define DRAWCHARSTYLE_DOUBLE                   (3)
#define DRAWCHARSTYLE_SINGLE_INVERTED          (4)
#define DRAWCHARSTYLE_DOUBLE_INVERTED          (5)
#define DRAWCHARSTYLE__LIMIT                   (5)

#define QUITSTATE_IDLE                         (0)
#define QUITSTATE_START                        (1)
#define QUITSTATE_DONE                         (2)

#define USERINPUT_RIGHT                     (1<<0)
#define USERINPUT_LEFT                      (1<<1)
#define USERINPUT_DOWN                      (1<<2) // aka brake
#define USERINPUT_UP                        (1<<3) // aka accelerate
#define USERINPUT_FIRE                      (1<<4) // aka gear
#define USERINPUT_TURBO                     (1<<5)
#define USERINPUT_PAUSE                     (1<<6)
#define USERINPUT_QUIT                      (1<<7)
#define USERINPUT_NOT_QUIT                  (0x7F)
#define USERINPUT_NONE                      (0x00)

#define USERINPUTMASK_ALLOW_NONE            (0x00)
#define USERINPUTMASK_ALLOW_ALL             (0xFF)

#define EFFECT_SQUEAL                          (1)
#define EFFECT_LANDING                         (2)
#define EFFECT_CAR_HIT                         (3)
#define EFFECT_SCENERY_HIT                     (4)
#define EFFECT_HAZARD_HIT                      (5)
#define EFFECT_WALL_HIT                        (6)
#define EFFECT_CORNERING                       (7)
#define EFFECT_BIP                             (8)
#define EFFECT_BOW                             (9)

// TODO: These state names need clarification
#define TIMEUPSTATE_INIT                       (0)
#define TIMEUPSTATE_CHECK_TIME_UP              (1)
#define TIMEUPSTATE_CAR_STOPPED                (2)
#define TIMEUPSTATE_CHECK_RESTART              (3)
#define TIMEUPSTATE_WAITING                    (4)

#define CHATTERSTATE_IDLE                      (0)
#define CHATTERSTATE_START                     (1)
#define CHATTERSTATE_RUN                       (2)
#define CHATTERSTATE_STOP                      (3)

typedef u8 chatterpriority_t;

#define PERPCAUGHTPHASE_0                      (0)
#define PERPCAUGHTPHASE_1                      (1)
#define PERPCAUGHTPHASE_2                      (2)
#define PERPCAUGHTPHASE_3                      (3)
#define PERPCAUGHTPHASE_4                      (4)
#define PERPCAUGHTPHASE_5                      (5)
#define PERPCAUGHTPHASE_6                      (6)

#define TRANSITIONCONTROL_STOP                 (0)
#define TRANSITIONCONTROL_DRAW_MUGSHOTS        (1)
#define TRANSITIONCONTROL_OVERLAY_MESSAGES     (2)
#define TRANSITIONCONTROL_FILL_ATTRIBUTES      (3)
#define TRANSITIONCONTROL_FADE                 (4)

#define TRANSITIONSTRIDE_FORWARD            (0x08)
#define TRANSITIONSTRIDE_REVERSE            (0xF8)

// Note: road_pos left..right is high..low
#define ROAD_126                          (0x0126)
#define ROAD_LEFTMOST                     (0x0105)
#define ROAD_RIGHTMOST                    (0x00F5)

#define HAZARD_USED                         (0xFF)
#define HAZARD_UNUSED                       (0x00)

#define ROADBUF_CURVATURE_OFFSET               (0)
#define ROADBUF_HEIGHT_OFFSET                 (32)
#define ROADBUF_LANES_OFFSET                  (64)
#define ROADBUF_RIGHTOBJS_OFFSET              (96)
#define ROADBUF_LEFTOBJS_OFFSET              (128)
#define ROADBUF_HAZARDS_OFFSET               (160)

#define PREGAMECMD_STOP                     (0x00)
#define PREGAMECMD_REPEAT                   (0x1F)
#define PREGAMECMD_SET_BG_0                 (0xD0) // to 0xDF
#define PREGAMECMD_DRAW_BASE                (0xE0)
#define PREGAMECMD_DRAW_HZ                  (0xE1)
#define PREGAMECMD_DRAW_VT                  (0xE2)
#define PREGAMECMD_SET_ADDR                 (0xF0) // to 0xFF

/* ----------------------------------------------------------------------- */

// Conv: The C version uses IDs for strings and blocks rather than inline
// addresses.

#define CHATTERCMD_RANDOM                   (0xFC) // Followed by three chatterblock indices
#define CHATTERCMD_PAUSE                    (0xFE) // Followed by a single chatterblock index
#define CHATTERCMD_STOP                     (0xFF)

/// Chatter characters
#define CHATTERCHR_PILOT                       (0)
#define CHATTERCHR_NANCY                       (1)
#define CHATTERCHR_RAYMOND                     (2)
#define CHATTERCHR_TONY                        (3)

/// Chatter string indices
#define CHATTERSTR_GIDDY_UP_BOY                (0)
#define CHATTERSTR_HOLD_ON_MAN                 (1)
#define CHATTERSTR_THIS_IS_NANCY               (2)
#define CHATTERSTR_THIS_IS_AIRBORNE            (3)
#define CHATTERSTR_TARGET_VEHICLE_TURNED       (4)
#define CHATTERSTR_RIGHT_AHEAD_OVER            (5)
#define CHATTERSTR_LEFT_AHEAD_OVER             (6)
#define CHATTERSTR_READ_LOUD_CLEAR             (7)
#define CHATTERSTR_ROGER                       (8)
#define CHATTERSTR_GOTCHA_NANCY                (9)
#define CHATTERSTR_WHAT_YOU_DOING             (10)
#define CHATTERSTR_GOING_OTHER_WAY            (11)
#define CHATTERSTR_MESSIN_AROUND              (12)
#define CHATTERSTR_TIME_RUN_OUT               (13)
#define CHATTERSTR_GET_MOVIN_MAN              (14)
#define CHATTERSTR_OH_NO                      (15)
#define CHATTERSTR_PLEASE                     (16)
#define CHATTERSTR_GREAT                      (17)
#define CHATTERSTR_OUCH                       (18)
#define CHATTERSTR_LETS_GO                    (19)
#define CHATTERSTR_YAOW                       (20)
#define CHATTERSTR_BEAR_DOWN                  (21)
#define CHATTERSTR_MORE_PUSH_MORE             (22)
#define CHATTERSTR_ONE_MORE_TIME              (23)
#define CHATTERSTR_OH_MAN                     (24)
#define CHATTERSTR_WHOA                       (25)
#define CHATTERSTR_HARDER                     (26)
#define CHATTERSTR_PICKED_WRONG_JOB           (27)
#define CHATTERSTR_CHECK_CLASSIFIED_ADS       (28)
#define CHATTERSTR_ONE_MORE_TRY               (29)
#define CHATTERSTR_MEDIOCRE_DRIVER            (30)
#define CHATTERSTR_SEE_YOU_LATER              (31)
#define CHATTERSTR__LIMIT                     (32)

/// Chatter block indices
#define CHATTERBLK_START_STAGE                 (0)
#define CHATTERBLK_TONY_GIDDY_UP               (1)
#define CHATTERBLK_TONY_HOLD_ON                (2)
#define CHATTERBLK_PILOT_TURN_LEFT             (3)
#define CHATTERBLK_PILOT_TURN_RIGHT            (4)
#define CHATTERBLK_HEROES_ACKNOWLEDGE          (5)
#define CHATTERBLK_TONY_LOUD_CLEAR             (6)
#define CHATTERBLK_RAYMOND_ROGER               (7)
#define CHATTERBLK_TONY_GOTCHA                 (8)
#define CHATTERBLK_RAYMOND_WRONG_WAY           (9)
#define CHATTERBLK_RAYMOND_SMASH              (10)
#define CHATTERBLK_RAYMOND_BEAR_DOWN          (10)
#define CHATTERBLK_RAYMOND_PUSH_IT            (12)
#define CHATTERBLK_RAYMOND_HARDER             (13)
#define CHATTERBLK_RAYMOND_OH_MAN             (14)
#define CHATTERBLK_RAYMOND_RANDOM_PLEAS       (15)
#define CHATTERBLK_RAYMOND_PLEASE             (16)
#define CHATTERBLK_RAYMOND_GET_MOVING         (17)
#define CHATTERBLK_NANCY_TIME_RUNNING_OUT     (18)
#define CHATTERBLK_RAYMOND_RANDOM_YELPS       (19)
#define CHATTERBLK_RAYMOND_OHNO               (20)
#define CHATTERBLK_RAYMOND_OUCH               (21)
#define CHATTERBLK_RAYMOND_YAOW               (22)
#define CHATTERBLK_TURBO                      (23)
#define CHATTERBLK_TONY_WHOA                  (24)
#define CHATTERBLK_TONY_GREAT                 (25)
#define CHATTERBLK_RAYMOND_ONE_MORE_TIME      (26)
#define CHATTERBLK_NANCY_BERATES              (27)
#define CHATTERBLK_NANCY_WRONG_JOB            (28)
#define CHATTERBLK_NANCY_ONE_MORE_TRY         (29)
#define CHATTERBLK_NANCY_MEDIOCRE_DRIVER      (30)
#define CHATTERBLK_TONY_LETS_GO               (31)
#define CHATTERBLK__LIMIT                     (32)

/* ----------------------------------------------------------------------- */

#define MAP_ESC                         (0)

#define MAP_WAIT(D)                     (D)

#define MAP_CMD_GOTO(ADDR)              MAP_ESC, (0), (ADDR) & 0xFF, (ADDR) >> 8
#define MAP_CMD_FORK_END                MAP_ESC, (1)
#define MAP_CMD_STOP_BARRIERS           MAP_ESC, (3)
#define MAP_CMD_START_BARRIERS_L        MAP_ESC, (7)
#define MAP_CMD_START_BARRIERS_R        MAP_ESC, (8)
#define MAP_CMD_START_TWO_BARRIERS      MAP_ESC, (9)
#define MAP_CMD_ARROW_OFF               MAP_ESC, (10)
#define MAP_CMD_ARROW_L                 MAP_ESC, (11)
#define MAP_CMD_ARROW_R                 MAP_ESC, (12)
#define MAP_CMD_START_CARS              MAP_ESC, (13)
#define MAP_CMD_STOP_CARS               MAP_ESC, (14)

// Curvature
#define MAP_CURVE_STRAIGHT(D)           (((D) << 4) | 0)
#define MAP_CURVE_RIGHT(D)              (((D) << 4) | 1)
#define MAP_CURVE_RIGHT_HARD(D)         (((D) << 4) | 2)
#define MAP_CURVE_RIGHT_VERY_HARD(D)    (((D) << 4) | 3)
#define MAP_CURVE_LEFT(D)               (((D) << 4) | 9)
#define MAP_CURVE_LEFT_HARD(D)          (((D) << 4) | 10)
#define MAP_CURVE_LEFT_VERY_HARD(D)     (((D) << 4) | 11)

// Height
#define MAP_HEIGHT_UP1(D)               (((D) << 4) | 1)
#define MAP_HEIGHT_UP3(D)               (((D) << 4) | 3)
#define MAP_HEIGHT_UP5(D)               (((D) << 4) | 5)
#define MAP_HEIGHT_UP7(D)               (((D) << 4) | 7)
#define MAP_HEIGHT_LEVEL(D)             (((D) << 4) | 8)
#define MAP_HEIGHT_DOWN1(D)             (((D) << 4) | 9)
#define MAP_HEIGHT_DOWN3(D)             (((D) << 4) | 11)
#define MAP_HEIGHT_DOWN5(D)             (((D) << 4) | 13)
#define MAP_HEIGHT_DOWN7(D)             (((D) << 4) | 15)

// Lanes
#define MAP_LANES(D,T)                  (D), (T)

#define MAP_LANES_4                     (0x00)
#define MAP_LANES_3L                    (0x81)
#define MAP_LANES_3R                    (0x82)
#define MAP_LANES_2L                    (0x01)
#define MAP_LANES_2M                    (0x02)
#define MAP_LANES_2R                    (0x03)
#define MAP_LANES_4TO3L                 (0xBD)
#define MAP_LANES_4TO3R                 (0x8E)
#define MAP_LANES_3TO4L                 (0xAD)
#define MAP_LANES_3TO4R                 (0x9E)
#define MAP_LANES_3TO2L                 (0x06)
#define MAP_LANES_3TO2R                 (0x0F)
#define MAP_LANES_2TO3L                 (0x2D)
#define MAP_LANES_2TO3R                 (0x1F)
#define MAP_LANES_TUNNEL                (0x45)
#define MAP_LANES_DIRTTRACK             (0xC1)
#define MAP_LANES_FORKED                (0xED)

// Objects (names are valid for Stage 1)
#define MAP_OBJECTS(D,T)                (((D) << 4) | T)

#define MAP_OBJ_NONE                    (0)
#define MAP_OBJ_TUNNEL_LIGHT            (1)
#define MAP_OBJ_UNKNOWN_2               (2)
#define MAP_OBJ_SHORT_POLE              (3)
#define MAP_OBJ_TREE                    (4)
#define MAP_OBJ_BUSH                    (5)
#define MAP_OBJ_STREET_LAMP             (6)
#define MAP_OBJ_TELEGRAPH_POLE          (7)
#define MAP_OBJ_TURN_SIGN_L             (8)
#define MAP_OBJ_TURN_SIGN_R             (9)

/* ----------------------------------------------------------------------- */

typedef struct hazard_s hazard_t;
typedef struct stagevars_s stagevars_t;
typedef struct chqstate_s chqstate_t;
typedef struct scenedata_s scenedata_t;

typedef void (hazard_handler_t)(chqstate_t *state);

/* ----------------------------------------------------------------------- */

typedef struct {
  u8        width;  // pixels
  u8        flags;  // assuming 1=>masked
  u8        height; // pixels
  const u8 *bitmap;
  const u8 *bitmapshifted;
} stretchybitmap_t;

/// Stretchy Set offset
/// (7 is sizeof(stretchybitmap_t)) -- use offsetof ?
#define OFFSET(N) ((N)*7+2)

typedef struct {
  const stretchybitmap_t *bitmaps; // -> array of bitmaps
  struct {
    u8 depth; // something depth-ish
    u8 offset;
  } pairs[10]; // maps depths to offsets
} stretchyset_t;

// root objects (an array of these) used with routine draw_stretchy_object_left/right
// bottom-most object is given first
typedef struct {
  // 1=>end
  // 2=>
  // 3=>repeats?
  // otherwise not sure. the value affects height.
  u8                   n;
  const stretchyset_t *set; // Conv: this is always present, can be NULL for
  // final entry
} stretchy_t;

/* ----------------------------------------------------------------------- */

void chasehq_reset_state(chqstate_t *state);

/* ----------------------------------------------------------------------- */

// TODO: Ideally all of these will become static in the long run.

void load_stage(chqstate_t *state);

void attract_mode(chqstate_t *state);

void start_siren_hook(chqstate_t *state);
void play_engine_or_siren_sfx_hook(chqstate_t *state);
void silence_audio_hook(chqstate_t *state);
void write_audio_registers_hook(chqstate_t *state);
void setup_engine_sfx_hook(chqstate_t *state);
void play_engine_sfx_hook(chqstate_t *state);
void play_speech_hook(chqstate_t *state, u8 A);
void attract_mode_hook(chqstate_t *state);

void main_loop(chqstate_t *state);

void cpu_driver(chqstate_t *state);

void run_pregame_screen(chqstate_t *state);
void reveal_perp_car(chqstate_t *state);
void animate_meters(chqstate_t *state);
void am_set_attrs(int counter, u8 *attrs);
void draw_pregame(chqstate_t *state);

void escape_scene(chqstate_t *state);

void set_up_stage(chqstate_t        *state,
                  const u8          *stage_data,
                  const scenedata_t *scene_data);

void reset_lights(u8 *attrptr);

void check_user_input(chqstate_t *state);
void check_user_input_quit_key(chqstate_t *state);

void clear_playfield_attrs(chqstate_t *state);
void clear_playfield(chqstate_t *state);

void start_sfx(chqstate_t *state, u8 index, u8 priority);
void drive_sfx(chqstate_t *state);
void sfx_crash(chqstate_t *state, u8 param);
void sfx_thud(chqstate_t *state, u8 param);
void sfx_cornering(chqstate_t *state, u8 param, u8 param2);
void sfx_bipbow(chqstate_t *state, u8 param, u8 param2);

int handle_perp_caught(chqstate_t *state);
void hpc_set_perp_speed(chqstate_t *state, u16 speed);

void fully_smashed(chqstate_t *state);

void transition(chqstate_t *state);
void transition_fade_chunk(chqstate_t *state, u8 mask, u8 *screen);

void setup_transition(chqstate_t *state, u8 stride);

void fill_attributes(chqstate_t *state);

void draw_overlay_messages(chqstate_t *state);

const u8 *print_message(chqstate_t *state,
                        u8          style,
                        const u8   *messages);

void setup_overlay_messages(chqstate_t *state, const u8 *message);
void setup_overlay_messages_with_transition(chqstate_t *state,
    u8          transition,
    const u8   *message);

void draw_mugshots(chqstate_t *state);

void draw_mugshot(chqstate_t *state,
                  u16         screenpos,
                  u16         backbuf,
                  const u8   *mugshot);

void draw_smash_bar(chqstate_t *state);
u16 draw_smash_bar_segments(chqstate_t *state, int nsegs, u16 buf);
u16 draw_smash_bar_solid_bit(chqstate_t *state, int nrows, u16 buf);

void draw_everything_else(chqstate_t *state);

void draw_overhead(chqstate_t *state);

void draw_stretchy_object(chqstate_t *state, int left_or_right);

void draw_tunnel_light(chqstate_t *state, int left_or_right);

void draw_object(chqstate_t *state, int left_or_right);

void plot_sprite(chqstate_t *state,
                 u8          width_bytes,
                 u8          height,
                 u8         *backbuf_addr,
                 u16         bitmap_stride,
                 const u8   *bitmap_data);
void plot_sprite_even_entry(chqstate_t *state,
                            int         jump_offset,
                            u8          height,
                            u8         *backbuf_addr,
                            u16         bitmap_stride,
                            const u8   *bitmap_data);
void ps_odd(chqstate_t *state,
            u8          width_bytes,
            u8          height,
            u8         *backbuf_addr,
            u16         bitmap_stride,
            const u8   *bitmap_data);

void plot_sprite_flipped(chqstate_t *state,
                         u8          width_bytes,
                         u8          height,
                         u8         *backbuf_addr,
                         u16         bitmap_stride,
                         const u8   *bitmap_data);

u8 rng(chqstate_t *state);

void start_chatter(chqstate_t       *state,
                   chatterpriority_t priority,
                   const u8         *chatterblk);

void drive_chatter(chqstate_t *state);
void drive_chatter_stop(chqstate_t *state);

void print_chatter(chqstate_t *state);
void pc_chatter_message(chqstate_t *state, const u8 *chatter);
void pc_clear_line(chqstate_t *state, u8 x);

void noise_effect(chqstate_t *state, u8 counter);
void noise_effect_9a5c(chqstate_t *state, u8 counter);
void ne_plot_attrs(chqstate_t *state, u8 attr);

void plot_face(chqstate_t *state,
               u16         screen,
               const u8   *face);
void plot_face_attributes(chqstate_t *state,
                          u16         screen,
                          const u8   *face);

void plot_mini_font_cursor_off(chqstate_t *state,
                               u8          x,
                               char        character);
void plot_mini_font_cursor_on(chqstate_t *state,
                              u8          x,
                              char        character);
void pmf_go(chqstate_t *state,
            u8          x,
            char        ascii,
            u8          extrabm1,
            u8          extrabm2);

void clear_message_line(chqstate_t *state);

void tick(chqstate_t *state);
void play_start_noise(chqstate_t *state);

void speed_score(chqstate_t *state);

void add_bonus(chqstate_t *state, u8 lo, u8 md, u8 hi);
int bonus_digit(u8 digit, u8 *nonzeroflag, char **poutput);

void increment_score(chqstate_t *state, u8 lo, u8 md, u8 hi);

void calc_overtake_bonus(chqstate_t *state);

void update_scoreboard(chqstate_t *state);

void toggle_light_brightness(chqstate_t *state, u8 *attrs);

void plot_turbos_and_scores(chqstate_t *state);
void ptas_led_digits(chqstate_t *state,
                     u8          iterations,
                     const u8   *digits,
                     u8         *stored,
                     u8         *screen);

u8 *ledfont_plot(chqstate_t *state, u8 ord, u8 *screen);

const u8 *draw_string_with_style(chqstate_t *state,
                                 u8          attrval,
                                 u8         *attrs,
                                 u8         *backbuf,
                                 const u8   *string,
                                 u8          style);
const u8 *draw_string_generic(chqstate_t *state,
                              u8          attrval,
                              u8         *attrs,
                              u8         *backbuf,
                              const u8   *string);
const u8 *draw_string_core(chqstate_t *state,
                           u8         *backbuf,
                           const u8   *string,
                           u8          style,
                           u8          attrval,
                           u8          attrsstride,
                           u8         *attrs);

void draw_char(chqstate_t *state,
               u8          character,
               u8         *screen,
               u8          style,
               u8          attrval,
               u8          stride,
               u8         *attrs,
               u8        **new_screen,
               u8        **new_attrs);

u8 keyscan(chqstate_t *state);
u8 keyscan_a112(chqstate_t *state, u8 *HL);
void keyscan_inner(chqstate_t *state, u8 A);

void check_scenery_collisions(chqstate_t *state);

void scenery_hit(chqstate_t *state);

void fork_completed(chqstate_t *state);

void layout_objects(chqstate_t *state);

void cycle_counters(chqstate_t *state);

hazard_handler_t perp_behaviour;

void spawn_cars(chqstate_t *state);

u16 get_spawn_lanes(chqstate_t *state, u8 extra);

hazard_handler_t hazard_handler;

void choose_dirt_and_stones(chqstate_t *state);

void layout_dirt_and_stones(chqstate_t *state);

void dust_stones_stuff(chqstate_t *state);

void draw_helicopter(chqstate_t *state);

void move_helicopter(chqstate_t *state);

void drive_helicopter(chqstate_t *state);

void spawn_hazards(chqstate_t *state);

void check_hazard_collisions(chqstate_t *state);

u8 check_collision(chqstate_t *state, u8 D, hazard_t *IX);

void draw_hazards(chqstate_t *state);

hazard_handler_t no_op;

void move_hero_car(chqstate_t *state);

void animate_hero_car(chqstate_t *state);

void scroll_horizon(chqstate_t *state);

void update_road_level(chqstate_t *state);

void layout_road(chqstate_t *state);

void exit_fork(chqstate_t *state);

void draw_screen(chqstate_t *state);

void clear_playfield_set_attrs(chqstate_t *state);

void read_map(chqstate_t *state);
void rm_cycle_buffer_offset(chqstate_t *state, u8 *pfastcounter);

void prepare_tunnel(chqstate_t *state);

void draw_tunnel(chqstate_t *state, u8 *IY);

void draw_road_scene_change(chqstate_t *state, u8 *IX, u8 *IY);

void draw_road(chqstate_t *state);

void pre_shift_backdrop(chqstate_t *state);

void forked_road_plotter(chqstate_t *state);

void backdrop_fill_choice(chqstate_t *state);

void build_curve_table(chqstate_t *state, int forked);
void build_curve_table_sub_cca8(chqstate_t *state,
                                u8     Bdash_alwayszero,
                                u16   *HLtableend,
                                u16    DEroadpos);

void build_height_table(chqstate_t *state);

typedef int8_t T; // works
T multiply(T a, T c);

void entrypt_48k(chqstate_t *state);
void entrypt_128k(chqstate_t *state);
void entrypt_common(chqstate_t *state, u8 Amode_128k, u8 Bnrelocs);

void menu_draw_char(chqstate_t *state,
                    u8     Achar,
                    u8     Fdash,
                    u8     Cdash,
                    u8    *DEdash,
                    u8    *HLdash,
                    u8   **DEdash_out,
                    u8   **HLdash_out);

void bootstrap(chqstate_t *state);

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_H */

