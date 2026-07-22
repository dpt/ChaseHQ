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

#include "ChaseHQ/ChaseHQ.h"

#include "ChaseHQ/Data/Bank7Data.h"
#include "ChaseHQ/Data/Stages.h"

#include "Types.h"
#include "Internal.h"
#include "State.h"
#include "Bank7State.h"

#include "Bank7.h"

/* ----------------------------------------------------------------------- */

u8 call_bank_7_128k(chqstate_t *state, int HLroutine)
{
  NOT_USED(state);
  NOT_USED(HLroutine);

  return 0;
}

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
#define ESCMD_DRAW_FRAME       (1) /* -> $E2D9 es_handler_draw_frame, runs immediately */
#define ESCMD_DRAW_WORD        (2) /* -> $E2DE es_handler_draw_word, runs immediately */
#define ESCMD_GLYPH_A          (3) /* -> $E42E routine_e42e via rs_exit, reload 16 */
#define ESCMD_GLYPH_B          (4) /* -> $E472 routine_e472 via rs_exit, reload 16 */
#define ESCMD_HANDSHAKE        (5) /* -> $E3B7 routine_e3b7 via rs_exit, reload 16 */
#define ESCMD_GLYPH_C          (6) /* -> $E46D routine_e46d via rs_exit, reload 32 */
#define ESCMD_IDLE             (7) /* -> rs_exit, handler = no-op, reload = script byte */
#define ESCMD_SET_A172         (8) /* -> rs_exit, sets $A172, handler = handshake, reload = script byte */
#define ESCMD_HANDSHAKE_AGAIN  (9) /* -> rs_exit, handler = handshake, reload = script byte */
#define ESCMD_10              (10) /* -> $E2F0 (undecoded) */
#define ESCMD_11              (11) /* -> $E2C0-style entry (undecoded) */
#define ESCMD_CALL_WORD       (12) /* -> $E2B2, runs immediately */
#define ESCMD_RENDER_SCORE    (13) /* -> $E256, runs immediately */

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
  0x0C,
  TWOBYTES(0x5C6E), // -> data_e06e
  0x07, 0xC0,

  ESCMD_DRAW_FRAME,
  TWOBYTES(0x60E1), // -> bitmap_endshot_1
  TWOBYTES(0x4889), // screen dst
  0x03, 0x07, 0xA0, 0x04,

  ESCMD_DRAW_FRAME,
  TWOBYTES(0x6489), // -> bitmap_endshot_2
  TWOBYTES(0x4889), // screen dst
  0x03, 0x07, 0xA0, 0x04,

  ESCMD_DRAW_FRAME,
  TWOBYTES(0x6831), // -> bitmap_endshot_3
  TWOBYTES(0x4889), // screen dst
  0x03, 0x07, 0xA0, 0x04,

  ESCMD_DRAW_FRAME,
  TWOBYTES(0x6BD9), // -> bitmap_endshot_4
  TWOBYTES(0x4889), // screen dst
  0x03, 0x07, 0xA0, 0x04,

  ESCMD_DRAW_FRAME,
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
  0x43, 0x4F, 0x52, 0xC5, 0x03, 0x07, 0x1E, 0x0D,
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
 * Rotate a byte left circularly (Z80 RLC), reporting the bit that wrapped.
 *
 * \param[in,out] v Byte to rotate in place (was (HL) or a register).
 * \return The old bit 7, i.e. the carry flag RLC sets (was Carry).
 */
static u8 rlc8(u8 *v)
{
  u8 old_bit7; /* bit that rotates out of the top and back in at the bottom */

  old_bit7 = (u8) ((*v >> 7) & 1);
  *v       = (u8) ((*v << 1) | old_bit7);

  return old_bit7;
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
  int  row;      /* bitmap row counter, 64 down to 1 (was B) */
  u16  DE;       /* current screen row address (was DE) */
  u8   Dattr;    /* attribute row address high byte (was D after RRCA x3) */
  u16  attraddr; /* current attribute row address (was DE in the attr loop) */
  int  attrrow;  /* attribute row counter, 8 down to 1 (was A) */

  DE = screen_addr;

  for (row = 64; row != 0; row--) {
    memcpy(ADDRTOSCREEN(DE), image, 13);
    image += 13;
    DE = next_screen_row(DE);
  }

  Dattr    = (u8) (screen_addr >> 8); /* original D, pre-rotate */
  Dattr    = (u8) ((((Dattr >> 3) | (Dattr << 5)) & 0x03) + 0xEF); /* RRCA x3; AND 3; ADD $EF */
  attraddr = (u16) ((Dattr << 8) | (screen_addr & 0xFF));

  for (attrrow = 8; attrrow != 0; attrrow--) {
    memcpy(ADDRTOSCREEN(attraddr), image, 13);
    image += 13;
    attraddr = (u16) (attraddr + 19);
  }
}

/**
 * Resolve a script-embedded end-shot bitmap address to its C data array.
 *
 * Conv: the original walks a real (relocated) Z80 pointer; script_data only
 * ever encodes these four literal addresses (see the ESCMD_DRAW_FRAME
 * entries above), so a small lookup replaces pointer arithmetic into
 * relocated bank memory the C port does not model byte-for-byte.
 *
 * \param[in] addr Raw address word read from the script (was HL).
 * \return Matching bitmap_endshot_N array.
 */
static const u8 *resolve_endshot(u16 addr)
{
  switch (addr) {
  case 0x60E1: return bitmap_endshot_1;
  case 0x6489: return bitmap_endshot_2;
  case 0x6831: return bitmap_endshot_3;
  case 0x6BD9: return bitmap_endshot_4;
  default:     return bitmap_endshot_1; /* ponytail: script_data never encodes any other value */
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
  const u8 *HLscript; /* script read pointer (was HL) */
  u16       image_addr;  /* raw bitmap address word read from script */
  u16       screen_addr; /* destination screen address word read from script */

  HLscript = *script;

  image_addr  = (u16) (HLscript[0] | (HLscript[1] << 8));
  screen_addr = (u16) (HLscript[2] | (HLscript[3] << 8));
  HLscript += 4;

  draw_endshot(state, resolve_endshot(image_addr), screen_addr);

  *script = HLscript;
}

/**
 * $E2D9: Clear the backbuffer, then draw an end-screen graphic frame
 *
 * \param[in]     state  Pointer to game state.
 * \param[in,out] script Script read pointer (was HL); advanced past the two
 *                        words consumed.
 */
static void es_handler_draw_frame(chqstate_t *state, const u8 **script)
{
  es_clear(state);
  es_draw_frame_common(state, script);
}

/**
 * $E42E routine_e42e: Sweep the middle attribute band toward the backbuffer
 * target colours
 *
 * Gate: only runs every other call (rlc8 flip-flops $5C6C; returns
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
static void es_handler_glyph_sweep(chqstate_t *state)
{
  u8 *HLattr;   /* current attribute cell (was HL) */
  u8 *DEback;   /* current backbuffer cell (was DE) */
  int cell;     /* attribute cell counter, 512 down to 0 (was H reaching $5B) */
  u8  A;        /* working accumulator (was A) */
  u8  B_target; /* masked target colour read from the backbuffer (was B) */
  u8  C_ink;    /* merged ink field (was C) */

  if (rlc8(&state->bank7->es_flag_5c6c))
    return;

  HLattr = ADDRTOATTRS(0x5900);
  DEback = ADDRTOBACKBUF(0xF000);

  for (cell = 512; cell != 0; cell--, HLattr++, DEback++) {
    if (*HLattr & 0x40) /* BRIGHT set: leave this cell untouched */
      continue;

    A = *DEback & 0x3F;
    if (A == *HLattr) {
      *HLattr = *DEback;
      continue;
    }

    B_target = A;

    C_ink = *HLattr & 0x07;
    if ((B_target & 0x07) != C_ink)
      C_ink++;

    A = *HLattr & 0x38;
    if ((B_target & 0x38) != A)
      A = (u8) (A + 0x08);

    *HLattr = (u8) (A | C_ink);
  }
}

/**
 * $E475: Shared fade-to-black tail for routine_e472/routine_e46d
 *
 * Sweeps the same 512-cell attribute band as es_handler_glyph_sweep,
 * decrementing each cell's ink field by 1 (floor 0) and paper field by one
 * unit (floor 0) every call it runs. Gated by rlc8 on *flag -- $5C6C for
 * routine_e472 (GLYPH_B, also called directly by the handshake handler),
 * $5C6D for routine_e46d (GLYPH_C).
 *
 * Conv: unlike es_handler_glyph_sweep, BRIGHT/FLASH are never tested here --
 * the original ANDs each byte down to its ink/paper fields before OR-ing
 * them back together, which drops those bits on every write. Matched
 * bug-for-bug.
 *
 * \param[in] state Pointer to game state.
 * \param[in] flag  Flip-flop gate byte to rotate (was HL -> $5C6C/$5C6D).
 */
static void es_glyph_fade_common(chqstate_t *state, u8 *flag)
{
  u8 *HLattr; /* current attribute cell (was HL) */
  int cell;   /* attribute cell counter, 512 down to 0 (was D pages) */
  u8  A;      /* working accumulator (was A) */
  u8  B_ink;  /* new ink field (was B) */

  if (!rlc8(flag))
    return;

  HLattr = ADDRTOATTRS(0x5900);

  for (cell = 512; cell != 0; cell--, HLattr++) {
    A = *HLattr;
    if (A == 0)
      continue;

    B_ink = A & 0x07;
    if (B_ink != 0)
      B_ink--;

    A &= 0x38;
    if (A != 0)
      A = (u8) (A - 0x08);

    *HLattr = (u8) (A | B_ink);
  }
}

/**
 * $E472 routine_e472: Fade the $5C6C-gated glyph attribute band
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_glyph_fade_b(chqstate_t *state)
{
  es_glyph_fade_common(state, &state->bank7->es_flag_5c6c);
}

/**
 * $E46D routine_e46d: Fade the $5C6D-gated glyph attribute band
 *
 * \param[in] state Pointer to game state.
 */
static void es_handler_glyph_fade_c(chqstate_t *state)
{
  es_glyph_fade_common(state, &state->bank7->es_flag_5c6d);
}

/* $E3A5 handshake_table: row-count + source bitmap per animation frame,
 * cycling 1-2-3-4-3-2 (see routine_e3b7 below). */
static const struct {
  u8         rows;
  const u8  *image;
} handshake_table[6] = {
  { 37, bitmap_handshake_1 },
  { 35, bitmap_handshake_2 },
  { 34, bitmap_handshake_3 },
  { 32, bitmap_handshake_4 },
  { 34, bitmap_handshake_3 },
  { 35, bitmap_handshake_2 },
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
  u8        A_index; /* frame index 0..5, wrapped (was A/B) */
  const u8 *HLimage;  /* handshake bitmap source, walked forward (was HL) */
  u16       DE;       /* screen destination address (was DE) */
  int       row;      /* bitmap row counter for this frame (was B) */
  int       blank;    /* blank-row counter, 3 down to 0 (was C) */
  u8       *HLattr;    /* decorative attribute cell (was HL) */
  int       group;     /* decorative attribute group counter, 5 down to 0 (was C) */

  es_handler_glyph_fade_b(state);

  if (rlc8(&state->bank7->es_flag_5c6d)) {
    A_index = state->bank7->es_handshake_index;
    state->bank7->es_handshake_index = (u8) (A_index + 1 == 6 ? 0 : A_index + 1);

    HLimage = handshake_table[A_index].image;
    DE      = 0x48AC;

    for (row = handshake_table[A_index].rows; row != 0; row--) {
      memcpy(ADDRTOSCREEN(DE), HLimage, 8);
      HLimage += 8;
      DE = next_screen_row(DE);
    }

    for (blank = 3; blank != 0; blank--) {
      memset(ADDRTOSCREEN(DE), 0, 8);
      DE = next_screen_row(DE);
    }
  }

  HLattr = ADDRTOATTRS(0x59AC);
  for (group = 5; group != 0; group--) {
    memset(HLattr, 0x07, 8);
    HLattr += 0x20; /* 8-byte fill + $0018 stride, matches ADD HL,DE */
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
  int       tally;   /* bonus-tally animation iteration counter (was BC) */
  const u8 *DEbcd;   /* packed-BCD score pointer, walked backwards (was DE) */
  u8       *HLdst;   /* destination ASCII bytes in script_data (was HL) */
  u8        Cseen;   /* sticky "non-blank digit already printed" flag (was C) */
  int       pair;    /* BCD byte-pair iteration counter (was B) */
  u8        Anibble; /* nibble being converted to ASCII (was A) */

  for (tally = 1000; tally != 0; tally--) {
    increment_score(state, 0, 0x00, 0x50);
    ptad_led_digits(4, &state->score_bcd[3], &state->session.score_digits[7],
                     ADDRTOSCREEN(0x4126));
    sfx_bipbow(state, 2, 2);
  }

  state->bank7->es_input_mask = 1;

  DEbcd = &state->score_bcd[3];
  HLdst = &script_data[0xFD];
  Cseen = 0;

  for (pair = 4; pair != 0; pair--) {
    Anibble = (*DEbcd >> 4) & 0x0F;
    if (Anibble != 0 || Cseen != 0) {
      Cseen    = 0xFF;
      *HLdst++ = 0x30 + Anibble;
    } else {
      *HLdst++ = 0x20;
    }

    Anibble = *DEbcd & 0x0F;
    if (Anibble != 0 || Cseen != 0) {
      Cseen    = 0xFF;
      *HLdst++ = 0x30 + Anibble;
    } else {
      *HLdst++ = 0x20;
    }

    DEbcd--;
  }

  HLdst[-1] |= EOS; /* SET 7,(HL): mark this text run's terminator byte */
}

/**
 * Resolve a script-embedded $E2B2 argument word to its C data array.
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
static const u8 *resolve_call_word_target(u16 addr)
{
  switch (addr) {
  case 0x5C6E: return data_e06e;
  default:     return NULL; /* ponytail: script_data never encodes any other value */
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
static void es_handler_call_word(chqstate_t *state)
{
  const u8 *HLscript;    /* script read pointer (was HL) */
  u16       target_addr; /* raw argument word read from the script (was DE/HL) */
  const u8 *chatterblk;  /* resolved data block pointer (was HL after CALL $9945 setup) */

  HLscript = state->bank7->es_script_ptr;

  target_addr = (u16) (HLscript[0] | (HLscript[1] << 8));
  HLscript += 2;

  chatterblk = resolve_call_word_target(target_addr);

  /* Conv: data_e06e is not a well-formed chatterblk -- see prologue. Guard
   * against the confirmed CHATTERSTR__LIMIT overrun in pc_chatter_message
   * rather than reproduce the crash. */
  if (chatterblk != NULL && chatterblk[1] < CHATTERSTR__LIMIT)
    start_chatter(state, 1, chatterblk);

  state->bank7->es_script_ptr = HLscript;
}

/**
 * $E2CD rs_exit: Set the per-frame handler and its frame-delay reload
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
static void es_set_dispatch(chqstate_t *state, void (*handler)(chqstate_t *),
                             u8 reload)
{
  state->bank7->es_handler     = handler;
  state->bank7->es_frame_count = reload;
}

/**
 * $E2F0-$E3A4 render_text/plot_char: NOT PORTED -- analysis notes
 *
 * Command entry points: $E2F0 (ESCMD_11, falls through to $E2F5 after a
 * PUSH HL / CALL $6099 (es_clear) / POP HL prologue) and $E2F5 (ESCMD_10,
 * entered directly). $E2F5 reads C=row/height byte, then DE=screen dest
 * word from the script (3 bytes total), computes an attribute-row address
 * from D exactly as draw_endshot does (RRCA x3; AND 3; ADD $EF; combine
 * with E) into HL.
 *
 * The width-class table at $E328-$E34C was decoded and is NOT the
 * ambiguous part: it is instruction-for-instruction identical to the
 * existing glyphid-mapping ladder in draw_char ($9FEC, Main.c
 * dc_have_range/dc_have_single), and $E357 "LD HL,$A27A; ADD HL,BC" (with
 * BC = 7*glyphid) indexes the very same font[41*7] table already ported in
 * CommonData.c. A real port would reuse that mapping, not reinvent it.
 *
 * The blocking ambiguity is the register-banking structure from $E306
 * onward, which nests three independent EXX-driven state switches:
 *   1. $E306 EXX banks {C=row count, DE=screen dest, HL=attr addr} into the
 *      shadow set.
 *   2. $E307 EX (SP),HL then swaps the (now-active, ex-shadow) HL with the
 *      script pointer sitting on the stack (pushed at $E2FB) -- so "HL"
 *      immediately after this instruction means the script read cursor,
 *      while whatever was in shadow HL before step 1 is now on the stack,
 *      to be popped back at $E31A after the closing EXX.
 *   3. Every plot_char call ($E31F-$E3A4) performs its OWN internal EXX
 *      (at $E35B and $E39A/$E3A3) to reach back into the set banked in
 *      step 1 and mutate the persistent column cursor (E of screen dest,
 *      L of attr addr) for the NEXT character, while borrowing the
 *      PRE-increment value as this character's blit destination via a
 *      PUSH/EXX/POP shuffle ($E35C-$E35F).
 *
 * Direct-translating this requires either (a) modelling three coexisting
 * "shadow" register generations with the project's single-shadow-variable
 * EXX convention, which the convention as documented does not cover, or
 * (b) restructuring to explicit cursor-state parameters -- which is a
 * legitimate translation strategy but is a design decision, not a
 * mechanical port, and risks silently changing behaviour if the row-wrap
 * arithmetic ($E37A-$E37F: E += 0x1F, D -= 7) doesn't get carried over
 * exactly. Given the surrounding code already has next_screen_row() for an
 * extremely similar row-wrap pattern, a future attempt should start by
 * checking whether $E37A-E37F reduces to a variant of that helper before
 * inventing new arithmetic.
 *
 * Not attempted rather than guessed, per project convention: pixel/column
 * geometry here directly affects on-screen output, so an unverified guess
 * would look plausible while being wrong in a way that's hard to spot from
 * a screenshot (a font glyph one row tall or one column off).
 */

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
 * Conv: the handler bodies themselves (draw/glyph-plot/handshake/score) are
 * not yet ported -- see the TODO stubs above. This function faithfully
 * reproduces the command dispatch and byte consumption only.
 *
 * \param[in] state Pointer to game state.
 */
static void run_script(chqstate_t *state)
{
  const u8 *HLscript; /* script program counter (was HL) */
  u8        Acmd;     /* command byte just read (was A) */
  u8        Creload;  /* frame-delay reload value about to be applied (was C) */

  HLscript = state->bank7->es_script_ptr;

  for (;;) {
    Acmd = *HLscript++;

    switch (Acmd) {
    case ESCMD_DRAW_FRAME:
      es_handler_draw_frame(state, &HLscript);
      continue;

    case ESCMD_DRAW_WORD:
      /* $E2DE is $E2D9's tail half, entered directly for this command
       * (skipping E2D9's own backbuffer-clear prefix). */
      es_draw_frame_common(state, &HLscript);
      continue;

    case ESCMD_GLYPH_A:
      es_set_dispatch(state, es_handler_glyph_sweep, 16);
      goto rs_exit;

    case ESCMD_GLYPH_B:
      es_set_dispatch(state, es_handler_glyph_fade_b, 16);
      goto rs_exit;

    case ESCMD_HANDSHAKE:
      es_set_dispatch(state, es_handler_handshake, 16);
      goto rs_exit;

    case ESCMD_GLYPH_C:
      es_set_dispatch(state, es_handler_glyph_fade_c, 32);
      goto rs_exit;

    case ESCMD_IDLE:
      Creload = *HLscript++;
      es_set_dispatch(state, es_handler_idle, Creload);
      goto rs_exit;

    case ESCMD_SET_A172:
      Creload = *HLscript++;
      state->bank7->es_handshake_index = 0; /* $E2C0 LD ($A172),A with A=0 */
      es_set_dispatch(state, es_handler_handshake, Creload);
      goto rs_exit;

    case ESCMD_HANDSHAKE_AGAIN:
      Creload = *HLscript++;
      es_set_dispatch(state, es_handler_handshake, Creload);
      goto rs_exit;

    case ESCMD_10:
    case ESCMD_11:
      /* TODO: $E2F0-$E3A4 (render_text/plot_char) genuinely ambiguous,
       * not ported -- see the analysis note below this switch. Script
       * bytes are NOT consumed here (unlike the other unhandled-command
       * fallback), so this stub cannot be safely left wired into a real
       * script run -- it would desync the script pointer against every
       * later command. Not confirmed whether script_data ever actually
       * dispatches command byte 10 or 11 (a manual byte-by-byte trace of
       * every other command's consumption length would be needed to be
       * sure); treat this path as untested. */
      goto rs_exit;

    case ESCMD_CALL_WORD:
      es_handler_call_word(state);
      continue;

    case ESCMD_RENDER_SCORE:
      es_handler_draw_score(state);
      continue;

    default:
      /* Unrecognised command: matches the Z80 fallback (reset to a fixed
       * script offset) closely enough for a defensive stop. */
      goto rs_exit;
    }
  }

rs_exit:
  state->bank7->es_script_ptr = HLscript;
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
  u8  Ainput;             /* keyscan result, tested for the fire bit (was A) */
  int outer_count;        /* per-keyscan frame divider, reloads to 5/6 (was A171) */

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

    es_service_speech(state);
    if (--state->bank7->es_frame_count == 0)
      run_script(state);
    state->bank7->es_handler(state);

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
