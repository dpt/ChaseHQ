/**
 * Internal.h
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

#ifndef CHASEHQ_INTERNAL_H
#define CHASEHQ_INTERNAL_H

#include "ChaseHQ/ChaseHQ.h"

/* ----------------------------------------------------------------------- */

/* Configuration constants */

#define SPEED_GEAR_CHANGE          (150) /* gear-change threshold: low gear below, high gear at or above */
#define SPEED_PERP_CHASE           (350) /* perp's base chase speed; also hazard speed cap after impact */
#define INITIAL_ATTRACT_SPEED      (400) /* scripted drive speed: attract mode camera */ // HACK was 400
#define SPEED_PERP_MIN              (70) /* perp slow-down threshold in handle_perp_caught */
#define SPEED_PERP_CAUGHT          (400) /* scripted drive speed: perp post-arrest */

#define INITIAL_TIME_BCD          (0x60) /* catch-up time allocation (BCD seconds) */
#define CHASE_TIME_BCD            (0x60) /* chase-down time allocation (BCD seconds) */
#define RESTART_TIME_BCD          (0x60) /* restarted chase-down time allocation (BCD seconds) */
#define LOW_TIME_WARNING_BCD      (0x15) /* low time warning (BCD seconds) */

#define INITIAL_BOOSTS               (3) /* number of boosts player starts each stage with */
#define RESTART_BOOSTS               (3) /* number of boosts player receives on restart */

#define SMASHCOUNTER_MAX            (20) /* fully smashed; also the smash bar segment count */

#define MINSTAGE                     (1)

#ifdef CHQ_ENABLE_TEST_STAGE
#define MAXSTAGE                     (6) /* five original stages plus the port-added test level */
#else
#define MAXSTAGE                     (5) /* five original stages; port-added stage 6 test level excluded (see Stages.c) */
#endif

#define SUBSECOND_TICKS_PER_SECOND  (15) /* used to calibrate the 60s game countdown */

/* ----------------------------------------------------------------------- */

/* Timing configuration constants */

#define MAIN_LOOP_TSTATES       (354955) /* calibrated so in-game timer is 60s */
#define ATTRACT_TSTATES         (348173)
#define PREGAME_TSTATES         (288192)
#define ESCAPE_SCENE_TSTATES    (283239)
#define TITLE_MUSIC_TSTATES     (100000) // TODO: Calibrate
/* Paces titlescr_animate_frame's object-animation loop. Hand-tuned by eye
 * against the original rather than derived from a frame length, hence the
 * odd 57% scaling of the base figure. TODO: Calibrate properly. */
#define TITLE_ANIM_TSTATES      (220167 * 57 / 100)
#define KEMPSTON_MUSIC_TSTATES  (100000) // TODO: Calibrate
#define OMD_MUSIC_TSTATES       (100000) // TODO: Calibrate
#define SUCCESS_MUSIC_TSTATES   (100000) // TODO: Calibrate
#define END_SCREEN_TSTATES       (60000) // TODO: Calibrate

/* $EF13 pm_wait_for_interrupt: play_music_48k ends by spinning until the next
 * 50Hz maskable interrupt, so one call is one frame however much of it was
 * spent bit-banging a drum sample. The C port has no interrupt to spin on, so
 * the wait becomes a stamp/sleep pair across the whole function. 48K frame
 * length; the callers are 48K-mode menu loops. */
#define MUSIC_TICK_48K_TSTATES   (69888)

/* $F36E-$F393: one nibble of play_speech_128k (AND $0F .. JR NZ,$F36E),
 * summed from the skool T-state counts. Covers the three OUT (C),A triplets
 * and the LD B,$13/DJNZ delay loop, so a single stamp/sleep models the whole
 * per-nibble output rate, not just the explicit delay. */
#define SPEECH_NIBBLE_TSTATES      (375) // TODO: Calibrate

/* $E256 loop body (es_handler_draw_score, Bank7.c): one bonus-tally
 * increment, comprising increment_score, ptad_led_digits and a sfx_bipbow(2,
 * 2) tone. Dominated by sfx_bipbow's DJNZ delay loops (~37200 T-states of
 * the total); a single stamp/sleep models the whole per-increment body, not
 * just the beeper delay. */
#define SCORE_TALLY_TSTATES      (38000) // TODO: Calibrate

/* 48K beeper drums timing. The Z80 pitches its bit-banged speaker output with
 * busy-wait delay loops; the C translations do no busy-waiting and instead
 * advance the speccy's virtual T-state clock via speccy->logtime so the host
 * can reconstruct the pulse spacing. Costs are summed from the skool
 * listings and exclude the OUT ($FE) itself (11 T-states, accounted
 * centrally in zx_out). */
/* TODO: Calibrate these against the original beeper drums. */
#define DJNZ_LOOP_TSTATES(b)  (((b) - 1) * 13 + 8)  /* DJNZ-to-self, b >= 1 iterations */
#define DECJR_LOOP_TSTATES(n) (((n) - 1) * 16 + 11) /* DEC r; JR NZ,-3 self-loop, n >= 1 iterations */
#define RNG_TSTATES                (143) /* CALL $961B (17) + rng body (126) */

/* ----------------------------------------------------------------------- */

/* Screen layout constants */

#define MARQUEELIGHT_WIDTH           (5) /* attribute cells */
#define MARQUEELIGHT_HEIGHT          (4) /* attribute cells */

#define MARQUEE_WIDTH              (256) // pixels
#define MARQUEE_HEIGHT           (8 * 8) // rows
#define PLAYFIELD_HEIGHT        (16 * 8) // rows

#define SCREEN_PLAYFIELD_BITMAP_ADDR (0x4800) // first playfield scan line (char row 8)
#define SCREEN_PLAYFIELD_ATTRS_ADDR  (0x5900) // first playfield attribute (char row 8)

#define MARQUEELIGHT_LEFT_ATTR_ADDR  (0x5820) /* screen attribute address of left marquee light */
#define MARQUEELIGHT_RIGHT_ATTR_ADDR (0x583B) /* screen attribute address of right marquee light */

#define FACE_ATTRS_ADDR              (0x5836) /* screen attribute address (22,1) of the face */

#define ROAD_MARKINGS_PAGE_ADDR      (0xE400) /* base of the $E4xx road graphics page */

/* ----------------------------------------------------------------------- */

/* Backbuffer layout constants */

#define BACKBUFFER_WIDTH           (256)
#define BACKBUFFER_ROWBYTES        (BACKBUFFER_WIDTH / 8)
#define BACKBUFFER_HEIGHT          (128)
#define BACKBUFFER_LENGTH          (BACKBUFFER_ROWBYTES * BACKBUFFER_HEIGHT)
#define BACKBUFFER_START_ADDRESS   (0xF000)
#define BACKBUFFER_END_ADDRESS     (BACKBUFFER_START_ADDRESS + BACKBUFFER_LENGTH)
// Padding amount for when flushing out memory scribblers.
#define BACKBUFFER_OVERFLOW        (0)

/* ----------------------------------------------------------------------- */

#define MAXHAZARDS                   (6)

#define TURBOWIDTH                  (16) /* pixels */
#define TURBOHEIGHT                 (14)
#define TURBOFRAMES                  (3)

#define TURBOROWBYTES               (TURBOWIDTH / 8)
#define TURBOFRAMELENGTH            (TURBOROWBYTES * TURBOHEIGHT * 2) // masked

#define FACEWIDTH                   (32)
#define FACEHEIGHT                  (40)

#define FACEROWBYTES                (FACEWIDTH / 8)
#define FACEATTRHEIGHT              (FACEHEIGHT / 8)
#define FACEBITMAPBYTES             (FACEROWBYTES * FACEHEIGHT)
#define FACEATTRWIDTH               (FACEWIDTH / 8)
#define FACEATTRBYTES               (FACEATTRWIDTH * FACEATTRHEIGHT)
#define FACEBYTES                   (FACEBITMAPBYTES + FACEATTRBYTES)

#define NFACES                       (3)

/* ----------------------------------------------------------------------- */

#define HAZARD_USED                  (0xFF)
#define HAZARD_UNUSED                (0x00)

#define DRAWOVERLAY_STOP             (0)

/* ----------------------------------------------------------------------- */

/* Bitmap constants */

#define BITMAPFLAG_DEFAULT           (0 << 0)
#define BITMAPFLAG_MASKED            (1 << 0)
#define BITMAPFLAG_FLIPPED           (1 << 1)

#define STRETCHY_TYPE_END            (1) // terminator (set=NULL)
#define STRETCHY_TYPE_FIXED          (2) // height = bitmap->width_bytes - 2, no perspective scaling
#define STRETCHY_TYPE_150PC          (3) // height = 150% of perspective scale
#define STRETCHY_TYPE_50PC           (4) // height =  50%
#define STRETCHY_TYPE_113PC          (5) // height = 112.5%
#define STRETCHY_TYPE_38PC           (6) // height =  37.5%
#define STRETCHY_TYPE_75PC           (7) // height =  75%
#define STRETCHY_TYPE_25PC           (8) // height =  25%
#define STRETCHY_TYPE_100PC          (9) // height = 100%
#define STRETCHY_TYPE_200PC         (10) // height = 200%; Z80 dispatch ADD A,A fall-through

/* ----------------------------------------------------------------------- */

/* Key/Input constants */

#define KEYDEF_QUIT                  (0)
#define KEYDEF_PAUSE                 (1)
#define KEYDEF_BOOST                 (2)
#define KEYDEF_GEAR                  (3)
#define KEYDEF_ACCELERATE            (4)
#define KEYDEF_BRAKE                 (5)
#define KEYDEF_LEFT                  (6)
#define KEYDEF_RIGHT                 (7)
#define KEYDEF__LIMIT                (8)

#define USERINPUT_RIGHT              (0)
#define USERINPUT_LEFT               (1)
#define USERINPUT_DOWN               (2) /* aka brake */
#define USERINPUT_UP                 (3) /* aka accelerate */
#define USERINPUT_FIRE               (4) /* aka gear */
#define USERINPUT_BOOST              (5)
#define USERINPUT_PAUSE              (6)
#define USERINPUT_QUIT               (7)

#define USERINPUTFLAG_RIGHT          (1 << USERINPUT_RIGHT)
#define USERINPUTFLAG_LEFT           (1 << USERINPUT_LEFT )
#define USERINPUTFLAG_DOWN           (1 << USERINPUT_DOWN )
#define USERINPUTFLAG_UP             (1 << USERINPUT_UP   )
#define USERINPUTFLAG_FIRE           (1 << USERINPUT_FIRE )
#define USERINPUTFLAG_BOOST          (1 << USERINPUT_BOOST)
#define USERINPUTFLAG_PAUSE          (1 << USERINPUT_PAUSE)
#define USERINPUTFLAG_QUIT           (1 << USERINPUT_QUIT )

#define USERINPUTFLAGMASK_NONE       (0x00)
#define USERINPUTFLAGMASK_NOT_QUIT   (0x7F) /* mask of all input bits except QUIT */

#define USERINPUTFLAGMASK_ALLOW_NONE (0x00)
#define USERINPUTFLAGMASK_ALLOW_ALL  (0xFF)

/* ----------------------------------------------------------------------- */

/* Memory constants */

#define STAGEDATA_BASE               (0x5C00) /* first byte of paged stage data in Z80 address space */
#define STAGEDATA_END                (0x7FFF) /* last byte of paged stage data, inclusive */
#define STAGEDATA_LENGTH             (STAGEDATA_END + 1 - STAGEDATA_BASE)

/* ----------------------------------------------------------------------- */

/* 48K music engine note bytes: shared by music_data (CommonData.c) and
 * es_music_data (Bank7Data.c). See play_music_48k (Main.c) / es_play_music_48k
 * (Bank7.c) for the byte-level decode these are built from. */
#define NOTE_DELAY(ticks) (ticks) // ticks between notes, reloaded into note_delay
#define NOTE_INST_MASK    (0x07)
#define NOTE_DRUM2_VAL    (1)
#define NOTE_DRUM1_VAL    (2)
#define NOTE_NOISE_VAL    (3)
#define NOTE_DRUM2(param) (((param) << 3) | NOTE_DRUM2_VAL)
#define NOTE_DRUM1(param) (((param) << 3) | NOTE_DRUM1_VAL)
#define NOTE_NOISE(param) (((param) << 3) | NOTE_NOISE_VAL)
#define NOTE_SILENCE      (0)
#define NOTE_XDELAY_FLAG  (0x80)
#define NOTE_XDELAY(note) ((note) | NOTE_XDELAY_FLAG) // adds a one-tick extra delay
#define NOTE_END          (1) // sentinel: advance to the next pattern

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

#define DRAWCHARSTYLE_SCREEN         (1)
#define DRAWCHARSTYLE_SINGLE         (2)
#define DRAWCHARSTYLE_DOUBLE         (3)
#define DRAWCHARSTYLE_SINGLE_INVERTED (4)
#define DRAWCHARSTYLE_DOUBLE_INVERTED (5)
#define DRAWCHARSTYLE__LIMIT         (5)

#define TRANSITIONCONTROL_STOP       (0)
#define TRANSITIONCONTROL_DRAW_MUGSHOTS (1)
#define TRANSITIONCONTROL_OVERLAY_MESSAGES (2)
#define TRANSITIONCONTROL_FILL_ATTRIBUTES (3)
#define TRANSITIONCONTROL_FADE       (4)

/* ----------------------------------------------------------------------- */

/* Other constants */

#define TRANSITIONSTRIDE_FORWARD     (8) /* screen wipe step: attribute rows per frame, top-to-bottom */
#define TRANSITIONSTRIDE_REVERSE    (-8) /* screen wipe step: attribute rows per frame, bottom-to-top */

// Note: road_pos left..right is high..low
#define ROAD_RIGHTMOST          (0x00F5) /* road_pos value at rightmost road edge */
#define ROAD_LEFTMOST           (0x0105) /* road_pos value at leftmost road edge */
#define ROAD_126                (0x0126) /* road_pos cap applied during car bounce */

#define ROADBUF_CURVATURE_OFFSET  (0<<5)
#define ROADBUF_HEIGHT_OFFSET     (1<<5)
#define ROADBUF_LANES_OFFSET      (2<<5)
#define ROADBUF_RIGHTOBJS_OFFSET  (3<<5)
#define ROADBUF_LEFTOBJS_OFFSET   (4<<5)
#define ROADBUF_HAZARDS_OFFSET    (5<<5)

#define PREGAMECMD_STOP           (0x00)
#define PREGAMECMD_REPEAT         (0x1F) /* repeat previous command */
#define PREGAMECMD_SET_BG_0       (0xD0) /* set background colour; low nibble = index (0xD0..0xDF) */
#define PREGAMECMD_DRAW_BASE      (0xE0) /* draw base sprite */
#define PREGAMECMD_DRAW_HZ        (0xE1) /* draw horizontal element */
#define PREGAMECMD_DRAW_VT        (0xE2) /* draw vertical element */
#define PREGAMECMD_SET_ADDR       (0xF0) /* set draw address; low nibble = index (0xF0..0xFF) */

/* ----------------------------------------------------------------------- */

void  update_screen(chqstate_t *state, int screen, int width, int height);
void  update_attrs(chqstate_t *state, int attrs, int width, int height);
void  update_whole_playfield(chqstate_t *state);

/* Shared with Bank3.c */

u8   *z80addrtoscreen(chqstate_t *state, int addr, int left, int right);
u8   *z80addrtoattrs(chqstate_t *state, int addr, int left, int right);
u8   *z80addrtobackbuf(chqstate_t *state, int addr);
u16   wordat(const u8 *addr);
void  setwordat(u8 *addr, int value);

/* Shared with Bank7.c */

void  clear_playfield(chqstate_t *state);
void  drive_chatter(chqstate_t *state);
void  drive_chatter_stop(chqstate_t *state);
u8    keyscan(chqstate_t *state);
void  play_speech_128k(chqstate_t *state, int index);
void  increment_score(chqstate_t *state, int A_lo, int D_hi, int E_md);
void ptad_led_digits(chqstate_t *state,
                     int         iterations,
                     const u8   *digits,
                     u8         *stored,
                     u8         *screen);
void  sfx_bipbow(chqstate_t *state, int param1, int param2);
void  play_noise(chqstate_t *state, int Aparam);

/* ----------------------------------------------------------------------- */

typedef void obj_handler_t(chqstate_t *state,
                           int         Biterations,
                           const void *arg,
                           const s16  *IXxpos,
                           const u8   *IYheight);

typedef struct hazard hazard_t;

typedef void (hazard_handler_t)(chqstate_t *state, hazard_t *IXhazard);

typedef u8 chatterpriority_t;

void start_chatter(chqstate_t *state, chatterpriority_t priority,
                    const u8 *chatterblk);

typedef struct session session_t;

typedef struct carpart {
  u8        y;
  u8        rows;
  const u8 *bitmap;
} carpart_t;

typedef struct carsmokeframe {
  u8        height;
  u8        width;
  u8        unflipped_x;
  u8        flipped_x;
  const u8 *bitmap;
} carsmokeframe_t;

typedef struct carframe {
  u8        y;
  u8        x;
  u8        index;
} carframe_t;

typedef struct caradornment {
  u8        height;
  u8        width;
  const u8 *bitmap;
} caradornment_t;

/* ----------------------------------------------------------------------- */

hazard_handler_t no_op;
hazard_handler_t perp_behaviour;
hazard_handler_t hazard_handler;

obj_handler_t draw_stretchy_object_left;
obj_handler_t draw_stretchy_object_right;
obj_handler_t draw_tunnel_light_left;
obj_handler_t draw_tunnel_light_right;
obj_handler_t draw_object_left;
obj_handler_t draw_object_right;
obj_handler_t draw_overhead;

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_INTERNAL_H */
