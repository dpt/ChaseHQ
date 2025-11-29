// proto.c
//
// Chase H.Q. code model
//
// by dpt

// vim: ts=8 sts=2 sw=2 et

// Compile with:
// gcc `sdl2-config --cflags --libs` -Wall -o proto proto.c

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

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

/* ----------------------------------------------------------------------- */

typedef int8_t T; // works

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

#define CHATTERSTATE_IDLE   (0)
#define CHATTERSTATE_START  (1)
#define CHATTERSTATE_RUN    (2)
#define CHATTERSTATE_STOP   (3)

typedef uint8_t chatterpriority_t;

/* ----------------------------------------------------------------------- */

typedef struct hazard_s
{
  uint8_t  used;
  uint8_t  distance;
  uint8_t  horz_pos;
  uint8_t  TBD3;
  uint8_t  TBD4;
  uint8_t  horz_pos_on_road;
  uint8_t  TBD6;
  uint8_t  TBD7;
  uint8_t  TBD8;
  uint16_t lod_addr;
  uint16_t hit_handler;
  uint16_t speed;
  uint8_t  TBD15;
  uint8_t  TBD16;
  uint8_t  TBD17;
  uint8_t  TBD18;
  uint8_t  TBD19;
}
hazard_t;

typedef struct chqstate_s
{
  // $4000
  uint8_t  screen[256*192/8+32*24];

  // $8002
  char     score_bcd[4];

  // $8007
  uint8_t  wanted_stage_number;

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

  uint8_t  SM_9982;

  // $9D51
  char    bonus_string[6];

  // $9D9B
  char    *SM_address_of_score_digits; // self modified

  // $A16D
  uint8_t  var_a16d;

  // $A188
  hazard_t hazards[6];

  // $A226
  uint8_t  correct_fork;

  // $A22C
  uint8_t  trigger_bonus_flag;

  // $A249
  uint8_t  fork_taken;

  // $A254
  uint8_t  allow_spawning;

  // $A265
  uint8_t  fork_visible;
  // $A266
  uint8_t  fork_countdown;
  // $A267
  uint16_t fork_distance;
  // $A269
  uint8_t  fork_in_progress;

  // $A26C
  uint16_t road_pos;

  // $A23F
  uint8_t  fast_counter;
  // $A240
  uint8_t *road_buffer_offset;
  uint8_t *road_buffer_start; // additional
  uint8_t *road_buffer_end; // additional

  // $E300
  uint8_t  table_e300[32]; // note: first byte should be $60
                           // $E320
  uint8_t  table_e320[2];
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
} chqstate_t;

/* ----------------------------------------------------------------------- */

static void chasehq_reset_state(chqstate_t *state)
{
  memset(state, 0, sizeof(*state));

  state->road_buffer_offset = &state->road_buffer[0];
  state->road_buffer_start  = &state->road_buffer[0];
  state->road_buffer_end    = &state->road_buffer[256];
}

/* ----------------------------------------------------------------------- */

static void start_chatter(chqstate_t       *state,
                          chatterpriority_t priority,
                          const uint8_t    *chatterblk);

static void drive_chatter(chqstate_t *state);

static void print_chatter(chqstate_t *state);
static void pc_chatter_message(chqstate_t *state);
static void pc_clear_line(chqstate_t *state);

static void noise_effect(chqstate_t *state, uint8_t counter);
static void noise_effect_9a5c(chqstate_t *state, uint8_t counter);
static void ne_plot_attrs(chqstate_t *state, uint8_t A);

static void plot_face(chqstate_t *state, uint8_t *HLface, uint8_t *DEscreen);

static void plot_mini_font_1(chqstate_t *state, uint8_t Aflag, char Acharacter);
static void plot_mini_font_2(chqstate_t *state, uint8_t Aflag, char Acharacter);
static void pmf_go(chqstate_t *state, uint8_t Aflag, char Acharacter, uint16_t BC);

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

static uint8_t DAA(uint8_t v, int *carry_out);

static void build_curve_table(chqstate_t *state, int forked);
static void build_curve_table_sub_cca8(chqstate_t *state,
                                       uint8_t     Bdash_alwayszero,
                                       uint16_t   *HLtableend,
                                       uint16_t    DEroadpos);

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
#define CHATTERSTR_THIS_IS_NANCY               (0)
#define CHATTERSTR_THIS_IS_AIRBORNE            (1)
#define CHATTERSTR_TARGET_VEHICLE_TURNED       (2)
#define CHATTERSTR_RIGHT_AHEAD_OVER            (3)
#define CHATTERSTR_LEFT_AHEAD_OVER             (4)
#define CHATTERSTR_READ_LOUD_CLEAR             (5)
#define CHATTERSTR_ROGER                       (6)
#define CHATTERSTR_GOTCHA_NANCY                (7)
#define CHATTERSTR_WHAT_YOU_DOING              (8)
#define CHATTERSTR_GOING_OTHER_WAY             (9)
#define CHATTERSTR_MESSIN_AROUND              (10)
#define CHATTERSTR_TIME_RUN_OUT               (11)
#define CHATTERSTR_GET_MOVIN_MAN              (12)
#define CHATTERSTR_OH_NO                      (13)
#define CHATTERSTR_PLEASE                     (14)
#define CHATTERSTR_GREAT                      (15)
#define CHATTERSTR_OUCH                       (16)
#define CHATTERSTR_LETS_GO                    (17)
#define CHATTERSTR_YAOW                       (18)
#define CHATTERSTR_BEAR_DOWN                  (19)
#define CHATTERSTR_MORE_PUSH_MORE             (20)
#define CHATTERSTR_ONE_MORE_TIME              (21)
#define CHATTERSTR_OH_MAN                     (22)
#define CHATTERSTR_WHOA                       (23)
#define CHATTERSTR_HARDER                     (24)
#define CHATTERSTR_PICKED_WRONG_JOB           (25)
#define CHATTERSTR_CHECK_CLASSIFIED_ADS       (26)
#define CHATTERSTR_ONE_MORE_TRY               (27)
#define CHATTERSTR_MEDIOCRE_DRIVER            (28)
#define CHATTERSTR_SEE_YOU_LATER              (29)
#define CHATTERSTR__LIMIT                     (30)

/// Chatter block indices
#define CHATTERBLK_PILOT_TURN_LEFT             (0)
#define CHATTERBLK_PILOT_TURN_RIGHT            (1)
#define CHATTERBLK_HEROES_ACKNOWLEDGE          (2)
#define CHATTERBLK_TONY_LOUD_CLEAR             (3)
#define CHATTERBLK_RAYMOND_ROGER               (4)
#define CHATTERBLK_TONY_GOTCHA                 (5)
#define CHATTERBLK_RAYMOND_WRONG_WAY           (6)
#define CHATTERBLK_RAYMOND_SMASH               (7)
#define CHATTERBLK_RAYMOND_BEAR_DOWN           (8)
#define CHATTERBLK_RAYMOND_PUSH_IT             (9)
#define CHATTERBLK_RAYMOND_HARDER             (10)
#define CHATTERBLK_RAYMOND_OH_MAN             (11)
#define CHATTERBLK_RAYMOND_RANDOM_PLEAS       (12)
#define CHATTERBLK_RAYMOND_PLEASE             (13)
#define CHATTERBLK_RAYMOND_GET_MOVING         (14)
#define CHATTERBLK_NANCY_TIME_RUNNING_OUT     (15)
#define CHATTERBLK_RAYMOND_RANDOM_YELPS       (16)
#define CHATTERBLK_RAYMOND_OHNO               (17)
#define CHATTERBLK_RAYMOND_OUCH               (18)
#define CHATTERBLK_RAYMOND_YAOW               (19)
#define CHATTERBLK_TURBO                      (20)
#define CHATTERBLK_TONY_WHOA                  (21)
#define CHATTERBLK_TONY_GREAT                 (22)
#define CHATTERBLK_RAYMOND_ONE_MORE_TIME      (23)
#define CHATTERBLK_NANCY_BERATES              (24)
#define CHATTERBLK_NANCY_WRONG_JOB            (25)
#define CHATTERBLK_NANCY_ONE_MORE_TRY         (26)
#define CHATTERBLK_NANCY_MEDIOCRE_DRIVER      (27)
#define CHATTERBLK_TONY_LETS_GO               (28)
#define CHATTERBLK__LIMIT                     (29)

// $97B5
static const char *chatter_strings[CHATTERSTR__LIMIT] = {
  "THIS IS NANCY AT CHASE H.Q.",
  "THIS IS SPECIAL INVESTIGATION AIRBORNE.",
  "THE TARGET VEHICLE HAS TURNED",
  "RIGHT UP AHEAD... OVER.",
  "LEFT UP AHEAD... OVER.",
  "WE READ LOUD AND CLEAR! OVER.",
  "ROGER!",
  "GOTCHA NANCY BABY!",
  "WHAT ARE YOU DOING MAN!!",
  "THE BAD GUYS ARE GOING THE OTHER WAY.",
  "IF YOU KEEP MESSIN' AROUND LIKE THAT",
  "YOUR TIME IS GOING TO RUN OUT... OVER.",
  "LET'S GET MOVIN' MAN!",
  "OH, NO!",
  "PLEASE!",
  "GREAT!",
  "OUCH!",
  "LET'S GO. MR. DRIVER.",
  "YAOOOOOW!",
  "BEAR DOWN.",
  "MORE, PUSH IT MORE!",
  "ONE MORE TIME.",
  "OH MAN.",
  "WHOAAAAA!",
  "HARDER!",
  "WE THINK YOU PICKED THE WRONG JOB.",
  "BETTER CHECK THE CLASSIFIED ADS.",
  "ONE MORE TRY FOR BEING A GOOD BOY!",
  "YOU'RE A MEDIOCRE DRIVER, BROTHER!",
  "SEE YOU LATER."
};

// $98A9
static const uint8_t chatterblk_pilot_turn_left[] = {
  CHATTERCHR_PILOT,
  CHATTERSTR_THIS_IS_AIRBORNE,
  CHATTERSTR_TARGET_VEHICLE_TURNED,
  CHATTERSTR_LEFT_AHEAD_OVER,
  CHATTERCMD_PAUSE,
  CHATTERBLK_TONY_LOUD_CLEAR
};

// $98B3
static const uint8_t chatterblk_pilot_turn_right[] = {
  CHATTERCHR_PILOT,
  CHATTERSTR_THIS_IS_AIRBORNE,
  CHATTERSTR_TARGET_VEHICLE_TURNED,
  CHATTERSTR_RIGHT_AHEAD_OVER,
  CHATTERCMD_PAUSE,
  CHATTERBLK_TONY_LOUD_CLEAR
};

// $98BD
static const uint8_t chatterblk_heroes_acknowledge[] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_TONY_LOUD_CLEAR,
  CHATTERBLK_RAYMOND_ROGER,
  CHATTERBLK_TONY_GOTCHA,
};

// $98C4
static const uint8_t chatterblk_tony_loud_clear[] = {
  CHATTERCHR_TONY,
  CHATTERSTR_READ_LOUD_CLEAR,
  CHATTERCMD_STOP
};

// $98C8
static const uint8_t chatterblk_raymond_roger[] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_ROGER,
  CHATTERCMD_STOP
};

// $98CC
static const uint8_t chatterblk_tony_gotcha[] = {
  CHATTERCHR_TONY,
  CHATTERSTR_GOTCHA_NANCY,
  CHATTERCMD_STOP
};

// $98D0
static const uint8_t chatterblk_raymond_wrong_way[] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_WHAT_YOU_DOING,
  CHATTERSTR_GOING_OTHER_WAY,
  CHATTERCMD_STOP
};

// $98D6
static const uint8_t chatterblk_raymond_smash[] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_RAYMOND_BEAR_DOWN,
  CHATTERBLK_RAYMOND_RANDOM_PLEAS,
  CHATTERBLK_RAYMOND_PUSH_IT
};

// $98DD
static const uint8_t chatterblk_raymond_bear_down[] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_BEAR_DOWN,
  CHATTERCMD_STOP
};

// $98E1
static const uint8_t chatterblk_raymond_push_it[] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_MORE_PUSH_MORE,
  CHATTERCMD_STOP
};

// $98E5
static const uint8_t chatterblk_raymond_harder[] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_HARDER,
  CHATTERCMD_STOP
};

// $98E9
static const uint8_t chatterblk_raymond_oh_man[] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_OH_MAN,
  CHATTERCMD_STOP
};

// $98ED
static const uint8_t chatterblk_raymond_random_pleas[] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_RAYMOND_OH_MAN,
  CHATTERBLK_RAYMOND_HARDER,
  CHATTERBLK_RAYMOND_PLEASE
};

// $98F4
static const uint8_t chatterblk_raymond_please[] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_PLEASE,
  CHATTERCMD_STOP
};

// $98F8
static const uint8_t chatterblk_raymond_get_moving[] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_GET_MOVIN_MAN,
  CHATTERCMD_STOP
};

// $98FC
static const uint8_t chatterblk_nancy_time_running_out[] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_THIS_IS_NANCY,
  CHATTERSTR_MESSIN_AROUND,
  CHATTERSTR_TIME_RUN_OUT,
  CHATTERCMD_STOP
};

// $9904
static const uint8_t chatterblk_raymond_random_yelps[] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_RAYMOND_OHNO,
  CHATTERBLK_RAYMOND_OUCH,
  CHATTERBLK_RAYMOND_YAOW
};

// $990B
static const uint8_t chatterblk_raymond_ohno[] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_OH_NO,
  CHATTERCMD_STOP
};

// $990F
static const uint8_t chatterblk_raymond_ouch[] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_OUCH,
  CHATTERCMD_STOP
};

// $9913
static const uint8_t chatterblk_raymond_yaow[] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_YAOW,
  CHATTERCMD_STOP
};

// $9917
static const uint8_t chatterblk_turbo[] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_TONY_WHOA,
  CHATTERBLK_TONY_GREAT,
  CHATTERBLK_RAYMOND_ONE_MORE_TIME
};

// $991E
static const uint8_t chatterblk_tony_whoa[] = {
  CHATTERCHR_TONY,
  CHATTERSTR_WHOA,
  CHATTERCMD_STOP
};

// $9922
static const uint8_t chatterblk_tony_great[] = {
  CHATTERCHR_TONY,
  CHATTERSTR_GREAT,
  CHATTERCMD_STOP
};

// $9926
static const uint8_t chatterblk_raymond_one_more_time[] = {
  CHATTERCHR_RAYMOND,
  CHATTERSTR_ONE_MORE_TIME,
  CHATTERCMD_STOP
};

// $992A
static const uint8_t chatterblk_nancy_berates_hero[] = {
  CHATTERCMD_RANDOM,
  CHATTERBLK_NANCY_WRONG_JOB,
  CHATTERBLK_NANCY_ONE_MORE_TRY,
  CHATTERBLK_NANCY_MEDIOCRE_DRIVER
};

// $9931
static const uint8_t chatterblk_nancy_wrong_job[] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_PICKED_WRONG_JOB,
  CHATTERCMD_STOP
};

// $9937
static const uint8_t chatterblk_nancy_one_more_try[] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_ONE_MORE_TRY,
  CHATTERCMD_STOP
};

// $993B
static const uint8_t chatterblk_nancy_mediocre_driver[] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_MEDIOCRE_DRIVER,
  CHATTERSTR_SEE_YOU_LATER,
  CHATTERCMD_STOP
};

// $9941
static const uint8_t chatterblk_tony_lets_go[] = {
  CHATTERCHR_TONY,
  CHATTERSTR_LETS_GO,
  CHATTERCMD_STOP
};

// Additional: List of all chatter blocks
static const uint8_t *chatter_blocks[CHATTERBLK__LIMIT] = {
  &chatterblk_pilot_turn_left[0],
  &chatterblk_pilot_turn_right[0],
  &chatterblk_heroes_acknowledge[0],
  &chatterblk_tony_loud_clear[0],
  &chatterblk_raymond_roger[0],
  &chatterblk_tony_gotcha[0],
  &chatterblk_raymond_wrong_way[0],
  &chatterblk_raymond_smash[0],
  &chatterblk_raymond_bear_down[0],
  &chatterblk_raymond_push_it[0],
  &chatterblk_raymond_harder[0],
  &chatterblk_raymond_oh_man[0],
  &chatterblk_raymond_random_pleas[0],
  &chatterblk_raymond_please[0],
  &chatterblk_raymond_get_moving[0],
  &chatterblk_nancy_time_running_out[0],
  &chatterblk_raymond_random_yelps[0],
  &chatterblk_raymond_ohno[0],
  &chatterblk_raymond_ouch[0],
  &chatterblk_raymond_yaow[0],
  &chatterblk_turbo[0],
  &chatterblk_tony_whoa[0],
  &chatterblk_tony_great[0],
  &chatterblk_raymond_one_more_time[0],
  &chatterblk_nancy_berates_hero[0],
  &chatterblk_nancy_wrong_job[0],
  &chatterblk_nancy_one_more_try[0],
  &chatterblk_nancy_mediocre_driver[0],
  &chatterblk_tony_lets_go[0]
};

/* ----------------------------------------------------------------------- */

// $E540
static const uint16_t inward_bend_table[96] = {
  0x0000,
  0x0006,
  0x000C,
  0x0012,
  0x0017,
  0x001C,
  0x0021,
  0x0026,
  0x002A,
  0x002F,
  0x0033,
  0x0037,
  0x003C,
  0x0040,
  0x0043,
  0x0047,
  0x004B,
  0x004F,
  0x0052,
  0x0056,
  0x0059,
  0x005D,
  0x0060,
  0x0063,
  0x0067,
  0x006A,
  0x006D,
  0x0070,
  0x0073,
  0x0077,
  0x007A,
  0x007D,
  0x0080,
  0x0083,
  0x0086,
  0x0089,
  0x008D,
  0x0090,
  0x0093,
  0x0096,
  0x0099,
  0x009D,
  0x00A0,
  0x00A3,
  0x00A7,
  0x00AA,
  0x00AE,
  0x00B1,
  0x00B5,
  0x00B9,
  0x00BD,
  0x00C0,
  0x00C4,
  0x00C9,
  0x00CD,
  0x00D1,
  0x00D6,
  0x00DA,
  0x00DF,
  0x00E4,
  0x00E9,
  0x00EE,
  0x00F4,
  0x00FA,
  0x0100,
  0x0106,
  0x010D,
  0x0114,
  0x011C,
  0x0124,
  0x012D,
  0x0136,
  0x0140,
  0x014A,
  0x0156,
  0x0162,
  0x016F,
  0x017E,
  0x018F,
  0x01A1,
  0x01B5,
  0x01CC,
  0x01E6,
  0x0203,
  0x0226,
  0x024F,
  0x027F,
  0x02BA,
  0x0303,
  0x0362,
  0x03DF,
  0x048E,
  0x0594,
  0x0747,
  0x0AAD,
  0x14DE
};

// $E6B0 - a right hand table?
static const uint8_t horizontal_e6b0[8][22] = {
  { 0xEB,0xC1,0xAA,0x8F,0x8E,0x6E,0x5D,0x66,0x71,0x55,0x2E,0x61,0x35,0x38,0x3C,0x40,0x44,0x49,0x4E,0x55,0x5D,0x66 },
  { 0xD5,0xCA,0xA7,0x8C,0x8B,0x6B,0x78,0x66,0x4B,0x51,0x59,0x61,0x35,0x38,0x3C,0x40,0x44,0x49,0x4E,0x55,0x5D,0x66 },
  { 0xB5,0xCF,0xB3,0x89,0x88,0x83,0x78,0x66,0x4B,0x51,0x59,0x30,0x66,0x38,0x3C,0x40,0x44,0x49,0x4E,0x55,0x5D,0x66 },
  { 0xA0,0xCA,0xBD,0x9A,0x88,0x69,0x75,0x63,0x6D,0x51,0x59,0x30,0x66,0x38,0x3C,0x40,0x44,0x49,0x4E,0x55,0x5D,0x66 },
  { 0x80,0xDB,0xAA,0xA7,0x85,0x80,0x75,0x63,0x49,0x76,0x2C,0x5D,0x33,0x6B,0x3C,0x40,0x44,0x49,0x4E,0x55,0x5D,0x66 },
  { 0x60,0xDF,0xB5,0xA4,0x82,0x7C,0x71,0x60,0x69,0x4E,0x55,0x5D,0x33,0x6B,0x3C,0x40,0x44,0x49,0x4E,0x55,0x5D,0x66 },
  { 0x4B,0xDB,0xBE,0xA1,0x95,0x7C,0x71,0x60,0x69,0x4E,0x55,0x5D,0x33,0x35,0x71,0x40,0x44,0x49,0x4E,0x55,0x5D,0x66 },
  { 0x20,0xE7,0xB9,0xAD,0x92,0x79,0x6E,0x7C,0x69,0x4E,0x55,0x5D,0x33,0x35,0x38,0x3C,0x40,0x44,0x49,0x4E,0x55,0x5D },
};

// $E760 - a left hand table?
static const uint8_t horizontal_e760[8][22] = {
  { 0x42,0x2A,0x1E,0x15,0x12,0x0C,0x09,0x09,0x09,0x06,0x03,0x06,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00 },
  { 0x3D,0x2D,0x1F,0x15,0x12,0x0C,0x0C,0x09,0x06,0x06,0x06,0x06,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00 },
  { 0x34,0x30,0x21,0x15,0x12,0x0F,0x0C,0x09,0x06,0x06,0x06,0x03,0x06,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00 },
  { 0x2E,0x30,0x24,0x18,0x12,0x0C,0x0C,0x09,0x09,0x06,0x06,0x03,0x06,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00 },
  { 0x25,0x36,0x21,0x1C,0x12,0x0F,0x0C,0x09,0x06,0x09,0x03,0x06,0x03,0x06,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00 },
  { 0x1C,0x3A,0x24,0x1B,0x12,0x0F,0x0C,0x09,0x09,0x06,0x06,0x06,0x03,0x06,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00 },
  { 0x15,0x39,0x27,0x1B,0x15,0x0F,0x0C,0x09,0x09,0x06,0x06,0x06,0x03,0x03,0x06,0x03,0x03,0x03,0x03,0x03,0x03,0x00 },
  { 0x0A,0x3F,0x27,0x1E,0x15,0x0F,0x0C,0x0C,0x09,0x06,0x06,0x06,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00 },
};

/* ----------------------------------------------------------------------- */

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
  uint8_t chatter_state; // was A
  int     carry = 0;
  char    D;
  uint8_t A;
  uint8_t B;
  const char *HL;
  const uint8_t *HLchatterblk;

  chatter_state = state->chatter_state;
  chatter_state--;
  if (chatter_state == 0) // starting (1)
    goto starting;
  chatter_state--;
  if (chatter_state == 0) // displaying (2)
    goto do_noise_effect;
  chatter_state--;
  if (chatter_state != 0) // not stopping (3), must be idle (0)
    goto idle;

  if (--state->noise_counter != 0) {
    noise_effect(state, state->noise_counter); // exit via
    return;
  }

  state->chatter_state = CHATTERSTATE_IDLE;
  ne_plot_attrs(state, 0 /* black on black attrs */);

idle:
  D = ' ';
  A = state->SM_9982;
  RRC(A);
  state->SM_9982 = A;
  A = 0xFF;
  if (carry)
    plot_mini_font_2(state, A, D); // exit via
  else
    plot_mini_font_1(state, A, D); // exit via
  return;

do_noise_effect:
  if (state->noise_counter) {
    noise_effect(state, state->noise_counter); // exit via
    return;
  }
  A = state->chatter_delay;
  if (A == 0)
    goto clear_line;
  A--;
  state->chatter_delay = A;
  B = state->chatter_delay;
  if (A == 0)
    goto read_message;
  HL = state->next_character - 1; // addr of next char
  D = *HL & ~(1 << 7); // load char and clear terminator
  A = state->message_x - 1;
  RR(B);
  if (carry)
    plot_mini_font_2(state, A, D); // exit via
  else
    plot_mini_font_1(state, A, D); // exit via
  return;

clear_line:
  A = state->message_x;
  if (A) {
    pc_clear_line(state); // exit via
    return;
  }

read_message:
  HLchatterblk = state->chatterblk_ptr;
  A = *HLchatterblk;
  if (A == CHATTERCMD_STOP)
    goto stop;
  if (A != CHATTERCMD_PAUSE) {
    pc_chatter_message(state); // exit via
    return;
  }
  // Conv: The next byte is no longer an address but an index into table of
  // chatter blocks.
  HLchatterblk++;
  state->chatterblk_ptr = chatter_blocks[*HLchatterblk];
  goto clear;

stop:
  state->noise_counter = 4;
  state->chatter_state = CHATTERSTATE_STOP;
  clear_message_line(state); // exit via
  return;

starting:
  state->chatter_state = CHATTERSTATE_RUN;

clear:
  clear_message_line(state);
  noise_effect(state, 4); // exit via
}

// $99EC
static void print_chatter(chqstate_t *state)
{
}

// $9A24
static void pc_chatter_message(chqstate_t *state)
{
}

// $9A30
static void pc_clear_line(chqstate_t *state)
{
}

// $9A55
static void noise_effect(chqstate_t *state, uint8_t counter)
{
  counter--;
  state->noise_counter = counter;
  if (counter == 0)
    print_chatter(state); // exit via
  else
    noise_effect_9a5c(state, counter); // FALLTHROUGH
}

static void noise_effect_9a5c(chqstate_t *state, uint8_t counter)
{
  uint8_t  flag; // was A
  char     character; // was D
  uint16_t DE;
  uint8_t  C;
  int      carry = 0;
  uint8_t  B;
  uint8_t *HLnoisebytes;
  uint16_t DEscreen;
  uint16_t DEscreen_saved;
  uint8_t  A; // was A

  RR(counter);
  flag = 0xFF;
  character = ' ';
  if (!carry)
    plot_mini_font_2(state, flag, character);
  else
    plot_mini_font_1(state, flag, character);
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
      // *DEscreen = A; // Conv: will need to go via state
      DEscreen++; // was E++
    } while (--B > 0);
    DEscreen = DEscreen_saved; // was POP - restore row ptr

    // Move to next row
    DEscreen += 256;
    if (((DEscreen >> 8) & 7) == 0) {
      int t = (DE & 0xFF) + 32;
      DE = (DE & 0xFF00) | (t & 0xFF);
      if (t < 0x100) { // didn't carry
        t = (DE >> 8) - 8;
        DE = (t << 8) | (DE & 0xFF);
      }
    }
  } while (--C > 0);

  ne_plot_attrs(state, 0x47); // BRIGHT + white over black
  // FALLTHROUGH
}

// $9A98
static void ne_plot_attrs(chqstate_t *state, uint8_t attr)
{
  uint16_t HL;
  uint8_t  B;
  uint16_t DE;
  
  HL = 0x5836; // Screen attribute position (22,1)
  B = 5; // 5 rows
  DE = 32 - 3; // row skip
  do {
    // Conv: Screen write now goes via state.
    state->screen[HL++ - 0x5800] = attr;
    state->screen[HL++ - 0x5800] = attr;
    state->screen[HL++ - 0x5800] = attr;
    state->screen[HL   - 0x5800] = attr;
    HL += DE;
  } while (--B > 0);
}

// $9AAB
static void plot_face(chqstate_t *state, uint8_t *HLface, uint8_t *DEscreen)
{
}

// $9AEC
static void plot_mini_font_1(chqstate_t *state, uint8_t flag, char character)
{
  pmf_go(state, flag, character, 0x0000);
}

// $9AF1
static void plot_mini_font_2(chqstate_t *state, uint8_t flag, char character)
{
  pmf_go(state, flag, character, 0x0780);
}

// $9AF4
static void pmf_go(chqstate_t *state, uint8_t flag, char character, uint16_t BC)
{
}

// $9BA7
static void clear_message_line(chqstate_t *state)
{
}

// $9BCF
static void tick(chqstate_t *state)
{
}

// $9CC2
static void speed_score(chqstate_t *state)
{
}

// $9CD6
// Bug: As soon as a nonzero->zero transition is seen the routine finishes so you can only have a single run of zeroes in the bonus.
static void add_bonus(chqstate_t *state,
                      uint8_t     A_lo,
                      uint8_t     E_md,
                      uint8_t     D_hi)
{
  char   *output;      // was HL
  uint8_t nonzeroflag; // was C - used to track if a zero has been emitted.

  output = &state->bonus_string[6]; // points to byte after buffer
  nonzeroflag = 0xFF; // flag (zero not seen)
  (void) bonus_digit(A_lo >> 0, &nonzeroflag, &output); // always runs since flag > 0
  *output |= 1<<7; // terminate string

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

static void increment_score(chqstate_t *state,
                            uint8_t     A_lo,
                            uint8_t     E_md,
                            uint8_t     D_hi)
{
  char    *HLscorebcd;
  uint8_t  A;
  int      carry;

  HLscorebcd = &state->score_bcd[0];
  A = A_lo + *HLscorebcd;
  *HLscorebcd++ = DAA(A, &carry);
  A = E_md + *HLscorebcd + carry;
  *HLscorebcd++ = DAA(A, &carry);
  A = D_hi + *HLscorebcd + carry;
  *HLscorebcd++ = DAA(A, &carry);
  A = *HLscorebcd + carry;
  *HLscorebcd = DAA(A, &carry);
}

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
  int            carry = 0;
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
  if ((*DElanedata & 4) != 0) // check for forked road (have already checked flags for 0xE1)
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
  Aiterations += state->var_a16d;
  Ca16d = Aiterations; // new value for $A16D
  Aiterations -= 2;
  if (Aiterations >= 256-2) // carried?
    goto lr_set_var_a16d_from_c;
  Ca16d = Aiterations; // new value for $A16D
  HLforkdistance += 16;
  state->fork_distance = HLforkdistance;
lr_set_var_a16d_from_c:
  state->var_a16d = Ca16d;
lr_no_car_spawning:
  carry = state->var_a16d & 1; // CHECK
  Aiterations = state->fast_counter;
  RL(Aiterations);
  RL(Aiterations);
  RL(Aiterations);
  RL(Aiterations);
  Aiterations -= 0x10; // sets top nibble to $F
  HLforkdistance += 0xFF | Aiterations; // a signed -15..16 value now IS THIS INCREMENT WRONG?
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
    printf("curvature_A=%d\n",curvature_A);

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
    carry = ((A & (1<<7)) != 0);
    A = (A << 1) & 0xFF;
    if (carry) HLdash = BCdash << 1;
    carry = ((A & (1<<7)) != 0);
    A = (A << 1) & 0xFF;
    if (carry) HLdash += BCdash;
    HLdash <<= 1;
    carry = ((A & (1<<7)) != 0);
    A = (A << 1) & 0xFF;
    if (carry) HLdash += BCdash;
    HLdash <<= 1;
    carry = ((A & (1<<7)) != 0);
    A = (A << 1) & 0xFF;
    if (carry) HLdash += BCdash;
    HLdash <<= 1;
    carry = ((A & (1<<7)) != 0);
    A = (A << 1) & 0xFF;
    if (carry) HLdash += BCdash;

    HLdash = (HLdash >> 8) + ((HLdash & (1<<7)) != 0); // rounding
    A = HLdash & 0xFF;
    if (HLdash & (1<<7)) HLdash |= 0xFF00;

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
    state->object_positions[IYe300 - 1 - &state->table_e300[0]] = A; // must write to $E34F+ which is object_positions
    A -= Bdash;
    Bdash = A;
    Cdash = A;
    Ldash = state->table_e320[IYe300 - 1 - &state->table_e300[0]]; // IY[$1F]; // $E320+
    if ((Ldash & (1<<7)) != 0) {
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
        if (Aopcode == 0x13) DEroadpos++; else DEroadpos--;
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
      if (Aopcode == 0x13) DEroadpos++; else DEroadpos--;
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

/* ----------------------------------------------------------------------- */

int main(void)
{
#if 0
  // testing the multiplier routine

  static const T a[5] = { 0xA0, 0xE7, 0x20, 0xC0, 0xE6 };
  static const T c[5] = { 0x05, 0x02, 0x05, 0x03, 0xFE };
  static const T r[5] = { 0x03, 0x02, 0x01, 0x02, 0xFE };

  for (int i = 0; i < 5; i++)
  {
    T n = multiply(a[i], c[i]);
    if (n == r[i])
      printf("ok: %d x %d = %d\n", a[i], c[i], r[i]);
    else
      printf("error: %d x %d = %d, got %d\n", a[i], c[i], r[i], n);
  }

  return 0;
#else
  SDL_Event event;
  SDL_Surface *surface;
  SDL_Window *window;
  chqstate_t state;
  int quit = 0;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
  {
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  window = SDL_CreateWindow("Chase H.Q.", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 192, SDL_WINDOW_SHOWN);
  if (!window)
  {
    printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  surface = SDL_GetWindowSurface(window);
  if (!surface)
  {
    printf("SDL_GetWindowSurface failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
  SDL_UpdateWindowSurface(window);

  chasehq_reset_state(&state);

  while (!quit)
  {
    SDL_PollEvent(&event);
    switch (event.type)
    {
      case SDL_QUIT:
        quit = 1;
        break;

      case SDL_MOUSEMOTION:
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
        break;

      case SDL_MOUSEBUTTONDOWN:
        break;

      case SDL_MOUSEBUTTONUP:
        break;
    }
  }

  SDL_DestroyWindow(window);

  SDL_Quit();
#endif
}
