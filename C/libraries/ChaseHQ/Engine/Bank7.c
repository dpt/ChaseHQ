/**
 * Bank7.c
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
 * C translation of ZX Spectrum 128K "bank 7" -- the end-of-game
 * results/credits sequence. Paged into $C000-$FFFF and reached via an inner
 * relocation from $F7EF to $F300, driven by a script interpreter at $E20A.
 * See show_end_screen() below. es_setup_interrupts remains a stub -- see its
 * own prologue. es_play_music_48k drives the beatbox each frame (relocated
 * to $F340 -- the real target of that call, not speech as originally
 * guessed) but its drum/noise instruments (es_playdrum_2/1, es_play_noise)
 * remain unported stubs, so playback is currently silent on instrument
 * notes.
 */

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "C99/Types.h"

#include "ZXSpectrum/Macros.h"
#include "ZXSpectrum/Spectrum.h"
#include "ZXSpectrum/Z80.h"

#include "ChaseHQ/ChaseHQ.h"

#include "ChaseHQ/Data/Bank7Data.h"
#include "ChaseHQ/Data/CommonData.h"

#include "Types.h"
#include "Internal.h"
#include "State.h"
#include "Bank7State.h"

#include "Bank7.h"

#include <assert.h>

/* ----------------------------------------------------------------------- */

#define ADDRTOSCREEN(addr)  z80addrtoscreen(state, addr, 0, 0)
#define ADDRTOATTRS(addr)   z80addrtoattrs(state, addr, 0, 0)
#define ADDRTOBACKBUF(addr) z80addrtobackbuf(state, addr)

#define ENDSHOT_WIDTH  (13 * 8) /* pixel width of an end-screen bitmap */
#define ENDSHOT_HEIGHT (64)     /* pixel height of an end-screen bitmap */

/* ----------------------------------------------------------------------- */

static void es_clear(chqstate_t *state);
static u16 next_screen_row(u16 addr);
static void draw_endshot(chqstate_t *state, const u8 *image, u16 screen_addr);
static const u8 *z80addrtoendshot(u16 addr);
static void es_draw_frame_common(chqstate_t *state, const u8 **script);
static void es_clear_then_draw_frame(chqstate_t *state, const u8 **script);
static void es_attribute_fade_in(chqstate_t *state);
static void es_attribute_fade_out(chqstate_t *state, u8 *flag);
static void es_handler_glyph_fade_b(chqstate_t *state);
static void es_handler_glyph_fade_c(chqstate_t *state);
static void es_handler_handshake(chqstate_t *state);
static void es_handler_handshake_advance(chqstate_t *state);
static void es_handler_idle(chqstate_t *state);
static void es_handler_draw_score(chqstate_t *state);
static const u8 *z80addrtochatterblk(u16 addr);
static void es_chatter(chqstate_t *state);
static void es_set_dispatch(chqstate_t *state,
                            void (*handler)(chqstate_t *state),
                            u8 reload);
static int ascii_to_glyph_id(int character);
static void plot_char(chqstate_t *state,
                      u8          A_char,
                      u8          D_row,
                      u8         *E_col,
                      u8          H_attr,
                      u8         *L_attr,
                      u8          A_attr);
static void render_text_common(chqstate_t *state, const u8 **script);
static void es_handler_render_text(chqstate_t *state, const u8 **script);
static void run_script(chqstate_t *state);
static void es_setup_interrupts(chqstate_t *state);
static void es_next_pattern_at_addr(chqstate_t *state, const u8 *HL_pataddr);
static void es_advance_pattern(chqstate_t *state);
static void es_reset_music(chqstate_t *state);
static void es_playdrum_go(chqstate_t *state, int D_length, u8 *HL_data);
static void es_playdrum_2(chqstate_t *state, int A_speed);
static void es_playdrum_1(chqstate_t *state, int A_speed);
static void es_play_noise(chqstate_t *state, int A_param);
static void es_play_music_48k(chqstate_t *state);

/* ----------------------------------------------------------------------- */
/**
 * $E000: Show the end screen
 *
 * Displays the end-of-game results screen: clears the playfield, starts the
 * beatbox and bank 7's own interrupt-driven music/script engine, then loops
 * driving the script/frame-advance dispatch and drive_chatter each frame
 * until the fire key is pressed twice (once to reach the congratulations
 * script, once more to exit), clearing the queued key each time.
 *
 * Conv: the Z80 entry point LDIRs itself from $E000 to $5C00, then LDIRs a
 *       second, inner 768-byte block from $F7EF to $F300 and runs from there
 *       (see project memory project-endscreen-bank7-double-relocation). Both
 *       relocations are pure ZX paging/self-modification artefacts and are
 *       discarded here, following the load_stage precedent -- the C functions
 *       below are simply called directly.
 */
void show_end_screen(chqstate_t *state)
{
  u8  A_input;     /* keyscan result, tested for the fire bit (was A) */
  int outer_count; /* per-keyscan frame divider, reloads to 5/6 (was A171) */

  es_clear(state);
  es_setup_interrupts(state);
  es_reset_music(state);

  assert(sizeof(es_script) == sizeof(state->bank7->es_script));
  memcpy(state->bank7->es_script, es_script, sizeof(es_script));
  state->bank7->es_script_ptr  = state->bank7->es_script;
  state->bank7->es_frame_count = 1;
  state->bank7->es_handler     = es_handler_idle;
  state->bank7->es_input_mask  = 0;

  outer_count = 6;

  for (;;) {
    CHECK_HOST_QUIT(state);

    state->speccy->stamp(state->speccy);

    es_play_music_48k(state);
    if (--state->bank7->es_frame_count == 0)
      run_script(state);
    state->bank7->es_handler(state);

    state->speccy->sleep(state->speccy, END_SCREEN_TSTATES);

    if (--outer_count != 0)
      continue;
    outer_count = 5;

    drive_chatter(state);
    A_input = keyscan(state);
    if (!(A_input & USERINPUTFLAG_FIRE))
      continue;

    if (state->bank7->es_input_mask == 0) {
      /* First fire press: skip ahead to the congratulations script. */
      state->bank7->es_input_mask  = 1;
      state->bank7->es_frame_count = 1; /* $E04F: run the script next frame */
      state->bank7->es_script_ptr =
        &state->bank7->es_script[ES_SCRIPT_CONGRATS_OFFSET]; /* $E052 */
      drive_chatter_stop(state);
      /* Conv: the Z80 spins here on the keyboard alone. The C port must pace
       * the wait -- an unpaced poll runs the game thread flat out and the host
       * never gets a frame -- and honour the quit request while the player is
       * still holding fire. */
      while (keyscan(state) & USERINPUTFLAG_FIRE) {
        CHECK_HOST_QUIT(state);

        state->speccy->stamp(state->speccy);
        state->speccy->sleep(state->speccy, END_SCREEN_TSTATES);
      }
      continue;
    }

    /* Second fire press: leave the end screen. */
    break;
  }

  drive_chatter_stop(state);
}

/**
 * $E01E: Set up bank 7's own interrupt handler and 48K music engine
 *
 * TODO: not yet ported. Bank 7 carries its own copy of the 48K sound code
 * (es_setup_interrupts onward in the skool), relocated into the copied
 * $F300 buffer.
 */
static void es_setup_interrupts(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $E20A: Run the current end-screen script
 *
 * Reads and dispatches script command bytes from state->es_script_ptr in a
 * DEC A/JP Z chain matching the ESCMD_* constants above. "Immediate" commands
 * (draw frame, call word, draw score) run their handler stub straight away
 * and loop for the next command in the same call; all other commands instead
 * arm state->es_handler/es_frame_count via es_set_dispatch and return,
 * leaving show_end_screen's per-frame loop to invoke the handler on a delay.
 *
 * ESCMD_DRAW_TEXT_NO_CLEAR_VAL/ESCMD_DRAW_TEXT_VAL (render_text/plot_char) also run immediately, drawing
 * their text run within this same call rather than arming a per-frame
 * handler -- see render_text_common and plot_char above.
 *
 * An unrecognised command byte resets HL_script to ES_SCRIPT_RESET_OFFSET
 * and loops rather than returning, matching skool $E251.
 */
static void run_script(chqstate_t *state)
{
  const u8 *HL_script; /* script program counter (was HL) */
  u8        A_cmd;     /* command byte just read (was A) */
  u8        C_reload;  /* frame-delay reload value about to be applied (was C) */

  HL_script = state->bank7->es_script_ptr;

  for (;;) {
    A_cmd = *HL_script++;

    /* Conv: es_chatter reads/advances state->bank7->es_script_ptr directly
     * rather than taking &HL_script like the other handlers, so it must see
     * the pointer already advanced past A_cmd before it runs. */
    state->bank7->es_script_ptr = HL_script;

    switch (A_cmd) {
    case ESCMD_CLEAR_DRAW_FRAME_VAL:
      es_clear_then_draw_frame(state, &HL_script);
      continue;

    case ESCMD_DRAW_WORD_VAL:
      /* $E2DE is $E2D9's tail half, entered directly for this command
       * (skipping E2D9's own backbuffer-clear prefix). */
      es_draw_frame_common(state, &HL_script);
      continue;

    case ESCMD_FADE_IN_A_VAL:
      es_set_dispatch(state, es_attribute_fade_in, 16);
      goto rs_exit;

    case ESCMD_FADE_IN_B_VAL:
      es_set_dispatch(state, es_handler_glyph_fade_b, 16);
      goto rs_exit;

    case ESCMD_HANDSHAKE_VAL:
      es_set_dispatch(state, es_handler_handshake, 16);
      goto rs_exit;

    case ESCMD_FADE_IN_C_VAL:
      es_set_dispatch(state, es_handler_glyph_fade_c, 32);
      goto rs_exit;

    case ESCMD_IDLE_VAL:
      C_reload = *HL_script++;
      es_set_dispatch(state, es_handler_idle, C_reload);
      goto rs_exit;

    case ESCMD_RESET_HANDSHAKE_VAL:
      C_reload = *HL_script++;
      state->bank7->es_handshake_index = 0; /* $E2C0 LD ($A172),A with A=0 */
      es_set_dispatch(state, es_handler_handshake_advance, C_reload);
      goto rs_exit;

    case ESCMD_HANDSHAKE_AGAIN_VAL:
      C_reload = *HL_script++;
      es_set_dispatch(state, es_handler_handshake_advance, C_reload);
      goto rs_exit;

    case ESCMD_DRAW_TEXT_NO_CLEAR_VAL:
      /* $E2F5 entered directly: no backbuffer-clear prefix. */
      render_text_common(state, &HL_script);
      continue;

    case ESCMD_DRAW_TEXT_VAL:
      es_handler_render_text(state, &HL_script);
      continue;

    case ESCMD_CHATTER_VAL:
      es_chatter(state);
      HL_script = state->bank7->es_script_ptr; /* es_chatter advanced it directly */
      continue;

    case ESCMD_DRAW_SCORE_VAL:
      es_handler_draw_score(state);
      continue;

    default:
      /* Unrecognised command: skool $E251 resets HL to the CHATTER(0x5C78)
       * command rather than stopping -- see ES_SCRIPT_RESET_OFFSET. */
      HL_script = &state->bank7->es_script[ES_SCRIPT_RESET_OFFSET];
      continue;
    }
  }

rs_exit:
  state->bank7->es_script_ptr = HL_script;
}

/**
 * $E256: Tally the bonus into the score, then draw the final score text
 *
 * First runs a 1000-iteration bonus-tally animation: each iteration adds
 * 5,000 to the score (increment_score), redraws the scoreboard LED digits
 * (ptad_led_digits, same call as the in-game scoreboard update) and plays one
 * "bipbow" tick (sfx_bipbow), giving the classic arcade rising-score sound
 * and visual. Sets es_input_mask so the very next fire press exits the end
 * screen (see Bank7State.h). Then formats the final score as ASCII -- most
 * significant BCD byte first, blanking leading zeros -- directly into the
 * "GBP________ PTS" placeholder already sitting in state->bank7->es_script
 * (offset 0xFD, $5DFB relocated), so the later draw-text command renders it.
 *
 * Conv: the leading-zero blank/print decision (Z80: RLC C carry chain) is
 *       modelled as a sticky "seen a non-blank digit yet" flag: print the digit
 *       if it is non-zero or a digit has already been printed, otherwise print
 *       a space.
 */
static void es_handler_draw_score(chqstate_t *state)
{
  int       tally;    /* bonus-tally animation iteration counter (was BC) */
  const u8 *DE_bcd;   /* packed-BCD score pointer, walked backwards (was DE) */
  u8       *HL_dst;   /* destination ASCII bytes in es_script (was HL) */
  u8        C_seen;   /* sticky "non-blank digit already printed" flag (was C) */
  int       pair;     /* BCD byte-pair iteration counter (was B) */
  u8        A_nibble; /* nibble being converted to ASCII (was A) */

  for (tally = 1000; tally != 0; tally--) {
    /* Conv: the tally runs for ~11s; without this the window stays up for the
     * remainder of it after the host asks to quit. */
    CHECK_HOST_QUIT(state);

    state->speccy->stamp(state->speccy);

    increment_score(state, 0, 0x00, 0x50);
    ptad_led_digits(state, 4, &state->score_bcd[3],
                    &state->session.score_digits[7], ADDRTOSCREEN(0x4126));
    sfx_bipbow(state, 2, 2);

    // Sleeps out the whole per-increment body timed from the stamp() above,
    // not just sfx_bipbow's delay loops (see SCORE_TALLY_TSTATES). Without
    // this the 1000-iteration tally runs in zero real time and the classic
    // rising-score animation is never seen.
    state->speccy->sleep(state->speccy, SCORE_TALLY_TSTATES);
  }

  state->bank7->es_input_mask = 1;

  DE_bcd = &state->score_bcd[3];
  HL_dst = &state->bank7->es_script[0xFD];
  C_seen = 0;

  for (pair = 4; pair != 0; pair--) {
    A_nibble = (*DE_bcd >> 4) & 0x0F;
    if (A_nibble != 0 || C_seen != 0) {
      C_seen    = 0xFF;
      *HL_dst++ = '0' + A_nibble;
    } else {
      *HL_dst++ = ' ';
    }

    A_nibble = *DE_bcd & 0x0F;
    if (A_nibble != 0 || C_seen != 0) {
      C_seen    = 0xFF;
      *HL_dst++ = '0' + A_nibble;
    } else {
      *HL_dst++ = ' ';
    }

    DE_bcd--;
  }

  HL_dst[-1] |= EOS;
}

/**
 * Resolve a script-embedded argument word to its C data array.
 *
 * Conv: as with z80addrtoendshot, es_script only ever encodes one literal value
 *       here ($5C6E, pre-relocation for data_e06e at post-relocation $E06E via
 *       the bank's uniform +0x8400 rule), so a small lookup replaces pointer
 *       arithmetic into relocated bank memory the C port does not model
 *       byte-for-byte.
 *
 * \param[in] addr Raw address word read from the script (was HL after EX DE,HL
 *                 at $E2B7).
 * \return         Matching data block, or NULL if unrecognised.
 */
static const u8 *z80addrtochatterblk(u16 addr)
{
  switch (addr) {
  case CHATTERBLK_NANCY_CONGRATULATES_ADDR:
    return &chatterblk_nancy_congratulates[0];
  case CHATTERBLK_PRESS_GEAR_ADDR:
    return &chatterblk_press_gear[0];
  default:
    assert(0);
    return NULL;
  }
}

/**
 * $E2B2: Call a script-supplied handler with a literal argument
 *
 * Reads a 2-byte little-endian pointer word from the script, advances the
 * script pointer past it, then calls start_chatter with priority 1 (Z80
 * sets A=0 then INC A immediately before CALL $9945) and the resolved
 * pointer. Runs immediately (loops back into run_script rather than
 * returning).
 *
 * Conv: data_e06e (the only live target) is NOT a standard {CHATTERCHR,
 *       CHATTERSTR, CHATTERCMD} chatterblk -- see the comment on data_e06e in
 *       Bank7Data.h. Byte 2 of that block ($3F = 63) would be consumed as a
 *       CHATTERSTR index by pc_chatter_message (Main.c, the "assert(*chatterblk
 *       < CHATTERSTR__LIMIT)" guard around line 5871) and fail that bounds
 *       check immediately -- CHATTERSTR__LIMIT is 36. In a release build
 *       without asserts this reads common_chatter_strings[63] out of its
 *       36-entry array and dereferences whatever garbage pointer turns up, i.e.
 *       every single playthrough would crash on reaching the end screen. The
 *       original Z80 has the same malformed data, so this path is presumed
 *       unreached in practice (the skool marks it "unproven, dead end"); rather
 *       than risk that byte-for-byte here, the start_chatter call is skipped
 *       whenever the resolved target does not look like a well-formed
 *       chatterblk.
 *
 * \param[in,out] state Pointer to game state; state->bank7->es_script_ptr is
 *                      read and advanced past the word consumed.
 */
static void es_chatter(chqstate_t *state)
{
  const u8 *HL_script_ptr; /* script read pointer (was HL) */
  u16       target_addr;   /* raw argument word read from the script (was DE/HL) */
  const u8 *chatterblk;    /* resolved data block pointer (was HL after CALL $9945 setup) */

  HL_script_ptr = state->bank7->es_script_ptr;

  target_addr = wordat(HL_script_ptr);
  HL_script_ptr += 2;

  chatterblk = z80addrtochatterblk(target_addr);
  start_chatter(state, 1, chatterblk);

  state->bank7->es_script_ptr = HL_script_ptr;
}

/**
 * $E2CD: Set the per-frame handler and its frame-delay reload
 *
 * Common tail shared by the run_script commands that hand off to a
 * self-modified per-frame handler rather than running immediately: stores
 * the new handler and reload count, ready for show_end_screen's loop to
 * count down and re-invoke run_script when it reaches zero.
 *
 * \param[in] handler New per-frame handler (was DE).
 * \param[in] reload  New $A170 frame-delay reload count (was C).
 */
static void es_set_dispatch(chqstate_t *state,
                            void (*handler)(chqstate_t *state),
                            u8 reload)
{
  state->bank7->es_handler     = handler;
  state->bank7->es_frame_count = reload;
}

/**
 * Map an ASCII character to its glyph index in font[]
 *
 * $E328-$E356 width-class ladder. Instruction-for-instruction identical to
 * draw_char's ($9FEC, Main.c) dc_have_range/dc_have_single mapping -- see
 * that function for the equivalent structure. Reused here (rather than
 * reinvented) because both routines index the same font[41*7] table.
 *
 * \param[in] character ASCII character, already offset by ' ' (was A after SUB
 *                      $20; space and 0 are handled by the caller).
 * \return              Glyph index into font[] (multiply by 7 for the row
 *                      pointer).
 */
static int ascii_to_glyph_id(int character)
{
  int glyphid; /* glyph index accumulator (was C) */

  glyphid = 0x12;
  if (character >= ('A' - ' ')) goto have_range;
  glyphid = 0x0B;
  if (character >= ('0' - ' ')) goto have_range;
  glyphid = 0;
  character--;
  if (character == 0) goto have_single;
  glyphid++;
  character -= 7;
  if (character == 0) goto have_single;
  glyphid++;
  character--;
  if (character == 0) goto have_single;
  glyphid++;
  character -= 3;
  if (character == 0) goto have_single;
  glyphid++;
  goto have_single;

have_range:
  glyphid = character - glyphid;

have_single:
  return glyphid;
}

/**
 * $E2D8 (stub): Idle per-frame handler (no drawing)
 *
 * Does nothing; dispatched between animation beats when a script step has no
 * per-frame work to do.
 */
static void es_handler_idle(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $E2D9: Clear the backbuffer, then draw an end-screen graphic frame
 *
 * Thin wrapper: es_clear followed by es_draw_frame_common.
 *
 * \param[in,out] script Script read pointer (was HL); advanced past the two
 *                       words consumed.
 */
static void es_clear_then_draw_frame(chqstate_t *state, const u8 **script)
{
  es_clear(state);
  es_draw_frame_common(state, script);
}

/**
 * $E2DE: Read an image+destination pair and blit it
 *
 * Reads a bitmap address word and a destination screen address word from
 * the script, then blits the resolved image there. Does not clear the
 * backbuffer first; see es_clear_then_draw_frame for the variant that does.
 *
 * \param[in,out] script Script read pointer (was HL); advanced past the two
 *                       words consumed.
 */
static void es_draw_frame_common(chqstate_t *state, const u8 **script)
{
  const u8 *HL_script;   /* script read pointer (was HL) */
  u16       image_addr;  /* raw bitmap address word read from script */
  u16       screen_addr; /* destination screen address word read from script */

  HL_script = *script;

  image_addr  = wordat(HL_script);
  screen_addr = wordat(HL_script + 2);
  HL_script += 4;

  draw_endshot(state, z80addrtoendshot(image_addr), screen_addr);

  *script = HL_script;
}

/**
 * $E2F0: Clear the backbuffer, then render an end-screen text run
 *
 * Clears the backbuffer before handing off to render_text_common, used for
 * script commands that redraw the whole end-screen text area from scratch.
 *
 * \param[in,out] script Script read pointer (was HL); advanced as per
 *                       render_text_common.
 */
static void es_handler_render_text(chqstate_t *state, const u8 **script)
{
  es_clear(state);
  render_text_common(state, script);
}

/**
 * $E2F5: Parse and draw a script text-render command
 *
 * Reads a colour byte and a screen destination word from the script (3
 * bytes total), derives the attribute-row address exactly as draw_endshot
 * does, then plots each following script character via plot_char until the
 * EOS-terminated (top-bit-set) character has been drawn.
 *
 * \param[in,out] script Script read pointer (was HL); advanced past the 3-byte
 *                       header and the whole character run.
 */
static void render_text_common(chqstate_t *state, const u8 **script)
{
  const u8 *HL_script; /* script read pointer (was HL) */
  u8        C_attr;    /* attribute byte read from the script (was C) */
  u8        E_scr;     /* screen destination column byte (was E) */
  u8        D_scr;     /* screen destination row byte (was D) */
  u8        H_attr;    /* attribute-row address high byte (was H, $E304) */
  u8        L_attr;    /* attribute address column byte (was L, $E305) */
  u8        raw;       /* raw script byte, EOS bit intact (was (HL) at $E312) */
  u8        A_char;    /* script character byte, EOS bit masked off (was A) */

  HL_script = *script;

  C_attr = *HL_script++;
  E_scr  = *HL_script++;
  D_scr  = *HL_script++;

  H_attr = (u8) ((((D_scr >> 3) | (D_scr << 5)) & 0x03) + 0xEF);
  L_attr = E_scr;

  do {
    raw    = *HL_script;
    A_char = raw & (u8) ~EOS;
    plot_char(state, A_char, D_scr, &E_scr, H_attr, &L_attr, C_attr);
    HL_script++;
  } while ((raw & EOS) == 0);

  *script = HL_script;
}

/**
 * $E31F: Render one end-screen text character
 *
 * Space ($E323-$E327): advances both persistent cursors by one column and
 * draws nothing.
 *
 * Non-space ($E328-$E3A4): looks up the glyph via ascii_to_glyph_id(), then
 * blits it double-height directly to the screen in two passes (font rows
 * 0-3, then 4-6 plus a trailing blank row) -- the same "double height via
 * two separate 4- and 3-row passes with a column advance mid-glyph"
 * structure as draw_char's style==0 case, except writing to real (bank-7)
 * screen addresses rather than a flat backbuffer offset, so the mid-glyph
 * advance has to reproduce the Z80's raw row/column byte arithmetic.
 * Finally stamps the call's colour byte into both glyph-cell attributes and
 * advances the persistent cursor by one column.
 *
 * Conv: register-banking notes, resolved from the original stalled attempt. The
 *       Z80 threads three logically distinct values through nested EXX/stack
 *       shuffles ($E306, $E307/EX (SP),HL, and plot_char's own
 *       $E35B/$E39A-$E3A3 EXX pairs): the script read cursor (HL throughout the
 *       character loop -- modelled as the caller's script pointer, untouched by
 *       plot_char), the persistent column cursor (screen-dest E and attr-addr
 *       L, both threaded here as [in,out] E_col/L_attr), and this character's
 *       own draw position (screen-dest E's PRE-increment value, borrowed via a
 *       PUSH/EXX/POP shuffle at $E35B-$E35F -- modelled here as the local
 *       E_cur, read from *E_col before it is advanced). Everything else the EXX
 *       dance shuffles (Set S's stale/arbitrary BC and DE, pushed and popped
 *       purely to balance the stack) carries no live data and is correctly
 *       omitted.
 *
 * Conv: A_attr (was C, Set M) is NOT the "row count" the ($E2F5) prologue
 *       naming originally suggested. $E399's EXX switches back to the SAME
 *       physical register set read at $E2F5 -- the ladder's own use of C
 *       ($E328-$E356) is a completely different (Set S) C that plot_char's own
 *       LDI calls decrement into irrelevance and never reads back. Confirmed
 *       against es_script: the byte read here for "CONGRATULATIONS!" is
 *       attribute_BRIGHT_WHITE_OVER_BLACK -- a plausible text colour, not a row
 *       count. It survives unclobbered in Set M across the whole render_text
 *       call and is written verbatim into both glyph-cell attributes at
 *       $E399/$E3A0.
 *
 * Conv: $E37A-$E37F (E += $1F, then the pending LDI increment folds in a
 *       further +1, netting E += $20; D -= 7) is NOT the same computation as
 *       next_screen_row() -- it never checks for, or propagates, a carry out of
 *       the column byte into the row byte, unlike next_screen_row's explicit
 *       "did this cross a screen third" branch. Reproduced literally as two
 *       independent 8-bit adds rather than substituting next_screen_row, since
 *       the two are only equivalent when no such carry occurs -- true for every
 *       script-supplied text position in es_script, but not guaranteed in
 *       general.
 *
 * Conv: the attribute-row address computed from D_row (H_attr, range $EF-$F2)
 *       is resolved via ADDRTOSCREEN, not ADDRTOATTRS, matching the established
 *       precedent in draw_endshot's attribute-row loop above -- this bank-7
 *       memory range is not standard $5800-$5AFF attribute space.
 *
 * \param[in]     A_char Script character byte, EOS bit already masked off by
 *                       the caller (was A).
 * \param[in]     D_row  Screen destination row byte; constant for the whole
 *                       render_text call (was D, Set M).
 * \param[in,out] E_col  Screen destination column byte; the persistent cursor,
 *                       advanced by one per character (was E, Set M).
 * \param[in]     H_attr Attribute-row address high byte; constant for the whole
 *                       call (was H, Set M).
 * \param[in,out] L_attr Attribute address column byte; the persistent cursor,
 *                       mirrors *E_col (was L, Set M).
 * \param[in]     A_attr Attribute/colour byte read once from the script at
 *                       $E2F5 and held constant for the whole call
 *                       (was C, Set M) -- see the Conv note above.
 */
static void plot_char(chqstate_t *state,
                      u8          A_char,
                      u8          D_row,
                      u8         *E_col,
                      u8          H_attr,
                      u8         *L_attr,
                      u8          A_attr)
{
  int       character;     /* character code, offset by ' ' (was A) */
  int       glyphid;       /* glyph index into font[] (was C during the ladder) */
  const u8 *HL_font;       /* current font row pointer, walked forward (was HL) */
  u8        E_cur;         /* this character's draw column (was E, Set S) */
  u16       starting_addr; /* glyph's first screen byte, saved for the dirty-box call (Conv: added) */
  u8        D_cur;         /* current screen row byte during the blit (was D) */
  int       i;             /* pass loop index (Conv: no Z80 register) */
  int       data;          /* font byte read for the current scanline pair (was A) */
  u8        L_cur;         /* this character's attribute column (was L) */

  character = A_char - ' ';
  if (character == 0) {
    // Space: $E323-$E327.
    (*E_col)++;
    (*L_attr)++;
    return;
  }

  glyphid = ascii_to_glyph_id(character);
  HL_font  = &font[glyphid * 7];

  E_cur = *E_col;
  starting_addr = (u16) (((u16) D_row << 8) | E_cur);
  (*E_col)++; // $E35D: persistent cursor advances for the NEXT character now.

  // Pass 1 ($E360-$E378): font bytes 0-3, double height.
  D_cur = D_row;
  for (i = 0; i < 4; i++) {
    data = *HL_font++;
    *ADDRTOSCREEN(((u16) D_cur << 8) | E_cur) = (u8) data;
    D_cur++;
    *ADDRTOSCREEN(((u16) D_cur << 8) | E_cur) = (u8) data;
    if (i != 3)
      D_cur++;
  }

  // $E37A-$E381: mid-glyph row-wrap -- see Conv note in the prologue.
  E_cur = (u8) (E_cur + 0x20);
  D_cur = (u8) (D_cur - 7);

  // Pass 2 ($E382-$E398): font bytes 4-6, double height, then a blank row.
  for (i = 0; i < 3; i++) {
    data = *HL_font++;
    *ADDRTOSCREEN(((u16) D_cur << 8) | E_cur) = (u8) data;
    D_cur++;
    *ADDRTOSCREEN(((u16) D_cur << 8) | E_cur) = (u8) data;
    D_cur++;
  }
  *ADDRTOSCREEN(((u16) D_cur << 8) | E_cur) = 0;

  update_screen(state, starting_addr, 8, 16); /* Conv: added */

  // $E399-$E3A4: stamp the call's colour into both glyph-cell attributes.
  L_cur = *L_attr;
  *ADDRTOBACKBUF(((u16) H_attr << 8) | L_cur)               = A_attr;
  *ADDRTOBACKBUF(((u16) H_attr << 8) | (u8) (L_cur + 0x20)) = A_attr;
  *L_attr = (u8) (L_cur + 1);
}

/**
 * $E3B7: Fade the $5C6C attribute band, then advance
 *
 * Fades the attribute band one step (routine_e472's shared tail, called
 * directly rather than duplicated), then runs the handshake animation-advance
 * ($E3BA, es_handler_handshake_advance).
 *
 * This is the entry point ESCMD_HANDSHAKE dispatches to; ESCMD_RESET_HANDSHAKE
 * and ESCMD_HANDSHAKE_AGAIN dispatch to es_handler_handshake_advance directly,
 * skipping this fade-b call ($5FBA vs $5FB7 in the relocated dispatch table).
 */
static void es_handler_handshake(chqstate_t *state)
{
  es_handler_glyph_fade_b(state);
  es_handler_handshake_advance(state);
}

/**
 * $E3BA: Advance the handshake animation frame
 *
 * Rotates the gate byte at $5C6D: when its old top bit was clear, the
 * animation-advance block below is skipped entirely; otherwise the 0-5
 * ping-pong frame index ($A172) advances into handshake_frames, that frame's
 * rows are LDIR'd to screen $48AC (8 bytes/row, wraparound-stepped via
 * next_screen_row), and 3 further 8-byte rows are zero-filled to pad every
 * frame out to a fixed height. Either way, finishes by stamping a fixed
 * 5-group x 8-byte decorative attribute pattern at $59AC.
 *
 * This is the entry point ESCMD_RESET_HANDSHAKE and ESCMD_HANDSHAKE_AGAIN
 * dispatch to directly ($5FBA in the relocated table), skipping the $5C6C
 * fade-b call that only the plain ESCMD_HANDSHAKE entry point runs.
 */
static void es_handler_handshake_advance(chqstate_t *state)
{
  int       carry;     /* carry flag set by RLC (carry) */
  u8        A_index;   /* frame index 0..5, wrapped (was A/B) */
  const u8 *HL_image;  /* handshake bitmap source, walked forward (was HL) */
  u16       DE_screen; /* screen destination address (was DE) */
  int       row;       /* bitmap row counter for this frame (was B) */
  int       blank;     /* blank-row counter, 3 down to 0 (was C) */
  u8       *HL_attr;   /* decorative attribute cell (was HL) */
  int       group;     /* decorative attribute group counter, 5 down to 0 (was C) */

  RLC(state->bank7->es_fade_gate_c);
  if (carry) {
    A_index = state->bank7->es_handshake_index;
    state->bank7->es_handshake_index = (u8) ((A_index + 1 == 6) ? 0 : A_index + 1);

    HL_image = handshake_frames[A_index].image;
    DE_screen = 0x48AC;

    for (row = handshake_frames[A_index].rows; row != 0; row--) {
      memcpy(ADDRTOSCREEN(DE_screen), HL_image, 8);
      HL_image += 8;
      DE_screen = next_screen_row(DE_screen);
    }

    for (blank = 3; blank != 0; blank--) {
      memset(ADDRTOSCREEN(DE_screen), 0, 8);
      DE_screen = next_screen_row(DE_screen);
    }
  }

  HL_attr = ADDRTOATTRS(0x59AC);
  for (group = 5; group != 0; group--) {
    memset(HL_attr, attribute_WHITE_OVER_BLACK, 8);
    HL_attr += SCREEN_ATTRIBUTES_WIDTH;
  }

  update_attrs(state, 0x59AC, 8 * 8, 5 * 8); /* Conv: added */
}

/**
 * $E42E: Sweep attributes toward the target colours
 *
 * Gate: only runs every other call (RLC flip-flops $5C6C; returns
 * immediately when the old top bit was set). When it runs, walks all 512
 * attribute cells $5900-$5AFF against the corresponding backbuffer bytes at
 * $F000-$F1FF (the glyph shapes rasterised there by other code): cells with
 * the BRIGHT bit set are left untouched; cells whose masked colour already
 * matches the backbuffer target are copied verbatim; every other cell steps
 * its ink and paper fields one unit toward the target. Called repeatedly
 * this produces a gradual colour reveal as glyphs are plotted into the
 * backbuffer over several frames.
 *
 * Conv: the ink-field increment (`INC C`, $E45D) and paper-field increment
 *       (`ADD A,$08`, $E468) are not masked back into their 3-bit fields -- u8
 *       wraparound reproduces this bug-for-bug.
 */
static void es_attribute_fade_in(chqstate_t *state)
{
  int carry;    /* carry flag set by RLC (carry) */
  u8 *HL_attr;  /* current attribute cell (was HL) */
  u8 *DE_back;  /* current backbuffer cell (was DE) */
  int c;        /* attribute cell counter, 512 down to 0 (was H reaching $5B) */
  u8  A_target; /* masked target colour read from the backbuffer (was A) */
  u8  A_paper;  /* working accumulator (was A) */
  u8  B_target; /* masked target colour read from the backbuffer (was B) */
  u8  C_ink;    /* merged ink field (was C) */

  RLC(state->bank7->es_fade_gate_ab);
  if (carry)
    return;

  HL_attr = ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR);
  DE_back = ADDRTOBACKBUF(BACKBUFFER_START_ADDRESS);

  for (c = SCREEN_ATTRIBUTES_WIDTH * PLAYFIELD_HEIGHT / 8; c != 0; c--, HL_attr++, DE_back++) {
    if (*HL_attr & ATTR_BRIGHT) /* BRIGHT set: leave this cell untouched */
      continue;

    A_target = *DE_back & (ATTR_INK_MASK | ATTR_PAPER_MASK);
    if (A_target == *HL_attr) {
      *HL_attr = *DE_back;
      continue; // already there
    }

    B_target = A_target;

    C_ink = *HL_attr & ATTR_INK_MASK;
    if ((B_target & ATTR_INK_MASK) != C_ink)
      C_ink++;

    A_paper = *HL_attr & ATTR_PAPER_MASK;
    if ((B_target & ATTR_PAPER_MASK) != A_paper)
      A_paper = (u8) (A_paper + 0x08);

    *HL_attr = (u8) (A_paper | C_ink);
  }

  update_attrs(state, SCREEN_PLAYFIELD_ATTRS_ADDR, SCREEN_WIDTH, PLAYFIELD_HEIGHT); /* Conv: added */
}

/**
 * $E46D: Fade the $5C6D-gated glyph attribute band
 *
 * Thin wrapper: es_attribute_fade_out against es_fade_gate_c.
 */
static void es_handler_glyph_fade_c(chqstate_t *state)
{
  es_attribute_fade_out(state, &state->bank7->es_fade_gate_c);
}

/**
 * $E472: Fade the $5C6C-gated glyph attribute band
 *
 * Thin wrapper: es_attribute_fade_out against es_fade_gate_ab.
 */
static void es_handler_glyph_fade_b(chqstate_t *state)
{
  es_attribute_fade_out(state, &state->bank7->es_fade_gate_ab);
}

/**
 * $E475: Shared fade-to-black tail for routine_e472/routine_e46d
 *
 * Sweeps the same 512-cell attribute band as es_attribute_fade_in,
 * decrementing each cell's ink field by 1 (floor 0) and paper field by one
 * unit (floor 0) every call it runs. Gated by rlc8 on *flag -- $5C6C for
 * routine_e472 (GLYPH_B, also called directly by the handshake handler),
 * $5C6D for routine_e46d (GLYPH_C).
 *
 * Conv: unlike es_attribute_fade_in, BRIGHT/FLASH are never tested here -- the
 *       original ANDs each byte down to its ink/paper fields before OR-ing them
 *       back together, which drops those bits on every write. Matched
 *       bug-for-bug.
 *
 * \param[in] flag Flip-flop gate byte to rotate (was HL -> $5C6C/$5C6D).
 */
static void es_attribute_fade_out(chqstate_t *state, u8 *flag)
{
  int carry;     /* carry flag set by RLC (carry) */
  u8 *HL_pattrs; /* current attribute cell (was HL) */
  int c;         /* attribute cell counter, 512 down to 0 (was D pages) */
  u8  A_attr;    /* attribute cell (was A) */
  u8  B_ink;     /* new ink field (was B) */
  u8  A_paper;   /* working accumulator (was A) */

  RLC(*flag);
  if (!carry)
    return;

  HL_pattrs = ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR);

  for (c = SCREEN_ATTRIBUTES_WIDTH * PLAYFIELD_HEIGHT / 8; c != 0; c--, HL_pattrs++) {
    A_attr = *HL_pattrs;
    if (A_attr == 0)
      continue;

    B_ink = A_attr & ATTR_INK_MASK;
    if (B_ink != 0)
      B_ink--;

    A_paper = A_attr & ATTR_PAPER_MASK;
    if (A_paper != 0)
      A_paper = (u8) (A_paper - 0x08);

    *HL_pattrs = (u8) (A_paper | B_ink);
  }

  update_attrs(state, SCREEN_PLAYFIELD_ATTRS_ADDR, SCREEN_WIDTH, PLAYFIELD_HEIGHT); /* Conv: added */
}

/**
 * $E499: Clear the playfield ready for the end screen
 *
 * Zeros the on-screen playfield (attributes and bitmap) plus the first 512
 * bytes of the "backbuffer" area, which alias the attribute portion of the
 * end-screen montage/glyph drawing area ($F000 = &state->backbuffer[0]).
 */
static void es_clear(chqstate_t *state)
{
  clear_playfield(state);
  memset(&state->backbuffer[0], 0, 512);
}

/* Script command bytes, $E20D's DEC A/JP Z chain (1-based, in read order). */

/**
 * Advance a raw Z80 screen address by one character-cell row.
 *
 * Common wraparound arithmetic shared by draw_endshot ($E4A9) and
 * routine_e3b7's handshake blit ($E3E9-$E3F8, $E407-$E416): within a
 * character row D climbs through its low 3 bits (one pixel scanline per
 * call); when that wraps, E jumps on by 32 (next character row) and D drops
 * back by 8 unless E itself carried into the next screen third.
 *
 * \param[in] addr Current screen address (was DE).
 * \return         Screen address one row down.
 */
static u16 next_screen_row(u16 addr)
{
  u8 D_hi; /* screen address high byte after +1 scanline (was D via A) */
  u8 E_lo; /* screen address low byte after +32 column step (was E via A) */

  D_hi = (u8) ((addr >> 8) + 1);
  if ((D_hi & 0x07) != 0)
    return (u16) ((D_hi << 8) | (addr & 0xFF));

  E_lo = (u8) ((addr & 0xFF) + 0x20);
  if (E_lo < 0x20) /* carry out of E: stay in the next screen third */
    return (u16) ((D_hi << 8) | E_lo);
  else
    return (u16) (((D_hi - 0x08) << 8) | E_lo);
}

/**
 * $E4A9: Blit an end-game montage shot to the screen
 *
 * Copies a 13-byte-wide bitmap, 64 rows tall, from image into the screen at
 * screen_addr, then 8 rows of attribute bytes into the corresponding
 * attribute third. Row addressing mimics the Z80's raw D/E screen-address
 * increment: within a character row D climbs through its low 3 bits: when
 * that wraps, E jumps on by 32 (next character column pair... actually next
 * character row) and D drops back by 8 unless E itself carried into the next
 * screen third.
 *
 * Conv: operates on the raw 16-bit Z80 screen address (screen_addr) and calls
 *       ADDRTOSCREEN per row, rather than walking a pre-resolved C pointer, so
 *       the row-wrap arithmetic can mirror the Z80 exactly.
 *
 * \param[in] image       Bitmap+attribute source blob (was HL).
 * \param[in] screen_addr Top-left destination screen address (was DE).
 */
static void draw_endshot(chqstate_t *state, const u8 *image, u16 screen_addr)
{
  int row;            /* bitmap row counter, 64 down to 1 (was B) */
  u16 DE_screen_addr; /* current screen row address (was DE) */
  u8  D_attr;         /* attribute row address high byte (was D after RRCA x3) */
  u16 attraddr;       /* current attribute row address (was DE in the attr loop) */
  int attrrow;        /* attribute row counter, 8 down to 1 (was A) */

  DE_screen_addr = screen_addr;

  for (row = ENDSHOT_HEIGHT; row != 0; row--) {
    memcpy(ADDRTOSCREEN(DE_screen_addr), image, ENDSHOT_WIDTH / 8);
    image += ENDSHOT_WIDTH / 8;
    DE_screen_addr = next_screen_row(DE_screen_addr);
  }

  /* Conv: skool POPs DE here, restoring the original destination pushed at
   * function entry -- NOT the row loop's final DE_screen_addr. Must use the
   * screen_addr parameter, which the loop above never mutates. */
  D_attr    = (u8) (screen_addr >> 8); /* original D, pre-rotate */
  D_attr    = (u8) ((((D_attr >> 3) | (D_attr << 5)) & 0x03) + 0xEF);
  attraddr = (u16) ((D_attr << 8) | (screen_addr & 0xFF));

  for (attrrow = 8; attrrow != 0; attrrow--) {
    memcpy(ADDRTOBACKBUF(attraddr), image, 13);
    image += 13;
    /* Conv: the skool's LDIR ($E4D6) advances DE by 13 as a side effect of
     * the copy itself, then adds a further 19 to skip the remaining columns
     * of the 32-byte attribute row (skool comment at $E4D8: "13+19 = 32").
     * memcpy has no such side effect on attraddr, so both parts of that
     * total must be added explicitly here. */
    attraddr = (u16) (attraddr + 13 + 19);
  }

  update_screen(state, screen_addr, ENDSHOT_WIDTH, ENDSHOT_HEIGHT); /* Conv: added */
}

/**
 * Resolve a script-embedded end-shot bitmap address to its C data array.
 *
 * Conv: the original walks a real (relocated) Z80 pointer; es_script only ever
 *       encodes these four literal addresses (see the
 *       ESCMD_CLEAR_DRAW_FRAME_VAL entries above), so a small lookup replaces
 *       pointer arithmetic into relocated bank memory the C port does not model
 *       byte-for-byte.
 *
 * \param[in] addr Raw address word read from the script (was HL).
 * \return         Matching bitmap_endshot_N array.
 */
static const u8 *z80addrtoendshot(u16 addr)
{
  switch (addr) {
  case BITMAP_ENDSHOT_1_ADDR: return &bitmap_endshot_1[0];
  case BITMAP_ENDSHOT_2_ADDR: return &bitmap_endshot_2[0];
  case BITMAP_ENDSHOT_3_ADDR: return &bitmap_endshot_3[0];
  case BITMAP_ENDSHOT_4_ADDR: return &bitmap_endshot_4[0];
  default:                    assert(0); return NULL;
  }
}

/**
 * $F300: Start the end-screen beatbox
 *
 * Clears the three playback flags that carry state across ticks
 * (drum_active, extra_delay, started) then loads the first pattern in
 * es_music_patterns, exactly as Main.c's reset_music does for the shared
 * in-game engine.
 */
static void es_reset_music(chqstate_t *state)
{
  state->bank7->es_music.drum_active = 0;
  state->bank7->es_music.extra_delay = 0;
  state->bank7->es_music.started     = 0;
  es_next_pattern_at_addr(state, &es_music_patterns[0]);
}

/**
 * $F310: Advance to the next pattern once the current one's repeats expire
 *
 * Decrements pattern_repeats and returns immediately while repeats remain;
 * once it reaches zero, loads the pattern whose address follows the one
 * just played. Same structure as Main.c's next_pattern, operating on bank
 * 7's own es_music state.
 */
static void es_advance_pattern(chqstate_t *state)
{
  if (--state->bank7->es_music.pattern_repeats)
    return;
  es_next_pattern_at_addr(state, state->bank7->es_music.pattern_addr);
}

/**
 * $F318: Load the pattern at pattern_addr into bank 7 music state
 *
 * Reads the pattern's repeat count; $FF marks the end of the pattern list
 * and restarts from the word-pointer that follows it (an offset from
 * es_music_patterns' own base, $F53C when relocated -- see
 * z80addrtoendshot for the same "es_script only ever encodes one literal
 * table base" reasoning). Otherwise stores the repeat count and advances
 * pattern_addr past it, reads the one-byte offset into es_music_data,
 * and primes note_delay/note_delay_reload/pattern_start_ptr from the note
 * stream at that offset.
 *
 * Same structure as Main.c's next_pattern_at_addr, operating on bank 7's own
 * es_music_patterns/es_music_data tables and es_music state instead of the
 * shared in-game music engine's.
 *
 * \param[in] HL_pataddr Pattern-list read pointer (was HL).
 */
static void es_next_pattern_at_addr(chqstate_t *state, const u8 *HL_pataddr)
{
  int       A_n_repeats; /* pattern repeat count just read, or 0xFF end marker (was A) */
  int       C_offset;    /* offset into es_music_data for this pattern's notes (was C) */
  const u8 *HL_data;     /* es_music_data read pointer, walked past the note-delay byte (was HL) */

  for (;;) {
    A_n_repeats = *HL_pataddr++;
    if (A_n_repeats != 0xFF) {
      // Not end of pattern(s)
      state->bank7->es_music.pattern_repeats = (u8) A_n_repeats;
      C_offset = *HL_pataddr++;
      state->bank7->es_music.pattern_addr = HL_pataddr;

      // Calculate address of music data
      HL_data = &es_music_data[C_offset];
      state->bank7->es_music.note_delay_reload = state->bank7->es_music.note_delay = *HL_data++;
      state->bank7->es_music.pattern_start_ptr = HL_data;
      return;
    } else {
      // Restart
      HL_pataddr = &es_music_patterns[wordat(HL_pataddr) - 0xF53C];
    }
  }
}

/**
 * $F340: Service the end-screen beatbox for the current frame
 *
 * Per-tick music driver for bank 7's own 48K music engine, almost identical
 * to Main.c's play_music_48k operating on es_music/es_music_patterns/
 * es_music_data instead of the shared in-game engine's tables. Either
 * initialises playback on the first call or decrements the note delay
 * counter; when the delay expires, reads the next byte from the pattern
 * stream (a terminating byte of 1 advances to the next pattern via
 * es_advance_pattern). Bytes with bit 7 set carry a one-tick extra delay
 * flag; the lower three bits of the remaining byte select the instrument
 * (0 = silence, 1 = drum 2, 2 = drum 1, 3 = noise).
 *
 * Conv: the Z80 clears an interrupt flag ($F3BC) on entry then, once its own
 *       processing is done, busy-waits on that flag in a loop
 *       (b7pm_wait_for_interrupt) until the next interrupt sets it -- this is
 *       how the routine paces itself to one call per frame. show_end_screen's
 *       loop already paces each call via state->speccy->sleep, so both the flag
 *       and the wait loop are omitted; this function represents one
 *       already-paced tick, same as Main.c's play_music_48k.
 *
 * Conv: the Z80 checks es_input_mask ($A16F) first and, if it is non-zero (the
 *       player has pressed fire once already, skipping ahead to the
 *       congratulations script), jumps straight to the wait-for-interrupt loop
 *       -- i.e. does no music processing at all that tick. C returns
 *       immediately in that case.
 */
static void es_play_music_48k(chqstate_t *state)
{
  int       A_delay;      /* note_delay-1; tests whether the current note's delay has expired (was A) */
  const u8 *HL_data;      /* pattern byte-stream read pointer (was HL) */
  int       A_n_note;     /* raw music byte minus 1; zero marks the end-of-pattern sentinel (was A) */
  int       D_note;       /* adjusted music byte: delay bit consumed, upper bits = param, lower 3 = instrument (was D) */
  int       B_instrument; /* instrument index: lower 3 bits of D_note (was B) */
  int       A_param;      /* pitch/parameter value passed to the instrument handler (was A) */

  if (state->bank7->es_input_mask != 0)
    return; // Conv: wait-for-interrupt loop is a no-op here (see prologue)

  if (state->bank7->es_music.started == 0) {
    state->bank7->es_music.started = 1;
    goto pm_reset_pattern;
  }

  A_delay = state->bank7->es_music.note_delay - 1;
  if (A_delay) {
    state->bank7->es_music.note_delay = A_delay;
  } else {
    state->bank7->es_music.note_delay = state->bank7->es_music.note_delay_reload;
    HL_data = state->bank7->es_music.data_ptr;

    for (;;) {
      A_n_note = *HL_data - 1;
      if (A_n_note)
        break;

      es_advance_pattern(state);

pm_reset_pattern:
      HL_data = state->bank7->es_music.pattern_start_ptr;
      state->bank7->es_music.data_ptr = HL_data;
    }

    state->bank7->es_music.data_ptr = ++HL_data;
    if (++A_n_note > NOTE_XDELAY_FLAG) {
      // A byte of the form 0b1aaaaiii (1 is the delay flag bit)
      A_n_note &= ~NOTE_XDELAY_FLAG;
      state->bank7->es_music.note_delay = 1;
      state->bank7->es_music.extra_delay = 1;
    }

    D_note       = A_n_note;
    B_instrument = D_note & NOTE_INST_MASK;
    if (B_instrument) {
      A_param = D_note >> 3;
      switch (B_instrument) {
      case NOTE_DRUM2_VAL: es_playdrum_2(state, A_param); return;
      case NOTE_DRUM1_VAL: es_playdrum_1(state, A_param); return;
      case NOTE_NOISE_VAL: es_play_noise(state, A_param); return;
      }
    }
  }

  if (state->bank7->es_music.extra_delay) {
    state->bank7->es_music.note_delay--;
    state->bank7->es_music.extra_delay--;
  }

  // Conv: b7pm_start_drums/b7pm_wait_for_interrupt -- resuming a drum sample
  // suspended by a real Z80 interrupt has no equivalent here; playdrum
  // playback (once ported) will always run to completion within one call.
}

/**
 * $F3CA: Play bank 7 drum sample 2 for the current tick
 *
 * Starts playback of drum sample 2 (94 bytes). Records the drum speed and
 * marks the drum as active, then calls es_playdrum_go to output it.
 * Analogous to Main.c's playdrum_2/playdrum_start.
 *
 * \param[in] A_speed Playback speed: inner loop count per sample byte (was A).
 */
static void es_playdrum_2(chqstate_t *state, int A_speed)
{
  state->bank7->es_music.drum_speed  = A_speed;
  state->bank7->es_music.drum_active = 1;
  es_playdrum_go(state, sizeof(state->bank7->es_drum2), &state->bank7->es_drum2[0]);
}

/**
 * $F3D1: Play bank 7 drum sample 1 for the current tick
 *
 * Starts playback of drum sample 1 (160 bytes). Records the drum speed and
 * marks the drum as active, then calls es_playdrum_go to output it.
 * Analogous to Main.c's playdrum_1/playdrum_start.
 *
 * \param[in] A_speed Playback speed: inner loop count per sample byte (was A).
 */
static void es_playdrum_1(chqstate_t *state, int A_speed)
{
  state->bank7->es_music.drum_speed  = A_speed;
  state->bank7->es_music.drum_active = 1;
  es_playdrum_go(state, sizeof(state->bank7->es_drum1), &state->bank7->es_drum1[0]);
}

/**
 * $F504: Play bank 7's noise instrument for the current tick
 *
 * Bank 7's own copy of Main.c's play_noise routine, decoded by hand from
 * the raw bytes at source $F9F3-$FA2A (56 bytes) after the disassembler
 * mislabelled the region as data. Confirmed identical to the shared
 * routine, operating on the same fixed-address state->rng_seed -- so
 * rather than duplicate it, this calls the shared implementation directly.
 *
 * \param[in] A_param Noise duration: outer loop count and pulse timing (was A).
 */
static void es_play_noise(chqstate_t *state, int A_param)
{
  play_noise(state, A_param); /* tail call */
}

/**
 * $F8C5: Output bank 7 PCM drum sample
 *
 * Only the es_playdrum_2/es_playdrum_1 entry points at $F3CA/$F3D1 and the
 * sample tables were decoded this pass.
 *
 * Outputs a PCM drum sample byte-by-byte to the speaker port. For each
 * sample byte, an inner loop runs drum_speed iterations; each iteration
 * writes bit 7 of the current sample byte to the EAR bit of
 * port_BORDER_EAR_MIC, then rotates the sample byte left in-place (RLC) so
 * successive iterations output successive bits -- 1-bit PCM at drum_speed
 * bits per byte. When all [D_length] bytes have been output, drum_active is
 * cleared. Identical in structure to Main.c's playdrum_go, operating on
 * bank 7's own es_music state and es_drum2/es_drum1 buffers.
 *
 * \param[in] D_length Number of sample bytes remaining to output (was D).
 * \param[in] HL_data  Pointer to the next sample byte in
 *                     state->bank7->es_drum2[] or state->bank7->es_drum1[]
 *                     (was HL).
 *
 * Conv: the Z80 uses RLC (HL) to walk bit 7 through all 8 bit positions across
 *       8 iterations -- the byte doubles as its own iteration counter, no
 *       separate bit-index register needed. This rotation mutates the sample
 *       data in place (only a full 8-bit rotation restores it), so the drum
 *       samples live in bank7 state as mutable copies of
 *       es_drum_sample_2_template/es_drum_sample_1_template.
 *
 * Conv: the inter-OUT delay code is modelled as speccy->logtime so the host can
 *       reconstruct the bit timing.
 *
 * Conv: C has no mid-sample interrupts, so the early-return resume path never
 *       triggers and the sample always plays to completion in one call.
 */
static void es_playdrum_go(chqstate_t *state, int D_length, u8 *HL_data)
{
  zxspectrum_t *speccy; /* game's ZX Spectrum facade (was N/A) */
  int           carry;  /* carry flag used by RLC (carry) */
  int           i;      /* inner loop counter: drum_speed ticks per sample byte (was B') */
  int           bits;   /* speaker output level: port_MASK_EAR or 0 based on sample bit 7 (was A) */

  speccy = state->speccy;
  carry  = 0;
  for (;;) {
    i = state->bank7->es_music.drum_speed;
    do {
      bits = port_MASK_EAR; // speaker bit
      if ((*HL_data & (1 << 7)) == 0)
        bits = 0;
      speccy->out(speccy, port_BORDER_EAR_MIC, bits);
      RLC(*HL_data); /* rotate sample byte in place */
      /* inter-bit cost 15+13+7+4+12+12 (bit-set path) */
      speccy->logtime(speccy, 63);
    } while (--i > 0);
    HL_data++;
    /* inter-byte cost 6+4+7+13+4+10+7, less the DJNZ not-taken saving */
    speccy->logtime(speccy, 46);
    if (--D_length == 0)
      goto pd_end_of_sample;
  }
  // EXX unbank
  return;

pd_end_of_sample:
  state->bank7->es_music.drum_active = 0;
}

/* ----------------------------------------------------------------------- */

/**
 * Allocate and initialise the bank 7 sub-state.
 *
 * Conv: host lifecycle helper; has no Z80 address. Called once from chq_create.
 *       Sets the fade-gate bytes and drum-sample copies that calloc's zero fill
 *       would otherwise leave wrong (see the comments below).
 *
 * \return 0 on success, -1 if allocation failed.
 */
int bank7_state_create(chqstate_t *state)
{
  state->bank7 = calloc(1, sizeof(*state->bank7));
  if (state->bank7 == NULL)
    return -1;

  /* $E06C/$E06D initial bytes (relocated to $5C6C/$5C6D): these flip-flop
   * gate bytes live in what was code space before relocation, so calloc's
   * zero is wrong -- RLC(0) is always 0 with no carry, which permanently
   * disables the GLYPH_B fade and the handshake animation's advance/draw
   * block. */
  state->bank7->es_fade_gate_ab = 0xAA;
  state->bank7->es_fade_gate_c = 0x88;

  /* es_playdrum_go rotates each sample byte in place during playback (RLC),
   * so bank 7 needs its own mutable copies of the drum templates, refreshed
   * per instance exactly like chqstate_t's own music.drum1/music.drum2 (Create.c). */
  memcpy(state->bank7->es_drum2, es_drum_sample_2_template, sizeof(es_drum_sample_2_template));
  memcpy(state->bank7->es_drum1, es_drum_sample_1_template, sizeof(es_drum_sample_1_template));

  return 0;
}

/**
 * Free the bank 7 sub-state.
 *
 * Conv: host lifecycle helper; has no Z80 address. Called once from
 *       chq_destroy.
 */
void bank7_state_destroy(chqstate_t *state)
{
  free(state->bank7);
  state->bank7 = NULL;
}
