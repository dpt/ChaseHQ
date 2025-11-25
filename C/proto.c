// proto.c
//
// Chase H.Q. code model
//
// by dpt

#include "SDL2/SDL_events.h"
#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_surface.h"
#include "SDL2/SDL_video.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <sys/_types/_null.h>
#include <sys/types.h>

/* ----------------------------------------------------------------------- */

/* Z80 instruction simulator macros. */

/**
 * Shift left arithmetic.
 */
#define SLA(r)      \
do {                \
  carry = (r) >> 7; \
  (r) <<= 1;        \
} while (0)

/**
 * Shift right logical.
 */
#define SRL(r)      \
do {                \
  carry = (r) & 1;  \
  (r) >>= 1;        \
} while (0)

/**
 * Rotate left.
 */
#define RL(r)                 \
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
#define RR(r)                       \
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
#define RRC(r)                      \
do {                                \
  carry = (r) & 1;                  \
  (r) = ((r) >> 1) | (carry << 7);  \
} while (0)

/* ----------------------------------------------------------------------- */

typedef int8_t T; // works

// $CDD6
T multiply(T a, T c)
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
    // $8007
    uint8_t  wanted_stage_number;

    // $A16D
    uint8_t  var_a16d;

    // $A188
    hazard_t hazards[6];

    // $A226
    uint8_t  correct_fork;

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

void build_curve_table(chqstate_t *state, int forked);

void build_curve_table_sub_cca8(chqstate_t *state,
                                uint8_t     Bdash_alwayszero,
                                uint16_t   *HLtableend,
                                uint16_t    DEroadpos);

/* ----------------------------------------------------------------------- */

#define CHATTER_STOP                        (0xFF)

///Chatter characters
#define CHATTERCHR_PILOT                    (0)
#define CHATTERCHR_NANCY                    (1)
#define CHATTERCHR_RAYMOND                  (2)
#define CHATTERCHR_TONY                     (3)

///Chatter strings
#define CHATTERSTR_THIS_IS_NANCY            (0)
#define CHATTERSTR_THIS_IS_AIRBORNE         (1)
#define CHATTERSTR_TARGET_VEHICLE_TURNED    (2)
#define CHATTERSTR_RIGHT_AHEAD_OVER         (3)
#define CHATTERSTR_LEFT_AHEAD_OVER          (4)
#define CHATTERSTR_READ_LOUD_CLEAR          (5)
#define CHATTERSTR_ROGER                    (6)
#define CHATTERSTR_GOTCHA_NANCY             (7)
#define CHATTERSTR_WHAT_YOU_DOING           (8)
#define CHATTERSTR_GOING_OTHER_WAY          (9)
#define CHATTERSTR_MESSIN_AROUND            (10)
#define CHATTERSTR_TIME_RUN_OUT             (11)
#define CHATTERSTR_GET_MOVIN_MAN            (12)
#define CHATTERSTR_OH_NO                    (13)
#define CHATTERSTR_PLEASE                   (14)
#define CHATTERSTR_GREAT                    (15)
#define CHATTERSTR_OUCH                     (16)
#define CHATTERSTR_LETS_GO                  (17)
#define CHATTERSTR_YAOW                     (18)
#define CHATTERSTR_BEAR_DOWN                (19)
#define CHATTERSTR_MORE_PUSH_MORE           (20)
#define CHATTERSTR_ONE_MORE_TIME            (21)
#define CHATTERSTR_OH_MAN                   (22)
#define CHATTERSTR_WHOA                     (23)
#define CHATTERSTR_HARDER                   (24)
#define CHATTERSTR_PICKED_WRONG_JOB         (25)
#define CHATTERSTR_CHECK_CLASSIFIED_ADS     (26)
#define CHATTERSTR_ONE_MORE_TRY             (27)
#define CHATTERSTR_MEDIOCRE_DRIVER          (28)
#define CHATTERSTR_SEE_YOU_LATER            (29)
#define CHATTERSTR__LIMIT                   (30)

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

// Conv: The C version uses an ID for strings in chatter data rather than an inline address.

// $98D0
static const uint8_t raymond_says_wrong_way_chatter[] = {
    CHATTERCHR_RAYMOND,
    CHATTERSTR_WHAT_YOU_DOING,
    CHATTERSTR_GOING_OTHER_WAY,
    CHATTER_STOP
};

// $9941
static const uint8_t tony_says_lets_go_mr_driver[] = {
    CHATTERCHR_TONY,
    CHATTERSTR_LETS_GO,
    CHATTER_STOP
};

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
void start_chatter(uint8_t priority, const uint8_t *chatter)
{
}

// $9CD6
void add_bonus(uint8_t A_low, uint16_t DE_high)
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
void layout_road(chqstate_t *state)
{
    uint8_t  *DElanedata;
    uint8_t   Biterations;
    uint8_t   Lcounter;
    uint16_t *SProadright;
    uint8_t   Aiterations;
    uint8_t   Aforkinprogress;
    uint16_t *SMroadcentre;
    uint16_t *SMroadcentreright;
    uint16_t *SMroadcentreleft;
    uint16_t *SMroadleft;
    uint16_t *SMroadright;
    uint16_t *SMveryright;
    uint16_t  BCdash;
    uint16_t  DEdash;
    uint16_t  HLdash;
    uint16_t  DEroadpos;
    const uint8_t *HLchatter;
    uint8_t   Ca16d;
    uint16_t  HLforkdistance;
    uint16_t  DEforkdistance;
    int       carry = 0;
    uint16_t  HLroadpos;
    uint16_t  HLroadpos_saved;
    uint8_t  *HLunknown;

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
        HLchatter = &tony_says_lets_go_mr_driver[0];
    } else {
        // Incorrect fork taken
        state->hazards[0].speed = 95; // boost perp speed from normal 60 (writes $A195)
        // Q. Why is a bonus awarded for going the wrong way?
        add_bonus(0, (state->wanted_stage_number + 4) << 8);
        HLchatter = &raymond_says_wrong_way_chatter[0];
    }
    start_chatter(20, HLchatter);
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
void build_curve_table(chqstate_t *state, int forked)
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
void build_curve_table_sub_cca8(chqstate_t *state,
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
