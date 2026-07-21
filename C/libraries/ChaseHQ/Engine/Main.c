/**
 * Main.c
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

/* Notes
 *
 * This is a conversion to C of the ZX Spectrum version of Chase H.Q. It was
 * created by analysing the original game binary to the point where its Z80
 * functions could be disassembled and reimplemented and its data could be
 * marshalled into portable structures.
 *
 * Like with my conversion of "The Great Escape" to C we model the game as if
 * it's still running on a ZX Spectrum, including a Spectrum screen memory
 * layout, keyboard and sound. This avoids a full rewrite of the original code
 * and means that we leave numerous Z80-specific patterns in place. This allows
 * the code to remain a useful basis for comparison and lowers, but certainly
 * doesn't eliminate, the risk of translation errors. Although it's very
 * tempting to rewrite all the code to be fully idiomatic C the greater the
 * difference from the original disassembly the harder it gets to refer back to
 * it and realise our mistakes. The goal after all is to use this C conversion
 * to expose problem points and feed those back into the disassembly's
 * description.
 *
 * Ideally the ordering of the code will be preserved such that the original
 * game code and this reimplementation have broadly the same layout and
 * structure. Some code will unavoidably need to be changed however, such as
 * the stack trick where PUSH is used to accelerate bulk stores.
 *
 * My original intention was to retain the level data (called "stage" data in
 * this conversion to match the original game) whole in the converted game,
 * including any embedded addresses. I wanted to 'page in' levels by copying
 * the original game data into the game's state structure. This would also mean
 * that any new or adjusted levels produced by means of this conversion would
 * be compatible with the original game. However, it turned out that allowing
 * binary compatibility would have meant duplicating some core functions where
 * the data structures exist in both the engine part and the stage data. So I
 * gave up on that. Having the stage data in C does make it more tweakable,
 * which is good. Long term it would be nice if the stages - at least the map
 * portion - were expressible with a concise text format.
 *
 * Pointers present a problem. The original game data uses 16-bit pointers
 * sometimes embedded in byte data but the converted code could be using 32- or
 * 64-bit ones. Instead of embedding huge pointers we'll either use byte tokens
 * or leave the original values in place and indrect them through new tables or
 * switch lookups. For example see the "chatter" code: the code that prints the
 * messages on-screen as the game runs. It previously embedded addresses inline
 * in chatter structures. These are replaced with single bytes that reference a
 * new tables of pointers.
 *
 * As with my conversion of The Great Escape a state structure is added to
 * encapsulate the entire current game state. It is passed to every
 * state-accessing function in the game. Globals are banned.
 *
 * Screen handling in the original game assumes the alignment of the screen and
 * the back buffer. That can't be guaranteed in a portable conversion. We can
 * address this by converting pointers to offsets when we need to perform
 * address arithmetic.
 *
 * (SM) means self modified. There is a _lot_ of self-modified code in the
 * game.
 */

/* TODOs
 * - Decide how to drive the main loop(s).
 * - Promote variables to int from u8/s8/u16/s16 where possible.
 */

#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "C99/Types.h"
#include "ZXSpectrum/Macros.h"
#include "ZXSpectrum/Pixels.h"
#include "ZXSpectrum/Spectrum.h"
#include "ZXSpectrum/Z80.h"

#include "ChaseHQ/Data/CommonData.h"
#include "ChaseHQ/Data/SoundSamples.h"
#include "ChaseHQ/Data/Stage1Data.h"
#include "ChaseHQ/Data/Stage2Data.h"
#include "ChaseHQ/Data/Stage3Data.h"
#include "ChaseHQ/Data/Stage4Data.h"
#include "ChaseHQ/Data/Stage5Data.h"
#include "ChaseHQ/Data/Stage6Data.h"
#include "ChaseHQ/Data/Stages.h"

#include "Types.h"
#include "State.h"
#include "Bank3.h"

#include "ChaseHQ/ChaseHQ.h"

/* ----------------------------------------------------------------------- */

/* Z80 ish macros */

/** Return `t`+`d` but only alter the low byte. */
#define LO_ADD(t,d) ((t) = (((t) & ~0xFF) | (((t) + (d)) & 0xFF)))

/** Add `d` to the high byte of a 16-bit word `v` */
#define HI_ADD(v,d) ((v) += ((d) << 8))

/** Decrement the high byte of a 16-bit word `v` */
#define HI_DEC(v) ((v) = ((v) - 0x0100) & 0xFFFF)

/** Return ptr advanced by delta modulo 256. */
#define WRAP(ptr, delta, base) &(base)[((ptr) + (delta) - (base)) & 0xFF]

/** Assign ptr advanced by delta modulo 256. */
#define WRAP_ASSIGN(ptr, delta, base) ((ptr) = WRAP(ptr, delta, base))

/** Assign ptr advanced by 1 modulo 256. */
#define WRAP_INCREMENT_ASSIGN(ptr, base) WRAP_ASSIGN(ptr, 1, base)

/* ----------------------------------------------------------------------- */

/* Screen, attributes and backbuffer macros */

/* Pointer validators */

/** Return if ptr is within the screen bitmap, extended by left/right bytes. */
#define VALID_SCREEN_LR(ptr, left, right) \
((ptr) >= &state->speccy->screen.pixels[0] - (left) && (ptr) < &state->speccy->screen.pixels[SCREEN_BITMAP_LENGTH] + (right))

/** Return if ptr is within the screen attributes, extended by left/right bytes. */
#define VALID_ATTRS_LR(ptr, left, right) \
((ptr) >= &state->speccy->screen.attributes[0] - (left) && (ptr) < &state->speccy->screen.attributes[SCREEN_ATTRIBUTES_LENGTH] + (right))

/** Return if ptr is within the backbuffer, extended by left/right bytes. */
#define VALID_BACKBUF_PTR_LR(ptr, left, right) \
((ptr) >= &state->backbuffer[-left] && (ptr) < &state->backbuffer[BACKBUFFER_LENGTH + right])

#define VALID_SCREEN_PTR(ptr)  VALID_SCREEN_LR(ptr, 0, 0)
#define VALID_ATTRS_PTR(ptr)   VALID_ATTRS_LR(ptr, 0, 0)
#define VALID_BACKBUF_PTR(ptr) VALID_BACKBUF_PTR_LR(ptr, 0, 0)

/* Address validators */

/** Return if addr is within the screen bitmap address range, extended by left/right. */
#define VALID_SCREEN_ADDR_LR(addr, left, right) \
((addr) >= SCREEN_START_ADDRESS - (left) && (addr) < SCREEN_START_ADDRESS + SCREEN_BITMAP_LENGTH + (right))

/** Return if addr is within the screen attributes address range, extended by left/right. */
#define VALID_ATTRS_ADDR_LR(addr, left, right) \
((addr) >= SCREEN_ATTRIBUTES_START_ADDRESS - (left) && (addr) < SCREEN_ATTRIBUTES_START_ADDRESS + SCREEN_ATTRIBUTES_LENGTH + (right))

/** Return if addr is within the backbuffer address range, extended by left/right. */
#define VALID_BACKBUF_ADDR_LR(addr, left, right) \
((addr) >= BACKBUFFER_START_ADDRESS - (left) && (addr) < BACKBUFFER_END_ADDRESS + (right))

#define VALID_BACKBUF_ADDR(addr) VALID_BACKBUF_ADDR_LR(addr, 0, 0)

/* Offset validators */

/** Return if off is a valid screen bitmap byte offset, extended by left/right. */
#define VALID_SCREEN_OFFSET_LR(off, left, right) \
((off) >= -(left) && (off) < SCREEN_BITMAP_LENGTH + (right))

/** Return if off is a valid backbuffer byte offset, extended by left/right. */
#define VALID_BACKBUF_OFFSET_LR(off, left, right) \
((off) >= -(left) && (off) < BACKBUFFER_LENGTH + (right))


/* Address-to-pointer converters */

/** Return screen bitmap pointer given a Z80 address. */
#define ADDRTOSCREEN_M(addr) \
  (&state->speccy->screen.pixels[(addr) - SCREEN_START_ADDRESS])

u8 *z80addrtoscreen(chqstate_t *state, int addr, int left, int right)
{
  u8 *ptr;
  assert(VALID_SCREEN_ADDR_LR(addr, left, right));
  ptr = ADDRTOSCREEN_M(addr);
  assert(VALID_SCREEN_LR(ptr, left, right));
  return ptr;
}

#define ADDRTOSCREEN(addr)                 z80addrtoscreen(state, addr, 0, 0)

/** Return attributes pointer given a Z80 address. */
#define ADDRTOATTRS_M(addr) \
  (&state->speccy->screen.attributes[(addr) - SCREEN_ATTRIBUTES_START_ADDRESS])

u8 *z80addrtoattrs(chqstate_t *state, int addr, int left, int right)
{
  u8 *ptr;
  assert(VALID_ATTRS_ADDR_LR(addr, left, right));
  ptr = ADDRTOATTRS_M(addr);
  assert(VALID_ATTRS_LR(ptr, left, right));
  return ptr;
}

#define ADDRTOATTRS(addr)                 z80addrtoattrs(state, addr, 0, 0)

/** Return backbuffer[] pointer given a Z80 address. */
#define ADDRTOBACKBUF_M(addr) \
  (&state->backbuffer[(addr) - BACKBUFFER_START_ADDRESS])

static u8 *z80addrtobackbuf(chqstate_t *state, int addr)
{
  u8 *ptr;
  if (addr < 0x0020) {
    ptr = ADDRTOBACKBUF_M(0x10000 + addr);
  } else {
    //assert(addr >= BACKBUFFER_START_ADDRESS && addr < BACKBUFFER_END_ADDRESS);
    ptr = ADDRTOBACKBUF_M(addr);
  }
  return ptr;
}

#define ADDRTOBACKBUF(addr) z80addrtobackbuf(state, addr)

/* Pointer-to-offset converters */

/** Return byte offset of a screen[] pointer. */
#define SCREENTOOFFSET_M(ptr) \
  ((ptr) - &state->speccy->screen.pixels[0])

static int z80screentooffset(chqstate_t *state, const u8 *ptr, int left, int right)
{
  int off;
  assert(VALID_SCREEN_LR(ptr, left, right));
  off = SCREENTOOFFSET_M(ptr);
  assert(VALID_SCREEN_OFFSET_LR(off, left, right));
  return off;
}

#define SCREENTOOFFSET_LR(ptr, left, right) z80screentooffset(state, ptr, left, right)

/** Return byte offset of an attributes[] pointer. */
#define ATTRSTOOFFSET_M(ptr) \
  ((ptr) - &state->speccy->screen.attributes[0])

/** Return byte offset of a backbuffer[] pointer. */
#define BACKBUFTOOFFSET_M(ptr) \
  ((ptr) - &state->backbuffer[0])

static int z80backbuftooffset(chqstate_t *state, const u8 *ptr, int left, int right)
{
  int off;
  assert(VALID_BACKBUF_PTR_LR(ptr, left, right));
  off = BACKBUFTOOFFSET_M(ptr);
  assert(VALID_BACKBUF_OFFSET_LR(off, left, right));
  return off;
}

#define BACKBUFTOOFFSET(ptr)                 z80backbuftooffset(state, ptr, 0, 0)
#define BACKBUFTOOFFSET_LR(ptr, left, right) z80backbuftooffset(state, ptr, left, right)

/* Pointer-to-address converters */

/** Return a Z80 address of a screen[] pointer. */
#define SCREENTOADDR_M(ptr) \
  (SCREEN_START_ADDRESS + SCREENTOOFFSET_M(ptr))

static int z80screentoaddr(chqstate_t *state, const u8 *ptr, int left, int right)
{
  int addr;
  assert(VALID_SCREEN_LR(ptr, left, right));
  addr = SCREENTOADDR_M(ptr);
  assert(VALID_SCREEN_ADDR_LR(addr, left, right));
  return addr;
}

#define SCREENTOADDR(ptr)                 z80screentoaddr(state, ptr, 0, 0)

/** Return a Z80 address of an attributes[] pointer. */
#define ATTRSTOADDR_M(ptr) \
  (SCREEN_ATTRIBUTES_START_ADDRESS + ATTRSTOOFFSET_M(ptr))

static int z80attrstoaddr(chqstate_t *state, const u8 *ptr, int left, int right)
{
  int addr;
  assert(VALID_ATTRS_LR(ptr, left, right));
  addr = ATTRSTOADDR_M(ptr);
  assert(VALID_ATTRS_ADDR_LR(addr, left, right));
  return addr;
}

#define ATTRSTOADDR(ptr)                 z80attrstoaddr(state, ptr, 0, 0)

/** Return a Z80 address of a backbuffer[] pointer. */
#define BACKBUFTOADDR_M(ptr) \
  (BACKBUFFER_START_ADDRESS + BACKBUFTOOFFSET_M(ptr))

static int z80backbuftoaddr(chqstate_t *state, const u8 *ptr, int left, int right)
{
  int addr;
  assert(VALID_BACKBUF_PTR_LR(ptr, left, right));
  addr = BACKBUFTOADDR_M(ptr);
  assert(VALID_BACKBUF_ADDR_LR(addr, left, right));
  return addr;
}

#define BACKBUFTOADDR(ptr)                 z80backbuftoaddr(state, ptr, 0, 0)

/* Offset-to-pointer converters */

/** Return screen[] pointer given byte offset. */
#define OFFSETTOSCREEN_M(off) \
  (&state->speccy->screen.pixels[off])

static u8 *z80offsettoscreen(chqstate_t *state, int off, int left, int right)
{
  u8 *ptr;
  assert(VALID_SCREEN_OFFSET_LR(off, left, right));
  ptr = OFFSETTOSCREEN_M(off);
  assert(VALID_SCREEN_LR(ptr, left, right));
  return ptr;
}

#define OFFSETTOSCREEN(off)                 z80offsettoscreen(state, off, 0, 0)

/** Return backbuffer[] pointer given byte offset. */
#define OFFSETTOBACKBUF_M(off) \
  (&state->backbuffer[off])

static u8 *z80offsettobackbuf(chqstate_t *state, int off, int left, int right)
{
  u8 *ptr;
  assert(VALID_BACKBUF_OFFSET_LR(off, left, right));
  ptr = OFFSETTOBACKBUF_M(off);
  assert(VALID_BACKBUF_PTR_LR(ptr, left, right));
  return ptr;
}

#define OFFSETTOBACKBUF(off)                 z80offsettobackbuf(state, off, 0, 0)

/* ----------------------------------------------------------------------- */

/* Road buffer macros */

/** Given a road buffer pointer return a new wrapped-around buffer index. */
#define ROADBUF_PTR2IDX(PTR) \
  (((PTR) - state->roadbuf_start) & 0xFF)

/** Given a road buffer delta return a new wrapped-around buffer index. */
#define ROADBUF_FWD2IDX(N) \
  ROADBUF_PTR2IDX(state->roadbufptr + N)

/** Given a road buffer delta return a pointer. */
#define ROADBUF_FWD2PTR(N) \
  (&state->roadbuf_start[ROADBUF_FWD2IDX(N)])

/* ----------------------------------------------------------------------- */

/* Perspective table stuff */

/** Scale a raw speed counter (multiples of 32) to a persp_y_scale row offset
 * (multiples of PERSP_TABLE_COLS). */
#define COUNTER_TO_PERSP_Y_ROW(x) ((x) - ((x) >> 2) - ((x) >> 4))

/** Map state->fast_counter to a perspective table row index (0..7). */
#define FAST_COUNTER_PERSP_ROW(s) \
  (COUNTER_TO_PERSP_Y_ROW((s)->fast_counter & 0xE0) / PERSP_TABLE_COLS)

/* ----------------------------------------------------------------------- */

/* Read an arbitrary native word */
u16 wordat(const u8 *addr)
{
  return (addr[0] << 0) | (addr[1] << 8);
}

/* Write an arbitrary native word */
void setwordat(u8 *addr, int value)
{
  addr[0] = value;
  addr[1] = value >> 8;
}

/* Move to next screen row (downwards)
 *
 * Conv: Extracted to function.
 */
static u16 next_scr_row(int screen)
{
  int t;

  screen += 256;
  if (((screen >> 8) & 7) == 0) {
    t = (screen & 0xFF) + 32;
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
static u16 prev_buf_row(int backbuf)
{
  int orig;
  int t;

  assert(VALID_BACKBUF_ADDR(backbuf));

  orig = backbuf;
  backbuf -= 0x0100;
  if ((orig & 0x0F00) == 0) { /* field LLLL was zero on entry */
    t = (backbuf & 0xFF) - 32; /* decrement field RRRC */
    backbuf = (backbuf & 0xFF00) | (t & 0xFF);
    /* Conv: skool $B763 JR C,$B71C only restores the 1111 marker nibble
     * (1110 -> 1111) when the RRRC subtraction above did NOT borrow. When
     * RRR itself wraps past zero the marker is deliberately left
     * uncompensated. A previous translation applied this unconditionally,
     * corrupting the address whenever a sprite's row advance crossed the
     * very top of the back buffer (visible as missing top rows on tall,
     * close objects). */
    if (t >= 0)
      backbuf += 0x1000; /* 1110 -> 1111 */
  }
  // assert(VALID_BACKBUF_ADDR(backbuf));
  return backbuf;
}

/**
 * Given a ZX Chase HQ Z80 map address return the equivalent C conversion
 * pointer.
 *
 * This is for mapping addresses that I've decided to leave in-place for the
 * time being.
 */
static const void *lookup_map_goto(int current_stage_number, int z80)
{
  static const void *(*const stage_lookup_fns[])(u16) = {
    stage1_lookup_map_goto,
    stage2_lookup_map_goto,
    stage3_lookup_map_goto,
    stage4_lookup_map_goto,
    stage5_lookup_map_goto,
    stage6_lookup_map_goto,
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
    assert(current_stage_number >= 1 &&
           current_stage_number <= (int)NELEMS(stage_lookup_fns));
    return stage_lookup_fns[current_stage_number - 1](z80);
  }
}

/* ----------------------------------------------------------------------- */

typedef void dso_callback_t(chqstate_t     *state,
                            int             Bdepth,
                            const bitmap_t *HLbitmap,
                            const s16      *IXxpos,
                            const u8       *IYheight);

typedef void draw_object_entrypt_t(chqstate_t       *state,
                                   int               Acol_offset,
                                   int               Bdepth,
                                   const depthset_t *DEdepthset,
                                   const s16        *IXxpos,
                                   const u8         *IYheight);

/* ----------------------------------------------------------------------- */

static void show_end_screen(chqstate_t *state);

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
static void play_speech_hook(chqstate_t *state, int Asample); /* (was A) */
static void attract_mode_hook(chqstate_t *state);

static void bootstrap(chqstate_t *state);
static void main_loop(chqstate_t *state);

static void drive_attract_demo(chqstate_t *state);

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
                               int         style,
                               const u8   *messages);

static void setup_overlay_messages(chqstate_t *state, const u8 *message);
static void setup_overlay_messages_with_transition(chqstate_t *state,
    int          transition,
    const u8   *message);

static void draw_mugshots(chqstate_t *state);

static void draw_mugshot(chqstate_t *state, int BCattrs, int DEbackbuf,
                         const u8 *HLmugshot);
static void draw_smash_bar(chqstate_t *state);
static u16 draw_smash_bar_segments(chqstate_t *state, int C_nsegs, int HLbackbuf);
static u16 draw_smash_bar_solid_bit(chqstate_t *state, int B_nrows, int HLbackbuf);

static void draw_scene_objects(chqstate_t *state);

static void draw_stretchy_object_common(chqstate_t       *state,
                                        int               Bdepth,
                                        const stretchy_t *DEstretchy,
                                        dso_callback_t   *HLcallback,
                                        const s16        *IXxpos,
                                        const u8         *IYheight);

static void draw_tunnel_light_common(chqstate_t            *state,
                                     int                    Bdepth,
                                     const depthset_t      *DEdepthset,
                                     draw_object_entrypt_t *HLcallback,
                                     const s16             *IXxpos,
                                     const u8              *IYheight);

static void draw_object_left_entrypt(chqstate_t       *state,
                                     int               Acol_offset,
                                     int               Bdepth,
                                     const depthset_t *DEdepthset,
                                     const s16        *IXxpos,
                                     const u8         *IYheight);

static void draw_object_left_stretchy_entrypt(chqstate_t     *state,
                                              int             B_depth,
                                              const bitmap_t *HL_bitmap,
                                              const s16      *IX_xpos,
                                              const u8       *IY_height);

static void draw_object_left_width_entrypt(chqstate_t     *state,
                                                int             Awidth_bytes,
                                                const bitmap_t *HLbitmap,
                                                const u8       *IYheight);

static void draw_object_right_entrypt(chqstate_t       *state,
                                      int               Acol_offset,
                                      int               Bdepth,
                                      const depthset_t *DEdepthset,
                                      const s16        *IXxpos,
                                      const u8         *IYheight);

static void draw_object_right_stretchy_entrypt(chqstate_t     *state,
                                               int             Bdepth,
                                               const bitmap_t *HLbitmap,
                                               const s16      *IXxpos,
                                               const u8       *IYheight);

static void draw_object_right_width_entrypt(chqstate_t     *state,
                                                 int             Awidth_bytes,
                                                 const bitmap_t *HLbitmap,
                                                 const u8       *IYheight);

static void draw_object_perspective_entrypt(chqstate_t     *state,
                                            int             Awidth_bytes,
                                            int             Cpadding,
                                            const bitmap_t *HLbitmap,
                                            const u8       *IYheight);

static void draw_object_common_flipped(chqstate_t     *state,
                                       int             Bheight,
                                       int             Cpadding,
                                       int             Ebitmap_stride,
                                       const bitmap_t *HLbitmap,
                                       int             Adash_width_bytes,
                                       int             Fdash_zero,
                                       int             Fdash_carry,
                                       const u8       *IYheight);

static void draw_object_clipped(chqstate_t     *state,
                                int             zero_flipped,
                                int             carry_masked_flag,
                                int             A_width_bytes,
                                int             B_height,
                                int             Cpadding,
                                int             Ebitmap_stride,
                                const bitmap_t *HLbitmap,
                                const u8       *IYheight);

static void plot_sprite(chqstate_t *state,
                        int         width_bytes,
                        u8         *backbuf_addr,
                        int         height,
                        int         bitmap_stride,
                        const u8   *bitmap_data);
static u8 *plot_sprite_even(chqstate_t *state,
                            int         jump_offset,
                            u8         *backbuf_addr,
                            int         height,
                            int         bitmap_stride,
                            const u8   *bitmap_data);
static u8 *plot_sprite_odd(chqstate_t *state,
                           int         width_bytes,
                           u8         *backbuf_addr,
                           int         height,
                           int         bitmap_stride,
                           const u8   *bitmap_data);
static u8 *plot_sprite_odd_entrypt(chqstate_t *state,
                                   int         jump_offset,
                                   u8         *backbuf_addr,
                                   int         height,
                                   int         bitmap_stride,
                                   const u8   *bitmap_data);

static void plot_sprite_flipped(chqstate_t *state,
                                int         width_bytes,
                                u8         *backbuf_addr,
                                int         height,
                                int         bitmap_stride,
                                const u8   *bitmap_data);
static void plot_sprite_flipped_even(chqstate_t *state,
                                     int         jump_offset,
                                     const u8   *flip_table,
                                     u8         *backbuf_addr,
                                     int         height,
                                     int         bitmap_stride,
                                     const u8   *bitmap_data);
static void plot_sprite_flipped_odd(chqstate_t *state,
                                    int         width_bytes,
                                    u8         *backbuf_addr,
                                    int         height,
                                    int         bitmap_stride,
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
                                      int         x,
                                      char        character);
static void plot_mini_font_cursor_on(chqstate_t *state,
                                     int         x,
                                     char        character);
static void plot_mini_font_char(chqstate_t *state,
                                int         x,
                                char        ascii,
                                int         extrabm1,
                                int         extrabm2);

static void clear_message_line(chqstate_t *state);

static void check_time_up(chqstate_t *state);
static void play_start_noise(chqstate_t *state);

static void speed_score(chqstate_t *state);

static void add_bonus(chqstate_t *state, int A_lo, int E_md, int D_hi);
static int bonus_digit(int Adigit, int *pCzeroflag, char **pHLoutput);

static void increment_score(chqstate_t *state, int A_lo, int E_md, int D_hi);

static void calc_overtake_bonus(chqstate_t *state);

static void update_scoreboard(chqstate_t *state);

static void toggle_light_brightness(chqstate_t *state, u8 *attrs);

static void plot_turbos_and_digits(chqstate_t *state);
static void ptad_led_digits(int         iterations,
                            const u8   *digits,
                            u8         *stored,
                            u8         *screen);

static u8 *ledfont_plot(int ord, u8 *screen);

static const u8 *draw_string_with_style(chqstate_t *state,
                                        int         attrval,
                                        u8         *attrs,
                                        u8         *backbuf,
                                        const u8   *string,
                                        int         style);
static const u8 *draw_string_screen(chqstate_t *state,
                                     int        attrval,
                                     u8        *attrs,
                                     u8        *dst,
                                     const u8  *string);
static const u8 *draw_string_core(chqstate_t *state,
                                  u8         *dst,
                                  const u8   *string,
                                  int         style,
                                  int         attrval,
                                  int         attrsstride,
                                  u8         *attrs);

static void draw_char(chqstate_t *state,
                      int          character,
                      u8         *dst,
                      int          style,
                      int          attrval,
                      int          attrstride,
                      u8         *attrs,
                      u8        **new_screen,
                      u8        **new_attrs);

static u8 keyscan(chqstate_t *state);
static u8 keyscan_keydefs(chqstate_t *state, const u8 *HLkeydefs, u8 Estopbit);
static int keyscan_inner(const chqstate_t *state, int Ainput);

static void check_scenery_collisions(chqstate_t *state);
static void csc_hit_scenery(chqstate_t *state, int Aflip_flag, int Adash_speed);

static void scenery_hit(chqstate_t *state, int Aflip_flag, int Adash_threshold);

static void check_fork_scenery_collisions(chqstate_t *state, int DEdash,
    int HLdash);

static void layout_objects(chqstate_t *state);

static void cycle_counters(chqstate_t *state);

static void spawn_cars(chqstate_t *state);

static u16 get_spawn_lanes(chqstate_t *state, int extra);

static void choose_dirt_and_stones(chqstate_t *state);

static void layout_dirt_and_stones(chqstate_t *state);

static void draw_dirt_and_stones(chqstate_t *state, int Biterations,
                              const u8 *IYheight);

static void draw_helicopter(chqstate_t *state, int Bdistance, u8 *IYheight);
static void draw_helicoper_part(chqstate_t                *state,
                                int                         Acol_pos,
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
                          hazard_t *hazard);

static void advance_hazards(chqstate_t *state);
static void advance_hazard(chqstate_t *state,
                               hazard_t   *IXhazard,
                               const u8   *IYbase);
static void draw_hazard_sprites(chqstate_t *state,
                                  int          Biterations,
                                  const u8   *IYheight);
static void dhs_smoke(chqstate_t *state, u8 *HLsmoke, const u8 *IYheight);
static void dhs_draw(chqstate_t     *state,
                    int              Bx,
                    int              Cy,
                    int             DEoffset,
                    const bitmap_t *HLbitmaps,
                    const u8       *IYheight);
static void dhs_draw_bitmap(chqstate_t     *state,
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
static void draw_part_entrypt2(chqstate_t *state,
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

static u8 *plot_masked_sprite(chqstate_t *state,
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
static void plot_masked_sprite_flipped_entrypt2(chqstate_t *state,
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

static void set_playfield_attrs(chqstate_t *state);

static void read_map(chqstate_t *state);
static void rm_cycle_buffer_offset(chqstate_t *state, u8 *HLfast_counter);

static void prepare_tunnel(chqstate_t *state);

static void draw_tunnel(chqstate_t *state, u8 *IYheight);

static void draw_road_lanes_change(chqstate_t *state,
                                   int         B_fill_pattern,
                                   int         C_horizon,
                                   int         DE_backbuf,
                                   int         H_left_hand_table_hi,
                                   int         L_row,
                                   u8        **IX_lanesptr,
                                   const u8  **IY_heightptr);

static void draw_road(chqstate_t *state);
static void dr_read_lanes(chqstate_t *state, u8 *IXlanesptr, const u8 *IYheightptr,
                          int Bfill_pattern, int Ccounter, int DEbackbuf, int Lrow);
static void dr_four_lane_highway(chqstate_t *state, int Bfill_pattern,
                                 int Ccounter, int DEbackbuf, int Lrow,
                                 u8 **IXlanesptr, const u8 **IYheightptr);
static void dr_set_lane_callback(chqstate_t *state, int Bfill_pattern,
                                 int Ccounter, int DEbackbuf, int Lrow,
                                 dr_callback_t *HLdash_callback,
                                 u8 **IXlanesptr, const u8 **IYheightptr);
static void dr_dispatch(chqstate_t *state, int Bfill_pattern, int Ccounter,
                             int DEbackbuf, int Lrow,
                             u8 **IXlanesptr, const u8 **IYheightptr);
static void dr_advance_unfilled(chqstate_t *state, int Ccounter, int DEbackbuf,
                                int Lrow, int Adash_fill_pattern,
                                u8 **IXlanesptr, const u8 **IYheightptr);
static void dr_write_scanline_unfilled(chqstate_t *state, int Ccounter, int DEbackbuf,
                                       int Lrow, int Adash_fill_pattern,
                                       u8 **IXlanesptr, const u8 **IYheightptr);
static void dr_rollover_filled(chqstate_t *state, int Ccounter, int DEbackbuf,
                                   int Lrow, int Adash_fill_pattern,
                                   u8 **IXlanesptr, const u8 **IYheightptr);
static void dr_rollover_unfilled(chqstate_t *state, int Ccounter, int DEbackbuf,
                                int Lrow, int Adash_fill_pattern,
                                u8 **IXlanesptr, const u8 **IYheightptr);
static void dr_dispatch_filled(chqstate_t *state, int Afill_pattern, int Ccounter, int DEbackbuf,
                            int Lrow, u8 **IXlanesptr, const u8 **IYheightptr);
static void dr_advance_filled(chqstate_t *state, int Ccounter, int DEbackbuf, int Lrow,
                    int Adash_fill, u8 **IXlanesptr, const u8 **IYheightptr);
static void dr_fill(chqstate_t *state, int Ccounter, int DEbackbuf, int Lrow,
                    int Adash_fill_pattern, u8 **IXlanesptr, const u8 **IYheightptr);
static void dr_fill_left_stripe(chqstate_t *state,
                                u8         *SPoutput,
                                int         jump_index,
                                int         Bneg_lane_count,
                                u8          Ccounter,
                                int         DEbackbuf,
                                int         Lrow,
                                int         Adash_fill_pattern,
                                int         Cdash_zerofill,
                                int         DEdash_backbuf,
                                int         Hdash_fill,
                                u8        **IXlanesptr,
                                const u8  **IYheightptr);

static void pre_shift_backdrop(chqstate_t *state);

static void draw_forked_road(chqstate_t *state, const u8 *IXlanes, const u8 *IYheight,
                             int Bfill_pattern, int Ccounter, int DEbackbuf, int Lrow);

static void dr_start_backdrop_fill(chqstate_t *state, int DEbackbuf, int Lrow);
static void backdrop_fill_dispatch(chqstate_t *state, int DEbackbuf, int Lrow);

static void build_curve_table(chqstate_t *state, int forked);
static void build_curve_table_fill(chqstate_t *state,
                                   s16        *HLtableend,
                                   int         Bdash_alwayszero,
                                   int         DEroadpos);

static void build_height_table(chqstate_t *state);

static int8_t scale_curvature_or_height(int8_t a, int8_t c);

static void entry_48k(chqstate_t *state);
static void entry_128k(chqstate_t *state);
static void entry_common(chqstate_t *state, int Amode_128k, int Bnrelocs);

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
static u8 redefine_keyscan(chqstate_t *state, u8 *Dkeydef_out);
static int define_a_key(chqstate_t *state, int Bindex, int Cindex,
                        int DEscreen);
static u16 dak_move_down(int DEscreen);

static void setup_interrupts(chqstate_t *state);
static void reset_music(chqstate_t *state);
static void next_pattern(chqstate_t *state);
static void next_pattern_at_addr(chqstate_t *state, const u8 *HLpataddr);
static void play_music_48k(chqstate_t *state);
static void playdrum_2(chqstate_t *state, int Aspeed);
static void playdrum_1(chqstate_t *state, int Aspeed);
static void playdrum_start(chqstate_t *state, int Aspeed, int Dlength,
                           u8 *HLdata);
static void playdrum_bank_go(chqstate_t *state, int Ddash_length,
                             u8 *HLdash_data);
static void playdrum_go(chqstate_t *state, int Dlength, u8 *HLdata);
static void play_noise(chqstate_t *state, int Aparam);

static void start_siren_128k(chqstate_t *state);
static void play_siren_sfx_128k(chqstate_t *state);
static void silence_audio_128k(chqstate_t *state);
static void write_audio_registers_128k(chqstate_t *state);
static void engine_sfx_from_speed_128k(chqstate_t *state);
static void setup_turbo_sfx_128k(chqstate_t *state);
static void play_engine_or_turbo_sfx_128k(chqstate_t *state);
static void play_speech_128k(chqstate_t *state, int index);
static void handle_perp_caught_128k(chqstate_t *state);
static void page_128k(chqstate_t *state);
static void reset_paging_128k(chqstate_t *state);
static void attract_mode_128k(chqstate_t *state);

/* ----------------------------------------------------------------------- */

/**
 * $5C00: Show the end screen
 *
 * Displays the end-of-game results screen.
 *
 * Conv: Not yet implemented; the Z80 version drives a full results/credits
 * sequence. This stub returns immediately.
 *
 * \param[in] state Pointer to game state.
 */
static void show_end_screen(chqstate_t *state)
{
  // TODO: Write this.
  NOT_USED(state);
}

/**
 * $8014: Switch the active stage data to the wanted stage
 *
 * Returns immediately if the wanted stage is already loaded. Otherwise records
 * the new stage number and updates the stage pointer.
 *
 * In the Z80 version this is a full tape-loading routine: it clears the screen,
 * initiates a reverse transition, reads a header from tape to identify the
 * stage, then loads 6896 bytes of stage data to $5C00. All tape handling is
 * removed in C; stage data is pre-loaded as read-only arrays in
 * Stage1Data.c (and future stage files).
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Tape loading, header validation, screen clearing and transition setup
 * are all removed. C switches state->stage to the pre-loaded data table for the
 * requested stage.
 */
static void load_stage(chqstate_t *state)
{
  int wanted; /* wanted stage number (was A) */

  wanted = state->wanted_stage_number;
  if (wanted == state->current_stage_number)
    return;

  state->current_stage_number = wanted;
  state->stage = stages[wanted];
}

// $80B9 tape_load_to_5c00

// $81DD start_stage_chatter - was hoisted

/**
 * $8204: Derive 48K engine tone parameters from speed and gear
 *
 * Computes the iteration count (nloops) and off/on-phase delay counts for the
 * 48K border-port engine sound. The speed is halved and complemented to give an
 * inverse-speed divisor, right-shifted by 2 and OR'd with 1 to keep it odd and
 * non-zero. In high gear nloops is halved again (higher speed → more loops →
 * higher pitch). In a tunnel, the off-phase delay is reduced from 3 to 1. The
 * computed values are stored in SM fields and play_engine_sfx_48k is called to
 * emit a tone pulse.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 stores nloops and delays via self-modifying LD C,n / LD B,n
 * instructions inside play_engine_sfx_48k at $8243/$8249/$8251; C stores to the
 * SM fields engine_sfx_nloops, engine_sfx_off_cycle and engine_sfx_on_cycle in
 * chqstate.
 */
static void setup_engine_sfx_48k(chqstate_t *state)
{
  int nloops;    /* tone pulse iteration count (was L) */
  int off_cycle; /* off-phase delay loop count (was H) */

  /* Conv: mask to 8 bits — the Z80 CPL and SRLs at $820B-$820E operate on
   * A. Without the mask C's ~ yields a negative int and nloops | 1 stays
   * negative, collapsing every engine burst to a single pulse. */
  nloops    = (((~(state->speed >> 1)) & 0xFF) >> 2) | 1;
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
 * $8234: Emit one 48K border-port engine tone burst
 *
 * Runs only on every 4th call (the counter skips three out of four
 * invocations). Suppressed when the perp-caught phase has the car stopped.
 * Toggles port $FE between 0 and $18 (EAR+MIC) nloops times, with off-phase and
 * on-phase delay loops to tune the pitch. The Z80 inner loop body is: OUT
 * ($FE),0; B DJNZ loops; OUT ($FE),$18; B DJNZ loops; DEC C; JR NZ.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 uses OUT ($FE) to drive the border/speaker port and idle DJNZ
 * busy-loops to set the duty cycle. C issues the OUTs via speccy->out and
 * models the busy-loops as speccy->logtime so the host can reconstruct the
 * pulse timing.
 */
static void play_engine_sfx_48k(chqstate_t *state)
{
  int phase;     /* perp-caught phase; suppresses effect when car is stopped (was A) */
  int counter;   /* per-call skip counter; fires every 4th call (was A) */
  int off_cycle; /* off-phase delay loop count; 0 means 256 as DJNZ (was B) */
  int on_cycle;  /* on-phase delay loop count; 0 means 256 as DJNZ (was B) */
  int nloops;    /* tone pulse iteration count (was C) */

  phase = state->perp_caught_phase;
  if (phase >= PERPCAUGHTPHASE_STOPPED)
    return;

  counter = (state->engine_sfx_counter + 1) & 3;
  state->engine_sfx_counter = counter;
  if (counter)
    return;

  /* Conv: DJNZ with B = 0 loops 256 times */
  off_cycle = state->engine_sfx_off_cycle ? state->engine_sfx_off_cycle : 256;
  on_cycle  = state->engine_sfx_on_cycle  ? state->engine_sfx_on_cycle  : 256;

  nloops = state->engine_sfx_nloops;
  do {
    state->speccy->out(state->speccy, port_BORDER_EAR_MIC, 0);
    /* off-phase delay (7 + loop + 7 T-states) */
    state->speccy->logtime(state->speccy,
                           14 + DJNZ_LOOP_TSTATES(off_cycle));
    state->speccy->out(state->speccy, port_BORDER_EAR_MIC,
                       port_MASK_EAR | port_MASK_MIC);
    /* on-phase delay (7 + loop + 4 + 12 + 4 T-states) */
    state->speccy->logtime(state->speccy,
                           27 + DJNZ_LOOP_TSTATES(on_cycle));
  } while (--nloops > 0);
}

/**
 * $8258: Run the 48K attract mode demo loop
 *
 * Sets up the attract stage and drives the game in demonstration mode. Each
 * frame: scans for the fire button (returns immediately if pressed), runs a
 * game tick, then draws the "CHASE HQ / PRESS GEAR TO PLAY" messages. The
 * second message blinks by rotating a pattern through RRCA each frame. When the
 * transition is stopped, alternates between showing the credits and copyright
 * messages via setup_overlay_messages.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 uses two self-modifying LD A,n operands: one at $8277 for the RRCA
 * blink pattern (C field: state->attract_blinker) and one at $828C for the
 * credits/copyright toggle (C local: blinker). The speed initialisation (LD
 * HL,$0190; LD ($A24A),HL) is reproduced as state->speed =
 * INITIAL_ATTRACT_SPEED.
 */
static void attract_mode_48k(chqstate_t *state)
{
  int          carry;           /* carry flag used by RRC (carry) */
  int          blinker;         /* credits/copyright toggle; SM at $828C (was $828C) */
  int          keys;            /* keyscan result (was A) */
  const u8    *messages;        /* pointer to current message record (was HL) */
  int          nmessages;       /* number of messages to draw (was B) */
  u8           attract_blinker; /* rotating blink pattern for the second message (was A) */
  int          style;           /* message style byte read from record (was A) */
  carry = 0;

  set_up_stage(state, &state->stage->attract_data);
  blinker = 0;
  state->speed = INITIAL_ATTRACT_SPEED;
  for (;;) {
    if (state->host_quit)
      longjmp(state->host_quit_jmp, 1);

    keys = keyscan(state);
    if (keys == USERINPUTFLAG_FIRE)
      return;

    drive_attract_demo(state);

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

    if (state->transition_control == TRANSITIONCONTROL_STOP) {
      // Alternate between credits and copyright messages
      blinker ^= 1;
      messages = &credits_messages[0];
      if (blinker)
        messages = &copyright_messages[0];
      setup_overlay_messages(state, messages);
    }

    transition(state);
    update_screen(state);
  }
}

/**
 * $83B5: Dispatch siren start to the 128K or 48K audio driver
 *
 * In the Z80, each hook address holds a single JP instruction. On 128K
 * hardware, bank 3 is paged in so the jump target contains the 128K audio code.
 * On 48K hardware, the target is a NOP stub at $8A56. C replaces the paged jump
 * with an explicit mode check throughout all eight hooks.
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
 * $83B8: Dispatch engine or siren SFX to the 128K or 48K audio driver
 *
 * See start_siren_hook for the paged-jump dispatch pattern common to all hooks.
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
 * $83BB: Silence audio via the 128K driver; no-op on 48K
 *
 * See start_siren_hook for the paged-jump dispatch pattern.
 *
 * \param[in] state Pointer to game state.
 */
static void silence_audio_hook(chqstate_t *state)
{
  if (state->mode_128k)
    silence_audio_128k(state);
}

/**
 * $83BE: Flush audio register writes via the 128K driver; no-op on 48K
 *
 * See start_siren_hook for the paged-jump dispatch pattern.
 *
 * \param[in] state Pointer to game state.
 */
static void write_audio_registers_hook(chqstate_t *state)
{
  if (state->mode_128k)
    write_audio_registers_128k(state);
}

/**
 * $83C1: Configure the engine SFX via the 128K driver; no-op on 48K
 *
 * See start_siren_hook for the paged-jump dispatch pattern.
 *
 * \param[in] state Pointer to game state.
 */
static void setup_engine_sfx_hook(chqstate_t *state)
{
  if (state->mode_128k)
    setup_turbo_sfx_128k(state);
}

/**
 * $83C4: Drive engine SFX via the 128K or 48K audio driver
 *
 * See start_siren_hook for the paged-jump dispatch pattern.
 *
 * \param[in] state Pointer to game state.
 */
static void play_engine_sfx_hook(chqstate_t *state)
{
  if (state->mode_128k)
    play_engine_or_turbo_sfx_128k(state);
  else
    setup_engine_sfx_48k(state);
}

/**
 * $83C7: Play a speech sample via the 128K driver; no-op on 48K
 *
 * See start_siren_hook for the paged-jump dispatch pattern.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Asample Index of the sample to play. (was A)
 */
static void play_speech_hook(chqstate_t *state, int Asample)
{
  if (state->mode_128k)
    play_speech_128k(state, Asample);
}

/**
 * $83CA: Dispatch attract mode to the 128K or 48K driver
 *
 * See start_siren_hook for the paged-jump dispatch pattern.
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
 * $83CD: Build the flip table, then loop through attract → game → bank 3
 *
 * Builds a 256-entry byte bit-reversal lookup table at state->flipped: for each
 * index I, flipped[I] is I with its bits in reverse order. The table is
 * computed by iterating over all 256 byte values and rotating each bit out of A
 * (via RLCA) into C (via RR C) eight times.
 *
 * After the table is ready the function enters an infinite outer loop: call
 * attract_mode_hook (returns when the player hits fire), reset game state
 * (overtake bonus, score, stage number, credits), call main_loop, and
 * optionally call the 128K bank 3 bootstrap routine.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 loops back via JR $83CD; C uses for(;;). The five-byte clear (DJNZ
 * loop) is replaced with memset.
 */
static void bootstrap(chqstate_t *state)
{
  int  carry;       /* carry flag used by RLC/RR (carry) */
  u8   Cresult;     /* bit-reversed result accumulator (was C) */
  u8  *HLflipped;   /* pointer walking the 256-byte flip table (was HL) */
  int  Biterations; /* inner loop iteration count, 8 bits per byte (was B) */
  u8   Aindex;      /* current table index; bit source for RLC (was A) */

  // Bootstrap is itself a loop
  for (;;) {
    if (state->host_quit)
      longjmp(state->host_quit_jmp, 1);

    /* Build a table of flipped bytes at "$EF00" */
    // It's unclear why this is part of the overall game loop when it's constant.
    carry = 0;
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

    // When attract mode yields then we set up the game.
    state->overtake_bonus_bcd = 0;

    // Clear score_bcd and retry_count.
    memset(&state->score_bcd[0], 0, sizeof(state->score_bcd));
    state->retry_count = 0;

    // Reset wanted_stage_number and credits.
    state->wanted_stage_number = MINSTAGE;
    state->credits = 2;

    // Run the main game loop.
    main_loop(state);

    // If in 128K mode, call the 128K/bank 3 high score routine.
    if (state->mode_128k)
      call_bank_3_128k(state, BANK3_HI_SCORE);
  }
}

/**
 * $8401: Per-stage game loop
 *
 * Drives all stages of the game in sequence. Each iteration loads the wanted
 * stage, runs the pregame radio screen, sets up the stage and then frames
 * through the game until the perp is caught or the player quits. When stage 6
 * is requested, the end screen runs and control returns to bootstrap. Called
 * "main loop" in the skool; it is really a subroutine of bootstrap.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 uses POP / JP to restart the frame loop on stage transition; C uses
 * nested for(;;) loops and break. The quit path at $8A57 in the Z80 calls
 * escape_scene then JP $8401; in C escape_scene returns to main_loop which then
 * returns to bootstrap. Test-mode shortcuts (keys 1–3) are a C addition; the
 * Z80 has no equivalent.
 */
static void main_loop(chqstate_t *state)
{
  int  carry;               /* carry flag used by SRL (carry) */
  int  start_speech_index;  /* index into the 3-entry speech cycle (was A) */
  int  start_speech;        /* speech sample index to play (was A) */
  u8   keys;                /* keyboard state in test mode (was A) */
  u8  *pstart_speech;       /* pointer to start_speech field (was HL) */
  int  quit_state;          /* current quit-state value (was A) */
  carry = 0;

  for (;;) {
    if (state->host_quit)
      longjmp(state->host_quit_jmp, 1);

    load_stage(state);

    if (state->wanted_stage_number == MAXSTAGE + 1) {
      show_end_screen(state);
      state->wanted_stage_number = MINSTAGE;
      load_stage(state);
      state->wanted_stage_number = MAXSTAGE + 1; // not sure why
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
      if (state->host_quit)
        longjmp(state->host_quit_jmp, 1);

      state->speccy->stamp(state->speccy);
      drive_sfx(state);
      (void) keyscan(state);
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
      advance_hazards(state);
      layout_dirt_and_stones(state);
      play_regular_sfx_hook(state);
      move_helicopter(state);
      check_scenery_collisions(state);
      play_regular_sfx_hook(state);
      draw_scene_objects(state);
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
      state->speccy->sleep(state->speccy, MAIN_LOOP_TSTATES);

      if (state->test_mode) {
        keys = ~state->speccy->in(state->speccy, port_KEYBOARD_12345) & 0x1F;
        if (keys) {
          start_sfx(state, EFFECT_BIP, 4); /* priority 4 */
          silence_audio_hook(state);

          /* RR keys x3, testing carry each time: each carry_out depends only
           * on the tested bit of the original byte, not on carry-in. */
          if (keys & 0x01) // Is bit 0 set? (key 1 to restart the level)
            break;

          if (keys & 0x02) { // Is bit 1 set? (key 2 to load the next level)
            state->wanted_stage_number++;
            break;
          }

          if (keys & 0x04) { // Is bit 2 set? (key 3 to load the end screen)
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
              escape_scene(state); /* tail call */
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
 * $852A: Drive the attract mode demo with automatic input
 *
 * Generates simulated user input from the current road position: steers left
 * when the road is biased rightward (roadpos < ROAD_LEFTMOST), right when
 * biased leftward (roadpos >= ROAD_RIGHTMOST), otherwise straight. Adds a
 * gear-change flag whenever the actual gear does not match the speed-derived
 * target gear. Then executes a reduced frame tick (read_map → animate_hero_car)
 * without scoring, hazards or overlay logic.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: The host_quit longjmp check is a C addition (the Z80 has no clean-exit
 * mechanism). The CHECK assert macros are also C-only debug guards. The sleep()
 * at the end is a timing approximation.
 */
static void drive_attract_demo(chqstate_t *state)
{
  int roadpos; /* current lateral road position (was HL) */
  int input;   /* computed user-input flags (was A) */

  if (state->host_quit)
    longjmp(state->host_quit_jmp, 1);

  roadpos = state->scenedata.road_pos;
  input = USERINPUTFLAG_UP | USERINPUTFLAG_RIGHT;
  if (roadpos < ROAD_LEFTMOST) {
    input = USERINPUTFLAG_UP | USERINPUTFLAG_LEFT;
    if (roadpos >= ROAD_RIGHTMOST)
      input = USERINPUTFLAG_UP;
  }

  if (state->gear != (state->speed < SPEED_GEAR_CHANGE))
    input |= USERINPUTFLAG_FIRE;

  state->user_input = input;

  state->speccy->stamp(state->speccy);

  read_map(state);
  spawn_cars(state);
  cycle_counters(state);
  build_height_table(state);
  scroll_horizon(state);
  layout_road(state);
  draw_road(state);
  layout_objects(state);
  prepare_tunnel(state);
  spawn_hazards(state);
  choose_dirt_and_stones(state);
  layout_dirt_and_stones(state);
  advance_hazards(state);
  move_hero_car(state);
  check_scenery_collisions(state);
  draw_scene_objects(state);
  animate_hero_car(state); /* tail call */

  state->speccy->sleep(state->speccy, ATTRACT_TSTATES);
}

/**
 * $858C: Initialise the pre-game "CHASE HQ MONITORING SYSTEM" screen
 *
 * Sets up the stage data, starts a reverse transition, clears the playfield and
 * initiates the chatter sequence that describes the current stage's target. The
 * perp car reveal counter is reset to zero so that reveal_perp_car gradually
 * uncovers the car each frame.
 *
 * The pregame frame loop was extracted into run_pregame_screen_loop so the
 * caller can drive it from main_loop.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: dont_draw_screen_attrs is set to 1; the Z80 used 0xF8 (non-zero but
 * with palette bits set). Pregame loop extracted (run_pregame_screen_loop).
 * Dead code at the end of the Z80 routine removed.
 */
static void run_pregame_screen(chqstate_t *state)
{
  set_up_stage(state, &state->stage->stage_data);

  state->dont_draw_screen_attrs = 1; // Conv: Was 0xF8.
  setup_transition(state, TRANSITIONSTRIDE_REVERSE);
  set_playfield_attrs(state);
  // Reset the counter in #R$85E4 that reveals the perp's car
  state->pregame_car_revealed_height = 0;
  start_chatter(state, 0xFF, state->stage->addrof_perp_description);

  // Conv: Pregame loop extracted (below)

  // Conv: Dead code removed
}

/**
 * $85A8: Execute one frame of the pre-game screen
 *
 * Called repeatedly from main_loop until it returns zero. Each frame: draws the
 * pregame scene (perp portrait, speed/distance meters, chatter text), advances
 * the perp car reveal animation, drives the transition and flushes the screen.
 * Returns zero once the transition has completed and the chatter sequence is
 * idle, or immediately if the player presses fire to skip the intro.
 *
 * \param[in] state Pointer to game state.
 *
 * \return 1 to continue looping; 0 when the pregame screen is complete.
 *
 * Conv: In the Z80 this is the tail of run_pregame_screen ($858C); C splits it
 * into a separate function so main_loop can control the iteration. The sleep()
 * call is a frame-timing approximation.
 */
static int run_pregame_screen_loop(chqstate_t *state)
{
  int rc; /* loop/stop flag: 1 = continue, 0 = done */
  rc = 1;

  if (state->host_quit)
    longjmp(state->host_quit_jmp, 1);

  state->speccy->stamp(state->speccy);

  draw_pregame(state);
  drive_chatter(state);
  reveal_perp_car(state);
  animate_meters(state);
  transition(state);
  update_screen(state);
  if (state->transition_control == 0) {
    if (state->chatter_state == CHATTERSTATE_IDLE) {
      rc = 0; // stop
      goto exit;
    }
    if (state->chatter_state < CHATTERSTATE_STOP) {
      if (keyscan(state) & USERINPUTFLAG_FIRE) {
        drive_chatter_stop(state);
        play_start_noise(state); /* tail call */
        rc = 0; // stop
        goto exit;
      }
    } else {
      setup_transition(state, TRANSITIONSTRIDE_FORWARD);
    }
  }

exit:
  state->speccy->sleep(state->speccy, PREGAME_TSTATES);
  return rc; // loop
}

/**
 * $85E4: Reveal the perpetrator's car progressively from the bottom up
 *
 * Each call increments the reveal height counter by 1, capping at 50. The
 * perp's car sprite is then plotted using that height as a clip limit (so fewer
 * rows show until the counter reaches the sprite's full height). On stage 5 the
 * car is suppressed entirely.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 stores the reveal height in a self-modifying LD A,n operand at
 * $85EB; C uses state->pregame_car_revealed_height. Z80 banks parameters via
 * EXX before calling plot_sprite and uses JP (tail call); C passes parameters
 * directly and calls plot_sprite.
 */
static void reveal_perp_car(chqstate_t *state)
{
  const int MaxHeight = 50;

  int             revealed_height; /* was A */
  const bitmap_t *perp_bitmap;     /* was HL */
  int             width_bytes;     /* was DE */
  int             height;          /* was B (banked?) */
  const u8       *bitmap;          /* was HL (banked?) */

  if (state->wanted_stage_number == 5)
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
              bitmap); /* tail call */
}

/**
 * $860F: Animate the two signal-strength meters on the pregame screen
 *
 * Calls rng twice — once per meter — and uses the sign of the result to nudge
 * the meter level up (positive) or down (negative), clamping to 0–7. Each level
 * is rendered by am_set_attrs as a row of up to seven coloured attribute cells
 * (green = signal, red = noise).
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 stores each meter level in a self-modifying LD A,n operand ($8614
 * and $8631); C uses state->meter_1_level and meter_2_level.
 */
static void animate_meters(chqstate_t *state)
{
  static const zxbox_t meters_box = { /* speed meter columns 23–29, attr rows 16 and 18 */
    184, 40, 240, 64
  };
  int random; /* signed RNG result; sign determines direction (was A) */
  int level;  /* current meter level, 0–7 (was A) */

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
  state->speccy->draw(state->speccy, &meters_box); /* Conv: added */
}

/**
 * $8646: Fill a 7-cell attribute bar with green then red segments
 *
 * Writes [counter] green (bright-black-on-green) attribute bytes then
 * `7 − [counter]` red (bright-black-on-red) bytes into consecutive screen
 * attribute cells. Used to draw each signal-strength meter bar on the
 * pregame screen.
 *
 * \param[in] counter Number of green (lit) segments, 0–7. (was A)
 * \param[in] attrs Pointer to the first attribute cell of the bar. (was HL)
 *
 * Conv: Z80 computes `7 − [counter]` via CPL+ADD A,8 (two's-complement trick
 * on the low three bits); C uses `7 - counter` directly.
 */
static void am_set_attrs(int counter, u8 *attrs)
{
  int iterations; /* DJNZ loop counter (was B) */

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
 * Interprets a compact command stream in pregame_data[] to paint the pre-game
 * title screen into the back buffer. Commands select a draw direction
 * (horizontal/vertical), set a background attribute colour, set the current
 * write address, or plot one or more 8x1-row tiles from pregame_tiles[]. After
 * the STOP command, prints four overlay message strings via print_message.
 *
 * \param[in,out] state Pointer to game state.
 */
static void draw_pregame(chqstate_t *state)
{
  static const zxbox_t playfield_box = { /* lower two-thirds of screen */
    0, 0, SCREEN_WIDTH, PLAYFIELD_HEIGHT
  };

  int       carry;      /* carry from attribute-address shift computation (carry) */
  const u8 *cmds;       /* pointer walking pregame_data[] command stream (was HL) */
  int       cmd;        /* current command byte from the stream (was A) */
  u16       cmdaddr;    /* current Z80 back-buffer write address from SET_ADDR command (was DE) */
  int       tileidx;    /* tile index into pregame_tiles[] (was A) */
  const u8 *srctile;    /* pointer to the current tile pixel data in pregame_tiles[] (was DE) */
  u8       *backbuf;    /* back-buffer pointer derived from cmdaddr (was HL) */
  int       tile_count; /* number of tiles to repeat from a REPEAT command (was B) */
  int       iterations; /* 8: row counter for one tile; 4: message loop counter (was B) */
  u16       bufoffset;  /* BACKBUF offset after tile draw; encodes column + row field (was BC) */
  int       E;          /* column portion of attribute address: (offset & 0x1F) | row-bit (was E) */
  u8        rows;       /* row field from bufoffset, shifted left for attribute address (was ?) */
  int       bgattr;     /* draw_pregame_background: OR'd into attribute cell if non-zero (was A) */
  const u8 *messages;   /* pointer walking pregame_messages[] for print_message calls (was HL) */
  u16       attrs;      /* computed attribute address for the tile just drawn (was DE) */

  carry = 0;

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
        *ADDRTOATTRS(attrs) = bgattr;

      // dp_direction
      if (state->draw_pregame_direction != 1) {
        bufoffset = BACKBUFTOOFFSET_LR(backbuf, 0, 256);
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
  state->speccy->draw(state->speccy, &playfield_box); /* Conv: added */
}

/**
 * $873C: Run the "game over" escape scene
 *
 * Drives the time-limit expiry sequence: silences audio, sets up the escape
 * scene (the perp's car escaping into a tunnel), plays the "game over" chatter,
 * then loops the road/hazard/object pipeline until the perp car clears the
 * tunnel and the chatter finishes. At that point three barriers are activated
 * as the final obstacle.
 *
 * Called from main_loop when escape_scene_requested is set.
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
    if (state->host_quit)
      longjmp(state->host_quit_jmp, 1);

    // Print "GAME OVER" once the transition has completed.
    if (state->transition_control != TRANSITIONCONTROL_FADE)
      setup_overlay_messages(state, &game_over_message[0]);

    state->speccy->stamp(state->speccy);
    read_map(state);
    build_height_table(state);
    scroll_horizon(state);
    layout_road(state);
    draw_road(state);
    layout_objects(state);
    prepare_tunnel(state);
    spawn_hazards(state);
    advance_hazards(state);
    draw_scene_objects(state);
    update_scoreboard(state);
    drive_chatter(state);
    transition(state);
    update_screen(state);
    state->speccy->sleep(state->speccy, ESCAPE_SCENE_TSTATES);

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
 * $87DC: Initialise stage state from a scene data block
 *
 * Resets the road buffer, copies the saved game session back into the active
 * state, clears all hazard slots except slot 0, copies the supplied scene data
 * into state->scenedata, pre-shifts the backdrop image, resets the horizon
 * attribute table, disables helicopter and tunnel draw calls, primes the map
 * reader by cycling it 32 times, then sets up a reverse transition, clears the
 * playfield, resets the marquee lights and silences audio.
 *
 * \param[in] state Pointer to game state.
 * \param[in] scene_data Scene data to load (attract, stage or escape). (was HL)
 *
 * Conv: Z80 uses PUSH HL before zeroing the road buffer (to preserve the data
 * pointer) and POP HL after; C has no equivalent need. LDIR zero fills are
 * replaced with memset. Struct assignment replaces field-by- field LDIR copies.
 */
static void set_up_stage(chqstate_t        *state,
                         const scenedata_t *scene_data)
{
  int iterations; /* map reader prime loop count (was B) */

  state->roadbufptr = &state->road_buffer[0];
  memset(&state->road_buffer[0], 0, 256);

  state->session = saved_game_state;
  state->hazards[0] = saved_game_state_perp_hazard;

  memset(&state->hazards[1], 0, sizeof(hazard_t) * (MAXHAZARDS - 1));

  state->ay_regs.chan_a_pitch = 0;
  state->ay_regs.chan_b_pitch = 0;
  state->ay_regs.chan_c_pitch = 0;
  state->ay_regs.noise_pitch = 0;
  state->ay_regs.mixer = 0;
  state->ay_regs.chan_a_vol = 0;
  state->ay_regs.chan_b_vol = 0;
  state->ay_regs.chan_c_vol = 0;
  state->ay_regs.env_fine = 0;
  state->dont_draw_screen_attrs = 0;
  state->inhibit_collision_detection = 0;
  state->n_hazards = 0;
  state->displayed_stage = 0;
  state->helicopter_control = 0;
  state->dont_spawn_cars = 0;
  state->correct_fork = 0;
  state->floating_arrow = 0;
  state->cherry_light = 0;
  state->time_up_state = 0;
  state->car_y = 0;
  state->overtake_bonus_counter = 0;
  state->trigger_bonus_flag = 0;
  state->bonus_counter = 0;
  state->sighted_flag = 0;
  state->hand_flag = 0;
  state->perp_caught_phase = 0;
  state->transition_control = 0;
  state->smash_level = 0;
  state->smash_counter = 0;
  state->anim_counter = 0;
  state->frame_toggle = 0;
  state->slow_anim_counter = 0;
  state->sfx_index = 0;
  state->sfx_priority = 0;
  state->siren_enabled = 0;
  state->turbo_sfx_pitch = 0;
  state->tunnel_sfx = 0;
  state->trigger_righthand_sfx = 0;
  state->trigger_lefthand_sfx = 0;
  state->off_road = 0;
  state->fast_counter = 0;
  state->roadbufptr = &state->road_buffer[0];   // $EE00
  state->curvature_byte = 0;
  state->height_byte = 0;
  state->leftside_byte = 0;
  state->rightside_byte = 0;
  state->hazards_counter = 0;
  state->lanes_counter = 0;
  state->on_dirt_track = 0;
  state->fork_taken = 0;
  state->speed = 0;
  state->inclined_counter = 0;
  state->cornering = 0;
  state->boost = 0;
  state->smoke = 0;
  state->turn_speed = 0;
  state->flip_car = 0;
  state->gear_lockout = 0;
  state->gear = 0;
  state->allow_spawning = 0;
  state->distance_bcd[0] = 0;
  state->distance_bcd[1] = 0;
  state->incline = 0;
  state->prev_road_height = 0;
  state->horizon_y_accum = 0;
  state->horizon_y_step = 0;
  state->current_curvature = 0;
  state->horizon_curve_index = 0;
  state->horizon_x_scroll = 0;
  state->horizontal_adjust = 0;
  state->horizon_scroll_sub = 0;
  state->curvature_ticks = 0;
  state->right_turn = 0;
  state->left_turn = 0;
  state->fork_visible = 0;
  state->fork_countdown = 0;
  state->fork_distance = 0;
  state->fork_in_progress = 0;
  state->quit_state = 0;
  state->start_speech = 0;

  state->scenedata = *scene_data;

  pre_shift_backdrop(state);

  // Set backdrop position in horizon table (used to draw attributes)
  state->horizon_attr[0] = 8;
  state->horizon_attr[1] = state->horizon_attr[2] = 0;

  // Disable the helicopter and tunnel drawing calls in draw_scene_objects
  state->dee_draw_tunnel_1 = 0;
  state->dee_draw_helicopter = 0;
  state->dee_draw_tunnel_2 = 0;

  state->rm_hazard_byte = 0; // clear current hazard command
  state->mhc_y_offset = 0; // reset car jump counter

  state->hazards[0].hittable.bitmaps = state->stage->bitmaps_perp_car;

  // Conv: Duplicate work removed.

  // Run the map reader 32 times
  iterations = 32;
  do
    rm_cycle_buffer_offset(state, &state->fast_counter);
  while (--iterations > 0);

  // Disallow spawning
  state->allow_spawning = 0;

  setup_transition(state, TRANSITIONSTRIDE_REVERSE);

  set_playfield_attrs(state);

  // Clear the lights' BRIGHT bit
  set_up_stage_reset_lights(ADDRTOATTRS(MARQUEELIGHT_LEFT_ATTR_ADDR));
  set_up_stage_reset_lights(ADDRTOATTRS(MARQUEELIGHT_RIGHT_ATTR_ADDR));

  silence_audio_hook(state);
  update_scoreboard(state); /* tail call */
}

/**
 * $8860: Clear the BRIGHT bit from a marquee light attribute block
 *
 * Walks a MARQUEELIGHT_HEIGHT × MARQUEELIGHT_WIDTH block of screen attribute
 * bytes, masking out the ATTR_BRIGHT bit in each cell to return the lights to
 * their dim state. Called twice by set_up_stage (once for each side light).
 *
 * \param[in,out] attrptr Pointer to the top-left attribute cell of the
 * light block. (was HL)
 */
static void set_up_stage_reset_lights(u8 *attrptr)
{
  int rows; /* row counter, MARQUEELIGHT_HEIGHT down to 1 (was C) */
  int cols; /* column counter, MARQUEELIGHT_WIDTH down to 1 (was B) */

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
 * $8876: Filter user input and dispatch to the active-button handler
 *
 * Suppresses all user input during a FADE transition. Otherwise masks
 * user_input with the stage's input-mask to yield the effective input. If any
 * of the quit, pause or boost bits are set, dispatches to the appropriate
 * handler: quit → check_user_input_quit_key; pause → spin until the button is
 * released then wait for any key then debounce; boost → arm a 60-tick boost and
 * start turbo chatter.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 tests for TRANSITIONCONTROL_FADE with CP $04; C uses the named
 * constant. The quit-key path at $88A9 is a separate function in C
 * (check_user_input_quit_key) rather than a fall-through at $88A9.
 */
static void check_user_input(chqstate_t *state)
{
  int  transctl;   /* transition control value; FADE suppresses input (was A) */
  u8  *puserinput; /* pointer to user_input field (was HL) */
  int  input;      /* masked user input flags (was A) */
  u8  *pboost;     /* pointer to boost timer field (was HL) */
  int  keys;       /* keyscan result during pause/debounce loop (was A) */

  transctl = state->transition_control;
  puserinput = &state->user_input;
  if (transctl == TRANSITIONCONTROL_FADE) {
    *puserinput = USERINPUTFLAGMASK_NONE;
    return;
  }

  *puserinput = input = (state->session.user_input_mask & *puserinput);
  if ((input & (USERINPUTFLAG_QUIT | USERINPUTFLAG_PAUSE | USERINPUTFLAG_BOOST)) == 0)
    return;

  if (input & USERINPUTFLAG_QUIT) {
    check_user_input_quit_key(state);
    return;
  }

  if ((input & USERINPUTFLAG_PAUSE) == 0) {
    // (If it's not pause it's...) Turbo pressed
    pboost = &state->boost;
    if (*pboost > 0 || state->session.turbos == 0)
      return; // already boosting or no turbos remain

    *pboost = 60; // set 60 ticks of boost

    start_chatter(state, 2, &chatterblk_turbo[0]);
    setup_engine_sfx_hook(state); /* tail call */
  } else {
    // Conv: check_user_input_quit_key hoisted out from here.

    silence_audio_hook(state);
    do
      keys = keyscan(state);
    while (keys & USERINPUTFLAG_PAUSE);
    do
      keys = keyscan(state);
    while ((keys & USERINPUTFLAGMASK_NOT_QUIT) == 0);
    do
      keys = keyscan(state);
    while ((keys & USERINPUTFLAGMASK_NOT_QUIT) != 0);
  }
}

/**
 * $88A9: Initiate the quit sequence when the quit key is pressed
 *
 * Ignores the request if a quit is already in progress. Otherwise stops
 * chatter, fills the attribute file (to blank the screen) and arms the quit
 * state machine so that escape_scene runs on the next transition.
 *
 * \param[in] state Pointer to game state.
 */
static void check_user_input_quit_key(chqstate_t *state)
{
  if (state->quit_state != QUITSTATE_IDLE)
    return;

  drive_chatter_stop(state);
  fill_attributes(state);

  state->session.user_input_mask = USERINPUTFLAGMASK_ALLOW_NONE;
  state->quit_state = QUITSTATE_START;
}

/**
 * $88D5: Zero all attribute bytes in the playfield area
 *
 * Sets the 512 attribute bytes covering the lower 16 character rows (the
 * playfield) to black-on-black. The Z80 primes HL = $5900, writes zero to (HL),
 * then LDIRs 511 bytes from $5900 → $5901, producing a rolling zero fill across
 * $5900–$58FF.
 *
 * \param[in] state Pointer to game state.
 */
static void clear_playfield_attrs(chqstate_t *state)
{
  static const zxbox_t playfield_box = { /* lower two-thirds of screen */
    0, 0, SCREEN_WIDTH, PLAYFIELD_HEIGHT
  };

  memset(ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR),
         attribute_BLACK_OVER_BLACK,
         SCREEN_ATTRIBUTES_ROWBYTES * PLAYFIELD_HEIGHT / 8);
  state->speccy->draw(state->speccy, &playfield_box); /* Conv: added */
}

/**
 * $88E2: Zero all attribute and bitmap bytes in the playfield area
 *
 * Calls clear_playfield_attrs to zero the 512 attribute bytes, then zeros the
 * 4096 bitmap bytes covering the lower 16 character rows. The Z80 uses an LDIR
 * rolling-zero fill for each block.
 *
 * \param[in] state Pointer to game state.
 */
static void clear_playfield(chqstate_t *state)
{
  static const zxbox_t playfield_box = { /* lower two-thirds of screen */
    0, 0, SCREEN_WIDTH, PLAYFIELD_HEIGHT
  };

  clear_playfield_attrs(state);
  memset(ADDRTOSCREEN(SCREEN_PLAYFIELD_BITMAP_ADDR),
         ________,
         SCREEN_BITMAP_ROWBYTES * PLAYFIELD_HEIGHT);
  state->speccy->draw(state->speccy, &playfield_box); /* Conv: added */
}

/**
 * $88F2: Request a sound effect, replacing the current one if priority allows
 *
 * Stores [index] and [priority] only when no effect is active (curr == 0) or
 * when the incoming [priority] is at least as high as the current one (lower
 * numeric value = higher precedence; 1 is highest). The Z80 tests for zero
 * first (JR Z to assign), then CP C / RET C to bail when the current effect
 * outranks the request.
 *
 * \param[in] state Pointer to game state.
 * \param[in] index Sound effect index 1–9, indexing the table at $893C. (was B)
 * \param[in] priority Priority; lower value = higher precedence. (was C)
 */
static void start_sfx(chqstate_t *state, int index, int priority)
{
  int A_curr; /* current sfx priority; 0 means no active effect (was A) */

  A_curr = state->sfx_priority;
  if (A_curr == 0 || A_curr >= priority) {
    state->sfx_index    = index;
    state->sfx_priority = priority;
  }
}

/**
 * $8903: Drive the SFX state machine
 *
 * Each frame: if not in a tunnel, ORs left and right cornering triggers and
 * queues the cornering SFX if the result is non-zero. Then invokes the engine,
 * siren and register-write hooks. If sfx_index is non-zero, looks up the SFX
 * entry in the 9-entry table ($893C), clears the index and priority, and calls
 * the handler with the entry's two parameters.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 uses RLCA+RLCA to multiply sfx_index by 4 for a 4-byte stride
 * table; C uses sfx_index−1 as a direct array index into a struct array and
 * calls the handler via a function pointer.
 */
static void drive_sfx(chqstate_t *state)
{
  /* $893C — four-byte stride: arg1, arg2, hi(handler), lo(handler) */
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

  const struct sfxtab *sfx; /* pointer to the active SFX table entry (was HL) */

  if (state->tunnel_sfx == 0) {
    state->trigger_lefthand_sfx |= state->trigger_righthand_sfx;
    if (state->trigger_lefthand_sfx)
      start_sfx(state, EFFECT_CORNERING, 4); /* priority 4 */
  }

  play_engine_sfx_hook(state);
  play_regular_sfx_hook(state);
  write_audio_registers_hook(state);

  if (state->sfx_index == 0)
    return;

  sfx = &sfx_table[state->sfx_index - 1];

  state->sfx_index    = 0;
  state->sfx_priority = 0;

  sfx->handler(state, sfx->arg1, sfx->arg2);
}

/**
 * $8960: Play the 48K crash sound effect
 *
 * Cycles through a 93-byte waveform table (sfx_crash_table) in state. For each
 * byte, runs an inner loop of [param1] iterations: if the byte's top bit is set
 * the EAR output bit is set, otherwise it is cleared. Each iteration rotates
 * the byte left in place via RLC so the next iteration uses the next bit. The
 * table is modified in place, so successive calls produce a different waveform.
 *
 * \param[in] state Pointer to game state.
 * \param[in] param1 Inner loop count; controls pulse width. (was D)
 * \param[in] param2 Unused. (was E)
 *
 * Conv: Z80 drives the border port via OUT ($FE); C issues the equivalent
 * write via speccy->out and models the delay code as speccy->logtime so the
 * host can reconstruct the pulse timing. RLC (HL) modifies the table in
 * place, matching the Z80's in-RAM table at $897C.
 */
static void sfx_crash(chqstate_t *state, int param1, int param2)
{
  zxspectrum_t *speccy; /* hoisted state->speccy (Conv: added) */
  int           carry;  /* carry flag used by RLC (carry) */
  u8           *tab;    /* pointer walking sfx_crash_table (was HL) */
  int           j;      /* outer iteration count, 93 bytes (was C) */
  int           i;      /* inner loop counter, param1 times per byte (was B) */
  int           bits;   /* EAR output bit state (was A) */

  NOT_USED(param2);

  speccy = state->speccy;
  carry = 0;
  tab = &state->sfx_crash_table[0];
  j  = NELEMS(sfx_crash_table);
  do {
    i = param1;
    do {
      bits = port_MASK_EAR;
      /* Conv: $896A JR NZ skips the RES when bit 7 is set, so EAR follows
       * bit 7 — the previous C inverted this. */
      if ((*tab & (1 << 7)) == 0)
        bits &= ~port_MASK_EAR; /* i.e. bits = 0 */
      speccy->out(speccy, port_BORDER_EAR_MIC, bits);
      RLC(*tab);
      /* $8970: RLC (HL); NOP; NOP; DJNZ; LD A,$10; BIT 7,(HL); JR —
       * inter-pulse cost 15+4+4+13+7+12+12 (bit-set path) */
      speccy->logtime(speccy, 67);
    } while (--i > 0);
    tab++;
    /* $8976: INC HL; DEC C; JP NZ (6+4+10), less the DJNZ not-taken saving */
    speccy->logtime(speccy, 15);
  } while (--j > 0);
}

/**
 * $89D9: Play the 48K "thud" impact sound effect
 *
 * Steps through a 32-byte delay table ($89EF). Each byte gives the number of
 * output pulses at the current EAR level. After each group, the EAR bit is
 * toggled. [param1] controls the delay between pulses (larger = lower pitch).
 * Used for car landings ([param1]=8) and hazard hits ([param1]=3).
 *
 * \param[in] state Pointer to game state.
 * \param[in] param1 Delay multiplier between pulses; larger = lower pitch.
 *   (was D)
 * \param[in] param2 Unused. (was E)
 *
 * Conv: Z80 drives the border port via OUT ($FE); C issues the equivalent
 * write via speccy->out and models the delay loops as speccy->logtime so the
 * host can reconstruct the pulse timing.
 */
static void sfx_thud(chqstate_t *state, int param1, int param2)
{
  /* $89EF — each byte is a toggle-count between EAR-bit flips */
  static const u8 sfx_thud_table[32] = {
    0x02, 0x07, 0x05, 0x02, 0x04, 0x0A, 0x01, 0x04,
    0x09, 0x09, 0x06, 0x45, 0x01, 0x01, 0x04, 0x03,
    0x01, 0x03, 0x31, 0x04, 0x25, 0x02, 0x01, 0xBD,
    0x8E, 0xED, 0x01, 0x01, 0x01, 0x06, 0x07, 0x01
  };

  zxspectrum_t *speccy; /* hoisted state->speccy (Conv: added) */
  int           j;      /* outer iteration count, 32 bytes (was C) */
  const u8     *tab;    /* pointer walking sfx_thud_table (was HL) */
  int           bits;   /* EAR output bit state, toggled between 0 and port_MASK_EAR (was A) */
  int           i;      /* pulse count for this table entry (was B) */

  NOT_USED(param2);

  speccy = state->speccy;
  j = NELEMS(sfx_thud_table);
  tab = &sfx_thud_table[0];
  bits = 0;
  do {
    i = *tab;
    do {
      speccy->out(speccy, port_BORDER_EAR_MIC, bits);
      /* $89E2: LD E,D; DEC E/JR NZ x param1; DJNZ (4 + loop + 13) */
      speccy->logtime(speccy, 17 + DECJR_LOOP_TSTATES(param1));
    } while (--i > 0);
    bits ^= port_MASK_EAR;
    tab++;
    /* $89E8: XOR $10; INC HL; DEC C; JR NZ; LD B,(HL) (7+6+4+12+7), less
     * the DJNZ not-taken saving */
    speccy->logtime(speccy, 31);
  } while (--j > 0);
}

/**
 * $8A0F: Play the 48K cornering noise, running every other call
 *
 * Maintains a toggle flag (sfx_cornering_toggle) so that only every other call
 * proceeds to sfx_cornering_loop_outer; the intervening call returns
 * immediately. This halves the rate at which the noise fires. The game always
 * passes duty factor 100 and count 1.
 *
 * \param[in] state Pointer to game state.
 * \param[in] param1 Duty factor and outer loop count. (was D)
 * \param[in] param2 Inner loop count. (was E)
 *
 * Conv: Z80 uses a self-modifying LD A,n at $8A10 as the toggle; C uses
 * state->sfx_cornering_toggle. Z80 falls through to sfx_cornering_loop_outer at
 * $8A17; C calls it.
 */
static void sfx_cornering(chqstate_t *state, int param1, int param2)
{
  state->sfx_cornering_toggle ^= 1;
  if (state->sfx_cornering_toggle)
    return;

  sfx_cornering_loop_outer(state, param1, param2); /* was FALLTHROUGH */
}

/**
 * $8A17: Inner cornering noise loop
 *
 * Runs a random-noise burst: for each of [param2] inner iterations, calls rng
 * and — if bit 4 is set — toggles the EAR+MIC output bit with two delay loops
 * (off-phase: 24−[param1] cycles; on-phase: [param1] cycles). The outer loop
 * runs [param1] times, so heavier cornering (larger [param1]) gives more
 * iterations but shorter individual delays.
 *
 * \param[in] state Pointer to game state.
 * \param[in] param1 Outer loop count and on-phase delay. (was D)
 * \param[in] param2 Inner loop count. (was E)
 *
 * Conv: Z80 drives the border port via OUT ($FE); C issues the equivalent
 * write via speccy->out and models the delay loops as speccy->logtime so the
 * host can reconstruct the pulse timing.
 */
static void sfx_cornering_loop_outer(chqstate_t *state, int param1, int param2)
{
  zxspectrum_t *speccy; /* hoisted state->speccy (Conv: added) */
  int           i;      /* inner loop counter, param2 down to 1 (was C) */
  int           delay;  /* delay loop counter (was B) */

  speccy = state->speccy;
  do {
    i = param2;
    do {
      /* $8A18: CALL rng; AND $10 */
      speccy->logtime(speccy, RNG_TSTATES + 7);
      if (rng(state) & (1 << 4)) {
        /* u8 wrap when param1 > 24; B == 0 makes the DJNZ loop 256 times */
        delay = (24 - param1) & 0xFF;
        if (delay == 0)
          delay = 256;
        /* JR Z not taken; LD A; SUB; LD B (7+7+4+4) + DJNZ */
        speccy->logtime(speccy, 22 + DJNZ_LOOP_TSTATES(delay));
        speccy->out(speccy, port_BORDER_EAR_MIC,
                     port_MASK_EAR | port_MASK_MIC);
        /* $8A29: LD B,D; DJNZ; XOR A (4 + loop + 4) */
        speccy->logtime(speccy, 8 + DJNZ_LOOP_TSTATES(param1));
        speccy->out(speccy, port_BORDER_EAR_MIC, 0);

        speccy->logtime(speccy, 16);
      } else {
        /* $8A1D: JR Z taken; DEC C; JR NZ (12+4+12) */
        speccy->logtime(speccy, 28);
      }
    } while (--i > 0);
  } while (--param1 > 0);
}

/**
 * $8A36: Play the 48K "bip-bow" descending tone effect
 *
 * Produces a descending-pitch tone by running 20 outer iterations with a
 * shrinking on-phase delay (B = C each iteration) and a fixed 5-step inner
 * burst. Each inner step: delays [param1] cycles (the initial delay), restores
 * [param1] from [param2], delays 24−C cycles, drives EAR+MIC high, delays C
 * cycles, drives it low. As C counts down from 20 the on-phase shortens and
 * off-phase lengthens, creating the falling pitch.
 *
 * \param[in] state Pointer to game state.
 * \param[in] param1 Initial per-step delay (restored from [param2] each step).
 *   (was D)
 * \param[in] param2 Per-step delay reset value. (was E)
 *
 * Conv: Z80 drives the border port via OUT ($FE); C issues the equivalent
 * write via speccy->out and models the delay loops as speccy->logtime so the
 * host can reconstruct the pulse timing.
 */
static void sfx_bipbow(chqstate_t *state, int param1, int param2)
{
  zxspectrum_t *speccy; /* hoisted state->speccy (Conv: added) */
  int           j;      /* outer iteration counter, 20 down to 1 (was C) */
  int           i;      /* inner burst counter, restored from L each outer step (was H) */
  int           inner;  /* inner burst reset value, 5 (was L) */

  speccy = state->speccy;
  j = 20;
  i = inner = 5;
  do {
    do {
      /* $8A3B: DEC D/JR NZ x param1; LD D,E (loop + 4) */
      speccy->logtime(speccy, 4 + DECJR_LOOP_TSTATES(param1));
      param1 = param2;
      /* $8A3F: LD A,$18; SUB C; LD B,A; DJNZ (7+4+4 + loop) */
      speccy->logtime(speccy, 15 + DJNZ_LOOP_TSTATES(24 - j));
      speccy->out(speccy, port_BORDER_EAR_MIC,
                  port_MASK_EAR | port_MASK_MIC);
      /* $8A49: LD B,C; DJNZ; XOR A (4 + loop + 4) */
      speccy->logtime(speccy, 8 + DJNZ_LOOP_TSTATES(j));
      speccy->out(speccy, port_BORDER_EAR_MIC, 0);

      speccy->logtime(speccy, 16);
    } while (--i > 0);
    i = inner;
  } while (--j > 0);
}

/**
 * $8A57: Drive the perp-caught celebration sequence each frame
 *
 * Runs a multi-phase state machine, advancing one phase at a time across
 * successive frames. Returns non-zero only at phase 6, signalling main_loop to
 * skip the rest of the frame and proceed to the next stage.
 *
 * Phase 0 (NONE)      — not in the catch sequence; return 0 immediately.
 * Phase 1 (ALIGNING)  — steers the perp car to x=35 and approaches the hero.
 *                       Once close enough both cars stop and phase advances
 *                       to 2.
 * Phase 2 (STOPPING)  — moves hero car upward (car_y += 4) and scrolls
 *                       road_pos toward ROAD_126; also increments
 *                       fast_counter to keep the road scrolling. Advances to
 *                       phase 3 once car_y reaches 16.
 * Phase 3 (STOPPED)   — counts down 4 frames then triggers the arrest
 *                       overlay and advances to phase 4.
 * Phase 4 (SCORE)     — computes the stage-clear bonus (×100,000, or
 *                       ×10,000 on retry) and time-remaining bonus, formats
 *                       them into score_messages[] and shows the score
 *                       overlay.
 * Phase 5 (FADING)    — advances phase to 6 and starts a forward fade.
 * Phase 6 (ADVANCING) — silences audio, increments wanted_stage_number and
 *                       returns 1 so main_loop skips to the next stage.
 *
 * Conv: At phase 6, the Z80 uses `POP HL; JP $8401` to discard the return
 * address and jump directly into main_loop, bypassing the rest of the frame. C
 * returns 1 and the caller skips the loop body instead. Conv: At the phase 5/6
 * boundary ($8A69) the Z80 banks the remaining phase value in A' via EX AF,AF'
 * so that A can test transition_control without losing A. C uses explicit
 * conditionals without banking. Conv: EX AF,AF' pairs throughout the score
 * phase bank ASCII digits via A'; C uses Adash to hold the banked value. Conv:
 * The Z80 at $8B3D has a known bug (credit: Russell Marks): EX AF,AF' before LD
 * B,A causes B to receive the wrong (pre-exchange) value, so the time-bonus
 * increment loop runs zero times for any non-zero low digit. C applies the fix:
 * Biterations is assigned from A (the low digit) directly, before it could be
 * clobbered.
 *
 * \param[in] state Pointer to game state.
 * \return 1 when phase 6 is reached (advance to next stage); 0 otherwise.
 */
static int handle_perp_caught(chqstate_t *state)
{
  int       carry;         /* carry flag from SBC/RLC operations (carry) */
  int       zero;          /* zero flag from SBC operations (zero) */
  int       phase;         /* perp_caught_phase value read at entry (was A) */
  int       car_y;         /* hero car y position advancing during phase 2 (was A) */
  int       HLroadpos;     /* road_pos adjusted in phases 2 and 1 (was HL) */
  int       fastcounter;   /* fast_counter incremented during phase 2 (was A) */
  u8        A;             /* general accumulator across phase 3 and score phases (was A) */
  const u8 *HLmessages;    /* pointer to the stage arrest message list (was HL) */
  int       H;             /* high ASCII digit for the stage clear bonus (was H) */
  u8        D;             /* wanted_stage_number, rotated for score increment (was D) */
  int       L;             /* low ASCII digit for the stage clear bonus (was L) */
  int       C;             /* (phase 4) time BCD copy; (phase 1) perp x temp (was C) */
  int       Adash;         /* ASCII digit banked via EX AF,AF' in Z80 (was A') */
  int       Biterations;   /* score increment loop counter (was B) */
  u8        Cflag;         /* non-zero once a non-space score digit has been seen (was C) */
  u8       *DE;            /* pointer walking score_bcd from the high end (was DE) */
  u8       *HLscore;       /* pointer walking the score message buffer (was HL) */
  int       Bdelta;        /* perp horizontal position step: +5 or −5 (was B) */
  int       Ainput;        /* hero input flags before moving into Cinput (was A) */
  int       Cinput;        /* user input flags computed for the hero car (was C) */
  u8       *HLphc;         /* pointer to session.perp_halt_counter (was HL) */
  int       DEspeed;       /* perp target speed passed to hpc_set_perp_speed (was DE) */
  int       Aperpdistance; /* perp's current distance from the hero (was A) */
  int       HLspeed;       /* current speed in comparisons (was HL) */
  int       HLspeedpushed; /* hero speed saved across a PUSH/POP pair (was HL) */

  carry = 0;
  zero  = 0;

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
  setup_transition(state, TRANSITIONSTRIDE_FORWARD); /* tail call */
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
  fill_attributes(state); /* tail call */
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
                                         HLmessages); /* tail call */
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
    D = (D << 4) | (D >> 4); /* RLC D x4: swap nibbles */

    H = L;
    L = ' ';
  }
  // Write to CLEAR BONUS line
  // Conv: Split up
  state->score_messages[0x8C6F - SCORE_MESSAGES_BASE] = L;
  state->score_messages[0x8C70 - SCORE_MESSAGES_BASE] = H;

  D = (D << 4) | (D >> 4); /* RLC D x4: swap nibbles */
  increment_score(state, 0, 0, D);

  A = state->session.time_bcd;
  state->score_messages[0x8C8A - SCORE_MESSAGES_BASE] =
    A; // Write to TIME BONUS line
  C = A;
  A = (A >> 4) & 0x0F; /* RLC A x4; AND 0x0F: high nibble */
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
  // $8B34 EX AF,AF' is a swap: A' takes the old main A, which is 0 on both
  // entry paths, so a zero low digit later stores '0' not the high digit.
  Adash = 0;
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
    A = (A >> 4) & 0x0F; /* RLC A x4; AND 0x0F: high nibble */
    if (A)
      goto score_have_high_digit;

    RLC(Cflag); /* test the saw-a-digit flag, not A */
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

  setup_overlay_messages(state, &state->score_messages[0]); /* tail call */
  return 0;

move_perp:
  A = state->hazards[0].horz_pos;
  Bdelta = 5;
  if (A == 35) {
    C = A;
    goto assign_perp_pos;
  }
  else if (A < 35)
    goto change_perp_pos;
  Bdelta = -5; // else greater than

change_perp_pos:
  C = A + Bdelta;

assign_perp_pos: // is this in the right place?
  A = C;
  state->hazards[0].horz_pos = A;
  HLroadpos = state->scenedata.road_pos;
  // Conv: $8BA8 SBC HL,$0105 has carry_in=1 when the perp moved right-to-left
  // ($8B9B ADD A,$FB set carry), making the Z80 test road_pos < 0x0106 in
  // that case; this models carry_in=0 unconditionally (one boundary value).
  carry = (HLroadpos < ROAD_LEFTMOST); /* was PUSH/SUB/POP */
  Ainput = USERINPUTFLAG_UP | USERINPUTFLAG_RIGHT;
  if (!carry)
    goto assign_hero_pos;

  HLroadpos -= ROAD_RIGHTMOST;
  carry = (HLroadpos <= 0); /* Z80: SBC HL,DE with carry_in=1; fires if road_pos <= ROAD_RIGHTMOST */
  Ainput = USERINPUTFLAG_UP | USERINPUTFLAG_LEFT;
  if (carry)
    goto assign_hero_pos;

  Ainput = USERINPUTFLAG_UP;

assign_hero_pos:
  /* Cinput keeps the full input; the AND below only tests the LEFT/RIGHT
   * bits. Cinput must retain UP or the hero never accelerates. */
  Cinput = Ainput;
  if (Ainput & (USERINPUTFLAG_LEFT | USERINPUTFLAG_RIGHT))
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
    // Conv: $8BEF CPL; ADD A,$0F gives 14-distance (not 16-), and the ADD
    // leaves carry set so the first SBC HL,DE subtracts 21. Z80 edge case
    // distance==14 gives B=0 so DJNZ loops 256 times; here it subtracts 1.
    HLspeed -= 20 * (14 - Aperpdistance) + 1;
  DEspeed = HLspeed; // perp's adjusted speed
  HLspeed = state->speed; // our speed
  HLspeedpushed = HLspeed; // PUSH HL
  carry = (HLspeed < DEspeed);
  HLspeed -= DEspeed;
  if (!carry) {
    Cinput &= ~USERINPUTFLAG_UP;
    carry = (HLspeed < 50);
    HLspeed -= 50;
    if (!carry)
      Cinput |= USERINPUTFLAG_DOWN;
  }
  HLspeed = HLspeedpushed; // POP HL
  carry = (HLspeed < SPEED_GEAR_CHANGE);
  HLspeed -= SPEED_GEAR_CHANGE;
  A = state->gear - carry; // set low speed if speed<SPEED_GEAR_CHANGE
  if (A == 0)
    Cinput |= USERINPUTFLAG_FIRE; // change gear
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
 * $8C35: Set the perp car's scripted drive speed
 *
 * Writes the given [speed] to hazards[0].[speed] so that the perp car follows
 * its scripted velocity during the pull-over sequence.
 *
 * The Z80 function is labelled hpc_set_perp_pos_or_accel in the skool because
 * the role of the written field was unclear during disassembly; the C port maps
 * it to the [speed] field of hazards[0].
 *
 * \param[in] state Pointer to game state.
 * \param[in] speed Scripted drive speed to assign. (was DE)
 */
static void hpc_set_perp_speed(chqstate_t *state, int speed)
{
  state->hazards[0].speed = speed;
}

/**
 * $8C3A: Initiate the pull-over sequence after the perp is fully smashed
 *
 * Called once the player's smash_counter reaches its maximum, signalling that
 * the perp has been disabled. Starts the perp car alignment phase, raises the
 * stop hand, suppresses player input (only pause and quit are allowed), shows
 * the "OK! PULL OVER CREEP!" overlay and sets the perp to its post-arrest
 * scripted drive speed.
 *
 * \param[in] state Pointer to game state.
 */
static void fully_smashed(chqstate_t *state)
{
  state->perp_caught_phase  = PERPCAUGHTPHASE_ALIGNING;
  state->hand_flag          = HANDFLAG_STOP;
  state->smash_counter      = SMASHCOUNTER_MAX;
  state->session.user_input_mask = USERINPUTFLAG_PAUSE | USERINPUTFLAG_QUIT;
  setup_overlay_messages(state, &pull_over_message[0]);
  hpc_set_perp_speed(state, SPEED_PERP_CAUGHT);
}

/**
 * $8D8F: Drive the scene transition or overlay effect each frame
 *
 * Dispatches on transition_control to perform the per-frame transition work.
 * Non-FADE modes delegate immediately to draw_mugshots, draw_overlay_messages
 * or fill_attributes. FADE mode draws eight pairs of back-buffer stripes per
 * frame by ORing a fade mask into the pixels, stepping through the mask table
 * by transition_frame_stride each frame until transition_nframes reaches zero.
 *
 * The Z80 uses self-modifying instructions to track state: the SM field at
 * $8DA1 holds the remaining frame count, the word at $8DBB holds the current
 * mask pointer and the word at $8DB1 holds the per-frame stride. In C these
 * three values are stored in state->transition_nframes, state->transition_mask
 * and state->transition_frame_stride respectively.
 *
 * \param[in] state Pointer to game state.
 */
static void transition(chqstate_t *state)
{
  int       mask;        /* fade mask byte ORed into each back-buffer pixel (was E) */
  u16       backbuf;     /* back-buffer high byte packed in u16; L forced per call (was HL) */
  const u8 *maskptr;     /* pointer walking the transition mask table (was HL') */
  int       iterations;  /* shadow iteration count, 8 chunks (was B') */
  u16       backbufcopy; /* saved backbuf restored after the odd-chunk call (was D) */

  switch (state->transition_control) {
  case TRANSITIONCONTROL_STOP:
    return;
  case TRANSITIONCONTROL_DRAW_MUGSHOTS:
    draw_mugshots(state); /* tail call */
    return;
  case TRANSITIONCONTROL_OVERLAY_MESSAGES:
    draw_overlay_messages(state); /* tail call */
    return;
  case TRANSITIONCONTROL_FILL_ATTRIBUTES:
    fill_attributes(state); /* tail call */
    return;
  case TRANSITIONCONTROL_FADE:
    break;
  default:
    assert(0);
  }

  if (--state->transition_nframes == 0)
    state->transition_control = TRANSITIONCONTROL_STOP;
  else
    // Advance before use — initial mask points one entry before the first frame
    state->transition_mask += state->transition_frame_stride;

  backbuf    = 0xFF00; /* $8DB7: H=$FF — top-of-back-buffer high byte */
  maskptr    = state->transition_mask;
  iterations = 8;
  do {
    mask         = *maskptr;
    backbufcopy  = backbuf; // Conv: Z80 saved only H in D; C saves the full u16
    backbuf      = (backbuf & ~0xFF) | 0xFE;
    transition_fade_chunk(state, mask, ADDRTOBACKBUF(backbuf));
    backbuf -= 8 << 8;
    transition_fade_chunk(state, mask, ADDRTOBACKBUF(backbuf));
    backbuf = backbufcopy - 256; // restore H, step down one row
    maskptr++;
  } while (--iterations > 0);
}

/**
 * $8DD8: OR a fade mask into one 8-row stripe of the back buffer
 *
 * Writes [mask] into 30 bytes per row (6 iterations of 5 ORs each) across 8
 * rows, stepping backward through the buffer. The two-byte skip at the end of
 * each row skips the two bytes that fall outside the playfield width. Called
 * twice per chunk by transition — once for the upper stripe and once for the
 * lower stripe 8 rows higher.
 *
 * \param[in] state Pointer to game state.
 * \param[in] mask Fade mask byte ORed into each pixel. (was E)
 * \param[in] backbuf Pointer to the last byte of the first row to process.
 *   (was HL)
 */
static void transition_fade_chunk(chqstate_t *state, int mask, u8 *backbuf)
{
  int rows;       /* outer row counter, 8 rows per stripe (was C) */
  int iterations; /* inner column iteration count, 6 × 5 = 30 bytes per row (was B) */

  NOT_USED(state);

  rows = 8;
  do {
    iterations = 6; /* 6 iterations × 5 ORs = 30 bytes written per row */
    do {
      *backbuf-- |= mask;
      *backbuf-- |= mask;
      *backbuf-- |= mask;
      *backbuf-- |= mask;
      *backbuf-- |= mask;
    } while (--iterations > 0);
    backbuf -= 2; /* skip the 2 bytes beyond the playfield edge */
  } while (--rows > 0);
}

/**
 * $8DF9: Set up a new scene transition
 *
 * Picks a random entry from the forward or reverse half of the transition
 * table, then stores the frame count, initial mask pointer and per-frame
 * [stride] into state so that transition() can drive the effect each frame. A
 * positive [stride] (TRANSITIONSTRIDE_FORWARD = 8) selects the first four table
 * entries; a negative [stride] (TRANSITIONSTRIDE_REVERSE = -8) selects the last
 * four.
 *
 * The Z80 stores these values via self-modification: nframes into the operand
 * of `LD A,n` at $8DA1, the mask pointer into the word at $8DBB and the
 * [stride] into the word at $8DB1. C replaces those SM locations with
 * state->transition_nframes, state->transition_mask and
 * state->transition_frame_stride.
 *
 * Conv: Z80 table index is A * 3 into a byte-packed layout; C reads from a
 * transition_t struct array directly. Conv: Z80 widens the [stride] into BC by
 * setting B = $FF when negative; C uses a signed int throughout. Conv: Points
 * at non-relocated table transitions_e88e rather than $EC00.
 *
 * \param[in] state Pointer to game state.
 * \param[in] stride Per-frame mask-pointer step: +8 forward, −8 reverse.
 *   (was A)
 */
static void setup_transition(chqstate_t *state, int stride)
{
  int                 frame_stride;   /* signed per-frame pointer step (was BC) */
  const transition_t *transition_set; /* base of forward or reverse table half (was DE) */
  const transition_t *transition;     /* randomly chosen entry (was HL) */

  assert(stride == 8 || stride == -8);

  frame_stride = stride;
  transition_set = &transitions[(frame_stride >= 0) ? 0 : 4]; /* select forward or reverse set */

  state->transition_frame_stride = frame_stride;

  transition = &transition_set[rng(state) & 3];

  state->transition_nframes = transition->nframes;
  state->transition_mask    = transition->frames;
  state->transition_control = TRANSITIONCONTROL_FADE;
}

/**
 * $8E29: Propagate the leftmost attribute byte across each playfield row
 *
 * The caller (e.g. scenery_hit) has already set the attribute at column 1 of
 * each row; this routine extends it rightward across the next 28 columns. The
 * Z80 does this with a rolling LDIR: DE = HL + 1, BC = 28, LDIR copies byte 0 →
 * bytes 1..28, then HL advances to the start of the next row. After 16 rows,
 * transition_control is set to TRANSITIONCONTROL_STOP.
 *
 * \param[in] state Pointer to game state.
 */
static void fill_attributes(chqstate_t *state)
{
  static const zxbox_t playfield_box = { /* lower two-thirds of screen */
    0, 0, SCREEN_WIDTH, PLAYFIELD_HEIGHT
  };

  u8 *HLsrc;      /* pointer to first attribute of current row (was HL) */
  u8 *DEdst;      /* destination: HLsrc + 1 each iteration (was DE) */
  int A_rows;     /* row counter, 16 down to 1 (was A) */
  int BC_columns; /* column copy count, 28 (was BC) */

  HLsrc  = ADDRTOATTRS(0x5901);
  A_rows = 16;
  do {
    DEdst      = HLsrc + 1;
    BC_columns = 28;
    do { *DEdst++ = *HLsrc++; } while (--BC_columns > 0);
    HLsrc += SCREEN_ATTRIBUTES_WIDTH - 28;
  } while (--A_rows > 0);

  state->transition_control = TRANSITIONCONTROL_STOP;
  state->speccy->draw(state->speccy, &playfield_box); /* Conv: added */
}

/**
 * $8E42: Progressively reveal overlay messages into the back buffer
 *
 * Drives the frame-by-frame reveal of a sequence of overlay message blocks
 * (e.g. the arrest bonus screen). Each call draws messages [0..count-1] from
 * the current overlay_message list, then decrements overlay_delay. When the
 * delay reaches zero a new delay is read from the message stream and
 * overlay_count is incremented, causing one additional message to appear on the
 * next call. This continues until a DRAWOVERLAY_STOP sentinel is reached, at
 * which point transition_control is updated from the byte preceding the
 * sentinel to trigger the next state.
 *
 * The Z80 uses self-modification to track state: HL at $8E45 holds the base
 * message pointer, B at $8E46 holds the current count and the delay is the
 * operand at $8E4A. In C these are stored in state->overlay_message,
 * state->overlay_count and state->overlay_delay respectively.
 *
 * \param[in] state Pointer to game state.
 */
static void draw_overlay_messages(chqstate_t *state)
{
  const u8 *message; /* base pointer to the overlay message stream (was HL) */
  int       count;   /* number of messages to draw this call (was B) */
  int       style;   /* style byte of the current message block (was A) */

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
 * $8E6C: Draw one overlay-message block to the back buffer
 *
 * Reads a 6-byte header from messages[], decoding the attribute byte,
 * back-buffer destination address and screen attribute address, then calls
 * draw_string_with_style to render the NUL-terminated string that follows.
 * Returns a pointer to the byte after the NUL so the caller can chain calls.
 *
 * The Z80 banks the [style] byte in A' via EX AF,AF' at entry, reads the header
 * with successive INC HL / LD r,(HL) pairs, then tail-calls $9F99
 * (draw_string_with_style via its alternate entry point).
 *
 * Conv: messages[0] (the flags byte) is skipped with INC HL in the Z80; C reads
 * messages[1] directly and ignores messages[0]. Conv: Z80 preserves BC with
 * PUSH/POP around the call; C locals survive calls without banking.
 *
 * \param[in] state Pointer to game state.
 * \param[in] style Rendering style selector. (was A)
 * \param[in] messages Pointer to the start of the message data block. (was HL)
 * \return Pointer to the byte following the NUL terminator.
 */
static const u8 *print_message(chqstate_t *state,
                               int         style,
                               const u8   *messages)
{
  u8  A_attr;    /* attribute byte for the string (was A) */
  u16 DEbackbuf; /* back-buffer destination address (was DE) */
  u16 BCtarget;  /* screen attribute address (was BC) */

  /* Conv: header fields read explicitly rather than via INC HL chains */
  A_attr    = messages[1]; /* messages[0] is the flags byte skipped by INC HL */
  DEbackbuf = wordat(messages + 2);
  BCtarget  = wordat(messages + 4);
  messages += 6;

  return draw_string_with_style(state,
                                A_attr,
                                ADDRTOATTRS(BCtarget),
                                ADDRTOBACKBUF(DEbackbuf),
                                messages,
                                style);
}

/**
 * $8E7E: Arm the overlay-message display for a time-up or arrest sequence
 *
 * Convenience wrapper: loads A with TRANSITIONCONTROL_OVERLAY_MESSAGES (2) then
 * falls through to setup_overlay_messages_with_transition. In the Z80 the
 * fall-through is literal; in C it is an explicit call.
 *
 * \param[in] state Pointer to game state.
 * \param[in] message Pointer to the message data block (delay byte, then
 *   records). (was HL)
 */
static void setup_overlay_messages(chqstate_t *state, const u8 *message)
{
  setup_overlay_messages_with_transition(state,
                                         TRANSITIONCONTROL_OVERLAY_MESSAGES,
                                         message);
}

/**
 * $8E80: Arm the overlay-message display with a caller-supplied transition code
 *
 * Stores the four values needed by draw_overlay_messages so that the main loop
 * starts rendering the [message] sequence on the next frame.
 *
 * The Z80 uses self-modifying code at $8E49/$8E43/$8E46 (inside
 * draw_overlay_messages) to store the delay, [message] pointer and initial
 * count; C stores them in state fields with equivalent semantics.
 *
 * Conv: Z80 SM writes to instructions inside draw_overlay_messages; C uses
 * struct fields overlay_delay, overlay_message and overlay_count.
 *
 * \param[in] state Pointer to game state.
 * \param[in] transition Transition mode to activate (e.g.
 *   TRANSITIONCONTROL_OVERLAY_MESSAGES). (was A)
 * \param[in] message Pointer to the message data block; message[0] is the frame
 *   delay. (was HL)
 */
static void setup_overlay_messages_with_transition(chqstate_t *state,
                                                   int         transition,
                                                   const u8   *message)
{
  state->transition_control = transition;
  state->overlay_delay      = message[0];  /* $8E83–$8E84 SM frame delay */
  state->overlay_message    = &message[1]; /* $8E87–$8E88 SM message pointer */
  state->overlay_count      = 1;           /* $8E8B–$8E8D SM initial count */
}

/**
 * $8E91: Draw the perp's and officers' mugshots to the back buffer
 *
 * Draws three 4×5-character (32×40-pixel) face bitmaps to the back buffer: the
 * perp (from stage data), Tony (bitmap_faces row 2) and Raymond (row 1). Each
 * face is drawn from the end of its bitmap upwards via draw_mugshot. Afterwards
 * falls through to draw_overlay_messages via JR in the Z80.
 *
 * Conv: Z80 loads the perp mugshot address from ($5CF0) — the end of the
 * per-stage mugshot data; C reads it from
 * state->stage->addrof_perp_mugshot_attributes. Conv: Z80 JR $8E42 tail-calls
 * draw_overlay_messages; C calls it directly.
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
 * $8EB7: Copy one mugshot bitmap into the back buffer and set its attributes
 *
 * Copies FACEBITMAPBYTES (160 = 32×5 pixels) of bitmap data from the byte
 * immediately before [HLmugshot] down to the back buffer at [DEbackbuf]. The
 * copy proceeds four bytes at a time (four LDD instructions per iteration),
 * each batch writing one four-byte slice across a 4-pixel-wide column and then
 * advancing the back-buffer pointer to the previous scanline via prev_buf_row.
 * After all bytes are copied, falls through to plot_face_attributes.
 *
 * The Z80 PUSH HL / POP HL preserves the mugshot pointer so that the attribute
 * address is still in HL after the loop. C preserves it in HLsaved.
 *
 * Conv: Z80 LDD copies (HL)→(DE) with decrement; C indexes backbuffer directly.
 * Conv: Z80 PUSH/POP HL saves the mugshot attributes pointer; C uses HLsaved.
 * Conv: Z80 DEC H / rollover for scanline advance; C uses prev_buf_row(). Conv:
 * JP PO branches when BC wraps to zero after LDD; C checks counter == 0.
 *
 * \param[in] state Pointer to game state.
 * \param[in] BCattrs Screen attribute address for the face. (was BC)
 * \param[in] DEbackbuf Back-buffer address of the last bitmap byte. (was DE)
 * \param[in] HLmugshot Pointer to the start of the mugshot attribute data;
 * the bitmap immediately precedes it. (was HL)
 */
static void draw_mugshot(chqstate_t *state,
                         int         BCattrs,
                         int         DEbackbuf,
                         const u8   *HLmugshot)
{
  const u8 *HLsaved;   /* saved HL: mugshot attribute pointer (was PUSH HL / POP HL) */
  int       BC_count;  /* remaining bytes to copy; JP PO exits when 0 (was BC) */

  HLsaved  = HLmugshot;
  HLmugshot--;         /* step back to end of bitmap data */
  BC_count = FACEBITMAPBYTES;
  for (;;) {
    DEbackbuf -= BACKBUFFER_START_ADDRESS; /* Conv: Z80 address → array offset */
    state->backbuffer[DEbackbuf--] = *HLmugshot--; BC_count--;
    state->backbuffer[DEbackbuf--] = *HLmugshot--; BC_count--;
    state->backbuffer[DEbackbuf--] = *HLmugshot--; BC_count--;
    state->backbuffer[DEbackbuf--] = *HLmugshot--; BC_count--;
    DEbackbuf += BACKBUFFER_START_ADDRESS; /* Conv: offset → Z80 address */
    DEbackbuf += 4; /* restores E to pre-LDD value (Conv: see above) */
    if (BC_count == 0) /* $8EC7 JP PO — parity overflow = BC wrapped to 0 */
      break;

    DEbackbuf = prev_buf_row(DEbackbuf);
  }

  plot_face_attributes(state, BCattrs, HLsaved);
}

/**
 * $8EE7: Draw the smash-counter bar in the back buffer
 *
 * The bar runs vertically on the right edge of the playfield. It is composed of
 * three sections drawn bottom-to-top:
 *
 * BorderHeight (2) solid rows at the bottom. smash_counter segments, each 3
 * rows (2 dashed + 1 solid gap). Remaining solid rows up to the total bar
 * height.
 *
 * Returns early (invisible bar) when the perp has not yet been sighted or when
 * the perp has been stopped (perp_caught_phase >= PERPCAUGHTPHASE_STOPPED).
 *
 * The Z80 uses D=$0F (scanline mask) and E=$10 (row stride) for the
 * prev-scanline address arithmetic; C abstracts this into prev_buf_row().
 *
 * Conv: Z80 uses EX AF,AF' to preserve smash_counter across the segment call; C
 * keeps it in A_nsmashsegs which survives the call naturally. Conv: Z80
 * computes nsolid as CPL(A*3)+$3F (bit-complement + 63); C uses the equivalent
 * arithmetic expression.
 *
 * \param[in] state Pointer to game state.
 */
static void draw_smash_bar(chqstate_t *state)
{
  const int MaxSegments    = SMASHCOUNTER_MAX;  /* maximum smash-counter value */
  const int SegmentHeight  = 3;                 /* scanlines per segment */
  const int BorderHeight   = 2;                 /* solid border rows top and bottom */
  const int TotalBarHeight = MaxSegments * SegmentHeight + BorderHeight * 2;

  u16 HLbackbuf;    /* back-buffer address walking upward through the bar (was HL) */
  int A_nsmashsegs; /* current smash_counter value; banked to A' around $8F05 (was A) */
  int B_nsolid;     /* number of solid rows to fill above the segments (was B) */

  if (state->sighted_flag == 0)
    return;
  if (state->perp_caught_phase >= PERPCAUGHTPHASE_STOPPED)
    return;

  HLbackbuf = 0xF7A2; /* bottom of bar in back buffer */

  HLbackbuf  = draw_smash_bar_solid_bit(state, BorderHeight, HLbackbuf);
  A_nsmashsegs = state->smash_counter;
  if (A_nsmashsegs > 0)
    HLbackbuf = draw_smash_bar_segments(state, A_nsmashsegs, HLbackbuf);

  B_nsolid = TotalBarHeight - BorderHeight - A_nsmashsegs * SegmentHeight;
  (void) draw_smash_bar_solid_bit(state, B_nsolid, HLbackbuf);
}

/**
 * $8F13: Draw smash-counter segments (dashed pairs with a solid gap)
 *
 * Each segment is three scanlines: two dashed (X______X) rows and one solid row
 * drawn by draw_smash_bar_solid_bit. The loop runs nsegs times upward through
 * the back buffer.
 *
 * The Z80 uses the DEC H / rollover pattern for prev-scanline movement; C uses
 * prev_buf_row() throughout.
 *
 * \param[in] state Pointer to game state.
 * \param[in] C_nsegs Number of segments to draw. (was C)
 * \param[in] HLbackbuf Back-buffer address at which to start. (was HL)
 * \return Back-buffer address after the last row written.
 */
static u16 draw_smash_bar_segments(chqstate_t *state, int C_nsegs, int HLbackbuf)
{
  do {
    *ADDRTOBACKBUF(HLbackbuf) = X______X;
    HLbackbuf = prev_buf_row(HLbackbuf);
    *ADDRTOBACKBUF(HLbackbuf) = X______X;
    HLbackbuf = prev_buf_row(HLbackbuf);
    HLbackbuf = draw_smash_bar_solid_bit(state, 1, HLbackbuf);
  } while (--C_nsegs > 0);
  return HLbackbuf;
}

/**
 * $8F47: Draw a run of solid (fully filled) smash-bar scanlines
 *
 * Writes $FF (XXXXXXXX) to nrows consecutive scanlines, walking upward through
 * the back buffer with prev_buf_row(). Used for both the border rows and the
 * solid fill above the segments.
 *
 * \param[in] state Pointer to game state.
 * \param[in] B_nrows Number of solid rows to draw. (was B)
 * \param[in] HLbackbuf Back-buffer address at which to start. (was HL)
 * \return Back-buffer address after the last row written.
 */
static u16 draw_smash_bar_solid_bit(chqstate_t *state, int B_nrows, int HLbackbuf)
{
  do {
    *ADDRTOBACKBUF(HLbackbuf) = XXXXXXXX;
    HLbackbuf = prev_buf_row(HLbackbuf);
  } while (--B_nrows > 0);
  return HLbackbuf;
}

/**
 * $8F5F: Draw scene objects
 *
 * Draws all non-road, non-hero scene elements each frame: road-edge scenery
 * (signs, poles, trees, barriers), the perp-vehicle floating arrow, overhead
 * objects (bridges), and hazard cars via draw_hazard_sprites. Adjusts
 * height_table and clamped_heights by +32 to convert from road-buffer
 * coordinates to screen coordinates, then walks the object table for the
 * current road section drawing each object through its type-specific callback.
 *
 * \param[in,out] state Pointer to game state.
 */
static void draw_scene_objects(chqstate_t *state)
{
  u8             *HLheight_table;      /* pointer walking height_table[], adjusted +32 (was HL) */
  u8             *DEclamped_heights;   /* pointer walking clamped_heights[], adjusted +32 (was DE) */
  int             Biterations;         /* loop counter for table-adjustment and object passes (was B) */
  u8             *IYheight_table;      /* pointer into height_table for per-object calls (was IY) */
  u8             *HLroadbuf;           /* pointer into road_buffer for right-side object scan (was HL) */
  s16            *IXtable_ea00;        /* pointer into xpos_road_centre at offset 88 ($EAB0) (was IX) */
  int             Aobj;                /* current object byte from the road buffer (was A) */
  int             Afloating_arrow;     /* non-zero when the floating target arrow should be drawn (was A) */
  const bitmap_t *HLarrow_defn;        /* pointer to the floating arrow bitmap definition (was HL) */
  int             Ex;                  /* horizontal position for the arrow draw (was E) */
  int             Dy;                  /* vertical position for the arrow draw (was D) */
  int             Cwidth_bytes;        /* sprite byte width for the arrow (was C) */
  int             Bdash_flip_flag;     /* flip flag banked at EXX for the arrow draw (was B') */
  int             Edash_bitmap_stride; /* bitmap stride banked at EXX for the arrow draw (was E') */
  int             Cdash;               /* start offset banked at EXX for the arrow draw (was C') */
  int             Bheight;             /* sprite row count for the arrow (was B) */
  const u8       *HLbitmap;            /* pointer to arrow pixel data (was HL) */
  int             Eobj;                /* object index used to look up the object table entry (was E) */
  const obj_t    *HLobj;               /* pointer to the current scene object descriptor (was HL) */

  assert(state->stage != NULL);
  assert(state->roadbuf_start == &state->road_buffer[0]);
  assert(state->roadbuf_end   == &state->road_buffer[256]);

  state->ddas_particle = (u8 *) state->xpos_road_fork_right + 0x28; // $ED28
  state->dhs_xpos_table = &state->xpos_road_centre_left[0];
  assert(state->ddas_particle >= (u8 *) &state->xpos_road_fork_right[0] &&
         state->ddas_particle < (u8 *) &state->xpos_road_fork_right[128]);
  assert(state->dhs_xpos_table >= &state->xpos_road_centre_left[0] &&
         state->dhs_xpos_table < &state->xpos_road_centre_left[128]);

  HLheight_table = &state->height_table[1];
  DEclamped_heights = &state->clamped_heights[0];
  Biterations = 21;
  do {
    assert(HLheight_table >= &state->height_table[0]
           && HLheight_table < &state->height_table[PERSP_TABLE_COLS]);
    assert(DEclamped_heights >= &state->clamped_heights[0]
           && DEclamped_heights < &state->clamped_heights[21]);
    *HLheight_table++ += 32;
    *DEclamped_heights++ += 32;
  } while (--Biterations > 0);
  assert(HLheight_table == &state->height_table[PERSP_TABLE_COLS]);
  assert(DEclamped_heights == &state->clamped_heights[21]);

  IYheight_table = &state->height_table[21];
  assert(IYheight_table == &state->height_table[21]);
  if (state->dee_draw_tunnel_1)
    draw_tunnel(state, IYheight_table);
  IYheight_table--;
  assert(IYheight_table == &state->height_table[20]);

  HLroadbuf = ROADBUF_FWD2PTR(115); // right side objects
  assert(HLroadbuf >= state->roadbuf_start
         && HLroadbuf < state->roadbuf_end);

  IXtable_ea00 = &state->xpos_road_centre[88]; // $EAB0
  assert(IXtable_ea00 == &state->xpos_road_centre[88]);
  Biterations = 20; // iterations
  do {
    assert(Biterations >= 1 && Biterations <= 20);
    assert(HLroadbuf >= state->roadbuf_start
           && HLroadbuf < state->roadbuf_end);
    assert(IYheight_table >= &state->height_table[0]
           && IYheight_table < &state->height_table[PERSP_TABLE_COLS]);
    assert(IXtable_ea00 >= &state->xpos_road_centre[0]
           && IXtable_ea00 < &state->xpos_road_centre[128]);

    if (state->n_hazards)
      draw_hazard_sprites(state, Biterations, IYheight_table);

    draw_dirt_and_stones(state, Biterations, IYheight_table);

    if (state->dee_draw_helicopter)
      draw_helicopter(state, Biterations, IYheight_table);

    if (state->dee_draw_tunnel_2)
      draw_tunnel(state, IYheight_table);

    Aobj = *HLroadbuf; // fetch right side object from road buffer
    assert(Aobj <= 9); // object indices are 0..9
    if (Aobj)
      goto right_hand_stuff;

continue_after_right_hand_done:
    IXtable_ea00++;
    WRAP_ASSIGN(HLroadbuf, 32,
             state->roadbuf_start); // advance to left-side column
    assert(HLroadbuf >= state->roadbuf_start
           && HLroadbuf < state->roadbuf_end);

    Aobj = *HLroadbuf; // fetch left side object from road buffer
    assert(Aobj <= 9); // object indices are 0..9
    if (Aobj)
      goto left_hand_stuff;

continue_after_left_hand_done:
    IXtable_ea00++;
    WRAP_ASSIGN(HLroadbuf, -33, state->roadbuf_start); // retreat one row
    assert(HLroadbuf >= state->roadbuf_start
           && HLroadbuf < state->roadbuf_end);

    IYheight_table--;
  } while (--Biterations > 0);
  assert(IYheight_table == &state->height_table[0]);
  assert(IXtable_ea00 ==
         &state->xpos_road_centre[128]); // one-past-end after 20*2 advances from [88]

  if (state->dee_draw_helicopter)
    return;

  Afloating_arrow = state->floating_arrow;
  if (Afloating_arrow == 0)
    return;

  // Draw the floating arrow
  assert(Afloating_arrow == 1 || Afloating_arrow == 2);
  if (Afloating_arrow == 1) {
    HLarrow_defn = &floating_arrow_left_defn;
    Ex = 120;
  } else {
    HLarrow_defn = &floating_arrow_right_defn;
    Ex = 128;
  }
  assert(HLarrow_defn != NULL);
  assert(HLarrow_defn->data != NULL);
  assert(Ex == 120 || Ex == 128);
  Dy           = 48;
  Cwidth_bytes = HLarrow_defn->width_bytes;
  assert(Cwidth_bytes == 3); // both arrow bitmaps have width_bytes=3
  Bdash_flip_flag = HLarrow_defn->flags >> 1;
  Edash_bitmap_stride = Cwidth_bytes;
  assert(Edash_bitmap_stride == Cwidth_bytes);
  Cdash        = 0; // this must be passed in
  Bheight      = HLarrow_defn->height;
  assert(Bheight == 21); // both arrow bitmaps have height=21
  assert(Bheight > 0);
  HLbitmap      = HLarrow_defn->data;
  assert(HLbitmap != NULL);
  draw_part_entrypt2(state,
                     Bheight,
                     Cwidth_bytes,
                     Dy,
                     Ex,
                     HLbitmap,
                     Bdash_flip_flag,
                     Cdash,
                     Edash_bitmap_stride); /* tail call */
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
  // Conv: Z80 $9024 jumps to handler directly if A==2 (skips IX[1] check).
  //       $902C exits without calling handler if IX[1] != 0.
  //       Handler runs when: Aobj == 2 OR IX[1] == 0.
  if (Aobj == 2
      || !((u8 *)IXtable_ea00)[1]) { // Z80: LD A,(IX+1) -- buffer offset/distance
    HLobj = &state->stage->addrof_left_hand_objects[Eobj];
    assert(HLobj->handler != NULL);
    HLobj->handler(state, Biterations, HLobj->arg, IXtable_ea00, IYheight_table);
  }
  goto continue_after_left_hand_done;
}

/* Total byte writes in the unrolled deck-fill loop at $9117-$9151. */
#define BRIDGE_DECK_LOOP_WRITES (30)

/**
 * $9052: Draw overhead objects
 *
 * Draws overhead spanning objects such as bridges. First calls
 * draw_stretchy_object_left if IXxpos[1] is zero. Looks up the perspective
 * y-scale for the current frame row, computes the vertical offset written to
 * do_vert_sub, and selects the depth-set pair index as MIN([Bparam] − 1, 9).
 * Then checks the xpos table entries on both sides of the current position to
 * determine visibility and clipping extent (D and E width fields). Draws the
 * overhead deck row-by-row: each depth level has an overhead_span_t entry
 * (a row count plus one solid fill byte per row) selected by Bminheight;
 * every row is a single-colour memset spanning the clipped width.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Bparam Depth scale index; also selects the pair entry (0..9).
 *   (was B)
 * \param[in]     arg Pointer to the stretchy object descriptor array. (was DE)
 * \param[in]     IXxpos Pointer into xpos_road_centre for this object slot.
 *   (was IX)
 * \param[in]     IYheight Pointer into height_table for this object slot.
 *   (was IY)
 */
void draw_overhead(chqstate_t *state,
                   int         Bparam,
                   const void *arg,
                   const s16  *IXxpos,
                   const u8   *IYheight)
{
  const stretchy_t      *DEstretchy;  /* pointer to the stretchy object descriptor array (was DE) */
  const stretchy_t      *HLstretchy;  /* pointer to the stretchy descriptor (was HL) */
  const depthset_pair_t *DEpairs;     /* pointer to the depth-set pairs array (was DE) */
  u8                    *HLdst;       /* destination pointer during row copy (was HL) */
  const u8              *DEsrc;       /* source pointer during row copy (was DE) */
  int                    Avertical;   /* raw vertical scale value from persp_y_scale (was A) */
  int                    Cparam;      /* scratch: computed vertical offset (was C) */
  int                    A;           /* scratch accumulator: visibility tests and offsets (was A) */
  int                    Aminheight;  /* MIN(Bparam−1, 9): pair index into depth-set (was A) */
  int                    Bminheight;  /* copy of Aminheight for draw loop (was B) */
  const u8              *HLvertical;  /* pointer into persp_y_scale for current frame row (was HL) */
  int                    D;           /* left-clip width in bytes for the right edge of the span (was D) */
  int                    E;           /* right-clip extent: 0x1F minus visibility offset (was E) */
  const depthset_pair_t *HLpair;      /* pointer to the chosen depth-set pair entry (was HL) */
  int                    Cdepth;      /* depth value from the selected pair entry (was C) */
  const overhead_span_t *HLspan;      /* chosen overhead span-fill entry (was HL) */
  int                    Acopy;       /* scratch copy of A during sub-loop (was A) */
  int                    B;           /* sub-loop counter for per-row byte writes (was B) */

  DEstretchy = (const stretchy_t *) arg;

  // PUSH IXxpos/DE/BC
  // Conv: IXxpos[1] would index the *next* s16 element (see pitfall: index-
  // before-u8-cast); (IX+1) in the Z80 is the high byte of *this* entry.
  if (((const u8 *) IXxpos)[1] == 0) // buffer offset/distance
    draw_stretchy_object_left(state, Bparam, arg, IXxpos, IYheight);
  // POP BC/HL/IXxpos

  HLstretchy = DEstretchy; // e.g. $6F26 in Stage 3's data, loads $6F2D
  //HLstretchy++; - accounted for
  DEpairs = &HLstretchy->set->pairs[0];

  HLvertical = &persp_y_scale[FAST_COUNTER_PERSP_ROW(state)][Bparam];

  Cparam = IYheight[0] - IYheight[0x35];
  Avertical = *HLvertical;
  A = (Avertical >> 1) + Avertical - Cparam; // Conv: removed use of L
  state->do_vert_sub = A; // Self modify 'SUB x' at $90F1
  Aminheight = MIN(Bparam - 1, 9);
  Bminheight = Aminheight;
  HLpair = &DEpairs[Aminheight];
  Cdepth = HLpair->depth;
  HLspan = &HLstretchy->set->spans[Bminheight];

  D = 1;
  A = ((const u8 *) IXxpos)[1]; // buffer offset/distance
  if ((s8) A >= 0) {
    if (A)
      return;
    A = (IXxpos[0] + 24) & 0xFF; // $90AD-$90B0: ADD A,$18 (8-bit wrap)
    // Conv: each Z80 SUB below sets carry (borrow) when the subtrahend
    // exceeds A; a sign-bit test on the difference is only reliable while
    // the difference fits -128..+127; direct unsigned comparisons are used
    // instead (see pitfall: borrow detection via bit 7).
    if (A >= Cdepth) { // $90B2-$90B3: SUB C; JR C (skip on borrow)
      A -= Cdepth;
      if (A >= 8) { // $90B5-$90B7: SUB $08; JR C (skip on borrow)
        A -= 8;
        if (A >= 8) // $90B9-$90BB: CP $08; JR C (skip on carry)
          D = A >> 3;
      }
    }
  }

  // Conv: Z80 `DEC IX; DEC IX` moves back 2 *bytes* = 1 entry in a table of
  // s16 words (the paired right-hand slot for this row). IXxpos -= 2 on a
  // s16* moves back 2 *elements* (4 bytes), landing one row too far.
  IXxpos -= 1;

  E = 0x1F;
  A = ((const u8 *) IXxpos)[1]; // buffer offset/distance
  if ((s8) A < 0)
    return;
  if (A == 0) {
    A = IXxpos[0] + Cdepth;
    if (A <= 255) {
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
  B = HLspan->nrows;
  Acopy -= B;
  if ((s8) Acopy < 0) {
    Acopy += B;
    B = Acopy;
  }
  DEsrc = HLspan->fill_bytes;
  // POP AF
  HLdst = ADDRTOBACKBUF((((A & 0x0F) + 0xF0) << 8) | ((A & 0x70) * 2 + Cparam));
  goto do_draw_span;

  for (;;) {
    DEsrc++;
    if (--B == 0) // iterations
      return;

do_draw_span:
    // Conv: state->do_span_width_words is the JR displacement self-modified
    // at $9116, which selects an entry point into the 30-write unrolled
    // fill loop ($9117-$9151, LD (HL),A / INC L pairs, 2 bytes each). A
    // larger displacement skips further into the loop, leaving FEWER writes
    // before the fixed end at $9151 — so the byte count is (30 - words/2),
    // not words/2. Using words/2 directly inverts the clip: as the deck
    // narrows towards the right edge (words grows), the span was drawn
    // *wider* instead of narrower, overrunning past the edge.
    memset(HLdst, *DEsrc, BRIDGE_DECK_LOOP_WRITES - state->do_span_width_words / 2);
    HLdst = ADDRTOBACKBUF(prev_buf_row(BACKBUFTOADDR(HLdst)));
  }
}

/**
 * $916C: Draw a left-side stretchy object (e.g. a tree)
 *
 * Entry point for left-side objects whose width scales with distance. Selects
 * draw_object_left_stretchy_entrypt as the per-segment drawing callback and
 * delegates to draw_stretchy_object_common.
 *
 * Conv: The Z80 stores the callback address ($9293) via self-modification at
 * $91CE/$9244; C passes it as an explicit function pointer.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bdepth Depth index of the object. (was B)
 * \param[in] DEarg Pointer to the stretchy_t data for this object. (was DE)
 * \param[in] IXxpos X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
void draw_stretchy_object_left(chqstate_t *state,
                               int         Bdepth,
                               const void *DEarg,
                               const s16  *IXxpos,
                               const u8   *IYheight)
{
  draw_stretchy_object_common(state,
                              Bdepth,
                              DEarg,
                              draw_object_left_stretchy_entrypt,
                              IXxpos,
                              IYheight);
}

/**
 * $9171: Draw a right-side stretchy object (e.g. a tree)
 *
 * Entry point for right-side objects whose width scales with distance. Selects
 * draw_object_right_stretchy_entrypt as the per-segment drawing callback and
 * delegates to draw_stretchy_object_common.
 *
 * Conv: The Z80 stores the callback address ($92FC) via self-modification at
 * $91CE/$9244; C passes it as an explicit function pointer.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bdepth Depth index of the object. (was B)
 * \param[in] DEarg Pointer to the stretchy_t data for this object. (was DE)
 * \param[in] IXxpos X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
void draw_stretchy_object_right(chqstate_t *state,
                                int         Bdepth,
                                const void *DEarg,
                                const s16  *IXxpos,
                                const u8   *IYheight)
{
  draw_stretchy_object_common(state,
                              Bdepth,
                              DEarg,
                              draw_object_right_stretchy_entrypt,
                              IXxpos,
                              IYheight);
}

/**
 * $9174: Draw stretchy objects
 *
 * Draws depth-scaled repeating scenery objects (trees, poles, signs) using a
 * variable-resolution representation called "stretchy" objects. Iterates
 * through a stretchy_t[] array; each entry supplies a depthset (a set of
 * bitmaps at different scales) and a type byte. Type 1 (STRETCHY_TYPE_END)
 * terminates the loop. Type 2 draws via SM_91CD_callback. Other types draw a
 * scaled single pixel column via SM_9244_callback with a derived vertical
 * height from persp_y_scale. SM_91BA_bitmap_offset and SM_91CD/9244_callback
 * are stored as SM fields to share state between this function and its
 * callbacks.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Bdepth Depth scale index; caps at DEPTHSET_MAX. (was B)
 * \param[in]     DEstretchy Pointer to the stretchy_t descriptor array.
 *   (was DE)
 * \param[in]     HLcallback Per-entry draw callback stored in SM fields.
 *   (was HL)
 * \param[in]     IXxpos Pointer into the xpos table for this object slot.
 *   (was IX)
 * \param[in]     IYheight Pointer into height_table for this object slot.
 *   (was IY)
 */
static void draw_stretchy_object_common(chqstate_t       *state,
                                        int               Bdepth,
                                        const stretchy_t *DEstretchy,
                                        dso_callback_t   *HLcallback,
                                        const s16        *IXxpos,
                                        const u8         *IYheight)
{
  dso_callback_t        *SM_91CD_callback;      /* SM: type-2 draw callback (was $91CD) */
  dso_callback_t        *SM_9244_callback;      /* SM: single-column draw callback (was $9244) */
  int                    A;                     /* scratch accumulator: type check and vertical scale (was A) */
  int                    SM_91DB_vertical;      /* SM: vertical scale value from persp_y_scale (was $91DB) */
  const stretchy_t      *HLstretchy;            /* pointer walking DEstretchy[] (was HL) */
  u16                    DEbitmapoffset;        /* (MIN(Bdepth,DEPTHSET_MAX) * 2 − 1): pair index (was DE) */
  int                    C_total;               /* running column total; negated into doc_col_pos (was C) */
  u16                    SM_91BA_bitmap_offset; /* SM: copy of DEbitmapoffset for use by callbacks (was $91BA) */
  int                    Bstretchy_type;        /* stretchy type byte minus 1; 0=end, 1=type2, else=column (was B) */
  const depthset_t      *DEdepthset;            /* depthset pointer from the current stretchy entry (was DE) */
  const depthset_t      *HLdepthset;            /* copy of DEdepthset after EX DE,HL (was HL) */
  const bitmap_t        *DEbitmap;              /* pointer to the depthset's bitmap array (was DE) */
  const depthset_pair_t *HLpair;                /* pointer to the chosen depth-set pair entry (was HL) */
  int                    Apairdepth;            /* depth value from the selected pair (was A) */
  u16                    HLoffset;              /* bitmap offset from the pair entry (was HL) */
  const bitmap_t        *HLbitmap;              /* pointer to the selected bitmap definition (was HL) */
  int                    Bcoltotal_delta;       /* amount added to C_total after each band's draw (was B) */
  int                    Bpairdepth;            /* copy of Apairdepth passed to SM_91CD_callback (was B) */
  int                    Avertical;             /* vertical scale from SM_91DB, halved for column height (was A) */
  int                    Bvertical;             /* half of Avertical: column height in pixels (was B) */

  // These just duplicate the HLcallback arg so could be removed in time.
  SM_91CD_callback = HLcallback;
  SM_9244_callback = HLcallback;

  SM_91DB_vertical = persp_y_scale[FAST_COUNTER_PERSP_ROW(state)][Bdepth];
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
    assert(DEbitmap);
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
    // Conv: $91C8 `LD B,(HL)` reads *before* the `DEC HL` x2 at $91C9-$91CA
    // that backs HL up to the bitmap struct's start (byte 0 = width_bytes)
    // for the callback call below. So at $91C8, HL still points at
    // struct_start + 2, which is the `height` field, not `width_bytes`.
    Bcoltotal_delta = HLbitmap->height;
    // PUSH (Bcoltotal_delta,C_total)
    Bpairdepth = Apairdepth;
    SM_91CD_callback(state, Bpairdepth, HLbitmap, IXxpos, IYheight);

dso_loop_continue:
    // POP (Bcoltotal_delta,C_total)
    C_total += Bcoltotal_delta;
    // POP IXxpos/HLstretchy
  }

  // EX AF,AF' -- save Adepth

  Avertical = SM_91DB_vertical;
  // Conv: Dispatch ladder converted to switch.
  // Bstretchy_type = data_type - 2 (two decrements already applied).
  switch (Bstretchy_type) {
  default:                      /* data type STRETCHY_TYPE_200PC (and any higher) */
  case 1: goto dso_case_150pc;  /* data type STRETCHY_TYPE_150PC */
  case 2: goto dso_case_50pc;   /* data type STRETCHY_TYPE_50PC */
  case 3: goto dso_case_113pc;  /* data type STRETCHY_TYPE_113PC */
  case 4: goto dso_case_38pc;   /* data type STRETCHY_TYPE_38PC */
  case 5: goto dso_case_75pc;   /* data type STRETCHY_TYPE_75PC */
  case 6: goto dso_case_25pc;   /* data type STRETCHY_TYPE_25PC */
  case 7: goto dso_continue;    /* data type STRETCHY_TYPE_100PC */
  }
  Avertical *= 2;               /* data type STRETCHY_TYPE_200PC: Z80 ADD A,A */
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
  if (Avertical <= 0)
    Avertical = 1;

  Bvertical = Avertical;
  // PUSH BC
  A = IYheight[53] + 1 - C_total - Bvertical;
  if (A < 0) // was carry
    Bvertical += A;

  state->doc_rows_main = Bvertical;
  // Conv: $9236 `LD A,(HL)` reads the same HLbitmap pointer as the Type 2
  // case above ($91C8), which is the bitmap struct's `height` field, not
  // `width_bytes` (see the Conv: comment at the Type 2 assignment). This is
  // the per-tile repeat height for the doc_rows_main/doc_rows_2nd split loop
  // in draw_object_clipped; reading width_bytes here made each repeat
  // section too short, doubling the visible repeat count at half height.
  state->doc_rows_2nd = HLbitmap->height;
  state->doc_plot_mode = 2; // inverted

  // EX AF,AF' -- restore Adepth

  Bpairdepth = Apairdepth;
  SM_9244_callback(state, Bpairdepth, HLbitmap, IXxpos, IYheight);
  Bcoltotal_delta = Bvertical; // $91D0-$91D3: POP BC restores saved Bvertical; C += B
  state->doc_plot_mode = 0; // reset
  goto dso_loop_continue;
}

/**
 * $924D: Draw a left-side tunnel light
 *
 * Entry point for tunnel lights on the left side of the road. Selects
 * draw_object_left_entrypt as the callback and delegates to
 * draw_tunnel_light_common.
 *
 * Conv: The Z80 stores the callback address ($9279) in HL then uses PUSH HL /
 * RET to dispatch; C passes it as an explicit function pointer.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bdepth Depth index of the light (0 = nearest). (was B)
 * \param[in] DEarg Depth-set pointer for the light object. (was DE)
 * \param[in] IXxpos X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
void draw_tunnel_light_left(chqstate_t *state,
                            int         Bdepth,
                            const void *DEarg,
                            const s16  *IXxpos,
                            const u8   *IYheight)
{
  draw_tunnel_light_common(state, Bdepth, DEarg, draw_object_left_entrypt, IXxpos,
                           IYheight);
}

/**
 * $9252: Draw a right-side tunnel light
 *
 * Entry point for tunnel lights on the right side of the road. Selects
 * draw_object_right_entrypt as the callback and delegates to
 * draw_tunnel_light_common.
 *
 * Conv: The Z80 stores the callback address ($92E2) in HL then uses PUSH HL /
 * RET to dispatch; C passes it as an explicit function pointer.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bdepth Depth index of the light (0 = nearest). (was B)
 * \param[in] DEarg Depth-set pointer for the light object. (was DE)
 * \param[in] IXxpos X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
void draw_tunnel_light_right(chqstate_t *state,
                             int         Bdepth,
                             const void *DEarg,
                             const s16  *IXxpos,
                             const u8   *IYheight)
{
  draw_tunnel_light_common(state, Bdepth, DEarg, draw_object_right_entrypt,
                           IXxpos, IYheight);
}

/**
 * $9255: Draw tunnel light common
 *
 * Shared core for draw_tunnel_light_left and draw_tunnel_light_right. Returns
 * immediately if the light's depth index is out of the visible range ($10).
 * Otherwise looks up the perspective scale for the current animation frame and
 * depth, derives a negative column offset from it, and invokes the left- or
 * right-specific drawing callback with that offset.
 *
 * The Z80 dispatches to the callback via PUSH HL / RET; here the callback is
 * passed explicitly.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bdepth Depth index of the light (0 = nearest). (was B)
 * \param[in] DEdepthset Depth-set table for the light object. (was DE)
 * \param[in] HLcallback Left or right object drawing entry point. (was HL)
 * \param[in] IXxpos X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
static void draw_tunnel_light_common(chqstate_t            *state,
                                     int                     Bdepth,
                                     const depthset_t      *DEdepthset,
                                     draw_object_entrypt_t *HLcallback,
                                     const s16              *IXxpos,
                                     const u8              *IYheight)
{
  int A; /* perspective-adjusted column offset passed to callback (was A) */

  if (Bdepth >= 16) /* light beyond visible depth */
    return;

  A = persp_y_scale[FAST_COUNTER_PERSP_ROW(state)][Bdepth];
  /* Conv: Z80 LD C,A; SRL C; SRL C; SUB C; NEG → -(A - A/4) = A/4 - A. */
  A = (A >> 2) - A;

  HLcallback(state, A, Bdepth, DEdepthset, IXxpos, IYheight);
}

/**
 * $9278: Draw a left-side road object with no column offset
 *
 * Entry point used by the object dispatch table for standard left-side scenery
 * objects (turn signs, lamp posts, etc.). Calls draw_object_left_entrypt with a
 * column offset of zero.
 *
 * Conv: The Z80 uses XOR A (A = 0) then falls through to $9279
 * (draw_object_left_entrypt); C calls it directly with col_offset = 0.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bdepth Depth index of the object. (was B)
 * \param[in] DEdepthset Pointer to the depthset_t for this object. (was DE)
 * \param[in] IXxpos X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
void draw_object_left(chqstate_t *state,
                      int         Bdepth,
                      const void *DEdepthset,
                      const s16  *IXxpos,
                      const u8   *IYheight)
{
  draw_object_left_entrypt(state, 0, Bdepth, DEdepthset, IXxpos, IYheight);
}

/**
 * $9279: Resolve depth and bitmap for a left-side object, then draw it
 *
 * Stores the column offset, caps the depth index to DEPTHSET_MAX, then looks up
 * the depth value and bitmap pointer from the depthset table and dispatches to
 * draw_object_left_stretchy_entrypt.
 *
 * In the Z80, this function falls through to the same entrypt after computing
 * the depth/bitmap via EX DE,HL and indexed HL arithmetic. The col_offset (A)
 * is stored to state->doc_col_pos via a self-modified `LD D,x` at $933D; C
 * writes it to doc_col_pos directly.
 *
 * Conv: Z80 self-modifies the operand at $933E to hold [Acol_offset] (later
 * read as D in draw_object_common); C stores it in state->doc_col_pos. Conv:
 * Z80 index arithmetic `HL += A*2−1` / byte reads replaced by struct field
 * access on depthset_t. The Z80 indexing is 1-based (A=1 → pair[0]); C uses
 * pairs[Bdepth-1] to match.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Acol_offset Column offset added to the object position. (was A)
 * \param[in] Bdepth Depth index into the depthset table. (was B)
 * \param[in] DEdepthset Pointer to the depthset for this object. (was DE)
 * \param[in] IXxpos X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
static void draw_object_left_entrypt(chqstate_t       *state,
                                     int               Acol_offset,
                                     int               Bdepth,
                                     const depthset_t *DEdepthset,
                                     const s16        *IXxpos,
                                     const u8         *IYheight)
{
  const depthset_t *ds;      /* depthset pointer from DEdepthset arg (was HL) */
  const bitmap_t   *bitmaps; /* bitmap array base from ds->bitmaps (was DE) */
  int               depth;   /* depth value at pairs[Bdepth] (was B) */
  const bitmap_t   *bitmap;  /* selected bitmap within bitmaps (was HL) */

  state->doc_col_pos = Acol_offset;

  if (Bdepth >= DEPTHSET_MAX)
    Bdepth = DEPTHSET_MAX;

  ds = DEdepthset; /* EX DE,HL */

  bitmaps = ds->bitmaps;
  // Conv: Z80 uses A*2-1 indexing (1-based); pairs[Bdepth-1] corrects for C's 0-based array.
  depth   = ds->pairs[Bdepth - 1].depth;
  bitmap  = &bitmaps[ds->pairs[Bdepth - 1].offset / 7];

  draw_object_left_stretchy_entrypt(state, depth, bitmap, IXxpos, IYheight);
}

/**
 * $9293: Compute left-side screen width then draw the object
 *
 * Adds 16 to the x-position table value, subtracts the depth, and returns early
 * if the result is negative (object entirely off-screen to the left). Otherwise
 * passes the remaining screen width to draw_object_left_width_entrypt.
 *
 * \param[in] state Pointer to game state.
 * \param[in] B_depth Depth value from the depthset, subtracted from width.
 *   (was B)
 * \param[in] HL_bitmap Source bitmap data. (was HL)
 * \param[in] IX_xpos X-position table pointer. (was IX)
 * \param[in] IY_height Height table pointer. (was IY)
 */
static void draw_object_left_stretchy_entrypt(chqstate_t     *state,
                                              int             B_depth,
                                              const bitmap_t *HL_bitmap,
                                              const s16      *IX_xpos,
                                              const u8       *IY_height)
{
  int A_width_bytes; /* available screen width = IX[0] + 16 − depth (was A) */

  A_width_bytes = IX_xpos[0] + 16;
  if (A_width_bytes >= B_depth)
    draw_object_left_width_entrypt(state, A_width_bytes - B_depth, HL_bitmap, IY_height); /* was FALLTHROUGH */
}

/**
 * $929A: Draw object — left-side clipping entry point
 *
 * Entry point for objects that arrive at the left-hand edge of the screen.
 * Returns immediately if [Awidth_bytes] < 8 (object fully off-screen left).
 * Otherwise computes the visible column count and padding from the bitmap width
 * and the clipping width, sets doc_shift_select, and dispatches to
 * draw_object_clipped or draw_object_common_flipped depending on the
 * BITMAPFLAG_FLIPPED bit.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Awidth_bytes Available pixel width at the left edge. (was A)
 * \param[in]     HLbitmap Pointer to the bitmap descriptor. (was HL)
 * \param[in]     IYheight Pointer into height_table for this object slot.
 *   (was IY)
 */
static void draw_object_left_width_entrypt(chqstate_t     *state,
                                           int             Awidth_bytes,
                                           const bitmap_t *HLbitmap,
                                           const u8       *IYheight)
{
  int zero;           /* non-zero when bitmap is NOT flipped (BITMAPFLAG_FLIPPED clear) (was Z) */
  int carry;          /* non-zero when bitmap uses a mask (BITMAPFLAG_MASKED set) (carry) */
  int Cpadding;       /* byte padding between visible and full-width columns (was C) */
  int Ebitmap_stride; /* full bitmap row stride in bytes from HLbitmap->width_bytes (was E) */
  int Bheight;        /* height correction: 1 in both paths; decremented in flipped path (was B) */
  int Dwidth_bytes;   /* visible width saved while padding is computed (was D) */
  int Adash;          /* Awidth_bytes banked for EX AF,AF' into flipped path (was A') */
  int Fdash_zero;     /* zero flag banked for EX AF,AF' into flipped path (was Z in F') */
  int Fdash_carry;    /* carry banked for EX AF,AF' into flipped path (was carry in F') */

  if (Awidth_bytes < 8)
    return;

  Cpadding = 0;
  Ebitmap_stride = HLbitmap->width_bytes << 3;
  Awidth_bytes -= Ebitmap_stride;
  if (Awidth_bytes >= 0) {
    if (Awidth_bytes >= 8) {
      draw_object_perspective_entrypt(state, Awidth_bytes, Cpadding, HLbitmap, IYheight); /* tail call */
      return;
    }

    Ebitmap_stride = HLbitmap->width_bytes;
    // Conv: Z80 $92AE-$92AF is RRCA RRCA on the raw (unmasked) byte, not a
    // shift -- unlike the negative-width branch below, which ANDs off the low
    // 2 bits with $FC before rotating. Here bits rotated off the bottom wrap
    // into bits 7-6, so a plain >>= 2 silently drops them instead of setting
    // them (pitfall: rotate mistranslated as shift).
    Awidth_bytes = ((Awidth_bytes >> 2) | (Awidth_bytes << 6)) & 0xFF; /* RRCA x2 */
    state->doc_shift_select = Awidth_bytes;
    Awidth_bytes = Ebitmap_stride - 1;
    // Conv: if width_bytes == 1, Awidth_bytes == 0 → nothing to draw.
    if (Awidth_bytes == 0)
      return;

    Bheight = 1;
    Cpadding = 1;
  } else {
    Ebitmap_stride = HLbitmap->width_bytes;
    Awidth_bytes = ((u8)Awidth_bytes & 0xFC) >> 2;
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
                        IYheight); /* tail call */
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
                               IYheight); /* tail call */
  }
}

/**
 * $92E1: Draw a right-side road object with no column offset
 *
 * Entry point used by the object dispatch table for standard right-side scenery
 * objects. Calls draw_object_right_entrypt with a column offset of zero.
 *
 * Conv: The Z80 uses XOR A (A = 0) then falls through to $92E2
 * (draw_object_right_entrypt); C calls it directly with col_offset = 0.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bdepth Depth index of the object. (was B)
 * \param[in] DEdepthset Pointer to the depthset_t for this object. (was DE)
 * \param[in] IXxpos X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
void draw_object_right(chqstate_t *state,
                       int         Bdepth,
                       const void *DEdepthset,
                       const s16  *IXxpos,
                       const u8   *IYheight)
{
  draw_object_right_entrypt(state, 0, Bdepth, DEdepthset, IXxpos, IYheight);
}

/**
 * $92E2: Resolve depth and bitmap for a right-side object, then draw it
 *
 * Right-side mirror of draw_object_left_entrypt ($9279). Stores the column
 * offset, caps the depth index and looks up the depth value and bitmap pointer
 * from the depthset table before calling draw_object_right_stretchy_entrypt.
 *
 * Conv: Z80 self-modifies the operand at $933E to hold [Acol_offset]; C writes
 * it to state->doc_col_pos. Conv: Z80 index arithmetic `HL += A*2−1` / byte
 * reads replaced by struct field access on depthset_t. The Z80 indexing is
 * 1-based (A=1 → pair[0]); C uses pairs[Bdepth-1] to match.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Acol_offset Column offset added to the object position. (was A)
 * \param[in] Bdepth Depth index into the depthset table. (was B)
 * \param[in] DEdepthset Pointer to the depthset for this object. (was DE)
 * \param[in] IXxpos X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
static void draw_object_right_entrypt(chqstate_t       *state,
                                      int               Acol_offset,
                                      int               Bdepth,
                                      const depthset_t *DEdepthset,
                                      const s16        *IXxpos,
                                      const u8         *IYheight)
{
  const depthset_t *ds;      /* depthset pointer from DEdepthset arg (was HL) */
  const bitmap_t   *bitmaps; /* bitmap array base from ds->bitmaps (was DE) */
  int               depth;   /* depth value at pairs[Bdepth] (was B) */
  const bitmap_t   *bitmap;  /* selected bitmap within bitmaps (was HL) */

  state->doc_col_pos = Acol_offset;

  if (Bdepth >= DEPTHSET_MAX)
    Bdepth = DEPTHSET_MAX;

  ds = DEdepthset; /* EX DE,HL */

  bitmaps = ds->bitmaps;
  // Conv: Z80 uses A*2-1 indexing (1-based); pairs[Bdepth-1] corrects for C's 0-based array.
  depth   = ds->pairs[Bdepth - 1].depth;
  bitmap  = &bitmaps[ds->pairs[Bdepth - 1].offset / 7];

  draw_object_right_stretchy_entrypt(state, depth, bitmap, IXxpos, IYheight);
}

/**
 * $92FC: Compute right-side available width then draw the object
 *
 * Reads the x-position table value and adds the (signed) depth. Returns early
 * if the result is zero or overflows u8 (i.e. the object is off-screen to the
 * right). Otherwise passes the remaining screen width to
 * draw_object_right_width_entrypt.
 *
 * Z80 flow: `BIT 7,B; JR Z,$9306` — positive B jumps to $9306 which does `ADD
 * A,B; RET C` (return on u8 carry, i.e. A+B > 255); negative B path ($9303)
 * adds without a carry check. Both paths share `RET Z` at $9308.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bdepth Signed depth value from the depthset. (was B)
 * \param[in] HLbitmap Source bitmap data. (was HL)
 * \param[in] IXxpos X-position table pointer. (was IX)
 * \param[in] IYheight Height table pointer. (was IY)
 */
static void draw_object_right_stretchy_entrypt(chqstate_t     *state,
                                               int             Bdepth,
                                               const bitmap_t *HLbitmap,
                                               const s16      *IXxpos,
                                               const u8       *IYheight)
{
  int A_width_bytes; /* available screen width = IX[0] ± depth (was A) */

  A_width_bytes = IXxpos[0];
  if ((s8) Bdepth < 0) {
    A_width_bytes += Bdepth;         /* negative Bdepth, no carry check */
  } else {
    A_width_bytes += Bdepth;
    if (A_width_bytes > 255) return;
  }
  if (A_width_bytes)
    draw_object_right_width_entrypt(state, A_width_bytes, HLbitmap, IYheight);
}

/**
 * $9309: Guard for right-side perspective draw: skip if width >= 247
 *
 * Returns immediately if [Awidth_bytes] >= 247 ($F7), indicating the object is
 * at least partly off the right edge of the screen. Otherwise falls through to
 * draw_object_perspective_entrypt with a zero padding value.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Awidth_bytes Available screen width for the object. (was A)
 * \param[in] HLbitmap Source bitmap data. (was HL)
 * \param[in] IYheight Height table pointer. (was IY)
 */
static void draw_object_right_width_entrypt(chqstate_t     *state,
                                            int             Awidth_bytes,
                                            const bitmap_t *HLbitmap,
                                            const u8       *IYheight)
{
  if (Awidth_bytes < 247)
    draw_object_perspective_entrypt(state, Awidth_bytes, 0, HLbitmap, IYheight);
}

/**
 * $930E: Draw object — perspective height/width entry point
 *
 * Derives the height and draw-width for a depth-scaled object from the clipping
 * width [Awidth_bytes]. Right-shifts by 2 (then 1 more) to get the height
 * index, stores it in doc_shift_select, and computes the visible column count
 * as MAX(width_bytes, 31 − height). Dispatches to draw_object_clipped or
 * draw_object_common_flipped depending on the BITMAPFLAG_FLIPPED bit.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Awidth_bytes Clipping pixel width from the xpos table. (was A)
 * \param[in]     Cpadding Byte padding between visible and full columns.
 *   (was C)
 * \param[in]     HLbitmap Pointer to the bitmap descriptor. (was HL)
 * \param[in]     IYheight Pointer into height_table for this object slot.
 *   (was IY)
 */
static void draw_object_perspective_entrypt(chqstate_t     *state,
                                            int             Awidth_bytes,
                                            int             Cpadding,
                                            const bitmap_t *HLbitmap,
                                            const u8       *IYheight)
{
  int carry;             /* carry from RRA steps; always 0 at entry (carry) */
  int Bheight;           /* height index: Awidth_bytes >> 3, used as doc_shift_select (was B) */
  int Ebitmap_stride;    /* full bitmap row stride from HLbitmap->width_bytes (was E) */
  int Zflipped;          /* non-zero when bitmap is NOT flipped (BITMAPFLAG_FLIPPED clear) (was Z) */
  int Cwidth_bytes;      /* computed draw width: MIN(Ebitmap_stride, 31 − Bheight) (was C) */
  int Adash_width_bytes; /* Cwidth_bytes banked for EX AF,AF' into flipped path (was A') */
  int Fdash_zero;        /* Zflipped banked for EX AF,AF' (was Z in F') */
  int Fdash_carry;       /* carry banked for EX AF,AF' (was carry in F') */

  Awidth_bytes >>= 2; /* was AND-RRCA-RRCA */
  state->doc_shift_select = Awidth_bytes;
  Awidth_bytes >>= 1; /* was RRA */
  Bheight = Awidth_bytes;
  Ebitmap_stride = HLbitmap->width_bytes;
  Awidth_bytes = 31 - Awidth_bytes;
  if (Ebitmap_stride < Awidth_bytes)
    Awidth_bytes = Ebitmap_stride;
  // Conv: Bheight=31 gives Awidth_bytes=0 (no columns to draw); Z80 executes a
  // zero-width sprite which reads past the jump table — skip it.
  if (Awidth_bytes <= 0)
    return;
  // Conv: HLbitmap++ removed, now passed as-is into draw_object_common/_9333
  Zflipped = (HLbitmap->flags >> 1) == 0;
  carry = HLbitmap->flags & BITMAPFLAG_MASKED;
  if (Zflipped) {
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
    Adash_width_bytes = Awidth_bytes; Fdash_zero = Zflipped;
    Fdash_carry = carry; // was EX AF,AF' -- banking A & carry
    Cpadding = Ebitmap_stride - Cwidth_bytes;
    draw_object_common_flipped(state,
                               Bheight,
                               Cpadding,
                               Ebitmap_stride,
                               HLbitmap,
                               Adash_width_bytes,
                               Fdash_zero,
                               Fdash_carry,
                               IYheight); /* was FALLTHROUGH */
  }
}

/**
 * $932B: Draw a flipped object, A_toggling the shift selector
 *
 * Entry point for horizontally-flipped objects. Inverts the shift-selector byte
 * so that the complementary (shifted) bitmap variant is chosen, then unbanks
 * the width and flag values passed through the shadow registers before
 * delegating to draw_object_clipped ($9333).
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bheight Initial height parameter from caller. (was B)
 * \param[in] Cpadding Padding bytes between bitmap rows. (was C)
 * \param[in] Ebitmap_stride Full stride of one bitmap row, in bytes. (was E)
 * \param[in] HLbitmap Pointer to the bitmap descriptor. (was HL)
 * \param[in] Adash_width_bytes Draw width of bitmap, in bytes. (was A')
 * \param[in] Fdash_zero Non-zero if the object is NOT flipped. (was Z in F')
 * \param[in] Fdash_carry Non-zero if the bitmap uses a mask. (was carry in F')
 * \param[in] IYheight Pointer to the Y-height table entry for this
 * object slot. (was IY)
 */
static void draw_object_common_flipped(chqstate_t     *state,
                                       int             Bheight,
                                       int             Cpadding,
                                       int             Ebitmap_stride,
                                       const bitmap_t *HLbitmap,
                                       int             Adash_width_bytes,
                                       int             Fdash_zero,
                                       int             Fdash_carry,
                                       const u8       *IYheight)
{
  int Awidth_bytes; /* draw width in bytes, unbanked from shadow (was A) */
  int zero;         /* not-flipped flag, unbanked from shadow F' (was Z flag) */
  int carry;        /* masked flag, unbanked from shadow F' (was carry) */

  // Conv: Z80 reads/complements/writes the byte at $9396 directly;
  //       here we complement the state field that models that byte.
  state->doc_shift_select = ~state->doc_shift_select;

  // EX AF,AF' - unbank A' and F' into main registers
  Awidth_bytes = Adash_width_bytes;
  zero         = Fdash_zero;
  carry        = Fdash_carry;

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
 * $9333: Clip object to visible area and dispatch to plot function
 *
 * Determines how many rows of the bitmap are visible given the object's
 * position in the Y-height table, skips invisible leading rows, then dispatches
 * to the appropriate sprite-plot routine (masked/unmasked,
 * normal/flipped/inverted, even/odd width).
 *
 * \param[in] state Pointer to game state.
 * \param[in] zero_flipped Non-zero if the object is NOT flipped. (was Z flag)
 * \param[in] carry_masked_flag Non-zero if the bitmap uses a transparency mask.
 *   (was C flag / carry)
 * \param[in] A_width_bytes Draw width of bitmap, in bytes. (was A)
 * \param[in] B_height Initial height parameter from caller. (was B)
 * \param[in] Cpadding Padding bytes between bitmap rows. (was C)
 * \param[in] Ebitmap_stride Full stride of one bitmap row, in bytes. (was E)
 * \param[in] HLbitmap Pointer to the bitmap descriptor. (was HL)
 * \param[in] IYheight Pointer to the Y-height table entry for this
 * object slot. (was IY)
 */
static void draw_object_clipped(chqstate_t     *state,
                                int             zero_flipped,
                                int             carry_masked_flag,
                                int             A_width_bytes,
                                int             B_height,
                                int             Cpadding,
                                int             Ebitmap_stride,
                                const bitmap_t *HLbitmap,
                                const u8       *IYheight)
{
  int               carry;               /* carry flag */
  int               D_col_pos;           /* self-modified col position ($933D) (was D) */
  int               Adash_y_range;       /* IY[0]-IY[$35]: vertical range of object (was A') */
  int               Adash_y_pos;         /* working row-position accumulator (was A') */
  int               Adash_clip_rows;     /* height - 1 - y_range: rows clipped (was A') */
  int               Adash_col_adj;       /* col_pos adjusted for bitmap height (was A') */
  int               D_height;            /* bitmap height in rows (was D) */
  int               D_y_range;           /* y-range difference used for row skip (was D) */
  int               Ay_remaining;        /* remaining rows after adjustment (was A) */
  int               BC_padding;          /* row padding bytes, B=0 (was BC) */
  const u8         *HL_bitmap_data;      /* pointer into selected bitmap row data (was HL) */
  int               Adash_row_skip;      /* rows to skip before drawing (was A') */
  int               Bdash_height;        /* banked draw height for row loop (was B') */
  int               Adash_inverted;      /* doc_plot_mode value for dispatch (was A') */
  int               IX_jump_offset;      /* jump-table offset into plot routine (was IX) */
  plot_sprite_cb_t *BCdash_callback;     /* banked pointer to plot callback (was BC') */
  int               DE_bitmap_stride;    /* bitmap stride, widened to int (was DE) */
  int               Adash_y_pos_pushed;  /* banked row base, preserved across clipping (was A') */
  u8               *HLdash_backbuf_addr; /* back-buffer write address (was HL') */
  u8                D_clip_rows;
  u16               BC_bitmap_stride;
  u8                B_clip_rows;

  if (carry_masked_flag) {
    // EX AF,AF'  - preserve carry while we double these args (since they're masked)
    Cpadding *= 2;
    Ebitmap_stride *= 2;
    // EX AF,AF'  - restore
  }

  // EX AF,AF'  - bank A_width_bytes & carry_masked_flag
  // INC HL  - point HL at height field -- removed

  // Loop until ... <what condition?>
  for (;;) {
    D_col_pos = state->doc_col_pos;
    Adash_y_range = IYheight[0] - IYheight[53];
    if (Adash_y_range == 0) {
doc_y_range_is_zero:
      Adash_clip_rows = IYheight[53] + D_col_pos;
      if ((s8) Adash_clip_rows < 0)
        return;

      Adash_y_pos_pushed = Adash_clip_rows; // PUSH AF  -- push Adash_y_pos (& flags)
      Adash_clip_rows++;

      D_height = HLbitmap->height; /* Conv: HLbitmap adjusted, this loads from bitmap.height */
      // Conv: Z80 $934F SUB D; JR C,$9353 tests unsigned borrow (was
      // Adash_clip_rows < D_height before the subtraction), not the sign of
      // the wrapped 8-bit difference. Close-up objects give height values
      // large enough that (s8)(Adash_clip_rows - D_height) does not track
      // true borrow (pitfall #8) -- use a direct unsigned comparison, giving
      // Adash_clip_rows = min(Adash_clip_rows, D_height).
      if ((u8) Adash_clip_rows >= (u8) D_height)
        Adash_clip_rows = D_height;
      // INC HL  - point HL at data field -- removed
      D_y_range = 1;
      goto doc_compute_bitmap;
    }

doc_y_range_nonzero:
    // Conv: Z80 $9359 BIT 7,D; JR NZ,$9361 selects ADD A,D (D negative) or
    // SUB D (D positive/zero) -- net effect is always A -= |D|. doc_col_pos
    // (D_col_pos) is s8 and CAN be positive: draw_hazard_sprites sets it to
    // persp_col - hit_wobble ($AF04-$AF0A), which is commonly positive for
    // cars and hazards. A plain `+= D_col_pos` only matches the Z80 for D_col_pos <= 0;
    // for positive D_col_pos it added instead of subtracted, inflating
    // Adash_y_range and pushing close-range objects into the "inverted"
    // re-clip path below, which then chops rows off their top.
    if (D_col_pos < 0)
      Adash_y_range += D_col_pos;
    else
      Adash_y_range -= D_col_pos;

    D_y_range = Adash_y_range;
    if ((s8) Adash_y_range <= 0)
      goto doc_y_range_is_zero;

    Adash_y_pos = IYheight[53];
    Adash_y_pos_pushed = Adash_y_pos; // PUSH AF  -- push Adash_y_pos (& flags)

    // Conv: $936F SUB D; $9370 JP NC tests true unsigned borrow (whether
    // height-1 < D_y_range), not the sign of the wrapped 8-bit difference --
    // same pitfall as the zero-y-range branch's clamp above ($9350). Close-up
    // percentage bands push HLbitmap->height past 127, at which point
    // (s8)(height-1-D_y_range) stops tracking the true borrow and this branch
    // wrongly falls into the "inverted" re-clip path instead of breaking out
    // to draw, chopping rows off the top of the band.
    D_height = HLbitmap->height; /* Conv: HLbitmap adjusted, this loads from bitmap.height */
    if ((u8) (D_height - 1) >= (u8) D_y_range) {
      Adash_clip_rows = D_height - 1 - D_y_range;
      break;
    }

    // POP AF - discard Adash_y_pos_pushed

    if (state->doc_plot_mode == 0) // set to 0 or 2
      return;

    // inverted

    Adash_col_adj = state->doc_col_pos;
    D_height = HLbitmap->height; // reads bitmap.height again
    Adash_col_adj -= D_height;
    if ((s8) Adash_col_adj >= 0) // need this cast?
      return;
    state->doc_col_pos = Adash_col_adj;

    // $9382
    Ay_remaining = state->doc_rows_main - D_height;
    if (Ay_remaining <= 0)
      return;
    state->doc_rows_main = Ay_remaining;
  }

  // AF (Adash_y_pos & flags) is still pushed when the loop exits here.

  Adash_clip_rows++;
  D_y_range++;
  // INC HL  - point HL at data field -- removed

  // $9390
doc_compute_bitmap:
  // PUSH BC - store B_height & Cpadding
  // PUSH DE - store Diy_diff & Ebitmap_stride
  D_clip_rows = Adash_clip_rows;
  BC_padding = Cpadding; // widen. was B = 0
  HL_bitmap_data = (state->doc_shift_select & 1) ? HLbitmap->shifted : HLbitmap->data;
  // was HL += 2; Conv: removed - points HL at bitmap.shifted

  // $939C
  HL_bitmap_data += BC_padding; BC_padding = 0xAAAA; // deliberately wipe
  state->doc_bitmap_ptr = HL_bitmap_data;
  BC_bitmap_stride = Ebitmap_stride; // was LD C,E (B is zero)
  Adash_row_skip = D_y_range - 1; // was POP AF restoring PUSH DE, then DEC A

  // $93A9 - skip leading rows
  HL_bitmap_data += BC_bitmap_stride * Adash_row_skip; // Conv: was multiply-increment routine

  // $93AE
  B_clip_rows = D_clip_rows; // LD B,D
  DE_bitmap_stride = Ebitmap_stride; // widen

  // $93B1 - EXX - BANK - first banking op in this routine

  // Conv: B_height = 0 from perspective calc means Awidth_bytes was in [1,3]; the Z80
  // would wrap B to 255 and plot 256 garbage rows — treat as nothing to draw.
  if (B_height <= 0)
    return;

  // POP BC - restoring B_height & Cpadding to Bdash and Cdash
  Bdash_height = B_height;
  // Cdash_padding = Cpadding; // restore if we find this is used
  // Conv: $93AE/$93B1 bank B_clip_rows into shadow B' before the POP BC above
  // restores the unrelated B_height/Cpadding pair into main BC. Every plot
  // dispatched below reads its row count from shadow B', i.e. B_clip_rows,
  // not from B_height — Bdash_height (unclamped B_height) is used only for
  // the backbuf address calculation just below. A previous translation
  // instead clamped B_height to B_clip_rows (min of the two), which is only
  // correct when B_height > B_clip_rows; whenever B_height <= B_clip_rows
  // (the common case for perspective-derived and fixed-height=1 callers)
  // it wrongly drew B_height rows instead of B_clip_rows, truncating the
  // top of tall/close objects. This mirrors the fix already applied to the
  // doc_masked_rows loop below (see its comment at $945C).
  B_height = B_clip_rows;
  // POP AF - restoring Adash_y_pos_pushed & flags

  // $93B4
  // 0b_1111_LLLL_RRRC_CCCC so A holds ?RRRLLLL and B holds ???CCCCC
  HLdash_backbuf_addr = OFFSETTOBACKBUF(
      ((Adash_y_pos_pushed & 0x0F) << 8) |
      ((((Adash_y_pos_pushed & 0x70) << 1) + Bdash_height) & 0xFF)); // Conv: $93BE ADD A,B wraps
                                                                      // 8-bit; mask before OR'ing
                                                                      // into high byte
  assert(VALID_BACKBUF_PTR(HLdash_backbuf_addr));

  // $93C0
  Adash_inverted = state->doc_plot_mode; // set to 0 or 2

  if (Adash_inverted) {
    if (--Adash_inverted)
      goto doc_unmasked_rows;

    // $93D0 - EX AF,AF'  -- unbank flags (carry => masked flagz) and A_width_bytes

    if (carry_masked_flag) {
      plot_masked_sprite_inverted(state,
                                  A_width_bytes,
                                  HLdash_backbuf_addr,
                                  B_height,
                                  DE_bitmap_stride,
                                  HL_bitmap_data); /* tail call */
      return;
    } else {
      goto unmasked_inverted;
    }
  }

  // EX AF,AF'  -- unbank flags (carry => masked) and A_width_bytes

  if (zero_flipped) { // Z set if flipped
    if (carry_masked_flag) {
      draw_part_plot_masked_sprite(state,
                                   A_width_bytes,
                                   HLdash_backbuf_addr,
                                   B_height,
                                   DE_bitmap_stride & 0xFF, /* Conv: Original only used E' */
                                   HL_bitmap_data); /* tail call */
    } else {
      plot_sprite(state,
                  A_width_bytes,
                  HLdash_backbuf_addr,
                  B_height,
                  DE_bitmap_stride,
                  HL_bitmap_data); /* tail call */
    }
  } else {
    if (carry_masked_flag) {
      plot_masked_sprite_flipped(state,
                                 A_width_bytes,
                                 HLdash_backbuf_addr,
                                 B_height,
                                 DE_bitmap_stride,
                                 HL_bitmap_data); /* tail call */
    } else {
      plot_sprite_flipped(state,
                          A_width_bytes,
                          HLdash_backbuf_addr,
                          B_height,
                          DE_bitmap_stride,
                          HL_bitmap_data); /* tail call */
    }
  }
  return; /* all possible paths prior are tail call */

doc_unmasked_rows:
  // EX AF,AF'  -- unbank flags (carry => masked) and A_width_bytes
  if (carry_masked_flag)
    goto doc_masked_rows;
  SRL(A_width_bytes); // carry set if odd
  if (carry)
    goto doc_unmasked_odd;

  IX_jump_offset = (4 - A_width_bytes) * 5;
  BCdash_callback = plot_sprite_even;

doc_set_callbacks:
  state->doc_plot_fn   = BCdash_callback;
  state->doc_plot_fn_2 = BCdash_callback;

  // EXX - UNBANK

  // Conv: $9404–$941D is a loop (back-edge at $9417 JP $9404), structurally
  // identical to the doc_masked_rows loop below.  Each iteration subtracts the
  // current section height (B = B_clip_rows, then doc_rows_2nd) from
  // doc_rows_main; exits via $9407 JR Z / $9409 JR C when remaining ≤ 0.
  // $941A ADD A,B recovers the final (partial) section height; $941D JP falls
  // through to doc_plot_fn_2.  B' (= D_draw_height, banked) carries the actual
  // row count into each plot call; main B ($941B LD B,A) is for the back-buffer
  // address only.
  BC_padding = B_clip_rows; /* $9403 EXX: MAIN B = B_clip_rows for first iteration */
  for (;;) {
    A_width_bytes = state->doc_rows_main - BC_padding; // $9404/$9406
    if (A_width_bytes > 0) {                           // $9407 JR Z / $9409 JR C
      state->doc_rows_main = A_width_bytes;         // $940B: update SM
      // Conv: the Z80 leaves the advanced backbuffer address in shadow HL'
      // across the CALL (it is never reloaded from a fixed value); the C
      // callback returns it explicitly so it carries into the next iteration.
      // Conv: the row count drawn each call is the section height just
      // subtracted (BC_padding), not the outer B_height — mirrors the
      // doc_masked_rows loop below, which reuses the same variable for both
      // roles instead of a separate constant.
      HLdash_backbuf_addr = state->doc_plot_fn(state,
                         IX_jump_offset,
                         HLdash_backbuf_addr,
                         BC_padding,
                         DE_bitmap_stride,
                         HL_bitmap_data);           // $940F: CALL doc_plot_fn
      HL_bitmap_data = state->doc_bitmap_ptr;       // $9412: HL = SM bitmap ptr
      BC_padding = state->doc_rows_2nd;             // $9415: B = SM doc_rows_2nd
    } else {
      A_width_bytes += BC_padding;                   // $941A: A += B
      break;
    }
  }
  Bdash_height = A_width_bytes;                     // $941B: B = A
  // $941C EXX: B' = remaining rows (= Bdash_height); $941D JP doc_plot_fn_2
  state->doc_plot_fn_2(state,
                       IX_jump_offset,
                       HLdash_backbuf_addr,
                       Bdash_height,
                       DE_bitmap_stride,
                       HL_bitmap_data); /* tail call $941D */
  return;

doc_unmasked_odd:
  // this will need to become a jump offset and a new entry point for plot_sprite_odd is needed
  IX_jump_offset = 5 * (3 - A_width_bytes);
  BCdash_callback = plot_sprite_odd_entrypt;
  goto doc_set_callbacks;

doc_masked_rows:
  IX_jump_offset = (8 - A_width_bytes) * 6; // plot_masked_sprite needs * 6
  state->doc_mask_bitmap_ptr = state->doc_bitmap_ptr;

  state->doc_mask_rows_2nd  = state->doc_rows_2nd;
  state->doc_mask_rows_main = state->doc_rows_main;

  // Bdash = 0xF; // mask [commented out - I can't see why this exists]

  // EXX - UNBANK

  DE_bitmap_stride &= 0xFF; // clear top of DEbitmap_stride
  // Conv: $9442 `LD B,$00` (masked jump-table offset scratch) clobbers the
  // B_height that was popped into this bank at $93B2 before it is ever read;
  // the value the real Z80 actually uses for the first iteration below is
  // B_clip_rows, banked away at $93AE and restored by the $945C EXX -- this
  // mirrors the unmasked loop above, which explicitly seeds its first
  // iteration with `BC_padding = B_clip_rows`. Using the raw B_height
  // parameter here (unclipped) skipped the first-section split for
  // tall/close objects, truncating their top rows.
  B_height = B_clip_rows; /* $945C EXX: MAIN B = B_clip_rows for first iteration */
  for (;;) {
    Ay_remaining = state->doc_mask_rows_main - B_height;
    if (Ay_remaining > 0) {
      state->doc_mask_rows_main = Ay_remaining;

      // Conv: same backbuffer-address carry as the unmasked repeat loop
      // above (see the doc_plot_fn comment) — must be captured across calls.
      HLdash_backbuf_addr = plot_masked_sprite(state,
                         IX_jump_offset,
                         B_height,
                         DE_bitmap_stride,
                         HL_bitmap_data,
                         HLdash_backbuf_addr);

      HL_bitmap_data = state->doc_mask_bitmap_ptr;
      B_height = state->doc_mask_rows_2nd;
    } else {
      B_height += Ay_remaining;
      plot_masked_sprite(state,
                         IX_jump_offset,
                         B_height,
                         DE_bitmap_stride,
                         HL_bitmap_data,
                         HLdash_backbuf_addr); /* tail call */
      return;
    }
  }

unmasked_inverted:
  DE_bitmap_stride = Ebitmap_stride;
  HL_bitmap_data += (B_height - 1) * DE_bitmap_stride;
  DE_bitmap_stride = -DE_bitmap_stride;

  plot_sprite(state,
              A_width_bytes,
              HLdash_backbuf_addr,
              B_height,
              DE_bitmap_stride,
              HL_bitmap_data); /* was FALLTHROUGH */
}

/**
 * $949C: Plot an unmasked sprite to the back buffer
 *
 * Dispatches to the odd- or even-width inner loop based on whether
 * [width_bytes] is odd or even. For odd widths the low bit is the remainder
 * byte after dividing by 2; for even widths a jump-table offset selects where
 * in the unrolled POP loop to enter.
 *
 * Conv: Z80 uses `SRL A` to test the carry (bit 0 of A) and halve A
 * simultaneously; C uses a bit test and a right shift separately. Conv: Row
 * advance is via DEC H with a multi-step rollover in the Z80; C delegates to
 * prev_buf_row().
 *
 * \param[in] state Pointer to game state.
 * \param[in] width_bytes Draw width of bitmap, in bytes. (was A)
 * \param[in] backbuf_addr Back-buffer address to draw at. (was HL)
 * \param[in] height Number of rows to draw. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was DE')
 * \param[in] bitmap_data Source bitmap data. (was HL')
 */
static void plot_sprite(chqstate_t *state,
                        int         width_bytes,
                        u8         *backbuf_addr,
                        int         height,
                        int         bitmap_stride,
                        const u8   *bitmap_data)
{
  int odd;         /* non-zero when width_bytes is odd, selecting the odd path (was carry) */
  int jump_offset; /* byte offset into the unrolled POP jump table (was IX) */

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
 * $94B1: Plot an unmasked sprite (even byte widths)
 *
 * Draws up to 8 bytes per row (4 pairs of 2) into the back buffer using a
 * POP-unroll inner loop, entered via [jump_offset]. Each row advances backward
 * through the back buffer using prev_buf_row.
 *
 * Conv: Z80 sets SP = HL (bitmap pointer) and uses POP DE to load two bytes at
 * a time — the fastest possible load on Z80. C uses a plain src pointer with
 * `*src++` pairs. Conv: Z80 enters the unrolled POP loop via `JP (IX)` (IX =
 * table base + [jump_offset]); C uses switch/case with fallthrough. Conv: Z80
 * self-modifies `LD SP,$0000` at $94BF to save the original SP; not needed in
 * C. Conv: Row advance uses DEC H with multi-step carry correction in Z80; C
 * delegates to prev_buf_row().
 *
 * \param[in] state Pointer to game state.
 * \param[in] jump_offset Byte offset into the unrolled POP table. (was IX)
 * \param[in] backbuf_addr Back-buffer address to draw at. (was HL)
 * \param[in] height Number of rows to draw. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was DE')
 * \param[in] bitmap_data Source bitmap data. (was HL')
 */
static u8 *plot_sprite_even(chqstate_t *state,
                            int         jump_offset,
                            u8         *backbuf_addr,
                            int         height,
                            int         bitmap_stride,
                            const u8   *bitmap_data)
{
  const u8 *src;          /* bitmap source pointer; Z80 used SP via POP (was SP) */
  u8       *backbuf_orig; /* row start in back buffer, saved for prev_buf_row (was A) */
  int       n;            /* bytes to copy this row: (4 − case) × 2 (Conv: no Z80 register) */

  assert(jump_offset % 5 == 0);
  /* Conv: case 4 (jump_offset=20) is valid: Z80 offset 20 lands at $94DC
   * (LD L,A), the post-table row-restore that follows all four plot entries,
   * drawing 0 pairs. This happens when the sprite is clipped to 0 bytes wide
   * at the right edge. */
  assert(jump_offset / 5 >= 0 && jump_offset / 5 <= 4);
  assert(VALID_BACKBUF_PTR(backbuf_addr));
  assert(height >= 1 && height < 192);
  assert(bitmap_data != NULL);

  // Conv: B & C moved into prev_buf_row
  // EXX - bank
  goto plot_sprite_even_start;

  for (;;) {
    // EXX - bank
    if (--height == 0)
      return backbuf_addr;

    bitmap_data += bitmap_stride; // Advance to start of next row

plot_sprite_even_start:
    src = bitmap_data;
    // EXX - unbank
    backbuf_orig = backbuf_addr;
    /* Conv: Z80 jump table (4 − case) POP pairs → (4 − case)×2 bytes. */
    n = (4 - jump_offset / 5) * 2;
    assert(VALID_BACKBUF_PTR(backbuf_addr));
    memcpy(backbuf_addr, src, (size_t)n);
    backbuf_addr = ADDRTOBACKBUF(prev_buf_row(BACKBUFTOADDR(backbuf_orig)));
  }
}

/**
 * $94F2: Plot an unmasked sprite (odd byte widths)
 *
 * Derives the jump-table offset from [width_bytes] and delegates to
 * plot_sprite_odd_entrypt, which runs the odd-width unrolled inner loop. The
 * Z80 function falls through directly to ps_odd_jumptable.
 *
 * Conv: Z80 uses INC A; CPL; ADD A,5 to compute (4 - A) for the jump offset; C
 * uses a direct expression. Conv: Z80 uses SP and POP for source reads; C uses
 * a plain src pointer. Conv: Row advance uses DEC H with carry correction; C
 * uses prev_buf_row().
 *
 * \param[in] state Pointer to game state.
 * \param[in] width_bytes Number of byte pairs in the bitmap width (halved).
 *   (was A)
 * \param[in] backbuf_addr Back-buffer address to draw at. (was HL)
 * \param[in] height Number of rows to draw. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was DE')
 * \param[in] bitmap_data Source bitmap data. (was HL')
 */
static u8 *plot_sprite_odd(chqstate_t *state,
                           int          width_bytes,
                           u8         *backbuf_addr,
                           int          height,
                           int         bitmap_stride,
                           const u8   *bitmap_data)
{
  int jump_offset; /* byte offset into the unrolled POP jump table (was IX) */

  assert(width_bytes >= 0 && width_bytes <= 3);
  assert(VALID_BACKBUF_PTR(backbuf_addr));
  assert(height >= 1);
  assert(bitmap_data != NULL);

  jump_offset = 5 * (3 - width_bytes); // 5 bytes/op

  return plot_sprite_odd_entrypt(state,
                                 jump_offset,
                                 backbuf_addr,
                                 height,
                                 bitmap_stride,
                                 bitmap_data); /* was FALLTHROUGH */
}

// Direct entry point for plot_sprite_odd
static u8 *plot_sprite_odd_entrypt(chqstate_t *state,
                                   int         jump_offset,
                                   u8         *backbuf_addr,
                                   int          height,
                                   int         bitmap_stride,
                                   const u8   *bitmap_data)
{
  const u8 *src;          /* was SP */
  u8       *backbuf_orig; /* was A */
  int       n;            /* bytes to copy this row: (4 − case)×2 − 1 (Conv: no Z80 register) */

  assert(jump_offset % 5 == 0);
  assert(jump_offset / 5 >= 0 && jump_offset / 5 <= 3);
  assert(VALID_BACKBUF_PTR(backbuf_addr));
  assert(height >= 1);
  assert(bitmap_data != NULL);

  // Conv: B & C moved into prev_buf_row
  // EXX - bank
  goto plot_sprite_odd_start;

  for (;;) {
    // EXX - bank
    if (--height == 0)
      return backbuf_addr;

    bitmap_data += bitmap_stride;

plot_sprite_odd_start:
    src = bitmap_data;
    // EXX - unbank
    backbuf_orig = backbuf_addr;
    /* Conv: Z80 jump table 3 POP pairs + 1 odd byte → (4 − case)×2 − 1 bytes. */
    n = (4 - jump_offset / 5) * 2 - 1;
    memcpy(backbuf_addr, src, (size_t)n);
    backbuf_addr = ADDRTOBACKBUF(prev_buf_row(BACKBUFTOADDR(backbuf_orig)));
  }
}

/**
 * $9542: Plot a horizontally flipped unmasked sprite to the back buffer
 *
 * Advances [backbuf_addr] by [width_bytes] so that the back buffer is filled
 * right-to-left, then dispatches to the odd- or even-width flipped inner loop.
 * Each byte is bit-reversed via the flip table (state->flipped) before being
 * written, giving a horizontal mirror of the source sprite.
 *
 * Conv: Z80 uses `SRL A` to test the carry (bit 0 of A) and halve A
 * simultaneously; C uses a bit test and a right shift separately. Conv: Row
 * advance via DEC H with carry correction in Z80; C uses prev_buf_row().
 *
 * \param[in] state Pointer to game state.
 * \param[in] width_bytes Draw width of bitmap, in bytes. (was A)
 * \param[in] backbuf_addr Back-buffer address of the leftmost byte to draw.
 *   (was HL)
 * \param[in] height Number of rows to draw. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was E')
 * \param[in] bitmap_data Source bitmap data. (was HL')
 */
static void plot_sprite_flipped(chqstate_t *state,
                                int         width_bytes,
                                u8         *backbuf_addr,
                                int         height,
                                int         bitmap_stride,
                                const u8   *bitmap_data)
{
  int odd;         /* non-zero when width_bytes is odd, selecting the odd path (was carry) */
  int jump_offset; /* byte offset into the flipped-POP jump table (was IX) */

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
 * $9565: Plot a horizontally flipped sprite (even byte widths)
 *
 * Draws up to 8 bytes per row right-to-left into the back buffer, bit-
 * reversing each source byte via [flip_table]. Each pair of bytes is written
 * using POP reads in the Z80 (SP as source); C uses a plain src pointer. The
 * unrolled inner loop is entered via [jump_offset] (9 bytes per operation).
 *
 * Conv: Z80 uses SP = HL bitmap pointer and POP DE for two-byte reads; C uses a
 * plain src pointer. Conv: Z80 enters the flip+write loop via `JP (IX)`; C uses
 * switch/case with fallthrough. Conv: Row advance via DEC H with carry
 * correction; C uses prev_buf_row().
 *
 * \param[in] state Pointer to game state.
 * \param[in] jump_offset Byte offset into the unrolled flip-POP table. (was IX)
 * \param[in] flip_table 256-entry bit-reversal look-up table. (was DE)
 * \param[in] backbuf_addr Back-buffer address to draw at. (was HL)
 * \param[in] height Number of rows to draw. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was DE')
 * \param[in] bitmap_data Source bitmap data. (was HL')
 */
static void plot_sprite_flipped_even(chqstate_t *state,
                                     int         jump_offset,
                                     const u8   *flip_table,
                                     u8         *backbuf_addr,
                                     int         height,
                                     int         bitmap_stride,
                                     const u8   *bitmap_data)
{
  const u8 *src;          /* bitmap source pointer; Z80 used SP via POP (was SP) */
  u8       *backbuf_orig; /* row start in back buffer, saved for prev_buf_row (was A) */

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
    backbuf_addr = ADDRTOBACKBUF(prev_buf_row(BACKBUFTOADDR(backbuf_orig)));
  }
}

/**
 * $95B3: Plot a horizontally flipped sprite (odd byte widths)
 *
 * Draws up to 7 bytes per row (the odd trailing byte plus up to 3 pairs)
 * right-to-left into the back buffer, bit-reversing each byte via
 * state->flipped. The Z80 function adds 1 to [width_bytes] before computing the
 * jump offset, making the effective range ([width_bytes]+1) = 1..4 pairs.
 *
 * Conv: Z80 uses SP and POP for source reads; C uses a plain src pointer. Conv:
 * Z80 enters the unrolled loop via `JP (IX)`; C uses switch/case with
 * fallthrough. Conv: Row advance via DEC H with carry correction; C uses
 * prev_buf_row().
 *
 * \param[in] state Pointer to game state.
 * \param[in] width_bytes Number of byte pairs (halved width from caller).
 *   (was A)
 * \param[in] backbuf_addr Back-buffer address to draw at. (was HL)
 * \param[in] height Number of rows to draw. (was B')
 * \param[in] bitmap_stride Stride of bitmap data, in bytes. (was E')
 * \param[in] bitmap_data Source bitmap data. (was HL')
 */
static void plot_sprite_flipped_odd(chqstate_t *state,
                                    int          width_bytes,
                                    u8         *backbuf_addr,
                                    int          height,
                                    int          bitmap_stride,
                                    const u8   *bitmap_data)
{
  int       jump_offset;  /* unroll jump index into the flip-write table (was IX) */
  const u8 *src;          /* bitmap source pointer; Z80 used SP via POP (was SP) */
  u8       *backbuf_orig; /* row start in back buffer, saved for prev_buf_row (was A) */

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
    backbuf_addr = ADDRTOBACKBUF(prev_buf_row(BACKBUFTOADDR(backbuf_orig)));
  }
}

/**
 * $961B: Advance the three-byte LFSR and return a pseudo-random byte
 *
 * Each call mutates the three-byte seed in state->rng_seed as follows:
 *
 * seed[0] -= 141            ($961E–$9621) seed[1] += 3
 * ($9623–$9625) A        = seed[0] + seed[1], then rotated right ($9626–$9628)
 * seed[2]  = A + RRCA(seed[2])  ($9629–$962C)
 *
 * The returned value is A (= updated seed[2]).
 *
 * Conv: Z80 uses RRCA at $9628 (rotate A right circular) and RRC (HL) at $9629
 * (rotate seed[2] right circular, setting carry). The C macro RRC(r) uses the
 * local variable `carry` as a scratch; it is not read before being written, so
 * any prior value is irrelevant.
 *
 * \param[in] state Pointer to game state.
 * \return Pseudo-random byte.
 */
static u8 rng(chqstate_t *state)
{
  u8 *HLseed; /* pointer walking rng_seed[0..2] (was HL) */
  u8  A;      /* accumulator; final result (was A) */
  int carry;  /* carry scratch required by the RRC macro (carry) */

  HLseed    = &state->rng_seed[0];
  A         = *HLseed - 141;
  *HLseed++ = A;
  *HLseed  += 3;
  A        += *HLseed++;
  RRC(A);
  RRC(*HLseed);
  A        += *HLseed;
  *HLseed   = A;
  return A;
}

/**
 * $9945: Initiate a chatter sequence if priority allows it
 *
 * Sets up a new chatter display. If chatter is already in progress and the
 * running [priority] is at least as high as the requested [priority], the new
 * request is silently ignored. Otherwise the state machine is primed with the
 * new block pointer and reset to CHATTERSTATE_START.
 *
 * \param[in] state Pointer to game state.
 * \param[in] priority Priority of this chatter; higher values win. (was A)
 * \param[in] chatterblk Pointer to the chatter data block to play. (was HL)
 */
static void start_chatter(chqstate_t       *state,
                          chatterpriority_t priority,
                          const u8         *chatterblk)
{
  int chatter_state; /* current FSM state, checked against IDLE and STOP (was A) */

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
 * $9965: Advance the chatter state machine one frame
 *
 * Drives the four-state chatter FSM each frame: - STARTING (1): transitions to
 * RUN, clears the message line and starts the noise-in effect. - RUN (2): if
 * the noise effect is still counting down, drives it; otherwise advances the
 * message cursor with an optional blink animation, or reads the next chatter
 * command (STOP, PAUSE or a new message block). - STOPPING (3): counts down
 * noise_counter; on expiry enters IDLE and wipes the face attributes to black.
 * - IDLE (0): blinks a space cursor off-screen (purely cosmetic).
 *
 * Triggers a screen draw at the end of every call.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: The idle cursor blink byte ($AA/$55 alternator) was self-modified at
 * $9982; C reads and writes state->chatter_cursor_blink instead.
 */
static void drive_chatter(chqstate_t *state)
{
  int          carry;         /* carry from RRC/RR operations (carry) */
  int          chatter_state; /* FSM state at entry, decremented to dispatch (was A) */
  char         character;     /* character to display: space or message char (was D) */
  u8           rotating;      /* cursor blink byte, RRC-rotated each frame (was A) */
  int          x;             /* x position for mini-font cursor plot (was A) */
  int          delay;         /* chatter display delay counter (was A) */
  u8           B;             /* copy of chatter_delay used for RR blink-rate test (was B) */
  const char  *HLnextchar;    /* pointer to current character in message string (was HL) */
  const u8    *chatterblk;    /* pointer into the chatter data block (was HL) */
  int          chattercmd;    /* command byte from chatterblk: $FF=stop, $FE=pause (was A) */

  carry = 0;

  chatter_state = state->chatter_state;
  if (--chatter_state == 0) // starting (1)
    goto starting;
  if (--chatter_state == 0) // displaying (2)
    goto do_noise_effect;
  if (--chatter_state == 0) { // stopping (3)
    if (--state->noise_counter) {
      draw_noise_effect(state, state->noise_counter); /* tail call */
      goto exit;
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
    plot_mini_font_cursor_on(state, x, character); /* tail call */
  else
    plot_mini_font_cursor_off(state, x, character); /* tail call */
  goto exit;

do_noise_effect:
  if (state->noise_counter) {
    drive_noise_effect(state, state->noise_counter); /* tail call */
    goto exit;
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
    plot_mini_font_cursor_on(state, x, character); /* tail call */
  else
    plot_mini_font_cursor_off(state, x, character); /* tail call */
  goto exit;

clear_line:
  x = state->message_x;
  if (x) {
    pc_clear_line(state, x); /* tail call */
    goto exit;
  }

read_message:
  chatterblk = state->chatterblk_ptr;
  chattercmd = *chatterblk;
  if (chattercmd == CHATTERCMD_STOP) {
    drive_chatter_stop(state);
    goto exit;
  }
  if (chattercmd != CHATTERCMD_PAUSE) {
    pc_chatter_message(state, chatterblk); /* tail call */
    goto exit;
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
  drive_noise_effect(state, 4); /* tail call */

exit:
  state->speccy->draw(state->speccy, NULL); /* Conv: added */
}

/**
 * $99D3: Begin the chatter stop sequence
 *
 * Primes the noise-out countdown (noise_counter = 4), switches the FSM to
 * STOPPING, and clears the on-screen message line. The noise effect will count
 * down over the next four frames before entering IDLE.
 *
 * \param[in] state Pointer to game state.
 */
static void drive_chatter_stop(chqstate_t *state)
{
  state->noise_counter = 4;
  state->chatter_state = CHATTERSTATE_STOP;
  clear_message_line(state); /* tail call */
}

/**
 * $99EC: Resolve the speaking character and start showing the message
 *
 * Walks the current chatter block, skipping RANDOM ($FC) command bytes each of
 * which triggers a three-way random branch to select the actual sub-block. The
 * first non-$FC byte is the speaking character's ID (0=pilot, 1=Nancy,
 * 2=Raymond, 3=Tony). The corresponding face bitmap is plotted at screen
 * position (176,8), then pc_chatter_message is called to queue the message.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 computes face bitmap address via repeated ADD HL,DE (multiply by
 * $B4=180); C indexes directly into bitmap_faces[]. Conv: Chatter block entries
 * were Z80 addresses; C uses table indices into chatter_blocks[].
 */
static void print_chatter(chqstate_t *state)
{
  const u8 *chatterblk; /* pointer walking the chatter block data (was HL) */
  int       cmd;        /* command byte: $FC=random choice, or character ID (was A) */
  int       rnd;        /* random byte used for three-way branch (was A) */
  const u8 *face;       /* pointer to the face bitmap for the speaking character (was HL) */

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
 * $9A24: Load the next message string from a chatter block
 *
 * Reads a string index from the chatter block, resolves it to a C string (from
 * common_chatter_strings[] or the stage's per-stage strings), saves the updated
 * block pointer and the string start in state, then falls through to
 * pc_clear_line to begin displaying from column 0.
 *
 * \param[in] state Pointer to game state.
 * \param[in] chatterblk Pointer to the current position in the chatter block.
 * (was HL)
 *
 * Conv: Z80 stores a 2-byte message address; C stores a 1-byte index into
 * common_chatter_strings[] or stage->chatter_strings[].
 */
static void pc_chatter_message(chqstate_t *state, const u8 *chatterblk)
{
  int         string_index; /* index into chatter string tables (Conv: no Z80 register) */
  const char *DEstring;     /* pointer to the resolved message string (was DE) */

  // Conv: Original game loads an address directly here.
  assert(*chatterblk < CHATTERSTR__LIMIT);
  string_index = *chatterblk++;
  if (string_index < CHATTERSTR_PERP_DESC_1) {
    DEstring = common_chatter_strings[string_index];
  } else {
    assert(string_index < CHATTERSTR__LIMIT);
    DEstring = state->stage->chatter_strings[string_index - CHATTERSTR_PERP_DESC_1];
  }
  assert(DEstring);
  state->chatterblk_ptr = chatterblk;
  state->next_character = DEstring;
  pc_clear_line(state, 0); /* was FALLTHROUGH */
}

/**
 * $9A30: Plot the next character into the chatter message line
 *
 * If [x] is zero, clears the message line first. Reads the next character from
 * state->next_character (masking the EOS bit), plots it with a cursor block at
 * column [x], and advances the message position. If the EOS bit was set the
 * character is the last one; a delay of 10 frames is set so the player can read
 * it before the display cycles on.
 *
 * \param[in] state Pointer to game state.
 * \param[in] x Column at which to plot the character. Zero triggers a
 * line clear first. (was A)
 */
static void pc_clear_line(chqstate_t *state, int x)
{
  const char *nextch;    /* pointer to the current character in the message string (was HL) */
  char        character; /* character to plot, with EOS bit cleared (was D) */

  if (x == 0)
    clear_message_line(state);

  nextch = state->next_character;
  assert(nextch);
  character = *nextch & ~EOS; // remove any terminator
  assert(character >= ' ' && character <= 'Z');
  plot_mini_font_cursor_on(state, x, character);
  if (*nextch++ & EOS) // if terminated
    state->chatter_delay = 10; // pause at end of string
  state->message_x = x + 1;
  state->next_character = nextch;
}

/**
 * $9A55: Advance the noise effect counter and dispatch
 *
 * Decrements noise_counter and stores it. If the [counter] has reached zero the
 * noise effect is over: calls print_chatter to reveal the face and message.
 * Otherwise falls through to draw_noise_effect to render the next static frame.
 *
 * \param[in] state Pointer to game state.
 * \param[in] counter Current noise counter value, decremented before use.
 *   (was A)
 */
static void drive_noise_effect(chqstate_t *state, int counter)
{
  state->noise_counter = --counter;
  if (counter == 0)
    print_chatter(state); /* tail call */
  else
    draw_noise_effect(state, counter); /* was FALLTHROUGH */
}

/**
 * $9A5C: Render one frame of the noise/static effect over the face area
 *
 * Uses the bottom bit of [counter] (via RRA) to alternate between showing a
 * cursor-on or cursor-off space at x=-1. Then iterates 40 screen rows of 4
 * bytes each, computing pseudo-random pixel values by XOR-rotating the
 * noise_bytes[] state array and writing the result directly to screen memory in
 * the face area starting at (176,8). Finishes by setting the face attributes to
 * bright-white-on-black.
 *
 * \param[in] state Pointer to game state.
 * \param[in] counter Noise counter; bit 0 selects cursor style. (was A)
 *
 * Conv: Z80 uses RRA to shift bit 0 into carry; C uses RR([counter]) which
 * updates the local carry variable.
 */
static void draw_noise_effect(chqstate_t *state, int counter)
{
  int   carry;          /* carry from RR(counter), selects cursor-on or off (carry) */
  int   x;              /* cursor x position, always -1 (0xFF) (was A) */
  char  character;      /* character to plot, always space (was D) */
  u16   DEscreen;       /* screen address walking through the face area rows (was DE) */
  int   C;              /* outer row counter, 40 rows (was C) */
  int   B;              /* inner column counter, 4 bytes per row (was B) */
  u16   DEscreen_saved; /* row start saved across the inner column loop (was stack) */
  u8   *noisebytes;     /* pointer into state->noise_bytes[] (was HL) */
  u8    A;              /* accumulated noise byte written to screen (was A) */

  carry = 0;

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
    DEscreen = next_scr_row(DEscreen);
  } while (--C > 0);

  ne_plot_attrs(state, attribute_BRIGHT_WHITE_OVER_BLACK);
  /* was FALLTHROUGH */
}

/**
 * $9A98: Fill the face-area screen attributes
 *
 * Writes [attr] to a 4-column × 5-row block of attribute bytes starting at
 * screen attribute address $5836 (row 22, column 1). Called with
 * attribute_BRIGHT_WHITE_OVER_BLACK when drawing noise, and 0 (black on black)
 * when wiping the face after the chatter sequence ends.
 *
 * \param[in] state Pointer to game state.
 * \param[in] attr Attribute byte to fill the face area with. (was A)
 */
static void ne_plot_attrs(chqstate_t *state, int attr)
{
  static const zxbox_t face_box = { /* face area: ZX rows 8–47, cols 22–25 */
    176, 144, 208, 184
  };
  u8 *addr;       /* pointer to the current attribute row in the face area (was HL) */
  int iterations; /* row countdown, 5 rows (was B) */

  // Screen attribute (22,1) (Conv: address -> offset)
  addr       = ADDRTOATTRS(0x5836);
  iterations = FACEATTRHEIGHT; // 5 rows
  do {
    // Conv: Screen write now goes via state.
    memset(addr, attr, FACEATTRWIDTH);
    addr += SCREEN_ATTRIBUTES_WIDTH;
  } while (--iterations > 0);
  state->speccy->draw(state->speccy, &face_box); /* Conv: added */
}

/**
 * $9AAB: Plot a face bitmap and its attribute block to the screen
 *
 * Copies FACEBITMAPBYTES (160) bytes of [face] bitmap data to the ZX Spectrum
 * starting at [screen], advancing one scanline at a time using the standard
 * ZX Spectrum row-advance logic. Falls through to plot_face_attributes to
 * write the matching colour attribute block.
 *
 * \param[in] state Pointer to game state.
 * \param[in] screen ZX Spectrum screen address to start drawing at (always
 * $4036, i.e. row 8 pixel 6 of the display). (was DE)
 * \param[in] face Pointer to face data: bitmap bytes followed immediately
 * by attribute bytes. (was HL)
 */
static void plot_face(chqstate_t *state,
                      int         screen,
                      const u8   *face)
{
  u16 saved_screen; /* screen start address saved for the attribute pass (was PUSH DE) */
  int counter;      /* byte countdown: FACEBITMAPBYTES (160) down to 0 (was BC) */

  assert(screen >= SCREEN_START_ADDRESS && screen < SCREEN_END_ADDRESS);
  assert(face);

  counter = FACEBITMAPBYTES;
  saved_screen = screen;
  screen -= SCREEN_START_ADDRESS; // Conv: address -> offset
  for (;;) {
    memcpy(&state->speccy->screen.pixels[screen], face, 4); /* Conv: memcpy */
    face    += 4;
    counter -= 4;
    if (counter == 0)
      break;
    screen = next_scr_row(screen);
  }

  plot_face_attributes(state, saved_screen, face); /* was fallthrough */
}

/**
 * $9ACE: Write the attribute block for a face bitmap
 *
 * Derives the attribute-area start address from the saved bitmap [screen]
 * address: the high byte encodes the pixel band (0–2), which maps to attribute
 * rows $58xx–$5Axx. Copies FACEATTRBYTES (20) attribute bytes in runs of 4,
 * advancing by one attribute row (32 bytes) between runs.
 *
 * \param[in] state Pointer to game state.
 * \param[in] screen ZX Spectrum screen address saved from the bitmap pass;
 * its high byte encodes the display band. (was stack/POP DE)
 * \param[in] face Pointer to the attribute bytes that follow the bitmap
 * data in the [face] data block. (was HL)
 */
static void plot_face_attributes(chqstate_t *state,
                                 int         screen,
                                 const u8   *face)
{
  static const zxbox_t face_box = { /* face area: ZX rows 8–47, cols 22–25 */
    176, 144, 208, 184
  };
  u8  A_attrhi;  /* screen high byte, rotated to extract band, then biased to $58 (was A) */
  int counter;   /* byte countdown: FACEATTRBYTES (20) down to 0 (was BC) */
  int A_rowadv;  /* low-byte row-stride computation: (screen & 0xFF) + 0x1C (was A) */

  A_attrhi = screen >> 8;
  A_attrhi = (A_attrhi >> 3) & 3; /* RRC A x3; AND 3 */
  A_attrhi += 0x58;
  screen = (A_attrhi << 8) | (screen & 0xFF);
  screen -= SCREEN_ATTRIBUTES_START_ADDRESS; // Conv: address -> offset
  counter = FACEATTRBYTES;
  for (;;) {
    memcpy(&state->speccy->screen.attributes[screen], face, 4); /* Conv: memcpy */
    screen  += 4;
    face    += 4;
    counter -= 4;
    if (counter == 0)
      break;

    // TODO Hoist to next-attr-row macro?
    A_rowadv = (screen & 0xFF) + 0x1C;
    screen = (screen & 0xFF00) | (A_rowadv & 0xFF);
    if (A_rowadv >= 0x100)
      screen += 256;
  }
  state->speccy->draw(state->speccy, &face_box); /* Conv: added */
}

/**
 * $9AEC: Plot a mini-font character with no cursor underline
 *
 * Entry point with BC=0: both extra bitmap bytes are zero so no cursor block
 * appears beneath the [character].
 *
 * \param[in] state Pointer to game state.
 * \param[in] x Column index (0–based); 0xFF means the special
 * off-screen cursor position. (was A)
 * \param[in] character ASCII character to draw. (was D)
 */
static void plot_mini_font_cursor_off(chqstate_t *state,
                                      int          x,
                                      char        character)
{
  plot_mini_font_char(state, x, character, ________, ________);
}

/**
 * $9AF1: Plot a mini-font character with a cursor underline
 *
 * Entry point with BC=$0780: B=$07 ORs three bits into the left glyph byte (the
 * cursor underline row) and C=$80 sets the MSB of the right glyph byte,
 * producing a visible cursor block beneath the [character].
 *
 * \param[in] state Pointer to game state.
 * \param[in] x Column index (0–based); 0xFF means the special
 * off-screen cursor position. (was A)
 * \param[in] character ASCII character to draw. (was D)
 */
static void plot_mini_font_cursor_on(chqstate_t *state,
                                     int          x,
                                     char        character)
{
  plot_mini_font_char(state, x, character, _____XXX, X_______);
}

/**
 * $9AF4: Plot one mini-font character to the chatter message line
 *
 * Computes the screen column byte address and right-shift amount from [x],
 * converts the ASCII character to a glyph ID, looks up the 6-row glyph bitmap
 * in minifont[], ORs in the extra bitmap bytes (used by the cursor underline),
 * right-shifts the combined 16-bit word to align it to the pixel column, masks
 * the left screen byte to preserve adjacent pixels, and writes two bytes per
 * row for MFHEIGHT (6) rows.
 *
 * \param[in] state Pointer to game state.
 * \param[in] x Column slot (0-based); 0xFF selects the off-screen
 * cursor slot. (was A)
 * \param[in] ascii ASCII character to draw. (was D)
 * \param[in] extrabm1 Extra bits ORed into the left glyph byte (cursor
 * underline pattern or 0). (was B, self-modifies $9B64)
 * \param[in] extrabm2 Extra bits placed in the right glyph byte (cursor
 * underline MSB or 0). (was C, self-modifies $9B61)
 *
 * Conv: Z80 self-modifies three operands ($9B61, $9B64, $9B89) and uses a
 * jump-table cascade (SRL B; RR C repeated) for the pixel shift; C uses local
 * variables and a single right-shift of a 16-bit composite word. Conv: Row
 * counter banked to A' in Z80 (EX AF,AF'); C uses a plain local.
 */
static void plot_mini_font_char(chqstate_t *state,
                                int         x,
                                char        ascii,
                                int         extrabm1,
                                int         extrabm2)
{
  static const zxbox_t message_line_box = { /* chatter message area: ZX rows 53–58 */
    0, 133, SCREEN_WIDTH, 139
  };
  int       carry;    /* carry from SRL/RR shift operations (carry) */
  int       extra2;   /* extra bits for right (low) glyph byte; self-modifies $9B61 (was C) */
  int       extra1;   /* extra bits ORed into left (high) glyph byte; self-modifies $9B64 (was B) */
  int       mask;     /* left-column pixel mask preserving adjacent bits; self-modifies $9B89 (was A) */
  int       rotate;   /* right-shift count for pixel column alignment; self-modifies $9B67 (was A) */
  int       A;        /* scaled x position, reduced to find column and shift (was A) */
  int       ascii2;   /* copy of ascii used for character classification (was A) */
  int       row;      /* row countdown, MFHEIGHT down to 1 (was A, banked to A') */
  int       gid;      /* glyph ID for punctuation lookup (was C) */
  int       sgid;     /* ASCII value of the glyph, for font table indexing (was A) */
  u16       screen;   /* ZX Spectrum screen address: D=high byte, E=column offset (was DE) */
  const u8 *fontdata; /* pointer to current glyph row in minifont[] (was DE after EX DE,HL) */
  u16       HLscreen; /* working screen address advanced one scanline per row (was HL) */
  u8        bm2;      /* right (low) bitmap byte after shift (was C) */
  u8        bm1;      /* left (high) bitmap byte: font byte OR'd with extra1, then shifted (was B) */
  unsigned  bm;       /* combined 16-bit bitmap word before split into bm1/bm2 (Conv: no Z80 reg) */
  u8       *screen2;  /* pointer into ZX Spectrum pixel buffer derived from HLscreen (Conv: no Z80 reg) */

  carry = 0;

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
  switch (ascii2) {
  case '.':  gid = 26; break;
  case ',':  gid = 27; break;
  case '!':  gid = 28; break;
  case ' ':  gid = 29; break;
  case '\'': gid = 30; break;
  default:
    if (ascii2 >= ';') { sgid = ascii2; goto pmf_have_ascii; }
    assert(0);
    gid = 30 + ascii2 - '/'; // not convinced this is ever used in the game
    break;
  }

  sgid = gid + 'A'; // Turn the glyph ID in #REGc into ASCII in #REGa

pmf_have_ascii:
  fontdata = &minifont[(sgid - 'A') * MFHEIGHT];
  HLscreen = screen; /* was EX */
  row = MFHEIGHT;
  do {
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
      bm = (bm1 << 8) | bm2;
      bm >>= (8 - rotate);
      bm1 = bm >> 8;
      bm2 = bm & 0xff;
    }

    screen2 = ADDRTOSCREEN(HLscreen); // Conv: added
    screen2[0] = (mask & screen2[0]) | bm1;
    screen2[1] = bm2;
    fontdata++;
    HLscreen = next_scr_row(HLscreen);
  } while (--row > 0);
  state->speccy->draw(state->speccy, &message_line_box); /* Conv: added */
}

/**
 * $9BA7: Zero the six-scanline message-line area on the bitmap
 *
 * Wipes six consecutive scanlines starting at screen address $45C1 (pixel row 8
 * within character row 8, column 33 = approximately the chatter area). Each
 * scanline is zeroed by the rolling LDIR technique: LD (HL),B clears the first
 * byte, then LDIR from HL to HL+1 propagates the zero across the next 29 bytes
 * (30 bytes total per row). After each row HL is restored from the stack and
 * advanced to the next scanline via INC H with the standard ZX Spectrum
 * character-row wrap.
 *
 * The row counter starts in A and is banked to A' during each loop body so that
 * B can hold the zero fill value.
 *
 * Conv: Z80 LDIR zeros 30 bytes (HL through HL+29); C memset zeroes only the 29
 * bytes HL+1 through HL+29, omitting HL itself. The first byte is part of the
 * chatter area and is overwritten anyway by the next print call.
 *
 * \param[in] state Pointer to game state.
 */
static void clear_message_line(chqstate_t *state)
{
  static const zxbox_t message_line_box = { /* chatter message area: ZX rows 53–58 */
    0, 133, SCREEN_WIDTH, 139
  };
  u16 HLscreen; /* screen address of first byte in the current scanline (was HL) */
  int A_rows;   /* scanline counter, 6 down to 1; banked to A' during loop body (was A) */

  HLscreen = 0x45C1;
  A_rows   = 6;
  do {
    memset(ADDRTOSCREEN(HLscreen + 1), 0, 29); /* Conv: replaces LD (HL),B + LDIR */
    HLscreen = next_scr_row(HLscreen);
  } while (--A_rows);
  state->speccy->draw(state->speccy, &message_line_box); /* Conv: added */
}

/**
 * $9BCF: Handle time running out, countdown and continue
 *
 * Drives the time-up state machine, called once per frame from the main loop: -
 * INIT (0): decrements time every 15 frames (1 second); when 15s remain Nancy
 * warns that time is running out; when time hits 0 suppresses player input and
 * moves to CHECK_TIME_UP. - CHECK_TIME_UP (1): waits until the hero car stops,
 * then moves to CHECK_CREDITS. - CHECK_CREDITS (2): if credits remain, consumes
 * one, shows the 10-second continue countdown and moves to CHECK_RESTART. -
 * CHECK_RESTART (3): ticks the countdown with a BIP/BOW effect every half
 * second; FIRE resets the mission and restarts; expiry triggers quit. - WAITING
 * (4): quitting is in progress; do nothing.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 implements the FSM via self-modifying JP; C uses a switch dispatch.
 * Conv: BCD time decrement uses DAA_sub with the Z80 half-borrow (H flag).
 */
static void check_time_up(chqstate_t *state)
{
  const u8 *ptime_bcd;            /* pointer to BCD time counter in session state (was HL) */
  int       time_bcd;             /* updated BCD time value after DAA_sub decrement (was A) */
  int       time_up_state;        /* FSM state at entry, used as switch index (was A) */
  int       half_borrow;          /* H flag: low BCD nibble was 0 before decrement (was H flag) */
  int       remaining_seconds_x2; /* doubled countdown: 21 (= 10 seconds × 2 + 1) down to 0 (was L) */
  int       remaining_subseconds; /* sub-second frame counter, 6 down to 1 (was H) */
  int       effect;               /* SFX index: EFFECT_BIP or EFFECT_BOW for the tick-tock sound (was B) */
  int       seconds;              /* countdown in whole seconds, derived by halving remaining_seconds_x2 (was A) */
  u8       *time_digits;          /* pointer to the two-digit time field in continue_messages (was DE) */
  int       hidigit;              /* ASCII high digit of the countdown display: '1' or space (was A) */
  int       lodigit;              /* low digit of the countdown as integer, converted to ASCII on write (was L) */

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
    state->session.user_input_mask = USERINPUTFLAG_RIGHT |
                                     USERINPUTFLAG_LEFT |
                                     USERINPUTFLAG_FIRE |
                                     USERINPUTFLAG_QUIT;
    return;
  }

update_remaining_time:
  if (--state->session.subsecond_ticks > 0)
    return;

  state->session.subsecond_ticks = SUBSECOND_TICKS_PER_SECOND;
  half_borrow = (state->session.time_bcd & 0x0F) == 0;
  state->session.time_bcd = time_bcd = DAA_sub(state->session.time_bcd - 1, half_borrow, NULL);

  // When 15s remain Nancy warns that time is running out.
  if (time_bcd == LOW_TIME_WARNING_BCD)
    // Note: This passes time_bcd as the priority which is 21.
    start_chatter(state, time_bcd,
                  chatterblk_nancy_time_running_out); /* tail call */
  return;

check_time_up:
  if (*ptime_bcd != 0) {
    state->time_up_state = TIMEUPSTATE_INIT;
    state->session.user_input_mask = USERINPUTFLAGMASK_ALLOW_ALL;
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
    check_user_input_quit_key(state); /* tail call */
  } else {
    // Display the pre-decrement count: Z80 loads A before DEC (HL) ($9C39/$9C3E)
    state->continue_messages[CONTINUE_MESSAGES_CREDIT_N] = (state->credits + '0') | EOS;
    state->credits--;
    state->time_up_state = TIMEUPSTATE_CHECK_RESTART;
    state->tick_remaining_seconds_x2 = 21; // a 10 second countdown, doubled, plus 1
    state->tick_remaining_subseconds = 1;  // force an initial decrement
  }
  return;

check_restart:
  if (state->user_input & USERINPUTFLAG_FIRE) {
    // Reset mission
    state->time_up_state           = TIMEUPSTATE_INIT;
    state->smash_level             = 0;
    state->smash_counter           = 0;
    state->session.user_input_mask = USERINPUTFLAGMASK_ALLOW_ALL;
    state->gear_lockout            = 3;
    state->transition_control      = TRANSITIONCONTROL_FILL_ATTRIBUTES;
    state->session.turbos          = RESTART_BOOSTS;
    state->session.time_bcd        = RESTART_TIME_BCD;
    state->retry_count++;

    play_start_noise(state);
    return;
  }

  setup_overlay_messages(state, &state->continue_messages[0]);

  // Conv: Original loads these two vars at once.
  remaining_seconds_x2 = state->tick_remaining_seconds_x2;
  remaining_subseconds = state->tick_remaining_subseconds;
  if (--remaining_subseconds == 0) {
    remaining_subseconds = 6; // game timing dependent
    --remaining_seconds_x2;

    // Play a "bip" or a "bow" sound effect every half second (this is why we double the countdown)
    effect = (remaining_seconds_x2 & 1) ? EFFECT_BOW : EFFECT_BIP; // odd = bow ($9C92 JR NC keeps bip)
    start_sfx(state, effect, 1); /* priority 1 => high */

    if (remaining_seconds_x2 == 0) {
      state->quit_state    = QUITSTATE_START;
      state->time_up_state = TIMEUPSTATE_WAITING;
    }
  }

  state->tick_remaining_seconds_x2 = remaining_seconds_x2;
  state->tick_remaining_subseconds = remaining_subseconds;

  seconds = remaining_seconds_x2 >> 1;

  time_digits = &state->continue_messages[CONTINUE_MESSAGES_TIME_NN]; // Load address of nn in "TIME nn"
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
 * $9C79: Trigger the "START" voice sample
 *
 * Tail-calls play_speech_hook with SAMPLE_START (index 5). In the Z80 this is a
 * JP rather than CALL/RET; in C it is a regular call with the same effect.
 * Called from check_time_up when the countdown reaches zero and the
 * continue-mission sequence begins.
 *
 * Conv: Z80 JP $83C7 is a tail call; C uses a normal call.
 *
 * \param[in] state Pointer to game state.
 */
static void play_start_noise(chqstate_t *state)
{
  play_speech_hook(state, SAMPLE_START);
}

/**
 * $9CC2: Award a speed-proportional score increment
 *
 * Derives a small BCD bonus from the current speed and adds it to the running
 * score via increment_score. The derivation is unusual: the low byte of speed
 * is rotated using the carry from the high byte (RR H; RL A), then divided by
 * four (SRL×2), then BCD-corrected and any remaining carry is folded in. The
 * result is treated as the low BCD digit pair of the increment; the high pairs
 * are zero.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 comment in skool notes "This code makes little sense" — the
 * rotation direction and carry handling appear to be a coding quirk rather than
 * an intentional algorithm.
 */
static void speed_score(chqstate_t *state)
{
  int carry; /* carry from RR/RL shift operations (carry) */
  u16 speed; /* current speed value (was HL) */
  u8  A;     /* speed low byte, shifted and BCD-corrected for score increment (was A) */
  u8  H;     /* speed high byte, bottom bit merged into A via RR then RL (was H) */

  carry = 0;

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
  DAA_add(A, &carry);
  increment_score(state, A, 0, 0); /* tail call */
}

/**
 * $9CD6: Format a BCD bonus value as a decimal string and add it to the score
 *
 * Formats the six packed-BCD digits of ([D_hi], [E_md], [A_lo]) as a
 * right-aligned decimal string in state->bonus_string, suppressing leading and
 * trailing zeros. A non-zero-to-zero digit transition terminates the chain
 * early (see bonus_digit), so bonus values with an internal zero (e.g. 50500)
 * are not rendered correctly — this is a Z80 original bug. Sets
 * SM_address_of_score_digits to point to the first significant digit, sets
 * trigger_bonus_flag, then falls through to increment_score to add the bonus to
 * the running score.
 *
 * The Z80 fills the string right-to-left starting from one byte past the end of
 * the six-byte buffer ($9D57). C models this with a double pointer (pHLoutput).
 *
 * \param[in] state Pointer to game state.
 * \param[in] A_lo Low two BCD digits of the bonus. (was A)
 * \param[in] E_md Middle two BCD digits of the bonus. (was E)
 * \param[in] D_hi High two BCD digits of the bonus. (was D)
 */
static void add_bonus(chqstate_t *state, int A_lo, int E_md, int D_hi)
{
  char *HLoutput;  /* pointer walking bonus_string right-to-left (was HL) */
  int   Czeroflag; /* $FF until first non-zero digit is seen, then 0 (was C) */

  HLoutput  = &state->bonus_string[6]; /* one past the 6-byte buffer, like Z80's $9D57 */
  Czeroflag = 0xFF;
  (void) bonus_digit(A_lo >> 0, &Czeroflag, &HLoutput);
  *HLoutput |= EOS; /* Conv: SET 7,(HL) — top-bit-terminate the lowest digit */

  /* Conv: Z80 alternates CALL $9CF8 (bonus_high_nibble: RRA×4 then fall into
   * $9CFC) and CALL $9CFC (bonus_digit) for each byte.  C passes (value >> 4)
   * directly and uses short-circuit && so a -1 return terminates the chain,
   * mirroring the Z80's POP AF early-exit. */
  (void)(bonus_digit(A_lo >> 4, &Czeroflag, &HLoutput) >= 0 &&
         bonus_digit(E_md >> 0, &Czeroflag, &HLoutput) >= 0 &&
         bonus_digit(E_md >> 4, &Czeroflag, &HLoutput) >= 0 &&
         bonus_digit(D_hi >> 0, &Czeroflag, &HLoutput) >= 0 &&
         bonus_digit(D_hi >> 4, &Czeroflag, &HLoutput) >= 0);

  state->SM_address_of_score_digits = (const u8 *)HLoutput;
  state->trigger_bonus_flag         = 1;
  increment_score(state, A_lo, E_md, D_hi); /* Conv: Z80 falls through to $9D17 */
}

/**
 * $9CFC: Write one BCD nibble into the bonus string
 *
 * Masks the low nibble of [Adigit] and writes the corresponding decimal
 * character into the bonus string at (*[pHLoutput] - 1), filling right-to-left
 * (least-significant digit first). Tracks whether any non-zero digit has been
 * seen via *[pCzeroflag] ($FF = all-zero so far, 0 = at least one non-zero
 * seen).
 *
 * The entry point at $9CF8 (bonus_high_nibble) RRA×4-shifts the high nibble
 * into the low nibble before reaching this code; in C, callers pass (value >>
 * 4) directly so bonus_digit is always entered at $9CFC.
 *
 * Returns -1 on a non-zero-to-zero digit transition (suppressing trailing
 * zeros). The Z80 models this with POP AF to discard the return address and
 * jump to bonus_exit; C callers use short-circuit && to achieve the same
 * early-exit behaviour.
 *
 * \param[in]     Adigit BCD digit to write; only the low nibble is used.
 *   (was A)
 * \param[in,out] pCzeroflag Flag: $FF while all digits so far are zero, 0 once
 * a non-zero digit is seen. (was C)
 * \param[in,out] pHLoutput Address of the output pointer; decremented before
 * each write. (was HL)
 * \return 0 on success; -1 on non-zero-to-zero transition (stop writing).
 */
static int bonus_digit(int Adigit, int *pCzeroflag, char **pHLoutput)
{
  Adigit &= 0x0F;

  if (Adigit == 0) {
    if (*pCzeroflag != 0)
      goto bd_store;
    /* Conv: Z80 uses POP AF to discard the return address and jump to
     * bonus_exit, causing the caller's call chain to terminate early.
     * C models this as return -1; callers use short-circuit &&. */
    return -1;
  }

  *pCzeroflag = 0;
bd_store:
  (*pHLoutput)--;
  **pHLoutput = Adigit + '0';
  return 0;
}

/**
 * $9D17: Increment the score by a three-byte BCD value
 *
 * Adds ([D_hi], [E_md], [A_lo]) to the four-byte packed-BCD score in
 * state->score_bcd[0..3], propagating carry through all four bytes. Each byte
 * holds two decimal digits. The fourth byte absorbs any carry out of the high
 * byte so the score never wraps silently.
 *
 * \param[in] state Pointer to game state.
 * \param[in] A_lo Low two BCD digits of the increment. (was A)
 * \param[in] E_md Middle two BCD digits of the increment. (was E)
 * \param[in] D_hi High two BCD digits of the increment. (was D)
 */
static void increment_score(chqstate_t *state, int A_lo, int E_md, int D_hi)
{
  int  carry;        /* carry flag propagated between BCD additions (was carry flag) */
  u8  *HLscore_bcd; /* pointer walking state->score_bcd (was HL) */

  carry        = 0;
  HLscore_bcd  = &state->score_bcd[0];
  *HLscore_bcd = DAA_add(A_lo + *HLscore_bcd,          &carry); HLscore_bcd++;
  *HLscore_bcd = DAA_add(E_md + *HLscore_bcd + carry,  &carry); HLscore_bcd++;
  *HLscore_bcd = DAA_add(D_hi + *HLscore_bcd + carry,  &carry); HLscore_bcd++;
  *HLscore_bcd = DAA_add(*HLscore_bcd + carry, NULL);
}

/**
 * $9D2E: Calculate and apply the overtake bonus
 *
 * Called once per frame from the main loop. Runs overtake_bonus_counter
 * iterations (one per pending overtake), each time advancing the BCD
 * accumulator overtake_bonus_bcd by 2 (clamped to $80) and calling add_bonus
 * with that value as the middle-digit pair (E_md), so the bonus applied each
 * iteration is accumulator × 100 (e.g. $02 → 200, $04 → 400). After all
 * iterations the counter is cleared.
 *
 * Conv: The Z80 uses EXX before calling add_bonus ($9CD6) to bank B and HL into
 * shadow registers, protecting them from clobbering during the call, and a
 * second EXX on return to restore them. C has no register pressure so the
 * banking is omitted — Biterations and HLbcd are plain locals that survive the
 * call naturally.
 *
 * \param[in] state Pointer to game state.
 */
static void calc_overtake_bonus(chqstate_t *state)
{
  int  Acounter;    /* BCD accumulator: current overtake bonus value (was A) */
  int  Biterations; /* loop count: number of pending overtakes (was B) */
  u8  *HLbcd;       /* pointer to state->overtake_bonus_bcd (was HL) */

  Acounter = state->overtake_bonus_counter;
  if (Acounter == 0)
    return;

  Biterations = Acounter;
  HLbcd       = &state->overtake_bonus_bcd;
  do {
    Acounter = DAA_add(*HLbcd + 2, NULL); /* ADD A,$02; DAA */
    if (Acounter >= 0x80) Acounter = 0x80;
    *HLbcd = Acounter;
    add_bonus(state, 0, Acounter, 0); /* bonus = Acounter * 100 */
  } while (--Biterations > 0);

  state->overtake_bonus_counter = 0;
}

/**
 * $9D62: Refresh the scoreboard HUD for the current frame
 *
 * Covers five sections in sequence:
 *
 * 1. Stage display ($9D62): on the first call after a stage change
 * (displayed_stage == 0), formats the current stage number into the "STAGE N"
 * string, marks it displayed, and draws it to screen $4486.
 *
 * 2. Bonus trigger ($9D7C us_bonus_start): if trigger_bonus_flag is set, clears
 * the 5×7-pixel bonus area at screen $4168 and redraws it from
 * SM_address_of_score_digits (the start of the significant digits in
 * bonus_string, set by add_bonus via the SM field at $9D9C). Resets
 * bonus_counter to 8 to start the flash sequence.
 *
 * 3. Bonus flash ($9DAC us_bonus_set_counter): decrements bonus_counter each
 * frame while it is non-zero. Uses SRL to derive a colour: zero → black, odd →
 * bright red, even → bright yellow. Writes the colour to the five attribute
 * cells at $5868 covering the bonus digit area.
 *
 * 4. Gear display ($9DC3 us_gear): redraws the "HI"/"LO" gear string at screen
 * $448E only when gear differs from session.displayed_gear.
 *
 * 5. Lights ($9DDC us_lights): when a perp is sighted and frame_toggle is set,
 * toggles the BRIGHT bit on the two marquee light attribute blocks. Falls
 * through to plot_turbos_and_digits.
 *
 * Conv: The Z80 instruction at $9D9B is self-modified by add_bonus ($9D0E LD
 * ($9D9C),HL) to point HL at the first significant digit of bonus_string. C
 * models this via state->SM_address_of_score_digits. Conv: The five-byte clear
 * loop ($9D8D) and the five-cell attribute write ($9DBF) are replaced by
 * memset. Conv: INC H to advance one ZX scanline is modelled as += 256 in the
 * pixel array. Conv: draw_string_screen passes 0/dummy attrs because style=0
 * ignores them.
 *
 * \param[in] state Pointer to game state.
 */
static void update_scoreboard(chqstate_t *state)
{
  /* $9D57: "HI"/"LO" gear strings */
  static const u8 gear_hi[] = { 'H', 'I' | EOS };
  static const u8 gear_lo[] = { 'L', 'O' | EOS };

  int  A_stagechar;  /* stage digit as EOS-terminated ASCII character (was A) */
  int  A_bonus_flag; /* trigger_bonus_flag snapshot (was A) */
  u8  *HL_screen;    /* pixel pointer walking the bonus clear area (was HL) */
  int  B_iterations; /* scanline loop counter for the bonus clear (was B) */
  int  A_counter;    /* bonus counter; shared between trigger and countdown paths (was A) */
  int  carry;        /* bit 0 of A_counter before SRL; selects red vs yellow (was carry) */
  int  C_attrval;    /* bonus flash attribute colour written to five cells (was C) */
  int  A_gear;       /* current gear value (was A) */

  /* $9D62 — stage display: draw stage text once per stage */
  if (state->displayed_stage == 0) {
    A_stagechar            = ('0' + state->wanted_stage_number) | EOS;
    state->stage_n[6]      = A_stagechar;
    state->displayed_stage = A_stagechar;
    draw_string_screen(state,
                       0,
                       ADDRTOATTRS(SCREEN_ATTRIBUTES_START_ADDRESS), /* Conv: dummy */
                       ADDRTOSCREEN(0x4486),
                       &state->stage_n[0]);
  }

  /* $9D7C us_bonus_start — draw new bonus value if flagged */
  A_bonus_flag = state->trigger_bonus_flag;
  if (A_bonus_flag != 0) {
    state->trigger_bonus_flag = 0;
    /* $9D86 — clear 5×7 pixel area at screen $4168 */
    HL_screen    = ADDRTOSCREEN(0x4168);
    B_iterations = 7;
    do {
      memset(HL_screen, 0, 5);
      HL_screen += 256; /* Conv: INC H = one ZX scanline = +256 in pixel array */
    } while (--B_iterations);
    /* $9D9B — draw bonus digits; HL is self-modified by add_bonus ($9D0E) */
    draw_string_screen(state,
                       0,
                       ADDRTOATTRS(SCREEN_ATTRIBUTES_START_ADDRESS), /* Conv: dummy */
                       ADDRTOSCREEN(0x4168),
                       state->SM_address_of_score_digits);
    A_counter = 8;
  } else {
    /* $9DA5 us_bonus_countdown — decrement bonus flash counter */
    A_counter = state->bonus_counter;
    if (A_counter == 0)
      goto us_gear;
    A_counter--;
  }

  /* $9DAC us_bonus_set_counter */
  state->bonus_counter = A_counter;
  /* zero = no colour, odd = red, even = yellow */
  carry     = A_counter & 1;
  A_counter >>= 1;
  C_attrval  = A_counter;
  if (C_attrval != 0)
    C_attrval = carry ? attribute_BRIGHT_RED_OVER_BLACK : attribute_BRIGHT_YELLOW_OVER_BLACK;
  /* $9DBA — write colour to 5 attribute cells at $5868 */
  memset(ADDRTOATTRS(0x5868), C_attrval, 5);

us_gear:
  /* $9DC3 — gear display: redraw only on change */
  A_gear = state->gear;
  if (A_gear != state->session.displayed_gear) {
    state->session.displayed_gear = (u8)A_gear;
    /* Conv: Z80 AND A / JR Z selects LO string when A=0; collapsed to ternary */
    draw_string_screen(state,
                       0,
                       ADDRTOATTRS(SCREEN_ATTRIBUTES_START_ADDRESS), /* Conv: dummy */
                       ADDRTOSCREEN(0x448E),
                       (A_gear == 0) ? gear_lo : gear_hi);
  }

  /* $9DDC us_lights — toggle marquee lights while perp is sighted */
  if (state->sighted_flag & state->frame_toggle) {
    toggle_light_brightness(state, ADDRTOATTRS(MARQUEELIGHT_LEFT_ATTR_ADDR));
    toggle_light_brightness(state, ADDRTOATTRS(MARQUEELIGHT_RIGHT_ATTR_ADDR));
  }

  plot_turbos_and_digits(state);

  state->speccy->draw(state->speccy, NULL); /* Conv: added */
}

/**
 * $9DF4: XOR the BRIGHT bit across the marquee light attribute block
 *
 * Toggles the BRIGHT attribute ($40) across a MARQUEELIGHT_HEIGHT ×
 * MARQUEELIGHT_WIDTH (4 × 5) block of screen attribute bytes starting at
 * [attrs]. Each row is processed by XORing five consecutive bytes, then
 * advancing to the next attribute row (+ SCREEN_ATTRIBUTES_ROWBYTES).
 *
 * Conv: The Z80 uses INC L to step within the attribute page, relying on L
 * wrapping within a 256-byte page boundary. C uses a plain pointer which stays
 * in-bounds for the same reason (the block fits within one page).
 *
 * \param[in]     state Pointer to game state.
 * \param[in,out] attrs Pointer to the first attribute byte of the light block.
 *   (was HL)
 */
static void toggle_light_brightness(chqstate_t *state, u8 *attrs)
{
  static const zxbox_t lights_box = { /* marquee lights: ZX rows 8–39, full width */
    0, 152, SCREEN_WIDTH, 184
  };
  int B_rows; /* row counter, MARQUEELIGHT_HEIGHT down to 1 (was B) */
  int C_attr; /* attribute XOR mask, $40 = BRIGHT (was C) */

  B_rows = MARQUEELIGHT_HEIGHT;
  C_attr = ATTR_BRIGHT;
  do {
    *attrs++ ^= C_attr;
    *attrs++ ^= C_attr;
    *attrs++ ^= C_attr;
    *attrs++ ^= C_attr;
    *attrs   ^= C_attr; /* fifth byte (INC L not applied after last) */
    attrs += SCREEN_ATTRIBUTES_ROWBYTES - (MARQUEELIGHT_WIDTH - 1);
  } while (--B_rows > 0);
  state->speccy->draw(state->speccy, &lights_box); /* Conv: added */
}

/**
 * $9E11: Plot turbo boost sprites and all HUD digit displays
 *
 * Covers three sections:
 *
 * 1. Turbo sprites ($9E11): draws one 2-wide × 14-high back-buffer sprite for
 * each remaining turbo boost at successive column offsets. The last turbo uses
 * the current spin-animation frame; all others use frame 0 (the resting
 * position).
 *
 * 2. Speed digits ($9E7B): scales the internal speed (0–511) by 82%, then
 * extracts 10,000s, 1,000s and 100s digits and plots them with ledfont_plot.
 *
 * 3. Time/distance/score digits ($9EC7–$9F12): delegates to ptad_led_digits for
 * each of the three remaining HUD digit groups.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 uses SP as a fast bitmap source pointer (LD SP,HL; POP DE); C uses
 * a typed u16* (SPbitmap) with explicit *SPbitmap++ reads. Conv: Z80
 * self-modifies $9E45 to store the animation frame address and $9E79 to restore
 * SP; C uses local variables SM_9e45 and restores nothing. Conv: EXX banks main
 * DE/HL/BC into shadow registers before the speed multiply; C uses distinct
 * DEdash/HLdash/BCdash names.
 */
static void plot_turbos_and_digits(chqstate_t *state)
{
  int        carry;           /* carry from RL/SBC operations in the speed multiply (carry) */
  int        Aturbos;         /* number of turbo boost sprites remaining to draw (was A) */
  int        Cturbos;         /* turbo countdown; decremented to select frame (was C) */
  int        Aboost;          /* current boost time: non-zero means turbos are spinning (was A) */
  const u8  *HLbitmap;        /* pointer to the turbo sprite frame to draw (was HL) */
  int        Aframe;          /* turbo spin animation frame index 0–2 (was A, SM $9E22) */
  const u16 *SM_9e45;         /* frame data pointer for the last turbo sprite (was SM $9E45) */
  int        A;               /* back-buffer column offset for each turbo position (was A) */
  const u16 *SPbitmap;        /* pointer walking the turbo frame bitmap data (was SP) */
  u8        *HLbackbuf;       /* back-buffer pointer for sprite row writes (was HL) */
  int        B;               /* row counter for the sprite draw loop, TURBOHEIGHT down to 1 (was B) */
  int        DEbitmap;        /* combined mask+bitmap word from the frame data (was DE) */
  int        Emask;           /* pixel mask byte extracted from DEbitmap (was E) */
  int        Dbitmap;         /* pixel bitmap byte extracted from DEbitmap (was D) */
  u8        *DEscreen;        /* pointer to the speed digit area on screen (was DE) */
  int        DEdash_speed;    /* raw speed value, banked to shadow DE (was DE') */
  int        HLdash;          /* accumulated scaled speed value (was HL') */
  int        Bdash_iterations;/* bit count for the multiply loop, 7 iterations (was B') */
  u8         Ascale;          /* scale factor 82, RLA-shifted through for multiply (was A') */
  int        BCdash;          /* divisor for digit extraction: 10000, 1000, or 100 (was BC') */
  int        Ddash;           /* extracted 10,000s digit (was D') */
  int        Edash;           /* extracted 1,000s digit (was E') */
  u8        *DEbcd;           /* pointer into distance_bcd for conversion output (was DE) */
  int        HLdistance;      /* distance to perp in integer units (was HL) */
  int        BCdivisor;       /* divisor for distance digit extraction: 1000, 100 or 10 (was BC) */

  carry = 0;

  Aturbos = state->session.turbos;
  if (Aturbos) {

    Cturbos = Aturbos;
    Aboost = state->boost;
    HLbitmap = &bitmap_turbospin[0];
    if (Aboost == 0)
      goto ptad_turbo_setup;

    Aframe = state->turbo_spin_frame + 1;
    if (Aframe == 3)
      Aframe = 0;
    state->turbo_spin_frame = Aframe;
    if (Aframe == 0)
      goto ptad_turbo_setup;

    HLbitmap += 56 * Aframe;

ptad_turbo_setup:
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
        DEbitmap = *SPbitmap++; // POP DEbitmap
        Emask = DEbitmap & 0xFF;
        Dbitmap = DEbitmap >> 8;
        *HLbackbuf = (*HLbackbuf & Emask) | Dbitmap, HLbackbuf++;

        DEbitmap = *SPbitmap++; // POP DEbitmap
        Emask = DEbitmap & 0xFF;
        Dbitmap = DEbitmap >> 8;
        *HLbackbuf = (*HLbackbuf & Emask) | Dbitmap, HLbackbuf--;

        HLbackbuf = ADDRTOBACKBUF(prev_buf_row(BACKBUFTOADDR(HLbackbuf)));
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
  DEscreen = ledfont_plot(Ddash, DEscreen); // draw 10,000s
  DEscreen = ledfont_plot(Edash, DEscreen); // draw  1,000s
  (void) ledfont_plot(A, DEscreen); // draw    100s

  // Time
  // EXX
  ptad_led_digits(1, &state->session.time_bcd,
                  &state->session.time_digits[1],
                  ADDRTOSCREEN(0x412F)); // (120,9)

  // Distance (to perp)

  DEbcd = &state->distance_bcd[1];
  // hazard_lane_OR_perp_dist_hi is the high byte of the perp's distance
  HLdistance = (state->hazards[0].hazard_lane_OR_perp_dist_hi << 8) |
               state->hazards[0].distance;
  assert(HLdistance >= 0 && HLdistance < 10000);

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

  ptad_led_digits(2, &state->distance_bcd[1],
                  &state->session.distance_digits[3],
                  ADDRTOSCREEN(0x4191)); /* was fallthrough */

  // Score

  ptad_led_digits(4, &state->score_bcd[3], &state->session.score_digits[7],
                  ADDRTOSCREEN(0x4126)); /* was fallthrough */
}

/**
 * $9F1E: Selectively redraw changed LED digit pairs
 *
 * Walks a packed-BCD buffer and a parallel "already-drawn" shadow buffer. For
 * each digit pair, unpacks the high nibble and low nibble separately; if either
 * digit differs from the shadow, updates the shadow and calls ledfont_plot to
 * redraw it. Unchanged [digits] advance the [screen] pointer without a redraw,
 * saving time.
 *
 * \param[in]     iterations Number of BCD byte pairs to process. (was B)
 * \param[in]     digits Pointer to the end of the packed-BCD source buffer;
 * walked backwards one byte per pair. (was DE)
 * \param[in,out] stored Pointer to the end of the shadow digit buffer;
 * updated in-place when a digit changes. (was HL)
 * \param[in,out] screen Pointer to the screen column for the first digit;
 * advanced one column per plotted or skipped digit. (was DE')
 */
static void ptad_led_digits(int         iterations,
                            const u8   *digits,
                            u8         *stored,
                            u8         *screen)
{
  int Adigits; /* packed BCD digit pair read from the digits buffer (was A) */
  int Cdigits; /* saved copy of Adigits for the low-nibble pass (was C) */

  do {
    Adigits = *digits;
    Cdigits = Adigits; //tmp copy

    Adigits >>= 4;
    if (Adigits != *stored)
      goto ptad_led_plot_1st;
    screen++; // move screen pos

ptad_led_next_half:
    stored--;
    Adigits = Cdigits & 0x0F;
    if (Adigits != *stored)
      goto ptad_led_plot_2nd;
    screen++; // move screen pos

ptad_led_next_whole:
    stored--;
    digits--;
  } while (--iterations > 0);
  return;

ptad_led_plot_1st:
  *stored = Adigits;
  screen = ledfont_plot(Adigits, screen);
  goto ptad_led_next_half;

ptad_led_plot_2nd:
  *stored = Adigits;
  screen = ledfont_plot(Adigits, screen);
  goto ptad_led_next_whole;
}

/**
 * $9F47: Plot one LED font digit to the screen
 *
 * Draws a single 8×15 LED font glyph at the given [screen] pointer. The glyph
 * is stored as 15 bytes: the first 7 occupy the top 7 scanlines of the first
 * character row, then the final 8 span the top 8 scanlines of the next
 * character row below. The Z80 uses LDI with INC D/DEC E to step one scanline
 * down while holding the column fixed; C models this with += 256 (one scanline)
 * and -= 256 + 32 (one scanline up and one column right) to advance to the
 * second character row.
 *
 * \param[in]     ord Digit index 0–9. (was A)
 * \param[in,out] screen Pointer to the screen byte to draw the digit at.
 *   (was DE')
 * \return Pointer to the next digit column (orig_screen + 1).
 *
 * Conv: Z80 uses EXX to bank main registers around LDI; C uses plain locals.
 * Conv: LDI (HL→DE, both increment, BC--) unrolled to indexed loops.
 */
static u8 *ledfont_plot(int ord, u8 *screen)
{
  const u8 *src;         /* pointer walking the LED font glyph data (was HL) */
  u8       *orig_screen; /* screen start for this digit, saved for next-column advance (was PUSH DE) */
  int       i;           /* loop index for unrolled LDI sequences (Conv: no Z80 register) */

  assert(ord >= 0 && ord < 10);

  src = &ledfont[ord * LEDFONT_HEIGHT];
  orig_screen = screen;
  for (i = 0; i < 7; i++) { *screen = *src++; screen += 256; } /* Conv: rolled */
  screen = orig_screen - 256 + 32;
  for (i = 0; i < 8; i++) { *screen = *src++; screen += 256; } /* Conv: rolled */
  return orig_screen + 1;
}

//0b_010BBLLL_RRRCCCCC (B = band, L = scanline, R = row (group), C = column)

/**
 * $9F99: Draw a NUL-terminated string to the back buffer with a specified style
 *
 * Adapter entry point called from print_message ($8E6C) and keyscan_keydefs
 * ($A112). Reorders parameters to match the Z80 register layout expected by
 * draw_string_core and supplies the constant attribute stride of 32 bytes (one
 * attribute row).
 *
 * \param[in] state Pointer to game state.
 * \param[in] attrval Attribute byte to write at each character cell. (was A)
 * \param[in] attrs Pointer to the first screen attribute to write. (was BC)
 * \param[in] backbuf Pointer to the first back-buffer byte to write. (was DE)
 * \param[in] string NUL-terminated (top-bit-set) string data. (was HL)
 * \param[in] style Draw style selector (e.g. DRAWCHARSTYLE_SINGLE). (was A')
 * \return Pointer to the byte after the NUL terminator.
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
                          string,                /* HL */
                          style,                 /* A' */
                          attrval,               /* C' */
                          32,                    /* DE' — one attribute row */
                          attrs);                /* HL' */
}

/**
 * $9FA3: Draw a NUL-terminated string directly to the screen bitmap
 *
 * Wrapper around draw_string_core that fixes the draw style to
 * DRAWCHARSTYLE_SCREEN and supplies the constant attribute stride of 32.
 * Equivalent to draw_string_with_style with style = DRAWCHARSTYLE_SCREEN.
 *
 * \param[in] state Pointer to game state.
 * \param[in] attrval Attribute byte to write at each character cell. (was A)
 * \param[in] attrs Pointer to the first screen attribute to write. (was BC)
 * \param[in] dst Pointer to the first screen bitmap byte to write. (was DE)
 * \param[in] string NUL-terminated (top-bit-set) string data. (was HL)
 * \return Pointer to the byte after the NUL terminator.
 */
static const u8 *draw_string_screen(chqstate_t *state,
                                     int         attrval,
                                     u8         *attrs,
                                     u8         *dst,
                                     const u8   *string)
{
  return draw_string_core(state,
                          dst,
                          string,                /* HL */
                          DRAWCHARSTYLE_SCREEN,  /* A' */
                          attrval,               /* C' */
                          32,                    /* DE' — one attribute row */
                          attrs);                /* HL' */
}

/**
 * $9FA6: Draw a NUL-terminated string to the screen or back buffer
 *
 * Core string-rendering loop. Reads each character byte, masks off the EOS
 * (top-bit) sentinel, passes it to draw_char, then advances the destination and
 * attribute pointers. Stops after the byte with the EOS bit set.
 *
 * \param[in]     state Pointer to game state.
 * \param[in,out] dst Destination bitmap pointer (screen or back buffer).
 *   (was DE)
 * \param[in]     string NUL-terminated (top-bit-set) string data. (was HL)
 * \param[in]     style Draw style selector. (was A')
 * \param[in]     attrval Attribute byte to write at each cell. (was C')
 * \param[in]     attrsstride Bytes between successive attribute rows. (was DE')
 * \param[in,out] attrs Pointer to the first screen attribute to write.
 *   (was HL')
 * \return Pointer to the byte after the NUL terminator.
 */
static const u8 *draw_string_core(chqstate_t *state,
                                  u8         *dst,
                                  const u8   *string,
                                  int         style,
                                  int         attrval,
                                  int         attrsstride,
                                  u8         *attrs)
{
  int A_char; /* current character (EOS bit masked off) (was A) */

  do {
    A_char = *string & ~EOS;
    draw_char(state, A_char, dst, style, attrval, attrsstride, attrs,
              &dst, &attrs);
  } while ((*string++ & EOS) == 0);

  return string;
}

/**
 * $9FB4: Draw one character glyph to the screen or back buffer
 *
 * Maps an ASCII [character] to a glyph index, then dispatches on [style] to one
 * of six render modes: - DRAWCHARSTYLE_SCREEN (1): single-height, directly to
 * screen with ZX scanline row-advance. - DRAWCHARSTYLE_SINGLE (2):
 * single-height to back buffer (9 scanlines: blank, 7 glyph rows, blank). -
 * DRAWCHARSTYLE_DOUBLE (3): double-height to back buffer (each row repeated on
 * two consecutive scanlines). - DRAWCHARSTYLE_SINGLE_INV (4): single-height,
 * inverted. - DRAWCHARSTYLE_DOUBLE_INV (5): double-height, inverted. - 0:
 * double-height via two separate 4- and 3-row passes with a column advance
 * mid-glyph. Writes the attribute byte ([attrval]) to the attribute buffer for
 * single-row styles and to two rows for double-height styles.
 *
 * \param[in]  state Pointer to game state.
 * \param[in]  character ASCII character to draw. (was A)
 * \param[in]  dst Destination: screen or back-buffer pointer. (was DE)
 * \param[in]  style Render style selector (0–5). (was A')
 * \param[in]  attrval Attribute byte to OR into the attribute cells. (was C')
 * \param[in]  attrstride Bytes between successive attribute rows. (was DE')
 * \param[in]  attrs Pointer to the attribute cell for this [character].
 *   (was HL')
 * \param[out] new_screen Updated screen pointer after drawing. (was DE on exit)
 * \param[out] new_attrs Updated attribute pointer after drawing. (was HL' on
 *   exit)
 *
 * Conv: Z80 uses EX AF,AF' / EXX to bank [style] and attribute registers; C
 * passes all values as explicit parameters. Conv: Z80 dispatch is a DEC C; JP Z
 * ladder; C uses a switch.
 */
static void draw_char(chqstate_t *state,
                      int         character,
                      u8         *dst,
                      int         style,
                      int         attrval,
                      int         attrstride,
                      u8         *attrs,
                      u8        **new_screen,
                      u8        **new_attrs)
{
  int       glyphid;    /* glyph index into the font table (was C) */
  int       data;       /* font byte for the current scanline row (was A) */
  int       iterations; /* row countdown for each drawing loop (was B) */
  const u8 *fontdata;   /* pointer to current row of the glyph in font[] (was HL) */
  u8       *orig;       /* saved dst start; restored to advance one column after drawing (was PUSH DE) */
  int       i;          /* loop index for rolled-up row copy (Conv: no Z80 register) */

  assert(VALID_SCREEN_PTR(dst) || VALID_BACKBUF_PTR(dst));
  assert(style <= DRAWCHARSTYLE__LIMIT);
  assert(VALID_ATTRS_PTR(attrs));

  character -= ' ';
  if (character == 0) {
    // Space
    dst++;
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
  case 1: goto dc_screen;
  case 2: goto dc_single_height;
  case 3: goto dc_double_height;
  case 4: goto dc_single_height_inverted;
  case 5: goto dc_double_height_inverted;
  case 0: break;
  default: assert(0);
  }

  // Otherwise it's type 0 or anything else
  orig = dst;
  iterations = 4;
  do {
    data = *fontdata++;
    *dst = data;
    dst += 256;
    *dst = data;
    dst += 256;
  } while (--iterations > 0);
  dst -= 8 * 256;
  dst += 32;
  iterations = 3;
  do {
    data = *fontdata++;
    *dst = data;
    dst += 256;
    *dst = data;
    dst += 256;
  } while (--iterations > 0);
  goto dc_set_double_attrs;

  // double height inverted
dc_double_height_inverted:
  orig = dst;
  iterations = 7;
  do {
    data = ~*fontdata++;
    *dst = data;
    dst += 256;
    *dst = data;
    dst += 256;
  } while (--iterations > 0);
  goto dc_set_double_attrs;

dc_single_height_inverted:
  orig = dst;
  iterations = 7;
  do {
    *dst = ~*fontdata++;
    dst += 256;
  } while (--iterations > 0);
  goto dc_set_single_attrs;

  // Plots double-height glyphs. dst->dst font->glyph def
dc_double_height:
  orig = dst;
  *dst = 0; // leave gap at top
  dst += 256;
  for (i = 0; i < 7; i++) { // Conv: rolled up
    *dst = *fontdata;
    dst += 256;
    *dst++ = *fontdata++; /* was LDI, could reuse A */
    dst--; /* was DEC E, could remove if dst++ above is dropped */
    dst += 256;
  }
  *dst = 0; // leave gap at bottom

dc_set_double_attrs:
  dst = orig + 1; /* was POP dst, INC E */
  *attrs |= attrval;
  attrs += attrstride;
  *attrs |= attrval;
  attrs -= attrstride; /* was POP attrs */
  attrs++; /* was INC L */
  goto dc_return;

dc_single_height: // seems to store 9 rows
  orig = dst;
  *dst = 0; // leave gap at top
  dst += 256;
  for (i = 0; i < 7; i++) { // Conv: rolled up
    *dst++ = *fontdata++;
    dst--; // could drop
    dst += 256;
  }
  if (dst < &state->backbuffer[BACKBUFFER_LENGTH]) /* Conv: avoid scribble */
    *dst = 0; // leave gap at bottom

dc_set_single_attrs:
  dst = orig + 1; /* was POP dst, INC E */
  *attrs |= attrval;
  attrs++; /* was INC L */
  goto dc_return;

dc_screen:
  orig = dst;
  iterations = 7;
  do {
    *dst = *fontdata++;
    dst = ADDRTOSCREEN(next_scr_row(SCREENTOADDR(dst)));
  } while (--iterations > 0);
  dst = orig + 1; /* was POP dst */

dc_return:
  *new_screen = dst;
  *new_attrs  = attrs;
}

/**
 * $A0D6: Read all active inputs and store the result
 *
 * Scans the keyboard (and optionally the Kempston joystick) and resolves
 * conflicting inputs: - Kempston path: reads the joystick port (5 bits, active
 * high), then scans the keyboard with a 3-bit stop mask and rotates the result
 * into the high bits before merging with the joystick reading. - Keyboard-only
 * path: scans with a 1-bit stop mask, collecting 8 bits. After merging,
 * simultaneous LEFT+RIGHT or UP+DOWN are cleared (both cancel). The result is
 * written to state->user_input and returned.
 *
 * \param[in] state Pointer to game state.
 * \return The new user_input byte.
 *
 * Conv: Z80 PUSH AF/POP DE to shuttle the Kempston reading past the
 * keyscan_keydefs call; C uses a local variable Akempston instead.
 */
static u8 keyscan(chqstate_t *state)
{
  int Akempston;      /* raw Kempston joystick reading, 5 bits active-high (was A) */
  int Akeys;          /* keyboard scan result used for left/right conflict check (was A) */
  int Ekeys;          /* merged input byte: keyboard bits and/or Kempston bits (was E) */
  u8  Aleft_and_right;/* left and right bits isolated for simultaneous-press check (was A) */
  u8  Aorig;          /* working copy of Ekeys during conflict stripping (was A) */

  if (state->kempston_flag) {
    Akempston = state->speccy->in(state->speccy, port_KEMPSTON_JOYSTICK) & 0x1F;
    // PUSH AF
    Akeys = keyscan_keydefs(state, &state->keydefs[0], 0x20); // 3 bits max
    Akeys = (Akeys & 0x07) << 5; /* RRC A x3; AND 0xE0 */
    // POP DE
    Ekeys = Akeys | Akempston;
  } else {
    Ekeys = Akeys = keyscan_keydefs(state, &state->keydefs[0], 0x01); // 8 bits max
  }

  Aleft_and_right = Akeys & (USERINPUTFLAG_RIGHT | USERINPUTFLAG_LEFT);
  Aorig = Ekeys;
  /* if left and right are both pressed then clear them both */
  if (Aleft_and_right == (USERINPUTFLAG_RIGHT | USERINPUTFLAG_LEFT)) {
    Aorig &= ~(USERINPUTFLAG_RIGHT | USERINPUTFLAG_LEFT);
    Ekeys = Aorig;
  }
  /* if up and down are both pressed then clear them both */
  state->user_input = ((Aorig & (USERINPUTFLAG_DOWN | USERINPUTFLAG_UP)) != (USERINPUTFLAG_DOWN | USERINPUTFLAG_UP)) ? Ekeys : Ekeys & ~(USERINPUTFLAG_DOWN | USERINPUTFLAG_UP);
  return state->user_input;
}

/**
 * $A112: Scan a keydef list, rotating each result into an accumulator
 *
 * Walks [HLkeydefs] one byte at a time, passing each byte to keyscan_inner to
 * test whether the corresponding key is pressed. The result (active low in Z80,
 * inverted to active high in C) is left-rotated into [Estopbit]. Scanning
 * continues until the stop bit rotates into carry (i.e. all slots have been
 * filled). The number of keys scanned equals the number of bits between the
 * stop bit's initial position and bit 8.
 *
 * \param[in] state Pointer to game state.
 * \param[in] HLkeydefs Pointer to the keydef byte array. (was HL)
 * \param[in] Estopbit Sentinel: $01 for 8-key scan, $20 for 3-key scan;
 * scanning stops when this bit rotates out of the byte. (was E)
 * \return Packed key state in bits 7..0 (or 7..5 for the 3-key path).
 *
 * Conv: Z80 CCF inverts carry after CALL keyscan_inner (active-low result); C
 * uses logical NOT on the return value instead.
 */
static u8 keyscan_keydefs(chqstate_t *state, const u8 *HLkeydefs, u8 Estopbit)
{
  int carry; /* carry from RL(Estopbit): set when the stop bit rotates out (carry) */

  do {
    carry = !keyscan_inner(state, *HLkeydefs++); // active low<>high
    RL(Estopbit);
  } while (!carry);
  return Estopbit;
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
 * $A399: Test for off-road and object collisions each frame
 *
 * Checks whether the hero car has gone off-road or struck a roadside object:
 *
 * 1. Fork shortcut: if the fork is visible and fork_countdown is zero,
 * delegates entirely to check_fork_scenery_collisions and returns.
 *
 * 2. Left/right edge: reads xpos_road_centre[127] and [126] to determine
 * whether the car is on-road (0), partially off-road (1) or fully off-road (2).
 * Inside a tunnel, off-road means hitting a wall; the road_pos high byte
 * selects which wall. The result is stored in state->off_road and
 * state->ahc_crash_spin.
 *
 * 3. Object collision: reads right- then left-side object IDs from the road
 * buffer, looks up their collision thresholds, and calls csc_hit_scenery if the
 * car's x position falls within the zone.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: EXX at entry banks HLdash_road_pos_a/$0048 and DEdash_road_pos_b/ $01D8
 * into shadow registers as default ahc_road_pos values; a second EXX inside the
 * tunnel path overwrites them with tunnel-specific values. C models both banks
 * as named locals that are written to state fields at store_crash_spin.
 */
static void check_scenery_collisions(chqstate_t *state)
{
  int          carry;             /* carry from RL operations testing the road buffer (carry) */
  int          HLdash_road_pos_a; /* road position A, banked to shadow HL, written to ahc_road_pos_a (was HL') */
  int          DEdash_road_pos_b; /* road position B, banked to shadow DE, written to ahc_road_pos_b (was DE') */
  int          Afork_countdown;   /* fork_countdown snapshot; decremented to detect imminent fork (was A) */
  s16          HLxpos;            /* x position from xpos_road_centre, used for boundary checks (was HL') */
  int          Aoff_road;         /* off-road level: 0=on, 1=one wheel off, 2=both wheels off (was A) */
  int          Ccrash_spin;       /* crash spin type: 0=none, 1=left tunnel wall, 2=right wall (was C) */
  u8          *HLbufptr;          /* pointer into the road buffer at the relevant object offset (was HL') */
  u8           Alanes;            /* road buffer lanes byte, tested for tunnel/fork/dirt flags (was A) */
  int          Ztunnel_body;      /* non-zero when inside a tunnel body (not a portal) (was Z flag) */
  int          Aroad_pos_hi;      /* high byte of road_pos, distinguishes left/right tunnel wall (was A) */
  int          Croad_pos_hi;      /* road_pos high byte saved to shadow C before scenery_hit call (was C') */
  int          Aspeed;            /* speed cap (20) passed to scenery_hit for tunnel wall impact (was A) */
  int          Adash_flip;        /* flip flag for tunnel wall: low bit of road_pos high byte (was A') */
  u8           A;                 /* road buffer forward index used for RL carry test (was A) */
  int          Adash;             /* road buffer offset banked to shadow A via EX AF,AF' (was A') */
  int          raw_byte1;         /* first road-buffer byte before OR (no Z80 register) */
  int          Aobj;              /* object ID byte OR'd from two consecutive road buffer positions (was A) */
  int          raw_byte2;         /* second road-buffer byte after optional pointer advance (no Z80 register) */
  const obj_t *HLobj;             /* pointer to the struck object's data record (was HL') */
  int          BCdash_max;        /* upper x collision threshold from right-side object data (was BC') */
  int          DEdash_min;        /* lower x collision threshold from right-side object data (was DE') */
  int          Aspeed_cap;        /* impact speed cap read from object data (was A') */
  int          BCdash_min;        /* lower x collision threshold from left-side object data (was BC') */
  int          DEdash_max;        /* upper x collision threshold from left-side object data (was DE') */

  carry = 0;

  // Note: EXX is treated as a 'stash' operation in this routine.

  HLdash_road_pos_a = 72;
  DEdash_road_pos_b = 472;
  // EXX - deliberate bank

  if (state->fork_visible) {
    /* The fork is visible. */
    Afork_countdown = state->fork_countdown;
    if (Afork_countdown == 0) {
      /* Conv: parameter order is (DEdash, HLdash) — passing (HL, DE) here
       * stored the road clamp bounds swapped (min 472, max 72), making
       * animate_hero_car slam road_pos to alternate ends every fork frame
       * (the whole-screen left/right flicker). */
      check_fork_scenery_collisions(state, DEdash_road_pos_b, HLdash_road_pos_a); /* tail call */
      return;
    }

    if (--Afork_countdown == 0)
      return;
  }

  // Check left hand side
  //
  // high byte ≠ 0, or value < 64 => trigger_lefthand_sfx = 0; fall through to right hand check
  // 64 – 105                     => on-road; jump to right hand check
  // 106 – 132                    => off_road = 1 (one wheel left of road)
  // ≥ 133                        => off_road = 2 (both wheels left of road)
  //
  // Note that is where an object *could be*. There's not necessarily an
  // object always there.
  HLxpos = state->xpos_road_centre[127];
  if ((HLxpos >> 8) == 0 && HLxpos >= 64) { // 64..255
    if (HLxpos < 106)
      goto check_right_hand; // 64..105 => not close enough to be off-road
    // how far off-road are we? partially/fully off-road is 1/2
    Aoff_road = (HLxpos < 133) ? 1 : 2;
    goto store_off_road;
  }

  // If we don't arrive here we're close to the left hand object
  state->trigger_lefthand_sfx = 0;

  // Check right hand side
  //
  // high byte ≠ 0 (off-screen right) => trigger_righthand_sfx = 0; skip off-road
  // high byte = 0, ≥ 190             => on-road right; trigger_righthand_sfx = 0
  // high byte = 0, 142 – 189         => off_road = 0 (borderline, still on-road), goto store_off_road
  // high byte = 0, 125 – 141         => off_road = 1
  // high byte = 0, < 125             => off_road = 2
  //
  // $A3EE JR NC uses the carry from the $A3EB SBC HL,$8E test, which is still
  // live when $A3F2 SBC HL,$7C executes, making the effective lower threshold
  // $7C+1 = 125 (not $7C = 124).
check_right_hand:
  // "pos" here is approx 75..368 for (centred .. off-screen on the right).
  HLxpos = state->xpos_road_centre[126];
  Aoff_road = 0;
  if ((HLxpos >> 8) == 0 && HLxpos < 190) { // 0..189
    if (HLxpos >= 142) // 142..189 => on-road
      goto store_off_road;
    Aoff_road = (HLxpos >= 125) ? 1 : 2; /* carry from $A3EB makes threshold $7C+1=125 */
    goto store_off_road; /* $A3F5/$A3F8 JR: bypass trigger_righthand_sfx reset */
  }

  // If we don't arrive here we're close to the right hand object
  state->trigger_righthand_sfx = 0;

store_off_road:
  // 0/1/2 => on-road/one wheel off-road/both wheels off-road
  state->off_road = Aoff_road;

  Ccrash_spin = 0;
  if (Aoff_road) {
    // Otherwise we're off-road.

    HLbufptr = ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET);
    Alanes = *HLbufptr;
    if ((Alanes & (1 << 6)) == 0) /* jump if normal road */
      goto store_crash_spin;
    RL(Alanes); /* jump if forked road or dirt track */
    if (carry)
      goto store_crash_spin;

    /* tunnel */

    Ztunnel_body = ((Alanes & (1 << 3)) == 0); // Test lanes bit 2 (note: RLA moved it)
    Aroad_pos_hi = state->scenedata.road_pos >> 8;
    if (!Ztunnel_body) {
      Croad_pos_hi = Aroad_pos_hi; // save road_pos
      Aspeed = 20;
      // EX AF,AF' - bank
      Adash_flip = Croad_pos_hi & 1;
      scenery_hit(state, Adash_flip, Aspeed); /* tail call */
      return;
    }

    HLdash_road_pos_a = 209;
    DEdash_road_pos_b = 405;
    // EXX - deliberate bank ($A42C)

    Ccrash_spin = (Aroad_pos_hi) ? 2 : 1;

    // Hit tunnel wall.
    start_sfx(state, EFFECT_WALL_HIT, 4); /* priority 4 */
  }

store_crash_spin:
  state->ahc_crash_spin = Ccrash_spin; // Conv: use of A removed

  // EXX - unbank ($A43F)
  state->ahc_road_pos_a = HLdash_road_pos_a;
  state->ahc_road_pos_b = DEdash_road_pos_b;
  if (Ccrash_spin) // Conv: use of A removed
    return;

  // -- RIGHT SIDE OBJECT HIT CHECKING --
  HLbufptr = ROADBUF_FWD2PTR(ROADBUF_RIGHTOBJS_OFFSET);
  Adash = ROADBUF_PTR2IDX(HLbufptr); // was EX AF,AF' -- seems to be unused
  A = ROADBUF_FWD2IDX(0);
  RL(A);
  raw_byte1 = *HLbufptr; /* Read a right side object data byte */
  Aobj = raw_byte1;
  if (carry)
    WRAP_INCREMENT_ASSIGN(HLbufptr, state->roadbuf_start);
  raw_byte2 = *HLbufptr;
  Aobj |= raw_byte2;
  if (Aobj) {
    HLobj = &state->stage->addrof_right_hand_objects[Aobj];
    // Read collision values.
    BCdash_max = HLobj->hit_max_or_min; /* Conv: was LD C,(HL) but B already zero */
    DEdash_min = HLobj->hit_min_or_max; /* Conv: was LD E,(HL) but D set to zero */
    Aspeed_cap = HLobj->impact_speed_cap;

    // Check for collisions with scenery (right hand side).
    HLxpos = state->xpos_road_centre[126];
    if (HLxpos < BCdash_max && HLxpos > DEdash_min) { /* > not >=: SBC carry-in=1 at $A478 */
      // EX AF,AF' -- deliberate bank Aspeed_cap
      csc_hit_scenery(state, 0 /* no flip */, Aspeed_cap); /* tail call */
      return;
    }
  }

  // EX AF,AF'  Unbank road buffer offset or/and bank mystery value in A

  // -- LEFT SIDE OBJECT HIT CHECKING --
  HLbufptr = ROADBUF_FWD2PTR(ROADBUF_LEFTOBJS_OFFSET);
  A = ROADBUF_FWD2IDX(0);
  RL(A);
  raw_byte1 = *HLbufptr; /* Read a left side object data byte */
  Aobj = raw_byte1;
  if (carry)
    WRAP_INCREMENT_ASSIGN(HLbufptr, state->roadbuf_start);
  raw_byte2 = *HLbufptr;
  Aobj |= raw_byte2;
  if (Aobj) {
    HLobj = &state->stage->addrof_left_hand_objects[Aobj];
    // Read collision values.
    BCdash_min = HLobj->hit_max_or_min;
    DEdash_max = HLobj->hit_min_or_max;
    Aspeed_cap = HLobj->impact_speed_cap;

    // Check for collisions with scenery (left hand side).
    HLxpos = state->xpos_road_centre[127];
    if (HLxpos >= BCdash_min && HLxpos < DEdash_max) {
      // EX AF,AF' -- deliberate bank Aspeed_cap
      csc_hit_scenery(state, 1 /* flip */, Aspeed_cap); /* was FALLTHROUGH */
    }
  }
}

/**
 * $A4B0: Play the scenery-hit sound then initiate the crash sequence
 *
 * Plays EFFECT_SCENERY_HIT at priority 3, then falls through to scenery_hit to
 * set up the crash state. Called when the hero car drives into a tree, lamp
 * post or other roadside object.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Aflip_flag Flip flag: 0 = right-side hit, 1 = left-side hit.
 * (was A)
 * \param[in] Adash_speed Impact speed cap passed to scenery_hit. (was A')
 */
static void csc_hit_scenery(chqstate_t *state, int Aflip_flag, int Adash_speed)
{
  start_sfx(state, EFFECT_SCENERY_HIT, 3); /* priority 3 */
  scenery_hit(state, Aflip_flag, Adash_speed); /* was FALLTHROUGH */
}

/**
 * $A4B8: Set up the crash state after a scenery or tunnel-wall impact
 *
 * Guards against double-entry (returns immediately if already crashed).
 * Computes the initial crash spin speed as max(24, speed/16 + 16) and the crash
 * speed threshold as min([Adash_threshold], current speed). Writes the crash
 * flags, flip direction, delay counter, spin speed and speed threshold into the
 * appropriate state fields.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Aflip_flag Flip flag: 0 = right-side, 1 = left-side. (was A)
 * \param[in] Adash_threshold Speed cap for the crash: the animation starts
 * at min(speed, threshold). (was A')
 *
 * Conv: Z80 self-modifies operands at $B357 (spin speed) and $B32F (threshold)
 * via LD (addr),HL; C writes directly to state->ahc_crash_spin_speed and
 * state->ahc_crash_speed_threshold.
 */
static void scenery_hit(chqstate_t *state, int Aflip_flag, int Adash_threshold)
{
  int speed;        /* hero car speed at time of impact (was HL) */
  int scaled_speed; /* speed scaled to crash spin rate: speed/16 + 16 (was A) */
  int spin_speed;   /* actual spin speed: max(24, scaled_speed) (was L) */
  int threshold;    /* crash speed cap: min(Adash_threshold, speed) (was HL) */

  if (state->ahc_crashed_flag)
    return; /* already crashed */

  state->ahc_crashed_flag     = 1;
  state->ahc_flip_flag        = Aflip_flag;
  state->ahc_crash_flip_count = Aflip_flag + 1;
  state->ahc_delay            = 5;

  speed = state->speed;
  scaled_speed = (speed >> 4) + 16;

  // i.e. spin_speed = MAX(24, Aspeed);
  spin_speed = 24;
  if (scaled_speed >= spin_speed)
    spin_speed = scaled_speed;
  state->ahc_crash_spin_speed = (u16) spin_speed; /* SRL H at $A4D0 always zeros high byte for speed <= 511 */

  // i.e. HL_threshold = MIN(Adash_threshold, state->speed);
  threshold = Adash_threshold;
  if (threshold >= state->speed)
    threshold = state->speed;
  state->ahc_crash_speed_threshold = threshold;
}

/**
 * $A4F6: Check off-road and pole collisions at a road fork
 *
 * Variant of check_scenery_collisions used when the road fork is visible and
 * fork_countdown has reached zero. Uses xpos_road_left[127] and
 * xpos_road_fork_right[126] rather than the normal centre tables to detect
 * off-road, and checks the short pole object on whichever side of the fork the
 * player did NOT take.
 *
 * \param[in] state Pointer to game state.
 * \param[in] DEdash Road position B value inherited from
 *   check_scenery_collisions;
 * stored directly to ahc_road_pos_b. (was DE')
 * \param[in] HLdash Road position A value inherited from
 *   check_scenery_collisions;
 * stored directly to ahc_road_pos_a. (was HL')
 */
static void check_fork_scenery_collisions(chqstate_t *state,
                                          int         DEdash,
                                          int         HLdash)
{
  int          pos;            /* x position from road table used for off-road check (was HL) */
  int          off_road;       /* off-road level: 0=on, 1=one wheel off, 2=both off (was A) */
  const obj_t *shortpoleobj;   /* pointer to the short pole object on the non-taken fork side (was HL) */
  int          hit_max_or_min; /* upper collision threshold from the pole object data (was BC') */
  int          hit_min_or_max; /* lower collision threshold from the pole object data (was DE') */
  int          A;              /* impact_speed_cap loaded from object but overridden to 0x8C (was A) */
  int          pos2;           /* x position from xpos_road_centre for the final collision test (was HL') */

  // $A4FB/$A4FD: both JR NZ,$A510 (H!=0) and JR C,$A510 (pos<0x6A) fall into
  // the fc_a510 right-hand check below; only pos in [0x6A,0xFF] with H==0
  // resolves off-road here without checking the right side.
  pos = state->xpos_road_left[127];
  if ((pos >> 8) == 0 && pos >= 0x6A) {
    off_road = (pos >= 0x85) ? 2 : 1;
    goto set_off_road;
  }

  pos = state->xpos_road_fork_right[126];
  off_road = 0;
  if ((pos >> 8) == 0)
    if (pos < 0x8E)
      off_road = (pos >= 0x7D) ? 1 : 2;

set_off_road:
  state->off_road        = off_road;
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
      csc_hit_scenery(state, 0 /* no flip */, 0x8C); /* tail call */
  } else {
    // Right fork was taken, short pole object is on left hand of road.
    shortpoleobj = state->stage->addrof_left_hand_short_pole_object;

    // Read collision values
    hit_max_or_min = shortpoleobj->hit_max_or_min; // min
    hit_min_or_max = shortpoleobj->hit_min_or_max; // max

    pos2 = state->xpos_road_centre[126];
    if (pos2 >= hit_max_or_min && pos2 < hit_min_or_max)
      csc_hit_scenery(state, 1 /* flip */, 0x8C); /* tail call */
  }
}

/**
 * $A579: Populate the object x-position pairs for the current frame
 *
 * Two passes over up to 21 road slots:
 *
 * 1. Prefix-sum ($A57F): walks object_positions[] and converts the run of
 * per-slot sizes into cumulative totals, so each entry holds the starting
 * offset of that slot's object strip.
 *
 * 2. Position fill ($A5A7): for each slot reads the lanes byte from the road
 * buffer, then pushes a (left, right) pair of s16 x positions onto a stack that
 * grows down from $EB00 (xpos_road_centre_right[]). The lane offset bits (bits
 * 1:0) select which xpos table to use for the left boundary; bits 7:2 determine
 * the right boundary table via a separate lane-shift calculation. Fork slots
 * use the centre and centre-right tables directly.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 uses SP as a descending stack pointer into $EB00; C uses an
 * explicit s16 pointer SP walked with prefix decrement. Conv: Z80 byte offset
 * L' = (~(IY[0]*2)) & 0xFF; C translates directly to the s16 array index 127 -
 * (objpos2[0] & 0x7F). Conv: EXX banks DE (lanes byte) and IY/HL' (tables)
 * around the inner loop body; C uses distinct named locals.
 */
static void layout_objects(chqstate_t *state)
{
  int       carry;      /* carry from RL(lanesbyte2), selects tunnel vs. fork/dirt path (carry) */
  u8       *objpos;     /* pointer walking object_positions[] in the prefix-sum pass (was HL) */
  int       iterations; /* loop countdown: 21 slots, or fork_countdown for the fork path (was B) */
  int       total;      /* running prefix sum accumulated across object_positions[] (was A) */
  s16      *SP;         /* descending stack pointer into xpos_road_centre_right[] (was SP) */
  u8       *bufptr;     /* pointer to current road-buffer lane byte for this slot (was DE) */
  const u8 *objpos2;    /* pointer walking object_positions[] in the position-fill pass (was IY) */
  int       countdown;  /* fork_countdown snapshot used to split normal vs. fork iterations (was A) */
  int       lanesbyte;  /* lanes byte read from road buffer before EXX bank (was A) */
  u8        lanesbyte2; /* lanes byte after EXX, rotated to test fork/tunnel/dirt flags (was E') */
  int       L;          /* xpos row index for the fork tail pass (was L) */
  int       Ldash;      /* xpos table row index: 127 - (objpos2[0] & 0x7F) (was L') */
  int       laneoffset; /* lane offset bits 1:0 from lanesbyte2, selects left table (was A) */
  s16      *tabptr;     /* pointer into the selected xpos table for right-boundary lookup (was HL') */
  int       laneshift;  /* right-boundary table selector, derived from lanes byte bit pattern (was A) */
  int       A;          /* remaining slots for the fork tail pass: 21 - fork_countdown (was A) */

  carry = 0;

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
      /* Conv: Z80 $A5AA-$A5AF: L' = (~(*IY*2)) & 0xFF — odd byte offset into the
       * xpos page; s16 is read at L'-1 (even) giving index (L'-1)/2 = 127-(N&0x7F).
       * C translates directly to the s16 index rather than the byte offset. */
      Ldash = 127 - ((int)*objpos2 & 0x7F);

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

      /* Right table page = $E8 + (laneoffset-1) + laneshift, i.e. left table + laneshift. */
      switch (laneoffset + laneshift - 1) {
      case 2: tabptr = &state->xpos_road_centre[Ldash]; break;
      case 3: tabptr = &state->xpos_road_centre_right[Ldash]; break;
      case 4: tabptr = &state->xpos_road_right[Ldash]; break;
      case 5: tabptr = &state->xpos_road_fork_right[Ldash]; break;
      default: assert(0);
      }

load_and_store_right:
      --SP; *SP = *tabptr;

      // EXX
      objpos2++;
      WRAP_INCREMENT_ASSIGN(bufptr, state->roadbuf_start);
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
    /* Conv: same byte-offset → s16-index translation as main loop above */
    L = 127 - ((int)*objpos2 & 0x7F);
    --SP; *SP = state->xpos_road_centre[L];
    --SP; *SP = state->xpos_road_centre_right[L];
    objpos2++;
  } while (--iterations > 0);
}

/**
 * $A60E: Advance the three frame-rate counters
 *
 * Called once per frame from the main loop. Updates:
 *
 * anim_counter      cycles 0–3 every frame  ($A234; AND $03) frame_toggle
 * alternates 0/1 every frame  ($A235; XOR $01) slow_anim_counter cycles 0–3
 * every other frame  ($A236; AND $03, only when frame_toggle becomes 1)
 *
 * The Z80 uses RET Z after the XOR to bail when frame_toggle becomes 0 (i.e. on
 * even frames); slow_anim_counter is advanced only on odd frames.
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
 * $A637: perp_behaviour
 *
 * Controls the perp car each frame: reacts to collisions, manages lane changes
 * and scales approach speed by distance.
 *
 * IX[7] (hit_timer) drives a three-way FSM on entry: - positive: just hit;
 * branch to pb_set_delay to apply crash penalty, add bonus and reset timer to
 * $FC. - negative ($FC–$FF): still counting down; increment and return. - zero:
 * normal frame; run full lane/speed management.
 *
 * In the normal path the function first walks the five non-perp hazard slots to
 * check whether any active vehicle is in the same lane and within range,
 * forcing a random lane change if so. It then picks a new lane via a random ±1
 * walk biased by road width, clamps the result to the spawn-lane bounds, slides
 * horz_pos toward the target position and finally scales the perp's approach
 * speed by the remaining distance.
 *
 * \param[in]     state Pointer to game state.
 * \param[in,out] IXperp Perp hazard slot. (was IX)
 */
void perp_behaviour(chqstate_t *state, hazard_t *IXperp)
{
  int       carry;              /* modelled Z80 carry flag: hazard proximity check, then SBC HL,DE carry-in (carry) */
  int       Ahit_timer;         /* hit timer from perp slot: positive=just hit, negative=counting down (was A) */
  int       Cperp_distance;     /* perp's road buffer offset, used for vehicle proximity check (was C) */
  int       Biterations;        /* hazard slot loop counter: 5 iterations (was B) */
  hazard_t *IYhazard;           /* pointer walking the five non-perp hazard slots (was IY) */
  u8        Adistancediff;      /* distance difference between hazard car and perp, mod 256 (was A) */
  u8        Achanging_lane_flag; /* pb_changing_lane flag read (was A) */
  u8        Adist_lane_gate;    /* perp distance compared to 7, gating the lane-change timer path (was A) */
  int       HL;                 /* road position remainder for iterative lane-boundary checks (was HL) */
  int       DE;                 /* lane-boundary step value: 70 (was DE) */
  u8        Alane_timer;        /* lane-change countdown value, decremented then reloaded on zero (was A) */
  int       Bmin_lane;          /* lower lane bound from road-width table (was B) */
  int       Cmax_lane;          /* upper lane bound from road-width table (was C) */
  u8        Acurrlane_bound;    /* current_lane re-read for boundary check against Bmin_lane/Cmax_lane (was A) */
  int       Ccurrentlane;       /* current_lane captured before random ±1 walk (was C) */
  int       Cnewlane;           /* candidate new lane after random ±1 step (was C) */
  int       Acurrlane;          /* perp's current_lane, updated during lane management (was A) */
  int       Cdelta;             /* lane correction delta: +2 or −2 when lane is out of range (was C) */
  int       BCspawn_lanes;      /* packed return from get_spawn_lanes: high byte=min, low byte=max (was BC) */
  int       Bmin_spawn_lane;    /* minimum valid lane from road buffer, unpacked from BCspawn_lanes (was B) */
  int       Cmax_spawn_lane;    /* maximum valid lane from road buffer, unpacked from BCspawn_lanes (was C) */
  const u8 *HLtab;              /* pointer into hazard_pos_speed for the perp's target lane column (was HL) */
  int       Ahorzpos;           /* perp's horizontal position, slid toward target column each frame (was A) */
  int       changing_lane;      /* 1 while perp is mid-lane-change, 0 once it reaches target (was C) */
  int       Adelay;             /* outer approach-delay countdown, 10..1 before distance boost fires (was A) */
  int       DEspeedmult;        /* speed increment per missing-distance unit: 30 (was DE) */
  int       HLspeed;            /* perp's approach speed accumulator, base 230 (was HL) */
  int       Acounter;           /* inner approach-timer countdown; resets from perp_approach_base+rng (was A) */
  int       Adistance;          /* perp's buffer distance, compared to 13 for close-approach boost (was A) */
  u8        Adist_minus_6;      /* distance-6 scratch value; result is discarded (dead code) (was A) */
  int       Adash_threshold;    /* crash speed threshold: 200 normally, 230 during turbo boost (was A') */
  int       Dbonus_hi;          /* high bonus digit: 0 base, +4 for a second smash (was D) */
  int       smash_twice;        /* 1 when hit hard enough to call smash() twice (was second PUSH HL) */
  int       Ebonus_mid;         /* middle bonus digit, rotated into position when retry_count is set (was E) */
  u8        Abonus_rotate;      /* bonus digit rotated left 4 bits for the retry_count case (was A) */
  carry = 0;

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
  Adistancediff = (u8) (IYhazard->distance - Cperp_distance);
  if (IYhazard->distance < Cperp_distance) {
    // Otherwise hazard-car is behind perp...
    carry = Adistancediff > (u8) (0xFF - 2); // carry out of ADD A,2
    Adistancediff += 2;                      // move it two lanes away?
  } else {
    carry = Adistancediff < 3; // borrow out of SUB A,3
    Adistancediff -= 3;
  }

  if (!carry) // out of range: hazard too far from perp's distance to matter
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
  Achanging_lane_flag = state->pb_changing_lane;
  if (Achanging_lane_flag)
    goto pb_check_lane;

  // Otherwise not changing lane?
  Adist_lane_gate = IXperp->distance;
  if (Adist_lane_gate >= 7)
    goto pb_check_lane;
  carry = 1; // Conv: CP $07 ($A697) leaves carry set here (distance < 7); it
             // survives to the SBC HL,DE at $A6B4 on the fall-through path.

  // Delay between perp lane changes: pb_lane_change_timer counts down; on
  // zero it resets to perp_lane_change_base + (rng & 31).
  // In-place decrementing counter.
  Alane_timer = state->pb_lane_change_timer - 1;
  if (Alane_timer)
    goto pb_update_counter;

  // When it hits zero we pick a random number...
  Alane_timer = state->stage->perp_lane_change_base + (rng(state) & 31);
  carry = 0; // Conv: carry from ADD A,C ($A6AA); never set for any stage's
             // perp_lane_change_base, so this path subtracts 164 exactly.

pb_update_counter:
  state->pb_lane_change_timer = Alane_timer;

  // This smells like it's detecting position and turning that into lanes.
  // The values are like those used by get_spawn_lanes.

  HL = state->scenedata.road_pos - 164 - carry; // SBC HL,DE ($A6B4) takes carry-in
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
  Acurrlane_bound = IXperp->current_lane;
  if (Acurrlane_bound == Cmax_lane)
    goto pb_random_move_left_or_right;
  if (Acurrlane_bound != Bmin_lane)
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
  if (Ahorzpos < 0) // carried: original position was < 10 ($A725 JR C)
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
  if (Adelay == 0) // $A766 JR Z: skip boost only on the frame delay reaches 0
    goto pb_a776;

  Adistance = IXperp->distance;
  if (Adistance >= 13)
    goto pb_a776;

  // Distance to perp is 12 or less.
  // .
  // HL += (13 - Adistance) * DE    HL is 230, DE is 30
  // .
  // This seems to be using the distance to the perp as a scale by which to adjust
  // its horizontal position.

  HLspeed += (13 - Adistance) * DEspeedmult;

pb_a776:
  Adist_minus_6 = IXperp->distance - 6;
  if ((s8) Adist_minus_6 < 0) {
    // Distance to perp is 5 or less
    Adist_minus_6 = (Adist_minus_6 + 5) * 8; // Bug? we do nothing with #REGa...
    HLspeed += DEspeedmult;
  }
  IXperp->speed = HLspeed;
  return;

  // If I meddle with this value the perp seems to race off too fast to catch.
pb_set_delay:
  IXperp->hit_timer = -4; // $FC
  carry = Ahit_timer < 3; // carry from CP $03, saved by PUSH AF before the SUB
  // PUSH AF // Ahit_timer
  if (Ahit_timer >= 3)
    Ahit_timer -= 3;

  Adash_threshold = (state->boost == 0) ? 200 : 230;
  scenery_hit(state, Ahit_timer, Adash_threshold);

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
    Abonus_rotate = Dbonus_hi;
    Dbonus_hi = Ebonus_mid;
    Abonus_rotate = (Abonus_rotate << 4) | (Abonus_rotate >> 4); /* RLC x4: swap nibbles */
    Ebonus_mid = Abonus_rotate;
  }
  add_bonus(state, 0, Ebonus_mid, Dbonus_hi);
  state->pb_delay = 5; // set delay counter to 5 turns
  start_chatter(state, 5, &chatterblk_raymond_smash[0]);
  start_sfx(state, EFFECT_CAR_HIT, 1); /* priority 1 */ /* tail call */

  smash(state); // Conv: Direct call rather than stack push
  if (smash_twice)
    smash(state);
}

/**
 * $A7F3: spawn_cars
 *
 * Spawns a new traffic car into an empty hazard slot each time the inline
 * self-modifying counter sc_spawn_counter reaches zero.
 *
 * Skips spawning if perp_caught_phase or dont_spawn_cars are set, or if
 * allow_spawning is zero. On each call sc_spawn_counter is decremented by
 * allow_spawning (1 or 2); when it fires, a new delay is drawn from
 * car_spawn_delay plus a random 0–15 jitter (with an extra +25 when the perp
 * has been sighted).
 *
 * It then walks the five non-perp hazard slots looking for an unused one. If
 * three or more vehicles are already active it aborts. Otherwise it copies
 * hazard_template into the free slot, picks a random lane clamped to the road
 * bounds, sets the initial horizontal position and speed from hazard_pos_speed,
 * and assigns a random car bitmap (avoiding the perp lookalike when sighted).
 *
 * \param[in] state Pointer to game state.
 */
static void spawn_cars(chqstate_t *state)
{
  int       allow_spawning;     /* allow_spawning flag: 0=disabled, 1/2=normal/fast (was A) */
  int       random_extra_delay; /* random 0–15 jitter added to spawn delay (was C) */
  int       spawn_delay;        /* total spawn delay: base + sighted boost + jitter (was A) */
  int       iterations;         /* hazard slot loop counter: 5 iterations (was B) */
  int       cars_seen;          /* bitmask: one bit set per active vehicle seen in the loop (was C) */
  hazard_t *hazard;             /* pointer to the hazard slot under examination (was IX) */
  int       spawn_lanes;        /* packed return from get_spawn_lanes: high byte=min, low=max (was BC) */
  u8        min_lane;           /* minimum spawn lane, unpacked from spawn_lanes (was B) */
  int       max_lane;           /* maximum spawn lane, unpacked from spawn_lanes (was C) */
  int       new_lane;           /* randomly chosen spawn lane, clamped to min..max (was A) */
  const u8 *phazard_pos_speed;  /* pointer into hazard_pos_speed for the chosen lane (was HL) */
  int       bitmap_index;       /* random even index 0–6 selecting the car bitmap pair (was C) */

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
  if (state->sc_spawn_counter > allow_spawning)
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
  phazard_pos_speed = &hazard_pos_speed[new_lane - 1];
  hazard->horz_pos_on_road = phazard_pos_speed[0];
  hazard->speed            = phazard_pos_speed[state->sighted_flag ? 8 : 4];

  // Now pick a random car bitmap to show.
  bitmap_index = rng(state) & 6;
  // If we've sighted the perp then don't spawn any generic cars (offset 6)
  // since they look just like the perp's. Instead use offset 4.
  if (state->sighted_flag && bitmap_index == 6)
    bitmap_index -= 2; // 6 -> 4

  hazard->hittable.bitmaps = state->stage->bitmaps_vehicles[bitmap_index / 2];
}

/**
 * $A89C: get_spawn_lanes
 *
 * Reads the lanes byte from the road buffer at the given offset and returns the
 * min/max lane pair valid for car spawning at that position.
 *
 * The return value is a packed u16: high byte = min lane, low byte = max lane.
 * Possible returns: $0104 — four-lane road (or dirt track / fork): lanes 1–4
 * $0103 — three-lane road or tunnel: lanes 1–3 $0204 — three-lane road,
 * right-biased: lanes 2–4 $0102 — two-lane road, left-biased: lanes 1–2 $0304 —
 * two-lane road, right-biased: lanes 3–4
 *
 * \param[in] state Pointer to game state.
 * \param[in] extra Road buffer offset added to the base index when reading
 * the lane byte. (was C)
 * \return Packed min/max lane pair (high byte = min, low byte = max).
 */
static u16 get_spawn_lanes(chqstate_t *state, int extra)
{
  u8 *roadbuf;     /* pointer into road buffer lanes data at the queried offset (was HL) */
  u8  lanes;       /* raw lanes byte read from road buffer (was A) */
  int lanes_copy;  /* preserved copy of lanes before masking to $C1 (was E) */
  int carry;       /* carry from SLA of masked lanes byte (carry) */

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
 * $A8CD: hazard_handler
 *
 * Updates one traffic hazard slot each frame: clamps its lane to the current
 * road bounds, slides its horizontal position toward the target and handles a
 * collision with the hero car.
 *
 * If perp_caught_phase or dont_spawn_cars are set the hazard is pushed
 * off-screen by setting its speed word to $01FF.
 *
 * Lane clamping: get_spawn_lanes is called for the hazard's current road
 * position. If the hazard's assigned lane (hazard_lane_OR_perp_dist_hi) falls
 * outside the returned min/max range, current_lane is clamped. When
 * current_lane differs from the assigned lane the function slides horz_pos ±5
 * per frame toward the target column in hazard_pos_speed; on arrival the
 * assigned lane is updated to match.
 *
 * Hit detection: if hit_timer is non-zero (set by the collision engine), the
 * hazard is marked unused, the overtake bonus is cleared and scenery_hit is
 * called to apply the crash penalty.
 *
 * \param[in]     state Pointer to game state.
 * \param[in,out] IXhazard Hazard slot to update. (was IX)
 */
void hazard_handler(chqstate_t *state, hazard_t *IXhazard)
{
  int       spawn_lanes;        /* packed return from get_spawn_lanes: high byte=min, low byte=max (was BC) */
  u8        min_lane;           /* minimum valid lane for this hazard's road position (was B) */
  int       max_lane;           /* maximum valid lane for this hazard's road position (was C) */
  int       lane;               /* hazard's assigned lane from hazard_lane_OR_perp_dist_hi (was A) */
  int       current_lane;       /* hazard's current_lane after clamping to road bounds (was A) */
  int       carry;              /* current_lane < lane, selects left/right slide direction (carry) */
  const u8 *phazard_pos_speed;  /* pointer into hazard_pos_speed for the target lane column (was HL) */
  int       horz_pos;           /* hazard's horizontal position, slid ±5 toward target each frame (was A) */
  int       hit_timer;          /* hit_timer from hazard slot; non-zero triggers a crash (was A) */

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
    // $A8FB CP (IX+$11) borrow, carried through the RL/RR B round trip
    // ($A900-$A90B); min_lane itself is never read again, so the round
    // trip collapses to the comparison it was shuttling.
    carry = current_lane < lane;
    phazard_pos_speed = &hazard_pos_speed[current_lane - 1];
    horz_pos = IXhazard->horz_pos_on_road;
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
  start_sfx(state, EFFECT_CAR_HIT, 2); /* priority 2 */ /* tail call */
}

/**
 * $A955: choose_dirt_and_stones
 *
 * Initialises one stone or dirt particle entry per frame on dirt-track
 * sections. Returns immediately unless on_dirt_track and allow_spawning are
 * both non-zero.
 *
 * A random byte from rng() determines the particle type: non-negative gives 1
 * (stone), negative gives 2 (dirt). A second random byte provides the
 * horizontal position. Both are written to the $ED28 particle table and the
 * three SM flags driving layout_dirt_and_stones and draw_dirt_and_stones are set
 * to 1.
 *
 * \param[in] state Pointer to game state.
 */
static void choose_dirt_and_stones(chqstate_t *state)
{
  u8 *table;  /* byte pointer into the $ED28 particle table: [0]=type, [1]=position (was DE) */

  if (state->on_dirt_track == 0 || state->allow_spawning == 0)
    return;

  // Conv: byte offset 0x28 into the $ED00 page = $ED28, the first particle
  // table entry: [0]=type, [1]=position.
  table = (u8 *) state->xpos_road_fork_right + 0x28;
  table[0] = ((s8) rng(state) >= 0) ? 1 : 2; // choose stone or dirt
  table[1] = rng(state); // choose random position
  state->ldas_enabled = 1;
  state->rm_scroll_dirt_particles = 1;
  state->ddas_enabled  = 1;
}

/**
 * $A97E: layout_dirt_and_stones
 *
 * Computes screen x-positions for up to 20 stone/dirt particles each frame.
 * Returns immediately if the ldas_enabled SM flag is zero.
 *
 * The function walks the $ED28 particle table in 4-byte entries: type byte,
 * position byte then a result word. For each entry whose type byte is non-zero
 * it reads the road left and right edge words at the object's road position
 * and multiplies the position byte by the road width using an 8-bit
 * shift-and-add loop. The x-position, left edge + (position * width) / 256, is
 * written back into the entry word for draw_dirt_and_stones to render.
 *
 * When the pass completes with a zero total (all entries were zero), the three
 * SM flags ldas_enabled, rm_scroll_dirt_particles and ddas_enabled are
 * cleared.
 *
 * \param[in] state Pointer to game state.
 */
static void layout_dirt_and_stones(chqstate_t *state)
{
  const u8 *obj_pos;           /* pointer walking object_positions backward from index 18 (was IY) */
  int       iterations;        /* outer loop counter: 20 table entries (was B) */
  int       total;             /* count of non-zero particle entries processed this call (was C) */
  u8       *particle;          /* byte pointer walking the $ED28 particle table (was HL) */
  u8        pos;               /* particle position byte: fraction of the road width (was A) */
  u8        Ldash;             /* ~(IY[1] * 2): byte index into the road edge tables, always odd (was L') */
  int       DEdash_left_edge;  /* left road edge word; stays live for the POP at $A9D2 (was DE') */
  int       HLdash_right_edge; /* right road edge word (was HL') */
  int       DEdash_width;      /* road width: right edge minus left edge (was DE' after SBC and EX) */
  u16       result;            /* multiply accumulator, zeroed for each particle (was HL') */
  int       carry;             /* carry linking RLA, ADD HL,HL and RRA in the multiply (carry) */
  int       iterations2;       /* multiply loop counter: 8 iterations (was B') */
  int       Cdash;             /* product high byte recovered by RRA (was C') */

  if (state->ldas_enabled == 0)
    return;

  obj_pos = &state->object_positions[18];
  iterations = 20;
  total = 0;
  particle = (u8 *) state->xpos_road_fork_right + 0x28; // $ED28
  do {
    if (*particle++)
      goto ldas_do_work;
    particle += 3;
ldas_loop1_continue:
    obj_pos--;
  } while (--iterations > 0);

  if (total == 0) {
    state->ldas_enabled = 0;
    state->rm_scroll_dirt_particles = 0;
    state->ddas_enabled = 0;
  }
  return;

ldas_do_work:
  pos = *particle++; /* position byte: the multiplicand */
  total++;

  // EXX - bank ($A9AA)

  // EX AF,AF' - bank A ($A9AB)

  // Conv: ADD A,A and CPL are 8-bit so Ldash is (255 - 2 * obj_pos[1]) & 0xFF,
  // always odd. Both edge reads are the little-endian word at bytes
  // Ldash-1/Ldash, i.e. word index Ldash / 2.
  Ldash = (u8) ~(obj_pos[1] * 2);
  DEdash_left_edge  = state->xpos_road_left[Ldash / 2];
  HLdash_right_edge = state->xpos_road_right[Ldash / 2];

  // PUSH DE ($A9BE) - DEdash_left_edge stays live for the POP at $A9D2
  DEdash_width = HLdash_right_edge - DEdash_left_edge;
  result = 0;

  // EX AF,AF' - unbank A ($A9C5); carry arrives clear from AND A at $A98F
  carry = 0;

  // Multiply the position byte by the road width.
  iterations2 = 8;
  do {
    RL(pos);
    if (carry)
      result += DEdash_width;
    carry = (result >> 15) & 1; /* ADD HL,HL carry out; feeds the next RLA and the final RRA */
    result <<= 1;
  } while (--iterations2 > 0);

  pos = result >> 8;
  RR(pos);           /* halve the doubled product; carry restores its top bit */
  Cdash = pos;

  // POP HL ($A9D2) - retrieve the left edge pushed at $A9BE
  result = (u16) (DEdash_left_edge + Cdash); /* B is zero after DJNZ */

  // PUSH HL / EXX / POP DE ($A9D4-$A9D6)

  particle[0] = result & 0xFF; /* little-endian x-position word */
  particle[1] = result >> 8;
  particle += 2;

  goto ldas_loop1_continue;
}

/**
 * $A9DE: draw_dirt_and_stones
 *
 * Renders one stone or dirt particle per frame using position data written by
 * layout_dirt_and_stones. Returns immediately if the ddas_enabled SM flag is
 * zero.
 *
 * On each call it reads the current entry from ddas_particle_ptr. If the type
 * byte is zero the slot is inactive and the pointer advances past it. Otherwise
 * it selects stones_lods or dust_lods based on the type byte, clamps
 * [Biterations] to 10, halves it as a LOD index and dispatches to
 * draw_object_left/right_width_entrypt with the computed x-position.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Biterations Distance-based counter; clamped to 10 for LOD
 * selection. (was B)
 * \param[in] IYheight Pointer into the height table. (was IY)
 */
static void draw_dirt_and_stones(chqstate_t *state, int Biterations,
                                 const u8 *IYheight)
{
  u8              *HLparticle;                /* byte pointer into the $ED28 table via ddas_particle_ptr (was HL) */
  u8               Atype;                     /* type byte: 1=stone, 2=dirt (was A) */
  const bitmap_t (*DEbitmaps)[SPRITE_FRAMES]; /* bitmap LOD array: stones_lods or dust_lods (was DE) */
  int              Cx;                        /* x-position low byte from the particle table (was C) */
  int              Ax;                        /* LOD index (was A) */
  const bitmap_t  *HLbitmap;                  /* selected bitmap frame for the current LOD level (was HL) */
  int              Ewidth;                    /* pixel width of selected bitmap: width_bytes * 8 (was E) */
  int              saved_A;                   /* banked x-position high byte; sign selects the draw path (was A') */
  int              Aiterations;               /* (was A) */

  if (state->ddas_enabled == 0)
    return;

  HLparticle = state->ddas_particle; // table ptr
  Atype = *HLparticle;  /* type byte */
  HLparticle += 2;  /* skip the type and position bytes */
  if (Atype)
    goto ddas_bitmaps;

  HLparticle += 2;  /* skip the unused x-position word */
  state->ddas_particle = HLparticle;
  return;

ddas_bitmaps:
  DEbitmaps = state->stage->bitmaps_stones;
  if (--Atype)
    DEbitmaps = state->stage->bitmaps_dust;

  Cx = *HLparticle++; /* x-position low byte */
  Ax = *HLparticle++; /* x-position high byte */
  // EX AF,AF' -- bank the x-position high byte ($A9FF)
  saved_A = (s8) Ax; /* Conv: JP M at $AA26 tests bit 7 so sign-extend */
  state->ddas_particle = HLparticle;
  state->doc_col_pos = 0;
  // H = 0;
  Aiterations = Biterations - 1;
  if (Aiterations > 10)
    Aiterations = 10;

  Aiterations >>= 1;
  // Conv: $AA13-$AA19: L = A * 7; ADD HL,DE — each LOD entry is 7 bytes, so
  // this selects the A-th bitmap_t within the single table. DEbitmaps[A]
  // would step A whole 6-entry tables (A*6 bitmap_t) and read out of bounds.
  HLbitmap = &(*DEbitmaps)[Aiterations];
  Ewidth = HLbitmap->width_bytes * 8;

  // EX AF,AF' -- unbank A' (table byte 3) into A ($AA21)
  Ax = Cx;
  Cx = 0;
  if (saved_A >= 0) {
    if (saved_A)
      return;

    // So it's zero
    if (Ax >= 128) {
      draw_object_right_width_entrypt(state, Ax, HLbitmap,
                                      IYheight); /* tail call */
    } else {
      Ax += Ewidth;
      draw_object_left_width_entrypt(state, Ax, HLbitmap,
                                     IYheight); /* tail call */
    }
  } else {
    if (Ax + Ewidth <= 255) /* fully off-screen when the add doesn't overflow */
      return;

    Ax += Ewidth; /* Conv: wraps as the Z80 ADD does */
    draw_object_left_width_entrypt(state, Ax, HLbitmap,
                                   IYheight);  /* tail call */
  }
}

/**
 * $AA38: draw_helicopter
 *
 * Renders the helicopter sprite at the current frame. Returns immediately
 * unless [Biterations] equals 3, the only distance at which the helicopter is
 * visible.
 *
 * The rotor animation position is derived by multiplying the top three bits of
 * fast_counter by the height delta object_positions[Biterations] −
 * object_positions[Biterations − 1], using an 8-bit shift-and-add loop. The
 * high byte of the result, halved, becomes dhs_heli_rotor_pos. The body
 * y-offset is computed from dhs_heli_vert_base minus
 * object_positions[Biterations − 1].
 *
 * Conv: the original reads these two bytes via IY+$4E/IY+$4F, where IY is the
 * same pointer draw_scene_objects walks down through height_table ($E300 +
 * Biterations). Because the Z80 address space is flat, those two offsets
 * actually land past height_table, past clamped_heights, past horizon_attr
 * and one unused byte, inside the separate 21-byte object_positions buffer
 * at $E34F (see layout_objects, $A579) — specifically at
 * object_positions[Biterations − 1] and object_positions[Biterations]. A
 * literal port that kept indexing off the height_table pointer would read 78
 * and 79 bytes past a 32-byte C array instead.
 *
 * Five body parts are drawn in a loop via draw_helicoper_part, followed by a
 * sixth rotor entry drawn separately using the self-modified rotor position.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bdistance Distance counter; must equal 3 to render. (was B)
 * \param[in] IYheight Pointer into the height table, passed through to
 *            draw_helicoper_part for object plotting. (was IY)
 */
static void draw_helicopter(chqstate_t *state, int Bdistance, u8 *IYheight)
{
  int                        carry;        /* carry from RLA in the rotor multiply loop (carry) */
  int                        diff;         /* object_positions height delta, multiply operand (was DE) */
  u16                        total;        /* shift-and-add multiply accumulator (was HL) */
  u8                         fast_counter; /* top three bits of fast_counter, shifted through multiply (was A) */
  int                        Biterations2; /* loop counter: 8 for multiply, 5 for body-part draw (was B) */
  u8                         Atotal;       /* high byte of multiply result, halved for rotor position (was A) */
  int                        frame;        /* animation frame index: anim_counter bit 0 selects bitmap set (was A) */
  const heli_part_ptr_t     *helitable;    /* pointer to current 6-entry table of part pointers (was HL) */
  const heli_bitmap_t       *helipart;     /* body part block: y_offset + inner bitmap (was DE) */
  const heli_bitmap_inner_t *helirotor;    /* rotor block: bare inner bitmap, no y_offset (was DE) */

  carry = 0;

  if (Bdistance != 3)
    return;

  if (state->stage->addrof_helicopter_stuff_1 == NULL)
    return;

  diff = state->object_positions[Bdistance] -
         state->object_positions[Bdistance - 1]; // Conv: see prologue
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
  state->dhs_heli_rotor_pos = Atotal;

  state->dhs_heli_y_offset =
    state->dhs_heli_vert_base - state->object_positions[Bdistance - 1];

  Biterations2 = 5; // iterations (draw first five)
  frame = state->anim_counter & 1; // heli frame

  helitable = (frame == 0) ? state->stage->addrof_helicopter_stuff_1 :
                             state->stage->addrof_helicopter_stuff_2;

  // Conv: the table is six 16-bit pointers to per-part blocks (HL advances 2
  // bytes per part; pitfall #29), not six consecutive heli_bitmap_t structs.
  // Parts 1-5 are heli_bitmap_t (y_offset + inner); part 6 (rotor, $AA86) is
  // a bare heli_bitmap_inner_t with no y_offset byte, so it is read via a
  // separate pointer type after the loop.
  do {
    helipart = (const heli_bitmap_t *)*helitable++;
    draw_helicoper_part(state, helipart->y_offset + state->dhs_heli_y_offset,
                        &helipart->inner, IYheight);
  } while (--Biterations2 > 0);

  helirotor = (const heli_bitmap_inner_t *)*helitable;
  // A = 0; // an apparently useless op
  draw_helicoper_part(state, state->dhs_heli_rotor_pos, helirotor, IYheight);
}

static void draw_helicoper_part(chqstate_t                *state,
                                int                        Acol_pos,
                                const heli_bitmap_inner_t *DEinnerbitmap,
                                const u8                  *IYheight)
{
  int             carry;
  int             BC_helipos; /* was BC */
  int             screen_pos; /* was HL/DE */
  const bitmap_t *HLbitmap;   /* was HL */
  int             Bwidth;     /* was B */
  int             Atop;       /* was A */
  u8              Abot;       /* was A; Conv: u8 so += wraps mod 256 like the Z80 ADD A,B */
  int             C;          /* was C */

  BC_helipos = state->dhl_helipos;
  state->doc_col_pos = -Acol_pos; // in draw_object_common

  screen_pos = DEinnerbitmap->horz_offset + BC_helipos;

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
      draw_object_right_width_entrypt(state, Abot, HLbitmap,
                                           IYheight); /* tail call */
    } else {
      Abot += Bwidth;
      draw_object_left_width_entrypt(state, Abot, HLbitmap,
                                          IYheight); /* tail call */
    }
  } else {
    carry = (Abot + Bwidth) > 255;
    Abot += Bwidth;
    if (!carry)
      return;

    draw_object_left_width_entrypt(state, Abot, HLbitmap,
                                        IYheight); /* tail call */
  }
}

/**
 * $AAC6: move_helicopter
 *
 * Advances the helicopter's position and animation each frame. Returns
 * immediately if helicopter_control is zero.
 *
 * The vertical position (mh_height) descends by 2 pixels per frame toward 97.
 * The animation frame cycles 0–3; when it wraps back to 0 the horizontal swing
 * direction is negated. The direction value accumulates into mh_offset which,
 * added to height, gives dhs_heli_vert_base for the draw function.
 *
 * The horizontal screen position dhl_helipos is updated by adding the road_pos
 * delta since the last frame. The result is then stepped 8 pixels per frame
 * toward mh_heli_centre_y (normally 112, the screen centre; drive_helicopter
 * redirects it to −56 to fly the helicopter off-screen during the exit
 * sequence), clamping when it would overshoot.
 *
 * \param[in] state Pointer to game state.
 */
static void move_helicopter(chqstate_t *state)
{
  int height;      /* helicopter height SM field, descending toward 97 each frame (was A, then C) */
  int direction;   /* horizontal swing direction: +1 or −1, negated each anim cycle (was A) */
  int offset;      /* accumulated swing offset, added to height for dhs_heli_vert_base (was A) */
  int helipos;     /* horizontal position: dhl_helipos adjusted by road_pos delta (was HL) */
  int centre;      /* screen centre column target for helipos: mh_heli_centre_y (was DE) */
  int newhelipos;  /* updated helipos after one 8-pixel step toward centre (was HL) */

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

  state->dhs_heli_vert_base = offset + height;

  helipos = state->scenedata.road_pos - state->mh_prevroadpos; // delta
  state->mh_prevroadpos = state->scenedata.road_pos; // update

  helipos += state->dhl_helipos;
  centre = state->mh_heli_centre_y; // Conv: SM constant, redirected by drive_helicopter
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
 * $AB33: drive_helicopter
 *
 * State machine controlling the helicopter event sequence. Returns immediately
 * if helicopter_control is zero.
 *
 * States on entry: 1: redirect mh_heli_centre_y to −56 (off-screen) so
 * move_helicopter flies the helicopter away, and transition to state 2. 2:
 * helicopter departing; waits while dhl_helipos is still non-negative
 * (on-screen), then disables the helicopter (state 0) once it has gone
 * negative. 3: helicopter turning left; initialise SM fields, play pilot
 * chatter and set centre_y to 112, transition to state 5. 4: helicopter
 * turning right; same as state 3 with right-turn chatter. 5+: not yet active;
 * return immediately.
 *
 * When the helicopter is enabled, dee_draw_helicopter is set to 1 and the call
 * site at $8FA4 is self-modified to CALL draw_helicopter. When disabled it is
 * patched back to NOPs.
 *
 * \param[in] state Pointer to game state.
 */
static void drive_helicopter(chqstate_t *state)
{
  int       heli_ctl;      /* helicopter_control value on entry; decremented to dispatch (was A) */
  int       helipos;       /* dhl_helipos; sign checked in state 2 (was A, high byte of HL) */
  int       draw_heli;     /* 1 to enable helicopter rendering, 0 to disable (was A) */
  int       new_heli_ctl;  /* new value to write back to helicopter_control (was A) */
  int       HL_centre_y;   /* starting centre_y for mh_heli_centre_y: −56 or 112 (was HL) */
  const u8 *chatterblk;    /* pointer to pilot chatter block for turn-left or turn-right (was HL) */

  heli_ctl = state->helicopter_control;
  if (heli_ctl == 0)
    return;

  if (--heli_ctl == 0) // 1
    goto hc_1;

  if (--heli_ctl) // 3+
    goto hc_pick_direction;

  // Otherwise helicopter_control is 2.

  helipos = state->dhl_helipos;
  // Conv: original tests the high byte of the 16-bit dhl_helipos word for
  // zero (RET Z), i.e. waits while dhl_helipos is still non-negative and
  // falls through to disable once it has gone negative (flown off-screen).
  if (helipos >= 0)
    return;

  draw_heli = 0; // false
  new_heli_ctl = 0; // New value for helicopter_control is 0
  goto hc_exit;

hc_1:
  HL_centre_y = -56;
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
  HL_centre_y = 112;
  // Set starting vertical position of the helicopter.
  state->mh_height = 133;
  new_heli_ctl = 5; // New value for helicopter_control is 5
hc_set_draw:
  state->mh_heli_centre_y = HL_centre_y;
  draw_heli = 1; // true
hc_exit:
  state->dee_draw_helicopter = draw_heli;
  state->helicopter_control = new_heli_ctl;
}

/**
 * $AB9A: spawn_hazards
 *
 * Spawns hittable obstacles (barriers or tumbleweeds) into hazard slots based
 * on the road-buffer hazard byte at the current spawning distance. Returns
 * immediately if allow_spawning is zero.
 *
 * The spawning distance is 20 − allow_spawning (18 or 19). If the road-buffer
 * hazard byte at that offset is zero, nothing is spawned.
 *
 * A hazard byte of 4 or more selects the heavier hittable variant (index 3) and
 * subtracts 3 before dispatch. The remaining value controls count and
 * placement: 1: one obstacle at x=50. 2: one obstacle at x=220. 3: two or three
 * obstacles; barriers when inhibit_collision_detection is set, tumbleweeds
 * otherwise.
 *
 * \param[in] state Pointer to game state.
 */
static void spawn_hazards(chqstate_t *state)
{
  int allow_spawning;    /* allow_spawning flag: 0=disabled, 1/2=normal/fast (was A) */
  int Cdistance;         /* spawn distance: 20 − allow_spawning; used as road-buffer offset (was C) */
  u8 *roadbuf;           /* pointer into road buffer at the hazards data offset (was HL) */
  int hazard;            /* hazard type byte from road buffer, dispatches spawn variant (was A) */
  int DEhittable_offset; /* hittable object table offset: 0=light variant, 3=heavy variant (was DE) */
  int horz_pos;          /* x position for the final (or only) spawned hazard (was B) */

  allow_spawning = state->allow_spawning;
  if (allow_spawning == 0)
    return;

  // Calculate a spawning distance.
  Cdistance = 20 - allow_spawning;

  // Point #REGhl at hazards data.
  roadbuf = ROADBUF_FWD2PTR(ROADBUF_HAZARDS_OFFSET + Cdistance);

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

  WRAP_ASSIGN(roadbuf, 2, state->roadbuf_start);
  *roadbuf = DEhittable_offset >> 8; // D is zero

  WRAP_ASSIGN(roadbuf, 2, state->roadbuf_start);
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
 * $AC3C: hazard_hit
 *
 * Hit handler for static road hazards (barriers and tumbleweeds). Registered as
 * the hit_handler function pointer in each hazard slot spawned by
 * spawn_hazards.
 *
 * hazard_flags drives a three-state FSM: 0: first-hit phase. If hit_timer is
 * zero the hit has not yet registered; return immediately. Otherwise look up
 * wobble parameters in wobble_params_by_speed based on hero speed, store them in
 * hazard_lane_OR_perp_dist_hi and current_lane, scale the approach speed, play
 * EFFECT_HAZARD_HIT and set hazard_flags to 2. 1: wobble complete; return
 * immediately (--flags reaches zero). 2+: wobble animation. Each call advances
 * the hit_wobble index through wobble_amplitudes, decays speed by 1/32, toggles the
 * inverted bit and decrements the current_lane countdown. When it reaches zero,
 * speed and inverted are zeroed and hazard_flags reverts to 1.
 *
 * \param[in]     state Pointer to game state.
 * \param[in,out] IXhazard Hazard slot that was hit. (was IX)
 */
static void hazard_hit(chqstate_t *state, hazard_t *IXhazard)
{
  // $ACDB
  //
  // Per-frame wobble amplitudes: a series of ever-smaller humps, each
  // rising to a peak, falling back and ending in a zero. A hit enters
  // part-way through (see wobble_params_by_speed) and reads one value
  // per frame, so the hazard's shudder decays away to nothing.
  static const u8 wobble_amplitudes[] = {
    0x19, 0x28, 0x32, 0x37, 0x39, 0x37, 0x32, 0x28,
    0x19, 0x00, 0x0F, 0x19, 0x1F, 0x22, 0x24, 0x22,
    0x1F, 0x19, 0x0F, 0x00, 0x0A, 0x10, 0x13, 0x15,
    0x13, 0x10, 0x0A, 0x00, 0x06, 0x09, 0x0B, 0x09,
    0x06, 0x00, 0x02, 0x00, 0x02, 0x00, 0x01, 0x00
  };

  // $AD03
  //
  // Pairs of (start index into wobble_amplitudes, wobble frame countdown)
  // for five speed brackets, slowest first. Faster hits start earlier in
  // the amplitude table so the wobble is bigger and lasts longer.
  static const u8 wobble_params_by_speed[5 * 2] = {
    0x06, 0x22,
    0x0C, 0x1C,
    0x0E, 0x14,
    0x12, 0x0A,
    0x14, 0x00
  };

  int       A_hazard_flags; /* hazard_flags on entry; drives three-state FSM (was A) */
  int       hit_timer;  /* IX[7]: zero=no hit yet, negative=max-speed hit, positive=normal hit (was A) */
  int       speed;      /* hero car speed at time of impact; 280 when hit_timer is negative (was DE, BC) */
  int       index;      /* speed-derived index into wobble_params_by_speed, two bytes per entry (Conv: added) */
  const u8 *ptable;     /* pointer into wobble_params_by_speed for the current speed bracket (was HL) */

  A_hazard_flags = IXhazard->hazard_flags;
  if (A_hazard_flags == 0) {
    if ((hit_timer = IXhazard->hit_timer) == 0)
      return;

    // If we arrive here then a hit has occurred.
    speed = (hit_timer >= 0) ? state->speed : 280;

    // Conv: $AC56-$AC60: RL E/RLA/AND 3 yields (speed >> 7) & 3; RR D puts
    // bit 8 of speed into carry which ADC A,D adds back.
    index = ((speed >> 7) & 3) + ((speed >> 8) & 1);
    ptable = &wobble_params_by_speed[index * 2];

    IXhazard->hazard_lane_OR_perp_dist_hi = ptable[0];
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
  } else if (A_hazard_flags == 1) {
    /* was DEC A */
    return;
  }

  // Conv: wobble_params_by_speed entry 4 starts the index at 20 with a countdown of 0,
  // which wraps to 255 and marches the index past the 40-byte table. The Z80
  // harmlessly reads whatever follows $ACDB; here we clamp to 0 (the table's
  // settled value) to avoid the out-of-bounds read.
  if (IXhazard->hazard_lane_OR_perp_dist_hi < sizeof(wobble_amplitudes))
    IXhazard->hit_wobble = wobble_amplitudes[IXhazard->hazard_lane_OR_perp_dist_hi];
  else
    IXhazard->hit_wobble = 0;
  IXhazard->hazard_lane_OR_perp_dist_hi++;
  IXhazard->speed -= IXhazard->speed / 32;
  IXhazard->inverted ^= 1; // toggle inverted
  if (--IXhazard->current_lane == 0) {
    IXhazard->speed        = 0;
    IXhazard->inverted     = 1;
    IXhazard->hazard_flags = 1;
  }
}

/**
 * $AD0D: Test every active hazard for a collision with the player car.
 *
 * Walks all 6 hazard slots. Skips any slot whose `used` field is HAZARD_UNUSED
 * (0x00). For active slots the function also skips the deferred-hit state: when
 * `hazard_flags == 0xFF` and the perpendicular distance high byte
 * (`hazard_lane_OR_perp_dist_hi`) is non-zero the hazard is too far laterally
 * for a hit this frame. Otherwise, if the hazard is within distance 20 and
 * `check_collision` returns a positive result, the slot's `hit_handler` is
 * dispatched (provided `hazard_flags` has not already been set to 0xFF by a
 * prior hit).
 *
 * Conv: Z80 uses `RLC (IX+$00); JR NC` to test bit 7 of `used`; C uses `!=
 * HAZARD_UNUSED`. Equivalent because `used` is always 0x00 or 0xFF. Conv: Z80
 * pushes $AD4B (the loop-advance address) before the per-slot checks so that
 * early `RET`s jump directly to the DJNZ tail; C uses `goto chc_continue` for
 * the same effect.
 *
 * \param[in] state Pointer to game state.
 */
static void check_hazard_collisions(chqstate_t *state)
{
  hazard_t *IX_hazard;   /* pointer to current hazard slot under test (was IX) */
  int       B_iterations; /* countdown over 6 hazard slots (was B) */

  if (state->inhibit_collision_detection)
    return;

  IX_hazard    = &state->hazards[0];
  B_iterations = 6;
  do {
    if (IX_hazard->used != HAZARD_UNUSED) {
      /* $AD26: when hazard_flags == 0xFF the slot is in deferred-hit mode;
       * skip if the perpendicular distance high byte is still non-zero */
      if (IX_hazard->hazard_flags == 0xFF &&
          IX_hazard->hazard_lane_OR_perp_dist_hi)
        goto chc_continue;

      /* $AD31: skip if distance >= 20 or no collision; dispatch hit handler */
      if (IX_hazard->distance < 20 &&
          check_collision(state, 0, 0, IX_hazard) > 0 &&
          IX_hazard->hazard_flags != 0xFF)
        IX_hazard->hit_handler(state, IX_hazard);
    }

chc_continue:
    IX_hazard++;
  } while (--B_iterations > 0);
}

/**
 * $AD51: Test whether the player car has hit a given hazard.
 *
 * Returns 1 (and arms the [hazard]'s hit_timer) if all of the following hold: -
 * hit_timer is zero (no recent hit cooldown) - horz_clip is zero ([hazard]
 * fully on screen) - distance is below the threshold (2 or 3 depending on
 * hazard_flags) - fast_counter sign passes the speed gate for the current
 * distance - horizontal position overlaps the player car's bounding zone
 * ($68..$8F)
 *
 * The hit_timer value written (1, 2, or 4) encodes the hit severity: 1 for a
 * glancing blow at close range, 4 for a dead-centre strike.
 *
 * Conv: Z80 uses D as both the "no-hit" return value (caller sets D=0 before
 * the CALL) and the collision flag (set to 1 at $AD9D). C separates these into
 * the [default_retval] parameter and the u8 return value. Conv: Z80 leaves [HL]
 * = (horz_clip<<8)|horz_pos at the point of return, but the one call site that
 * mattered ($AE7A) discards it -- horz_pos/horz_clip are written by the caller
 * before the CALL, not read back after -- so the C port has no output parameter.
 *
 * \param[in]     state Pointer to game state.
 * \param[in]     default_retval value returned when no collision (was D).
 * \param[in]     HL Initial HL; unused within the function body, kept for
 * skool traceability of the original register (was [HL]).
 * \param[in,out] hazard Hazard being tested; hit_timer written on hit (was IX).
 * \return 1 on collision, [default_retval] otherwise (was D).
 */
static u8 check_collision(chqstate_t *state,
                          int          default_retval,
                          int          HL,
                          hazard_t    *hazard)
{
  int L_horz_pos;  /* hazard horizontal position byte (was L) */
  int H_horz_clip; /* hazard clip flag byte; non-zero means off-screen (was H) */
  int A_flags;     /* hazard_flags + 1; non-zero selects 3-distance threshold (was A) */
  int A_distance;  /* hazard distance; decremented to test range (was A) */
  int C_max_dist;  /* distance threshold: 3 if A_flags != 0, else 2 (was C) */
  int E_hit_timer; /* hit severity: 1 glancing, 2 normal, 4 dead centre (was E) */
  int A_fast_cnt;  /* fast_counter; sign-tested to gate collision at distance 1 (was A) */
  int A_horz_pos;  /* L_horz_pos masked to $F8; used for bounding-box overlap (was A) */

  NOT_USED(HL);

  if (hazard->hit_timer) /* still in hit cooldown */
    return default_retval;

  L_horz_pos  = hazard->horz_pos;
  H_horz_clip = hazard->horz_clip;

  if (hazard->horz_clip) /* hazard clipped off-screen; no collision */
    return default_retval;

  A_flags    = (u8) (hazard->hazard_flags + 1); /* Conv: truncate to 8 bits so 0xFF wraps to 0, matching Z80 INC A */
  A_distance = hazard->distance;
  C_max_dist = (A_flags != 0) ? 3 : 2;
  if (A_distance >= C_max_dist)
    return default_retval;

  E_hit_timer = 4;
  A_distance--;
  A_fast_cnt = state->fast_counter;
  if (A_distance == 0) {
    /* at distance 1, only hit if fast_counter is negative */
    if ((s8) A_fast_cnt < 0)
      E_hit_timer = 1;
  } else {
    /* at distance > 1, skip collision if fast_counter >= 0 */
    if ((s8) A_fast_cnt >= 0)
      return default_retval;
  }

  // Check horizontal position
  A_horz_pos = L_horz_pos & 0xF8;
  if (A_horz_pos >= 144)
    return default_retval;

  A_horz_pos += hazard->hittable.width;
  if (A_horz_pos <= 112)
    return default_retval;

  A_horz_pos -= hazard->hittable.width;
  if (A_horz_pos >= 104) {
    E_hit_timer--;
    if (A_horz_pos >= 120)
      E_hit_timer += 2;
  }

  hazard->hit_timer = E_hit_timer;
  return 1;
}

/**
 * $ADA0: advance_hazards
 *
 * Resets n_hazards to zero then walks all six hazard slots, calling
 * advance_hazard for each active slot.
 *
 * advance_hazard advances the hazard's distance, performs perspective
 * projection, inserts it into the depth-sorted draw list and fires its hit
 * handler. The height table pointer passed to it indexes the $E300 buffer.
 *
 * \param[in] state Pointer to game state.
 */
static void advance_hazards(chqstate_t *state)
{
  const u8 *height_table; /* pointer to state->height_table[0] at $E300 (was IY) */
  hazard_t *hazard;       /* pointer to current hazard slot under examination (was IX) */
  int       iterations;   /* loop counter: 6 hazard slots (was B) */

  state->n_hazards = 0;
  height_table = &state->height_table[0];
  hazard = &state->hazards[0];
  iterations = 6;
  do {
    if (hazard->used == HAZARD_USED)
      advance_hazard(state, hazard, height_table); // called with regs banked
    hazard++;
  } while (--iterations > 0);
}

/**
 * $ADBE: Advance one hazard and insert it into the depth-sorted draw list.
 *
 * Called for each in-use hazard slot. The function has two phases.
 *
 * Phase 1 — distance advance ($ADBE–$ADFF): Subtracts the hazard's speed
 * (IX[13]/IX[4]) from dist_frac; any borrow increments C_dist. Adds IX[1] to
 * C_dist to get the new total distance. For ordinary hazards (A_flags != 0):
 * retire if C_dist >= 23, else store and fall through to phase 2 if < 20. For
 * the perp-car path (A_flags == 0): the lane counter IX[17] is advanced with
 * wrap at 4 before the distance update; if the lane counter was already zero
 * the function drops into phase 2 immediately.
 *
 * Phase 2 — perspective + draw list ($AE00–$AECE): Two 8-bit
 * shift-and-accumulate multiplies map dist_frac × height-delta to a perspective
 * column, then horz_pos_on_road × road-width to a screen x-position. The result
 * is passed to check_collision ($AE7A), which also updates IX[2]/IX[3]
 * (horz_pos/horz_clip). The hazard is then inserted into the depth-sorted draw
 * list at state->xpos_road_centre_left. Finally the hit handler is called via
 * the hazard's function pointer.
 *
 * Conv: Z80 uses PUSH IX / POP DE to get IX as a pointer into DE; C stores the
 * hazard slot index ([IXhazard] - hazards[0]) instead. Conv: Z80 calls the hit
 * handler via JP (HL) ($AEC8–$AECE); C calls the function pointer directly.
 * Conv: Z80 LDDR at $AEBA shifts the draw list down by bytes; C uses a
 * pointer-decrement loop over s16 words.
 *
 * \param[in]     state Pointer to game state.
 * \param[in,out] IXhazard Hazard slot to advance and render (was IX).
 * \param[in]     IYbase Base of the height table at $E300 (was IY).
 */
static void advance_hazard(chqstate_t *state,
                           hazard_t   *IXhazard,
                           const u8   *IYbase)
{
  int       C_dist;       /* distance accumulator; speed borrow + IX[1] (was C) */
  u8        A_old_frac;   /* dist_frac before the speed subtraction, for borrow detection (was A) */
  u8        A_speed_lo;   /* speed low byte subtracted from dist_frac (was A) */
  int       A_flags;      /* IX[15]+1; zero selects perp path, non-zero car path (was A) */
  int       A_lane;       /* IX[17] lane counter; advanced on perp path only (was A) */
  int       zero;         /* Z-flag: A_lane was zero before distance overwrite */
  int       A_dist;       /* new distance value to be written to IX[1] (was A) */
  int       A_fc_inv;     /* ~(fast_counter & $E0); speed gate for overtake check (was A) */
  int       B_flags;      /* IX[15]+1; carry after RL signals overtake bonus (was B) */
  int       carry;        /* Z80 carry/borrow flag */
  const u8 *IY;           /* height table row pointer; IYl = dist offset (was IY) */
  int       C_ht_hi;      /* IY[1]; upper height bound, held across first multiply (was C) */
  int       DE;           /* height delta → xpos_road_left → road width (was DE) */
  u16       HLresult;     /* 8-bit multiply accumulator (was HL) */
  u8        A_mult;       /* multiplicand for each shift-accumulate loop (was A) */
  int       Biterations;  /* 8-iteration multiply counter; reused for draw-list scan (was B) */
  int       A_persp;      /* perspective column: (HLresult>>8)>>1 from first multiply (was A) */
  int       A_xidx;       /* xpos table index derived from C_ht_hi and A_persp (was A) */
  s16      *HLtable;      /* xpos table and draw-list pointer (was HL) */
  s16       HL;           /* xpos_road_right value; later hazard screen x-position (was HL) */
  int       A_xresult;    /* xpos offset: (HLresult>>8)>>1 from second multiply (was A) */
  int       hl_carry;     /* ADD HL,HL carry-out, captured for the post-loop RRA (Conv: added) */
  int       Ddistance;    /* hazard distance for draw-list depth comparison (was D) */
  int       Edist_frac;   /* hazard dist_frac for draw-list depth comparison (was E) */
  u8       *HL_n_hazards; /* pointer to state->n_hazards (was HL) */
  int       A_n_hazards;  /* n_hazards count before increment (was A) */
  u16       HLword0;      /* draw-list record word0: distance | (frac << 8) (Conv: added) */
  int       BCwords;      /* draw-list entries × 2; word count for shift-down (Conv: added) */
  s16      *DEtable;      /* destination pointer during draw-list upward shift (was DE) */
  s16      *HLstart;      /* freed record slot for the new entry (Conv: added) */

  carry = 0;

  /* $ADBE: role of this top byte of speed is uncertain */
  C_dist = IXhazard->speed >> 8;
  A_old_frac         = IXhazard->dist_frac;
  A_speed_lo         = IXhazard->speed & 0xFF;
  IXhazard->dist_frac = A_old_frac - A_speed_lo;
  if (A_old_frac < A_speed_lo)
    C_dist++;
  C_dist += IXhazard->distance;

  A_flags = (u8) (IXhazard->hazard_flags + 1);
  /* Conv: truncate to 8 bits so 0xFF wraps to 0, matching Z80 INC A */
  if (A_flags == 0) {
    /* Perp path: advance lane counter IX[17] */
    A_lane = IXhazard->hazard_lane_OR_perp_dist_hi;
    if (C_dist > 255) {
      A_lane++;
      if (A_lane >= 4) {
        A_lane--;
        C_dist = 0xFF;                  /* $ADE3: cap and mark distance max */
      }
      IXhazard->hazard_lane_OR_perp_dist_hi = A_lane;
    }
    zero   = (A_lane == 0);
    A_dist = C_dist;
    if (zero)
      goto dhs_adfa;
    IXhazard->distance = A_dist;
    return;
  }

  A_dist = C_dist;
  if (A_dist >= 23) {
    IXhazard->used = HAZARD_UNUSED; /* $ADF5: hazard scrolled past player */
    return;
  }

dhs_adfa:
  IXhazard->distance = A_dist;
  if (A_dist >= 20)
    return;

  if (--A_dist == 0) {
    /* At closest visible distance: check if hazard has been overtaken */
    A_fc_inv = ~(state->fast_counter & 0xE0);
    if (A_fc_inv < IXhazard->dist_frac) {
      B_flags = (u8) (IXhazard->hazard_flags + 1);
      /* Conv: truncate to 8 bits so 0xFF wraps to 0, matching Z80 INC A */
      if (B_flags) {
        /* Car hazard overtaken: retire slot; carry from RL signals bonus */
        IXhazard->used = HAZARD_UNUSED;
        RL(B_flags);
        if (carry)
          state->overtake_bonus_counter++;
        return;
      }
      IXhazard->dist_frac = A_fc_inv;         /* $AE20: perp path stores speed gate */
    }
    A_dist = 0;
  }

  /* select height table row */
  IY = &IYbase[A_dist + 0x4E];

  /* height range delta */
  C_ht_hi = IY[1];
  /* Conv: $AE2F-$AE33 DE=A, HL=0 setup inlined; $AE2C SUB becomes direct expr */
  DE = C_ht_hi - IY[0]; /* height delta; multiplier for first loop */

  /* $AE34-$AE3E: 8-bit shift-accumulate: HLresult = dist_frac * height_delta */
  HLresult    = 0;
  A_mult      = IXhazard->dist_frac;
  Biterations = 8;
  do {
    RL(A_mult);
    if (carry) HLresult += DE;
    hl_carry = (HLresult >> 15) & 1; /* ADD HL,HL carry-out, before it is lost to truncation */
    HLresult <<= 1;
  } while (--Biterations > 0);

  /* perspective column */
  A_persp = HLresult >> 8;
  carry = hl_carry; /* Conv: RRA consumes the final loop's ADD HL,HL carry, not RLA's */
  RR(A_persp);
  IXhazard->persp_col = A_persp;

  /* xpos table index */
  A_xidx = ~((C_ht_hi - A_persp) << 1) & 0xFF; /* 8-bit NEG/ADD/CPL chain wraps mod 256 */

  /* $AE4A-$AE54: D=(E8|A), E=(E8|A-1), H=(EC|A+1), L=(EC|A)
   * Conv: Z80 uses hardcoded pages $E8/$EC for xpos_road_left/right;
   *       C uses named arrays. Index A/2 because entries are 16-bit words. */
  HLtable = &state->xpos_road_left[A_xidx / 2];
  DE      = *HLtable;
  HLtable = &state->xpos_road_right[A_xidx / 2];
  HL      = *HLtable;

  state->dhs_road_left_xpos = DE;

  /* $AE56-$AE6E: 8-bit shift-accumulate: HLresult = horz_pos * road_width */
  /* borrow feeds the loop's first RLA below */
  carry       = (u16) HL < (u16) DE;
  DE          = HL - DE; /* road width = right_xpos − left_xpos */
  HLresult    = 0;
  A_mult      = IXhazard->horz_pos_on_road;
  Biterations = 8;
  do {
    RL(A_mult);
    if (carry) HLresult += DE;
    hl_carry = (HLresult >> 15) & 1; /* ADD HL,HL carry-out, before it is lost to truncation */
    HLresult <<= 1;
  } while (--Biterations > 0);

  /* xpos offset within road width */
  A_xresult = HLresult >> 8;
  carry     = hl_carry; /* Conv: RRA consumes the final loop's ADD HL,HL carry, not RLA's */
  RR(A_xresult);

  /* $AE6F-$AE79: C=A; HL=SM+BC (SM was patched to left_xpos at $AE57);
   * then IX[2]=L, IX[3]=H — write xpos back to hazard
   * Conv: Z80 self-modifies LD HL at $AE70 to load dhs_road_left_xpos then
   *       adds BC; C computes directly and writes the two fields below. */
  HL = state->dhs_road_left_xpos + A_xresult;
  /* $AE74-$AE79: IX[2],IX[3] = HL (horz_pos, horz_clip); written before the
   * call, matching skool -- $AE7A's result is discarded by the caller. */
  IXhazard->horz_pos  = HL & 0xFF;
  IXhazard->horz_clip = HL >> 8;
  (void) check_collision(state, 0, HL, IXhazard);

  Ddistance  = IXhazard->distance;
  Edist_frac = IXhazard->dist_frac;

  /* $AE83-$AE8C: load/increment n_hazards; Conv: $AE88 LD HL,$E900 →
   *              state->xpos_road_centre_left named array */
  HL_n_hazards  = &state->n_hazards;
  A_n_hazards   = *HL_n_hazards;
  (*HL_n_hazards)++;
  HLtable = &state->xpos_road_centre_left[0];

  /* $AE8E-$AE9D: scan draw list for insertion point (front-to-back order).
   * Each record is 2 words: word0 = distance (low byte) | frac (high byte),
   * word1 = hazard slot. HLtable stays on word0 of the current record so
   * both bytes can be tested without walking into the next record. */
  if (A_n_hazards) {
    Biterations = A_n_hazards;
    do {
      HLword0 = (u16) *HLtable;
      if (Ddistance >= (int)(u8) HLword0) {
        if (Ddistance != (int)(u8) HLword0)
          goto dhs_insert;
        if (Edist_frac < (int)(u8) (HLword0 >> 8))
          goto dhs_insert;
      }
      HLtable += 2;
    } while (--Biterations > 0);
  }

  /* $AE9F-$AEAA: append (distance,dist_frac) pair then hazard slot index */
  *HLtable++ = Ddistance | (Edist_frac << 8);
  *HLtable++ = IXhazard - &state->hazards[0]; /* Conv: slot index, not ptr */
  goto dhs_call_handler;

dhs_insert:
  /* $AEAB-$AEC7: PUSH DE; BC=B*4; LDDR; POP DE — shift the remaining
   * Biterations records (this one and all after it) up by one record to
   * make room, then write the new record into the freed slot.
   * Conv: LDDR shifts bytes; C shifts s16 words (BCwords = B*2 entries).
   * LDDR copies (DE) <- (HL): HLtable is the source (last word of the
   * block), DEtable is the destination (last word of the shifted block,
   * one record higher); both walk backward together. */
  HLstart = HLtable;
  BCwords = Biterations * 2;
  DEtable = HLtable + BCwords + 1; /* dest: one record past the shift block */
  HLtable = HLtable + BCwords - 1; /* src: both computed from the original HLtable */
  do { *DEtable-- = *HLtable--; } while (--BCwords > 0);
  HLstart[0] = Ddistance | (Edist_frac << 8);
  HLstart[1] = IXhazard - &state->hazards[0]; /* Conv: slot index, not ptr */

dhs_call_handler:
  IXhazard->hit_handler(state, IXhazard);
}

/**
 * $AECF: draw_hazard_sprites
 *
 * Per-hazard render callback invoked for each depth-sorted draw-list entry
 * whose distance matches [Biterations]. Returns immediately if the first
 * draw-table word does not match.
 *
 * For each hazard at the current depth it fetches the bitmap via the LOD table,
 * computes the screen x position from horz_pos/horz_clip and dispatches to
 * draw_object_left/right_width_entrypt. The perp-car path (hazard_flags ==
 * 0xFF) additionally saves the position into SM fields used by dhs_draw_bitmap,
 * then draws: the floating "HERE!" arrow (if smash_level < 5), fire overlays
 * (if smash_level >= 4) and trailing smoke (smash_level 1–3 via a fallthrough
 * switch).
 *
 * \param[in] state Pointer to game state.
 * \param[in] Biterations Current draw depth; must match draw-table entry.
 *   (was B)
 * \param[in] IYheight Pointer into the height table. (was IY)
 */
static void draw_hazard_sprites(chqstate_t *state,
                                int         Biterations,
                                const u8   *IYheight)
{
  // $CDEC
  //
  // 4 pair of X,Y
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
  // 6 pair of X,Y
  static const u8 smoke_offsets[6 * 2] = {
    0xEE, 0x08,
    0xF3, 0x08,
    0xF8, 0x08,
    0xFA, 0x04,
    0xFC, 0x00,
    0xFE, 0x00
  };

  s16            *HLtable;             /* pointer into the depth-sorted draw table at dhs_xpos_table (was HL) */
  int             A;                   /* Biterations copy: distance comparison and LOD index (was A) */
  int             DEbitmapoffset;      /* bitmap frame index × 7: byte offset into hittable.bitmaps (was DE) */
  const hazard_t *IXhazard;            /* hazard slot loaded from the current draw-list entry (was IX) */
  const bitmap_t *HLbitmap;            /* selected bitmap for the current hazard (was HL) */
  int             Ewidth_bits;         /* pixel width of selected bitmap: width_bytes * 8 (was E) */
  int             Ahorz_clip;          /* hazard's horizontal clip flag: zero=on-screen, non-zero=clipped (was A) */
  u8              Ahorz_pos;           /* hazard's horizontal screen position (was A) */
  int             Asum;                /* Ahorz_pos + Ewidth_bits; wide int to detect the 8-bit carry (was A) */
  int             Aindex;              /* smoke_bitmap_index: LOD level for the arrow offset lookup (was A) */
  const u8       *HLarrows;            /* pointer into arrow_offsets for the floating arrow x,y (was HL) */
  int             Asmash_level;        /* smash_level at time of fire/smoke dispatch (was A) */
  int             Asmash_level_scaled; /* (smash_level − 4) * 4 plus animation frame offset (was A) */
  const u8       *HLsmokes;            /* pointer into smoke_offsets for the smoke x,y (was HL) */
  u8              Bx;                  /* smoke or fire x position read from smoke_offsets (was B) */
  u8              Cy;                  /* smoke or fire y position read from smoke_offsets (was C) */

  // is $E900 pairs of (data-word, hazard-ptr) ?

  HLtable = state->dhs_xpos_table; // sampled = $E900
  A = Biterations;
  if (A != (*HLtable & 0xFF)) // Conv: original CP (HL) tests only the low byte
    return;

  if (--A >= 11)
    A = 10;
  A >>= 1;
  state->smoke_bitmap_index = A;

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
    state->doc_plot_mode = IXhazard->inverted;

    if ((u8) (IXhazard->hazard_flags + 1) == 0) /* Conv: truncate to 8 bits so 0xFF wraps to 0, matching Z80 INC A */
      goto dafs_af50;

    Ahorz_clip = IXhazard->horz_clip;
    // AND A3
    Ahorz_pos = IXhazard->horz_pos;
    if (Ahorz_clip < 0) {
      Asum = Ahorz_pos + Ewidth_bits;
      if (Asum < 0x100) // no carry
        goto dafs_draw_done_1;
      Ahorz_pos = (u8) Asum;
    } else if (Ahorz_clip != 0) {
      goto dafs_draw_done_1;
    } else {
      if (Ahorz_pos >= 128)
        goto dafs_draw_right_1;

      Ahorz_pos += Ewidth_bits;
    }

    draw_object_left_width_entrypt(state, Ahorz_pos, HLbitmap, IYheight);
    goto dafs_draw_done_1;

dafs_draw_right_1:
    draw_object_right_width_entrypt(state, Ahorz_pos, HLbitmap, IYheight);

dafs_draw_done_1:
    // POP DE (DEbitmapoffset), BC (Biterations)   ??

    state->doc_plot_mode = 0;

    if (--state->n_hazards == 0)
      return; // no more hazards

    // POP HL (HLtable)
  } while ((*HLtable & 0xFF) == Biterations); // Conv: original CP B tests only the low byte

  state->dhs_xpos_table = HLtable;
  return;

dafs_af50:
  Ahorz_clip = IXhazard->horz_clip;
  state->dhs_SM_B029_horz_clip = Ahorz_clip;
  Ahorz_pos = IXhazard->horz_pos;
  state->dhs_SM_B02C_horz_pos = Ahorz_pos;
  if (Ahorz_clip < 0) {
    Asum = Ahorz_pos + Ewidth_bits;
    if (Asum < 0x100) // no carry
      goto dafs_draw_done_1;
    Ahorz_pos = (u8) Asum;
  } else if (Ahorz_clip != 0) {
    goto dafs_draw_done_1;
  } else {
    if (Ahorz_pos >= 128)
      goto dafs_draw_right_2;

    Ahorz_pos += Ewidth_bits;
  }

dafs_draw_left_2:
  draw_object_left_width_entrypt(state, Ahorz_pos, HLbitmap, IYheight);
  goto dafs_done_draw_object;

dafs_draw_right_2:
  draw_object_right_width_entrypt(state, Ahorz_pos, HLbitmap, IYheight);

dafs_done_draw_object:
  state->dhs_col_pos = state->doc_col_pos;

  if (state->smash_level < 5 && (Aindex = state->smoke_bitmap_index) < 4) {
    HLarrows = &arrow_offsets[Aindex * 2];
    dhs_draw_bitmap(state, HLarrows[0], HLarrows[1], &floating_arrow_here_defn, IYheight);
  }

  Asmash_level = state->smash_level;
  if (Asmash_level >= 4) {
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

    dhs_draw(state, Bx, Cy, DEbitmapoffset, HLbitmap, IYheight);
  }

  /* Conv: Converted to switch */
  switch (state->smash_level) {
  case 0:
    goto dafs_draw_done_1;
  case 1:
    dhs_smoke(state, state->smokes[1], IYheight);
    break;
  case 2:
    dhs_smoke(state, state->smokes[0], IYheight);
    dhs_smoke(state, state->smokes[1], IYheight);
    break;
  default: /* Conv: 3 and above all fall through to the same smoke draw */
    dhs_smoke(state, state->smokes[2], IYheight);
    dhs_smoke(state, state->smokes[0], IYheight);
    dhs_smoke(state, state->smokes[1], IYheight);
    break;
  }
}

/**
 * $AFF1: dhs_smoke
 *
 * Advances one smoke particle's frame counter and draws a smoke puff at the
 * computed position.
 *
 * The counter in HLsmoke[0] counts down 5..1 and resets to 5. The smoke
 * animation index is smoke_bitmap_index + counter. If this exceeds 5 the
 * particle is not yet visible and the function returns. Otherwise it reads x,y
 * offsets from [HLsmoke] at stride index*2, subtracts counter from x and calls
 * dhs_draw with the matching smoke_defns frame.
 *
 * \param[in]     state Pointer to game state.
 * \param[in,out] HLsmoke Smoke particle data: [0]=counter, [1+]=x,y pairs.
 *   (was HL)
 * \param[in]     IYheight Pointer into the height table. (was IY)
 */
static void dhs_smoke(chqstate_t *state, u8 *HLsmoke, const u8 *IYheight)
{
  int counter;   /* frame counter: counts down 5..1, resets to 5 at zero (was A, then E) */
  int index;     /* smoke_bitmap_index: base LOD level (was A) */
  int newindex;  /* combined index: smoke_bitmap_index + counter; selects smoke frame (was A, C, D) */
  u8  x;         /* smoke x position from table, shifted left by counter (was B) */
  u8  y;         /* smoke y position from table (was C) */

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
  dhs_draw(state, x, y, newindex * 7, &smoke_defns[0],
          IYheight); /* was fallthrough */
}

/**
 * $B01B: dhs_draw
 *
 * Wrapper around dhs_draw_bitmap that selects a bitmap entry from an array by
 * byte offset. [DEoffset] is divided by 7 (size of bitmap_t) to produce the
 * array index before forwarding to dhs_draw_bitmap.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bx Horizontal offset added to doc_col_pos. (was B)
 * \param[in] Cy Vertical offset applied to horz_pos. (was C)
 * \param[in] DEoffset Byte offset into [HLbitmaps]; divided by 7 for index.
 *   (was DE)
 * \param[in] HLbitmaps Base of the bitmap_t array to index into. (was HL)
 * \param[in] IYheight Pointer into the height table. (was IY)
 */
static void dhs_draw(chqstate_t     *state,
                    int             Bx,
                    int             Cy,
                    int             DEoffset,
                    const bitmap_t *HLbitmaps,
                    const u8       *IYheight)
{
  dhs_draw_bitmap(state, Bx, Cy, &HLbitmaps[DEoffset / 7], IYheight);
}

/**
 * $B01C: dhs_draw_bitmap
 *
 * Draws a single sprite bitmap at a position derived from the SM fields
 * dhs_SM_B029_horz_clip and dhs_SM_B02C_horz_pos, shifted by [Bx] and [Cy]. These
 * SM fields are written by draw_hazard_sprites for the current perp hazard.
 *
 * If horz_clip is positive and non-zero the sprite is fully off-screen and the
 * function returns without drawing. Otherwise it adds [Cy] to horz_pos, checks
 * for carry and dispatches to draw_object_left/right based on whether the
 * result is >= 128.
 *
 * \param[in] state Pointer to game state.
 * \param[in] Bx Horizontal column offset added to dhs_col_pos. (was B)
 * \param[in] Cy Vertical offset added to the SM horz_pos value. (was C)
 * \param[in] HLbitmap Bitmap definition to draw. (was HL)
 * \param[in] IYheight Pointer into the height table. (was IY)
 */
static void dhs_draw_bitmap(chqstate_t     *state,
                           int             Bx,
                           int             Cy,
                           const bitmap_t *HLbitmap,
                           const u8       *IYheight)
{
  int Ewidth_bits;  /* pixel width of bitmap: width_bytes * 8 (was E) */
  int Ahorz_clip;   /* SM horz_clip value: sign and zero determine draw path (was A) */
  u8  Ahorz_pos;    /* SM horz_pos value adjusted by Cy; unsigned for carry detection (was A) */

  Ewidth_bits = HLbitmap->width_bytes * 8;
  state->doc_col_pos = state->dhs_col_pos + Bx;
  Ahorz_clip = state->dhs_SM_B029_horz_clip;
  // Set flags for A here
  Ahorz_pos = state->dhs_SM_B02C_horz_pos;
  if (Ahorz_clip >= 0) {
    if (Ahorz_clip)
      return;

    Ahorz_pos += Cy;
    if (Ahorz_pos < Cy) // carried
      return;

    if (Ahorz_pos >= 128) {
      draw_object_right_width_entrypt(state, Ahorz_pos, HLbitmap,
                                           IYheight); /* tail call */
    } else {
dhs_exit_1:
      Ahorz_pos += Ewidth_bits; // add pixel width
      draw_object_left_width_entrypt(state, Ahorz_pos, HLbitmap,
                                          IYheight); /* tail call */
    }
  } else {
    Ahorz_pos += Cy;
    if (Ahorz_pos < Cy) // carried
      goto dhs_exit_1;

    Ahorz_pos += Ewidth_bits;
    if (Ahorz_pos < Ewidth_bits) // carried
      draw_object_left_width_entrypt(state, Ahorz_pos, HLbitmap,
                                          IYheight); /* tail call */
  }
}

/**
 * $ADF9: No-op stub used as a null hazard handler
 *
 * The Z80 draws hazards through a table of function pointers. When a slot needs
 * no behaviour, the table entry points to the lone RET at $ADF9. In C this is a
 * genuinely empty function; the compiler emits no code.
 *
 * Conv: The Z80 RET is shared as a call target; in C we give it its own body.
 *
 * \param[in] state Pointer to game state.
 * \param[in] hazard Hazard entry (unused).
 */
void no_op(chqstate_t *state, hazard_t *hazard)
{
  NOT_USED(state);
  NOT_USED(hazard);
}

/**
 * $B063: Move hero car
 *
 * Called every frame from the main loop to advance the hero car's physical
 * state. Decrements the jump counter and feeds pitch and y-position from the
 * jump table while airborne. Manages the boost timer, smoke counter and
 * gear-change lockout. Computes the speed limit from gear, boost and off-road
 * state, applies a pitch-derived correction when inclined, then clamps speed to
 * [0, 511]. Accumulates left/right turning forces from user input and caps them
 * against the current speed. Converts curvature scroll ticks to a road_pos
 * delta. Writes turn_speed (0/1/2) and flip_car for the sprite renderer.
 *
 * \param[in,out] state Pointer to game state.
 */
static void move_hero_car(chqstate_t *state)
{
  /* Gear-speed thresholds */
  const int SpeedIdleChatter     = 120; /* trigger "get moving" chatter below this speed */
  const int SpeedOffRoad1        = 120; /* max speed with one wheel off-road */
  const int SpeedOffRoad2        = 110; /* max speed with both wheels off-road */
  const int SpeedHighGearMin     = 220; /* high gear uses two different max speeds above/below this */
  const int SpeedLowGear         = 230; /* low gear max without boost; high gear max below SpeedHighGearMin */
  const int SpeedBoosted         = 470; /* max speed with boost active (low gear, or high gear < SpeedHighGearMin) */
  const int SpeedHighGear        = 360; /* high gear max without boost */
  const int SpeedHighGearBoosted = 695; /* high gear max with boost */

  int        y_offset;             /* jump counter: 8..1 while airborne, 0 when landed (was A) */
  const u8  *jump_data;            /* pointer into hero_car_jump_table for current air frame (was HL) */
  int        Cinput;               /* latched user_input; masked to fire-only when crashed (was C) */
  int        Ainput;               /* copy of Cinput used for fire and brake/accel checks (was A) */
  int        fire_pressed;         /* non-zero when FIRE held and gear lockout is zero (Conv: extracted) */
  u8        *pgear;                /* pointer to state->gear for toggle/read (was HL) */
  int        gear_lockout;         /* gear-change lockout counter; counts down each frame (was A) */
  int        smoke;                /* value written to state->smoke on gear change (was A) */
  int        gear;                 /* current gear: 0=low, 1=high (was A) */
  int        speed;                /* current hero car speed in game units (was HL) */
  int        off_road;             /* off-road flags: 0=on, 1=one wheel off, 2=both off (was A) */
  int        BCmax_speed;          /* speed cap for current gear/boost/off-road combination (was BC) */
  int        BCspeed_diff;         /* signed delta applied to speed each frame (was BC) */
  int        boost;                /* turbo boost active flag; non-zero while boosting (was A) */
  int        Ainclined;            /* inclined_counter decremented; negative triggers pitch adjust (was A) */
  int        Apitch;               /* dhc_pitch value: selects pitch-speed-delta entry (was A) */
  int        BCpitch_speed_delta;  /* speed correction from road pitch: (Apitch − 5) | 1 (was BC) */
  int        DEoldspeed;           /* pre-pitch speed, restored if result exceeds 695 (was DE) */
  int        Hinput;               /* copy of Cinput used for left/right turn detection (was H) */
  int        Bright_turn;          /* rightward turning force accumulator, capped at 36 (was B) */
  int        Cleft_turn;           /* leftward turning force accumulator, capped at 36 (was C) */
  int        Aturn_speed;          /* speed-derived cap applied to turning forces (was A) */
  int        saved_Bright_turn;    /* Bright_turn saved across curvature scroll section (was B) */
  int        saved_Cleft_turn;     /* Cleft_turn saved across curvature scroll section (was C) */
  int        Bcount;               /* horizon scroll ticks consumed this frame (was B) */
  int        BCcount_scaled;       /* Bcount x 3, negated when scrolling right (was BC) */
  int        Enegative_scrolling;  /* non-zero when curvature is negative (rightward) (was E) */
  int        Acurrent_curvature;   /* current_curvature magnitude; index into horizon_table (was A) */
  const u8  *HLhorizon_table;      /* pointer into horizon_table for the current curvature (was HL) */
  int        Chorizon_scroll_sub;  /* horizon_scroll_sub: sub-pixel carry from last frame (was C) */
  int        Ahorizon_scroll_sub;  /* accumulated horizon scroll sub, updated after each tick (was A) */
  int        Acounter;             /* remaining fast_counter ticks minus horizon_scroll_sub (was A') */
  int        Chorz_tab_value;      /* horizon_table entry for this curvature (was C) */
  int        Acount;               /* copy of Bcount for curvature_ticks accumulation (was A') */
  int        saved_horiz_adj;      /* horizontal_adjust before clearing; diagnostic only (Conv: added) */
  int        HLhorizontal_adjust;  /* horizontal position delta: curvature scroll + net turn (was HL) */
  int        DEadjust;             /* horizontal adjustment when crashed: low byte of HL (was DE) */
  int        Acrashedflag;         /* ahc_crashed_flag on entry (was A) */
  int        Anet_turn;            /* net turning force: left_turn − right_turn (was A) */
  int        Acornering;           /* cornering flag written back to state (was A) */
  int        Dflip_car;            /* car sprite flip direction: 1=right, 0=left (was D) */
  int        Aturn_mag;            /* abs(DEadjust): magnitude of the SRA'd net turn (was A, $B2F2-$B2F8) */
  int        Bturn_speed;          /* animation rate: 0=straight, 1=turn, 2=turn-hard (was B) */

  y_offset = state->mhc_y_offset; // load jump counter, highest is 10
  assert(y_offset >= 0 && y_offset <= 10);
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
    state->user_input &= ~(USERINPUTFLAG_RIGHT | USERINPUTFLAG_LEFT | USERINPUTFLAG_DOWN |
                           USERINPUTFLAG_UP);
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
    Cinput &= USERINPUTFLAG_FIRE;

  Ainput = Cinput;
  // PUSH Ainput (PUSH AF)
  fire_pressed = (Ainput & USERINPUTFLAG_FIRE) != 0;
  pgear = &state->gear; // could use state
  gear_lockout = (s8) state->gear_lockout;
  if (fire_pressed != 0 && gear_lockout == 0) {
    *pgear ^= 1; // Toggle gear flag
    smoke = gear_lockout = 4;
    if (*pgear)
      state->smoke = smoke;
  }

  if (--gear_lockout >= 0)
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
      BCspeed_diff = (((BCmax_speed - speed) >> 4) & 0x3F) | 1;
    else
      BCspeed_diff = -((((speed - BCmax_speed) >> 4) & 0x1F) | 1); /* $B15A: excess over cap */
  } else {
    if (speed < SpeedHighGearMin) { // mhc_high_gear_slowing
      BCmax_speed = SpeedBoosted;
      // EX AF,AF' (unbank boost+flags)
      if (!boost) {
        BCspeed_diff = ((speed >> 4) | 1) & 0x1F; // $B18C JR $B19A (skip accel/decel check)
        goto mhc_check_brake;
      }
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
  if (Ainput & USERINPUTFLAG_DOWN) // checks BRAKE key
    BCspeed_diff = -20; // braking
  else if ((Ainput & USERINPUTFLAG_UP) == 0) // accelerate NOT pressed
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
    if (Hinput & USERINPUTFLAG_RIGHT)
      goto mhc_turning_right;
    if (Hinput & USERINPUTFLAG_LEFT)
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
      Acurrent_curvature = (u8)(-Acurrent_curvature); /* Z80 NEG is u8 */
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

      // EX AF,AF' - bank BCcount_scaled for scroll_horizon ($B854) to read
      state->curvature_scroll_shadow = (u8) BCcount_scaled;
      state->horizon_scroll_sub = Ahorizon_scroll_sub;
    }
  }

  // No curvature - No scroll required?
  saved_horiz_adj = state->horizontal_adjust;
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
  Anet_turn = saved_Cleft_turn - saved_Bright_turn; /* net turning: left − right ($B2B2/$B2B6) */
  if (Anet_turn < 0)
    DEadjust = 0xFF00;
  DEadjust = (DEadjust & 0xFF00) | (Anet_turn >> 1);
  HLhorizontal_adjust += DEadjust;
  if (Anet_turn < 0)
    Anet_turn = -Anet_turn; /* take magnitude for cornering/turn_speed below */

  Acornering = 0;
  if (Anet_turn >= 17) {
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
  if (DEadjust < 0) /* rightward net turn → flip sprite */
    Dflip_car = 0;

  /* $B2FA: turn_speed from |SRA(net_turn)|, compared to 12/6. Conv: must use
   * DEadjust (the already-SRA'd value) rather than re-deriving from the
   * truncating-divide Anet_turn/2 — they differ by 1 for odd negative
   * net_turn values, e.g. |SRA(-5)|=3 but |-5|/2=2. */
  Aturn_mag = (DEadjust < 0) ? -DEadjust : DEadjust;
  Bturn_speed = (Aturn_mag < 12) ? (Aturn_mag < 6) ? 0 : 1 :
                2; // straight/turn/turn-hard

  state->turn_speed = Bturn_speed; // should be 0/1/2
  state->flip_car   = Dflip_car;   // should be 0/1
  if (state->mhc_y_offset)
    state->cornering = 0; // reset cornering if jumping
}

/**
 * $B318: Animate hero car
 *
 * Called every frame after move_hero_car. Handles the crash-spin FSM: while
 * ahc_crashed_flag is set, decays speed by a quarter each frame, clears the
 * flag when speed falls below ahc_crash_speed_threshold, and advances the
 * crash-spin position via ahc_crash_spin_speed. Clamps road_pos to
 * ahc_road_pos_a/b bounds. Drives the perp-caught animation phase. Calls
 * draw_debris, ahc_check_hand_flag, draw_hero_car and (when cornering or
 * boosting) draw_smoke.
 *
 * \param[in,out] state Pointer to game state.
 */
static void animate_hero_car(chqstate_t *state)
{
  int HLspeed;            /* current hero car speed (was HL) */
  int Acrashed_flag;      /* ahc_crashed_flag on entry (was A) */
  int DEquartered_speed;  /* speed / 4 | 3: friction ramp during crash (was DE) */
  int Aturn_speed;        /* turn_speed value set during crash (was A) */
  int HL_b356;            /* crash spin speed decayed by 1/16 each frame (was HL) */
  int DE_b356;            /* copy of HL_b356 after EX DE,HL (was DE) */
  int HLroad_pos;         /* scenedata.road_pos, clamped to bounds (was HL) */
  int Cflip_flag;         /* ahc_flip_flag: selects add or subtract spin direction (was C) */
  int Adelay;             /* ahc_delay countdown; 0 triggers crash-spin frame advance (was A) */
  int DEother_road_pos;   /* road_pos bound from ahc_road_pos_a or ahc_road_pos_b (was DE) */
  int A;                  /* scratch: high-byte comparison result (was A) */
  int Aperp_caught_phase; /* perp_caught_phase: drives caught-criminal animation (was A) */
  int Aflipping;          /* ahc_crash_spin value: non-zero while crash spinning (was A) */
  int Cflipping;          /* spin frame index: Aflipping * 3 + 24, adjusted for turn rate (was C) */
  int Acounter_A;         /* anim_counter used to compute Bdash_anim_counter (was A) */
  int Bdash_anim_counter; /* anim_counter & 1: selects even/odd crash frame (was B) */
  int Cdash;              /* Bdash_anim_counter << 1: second frame parity argument (was A) */
  int Bwobble;            /* off-road vertical wobble offset: 0 or 3 at half rate (was B) */
  int Bsmoke_anim_frame;  /* animation counter fed to draw_smoke (was B) */

  if ((HLspeed = state->speed) == 0) {
    state->ahc_crash_spin = 0;
    state->off_road = 0;
  }

  // $B325
  if ((Acrashed_flag = state->ahc_crashed_flag) != 0) {
    state->cornering = Acrashed_flag;
    if (HLspeed < state->ahc_crash_speed_threshold)
      goto ahc_speed_less_or_eq;

    DEquartered_speed = (HLspeed >> 2) | 3;
    if (HLspeed == DEquartered_speed)
      goto ahc_speed_less_or_eq;

    Aturn_speed = 2; // fast turning
    if (HLspeed < DEquartered_speed) {
ahc_speed_less_or_eq:
      state->ahc_crashed_flag = 0;
      Aturn_speed = 1; // normal turning
    } else {
      state->speed = HLspeed - DEquartered_speed;
    }

    // $B353
    state->turn_speed = Aturn_speed;

    // $B356
    HL_b356 = state->ahc_crash_spin_speed - (state->ahc_crash_spin_speed >> 4);
    state->ahc_crash_spin_speed = HL_b356;
    DE_b356 = HL_b356; /* was EX DE,HL */

    // $B36B
    HLroad_pos = state->scenedata.road_pos;
    Cflip_flag = state->ahc_flip_flag;
    state->flip_car = Cflip_flag & 1;
    if (Cflip_flag != 1) {
      if (Cflip_flag != 2)
        HLroad_pos += DE_b356; // presumably zero
    } else {
      HLroad_pos -= DE_b356; // case 1
    }

    // $B381
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
      if ((HLroad_pos & 0xFF) < (DEother_road_pos & 0xFF)) // JR C: borrow
        goto ahc_b3b0;
    }

    DEother_road_pos = state->ahc_road_pos_b;
    A = HLroad_pos >> 8;
    if (A >= (DEother_road_pos >> 8)) { // carry
      if (A == (DEother_road_pos >> 8)) { // JR NZ: high bytes equal, check low
        A = (HLroad_pos & 0xFF) - (DEother_road_pos & 0xFF); // low diff
        if ((HLroad_pos & 0xFF) < (DEother_road_pos & 0xFF)) // JR C: borrow
          goto ahc_assign_road_pos_2;
      }

ahc_b3b0:
      // EX DE,HLroad_pos
      HLroad_pos = DEother_road_pos;
    }
  } else {
    // JP M: high byte negative → clamp to lower bound ($B39A EX DE,HL with DE=ahc_road_pos_a)
    HLroad_pos = DEother_road_pos;
  }

ahc_assign_road_pos_2:
  state->scenedata.road_pos = HLroad_pos;
  if (state->cornering || state->smoke)
    start_sfx(state, EFFECT_SQUEAL, 5); /* priority 5 */

  Aperp_caught_phase = state->perp_caught_phase;
  if (Aperp_caught_phase) {
    if (--Aperp_caught_phase != 0) { /* was > 1: update turn_speed and flip */
      Aturn_speed = (Aperp_caught_phase >= 2) ? 2 : Aperp_caught_phase;
      state->turn_speed = Aturn_speed;
      state->flip_car   = 1;
    }
    /* draw_debris always called regardless of phase ($B3D8) */
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
    Cdash = Bdash_anim_counter << 1;
    // EXX - unbank

    draw_crash(state, Cflipping, Bdash_anim_counter, Cdash);
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
  draw_smoke(state, Bsmoke_anim_frame, 1); // left hand; tail call
}

/**
 * $B457: AHC check hand flag
 *
 * Draws the "stop" hand overlay shown when the perp has been caught. Three
 * modes: HANDFLAG_NONE returns immediately; any other non-HANDFLAG_ANIMATING
 * value draws the static hand at frame 36 or 37 via draw_crash;
 * HANDFLAG_ANIMATING advances a two-speed delay counter
 * (ahc_hand_delay/ahc_hand_step) through 7 animation frames, enables the cherry
 * light on the final frame, and draws one or two hand sprites per tick via
 * draw_crash_unflipped.
 *
 * \param[in,out] state Pointer to game state.
 */
static void ahc_check_hand_flag(chqstate_t *state)
{
  int Ahand_flag;      /* hand_flag on entry: HANDFLAG_NONE/ANIMATING/static (was A) */
  int Bdash_flip_flag; /* hand_flag banked for EXX; passed to draw_crash (was B) */
  int Cdash;           /* hand_flag banked for EXX; passed to draw_crash (was C) */
  int Chand_flag;      /* ahc_hand_step: current animation step 0..6 (was C) */
  int Ahand_frame;     /* animation frame index derived from ahc_hand_delay/step (was A) */
  int Bhand_frame;     /* reset delay value: 2 normally, 3 on step 2 (was B) */
  int Chand_frame;     /* copy of Ahand_frame used for the second draw_crash_unflipped call (was C) */

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
      draw_crash(state, 36, Bdash_flip_flag, Cdash); /* tail call */
    else
      // Otherwise turn_speed is 2 (turn hard).
      draw_crash(state, state->flip_car + 37, Bdash_flip_flag, Cdash); /* tail call */
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
    draw_crash_unflipped(state, Ahand_flag); /* tail call */
  } else {
    state->cherry_light = 1;
  }
}

/**
 * $B4CC: Start chase
 *
 * Triggers when the perp vehicle is first sighted. Resets the hand animation
 * state (ahc_hand_step, ahc_hand_delay) and sets hand_flag to
 * HANDFLAG_ANIMATING. Enables sighted_flag (flashing lights and smash bar).
 * Resets the time limit (15 sixteenths, BCD 60 seconds). Toggles the marquee
 * left light brightness, shows the sighting overlay message, and starts the
 * siren.
 *
 * \param[in,out] state Pointer to game state.
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

  state->session.subsecond_ticks = SUBSECOND_TICKS_PER_SECOND;
  state->session.time_bcd        = CHASE_TIME_BCD;

  // Toggle the left light's brightness
  toggle_light_brightness(state, ADDRTOATTRS(MARQUEELIGHT_LEFT_ATTR_ADDR));

  // Show the "SIGHTING OF TARGET VEHICLE" message
  setup_overlay_messages(state, &sighting_message[0]);

  start_siren_hook(state); /* tail call */
}

/**
 * $B4F0: Smash
 *
 * Called each time the hero car rams the perp. Advances the
 * smash_cycling_counter (0..3) to select the next debris sub-table, resets the
 * debris frame counter to 9, and increments smash_counter. When smash_counter
 * reaches SMASHCOUNTER_MAX the perp is fully smashed via fully_smashed; one
 * below that triggers a "one more time" chatter. Updates smash_level (0..6)
 * from a stepped threshold table of hit counts.
 *
 * \param[in,out] state Pointer to game state.
 */
static void smash(chqstate_t *state)
{
  int counter; /* smash_cycling_counter incremented mod 4; selects debris sub-table (was A) */
  int hits;    /* smash_counter after increment: total ram count this stage (was A) */
  int level;   /* smash_level 0..6 derived from hit count thresholds (was C) */

  counter = (state->smash_cycling_counter + 1) & 3;
  state->smash_cycling_counter = counter;

  // Setup debris_table entry in draw_debris
  state->dd_debris_subtables_start = &state->debris_table[counter * 3];
  state->dd_SM_B549_frame_counter = 9; // set counter

  hits = state->smash_counter + 1;
  if (hits >= SMASHCOUNTER_MAX) {
    fully_smashed(state); /* tail call */
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
 * Draws the three debris pieces that fly off when the perp is rammed.
 * dd_SM_B549_frame_counter counts down from 9; when it reaches zero the
 * function returns early and draws nothing. Each of the three sub-tables
 * pointed to by dd_debris_subtables_start holds a 0..3 per-piece cycling
 * counter followed by a 9-entry y/x position table. The counter selects the
 * bitmap frame (12 bytes per frame in bitmap_debris); the current
 * dd_frame_offset selects the y/x pair within the sub-table. Each piece is
 * drawn via draw_part_entrypt2 at 6×1 bytes masked.
 *
 * \param[in,out] state Pointer to game state.
 */
static void draw_debris(chqstate_t *state)
{
  int       Aframe_counter;    /* dd_SM_B549_frame_counter: counts 9..1, 0=skip (was A) */
  int       Biterations;       /* loop counter: 3 debris pieces per smash (was B) */
  u8      **HLsubtables;       /* pointer walking dd_debris_subtables_start array (was HL) */
  u8       *DEsubtable;        /* pointer into the current debris sub-table (was DE) */
  int       BCframe_offset;    /* piece cycling counter x 12: byte offset into bitmap_debris (was BC) */
  int       HLoffset;          /* dd_frame_offset: selects y/x pair within the sub-table (was HL) */
  u8       *HLsubtable;        /* pointer to the y byte in the sub-table position entry (was HL) */
  u8        Dy;                /* y screen position for this debris piece (was D) */
  u8        Ex;                /* x screen position for this debris piece (was E) */
  const u8 *HLbitmap;          /* pointer into bitmap_debris at the chosen frame offset (was HL) */
  u8        Bheight;           /* sprite height: 6 rows (was B) */
  u8        Cwidth_bytes;      /* sprite width in bytes: 1 (was C) */
  int       BCdash;            /* shadow BC banked at EXX: 0 for no extra offset (was BC) */
  u8        Edash_width_bytes; /* shadow E banked at EXX: 1 byte wide (was E) */

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

    HLbitmap = &bitmap_debris[0][0] + BCframe_offset;

    Bheight = 6; // rows
    Cwidth_bytes = 1; // 1 byte wide masked?
    // EXX - bank
    BCdash = 0;
    Edash_width_bytes = 1;
    // EXX - unbank
    draw_part_entrypt2(state,
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
 * Draws the full hero car sprite at its current vertical position. First draws
 * the shadow (56-px wide) at y=120. Computes the car body index (0..8) from
 * turn_speed, [Bwobble] and dhc_pitch, looks up the hero_car_parts entry,
 * computes the back-buffer address from the adjusted y, then calls plot_sprite
 * or plot_sprite_flipped depending on flip_car. Finishes by drawing the
 * windscreen, wheels and both side panels via draw_hero_car_part.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Aturn_speed Turning speed index 0..2: straight/turn/turn-hard.
 *   (was A)
 * \param[in]     Bwobble Vertical wobble offset added to car body index.
 *   (was B)
 */
static void draw_hero_car(chqstate_t *state, int Aturn_speed, int Bwobble)
{
  int              Cturn_speed;          /* copy of Aturn_speed preserved across shadow draw (was C) */
  u8               Dy;                   /* vertical screen position: 117 minus dhc_jump_y (was D) */
  int              Acar_direction;       /* body part index 0..8: turn + wobble + pitch (was A) */
  const carpart_t *HLcarpart;            /* pointer into hero_car_parts[Acar_direction] (was HL) */
  u8               Ey;                   /* adjusted y after subtracting car_y and part y offset (was E) */
  u16              DEbackbuf_addr;       /* back-buffer plot address derived from Ey (was DE) */
  u8               Bdash_height;         /* sprite row count from carpart (was B') */
  const u8        *HLbitmap_data;        /* pointer to car body bitmap data (was HL') */
  u8               Awidth_bytes;         /* sprite byte width: 5 (was A) */
  u16              DEbitmap_stride;      /* bitmap row stride: 5 (was DE') */
  u16              HLdash_backbuf_addr;  /* copy of DEbackbuf_addr for plot call (was HL') */
  int              Adash_flip_car;       /* flip_car flag: 0=normal, 1=flipped (was A') */

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
 * Draws one car part (shadow, windscreen, wheels or side panel) by reading the
 * y-offset and row count from the carpart_t entry, subtracting the part's y
 * from [Dy], and calling draw_part. flip_car selects the start offset (Cdash):
 * 0 when unflipped, [Cwidth_bytes] − 1 when flipped. Returns a pointer to the
 * next carpart_t entry so callers can chain draws.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Cwidth_bytes Sprite width in bytes; also the bitmap stride.
 *   (was C)
 * \param[in]     Dy Base vertical screen position of the car body. (was D)
 * \param[in]     Ex Horizontal screen position in pixels. (was E)
 * \param[in]     HLpart Pointer to the carpart_t entry to draw. (was HL)
 * \return Pointer to the following carpart_t entry. (was HL)
 */
static const carpart_t *draw_hero_car_part(chqstate_t      *state,
    int               Cwidth_bytes,
    int               Dy,
    int               Ex,
    const carpart_t *HLpart)
{
  int       Ay;                   /* copy of Dy: base y before part offset subtraction (was A) */
  u8        Dnew_y;               /* Ay minus the part's y offset (was D) */
  u8        Bheight;              /* row count from the carpart entry (was B) */
  const u8 *HLbitmap;             /* pointer to bitmap data from the carpart entry (was HL) */
  int       Bdash_flip_flag;      /* flip_car banked for EXX; passed to draw_part (was B') */
  u8        Edash_bitmap_stride;  /* bitmap stride = Cwidth_bytes, banked at EXX (was E') */
  u8        Cdash;                /* start offset: 0 unflipped, Cwidth_bytes−1 flipped (was C') */

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
 * Draws one turbo-exhaust smoke cloud. Looks up the frame in
 * hero_car_turbo_smoke[Aanim_frame]. Returns immediately if the car is airborne
 * (mhc_y_offset != 0). When unflipped, Cdash is set to 0 and x comes from
 * unflipped_x; when flipped, Cdash is width − 1 and x comes from flipped_x.
 * Draws via draw_part at y=119, centred at x + 127.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Aanim_frame Index into hero_car_turbo_smoke[]. (was A)
 * \param[in]     Adash_flip_flag Non-zero to draw flipped (left exhaust).
 *   (was A')
 */
static void draw_smoke(chqstate_t *state, int Aanim_frame, int Adash_flip_flag)
{
  const carsmokeframe_t *HLframe;           /* pointer to the chosen smoke animation frame (was HL) */
  u8                     Cwidth;            /* sprite byte width from the frame (was C) */
  u8                     Bheight;           /* sprite row count from the frame (was B) */
  u8                     Dflipped_x;        /* horizontal position used when flipped (was D) */
  u8                     Eunflipped_x;      /* horizontal position used when unflipped (was E) */
  const u8              *HLbitmap;          /* pointer to sprite pixel data from the frame (was HL) */
  u8                     Cdash;             /* start offset: 0 unflipped, width−1 flipped (was C') */
  u8                     Bdash_flip_flag;   /* Adash_flip_flag banked at EXX (was B') */
  u8                     Edash_width_bytes; /* stride = Cwidth, banked at EXX (was E') */
  u8                     Ax;               /* selected horizontal position, offset by 127 (was A) */

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
            Edash_width_bytes); /* tail call */
}

/**
 * $B67C: Draw cherry light
 *
 * Draws the spinning cherry (police) light on the car roof. Advances
 * [Aframe_index] by the slow_anim_counter LSB for the base flicker. When
 * turn_speed is at or above [Bturn_limit], adds [Cturn_delta] once (straight)
 * or twice (flipped) to shift to the turned-light frames. Renders via
 * draw_crash_unflipped.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Aframe_index Base frame index into the cherry-light sprite
 *   table. (was A)
 * \param[in]     Bturn_limit Minimum turn_speed that triggers the turn-offset
 *   path. (was B)
 * \param[in]     Cturn_delta Frame delta applied once or twice when turning.
 *   (was C)
 */
static void draw_cherry_light(chqstate_t *state,
                              int          Aframe_index,
                              int          Bturn_limit,
                              int          Cturn_delta)
{
  int Aturn_speed; /* turn-derived frame offset: Cturn_delta * 1 or 2 (was A) */
  int Cturn_speed; /* copy of Aturn_speed passed to Aframe_index (was C) */

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
 * $B699: Draw crash frame with no flip and no extra offset
 *
 * Convenience entry point that calls draw_crash with Bdash_flip_flag=0 and
 * Cdash=0, producing an unflipped render with no horizontal start offset.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Aframe_index Frame index into car_frames[]. (was A)
 */
static void draw_crash_unflipped(chqstate_t *state, int Aframe_index)
{
  draw_crash(state, Aframe_index, 0, 0); /* was FALLTHROUGH */
}

/**
 * $B69E: Draw crash
 *
 * Draws one frame of the crash/hand/smash animation. Looks up the
 * car_frames[Aframe_index] entry for the x/y offset and adornment index, reads
 * the adornment's dimensions and bitmap, adjusts y for jump height and road
 * pitch, then calls draw_part.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Aframe_index Index into car_frames[]. (was A)
 * \param[in]     Bdash_flip_flag Non-zero to draw the frame horizontally
 *   flipped. (was B')
 * \param[in]     Cdash Horizontal start offset within the sprite (was C')
 */
static void draw_crash(chqstate_t *state,
                       int          Aframe_index,
                       int          Bdash_flip_flag,
                       int          Cdash)
{
  const carframe_t     *frame;         /* pointer to car_frames[Aframe_index] (was HL) */
  u8                    y;             /* base vertical position: frame->y + 121 adjusted for jump/pitch (was D) */
  u8                    x;             /* horizontal position: 128 + frame->x (was E) */
  const caradornment_t *adornment;     /* pointer to car_adornments[frame->index / 4] (was HL) */
  u8                    height;        /* sprite row count from the adornment (was B) */
  u8                    width;         /* sprite byte width from the adornment (was C) */
  const u8             *bitmap;        /* pointer to sprite pixel data from the adornment (was HL) */
  u8                    bitmap_stride; /* bitmap row stride: equals width (was E) */
  int                   pitch;         /* dhc_pitch >> 1: 0/1/3 vertical correction from road incline (was A) */

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
 * $B6D6: Reduce y by car_y then draw part
 *
 * Subtracts state->car_y from [y] before forwarding all parameters to
 * draw_part_entrypt2. car_y encodes the on-screen vertical slot of the car;
 * subtracting it converts an absolute row number to a back-buffer row.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     height Number of pixel rows to draw. (was B)
 * \param[in]     width Sprite byte width. (was C)
 * \param[in]     y Absolute vertical screen row before car_y adjustment.
 *   (was D)
 * \param[in]     x Horizontal screen position in pixels. (was E)
 * \param[in]     bitmap Source sprite data. (was HL)
 * \param[in]     Bdash_flip_flag Non-zero to draw horizontally flipped.
 *   (was B')
 * \param[in]     Cdash Horizontal start offset within the sprite. (was C')
 * \param[in]     Edash_bitmap_stride Sprite row stride in bytes. (was E')
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
  draw_part_entrypt2(state,
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
 * $B6DD: Draw part — second entry point
 *
 * Core sprite compositor shared by all car-part draws. Computes the back-buffer
 * address from ([Dy], [Ex]) using the ZX Spectrum screen layout formula (bits
 * 3..6 of y to RRR, bits 0..3 of y to LLLL, x >> 3 to CCCCC), adds [Cdash] as a
 * horizontal sub-byte offset, then dispatches to
 * plot_masked_sprite_flipped_entrypt2 (when [Bdash_flip_flag] is set) or
 * draw_part_plot_masked_sprite (when clear).
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Bheight Number of pixel rows to draw. (was B)
 * \param[in]     Cwidth_bytes Sprite byte width. (was C)
 * \param[in]     Dy Vertical screen row after car_y adjustment. (was D)
 * \param[in]     Ex Horizontal screen position in pixels. (was E)
 * \param[in]     HLbitmap_data Source sprite data. (was HL)
 * \param[in]     Bdash_flip_flag Non-zero to draw horizontally flipped.
 *   (was B')
 * \param[in]     Cdash Horizontal start offset within the sprite. (was C')
 * \param[in]     Edash_bitmap_stride Sprite row stride in bytes. (was E')
 */
static void draw_part_entrypt2(chqstate_t *state,
                               int         Bheight,
                               int         Cwidth_bytes,
                               int         Dy,
                               int         Ex,
                               const u8  *HLbitmap_data,
                               int         Bdash_flip_flag,
                               int         Cdash,
                               int         Edash_bitmap_stride)
{
  int carry_flip_flag; /* LSB of Bdash_flip_flag: 1=flip, 0=normal (was carry) */
  u8  Ay;              /* copy of Dy banked across EX AF,AF' (was A) */
  u16 DEbackbuf;       /* back-buffer address assembled from x and y fields (was DE) */
  u8  Estride;         /* bitmap stride doubled: Cwidth_bytes << 1 (was E) */
  u16 HLdash_backbuf;  /* copy of DEbackbuf with Cdash added for EXX (was HL') */
  u8  Awidth_bytes;    /* sprite stride from Edash_bitmap_stride (was A) */

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
    plot_masked_sprite_flipped_entrypt2(state,
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
 * $B701: Draw part — plot masked sprite
 *
 * Converts [Awidth_bytes] (1..8) to an IX jump-table offset ((8 −
 * [Awidth_bytes]) × 6) and forwards to plot_masked_sprite. The jump offset
 * selects the fall-through case in the unrolled byte-copy loop so that only
 * [Awidth_bytes] bytes are written per row.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Awidth_bytes Sprite width in bytes, 1..8. (was A)
 * \param[in]     HLbackbuf_addr Back-buffer write address. (was HL')
 * \param[in]     Bdash_height Number of pixel rows to draw. (was B')
 * \param[in]     Edash_bitmap_stride Sprite row stride in bytes. (was E')
 * \param[in]     HLdash_bitmap_data Source masked sprite data. (was HL')
 */
static void draw_part_plot_masked_sprite(chqstate_t *state,
    int          Awidth_bytes,
    u8         *HLbackbuf_addr,
    int          Bdash_height,
    int          Edash_bitmap_stride,
    const u8   *HLdash_bitmap_data)
{
  int IXjump_offset; /* fall-through index: (8 − Awidth_bytes) × 6 (was IX) */

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
 * Draws a masked sprite to the back buffer using a fall-through switch that
 * mimics the Z80's jump-table dispatch. Each row writes up to 8 pairs of (mask,
 * data) bytes: the back-buffer byte is ANDed with mask then ORed with data.
 * After each row the source pointer is advanced by [bitmap_stride] and the
 * back-buffer pointer is reset to the row-start address plus one (scanline
 * advance). Iterates until [height] rows are drawn.
 *
 * \param[in,out] state Pointer to game state (unused but required by
 *   signature).
 * \param[in]     jump_offset Fall-through start index: (8 − width) × 6.
 *   (was IX)
 * \param[in]     height Number of pixel rows to draw. (was B)
 * \param[in]     bitmap_stride Source bitmap row stride in bytes. (was DE)
 * \param[in]     bitmap_data Source masked sprite data (mask/data byte pairs).
 *   (was HL)
 * \param[in]     backbuf_addr Back-buffer write address for the first row.
 *   (was HL')
 */
static u8 *plot_masked_sprite(chqstate_t *state,
                              int         jump_offset,
                              int         height,
                              int         bitmap_stride,
                              const u8   *bitmap_data,
                              u8         *backbuf_addr)
{
  const u8 *src;          /* pointer to current mask/data byte pair in the source (was SP) */
  u8       *backbuf_orig; /* start of current back-buffer row; advanced each scanline (was C) */
  u8        mask;         /* mask byte: ANDed with back-buffer byte before OR (was E) */
  u8        data;         /* data byte: ORed into back-buffer byte after masking (was D) */

  assert(jump_offset / 6 >= 0);
  assert(jump_offset / 6 <= 7);
  assert(VALID_BACKBUF_PTR(backbuf_addr));

  goto plot_masked_sprite_entry;

  for (;;) {
    // EXX - Unbank
    if (--height == 0)
      return backbuf_addr;

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
    backbuf_addr = ADDRTOBACKBUF(prev_buf_row(BACKBUFTOADDR(
        backbuf_orig))); // Restore row start address
  }
}

/**
 * $B76C: Plot a flipped and masked sprite
 *
 * Adjusts [backbuf_addr] forward by [width_bytes] so that the second entry
 * point begins at the right edge of the sprite, then falls through to
 * plot_masked_sprite_flipped_entrypt2 which draws rightward-to-leftward.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     width_bytes Sprite width in bytes. (was A)
 * \param[in]     backbuf_addr Back-buffer address of the left edge of the
 *   sprite. (was HL)
 * \param[in]     height Number of pixel rows to draw. (was B')
 * \param[in]     bitmap_stride Source bitmap row stride in bytes. (was E')
 * \param[in]     bitmap_data Source masked sprite data. (was HL')
 */
static void plot_masked_sprite_flipped(chqstate_t *state,
                                       int         width_bytes,
                                       u8         *backbuf_addr,
                                       int         height,
                                       int         bitmap_stride,
                                       const u8   *bitmap_data)
{
  plot_masked_sprite_flipped_entrypt2(state,
                                    width_bytes,
                                    backbuf_addr + width_bytes, // moving dst ptr to end
                                    height,
                                    bitmap_stride,
                                    bitmap_data); /* was FALLTHROUGH */
}

/**
 * $B770: Plot a flipped and masked sprite — second entry point
 *
 * Draws a horizontally flipped masked sprite. [backbuf_addr] points one byte
 * past the right edge of the sprite (the first byte written is at
 * [backbuf_addr] − 1). Selects a fall-through case in a descending switch (case
 * 0 = widest, case 7 = narrowest): each step reads a (mask, data) pair from the
 * source, bit-reverses both via state->flipped[], and writes the combined byte
 * leftward through the back buffer. After each row the source pointer advances
 * by [bitmap_stride] and the back buffer returns to the row-start address less
 * one.
 *
 * \param[in,out] state Pointer to game state (flipped[] lookup table).
 * \param[in]     width_bytes Sprite width in bytes, 1..8. (was A)
 * \param[in]     backbuf_addr Back-buffer address one byte past the right edge.
 *   (was HL)
 * \param[in]     height Number of pixel rows to draw. (was B')
 * \param[in]     bitmap_stride Source bitmap row stride in bytes. (was E')
 * \param[in]     bitmap_data Source masked sprite data. (was HL')
 */
static void plot_masked_sprite_flipped_entrypt2(chqstate_t *state,
    int         width_bytes,
    u8         *backbuf_addr,
    int         height,
    int         bitmap_stride,
    const u8   *bitmap_data)
{
  int       jump_offset;   /* fall-through start: 8 − width_bytes (was IX) */
  const u8 *src;           /* pointer to current mask/data pair in the source (was SP) */
  u8       *backbuf_orig;  /* start of current back-buffer row; reset after each row (was A') */
  u8        mask;          /* mask byte, bit-reversed via state->flipped[] (was C) */
  u8        data;          /* data byte, bit-reversed via state->flipped[] (was B) */

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
    backbuf_addr = ADDRTOBACKBUF(prev_buf_row(BACKBUFTOADDR(
        backbuf_orig))); // Restore row start address
  }
}

/**
 * $B7EF: Plot a masked sprite drawn bottom-to-top
 *
 * Used when barriers are flipped upside-down. Computes the address of the last
 * row of the bitmap ((height − 1) × stride + base), negates the stride, and
 * calls plot_masked_sprite so the bitmap is consumed in reverse row order,
 * producing a vertically inverted render. Observed with BC' = $0704 (7 rows,
 * 4-byte stride) and HL' = bitmap_barrier_4s.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Awidth_bytes Sprite width in bytes. (was A)
 * \param[in]     HLbackbuf_addr Back-buffer write address for the first
 *   (topmost) row. (was HL)
 * \param[in]     Bdash_height Number of pixel rows to draw. (was B')
 * \param[in]     Edash_bitmap_stride Source bitmap row stride in bytes
 *   (positive). (was E')
 * \param[in]     HLdash_bitmap_data Pointer to the first byte of the source
 *   bitmap. (was HL')
 */
static void plot_masked_sprite_inverted(chqstate_t *state,
                                        int         Awidth_bytes,
                                        u8         *HLbackbuf_addr,
                                        int         Bdash_height,
                                        int         Edash_bitmap_stride,
                                        const u8   *HLdash_bitmap_data)
{
  int       jump_offset;              /* fall-through index: (8 − Awidth_bytes) × 6 (was IX) */
  u16       DEdash_bitmap_stride;     /* stride then negated; unsigned to allow 16-bit negation (was DE) */
  const u8 *HLdash_bitmap_data_final; /* pointer to the last bitmap row (was HL') */
  u8        Bheight;                  /* copy of Bdash_height passed to plot_masked_sprite (was B) */

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
                     (s16)DEdash_bitmap_stride, /* Conv: sign-extend; u16 -> int
                                                    would otherwise drop the sign */
                     HLdash_bitmap_data_final,
                     HLbackbuf_addr); /* tail call pms_entry */
}

/**
 * $B848: Scroll horizon
 *
 * Updates the horizon scroll state each frame. Returns immediately when speed
 * is zero. Horizontal section: when current_curvature is non-zero, derives a
 * scroll-rate entry from horizon_table using a speed-scaled index, decrements
 * horizon_x_scroll, and on underflow wraps dr_horizon_x_scroll (0..19) by the
 * table value's direction byte. Vertical section: reads the current incline,
 * looks up the per-step threshold, counts how many ticks have elapsed since the
 * last horizon_y_step, accumulates the delta into horizon_y_accum, and adjusts
 * session.horizon_level and horizon_y_step.
 *
 * \param[in,out] state Pointer to game state.
 */
static void scroll_horizon(chqstate_t *state)
{
  int        carry;                     /* carry from RL operations on Adash (carry) */
  int        speed;                     /* hero car speed; early-out if zero (was HL) */
  int        current_curvature;         /* current_curvature: selects horizontal scroll path (was A) */
  u8         Adash;                     /* banked A': approximated as 0 — Z80 carried this in from $B296 (was A') */
  u8         Bhorizon_table_value;      /* high byte of horizon_table entry: reload for horizon_x_scroll (was B) */
  u8         Chorizon_x_delta;         /* low byte of horizon_table entry: signed direction for x-scroll (was C) */
  int        Aregular;                  /* dr_horizon_x_scroll after wrap, 0..19 (was A) */
  int        Ahorizon_y_a25a_delta;     /* accumulated sub-step delta added to horizon_y_step (was A') in loop */
  int        Bcounter;                  /* number of horizon_y ticks consumed this frame (was B) */
  int        Eset_if_incline_negative;  /* 1 when incline is negative (downhill); sign-extends BCcounter (was E) */
  int        Aincline;                  /* state->incline value; magnitude used for table index (was A) */
  const u8  *HLhorizon_table;           /* pointer into horizon_table for incline-rate lookup (was HL) */
  int        Adiff;                     /* fast_counter minus horizon_y_step: ticks since last advance (was A) */
  int        Chorizon_table_value;      /* threshold from horizon_table for the vertical scroll rate (was C) */
  int        BCcounter;                 /* signed tick count: positive=uphill, negative=downhill (was BC) */

  carry = 0;

  if ((speed = state->speed) == 0)
    return;

  // Horizontal scrolling
  //

  if ((current_curvature = state->current_curvature) != 0) {
    // Conv: current_curvature is banked into A' by EX AF,AF' at $B854 and is
    // never read back — it is immediately overwritten by LD A,C at $B873.
    // The RLA sequence instead operates on the old A' value (Adash), which
    // the Z80 carries in from move_hero_car ($B296): the last BCcount_scaled
    // it banked into shadow. state->curvature_scroll_shadow models that
    // shadow slot (see State.h) — none of the calls between $B296 and $B854
    // (spawn_cars, cycle_counters, play_engine_or_siren_sfx_hook,
    // build_height_table) execute EX AF,AF', so it survives unclobbered.
    // Conv: $B854: EX AF,AF' banks current_curvature; $B855: RR H gives carry
    /* RL Adash x3; AND 6: top two bits of speed, scaled up by 2 */
    Adash = ((state->curvature_scroll_shadow >> 6) & 2) | (((speed >> 8) & 1) << 2);
    Bhorizon_table_value = horizon_table[state->horizon_curve_index + Adash];
    Chorizon_x_delta     = horizon_table[state->horizon_curve_index + Adash + 1];

    // Decrement horizon_a25e
    if (--state->horizon_x_scroll == 0) {
      state->horizon_x_scroll = Bhorizon_table_value;
      // EX AF,AF' - $B872 restores AF from the bank made at $B854;
      // F now holds the Sign flag from AND A ($B851) which reflects the
      // sign of current_curvature.  JP P ($B874) therefore branches on the
      // sign of current_curvature, not on the sign of C.
      Aregular = Chorizon_x_delta;
      if ((s8) current_curvature < 0)
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

  /* $B898: HL = horizon_table-1; $B89B: BC=Aincline; $B89C: HL+=BC → byte offset = Aincline-1 */
  HLhorizon_table = &horizon_table[Aincline - 1];
  Adiff = state->fast_counter - state->horizon_y_step;
  if (!Adiff)
    return; /* no ticks elapsed, nothing to do */

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

  state->horizon_y_accum += Bcounter;

  // Sign extend based on low bit of Eset_if_incline_negative
  BCcounter = (Eset_if_incline_negative) ? -Bcounter : Bcounter;

  // Adjust horizon_level
  state->session.horizon_level += BCcounter;
  // EX AF,AF' — unbanks Ahorizon_y_a25a_delta (accumulated in loop)
  state->horizon_y_step += Ahorizon_y_a25a_delta;
}

/**
 * $B8D2: Update per-frame road-level state
 *
 * Called once per frame from read_map. Covers six groups of state:
 *
 * 1. Horizon level: adjusts session.horizon_level by the accumulated incline
 * step, then resets the accumulator. 2. Incline: reads the next height byte
 * from the road buffer, halves it (SRA) to derive the new incline, and stores
 * it. 3. Pitch: derives the dhc_pitch animation value from the frontmost height
 * byte. 4. Car jump: detects whether the hero car should launch off a road
 * crest and selects jump parameters from car_jump_resume_params. 5. Curvature:
 * reads the new curvature byte (negating for the taken fork), derives
 * horizon_curve_index, and updates horizon_x_scroll. 6. Horizontal adjust:
 * computes the per-frame steering correction from the old-vs-new curvature
 * difference, then resets curvature counters.
 *
 * \param[in] state Pointer to game state.
 */
static void update_road_level(chqstate_t *state)
{
  int       carry;              /* carry/borrow flag */
  int       Bhorizon_accum;     /* accumulated horizon step from scroll_horizon (was B) */
  int       Cnegate_flag;       /* 1 if incline is negative (climbing) (was C) */
  int       Aincline;           /* current incline value, made positive for arith (was A) */
  const u8 *HLroadbuf;          /* pointer into road buffer (was HL) */
  int       Aheight;            /* road height byte, halved for new incline (was A) */
  int       Cheight;            /* frontmost height byte, kept for pitch (was C) */
  int       Bpitch;             /* derived pitch value for dhc_pitch (was B) */
  s8       *HLprev_road_height; /* pointer to state->prev_road_height (was HL) */
  int       Aprev_road_height;  /* previous road height byte (was A) */
  int       Bprev_road_height;  /* previous road height, positive copy (was B) */
  int       Ay_offset;          /* current mhc_y_offset jump counter (was A) */
  int       Adiff;              /* height difference triggering jump (was A) */
  int       car_jump_params_index; /* byte index into car_jump_params (no register) */
  const u8 *HLjump_params;      /* pointer into car_jump_params (was HL) */
  int       Ehero_car_jump_table_index; /* jump table row offset into hero_car_jump_table (was E) */
  int       Acurrent_curvature; /* curvature value from previous frame (was A) */
  int       Afork_visible;      /* fork_visible flag (was A) */
  int       Acurvature_byte;    /* curvature byte read from road buffer (was A) */
  int       Afork_taken;        /* fork_taken flag (was A) */
  int       Bcurv_idx;          /* scaled curvature index copy for BC indexing (was B) */
  int       Ax_scroll;          /* computed horizon x-scroll value (was A) */
  int       Bcurvature_ticks;   /* accumulated curvature ticks from previous frame (was B) */
  int       C_curv_dir;         /* 1 if old curvature was negative (was C) */
  int       A_curv_diff;        /* old-minus-new curvature magnitude (was A) */
  int       B_sign_ext;         /* sign-extension byte for horizontal_adjust (was B) */

  carry = 0;
  Bhorizon_accum = state->horizon_y_accum; // load and widen
  Cnegate_flag = 0;
  Aincline = state->incline;
  if (Aincline < 0) { // if road climbing
    Aincline = -Aincline;
    Cnegate_flag = 1; /* was INC C */
  }

  Aincline -= Bhorizon_accum;
  if (Aincline)
    state->session.horizon_level += (Cnegate_flag) ? -Aincline : Aincline;

  HLroadbuf = ROADBUF_FWD2PTR(ROADBUF_HEIGHT_OFFSET + 2);

  state->horizon_y_step = state->horizon_y_accum = 0;

  Aheight = (s8) * HLroadbuf >> 1;
  if (Aheight < 0)
    Aheight++;
  state->incline = Aheight;

  // $B90F
  WRAP_ASSIGN(HLroadbuf, -2, state->roadbuf_start);
  Cheight = Aheight = (s8) *HLroadbuf;
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

  // $B92B
  HLprev_road_height = &state->prev_road_height;
  Aprev_road_height = *HLprev_road_height;
  if (Aprev_road_height < 0) { // could combine exprs
    if ((Cheight & (1 << 7)) == 0) { // ie. positive
      Aprev_road_height = -Aprev_road_height;
      carry = Aprev_road_height < 2, Aprev_road_height -= 2;
      if (!carry) {
        Bprev_road_height = Aprev_road_height;
        Ay_offset = state->mhc_y_offset;
        assert(Ay_offset >= 0 && Ay_offset <= 10); /* table y_heights reach 10 */
        if (!Ay_offset) { /* Z80: JR NZ → skip if already airborne */
          Adiff = Bprev_road_height - (3 - ((state->speed >> 7) & 3)); // result = 1..6 // Conv: folded a lot here
          if (Adiff > 0) { /* was !C && !Z */
            // PUSH HLprev_road_height
            // Conv: $B95A-$B961: RLCA (A*=2) folded into the index. Z80 forms
            // HL = $B057 + 2A but the table starts at $B059, so the byte
            // offset is (A-1)*2 — always the first byte of a pair.
            // Conv: clamp Adiff to 5. For Adiff == 6 (height byte -8 at top
            // speed) the Z80 reads the two code bytes at $B063 that follow
            // the table, yielding a garbage jump; use the longest jump
            // instead.
            if (Adiff > 5)
              Adiff = 5;
            car_jump_params_index = (Adiff - 1) * 2;
            assert(car_jump_params_index >= 0 && car_jump_params_index <= 8);
            HLjump_params = &car_jump_params[car_jump_params_index];
            Ehero_car_jump_table_index = *HLjump_params++; // an offset
            assert(*HLjump_params <= 10);
            state->mhc_y_offset = *HLjump_params;
            assert(Ehero_car_jump_table_index >= 0 && Ehero_car_jump_table_index <= 19);
            state->mhc_jump_data = &hero_car_jump_table[Ehero_car_jump_table_index];
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
    Bcurv_idx = Acurvature_byte;
    if (state->horizon_x_scroll)
      goto url_B9C5;

    Ax_scroll = horizon_table[((state->speed >> 6) & 6) +
                              (Bcurv_idx & 0xFF)]; // use of BC removed
  } else {
    Ax_scroll = Acurvature_byte; // Conv: added
  }
  state->horizon_x_scroll = Ax_scroll;

url_B9C5:
  Bcurvature_ticks = state->curvature_ticks;
  C_curv_dir = 0;
  // EX AF,AF' - unbank Acurrent_curvature
  if ((s8) Acurrent_curvature < 0) {
    Acurrent_curvature = (u8)(-Acurrent_curvature); /* Z80 NEG is u8 */
    C_curv_dir++;
  }

  A_curv_diff = Acurrent_curvature - Bcurvature_ticks;
  if ((s8) A_curv_diff > 0) {
    A_curv_diff = (A_curv_diff << 2) + (A_curv_diff >> 1);
    B_sign_ext = 0;
    if (C_curv_dir & 1) { // invert BA
      B_sign_ext = 0xFF; /* was DEC B */
      A_curv_diff = -A_curv_diff;
    }
    state->horizontal_adjust = (B_sign_ext << 8) | A_curv_diff;
  }
  state->horizon_scroll_sub = state->curvature_ticks = 0;
}

/**
 * $B9F4: Lay out the road x-position tables for the current frame
 *
 * Called once per frame from read_map. Scans up to 20 lane-data entries looking
 * for a fork marker (byte & 0xE1 == 0xE1). Two paths:
 *
 * Non-fork: calls build_curve_table (straight road), then fills 104 entries of
 * xpos_road_centre, xpos_road_centre_right, and xpos_road_centre_left by
 * interpolating between the already-populated xpos_road_left and
 * xpos_road_right tables.
 *
 * Fork: records which fork the player is taking, triggers chatter, advances the
 * spawn accumulator and fork_distance, then calls build_curve_table twice (once
 * per road half, adjusting road_pos temporarily). The fork inner loop runs
 * Bdash_fork_iters iterations (from the height at the fork), with the remaining
 * entries handled by falling through to the non-fork path.
 *
 * \param[in] state Pointer to game state.
 */
static void layout_road(chqstate_t *state)
{
  u8       *DElanedata_base;   /* base of lane data in road buffer, for wrap-around (was DE) */
  u8       *DElanedata;        /* advancing pointer through lane data entries (was DE) */
  int       Biterations;       /* entries to scan; counts down 20→0 (was B) */
  int       Ldistance_to_fork; /* count of lane entries before a fork marker (was L) */
  s16      *SProadright;       /* pointer into xpos_road_right[], post-incremented per entry (was SP) */
  u8        Aiterations;       /* loop counter: 0x30→0 in steps of 2 (u8 wrap = 104 iters) (was A) */
  s16      *SMroadcentre;      /* pointer to current xpos_road_centre output slot (was $BA36 SM) */
  s16      *SMroadcentreleft;  /* pointer to current xpos_road_centre_left output slot (was $BA45 SM) */
  s16      *SMroadcentreright; /* pointer to current xpos_road_centre_right output slot (was $BA40 SM) */
  s16      *SMroadleft;        /* pointer to current xpos_road_left input slot (was $BA29 SM) */
  int       DEdash;            /* road-left x-position read from xpos_road_left (was DE') */
  int       HLdash;            /* x-position accumulator; updated through each interpolation (was HL') */
  int       BCdash;            /* half-width copy for centre-right/centre-left calc (was BC') */
  u8       *HLheight_at_fork;  /* pointer into height_table at fork distance (was HL) */
  u8        Bdash_fork_iters;  /* DJNZ counter for fork inner loop; from PUSH AF at $BA60 (was B') */
  int       HLforkdistance;    /* fork_distance; updated and written back (was HL) */
  int       Aforkinprogress;   /* fork_in_progress minus 1 (was A) */
  int       DEroadpos;         /* road_pos for fork-side detection (was DE) */
  int       D_side;            /* road_pos high byte minus 1; sign/zero selects fork side (was D) */
  const u8 *HLchatterblk;      /* pointer to chatter block for correct/incorrect fork (was HL) */
  int       C_spawn_accum;     /* candidate new spawn_accumulator (was C) */
  int       DEforkdistance;    /* fork distance copy for temporary road_pos adjustment (was DE) */
  int       HLroadpos;         /* road_pos shifted for one fork's curve build (was HL) */
  int       HLroadpos_saved;   /* saved road_pos restored after both fork curve builds (was HL) */
  s16      *SMveryright;       /* pointer to current xpos_road_fork_right input slot (was SM) */
  s16      *SMroadright;       /* pointer to current xpos_road_right output slot (was SM) */

  // $B9F4: Point at lane data
  DElanedata_base = DElanedata = ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET);

  // $B9F9: Count the distance to the forked road
  Biterations = PERSP_TABLE_COLS;
  Ldistance_to_fork = 0;
  do {
    if ((*DElanedata & 0xE1) == 0xE1)
      goto lr_forked_road;

    WRAP_INCREMENT_ASSIGN(DElanedata, DElanedata_base);
    Ldistance_to_fork++;
  } while (--Biterations > 0);

  // $BA0B: No forked road found
  build_curve_table(state, /*forked=*/0);
  SProadright = &state->xpos_road_right[48 >> 1];
  Aiterations = 48 >> 1; // 48..256 in steps of 2 = 104 iterations

  // $BA17: Forked code jumps back here
lr_calc_single_lane:
  do {
    // $BA20
    SMroadcentre      = &state->xpos_road_centre[Aiterations];
    SMroadcentreleft  = &state->xpos_road_centre_left[Aiterations];
    SMroadcentreright = &state->xpos_road_centre_right[Aiterations];
    SMroadleft        = &state->xpos_road_left[Aiterations];

    // $BA26 EXX Bank

    // $BA27: Centre = Left + (Right - Left) / 2
    DEdash = *SMroadleft; // read from road left (s16: negative when road_pos < 295)
    HLdash = *SProadright++; // POP from $ECxx
    HLdash = (HLdash - DEdash) >> 1; // halve total width
    BCdash = HLdash; // stash halved width
    HLdash += DEdash; // calc centre from left
    *SMroadcentre = HLdash; // store centre pos

    // $BA38
    DEdash = BCdash;
    BCdash >>= 1; // halve again for quarter width
    HLdash += BCdash; // calc centre+quarter width
    *SMroadcentreright = HLdash; // store centre-right pos

    // $BA44
    HLdash -= DEdash; // calc centre-halved width
    *SMroadcentreleft = HLdash; // store centre-left pos

    // $BA47 - moved below
    // $BA4A EXX Unbank
  } while (++Aiterations != 128);
  return;

lr_forked_road:
  state->fork_countdown = Ldistance_to_fork;
  HLheight_at_fork = &state->height_table[Ldistance_to_fork];
  Aiterations = 96;
  state->fork_visible = Aiterations; // just a flag AFACIT
  // Conv: Z80 $BA5C: SUB (HL); CPL; ADD A,$69 = (8 + *HL) & 0xFF
  Aiterations = (u8)(8 + *HLheight_at_fork);
  // Conv: models PUSH AF at $BA60; value recovered at $BB0B POP BC as DJNZ counter
  Bdash_fork_iters = Aiterations;
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
  D_side = ((DEroadpos >> 8) - 1) & 0xFF; /* was DEC D */
  // Chooses the fork taken based on car's distance from centre
  // Conv: Z80 $BA7B JP M,$BA89 fires when D−1 is negative as a byte (D==0 or
  //       D≥0x81): right fork with A==1. $BA7E JP NZ,$BA88 fires when D−1 is
  //       1..0x7F: left fork. Only D==1 falls through to the E<12 test.
  if (D_side & 0x80) {
    // Right fork: Aiterations stays 1
  } else if (D_side != 0) {
    Aiterations--; // 1 → 0: left fork
  } else if ((DEroadpos & 0xFF) >= 12) { // E >= 12: not close to centre
    Aiterations--; // 1 → 0: left fork
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
  C_spawn_accum = Aiterations; // new value for $A16D
  Aiterations -= 2;
  if (Aiterations >= 256 - 2) // carried?
    goto lr_set_var_a16d_from_c;
  C_spawn_accum = Aiterations; // new value for $A16D
  HLforkdistance += 16;
  state->fork_distance = HLforkdistance;
lr_set_var_a16d_from_c:
  state->session.spawn_accumulator = C_spawn_accum;
lr_no_car_spawning:
  /* RL A x4; AND 0x0F: low nibble = (fast_counter >> 5) | (spawn_accumulator & 1) << 3 */
  Aiterations = ((state->fast_counter >> 5) & 0x07) |
                ((state->session.spawn_accumulator & 1) << 3); // $BAD7
  Aiterations -= 0x10; // sets top nibble to $F
  // Conv: Z80 $BADB LD E,A; LD D,$FF forms signed DE = 0xFF00|A ∈ {−16..−1}
  HLforkdistance += (s16)(0xFF00 | (u8)Aiterations);
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
  // Conv: $BB0B POP BC restores PUSH AF value into B = Bdash_fork_iters (DJNZ counter)
  // Conv: $BB0C/$BB10 restore SP to $EC30 (road right) for POP-based reads
  SProadright = &state->xpos_road_right[0x30 >> 1];
  Aiterations = 0x30;
  do {
    SMroadcentre      = &state->xpos_road_centre[Aiterations >> 1];
    SMroadcentreleft  = &state->xpos_road_centre_left[Aiterations >> 1];
    SMroadleft        = &state->xpos_road_left[Aiterations >> 1];
    SMveryright       = &state->xpos_road_fork_right[Aiterations >> 1];
    SMroadcentreright = &state->xpos_road_centre_right[Aiterations >> 1];
    SMroadright       = &state->xpos_road_right[Aiterations >> 1];
    // EXX Bank for inner loop
    DEdash = *SMroadleft;
    HLdash = *SProadright++;
    // Conv: accumulate HLdash through each ADD HL,DE; SRA H; RR L step ($BB32-$BB3F)
    HLdash = (HLdash + DEdash) / 2; // (right+left)/2 = new road centre
    *SMroadcentre = HLdash;
    HLdash = (HLdash + DEdash) / 2; // (centre+left)/2 = new road centre left
    *SMroadcentreleft = HLdash;

    DEdash = *SMveryright;
    HLdash = *SMroadcentreright;
    HLdash = (HLdash + DEdash) / 2; // (centre-right+fork-right)/2 = new centre-right
    *SMroadcentreright = HLdash;
    HLdash = (HLdash + DEdash) / 2; // (new-centre-right+fork-right)/2 = road right
    *SMroadright = HLdash;

    Aiterations += 2;
    // EXX Unbank
  } while (--Bdash_fork_iters != 0); /* Conv: models DJNZ $BB5C */
  // Conv: Z80 $BB5E JP Z,$BA4D: if A wrapped to 0, all 104 entries done → exit
  if (Aiterations == 0)
    return;
  SProadright = &state->xpos_road_right[Aiterations >> 1];
  // Conv: switch Aiterations from raw Z80 form (0x30..0xFE) to halved array index
  Aiterations >>= 1;
  goto lr_calc_single_lane;
}

/**
 * $BB69: Transition the road back to a single lane after a fork exit
 *
 * Guards on the high byte of fork_distance being non-zero; returns immediately
 * if the fork has not progressed far enough.
 *
 * Based on fork_taken, selects the left or right exit configuration: - Points
 * the road data stream pointers at the forked-road exit tables (curvature,
 * height, lanes, hazards, side objects). - Wires the per-command SM handlers
 * via lookup_map_goto for each stream on the chosen branch. - Fills 32
 * curvature bytes with the turn type, 32 lane bytes with the lane type, and
 * zeros 32 object bytes in the road buffer.
 *
 * Finally resets all fork and road counters so the engine resumes single-road
 * rendering.
 *
 * \param[in] state Pointer to game state.
 */
static void exit_fork(chqstate_t *state)
{
  int D_curve_type;   /* curvature byte to fill: +4 (right turn) or -4 (left) (was D) */
  int E_lanes_type;   /* lanes byte to fill: 0x03 (right fork) or 0x01 (left) (was E) */
  int C_obj_offset;   /* extra road-buffer offset for zeroing object bytes (was C) */
  int Bfill;        /* fill-loop iteration counter; models DJNZ with B=32 (was B) */

  /* $BB69: return if fork_distance high byte is zero */
  if ((state->fork_distance & 0xFF00) == 0)
    return;

  if (state->fork_taken == 1) {
    /* $BBA1: ef_right */
    D_curve_type = 0x04;
    E_lanes_type = 0x03;
    state->scenedata.road_leftside_ptr  = forked_road_exit_rightobjs - 1;
    state->scenedata.road_rightside_ptr = forked_road_exit_leftobjs - 1;
    state->scenedata.road_lanes_ptr     = forked_road_exit_right_lanes - 1;
    state->rm_curvature_fork_end_ptr = lookup_map_goto(state->current_stage_number,
      state->rm_rightfork_curve);
    state->rm_height_fork_end_ptr    = lookup_map_goto(state->current_stage_number,
      state->rm_rightfork_height);
    state->rm_lanes_fork_end_ptr     = lookup_map_goto(state->current_stage_number,
      state->rm_rightfork_lanes);
    state->rm_hazards_fork_end_ptr   = lookup_map_goto(state->current_stage_number,
      state->rm_rightfork_hazards);
    state->rm_rightside_fork_end_ptr = lookup_map_goto(state->current_stage_number,
      state->rm_rightfork_rightside);
    state->rm_leftside_fork_end_ptr  = lookup_map_goto(state->current_stage_number,
      state->rm_rightfork_leftside);
    // Conv: Z80 $BBCA LD A,$20; maps to ROADBUF_LANES_OFFSET + 32 = ROADBUF_RIGHTOBJS_OFFSET
    C_obj_offset = 32;
  } else {
    /* $BB74: ef_left */
    D_curve_type = 0xFC;
    E_lanes_type = 0x01;
    state->scenedata.road_leftside_ptr  = forked_road_exit_leftobjs - 1;
    state->scenedata.road_rightside_ptr = forked_road_exit_rightobjs - 1;
    state->scenedata.road_lanes_ptr     = forked_road_exit_left_lanes - 1;
    state->rm_curvature_fork_end_ptr = lookup_map_goto(state->current_stage_number,
      state->rm_leftfork_curve);
    state->rm_height_fork_end_ptr    = lookup_map_goto(state->current_stage_number,
      state->rm_leftfork_height);
    state->rm_lanes_fork_end_ptr     = lookup_map_goto(state->current_stage_number,
      state->rm_leftfork_lanes);
    state->rm_hazards_fork_end_ptr   = lookup_map_goto(state->current_stage_number,
      state->rm_leftfork_hazards);
    state->rm_rightside_fork_end_ptr = lookup_map_goto(state->current_stage_number,
      state->rm_leftfork_rightside);
    state->rm_leftside_fork_end_ptr  = lookup_map_goto(state->current_stage_number,
      state->rm_leftfork_leftside);
    // Conv: Z80 $BB9D LD A,$40; maps to ROADBUF_LANES_OFFSET + 64 = ROADBUF_LEFTOBJS_OFFSET
    C_obj_offset = 64;
  }

  /* $BBE3: common exit-fork road pointers */
  state->scenedata.road_curvature_ptr = forked_road_exit_curvature - 1;
  state->scenedata.road_height_ptr    = forked_road_exit_height - 1;
  state->scenedata.road_hazard_ptr    = forked_road_exit_hazards - 1;

  /* $BBFD: fill 32 curvature bytes, 32 lanes bytes, 32 object bytes (zeroed) */
  /* Conv: Z80 uses three DJNZ loops (B=32 each); C counts up 0..31 instead */
  // TODO: use memset
  for (Bfill = 0; Bfill < 32; Bfill++)
    *ROADBUF_FWD2PTR(ROADBUF_CURVATURE_OFFSET + Bfill) = D_curve_type;
  for (Bfill = 0; Bfill < 32; Bfill++)
    *ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET + Bfill) = E_lanes_type;
  for (Bfill = 0; Bfill < 32; Bfill++)
    *ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET + C_obj_offset + Bfill) = 0;

  /* $BC15: reset per-frame road state */
  state->curvature_byte            = 0;
  state->height_byte               = 0;
  state->leftside_byte             = 0;
  state->rightside_byte            = 0;
  state->hazards_counter           = 0;
  state->lanes_counter             = 0;
  state->fork_in_progress          = 0;
  state->fork_taken                = 0;
  state->fork_visible              = 0;
  state->session.no_objects_flag   = 1;
  state->session.spawn_accumulator = 1;
  state->fork_distance             = 0;
}

// The screen has the format 0b010BBLLLRRRCCCCC (B = band, L = scanline, R = row (group), C = column)
// The buffer has the format 0b1111LLLLRRRCCCCC (L = scanline, R = row (group))

/**
 * $BC3E: Copy the backbuffer to the screen and update attributes.
 *
 * Transfers all 128 rows of the road backbuffer to the playfield area of the ZX
 * Spectrum screen ($4800–$57FF), then updates the sky/ground horizon colour row
 * and the smash-meter attribute strip.
 *
 * The Z80 original temporarily hijacks SP to use PUSH/POP as a fast bulk-copy
 * engine: it POPs 16 (or 14) bytes forward out of the backbuffer via HL',
 * reverses them onto the screen by PUSHing from HL, and advances H of each
 * pointer by 1 (= +$100, one ZX scanline) per step. Two passes per row-group
 * cover the left 16 bytes ($BC49 ds_loop_16bytes) then the right 14 bytes
 * ($BCC8 ds_loop_14bytes) of each row. Eight groups of 16 scanlines account for
 * the full 128-row playfield. The ZX screen's non-linear three-band address
 * layout requires a pointer reset to $5011 at the 64-row midpoint ($BD45) and
 * an offset-based advance at $BD4D when the backbuffer high nibble overflows.
 * Conv: All PUSH/POP bulk-copy chains are replaced by memcpy.
 *
 * The attribute section ($BD5A ds_attributes) reads the one-frame-lagged
 * horizon delta from state->horizon_attr[2] to scroll the sky/ground colour
 * boundary up or down by that many attribute rows.
 *
 * The smash-meter section ($BD93 ds_smash_meter) paints six attribute rows at
 * $5962 with the smash-o-meter colour gradient when a perp is sighted.
 *
 * \param[in] state Pointer to game state.
 */
static void update_screen(chqstate_t *state)
{
  /* Conv: Z80 restores SP via a self-modified instruction at $BDBE; C
   * has no equivalent and passes the dirty rect to the host draw callback. */
  static const zxbox_t playfield_box = { /* lower two-thirds of screen */
    0, 0, SCREEN_WIDTH, PLAYFIELD_HEIGHT
  };

  u8        *HLscr;        /* screen write pointer (was HL) */
  u8        *HLbuf;        /* backbuffer read pointer (was HL') */
  u16        bufoffset;    /* backbuffer offset for row-group boundary test (Conv: added) */
  ptrdiff_t  screen_off;   /* screen address offset for pointer arithmetic (Conv: added) */
  u8         H_bufpage;    /* backbuffer start page $F0, used as subtraction base (was H) */
  u8         A_buflo;      /* low byte of bufoffset; compared against H_bufpage (was A) */
  int        A_sub;        /* A_buflo − H_bufpage; result of $BD34 SUB H (was A) */
  int        carry;        /* unsigned borrow: set while row-groups remain */
  int        overflow;     /* signed overflow of $BD34 SUB H; fires at 64-row midpoint */
  u8         L_nextlo;     /* low byte of next row-group backbuffer start (was L) */
  u8         A_cur_delta;  /* current horizon attr delta, $E34C (was A) */
  u8         E_prev_delta; /* previous horizon attr delta, $E34D (was E) */
  u8         D_sign;       /* sign extension of E_prev_delta (was D) */
  s16        DElevel;      /* signed attr-row offset in bytes (was DE) */
  u8        *HLattrs;      /* pointer into attr memory (was HL) */
  u16        BCattrs;      /* attribute colour word (was BC) */
  u8         Cattr;        /* single attribute byte (was C) */

  HLscr = ADDRTOSCREEN(0x4811); // (136, 64)
  HLbuf = ADDRTOBACKBUF(0xF001); // (8, 1)

  for (;;) {
    // Conv: $BC49-$BCB8 ds_loop_16bytes: Z80 uses SP/PUSH/POP chains to bulk-copy
    //       16 bytes per scanline; C uses memcpy. Left-half of each row (bytes 17-32).
    do {
      memcpy(HLscr - 16, HLbuf, 16); HLscr += 256; HLbuf += 256;
      memcpy(HLscr - 16, HLbuf, 16); HLscr += 256; HLbuf += 256;
      memcpy(HLscr - 16, HLbuf, 16); HLscr += 256; HLbuf += 256;
      memcpy(HLscr - 16, HLbuf, 16); HLscr += 256; HLbuf += 256;
      bufoffset = BACKBUFTOOFFSET_LR(HLbuf, 0, 256); // Conv: convert back to offset
      // Loop on the first pass (4 lines of 8 done) but not the second
    } while (bufoffset & (1 << 10));

    // Conv: $BCBE-$BCC7: advance both pointers to the right-half start.
    // $BCC8-$BD27 ds_loop_14bytes: 14-byte copy per scanline (bytes 2-15).
    HLbuf = OFFSETTOBACKBUF(bufoffset - 0x07F0);
    screen_off = SCREENTOOFFSET_LR(HLscr, 0, 256) - 0x07F2;
    HLscr = OFFSETTOSCREEN(screen_off);
    do {
      memcpy(HLscr - 14, HLbuf, 14); HLscr += 256; HLbuf += 256;
      memcpy(HLscr - 14, HLbuf, 14); HLscr += 256; HLbuf += 256;
      memcpy(HLscr - 14, HLbuf, 14); HLscr += 256; HLbuf += 256;
      memcpy(HLscr - 14, HLbuf, 14); HLscr += 256; HLbuf += 256;
      bufoffset = BACKBUFTOOFFSET_LR(HLbuf, 0, 256); // Conv: convert back to offset
      // Loop on the first pass (4 lines of 8 done) but not the second
    } while (bufoffset & (1 << 10));

    if ((bufoffset & (1 << 11)) == 0) {
      /* HLbuf has advanced past a 0x1000 boundary (bit 11 just cleared),
       * meaning another 16 backbuffer rows have been written and it is
       * time to advance to the next row-group.
       *
       * This is a Z80 "SUB H" on the low byte of bufoffset, using H set
       * to 0xF0 — the high byte of BACKBUFFER_START_ADDRESS.  Each time
       * the magic fires, A_buflo holds successive low bytes of the
       * end-of-group bufoffset: 0x11, 0x31, 0x51, 0x71, 0x91, 0xB1,
       * 0xD1, 0xF1.
       *
       * carry    (unsigned A_buflo < H_bufpage = 0xF0): set for the first
       *          seven groups (A_buflo = 0x11..0xD1); clear at 0xF1,
       *          meaning all eight groups of 16 rows (128 total) → break.
       *
       * overflow (signed 8-bit overflow of A_buflo - H_bufpage): fires
       *          when A_buflo + 16 exceeds +127, i.e. the first time
       *          A_buflo = 0x71. That is exactly the 64-row midpoint of
       *          the backbuffer, which corresponds to the boundary between
       *          the ZX Spectrum screen's middle third (rows 64-127,
       *          starting at 0x4811) and its bottom third (rows 128-191,
       *          starting at 0x5011).
       *
       * L_nextlo = (A_buflo - H_bufpage) & 0xFF is the low byte of the
       * next row-group's backbuffer start address, so
       * ADDRTOBACKBUF(0xF000 | L_nextlo) resets HLbuf to the next group.
       */
      H_bufpage = 0xF0;
      A_buflo   = bufoffset & 0xFF;
      A_sub     = A_buflo - H_bufpage;
      carry     = (A_buflo < H_bufpage); // unsigned: set while rows remain
      overflow  = ((H_bufpage ^ A_buflo) & (A_sub ^ A_buflo)) >> 7; // V-flag
      L_nextlo  = A_sub; // low byte of next group start

      if (!carry)
        break; // A_buflo >= 0xF0: all 128 rows written

      HLbuf = ADDRTOBACKBUF((H_bufpage << 8) | L_nextlo); // next row-group

      if (!overflow) {
        screen_off = SCREENTOOFFSET_LR(HLscr, 0, 256) - 0x07EE;
        HLscr = OFFSETTOSCREEN(screen_off);
      } else {
        // 64-row midpoint: jump to the ZX screen's bottom third
        HLscr = ADDRTOSCREEN(0x5011); // (136, 128)
      }
    } else {
      screen_off = SCREENTOOFFSET_LR(HLscr, 0, 256) - 0x07EE;
      HLscr = OFFSETTOSCREEN(screen_off);
      HLbuf -= 16;
    }
  }

  /* Set screen attributes */
  {
    // Don't update the attributes if the level intro screen is being shown
    if (state->dont_draw_screen_attrs)
      goto exit;

    A_cur_delta  = state->horizon_attr[1]; // current delta ($E34C)
    E_prev_delta = state->horizon_attr[2]; // previous delta ($E34D)
    state->horizon_attr[2] = A_cur_delta;  // $E34D = current
    // $BD67 LD A,E — use the *previous* delta for movement this frame
    if (E_prev_delta != 0) {
      // Sign-extend from E_prev_delta before shifting it
      D_sign       = (E_prev_delta >= 64) ? 0xFF : 0x00; /* was SBC A,A */
      E_prev_delta = (E_prev_delta << 2); /* previous * 4 (attr bytes) */
      DElevel      = (D_sign << 8) | E_prev_delta;

      assert(state->session.horizon_attribute != 0);

      HLattrs = ADDRTOATTRS(state->session.horizon_attribute);

      // Set sky colour by default
      BCattrs = (attribute_BRIGHT_BLACK_OVER_CYAN << 8) |
                attribute_BRIGHT_BLACK_OVER_CYAN;
      // $BD76 JR Z — sky if D==0, ground if D!=0
      if (DElevel < 0) {
        // Set ground colour; adjust pointer up before filling
        BCattrs = state->stage->ground_colour;
        HLattrs += DElevel;
      }

      // Z80 fills 30 bytes backward via PUSH BC×15 from end-of-row pointer;
      // equivalent forward fill starts 30 bytes before HLattrs.
      memset(HLattrs - 30, BCattrs & 0xFF, 30);
      // $BD8F ADD HL,DE — sky only: advance pointer to next row
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
  state->speccy->draw(state->speccy, &playfield_box); /* Conv: Added */
}

/**
 * $BDC1: Clear the playfield bitmap and set the attribute colour gradient.
 *
 * Clears the playfield (lower two-thirds of screen) via clear_playfield, then
 * resets its bitmap to $FF and lays in three attribute bands: two rows of plain
 * sky (black/cyan), three rows of bright sky (bright black/cyan), and eleven
 * rows of the stage ground colour. Finally stamps both edge columns of every
 * playfield attribute row with black-on-black to hide the road overdraw at the
 * screen borders.
 *
 * \param[in] state Pointer to game state.
 */
static void set_playfield_attrs(chqstate_t *state)
{
  static const zxbox_t playfield_box = { /* lower two-thirds of screen */
    0, 0, SCREEN_WIDTH, PLAYFIELD_HEIGHT
  };

  u8  *HLattrs;     /* attribute pointer walking left/right edge columns (was HL) */
  int  DEoffset;    /* byte distance from left to right edge column in one row (was DE) */
  int  Biterations; /* row iteration count (was B) */

  clear_playfield(state);

  /* Conv: $BDC4-$BDCF: LDIR re-clears $4800-$57FF with $FF, duplicating work
   * done by clear_playfield. The Z80 BC=$0FFF causes the copy to overshoot
   * by one row into $5800-$58FF (top-third attribute rows), but those bytes
   * are restored by scoreboard drawing immediately after and are harmless. */
  memset(ADDRTOSCREEN(SCREEN_PLAYFIELD_BITMAP_ADDR), 0xFF,
         PLAYFIELD_HEIGHT * SCREEN_BITMAP_ROWBYTES);

  /* $BDD1-$BDDB: rows 0-1 of playfield → $28 (black ink, cyan paper: sky) */
  memset(ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR),
         attribute_BLACK_OVER_CYAN,
         2 * SCREEN_ATTRIBUTES_ROWBYTES);

  /* $BDDD-$BDE1: rows 2-4 of playfield → $68 (bright black/cyan: lit sky) */
  memset(ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR + 2 * SCREEN_ATTRIBUTES_ROWBYTES),
         attribute_BRIGHT_BLACK_OVER_CYAN,
         3 * SCREEN_ATTRIBUTES_ROWBYTES);

  /* $BDE3-$BDEA: rows 5-15 of playfield → stage ground colour */
  memset(ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR + 5 * SCREEN_ATTRIBUTES_ROWBYTES),
         state->stage->ground_colour,
         11 * SCREEN_ATTRIBUTES_ROWBYTES);

  /* $BDEC-$BDF8: write $00 (black/black) to attribute columns 0 and 31 of
   * each of the 16 playfield rows. HL starts at row 0 col 0; ADD HL,DE
   * (DE=$001F=31) steps to col 31 of the same row; INC HL then lands on
   * col 0 of the next row. D=$00 is both the write value and the high byte
   * of the stride, so attribute_BLACK_OVER_BLACK is used directly. */
  HLattrs  = ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR);
  DEoffset = SCREEN_ATTRIBUTES_ROWBYTES - 1; /* $1F = col 31 offset within a row */
  Biterations = 16;
  do {
    *HLattrs   = attribute_BLACK_OVER_BLACK; /* left edge: col 0 */
    HLattrs   += DEoffset;
    *HLattrs++ = attribute_BLACK_OVER_BLACK; /* right edge: col 31 */
  } while (--Biterations > 0);
  state->speccy->draw(state->speccy, &playfield_box); /* Conv: added */
}

/**
 * $BDFB: Read the next map frame into the road buffer.
 *
 * Resets the per-frame SFX triggers and the allow-spawning counter, then
 * decides how many times to advance the cyclic road buffer this frame. At
 * normal speed (high byte of speed = 0) the buffer advances once only when
 * fast_counter overflows after accumulating the speed value. At high speed
 * (high byte odd) the buffer is advanced once unconditionally before the
 * accumulation, and a second time if the accumulation overflows. All actual
 * buffer advancing and data-channel decoding are delegated to
 * rm_cycle_buffer_offset.
 *
 * \param[in] state Pointer to game state.
 */
static void read_map(chqstate_t *state)
{
  u8  *HLfast_counter; /* pointer to the per-frame accumulator (was HL) */
  int  DEspeed;        /* current vehicle speed word (was DE) */
  int  Aspeed_lo;      /* low byte of speed; added to fast_counter each frame (was A) */

  state->trigger_lefthand_sfx = state->trigger_righthand_sfx = 0;
  state->allow_spawning = 0;

  HLfast_counter = &state->fast_counter;
  DEspeed   = state->speed;
  Aspeed_lo = DEspeed & 0xFF;

  /* $BE0D-$BE10: RR D sets carry = D's old bit 0; JR NC skips when carry=0.
   * Extra cycle when D is odd (in practice D is 0 or 1, so this is
   * equivalent to D != 0). Conv: translated as (DEspeed & 0x100) which
   * checks bit 0 of D exactly; the prior C `speed > 255` was equivalent
   * for D in {0,1} but wrong for D >= 2. */
  if (DEspeed & 0x100)
    rm_cycle_buffer_offset(state, HLfast_counter);

  *HLfast_counter += (u8)Aspeed_lo;

  /* $BE1A-$BE1C: LD A,$00; JP NC,$C0D9: if no carry skip to rm_inc_spawning
   * with A=0. Conv: the $C0D9 ADD A,(HL) with A=0 is a no-op (allow_spawning
   * stays 0); omitted. */
  if (*HLfast_counter >= (u8)Aspeed_lo) { /* JP NC: no overflow */
    check_hazard_collisions(state);
    return;
  }

  /* carry: cycle the buffer a second time this frame */
  rm_cycle_buffer_offset(state, HLfast_counter);
}

/**
 * $BE1F: Cycle road buffer by one slot and decode all map data channels.
 *
 * Advances roadbufptr by one position in the cyclic road buffer, then decodes
 * six data channels into the new slot: curvature ($BE3A), height ($BEC4), lanes
 * ($BEF2), right-side objects ($BF55), left-side objects ($BF9E), and hazards
 * ($BFE7). Each channel is run-length encoded as $CT bytes where the high
 * nibble is a countdown counter and the low nibble carries the type. When the
 * counter expires (i.e. subtracting 16 produces carry), a new map byte is
 * loaded; a zero escape byte introduces a command — goto (0), fork-end (1), or
 * fork-split (2). After channel decoding the road level is advanced, all six
 * hazard slots are ticked ($C01D), and if the dirt/stones copy-back flag is set
 * ($C0BE) the xpos fork buffer is shifted down by one entry. Exits via
 * check_hazard_collisions.
 *
 * \param[in] state Pointer to game state.
 * \param[in] HLfast_counter Pointer to the fast_counter field; the Z80 used
 * HL+1 (= &road_buffer_offset) to advance the buffer. Conv: unused in C —
 * roadbufptr is advanced directly via ROADBUF_FWD2PTR. (was HL)
 */
static void rm_cycle_buffer_offset(chqstate_t *state, u8 *HLfast_counter)
{
  u8        *HL_rightside_ptr;   /* road buffer right-side objects slot (was HL) */
  u8        *HL_leftside_ptr;    /* road buffer left-side objects slot (was HL) */

  u8        *HL_curve_ptr;       /* road buffer curvature slot (was HL) */
  u8         A_curve_byte;       /* raw curvature byte from map; format $CT (was A) */
  const u8  *DE_curve_ptr;       /* road curvature map read pointer (was DE) */
  int        A_curvature;        /* decoded curvature value written to road buffer (was A) */

  u8        *HL_height_ptr;      /* road buffer height slot (was HL) */
  u8         A_height_byte;      /* raw height byte from map; format $CT (was A) */
  const u8  *DE_height_ptr;      /* road height map read pointer (was DE) */

  u8        *HL_lanes_ptr;       /* road buffer lanes slot (was HL) */
  u8         A_lanes_counter;    /* lanes countdown; reloads at 0xFF (was A) */
  const u8  *DE_lanes_ptr;       /* road lanes map read pointer (was DE) */
  u8         A_lanes_byte;       /* lanes command/type byte from map (was A) */
  int        C_lanes_byte;       /* current lanes byte for count-resume path (was C) */

  u8        *HL_hazards_ptr;     /* road buffer hazards slot (was HL) */
  u8         A_rightside_byte;   /* raw right-side objects byte from map; format $CT (was A) */
  const u8  *DE_rightside_ptr;   /* right-side objects map read pointer (was DE) */
  u8         A_leftside_byte;    /* raw left-side objects byte from map; format $CT (was A) */
  const u8  *DE_leftside_ptr;    /* left-side objects map read pointer (was DE) */
  u8         A_hazards_counter;  /* hazards countdown; reloads at 0xFF (was A) */
  const u8  *DE_hazards_ptr;     /* hazards map read pointer (was DE) */
  u8         A_hazards_byte;     /* hazards command byte from map (was A) */

  hazard_t  *IX_hazard;          /* pointer to current hazard slot being updated (was IX) */
  int        C_overtake_bonus;   /* count of scored overtakes this cycle (was C) */
  int        B_iterations;       /* countdown over 6 hazard slots (was B) */
  u8         old_used;           /* hazard[0].used before RLC (was A) */
  int        A_flags_inc;        /* hazard_flags + 1 for signed/zero check (was A) */

  u8        *copy_base;          /* base of xpos_road_fork_right for copy-back */
  u8        *HL_src;             /* copy-back loop source pointer (was HL) */
  u8        *DE_dst;             /* copy-back loop destination pointer (was DE) */
  int        BC_count;           /* copy-back loop byte count (was BC) */

  NOT_USED(HLfast_counter);

  /* Advance roadbufptr */
  state->roadbufptr = ROADBUF_FWD2PTR(1);

  /* Set sound effect triggers (before we clobber those bytes) */
  /* We fetch and store these flags separately but the sole user drive_sfx
   * ultimately just merges them together. */
  /* offset 96 -> first byte of right hand objects */
  HL_rightside_ptr = ROADBUF_FWD2PTR(95);
  state->trigger_righthand_sfx |= *HL_rightside_ptr;
  /* offset 128 -> first byte of left hand objects */
  HL_leftside_ptr = &state->roadbuf_start[ROADBUF_PTR2IDX(HL_rightside_ptr + 32)];
  state->trigger_lefthand_sfx |= *HL_leftside_ptr;

  /* ---------------- *
   * $BE3A: CURVATURE *
   * ---------------- */

  /* We point at the FINAL byte of curvature data here. */
  HL_curve_ptr = &state->roadbuf_start[ROADBUF_PTR2IDX(HL_leftside_ptr - 96)];

  /* Format: $CT where [C]ounter; Curve [T]ype */
  A_curve_byte = state->curvature_byte - 16;
  /* If it carries we need to load a new curvature byte */
  if (A_curve_byte >= 240) {
    DE_curve_ptr = state->scenedata.road_curvature_ptr + 1;
    A_curve_byte = *DE_curve_ptr;
    if (A_curve_byte == 0) {
      // Escape byte (0): read command byte.

      // Conv: EX DE,HL register swap was folded in from $BE4E here to $BE75 below

      DE_curve_ptr++;
      A_curve_byte = *DE_curve_ptr++;
      if (A_curve_byte != MAP_CMDCODE_GOTO) { /* not 0 */
        if (A_curve_byte != MAP_CMDCODE_FORK_END) { /* not 1 */
          // $BE58 - Split command (2)
          state->rm_leftfork_curve  = wordat(DE_curve_ptr + 0);
          state->rm_rightfork_curve = wordat(DE_curve_ptr + 2);
          DE_curve_ptr = &forked_road_curvature[0];
        } else {
          // $BE6C - Fork end command (1)
          DE_curve_ptr = state->rm_curvature_fork_end_ptr;
        }
      } else {
        // $BE71 - Goto command (0)
        DE_curve_ptr = lookup_map_goto(state->current_stage_number, wordat(DE_curve_ptr));
      }

      // $BE75
      A_curve_byte = *DE_curve_ptr;
    }

    // $BE77
    state->scenedata.road_curvature_ptr = DE_curve_ptr;
    A_curve_byte -= 16;
  }

  // $BE7D - save curvature byte
  state->curvature_byte = A_curve_byte;
  A_curvature = A_curve_byte & 0x0F;
  if (A_curvature & 8)
    A_curvature = -(A_curvature & 7); // was NEG
  *HL_curve_ptr = A_curvature << 1;

  /* ------------- *
   * $BE90: HEIGHT *
   * ------------- */

  HL_height_ptr = &state->roadbuf_start[ROADBUF_PTR2IDX(HL_curve_ptr + 32)];

  /* Format: $CT where [C]ounter; Height [T]ype */
  A_height_byte = state->height_byte - 16;
  /* If it carries we need to load a new height byte */
  if (A_height_byte >= 240) {
    DE_height_ptr = state->scenedata.road_height_ptr + 1;
    A_height_byte = *DE_height_ptr;
    if (A_height_byte == 0) {
      // Escape byte (0): read command byte.

      // Conv: EX DE,HL register swap was folded in from $BE4E here to $BE75 below

      DE_height_ptr++;
      A_height_byte = *DE_height_ptr++;
      if (A_height_byte != MAP_CMDCODE_GOTO) { /* not 0 */
        if (A_height_byte != MAP_CMDCODE_FORK_END) { /* not 1 */
          // $BEAA - Split command (2)
          state->rm_leftfork_height  = wordat(DE_height_ptr + 0);
          state->rm_rightfork_height = wordat(DE_height_ptr + 2);
          DE_height_ptr = &forked_road_height[0];
        } else {
          // $BEBE - Fork end command (1)
          DE_height_ptr = state->rm_height_fork_end_ptr;
        }
      } else {
        // $BEC3 - Goto command (0)
        DE_height_ptr = lookup_map_goto(state->current_stage_number, wordat(DE_height_ptr));
      }

      // $BEC7
      A_height_byte = *DE_height_ptr;
    }

    // $BEC9
    state->scenedata.road_height_ptr = DE_height_ptr;
    A_height_byte -= 16;
  }

  // $BECF - save height byte
  state->height_byte = A_height_byte;
  *HL_height_ptr = (A_height_byte & 0x0F) - 8;

  /* ------------ *
   * $BEDB: LANES *
   * ------------ */

  HL_lanes_ptr = state->roadbuf_start + ROADBUF_PTR2IDX(HL_height_ptr + 32);

  /* Format: $CC $TT where [CC]ounter; Lane [TT]ype */
  A_lanes_counter = state->lanes_counter - 1;
  /* If it runs out we need to load a new lanes byte */
  if (A_lanes_counter == 0xFF) {
    DE_lanes_ptr = state->scenedata.road_lanes_ptr + 1;
    A_lanes_counter = *DE_lanes_ptr;
    if (A_lanes_counter == 0) {
      // Escape byte (0): read command byte.

      // Conv: EX DE,HL register swap was folded in from $BEEB here to $BF12 below

      DE_lanes_ptr++;
      A_lanes_byte = *DE_lanes_ptr++;
      switch (A_lanes_byte) {
      case MAP_CMDCODE_GOTO:
        // $BF0E - Goto command (0)
        DE_lanes_ptr = lookup_map_goto(state->current_stage_number, wordat(DE_lanes_ptr));
        break;
      case MAP_CMDCODE_FORK_END:
        // $BF09 - Fork end command (1)
        DE_lanes_ptr = state->rm_lanes_fork_end_ptr;
        break;
      default:
        // $BEF5 - Split command (2)
        state->rm_leftfork_lanes  = wordat(DE_lanes_ptr + 0);
        state->rm_rightfork_lanes = wordat(DE_lanes_ptr + 2);
        DE_lanes_ptr = &forked_road_lanes[0];
        break;
      }

      // $BF12-$BF13: the reload lands in A_lanes_counter — the Z80 keeps the
      // counter byte in A through $BF13/$BF14. Reading it into A_lanes_byte
      // left the $BF15 store computing 0 - 1 = 255 after any lanes command,
      // stalling the channel for ~256 steps (tunnel walls arrived long after
      // the tunnel lights). Same shape as the hazards-channel fix.
      A_lanes_counter = *DE_lanes_ptr;
    }

    // $BF14 - save lanes bytes
    state->lanes_counter = A_lanes_counter - 1;
    DE_lanes_ptr++;
    state->scenedata.road_lanes_ptr = DE_lanes_ptr;
    *HL_lanes_ptr = *DE_lanes_ptr & 0xF7;
    state->rm_lanes_byte = *DE_lanes_ptr & 0xFB;
  } else {
    // $BF29 - rm_lanes_count_resume
    state->lanes_counter = A_lanes_counter;
    C_lanes_byte = state->rm_lanes_byte;
    if (C_lanes_byte & 0x0C)
      state->rm_lanes_byte = C_lanes_byte & 0xF3;
    *HL_lanes_ptr = C_lanes_byte;
  }

  /* ----------------- *
   * $BF3E: SKIP CHECK *
   * ----------------- */

  // $BF3A - done
  HL_rightside_ptr = state->roadbuf_start + ROADBUF_PTR2IDX(HL_lanes_ptr + 32);

  if (state->session.no_objects_flag != 1) {
    *HL_rightside_ptr = 0; // rightside slot
    HL_leftside_ptr = state->roadbuf_start + ROADBUF_PTR2IDX(HL_rightside_ptr + 32);
    *HL_leftside_ptr = 0; // leftside slot
    HL_hazards_ptr = state->roadbuf_start + ROADBUF_PTR2IDX(HL_leftside_ptr + 32);
    *HL_hazards_ptr = 0; // hazards slot
    state->session.no_objects_flag = 1;
  } else {
    /* ------------------------- *
     * $BF55: RIGHT-SIDE OBJECTS *
     * ------------------------- */

    /* Format: $CT where [C]ounter; Height [T]ype */
    A_rightside_byte = state->rightside_byte - 16;
    /* If it carries we need to load a new right hand byte */
    if (A_rightside_byte >= 240) {
      DE_rightside_ptr = state->scenedata.road_rightside_ptr + 1;
      A_rightside_byte = *DE_rightside_ptr;
      if (A_rightside_byte == 0) {
        // Escape byte (0): read command byte.

        // Conv: EX DE,HL register swap was folded in from $BF65 here to $BF8E below

        DE_rightside_ptr++;
        A_rightside_byte = *DE_rightside_ptr++;
        if (A_rightside_byte != MAP_CMDCODE_GOTO) { /* not 0 */
          if (A_rightside_byte != MAP_CMDCODE_FORK_END) { /* not 1 */
            // $BF6F - Split command (2)
            state->rm_leftfork_rightside  = wordat(DE_rightside_ptr + 0);
            state->rm_rightfork_rightside = wordat(DE_rightside_ptr + 2);
            DE_rightside_ptr = &fork_leftrightobjs[1]; // $E2C1
          } else {
            // $BF83 - Fork end command (1)
            DE_rightside_ptr = state->rm_rightside_fork_end_ptr;
          }
        } else {
          // $BF88 - Goto command (0)
          DE_rightside_ptr = lookup_map_goto(state->current_stage_number, wordat(DE_rightside_ptr));
        }

        // $BF8C
        A_rightside_byte = *DE_rightside_ptr;
      }

      // $BF8E
      state->scenedata.road_rightside_ptr = DE_rightside_ptr;
      A_rightside_byte -= 16;
    }

    // $BF94 - save right side byte
    state->rightside_byte = A_rightside_byte;
    *HL_rightside_ptr = A_rightside_byte & 0x0F;

    /* ------------------------ *
     * $BF9E: LEFT-SIDE OBJECTS *
     * ------------------------ */

    HL_leftside_ptr = state->roadbuf_start + ROADBUF_PTR2IDX(HL_rightside_ptr + 32);

    /* Format: $CT where [C]ounter; Height [T]ype */
    A_leftside_byte = state->leftside_byte - 16;
    /* If it carries we need to load a new left hand byte */
    if (A_leftside_byte >= 240) {
      DE_leftside_ptr = state->scenedata.road_leftside_ptr + 1;
      A_leftside_byte = *DE_leftside_ptr;
      if (A_leftside_byte == 0) {
        // Escape byte (0): read command byte.

        // Conv: EX DE,HL register swap was folded in from $BFAB here to $BFD5 below

        DE_leftside_ptr++;
        A_leftside_byte = *DE_leftside_ptr++;
        if (A_leftside_byte != MAP_CMDCODE_GOTO) { /* not 0 */
          if (A_leftside_byte != MAP_CMDCODE_FORK_END) { /* not 1 */
            // $BFB8 - Split command (2)
            state->rm_leftfork_leftside  = wordat(DE_leftside_ptr + 0);
            state->rm_rightfork_leftside = wordat(DE_leftside_ptr + 2);
            DE_leftside_ptr = &fork_leftrightobjs[0]; // $E2C0
          } else {
            // $BFCC - Fork end command (1)
            DE_leftside_ptr = state->rm_leftside_fork_end_ptr;
          }
        } else {
          // $BFD1 - Goto command (0)
          DE_leftside_ptr = lookup_map_goto(state->current_stage_number, wordat(DE_leftside_ptr));
        }

        // $BFD5
        A_leftside_byte = *DE_leftside_ptr;
      }

      state->scenedata.road_leftside_ptr = DE_leftside_ptr;
      A_leftside_byte -= 16;
    }

    // $BFDD - save left side byte
    state->leftside_byte = A_leftside_byte;
    *HL_leftside_ptr = A_leftside_byte & 0x0F;

    /* -------------- *
     * $BFE7: HAZARDS *
     * -------------- */

    HL_hazards_ptr = state->roadbuf_start + ROADBUF_PTR2IDX(HL_leftside_ptr + 32);

    /* Format: single $CC counter bytes only — the written hazard value is
     * the self-modified rm_hazard_byte ($C058), set by commands 3..9. */
    A_hazards_counter = state->hazards_counter - 1;
    /* If it runs out we need to load a new hazards byte */
    if (A_hazards_counter == 0xFF) {
      DE_hazards_ptr = state->scenedata.road_hazard_ptr + 1;

rm_restart_hazards_read: // $BFF3
      A_hazards_counter = *DE_hazards_ptr;
      if (A_hazards_counter == 0) {
        // Escape byte (0): read command byte.

        // Conv: EX DE,HL register swap was folded in from $BFF7 here to $C04E below

        DE_hazards_ptr++;
        A_hazards_byte = *DE_hazards_ptr++;
        if (A_hazards_byte != MAP_CMDCODE_GOTO) {
          /* cmd was NOT 0 */
          A_hazards_byte--;
          if (A_hazards_byte != 0) {
            /* cmd was NOT 1 either */
            A_hazards_byte--;
            if (A_hazards_byte != 0) {
              /* cmd was NOT 2 either */
              A_hazards_byte--;
              if (A_hazards_byte >= 7) {
                /* cmd 10+ */
                if (A_hazards_byte >= 10) {
                  /* cmd 13+ */
                  if (A_hazards_byte >= 12)
                    /* cmd 15+: helicopter control = cmd - 14 */
                    state->helicopter_control = A_hazards_byte - 11;
                  else
                    /* $C018: cmd 13/14 → dont_spawn_cars = cmd - 13 */
                    state->dont_spawn_cars = A_hazards_byte - 10;
                  // $C016/$C01D
                  // DE_hazards_ptr ↔ HL_lanes_ptr; // was EX DE,HL (folded away)
                  goto rm_restart_hazards_read;
                } else {
                  // $C01F: cmd 10/11/12 → floating_arrow = cmd - 10
                  A_hazards_byte -= 7; // floating_arrow = A - 7 (from A = cmd-3)
                  state->floating_arrow = A_hazards_byte;
                  if (A_hazards_byte != 0)
                    state->correct_fork = A_hazards_byte;
                  // $C029
                  // DE_hazards_ptr ↔ HL_lanes_ptr; // was EX DE,HL (folded away)
                  goto rm_restart_hazards_read;
                }
              } else {
                // $C02C: cmd 3..9 → rm_hazard_byte = cmd - 3
                state->rm_hazard_byte = A_hazards_byte;
              }
            } else {
              // $C031 - Split command (2)
              state->rm_leftfork_hazards  = wordat(DE_hazards_ptr + 0);
              state->rm_rightfork_hazards = wordat(DE_hazards_ptr + 2);
              DE_hazards_ptr = &fork_hazards[0];
            }
          } else {
            // $C045 - Fork end command (1)
            DE_hazards_ptr = state->rm_hazards_fork_end_ptr;
          }
        } else {
          // $C04A - Goto command (0)
          DE_hazards_ptr = lookup_map_goto(state->current_stage_number, wordat(DE_hazards_ptr));
        }

        // $C04E: pointer already set up; byte is always regular here
        // DE_hazards_ptr ↔ HL_lanes_ptr; // was EX DE,HL (folded away)
        // Conv: the reload lands in A_hazards_counter — the Z80 keeps the
        // regular data byte in A through $C04F/$C054/$C055. Reading it into
        // A_hazards_byte left the $C055 store holding the escape byte (0),
        // swallowing the first WAIT after any command (the fork arrow
        // vanished ~2 steps after the SPLIT instead of after WAIT(12)).
        A_hazards_counter = *DE_hazards_ptr;
      }

      // $C050
      state->scenedata.road_hazard_ptr = DE_hazards_ptr;
      A_hazards_counter--;
    }

    // $C055
    state->hazards_counter = A_hazards_counter;
    *HL_hazards_ptr = state->rm_hazard_byte;
    state->session.no_objects_flag = 2;
  }

  // $C05C (also entered from skip path with no_objects_flag=1)
  update_road_level(state);

  // Hazard update loop ($C062): iterate over all 6 hazard slots.
  IX_hazard = &state->hazards[0];
  C_overtake_bonus = 0;
  for (B_iterations = 6; B_iterations > 0; B_iterations--, IX_hazard++) {
    old_used = IX_hazard->used;
    IX_hazard->used = (u8)((old_used << 1) | (old_used >> 7)); // RLC
    if (!(old_used & 0x80)) // bit 7 was clear → skip
      continue;

    // rm_c080: bit 7 was set (hazard is active)
    A_flags_inc = (u8) (IX_hazard->hazard_flags + 1);
    /* Conv: truncate to 8 bits so 0xFF wraps to 0, matching Z80 INC A */
    if (A_flags_inc == 0) { // hazard_flags was 0xFF: rm_c096
      A_flags_inc = IX_hazard->distance; // value before the subtract, for the borrow test
      // SUB $01 always executes and always stores, wrapping 0 -> 255 (Conv:
      // the u8 field assignment performs the wraparound).
      IX_hazard->distance = (u8) (A_flags_inc - 1);
      if (A_flags_inc == 0) { // rm_c0b2: distance was already 0 -> borrowed
        IX_hazard->hazard_lane_OR_perp_dist_hi--;
        continue;
      }
      if (IX_hazard->distance != 0) // non-zero result: keep going
        continue;
      // distance just hit 0 (no borrow)
      if (IX_hazard->hazard_lane_OR_perp_dist_hi != 0)
        continue;
      IX_hazard->distance = 1;
      IX_hazard->dist_frac = 0xFF;
      continue;
    }

    // hazard_flags != 0xFF: decrement distance
    if (--IX_hazard->distance == 0) {
      IX_hazard->used = 0; // mark unused
      if ((u8)(IX_hazard->hazard_flags + 1) & 0x80) // RLA carry: hazard_flags >= 0x7F
        C_overtake_bonus++;
    }
  }
  state->overtake_bonus_counter = C_overtake_bonus;

  // $C0BB: copy block if rm_scroll_dirt_particles is set
  if (state->rm_scroll_dirt_particles) {
    copy_base = (u8 *)state->xpos_road_fork_right;
    HL_src = copy_base + 0x73;
    DE_dst = copy_base + 0x77;
    BC_count = 0x26;
    do {
      HL_src -= 2;
      DE_dst -= 2;
      *DE_dst-- = *HL_src--; BC_count--;
      *DE_dst-- = *HL_src--; BC_count--;
    } while (BC_count != 0);
    HL_src++;
    *HL_src = 0;
  }

  // rm_allow_car_spawning ($C0D7)
  state->allow_spawning++;

  check_hazard_collisions(state); /* tail call */
}

/**
 * $C0E1: Prepare tunnel rendering for the current frame.
 *
 * Three mutually exclusive paths:
 *
 * 1. Not yet in a tunnel (dt_tunnel_visible == 0, dr_in_tunnel == 0): clears
 * tunnel_sfx, NOPs out the draw_tunnel CALL hooks, returns.
 *
 * 2. In tunnel but tunnel not yet marked visible (dt_tunnel_visible == 0,
 * dr_in_tunnel != 0): scans adjacent pairs of xpos_road_centre entries working
 * inward from near-perspective rows, counting how many steps before the values
 * converge. Stores that count as dt_tunnel_distance, sets dt_tunnel_visible = 2
 * and falls through to arm the hooks.
 *
 * 3. Tunnel already visible (dt_tunnel_visible != 0): skips the scan and falls
 * straight through to arm the hooks.
 *
 * Sets the draw_tunnel hooks by patching dee_draw_tunnel_1 / dee_draw_tunnel_2
 * with Z80_CALL_NN, and writes dt_far_wall_mode = dr_in_tunnel ^ 1.
 *
 * \param[in] state Pointer to game state.
 */
static void prepare_tunnel(chqstate_t *state)
{
  int   Atunnel_visible; /* dt_tunnel_visible: 0 until tunnel appears (was A at $C0E1) */
  u8    Ain_tunnel;      /* dr_in_tunnel value; doubles as loop counter (was A at $C0E5) */
  s16  *HLmain;          /* pointer into xpos_road_centre, main-register stream (was HL) */
  int   BCmain;          /* previous entry from main stream; updated each iteration (was BC) */
  s16  *HLdash;          /* pointer into xpos_road_centre, shadow-register stream (was HL') */
  int   DEdash;          /* previous entry from shadow stream (was DE') */
  int   carry;           /* carry out of SBC comparisons */
  int   DEmain;          /* current entry from main stream (was DE) */
  int   BCdash;          /* current entry from shadow stream (was BC') */

  /* AND A preserves Z flag through next LD */
  Atunnel_visible = state->dt_tunnel_visible;
  /* $C0E5: read dr_in_tunnel into A without disturbing Z flag */
  Ain_tunnel = state->dr_in_tunnel;
  /* skip scan if tunnel already visible */
  if (Atunnel_visible != 0)
    goto pt_arm_hooks;

  /* jump to scan if in tunnel but not yet marked */
  if (Ain_tunnel == 0) {
    /* not in tunnel; NOP out the CALL hooks */
    state->tunnel_sfx = 0;
    state->dee_draw_tunnel_1 = 0;
    state->dee_draw_tunnel_2 = 0;
    return;
  }

  /* $C100: pt_yes_a_tunnel — in tunnel, not yet marked visible; scan xpos table */
  state->tunnel_sfx = 5; /* quietens road-sfx while inside the tunnel */

  /* $C105-$C10D: main-stream initial values at xpos_road_centre[121] and [119].
   * $EAF3/$EAF2 = index 121 (byte pointer at high byte of s16 pair). */
  HLmain = &state->xpos_road_centre[0xF2 / 2]; /* index 121 */
  BCmain = *HLmain;
  HLmain -= 2; /* HL -= 4 bytes = 2 s16 entries → index 119 */

  /* $C10E EXX Bank; $C10F-$C117: shadow-stream initial values at [120] and [118] */
  HLdash = &state->xpos_road_centre[0xF0 / 2]; /* index 120 */
  DEdash = *HLdash;
  HLdash -= 2; /* index 118 */

  /* $C118: pt_tunnel_loop.
   * Main stream reads at indices 119, 117, 115 … (step −2), breaks when
   * current < previous (SBC HL,BC carry: narrowing from one side).
   * Shadow stream reads at indices 118, 116, 114 … (step −2), breaks when
   * previous < current (SBC HL,BC carry: narrowing from the other side).
   * Conv: EX DE,HL pairs at $C120/$C126 and $C12E/$C133 are folded away;
   *       PUSH DE / POP BC at $C11C/$C123 collapsed to BCmain = DEmain. */
  do {
    /* $C118 EXX Unbank; $C119-$C11F: read next main-stream entry */
    DEmain  = *HLmain;
    HLmain -= 2;
    carry   = (DEmain < BCmain);
    BCmain  = DEmain; /* old DE → BC (was PUSH DE / POP BC) */
    if (carry)
      break;

    /* $C127 EXX Bank; $C128-$C12D: read next shadow-stream entry */
    BCdash  = *HLdash;
    HLdash -= 2;
    /* carry=0 from above: prev < current → converging */
    if (DEdash < BCdash)
      break;
    DEdash = BCdash;
  } while (--Ain_tunnel);

  /* store distance and mark visible */
  state->dt_tunnel_distance = 9 - Ain_tunnel;
  Ain_tunnel = 2;
  state->dt_tunnel_visible = Ain_tunnel; /* $C141 self-modify $C160 */

pt_arm_hooks:
  /* $C144-$C146: dt_far_wall_mode = dr_in_tunnel ^ 1
   * (= 3 on first appearance with Ain_tunnel=2; dr_in_tunnel^1 when already visible) */
  state->dt_far_wall_mode = Ain_tunnel ^ 1;

  /* $C149-$C157: patch $8F82/$8FA7 with Z80_CALL_NN to arm draw_tunnel hooks */
  state->dee_draw_tunnel_1 = state->dee_draw_tunnel_2 = Z80_CALL_NN;
}

/**
 * $C15B: Draw tunnel
 *
 * Renders the tunnel entrance, interior, and far wall into the back buffer for
 * the current road scanline. Called via a self-modified CALL instruction
 * patched into the main render loop at $8F82 and $8FA7 when the tunnel is
 * active.
 *
 * Three fill phases are drawn back-to-front using the PUSH-table mechanism (the
 * Z80 uses SP for speed; modelled here with a fall-through switch): 1. Main
 * body: left-edge fill (dt_fill_start_a) + right fill (dt_fill_start_b) 2.
 * Transition rows: right fill only (C = $0F masks every row) 3. Far wall: right
 * fill only (dt_far_wall_mode controls depth)
 *
 * The horizontal fill extents are derived from the road centre-right and left
 * x-position tables, giving the left and right tunnel wall column positions.
 *
 * \param[in] state Pointer to game state.
 * \param[in] IYheight Pointer to current row's entry in the height table.
 *   (was IY)
 */
static void draw_tunnel(chqstate_t *state, u8 *IYheight)
{
  int       carry = 0;
  int       Adistance; /* row index of this call; compared to SM trigger (was A) */
  int       Avisible;  /* tunnel-visible countdown; 1 = entrance frame (was A) */
  u8        Dfill;     /* fill byte: $EE for entrance stripes, $FF for interior; rotated per row (was D) */
  u8        L;         /* low byte of HL: xpos byte-offset then backbuf low byte (was L) */
  u8        C;         /* fill width correction derived from road edge positions (was C) */
  s16      *HL;        /* pointer into xpos table during edge calculation (was HL) */
  u8        A;         /* Z80 accumulator; reused for multiple transient values (was A) */
  u8        D;         /* left fill boundary 0..22 (jump-table index), then fill byte (was D) */
  int       E;         /* right fill boundary 0..16 (jump-table index), then fill byte (was E) */
  u8        B;         /* road row data, then fill loop iteration count (was B) */
  int       H;         /* high byte of back-buffer row address ($F0..$FF) (was H) */
  const u8 *DE;        /* pointer into persp_y_scale table (was DE) */
  u16       DEfill;    /* 16-bit fill word: fill byte repeated (was DE) */
  u8       *HLbackbuf; /* back-buffer row pointer for PUSH-based fill (was HL/SP) */
  u8       *SPoutput;  /* per-scanline write pointer; simulates Z80 SP (was SP) */
  int       Adash;     /* A' = running row sum, banked via EX AF,AF' at $C1E5/$C21A/$C27B (was A') */
  u8        Bdash;     /* B' = 16 - IYl + E, banked via EXX at $C208 (was B') */
  int       n_a;       /* PUSH count for first fill: (16 - dt_fill_start_a) (Conv: added) */
  int       n_b;       /* PUSH count for second fill: (16 - dt_fill_start_b) (Conv: added) */

  /* Conv: dt_max_fill ($C188) and the $C1A4 JP M,$C1BC path leave E and B
   * unset in the Z80 too — both are genuine stale-register reads there.
   * For E, every path into dt_max_fill also sets dt_fill_start_a == 22,
   * which makes dt_start_fill's consumer skip dt_fill_start_b entirely, so
   * the stale value is never actually read. For B, dt_compute_fill_bounds
   * does use it (to derive C, the fill-width correction) — that path reads
   * whatever B held on entry to draw_tunnel, and we haven't traced callers
   * to know what that is. Zero-initialised here to avoid C-level UB; the B
   * case may not match original hardware behaviour. */
  E = 0;
  B = 0;

  Adistance = IYheight - &state->height_table[0];
  if (Adistance != state->dt_tunnel_distance)
    return;

  Avisible = state->dt_tunnel_visible;
  Dfill = 0xEE;
  if (--Avisible)
    Dfill = 0xFF;

  state->dt_fill_pattern = Dfill * 0x0101; /* widen to $EEEE or $FFFF */

  // Conv: Removed SP store ($C16E LD ($C2E4),SP)

  L = ~((IYheight[0x4E] - 2) << 1);
  C = 0;
  HL = &state->xpos_road_centre_right[L / 2];
  A = (*HL >> 8) & 0xFF; /* Conv: LD A,(HL) with L odd reads the high byte of the little-endian s16 */
  if (A == 0)
    goto dt_centre_right_at_zero;

  D = 16;
  if ((s8) A >= 0)
    goto dt_check_left;

dt_max_fill: /* $C188: xpos is wide/positive — use maximum fill extents */
  D = 22;
  L = 31;
  goto dt_start_fill;

dt_centre_right_at_zero:
  /* Conv: Z80 DEC L; LD A,(HL); INC L. L is always odd so L/2 == (L-1)/2
   * (truncation), making HL[0] the same element as (HL) after DEC L. */
  A = (u8)(*HL & 0xFF);
  A = (A & 0xF8) >> 3;
  A >>= 1;
  RR(C);
  A++;
  D = A;

dt_check_left: /* $C19C: switch to left xpos table */
  HL = &state->xpos_road_left[L / 2];
  A = (*HL >> 8) & 0xFF; /* Conv: LD A,(HL) with L odd reads the high byte of the little-endian s16 */
  if (A == 0)
    goto dt_left_at_zero;

  E = 16;
  if ((s8) A < 0)
    goto dt_compute_fill_bounds;

  goto dt_max_fill;

dt_left_at_zero: /* $C1AA: left xpos sentinel is zero; read byte before and add 8 */
  /* Conv: Z80 DEC L then LD A,(HL). As above, L is odd so DEC L gives the same
   * s16 element — HL does not need to move. Carry from ADD A,$08 was missing. */
  L--;
  { int Atmp = (*HL & 0xFF) + 8; carry = Atmp > 0xFF; A = (u8) Atmp; }
  if (carry)
    goto dt_max_fill;
  A = (A & 0xF8) >> 3;

  B = A;
  E = 16 - (A >> 1);

dt_compute_fill_bounds: /* $C1BC: compute left fill width and correction C from B */
  A = 32 - B;
  L = 32;
  RL(C);
  if (carry)
    goto dt_set_fill_c;

  L--;
  A--;

dt_set_fill_c:
  C = A;

dt_start_fill: /* $C1C8: store fill boundaries; compute starting back-buffer address */
  state->dt_fill_start_a = D;
  state->dt_fill_start_b = E;

  A = IYheight[0x35];
  B = A;
  H = (A & 15) + 0xF0;
  /* Conv: Z80 ADD A,A then ADD A,L can overflow — capture the carry. */
  { int Atmp = ((B & 0x70) << 1) + L; carry = Atmp > 0xFF; A = (u8) Atmp; }
  if (carry)
    A--;

  L = A;
  Adash = 128 - B;
  // EX AF,AF' — banks Adash = 128-B to A'
  E = state->fast_counter & 0xE0;
  /* Conv: Z80 $C1E6 LD A,(fast_counter) overwrites A immediately after EX AF,AF',
   * so COUNTER_TO_PERSP_Y_ROW operates on E (fast_counter & $E0), not on 128-B. */
  A = E - (E >> 2) - (E >> 4); /* map (0,32,64,...,224) to (0,22,44,...,154) */
  A += (IYheight - &state->height_table[0]); /* was IYl */
  DE = &persp_y_scale[A / PERSP_TABLE_COLS][A % PERSP_TABLE_COLS];
  B = *IYheight - B;
  A = *DE;
  E = A;
  A = (16 - (IYheight - &state->height_table[0])) + E; /* was IYl */
  // EXX - bank ($C208)
  Bdash = A; /* $C209 LD B,A runs in the shadow bank: A → B' */
  // EXX - unbank ($C20A); B retains pre-EXX value (*IYheight - B_step35)
  A -= B;
  D = A;
  // EX AF,AF' — restores Adash = 128-B to A
  A = Adash;
  A += D;
  if ((s8) A >= 0)
    goto dt_clamp_rows;

  E = A;
  D -= A - 0x81;
  A = E;

dt_clamp_rows:
  // EX AF,AF' — bank the running row sum
  Adash = A; /* A → A'; retrieved at $C26A */
  B = D;

  DEfill = state->dt_fill_pattern;
  /* Conv: Z80 LD SP,HL at $C21F — init HLbackbuf from the H:L pair computed above.
   * Was missing; caused use of uninitialised pointer on first SPoutput = HLbackbuf. */
  HLbackbuf = ADDRTOBACKBUF((H << 8) | L);
  do {
    SPoutput = HLbackbuf;
    /* Conv: read the live pointer low byte — the tracked L
     * variable misses prev_buf_row's row-boundary adjustments. */
    A = BACKBUFTOADDR(HLbackbuf) & 0xFF;
    if (state->dt_fill_start_a == 22) {
      /* Conv: $C221 JR with offset 22 lands at $C239, midway into the
       * *second* PUSH chain: 15 PUSHes from the original SP, skipping the
       * $C233 SUB C and second LD SP,HL. One combined 30-byte fill. */
      SPoutput -= 15 * 2;
      memset(SPoutput, (u8)DEfill, 15 * 2);
    } else {
      /* Conv: Z80 JR jump table indexed 0-16 where entry N executes (16-N)
       * PUSHes backward from SP. C computes the count and uses memset. */
      n_a = 16 - state->dt_fill_start_a;
      SPoutput -= n_a * 2;
      memset(SPoutput, (u8)DEfill, (size_t)(n_a * 2));
      A -= C;
      /* Conv: $C235 LD SP,HL with updated (L - C). H comes from the current
       * HLbackbuf row (Z80 DEC H advances H each iteration via the loop
       * header), not the frozen initial H variable. */
      SPoutput = ADDRTOBACKBUF((BACKBUFTOADDR(HLbackbuf) & 0xFF00) | A);
      /* Conv: Z80 second fill JR table indexed 0-16; entry 16 → 0 PUSHes. */
      n_b = (state->dt_fill_start_b <= 15) ? (16 - state->dt_fill_start_b) : 0;
      SPoutput -= n_b * 2;
      memset(SPoutput, (u8)DEfill, (size_t)(n_b * 2));
    }
    A += C;
    /* In the normal flow A is back to the original L, so this
     * is a no-op; the combined-fill path skipped SUB C, so L becomes L + C. */
    HLbackbuf = ADDRTOBACKBUF((BACKBUFTOADDR(HLbackbuf) & 0xFF00) | A);
    /* Conv: prev_buf_row models $C24B DEC H plus row-boundary L adjustment. */
    HLbackbuf = ADDRTOBACKBUF(prev_buf_row(BACKBUFTOADDR(HLbackbuf)));
    /* Rotate the fill word for the next row.
     * Conv: the fill lives in Dfill/DEfill here, not the boundary index D. */
    RLC(Dfill);
    DEfill = Dfill * 0x0101;
  } while (--B > 0);

  // EXX - unbank ($C260): shadow B (16 - IYl + E) banked at $C208
  B = (Bdash >> 1) - (Bdash >> 3);
  // EX AF,AF' — restores the row sum banked at $C21A
  A = Adash;
  if ((s8) A < 0)
    goto dt_exit;

  A += B;
  if ((s8) A >= 0)
    goto dt_second_phase;

  E = A;
  A -= 0x81;
  A = -A;
  A += B;
  B = A;
  A = E;

dt_second_phase:
  // EX AF,AF' — bank the updated row sum
  Adash = A; /* A → A'; retrieved at $C2AC */
  A = B;
  // EXX
  B = A;
  /* Conv: the live L is HLbackbuf's low byte; the tracked L
   * variable is stale (it missed prev_buf_row's row-boundary adjustments). */
  A = BACKBUFTOADDR(HLbackbuf) & 0xFF;
  A &= 0x0F;
  if (A == 0) {
    /* Step the fill pointer back one byte on 16-row alignment.
     * Conv: applied to HLbackbuf's low byte only, as the low-byte step cannot carry into the high byte. */
    L = (BACKBUFTOADDR(HLbackbuf) - 1) & 0xFF;
    HLbackbuf = ADDRTOBACKBUF((BACKBUFTOADDR(HLbackbuf) & 0xFF00) | L);
  }

  /* $C285: dt_second_loop */
  C = 0x0F;
  do {
    SPoutput = HLbackbuf;
    /* Conv: no jump table here — the Z80 executes 15 unconditional PUSHes
     * ($C288-$C296), a full-width 30-byte fill every row. */
    SPoutput -= 15 * 2;
    memset(SPoutput, (u8)DEfill, 15 * 2);
    HLbackbuf = ADDRTOBACKBUF(prev_buf_row(BACKBUFTOADDR(HLbackbuf)));
    /* rotate the fill word for the next row */
    RLC(Dfill);
    DEfill = Dfill * 0x0101;
  } while (--B > 0);

  // EX AF,AF' — restores the row sum banked at $C27B
  A = Adash;
  if ((s8) A < 0)
    goto dt_exit;

  B = ~A + 0x82;
  /* far-wall fill word (not the persp_y_scale pointer DE) */
  DEfill = 0x0000;
  A = state->dt_far_wall_mode;
  if (A == 0)
    goto dt_far_wall_loop;
  A--;
  if (A)
    goto dt_exit;
  DEfill = 0xFFFF;
dt_far_wall_loop:
  do {
    SPoutput = HLbackbuf;
    /* Conv: no jump table — 15 unconditional PUSHes ($C2C2-$C2D0), a
     * full-width 30-byte fill every row. */
    SPoutput -= 15 * 2;
    memset(SPoutput, (u8)DEfill, 15 * 2);
    HLbackbuf = ADDRTOBACKBUF(prev_buf_row(BACKBUFTOADDR(HLbackbuf)));
  } while (--B > 0);

dt_exit:
  ; // Conv: SP restore removed ($C2E3 LD SP,$0000)
}

/**
 * Return pointer to the start of the 256-byte Z80 road-position page
 * ($E8..$ED).
 */
static s16 *hi_to_xpostab(chqstate_t *state, int hi)
{
  switch (hi) {
  case 0xE8: return state->xpos_road_left;
  case 0xE9: return state->xpos_road_centre_left;
  case 0xEA: return state->xpos_road_centre;
  case 0xEB: return state->xpos_road_centre_right;
  case 0xEC: return state->xpos_road_right;
  case 0xED: return state->xpos_road_fork_right;
  default:   assert(0); return NULL;
  }
}

/** Return byte pointer into a road-position table given a Z80 address. */
static u8 *addr_to_xpos(chqstate_t *state, int H, int L)
{
  return (u8 *)hi_to_xpostab(state, H) + L;
}

/**
 * $C2E7: Draw road lane change
 *
 * Fills road-edge position table entries for a lane-change section (road
 * narrowing or widening). Uses a Bresenham-style algorithm to interpolate
 * road-edge x-positions between two height-table entries and writes them to the
 * appropriate road table via an SP-based write pointer.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     B_fill_pattern Road fill pattern for the current scanline.
 *   (was B)
 * \param[in]     C_horizon Horizon scanline counter. (was C)
 * \param[in]     DE_backbuf Back-buffer row address. (was DE)
 * \param[in]     H_left_hand_table_hi High byte of left x-position table
 *   ($E8..$EC). (was H)
 * \param[in]     L_row Low byte (row index) within the x-position table page.
 *   (was L)
 * \param[in,out] IX_lanesptr Pointer into road buffer lane data. (was IX)
 * \param[in,out] IY_heightptr Pointer into height table. (was IY)
 */
static void draw_road_lanes_change(chqstate_t *state,
                                   int         B_fill_pattern,
                                   int         C_horizon,
                                   int         DE_backbuf,
                                   int         H_left_hand_table_hi,
                                   int         L_row,
                                   u8        **IX_lanesptr,
                                   const u8  **IY_heightptr)
{
  u8   A_dist;              /* byte offset of IYheight within height_table (was A) */
  u8   L_lane_flags;        /* IXlanes[0] (was L) */
  u8   Adash_masked_lane_flags; /* lane_flags & 0x0C: bits 2-3 encode curve type (was A') */
  u8   A_curve_step;        /* 0x20 or 0x00: per-curve step magnitude (was A) */
  u8   C_ref_height;        /* reference height for boundary check (IYheight[1] or [2]) (was C) */
  u8   SM_C345_bend_offset; /* stored A_curve_step: bend component of animation offset (SM $C345) */
  u8   A_bresen_range;      /* Bresenham range (was A) */
  u8   C_bresen_range;      /* Bresenham range = IYheight[0] - ref_height (was C) */
  u8   B_HLzone_stride;     /* table pointer stride = A_curve_step * 2 (was B) */
  u8   L_left_hand_table_lo; /* low byte of x-position table address (was L) */
  u8  *HL_left_hand_table;  /* current pointer into x-position table (was HL) */
  u8  *SP_output;           /* output pointer into road position buffer (was SP) */
  u16  DE_roadpos;          /* current road x-position being written (was DE) */
  u8   A_anim_offset;       /* animation offset: (fast_counter >> 3) & 0x1C + bend_offset (was A) */
  u8   SM_C3BD_bend_offset; /* stored A_curve_step: bend offset for path 2 (SM $C3BD) */
  u16  HL_pos_delta;        /* delta between two road x-positions (was HL) */
  s8   A_pos_delta_lo;      /* low byte of HL_pos_delta, clamped to become A_step (was A) */
  s8   A_step;              /* clamped per-scanline displacement for Bresenham (was A) */
  s8   L_step;              /* copy of A_step used in Bresenham loop (was L) */
  u8   B_range;             /* Bresenham range (was B) */
  u8   A_range;             /* Bresenham range copy for direction comparison (was A) */
  s8   A_direction;         /* Bresenham direction (+1/-1) (was A) */
  u8   A_accum;             /* Bresenham accumulator (was A) */

  // $C2E7
  A_dist = *IY_heightptr - &state->height_table[0];
  assert(A_dist < PERSP_TABLE_COLS);
  if (A_dist < 19) {
    // $C2EE -- bank A_dist
    // EX AF,AF'

    // $C2EF
    L_lane_flags = (*IX_lanesptr)[0];
    Adash_masked_lane_flags = L_lane_flags & 0x0C;
    if (Adash_masked_lane_flags) {
      // Road is changing width. Any of:
      // {4TO3L, 4TO3R, 3TO4L, 3TO4R, 3TO2L, 3TO2R, 2TO3L, 2TO3R,
      // TUNNEL_ENTRY, TUNNEL_EXIT, FORKED}

      // $C2F8
      (*IY_heightptr)--;

      /* $C2FA-$C304: adjust H based on bits 5 and 7 of L_lane_flags. */
      if (L_lane_flags & (1 << 5)) { /* bit 5 SET */
        // {4TO3L, 3TO4L, 2TO3L, FORKED}
        if (L_lane_flags & (1 << 7)) {
          /* bit 7 SET */
          // {4TO3L, 3TO4L, FORKED}
          H_left_hand_table_hi = 0xEC;
        } else
          // {2TO3L}
          H_left_hand_table_hi = 0xEB; // was DEC H
      }

      // $C305
      if (L_lane_flags & (1 << 4)) { /* bit 4 SET */
        // {4TO3L, 3TO4R, 2TO3R, TUNNEL_EXIT}

        // $C30A -- unbank A_dist / bank Adash_curve_bits
        // EX AF,AF'


        if (A_dist < 2) {
          // $C310 -- bank A_dist / unbank Adash_curve_bits
          // EX AF,AF'

          /* $C311: path 1a -- bit4=1, dist<2 */
          if (Adash_masked_lane_flags != 4) {
            // {4TO3L, 3TO4R, 2TO3R, TUNNEL_EXIT}
            A_curve_step = 0x20;
            C_ref_height = (*IY_heightptr)[1];
          } else {
            // {<unused?>}
            A_curve_step = 0x00;
            C_ref_height = (*IY_heightptr)[2];
          }

          // $C31E: C = IY[0] - ref_height (height span)
          SM_C345_bend_offset = A_curve_step;
          A_bresen_range = (*IY_heightptr)[0] - C_ref_height;
          if ((s8) A_bresen_range <= 0)
            goto drlc_continue;

          // $C32B: B = C*2
          C_bresen_range = A_bresen_range;
          B_HLzone_stride = A_bresen_range * 2;
          L_left_hand_table_lo = ~((96 - (*IY_heightptr)[0]) << 1); // byte offset
          HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);

          // $C336
          SP_output = HL_left_hand_table;
          /* L is ~(even) = odd → hi byte of s16 pair; [0]=hi, [-1]=lo */
          DE_roadpos = (HL_left_hand_table[0] << 8) + HL_left_hand_table[-1];

          // $C33A
          L_left_hand_table_lo -= B_HLzone_stride;
          H_left_hand_table_hi--; // widen to left
          HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);

          // $C33D
          A_anim_offset = ((state->fast_counter >> 3) & 0x1C) + SM_C345_bend_offset;

          // $C347-$C350: DE = road_pos - anim  ($C34E: SBC HL,DE; $C350: EX DE,HL)
          DE_roadpos -= A_anim_offset;
        } else {
          /* $C357: bit4=1, dist>=2 (must be 4): far-boundary setup, no animation offset */

          // $C357 -- unbank A_dist / bank Adash_curve_bits
          // EX AF,AF'

          if (Adash_masked_lane_flags != 4 ||
              (*IY_heightptr)[0] <= (*IY_heightptr)[2])
            goto drlc_continue;

          // $C369: C = IY[0] - IY[2] (height span), B = C*2
          C_bresen_range = (*IY_heightptr)[0] - (*IY_heightptr)[2];
          B_HLzone_stride = C_bresen_range * 2;
          L_left_hand_table_lo = ~((96 - (*IY_heightptr)[0]) << 1); // byte offset
          HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);
          SP_output = HL_left_hand_table;
          /* L is ~(even) = odd → hi byte; [0]=hi, [-1]=lo */
          DE_roadpos = (HL_left_hand_table[0] << 8) + HL_left_hand_table[-1];

          L_left_hand_table_lo -= B_HLzone_stride;
          H_left_hand_table_hi--;
          HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);
        }
      } else { /* bit 4 CLEAR */
        // {4TO3R, 3TO4L, 3TO2L, 3TO2R, 2TO3L, TUNNEL_ENTRY, FORKED}

        /* $C37E: bit4=0 entry: dispatch to far-boundary or fall through to near-boundary */
        // EX AF,AF' -- unbank 'dist' / bank Adash_curve_bits
        if (A_dist < 2) {
          // $C37E -- bank 'dist' again / unbank Adash_curve_bits
          // EX AF,AF'

          /* $C37F: path 2 -- bit4=0, dist<2 */
          if (Adash_masked_lane_flags != 4) {
            // {4TO3R, 3TO4L, 3TO2R, 2TO3L, FORKED}
            A_curve_step = 0x20;
            C_ref_height = (*IY_heightptr)[1];
          } else {
            // {3TO2L, TUNNEL_ENTRY}
            A_curve_step = 0x00;
            C_ref_height = (*IY_heightptr)[2];
          }

          // $C392: C = IY[0] - ref_height (height span)
          SM_C3BD_bend_offset = A_curve_step;
          A_bresen_range = (*IY_heightptr)[0] - C_ref_height;
          if ((s8) A_bresen_range <= 0)
            goto drlc_continue;

          // $C39F: B = C*2
          C_bresen_range = A_bresen_range;
          B_HLzone_stride = A_bresen_range * 2;
          L_left_hand_table_lo = ~((96 - (*IY_heightptr)[0]) << 1); // byte offset
          HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);

          // $C3AA
          SP_output = HL_left_hand_table;
          H_left_hand_table_hi--; // widen to left
          HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);
          /* L unchanged (odd) → still hi byte; [0]=hi, [-1]=lo */
          DE_roadpos = (HL_left_hand_table[0] << 8) + HL_left_hand_table[-1];

          // $C3AF
          L_left_hand_table_lo -= B_HLzone_stride;
          H_left_hand_table_hi++; // widen to right
          HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);

          // $C3B2
          A_anim_offset = ((state->fast_counter >> 3) & 0x1C) + SM_C3BD_bend_offset;

          // $C3BF-$C3C2: DE = road_pos + anim  ($C3C2: ADD HL,DE; $C3C3: EX DE,HL)
          // Conv: path 2a adds anim (Z80: ADD HL,DE) whereas path 1a subtracts it
          // (Z80: SBC HL,DE) — mirror-image treatment of the two road edges.
          DE_roadpos += A_anim_offset;
        } else {
          /* $C3CA: bit4=0, dist>=2 (must be 4): far-boundary setup, no animation offset */

          // $C3CA -- unbank A_dist / bank Adash_curve_bits
          // EX AF,AF'

          if (Adash_masked_lane_flags != 4 ||
              (*IY_heightptr)[0] <= (*IY_heightptr)[2])
            goto drlc_continue;

          // $C3DB: C = IY[0] - IY[2] (height span), B = C*2
          C_bresen_range = (*IY_heightptr)[0] - (*IY_heightptr)[2];
          B_HLzone_stride = C_bresen_range * 2;
          L_left_hand_table_lo = ~((96 - (*IY_heightptr)[0]) << 1); // byte offset
          HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);
          SP_output = HL_left_hand_table;
          H_left_hand_table_hi--; // widen to left
          HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);
          /* L unchanged (odd) → still hi byte; [0]=hi, [-1]=lo */
          DE_roadpos = (HL_left_hand_table[0] << 8) + HL_left_hand_table[-1];

          L_left_hand_table_lo -= B_HLzone_stride;
          H_left_hand_table_hi++; // widen to right
          HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);
        }
      }

      // Q. Is it a right hand table at this point?

      /* $C3EE-$C405: read second table value, compute clamped displacement */
      HL_pos_delta = ((HL_left_hand_table[0] << 8) + HL_left_hand_table[-1]) - DE_roadpos;
      A_pos_delta_lo = HL_pos_delta & 0xFF;
      if ((s16) HL_pos_delta >= 0)
        A_step = (A_pos_delta_lo < 0) ? 127 : A_pos_delta_lo;
      else
        A_step = (A_pos_delta_lo >= 0) ? -127 : A_pos_delta_lo;

      /* $C407-$C412: set up SP output pointer */
      SP_output++;
      if ((*IX_lanesptr)[0] & (1 << 5))
        // {4TO3L, 3TO4L, 2TO3L, FORKED}
        SP_output -= 256; /* step back by 256 bytes ($C408) */

      /* $C413-$C420: derive step and direction */
      L_step = A_step;
      B_range = C_bresen_range;
      A_range = C_bresen_range;
      if (L_step < 0) {
        L_step = -L_step;
        A_direction = -1;
        if (B_range < L_step) /* Conv: removed A shuffle */
          goto drlc_steep_step;
      } else {
        A_direction = +1;
        if (A_range < L_step)
          goto drlc_steep_step;
      }

      /* $C42B-$C437: normal Bresenham -- step <= range */
      A_accum = B_range >> 1;
      do {
        A_accum += L_step;
        if (A_accum >= C_bresen_range) {
          A_accum -= C_bresen_range;
          DE_roadpos += A_direction; /* Conv: self modified code replaced with direction */
        }
        SP_output -= 2;
        SP_output[0] = DE_roadpos & 0xFF;
        SP_output[1] = DE_roadpos >> 8;
      } while (--B_range > 0);

drlc_continue:
      (*IY_heightptr)++;
    }
  }

  // $C43B
  dr_set_lane_callback(state,
                       B_fill_pattern,
                       C_horizon,
                       DE_backbuf,
                       L_row,
                       dr_four_lane_highway,
                       IX_lanesptr,
                       IY_heightptr); /* tail call */
  return;

drlc_steep_step:
  /* $C441-$C450: alternate Bresenham -- step > range */
  A_accum = 0;
  do {
    for (;;) {
      DE_roadpos += A_direction;
      A_accum += C_bresen_range;
      if (A_accum < C_bresen_range || A_accum >= L_step) /* overflow or >= step */
        break;
    }
    A_accum -= L_step;
    SP_output -= 2;
    SP_output[0] = DE_roadpos & 0xFF;
    SP_output[1] = DE_roadpos >> 8;
  } while (--B_range > 0);
  goto drlc_continue;
}

/**
 * $C452: Draw road
 *
 * Main road-drawing entry point. Initialises per-frame road state: clears
 * on_dirt_track, dt_tunnel_visible and dr_in_tunnel; sets dr_edge_thickness to
 * 3. Computes the initial horizon counter (96 − height_table[1]). Determines
 * the stripe phase from the lane-data buffer offset (bit 0 = XOR base, bit 1 =
 * initial stripe state), sets the stripe parameters, then falls through to
 * dr_read_lanes to begin the row-by-row road render.
 *
 * \param[in,out] state Pointer to game state.
 */
static void draw_road(chqstate_t *state)
{
  u8  *IYheightptr;      /* pointer to height_table[1]: starting height entry (was IY) */
  int  Ccounter;         /* 96 minus the first height value: row count to horizon (was C) */
  u8  *IXlanesptr;       /* pointer into road_buffer lane data at ROADBUF_LANES_OFFSET (was IX) */
  int  Blanesdataoffset; /* byte offset of IXlanesptr within road_buffer: selects stripe phase (was B) */
  int  carry_stripe;     /* bit 1 of Blanesdataoffset: selects XOR-stripe or empty-fill path (carry) */
  int  Htable_offset;    /* stripe table offset: $D0 (stripe) or $00 (no stripe) (was H) */
  int  Lstripe_height;   /* dr_edge_graphic_offset value: 16 (stripe) or 48 (no stripe) (was L) */
  int  Axor_base;        /* dr_stripe_xor_base: always $D0 (was A) */
  u8   Bfill_pattern;    /* road fill pattern: _X_X_X_X (stripe) or ________ (no stripe) (was B) */
  u8   Lrow;             /* initial row index: 0xFF (row −1, fills upward) (was L) */
  u16  DEbackbuf;        /* initial back-buffer address: $0100 (wrapped ROM sentinel) (was DE) */

  state->on_dirt_track = 0;

  state->dt_tunnel_visible = 0;
  state->dr_in_tunnel = 0;

  /* This affects the thickness of the road edges and lane markings with
   * increasing distance. A larger value means lines remain thick into
   * distance. */
  state->dr_edge_thickness = 3;

  IYheightptr = &state->height_table[1];
  Ccounter = 96 - *IYheightptr;

  /* Set initial road stripe state */
  IXlanesptr = ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET);
  Blanesdataoffset = IXlanesptr - &state->roadbuf_start[0];
  state->dr_initial_stripe_state = Blanesdataoffset & 1;
  carry_stripe = (Blanesdataoffset >> 1) & 1; // test bit 1

  Htable_offset = 0xD0;
  Lstripe_height = 16;
  Axor_base = 0xD0;
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
  Lrow = 0xFF; // row -1
  DEbackbuf = 0x0100; /* a wrapped-around ROM address since we draw by PUSHing! */
  state->dr_fill_pattern = Bfill_pattern;

  dr_read_lanes(state, IXlanesptr, IYheightptr, Bfill_pattern, Ccounter,
                DEbackbuf, Lrow); // was FALLTHROUGH
}

/**
 * $C4AD: draw_road — read lanes
 *
 * Reads the current road-buffer lane byte, extracts the left-offset field, and
 * dispatches to the appropriate road-section handler. If the left offset is
 * zero the road is a standard un-modified section and dr_callback is called
 * directly. Otherwise bit 6 of the lane byte (after SLA) selects: clear =
 * normal 2/3/4-lane or lane-change section dispatching to
 * draw_road_lanes_change; set with bit 7 clear = tunnel section dispatching to
 * dr_dispatch; set with bit 7 set = dirt track or forked road.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     IXlanesptr Pointer into road_buffer at the current lane byte.
 *   (was IX)
 * \param[in]     IYheightptr Pointer into height_table at the current row.
 *   (was IY)
 * \param[in]     Bfill_pattern Road fill pattern for this scanline. (was B)
 * \param[in]     Ccounter Horizon scanline counter. (was C)
 * \param[in]     DEbackbuf Back-buffer row address. (was DE)
 * \param[in]     Lrow Row index within the x-position table page. (was L)
 */
static void dr_read_lanes(chqstate_t *state, u8 *IXlanesptr, const u8 *IYheightptr,
                          int Bfill_pattern, int Ccounter, int DEbackbuf,
                          int Lrow)
{
  int carry;                /* carry from SLA of lane byte; indicates left-side occupancy (carry) */
  int Aleft_offset;         /* left-offset field of the lane byte (was A) */
  u8  Ldash_lanes;          /* full lane byte banked in L' for bit testing (was L') */
  int Aleft_hand_table_hi;  /* high byte of left x-position table address (was A) */
  int H_left_hand_table_hi; /* copy of Aleft_hand_table_hi for draw_road_lanes_change (was H) */
  int Cdash_neg_lane_count; /* negative lane count: −2 or −3 for 2/3-lane sections (was C') */
  int Hdash_in_tunnel;      /* 1 when inside a tunnel section, 0 otherwise (was H') */
  int Atunnel_visible;      /* tunnel visibility state: 1=entering, 2=exiting (was A) */
  int Cdash_fill_pattern;   /* fill pattern override for tunnel: 0xFF (solid) or 0x00 (open) (was C') */

  Aleft_offset = *IXlanesptr & MAP_LANES_LEFT_OFFSET_MASK;
  if (Aleft_offset == 0) {
    // no left side calcs required in this case?
    // callback here is e.g. dr_four_lane_highway
    state->dr_callback(state, Bfill_pattern, Ccounter, DEbackbuf, Lrow, &IXlanesptr, &IYheightptr); /* tail call */
    return;
  }

  // EXX - BANK

  Ldash_lanes = *IXlanesptr; // reload lanes byte
  // convert left hand pos (1+) to table hi byte ($E8+)
  Aleft_hand_table_hi = Aleft_offset + 0xE7;
  state->dr_left_table_hi_1 = Aleft_hand_table_hi;
  state->dr_left_table_hi_2 = Aleft_hand_table_hi;
  H_left_hand_table_hi = Aleft_hand_table_hi;
  SLA(Ldash_lanes);
  if ((Ldash_lanes & (1 << 7)) == 0) {
    /* If bit 6 was clear then it's a normal road (not tunnel, dirt track or forked road). */
    if (carry) {
      /* If bit 7 was set then it's a 3-lane, 3-to-4 lane or 4-to-3 lane section. */
      Aleft_hand_table_hi += 3; // $E8..$EA becomes $EB..$ED
      Cdash_neg_lane_count = -3;
    } else {
      /* If bit 7 was clear then it's a 2-lane, 2-to-3 lane or 3-to-2 lane section. */
      Aleft_hand_table_hi += 2; // $E8..$EA becomes $EA..$EC
      Cdash_neg_lane_count = -2;
    }
    state->dr_right_table_hi_2 = state->dr_right_table_hi_1 = Aleft_hand_table_hi;
    state->dr_neg_lane_count = Cdash_neg_lane_count; // Conv: A removed
    draw_road_lanes_change(state,
                           Bfill_pattern,
                           Ccounter,
                           DEbackbuf,
                           H_left_hand_table_hi,
                           Lrow,
                           &IXlanesptr,
                           &IYheightptr); /* tail call */
  } else {
    /* If bit 6 was clear then it's a special road (tunnel, dirt track or forked road). */
    if (carry == 0) {
      /* If bit 7 was clear then it's a tunnel section. */
      Cdash_fill_pattern = 0xFF;
      Hdash_in_tunnel = 1;
      if ((Ldash_lanes & (3 << 3)) != 0) {
        /* If bits 2 or 3 are set then it's a tunnel transition section. */
        state->dt_tunnel_distance = IYheightptr - &state->height_table[0];
        Atunnel_visible = 1;
        Cdash_fill_pattern = 0x00; // was INC C
        if ((Ldash_lanes & (1 << 5)) != 0) {
          /* If bit 4 is set then it's a tunnel exit. */
          Cdash_fill_pattern = 0xFF; // was DEC C
          Atunnel_visible = 2; // was INC A
          Hdash_in_tunnel = 0; // was DEC H
        }
        state->dt_tunnel_visible = Atunnel_visible;
      }

      state->dr_in_tunnel = Hdash_in_tunnel;
      state->dr_right_table_hi_2 = state->dr_right_table_hi_1 = 0xEB;
      state->dr_neg_lane_count = -1;
      // Conv: Reg B shuffle removed

      // EXX - UNBANK

      dr_dispatch(state, Cdash_fill_pattern, Ccounter, DEbackbuf, Lrow, &IXlanesptr, &IYheightptr); /* tail call */
    } else {
      /* If bit 7 was set then it's a dirt track or forked road. */

      // Note: We're still banked here!

      if (Ldash_lanes & (1 << 6)) {
        /* If bit 5 was set then it's a forked road. */
        draw_forked_road(state, IXlanesptr, IYheightptr,
                         Bfill_pattern, Ccounter, DEbackbuf, Lrow); /* tail call */
      } else {
        /* If bit 5 was clear then it's a dirt track section. */
        /* Note: This is a mystery. There's a check here which sets conditionally
         * on_dirt_track but I've not yet found any use of this track type. */
        state->on_dirt_track = ((Ldash_lanes & 0x18) == 0) ? 1 : 0; /* bits 2 or 3 clear => dirt track */
        state->dr_neg_lane_count = -1;
        dr_set_lane_callback(state, Bfill_pattern, Ccounter, DEbackbuf, Lrow, dr_four_lane_highway, &IXlanesptr, &IYheightptr); /* tail call */
      }
    }
  }
}

/**
 * $C534: draw_road — four lane highway
 *
 * Sets the left/right x-position table high bytes to the widest possible road
 * extent ($E8/$EC) and the negative lane count to −4, then falls through to
 * dr_set_lane_callback to store dr_dispatch as the lane callback and begin the
 * fill dispatch.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Bfill_pattern Road fill pattern for this scanline. (was B)
 * \param[in]     Ccounter Horizon scanline counter. (was C)
 * \param[in]     DEbackbuf Back-buffer row address. (was DE)
 * \param[in]     Lrow Row index within the x-position table page. (was L)
 * \param[in,out] IXlanesptr Pointer into road_buffer lane data. (was IX)
 * \param[in,out] IYheightptr Pointer into height_table. (was IY)
 */
static void dr_four_lane_highway(chqstate_t *state, int Bfill_pattern,
                                 int Ccounter, int DEbackbuf, int Lrow,
                                 u8 **IXlanesptr, const u8 **IYheightptr)
{
  // EXX - BANK (we enter unbanked)

  // Set left/right hand road position to leftmost/rightmost
  state->dr_left_table_hi_1 = 0xE8;
  state->dr_left_table_hi_2 = 0xE8;
  state->dr_right_table_hi_1 = 0xEC;
  state->dr_right_table_hi_2 = 0xEC;
  state->dr_neg_lane_count = -4;

  dr_set_lane_callback(state, Bfill_pattern, Ccounter, DEbackbuf, Lrow,
                       dr_dispatch, IXlanesptr, IYheightptr); // was FALLTHROUGH
}

/**
 * $C54D: draw_road — set lane callback and enter fill dispatch
 *
 * Stores [HLdash_callback] in state->dr_callback (models the Z80 self-modifying
 * `CALL nn` at $C551), then falls through to dr_dispatch.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Bfill_pattern Road fill pattern for this scanline. (was B)
 * \param[in]     Ccounter Horizon scanline counter. (was C)
 * \param[in]     DEbackbuf Back-buffer row address. (was DE)
 * \param[in]     Lrow Row index within the x-position table page. (was L)
 * \param[in]     HLdash_callback Lane callback stored in dr_callback. (was HL')
 * \param[in,out] IXlanesptr Pointer into road_buffer lane data. (was IX)
 * \param[in,out] IYheightptr Pointer into height_table. (was IY)
 */
static void dr_set_lane_callback(chqstate_t *state, int Bfill_pattern,
                                 int Ccounter, int DEbackbuf, int Lrow,
                                 dr_callback_t *HLdash_callback,
                                 u8 **IXlanesptr, const u8 **IYheightptr)
{
  state->dr_callback = HLdash_callback;
  // EXX - UNBANK (we enter banked)
  dr_dispatch(state, Bfill_pattern, Ccounter, DEbackbuf, Lrow, IXlanesptr, IYheightptr); // was FALLTHROUGH
}

/**
 * $C551: draw_road — dispatch to filled or unfilled path
 *
 * Reads [Bfill_pattern]; non-zero dispatches to dr_dispatch_filled (the
 * filled/chequerboard road path), zero sets dr_fill_fn to dr_advance_unfilled
 * and falls through to dr_advance_unfilled (the empty/sky path).
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Bfill_pattern Non-zero selects the filled road scanline path.
 *   (was B)
 * \param[in]     Ccounter Horizon scanline counter. (was C)
 * \param[in]     DEbackbuf Back-buffer row address. (was DE)
 * \param[in]     Lrow Row index within the x-position table page. (was L)
 * \param[in,out] IXlanesptr Pointer into road_buffer lane data. (was IX)
 * \param[in,out] IYheightptr Pointer into height_table. (was IY)
 */
static void dr_dispatch(chqstate_t *state, int Bfill_pattern, int Ccounter,
                             int DEbackbuf, int Lrow,
                             u8 **IXlanesptr, const u8 **IYheightptr)
{
  // ENTERED UNBANKED

  int Afill_pattern; /* copy of Bfill_pattern; tested for filled/unfilled branch (was A) */

  Afill_pattern = Bfill_pattern;
  if (Afill_pattern) {
    dr_dispatch_filled(state, Afill_pattern, Ccounter, DEbackbuf, Lrow, IXlanesptr, IYheightptr); /* tail call */
    return;
  }

  // EXX - BANK
  state->dr_fill_fn = dr_advance_unfilled;
  // EXX - UNBANK

  // EX AF,AF' - BANK
  dr_advance_unfilled(state, Ccounter, DEbackbuf, Lrow, Afill_pattern /* as A' */, IXlanesptr, IYheightptr); // was FALLTHROUGH
}

/**
 * $C55F: draw_road — advance backbuffer and rollover check (unfilled path)
 *
 * Decrements the high byte of [DEbackbuf]. When the low nibble of the old high
 * byte is zero (Spectrum screen row boundary), falls through to
 * dr_rollover_unfilled to apply the row-group advance; otherwise falls through
 * to dr_write_scanline_unfilled to write the current row.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Ccounter Horizon scanline counter. (was C)
 * \param[in]     DEbackbuf Back-buffer row address. (was DE)
 * \param[in]     Lrow Row index within the x-position table page. (was L)
 * \param[in]     Adash_fill_pattern Fill pattern banked in A'. (was A')
 * \param[in,out] IXlanesptr Pointer into road_buffer lane data. (was IX)
 * \param[in,out] IYheightptr Pointer into height_table. (was IY)
 */
static void dr_advance_unfilled(chqstate_t *state,
                                int Ccounter,
                                int DEbackbuf,
                                int Lrow,
                                int Adash_fill_pattern,
                                u8 **IXlanesptr,
                                const u8 **IYheightptr)
{
  // ENTERED UNBANKED (but AF is banked)

  int A; /* high byte of DEbackbuf before decrement; low nibble detects row boundary (was A) */

  A = DEbackbuf >> 8;
  HI_DEC(DEbackbuf);
  if ((A & 0x0F) == 0) {
    dr_rollover_unfilled(state, Ccounter, DEbackbuf, Lrow, Adash_fill_pattern, IXlanesptr, IYheightptr); /* tail call */
    return;
  }

  dr_write_scanline_unfilled(state, Ccounter, DEbackbuf, Lrow, Adash_fill_pattern, IXlanesptr, IYheightptr); // was FALLTHROUGH
}

/**
 * $C565: draw_road — write unfilled (zero) road scanline
 *
 * Clamps [DEbackbuf] to the last valid back-buffer row when it is out of range
 * (the Z80 would silently write to unintended low memory). Stores [DEbackbuf]
 * in dr_backbuf_1, then computes the rightmost byte address of the row and
 * calls dr_fill_left_stripe with a zero fill value and zero jump index (15 zero
 * bytes written per scanline).
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Ccounter Horizon scanline counter. (was C)
 * \param[in]     DEbackbuf Back-buffer row address. (was DE)
 * \param[in]     Lrow Row index within the x-position table page. (was L)
 * \param[in]     Adash_fill_pattern Fill pattern banked in A'. (was A')
 * \param[in,out] IXlanesptr Pointer into road_buffer lane data. (was IX)
 * \param[in,out] IYheightptr Pointer into height_table. (was IY)
 */
static void dr_write_scanline_unfilled(chqstate_t *state, int Ccounter, int DEbackbuf,
                                       int Lrow, int Adash_fill_pattern,
                                       u8 **IXlanesptr, const u8 **IYheightptr)
{
  // ENTERED UNBANKED (but AF is banked)

  s8   Bneg_lane_count;    /* −1: passed to dr_fill_left_stripe as lane count (was B) */
  u16  DEdash_backbuf;     /* banked copy of DEbackbuf (was DE') */
  u8   Ldash;              /* low byte of DEdash_backbuf + 31: rightmost byte column (was L') */
  u8   Hdash;              /* high byte of DEdash_backbuf (was H') */
  u8  *SPoutput;           /* back-buffer pointer for the rightmost byte of this row (was SP) */
  u16  HLdash_fill;        /* fill word for the unfilled scanline: 0 (was HL') */
  u8   Cdash_zerofill;     /* fill byte for zero-fill: 0 (was C') */

  /* Z80: writes below the backbuffer landing anywhere in $0000-$EFFF don't
   * crash the original -- a no-op in ROM ($0000-$3FFF), a harmless stray
   * write elsewhere in RAM otherwise.
   * C port: redirect any out-of-range DEbackbuf to the last backbuffer row
   * ($FFE0) so all pointer arithmetic (here and in downstream callers,
   * including dr_fill_left_stripe's secondary dr_read_lanes) stays valid. */
  if (DEbackbuf < 0xF000 || DEbackbuf > 0x10000)
    DEbackbuf = 0xFFE0;
  state->dr_backbuf_1 = (u16)DEbackbuf;
  Bneg_lane_count = -1; // CHECK

  // EXX - BANK

  DEdash_backbuf = state->dr_backbuf_1;
  Ldash = (DEdash_backbuf & 0xFF) + 31; // screen address of row's rightmost byte
  Hdash = DEdash_backbuf >> 8;
  SPoutput = ADDRTOBACKBUF((Hdash << 8) | Ldash);

  HLdash_fill = 0; // fill value (zeros for unfilled verge)
  Cdash_zerofill = 0;
  /* jump_index being 0 here causes 15 PUSH HLs */
  dr_fill_left_stripe(state,
                      SPoutput,
                      0,
                      Bneg_lane_count,
                      Ccounter,
                      DEbackbuf,
                      Lrow,
                      Adash_fill_pattern,
                      Cdash_zerofill,
                      DEdash_backbuf,
                      HLdash_fill,
                      IXlanesptr,
                      IYheightptr);
}

/**
 * $C57C: draw_road — backbuffer row advance with rollover (filled path)
 *
 * Subtracts 32 from the low byte of [DEbackbuf]. When no borrow occurs the
 * Spectrum row group has not rolled over, so adds 16 to the high byte to
 * advance by one row group. Falls through to dr_fill.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Ccounter Horizon scanline counter. (was C)
 * \param[in]     DEbackbuf Back-buffer row address before rollover adjust.
 *   (was DE)
 * \param[in]     Lrow Row index within the x-position table page. (was L)
 * \param[in]     Adash_fill_pattern Fill pattern banked in A'. (was A')
 * \param[in,out] IXlanesptr Pointer into road_buffer lane data. (was IX)
 * \param[in,out] IYheightptr Pointer into height_table. (was IY)
 */
static void dr_rollover_filled(chqstate_t *state, int Ccounter, int DEbackbuf,
                               int Lrow, int Adash_fill_pattern,
                               u8 **IXlanesptr, const u8 **IYheightptr)
{
  LO_ADD(DEbackbuf, -32);
  if ((DEbackbuf & 0xFF) < 224) // if no carry: D += 16
    HI_ADD(DEbackbuf, 16);
  dr_fill(state, Ccounter, DEbackbuf, Lrow, Adash_fill_pattern, IXlanesptr, IYheightptr); /* tail call */
}

/**
 * $C58A: draw_road — backbuffer row advance with rollover (unfilled path)
 *
 * Identical row-group advance logic to dr_rollover_filled but falls through to
 * dr_write_scanline_unfilled instead of dr_fill.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Ccounter Horizon scanline counter. (was C)
 * \param[in]     DEbackbuf Back-buffer row address before rollover adjust.
 *   (was DE)
 * \param[in]     Lrow Row index within the x-position table page. (was L)
 * \param[in]     Adash_fill_pattern Fill pattern banked in A'. (was A')
 * \param[in,out] IXlanesptr Pointer into road_buffer lane data. (was IX)
 * \param[in,out] IYheightptr Pointer into height_table. (was IY)
 */
static void dr_rollover_unfilled(chqstate_t *state, int Ccounter, int DEbackbuf,
                                 int Lrow, int Adash_fill_pattern,
                                 u8 **IXlanesptr, const u8 **IYheightptr)
{
  LO_ADD(DEbackbuf, -32);
  if ((DEbackbuf & 0xFF) < 224) // if no carry: D += 16
    HI_ADD(DEbackbuf, 16);
  dr_write_scanline_unfilled(state, Ccounter, DEbackbuf, Lrow, Adash_fill_pattern, IXlanesptr, IYheightptr); /* tail call */
}

/**
 * $C598: draw_road — enter filled path
 *
 * Stores dr_advance_filled in state->dr_fill_fn (the SM field that switches
 * between filled and unfilled draw functions), then falls through to
 * dr_advance_filled with [Afill_pattern] banked in A'.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Afill_pattern Non-zero fill pattern for the road surface.
 *   (was A)
 * \param[in]     Ccounter Horizon scanline counter. (was C)
 * \param[in]     DEbackbuf Back-buffer row address. (was DE)
 * \param[in]     Lrow Row index within the x-position table page. (was L)
 * \param[in,out] IXlanesptr Pointer into road_buffer lane data. (was IX)
 * \param[in,out] IYheightptr Pointer into height_table. (was IY)
 */
static void dr_dispatch_filled(chqstate_t *state, int Afill_pattern, int Ccounter,
                               int DEbackbuf, int Lrow, u8 **IXlanesptr,
                               const u8 **IYheightptr)
{
  // ENTERED UNBANKED

  // EXX BANK
  state->dr_fill_fn = dr_advance_filled;
  // EXX - UNBANK

  // EX AF,AF' - BANK
  dr_advance_filled(state, Ccounter, DEbackbuf, Lrow, Afill_pattern /*in A'*/, IXlanesptr, IYheightptr); // was FALLTHROUGH
}

/**
 * $C5A1: draw_road — advance backbuffer and rollover check (filled path)
 *
 * Filled-path equivalent of dr_advance_unfilled. Decrements the high byte of
 * [DEbackbuf]; when the low nibble of the old high byte is zero calls
 * dr_rollover_filled, otherwise falls through to dr_fill.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Ccounter Horizon scanline counter. (was C)
 * \param[in]     DEbackbuf Back-buffer row address. (was DE)
 * \param[in]     Lrow Row index within the x-position table page. (was L)
 * \param[in]     Adash_fill Fill pattern banked in A'. (was A')
 * \param[in,out] IXlanesptr Pointer into road_buffer lane data. (was IX)
 * \param[in,out] IYheightptr Pointer into height_table. (was IY)
 */
static void dr_advance_filled(chqstate_t *state, int Ccounter, int DEbackbuf,
                              int Lrow, int Adash_fill, u8 **IXlanesptr,
                              const u8 **IYheightptr)
{
  // ENTERED UNBANKED (but AF is banked)

  int A; /* high byte of DEbackbuf before decrement; low nibble detects row boundary (was A) */

  A = DEbackbuf >> 8;
  HI_DEC(DEbackbuf);
  if ((A & 0x0F) == 0) {
    dr_rollover_filled(state, Ccounter, DEbackbuf, Lrow, Adash_fill, IXlanesptr, IYheightptr); /* tail call */
    return;
  }

  dr_fill(state, Ccounter, DEbackbuf, Lrow, Adash_fill, IXlanesptr, IYheightptr); // was FALLTHROUGH
}

/**
 * $C5A7: draw_road — setup and draw right verge and centre spans
 *
 * Core road scanline fill function. Reads the current row from the x-position
 * tables (via [Lrow]), looks up the right-verge and left-stripe widths, builds
 * the fill word from [Adash_fill_pattern] and a counter-derived XOR, computes
 * the back-buffer write pointer (SPoutput) from [DEbackbuf], then calls
 * dr_fill_left_stripe with the assembled parameters to write the right-verge,
 * lane-marking and left-verge bytes to the back buffer.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Ccounter Horizon scanline counter. (was C)
 * \param[in]     DEbackbuf Back-buffer row address. (was DE)
 * \param[in]     Lrow Row index within the x-position table page. (was L)
 * \param[in]     Adash_fill_pattern Road fill pattern banked in A'. (was A')
 * \param[in,out] IXlanesptr Pointer into road_buffer lane data. (was IX)
 * \param[in,out] IYheightptr Pointer into height_table. (was IY)
 */
static void dr_fill(chqstate_t *state,
                    int         Ccounter,
                    int         DEbackbuf,
                    int         Lrow,
                    int         Adash_fill_pattern,
                    u8        **IXlanesptr,
                    const u8  **IYheightptr)
{
  // ENTERED UNBANKED (but AF is banked)

  int  carry;                   /* carry from XOR/stripe computation (carry) */
  int  Arow;                    /* current row index after counter-to-row conversion (was A) */
  int  Bneg_lane_count;         /* negative lane count from dr_neg_lane_count (was B) */
  int  Ldash_row;               /* banked copy of Lrow; row index within x-position page (was L') */
  int  Bdash_holds_16;          /* 16: constant for XOR cycle computation (was B') */
  int  Cdash_mask;              /* XOR mask derived from Arow and dr_stripe_xor_base (was C') */
  u8  *HLdash_ptr;              /* pointer into x-position table for right-edge lookup (was HL') */
  int  Aleftval;                /* left x-position table value for this row (was A) */
  u8   Aleft_stripe_width;      /* left stripe width after dr_edge_graphic_offset adjust (was A) */
  int  Edash_left_stripe_width; /* banked copy of Aleft_stripe_width (was E') */
  int  Arightval;               /* right x-position table value for this row (was A) */
  u8   Aright_stripe_width;     /* right stripe width: right table value + dr_right_edge_offset (was A) */
  u16  DEdash_backbuf;          /* banked DEbackbuf copy; used to build SPoutput (was DE') */
  int  Ldash_backbuf;           /* low byte of DEdash_backbuf + 31: rightmost column (was L') */
  int  Hdash_backbuf;           /* high byte of DEdash_backbuf (was H') */
  u8  *SPoutput;                /* back-buffer write pointer for this scanline's rightmost byte (was SP) */
  u16  HLdash_fill;             /* fill word: fill_pattern repeated in both bytes (was HL') */
  u16  BCdash_zerofill;         /* zero fill word: 0x0000 for the verge region (was BC') */

  carry = 0;

  /* Z80: writes below the backbuffer landing anywhere in $0000-$EFFF don't
   * crash the original -- a no-op in ROM ($0000-$3FFF), a harmless stray
   * write elsewhere in RAM otherwise.
   * C port: redirect any out-of-range DEbackbuf to the last backbuffer row
   * ($FFE0) so all pointer arithmetic (here and in downstream callers,
   * including dr_fill_left_stripe's secondary dr_read_lanes) stays valid. */
  if (DEbackbuf < 0xF000 || DEbackbuf > 0x10000)
    DEbackbuf = 0xFFE0;

  state->dr_backbuf_2 = DEbackbuf;
  Arow = Lrow; // byte offset within road table page (0xFF = bottom scanline)
  Bneg_lane_count = state->dr_neg_lane_count;
  assert(Bneg_lane_count >= -4 && Bneg_lane_count <= -1);
  assert(state->dr_left_table_hi_2  >= 0xE8 && state->dr_left_table_hi_2  <= 0xED);
  assert(state->dr_right_table_hi_2 >= 0xE8 && state->dr_right_table_hi_2 <= 0xED);

  // EXX - BANK

  Ldash_row = Arow;
  Bdash_holds_16 = 16;
  Cdash_mask = 0xF8;

  HLdash_ptr = (u8 *)hi_to_xpostab(state, state->dr_left_table_hi_2) + Ldash_row;
  Aleftval = *HLdash_ptr;
  if (Aleftval) {
    Aleft_stripe_width = ((s8) Aleftval < 0) ? 0 : 15;
  } else {
    Aleft_stripe_width = (HLdash_ptr[-1] & Cdash_mask) >> 3;
    RR(Aleft_stripe_width);
    Aleft_stripe_width += carry;
    if (Aleft_stripe_width >= Bdash_holds_16)
      Aleft_stripe_width--;
  }
  assert(Aleft_stripe_width <= 15);

  // $C5D3
  Edash_left_stripe_width = Aleft_stripe_width;
  Aleft_stripe_width = ~Edash_left_stripe_width + Bdash_holds_16; // (15 - x)
  assert(Aleft_stripe_width <= 15);
  state->dr_left_stripe_width = Aleft_stripe_width;

  HLdash_ptr = (u8 *)hi_to_xpostab(state, state->dr_right_table_hi_2) + Ldash_row;
  Arightval = *HLdash_ptr;
  carry = 0;
  if (Arightval) {
    Aright_stripe_width = ((s8) Arightval < 0) ? 0 : 15;
  } else {
    Aright_stripe_width = (HLdash_ptr[-1] & Cdash_mask) >> 3;
    RR(Aright_stripe_width); /* carry is 0 as set above */
  }
  assert(Aright_stripe_width <= 15);

  // $C5F0
  state->dr_right_stripe_width = Aright_stripe_width;

  // $C5F3 — road_width = (15 - raw_right) + raw_left; valid range 0..30
  state->dr_road_width = ~Aright_stripe_width + Bdash_holds_16 + Edash_left_stripe_width; // another (15 - x + ...)
  assert(state->dr_right_stripe_width <= 15);
  assert(state->dr_left_stripe_width  <= 15);
  DEdash_backbuf = state->dr_backbuf_2;
  Ldash_backbuf = (DEdash_backbuf & 0xFF) + 31;
  Hdash_backbuf = DEdash_backbuf >> 8;
  SPoutput = ADDRTOBACKBUF((Hdash_backbuf << 8) | Ldash_backbuf);

  // EX AF,AF' - unbank Afill
  RLC(Adash_fill_pattern); // Rotate fill pattern so we checkerboard
  HLdash_fill = Adash_fill_pattern + (Adash_fill_pattern << 8); // Widen stripe fill byte to whole word
  // EX AF,AF' - bank Afill

  BCdash_zerofill = 0x0000; // Set road fill to zero

  /* Fill right hand road stripes - starting from right hand side. */
  // Conv: uses memset
  int n;
  n = (15 - state->dr_right_stripe_width) * 2;
  assert(n >= 0 && n <= 30);
  assert(VALID_BACKBUF_PTR(SPoutput));
  if (n > 0)
    memset(SPoutput -= n, HLdash_fill, n);

  /* Fill blank road surface - continuing from the right hand side. */
  // Conv: road_width > 15 is valid during transitions; Z80 jump table overshoots → zero fill.
  n = (15 - state->dr_road_width) * 2;
  assert(VALID_BACKBUF_PTR(SPoutput));
  if (n > 0)
    memset(SPoutput -= n, BCdash_zerofill, n);

  dr_fill_left_stripe(state,
                      SPoutput,
                      state->dr_left_stripe_width / 1,
                      Bneg_lane_count,
                      Ccounter,
                      DEbackbuf,
                      Lrow,
                      Adash_fill_pattern,
                      BCdash_zerofill & 0xFF,
                      DEdash_backbuf,
                      HLdash_fill,
                      IXlanesptr,
                      IYheightptr); // FALLTHROUGH
}

/**
 * $C62E: draw_road — left verge fill, road edges and lane markers
 *
 * Innermost road-scanline renderer. Writes the right-verge fill bytes to the
 * back buffer via a fall-through PUSH switch ([jump_index] selects how many
 * bytes). Then reads the left-verge x-position, writes the left-edge marking
 * (AND-OR masked), iterates over interior lane markings (up to
 * |[Bneg_lane_count]| lanes), writes the right-edge marking, and loops back via
 * dr_read_lanes to process the next scanline. After the loop, handles the
 * backdrop/sky fill based on the horizon counter. Recursion models the Z80's JP
 * NZ loop.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     SPoutput Back-buffer write pointer for this scanline's
 *   rightmost byte. (was SP)
 * \param[in]     jump_index Fall-through index into the fill-byte switch (0 =
 *   max fill). (was IX offset)
 * \param[in]     Bneg_lane_count Negative lane count: number of interior lanes
 *   to mark. (was B)
 * \param[in]     Ccounter Horizon scanline counter; loop terminates when it
 *   reaches 0. (was C)
 * \param[in]     DEbackbuf Back-buffer row address. (was DE)
 * \param[in]     Lrow Row index within the x-position table page. (was L)
 * \param[in]     Adash_fill_pattern Road fill pattern banked in A'. (was A')
 * \param[in]     Cdash_zerofill Zero fill byte for the verge region. (was C')
 * \param[in]     DEdash_backbuf Banked copy of [DEbackbuf] for x-position
 *   lookups. (was DE')
 * \param[in]     Hdash_fill High byte of fill word (same as [Cdash_zerofill]).
 *   (was H')
 * \param[in,out] IXlanesptr Pointer into road_buffer lane data. (was IX)
 * \param[in,out] IYheightptr Pointer into height_table. (was IY)
 */
static void dr_fill_left_stripe(chqstate_t *state,
                                u8         *SPoutput,
                                int         jump_index,
                                int         Bneg_lane_count,
                                u8          Ccounter,
                                int         DEbackbuf,
                                int         Lrow,
                                int         Adash_fill_pattern,
                                int         Cdash_zerofill,
                                int         DEdash_backbuf,
                                int         Hdash_fill,
                                u8        **IXlanesptr,
                                const u8  **IYheightptr)
{
  // BANKED ON ENTRY

  u8        Bdash;                /* banked base column offset for marking address: DEdash_backbuf low byte (was B') */
  u8        Hdash_markingsptr_hi; /* high byte of edge_markings address ($E4): index into edge_markings[] (was H') */
  u8        H;                    /* high byte of current x-position table address during lane/edge walk (was H) */
  u8       *HL;                   /* pointer into x-position table for the current row (was HL) */
  u8        A;                    /* x-position byte read from the table; also scratch (was A) */
  u8        Axpos;                /* left x-position table value for the left-edge lookup (was A) */
  u8        Hdash_markingsptr_lo; /* low byte of edge_markings address derived from A (was H') */
  const u8 *HLdash_markingsptr;   /* pointer into edge_markings[] for the current marking graphic (was HL') */
  u8        Edash;                /* column byte offset within back-buffer row derived from A (was E') */
  u8       *DEdash_backbufptr;    /* back-buffer write pointer for the current marking (was DE') */
  u8        Bfill_pattern;        /* fill pattern loaded from Adash_fill_pattern (was B) */
  u8        Ainitial_stripe_state; /* dr_initial_stripe_state: selects even/odd stripe XOR cycle (was A) */
  u8        Afill_pattern;        /* fill pattern read from state->dr_fill_pattern (was A) */
  u8        Aedge_graphic_offset; /* dr_edge_graphic_offset: selects edge marking variant (was A) */
  u8        Astripe_table_offset; /* dr_stripe_table_offset: selects stripe sub-table (was A) */
  u8        Aedge_thickness;      /* dr_edge_thickness: scales edge marking width (was A) */
  u8        Anew_xor_base;        /* updated dr_stripe_xor_base after XOR cycle wrap (was A) */
  u8        Cxor_base;
  u8        Aprev_height;
  u8        Aheight_diff;
  u8        Clane_byte;
  u8        B;
  u8        Atunnel_visible;
  u8        Cin_tunnel;
  u8        Ain_tunnel;
  u8        Cheight_diff;
  s8        Anew_diff;

  assert(jump_index >= 0 && jump_index <= 15);
  assert(Bneg_lane_count >= -4 && Bneg_lane_count <= -1);
  assert(state->dr_left_table_hi_1  >= 0xE8 && state->dr_left_table_hi_1  <= 0xED);
  assert(state->dr_right_table_hi_1 >= 0xE8 && state->dr_right_table_hi_1 <= 0xED);

  // Conv: use memset
  int n = (15 - jump_index) * 2;
  assert(n >= 0);
  assert(VALID_BACKBUF_PTR(SPoutput));
  if (n)
    memset(SPoutput -= n, Hdash_fill & 0xFF, n);

  Bdash = DEbackbuf & 0xFF;
  Cdash_zerofill--; // 0 -> 255
  Hdash_markingsptr_hi = 0xE4; // edge_markings hi/page

  // EXX - Unbank

  H = state->dr_left_table_hi_1;
  HL = (u8 *)hi_to_xpostab(state, H) + Lrow;
  if (*HL == 0) {
    Axpos = *WRAP(HL, -1, (u8 *)hi_to_xpostab(state, H));

    // EXX - Bank

    /* Build address of road edge marking graphic. */
    Hdash_markingsptr_lo = ((Axpos & 7) << 2) + state->dr_edge_graphic_offset;
    HLdash_markingsptr = &edge_markings[((Hdash_markingsptr_hi << 8) | Hdash_markingsptr_lo) - 0xE400];
    assert(HLdash_markingsptr >= &edge_markings[0]);
    assert(HLdash_markingsptr + 2 < &edge_markings[256]);

    Edash = ((Axpos >> 3) & 31) + Bdash;
    DEdash_backbufptr = ADDRTOBACKBUF((DEdash_backbuf & 0xFF00) | Edash);

    /* AND-OR masking here. */
    /* Left outer edge ($C643-$C666): AND-OR blend at E, direct copy at E+1 */
    *DEdash_backbufptr = (*DEdash_backbufptr & HLdash_markingsptr[0]) | HLdash_markingsptr[1]; // wraparound needed?
    HLdash_markingsptr += 3;
    DEdash_backbufptr++;
    *DEdash_backbufptr++ = *HLdash_markingsptr++; // also BC--;

    // EXX - Unbank
  }

  /* $C667 - Draw road lane markings. */
  while (++Bneg_lane_count != 0) {
    H++; // next lane
    assert(H >= 0xE8 && H <= 0xED);
    HL = (u8 *)hi_to_xpostab(state, H) + Lrow;
    if (*HL)
      continue;

    A = HL[-1]; // wraparound needed?

    // EXX - Bank

    /* Build address of road lane marking graphic. */
    Hdash_markingsptr_lo = ((A & 7) << 1) + state->dr_stripe_table_offset;
    HLdash_markingsptr = &edge_markings[((Hdash_markingsptr_hi << 8) | Hdash_markingsptr_lo) - 0xE400];
    assert(HLdash_markingsptr >= &edge_markings[0]);
    assert(HLdash_markingsptr + 1 < &edge_markings[256]);
    Edash = ((A >> 3) & 31) + Bdash;
    DEdash_backbufptr = ADDRTOBACKBUF((DEdash_backbuf & 0xFF00) | Edash);

    /* Draw - no masking since we're on a plain surface. */
    *DEdash_backbufptr++ = *HLdash_markingsptr++; // also BC--;
    *DEdash_backbufptr = *HLdash_markingsptr;

    // EXX - Unbank
  }

  /* $C68A - Right edge. */
  H = state->dr_right_table_hi_1;
  HL = (u8 *)hi_to_xpostab(state, H) + Lrow;
  A = *HL;
  Lrow--;
  if (*HL == 0) {
    A = HL[-1];

    // EXX - Bank

    /* Build address of road edge marking graphic. */
    Hdash_markingsptr_lo = ((A & 7) << 2) + state->dr_right_edge_offset;
    HLdash_markingsptr = &edge_markings[((Hdash_markingsptr_hi << 8) | Hdash_markingsptr_lo) - 0xE400];
    assert(HLdash_markingsptr >= &edge_markings[0]);
    assert(HLdash_markingsptr + 2 < &edge_markings[256]);
    Edash = ((A >> 3) & 31) + Bdash;
    DEdash_backbufptr = ADDRTOBACKBUF((DEdash_backbuf & 0xFF00) | Edash);

    /* AND-OR masking here. */
    *DEdash_backbufptr++ = *HLdash_markingsptr++; // also BC--;
    *DEdash_backbufptr = (*DEdash_backbufptr & HLdash_markingsptr[0]) | HLdash_markingsptr[1]; // wraparound needed?

    // EXX - Unbank
  }

  /* $C6AB - Loop end */
  Lrow = (Lrow - 1) & 0xFF;
  // Conv: This is a loop in the original but since the C conversion splits
  // draw_road into multiple functions this becomes recursion. The height-check
  // block below ($C6B0..$C79A) runs ONCE after the Z80 JP NZ loop exits. So
  // outer recursion levels must return here; only the innermost level (where
  // --Ccounter reaches 0 and the call is skipped) falls through to the
  // height-check.
  if (--Ccounter > 0) {
    state->dr_fill_fn(state, Ccounter, DEbackbuf, Lrow, Adash_fill_pattern,
                      IXlanesptr, IYheightptr);
    return;
  }

  // EX AF,AF'
  Bfill_pattern = Adash_fill_pattern;


dr_set_stripes:
  Ainitial_stripe_state = state->dr_initial_stripe_state ^ 1;
  state->dr_initial_stripe_state = Ainitial_stripe_state;
  if (Ainitial_stripe_state == 0) {
    Afill_pattern = state->dr_fill_pattern ^ 0x55;
    state->dr_fill_pattern = Afill_pattern;
    Bfill_pattern = Afill_pattern;
    Aedge_graphic_offset = state->dr_edge_graphic_offset ^ 0x20;
    state->dr_edge_graphic_offset = Aedge_graphic_offset;
    state->dr_right_edge_offset = Aedge_graphic_offset + 1;
    Astripe_table_offset = state->dr_stripe_table_offset ^ state->dr_stripe_xor_base;
    state->dr_stripe_table_offset = Astripe_table_offset;
  }



  Aedge_thickness = state->dr_edge_thickness - 1;
  state->dr_edge_thickness = Aedge_thickness;
  if (Aedge_thickness == 0) {
    Anew_xor_base = state->dr_stripe_xor_base + 0x10;
    if (Anew_xor_base > state->dr_stripe_xor_base) {
      state->dr_stripe_xor_base = Anew_xor_base;
      Cxor_base = Anew_xor_base;
      Astripe_table_offset = state->dr_stripe_table_offset;
      if (Astripe_table_offset)
        state->dr_stripe_table_offset = Cxor_base;
      state->dr_edge_graphic_offset += 0x40;
      state->dr_edge_thickness = 5;
    }
  }


  Aprev_height = **IYheightptr;
  (*IYheightptr)++;

  WRAP_INCREMENT_ASSIGN(*IXlanesptr, state->roadbuf_start);
  Aheight_diff = Aprev_height - **IYheightptr;
  if (Aheight_diff == 0)
    goto dr_level_road;
  else if ((s8)Aheight_diff > 0)
    goto dr_increasing;
  else goto dr_decreasing;

  /* $C715: Level road */
dr_level_road:
  Lrow = (Lrow - 2) & 0xFF;

  Clane_byte = **IXlanesptr;
  if ((Clane_byte & (1 << 6)) == 0)
    goto dr_set_stripes; /* loop/jump if not special road */
  if ((Clane_byte & (1 << 7)) != 0)
    goto dr_set_stripes; /* loop/jump if not dirt track */
  /* it's a tunnel */
  B = 0xFF;
  Atunnel_visible = 1;
  Ain_tunnel = 1;
  if ((Clane_byte & (1 << 2)) != 0) {
    state->dt_tunnel_distance = *IYheightptr - &state->height_table[0];
    Atunnel_visible = 1;
    B++; // 255->0
    Cin_tunnel = Atunnel_visible;
    if ((Clane_byte & (1 << 4)) != 0) {
      B--;
      Atunnel_visible++;
      Cin_tunnel--;
    }
    // $C73C
    state->dt_tunnel_visible = Atunnel_visible;
    Ain_tunnel = Cin_tunnel;
  }
  state->dr_in_tunnel = Ain_tunnel;
  goto dr_set_stripes;

  /* $C746: Road height decreasing (diff = -128..-1) */
dr_decreasing:
  Clane_byte = **IXlanesptr;
  if ((Clane_byte & (1 << 6)) == 0)
    goto dr_calc_height_delta; /* loop/jump if not special road */
  if ((Clane_byte & (1 << 7)) != 0)
    goto dr_calc_height_delta; /* loop/jump if not dirt track */
  /* it's a tunnel */
  // EX AF,AF' - preserve Aheight_diff?
  B = 0xFF;
  Atunnel_visible = 1;
  Ain_tunnel = 1;
  if ((Clane_byte & (1 << 2)) != 0) {
    state->dt_tunnel_distance = *IYheightptr - &state->height_table[0];
    Atunnel_visible = 1;
    B++; // 255->0
    Cin_tunnel = Atunnel_visible;
    if ((Clane_byte & (1 << 4)) != 0) {
      B--;
      Atunnel_visible++;
      Cin_tunnel--;
    }
    state->dt_tunnel_visible = Atunnel_visible;
    Ain_tunnel = Cin_tunnel;
  }
  // $C770
  state->dr_in_tunnel = Ain_tunnel;
  // EX AF,AF' - restore Aheight_diff?

dr_calc_height_delta:
  Cheight_diff = Aheight_diff;
  Anew_diff = **IYheightptr;
  (*IYheightptr)++;
  WRAP_INCREMENT_ASSIGN(*IXlanesptr, state->roadbuf_start);
  Anew_diff -= **IYheightptr;
  /* Conv: Z80 compares u8 result of SUB; int Anew_diff can be negative so
   * cast to u8 before comparing. Without the cast, negative diffs bypass the
   * $E0/$50 boundaries, skip the backdrop exit, and loop indefinitely. */
  if (Anew_diff >= -32 && Anew_diff <= -1)
    goto dr_small_negative_delta;
  if (Anew_diff >= 80 || Anew_diff <= -1)
    goto dr_backdrop;

dr_small_negative_delta:
  Lrow = (Lrow - 2) & 0xFF;
  Anew_diff += Cheight_diff;
  if (Anew_diff <= 0)
    goto dr_decreasing;
  Ccounter = Anew_diff;
  dr_read_lanes(state, *IXlanesptr, (u8 *)*IYheightptr, Bfill_pattern, Ccounter, DEbackbuf, Lrow); /* tail call */
  return;

dr_increasing:
  Ccounter = Aheight_diff;
  if ((u8)Aheight_diff < 0x50) {
    dr_read_lanes(state, *IXlanesptr, (u8 *)*IYheightptr, Bfill_pattern, Ccounter, DEbackbuf, Lrow); /* tail call */
    return;
  }

dr_backdrop:
  /* $C79A: large rise — fall through to backdrop fill */
  dr_start_backdrop_fill(state, DEbackbuf, Lrow); /* was FALLTHROUGH */
}

/**
 * $C79A: Backdrop copy and sky fill
 *
 * Blits dr_sky_rows of backdrop data to the ZX screen above the road, then
 * fills remaining sky rows with 0x00 (clear sky) or 0xFF (tunnel).
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     DEbackbuf Back-buffer pointer at call site (D=high byte, E=low
 *   byte). (was DE)
 * \param[in]     Lrow Unused; kept for a consistent call signature across
 *   draw_road helpers.
 */
static void dr_start_backdrop_fill(chqstate_t *state, int DEbackbuf, int Lrow)
{
  int       D;                  /* screen address high byte (was D) */
  int       E;                  /* screen address low byte after INC E (was E) */
  int       C;                  /* pixel-row nibble → 24 → sky-row count (was C) */
  int       B;                  /* backdrop bias = ~((E>>1)+C)+0x80 (was B) */
  int       A;                  /* multi-use accumulator (was A) */
  int       L_horz;             /* horizon_level low byte for sky-row clamp (was L) */
  int       carry;              /* carry/borrow flag */
  int       BC_backdrop_offset; /* row byte offset into backdrop = (24-C)*10 (was BC) */
  int       Ascroll;            /* dr_horizon_x_scroll >> 1, selects shift amount (was A) */
  const u8 *HLbackdrop;         /* pointer to first byte of the current backdrop row (was HL) */
  int       Ajump;              /* offset for copying instructions (was A) */
  int       A_col;              /* backdrop source column; reset per LD L,A via A' (was L/A') */
  int       Bloop;              /* scanline countdown = dr_sky_rows (was B in DJNZ) */
  u8       *DEscr;              /* screen destination pointer for backdrop copy (was DE in LDI) */
  const u8 *HLsrc;              /* backdrop source pointer within the current row (was HL in LDI) */
  int       i;                  /* byte index into dr_backdrop_copy_instrs stream */
  u8        DEfillpattern;      /* sky-fill byte: 0x00 open, 0xFF tunnel (was DE = $0000/$FFFF) */
  u8       *HLbackbuf;          /* screen scanline pointer in sky fill (was HL via SP) */

  NOT_USED(Lrow);

  D = (DEbackbuf >> 8) & 0xFF;
  E = (u8)(DEbackbuf + 1);

  /* $C79B-$C7A3: in-tunnel or tunnel-visible check */
  if ((state->dr_in_tunnel | state->dt_tunnel_visible) == 0) {
    C = D & 0x0F;
    B = (int)(u8)(~((u8)((u8)(E >> 1) + (u8)C)) + 0x80);

    C = 24;
    A = (int)((state->session.horizon_level >> 8) & 0xFF);
    if ((s8)(u8)A < 0)
      goto dr_blank_sky_fill;
    if (A != 0)
      goto dr_c7ca;

    /* $C7B5-$C7C9: compute sky rows from L vs B */
    L_horz = (int)(state->session.horizon_level & 0xFF);
    if ((u8)L_horz >= (u8)B)
      goto dr_c7ca;
    A = (int)(u8)((u8)L_horz - (u8)B) + C;
    carry = (A > 0xFF);
    A = (u8)A;
    if (!carry || A == 0)
      goto dr_blank_sky_fill;
    C = A;

dr_c7ca:
    A = C;
    state->dr_sky_rows = (u8)A;
    A = (int)(u8)A + (int)(u8)B;
    if ((s8)(u8)A >= 0) goto dr_c7db;
    /* $C7D2-$C7DA: negative sum — clamp sky rows */
    A = (u8)((u8)A - 127);
    A = (u8)(-(int)(s8)(u8)A);
    A = (u8)((int)(u8)A + C);
    state->dr_sky_rows = (u8)A;
    C = A;

dr_c7db:

    BC_backdrop_offset = (24 - (int)(u8)C) * 10;
    Ascroll = state->dr_horizon_x_scroll;
    carry = Ascroll & 1; // low bit becomes choice between original and shifted version
    Ascroll >>= 1; // halve the actual shift
    /* TODO(unverified): direction of this carry-selected choice between
     * stage->backdrop and pre_shifted_backdrop has not been confirmed
     * against the skool disassembly at $C7F1. Do not treat as settled
     * until checked. */
    HLbackdrop = carry ? &state->stage->backdrop[BC_backdrop_offset] : &state->pre_shifted_backdrop[BC_backdrop_offset];
    Ajump = (u8)(18 - (int)(u8)Ascroll * 2);
    state->dr_backdrop_copy_jump = (u8)Ajump;
    assert(Ajump + 18 <= 36);
    memcpy(&state->dr_backdrop_copy_instrs[0], &backdrop_copy_instrs_template[Ajump], 18);

    /* HLbackdrop already points to the correct row, so A_col starts at 0
     * and advances by 10 per scanline (each backdrop row = 10 bytes). */
    Bloop = (int)state->dr_sky_rows;
    A_col = 0;
    goto dr_c824; // jumps into loop

    /* $C813: screen pointer row-block advance */
dr_backdrop_fill_advance:
    A = (u8)(E - 32);
    carry = (E < 32);
    E = A;
    if (!carry)
      D = (u8)(D + 16);
    goto dr_copy_row;

    do {
      // EXX ($C821)
      // EX AF,AF' ($C822) — A_col is shuttled through E ($C823 LD E,A) so that
      // $C82C LD L,A can retrieve it after the exchange; E (screen column)
      // is otherwise unaffected, so the C model leaves it untouched here and
      // reads A_col directly at the point of use below.

dr_c824:
      A = D;
      D = (u8)(D - 1);
      if ((A & 0x0F) == 0)
        goto dr_backdrop_fill_advance; // up

dr_copy_row:
      /* Blit one screen row.
       * Source: HLbackdrop + A_col (restarted after each LD L,A).
       * Dest:   ADDRTOBACKBUF(D:E), advancing right per LDI. */
      DEscr = ADDRTOBACKBUF(((u8)D << 8) | (u8)E);

      /* $C82D: 18-byte copy stream (scroll-dependent INC L / LDI mix).
       * Each slot is 2 bytes: INC L = {0x2C, 0x00}, LDI = {0xED, 0xA0}.
       * INC L advances the source by 1 (skip); LDI copies one byte. */
      // Fill partial left hand section
      HLsrc = HLbackdrop + (u8)A_col;
      for (i = 0; i < 18; i += 2)
        if (state->dr_backdrop_copy_instrs[i] == 0x2C)
          /* INC L + NOP: skip one source byte */
          HLsrc++;
        else
          /* 0xED 0xA0 = LDI: copy one byte */
          *DEscr++ = *HLsrc++;
      /* The source pointer is reloaded back to HLbackdrop + A_col before
       * this section and the one below, so each restarts reading from
       * A_col rather than continuing where the previous section left off.
       * This is not a bug: each section of the row is a separately-scrolled
       * strip of the backdrop, so they legitimately share the same source
       * origin. */
      // Fill solid middle section
      *DEscr++ = *HLsrc++;
      /* Conv: 10 consecutive LDIs (byte copy, no mask, no flip table) folded
       * into memcpy — safe because the source run is contiguous and
       * non-wrapping within backdrop row storage. */
      memcpy(DEscr, HLbackdrop + (u8)A_col, 10); DEscr += 10;
      // Fill solid right hand section
      /* Conv: same reasoning as the middle section above — 10 plain LDIs
       * from a re-seeded HLbackdrop + A_col, folded into memcpy. */
      memcpy(DEscr, HLbackdrop + (u8)A_col, 10); DEscr += 10;
      /* $C86C JR (self-modified): JR target = $C86E + dr_backdrop_copy_jump →
       * copies (18 - dr_backdrop_copy_jump) / 2 LDIs from the 9-slot variable block.
       * dr_backdrop_copy_jump = 18 - scroll*2: scroll=0 → 0 LDIs, scroll=9 → 9 LDIs. */
      // Fill partial right hand section
      /* Conv: variable-length run of plain LDIs (length set by
       * dr_backdrop_copy_jump) folded into memcpy — same non-wrapping,
       * unmasked source guarantee as above. */
      memcpy(DEscr, HLbackdrop + (u8)A_col, (18 - state->dr_backdrop_copy_jump) / 2);
      A_col = (u8)(A_col + 10);
    } while (--Bloop > 0);
  }

  /* Sky fill uses the current screen address (D:E), with the column
   * advanced by 30. */
dr_blank_sky_fill:
  E = (u8)(E + 30);
  DEfillpattern = state->dr_in_tunnel ? 0xFF : 0x00;
  for (;;) {
    // This is prev_buf_row() or a variant of?
    A = D;
    D = (u8)(D - 1);
    if ((A & 0x0F) == 0) {
      A = (u8)(E - 32);
      carry = (E < 32);
      E = A;
      if (carry) return;
      D = (u8)(D + 16);
    }
    HLbackbuf = ADDRTOBACKBUF(((u8)D << 8) | (u8)E);
    memset(HLbackbuf - 30, DEfillpattern, 30);
  }
}

/**
 * $C8BE: Pre-shift backdrop
 *
 * Copies the stage backdrop bitmap to pre_shifted_backdrop[], then
 * right-rotates it in-place by one nibble (4 bits) per row. The rotation uses
 * the Z80 RRD instruction (modelled as a 4-bit right rotate through each byte
 * with the carry passing the low nibble to the next byte). The result is the
 * pre-shifted backdrop variant used when the horizontal scroll offset is in the
 * range that requires a half-byte shift.
 *
 * \param[in,out] state Pointer to game state.
 */
static void pre_shift_backdrop(chqstate_t *state)
{
  int       tmp;        /* temporary used by the RRD() macro (Conv: extracted) */
  const u8 *source;     /* pointer to stage->backdrop[]: copy source (was HL) */
  u8       *preshifted; /* pointer to pre_shifted_backdrop[]: copy and rotate target (was DE) */
  const u8 *endptr;     /* pointer to the last byte of the current row: RRD seed (was DE) */
  u8       *bmptr;      /* pointer walking preshifted[] during the RRD rotation (was HL) */
  u8        row;        /* row counter: BACKDROP_HEIGHT down to 1 (was C) */
  u8        col;        /* column counter: BACKDROP_WIDTH down to 1 (was B) */
  u8        pix;        /* carry nibble between RRD calls: initial value from endptr (was A) */

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
 * $C8E3: draw_forked_road — render one frame of the forked-road view.
 *
 * Called from draw_road when the road is in a fork. Mirrors the structure of
 * draw_road but drives five screen zones (left verge, left road, middle verge,
 * right road, right verge) instead of three. Zone widths are read from the six
 * xpos tables ($E8xx–$EDxx) at each scanline.
 *
 * On entry the Z80 performs EXX to bank the draw_road shadow context (D, E, B,
 * C, L) into the main registers, then copies ten SM operands from the draw_road
 * SM state into the local SM bytes at $CA9D–$CB65. In C these are modelled as
 * local variables (sm_CA9D etc.) re-initialised from the corresponding state
 * fields on every call.
 *
 * The banked D, E, B, C, L come from draw_road's own register state at the
 * point it dispatches into the fork/dirt-track branch ($C4E2 JR C,$C519 — taken
 * with no intervening register writes), which is the same state dr_dispatch and
 * draw_road_lanes_change receive as [Bfill_pattern], [Ccounter], [DEbackbuf]
 * and [Lrow]. Conv: rather than re-deriving these via a full EXX
 * shadow-register model in draw_road, they are threaded through as parameters,
 * exactly as they already are for the sibling dispatch calls.
 *
 * The function has two inner loops depending on whether the outer B counter is
 * zero (dfr_c923, zero-fill path) or non-zero (dfr_c963, 5-zone path). The
 * $CA00–$CA65 scanline fill (five PUSH sequences) is stubbed out and only the
 * road-marking update ($CA68–$CB2E) is implemented.
 *
 * \param[in]     state Pointer to game state.
 * \param[in]     IXlanes Road-buffer lanes pointer (was IX).
 * \param[in,out] IYheight Height table pointer; advanced once per block
 *   (was IY).
 * \param[in]     Bfill_pattern Banked initial B: fill pattern byte (was B).
 * \param[in]     Ccounter Banked initial C: horizon scanline counter (was C).
 * \param[in]     DEbackbuf Banked initial DE: back-buffer address (was DE).
 * \param[in]     Lrow Banked initial L: row index (was L).
 */
static void draw_forked_road(chqstate_t *state, const u8 *IXlanes, const u8 *IYheight,
                             int Bfill_pattern, int Ccounter, int DEbackbuf, int Lrow)
{
  u8  sm_CB65;    /* road edge thickness countdown (was $CB65) */
  u8  sm_CB36;    /* stripe-pair toggle state (was $CB36) */
  u8  sm_CB5D;    /* XOR operand for sm_CA9D update (was $CB5D) */
  u8  sm_CA9D;    /* ADD operand for single-RLCA road-mark path (was $CA9D) */
  u8  sm_CB00;    /* ADD operand for dual-RLCA road-mark path (was $CB00) */
  u8  sm_CA7A;    /* ADD operand base (was $CA7A) */
  u8  sm_CADC;    /* ADD operand base, second copy (was $CADC) */
  u8  sm_CB1C;    /* ADD operand = sm_CA7A + 1 (was $CB1C) */
  u8  sm_CABB;    /* ADD operand = sm_CA7A + 1, second copy (was $CABB) */
  u8  sm_CB40;    /* verge fill pattern (was $CB40) */
  u8  D;          /* screen address high byte (was D) */
  u8  E;          /* screen address low byte (was E) */
  u8  B;          /* inner counter: banked from Bfill_pattern, 16 after dfr_c969 init (was B) */
  u8  C;          /* per-pass scratch counter: reset every scanline pass, has no effect on loop exit (was C) */
  u8  Cdash;       /* scan-block exit counter: banked from Ccounter, decremented once per pass (was C') */
  u8  L;          /* road table byte index; decrements as road nears (was L) */
  u8  Ldash;      /* banked row index for the zone boundary reads; reloaded from L each pass (was L') */
  u8  af_prime;   /* current scanline fill pattern, banked in A' (was A') */
  u8  loop_path;  /* 0 = dfr_c923 (zero-fill) path; 1 = dfr_c963 (5-zone) */
  int A_row;      /* scanline row check: (D-1) & 0x0F; drives advance test (was A) */
  int carry;      /* carry/borrow flag for E underflow in scanline advance */
  const u8 *HLzone;   /* pointer to zone position table for current zone (was HL) */
  int A_zone;     /* zone position byte from xpos table; 0 = interpolate (was A) */
  u8  pos_E8;     /* lefthand verge end: road left boundary ($E8) */
  int A_prev;     /* previous xpos table entry, used for interpolation (was A) */
  u8  pos_EA;     /* lefthand road end: road centre boundary ($EA) */
  u8  pos_EB;     /* median verge end: road centre right boundary ($EB) */
  u8  pos_ED;     /* righthand road end: fork right boundary ($ED) */
  int z_lr;       /* lefthand road width = pos_EA - pos_E8 */
  int z_mv;       /* median verge width = pos_EB - pos_EA */
  int z_rr;       /* righthand road width = pos_ED - pos_EB */
  int z_rv;       /* righthand verge width = 15 - pos_ED */
  u8  A_rot_pat;  /* verge fill byte: af_prime rotated left one bit (was A, via A') */
  int fill_end_addr; /* Z80 address one past the fillable region: (D<<8)|(E+31) (was HL/SP) */
  u8       *SPfill;   /* backward-fill cursor for the five PUSH-chain zones (was SP) */
  int zone_widths[4]; /* zone pair counts, right to left: rv, rr, mv, lr (Conv) */
  int remaining;      /* pairs left of the 15-pair scanline budget (Conv) */
  int take;           /* pairs taken by the current zone (Conv) */
  int zi;             /* zone index for the budgeted fill loop (Conv) */
  int B_row_lo;   /* screen low byte; base for column calc (was B) */
  int D_row_hi;   /* screen high byte at marking time (was D) */
  int H_xpos_hi;  /* current xpos-table page high byte, $E8→$ED (was H) */
  const u8 *HLtbl;    /* pointer to xpos-position table row in marking section (was HL) */
  u8       *DEmark;   /* backbuffer destination for road-marking write (was DE) */
  int DEaddr;     /* candidate Z80 address for DEmark, checked before conversion (was DE) */
  int E_col;      /* screen column: (xpos >> 3) & 0x1F + B_row_lo (was E) */
  int L_gfx;      /* graphics offset: (xpos & 7) << shift + table offset (was L) */
  u8  A_xpos;     /* road x-position byte from xpos table (was A) */
  u8  A_tog;      /* toggled sm_CB36 value; drives stripe-pair branch (was A) */
  u8  A_newpat;   /* new verge fill pattern after XOR $55 (was A) */
  u8  A_nca9d;    /* new sm_CA9D value after XOR with sm_CB5D (was A) */
  u8  A_newxor;   /* new XOR operand after +$10; carry check determines edge advance (was A) */
  u8  A_old_h;    /* height at previous IYheight entry; base for difference (was A) */
  u8  A_diff;     /* height difference old−new; sign drives re-entry or backdrop (was A) */

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
   * These are draw_road's own D, E, B, C, L at the point it dispatched into
   * this branch, threaded through as parameters (see prologue). */
  D = (u8)(DEbackbuf >> 8);   /* screen high byte */
  E = (u8)(DEbackbuf & 0xFF); /* screen low byte */
  B = (u8)Bfill_pattern;      /* fill pattern byte, banked from draw_road */
  C = (u8)Ccounter;           /* horizon scanline counter, banked from draw_road */
  L = (u8)Lrow;                /* row index, banked from draw_road */
  /* Conv: the marking section ($CA68-$CB2E) contains 13 EXX instructions
   * (odd), so the loop-exit test at $CB31 actually reads shadow C', not the
   * main C decremented at $CA67/via the six LDIs. Since draw_road's own C
   * (== Ccounter) was still live in the main register just before its own
   * EXX into this context, shadow C' == main C == Ccounter at entry, so we
   * seed Cdash the same way and decrement it once per pass (see
   * dfr_after_marking) independently of C's per-pass resets below. */
  Cdash = C;

  af_prime = sm_CB40; /* EX AF,AF': current scanline fill pattern */

dfr_c915:
  if (B != 0)
    goto dfr_c95a;

  /* $C91A: B == 0 -- set jump target to dfr_c923 (zero-fill path) */
  loop_path = 0;
  /* EX AF,AF' */
  af_prime = sm_CB40;

dfr_c923:
  A_row = D;
  D--;
  A_row &= 0x0F;
  if (A_row == 0)
    goto dfr_next_scanline_c929;

dfr_c929: /* $C929: zero-fill scanline (inner road, pre-fork area) */
  /* $C929-$C93A: LD ($C92F),DE; LD DE,<SM>; HL = DE+$1F; SP = HL; HL = 0.
   * $CA57: 15 x PUSH HL (fills the same 30 bytes as the 5-zone fill below,
   * but uniformly with 0 -- there is no fork yet, so the whole scanline
   * width between the two marking columns is plain road). */
  fill_end_addr = ((int)D << 8) | (u8)(E + 31);
  if (VALID_BACKBUF_ADDR(fill_end_addr) && VALID_BACKBUF_ADDR(((int)D << 8) | E)) {
    SPfill = ADDRTOBACKBUF(fill_end_addr);
    memset(SPfill - 30, 0, 30);
  }
  /* $C937-$C93A: HL' = 0 (the PUSH fill value) and C' = L' = 0 — all on the
   * banked side. Conv: a previous translation zeroed the MAIN row index L
   * here, scrambling every zone and marking read for the rest of the frame;
   * main L must survive untouched. Shadow C' is modelled by C (scratch). */
  C = 0;
  /* $CA57 falls through the 15 PUSHes into the shared $CA66 marking
   * code below -- must not skip straight to dfr_after_marking, or the
   * marking section (and its real C-- accounting) never runs and the
   * scan-block counter never reaches zero. */
  goto dfr_ca66;

dfr_next_scanline_c929:
  carry = (E < 0x20);
  E     = E - 0x20;
  if (!carry)
    D = D + 0x10;
  goto dfr_c929;

dfr_c95a: /* $C95A: B != 0 -- set jump target to dfr_c963 (5-zone path) */
  loop_path = 1;
  /* EX AF,AF' */
  af_prime = sm_CB40;

dfr_c963:
  A_row = D;
  D--;
  A_row &= 0x0F;
  if (A_row == 0)
    goto dfr_next_scanline_c969;

dfr_c969: /* $C969: 5-zone fork scanline render */
  /* B=16 inner counter, C=$F8 scan-block mask */
  B = 16;
  C = 0xF8; /* Conv: this is the banked C used to drive the scan-block loop */
  /* $C96D-$C96F: LD A,L; EXX; LD L,A — the zone boundary reads run on the
   * BANKED L', reloaded from main L every pass. Boundary 4's DEC L ($C9F0)
   * therefore does not persist into the next pass or the marking section,
   * which runs on main L after the $CA6A EXX. */
  Ldash = L;

  /* $C973: H = $E8 → read zone boundaries from road tables $E8/$EA/$EB/$ED.
   * Conv: the Z80 walks H as $E8, INC H ×2 → $EA, INC H → $EB, INC H ×2 →
   * $ED. During a fork the left road spans left..centre ($E8..$EA), the
   * median gap centre..centre-right ($EA..$EB) and the right road
   * centre-right..fork-right ($EB..$ED). A previous translation misread
   * the INC H counts as $E8/$E9/$EA/$EC, garbling every fork scanline. */

  /* ---- Boundary 1: lefthand verge end (table $E800, road left) ---- */
  HLzone = (const u8 *)state->xpos_road_left;
  A_zone   = HLzone[Ldash];
  if (A_zone != 0) {
    pos_E8 = (A_zone & 0x80) ? 0 : 15;
  } else {
    /* dfr_c984: DEC L; A=HL[-1]; INC L; AND $F8; 3×RRCA; RRA; ADC; CP B; DEC */
    A_prev   = HLzone[(u8)(Ldash - 1)];
    pos_E8 = (u8)((A_prev >> 4) + ((A_prev >> 3) & 1)); /* 3×RRCA + RRA + ADC */
    if (pos_E8 >= 16)
      pos_E8--;
  }

  /* ---- Boundary 2: lefthand road end (table $EA00, road centre) ---- */
  HLzone = (const u8 *)state->xpos_road_centre;
  A_zone   = HLzone[Ldash];
  if (A_zone != 0) {
    pos_EA = (A_zone & 0x80) ? 0 : 15;
  } else {
    /* dfr_c9aa: DEC L; A=HL[-1]; INC L; AND $F8; 3×RRCA; RRA (no ADC) */
    A_prev   = HLzone[(u8)(Ldash - 1)];
    pos_EA = (u8)((A_prev & 0xF8) >> 4); /* 3×RRCA + RRA, no rounding */
  }

  /* ---- Boundary 3: median verge end (table $EB00, road centre right) ---- */
  HLzone = (const u8 *)state->xpos_road_centre_right;
  A_zone   = HLzone[Ldash];
  if (A_zone != 0) {
    pos_EB = (A_zone & 0x80) ? 0 : 15;
  } else {
    /* dfr_c9c9: DEC L; A=HL[-1]; INC L; AND $F8; 3×RRCA; RRA; ADC; CP B; DEC */
    A_prev   = HLzone[(u8)(Ldash - 1)];
    pos_EB = (u8)((A_prev >> 4) + ((A_prev >> 3) & 1));
    if (pos_EB >= 16)
      pos_EB--;
  }

  /* ---- Boundary 4: righthand road end (table $ED00, fork right) ---- */
  HLzone = (const u8 *)state->xpos_road_fork_right;
  A_zone   = HLzone[Ldash];
  if (A_zone != 0) {
    pos_ED = (A_zone & 0x80) ? 0 : 15;
  } else {
    /* dfr_c9f0: DEC L; A=HL[-1] (no INC L -- L' stays decremented, but only
     * on the banked side; main L must not change or every following row's
     * reads flip parity and the road vanishes towards the horizon) */
    Ldash--;
    A_prev   = HLzone[Ldash]; /* read from new L' (= L'-1) */
    pos_ED = (u8)((A_prev & 0xF8) >> 4); /* 3×RRCA + RRA, no rounding */
  }

  /* Derive zone widths from cumulative positions; the lefthand verge width
   * (pos_E8) is taken as the fill budget remainder below */
  z_lr = pos_EA - pos_E8;               /* lefthand road */
  z_mv = pos_EB - pos_EA;               /* middle verge */
  z_rr = pos_ED - pos_EB;               /* righthand road */
  z_rv = 15    - pos_ED;                /* righthand verge */

  /* $CA00-$CA65: Fill scanline right-to-left using SP as pointer.
   * DE = current scanline address; HL = DE+$1F (truncating low-byte add,
   * no carry into H -- always safe since E is 32-byte row aligned); SP = HL.
   * EX AF,AF': rotate stripe fill pattern ($CA09-$CA0D).
   * BC = 0 (road fill = black); HL = rotated stripe pattern (both bytes).
   * Five jump-table JRs (self-modified) skip into PUSH sequences:
   *   $CA11 JR → 0..15 × PUSH HL  = righthand verge  (z_rv pairs)
   *   $CA22 JR → 0..15 × PUSH BC  = righthand road   (z_rr pairs)
   *   $CA33 JR → 0..15 × PUSH HL  = middle verge     (z_mv pairs)
   *   $CA44 JR → 0..15 × PUSH BC  = lefthand road    (z_lr pairs)
   *   $CA55 JR → 0..15 × PUSH HL  = lefthand verge   (z_lv pairs)
   * Total = 15 pairs = 30 bytes; scanline is 32 bytes -- the leftmost and
   * rightmost bytes are left untouched here for the marking section below.
   * Conv: PUSH decrements SP before writing, so N pushes fill 2N bytes
   * before the pointer, not after it (see translation-pitfalls.md #34). */
  A_rot_pat = (u8)((af_prime << 1) | (af_prime >> 7));
  /* $CA09/$CA0D EX AF,AF' pair: the RLCA result is banked BACK into A', so
   * the pattern rotates once per scanline (0x55 <-> 0xAA), producing the
   * stippled verge. Without this write-back every scanline used the same
   * byte and the verges rendered as solid vertical stripes. */
  af_prime = A_rot_pat;
  fill_end_addr = ((int)D << 8) | (u8)(E + 31);
  /* Conv: D can drift below BACKBUFFER_START_ADDRESS over enough scanlines
   * (see D-- in dfr_c923/dfr_c963); skip the write rather than let
   * ADDRTOBACKBUF assert, mirroring the marking-section guard below. */
  /* Conv: the Z80 enters five fixed 15-PUSH chains via self-modified JR
   * displacements, so a scanline always receives exactly 15 pairs; when
   * the zone positions cross (transient rows during dirt/fork
   * transitions) a JR overshoots its own chain and the pairs land in a
   * garbled but bounded pattern. In C the u8 width subtraction wrapped to
   * ~250 and the memsets ran hundreds of bytes backwards out of the
   * backbuffer, scribbling the 0xAA stripe pattern over road_buffer (the
   * draw_scene_objects Aobj=170 assert). Model the structural bound with
   * a 15-pair budget: each zone takes at most its (non-negative) width,
   * right to left, and the lefthand verge takes whatever remains, so the
   * scanline is always fully filled and the cursor can never escape. */
  if (VALID_BACKBUF_ADDR(fill_end_addr) && VALID_BACKBUF_ADDR(((int)D << 8) | E)) {
    zone_widths[0] = z_rv; /* righthand verge (pattern) */
    zone_widths[1] = z_rr; /* righthand road (black) */
    zone_widths[2] = z_mv; /* middle verge (pattern) */
    zone_widths[3] = z_lr; /* lefthand road (black); z_lv = remainder */
    SPfill = ADDRTOBACKBUF(fill_end_addr);
    remaining = 15;
    for (zi = 0; zi < 4; zi++) {
      take = zone_widths[zi];
      if (take < 0)
        take = 0;
      else if (take > remaining)
        take = remaining;
      remaining -= take;
      SPfill -= (size_t)take * 2;
      memset(SPfill, (zi & 1) ? 0 : A_rot_pat, (size_t)take * 2);
    }
    SPfill -= (size_t)remaining * 2;
    memset(SPfill, A_rot_pat, (size_t)remaining * 2);
  }

dfr_ca66: /* $CA66: B = E (save screen low byte); C-- -- shared by both
           * the zero-fill (dfr_c929) and 5-zone (dfr_c969) fill paths.
           * Conv: this DEC C hits main C, which the loop-exit test never
           * reads (see Cdash above) -- omitted since C has no other use
           * in this port. */
  B = E;

  /* $CA68-$CB2E: Road marking update — six fixed boundaries for the fork.
   * Boundaries step through xpos pages $E8→$ED.  Sections 1 and 4 are
   * left-edge format (AND-OR blend + LDI copy); sections 3 and 6 are
   * right-edge format (LDI copy + AND-OR blend); sections 2 and 5 are
   * lane-marking format (two direct copies).  Section 6 has a permanent
   * DEC L at $CB11 that takes effect only after its own xpos check. */
  {
    B_row_lo = B;
    D_row_hi = D;
    H_xpos_hi = 0xE8;

    /* Conv: D_row_hi can drift below BACKBUFFER_START_ADDRESS over enough
     * scanlines (see D-- in dfr_c923/dfr_c963). On real Z80 hardware that
     * is a harmless write into low ROM or unused RAM; the C port must
     * skip the write rather than let ADDRTOBACKBUF assert, mirroring the
     * DEbackbuf clamp in dr_write_scanline_unfilled. */
#define FRP_LEFT_EDGE(off) \
    HLtbl = (const u8 *)hi_to_xpostab(state, H_xpos_hi); \
    if (HLtbl && HLtbl[L] == 0) { \
      A_xpos = HLtbl[(L - 1) & 0xFF]; \
      L_gfx = ((A_xpos & 7) << 2) + (off); \
      E_col = ((A_xpos >> 3) & 0x1F) + B_row_lo; \
      DEaddr = (D_row_hi << 8) | (u8)E_col; \
      if (VALID_BACKBUF_ADDR(DEaddr)) { \
        DEmark = ADDRTOBACKBUF(DEaddr); \
        if (VALID_BACKBUF_PTR(DEmark) && VALID_BACKBUF_PTR(DEmark + 1)) { \
          if (L_gfx + 3 < (int)sizeof(edge_markings)) { \
            DEmark[0] = (DEmark[0] & edge_markings[L_gfx]) | edge_markings[L_gfx + 1]; \
            DEmark[1] = edge_markings[L_gfx + 3]; \
          } \
        } \
      } \
    }
#define FRP_LANE_MARK(off) \
    HLtbl = (const u8 *)hi_to_xpostab(state, H_xpos_hi); \
    if (HLtbl && HLtbl[L] == 0) { \
      A_xpos = HLtbl[(L - 1) & 0xFF]; \
      L_gfx = ((A_xpos & 7) << 1) + (off); \
      E_col = ((A_xpos >> 3) & 0x1F) + B_row_lo; \
      DEaddr = (D_row_hi << 8) | (u8)E_col; \
      if (VALID_BACKBUF_ADDR(DEaddr)) { \
        DEmark = ADDRTOBACKBUF(DEaddr); \
        if (VALID_BACKBUF_PTR(DEmark) && VALID_BACKBUF_PTR(DEmark + 1)) { \
          if (L_gfx + 1 < (int)sizeof(edge_markings)) { \
            DEmark[0] = edge_markings[L_gfx]; \
            DEmark[1] = edge_markings[L_gfx + 1]; \
          } \
        } \
      } \
    }
#define FRP_RIGHT_EDGE(off) \
    HLtbl = (const u8 *)hi_to_xpostab(state, H_xpos_hi); \
    if (HLtbl && HLtbl[L] == 0) { \
      A_xpos = HLtbl[(L - 1) & 0xFF]; \
      L_gfx = ((A_xpos & 7) << 2) + (off); \
      E_col = ((A_xpos >> 3) & 0x1F) + B_row_lo; \
      DEaddr = (D_row_hi << 8) | (u8)E_col; \
      if (VALID_BACKBUF_ADDR(DEaddr)) { \
        DEmark = ADDRTOBACKBUF(DEaddr); \
        if (VALID_BACKBUF_PTR(DEmark) && VALID_BACKBUF_PTR(DEmark + 1)) { \
          if (L_gfx + 2 < (int)sizeof(edge_markings)) { \
            DEmark[0] = edge_markings[L_gfx]; \
            DEmark[1] = (DEmark[1] & edge_markings[L_gfx + 1]) | edge_markings[L_gfx + 2]; \
          } \
        } \
      } \
    }

    /* Conv: each of sections 2-6 begins with INC H ($CA90/$CAAD/$CACE/
     * $CAF2/$CB0F), so the increment must happen BEFORE that section's
     * table read. A previous translation incremented after, shifting every
     * section from 2 on onto the previous section's table: the lane dash
     * drew at the left-edge position, the edges walked inward one boundary
     * and $ED (the right road's right edge) never got a marking. */
    FRP_LEFT_EDGE(sm_CA7A)
    H_xpos_hi++; FRP_LANE_MARK(sm_CA9D)
    H_xpos_hi++; FRP_RIGHT_EDGE(sm_CABB)
    H_xpos_hi++; FRP_LEFT_EDGE(sm_CADC)
    H_xpos_hi++; FRP_LANE_MARK(sm_CB00)
    /* $CB0F-$CB11: test/interpolate use the same L as section 5 left it;
     * the DEC L at $CB11 only takes effect afterwards, permanently. */
    H_xpos_hi++; FRP_RIGHT_EDGE(sm_CB1C)
    L--;                                  /* permanent decrement after section 6 */

#undef FRP_LEFT_EDGE
#undef FRP_LANE_MARK
#undef FRP_RIGHT_EDGE
  }

dfr_after_marking:
  L--;
  /* $CB30: DEC C -- this DEC lands on shadow C' due to the marking
   * section's odd EXX count (see Cdash setup above); Cdash is the
   * loop-exit counter, not the main-register C. */
  Cdash--;
  if (Cdash != 0) {
    if (loop_path == 0)
      goto dfr_c923;
    else
      goto dfr_c963;
  }

  /* C == 0: scanline block complete.
   * $CB34: EX AF,AF'  $CB35: B = A (restore fill pattern) */
  B = af_prime;

dfr_loop: {
    A_tog = sm_CB36 ^ 1;
    sm_CB36 = A_tog;
    if (A_tog != 0)
      goto dfr_cb65;
  }

  /* sm_CB36 == 0: update fill pattern and ADD operands */
  {
    A_newpat = sm_CB40 ^ 0x55;
    sm_CB40 = A_newpat;
    B       = A_newpat;
    sm_CA7A = sm_CA7A ^ 0x20;
    sm_CADC = sm_CA7A;
    sm_CB1C = sm_CA7A + 1;
    sm_CABB = sm_CB1C;
    {
      A_nca9d = sm_CA9D ^ sm_CB5D;
      sm_CA9D = A_nca9d;
      sm_CB00 = A_nca9d;
    }
  }

dfr_cb65:
  sm_CB65--;
  if (sm_CB65 != 0)
    goto dfr_cb90;

  /* sm_CB65 reached zero: advance XOR operand ($CB6E-$CB8D) */
  {
    A_newxor = sm_CB5D + 0x10;
    if (A_newxor < sm_CB5D)
      goto dfr_cb90;
    sm_CB5D = A_newxor;
    if (sm_CA9D != 0)
      sm_CA9D = A_newxor;
    sm_CA7A = sm_CA7A + 0x40;
    sm_CB65 = 5;                  /* $CB8D: reset thickness countdown */
  }

dfr_cb90: {
    A_old_h = *IYheight;
    IYheight++;
    WRAP_INCREMENT_ASSIGN(IXlanes, state->roadbuf_start);
    A_diff = (u8)(A_old_h - *IYheight);
    if (A_diff == 0) {
      L -= 2;
      goto dfr_loop;
    }
    if ((s8)A_diff > 0) {
      /* $CBC5 LD C,A: the height difference becomes the next scan-block
       * counter — each block draws A_diff scanlines then re-checks the
       * height walk. Omitting this left Cdash free-running (256-scanline
       * blocks) so the function never terminated at the backdrop and
       * repainted the whole buffer with drifted addresses. */
      Cdash = A_diff;
      /* backdrop_fill_dispatch ($CBC5): A < 0x50 → re-enter, A >= 0x50 → backdrop fill */
      if (A_diff >= 0x50) {
        /* Conv: D can drift below BACKBUFFER_START_ADDRESS over enough
         * scanlines (see D-- in dfr_c923/dfr_c963); on real Z80 hardware
         * dr_start_backdrop_fill's INC E doesn't touch D, so this address
         * carries the same drift into its own backbuffer writes. Skip the
         * call rather than let ADDRTOBACKBUF assert, mirroring the
         * marking-section guard above. */
        if (VALID_BACKBUF_ADDR(((int)D << 8) | E))
          dr_start_backdrop_fill(state, ((int)D << 8) | E, (int)L);
        return;
      }
      goto dfr_c915;
    }
    /* A_diff < 0 (negative): fall through */
    L = (u8)(L - 2);
    goto dfr_loop;
  }

dfr_next_scanline_c969:
  carry = (E < 0x20);
  E     = E - 0x20;
  if (!carry)
    D = D + 0x10;
  goto dfr_c969;
}

// $CBA4
// mystery_cba4 would go here, if we knew what it did

/**
 * $CBC5: Backdrop fill choice
 *
 * Called from draw_forked_road when the height difference is >= 0x50. The A <
 * 0x50 branch (JP C,$C915) is handled inline by the caller because dfr_c915 is
 * a label inside draw_forked_road. This function handles only the $CBCB JP
 * $C79A path (diff >= 0x50 → start backdrop fill).
 *
 * \param[in,out] state Game state.
 * \param[in]     DEbackbuf Back-buffer pointer at call site (D=high, E=low).
 * \param[in]     Lrow Road table row index at call site.
 */
static void backdrop_fill_dispatch(chqstate_t *state, int DEbackbuf, int Lrow)
{
  dr_start_backdrop_fill(state, DEbackbuf, Lrow);
}

/**
 * $CBD6: Build curve table
 *
 * Fills curvature_table[] with 22 per-row x-position deltas derived from the
 * road buffer and the perspective scale table, then calls
 * build_curve_table_fill twice to convert those deltas into the right-hand and
 * left-hand x-position tables (xpos_road_right/xpos_road_left or their [forked]
 * equivalents).
 *
 * The right-hand pass reads persp_x_scale_right and accumulates road_pos
 * forward from the current scene position. The left-hand pass adds
 * persp_x_delta_left offsets to the same curvature_table and starts from
 * road_pos − 295 (the vanishing-point offset for the left edge).
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     forked Non-zero if the road is forked; negates curvature bytes
 * and targets the fork tables instead of the main tables.
 */
static void build_curve_table(chqstate_t *state, int forked)
{
  s16       *H_righttab_end;         /* right-side xpos output table (was H, SM $CC72) */
  s16       *L_lefttab_end;          /* left-side xpos output table (was L, SM $CCA7) */
  const u8  *HL_roadbufptr;      /* curvature road buffer pointer (was HL) */
  int        C_curvature;        /* curvature byte from road buffer (was C) */
  const u8  *IY_height;          /* perspective x-scale row pointer (was IY) */
  int        A_scratch;          /* multiply scratch (was A) */
  const u16 *IX_lanes;           /* bend table pointer (was IX) */
  u8        *DE_output;          /* curvature_table write pointer (was DE) */
  int        B_iterations;       /* curvature fill loop count (was B) */
  int        DEdash_roadposacc;  /* banked road_pos accumulator (was DE') */
  int        A_curvature;        /* curvature byte for this iteration (was A) */
  int        IX_l;               /* bend table byte offset accumulator (was IXl) */
  int        HLdash_multiplied;  /* banked multiplier result (was HL') */
  int        BCdash;             /* banked bend-table entry minus road_pos (was BC') */
  u8         A_height;           /* height (was A) */
  int        carry;              /* carry from bit-7 test during multiply (carry) */
  int        DE_roadpos;         /* road position for fill calls (was DE) */
  const u8  *HL_rowptr;          /* persp_x_delta_left row pointer (was HL) */
  u8        *DE_curvature;       /* curvature_table delta write pointer (was DE) */
  int        Bdash_iterations;   /* add-loop / second fill count (was B) */
  int        DEdash_roadpos;     /* left hand (was DE') */

  // Set up table pointer to *end* of tables we're building.
  if (forked) {
    H_righttab_end = &state->xpos_road_fork_right[128];   /* was $EE00 */
    L_lefttab_end  = &state->xpos_road_centre_right[128]; /* was $EC00 */
  } else {
    H_righttab_end = &state->xpos_road_right[128]; /* was $ED00 - right table */
    L_lefttab_end  = &state->xpos_road_left[128];  /* was $E900 - left table */
  }

  HL_roadbufptr = state->roadbufptr;
  C_curvature = *HL_roadbufptr;

  IY_height = &persp_x_scale_right[FAST_COUNTER_PERSP_ROW(state)][0];

  // now need high byte of offset from base of struct, seems to be $E6 or $E7
  A_scratch = 0xE6 + ((IY_height - &persp_x_scale_right[0][0]) >> 8);
  A_scratch = scale_curvature_or_height(A_scratch, C_curvature);
  A_scratch = (128 - A_scratch) & 0xFE; // 0xFE must round to whole word
  // A expecting $7C to $82 depending on curvature (7C if bending right?)
  A_scratch = (A_scratch - 0x40) / 2; // adjust to index inward_bend_table
  assert(A_scratch >= 0 && A_scratch <= 95);
  IX_lanes = &curvature_to_xpos[A_scratch]; // table is 16-bit

  DE_output = &state->curvature_table[0];
  B_iterations = PERSP_TABLE_COLS;
  // EXX Bank
  // Conv: (s16) cast: during a fork layout_road temporarily sets road_pos to
  // road_pos ± fork_distance, which can go negative. The Z80 works mod 65536
  // throughout; the u16 field must be re-signed when loaded into a wider int.
  DEdash_roadposacc = (s16)state->scenedata.road_pos;
  // PUSH DEdash; // save on stack
  // EXX Unbank

  // Calculate curvature_table
  do {
    A_curvature = *HL_roadbufptr;
    if (forked)
      A_curvature = (-A_curvature) & 0xFF;
    assert(A_curvature >= 0 && A_curvature <= 255);

    if (++HL_roadbufptr == state->roadbuf_end)
      HL_roadbufptr = state->roadbuf_start;

    // EXX Bank

    // Z80: ADD A,IXl; LD IXl,A  -- IXl accumulates curvature; table at $E540 = $40 into page
    // Conv: Z80 IXl wraps in 8-bit; values < 0x40 index before the table (adjacent Z80 RAM).
    // Clamp to table bounds rather than letting the pointer escape the array.
    IX_l = (IX_lanes - &curvature_to_xpos[0]) * 2 + 0x40;
    IX_l = (IX_l + A_curvature) & 0xFF;
    if (IX_l < 0x40) IX_l = 0x40;
    IX_lanes = &curvature_to_xpos[(IX_l - 0x40) / 2];

    HLdash_multiplied = 0; // Initialise a multiplier result
    BCdash = *IX_lanes - DEdash_roadposacc;

    // reading first byte from table row?
    A_height = *IY_height++; // points into horizontal_e6b0

#if 1
    HLdash_multiplied = (A_height >> 2) * BCdash;
#else
    // multiplier
    carry = ((A_height & (1 << 7)) != 0);
    A_height = (A_height << 1) & 0xFF;
    if (carry) HLdash_multiplied = BCdash << 1;
    carry = ((A_height & (1 << 7)) != 0);
    A_height = (A_height << 1) & 0xFF;
    if (carry) HLdash_multiplied += BCdash;
    HLdash_multiplied <<= 1;
    carry = ((A_height & (1 << 7)) != 0);
    A_height = (A_height << 1) & 0xFF;
    if (carry) HLdash_multiplied += BCdash;
    HLdash_multiplied <<= 1;
    carry = ((A_height & (1 << 7)) != 0);
    A_height = (A_height << 1) & 0xFF;
    if (carry) HLdash_multiplied += BCdash;
    HLdash_multiplied <<= 1;
    carry = ((A_height & (1 << 7)) != 0);        /* bct_mult4 */
    A_height = (A_height << 1) & 0xFF;
    if (carry) HLdash_multiplied += BCdash;
    HLdash_multiplied <<= 1;                       /* bct_mult5 */
    carry = ((A_height & (1 << 7)) != 0);
    A_height = (A_height << 1) & 0xFF;
    if (carry) HLdash_multiplied += BCdash;
#endif

    HLdash_multiplied = (HLdash_multiplied >> 8) + ((HLdash_multiplied & (1 << 7)) != 0); // rounding ($CC59)

    A_curvature = HLdash_multiplied & 0xFF;
    HLdash_multiplied = (s8) A_curvature; /* sign extend */

    DEdash_roadposacc += HLdash_multiplied;

    // EXX Unbank

    *DE_output++ = A_curvature; // write to curvature_table
  } while (--B_iterations);

  DE_roadpos = (s16)state->scenedata.road_pos; /* was POP DE; (s16): see above */
  B_iterations = 0; // init counter
  // EXX Bank
  build_curve_table_fill(state,
                         H_righttab_end, // table1 is $EE00 or $ED00 (right hand table)
                         B_iterations,
                         DE_roadpos);

  // repeat of above code - generate left hand table

  HL_rowptr = &persp_x_delta_left[FAST_COUNTER_PERSP_ROW(state)][0];
  DE_curvature = &state->curvature_table[0];
  for (Bdash_iterations = PERSP_TABLE_COLS; Bdash_iterations > 0; Bdash_iterations--)
    *DE_curvature++ += *HL_rowptr++;

  DEdash_roadpos = (s16)state->scenedata.road_pos - 295; // vanishing point config (for left hand); (s16): see above

  Bdash_iterations = 0; // init counter
  // EXX Unbank
  build_curve_table_fill(state,
                         L_lefttab_end, // table2 is $EC00 or $E900 (left hand table)
                         Bdash_iterations,
                         DEdash_roadpos);
}

// HL -> points past end of destination table we're filling
static void build_curve_table_fill(chqstate_t *state,
                                   s16        *HLtableend,
                                   int         Bdash_alwayszero,
                                   int         DEroadpos)
{
  u8  *IYheight_table;
  int  Biterations;
  s16 *SPoutput;
  int  A;
  int  Bdash_iterations;
  int  Cdash;
  int  Ldash;
  int  Aopcode;
  int  Atotal;
  u16  HLdash;
  int  overflow;

  assert(Bdash_alwayszero == 0); /* Z80 B' (shadow) is always 0 at call sites */
  assert(HLtableend == &state->xpos_road_right[128]        ||
         HLtableend == &state->xpos_road_left[128]         ||
         HLtableend == &state->xpos_road_fork_right[128]   ||
         HLtableend == &state->xpos_road_centre_right[128]);
  assert(DEroadpos >= INT16_MIN && DEroadpos <= INT16_MAX);

  IYheight_table = &state->height_table[0];
  Biterations = 21;
  // (restore SP on exit, load SP with HL)
  SPoutput = HLtableend;
  do {
    // EXX Bank
    A = (Bdash_alwayszero - 2 + IYheight_table[0] - IYheight_table[1]) & 0xFF;
    IYheight_table++;
    if (A >= 128)
      goto bct_endbit_negative;
    A += 2;
    state->object_positions[IYheight_table - 1 - &state->height_table[0]] =
      A; // must write to $E34F+ which is object_positions
    A -= Bdash_alwayszero;
    Bdash_iterations = A;
    Cdash = A;
    Ldash = state->curvature_table[IYheight_table - 1 -
                                   &state->height_table[0]]; // IY[$1F]; // $E320+
    // $CCCD
    if ((Ldash & (1 << 7)) != 0) {
      Ldash = -Ldash & 0xFF; // mask here to fix neg?
      A = Bdash_iterations;
      Aopcode = Z80_DEC_DE;
      if (A < Ldash) goto bct_endbit_A;
    } else {
      Aopcode = Z80_INC_DE;
      if (A < Ldash) goto bct_endbit_A;
    }
    A = Bdash_iterations >> 1;
    do { // $CCE8
      A += Ldash;
      if (A >= Cdash) {
        A -= Cdash;
        DEroadpos += (Aopcode == Z80_INC_DE) ? +1 : -1;
      }
      if (SPoutput <= HLtableend - 128)
        goto bct_continue; /* Conv: Z80 overflows into adjacent table; cap at 128 writes */
      SPoutput--; *SPoutput = DEroadpos; // PUSH to output table
    } while (--Bdash_iterations > 0);
bct_continue:
    ; // EXX Unbank
  } while (--Biterations > 0);
  return;

  // #REGa is opcode of instruction (INC DE/DEC DE)
  // #REGb is max iterations
  // #REGc is ?
  // #REGl is ?
  // #REGde is ?
bct_endbit_A:
  /* Conv: Z80 $CCF8 LD ($CCFC),A stores the opcode into the self-modifying
   * instruction. A already holds Z80_DEC_DE or Z80_INC_DE (set just before JR C,$CCF8).
   * Aopcode was set to the same value before the goto, so no overwrite here. */
  Atotal = 0;
  do {
    do {
      if (Aopcode == Z80_INC_DE) DEroadpos++;
      else DEroadpos--;
      Atotal += Cdash;
      overflow = Atotal > 0xff;
      Atotal &= 0xff;
    } while (!overflow && Atotal < Ldash);
    Atotal -= Ldash;
    Atotal &= 0xff; /* Z80 SUB L wraps; without mask Atotal goes negative across outer iters */
    if (SPoutput > HLtableend - 128) {
      SPoutput--; *SPoutput = DEroadpos; // PUSH to output table
    }
  } while (--Bdash_iterations);
  goto bct_continue;

bct_endbit_negative:
  state->object_positions[IYheight_table - 1 - &state->height_table[0]] = 1;
  if (++A != 0) A++;
  Bdash_iterations = A;
  A = state->curvature_table[IYheight_table - 1 -
                             &state->height_table[0]]; // IY[$1F]; // $E320+
  //Ldash = A;
  //carry = (A & (1<<7) != 0;
  //Hdash = -carry; //sign ext
  HLdash = (s8) A;
  HLdash += DEroadpos;
  DEroadpos = HLdash; /* was EX */
  if (SPoutput > HLtableend - 128)
    { SPoutput--; *SPoutput = DEroadpos; } // PUSH to output table
  goto bct_continue;
}

/**
 * $CD3A: Build the per-row screen-height lookup table
 *
 * Called once per frame from the main loop. Uses the current road-buffer height
 * channel and the perspective Y-scale table to produce three outputs:
 *
 * Phase 1 — bht_loop ($CD64, 21 iterations): IY walks the road buffer height
 * channel one step per iteration. For each row, Cmin accumulates the incline
 * values seen so far. The perspective scale for the row (from persp_y_scale) is
 * multiplied by |Cmin| using the 7-bit shift-and-add routine at $CD84 (negating
 * the multiplier when Cmin is negative) and added to the base scale entry. The
 * result is written to height_table[1..21], with 0xA0 as a sentinel at
 * height_table[22].
 *
 * Phase 2 — bht_loop2 ($CDC0, 21 iterations): Copies height_table[1..21] to
 * clamped_heights[0..20], tracking the running minimum (initialised to 96). Any
 * entry below the current minimum replaces it, so the table never rises above
 * the closest road point seen so far.
 *
 * Phase 3 — horizon delta ($CDCB): Rounds Cmin down to the nearest multiple of
 * 8, writes it to horizon_attr[0] (overwriting the previous frame's value), and
 * writes the difference to horizon_attr[1]. update_screen reads horizon_attr[2]
 * (the one-frame-lagged delta) to scroll the sky/ground colour boundary.
 *
 * Conv: IY is used in the Z80 for the height-channel road buffer pointer; C
 * uses ROADBUF_FWD2PTR(ROADBUF_HEIGHT_OFFSET) for the same address. Conv: EXX
 * at $CD63 banks C (Cmin) and HL (HLpvtab) into shadow registers so the inner
 * loop can use B, DE and HL freely; C locals need no banking. Conv: The 7-bit
 * shift-and-add multiply at $CD84–$CDA9 is replaced by the equivalent
 * expression ((A_height & 0x7F) * DE_v) >> 7. Each of the 7 bits of A
 * contributes DE × 2^(bit_position); summing and taking the high byte gives (A
 * & 0x7F) * DE / 128.
 *
 * \param[in] state Pointer to game state.
 */
static void build_height_table(chqstate_t *state)
{
  u8       *IYroadbuf;     /* road buffer height-channel pointer (was IY) */
  int       C_heightbyte;  /* height byte read on entry; multiply input (was C) */
  int       A_counter;     /* fast_counter & 0xE0; perspective row selector and multiply arg (was A/B) */
  const u8 *HLpvtab;      /* pointer into perspective Y-scale table $E6xx (was HL) */
  int       Cmin;          /* incline accumulator; becomes running minimum in phase 2 (was C) */
  int       Bdash_iters;   /* bht_loop iteration count, 21 (was B') */
  u8       *DEphtab;       /* pointer walking height_table[1..21] (was DE') */
  int       DE_v;          /* perspective scale entry × 2; shift-multiply input (was DE) */
  u8        A_height;      /* scaled pixel row height written to height_table (was A) */
  u8       *HLdst;         /* walks clamped_heights[0..20] then horizon_attr[0..1] (was HL) */
  const u8 *DEsrc;         /* source pointer walking height_table in phase 2 (was DE) */
  int       B_iters;       /* bht_loop2 iteration count, 21 (was B) */

  IYroadbuf    = ROADBUF_FWD2PTR(ROADBUF_HEIGHT_OFFSET);
  C_heightbyte = *IYroadbuf;

  A_counter = state->fast_counter & 0xE0;
  HLpvtab   = &persp_y_scale[FAST_COUNTER_PERSP_ROW(state)][1];
  Cmin      = -scale_curvature_or_height(A_counter, C_heightbyte);

  /* Conv: EXX at $CD63 — Cmin stays in C; HLpvtab stays in HL (shadow).
   * B' = 21 and DE' = &height_table[1] are loaded into main registers. */
  Bdash_iters = 21;
  DEphtab     = &state->height_table[1];
  do {
    /* $CD69 — unbank: HL (HLpvtab) and C (Cmin) restored from shadow */
    DE_v     = *HLpvtab * 2;
    Cmin     = A_height = Cmin + *IYroadbuf;
    if (Cmin != 0) {
      if ((s8) Cmin < 0) {
        DE_v     = -DE_v;               /* $CD7C–$CD80 negate DE */
        A_height = -Cmin;
      }
      /* Conv: Z80 shift-and-add multiply ($CD84–$CDA9): result H =
       * (A & 0x7F) * DE / 128.  C uses the equivalent direct expression. */
      A_height = ((A_height & 0x7F) * DE_v) >> 7; /* $CD84 bht_multiplier */
    }
    A_height += *HLpvtab;
    HLpvtab++;

    *DEphtab = A_height;
    DEphtab++;
    WRAP_INCREMENT_ASSIGN(IYroadbuf, state->roadbuf_start);
  } while (--Bdash_iters > 0);

  *DEphtab = 0xA0; /* $CDB4–$CDB6 sentinel */

  /* Phase 2 — $CDB7 bht_loop2: copy to clamped_heights, tracking minimum */
  HLdst   = &state->clamped_heights[0];
  DEsrc   = &state->height_table[1];
  B_iters = 21;
  Cmin    = 96;
  do {
    A_height = *DEsrc;
    if (A_height < Cmin)
      Cmin = A_height;           /* update running minimum */
    *HLdst = Cmin;
    HLdst++;
    DEsrc++;
  } while (--B_iters > 0);

  /* Phase 3 — $CDCB: round Cmin to multiple of 8 and write horizon delta */
  Cmin     = A_height = (Cmin + 3) & 0xF8;
  A_height -= *HLdst;                       /* delta from last frame */
  *HLdst   = Cmin;                          /* horizon_attr[0] */
  HLdst++;
  *HLdst   = A_height;                      /* horizon_attr[1] */
}

/**
 * $CDD6: Scale a curvature or height byte: multiply the top three bits of A
 * by C, divide by 8, with rounding
 *
 * Three iterations of RL E / conditional ADD A,C / ADD A,A extract bits 7, 6
 * and 5 of the multiplier one at [a] time and accumulate their contribution to
 * the product. After the loop, four arithmetic right shifts (RRA + SRA×3) with
 * rounding via ADC A,$00 reduce the result to [a] single byte.
 *
 * Callers pass multiples of $20 (top three bits of fast_counter, or [a] high
 * byte derived from the road-buffer height channel) as the multiplier, so the
 * loop always operates on exactly those three significant bits.
 *
 * Conv: Z80 RRA shifts through the carry produced by the final ADD A,A. The C
 * translation treats RRA as [a] plain >>1 (carry ignored), which introduces [a]
 * rounding difference of at most 1 ULP on the intermediate value — well within
 * the precision already discarded by the final SRA×3.
 *
 * \param[in] a Multiplier; only bits 7, 6 and 5 are used. (was A)
 * \param[in] c Multiplicand. (was C)
 * \return Rounded result of (([a] & 0xE0) >> 5) * [c] / 8, as [a] signed byte.
 */
static int8_t scale_curvature_or_height(int8_t a, int8_t c)
{
  int B_iters; /* iteration count, 3 (was B) */
  int E_copy;  /* destructible copy of multiplier (was E) */
  int carry;   /* carry flag (carry) */

  B_iters = 3;
  E_copy  = a;
  a       = 0;
  do {
    carry  = (E_copy >> 7) & 1; /* RL E — shift MSB into carry */
    E_copy <<= 1;
    if (carry) a += c;          /* ADD A,C */
    a <<= 1;                    /* ADD A,A */
  } while (--B_iters);
  a    >>= 1;        /* RRA — undo final doubling */
  a    >>= 2;        /* SRA A; SRA A */
  carry  = a & 1;
  a      = (a >> 1) + carry; /* SRA A; ADC A,$00 — round */
  return a;
}

/**
 * $E810: 48K game entry point
 *
 * Initialises the game for 48K mode. Sets the 128K mode flag to zero and calls
 * the common entry path with a relocation count of three.
 *
 * In the Z80 version the mode flag is set via XOR A, B is loaded with 3, then
 * JP $E81D transfers control to entry_common.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 uses JP $E81D; C calls entry_common directly.
 */
static void entry_48k(chqstate_t *state)
{
  entry_common(state, 0, 3);
}

/**
 * $E816: 128K game entry point
 *
 * Initialises the game for 128K mode. Clears the playfield attribute file, sets
 * the 128K mode flag to one and calls the common entry path with a relocation
 * count of five.
 *
 * In the Z80 version the code falls through from $E816 into entry_common at
 * $E81D after storing A (mode flag) and B (relocation count).
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 falls through to $E81D; C calls entry_common explicitly.
 */
static void entry_128k(chqstate_t *state)
{
  clear_playfield_attrs(state);
  entry_common(state, 1, 5);
}

/**
 * $E81D: Entry common
 *
 * Shared entry point reached from both entry_48k and entry_128k. Records the
 * hardware mode, copies the marquee bitmap and attribute data to the screen,
 * optionally shows the "STOP THE TAPE" prompt (48K only), performs the Z80
 * table relocations (omitted in the C port), loads stage 1 data, and calls
 * bootstrap to complete initialisation.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Amode_128k Non-zero when running on 128K hardware. (was A)
 * \param[in]     Bnrelocs Number of relocation entries to copy. (was B)
 *
 * Conv: The relocation loop copies blocks of the loaded game binary between
 * RAM pages at fixed Z80 addresses ($8014, $83B5, $EA00, $EB00, $EC00). In
 * the C port these tables are compiled-in constants, so the relocation
 * block is omitted. Similarly, the stop_the_tape_48k call is a no-op stub
 * since there is no tape to load.
 */
static void entry_common(chqstate_t *state, int Amode_128k, int Bnrelocs)
{
#if 0
  static const struct Relocations {
    const u8 *src;
    ptrdiff_t dst;
    size_t    len;
  } relocations[] = {
    { transitions, offsetof(chqstate_t, transitions_ec00), sizeof(transitions) },
    { square_transition_frames, 0xEB00, sizeof(square_transition_frames) },
    { diamond_transition_frames, 0xEA00, sizeof(diamond_transition_frames) },
    // { 0xF220, 0x8014, 926 }, // copies load_stage_128k into place
    // { 0xE876, 0x83B5, 24 },  // copies hooks_128k
  };

  const struct Relocations *reloc; /* was HL */
  u8                   iterations; /* was BC */
#endif

  NOT_USED(Bnrelocs);

  state->mode_128k = Amode_128k;

  memcpy(ADDRTOSCREEN(SCREEN_START_ADDRESS), marquee_initial,
         sizeof(marquee_initial));
  memcpy(ADDRTOATTRS(SCREEN_ATTRIBUTES_START_ADDRESS), marquee_attrs,
         sizeof(marquee_attrs));
  state->speccy->draw(state->speccy, NULL); /* Conv: added — full-screen marquee copy */

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
  state->wanted_stage_number = 1;
  state->current_stage_number = 0;
  load_stage(state);

  bootstrap(state);
}

/**
 * $E8FE: Display "Stop the tape" and collect the controller choice
 *
 * Initialises interrupts and music, clears the screen and shows the "STOP THE
 * TAPE / PRESS ANY KEY" message to prompt the user to stop the cassette player.
 * After a key press and release, an input selection menu (Sinclair, Cursor or
 * Kempston joystick, or keyboard) is displayed and the chosen controller is
 * stored in controls_selected.
 *
 * Only reached in 48K mode; entry_128k does not call this function.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Removed. The C host loads the game directly without tape loading, so
 * neither the tape prompt nor the controller menu is needed.
 */
void stop_the_tape_48k(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $EBF7: Draw a NUL-terminated sequence of menu string records
 *
 * Calls menu_draw_string repeatedly, advancing through the record list, until a
 * zero byte is found at the start of the next record.
 *
 * In the Z80 version this is a tight three-instruction loop: CALL $EBFF; LD
 * A,(HL); AND A; RET Z; JR loop. C uses a do-while to preserve the Z80 "call
 * first, check after" ordering.
 *
 * \param[in] state Pointer to game state.
 * \param[in] strings Pointer to the first menu string record. (was HL)
 *
 * Conv: Z80 loop uses JR and RET Z; C uses a do-while. Behaviour is identical:
 * the terminator check follows each draw call.
 */
void menu_draw_strings(chqstate_t *state, const u8 *strings)
{
  do
    strings = menu_draw_string(state, strings);
  while (*strings != 0);
}

/**
 * $EBFF: menu_draw_string — draw one packed string record from the menu table.
 *
 * Reads a packed record from [HLstring]: one attribute byte (top bit =
 * double-height flag), a two-byte little-endian screen address, then ASCII
 * character bytes with bit 7 set on the last character. The double-height flag
 * is extracted from the attribute byte via RL C / EX AF,AF' / SRL C before the
 * character loop begins.
 *
 * The loop calls menu_draw_char for each character, passing screen and
 * attribute addresses as C pointers; the addresses are re-derived from the
 * updated pointers after each call.
 *
 * \param[in] state Pointer to game state.
 * \param[in] HLstring Pointer to packed string record: attribute byte, screen
 *   address
 * lo, screen address hi, then character bytes (bit 7 set on the last character)
 * (was HL).
 * \return             Pointer to the byte past the terminating character.
 */
const u8 *menu_draw_string(chqstate_t *state, const u8 *HLstring)
{
  u8   C_attribute; /* attribute byte; top bit is double-height flag (was C) */
  int  banked_carry; /* double-height flag saved across EX AF,AF' (was F') */
  u16  DEscr;       /* ZX Spectrum screen address read from string (was DE) */
  u16  HLattr;      /* ZX Spectrum attribute address derived from DEscr (was HL) */
  int  A_ascii;     /* ASCII character code masked from string byte (was A) */
  u8  *DEscr_ptr;   /* C pointer form of DEscr for each character draw (was DE') */
  u8  *HLattr_ptr;  /* C pointer form of HLattr for each character draw (was HL') */

  C_attribute = *HLstring;
  /* top bit -> banked_carry, then stripped */
  banked_carry = C_attribute >> 7;
  C_attribute &= 0x7F;
  HLstring++;
  DEscr = wordat(HLstring);
  HLstring += 2;
  // PUSH HLstring

  /* $EC0B: Calculate attribute address from screen address */
  HLattr = (0x5800 + ((DEscr >> 3) & 0x0300)) | (DEscr & 0xFF);
  // EXX - Bank (shadow HL' = HLattr, shadow DE' = DEscr)
  // EX (SP),HLstring
  // PUSH DEdash, BCdash
  do {
    A_ascii = *HLstring & 0x7F;
    // PUSH HLstring
    // Conv: Z80 uses DE'/HL' directly as pointers; C converts between u16 address
    //       and u8* at each character step via ADDRTOSCREEN/ADDRTOATTRS/SCREENTOADDR
    DEscr_ptr  = ADDRTOSCREEN(DEscr);
    HLattr_ptr = ADDRTOATTRS(HLattr);
    menu_draw_char(A_ascii,
                   banked_carry,
                   C_attribute,
                   DEscr_ptr,
                   HLattr_ptr,
                   &DEscr_ptr,
                   &HLattr_ptr);
    DEscr  = SCREENTOADDR(DEscr_ptr);
    HLattr = ATTRSTOADDR(HLattr_ptr);
    // POP HLstring
  } while ((*HLstring++ & EOS) == 0);
  // EXX - Unbank
  // POP BC, DE, HLstring
  // EXX - Bank

  return HLstring;
}

/**
 * $EC2C: menu_draw_char — render one character into the screen and attribute
 * buffers.
 *
 * Maps the ASCII character to a glyph index, then copies the 8×7 font data into
 * the screen buffer. A space advances both pointers by one column without
 * writing pixels. All other characters are routed through a range table to a
 * glyph index covering punctuation, digits and letters.
 *
 * Two rendering modes: double-height (rows duplicated across two attribute
 * rows, BRIGHT set on top row) and single-height (seven scanlines, one
 * attribute row). The mode is selected by [Fdash] (was the carry in F' from the
 * caller's EX AF,AF').
 *
 * The Z80 function operates on the banked shadow register set (F', C', DE',
 * HL') set up by the caller via EXX / EX AF,AF'. In C these are plain
 * parameters; the EXX / PUSH / POP sequences are collapsed to direct
 * assignments.
 *
 * \param[in]  Achar ASCII character to draw (was A).
 * \param[in]  Fdash Non-zero = single height; zero = double height
 *   (was carry/F').
 * \param[in]  Cdash Screen attribute byte (was C').
 * \param[in]  DEdash Screen pixel address; one column is added on entry
 *   (was DE').
 * \param[in]  HLdash Screen attribute address (was HL').
 * \param[out] DEdash_out Updated screen pixel address after drawing (was DE').
 * \param[out] HLdash_out Updated screen attribute address after drawing
 *   (was HL').
 */
static void menu_draw_char(int   Achar,
                           int   Fdash,
                           int   Cdash,
                           u8  *DEdash,
                           u8  *HLdash,
                           u8 **DEdash_out,
                           u8 **HLdash_out)
{
  int       Cglyphid;     /* glyph index into font array (was C) */
  const u8 *HLfont;       /* pointer to first row of current glyph in font (was HL) */
  u8       *DEscreen;     /* screen pixel write pointer (was DE) */
  int       row;          /* character row index; no Z80 equivalent (Conv: rolled loops) */
  u8       *HLdash_saved; /* attribute pointer saved before double-height row advance (was B') */

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
  // Conv: $EC68–$EC6C EXX/PUSH DE/INC E/EXX/POP DE collapsed; C advances DEdash directly
  DEdash++;
  // EXX
  // POP DE
  DEscreen = DEdash;
  if (!Fdash) { // checking banked carry here
    // double height
    for (row = 0; row < 4; row++) { // Conv: rolled
      *DEscreen = *HLfont;
      DEscreen += 256;
      *DEscreen = *HLfont++;
      DEscreen += 256;
    }
    /* $EC8B-$EC92: crosses 8-scanline group boundary. Conv: the literal Z80
     * does E += 0x1F, D -= 7 on the *register* DE (D0+7, E0+1 at this
     * point), landing on (D0, E0+32). The rolled loop above instead
     * advances DEscreen by 256 twice per font byte, so it is already 255
     * bytes further along (D0+8, E0) than the literal register state.
     * -2016 is the byte delta that lands this pointer on the same
     * (D0, E0+32) target; 0xF820 (65536-2016) is only equivalent to that
     * under 16-bit modular register arithmetic, not real pointer
     * arithmetic, so it must not be added directly to DEscreen. */
    DEscreen -= 2016;
    for (row = 0; row < 3; row++) { // Conv: rolled
      *DEscreen = *HLfont;
      DEscreen += 256;
      *DEscreen = *HLfont++;
      DEscreen += 256;
    }
    *DEscreen = 0; // final row always blank?
    // EXX
    // Conv: $ECAB LD B,L saves only L' (column); C saves full pointer for simplicity
    HLdash_saved = HLdash;
    Cdash |= ATTR_BRIGHT;
    *HLdash = Cdash; // set with bright set
    HLdash += 32; // move to next attr row
    Cdash &= ~ATTR_BRIGHT; // set with bright clear
    *HLdash = Cdash;
    HLdash = HLdash_saved + 1;
    // EXX
  } else {
    // single height
    for (row = 0; row < 7; row++) { // Conv: rolled
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
 * $ECDA: Zero the attribute and bitmap bytes of the playfield area
 *
 * Clears the lower two-thirds of the screen: 512 attribute bytes ($5900–$58FF)
 * and 4096 bitmap bytes ($4800–$57FF). The Z80 primes HL at the start of each
 * block, writes zero via LD (HL),L (the low byte is 0 for both addresses), then
 * fills the rest with LDIR. Functionally identical to clear_playfield ($88E2)
 * but called from the 128K startup and menu paths rather than in-game reset.
 *
 * \param[in] state Pointer to game state.
 */
static void clear_screen(chqstate_t *state)
{
  static const zxbox_t playfield_box = { /* lower two-thirds of screen */
    0, 0, SCREEN_WIDTH, PLAYFIELD_HEIGHT
  };

  memset(ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR), 0,
         SCREEN_ATTRIBUTES_ROWBYTES * PLAYFIELD_HEIGHT / 8);
  memset(ADDRTOSCREEN(SCREEN_PLAYFIELD_BITMAP_ADDR), 0,
         SCREEN_BITMAP_ROWBYTES * PLAYFIELD_HEIGHT);
  state->speccy->draw(state->speccy, &playfield_box); /* Conv: added */
}

/**
 * $ECF3: Redefine keys 48K
 *
 * Presents the key-redefinition menu and scans for eight consecutive key
 * presses. After each press, the keydef is recorded in temp_keydefs and its
 * name is drawn on screen at the current screen address. After all eight keys
 * are defined, the sequence is compared against shocked_keydefs; if they match,
 * test_mode is enabled and a confirmation screen is shown.
 *
 * \param[in,out] state Pointer to game state.
 */
static void redefine_keys_48k(chqstate_t *state)
{
  u16       DE_scr;       /* current screen address for key-name drawing (was DE) */
  int       B_iterations; /* outer loop: 8 keys to define; inner: 20 music ticks to wait (was B) */
  int       C_index;      /* index into temp_keydefs for the current key being defined (was C) */
  const u8 *DE_shocked;   /* pointer walking shocked_keydefs[] during the cheat check (was DE) */
  const u8 *HL_keydefs;   /* pointer walking state->temp_keydefs[] during the cheat check (was HL) */

  for (;;) {
    clear_screen(state);

    menu_draw_strings(state, &messages_redefine_keys[0]);

    DE_scr = 0x48D6;
    B_iterations = 8;
    C_index = 1;
    do {
      do {
        // PUSH HL,DE,BC
        play_music_48k(state);
        // POP BC,DE,HL

        // Wait for the keyboard to clear
      } while (~state->speccy->in(state->speccy, port_BORDER_EAR_MIC) & 0x1F);

      DE_scr = define_a_key(state, B_iterations, C_index, DE_scr);
      C_index++;
      // HL++; might be stray code
    } while (--B_iterations > 0);

    // All keys are now defined
    B_iterations = 20;
    do {
      // PUSH BC
      play_music_48k(state);
      // POP BC
    } while (--B_iterations > 0);

    // Test if keys are "SHOCKED<ENTER>"
    B_iterations = 8;
    DE_shocked = &shocked_keydefs[0];
    HL_keydefs = &state->temp_keydefs[0];
    do {
      if (*DE_shocked++ != *HL_keydefs++)
        return;
    } while (--B_iterations > 0);

    // Matched: Show the test mode screen
    state->test_mode = 1;
    clear_screen(state);
    menu_draw_strings(state, &messages_test_mode[0]);

    // Wait for any key
    for (;;) {
      play_music_48k(state);
      if (~state->speccy->in(state->speccy, port_BORDER_EAR_MIC) & 0x1F)
        break;
    }
  }
}

/**
 * $ED4D: Scan for a key press during key redefinition
 *
 * Scans all eight keyboard half-rows by iterating through IN port high bytes
 * ($FE, $FD, $FB, $F7, $EF, $DF, $BF, $7F). For each row, inverts the five key
 * bits from the IN result. If exactly one bit is set (unique key press), shifts
 * the bit out to identify the key column, then packs the row and column into
 * [Dkeydef_out] in the form kkkkkrrr. Returns non-zero if any key is found,
 * zero otherwise.
 *
 * \param[in,out] state Pointer to game state.
 * \param[out]    Dkeydef_out Receives packed key+row value: bits 7..3 = key
 *   column,
 * bits 2..0 = row. (was D)
 * \return Non-zero if a key is pressed; zero otherwise.
 */
static u8 redefine_keyscan(chqstate_t *state, u8 *Dkeydef_out)
{
  int carry;      /* carry from SRL/RLC operations (carry) */
  int Dflag;      /* sentinel: 0xFF at entry; incremented to 0 on first active row (was D) */
  int Ekeyandrow; /* packed key+row accumulator; decremented per row (was E) */
  u8  Bport_hi;   /* high byte of keyboard IN port; shifted through all eight row addresses (was B) */
  int Cport_lo;   /* low byte of keyboard IN port: constant $FE (was C) */
  int Akeys;      /* active key bits from IN: inverted and masked to five bits (was A) */
  u8  Hkeys;      /* copy of Akeys; shifted right to find the set bit column (was H) */
  int A;          /* column offset: decremented by 8 per SRL until the set bit falls out (was A) */

  NOT_USED(Dkeydef_out);

  carry = 0;

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
      } while (!carry); // Conv: fixed -- JR NC loops while carry clear
      if (Hkeys)
        return 1; // Conv: fixed -- RET NZ tests H (bits remaining) after the
                   // shift, not A (the row/column accumulator)

      Dflag = A;
    }
    Ekeyandrow--;
    RLC(Bport_hi);
  } while (carry);

  return 0; // No keys were pressed
}

/**
 * $ED6D: Define a single key
 *
 * Waits until redefine_keyscan reports a key press, checks the keydef has not
 * already been assigned, records it in temp_keydefs[Cindex], looks up the key
 * name from key_names[], draws it on screen at [DEscreen] and advances the
 * screen address to the next row. If [Bindex] == 4 (the mid-point of the list)
 * an extra row skip is inserted.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Bindex Position in the eight-key list (1..8); 4 triggers extra
 *   gap. (was B)
 * \param[in]     Cindex 1-based index of the key being defined into
 *   temp_keydefs[]. (was C)
 * \param[in]     DEscreen Z80 screen address at which the key name is drawn.
 *   (was DE)
 * \return Updated screen address after the drawn key name.
 */
static int define_a_key(chqstate_t *state, int Bindex, int Cindex,
                        int DEscreen)
{
  int       carry;        /* carry from SRL in redefine_keyscan (not used directly here) (carry) */
  u8        Dkeydef;      /* keydef byte returned by redefine_keyscan: kkkkkrrr (was D) */
  u8       *HLtmpkeys;    /* pointer walking temp_keydefs[] to check for duplicate assignments (was HL) */
  const u8 *HLkeynames;   /* pointer into key_names[] for the matched key's display string (was HL) */
  int       Biterations;  /* duplicate-check loop counter: Cindex−1 already-defined slots (was B) */
  int       Akeydef;      /* copy of Dkeydef for comparison and storage (was A) */

  // PUSH DEscreen,BC -- index
dak_loop1:
  do {
    do {
      play_music_48k(state);
      carry = redefine_keyscan(state, &Dkeydef);
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
  return DEscreen;
}

/**
 * $EDCC: DAK move down
 *
 * Advances a Z80 screen address by one character row: adds 32 to the low
 * byte (next column group), then adds 8 to the high byte (next pixel row
 * within the character cell) only if that addition overflowed -- i.e. only
 * once every 8 columns, when the low byte wraps back round. Used by
 * define_a_key to step the screen cursor between key-name slots.
 *
 * \param[in] DEscreen Z80 screen address (D = high byte, E = low byte).
 * \return Screen address of the next character row.
 *
 * Conv: fixed -- previously added 8 to D unconditionally on every call; the
 * Z80's `RET NC` at $EDD0 only takes that step when the E+=32 addition
 * overflows (JR NC / RET NC = skip on no-carry, so the D increment is
 * conditional on carry, not automatic).
 */
static u16 dak_move_down(int DEscreen)
{
  int E_sum;   /* E + 32 before truncation, to test for overflow (was A) */
  int carry;   /* carry out of the E += 32 addition (carry) */
  u8  E;       /* low byte of DEscreen: byte column offset + 32 (was E) */
  u8  D;       /* high byte of DEscreen: pixel row within third (was D) */

  E_sum = (DEscreen & 0xFF) + 32;
  carry = E_sum > 0xFF;
  E     = (u8) E_sum;

  D = (u8) (DEscreen >> 8);
  if (carry)
    D += 8;

  return (u16) ((D << 8) | E);
}

/**
 * $EE40: Configure the Z80 mode-2 interrupt vector table
 *
 * Fills the 257-byte interrupt vector table at $FD00–$FDFF with $FE (so every
 * vector points to $FEFE), then writes a JP $EF19 at $FEFE and sets I=$FD and
 * IM 2. Under mode 2 all interrupts are routed through $EF19, the ISR itself
 * (not modelled in C — see the 128K setup_im2_interrupt_table stub for the
 * equivalent 128K case).
 *
 * Conv: Z80 interrupt wiring has no equivalent in C; SDL delivers events on its
 * own thread. This function is a no-op in the C port.
 *
 * \param[in] state Pointer to game state.
 */
static void setup_interrupts(chqstate_t *state)
{
  /* Conv: no equivalent in C — SDL owns interrupt delivery */
  NOT_USED(state);
}

/**
 * $EE5E: Reset music playback to the start of the pattern list
 *
 * Clears the three SM operands that carry music state across frames
 * (drum_active, extra_delay, started), then falls through to the np_start_at_hl
 * entry point of next_pattern_at_addr with HL = $F0FE (the start of the music
 * pattern table).
 *
 * Conv: Z80 writes directly to SM operands at $EF0E, $EF01 and $EEA3; C writes
 * to the equivalent state fields. Conv: Z80 JP $EE78 is a tail call to
 * np_start_at_hl; C calls next_pattern_at_addr.
 *
 * \param[in] state Pointer to game state.
 */
static void reset_music(chqstate_t *state)
{
  state->music.drum_active = 0;
  state->music.extra_delay = 0;
  state->music.started     = 0;
  next_pattern_at_addr(state, &music_patterns[0]);
}

/**
 * $EE6E: Advance the music to the next pattern when the repeat count expires
 *
 * Decrements the SM repeat counter at $EE6F (pattern_repeats) and returns
 * immediately if repeats remain. When the counter reaches zero the function
 * falls through to np_next → np_start_at_hl to load the address of the next
 * pattern and start it.
 *
 * Conv: Z80 SM counter at $EE6F → state->music.pattern_repeats. Conv: Z80 falls
 * through via jp-less control flow; C calls next_pattern_at_addr.
 *
 * \param[in] state Pointer to game state.
 */
static void next_pattern(chqstate_t *state)
{
  if (--state->music.pattern_repeats)
    return;
  next_pattern_at_addr(state, state->music.pattern_addr); /* $EE75–$EE78 np_next */
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
 * Per-tick music driver for 48K hardware. Either initialises playback on the
 * first call or decrements the note delay counter. When the delay expires,
 * reads the next byte from the pattern stream; a zero byte advances to the
 * next pattern. Bytes with bit 6 set carry a one-tick extra delay flag. The
 * lower three bits select the instrument: 0 = silence, 1 = playdrum_2, 2 =
 * playdrum_1, 3 = noise. After dispatch, handles the extra-delay tick and
 * optionally continues drum playback; this represents one already-paced
 * tick, so there is no interrupt left to wait for at the end.
 *
 * \param[in,out] state Pointer to game state.
 */
static void play_music_48k(chqstate_t *state)
{
  int       Adelay;  /* note_delay decremented each tick; zero triggers next note (was A) */
  const u8 *HL;      /* pointer walking the current music pattern byte stream (was HL) */
  int       A;       /* raw pattern byte minus 1; zero means end-of-pattern sentinel (was A) */
  int       D;       /* copy of adjusted pattern byte; upper bits = pitch param, lower = instrument (was D) */
  int       B;       /* instrument index: lower three bits of D (was B) */
  int       Aparam;  /* pitch/parameter value: upper five bits of D, passed to instrument handler (was A) */

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
      switch (B) {
      case 1: playdrum_2(state, Aparam); return;
      case 2: playdrum_1(state, Aparam); return;
      case 3: play_noise(state, Aparam); return;
      }
    }
  }

  if (state->music.extra_delay) {
    state->music.note_delay--;
    state->music.extra_delay--;
  }

  if (state->music.drum_active == 1) {
    /* $EF10: JP Z,$EF38 — resume a drum sample suspended by an interrupt,
     * with position and length still banked in HL'/D'. Conv: unreachable in
     * C — playdrum_go sees no mid-sample interrupts, always plays to
     * completion and clears drum_active before returning. */
  } else {
    /* Conv: was pm_wait_for_interrupt(state) — this function already
     * represents one paced tick, so there is nothing left to wait for. */
  }
}

/**
 * $EF22: Playdrum 2
 *
 * Starts playback of drum sample 2 (108 bytes). Falls through to
 * playdrum_start.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Aspeed Playback speed: inner loop count per sample byte.
 *   (was A)
 */
static void playdrum_2(chqstate_t *state, int Aspeed)
{
  playdrum_start(state, Aspeed, 108, &state->drum2[0]); /* tail call */
}

/**
 * $EF29: Playdrum 1
 *
 * Starts playback of drum sample 1 (252 bytes). Falls through to
 * playdrum_start.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Aspeed Playback speed: inner loop count per sample byte.
 *   (was A)
 */
static void playdrum_1(chqstate_t *state, int Aspeed)
{
  playdrum_start(state, Aspeed, 252, &state->drum1[0]); /* was FALLTHROUGH */
}

/**
 * $EF2E: Playdrum start
 *
 * Records the drum speed and marks the drum as active, then falls through to
 * playdrum_bank_go to begin sample output.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Aspeed Inner loop count per sample byte; controls playback
 *   rate. (was A)
 * \param[in]     Dlength Total number of sample bytes to output. (was D)
 * \param[in]     HLdata Pointer to the start of the drum sample data. (was HL)
 */
static void playdrum_start(chqstate_t *state, int Aspeed, int Dlength,
                           u8 *HLdata)
{
  state->music.drum_speed = Aspeed;
  state->music.drum_active  = 1;
  playdrum_bank_go(state, Dlength, HLdata); /* was FALLTHROUGH */
}

/**
 * $EF38: Playdrum bank go
 *
 * EXX entry point that banks the sample pointer and length into shadow
 * registers before calling playdrum_go. In the Z80, EXX swaps BC/DE/HL with the
 * shadow set; the C port passes the values directly.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Ddash_length Sample length in bytes, held in D' after EXX.
 *   (was D')
 * \param[in]     HLdash_data Pointer to sample data, held in HL' after EXX.
 *   (was HL')
 *
 * Conv: Z80 EXX banks the arguments into shadow registers; C passes them
 * directly to playdrum_go, which uses the same parameter names.
 */
static void playdrum_bank_go(chqstate_t *state, int Ddash_length,
                             u8 *HLdash_data)
{
  /* EXX */
  playdrum_go(state, Ddash_length, HLdash_data);
}

/**
 * $EF39: Playdrum go
 *
 * Outputs a PCM drum sample byte-by-byte to the speaker port. For each sample
 * byte, an inner loop runs drum_speed iterations; each iteration writes bit 7
 * of the current sample byte to the EAR bit of port_BORDER_EAR_MIC, then
 * rotates the sample byte left in-place (RLC) so successive iterations output
 * successive bits — 1-bit PCM at drum_speed bits per byte. When all
 * [Dlength] bytes have been output, drum_active is cleared.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Dlength Number of sample bytes remaining to output. (was D)
 * \param[in]     HLdata Pointer to the next sample byte in state->drum1[] or
 *   state->drum2[]. (was HL)
 *
 * Conv: the RLC (HL) rotation mutates the sample data in place (only a full
 * 8-bit rotation restores it), so the drum samples live in state as mutable
 * copies of drum1_template/drum2_template. Conv: the inter-OUT delay code is
 * modelled as speccy->logtime so the host can reconstruct the bit timing.
 * Conv: C has no mid-sample interrupts, so the early-return resume path
 * never triggers and the sample always plays to completion in one call.
 */
static void playdrum_go(chqstate_t *state, int Dlength, u8 *HLdata)
{
  int carry;            /* carry flag used by RLC (carry) */
  int Bdash_iterations; /* inner loop counter: drum_speed ticks per sample byte (was B') */
  int A;                /* speaker output level: port_MASK_EAR or 0 based on sample bit 7 (was A) */

  carry = 0;
  do {
    Bdash_iterations = state->music.drum_speed; // aka speed
    do {
      A = port_MASK_EAR; // speaker bit
      // NOP
      if ((*HLdata & (1 << 7)) == 0)
        A = 0;
      state->speccy->out(state->speccy, port_BORDER_EAR_MIC, A);
      RLC(*HLdata); /* rotate sample byte in place */
      /* inter-bit cost 15+13+7+4+12+12 (bit-set path) */
      state->speccy->logtime(state->speccy, 63);
    } while (--Bdash_iterations > 0);
    HLdata++;
    /* inter-byte cost 6+4+7+13+4+10+7, less the DJNZ not-taken saving */
    state->speccy->logtime(state->speccy, 46);
    if (--Dlength == 0)
      goto pd_end_of_sample;
  } while (1);
  // EXX unbank
  return;

pd_end_of_sample:
  state->music.drum_active = 0;
}

/**
 * $F0C6: Play a noise burst
 *
 * Generates a noise burst on the speaker by running an LFSR-like update on the
 * two-byte rng_seed, then toggling the EAR/MIC outputs whenever bit 4 of the
 * result is set. The outer loop runs Eduration ticks; each tick iterates an
 * inner loop of 50 noise steps. On each step the seed bytes are updated and
 * rotated, and if bit 4 fires, two timed pulses are written to
 * port_BORDER_EAR_MIC: first high after (24 − Eduration) delay iterations,
 * then low after Eduration iterations.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     Aparam Noise duration: outer loop count and pulse timing
 *   (was A).
 *
 * Conv: Z80 drives the border port via OUT ($FE); C issues the equivalent
 * write via speccy->out and models the delay loops as speccy->logtime so the
 * host can reconstruct the pulse timing.
 */
static void play_noise(chqstate_t *state, int Aparam)
{
  int carry;       /* carry from RLC/RRC operations on seed bytes (carry) */
  int Eduration;   /* outer loop count and pulse high/low timing parameter (was E) */
  int Dinner;      /* inner loop count: 50 noise steps per tick (was D) */
  u8 *seed;        /* pointer into rng_seed[]: walked for each LFSR step (was HL) */
  int B;           /* intermediate seed byte read during LFSR update (was B) */
  u8  A;           /* LFSR result byte; bit 4 gates the speaker pulse (was A) */

  carry = 0;

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
      /* $F0C9: LFSR step through rng_seed + AND $10 (127 T-states) */
      state->speccy->logtime(state->speccy, 127);
      if (A & (1 << 4)) {
        /* $F0DE: JR Z not taken; LD A,$18; SUB E; LD B,A (7+7+4+4) + DJNZ */
        state->speccy->logtime(state->speccy,
                               22 + DJNZ_LOOP_TSTATES(24 - Eduration));
        state->speccy->out(state->speccy,
                           port_BORDER_EAR_MIC,
                           port_MASK_EAR | port_MASK_MIC);
        /* $F0EA: LD B,E; DJNZ; XOR A (4 + loop + 4) */
        state->speccy->logtime(state->speccy,
                               8 + DJNZ_LOOP_TSTATES(Eduration));
        state->speccy->out(state->speccy, port_BORDER_EAR_MIC, 0);

        state->speccy->logtime(state->speccy, 16);
      } else {
        /* $F0DE: JR Z taken; DEC D; JR NZ (12+4+12) */
        state->speccy->logtime(state->speccy, 28);
      }
    } while (--Dinner > 0);
  } while (--Eduration > 0);
}

// $F220 - load_stage_128k - merged into load_stage

/**
 * $F251: Initialise the police siren AY sound effect
 *
 * Presets the AY-3-8912 register soft copies for the alternating police siren
 * tone: channel A fine pitch 140, channel A volume 14, channel B volume 12.
 * Seeds the rotating siren pattern and enables the siren flag.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 writes only the fine (low) byte of channel A pitch to $A213; C
 * assigns the full ay_chan_a_pitch register. The Z80 stores the siren pattern
 * to a self-modifying 'LD B,n' operand at $8066 ($F271 before relocation); C
 * stores to state->siren_pattern.
 */
static void start_siren_128k(chqstate_t *state)
{
  state->ay_regs.chan_a_pitch = 140; /* Conv: full register; Z80 wrote low byte only */
  state->ay_regs.chan_a_vol   = 14;
  state->ay_regs.chan_b_vol   = 12;
  state->siren_pattern   = 0xAA;
  state->siren_enabled   = 0xAA;
}

/**
 * $F269: Advance the police siren pitch and write AY registers
 *
 * Each frame, rotates the alternating siren pattern left (RLC) to produce a
 * carry that selects the direction: carry clear → decrease pitch by 3; carry
 * set → increase pitch by 3. If the new pitch stays within the 90–139 range the
 * pattern is not updated; if it leaves the range the new pattern is committed
 * and the pitch is clamped. Writes the updated fine pitch to channels A and B
 * (B is 4 below A) and flushes all AY registers.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 stores the updated pattern to a self-modifying 'LD B,n' operand at
 * $8066; C stores to state->siren_pattern. The EX AF,AF' pair at $F285/$F28A
 * that preserves the new pitch across the SM write is unnecessary in C (locals
 * are not affected by the write).
 */
static void play_siren_sfx_128k(chqstate_t *state)
{
  int carry;   /* carry flag; set/cleared by RLC (carry) */
  u8  pitch;   /* channel A fine pitch, adjusted each frame (was A) */
  u8  pattern; /* alternating siren pattern; rotated left each frame (was B) */
  carry = 0;

  if (state->siren_enabled == 0)
    return;

  pitch = state->ay_regs.chan_a_pitch & 0xFF;
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
  state->ay_regs.chan_a_pitch = (state->ay_regs.chan_a_pitch & 0xFF00) | pitch;
  state->ay_regs.chan_b_pitch = (state->ay_regs.chan_b_pitch & 0xFF00) | (pitch - 4);
  state->ay_regs.mixer &= 0x3C; // enable tone A & B
  write_audio_registers_128k(state); /* tail call */
}

/**
 * $F29D: Silence all AY audio channels
 *
 * Sets the AY mixer register to $3F, disabling all noise and tone channels for
 * all three voices, then flushes the AY register soft copies to the hardware.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 falls through into write_audio_registers_128k; C calls it.
 */
static void silence_audio_128k(chqstate_t *state)
{
  state->ay_regs.mixer = 0x3F; /* disable all noise and tone channels */
  write_audio_registers_128k(state); /* was FALLTHROUGH */
}

/**
 * $F2A2: Flush AY-3-8912 register soft copies to hardware
 *
 * Writes registers 11 down to 0 from the AY register soft-copy block
 * (ay_regs.env_fine..ay_regs.chan_a_pitch's low byte) by selecting each register via port $FFFD
 * then writing its value via port $BFFD. The loop uses OUTD which decrements HL
 * and B after each write; the JP P condition exits when A underflows from 0 to
 * −1 (i.e. once register 0 has been written).
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 uses the OUTD instruction (LD B,$FF / OUT (C),A / LD B,$BF / OUTD
 * in sequence); C issues two separate out() calls per register.
 */
static void write_audio_registers_128k(chqstate_t *state)
{
  zxspectrum_t *speccy; /* ZX Spectrum callbacks (Conv: C-only) */
  const u8     *values; /* pointer walking AY soft copies downward (was HL) */
  u8            regno;  /* AY register index, 11 down to 0 (was A) */

  speccy  = state->speccy;
  values  = &state->ay_regs.env_fine;
  regno   = 11;
  do {
    speccy->out(speccy, 0xFFFD, regno);
    speccy->out(speccy, 0xBFFD, *values--); /* was OUTD */
  } while ((s8) --regno >= 0);
}

/**
 * $F2B6: Derive AY engine pitch and volume from current car speed
 *
 * Computes the AY channel C pitch divisor from the car's speed. The speed is
 * halved via right-rotate, complemented and then shifted left twice (×4) to
 * give a pitch inversely proportional to speed. In high gear the divisor is
 * doubled once more to lower the pitch further. A tunnel-dependent constant
 * ($0190 outside a tunnel, $0258 inside) is then added. Sets channel C pitch
 * (12-bit fine+coarse), volume and enables tone C in the mixer.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 computes ~(HL>>1) via RR H / LD A,L / RRA / CPL / LD L,A; C uses
 * ~(state->speed >> 1) on a u16 directly. The tunnel check is restructured to
 * an if-else rather than the Z80's load-default-then-overwrite pattern.
 */
static void engine_sfx_from_speed_128k(chqstate_t *state)
{
  u16 pitch;      /* AY channel C pitch divisor, derived from speed (was HL) */
  u16 base_pitch; /* base pitch divisor: tunnel vs non-tunnel constant (was DE) */
  u8  volume;     /* AY channel C volume (was A) */

  pitch = (~(state->speed >> 1)) & 0xFF;
  if (state->gear)
    pitch <<= 1; /* double divisor in high gear to lower pitch */
  pitch <<= 2;   /* quadruple divisor further */
  // Conv: tunnel check restructured to if-else; Z80 loads non-tunnel defaults then overwrites
  if (state->tunnel_sfx == 0) {
    base_pitch  = 0x190; /* non-tunnel base divisor (~277 Hz) */
    volume = 15;
  } else {
    base_pitch  = 0x258; /* in-tunnel base divisor (~185 Hz) */
    volume = 12;
  }
  state->ay_regs.chan_c_pitch = pitch + base_pitch;
  state->ay_regs.chan_c_vol   = volume;
  state->ay_regs.mixer       &= 0x3B;
}

/**
 * $F2F1: Initialise the turbo boost sound effect
 *
 * Seeds both the AY noise pitch and the turbo SFX countdown to $3C (60).
 * play_turbo_sfx_128k then decrements these each frame to produce a descending
 * noise burst before handing off to the engine sound.
 *
 * \param[in] state Pointer to game state.
 */
static void setup_turbo_sfx_128k(chqstate_t *state)
{
  state->ay_regs.noise_pitch  = 0x3C;
  state->turbo_sfx_pitch = 0x3C;
}

/**
 * $F2FA: Drive the turbo boost sound effect each frame
 *
 * Decrements the turbo SFX countdown and the AY noise pitch each frame to
 * produce a descending noise burst. When the turbo countdown reaches zero the
 * effect is complete and control falls through to the engine sound. The noise
 * pitch register drives AY channel C; once it reaches zero, tone and noise C
 * are disabled and the engine effect takes over.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 uses JP $80AA (tail call to engine_sfx_from_speed_128k after
 * relocation); C calls it directly.
 */
static void play_engine_or_turbo_sfx_128k(chqstate_t *state)
{
  if (state->turbo_sfx_pitch == 0) {
    engine_sfx_from_speed_128k(state); /* tail call */
    return;
  }

  if (state->turbo_sfx_pitch == 1)
    return;

  if (--state->ay_regs.noise_pitch) {
    state->ay_regs.chan_c_pitch = state->ay_regs.noise_pitch + 10;
    state->ay_regs.mixer &= 0x1B; // Set mixer to enable Tone C and Noise C
    state->ay_regs.chan_c_vol = 13;
    return;
  }

  state->ay_regs.mixer |= 0x24; // Set mixer to disable Tone C and Noise C
  state->turbo_sfx_pitch = 0;
  engine_sfx_from_speed_128k(state); /* tail call */
}

/**
 * $F342: Play speech 128K
 *
 * Plays one of five digitised speech samples through the AY-3-8912 DAC by
 * writing each nibble as a volume level to AY channels A, B and C in turn.
 * Silences the audio, pages in the sample bank, then iterates over
 * DEdash_length sample bytes. Each byte yields two 4-bit nibbles (high then
 * low); each nibble is written to AY registers 8, 9 and 10 via OUT (C),A
 * through port $FFFD (select) and $BFFD (write). A short delay between nibbles
 * sets the output frequency. After all samples are output, reset_paging_128k
 * restores the default memory map.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     index 1-based speech sample index (1..5). (was A)
 *
 * Conv: The Z80 uses EX AF,AF'/EXX to bank registers across the inner loops; C
 * passes the values as function parameters and locals instead.
 */
static void play_speech_128k(chqstate_t *state, int index)
{
  /* $F32E: speech sample table */
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

  zxspectrum_t *speccy;           /* cached speccy pointer (Conv: extracted) */
  int           Cport_lo;         /* low byte of AY port address: $FD (was C) */
  int           Hff;              /* AY select port high byte: $FF → port $FFFD (was H) */
  int           Lbf;              /* AY write port high byte: $BF → port $BFFD (was L) */
  int           Deight;           /* AY volume register base: 8 = channel A volume (was D) */
  int           DEdash_length;    /* number of sample bytes remaining; two nibbles each (was DE') */
  const u8     *HLdash_samples;   /* pointer walking sound_samples[] for the selected clip (was HL') */
  int           Cdash_iterations; /* nibble counter: 2 per byte (high then low) (was C') */
  u8            Asample;          /* current sample byte / nibble being output (was A) */
  int           Bport_hi;         /* AY port high byte: $FF or $BF depending on op (was B) */
  int           Aregno;           /* AY register number: 8, 9, 10 for channels A, B, C (was A) */

  speccy = state->speccy;

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
  HLdash_samples = &sound_samples[speech_samples_table[index - 1].data - SOUND_SAMPLES_Z80_BASE];

  // There are two samples per byte so we iterate here.
  do {
    Cdash_iterations = 2;
    Asample = *HLdash_samples;
    /* RR A x4: bits 0-3 of result = original bits 4-7, independent of carry-in */
    Asample >>= 4; // Get high nibble
    do {
      speccy->stamp(speccy); // stamp at $F36E, start of the per-nibble body

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

      // Sleeps out the whole per-nibble body timed from the stamp() above,
      // not just the LD B,$13/DJNZ delay loop (see SPEECH_NIBBLE_TSTATES).
      speccy->sleep(speccy, SPEECH_NIBBLE_TSTATES);

      Asample = *HLdash_samples; // Load next sample (same byte, but next nibble)
    } while (--Cdash_iterations > 0); // Decrement nibble counter
    HLdash_samples++; // Advance to next byte of sample data
  } while (--DEdash_length > 0);

  reset_paging_128k(state); /* tail call */
}

/**
 * $F39F: Celebrate the perpetrator being caught (128K mode)
 *
 * Waits until overlay_delay reaches 42, then silences audio, disables the siren
 * flag, resets the turbo SFX countdown to 1 and triggers the success music
 * sequence via bank 3.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Z80 falls through from $F3B3 (LD HL,$C006) into call_bank_3_128k; C
 * passes BANK3_SUCCESS_MUSIC explicitly. The LD ($8E4A),A at $F3B0 self-modifies
 * overlay_delay; C assigns state->overlay_delay directly. Now that
 * BANK3_SUCCESS_MUSIC blocks for BASL_JINGLE_FRAMES frames of real playback
 * (see boot_and_run_sound_loop), truncating overlay_delay to 1 here is safe:
 * the blocking call itself holds the arrest-message overlay for the
 * jingle's duration, matching the Z80.
 */
static void handle_perp_caught_128k(chqstate_t *state)
{
  int Adelay; /* overlay frame delay; return early if < 42 (was A) */

  Adelay = state->overlay_delay;
  if (Adelay < 42)
    return;

  silence_audio_hook(state);

  state->siren_enabled   = 0;
  state->turbo_sfx_pitch = 1;
  state->overlay_delay   = 1;

  call_bank_3_128k(state, BANK3_SUCCESS_MUSIC); /* was FALLTHROUGH */
}

/**
 * $F3E2: Copy 128K memory banks for audio playback
 *
 * Iterates across four 4 KB windows starting at $C000. For each window, copies
 * 4096 bytes to $B000, calls setup_engine_sfx, then replays the window
 * byte-by-byte via LDI with a write-back before calling reset_paging_128k to
 * restore the memory pager. Driven entirely by 128K hardware memory-bank
 * switching.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Removed. C has no 128K memory-paging hardware to drive.
 */
static void page_128k(chqstate_t *state)
{
  // Conv: Removed
  NOT_USED(state);
}

/**
 * $F414: Reset the 128K memory pager to bank 0
 *
 * Writes zero to the 128K paging register at port $7FFD via OUT (C),A,
 * restoring the default memory layout (ROM 0, RAM bank 0, screen 0).
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: Removed. C has no hardware OUT port for 128K memory paging.
 */
static void reset_paging_128k(chqstate_t *state)
{
  // Conv: Removed
  NOT_USED(state);
}

/**
 * $F41B: Attract mode 128K
 *
 * Top-level attract loop for 128K hardware. Calls the bouncy logo (bank 3),
 * sets up a stage, then spins in a per-frame loop calling drive_attract_demo
 * and update_screen. Each frame shows either "ENTER FOR OPTIONS" or "PRESS
 * GEAR" depending on whether a controller has been selected. ENTER detected via
 * keyboard port switches into the input-selection bank-3 routine. An
 * attract_mode_128k_blink counter produces a 4-on / 4-off text flash. When
 * transition_control reaches zero, the countdown decrements: positive values
 * show credits, zero shows best-officers, negative restarts the loop. FIRE
 * exits attract mode and starts the game.
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: Z80 uses JP for looping and bank-3 call dispatch; C uses gotos and
 * call_bank_3_128k which dispatches via switch. The RRA for ENTER detection is
 * replaced by a direct bit-0 mask.
 */
static void attract_mode_128k(chqstate_t *state)
{
  int       HL_routine;           /* bank-3 routine address constant to invoke (was HL) */
  int       A_result;             /* return value from call_bank_3_128k: 0 = early return (was A) */
  const u8 *DE_messages;          /* messages pointer: enter_for_options or press_gear (was DE) */
  int       enter_pressed;        /* non-zero when Enter key is held (was carry from RRA) */
  const u8 *HL_messages;          /* copy of DE_messages passed to print_message (was HL) */
  int       carry;                /* carry from RRC of attract_mode_128k_blink (carry) */
  int       A_transition_control; /* transition_control state: non-zero while transition is running (was A) */
  int       A_countdown;          /* attract_mode_128k_countdown: 2..negative; triggers scene restart (was A) */

attract_mode_128k_start:
  HL_routine = BANK3_TITLE_SCREEN;
call_bank_3:
  A_result = call_bank_3_128k(state, HL_routine);
  if (A_result == 0)
    return;

  set_up_stage(state, &state->stage->attract_data);

  state->attract_mode_128k_countdown = 2; // two runs through
  state->speed = INITIAL_ATTRACT_SPEED;
  for (;;) {
    if (state->host_quit)
      longjmp(state->host_quit_jmp, 1);

    drive_attract_demo(state);

    if (state->controls_selected == 0) {
      DE_messages = &enter_for_options_messages[0];
    } else {
      if (keyscan(state) & USERINPUTFLAG_FIRE) {
        play_start_noise(state);
        return;
      }
      DE_messages = &press_gear_messages[0];
    }

    enter_pressed = ~state->speccy->in(state->speccy,
                                       port_KEYBOARD_ENTERLKJH) & 1; /* Conv: was IN+RRA */
    HL_routine = BANK3_INPUT_SELECTION;
    if (enter_pressed)
      goto call_bank_3;

    HL_messages = DE_messages;
    RRC(state->attract_mode_128k_blink); /* rotating 4-on-4-off pattern */
    if (carry)
      print_message(state, *HL_messages, HL_messages);

    A_transition_control = state->transition_control;
    if (A_transition_control == 0) {
      A_countdown = state->attract_mode_128k_countdown;
      if (A_countdown < 0)
        goto attract_mode_128k_start;
      state->attract_mode_128k_countdown = --A_countdown;
      if (A_countdown < 0)
        setup_transition(state, TRANSITIONSTRIDE_FORWARD);
      else
        setup_overlay_messages(state, (A_countdown > 0) ? &credits_messages_128[0] : &best_officers[0]);
    }

    transition(state);
    update_screen(state);
  }
}

/* ----------------------------------------------------------------------- */

CHQ_API void chq_setup(chqstate_t *state)
{
  if (setjmp(state->host_quit_jmp) == 0)
    if (1)
      entry_128k(state);
    else
      entry_48k(state);
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
  while (run_pregame_screen_loop(state)) /* Conv: Split out */
    ;
}

/* ----------------------------------------------------------------------- */

#ifdef CHQ_TESTS

#include "ChaseHQ/Engine/Tests.h"

void chq_test_load_stage(chqstate_t *state)
{
  load_stage(state);
}

void chq_test_set_up_stage(chqstate_t *state)
{
  set_up_stage(state, &state->stage->stage_data);
}

void chq_test_set_up_stage_attract(chqstate_t *state)
{
  set_up_stage(state, &state->stage->attract_data);
}

u8 *chq_test_lanes_slot(chqstate_t *state)
{
  return ROADBUF_FWD2PTR(ROADBUF_LANES_OFFSET);
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

void chq_test_exit_fork(chqstate_t *state)
{
  exit_fork(state);
}

void chq_test_advance_hazards(chqstate_t *state)
{
  advance_hazards(state);
}

void chq_test_game_frame(chqstate_t *state)
{
  /* Mirrors the run_game main-loop frame ($8401), omitting input and audio
   * calls that need a live host (keyscan, check_user_input, drive_sfx and the
   * sfx hooks). Ordering matches run_game; keep in sync when the main loop
   * changes. Callers should top up time_bcd and keep hazards[0].used set to
   * HAZARD_USED as run_game's pre-loop setup does. */
  check_time_up(state);
  read_map(state);
  if (handle_perp_caught(state))
    return;
  move_hero_car(state);
  spawn_cars(state);
  cycle_counters(state);
  build_height_table(state);
  scroll_horizon(state);
  layout_road(state);
  draw_road(state);
  /* Debug aid: snapshot the road-only backbuffer (before sprites) so the
   * test can dump it for visual inspection alongside the final screen. */
  {
    extern u8 chq_test_backbuf_snapshot[];
    memcpy(chq_test_backbuf_snapshot, state->backbuffer, BACKBUFFER_LENGTH);
  }
  layout_objects(state);
  prepare_tunnel(state);
  spawn_hazards(state);
  drive_helicopter(state);
  choose_dirt_and_stones(state);
  advance_hazards(state);
  layout_dirt_and_stones(state);
  move_helicopter(state);
  check_scenery_collisions(state);
  draw_scene_objects(state);
  animate_hero_car(state);
  speed_score(state);
  update_scoreboard(state);
  calc_overtake_bonus(state);
  drive_chatter(state);
  draw_smash_bar(state);
  transition(state);
  update_screen(state);
  exit_fork(state);
}

int chq_test_max_side_object(chqstate_t *state)
{
  u8  *HLroadbuf;   /* pointer into road_buffer, as in draw_scene_objects */
  int  max;         /* largest object byte seen */
  int  Biterations; /* loop counter, as in draw_scene_objects */

  /* Replicates the draw_scene_objects right/left object scan so tests can
   * detect corrupt object ids (> 9) without running the full draw. */
  HLroadbuf = ROADBUF_FWD2PTR(115);
  max = 0;
  Biterations = 20;
  do {
    if (*HLroadbuf > max)
      max = *HLroadbuf;
    WRAP_ASSIGN(HLroadbuf, 32, state->roadbuf_start);
    if (*HLroadbuf > max)
      max = *HLroadbuf;
    WRAP_ASSIGN(HLroadbuf, -33, state->roadbuf_start);
  } while (--Biterations > 0);
  return max;
}

/* Debug aid: road-only backbuffer snapshot taken by chq_test_game_frame. */
u8 chq_test_backbuf_snapshot[BACKBUFFER_LENGTH];

void chq_test_draw_road(chqstate_t *state)
{
  draw_road(state);
}

void chq_test_draw_road_lanes_change(chqstate_t *state, u8 lane_flags, int height_offset)
{
  u8       *local_IX;
  const u8 *local_IY;

  /* Conv: IX must point into road_buffer so that WRAP_INCREMENT_ASSIGN (used by
   * the height-check secondary chain) stays within the buffer. Placing
   * lane_flags at roadbufptr works because the immediately following
   * bytes are 0 (calloc) and therefore straight; the height-check chain
   * advances IYheightptr monotonically without oscillating. */
  *state->roadbufptr = lane_flags;
  local_IX = state->roadbufptr;
  local_IY = &state->height_table[height_offset];
  draw_road_lanes_change(state, 0 /* fill_pattern */, 0 /* horizon */, 0x0100 /* DEbackbuf */,
                         0xEC /* H_left_hand_table_hi */, 0xFF /* Lrow */, &local_IX, &local_IY);
}

#endif /* CHQ_TESTS */
