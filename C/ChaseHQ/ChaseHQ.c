// ChaseHQ.c
//
// Chase H.Q. code model
//
// by dpt

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
// Some code will unavoidably need to be changed however, such as the stack
// trick where PUSH and POP are used to accelerate loads and stores.
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
// (SM) means self modified.
//
// Remember that much of this code is in progress and untested - or just
// broken.
//

// TODO
//
// Get a sprite plotter going.
//
// Stub out all functions.
//
// Import all graphic data.
//
// Copy whole messages that get modified into the state structure.
//
// Decide how to drive the main loop(s).
//

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "../ZXSpectrum/Pixels.h"
#include "../ZXSpectrum/Spectrum.h"
#include "../ZXSpectrum/Z80.h"

#include "Types.h"
#include "ChaseHQ-Data.h"
#include "ChaseHQ-Stages.h"
#include "ChaseHQ-State.h"
#include "ChaseHQ.h"

/* ----------------------------------------------------------------------- */

void chasehq_reset_state(chqstate_t *state)
{
  memset(state, 0, sizeof(*state));

  state->wanted_stage_number = 1;

  state->attract_blinker = 0xF0; // attract mode blinker

  memcpy(&state->sfx_crash_table[0], sfx_crash_table, sizeof(sfx_crash_table));

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

/* ----------------------------------------------------------------------- */

/// Given a road buffer offset return a wrapped-around buffer index.
#define ROADBUFINDEX(N) \
  ((state->road_buffer_offset + (N) - state->road_buffer_start) & 0xFF)

/// Given a road buffer offset return a pointer.
#define ROADBUFPTR(N) \
  (&state->road_buffer_start[ROADBUFINDEX(N)])

/* ----------------------------------------------------------------------- */

// $5C00
void end_screen(chqstate_t *state)
{
}

// $8014 (copied to that position in the original)
// $F220 page_in_stage_128k
void load_stage(chqstate_t *state)
{
  u8 wanted; // was A

  // Return if the stage is already loaded
  wanted = state->wanted_stage_number;
  if (wanted == state->current_stage_number)
    return;

  state->current_stage_number = wanted;

  state->stage = stages[wanted];
}

// $80B9 tape_load_to_5c00

// $81DD start_stage_chatter - was hoisted

// $8204
void setup_engine_sfx_48k(chqstate_t *state)
{
  int nloops;    // was L
  int off_cycle; // was H

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

// $8234
void play_engine_sfx_48k(chqstate_t *state)
{
  u8  phase;   // was A
  int counter; // was A
  int nloops;  // was C
  int c;       // was B

  phase = state->perp_caught_phase;
  if (phase >= PERPCAUGHTPHASE_3)
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

// $8258
void attract_mode_48k(chqstate_t *state)
{
  int       carry = 0;
  u8        blinker;         // was $828C (SM)
  u8        keys;            // was A
  const u8 *messages;        // was HL
  u8        nmessages;       // was B
  u8        attract_blinker; // was A
  u8        style;           // was A

  set_up_stage(state, &state->stage->attract_data);
  blinker = 0;
  state->speed = 400;
  for (;;) {
    keys = keyscan(state);
    if (keys == USERINPUT_FIRE)
      return;

    cpu_driver(state);

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
    draw_screen(state);
  }
}

// $83B5
void start_siren_hook(chqstate_t *state)
{
  // NOP
}

// $83B8
void play_engine_or_siren_sfx_hook(chqstate_t *state)
{
  play_engine_sfx_48k(state);
}

// $83BB
void silence_audio_hook(chqstate_t *state)
{
  // NOP
}

// $83BE
void write_audio_registers_hook(chqstate_t *state)
{
  // NOP
}

// $83C1
void setup_engine_sfx_hook(chqstate_t *state)
{
  // NOP
}

// $83C4
void play_engine_sfx_hook(chqstate_t *state)
{
  setup_engine_sfx_48k(state);
}

// $83C7
void play_speech_hook(chqstate_t *state, u8 A)
{
  // NOP
}

// $83CA
void attract_mode_hook(chqstate_t *state)
{
  attract_mode_48k(state);
}

// $8401
void main_loop(chqstate_t *state)
{
  int carry = 0;
  u8  start_speech_index; // was A
  u8  keys;               // was A
  u8 *pstart_speech;      // was HL
  u8  quit_state;         // was A
  u8  start_speech;       // was A

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
      drive_sfx(state);
      keyscan(state);
      tick(state);
      check_user_input(state);
      read_map(state);
      if (handle_perp_caught(state))
        break; // Conv: Original would POP and goto main_loop to cause a restart
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

      if (state->test_mode) {
        keys = 0; // TODO ~state->speccy->in(state->speccy, port_KEYBOARD_12345) & 0x1F;
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
              escape_scene(state); // exit via
              return;
            }

            state->quit_state = QUITSTATE_DONE;
            setup_transition(state, TRANSITIONSTRIDE_FORWARD);
          }
        }
      }
    }
  }
}

// $852A
void cpu_driver(chqstate_t *state)
{
  const u8 MinSpeed = 150;

  u16 roadpos; // was HL
  u8  input;   // was A

  roadpos = state->scenedata.road_pos;
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

// $858C
void run_pregame_screen(chqstate_t *state)
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

// $85A8
int run_pregame_screen_loop(chqstate_t *state)
{
  draw_pregame(state);
  drive_chatter(state);
  reveal_perp_car(state);
  animate_meters(state);
  transition(state);
  draw_screen(state);
  if (state->transition_control == 0) {
    if (state->chatter_state == CHATTERSTATE_IDLE)
      return 0; // stop
    if (state->chatter_state < CHATTERSTATE_STOP) {
      if (keyscan(state) & USERINPUT_FIRE) {
        drive_chatter_stop(state);
        play_start_noise(state); // exit via
        return 0; // stop
      }
    } else {
      setup_transition(state, TRANSITIONSTRIDE_FORWARD);
    }
  }

  return 1; // loop
}

// $85E4
void reveal_perp_car(chqstate_t *state)
{
  const int MaxHeight = 50;

  u8           revealed_height; // was A
  const lod_t *perp_lod;        // was HL
  u16          width_bytes;     // was DE
  u8           height;          // was B
  const u8    *bitmap;          // was HL

  if (state->wanted_stage_number == MAXSTAGE)
    return; // perp car is hidden on stage 5

  revealed_height = state->pregame_car_revealed_height + 1;
  if (revealed_height > MaxHeight)
    revealed_height--;
  state->pregame_car_revealed_height = revealed_height;

  // Get the largest of the perp car LOD
  perp_lod = state->stage->lods_perp_car;
  width_bytes = perp_lod->width_bytes;
  height      = perp_lod->height;
  if (revealed_height < height)
    height = revealed_height;
  bitmap = perp_lod->bitmap;

  plot_sprite(state,
              width_bytes,
              height,
              ADDRTOBACKBUF(0xF4CD),
              width_bytes,
              bitmap); // exit via
}

// $860F
void animate_meters(chqstate_t *state)
{
  s8 random; // was A
  s8 level;  // was A

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
  am_set_attrs(level, ADDRTOSCREEN(0x5A17));

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
  am_set_attrs(level, ADDRTOSCREEN(0x5A57));
}

// $8646
//
// counter - was A
// attrs - was HL
void am_set_attrs(int counter, u8 *attrs)
{
  int iterations; // was B

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

// $865A
void draw_pregame(chqstate_t *state)
{
  int       carry = 0;
  const u8 *cmds;       // was HL
  u8        cmd;        // was A
  u16       cmdaddr;    // was DE
  u8        tileidx;    // was A
  const u8 *srctile;    // was DE
  u8       *backbuf;    // was HL
  int       tile_count; // was B
  int       iterations; // was B
  u16       bufoffset;  // was BC
  u8        E;          // was E
  u8        rows;       // was ?
  u8        bgattr;     // was A
  const u8 *messages;   // was HL
  u16       attrs;      // was DE

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
      srctile -= 8; // was POP
    } while (--tile_count > 0);
    cmdaddr = 0xF000 + bufoffset; // was EX DE,HL ; #REGde = Back buffer ptr
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

// $873C
void escape_scene(chqstate_t *state)
{
  silence_audio_hook(state);
  set_up_stage(state, &escape_scene_data);
  state->speed = 250; // speed of camera
  memcpy(&state->hazards[0], &escape_scene_perp, sizeof(escape_scene_perp));
  state->hazards[0].lod_addr = state->stage->lods_perp_car;
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
    draw_hazards(state);
    draw_everything_else(state);
    update_scoreboard(state);
    drive_chatter(state);
    transition(state);
    draw_screen(state);

    // Loop unless the tunnel has appeared - and is right size?
    if (state->SM_C161 == 0 || state->SM_C15E >= 7)
      continue;

    // Activate the three barriers once close enough
    if (state->hazards[0].distance == 5)
      state->hazards[1].TBD7 =
        state->hazards[2].TBD7 =
          state->hazards[3].TBD7 = 0xFF;

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

// $87DC
void set_up_stage(chqstate_t        *state,
                  const scenedata_t *scene_data)
{
  u8  iterations;   // was B

  memset(&state->road_buffer[0], 0, 256);
  state->st         = saved_game_state;
  state->hazards[0] = saved_game_state_hazard_0;
  memset(&state->hazards[1], 0, sizeof(hazard_t) * (MAXHAZARDS - 1));

  state->scenedata = *scene_data;

  pre_shift_backdrop(state);

  // Set backdrop position in horizon table (used to draw attributes)
  state->horizon_table_e34b[0] = 8;
  state->horizon_table_e34b[1] =
    0; // initialised strangely, presumed to be zero (needs checking)
  state->horizon_table_e34b[2] = 0;

  // Disable the helicopter and tunnel drawing calls in draw_everything_else
  state->dee_draw_tunnel_1 = 0; // draw tunnel call
  state->dee_draw_helicopter = 0; // draw heli call
  state->dee_draw_tunnel_2 = 0; // draw tunnel call

  state->SM_C058 = 0; // clear current hazard?
  state->SM_B063 = 0; // clear jump counter?

  state->hazards[0].lod_addr = state->stage->lods_perp_car;

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
  set_up_stage_reset_lights(ADDRTOSCREEN(0x5820));
  set_up_stage_reset_lights(ADDRTOSCREEN(0x583B));

  silence_audio_hook(state);
  update_scoreboard(state); // exit via
}

// $8860 (pulled out of set_up_stage above)
//
// attrptr - was HL
void set_up_stage_reset_lights(u8 *attrptr)
{
  int rows; // was C
  int cols; // was B

  rows = MARQUEELIGHT_HEIGHT;
  do {
    cols = MARQUEELIGHT_WIDTH;
    do
      *attrptr++ &= ~ATTR_BRIGHT;
    while (--cols > 0);
    attrptr += SCREEN_ATTRIBUTES_ROWBYTES - MARQUEELIGHT_WIDTH;
  } while (--rows > 0);
}

// $8876
void check_user_input(chqstate_t *state)
{
  u8  transctl;   // was A
  u8 *puserinput; // was HL
  u8  input;      // was A
  u8 *pboost;     // was HL
  u8  keys;       // was A

  transctl = state->transition_control;
  puserinput = &state->user_input;
  if (transctl != TRANSITIONCONTROL_FADE) {
    *puserinput = USERINPUT_NONE;
    return;
  }

  *puserinput = input = (state->st.user_input_mask & *puserinput);
  if ((input & (USERINPUT_QUIT | USERINPUT_PAUSE | USERINPUT_TURBO)) == 0)
    return;

  if (input & USERINPUT_QUIT) {
    check_user_input_quit_key(state);
    return;
  }

  if ((input & USERINPUT_PAUSE) == 0) {
    // (If it's not pause it's...) Turbo pressed
    pboost = &state->boost;
    if (*pboost > 0 || state->st.turbos == 0)
      return; // already boosting or no turbos remain

    *pboost = 60; // set 60 ticks of boost

    start_chatter(state, 2, &chatterblk_turbo[0]);
    setup_engine_sfx_hook(state); // exit via
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

// $88A9
void check_user_input_quit_key(chqstate_t *state)
{
  if (state->quit_state != QUITSTATE_IDLE)
    return;

  drive_chatter_stop(state);
  fill_attributes(state);

  state->st.user_input_mask = USERINPUTMASK_ALLOW_NONE;
  state->quit_state         = QUITSTATE_START;
}

// $88D5
void clear_playfield_attrs(chqstate_t *state)
{
  memset(ADDRTOSCREEN(0x5900),
         attribute_BLACK_OVER_BLACK,
         SCREEN_ATTRIBUTES_ROWBYTES * PLAYFIELD_HEIGHT / 8);
}

// $88E2
void clear_playfield(chqstate_t *state)
{
  clear_playfield_attrs(state);
  memset(ADDRTOSCREEN(0x4800),
         ________,
         SCREEN_BITMAP_ROWBYTES * PLAYFIELD_HEIGHT);
}

// $88F2
//
// index - was B
// priority - was C
void start_sfx(chqstate_t *state, u8 index, u8 priority)
{
  u8 curr_priority; // was A

  curr_priority = state->sfx_priority;
  if (curr_priority == 0 || curr_priority >= priority) {
    state->sfx_index    = index;
    state->sfx_priority = priority;
  }
}

// $8903
void drive_sfx(chqstate_t *state)
{
  // $893C
  static const struct sfxtab {
    u8     arg1;
    u8     arg2;
    void (*handler)(chqstate_t *state, u8 arg1, u8 arg2);
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

  int                  index; // was A
  const struct sfxtab *tab;   // was HL

  if (state->tunnel_sfx == 0) {
    state->var_a23d |= state->var_a23c;
    if (state->var_a23d)
      start_sfx(state, EFFECT_CORNERING, 4); /* priority 4 */
  }

  play_engine_sfx_hook(state);
  play_engine_or_siren_sfx_hook(state);
  write_audio_registers_hook(state);

  if (state->sfx_index == 0)
    return;

  state->sfx_index    = 0;
  state->sfx_priority = 0;

  tab = &sfx_table[index - 1];
  tab->handler(state, tab->arg1, tab->arg2);
}

// $8960
//
// param1 - was D
// param2 - was E
void sfx_crash(chqstate_t *state, u8 param1, u8 param2)
{
  int  carry = 0;
  u8  *tab; // was HL
  int  C;   // was C
  int  B;   // was B
  int  A;   // was A

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

// $89D9
//
// param1 - was D
// param2 - was E
void sfx_thud(chqstate_t *state, u8 param1, u8 param2)
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
  u8        A;
  u8        B;
  u8        E;

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

// $8A0F
//
// param1 - was D
// param2 - was E
void sfx_cornering(chqstate_t *state, u8 param1, u8 param2)
{
  u8 A;

  A = state->SM_8A0F ^ 1;
  state->SM_8A0F = A;
  if (A)
    return;

  sfx_cornering_loop_outer(state, param1, param2); // was fallthrough
}

void sfx_cornering_loop_outer(chqstate_t *state, u8 param1, u8 param2)
{
  u8 C;
  u8 B;

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

// $8A36
//
// param1 - was D
// param2 - was E
void sfx_bipbow(chqstate_t *state, u8 param1, u8 param2)
{
  u8 C;
  u8 H;
  u8 L;
  u8 B;

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

// $8A57
int handle_perp_caught(chqstate_t *state)
{
  int       carry = 0;
  int       zero  = 0;
  u8        phase;       // was A
  u8        car_y;       // was A
  u8        fastcounter; // was A
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
  switch (phase) {
  case PERPCAUGHTPHASE_0:
    return 0;
  case PERPCAUGHTPHASE_1:
    goto move_perp;
  case PERPCAUGHTPHASE_2:
    goto phase2;
  case PERPCAUGHTPHASE_3:
    goto phase3;
  case PERPCAUGHTPHASE_4:
    goto phase4;
  default:
    break;
  }

  // Otherwise 5/6
  if (state->transition_control)
    return 0;
  if (phase == PERPCAUGHTPHASE_5)
    goto phase5;

  // Must be 6
  silence_audio_hook(state);
  state->wanted_stage_number++;
  return 1; // Conv: signal to bypass remainder of main loop

phase5:
  state->perp_caught_phase = PERPCAUGHTPHASE_6;
  setup_transition(state, TRANSITIONSTRIDE_FORWARD); // was exit via
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
  fill_attributes(state); // exit via
  return 0;

phase3:
  A = state->handle_perp_caught_delay - 1;
  state->handle_perp_caught_delay = A;
  if (A)
    return 0;

  state->perp_caught_phase = PERPCAUGHTPHASE_4;
  HLmessages = state->stage->addrof_arrest_messages;
  setup_overlay_messages_with_transition(state,
                                         TRANSITIONCONTROL_DRAW_MUGSHOTS,
                                         HLmessages); // was exit via
  return 0;

phase4:
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

  HLscore--;
  *HLscore |= STREND;

  setup_overlay_messages(state, &state->score_messages[0]); // was exit via
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

assign_perp_pos:
  A = C;
  state->hazards[0].horz_pos = A;
  HLroadpos = state->scenedata.road_pos;
  // PUSH HLroadpos
  carry = (HLroadpos < ROAD_LEFTMOST); // was SUB
  // POP HLroadpos
  Ainput = USERINPUT_UP | USERINPUT_RIGHT;
  if (!carry)
    goto assign_hero_pos;

  HLroadpos -= ROAD_RIGHTMOST;
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

  HLphc = &state->st.perp_halt_counter;
  (*HLphc)--;
  if (*HLphc)
    goto perp_too_far_away;

  state->speed = 0;
  state->hazards[0].TBD4 = 0;
  state->hazards[0].distance = 1;
  state->perp_caught_phase = PERPCAUGHTPHASE_2;
  state->smoke = 3;
  DEspeed = 0; // Conv
  goto set_perp_speed;

perp_too_far_away:
  Aperpdistance = state->hazards[0].distance;
  HLspeed = 350;
  if (Aperpdistance < 15) {
    Biterations = 16 - Aperpdistance;
    do
      HLspeed -= 20;
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
    goto set_perp_speed; // with DE=70
  }
  HLspeed -= 5;
  DEspeed = HLspeed;

set_perp_speed:
  hpc_set_perp_speed(state, DEspeed); // was fallthrough
  return 0;
}

// $8C35
//
// speed - was DE
void hpc_set_perp_speed(chqstate_t *state, u16 speed)
{
  state->hazards[0].speed = speed;
}

// $8C3A
void fully_smashed(chqstate_t *state)
{
  state->perp_caught_phase  = PERPCAUGHTPHASE_1;
  state->hand_flag          = 2; // TODO: Add a symbol for this
  state->smash_counter      = 20;
  state->st.user_input_mask = USERINPUT_PAUSE | USERINPUT_QUIT;
  setup_overlay_messages(state, &pull_over_message[0]);
  hpc_set_perp_speed(state, 400);
}

// $8D8F
void transition(chqstate_t *state)
{
  int       iterations; // was B'
  u16       backbuf;    // was HL
  const u8 *maskptr;    // was HL'
  u16       screencopy; // was D
  u8        mask;       // was E

  switch (state->transition_control) {
  case TRANSITIONCONTROL_STOP:
    return;
  case TRANSITIONCONTROL_DRAW_MUGSHOTS:
    draw_mugshots(state);
    return;
  case TRANSITIONCONTROL_OVERLAY_MESSAGES:
    draw_overlay_messages(state);
    return;
  case TRANSITIONCONTROL_FILL_ATTRIBUTES:
    fill_attributes(state);
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

  backbuf  = 0xFF00; // was H=$FF
  maskptr = state->transition_mask;
  iterations = 8;
  do {
    mask = *maskptr;
    screencopy = backbuf; // Conv: Original just saved H in D
    backbuf = (backbuf & 0xFF00) | 0xFE;
    transition_fade_chunk(state, mask, ADDRTOBACKBUF(backbuf));
    backbuf -= 8 << 8;
    transition_fade_chunk(state, mask, ADDRTOBACKBUF(backbuf));
    backbuf = screencopy - 256; // restore
    maskptr++;
  } while (--iterations > 0);
}

// $8DD8
// Overwrite odd/even UDG rows of the back buffer with a single byte.
//
// mask - was E
// backbuf - was HL
void transition_fade_chunk(chqstate_t *state, u8 mask, u8 *backbuf)
{
  int rows;       // was C
  int iterations; // was B

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

// $8DF9
//
// stride - was A
void setup_transition(chqstate_t *state, u8 stride)
{
  s16                 frame_stride; // was BC
  const transition_t *transitions;  // was DE
  const transition_t *transition;   // was HL

  assert(stride == 8 || (s8) stride == -8);

  frame_stride = stride;
  // Conv: Points at non-relocated table.
  transitions = &transitions_e88e[0];
  if ((s8) stride < 0) { // reversed
    frame_stride |= 0xFF00; // widen -8 to 16 bits
    transitions = &transitions_e88e[4]; // second half of table
  }

  state->transition_frame_stride = frame_stride;

  // Pick a random entry in the table
  transition = &transitions[rng(state) & 3];

  state->transition_nframes = transition->nframes;
  state->transition_mask    = transition->maskbase;
  state->transition_control = TRANSITIONCONTROL_FADE;
}

// $8E29
void fill_attributes(chqstate_t *state)
{
  u8 *src;     // was HL
  u8 *dst;     // was DE
  int rows;    // was A
  int columns; // was BC

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

  state->transition_control = TRANSITIONCONTROL_STOP;
}

// $8E42
void draw_overlay_messages(chqstate_t *state)
{
  const u8 *message;    // was HL
  u8        iterations; // was B
  u8        delay;      // was A
  u8        style;      // was A

  message    = state->overlay_message;
  iterations = state->overlay_count;
  for (;;) {
    if (--iterations == 0) {
      delay = state->overlay_delay - 1;
      state->overlay_delay = delay;
      if (delay)
        return;

      state->overlay_delay = *message; // set new delay
      state->overlay_count++;
      iterations++;
    }

    style = *++message;
    if (style == DRAWCHARSTYLE_DUNNO) // or possibly a special marker?
      break;

    print_message(state, style, message);
  }

  state->transition_control = message[-1];
}

// $8E6C
//
// style - was A
// messages - was HL
const u8 *print_message(chqstate_t *state,
                        u8          style,
                        const u8   *messages)
{
  u8  attr;     // was A
  u16 backbuf;  // was DE
  u16 attraddr; // was BC

  // we ignore flags in messages[0]
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

// $8E7E
//
// message - was HL
void setup_overlay_messages(chqstate_t *state, const u8 *message)
{
  setup_overlay_messages_with_transition(state,
                                         TRANSITIONCONTROL_OVERLAY_MESSAGES,
                                         message);
}

// $8E80
//
// transition - was A
// message - was HL
void setup_overlay_messages_with_transition(chqstate_t *state,
    u8          transition,
    const u8   *message)
{
  state->transition_control = transition;
  state->overlay_delay      = *message++;
  state->overlay_message    = message;
  state->overlay_count      = 1;
}

// $8E91
void draw_mugshots(chqstate_t *state)
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

// Returns the previous row for the back buffer (visually upwards).
//
// Back buffer addresses are of the form 0b_1111_LLLL_RRRC_CCCC
//
// Conv: Extracted to function.
static u16 prevbufrow(u16 backbuf)
{
  int orig;

  assert(backbuf >= BACKBUFFER_START_ADDRESS);

  orig = backbuf;
  backbuf -= 256;
  if ((orig & 0x0F00) == 0) { // LLLL was zero on entry
    backbuf += 0x1000; // 1110 -> 1111
    int t = (backbuf & 0xFF) - 32; // decrement RRR
    backbuf = (backbuf & 0xFF00) | (t & 0xFF);
    if (t < 0) // did carry - unsure if happens in practice
      backbuf -= 0x1000;
  }

  assert(backbuf >= BACKBUFFER_START_ADDRESS);
  return backbuf;
}

// $8EB7
//
// screenpos - was BC
// backbuf - was DE
// mugshot - was HL
void draw_mugshot(chqstate_t *state,
                  u16         screenpos,
                  u16         backbuf,
                  const u8   *mugshot)
{
  const u8 *orig_mugshot; // was PUSH-POP
  u16       counter;      // was BC

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

  plot_face_attributes(state, screenpos, orig_mugshot); // exit via
}

// $8EE7
void draw_smash_bar(chqstate_t *state)
{
  const int MaxSegments    = 20;
  const int SegmentHeight  = 3;
  const int BorderHeight   = 2;
  const int TotalBarHeight = MaxSegments * SegmentHeight * BorderHeight * 2;

  u16 buf;        // was HL
  u8  nsmashsegs; // was A
  u8  nsolid;     // was B

  if (state->sighted_flag == 0)
    return; // Return if the perp has not yet been sighted

  if (state->perp_caught_phase >= PERPCAUGHTPHASE_3)
    return; // Return if perp_caught_phase is >= 3 (car has stopped)

  buf = 0xF7A2; // Back buffer address of bottom of bar
  // Conv: D & E moved into prevbufrow forward

  // Draws bottom two rows
  buf = draw_smash_bar_solid_bit(state, BorderHeight, buf);

  nsmashsegs = state->smash_counter;
  if (nsmashsegs > 0)
    buf = draw_smash_bar_segments(state, nsmashsegs, buf);

  nsolid = TotalBarHeight - BorderHeight - nsmashsegs *
           SegmentHeight; // Number of solid rows to draw at the top
  (void) draw_smash_bar_solid_bit(state, nsolid, buf); // exit via
}

// $8F13
//
// nsegs - was C
// buf - was HL
u16 draw_smash_bar_segments(chqstate_t *state, int nsegs, u16 buf)
{
  do {
    *ADDRTOBACKBUF(buf) = X______X; // Set 8 pixels
    buf = prevbufrow(buf);
    *ADDRTOBACKBUF(buf) = X______X; // Set 8 pixels
    buf = prevbufrow(buf);
    buf = draw_smash_bar_solid_bit(state, 1, buf); // 1 row gap
  } while (--nsegs > 0);
  return buf;
}

// $8F47
//
// nrows - was B
// buf - was HL
u16 draw_smash_bar_solid_bit(chqstate_t *state, int nrows, u16 buf)
{
  do {
    *ADDRTOBACKBUF(buf) = XXXXXXXX; // Set 8 pixels
    buf = prevbufrow(buf);
  } while (--nrows > 0);
  return buf;
}

// $8F5F
void draw_everything_else(chqstate_t *state)
{
  u8          *HLtable;        // was HL
  u8          *DEtable;        // was DE
  int          iterations;     // was B
  u8          *IY;             // was IY
  u8          *roadbuf;        // was HL
  u16         *IX;             // was IX
  u8           floating_arrow; // was A
  u8           A;              // was A
  const lod_t *arrow_defn;     // was HL
  u8           Ex;             // was E
  u8           Dy;             // was D
  u8           Cwidth;         // was C
  u8           Bflags;         // was B
  u8           Edash;          // was E
  u8           Cdash;          // was C
  u8           Bheight;        // was B
  const u8    *HLbitmap;       // was HL
  u8           E;              // was E
  const obj_t *HLobj;          // was HL

  state->SM_A9E2 = &state->table_ed00[20];
  state->SM_AECF = &state->table_e900[0];

  HLtable = &state->table_e300[1]; // table of objects?
  DEtable = &state->table_e336[0];
  iterations = 21;
  do {
    *HLtable++ += 32;
    *DEtable++ += 32;
  } while (--iterations > 0);

  IY = &state->table_e300[21];
  if (state->dee_draw_tunnel_1)
    draw_tunnel(state, IY);
  IY--;

  roadbuf = ROADBUFPTR(115);

  IX = &state->table_ea00[88];
  iterations = 20; // iterations
  do {
    if (state->n_hazards)
      dh_aecf(state);

    dust_stones_stuff(state);

    if (state->dee_draw_helicopter)
      draw_helicopter(state);

    if (state->dee_draw_tunnel_2)
      draw_tunnel(state, IY);

    A = *HLtable; // fetch (object?) from right hand side
    if (A)
      goto right_hand_stuff;
continue_after_right_hand_done:
    IX += 2;
    HLtable += 32; // FIXME needs to wrap?

    A = *HLtable; // fetch (object?) from left hand side
    if (A)
      goto left_hand_stuff;
continue_after_left_hand_done:
    IX += 2;
    HLtable -= 33; // FIXME needs to wrap

    IY--;
  } while (--iterations > 0);

  if (state->dee_draw_helicopter)
    return;

  floating_arrow = state->floating_arrow;
  if (floating_arrow == 0)
    return;

  // Draw the floating arrow
  arrow_defn = &floating_arrow_left_defn;
  Ex = 120; // horz pos
  if (floating_arrow != 1) {
    arrow_defn = &floating_arrow_right_defn;
    Ex = 128;
  }
  Dy       = 48; // vert pos
  Cwidth   = arrow_defn->width_bytes;
  Bflags   = arrow_defn->flags >> 1; // goes in B'
  Edash    = Cdash;
  Cdash    = 0; // this must be passed in
  Bheight  = arrow_defn->height;
  HLbitmap = arrow_defn->bitmap;
  draw_part_entry2(state, Bheight, Cwidth, Dy, Ex, HLbitmap, Bflags); // exit via
  return;

right_hand_stuff:
  E = A;
  if (IX[1]) // buffer offset/distance
    goto continue_after_right_hand_done;

  HLobj = &state->stage->addrof_right_hand_objects[E];
  HLobj->handler(state, Bheight, HLobj->arg, IX, IY);
  goto continue_after_right_hand_done;

left_hand_stuff:
  E = A;
  if (A != 2 && IX[1])
    goto continue_after_left_hand_done;

  HLobj = &state->stage->addrof_left_hand_objects[E];
  HLobj->handler(state, Bheight, HLobj->arg, IX, IY);
  goto continue_after_left_hand_done;
}

// $9052
//
// B -
// C -
// DElod -
// IX -
void draw_overhead(chqstate_t  *state,
                   u8           B,
                   u8           C,
                   const lod_t *DElod,
                   u8          *IX)
{
#if 0
  int          carry = 0;
  const lod_t *HLlod;      // was HL
  u16          DE;
  u8           counter;    // was A
  int          iterations; // was B
  u8          *HLdst;
  u8          *DEsrc;
  u8 A;
  const u8 *HL;
  u8 D, E, H, L;
  u16 IY;

  // PUSH IX/DE/BC
  if (IX[1] == 0) // buffer offset/distance
    draw_stretchy_object_left(state, DElod);
  // POP BC/HL/IX

  HLlod = DElod; // e.g. $6F26 in Stage 3's data, loads $6F2D
  HLlod++;
  DE = wordat(HLlod) + 2;

  counter = state->fast_counter & 0xE0;

  // Scale down pattern.
  counter = (counter - (counter >> 2) - (counter >> 4));
  HL = &vertical_e600[counter / 22][B];

  C = IY[0] - IY[0x35];
  A = *HL;
  L = A;
  SRL(A);
  A = A + L - C;
  state->SM_90F1 = A; // Self modify 'SUB x' at $90F1
  A = B - 1;
  if (A >= 10)
    A = 9;

  B = A;
  HL = DE + A * 2;
  C = *HL;
  HL = DE + B * 3 + 20;

  D = 1;
  A = IX[1]; // buffer offset/distance
  if (A < 0)
    goto do_90c4;
  if (A)
    return;

  A = IX[0] + 24 - C;
  if (carry)
    goto do_90c4;
  A -= 8;
  if (carry)
    goto do_90c4;
  if (A < 8)
    goto do_90c4;

  D = A >> 3;

do_90c4:
  IX -= 2;
  E = 0x1F;
  A = IX[1]; // buffer offset/distance
  if (A < 0)
    return;
  if (A)
    goto do_90e4;

  A = IX[0] + C;
  if (carry)
    goto do_90e4;
  if (A == 0)
    goto do_90e4;
  if (A >= 0xF7) // -8
    goto do_90e4;

  E = A >> 3;

do_90e4:
  C = D;
  state->SM_9115 = ~((E - C) * 2) + 61;

  A = IY[0x35] - state->SM_90F1;
  if (A < 0)
    return;

  // PUSH AF
  A++;
  B = *HL;
  A -= B;
  if (carry) {
    A += B;
    B = A;
  }
  HL++;
  DEsrc = wordat(HL); // load bitmap?
  // POP AF

  // Build dst
  H = (L & 0x0F) + 0xF0;
  L = (L & 0x70) * 2 + C;
  goto do_draw;

do_continue:
  DEsrc++;
  if (--iterations == 0)
    return;

do_draw: // draws a span
  memset(HLdst, *DEsrc, state->SM_9115 / 2);
  HLdst = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(HLdst)));

  goto do_continue;
#endif
}

// $916C
void draw_stretchy_object_left(chqstate_t *state,
                               u8          B,
                               const void *arg,
                               const u16  *IX,
                               const u8   *IY)
{
  draw_stretchy_object_common(state,
                              B,
                              arg,
                              draw_object_left_stretchy_entrypt,
                              IX,
                              IY);
}

// $9171
void draw_stretchy_object_right(chqstate_t *state,
                                u8          B,
                                const void *arg,
                                const u16  *IX,
                                const u8   *IY)
{
  draw_stretchy_object_common(state,
                              B,
                              arg,
                              draw_object_right_stretchy_entrypt,
                              IX,
                              IY);
}

// $9174
//
// B - was B
// DEarg - was DE
// HLcallback - was HL
void draw_stretchy_object_common(chqstate_t     *state,
                                 u8              B,
                                 const void     *DEarg, // lod?
                                 dso_callback_t *HLcallback,
                                 const u16      *IX,
                                 const u8       *IY)
{
#if 0
  dso_callback_t *SM_91CD = HLcallback;
  dso_callback_t *SM_9244 = HLcallback; // probably don't need these

  int       carry = 0;
  u8        counter; // was A
  const u8 *HL;
  u8        A;
  u8        SM_91DB;
  u16       DE;
  u8        C;
  u16       SM_91BA;
  u16       BC;

  counter = state->fast_counter & 0xE0;
  counter = (counter - (counter >> 2) - (counter >> 4));
  HL = &vertical_e600[counter / 22][0];
  A = *HL;
  SM_91DB = A;
  // EX DE,HL
  A = B;
  if (A >= DEPTHSET_MAX)
    A = DEPTHSET_MAX;
  DE = A * 2 - 1;
  C = 0;
  SM_91BA = DE;

dso_loop:
  A = -C;
  state->SM_933D = A;
  B = *HL - 1;
  if (B == 0)
    return;
  HL++;
  DE = wordat(HL);
  HL += 2;
  // PUSH HL/IX/BC
  // EX DE,HL
  DE = wordat(HL); // read LOD ptr
  BC = SM_91BA;
  HL += BC;
  A = *HL++;
  HL = *HL;
  HL += DE;
  // POP BC
  if (--B)
    goto dso_dispatch;

  // Zero path
  B = *HL - 2;
  // PUSH BC
  B = A;
  SM_91CD(state, B, HL, IX);

dso_loop_perhaps:
  // POP BC
  C += B;
  // POP IX/HL
  goto dso_loop;

dso_dispatch:
  // EX AF,AF'
  A = SM_91DB;
  // Dispatch ladder
  if (--B == 0) goto dso_case_150pc; // jump if 3
  if (--B == 0) goto dso_case_50pc;  // jump if 4
  if (--B == 0) goto dso_case_113pc; // jump if 5
  if (--B == 0) goto dso_case_38pc;  // jump if 6
  if (--B == 0) goto dso_case_75pc;  // jump if 7
  if (--B == 0) goto dso_case_25pc;  // jump if 8
  if (--B == 0) goto dso_continue;   // jump if 9
  A += A;
  goto dso_continue;

dso_case_25pc:
  // Scale A to 25%
  A >>= 2;
  goto dso_continue;

dso_case_75pc:
  // Scale A to 75%
  A = (A >> 1) + (A >> 2);
  goto dso_continue;

dso_case_38pc:
  // Scale A to 37.5%
  A = (A >> 1) - (A >> 3);
  goto dso_continue;

dso_case_113pc:
  // Scale A to 112.5%
  A += (A >> 3);
  goto dso_continue;

dso_case_50pc:
  // Scale A to 50%
  A = (A >> 1);
  goto dso_continue;

dso_case_150pc:
  // Scale A to 150%
  A += (A >> 1);

dso_continue:
  A -= C;
  if (A == 0)
    goto dso_9224;
  if (!carry)
    goto dso_9226;

dso_9224:
  A = 1;

dso_9226:
  B = A;
  // PUSH BC
  A = IY[53] + 1 - C - B;
  if (!carry)
    goto dso_9232;
  A += B;
  B = A;

dso_9232:
  state->SM_9404 = B;
  state->SM_9415 = *HL - 2;
  state->SM_93C0 = 2;
  // EX AF,AF'
  B = A;
  SM_9244(state, B, HL, IX);
  state->SM_93C0 = 0;
  goto dso_loop_perhaps;
#endif
}

// $924D
void draw_tunnel_light_left(chqstate_t  *state,
                            u8          B,
                            const void *DEarg,
                            const u16  *IX,
                            const u8   *IY)
{
  draw_tunnel_light_common(state, B, DEarg, draw_object_left_entrypt, IX);
}

// $9252
void draw_tunnel_light_right(chqstate_t *state,
                             u8          B,
                             const void *DEarg,
                             const u16  *IX,
                             const u8   *IY)
{
  draw_tunnel_light_common(state, B, DEarg, draw_object_right_entrypt, IX);
}

void draw_tunnel_light_common(chqstate_t            *state,
                              u8                     B,
                              const depthset_t      *DElight,
                              draw_object_entrypt_t *HLcallback,
                              const u16             *IX)
{
  u8 counter; // was A
  u8 A;

  if (B >= 16)
    return;

  counter = state->fast_counter & 0xE0;
  counter = counter - (counter >> 2) - (counter >> 4);
  A = vertical_e600[counter / 22][B];
  A = (A >> 2) - A;

  // callback must need to take A
  HLcallback(state, A, B, DElight, IX); // e.g. calls draw_object_left_entrypt
}

// $9278
void draw_object_left(chqstate_t *state,
                      u8          B,
                      const void *DEarg, // a depthset_t *
                      const u16  *IX,
                      const u8   *IY)
{
  draw_object_left_entrypt(state, 0, B, DEarg, IX);
}


// DEarg is e.g. turn_sign_left (a depthset_t)
void draw_object_left_entrypt(chqstate_t       *state,
                              u8                A,
                              u8                B,
                              const depthset_t *DEarg,
                              const u16        *IX)
{
  const depthset_t *ds;    // was HL
  const lod_t      *lods;  // was DE
  u8                depth; // was B
  const lod_t      *lod;   // was HL

  state->SM_933D = A;

  if (B >= DEPTHSET_MAX)
    B = DEPTHSET_MAX;

  ds = DEarg; // EX DE,HL - save arg address

  lods  = ds->lods; // loads address of e.g. turn_sign_lods
  depth = ds->pairs[B].depth;
  lod   = &lods[ds->pairs[B].offset / 7];

  draw_object_left_stretchy_entrypt(state, depth, lod, IX);
}

void draw_object_left_stretchy_entrypt(chqstate_t *state, u8 B, const lod_t *HL,
                                       const u16 *IX)
{
  u8 A;

  A = IX[0] + 16 - B;
  if ((s8) A < 0) // carry?
    return;

  draw_object_left_helicopter_entrypt(state, A, HL);
}

void draw_object_left_helicopter_entrypt(chqstate_t *state, u8 A,
    const lod_t *HLlod)
{
#if 0
  int carry = 0;
  u8  C;
  u8  E;
  u16 BC;
  u8  B;
  u8  D;

  if (A < 8)
    return;

  C = 0;
  E = HLlod->width_bytes << 3;
  A -= E;
  if (!carry) {
    if (A >= 8) {
      draw_object_930e_entrypt(state); // exit via
      return;
    }

    E = HLlod->width_bytes;
    A >>= 2;
    state->SM_9396 = A;
    A = E - 1;
    BC = 0x0101;
  } else {
    E = HLlod->width_bytes;
    A = (A & 0xFC) >> 2;
    state->SM_9396 = A;
    RR(A);
    B = A;
    A += E - 33;
    if (carry || A == 0)
      return;

    D = A;
    C = E - A;
    A = D;
    B = 1;
  }

  D = HLlod->flags >> 1; // checking LODFLAG_FLIPPED?
  if (D == 0) { // was JP Z - check
    draw_object_9333(state, carry, C, E, HLlod, IY); // exit via
    return;
  }

  B--;
  A++;
  C = 0;
  // EX AF,AF'
  draw_object_common(state, A, HLlod);
#endif
}

// $92E1
void draw_object_right(chqstate_t *state,
                       u8          B,
                       const void *DEarg,
                       const u16  *IX,
                       const u8   *IY)
{
  draw_object_right_entrypt(state, 0, B, DEarg, IX);
}

void draw_object_right_entrypt(chqstate_t      *state,
                               u8                A,
                               u8                B,
                               const depthset_t *DEarg,
                               const u16        *IX)
{
}

void draw_object_right_stretchy_entrypt(chqstate_t *state, u8 B,
                                        const lod_t *HL, const u16 *IX)
{
}

void draw_object_right_helicopter_entrypt(chqstate_t *state, u8 A,
    const lod_t *HLlod)
{
}

void draw_object_930e_entrypt(chqstate_t *state)
{
}

void draw_object_common(chqstate_t *state, u8 A, const lod_t *HLlod)
{
#if 0
  int carry = 0;
  u8  Adash;

  A = ~state->SM_9396;
  state->SM_9396 = A;

  Adash = A; // EX AF,AF'

  draw_object_9333(state, carry, C, E, HLlod, IY);
#endif
}

void draw_object_9333(chqstate_t *state, int carry, u8 C, u8 E, u8 *HL, u8 *IY)
{
#if 0
  u8 D;
  u8 A;

  if (carry) {
    // EX AF,AF'
    SLA(C);
    SLA(E);
    // EX AF,AF'
  }

  // EX AF,AF'
_933d:
  D = state->SM_933D;
  A = IY[0] - IY[53];
  if (A)
    goto _9359;

_9347:
  A = IY[53] + D;
  if (M)
    return;
  // PUSH AF
  A++;
  D = *HL;
  A -= D;
  if (carry)
    _9353;
  A = 0;

_9353:
  A += D;
  HL++;
  D = 1;
  goto _9390;

_9359:
  if ((D & (1 << 7)) == 0)
    A -= D;
  else
    A += D;

  D = A;
  if ((s8) A <= 0)
    goto _9347;

  A = IY[53];
  // PUSH AF
  A = *HL - 1 - D;
  if (!carry)
    goto _938d;
  // POP AF
  A = state->SM_93C0; // set to 0 or 2
  if (A == 0)
    return;

  A = state->SM_933D;
  D = *HL;
  A -= D;
  if (P)
    return;
  state->SM_933D = A;
  A = state->SM_9404 - D;
  if (C || Z)
    return;
  state->SM_9404 = A;
  goto _933d;

_938d:
  A++;
  D++;
  HL++;

_9390:
  // PUSH BC,DE
  D = A;
  B = 0;
  A = state->SM_9395;
  RR(A);
  if (!carry)
    goto _939c;
  HL += 2;
_939c:
  HL += BC;
  state->SM_9412 = HL;
  C = E;
  // POP AF
  A--;
  if (A) { // multiplier
    do
      HL += BC;
    while (--A > 0);
  }

  B = D;
  D = 0;
  // EXX
  // POP BC,AF
  D = A;
  H = (A & 0x0F) + 0xF0;
  A = D;
  L = (A & 0x70) * 2 + B;
  A = state->SM_93C0; // set to 0 or 2
  if (A == 0)
    goto _93d0;
  A--;
  if (A)
    goto _93df;
  // EX AF,AF'
  if (carry)
    goto plot_masked_sprite_variant; // exit via
  goto _9479;

_93d0:
  // EX AF,AF'
  if (Z) {
    if (carry)
      draw_part_entry3(...); // exit via
    else
      plot_sprite(...); // exit via
  } else {
    if (carry)
      plot_masked_sprite_flipped(...); // exit via
    else
      plot_sprite_flipped(...); // exit via
  }
  return;

_93df:
  // EX AF,AF
  if (carry)
    goto _9436;
  SRL(A);
  if (carry)
    goto _9420;

  IX = 0x94C8; // base of jump table in plot_sprite...
  A = 4 - A;
  IX += A * 5;
  BC = plot_sprite_even_entry;

do_set_callbacks:
  state->SM_940F = BC;
  state->SM_941D = BC;
  // EXX

_9404:
  A = state->SM_9404 - B;
  if (A == 0 || carry)
    goto _941a;
  state->SM_9494 = A;
  // EXX

  // call e.g. plot_sprite_even_entry
  state->SM_940F(state, IX, B, HL, DEdash, HLdash);

  HL = state->SM_9412;
  B  = state->SM_9415;
  goto _9404; // loop

_941a:
  A += B;
  B = A;
  // EXX
  goto _94b1;

plot_sprite_xxx_odd:
  A++;
  IX = 0x951a; // ps_odd_jumptable
  A = 4 - A;
  BC = 5 * A;
  IX += BC;
  BC = plot_sprite_odd_entry;
  goto do_set_callbacks;

_9436:
  IX = pms_jumptable;
  A = 8 - A;
  IX += A * 6;
  BC = state->SM_9412;
  state->SM_946C = BC;

  state->SM_946F = state->SM_9415;
  state->SM_945F = state->SM_9404;

  Bdash = 0xF; // mask
  // EXX
  D = 0;
_945f:
  A = state->SM_945F - B;
  if (C || Z) // maybe ((s8) A <= 0))
    goto _9474;
  state->SM_945F = A;

  plot_masked_sprite(state, ...); // call

  HL = state->SM_946C;
  B  = 0;
  goto _945f; // loop

_9474:
  B += A;
  plot_masked_sprite(sprite, ...); // exit via
  return;

_9479:
  // EX AF,AF'
  // EXX
  D = 0;
  // PUSH BC,HL
  H = D;
  L = D;
  A = B;
  B = 5;
  A--;
  A <<= 2;

  do {
    RL(A);
    if (carry)
      HL += DE;
    HL <<= 1;
  } while (--B > 0);

  RL(A);
  if (carry)
    HL += DE;

  // POP BC
  HL += BC;
  // POP BC
  D--;
  E = -E;
  // EXX
  // EX AF,AF'

  plot_sprite(state, Awidth_bytes, Bheight, HLbackbuf_addr,
              DEdash_bitmap_stride, HLdash_bitmap_data);
#endif
}

// $949C
//
// width_bytes - was A
// height - was B
// backbuf_addr - was HL
// bitmap_stride - was DE'
// bitmap_data - was HL'
void plot_sprite(chqstate_t *state,
                 u8          width_bytes,
                 u8          height,
                 u8         *backbuf_addr,
                 u16         bitmap_stride,
                 const u8   *bitmap_data)
{
  int carry = 0;
  int jump_offset; // was IX

  SRL(width_bytes);
  if (carry) {
    plot_sprite_odd(state,
                    width_bytes,
                    height,
                    backbuf_addr,
                    bitmap_stride,
                    bitmap_data);
    return;
  }

  // sprite has even width

  jump_offset = 5 * (4 - width_bytes); // 5 bytes/op

  plot_sprite_even_entry(state,
                         jump_offset,
                         height,
                         backbuf_addr,
                         bitmap_stride,
                         bitmap_data);
}

void plot_sprite_even_entry(chqstate_t *state,
                            int         jump_offset,
                            u8          height,
                            u8         *backbuf_addr,
                            u16         bitmap_stride,
                            const u8   *bitmap_data)
{
  const u8 *SPsrc;
  u8       *backbuf_orig; //  was A

  // Conv: B & C moved into prevbufrow forward
  // EXX bank
  goto plot_sprite_even_start;

  for (;;) {
    // EXX bank
    if (--height == 0)
      return;

    bitmap_data += bitmap_stride;

plot_sprite_even_start:
    SPsrc = bitmap_data;
    // EXX unbank
    backbuf_orig = backbuf_addr;
    switch (jump_offset / 5) {
    default:
      assert(0);
    case 0:
      // Conv: Original uses POP that loads 16 bits at a time
      *backbuf_addr++ = *SPsrc++;
      *backbuf_addr++ = *SPsrc++;
    case 1:
      *backbuf_addr++ = *SPsrc++;
      *backbuf_addr++ = *SPsrc++;
    case 2:
      *backbuf_addr++ = *SPsrc++;
      *backbuf_addr++ = *SPsrc++;
    case 3:
      *backbuf_addr++ = *SPsrc++;
      *backbuf_addr = *SPsrc++;
    }
    backbuf_addr = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(backbuf_orig)));
  }
}

void plot_sprite_odd(chqstate_t *state,
                     u8          width_bytes,
                     u8          height,
                     u8         *backbuf_addr,
                     u16         bitmap_stride,
                     const u8   *bitmap_data)
{
  int       jump_offset;  // was IX
  const u8 *SPsrc;
  u8       *backbuf_orig; // was A

  // sprite has an odd width

  width_bytes++;
  jump_offset = 5 * (4 - width_bytes); // 5 bytes/op

  // Conv: B & C moved into prevbufrow
  // EXX bank
  goto plot_sprite_odd_start;

  for (;;) {
    // EXX bank
    if (--height == 0)
      return;

    bitmap_data += bitmap_stride;

plot_sprite_odd_start:
    SPsrc = bitmap_data;
    // EXX unbank
    backbuf_orig = backbuf_addr;
    switch (jump_offset / 5) {
    default:
      assert(0);
    case 0:
      // Conv: Original uses POP that loads 16 bits at a time
      *backbuf_addr++ = *SPsrc++;
      *backbuf_addr++ = *SPsrc++;
    case 1:
      *backbuf_addr++ = *SPsrc++;
      *backbuf_addr++ = *SPsrc++;
    case 2:
      *backbuf_addr++ = *SPsrc++;
      *backbuf_addr++ = *SPsrc++;
    case 3:
      *backbuf_addr = *SPsrc++;
    }
    backbuf_addr = OFFSETTOBACKBUF(prevbufrow(BACKBUFTOOFFSET(backbuf_orig)));
  }
}

// $9542
//
// width_bytes - was A
// height - was B
// backbuf_addr - was HL
// bitmap_stride - was DE'
// bitmap_data - was HL'
void plot_sprite_flipped(chqstate_t *state,
                         u8          width_bytes,
                         u8          height,
                         u8         *backbuf_addr,
                         u16         bitmap_stride,
                         const u8   *bitmap_data)
{
  int carry = 0;
  int jump_offset; // was IX

  backbuf_addr += width_bytes;

  SRL(width_bytes);
  if (carry) {
    plot_sprite_flipped_odd(state, width_bytes, height, backbuf_addr,
                            bitmap_stride, bitmap_data);
    return;
  }

  // sprite has even width

  jump_offset = 9 * (4 - width_bytes); // 9 bytes/op

#if 0
  D' = 0xEF;
  // EXX bank
  D = 0;
#endif

  // -- split here? (creating plot_sprite_flipped_even)

  const u8 *SPsrc;
  u8       *backbuf_orig; //  was A

  goto plot_sprite_flipped_even_start;

  for (;;) {
  // EXX bank
  if (--height == 0)
  return;

  bitmap_data += bitmap_stride;

  plot_sprite_flipped_even_start:
  SPsrc = bitmap_data;
  // EXX unbank
  backbuf_orig = backbuf_addr;
  switch (jump_offset / 9) {
  default:
  assert(0);
  case 0:
  // Conv: Original uses POP that loads 16 bits at a time
  *backbuf_addr-- = state->flipped[*SPsrc++ & 0xFF];
  *backbuf_addr-- = state->flipped[*SPsrc++ >> 8];
  case 1:
  *backbuf_addr-- = state->flipped[*SPsrc++ & 0xFF];
  *backbuf_addr-- = state->flipped[*SPsrc++ >> 8];
  case 2:
  *backbuf_addr-- = state->flipped[*SPsrc++ & 0xFF];
  *backbuf_addr-- = state->flipped[*SPsrc++ >> 8];
  case 3:
  *backbuf_addr-- = state->flipped[*SPsrc++ & 0xFF];
  *backbuf_addr-- = state->flipped[*SPsrc++ >> 8];
}
  backbuf_addr = ADDRTOBACKBUF(prevbufrow(BACKBUFTOADDR(backbuf_orig)));
}
}

  void plot_sprite_flipped_odd(chqstate_t *state,
  u8          width_bytes,
  u8          height,
  u8         *backbuf_addr,
  u16         bitmap_stride,
  const u8   *bitmap_data)
  {
  int       jump_offset; // was IX
  const u8 *SPsrc;
  u8       *backbuf_orig; //  was A

  // sprite has an odd width

  width_bytes++;
  jump_offset = 9 * (4 - width_bytes); // 9 bytes/op

#if 0
  D' = 0xEF;
  // EXX bank
  D = 0;
#endif

  // -- split here ?

  // Conv: B & C moved into prevbufrow
  // EXX bank
  goto psf_odd_body;

  for (;;) {
    // EXX bank
    if (--height == 0)
      return;

    bitmap_data += bitmap_stride;

psf_odd_body:
    SPsrc = bitmap_data;
    // EXX unbank
    backbuf_orig = backbuf_addr;
    switch (jump_offset / 9) {
    default:
      assert(0);
    case 0:
      // Conv: Original uses POP that loads 16 bits at a time
      *backbuf_addr-- = state->flipped[*SPsrc++ & 0xFF];
      *backbuf_addr-- = state->flipped[*SPsrc++ >> 8];
    case 1:
      *backbuf_addr-- = state->flipped[*SPsrc++ & 0xFF];
      *backbuf_addr-- = state->flipped[*SPsrc++ >> 8];
    case 2:
      *backbuf_addr-- = state->flipped[*SPsrc++ & 0xFF];
      *backbuf_addr-- = state->flipped[*SPsrc++ >> 8];
    case 3:
      *backbuf_addr-- = state->flipped[*SPsrc++ & 0xFF];
    }
    backbuf_addr = OFFSETTOBACKBUF(prevbufrow(BACKBUFTOOFFSET(backbuf_orig)));
  }
}

// $961B
u8 rng(chqstate_t *state)
{
  int carry = 0;
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
//
// priority - was A
// chatterblk - was HL
void start_chatter(chqstate_t       *state,
                   chatterpriority_t priority,
                   const u8         *chatterblk)
{
  u8 chatter_state; // was A

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

// $9965
void drive_chatter(chqstate_t *state)
{
  int          carry = 0;
  u8           chatter_state; // was A
  char         character;     // was D
  u8           rotating;      // was A
  u8           delay;         // was A
  u8           x;             // was A
  u8           B;
  const char  *HL;
  const u8    *chatterblk;    // was HL
  u8           chattercmd;    // was A

  chatter_state = state->chatter_state;

  if (--chatter_state == 0) // starting (1)
    goto starting;

  if (--chatter_state == 0) // displaying (2)
    goto do_noise_effect;

  if (--chatter_state == 0) { // stopping (3)
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
  rotating = state->chatter_cursor_blink; // Conv: Was self modified
  RRC(rotating);
  state->chatter_cursor_blink = rotating;
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
  assert(chatter);
  for (;;) {
    cmd = *chatter++; // read a command ($FC) or speaking character's ID
    if (cmd != CHATTERCMD_RANDOM)
      break;

    // Random choice
    rnd = rng(state);
    if (rnd >= 0x55) {
      chatter += 2;
      if (rnd >= 0xAA)
        chatter += 2;
    }
    /* Conv: This is an index, not an address */
    assert(*chatter < CHATTERBLK__LIMIT);
    chatter = chatter_blocks[*chatter];
  }

  // cmd is now the character ID
  assert(cmd <= CHATTERCHR_TONY);
  face = state->stage->addrof_perp_mugshot_bitmap;
  if (cmd != CHATTERCHR_PILOT)
    face = &bitmap_faces[(cmd - 1) * FACEBYTES]; // Conv: Simplified

  plot_face(state, 0x4036, face); // Set plot address to (176,8)

  pc_chatter_message(state, chatter); // was FALLTHROUGH
}

// $9A24
//
// chatter - was HL
void pc_chatter_message(chqstate_t *state, const u8 *chatter)
{
  u8          index;
  const char *string; // was DE

  // Conv: Original game loads an address directly here.
  assert(*chatter < CHATTERSTR__LIMIT);
  index = *chatter++;
  if (index < CHATTERSTR_PERP_DESC_1)
    string = common_chatter_strings[index];
  else {
    assert(index < CHATTERSTR__LIMIT);
    string = state->stage->chatter_strings[index - CHATTERSTR_PERP_DESC_1];
  }
  assert(string);
  state->chatterblk_ptr = chatter;
  state->next_character = string;
  pc_clear_line(state, 0); // was FALLTHROUGH
}

// $9A30
//
// x - was A
void pc_clear_line(chqstate_t *state, u8 x)
{
  const char *nextch;    // was HL
  char        character; // was D

  if (x == 0)
    clear_message_line(state);

  nextch = state->next_character;
  assert(nextch);
  character = *nextch & ~STREND; // remove any terminator
  assert(character >= ' ' && character < 'Z');
  plot_mini_font_cursor_on(state, x, character);
  if (*nextch++ & STREND) // if terminated
    state->chatter_delay = 10; // pause at end of string
  state->message_x = x + 1;
  state->next_character = nextch;
}

// $9A55
//
// counter - was A
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
static u16 nextscrrow(u16 screen)
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
//
// counter - was A
void noise_effect_9a5c(chqstate_t *state, u8 counter)
{
  int   carry = 0;
  u8    x;              // was A
  char  character;      // was D
  u16   DEscreen;       // was DE
  u8    C;              // was C
  u8    B;              // was B
  u16   DEscreen_saved; // was stack?
  u8   *noisebytes;     // was HL
  u8    A;              // was A

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
    noisebytes = &state->noise_bytes[0];
    do {
      A = *noisebytes - B;
      *noisebytes++ = A;
      RLC(A);
      A += *noisebytes;
      *noisebytes = A;
      state->screen[DEscreen - SCREEN_START_ADDRESS] = A;
      DEscreen++; // was E++
    } while (--B > 0);
    DEscreen = DEscreen_saved; // was POP - restore row ptr
    DEscreen = nextscrrow(DEscreen);
  } while (--C > 0);

  ne_plot_attrs(state, attribute_BRIGHT_WHITE_OVER_BLACK);
  // was FALLTHROUGH
}

// $9A98
//
// attr - was A
void ne_plot_attrs(chqstate_t *state, u8 attr)
{
  int addr;       // was HL
  int iterations; // was B

  // Screen attribute (22,1) (Conv: address -> offset)
  addr       = 0x5836 - SCREEN_START_ADDRESS;
  iterations = FACEATTRHEIGHT; // 5 rows
  do {
    // Conv: Screen write now goes via state.
    memset(&state->screen[addr], attr, FACEATTRWIDTH);
    addr += SCREEN_ATTRIBUTES_WIDTH;
  } while (--iterations > 0);
}

// $9AAB
//
// screen - was DE
// face - was HL
void plot_face(chqstate_t *state,
               u16         screen, // Z80 address
               const u8   *face)
{
  u16 saved_screen; // was stack
  u16 counter;      // was BC

  assert(screen >= SCREEN_START_ADDRESS && screen < SCREEN_END_ADDRESS);
  assert(face);

  counter = FACEBITMAPBYTES;
  saved_screen = screen;
  screen -= SCREEN_START_ADDRESS; // Conv: address -> offset
  for (;;) {
    state->screen[screen++] = *face++; counter--;
    state->screen[screen++] = *face++; counter--;
    state->screen[screen++] = *face++; counter--;
    state->screen[screen++] = *face++; counter--;
    screen -= 4; // replaces PUSH/POP
    if (counter == 0)
      break;
    screen = nextscrrow(screen);
  }

  plot_face_attributes(state, saved_screen, face); // was fallthrough
}

// $9ACE
//
// screen - was POP DE
// face - HL
void plot_face_attributes(chqstate_t *state,
                          u16         screen,
                          const u8   *face)
{
  int carry = 0;
  u8  A;       // was A
  u16 counter; // was BC

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
    state->screen[screen++] = *face++; counter--;
    state->screen[screen++] = *face++; counter--;
    state->screen[screen++] = *face++; counter--;
    state->screen[screen++] = *face++; counter--;
    if (counter == 0)
      break;

    // TODO Hoist to next-attr-row macro?
    int t = (screen & 0xFF) + 0x1C;
    screen = (screen & 0xFF00) | (t & 0xFF);
    if (t >= 0x100)
      screen += 256;
  }
}

// $9AEC
void plot_mini_font_cursor_off(chqstate_t *state,
                               u8          x,
                               char        character)
{
  pmf_go(state, x, character, ________, ________);
}

// $9AF1
void plot_mini_font_cursor_on(chqstate_t *state,
                              u8          x,
                              char        character)
{
  pmf_go(state, x, character, _____XXX, X_______);
}

// $9AF4
//
// x - was A
// ascii - was D
// extrabm1 - was B
// extrabm2 - was C
void pmf_go(chqstate_t *state,
            u8          x,
            char        ascii,
            u8          extrabm1,
            u8          extrabm2)
{
  int       carry = 0;

  u8        extra2;   // was self modified $9B61 - right extra bitmap
  u8        extra1;   // was self modified $9B64 - left extra bitmap
  u8        mask;     // was self modified $9B89
  u8        rotate;   // was self modified $96B7

  u8        A;        // was A
  u8        ascii2;   // was A
  u8        row;      // was A
  u8        gid;      // was C
  u8        sgid;     // was A
  u16       screen;   // was DE
  const u8 *fontdata; // was DE
  u16       HLscreen; // was HL

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
  assert(0);
  gid += ascii2 - '/'; // not convinced this is ever used in the game

pmf_have_glyph_id:
  sgid = gid + 'A'; // Turn the glyph ID in #REGc into ASCII in #REGa

pmf_have_ascii:
  fontdata = &minifont[(sgid - 'A') * MFHEIGHT];
  HLscreen = screen; // was EX
  row = MFHEIGHT;
  do {
    u8 bm2; // was C
    u8 bm1; // was B

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

// $9BA7
void clear_message_line(chqstate_t *state)
{
  u16 screen; // was HL
  u8  rows;   // was A

  screen = 0x45C1; // Screen coordinate (8,53)
  rows   = 6;      // Clear six rows
  do {
    memset(ADDRTOSCREEN(screen + 1), 0, 29); // Conv: Replacing LDIR
    screen = nextscrrow(screen);
  } while (--rows);
}

// $9BCF
void tick(chqstate_t *state)
{
  int   carry = 0;
  u8   *ptimebcd;    // was HL
  u8    timeupstate; // was A
  u8    timebcd;     // was A
  char *timedigits;  // was DE
  u8    effect;      // was B
  u8    H;           // was H
  u8    L;           // was L
  u8    A;           // was A
  u8    hidigit;     // was A
  u8    lodigit;     // was L

  if (state->perp_caught_phase > PERPCAUGHTPHASE_0
      || state->transition_control == TRANSITIONCONTROL_FADE)
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
  play_speech_hook(state, 4);

check_credits:
  if (state->transition_control > TRANSITIONCONTROL_STOP)
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
  state->transition_control = TRANSITIONCONTROL_FILL_ATTRIBUTES;
  state->st.turbos          = MAXTURBOS;
  state->st.time_bcd        = RESTART_TIME_BCD;
  state->retry_count++;

  play_start_noise(state);
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
  start_sfx(state, effect, 1); /* priority 1 => high */
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

// $9C79
//
// Extracted from above
void play_start_noise(chqstate_t *state)
{
  play_speech_hook(state, 5); // exit via
}

// $9CC2
void speed_score(chqstate_t *state)
{
  int carry = 0;
  u16 speed; // was HL
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
  increment_score(state, A, 0, 0); // exit via
}

// $9CD6
//
// lo - was A
// md - was E
// hi - was D
//
// Bug: As soon as a non-zero->zero transition is seen the routine finishes so
// you can only have a single run of zeroes in the bonus.
void add_bonus(chqstate_t *state, u8 lo, u8 md, u8 hi)
{
  char *output;      // was HL
  u8    nonzeroflag; // was C - used to track if a zero has been emitted.

  output = &state->bonus_string[6]; // points to byte after buffer
  nonzeroflag = 0xFF; // flag (zero not seen)
  (void) bonus_digit(lo >> 0,
                     &nonzeroflag,
                     &output); // always runs since flag > 0
  *output |= STREND; // terminate string

  // Using lazy evaluation here to avoid having a load of gotos
  (void)(bonus_digit(lo >> 4, &nonzeroflag, &output) >= 0 &&
         bonus_digit(md >> 0, &nonzeroflag, &output) >= 0 &&
         bonus_digit(md >> 4, &nonzeroflag, &output) >= 0 &&
         bonus_digit(hi >> 0, &nonzeroflag, &output) >= 0 &&
         bonus_digit(hi >> 4, &nonzeroflag, &output) >= 0);

  state->SM_address_of_score_digits = output;
  state->trigger_bonus_flag = 1;
  increment_score(state, lo, md, hi); // was fallthrough
}

// Subroutine of above broken out
int bonus_digit(u8 digit, u8 *nonzeroflag, char **poutput)
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
void increment_score(chqstate_t *state, u8 lo, u8 md, u8 hi)
{
  int carry = 0;
  u8 *scorebcd; // was HL
  u8  A;        // was A

  scorebcd = &state->score_bcd[0];
  A = lo + *scorebcd;
  *scorebcd++ = DAA(A, &carry);
  A = md + *scorebcd + carry;
  *scorebcd++ = DAA(A, &carry);
  A = hi + *scorebcd + carry;
  *scorebcd++ = DAA(A, &carry);
  A = *scorebcd + carry;
  *scorebcd = DAA(A, &carry);
}

// $9D2E
void calc_overtake_bonus(chqstate_t *state)
{
  int carry = 0;
  u8  counter;    // was A
  u8  iterations; // was B
  u8 *bcd;        // was HL

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

// $9D62
void update_scoreboard(chqstate_t *state)
{
  toggle_light_brightness(state, ADDRTOSCREEN(0x5820));
  toggle_light_brightness(state, ADDRTOSCREEN(0x583B));
  plot_turbos_and_digits(state);
}

// $9DF4
//
// attrs - was HL
void toggle_light_brightness(chqstate_t *state, u8 *attrs)
{
  int rows; // was B
  u8  attr; // was C

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

// $9E11
void plot_turbos_and_digits(chqstate_t *state)
{
  int        carry = 0;
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

  // Score

  ptas_led_digits(state, 4, &state->score_bcd[3], &state->st.score_digits[7],
                  ADDRTOSCREEN(0x4126)); // was fallthrough
}

// $9F1E
//
// iterations - was B
// digits - was DE
// stored - was HL
// screen - was DE'
void ptas_led_digits(chqstate_t *state,
                     u8          iterations,
                     const u8   *digits,
                     u8         *stored,
                     u8         *screen)
{
  u8 Adigits;
  u8 Cdigits;

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

#define LEDFONT_HEIGHT (15)

// $9F47
//
// ord - was A
// screen - was DE'
u8 *ledfont_plot(chqstate_t *state, int ord, u8 *screen)
{
  const u8 *src;         // was HL
  u8       *screen_copy; // was stacked

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

// $9F99
//
// attrval - was A
// attrs - was BC
// backbuf - was DE
// string - was HL
// style - was A'
const u8 *draw_string_with_style(chqstate_t *state,
                                 u8          attrval,
                                 u8         *attrs,
                                 u8         *backbuf,
                                 const u8   *string,
                                 u8          style)
{
  return draw_string_core(state,
                          backbuf,
                          string, /*HL*/
                          style, /*A'*/
                          attrval, /*C'*/
                          32, /*DE'*/
                          attrs/*HL'*/);
}

// $9FA3
//
// attrval - was A
// attrs - was BC
// backbuf - was DE
// string - was HL
const u8 *draw_string_generic(chqstate_t *state,
                              u8          attrval,
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

// $9FA6
//
// Broken out from above.
//
// backbuf - was DE
// string - was HL
// style - was A'
// attrval - was C'
// attrsstride - was DE'
// attrs - was HL'
const u8 *draw_string_core(chqstate_t *state,
                           u8         *backbuf,
                           const u8   *string,
                           u8          style,
                           u8          attrval,
                           u8          attrsstride,
                           u8         *attrs)
{
  u8 character; // was A

  do {
    character = *string & ~STREND;
    draw_char(state, character, backbuf, style, attrval, attrsstride, attrs,
              &backbuf, &attrs);
  } while ((*string++ & STREND) == 0);

  return string;
}

// $9FB4
//
// character - was A
// screen - screen address - was DE
// style - draw style - was A'
// attrval - attribute value - was C'
// attrstride - was DE'
// attrs - was HL'
// new_screen - added
// new_attrs - added
void draw_char(chqstate_t *state,
               u8          character,
               u8         *screen,
               u8          style,
               u8          attrval,
               u8          attrstride,
               u8         *attrs,
               u8        **new_screen,
               u8        **new_attrs)
{
  u8        glyphid;    // was C
  u8        data;       // was A
  u8        iterations; // was B
  const u8 *fontdata;   // was HL
  u8       *orig;       // was stacked
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
    *screen++ = *fontdata++; // was LDI, could reuse A
    screen--; // was DEC E, could remove if screen++ above is dropped
    screen += 256;
  }
  *screen = 0; // leave gap at bottom

dc_set_double_attrs:
  screen = orig + 1; // was POP screen, INC E
  *attrs |= attrval;
  attrs += attrstride;
  *attrs |= attrval;
  attrs -= attrstride; // was POP attrs
  attrs++; // was INC L
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
  screen = orig + 1; // was POP screen, INC E
  *attrs |= attrval;
  attrs++; // was INC L
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
  screen = orig + 1; // was POP screen

dc_return:
  *new_screen = screen;
  *new_attrs  = attrs;
}

// $A0D6
u8 keyscan(chqstate_t *state)
{
#if 0
  u8  Ainput;
  u8  E;
  u8 *HL;
  u8  A;

  if (state->kempston_flag) {
    Ainput = 0; // TODO state->speccy->in(state->speccy, port_KEMPSTON_JOYSTICK) & 0x1F;
    E = 0x20;
    HL = &state->keydefs[0];
    A = keyscan_a112(state, HL);
    RRC(A);
    RRC(A);
    RRC(A);
    A &= 0xE0;
    E = A | Ainput;
  } else {
    E = 1;
    HL = &state->keydefs[0];
    A = keyscan_a112(state, HL);
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
#endif
  return 0;
}

// $A112
u8 keyscan_a112(chqstate_t *state, u8 *HL)
{
#if 0
  u8 A;

  do {
    A = *HL++;
    keyscan_inner(state, A);
    carry = !carry;
    RL(E);
  } while (!carry);
  return E;
#endif
  return 0;
}

void keyscan_inner(chqstate_t *state, u8 A)
{
#if 0
  C = A;
  B = (A + 7) + 1; // shift
  SRL(C);
  SRL(C);
  SRL(C);
  C = 5 - C; // another shift
  A = 0xFE;
  do RRC(A);
  while (--B > 0);
  A = 0; // TODO state->speccy->in(state->speccy, port_?);
  do RR(A);
  while (--C > 0);
#endif
}

// $A399
void check_scenery_collisions(chqstate_t *state)
{
}

// $A4B8
void scenery_hit(chqstate_t *state)
{
}

// $A4F6
void fork_completed(chqstate_t *state)
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

// $A637
void perp_behaviour(chqstate_t *state)
{
}

// $A7F3
void spawn_cars(chqstate_t *state)
{
  u8        allow_spawning;     // was A
  u8        random_extra_delay; // was C
  u8        spawn_delay;        // was A
  u8        iterations;         // was B
  u8        cars_seen;          // was C
  hazard_t *hazard;             // was IX
  u16       spawn_lanes;        // was BC
  u8        min_lane;           // was B
  u8        max_lane;           // was C
  u8        new_lane;           // was A
  const u8 *hazard_pos;         // was HL
  u8        lod_index;          // was C

  // Return without spawning anything if perp_caught_phase is non-zero or the
  // dont_spawn_cars flag is set.
  if (state->perp_caught_phase > PERPCAUGHTPHASE_0 || state->dont_spawn_cars)
    return;

  // Return without spawning anything if allow_spawning is zero.
  allow_spawning = state->allow_spawning;
  if (allow_spawning == 0)
    return;

  // Reduce inline spawn delay counter by the value of allow_spawning (1 or 2
  // here).
  state->spawn_counter -= allow_spawning;
  if (state->spawn_counter > 0)
    return;

  random_extra_delay = rng(state) & 0x0F;

  spawn_delay = state->stage->car_spawn_delay;
  if (state->sighted_flag)
    // Perp was sighted so increase the spawn delay by 25.
    spawn_delay += 25;
  spawn_delay += random_extra_delay;
  state->spawn_counter = spawn_delay;

  // Now walk the hazards array to find an unused slot.
  iterations = 5;
  cars_seen = 0;  // one bit is set each time a car is seen
  hazard = &state->hazards[1];
  do {
    if (hazard->used == HAZARD_UNUSED)
      goto fill_in;
    if (hazard->TBD15 & (1 << 7)) // top bit is set for vehicles
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

  hazard->TBD17 = new_lane;
  hazard->TBD18 = new_lane;

  // Copy hazard_pos_speed values to hazard position and speed.
  hazard_pos = &hazard_pos_speed[-1 + new_lane];
  hazard->horz_pos_on_road = hazard_pos[0];
  hazard->speed            = hazard_pos[state->sighted_flag ? 8 : 4];

  // Now pick a random car LOD to show.
  lod_index = rng(state) & 6;
  // If we've sighted the perp then don't spawn any generic cars (offset 6)
  // since they look just like the perp's. Instead use offset 4.
  if (state->sighted_flag && lod_index == 6)
    lod_index -= 2; // 6 -> 4

  hazard->lod_addr = state->stage->lods_vehicles[lod_index / 2];
}

// $A89C
//
// extra - was C - extra buffer offset
u16 get_spawn_lanes(chqstate_t *state, u8 extra)
{
  int carry;
  u8 *roadbuf;    // was HL
  u8  lanes;      // was A
  u8  lanes_copy; // was E

  roadbuf = ROADBUFPTR(ROADBUF_LANES_OFFSET + 2 + extra);
  lanes = *roadbuf;
  if (lanes == MAP_LANES_4_VAL) // 0
    return 0x0104;
  lanes_copy = lanes;
  lanes &= 0xC1;
  if (lanes == 0xC1)
    return 0x0104;
  if (lanes == 0x41) // tunnel
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

// $A8CD
void hazard_handler(chqstate_t *state)
{
}

// $A955
void choose_dirt_and_stones(chqstate_t *state)
{
  u8 *table;  // was DE

  if (state->on_dirt_track == 0 || state->allow_spawning == 0)
    return;

  // TODO: table_ed00 is u16s but this stores two bytes at byte offset 40: a
  // stone/dirt type and a random position.

  table = (u8 *) &state->table_ed00[40];
  table[0] = ((s8) rng(state) >= 0) ? 1 : 2; // choose stone or dirt
  table[1] = rng(state); // choose random position
  state->SM_A97F = 1;
  state->SM_C0BC = 1;
  state->SM_A9DF = 1;
}

// $A97E
void layout_dirt_and_stones(chqstate_t *state)
{
}

// $A9DE
void dust_stones_stuff(chqstate_t *state)
{
}

// $AA38
void draw_helicopter(chqstate_t *state)
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

// $AD0D
void check_hazard_collisions(chqstate_t *state)
{
  hazard_t *hazard;     // was IX
  u8        iterations; // was B

  if (state->inhibit_collision_detection)
    return;

  // Iterate over all hazards.
  hazard = &state->hazards[0];
  iterations = 6;
  do {
    if (hazard->used != HAZARD_UNUSED) {
      // TBD15 is a delay of some sort used for hits
      // TBD17 suspected perp distance high byte
      if (hazard->TBD15 == 0xFF && hazard->TBD17)
        goto chc_continue;

      // Distance is < 20.
      // There was a collision.
      // TBD15 ?
      if (hazard->distance < 20 &&
          check_collision(state, 0, hazard) > 0 &&
          hazard->TBD15 != 0xFF)
        hazard->hit_handler(state);
    }

chc_continue:
    hazard++;
  } while (--iterations > 0);
}

// $AD51
//
// hazard - was IX
u8 check_collision(chqstate_t *state, u8 D, hazard_t *hazard)
{
  return 0;
}

// $ADA0
void draw_hazards(chqstate_t *state)
{
}
void dh_aecf(chqstate_t *state)
{
}

// $ADF9
//
// Conv: Original game used the RET at $ADF9 as a no-op.
void no_op(chqstate_t *state)
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

// $B4CC
void start_chase(chqstate_t *state)
{
}

// $B4F0
void smash(chqstate_t *state)
{
}

// $B549
void draw_debris(chqstate_t *state)
{
}

// $B58E
void draw_car(chqstate_t *state)
{
}

// $B627
void draw_car_part(chqstate_t *state)
{
}

// $B648
void draw_smoke(chqstate_t *state)
{
}

// $B67C
void draw_cherry(chqstate_t *state)
{
}

// $B69E
void draw_crash(chqstate_t *state)
{
}

// $B6D6
void draw_part(chqstate_t *state)
{
}

// $B6DD
void draw_part_entry2(chqstate_t *state,
                      u8          height,
                      u8          width,
                      u8          y,
                      u8          x,
                      const u8   *bitmap,
                      u8          flags)
{
}

// $B701
void draw_part_entry3(chqstate_t *state)
{
}

// $B716
void plot_masked_sprite(chqstate_t *state)
{
}

// $B724
void pms_entry(chqstate_t *state)
{
}

// $B76C
void plot_masked_sprite_flipped(chqstate_t *state)
{
}

// $B770
void plot_masked_sprite_flipped_entry2(chqstate_t *state)
{
}

// $B7EF
void plot_masked_sprite_variant(chqstate_t *state)
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

// $B9F4
void layout_road(chqstate_t *state)
{
  int       carry = 0;
  u8       *DElanedata_base;
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
  DElanedata_base = DElanedata = ROADBUFPTR(ROADBUF_LANES_OFFSET);

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
  DEroadpos = state->scenedata.road_pos;
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
    HLroadpos = state->scenedata.road_pos;
    HLroadpos_saved = HLroadpos; // was PUSH HLroadpos
    HLroadpos += DEforkdistance;
    state->scenedata.road_pos = HLroadpos; // adjust road pos for fork rendering
    build_curve_table(state, /*forked=*/1);
  } else {
    build_curve_table(state, /*forked=*/1);
    DEforkdistance = HLforkdistance; // was POP DEforkdistance
    HLroadpos = state->scenedata.road_pos;
    HLroadpos_saved = HLroadpos; // was PUSH HLroadpos
    HLroadpos -= DEforkdistance;
    state->scenedata.road_pos = HLroadpos; // adjust road pos for fork rendering
    build_curve_table(state, /*forked=*/0);
  }
  // $BB07
  HLroadpos = HLroadpos_saved; // was POP HLroadpos
  state->scenedata.road_pos =
    HLroadpos; // restore normal road pos after fork rendering
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
    *SMroadcentre      = (HLdash + DEdash) / 2; // (right+left)/2 = new road centre
    *SMroadcentreleft  = (HLdash + DEdash) / 2; // new road centre left

    DEdash = *SMveryright;
    HLdash = *SMroadcentreright;
    *SMroadcentreright = (HLdash + DEdash) / 2;
    *SMroadright       = (HLdash + DEdash) / 2;

    Aiterations += 2;
    // EXX Unbank
  } while (--Aiterations > 0);
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
      u8  H;
      u8  A;
      u8  L;
      int res;
      int carry, overflow;

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
      BCattrs = (attribute_BRIGHT_BLACK_OVER_CYAN << 8) |
                attribute_BRIGHT_BLACK_OVER_CYAN;
      // If A was zero then jump (Z => sky, NZ => ground)
      if (D != 0) {
        // Set ground colour
        BCattrs = state->stage->ground_colour;
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
}

// $BDC1
void clear_playfield_set_attrs(chqstate_t *state)
{
  u8 *screen;     // was HL
  u16 stride;     // was DE
  int iterations; // was B

  clear_playfield(state);

  // Clear the playfield pixels to $FF (bug: duplicates work just done)
  memset(ADDRTOSCREEN(0x4800), 0xFF, PLAYFIELD_HEIGHT * SCREEN_BITMAP_ROWBYTES);

  // Clear the playfield attributes to $28 (black over cyan) - first two
  // rows only
  memset(ADDRTOSCREEN(0x5900), attribute_BLACK_OVER_CYAN,
         2 * SCREEN_ATTRIBUTES_ROWBYTES);

  // Clear the next three rows to $68 (bright, black over cyan)
  memset(ADDRTOSCREEN(0x5940), attribute_BRIGHT_BLACK_OVER_CYAN,
         3 * SCREEN_ATTRIBUTES_ROWBYTES);

  // Clear the next 11 rows to the current ground colour
  // Note: Only using the bottom byte of ground_colour (as orig).
  memset(ADDRTOSCREEN(0x59A0), state->stage->ground_colour,
         11 * SCREEN_ATTRIBUTES_ROWBYTES);

  // Clear the edges of the playfield to black on black
  screen = ADDRTOSCREEN(0x5900);
  stride = SCREEN_BITMAP_ROWBYTES - 1;
  iterations = 16;
  do {
    *screen = attribute_BLACK_OVER_BLACK;
    screen += stride;
    *screen++ = attribute_BLACK_OVER_BLACK;
  } while (--iterations > 0);
}

// $BDFB
void read_map(chqstate_t *state)
{
  int carry = 0;
  u8 *pfast_counter;  // was HL
  u16 speed;          // was DE
  u8  speed_lo;       // was A
  u8  allow_spawning; // was A

  state->var_a23d       = 0;
  state->var_a23c       = 0;
  state->allow_spawning = 0;
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
    rm_cycle_buffer_offset(state, pfast_counter); // was fallthrough

  state->allow_spawning += allow_spawning;
  check_hazard_collisions(state); // exit via
}

/// An add that affects the low byte only.
#define LO_ADD(t,d) (((t) & ~0xFF) | (((t) + (d)) & 0xFF))

// $BE1F
//
// pfastcounter - was HL
void rm_cycle_buffer_offset(chqstate_t *state, u8 *pfastcounter)
{
#if 0
  int carry = 0;
  u8 *HL;
  u8  A;
  u8 *DE;

  HL = state->road_buffer_offset; // Conv: was an INC
  A = *HL + 1;
  *HL = A;
  A += 0x5F;
  HL = ROADBUFPTR(A);
  state->var_a23c |= *HL;
  HL = LO_ADD(HL, 0x20); // ROADBUFPTR(A + 0x20);
  state->var_a23d |= *HL;
  HL = LO_ADD(HL, -0x60); // ROADBUFPTR(A + 0x40); ?

  // -- CURVATURE --

  A = state->curvature_byte;
  carry = A < 16;
  A -= 16;
  if (!carry)
    goto rm_save_curvature_byte;

  DE = state->scenedata.road_curvature_ptr + 1;
  A = *DE;
  if (A)
    goto rm_curvature_regular_byte;

rm_curvature_escape_byte:
  // EX DE,HL
  HL++;
  A = *HL;
  HL++;
  if (A == 0)
    goto rm_curvature_jump_command;
  if (--A == 0)
    goto rm_curvature_one_command;
  // Otherwise it must be a fork road command (byte == 2).
  state->SM_BB95 = wordat(HL);
  HL += 2;
  state->SM_BBC2 = wordat(HL);

  HL = &forked_road_curvature[0];
  goto rm_read_curvature;

rm_curvature_one_command:
  HL = state->SM_something;
  goto rm_read_curvature;

rm_curvature_jump_command:
  HLsomething = wordat(HL);

rm_read_curvature:
  // EX DE,HL
  A = *DE;

rm_curvature_regular_byte:
  state->scenedata.road_curvature_ptr = DE;
  A -= 16;

rm_save_curvature_byte:
  state->curvature_byte = A;
  A &= 0x0F;
  if ((A & (1 << 3)) == 0)
    goto rm_set_curvature;
  A &= 0x07;
  A = -A;

rm_set_curvature:
  A += A;
  *HL = A;
  L += 0x20;
#endif
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

// $C8BE
void pre_shift_backdrop(chqstate_t *state)
{
  int       tmp;        // for RRD()
  const u8 *source;     // was HL
  u8       *preshifted; // was DE
  const u8 *endptr;     // was DE
  u8       *bmptr;      // was HL
  u8        row;        // was C
  u8        col;        // was B
  u8        pix;        // was A

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

// $C8E3
void forked_road_plotter(chqstate_t *state)
{
}

// $CBA4
// mystery_cba4 would go here, if we knew what it did

// $CBC5
void backdrop_fill_choice(chqstate_t *state)
{
//  C = A;
//  if (A < 80)
//    goto frp_c915; // inside forked_road_plotter
//  else
//    goto dr_start_backdrop_fill; // inside draw_road
}

// $CBD6 ish
void build_curve_table(chqstate_t *state, int forked)
{
  u16       *table1, *table2;
  const u8  *road_buffer_ptr_HL; // was HL
  u8         curvature_C;        // was C
  int        A;
  int        B;
  const u8  *IY;
  const u16 *IX;
  u8        *DE;
  u16        DEdash;
  u8         curvature_A;
  u16        HLdash;
  u16        BCdash;
  int        carry;
  u16        DEroadpos;          // was DE
  u8        *DEe320;
  const u8  *HLe760;
  int        Bdash;

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
  DEdash = state->scenedata.road_pos;
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

  DEroadpos = state->scenedata.road_pos; // was POP DE
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
                                u8          Bdash_alwayszero,
                                u16        *HLtableend,
                                u16         DEroadpos)
{
  u8  *IYe300;
  int  Biterations;
  u16 *SPoutput;
  int  A;
  int  Bdash;
  int  Cdash;
  int  Ldash;
  int  Aopcode;
  int  Atotal;
  u16  HLdash;

  IYe300 = &state->table_e300[0]; // was 0xE300; // addr of height table
  Biterations = 21;
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
  int       carry = 0;
  u8       *proadbuf_height;      // was IY
  u8       *proadbuf_height_base; // Conv: added
  u8        heightbyte;           // was C
  u8        counter;              // was A
  u8        orig_counter;         // was B
  const u8 *pvtab;                // was HL
  const u8 *pvtabbase;            // Conv: added
  u8        C;                    // was C
  u8        iterations;           // was B'
  u8       *phtab;                // was DE'
  u8       *phtabbase;            // Conv: added
  u16       v;                    // was DE
  u16       result;               // was HL
  u8        A;                    // was A
  u8       *pdst;                 // was HL
  u8       *pdstbase;             // Conv: added
  const u8 *htab2;                // was DE
  const u8 *htabbase2;            // Conv: added
  u8        iterations2;          // was B

  proadbuf_height_base = proadbuf_height = ROADBUFPTR(ROADBUF_HEIGHT_OFFSET);

  // Read the current height byte
  heightbyte = *proadbuf_height;
  counter = state->fast_counter & 0xE0;

  // Scale 0..223 (in steps of 16) to 0..153, reducing <counter> by 31.25%,
  // mapping the incoming value to the 7x22 byte tables. So fast_counter
  // indexes the rows of the table.
  orig_counter = counter; // Copy to be a multiplier later
  counter = counter - (counter >> 2) - (counter >> 4);

  pvtabbase = pvtab = &vertical_e600[counter / 22][1];
  C = -multiply(orig_counter, heightbyte);
  // EXX bank

  // This builds the look-up table at $E301. Assuming it's a height table.
  iterations = 21;
  phtabbase = phtab = &state->table_e300[1];
  do {
    v = *pvtab * 2;
    result = 0;
    C = A = C + *proadbuf_height;
    if (C != 0) {
      if (C < 0) {
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
      carry = (A >> 7) & 1; A <<= 1;
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
  pdstbase  = pdst  = &state->table_e336[0]; // destination
  htabbase2 = htab2 = &state->table_e300[1]; // src
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

  const struct Relocations *reloc; // was HL
  u8                   iterations; // was BC
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

  bootstrap(state);
}

// $EC2C
void menu_draw_char(chqstate_t *state,
                    u8          Achar,  // ASCII
                    u8          Fdash,  // dbl height if carry set
                    u8          Cdash,  // attribute byte
                    u8         *DEdash, // screen address
                    u8         *HLdash, // attribute address
                    u8        **DEdash_out,
                    u8        **HLdash_out)
{
  const u8 *HLfont;       // was HL
  u8       *DEscreen;     // was DE
  u8        Cglyphid;     // was C
  u8       *HLdash_saved; // was B'
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
  DEdash++; // was INC E
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
    HLdash_saved = HLdash; // was just B' saving L'
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

// $EF00
void bootstrap(chqstate_t *state)
{
  for (;;) {
    int carry = 0;
    u8 *HL;
    int B;
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

