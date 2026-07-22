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
 * This file will hold the C translation of ZX Spectrum 128K "bank 7" -- the
 * end-of-game results/credits sequence. Paged into $C000-$FFFF and reached
 * via an inner relocation from $F7EF to $F300, driven by a script
 * interpreter at $E20A. Not yet ported; see show_end_screen() in Main.c.
 */

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "C99/Types.h"

#include "ZXSpectrum/Macros.h"
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

/**
 * $E499: Clear the playfield ready for the end screen
 *
 * Zeros the on-screen playfield (attributes and bitmap) plus the first 512
 * bytes of the "backbuffer" area, which alias the attribute portion of the
 * end-screen montage/glyph drawing area ($F000 = &state->backbuffer[0]).
 *
 * \param[in] state Pointer to game state.
 */
static void es_clear(chqstate_t *state)
{
  clear_playfield(state);
  memset(&state->backbuffer[0], 0, 512);
}

/* Script command bytes, $E20D's DEC A/JP Z chain (1-based, in read order). */
#define ESCMD_CLEAR_DRAW_FRAME    (1) /* -> $E2D9 es_clear_then_draw_frame, runs immediately */
#define ESCMD_DRAW_WORD           (2) /* -> $E2DE es_handler_draw_word, runs immediately */
#define ESCMD_FADE_IN_A           (3) /* -> $E42E es_attribute_fade_in via rs_exit, reload 16 */
#define ESCMD_FADE_IN_B           (4) /* -> $E472 es_handler_glyph_fade_b via rs_exit, reload 16 */
#define ESCMD_HANDSHAKE           (5) /* -> $E3B7 es_handler_handshake via rs_exit, reload 16 */
#define ESCMD_FADE_IN_C           (6) /* -> $E46D es_handler_glyph_fade_c via rs_exit, reload 32 */
#define ESCMD_IDLE                (7) /* -> rs_exit, handler = no-op, reload = script byte */
#define ESCMD_RESET_HANDSHAKE     (8) /* -> rs_exit, sets $A172, handler = handshake, reload = script byte */
#define ESCMD_HANDSHAKE_AGAIN     (9) /* -> rs_exit, handler = handshake, reload = script byte */
#define ESCMD_DRAW_TEXT_NO_CLEAR (10) /* -> $E2F0 (undecoded) */
#define ESCMD_DRAW_TEXT          (11) /* -> $E2C0-style entry (undecoded) */
#define ESCMD_CHATTER            (12) /* -> $E2B2, runs immediately */
#define ESCMD_DRAW_SCORE         (13) /* -> $E256, runs immediately */

#define ADDRTOSCREEN(addr)  z80addrtoscreen(state, addr, 0, 0)
#define ADDRTOATTRS(addr)   z80addrtoattrs(state, addr, 0, 0)
#define ADDRTOBACKBUF(addr) z80addrtobackbuf(state, addr)

/**
 * $E0FE-$E209: Raw end-screen script bytes.
 *
 * Verbatim transcription of the skool's script_data block (268 bytes):
 * command/argument bytes interleaved with bitmap addresses (as raw
 * little-endian DEFW pairs) and embedded high-bit-terminated ASCII text
 * ("CONGRATULATIONS", "WELL DONE", etc). run_script's command dispatch
 * (below) has not yet been checked against every byte here -- this is
 * the raw data only, not yet validated end-to-end against a real script
 * run.
 *
 * Conv: not const -- es_handler_draw_score patches the "GBP________ PTS"
 * placeholder text in-place (offset 0xFD, matching $5DFB relocated) with the
 * player's formatted score, exactly as the original self-modifies its own
 * script_data at that address.
 */
static u8 script_data[] = {
  ESCMD_CHATTER,
  TWOBYTES(0x5C6E), /* -> chatterblk_nancy_congratulates */
  0x07, 0xC0,

  ESCMD_CLEAR_DRAW_FRAME,
  TWOBYTES(0x60E1), // -> bitmap_endshot_1
  TWOBYTES(0x4889), // screen dst
  0x03, 0x07, 0xA0, 0x04,

  ESCMD_CLEAR_DRAW_FRAME,
  TWOBYTES(0x6489), // -> bitmap_endshot_2
  TWOBYTES(0x4889), // screen dst
  0x03, 0x07, 0xA0, 0x04,

  ESCMD_CLEAR_DRAW_FRAME,
  TWOBYTES(0x6831), // -> bitmap_endshot_3
  TWOBYTES(0x4889), // screen dst
  0x03, 0x07, 0xA0, 0x04,

  ESCMD_CLEAR_DRAW_FRAME,
  TWOBYTES(0x6BD9), // -> bitmap_endshot_4
  TWOBYTES(0x4889), // screen dst
  0x03, 0x07, 0xA0, 0x04,

  ESCMD_CLEAR_DRAW_FRAME,
  TWOBYTES(0x60E1), // -> bitmap_endshot_1
  TWOBYTES(0x4802), // screen dst
  0x03, 0x07, 0x50,

  0x02,
  TWOBYTES(0x6489), // -> bitmap_endshot_2
  TWOBYTES(0x4811), // screen dst
  0x03, 0x07, 0x50,

  0x02,
  TWOBYTES(0x6831), // -> bitmap_endshot_3
  TWOBYTES(0x5002), // screen dst
  0x03, 0x07, 0x50,

  0x02,
  TWOBYTES(0x6BD9), // -> bitmap_endshot_4
  TWOBYTES(0x5011), // screen dst
  0x03, 0x07, 0x50,

  0x08, 0xC0, 0x09, 0xB0, 0x05, 0x09, 0xB0, 0x04,
  0x0B, 0x47, 0x48, 0x48, 0x43, 0x4F, 0x4E, 0x47,
  0x52, 0x41, 0x54, 0x55, 0x4C, 0x41, 0x54, 0x49,
  0x4F, 0x4E, 0x53, 0xA1, 0x03, 0x07, 0x08, 0x0A,
  0x47, 0xCB, 0x48, 0x41, 0x4C, 0x4C, 0x20, 0x20,
  0x43, 0x4C, 0x45, 0x41, 0xD2, 0x03, 0x07, 0x08,
  0x0A, 0x47, 0x49, 0x50, 0x35, 0x2C, 0x30, 0x30,
  0x30, 0x2C, 0x30, 0x30, 0x30, 0x20, 0x20, 0x50,
  0x54, 0x53, 0xAE, 0x03, 0x07, 0xC0, 0x06, 0x07,
  0x60, 0x0B, 0x47, 0x84, 0x48, 0x28, 0x43, 0x29,
  0x20, 0x31, 0x39, 0x38, 0x39, 0x20, 0x4F, 0x43,
  0x45, 0x41, 0x4E, 0x20, 0x53, 0x4F, 0x46, 0x54,
  0x57, 0x41, 0x52, 0xC5, 0x0A, 0x47, 0x43, 0x50,
  0x28, 0x43, 0x29, 0x20, 0x31, 0x39, 0x38, 0x38,
  0x20, 0x54, 0x41, 0x49, 0x54, 0x4F, 0x20, 0x43,
  0x4F, 0x52, 0x50, 0x4F, 0x52, 0x41, 0x54, 0x49,
  0x4F, 0xCE, 0x03, 0x07, 0xF0, 0x06, 0x07, 0x60,
  0x0B, 0x47, 0xCC, 0x48, 0x54, 0x48, 0x45, 0x20,
  0x20, 0x45, 0x4E, 0xC4, 0x03, 0x07, 0xF0, 0x07,
  0x60, 0x06, 0x07, 0x60,
  0x0B, 0x45, 0xAA, 0x48,
  0x46, 0x49, 0x4E, 0x41, 0x4C, 0x20, 0x20, 0x53,
  0x43, 0x4F, 0x52, 0xC5, 0x03, 0x07, 0x1E,

  0x0D,

  0x0A, 0x47, 0x2C, 0x50, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0xA0, 0x03, 0x0C, 0x78, 0x5C,
  0x07, 0x00, 0x0E
};

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
 * \return Screen address one row down.
 */
static u16 next_screen_row(u16 addr)
{
  u8 Dhi; /* screen address high byte after +1 scanline (was D via A) */
  u8 Elo; /* screen address low byte after +32 column step (was E via A) */

  Dhi = (u8) ((addr >> 8) + 1);
  if ((Dhi & 0x07) != 0)
    return (u16) ((Dhi << 8) | (addr & 0xFF));

  Elo = (u8) ((addr & 0xFF) + 0x20);
  if (Elo < 0x20) /* carry out of E: stay in the next screen third */
    return (u16) ((Dhi << 8) | Elo);
  else
    return (u16) (((Dhi - 0x08) << 8) | Elo);
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
 * Conv: operates on the raw 16-bit Z80 screen address (screen_addr) and
 * calls ADDRTOSCREEN per row, rather than walking a pre-resolved C pointer,
 * so the row-wrap arithmetic can mirror the Z80 exactly.
 *
 * \param[in] state       Pointer to game state.
 * \param[in] image       Bitmap+attribute source blob (was HL).
 * \param[in] screen_addr Top-left destination screen address (was DE).
 */
static void draw_endshot(chqstate_t *state, const u8 *image, u16 screen_addr)
{
  int row;            /* bitmap row counter, 64 down to 1 (was B) */
  u16 DE_screen_addr; /* current screen row address (was DE) */
  u8  Dattr;          /* attribute row address high byte (was D after RRCA x3) */
  u16 attraddr;       /* current attribute row address (was DE in the attr loop) */
  int attrrow;        /* attribute row counter, 8 down to 1 (was A) */

  DE_screen_addr = screen_addr;

  for (row = 64; row != 0; row--) {
    memcpy(ADDRTOSCREEN(DE_screen_addr), image, 13);
    image += 13;
    DE_screen_addr = next_screen_row(DE_screen_addr);
  }

  /* Conv: skool POPs DE here, restoring the original destination pushed at
   * function entry -- NOT the row loop's final DE_screen_addr. Must use the
   * screen_addr parameter, which the loop above never mutates. */
  Dattr    = (u8) (screen_addr >> 8); /* original D, pre-rotate */
  Dattr    = (u8) ((((Dattr >> 3) | (Dattr << 5)) & 0x03) + 0xEF); /* RRCA x3; AND 3; ADD $EF */
  attraddr = (u16) ((Dattr << 8) | (screen_addr & 0xFF));

  for (attrrow = 8; attrrow != 0; attrrow--) {
    memcpy(ADDRTOBACKBUF(attraddr), image, 13);
    image += 13;
    attraddr = (u16) (attraddr + 19);
  }
}

/**
 * Resolve a script-embedded end-shot bitmap address to its C data array.
 *
 * Conv: the original walks a real (relocated) Z80 pointer; script_data only
 * ever encodes these four literal addresses (see the ESCMD_CLEAR_DRAW_FRAME
 * entries above), so a small lookup replaces pointer arithmetic into
 * relocated bank memory the C port does not model byte-for-byte.
 *
 * \param[in] addr Raw address word read from the script (was HL).
 * \return Matching bitmap_endshot_N array.
 */
static const u8 *resolve_endshot(u16 addr)
{
  switch (addr) {
  case 0x60E1: return &bitmap_endshot_1[0];
  case 0x6489: return &bitmap_endshot_2[0];
  case 0x6831: return &bitmap_endshot_3[0];
  case 0x6BD9: return &bitmap_endshot_4[0];
  default:     assert(0); return NULL;
  }
}

/**
 * $E2DE es_handler_draw_word: Read an image+destination pair from the
 * script and blit it, without the backbuffer-clear prefix.
 *
 * \param[in]     state  Pointer to game state.
 * \param[in,out] script Script read pointer (was HL); advanced past the two
 *                        words consumed.
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

  draw_endshot(state, resolve_endshot(image_addr), screen_addr);

  *script = HL_script;
}

/**
 * $E2D9: Clear the backbuffer, then draw an end-screen graphic frame
 *
 * \param[in]     state  Pointer to game state.
 * \param[in,out] script Script read pointer (was HL); advanced past the two
 *                       words consumed.
 */
static void es_clear_then_draw_frame(chqstate_t *state, const u8 **script)
{
  es_clear(state);
  es_draw_frame_common(state, script);
}

/**
 * $E42E es_attribute_fade_in: Sweep the middle attribute band toward
 * the backbuffer target colours
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
 * (`ADD A,$08`, $E468) are not masked back into their 3-bit fields -- u8
 * wraparound reproduces this bug-for-bug.
 *
 * \param[in] state Pointer to game state.
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

  RLC(state->bank7->es_flag_5c6c);
  if (carry)
    return;

  HL_attr = ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR);
  DE_back = ADDRTOBACKBUF(0xF000);

  for (c = 32 * 16; c != 0; c--, HL_attr++, DE_back++) {
    if (*HL_attr & ATTR_BRIGHT) /* BRIGHT set: leave this cell untouched */
      continue;

    A_target = *DE_back & 0x3F;
    if (A_target == *HL_attr) {
      *HL_attr = *DE_back;
      continue; // already there
    }

    B_target = A_target;

    C_ink = *HL_attr & 0x07; // Ink
    if ((B_target & 0x07) != C_ink)
      C_ink++;

    A_paper = *HL_attr & 0x38; // Paper
    if ((B_target & 0x38) != A_paper)
      A_paper = (u8) (A_paper + 0x08);

    *HL_attr = (u8) (A_paper | C_ink);
  }
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
 * Conv: unlike es_attribute_fade_in, BRIGHT/FLASH are never tested here --
 * the original ANDs each byte down to its ink/paper fields before OR-ing
 * them back together, which drops those bits on every write. Matched
 * bug-for-bug.
 *
 * \param[in] state Pointer to game state.
 * \param[in] flag  Flip-flop gate byte to rotate (was HL -> $5C6C/$5C6D).
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

  for (c = 32 * 16; c != 0; c--, HL_pattrs++) {
    A_attr = *HL_pattrs;
    if (A_attr == 0)
      continue;

    B_ink = A_attr & 0x07;
    if (B_ink != 0)
      B_ink--;

    A_paper = A_attr & 0x38;
    if (A_paper != 0)
      A_paper = (u8) (A_paper - 0x08);

    *HL_pattrs = (u8) (A_paper | B_ink);
  }
}

/**
 * $E472 routine_e472: Fade the $5C6C-gated glyph attribute band
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_glyph_fade_b(chqstate_t *state)
{
  es_attribute_fade_out(state, &state->bank7->es_flag_5c6c);
}

/**
 * $E46D routine_e46d: Fade the $5C6D-gated glyph attribute band
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_glyph_fade_c(chqstate_t *state)
{
  es_attribute_fade_out(state, &state->bank7->es_flag_5c6d);
}

/* $E3A5 handshake_table: row-count + source bitmap per animation frame,
 * cycling 1-2-3-4-3-2 (see routine_e3b7 below). */
static const struct {
  u8         rows;
  const u8  *image;
} handshake_table[6] = {
  { 37, &bitmap_handshake_1[0] },
  { 35, &bitmap_handshake_2[0] },
  { 34, &bitmap_handshake_3[0] },
  { 32, &bitmap_handshake_4[0] },
  { 34, &bitmap_handshake_3[0] },
  { 35, &bitmap_handshake_2[0] },
};

/**
 * $E3B7 routine_e3b7: Advance the handshake animation frame
 *
 * Always fades the $5C6C attribute band one step first (routine_e472's
 * shared tail, called directly rather than duplicated). Then rotates the
 * gate byte at $5C6D: when its old top bit was clear, the animation-advance
 * block below is skipped entirely; otherwise the 0-5 ping-pong frame index
 * ($A172) advances into handshake_table, that frame's rows are LDIR'd to
 * screen $48AC (8 bytes/row, wraparound-stepped via next_screen_row), and 3
 * further 8-byte rows are zero-filled to pad every frame out to a fixed
 * height. Either way, finishes by stamping a fixed 5-group x 8-byte
 * decorative attribute pattern at $59AC.
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_handshake(chqstate_t *state)
{
  int       carry;    /* carry flag set by RLC (carry) */
  u8        A_index;  /* frame index 0..5, wrapped (was A/B) */
  const u8 *HL_image; /* handshake bitmap source, walked forward (was HL) */
  u16       DE;       /* screen destination address (was DE) */
  int       row;      /* bitmap row counter for this frame (was B) */
  int       blank;    /* blank-row counter, 3 down to 0 (was C) */
  u8       *HL_attr;  /* decorative attribute cell (was HL) */
  int       group;    /* decorative attribute group counter, 5 down to 0 (was C) */

  es_handler_glyph_fade_b(state);

  RLC(state->bank7->es_flag_5c6d);
  if (carry) {
    A_index = state->bank7->es_handshake_index;
    state->bank7->es_handshake_index = (u8) (A_index + 1 == 6 ? 0 : A_index + 1);

    HL_image = handshake_table[A_index].image;
    DE      = 0x48AC;

    for (row = handshake_table[A_index].rows; row != 0; row--) {
      memcpy(ADDRTOSCREEN(DE), HL_image, 8);
      HL_image += 8;
      DE = next_screen_row(DE);
    }

    for (blank = 3; blank != 0; blank--) {
      memset(ADDRTOSCREEN(DE), 0, 8);
      DE = next_screen_row(DE);
    }
  }

  HL_attr = ADDRTOATTRS(0x59AC);
  for (group = 5; group != 0; group--) {
    memset(HL_attr, 0x07, 8);
    HL_attr += 0x20; /* 8-byte fill + $0018 stride, matches ADD HL,DE */
  }
}

/**
 * $E2D8 (stub): Idle per-frame handler (no drawing)
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_idle(chqstate_t *state)
{
  NOT_USED(state);
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
 * "GBP________ PTS" placeholder already sitting in script_data (offset
 * 0xFD, $5DFB relocated), so the later draw-word command renders it.
 *
 * Conv: the leading-zero blank/print decision (Z80: RLC C carry chain) is
 * modelled as a sticky "seen a non-blank digit yet" flag: print the digit if
 * it is non-zero or a digit has already been printed, otherwise print a
 * space.
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_draw_score(chqstate_t *state)
{
  int       tally;    /* bonus-tally animation iteration counter (was BC) */
  const u8 *DE_bcd;   /* packed-BCD score pointer, walked backwards (was DE) */
  u8       *HL_dst;   /* destination ASCII bytes in script_data (was HL) */
  u8        C_seen;   /* sticky "non-blank digit already printed" flag (was C) */
  int       pair;     /* BCD byte-pair iteration counter (was B) */
  u8        A_nibble; /* nibble being converted to ASCII (was A) */

  for (tally = 1000; tally != 0; tally--) {
    increment_score(state, 0, 0x00, 0x50);
    ptad_led_digits(4, &state->score_bcd[3], &state->session.score_digits[7],
                     ADDRTOSCREEN(0x4126));
    sfx_bipbow(state, 2, 2);
  }

  state->bank7->es_input_mask = 1;

  DE_bcd = &state->score_bcd[3];
  HL_dst = &script_data[0xFD];
  C_seen = 0;

  for (pair = 4; pair != 0; pair--) {
    A_nibble = (*DE_bcd >> 4) & 0x0F;
    if (A_nibble != 0 || C_seen != 0) {
      C_seen    = 0xFF;
      *HL_dst++ = 0x30 + A_nibble;
    } else {
      *HL_dst++ = 0x20;
    }

    A_nibble = *DE_bcd & 0x0F;
    if (A_nibble != 0 || C_seen != 0) {
      C_seen    = 0xFF;
      *HL_dst++ = 0x30 + A_nibble;
    } else {
      *HL_dst++ = 0x20;
    }

    DE_bcd--;
  }

  HL_dst[-1] |= EOS; /* SET 7,(HL): mark this text run's terminator byte */
}

/**
 * Resolve a script-embedded argument word to its C data array.
 *
 * Conv: as with resolve_endshot, script_data only ever encodes one literal
 * value here ($5C6E, pre-relocation for data_e06e at post-relocation $E06E
 * via the bank's uniform +0x8400 rule), so a small lookup replaces pointer
 * arithmetic into relocated bank memory the C port does not model
 * byte-for-byte.
 *
 * \param[in] addr Raw address word read from the script (was HL after
 *                 EX DE,HL at $E2B7).
 * \return Matching data block, or NULL if unrecognised.
 */
static const u8 *resolve_chatterblk(u16 addr)
{
  switch (addr) {
  case 0x5C6E: return &chatterblk_nancy_congratulates[0];
  case 0x5C78: return &chatterblk_press_gear[0];
  default:     assert(0); return NULL;
  }
}

/**
 * $E2B2: es_chatter: Call a script-supplied handler with a literal argument
 *
 * Reads a 2-byte little-endian pointer word from the script, advances the
 * script pointer past it, then calls start_chatter with priority 1 (Z80
 * sets A=0 then INC A immediately before CALL $9945) and the resolved
 * pointer. Runs immediately (loops back into run_script rather than
 * returning).
 *
 * Conv: data_e06e (the only live target) is NOT a standard
 * {CHATTERCHR, CHATTERSTR, CHATTERCMD} chatterblk -- see the comment on
 * data_e06e in Bank7Data.h. Byte 2 of that block ($3F = 63) would be
 * consumed as a CHATTERSTR index by pc_chatter_message (Main.c, the
 * "assert(*chatterblk < CHATTERSTR__LIMIT)" guard around line 5871) and
 * fail that bounds check immediately -- CHATTERSTR__LIMIT is 36. In a
 * release build without asserts this reads common_chatter_strings[63] out
 * of its 36-entry array and dereferences whatever garbage pointer turns up,
 * i.e. every single playthrough would crash on reaching the end screen.
 * The original Z80 has the same malformed data, so this path is presumed
 * unreached in practice (the skool marks it "unproven, dead end"); rather
 * than risk that byte-for-byte here, the start_chatter call is skipped
 * whenever the resolved target does not look like a well-formed chatterblk.
 *
 * \param[in,out] state Pointer to game state; state->bank7->es_script_ptr is
 *                       read and advanced past the word consumed.
 */
static void es_chatter(chqstate_t *state)
{
  const u8 *HL_script_ptr; /* script read pointer (was HL) */
  u16       target_addr;   /* raw argument word read from the script (was DE/HL) */
  const u8 *chatterblk;    /* resolved data block pointer (was HL after CALL $9945 setup) */

  HL_script_ptr = state->bank7->es_script_ptr;

  target_addr = wordat(HL_script_ptr);
  HL_script_ptr += 2;

  chatterblk = resolve_chatterblk(target_addr);
  start_chatter(state, 1, chatterblk);

  state->bank7->es_script_ptr = HL_script_ptr;
}

/**
 * $E2CD es_set_dispatch: Set the per-frame handler and its frame-delay reload
 *
 * Common tail shared by the run_script commands that hand off to a
 * self-modified per-frame handler rather than running immediately: stores
 * the new handler and reload count, ready for show_end_screen's loop to
 * count down and re-invoke run_script when it reaches zero.
 *
 * \param[in] state Pointer to game state.
 * \param[in] handler New per-frame handler (was DE).
 * \param[in] reload New $A170 frame-delay reload count (was C).
 */
static void es_set_dispatch(chqstate_t *state,
                            void      (*handler)(chqstate_t *state),
                            u8          reload)
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
 * \param[in] character ASCII character, already offset by ' ' (was A after
 *                        SUB $20; space and 0 are handled by the caller).
 * \return Glyph index into font[] (multiply by 7 for the row pointer).
 */
static int text_glyph_id(int character)
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
 * $E31F plot_char: Render one end-screen text character
 *
 * Space ($E323-$E327): advances both persistent cursors by one column and
 * draws nothing.
 *
 * Non-space ($E328-$E3A4): looks up the glyph via text_glyph_id(), then
 * blits it double-height directly to the screen in two passes (font rows
 * 0-3, then 4-6 plus a trailing blank row) -- the same "double height via
 * two separate 4- and 3-row passes with a column advance mid-glyph"
 * structure as draw_char's style==0 case, except writing to real (bank-7)
 * screen addresses rather than a flat backbuffer offset, so the mid-glyph
 * advance has to reproduce the Z80's raw row/column byte arithmetic.
 * Finally stamps the call's colour byte into both glyph-cell attributes and
 * advances the persistent cursor by one column.
 *
 * Conv: register-banking notes, resolved from the original stalled attempt.
 * The Z80 threads three logically distinct values through nested EXX/stack
 * shuffles ($E306, $E307/EX (SP),HL, and plot_char's own $E35B/$E39A-$E3A3
 * EXX pairs): the script read cursor (HL throughout the character loop --
 * modelled as the caller's script pointer, untouched by plot_char), the
 * persistent column cursor (screen-dest E and attr-addr L, both threaded
 * here as [in,out] Ecol/Lattr), and this character's own draw position
 * (screen-dest E's PRE-increment value, borrowed via a PUSH/EXX/POP
 * shuffle at $E35B-$E35F -- modelled here as the local Ecur, read from
 * *Ecol before it is advanced). Everything else the EXX dance shuffles
 * (Set S's stale/arbitrary BC and DE, pushed and popped purely to balance
 * the stack) carries no live data and is correctly omitted.
 *
 * Conv: A_colour (was C, Set M) is NOT the "row count" the ($E2F5) prologue
 * naming originally suggested. $E399's EXX switches back to the SAME
 * physical register set read at $E2F5 -- the ladder's own use of C
 * ($E328-$E356) is a completely different (Set S) C that plot_char's own
 * LDI calls decrement into irrelevance and never reads back. Confirmed
 * against script_data: the byte read here for "CONGRATULATIONS!" is $47 =
 * BRIGHT, PAPER black, INK white -- a plausible text colour, not a row
 * count. It survives unclobbered in Set M across the whole render_text call
 * and is written verbatim into both glyph-cell attributes at $E399/$E3A0.
 *
 * Conv: $E37A-$E37F (E += $1F, then the pending LDI increment folds in a
 * further +1, netting E += $20; D -= 7) is NOT the same computation as
 * next_screen_row() -- it never checks for, or propagates, a carry out of
 * the column byte into the row byte, unlike next_screen_row's explicit
 * "did this cross a screen third" branch. Reproduced literally as two
 * independent 8-bit adds rather than substituting next_screen_row, since
 * the two are only equivalent when no such carry occurs -- true for every
 * script-supplied text position in script_data, but not guaranteed in
 * general.
 *
 * Conv: the attribute-row address computed from Drow (Hattr, range
 * $EF-$F2) is resolved via ADDRTOSCREEN, not ADDRTOATTRS, matching the
 * established precedent in draw_endshot's attribute-row loop above -- this
 * bank-7 memory range is not standard $5800-$5AFF attribute space.
 *
 * \param[in]     state    Pointer to game state.
 * \param[in]     A_char   Script character byte, EOS bit already masked off
 *                          by the caller (was A).
 * \param[in]     Drow     Screen destination row byte; constant for the
 *                          whole render_text call (was D, Set M).
 * \param[in,out] Ecol     Screen destination column byte; the persistent
 *                          cursor, advanced by one per character (was E,
 *                          Set M).
 * \param[in]     Hattr    Attribute-row address high byte; constant for the
 *                          whole call (was H, Set M).
 * \param[in,out] Lattr    Attribute address column byte; the persistent
 *                          cursor, mirrors *Ecol (was L, Set M).
 * \param[in]     A_colour Attribute/colour byte read once from the script
 *                          at $E2F5 and held constant for the whole call
 *                          (was C, Set M) -- see the Conv note above.
 */
static void plot_char(chqstate_t *state, u8 A_char, u8 Drow, u8 *Ecol,
                      u8 Hattr, u8 *Lattr, u8 A_colour)
{
  int       character; /* character code, offset by ' ' (was A) */
  int       glyphid;   /* glyph index into font[] (was C during the ladder) */
  const u8 *HLfont;    /* current font row pointer, walked forward (was HL) */
  u8        Ecur;      /* this character's draw column (was E, Set S) */
  u8        Dcur;      /* current screen row byte during the blit (was D) */
  int       i;         /* pass loop index (Conv: no Z80 register) */
  int       data;      /* font byte read for the current scanline pair (was A) */
  u8        Lcur;      /* this character's attribute column (was L) */

  character = A_char - ' ';
  if (character == 0) {
    // Space: $E323-$E327.
    (*Ecol)++;
    (*Lattr)++;
    return;
  }

  glyphid = text_glyph_id(character);
  HLfont  = &font[glyphid * 7];

  Ecur = *Ecol;
  (*Ecol)++; // $E35D: persistent cursor advances for the NEXT character now.

  // Pass 1 ($E360-$E378): font bytes 0-3, double height.
  Dcur = Drow;
  for (i = 0; i < 4; i++) {
    data = *HLfont++;
    *ADDRTOSCREEN(((u16) Dcur << 8) | Ecur) = (u8) data;
    Dcur++;
    *ADDRTOSCREEN(((u16) Dcur << 8) | Ecur) = (u8) data;
    if (i != 3)
      Dcur++;
  }

  // $E37A-$E381: mid-glyph row-wrap -- see Conv note in the prologue.
  Ecur = (u8) (Ecur + 0x20);
  Dcur = (u8) (Dcur - 7);

  // Pass 2 ($E382-$E398): font bytes 4-6, double height, then a blank row.
  for (i = 0; i < 3; i++) {
    data = *HLfont++;
    *ADDRTOSCREEN(((u16) Dcur << 8) | Ecur) = (u8) data;
    Dcur++;
    *ADDRTOSCREEN(((u16) Dcur << 8) | Ecur) = (u8) data;
    Dcur++;
  }
  *ADDRTOSCREEN(((u16) Dcur << 8) | Ecur) = 0;

  // $E399-$E3A4: stamp the call's colour into both glyph-cell attributes.
  Lcur    = *Lattr;
  *ADDRTOBACKBUF(((u16) Hattr << 8) | Lcur)               = A_colour;
  *ADDRTOBACKBUF(((u16) Hattr << 8) | (u8) (Lcur + 0x20)) = A_colour;
  *Lattr = (u8) (Lcur + 1);
}

/**
 * $E2F5 render_text_common: Parse and draw a script text-render command
 *
 * Reads a colour byte and a screen destination word from the script (3
 * bytes total), derives the attribute-row address exactly as draw_endshot
 * does, then plots each following script character via plot_char until the
 * EOS-terminated (top-bit-set) character has been drawn.
 *
 * \param[in]     state  Pointer to game state.
 * \param[in,out] script Script read pointer (was HL); advanced past the
 *                       3-byte header and the whole character run.
 */
static void render_text_common(chqstate_t *state, const u8 **script)
{
  const u8 *HL_script; /* script read pointer (was HL) */
  u8        C_colour;  /* attribute/colour byte read from the script (was C) */
  u8        E_scr;     /* screen destination column byte (was E) */
  u8        D_scr;     /* screen destination row byte (was D) */
  u8        H_attr;    /* attribute-row address high byte (was H, $E304) */
  u8        L_attr;    /* attribute address column byte (was L, $E305) */
  u8        raw;       /* raw script byte, EOS bit intact (was (HL) at $E312) */
  u8        A_char;    /* script character byte, EOS bit masked off (was A) */

  HL_script = *script;

  C_colour = *HL_script++;
  E_scr    = *HL_script++;
  D_scr    = *HL_script++;

  H_attr = (u8) ((((D_scr >> 3) | (D_scr << 5)) & 0x03) + 0xEF); /* RRCA x3; AND 3; ADD $EF */
  L_attr = E_scr;

  do {
    raw    = *HL_script;
    A_char = raw & (u8) ~EOS;
    plot_char(state, A_char, D_scr, &E_scr, H_attr, &L_attr, C_colour);
    HL_script++;
  } while ((raw & EOS) == 0);

  *script = HL_script;
}

/**
 * $E2F0: Clear the backbuffer, then render an end-screen text run
 *
 * \param[in]     state  Pointer to game state.
 * \param[in,out] script Script read pointer (was HL); advanced as per
 *                       render_text_common.
 */
static void es_handler_render_text(chqstate_t *state, const u8 **script)
{
  es_clear(state);
  render_text_common(state, script);
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
 * ESCMD_DRAW_TEXT_NO_CLEAR/ESCMD_DRAW_TEXT (render_text/plot_char) also run immediately, drawing
 * their text run within this same call rather than arming a per-frame
 * handler -- see render_text_common and plot_char above.
 *
 * \param[in] state Pointer to game state.
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
    case ESCMD_CLEAR_DRAW_FRAME:
      es_clear_then_draw_frame(state, &HL_script);
      continue;

    case ESCMD_DRAW_WORD:
      /* $E2DE is $E2D9's tail half, entered directly for this command
       * (skipping E2D9's own backbuffer-clear prefix). */
      es_draw_frame_common(state, &HL_script);
      continue;

    case ESCMD_FADE_IN_A:
      es_set_dispatch(state, es_attribute_fade_in, 16);
      goto rs_exit;

    case ESCMD_FADE_IN_B:
      es_set_dispatch(state, es_handler_glyph_fade_b, 16);
      goto rs_exit;

    case ESCMD_HANDSHAKE:
      es_set_dispatch(state, es_handler_handshake, 16);
      goto rs_exit;

    case ESCMD_FADE_IN_C:
      es_set_dispatch(state, es_handler_glyph_fade_c, 32);
      goto rs_exit;

    case ESCMD_IDLE:
      C_reload = *HL_script++;
      es_set_dispatch(state, es_handler_idle, C_reload);
      goto rs_exit;

    case ESCMD_RESET_HANDSHAKE:
      C_reload = *HL_script++;
      state->bank7->es_handshake_index = 0; /* $E2C0 LD ($A172),A with A=0 */
      es_set_dispatch(state, es_handler_handshake, C_reload);
      goto rs_exit;

    case ESCMD_HANDSHAKE_AGAIN:
      C_reload = *HL_script++;
      es_set_dispatch(state, es_handler_handshake, C_reload);
      goto rs_exit;

    case ESCMD_DRAW_TEXT_NO_CLEAR:
      /* $E2F5 entered directly: no backbuffer-clear prefix. */
      render_text_common(state, &HL_script);
      continue;

    case ESCMD_DRAW_TEXT:
      es_handler_render_text(state, &HL_script);
      continue;

    case ESCMD_CHATTER:
      es_chatter(state);
      HL_script = state->bank7->es_script_ptr; /* es_chatter advanced it directly */
      continue;

    case ESCMD_DRAW_SCORE:
      es_handler_draw_score(state);
      continue;

    default:
      /* Unrecognised command: matches the Z80 fallback (reset to a fixed
       * script offset) closely enough for a defensive stop. */
      goto rs_exit;
    }
  }

rs_exit:
  state->bank7->es_script_ptr = HL_script;
}

/**
 * $E01E (stub): Set up bank 7's own interrupt handler and 48K music engine
 *
 * TODO: not yet ported. Bank 7 carries its own copy of the 48K sound code
 * (bank7_setup_interrupts onward in the skool), relocated into the copied
 * $F300 buffer.
 *
 * \param[in] state Pointer to game state.
 */
static void bank7_setup_interrupts(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $F300 (stub): Start the end-screen fanfare
 *
 * TODO: not yet ported. This is bank 7's own copy of the turbo-siren sound
 * code, relocated into the copied $F300 buffer -- distinct from Main.c's
 * play_engine_or_turbo_sfx_128k.
 *
 * \param[in] state Pointer to game state.
 */
static void play_turbo_sfx_128k(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $F340 (stub): Service end-screen speech playback for the current frame
 *
 * TODO: not yet ported. Skool comment marks this as living two bytes before
 * the shared play_speech_128k in the relocated buffer; not confirmed to be
 * the same routine.
 *
 * \param[in] state Pointer to game state.
 */
static void es_service_speech(chqstate_t *state)
{
  NOT_USED(state);
}

/**
 * $E000: Show the end screen
 *
 * Displays the end-of-game results screen: clears the playfield, starts the
 * fanfare and bank 7's own interrupt-driven music/script engine, then loops
 * driving the script/frame-advance dispatch and drive_chatter each frame
 * until the fire key is pressed twice (once to reach the congratulations
 * script, once more to exit), clearing the queued key each time.
 *
 * Conv: the Z80 entry point LDIRs itself from $E000 to $5C00, then LDIRs a
 * second, inner 768-byte block from $F7EF to $F300 and runs from there (see
 * project memory project-endscreen-bank7-double-relocation). Both
 * relocations are pure ZX paging/self-modification artefacts and are
 * discarded here, following the load_stage precedent -- the C functions
 * below are simply called directly.
 *
 * \param[in] state Pointer to game state.
 */
void show_end_screen(chqstate_t *state)
{
  u8  Ainput;      /* keyscan result, tested for the fire bit (was A) */
  int outer_count; /* per-keyscan frame divider, reloads to 5/6 (was A171) */

  es_clear(state);
  bank7_setup_interrupts(state);
  play_turbo_sfx_128k(state);

  state->bank7->es_script_ptr  = script_data;
  state->bank7->es_frame_count = 1;
  state->bank7->es_handler     = es_handler_idle;
  state->bank7->es_input_mask  = 0;

  outer_count = 6;

  for (;;) {
    if (state->host_quit)
      longjmp(state->host_quit_jmp, 1);

    state->speccy->stamp(state->speccy);

    es_service_speech(state);
    if (--state->bank7->es_frame_count == 0)
      run_script(state);
    state->bank7->es_handler(state);

    state->speccy->sleep(state->speccy, END_SCREEN_TSTATES);

    if (--outer_count != 0)
      continue;
    outer_count = 5;

    drive_chatter(state);
    Ainput = keyscan(state);
    if (!(Ainput & USERINPUTFLAG_FIRE))
      continue;

    if (state->bank7->es_input_mask == 0) {
      /* First fire press: skip ahead to the congratulations script. */
      state->bank7->es_input_mask = 1;
      drive_chatter_stop(state);
      while (keyscan(state) & USERINPUTFLAG_FIRE)
        ;
      continue;
    }

    /* Second fire press: leave the end screen. */
    break;
  }

  drive_chatter_stop(state);
}

int bank7_state_create(chqstate_t *state)
{
  state->bank7 = calloc(1, sizeof(*state->bank7));
  if (state->bank7 == NULL)
    return -1;

  return 0;
}

void bank7_state_destroy(chqstate_t *state)
{
  free(state->bank7);
  state->bank7 = NULL;
}
