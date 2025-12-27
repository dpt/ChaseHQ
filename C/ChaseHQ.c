// proto.c
//
// Chase H.Q. code model
//
// by dpt

// vim: ts=8 sts=2 sw=2 et

// Notes
//
// As with my conversion of The Great Escape to C we model the game as if
// it's still running on a ZX Spectrum, including a Spectrum screen layout
// etc. This avoids a full rewrite of the original code and leaving (some)
// Z80-specific micro-optimisations in place. This means the code remains a
// useful basis for comparison and lowers the risk of translation errors.
// Although it's very tempting to rewrite all the code to be fully idiomatic
// C, the more different the code is made from the original disassembly, the
// harder it gets to refer back to it and spot our mistakes. The goal after
// all is to use this C conversion to expose problem points and feed those
// back into the disassembly's description.
//
// The level data (called "stage" data in this conversion to match the
// original game) is retained whole in the converted game, including any
// embedded addresses. This lets us 'page in' levels by copying the original
// game data into the game's state structure. This also means that any new or
// adjusted levels produced by means of this conversion will be compatible
// with the original game. It will be interesting to see, but unlikely, if
// the Sinclair User demo version of the game uses the same level format. The
// Amstrad CPC version will no doubt be considerably different but perhaps
// familiar.
//
// Generic code (code not interacting with stage data) will be converted to
// use native pointers. This means that some word-sized values will need to
// be indirected through new tables. For example see the "chatter" code: the
// code that prints the messages on-screen as the game runs. It previously
// embedded addresses inline in chatter structures. These are replaced with
// single bytes that reference a new tables of pointers.
//
// Like with TGE a game state structure is added to encapsulate the complete
// game state. It is passed to every state-accessing function in the game.
//
// Screen handling in the original game assumes the alignment of the screen
// and the back buffer. That can't be guaranteed in a portable conversion. We
// can address this by using posix_memalign to align the state structure and
// also by converting pointers to offsets when we need to perform address
// arithmetic.
//

// TODO
//
// Copy whole messages that get modified into the state structure.
//

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Types.h"
#include "Pixels.h"
#include "Spectrum.h"
#include "Z80.h"
#include "ChaseHQ-Data.h"
#include "ChaseHQ-State.h"
#include "ChaseHQ.h"

/* ----------------------------------------------------------------------- */

void chasehq_reset_state(chqstate_t *state)
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
static u16 wordat(const u8 *addr)
{
  return (addr[0] << 0) | (addr[1] << 8);
}

// given a Z80 address read a stagedata byte
static u16 stgbyte(chqstate_t *state, int address)
{
  int offset;
  assert(address >= STAGEDATA_BASE && address <= STAGEDATA_END);
  offset = address - STAGEDATA_BASE;
  return state->stagedata[offset];
}

// given a Z80 address read a stagedata word
static u16 stgword(chqstate_t *state, int address)
{
  int offset;
  assert(address >= STAGEDATA_BASE && address <= STAGEDATA_END);
  offset = address - STAGEDATA_BASE;
  return wordat(&state->stagedata[offset]);
}

// map a Z80 address to a stagedata pointer
static const u8 *stgmap(chqstate_t *state, int address)
{
  if (address == 0)
    return NULL;
  assert(address >= STAGEDATA_BASE && address <= STAGEDATA_END);
  return &state->stagedata[address - STAGEDATA_BASE];
}

// given a Z80 address read a stagedata word and map it to a native pointer into stagedata
static const u8 *stgwordtostgptr(chqstate_t *state, int address)
{
  return stgmap(state, stgword(state, address));
}

// given a native pointer read a word and map it to a native pointer into stagedata
static const u8 *ptrtostgptr(chqstate_t *state, const u8 *addr)
{
  return stgmap(state, wordat(addr));
}

/* ----------------------------------------------------------------------- */

// $8014 (copied to that position in the original)
// $F220 page_in_stage_128k
void load_stage(chqstate_t *state)
{
  static const u8 *stage_data_locations[] = {
    &stage1[0],
    &stage1[0],
    &stage1[0],
    &stage1[0],
    &stage1[0],
    &stage1[0]
  };

  u8 A;

  // Return if the stage is already loaded
  A = state->wanted_stage_number;
  if (A == state->current_stage_number)
    return;
  state->current_stage_number = A;

  // Copy the stage data from source to stagedata[]
  memcpy((u8 *) stgmap(state, STAGEDATA_BASE), stage_data_locations[A],
         STAGEDATA_LENGTH);
}

// $8258
void attract_mode(chqstate_t *state)
{
  int       carry = 0;
  u8        SM_828C; // blinker - move to state?
  u8        A;
  const u8 *HL;
  u8        B;

  set_up_stage(state, stgmap(state, 0x5D2B)); // attract_data
  SM_828C = 0;
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
      A = *HL; // load flags/attr - is this used in practice?
      HL = print_message(state, A, HL);
    } while (--B > 0);

    if (state->transition_control == TRANSITIONCONTROL_0) {
      // Alternate between credits and copyright messages.
      SM_828C ^= 1;
      HL = &credits_messages[0];
      if (SM_828C)
        HL = &copyright_messages[0];
      setup_overlay_messages(state, HL);
    }

    transition(state);
    draw_screen(state);
  }
}

// $83B5
void start_siren_hook(chqstate_t *state)
{
}

// $83B8
void play_engine_or_siren_sfx_hook(chqstate_t *state)
{
}

// $83BB
void silence_audio_hook(chqstate_t *state)
{
}

// $83BE
void write_audio_registers_hook(chqstate_t *state)
{
}

// $83C1
void setup_engine_sfx_hook(chqstate_t *state)
{
}

// $83C4
void play_engine_sfx_hook(chqstate_t *state)
{
}

// $83C7
void play_speech_hook(chqstate_t *state)
{
}

// $83CA
void attract_mode_hook(chqstate_t *state)
{
  attract_mode(state);
}

// $8401
void main_loop(chqstate_t *state)
{
  u8 start_speech_index; // was A

restart:
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
    if (handle_perp_caught(state))
      goto restart; // Conv: hpc would POP and goto main_loop to cause a
    // restart
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
void cpu_driver(chqstate_t *state)
{
  const u8 MinSpeed = 150;

  u16 roadpos; // was HL
  u8  input;   // was A

  roadpos = state->road_pos;
  input = USERINPUT_UP | USERINPUT_RIGHT;
  if (roadpos < ROAD_LEFTMOST) {
    input = USERINPUT_UP | USERINPUT_LEFT;
    if (roadpos >= ROAD_RIGHTMOST)
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
void set_up_stage(chqstate_t *state, const u8 *stage_data)
{
  u8  iterations; // was B
  u8 *HL;

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

  setup_transition(state, TRANSITION_REVERSE);

  clear_playfield_set_attrs(state);
  // Clear the lights' BRIGHT bit
  sus_clear_lights(ADDRTOSCREEN(0x5820));
  sus_clear_lights(ADDRTOSCREEN(0x583B));

  silence_audio_hook(state);
  update_scoreboard(state); // exit via
}

// $8860 (pulled out of above)
void sus_clear_lights(u8 *attrptr)
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
void check_user_input(chqstate_t *state)
{
  u8  Atransition;
  u8 *HLuserinput;
  u8  Ainput;
  u8 *HLboost;
  u8  Akey;

  Atransition = state->transition_control;
  HLuserinput = &state->user_input;
  if (Atransition != TRANSITIONCONTROL_4) {
    *HLuserinput = USERINPUT_NONE;
    return;
  }

  *HLuserinput = Ainput = (state->st.user_input_mask & *HLuserinput);
  if ((Ainput & (USERINPUT_QUIT | USERINPUT_PAUSE | USERINPUT_TURBO)) == 0)
    return;

  if (Ainput & USERINPUT_QUIT) {
    check_user_input_quit_key(state);
    return;
  } else if (Ainput & USERINPUT_PAUSE)
    goto pause_key;

  // Turbo pressed
  HLboost = &state->boost;
  if (*HLboost > 0 || state->st.turbos == 0)
    return; // already boosting or no turbos remain
  *HLboost = 60; // set ticks of boost

  start_chatter(state, 2, &chatterblk_turbo[0]);
  setup_engine_sfx_hook(state); // exit via
  return;

  // Conv: check_user_input_quit_key hoisted out from here.

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

// $88A9
void check_user_input_quit_key(chqstate_t *state)
{
  if (state->quit_state == QUITSTATE_IDLE) {
    drive_chatter_stop(state);
    fill_attributes(state);

    state->st.user_input_mask = USERINPUTMASK_ALLOW_NONE;
    state->quit_state         = QUITSTATE_START;
  }
}

// $88D5
void clear_playfield_attrs(chqstate_t *state)
{
  memset(ADDRTOSCREEN(0x5900), attribute_BLACK_OVER_BLACK,
         SCREEN_ATTRIBUTES_ROWBYTES * PLAYFIELD_HEIGHT / 8);
}

// $88E2
void clear_playfield(chqstate_t *state)
{
  clear_playfield_attrs(state);
  memset(ADDRTOSCREEN(0x4800), 0x00, SCREEN_BITMAP_ROWBYTES * PLAYFIELD_HEIGHT);
}

// $88F2
void start_sfx(chqstate_t *state, u8 Bindex, u8 Cpriority)
{
}

// $8903
void drive_sfx(chqstate_t *state)
{
}

// $8960
void sfx_crash(chqstate_t *state, u8 Dparam)
{
}

// $89D9
void sfx_thud(chqstate_t *state, u8 Dparam)
{
}

// $8A0F
void sfx_cornering(chqstate_t *state, u8 Dparam, u8 Eparam)
{
}

// $8A36
void sfx_bipbow(chqstate_t *state, u8 Dparam, u8 Eparam)
{
}

// $8A57
int handle_perp_caught(chqstate_t *state)
{
  int       carry = 0;
  int       zero = 0;
  u8        phase; // was A
  u8        A;
  u8        Ainput;
  u8        H;
  u8        L;
  u8        D;
  u8        C = 0; // tmp fix
  u8        Cinput;
  u8        Biterations;
  u8        Bdelta;
  u8        Adash;
  u8       *DE;
  u8        DEspeed;
  u8       *HLscore;
  u8       *HLphc;
  u8        Aperpdistance;
  u16       HLspeed;
  u16       HLspeedpushed;
  u16       HLroadpos;
  const u8 *HLmessages;
  u8        Cflag;

  phase = state->perp_caught_phase;
  if (phase == PERPCAUGHTPHASE_0)
    return 0;

  phase--;
  if (phase == 0) goto hpc_move_perp;
  phase--;
  if (phase == 0) goto hpc_phase2;
  phase--;
  if (phase == 0) goto hpc_phase3;
  phase--;
  if (phase == 0) goto hpc_phase4;

  // Otherwise 5/6
  if (state->transition_control)
    return 0;
  phase--;
  if (phase == 0) goto hpc_phase5;

  // Must be 6
  silence_audio_hook(state);
  state->wanted_stage_number++;
  return 1; // Conv: signal to bypass remainder of main loop

hpc_phase5:
  state->perp_caught_phase = PERPCAUGHTPHASE_6;
  setup_transition(state, TRANSITION_FORWARD); // was exit via
  return 0;

hpc_phase2:
  A = state->car_y;
  if (A >= 16)
    goto hpc_start_phase_3;
  state->car_y = A + 4;

  HLroadpos = state->road_pos + 12;
  if (HLroadpos >= 0x126)
    HLroadpos = 0x126;
  state->road_pos = HLroadpos;

  A = state->fast_counter + 32;
  if (state->fast_counter + 32 > 255)
    return 0;
  state->fast_counter = A;
  return 0;

hpc_start_phase_3:
  state->perp_caught_phase = 3;
  state->SM_8ABE = 4;
  fill_attributes(state); // exit via
  return 0;

hpc_phase3:
  A = state->SM_8ABE - 1;
  state->SM_8ABE = A;
  if (A)
    return 0;

  state->perp_caught_phase = PERPCAUGHTPHASE_4;
  HLmessages = stgwordtostgptr(state, 0x5D06); // addrof_arrest_messages
  setup_overlay_messages_with_A(state, TRANSITIONCONTROL_1,
                                HLmessages); // was exit via
  return 0;

hpc_phase4:
  if (state->mode_128k) {
    // TODO: handle_perp_caught_128k(state);
  }
  if (state->transition_control)
    return 0;
  if (state->mode_128k) {
    // TODO: handle_perp_caught_128k(state);
  }

  state->perp_caught_phase = PERPCAUGHTPHASE_5;

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

  A = state->st.time_bcd;
  state->score_messages[0x8C8A - SCORE_MESSAGES_BASE] =
    A; // Write to TIME BONUS line
  C = A;
  RLC(A);
  RLC(A);
  RLC(A);
  RLC(A);
  A &= 0xF;
  if (A)
    goto hpc_have_high_digit;

  Adash = ' ';
  goto hpc_store_time_bonus_high;

hpc_have_high_digit:
  Biterations = A;
  A += '0';
  Adash = A;
  do
    increment_score(state, 0, 0x00, 0x05); // 50,000 lo,mid,hi
  while (--Biterations > 0);
hpc_store_time_bonus_high:
  A = Adash;
  state->score_messages[0x8C8A - SCORE_MESSAGES_BASE] =
    A; // Write to TIME BONUS line
  A = C & 0x0F;
  if (A == 0)
    goto hpc_store_time_bonus_low;

  // Bug fix applied
  Biterations = A;
  Adash = A;
  do
    increment_score(state, 0, 0x50, 0x00); // 5,000 lo,mid,hi
  while (--Biterations > 0);
hpc_store_time_bonus_low:
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
      goto hpc_score_have_high_digit;

    RLC(A);
    if (carry)
      goto hpc_score_have_high_digit;
    A = ' ';
    goto hpc_score_store_high;

hpc_score_have_high_digit:
    Cflag = 0xFF;
    A += '0';

hpc_score_store_high:
    *HLscore++ = A;
    A = *DE & 0x0F;
    if (A)
      goto hpc_score_have_low_digit;

    RLC(Cflag);
    if (carry)
      goto hpc_score_have_low_digit;
    A = ' ';
    goto hpc_score_store_low;

hpc_score_have_low_digit:
    Cflag = 0xFF;
    A += '0';

hpc_score_store_low:
    *HLscore++ = A;
    DE--;
  } while (--Biterations > 0);

  HLscore--;
  *HLscore |= STREND;

  setup_overlay_messages(state, &state->score_messages[0]); // was exit via
  return 0;

hpc_move_perp:
  A = state->hazards[0].horz_pos;
  Bdelta = 5;
  if (A == 35) goto hpc_assign_perp_pos;
  else if (A < 35) goto hpc_change_perp_pos;
  Bdelta = -5; // else greater than

hpc_change_perp_pos:
  C = A + Bdelta;

hpc_assign_perp_pos:
  A = C;
  state->hazards[0].horz_pos = A;
  HLroadpos = state->road_pos;
  // PUSH HLroadpos
  carry = (HLroadpos < ROAD_LEFTMOST); // was SUB
  // POP HLroadpos
  Ainput = USERINPUT_UP | USERINPUT_RIGHT;
  if (!carry)
    goto hpc_assign_hero_pos;
  HLroadpos -= ROAD_RIGHTMOST;
  Ainput = USERINPUT_UP | USERINPUT_LEFT;
  if (carry)
    goto hpc_assign_hero_pos;
  Ainput = USERINPUT_UP;

hpc_assign_hero_pos:
  Cinput = Ainput & (USERINPUT_LEFT | USERINPUT_RIGHT);
  if (Cinput)
    goto hpc_perp_too_far_away;

  if (state->hazards[0].distance >= 3)
    goto hpc_perp_too_far_away;
  HLphc = &state->st.perp_halt_counter;
  (*HLphc)--;
  if (*HLphc)
    goto hpc_perp_too_far_away;
  state->speed = 0;
  state->hazards[0].TBD4 = 0;
  state->hazards[0].distance = 1;
  state->perp_caught_phase = PERPCAUGHTPHASE_2;
  state->smoke = 3;
  DEspeed = 0; // Conv
  goto hpc_set_perp_speed;

hpc_perp_too_far_away:
  Aperpdistance = state->hazards[0].distance;
  HLspeed = 350;
  if (Aperpdistance < 15) {
    Biterations = 16 - Aperpdistance;
    do HLspeed -= 20;
    while (--Biterations > 0);
  }
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
  carry = (HLspeed > 150);
  HLspeed -= 150;
  A = state->gear - carry; // set low speed if speed<150
  if (A == 0)
    Cinput |= USERINPUT_FIRE; // change gear
  state->user_input = Cinput;

  HLspeed = state->hazards[0].speed;
  HLspeedpushed = HLspeed; // PUSH HL
  DEspeed = 70;
  carry = (HLspeed < DEspeed);
  HLspeed -= DEspeed;
  zero = (HLspeed == 0);
  HLspeed = HLspeedpushed; // POP HL
  if (zero || carry) {
    goto hpc_set_perp_speed; // with DE=70
  }
  HLspeed -= 5;
  DEspeed = HLspeed;

hpc_set_perp_speed:
  hpc_set_perp_speed(state, DEspeed); // was fallthrough
  return 0;
}

// $8C35
void hpc_set_perp_speed(chqstate_t *state, u16 DEspeed)
{
  state->hazards[0].speed = DEspeed;
}

// $8C3A
void fully_smashed(chqstate_t *state)
{
  state->perp_caught_phase  = PERPCAUGHTPHASE_1;
  state->hand_flag          = 2; // need symbol for this
  state->smash_counter      = 20;
  state->st.user_input_mask = USERINPUT_PAUSE | USERINPUT_QUIT;
  setup_overlay_messages(state, &pull_over_message[0]);
  hpc_set_perp_speed(state, 400);
}

// $8D8F
void transition(chqstate_t *state)
{
  u8 A;

  A = state->transition_control;
  if (A == 0)
    return;

  A--;
  if (A == 0) { draw_mugshots(state); return; }
  A--;
  if (A == 0) { draw_overlay_messages(state); return; }
  A--;
  if (A == 0) { fill_attributes(state); return; }
  // Otherwise it's 4

  // TODO
}

// $8DF9
void setup_transition(chqstate_t *state, u8 Atransition)
{
  u16  BC;
  u16  DE;
  u8  *HL;

  BC = Atransition;
  DE = 0xEC00; // table ptr
  if (Atransition < 0) {
    BC = 0xFF00 | (BC & 0x00FF);
    DE = 0xEC0C; // table ptr
  }

  state->SM_8DB1 = BC;

  //  // Pick a random entry in the table
  //  HL = &DE[(rng(state) & 3) * 3];
  //
  //  state->SM_8DA1 = *HL++;
  //  state->SM_8DBB = wordat(HL); // Set transition animation start address
  state->transition_control = TRANSITIONCONTROL_4;
}

// $8E29
void fill_attributes(chqstate_t *state)
{
  u8 *src;     // was HL
  u8 *dst;     // was DE
  int      rows;    // was A
  int      columns; // was BC

  src = ADDRTOSCREEN(0x5901); // (1,8)
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

  state->transition_control = TRANSITIONCONTROL_0;
}

// $8E42
void draw_overlay_messages(chqstate_t *state)
{
  const u8 *message;    // was HL
  u8        iterations; // was B
  u8        delay;      // was A
  u8        flags;      // was A

  message    = state->SM_8E43;
  iterations = state->SM_8E46;
  for (;;) {
    if (--iterations == 0) {
      delay = state->SM_8E4A_delay - 1;
      state->SM_8E4A_delay = delay;
      if (delay)
        return;

      state->SM_8E4A_delay = *message; // set new delay
      state->SM_8E46++;
      iterations++;
    }

    flags = *++message;
    if (flags == 0)
      break;

    print_message(state, flags, message);
  }

  state->transition_control = message[-1];
}

// $8E6C
const u8 *print_message(chqstate_t    *state,
                        u8        Aflags,
                        const u8 *HLmessages)
{
  u8  attr;     // was A
  u16 backbuf;  // was DE
  u16 attraddr; // was BC

  attr     = HLmessages[0];
  backbuf  = (HLmessages[2] << 8) | HLmessages[1];
  attraddr = (HLmessages[4] << 8) | HLmessages[3];
  HLmessages += 5;

  draw_string_A(state, attr, ADDRTOSCREEN(attraddr), ADDRTOBACKBUF(backbuf),
                HLmessages,
                Aflags);

  return HLmessages;
}

// $8E7E
void setup_overlay_messages(chqstate_t *state, const u8 *HL)
{
  setup_overlay_messages_with_A(state, 2, HL);
}

// $8E80
void setup_overlay_messages_with_A(chqstate_t    *state,
                                   u8        Atransition,
                                   const u8 *HL)
{
  u8 delay; // was A

  state->transition_control = Atransition;
  delay = *HL++;
  state->SM_8E4A_delay = delay;
  state->SM_8E43 = HL;
  state->SM_8E46 = 1;
}

// $8E91
void draw_mugshots(chqstate_t *state)
{
  draw_mugshot(state, 0x48A5, 0xFF88, stgwordtostgptr(state, 0x5CF0));
  draw_mugshot(state, 0x48B4, 0xFF97,
               &bitmap_faces[2 * FACEBYTES + FACEBITMAPBYTES]);
  draw_mugshot(state, 0x48B9, 0xFF9C,
               &bitmap_faces[1 * FACEBYTES + FACEBITMAPBYTES]);
  //tmp nobbled draw_overlay_messages(state);
}

// Returns the previous row for the back buffer (visually upwards).
//
// Back buffer addresses are of the form 0b_1111_LLLL_RRRC_CCCC
//
// Conv: Extracted to function.
u16 prevbufrow(u16 backbuf)
{
  int orig;

  orig = backbuf;
  backbuf -= 256;
  if ((orig & 0x0F00) == 0) {
    backbuf += 0x1000; // re-add borrow?
    int t = (backbuf & 0xFF) - 32;
    backbuf = (backbuf & 0xFF00) | (t & 0xFF);
    if (t >= 0x0100) // did carry
      backbuf -= 0x1000;
  }
  return backbuf;
}

// $8EB7
void draw_mugshot(chqstate_t    *state,
                  u16       BCscreenpos,
                  u16       DEbackbuf,
                  const u8 *HLmugshot)
{
  const u8 *orig_mugshot; // was PUSH-POP
  u16       counter;      // was BC

  orig_mugshot = HLmugshot;
  HLmugshot--; // step back from attributes start to bitmap data end
  counter = FACEBITMAPBYTES;
  DEbackbuf -= BACKBUFFER_START_ADDRESS; // Conv: address -> offset
  for (;;) {
    state->backbuffer[DEbackbuf--] = *HLmugshot--; counter--;
    state->backbuffer[DEbackbuf--] = *HLmugshot--; counter--;
    state->backbuffer[DEbackbuf--] = *HLmugshot--; counter--;
    state->backbuffer[DEbackbuf--] = *HLmugshot--; counter--;
    DEbackbuf += 4; // Conv: replaces register stash
    if (counter == 0)
      break;

    DEbackbuf = prevbufrow(DEbackbuf);
  }

  plot_face_attributes(state, BCscreenpos, orig_mugshot); // exit via
}

// $8EE7
void draw_smash_bar(chqstate_t *state)
{
  const int TotalBarHeight = 64;
  const int BottomEdgeHeight = 2;
  const int SegmentHeight = 3;

  u16 HLbuf;
  u8  nsmashsegs; // was A
  u8  nsolid; // was B

  if (state->sighted_flag == 0)
    return; // Return if the perp has not yet been sighted

  if (state->perp_caught_phase >= PERPCAUGHTPHASE_3)
    return; // Return if perp_caught_phase is >= 3 (car has stopped)

  HLbuf = 0xF7A2; // Back buffer address of bottom of bar
  // Conv: D & E moved into prevbufrow forward

  // Draws bottom two rows
  HLbuf = draw_smash_bar_solid_bit(state, BottomEdgeHeight, HLbuf);

  nsmashsegs = state->smash_counter;
  if (nsmashsegs > 0)
    HLbuf = draw_smash_bar_segment(state, nsmashsegs, HLbuf);

  nsolid = TotalBarHeight - BottomEdgeHeight - nsmashsegs *
           SegmentHeight; // number of solid rows to draw at the top
  (void) draw_smash_bar_solid_bit(state, nsolid, HLbuf); // exit via
}

u16 draw_smash_bar_segment(chqstate_t *state, u8 Cnrows, u16 HLbuf)
{
  do {
    *ADDRTOBACKBUF(HLbuf) = X______X; // Set 8 pixels)
    HLbuf = prevbufrow(HLbuf);
    *ADDRTOBACKBUF(HLbuf) = X______X; // Set 8 pixels)
    HLbuf = prevbufrow(HLbuf);
    HLbuf = draw_smash_bar_solid_bit(state, 1, HLbuf); // 1 row gap
  } while (--Cnrows > 0);
  return HLbuf;
}

u16 draw_smash_bar_solid_bit(chqstate_t *state, u8 Bnrows, u16 HLbuf)
{
  do {
    *ADDRTOBACKBUF(HLbuf) = XXXXXXXX; // Set 8 pixels)
    HLbuf = prevbufrow(HLbuf);
  } while (--Bnrows > 0);
  return HLbuf;
}

// $8F5F
void draw_everything_else(chqstate_t *state)
{
}

// $961B
u8 rng(chqstate_t *state)
{
  int      carry = 0;
  u8 *seed; // was HL
  u8  A;

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
void start_chatter(chqstate_t       *state,
                   chatterpriority_t priority,
                   const u8    *chatterblk)
{
  u8 chatter_state; // was A

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
void drive_chatter(chqstate_t *state)
{
  u8        chatter_state; // was A
  int            carry = 0;
  char           character; // was D
  u8        rotating; // was A
  u8        delay; // was A
  u8        x; // was A
  u8        B;
  const char    *HL;
  const u8 *chatterblk; // was HL
  u8        chattercmd; // was A

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
void drive_chatter_stop(chqstate_t *state)
{
  state->noise_counter = 4;
  state->chatter_state = CHATTERSTATE_STOP;
  clear_message_line(state); // exit via
}

// $99EC
void print_chatter(chqstate_t *state)
{
  const u8 *chatter; // was HL
  u8        cmd;     // was A
  u8        rnd;     // was A
  const u8 *face;    // was HL

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
    face = &bitmap_faces[cmd * FACEBYTES]; // Conv: Simplified

  plot_face(state, 0x4036, face); // Set plot address to (176,8)

  pc_chatter_message(state, chatter); // was FALLTHROUGH
}

// $9A24
void pc_chatter_message(chqstate_t *state, const u8 *HLchatter)
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
void pc_clear_line(chqstate_t *state, u8 x)
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
void noise_effect(chqstate_t *state, u8 counter)
{
  state->noise_counter = --counter;
  if (counter == 0)
    print_chatter(state); // exit via
  else
    noise_effect_9a5c(state, counter); // was FALLTHROUGH
}

// Move to next screen row (downwards)
// Conv: added
u16 nextscrrow(u16 screen)
{
  screen += 256;
  if (((screen >> 8) & 7) == 0) {
    int t = (screen & 0xFF) + 32;
    screen = (screen & 0xFF00) | (t & 0xFF);
    if (t < 0x100) { // didn't carry
      t = (screen >> 8) - 8; // reduce?
      screen = (t << 8) | (screen & 0xFF);
    }
  }
  return screen;
}

// $9A5C
void noise_effect_9a5c(chqstate_t *state, u8 counter)
{
  u8  x;               // was A
  char     character;       // was D
  u8  C;
  int      carry = 0;
  u8  B;
  u8 *HLnoisebytes;
  u16 DEscreen;
  u16 DEscreen_saved;
  u8  A;               // was A

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
    DEscreen = nextscrrow(DEscreen);
  } while (--C > 0);

  ne_plot_attrs(state, 0x47); // BRIGHT + white over black
  // was FALLTHROUGH
}

// $9A98
void ne_plot_attrs(chqstate_t *state, u8 attr)
{
  u16 HL;
  u8  B;
  u16 DE;

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
void plot_face(chqstate_t    *state,
               u16       DEscreen,
               const u8 *HLface)
{
  u16 DEscreen_saved;
  u16 counter; // was BC

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
    DEscreen = nextscrrow(DEscreen);
  }

  plot_face_attributes(state, DEscreen_saved, HLface); // was fallthrough
}

// $9ACE
//
// DEscreen - was POP
// HLface - HL
void plot_face_attributes(chqstate_t    *state,
                          u16       DEscreen,
                          const u8 *HLface)
{
  int      carry = 0;
  u8  A;
  u16 counter; // was BC

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

    // TODO Hoist to next-attr-row macro?
    int t = (DEscreen & 0xFF) + 0x1C;
    DEscreen = (DEscreen & 0xFF00) | (t & 0xFF);
    if (t >= 0x100)
      DEscreen += 256;
  }
}

// $9AEC
void plot_mini_font_cursor_off(chqstate_t *state,
                               u8     x,
                               char        character)
{
  pmf_go(state, x, character, ________, ________);
}

// $9AF1
void plot_mini_font_cursor_on(chqstate_t *state,
                              u8     x,
                              char        character)
{
  pmf_go(state, x, character, _____XXX, X_______);
}

// $9AF4
void pmf_go(chqstate_t *state,
            u8     x,     // was A
            char        ascii, // was D
            u8     B,     // extra bitmap
            u8     C)
{
  int            carry = 0;

  u8        extra2;  // was self modified $9B61 - right extra bitmap
  u8        extra1;  // was self modified $9B64 - left extra bitmap
  u8        mask;    // was self modified $9B89
  u8        rotate;  // was self modified $96B7

  u8        A;
  u8        ascii2; // was A
  u8        row;    // was A
  u8        gid;    // was C
  u8        sgid;   // was A
  u16       screen; // was DE
  const u8 *DEfont;
  u16       HLscreen;

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
    u8 bm2; // was C
    u8 bm1; // was B

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

    u8 *screen = ADDRTOSCREEN(HLscreen); // Conv: added
    screen[0] = (mask & screen[0]) | bm1;
    screen[1] = bm2;
    DEfont++;
    HLscreen = nextscrrow(HLscreen);
  } while (--row > 0);
}

// $9BA7
void clear_message_line(chqstate_t *state)
{
  u16 screen; // was HL
  u8  rows;   // was A

  screen = 0x45C1; // Screen coordinate (8,53)
  rows = 6; // Clear six rows
  do {
    memset(ADDRTOSCREEN(screen + 1), 0, 29); // Conv: Replacing LDIR
    screen = nextscrrow(screen);
  } while (--rows);
}

// $9BCF
void tick(chqstate_t *state)
{
  int      carry = 0;
  u8 *ptimebcd;    // was HL
  u8  timeupstate; // was A
  u8  timebcd;     // was A
  char    *timedigits;  // was DE
  u8  effect;      // was B
  u8  H;
  u8  L;
  u8  A;
  u8  hidigit;      // was A
  u8  lodigit;      // was L

  if (state->perp_caught_phase > PERPCAUGHTPHASE_0
      || state->transition_control == TRANSITIONCONTROL_4)
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
  if (state->transition_control > TRANSITIONCONTROL_0)
    return;
  if (state->credits == 0) {
    check_user_input_quit_key(state); // exit via
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
  state->transition_control = TRANSITIONCONTROL_3;
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
  start_sfx(state, effect, 1);  // 1 for high priority
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
void speed_score(chqstate_t *state)
{
  u16 speed; // was HL
  u8  A;
  u8  H;
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
void add_bonus(chqstate_t *state,
               u8     A_lo,
               u8     E_md,
               u8     D_hi)
{
  char   *output;      // was HL
  u8 nonzeroflag; // was C - used to track if a zero has been emitted.

  output = &state->bonus_string[6]; // points to byte after buffer
  nonzeroflag = 0xFF; // flag (zero not seen)
  (void) bonus_digit(A_lo >> 0, &nonzeroflag,
                     &output); // always runs since flag > 0
  *output |= STREND; // terminate string

  // Using lazy evaluation here to avoid having a load of gotos
  (void)(bonus_digit(A_lo >> 4, &nonzeroflag, &output) >= 0 &&
         bonus_digit(E_md >> 0, &nonzeroflag, &output) >= 0 &&
         bonus_digit(E_md >> 4, &nonzeroflag, &output) >= 0 &&
         bonus_digit(D_hi >> 0, &nonzeroflag, &output) >= 0 &&
         bonus_digit(D_hi >> 4, &nonzeroflag, &output) >= 0);

  state->SM_address_of_score_digits = output;
  state->trigger_bonus_flag = 1;
  increment_score(state, A_lo, E_md, D_hi); // was fallthrough
}

// Subroutine of above broken out
int bonus_digit(u8  digit,
                u8 *nonzeroflag,
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
void increment_score(chqstate_t *state,
                     u8     A_lo,
                     u8     E_md,
                     u8     D_hi)
{
  int      carry = 0;
  u8 *scorebcd; // was HL
  u8  A;

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
void calc_overtake_bonus(chqstate_t *state)
{
  int      carry = 0;
  u8  A;
  u8  B;
  u8 *HL;

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
void update_scoreboard(chqstate_t *state)
{
  toggle_light_brightness(state, ADDRTOSCREEN(0x5820));
  toggle_light_brightness(state, ADDRTOSCREEN(0x583B));
  plot_turbos_and_scores(state);
}

// $9DF4
void toggle_light_brightness(chqstate_t *state, u8 *HL)
{
  u8 B;
  u8 C;

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
void plot_turbos_and_scores(chqstate_t *state)
{
  int             carry = 0;
  u8         Aturbos;
  u8         Cturbos;
  u8         Aboost;
  const u8  *HLbitmap;
  u8         Aframe;
  u8         B;
  const u16 *SM_9e45;
  u8         A;
  const u16 *SPbitmap;
  u8        *HLscreen;
  u16        DEbitmap;
  u8        *DEscreen;
  u16        DEdash_speed;
  u8         Bdash_iterations;
  u8         Ascale;
  u16        HLdash;
  u16        BCdash;
  u8         Ddash;
  u8         Edash;
  u8        *DEbcd;
  u16        HLdistance;
  u16        BCdivisor;

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
    SM_9e45 = (const u16 *) HLbitmap; // local
    //SM_9e79 = SP; // save old SP

    A = 0xE1; // Low byte of back buffer draw address
    do {
      SPbitmap = (const u16 *) &bitmap_turbospin[0];
      Cturbos--;
      if (Cturbos == 0)
        SPbitmap = SM_9e45;
      Cturbos++;
      HLscreen = ADDRTOBACKBUF(0xFE00 | A);
      // EX AF,AF'
      B = TURBOHEIGHT;
      do {
        u8 Emask, Dbitmap;

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

  DEscreen = ADDRTOSCREEN(0x4132); // speed digits pos (144,9)
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
                  ADDRTOSCREEN(0x412F)); // (120,9)

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
                  ADDRTOSCREEN(0x4191)); // was fallthrough

  ptas_led_digits(state, 4, &state->score_bcd[3], &state->st.score_digits[7],
                  ADDRTOSCREEN(0x4126)); // was fallthrough
}

// $9F1E
//
// Biterations was B
// DEdigits was DE
// HLstored was HL
// DEscreen was DE'
void ptas_led_digits(chqstate_t    *state,
                     u8        Biterations,
                     const u8 *DEdigits,
                     u8       *HLstored,
                     u8       *DEscreen)
{
  u8 Adigits;
  u8 Cdigits;

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
u8 *ledfont_plot(chqstate_t *state, u8 ord, u8 *screen)
{
  const u8 *font;        // was HL
  u8       *screen_copy; // was stacked

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
void draw_string_A(chqstate_t    *state,
                   u8        Aattr,
                   u8       *BCattrs,
                   u8       *DEbackbuf,
                   const u8 *HLstring,
                   u8        Adash)
{
  draw_string_entry(state, DEbackbuf, HLstring/*HL*/, Adash/*Adash*/, Aattr/*C'*/,
                    32/*DE'*/, BCattrs/*HL'*/);
}

// $9FA3
void draw_string(chqstate_t    *state,
                 u8        Aattr,
                 u8       *BCattrs,
                 u8       *DEbackbuf,
                 const u8 *HLstring)
{
  draw_string_entry(state, DEbackbuf, HLstring/*HL*/, 1/*Adash*/, Aattr/*C'*/,
                    32/*DE'*/, BCattrs/*HL'*/);
}

// $9FA6
void draw_string_entry(chqstate_t    *state,
                       u8       *DEscreen,
                       const u8 *HLstring,
                       u8        Adash,
                       u8        Cdash,
                       u8        DEstride, // was DE'
                       u8       *HLattrs)  // was HL'
{
  u8 Achar; // was A

  do {
    Achar = *HLstring & ~STREND;
    draw_char(state, Achar, DEscreen, Adash, Cdash, DEstride, HLattrs);
  } while ((Achar & STREND) == 0);
}

// $9FB4
void draw_char(chqstate_t *state,
               u8     Achar,
               u8    *DEscreen, // screen address
               u8     Adash,    // draw type
               u8     Cdash,    // attribute
               u8     DEstride, // was DE' e.g. 32 - a stride?
               u8    *HLattrs)  // was HL'
{
  u8        Cglyphid; // was C
  u8        Ctype;    // was C
  u8        Adata;    // was A
  u8        B;
  const u8 *HLfont;   // was HL
  u8       *DEorig;   // was stacked

  Achar -= ' ';
  if (Achar == 0) {
    // Space
    DEscreen++;
    HLattrs++; // FIXME: Do we need to return these?
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
    Adata = *HLfont;
    *DEscreen = Adata;
    DEscreen += 256;
    *DEscreen = Adata;
    DEscreen += 256;
    HLfont++;
  } while (--B > 0);
  DEscreen -= 8 * 256;
  DEscreen += 32;
  B = 3; // iterations
  do {
    Adata = *HLfont;
    *DEscreen = Adata;
    DEscreen += 256;
    *DEscreen = Adata;
    DEscreen += 256;
    HLfont++;
  } while (--B > 0);
  goto dc_set_double_attrs;

  // double height inverted
dc_double_height_inverted:
  DEorig = DEscreen;
  B = 7; // iterations
  do {
    Adata = ~*HLfont;
    *DEscreen = Adata;
    DEscreen += 256;
    *DEscreen = Adata;
    DEscreen += 256;
    HLfont++;
  } while (--B > 0);
  goto dc_set_double_attrs;

dc_single_height_inverted:
  DEorig = DEscreen;
  B = 7; // iterations
  do {
    Adata = ~*HLfont;
    *DEscreen = Adata;
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
    Adata = *HLfont;
    *DEscreen = Adata;
    DEscreen += 256;
    *DEscreen++ = *HLfont++; // was LDI, could reuse A
    DEscreen--; // was DEC E, could remove if DEscreen++ above is dropped
    DEscreen += 256;
  }
  *DEscreen = 0; // leave gap at bottom

dc_set_double_attrs:
  DEscreen = DEorig + 1; // was POP DEscreen, INC E
  *HLattrs |= Cdash;
  HLattrs += DEstride;
  *HLattrs |= Cdash;
  HLattrs -= DEstride; // was POP HLattrs
  HLattrs++; // was INC L
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
  *HLattrs |= Cdash;
  HLattrs++; // was INC L
  return;

dc_generic:
  DEorig = DEscreen;
  B = 7; // iterations
  do {
    *DEscreen = *HLfont;
    DEscreen += 256;
    HLfont++;

    // variation on nextscrrow()
    // DEscreen = nextscrrow(DEscreen); // won't work!
  } while (--B > 0);
  DEscreen = DEorig + 1; // was POP DEscreen
  return;
}

// $A0D6
u8 keyscan(chqstate_t *state)
{
  return 0;
}

// $A399
void check_scenery_collisions(chqstate_t *state)
{
}

// $A579
void layout_objects(chqstate_t *state)
{
}

// $A60E
void cycle_counters(chqstate_t *state)
{
  state->counter_A = (state->counter_A + 1) & 3;
  state->counter_B = (state->counter_B + 1) & 1;
  if (state->counter_B == 0)
    return;
  state->counter_C = (state->counter_C + 1) & 3;
}

// $A7F3
void spawn_cars(chqstate_t *state)
{
}

// $A955
void choose_dirt_and_stones(chqstate_t *state)
{
}

// $A97E
void layout_dirt_and_stones(chqstate_t *state)
{
}

// $AAC6
void move_helicopter(chqstate_t *state)
{
}

// $AB33
void drive_helicopter(chqstate_t *state)
{
}

// $AB9A
void spawn_hazards(chqstate_t *state)
{
}

// $ADA0
void draw_hazards(chqstate_t *state)
{
}

// $B063
void move_hero_car(chqstate_t *state)
{
}

// $B318
void animate_hero_car(chqstate_t *state)
{
}

// $B848
void scroll_horizon(chqstate_t *state)
{
}

// $B8D2
void update_road_level(chqstate_t *state)
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
  int            carry = 0;
  u8       *DElanedata;
  u8        Biterations;
  u8        Lcounter;
  u16      *SProadright;
  u8        Aiterations;
  u8        Aforkinprogress;
  u16      *SMroadcentre;
  u16      *SMroadcentreright;
  u16      *SMroadcentreleft;
  u16      *SMroadleft;
  u16      *SMroadright;
  u16      *SMveryright;
  u16       BCdash;
  u16       DEdash;
  u16       HLdash;
  u16       DEroadpos;
  const u8 *HLchatterblk;
  u8        Ca16d;
  u16       HLforkdistance;
  u16       DEforkdistance;
  u16       HLroadpos;
  u16       HLroadpos_saved;
  u8       *HLunknown;

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
void exit_fork(chqstate_t *state)
{
}

// The screen has the format 0b010BBLLLRRRCCCCC (B = band, L = scanline, R = row (group), C = column)
// The buffer has the format 0b1111LLLLRRRCCCCC (L = scanline, R = row (group))

// $BC3E
void draw_screen(chqstate_t *state)
{
  u8  *scr;       // was HL
  u8  *buf;       // was HL'
  u16  bufoffset; // Conv: added

  scr = ADDRTOSCREEN(0x4811); // (136, 64)
  buf = ADDRTOBACKBUF(0xF001); // (8, 1)

  for (;;) {
    // First do left hand side (original reads forwards, stores backwards)
lefthand_16_bytes:
    memcpy(scr - 16, buf, 16); scr += 256; buf += 256;
    memcpy(scr - 16, buf, 16); scr += 256; buf += 256;
    memcpy(scr - 16, buf, 16); scr += 256; buf += 256;
    memcpy(scr - 16, buf, 16); scr += 256; buf += 256;
    bufoffset = BACKBUFTOOFFSET(buf); // Conv: convert back to offset
    // Loop on the first pass (4 lines of 8 done) but not the second
    if (bufoffset & (1 << 10))
      goto lefthand_16_bytes;

    // Now move over to the right hand side
    // e.g. (0xF001 + 8*256 - 0x7F0) == 0xF011 on the first pass
    buf = OFFSETTOBACKBUF(bufoffset - 0x07F0);
    scr = OFFSETTOSCREEN(SCREENTOOFFSET(scr) - 0x07F2);
righthand_14_bytes:
    memcpy(scr - 14, buf, 14); scr += 256; buf += 256;
    memcpy(scr - 14, buf, 14); scr += 256; buf += 256;
    memcpy(scr - 14, buf, 14); scr += 256; buf += 256;
    memcpy(scr - 14, buf, 14); scr += 256; buf += 256;
    bufoffset = BACKBUFTOOFFSET(buf); // Conv: convert back to offset
    // Loop on the first pass (4 lines of 8 done) but not the second
    if (bufoffset & (1 << 10))
      goto righthand_14_bytes;

    if ((bufoffset & (1 << 11)) == 0) {
      u8 H;
      u8 A;
      u8 L;
      int     res;
      int     carry, overflow;

      // Otherwise we've rolled into to the top nibble

      // TODO: This is magic that I cannot yet explain.
      H        = 0xF0;
      A        = bufoffset & 0xFF;
      res      = A - H;
      carry    = (A < H); // treating as unsigned
      overflow = ((H ^ A) & (res ^ A)) >> 7;
      L        = res; // truncates

      if (!carry)
        goto draw_attributes; // bitmap copy complete

      buf = ADDRTOBACKBUF((H << 8) | L);

      if (!overflow)
        scr = OFFSETTOSCREEN(SCREENTOOFFSET(scr) - 0x07EE);
      else
        // Start of second half
        scr = ADDRTOSCREEN(0x5011); // (136, 128)
    } else {
      scr = OFFSETTOSCREEN(SCREENTOOFFSET(scr) - 0x07EE);
      buf -= 16;
    }
  }

  {
    u8   A;
    u8   E;
    u8   D;
    u16  DE;
    u8  *HLattrs;
    u16  BCattrs;
    u8   Cattr;

draw_attributes:
    // Attributes
    if (state->dont_draw_screen_attrs)
      return;

    A = state->horizon_table_e34b[1]; // -> horizon table?
    E = state->horizon_table_e34b[2]; // current value?
    state->horizon_table_e34b[2] = A;
    if (E != 0) { // if moved? some sort of previous/current behaviour here
      E = A * 4;
      D = (A * 4 >= 256) ? 0xFF : 0; // was SBC A,A - must be sign extending
      DE = (D << 8) | E;
      HLattrs = ADDRTOSCREEN(state->st.horizon_attribute);
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

      state->st.horizon_attribute = SCREEN_START_ADDRESS + SCREENTOOFFSET(
                                      HLattrs); // create OFFSETTOSCREENADDR?
    }

    /* Draw smash meter attributes */

    if (state->sighted_flag == 0 || state->perp_caught_phase >= PERPCAUGHTPHASE_3)
      return;

    HLattrs = ADDRTOSCREEN(0x5962); // attr (2, 11)

    Cattr = attribute_BLACK_OVER_BRIGHT_RED;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    Cattr = attribute_BLACK_OVER_BRIGHT_MAGENTA;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    Cattr = attribute_BLACK_OVER_BRIGHT_GREEN;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    Cattr = attribute_BLACK_OVER_BRIGHT_WHITE;
    *HLattrs = Cattr; HLattrs += SCREEN_ATTRIBUTES_WIDTH;
    *HLattrs = Cattr;
  }
}

// $BDC1
void clear_playfield_set_attrs(chqstate_t *state)
{
  int      B;
  u8 *screen; // was HL
  u16 DE;

  clear_playfield(state);

  // Clear the playfield pixels to $FF (bug: duplicates work just done)
  memset(ADDRTOSCREEN(0x4800), 0xFF, 0x1000);

  // Clear the playfield attributes to $28 (black over cyan) - first two
  // rows only
  memset(ADDRTOSCREEN(0x5900), 0x28, 2 * 32);

  // Clear the next three rows to $68 (black over bright cyan)
  memset(ADDRTOSCREEN(0x5940), 0x68, 3 * 32);

  // Clear the next 11 rows to the current ground colour
  // Note: Only using the bottom byte of ground_colour (as orig).
  memset(ADDRTOSCREEN(0x59A0), stgbyte(state, 0x5CF4), 0x160); // CHECK

  // Clear the edges of the playfield to black on black
  screen = ADDRTOSCREEN(0x5900);
  DE = 0x1F;
  B = 16;
  do {
    *screen = 0;
    screen += DE;
    *screen++ = 0;
  } while (--B > 0);
}

// $BDFB
void read_map(chqstate_t *state)
{
}

// $C0E1
void prepare_tunnel(chqstate_t *state)
{
}

// $C15B
void draw_tunnel(chqstate_t *state, u8 *IY)
{
}

// $C2E7
void draw_road_scene_change(chqstate_t *state, u8 *IX, u8 *IY)
{
}

// $C452
void draw_road(chqstate_t *state)
{
}

#define BACKDROPWIDTH  (10) // bytes
#define BACKDROPHEIGHT (24) // rows

// $C8BE
void pre_shift_backdrop(chqstate_t *state)
{
  int            tmp;        // for RRD()
  const u8 *source;     // was HL
  u8       *preshifted; // was DE
  const u8 *endptr;     // was DE
  u8       *bmptr;      // was HL
  u8        row;        // was C
  u8        col;        // was B
  u8        pix;        // was A

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
void forked_road_plotter(chqstate_t *state)
{
}

// mystery_cba4 would go here, if we knew what it did

// $CBC5
void backdrop_fill_choice(chqstate_t *state)
{
}

// $CBD6 ish
void build_curve_table(chqstate_t *state, int forked)
{
  u16       *table1, *table2;
  const u8  *road_buffer_ptr_HL; // was HL
  u8         curvature_C; // was C
  int             A;
  int             B;
  const u8  *IY;
  const u16 *IX;
  u8        *DE;
  u16        DEdash;
  u8         curvature_A;
  u16        HLdash;
  u16        BCdash;
  int             carry;
  u16        DEroadpos; // was DE
  u8        *DEe320;
  const u8  *HLe760;
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
void build_curve_table_sub_cca8(chqstate_t *state,
                                u8     Bdash_alwayszero,
                                u16   *HLtableend,
                                u16    DEroadpos)
{
  u8  *IYe300;
  int       Biterations;
  u16 *SPoutput;
  int       A;
  int       Bdash;
  int       Cdash;
  int       Ldash;
  int       Aopcode;
  int       Atotal;
  u16  HLdash;

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
  HLdash = (s8) A;
  HLdash += DEroadpos;
  DEroadpos = HLdash; // was EX
  SPoutput--; *SPoutput = DEroadpos; // PUSH to output table
  goto bct_continue;
}

// $CD3A
void build_height_table(chqstate_t *state)
{
}

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

// $E810
void entrypt_48k(chqstate_t *state)
{
  entrypt_common(state, 0, 3);
}

// $E816
void entrypt_128k(chqstate_t *state)
{
  clear_playfield_attrs(state);
  entrypt_common(state, 1, 5);
}

// $E81D
void entrypt_common(chqstate_t *state, u8 Amode_128k, u8 Bnrelocs)
{
  static const struct Relocations {
    const u8 *src;
    ptrdiff_t      dst;
    size_t         len;
  } relocations[] = {
    { transitions_e88e, offsetof(chqstate_t, transitions_ec00), sizeof(transitions_e88e) },
    { square_transition_mask, 0xEB00, sizeof(square_transition_mask) },
    { diamond_transition_mask, 0xEA00, sizeof(diamond_transition_mask) },
    // { 0xF220, 0x8014, 926 }, // copies load_stage_128k into place
    // { 0xE876, 0x83B5, 24 },  // copies hooks_128k
  };

  const struct Relocations *reloc; // was HL
  u8                   iterations; // was BC

  state->mode_128k = Amode_128k;

  memcpy(ADDRTOSCREEN(SCREEN_START_ADDRESS), marquee_initial,
         sizeof(marquee_initial));
  memcpy(ADDRTOSCREEN(SCREEN_ATTRIBUTES_START_ADDRESS), marquee_attrs,
         sizeof(marquee_attrs));
  if (Amode_128k == 0) {
    // stop_the_tape_48k(state);
  }

  reloc      = &relocations[0];
  iterations =
    3; // Have to ignore requested nrelocs since 128K copies aren't done here
  do
    memcpy((char *) state + reloc->dst, reloc->src, reloc->len);
  while (--iterations > 0);

  bootstrap(state);
}

// $EC2C
void menu_draw_char(chqstate_t *state,
                    u8     Achar, // ASCII
                    u8     Fdash, // dbl height if carry set
                    u8     Cdash, // attribute byte
                    u8    *DEdash, // screen address
                    u8    *HLdash, // attribute address
                    u8   **DEdash_out,
                    u8   **HLdash_out)
{
  const u8 *HLfont;   //
  u8       *DEscreen; //
  u8        Cglyphid; //
  u8       *HLdash_saved; // was B'

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

// $EF00
void bootstrap(chqstate_t *state)
{
  for (;;) {
    int      carry = 0;
    u8 *HL;
    int      B;
    u8  A;
    u8  Aorig;
    u8  C = 0; // Conv: Original doesn't initialise C.

    // Build a table of flipped bytes at $EF00.
    HL = &state->flipped[0];
    do {
      B = 8;
      Aorig = A = HL - &state->flipped[0];
      do {
        RLC(A);
        RR(C);
      } while (--B > 0);
      *HL++ = C;
    } while (Aorig);

    // Conv: Returning here - may have to split this routine up for
    // conversion.
    return;

    // Start attract mode.
    attract_mode_hook(state);

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
    //TODO if (state->mode_128k)
    //TODO   call_bank_3_128k(0xC003);
  }
}

