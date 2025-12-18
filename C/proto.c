// proto.c
//
// Chase H.Q. code model
//
// by dpt

// vim: ts=8 sts=2 sw=2 et

// Compile with:
// gcc `sdl2-config --cflags --libs` -Wall -Wextra -Wpedantic -o proto proto.c

// Notes
//
// As with The Great Escape C conversion we model the game as if it's still
// running on a ZX Spectrum, avoiding a full rewrite of the original code and
// leaving (some) Z80-specific microoptimisations in place. This means the
// code remains a useful basis for comparison and lowers the risk of
// translation errors. Although it's very tempting to rewrite all the code to
// be fully idiomatic C, if the code's made too different then it gets harder
// and harder to refer back to the disassembled game and spot our mistakes.
// The goal after all is to use this C conversion to expose problem points
// and feed those back into the disassembly's description.
//
// The level data (called "stage" data in this conversion to match the
// original game) is retained whole in the converted game, even including
// embedded addresses. This lets us 'page in' levels by just importing the
// original game data. This also means that any new or adjusted levels
// produced by means of this conversion will be compatible with the original
// game. It will be interesting to see, but unlikely, if the Sinclair User
// demo version of the game uses the same level format. The Amstrad CPC
// version will no doubt be considerably different but perhaps familiar.
//
// Generic code will be converted to use native pointers. This means that
// some word-sized values will need to be indirected through new tables. For
// example see the "chatter" code: the code that prints the messages
// on-screen as the game runs. It previously embedded addresses inline in
// chatter structures. These are replaced with single bytes that reference
// new tables of pointers.
//
// This code is presently one honkin' great source file, including bits I've
// pinched from TGE-in-C. I'll likely split it up in due course, keeping the
// logic and the data separate.
//

// TODO
//
// Align the screen (and perhaps the back buffer too) on a 4K? boundary such
// that the assumed alignment still works.
//
// Copy whole messages that get modified into the state structure.
//

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

/* ----------------------------------------------------------------------- */

/* Macros for building bitmaps in code (pinched from TGE-in-C) */

#define ________   (0)
#define _______X   (1)
#define ______X_   (2)
#define ______XX   (3)
#define _____X__   (4)
#define _____X_X   (5)
#define _____XX_   (6)
#define _____XXX   (7)
#define ____X___   (8)
#define ____X__X   (9)
#define ____X_X_  (10)
#define ____X_XX  (11)
#define ____XX__  (12)
#define ____XX_X  (13)
#define ____XXX_  (14)
#define ____XXXX  (15)
#define ___X____  (16)
#define ___X___X  (17)
#define ___X__X_  (18)
#define ___X__XX  (19)
#define ___X_X__  (20)
#define ___X_X_X  (21)
#define ___X_XX_  (22)
#define ___X_XXX  (23)
#define ___XX___  (24)
#define ___XX__X  (25)
#define ___XX_X_  (26)
#define ___XX_XX  (27)
#define ___XXX__  (28)
#define ___XXX_X  (29)
#define ___XXXX_  (30)
#define ___XXXXX  (31)
#define __X_____  (32)
#define __X____X  (33)
#define __X___X_  (34)
#define __X___XX  (35)
#define __X__X__  (36)
#define __X__X_X  (37)
#define __X__XX_  (38)
#define __X__XXX  (39)
#define __X_X___  (40)
#define __X_X__X  (41)
#define __X_X_X_  (42)
#define __X_X_XX  (43)
#define __X_XX__  (44)
#define __X_XX_X  (45)
#define __X_XXX_  (46)
#define __X_XXXX  (47)
#define __XX____  (48)
#define __XX___X  (49)
#define __XX__X_  (50)
#define __XX__XX  (51)
#define __XX_X__  (52)
#define __XX_X_X  (53)
#define __XX_XX_  (54)
#define __XX_XXX  (55)
#define __XXX___  (56)
#define __XXX__X  (57)
#define __XXX_X_  (58)
#define __XXX_XX  (59)
#define __XXXX__  (60)
#define __XXXX_X  (61)
#define __XXXXX_  (62)
#define __XXXXXX  (63)
#define _X______  (64)
#define _X_____X  (65)
#define _X____X_  (66)
#define _X____XX  (67)
#define _X___X__  (68)
#define _X___X_X  (69)
#define _X___XX_  (70)
#define _X___XXX  (71)
#define _X__X___  (72)
#define _X__X__X  (73)
#define _X__X_X_  (74)
#define _X__X_XX  (75)
#define _X__XX__  (76)
#define _X__XX_X  (77)
#define _X__XXX_  (78)
#define _X__XXXX  (79)
#define _X_X____  (80)
#define _X_X___X  (81)
#define _X_X__X_  (82)
#define _X_X__XX  (83)
#define _X_X_X__  (84)
#define _X_X_X_X  (85)
#define _X_X_XX_  (86)
#define _X_X_XXX  (87)
#define _X_XX___  (88)
#define _X_XX__X  (89)
#define _X_XX_X_  (90)
#define _X_XX_XX  (91)
#define _X_XXX__  (92)
#define _X_XXX_X  (93)
#define _X_XXXX_  (94)
#define _X_XXXXX  (95)
#define _XX_____  (96)
#define _XX____X  (97)
#define _XX___X_  (98)
#define _XX___XX  (99)
#define _XX__X__ (100)
#define _XX__X_X (101)
#define _XX__XX_ (102)
#define _XX__XXX (103)
#define _XX_X___ (104)
#define _XX_X__X (105)
#define _XX_X_X_ (106)
#define _XX_X_XX (107)
#define _XX_XX__ (108)
#define _XX_XX_X (109)
#define _XX_XXX_ (110)
#define _XX_XXXX (111)
#define _XXX____ (112)
#define _XXX___X (113)
#define _XXX__X_ (114)
#define _XXX__XX (115)
#define _XXX_X__ (116)
#define _XXX_X_X (117)
#define _XXX_XX_ (118)
#define _XXX_XXX (119)
#define _XXXX___ (120)
#define _XXXX__X (121)
#define _XXXX_X_ (122)
#define _XXXX_XX (123)
#define _XXXXX__ (124)
#define _XXXXX_X (125)
#define _XXXXXX_ (126)
#define _XXXXXXX (127)
#define X_______ (128)
#define X______X (129)
#define X_____X_ (130)
#define X_____XX (131)
#define X____X__ (132)
#define X____X_X (133)
#define X____XX_ (134)
#define X____XXX (135)
#define X___X___ (136)
#define X___X__X (137)
#define X___X_X_ (138)
#define X___X_XX (139)
#define X___XX__ (140)
#define X___XX_X (141)
#define X___XXX_ (142)
#define X___XXXX (143)
#define X__X____ (144)
#define X__X___X (145)
#define X__X__X_ (146)
#define X__X__XX (147)
#define X__X_X__ (148)
#define X__X_X_X (149)
#define X__X_XX_ (150)
#define X__X_XXX (151)
#define X__XX___ (152)
#define X__XX__X (153)
#define X__XX_X_ (154)
#define X__XX_XX (155)
#define X__XXX__ (156)
#define X__XXX_X (157)
#define X__XXXX_ (158)
#define X__XXXXX (159)
#define X_X_____ (160)
#define X_X____X (161)
#define X_X___X_ (162)
#define X_X___XX (163)
#define X_X__X__ (164)
#define X_X__X_X (165)
#define X_X__XX_ (166)
#define X_X__XXX (167)
#define X_X_X___ (168)
#define X_X_X__X (169)
#define X_X_X_X_ (170)
#define X_X_X_XX (171)
#define X_X_XX__ (172)
#define X_X_XX_X (173)
#define X_X_XXX_ (174)
#define X_X_XXXX (175)
#define X_XX____ (176)
#define X_XX___X (177)
#define X_XX__X_ (178)
#define X_XX__XX (179)
#define X_XX_X__ (180)
#define X_XX_X_X (181)
#define X_XX_XX_ (182)
#define X_XX_XXX (183)
#define X_XXX___ (184)
#define X_XXX__X (185)
#define X_XXX_X_ (186)
#define X_XXX_XX (187)
#define X_XXXX__ (188)
#define X_XXXX_X (189)
#define X_XXXXX_ (190)
#define X_XXXXXX (191)
#define XX______ (192)
#define XX_____X (193)
#define XX____X_ (194)
#define XX____XX (195)
#define XX___X__ (196)
#define XX___X_X (197)
#define XX___XX_ (198)
#define XX___XXX (199)
#define XX__X___ (200)
#define XX__X__X (201)
#define XX__X_X_ (202)
#define XX__X_XX (203)
#define XX__XX__ (204)
#define XX__XX_X (205)
#define XX__XXX_ (206)
#define XX__XXXX (207)
#define XX_X____ (208)
#define XX_X___X (209)
#define XX_X__X_ (210)
#define XX_X__XX (211)
#define XX_X_X__ (212)
#define XX_X_X_X (213)
#define XX_X_XX_ (214)
#define XX_X_XXX (215)
#define XX_XX___ (216)
#define XX_XX__X (217)
#define XX_XX_X_ (218)
#define XX_XX_XX (219)
#define XX_XXX__ (220)
#define XX_XXX_X (221)
#define XX_XXXX_ (222)
#define XX_XXXXX (223)
#define XXX_____ (224)
#define XXX____X (225)
#define XXX___X_ (226)
#define XXX___XX (227)
#define XXX__X__ (228)
#define XXX__X_X (229)
#define XXX__XX_ (230)
#define XXX__XXX (231)
#define XXX_X___ (232)
#define XXX_X__X (233)
#define XXX_X_X_ (234)
#define XXX_X_XX (235)
#define XXX_XX__ (236)
#define XXX_XX_X (237)
#define XXX_XXX_ (238)
#define XXX_XXXX (239)
#define XXXX____ (240)
#define XXXX___X (241)
#define XXXX__X_ (242)
#define XXXX__XX (243)
#define XXXX_X__ (244)
#define XXXX_X_X (245)
#define XXXX_XX_ (246)
#define XXXX_XXX (247)
#define XXXXX___ (248)
#define XXXXX__X (249)
#define XXXXX_X_ (250)
#define XXXXX_XX (251)
#define XXXXXX__ (252)
#define XXXXXX_X (253)
#define XXXXXXX_ (254)
#define XXXXXXXX (255)

/* ----------------------------------------------------------------------- */

/* Z80 instruction simulator macros. */

/**
 * Shift left arithmetic.
 */
#define SLA(r)        \
  do {                \
    carry = (r) >> 7; \
    (r) <<= 1;        \
  } while (0)

/**
 * Shift right logical.
 */
#define SRL(r)        \
  do {                \
    carry = (r) & 1;  \
    (r) >>= 1;        \
  } while (0)

/**
 * Rotate left.
 */
#define RL(r)                   \
  do {                          \
    int carry_out;              \
                                \
    carry_out = (r) >> 7;       \
    (r) = ((r) << 1) | (carry); \
    carry = carry_out;          \
  } while (0)

/**
 * Rotate right through carry.
 */
#define RR(r)                         \
  do {                                \
    int carry_out;                    \
                                      \
    carry_out = (r) & 1;              \
    (r) = ((r) >> 1) | (carry << 7);  \
    carry = carry_out;                \
  } while (0)

/**
 * Rotate right.
 */
#define RRC(r)                        \
  do {                                \
    carry = (r) & 1;                  \
    (r) = ((r) >> 1) | (carry << 7);  \
  } while (0)

#define RRD(A, HL)                        \
  do {                                    \
    tmp = *HL & 0x0F;                     \
    *HL = (*HL >> 4) | ((A & 0x0F) << 4); \
    HL++;                                 \
    A = (A & 0xF0) | tmp;                 \
  } while (0)

/// Minimal equivalent of Z80 BCD correct operation
/// Additional
static uint8_t DAA(uint8_t v, int *carry_out)
{
  int lo, hi;
  int carry = 0;

  lo = (v >> 0) & 0x0F;
  hi = (v >> 4) & 0x0F;
  if (lo >= 10) { lo -= 10; hi++; }
  if (hi >= 10) { hi -= 10; carry++; }

  *carry_out = carry;
  return (hi << 4) | (lo << 0);
}

/* ----------------------------------------------------------------------- */

// Pinched from Spectrum.h + modified

#define SCREEN_WIDTH                    (256)
#define SCREEN_HEIGHT                   (192)

#define ATTRIBUTE_BRIGHT                (1<<6)

enum {
  attribute_BLACK_OVER_BLACK          = 0,
  attribute_BLUE_OVER_BLACK           = 1,
  attribute_RED_OVER_BLACK            = 2,
  attribute_PURPLE_OVER_BLACK         = 3,
  attribute_GREEN_OVER_BLACK          = 4,
  attribute_CYAN_OVER_BLACK           = 5,
  attribute_YELLOW_OVER_BLACK         = 6,
  attribute_WHITE_OVER_BLACK          = 7,
  attribute_BRIGHT_BLUE_OVER_BLACK    = 65,
  attribute_BRIGHT_RED_OVER_BLACK     = 66,
  attribute_BRIGHT_PURPLE_OVER_BLACK  = 67,
  attribute_BRIGHT_GREEN_OVER_BLACK   = 68,
  attribute_BRIGHT_CYAN_OVER_BLACK    = 69,
  attribute_BRIGHT_YELLOW_OVER_BLACK  = 70,
  attribute_BRIGHT_WHITE_OVER_BLACK   = 71,
  attribute_BLACK_OVER_BRIGHT_RED     = 0x50,
  attribute_BLACK_OVER_BRIGHT_MAGENTA = 0x58,
  attribute_BLACK_OVER_BRIGHT_GREEN   = 0x60,
  attribute_BLACK_OVER_BRIGHT_CYAN    = 0x68,
  attribute_BLACK_OVER_BRIGHT_WHITE   = 0x78
};

/* Memory map */

#define ROM_START_ADDRESS               (0x0000)
#define ROM_LENGTH                      (0x4000)
#define ROM_END_ADDRESS                 (ROM_START_ADDRESS + ROM_LENGTH - 1)

#define SCREEN_START_ADDRESS            (ROM_END_ADDRESS + 1)
#define SCREEN_BITMAP_ROWBYTES          (SCREEN_WIDTH / 8)
#define SCREEN_BITMAP_LENGTH            (SCREEN_BITMAP_ROWBYTES * SCREEN_HEIGHT)
#define SCREEN_ATTRIBUTES_START_ADDRESS (SCREEN_START_ADDRESS + SCREEN_BITMAP_LENGTH)
#define SCREEN_ATTRIBUTES_WIDTH         (SCREEN_WIDTH / 8)
#define SCREEN_ATTRIBUTES_ROWBYTES      SCREEN_ATTRIBUTES_WIDTH
#define SCREEN_ATTRIBUTES_HEIGHT        (SCREEN_HEIGHT / 8)
#define SCREEN_ATTRIBUTES_LENGTH        (SCREEN_ATTRIBUTES_ROWBYTES * SCREEN_ATTRIBUTES_HEIGHT)
#define SCREEN_LENGTH                   (SCREEN_BITMAP_LENGTH + SCREEN_ATTRIBUTES_LENGTH)
#define SCREEN_END_ADDRESS              (SCREEN_START_ADDRESS + SCREEN_LENGTH - 1)
#define SCREEN_ATTRIBUTES_END_ADDRESS   SCREEN_END_ADDRESS

/* ----------------------------------------------------------------------- */

#define PLAYFIELD_HEIGHT                (16 * 8)

#define BACKBUFFER_WIDTH                (256) // or is it 240?
#define BACKBUFFER_HEIGHT               (128)
#define BACKBUFFER_LENGTH               (BACKBUFFER_WIDTH / 8 * BACKBUFFER_HEIGHT)
#define BACKBUFFER_START_ADDRESS        ((uint16_t) 0xF000)

#define SCREEN(addr)    (&state->screen[(addr) - SCREEN_START_ADDRESS])
#define BACKBUF(addr)   (&state->backbuffer[(addr) - BACKBUFFER_START_ADDRESS])

#define UNSCREEN(ptr)   ((ptr) - &state->screen[0])
// Returns byte offset within backbuf, given a pointer [TODO cast to char *]
#define UNBACKBUF(ptr)  ((ptr) - &state->backbuffer[0])

#define STAGEDATA_BASE        (0x5C00)
#define STAGEDATA_END         (0x76EF) // inclusive
#define STAGEDATA_LENGTH      (STAGEDATA_END + 1 - STAGEDATA_BASE)

#define MAXHAZARDS            (6)

#define MARQUEELIGHTWIDTH     (5)
#define MARQUEELIGHTHEIGHT    (4)

#define STREND                (1<<7) // string terminating top bit

#define TURBOWIDTH            (16) // pixels
#define TURBOHEIGHT           (14)
#define TURBOFRAMES           (3)

#define FACEBITMAPBYTES       (32 / 8 * 40)
#define FACEATTRBYTES         (4 * 5)
#define FACEBYTES             (FACEBITMAPBYTES + FACEATTRBYTES)
#define NFACES                (3)

#define DRAWCHAR_TYPE_DUNNO           (0)
#define DRAWCHAR_TYPE_GENERIC         (1)
#define DRAWCHAR_TYPE_SINGLE          (2)
#define DRAWCHAR_TYPE_DOUBLE          (3)
#define DRAWCHAR_TYPE_SINGLE_INVERTED (4)
#define DRAWCHAR_TYPE_DOUBLE_INVERTED (5)

#define QUITSTATE_IDLE        (0)
#define QUITSTATE_START       (1)
#define QUITSTATE_DONE        (2) // not sure

#define USERINPUT_RIGHT       (1<<0)
#define USERINPUT_LEFT        (1<<1)
#define USERINPUT_DOWN        (1<<2) // aka brake
#define USERINPUT_UP          (1<<3) // aka accelerate
#define USERINPUT_FIRE        (1<<4) // aka gear
#define USERINPUT_TURBO       (1<<5)
#define USERINPUT_PAUSE       (1<<6)
#define USERINPUT_QUIT        (1<<7)
#define USERINPUT_NOT_QUIT    (0x7F)
#define USERINPUT_NONE        (0x00)

#define USERINPUTMASK_ALLOW_NONE (0x00)
#define USERINPUTMASK_ALLOW_ALL (0xFF)

#define EFFECT_SQUEAL         (1)
#define EFFECT_LANDING        (2)
#define EFFECT_CAR_HIT        (3)
#define EFFECT_SCENERY_HIT    (4)
#define EFFECT_HAZARD_HIT     (5)
#define EFFECT_WALL_HIT       (6)
#define EFFECT_CORNERING      (7)
#define EFFECT_BIP            (8)
#define EFFECT_BOW            (9)

// these state names need clarification
#define TIMEUPSTATE_INIT           (0)
#define TIMEUPSTATE_CHECK_TIME_UP  (1)
#define TIMEUPSTATE_CAR_STOPPED    (2)
#define TIMEUPSTATE_CHECK_RESTART  (3)
#define TIMEUPSTATE_WAITING        (4)

#define CHATTERSTATE_IDLE     (0)
#define CHATTERSTATE_START    (1)
#define CHATTERSTATE_RUN      (2)
#define CHATTERSTATE_STOP     (3)

typedef uint8_t chatterpriority_t;

#define PERPCAUGHTPHASE_0     (0)
#define PERPCAUGHTPHASE_1     (1)
#define PERPCAUGHTPHASE_2     (2)
#define PERPCAUGHTPHASE_3     (3)

/* ----------------------------------------------------------------------- */

// Conv: The C version uses IDs for strings and blocks rather than inline addresses.

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

#define HAZARD_UNUSED           (0xFF)

typedef struct hazard_s {
  uint8_t        used;
  uint8_t        distance;
  uint8_t        horz_pos;
  uint8_t        TBD3;
  uint8_t        TBD4;
  uint8_t        horz_pos_on_road;
  uint8_t        TBD6;
  uint8_t        TBD7;
  uint8_t        TBD8;
  const uint8_t *lod_addr; // Conv: uint16_t becomes pointer
  uint16_t       hit_handler;
  uint16_t       speed;
  uint8_t        TBD15;
  uint8_t        TBD16;
  uint8_t        TBD17; // perp distance high byte
  uint8_t        TBD18;
  uint8_t        TBD19;
}
hazard_t;

// crap name
typedef struct stagevars_s {
  // $A16D
  uint8_t  var_a16d;
  // $A16E
  uint8_t  idle_timer;
  // $A16F
  uint8_t  user_input_mask;
  // $A170
  uint8_t  turbos;
  // $A171
  uint16_t horizon_level;
  // $A173
  uint8_t  perp_halt_counter;
  // $A174
  uint8_t  displayed_gear;
  // $A175
  uint8_t  score_digits[8];
  // $A17D
  uint8_t  time_sixteenths;
  // $A17E
  uint8_t  time_bcd;
  // $A17F
  uint8_t  time_digits[2];
  // $A181
  uint8_t  distance_digits[4];
  // $A185
  uint8_t  no_objects_counter;
  // $A186
  uint16_t horizon_attribute;
}
stagevars_t;

typedef struct chqstate_s {
  // $4000
  uint8_t  screen[SCREEN_LENGTH];

  // $5B00
  uint8_t  pre_shifted_backdrop[10 * 24]; // occupies 256 bytes in real game?

  // $8000
  uint8_t  test_mode;
  // $8001
  uint8_t  attract_cycle;
  // $8002
  uint8_t  score_bcd[4];
  // $8006
  uint8_t  retry_count;
  // $8007
  uint8_t  wanted_stage_number;

  // $5C00..$76EF
  uint8_t  stagedata[STAGEDATA_LENGTH];

  // $8277
  uint8_t  SM_8277;
  // $828C
  uint8_t  SM_828c;

  // $8D77
  char     time_nn[7]; // initialised to "TIME 10"
  // $8D85
  char     credit_n[8]; // initialised to "CREDIT  "

  // $9618
  uint8_t  rng_seed[3];

  // $962E
  const char *next_character;
  // $9630
  const uint8_t *chatterblk_ptr; // current chatter block byte
  // $9632
  uint8_t  message_x;
  // $9633
  uint8_t  chatter_delay;
  // $9634
  uint8_t  noise_bytes[5];

  // $963C
  uint8_t  noise_counter;
  // $963D
  uint8_t  chatter_state;
  // $963E
  chatterpriority_t chatter_priority;

  // $9982
  uint8_t  SM_9982;

  // $9C85
  uint8_t  SM_9c85;
  // $9C86
  uint8_t  SM_9c86;

  // $9D51
  char     bonus_string[6];

  // $9D9B
  char    *SM_address_of_score_digits; // was self modified

  // $9E22
  uint8_t  SM_9e22; // in plot_turbos_and_scores

  // $A0D5
  uint8_t  user_input;

  // $A139
  uint8_t  mode_128k;
  // $A13A
  uint8_t  current_stage_number; // init to 1
  // $A13B
  uint8_t  start_speech_cycle; // init to 4
  // $A13C
  uint8_t  overtake_bonus_bcd;
  // $A13D
  uint8_t  credits;

  // $A16D
  stagevars_t st;

  // $A188
  hazard_t hazards[MAXHAZARDS];

  // $A220
  uint8_t  dont_draw_screen_attrs;
  // $A221
  uint8_t  inhibit_collision_detection;
  // $A222
  uint8_t  n_hazards;
  // $A223
  uint8_t  displayed_stage;
  // $A224
  uint8_t  helicopter_control;
  // $A225
  uint8_t  dont_spawn_cars;
  // $A226
  uint8_t  correct_fork;
  // $A227
  uint8_t  floating_arrow;
  // $A228
  uint8_t  cherry_light;
  // $A229
  uint8_t  time_up_state;
  // $A22A
  uint8_t  car_y;
  // $A22B
  uint8_t  overtake_bonus_counter;
  // $A22C
  uint8_t  trigger_bonus_flag;
  // $A22D
  uint8_t  bonus_counter;
  // $A22E
  uint8_t  sighted_flag;
  // $A22F
  uint8_t  hand_flag;
  // $A230
  uint8_t  perp_caught_phase;
  // $A231
  uint8_t  transition_control;
  // $A232
  uint8_t  smash_level;
  // $A233
  uint8_t  smash_counter;
  // $A234
  uint8_t  counter_A;
  // $A235
  uint8_t  counter_B;
  // $A236
  uint8_t  counter_C;

  // $A23F
  uint8_t  fast_counter;
  // $A240
  uint8_t *road_buffer_offset;
  uint8_t *road_buffer_start; // additional
  uint8_t *road_buffer_end; // additional

  // $A249
  uint8_t  fork_taken;

  // $A24A
  uint16_t speed;
  // $A24C
  uint8_t  inclined;
  // $A24D
  uint8_t  cornering;
  // $A24E
  uint8_t  boost;
  // $A24F
  uint8_t  smoke;
  // $A250
  uint8_t  turn_speed;
  // $A251
  uint8_t  flip_car;
  // $A252
  uint8_t  gear_lockout;
  // $A253
  uint8_t  gear;
  // $A254
  uint8_t  allow_spawning;
  // $A255
  uint8_t  distance_bcd[2];

  // $A265
  uint8_t  fork_visible;
  // $A266
  uint8_t  fork_countdown;
  // $A267
  uint16_t fork_distance;
  // $A269
  uint8_t  fork_in_progress;
  // $A26A
  uint8_t  quit_state;
  // $A26B
  uint8_t  start_speech;
  // $A26C
  uint16_t road_pos;
  const uint8_t *road_curvature_ptr;
  const uint8_t *road_height_ptr;
  const uint8_t *road_lanes_ptr;
  const uint8_t *road_rightside_ptr;
  const uint8_t *road_leftside_ptr;
  const uint8_t *road_hazard_ptr;

  // $E300
  uint8_t  table_e300[32]; // note: first byte should be $60
  // $E320
  uint8_t  table_e320[2];
  // $E34B
  uint8_t  horizon_table_e34b[3]; // horizon related
  // $E34F
  uint8_t  object_positions[21];
  // $E800
  uint16_t table_e800[128];
  // $E900
  uint16_t table_e900[128];
  // $EA00
  uint16_t table_ea00[128];
  // $EB00
  uint16_t table_eb00[128];
  // $EC00
  uint16_t table_ec00[128];
  // $ED00
  uint16_t table_ed00[128];
  // $EE00
  uint8_t  road_buffer[256];

  // $F000
  uint8_t  backbuffer[BACKBUFFER_LENGTH];
} chqstate_t;

/* ----------------------------------------------------------------------- */

#include "data.c"

/* ----------------------------------------------------------------------- */

static void chasehq_reset_state(chqstate_t *state)
{
  memset(state, 0, sizeof(*state));

  state->wanted_stage_number = 1;

  state->SM_8277 = 0xF0; // attract mode blinker

  memcpy(state->time_nn, "TIME 1\xB0", 7);
  memcpy(state->credit_n, "CREDIT \xA0", 8);

  state->rng_seed[0] = 0x7B;
  state->rng_seed[1] = 0x2D;
  state->rng_seed[2] = 0xE9;

  state->road_buffer_offset = &state->road_buffer[0];
  state->road_buffer_start  = &state->road_buffer[0];
  state->road_buffer_end    = &state->road_buffer[256];
}

/* ----------------------------------------------------------------------- */

// read an arbitrary native word
static uint16_t wordat(const uint8_t *addr)
{
  return (addr[0] << 0) | (addr[1] << 8);
}

// given a Z80 address read a stagedata byte
static uint16_t stgbyte(chqstate_t *state, int address)
{
  int offset;
  assert(address >= STAGEDATA_BASE && address <= STAGEDATA_END);
  offset = address - STAGEDATA_BASE;
  return state->stagedata[offset];
}

// given a Z80 address read a stagedata word
static uint16_t stgword(chqstate_t *state, int address)
{
  int offset;
  assert(address >= STAGEDATA_BASE && address <= STAGEDATA_END);
  offset = address - STAGEDATA_BASE;
  return wordat(&state->stagedata[offset]);
}

// map a Z80 address to a stagedata pointer
static const uint8_t *stgmap(chqstate_t *state, int address)
{
  if (address == 0)
    return NULL;
  assert(address >= STAGEDATA_BASE && address <= STAGEDATA_END);
  return &state->stagedata[address - STAGEDATA_BASE];
}

// given a Z80 address read a stagedata word and map it to a native pointer into stagedata
static const uint8_t *stgwordtostgptr(chqstate_t *state, int address)
{
  return stgmap(state, stgword(state, address));
}

// given a native pointer read a word and map it to a native pointer into stagedata
static const uint8_t *ptrtostgptr(chqstate_t *state, const uint8_t *addr)
{
  return stgmap(state, wordat(addr));
}

/* ----------------------------------------------------------------------- */

static void attract_mode(chqstate_t *state);

static void start_siren_hook(chqstate_t *state);

static void play_engine_or_siren_sfx_hook(chqstate_t *state);
static void silence_audio_hook(chqstate_t *state);
static void write_audio_registers_hook(chqstate_t *state);
static void setup_engine_sfx_hook(chqstate_t *state);
static void play_engine_sfx_hook(chqstate_t *state);
static void play_speech_hook(chqstate_t *state);
static void attract_mode_hook(chqstate_t *state);

static void main_loop(chqstate_t *state);

static void cpu_driver(chqstate_t *state);

static void set_up_stage(chqstate_t *state, const uint8_t *stage_data);
static void sus_clear_lights(uint8_t *attrptr);

static void check_user_input(chqstate_t *state);

static void start_sfx(chqstate_t *state, uint8_t Bindex, uint8_t Cpriority);

static void drive_sfx(chqstate_t *state);

static void sfx_crash(chqstate_t *state, uint8_t Dparam);
static void sfx_thud(chqstate_t *state, uint8_t Dparam);
static void sfx_cornering(chqstate_t *state, uint8_t Dparam, uint8_t Eparam);
static void sfx_bipbow(chqstate_t *state, uint8_t Dparam, uint8_t Eparam);

static void handle_perp_caught(chqstate_t *state);

static void clear_playfield_attrs(chqstate_t *state);
static void clear_playfield(chqstate_t *state);

static void fully_smashed(chqstate_t *state);

static void transition(chqstate_t *state);

static void fill_attributes(chqstate_t *state);

static void draw_overlay_messages(chqstate_t *state);

static const uint8_t *print_message(chqstate_t    *state,
                                    uint8_t        Aflags,
                                    const uint8_t *HLmessages);

static void setup_overlay_messages(chqstate_t *state, const uint8_t *HL);

static void draw_smash_bar(chqstate_t *state);

static void draw_everything_else(chqstate_t *state);

static uint8_t rng(chqstate_t *state);

static void start_chatter(chqstate_t       *state,
                          chatterpriority_t priority,
                          const uint8_t    *chatterblk);

static void drive_chatter(chqstate_t *state);
static void drive_chatter_stop(chqstate_t *state);

static void print_chatter(chqstate_t *state);
static void pc_chatter_message(chqstate_t *state, const uint8_t *HLchatter);
static void pc_clear_line(chqstate_t *state, uint8_t x);

static void noise_effect(chqstate_t *state, uint8_t counter);
static void noise_effect_9a5c(chqstate_t *state, uint8_t counter);
static void ne_plot_attrs(chqstate_t *state, uint8_t A);

static void plot_face(chqstate_t    *state,
                      const uint8_t *HLface,
                      uint16_t       DEscreen);

static void plot_mini_font_cursor_off(chqstate_t *state,
                                      uint8_t     x,
                                      char        character);
static void plot_mini_font_cursor_on(chqstate_t *state,
                                     uint8_t     x,
                                     char        character);
static void pmf_go(chqstate_t *state,
                   uint8_t     x,
                   char        character,
                   uint8_t     B,
                   uint8_t     C);

static void clear_message_line(chqstate_t *state);

static void tick(chqstate_t *state);

static void speed_score(chqstate_t *state);

static void add_bonus(chqstate_t *state,
                      uint8_t     A_lo,
                      uint8_t     E_md,
                      uint8_t     D_hi);
static int bonus_digit(uint8_t digit,
                       uint8_t *nonzeroflag,
                       char   **poutput);

static void increment_score(chqstate_t *state,
                            uint8_t     A_lo,
                            uint8_t     E_md,
                            uint8_t     D_hi);

static void calc_overtake_bonus(chqstate_t *state);

static void update_scoreboard(chqstate_t *state);

static void toggle_light_brightness(chqstate_t *state, uint8_t *HL);

static void plot_turbos_and_scores(chqstate_t *state);
static void ptas_led_digits(chqstate_t    *state,
                            uint8_t        Biterations,
                            const uint8_t *DEdigits,
                            uint8_t       *HLstored,
                            uint8_t       *DEscreen);

static uint8_t *ledfont_plot(chqstate_t *state, uint8_t ord, uint8_t *screen);

static void draw_string_A(chqstate_t    *state,
                          uint8_t        A,
                          const uint8_t *BCstring,
                          uint8_t       *DEscreen);
static void draw_string(chqstate_t    *state,
                        uint8_t        A,
                        const uint8_t *BCstring,
                        uint8_t       *DEscreen);
static void draw_string_entry(chqstate_t    *state,
                              uint8_t       *DEscreen,
                              const uint8_t *HLstring,
                              uint8_t        Adash,
                              uint8_t        Cdash,
                              uint8_t        DEstride,
                              uint8_t       *HLattr);

static void draw_char(chqstate_t *state,
                      uint8_t     Achar,
                      uint8_t    *DE,    // screen address
                      uint8_t     Adash, // draw type
                      uint8_t     Cdash, // attribute
                      uint8_t     DEdash, // e.g. 32 - a stride?
                      uint8_t    *HLdash);

static uint8_t keyscan(chqstate_t *state);

static void check_scenery_collisions(chqstate_t *state);

static void layout_objects(chqstate_t *state);

static void cycle_counters(chqstate_t *state);

static void spawn_cars(chqstate_t *state);

static void choose_dirt_and_stones(chqstate_t *state);

static void layout_dirt_and_stones(chqstate_t *state);

static void move_helicopter(chqstate_t *state);

static void drive_helicopter(chqstate_t *state);

static void spawn_hazards(chqstate_t *state);

static void draw_hazards(chqstate_t *state);

static void move_hero_car(chqstate_t *state);

static void animate_hero_car(chqstate_t *state);

static void scroll_horizon(chqstate_t *state);

static void update_road_level(chqstate_t *state);

static void layout_road(chqstate_t *state);

static void exit_fork(chqstate_t *state);

static void draw_screen(chqstate_t *state);

static void clear_playfield_set_attrs(chqstate_t *state);

static void read_map(chqstate_t *state);

static void prepare_tunnel(chqstate_t *state);

static void draw_tunnel(chqstate_t *state, uint8_t *IY);

static void draw_road_scene_change(chqstate_t *state, uint8_t *IX, uint8_t *IY);

static void draw_road(chqstate_t *state);

static void pre_shift_backdrop(chqstate_t *state);

static void forked_road_plotter(chqstate_t *state);

static void backdrop_fill_choice(chqstate_t *state);

static void build_curve_table(chqstate_t *state, int forked);
static void build_curve_table_sub_cca8(chqstate_t *state,
                                       uint8_t     Bdash_alwayszero,
                                       uint16_t   *HLtableend,
                                       uint16_t    DEroadpos);

static void build_height_table(chqstate_t *state);

typedef int8_t T; // works
static T multiply(T a, T c);

/* ----------------------------------------------------------------------- */

// $8014 (copied to that position in the original)
// $F220 page_in_stage_128k
static void load_stage(chqstate_t *state)
{
  static const uint8_t *stage_data_locations[] = {
    &stage1[0],
    &stage1[0],
    &stage1[0],
    &stage1[0],
    &stage1[0],
    &stage1[0]
  };

  uint8_t A;

  // Return if the stage is already loaded
  A = state->wanted_stage_number;
  if (A == state->current_stage_number)
    return;
  state->current_stage_number   = A;

  // Copy the stage data from source to stagedata[]
  memcpy((uint8_t *) stgmap(state, STAGEDATA_BASE), stage_data_locations[A],
         STAGEDATA_LENGTH);
}

// $8258
static void attract_mode(chqstate_t *state)
{
  int            carry = 0;
  uint8_t        SM_828c; // blinker - move to state?
  uint8_t        A;
  const uint8_t *HL;
  uint8_t        B;

  set_up_stage(state, stgmap(state, 0x5D2B)); // attract_data
  SM_828c = 0;
  state->speed = 400;
  for (;;) {
    A = keyscan(state);
    if (A == USERINPUT_FIRE)
      return;

    cpu_driver(state);

    HL = &attract_messages[0];
    B  = 1;
    A  = state->SM_8277;
    RRC(A);
    state->SM_8277 = A;
    if (!carry)
      B++;

    // Display 'B' messages
    do {
      A = *HL; // load flags
      HL = print_message(state, A, HL);
    } while (--B > 0);

    if (state->transition_control == 0) {
      // Alternate between credits and copyright messages.
      A = state->SM_828c;
      A ^= 1;
      state->SM_828c = A;
      HL = &credits_messages[0];
      if (A)
        HL = &copyright_messages[0];
      setup_overlay_messages(state, HL);
    }

    transition(state);
    draw_screen(state);
  }
}

// $83B5
static void start_siren_hook(chqstate_t *state)
{
}

// $83B8
static void play_engine_or_siren_sfx_hook(chqstate_t *state)
{
}

// $83BB
static void silence_audio_hook(chqstate_t *state)
{
}

// $83BE
static void write_audio_registers_hook(chqstate_t *state)
{
}

// $83C1
static void setup_engine_sfx_hook(chqstate_t *state)
{
}

// $83C4
static void play_engine_sfx_hook(chqstate_t *state)
{
}

// $83C7
static void play_speech_hook(chqstate_t *state)
{
}

// $83CA
static void attract_mode_hook(chqstate_t *state)
{
}

// $8401
static void main_loop(chqstate_t *state)
{
  uint8_t start_speech_index; // was A

  load_stage(state);
  if (state->wanted_stage_number != 6)
    goto ml_not_credits;

  // TODO: Call $5C00
  state->wanted_stage_number = 1;
  // TODO: load_stage();
  state->wanted_stage_number = 6; // not sure why
  return;

ml_not_credits:
  // TODO: Call run_pregame_screen
  set_up_stage(state, stgmap(state, 0x5D1D));

  // Cycle start_speech_cycle 3,2,1 then repeat
  start_speech_index = state->start_speech_cycle - 1;
  if (start_speech_index == 0)
    start_speech_index = 3;
  state->start_speech_cycle = start_speech_index;

  // Choose the startup speech sample
  state->start_speech = (start_speech_index * 4) | 2;
  state->hazards[0].used = HAZARD_UNUSED;
  if (state->mode_128k == 0)
    start_chatter(state, 0xFF, chatterblk_start_stage);

  do {
    drive_sfx(state);
    keyscan(state);
    tick(state);
    check_user_input(state);
    read_map(state);
    handle_perp_caught(state);
    move_hero_car(state);
    spawn_cars(state);
    cycle_counters(state);
    play_engine_or_siren_sfx_hook(state);
    build_height_table(state);
    scroll_horizon(state);
    play_engine_or_siren_sfx_hook(state);
    layout_road(state);
    play_engine_or_siren_sfx_hook(state);
    draw_road(state);
    play_engine_or_siren_sfx_hook(state);
    layout_objects(state);
    prepare_tunnel(state);
    spawn_hazards(state);
    drive_helicopter(state);
    choose_dirt_and_stones(state);
    play_engine_or_siren_sfx_hook(state);
    draw_hazards(state);
    layout_dirt_and_stones(state);
    play_engine_or_siren_sfx_hook(state);
    move_helicopter(state);
    check_scenery_collisions(state);
    play_engine_or_siren_sfx_hook(state);
    draw_everything_else(state);
    play_engine_or_siren_sfx_hook(state);
    animate_hero_car(state);
    speed_score(state);
    update_scoreboard(state);
    calc_overtake_bonus(state);
    play_engine_or_siren_sfx_hook(state);
    drive_chatter(state);
    draw_smash_bar(state);
    transition(state);
    play_engine_or_siren_sfx_hook(state);
    draw_screen(state);
    exit_fork(state);
  } while (state->test_mode == 0);

  // TODO test mode etc.
}

// $852A
static void cpu_driver(chqstate_t *state)
{
  const uint16_t LeftPos  = 0x0105; // note: road_pos left..right is high..low
  const uint16_t RightPos = 0x00F5;
  const uint8_t  MinSpeed = 150;

  uint16_t roadpos; // was HL
  uint8_t  input;   // was A

  roadpos = state->road_pos;
  input = USERINPUT_UP | USERINPUT_RIGHT;
  if (roadpos < LeftPos) {
    input = USERINPUT_UP | USERINPUT_LEFT;
    if (roadpos >= RightPos)
      input = USERINPUT_UP;
  }

  if (state->gear != (state->speed < MinSpeed))
    input |= USERINPUT_FIRE;

  state->user_input = input;
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
  draw_hazards(state);
  move_hero_car(state);
  check_scenery_collisions(state);
  draw_everything_else(state);
  animate_hero_car(state); // exit via
}

// $87DC
//
// stage_data is genuine pointer here since sometimes it's pointed at stage
// data or attract data and at other times it's the escape scene data.
static void set_up_stage(chqstate_t *state, const uint8_t *stage_data)
{
  uint8_t  iterations; // was B
  uint8_t *HL;

  memset(&state->road_buffer[0], 0, 256);
  state->st         = saved_game_state;
  state->hazards[0] = saved_game_state_hazard_0;
  memset(&state->hazards[1], 0, sizeof(hazard_t) * (MAXHAZARDS - 1));

  state->road_pos           = wordat(stage_data);
  state->road_curvature_ptr = ptrtostgptr(state, stage_data +  2);
  state->road_height_ptr    = ptrtostgptr(state, stage_data +  4);
  state->road_lanes_ptr     = ptrtostgptr(state, stage_data +  6);
  state->road_rightside_ptr = ptrtostgptr(state, stage_data +  8);
  state->road_leftside_ptr  = ptrtostgptr(state, stage_data + 10);
  state->road_hazard_ptr    = ptrtostgptr(state, stage_data + 12);

  pre_shift_backdrop(state);

  // Set backdrop position in horizon table (used to draw attributes)
  state->horizon_table_e34b[0] = 8;
  state->horizon_table_e34b[1] =
    0; // initialised strangely, presumed to be zero (needs checking)
  state->horizon_table_e34b[2] = 0;

#if 0
  // NOP some things TBD
  state->SM_8F82 = 0;
  //$8F83 = 0; // first one covers it all
  //$8F84 = 0;

  // NOP heli and tunnel drawing calls
  state->SM_8FA4 = 0; // draw heli call
  state->SM_8FA7 = 0; // draw tunnel call

  state->SM_C058 = 0; // ?
  state->SM_B063 = 0;
#endif
  state->hazards[0].lod_addr = stgwordtostgptr(state, 0x5D10);

  // Conv: Duplicate work removed.

  // Run the map reader 32 times [enough to draw the screen?]
  iterations = 32;
  do {
    HL = &state->fast_counter;
#if 0
    rm_cycle_buffer_offset(state, HL);
#endif
  } while (--iterations > 0);

  // Disallow spawning
  state->allow_spawning = 0;

#if 0
  A = 0xF8; // reverse transition
  setup_transition(state, ...);
#endif

  clear_playfield_set_attrs(state);
  // Clear the lights' BRIGHT bit
  sus_clear_lights(SCREEN(0x5820));
  sus_clear_lights(SCREEN(0x583B));

  silence_audio_hook(state);
  update_scoreboard(state); // exit via
}

// $8860 (pulled out of above)
static void sus_clear_lights(uint8_t *attrptr)
{
  int rows; // was C
  int cols; // was B

  rows = MARQUEELIGHTHEIGHT;
  do {
    cols = MARQUEELIGHTWIDTH;
    do
      *attrptr++ &= ~ATTRIBUTE_BRIGHT;
    while (--cols > 0);
    attrptr += (SCREEN_ATTRIBUTES_ROWBYTES - MARQUEELIGHTWIDTH);
  } while (--rows > 0);
}

// $8876
static void check_user_input(chqstate_t *state)
{
  uint8_t  Atransition;
  uint8_t *HLuserinput;
  uint8_t  Ainput;
  uint8_t *HLboost;
  uint8_t *HLchatter;
  uint8_t  Akey;

  Atransition = state->transition_control;
  HLuserinput = &state->user_input;
  if (Atransition != 4) {
    *HLuserinput = USERINPUT_NONE;
    return;
  }

  *HLuserinput = Ainput = (state->st.user_input_mask & *HLuserinput);
  if ((Ainput & (USERINPUT_QUIT | USERINPUT_PAUSE | USERINPUT_TURBO)) == 0)
    return;

  if (Ainput & USERINPUT_QUIT)
    goto quit_key;
  else if (Ainput & USERINPUT_PAUSE)
    goto pause_key;

  // Turbo pressed
  HLboost = &state->boost;
  if (*HLboost > 0 || state->st.turbos == 0)
    return; // already boosting or no turbos remain
  *HLboost = 60; // set ticks of boost

  start_chatter(state, 2, &chatterblk_turbo[0]);
  setup_engine_sfx_hook(state); // exit via
  return;

quit_key:
  if (state->quit_state == 0) {
    drive_chatter_stop(state);
    fill_attributes(state);

    state->st.user_input_mask = USERINPUTMASK_ALLOW_NONE;
    state->quit_state         = QUITSTATE_START;
  }
  return;

pause_key:
  silence_audio_hook(state);
  do
    Akey = keyscan(state);
  while (Akey & USERINPUT_PAUSE);
  do
    Akey = keyscan(state);
  while ((Akey & USERINPUT_NOT_QUIT) == 0);
  do
    Akey = keyscan(state);
  while ((Akey & USERINPUT_NOT_QUIT) != 0);
}

// $88D5
static void clear_playfield_attrs(chqstate_t *state)
{
  memset(SCREEN(0x5900), 0, SCREEN_ATTRIBUTES_ROWBYTES * PLAYFIELD_HEIGHT / 8);
}

// $88E2
static void clear_playfield(chqstate_t *state)
{
  clear_playfield_attrs(state);
  memset(SCREEN(0x4800), 0, SCREEN_BITMAP_ROWBYTES * PLAYFIELD_HEIGHT);
}

// $88F2
static void start_sfx(chqstate_t *state, uint8_t Bindex, uint8_t Cpriority)
{
}

// $8903
static void drive_sfx(chqstate_t *state)
{
}

// $8960
static void sfx_crash(chqstate_t *state, uint8_t Dparam)
{
}

// $89D9
static void sfx_thud(chqstate_t *state, uint8_t Dparam)
{
}

// $8A0F
static void sfx_cornering(chqstate_t *state, uint8_t Dparam, uint8_t Eparam)
{
}

// $8A36
static void sfx_bipbow(chqstate_t *state, uint8_t Dparam, uint8_t Eparam)
{
}

// $8A57
static void handle_perp_caught(chqstate_t *state)
{
}

// $8C3A
static void fully_smashed(chqstate_t *state)
{
  state->perp_caught_phase  = PERPCAUGHTPHASE_1;
  state->hand_flag          = 2; // for this one too
  state->smash_counter      = 20;
  state->st.user_input_mask = USERINPUT_PAUSE | USERINPUT_QUIT;
  setup_overlay_messages(state, &pull_over_message[0]);
  // hpc_set_perp_speed(state, 0x0190);
}

// $8D8F
static void transition(chqstate_t *state)
{
}

// $8E29
static void fill_attributes(chqstate_t *state)
{
  uint8_t *src;     // was HL
  uint8_t *dst;     // was DE
  int      rows;    // was A
  int      columns; // was BC

  src = SCREEN(0x5901); // (1,8)
  rows = 16; // rows
  do {
    if (1) {
      // As original code
      dst = src + 1;
      columns = 28;
      do { *dst++ = *src++; } while (--columns > 0); // was LDIR
      src += 32 - 28;
    } else {
      // Conv: Alternative that uses memset
      memset(src + 1, *src, 28);
      src += 32;
    }
  } while (--rows > 0);

  state->transition_control = 0;
}

// $8E42
static void draw_overlay_messages(chqstate_t *state)
{
}

// $8E6C
static const uint8_t *print_message(chqstate_t    *state,
                                    uint8_t        Aflags,
                                    const uint8_t *HLmessages)
{
  return NULL; // TODO
}

// $8E7E
static void setup_overlay_messages(chqstate_t *state, const uint8_t *HL)
{
}

// $8EE7
static void draw_smash_bar(chqstate_t *state)
{
}

// $8F5F
static void draw_everything_else(chqstate_t *state)
{
}

// $961B
static uint8_t rng(chqstate_t *state)
{
  int      carry = 0;
  uint8_t *seed; // was HL
  uint8_t  A;

  seed = &state->rng_seed[0];
  A = *seed - 0x8D;
  *seed++ = A;
  *seed += 3;
  A += *seed++;
  RRC(A);
  RRC(*seed);
  A += *seed;
  *seed = A;
  return A;
}

// $9945
static void start_chatter(chqstate_t       *state,
                          chatterpriority_t priority,
                          const uint8_t    *chatterblk)
{
  uint8_t chatter_state; // was A

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

// $9965
static void drive_chatter(chqstate_t *state)
{
  uint8_t        chatter_state; // was A
  int            carry = 0;
  char           character; // was D
  uint8_t        rotating; // was A
  uint8_t        delay; // was A
  uint8_t        x; // was A
  uint8_t        B;
  const char    *HL;
  const uint8_t *chatterblk; // was HL
  uint8_t        chattercmd; // was A

  chatter_state = state->chatter_state;

  chatter_state--;
  if (chatter_state == 0) // starting (1)
    goto starting;

  chatter_state--;
  if (chatter_state == 0) // displaying (2)
    goto do_noise_effect;

  chatter_state--;
  if (chatter_state == 0) { // stopping (3)
    if (--state->noise_counter != 0) {
      noise_effect(state, state->noise_counter); // exit via
      return;
    }

    // enter idle state, hide face by wiping attrs to black on black
    state->chatter_state = CHATTERSTATE_IDLE;
    ne_plot_attrs(state, 0 /* black on black attrs */);
  }

  // idle state (0)

  character = ' ';
  rotating = state->SM_9982; // Conv: Was self modified
  RRC(rotating);
  state->SM_9982 = rotating;
  x = 0xFF; // ie -1
  if (carry)
    plot_mini_font_cursor_on(state, x, character); // exit via
  else
    plot_mini_font_cursor_off(state, x, character); // exit via
  return;

do_noise_effect:
  if (state->noise_counter) {
    noise_effect(state, state->noise_counter); // exit via
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
  HL = state->next_character - 1; // addr of next char
  character = *HL & ~STREND; // load char and clear any terminator
  x = state->message_x - 1;
  RR(B);
  if (carry)
    plot_mini_font_cursor_on(state, x, character); // exit via
  else
    plot_mini_font_cursor_off(state, x, character); // exit via
  return;

clear_line:
  x = state->message_x;
  if (x) {
    pc_clear_line(state, x); // exit via
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
    pc_chatter_message(state, chatterblk); // exit via
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
  noise_effect(state, 4); // exit via
}

// $99D3
// Factored out from above, since other routines call it.
static void drive_chatter_stop(chqstate_t *state)
{
  state->noise_counter = 4;
  state->chatter_state = CHATTERSTATE_STOP;
  clear_message_line(state); // exit via
}

// $99EC
static void print_chatter(chqstate_t *state)
{
  const uint8_t *chatter; // was HL
  uint8_t        cmd;     // was A
  uint8_t        rnd;     // was A
  const uint8_t *face;    // was HL

  chatter = state->chatterblk_ptr;
  for (;;) {
    cmd = *chatter++; // read a command ($FC) or speaking character's ID
    if (cmd != CHATTERCMD_RANDOM)
      goto pc_plot_character; // use break instead?

    // Random choice
    rnd = rng(state);
    if (rnd >= 0x55) {
      chatter += 2;
      if (rnd >= 0xAA)
        chatter += 2;
    }
    /* Conv: This is an index, not an address */
    chatter = chatter_blocks[*chatter];
  }

pc_plot_character:
  // cmd is now the character ID
  face = stgwordtostgptr(state, 0x5CF2);
  if (cmd != CHATTERCHR_PILOT)
    face = &bitmap_faces[cmd]; // Conv: Simplified

  plot_face(state, face, 0x4036); // Set plot address to (176,8)

  pc_chatter_message(state, chatter); // was FALLTHROUGH
}

// $9A24
static void pc_chatter_message(chqstate_t *state, const uint8_t *HLchatter)
{
  const char *chatterblk; // was DE

  // Conv: Original game loads an address directly here.
  chatterblk = chatter_strings[*HLchatter++];
  state->chatterblk_ptr = HLchatter;
  state->next_character = chatterblk;
  pc_clear_line(state, 0); // was FALLTHROUGH
}

// $9A30
// 'x' was A
static void pc_clear_line(chqstate_t *state, uint8_t x)
{
  const char *nextch;    // was HL
  char        character; // was D

  if (x == 0)
    clear_message_line(state);

  nextch = state->next_character;
  assert(nextch);
  character = *nextch & ~STREND; // remove any terminator
  plot_mini_font_cursor_on(state, x, character);
  if (*nextch++ & STREND) // if terminated
    state->chatter_delay = 10; // pause at end of string
  state->message_x = x + 1;
  state->next_character = nextch;
}

// $9A55
static void noise_effect(chqstate_t *state, uint8_t counter)
{
  state->noise_counter = --counter;
  if (counter == 0)
    print_chatter(state); // exit via
  else
    noise_effect_9a5c(state, counter); // was FALLTHROUGH
}

// Move to next row (down)
// Conv: added
static uint16_t nextrow(uint16_t screen)
{
  screen += 256;
  if (((screen >> 8) & 7) == 0) {
    int t = (screen & 0xFF) + 32;
    screen = (screen & 0xFF00) | (t & 0xFF);
    if (t < 0x100) { // didn't carry
      t = (screen >> 8) - 8;
      screen = (t << 8) | (screen & 0xFF);
    }
  }
  return screen;
}

// $9A5C
static void noise_effect_9a5c(chqstate_t *state, uint8_t counter)
{
  uint8_t  x;               // was A
  char     character;       // was D
  uint8_t  C;
  int      carry = 0;
  uint8_t  B;
  uint8_t *HLnoisebytes;
  uint16_t DEscreen;
  uint16_t DEscreen_saved;
  uint8_t  A;               // was A

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
    DEscreen_saved = DEscreen; // was PUSH
    HLnoisebytes = &state->noise_bytes[0];
    do {
      A = *HLnoisebytes - B;
      *HLnoisebytes++ = A;
      RRC(A); // FIXME should be RLC(A);
      A += *HLnoisebytes;
      *HLnoisebytes = A;
      state->screen[DEscreen - SCREEN_START_ADDRESS] = A;
      DEscreen++; // was E++
    } while (--B > 0);
    DEscreen = DEscreen_saved; // was POP - restore row ptr
    DEscreen = nextrow(DEscreen);
  } while (--C > 0);

  ne_plot_attrs(state, 0x47); // BRIGHT + white over black
  // was FALLTHROUGH
}

// $9A98
static void ne_plot_attrs(chqstate_t *state, uint8_t attr)
{
  uint16_t HL;
  uint8_t  B;
  uint16_t DE;

  HL = 0x5836 - 0x5800; // Screen attribute (22,1) (Conv: address -> offset)
  B = 5; // 5 rows
  DE = 32 - 3; // row skip
  do {
    // Conv: Screen write now goes via state.
    state->screen[HL++] = attr;
    state->screen[HL++] = attr;
    state->screen[HL++] = attr;
    state->screen[HL  ] = attr;
    HL += DE;
  } while (--B > 0);
}

// $9AAB
static void plot_face(chqstate_t *state,
                      const uint8_t    *HLface,
                      uint16_t    DEscreen)
{
  uint16_t DEscreen_saved;
  uint16_t counter; // was BC
  uint8_t  A;
  int      carry = 0;

  counter = FACEBITMAPBYTES;
  DEscreen_saved = DEscreen;
  DEscreen -= SCREEN_START_ADDRESS; // Conv: address -> offset
  for (;;) {
    state->screen[DEscreen++] = *HLface++; counter--;
    state->screen[DEscreen++] = *HLface++; counter--;
    state->screen[DEscreen++] = *HLface++; counter--;
    state->screen[DEscreen++] = *HLface++; counter--;
    DEscreen -= 4; // replaces PUSH/POP
    if (counter == 0)
      break;
    DEscreen = nextrow(DEscreen);
  }

  DEscreen = DEscreen_saved;
  A = DEscreen >> 8;
  RRC(A);
  RRC(A);
  RRC(A);
  A &= 3; // extract band
  A += 0x58;
  DEscreen = (A << 8) | (DEscreen & 0xFF);
  DEscreen -= SCREEN_START_ADDRESS; // Conv: address -> offset
  counter = FACEATTRBYTES;
  for (;;) {
    state->screen[DEscreen++] = *HLface++; counter--;
    state->screen[DEscreen++] = *HLface++; counter--;
    state->screen[DEscreen++] = *HLface++; counter--;
    state->screen[DEscreen++] = *HLface++; counter--;
    if (counter == 0)
      break;

    int t = (DEscreen & 0xFF) + 0x1C;
    DEscreen = (DEscreen & 0xFF00) | (t & 0xFF);
    if (t >= 0x100)
      DEscreen += 256;
  }
}

// $9AEC
static void plot_mini_font_cursor_off(chqstate_t *state,
                                      uint8_t     x,
                                      char        character)
{
  pmf_go(state, x, character, ________, ________);
}

// $9AF1
static void plot_mini_font_cursor_on(chqstate_t *state,
                                     uint8_t     x,
                                     char        character)
{
  pmf_go(state, x, character, _____XXX, X_______);
}

// $9AF4
static void pmf_go(chqstate_t *state,
                   uint8_t     x,     // was A
                   char        ascii, // was D
                   uint8_t     B,     // extra bitmap
                   uint8_t     C)
{
  int            carry = 0;

  uint8_t        extra2;  // was self modified $9B61 - right extra bitmap
  uint8_t        extra1;  // was self modified $9B64 - left extra bitmap
  uint8_t        mask;    // was self modified $9B89
  uint8_t        rotate;  // was self modified $96B7

  uint8_t        A;
  uint8_t        ascii2; // was A
  uint8_t        row;    // was A
  uint8_t        gid;    // was C
  uint8_t        sgid;   // was A
  uint16_t       screen; // was DE
  const uint8_t *DEfont;
  uint16_t       HLscreen;

#define MFWIDTH  (5)
#define MFHEIGHT (6)

  extra1 = B; // ORed with hi font bytes
  extra2 = C; // lo font bytes
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
    screen++; // was E++
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
  gid += ascii2 - '/'; // not convinced this is ever used in the game

pmf_have_glyph_id:
  sgid = gid + 'A'; // Turn the glyph ID in #REGc into ASCII in #REGa

pmf_have_ascii:
  DEfont = &minifont[(sgid - 'A') * MFHEIGHT];
  HLscreen = screen; // was EX
  row = MFHEIGHT;
  do {
    uint8_t bm2; // was C
    uint8_t bm1; // was B

    bm2 = extra2;
    bm1 = *DEfont | extra1; // first pixel written

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

    uint8_t *screen = SCREEN(HLscreen); // Conv: added
    screen[0] = (mask & screen[0]) | bm1;
    screen[1] = bm2;
    DEfont++;
    HLscreen = nextrow(HLscreen);
  } while (--row > 0);
}

// $9BA7
static void clear_message_line(chqstate_t *state)
{
  uint16_t screen; // was HL
  uint8_t  rows;   // was A

  screen = 0x45C1; // Screen coordinate (8,53)
  rows = 6; // Clear six rows
  do {
    memset(SCREEN(screen + 1), 0, 29); // Conv: Replacing LDIR
    screen = nextrow(screen);
  } while (--rows);
}

// $9BCF
static void tick(chqstate_t *state)
{
  int      carry = 0;
  uint8_t *ptimebcd;    // was HL
  uint8_t  timeupstate; // was A
  uint8_t  timebcd;     // was A
  char    *timedigits;  // was DE
  uint8_t  effect;      // was B
  uint8_t  H;
  uint8_t  L;
  uint8_t  A;
  uint8_t  hidigit;      // was A
  uint8_t  lodigit;      // was L

  if (state->perp_caught_phase > PERPCAUGHTPHASE_0
      || state->transition_control == 4)
    return;

  ptimebcd = &state->st.time_bcd;
  timeupstate = state->time_up_state;
  switch (timeupstate) {
  case TIMEUPSTATE_CHECK_TIME_UP: goto check_time_up;
  case TIMEUPSTATE_CAR_STOPPED:   goto check_credits;
  case TIMEUPSTATE_CHECK_RESTART: goto check_restart;
  case TIMEUPSTATE_WAITING:       return;
  }

  // otherwise it's state 0

  if (*ptimebcd == 0) {
    // Ran out of time
    state->time_up_state      = TIMEUPSTATE_CHECK_TIME_UP;
    // Stop acceleration/brake/turbo/pause
    state->st.user_input_mask = USERINPUT_RIGHT | USERINPUT_LEFT | USERINPUT_FIRE |
                                USERINPUT_QUIT;
    return;
  }

update_remaining_time:
  if (--state->st.time_sixteenths > 0)
    return;
  state->st.time_sixteenths =
    15; // is this sixteenths or fifteenths since we reset to 15?
  state->st.time_bcd = timebcd = DAA(state->st.time_bcd - 1, &carry);
  if (timebcd == 0x15)
    // suss: passes timebcd(A) as priority...
    start_chatter(state, timebcd, chatterblk_nancy_time_running_out); // exit via
  return;

check_time_up:
  if (*ptimebcd != 0) {
    state->time_up_state      = TIMEUPSTATE_INIT;
    state->st.user_input_mask = USERINPUTMASK_ALLOW_ALL;
    goto update_remaining_time;
  }

  setup_overlay_messages(state, &time_up_message[0]);
  if (state->speed > 0)
    return;
  state->time_up_state = TIMEUPSTATE_CAR_STOPPED;
  // TODO play_speech_hook(state, 4);

check_credits:
  if (state->transition_control > 0)
    return;
  if (state->credits == 0) {
    // TODO cui_quit_key(state); // exit via
  } else {
    state->credits--;
    state->credit_n[7]   = (state->credits + '0') | STREND;
    state->time_up_state = TIMEUPSTATE_CHECK_RESTART;
    state->SM_9c85       = 0x15; // seconds remaining BCD?
    state->SM_9c86       = 0x01; // causes set_digits to run once?
  }
  return;

check_restart:
  if ((state->user_input & USERINPUT_FIRE) == 0)
    goto print_continue;

  // Resetting mission code.
  state->time_up_state      = TIMEUPSTATE_INIT;
  state->smash_level        = 0;
  state->smash_counter      = 0;
  state->st.user_input_mask = USERINPUTMASK_ALLOW_ALL;
  state->gear_lockout       = 3;
  state->transition_control = 3;
  state->st.turbos          = 3;
  state->st.time_bcd        = 0x60; // 60 seconds
  state->retry_count++;

// play_start_noise:  (code elsewhere jumps to this)
  // TODO play_speech_hook(state, 5); // exit via
  return;

print_continue:
  setup_overlay_messages(state, &continue_messages[0]);
  L = state->SM_9c85;
  H = state->SM_9c86 - 1;
  if (H != 0)
    goto set_digits;

  H = 6; // reset to 6 for when storeda - delay?
  L--;
  A = L;
  RR(A);
  effect = (carry) ? EFFECT_BIP : EFFECT_BOW;
  // TODO start_sfx(state, effect, 1);  // 1 for high priority
  A = L;
  if (A == 0) {
    state->quit_state    = QUITSTATE_START;
    state->time_up_state = TIMEUPSTATE_WAITING;
  }

set_digits:
  state->SM_9c85 = L;
  state->SM_9c86 = H;
  A = L;
  SRL(A);
  timedigits = &state->time_nn[5]; // Load address of nn in "TIME nn"
  if (A == 10) {
    hidigit = '1'; // ASCII
    lodigit = 0;   // integer
  } else {
    lodigit = A;   // integer
    hidigit = ' '; // ASCII
  }

  timedigits[0] = hidigit; // write first digit (must be ASCII)
  timedigits[1] = (lodigit + '0') | STREND;
}

// $9CC2
static void speed_score(chqstate_t *state)
{
  uint16_t speed; // was HL
  uint8_t  A;
  uint8_t  H;
  int      carry = 0;

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
  increment_score(state, A, 0, 0); // exit via
}

// $9CD6
// Bug: As soon as a non-zero->zero transition is seen the routine finishes so
// you can only have a single run of zeroes in the bonus.
static void add_bonus(chqstate_t *state,
                      uint8_t     A_lo,
                      uint8_t     E_md,
                      uint8_t     D_hi)
{
  char   *output;      // was HL
  uint8_t nonzeroflag; // was C - used to track if a zero has been emitted.

  output = &state->bonus_string[6]; // points to byte after buffer
  nonzeroflag = 0xFF; // flag (zero not seen)
  (void) bonus_digit(A_lo >> 0, &nonzeroflag,
                     &output); // always runs since flag > 0
  *output |= STREND; // terminate string

  // Using lazy evaluation here to avoid having a load of gotos
  (void) (bonus_digit(A_lo >> 4, &nonzeroflag, &output) >= 0 &&
          bonus_digit(E_md >> 0, &nonzeroflag, &output) >= 0 &&
          bonus_digit(E_md >> 4, &nonzeroflag, &output) >= 0 &&
          bonus_digit(D_hi >> 0, &nonzeroflag, &output) >= 0 &&
          bonus_digit(D_hi >> 4, &nonzeroflag, &output) >= 0);

  state->SM_address_of_score_digits = output;
  state->trigger_bonus_flag = 1;
  increment_score(state, A_lo, E_md, D_hi); // was fallthrough
}

// Subroutine of above broken out
static int bonus_digit(uint8_t  digit,
                       uint8_t *nonzeroflag,
                       char   **poutput)
{
  digit &= 0x0F;

  if (digit != 0)
    goto non_zero;
  else if (*nonzeroflag != 0)
    goto store;
  else
    // Conv: Was a POP+JP to cause exit.
    return -1; // We saw a non-zero-to-zero transition, so terminate.

non_zero:
  *nonzeroflag = 0; // Set flag to zero now we've seen a non-zero digit
store:
  (*poutput)--;
  **poutput = digit + '0';
  return 0;
}

// $9D17
static void increment_score(chqstate_t *state,
                            uint8_t     A_lo,
                            uint8_t     E_md,
                            uint8_t     D_hi)
{
  int      carry = 0;
  uint8_t *scorebcd; // was HL
  uint8_t  A;

  scorebcd = &state->score_bcd[0];
  A = A_lo + *scorebcd;
  *scorebcd++ = DAA(A, &carry);
  A = E_md + *scorebcd + carry;
  *scorebcd++ = DAA(A, &carry);
  A = D_hi + *scorebcd + carry;
  *scorebcd++ = DAA(A, &carry);
  A = *scorebcd + carry;
  *scorebcd = DAA(A, &carry);
}

// $9D2E
static void calc_overtake_bonus(chqstate_t *state)
{
  int      carry = 0;
  uint8_t  A;
  uint8_t  B;
  uint8_t *HL;

  A = state->overtake_bonus_counter;
  if (A == 0)
    return;

  B = A;
  HL = &state->overtake_bonus_bcd;
  // Increment bonus by 2 up to a max of 128.
  do {
    A = DAA(*HL + 2, &carry);
    if (A >= 0x80) A = 0x80;
    *HL = A;
    // Set bonus to N * 100.
    add_bonus(state, 0, A, 0);
  } while (--B > 0);

  state->overtake_bonus_counter = 0;
}

// $9D62
static void update_scoreboard(chqstate_t *state)
{
  toggle_light_brightness(state, SCREEN(0x5820));
  toggle_light_brightness(state, SCREEN(0x583B));
  plot_turbos_and_scores(state);
}

// $9DF4
static void toggle_light_brightness(chqstate_t *state, uint8_t *HL)
{
  uint8_t B;
  uint8_t C;

  B = 4; // rows
  C = ATTRIBUTE_BRIGHT;
  do {
    *HL++ ^= C;
    *HL++ ^= C;
    *HL++ ^= C;
    *HL++ ^= C;
    *HL   ^= C;
    HL += (32 - 5);
  } while (--B > 0);
}

// $9E11
static void plot_turbos_and_scores(chqstate_t *state)
{
  int             carry;
  uint8_t         Aturbos;
  uint8_t         Cturbos;
  uint8_t         Aboost;
  const uint8_t  *HLbitmap;
  uint8_t         Aframe;
  uint8_t         B;
  const uint16_t *SM_9e45;
  uint8_t         A;
  const uint16_t *SPbitmap;
  uint8_t        *HLscreen;
  uint16_t        DEbitmap;
  uint8_t        *DEscreen;
  uint16_t        DEdash_speed;
  uint8_t         Bdash_iterations;
  uint8_t         Ascale;
  uint16_t        HLdash;
  uint16_t        BCdash;
  uint8_t         Ddash;
  uint8_t         Edash;
  uint8_t        *DEbcd;
  uint16_t        HLdistance;
  uint16_t        BCdivisor;

  Aturbos = state->st.turbos;
  if (Aturbos) {

    Cturbos = Aturbos;
    Aboost = state->boost;
    HLbitmap = &bitmap_turbospin[0];
    if (Aboost == 0)
      goto ptas_turbo_setup;

    Aframe = state->SM_9e22 + 1;
    if (Aframe == 3)
      Aframe = 0;
    state->SM_9e22 = Aframe;
    if (Aframe == 0)
      goto ptas_turbo_setup;

    // Calculate the frame address
    B = Aframe;
    do { HLbitmap += 56; } while (--B > 0);

ptas_turbo_setup:
    SM_9e45 = (const uint16_t *) HLbitmap; // local
    //SM_9e79 = SP; // save old SP

    A = 0xE1; // Low byte of back buffer draw address
    do {
      SPbitmap = (const uint16_t *) &bitmap_turbospin[0];
      Cturbos--;
      if (Cturbos == 0)
        SPbitmap = SM_9e45;
      Cturbos++;
      HLscreen = BACKBUF(0xFE00 | A);
      // EX AF,AF'
      B = TURBOHEIGHT;
      do {
        uint8_t Emask, Dbitmap;

        DEbitmap = *SPbitmap++; // POP DEbitmap
        Emask = DEbitmap & 0xFF;
        Dbitmap = DEbitmap >> 8;
        *HLscreen = (*HLscreen & Emask) | Dbitmap;
        HLscreen++;

        DEbitmap = *SPbitmap++; // POP DEbitmap
        Emask = DEbitmap & 0xFF;
        Dbitmap = DEbitmap >> 8;
        *HLscreen = (*HLscreen & Emask) | Dbitmap;
        HLscreen--;

        // Is this advancing a screen or a backbuffer pointer?

        // FIXME row advance
        //A = H;
        //H--;
        //A &= 15;
        //JP NZ;
        //A = H;
        //A += 0x10;
        //H = A;
        //A = L;
        //A -= 0x20;
        //L = A;
        //JP NC;
        //A = H;
        //A -= 0x10;
        //H = A;
      } while (--B > 0);
      // EX AF,AF'
      A += 2;
    } while (--Cturbos > 0);

    // LD SP was here
  }

  DEscreen = SCREEN(0x4132); // speed digits pos (144,9)
  // EXX
  DEdash_speed = state->speed;

  // Scale speed by 82%
  HLdash = 0;
  Bdash_iterations = 7; // iterations
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
  ledfont_plot(state, Ddash, DEscreen); // draw 10,000s
  ledfont_plot(state, Edash, DEscreen); // draw  1,000s
  ledfont_plot(state, A,     DEscreen); // draw    100s

  // Time
  // EXX
  ptas_led_digits(state, 1, &state->st.time_bcd, &state->st.time_digits[1],
                  SCREEN(0x412F)); // (120,9)

  // Distance (to perp)

  DEbcd = &state->distance_bcd[1];
  // TBD17 is the high byte of the distance
  HLdistance = (state->hazards[0].TBD17 << 8) | state->hazards[0].distance;

  // Count 1,000s (no loop required)
  BCdivisor = 1000;
  HLdistance -= BCdivisor; // TODO set carry
  A = 0x10; // BCD
  if (carry) {
    HLdistance += BCdivisor; // correct overshoot
    A = 0x00; // BCD
  }

  // Count 100s
  BCdivisor = 100;
  do {
    A++;
    HLdistance -= BCdivisor;
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
    // AND A
    HLdistance -= BCdivisor;
  } while (!carry);
  HLdistance += BCdivisor; // correct overshoot

  A |= HLdistance & 0xFF; // OR in remainder
  DEbcd[-1] = A;

  ptas_led_digits(state, 2, &state->distance_bcd[1],
                  &state->st.distance_digits[3],
                  SCREEN(0x4191)); // was fallthrough

  ptas_led_digits(state, 4, &state->score_bcd[3], &state->st.score_digits[7],
                  SCREEN(0x4126)); // was fallthrough
}

// $9F1E
//
// Biterations was B
// DEdigits was DE
// HLstored was HL
// DEscreen was DE'
static void ptas_led_digits(chqstate_t    *state,
                            uint8_t        Biterations,
                            const uint8_t *DEdigits,
                            uint8_t       *HLstored,
                            uint8_t       *DEscreen)
{
  uint8_t Adigits;
  uint8_t Cdigits;

  do {
    Adigits = *DEdigits;
    Cdigits = Adigits; //tmp copy

    Adigits = Adigits >> 4;
    if (Adigits != *HLstored)
      goto ptas_led_plot_1st;
    DEscreen++; // move screen pos
ptas_led_next_half:
    HLstored--;
    Adigits = Cdigits & 0x0F;
    if (Adigits != *HLstored)
      goto ptas_led_plot_2nd;
    DEscreen++; // move screen pos

ptas_led_next_whole:
    HLstored--;
    DEscreen--;
  } while (--Biterations > 0);
  return;

ptas_led_plot_1st:
  *HLstored = Adigits;
  ledfont_plot(state, Adigits, DEscreen);
  goto ptas_led_next_half;

ptas_led_plot_2nd:
  *HLstored = Adigits;
  ledfont_plot(state, Adigits, DEscreen);
  goto ptas_led_next_whole;
}

#define LEDFONT_HEIGHT (15)

// $9F47
//
// ord (was A)
// screen (was DE')
static uint8_t *ledfont_plot(chqstate_t *state, uint8_t ord, uint8_t *screen)
{
  const uint8_t *font;        // was HL
  uint8_t       *screen_copy; // was stacked

  font = &ledfont[ord * LEDFONT_HEIGHT];
  screen_copy = screen;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  screen = screen_copy - 256 + 32 - 1;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  *screen = *font++; screen += 256;
  return screen_copy + 1; // move to next column
}

// $9F99
static void draw_string_A(chqstate_t    *state,
                          uint8_t        A,
                          const uint8_t *BCstring,
                          uint8_t       *DEscreen)
{
  draw_string_entry(state, DEscreen, BCstring/*HL*/, 0/*Adash*/, A/*C'*/,
                    32/*DE'*/, NULL); // FIXME: NULL needs to be attr ptr
}

// $9FA3
static void draw_string(chqstate_t    *state,
                        uint8_t        A,
                        const uint8_t *BCstring,
                        uint8_t       *DEscreen)
{
  draw_string_entry(state, DEscreen, BCstring/*HL*/, 1/*Adash*/, A/*C'*/,
                    32/*DE'*/, NULL);
}

// $9FA6
static void draw_string_entry(chqstate_t    *state,
                              uint8_t       *DEscreen,
                              const uint8_t *HLstring,
                              uint8_t        Adash,
                              uint8_t        Cdash,
                              uint8_t        DEstride, // was DE'
                              uint8_t       *HLattr)
{
  uint8_t Achar;

  do {
    Achar = *HLstring & ~STREND;
    draw_char(state, Achar, DEscreen, Adash, Cdash, DEstride, HLattr);
  } while ((Achar & STREND) == 0);
}

// $9FB4
static void draw_char(chqstate_t *state,
                      uint8_t     Achar,
                      uint8_t    *DEscreen, // screen address
                      uint8_t     Adash, // draw type
                      uint8_t     Cdash, // attribute
                      uint8_t     DEstride, // DE' e.g. 32 - a stride?
                      uint8_t    *HLdash)
{
  uint8_t        Cglyphid; // was C
  uint8_t        Ctype;    // was C
  uint8_t        A;
  uint8_t        B;
  const uint8_t *HLfont;   // was HL
  uint8_t       *DEorig;   // was stacked

  Achar -= ' ';
  if (Achar == 0) {
    // Space
    DEscreen++;
    HLdash++; // FIXME: Do we need to return these?
    return;
  }

  // Map ASCII to glyph IDs
  Cglyphid = 0x12;
  if (Achar >= ('A' - ' ')) goto dc_have_range;
  Cglyphid = 0x0B;
  if (Achar >= ('0' - ' ')) goto dc_have_range;
  Cglyphid = 0;
  Achar--;
  if (Achar == 0) goto dc_have_single;
  Cglyphid++;
  Achar -= 7;
  if (Achar == 0) goto dc_have_single;
  Cglyphid++;
  Achar--;
  if (Achar == 0) goto dc_have_single;
  Cglyphid++;
  Achar -= 3;
  if (Achar == 0) goto dc_have_single;
  Cglyphid++;
  goto dc_have_single;

dc_have_range:
  Cglyphid = Achar - Cglyphid;

dc_have_single:
  HLfont = &font[Cglyphid * 7]; // add symbol for glyph height

  Ctype = Adash;
  Ctype--;
  if (Ctype == 0) goto dc_generic; // 1
  Ctype--;
  if (Ctype == 0) goto dc_single_height; // 2
  Ctype--;
  if (Ctype == 0) goto dc_double_height; // 3
  Ctype--;
  if (Ctype == 0) goto dc_single_height_inverted; // 4
  Ctype--;
  if (Ctype == 0) goto dc_double_height_inverted; // 5

  // Otherwise it's type 0 or anything else
  DEorig = DEscreen;
  B = 4; // iterations
  do {
    A = *HLfont;
    *DEscreen = A;
    DEscreen += 256;
    *DEscreen = A;
    DEscreen += 256;
    HLfont++;
  } while (--B > 0);
  DEscreen -= 8 * 256;
  DEscreen += 32;
  B = 3; // iterations
  do {
    A = *HLfont;
    *DEscreen = A;
    DEscreen += 256;
    *DEscreen = A;
    DEscreen += 256;
    HLfont++;
  } while (--B > 0);
  goto dc_set_double_attrs;

  // double height inverted
dc_double_height_inverted:
  DEorig = DEscreen;
  B = 7;
  do {
    A = ~*HLfont;
    *DEscreen = A;
    DEscreen += 256;
    *DEscreen = A;
    DEscreen += 256;
    HLfont++;
  } while (--B > 0);
  goto dc_set_double_attrs;

dc_single_height_inverted:
  DEorig = DEscreen;
  B = 7;
  do {
    A = ~*HLfont;
    *DEscreen = A;
    HLfont++;
    DEscreen += 256;
  } while (--B > 0);
  goto dc_set_single_attrs;

// Plots double-height glyphs. DEscreen->screen HLfont->glyph def
dc_double_height:
  DEorig = DEscreen;
  *DEscreen = 0; // leave gap at top
  DEscreen += 256;
  for (int i = 0; i < 7; i++) { // Conv: rolled
    A = *HLfont;
    *DEscreen = A;
    DEscreen += 256;
    *DEscreen++ = *HLfont++; // was LDI, could reuse A
    DEscreen--; // was DEC E, could remove if DEscreen++ above is dropped
    DEscreen += 256;
  }
  *DEscreen = 0; // leave gap at bottom

dc_set_double_attrs:
  DEscreen = DEorig + 1; // was POP DEscreen, INC E
  *HLdash |= Cdash;
  HLdash += DEstride;
  *HLdash |= Cdash;
  HLdash -= DEstride; // was POP HLdash
  HLdash++; // was INC L
  return;

dc_single_height: // seems to store 9 rows
  DEorig = DEscreen;
  *DEscreen = 0; // leave gap at top
  DEscreen += 256;
  for (int i = 0; i < 7; i++) { // Conv: rolled
    *DEscreen++ = *HLfont++;
    DEscreen--; // could drop
    DEscreen += 256;
  }
  *DEscreen = 0; // leave gap at bottom

dc_set_single_attrs:
  DEscreen = DEorig + 1; // was POP DEscreen
  *HLdash |= Cdash;
  HLdash++; // was INC L
  return;

dc_generic:
  DEorig = DEscreen;
  B = 7; // iterations
  do {
    *DEscreen = *HLfont;
    DEscreen += 256;
    HLfont++;

    // variation on nextrow()
    // DEscreen = nextrow(DEscreen); // won't work!
  } while (--B > 0);
  DEscreen = DEorig + 1; // was POP DEscreen
  return;
}

// $A0D6
static uint8_t keyscan(chqstate_t *state)
{
  return 0;
}

// $A399
static void check_scenery_collisions(chqstate_t *state)
{
}

// $A579
static void layout_objects(chqstate_t *state)
{
}

// $A60E
static void cycle_counters(chqstate_t *state)
{
  state->counter_A = (state->counter_A + 1) & 3;
  state->counter_B = (state->counter_B + 1) & 1;
  if (state->counter_B == 0)
    return;
  state->counter_C = (state->counter_C + 1) & 3;
}

// $A7F3
static void spawn_cars(chqstate_t *state)
{
}

// $A955
static void choose_dirt_and_stones(chqstate_t *state)
{
}

// $A97E
static void layout_dirt_and_stones(chqstate_t *state)
{
}

// $AAC6
static void move_helicopter(chqstate_t *state)
{
}

// $AB33
static void drive_helicopter(chqstate_t *state)
{
}

// $AB9A
static void spawn_hazards(chqstate_t *state)
{
}

// $ADA0
static void draw_hazards(chqstate_t *state)
{
}

// $B063
static void move_hero_car(chqstate_t *state)
{
}

// $B318
static void animate_hero_car(chqstate_t *state)
{
}

// $B848
static void scroll_horizon(chqstate_t *state)
{
}

// $B8D2
static void update_road_level(chqstate_t *state)
{
}

#define ROADBUF(N) \
  ((state->road_buffer_offset + (N) - state->road_buffer_start) & 0xFF)

#define GETNEXTROADBUF(R) \
  do { \
    if (++(R) == state->road_buffer_end) \
    (R) = state->road_buffer_start; \
  } while (0)

#define ROADBUF_LANES_OFFSET (64)

// $B9F4
static void layout_road(chqstate_t *state)
{
  int            carry = 0;
  uint8_t       *DElanedata;
  uint8_t        Biterations;
  uint8_t        Lcounter;
  uint16_t      *SProadright;
  uint8_t        Aiterations;
  uint8_t        Aforkinprogress;
  uint16_t      *SMroadcentre;
  uint16_t      *SMroadcentreright;
  uint16_t      *SMroadcentreleft;
  uint16_t      *SMroadleft;
  uint16_t      *SMroadright;
  uint16_t      *SMveryright;
  uint16_t       BCdash;
  uint16_t       DEdash;
  uint16_t       HLdash;
  uint16_t       DEroadpos;
  const uint8_t *HLchatterblk;
  uint8_t        Ca16d;
  uint16_t       HLforkdistance;
  uint16_t       DEforkdistance;
  uint16_t       HLroadpos;
  uint16_t       HLroadpos_saved;
  uint8_t       *HLunknown;

  // point at lane data
  DElanedata = &state->road_buffer_start[ROADBUF(ROADBUF_LANES_OFFSET)];

  // Count the distance to the forked road.
  Biterations = 20; // iterations
  Lcounter = 0; // counter
  do {
    if ((*DElanedata & 0xE1) == 0xE1)
      goto lr_forked_road;
    GETNEXTROADBUF(DElanedata);
    Lcounter++;
  } while (--Biterations > 0);

  // No forked road found.
  build_curve_table(state, /*forked=*/0);
  // $E800 now contains the left edges and $EC00 contains the right edges.
  SProadright = &state->table_ec00[0x30];
  Aiterations = 0x30; // 48..256 in steps of 2 = 104 iterations
lr_calc_single_lane:
  do {
    SMroadcentre      = &state->table_ea00[Aiterations];
    SMroadcentreright = &state->table_eb00[Aiterations];
    SMroadcentreleft  = &state->table_e900[Aiterations];
    SMroadleft        = &state->table_e800[Aiterations];

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
  HLunknown = &state->table_e300[Lcounter];
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
  DEroadpos = state->road_pos;
  Aiterations = 1;
  DEroadpos -= 256; // was DEC D
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
  Aiterations += state->st.var_a16d;
  Ca16d = Aiterations; // new value for $A16D
  Aiterations -= 2;
  if (Aiterations >= 256 - 2) // carried?
    goto lr_set_var_a16d_from_c;
  Ca16d = Aiterations; // new value for $A16D
  HLforkdistance += 16;
  state->fork_distance = HLforkdistance;
lr_set_var_a16d_from_c:
  state->st.var_a16d = Ca16d;
lr_no_car_spawning:
  carry = state->st.var_a16d & 1; // CHECK
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
    DEforkdistance = HLforkdistance; // was POP DEforkdistance
    HLroadpos = state->road_pos;
    HLroadpos_saved = HLroadpos; // was PUSH HLroadpos
    HLroadpos += DEforkdistance;
    state->road_pos = HLroadpos; // adjust road pos for fork rendering
    build_curve_table(state, /*forked=*/1);
  } else {
    build_curve_table(state, /*forked=*/1);
    DEforkdistance = HLforkdistance; // was POP DEforkdistance
    HLroadpos = state->road_pos;
    HLroadpos_saved = HLroadpos; // was PUSH HLroadpos
    HLroadpos -= DEforkdistance;
    state->road_pos = HLroadpos; // adjust road pos for fork rendering
    build_curve_table(state, /*forked=*/0);
  }
  // $BB07
  HLroadpos = HLroadpos_saved; // was POP HLroadpos
  state->road_pos = HLroadpos; // restore normal road pos after fork rendering
  // POP BC
  // (set SP restoring op)
  SProadright = &state->table_ec00[0x30]; // (set SP to $EC30)
  Aiterations = 0x30; // 48..256 in steps of 2 = 104 iterations
  do {
    SMroadcentre      = &state->table_ea00[Aiterations];
    SMroadcentreleft  = &state->table_e900[Aiterations];
    SMroadleft        = &state->table_e800[Aiterations];
    SMveryright       = &state->table_ed00[Aiterations]; // output right?
    SMroadcentreright = &state->table_eb00[Aiterations]; // output left?
    SMroadright       = &state->table_ec00[Aiterations];
    // EXX Bank for inner loop
    DEdash = *SMroadleft;
    HLdash = *SProadright++; // POP HLdash // read from $ECxx
    *SMroadcentre      = (HLdash + DEdash) / 2; // (right+left)/2 = new road centre)
    *SMroadcentreleft  = (HLdash + DEdash) / 2; // new road centre left

    DEdash = *SMveryright;
    HLdash = *SMroadcentreright;
    *SMroadcentreright = (HLdash + DEdash) / 2;
    *SMroadright       = (HLdash + DEdash) / 2;

    Aiterations += 2;
    // EXX Unbank
  } while (Biterations > 0);
  SProadright = &state->table_ec00[Aiterations];
  goto lr_calc_single_lane; // jump into no_fork code
}

// $BB69
static void exit_fork(chqstate_t *state)
{
}

// The screen has the format 0b010BBLLLRRRCCCCC (B = band, L = scanline, R = row (group), C = column)
// The buffer has the format 0b1111LLLLRRRCCCCC (L = scanline, R = row (group))

// $BC3E
static void draw_screen(chqstate_t *state)
{
  uint8_t  *scr;      // was HL
  uint8_t  *buf;      // was HL'
  uint16_t  offset;   // added
  uint16_t  BCattrs;
  uint16_t  DEstride;
  uint8_t   Cattr;
  uint8_t  *HLattrs;
  uint8_t   A;
  uint8_t   D;
  uint8_t   E;

  scr = SCREEN(0x4811); // (136, 64)
  buf = BACKBUF(0xF001); // (8, 1)
  for (;;) {
ds_loop_16bytes:
    memcpy(scr, buf, 16); scr += 256; buf += 256;
    memcpy(scr, buf, 16); scr += 256; buf += 256;
    memcpy(scr, buf, 16); scr += 256; buf += 256;
    memcpy(scr, buf, 16); scr += 256; buf += 256;
    offset = UNBACKBUF(buf); // Conv: convert back to offset
    // Loop on the first pass (4 lines of 8 done) but not the second
    if (offset & (1 << 10))
      goto ds_loop_16bytes;

    // Move to right hand side?
    buf = BACKBUF(UNBACKBUF(buf) -
                  0x07F0); // e.g. (0xF001 + 8*256 - 0x7F0) == 0xF011 on the first pass
    scr = SCREEN(UNSCREEN(scr) - 0x07F2);

ds_loop_14bytes:
    memcpy(scr, buf, 14); scr += 256; buf += 256;
    memcpy(scr, buf, 14); scr += 256; buf += 256;
    memcpy(scr, buf, 14); scr += 256; buf += 256;
    memcpy(scr, buf, 14); scr += 256; buf += 256;
    offset = UNBACKBUF(buf); // Conv: convert back to offset
    // Loop on the first pass (4 lines of 8 done) but not the second
    if (offset & (1 << 10))
      goto ds_loop_14bytes;

    offset = UNBACKBUF(buf); // Conv: convert back to offset
    // Loop on the first pass (8 lines of 16 done) but not the second
    if ((offset & (1 << 11)) == 0) {
      // Otherwise we've rolled into to top nibble
      uint8_t tmp, lo, hi;

      hi = 0xF0;
      tmp = offset & 0xFF;
      lo = tmp - hi;
      if (tmp >= hi)
        goto ds_attributes;
      if (lo <= tmp) { // not overflowed
        scr = SCREEN(UNSCREEN(scr) - 0x07EE);
      } else { // overflowed
        scr = SCREEN(0x5011); // (136,128)
      }
    } else {
      // next scanline
      scr = SCREEN(UNSCREEN(scr) - 0x07EE);
      buf -= 0x10;
    }
  }

ds_attributes:
  // Attributes
  if (state->dont_draw_screen_attrs)
    return;

  uint16_t DE;

  A = state->horizon_table_e34b[1]; // -> horizon table?
  E = state->horizon_table_e34b[2]; // current value?
  state->horizon_table_e34b[2] = A;
  if (E != 0) { // if moved? some sort of previous/current behaviour here
    E = A * 4;
    D = (A * 4 >= 256) ? 0xFF : 0; // was SBC A,A - must be sign extending
    DE = (D << 8) | E;
    HLattrs = SCREEN(state->st.horizon_attribute);
    // Set sky colour
    BCattrs = (attribute_BLACK_OVER_BRIGHT_CYAN << 8) |
              attribute_BLACK_OVER_BRIGHT_CYAN;
    // If A was zero then jump (Z => sky, NZ => ground)
    if (D != 0) {
      // Set ground colour
      BCattrs = stgword(state, 0x5CF4); // load stage's ground_colour (pair of attrs)
      HLattrs += DE;
    }

    // Conv: Use memset and only use bottom byte of BCattrs
    memset(HLattrs, BCattrs, 30); // scr attr width -2
    if (D == 0)
      HLattrs += DE;

    state->st.horizon_attribute = SCREEN_START_ADDRESS + UNSCREEN(HLattrs);
  }

  /* Draw smash meter attributes */

  if (state->sighted_flag == 0 || state->perp_caught_phase >= PERPCAUGHTPHASE_3)
    return;

  HLattrs = SCREEN(0x5962); // attr (2,11)
  DEstride = 32;

  Cattr = attribute_BLACK_OVER_BRIGHT_RED;
  *HLattrs = Cattr; HLattrs += DEstride;
  *HLattrs = Cattr; HLattrs += DEstride;
  Cattr = attribute_BLACK_OVER_BRIGHT_MAGENTA;
  *HLattrs = Cattr; HLattrs += DEstride;
  *HLattrs = Cattr; HLattrs += DEstride;
  Cattr = attribute_BLACK_OVER_BRIGHT_GREEN;
  *HLattrs = Cattr; HLattrs += DEstride;
  *HLattrs = Cattr; HLattrs += DEstride;
  Cattr = attribute_BLACK_OVER_BRIGHT_WHITE;
  *HLattrs = Cattr; HLattrs += DEstride;
  *HLattrs = Cattr;
}

// $BDC1
static void clear_playfield_set_attrs(chqstate_t *state)
{
  int      B;
  uint8_t *screen; // was HL
  uint16_t DE;

  clear_playfield(state);

  // Clear the playfield pixels to $FF (bug: duplicates work just done)
  memset(SCREEN(0x4800), 0xFF, 0x1000);

  // Clear the playfield attributes to $28 (black over cyan) - first two
  // rows only
  memset(SCREEN(0x5900), 0x28, 2 * 32);

  // Clear the next three rows to $68 (black over bright cyan)
  memset(SCREEN(0x5940), 0x68, 3 * 32);

  // Clear the next 11 rows to the current ground colour
  // Note: Only using the bottom byte of ground_colour (as orig).
  memset(SCREEN(0x59A0), stgbyte(state, 0x5CF4), 0x160); // CHECK

  // Clear the edges of the playfield to black on black
  screen = SCREEN(0x5900);
  DE = 0x1F;
  B = 16;
  do {
    *screen = 0;
    screen += DE;
    *screen++ = 0;
  } while (--B > 0);
}

// $BDFB
static void read_map(chqstate_t *state)
{
}

// $C0E1
static void prepare_tunnel(chqstate_t *state)
{
}

// $C15B
static void draw_tunnel(chqstate_t *state, uint8_t *IY)
{
}

// $C2E7
static void draw_road_scene_change(chqstate_t *state, uint8_t *IX, uint8_t *IY)
{
}

// $C452
static void draw_road(chqstate_t *state)
{
}

#define BACKDROPWIDTH  (10) // bytes
#define BACKDROPHEIGHT (24) // rows

// $C8BE
static void pre_shift_backdrop(chqstate_t *state)
{
  int            tmp;        // for RRD()
  const uint8_t *source;     // was HL
  uint8_t       *preshifted; // was DE
  const uint8_t *endptr;     // was DE
  uint8_t       *bmptr;      // was HL
  uint8_t        row;        // was C
  uint8_t        col;        // was B
  uint8_t        pix;        // was A

  // Copy whole source bitmap to destination
  source     = stgmap(state, STAGEDATA_BASE);
  preshifted = &state->pre_shifted_backdrop[0];
  memcpy(preshifted, source, BACKDROPWIDTH * BACKDROPHEIGHT);

  // Shift it in-place by a nibble
  endptr = &source[BACKDROPWIDTH -
                   1]; // final scanline nibble rolls around to start
  bmptr  = preshifted;
  row    = BACKDROPHEIGHT;
  do {
    col = BACKDROPWIDTH;
    pix = *endptr;
    do {
      RRD(pix, bmptr);
      bmptr++;
      endptr++; // TODO: Not used in inner loop - could hoist
    } while (--col > 0);
  } while (--row > 0);
}

// $C8E3
static void forked_road_plotter(chqstate_t *state)
{
}

// mystery_cba4 would go here, if we knew what it did

// $CBC5
static void backdrop_fill_choice(chqstate_t *state)
{
}

// $CBD6 ish
static void build_curve_table(chqstate_t *state, int forked)
{
  uint16_t       *table1, *table2;
  const uint8_t  *road_buffer_ptr_HL; // was HL
  uint8_t         curvature_C; // was C
  int             A;
  int             B;
  const uint8_t  *IY;
  const uint16_t *IX;
  uint8_t        *DE;
  uint16_t        DEdash;
  uint8_t         curvature_A;
  uint16_t        HLdash;
  uint16_t        BCdash;
  int             carry;
  uint16_t        DEroadpos; // was DE
  uint8_t        *DEe320;
  const uint8_t  *HLe760;
  int             Bdash;

  // Set up table pointer to *end* of tables we're building.
  if (forked) {
    table1 = &state->table_ed00[128]; // was $EE00
    table2 = &state->table_eb00[128]; // was $EC00 - centre right table?
  } else {
    table1 = &state->table_ec00[128]; // was $ED00 - right table
    table2 = &state->table_e800[128]; // was $E900 - left table
  }

  road_buffer_ptr_HL = state->road_buffer_offset;
  curvature_C = *road_buffer_ptr_HL;

  A = state->fast_counter & 0xE0;
  A = A - (A >> 2) - (A >> 4); // map (0,32,64,96,...,224) to (0,22,44,66,...,154)
  IY = &horizontal_e6b0[A / 22][0];

  // now need high byte of offset from base of struct, seems to be $E6 or $E7
  A = 0xE6 + ((IY - &horizontal_e6b0[0][0]) >> 8);
  A = multiply(A, curvature_C);
  A = (128 - A) & 0xFE; // 0xFE must round to whole word
  // A expecting $7C to $82 depending on curvature (7C if bending right?)
  A = (A - 0x40) / 2; // adjust to index inward_bend_table
  assert(A >= 0 && A <= 95);
  IX = &inward_bend_table[A]; // table is 16-bit

  DE = &state->table_e320[0];
  B = 20; // iterations
  // EXX Bank
  DEdash = state->road_pos;
  // PUSH DEdash; // save on stack
  // EXX Unbank

  // Calculate table_e320
  do {
    curvature_A = *road_buffer_ptr_HL;
    if (forked)
      curvature_A = -curvature_A;
    assert(curvature_A >= 0 && curvature_A <= 255);
    printf("curvature_A=%d\n", curvature_A);

    if (++road_buffer_ptr_HL == state->road_buffer_end)
      road_buffer_ptr_HL = state->road_buffer_start;

    // EXX Bank

    int offset = (IX - &inward_bend_table[0]) * 2;
    offset = (offset & ~0xFF) + curvature_A; // - 0x40; ?
    IX = &inward_bend_table[offset / 2];
    assert(IX >= &inward_bend_table[0] && IX < &inward_bend_table[96]);

    HLdash = 0; // Initialise a multiplier result
    BCdash = *IX - DEdash;

    // reading first byte from table row?
    A = *IY++; // points into horizontal_e6b0

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

    *DE++ = A; // write to table_e320
  } while (--B);

  DEroadpos = state->road_pos; // was POP DE
  B = 0; // init counter
  // EXX Bank
  build_curve_table_sub_cca8(state,
                             B,
                             table1, // table1 is $EE00 or $ED00 (right hand table)
                             DEroadpos);

  // repeat of above code - generate left hand table

  A = state->fast_counter & 0xE0;
  A = A - (A >> 2) - (A >> 4); // map (0,32,64,96,...,224) to (0,22,44,66,...,154)

  HLe760 = &horizontal_e760[A / 22][0];
  DEe320 = &state->table_e320[0];
  for (Bdash = 22; Bdash > 0; Bdash--)
    *DEe320++ += *HLe760++;

  DEroadpos = DEroadpos - 295; // vanishing point config (for left hand)

  Bdash = 0; // init counter
  // EXX Unbank
  build_curve_table_sub_cca8(state,
                             Bdash,
                             table2, // table2 is $EC00 or $E900 (left hand table)
                             DEroadpos);
}

// HL -> points past end of destination table we're filling
static void build_curve_table_sub_cca8(chqstate_t *state,
                                       uint8_t     Bdash_alwayszero,
                                       uint16_t   *HLtableend,
                                       uint16_t    DEroadpos)
{
  uint8_t  *IYe300;
  int       Biterations;
  uint16_t *SPoutput;
  int       A;
  int       Bdash;
  int       Cdash;
  int       Ldash;
  int       Aopcode;
  int       Atotal;
  uint16_t  HLdash;

  IYe300 = &state->table_e300[0]; // was 0xE300; // addr of height table
  Biterations = 21; // iterations
  // (restore SP on exit, load SP with HL)
  SPoutput = HLtableend;
  do {
    // EXX Bank
    A = (Bdash_alwayszero - 2 + IYe300[0] - IYe300[1]) & 0xFF;
    IYe300++;
    if (A > 128) goto bct_endbit_negative; // if A is negative
    A += 2;
    state->object_positions[IYe300 - 1 - &state->table_e300[0]] =
      A; // must write to $E34F+ which is object_positions
    A -= Bdash;
    Bdash = A;
    Cdash = A;
    Ldash = state->table_e320[IYe300 - 1 -
                              &state->table_e300[0]]; // IY[$1F]; // $E320+
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
  state->object_positions[IYe300 - 1 - &state->table_e300[0]] = 1;
  if (++A != 0) A++;
  Bdash = A;
  A = state->table_e320[IYe300 - 1 - &state->table_e300[0]]; // IY[$1F]; // $E320+
  //Ldash = A;
  //carry = (A & (1<<7) != 0;
  //Hdash = -carry; //sign ext
  HLdash = (int8_t) A;
  HLdash += DEroadpos;
  DEroadpos = HLdash; // was EX
  SPoutput--; *SPoutput = DEroadpos; // PUSH to output table
  goto bct_continue;
}

// $CD3A
static void build_height_table(chqstate_t *state)
{
}

/* ----------------------------------------------------------------------- */

// $CDD6
static T multiply(T a, T c)
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

/* ----------------------------------------------------------------------- */

// $EC2C
static void menu_draw_char(chqstate_t *state,
                           uint8_t     Achar, // ASCII
                           uint8_t     Fdash, // dbl height if carry set
                           uint8_t     Cdash, // attribute byte
                           uint8_t    *DEdash, // screen address
                           uint8_t    *HLdash, // attribute address
                           uint8_t   **DEdash_out,
                           uint8_t   **HLdash_out)
{
  const uint8_t *HLfont;   //
  uint8_t       *DEscreen; //
  uint8_t        Cglyphid; //
  uint8_t       *HLdash_saved; // was B'

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
  DEdash++; // was INC E
  // EXX
  // POP DE
  DEscreen = DEdash;
  if (!Fdash) { // checking banked carry here
    // double height
    for (int i = 0; i < 4; i++) { // Conv: rolled
      *DEscreen = *HLfont;
      DEscreen += 256;
      *DEscreen = *HLfont++;
      DEscreen += 256;
    }
    DEscreen += 0xF81F;
    for (int i = 0; i < 3; i++) { // Conv: rolled
      *DEscreen = *HLfont;
      DEscreen += 256;
      *DEscreen = *HLfont++;
      DEscreen += 256;
    }
    *DEscreen = 0; // final row always blank?
    // EXX
    HLdash_saved = HLdash; // was just B' saving L'
    Cdash |= ATTRIBUTE_BRIGHT;
    *HLdash = Cdash; // set with bright set
    HLdash += 32; // move to next attr row
    Cdash &= ~ATTRIBUTE_BRIGHT; // set with bright clear
    *HLdash = Cdash;
    HLdash = HLdash_saved + 1;
    // EXX
  } else {
    // single height
    for (int i = 0; i < 7; i++) { // Conv: rolled
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

/* ----------------------------------------------------------------------- */

/**
 * Return the minimum of (a,b).
 */
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

/**
 * Return the maximum of (a,b).
 */
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

/**
 * Return 'a' clamped to the range [b..c].
 */
#define CLAMP(a,b,c) MIN(MAX(a,b),c)

typedef struct zxbox {
  int x0, y0, x1, y1;
}
zxbox_t;

/* Define to highlight dirty rectangles when they're drawn. */
//#define SHOW_DIRTY_RECTS

#define RGB

#ifdef RGB
/* 0x00RRGGBB */
#define BK_ 0x00000000
#define RD_ 0x00010000
#define GR_ 0x00000100
#define YL_ 0x00010100
#define BL_ 0x00000001
#define MG_ 0x00010001
#define CY_ 0x00000101
#define WH_ 0x00010101
#else
/* 0x00BBGGRR */
#define BK_ 0x00000000
#define RD_ 0x00000001
#define GR_ 0x00000100
#define YL_ 0x00000101
#define BL_ 0x00010000
#define MG_ 0x00010001
#define CY_ 0x00010100
#define WH_ 0x00010101
#endif

#define NORMAL(C) (C * 0xCD)
#define BRIGHT(C) (C * 0xFF)

/* BRIGHT 0 */
#define BKd NORMAL(BK_)
#define RDd NORMAL(RD_)
#define GRd NORMAL(GR_)
#define YLd NORMAL(YL_)
#define BLd NORMAL(BL_)
#define MGd NORMAL(MG_)
#define CYd NORMAL(CY_)
#define WHd NORMAL(WH_)
/* BRIGHT 1 */
#define BKb BRIGHT(BK_)
#define RDb BRIGHT(RD_)
#define GRb BRIGHT(GR_)
#define YLb BRIGHT(YL_)
#define BLb BRIGHT(BL_)
#define MGb BRIGHT(MG_)
#define CYb BRIGHT(CY_)
#define WHb BRIGHT(WH_)

/* Given a set of tokens: A, B, C, which have permutations:
 *   (AA, AB, AC, BA, BB, BC, CA, CB, CC)
 * They can be written out contiguously as:
 *   AAABACBABBBCCACBCC
 * We can remove and index the redundant overlaps:
 *   AABACBBCACC
 * (Note that 'AC' occurs twice in the sequence).
 * Producing a mapping:
 *   (0, 1, 3, 2, 5, 6, 7, 4, 9)
 * This is a De Bruijn sequence.
 *
 * The following palette is laid out in a De Bruijn sequence in a vain attempt
 * to improve performance.
 */
static const unsigned int palette[66 + 65] = {
  BKd, BKd, BLd, BKd, RDd, BKd, MGd, BKd,
  GRd, BKd, CYd, BKd, YLd, BKd, WHd, BLd,
  BLd, RDd, BLd, MGd, BLd, GRd, BLd, CYd,
  BLd, YLd, BLd, WHd, RDd, RDd, MGd, RDd,
  GRd, RDd, CYd, RDd, YLd, RDd, WHd, MGd,
  MGd, GRd, MGd, CYd, MGd, YLd, MGd, WHd,
  GRd, GRd, CYd, GRd, YLd, GRd, WHd, CYd,
  CYd, YLd, CYd, WHd, YLd, YLd, WHd, BKd,
  WHd, WHd,

  /* The zeroth bright entry is shared. */
  BKb, BLb, BKb, RDb, BKb, MGb, BKb,
  GRb, BKb, CYb, BKb, YLb, BKb, WHb, BLb,
  BLb, RDb, BLb, MGb, BLb, GRb, BLb, CYb,
  BLb, YLb, BLb, WHb, RDb, RDb, MGb, RDb,
  GRb, RDb, CYb, RDb, YLb, RDb, WHb, MGb,
  MGb, GRb, MGb, CYb, MGb, YLb, MGb, WHb,
  GRb, GRb, CYb, GRb, YLb, GRb, WHb, CYb,
  CYb, YLb, CYb, WHb, YLb, YLb, WHb, BKb,
  WHb, WHb,
};

static const unsigned char offsets[66 + 65] = {
  0,   1,   3,   5,   7,   9,  11,  13,
  2,  15,  16,  18,  20,  22,  24,  26,
  4,  17,  28,  29,  31,  33,  35,  37,
  6,  19,  30,  39,  40,  42,  44,  46,
  8,  21,  32,  41,  48,  49,  51,  53,
  10,  23,  34,  43,  50,  55,  56,  58,
  12,  25,  36,  45,  52,  57,  60,  61,
  62,  14,  27,  38,  47,  54,  59,  64,
  0,  66,  68,  70,  72,  74,  76,  78,
  67,  80,  81,  83,  85,  87,  89,  91,
  69,  82,  93,  94,  96,  98, 100, 102,
  71,  84,  95, 104, 105, 107, 109, 111,
  73,  86,  97, 106, 113, 114, 116, 118,
  75,  88,  99, 108, 115, 120, 121, 123,
  77,  90, 101, 110, 117, 122, 125, 126,
  127,  79,  92, 103, 112, 119, 124, 129,
};

#define WRITE8PIX(shift)                            \
do {                                                \
  pal = &palette[offsets[(attrs >> shift) & 0x7F]]; \
  *poutput++ = pal[(input >> (shift + 7)) & 1];     \
  *poutput++ = pal[(input >> (shift + 6)) & 1];     \
  *poutput++ = pal[(input >> (shift + 5)) & 1];     \
  *poutput++ = pal[(input >> (shift + 4)) & 1];     \
  *poutput++ = pal[(input >> (shift + 3)) & 1];     \
  *poutput++ = pal[(input >> (shift + 2)) & 1];     \
  *poutput++ = pal[(input >> (shift + 1)) & 1];     \
  *poutput++ = pal[(input >> (shift + 0)) & 1];     \
} while (0)

/* For reference:
 *
 * Spectrum screen memory has the arrangement:
 * 0b010BBLLLRRRCCCCC (B = band, L = line, R = row, C = column)
 *
 * Attribute bytes have the format:
 * 0bLRBBBFFF (L = flash, R = bright, B = paper (background), F = ink (foreground))
 */

void zxscreen_convert(const void    *vscr,
                      unsigned int  *poutput,
                      const zxbox_t *dirty)
{
  zxbox_t              box;
  int                  height;
  const unsigned int  *pattrs;
  int                  width;
  int                  x, linear_y;
  const unsigned int  *pinput;
  unsigned int         input;
  unsigned int         attrs;
  const unsigned int  *pal;

  assert(dirty);

#ifdef SHOW_DIRTY_RECTS
  static int dirtybits;
  dirtybits = 0x20202020 - dirtybits;
#endif

  /* Clamp the dirty rectangle to the screen dimensions. */
  box.x0 = CLAMP(dirty->x0, 0, 255);
  box.y0 = CLAMP(dirty->y0, 0, 191);
  box.x1 = CLAMP(dirty->x1, 1, 256);
  box.y1 = CLAMP(dirty->y1, 1, 192);

  /* The inner loop processes 32 pixels at a time, so we need to convert x
   * coordinates into chunks four attributes wide while rounding up and down as
   * required. */
  box.x0 = (box.x0) / 32;      /* divide to 0..7 rounding down */
  box.x1 = (box.x1 + 31) / 32; /* divide to 0..7 rounding up */

  /* Convert y coordinates into screen space - (0,0) is top left. */
  height = box.y1 - box.y0;
  box.y0 = 192 - box.y1;
  box.y1 = box.y0 + height;

  pattrs = (const unsigned int *) vscr
           + (SCREEN_BITMAP_LENGTH
              + box.y0 / 8 * 32  /* 8 scanlines/row, 32 attrs/row */
              + box.x0 * 4) / 4; /* 4 bytes/chunk, 4 bytes/word */

  poutput += box.y0 * 256 /* 256 pixels/row (256 words) for output */
             + box.x0 * 32; /* 32 pixels/chunk (32 words) */

  width = box.x1 - box.x0; /* hoisted out of loop */

  for (linear_y = box.y0; linear_y < box.y1; linear_y++) {
    /* Transpose fields using XOR */
    unsigned int tmp = (linear_y ^ (linear_y >> 3)) & 7;
    int          y   = linear_y ^ (tmp | (tmp << 3));

    pinput = (const unsigned int *) vscr
             + (y     * 32           /* 32 bytes/row */
                + box.x0 * 32 / 8) / 4; /* 32 bytes/row, 8 pixels/byte, 4 bytes/word */
    for (x = width; x > 0; x--) { /* x is unused in the loop body */
      input = *pinput++;
      attrs = *pattrs++;
#ifdef SHOW_DIRTY_RECTS
      attrs ^= dirtybits; /* force colour attrs to show redrawn areas */
#endif

      WRITE8PIX(0);
      WRITE8PIX(8);
      WRITE8PIX(16);
      WRITE8PIX(24);
    }

    /* Skip to the start of the next row. */
    pattrs  += 8   - width;
    poutput += 256 - width * 32;

    /* Rewind pattrs except at the end of an attribute row. */
    if ((linear_y & 7) != 7)
      pattrs -= 8;
  }
}

/* ----------------------------------------------------------------------- */

#define GAMEWIDTH 256
#define GAMEHEIGHT 192

static const SDL_Rect dstrect = { 0, 0, GAMEWIDTH, GAMEHEIGHT };
unsigned int pixels[GAMEWIDTH * GAMEHEIGHT];

int main(void)
{
#if 0
  // testing the multiplier routine

  static const T a[5] = { 0xA0, 0xE7, 0x20, 0xC0, 0xE6 };
  static const T c[5] = { 0x05, 0x02, 0x05, 0x03, 0xFE };
  static const T r[5] = { 0x03, 0x02, 0x01, 0x02, 0xFE };

  for (int i = 0; i < 5; i++) {
    T n = multiply(a[i], c[i]);
    if (n == r[i])
      printf("ok: %d x %d = %d\n", a[i], c[i], r[i]);
    else
      printf("error: %d x %d = %d, got %d\n", a[i], c[i], r[i], n);
  }

  return 0;
#else
  SDL_Event     event;
  SDL_Window   *window;
  SDL_Renderer *renderer;
  SDL_Texture  *texture;
  chqstate_t    state;
  int           quit = 0;
  int           mx = 0, my = 0;
  int           t = 0;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  window = SDL_CreateWindow("Chase H.Q.",
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            GAMEWIDTH, GAMEHEIGHT,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  renderer = SDL_CreateRenderer(window,
                                -1,
                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  texture = SDL_CreateTexture(renderer,
                              SDL_PIXELFORMAT_ARGB8888, // fastest?
                              SDL_TEXTUREACCESS_STREAMING,
                              GAMEWIDTH, GAMEHEIGHT);
  if (texture == NULL) {
    printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE) < 0) {
    printf("SDL_SetTextureBlendMode failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  chasehq_reset_state(&state);

  if (1) { // temp - fill screen with junk
    for (int i = 0; i < SCREEN_BITMAP_LENGTH; i++)
      state.screen[i] = rng(&state);
    for (int i = 0; i < SCREEN_ATTRIBUTES_LENGTH; i++)
      state.screen[SCREEN_ATTRIBUTES_START_ADDRESS - SCREEN_START_ADDRESS + i] =
        attribute_YELLOW_OVER_BLACK;

    clear_playfield_set_attrs(&state);
  }

  while (!quit) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = 1;
        break;

      case SDL_KEYDOWN:
      case SDL_KEYUP:
        if (1) { // temp - fill screen with junk
          static const char msg[] = "GREETS TO THE RETRO FUNSTERS FROM CHASE D.P.T.!";
          for (int i = 0; i < sizeof(msg) - 1; i++) {
            if (t)
              plot_mini_font_cursor_off(&state, i, msg[i]);
            else
              plot_mini_font_cursor_on(&state, i, msg[i]);
          }
        }
        break;

      case SDL_MOUSEMOTION:
        mx = event.motion.x;
        my = event.motion.y;
        ledfont_plot(&state, 1 + my % 10,
                     &state.screen[(0x4000 + mx / 8) - SCREEN_START_ADDRESS]);
        break;

      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEBUTTONDOWN:
        t  = event.button.state == SDL_PRESSED;
        break;

        // main_loop(&state);

        //plot_face(&state, &bitmap_faces[FACEBYTES*0], 0x4036);
        //plot_face(&state, &bitmap_faces[FACEBYTES*1], 0x4836);
        //plot_face(&state, &bitmap_faces[FACEBYTES*2], 0x5036);

#if 0
        // Handle mouse motion event
        for (int i = 0; i < 22; i++) // we read 20/21 entries
          state.road_buffer[i] = 0x70 + (event.motion.x * 0x10 / 256);
        build_curve_table(&state, /*forked=*/0);

        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));

        for (int y = 0; y < 104; y++) {
          int yp = y; // 0 is top

          int r = state.table_ec00[0x30 + y] >> 0;
          int l = state.table_e800[0x30 + y] >> 0;
          SDL_Rect rrect = {255 - r, yp, r, 1};
          SDL_Rect lrect = {      0, yp, l, 1};
          SDL_FillRect(surface, &rrect, SDL_MapRGB(surface->format, 0x00, 0x00, 0xFF));
          SDL_FillRect(surface, &lrect, SDL_MapRGB(surface->format, 0xFF, 0x00, 0x00));

          SDL_Rect rect = {0, yp, 1, 1};
          SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0x00));
        }
        SDL_UpdateWindowSurface(window);
        state.fast_counter++;
#endif
        break;
      }
    }

    if (quit)
      break;

    {
      zxbox_t dirty = {0, 0, GAMEWIDTH, GAMEHEIGHT};

      zxscreen_convert(&state.screen[0], pixels, &dirty);
      SDL_UpdateTexture(texture, NULL, pixels, GAMEWIDTH * 4);
    }

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_RenderPresent(renderer);

    SDL_Delay(1000 / 60); //fps
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
#endif
}
