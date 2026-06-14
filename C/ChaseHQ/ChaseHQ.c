/**
 * ChaseHQ.c
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
 * The original game and design is copyright (c) 1988 Taito Corporation
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited
 * The recreated version is copyright (c) 2023-2026 David Thomas
 */

/* Important Note
 *
 * While this code _looks_ plausible and compiles in its current state know
 * that it's all sorts of broken!
 */

/* Notes
 *
 * Like with my conversion of The Great Escape to C we model the game as if
 * it's still running on a ZX Spectrum, including a Spectrum screen memory
 * layout and IO world. This avoids a full rewrite of the original code and
 * means that we leave some of the Z80-specific micro-optimisations in place.
 * This allows the code to remain a useful basis for comparison and lowers
 * the risk of translation errors. Although it's very tempting to rewrite all
 * the code to be fully idiomatic C the greater the difference from the
 * original disassembly the harder it gets to refer back to it and spot our
 * mistakes. The goal after all is to use this C conversion to expose
 * problem points and feed those back into the disassembly's description.
 *
 * Ideally the ordering of the code will be preserved such that the original
 * game code and this reimplementation have broadly the same structure.
 * Some code will unavoidably need to be changed however, such as the stack
 * trick where PUSH and POP are used to accelerate loads and stores.
 *
 * My original intention was to retain the level data (called "stage" data in
 * this conversion to match the original game) whole in the converted game,
 * including any embedded addresses. I wanted to 'page in' levels by copying
 * the original game data into the game's state structure. This would also
 * mean that any new or adjusted levels produced by means of this conversion
 * would be compatible with the original game. However, it turned out that
 * allowing binary compatibility would have meant duplicating some core
 * functions where the data structures exist in both the engine part and the
 * stage data. So I gave up on that. Having the stage data in C does make it
 * more tweakable, which is good. Long term it would be nice if the stages -
 * at least the map portion - were expressible with a concise text format.
 *
 * Pointers present a problem. The original game data uses 16-bit pointers
 * sometimes embedded in byte data but the converted code could be using 32-
 * or 64-bit ones. Instead of embedding huge pointers we'll either use byte
 * tokens or leave the original values in place and indrect them through new
 * tables or switch lookups. For example see the "chatter" code: the code
 * that prints the messages on-screen as the game runs. It previously
 * embedded addresses inline in chatter structures. These are replaced with
 * single bytes that reference a new tables of pointers.
 *
 * Like with TGE a game state structure is added to encapsulate the complete
 * game state. It is passed to every state-accessing function in the game.
 * Globals are banned.
 *
 * Screen handling in the original game assumes the alignment of the screen
 * and the back buffer. That can't be guaranteed in a portable conversion. We
 * can address this by converting pointers to offsets when we need to perform
 * address arithmetic.
 *
 * (SM) means self modified. There is a _lot_ of self-modified code in the
 * game.
 *
 * Remember that much of this code is in progress and untested - or just
 * broken.
 */

/* TODOs
 * - Copy whole messages that get modified into the state structure.
 * - Decide how to drive the main loop(s).
 * - Promote variables to int from u8/s8/u16/s16 where possible,
 */

#include <assert.h>
#include <stddef.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "C99/Types.h"
#include "ZXSpectrum/Macros.h"
#include "ZXSpectrum/Pixels.h"
#include "ZXSpectrum/Spectrum.h"
#include "ZXSpectrum/Z80.h"

#include "ChaseHQ.h"
#include "Data/ChaseHQ-CommonData.h"
#include "Data/ChaseHQ-SoundSamples.h"
#include "Data/ChaseHQ-Stage1Data.h"
#include "Data/ChaseHQ-Stage2Data.h"
#include "Data/ChaseHQ-Stage3Data.h"
#include "Data/ChaseHQ-Stage4Data.h"
#include "Data/ChaseHQ-Stage5Data.h"
#include "ChaseHQ-Stages.h"
#include "ChaseHQ-State.h"

/* ----------------------------------------------------------------------- */

// Test rigging enable
#define RUN_FULL_GAME 1

/* ----------------------------------------------------------------------- */

/* Z80 ish macros */

/** Return an 8-bit value `v` rotated right by `sh` bits */
#define ROR_8(v,sh) (((v) >> (sh)) | ((v) << (8 - (sh)))

/** Return `t`+`d` but only alter the low byte. */
#define LO_ADD(t,d) ((t) = (((t) & ~0xFF) | (((t) + (d)) & 0xFF)))

/** Add `d` to the high byte of a 16-bit word `v` */
#define HI_ADD(v,d) ((v) += ((d) << 8))

/** Decrement the high byte of a 16-bit word `v` */
#define HI_DEC(v) ((v) -= 0x0100)

/** Return ptr advanced by delta modulo 256, assigning back in-place. */
#define WRAPPING(ptr, delta, base) \
  ((ptr) = &(base)[((ptr) + delta - (base)) & 0xFF])

/** Return ptr advanced by 1 modulo 256. */
#define WRAPPINGINCREMENT(ptr, base) \
  WRAPPING(ptr, 1, base)

/* ----------------------------------------------------------------------- */

/* Screen, attributes and backbuffer macros */

/** Return screen pointer given a Z80 address. */
#define ADDRTOSCREEN(addr) \
  (&state->speccy->screen.pixels[(addr) - SCREEN_START_ADDRESS])

/** Return attributes pointer given a Z80 address. */
#define ADDRTOATTRS(addr) \
  (&state->speccy->screen.attributes[(addr) - SCREEN_ATTRIBUTES_START_ADDRESS])

/** Return backbuffer[] pointer given a Z80 address. */
#define ADDRTOBACKBUF(addr) \
  (&state->backbuffer[(addr) - BACKBUFFER_START_ADDRESS])

/** Return a Z80 address of a screen[] pointer. */
#define SCREENTOADDR(ptr) \
  (SCREEN_START_ADDRESS + SCREENTOOFFSET(ptr))

/** Return a Z80 address of an attributes[] pointer. */
#define ATTRSTOADDR(ptr) \
  (SCREEN_ATTRIBUTES_START_ADDRESS + ATTRSTOOFFSET(ptr))

/** Return a Z80 address of backbuffer[] pointer. */
#define BACKBUFTOADDR(ptr) \
  (BACKBUFFER_START_ADDRESS + BACKBUFTOOFFSET(ptr))

/** Return byte offset of a screen[] pointer. */
#define SCREENTOOFFSET(ptr) \
  ((ptr) - &state->speccy->screen.pixels[0])

/** Return byte offset of an attributes[] pointer. */
#define ATTRSTOOFFSET(ptr) \
  ((ptr) - &state->speccy->screen.attributes[0])

/** Return byte offset of a backbuffer[] pointer. */
#define BACKBUFTOOFFSET(ptr) \
  ((ptr) - &state->backbuffer[0])

/** Return screen[] pointer given byte offset. */
#define OFFSETTOSCREEN(off) \
  (&state->speccy->screen.pixels[off])

/** Return attributes[] pointer given byte offset. */
#define OFFSETTOATTRS(off) \
  (&state->speccy->screen.attributes[off])

/** Return backbuffer[] pointer given byte offset. */
#define OFFSETTOBACKBUF(off) \
  (&state->backbuffer[off])

/** Return if the given pointer is a valid screen attributes pointer. */
#define VALID_ATTRS(ptr) \
  ((ptr) >= &state->speccy->screen.attributes[0] && (ptr) < &state->speccy->screen.attributes[SCREEN_ATTRIBUTES_LENGTH])

/** Return if the given pointer is a valid backbuffer pointer. */
#define VALID_BACKBUF_PTR(ptr) \
  ((ptr) >= &state->backbuffer[0] && (ptr) < &state->backbuffer[BACKBUFFER_LENGTH])

/** Return if the given Z80 address is a valid backbuffer address. */
#define VALID_BACKBUF_ADDR(addr) \
  ((addr) >= BACKBUFFER_START_ADDRESS && (addr) < BACKBUFFER_END_ADDRESS)

/* ----------------------------------------------------------------------- */

/* Road buffer macros */

/** Given a road buffer pointer return a new wrapped-around buffer index. */
#define ROADBUF_PTR2IDX(PTR) \
  (((PTR) - state->road_buffer_start) & 0xFF)

/** Given a road buffer delta return a new wrapped-around buffer index. */
#define ROADBUF_FWD2IDX(N) \
  ROADBUF_PTR2IDX(state->road_buffer_offset + N)

/** Given a road buffer delta return a pointer. */
#define ROADBUF_FWD2PTR(N) \
  (&state->road_buffer_start[ROADBUF_FWD2IDX(N)])

/* ----------------------------------------------------------------------- */

/* Configuration constants */

#define MAXTURBOS                    (3) /* max turbo boosts player starts each stage with */

#define RESTART_TIME_BCD          (0x60) /* seconds in BCD */

#define SPEED_PERP_MIN              (70) /* perp slow-down threshold in handle_perp_caught */
#define SPEED_GEAR_CHANGE          (150) /* gear-change threshold: low gear below, high gear at or above */
#define SPEED_PERP_CHASE           (350) /* perp's base chase speed; also hazard speed cap after impact */
#define SPEED_ATTRACT              (400) /* scripted drive speed: attract mode camera, perp post-arrest */

#define MARQUEELIGHT_WIDTH           (5) /* attribute cells */
#define MARQUEELIGHT_HEIGHT          (4) /* attribute cells */

#define MINSTAGE                     (1)
#define MAXSTAGE                     (5)

#define SMASHCOUNTER_MAX            (20) /* fully smashed; also the smash bar segment count */

/* ----------------------------------------------------------------------- */

/* Memory constants */

#define STAGEDATA_BASE          (0x5C00) /* first byte of paged stage data in Z80 address space */
#define STAGEDATA_END           (0x7FFF) /* last byte of paged stage data, inclusive */
#define STAGEDATA_LENGTH        (STAGEDATA_END + 1 - STAGEDATA_BASE)

#define MARQUEELIGHT_LEFT_ATTR_ADDR  (0x5820) /* screen attribute address of left marquee light */
#define MARQUEELIGHT_RIGHT_ATTR_ADDR (0x583B) /* screen attribute address of right marquee light */

#define BANK3_BOUNCY_LOGO       (0xC000)
#define BANK3_ROUTINE_3         (0xC003) /* bootstrap */
#define BANK3_ROUTINE_6         (0xC006) /* success music */
#define BANK3_INPUT_SELECTION   (0xC009)

/* ----------------------------------------------------------------------- */

/* Enumeration constants */

#define QUITSTATE_IDLE               (0)
#define QUITSTATE_START              (1)
#define QUITSTATE_DONE               (2)

#define EFFECT_SQUEAL                (1)
#define EFFECT_LANDING               (2) /* hero car landing after a jump */
#define EFFECT_CAR_HIT               (3)
#define EFFECT_SCENERY_HIT           (4)
#define EFFECT_HAZARD_HIT            (5)
#define EFFECT_WALL_HIT              (6)
#define EFFECT_CORNERING             (7) /* tyre screech when cornering */
#define EFFECT_BIP                   (8) /* high-pitched countdown beep */
#define EFFECT_BOW                   (9) /* low-pitched countdown beep */

#define TIMEUPSTATE_INIT             (0)
#define TIMEUPSTATE_CHECK_TIME_UP    (1)
#define TIMEUPSTATE_CHECK_CREDITS    (2)
#define TIMEUPSTATE_CHECK_RESTART    (3)
#define TIMEUPSTATE_WAITING          (4) /* game-over countdown running; waiting to expire */

#define CHATTERSTATE_IDLE            (0)
#define CHATTERSTATE_START           (1)
#define CHATTERSTATE_RUN             (2)
#define CHATTERSTATE_STOP            (3)

#define HANDFLAG_NONE                (0) /* no hand visible */
#define HANDFLAG_ANIMATING           (1) /* cherry light animating onto roof */
#define HANDFLAG_STOP                (2) /* static "stop" hand */

#define PERPCAUGHTPHASE_NONE         (0)
#define PERPCAUGHTPHASE_ALIGNING     (1)
#define PERPCAUGHTPHASE_STOPPING     (2)
#define PERPCAUGHTPHASE_STOPPED      (3) /* car has stopped; engine off; smash bar is removed */
#define PERPCAUGHTPHASE_SCORE        (4)
#define PERPCAUGHTPHASE_FADING       (5)
#define PERPCAUGHTPHASE_ADVANCING    (6) /* transition */

/* ----------------------------------------------------------------------- */

/* Flag constants */

#define USERINPUT_RIGHT           (1<<0)
#define USERINPUT_LEFT            (1<<1)
#define USERINPUT_DOWN            (1<<2) /* aka brake */
#define USERINPUT_UP              (1<<3) /* aka accelerate */
#define USERINPUT_FIRE            (1<<4) /* aka gear */
#define USERINPUT_TURBO           (1<<5)
#define USERINPUT_PAUSE           (1<<6)
#define USERINPUT_QUIT            (1<<7)
#define USERINPUT_NOT_QUIT        (0x7F) /* mask of all input bits except QUIT */
#define USERINPUT_NONE            (0x00)

/* ----------------------------------------------------------------------- */

/* Other constants */

#define TRANSITIONSTRIDE_FORWARD     (8) /* screen wipe step: attribute rows per frame, top-to-bottom */
#define TRANSITIONSTRIDE_REVERSE    (-8) /* screen wipe step: attribute rows per frame, bottom-to-top */

// Note: road_pos left..right is high..low
#define ROAD_RIGHTMOST          (0x00F5) /* road_pos value at rightmost road edge */
#define ROAD_LEFTMOST           (0x0105) /* road_pos value at leftmost road edge */
#define ROAD_126                (0x0126) /* road_pos cap applied during car bounce */

#define ROADBUF_CURVATURE_OFFSET     (0)
#define ROADBUF_HEIGHT_OFFSET       (32)
#define ROADBUF_LANES_OFFSET        (64)
#define ROADBUF_RIGHTOBJS_OFFSET    (96)
#define ROADBUF_LEFTOBJS_OFFSET    (128)
#define ROADBUF_HAZARDS_OFFSET     (160)

#define PREGAMECMD_STOP           (0x00)
#define PREGAMECMD_REPEAT         (0x1F) /* repeat previous command */
#define PREGAMECMD_SET_BG_0       (0xD0) /* set background colour; low nibble = index (0xD0..0xDF) */
#define PREGAMECMD_DRAW_BASE      (0xE0) /* draw base sprite */
#define PREGAMECMD_DRAW_HZ        (0xE1) /* draw horizontal element */
#define PREGAMECMD_DRAW_VT        (0xE2) /* draw vertical element */
#define PREGAMECMD_SET_ADDR       (0xF0) /* set draw address; low nibble = index (0xF0..0xFF) */

/* ----------------------------------------------------------------------- */

/* Perspective table stuff */

/** Number of columns in each perspective scaling table (persp_y_scale,
 * persp_x_scale_right, persp_x_delta_left). */
#define PERSP_TABLE_COLS (22)

/** Scale a raw speed counter (multiples of 32) to a persp_y_scale row offset
 * (multiples of PERSP_TABLE_COLS). */
#define COUNTER_TO_PERSP_Y_ROW(x) ((x) - ((x) >> 2) - ((x) >> 4))

/* ----------------------------------------------------------------------- */

/* Read an arbitrary native word */
static u16 wordat(const u8 *addr)
{
  return (addr[0] << 0) | (addr[1] << 8);
}

/* Write an arbitrary native word */
static void setwordat(u8 *addr, int value)
{
  addr[0] = value;
  addr[1] = value >> 8;
}

/* Move to next screen row (downwards)
 *
 * Conv: Extracted to function.
 */
static u16 nextscrrow(int screen)
{
  screen += 256;
  if (((screen >> 8) & 7) == 0) {
    int t = (screen & 0xFF) + 32;
    screen = (screen & 0xFF00) | (t & 0xFF);
    if (t < 0x100) { /* didn't carry */
      t = (screen >> 8) - 8; /* reduce? */
      screen = (t << 8) | (screen & 0xFF);
    }
  }
  return screen;
}

/* Returns the previous row for the back buffer (visually upwards)
 *
 * Back buffer addresses are of the form 0b_1111_LLLL_RRRC_CCCC.
 *
 * Conv: Extracted to function.
 */
static u16 prevbufrow(int backbuf)
{
  int orig;

  assert(VALID_BACKBUF_ADDR(backbuf));

  orig = backbuf;
  backbuf -= 0x0100;
  if ((orig & 0x0F00) == 0) { /* field LLLL was zero on entry */
    backbuf += 0x1000; /* 1110 -> 1111 */
    int t = (backbuf & 0xFF) - 32; /* decrement field RRRC */
    backbuf = (backbuf & 0xFF00) | (t & 0xFF);
    // DPT: Claude deleted this bit here and I don't trust it.
    /* t < 0 means L borrowed; the Z80 8-bit wrap is already captured by (t & 0xFF) above */
  }
  assert(VALID_BACKBUF_ADDR(backbuf));
  return backbuf;
}

/**
 * Given a ZX Chase HQ Z80 map address return the equivalent C conversion pointer.
 *
 * This is for mapping addresses that I've decided to leave in-place for the
 * time being.
 */
static const void *lookup_map_goto(chqstate_t *state, int z80)
{
  static const void *(*const stage_lookup_fns[])(chqstate_t *, u16) = {
    stage1_lookup_map_goto,
    stage2_lookup_map_goto,
    stage3_lookup_map_goto,
    stage4_lookup_map_goto,
    stage5_lookup_map_goto,
  };

  switch (z80) {
  case 0xE2AA: return &perp_escape_curvature[0];
  case 0xE2AF: return &perp_escape_height[0];
  case 0xE2B8: return &fork_hazards[0];
  case 0xE2C0: return &fork_leftrightobjs[0];
  case 0xE2C1: return &fork_leftrightobjs[1];
  case 0xE2C6: return &forked_road_curvature[0];
  case 0xE2C7: return &forked_road_curvature[1];
  case 0xE2CC: return &forked_road_height[0];
  case 0xE2D1: return &forked_road_lanes[0];
  default:
    assert(state->current_stage_number >= 1 &&
           state->current_stage_number < (int)NELEMS(stage_lookup_fns));
    return stage_lookup_fns[state->current_stage_number - 1](state, z80);
  }
}

/* ----------------------------------------------------------------------- */

typedef void dso_callback_t(chqstate_t     *state,
                            int             Bdepth,
                            const bitmap_t *HLbitmap,
                            const u16      *IXxpos,
                            const u8       *IYheight);

typedef void draw_object_entrypt_t(chqstate_t       *state,
                                   int               Acol_offset,
                                   int               Bdepth,
                                   const depthset_t *DEdepthset,
                                   const u16        *IXxpos,
                                   const u8         *IYheight);

/* ----------------------------------------------------------------------- */

static void end_screen(chqstate_t *state);

static void load_stage(chqstate_t *state);

static void setup_engine_sfx_48k(chqstate_t *state);
static void play_engine_sfx_48k(chqstate_t *state);

static void attract_mode_48k(chqstate_t *state);

static void start_siren_hook(chqstate_t *state);
static void play_regular_sfx_hook(chqstate_t *state);
static void silence_audio_hook(chqstate_t *state);
static void write_audio_registers_hook(chqstate_t *state);
static void setup_engine_sfx_hook(chqstate_t *state);
static void play_engine_sfx_hook(chqstate_t *state);
static void play_speech_hook(chqstate_t *state, int Asample);
static void attract_mode_hook(chqstate_t *state);

static void bootstrap(chqstate_t *state);
static void main_loop(chqstate_t *state);

static void cpu_driver(chqstate_t *state);

static void run_pregame_screen(chqstate_t *state);
static int run_pregame_screen_loop(chqstate_t *state);
static void reveal_perp_car(chqstate_t *state);
static void animate_meters(chqstate_t *state);
static void am_set_attrs(int counter, u8 *attrs);
static void draw_pregame(chqstate_t *state);

static void escape_scene(chqstate_t *state);

static void set_up_stage(chqstate_t        *state,
                         const scenedata_t *scene_data);
static void set_up_stage_reset_lights(u8 *attrptr);

static void check_user_input(chqstate_t *state);
static void check_user_input_quit_key(chqstate_t *state);

static void clear_playfield_attrs(chqstate_t *state);
static void clear_playfield(chqstate_t *state);

static void start_sfx(chqstate_t *state, int index, int priority);
static void drive_sfx(chqstate_t *state);
static void sfx_crash(chqstate_t *state, int param1, int param2);
static void sfx_thud(chqstate_t *state, int param1, int param2);
static void sfx_cornering(chqstate_t *state, int param1, int param2);
static void sfx_cornering_loop_outer(chqstate_t *state, int param1, int param2);
static void sfx_bipbow(chqstate_t *state, int param1, int param2);

static int handle_perp_caught(chqstate_t *state);
static void hpc_set_perp_speed(chqstate_t *state, int speed);

static void fully_smashed(chqstate_t *state);

static void transition(chqstate_t *state);
static void transition_fade_chunk(chqstate_t *state, int mask, u8 *backbuf);

static void setup_transition(chqstate_t *state, int stride);

static void fill_attributes(chqstate_t *state);

static void draw_overlay_messages(chqstate_t *state);

static const u8 *print_message(chqstate_t *state,
                               int          style,
                               const u8   *messages);

static void setup_overlay_messages(chqstate_t *state, const u8 *message);
static void setup_overlay_messages_with_transition(chqstate_t *state,
    int          transition,
    const u8   *message);

static void draw_mugshots(chqstate_t *state);

static void draw_mugshot(chqstate_t *state,
                         int         attrs,
                         int         backbuf,
                         const u8   *mugshot);

static void draw_smash_bar(chqstate_t *state);
static u16 draw_smash_bar_segments(chqstate_t *state, int nsegs, int backbuf);
static u16 draw_smash_bar_solid_bit(chqstate_t *state, int nrows, int backbuf);

static void draw_everything_else(chqstate_t *state);

static void draw_overhead(chqstate_t       *state,
                          int                Bparam,
                          int                Cparam,
                          const stretchy_t *DEstretchy,
                          const u16        *IXxpos,
                          const u8         *IYheight);

static void draw_stretchy_object_common(chqstate_t       *state,
                                        int                Bdepth,
                                        const stretchy_t *DEstretchy,
                                        dso_callback_t   *HLcallback,
                                        const u16        *IXxpos,
                                        const u8         *IYheight);

static void draw_tunnel_light_common(chqstate_t            *state,
                                     int                     Bdepth,
                                     const depthset_t      *DEdepthset,
                                     draw_object_entrypt_t *HLcallback,
                                     const u16             *IXxpos,
                                     const u8              *IYheight);

static void draw_object_left_entrypt(chqstate_t       *state,
                                     int                Acol_offset,
                                     int                Bdepth,
                                     const depthset_t *DEdepthset,
                                     const u16        *IXxpos,
                                     const u8         *IYheight);
static void draw_object_left_stretchy_entrypt(chqstate_t     *state,
    int              Bdepth,
    const bitmap_t *HLbitmap,
    const u16      *IXxpos,
    const u8       *IYheight);
static void draw_object_left_helicopter_entrypt(chqstate_t     *state,
    int              Awidth_bytes,
    const bitmap_t *HLbitmap,
    const u8       *IYheight);

static void draw_object_right_entrypt(chqstate_t       *state,
                                      int                Acol_offset,
                                      int                Bdepth,
                                      const depthset_t *DEdepthset,
                                      const u16        *IXxpos,
                                      const u8         *IYheight);
static void draw_object_right_stretchy_entrypt(chqstate_t     *state,
    int              Bdepth,
    const bitmap_t *HLbitmap,
    const u16      *IXxpos,
    const u8       *IYheight);
static void draw_object_right_helicopter_entrypt(chqstate_t     *state,
    int              Awidth_bytes,
    const bitmap_t *HLbitmap,
    const u8       *IYheight);

static void draw_object_perspective_entrypt(chqstate_t     *state,
                                     int              Awidth_bytes,
                                     int              Cpadding,
                                     const bitmap_t *HLbitmap,
                                     const u8       *IYheight);

static void draw_object_common_flipped(chqstate_t     *state,
                                       int              Bheight,
                                       int              Cpadding,
                                       int              Ebitmap_stride,
                                       const bitmap_t *HLbitmap,
                                       int              Adash_width_bytes,
                                       int             Fdash_zero,
                                       int             Fdash_carry,
                                       const u8       *IYheight);

static void draw_object_clipped(chqstate_t     *state,
                                    int             zero_flipped,
                                    int             carry_masked,
                                    int              Awidth_bytes,
                                    int              Bheight,
                                    int              Cpadding,
                                    int              Ebitmap_stride,
                                    const bitmap_t *HLbitmap,
                                    const u8       *IYheight);

static void plot_sprite(chqstate_t *state,
                        int          width_bytes,
                        u8         *backbuf_addr,
                        int          height,
                        int         bitmap_stride,
                        const u8   *bitmap_data);
static void plot_sprite_even(chqstate_t *state,
                             int         jump_offset,
                             u8         *backbuf_addr,
                             int          height,
                             int         bitmap_stride,
                             const u8   *bitmap_data);
static void plot_sprite_odd(chqstate_t *state,
                            int          width_bytes,
                            u8         *backbuf_addr,
                            int          height,
                            int         bitmap_stride,
                            const u8   *bitmap_data);
static void plot_sprite_odd_entry(chqstate_t *state,
                                  int         jump_offset,
                                  u8         *backbuf_addr,
                                  int          height,
                                  int         bitmap_stride,
                                  const u8   *bitmap_data);

static void plot_sprite_flipped(chqstate_t *state,
                                int          width_bytes,
                                u8         *backbuf_addr,
                                int          height,
                                int          bitmap_stride,
                                const u8   *bitmap_data);
static void plot_sprite_flipped_even(chqstate_t *state,
                                     int         jump_offset,
                                     const u8   *flip_table,
                                     u8         *backbuf_addr,
                                     int          height,
                                     int         bitmap_stride,
                                     const u8   *bitmap_data);
static void plot_sprite_flipped_odd(chqstate_t *state,
                                    int          width_bytes,
                                    u8         *backbuf_addr,
                                    int          height,
                                    int          bitmap_stride,
                                    const u8   *bitmap_data);

static u8 rng(chqstate_t *state);

static void start_chatter(chqstate_t       *state,
                          chatterpriority_t priority,
                          const u8         *chatterblk);

static void drive_chatter(chqstate_t *state);
static void drive_chatter_stop(chqstate_t *state);

static void print_chatter(chqstate_t *state);
static void pc_chatter_message(chqstate_t *state, const u8 *chatterblk);
static void pc_clear_line(chqstate_t *state, int x);

static void drive_noise_effect(chqstate_t *state, int counter);
static void draw_noise_effect(chqstate_t *state, int counter);
static void ne_plot_attrs(chqstate_t *state, int attr);

static void plot_face(chqstate_t *state,
                      int         screen,
                      const u8   *face);
static void plot_face_attributes(chqstate_t *state,
                                 int         screen,
                                 const u8   *face);

static void plot_mini_font_cursor_off(chqstate_t *state,
                                      int          x,
                                      char        character);
static void plot_mini_font_cursor_on(chqstate_t *state,
                                     int          x,
                                     char        character);
static void pmf_go(chqstate_t *state,
                   int          x,
                   char        ascii,
                   int          extrabm1,
                   int          extrabm2);

static void clear_message_line(chqstate_t *state);

static void check_time_up(chqstate_t *state);
static void play_start_noise(chqstate_t *state);

static void speed_score(chqstate_t *state);

static void add_bonus(chqstate_t *state, int lo, int md, int hi);
static int bonus_digit(int digit, int *zeroflag, char **poutput);

static void increment_score(chqstate_t *state, int lo, int md, int hi);

static void calc_overtake_bonus(chqstate_t *state);

static void update_scoreboard(chqstate_t *state);

static void toggle_light_brightness(chqstate_t *state, u8 *attrs);

static void plot_turbos_and_digits(chqstate_t *state);
static void ptas_led_digits(chqstate_t *state,
                            int          iterations,
                            const u8   *digits,
                            u8         *stored,
                            u8         *screen);

static u8 *ledfont_plot(chqstate_t *state, int ord, u8 *screen);

static const u8 *draw_string_with_style(chqstate_t *state,
                                        int          attrval,
                                        u8         *attrs,
                                        u8         *backbuf,
                                        const u8   *string,
                                        int          style);
static const u8 *draw_string_generic(chqstate_t *state,
                                     int          attrval,
                                     u8         *attrs,
                                     u8         *backbuf,
                                     const u8   *string);
static const u8 *draw_string_core(chqstate_t *state,
                                  u8         *backbuf,
                                  const u8   *string,
                                  int          style,
                                  int          attrval,
                                  int          attrsstride,
                                  u8         *attrs);

static void draw_char(chqstate_t *state,
                      int          character,
                      u8         *screen,
                      int          style,
                      int          attrval,
                      int          attrstride,
                      u8         *attrs,
                      u8        **new_screen,
                      u8        **new_attrs);

static u8 keyscan(chqstate_t *state);
static u8 keyscan_keydefs(chqstate_t *state, const u8 *HLkeydefs, int Eresult);
static int keyscan_inner(const chqstate_t *state, int Ainput);

static void check_scenery_collisions(chqstate_t *state);
static void csc_hit_scenery(chqstate_t *state, int Aflip, int Adash);

static void scenery_hit(chqstate_t *state, int Aflip, int Adash);

static void check_fork_scenery_collisions(chqstate_t *state, int DEdash,
    int HLdash);

static void layout_objects(chqstate_t *state);

static void cycle_counters(chqstate_t *state);

static void spawn_cars(chqstate_t *state);

static u16 get_spawn_lanes(chqstate_t *state, int extra);

static void choose_dirt_and_stones(chqstate_t *state);

static void layout_dirt_and_stones(chqstate_t *state);

static void dust_stones_stuff(chqstate_t *state, int Biterations,
                              const u8 *IYheight);

static void draw_helicopter(chqstate_t *state, int Biterations, u8 *IYheight);
static void draw_helicoper_part(chqstate_t                *state,
                                int                         A,
                                const heli_bitmap_inner_t *DEinnerbitmap,
                                const u8                  *IYheight);

static void move_helicopter(chqstate_t *state);

static void drive_helicopter(chqstate_t *state);

static void spawn_hazards(chqstate_t *state);
static int sh_find_free(chqstate_t *state,
                        int          Bhorz_pos,
                        int          Cdistance,
                        int         DEhittable_offset);

static hazard_handler_t hazard_hit;

static void check_hazard_collisions(chqstate_t *state);

static u8 check_collision(chqstate_t *state, int default_retval, int HL,
                          hazard_t *hazard, u16 *HLout);

static void draw_all_hazards(chqstate_t *state);
static void dh_draw_one_hazard(chqstate_t *state,
                               hazard_t   *IXhazard,
                               const u8   *IYbase);
static void draw_arrow_fire_smoke(chqstate_t *state,
                                  int          Biterations,
                                  const u8   *IYheight);
static void dh_smoke(chqstate_t *state, u8 *HLsmoke, const u8 *IYheight);
static void dh_draw(chqstate_t     *state,
                    int              Bx,
                    int              Cy,
                    int             DEoffset,
                    const bitmap_t *HLbitmaps,
                    const u8       *IYheight);
static void dh_draw_bitmap(chqstate_t     *state,
                           int              Bx,
                           int              Cy,
                           const bitmap_t *HLbitmap,
                           const u8       *IYheight);

static void move_hero_car(chqstate_t *state);

static void animate_hero_car(chqstate_t *state);
static void ahc_check_hand_flag(chqstate_t *state);

static void start_chase(chqstate_t *state);

static void smash(chqstate_t *state);

static void draw_debris(chqstate_t *state);

static void draw_hero_car(chqstate_t *state, int Aturn_speed, int Bwobble);

static const carpart_t *draw_hero_car_part(chqstate_t      *state,
    int               Cwidth_bytes,
    int               Dy,
    int               Ex,
    const carpart_t *HLpart);

static void draw_smoke(chqstate_t *state, int Aanim_frame, int Adash_flip_flag);

static void draw_cherry_light(chqstate_t *state, int Aframe_index,
                              int Bturn_limit, int Cturn_delta);
static void draw_crash_unflipped(chqstate_t *state, int Aframe_index);

static void draw_crash(chqstate_t *state, int Aframe_index, int Bdash_flip_flag,
                       int Cdash);

static void draw_part(chqstate_t *state,
                      int          height,
                      int          width,
                      int          y,
                      int          x,
                      const u8   *bitmap,
                      int          Bdash_flip_flag,
                      int          Cdash,
                      int          Edash_bitmap_stride);
static void draw_part_entry2(chqstate_t *state,
                             int          Bheight,
                             int          Cwidth_bytes,
                             int          Dy,
                             int          Ex,
                             const u8   *HLbitmap_data,
                             int          Bdash_flip_flag,
                             int          Cdash,
                             int          Edash_bitmap_stride);
static void draw_part_plot_masked_sprite(chqstate_t *state,
    int          Awidth_bytes,
    u8         *HLbackbuf_addr,
    int          Bdash_height,
    int          Edash_bitmap_stride,
    const u8   *HLdash_bitmap_data);

static void plot_masked_sprite(chqstate_t *state,
                               int         jump_offset,
                               int          height,
                               int         bitmap_stride,
                               const u8   *bitmap_data,
                               u8         *backbuf_addr);

static void plot_masked_sprite_flipped(chqstate_t *state,
                                       int          width_bytes,
                                       u8         *backbuf_addr,
                                       int          height,
                                       int         bitmap_stride,
                                       const u8   *bitmap_data);
static void plot_masked_sprite_flipped_entry2(chqstate_t *state,
    int          width_bytes,
    u8         *backbuf_addr,
    int          height,
    int         bitmap_stride,
    const u8   *bitmap_data);

static void plot_masked_sprite_inverted(chqstate_t *state,
                                        int          Awidth_bytes,
                                        u8         *HLbackbuf_addr,
                                        int          Bdash_height,
                                        int         Edash_bitmap_stride,
                                        const u8   *HLdash_bitmap_data);

static void scroll_horizon(chqstate_t *state);

static void update_road_level(chqstate_t *state);

static void layout_road(chqstate_t *state);

static void exit_fork(chqstate_t *state);

static void update_screen(chqstate_t *state);

static void clear_playfield_set_attrs(chqstate_t *state);

static void read_map(chqstate_t *state);
static void rm_cycle_buffer_offset(chqstate_t *state, u8 *pfastcounter);

static void prepare_tunnel(chqstate_t *state);

static void draw_tunnel(chqstate_t *state, u8 *IYheight);

static void draw_road_scene_change(chqstate_t *state, u8 *IXlanes,
                                   u8 *IYheight);

static void draw_road(chqstate_t *state);
static void dr_read_lanes(chqstate_t *state, u8 *IXlanesptr, u8 *IYheightptr,
                          int Bfill_pattern, int Chorizon, int DEbackbuf, int Lrow);
static void dr_four_lane_highway(chqstate_t *state, int Bfill_pattern,
                                 int Chorizon, int DEbackbuf, int Lrow);
static void dr_set_lane_callback(chqstate_t *state, int Bfill_pattern,
                                 int Chorizon, int DEbackbuf, int Lrow,
                                 dr_callback_t *HLdash_callback);
static void dr_dispatch_fill(chqstate_t *state, int Bfill_pattern, int Chorizon,
                             int DEbackbuf, int Lrow);
static void dr_advance_unfilled(chqstate_t *state, int DEbackbuf,
                                  int Lrow, int Adash_fill);
static void dr_write_scanline_unfilled(chqstate_t *state, int DEbackbuf);
static void dr_rollover_filled(chqstate_t *state, int DEbackbuf,
                                   int Lrow, int Adash_fill);
static void dr_rollover_unfilled(chqstate_t *state, int DEbackbuf);
static void dr_enter_filled(chqstate_t *state, int DEbackbuf,
                                int Lrow, int Adash_fill);
static void dr_advance_filled(chqstate_t *state, int DEbackbuf, int Lrow,
                    int Adash_fill);
static void dr_fill(chqstate_t *state, int DEbackbuf, int Lrow,
                    int Adash_fill);
static void dr_fill_left_stripe(chqstate_t *state, u8 *SPoutput, int jump_index,
                    int DEbackbuf, int HLfill);

static void pre_shift_backdrop(chqstate_t *state);

static void forked_road_plotter(chqstate_t *state, u8 *IXlanes, u8 *IYheight);

static void backdrop_fill_choice(chqstate_t *state);

static void build_curve_table(chqstate_t *state, int forked);
static void build_curve_table_fill(chqstate_t *state,
                                       int          Bdash_alwayszero,
                                       u16        *HLtableend,
                                       int         DEroadpos);

static void build_height_table(chqstate_t *state);

static int8_t multiply(int8_t a, int8_t c);

static void entrypt_48k(chqstate_t *state);
static void entrypt_128k(chqstate_t *state);
static void entrypt_common(chqstate_t *state, int Amode_128k, int Bnrelocs);

void stop_the_tape_48k(chqstate_t *state);

void menu_draw_strings(chqstate_t *state, const u8 *strings);
const u8 *menu_draw_string(chqstate_t *state, const u8 *HLstring);
static void menu_draw_char(int   Achar,
                           int   Fdash,
                           int   Cdash,
                           u8  *DEdash,
                           u8  *HLdash,
                           u8 **DEdash_out,
                           u8 **HLdash_out);

static void clear_screen(chqstate_t *state);

static void redefine_keys_48k(chqstate_t *state);
static u8 keyscan_all(chqstate_t *state, u8 *Dkeydef_out);
static void define_a_key(chqstate_t *state, int Bindex, int Cindex,
                         int DEscreen);
static u16 dak_move_down(int DEscreen);

static void setup_interrupts(chqstate_t *state);
static void reset_music(chqstate_t *state);
static void next_pattern(chqstate_t *state);
static void next_pattern_at_addr(chqstate_t *state, const u8 *HLpataddr);
static void play_music_48k(chqstate_t *state);
static void pm_wait_for_interrupt(chqstate_t *state);
static void interrupt_entry(chqstate_t *state);
static void playdrum_2(chqstate_t *state, int Aspeed);
static void playdrum_1(chqstate_t *state, int Aspeed);
static void playdrum_start(chqstate_t *state, int Aspeed, int Dlength,
                           const u8 *HLdata);
static void playdrum_bank_go(chqstate_t *state, int Ddash_length,
                             const u8 *HLdash_data);
static void playdrum_go(chqstate_t *state, int Dlength, const u8 *HLdata);
static void noise(chqstate_t *state, int Aparam);

static void load_stage_128k(chqstate_t *state);
static void start_siren_128k(chqstate_t *state);
static void play_siren_sfx_128k(chqstate_t *state);
static void silence_audio_128k(chqstate_t *state);
static void write_audio_registers_128k(chqstate_t *state);
static void engine_sfx_from_speed_128k(chqstate_t *state);
static void setup_turbo_sfx_128k(chqstate_t *state);
static void play_turbo_sfx_128k(chqstate_t *state);
static void play_speech_128k(chqstate_t *state, int index);
static void handle_perp_caught_128k(chqstate_t *state);
static u8 call_bank_3_128k(chqstate_t *state, int HLroutine);
static void page_128k(chqstate_t *state);
static void reset_paging_128k(chqstate_t *state);
static void attract_mode_128k(chqstate_t *state);

/* ----------------------------------------------------------------------- */

/**
 * $5C00: End screen
 *
 * \param[in] state Pointer to game state.
 */
static void end_screen(chqstate_t *state)
{
  // TODO: Write this.
}

/**
 * $8014: Load stage
 *
 * \param[in] state Pointer to game state.
 */
static void load_stage(chqstate_t *state)
{
  int wanted; /* was A */

  // Return if the stage is already loaded
  wanted = state->wanted_stage_number;
  if (wanted == state->current_stage_number)
    return;

  state->current_stage_number = wanted;

  state->stage = stages[wanted];
}

// $80B9 tape_load_to_5c00

// $81DD start_stage_chatter - was hoisted

/**
 * $8204: Setup engine SFX 48K
 *
 * \param[in] state Pointer to game state.
 */
static void setup_engine_sfx_48k(chqstate_t *state)
{
  int nloops;    /* was L */
  int off_cycle; /* was H */

  nloops = ((~(state->speed >> 1)) >> 2) | 1;
  off_cycle = 3;

  if (state->gear == 0)
    nloops >>= 1;

  if (state->tunnel_sfx)
    off_cycle = 1;

  state->engine_sfx_nloops    = nloops;
  state->engine_sfx_off_cycle = off_cycle;
  state->engine_sfx_on_cycle  = 5 - off_cycle;

  play_engine_sfx_48k(state);
}

/**
 * $8234: Play engine SFX 48K
 *
 * \param[in] state Pointer to game state.
 */
static void play_engine_sfx_48k(chqstate_t *state)
{
  int phase;   /* was A */
  int counter; /* was A */
  int nloops;  /* was C */
  int c;       /* was B */

  phase = state->perp_caught_phase;
  if (phase >= PERPCAUGHTPHASE_STOPPED)
    return;

  counter = (state->engine_sfx_counter + 1) & 3;
  state->engine_sfx_counter = counter;
  if (counter)
    return;

  nloops = state->engine_sfx_nloops;
  {
    // OUT $(FE),0 // output zero
    c = state->engine_sfx_off_cycle;
    do {/*idle*/} while (--c);
    // OUT $(FE),24 // output EAR+MIC
    c = state->engine_sfx_on_cycle;
    do {/*idle*/} while (--c);
  } while (--nloops > 0);
}

/**
 * $8258: Attract mode 48K
 *
 * \param[in] state Pointer to game state.
 */
static void attract_mode_48k(chqstate_t *state)
{
  static void *am_draw_saved = NULL;
  int          carry = 0;
  int          blinker;         /* was $828C (SM) */
  int          keys;            /* was A */
  const u8    *messages;        /* was HL */
  int          nmessages;       /* was B */
  u8           attract_blinker; /* was A */
  int          style;           /* was A */

#define CHKDRAW_AM(tag) do { \
  void *d = (void *)state->speccy->draw; \
  if (am_draw_saved == NULL) am_draw_saved = d; \
  else if (d != am_draw_saved) { fprintf(stderr, "DRAW CORRUPT (attract_mode_48k) after %s: %p\n", tag, d); abort(); } \
} while(0)

  set_up_stage(state, &state->stage->attract_data);
  blinker = 0;
  state->speed = SPEED_ATTRACT;
  for (;;) {
    keys = keyscan(state);
    CHKDRAW_AM("keyscan");
    if (keys == USERINPUT_FIRE)
      return;

    cpu_driver(state);
    CHKDRAW_AM("cpu_driver");

    messages  = &attract_messages[0];
    nmessages = 1;
    attract_blinker = state->attract_blinker;
    RRC(attract_blinker);
    state->attract_blinker = attract_blinker;
    if (!carry)
      nmessages++;

    // Display 'nmessages' messages
    do {
      style = *messages;
      messages = print_message(state, style, messages);
    } while (--nmessages > 0);
    CHKDRAW_AM("print_message");

    if (state->transition_control == TRANSITIONCONTROL_STOP) {
      // Alternate between credits and copyright messages
      blinker ^= 1;
      messages = &credits_messages[0];
      if (blinker)
        messages = &copyright_messages[0];
      setup_overlay_messages(state, messages);
      CHKDRAW_AM("setup_overlay_messages");
    }

    transition(state);
    CHKDRAW_AM("transition");
    update_screen(state);
    CHKDRAW_AM("update_screen");
  }
#undef CHKDRAW_AM
}

/**
 * $83B5: Start siren hook
 *
 * \param[in] state Pointer to game state.
 */
static void start_siren_hook(chqstate_t *state)
{
  if (state->mode_128k)
    play_siren_sfx_128k(state);
  else
    start_siren_128k(state);
}

/**
 * $83B8: Play engine or siren SFX hook
 *
 * \param[in] state Pointer to game state.
 */
static void play_regular_sfx_hook(chqstate_t *state)
{
  if (state->mode_128k)
    play_siren_sfx_128k(state);
  else
    play_engine_sfx_48k(state);
}

/**
 * $83BB: Silence audio hook
 *
 * \param[in] state Pointer to game state.
 */
static void silence_audio_hook(chqstate_t *state)
{
  if (state->mode_128k)
    silence_audio_128k(state);
  else
    ; // NOP
  }

/**
 * $83BE: Write audio registers hook
 *
 * \param[in] state Pointer to game state.
 */
static void write_audio_registers_hook(chqstate_t *state)
{
  if (state->mode_128k)
    write_audio_registers_128k(state);
  else
    ; // NOP
}

/**
 * $83C1: Setup engine SFX hook
 *
 * \param[in] state Pointer to game state.
 */
static void setup_engine_sfx_hook(chqstate_t *state)
{
  if (state->mode_128k)
    setup_turbo_sfx_128k(state);
  else
    ; // NOP
}

/**
 * $83C4: Play engine SFX hook
 *
 * \param[in] state Pointer to game state.
 */
static void play_engine_sfx_hook(chqstate_t *state)
{
  if (state->mode_128k)
    play_turbo_sfx_128k(state);
  else
    setup_engine_sfx_48k(state);
}

/**
 * $83C7: Play speech hook
 *
 * \param[in] state  Pointer to game state.
 * \param[in] sample Index of sample to play. (was A)
 */
static void play_speech_hook(chqstate_t *state, int sample)
{
  if (state->mode_128k)
    play_speech_128k(state, sample);
  else
    ; // NOP
}

/**
 * $83CA: Attract mode hook
 *
 * \param[in] state Pointer to game state.
 */
static void attract_mode_hook(chqstate_t *state)
{
  if (state->mode_128k)
    attract_mode_128k(state);
  else
    attract_mode_48k(state);
}

/**
 * $83CD: Bootstrap
 *
 * \param[in] state Pointer to game state.
 */
static void bootstrap(chqstate_t *state)
{
  // Bootstrap is itself a loop
  for (;;) {
    int  carry = 0;
    u8  *HLflipped;   /* was HL */
    int  Biterations; /* was B */
    u8   Aindex;      /* was A */
    u8   Cresult;     /* was C */

    /* Build a table of flipped bytes at "$EF00" */
    Cresult = 0; // Conv: Original didn't initialise C
    HLflipped = &state->flipped[0];
    do {
      Biterations = 8;
      Aindex = HLflipped - &state->flipped[0];
      do {
        RLC(Aindex);
        RR(Cresult);
      } while (--Biterations > 0);
      *HLflipped++ = Cresult;
    } while (HLflipped < &state->flipped[256]);

    // Start attract mode.
    attract_mode_hook(state);

    assert(0); // shouldn't get here for now

    // When attract mode yields then we set up the game.
    state->overtake_bonus_bcd = 0;

    // Clear score_bcd and retry_count.
    memset(&state->score_bcd[0], 0, sizeof(state->score_bcd));
    state->retry_count = 0;

    // Reset wanted_stage_number and credits.
    state->wanted_stage_number = 1;
    state->credits = 2;
    main_loop(state);

    // Call the 128K/bank 3 ?bootstrap routine.
    if (state->mode_128k)
      call_bank_3_128k(state, BANK3_ROUTINE_3);
  }
}

/**
 * $8401: Main loop
 *
 * \param[in] state Pointer to game state.
 */
static void main_loop(chqstate_t *state)
{
  int  carry = 0;
  int  start_speech_index; /* was A */
  u8   keys;               /* was A */
  u8  *pstart_speech;      /* was HL */
  int  quit_state;         /* was A */
  int  start_speech;       /* was A */

  for (;;) {
    load_stage(state);

    if (state->wanted_stage_number == 6) {
      end_screen(state);
      state->wanted_stage_number = 1;
      load_stage(state);
      state->wanted_stage_number = 6; // not sure why
      return;
    }

    run_pregame_screen(state);
    while (run_pregame_screen_loop(state)) /* Conv: Split out */
      ;

    set_up_stage(state, &state->stage->stage_data);

    // Cycle start_speech_cycle 3,2,1 then repeat
    start_speech_index = state->start_speech_cycle - 1;
    if (start_speech_index == 0)
      start_speech_index = 3;
    state->start_speech_cycle = start_speech_index;

    // Choose the startup speech sample
    state->start_speech = (start_speech_index * 4) | 2;
    state->hazards[0].used = HAZARD_USED; // keep perp spawned
    if (state->mode_128k == 0)
      start_chatter(state, 0xFF, chatterblk_start_stage);

    for (;;) {
      state->speccy->stamp(state->speccy);
      drive_sfx(state);
      keyscan(state);
      check_time_up(state);
      check_user_input(state);
      read_map(state);
      if (handle_perp_caught(state)) {
        state->speccy->sleep(state->speccy, 0); // balance the stamp above
        break; // Conv: Original would POP and goto main_loop to cause a restart
      }
      move_hero_car(state);
      spawn_cars(state);
      cycle_counters(state);
      play_regular_sfx_hook(state);
      build_height_table(state);
      scroll_horizon(state);
      play_regular_sfx_hook(state);
      layout_road(state);
      play_regular_sfx_hook(state);
      draw_road(state);
      play_regular_sfx_hook(state);
      layout_objects(state);
      prepare_tunnel(state);
      spawn_hazards(state);
      drive_helicopter(state);
      choose_dirt_and_stones(state);
      play_regular_sfx_hook(state);
      draw_all_hazards(state);
      layout_dirt_and_stones(state);
      play_regular_sfx_hook(state);
      move_helicopter(state);
      check_scenery_collisions(state);
      play_regular_sfx_hook(state);
      draw_everything_else(state);
      play_regular_sfx_hook(state);
      animate_hero_car(state);
      speed_score(state);
      update_scoreboard(state);
      calc_overtake_bonus(state);
      play_regular_sfx_hook(state);
      drive_chatter(state);
      draw_smash_bar(state);
      transition(state);
      play_regular_sfx_hook(state);
      update_screen(state);
      exit_fork(state);
      state->speccy->sleep(state->speccy, 100000); // guess

      if (state->test_mode) {
        keys = ~state->speccy->in(state->speccy, port_KEYBOARD_12345) & 0x1F;
        if (keys) {
          start_sfx(state, EFFECT_BIP, 4); /* priority 4 */
          silence_audio_hook(state);

          RR(keys); // Is bit 0 set? (key 1 to restart the level)
          if (carry)
            break;

          RR(keys); // Is bit 1 set? (key 2 to load the next level)
          if (carry) {
            state->wanted_stage_number++;
            break;
          }

          RR(keys); // Is bit 2 set? (key 3 to load the end screen)
          if (carry) {
            state->wanted_stage_number = 6; // stage 6
            break;
          }

          if (state->credits < 9) // Increment credits unless maxed out at 9
            state->credits++;
        }
      }

      if (state->transition_control == TRANSITIONCONTROL_STOP) {
        // Play speech when we see a 1-bit shift out of start_speech.
        pstart_speech = &state->start_speech;
        SRL(*pstart_speech);
        if (carry) {
          start_speech = *pstart_speech;
          *pstart_speech = 0;
          play_speech_hook(state, start_speech);
        } else {
          quit_state = state->quit_state;
          if (quit_state > 0) {
            // Quitting the game is in progress.
            if (quit_state == QUITSTATE_START) {
              escape_scene(state); /* exit via */
              return;
            }

            state->quit_state = QUITSTATE_DONE;
            setup_transition(state, TRANSITIONSTRIDE_FORWARD);
          }
        }
      }
      // break; //  temporary
    }
    //break; // temporary
  }
}

/**
 * $852A: CPU driver
 *
 * \param[in] state Pointer to game state.
 */
static void cpu_driver(chqstate_t *state)
{
  static void *draw_saved = NULL;
#define CHKDRAW(tag) do { \
  void *d = (void *)state->speccy->draw; \
  if (draw_saved == NULL) draw_saved = d; \
  if (d != draw_saved) { fprintf(stderr, "DRAW CORRUPT after %s: %p\n", tag, d); abort(); } \
} while(0)

  int roadpos; /* was HL */
  int input;   /* was A */

  if (state->host_quit)
    longjmp(state->host_quit_jmp, 1);

  CHKDRAW("entry");

  roadpos = state->scenedata.road_pos;
  input = USERINPUT_UP | USERINPUT_RIGHT;
  if (roadpos < ROAD_LEFTMOST) {
    input = USERINPUT_UP | USERINPUT_LEFT;
    if (roadpos >= ROAD_RIGHTMOST)
      input = USERINPUT_UP;
  }

  if (state->gear != (state->speed < SPEED_GEAR_CHANGE))
    input |= USERINPUT_FIRE;

  state->user_input = input;

  state->speccy->stamp(state->speccy);

  read_map(state);
  CHKDRAW("read_map");
  spawn_cars(state);
  CHKDRAW("spawn_cars");
  cycle_counters(state);
  CHKDRAW("cycle_counters");
  build_height_table(state);
  CHKDRAW("build_height_table");
  scroll_horizon(state);
  CHKDRAW("scroll_horizon");
  layout_road(state);
  CHKDRAW("layout_road");
  draw_road(state);
  CHKDRAW("draw_road");
  layout_objects(state);
  CHKDRAW("layout_objects");
  prepare_tunnel(state);
  CHKDRAW("prepare_tunnel");
  spawn_hazards(state);
  CHKDRAW("spawn_hazards");
  choose_dirt_and_stones(state);
  CHKDRAW("choose_dirt_and_stones");
  layout_dirt_and_stones(state);
  CHKDRAW("layout_dirt_and_stones");
  draw_all_hazards(state);
  CHKDRAW("draw_all_hazards");
  move_hero_car(state);
  CHKDRAW("move_hero_car");
  check_scenery_collisions(state);
  CHKDRAW("check_scenery_collisions");
  //draw_everything_else(state);
  animate_hero_car(state); /* exit via */
  CHKDRAW("animate_hero_car");

  state->speccy->sleep(state->speccy, 150000); // guess

#undef CHKDRAW
}

/**
 * $858C: Pre-game radio screen ("CHASE HQ MONITORING SYSTEM")
 *
 * \param[in] state Pointer to game state.
 */
static void run_pregame_screen(chqstate_t *state)
{
  set_up_stage(state, &state->stage->stage_data);

  state->dont_draw_screen_attrs = 1; // Conv: Was 0xF8.
  setup_transition(state, TRANSITIONSTRIDE_REVERSE);
  clear_playfield_set_attrs(state);
  // Reset the counter in #R$85E4 that reveals the perp's car
  state->pregame_car_revealed_height = 0;
  start_chatter(state, 0xFF, state->stage->addrof_perp_description);

  // Conv: Pregame loop extracted (below)

  // Conv: Dead code removed
}

static void test_car_anim(chqstate_t *state)
{
  memset(&state->speccy->screen.attributes[256],
         attribute_BRIGHT_BLACK_OVER_GREEN, 512);
  memset(&state->backbuffer[0], 0, 4096);

  static int turnitude = 0;
  static int wobble    = 0;
  static int pitch     = 0;
  static int frame     = 0;
  static int y         = 0;
  static int yi        = 0;
  static int boosting  = 0;

  int do_turn  = (rand() % 100) < 20;
  int turn_rt  = (rand() % 100) < 50;
  int ch_ptch  = (rand() % 100) < 20;
  int go_up    = (rand() % 100) < 50;
  int st_jump  = (rand() % 100) < 1;
  int st_boost = (rand() % 100) < 10;

  if (do_turn) {
    if (turn_rt) {
      if (++turnitude > 2) turnitude = 2;
    } else {
      if (--turnitude < -2) turnitude = -2;
    }
  }

  if (ch_ptch) {
    if (go_up) {
      pitch = (pitch == 0) ? 3 : (pitch == 6) ? 0 : pitch;
    } else {
      pitch = (pitch == 0) ? 6 : (pitch == 3) ? 0 : pitch;
    }
  }

  if (st_jump && yi == 0) {
    yi = 5;
  } else {
    if (y >= 25)
      yi = -9;
  }

  y += yi;
  if (y < 0) {
    y = 0;
    yi = 0;
  }

  if (st_boost && boosting == 0) {
    boosting = 20;
  }

  state->car_y      = 0;
  state->flip_car   = (turnitude < 0) ? 1 : 0;
  state->dhc_pitch  = pitch;
  state->dhc_jump_y = y;

  draw_hero_car(state, abs(turnitude), wobble);

  state->mhc_y_offset = 0;

  if (y == 0 && boosting) {
    draw_smoke(state, frame % 4, 0); // right hand
    draw_smoke(state, frame % 4, 1); // left hand
    boosting--;
  }

  frame++;
}

/**
 * $85A8: Run pregame screen loop
 *
 * \param[in] state Pointer to game state.
 * \return Non-zero on success.
 */
static int run_pregame_screen_loop(chqstate_t *state)
{
  int rc = 1; // loop

  state->speccy->stamp(state->speccy);

  draw_pregame(state);
  drive_chatter(state);
  if (!RUN_FULL_GAME) {
    // forcing the pregame screen loop to do other stuff for now
    update_scoreboard(state);
    test_car_anim(state);
  } else {
    reveal_perp_car(state);
    animate_meters(state);
  }
  transition(state);
  update_screen(state);
  if (state->transition_control == 0) {
    if (state->chatter_state == CHATTERSTATE_IDLE) {
      rc = 0; // stop
      goto exit;
    }
    if (state->chatter_state < CHATTERSTATE_STOP) {
      if (keyscan(state) & USERINPUT_FIRE) {
        drive_chatter_stop(state);
        play_start_noise(state); /* exit via */
        rc = 0; // stop
        goto exit;
      }
    } else {
      setup_transition(state, TRANSITIONSTRIDE_FORWARD);
    }
  }

exit:
  state->speccy->sleep(state->speccy, 250000); // wild guess
  return rc; // loop
}

/**
 * $85E4: Reveal perp car
 *
 * \param[in] state Pointer to game state.
 */
static void reveal_perp_car(chqstate_t *state)
{
  const int MaxHeight = 50;

  int             revealed_height; /* was A */
  const bitmap_t *perp_bitmap;     /* was HL */
  int             width_bytes;     /* was DE */
  int             height;          /* was B (banked?) */
  const u8       *bitmap;          /* was HL (banked?) */

  if (state->wanted_stage_number == MAXSTAGE)
    return; // perp car is hidden on stage 5

  revealed_height = state->pregame_car_revealed_height + 1;
  if (revealed_height > MaxHeight)
    revealed_height--;
  state->pregame_car_revealed_height = revealed_height;

  // Get the largest of the perp car bitmaps
  perp_bitmap = state->stage->bitmaps_perp_car;
  width_bytes = perp_bitmap->width_bytes;
  height      = perp_bitmap->height;
  if (revealed_height < height)
    height = revealed_height;
  bitmap = perp_bitmap->data;

  plot_sprite(state,
              width_bytes,
              ADDRTOBACKBUF(0xF4CD),
              height,
              width_bytes,
              bitmap); /* exit via */
}

/**
 * $860F: Animate meters
 *
 * \param[in] state Pointer to game state.
 */
static void animate_meters(chqstate_t *state)
{
  int random; /* was A */
  int level;  /* was A */

  random = (s8) rng(state);
  level  = state->meter_1_level;

  // Use sign of the random value to enlarge or reduce the apparent meter
  // level.
  if (random < 0)
    if (--level >= 0)
      goto set_level;
  if (++level >= 8)
    --level;
set_level:
  state->meter_1_level = level;
  am_set_attrs(level, ADDRTOATTRS(0x5A17));

  // Update the second meter. Essentially duplicates the above code.
  random = (s8) rng(state);
  level  = state->meter_2_level;

  if (random < 0)
    if (--level >= 0)
      goto set_level2;
  if (++level >= 8)
    --level;
set_level2:
  state->meter_2_level = level;
  am_set_attrs(level, ADDRTOATTRS(0x5A57));
}

/**
 * $8646: AM set attrs
 *
 * \param[in] counter Count. (was A)
 * \param[in] attrs   Attribute address. (was HL)
 */
static void am_set_attrs(int counter, u8 *attrs)
{
  int iterations; /* was B */

  if (counter) {
    iterations = counter;
    do
      *attrs++ = attribute_BRIGHT_BLACK_OVER_GREEN;
    while (--iterations > 0);
  }

  counter = 7 - counter;
  if (counter) {
    iterations = counter;
    do
      *attrs++ = attribute_BRIGHT_BLACK_OVER_RED;
    while (--iterations > 0);
  }
}

/**
 * $865A: Draw pregame
 *
 * \param[in] state Pointer to game state.
 */
static void draw_pregame(chqstate_t *state)
{
  int       carry = 0;
  const u8 *cmds;       /* was HL */
  int       cmd;        /* was A */
  u16       cmdaddr;    /* was DE */
  int       tileidx;    /* was A */
  const u8 *srctile;    /* was DE */
  u8       *backbuf;    /* was HL */
  int       tile_count; /* was B */
  int       iterations; /* was B */
  u16       bufoffset;  /* was BC */
  int       E;          /* was E */
  u8        rows;       /* was ? */
  int       bgattr;     /* was A */
  const u8 *messages;   /* was HL */
  u16       attrs;      /* was DE */

  cmds = &pregame_data[0];
dp_get_command:
  cmd = *cmds;
  if (cmd != PREGAMECMD_STOP) {
    cmds++;

    if (cmd < PREGAMECMD_SET_BG_0) // either "Repeat" or "Plot tile"
      goto dp_repeat_or_plot_tile;
    if (cmd < PREGAMECMD_DRAW_BASE) // "Set background colour"
      goto dp_set_bg_colour;
    if (cmd >= PREGAMECMD_SET_ADDR)
      goto dp_set_address;

    // Set direction
    state->draw_pregame_direction = (cmd - PREGAMECMD_DRAW_BASE);
    goto dp_get_command;

dp_set_bg_colour:
    state->draw_pregame_background = (cmd - PREGAMECMD_SET_BG_0) << 3;
    goto dp_get_command;

dp_set_address:
    cmdaddr = (cmd << 8) | *cmds++; // cmd is 0xF0
    goto dp_get_command;

    // $00..$CF could be either "Repeat" or "Plot tile".
dp_repeat_or_plot_tile:
    tile_count = 1;
    if (cmd < PREGAMECMD_REPEAT) { // Multiple tiles
      tile_count = cmd;
      tileidx = *cmds++;
    } else {
      tileidx = cmd;
    }
    srctile = &pregame_tiles[(tileidx - PREGAMECMD_REPEAT) * 8];
    backbuf = ADDRTOBACKBUF(cmdaddr);
    do {
      // Plot a tile
      iterations = 8; // 8 rows per tile
      do {
        *backbuf = *srctile++;
        backbuf += 256;
      } while (--iterations > 0);

      // Build attribute address from back buffer ptr
      bufoffset = BACKBUFTOOFFSET(backbuf - 256 * 8);
      E = (bufoffset & 0x1F) | ((bufoffset >> 6) & 0x20); // columns + 1 row
      rows  = (bufoffset & 0xE0);
      carry = (bufoffset & 0x80) >> 7;
      rows <<= 1;

      attrs = (SCREEN_ATTRIBUTES_START_ADDRESS + 256) + E; // playfield attrs base
      if (carry)
        attrs += 256;
      attrs += rows;

      bgattr = state->draw_pregame_background;
      if (bgattr)
        *ADDRTOSCREEN(attrs) = bgattr;

      // dp_direction
      if (state->draw_pregame_direction != 1) {
        bufoffset = BACKBUFTOOFFSET(backbuf);
        // vertical
        if ((bufoffset & 0x0F00) == 0) {
          bufoffset -= 0x1000; // undoing overflow?
          bufoffset = (bufoffset & 0xFF00) | (((bufoffset & 0xFF) + 0x20) &
                                              0xFF); // L += 32
        }
      } else {
        // horizontal
        bufoffset++;
      }

      backbuf = OFFSETTOBACKBUF(bufoffset);
      srctile -= 8; /* was POP */
    } while (--tile_count > 0);
    cmdaddr = 0xF000 + bufoffset; /* was EX DE,HL ; #REGde = Back buffer ptr */
    goto dp_get_command;
  } // !CMD_STOP

  // Print strings
  iterations = 4;
  // Conv: Removed (messages-1) adjustment and pregame_messages adjusted.
  messages = &pregame_messages[0];
  do
    messages = print_message(state,
                             DRAWCHARSTYLE_SINGLE_INVERTED,
                             messages);
  while (--iterations > 0);
}

/**
 * $873C: Escape scene
 *
 * \param[in] state Pointer to game state.
 */
static void escape_scene(chqstate_t *state)
{
  silence_audio_hook(state);
  set_up_stage(state, &escape_scene_data);
  state->speed = 250; // speed of camera
  memcpy(&state->hazards[0], &escape_scene_perp, sizeof(escape_scene_perp));
  state->hazards[0].hittable.bitmaps = state->stage->bitmaps_perp_car;
  state->inhibit_collision_detection = 0xFF;
  start_chatter(state, 0xFF, &chatterblk_nancy_berates_hero[0]);

  for (;;) {
    // Print "GAME OVER" once the transition has completed.
    if (state->transition_control != TRANSITIONCONTROL_FADE)
      setup_overlay_messages(state, &game_over_message[0]);

    read_map(state);
    build_height_table(state);
    scroll_horizon(state);
    layout_road(state);
    draw_road(state);
    layout_objects(state);
    prepare_tunnel(state);
    spawn_hazards(state);
    draw_all_hazards(state);
    draw_everything_else(state);
    update_scoreboard(state);
    drive_chatter(state);
    transition(state);
    update_screen(state);

    // Loop unless the tunnel has appeared - and is right size?
    if (state->dt_tunnel_visible == 0 || state->dt_tunnel_distance >= 7)
      continue;

    // Activate the three barriers once close enough
    if (state->hazards[0].distance == 5)
      state->hazards[1].hit_timer =
        state->hazards[2].hit_timer =
          state->hazards[3].hit_timer = 0xFF;

    state->speed = 0; // Set speed to zero [speed of camera]

    if (state->hazards[0].used == HAZARD_USED ||
        state->chatter_state > CHATTERSTATE_IDLE)
      continue;

    if (state->transition_control == TRANSITIONCONTROL_STOP)
      return;

    if (state->transition_control != TRANSITIONCONTROL_FADE)
      setup_transition(state, TRANSITIONSTRIDE_FORWARD);
  }
}

/**
 * $87DC: Set up stage
 *
 * \param[in] state      Pointer to game state.
 * \param[in] scene_data Source bitmap data.
 */
static void set_up_stage(chqstate_t        *state,
                         const scenedata_t *scene_data)
{
  int iterations;   /* was B */

  memset(&state->road_buffer[0], 0, 256);
  state->session         = saved_game_state;
  state->hazards[0] = saved_game_state_hazard_0;
  memset(&state->hazards[1], 0, sizeof(hazard_t) * (MAXHAZARDS - 1));

  state->scenedata = *scene_data;

  pre_shift_backdrop(state);

  // Set backdrop position in horizon table (used to draw attributes)
  state->horizon_attr[0] = 8;
  state->horizon_attr[1] =
    0; // initialised strangely, presumed to be zero (needs checking)
  state->horizon_attr[2] = 0;

  // Disable the helicopter and tunnel drawing calls in draw_everything_else
  state->dee_tunnel_1 = 0; // draw tunnel call
  state->dee_helicopter = 0; // draw heli call
  state->dee_tunnel_2 = 0; // draw tunnel call

  state->rm_SM_C058_hazard_type = 0; // clear current hazard?
  state->mhc_y_offset = 0; // clear jump counter?

  state->hazards[0].hittable.bitmaps = state->stage->bitmaps_perp_car;

  // Conv: Duplicate work removed.

  // Run the map reader 32 times [enough to draw the screen?]
  iterations = 32;
  do
    rm_cycle_buffer_offset(state, &state->fast_counter);
  while (--iterations > 0);

  // Disallow spawning
  state->allow_spawning = 0;

  setup_transition(state, TRANSITIONSTRIDE_REVERSE);

  clear_playfield_set_attrs(state);
  // Clear the lights' BRIGHT bit
  set_up_stage_reset_lights(ADDRTOATTRS(MARQUEELIGHT_LEFT_ATTR_ADDR));
  set_up_stage_reset_lights(ADDRTOATTRS(MARQUEELIGHT_RIGHT_ATTR_ADDR));

  silence_audio_hook(state);
  update_scoreboard(state); /* exit via */
}

/**
 * $8860: Set up stage reset lights
 *
 * \param[in] attrptr Screen attribute address. (was HL)
 */
static void set_up_stage_reset_lights(u8 *attrptr)
{
  int rows; /* was C */
  int cols; /* was B */

  rows = MARQUEELIGHT_HEIGHT;
  do {
    cols = MARQUEELIGHT_WIDTH;
    do
      *attrptr++ &= ~ATTR_BRIGHT;
    while (--cols > 0);
    attrptr += SCREEN_ATTRIBUTES_ROWBYTES - MARQUEELIGHT_WIDTH;
  } while (--rows > 0);
}

/**
 * $8876: Check user input
 *
 * \param[in] state Pointer to game state.
 */
static void check_user_input(chqstate_t *state)
{
  int  transctl;   /* was A */
  u8  *puserinput; /* was HL */
  int  input;      /* was A */
  u8  *pboost;     /* was HL */
  int  keys;       /* was A */

  transctl = state->transition_control;
  puserinput = &state->user_input;
  if (transctl != TRANSITIONCONTROL_FADE) {
    *puserinput = USERINPUT_NONE;
    return;
  }

  *puserinput = input = (state->session.user_input_mask & *puserinput);
  if ((input & (USERINPUT_QUIT | USERINPUT_PAUSE | USERINPUT_TURBO)) == 0)
    return;

  if (input & USERINPUT_QUIT) {
    check_user_input_quit_key(state);
    return;
  }

  if ((input & USERINPUT_PAUSE) == 0) {
    // (If it's not pause it's...) Turbo pressed
    pboost = &state->boost;
    if (*pboost > 0 || state->session.turbos == 0)
      return; // already boosting or no turbos remain

    *pboost = 60; // set 60 ticks of boost

    start_chatter(state, 2, &chatterblk_turbo[0]);
    setup_engine_sfx_hook(state); /* exit via */
  } else {
    // Conv: check_user_input_quit_key hoisted out from here.

    silence_audio_hook(state);
    do
      keys = keyscan(state);
    while (keys & USERINPUT_PAUSE);
    do
      keys = keyscan(state);
    while ((keys & USERINPUT_NOT_QUIT) == 0);
    do
      keys = keyscan(state);
    while ((keys & USERINPUT_NOT_QUIT) != 0);
  }
}

/**
 * $88A9: Check user input quit key
 *
 * \param[in] state Pointer to game state.
 */
static void check_user_input_quit_key(chqstate_t *state)
{
  if (state->quit_state != QUITSTATE_IDLE)
    return;

  drive_chatter_stop(state);
  fill_attributes(state);

  state->session.user_input_mask = USERINPUTMASK_ALLOW_NONE;
  state->quit_state         = QUITSTATE_START;
}

/**
 * $88D5: Clear playfield attrs
 *
 * \param[in] state Pointer to game state.
 */
static void clear_playfield_attrs(chqstate_t *state)
{
  memset(ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR),
         attribute_BLACK_OVER_BLACK,
         SCREEN_ATTRIBUTES_ROWBYTES * PLAYFIELD_HEIGHT / 8);
}

/**
 * $88E2: Clear playfield
 *
 * \param[in] state Pointer to game state.
 */
static void clear_playfield(chqstate_t *state)
{
  clear_playfield_attrs(state);
  memset(ADDRTOSCREEN(SCREEN_PLAYFIELD_BITMAP_ADDR),
         ________,
         SCREEN_BITMAP_ROWBYTES * PLAYFIELD_HEIGHT);
}

/**
 * $88F2: Start SFX
 *
 * \param[in] state    Pointer to game state.
 * \param[in] index    Sound effect index. (was B)
 * \param[in] priority Priority; higher value wins. (was C)
 */
static void start_sfx(chqstate_t *state, int index, int priority)
{
  int curr_priority; /* was A */

  curr_priority = state->sfx_priority;
  if (curr_priority == 0 || curr_priority >= priority) {
    state->sfx_index    = index;
    state->sfx_priority = priority;
  }
}

/**
 * $8903: Drive SFX
 *
 * \param[in] state Pointer to game state.
 */
static void drive_sfx(chqstate_t *state)
{
  // $893C
  static const struct sfxtab {
    u8     arg1;
    u8     arg2;
    void (*handler)(chqstate_t *state, int arg1, int arg2);
  } sfx_table[9] = {
    { 0x64, 0x01, sfx_cornering            },
    { 0x08, 0x00, sfx_thud                 },
    { 0x08, 0x00, sfx_crash                },
    { 0x0C, 0x00, sfx_crash                },
    { 0x03, 0x00, sfx_thud                 },
    { 0x1A, 0x04, sfx_cornering_loop_outer },
    { 0x04, 0x78, sfx_cornering_loop_outer },
    { 0x78, 0x78, sfx_bipbow               },
    { 0xC8, 0xC8, sfx_bipbow               },
  };

  const struct sfxtab *sfx; /* was HL */

  if (state->tunnel_sfx == 0) {
    state->trigger_lane_change_sfx |= state->trigger_passed_object_sfx;
    if (state->trigger_lane_change_sfx)
      start_sfx(state, EFFECT_CORNERING, 4); /* priority 4 */
  }

  play_engine_sfx_hook(state);
  play_regular_sfx_hook(state);
  write_audio_registers_hook(state);

  if (state->sfx_index == 0)
    return;

  state->sfx_index    = 0;
  state->sfx_priority = 0;

  sfx = &sfx_table[state->sfx_index - 1];
  sfx->handler(state, sfx->arg1, sfx->arg2);
}

/**
 * $8960: SFX crash
 *
 * \param[in] state  Pointer to game state.
 * \param[in] param1 First SFX parameter. (was D)
 * \param[in] param2 Second SFX parameter. (was E)
 */
static void sfx_crash(chqstate_t *state, int param1, int param2)
{
  int  carry = 0;
  u8  *tab; /* was HL */
  int  C;   /* was C */
  int  B;   /* was B */
  int  A;   /* was A */

  tab = &state->sfx_crash_table[0];
  C  = 93; // NELEMS(sfx_crash_table);
  do {
    B = param1;
    do {
      A = 1 << 4; // EAR bit
      if (*tab & (1 << 7))
        A &= ~(1 << 4);
      // OUT ($FE),A
      RLC(*tab);
      // NOP (twice)
    } while (--B > 0);
    tab++;
  } while (--C > 0);
}

/**
 * $89D9: SFX thud
 *
 * \param[in] state  Pointer to game state.
 * \param[in] param1 First SFX parameter. (was D)
 * \param[in] param2 Second SFX parameter. (was E)
 */
static void sfx_thud(chqstate_t *state, int param1, int param2)
{
  // $89EF
  static const u8 sfx_thud_table[32] = {
    0x02, 0x07, 0x05, 0x02, 0x04, 0x0A, 0x01, 0x04,
    0x09, 0x09, 0x06, 0x45, 0x01, 0x01, 0x04, 0x03,
    0x01, 0x03, 0x31, 0x04, 0x25, 0x02, 0x01, 0xBD,
    0x8E, 0xED, 0x01, 0x01, 0x01, 0x06, 0x07, 0x01
  };

  int       C;
  const u8 *HL;
  int       A;
  int       B;
  int       E;

  C = 32; // NELEMS(sfx_thud_table);
  HL = &sfx_thud_table[0];
  A = 0;
  do {
    B = *HL;
    do {
      // OUT $(FE),A
      E = param1;
      do {/*delay*/} while (--E > 0);
    } while (--B > 0);
    A ^= 16; // EAR bit
    HL++;
  } while (--C > 0);
}

/**
 * $8A0F: SFX cornering
 *
 * \param[in] state  Pointer to game state.
 * \param[in] param1 First SFX parameter. (was D)
 * \param[in] param2 Second SFX parameter. (was E)
 */
static void sfx_cornering(chqstate_t *state, int param1, int param2)
{
  state->sfx_cornering_toggle ^= 1;
  if (state->sfx_cornering_toggle)
    return;

  sfx_cornering_loop_outer(state, param1, param2); /* was fallthrough */
}

static void sfx_cornering_loop_outer(chqstate_t *state, int param1, int param2)
{
  int C;
  int B;

  do {
    C = param2;
    do {
      if (rng(state) & (1 << 4)) {
        B = 24 - param1;
        do {/*delay*/} while (--B > 0);
        // OUT ($FE),8 + 16; // EAR + MIC bits
        B = param1;
        do {/*delay*/} while (--B > 0);
        // OUT ($FE),0
      }
    } while (--C > 0);
  } while (--param1 > 0);
}

/**
 * $8A36: SFX bipbow
 *
 * \param[in] state  Pointer to game state.
 * \param[in] param1 First SFX parameter. (was D)
 * \param[in] param2 Second SFX parameter. (was E)
 */
static void sfx_bipbow(chqstate_t *state, int param1, int param2)
{
  int C;
  int H;
  int L;
  int B;

  C = 20;
  H = L = 5;
  do {
    do {
      do {/* delay */} while (--param1);
      param1 = param2;
      B = 24 - C;
      do {/* delay */} while (--B);
      // OUT ($FE),8 + 16; // EAR + MIC bits
      B = C;
      do {/* delay */} while (--B);
      // OUT ($FE),0
    } while (--H > 0);
    H = L;
  } while (--C > 0);
}

/**
 * $8A57: Handle perp caught
 *
 * \param[in] state Pointer to game state.
 * \return Non-zero on success.
 */
static int handle_perp_caught(chqstate_t *state)
{
  int       carry = 0;
  int       zero  = 0;
  int       phase;       /* was A */
  int       car_y;       /* was A */
  int       fastcounter; /* was A */
  u8        A;
  int       Ainput;
  int       H;
  int       L;
  u8        D;
  int       C = 0; // tmp fix
  int       Cinput;
  int       Biterations;
  int       Bdelta;
  int       Adash;
  u8       *DE;
  int       DEspeed;
  u8       *HLscore;
  u8       *HLphc;
  int       Aperpdistance;
  int       HLspeed;
  int       HLspeedpushed;
  int       HLroadpos;
  const u8 *HLmessages;
  u8        Cflag;

  phase = state->perp_caught_phase;
  switch (phase) {
  case PERPCAUGHTPHASE_NONE:
    return 0;
  case PERPCAUGHTPHASE_ALIGNING:
    goto move_perp;
  case PERPCAUGHTPHASE_STOPPING:
    goto phase2;
  case PERPCAUGHTPHASE_STOPPED:
    goto phase3;
  case PERPCAUGHTPHASE_SCORE:
    goto phase4;
  default:
    break;
  }

  // Otherwise 5/6
  if (state->transition_control)
    return 0;
  if (phase == PERPCAUGHTPHASE_FADING)
    goto phase5;

  // Must be 6
  silence_audio_hook(state);
  state->wanted_stage_number++;
  return 1; // Conv: signal to bypass remainder of main loop

phase5:
  state->perp_caught_phase = PERPCAUGHTPHASE_ADVANCING;
  setup_transition(state, TRANSITIONSTRIDE_FORWARD); /* was exit via */
  return 0;

phase2:
  car_y = state->car_y;
  if (car_y >= 16)
    goto start_phase_3;
  state->car_y = car_y + 4;

  HLroadpos = state->scenedata.road_pos + 12;
  if (HLroadpos >= ROAD_126)
    HLroadpos = ROAD_126;
  state->scenedata.road_pos = HLroadpos;

  fastcounter = state->fast_counter + 32;
  if (state->fast_counter + 32 > 255)
    return 0;
  state->fast_counter = fastcounter;
  return 0;

start_phase_3:
  state->perp_caught_phase = 3;
  state->handle_perp_caught_delay = 4;
  fill_attributes(state); /* exit via */
  return 0;

phase3:
  A = state->handle_perp_caught_delay - 1;
  state->handle_perp_caught_delay = A;
  if (A)
    return 0;

  state->perp_caught_phase = PERPCAUGHTPHASE_SCORE;
  HLmessages = state->stage->addrof_arrest_messages;
  setup_overlay_messages_with_transition(state,
                                         TRANSITIONCONTROL_DRAW_MUGSHOTS,
                                         HLmessages); /* was exit via */
  return 0;

phase4:
  if (state->mode_128k)
    handle_perp_caught_128k(state);
  if (state->transition_control)
    return 0;
  if (state->mode_128k)
    handle_perp_caught_128k(state);

  state->perp_caught_phase = PERPCAUGHTPHASE_FADING;

  // Calc bonus

  H = '0';
  D = state->wanted_stage_number;
  L = D + '0';

  if (state->retry_count) {
    RLC(D);
    RLC(D);
    RLC(D);
    RLC(D);

    H = L;
    L = ' ';
  }
  // Write to CLEAR BONUS line
  // Conv: Split up
  state->score_messages[0x8C6F - SCORE_MESSAGES_BASE] = L;
  state->score_messages[0x8C70 - SCORE_MESSAGES_BASE] = H;

  RLC(D);
  RLC(D);
  RLC(D);
  RLC(D);
  increment_score(state, 0, 0, D);

  A = state->session.time_bcd;
  state->score_messages[0x8C8A - SCORE_MESSAGES_BASE] =
    A; // Write to TIME BONUS line
  C = A;
  RLC(A);
  RLC(A);
  RLC(A);
  RLC(A);
  A &= 0x0F;
  if (A)
    goto have_high_digit;

  Adash = ' ';
  goto store_time_bonus_high;

have_high_digit:
  Biterations = A;
  A += '0';
  Adash = A;
  do
    increment_score(state, 0, 0x00, 0x05); // 50,000 lo,mid,hi
  while (--Biterations > 0);

store_time_bonus_high:
  A = Adash;
  state->score_messages[0x8C8A - SCORE_MESSAGES_BASE] =
    A; // Write to TIME BONUS line
  A = C & 0x0F;
  if (A == 0)
    goto store_time_bonus_low;

  // Bug fix applied
  Biterations = A;
  Adash = A;
  do
    increment_score(state, 0, 0x50, 0x00); // 5,000 lo,mid,hi
  while (--Biterations > 0);

store_time_bonus_low:
  A = Adash + '0';
  state->score_messages[0x8C8B - SCORE_MESSAGES_BASE] =
    A; // Write to TIME BONUS line

  // Display score
  Biterations = 4;
  Cflag = 0; // flag
  DE = &state->score_bcd[3];
  HLscore = &state->score_messages[0x8CA8 - SCORE_MESSAGES_BASE];
  do {
    A = *DE;
    RLC(A);
    RLC(A);
    RLC(A);
    RLC(A);
    A &= 0x0F;
    if (A)
      goto score_have_high_digit;

    RLC(A);
    if (carry)
      goto score_have_high_digit;
    A = ' ';
    goto score_store_high;

score_have_high_digit:
    Cflag = 0xFF;
    A += '0';

score_store_high:
    *HLscore++ = A;
    A = *DE & 0x0F;
    if (A)
      goto score_have_low_digit;

    RLC(Cflag);
    if (carry)
      goto score_have_low_digit;
    A = ' ';
    goto score_store_low;

score_have_low_digit:
    Cflag = 0xFF;
    A += '0';

score_store_low:
    *HLscore++ = A;
    DE--;
  } while (--Biterations > 0);

  *--HLscore |= EOS;

  setup_overlay_messages(state, &state->score_messages[0]); /* was exit via */
  return 0;

move_perp:
  A = state->hazards[0].horz_pos;
  Bdelta = 5;
  if (A == 35)
    goto assign_perp_pos;
  else if (A < 35)
    goto change_perp_pos;
  Bdelta = -5; // else greater than

change_perp_pos:
  C = A + Bdelta;

assign_perp_pos: // is this in the right place?
  A = C;
  state->hazards[0].horz_pos = A;
  HLroadpos = state->scenedata.road_pos;
  carry = (HLroadpos < ROAD_LEFTMOST); /* was PUSH/SUB/POP */
  Ainput = USERINPUT_UP | USERINPUT_RIGHT;
  if (!carry)
    goto assign_hero_pos;

  HLroadpos -= ROAD_RIGHTMOST; // FIXME set carry here?
  Ainput = USERINPUT_UP | USERINPUT_LEFT;
  if (carry)
    goto assign_hero_pos;

  Ainput = USERINPUT_UP;

assign_hero_pos:
  Cinput = Ainput & (USERINPUT_LEFT | USERINPUT_RIGHT);
  if (Cinput)
    goto perp_too_far_away;

  if (state->hazards[0].distance >= 3)
    goto perp_too_far_away;

  HLphc = &state->session.perp_halt_counter;
  (*HLphc)--;
  if (*HLphc)
    goto perp_too_far_away;

  state->speed = 0;
  state->hazards[0].dist_frac = 0;
  state->hazards[0].distance = 1;
  state->perp_caught_phase = PERPCAUGHTPHASE_STOPPING;
  state->smoke = 3;
  DEspeed = 0; // Conv
  goto set_perp_speed;

perp_too_far_away:
  Aperpdistance = state->hazards[0].distance;
  HLspeed = SPEED_PERP_CHASE;
  if (Aperpdistance < 15)
    HLspeed -= 20 * (16 - Aperpdistance);
  DEspeed = HLspeed; // perp's adjusted speed
  HLspeed = state->speed; // our speed
  HLspeedpushed = HLspeed; // PUSH HL
  carry = (HLspeed < DEspeed);
  HLspeed -= DEspeed;
  if (!carry) {
    Cinput &= ~USERINPUT_UP;
    carry = (HLspeed < 50);
    HLspeed -= 50;
    if (!carry)
      Cinput |= USERINPUT_DOWN;
  }
  HLspeed = HLspeedpushed; // POP HL
  carry = (HLspeed > SPEED_GEAR_CHANGE);
  HLspeed -= SPEED_GEAR_CHANGE;
  A = state->gear - carry; // set low speed if speed<SPEED_GEAR_CHANGE
  if (A == 0)
    Cinput |= USERINPUT_FIRE; // change gear
  state->user_input = Cinput;

  HLspeed = state->hazards[0].speed;
  HLspeedpushed = HLspeed; // PUSH HL
  DEspeed = SPEED_PERP_MIN;
  carry = (HLspeed < DEspeed);
  HLspeed -= DEspeed;
  zero = (HLspeed == 0);
  HLspeed = HLspeedpushed; // POP HL
  if (zero || carry) {
    goto set_perp_speed; // with DE=70
  }
  HLspeed -= 5;
  DEspeed = HLspeed;

set_perp_speed:
  hpc_set_perp_speed(state, DEspeed); /* was fallthrough */
  return 0;
}

/**
 * $8C35: HPC set perp speed
 *
 * \param[in] state Pointer to game state.
 * \param[in] speed Speed value. (was DE)
 */
static void hpc_set_perp_speed(chqstate_t *state, int speed)
{
  state->hazards[0].speed = speed;
}

/**
 * $8C3A: Fully smashed
 *
 * \param[in] state Pointer to game state.
 */
static void fully_smashed(chqstate_t *state)
{
  state->perp_caught_phase  = PERPCAUGHTPHASE_ALIGNING;
  state->hand_flag          = HANDFLAG_STOP;
  state->smash_counter      = SMASHCOUNTER_MAX;
  state->session.user_input_mask = USERINPUT_PAUSE | USERINPUT_QUIT;
  setup_overlay_messages(state, &pull_over_message[0]);
  hpc_set_perp_speed(state, SPEED_ATTRACT);
}

/**
 * $8D8F: Transition
 *
 * \param[in] state Pointer to game state.
 */
static void transition(chqstate_t *state)
{
  int       iterations;  /* was B' */
  u16       backbuf;     /* was HL */
  const u8 *maskptr;     /* was HL' */
  u16       backbufcopy; /* was D */
  int       mask;        /* was E */

  switch (state->transition_control) {
  case TRANSITIONCONTROL_STOP:
    return;
  case TRANSITIONCONTROL_DRAW_MUGSHOTS:
    draw_mugshots(state); // exit via
    return;
  case TRANSITIONCONTROL_OVERLAY_MESSAGES:
    draw_overlay_messages(state); // exit via
    return;
  case TRANSITIONCONTROL_FILL_ATTRIBUTES:
    fill_attributes(state); // exit via
    return;
  case TRANSITIONCONTROL_FADE:
    break;
  default:
    assert(0);
  }

  if (--state->transition_nframes == 0)
    state->transition_control = TRANSITIONCONTROL_STOP;
  else
    // Advance before use - initial mask points one earlier/later
    state->transition_mask += state->transition_frame_stride;

  backbuf  = 0xFF00; /* was H=$FF */
  maskptr = state->transition_mask;
  iterations = 8;
  do {
    mask = *maskptr;
    backbufcopy = backbuf; // Conv: Original just saved H in D
    backbuf = (backbuf & ~0xFF) | 0xFE;
    transition_fade_chunk(state, mask, ADDRTOBACKBUF(backbuf));
    backbuf -= 8 << 8;
    transition_fade_chunk(state, mask, ADDRTOBACKBUF(backbuf));
    backbuf = backbufcopy - 256; // restore
    maskptr++;
  } while (--iterations > 0);
}

/**
 * $8DD8: Overwrite odd/even UDG rows of the back buffer with a single byte
 *
 * \param[in] state   Pointer to game state.
 * \param[in] mask    Mask. (was E)
 * \param[in] backbuf Back buffer address. (was HL)
 */
static void transition_fade_chunk(chqstate_t *state, int mask, u8 *backbuf)
{
  int rows;       /* was C */
  int iterations; /* was B */

  rows = 8; // rows
  do {
    iterations =
      6; // 6 iterations (of 5 ops each in the loop below) = 30 bytes written (~ a scanline)
    do {
      *backbuf-- |= mask;
      *backbuf-- |= mask;
      *backbuf-- |= mask;
      *backbuf-- |= mask;
      *backbuf-- |= mask;
    } while (--iterations > 0);
    backbuf -= 2;
  } while (--rows > 0);
}

/**
 * $8DF9: Setup transition
 *
 * \param[in] state  Pointer to game state.
 * \param[in] stride Stride of bitmap data, in bytes. (was A)
 */
static void setup_transition(chqstate_t *state, int stride)
{
  int                 frame_stride; /* was BC */
  const transition_t *transitions;  /* was DE */
  const transition_t *transition;   /* was HL */

  assert(stride == 8 || stride == -8);

  frame_stride = stride;
  // Conv: Points at non-relocated table.
  transitions = &transitions_e88e[0];
  if (frame_stride < 0) // reversed
    // Conv: Removed widening op (already in frame_stride)
    transitions = &transitions_e88e[4]; // second half of table

  state->transition_frame_stride = frame_stride;

  // Pick a random entry in the table
  transition = &transitions[rng(state) & 3];

  state->transition_nframes = transition->nframes;
  state->transition_mask    = transition->maskbase;
  state->transition_control = TRANSITIONCONTROL_FADE;
}

/**
 * $8E29: Fill attributes
 *
 * \param[in] state Pointer to game state.
 */
static void fill_attributes(chqstate_t *state)
{
  u8 *src;     /* was HL */
  u8 *dst;     /* was DE */
  int rows;    /* was A */
  int columns; /* was BC */

  src = ADDRTOATTRS(0x5901); // (1,8)
  rows = 16; // rows
  do {
    if (1) {
      // As original code
      dst = src + 1;
      columns = 28;
      do { *dst++ = *src++; } while (--columns > 0); /* was LDIR */
      src += 32 - 28;
    } else {
      // Conv: Alternative that uses memset
      memset(src + 1, *src, 28);
      src += 32;
    }
  } while (--rows > 0);

  state->transition_control = TRANSITIONCONTROL_STOP;
}

/**
 * $8E42: Progresively draw overlay messages to the back buffer
 *
 * Overlay messages are, for example, the messages shown when the perp has
 * been arrested.
 *
 * This function draws the overlay_message currently set in state to the back
 * buffer. The initial count is 1 so that the messages are made to
 * progressively appear. Once all messages are drawn the delay value is
 * checked and decremented. While the delay is non-zero the routine returns.
 * When the delay is zero a new delay is set and the number of messages
 * increased. The first message pointed to is the second byte of the message
 * (delay).
 *
 * \param[in] state Pointer to game state.
 */
static void draw_overlay_messages(chqstate_t *state)
{
  const u8 *message; /* was HL */
  int       count;   /* was B */
  int       style;   /* was A */

  message = state->overlay_message;
  count   = state->overlay_count;
  for (;;) {
    if (--count == 0) {
      if (--state->overlay_delay)
        return;

      state->overlay_delay = *message; // set new delay
      state->overlay_count++;
      count++;
    }

    style = *++message;
    if (style == DRAWOVERLAY_STOP) // special marker (rename)
      break;

    message = print_message(state, style, message);
  }

  state->transition_control = message[-1];
}

/**
 * $8E6C: Print a message on the back buffer
 *
 * This function draws a message to the back buffer using the specified drawing
 * style and message data. The message data includes attributes, the
 * destination back buffer address and screen attribute address.
 *
 * Note that the screen attribute bytes are drawn directly to the real screen
 * since they have no parallel in the back buffer. The user may briefly see
 * attribute changes prior to the back buffer arriving on-screen.
 *
 * \param[in] state    Pointer to game state.
 * \param[in] style    Message draw style (e.g. DRAWCHARSTYLE_SINGLE). (was A)
 * \param[in] messages Pointer to message data. (was HL)
 * \return Next byte of message data. (was HL)
 */
static const u8 *print_message(chqstate_t *state,
                               int          style,
                               const u8   *messages)
{
  int attr;     /* was A */
  u16 backbuf;  /* was DE */
  u16 attraddr; /* was BC */

  // The style in messages[0] is ignored.
  attr     = messages[1];
  backbuf  = wordat(messages + 2);
  attraddr = wordat(messages + 4);
  messages += 6;

  return draw_string_with_style(state,
                                attr,
                                ADDRTOSCREEN(attraddr),
                                ADDRTOBACKBUF(backbuf),
                                messages,
                                style);
}

/**
 * $8E7E: Setup overlay messages
 *
 * \param[in] state   Pointer to game state.
 * \param[in] message Message data pointer. (was HL)
 */
static void setup_overlay_messages(chqstate_t *state, const u8 *message)
{
  setup_overlay_messages_with_transition(state,
                                         TRANSITIONCONTROL_OVERLAY_MESSAGES,
                                         message);
}

/**
 * $8E80: Setup overlay messages with transition
 *
 * \param[in] state      Pointer to game state.
 * \param[in] transition Transition. (was A)
 * \param[in] message    Message. (was HL)
 */
static void setup_overlay_messages_with_transition(chqstate_t *state,
    int          transition,
    const u8   *message)
{
  state->transition_control = transition;
  state->overlay_delay      = message[0];
  state->overlay_message    = &message[1];
  state->overlay_count      = 1;
}

/**
 * $8E91: Draws the three mugshots to the back buffer
 *
 * Used when the perp is caught.
 *
 * \param[in] state Pointer to game state.
 */
static void draw_mugshots(chqstate_t *state)
{
  draw_mugshot(state,
               0x48A5,
               0xFF88,
               state->stage->addrof_perp_mugshot_attributes);
  draw_mugshot(state,
               0x48B4,
               0xFF97,
               &bitmap_faces[2 * FACEBYTES + FACEBITMAPBYTES]);
  draw_mugshot(state,
               0x48B9,
               0xFF9C,
               &bitmap_faces[1 * FACEBYTES + FACEBITMAPBYTES]);

  draw_overlay_messages(state);
}

/**
 * $8EB7: Draws a single mugshot to the back buffer
 *
 * \param[in] state   Pointer to game state.
 * \param[in] attrs   Screen address at which to set attributes. (was BC)
 * \param[in] backbuf Back buffer address at which to draw. (was DE)
 * \param[in] mugshot Mugshot data attributes address. Bitmap data precedes. (was HL)
 */
static void draw_mugshot(chqstate_t *state,
                         int         attrs,
                         int         backbuf,
                         const u8   *mugshot)
{
  const u8 *orig_mugshot; /* was PUSH-POP */
  int       counter;      /* was BC */

  orig_mugshot = mugshot;
  mugshot--; // step back from attributes start to bitmap data end
  counter = FACEBITMAPBYTES;
  for (;;) {
    backbuf -= BACKBUFFER_START_ADDRESS; // Conv: address -> offset
    state->backbuffer[backbuf--] = *mugshot--; counter--;
    state->backbuffer[backbuf--] = *mugshot--; counter--;
    state->backbuffer[backbuf--] = *mugshot--; counter--;
    state->backbuffer[backbuf--] = *mugshot--; counter--;
    backbuf += BACKBUFFER_START_ADDRESS; // Conv: offset -> address
    backbuf += 4; // Conv: replaces register stash
    if (counter == 0)
      break;

    backbuf = prevbufrow(backbuf);
  }

  plot_face_attributes(state, attrs, orig_mugshot); /* exit via */
}

/**
 * $8EE7: Draws the smash bar
 *
 * \param[in] state Pointer to game state.
 */
static void draw_smash_bar(chqstate_t *state)
{
  const int MaxSegments    = SMASHCOUNTER_MAX;
  const int SegmentHeight  = 3;
  const int BorderHeight   = 2;
  const int TotalBarHeight = MaxSegments * SegmentHeight * BorderHeight * 2;

  u16 backbuf;    /* was HL */
  int nsmashsegs; /* was A */
  int nsolid;     /* was B */

  if (state->sighted_flag == 0)
    return; // Return if the perp has not yet been sighted

  if (state->perp_caught_phase >= PERPCAUGHTPHASE_STOPPED)
    return; // Return if perp_caught_phase is >= 3 (car has stopped)

  backbuf = 0xF7A2; // Back buffer address of bottom of bar
  // Conv: D & E moved into prevbufrow forward

  // Draws bottom two rows
  backbuf = draw_smash_bar_solid_bit(state, BorderHeight, backbuf);

  nsmashsegs = state->smash_counter;
  if (nsmashsegs > 0)
    backbuf = draw_smash_bar_segments(state, nsmashsegs, backbuf);

  nsolid = TotalBarHeight - BorderHeight - nsmashsegs *
           SegmentHeight; // Number of solid rows to draw at the top
  (void) draw_smash_bar_solid_bit(state, nsolid, backbuf); /* exit via */
}

/**
 * $8F13: Draws the specified number of smash bar segments
 *
 * Each segment is three rows high, the last of which is solid.
 *
 * \param[in] state   Pointer to game state.
 * \param[in] nsegs   Number of segments to draw. (was C)
 * \param[in] backbuf Back buffer address at which to draw. (was HL)
 */
static u16 draw_smash_bar_segments(chqstate_t *state, int nsegs, int backbuf)
{
  do {
    *ADDRTOBACKBUF(backbuf) = X______X; // Set 8 pixels
    backbuf = prevbufrow(backbuf);
    *ADDRTOBACKBUF(backbuf) = X______X; // Set 8 pixels
    backbuf = prevbufrow(backbuf);
    backbuf = draw_smash_bar_solid_bit(state, 1, backbuf); // 1 row gap
  } while (--nsegs > 0);
  return backbuf;
}

/**
 * $8F47: Draws the specified number of smash bar solid rows
 *
 * \param[in] state   Pointer to game state.
 * \param[in] nrows   Number of segments to draw. (was B)
 * \param[in] backbuf Back buffer address at which to draw. (was HL)
 */
static u16 draw_smash_bar_solid_bit(chqstate_t *state, int nrows, int backbuf)
{
  do {
    *ADDRTOBACKBUF(backbuf) = XXXXXXXX; // Set 8 pixels
    backbuf = prevbufrow(backbuf);
  } while (--nrows > 0);
  return backbuf;
}

/**
 * $8F5F: Draw everything else
 *
 * \param[in] state Pointer to game state.
 */
static void draw_everything_else(chqstate_t *state)
{
  u8             *HLheight_table;        /* was HL */
  u8             *DEclamped_heights;        /* was DE */
  int             Biterations;         /* was B */
  u8             *IYheight_table;        /* was IY */
  u8             *HLroadbuf;           /* was HL */
  u16            *IXtable_ea00;        /* was IX */
  int             Afloating_arrow;     /* was A */
  int             Aobj;                /* was A */
  const bitmap_t *HLarrow_defn;        /* was HL */
  int             Ex;                  /* was E */
  int             Dy;                  /* was D */
  int             Cwidth_bytes;        /* was C */
  int             Bdash_flip_flag;     /* was B */
  int             Edash_bitmap_stride; /* was E */
  int             Cdash;               /* was C */
  int             Bheight;             /* was B */
  const u8       *HLbitmap;            /* was HL */
  int             Eobj;                /* was E */
  const obj_t    *HLobj;               /* was HL */

  assert(state->stage != NULL);
  assert(state->road_buffer_start == &state->road_buffer[0]);
  assert(state->road_buffer_end   == &state->road_buffer[256]);

  state->dss_fork_xpos_ptr = &state->xpos_road_fork_right[20]; // $ED28
  state->dh_xpos_table = &state->xpos_road_centre_left[0];
  assert(state->dss_fork_xpos_ptr >= &state->xpos_road_fork_right[0] &&
         state->dss_fork_xpos_ptr < &state->xpos_road_fork_right[128]);
  assert(state->dh_xpos_table >= &state->xpos_road_centre_left[0] &&
         state->dh_xpos_table < &state->xpos_road_centre_left[128]);

  HLheight_table = &state->height_table[1]; // table of objects?
  DEclamped_heights = &state->clamped_heights[0];
  Biterations = 21;
  do {
    assert(HLheight_table >= &state->height_table[0]
           && HLheight_table < &state->height_table[22]);
    assert(DEclamped_heights >= &state->clamped_heights[0]
           && DEclamped_heights < &state->clamped_heights[21]);
    *HLheight_table++ += 32;
    *DEclamped_heights++ += 32;
  } while (--Biterations > 0);
  assert(HLheight_table == &state->height_table[22]);
  assert(DEclamped_heights == &state->clamped_heights[21]);

  IYheight_table = &state->height_table[21];
  assert(IYheight_table == &state->height_table[21]);
  if (state->dee_tunnel_1)
    draw_tunnel(state, IYheight_table);
  IYheight_table--;
  assert(IYheight_table == &state->height_table[20]);

  HLroadbuf = ROADBUF_FWD2PTR(115); // right side objects
  assert(HLroadbuf >= state->road_buffer_start
         && HLroadbuf < state->road_buffer_end);

  IXtable_ea00 = &state->xpos_road_centre[88]; // $EAB0
  assert(IXtable_ea00 == &state->xpos_road_centre[88]);
  Biterations = 20; // iterations
  do {
    assert(Biterations >= 1 && Biterations <= 20);
    assert(HLroadbuf >= state->road_buffer_start
           && HLroadbuf < state->road_buffer_end);
    assert(IYheight_table >= &state->height_table[0]
           && IYheight_table < &state->height_table[22]);
    assert(IXtable_ea00 >= &state->xpos_road_centre[0]
           && IXtable_ea00 < &state->xpos_road_centre[128]);

    if (state->n_hazards)
      draw_arrow_fire_smoke(state, Biterations, IYheight_table);

    dust_stones_stuff(state, Biterations, IYheight_table);

    if (state->dee_helicopter)
      draw_helicopter(state, Biterations, IYheight_table);

    if (state->dee_tunnel_2)
      draw_tunnel(state, IYheight_table);

    Aobj = *HLroadbuf; // fetch right side object from road buffer
    assert(Aobj <= 9); // object indices are 0..9
    if (Aobj)
      goto right_hand_stuff;

continue_after_right_hand_done:
    IXtable_ea00++;
    WRAPPING(HLroadbuf, 32,
             state->road_buffer_start); // advance to left-side column
    assert(HLroadbuf >= state->road_buffer_start
           && HLroadbuf < state->road_buffer_end);

    Aobj = *HLroadbuf; // fetch left side object from road buffer
    assert(Aobj <= 9); // object indices are 0..9
    if (Aobj)
      goto left_hand_stuff;

continue_after_left_hand_done:
    IXtable_ea00++;
    WRAPPING(HLroadbuf, -33, state->road_buffer_start); // retreat one row
    assert(HLroadbuf >= state->road_buffer_start
           && HLroadbuf < state->road_buffer_end);

    IYheight_table--;
  } while (--Biterations > 0);
  assert(IYheight_table == &state->height_table[0]);
  assert(IXtable_ea00 ==
         &state->xpos_road_centre[128]); // one-past-end after 20*2 advances from [88]

  if (state->dee_helicopter)
    return;

  Afloating_arrow = state->floating_arrow;
  if (Afloating_arrow == 0)
    return;

  // Draw the floating arrow
  assert(Afloating_arrow == 1 || Afloating_arrow == 2);
  HLarrow_defn = &floating_arrow_left_defn;
  Ex = 120; // horz pos
  if (Afloating_arrow != 1) {
    HLarrow_defn = &floating_arrow_right_defn;
    Ex = 128;
  }
  assert(HLarrow_defn != NULL);
  assert(HLarrow_defn->data != NULL);
  assert(Ex == 120 || Ex == 128);
  Dy           = 48; // vert pos
  assert(Dy == 48);
  Cwidth_bytes = HLarrow_defn->width_bytes;
  assert(Cwidth_bytes == 3); // both arrow bitmaps have width_bytes=3
  Bdash_flip_flag = HLarrow_defn->flags >> 1; // goes in B'
  Edash_bitmap_stride = Cwidth_bytes;
  assert(Edash_bitmap_stride == Cwidth_bytes);
  Cdash        = 0; // this must be passed in
  Bheight      = HLarrow_defn->height;
  assert(Bheight == 21); // both arrow bitmaps have height=21
  assert(Bheight > 0);
  HLbitmap      = HLarrow_defn->data;
  assert(HLbitmap != NULL);
  draw_part_entry2(state,
                   Bheight,
                   Cwidth_bytes,
                   Dy,
                   Ex,
                   HLbitmap,
                   Bdash_flip_flag,
                   Cdash,
                   Edash_bitmap_stride); /* exit via */
  return;

right_hand_stuff:
  Eobj = Aobj;
  assert(Eobj >= 1
         && Eobj <= 9); // valid index range given base ptr at [-1] of a 9-element array
  assert(state->stage->addrof_right_hand_objects != NULL);
  // assert(Bheight != 255);
  if (((u8 *)IXtable_ea00)[1] ==
      0) { // Z80: LD A,(IX+1) -- buffer offset/distance (high byte of road pos)
    HLobj = &state->stage->addrof_right_hand_objects[Eobj];
    assert(HLobj->handler != NULL);
    HLobj->handler(state, Biterations, HLobj->arg, IXtable_ea00, IYheight_table);
  }
  goto continue_after_right_hand_done;

left_hand_stuff:
  Eobj = Aobj;
  assert(Eobj >= 1
         && Eobj <= 9); // valid index range given base ptr at [-1] of a 9-element array
  assert(state->stage->addrof_left_hand_objects != NULL);
  assert(Bheight != 255);
  if (Aobj != 2
      && ((u8 *)IXtable_ea00)[1]) { // Z80: LD A,(IX+1) -- buffer offset/distance
    HLobj = &state->stage->addrof_left_hand_objects[Eobj];
    assert(HLobj->handler != NULL);
    HLobj->handler(state, Biterations, HLobj->arg, IXtable_ea00, IYheight_table);
  }
  goto continue_after_left_hand_done;
}

/**
 * $9052: Draws overhead objects
 *
 * \param[in] state      Pointer to game state.
 * \param[in] Bparam     Parameter.
 * \param[in] Cparam     Parameter.
 * \param[in] DEstretchy DEstretchy.
 * \param[in] IXxpos  X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
static void draw_overhead(chqstate_t       *state,
                          int                Bparam,
                          int                Cparam,
                          const stretchy_t *DEstretchy, // TODO: Should this be a void * ?
                          const u16        *IXxpos,
                          const u8         *IYheight)
{
  const stretchy_t      *HLstretchy;  /* was HL */
  const depthset_pair_t *DEpairs;     /* was DE */
  int                    counter;     /* was A */
  u8                    *HLdst;       /* was HL */
  u8                    *DEsrc;       /* was DE */
  int                    Avertical;   /* was A */
  int                    A;
  int                    Aminheight;  /* was A */
  int                    Bminheight;  /* was B */
  const u8              *HLvertical;  /* was HL */
  int                    D, E, H, L;
  const depthset_pair_t *HLpair;      /* was HL */
  int                    Cdepth;      /* was C */
  const u8              *HL;
  int                    Acopy;       /* was A */
  int                    B;

  // PUSH IXxpos/DE/BC
  if (IXxpos[1] == 0) // buffer offset/distance
    draw_stretchy_object_left(state, Bparam, DEstretchy, IXxpos, IYheight);
  // POP BC/HL/IXxpos

  HLstretchy = DEstretchy; // e.g. $6F26 in Stage 3's data, loads $6F2D
  //HLstretchy++; - accounted for
  DEpairs = &HLstretchy->set->pairs[0];

  // "Scale down" pattern
  counter = state->fast_counter & 0xE0;
  counter = COUNTER_TO_PERSP_Y_ROW(counter);
  HLvertical = &persp_y_scale[counter / 22][Bparam];

  Cparam = IYheight[0] - IYheight[0x35];
  Avertical = *HLvertical;
  A = (Avertical >> 1) + Avertical - Cparam; // Conv: removed use of L
  state->do_vert_sub = A; // Self modify 'SUB x' at $90F1
  Aminheight = MIN(Bparam - 1, 9);
  Bminheight = Aminheight;
  HLpair = &DEpairs[Aminheight];
  Cdepth = HLpair->depth;
  HL = (const u8 *) &DEpairs[10] + Bminheight *
       3; // depthsets seem to have more data than expected...

  D = 1;
  A = IXxpos[1]; // buffer offset/distance
  if ((s8) A >= 0) {
    if (A)
      return;
    A = IXxpos[0] + 24 - Cdepth;
    if ((s8) A >= 0) {
      A -= 8;
      if ((s8) A >= 0) {
        if (A >= 8)
          D = A >> 3;
      }
    }
  }

  IXxpos -= 2;

  E = 0x1F;
  A = IXxpos[1]; // buffer offset/distance
  if ((s8) A < 0)
    return;
  if (A == 0) {
    A = IXxpos[0] + Cdepth;
    if ((s8) A >= 0) {
      if (A) {
        if (A < 0xF7) // -8
          E = A >> 3;
      }
    }
  }

  Cparam = D;
  state->do_span_width_words = ~((E - Cparam) * 2) + 61;

  A = IYheight[0x35] - state->do_vert_sub;
  if ((s8) A < 0)
    return;

  // PUSH AF
  Acopy = A + 1;
  B = *HL;
  Acopy -= B;
  if ((s8) Acopy < 0) {
    Acopy += B;
    B = Acopy;
  }
  DEsrc = NULL; // TODO wordat(++HL); // load bitmap?
  // POP AF
  HLdst = ADDRTOBACKBUF((((A & 0x0F) + 0xF0) << 8) | ((A & 0x70) * 2 + Cparam));
  goto do_draw_span;

  for (;;) {
    DEsrc++;
    if (--B == 0) // iterations
      return;

do_draw_span:
    memset(HLdst, *DEsrc, state->do_span_width_words / 2);
    HLdst = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(HLdst)));
  }
}

/**
 * $916C: Draw stretchy object left
 *
 * \param[in] state    Pointer to game state.
 * \param[in] Bdepth   Parameter.
 * \param[in] arg      Object data pointer.
 * \param[in] IXxpos   X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
void draw_stretchy_object_left(chqstate_t *state,
                               int          Bdepth,
                               const void *arg,
                               const u16  *IXxpos,
                               const u8   *IYheight)
{
  draw_stretchy_object_common(state,
                              Bdepth,
                              arg,
                              draw_object_left_stretchy_entrypt,
                              IXxpos,
                              IYheight);
}

/**
 * $9171: Draw stretchy object right
 *
 * \param[in] state    Pointer to game state.
 * \param[in] Bdepth   Parameter.
 * \param[in] arg      Object data pointer.
 * \param[in] IXxpos   IXxpos register value.
 * \param[in] IYheight IYheight register value.
 */
void draw_stretchy_object_right(chqstate_t *state,
                                int          Bdepth,
                                const void *arg,
                                const u16  *IXxpos,
                                const u8   *IYheight)
{
  draw_stretchy_object_common(state,
                              Bdepth,
                              arg,
                              draw_object_right_stretchy_entrypt,
                              IXxpos,
                              IYheight);
}

/**
 * $9174: Draws stretchy objects, such as trees
 *
 * \param[in] state      Pointer to game state.
 * \param[in] Bdepth     Depth scale index. (was B)
 * \param[in] DEstretchy An array of stretchy_t. (was DE)
 * \param[in] HLcallback Per-object draw callback. (was HL)
 * \param[in] IXxpos     Height-offset table pointer. (was IXxpos)
 * \param[in] IYheight   Road-table pointer. (was IYheight)
 */
static void draw_stretchy_object_common(chqstate_t       *state,
                                        int                Bdepth,
                                        const stretchy_t *DEstretchy,
                                        dso_callback_t   *HLcallback,
                                        const u16        *IXxpos,
                                        const u8         *IYheight)
{
  dso_callback_t        *SM_91CD_callback;      /* was $91CD (SM) */
  dso_callback_t        *SM_9244_callback;      /* was $9244 (SM) */
  int                    counter;               /* was A */
  int                    A;
  int                    SM_91DB_vertical;      /* was $91DB (SM) */
  const stretchy_t      *HLstretchy;            /* was HL */
  u16                    DEbitmapoffset;
  int                    C_total;
  u16                    SM_91BA_bitmap_offset; /* was $91BA (SM) */
  int                    Bstretchy_type;
  const depthset_t      *DEdepthset;
  const depthset_t      *HLdepthset;
  const bitmap_t        *DEbitmap;
  const depthset_pair_t *HLpair;
  int                    Apairdepth;
  u16                    HLoffset;
  const bitmap_t        *HLbitmap;
  int                    Bwidthbytes;
  int                    Bpairdepth;
  int                    Avertical;
  int                    Bvertical;

  // These just duplicate the HLcallback arg so could be removed in time.
  SM_91CD_callback = HLcallback;
  SM_9244_callback = HLcallback;

  // "Scale down" pattern
  counter = state->fast_counter & 0xE0;
  // Scale 0..223 (in steps of 16) to 0..153, reducing A by 31.25%, mapping the
  // incoming value to the 8x22 byte tables. So fast_counter indexes the rows of
  // the table.
  counter = COUNTER_TO_PERSP_Y_ROW(counter);
  SM_91DB_vertical = persp_y_scale[counter / 22][0];
  HLstretchy = DEstretchy; // was EX DE,HL
  DEbitmapoffset = MIN(Bdepth,
                       DEPTHSET_MAX) * 2 - 1; // prob 1-indexed so the -1 is +1
  C_total = 0; // increases with loop
  SM_91BA_bitmap_offset = DEbitmapoffset;

  for (;;) {
    state->doc_col_pos = -C_total;
    // read "n" count byte from graphic stream, e.g. stretchy_shortpole + 0
    Bstretchy_type = HLstretchy->type - 1;
    if (Bstretchy_type == 0)
      return; /* Return if count byte was terminator STRETCHY_TYPE_END (1) */

    //HL++; - replaced
    DEdepthset = HLstretchy->set;
    //HL += 2; - replaced
    HLstretchy++; // Replaces nearby decrements
    // PUSH HLstretchy/IXxpos/(Bstretchy_n,C_total)
    HLdepthset = DEdepthset; // was EX DE,HL
    DEbitmap = HLdepthset->bitmaps;
    HLpair = &HLdepthset->pairs[(SM_91BA_bitmap_offset - 1) /
                                2]; // use of BC removed here
    Apairdepth = HLpair->depth;
    // HL++; - replaced
    HLoffset = HLpair->offset; // loads byte and widens to HL
    HLbitmap = &DEbitmap[HLoffset / 7]; // undo table built-in offset scale
    // POP (Bstretchy_n,C_total)
    if (--Bstretchy_type)
      break;

    // Type 2
    Bwidthbytes = HLbitmap->width_bytes - 2;
    // PUSH (Bwidthbytes,C_total)
    Bpairdepth = Apairdepth;
    SM_91CD_callback(state, Bpairdepth, HLbitmap, IXxpos, IYheight);

dso_loop_continue:
    // POP (Bwidthbytes,C_total)
    C_total += Bwidthbytes;
    // POP IXxpos/HLstretchy
  }

  // EX AF,AF' -- save Adepth

  Avertical = SM_91DB_vertical;
  // Conv: Dispatch ladder converted to switch.
  // Bstretchy_type = data_type - 2 (two decrements already applied).
  switch (Bstretchy_type) {
  default: assert(0);
  case 1: goto dso_case_150pc;  /* data type STRETCHY_TYPE_150PC */
  case 2: goto dso_case_50pc;   /* data type STRETCHY_TYPE_50PC */
  case 3: goto dso_case_113pc;  /* data type STRETCHY_TYPE_113PC */
  case 4: goto dso_case_38pc;   /* data type STRETCHY_TYPE_38PC */
  case 5: goto dso_case_75pc;   /* data type STRETCHY_TYPE_75PC */
  case 6: goto dso_case_25pc;   /* data type STRETCHY_TYPE_25PC */
  case 7: goto dso_continue;    /* data type STRETCHY_TYPE_100PC */
  }
  Avertical *= 2;
  goto dso_continue;

dso_case_25pc:
  // Scale A to 25%
  Avertical >>= 2;
  goto dso_continue;

dso_case_75pc:
  // Scale A to 75%
  Avertical = (Avertical >> 1) + (Avertical >> 2);
  goto dso_continue;

dso_case_38pc:
  // Scale A to 37.5%
  Avertical = (Avertical >> 1) - (Avertical >> 3);
  goto dso_continue;

dso_case_113pc:
  // Scale A to 112.5%
  Avertical += (Avertical >> 3);
  goto dso_continue;

dso_case_50pc:
  // Scale A to 50%
  Avertical = (Avertical >> 1);
  goto dso_continue;

dso_case_150pc:
  // Scale A to 150%
  Avertical += (Avertical >> 1);

dso_continue:
  Avertical -= C_total;
  if ((s8) Avertical < 0)
    Avertical = 1;

  Bvertical = Avertical;
  // PUSH BC
  A = IYheight[53] + 1 - C_total - Bvertical;
  if (A < Bvertical) // was carry
    Bvertical += A;

  state->doc_rows_main = Bvertical;
  state->doc_rows_2nd = HLbitmap->width_bytes - 2;
  state->doc_inverted = 2; // inverted

  // EX AF,AF' -- restore Adepth

  Bpairdepth = Apairdepth; // might be Bwidthbytes?
  SM_9244_callback(state, Bpairdepth, HLbitmap, IXxpos,
                   IYheight); // does this update B?
  Bwidthbytes =
    0; // Conv: added this - TODO will need to test this behaviour since B is used to advance Ctotal
  state->doc_inverted = 0;
  goto dso_loop_continue;
}

/**
 * $924D: Draw tunnel light (left)
 *
 * \param[in] state    Pointer to game state.
 * \param[in] Bdepth   Parameter.
 * \param[in] DEarg    Parameter to pass in DE.
 * \param[in] IXxpos   X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
void draw_tunnel_light_left(chqstate_t *state,
                            int          Bdepth,
                            const void *DEarg,
                            const u16  *IXxpos,
                            const u8   *IYheight)
{
  draw_tunnel_light_common(state, Bdepth, DEarg, draw_object_left_entrypt, IXxpos,
                           IYheight);
}

/**
 * $9252: Draw tunnel light (right)
 *
 * \param[in] state    Pointer to game state.
 * \param[in] Bdepth   Parameter.
 * \param[in] DEarg    Parameter to pass in DE.
 * \param[in] IXxpos   IXxpos register value.
 * \param[in] IYheight IYheight register value.
 */
void draw_tunnel_light_right(chqstate_t *state,
                             int          Bdepth,
                             const void *DEarg,
                             const u16  *IXxpos,
                             const u8   *IYheight)
{
  draw_tunnel_light_common(state, Bdepth, DEarg, draw_object_right_entrypt,
                           IXxpos, IYheight);
}

/**
 * $9255: Draw tunnel light common
 *
 * \param[in] state      Pointer to game state.
 * \param[in] Bdepth     Parameter.
 * \param[in] DEdepthset A depth set.
 * \param[in] HLcallback Object drawing callback.
 * \param[in] IXxpos     IXxpos register value.
 * \param[in] IYheight   IYheight register value.
 */
static void draw_tunnel_light_common(chqstate_t            *state,
                                     int                     Bdepth,
                                     const depthset_t      *DEdepthset,
                                     draw_object_entrypt_t *HLcallback,
                                     const u16             *IXxpos,
                                     const u8              *IYheight)
{
  int counter; /* was A */
  int A;

  if (Bdepth >= 16)
    return;

  // "Scale down" pattern
  counter = state->fast_counter & 0xE0;
  counter = COUNTER_TO_PERSP_Y_ROW(counter);
  A = persp_y_scale[counter / 22][Bdepth];
  A = (A >> 2) - A;

  // callback must need to take A
  HLcallback(state, A, Bdepth, DEdepthset, IXxpos,
             IYheight); // e.g. calls draw_object_left_entrypt
}

/**
 * $9278: Draw object (left)
 *
 * \param[in] state      Pointer to game state.
 * \param[in] Bdepth     Parameter.
 * \param[in] DEdepthset Parameter to pass in DE.
 * \param[in] IXxpos     X-position table pointer. (was IX)
 * \param[in] IYheight   Height table pointer. (was IY)
 */
void draw_object_left(chqstate_t *state,
                      int          Bdepth,
                      const void *DEdepthset, // a depthset_t *
                      const u16  *IXxpos,
                      const u8   *IYheight)
{
  draw_object_left_entrypt(state, 0, Bdepth, DEdepthset, IXxpos, IYheight);
}

/**
 * $9279: Draw object (left) entry point
 *
 * \param[in] state       Pointer to game state.
 * \param[in] Acol_offset Parameter.
 * \param[in] Bdepth      Parameter.
 * \param[in] DEdepthset  A depth set, e.g. turn_sign_left (a depthset_t)
 * \param[in] IXxpos      IXxpos register value.
 * \param[in] IYheight    IYheight register value.
 */
static void draw_object_left_entrypt(chqstate_t       *state,
                                     int                Acol_offset,
                                     int                Bdepth,
                                     const depthset_t *DEdepthset,
                                     const u16        *IXxpos,
                                     const u8         *IYheight)
{
  const depthset_t *ds;      /* was HL */
  const bitmap_t   *bitmaps; /* was DE */
  int               depth;   /* was Bdepth */
  const bitmap_t   *bitmap;  /* was HL */

  state->doc_col_pos = Acol_offset;

  if (Bdepth >= DEPTHSET_MAX) // FIXME should be just greater than?
    Bdepth = DEPTHSET_MAX;

  ds = DEdepthset; // EX DE,HL - save arg address

  bitmaps = ds->bitmaps; // loads address of e.g. turn_sign_bitmaps
  depth   = ds->pairs[Bdepth].depth;
  bitmap  = &bitmaps[ds->pairs[Bdepth].offset / 7];

  draw_object_left_stretchy_entrypt(state, depth, bitmap, IXxpos, IYheight);
}

/**
 * $9293: Draw object left stretchy entrypt
 *
 * \param[in] state    Pointer to game state.
 * \param[in] Bdepth   Parameter.
 * \param[in] HLbitmap Source bitmap data.
 * \param[in] IXxpos   IXxpos register value.
 * \param[in] IYheight IYheight register value.
 */
static void draw_object_left_stretchy_entrypt(chqstate_t     *state,
    int              Bdepth,
    const bitmap_t *HLbitmap,
    const u16      *IXxpos,
    const u8       *IYheight)
{
  int A;

  A = IXxpos[0] + 16;
  if (A < Bdepth)
    return;
  A -= Bdepth;

  draw_object_left_helicopter_entrypt(state, A, HLbitmap,
                                      IYheight); /* was FALLTHROUGH */
}

/**
 * $929A: Draw object left helicopter entrypt
 *
 * \param[in] state        Pointer to game state.
 * \param[in] Awidth_bytes Bitmap byte width.
 * \param[in] HLbitmap     Source bitmap data.
 * \param[in] IYheight     Height table pointer. (was IY)
 */
static void draw_object_left_helicopter_entrypt(chqstate_t     *state,
    int              Awidth_bytes,
    const bitmap_t *HLbitmap,
    const u8       *IYheight)
{
  int zero;
  int carry;
  int Cpadding;
  int Ebitmap_stride;
  int Bheight;
  int Dwidth_bytes;
  int Adash;
  int Fdash_zero;
  int Fdash_carry;

  if (Awidth_bytes < 8)
    return;

  Cpadding = 0;
  Ebitmap_stride = HLbitmap->width_bytes << 3;
  Awidth_bytes -= Ebitmap_stride;
  if ((s8) Awidth_bytes >= 0) {
    if (Awidth_bytes >= 8) {
      draw_object_perspective_entrypt(state, Awidth_bytes, Cpadding, HLbitmap,
                               IYheight); /* exit via */
      return;
    }

    Ebitmap_stride = HLbitmap->width_bytes;
    Awidth_bytes >>= 2;
    state->doc_shift_select = Awidth_bytes;
    Awidth_bytes = Ebitmap_stride - 1;
    Bheight = 1;
    Cpadding = 1;
  } else {
    Ebitmap_stride = HLbitmap->width_bytes;
    Awidth_bytes = (Awidth_bytes & 0xFC) >> 2;
    state->doc_shift_select = Awidth_bytes;
    carry = 0; RR(Awidth_bytes);
    Bheight = Awidth_bytes;
    Awidth_bytes += Ebitmap_stride;
    Awidth_bytes -= 33;
    if ((s8) Awidth_bytes <= 0)
      return;

    Dwidth_bytes = Awidth_bytes;
    Cpadding = Ebitmap_stride - Awidth_bytes;
    Awidth_bytes = Dwidth_bytes;
    Bheight = 1;
  }

  carry = HLbitmap->flags & BITMAPFLAG_MASKED; /* gets bit 0 */
  zero  = (HLbitmap->flags >> 1) == 0;
  if (zero) { /* check if not BITMAPFLAG_FLIPPED */
    draw_object_clipped(state,
                            zero,
                            carry,
                            Awidth_bytes,
                            Bheight,
                            Cpadding,
                            Ebitmap_stride,
                            HLbitmap,
                            IYheight); /* exit via */
  } else {
    Bheight--; // B's not used - suss
    Awidth_bytes++; // this goes into banked A which we're not passing - also suss
    Cpadding = 0;
    Adash = Awidth_bytes; Fdash_zero = zero;
    Fdash_carry = carry; // was EX AF,AF' -- bank A & carry?
    draw_object_common_flipped(state,
                               Bheight,
                               Cpadding,
                               Ebitmap_stride,
                               HLbitmap,
                               Adash,
                               Fdash_zero,
                               Fdash_carry,
                               IYheight); /* exit via */
  }
}

/**
 * $92E1: Draw object (right)
 *
 * \param[in] state      Pointer to game state.
 * \param[in] Bdepth     Parameter.
 * \param[in] DEdepthset Parameter to pass in DE.
 * \param[in] IXxpos     X-position table pointer. (was IX)
 * \param[in] IYheight   Height table pointer. (was IY)
 */
void draw_object_right(chqstate_t *state,
                       int          Bdepth,
                       const void *DEdepthset,
                       const u16  *IXxpos,
                       const u8   *IYheight)
{
  draw_object_right_entrypt(state, 0, Bdepth, DEdepthset, IXxpos, IYheight);
}

/**
 * $92E2: Draw object (right) entry point
 *
 * \param[in] state       Pointer to game state.
 * \param[in] Acol_offset Parameter.
 * \param[in] Bdepth      Parameter.
 * \param[in] DEdepthset  A depth set, e.g. turn_sign_right (a depthset_t)
 * \param[in] IXxpos      IXxpos register value.
 * \param[in] IYheight    IYheight register value.
 */
static void draw_object_right_entrypt(chqstate_t       *state,
                                      int                Acol_offset,
                                      int                Bdepth,
                                      const depthset_t *DEdepthset,
                                      const u16        *IXxpos,
                                      const u8         *IYheight)
{
  const depthset_t *ds;      /* was HL */
  const bitmap_t   *bitmaps; /* was DE */
  int               depth;   /* was Bdepth */
  const bitmap_t   *bitmap;  /* was HL */

  state->doc_col_pos = Acol_offset;

  if (Bdepth >= DEPTHSET_MAX) // FIXME should be just greater than?
    Bdepth = DEPTHSET_MAX;

  ds = DEdepthset; // EX DE,HL

  bitmaps = ds->bitmaps; // loads address of e.g. turn_sign_bitmaps
  depth   = ds->pairs[Bdepth].depth;
  bitmap  = &bitmaps[ds->pairs[Bdepth].offset / 7];

  draw_object_right_stretchy_entrypt(state, depth, bitmap, IXxpos, IYheight);
}

/**
 * $92FC: Draw object (right) stretchy entry point
 *
 * \param[in] state    Pointer to game state.
 * \param[in] Bdepth   Parameter.
 * \param[in] HLbitmap Source bitmap data.
 * \param[in] IXxpos   IXxpos register value.
 * \param[in] IYheight IYheight register value.
 */
static void draw_object_right_stretchy_entrypt(chqstate_t     *state,
    int              Bdepth,
    const bitmap_t *HLbitmap,
    const u16      *IXxpos,
    const u8       *IYheight)
{
  int Awidth_bytes;

  Awidth_bytes = IXxpos[0];
  if ((s8) Bdepth < 0) {
    Awidth_bytes += Bdepth;
  } else {
    if (Awidth_bytes < Bdepth)
      return;
    Awidth_bytes += Bdepth;
  }
  if (Awidth_bytes)
    draw_object_right_helicopter_entrypt(state, Awidth_bytes, HLbitmap, IYheight);
}

/**
 * $9309: Draw object right helicopter entrypt
 *
 * \param[in] state        Pointer to game state.
 * \param[in] Awidth_bytes Bitmap byte width.
 * \param[in] HLbitmap     Source bitmap data.
 * \param[in] IYheight     IYheight register value.
 */
static void draw_object_right_helicopter_entrypt(chqstate_t     *state,
    int              Awidth_bytes,
    const bitmap_t *HLbitmap,
    const u8       *IYheight)
{
  if (Awidth_bytes < 247)
    draw_object_perspective_entrypt(state, Awidth_bytes, 0, HLbitmap, IYheight);
}

/**
 * $930E: Draw object: compute perspective height/width and dispatch
 *
 * \param[in] state        Pointer to game state.
 * \param[in] Awidth_bytes Bitmap byte width.
 * \param[in] Cpadding     Cpadding.
 * \param[in] HLbitmap     Source bitmap data.
 * \param[in] IYheight     Height table pointer. (was IY)
 */
static void draw_object_perspective_entrypt(chqstate_t     *state,
                                     int              Awidth_bytes,
                                     int              Cpadding,
                                     const bitmap_t *HLbitmap,
                                     const u8       *IYheight)
{
  int carry = 0;
  int Bheight;
  int Ebitmap_stride;
  int Zflipped;
  int Cwidth_bytes;
  int Adash;
  int Fdash_zero;
  int Fdash_carry;

  Awidth_bytes >>= 2; /* was AND-RRCA-RRCA */
  state->doc_shift_select = Awidth_bytes;
  Awidth_bytes >>= 1; /* was RRA */
  Bheight = Awidth_bytes;
  Ebitmap_stride = HLbitmap->width_bytes;
  Awidth_bytes = 31 - Awidth_bytes;
  if (Ebitmap_stride >= Awidth_bytes)
    Awidth_bytes = Ebitmap_stride;
  // Conv: HLbitmap++ removed, now passed as-is into draw_object_common/_9333
  Zflipped = (HLbitmap->flags >> 1) == 0;
  if (Zflipped == 0) {
    draw_object_clipped(state,
                            Zflipped,
                            carry,
                            Awidth_bytes,
                            Bheight,
                            Cpadding,
                            Ebitmap_stride,
                            HLbitmap,
                            IYheight);
  } else {
    Cwidth_bytes = Awidth_bytes;
    Adash = Awidth_bytes; Fdash_zero = Zflipped;
    Fdash_carry = carry; // was EX AF,AF' -- banking A & carry
    Cpadding = Ebitmap_stride - Cwidth_bytes;
    draw_object_common_flipped(state,
                               Bheight,
                               Cpadding,
                               Ebitmap_stride,
                               HLbitmap,
                               Adash,
                               Fdash_zero,
                               Fdash_carry,
                               IYheight); /* was FALLTHROUGH */
  }
}

/**
 * $932B: Draw object common flipped
 *
 * \param[in] state             Pointer to game state.
 * \param[in] Bheight           Number of rows.
 * \param[in] Cpadding          Cpadding.
 * \param[in] Ebitmap_stride    Source bitmap data.
 * \param[in] HLbitmap          Source bitmap data.
 * \param[in] Adash_width_bytes Bitmap byte width.
 * \param[in] Fdash_zero        Fdash zero.
 * \param[in] Fdash_carry       Fdash carry.
 * \param[in] IYheight          IYheight register value.
 */
static void draw_object_common_flipped(chqstate_t     *state,
                                       int              Bheight,
                                       int              Cpadding,
                                       int              Ebitmap_stride,
                                       const bitmap_t *HLbitmap,
                                       int              Adash_width_bytes,
                                       int             Fdash_zero,
                                       int             Fdash_carry,
                                       const u8       *IYheight)
{
  int Awidth_bytes;
  int zero;
  int carry;

  state->doc_shift_select = ~state->doc_shift_select;

  Awidth_bytes = Adash_width_bytes; zero = Fdash_zero;
  carry = Fdash_carry; // was EX AF,AF' -- unbank A & carry? carry might be a is-masked flag

  draw_object_clipped(state,
                          zero,
                          carry,
                          Awidth_bytes,
                          Bheight,
                          Cpadding,
                          Ebitmap_stride,
                          HLbitmap,
                          IYheight);
}

/**
 * $9333: Draw object: clip to visible area and dispatch to plot function
 *
 * \param[in] state          Pointer to game state.
 * \param[in] zero_flipped   Zero flipped.
 * \param[in] carry_masked   Carry masked.
 * \param[in] Awidth_bytes   Bitmap byte width.
 * \param[in] Bheight        Number of rows.
 * \param[in] Cpadding       Cpadding.
 * \param[in] Ebitmap_stride Source bitmap data.
 * \param[in] HLbitmap       Source bitmap data.
 * \param[in] IYheight       IYheight register value.
 */
static void draw_object_clipped(chqstate_t     *state,
                                    int             zero_flipped,
                                    int             carry_masked,
                                    int              Awidth_bytes,
                                    int              Bheight,
                                    int              Cpadding,
                                    int              Ebitmap_stride,
                                    const bitmap_t *HLbitmap,
                                    const u8       *IYheight)
{
  int               carry;
  int               D_933D;
  int               Adash_iy_diff;
  int               Adash1;
  int               Adash2;
  int               Adash933D;
  int               Dheight;
  int               Diy_diff;
  int               Ay;
  int               D;
  int               BCpadding;
  int               carry_shifted;
  int               Adash_9395;
  const u8         *HLbitmap_data;
  int               BCwidth_bytes;
  int               Adash3;
  int               Bdash_height;
  int               Adash_type;
  int               IXjump_offset;
  plot_sprite_cb_t *BCdash_callback;
  int               By;
  int               DEbitmap_stride;
  u8               *HLdash_backbuf_addr;


  if (carry_masked) {
    // EX AF,AF'  - preserve carry while we double these args
    Cpadding *= 2;
    Ebitmap_stride *= 2;
    // EX AF,AF'  - restore
  }

  // EX AF,AF'  - bank Awidth_bytes & carry_masked
  // INC HL  - removed

  for (;;) {
    D_933D = state->doc_col_pos;
    Adash_iy_diff = IYheight[0] - IYheight[53];
    if (Adash_iy_diff)
      goto doc_9359;

doc_9347:
    Adash1 = IYheight[53] + D_933D;
    if ((s8) Adash1 < 0)
      return;

    // PUSH AF  -- push Adash & flags

    Adash1++;

    Dheight = HLbitmap->height; /* Conv: HLbitmap adjusted, this loads from bitmap.height */
    Adash1 -= Dheight;
    if ((s8) Adash1 >= 0) // was !carry
      Adash1 = 0;
    Adash1 += Dheight;
    // HLbitmap++; // now points at HLbitmap.data
    Diy_diff = 1;
    goto doc_9390;

doc_9359:
    if ((D_933D & (1 << 7)) == 0) // checking for +ve?
      Adash_iy_diff -= D_933D;
    else
      Adash_iy_diff += D_933D;

    Diy_diff = Adash_iy_diff;
    if ((s8) Adash_iy_diff <= 0)
      goto doc_9347;

    Adash1 = IYheight[53];

    // PUSH AF  -- push Adash & flags
    Adash2 = HLbitmap->height - 1 -
             Diy_diff; /* Conv: HLbitmap adjusted, this loads from bitmap.height */
    if ((s8) Adash2 >= 0) // was !carry
      break; // was goto _938d
    // POP AF - pop IYheight[53] to discard it?

    if (state->doc_inverted == 0) // set to 0 or 2
      return;

    // inverted

    Adash933D = state->doc_col_pos;
    Dheight = HLbitmap->height; // reads bitmap.height again
    Adash933D -= Dheight;
    if ((s8) Adash933D >= 0)
      return;
    state->doc_col_pos = Adash933D;

    Ay = state->doc_rows_main - Dheight;
    if ((s8) Ay <= 0)
      return;
    state->doc_rows_main = Ay;
  }

  // AF is pushed here

  Adash2++;
  Diy_diff++;
  // HLbitmap++; Conv: removed - points HL at bitmap.data

doc_9390:
  // PUSH BC,DE
  D = Adash1; // heightish value
  BCpadding = Cpadding; // was B = 0
  Adash_9395 = state->doc_shift_select;
  carry_shifted = Adash_9395 & 1; Adash_9395 >>= 1;
  HLbitmap_data = (carry_shifted) ? HLbitmap->shifted : HLbitmap->data;
  // was HL += 2; Conv: removed - points HL at bitmap.shifted

  HLbitmap_data += BCpadding;
  state->doc_bitmap_ptr = HLbitmap_data;
  BCwidth_bytes = Ebitmap_stride;
  Adash3 = D; // was POP AF  (restoring what was DE)
  HLbitmap_data += BCwidth_bytes *
                   Adash3; // Conv: multiplier routine replaced with single mul

  BCpadding = D; //  (B is zero here)  D is heightish value from above
  DEbitmap_stride = Ebitmap_stride; // widen

  // EXX - BANK - first banking op in this routine

  Bdash_height =
    Bheight; // was POP BC,AF  (restoring what was BC and the AF which is IYheight[53])

  // 0b_1111_LLLL_RRRC_CCCC so A holds ?RRRLLLL and B holds ???CCCCC
  HLdash_backbuf_addr = OFFSETTOBACKBUF(((Awidth_bytes & 0x0F) << 8) | (((
                                          Awidth_bytes & 0x70) << 1) + Bdash_height)); // might this overflow?
  assert(VALID_BACKBUF_PTR(HLdash_backbuf_addr));

  Adash_type = state->doc_inverted; // set to 0 or 2
  if (Adash_type) {
    if (--Adash_type)
      goto doc_93df;
    // EX AF,AF'  -- unbank flags (carry => masked) and Awidth_bytes
    if (carry_masked) {
      plot_masked_sprite_inverted(state,
                                  Awidth_bytes,
                                  HLdash_backbuf_addr,
                                  Bdash_height,
                                  DEbitmap_stride,
                                  HLbitmap_data); /* exit via */
      return;
    }
    goto unmasked_inverted;
  }

  // EX AF,AF'  -- unbank flags (carry => masked) and Awidth_bytes
  if (zero_flipped) { // Z set if flipped
    if (carry_masked)
      draw_part_plot_masked_sprite(state,
                                   Awidth_bytes,
                                   HLdash_backbuf_addr,
                                   Bdash_height,
                                   DEbitmap_stride & 0xFF, /* Conv: Original only used E' */
                                   HLbitmap_data); /* exit via */
    else
      plot_sprite(state,
                  Awidth_bytes,
                  HLdash_backbuf_addr,
                  Bheight,
                  DEbitmap_stride,
                  HLbitmap_data); /* exit via */
  } else {
    if (carry_masked)
      plot_masked_sprite_flipped(state,
                                 Awidth_bytes,
                                 HLdash_backbuf_addr,
                                 Bheight,
                                 DEbitmap_stride,
                                 HLbitmap_data); /* exit via */
    else
      plot_sprite_flipped(state,
                          Awidth_bytes,
                          HLdash_backbuf_addr,
                          Bheight,
                          DEbitmap_stride,
                          HLbitmap_data); /* exit via */
  }
  return;

doc_93df:
  // EX AF,AF'  -- unbank flags (carry => masked) and Awidth_bytes
  if (carry_masked)
    goto doc_9436;
  SRL(Awidth_bytes); // carry set if odd
  if (carry)
    goto plot_sprite_xxx_odd;

  IXjump_offset = (4 - Awidth_bytes) * 5;
  BCdash_callback = plot_sprite_even;

doc_do_set_callbacks:
  state->doc_plot_fn = BCdash_callback;
  state->doc_plot_fn_2 = BCdash_callback;

  // EXX - UNBANK

  for (;;) {
    Awidth_bytes = state->doc_rows_main - By;
    if ((s8) Awidth_bytes <= 0)
      break;

    state->doc_rows_main = Awidth_bytes;

    // EXX - BANK

    // call e.g. plot_sprite_even
    state->doc_plot_fn(state,
                       IXjump_offset,
                       HLdash_backbuf_addr,
                       Bheight,
                       DEbitmap_stride,
                       HLbitmap_data);

    HLbitmap_data = state->doc_bitmap_ptr;
    By = state->doc_rows_2nd;
  }

  Bdash_height = Awidth_bytes + By;

  // EXX - BANK

  plot_sprite_even(state,
                   IXjump_offset,
                   HLdash_backbuf_addr,
                   Bheight,
                   DEbitmap_stride,
                   HLbitmap_data); /* was exit via */
  return;

plot_sprite_xxx_odd:
  // this will need to become a jump offset and a new entry point for plot_sprite_odd is needed
  IXjump_offset = 5 * (3 - Awidth_bytes);
  BCdash_callback = plot_sprite_odd_entry;
  goto doc_do_set_callbacks;

doc_9436:
  IXjump_offset = (8 - Awidth_bytes) * 6; // plot_masked_sprite needs * 6
  state->doc_mask_bitmap_ptr = state->doc_bitmap_ptr;

  state->doc_mask_rows_2nd   = state->doc_rows_2nd;
  state->doc_mask_rows_main = state->doc_rows_main;

  // Bdash = 0xF; // mask [commented out - I can't see why this exists]

  // EXX - UNBANK

  DEbitmap_stride &= 0xFF; // clear top of DEbitmap_stride
  for (;;) {
    Ay = state->doc_mask_rows_main - Bheight; // y offset?
    if ((s8) Ay > 0) {
      state->doc_mask_rows_main = Ay;

      plot_masked_sprite(state,
                         IXjump_offset,
                         Bheight,
                         DEbitmap_stride,
                         HLbitmap_data,
                         HLdash_backbuf_addr);

      HLbitmap_data = state->doc_mask_bitmap_ptr;
      Bheight = 0;
    } else {
      Bheight += Ay;
      plot_masked_sprite(state,
                         IXjump_offset,
                         Bheight,
                         DEbitmap_stride,
                         HLbitmap_data,
                         HLdash_backbuf_addr); /* exit via */
      return;
    }
  }

unmasked_inverted:
  DEbitmap_stride = Ebitmap_stride;
  HLbitmap_data += (Bheight - 1) * DEbitmap_stride;
  DEbitmap_stride = -DEbitmap_stride;

  plot_sprite(state,
              Awidth_bytes,
              HLdash_backbuf_addr,
              Bheight,
              DEbitmap_stride,
              HLbitmap_data); /* was FALLTHROUGH */
}

/**
 * $949C: Plot an unmasked sprite
 *
 * This function draws the given bitmap to the back buffer.
 *
 * \param[in] state         Pointer to game state.
 * \param[in] width_bytes   Draw width of bitmap data, in bytes. (was A)
 * \param[in] backbuf_addr  Back buffer address to draw at. (was HL)
 * \param[in] height        Number of rows to draw. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was DE')
 * \param[in] bitmap_data   Source bitmap data. (was HL')
 */
static void plot_sprite(chqstate_t *state,
                        int          width_bytes,
                        u8         *backbuf_addr,
                        int          height,
                        int         bitmap_stride,
                        const u8   *bitmap_data)
{
  int odd;         /* was carry */
  int jump_offset; /* was IX */

  assert(width_bytes >= 1);
  assert(VALID_BACKBUF_PTR(backbuf_addr));
  assert(height >= 1);
  assert(bitmap_data != NULL);

  odd = width_bytes & 1;
  width_bytes >>= 1;
  if (odd) {
    plot_sprite_odd(state,
                    width_bytes,
                    backbuf_addr,
                    height,
                    bitmap_stride,
                    bitmap_data);
  } else {
    jump_offset = 5 * (4 - width_bytes); // 5 bytes/op
    plot_sprite_even(state,
                     jump_offset,
                     backbuf_addr,
                     height,
                     bitmap_stride,
                     bitmap_data);
  }
}

/**
 * $94B1: Plot an unmasked sprite (for even byte widths)
 *
 * This function draws the given bitmap to the back buffer.
 *
 * \param[in] state         Pointer to game state.
 * \param[in] jump_offset   Jump table byte offset (e.g. N * 5). (was IX)
 * \param[in] backbuf_addr  Back buffer address to draw at. (was HL)
 * \param[in] height        Number of rows to draw. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was DE')
 * \param[in] bitmap_data   Source bitmap data. (was HL')
 */
static void plot_sprite_even(chqstate_t *state,
                             int         jump_offset,
                             u8         *backbuf_addr,
                             int          height,
                             int         bitmap_stride,
                             const u8   *bitmap_data)
{
  const u8 *src;          /* was SP */
  u8       *backbuf_orig; /* was A */

  assert(jump_offset % 5 == 0);
  assert(jump_offset / 5 >= 0 && jump_offset / 5 <= 3);
  assert(VALID_BACKBUF_PTR(backbuf_addr));
  assert(height >= 1);
  assert(bitmap_data != NULL);

  // Conv: B & C moved into prevbufrow
  // EXX - bank
  goto plot_sprite_even_start;

  for (;;) {
    // EXX - bank
    if (--height == 0)
      return;

    bitmap_data += bitmap_stride; // Advance to start of next row

plot_sprite_even_start:
    src = bitmap_data;
    // EXX - unbank
    backbuf_orig = backbuf_addr;
    switch (jump_offset / 5) {
    default:
      assert(0);
    case 0:
      // Conv: Original uses POP that loads 16 bits at a time
      *backbuf_addr++ = *src++;
      *backbuf_addr++ = *src++;
    case 1:
      *backbuf_addr++ = *src++;
      *backbuf_addr++ = *src++;
    case 2:
      *backbuf_addr++ = *src++;
      *backbuf_addr++ = *src++;
    case 3:
      *backbuf_addr++ = *src++;
      *backbuf_addr = *src++;
    }
    backbuf_addr = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(backbuf_orig)));
  }
}

/**
 * $94F2: Plot an unmasked sprite (for odd byte widths)
 *
 * This function draws the given bitmap to the back buffer.
 *
 * \param[in] state         Pointer to game state.
 * \param[in] width_bytes   Draw width of bitmap data, in bytes. (was A)
 * \param[in] backbuf_addr  Back buffer address to draw at. (was HL)
 * \param[in] height        Number of rows to draw. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was DE')
 * \param[in] bitmap_data   Source bitmap data. (was HL')
 */
static void plot_sprite_odd(chqstate_t *state,
                            int          width_bytes,
                            u8         *backbuf_addr,
                            int          height,
                            int         bitmap_stride,
                            const u8   *bitmap_data)
{
  int jump_offset; /* was IX */

  assert(width_bytes >= 0 && width_bytes <= 3);
  assert(VALID_BACKBUF_PTR(backbuf_addr));
  assert(height >= 1);
  assert(bitmap_data != NULL);

  jump_offset = 5 * (3 - width_bytes); // 5 bytes/op

  plot_sprite_odd_entry(state,
                        jump_offset,
                        backbuf_addr,
                        height,
                        bitmap_stride,
                        bitmap_data); /* was FALLTHROUGH */
}

// Direct entry point for plot_sprite_odd
static void plot_sprite_odd_entry(chqstate_t *state,
                                  int         jump_offset,
                                  u8         *backbuf_addr,
                                  int          height,
                                  int         bitmap_stride,
                                  const u8   *bitmap_data)
{
  const u8 *src;          /* was SP */
  u8       *backbuf_orig; /* was A */

  assert(jump_offset % 5 == 0);
  assert(jump_offset / 5 >= 0 && jump_offset / 5 <= 3);
  assert(VALID_BACKBUF_PTR(backbuf_addr));
  assert(height >= 1);
  assert(bitmap_data != NULL);

  // Conv: B & C moved into prevbufrow
  // EXX - bank
  goto plot_sprite_odd_start;

  for (;;) {
    // EXX - bank
    if (--height == 0)
      return;

    bitmap_data += bitmap_stride;

plot_sprite_odd_start:
    src = bitmap_data;
    // EXX - unbank
    backbuf_orig = backbuf_addr;
// ps_odd_jumptable:
    switch (jump_offset / 5) {
    default:
      assert(0);
    case 0:
      // Conv: Original uses POP that loads 16 bits at a time
      *backbuf_addr++ = *src++;
      *backbuf_addr++ = *src++;
    case 1:
      *backbuf_addr++ = *src++;
      *backbuf_addr++ = *src++;
    case 2:
      *backbuf_addr++ = *src++;
      *backbuf_addr++ = *src++;
    case 3:
      *backbuf_addr = *src++;
    }
    backbuf_addr = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(backbuf_orig)));
  }
}

/**
 * $9542: Plot a flipped sprite
 *
 * This function draws the given bitmap to the back buffer while flipping it
 * horizontally.
 *
 * \param[in] state         Pointer to game state.
 * \param[in] width_bytes   Draw width of bitmap data, in bytes. (was A)
 * \param[in] backbuf_addr  Back buffer address to draw at. (was HL)
 * \param[in] height        Number of rows to draw. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was E')
 * \param[in] bitmap_data   Source bitmap data. (was HL')
 */
static void plot_sprite_flipped(chqstate_t *state,
                                int          width_bytes,
                                u8         *backbuf_addr,
                                int          height,
                                int          bitmap_stride,
                                const u8   *bitmap_data)
{
  int odd;         /* was carry */
  int jump_offset; /* was IX */

  assert(width_bytes >= 1);
  assert(VALID_BACKBUF_PTR(backbuf_addr));
  assert(height >= 1);
  assert(bitmap_data != NULL);

  backbuf_addr += width_bytes;

  odd = width_bytes & 1;
  width_bytes >>= 1;
  if (odd) {
    plot_sprite_flipped_odd(state, width_bytes, backbuf_addr, height,
                            bitmap_stride, bitmap_data);
    return;
  }

  // sprite has even width

  jump_offset = 9 * (4 - width_bytes); // 9 bytes/op
  plot_sprite_flipped_even(state,
                           jump_offset,
                           state->flipped,
                           backbuf_addr,
                           height,
                           bitmap_stride,
                           bitmap_data); /* was fallthrough */
}

/**
 * $9565: Plot a flipped sprite (for even byte widths)
 *
 * This function draws the given bitmap to the back buffer while flipping it
 * horizontally.
 *
 * \param[in] state         Pointer to game state.
 * \param[in] jump_offset   Jump table byte offset (e.g. N * 9). (was IX)
 * \param[in] flip_table    Flipped byte look-up table. (was DE)
 * \param[in] backbuf_addr  Back buffer address to draw at. (was HL)
 * \param[in] height        Number of rows to draw. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was DE')
 * \param[in] bitmap_data   Source bitmap data. (was HL')
 */
static void plot_sprite_flipped_even(chqstate_t *state,
                                     int         jump_offset,
                                     const u8   *flip_table,
                                     u8         *backbuf_addr,
                                     int          height,
                                     int         bitmap_stride,
                                     const u8   *bitmap_data)
{
  const u8 *src;          /* was SP */
  u8       *backbuf_orig; /* was A */

  assert(jump_offset % 9 == 0);
  assert(jump_offset / 9 >= 0 && jump_offset / 9 <= 3);
  assert(flip_table != NULL);
  assert(VALID_BACKBUF_PTR(backbuf_addr));
  assert(height >= 1);
  assert(bitmap_data != NULL);

  goto plot_sprite_flipped_even_start;

  for (;;) {
    // EXX - UNBANK
    if (--height == 0)
      return;

    bitmap_data += bitmap_stride;

plot_sprite_flipped_even_start:
    src = bitmap_data;
    // EXX - BANK
    backbuf_orig = backbuf_addr;
    // EX AF,AF' - BANK
    switch (jump_offset / 9) {
    default:
      assert(0);
    case 0:
      *backbuf_addr-- = flip_table[*src++];
      *backbuf_addr-- = flip_table[*src++];
    case 1:
      *backbuf_addr-- = flip_table[*src++];
      *backbuf_addr-- = flip_table[*src++];
    case 2:
      *backbuf_addr-- = flip_table[*src++];
      *backbuf_addr-- = flip_table[*src++];
    case 3:
      *backbuf_addr-- = flip_table[*src++];
      *backbuf_addr-- = flip_table[*src++];
    }
    // EX AF,AF' - UNBANK
    backbuf_addr = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(backbuf_orig)));
  }
}

/**
 * $95B3: Plot a flipped sprite (for odd byte widths)
 *
 * This function draws the given bitmap to the back buffer while flipping it
 * horizontally.
 *
 * \param[in] state         Pointer to game state.
 * \param[in] width_bytes   Draw width of bitmap data, in bytes. (was A)
 * \param[in] backbuf_addr  Back buffer address to draw at. (was HL)
 * \param[in] height        Number of rows to draw. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was E')
 * \param[in] bitmap_data   Source bitmap data. (was HL')
 */
static void plot_sprite_flipped_odd(chqstate_t *state,
                                    int          width_bytes,
                                    u8         *backbuf_addr,
                                    int          height,
                                    int          bitmap_stride,
                                    const u8   *bitmap_data)
{
  int       jump_offset;  /* was IX */
  const u8 *src;          /* was SP */
  u8       *backbuf_orig; /* was A */

  assert(width_bytes >= 0 && width_bytes <= 3);
  assert(VALID_BACKBUF_PTR(backbuf_addr));
  assert(height >= 1);
  assert(bitmap_data != NULL);

  width_bytes++;
  jump_offset = 4 - width_bytes; // 9 bytes/op mult - removed

  // Conv: Removed D' the flipped bytes table ptr
  // EXX - bank
  // Ddash = 0;  widen bitmap_stride to DE'

  goto psf_odd_body;

  for (;;) {
    // EXX - bank
    if (--height == 0)
      return;

    bitmap_data += bitmap_stride;

psf_odd_body:
    src = bitmap_data;
    // EXX - unbank
    backbuf_orig = backbuf_addr;
    switch (jump_offset) {
    default:
      assert(0);
    case 0:
      // Conv: Original uses POP that loads 16 bits at a time
      *backbuf_addr-- = state->flipped[*src++];
      *backbuf_addr-- = state->flipped[*src++];
    case 1:
      *backbuf_addr-- = state->flipped[*src++];
      *backbuf_addr-- = state->flipped[*src++];
    case 2:
      *backbuf_addr-- = state->flipped[*src++];
      *backbuf_addr-- = state->flipped[*src++];
    case 3:
      *backbuf_addr-- = state->flipped[*src++];
    }
    backbuf_addr = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(backbuf_orig)));
  }
}

/**
 * $961B: Pseduo-random number generator
 *
 * \param[in] state Pointer to game state.
 * \return Pseduo-random byte
 */
static u8 rng(chqstate_t *state)
{
  int carry;
  u8 *seed; /* was HL */
  u8  A;

  seed = &state->rng_seed[0];
  A = *seed - 141;
  *seed++ = A;
  *seed += 3;
  A += *seed++;
  RRC(A);
  RRC(*seed);
  A += *seed;
  *seed = A;
  return A;
}

/**
 * $9945: Start chatter
 *
 * \param[in] state      Pointer to game state.
 * \param[in] priority   Priority of this chatter (higher wins). (was A)
 * \param[in] chatterblk Pointer to chatter data block. (was HL)
 */
static void start_chatter(chqstate_t       *state,
                          chatterpriority_t priority,
                          const u8         *chatterblk)
{
  int chatter_state; /* was A */

  assert(chatterblk);

  chatter_state = state->chatter_state;
  if (chatter_state != CHATTERSTATE_IDLE &&
      chatter_state < CHATTERSTATE_STOP)
    if (state->chatter_priority >= priority)
      return;

  state->chatter_priority = priority;
  state->chatterblk_ptr   = chatterblk;
  state->chatter_delay    = 0;
  state->chatter_state    = CHATTERSTATE_START;
}

/**
 * $9965: Drive chatter
 *
 * \param[in] state Pointer to game state.
 */
static void drive_chatter(chqstate_t *state)
{
  int          carry = 0;
  int          chatter_state; /* was A */
  char         character;     /* was D */
  u8           rotating;      /* was A */
  int          delay;         /* was A */
  int          x;             /* was A */
  u8           B;
  const char  *HLnextchar;    /* was HL */
  const u8    *chatterblk;    /* was HL */
  int          chattercmd;    /* was A */

  chatter_state = state->chatter_state;
  if (--chatter_state == 0) // starting (1)
    goto starting;
  if (--chatter_state == 0) // displaying (2)
    goto do_noise_effect;
  if (--chatter_state == 0) { // stopping (3)
    if (--state->noise_counter) {
      draw_noise_effect(state, state->noise_counter); /* exit via */
      return;
    }

    // enter idle state, hide face by wiping attrs to black on black
    state->chatter_state = CHATTERSTATE_IDLE;
    ne_plot_attrs(state, 0 /* black on black attrs */);
  }

  // idle state (0)

  character = ' ';
  rotating = state->chatter_cursor_blink; // Conv: Was self modified
  RRC(rotating);
  state->chatter_cursor_blink = rotating;
  x = 0xFF; // ie -1
  if (carry)
    plot_mini_font_cursor_on(state, x, character); /* exit via */
  else
    plot_mini_font_cursor_off(state, x, character); /* exit via */
  return;

do_noise_effect:
  if (state->noise_counter) {
    drive_noise_effect(state, state->noise_counter); /* exit via */
    return;
  }
  delay = state->chatter_delay;
  if (delay == 0)
    goto clear_line;
  delay--;
  state->chatter_delay = delay;
  B = state->chatter_delay;
  if (delay == 0)
    goto read_message;
  HLnextchar = state->next_character - 1; // addr of next char
  character = *HLnextchar & ~EOS; // load char and clear any terminator
  x = state->message_x - 1;
  RR(B);
  if (carry)
    plot_mini_font_cursor_on(state, x, character); /* exit via */
  else
    plot_mini_font_cursor_off(state, x, character); /* exit via */
  return;

clear_line:
  x = state->message_x;
  if (x) {
    pc_clear_line(state, x); /* exit via */
    return;
  }

read_message:
  chatterblk = state->chatterblk_ptr;
  chattercmd = *chatterblk;
  if (chattercmd == CHATTERCMD_STOP) {
    drive_chatter_stop(state);
    return;
  }
  if (chattercmd != CHATTERCMD_PAUSE) {
    pc_chatter_message(state, chatterblk); /* exit via */
    return;
  }
  // Conv: The next byte is no longer an address but an index into table of
  // chatter blocks.
  state->chatterblk_ptr = chatter_blocks[*++chatterblk];
  goto clear;

  // Conv: drive_chatter_stop lived here.

starting:
  state->chatter_state = CHATTERSTATE_RUN;

clear:
  clear_message_line(state);
  drive_noise_effect(state, 4); /* exit via */
}

/**
 * $99D3: Stop chatter
 *
 * \param[in] state Pointer to game state.
 */
static void drive_chatter_stop(chqstate_t *state)
{
  state->noise_counter = 4;
  state->chatter_state = CHATTERSTATE_STOP;
  clear_message_line(state); /* exit via */
}

/**
 * $99EC: Print chatter
 *
 * This function examines the current chatter block to determine which
 * character is speaking, handling random choice logic for three-way
 * selections. It calculates and displays the appropriate character mugshot
 * at screen position (176,8) based on the character ID, with special handling
 * for the pilot character. Finally, it outputs the associated message using
 * the processed chatter block data.
 *
 * \param[in] state Pointer to game state.
 */
static void print_chatter(chqstate_t *state)
{
  const u8 *chatterblk; /* was HL */
  int       cmd;        /* was A */
  int       rnd;        /* was A */
  const u8 *face;       /* was HL */

  chatterblk = state->chatterblk_ptr;
  assert(chatterblk);

  for (;;) {
    cmd = *chatterblk++; /* read a command (0xFC) or speaking character's ID */
    if (cmd != CHATTERCMD_RANDOM)
      break;

    /* Three-way random choice */
    rnd = rng(state);
    if (rnd >= 0x55) {
      chatterblk += 1; // Conv: was += 2 in Z80 (2-byte address), now 1-byte index
      if (rnd >= 0xAA)
        chatterblk += 1;
    }
    /* Conv: This is now an index and no longer an address */
    assert(*chatterblk < CHATTERBLK__LIMIT);
    chatterblk = chatter_blocks[*chatterblk];
  }

  /* cmd is now the character ID */
  assert(cmd <= CHATTERCHR_TONY);
  face = state->stage->addrof_perp_mugshot_bitmap;
  if (cmd != CHATTERCHR_PILOT)
    face = &bitmap_faces[(cmd - 1) * FACEBYTES]; // Conv: Simplified

  plot_face(state, 0x4036, face); /* Set screen plot address to (176,8) */

  pc_chatter_message(state, chatterblk); /* was FALLTHROUGH */
}

/**
 * $9A24: Print chatter
 *
 * \param[in] state      Pointer to game state.
 * \param[in] chatterblk Pointer to chatter data block. (was HL)
 */
static void pc_chatter_message(chqstate_t *state, const u8 *chatterblk)
{
  int         index;  // Conv: additional
  const char *string; /* was DE */

  // Conv: Original game loads an address directly here.
  assert(*chatterblk < CHATTERSTR__LIMIT);
  index = *chatterblk++;
  if (index < CHATTERSTR_PERP_DESC_1) {
    string = common_chatter_strings[index];
  } else {
    assert(index < CHATTERSTR__LIMIT);
    string = state->stage->chatter_strings[index - CHATTERSTR_PERP_DESC_1];
  }
  assert(string);
  state->chatterblk_ptr = chatterblk;
  state->next_character = string;
  pc_clear_line(state, 0); /* was FALLTHROUGH */
}

/**
 * $9A30: Clear the chatter line
 *
 * \param[in] state Pointer to game state.
 * \param[in] x     X position. (was A)
 */
static void pc_clear_line(chqstate_t *state, int x)
{
  const char *nextch;    /* was HL */
  char        character; /* was D */

  if (x == 0)
    clear_message_line(state);

  nextch = state->next_character;
  assert(nextch);
  character = *nextch & ~EOS; // remove any terminator
  assert(character >= ' ' && character < 'Z');
  plot_mini_font_cursor_on(state, x, character);
  if (*nextch++ & EOS) // if terminated
    state->chatter_delay = 10; // pause at end of string
  state->message_x = x + 1;
  state->next_character = nextch;
}

/**
 * $9A55: Drive the noise effect
 *
 * \param[in] state   Pointer to game state.
 * \param[in] counter Noise counter. (was A)
 */
static void drive_noise_effect(chqstate_t *state, int counter)
{
  state->noise_counter = --counter;
  if (counter == 0)
    print_chatter(state); /* exit via */
  else
    draw_noise_effect(state, counter); /* was FALLTHROUGH */
}

/**
 * $9A5C: Draw the noise effect
 *
 * \param[in] state   Pointer to game state.
 * \param[in] counter Noise counter. (was A)
 */
static void draw_noise_effect(chqstate_t *state, int counter)
{
  int   carry = 0;
  int   x;              /* was A */
  char  character;      /* was D */
  u16   DEscreen;       /* was DE */
  int   C;              /* was C */
  int   B;              /* was B */
  u16   DEscreen_saved; /* was stack? */
  u8   *noisebytes;     /* was HL */
  u8    A;              /* was A */

  RR(counter);
  x = 0xFF;
  character = ' ';
  if (!carry)
    plot_mini_font_cursor_on(state, x, character);
  else
    plot_mini_font_cursor_off(state, x, character);
  DEscreen = 0x4036; // Set plot address to (176,8)
  C = 40; // rows
  do {
    B = 4; // columns
    DEscreen_saved = DEscreen; /* was PUSH */
    noisebytes = &state->noise_bytes[0];
    do {
      A = *noisebytes - B;
      *noisebytes++ = A;
      RLC(A);
      A += *noisebytes;
      *noisebytes = A;
      state->speccy->screen.pixels[DEscreen - SCREEN_START_ADDRESS] = A;
      DEscreen++; /* was E++ */
    } while (--B > 0);
    DEscreen = DEscreen_saved; /* was POP - restore row ptr */
    DEscreen = nextscrrow(DEscreen);
  } while (--C > 0);

  ne_plot_attrs(state, attribute_BRIGHT_WHITE_OVER_BLACK);
  /* was FALLTHROUGH */
}

/**
 * $9A98: Set the noise attribute bytes
 *
 * \param[in] state Pointer to game state.
 * \param[in] attr  Attribute byte. (was A)
 */
static void ne_plot_attrs(chqstate_t *state, int attr)
{
  int addr;       /* was HL */
  int iterations; /* was B */

  // Screen attribute (22,1) (Conv: address -> offset)
  addr       = 0x5836 - SCREEN_START_ADDRESS;
  iterations = FACEATTRHEIGHT; // 5 rows
  do {
    // Conv: Screen write now goes via state.
    memset(&state->speccy->screen.pixels[addr], attr, FACEATTRWIDTH);
    addr += SCREEN_ATTRIBUTES_WIDTH;
  } while (--iterations > 0);
}

/**
 * $9AAB: Draw the given face
 *
 * \param[in] state  Pointer to game state.
 * \param[in] screen Screen address to draw at - a Z80 address, always 0x4036. (was DE)
 * \param[in] face   Face data to draw. (was HL)
 */
static void plot_face(chqstate_t *state,
                      int         screen,
                      const u8   *face)
{
  u16 saved_screen; /* was stack */
  int counter;      /* was BC */

  assert(screen >= SCREEN_START_ADDRESS && screen < SCREEN_END_ADDRESS);
  assert(face);

  counter = FACEBITMAPBYTES;
  saved_screen = screen;
  screen -= SCREEN_START_ADDRESS; // Conv: address -> offset
  for (;;) {
    state->speccy->screen.pixels[screen++] = *face++; counter--;
    state->speccy->screen.pixels[screen++] = *face++; counter--;
    state->speccy->screen.pixels[screen++] = *face++; counter--;
    state->speccy->screen.pixels[screen++] = *face++; counter--;
    screen -= 4; // replaces PUSH/POP
    if (counter == 0)
      break;
    screen = nextscrrow(screen);
  }

  plot_face_attributes(state, saved_screen, face); /* was fallthrough */
}

/**
 * $9ACE: Set the face attribute bytes
 *
 * \param[in] state  Pointer to game state.
 * \param[in] screen Screen address to draw at. (was stack)
 * \param[in] face   Face data to draw. (was HL)
 */
static void plot_face_attributes(chqstate_t *state,
                                 int         screen,
                                 const u8   *face)
{
  int carry;
  u8  A;       /* was A */
  int counter; /* was BC */

  A = screen >> 8;
  RRC(A);
  RRC(A);
  RRC(A);
  A &= 3; // extract band
  A += 0x58;
  screen = (A << 8) | (screen & 0xFF);
  screen -= SCREEN_START_ADDRESS; // Conv: address -> offset
  counter = FACEATTRBYTES;
  for (;;) {
    state->speccy->screen.pixels[screen++] = *face++; counter--;
    state->speccy->screen.pixels[screen++] = *face++; counter--;
    state->speccy->screen.pixels[screen++] = *face++; counter--;
    state->speccy->screen.pixels[screen++] = *face++; counter--;
    if (counter == 0)
      break;

    // TODO Hoist to next-attr-row macro?
    int t = (screen & 0xFF) + 0x1C;
    screen = (screen & 0xFF00) | (t & 0xFF);
    if (t >= 0x100)
      screen += 256;
  }
}

/**
 * $9AEC: Plot a character at horizontal position X with no cursor block
 *
 * \param[in] state     Pointer to game state.
 * \param[in] x         X position. (was A)
 * \param[in] character Character to draw. (was D)
 */
static void plot_mini_font_cursor_off(chqstate_t *state,
                                      int          x,
                                      char        character)
{
  pmf_go(state, x, character, ________, ________);
}

/**
 * $9AF1: Plot a character at horizontal position X with a cursor block
 *
 * \param[in] state     Pointer to game state.
 * \param[in] x         X position. (was A)
 * \param[in] character Character to draw. (was D)
 */
static void plot_mini_font_cursor_on(chqstate_t *state,
                                     int          x,
                                     char        character)
{
  pmf_go(state, x, character, _____XXX, X_______);
}

/**
 * $9AF4: Plot a character
 *
 * \param[in] state    Pointer to game state.
 * \param[in] x        X position. (was A)
 * \param[in] ascii    Character to draw. (was D)
 * \param[in] extrabm1 Additional bitmap data to draw. (was B)
 * \param[in] extrabm2 Additional bitmap data to draw. (was C)
 */
static void pmf_go(chqstate_t *state,
                   int          x,
                   char        ascii,
                   int          extrabm1,
                   int          extrabm2)
{
  int       carry = 0;

  int       extra2;   /* was self modified $9B61 - right extra bitmap */
  int       extra1;   /* was self modified $9B64 - left extra bitmap */
  int       mask;     /* was self modified $9B89 */
  int       rotate;   /* was self modified $96B7 */

  int       A;        /* was A */
  int       ascii2;   /* was A */
  int       row;      /* was A */
  int       gid;      /* was C */
  int       sgid;     /* was A */
  u16       screen;   /* was DE */
  const u8 *fontdata; /* was DE */
  u16       HLscreen; /* was HL */

#define MFWIDTH  (5)
#define MFHEIGHT (6)

  extra1 = extrabm1; // ORed with hi font bytes
  extra2 = extrabm2; // lo font bytes
  if (x == 0xFF) {
    screen = 0xBF; // low byte of screen addr
    A = MFWIDTH;
  } else {
    A = x * MFWIDTH + 2;
    screen = 0xC0; // low byte of screen addr
  }
  // A is now a scaled x position?

  // find rotate/shift by reducing scaled-x (A) until it's less than 8 bits
  do {
    screen++; /* was E++ */
    carry = 8 > A;
    A -= 8;
  } while (!carry);
  A = -A;

  rotate = A; // Conv: multiply by 4 removed
  mask   = 0xFF << A; // Conv: Loop replaced with shift

  // Turn ASCII into glyph IDs
  ascii2 = ascii; // remove ascii2 later
  screen |= 0x4500; // high byte of screen addr
  gid = 26;
  if (ascii2 == '.') goto pmf_have_glyph_id;
  gid++;
  if (ascii2 == ',') goto pmf_have_glyph_id;
  gid++;
  if (ascii2 == '!') goto pmf_have_glyph_id;
  gid++;
  if (ascii2 == ' ') goto pmf_have_glyph_id;
  gid++;
  if (ascii2 == '\'') goto pmf_have_glyph_id;
  if (ascii2 >= ';') { sgid = ascii2; goto pmf_have_ascii; }
  assert(0);
  gid += ascii2 - '/'; // not convinced this is ever used in the game

pmf_have_glyph_id:
  sgid = gid + 'A'; // Turn the glyph ID in #REGc into ASCII in #REGa

pmf_have_ascii:
  fontdata = &minifont[(sgid - 'A') * MFHEIGHT];
  HLscreen = screen; /* was EX */
  row = MFHEIGHT;
  do {
    u8 bm2; /* was C */
    u8 bm1; /* was B */

    bm2 = extra2;
    bm1 = *fontdata | extra1; // first pixel written

    if (0) {
      switch (rotate) {
      default: assert(0);
      case 0: SRL(bm1); RR(bm2);
      case 1: SRL(bm1); RR(bm2);
      case 2: SRL(bm1); RR(bm2);
      case 3: SRL(bm1); RR(bm2);
      case 4: SRL(bm1); RR(bm2);
      case 5: SRL(bm1); RR(bm2);
      case 6: SRL(bm1); RR(bm2);
      case 7: SRL(bm1); RR(bm2);
      case 8: break;
      }
    } else {
      // Conv: "modern" version
      unsigned bm = (bm1 << 8) | bm2;
      bm >>= (8 - rotate);
      bm1 = bm >> 8;
      bm2 = bm & 0xff;
    }

    u8 *screen2 = ADDRTOSCREEN(HLscreen); // Conv: added
    screen2[0] = (mask & screen2[0]) | bm1;
    screen2[1] = bm2;
    fontdata++;
    HLscreen = nextscrrow(HLscreen);
  } while (--row > 0);
}

/**
 * $9BA7: Clear the message line
 *
 * \param[in] state Pointer to game state.
 */
static void clear_message_line(chqstate_t *state)
{
  u16 screen; /* was HL */
  int rows;   /* was A */

  screen = 0x45C1; // Screen coordinate (8,53)
  rows   = 6;      // Clear six rows
  do {
    memset(ADDRTOSCREEN(screen + 1), 0, 29); // Conv: Replacing LDIR
    screen = nextscrrow(screen);
  } while (--rows);
}

/**
 * $9BCF: Handle "time up", countdown and continue
 *
 * This function handles timed events. When 15s or less remain then Nancy warns
 * that our heroes are running of time. When they do run out of time, and
 * sufficient credits remain, a 10s coundown timer and restart query are
 * presented along with a tick-tock sound effect. If restart is initiated the
 * game is partially reset and then continues.
 *
 * \param[in] state Pointer to game state.
 */
static void check_time_up(chqstate_t *state)
{
  const u8 *ptime_bcd;            /* was HL */
  int       time_up_state;        /* was A */
  int       time_bcd;             /* was A */
  char     *time_digits;          /* was DE */
  int       effect;               /* was B */
  int       remaining_subseconds; /* was H */
  int       remaining_seconds_x2; /* was L */
  int       seconds;              /* was A */
  int       hidigit;              /* was A */
  int       lodigit;              /* was L */

  if (state->perp_caught_phase > PERPCAUGHTPHASE_NONE ||
      state->transition_control == TRANSITIONCONTROL_FADE)
    return;

  ptime_bcd = &state->session.time_bcd;
  time_up_state = state->time_up_state;
  switch (time_up_state) {
  case TIMEUPSTATE_INIT:          break;
  case TIMEUPSTATE_CHECK_TIME_UP: goto check_time_up;
  case TIMEUPSTATE_CHECK_CREDITS: goto check_credits;
  case TIMEUPSTATE_CHECK_RESTART: goto check_restart;
  case TIMEUPSTATE_WAITING:       return;
  default: assert(0);
  }

  // Otherwise it's state 0

  if (*ptime_bcd == 0) {
    // Ran out of time
    state->time_up_state = TIMEUPSTATE_CHECK_TIME_UP;
    // Stop acceleration/brake/turbo/pause
    state->session.user_input_mask = USERINPUT_RIGHT |
                                     USERINPUT_LEFT |
                                     USERINPUT_FIRE |
                                     USERINPUT_QUIT;
    return;
  }

update_remaining_time:
  if (--state->session.time_sixteenths > 0)
    return;

  state->session.time_sixteenths = 15;
  state->session.time_bcd = time_bcd = DAA(state->session.time_bcd - 1, NULL);

  // When 15s remain Nancy warns that time is running out.
  if (time_bcd == 0x15)
    // Note: This passes time_bcd as the priority which is 21.
    start_chatter(state, time_bcd,
                  chatterblk_nancy_time_running_out); /* exit via */
  return;

check_time_up:
  if (*ptime_bcd != 0) {
    state->time_up_state = TIMEUPSTATE_INIT;
    state->session.user_input_mask = USERINPUTMASK_ALLOW_ALL;
    goto update_remaining_time;
  }

  setup_overlay_messages(state, &time_up_message[0]);
  if (state->speed > 0)
    return;

  state->time_up_state = TIMEUPSTATE_CHECK_CREDITS;
  play_speech_hook(state, SAMPLE_TIME_UP);

check_credits:
  if (state->transition_control > TRANSITIONCONTROL_STOP)
    return;

  if (state->credits == 0) {
    check_user_input_quit_key(state); /* exit via */
  } else {
    state->credits--;
    state->credit_n[7] = (state->credits + '0') | EOS;
    state->time_up_state = TIMEUPSTATE_CHECK_RESTART;
    state->tick_remaining_seconds_x2 = 21; // a 10 second countdown, doubled, plus 1
    state->tick_remaining_subseconds = 1;  // force an initial decrement
  }
  return;

check_restart:
  if (state->user_input & USERINPUT_FIRE) {
    // Reset mission
    state->time_up_state           = TIMEUPSTATE_INIT;
    state->smash_level             = 0;
    state->smash_counter           = 0;
    state->session.user_input_mask = USERINPUTMASK_ALLOW_ALL;
    state->gear_lockout            = 3;
    state->transition_control      = TRANSITIONCONTROL_FILL_ATTRIBUTES;
    state->session.turbos          = MAXTURBOS;
    state->session.time_bcd        = RESTART_TIME_BCD;
    state->retry_count++;

    play_start_noise(state);
    return;
  }

  setup_overlay_messages(state, &continue_messages[0]);

  // Conv: Original loads these two vars at once.
  remaining_seconds_x2 = state->tick_remaining_seconds_x2;
  remaining_subseconds = state->tick_remaining_subseconds;
  if (--remaining_subseconds == 0) {
    remaining_subseconds = 6; // game timing dependent
    --remaining_seconds_x2;

    // Play a "bip" or a "bow" sound effect every half second (this is why we double the countdown)
    effect = (remaining_seconds_x2 & 1) ? EFFECT_BIP : EFFECT_BOW;
    start_sfx(state, effect, 1); /* priority 1 => high */

    if (remaining_seconds_x2 == 0) {
      state->quit_state    = QUITSTATE_START;
      state->time_up_state = TIMEUPSTATE_WAITING;
    }
  }

  state->tick_remaining_seconds_x2 = remaining_seconds_x2;
  state->tick_remaining_subseconds = remaining_subseconds;

  seconds = remaining_seconds_x2 >> 1;

  time_digits = &state->time_nn[5]; // Load address of nn in "TIME nn"
  if (seconds == 10) {
    hidigit = '1'; // ASCII
    lodigit = 0;   // integer
  } else {
    lodigit = seconds; // integer
    hidigit = ' ';     // ASCII
  }

  time_digits[0] = hidigit; // write first digit (must be ASCII)
  time_digits[1] = (lodigit + '0') | EOS;
}

/**
 * $9C79: Extracted from above
 *
 * \param[in] state Pointer to game state.
 */
static void play_start_noise(chqstate_t *state)
{
  play_speech_hook(state, SAMPLE_START); /* exit via */
}

/**
 * $9CC2: Speed score
 *
 * \param[in] state Pointer to game state.
 */
static void speed_score(chqstate_t *state)
{
  int carry = 0;
  u16 speed; /* was HL */
  u8  A;
  u8  H;

  // The original code makes little sense...

  speed = state->speed;
  A = speed & 0xFF;
  H = speed >> 8; // Conv: added
  RR(H);
  RL(A);
  SRL(A);
  SRL(A);
  // E = A;
  A += carry;
  DAA(A, &carry);
  increment_score(state, A, 0, 0); /* exit via */
}

/**
 * $9CD6: Add bonus
 *
 * Bug/Limitation: As soon as a non-zero->zero transition is seen the routine.
 * finishes so you can only have a single run of zeroes in the bonus.
 *
 * \param[in] state Pointer to game state.
 * \param[in] lo    Lo. (was A)
 * \param[in] md    Md. (was E)
 * \param[in] hi    Hi. (was D)
 */
static void add_bonus(chqstate_t *state, int lo, int md, int hi)
{
  char *output;   /* was HL */
  int   zeroflag; /* was C */

  output = &state->bonus_string[6]; // points to byte after buffer
  zeroflag = 0xFF; // true until non-zero seen
  // This always runs since zeroflag is set
  (void) bonus_digit(lo >> 0, &zeroflag, &output);
  *output |= EOS; // terminate string

  // Conv: ab_high_nibble inlined in calls.
  // Using lazy evaluation here to avoid having a load of gotos
  (void)(bonus_digit(lo >> 4, &zeroflag, &output) >= 0 &&
         bonus_digit(md >> 0, &zeroflag, &output) >= 0 &&
         bonus_digit(md >> 4, &zeroflag, &output) >= 0 &&
         bonus_digit(hi >> 0, &zeroflag, &output) >= 0 &&
         bonus_digit(hi >> 4, &zeroflag, &output) >= 0);

  state->SM_address_of_score_digits = output;
  state->trigger_bonus_flag = 1;
  increment_score(state, lo, md, hi); /* was fallthrough */
}

/**
 * $9CFC: Subroutine of above broken out
 *
 * \param[in] digit    Digit.
 * \param[in] zeroflag Zeroflag.
 * \param[in] poutput  Poutput.
 * \return Non-zero on success.
 */
static int bonus_digit(int digit, int *zeroflag, char **poutput)
{
  digit &= 0x0F;

  if (digit != 0)
    goto non_zero;
  else if (*zeroflag != 0)
    goto store;
  else
    // Conv: Was a POP+JP to cause exit.
    return -1; // Non-zero-to-zero transition

non_zero:
  *zeroflag = 0; // Clear flag: non-zero digit seen
store:
  (*poutput)--;
  **poutput = digit + '0';
  return 0;
}

/**
 * $9D17: Increment score
 *
 * \param[in] state Pointer to game state.
 * \param[in] lo    Low digits.
 * \param[in] md    Middle digits.
 * \param[in] hi    High digits.
 */
static void increment_score(chqstate_t *state, int lo, int md, int hi)
{
  int carry = 0;
  u8 *score_bcd; /* was HL */

  score_bcd  = &state->score_bcd[0];
  *score_bcd = DAA(lo + *score_bcd,         &carry); score_bcd++;
  *score_bcd = DAA(md + *score_bcd + carry, &carry); score_bcd++;
  *score_bcd = DAA(hi + *score_bcd + carry, &carry); score_bcd++;
  *score_bcd = DAA(*score_bcd + carry, NULL);
}

/**
 * $9D2E: Calc overtake bonus
 *
 * \param[in] state Pointer to game state.
 */
static void calc_overtake_bonus(chqstate_t *state)
{
  int carry = 0;
  int counter;    /* was A */
  int iterations; /* was B */
  u8 *bcd;        /* was HL */

  counter = state->overtake_bonus_counter;
  if (counter == 0)
    return;

  iterations = counter;
  bcd = &state->overtake_bonus_bcd;
  // Increment bonus by 2 up to a max of 128.
  do {
    counter = DAA(*bcd + 2, &carry);
    if (counter >= 0x80) counter = 0x80;
    *bcd = counter;
    // Set bonus to N * 100.
    add_bonus(state, 0, counter, 0);
  } while (--iterations > 0);

  state->overtake_bonus_counter = 0;
}

/**
 * $9D62: Update scoreboard
 *
 * \param[in] state Pointer to game state.
 */
static void update_scoreboard(chqstate_t *state)
{
  toggle_light_brightness(state, ADDRTOATTRS(MARQUEELIGHT_LEFT_ATTR_ADDR));
  toggle_light_brightness(state, ADDRTOATTRS(MARQUEELIGHT_RIGHT_ATTR_ADDR));
  plot_turbos_and_digits(state);
}

/**
 * $9DF4: Toggle light brightness
 *
 * \param[in] state Pointer to game state.
 * \param[in] attrs Attribute address. (was HL)
 */
static void toggle_light_brightness(chqstate_t *state, u8 *attrs)
{
  int rows; /* was B */
  int attr; /* was C */

  rows = MARQUEELIGHT_HEIGHT;
  attr = ATTR_BRIGHT;
  do {
    *attrs++ ^= attr;
    *attrs++ ^= attr;
    *attrs++ ^= attr;
    *attrs++ ^= attr;
    *attrs   ^= attr;
    attrs += SCREEN_ATTRIBUTES_ROWBYTES - (MARQUEELIGHT_WIDTH - 1);
  } while (--rows > 0);
}

/**
 * $9E11: Plot turbos and digits
 *
 * \param[in] state Pointer to game state.
 */
static void plot_turbos_and_digits(chqstate_t *state)
{
  int        carry = 0;
  int        Aturbos;
  int        Cturbos;
  int        Aboost;
  const u8  *HLbitmap;
  int        Aframe;
  int        B;
  const u16 *SM_9e45;
  int        A;
  const u16 *SPbitmap;
  u8        *HLbackbuf;
  int        DEbitmap;
  u8        *DEscreen;
  int        DEdash_speed;
  int        Bdash_iterations;
  u8         Ascale;
  int        HLdash;
  int        BCdash;
  int        Ddash;
  int        Edash;
  u8        *DEbcd;
  int        HLdistance;
  int        BCdivisor;

  Aturbos = state->session.turbos;
  if (Aturbos) {

    Cturbos = Aturbos;
    Aboost = state->boost;
    HLbitmap = &bitmap_turbospin[0];
    if (Aboost == 0)
      goto ptas_turbo_setup;

    Aframe = state->turbo_spin_frame + 1;
    if (Aframe == 3)
      Aframe = 0;
    state->turbo_spin_frame = Aframe;
    if (Aframe == 0)
      goto ptas_turbo_setup;

    HLbitmap += 56 * Aframe;

ptas_turbo_setup:
    SM_9e45 = (const u16 *) HLbitmap; // local
    //SM_9e79 = SP; // save old SP

    A = 0xE1; // Low byte of back buffer draw address
    do {
      SPbitmap = (const u16 *) &bitmap_turbospin[0];
      Cturbos--;
      if (Cturbos == 0)
        SPbitmap = SM_9e45;
      Cturbos++;
      HLbackbuf = ADDRTOBACKBUF(0xFE00 | A);
      // EX AF,AF'
      B = TURBOHEIGHT;
      do {
        int Emask, Dbitmap;

        DEbitmap = *SPbitmap++; // POP DEbitmap
        Emask = DEbitmap & 0xFF;
        Dbitmap = DEbitmap >> 8;
        *HLbackbuf = (*HLbackbuf & Emask) | Dbitmap, HLbackbuf++;

        DEbitmap = *SPbitmap++; // POP DEbitmap
        Emask = DEbitmap & 0xFF;
        Dbitmap = DEbitmap >> 8;
        *HLbackbuf = (*HLbackbuf & Emask) | Dbitmap, HLbackbuf--;

        HLbackbuf = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(HLbackbuf)));
      } while (--B > 0);
      // EX AF,AF'
      A += 2;
    } while (--Cturbos > 0);

    // LD SP was here
  }

  DEscreen = ADDRTOSCREEN(0x4132); // speed digits pos (144,9)
  // EXX
  DEdash_speed = state->speed;

  // Scale speed by 82%
  HLdash = 0;
  Bdash_iterations = 7;
  Ascale = 82; // speed scale
  do {
    RL(Ascale);
    if (carry)
      HLdash += DEdash_speed;
    HLdash <<= 1;
  } while (--Bdash_iterations > 0);

  // Count 10,000s
  BCdash = 10000;
  Ddash = Edash = -1; // Conv: original inited both at once
  //A = 0; // clear carry?
  do {
    Ddash++;
    carry = (BCdash > HLdash), HLdash -= BCdash;
  } while (!carry);
  HLdash += BCdash; // correct overshoot

  // Count 1,000s
  //A = 0; // clear carry?
  BCdash = 1000;
  do {
    Edash++;
    carry = (BCdash > HLdash), HLdash -= BCdash;
  } while (!carry);
  HLdash += BCdash; // correct overshoot

  // Count 100s
  A = 0; // counter
  BCdash = 100;
  do {
    A++;
    carry = (BCdash > HLdash), HLdash -= BCdash;
  } while (!carry);
  A--; // correct for starting early

  // Plot speed digits
  DEscreen = ledfont_plot(state, Ddash, DEscreen); // draw 10,000s
  DEscreen = ledfont_plot(state, Edash, DEscreen); // draw  1,000s
  (void) ledfont_plot(state, A, DEscreen); // draw    100s

  // Time
  // EXX
  ptas_led_digits(state, 1, &state->session.time_bcd,
                  &state->session.time_digits[1],
                  ADDRTOSCREEN(0x412F)); // (120,9)

  // Distance (to perp)

  DEbcd = &state->distance_bcd[1];
  // hazard_lane_OR_perp_dist_hi is the high byte of the perp's distance
  HLdistance = (state->hazards[0].hazard_lane_OR_perp_dist_hi << 8) |
               state->hazards[0].distance;

  // Count 1,000s (no loop required)
  BCdivisor = 1000;
  carry = (BCdivisor > HLdistance), HLdistance -= BCdivisor;
  A = 0x10; // BCD
  if (carry) {
    HLdistance += BCdivisor; // correct overshoot
    A = 0x00; // BCD
  }

  // Count 100s
  BCdivisor = 100;
  do {
    A++;
    carry = (BCdivisor > HLdistance), HLdistance -= BCdivisor;
  } while (!carry);
  HLdistance += BCdivisor; // correct overshoot
  A--; // correct for starting early
  DEbcd[0] = A;

  // Count 10s
  BCdivisor = 10;
  // AND A
  A = 0xF0; // BCD
  do {
    A += 0x10;
    carry = (BCdivisor > HLdistance), HLdistance -= BCdivisor;
  } while (!carry);
  HLdistance += BCdivisor; // correct overshoot

  A |= HLdistance & 0xFF; // OR in remainder
  DEbcd[-1] = A;

  ptas_led_digits(state, 2, &state->distance_bcd[1],
                  &state->session.distance_digits[3],
                  ADDRTOSCREEN(0x4191)); /* was fallthrough */

  // Score

  ptas_led_digits(state, 4, &state->score_bcd[3], &state->session.score_digits[7],
                  ADDRTOSCREEN(0x4126)); /* was fallthrough */
}

/**
 * $9F1E: Ptas led digits
 *
 * \param[in] state      Pointer to game state.
 * \param[in] iterations Iterations. (was B)
 * \param[in] digits     Digits. (was DE)
 * \param[in] stored     Stored. (was HL)
 * \param[in] screen     Screen address. (was DE')
 */
static void ptas_led_digits(chqstate_t *state,
                            int          iterations,
                            const u8   *digits,
                            u8         *stored,
                            u8         *screen)
{
  int Adigits;
  int Cdigits;

  do {
    Adigits = *digits;
    Cdigits = Adigits; //tmp copy

    Adigits >>= 4;
    if (Adigits != *stored)
      goto ptas_led_plot_1st;
    screen++; // move screen pos

ptas_led_next_half:
    stored--;
    Adigits = Cdigits & 0x0F;
    if (Adigits != *stored)
      goto ptas_led_plot_2nd;
    screen++; // move screen pos

ptas_led_next_whole:
    stored--;
    digits--;
  } while (--iterations > 0);
  return;

ptas_led_plot_1st:
  *stored = Adigits;
  screen = ledfont_plot(state, Adigits, screen);
  goto ptas_led_next_half;

ptas_led_plot_2nd:
  *stored = Adigits;
  screen = ledfont_plot(state, Adigits, screen);
  goto ptas_led_next_whole;
}

/**
 * $9F47: Plot an LED font character
 *
 * \param[in] state  Pointer to game state.
 * \param[in] ord    Digit index 0..9. (was A)
 * \param[in] screen Back buffer screen address. (was DE')
 * \return Back buffer address of next character column.
 */
static u8 *ledfont_plot(chqstate_t *state, int ord, u8 *screen)
{
  const u8 *src;         /* was HL */
  u8       *screen_copy; /* was stacked */

  src = &ledfont[ord * LEDFONT_HEIGHT];
  screen_copy = screen;
  *screen = *src++; screen += 256;
  *screen = *src++; screen += 256;
  *screen = *src++; screen += 256;
  *screen = *src++; screen += 256;
  *screen = *src++; screen += 256;
  *screen = *src++; screen += 256;
  *screen = *src++;
  screen = screen_copy - 256 + 32;
  *screen = *src++; screen += 256;
  *screen = *src++; screen += 256;
  *screen = *src++; screen += 256;
  *screen = *src++; screen += 256;
  *screen = *src++; screen += 256;
  *screen = *src++; screen += 256;
  *screen = *src++; screen += 256;
  *screen = *src++;
  return screen_copy + 1; // move to next column
}

//0b_010BBLLL_RRRCCCCC (B = band, L = scanline, R = row (group), C = column)

/**
 * $9F99: Draw string with style
 *
 * \param[in] state   Pointer to game state.
 * \param[in] attrval Attribute value. (was A)
 * \param[in] attrs   Attribute address. (was BC)
 * \param[in] backbuf Back buffer address. (was DE)
 * \param[in] string  String data pointer. (was HL)
 * \param[in] style   Draw style. (was A')
 * \return Pointer past last character written.
 */
static const u8 *draw_string_with_style(chqstate_t *state,
                                        int          attrval,
                                        u8         *attrs,
                                        u8         *backbuf,
                                        const u8   *string,
                                        int          style)
{
  return draw_string_core(state,
                          backbuf,
                          string, /*HL*/
                          style, /*A'*/
                          attrval, /*C'*/
                          32, /*DE'*/
                          attrs/*HL'*/);
}

/**
 * $9FA3: Draw string
 *
 * \param[in] state   Pointer to game state.
 * \param[in] attrval Attribute value. (was A)
 * \param[in] attrs   Attribute address. (was BC)
 * \param[in] backbuf Back buffer address. (was DE)
 * \param[in] string  String data pointer. (was HL)
 * \return Pointer past last character written.
 */
static const u8 *draw_string_generic(chqstate_t *state,
                                     int          attrval,
                                     u8         *attrs,
                                     u8         *backbuf,
                                     const u8   *string)
{
  return draw_string_core(state,
                          backbuf,
                          string, /*HL*/
                          DRAWCHARSTYLE_GENERIC, /*A'*/
                          attrval, /*C'*/
                          32, /*DE'*/
                          attrs/*HL'*/);
}

/**
 * $9FA6: Draw string core
 *
 * Broken out from above.
 *
 * \param[in] state       Pointer to game state.
 * \param[in] backbuf     Back buffer address. (was DE)
 * \param[in] string      String data pointer. (was HL)
 * \param[in] style       Draw style. (was A')
 * \param[in] attrval     Attribute value. (was C')
 * \param[in] attrsstride Attribute stride. (was DE')
 * \param[in] attrs       Attribute address. (was HL')
 * \return Pointer past last character written.
 */
static const u8 *draw_string_core(chqstate_t *state,
                                  u8         *backbuf,
                                  const u8   *string,
                                  int          style,
                                  int          attrval,
                                  int          attrsstride,
                                  u8         *attrs)
{
  int character; /* was A */

  do {
    character = *string & ~EOS;
    draw_char(state, character, backbuf, style, attrval, attrsstride, attrs,
              &backbuf, &attrs);
  } while ((*string++ & EOS) == 0);

  return string;
}

/**
 * $9FB4: Draw char
 *
 * screen - screen address - was DE.
 * style - draw style - was A'.
 * attrval - attribute value - was C'.
 * new_screen - added.
 * new_attrs - added.
 *
 * \param[in] state      Pointer to game state.
 * \param[in] character  Character. (was A)
 * \param[in] screen     Screen address.
 * \param[in] style      Style.
 * \param[in] attrval    Attribute address.
 * \param[in] attrstride Attribute address. (was DE')
 * \param[in] attrs      Attribute address. (was HL')
 * \param[in] new_screen Screen address.
 * \param[in] new_attrs  Attribute address.
 */
static void draw_char(chqstate_t *state,
                      int          character,
                      u8         *screen,
                      int          style,
                      int          attrval,
                      int          attrstride,
                      u8         *attrs,
                      u8        **new_screen,
                      u8        **new_attrs)
{
  int       glyphid;    /* was C */
  int       data;       /* was A */
  int       iterations; /* was B */
  const u8 *fontdata;   /* was HL */
  u8       *orig;       /* was stacked */
  int       i;          // additional

  assert(screen);
  assert(style <= DRAWCHARSTYLE__LIMIT);
  assert(BACKBUFTOOFFSET(screen) >= 0);
  assert(BACKBUFTOOFFSET(screen) < BACKBUFFER_LENGTH);
  assert(SCREENTOOFFSET(attrs) >= SCREEN_ATTRIBUTES_START_ADDRESS -
         SCREEN_START_ADDRESS);
  assert(SCREENTOOFFSET(attrs) < SCREEN_LENGTH);

  character -= ' ';
  if (character == 0) {
    // Space
    screen++;
    attrs++;
    goto dc_return;
  }

  // Map ASCII to glyph IDs
  glyphid = 0x12;
  if (character >= ('A' - ' ')) goto dc_have_range;
  glyphid = 0x0B;
  if (character >= ('0' - ' ')) goto dc_have_range;
  glyphid = 0;
  character--;
  if (character == 0) goto dc_have_single;
  glyphid++;
  character -= 7;
  if (character == 0) goto dc_have_single;
  glyphid++;
  character--;
  if (character == 0) goto dc_have_single;
  glyphid++;
  character -= 3;
  if (character == 0) goto dc_have_single;
  glyphid++;
  goto dc_have_single;

dc_have_range:
  glyphid = character - glyphid;

dc_have_single:
  fontdata = &font[glyphid * 7]; // add symbol for glyph height

  /* Conv: if-else ladder replaced with switch. */
  switch (style) {
  case 1: goto dc_generic;
  case 2: goto dc_single_height;
  case 3: goto dc_double_height;
  case 4: goto dc_single_height_inverted;
  case 5: goto dc_double_height_inverted;
  case 0: break;
  default: assert(0);
  }

  // Otherwise it's type 0 or anything else
  orig = screen;
  iterations = 4;
  do {
    data = *fontdata++;
    *screen = data;
    screen += 256;
    *screen = data;
    screen += 256;
  } while (--iterations > 0);
  screen -= 8 * 256;
  screen += 32;
  iterations = 3;
  do {
    data = *fontdata++;
    *screen = data;
    screen += 256;
    *screen = data;
    screen += 256;
  } while (--iterations > 0);
  goto dc_set_double_attrs;

  // double height inverted
dc_double_height_inverted:
  orig = screen;
  iterations = 7;
  do {
    data = ~*fontdata++;
    *screen = data;
    screen += 256;
    *screen = data;
    screen += 256;
  } while (--iterations > 0);
  goto dc_set_double_attrs;

dc_single_height_inverted:
  orig = screen;
  iterations = 7;
  do {
    *screen = ~*fontdata++;
    screen += 256;
  } while (--iterations > 0);
  goto dc_set_single_attrs;

  // Plots double-height glyphs. screen->screen font->glyph def
dc_double_height:
  orig = screen;
  *screen = 0; // leave gap at top
  screen += 256;
  for (i = 0; i < 7; i++) { // Conv: rolled up
    *screen = *fontdata;
    screen += 256;
    *screen++ = *fontdata++; /* was LDI, could reuse A */
    screen--; /* was DEC E, could remove if screen++ above is dropped */
    screen += 256;
  }
  *screen = 0; // leave gap at bottom

dc_set_double_attrs:
  screen = orig + 1; /* was POP screen, INC E */
  *attrs |= attrval;
  attrs += attrstride;
  *attrs |= attrval;
  attrs -= attrstride; /* was POP attrs */
  attrs++; /* was INC L */
  goto dc_return;

dc_single_height: // seems to store 9 rows
  orig = screen;
  *screen = 0; // leave gap at top
  screen += 256;
  for (i = 0; i < 7; i++) { // Conv: rolled up
    *screen++ = *fontdata++;
    screen--; // could drop
    screen += 256;
  }
  *screen = 0; // leave gap at bottom

dc_set_single_attrs:
  screen = orig + 1; /* was POP screen, INC E */
  *attrs |= attrval;
  attrs++; /* was INC L */
  goto dc_return;

dc_generic:
  orig = screen;
  iterations = 7;
  do {
    *screen = *fontdata++;
    screen += 256;
    assert(0);
    // variation on nextscrrow()
    // screen = nextscrrow(screen); // won't work!
  } while (--iterations > 0);
  screen = orig + 1; /* was POP screen */

dc_return:
  *new_screen = screen;
  *new_attrs  = attrs;
}

/**
 * $A0D6: Keyscan
 *
 * \param[in] state Pointer to game state.
 * \return Non-zero on success.
 */
static u8 keyscan(chqstate_t *state)
{
  int carry = 0;
  int Ainput;
  int E;
  u8 *HL;
  u8  A;


  if (state->kempston_flag) {
    Ainput = state->speccy->in(state->speccy, port_KEMPSTON_JOYSTICK) & 0x1F;
    E = 0x20;
    HL = &state->keydefs[0];
    A = keyscan_keydefs(state, HL, E);
    RRC(A);
    RRC(A);
    RRC(A);
    A &= 0xE0;
    E = A | Ainput;
  } else {
    E = 1;
    HL = &state->keydefs[0];
    A = keyscan_keydefs(state, HL, E);
  }

  A &= 3;
  A = E; // FIX
  if (A == 3) {
    A &= 0xFC;
    E = A;
  }
  A &= 0x0C;
  if (A == 0x0C)
    A = E; // FIX
  A &= 0xF3;
  state->user_input = A;
  return 0;
}

/**
 * $A112: Scan a key-definition list, rotating each result into an accumulator
 *
 * \param[in] state Pointer to game state.
 * \param[in] HL    Hl.
 * \param[in] E     E.
 * \return Non-zero on success.
 */
static u8 keyscan_keydefs(chqstate_t *state, const u8 *HLkeydefs, int Eresult)
{
  int carry = 0;
  int A;

  do {
    A = *HLkeydefs++;
    carry = !keyscan_inner(state, A); // active low<>high
    RL(Eresult);
  } while (!carry);
  return Eresult;
  return 0;
}

static int keyscan_inner(const chqstate_t *state, int Ainput)
{
  int carry = 0;
  int Bport_shift; /* was B */
  int Ckey_shift;  /* was C */
  u8  Aport;       /* was A */
  u8  keys;        /* was A */

  assert(state->speccy);

  // Ainput = %RRRRRPPP where P is port shift and R is result shift (key)
  Bport_shift = (Ainput & 7) + 1;
  Ckey_shift  = 5 - (Ainput >> 3);
  Aport = 0xFE;
  do
    RRC(Aport);
  while (--Bport_shift > 0);
  keys = state->speccy->in(state->speccy, (Aport << 8) | 0xFE);
  do
    RR(keys);
  while (--Ckey_shift > 0);
  return carry;
}

/**
 * $A399: Check scenery collisions
 *
 * \param[in] state Pointer to game state.
 */
static void check_scenery_collisions(chqstate_t *state)
{
  int          carry = 0;
  int          HL;
  int          DE;
  int          fork_countdown; /* was A */
  s16          pos;            /* was HL' */
  int          offroad;        /* was A */
  int          Cdash;
  u8          *bufptr;         /* was HL' */
  u8           lanes;          /* was A */
  int          Z;
  u8           A;
  int          C;
  int          Adash;
  int          Aflip;
  int          HLdash;
  int          DEdash;
  int          BCdash;
  const obj_t *HLobj;
  int          Edash;
  int          obj;

  HL = 72;
  DE = 472; // road centre?

  // EXX - bank

  if (state->fork_visible) {
    fork_countdown = state->fork_countdown;
    if (fork_countdown == 0) {
      check_fork_scenery_collisions(state, HL, DE); /* exit via */
      return;
    }

    if (--fork_countdown == 0)
      return;
  }

  // Check left hand side
  //
  // "pos" here is (something like) a positive offset from the left edge of
  // the screen to the object. It's approx -186 .. 176 for (object fully
  // off-screen on the left to on-screen centred(?)).
  //
  // Note that is where an object *could be*. There's not necessarily an
  // object always there.
  pos = state->xpos_road_centre[127]; // signed
  if ((pos >> 8) != 0) {
    // object is visible(?)
    if (pos >= 64) {
      // reasonably close to hero car?
      if (pos < 106)
        goto check_right_hand; // not close enough to be off-road
      // how far off-road are we? partially/fully off-road is 1/2
      offroad = (pos < 133) ? 1 : 2;
      goto store_off_road;
    }
  }

  // If we don't arrive here we're close to the left hand object
  state->trigger_lane_change_sfx = 0;

  // Check right hand side
check_right_hand:
  // "pos" here is approx 75..368 for (centred .. off-screen on the right).
  pos = state->xpos_road_centre[126];
  offroad = 0;
  if ((pos >> 8) != 0) {
    if (pos < 190) {
      if (pos < 142)
        offroad = (pos < 124) ? 1 : 2; // as above
      goto store_off_road;
    }
  }

  // If we don't arrive here we're close to the right hand object
  state->trigger_passed_object_sfx = 0;

store_off_road:
  state->off_road =
    offroad; // 0/1/2 => on-road/one wheel off-road/both wheels off-road
  Cdash = 0;
  if (offroad == 0)
    goto csc_a43b;

  // Otherwise we're off-road.

  bufptr = ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET);
  lanes = *bufptr;
  if ((lanes & (1 << 6)) == 0) // tunnel??
    goto csc_a43b;
  RL(lanes); // test bit 7
  if (carry)
    goto csc_a43b;
  Z = ((lanes & (1 << 3)) == 0); // Test bit 3 (was bit 2 before RLA)
  A = state->scenedata.road_pos >> 8;
  if (!Z) {
    C = A; // save road_pos
    Adash = 20;
    // EX AF,AF'
    Aflip = C & 1;
    scenery_hit(state, Aflip, Adash); /* exit via */
    return;
  }

  HLdash = 209;
  DEdash = 405;
  // EXX - unbank
  C = (A) ? 2 : 1;

  // Hit tunnel wall.
  start_sfx(state, EFFECT_WALL_HIT, 4); /* priority 4 */

csc_a43b:
  A = C;
  state->ahc_crash_spin = A;
  // EXX - bank
  state->ahc_road_pos_a = HLdash;
  state->ahc_road_pos_b = DEdash;
  if (A)
    return;

  // -- RIGHT SIDE OBJECT HIT CHECKING --
  bufptr = ROADBUF_FWD2PTR(ROADBUF_RIGHTOBJS_OFFSET);
  // EX AF,AF' // FIXME stash road buffer offset here

  A = ROADBUF_FWD2IDX(0);
  RL(A);
  obj = *bufptr; // Read a right side object data byte
  if (carry)
    bufptr++; // FIXME needs wraparound
  obj |= *bufptr;
  if (obj == 0) // no object
    goto csc_check_left;

  HLobj = &state->stage->addrof_right_hand_objects[obj];
  // Read collision values.
  Cdash = HLobj->hit_max_or_min;
  Edash = HLobj->hit_min_or_max;
  A     = HLobj->impact_speed_cap;

  DEdash = Edash;
  BCdash = Cdash;

  // Check for collisions with scenery (right hand side).
  pos = state->xpos_road_centre[126]; // read pos
  if (pos >= BCdash)
    goto csc_check_left;
  if (pos < DEdash)
    goto csc_check_left;
  Adash = A; // EX AF,AF'  unbank(?) road buf offset
  Aflip = 0;
  csc_hit_scenery(state, Aflip, Adash); /* exit via */
  return;

csc_check_left:
  // EX AF,AF'  Unbank road buffer offset or/and bank mystery value in A

  // -- LEFT SIDE OBJECT HIT CHECKING --
  bufptr = ROADBUF_FWD2PTR(ROADBUF_LEFTOBJS_OFFSET);

  A = ROADBUF_FWD2IDX(0);
  RL(A);
  obj = *bufptr; // Read a left side object data byte
  if (carry)
    bufptr++; // FIXME needs wraparound
  obj |= *bufptr;
  if (obj == 0) // no object
    return;

  HLobj = &state->stage->addrof_left_hand_objects[obj];
  // Read collision values.
  Cdash = HLobj->hit_max_or_min;
  Edash = HLobj->hit_min_or_max;
  A = HLobj->impact_speed_cap;

  DEdash = Edash;
  BCdash = Cdash;

  // Check for collisions with scenery (left hand side).
  pos = state->xpos_road_centre[127];
  if (pos < BCdash || pos >= DEdash)
    return;
  // EX AF,AF'
  Aflip = 1; // likely an arg for scenery_hit()
  csc_hit_scenery(state, Aflip, Adash); /* was fallthrough */
}

/**
 * $A4B0: Csc hit scenery
 *
 * \param[in] state Pointer to game state.
 * \param[in] Aflip Aflip.
 * \param[in] Adash Adash.
 */
static void csc_hit_scenery(chqstate_t *state, int Aflip, int Adash)
{
  // Arrive here if hit scenery, e.g. drove into a tree or a lamp post.
  start_sfx(state, EFFECT_SCENERY_HIT, 3); /* priority 3 */
  scenery_hit(state, Aflip, Adash);
}

/**
 * $A4B8: Scenery hit
 *
 * \param[in] state Pointer to game state.
 * \param[in] Aflip Aflip.
 * \param[in] Adash Adash.
 */
static void scenery_hit(chqstate_t *state, int Aflip, int Adash)
{
  int speed;  /* was HL */
  int A;
  int HL;
  int L;
  int DE;

  if (state->ahc_crashed_flag)
    return; // already crashed

  state->ahc_crashed_flag = 1;
  state->ahc_flip_flag    = Aflip;
  state->ahc_crash_flip_count = ++Aflip;
  state->ahc_delay        = 5;

  speed = state->speed;
  A = (speed >> 4) + 16;
  // i.e. speed = max(24, A);
  L = 24; // minimum?
  if (A >= L)
    L = A;
  state->ahc_crash_spin_speed = (speed & ~0xFF) | L;

  // i.e. HL = min(Adash, state->speed);
  HL = Adash;
  DE = state->speed;
  if (HL >= DE)
    HL = DE;
  state->ahc_crash_speed_threshold = HL;
}

/**
 * $A4F6: Check fork scenery collisions
 *
 * \param[in] state  Pointer to game state.
 * \param[in] DEdash Dedash.
 * \param[in] HLdash Hldash.
 */
static void check_fork_scenery_collisions(chqstate_t *state, int DEdash,
    int HLdash)
{
  int          pos;            /* was HL */
  int          off_road;       /* was A */
  const obj_t *shortpoleobj;   /* was HL */
  int          hit_max_or_min; /* was BC' */
  int          hit_min_or_max; /* was DE' */
  int          A;              /* was A */
  int          pos2;           /* was HL' */

  pos = state->xpos_road_left[127];
  off_road = 0;
  if ((pos >> 8) == 0) {
    if (pos >= 0x6A)
      off_road = (pos >= 0x85) ? 1 : 2;
    goto set_off_road;
  }

  pos = state->xpos_road_fork_right[126];
  off_road = 0;
  if ((pos >> 8) == 0)
    if (pos < 0x8E)
      off_road = (pos >= 0x7C) ? 1 : 2;

set_off_road:
  state->off_road     = off_road;
  state->ahc_crash_spin  = 0;
  state->ahc_road_pos_a  = HLdash;
  state->ahc_road_pos_b  = DEdash;
  if (state->fork_taken == 0) {
    // Left fork was taken, short pole object is on right hand of road.
    shortpoleobj = state->stage->addrof_right_hand_short_pole_object;

    // Read collision values
    hit_max_or_min = shortpoleobj->hit_max_or_min; // max
    hit_min_or_max = shortpoleobj->hit_min_or_max; // min
    A              =
      shortpoleobj->impact_speed_cap;  // overridden to 0x8C in the call below

    pos2 = state->xpos_road_centre[127];
    if (pos2 < hit_max_or_min && pos2 >= hit_min_or_max)
      csc_hit_scenery(state, 0, 0x8C); /* exit via */
  } else {
    // Right fork was taken, short pole object is on left hand of road.
    shortpoleobj = state->stage->addrof_left_hand_short_pole_object;

    // Read collision values
    hit_max_or_min = shortpoleobj->hit_max_or_min; // min
    hit_min_or_max = shortpoleobj->hit_min_or_max; // max

    pos2 = state->xpos_road_centre[126];
    if (pos2 >= hit_max_or_min && pos2 < hit_min_or_max)
      csc_hit_scenery(state, 1, 0x8C); /* exit via */
  }
}

/**
 * $A579: Layout objects
 *
 * \param[in] state Pointer to game state.
 */
static void layout_objects(chqstate_t *state)
{
  int       carry = 0;
  u8       *objpos;       /* was HL */
  int       iterations;   /* was B */
  int       total;        /* was A */
  u16      *SP;
  u8       *bufptr;       /* was DE */
  const u8 *objpos2;      /* was IY */
  int       countdown;    /* was A */
  int       lanesbyte;    /* was A */
  u8        lanesbyte2;   /* was E' */
  int       Ldash;
  int       laneoffset;   /* was A */
  u16      *tabptr;       /* was HL' */
  int       laneshift;    /* was A */
  int       L;
  int       A;

  objpos = &state->object_positions[0];
  iterations = 21; // iterations
  total = 0;
  do {
    total += *objpos;
    *objpos++ = total;
  } while (--iterations > 0);

  SP = &state->xpos_road_centre_right[0]; // OR should this be ea00[256] ?
  bufptr = ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET);
  objpos2 = &state->object_positions[0];
  iterations = 21; // iterations
  if (state->fork_visible == 0)
    goto positions_loop;

  countdown = state->fork_countdown;
  if (countdown > 0) {
    iterations = countdown;
    do {
positions_loop:
      lanesbyte = *bufptr;
      // EXX
      lanesbyte2 = lanesbyte;
      Ldash = ~(*objpos2 * 2);

      // Read left hand offset bits (0+1).
      laneoffset = lanesbyte2 & 3;
      if (laneoffset == 0) {
        // Otherwise no left hand offset is set.
        --SP; *SP = state->xpos_road_left[Ldash];
set_right_hand:
        tabptr = &state->xpos_road_right[Ldash];
        goto load_and_store_right;
      }

      // The left hand position of the road in #REGa is 1/2/3 here. Use that to
      // select table $E8xx/$E9xx/$EAxx.
      // Not sure if I trust structure layout, so using a switch here.
      switch (laneoffset) {
      case 1: tabptr = &state->xpos_road_left[Ldash]; break;
      case 2: tabptr = &state->xpos_road_centre_left[Ldash]; break;
      case 3: tabptr = &state->xpos_road_centre[Ldash]; break;
      default: assert(0);
      }
      --SP; *SP = *tabptr;

      RL(lanesbyte2); // Shift bit 7 of lanes byte into carry (checked later)
      if ((lanesbyte2 & (1 << 7)) != 0) {
        if (carry)
          goto set_right_hand; // dirt track or fork
        // Otherwise it's a tunnel piece.
        laneshift = 3;
      } else {
        // Normal road
        laneshift = 3;
        if (!carry)
          // It's 2 lane or 2/3 lane widening/narrowing.
          laneshift--;
        // Otherwise it's 3 lane or 3/4 lane widening/narrowing
      }

      switch (laneshift) {
      case 1: tabptr = &state->xpos_road_left[Ldash]; break;
      case 2: tabptr = &state->xpos_road_centre_left[Ldash]; break;
      case 3: tabptr = &state->xpos_road_centre[Ldash]; break;
      default: assert(0);
      }

load_and_store_right:
      --SP; *SP = *tabptr;

      // EXX
      objpos2++;
      WRAPPINGINCREMENT(bufptr, state->road_buffer_start);
    } while (--iterations > 0);

    // $EB00 now contains pairs of 16-bit left,right object positions.
    countdown = state->fork_countdown;
    if (countdown == 0)
      return; // no fork
  }

  // Forking
  A = 21 - countdown;
  if (A == 0)
    return; // no fork, or not about to fork?

  iterations = A;
  do {
    L = ~(*objpos2 * 2);
    --SP; *SP = state->xpos_road_centre[L - 1];
    --SP; *SP = state->xpos_road_centre_right[L - 1];
    objpos2++;
  } while (--iterations > 0);
}

/**
 * $A60E: Cycle counters
 *
 * \param[in] state Pointer to game state.
 */
static void cycle_counters(chqstate_t *state)
{
  state->anim_counter = (state->anim_counter + 1) & 3;
  state->frame_toggle = (state->frame_toggle + 1) & 1;
  if (state->frame_toggle == 0)
    return;
  state->slow_anim_counter = (state->slow_anim_counter + 1) & 3;
}

/**
 * $A637: Perp behaviour
 *
 * \param[in] state Pointer to game state.
 * \param[in] IXxpos  X-position table pointer. (was IX)
 */
void perp_behaviour(chqstate_t *state, hazard_t *IXperp)
{
  int       carry = 0;
  int       Ahit_timer;         /* was A */
  int       Cperp_distance;     /* was C */
  hazard_t *IYhazard;           /* was IY */
  int       DE;
  int       HL;
  const u8 *HLtab;              /* was HL */
  int       Acurrlane;          /* was A */
  int       BCspawn_lanes;      /* was BC */
  int       Adash;              /* was A' */

  int       HLspeed;            /* was HL */
  int       smash_twice;        // Additional
  int       Bmin_spawn_lane;    /* was B */
  int       Cmax_spawn_lane;    /* was C */
  int       Ccurrentlane;       /* was C */
  int       Cnewlane;           /* was C */
  int       Cdelta;             /* was C */
  int       changing_lane;      /* was C */
  int       Ahorzpos;           /* was A */
  u8        A;
  int       Biterations;        /* was B */
  int       Adelay;             /* was A */
  int       Adistance;          /* was A */
  int       Acounter;           /* was A */
  int       DEspeedmult;        /* was DE */
  int       Adistancediff;      /* was A */
  int       Dbonus_hi;          /* was D */
  int       Ebonus_mid;         /* was E */
  int       Bmin_lane;          /* was B */
  int       Cmax_lane;          /* was C */

  if (state->perp_caught_phase > 0)
    return;

  // Start the chase if required (enables flashing lights, smash bar, sirens,
  // etc.)
  if (state->sighted_flag == 0)
    start_chase(state);

  // Reading a hit counter here? It starts at $FC (set at #R$A78A) and is
  // incremented. This seems like it might speed the perp car up when it's
  // hit.
  Ahit_timer =
    IXperp->hit_timer; // Read IXperp[7] e.g. $A18F  -- a hit counter/delay
  if (Ahit_timer == 0)
    goto pb_hit_timer_clear; // Jump if zero  -- delay finished, perp can be hit again?
  else if (Ahit_timer > 0)
    goto pb_set_delay; // Jump to set delay if positive
  // Otherwise #REGa is negative.

  // This line gets hit 4 times when we smash into the perp's car - matching
  // the $FC value it's reset to.
  if (++IXperp->hit_timer)
    return; // do nothing

  // IXperp[7] must be zero to arrive here. We now iterate over all non-perp
  // hazards.
pb_hit_timer_clear:
  // PUSH IY
  Cperp_distance =
    IXperp->distance; // Read perp's distance (buffer offset) into #REGc
  Biterations = 5; // iterations
  IYhazard = &state->hazards[1];
  do {
    if (IYhazard->used == HAZARD_USED) // (was RLC) hazard active
      goto pb_ensure_vehicle;

pb_find_unused_hazard_continue:
    IYhazard++; // Move to next hazard
  } while (--Biterations > 0);
  // POP IY

  goto pb_check_changing_lane_flag;

pb_ensure_vehicle:
  // It's $80 for vehicles, 0+ for hazards or $FF if unused
  if ((IYhazard->hazard_flags & (1 << 7)) ==
      0) // it's not a vehicle, continue to next hazard
    goto pb_find_unused_hazard_continue;

  // Calculate distance between current hazard-car and the perp.
  Adistancediff = IYhazard->distance - Cperp_distance;
  if ((s8) Adistancediff < 0)
    // Otherwise hazard-car is behind perp...
    Adistancediff += 2; // move it two lanes away?
  else
    Adistancediff -= 3;

  if (!carry) // FIXME out of range?
    goto pb_find_unused_hazard_continue;

  // compare to perp's lane
  if (IYhazard->hazard_lane_OR_perp_dist_hi != IXperp->current_lane)
    goto pb_find_unused_hazard_continue;

  // So the lanes match
  // POP IY
  goto pb_random_move_left_or_right;

pb_check_changing_lane_flag:
  // load "changing lane" flag that appears to be set to 1 when the perp
  // changes lane
  A = state->pb_changing_lane;
  if (A)
    goto pb_check_lane;

  // Otherwise not changing lane?
  A = IXperp->distance;
  if (A >= 7)
    goto pb_check_lane;

  // Delay between perp lane changes: pb_lane_change_timer counts down; on
  // zero it resets to perp_lane_change_base + (rng & 31).
  // In-place decrementing counter.
  A = state->pb_lane_change_timer - 1;
  if (A)
    goto pb_update_counter;

  // When it hits zero we pick a random number...
  A = state->stage->perp_lane_change_base + (rng(state) & 31);

pb_update_counter:
  state->pb_lane_change_timer = A;

  // This smells like it's detecting position and turning that into lanes.
  // The values are like those used by get_spawn_lanes.

  HL = state->scenedata.road_pos - 164;
  Bmin_lane = 4; Cmax_lane = 4;
  if ((s16) HL < 0) // carried, HL < 164
    goto pb_a6cf;
  DE = 70;
  Bmin_lane = 3;
  HL -= DE;
  if ((s16) HL < 0) // carried, HL < 70
    goto pb_a6cf;
  Bmin_lane = 2; Cmax_lane = 3;
  HL -= DE;
  if ((s16) HL < 0) // carried, HL < 70
    goto pb_a6cf;
  Bmin_lane = 1; Cmax_lane = 2;
  HL -= DE;
  if ((s16) HL < 0) // carried, HL < 70
    goto pb_a6cf;
  Cmax_lane = 1;

pb_a6cf:
  A = IXperp->current_lane;
  if (A == Cmax_lane)
    goto pb_random_move_left_or_right;
  if (A != Bmin_lane)
    goto pb_check_lane;

pb_random_move_left_or_right:
  Ccurrentlane = IXperp->current_lane; // current_lane
  Cnewlane = ((s8) rng(state) >= 0) ? Ccurrentlane + 1 : Ccurrentlane - 1;

  Acurrlane = Cnewlane;
  Cdelta = 2; // lane delta
  if (Acurrlane != 0) {
    if (Acurrlane < 5) // lane is reasonable?
      goto pb_set_current_lane;

    // Arrive here if the updated current_lane is >= 5.
    Cdelta = -2; // delta -2
  }
  Acurrlane += Cdelta;

pb_set_current_lane:
  IXperp->current_lane = Acurrlane; // Update current_lane

pb_check_lane:
  BCspawn_lanes = get_spawn_lanes(state, IXperp->distance);
  Bmin_spawn_lane = BCspawn_lanes >> 8; // Conv: added unpacking
  Cmax_spawn_lane = BCspawn_lanes & 0xFF;

  Acurrlane = IXperp->current_lane; // read current_lane
  if (Acurrlane >= Bmin_spawn_lane)
    goto pb_min_lane_set;

  // Otherwise the (perp?) needs to move right to stay on the road.
  Acurrlane += 2; // Move right by two lanes [why two?]
  IXperp->current_lane = Acurrlane;

pb_min_lane_set:
  if (Acurrlane <= Cmax_spawn_lane)
    goto pb_reread_current_lane;

  // Otherwise the (perp?) needs to move left to stay on the road.

  Acurrlane -= 2; // Move left by two lanes
  IXperp->current_lane = Acurrlane;

  // current_lane is 1/2/3/4

pb_reread_current_lane:
  Acurrlane =
    IXperp->current_lane; // Re-read current_lane [not convinced this is required]
  HLtab = &hazard_pos_speed[Acurrlane - 1];
  Ahorzpos = IXperp->horz_pos_on_road;
  // #REGc seems to be a flag that's 1 when changing lane and 0 otherwise. We
  // seem to be bumping the position by +/-10.
  changing_lane = 1; // changing lane flag

  if (Ahorzpos == *HLtab)
    goto pb_set_lane_from_table_2;
  if (Ahorzpos < *HLtab)
    goto pb_check_high;
  Ahorzpos -= 10;
  if ((s8) Ahorzpos < 0) // carried?
    goto pb_set_lane_from_table_1;

  if (Ahorzpos >= *HLtab)
    goto pb_set_horz_pos;

  // Redundant code path; jump to pb_set_lane_from_table_2 instead.

pb_set_lane_from_table_1:
  changing_lane--; // Decrement 1 to 0 so we're not changing lane
  Ahorzpos = *HLtab;
  goto pb_set_horz_pos;

pb_check_high:
  Ahorzpos += 10;
  if (Ahorzpos < 10) // carried
    goto pb_set_lane_from_table_2;
  if (Ahorzpos < *HLtab)
    goto pb_set_horz_pos;

pb_set_lane_from_table_2:
  changing_lane--;
  Ahorzpos = *HLtab;

pb_set_horz_pos:
  IXperp->horz_pos_on_road = Ahorzpos;
  state->pb_changing_lane = changing_lane;
  Adelay = state->pb_delay; // load delay counter

  DEspeedmult = 30; // multiplicand
  HLspeed = 230; // base speed
  if (Adelay)
    goto pb_bypass;

  // Countdown+rng stuff again... as at #R$A69B

  // In-place decrementing counter.
  Acounter = state->pb_approach_timer - 1;
  state->pb_approach_timer = Acounter;
  if (Acounter)
    goto pb_a776;

  // When it hits zero we pick a random number...
  Adelay = state->stage->perp_approach_base + (rng(state) & 0xF);

  state->pb_approach_timer = Adelay;
  Adelay = 10; // reset the delay loop

  // Count down outer delay loop.
pb_bypass:
  state->pb_delay = --Adelay;
  if (Adelay)
    goto pb_a776;

  Adistance = IXperp->distance;
  if (Adistance >= 13)
    goto pb_a776;

  // Distance to perp is 12 or less.
  // .
  // HL += (13 - A) * DE    HL is 230, DE is 30
  // .
  // This seems to be using the distance to the perp as a scale by which to adjust
  // its horizontal position.

  HLspeed += (13 - A) * DEspeedmult;

pb_a776:
  A = IXperp->distance - 6;
  if ((s8) A < 0) {
    // Distance to perp is 5 or less
    A = (A + 5) * 8; // Bug? we do nothing with #REGa...
    HLspeed += DEspeedmult;
  }
  IXperp->speed = HLspeed;
  return;

  // If I meddle with this value the perp seems to race off too fast to catch.
pb_set_delay:
  IXperp->hit_timer = -4; // $FC
  // PUSH AF // Ahit_timer
  if (Ahit_timer >= 3)
    Ahit_timer -= 3;

  Adash = (state->boost == 0) ? 200 : 230;
  scenery_hit(state, Ahit_timer, Adash);

  state->ahc_crash_speed_threshold += 40;

  Dbonus_hi = 0; // Zero bonus high digit

  smash_twice = 0;

  // POP AF  Restore Ahit_timer which holds IXperp[7] and flags from earlier
  if (!carry || Ahit_timer == 2)
    goto pb_a7be;

  smash_twice = 1; /* was PUSH HL -- Put another call to smash on the stack */

  Dbonus_hi = 4; // Set bonus high digit to 4
pb_a7be:
  Dbonus_hi += state->wanted_stage_number;
  Ebonus_mid = 0;
  if (state->retry_count) {
    A = Dbonus_hi;
    Dbonus_hi = Ebonus_mid;
    RLC(A);
    RLC(A);
    RLC(A);
    RLC(A);
    Ebonus_mid = A;
  }
  add_bonus(state, 0, Ebonus_mid, Dbonus_hi);
  state->pb_delay = 5; // set delay counter to 5 turns
  start_chatter(state, 5, &chatterblk_raymond_smash[0]);
  start_sfx(state, EFFECT_CAR_HIT, 1); /* priority 1 */ /* exit via */

  smash(state); // Conv: Direct call rather than stack push
  if (smash_twice)
    smash(state);
}

/**
 * $A7F3: Spawn cars
 *
 * \param[in] state Pointer to game state.
 */
static void spawn_cars(chqstate_t *state)
{
  int       allow_spawning;     /* was A */
  int       random_extra_delay; /* was C */
  int       spawn_delay;        /* was A */
  int       iterations;         /* was B */
  int       cars_seen;          /* was C */
  hazard_t *hazard;             /* was IX */
  int       spawn_lanes;        /* was BC */
  u8        min_lane;           /* was B */
  int       max_lane;           /* was C */
  int       new_lane;           /* was A */
  const u8 *hazard_pos;         /* was HL */
  int       bitmap_index;          /* was C */

  // Return without spawning anything if perp_caught_phase is non-zero or the
  // dont_spawn_cars flag is set.
  if (state->perp_caught_phase > PERPCAUGHTPHASE_NONE || state->dont_spawn_cars)
    return;

  // Return without spawning anything if allow_spawning is zero.
  allow_spawning = state->allow_spawning;
  if (allow_spawning == 0)
    return;

  // Reduce inline spawn delay counter by the value of allow_spawning (1 or 2
  // here).
  state->sc_spawn_counter -= allow_spawning;
  if (state->sc_spawn_counter > 0)
    return;

  random_extra_delay = rng(state) & 0x0F;

  spawn_delay = state->stage->car_spawn_delay;
  if (state->sighted_flag)
    // Perp was sighted so increase the spawn delay by 25.
    spawn_delay += 25;
  spawn_delay += random_extra_delay;
  state->sc_spawn_counter = spawn_delay;

  // Now walk the hazards array to find an unused slot.
  iterations = 5;
  cars_seen = 0;  // one bit is set each time a car is seen
  hazard = &state->hazards[1];
  do {
    if (hazard->used == HAZARD_UNUSED)
      goto fill_in;
    if (hazard->hazard_flags & (1 << 7)) // top bit is set for vehicles
      cars_seen = (cars_seen << 1) | 1;
    hazard++;
  } while (--iterations > 0);
  return;

fill_in:
  // Don't spawn if there are three or more cars already spawned.
  if (cars_seen & (1 << 2))
    return;

  // Copy template hazard to unused slot.
  memcpy(hazard, &hazard_template, sizeof(hazard_template));

  // Select a random lane in which to spawn the hazard.
  spawn_lanes = get_spawn_lanes(state, 20);
  min_lane = spawn_lanes >> 8;
  max_lane = spawn_lanes & 0xFF;

  new_lane = rng(state) & 3;

  // Clamp new lane to valid range.
  new_lane += min_lane;
  if (new_lane > max_lane)
    new_lane = max_lane;

  hazard->hazard_lane_OR_perp_dist_hi = new_lane;
  hazard->current_lane                = new_lane;

  // Copy hazard_pos_speed values to hazard position and speed.
  hazard_pos = &hazard_pos_speed[-1 + new_lane];
  hazard->horz_pos_on_road = hazard_pos[0];
  hazard->speed            = hazard_pos[state->sighted_flag ? 8 : 4];

  // Now pick a random car bitmap to show.
  bitmap_index = rng(state) & 6;
  // If we've sighted the perp then don't spawn any generic cars (offset 6)
  // since they look just like the perp's. Instead use offset 4.
  if (state->sighted_flag && bitmap_index == 6)
    bitmap_index -= 2; // 6 -> 4

  hazard->hittable.bitmaps = state->stage->bitmaps_vehicles[bitmap_index / 2];
}

/**
 * $A89C: Get spawn lanes
 *
 * \param[in] state Pointer to game state.
 * \param[in] extra Extra. (was C)
 * \return Non-zero on success.
 */
static u16 get_spawn_lanes(chqstate_t *state, int extra)
{
  int carry;
  u8 *roadbuf;    /* was HL */
  u8  lanes;      /* was A */
  int lanes_copy; /* was E */

  roadbuf = ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET + 2 + extra);
  lanes = *roadbuf;
  if (lanes == MAP_LANES_4_VAL) // 0
    return 0x0104;
  lanes_copy = lanes;
  lanes &= MAP_LANES_DIRTTRACK_VAL; // mask keeps bits 7,6,0; both DIRTTRACK and FORKED survive
  if (lanes == MAP_LANES_DIRTTRACK_VAL) // dirt track or forked road: treat as 4-lane
    return 0x0104;
  if (lanes == MAP_LANES_TUNNEL_VAL) // tunnel (masked: entry/exit/body all reduce to this)
    return 0x0103;
  lanes = lanes_copy & 0x82;
  carry = lanes & (1 << 7), lanes <<= 1;
  if (carry) {
    if (lanes)
      return 0x0204;
    else
      return 0x0103;
  } else {
    if (lanes == 0) // note: swapped vs above
      return 0x0102;
    else
      return 0x0304;
  }
}

/**
 * $A8CD: Hazard handler
 *
 * \param[in] state Pointer to game state.
 * \param[in] IXxpos  X-position table pointer. (was IX)
 */
void hazard_handler(chqstate_t *state, hazard_t *IXhazard)
{
  int       carry = 0;
  int       spawn_lanes;        /* was BC */
  u8        min_lane;           /* was B */
  int       max_lane;           /* was C */
  int       lane;               /* was A */
  int       current_lane;       /* was A */
  int       horz_pos;           /* was A */
  int       hit_timer;          /* was A */
  const u8 *phazard_pos_speed;  /* was HL */

  if (state->perp_caught_phase != 0 || state->dont_spawn_cars != 0)
    IXhazard->speed = 0x1FF;

  spawn_lanes = get_spawn_lanes(state, IXhazard->distance);
  min_lane = spawn_lanes >> 8;
  max_lane = spawn_lanes & 0xFF;

  lane = IXhazard->hazard_lane_OR_perp_dist_hi;
  if (lane < min_lane)
    IXhazard->current_lane = min_lane;
  if (lane > max_lane)
    IXhazard->current_lane = max_lane;

  current_lane = IXhazard->current_lane;
  if (current_lane != IXhazard->hazard_lane_OR_perp_dist_hi) {
    RL(min_lane); // I'm not understanding these rotates
    phazard_pos_speed = &hazard_pos_speed[current_lane];
    horz_pos = IXhazard->horz_pos_on_road;
    RR(min_lane);
    if (carry) {
      horz_pos -= 5;
      if (horz_pos < *phazard_pos_speed) {
        horz_pos = *phazard_pos_speed;
        IXhazard->hazard_lane_OR_perp_dist_hi = current_lane;
      }
    } else {
      horz_pos += 5;
      if (horz_pos >= *phazard_pos_speed) {
        horz_pos = *phazard_pos_speed;
        IXhazard->hazard_lane_OR_perp_dist_hi = current_lane;
      }
    }

    IXhazard->horz_pos_on_road = horz_pos;
  }

  hit_timer = IXhazard->hit_timer;
  if (hit_timer == 0)
    return;

  IXhazard->hit_timer = 0;

  if (state->ahc_crashed_flag)
    return; // already crashed

  IXhazard->used = HAZARD_UNUSED;
  state->overtake_bonus_bcd = 0;

  if (hit_timer >= 3)
    hit_timer -= 3;

  // Crashed
  scenery_hit(state, hit_timer, 0x96);
  start_chatter(state, 3, &chatterblk_raymond_random_yelps[0]);
  start_sfx(state, EFFECT_CAR_HIT, 2); /* priority 2 */ /* exit via */
}

/**
 * $A955: Choose dirt and stones
 *
 * \param[in] state Pointer to game state.
 */
static void choose_dirt_and_stones(chqstate_t *state)
{
  u8 *table;  /* was DE */

  if (state->on_dirt_track == 0 || state->allow_spawning == 0)
    return;

  // TODO: table_ed00 is u16s but this stores two bytes at byte offset 40: a
  // stone/dirt type and a random position.

  table = (u8 *) &state->xpos_road_fork_right[40];
  table[0] = ((s8) rng(state) >= 0) ? 1 : 2; // choose stone or dirt
  table[1] = rng(state); // choose random position
  state->ldas_enabled = 1;
  state->rm_SM_C0BB_fork_copy_pending   = 1;
  state->dss_enabled  = 1;
}

/**
 * $A97E: Layout dirt and stones
 *
 * \param[in] state Pointer to game state.
 */
static void layout_dirt_and_stones(chqstate_t *state)
{
  int       carry = 0;
  const u8 *obj_pos;             /* was IY */
  int       iterations;          /* was B */
  int       total;               /* was C */
  u16      *table_ed00;          /* was HL */
  u8        A;                   /* was A */
  int       Ldash;
  int       val_from_table_e800;
  int       val_from_table_ec00; /* was HL' */
  u16       result;              /* was HL' */
  int       iterations2;         /* was B' */
  int       Cdash;

  if (state->ldas_enabled == 0)
    return;

  obj_pos = &state->object_positions[18];
  iterations = 20;
  total = 0;
  table_ed00 = &state->xpos_road_fork_right[0x28 / 2]; // is this pairs?
  do {
    // FIXME increment + advance will be wrong since ed00 is u16s
    if (*table_ed00++)
      goto ldas_do_work;
    table_ed00 += 3;
loop1_continue:
    obj_pos--;
  } while (--iterations > 0);

  if (total == 0) {
    state->ldas_enabled = 0;
    state->rm_SM_C0BB_fork_copy_pending   = 0;
    state->dss_enabled  = 0;
  }
  return;

ldas_do_work:
  A = *table_ed00++; // multiplicand?
  total++;

  // EXX

  // EX AF,AF'
  Ldash = ~(obj_pos[1] * 2);
  val_from_table_e800 = state->xpos_road_left[Ldash /
                        2]; // FIXME Probably off by one here?
  val_from_table_ec00 = state->xpos_road_right[(Ldash - 1) / 2];

  // PUSH DEdash
  // Calc width of road?
  val_from_table_e800 = val_from_table_ec00 - val_from_table_e800; // multiplier?
  result = 0; // result
  // EX AF,AF'

  // Multiplier
  iterations2 = 8; // iterations
  do {
    RL(A);
    if (carry)
      result += val_from_table_e800;
    result <<= 1;
  } while (--iterations2 > 0);

  A = result & 0xFF;
  RR(A);
  Cdash = A;
  // POP result // HLdash
  result += Cdash; /* was BCdash but B is zero here */
  // PUSH result // HLdash

  // EXX

  // POP result to DE
  *table_ed00++ = result; // Conv: 16-bit write

  goto loop1_continue;
}

/**
 * $A9DE: Dust stones stuff
 *
 * \param[in] state       Pointer to game state.
 * \param[in] Biterations Biterations.
 * \param[in] IYheight Height table pointer. (was IY)
 */
static void dust_stones_stuff(chqstate_t *state, int Biterations,
                              const u8 *IYheight)
{
  int              carry = 0;
  u8               A;
  u16             *HLtable;
  const bitmap_t (*DEbitmaps)[SPRITE_FRAMES];
  const bitmap_t  *HLbitmap;

  int              C;
  int              E;
  int              saved_A;

  if (state->dss_enabled == 0)
    return;

  HLtable = state->dss_fork_xpos_ptr; // table ptr
  A = *HLtable & 0xFF;
  HLtable++; // halved advance since table is words
  if (A)
    goto dss_bitmaps;

  HLtable++;
  state->dss_fork_xpos_ptr = HLtable;
  return;

dss_bitmaps:
  DEbitmaps = state->stage->bitmaps_stones;
  if (--A)
    DEbitmaps = state->stage->bitmaps_dust;

  C = *HLtable++;
  A = *HLtable++;
  // EX AF,AF'
  state->dss_fork_xpos_ptr = HLtable;
  state->doc_col_pos = 0;
  // H = 0;
  A = Biterations - 1;
  if (A > 10)
    A = 10;

  SRL(A);
  HLbitmap = DEbitmaps[A];
  E = HLbitmap->width_bytes * 8;

  // EX AF,AF'
  saved_A = A;
  A = C;
  C = 0;
  if (saved_A >= 0) {
    if (saved_A)
      return;

    // So it's zero
    if (A >= 128) {
      draw_object_right_helicopter_entrypt(state, A, HLbitmap,
                                           IYheight); /* exit via */
    } else {
      A += E;
      draw_object_left_helicopter_entrypt(state, A, HLbitmap,
                                          IYheight); /* exit via */
    }
  } else {
    A += E;
    if (A >= E) //carry? check
      return;

    draw_object_left_helicopter_entrypt(state, A, HLbitmap,
                                        IYheight);  /* exit via */
  }
}

/**
 * $AA38: Draw helicopter
 *
 * \param[in] state       Pointer to game state.
 * \param[in] Biterations Biterations.
 * \param[in] IYheight Height table pointer. (was IY)
 */
static void draw_helicopter(chqstate_t *state, int Biterations, u8 *IYheight)
{
  int                   carry = 0;
  int                   diff;            /* was DE */
  u16                   total;           /* was HL */
  u8                    fast_counter;    /* was A */
  int                   Biterations2;    /* was B */
  u8                    Atotal;          /* was A */
  int                   frame;           /* was A */
  const heli_bitmap_t (*helibitmaps)[SPRITE_FRAMES]; /* was HL */
  const heli_bitmap_t  *helibitmap;      /* was DE */

  if (Biterations != 3)
    return;

  diff = IYheight[0x4F] - IYheight[0x4E]; // in the $E315 buffer?
  total = 0;
  fast_counter = state->fast_counter & 0xE0;
  Biterations2 = 8;
  // Multiplier
  do {
    RL(fast_counter);
    if (carry)
      total += diff;
    total <<= 1;
  } while (--Biterations2 > 0);

  Atotal = total >> 8;
  RR(Atotal); // halve?
  state->dh_heli_rotor_pos = Atotal;

  state->dh_heli_body_y_offset = state->dh_heli_vert_base - IYheight[0x4E];

  Biterations2 = 5; // iterations (draw first five)
  frame = state->anim_counter & 1; // heli frame

  helibitmaps = state->stage->addrof_helicopter_stuff_1;
  if (frame != 0)
    helibitmaps = state->stage->addrof_helicopter_stuff_2;

  do {
    helibitmap = *helibitmaps++;
    draw_helicoper_part(state, helibitmap->y_offset + state->dh_heli_body_y_offset,
                        &helibitmap->inner, IYheight);
  } while (--Biterations2 > 0);

  // BUT final entry seems to be a different format, so this can't be right.

  helibitmap = *helibitmaps;
  // A = 0; // an apparently useless op
  draw_helicoper_part(state, state->dh_heli_rotor_pos, &helibitmap->inner,
                      IYheight);
}

static void draw_helicoper_part(chqstate_t                *state,
                                int                         A,
                                const heli_bitmap_inner_t *DEinnerbitmap,
                                const u8                  *IYheight)
{
  int             carry;
  int             BC;         /* was BC */
  int             screen_pos; /* was HL/DE */
  const bitmap_t *HLbitmap;   /* was HL */
  int             Bwidth;     /* was B */
  int             Atop;       /* was A */
  int             Abot;       /* was A */
  int             C;          /* was C */

  BC = state->dhl_helipos;
  state->doc_col_pos = -A; // in draw_object_common

  screen_pos = DEinnerbitmap->horz_offset + BC;


  HLbitmap  = &DEinnerbitmap->bm; // Conv: Ops shuffled a bit

  Bwidth = HLbitmap->width_bytes * 8;
  Atop = screen_pos >> 8;
  // AND Atop  set flags here
  Abot = screen_pos & 0xFF;
  C = 0; // can't see what's using this
  if (Atop >= 0) {
    if (Atop != 0)
      return;

    if (Abot >= 0x80) { // or -ve?
      draw_object_right_helicopter_entrypt(state, Abot, HLbitmap,
                                           IYheight); /* exit via */
    } else {
      Abot += Bwidth;
      draw_object_left_helicopter_entrypt(state, Abot, HLbitmap,
                                          IYheight); /* exit via */
    }
  } else {
    carry = (Abot + Bwidth) > 255;
    Abot += Bwidth;
    if (!carry)
      return;

    draw_object_left_helicopter_entrypt(state, Abot, HLbitmap,
                                        IYheight); /* exit via */
  }
}

/**
 * $AAC6: Move helicopter
 *
 * \param[in] state Pointer to game state.
 */
static void move_helicopter(chqstate_t *state)
{
  int height;     /* was A & C */
  int direction;  /* was A */
  int offset;     /* was A */
  int helipos;    /* was HL */
  int newhelipos; /* was HL */
  int centre;     /* was DE */

  if (state->helicopter_control == 0)
    return;

  // Helicopter descends while moving to height 97 (smaller = lower).
  height = state->mh_height;
  if (height != 97)
    height -= 2;
  state->mh_height = height;

  // Animate
  state->mh_animframe = (state->mh_animframe + 1) & 3;

  // Switch direction at the end of each cycle
  direction = state->mh_direction;
  if (state->mh_animframe == 0)
    direction = -direction;
  state->mh_direction = direction;

  offset = direction + state->mh_offset;
  state->mh_offset = offset;

  state->dh_heli_vert_base = offset + height;

  helipos = state->scenedata.road_pos - state->mh_prevroadpos; // delta
  state->mh_prevroadpos = state->scenedata.road_pos; // update

  helipos += state->dhl_helipos;
  centre = 112; // const
  if (helipos != centre) {
    newhelipos = helipos + (helipos > centre ? -8 : 8);
    if (newhelipos >= centre) {
      if (helipos > centre) // note: rechecking earlier calc
        goto set_newpos;
      goto set_centre;
    }
    if (helipos > centre) { // note: rechecking earlier calc
set_centre:
      newhelipos = centre;
    }
  } else {
    newhelipos = helipos; // Conv: added
  }

set_newpos:
  state->dhl_helipos = newhelipos;
}

/**
 * $AB33: Drive helicopter
 *
 * \param[in] state Pointer to game state.
 */
static void drive_helicopter(chqstate_t *state)
{
  int       heli_ctl;     /* was A */
  int       helipos;      /* was A */
  int       draw_heli;    /* was A */
  int       new_heli_ctl; /* was A */
  int       HL_ab06;      /* was HL */
  const u8 *chatterblk;   /* was HL */

  heli_ctl = state->helicopter_control;
  if (heli_ctl == 0)
    return;

  if (--heli_ctl == 0) // 1
    goto hc_1;

  if (--heli_ctl) // 3+
    goto hc_pick_direction;

  // Otherwise helicopter_control is 2.

  helipos = state->dhl_helipos; // Conv: Original only checks low byte
  if (helipos == 0)
    return;

  draw_heli = 0; // false
  new_heli_ctl = 0; // New value for helicopter_control is 0
  goto hc_exit;

hc_1:
  HL_ab06 = -56;
  new_heli_ctl = 2; // New value for helicopter_control is 2
  goto hc_set_draw;

hc_pick_direction:
  heli_ctl--;

  chatterblk = &chatterblk_pilot_turn_left[0];
  if (heli_ctl) {
    if (--heli_ctl)
      return;

    chatterblk = &chatterblk_pilot_turn_right[0];
  }

  state->mh_prevroadpos = state->scenedata.road_pos;
  state->dhl_helipos    = -24;
  state->mh_animframe   = 0;
  state->mh_offset      = 0;
  state->mh_direction   = 1;
  start_chatter(state, 15, chatterblk);
  HL_ab06 = 112;
  // Set starting vertical position of the helicopter.
  state->mh_height = 133;
  new_heli_ctl = 5; // New value for helicopter_control is 5
hc_set_draw:
  state->mh_heli_centre_y = HL_ab06;
  draw_heli = 1; // true
hc_exit:
  state->dee_helicopter = draw_heli;
  state->helicopter_control = new_heli_ctl;
}

/**
 * $AB9A: Spawn hazards
 *
 * \param[in] state Pointer to game state.
 */
static void spawn_hazards(chqstate_t *state)
{
  int allow_spawning;    /* was A */
  int Cdistance;         /* was C */
  u8 *roadbuf;           /* was HL */
  int hazard;            /* was A */
  int DEhittable_offset; /* was DE */
  int horz_pos;          /* was B */

  allow_spawning = state->allow_spawning;
  if (allow_spawning == 0)
    return;

  // Calculate a spawning distance.
  Cdistance = 20 - allow_spawning;

  // Point #REGhl at hazards data.
  roadbuf = ROADBUF_FWD2PTR(160 + Cdistance);

  // Do we have a hazard?
  hazard = *roadbuf;
  if (hazard == 0)
    return; // no hazard?

  DEhittable_offset = 0; // index 0
  if (hazard >= 4) {
    DEhittable_offset = 3; // index 1, times sizeof(hittable)
    hazard -= 3;
  }

  horz_pos = 50; // x coord
  if (--hazard == 0)
    goto sh_add_hazards_done; // if 1, add one barrier?

  horz_pos = 220;
  if (--hazard == 0)
    goto sh_add_hazards_done; // if 2

  WRAPPING(roadbuf, 2, state->road_buffer_start);
  *roadbuf = DEhittable_offset >> 8; // D is zero

  WRAPPING(roadbuf, 2, state->road_buffer_start);
  *roadbuf = DEhittable_offset >> 8; // D is zero

  hazard = DEhittable_offset & 0xFF;
  if (hazard != 3)
    goto sh_add_two_tumbleweeds;

  // Use inhibit_collision_detection to choose between two or three barriers?
  // Seems odd

  hazard = state->inhibit_collision_detection;
  if (hazard == 0)
    goto sh_add_two_barriers;

  // Flag was set.

  // Populate hazards with three barriers (e.g. for perp escape screen).
  // Conv: POP HL used in original code to cause
  // spawn_hazards to return when the hazards table is full.
  // We have to specifically check in the C port.
  if (sh_find_free(state, 32, Cdistance, DEhittable_offset)) return;
  if (sh_find_free(state, 86, Cdistance, DEhittable_offset)) return;
  horz_pos = 140;
  goto sh_add_hazards_done;

  // Populate hazards with two barriers (e.g. for dirt track).
sh_add_two_barriers:
  if (sh_find_free(state, 80, Cdistance, DEhittable_offset)) return;
  horz_pos = 160;
  goto sh_add_hazards_done;

  // Populate hazards with two tumbleweeds (e.g. for dirt track).
sh_add_two_tumbleweeds:
  if (sh_find_free(state, 70, Cdistance, DEhittable_offset)) return;
  horz_pos = 180;

sh_add_hazards_done:
  (void) sh_find_free(state, horz_pos, Cdistance, DEhittable_offset);
}

static int sh_find_free(chqstate_t *state,
                        int          Bhorz_pos,
                        int          Cdistance,
                        int         DEhittable_offset)
{
  int       iterations; /* was B */
  hazard_t *hazard;     /* was HL */
  hazard_t *IXhazard;   /* was IX */

  iterations = 6;
  hazard = &state->hazards[0];
  do {
    if (hazard->used == HAZARD_UNUSED)
      goto sh_found_free;
    hazard++;
  } while (--iterations > 0);
  return 1; // cause an exit (Conv: was POP HL)

sh_found_free:
  IXhazard = hazard;
  memset(hazard, 0, sizeof(*hazard));
  IXhazard->hit_handler = hazard_hit;
  // orig sizeof(hittable) is 3
  IXhazard->hittable          =
    state->stage->addrof_hittable_objects[DEhittable_offset / 3];
  IXhazard->horz_pos_on_road = Bhorz_pos;
  IXhazard->distance         = Cdistance;
  IXhazard->used             = HAZARD_USED;
  return 0;
}

/**
 * $AC3C: Hazard hit
 *
 * \param[in] state    Pointer to game state.
 * \param[in] IXhazard Ixhazard.
 */
static void hazard_hit(chqstate_t *state, hazard_t *IXhazard)
{
  // $ACDB
  static const u8 table_acdb[] = {
    0x19, 0x28, 0x32, 0x37, 0x39, 0x37, 0x32, 0x28,
    0x19, 0x00, 0x0F, 0x19, 0x1F, 0x22, 0x24, 0x22,
    0x1F, 0x19, 0x0F, 0x00, 0x0A, 0x10, 0x13, 0x15,
    0x13, 0x10, 0x0A, 0x00, 0x06, 0x09, 0x0B, 0x09,
    0x06, 0x00, 0x02, 0x00, 0x02, 0x00, 0x01, 0x00
  };

  // $AD03
  //
  // pairs of (hazard_lane_OR_perp_dist_hi, current_lane) for different speed ranges and hit types (normal vs fast)?
  // current_lane byte isn't a lanes byte though
  static const u8 table_ad03[5 * 2] = {
    0x06, 0x22,
    0x0C, 0x1C,
    0x0E, 0x14,
    0x12, 0x0A,
    0x14, 0x00
  };

  int       flags;    /* was A */
  int       hit_timer;     /* was A */
  int       speed;    /* was DE, BC */
  int       index;    // added
  const u8 *ptable;   /* was HL */

  flags = IXhazard->hazard_flags;
  if (flags == 0) {
    hit_timer = IXhazard->hit_timer;
    if (hit_timer == 0)
      return;

    // If we arrive here then a hit has occurred.
    speed = state->speed;
    if (hit_timer < 0)
      speed = 280;

    index = ((speed >> 7) & 3) + (speed & 1); // CHECK - not convinced
    ptable = &table_ad03[index * 2];

    IXhazard->hazard_lane_OR_perp_dist_hi        = ptable[0];
    IXhazard->current_lane = ptable[1];

    speed *= 2;
    if ((speed >> 8) >= 2) // checking speed >= 512?
      speed = SPEED_PERP_CHASE;
    IXhazard->speed = (IXhazard->speed & 0xFF00) | (speed &
                      0x00FF); // set bottom byte only (weird)
    if (++IXhazard->hit_timer) // hit counter
      IXhazard->speed = (IXhazard->speed & 0x00FF) | (speed &
                        0xFF00); // set top byte only
    IXhazard->distance++;

    start_sfx(state, EFFECT_HAZARD_HIT, 3);

    IXhazard->hazard_flags = 2;
  }

  if (--flags == 0)
    return;

  IXhazard->hit_wobble = table_acdb[IXhazard->hazard_lane_OR_perp_dist_hi++];
  IXhazard->speed -= IXhazard->speed / 32;
  IXhazard->inverted ^= 1;
  if (--IXhazard->current_lane)
    return;

  IXhazard->speed    = 0;
  IXhazard->inverted = 1;
  IXhazard->hazard_flags    = 1;
}

/**
 * $AD0D: Check hazard collisions
 *
 * \param[in] state Pointer to game state.
 */
static void check_hazard_collisions(chqstate_t *state)
{
  hazard_t *hazard;     /* was IX */
  int       iterations; /* was B */

  if (state->inhibit_collision_detection)
    return;

  // Iterate over all hazards.
  hazard = &state->hazards[0];
  iterations = 6;
  do {
    if (hazard->used != HAZARD_UNUSED) {
      // hazard_flags is a delay of some sort used for hits
      // hazard_lane_OR_perp_dist_hi is the perp distance high byte when hazard_flags==0xFF
      if (hazard->hazard_flags == 0xFF && hazard->hazard_lane_OR_perp_dist_hi)
        goto chc_continue;

      // Distance is < 20.
      // There was a collision.
      // hazard_flags ?
      if (hazard->distance < 20 &&
          check_collision(state, 0, 0, hazard, NULL) > 0 &&
          hazard->hazard_flags != 0xFF)
        hazard->hit_handler(state, hazard);
    }

chc_continue:
    hazard++;
  } while (--iterations > 0);
}

/**
 * $AD51: Check collision
 *
 * \param[in] state          Pointer to game state.
 * \param[in] default_retval Default retval. (was D)
 * \param[in] HL             Hl.
 * \param[in] hazard         Hazard. (was IX)
 * \param[in] HLout          Hlout.
 * \return Non-zero on success.
 */
static u8 check_collision(chqstate_t *state,
                          int          default_retval,
                          int         HL,
                          hazard_t   *hazard,
                          u16        *HLout)
{
  int horz_pos;      /* was L */
  int horz_clip;     /* was H */
  int flags;         /* was A */
  int distance;      /* was A */
  int max_distance;  /* was C */
  int new_hit_timer; /* was E */
  int fast_counter;  /* was A */
  int Ahorz_pos;     /* was A */

  if (HLout) *HLout = HL;

  if (hazard->hit_timer) // hit counter / delay thing
    return default_retval;

  horz_pos = hazard->horz_pos;
  horz_clip     = hazard->horz_clip;
  if (HLout) *HLout = (horz_clip << 8) | horz_pos;

  if (hazard->horz_clip) // distance related
    return default_retval;

  flags = hazard->hazard_flags + 1; // just for compare
  distance = hazard->distance;
  max_distance = (flags != 0) ? 3 : 2;
  if (distance >= max_distance)
    return default_retval;

  new_hit_timer = 4;
  distance--;
  fast_counter = state->fast_counter;
  if (distance == 0) {
    if ((s8) fast_counter < 0) /* was JP P - why treating fast_counter as signed? */
      new_hit_timer = 1;
  } else {
    if ((s8) fast_counter >=
        0) /* was RET P - why treating fast_counter as signed? */
      return default_retval;
  }

  // Check horizontal position
  Ahorz_pos = horz_pos & 0xF8;
  if (Ahorz_pos >= 144)
    return default_retval;

  Ahorz_pos += hazard->hittable.width;
  if (Ahorz_pos <= 112)
    return default_retval;

  Ahorz_pos -= hazard->hittable.width;
  if (Ahorz_pos >= 104) {
    new_hit_timer--;
    if (Ahorz_pos >= 120)
      new_hit_timer += 2;
  }

  hazard->hit_timer = new_hit_timer;
  return 1;
}

/**
 * $ADA0: Draw all hazards
 *
 * \param[in] state Pointer to game state.
 */
static void draw_all_hazards(chqstate_t *state)
{
  const u8 *height_table; /* was IY */
  hazard_t *hazard;     /* was IX */
  int       iterations; /* was B */

  state->n_hazards = 0;
  height_table = &state->height_table[0];
  hazard = &state->hazards[0];
  iterations = 6;
  do {
    if (hazard->used == HAZARD_USED)
      dh_draw_one_hazard(state, hazard, height_table); // called with regs banked
    hazard++;
  } while (--iterations > 0);
}

/**
 * $ADBE: DH draw one hazard
 *
 * \param[in] state    Pointer to game state.
 * \param[in] IXhazard Ixhazard.
 * \param[in] IYbase   Iybase.
 */
static void dh_draw_one_hazard(chqstate_t *state,
                               hazard_t   *IXhazard,
                               const u8   *IYbase)
{
  int       carry = 0;
  int       C;
  int       Aflags;
  u8        A;
  u8        B;
  int       BCwords;
  int       DE;
  u16       HL;
  int       Biterations;
  u8       *HLp_n_hazards;
  int       An_hazards;
  u16      *HLtable;
  const u8 *IY;
  u16       HLresult;
  int       Ddistance;
  int       Edist_frac;
  u16      *DEtable;

  C = IXhazard->speed >> 8; // top byte of horz position or accel?
  IXhazard->dist_frac -= IXhazard->speed & 0xFF;
  if ((s8) IXhazard->dist_frac < 0) // carried
    C++;
  C += IXhazard->distance;
  Aflags = IXhazard->hazard_flags + 1;
  if (Aflags == 0) {
    A = IXhazard->hazard_lane_OR_perp_dist_hi;
    if (C < IXhazard->distance) { // carried
      A++;
      if (A >= 5) {
        A--;
        C = 0xFF;
      }
      IXhazard->hazard_lane_OR_perp_dist_hi = A;
    }
    int is_zero = (A == 0);
    A = C;
    if (is_zero)
      goto dh_adfa;
    IXhazard->distance = A;
    return;
  }

  A = C;
  if (A >= 23) {
    // Hazard gone
    IXhazard->used = HAZARD_UNUSED;
    return;
  }

dh_adfa:
  IXhazard->distance = A;
  if (A >= 20)
    return;

  if (--A == 0) {
    A = ~(state->fast_counter & 0xE0);
    if (A < IXhazard->dist_frac) {
      B = IXhazard->hazard_flags + 1;
      if (B) {
        // Wipe the hazard because car overtaken?
        IXhazard->used = HAZARD_UNUSED;
        RL(B);
        if (carry)
          state->overtake_bonus_counter++;
        return;
      }
      IXhazard->dist_frac = A;
    }
    A = 0;
  }
  A += 0x4E;

  IY = &IYbase[A];
  C = IY[1];
  A = C - IY[0];

  // This is probably equivalent to HLresult = IXhazard->dist_frac * A;
  DE = A; // multiplier
  HLresult = 0; // result
  A = IXhazard->dist_frac; // multiplicand
  Biterations = 8;
  do {
    RL(A);
    if (carry)
      HLresult += DE;
    HLresult <<= 1;
  } while (--Biterations > 0);
  A = HLresult >> 8; // high part of result
  RR(A);

  IXhazard->persp_col = A;
  A = ~((C - A) << 1);

  // Would this fetch from the wrong position?
  // It's loading D, moving down, then loading E...
  HLtable = &state->xpos_road_left[A / 2]; // road drawing left
  DE = *HLtable;
  HLtable = &state->xpos_road_right[A / 2];
  HL = *HLtable;

  state->dh_road_left_xpos = DE;

  // This is probably equivalent to HLresult = IXhazard->horz_pos_on_road * DE;
  DE = HL - DE; // might be road width
  HLresult = 0; // result
  A = IXhazard->horz_pos_on_road;
  Biterations = 8;
  do {
    RL(A);
    if (carry)
      HLresult += DE;
    HLresult <<= 1;
  } while (--Biterations > 0);
  A = HLresult >> 8; // high part of result
  RR(A);

  HL = state->dh_road_left_xpos + A;
  (void) check_collision(state, /*D*/0, HL, IXhazard,
                         &HL); // This modifies HL, not sure how to handle
  IXhazard->distance = HL & 0xFF;
  IXhazard->horz_clip     = HL >> 8;
  Ddistance = IXhazard->distance;
  Edist_frac     = IXhazard->dist_frac;

  HLp_n_hazards = &state->n_hazards;
  An_hazards = *HLp_n_hazards;
  (*HLp_n_hazards)++;

  HLtable = &state->xpos_road_centre_left[0]; // road centre left?
  if (An_hazards) {
    Biterations = An_hazards;
    do {
      HLtable++;
      if (Ddistance >=
          HLtable[-1]) { // these offsets are bound to be wrong due to byte/word
        if (Ddistance != HLtable[-1])
          goto dh_insert;

        if (Edist_frac < *HLtable)
          goto dh_insert;
      }
      HLtable += 3;
    } while (--Biterations > 0);
  }

  // Add/store hazard
  *HLtable++ = Edist_frac | (Ddistance << 8); // big endian store?
  *HLtable++ = IXhazard -
               &state->hazards[0]; // Conv: now stores an offset, not a ptr
  goto dh_call_handler;

dh_insert: // deleting or inserting a new hazard?
  // PUSH DE - Ddistance, Edist_frac
  BCwords = Biterations *
            2; // entries * sizeof hazard entry (Conv: WORDS to shift down)
  DEtable = HLtable + BCwords + 1;
  HLtable = HLtable + BCwords - 1;
  do { *HLtable-- = *DEtable--; } while (--BCwords > 0);
  HLtable = DEtable; // was EX DE,HL
  *HLtable-- = IXhazard -
               &state->hazards[0]; // Conv: now stores an offset, not a ptr
  // POP DE - Ddistance, Edist_frac
  *HLtable-- = Edist_frac | (Ddistance << 8); // big endian store?

dh_call_handler:
  IXhazard->hit_handler(state, IXhazard);
}

/**
 * $AECF: Draw arrow fire smoke
 *
 * \param[in] state       Pointer to game state.
 * \param[in] Biterations Biterations.
 * \param[in] IYheight          IYheight register value.
 */
static void draw_arrow_fire_smoke(chqstate_t *state,
                                  int          Biterations,
                                  const u8   *IYheight)
{
  // $CDEC
  //
  // 4 pair of X,Y - are these signed?
  static const u8 arrow_offsets[4 * 2] = {
    0xDE, 0x30,
    0xE6, 0x20,
    0xEA, 0x18,
    0xEE, 0x10
  };

  // $CDF4
  static const bitmap_t *fire_bitmaps[SPRITE_FRAMES] = {
    &fire5_defns[0],
    &fire6_defns[0],
    &fire3_defns[0],
    &fire4_defns[0],
    &fire1_defns[0],
    &fire2_defns[0]
  };

  // $CE00
  //
  // 6 pair of X,Y - are these signed?
  static const u8 smoke_offsets[6 * 2] = {
    0xEE, 0x08,
    0xF3, 0x08,
    0xF8, 0x08,
    0xFA, 0x04,
    0xFC, 0x00,
    0xFE, 0x00
  };

  u16            *HLtable;             /* was HL */
  int             A;                   /* was A */
  u8              Awidth_bytes;        /* was A */
  int             Asmash_level;        /* was A */
  int             DEbitmapoffset;      /* was DE */
  const bitmap_t *HLbitmap;            /* was HL */
  int             Ewidth_bits;         /* was E */
  const hazard_t *IXhazard;            /* was IX */
  u8              Bx;                  /* was B */
  u8              Cy;                  /* was C */
  int             Ahorz_clip;          /* was A */
  const u8       *HLarrows;            /* was HL */
  const u8       *HLsmokes;            /* was HL */
  u8              Ahorz_pos;           /* was A */
  int             Asmash_level_scaled; /* was A */

  // is $E900 pairs of (data-word, hazard-ptr) ?

  HLtable = state->dh_xpos_table; // sampled = $E900
  A = Biterations;
  if (A != *HLtable) // this is a word, original tested a byte, use *HLtable & 0xFF perhaps?
    return;

  if (--A >= 11)
    A = 10;
  A >>= 1;
  state->smoke_bitmap_index = A; // speed factor?

  DEbitmapoffset = A * 7;
  do {
    HLtable++; // Conv: halved
    // Conv: adjusted to expect an offset
    IXhazard =
      &state->hazards[*HLtable++]; // sampled = $A19C/hazard_1 $A1B0/hazard_2

    // PUSH HL (HLtable), BC (Biterations), DE (DEbitmapoffset)

    HLbitmap = &IXhazard->hittable.bitmaps[DEbitmapoffset / 7];

    Ewidth_bits = HLbitmap->width_bytes << 3;
    state->doc_col_pos = IXhazard->persp_col - IXhazard->hit_wobble;
    state->doc_inverted = IXhazard->inverted;

    if (IXhazard->hazard_flags + 1 == 0)
      goto dh_af50;

    Ahorz_clip = IXhazard->horz_clip;
    // AND A3
    Ahorz_pos = IXhazard->horz_pos;
    if (Ahorz_clip < 0)
      goto dh_af2f;
    if (Ahorz_clip != 0)
      goto dh_draw_done_1;
    if (Ahorz_pos >= 128)
      goto dh_draw_right_1;

    Ahorz_pos += Ewidth_bits;
    goto dh_draw_left_1;

dh_af2f:
    Ahorz_pos += Ewidth_bits;
    if (Ahorz_pos + Ewidth_bits < 0x100) // no carry
      goto dh_draw_done_1;

dh_draw_left_1:
    draw_object_left_helicopter_entrypt(state, Ahorz_pos, HLbitmap, IYheight);
    goto dh_draw_done_1;

dh_draw_right_1:
    draw_object_right_helicopter_entrypt(state, Ahorz_pos, HLbitmap, IYheight);

dh_draw_done_1:
    // POP DE (DEbitmapoffset), BC (Biterations)   ??

    state->doc_inverted = 0;

    if (--state->n_hazards == 0)
      return; // no more hazards

    // POP HL (HLtable)
  } while (*HLtable == Biterations); // again, test low byte only here?

  state->dh_xpos_table = HLtable;
  return;

dh_af50:
  Awidth_bytes = IXhazard->horz_clip;
  state->dh_SM_B029_horz_clip = Awidth_bytes;
  // set flags from A here
  Ahorz_pos = IXhazard->horz_pos;
  state->dh_SM_B02C_horz_pos = Ahorz_pos;
  if ((s8) Awidth_bytes < 0)
    goto dh_af6c;
  if (Awidth_bytes)
    goto dh_draw_done_1;
  if (Ahorz_pos >= 128)
    goto dh_draw_right_2;
  Awidth_bytes += Ewidth_bits;
  goto dh_draw_left_2;

dh_af6c:
  Awidth_bytes += Ewidth_bits;
  if ((s8) Awidth_bytes < 0)
    goto dh_draw_done_1;

dh_draw_left_2:
  draw_object_left_helicopter_entrypt(state, Awidth_bytes, HLbitmap, IYheight);
  goto dh_done_draw_object;

dh_draw_right_2:
  draw_object_right_helicopter_entrypt(state, Awidth_bytes, HLbitmap, IYheight);

dh_done_draw_object:
  state->dh_col_pos = state->doc_col_pos;

  if (state->smash_level >= 5)
    goto dh_smash_level;

  A = state->smoke_bitmap_index;
  if (A >= 4)
    goto dh_smash_level;

  HLarrows = &arrow_offsets[A]; // Conv: scaling accounted for
  Bx = HLarrows[0]; // x offset
  Cy = HLarrows[1]; // y offset
  dh_draw_bitmap(state, Bx, Cy, &floating_arrow_here_defn, IYheight);

dh_smash_level:
  Asmash_level = state->smash_level;
  if (Asmash_level < 4)
    goto dh_check_smash_level;

  Asmash_level_scaled = (Asmash_level - 4) * 4;

  // EX AF,AF' Bank Asmash_level_scaled

  HLsmokes = &smoke_offsets[state->smoke_bitmap_index * 2];
  Bx = HLsmokes[0];
  Cy = HLsmokes[1];

  // EX AF,AF' Unbank Asmash_level_scaled

  // Conv: shuffled around
  Asmash_level_scaled += (state->slow_anim_counter & 1) * 2;
  HLbitmap = fire_bitmaps[Asmash_level_scaled / 2];

  // POP DE (DEbitmapoffset)
  // PUSH DE (DEbitmapoffset)

  dh_draw(state, Bx, Cy, DEbitmapoffset, HLbitmap, IYheight);

dh_check_smash_level:
  /* Conv: Converted to switch */
  switch (state->smash_level) {
  case 3:
    dh_smoke(state, state->smokes[2], IYheight);
  case 2:
    dh_smoke(state, state->smokes[0], IYheight);
  case 1:
    dh_smoke(state, state->smokes[1], IYheight);
  case 0:
    goto dh_draw_done_1;
  default:
    assert(0);
  }
}

/**
 * $AFF1: DH smoke
 *
 * Decrements a counter 5..1 then repeats this must be the car-on-fire animation.
 *
 * \param[in] state   Pointer to game state.
 * \param[in] HLsmoke Hlsmoke.
 * \param[in] IYheight Height table pointer. (was IY)
 */
static void dh_smoke(chqstate_t *state, u8 *HLsmoke, const u8 *IYheight)
{
  int counter;   /* was A, E */
  int index;     /* was A */
  int newindex;  /* was A, C, D */
  u8  x;         /* was B */
  u8  y;         /* was C */

  counter = HLsmoke[0] - 1;
  if (counter <= 0)
    counter = 5; // It became zero, reset to 5
  HLsmoke[0] = counter;

  index = state->smoke_bitmap_index; // smoke animation index
  newindex = index + counter;
  if (newindex >= 6)
    return;

  x = HLsmoke[1 + index * 2] - counter;
  y = HLsmoke[1 + index * 2 + 1];
  dh_draw(state, x, y, newindex * 7, &smoke_defns[0],
          IYheight); /* was fallthrough */
}

/**
 * $B01B: DH draw
 *
 * B,C = x,y offset/position? HL -> graphic definition.
 *
 * \param[in] state     Pointer to game state.
 * \param[in] Bx        Bx.
 * \param[in] Cy        Cy.
 * \param[in] DEoffset  Deoffset.
 * \param[in] HLbitmaps Source bitmap data.
 * \param[in] IYheight        IYheight register value.
 */
static void dh_draw(chqstate_t     *state,
                    int              Bx,
                    int              Cy,
                    int             DEoffset,
                    const bitmap_t *HLbitmaps,
                    const u8       *IYheight)
{
  const bitmap_t *HLbitmap;

  HLbitmap = &HLbitmaps[DEoffset / 7];
  dh_draw_bitmap(state, Bx, Cy, HLbitmap, IYheight);
}

/**
 * $B01C: DH draw bitmap
 *
 * \param[in] state    Pointer to game state.
 * \param[in] Bx       Bx.
 * \param[in] Cy       Cy.
 * \param[in] HLbitmap Source bitmap data.
 * \param[in] IYheight       IYheight register value.
 */
static void dh_draw_bitmap(chqstate_t     *state,
                           int              Bx,
                           int              Cy,
                           const bitmap_t *HLbitmap,
                           const u8       *IYheight)
{
  int Ewidth_bits;
  int A1;
  u8  A2;

  Ewidth_bits = HLbitmap->width_bytes * 8;
  state->doc_col_pos = state->dh_col_pos + Bx;
  A1 = state->dh_SM_B029_horz_clip;
  // Set flags for A here
  A2 = state->dh_SM_B02C_horz_pos;
  if (A1 < 0)
    goto dh_exit_2;
  if (A1)
    return;

  A2 += Cy;
  if (A2 < Cy) // carried
    return;
  if (A2 >= 128) {
    draw_object_right_helicopter_entrypt(state, A2, HLbitmap,
                                         IYheight); /* was exit via */
    return;
  }

dh_exit_1:
  A2 += Ewidth_bits; // add pixel width
  draw_object_left_helicopter_entrypt(state, A2, HLbitmap,
                                      IYheight); /* was exit via */
  return;

dh_exit_2:
  A2 += Cy;
  if (A2 < Cy) // carried
    goto dh_exit_1;

  A2 += Ewidth_bits;
  if (A2 > Ewidth_bits) // carried
    draw_object_left_helicopter_entrypt(state, A2, HLbitmap,
                                        IYheight); /* was exit via */
}

/**
 * $ADF9: Conv: Original game used the RET at $ADF9 as a no-op
 *
 * \param[in] state  Pointer to game state.
 * \param[in] hazard Hazard.
 */
void no_op(chqstate_t *state, hazard_t *hazard)
{
}

/**
 * $B063: Move hero car
 *
 * \param[in] state Pointer to game state.
 */
static void move_hero_car(chqstate_t *state)
{
  // Gear-speed thresholds
  const int SpeedIdleChatter  =
    120; // trigger "get moving" chatter below this speed
  const int SpeedOffRoad1     = 110; // max speed with one wheel off-road
  const int SpeedOffRoad2     = 120; // max speed with both wheels off-road
  const int SpeedHighGearMin  =
    220; // high gear uses two different max speeds above/below this
  const int SpeedLowGear      =
    230; // low gear max without boost; high gear max below SpeedHighGearMin
  const int SpeedBoosted      =
    470; // max speed with boost active (low gear, or high gear < SpeedHighGearMin)
  const int SpeedHighGear     = 360; // high gear max without boost
  const int SpeedHighGearBoosted = 695; // high gear max with boost

  // TODO Sort these decls by use
  int        y_offset;             /* was A */
  const u8  *jump_data;            /* was HL */
  int        boost;                /* was A */
  int        Cinput;               /* was C */
  int        Ainput;               /* was A */
  u8        *pgear;                /* was HL */
  int        smoke;                /* was A */
  int        gear_lockout;         /* was A */
  int        gear;                 /* was A */
  int        speed;                /* was HL */
  int        off_road;             /* was A */
  int        BCmax_speed;          /* was BC */
  int        BCspeed_diff;         /* was BC */
  int        Ainclined;            /* was A */
  int        Apitch;               /* was A */
  int        Cleft_turn;           /* was C */
  int        Hinput;               /* was H */
  int        Bright_turn;          /* was B */
  int        BCpitch_speed_delta;  /* was BC */
  int        DEoldspeed;           /* was DE */
  int        Bturn_speed;          /* was B */
  int        Dflip_car;            /* was D */
  int        Acornering;           /* was A */
  int        Acurrent_curvature;   /* was A */
  const u8  *HLhorizon_table;      /* was HL */
  int        saved_Cleft_turn;     /* was C */
  int        saved_Bright_turn;    /* was B */
  int        Acrashedflag;         /* was A */
  int        Aturn_speed;          /* was A */
  int        Bcount;               /* was B */
  int        Chorizon_scroll_sub;  /* was C */
  int        Enegative_scrolling;  /* was E */
  int        Ahorizon_scroll_sub;  /* was A */
  int        Acounter;             /* was A' */
  int        Chorz_tab_value;      /* was C */
  int        Acount;               /* was A' */
  int        BCcount_scaled;       /* was BC */
  int        HLhorizontal_adjust;  /* was HL */
  int        DEadjust;             /* was DE */

  y_offset = state->mhc_y_offset; // load jump counter, highest is 8
  if (y_offset) {
    state->mhc_y_offset = --y_offset;
    if (y_offset == 0) {
      // Hero car has landed
      state->smoke = 3;
      start_sfx(state, EFFECT_LANDING, 3); /* priority 3 */
    }

    // Hero car is in mid-air, or has just landed
    jump_data = state->mhc_jump_data; // points into hero_car_jump_table
    state->off_road = 0;
    state->user_input &= ~(USERINPUT_RIGHT | USERINPUT_LEFT | USERINPUT_DOWN |
                           USERINPUT_UP);
    state->dhc_pitch = jump_data[0];
    y_offset = state->dhc_jump_y + jump_data[1];
    state->mhc_jump_data = jump_data + 2;
  }

  state->dhc_jump_y = y_offset;

  if (state->boost && --state->boost == 0) // Conv: Uses state directly
    state->session.turbos--;

  // Handle smoke effect
  if (state->smoke) // Conv: Uses state directly
    state->smoke--;

  // Handle gear changes
  Cinput = state->user_input;
  if (state->ahc_crashed_flag)
    Cinput &= USERINPUT_FIRE;

  Ainput = Cinput;
  // PUSH Ainput (PUSH AF)
  const int fire_pressed = (Ainput & USERINPUT_FIRE);
  pgear = &state->gear; // could use state
  if (fire_pressed != 0 && state->gear_lockout == 0) {
    *pgear ^= 1; // Toggle gear flag
    smoke = gear_lockout = 4;
    if (*pgear)
      state->smoke = smoke;
  } else {
    gear_lockout = 0; // Conv: Added
  }

  if ((s8) --gear_lockout >= 0)
    state->gear_lockout = gear_lockout;

  gear = *pgear;
  // EX AF,AF
  speed = state->speed;
  if (speed < SpeedIdleChatter &&
      state->perp_caught_phase == 0 &&
      --state->session.idle_timer == 0) {
    state->session.idle_timer = 100;
    start_chatter(state, 10, &chatterblk_raymond_get_moving[0]);
  }
  // DEspeed = HLspeed; // might not need
  off_road = state->off_road;
  if (off_road) {
    // Handle off-road (Aoff_road can be 1 or 2 here)
    BCmax_speed = (off_road == 1) ? SpeedOffRoad1 : SpeedOffRoad2;
    if (speed >= BCmax_speed) {
      // Conv: Removed RR/RLA sequence.
      BCspeed_diff = -(((speed >> 5) & 0x0F) | 1);
      goto mhc_check_brake;
    }
  }

  boost = state->boost;
  // EX AF,AF'
  if (!gear) {
    BCmax_speed = SpeedBoosted;
    // EX AF,AF' (unbank boost+flags)
    if (!boost) // No turbo boost
      BCmax_speed = SpeedLowGear;

mhc_low_gear_slowing:
    if (speed < BCmax_speed)
      BCspeed_diff = ((-speed >> 4) & 0x3F) | 1;
    else
      BCspeed_diff = -(((speed >> 4) & 0x1F) | 1);
  } else {
    if (speed < SpeedHighGearMin) { // mhc_high_gear_slowing
      BCmax_speed = SpeedBoosted;
      // EX AF,AF' (unbank boost+flags)
      if (!boost)
        BCspeed_diff = ((speed >> 4) | 1) & 0x1F;
    } else {
      BCmax_speed = SpeedHighGearBoosted;
      // EX AF,AF' (unbank boost+flags)
      if (!boost)
        BCmax_speed = SpeedHighGear;
    }
    goto mhc_low_gear_slowing; // jumps backwards!
  }

mhc_check_brake:
  Ainput = Cinput; // Conv: was POP AF-PUSH AF
  if (Ainput & USERINPUT_DOWN) // checks BRAKE key
    BCspeed_diff = -20; // braking
  else if ((Ainput & USERINPUT_UP) == 0) // accelerate NOT pressed
    BCspeed_diff = -10; // slow down at half the speed of braking

  speed += BCspeed_diff;
  if (speed < 0)
    speed = 0; // clamp to zero

  Ainclined = state->inclined_counter - 1;
  if ((s8) Ainclined < 0) {
    Apitch = state->dhc_pitch;
    if (Apitch == 0 || speed == 0) {
      Ainclined = 0; // don't adjust speed if car's halted?
    } else {
      // New speed is non-zero
      BCpitch_speed_delta = (Apitch - 5) | 1; // 0/3/6 in Cpitch => -5/-1/1
      DEoldspeed = speed;
      speed += BCpitch_speed_delta;
      if (speed >= 695) // seems high?
        speed = DEoldspeed; // clamp to max
      Ainclined = 3;
    }
  }
  state->inclined_counter = Ainclined;
  state->speed = MIN(speed, 511); // clamp to 511 max

  Hinput = Cinput; /* was POP HL (get user input) */
  Bright_turn = state->right_turn;
  Cleft_turn  = state->left_turn;
  if (state->mhc_y_offset == 0) { // if not in the air?
    if (Hinput & USERINPUT_RIGHT)
      goto mhc_turning_right;
    if (Hinput & USERINPUT_LEFT)
      goto mhc_turning_left;

    // User input is not left or right

    // Reduce left turning force
    Cleft_turn  = MAX(Cleft_turn - 9, 0);
    // Reduce right turning force:
    Bright_turn = MAX(Bright_turn - 9, 0);
    goto mhc_handle_speed;

mhc_turning_right:
    // = MIN(36, B + 4) -- 36 is the max turning force
    Bright_turn = ((Bright_turn + 4 >= 36) ? 36 : Bright_turn + 4);
    // = MAX(C - B, 0)
    Cleft_turn  = ((Cleft_turn >= Bright_turn) ? Cleft_turn - Bright_turn : 0);
    goto mhc_handle_speed;

mhc_turning_left:
    // = MIN(36, C + 4)
    Cleft_turn  = ((Cleft_turn + 4 >= 36) ? 36 : Cleft_turn + 4);
    // = MAX(B - C, 0)
    Bright_turn = ((Bright_turn >= Cleft_turn) ? Bright_turn - Cleft_turn : 0);

mhc_handle_speed:
    speed = state->speed >> 3; // a new use of 'speed'
    Aturn_speed = speed + (speed >> 1);
    if (Aturn_speed < Bright_turn)
      Bright_turn = Aturn_speed;
    if (Aturn_speed < Cleft_turn)
      Cleft_turn = Aturn_speed;
  }

  // Store turning forces for later (was PUSH BC)
  saved_Bright_turn = Bright_turn;
  saved_Cleft_turn  = Cleft_turn;

  // initing Bcount and BCcount_scaled here
  Bcount = BCcount_scaled = Enegative_scrolling = 0;
  Acurrent_curvature = state->current_curvature;
  if (Acurrent_curvature) {
    if ((s8) Acurrent_curvature < 0) {
      // Negative scroll => scroll horizon right.
      Enegative_scrolling++; // 0 -> 1
      Acurrent_curvature = -Acurrent_curvature;
    }

    // Positive scroll => scroll horizon left. (or negative - it falls through)
    // unclear if this table is bytes or words
    // -1 since it's 1-indexed (but now it's words so can this work?)
    HLhorizon_table = &horizon_table[Acurrent_curvature - 1];
    Chorizon_scroll_sub = Ahorizon_scroll_sub = state->horizon_scroll_sub;
    // EX AF,AF'
    Acounter = state->fast_counter - Chorizon_scroll_sub;
    if (Acounter) {
      Chorz_tab_value = *HLhorizon_table;
      for (;;) {
        Acounter -= Chorz_tab_value;
        if ((s8) Acounter < 0)
          break;
        Bcount++;
        // EX AF,AF'
        Ahorizon_scroll_sub += Chorz_tab_value;
        // EX AF,AF'
      }

      Acount = Bcount; // could perhaps merge Acount and Bcount
      if (Acount) {
        state->curvature_ticks += Acount;

        BCcount_scaled = Bcount * 3; /* was A and B */
        if (Enegative_scrolling)
          BCcount_scaled = -BCcount_scaled;
      }

      // EX AF,AF'
      state->horizon_scroll_sub = Ahorizon_scroll_sub;
    }
  }

  // No curvature - No scroll required?
  HLhorizontal_adjust = state->horizontal_adjust + BCcount_scaled;
  DEadjust = 0;
  state->horizontal_adjust = 0;
  Acrashedflag = state->ahc_crashed_flag;
  if (Acrashedflag) {
    /* was EX DE,HL */
    DEadjust = HLhorizontal_adjust & 0xFF;
    HLhorizontal_adjust = 0;
  }

  state->right_turn = saved_Bright_turn; /* was POP BC */
  state->left_turn  = saved_Cleft_turn;
  Acrashedflag -= saved_Bright_turn; // Seems odd
  if ((s8) Acrashedflag < 0)
    DEadjust = 0xFF00;
  DEadjust = (DEadjust & 0xFF00) | (Acrashedflag >> 1); /* was SRA */
  HLhorizontal_adjust += DEadjust;
  if (Acrashedflag && (s8) Acrashedflag < 0)
    Acrashedflag = -Acrashedflag;

  Acornering = 0;
  if (Acrashedflag >= 17) {
    if ((s16) HLhorizontal_adjust < 0) {
      if ((s16) DEadjust < 0)
        goto mhc_set_cornering;
      // Conv: Removed duplicated test from here.
    } else {
      if ((s16) DEadjust >= 0 || (s16) HLhorizontal_adjust < 0)
        goto mhc_set_cornering;
    }
    Acornering = 1;
  }

mhc_set_cornering:
  state->cornering = Acornering;
  state->scenedata.road_pos += HLhorizontal_adjust;
  Dflip_car = 1;
  if (Enegative_scrolling < 0) {
    Dflip_car = 0;
    Acornering = -Acornering;
  }

  // This could be replaced with a division by six.
  Bturn_speed = (Acornering < 12) ? (Acornering < 6) ? 0 : 1 :
                2; // straight/turn/turn-hard

  state->turn_speed = Bturn_speed; // should be 0/1/2
  state->flip_car   = Dflip_car;   // should be 0/1
  if (state->mhc_y_offset)
    state->cornering = 0; // reset cornering if jumping
}

/**
 * $B318: Animate hero car
 *
 * \param[in] state Pointer to game state.
 */
static void animate_hero_car(chqstate_t *state)
{
  int HLspeed;            /* was HL */
  int Acrashed_flag;      /* was A */
  int Aturn_speed;        /* was A */
  int HL_b356;            /* was HL */
  int DE_b356;            /* was DE */
  int HLroad_pos;         /* was HL */
  int Cflip_flag;         /* was C */
  int Adelay;             /* was A */
  int DEother_road_pos;   /* was DE */
  int A;                  /* was A */
  int Aperp_caught_phase; /* was A */
  int Aflipping;          /* was A */
  int Cflipping;          /* was C */
  int Acounter_A;         /* was A */
  int Bdash_anim_counter; /* was B */
  int Cdash;              /* was A */
  int Bwobble;            /* was B */
  int Bsmoke_anim_frame;  /* was B */

  HLspeed = state->speed;
  if (HLspeed > 0) {
    state->ahc_crash_spin = 0;
    state->off_road = 0;
  }

  Acrashed_flag = state->ahc_crashed_flag;
  if (Acrashed_flag) {
    state->cornering = Acrashed_flag;
    if (HLspeed < state->ahc_crash_speed_threshold)
      goto ahc_speed_less_or_eq;

    HLspeed -= (HLspeed >> 2) | 3;
    if (HLspeed == 0)
      goto ahc_speed_less_or_eq;

    Aturn_speed = 2; // fastest
    if ((s16) HLspeed < 0) {
ahc_speed_less_or_eq:
      // Otherwise HLspeed < 0
      state->ahc_crashed_flag = 0;
      Aturn_speed = 1;
    } else {
      state->speed = HLspeed;
    }

    state->turn_speed = Aturn_speed;

    HL_b356 = state->ahc_crash_spin_speed - (state->ahc_crash_spin_speed >> 4);
    state->ahc_crash_spin_speed = HL_b356;
    DE_b356 = HL_b356; /* was EX DE,HL */

    HLroad_pos = state->scenedata.road_pos;
    Cflip_flag = state->ahc_flip_flag;
    state->flip_car = Cflip_flag & 1;
    if (Cflip_flag != 1) {
      if (Cflip_flag != 2)
        HLroad_pos += DE_b356; // presumably zero
    } else {
      HLroad_pos -= DE_b356; // case 1
    }
    state->scenedata.road_pos = HLroad_pos;

    // Decrement this counter
    Adelay = state->ahc_delay;
    if (Adelay) {
      state->ahc_delay = --Adelay;
      Adelay = state->ahc_crash_flip_count;
    }
    state->ahc_crash_spin = Adelay;
  }

  // Arrive here if not crashed
  HLroad_pos = state->scenedata.road_pos;
  DEother_road_pos = state->ahc_road_pos_a;
  A = HLroad_pos >> 8;
  if ((s8) A >= 0) {
    if (A == 0) {
      A = (HLroad_pos & 0xFF) - (DEother_road_pos & 0xFF); // low diff
      if ((s8) A < 0)
        goto ahc_b3b0;
    }

    DEother_road_pos = state->ahc_road_pos_b;
    A = HLroad_pos >> 8;
    if (A >= (DEother_road_pos >> 8)) { // carry
      if (A == 0) {
        A = (HLroad_pos & 0xFF) - (DEother_road_pos & 0xFF); // low diff
        if ((s8) A < 0)
          goto ahc_assign_road_pos_2;
      }

ahc_b3b0:
      // EX DE,HLroad_pos
      HLroad_pos = DEother_road_pos;
    }
  }

ahc_assign_road_pos_2:
  state->scenedata.road_pos = HLroad_pos;
  if (state->cornering || state->smoke)
    start_sfx(state, EFFECT_SQUEAL, 5); /* priority 5 */

  Aperp_caught_phase = state->perp_caught_phase;
  if (Aperp_caught_phase) {
    if (--Aperp_caught_phase == 0) /* was 1 */
      goto ahc_load_flip_flag;
    if (Aperp_caught_phase >= 2)
      Aturn_speed = 2;

    state->turn_speed = Aturn_speed;
    state->flip_car   = 1;
  }

  draw_debris(state);

ahc_load_flip_flag:
  Aflipping = state->ahc_crash_spin;
  if (Aflipping) {
    Cflipping = Aflipping * 3 + 24;
    Aturn_speed = state->turn_speed;
    if (Aturn_speed >= 2) {
      if (state->flip_car)
        Cflipping++; // 2 -> 3
      Cflipping++; // 2/3 -> 3/4 // FIXME This isn't used...
    }

    // EXX - bank
    Acounter_A = state->anim_counter;
    Bdash_anim_counter = Acounter_A & 1; // animation counter OR flip flag, not sure
    Cdash = Acounter_A << 1;
    // EXX - unbank

    draw_crash(state, Acounter_A, Bdash_anim_counter, Cdash);
    state->off_road = 0;
  }

  ahc_check_hand_flag(state);

  // Make the car bounce up and down when it goes off-road
  Bwobble = 0;
  if (state->off_road == 1)
    Bwobble = (state->slow_anim_counter & 1) * 3; // half rate counter

  draw_hero_car(state, state->turn_speed, Bwobble);

  if (state->cherry_light)
    draw_cherry_light(state, 0, 1, 2);

  // Check to see if smoke needs drawing
  Bsmoke_anim_frame = state->anim_counter;
  if (state->cornering == 0) {
    // Not cornering
    Bsmoke_anim_frame = state->slow_anim_counter; // could move down
    if (state->boost == 0 && state->smoke == 0 && state->off_road != 2)
      return; // Return if no boost, no smoke and not fully off-road
  }

  draw_smoke(state, Bsmoke_anim_frame, 0); // right hand
  draw_smoke(state, Bsmoke_anim_frame, 1); // left hand; exit via
}

/**
 * $B457: AHC check hand flag
 *
 * \param[in] state Pointer to game state.
 */
static void ahc_check_hand_flag(chqstate_t *state)
{
  int Ahand_flag;      /* was A */
  int Bdash_flip_flag; /* was B */
  int Cdash;           /* was C */
  int Chand_flag;      /* was C */
  int Ahand_frame;     /* was A */
  int Bhand_frame;     /* was B */
  int Chand_frame;     /* was C */

  Ahand_flag = state->hand_flag;
  if (Ahand_flag == HANDFLAG_NONE)
    return;

  if (Ahand_flag != HANDFLAG_ANIMATING) {
    // Show the "stop" hand

    // EXX BANK
    Bdash_flip_flag = Ahand_flag;
    Cdash = Ahand_flag;
    // EXX UNBANK

    // Avoid the hand animation if turning hard?
    if (state->turn_speed != 2)
      draw_crash(state, 36, Bdash_flip_flag, Cdash); /* exit via */
    else
      // Otherwise turn_speed is 2 (turn hard).
      draw_crash(state, state->flip_car + 37, Bdash_flip_flag, Cdash); /* exit via */
    return;
  }

  // Start the animation
  Chand_flag  = state->ahc_hand_step; // zeroed in start_chase
  Ahand_frame = state->ahc_hand_delay - 1; // hand animation frame
  state->ahc_hand_delay = Ahand_frame;
  if (Ahand_frame == 0) {
    Bhand_frame = 2;
    Ahand_frame = ++Chand_flag;
    if (Ahand_frame < 4) {
      Chand_flag = ++Ahand_frame;
      if (Ahand_frame == 2)
        Bhand_frame++;
    }
    state->ahc_hand_delay = Bhand_frame;
  }
  state->ahc_hand_step = Chand_flag;

  if (Ahand_frame >= 7) {
    // Hide the "stop" hand
    state->hand_flag = HANDFLAG_NONE;
    return;
  }

  if (state->turn_speed != 2)
    Ahand_frame = 6;
  else
    Ahand_frame = (state->flip_car * 7) + 13;

  Ahand_frame += Chand_flag;
  // PUSH AF
  draw_crash_unflipped(state, Ahand_frame);
  // POP AF
  Chand_frame = Ahand_frame;
  Ahand_flag = state->ahc_hand_step;
  if (Ahand_flag < 4) {
    // A < 4
    Ahand_flag = ++Chand_frame;
    draw_crash_unflipped(state, Ahand_flag); /* exit via */
  } else {
    state->cherry_light = 1;
  }
}

/**
 * $B4CC: Start chase
 *
 * \param[in] state Pointer to game state.
 */
static void start_chase(chqstate_t *state)
{
  state->ahc_hand_step = 0;
  // Starts the animation that puts the cherry light on the roof
  state->hand_flag = HANDFLAG_ANIMATING;
  // Enable flashing lights and smash bar
  state->sighted_flag = 1;
  // This is animation frame related?
  state->ahc_hand_delay = 2;

  state->session.time_sixteenths = 15;
  state->session.time_bcd        = 0x60;

  // Toggle the left light's brightness
  toggle_light_brightness(state, ADDRTOATTRS(MARQUEELIGHT_LEFT_ATTR_ADDR));

  // Show the "SIGHTING OF TARGET VEHICLE" message
  setup_overlay_messages(state, &sighting_message[0]);

  start_siren_hook(state); /* exit via */
}

/**
 * $B4F0: Smash
 *
 * \param[in] state Pointer to game state.
 */
static void smash(chqstate_t *state)
{
  int counter; /* was A */
  int hits;    /* was A */
  int level;   /* was C */

  counter = (state->smash_cycling_counter + 1) & 3;
  state->smash_cycling_counter = counter;

  // Setup debris_table entry in draw_debris
  state->dd_debris_subtables_start = &state->debris_table[counter * 3];
  state->dd_SM_B549_frame_counter = 9; // set counter

  hits = state->smash_counter + 1;
  if (hits >= SMASHCOUNTER_MAX) {
    fully_smashed(state); /* exit via */
    return;
  }
  if (hits == SMASHCOUNTER_MAX - 1)
    start_chatter(state, 10, &chatterblk_raymond_one_more_time[0]);
  state->smash_counter = hits;

  level = 0;
  if (hits != 0) {
    level++;
    if (hits >= 4) {
      level++;
      if (hits >= 7) {
        level++;
        if (hits >= 11) {
          level++;
          if (hits >= 14) {
            level++;
            if (hits >= 17) {
              level++;
            }
          }
        }
      }
    }
  }
  state->smash_level = level;
}

/**
 * $B549: Draw debris
 *
 * \param[in] state Pointer to game state.
 */
static void draw_debris(chqstate_t *state)
{
  int       Aframe_counter;    /* was A */
  int       Biterations;       /* was B */
  u8      **HLsubtables;       /* was HL */
  u8       *DEsubtable;        /* was DE */
  int       Cframe_offset;     /* was C */
  int       HLoffset;          /* was HL */
  int       BCframe_offset;    /* was BC */
  u8       *HLsubtable;        /* was HL */
  u8        Dy;                /* was D */
  u8        Ex;                /* was E */
  const u8 *HLbitmap;          /* was HL */
  u8        Bheight;           /* was B */
  u8        Cwidth_bytes;      /* was C */
  int       BCdash;            /* was BC */
  u8        Edash_width_bytes; /* was E */

  Aframe_counter = state->dd_SM_B549_frame_counter;
  if (Aframe_counter == 0)
    return;
  state->dd_SM_B549_frame_counter = --Aframe_counter;

  state->dd_frame_offset = Aframe_counter * 2;

  Biterations = 3;
  HLsubtables = state->dd_debris_subtables_start;
  do {
    // PUSH BC
    DEsubtable = *HLsubtables++;
    // PUSH HL

    // First sub-table byte seems to be a 0..3 counter
    Aframe_counter = (*DEsubtable + 1) & 3;
    *DEsubtable++ = Aframe_counter;

    BCframe_offset = Aframe_counter * 12; // sizeof bitmap_debris images
    HLoffset = state->dd_frame_offset;

    // BCframe_offset = Cframe_offset;
    HLsubtable = DEsubtable + HLoffset;
    Dy = *HLsubtable++;
    Ex = *HLsubtable;

    HLbitmap = &bitmap_debris_1[0] +
               BCframe_offset; // TODO: Make bitmap_debris_X an array

    Bheight = 6; // rows
    Cwidth_bytes = 1; // 1 byte wide masked?
    // EXX - bank
    BCdash = 0;
    Edash_width_bytes = 1;
    // EXX - unbank
    draw_part_entry2(state,
                     Bheight,
                     Cwidth_bytes,
                     Dy,
                     Ex,
                     HLbitmap,
                     BCdash >> 8,
                     BCdash & 0xFF,
                     Edash_width_bytes);

    // POP HL // addr
    // POP BC // iterations
  } while (--Biterations > 0);
}

/**
 * $B58E: Draw hero car
 *
 * \param[in] state       Pointer to game state.
 * \param[in] Aturn_speed Speed value.
 * \param[in] Bwobble     Bwobble.
 */
static void draw_hero_car(chqstate_t *state, int Aturn_speed, int Bwobble)
{
  int              Cturn_speed;
  const carpart_t *HLcarpart;
  u8               Dy;
  u8               Ey;
  int              Acar_direction;
  u16              DEbackbuf_addr;
  u8               Bdash_height;
  const u8        *HLbitmap_data;
  u8               Awidth_bytes;
  u16              DEbitmap_stride;
  u16              HLdash_backbuf_addr;
  int              Adash_flip_car;

  assert(Aturn_speed < 3);
  assert(Bwobble == 0 || Bwobble == 3);

  if (Aturn_speed == 0)
    state->flip_car = 0;

  Cturn_speed = Aturn_speed;
  // PUSH BC -- preserve Cturn_speed & Bwobble
  (void) draw_hero_car_part(state,
                            7, /* 56px wide */
                            120,
                            96,
                            &hero_car_shadow[Aturn_speed]);
  // POP BC -- restore

  /* 117 is the car's default vertical position. Smaller values make it move higher. */
  Dy = 117 - state->dhc_jump_y;

  /* Build an index into hero_car_parts[]. Valid indices are 0 to 8 inclusive. */
  Acar_direction = Cturn_speed + Bwobble + state->dhc_pitch;
  if (Acar_direction >= 9)
    Acar_direction -= 9;
  HLcarpart = &hero_car_parts[Acar_direction][0];

  /* Calculate the screen buffer address. */
  // PUSH DE -- preserve Dy (anything in E?)
  Ey = Dy - (state->car_y + HLcarpart->y);
  DEbackbuf_addr = (((Ey & 0x0F) | 0xF0) << 8) | (((Ey & 0x70) << 1) +
                   13); // Conv: merged to one stmt
  // INC HL
  Bdash_height = HLcarpart->rows;
  // INC HL
  // orig only fetched low byte here allowing below PUSH to store HLcarpart
  HLbitmap_data = HLcarpart->bitmap; // Conv: shuffled around
  // INC HL
  // PUSH HLcarpart .. saving halfway through address build
  // PUSH DEbackbuf_addr -- backbuffer plotaddr

  Awidth_bytes = 5; // width & stride
  DEbitmap_stride = 5;
  // EXX - Bank for plot_sprite (DE' = stride, HL' = address of bitmap data)
  HLdash_backbuf_addr =
    DEbackbuf_addr; /* was POP HLbackbuf_addr  -- backbuffer plotaddr */
  // EX AF,AF'
  Adash_flip_car = state->flip_car; // reuse later perhaps?
  if (!Adash_flip_car) {
    // EX AF,AF' -- #REGa is (width in bytes)
    plot_sprite(state,
                Awidth_bytes,
                ADDRTOBACKBUF(HLdash_backbuf_addr),
                Bdash_height,
                DEbitmap_stride,
                HLbitmap_data);
  } else {
    // EX AF,AF'  -- unbank widthbytes
    HLdash_backbuf_addr--; // Adjust back buffer plot address to be a byte earlier
    plot_sprite_flipped(state,
                        Awidth_bytes,
                        ADDRTOBACKBUF(HLdash_backbuf_addr),
                        Bdash_height,
                        DEbitmap_stride,
                        HLbitmap_data);
  }

  // POP HL herocarpart ptr
  // POP DE car vert pos
  HLcarpart++; // INC HL advance to next car part

  // Draw the windscreen (top) part
  HLcarpart = draw_hero_car_part(state, 5, Dy, 104, HLcarpart);
  // Draw the wheels
  HLcarpart = draw_hero_car_part(state, 5, Dy, 104, HLcarpart);
  // Draw left hand side
  HLcarpart = draw_hero_car_part(state, 1, Dy, (!state->flip_car) ? 96 : 144,
                                 HLcarpart);
  // Draw right hand side
  (void) draw_hero_car_part(state, 1, Dy, (state->flip_car) ? 96 : 144,
                            HLcarpart); /* was FALLTHROUGH */
}

/**
 * $B627: Draw a portion of the hero car
 *
 * \param[in] state        Pointer to game state.
 * \param[in] Cwidth_bytes Byte width (drawing and/or stride?). (was C)
 * \param[in] Dy           Y position (in rows). (was D)
 * \param[in] Ex           X position (in pixels). (was E)
 * \param[in] HLpart       Car part to draw. (was HL)
 * \return Next car part. (was HL)
 */
static const carpart_t *draw_hero_car_part(chqstate_t      *state,
    int               Cwidth_bytes,
    int               Dy,
    int               Ex,
    const carpart_t *HLpart)
{
  int       Ay;                   /* was A */
  u8        Dnew_y;               /* was D */
  const u8 *HLbitmap;             /* was HL */
  u8        Bheight;              /* was B */
  int       Bdash_flip_flag;      /* was B' */
  u8        Edash_bitmap_stride;  /* was E' */
  u8        Cdash;                /* was C' */

  Ay = Dy;
  // PUSH DE -- preserve x,y until return
  Dnew_y = Ay - HLpart->y;
  Bheight = HLpart->rows;
  // PUSH HLpart -- orig stacks HLpart+1
  HLbitmap = HLpart->bitmap;
  // PUSH BC -- pres byte width
  // EXX BANK
  // POP BC -- restore byte width
  Bdash_flip_flag = state->flip_car;
  Edash_bitmap_stride = Cwidth_bytes; // width and stride always the same here?
  Cdash = (Bdash_flip_flag) ? Cwidth_bytes - 1 :
          0; // flipped start offset or something?
  // EXX UNBANK
  draw_part(state,
            Bheight,
            Cwidth_bytes,
            Dnew_y,
            Ex,
            HLbitmap,
            Bdash_flip_flag,
            Cdash,
            Edash_bitmap_stride);
  // POP HLpart
  // POP DE -- restore x,y
  return HLpart + 1; // return next row
}

/**
 * $B648: Draw smoke
 *
 * \param[in] state           Pointer to game state.
 * \param[in] Aanim_frame     Animation frame index. (was A)
 * \param[in] Adash_flip_flag Non-zero to draw flipped. (was A')
 */
static void draw_smoke(chqstate_t *state, int Aanim_frame, int Adash_flip_flag)
{
  const carsmokeframe_t *HLframe;
  u8                     Cwidth;
  u8                     Bheight;
  u8                     Dflipped_x;
  u8                     Eunflipped_x;
  const u8              *HLbitmap;
  u8                     Cdash;
  u8                     Bdash_flip_flag;
  u8                     Edash_width_bytes;
  u8                     Ax;

  HLframe = &hero_car_turbo_smoke[Aanim_frame];

  /* Don't draw smoke if car's mid-jump */
  if (state->mhc_y_offset)
    return;

  /* Load dimensions, positions and frame bitmap data pointer */
  Cwidth       = HLframe->width;
  Bheight      = HLframe->height;
  Dflipped_x   = HLframe->flipped_x;
  Eunflipped_x = HLframe->unflipped_x;
  HLbitmap     = HLframe->bitmap;

  // PUSH BC -- preserve width/height
  // EXX - Bank
  // EX AF,AF' - Bank
  Cdash = Cwidth; /* was POP BCdash -- restore width/height */
  Bdash_flip_flag = Adash_flip_flag;
  Edash_width_bytes = Cdash;
  Cdash--; // used if flipped
  if (Adash_flip_flag == 0) { // if not flipped
    Cdash = 0;
    // EXX Unbank
    Ax = Eunflipped_x;
  } else { // if flipped
    // EXX Unbank
    Ax = Dflipped_x;
  }
  draw_part(state,
            Bheight,
            Cwidth,
            119,
            Ax + 127, // centre
            HLbitmap,
            Bdash_flip_flag,
            Cdash,
            Edash_width_bytes); /* exit via */
}

/**
 * $B67C: Draw cherry light
 *
 * \param[in] state        Pointer to game state.
 * \param[in] Aframe_index Sound effect index.
 * \param[in] Bturn_limit  Bturn limit.
 * \param[in] Cturn_delta  Cturn delta.
 */
static void draw_cherry_light(chqstate_t *state,
                              int          Aframe_index,
                              int          Bturn_limit,
                              int          Cturn_delta)
{
  int Aturn_speed; /* was A */
  int Cturn_speed; /* was C */

  Aframe_index += state->slow_anim_counter & 1;

  // EX AF,AF'
  if (state->turn_speed >= Bturn_limit) {
    Aturn_speed = 0;
    if (state->flip_car)
      Aturn_speed += Cturn_delta;
    Aturn_speed += Cturn_delta;
    Cturn_speed = Aturn_speed;
    // EX AF,AF'
    Aframe_index += Cturn_speed;
    // EX AF,AF'
  }
  // EX AF,AF'
  draw_crash_unflipped(state, Aframe_index); /* was FALLTHROUGH */
}

/**
 * $B699: Draw crash frame with no flip and no extra offset (calls draw_crash defaults)
 *
 * \param[in] state        Pointer to game state.
 * \param[in] Aframe_index Sound effect index.
 */
static void draw_crash_unflipped(chqstate_t *state, int Aframe_index)
{
  draw_crash(state, Aframe_index, 0, 0); /* was FALLTHROUGH */
}

/**
 * $B69E: Draw crash
 *
 * \param[in] state           Pointer to game state.
 * \param[in] Aframe_index    Sound effect index.
 * \param[in] Bdash_flip_flag Bdash flip flag.
 * \param[in] Cdash           Cdash.
 */
static void draw_crash(chqstate_t *state,
                       int          Aframe_index,
                       int          Bdash_flip_flag,
                       int          Cdash)
{
  u8                    x;             /* was E */
  const carframe_t     *frame;         /* was HL */
  u8                    y;             /* was D */
  const caradornment_t *adornment;     /* was HL */
  u8                    height;        /* was B */
  u8                    width;         /* was C */
  const u8             *bitmap;        /* was HL */
  u8                    bitmap_stride; /* was E */
  int                   pitch;         /* was A */

  x = 128;
  frame = &car_frames[Aframe_index];
  y = frame->y + 121; /* vertical */
  x += frame->x; /* horizontal */
  adornment = &car_adornments[frame->index / 4];
  height = adornment->height;
  width  = adornment->width;
  bitmap = adornment->bitmap;
  // EXX
  bitmap_stride = width;
  // EXX

  y -= state->dhc_jump_y;

  pitch = state->dhc_pitch >> 1; // 0/3/6 -> 0/1/3
  if (pitch)
    // Otherwise pitch was 3/6
    y += pitch - 2; // make v.shift -1/1

  draw_part(state, height, width, y, x, bitmap, Bdash_flip_flag, Cdash,
            bitmap_stride); /* was FALLTHROUGH */
}

/**
 * $B6D6: Reduces y by car_y then falls through to draw_part_entry2
 *
 * \param[in] state               Pointer to game state.
 * \param[in] height              Number of rows to draw. (was B)
 * \param[in] width               Bitmap byte width. (was C)
 * \param[in] y                   Y position. (was D)
 * \param[in] x                   X position. (was E)
 * \param[in] bitmap              Source bitmap data. (was HL)
 * \param[in] Bdash_flip_flag     Non-zero to draw flipped horizontally. (was B')
 * \param[in] Cdash               Horizontal start offset. (was C')
 * \param[in] Edash_bitmap_stride Stride of bitmap data, in bytes. (was E')
 */
static void draw_part(chqstate_t *state,
                      int          height,
                      int          width,
                      int          y,
                      int          x,
                      const u8   *bitmap,
                      int          Bdash_flip_flag,
                      int          Cdash,
                      int          Edash_bitmap_stride)
{
  draw_part_entry2(state,
                   height,
                   width,
                   y - state->car_y,
                   x,
                   bitmap,
                   Bdash_flip_flag,
                   Cdash,
                   Edash_bitmap_stride); /* was FALLTHROUGH */
}

/**
 * $B6DD: Second entry point into draw_part
 *
 * \param[in] state               Pointer to game state.
 * \param[in] Bheight             Number of rows to draw. (was B)
 * \param[in] Cwidth_bytes        Bitmap byte width. (was C)
 * \param[in] Dy                  Y position. (was D)
 * \param[in] Ex                  X position. (was E)
 * \param[in] HLbitmap_data       Source bitmap data. (was HL)
 * \param[in] Bdash_flip_flag     Non-zero to draw flipped horizontally. (was B')
 * \param[in] Cdash               Horizontal start offset. (was C')
 * \param[in] Edash_bitmap_stride Stride of bitmap data, in bytes. (was E')
 */
static void draw_part_entry2(chqstate_t *state,
                             int          Bheight,
                             int          Cwidth_bytes,
                             int          Dy,
                             int          Ex,
                             const u8   *HLbitmap_data,
                             int          Bdash_flip_flag,
                             int          Cdash,
                             int          Edash_bitmap_stride)
{
  int carry_flip_flag; /* was carry */
  u8  Ay;              /* was A */
  u16 DEbackbuf;       /* was DE */
  u8  Estride;         /* was E */
  u16 HLdash_backbuf;  /* was HL' */
  u8  Awidth_bytes;    /* was A */

  // The buffer has the format 0b1111LLLLRRRCCCCC (L = scanline, R = row (group))

  DEbackbuf = (Ex & 0xF8) >> 3; // x pixel pos to field CCCCC
  Ay = Dy; // we make a temp copy but then bank - odd
  // EX AF,AF' - bank to use A as temp, or to preserve something like carry?
  DEbackbuf |= ((Dy & 0x0F) << 8) |
               0xF000; // y pixel pos (bottom nibble) to field LLLL
  // EX AF,AF'
  DEbackbuf |= (Ay & 0x70) << 1; // y pixel pos (remaining bits) to field RRR
  /* removed PUSH DEbackbuf */
  Estride = Cwidth_bytes << 1;
  // EXX - Bank
  HLdash_backbuf = DEbackbuf; /* was POP HLdash_backbuf */
  Awidth_bytes = Edash_bitmap_stride;
  carry_flip_flag = Bdash_flip_flag & 1; /* was shift (and zeroes the register) */
  // EX AF,AF'  -- preserve carry_flip_flag
  HLdash_backbuf += Cdash; /* was BCdash - B always zero here */
  // EX AF,AF'
  if (carry_flip_flag)
    plot_masked_sprite_flipped_entry2(state,
                                      Awidth_bytes,
                                      ADDRTOBACKBUF(HLdash_backbuf),
                                      Bheight,
                                      Estride,
                                      HLbitmap_data);
  else
    draw_part_plot_masked_sprite(state,
                                 Awidth_bytes,
                                 ADDRTOBACKBUF(HLdash_backbuf),
                                 Bheight,
                                 Estride,
                                 HLbitmap_data); /* was FALLTHROUGH */
}

/**
 * $B701: Draw part plot masked sprite
 *
 * \param[in] state               Pointer to game state.
 * \param[in] Awidth_bytes        Bitmap byte width.
 * \param[in] HLbackbuf_addr      Back buffer address.
 * \param[in] Bdash_height        Number of rows.
 * \param[in] Edash_bitmap_stride Source bitmap data.
 * \param[in] HLdash_bitmap_data  Source bitmap data.
 */
static void draw_part_plot_masked_sprite(chqstate_t *state,
    int          Awidth_bytes,
    u8         *HLbackbuf_addr,
    int          Bdash_height,
    int          Edash_bitmap_stride,
    const u8   *HLdash_bitmap_data)
{
  int IXjump_offset;

  IXjump_offset = (8 - Awidth_bytes) * 6;
  // B = 15; // Conv: Mask removed
  // EXX

  plot_masked_sprite(state,
                     IXjump_offset,
                     Bdash_height,
                     Edash_bitmap_stride, // Conv: Original clears top byte
                     HLdash_bitmap_data,
                     HLbackbuf_addr); /* was FALLTHROUGH */
}

/**
 * $B716: Plot a masked sprite
 *
 * This function draws the given masked bitmap to the back buffer.
 *
 * \param[in] state         Pointer to game state.
 * \param[in] jump_offset   Jump table byte offset (e.g. N * 6). (was IX)
 * \param[in] height        Number of rows. (was B)
 * \param[in] bitmap_stride Draw width of bitmap data, in bytes. (was DE)
 * \param[in] bitmap_data   Source bitmap data. (was HL)
 * \param[in] backbuf_addr  Back buffer address to draw at. (was HL')
 */
static void plot_masked_sprite(chqstate_t *state,
                               int         jump_offset,
                               int          height,
                               int         bitmap_stride,
                               const u8   *bitmap_data,
                               u8         *backbuf_addr)
{
  const u8 *src;          /* was SP */
  u8       *backbuf_orig; /* was C */
  u8        mask;         /* was E */
  u8        data;         /* was D */

  assert(jump_offset / 6 >= 0);
  assert(jump_offset / 6 <= 7);
  assert(VALID_BACKBUF_PTR(backbuf_addr));

  goto plot_masked_sprite_entry;

  for (;;) {
    // EXX - Unbank
    if (--height == 0)
      return;

    bitmap_data += bitmap_stride; // Advance to start of next row

    // Conv: This entry point pms_entry is used directly. In this C version it's
    // just the same as calling the function.
plot_masked_sprite_entry:
    src = bitmap_data;
    // EXX - Bank
    backbuf_orig = backbuf_addr; // Preserve start address
    switch (jump_offset / 6) {
    default: assert(0);
    case 0:
      // Conv: Original uses POP that loads 16 bits at a time
      mask = *src++, data = *src++, *backbuf_addr = (*backbuf_addr & mask) | data,
      backbuf_addr++;
    case 1:
      mask = *src++, data = *src++, *backbuf_addr = (*backbuf_addr & mask) | data,
      backbuf_addr++;
    case 2:
      mask = *src++, data = *src++, *backbuf_addr = (*backbuf_addr & mask) | data,
      backbuf_addr++;
    case 3:
      mask = *src++, data = *src++, *backbuf_addr = (*backbuf_addr & mask) | data,
      backbuf_addr++;
    case 4:
      mask = *src++, data = *src++, *backbuf_addr = (*backbuf_addr & mask) | data,
      backbuf_addr++;
    case 5:
      mask = *src++, data = *src++, *backbuf_addr = (*backbuf_addr & mask) | data,
      backbuf_addr++;
    case 6:
      mask = *src++, data = *src++, *backbuf_addr = (*backbuf_addr & mask) | data,
      backbuf_addr++;
    case 7:
      mask = *src++, data = *src++, *backbuf_addr = (*backbuf_addr & mask) | data,
      backbuf_addr++;
    }
    backbuf_addr = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(
        backbuf_orig))); // Restore row start address
  }
}

/**
 * $B76C: Plot a flipped and masked sprite
 *
 * This function draws the given masked bitmap to the back buffer while flipping
 * it horizontally.
 *
 * \param[in] state         Pointer to game state.
 * \param[in] width_bytes   Draw width of bitmap data, in bytes. (was A)
 * \param[in] backbuf_addr  Back buffer address to draw at. (was HL)
 * \param[in] height        Number of rows. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was E')
 * \param[in] bitmap_data   Source bitmap data. (was HL')
 */
static void plot_masked_sprite_flipped(chqstate_t *state,
                                       int          width_bytes,
                                       u8         *backbuf_addr,
                                       int          height,
                                       int         bitmap_stride,
                                       const u8   *bitmap_data)
{
  plot_masked_sprite_flipped_entry2(state,
                                    width_bytes,
                                    backbuf_addr + width_bytes, // moving dst ptr to end
                                    height,
                                    bitmap_stride,
                                    bitmap_data); /* was FALLTHROUGH */
}

/**
 * $B770: Plot a flipped and masked sprite (2nd entry point)
 *
 * \param[in] state         Pointer to game state.
 * \param[in] width_bytes   Draw width of bitmap data, in bytes. (was A)
 * \param[in] backbuf_addr  Back buffer address to draw at. (was HL)
 * \param[in] height        Number of rows. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was E')
 * \param[in] bitmap_data   Source bitmap data. (was HL')
 */
static void plot_masked_sprite_flipped_entry2(chqstate_t *state,
    int          width_bytes,
    u8         *backbuf_addr,
    int          height,
    int         bitmap_stride,
    const u8   *bitmap_data)
{
  int       jump_offset;   /* was IX */
  const u8 *src;           /* was SP */
  u8       *backbuf_orig;  /* was A'?? */
  u8        mask;          /* was C */
  u8        data;          /* was B */

  assert(VALID_BACKBUF_PTR(backbuf_addr));

  // EX DE,HL  -- move backbuffer ptr to DE?
  jump_offset = 8 - width_bytes; // Conv: Multiplication removed

  // HL = $EFxx  -- set reversing table addr top byte
  // EXX - Bank
  // Ddash = 0; // clearing hi byte of DE'? not sure why
  goto pmsf_start;

  for (;;) {
    // EX AF,AF' - Unbank
    // EXX - Bank
    if (--height == 0)
      return;

    bitmap_data += bitmap_stride; // Advance to start of next row

pmsf_start:
    src = bitmap_data;
    // EXX - Unbank
    backbuf_orig = backbuf_addr; // Preserve start address
    // EX AF,AF' - Bank
    switch (jump_offset) {
    default: assert(0);
    // Conv: Original uses POP that loads 16 bits at a time
    case 0: mask = *src++; data = *src++;
      *backbuf_addr = (*backbuf_addr & state->flipped[mask]) | state->flipped[data];
      backbuf_addr--;
    case 1: mask = *src++; data = *src++;
      *backbuf_addr = (*backbuf_addr & state->flipped[mask]) | state->flipped[data];
      backbuf_addr--;
    case 2: mask = *src++; data = *src++;
      *backbuf_addr = (*backbuf_addr & state->flipped[mask]) | state->flipped[data];
      backbuf_addr--;
    case 3: mask = *src++; data = *src++;
      *backbuf_addr = (*backbuf_addr & state->flipped[mask]) | state->flipped[data];
      backbuf_addr--;
    case 4: mask = *src++; data = *src++;
      *backbuf_addr = (*backbuf_addr & state->flipped[mask]) | state->flipped[data];
      backbuf_addr--;
    case 5: mask = *src++; data = *src++;
      *backbuf_addr = (*backbuf_addr & state->flipped[mask]) | state->flipped[data];
      backbuf_addr--;
    case 6: mask = *src++; data = *src++;
      *backbuf_addr = (*backbuf_addr & state->flipped[mask]) | state->flipped[data];
      backbuf_addr--;
    case 7: mask = *src++; data = *src++;
      *backbuf_addr = (*backbuf_addr & state->flipped[mask]) | state->flipped[data];
      backbuf_addr--;
    }
    backbuf_addr = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(
        backbuf_orig))); // Restore row start address
  }
}

/**
 * $B7EF: likely an inverted variation
 *
 * hit when barriers are flipped over.
 * sampled.
 * A' = 0, BC' = $0704  B'=height, DE' = $0004, HL' = $6EB5  bitmap_barrier_4s (which is 2bytes x 2 x 7).
 *
 * \param[in] state               Pointer to game state.
 * \param[in] Awidth_bytes        Bitmap byte width.
 * \param[in] HLbackbuf_addr      Back buffer address.
 * \param[in] Bdash_height        Number of rows.
 * \param[in] Edash_bitmap_stride Source bitmap data.
 * \param[in] HLdash_bitmap_data  Source bitmap data.
 */
static void plot_masked_sprite_inverted(chqstate_t *state,
                                        int          Awidth_bytes,
                                        u8         *HLbackbuf_addr,
                                        int          Bdash_height, // what's in C'?
                                        int         Edash_bitmap_stride,
                                        const u8   *HLdash_bitmap_data)
{
  int       jump_offset;              /* was IX */
  u16       DEdash_bitmap_stride;     /* was DE */
  const u8 *HLdash_bitmap_data_final; /* was HL' */
  u8        Bheight;                  /* was B */

  // Conv: Setting SP restore removed
  jump_offset = (8 - Awidth_bytes) * 6; // jump table index * entry size
  // Conv: Removed setting B to 15 for line stepping
  // EXX - Bank
  DEdash_bitmap_stride = Edash_bitmap_stride;
  HLdash_bitmap_data_final = (Bdash_height - 1) * DEdash_bitmap_stride + HLdash_bitmap_data;
  Bheight = Bdash_height;

  DEdash_bitmap_stride = -DEdash_bitmap_stride;

  // spot that the EXX is unpaired so we end up using the 'other' bank here
  plot_masked_sprite(state,
                     jump_offset,
                     Bheight,
                     DEdash_bitmap_stride,
                     HLdash_bitmap_data_final,
                     HLbackbuf_addr); /* was exit via pms_entry */
}

/**
 * $B848: Scroll horizon
 *
 * \param[in] state Pointer to game state.
 */
static void scroll_horizon(chqstate_t *state)
{
  int        carry = 0;
  int        speed;                     /* was HL */
  int        current_curvature;         /* was A */
  u8         Adash;                     /* was A' */
  const u8  *HLhorizon_table;           /* was HL */
  u8         BChorizon_table_value;     /* was BC */
  int        Aregular;                  /* was A */
  int        Aincline;                  /* was A */
  int        Adiff;                     /* was A */
  int        Bcounter;                  /* was C */
  int        Eset_if_incline_negative;  /* was C */
  int        Chorizon_table_value;      /* was C */
  int        Ahorizon_y_a25a_delta;     /* was A */
  int        BCcounter;                 /* was BC */

  if ((speed = state->speed) == 0)
    return;

  // Horizontal scrolling
  //

  if ((current_curvature = state->current_curvature) != 0) {
    // A' is used here uninitialised. The original game may be relying on it
    // being last set in move_hero_car at $B296. Or it might be a bug.
    //

    // EX AF,AF'  (suspect this should be LD A,L)
    carry = (speed >> 8) & 1;
    RL(Adash);
    RL(Adash);
    RL(Adash);
    Adash &= 6; // get top two bits of speed, scaled up by 2
    BChorizon_table_value = horizon_table[(state->horizon_curve_index + Adash) /
                                          2]; // CHECK scaling / offset
    BChorizon_table_value = (BChorizon_table_value >> 8) | (BChorizon_table_value <<
                            8); // Conv: Swap

    // Decrement horizon_a25e
    if (--state->horizon_x_scroll == 0) {
      state->horizon_x_scroll = BChorizon_table_value >> 8;
      // EX AF,AF' - bank so we can keep Aregular
      Aregular = BChorizon_table_value & 0xFF;
      if ((s8) Aregular < 0)
        Aregular = -Aregular;

      Aregular += state->dr_horizon_x_scroll; // 0..19
      if ((s8) Aregular < 0)
        Aregular += 20;
      if (Aregular >= 20)
        Aregular -= 20;
      state->dr_horizon_x_scroll = Aregular;
    }
  }

  Ahorizon_y_a25a_delta = 0;
  Bcounter = 0;
  Eset_if_incline_negative = 0;

  // Vertical scrolling
  //

  // EX AF,AF' - bank zeroed Aregular, unbank Aincline from earlier
  if ((Aincline = state->incline) == 0)
    return; // flat road

  if (Aincline < 0) {
    Eset_if_incline_negative = 1; /* was INC E */
    Aincline = -Aincline;
  }

  HLhorizon_table = &horizon_table[(-1 + Aincline) /
                                   2]; // CHECK: Scaling / offset
  Adiff = state->fast_counter - state->horizon_y_step;
  if (Adiff)
    return;

  Chorizon_table_value = *HLhorizon_table;
  for (;;) {
    carry = Adiff < Chorizon_table_value; // added
    Adiff -= Chorizon_table_value;
    if (carry)
      break;
    Bcounter++;
    // EX AF,AF'
    Ahorizon_y_a25a_delta += Chorizon_table_value;
    // EX AF,AF'
  }
  if (Bcounter == 0)
    return;

  state->horizon_y_accum += Ahorizon_y_a25a_delta;

  // Sign extend based on low bit of Eset_if_incline_negative
  BCcounter = (Eset_if_incline_negative) ? -Bcounter : Bcounter;

  // Adjust horizon_level
  state->session.horizon_level += BCcounter;
  // EX AF,AF'
  state->horizon_y_step += Bcounter;
}

/**
 * $B8D2: Update road level
 *
 * \param[in] state Pointer to game state.
 */
static void update_road_level(chqstate_t *state)
{
  int       carry = 0;
  int       Bvar_a25a;          /* was B */
  int       Cnegate_flag;       /* was C */
  int       Aincline;           /* was A */
  const u8 *HLroadbuf;          /* was HL */
  int       Aheight;            /* was A */
  int       Cheight;            /* was C */
  int       Bpitch;             /* was B */
  u8       *HLprev_road_height;         /* was HL */
  int       Aprev_road_height;          /* was A */
  int       Bprev_road_height;          /* was B */
  int       Ay_offset;          /* was A */
  int       Adiff;              /* was A */
  const u8 *HLptable_b059;      /* was HL */
  int       Eoffset;            /* was E */
  int       Acurrent_curvature; /* was A */
  int       Afork_visible;      /* was A */
  int       Acurvature_byte;    /* was A */
  int       Afork_taken;        /* was A */
  int       Bcurvature_byte;    /* was B */
  int       Ax_scroll;          /* was A */
  int       Bcurvature_ticks;   /* was B */
  int       C;                  /* was C */
  int       A;                  /* was A */
  int       B;                  /* was B */

  Bvar_a25a = state->horizon_y_accum; // load and widen
  Cnegate_flag = 0;
  Aincline = state->incline;
  if (Aincline < 0) { // if road climbing
    Aincline = -Aincline;
    Cnegate_flag = 1; /* was INC C */
  }

  Aincline -= Bvar_a25a;
  if (Aincline)
    state->session.horizon_level += (Cnegate_flag) ? -Aincline : Aincline;

  HLroadbuf = ROADBUF_FWD2PTR(ROADBUF_HEIGHT_OFFSET + 2);

  state->horizon_y_step = state->horizon_y_accum = 0;

  Aheight = (s8) * HLroadbuf >> 1;
  if (Aheight < 0)
    Aheight++;
  state->incline = Aheight;

  WRAPPING(HLroadbuf, -2, state->road_buffer_start);
  Cheight = Aheight = (s8) * HLroadbuf;
  // OR A
  Bpitch = 0;
  if (Aheight) {
    Bpitch = 6;
    if (Aheight < 0) {
      Aheight = -Aheight;
      Bpitch = 3;
    }
    if (Aheight < 3)
      Bpitch = 0;
  }
  state->dhc_pitch = Bpitch;

  HLprev_road_height = &state->prev_road_height;
  Aprev_road_height = *HLprev_road_height;
  if ((s8) Aprev_road_height < 0) { // could combine exprs
    if ((Cheight & (1 << 7)) == 0) { // ie. positive
      Aprev_road_height = -Aprev_road_height;
      carry = Aprev_road_height < 2, Aprev_road_height -= 2;
      if (!carry) {
        Bprev_road_height = Aprev_road_height;
        Ay_offset = state->mhc_y_offset;
        if (Ay_offset) {
          Adiff = Bprev_road_height - (3 - ((state->speed >> 7) &
                                            3)); // result = 1..5? // folded a lot here
          if ((s8) Adiff > 0) { /* was !C && !Z */
            // PUSH HLprev_road_height
            HLptable_b059 = &car_jump_resume_params[(Adiff * 2) -
                                                    1]; // use of DE removed, RLC folded in
            Eoffset = *HLptable_b059++; // an offset
            state->mhc_y_offset = *HLptable_b059;
            state->mhc_jump_data = &hero_car_jump_table[Eoffset];
            // POP HLprev_road_height
          }
        }
      }
    }
  }
  *HLprev_road_height = Cheight;

  Acurrent_curvature = state->current_curvature;
  // EX AF,AF' - bank Acurrent_curvature
  HLroadbuf = ROADBUF_FWD2PTR(ROADBUF_CURVATURE_OFFSET);
  Afork_visible = state->fork_visible;
  Acurvature_byte = *HLroadbuf; // load a curvature byte
  if (Afork_visible && state->fork_in_progress) {
    Afork_taken = state->fork_taken;
    Acurvature_byte = *HLroadbuf;
    if (Afork_taken)
      Acurvature_byte = -Acurvature_byte;
  }

  state->current_curvature = Acurvature_byte;
  // Ecurvature_byte = Acurvature_byte; // removed presumed unused
  if (Acurvature_byte) {
    // Eone_or_two = 1; // removed presumed unused
    if ((s8) Acurvature_byte < 0) {
      // Eone_or_two = 2; /* was RL(E) // removed presumed unused */
      Acurvature_byte = -Acurvature_byte;
    }
    // Dcurvature_byte = Acurvature_byte; // removed presumed unused
    Acurvature_byte <<= 2;
    state->horizon_curve_index = Acurvature_byte;
    Bcurvature_byte = Acurvature_byte;
    if (state->horizon_x_scroll)
      goto url_B9C5;

    Ax_scroll = horizon_table[((state->speed >> 6) & 6) +
                              (Bcurvature_byte & 0xFF)]; // use of BC removed
  } else {
    Ax_scroll = Acurvature_byte; // Conv: added
  }
  state->horizon_x_scroll = Ax_scroll;

url_B9C5:
  Bcurvature_ticks = state->curvature_ticks;
  C = 0;
  // EX AF,AF' - unbank Acurrent_curvature
  if ((s8) Acurrent_curvature < 0) {
    Acurrent_curvature = -Acurrent_curvature;
    C++;
  }

  A = Acurrent_curvature - Bcurvature_ticks;
  if ((s8) A > 0) {
    A = (A << 2) + (A >> 1);
    B = 0;
    if (C & 1) { // invert BA
      B = 0xFF; /* was DEC B */
      A = -A;
    }
    state->horizontal_adjust = (B << 8) | A;
  }
  state->horizon_scroll_sub = state->curvature_ticks = 0;
}

/**
 * $B9F4: Layout road
 *
 * \param[in] state Pointer to game state.
 */
static void layout_road(chqstate_t *state)
{
  int       carry = 0;
  u8       *DElanedata_base;
  u8       *DElanedata;
  int       Biterations;
  int       Lcounter;
  u16      *SProadright;
  u8        Aiterations;
  int       Aforkinprogress;
  u16      *SMroadcentre;
  u16      *SMroadcentreright;
  u16      *SMroadcentreleft;
  u16      *SMroadleft;
  u16      *SMroadright;
  u16      *SMveryright;
  int       BCdash;
  int       DEdash;
  int       HLdash;
  int       DEroadpos;
  const u8 *HLchatterblk;
  int       Ca16d;
  int       HLforkdistance;
  int       DEforkdistance;
  int       HLroadpos;
  int       HLroadpos_saved;
  u8       *HLunknown;

  // point at lane data
  DElanedata_base = DElanedata = ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET);

  // Count the distance to the forked road.
  Biterations = 20;
  Lcounter = 0; // counter
  do {
    if ((*DElanedata & 0xE1) == 0xE1)
      goto lr_forked_road;

    WRAPPINGINCREMENT(DElanedata, DElanedata_base);
    Lcounter++;
  } while (--Biterations > 0);

  // No forked road found.
  build_curve_table(state, /*forked=*/0);
  // $E800 now contains the left edges and $EC00 contains the right edges.
  SProadright = &state->xpos_road_right[0x30];
  Aiterations = 0x30; // 48..256 in steps of 2 = 104 iterations
lr_calc_single_lane:
  do {
    SMroadcentre      = &state->xpos_road_centre[Aiterations];
    SMroadcentreright = &state->xpos_road_centre_right[Aiterations];
    SMroadcentreleft  = &state->xpos_road_centre_left[Aiterations];
    SMroadleft        = &state->xpos_road_left[Aiterations];

    // EXX Bank

    // Centre = Left + (Right - Left) / 2
    DEdash = *SMroadleft; // read from road left
    HLdash = *SProadright++; // POP from $ECxx
    HLdash = (HLdash - DEdash) >> 1; // halve total width
    BCdash = HLdash; // stash halved width
    HLdash += DEdash; // calc centre from left
    *SMroadcentre = HLdash; // store centre pos

    DEdash = BCdash;
    BCdash >>= 1; // halve again for quarter width
    HLdash += BCdash; // calc centre+quarter width
    *SMroadcentreright = HLdash; // store centre-right pos

    HLdash -= DEdash; // calc centre-halved width
    *SMroadcentreleft = HLdash; // store centre-left pos

    Aiterations += 2;

    // EXX Unbank
  } while (Aiterations != 0);
  return;

lr_forked_road:
  state->fork_countdown = Lcounter;
  HLunknown = &state->height_table[Lcounter];
  Aiterations = 96;
  state->fork_visible = Aiterations; // just a flag AFACIT
  Aiterations = 106 - (Aiterations - *HLunknown);
  // PUSH AF  // preserve Aiterations
  HLforkdistance = state->fork_distance;
  if ((*DElanedata & 4) !=
      0) // check for forked road (have already checked flags for 0xE1)
    goto lr_badf;
  // is this forked or unforked or ...?
  Aforkinprogress = state->fork_in_progress - 1;
  if (Aforkinprogress == 0)
    goto lr_check_spawning; // hit fork?
  state->fork_in_progress = -Aforkinprogress; // why negate, is this a counter?
  DEroadpos = state->scenedata.road_pos;
  Aiterations = 1;
  DEroadpos -= 256; /* was DEC D */
  // Chooses the fork taken based on car's distance from centre
  if ((DEroadpos >> 8) < 128) { // possibly redundant check
    if (DEroadpos < 12) { // checking full word - car close to centre?
      Aiterations = 1;
    } else {
      Aiterations--; // must be doing 1 -> 0
    }
  }

  //lr_check_correct_fork_taken:
  state->fork_taken = Aiterations;
  Aiterations++; // 0/1 -> 1/2
  // PUSH HLforkdistance (ok)
  if (Aiterations == state->correct_fork) {
    // Correct fork taken
    HLchatterblk = &chatterblk_tony_lets_go[0];
  } else {
    // Incorrect fork taken
    state->hazards[0].speed = 95; // boost perp speed from normal 60 (writes $A195)
    // Q. Why is a bonus awarded for going the wrong way?
    add_bonus(state, 0, 0, state->wanted_stage_number + 4);
    HLchatterblk = &chatterblk_raymond_wrong_way[0];
  }
  start_chatter(state, 20, HLchatterblk);
  // POP HLforkdistance (ok)

lr_check_spawning:
  Aiterations = state->allow_spawning;
  if (Aiterations == 0)
    goto lr_no_car_spawning;
  Aiterations += state->session.spawn_accumulator;
  Ca16d = Aiterations; // new value for $A16D
  Aiterations -= 2;
  if (Aiterations >= 256 - 2) // carried?
    goto lr_set_var_a16d_from_c;
  Ca16d = Aiterations; // new value for $A16D
  HLforkdistance += 16;
  state->fork_distance = HLforkdistance;
lr_set_var_a16d_from_c:
  state->session.spawn_accumulator = Ca16d;
lr_no_car_spawning:
  carry = state->session.spawn_accumulator & 1; // CHECK
  Aiterations = state->fast_counter;
  RL(Aiterations);
  RL(Aiterations);
  RL(Aiterations);
  RL(Aiterations);
  Aiterations -= 0x10; // sets top nibble to $F
  HLforkdistance += 0xFF |
                    Aiterations; // a signed -15..16 value now IS THIS INCREMENT WRONG?
lr_badf:
  // PUSH HLforkdistance
  if (state->fork_taken - 1 != 0) {
    build_curve_table(state, /*forked=*/0);
    DEforkdistance = HLforkdistance; /* was POP DEforkdistance */
    HLroadpos = state->scenedata.road_pos;
    HLroadpos_saved = HLroadpos; /* was PUSH HLroadpos */
    HLroadpos += DEforkdistance;
    state->scenedata.road_pos = HLroadpos; // adjust road pos for fork rendering
    build_curve_table(state, /*forked=*/1);
  } else {
    build_curve_table(state, /*forked=*/1);
    DEforkdistance = HLforkdistance; /* was POP DEforkdistance */
    HLroadpos = state->scenedata.road_pos;
    HLroadpos_saved = HLroadpos; /* was PUSH HLroadpos */
    HLroadpos -= DEforkdistance;
    state->scenedata.road_pos = HLroadpos; // adjust road pos for fork rendering
    build_curve_table(state, /*forked=*/0);
  }
  // $BB07
  HLroadpos = HLroadpos_saved; /* was POP HLroadpos */
  state->scenedata.road_pos =
    HLroadpos; // restore normal road pos after fork rendering
  // POP BC
  // (set SP restoring op)
  SProadright = &state->xpos_road_right[0x30]; // (set SP to $EC30)
  Aiterations = 0x30; // 48..256 in steps of 2 = 104 iterations
  do {
    SMroadcentre      = &state->xpos_road_centre[Aiterations];
    SMroadcentreleft  = &state->xpos_road_centre_left[Aiterations];
    SMroadleft        = &state->xpos_road_left[Aiterations];
    SMveryright       = &state->xpos_road_fork_right[Aiterations]; // output right?
    SMroadcentreright = &state->xpos_road_centre_right[Aiterations]; // output left?
    SMroadright       = &state->xpos_road_right[Aiterations];
    // EXX Bank for inner loop
    DEdash = *SMroadleft;
    HLdash = *SProadright++; // POP HLdash // read from $ECxx
    *SMroadcentre      = (HLdash + DEdash) / 2; // (right+left)/2 = new road centre
    *SMroadcentreleft  = (HLdash + DEdash) / 2; // new road centre left

    DEdash = *SMveryright;
    HLdash = *SMroadcentreright;
    *SMroadcentreright = (HLdash + DEdash) / 2;
    *SMroadright       = (HLdash + DEdash) / 2;

    Aiterations += 2;
    // EXX Unbank
  } while (--Aiterations > 0);
  SProadright = &state->xpos_road_right[Aiterations];
  goto lr_calc_single_lane; // jump into no_fork code
}

/**
 * $BB69: Exit fork
 *
 * \param[in] state Pointer to game state.
 */
static void exit_fork(chqstate_t *state)
{
  int curve_type; /* was D */
  int lanes_type; /* was E */
  int obj_offset; /* was C */
  int i;          /* was B */

  /* $BB69: return if fork_distance high byte is zero */
  if ((state->fork_distance & 0xFF00) == 0)
    return;

  if (state->fork_taken == 1) {
    /* $BBA1: ef_right */
    curve_type = 0x04;
    lanes_type = 0x03;
    state->scenedata.road_leftside_ptr  = &forked_road_exit_rightobjs[-1];
    state->scenedata.road_rightside_ptr = &forked_road_exit_leftobjs[-1];
    state->scenedata.road_lanes_ptr     = &forked_road_exit_right_lanes[-1];
    state->rm_SM_BE6D_curvature_one_command_ptr = lookup_map_goto(state,
      state->rm_SM_BBC2_rightfork_curve);
    state->rm_SM_BEBF_height_one_command_ptr    = lookup_map_goto(state,
      state->rm_SM_BBC5_rightfork_height);
    state->rm_SM_BF0A_lanes_one_command_ptr     = lookup_map_goto(state,
      state->rm_SM_BBC8_rightfork_lanes);
    state->rm_SM_C046_hazards_one_command_ptr   = lookup_map_goto(state,
      state->rm_SM_BBB8_rightfork_hazards);
    state->rm_SM_BF84_rightside_one_command_ptr = lookup_map_goto(state,
      state->rm_SM_BBBB_rightfork_rightside);
    state->rm_SM_BFCD_leftside_one_command_ptr  = lookup_map_goto(state,
      state->rm_SM_BBBE_rightfork_leftside);
    obj_offset =
      32; /* rightobjs: ROADBUF_LANES_OFFSET + 32 = ROADBUF_RIGHTOBJS_OFFSET */
  } else {
    /* $BB74: ef_left */
    curve_type = 0xFC;
    lanes_type = 0x01;
    state->scenedata.road_leftside_ptr  = &forked_road_exit_leftobjs[-1];
    state->scenedata.road_rightside_ptr = &forked_road_exit_rightobjs[-1];
    state->scenedata.road_lanes_ptr     = &forked_road_exit_left_lanes[-1];
    state->rm_SM_BE6D_curvature_one_command_ptr = lookup_map_goto(state,
      state->rm_SM_BB95_leftfork_curve);
    state->rm_SM_BEBF_height_one_command_ptr    = lookup_map_goto(state,
      state->rm_SM_BB98_leftfork_height);
    state->rm_SM_BF0A_lanes_one_command_ptr     = lookup_map_goto(state,
      state->rm_SM_BB9B_leftfork_lanes);
    state->rm_SM_C046_hazards_one_command_ptr   = lookup_map_goto(state,
      state->rm_SM_BB8B_leftfork_hazards);
    state->rm_SM_BF84_rightside_one_command_ptr = lookup_map_goto(state,
      state->rm_SM_BB8E_leftfork_rightside);
    state->rm_SM_BFCD_leftside_one_command_ptr  = lookup_map_goto(state,
      state->rm_SM_BB91_leftfork_leftside);
    obj_offset =
      64; /* leftobjs: ROADBUF_LANES_OFFSET + 64 = ROADBUF_LEFTOBJS_OFFSET */
  }

  /* $BBE3: common exit-fork road pointers */
  state->scenedata.road_curvature_ptr = &forked_road_exit_curvature[-1];
  state->scenedata.road_height_ptr    = &forked_road_exit_height[-1];
  state->scenedata.road_hazard_ptr    = &forked_road_exit_hazards[-1];

  /* $BBFD: fill 32 curvature bytes, 32 lanes bytes, 32 object bytes (zeroed) */
  for (i = 0; i < 32; i++)
    *ROADBUF_FWD2PTR(ROADBUF_CURVATURE_OFFSET + i) = curve_type;
  for (i = 0; i < 32; i++)
    *ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET + i) = lanes_type;
  for (i = 0; i < 32; i++)
    *ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET + obj_offset + i) = 0;

  /* $BC15: reset per-frame road state */
  state->curvature_byte         = 0;
  state->height_byte            = 0;
  state->leftside_byte          = 0;
  state->rightside_byte         = 0;
  state->hazards_byte           = 0;
  state->lanes_counter_byte     = 0;
  state->fork_in_progress       = 0;
  state->fork_taken             = 0;
  state->fork_visible           = 0;
  state->session.no_objects_counter  = 1;
  state->session.spawn_accumulator            = 1;
  state->fork_distance          = 0;
}

// The screen has the format 0b010BBLLLRRRCCCCC (B = band, L = scanline, R = row (group), C = column)
// The buffer has the format 0b1111LLLLRRRCCCCC (L = scanline, R = row (group))

/**
 * $BC3E: Copy the backbuf to the real screen
 *
 * \param[in] state Pointer to game state.
 */
static void update_screen(chqstate_t *state)
{
  u8        *scr;       /* was HL */
  u8        *buf;       /* was HL' */
  u16        bufoffset; // Conv: added
  ptrdiff_t  scroff;    // Conv: for safe bounds checking

  scr = ADDRTOSCREEN(0x4811); // (136, 64)
  buf = ADDRTOBACKBUF(0xF001); // (8, 1)

  for (;;) {
    // First do left hand side (original reads forwards, stores backwards)
    do {
      memcpy(scr - 16, buf, 16); scr += 256; buf += 256;
      memcpy(scr - 16, buf, 16); scr += 256; buf += 256;
      memcpy(scr - 16, buf, 16); scr += 256; buf += 256;
      memcpy(scr - 16, buf, 16); scr += 256; buf += 256;
      bufoffset = BACKBUFTOOFFSET(buf); // Conv: convert back to offset
      // Loop on the first pass (4 lines of 8 done) but not the second
    } while (bufoffset & (1 << 10));

    // Now move over to the right hand side
    // e.g. (0xF001 + 8*256 - 0x7F0) == 0xF011 on the first pass
    buf = OFFSETTOBACKBUF(bufoffset - 0x07F0);
    scroff = SCREENTOOFFSET(scr) - 0x07F2;
    scr = OFFSETTOSCREEN(scroff);
    do {
      memcpy(scr - 14, buf, 14); scr += 256; buf += 256;
      memcpy(scr - 14, buf, 14); scr += 256; buf += 256;
      memcpy(scr - 14, buf, 14); scr += 256; buf += 256;
      memcpy(scr - 14, buf, 14); scr += 256; buf += 256;
      bufoffset = BACKBUFTOOFFSET(buf); // Conv: convert back to offset
      // Loop on the first pass (4 lines of 8 done) but not the second
    } while (bufoffset & (1 << 10));

    if ((bufoffset & (1 << 11)) == 0) {
      u8  H;
      u8  A;
      u8  L;
      int res;
      int carry, overflow;

      /* buf has advanced past a 0x1000 boundary (bit 11 just cleared),
       * meaning another 16 backbuffer rows have been written and it is
       * time to advance to the next row-group.
       *
       * This is a Z80 "SUB H" on the low byte of bufoffset, using H set
       * to 0xF0 — the high byte of BACKBUFFER_START_ADDRESS.  Each time
       * the magic fires, A holds successive low bytes of the end-of-group
       * bufoffset: 0x11, 0x31, 0x51, 0x71, 0x91, 0xB1, 0xD1, 0xF1.
       *
       * carry    (unsigned A < H = 0xF0): set for the first seven groups
       *          (A = 0x11..0xD1); clear at A = 0xF1, meaning all eight
       *          groups of 16 rows (128 rows total) are done → break.
       *
       * overflow (signed 8-bit overflow of A - H): fires when A + 16
       *          exceeds +127, i.e. the first time A = 0x71.  That is
       *          exactly the 64-row midpoint of the backbuffer, which
       *          corresponds to the boundary between the ZX Spectrum
       *          screen's middle third (rows 64-127, starting at 0x4811)
       *          and its bottom third (rows 128-191, starting at 0x5011).
       *
       * L = (A - H) & 0xFF is the low byte of the next row-group's
       * backbuffer start address, so ADDRTOBACKBUF(0xF000 | L) resets
       * buf to the beginning of the next group.
       */
      H        = 0xF0;
      A        = bufoffset & 0xFF;
      res      = A - H;
      carry    = (A < H); // unsigned: set while rows remain
      overflow = ((H ^ A) & (res ^ A)) >> 7; // Z80 V-flag formula
      L        = res; // low byte of next group start

      if (!carry)
        break; // A >= 0xF0: all 128 rows written

      buf = ADDRTOBACKBUF((H << 8) | L); // reposition to next row-group

      if (!overflow) {
        scroff = SCREENTOOFFSET(scr) - 0x07EE;
        scr = OFFSETTOSCREEN(scroff);
      } else {
        // 64-row midpoint: jump to the ZX screen's bottom third
        scr = ADDRTOSCREEN(0x5011); // (136, 128)
      }
    } else {
      scroff = SCREENTOOFFSET(scr) - 0x07EE;
      scr = OFFSETTOSCREEN(scroff);
      buf -= 16;
    }
  }

  /* Set screen attributes */
  {
    u8   A;
    u8   E;
    u8   D;
    s16  DElevel;
    u8  *HLattrs;
    u16  BCattrs;
    u8   Cattr;

    // Don't update the attributes if the level intro screen is being shown
    if (state->dont_draw_screen_attrs)
      goto exit;

    A = state->horizon_attr[1];
    E = state->horizon_attr[2]; // current value?
    state->horizon_attr[2] = A;
    if (E != 0) { // if moved? some sort of previous/current behaviour here. is table holding deltas?
      E = (A << 2);
      D = (A >= 64) ? 0xFF : 0x00; /* was SBC A,A - must be sign extending */
      DElevel = (D << 8) | E;

      assert(state->session.horizon_attribute != 0);

      HLattrs = ADDRTOATTRS(state->session.horizon_attribute);

      // Set sky colour by default
      BCattrs = (attribute_BRIGHT_BLACK_OVER_CYAN << 8) |
                attribute_BRIGHT_BLACK_OVER_CYAN;
      // If A was zero then jump (Z => sky, NZ => ground)
      if (DElevel < 0) {
        // Set ground colour
        BCattrs = state->stage->ground_colour;
        HLattrs += DElevel;
      }

      // Fill 30 bytes - length of attribute line minus the two blank edges
      // Conv: We instead use memset() and only use bottom byte of BCattrs
      memset(HLattrs, BCattrs & 0xFF, 30);
      if (DElevel >= 0)
        HLattrs += DElevel;
      state->session.horizon_attribute = ATTRSTOADDR(HLattrs);
    }

    /* Set smash meter attributes */

    if (state->sighted_flag == 0
        || state->perp_caught_phase >= PERPCAUGHTPHASE_STOPPED)
      goto exit;

    HLattrs = ADDRTOATTRS(0x5962); // attr (2, 11)

    Cattr = attribute_BRIGHT_BLACK_OVER_RED;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    Cattr = attribute_BRIGHT_BLACK_OVER_MAGENTA;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    Cattr = attribute_BRIGHT_BLACK_OVER_GREEN;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    Cattr = attribute_BRIGHT_BLACK_OVER_WHITE;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    *HLattrs = Cattr;
  }

exit:
  /* Redraw the whole screen (TODO: just the playfield). */
  state->speccy->draw(state->speccy, NULL); // Conv: Added
}

/**
 * $BDC1: Clear playfield set attrs
 *
 * \param[in] state Pointer to game state.
 */
static void clear_playfield_set_attrs(chqstate_t *state)
{
  u8 *screen;     /* was HL */
  u16 stride;     /* was DE */
  int iterations; /* was B */

  clear_playfield(state);

  // Clear the playfield pixels to $FF (bug: duplicates work just done)
  memset(ADDRTOSCREEN(SCREEN_PLAYFIELD_BITMAP_ADDR), 0xFF,
         PLAYFIELD_HEIGHT * SCREEN_BITMAP_ROWBYTES);

  // Clear the playfield attributes to $28 (black over cyan) - first two
  // rows only
  memset(ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR), attribute_BLACK_OVER_CYAN,
         2 * SCREEN_ATTRIBUTES_ROWBYTES);

  // Clear the next three rows to $68 (bright, black over cyan)
  memset(ADDRTOATTRS(0x5940), attribute_BRIGHT_BLACK_OVER_CYAN,
         3 * SCREEN_ATTRIBUTES_ROWBYTES);

  // Clear the next 11 rows to the current ground colour
  // Note: Only using the bottom byte of ground_colour (as orig).
  memset(ADDRTOATTRS(0x59A0), state->stage->ground_colour,
         11 * SCREEN_ATTRIBUTES_ROWBYTES);

  // Clear the edges of the playfield to black on black
  screen = ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR);
  stride = SCREEN_BITMAP_ROWBYTES - 1;
  iterations = 16;
  do {
    *screen = attribute_BLACK_OVER_BLACK;
    screen += stride;
    *screen++ = attribute_BLACK_OVER_BLACK;
  } while (--iterations > 0);
}

/**
 * $BDFB: Read map
 *
 * \param[in] state Pointer to game state.
 */
static void read_map(chqstate_t *state)
{
  int carry = 0;
  u8 *pfast_counter;  /* was HL */
  int speed;          /* was DE */
  int speed_lo;       /* was A */
  int allow_spawning; /* was A */

  state->trigger_lane_change_sfx   = 0;
  state->trigger_passed_object_sfx = 0;
  state->allow_spawning            = 0;
  pfast_counter = &state->fast_counter;
  speed = state->speed;
  speed_lo = speed & 0xFF;
  if (speed > 255) {
    // Otherwise we're going fast. This seems to cause the buffer to be
    // processed twice as often as when in slow mode.
    rm_cycle_buffer_offset(state, pfast_counter);
  }

  carry = (speed_lo + *pfast_counter) > 255;
  speed_lo += *pfast_counter; // TODO Set carry
  *pfast_counter = speed_lo;
  allow_spawning = 0; // Set flag to disallow car spawning
  if (carry)
    rm_cycle_buffer_offset(state, pfast_counter); /* was fallthrough */

  state->allow_spawning += allow_spawning;
  check_hazard_collisions(state); /* exit via */
}

/**
 * $BE1F: Rm cycle buffer offset
 *
 * \param[in] state        Pointer to game state.
 * \param[in] pfastcounter Pfastcounter. (was HL)
 */
static void rm_cycle_buffer_offset(chqstate_t *state, u8 *pfastcounter)
{
  // TODO: Sort declarations
  u8        *HLlanesptr;  /* was HL */
  u8         Acurvebyte;  /* was A */
  const u8  *DEcurveptr;  /* was DE */
  const u8  *HLcurveptr;  /* was HL */
  u8        *DElanesptr;  /* was DE */
  u8         Alanes;      /* was A */
  int        Clanes;      /* was C */
  const u8  *DElaneptr;   /* was DE */
  u8         Arightside;  /* was A */
  const u8  *DErightptr;  /* was DE */
  u8         Aleftside;   /* was A */
  const u8  *DEleftptr;   /* was DE */
  const u8  *HLleftptr;   /* was DE */
  u8         Ahazards;    /* was A */
  const u8  *DEhazptr;    /* was DE */
  int        Covertake_bonus_counter;           /* was C */
  hazard_t  *IXhazard;    /* was IX */
  int        Aloop;       /* was A */
  int        Acurvature;  /* was A */
  u8         Aheightbyte; /* was A */
  const u8  *DEheightptr; /* was DE */
  u8        *HLheightptr; /* was HL */
  int        Biterations; /* was B */

  // Advance road_buffer_offset
  state->road_buffer_offset = ROADBUF_FWD2PTR(1);

  // Read sound effect triggers
  HLlanesptr = ROADBUF_FWD2PTR(95); // final byte of lanes slot
  state->trigger_passed_object_sfx |= *HLlanesptr;
  HLlanesptr = state->road_buffer_start + ROADBUF_PTR2IDX(HLlanesptr + 32);
  state->trigger_lane_change_sfx |= *HLlanesptr;

  // -- CURVATURE ($BE3A) --
  HLcurveptr = state->road_buffer_start + ROADBUF_PTR2IDX(HLlanesptr - 96);
  HLlanesptr = (u8 *)
               HLcurveptr; // Z80: L -= 96 mutates HL in place; keep HLlanesptr in sync

  // Format: $CD where [C]ounter; Curve [D]ata
  Acurvebyte = state->curvature_byte - 16;
  if (Acurvebyte < 240) // JR NC: no carry = counter not yet exhausted
    goto rm_save_curvature_byte;

  DEcurveptr = state->scenedata.road_curvature_ptr + 1;
  Acurvebyte = *DEcurveptr;
  if (Acurvebyte)
    goto rm_curvature_regular_byte;

  // Escape byte (0): read command byte.
  DElanesptr = (u8 *)HLcurveptr; HLcurveptr = DEcurveptr; // was EX DE,HL ($BE4E)
  Acurvebyte = *++HLcurveptr;
  HLcurveptr++;
  if (Acurvebyte == 0)
    goto rm_curvature_jump_command;
  if (Acurvebyte == 1)
    goto rm_curvature_one_command;

  // $BE58
  // Command byte = fork road (2)
  state->rm_SM_BB95_leftfork_curve  = wordat(HLcurveptr + 0);
  state->rm_SM_BBC2_rightfork_curve = wordat(HLcurveptr + 2);
  HLcurveptr = &forked_road_curvature[0];
  goto rm_read_curvature;

  // $BE6C
rm_curvature_one_command:
  HLcurveptr = state->rm_SM_BE6D_curvature_one_command_ptr;
  goto rm_read_curvature;

  // $BE71
rm_curvature_jump_command:
  HLcurveptr = lookup_map_goto(state, wordat(HLcurveptr));

  // $BE75
rm_read_curvature:
  DEcurveptr = HLcurveptr; HLlanesptr = DElanesptr; // was EX DE,HL ($BE75)
  Acurvebyte = *DEcurveptr;

  // $BE77
rm_curvature_regular_byte:
  state->scenedata.road_curvature_ptr = DEcurveptr;
  Acurvebyte -= 16;

  // $BE7D
rm_save_curvature_byte:
  state->curvature_byte = Acurvebyte;
  Acurvature = Acurvebyte & 0x0F;
  if (Acurvature & 8) {
    Acurvature &= 7;
    Acurvature = -Acurvature; // was NEG
  }

  // $BE8A
  Acurvature <<= 1;
  *HLlanesptr = Acurvature;

  // -- HEIGHT ($BE90) --
  HLheightptr = state->road_buffer_start + ROADBUF_PTR2IDX(HLlanesptr + 32);

  // Format: $CD where [C]ounter; Height [D]ata
  Aheightbyte = state->height_byte - 16;
  if (Aheightbyte < 240) // no carry = counter not yet exhausted
    goto rm_save_height_byte;

  DEheightptr = state->scenedata.road_height_ptr + 1;
  Aheightbyte = *DEheightptr;
  if (Aheightbyte)
    goto rm_height_regular_byte;

  // Escape byte (0): read command byte.
  { u8 *_tmp = HLheightptr; HLheightptr = (u8 *)DEheightptr; DEheightptr = _tmp; } // was EX DE,HL ($BEA0)
  Aheightbyte = *++HLheightptr;
  HLheightptr++;
  if (Aheightbyte == 0)
    goto rm_height_jump_command;
  if (Aheightbyte == 1)
    goto rm_height_one_command;

  // $BEAA
  // Command byte = fork road (2)
  state->rm_SM_BB98_leftfork_height  = wordat(HLheightptr + 0);
  state->rm_SM_BBC5_rightfork_height = wordat(HLheightptr + 2);
  HLheightptr = &forked_road_height[0];
  goto rm_read_height;

  // $BEBE
rm_height_one_command:
  HLheightptr = state->rm_SM_BEBF_height_one_command_ptr;
  goto rm_read_height;

  // $BEC3
rm_height_jump_command:
  HLheightptr = lookup_map_goto(state, wordat(HLheightptr));

  // $BEC7
rm_read_height:
  { u8 *_tmp = (u8 *)DEheightptr; DEheightptr = HLheightptr; HLheightptr = _tmp; } // was EX DE,HL ($BEC7)
  Aheightbyte = *DEheightptr;

  // $BEC9
rm_height_regular_byte:
  state->scenedata.road_height_ptr = DEheightptr;
  Aheightbyte -= 16;

  // $BECF
rm_save_height_byte:
  state->height_byte = Aheightbyte;
  *HLheightptr = (Aheightbyte & 0xF) -
                 8; // problem: this writes through a ptr that should be const...

  // -- LANES ($BEDB) --
  HLlanesptr = state->road_buffer_start + ROADBUF_PTR2IDX(HLheightptr + 32);

  // RLE-counted bytes; counter stored separately.
  Alanes = state->lanes_counter_byte - 1;
  if (Alanes != 0xFF) // JR NC: counter was non-zero
    goto rm_lanes_count_resume;

  DElaneptr = state->scenedata.road_lanes_ptr + 1;
  Alanes = *DElaneptr;
  if (Alanes)
    goto rm_lanes_regular_byte;

  // Escape byte (0): read command byte.
  HLcurveptr = DElaneptr; DElanesptr = HLlanesptr; // was EX DE,HL ($BEEB)
  Alanes = *++HLcurveptr;
  HLcurveptr++;
  if (Alanes == 0)
    goto rm_lanes_jump_command;
  if (Alanes == 1)
    goto rm_lanes_one_command;

  // $BEF5
  // Command byte = fork road (2)
  state->rm_SM_BB9B_leftfork_lanes  = wordat(HLcurveptr + 0);
  state->rm_SM_BBC8_rightfork_lanes = wordat(HLcurveptr + 2);
  HLcurveptr = &forked_road_lanes[0];
  goto rm_read_lanes;

rm_lanes_one_command: // $BF09
  HLcurveptr = state->rm_SM_BF0A_lanes_one_command_ptr;
  goto rm_read_lanes;

rm_lanes_jump_command: // $BF0E
  HLcurveptr = lookup_map_goto(state, wordat(HLcurveptr));

rm_read_lanes: // $BF12
  DElaneptr = HLcurveptr; HLlanesptr = DElanesptr; // was EX DE,HL
  Alanes = *DElaneptr;

rm_lanes_regular_byte: // $BF14
  state->lanes_counter_byte = Alanes - 1; // DEC A; LD ($A247),A
  DElaneptr++;
  state->scenedata.road_lanes_ptr = DElaneptr;
  *HLlanesptr = *DElaneptr & 0xF7;
  state->rm_lanes_byte = *DElaneptr & 0xFB;
  goto rm_lanes_done;

rm_lanes_count_resume: // $BF29
  state->lanes_counter_byte = Alanes;
  Clanes = state->rm_lanes_byte;
  if (Clanes & 0x0C)
    state->rm_lanes_byte = Clanes & 0xF3;
  *HLlanesptr = Clanes;

rm_lanes_done: // $BF3A
  HLlanesptr = state->road_buffer_start + ROADBUF_PTR2IDX(HLlanesptr + 32);

  // -- SKIP CHECK ($BF3E) --
  // Every other call, rightside/leftside/hazards are skipped.

  if (state->session.no_objects_counter != 1) {
    *HLlanesptr = 0; // rightside slot
    HLlanesptr = state->road_buffer_start + ROADBUF_PTR2IDX(HLlanesptr + 32);
    *HLlanesptr = 0; // leftside slot
    HLlanesptr = state->road_buffer_start + ROADBUF_PTR2IDX(HLlanesptr + 32);
    *HLlanesptr = 0; // hazards slot
    state->session.no_objects_counter = 1;
    goto rm_all_hazards;
  }

  // -- RIGHT-SIDE OBJECTS ($BF55) --

  Arightside = state->rightside_byte - 16;
  if (Arightside < 240) // no carry = counter not yet exhausted
    goto rm_rightside_count_resume;

  DErightptr = state->scenedata.road_rightside_ptr + 1;
  Arightside = *DErightptr;
  if (Arightside)
    goto rm_rightside_regular_byte;

  // Escape byte (0): read command byte.
  HLcurveptr = DErightptr; DElanesptr = HLlanesptr; // was EX DE,HL ($BF65)
  Arightside = *++HLcurveptr;
  HLcurveptr++;
  if (Arightside == 0)
    goto rm_rightside_jump_command;
  if (Arightside == 1)
    goto rm_rightside_one_command;

  // Fork road command (byte == 2): $BF6F
  state->rm_SM_BB8E_leftfork_rightside  = wordat(HLcurveptr + 0);
  state->rm_SM_BBBB_rightfork_rightside = wordat(HLcurveptr + 2);
  HLcurveptr = &fork_leftrightobjs[1]; // $E2C1
  goto rm_read_rightside;

rm_rightside_one_command: // $BF83
  HLcurveptr = state->rm_SM_BF84_rightside_one_command_ptr;
  goto rm_read_rightside;

rm_rightside_jump_command: // $BF88
  HLcurveptr = lookup_map_goto(state, wordat(HLcurveptr));

rm_read_rightside: // $BF8C
  DErightptr = HLcurveptr; HLlanesptr = DElanesptr; // was EX DE,HL
  Arightside = *DErightptr;

rm_rightside_regular_byte: // $BF8E
  state->scenedata.road_rightside_ptr = DErightptr;
  Arightside -= 16;

rm_rightside_count_resume: // $BF94
  state->rightside_byte = Arightside;
  *HLlanesptr = Arightside & 0x0F;

  // -- LEFT-SIDE OBJECTS ($BF9E) --
  HLleftptr = state->road_buffer_start + ROADBUF_PTR2IDX(HLlanesptr + 32);

  Aleftside = state->leftside_byte - 16;
  if (Aleftside < 240) // no carry = counter not yet exhausted
    goto rm_leftside_count_resume;

  DEleftptr = state->scenedata.road_leftside_ptr + 1;
  Aleftside = *DEleftptr;
  if (Aleftside)
    goto rm_leftside_regular_byte;

  // Escape byte (0): read command byte.
  HLleftptr = DEleftptr; DEleftptr = HLleftptr; // was EX DE,HL ($BFAE)
  Aleftside = *++HLleftptr;
  HLleftptr++;
  if (Aleftside == 0)
    goto rm_leftside_jump_command;
  if (Aleftside == 1)
    goto rm_leftside_one_command;

  // Fork road command (byte == 2): $BFB8
  state->rm_SM_BB91_leftfork_leftside  = wordat(HLleftptr + 0);
  state->rm_SM_BBBE_rightfork_leftside = wordat(HLleftptr + 2);
  HLleftptr = &fork_leftrightobjs[0]; // $E2C0
  goto rm_read_leftside;

rm_leftside_one_command: // $BFCC
  HLleftptr = state->rm_SM_BFCD_leftside_one_command_ptr;
  goto rm_read_leftside;

rm_leftside_jump_command: // $BFD1
  HLleftptr = lookup_map_goto(state, wordat(HLleftptr));

rm_read_leftside: // $BFD5
  DEleftptr = HLleftptr; HLleftptr = DEleftptr; // was EX DE,HL
  Aleftside = *DEleftptr;

rm_leftside_regular_byte: // $BFD7
  state->scenedata.road_leftside_ptr = DEleftptr;
  Aleftside -= 16;

rm_leftside_count_resume: // $BFDD
  state->leftside_byte = Aleftside;
  *HLlanesptr = Aleftside & 0x0F;
  HLlanesptr = state->road_buffer_start + ROADBUF_PTR2IDX(HLlanesptr + 32);

  // -- HAZARDS ($BFE7) --

  Ahazards = state->hazards_byte - 1;
  if (Ahazards != 0xFF) // JR NC: counter was non-zero
    goto rm_no_hazards;

  DEhazptr = state->scenedata.road_hazard_ptr + 1;

rm_restart_hazards_read: // $BFF3
  Ahazards = *DEhazptr;
  if (Ahazards)
    goto rm_hazards_regular_byte;

  // Escape byte (0): read command byte.
  HLcurveptr = DEhazptr; DElanesptr = HLlanesptr; // was EX DE,HL ($BFF7)
  Ahazards = *++HLcurveptr;
  HLcurveptr++;
  if (Ahazards == 0)
    goto rm_hazards_jump_command;
  Ahazards--;
  if (Ahazards == 0) // cmd was 1
    goto rm_hazards_one_command;
  Ahazards--;
  if (Ahazards == 0) // cmd was 2
    goto rm_hazards_fork_road_command;
  Ahazards--; // A = cmd - 3
  if (Ahazards < 7) // cmd 3..9
    goto rm_hazards_set_hazard_command;
  if (Ahazards < 10) // cmd 10..12
    goto rm_hazards_set_arrow_command;
  if (Ahazards < 12) // cmd 13..14
    goto rm_hazards_spawning_command;
  // cmd >= 15: helicopter control = cmd - 14
  state->helicopter_control = Ahazards - 11;
  goto rm_do_restart;

rm_hazards_spawning_command: // $C018: cmd 13/14 → dont_spawn_cars = cmd - 13
  state->dont_spawn_cars = Ahazards - 10;
  goto rm_do_restart;

rm_hazards_set_arrow_command: // $C01F: cmd 10/11/12 → floating_arrow = cmd - 10
  Ahazards -= 7; // floating_arrow = A - 7 (from A = cmd-3)
  state->floating_arrow = Ahazards;
  if (Ahazards != 0)
    state->correct_fork = Ahazards;
  goto rm_do_restart;

rm_do_restart: // $C029
  DEhazptr = HLcurveptr; HLlanesptr = DElanesptr; // was EX DE,HL
  goto rm_restart_hazards_read;

rm_hazards_set_hazard_command: // $C02C: cmd 3..9 → rm_SM_C058_hazard_type = cmd - 3
  state->rm_SM_C058_hazard_type = Ahazards;
  goto rm_read_hazards;

rm_hazards_fork_road_command: // $C031
  state->rm_SM_BB8B_leftfork_hazards  = wordat(HLcurveptr + 0);
  state->rm_SM_BBB8_rightfork_hazards = wordat(HLcurveptr + 2);
  HLcurveptr = &fork_hazards[0];
  goto rm_read_hazards;

rm_hazards_one_command: // $C045
  HLcurveptr = state->rm_SM_C046_hazards_one_command_ptr;
  goto rm_read_hazards;

rm_hazards_jump_command: // $C04A
  HLcurveptr = lookup_map_goto(state, wordat(HLcurveptr));

rm_read_hazards: // $C04E: pointer already set up; byte is always regular here
  DEhazptr = HLcurveptr; HLlanesptr = DElanesptr; // was EX DE,HL
  Ahazards = *DEhazptr;

rm_hazards_regular_byte: // $C050
  state->scenedata.road_hazard_ptr = DEhazptr;
  Ahazards--;

rm_no_hazards: // $C055
  state->hazards_byte = Ahazards;
  *HLlanesptr = state->rm_SM_C058_hazard_type;
  state->session.no_objects_counter = 2;

rm_all_hazards: // $C05C (also entered from skip path with no_objects_counter=1)
  update_road_level(state);

  // Hazard update loop ($C062): iterate over all 6 hazard slots.
  IXhazard = &state->hazards[0];
  Covertake_bonus_counter = 0;
  for (Biterations = 6; Biterations > 0; Biterations--, IXhazard++) {
    u8 old_used = IXhazard->used;
    IXhazard->used = (u8)((old_used << 1) | (old_used >> 7)); // RLC
    if (!(old_used & 0x80)) // bit 7 was clear → skip
      continue;

    // rm_c080: bit 7 was set (hazard is active)
    Aloop = IXhazard->hazard_flags + 1;
    if (Aloop == 0) { // hazard_flags was 0xFF: rm_c096
      Aloop = IXhazard->distance;
      if (Aloop == 0) { // rm_c0b2: distance was 0
        IXhazard->hazard_lane_OR_perp_dist_hi--;
        continue;
      }
      Aloop--;
      IXhazard->distance = Aloop;
      if (Aloop != 0) // non-zero: keep going
        continue;
      // distance just hit 0
      if (IXhazard->hazard_lane_OR_perp_dist_hi != 0)
        continue;
      IXhazard->distance = 1;
      IXhazard->dist_frac = 0xFF;
      continue;
    }

    // hazard_flags != 0xFF: decrement distance
    if (--IXhazard->distance != 0)
      continue;
    IXhazard->used = 0; // mark unused
    if ((u8)(IXhazard->hazard_flags + 1) & 0x80) // RLA carry: hazard_flags >= 0x7F
      Covertake_bonus_counter++;
  }
  state->overtake_bonus_counter = Covertake_bonus_counter;

  // $C0BB: copy block if rm_SM_C0BB_fork_copy_pending is set
  if (state->rm_SM_C0BB_fork_copy_pending) {
    u8 *tbl = (u8 *)state->xpos_road_fork_right;
    u8 *HL = tbl + 0x73;
    u8 *DE = tbl + 0x77;
    int BC = 0x26;
    do {
      HL--; HL--;
      DE--; DE--;
      *DE = *HL; DE--; HL--; BC--;
      *DE = *HL; DE--; HL--; BC--;
    } while (BC != 0);
    HL++;
    *HL = 0;
  }

  // rm_allow_car_spawning ($C0D7) / rm_inc_spawning ($C0D9)
  state->allow_spawning++; // JP $AD0D is handled by caller (read_map)
}

/**
 * $C0E1: Prepare tunnel
 *
 * \param[in] state Pointer to game state.
 */
static void prepare_tunnel(chqstate_t *state)
{
  int  carry;
  int  A_c160;
  int  c160_is_zero; // bool, was Z
  u8   A_in_tunnel;
  u16 *HLtable;
  int  BCtablevalue1;
  u16 *HLdash_table;
  int  DEdash_tablevalue2;
  int  DEtablevalue3;
  int  saved_DE;            /* was stack */
  int  BCdash_tablevalue4;

  A_c160 = state->dt_tunnel_visible;
  c160_is_zero = (A_c160 == 0);
  A_in_tunnel = state->dr_in_tunnel;
  if (c160_is_zero) {
    /* Tunnel hasn't appeared */
    if (A_in_tunnel == 0) {
      state->tunnel_sfx = 0;

      /* NOP out draw_tunnel calls */
      state->dee_tunnel_1 = 0; /* NOP [$8F83/4 setting removed] */
      state->dee_tunnel_2 = 0; /* NOP [$8FA8/9 setting removed] */
      return;
    }

    /* Tunnel has appeared */
    state->tunnel_sfx = 5; // This quietens sfx when in the tunnel
    HLtable = &state->xpos_road_centre[0xF3 /
                                       2]; // somewhere in road height data table
    BCtablevalue1 = *HLtable;
    HLtable -= 4 / 2;

    // EXX - Bank
    HLdash_table = &state->xpos_road_centre[0xF1 / 2];
    DEdash_tablevalue2 = *HLdash_table;
    HLdash_table -= 4 / 2;

    do {
      // EXX - Unbank
      DEtablevalue3 = *HLtable;
      saved_DE = DEtablevalue3; // was PUSH DE
      HLtable -= 4 / 2;
      // EX DEtablevalue3,HLtable
      carry = (DEtablevalue3 < BCtablevalue1); // was SBC HL,BC
      BCtablevalue1 = saved_DE; // was POP BC
      if (carry)
        break; // Jump if #REGhl < #REGbc
      // EX DEtablevalue3,HLtable

      // EXX - Bank
      BCdash_tablevalue4 = *HLdash_table;
      HLdash_table -= 4 / 2;
      // EX DEdash_tablevalue2,HLdash_table
      if (DEdash_tablevalue2 < BCdash_tablevalue4) // was SBC HL,BC etc.
        break;
      // EX DEdash_tablevalue2,HLdash_table
      DEdash_tablevalue2 = BCdash_tablevalue4;
    } while (--A_in_tunnel);

    state->dt_tunnel_distance = 9 - A_in_tunnel;
    A_in_tunnel = 2; // set so it falls through
    state->dt_tunnel_visible = A_in_tunnel;
  }

  state->dt_far_wall_mode = A_in_tunnel ^ 1;

  /* Self modify #R$8F82 and #R$8FA7 to be CALL draw_tunnel. */
  state->dee_tunnel_1 = state->dee_tunnel_2 = 0xCD; // opcode of CALL
}

/**
 * $C15B: Draw tunnel
 *
 * \param[in] state Pointer to game state.
 * \param[in] IYheight Height table pointer. (was IY)
 */
static void draw_tunnel(chqstate_t *state, u8 *IYheight)
{
  int       carry = 0;
  int       Adistance;
  int       Avisible;
  int       Dfill;
  u8        L;
  u8        C;
  u16      *HL;
  u8        A;
  u8        D;
  int       E;
  u8        B;
  int       H;
  const u8 *DE;
  u16       DEfill;
  u8       *HLbackbuf;
  u8       *SPoutput;

  Adistance = IYheight - &state->height_table[0];
  if (Adistance != state->dt_tunnel_distance)
    return;

  Avisible = state->dt_tunnel_visible;
  Dfill = 0xEE; // Set fill value to use for striped tunnel entrance
  if (--Avisible)
    Dfill = 0xFF; // Set fill value to use for (much of) tunnel interior

  state->dt_fill_pattern = Dfill * 0x0101; // Widen fill value to $EEEE or $FFFF
  // Conv: Removed SP store

  L = ~((IYheight[0x4E] - 2) << 1);
  C = 0;
  HL = &state->xpos_road_centre_right[L / 2];
  A = *HL & 0xFF; // original loads byte here
  if (A == 0)
    goto dt_c18e;

  // This path is hit only when the tunnel is entered
  D = 16; // jump table target
  if ((s8) A < 0)
    goto dt_c19c;

dt_c188: // loop?
  D = 22; // jump table target
  L = 31;
  goto dt_c1c8;

dt_c18e:
  A = HL[-1 / 2]; // was DEC L:LD A,(HL):INC L
  A = (A & 0xF8) >> 3;
  A >>= 1;
  RR(C);
  A++;
  D = A;

dt_c19c:
  HL = &state->xpos_road_left[L / 2];
  A = *HL & 0xFF; // original loads byte here
  if (A == 0)
    goto dt_c1aa;

  E = 16;
  if ((s8) A < 0)
    goto dt_c1bc;

  goto dt_c188;

dt_c1aa:
  L--; // should move HL
  A = *HL + 8; // needs to set carry, or adjust below
  if (carry)
    goto dt_c188;
  A = (A & 0xF8) >> 3;

  B = A;
  E = 16 - (A >> 1);

dt_c1bc:
  A = 32 - B;
  L = 32;
  RL(C);
  if (carry)
    goto dt_c1c7;

  L--;
  A--;

dt_c1c7:
  C = A;

dt_c1c8:
  state->dt_fill_start_a = D; // jump table target
  state->dt_fill_start_b = E; // jump table target

  A = IYheight[0x35];
  B = A;
  H = (A & 15) + 0xF0;
  A = ((B & 0x70) << 1) + L; // needs to set carry
  if (carry)
    A--;

  L = A;
  A = 128 - B;
  // EX AF,AF'
  E = state->fast_counter & 0xE0;
  A = A - (E >> 2) - (E >> 4); // map (0,32,64,96,...,224) to (0,22,44,66,...,154)
  A += (IYheight - &state->height_table[0]); // was IYl
  DE = &persp_y_scale[A / 22][A % 22];
  B = *IYheight - B;
  A = *DE;
  E = A;
  A = (16 - (IYheight - &state->height_table[0])) + E; // was IYl
  // EXX
  B = A;
  // EXX
  A -= B;
  D = A;
  // EX AF,AF'
  A += D;
  if ((s8) A >= 0)
    goto dt_c21a;

  E = A;
  D -= A - 0x81;
  A = E;

dt_c21a:
  // EX AF,AF'
  B = D;

  // Pixels of tunnel loaded here. Top byte, D, seems to affect bottom row? Bottom
  // byte, E, affects whole pattern. The LD E,D later would explain that.
  DEfill = state->dt_fill_pattern; // pixels of tunnel
  do {
    SPoutput = HLbackbuf;
    A = L; // Preserve destination?
    switch (state->dt_fill_start_a) {
    default: assert(0);
    case  0: SPoutput -= 2; *SPoutput = DEfill;
    case  1: SPoutput -= 2; *SPoutput = DEfill;
    case  2: SPoutput -= 2; *SPoutput = DEfill;
    case  3: SPoutput -= 2; *SPoutput = DEfill;
    case  4: SPoutput -= 2; *SPoutput = DEfill;
    case  5: SPoutput -= 2; *SPoutput = DEfill;
    case  6: SPoutput -= 2; *SPoutput = DEfill;
    case  7: SPoutput -= 2; *SPoutput = DEfill;
    case  8: SPoutput -= 2; *SPoutput = DEfill;
    case  9: SPoutput -= 2; *SPoutput = DEfill;
    case 10: SPoutput -= 2; *SPoutput = DEfill;
    case 11: SPoutput -= 2; *SPoutput = DEfill;
    case 12: SPoutput -= 2; *SPoutput = DEfill;
    case 13: SPoutput -= 2; *SPoutput = DEfill;
    case 14: SPoutput -= 2; *SPoutput = DEfill;
    case 15: SPoutput -= 2; *SPoutput = DEfill;
    }
    A -= C;
    L = A; // restore HLbackbuf dest?
    SPoutput = HLbackbuf;
    switch (state->dt_fill_start_b) {
    default: assert(0);
    case  0: SPoutput -= 2; *SPoutput = DEfill;
    case  1: SPoutput -= 2; *SPoutput = DEfill;
    case  2: SPoutput -= 2; *SPoutput = DEfill;
    case  3: SPoutput -= 2; *SPoutput = DEfill;
    case  4: SPoutput -= 2; *SPoutput = DEfill;
    case  5: SPoutput -= 2; *SPoutput = DEfill;
    case  6: SPoutput -= 2; *SPoutput = DEfill;
    case  7: SPoutput -= 2; *SPoutput = DEfill;
    case  8: SPoutput -= 2; *SPoutput = DEfill;
    case  9: SPoutput -= 2; *SPoutput = DEfill;
    case 10: SPoutput -= 2; *SPoutput = DEfill;
    case 11: SPoutput -= 2; *SPoutput = DEfill;
    case 12: SPoutput -= 2; *SPoutput = DEfill;
    case 13: SPoutput -= 2; *SPoutput = DEfill;
    case 14: SPoutput -= 2; *SPoutput = DEfill;
    case 15: SPoutput -= 2; *SPoutput = DEfill;
    }
    A += C;
    L = A; // ie HLbackbuf
    HLbackbuf = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(HLbackbuf)));
    RLC(D);
    E = D;
  } while (--B > 0);

  // EXX
  B = (B >> 1) - (B >> 3);
  // EX AF,AF'
  if ((s8) A < 0)
    goto dt_exit;

  A += B;
  if ((s8) A >= 0)
    goto dt_c27b;

  E = A;
  A -= 0x81;
  A = -A;
  A += B;
  B = A;
  A = E;

dt_c27b:
  // EX AF,AF'
  A = B;
  // EXX
  B = A;
  A = L;
  A &= 0x0F;
  if (A)
    goto dt_c285;
  L--;

dt_c285:
  C = 0x0F;
  do {
    SPoutput = HLbackbuf;
    switch (state->dt_fill_start_b) {
    default: assert(0);
    case  0: SPoutput -= 2; *SPoutput = DEfill;
    case  1: SPoutput -= 2; *SPoutput = DEfill;
    case  2: SPoutput -= 2; *SPoutput = DEfill;
    case  3: SPoutput -= 2; *SPoutput = DEfill;
    case  4: SPoutput -= 2; *SPoutput = DEfill;
    case  5: SPoutput -= 2; *SPoutput = DEfill;
    case  6: SPoutput -= 2; *SPoutput = DEfill;
    case  7: SPoutput -= 2; *SPoutput = DEfill;
    case  8: SPoutput -= 2; *SPoutput = DEfill;
    case  9: SPoutput -= 2; *SPoutput = DEfill;
    case 10: SPoutput -= 2; *SPoutput = DEfill;
    case 11: SPoutput -= 2; *SPoutput = DEfill;
    case 12: SPoutput -= 2; *SPoutput = DEfill;
    case 13: SPoutput -= 2; *SPoutput = DEfill;
    case 14: SPoutput -= 2; *SPoutput = DEfill;
    }
    HLbackbuf = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(HLbackbuf)));
    RLC(D);
    E = D;
  } while (--B > 0);

  // EX AF,AF'
  if ((s8) A < 0)
    goto dt_exit;

  B = ~A + 0x82;
  DE = 0x0000;
  A = state->dt_far_wall_mode;
  if (A == 0)
    goto dt_c2c1;
  A--;
  if (A)
    goto dt_exit;
  DE--;
dt_c2c1:
  do {
    SPoutput = HLbackbuf;
    switch (state->dt_fill_start_b) {
    default: assert(0);
    case  0: SPoutput -= 2; *SPoutput = DEfill;
    case  1: SPoutput -= 2; *SPoutput = DEfill;
    case  2: SPoutput -= 2; *SPoutput = DEfill;
    case  3: SPoutput -= 2; *SPoutput = DEfill;
    case  4: SPoutput -= 2; *SPoutput = DEfill;
    case  5: SPoutput -= 2; *SPoutput = DEfill;
    case  6: SPoutput -= 2; *SPoutput = DEfill;
    case  7: SPoutput -= 2; *SPoutput = DEfill;
    case  8: SPoutput -= 2; *SPoutput = DEfill;
    case  9: SPoutput -= 2; *SPoutput = DEfill;
    case 10: SPoutput -= 2; *SPoutput = DEfill;
    case 11: SPoutput -= 2; *SPoutput = DEfill;
    case 12: SPoutput -= 2; *SPoutput = DEfill;
    case 13: SPoutput -= 2; *SPoutput = DEfill;
    case 14: SPoutput -= 2; *SPoutput = DEfill;
    }
    HLbackbuf = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(HLbackbuf)));
  } while (--B > 0);

dt_exit:
  // Conv: SP restore removed
}

/** Return byte pointer to the 256-byte Z80 road table at page H ($E7..$EC). */
static u8 *drsc_tbl(chqstate_t *state, int Htable_page)
{
  switch (Htable_page) {
  case 0xE7: return (u8 *)state->xpos_road_left - 256; /* within _gap_e364 */
  case 0xE8: return (u8 *)state->xpos_road_left;
  case 0xE9: return (u8 *)state->xpos_road_centre_left;
  case 0xEA: return (u8 *)state->xpos_road_centre;
  case 0xEB: return (u8 *)state->xpos_road_centre_right;
  default:   return (u8 *)state->xpos_road_right;        /* $EC */
  }
}

/**
 * $C2E7: Draw road scene change
 *
 * DPT: Claude generated this - check over
 *
 * Fills road-edge position table entries for a scene-change section (road
 * narrowing or widening). Uses a Bresenham-style algorithm to interpolate
 * road-edge x-positions between two height-table entries and writes them
 * to the appropriate road table via an SP-based write pointer.
 *
 * \param[in] state Pointer to game state.
 * \param[in] IX    Pointer into road buffer lane data.
 * \param[in] IY    Pointer into height table (height_table).
 */
static void draw_road_scene_change(chqstate_t *state, u8 *IXlanes, u8 *IYheight)
{
  u8   dist;         /* IYl = byte offset of IYheight within height_table */
  u8   H;            /* left-hand table hi byte ($E8/$E9/$EA/$EB/$EC) */
  u8   lane_flags;   /* IXlanes[0] */
  u8   L_sp;         /* L = ~((0x60 - IYheight[0]) << 1) -- always odd */
  u8   range;        /* Bresenham step count = IYheight[0] - C_val */
  u8   H_de;         /* table page for reading DE_road */
  u8   H_clamp;      /* table page for reading HL_clamp */
  u8   H_sp;         /* table page for SP output (after -256 adj) */
  u8   C_val;        /* C operand for range computation */
  u8   addval;       /* 0 or 0x20: offset applied to DE_road */
  u16  DE_road;      /* initial road-edge position from table */
  u16  HL_clamp_val; /* u16 from clamp-table read */
  u8   L_adj;        /* adjusted L for clamp read */
  s16  sbc;          /* HL_clamp_val - DE_road */
  u8   L_low;        /* low byte of sbc */
  s8   displacement; /* clamped signed displacement */
  u8   step;         /* |displacement| */
  u16  dir;          /* 1 (INC DE) or 0xFFFF (DEC DE) */
  u16  roadpos;      /* current road position in Bresenham loop */
  u16 *SP_out;       /* output table pointer */
  u8   accum;        /* Bresenham accumulator */
  u8   iters;        /* iteration counter */
  u8  *tbl;          /* byte table pointer */
  u8   new_acc;      /* temp for carry-detection in alternate Bresenham */

  /* $C2E7: load H = left-hand table hi byte from IXlanes[0] bits 0-1 */
  H    = (u8)((IXlanes[0] & 3) + 0xE7);
  dist = (u8)(IYheight - &state->height_table[0]);
  if (dist >= 19)
    goto c43b;

  lane_flags = IXlanes[0];
  if ((lane_flags & 0x0C) == 0)
    goto c43b;

  IYheight--;

  /* $C2FA-$C304: adjust H based on bits 5 and 7 of IXlanes[0] */
  if (lane_flags & 0x20) {          /* bit 5 set */
    H = 0xEC;
    if (!(lane_flags & 0x80))       /* bit 7 clear */
      H = 0xEB;
  }

  /* Defaults for bit-4-set paths: DE from H, clamp from H-1 */
  H_de    = H;
  H_clamp = (u8)(H - 1);

  if (!(lane_flags & 0x10))         /* bit 4 clear */
    goto c37e;

  /* Bit-4-set paths */
  if (dist >= 2)
    goto c357;

  /* $C310-$C354: path 1a -- bit4=1, dist<2 */
  addval = (lane_flags & 0x0C) == 4 ? 0x00 : 0x20;
  C_val  = (lane_flags & 0x0C) == 4 ? IYheight[2] : IYheight[1];
  range  = (u8)(IYheight[0] - C_val);
  if (range == 0 || (range & 0x80))
    goto c439;
  L_sp    = (u8)(~((u8)(0x60 - IYheight[0]) << 1));
  tbl     = drsc_tbl(state, H_de);
  DE_road = tbl[L_sp - 1] | (tbl[L_sp] << 8);
  DE_road = (u16)(DE_road - (((state->fast_counter & 0xE0) >> 3) + addval));
  goto c3ee;

c357:
  /* $C357-$C37B: path 1b -- bit4=1, dist>=2 (must be 4) */
  if (dist != 4)
    goto c439;
  range = (u8)(IYheight[0] - IYheight[2]);
  if (range == 0 || (range & 0x80))
    goto c439;
  L_sp    = (u8)(~((u8)(0x60 - IYheight[0]) << 1));
  tbl     = drsc_tbl(state, H_de);
  DE_road = tbl[L_sp - 1] | (tbl[L_sp] << 8);
  goto c3ee;

c37e:
  /* $C37E: bit-4-clear paths: swap table roles */
  H_de    = (u8)(H - 1);
  H_clamp = H;
  if (dist >= 2)
    goto c3ca;

  /* $C37E-$C3C7: path 2 -- bit4=0, dist<2 */
  addval = (lane_flags & 0x0C) == 4 ? 0x00 : 0x20;
  C_val  = (lane_flags & 0x0C) == 4 ? IYheight[2] : IYheight[1];
  range  = (u8)(IYheight[0] - C_val);
  if (range == 0 || (range & 0x80))
    goto c439;
  L_sp    = (u8)(~((u8)(0x60 - IYheight[0]) << 1));
  tbl     = drsc_tbl(state, H_de);
  DE_road = tbl[L_sp - 1] | (tbl[L_sp] << 8);
  DE_road = (u16)(DE_road + (((state->fast_counter & 0xE0) >> 3) + addval));
  goto c3ee;

c3ca:
  /* $C3CA-$C3ED: path 3ca -- bit4=0, dist>=2 (must be 4) */
  if (dist != 4)
    goto c439;
  range = (u8)(IYheight[0] - IYheight[2]);
  if (range == 0 || (range & 0x80))
    goto c439;
  L_sp    = (u8)(~((u8)(0x60 - IYheight[0]) << 1));
  tbl     = drsc_tbl(state, H_de);
  DE_road = tbl[L_sp - 1] | (tbl[L_sp] << 8);
  /* no offset -- fall through to c3ee */

c3ee:
  /* $C3EE-$C405: read second table value, compute clamped displacement */
  L_adj        = (u8)(L_sp - (u8)(range << 1));
  tbl          = drsc_tbl(state, H_clamp);
  HL_clamp_val = tbl[L_adj - 1] | (tbl[L_adj] << 8);
  sbc          = HL_clamp_val - DE_road;
  L_low        = sbc & 0xFF;
  if (sbc < 0)
    displacement = (L_low >= 0x80) ? (s8)L_low : (s8)0x81;
  else
    displacement = (L_low <  0x80) ? (s8)L_low : 0x7F;

  /* $C407-$C412: set up SP output pointer */
  H_sp   = (lane_flags & 0x20) ? (u8)(H - 1) : H;
  SP_out = (u16 *)(drsc_tbl(state, H_sp) + (u8)(L_sp + 1));

  /* $C413-$C420: derive step and direction */
  if (displacement < 0) {
    step = (u8)(-(s8)displacement);
    dir  = 0xFFFF; /* DEC DE */
  } else {
    step = (u8)displacement;
    dir  = 1;      /* INC DE */
  }

  roadpos = DE_road;
  iters   = range;

  if (range < step) {
    /* $C441-$C450: alternate Bresenham -- step > range */
    accum = 0;
    do {
      do {
        roadpos += dir;
        new_acc  = (u8)(accum + range);
        if (new_acc < accum || new_acc >= step) { /* overflow or >= step */
          accum = new_acc;
          break;
        }
        accum = new_acc;
      } while (1);
      accum  = (u8)(accum - step);
      *--SP_out = roadpos;
    } while (--iters > 0);
  } else {
    /* $C42E-$C437: normal Bresenham -- step <= range */
    accum = (u8)(range >> 1); /* LD A,B; RRA with carry=0 */
    do {
      accum += step;
      if (accum >= range) {
        accum  -= range;
        roadpos += dir;
      }
      *--SP_out = roadpos;
    } while (--iters > 0);
  }

c439:
  IYheight++; /* $C439: INC IYheight -- advance height pointer for next iteration */
c43b:
  state->dr_callback = dr_four_lane_highway;
  dr_dispatch_fill(state, 0, 0, 0, 0); // FIXME - must be crap
}

/**
 * $C452: Draw road
 *
 * This is the big cheese - the main road drawing function.
 *
 * \param[in] state Pointer to game state.
 */
static void draw_road(chqstate_t *state)
{
  u8 *IYheightptr;      /* was IY */
  int Chorizon;         /* was C */
  u8 *IXlanesptr;       /* was IX */
  int Blanesdataoffset; /* was B */
  int carry_stripe;     /* was carry */
  int Htable_offset;    /* was H */
  int Lstripe_height;   /* was L */
  int Axor_base;        /* was A */
  u8  Bfill_pattern;    /* was B */
  u8  Lrow;             /* was L */
  u16 DEbackbuf;        /* was DE */  // OR is this a *buffer* offset?

  state->on_dirt_track = 0;
  state->dt_tunnel_visible = 0;
  state->dr_in_tunnel = 0;

  // This affects the thickness of the road edges and lane markings with
  // increasing distance. Larger value => Lines remain thick into distance.
  state->dr_edge_thickness = 3;

  IYheightptr = &state->height_table[1];
  Chorizon = 96 - *IYheightptr; /* 96 = horizon row offset from screen bottom */
  IXlanesptr = ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET);

  // Set initial road stripe state
  Blanesdataoffset = IXlanesptr - &state->road_buffer_start[0];
  state->dr_initial_stripe_state = Blanesdataoffset & 1;
  carry_stripe = !!(Blanesdataoffset & (1 << 1)); // test bit 1 + ensure boolity

  Htable_offset = 0xD0;
  Lstripe_height = 16;
  Axor_base = Htable_offset;
  Bfill_pattern = _X_X_X_X; // chequerboard fill pattern
  if (!carry_stripe) {
    Htable_offset = 0x00;
    Lstripe_height = 48;
    Bfill_pattern = ________; // empty fill pattern
  }

  state->dr_stripe_xor_base = Axor_base; // always $D0
  state->dr_stripe_table_offset = Htable_offset; // $D0 or $00
  state->dr_edge_graphic_offset = Lstripe_height; // 16 or 48
  state->dr_right_edge_offset = Lstripe_height + 1; // 17 or 49
  state->dr_callback = dr_four_lane_highway;
  Lrow = 0xFF;
  // This is a Z80 address beyond the end of the buffer (not an offset!) - it
  // must be decremented before first write.
  DEbackbuf = 0x0100;
  state->dr_fill_pattern = Bfill_pattern;

  dr_read_lanes(state, IXlanesptr, IYheightptr, Bfill_pattern, Chorizon,
                DEbackbuf, Lrow); // was FALLTHROUGH
}

/**
 * $C4AD: draw_road: read lanes
 *
 * \param[in] state         Pointer to game state.
 * \param[in] IXlanesptr    Pointer into road buffer lanes bytes.
 * \param[in] IYheightptr   IY register value.
 * \param[in] Bfill_pattern Fill pattern.
 * \param[in] Chorizon      TBD - FIXME needs passing to sub-function somewhere
 * \param[in] DEbackbuf     Pointer into backbuffer.
 * \param[in] Lrow          Byte offset within road table page (0xFF = bottom scanline).
 */
static void dr_read_lanes(chqstate_t *state, u8 *IXlanesptr, u8 *IYheightptr,
                          int Bfill_pattern, int Chorizon, int DEbackbuf,
                          int Lrow)
{
  int carry;
  int Aleft_offset;             /* was A */
  u8  Ldash_lanes;              /* was L' */
  int Aleft_hand_table_hi;      /* was A */
  int Cdash_neg_lane_count;     /* was C' */
  int Hdash_in_tunnel;          /* was H' */
  int Atunnel_visible;          /* was A */
  int Cdash_fill_pattern;
  int Afill_pattern;

  Aleft_offset = *IXlanesptr & MAP_LANES_LEFT_OFFSET_MASK;
  if (Aleft_offset == 0) {
    // no left side calcs required in this case?
    // callback here is e.g. dr_four_lane_highway
    state->dr_callback(state, Bfill_pattern, Chorizon, DEbackbuf, Lrow); // exit via
    return;
  }

  // EXX - BANK

  Ldash_lanes = *IXlanesptr; // reload lanes byte
  // convert left hand pos (1+) to table hi byte ($E8+)
  Aleft_hand_table_hi = 0xE7 + Aleft_offset;
  state->dr_left_table_hi_1 = state->dr_left_table_hi_2 = Aleft_hand_table_hi;
  SLA(Ldash_lanes);
  if ((Ldash_lanes & (1 << 7)) == 0) {
    // Bit 6 was clear (NOT tunnel / dirt track / forked road)
    if (carry) {
      // Otherwise bit 7 was set indicating 3 lanes or 3/4 lanes
      // narrowing/widening.
      Aleft_hand_table_hi += 3; // $E8..$EA becomes $EB..$ED
      Cdash_neg_lane_count = -3;
    } else {
      // Two lane
      Aleft_hand_table_hi += 2; // $E8..$EA becomes $EA..$EC
      Cdash_neg_lane_count = -2;
    }
    state->dr_right_table_hi_2 = state->dr_right_table_hi_1 = Aleft_hand_table_hi;
    state->dr_neg_lane_count = Cdash_neg_lane_count; // Conv: A removed
    draw_road_scene_change(state, IXlanesptr, IYheightptr); // exit via
  } else {
    // Tunnel, dirt track or forked road
    if (carry == 0) {
      /* Otherwise it's a tunnel [confirmed in debugger]. */
      Cdash_fill_pattern = 0xFF;
      Hdash_in_tunnel = 1;
      if ((Ldash_lanes & (3 << 3)) != 0) { // having trouble understanding this bit
        // Tunnel transition
        state->dt_tunnel_distance = IYheightptr - &state->height_table[0];
        Atunnel_visible = 1;
        Cdash_fill_pattern = 0x00; // was INC C
        if ((Ldash_lanes & (1 << 5)) != 0) {
          /* Otherwise it's tunnel exit */
          Cdash_fill_pattern = 0xFF; // was DEC C
          Atunnel_visible = 2; // was INC A
          Hdash_in_tunnel = 0; // was DEC H
        }
        state->dt_tunnel_visible = Atunnel_visible;
      }

      state->dr_in_tunnel = Hdash_in_tunnel;
      state->dr_right_table_hi_2 = state->dr_right_table_hi_1 = 0xEB;
      state->dr_neg_lane_count = -1;
      Afill_pattern = Cdash_fill_pattern;

      // EXX - UNBANK

      dr_dispatch_fill(state, Afill_pattern, Chorizon, DEbackbuf, Lrow); // was exit via
    } else {
      // STILL BANKED HERE!

      if (Ldash_lanes & (1 << 6)) {
        // Forked road
        forked_road_plotter(state, IXlanesptr, IYheightptr); // was exit via
      } else {
        /* Dirt track check */
        // there must be other cases that can get here too
        state->on_dirt_track = ((Ldash_lanes & 0x18) == 0) ? 1 : 0;
        state->dr_neg_lane_count = -1;
        dr_set_lane_callback(state, Bfill_pattern, Chorizon, DEbackbuf, Lrow, dr_four_lane_highway); // exit via
      }
    }
  }
}

/**
 * $C534: draw_road: four lane highway
 *
 * \param[in] state         Pointer to game state.
 * \param[in] Bfill_pattern Fill pattern.
 * \param[in] Chorizon      Horizon level.
 * \param[in] DEbackbuf     Pointer into backbuffer.
 * \param[in] Lrow          Byte offset within road table page (row index).
 */
static void dr_four_lane_highway(chqstate_t *state, int Bfill_pattern,
                                 int Chorizon, int DEbackbuf, int Lrow)
{
  // EXX - UNBANK (we enter banked)

  // Set left/right hand road position to leftmost/rightmost
  state->dr_left_table_hi_1 = 0xE8;
  state->dr_left_table_hi_2 = 0xE8;
  state->dr_right_table_hi_1 = 0xEC;
  state->dr_right_table_hi_2 = 0xEC;
  state->dr_neg_lane_count = -4;

  dr_set_lane_callback(state, Bfill_pattern, Chorizon, DEbackbuf, Lrow,
                       dr_dispatch_fill); // was FALLTHROUGH
}

/**
 * $C54D: draw_road: set lane callback and enter fill dispatch
 *
 * \param[in] state           Pointer to game state.
 * \param[in] Bfill_pattern   Fill pattern.
 * \param[in] Chorizon        Horizon level.
 * \param[in] DEbackbuf       Pointer into backbuffer.
 * \param[in] Lrow            Byte offset within road table page (row index).
 * \param[in] HLdash_callback TBD
 */
static void dr_set_lane_callback(chqstate_t *state, int Bfill_pattern,
                                 int Chorizon, int DEbackbuf, int Lrow,
                                 dr_callback_t *HLdash_callback)
{
  state->dr_callback = HLdash_callback;
  // EXX - UNBANK (we enter banked)
  assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
  dr_dispatch_fill(state, Bfill_pattern, Chorizon, DEbackbuf, Lrow); // was FALLTHROUGH
}

/**
 * $C551: draw_road: dispatch to filled or unfilled path
 *
 * \param[in] state         Pointer to game state.
 * \param[in] Bfill_pattern Fill pattern.
 * \param[in] Chorizon      Horizon level.
 * \param[in] DEbackbuf     Pointer into backbuffer.
 * \param[in] Lrow          Byte offset within road table page (row index).
 */
static void dr_dispatch_fill(chqstate_t *state, int Bfill_pattern, int Chorizon,
                             int DEbackbuf, int Lrow)
{
  int Afill_pattern;

  Afill_pattern = Bfill_pattern;
  if (Afill_pattern) {
    assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
    dr_enter_filled(state, DEbackbuf, Lrow, Afill_pattern); // exit via
    return;
  }

  // EXX
  state->dr_fill_fn = dr_advance_unfilled;
  // EXX
  // EX AF,AF'
  assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
  dr_advance_unfilled(state, DEbackbuf, Lrow,
                        Afill_pattern); // was FALLTHROUGH
}

/**
 * $C55F: draw_road: advance backbuffer and rollover check (unfilled path)
 *
 * \param[in] state      Pointer to game state.
 * \param[in] DEbackbuf  Pointer into backbuffer.
 * \param[in] Lrow       Value
 * \param[in] Adash_fill Fill pattern.
 */
static void dr_advance_unfilled(chqstate_t *state, int DEbackbuf,
                                int Lrow, int Adash_fill)
{
  int Ahi;

  HI_DEC(DEbackbuf);
  Ahi = DEbackbuf >> 8;
  Ahi &= 0x0F;
  if (Ahi == 0) {
    assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
    dr_rollover_unfilled(state, DEbackbuf); // was exit via
    return;
  }

  assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
  dr_write_scanline_unfilled(state, DEbackbuf); // was FALLTHROUGH
}

/**
 * $C565: draw_road: write unfilled (zero) road scanline
 *
 * \param[in] state     Pointer to game state.
 * \param[in] DEbackbuf Pointer into backbuffer.
 */
static void dr_write_scanline_unfilled(chqstate_t *state, int DEbackbuf)
{
  int B;
  u16 DEdash_backbuf;
  u8  Ldash;
  u8  Hdash;
  u8 *SPoutput;
  u16 HLdash_fill;
  int Cdash;

  assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
  state->dr_backbuf_1 = DEbackbuf;
  B = 0xFF;

  // EXX - BANK

  DEdash_backbuf = state->dr_backbuf_1;
  Ldash = ((DEdash_backbuf & 0xFF) + 31) & 0xFF; // being explicit
  Hdash = DEdash_backbuf >> 8;
  SPoutput = OFFSETTOBACKBUF((Hdash << 8) | Ldash);


  if (SPoutput < &state->backbuffer[0])
    printf("before backbuf by %d bytes\n", &state->backbuffer[0] - SPoutput);
  else if (SPoutput > &state->backbuffer[BACKBUFFER_LENGTH])
    printf("after backbuf by %d bytes\n", SPoutput - &state->backbuffer[BACKBUFFER_LENGTH]);
  assert(VALID_BACKBUF_PTR(SPoutput));


  HLdash_fill = 0; // fill value
  Cdash = Ldash;
  dr_fill_left_stripe(state, SPoutput, 0 /* index */, DEdash_backbuf, HLdash_fill);
}

/**
 * $C57C: draw_road: backbuffer row advance with rollover (filled path)
 *
 * \param[in] state      Pointer to game state.
 * \param[in] DEbackbuf  Pointer into backbuffer.
 * \param[in] L          Value
 * \param[in] Adash_fill Value
 */
static void dr_rollover_filled(chqstate_t *state, int DEbackbuf,
                                   int Lrow, int Adash_fill)
{
  LO_ADD(DEbackbuf, 32);
  if ((DEbackbuf & 0xFF) < 32) // carry (low byte wrapped)
    HI_ADD(DEbackbuf, 16);
  assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
  dr_fill(state, DEbackbuf, Lrow, Adash_fill); // exit via
}

/**
 * $C58A: draw_road: backbuffer row advance with rollover (unfilled path)
 *
 * \param[in] state     Pointer to game state.
 * \param[in] DEbackbuf Pointer into backbuffer.
 */
static void dr_rollover_unfilled(chqstate_t *state, int DEbackbuf)
{
  LO_ADD(DEbackbuf, 32);
  if ((DEbackbuf & 0xFF) < 32) // carry (low byte wrapped)
    assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
  dr_write_scanline_unfilled(state, DEbackbuf); // exit via
}

/**
 * $C598: draw_road: enter filled path (set fill_fn and start advance)
 *
 * \param[in] state      Pointer to game state.
 * \param[in] DEbackbuf  Pointer into backbuffer.
 * \param[in] L          Value
 * \param[in] Adash_fill Fill pattern.
 */
static void dr_enter_filled(chqstate_t *state, int DEbackbuf,
                                int Lrow, int Adash_fill)
{
  // EXX
  state->dr_fill_fn = dr_advance_filled;
  // EXX
  // EX AF,AF'
  assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
  dr_advance_filled(state, DEbackbuf, Lrow, Adash_fill); // was FALLTHROUGH
}

/**
 * $C5A1: draw_road: advance backbuffer and rollover check (filled path)
 *
 * \param[in] state      Pointer to game state.
 * \param[in] DEbackbuf  Pointer into backbuffer.
 * \param[in] L          Value
 * \param[in] Adash_fill Fill pattern.
 */
static void dr_advance_filled(chqstate_t *state, int DEbackbuf, int Lrow,
                    int Adash_fill)
{
  int A;

  assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
  A = DEbackbuf >> 8;
  HI_DEC(DEbackbuf);
  if ((A & 0x0F) == 0) {
    assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
    dr_rollover_filled(state, DEbackbuf, Lrow, Adash_fill); // exit via
    return;
  }

  assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
  dr_fill(state, DEbackbuf, Lrow, Adash_fill); // was FALLTHROUGH
}

/**
 * $C5A7: draw_road: Setup and draw right and centre spans
 *
 * \param[in] state      Pointer to game state.
 * \param[in] DEbackbuf  Pointer into backbuffer.
 * \param[in] Lrow       Byte offset within road table page (row index); used as index into xpos_road tables.
 * \param[in] Adash_fill Fill pattern.
 */
static void dr_fill(chqstate_t *state, int DEbackbuf, int Lrow, int Adash_fill)
{
  int  carry = 0;
  int  Arow;
  int  Bneg_lane_count;
  int  Ldash_row;
  int  Ldash_backbuf;
  int  Bdash_holds_16;
  int  Cdash_mask;
  u8  *HLdash;
  int  Aleftval;
  u8   Aleft_stripe_width;
  u8   Aright_stripe_width;
  int  Edash;
  int  Arightval;
  u16  DEdash_backbuf;
  int  Hdash_backbuf;
  u8  *SPoutput;
  u16  HLfill;
  u16  BCzerofill;

  assert(DEbackbuf >= 0xF000 || DEbackbuf <= 0x0100);
  state->dr_backbuf_2 = DEbackbuf;
  Arow = Lrow; // byte offset within road table page (0xFF = bottom scanline)
  Bneg_lane_count = state->dr_neg_lane_count; // assigned but not used?

  // EXX - BANK

  Ldash_row = Arow;
  Bdash_holds_16 = 16;
  Cdash_mask = 0xF8; // propagate forward?

  HLdash = (u8 *)state->xpos_road_left + Ldash_row; // byte read: Ldash is a byte offset, not a u16 index
  Aleftval = *HLdash;
  if (Aleftval) {
    Aleft_stripe_width = ((s8) Aleftval < 0) ? 0 : 15;
  } else {
    Aleft_stripe_width = (HLdash[-1] & Cdash_mask) >> 3;
    RR(Aleft_stripe_width);
    Aleft_stripe_width += carry;
    if (Aleft_stripe_width >= Bdash_holds_16)
      Aleft_stripe_width--;
  }
  Edash = Aleft_stripe_width;
  Aleft_stripe_width = ~Edash + Bdash_holds_16;
  state->dr_left_stripe_width = Aleft_stripe_width;

  HLdash = (u8 *)state->xpos_road_right + Ldash_row; // byte read: Ldash is a byte offset, not a u16 index
  Arightval = *HLdash;
  if (Arightval) {
    Aright_stripe_width = ((s8) Arightval < 0) ? 0 : 15;
  } else {
    Aright_stripe_width = (HLdash[-1] & Cdash_mask) >> 3;
    RR(Aright_stripe_width);
  }
  state->dr_right_stripe_width = Aright_stripe_width;

  /* Calculate road jump table target */
  state->dr_road_width = ~Aright_stripe_width + Bdash_holds_16 + Edash;

  DEdash_backbuf = state->dr_backbuf_2;
  Ldash_backbuf = (DEdash_backbuf & 0xFF) + 31;
  Hdash_backbuf = DEdash_backbuf >> 8;
  SPoutput = OFFSETTOBACKBUF((Hdash_backbuf << 8) | Ldash_backbuf);
  assert(VALID_BACKBUF_PTR(SPoutput));

  // EX AF,AF' - unbank Afill

  RLC(Adash_fill);
  HLfill = Adash_fill + (Adash_fill << 8); // Widen stripe fill byte to whole word
  // EX AF,AF' - bank Afill
  BCzerofill = 0x0000; // Set road fill to zero

  /*  Fill right hand road stripes - starting from right hand side. */
  switch (state->dr_right_stripe_width / 1) {
  default: assert(0);
  case  0: SPoutput -= 2; *SPoutput = HLfill;
  case  1: SPoutput -= 2; *SPoutput = HLfill;
  case  2: SPoutput -= 2; *SPoutput = HLfill;
  case  3: SPoutput -= 2; *SPoutput = HLfill;
  case  4: SPoutput -= 2; *SPoutput = HLfill;
  case  5: SPoutput -= 2; *SPoutput = HLfill;
  case  6: SPoutput -= 2; *SPoutput = HLfill;
  case  7: SPoutput -= 2; *SPoutput = HLfill;
  case  8: SPoutput -= 2; *SPoutput = HLfill;
  case  9: SPoutput -= 2; *SPoutput = HLfill;
  case 10: SPoutput -= 2; *SPoutput = HLfill;
  case 11: SPoutput -= 2; *SPoutput = HLfill;
  case 12: SPoutput -= 2; *SPoutput = HLfill;
  case 13: SPoutput -= 2; *SPoutput = HLfill;
  case 14: SPoutput -= 2; *SPoutput = HLfill;
  case 15: break; // does this happen in practice?
  }

  /* Fill blank road surface - continuing from the right hand side. */
  switch (state->dr_road_width / 1) {
  default: assert(0);
  case  0: SPoutput -= 2; *SPoutput = BCzerofill;
  case  1: SPoutput -= 2; *SPoutput = BCzerofill;
  case  2: SPoutput -= 2; *SPoutput = BCzerofill;
  case  3: SPoutput -= 2; *SPoutput = BCzerofill;
  case  4: SPoutput -= 2; *SPoutput = BCzerofill;
  case  5: SPoutput -= 2; *SPoutput = BCzerofill;
  case  6: SPoutput -= 2; *SPoutput = BCzerofill;
  case  7: SPoutput -= 2; *SPoutput = BCzerofill;
  case  8: SPoutput -= 2; *SPoutput = BCzerofill;
  case  9: SPoutput -= 2; *SPoutput = BCzerofill;
  case 10: SPoutput -= 2; *SPoutput = BCzerofill;
  case 11: SPoutput -= 2; *SPoutput = BCzerofill;
  case 12: SPoutput -= 2; *SPoutput = BCzerofill;
  case 13: SPoutput -= 2; *SPoutput = BCzerofill;
  case 14: SPoutput -= 2; *SPoutput = BCzerofill;
  case 15: break; // does this happen in practice?
  }

  dr_fill_left_stripe(state, SPoutput, state->dr_left_stripe_width / 1, DEbackbuf,
          HLfill); // FALLTHROUGH
}

static void dr_fill_left_stripe(chqstate_t *state, u8 *SPoutput, int jump_index,
                    int DEbackbuf, int HLfill)
{
  // CHECK Are we banked on entry?

  assert(VALID_BACKBUF_PTR(SPoutput));

  switch (jump_index) {
  default: assert(0);
  case  0: SPoutput -= 2; *SPoutput = HLfill;
  case  1: SPoutput -= 2; *SPoutput = HLfill;
  case  2: SPoutput -= 2; *SPoutput = HLfill;
  case  3: SPoutput -= 2; *SPoutput = HLfill;
  case  4: SPoutput -= 2; *SPoutput = HLfill;
  case  5: SPoutput -= 2; *SPoutput = HLfill;
  case  6: SPoutput -= 2; *SPoutput = HLfill;
  case  7: SPoutput -= 2; *SPoutput = HLfill;
  case  8: SPoutput -= 2; *SPoutput = HLfill;
  case  9: SPoutput -= 2; *SPoutput = HLfill;
  case 10: SPoutput -= 2; *SPoutput = HLfill;
  case 11: SPoutput -= 2; *SPoutput = HLfill;
  case 12: SPoutput -= 2; *SPoutput = HLfill;
  case 13: SPoutput -= 2; *SPoutput = HLfill;
  case 14: SPoutput -= 2; *SPoutput = HLfill;
  case 15: break;
  }

#if 0
  B = E;
  C--;
  H = 0xE4; // Point #REGhl at road edge/markings table at $E4xx

  // EXX - bank/unbank

  // was H = state->dr_left_table_hi_1;
  switch (state->dr_left_table_hi_1) {
  default: assert(0);
  case 0xE8: HLxpos = &state->xpos_road_left[L]; break;
  case 0xE9: HLxpos = &state->xpos_road_centre_left[L]; break;
  case 0xEA: HLxpos = &state->xpos_road_centre[L]; break;
  }

  Axpos = *HLxpos;
  if (Axpos == 0) {
    // Otherwise zero.
    A = HLxpos[-1];

    // EXX - bank/unbank

    /* Build address of road edge marking graphic. */
    Eindex = A; // save index
    A &= 7; // select row number 0..7
    A <<= 2; // Turn into a scanline offset (4 bytes per row)
    A += state->dr_edge_graphic_offset; // Add offset of current edge marking graphic ($10,$30,$50,$70,$90,$B0)
    L = A; // Set #REGhl to road edge marking address

    // Top five bits select screen buffer addr?
    // If I break this it seems to affect the left hand side only.
    E = ((Eindex >> 3) & 31) + Bneg_lane_count;

    // AND-OR masking here. #REGde is address of screen buffer byte. #REGhl is
    // address of mask byte, followed by bitmap byte [then again since the edges are
    // 16x8]
    *DEbackbuf = (*DEbackbuf & HL[0]) | HL[1];
    HL += 3;
    DEbackbuf++;
    *DEbackbuf++ = *HL++; BC--; // was LDI

    // EXX - bank/unbank
  }

  // Perhaps a loop for road lane markings
  for (;;) {
    if (++B == 0)
      break; // exit loop

    HL += 256; // was H++
    if (*HL) // Read from road left drawing table e.g $E89F
      continue; // loop if nonzero

    A = HL[-1]; // CHECK needs wraparound?
    // EXX
    E = A;
    L = ((A & 7) << 1) + state->dr_stripe_table_offset;
    E = ((E >> 3) & 31) + B;
    *DEbackbuf++ = *HL++, BC--;

    /* This reads an (unmasked) road lane marker bitmap byte from $E4D0+. */
    *DEbackbuf = *HL;
    // EXX
  }

dr_c68a:
  H = state->dr_right_table_hi_1; // eg. $EC
  A = *HL;
  L--;
  if (A)
    goto dr_c6ab;
  A = *HL;
  // EXX
  E = A;

  /* Must be the right hand edge handling */
  L = ((A & 7) << 2) + state->dr_right_edge_offset; // - as above but * 4
  E = ((E >> 3) & 31) + B;
  *DEbackbuf++ = *HL++, BC--;

  /* AND-OR masking here. This affects road lines when altered. */
  A = *DEbackbuf & *HL;
  L++;
  *DEbackbuf = A | *HL;
  // EXX

dr_c6ab:
  L--;
  C--;
  if (NZ) {
    state->dr_fill_fn(state, B, D);
    return;
  }
  // EX AF,AF'
  B = A;

  /* This causes the alternating road stripes */
dr_set_stripes:
  A = state->dr_initial_stripe_state ^
      1; // loads (<lanes data offset> & 1) and toggles
  state->dr_initial_stripe_state = A;
  if (A)
    goto dr_stripe_perhaps_on;

dr_stripe_perhaps_off:
  A = state->dr_fill_pattern ^ 0x55; // toggle fill pattern
  state->dr_fill_pattern = A;

  B = A; // copy stripe fill
  /* This causes the road edge stripes. */
  state->dr_edge_graphic_offset ^=
    0x20; // switch between adjacent edge graphics (that are 32 bytes each)
  state->dr_right_edge_offset = A + 1; // ?advance past a mask byte (guess)?
  state->dr_stripe_table_offset ^= state->dr_stripe_xor_base;

dr_stripe_perhaps_on:
  A = state->dr_edge_thickness - 1; // load edge width value & decrement
  state->dr_edge_thickness = A;
  if (A)
    goto dr_edge_width_handled;

  /* Otherwise edge width value was zero */
  A = state->dr_stripe_xor_base + 16;
  if (carry)
    goto dr_edge_width_handled;

  state->dr_stripe_xor_base = A;
  C = A;
  A = state->dr_stripe_table_offset;
  if (A == 0)
    goto dr_c6f6;
  A = C;
  state->dr_stripe_table_offset = A;

dr_c6f6:
  state->dr_edge_graphic_offset += 64; // next road edge graphic?
  state->dr_edge_thickness = 5;

  // Sampled IY = $E301..$E315

dr_edge_width_handled:
  A = IY[0];
  IY++;

  // Sampled IX = $EEC5..$EED9
  IX.low++; // CHECK wrapping
  A -= IY[0];
  if (A == 0)
    goto dr_c715;
  if ((s8) A >= 0)
    goto dr_c794;
  goto dr_c746;

dr_c715:
  L -= 2;

  // Sampled IX = $EE2C ... EE39 EE45

  C = IX[0]; // Load the lanes flags byte
  if ((C & (1 << 6)) == 0) // Bit 6 indicates tunnel or dirt track
    goto dr_set_stripes; // Jump (likely a loop) if clear (neither tunnel nor dirt track)

  /* Tunnel or dirt track. */
  if (C & (1 << 7))
    goto dr_set_stripes; // Jump (loop?) if set (dirt track only)

  /* Tunnel */
  B = 255;
  A = 1; // Load 1  -- value for (tunnel related)
  if ((C & (1 << 2)) == 0)  // Bit 2 indicates tunnel start
    goto dr_c740;

  A = IY.low;
  state->dt_tunnel_distance = A;
  A = 1;
  B++;
  old_C = C;
  C = A;
  if ((old_C & (1 << 4)) == 0)
    goto dr_c73c;

  B--;
  A++;
  C--;

dr_c73c:
  state->dt_tunnel_visible = A;
  A = C;

dr_c740:
  state->dr_in_tunnel = A;
  goto dr_set_stripes;


  /* Variation: */

dr_c746:
  C = IX[0];
  if ((C & (1 << 6)) == 0)
    goto dr_c774;
  if (C & (1 << 6))
    goto dr_c774;

  // EX AF,AF'
  B = 255;
  A = 1;
  if ((C & (1 << 2)) == 0)
    goto dr_c770;

  A = IY.low; // load distance
  state->dt_tunnel_distance = A;

  A = 1;
  B++;
  old_C = C;
  C = A;
  if ((old_C & (1 << 4)) == 0)
    goto dr_c76c;

  B--;
  A++;
  C--;

dr_c76c:
  state->dt_tunnel_visible = A;
  A = C;

dr_c770:
  state->dr_in_tunnel = A;
  // EX AF,AF'

dr_c774:
  C = A;
  A = IY[0];
  IY++;
  IX.low++;
  A -= IY[0];
  if (A >= 0xE0)
    goto dr_c788;
  if (A >= 0x50)
    goto dr_start_backdrop_fill;

dr_c788:
  L -= 2;
  A += C;
  if ((s8) A <= 0)
    goto dr_c746;
  C = A;
  dr_read_lanes(state, IX, IY); // exit via
  return;

dr_c794:
  C = A;
  if (A < 0x50) {
    dr_read_lanes(state, IX, IY); // exit via
    return;
  }

  //  This entry point is used by the routines at #R$CBA4 and #R$CBC5.
dr_start_backdrop_fill:
  E++;
  HL = &state->dt_tunnel_visible; // Load address of x in 'LD A,x' @ #R$C160 (in draw_tunnel)
  A = state->dr_in_tunnel; // Read 'LD A,x' @ #R$C88F (tunnel related)
  A |= *HL;
  RR(A);
  if (carry)
    goto dr_start_sky_fill;

  C = D & 15;
  B = ~((E >> 1) + C) + 0x80;
  HLhorzlvl = state->session.horizon_level;
  C = 24;
  A = H;
  if ((s8) A < 0)
    goto dr_start_sky_fill;
  if (A)
    goto dr_c7ca;

  A = L - B;
  if (A >= B)
    goto dr_c7ca;
  A += C;
  if (!carry)
    goto dr_start_sky_fill;
  if (A == 0)
    goto dr_start_sky_fill;
  C = A;

dr_c7ca:
  A = C;
  state->dr_sky_rows = A; // Self modify xx in 'LD BC,$xxyy' @ #R$C80A
  A += B;
  if ((s8) >= 0)
    goto dr_c7db;
  A = -(A - 127) + C;
  state->dr_sky_rows = A;
  C = A;

dr_c7db:
  BC = (24 - C) * 3; // assuming no overflow from calc of C
  Ascroll = state->dr_horizon_x_scroll; // Load <self modified> horizon's horizontal shift value (ranges 0..19)
  // Decide whether to use the pre-shifted or the non-shifted version of the backdrop bitmap.
  RR(Ascroll);
  HLbackdropbitmap = &state->pre_shifted_backdrop[0];
  if (!carry)
    HLbackdropbitmap = &state->stage->backdrop[0];

dr_c7f2:
  HL += BC;
  // EXX - Bank/unbank
  // #REGa is 0..9 here, turn it back to 0..18 and invert to make jump table target.
  A = 18 - Ascroll * 2;
  state->dr_SM_C86C = A;
  memcpy(&state->dr_c82d_instrs[0], &backdrop_shifting_instrs[A], 18);

  BC = (state->dr_sky_rows << 8) + 10;
  // EXX
  A = L;
  // EX AF,AF'
  goto dr_c824;

  // Scanline advance pattern.
dr_c813:
  E -= 32;
  if (carry)
    goto dr_c82a;
  D -= 16;
  goto dr_c82a;

dr_c821_apparent_loop_start:
  // EXX bank/unbank
  // EX AF,AF'     ; Swap
  E = A;
dr_c824:
  A = D;
  D--;
  A &= 0x0F;
  if (A == 0)
    goto dr_c813;

  // This entry point is used by the routine at #R$C813.
dr_c82a:
  A = E;
  // EX AF,AF'
  L = A;

dr_c82d:
  // 18x NOP here

  *DEbackbuf++ = *HL++; BC--; // was LDI
  L = A;
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  L = A;
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  *DEbackbuf++ = *HL++; BC--; // was LDI
  L = A;

dr_c86c:
  switch (state->dr_SM_C86C / DIVIDER) {
  case 0: *DEbackbuf++ = *HL++; BC--; // was LDI
  case 1: *DEbackbuf++ = *HL++; BC--; // was LDI
  case 2: *DEbackbuf++ = *HL++; BC--; // was LDI
  case 3: *DEbackbuf++ = *HL++; BC--; // was LDI
  case 4: *DEbackbuf++ = *HL++; BC--; // was LDI
  case 5: *DEbackbuf++ = *HL++; BC--; // was LDI
  case 6: *DEbackbuf++ = *HL++; BC--; // was LDI
  case 7: *DEbackbuf++ = *HL++; BC--; // was LDI
  case 8: *DEbackbuf++ = *HL++; BC--; // was LDI
  }
  // EXX
  A += C;
  goto dr_c821_apparent_loop_start;

  // EXX
  // EX AF,AF'
  E = A;

  // This entry point is used by the routine at #R$C598.
dr_start_sky_fill:
  // EX DE,HL
  L += 30;

  DEfill = 0; // Fill value for blank sky
  /* The following value is tunnel related: 1 if in tunnel, 0 if not. */
  // It alternates fast when there is a partial tunnel on screen.
  if (state->dr_in_tunnel)
    /* In tunnel. Draw black scanlines for the sky instead of coloured. */
    DEfill--; /* $0000 -> $FFFF */

  Crowmask = 0x0F; // mask

  /* Decrement the screen address */
  for (;;) {
    A = H;
    H--; // decr row addr
    A &= Crowmask; // low 4bits of row addr
    if (A)
      goto dr_sky_fill_scanline; // easy case

    /* Otherwise it was zero so will need extra work. */
    L -= 32; // Decrement high three bits of the row address
    if (!carry)
      goto dr_sky_fill_fix_address;

    /* Exit. */
    // (restore SP was here)
    return;

dr_sky_fill_fix_address:
    H += 16; // no carry so fixup

    /* Writes #REGde to #REGhl 15 times filling the scanline. Draws the blank upper
     * part of the sky. DE must always be zero? or what about tunnels? */
dr_sky_fill_scanline:
    SPscr = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
    SPscr -= 2; *SP = DEfill;
  }
#endif
}

/**
 * $C8BE: Pre shift backdrop
 *
 * \param[in] state Pointer to game state.
 */
static void pre_shift_backdrop(chqstate_t *state)
{
  int       tmp;        // for RRD()
  const u8 *source;     /* was HL */
  u8       *preshifted; /* was DE */
  const u8 *endptr;     /* was DE */
  u8       *bmptr;      /* was HL */
  u8        row;        /* was C */
  u8        col;        /* was B */
  u8        pix;        /* was A */

  // Copy whole source bitmap to destination
  source     = &state->stage->backdrop[0];
  preshifted = &state->pre_shifted_backdrop[0];
  memcpy(preshifted, source, BACKDROP_LENGTH);

  // Shift it in-place by a nibble
  endptr = &preshifted[BACKDROP_WIDTH -
                       1]; // final scanline nibble rolls around to start
  bmptr  = &preshifted[0];
  row    = BACKDROP_HEIGHT;
  do {
    col = BACKDROP_WIDTH;
    pix = *endptr;
    do {
      assert(bmptr >= &state->pre_shifted_backdrop[0]);
      assert(bmptr < &state->pre_shifted_backdrop[BACKDROP_LENGTH]);
      RRD(pix, bmptr);
      bmptr++;
      endptr++; // TODO: Not used in inner loop - could hoist
    } while (--col > 0);
  } while (--row > 0);
}

/**
 * $C8E3: Forked road plotter
 *
 * Renders the screen scanlines for a road fork (two diverging lanes).
 * Computes five zone widths per scanline from road tables $E800/$E900/
 * $EA00/$EC00, fills them right-to-left with verge/road patterns, and
 * updates the road-edge stripe/thickness self-modifying state.
 *
 * \param[in] state Pointer to game state.
 * \param[in] IX    Lanes buffer pointer.
 * \param[in] IY    Height table pointer (into state->height_table).
 */
static void forked_road_plotter(chqstate_t *state, u8 *IXlanes, u8 *IYheight)
{
  /* SM fields - all reinitialized from draw_road on each call */
  u8  sm_CB65;   /* road edge thickness countdown */
  u8  sm_CB36;   /* stripe state (toggled per scanline pair) */
  u8  sm_CB5D;   /* XOR operand for sm_CA9D update */
  u8  sm_CA9D;   /* ADD operand for single-RLCA road-mark path */
  u8  sm_CB00;   /* ADD operand for dual-RLCA road-mark path */
  u8  sm_CA7A;   /* ADD operand base */
  u8  sm_CADC;   /* ADD operand base (second copy) */
  u8  sm_CB1C;   /* ADD operand = sm_CA7A + 1 */
  u8  sm_CABB;   /* ADD operand = sm_CA7A + 1 (second copy) */
  u8  sm_CB40;   /* fill pattern (verge texture) */
  /* Banked register state (from draw_road shadow context via EXX) */
  u8  D;         /* screen address high byte */
  u8  E;         /* screen address low byte */
  u8  B;         /* inner counter: 0 on first entry, 16 after frp_c969 init */
  u8  C;         /* scan-block counter (banked C; $F8 = 248 iterations) */
  u8  L;         /* road table byte index (decrements as road nears) */
  /* Per-scanline zone widths (each in 2-byte PUSH units, 0..15) */
  u8  z_lv;      /* lefthand verge width  = pos_E8 */
  u8  z_lr;      /* lefthand road width   = pos_E9 - pos_E8 */
  u8  z_mv;      /* middle verge width    = pos_EA - pos_E9 */
  u8  z_rr;      /* righthand road width  = pos_EC - pos_EA */
  u8  z_rv;      /* righthand verge width = 15 - pos_EC */
  /* Zone position accumulators */
  u8  pos_E8;    /* computed lefthand verge edge position */
  u8  pos_E9;    /* computed lefthand road end position */
  u8  pos_EA;    /* computed middle verge end position */
  u8  pos_EC;    /* computed righthand road end / verge complement */
  /* Misc */
  u8  loop_path; /* 0 = use frp_c923 (zero-fill) path; 1 = frp_c963 (5-zone) */
  u8  af_prime;  /* A' shadow: current scanline fill pattern */
  int A;
  int carry;
  int prev;
  const u8 *tbl;

  /* $C8E4-$C912: Copy SM operands from draw_road's current SM state.
   * In Z80 these are absolute self-modifying writes to the $CA/$CB region.
   * In C we hold them as locals since they are re-initialised every call. */
  sm_CB65  = state->dr_edge_thickness;
  sm_CB36  = state->dr_initial_stripe_state;
  sm_CB5D  = state->dr_stripe_xor_base;
  sm_CA9D  = state->dr_stripe_table_offset;
  sm_CB00  = state->dr_stripe_table_offset;
  sm_CA7A  = state->dr_edge_graphic_offset;
  sm_CADC  = state->dr_edge_graphic_offset;
  sm_CB1C  = state->dr_edge_graphic_offset + 1;
  sm_CABB  = state->dr_edge_graphic_offset + 1;
  sm_CB40  = state->dr_fill_pattern;

  /* EXX: restore banked register context (draw_road shadow registers).
   * In Z80 the shadow context holds the screen address and counters.
   * In C we approximate from available state; exact values require the
   * full EXX infrastructure to be implemented in draw_road. */
  D = state->dr_backbuf_1 >> 8;    /* screen high byte */
  E = state->dr_backbuf_1 & 0xFF;  /* screen low byte */
  B = 0;                               /* 0 on first call (banked B uninit) */
  C = 0xF8;                            /* scan-block count ($F8 = 248) */
  /* Banked L: set by draw_road at $C5B0 as 'LD L,C' where C = 96 - IYheight[0] */
  L = 0x60 - *IYheight;

  af_prime = sm_CB40; /* EX AF,AF': current scanline fill pattern */

frp_c915: /* $C915 */
  if (B != 0)
    goto frp_c95a;

  /* $C91A: B == 0 -- set jump target to frp_c923 (zero-fill path) */
  loop_path = 0;
  /* EX AF,AF' */
  af_prime = sm_CB40;

frp_c923: /* $C923 */
  A = D;
  D--;
  A &= 0x0F;
  if (A == 0)
    goto frp_next_scanline_c929;

frp_c929: /* $C929: zero-fill scanline (inner road, pre-fork area) */
#if 0
  /* $C929: LD ($C92F),DE  -- store current screen address
   * $C92E: LD DE,<SM>     -- reload
   * HL = DE+$1F, SP = HL, HL = 0, C = E+$1F
   * JP $CA57: 15 x PUSH HL (fills 30 bytes with 0) */
#endif
  C = E + 0x1F;
  B = E;  /* $CA66: LD B,E */
  C--;    /* $CA67: DEC C */
  goto frp_after_marking;

frp_next_scanline_c929: /* $C94C */
  carry = (E < 0x20);
  E     = E - 0x20;
  if (!carry)
    D = D + 0x10;
  goto frp_c929;

frp_c95a: /* $C95A: B != 0 -- set jump target to frp_c963 (5-zone path) */
  loop_path = 1;
  /* EX AF,AF' */
  af_prime = sm_CB40;

frp_c963: /* $C963 */
  A = D;
  D--;
  A &= 0x0F;
  if (A == 0)
    goto frp_next_scanline_c969;

frp_c969: /* $C969: 5-zone fork scanline render */
  /* $C970: LD BC,$10F8  (B=16 inner counter, C=$F8 scan-block mask) */
  B = 16;
  C = 0xF8; /* Conv: this is the banked C used to drive the scan-block loop */

  /* $C973: H = $E8 → read zone widths from road tables $E8/$E9/$EA/$EC */

  /* ---- Zone 5: lefthand verge (table $E800) ---- */
  tbl = (const u8 *)state->xpos_road_left;
  A   = tbl[L];
  if (A != 0) {
    pos_E8 = (A & 0x80) ? 0 : 15;
  } else {
    /* frp_c984: DEC L; A=HL[-1]; INC L; AND $F8; 3×RRCA; RRA; ADC; CP B; DEC */
    prev   = tbl[(u8)(L - 1)];
    pos_E8 = (u8)((prev >> 4) + ((prev >> 3) & 1)); /* 3×RRCA + RRA + ADC */
    if (pos_E8 >= 16)
      pos_E8--;
  }

  /* ---- Zone 4: lefthand road (table $E900) ---- */
  tbl = (const u8 *)state->xpos_road_centre_left;
  A   = tbl[L];
  if (A != 0) {
    pos_E9 = (A & 0x80) ? 0 : 15;
  } else {
    /* frp_c9aa: DEC L; A=HL[-1]; INC L; AND $F8; 3×RRCA; RRA (no ADC) */
    prev   = tbl[(u8)(L - 1)];
    pos_E9 = (u8)((prev & 0xF8) >> 4); /* 3×RRCA + RRA, no rounding */
  }

  /* ---- Zone 3: middle verge (table $EA00) ---- */
  tbl = (const u8 *)state->xpos_road_centre;
  A   = tbl[L];
  if (A != 0) {
    pos_EA = (A & 0x80) ? 0 : 15;
  } else {
    /* frp_c9c9: DEC L; A=HL[-1]; INC L; AND $F8; 3×RRCA; RRA; ADC; CP B; DEC */
    prev   = tbl[(u8)(L - 1)];
    pos_EA = (u8)((prev >> 4) + ((prev >> 3) & 1));
    if (pos_EA >= 16)
      pos_EA--;
  }

  /* ---- Zone 2+1: righthand road + verge (table $EC00, H skips $EB) ---- */
  tbl = (const u8 *)state->xpos_road_right;
  A   = tbl[L];
  if (A != 0) {
    pos_EC = (A & 0x80) ? 0 : 15;
  } else {
    /* frp_c9f0: DEC L; A=HL[-1] (no INC L -- L stays decremented) */
    L--;
    prev   = tbl[L]; /* read from new L (= L-1) */
    pos_EC = (u8)((prev & 0xF8) >> 4); /* 3×RRCA + RRA, no rounding */
  }

  /* Derive zone widths from cumulative positions */
  z_lv = pos_E8;                        /* lefthand verge */
  z_lr = (u8)(pos_E9 - pos_E8);         /* lefthand road */
  z_mv = (u8)(pos_EA - pos_E9);         /* middle verge */
  z_rr = (u8)(pos_EC - pos_EA);         /* righthand road */
  z_rv = (u8)(15    - pos_EC);          /* righthand verge */

#if 0
  /* $CA00-$CA65: Fill scanline right-to-left using SP as pointer.
   * DE = SM_CA01 (current scanline address); HL = DE+$1F; SP = HL.
   * EX AF,AF': rotate stripe fill pattern ($CA09-$CA0D).
   * BC = 0 (road fill = black); HL = rotated stripe pattern.
   * Five jump-table JRs (self-modified) skip into PUSH sequences:
   *   $CA11 JR → 0..15 × PUSH HL  = righthand verge  (z_rv pairs)
   *   $CA22 JR → 0..15 × PUSH BC  = righthand road   (z_rr pairs)
   *   $CA33 JR → 0..15 × PUSH HL  = middle verge     (z_mv pairs)
   *   $CA44 JR → 0..15 × PUSH BC  = lefthand road    (z_lr pairs)
   *   $CA55 JR → 0..15 × PUSH HL  = lefthand verge   (z_lv pairs)
   * Total = 15 pairs = 30 bytes; scanline is 32 bytes. */
  {
    /* JR offsets: $CA12=15-z_rv, $CA23=15-z_rr, $CA34=15-z_mv,
     *             $CA45=15-z_lr, $CA56=15-z_lv */
    u8  verge_pat = af_prime ^ (af_prime << 8);   /* HL fill = pattern */
    u16 road_pat  = 0;                            /* BC fill = 0 */
    u16 *sp = ... /* screen_ptr(DE) + 15; fill backwards */
              (void)z_lv; (void)z_lr; (void)z_mv; (void)z_rr; (void)z_rv;
    (void)verge_pat; (void)road_pat;
  }
#endif
  (void)z_lv; (void)z_lr; (void)z_mv; (void)z_rr; (void)z_rv;

  /* $CA66: B = E (save screen low byte); C-- */
  B = E;
  C--;

  /* $CA68-$CB2E: Road marking update (reads $E4/$E8/$E9/$EA/$EC tables,
   * writes road-edge and lane-marking pixels to the back buffer).
   * Similar to dr_c667_loop in draw_road but for the forked layout. */
#if 0
  /* Conv: road markings for the fork path not yet translated */
  {
    u8 H_mark = 0xE4;
    /* ... complex EXX+table read+AND-OR-masking chain ... */
    (void)sm_CA9D; (void)sm_CB00; (void)sm_CA7A;
    (void)sm_CADC; (void)sm_CB1C; (void)sm_CABB;
  }
#endif

frp_after_marking: /* $CB2F */
  L--;  /* $CB2F: DEC L */
  C--;  /* $CB30: DEC C (banked C) */
  if (C != 0) {
    /* $CB31: JP NZ, SM_CB32 */
    if (loop_path == 0)
      goto frp_c923;
    else
      goto frp_c963;
  }

  /* C == 0: scanline block complete.
   * $CB34: EX AF,AF'  $CB35: B = A (restore fill pattern) */
  B = af_prime;

frp_loop: { /* $CB36 */
    u8 tog = sm_CB36 ^ 1;
    sm_CB36 = tog;
    if (tog != 0)
      goto frp_cb65;
  }

  /* sm_CB36 == 0: update fill pattern and ADD operands */
  {
    u8 newpat = sm_CB40 ^ 0x55;
    sm_CB40 = newpat;
    B       = newpat;
    sm_CA7A = sm_CA7A ^ 0x20;          /* $CB4D: XOR $20 */
    sm_CADC = sm_CA7A;                 /* $CB50 */
    sm_CB1C = sm_CA7A + 1;             /* $CB54 */
    sm_CABB = sm_CB1C;                 /* $CB57 */
    {
      u8 nca9d = sm_CA9D ^ sm_CB5D; /* $CB5D: XOR <sm_CB5D> */
      sm_CA9D = nca9d;                    /* $CB5F */
      sm_CB00 = nca9d;                    /* $CB62 */
    }
  }

frp_cb65: /* $CB65 */
  sm_CB65--;
  if (sm_CB65 != 0)
    goto frp_cb90;

  /* sm_CB65 reached zero: advance XOR operand ($CB6E-$CB8D) */
  {
    u8 newxor = sm_CB5D + 0x10;
    if (newxor < sm_CB5D) /* carry: $CB73 JR C,$CB90 */
      goto frp_cb90;
    sm_CB5D = newxor;             /* $CB75 */
    if (sm_CA9D != 0)             /* $CB7C: AND A; JR Z,$CB83 */
      sm_CA9D = newxor;           /* $CB80 */
    sm_CA7A = sm_CA7A + 0x40; /* $CB86: ADD A,$40 */
    sm_CB65 = 5;                  /* $CB8D: reset thickness countdown */
  }

frp_cb90: { /* $CB90 */
    u8 old_h = *IYheight;
    IYheight++;
    WRAPPINGINCREMENT(IXlanes, state->road_buffer_start);
    A = (u8)(old_h - *IYheight);  /* $CB97: SUB (IYheight+$00) */
    if (A == 0) {           /* $CB9A: JR Z,$CB9F */
      L -= 2;
      goto frp_loop;
    }
    if ((s8)A > 0) {        /* $CB9C: JP P,$CBC5 */
      /* backdrop_fill_choice ($CBC5) */
      if (A >= 0x50)
        return; /* A >= 80: exit to draw_road backdrop fill */
      goto frp_c915; /* A < 80: re-enter forked road render */
    }
    /* A < 0 (negative): fall through */
    L = (u8)(L - 2);
    goto frp_loop;
  }

frp_next_scanline_c969: /* $C93E */
  carry = (E < 0x20);
  E     = E - 0x20;
  if (!carry)
    D = D + 0x10;
  goto frp_c969;
}

// $CBA4
// mystery_cba4 would go here, if we knew what it did

/**
 * $CBC5: Backdrop fill choice
 *
 * \param[in] state Pointer to game state.
 */
static void backdrop_fill_choice(chqstate_t *state)
{
//  C = A;
//  if (A < 80)
//    goto frp_c915; // inside forked_road_plotter
//  else
//    goto dr_start_backdrop_fill; // inside draw_road
}

/**
 * $CBD6: Build curve table
 *
 * \param[in] state  Pointer to game state.
 * \param[in] forked Non-zero if road is forked.
 */
static void build_curve_table(chqstate_t *state, int forked)
{
  u16       *table1, *table2;
  const u8  *road_buffer_ptr_HL; /* was HL */
  int        curvature_C;        /* was C */
  int        A;
  int        B;
  const u8  *IYheight;
  const u16 *IXlanes;
  u8        *DE;
  int        DEdash;
  int        curvature_A;
  int        HLdash;
  int        BCdash;
  int        carry;
  int        DEroadpos;          /* was DE */
  u8        *DEcurvature;
  const u8  *HLe760;
  int        Bdash;

  // Set up table pointer to *end* of tables we're building.
  if (forked) {
    table1 = &state->xpos_road_fork_right[128]; /* was $EE00 */
    table2 = &state->xpos_road_centre_right[128]; /* was $EC00 - centre right table? */
  } else {
    table1 = &state->xpos_road_right[128]; /* was $ED00 - right table */
    table2 = &state->xpos_road_left[128]; /* was $E900 - left table */
  }

  road_buffer_ptr_HL = state->road_buffer_offset;
  curvature_C = *road_buffer_ptr_HL;

  A = state->fast_counter & 0xE0;
  A = A - (A >> 2) - (A >> 4); // map (0,32,64,96,...,224) to (0,22,44,66,...,154)
  IYheight = &persp_x_scale_right[A / 22][0];

  // now need high byte of offset from base of struct, seems to be $E6 or $E7
  A = 0xE6 + ((IYheight - &persp_x_scale_right[0][0]) >> 8);
  A = multiply(A, curvature_C);
  A = (128 - A) & 0xFE; // 0xFE must round to whole word
  // A expecting $7C to $82 depending on curvature (7C if bending right?)
  A = (A - 0x40) / 2; // adjust to index inward_bend_table
  assert(A >= 0 && A <= 95);
  IXlanes = &curvature_to_xpos[A]; // table is 16-bit

  DE = &state->curvature_table[0];
  B = 20; // iterations
  // EXX Bank
  DEdash = state->scenedata.road_pos;
  // PUSH DEdash; // save on stack
  // EXX Unbank

  // Calculate curvature_table
  do {
    curvature_A = *road_buffer_ptr_HL;
    if (forked)
      curvature_A = -curvature_A;
    assert(curvature_A >= 0 && curvature_A <= 255);

    if (++road_buffer_ptr_HL == state->road_buffer_end)
      road_buffer_ptr_HL = state->road_buffer_start;

    // EXX Bank

    // Z80: ADD A,IXl; LD IXl,A  -- IXl accumulates curvature; table at $E540 = $40 into page
    // Conv: Z80 IXl wraps in 8-bit; values < 0x40 index before the table (adjacent Z80 RAM).
    // Clamp to table bounds rather than letting the pointer escape the array.
    int IXl = (IXlanes - &curvature_to_xpos[0]) * 2 + 0x40;
    IXl = (IXl + curvature_A) & 0xFF;
    if (IXl < 0x40) IXl = 0x40;
    IXlanes = &curvature_to_xpos[(IXl - 0x40) / 2];

    HLdash = 0; // Initialise a multiplier result
    BCdash = *IXlanes - DEdash;

    // reading first byte from table row?
    A = *IYheight++; // points into horizontal_e6b0

    // multiplier
    carry = ((A & (1 << 7)) != 0);
    A = (A << 1) & 0xFF;
    if (carry) HLdash = BCdash << 1;
    carry = ((A & (1 << 7)) != 0);
    A = (A << 1) & 0xFF;
    if (carry) HLdash += BCdash;
    HLdash <<= 1;
    carry = ((A & (1 << 7)) != 0);
    A = (A << 1) & 0xFF;
    if (carry) HLdash += BCdash;
    HLdash <<= 1;
    carry = ((A & (1 << 7)) != 0);
    A = (A << 1) & 0xFF;
    if (carry) HLdash += BCdash;
    HLdash <<= 1;
    carry = ((A & (1 << 7)) != 0);
    A = (A << 1) & 0xFF;
    if (carry) HLdash += BCdash;

    HLdash = (HLdash >> 8) + ((HLdash & (1 << 7)) != 0); // rounding
    A = HLdash & 0xFF;
    if (HLdash & (1 << 7)) HLdash |= 0xFF00;

    DEdash += HLdash;

    // EXX Unbank

    *DE++ = A; // write to curvature_table
  } while (--B);

  DEroadpos = state->scenedata.road_pos; /* was POP DE */
  B = 0; // init counter
  // EXX Bank
  build_curve_table_fill(state,
                             B,
                             table1, // table1 is $EE00 or $ED00 (right hand table)
                             DEroadpos);

  // repeat of above code - generate left hand table

  A = state->fast_counter & 0xE0;
  A = A - (A >> 2) - (A >> 4); // map (0,32,64,96,...,224) to (0,22,44,66,...,154)

  HLe760 = &persp_x_delta_left[A / 22][0];
  DEcurvature = &state->curvature_table[0];
  for (Bdash = 22; Bdash > 0; Bdash--)
    *DEcurvature++ += *HLe760++;

  DEroadpos = DEroadpos - 295; // vanishing point config (for left hand)

  Bdash = 0; // init counter
  // EXX Unbank
  build_curve_table_fill(state,
                             Bdash,
                             table2, // table2 is $EC00 or $E900 (left hand table)
                             DEroadpos);
}

// HL -> points past end of destination table we're filling
static void build_curve_table_fill(chqstate_t *state,
                                       int          Bdash_alwayszero,
                                       u16        *HLtableend,
                                       int         DEroadpos)
{
  u8  *IYheight_table;
  int  Biterations;
  u16 *SPoutput;
  int  A;
  int  Bdash;
  int  Cdash;
  int  Ldash;
  int  Aopcode;
  int  Atotal;
  u16  HLdash;

  IYheight_table =
    &state->height_table[0]; /* was 0xE300; // addr of height table */
  Biterations = 21;
  // (restore SP on exit, load SP with HL)
  SPoutput = HLtableend;
  do {
    // EXX Bank
    A = (Bdash_alwayszero - 2 + IYheight_table[0] - IYheight_table[1]) & 0xFF;
    IYheight_table++;
    if (A > 128) goto bct_endbit_negative; // if A is negative
    A += 2;
    state->object_positions[IYheight_table - 1 - &state->height_table[0]] =
      A; // must write to $E34F+ which is object_positions
    A -= Bdash_alwayszero;
    Bdash = A;
    Cdash = A;
    Ldash = state->curvature_table[IYheight_table - 1 -
                                   &state->height_table[0]]; // IY[$1F]; // $E320+
    if ((Ldash & (1 << 7)) != 0) {
      Ldash = -Ldash & 0xFF; // mask here to fix neg?
      Aopcode = 0x1B; // Opcode for DEC DE
      if (Bdash < Ldash) goto bct_endbit_A;
    } else {
      Aopcode = 0x13; // Opcode for INC DE
      if (A < Ldash) goto bct_endbit_A;
    }
    A = Bdash >> 1;
    do { // $CCE8
      A += Ldash;
      if (A >= Cdash) {
        A -= Cdash;
        if (Aopcode == 0x13) DEroadpos++;
        else DEroadpos--;
      }
      SPoutput--; *SPoutput = DEroadpos; // PUSH to output table
    } while (--Bdash > 0);
bct_continue:
    // EXX Unbank
  } while (--Biterations > 0);
  return;

  // #REGa is opcode of instruction (INC DE/DEC DE)
  // #REGb is max iterations
  // #REGc is ?
  // #REGl is ?
  // #REGde is ?
bct_endbit_A:
  Aopcode = A;
  Atotal = 0; // Initialise total to zero
  do {
    int overflow;
    do {
      if (Aopcode == 0x13) DEroadpos++;
      else DEroadpos--;
      Atotal += Cdash;
      overflow = Atotal > 0xff;
      Atotal &= 0xff;
    } while (!overflow && Atotal < Ldash);
    Atotal -= Ldash;
    SPoutput--; *SPoutput = DEroadpos; // PUSH to output table
  } while (--Bdash);
  goto bct_continue;

bct_endbit_negative:
  state->object_positions[IYheight_table - 1 - &state->height_table[0]] = 1;
  if (++A != 0) A++;
  Bdash = A;
  A = state->curvature_table[IYheight_table - 1 -
                             &state->height_table[0]]; // IY[$1F]; // $E320+
  //Ldash = A;
  //carry = (A & (1<<7) != 0;
  //Hdash = -carry; //sign ext
  HLdash = (s8) A;
  HLdash += DEroadpos;
  DEroadpos = HLdash; /* was EX */
  SPoutput--; *SPoutput = DEroadpos; // PUSH to output table
  goto bct_continue;
}

/**
 * $CD3A: Build height table
 *
 * \param[in] state Pointer to game state.
 */
static void build_height_table(chqstate_t *state)
{
  int       carry = 0;
  u8       *proadbuf_height;      /* was IY */
  u8       *proadbuf_height_base; // Conv: added
  int       heightbyte;           /* was C */
  int       counter;              /* was A */
  int       orig_counter;         /* was B */
  const u8 *pvtab;                /* was HL */
  const u8 *pvtabbase;            // Conv: added
  int       C;                    /* was C */
  int       iterations;           /* was B' */
  u8       *phtab;                /* was DE' */
  u8       *phtabbase;            // Conv: added
  int       v;                    /* was DE */
  u16       result;               /* was HL */
  u8        A;                    /* was A */
  u8       *pdst;                 /* was HL */
  u8       *pdstbase;             // Conv: added
  const u8 *htab2;                /* was DE */
  const u8 *htabbase2;            // Conv: added
  int       iterations2;          /* was B */

  proadbuf_height_base = proadbuf_height = ROADBUF_FWD2PTR(ROADBUF_HEIGHT_OFFSET);

  // Read the current height byte
  heightbyte = *proadbuf_height;

  // "Scale down" pattern
  counter = state->fast_counter & 0xE0;

  // Scale 0..223 (in steps of 16) to 0..153, reducing <counter> by 31.25%,
  // mapping the incoming value to the 7x22 byte tables. So fast_counter
  // indexes the rows of the table.
  orig_counter = counter; // Copy to be a multiplier later
  counter = COUNTER_TO_PERSP_Y_ROW(counter);

  pvtabbase = pvtab = &persp_y_scale[counter / 22][1];
  C = -multiply(orig_counter, heightbyte);
  // EXX - bank

  // This builds the look-up table at $E301. Assuming it's a height table.
  iterations = 21;
  phtabbase = phtab = &state->height_table[1];
  do {
    v = *pvtab * 2;
    result = 0;
    C = A = C + *proadbuf_height;
    if (C != 0) {
      if ((s8) C < 0) {
        v = -v;
        A = -C;
      }

      // multiplier
      A <<= 1; // Throw sign bit away?
      carry = (A >> 7) & 1; A <<= 1;
      if (carry) result = v << 1;
      carry = (A >> 7) & 1; A <<= 1;
      if (carry) result += v; result <<= 1;
      carry = (A >> 7) & 1; A <<= 1;
      if (carry) result += v; result <<= 1;
      carry = (A >> 7) & 1; A <<= 1;
      if (carry) result += v; result <<= 1;
      carry = (A >> 7) & 1; A <<= 1;
      if (carry) result += v; result <<= 1;
      carry = (A >> 7) & 1; A <<= 1;
      if (carry) result += v; result <<= 1;
      carry = (A >> 7) & 1; //A <<= 1;
      if (carry) result += v; result <<= 1;
      A = result >> 8;
    }
    A += *pvtab;
    WRAPPINGINCREMENT(pvtab, pvtabbase);

    *phtab = A; // Write #REGa to the table at $E3xx
    WRAPPINGINCREMENT(phtab, phtabbase);
    WRAPPINGINCREMENT(proadbuf_height, proadbuf_height_base);
  } while (--iterations > 0);

  *phtab = 0xA0;

  // Copy the table to $E336 while setting negative values to 96[?]
  pdstbase  = pdst  = &state->clamped_heights[0]; // destination
  htabbase2 = htab2 = &state->height_table[1]; // src
  iterations2 = 21;
  C = 96; // limit/minimum?
  do {
    s8 res; // Conv: added

    A = *htab2;
    res = A - C;
    if (res < 0)
      C = A;
    *pdst = C;
    WRAPPINGINCREMENT(pdst, pdstbase);
    WRAPPINGINCREMENT(htab2, htabbase2);
  } while (--iterations2 > 0);

  C = A = (C + 3) & 0xF8;
  A -= *pdst;
  *pdst = C;
  WRAPPINGINCREMENT(pdst, pdstbase);
  *pdst = A;
}

/**
 * $CDD6: Multiply
 *
 * \param[in] a     Parameter.
 * \param[in] c     C.
 * \return Non-zero on success.
 */
static int8_t multiply(int8_t a, int8_t c)
{
#if 1
  int b; // can be int, not T
  int e;
  int carry;

  b = 3;
  e = a;
  a = 0; // result
  do {
    carry = (e >> 7) & 1;
    e <<= 1;
    if (carry) a += c;
    a <<= 1;
  } while (--b);
  a >>= 1; // undo final doubling
  a >>= 2;
  carry = a & 1;
  a = (a >> 1) + carry;
  return a;
#else
  // This is theoretically equivalent but needs further testing.
  int t = (((a & 0xE0) >> 5) * c) >> 2;
  return (t >> 1) + (t & 1);
#endif
}

/**
 * $E810: Entrypt 48K
 *
 * \param[in] state Pointer to game state.
 */
static void entrypt_48k(chqstate_t *state)
{
  entrypt_common(state, 0, 3);
}

/**
 * $E816: Entrypt 128K
 *
 * \param[in] state Pointer to game state.
 */
static void entrypt_128k(chqstate_t *state)
{
  clear_playfield_attrs(state);
  entrypt_common(state, 1, 5);
}

/**
 * $E81D: Entrypt common
 *
 * \param[in] state      Pointer to game state.
 * \param[in] Amode_128k Amode 128k.
 * \param[in] Bnrelocs   Bnrelocs.
 */
static void entrypt_common(chqstate_t *state, int Amode_128k, int Bnrelocs)
{
#if 0
  static const struct Relocations {
    const u8 *src;
    ptrdiff_t dst;
    size_t    len;
  } relocations[] = {
    { transitions_e88e, offsetof(chqstate_t, transitions_ec00), sizeof(transitions_e88e) },
    { square_transition_mask, 0xEB00, sizeof(square_transition_mask) },
    { diamond_transition_mask, 0xEA00, sizeof(diamond_transition_mask) },
    // { 0xF220, 0x8014, 926 }, // copies load_stage_128k into place
    // { 0xE876, 0x83B5, 24 },  // copies hooks_128k
  };

  const struct Relocations *reloc; /* was HL */
  u8                   iterations; /* was BC */
#endif

  state->mode_128k = Amode_128k;

  memcpy(ADDRTOSCREEN(SCREEN_START_ADDRESS), marquee_initial,
         sizeof(marquee_initial));
  memcpy(ADDRTOSCREEN(SCREEN_ATTRIBUTES_START_ADDRESS), marquee_attrs,
         sizeof(marquee_attrs));
  if (Amode_128k == 0) {
    // stop_the_tape_48k(state);
  }

#if 0
  reloc      = &relocations[0];
  iterations =
    3; // Have to ignore requested nrelocs since 128K copies aren't done here
  do
    memcpy((char *) state + reloc->dst, reloc->src, reloc->len);
  while (--iterations > 0);
#endif

  // Conv: Load stage 1 data before attract mode starts (state->stage must not be NULL).
  state->stage = stages[0];

  bootstrap(state);
}

/**
 * $E8FE: "Stop the tape" handler (48K mode only)
 *
 * \param[in] state Pointer to game state.
 */
void stop_the_tape_48k(chqstate_t *state)
{
}

/**
 * $EBF7: Draws menu strings until it hits a NUL byte
 *
 * \param[in] state   Pointer to game state.
 * \param[in] strings List of menu strings to draw. NUL terminated. (was HL)
 */
void menu_draw_strings(chqstate_t *state, const u8 *strings)
{
  do
    strings = menu_draw_string(state, strings);
  while (*strings != 0);
}

/**
 * $EBFF: Draws a menu string
 *
 * A menu string has the structure: (byte: attribute byte, word: destination screen address, bytes: top bit set terminated ASCII string).
 *
 * \param[in] state    Pointer to game state.
 * \param[in] HLstring Menu string to draw. (was HL)
 * \return Address of next unconsumed byte.
 */
const u8 *menu_draw_string(chqstate_t *state, const u8 *HLstring)
{
  int carry = 0;
  int banked_carry = 0;
  u8  Cattribute;
  u16 DEscr;
  u16 HLattr;
  int Aascii;

  Cattribute = *HLstring;
  RL(Cattribute); // left shift topmost bit to carry (double height flag)
  banked_carry = carry;
  // EX AF,AF' - Bank
  SRL(Cattribute); // right
  HLstring++;
  DEscr = wordat(HLstring);
  HLstring += 2;
  // PUSH HLstring

  /* Calculate attribute address from screen address */
  HLattr = (0x5800 + ((DEscr >> 3) & 0x0300)) | (DEscr & 0xFF);
  // EXX - Bank
  // EX (SP),HLstring
  // PUSH DEdash, BCdash
  do {
    Aascii = *HLstring & 0x7F;
    // PUSH HLstring
    u8 *scr = ADDRTOSCREEN(DEscr);
    u8 *attr = ADDRTOATTRS(HLattr);
    menu_draw_char(Aascii,
                   banked_carry,
                   Cattribute,
                   scr,
                   attr,
                   &scr,
                   &attr);
    DEscr = SCREENTOADDR(scr);
    HLattr = SCREENTOADDR(attr);
    // POP HLstring
  } while ((*HLstring++ & EOS) == 0);
  // EXX - Unbank
  // POP BC, DE, HLstring
  // EXX - Bank

  return HLstring;
}

/**
 * $EC2C: Draw a character (menu system)
 *
 * \param[in]  Achar      ASCII character to draw. (was A)
 * \param[in]  Fdash      Double height flag; if set draw as double height. (was F')
 * \param[in]  Cdash      Screen attribute byte. (was C')
 * \param[in]  DEdash     Screen pixel address. (was DE')
 * \param[in]  HLdash     Screen attribute address. (was HL')
 * \param[out] DEdash_out Updated screen pixel address. (was DE')
 * \param[out] HLdash_out Updated screen attribute address. (was HL')
 */
static void menu_draw_char(int   Achar,
                           int   Fdash,
                           int   Cdash,
                           u8  *DEdash,
                           u8  *HLdash,
                           u8 **DEdash_out,
                           u8 **HLdash_out)
{
  const u8 *HLfont;       /* was HL */
  u8       *DEscreen;     /* was DE */
  int       Cglyphid;     /* was C */
  u8       *HLdash_saved; /* was B' */
  int       i;            // additional

  Achar -= ' ';
  if (Achar == 0) {
    // Space
    *HLdash_out = HLdash + 1;
    *DEdash_out = DEdash + 1;
    return;
  }

  // Map ASCII to glyph IDs
  Cglyphid = 0x12;
  if (Achar >= ('A' - ' ')) goto mdc_have_ascii;
  Cglyphid = 0x0B;
  if (Achar >= ('0' - ' ')) goto mdc_have_ascii;
  Cglyphid = 0;
  Achar--;
  if (Achar == 0) goto mdc_have_glyph;
  Cglyphid++;
  Achar -= 7;
  if (Achar == 0) goto mdc_have_glyph;
  Cglyphid++;
  Achar--;
  if (Achar == 0) goto mdc_have_glyph;
  Cglyphid++;
  Achar -= 3;
  if (Achar == 0) goto mdc_have_glyph;
  Cglyphid++;
  goto mdc_have_glyph;

mdc_have_ascii:
  Cglyphid = Achar - Cglyphid;

mdc_have_glyph:
  HLfont = &font[Cglyphid * 7]; // add symbol for glyph height
  // EXX
  // PUSH DEdash
  DEdash++; /* was INC E */
  // EXX
  // POP DE
  DEscreen = DEdash;
  if (!Fdash) { // checking banked carry here
    // double height
    for (i = 0; i < 4; i++) { // Conv: rolled
      *DEscreen = *HLfont;
      DEscreen += 256;
      *DEscreen = *HLfont++;
      DEscreen += 256;
    }
    DEscreen += 0xF81F;
    for (i = 0; i < 3; i++) { // Conv: rolled
      *DEscreen = *HLfont;
      DEscreen += 256;
      *DEscreen = *HLfont++;
      DEscreen += 256;
    }
    *DEscreen = 0; // final row always blank?
    // EXX
    HLdash_saved = HLdash; /* was just B' saving L' */
    Cdash |= ATTR_BRIGHT;
    *HLdash = Cdash; // set with bright set
    HLdash += 32; // move to next attr row
    Cdash &= ~ATTR_BRIGHT; // set with bright clear
    *HLdash = Cdash;
    HLdash = HLdash_saved + 1;
    // EXX
  } else {
    // single height
    for (i = 0; i < 7; i++) { // Conv: rolled
      *DEscreen = *HLfont++;
      DEscreen += 256;
    }
    // EXX
    *HLdash++ = Cdash; // Set the screen attribute and advance
    // EXX
  }

  *HLdash_out = HLdash;
  *DEdash_out = DEdash;
}

/**
 * $ECDA: Clear screen
 *
 * \param[in] state Pointer to game state.
 */
static void clear_screen(chqstate_t *state)
{
  memset(ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR), 0, 0x200);
  memset(ADDRTOSCREEN(SCREEN_PLAYFIELD_BITMAP_ADDR), 0, 0x1000);
}

/**
 * $ECF3: Redefine keys 48K
 *
 * \param[in] state Pointer to game state.
 */
static void redefine_keys_48k(chqstate_t *state)
{
  u16       DEscr;
  int       Biterations;
  int       Cindex;
  int       A;
  const u8 *DEshocked;
  const u8 *HLthing;

  for (;;) {
    clear_screen(state);

    menu_draw_strings(state, &messages_redefine_keys[0]);

    DEscr = 0x48D6;
    Biterations = 8;
    Cindex = 1;
    do {
      do {
        // PUSH HL,DE,BC
        play_music_48k(state);
        // POP BC,DE,HL

        // Wait for the keyboard to clear
        A = ~state->speccy->in(state->speccy, port_BORDER_EAR_MIC) & 0x1F;
      } while (A);

      define_a_key(state, Biterations, Cindex, DEscr);
      Cindex++;
      // HL++; might be stray code
    } while (--Biterations > 0);

    // All keys are now defined
    Biterations = 20;
    do
      // PUSH BC
      play_music_48k(state);
    // POP BC
    while (--Biterations > 0);

    // Test if keys are "SHOCKED<ENTER>"
    Biterations = 8;
    DEshocked = &shocked[0];
    HLthing = &state->temp_keydefs[0];
    do
      if (*DEshocked++ != *HLthing++)
        return;
    while (--Biterations > 0);

    // Matched: Show the test mode screen
    state->test_mode = 1;
    clear_screen(state);
    menu_draw_strings(state, &messages_test_mode[0]);

    // Wait for any key
    for (;;) {
      play_music_48k(state);
      A = ~state->speccy->in(state->speccy, port_BORDER_EAR_MIC) & 0x1F;
      if (A)
        break;
    }
  }
}

/**
 * $ED4D: Keyscan
 *
 * \param[in] state Pointer to game state.
 * \param[out] Dkeydef_out A keydef of the binary form kkkkkrrr (where k=key, r=row).
 * \return Non-zero if keys are pressed. Zero otherwise.
 */
static u8 keyscan_all(chqstate_t *state, u8 *Dkeydef_out)
{
  int carry = 0;
  int Dflag;      /* was D */
  int Ekeyandrow; /* was E */
  u8  Bport_hi;   /* was B */
  int Cport_lo;   /* was C */
  int Akeys;      /* was A */
  u8  Hkeys;      /* was H */
  int A;          /* was A */

  Dflag      = 0xFF;
  Ekeyandrow = 0x2F; // first keydef to try?
  Bport_hi   = 0xFE;
  Cport_lo   = 0xFE;

  do {
    Akeys = ~state->speccy->in(state->speccy, (Bport_hi << 8) | Cport_lo) & 0x1F;
    if (Akeys) {
      if (++Dflag)
        return 1; // Keys were pressed

      Hkeys = Akeys;
      A = Ekeyandrow;
      do {
        A -= 8;
        SRL(Hkeys);
      } while (carry);
      if (A)
        return 1; // Additional bits are set

      Dflag = A;
    }
    Ekeyandrow--;
    RLC(Bport_hi);
  } while (carry);

  return 0; // No keys were pressed
}

/**
 * $ED6D: Defines a single key
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bindex Index of ?. (was B)
 * \param[in] Cindex Key index we're defining. (was C)
 * \param[in] DEscreen Screen address to draw at - a Z80 address. (was DE)
 */
static void define_a_key(chqstate_t *state, int Bindex, int Cindex,
                         int DEscreen)
{
  int       carry;
  u8        Dkeydef;     /* was D */
  u8       *HLtmpkeys;   /* was HL */
  const u8 *HLkeynames;  /* was HL */
  int       Biterations; /* was B */
  int       Akeydef;     /* was A */

  // PUSH DEscreen,BC -- index
dak_loop1:
  do {
    do {
      play_music_48k(state);
      carry = keyscan_all(state, &Dkeydef);
    } while (carry);
    Dkeydef++;
  } while (Dkeydef == 0);
  Dkeydef--;
  Akeydef = Dkeydef;
  // POP BC -- get Cindex back
  // PUSH BC -- save index in Cindex
  HLtmpkeys = &state->temp_keydefs[0];
  Biterations = Cindex - 1;
  // Checking for existing uses of that key
  if (Biterations)
    do {
      if (Akeydef == *HLtmpkeys)
        goto dak_loop1; // Already used - try again
      HLtmpkeys++;
    } while (--Biterations > 0);
  // POP BC -- get Cindex back
  // PUSH BC -- retrieve index
  state->temp_keydefs[Cindex] = Akeydef;
  HLkeynames = &key_names[(Akeydef & 7) * 10 + (Akeydef >> 3) * 2]; // row + key
  // POP BC,DE
  // PUSH BC,DE
  state->messages_key_string[0] = 0xC7; // Conv: added
  setwordat(&state->messages_key_string[1], DEscreen);
  state->messages_key_string[3] = *HLkeynames++;
  state->messages_key_string[4] = *HLkeynames | EOS;
  menu_draw_string(state, &state->messages_key_string[0]);
  // POP DE
  DEscreen = dak_move_down(DEscreen);
  // POP BC
  if (Bindex == 4)
    DEscreen = dak_move_down(DEscreen);
}

/**
 * $EDCC: Dak move down
 *
 * \param[in] DE    Value.
 * \return Non-zero on success.
 */
static u16 dak_move_down(int DEscreen)
{
  u8 E;
  u8 D;

  E = (DEscreen & 0xFF) + 32;
  D = (DEscreen >> 8)   + 8;

  return (D << 8) | E;
}

/**
 * $EE40: Setup interrupts
 *
 * \param[in] state Pointer to game state.
 */
static void setup_interrupts(chqstate_t *state)
{
  // Conv: no equivalent in C
}

/**
 * $EE5E: Reset music
 *
 * \param[in] state Pointer to game state.
 */
static void reset_music(chqstate_t *state)
{
  state->music.drum_active = 0;
  state->music.extra_delay = 0;
  state->music.started = 0;
  next_pattern_at_addr(state, &music_patterns[0]); /* was FALLTHROUGH */
}

/**
 * $EE6E: Next pattern
 *
 * \param[in] state Pointer to game state.
 */
static void next_pattern(chqstate_t *state)
{
  if (--state->music.pattern_repeats)
    return;
  next_pattern_at_addr(state, state->music.pattern_addr); /* was FALLTHROUGH */
}

static void next_pattern_at_addr(chqstate_t *state, const u8 *HLpataddr)
{
  int       An_repeats; /* was A */
  int       Coffset;    /* was C */
  const u8 *HLdata;     /* was HL */

  for (;;) {
    An_repeats = *HLpataddr++;
    if (An_repeats != 0xFF) {
      // not end of pattern(s)
      state->music.pattern_repeats = An_repeats;
      Coffset = *HLpataddr++;
      state->music.pattern_addr = HLpataddr;

      // Calculate address of music data
      HLdata = &music_data[Coffset];
      state->music.note_delay_reload = state->music.note_delay = *HLdata++;
      state->music.pattern_start_ptr = HLdata;
      return;
    } else {
      // Restart
      HLpataddr = &music_patterns[wordat(HLpataddr) - 0xF0FE];
    }
  }
}

/**
 * $EE9E: Play music 48K
 *
 * \param[in] state Pointer to game state.
 */
static void play_music_48k(chqstate_t *state)
{
  int       Adelay;
  const u8 *HL;
  int       A;
  int       D;
  int       B;
  int       Aparam;

  state->music.irq_flag = 0;

  if (state->music.started == 0) {
    state->music.started = 1;
    goto pm_reset_pattern;
  }

  // delay?
  Adelay = state->music.note_delay - 1;
  if (Adelay) {
    state->music.note_delay = Adelay;
  } else {
    state->music.note_delay = state->music.note_delay_reload;
    HL = state->music.data_ptr;

    // Fetch a byte of the form 0bdaaaaiii (d is delay bit, aaaa is
    // argument, iii is instrument index)
    for (;;) {
      A = *HL - 1;
      if (A)
        break;

      next_pattern(state);

pm_reset_pattern:
      HL = state->music.pattern_start_ptr;
      state->music.data_ptr = HL; // not required
    }

    //pm_continue_pattern:
    state->music.data_ptr = ++HL;
    if (++A > 128) {
      // A byte of the form 0b1aaaaiii (1 is delay bit)
      A &= 0x7F;
      // EX AF,AF' bank
      state->music.note_delay = 1;
      state->music.extra_delay = 1;
      // EX AF,AF' unbank
    }

    D = A;
    B = D & 7;
    if (B) {
      Aparam = D >> 3; // general parameter
      // the call-return setup needs analysing here
      if (B == 1) { playdrum_2(state, Aparam); return; }
      if (B == 2) { playdrum_1(state, Aparam); return; }
      if (B == 3) { noise(state, Aparam); return; }
    }
  }

  if (state->music.extra_delay) {
    state->music.note_delay--;
    state->music.extra_delay--;
  }

  if (state->music.drum_active == 1) {
    // FIXME playdrum_bank_go(state, Ddash_length, HLdash_data); /* exit via */
  } else
    pm_wait_for_interrupt(state); /* was FALLTHROUGH */
}

static void pm_wait_for_interrupt(chqstate_t *state)
{
  while (state->music.irq_flag == 0)
    ;
}

/**
 * $EF19: Interrupt entry
 *
 * \param[in] state Pointer to game state.
 */
static void interrupt_entry(chqstate_t *state)
{
  state->music.irq_flag = 0xFF;
}

/**
 * $EF22: Playdrum 2
 *
 * \param[in] state  Pointer to game state.
 * \param[in] Aspeed Speed value.
 */
static void playdrum_2(chqstate_t *state, int Aspeed)
{
  playdrum_start(state, Aspeed, 108, &drum2[0]); /* exit via */
}

/**
 * $EF29: Playdrum 1
 *
 * \param[in] state  Pointer to game state.
 * \param[in] Aspeed Speed value.
 */
static void playdrum_1(chqstate_t *state, int Aspeed)
{
  playdrum_start(state, Aspeed, 252, &drum1[0]); /* was FALLTHROUGH */
}

/**
 * $EF2E: Playdrum start
 *
 * \param[in] state   Pointer to game state.
 * \param[in] Aspeed  Speed value.
 * \param[in] Dlength Dlength.
 * \param[in] HLdata  Source bitmap data.
 */
static void playdrum_start(chqstate_t *state, int Aspeed, int Dlength,
                           const u8 *HLdata)
{
  state->music.drum_speed = Aspeed;
  state->music.drum_active  = 1;
  playdrum_bank_go(state, Dlength, HLdata); /* was FALLTHROUGH */
}

/**
 * $EF38: Playdrum bank go
 *
 * \param[in] state        Pointer to game state.
 * \param[in] Ddash_length Ddash length.
 * \param[in] HLdash_data  Source bitmap data.
 */
static void playdrum_bank_go(chqstate_t *state, int Ddash_length,
                             const u8 *HLdash_data)
{
  // EXX
  playdrum_go(state, Ddash_length, HLdash_data);
}

/**
 * $EF39: Playdrum go
 *
 * \param[in] state   Pointer to game state.
 * \param[in] Dlength Dlength.
 * \param[in] HLdata  Source bitmap data.
 */
static void playdrum_go(chqstate_t *state, int Dlength, const u8 *HLdata)
{
  int Bdash_iterations;
  int A;

  do {
    Bdash_iterations = state->music.drum_speed; // aka speed
    do {
      A = port_MASK_EAR; // speaker bit
      // NOP
      if ((*HLdata & (1 << 7)) == 0)
        A = 0;
      state->speccy->out(state->speccy, port_BORDER_EAR_MIC, A);
      // FIXME This rotates the sample byte in-place ... RLC(*HLdata);
    } while (--Bdash_iterations > 0);
    HLdata++;
    if (--Dlength == 0)
      goto pd_end_of_sample;
    A = state->music.irq_flag;
  } while (A == 0);
  // EXX unbank
  return;

pd_end_of_sample:
  state->music.drum_active = 0;
  pm_wait_for_interrupt(state);
}

/**
 * $FC06: Noise
 *
 * \param[in] state  Pointer to game state.
 * \param[in] Aparam Aparam.
 */
static void noise(chqstate_t *state, int Aparam)
{
  int carry = 0;
  int Eduration;   /* was E */
  int Dinner;      /* was D */
  u8 *seed;        /* was HL */
  int B;
  u8  A;
  int Biterations; /* was B */

  Eduration = Aparam; // duration counter
  do {
    Dinner = 50;
    do {
      // Note that this is a different order of operations than in rng().
      seed = &state->rng_seed[0];
      *seed += 3;
      B = *seed++;
      A = *seed - 141;
      *seed = A;
      A += B;
      seed++;
      RLC(A);
      RRC(*seed);
      A += *seed;
      *seed = A;
      if (A & (1 << 4)) {
        Biterations = 24 - Eduration;
        while (--Biterations)
          ;
        state->speccy->out(state->speccy,
                           port_BORDER_EAR_MIC,
                           port_MASK_EAR | port_MASK_MIC);
        Biterations = Eduration;
        while (--Biterations)
          ;
        state->speccy->out(state->speccy, port_BORDER_EAR_MIC, 0);
      }
    } while (--Dinner > 0);

    // This whole interrupt check is redundant since AND A + RET C results in the
    // return never being taken. Should it be RET NZ instead? (RET Z messed things
    // up when I tried it.)
    A = state->music.irq_flag;
    if (0)
      return;
  } while (--Eduration > 0);
  pm_wait_for_interrupt(state);
}

// $F220 - load_stage_128k - merged into load_stage

/**
 * $F251: Start siren 128K
 *
 * \param[in] state Pointer to game state.
 */
static void start_siren_128k(chqstate_t *state)
{
  state->ay_chan_a_pitch =
    140; /* Conv: this sets the whole register, original just did the low byte */
  state->ay_chan_a_vol   = 14;
  state->ay_chan_b_vol   = 12;
  state->siren_pattern   = 0xAA;
  state->siren_enabled   = 0xAA;
}

/**
 * $F269: Play siren SFX 128K
 *
 * \param[in] state Pointer to game state.
 */
static void play_siren_sfx_128k(chqstate_t *state)
{
  int carry = 0;
  u8  pitch;   /* was A */
  u8  pattern; /* was B */

  if (state->siren_enabled == 0)
    return;

  pitch = state->ay_chan_a_pitch & 0xFF;
  pattern = state->siren_pattern;
  RLC(pattern);
  if (!carry) {
    /* Decreasing */
    pitch -= 3;
    if (pitch >= 90)
      goto set_regs;
  } else {
    /* Increasing */
    pitch += 3;
    if (pitch < 140)
      goto set_regs;
  }

  /* Arrive here if new fine pitch is outside of 90..139. */
  state->siren_pattern = pattern;

set_regs:
  // Arrive here if new fine pitch is 90..139.
  // CHECK Need to preserve high byte?
  state->ay_chan_a_pitch = (state->ay_chan_a_pitch & 0xFF00) | pitch;
  state->ay_chan_b_pitch = (state->ay_chan_b_pitch & 0xFF00) | (pitch - 4);
  state->ay_mixer &= 0x3C; // enable tone A & B
  write_audio_registers_128k(state); /* exit via */
}

/**
 * $F29D: Silence audio 128K
 *
 * \param[in] state Pointer to game state.
 */
static void silence_audio_128k(chqstate_t *state)
{
  state->ay_mixer = 0x3F; // all noise and tone channels disabled
  write_audio_registers_128k(state); /* was FALLTHROUGH */
}

/**
 * $F2A2: Write audio registers 128K
 *
 * \param[in] state Pointer to game state.
 */
static void write_audio_registers_128k(chqstate_t *state)
{
  const u8 *values; /* was HL */
  u8        regno;  /* was A */

  zxspectrum_t *speccy = state->speccy;

  values = &state->ay_env_fine; // final AY reg soft copy
  regno = 11; // reg 11
  do {
    speccy->out(speccy, 0xFFFD, regno);
    speccy->out(speccy, 0xBFFD, *values--); // was OUTD
  } while ((s8) --regno >= 0);
}

/**
 * $F2B6: Engine SFX from speed 128K
 *
 * \param[in] state Pointer to game state.
 */
static void engine_sfx_from_speed_128k(chqstate_t *state)
{
  u16 pitch;  /* was HL */
  u16 delta;  /* was DE */
  u8  volume; /* was A */

  pitch = ~(state->speed >> 1);
  // This is now part of the pitch divisor that we'll set later
  if (state->gear)
    // We're in high gear.
    pitch <<= 1; // Double divisor in #REGhl to lower the pitch
  pitch <<= 2; // Quadruple divisor in #REGhl to lower the pitch more
  if (!state->tunnel_sfx) { // Conv: moved
    delta = 0x190; // Not-in-tunnel base divisor (~277Hz)
    volume = 15; // Not-in-tunnel volume
  } else {
    // We're in the tunnel
    delta = 0x258; // In-tunnel base divisor (~185Hz)
    volume = 12; // In-tunnel volume
  }
  pitch += delta; // Add speed divisor to base divisor
  state->ay_chan_c_pitch =
    pitch; // Set Channel C pitch divisor (12-bit combined, fine and coarse registers)
  state->ay_chan_c_vol = volume; // Set Channel C volume
  state->ay_mixer &= 0x3B;
}

/**
 * $F2F1: Setup turbo SFX 128K
 *
 * \param[in] state Pointer to game state.
 */
static void setup_turbo_sfx_128k(chqstate_t *state)
{
  state->ay_noise_pitch        = 0x3C;
  state->turbo_sfx_noise_pitch = 0x3C;
}

/**
 * $F2FA: Play turbo SFX 128K
 *
 * \param[in] state Pointer to game state.
 */
static void play_turbo_sfx_128k(chqstate_t *state)
{
  if (state->turbo_sfx_noise_pitch == 0) {
    engine_sfx_from_speed_128k(state); /* exit via */
    return;
  }
  if (--state->turbo_sfx_noise_pitch == 0)
    return;

  if (--state->ay_noise_pitch) {
    state->ay_chan_c_pitch = state->ay_noise_pitch + 10;
    state->ay_mixer &= 0x1B; // Set mixer to enable Tone C and Noise C
    state->ay_chan_c_vol = 13;
    return;
  }

  state->ay_mixer |= 0x24; // Set mixer to disable Tone C and Noise C
  state->turbo_sfx_noise_pitch = 0;
  engine_sfx_from_speed_128k(state); /* exit via */
}

/**
 * $F342: Play speech 128K
 *
 * \param[in] state Pointer to game state.
 * \param[in] index Sound effect index. (was A)
 */
static void play_speech_128k(chqstate_t *state, int index)
{
  // $F32E
  static const struct {
    u16 length;
    u16 data;
  } speech_samples_table[5] = {
    { 0x0A8C, 0xC000 },
    { 0x14E6, 0xCA8C },
    { 0x0A5A, 0xDF72 },
    { 0x0ABE, 0xE9CC },
    { 0x0ADC, 0xF48A }
  };

  zxspectrum_t *speccy = state->speccy;
  int           carry = 0;
  int           Cport_lo;
  int           Hff;
  int           Lbf;
  int           Deight;
  int           DEdash_length;
  const u8     *HLdash_samples;
  int           Cdash_iterations;
  u8            Asample;
  int           Bport_hi;
  int           Aregno;

  assert(index >= 1 && index < SAMPLE__LIMIT); // 1-indexed, matching Z80 $F32E

  // EX AF,AF' - Bank index
  silence_audio_128k(state);
  speccy->out(speccy, 0x7FFD, 4);

  Cport_lo = 0xFD;
  Hff      = 0xFF;
  Lbf      = 0xBF;
  Deight   = 8; // Channel A volume register
  // EXX - Bank
  // EX AF,AF' - Unbank index
  DEdash_length  = speech_samples_table[index - 1].length;
  HLdash_samples = &sound_samples[speech_samples_table[index - 1].data];

  // There are two samples per byte so we iterate here.
  do {
    Cdash_iterations = 2;
    Asample = *HLdash_samples;
    // Get high nibble
    RR(Asample);
    RR(Asample);
    RR(Asample);
    RR(Asample);
    do {
      Asample &= 0x0F;
      // EX AF,AF' - Bank Asample
      // EXX - Unbank

      // Write sample as Channel A volume.

      Bport_hi = Hff; // Load $FF into #REGb to set high byte of port
      Aregno = Deight; // Load 8 into #REGa
      speccy->out(speccy, (Bport_hi << 8) | Cport_lo,
                  Aregno); // OUT (C),A -- Write to $FFFD to select register 8: Channel A volume
      Bport_hi = Lbf; // Load $BF into #REGb
      // EX AF,AF' - Unbank sample
      speccy->out(speccy, (Bport_hi << 8) | Cport_lo,
                  Asample); // OUT (C),A -- Write to $BFFD to write volume register
      // EX AF,AF' - Bank sample again

      // Write sample as Channel B volume.

      Aregno++; // Increment #REGa from 8 to 9
      Bport_hi = Hff; // Load $FF into #REGb to set high byte of port
      speccy->out(speccy, (Bport_hi << 8) | Cport_lo,
                  Aregno); // OUT (C),A -- Write to $FFFD to select register 9: Channel B volume
      Bport_hi = Lbf; // Load $BF into #REGb
      // EX AF,AF' - Unbank sample
      speccy->out(speccy, (Bport_hi << 8) | Cport_lo,
                  Asample); // OUT (C),A -- Write to $BFFD to write volume register
      // EX AF,AF' - Bank sample again

      // Write sample as Channel C volume.

      Aregno++; // Increment #REGa from 9 to 10
      Bport_hi = Hff; // Load $FF into #REGb to set high byte of port
      speccy->out(speccy, (Bport_hi << 8) | Cport_lo,
                  Aregno); // OUT (C),A -- Write to $FFFD to select register 10: Channel C volume
      Bport_hi = Lbf; // Load $BF into #REGb
      // EX AF,AF' - Unbank sample
      speccy->out(speccy, (Bport_hi << 8) | Cport_lo,
                  Asample); // OUT (C),A -- Write to $BFFD to write volume register
      // EXX - Bank

      // TODO Sort out delay handling
      // Delay for 19 DJNZ's.
      speccy->stamp(speccy); // stamp at start of loop?
      speccy->sleep(speccy, 19); // Delay loop (lower value => higher frequency)

      Asample = *HLdash_samples; // Load next sample (same byte, but next nibble)
    } while (--Cdash_iterations > 0); // Decrement nibble counter
    HLdash_samples++; // Advance to next byte of sample data
  } while (--DEdash_length > 0);

  reset_paging_128k(state); /* exit via */
}

/**
 * $F39F: Handle perp caught 128K
 *
 * \param[in] state Pointer to game state.
 */
static void handle_perp_caught_128k(chqstate_t *state)
{
  int Adelay;

  Adelay = state->overlay_delay;
  if (Adelay < 42)
    return;

  silence_audio_hook(state);

  state->siren_enabled         = 0;
  state->turbo_sfx_noise_pitch = 1;
  state->overlay_delay         = 1;

  call_bank_3_128k(state, BANK3_ROUTINE_6); /* was FALLTHROUGH */
}

/**
 * $F3B6: Call bank 3 128K
 *
 * \param[in] state     Pointer to game state.
 * \param[in] HLroutine Hlroutine.
 * \return Non-zero on success.
 */
static u8 call_bank_3_128k(chqstate_t *state, int HLroutine)
{
  switch (HLroutine) {
  default:
    assert(0);
    break;
  case BANK3_BOUNCY_LOGO:
    break;
  case BANK3_ROUTINE_3:
    break;
  case BANK3_ROUTINE_6:
    break;
  case BANK3_INPUT_SELECTION:
    break;
  }
  return 1;
}

/**
 * $F3E2: Page 128K
 *
 * \param[in] state Pointer to game state.
 */
static void page_128k(chqstate_t *state)
{
  // Conv: Removed
}

/**
 * $F414: Reset paging 128K
 *
 * \param[in] state Pointer to game state.
 */
static void reset_paging_128k(chqstate_t *state)
{
  // Conv: Removed
}

/**
 * $F41B: Attract mode 128K
 *
 * \param[in] state Pointer to game state.
 */
static void attract_mode_128k(chqstate_t *state)
{
  int       carry;
  int       enter_pressed;       /* was carry */
  int       routine;             /* was HL */
  int       result;              /* was A */
  int       attract_cycle;       /* was A */
  const u8 *DEmessages;          /* was DE */
  const u8 *HLmessages;          /* was HL */
  int       Atransition_control; /* was A */
  int       Adelay;              /* was A */

attract_mode_128k_start:
  routine = BANK3_BOUNCY_LOGO;
call_bank_3:
  result = call_bank_3_128k(state, routine);
  if (result == 0)
    return;

  set_up_stage(state, &state->stage->attract_data);

  state->attract_mode_128k_countdown = 2; // two runs through
  state->speed = SPEED_ATTRACT;
  for (;;) {
    cpu_driver(state);

    attract_cycle = state->attract_cycle;
    DEmessages = &enter_for_options_messages[0];
    if (attract_cycle) {
      if (keyscan(state) & USERINPUT_FIRE) {
        play_start_noise(state);
        return;
      }
      DEmessages = &press_gear_messages[0];
    }

    enter_pressed = ~state->speccy->in(state->speccy,
                                       port_KEYBOARD_ENTERLKJH) & 1; /* Conv: was IN+RRA */
    routine = BANK3_INPUT_SELECTION;
    if (enter_pressed)
      goto call_bank_3;

    HLmessages = DEmessages;
    // must be a flashing delay
    RRC(state->attract_mode_128k_blink);
    if (carry)
      print_message(state, *HLmessages, HLmessages);

    Atransition_control = state->transition_control;
    if (Atransition_control == 0) {
      Adelay = state->attract_mode_128k_countdown;
      if (Adelay < 0)
        goto attract_mode_128k_start;
      state->attract_mode_128k_countdown = --Adelay;
      if (Adelay < 0)
        setup_transition(state, TRANSITIONSTRIDE_FORWARD);
      else
        setup_overlay_messages(state, (Adelay > 0) ? &credits_messages_128[0] : &best_officers[0]);
    }

    transition(state);
    update_screen(state);
  }
}

/* ----------------------------------------------------------------------- */

CHQ_API void chq_setup(chqstate_t *state)
{
  if (RUN_FULL_GAME) {
    // try to run the full game
    if (setjmp(state->host_quit_jmp) == 0)
      entrypt_128k(state);
  } else {
    // run the pregame screen only
    memcpy(ADDRTOSCREEN(SCREEN_START_ADDRESS), marquee_initial,
           sizeof(marquee_initial));
    memcpy(ADDRTOSCREEN(SCREEN_ATTRIBUTES_START_ADDRESS), marquee_attrs,
           sizeof(marquee_attrs));
    state->current_stage_number = -1; // force load
    state->wanted_stage_number = 0;
    load_stage(state);
    run_pregame_screen(state);
  }
}

CHQ_API void chq_stop(chqstate_t *state)
{
  state->host_quit = 1;
}

CHQ_API void chq_main(chqstate_t *state)
{
  // There's no point calling this function yet. Not until the game logic is
  // teased apart. The real game logic hangs off of 'bootstrap'.
  assert(0);
  if (RUN_FULL_GAME) {
    main_loop(state);
  } else {
    while (run_pregame_screen_loop(state)) /* Conv: Split out */
      ;
  }
}

/* ----------------------------------------------------------------------- */

#ifdef CHQ_TESTS

#include "ChaseHQ/ChaseHQ-Tests.h"

void chq_test_load_stage(chqstate_t *state)
{
  load_stage(state);
}

void chq_test_set_up_stage(chqstate_t *state)
{
  set_up_stage(state, &state->stage->stage_data);
}

void chq_test_prime_road(chqstate_t *state, int iterations)
{
  while (iterations-- > 0)
    rm_cycle_buffer_offset(state, &state->fast_counter);
}

void chq_test_build_height_table(chqstate_t *state)
{
  build_height_table(state);
}

void chq_test_layout_road(chqstate_t *state)
{
  layout_road(state);
}

void chq_test_draw_road(chqstate_t *state)
{
  draw_road(state);
}

#endif /* CHQ_TESTS */
