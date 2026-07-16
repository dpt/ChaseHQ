/**
 * Bank3.c
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
 * The original game and design is copyright (c) 1988 Taito Corporation
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited
 * The recreated version is copyright (c) 2023-2026 David Thomas
 */

/* Notes
 *
 * This file holds the C translation of ZX Spectrum 128K "bank 3" -- the
 * animated title screen, its music/SFX service, and the keyboard/joystick
 * control-select and key-redefinition screens reached from it. These
 * routines are paged into $C000-$FFFF and dispatched from mainline code
 * (Main.c) via call_bank_3_128k, whose entry point and BANK3_* dispatch
 * constants are declared in Internal.h alongside the handful of low-level
 * helpers (z80addrtoscreen, z80addrtoattrs, setwordat) and the bank-4 title-
 * tune functions (start_tune, ts_music_service) shared with this file.
 */

#include <assert.h>
#include <string.h>

#include "C99/Types.h"
#include "ZXSpectrum/Spectrum.h"
#include "ZXSpectrum/Z80.h"

#include "ChaseHQ/ChaseHQ.h"
#include "ChaseHQ/Engine/Internal.h"
#include "ChaseHQ/Data/Stages.h"
#include "ChaseHQ/Engine/State.h"
#include "ChaseHQ/Data/CommonData.h"

/* ----------------------------------------------------------------------- */

#define BASL_JINGLE_FRAMES (0xB4) /* success-jingle duration; see boot_and_run_sound_loop Conv: */

/* state is always the enclosing function's chqstate_t* parameter. */
#define ADDRTOSCREEN(addr) z80addrtoscreen(state, addr, 0, 0)
#define ADDRTOATTRS(addr)  z80addrtoattrs(state, addr, 0, 0)

/* ----------------------------------------------------------------------- */

static u8   acp_read_byte(title_tune_channel_t *IX_channel,
                          const u8 **DE_pattern);
static void advance_channel_pattern(chqstate_t *state,
                                    title_tune_channel_t *IX_channel);
static u16  compute_channel_ay_registers(chqstate_t *state,
                                         title_tune_channel_t *IX_channel,
                                         u8 *A_volume_out);
static void start_tune(chqstate_t *state, u8 A_tune);
static void write_title_ay_registers(chqstate_t *state);
static void ts_music_service(chqstate_t *state);
static void setup_im2_interrupt_table(chqstate_t *state);
static void frame_interrupt_handler(chqstate_t *state);
static void start_tune_and_sfx_table(chqstate_t *state, u8 A_tune);
static void ts_animate_frame(chqstate_t *state);
static void sfx_music_service(chqstate_t *state);
static void clear_screen_bitmap_and_attrs(chqstate_t *state);
static void clear_and_fill_border_attrs(chqstate_t *state);
static void title_screen_driver(chqstate_t *state);
static u8   ts_wait_loop(chqstate_t *state);
static void ts_coin_inserted(chqstate_t *state);
static void ts_refresh_name_table(chqstate_t *state);
static u8   scan_keyboard_matrix(chqstate_t *state, u8 *D_key_code_out);
static u16  advance_key_label_column(u16 DE_screen);
static void read_new_key_definition(chqstate_t *state, u16 *DE_screen,
                                     u8 B_remaining, u8 C_control_index);
static void redefine_keys_screen(chqstate_t *state);
static void service_sound_and_loop_tune0(chqstate_t *state);
static u8   detect_kempston_joystick(chqstate_t *state);
static const u8 *print_character(chqstate_t *state, const u8 *HL_record);
static void print_string(chqstate_t *state, const u8 *HLstring);
static void clear_options_screen(chqstate_t *state);
static u8   omd_redraw_and_poll(chqstate_t *state);
static u8   options_menu_driver(chqstate_t *state);
static void boot_and_run_sound_loop(chqstate_t *state);

/* ----------------------------------------------------------------------- */

/**
 * $EDD6 (bank 3): Advance one title-tune channel's pattern by one tracker row
 *
 * Part of the 128K animated title screen's music driver. Called once per
 * channel, 3 times per tick, by ts_music_service ($EC71@bank3) once its tempo
 * counter reaches zero. Two responsibilities:
 *
 * 1. $EDD6-$EE48: if this channel's per-row wait countdown has not yet
 *    reached zero, only channel_slide_upkeep runs: nudges note_index by one
 *    semitone per frame while a portamento/slide effect (status bit 3) is
 *    active, direction taken from status bit 7.
 * 2. $EDDB-$EE95: otherwise, clears status and re-enters the pattern byte
 *    stream (pattern_ptr), reading and processing bytes one at a time until
 *    a note value (bit 7 clear) is found. Command/effect bytes (bit 7 set)
 *    are range-tested against a cascade of thresholds ($B0, $B8, $D0, $E0):
 *    below $B0 they dispatch through a dedicated per-byte handler (mixer
 *    bits, slide target, envelope params, vibrato mode, status bits, mute,
 *    row-duration reset); $B0-$B7 sets the tune tempo; $B8-$CF selects a
 *    pitch-offset sequence; $D0-$DF selects an envelope shape; $E0-$FF sets
 *    the per-row wait reload value. Every branch either loops back to read
 *    the next byte, or (a note value, or two of the fixed-length handlers)
 *    falls into acp_reset_row_counter: reload the wait countdown, store the
 *    advanced pattern pointer, and normalise a pending one-shot mute request.
 *
 * \param[in,out] state      Pointer to game state.
 * \param[in,out] IX_channel Pointer to this channel's tracker record. (was IX)
 *
 * Conv: $EE96 dispatch_pattern_command reaches the fixed-length handlers at
 * $ED36-$EDD1 via a computed jump through a table at $EC9D that stores a
 * 1-byte displacement per command byte (0x80-0xAF) rather than a full
 * address -- the same space-saving trick used by $EE59's pitch-offset table.
 * The skool could not resolve this statically ("no entry-point markers");
 * the mapping below was recovered by reading bank3.bin directly and
 * evaluating the displacement arithmetic for every byte value 0x80-0xAF.
 * Only 16 of the 48 possible values resolve to one of the named handlers'
 * entry points -- these become the `switch` cases below (0xA8 lands exactly
 * on acp_reset_row_counter's entry point, $EE22, even though it isn't one of
 * the primary pcmd_* handlers). Three further values are reachable but do
 * not target a handler entry point: 0x85 lands mid-instruction inside
 * pcmd_set_status_bits_3_7 (skipping its first SET 7, executing only SET 3),
 * 0x87 lands on the orphaned "JP $F1AE" at $ED33, and 0x8E lands on a bare
 * "POP HL; JP $ED0B" that would pop the real return address off the stack
 * and jump into stop_music_and_silence -- almost certainly a crash, so this
 * byte is assumed never to appear in real pattern data. The remaining values
 * (0x92-0xA7, 0xA9-0xAF) land on arbitrary bytes inside the handler block
 * and are equally assumed unused. All of these fall into the `default` case
 * below, which -- unlike the Z80 -- treats them as a no-op rather than
 * replicating undefined/crashing behaviour. See Translation notes for the
 * full derivation.
 *
 * Conv: the $F07C (pitch-offset sequence select, command bytes $B8-$CF) and
 * $F123 (envelope-shape select, command bytes $D0-$DF) lookup tables are not
 * yet ported to C -- both branches are TODO stubs that leave the affected
 * fields unchanged rather than asserting or dereferencing an invented table.
 *
 * Conv: pattern_ptr/pattern_base/pattern_len (State.h) are only populated for
 * tunes 0 and 1 (see start_tune) -- a channel with pattern_ptr == NULL (tunes
 * 2/3, not extracted) is treated as silent rather than dereferencing NULL.
 * Every read from the pattern stream goes through acp_read_byte, which wraps
 * the cursor back to pattern_base once it runs past pattern_base+pattern_len:
 * the extracted arrays only cover a finite prefix of the real Z80 data, whose
 * true loop point was not transcribed.
 */
static u8 acp_read_byte(title_tune_channel_t *IX_channel,
                         const u8 **DE_pattern)
{
  u8 A_byte; /* byte read before advancing the cursor (was A) */

  A_byte = *(*DE_pattern)++;
  if (*DE_pattern >= IX_channel->pattern_base + IX_channel->pattern_len)
    *DE_pattern = IX_channel->pattern_base; /* Conv: wrap to extracted prefix start */
  return A_byte;
}

/* Pitch-offset sequences: real data from bank3.bin's $F07C table
 * (24 x 1-byte self-referential-displacement entries, each pointing
 * to a byte sequence terminated by a bit-7-set marker byte). See
 * decode_pattern_command's pitch-select branch ($EE55-$EE6C). */
static const u8 title_pitch_offset_seq_00[] = { 0x80 };
static const u8 title_pitch_offset_seq_01[] = { 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };
static const u8 title_pitch_offset_seq_02[] = { 0x00, 0x04, 0x87 };
static const u8 title_pitch_offset_seq_03[] = { 0x00, 0x03, 0x88 };
static const u8 title_pitch_offset_seq_04[] = { 0x00, 0x05, 0x89 };
static const u8 title_pitch_offset_seq_05[] = { 0x00, 0x00, 0x00, 0x03, 0x03, 0x83 };
static const u8 title_pitch_offset_seq_06[] = { 0x00, 0x00, 0x00, 0x05, 0x05, 0x85 };
static const u8 title_pitch_offset_seq_07[] = { 0x00, 0x00, 0x00, 0x04, 0x04, 0x84 };
static const u8 title_pitch_offset_seq_08[] = { 0x00, 0x8C };
static const u8 title_pitch_offset_seq_09[] = { 0x00, 0x00, 0x00, 0x08, 0x08, 0x88 };
static const u8 title_pitch_offset_seq_10[] = { 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };
static const u8 title_pitch_offset_seq_11[] = { 0x00, 0x00, 0x03, 0x03, 0x07, 0x87 };
static const u8 title_pitch_offset_seq_12[] = { 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };
static const u8 title_pitch_offset_seq_13[] = { 0x00, 0x05, 0x07, 0x8C };
static const u8 title_pitch_offset_seq_14[] = { 0x00, 0x05, 0x0A, 0x8C };
static const u8 title_pitch_offset_seq_15[] = { 0x00, 0x05, 0x09, 0x8C };
static const u8 title_pitch_offset_seq_16[] = { 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x07, 0x07, 0x07, 0x07, 0x0C, 0x0C, 0x0C, 0x8C };
static const u8 title_pitch_offset_seq_17[] = { 0x1C, 0x01, 0x0B, 0x01, 0xFC };
static const u8 title_pitch_offset_seq_18[] = { 0xEF };
static const u8 title_pitch_offset_seq_19[] = { 0xC8 };
static const u8 title_pitch_offset_seq_20[] = { 0x51, 0x01, 0x3E, 0x01, 0x2C, 0x01, 0x1C, 0x01, 0x0B, 0x01, 0xFC };
static const u8 title_pitch_offset_seq_21[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };
static const u8 title_pitch_offset_seq_22[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };
static const u8 title_pitch_offset_seq_23[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };

/* Envelope-shape sequences: real data from bank3.bin's $F123 table
 * (16 x 2-byte pointers; byte at ptr-1 is the envelope speed, sequence
 * runs from ptr until a bit-7-set halt marker). Entries 11-15 have no
 * valid pointer in bank3.bin (never referenced by tunes 0/1's
 * extracted pattern data) -- stubbed silent/single-halt-byte. See
 * decode_pattern_command's envelope-select branch ($EE7E-$EE93). */
static const u8 title_envelope_shape_00[] = { 0x0F, 0x0F, 0x0E, 0x0D, 0x0C, 0x08, 0x87 };
static const u8 title_envelope_shape_01[] = { 0x0E, 0x0F, 0x0E, 0x87 };
static const u8 title_envelope_shape_02[] = { 0x0E, 0x0F, 0x0B, 0x87 };
static const u8 title_envelope_shape_03[] = { 0x0F, 0x0F, 0x0F, 0x0E, 0x0D, 0x0A, 0x87 };
static const u8 title_envelope_shape_04[] = { 0x0F, 0x09, 0x08, 0x06, 0x05, 0x04, 0x03, 0x87 };
static const u8 title_envelope_shape_05[] = { 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x06, 0x87 };
static const u8 title_envelope_shape_06[] = { 0x0F, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x87 };
static const u8 title_envelope_shape_07[] = { 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x87 };
static const u8 title_envelope_shape_08[] = { 0x0F, 0x0E, 0x0C, 0x06, 0x87 };
static const u8 title_envelope_shape_09[] = { 0x0F, 0x0D, 0x0C, 0x0A, 0x08, 0x07, 0x06, 0x87 };
static const u8 title_envelope_shape_10[] = { 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x87 };
static const u8 title_envelope_shape_11[] = { 0x80 };
static const u8 title_envelope_shape_12[] = { 0x80 };
static const u8 title_envelope_shape_13[] = { 0x80 };
static const u8 title_envelope_shape_14[] = { 0x80 };
static const u8 title_envelope_shape_15[] = { 0x80 };

static const struct { const u8 *base; u16 len; } pitch_offset_table[24] = {
  { title_pitch_offset_seq_00, sizeof(title_pitch_offset_seq_00) },
  { title_pitch_offset_seq_01, sizeof(title_pitch_offset_seq_01) },
  { title_pitch_offset_seq_02, sizeof(title_pitch_offset_seq_02) },
  { title_pitch_offset_seq_03, sizeof(title_pitch_offset_seq_03) },
  { title_pitch_offset_seq_04, sizeof(title_pitch_offset_seq_04) },
  { title_pitch_offset_seq_05, sizeof(title_pitch_offset_seq_05) },
  { title_pitch_offset_seq_06, sizeof(title_pitch_offset_seq_06) },
  { title_pitch_offset_seq_07, sizeof(title_pitch_offset_seq_07) },
  { title_pitch_offset_seq_08, sizeof(title_pitch_offset_seq_08) },
  { title_pitch_offset_seq_09, sizeof(title_pitch_offset_seq_09) },
  { title_pitch_offset_seq_10, sizeof(title_pitch_offset_seq_10) },
  { title_pitch_offset_seq_11, sizeof(title_pitch_offset_seq_11) },
  { title_pitch_offset_seq_12, sizeof(title_pitch_offset_seq_12) },
  { title_pitch_offset_seq_13, sizeof(title_pitch_offset_seq_13) },
  { title_pitch_offset_seq_14, sizeof(title_pitch_offset_seq_14) },
  { title_pitch_offset_seq_15, sizeof(title_pitch_offset_seq_15) },
  { title_pitch_offset_seq_16, sizeof(title_pitch_offset_seq_16) },
  { title_pitch_offset_seq_17, sizeof(title_pitch_offset_seq_17) },
  { title_pitch_offset_seq_18, sizeof(title_pitch_offset_seq_18) },
  { title_pitch_offset_seq_19, sizeof(title_pitch_offset_seq_19) },
  { title_pitch_offset_seq_20, sizeof(title_pitch_offset_seq_20) },
  { title_pitch_offset_seq_21, sizeof(title_pitch_offset_seq_21) },
  { title_pitch_offset_seq_22, sizeof(title_pitch_offset_seq_22) },
  { title_pitch_offset_seq_23, sizeof(title_pitch_offset_seq_23) },
};

static const struct { const u8 *base; u16 len; u8 speed; } envelope_shape_table[16] = {
  { title_envelope_shape_00, sizeof(title_envelope_shape_00), 0x01 },
  { title_envelope_shape_01, sizeof(title_envelope_shape_01), 0x02 },
  { title_envelope_shape_02, sizeof(title_envelope_shape_02), 0x02 },
  { title_envelope_shape_03, sizeof(title_envelope_shape_03), 0x04 },
  { title_envelope_shape_04, sizeof(title_envelope_shape_04), 0x04 },
  { title_envelope_shape_05, sizeof(title_envelope_shape_05), 0x00 },
  { title_envelope_shape_06, sizeof(title_envelope_shape_06), 0x02 },
  { title_envelope_shape_07, sizeof(title_envelope_shape_07), 0x06 },
  { title_envelope_shape_08, sizeof(title_envelope_shape_08), 0x00 },
  { title_envelope_shape_09, sizeof(title_envelope_shape_09), 0x01 },
  { title_envelope_shape_10, sizeof(title_envelope_shape_10), 0x02 },
  { title_envelope_shape_11, sizeof(title_envelope_shape_11), 0x00 },
  { title_envelope_shape_12, sizeof(title_envelope_shape_12), 0x00 },
  { title_envelope_shape_13, sizeof(title_envelope_shape_13), 0x00 },
  { title_envelope_shape_14, sizeof(title_envelope_shape_14), 0x00 },
  { title_envelope_shape_15, sizeof(title_envelope_shape_15), 0x00 },
};

static void advance_channel_pattern(chqstate_t *state,
                                    title_tune_channel_t *IX_channel)
{
  u8         A_status;    /* status byte tested by channel_slide_upkeep (was A, IX+$00) */
  const u8  *DE_pattern;  /* pattern-command byte stream cursor (was DE, IX+$01/$02) */
  u8         A_byte;      /* raw byte just read from the pattern stream (was A) */
  u8         A_note;      /* raw note value + transpose, before storing to note_index (was A) */
  const u8  *HL_ptr;      /* scratch pointer, reused for the pitch-offset then envelope-shape reset (was HL) */
  u8         A_env_byte;  /* envelope shape byte read at note time (was A) */
  u8         A_mix;       /* scratch accumulator for the replace-bits-under-mask mixer merge (was A) */
  u8         A_operand;   /* second operand byte of a 2-operand pattern command (was A) */

  if (IX_channel->pattern_ptr == NULL)
    return; /* Conv: tune not extracted (tunes 2/3) -- channel stays silent */

  /* $EDD6-$EDD9: per-row wait countdown. */
  if (--IX_channel->row_wait != 0) {
    /* $EE38 channel_slide_upkeep: nudge the note by one semitone per frame
     * while a portamento/slide effect is active. */
    A_status = IX_channel->status;
    if (!(A_status & 0x08))
      return;

    if (A_status & 0x80) // Conv: RLA + JR NC collapsed to a direct bit-7 test
      IX_channel->note_index++;
    else
      IX_channel->note_index--;
    return;
  }

  /* $EDDB-$EDE1: countdown reached zero -- start reading this row's pattern
   * bytes. */
  IX_channel->status = 0;
  DE_pattern = IX_channel->pattern_ptr;

  for (;;) {
    /* $EDE4 acp_read_pattern_byte. */
    A_byte = acp_read_byte(IX_channel, &DE_pattern);

    if (!(A_byte & 0x80)) {
      /* $EDEA-$EE1E: note value. */
      A_note = (u8) (A_byte + IX_channel->transpose);
      IX_channel->note_index = A_note;

      if (IX_channel->slide_update_flag & 0x01)
        state->title_music.shared_note_value = A_note; /* $EC79 (SM) */

      HL_ptr = IX_channel->pitch_offset_default;   /* +$09/$0A */
      IX_channel->pitch_offset_cur = HL_ptr;        /* +$0B/$0C: reset to loop start */

      HL_ptr = IX_channel->envelope_shape_default;  /* +$14/$15 */
      IX_channel->envelope_shape_ptr = HL_ptr;      /* +$16/$17: reset envelope pointer */

      A_env_byte = *HL_ptr;
      IX_channel->envelope_amplitude = A_env_byte;  /* +$18 */
      IX_channel->volume             = A_env_byte;  /* +$13 */

      IX_channel->envelope_step_counter = IX_channel->envelope_speed; /* +$19 = +$0F */

      IX_channel->status |= 0x20; /* bit5: envelope active */

      break; /* fall into acp_reset_row_counter below */
    }

    /* $EE49 decode_pattern_command: command/effect byte. */
    if (A_byte < 0xB0) {
      /* $EE96 dispatch_pattern_command -- see the Conv note above the
       * prologue for how this mapping was derived. */
      switch (A_byte) {
      case 0x80: /* pcmd_reset_row_counter_clear13 ($EDBC) */
        IX_channel->volume = 0;
        IX_channel->status &= (u8) ~0x20;
        goto reset_row_counter;

      case 0x81: /* pcmd_vibrato_off ($EDA5) */
        IX_channel->flags = 0;
        continue;

      case 0x82: /* pcmd_vibrato_on ($EDAA) */
        IX_channel->flags = 0x40;
        continue;

      case 0x83: /* pcmd_vibrato_on_mode2 ($EDB0) */
        IX_channel->flags = 0xC0;
        continue;

      case 0x84: /* pcmd_set_slide_target ($ED6F) */
        IX_channel->slide_step      = (s8) acp_read_byte(IX_channel, &DE_pattern); /* operand 1 */
        IX_channel->slide_accum     = 0;
        IX_channel->status         |= 0x04;               /* slide active */
        IX_channel->slide_countdown = acp_read_byte(IX_channel, &DE_pattern);      /* operand 2 */
        continue;

      case 0x86: /* pcmd_set_status_bits_3_7 ($ED9B) */
        IX_channel->status |= 0x88; /* bits 7 and 3 */
        continue;

      case 0x88: /* pcmd_set_envelope_params ($ED8C) */
        IX_channel->vibrato_increment = acp_read_byte(IX_channel, &DE_pattern); /* operand 1 -> +$1B */
        A_operand = acp_read_byte(IX_channel, &DE_pattern);                      /* operand 2, stored twice */
        IX_channel->vibrato_depth = A_operand;           /* +$1A */
        IX_channel->vibrato_phase = A_operand;           /* +$1C */
        continue;

      case 0x89: /* pcmd_set_driver_flag ($ED85) */
        state->title_music.pattern_driver_flag = acp_read_byte(IX_channel, &DE_pattern); /* $EED1 */
        continue;

      case 0x8A: /* pcmd_set_mixer_bits_high3 ($ED4B) */
        A_mix  = (u8) (IX_channel->mixer_mask & 0x38);
        A_mix ^= state->title_music.pending_mixer_bits; /* $EF7A */
        A_mix &= IX_channel->mixer_mask;
        A_mix ^= state->title_music.pending_mixer_bits;
        state->title_music.pending_mixer_bits = A_mix;
        IX_channel->slide_update_flag = 0;
        continue;

      case 0x8B: /* pcmd_set_mixer_bits_low3 ($ED36) */
        A_mix  = (u8) (IX_channel->mixer_mask & 0x07);
        A_mix ^= state->title_music.pending_mixer_bits; /* $EF7A */
        A_mix &= IX_channel->mixer_mask;
        A_mix ^= state->title_music.pending_mixer_bits;
        state->title_music.pending_mixer_bits = A_mix;
        IX_channel->slide_update_flag = 1;
        continue;

      case 0x8C: /* pcmd_clear_mixer_bits ($ED5F) */
        state->title_music.pending_mixer_bits =
          (u8) (~IX_channel->mixer_mask & state->title_music.pending_mixer_bits);
        IX_channel->slide_update_flag = 1;
        continue;

      case 0x8D: /* pcmd_set_status_bit1 ($EDB6) */
        IX_channel->status |= 0x02;
        continue;

      case 0x8F: /* pcmd_reset_row_counter ($EDC5) */
        IX_channel->status &= (u8) ~0x20;
        goto reset_row_counter;

      case 0x90: /* pcmd_mute_channel ($EDCB) */
        IX_channel->mute_pending = 0xFF;
        continue;

      case 0x91: /* pcmd_unmute_channel ($EDD1) */
        IX_channel->mute_pending = 0;
        continue;

      case 0xA8: /* lands exactly on acp_reset_row_counter ($EE22) */
        goto reset_row_counter;

      default:
        /* 0x85, 0x87, 0x8E and the rest of 0x92-0xAF: not a real handler
         * entry point -- see the Conv note above the prologue. Conv: treat
         * as a no-op. */
        continue;
      }
    } else if (A_byte < 0xB8) {
      /* $EE6F: set the tune tempo/speed byte. */
      state->title_music.tune_tempo = (u8) (A_byte - 0xB0 + 1); /* $EC9A */
      continue;
    } else if (A_byte < 0xD0) {
      /* $EE59-$EE6C: select a pitch-offset sequence via the $F07C table (24
       * entries, index A_byte-0xB8). Sets both the "current" and "default"
       * pointers immediately (unlike the envelope-shape select below, which
       * only sets the default). */
      HL_ptr = pitch_offset_table[A_byte - 0xB8].base;
      IX_channel->pitch_offset_cur     = HL_ptr; /* +$0B/$0C */
      IX_channel->pitch_offset_default = HL_ptr; /* +$09/$0A */
      continue;
    } else if (A_byte < 0xE0) {
      /* $EE7E-$EE93: select an envelope shape via the $F123 pointer table (16
       * entries, index A_byte-0xD0). Sets envelope_shape_default and
       * envelope_speed; envelope_shape_ptr is only reset from the default at
       * the next note event (see the note-value branch above). */
      IX_channel->envelope_shape_default = envelope_shape_table[A_byte - 0xD0].base; /* +$14/$15 */
      IX_channel->envelope_speed         = envelope_shape_table[A_byte - 0xD0].speed; /* +$0F */
      continue;
    } else {
      /* $EE77: set the per-row wait reload value. */
      IX_channel->row_wait_reload = (u8) (A_byte - 0xE0 + 1); /* +$11 */
      continue;
    }
  }

reset_row_counter:
  /* $EE22 acp_reset_row_counter. */
  IX_channel->row_wait    = IX_channel->row_wait_reload; /* +$10 = +$11 */
  IX_channel->pattern_ptr = DE_pattern;                   /* +$01/+$02 */

  if (IX_channel->mute_pending)
    IX_channel->mute_pending = 0xFF; /* normalise any nonzero value to the one-shot gate */
}

/**
 * $EE9E (bank 3): Recompute one title-tune channel's AY tone-period and
 * volume/mixer values
 *
 * Part of the 128K animated title screen's music driver (distinct from the
 * 48K play_music_48k engine, which happens to share this address in a
 * different skool/bank). Called once per channel per frame by
 * ts_music_service ($EC71@bank3) to refresh the AY register cache. Pure
 * calculation: no I/O, no state beyond the channel's
 * own tracker record and the shared mixer-cache byte. Five phases, in order:
 *
 * 1. $EEA1-$EED0: envelope amplitude advance. If status bit 5 is set,
 *    decrements the envelope-step counter; on underflow, reloads it from the
 *    speed value and reads the next byte from the envelope-shape table
 *    pointer (advanced by 1 unless the byte read has bit 7 set, an
 *    end-of-table marker that halts the pointer), storing that byte as the
 *    current amplitude. Either way, the amplitude is copied into the volume
 *    field returned to the caller.
 * 2. $EED0-$EEFC: tone-period lookup. Note index (set elsewhere from the
 *    pattern's raw note + transpose) plus a pitch-offset byte read from a
 *    per-channel offset sequence (default/loop start on end-of-sequence)
 *    indexes the 2-byte-per-note tone-period table, giving the base tone
 *    period.
 * 3. $EEFC-$EF4B: vibrato. Only runs if flags bit 6 is set. The vibrato
 *    phase counter ping-pongs between 0 and (depth * 2) by +/-increment each
 *    call. The phase is centred, sign-extended, then left-shifted a
 *    note-dependent number of times to scale a linear vibrato depth into the
 *    tone-period domain (pitch and period are inversely related), and the
 *    scaled offset is added onto the phase-2 tone period.
 * 4. $EF4B-$EF76: portamento/slide. Status bit 0 is unconditionally toggled
 *    every call. If status bit 2 was set (slide active), a countdown ticks
 *    down; when it reaches zero it reloads and the signed step is
 *    sign-extended and accumulated, which is then added onto the tone
 *    period.
 * 5. $EF76-$EFAE: mixer/volume finalisation. Every 4th call, XORs bit 3 of a
 *    shared byte into a self-modified operand elsewhere in the driver
 *    (purpose not established). Merges this channel's tone-enable bits into
 *    the shared mixer cache using a replace-bits-under-mask idiom. If a
 *    one-shot mute was just requested, clears this channel's noise-enable
 *    bits in the mixer cache and forces that same self-modified operand to
 *    $41.
 *
 * \param[in]  state       Pointer to game state.
 * \param[in]  IX_channel  Pointer to this channel's tracker record. (was IX)
 * \param[out] A_volume_out Receives the phase-1/-2 volume (+$13). (was A)
 *
 * \return The phase-3/4 tone period. (was HL)
 *
 * Conv: the Z80 pairs of EX DE,HL ($EF32/$EF4A, $EFAA) exist only to route
 * operands through ADD HL,DE / RET's register contract; C adds the values
 * directly and returns via the function's return value instead.
 *
 * Conv: at $EED0-$EED2 the Z80 does `LD A,$00; ADD A,(IX+$12)`, which is a
 * roundabout way of just reading the note index; collapsed to a direct
 * assignment here.
 *
 * Conv: at $EF33-$EF3D the Z80 manually sign-extends the centred vibrato
 * phase into D via the carry flag from the preceding SUB. C's (s8)->(s16)
 * cast performs the same sign extension natively.
 */
static u16 compute_channel_ay_registers(chqstate_t *state,
                                        title_tune_channel_t *IX_channel,
                                        u8 *A_volume_out)
{
  u8         C_status;         /* channel status/flags byte, unchanged through most of the function (was C, IX+$00) */
  u8         A_env_step;       /* envelope-step counter, then reload value (was A, IX+$19) */
  const u8  *HL_env_shape;     /* envelope shape table pointer (was HL, IX+$16/$17) */
  u8         A_env_byte;       /* envelope shape byte just read (was A) */
  u8         B_note;           /* base note index for the tone lookup (was B, IX+$12) */
  const u8  *HL_offset_ptr;    /* pitch-offset sequence pointer (was HL, IX+$0B/$0C) */
  u8         A_offset_byte;    /* pitch-offset byte read from the sequence (was A) */
  u8         A_note_combined;  /* offset byte + note index, before doubling (was A) */
  u8         A_note_lookup;    /* doubled combined index: byte offset into the tone-period table, reused in phase 3 as the vibrato scale seed (was A/H) */
  u16        DE_period;        /* running tone period (was DE) */
  u8         B_vib_range;      /* doubled vibrato depth; clamp bound for the phase counter (was B, IX+$1A after SLA) */
  u8         A_vib_phase;      /* vibrato triangle-wave phase counter (was A, IX+$1C) */
  s16        DEvib_offset;     /* signed, scaled vibrato pitch offset (was DE) */
  u16        A_shift_test;     /* shift-loop overflow accumulator (was A, tested via carry) */
  u8         B_slide_countdown; /* portamento reload countdown (was B, IX+$0E) */
  s8         C_slide_step;     /* signed per-tick portamento step (was C, IX+$0D) */
  u16        HL_slide_accum;   /* accumulated portamento/slide value (was HL, IX+$07/$08) */
  u8         A_status_new;     /* status with bit 0 toggled; stored back and re-tested in phase 5 (was A) */
  u8         A_mixer_test;     /* ~status & 3; nonzero except every 4th call (was A) */
  u8         A_shared;         /* shared driver byte, XORed and forwarded (was A, $EC79) */
  u8         A_mixer_val;      /* value merged into the shared mixer cache (was A) */
  u8         A_mute_flag;      /* one-shot mute-transition gate (was A, IX+$1F) */
  u8         A_noise_mask;     /* this channel's noise-enable bits, complemented for clearing (was A) */

  /* $EE9E-$EED0: phase 1, envelope amplitude advance. */
  C_status = IX_channel->status;
  if (C_status & 0x20) {
    A_env_step = IX_channel->envelope_step_counter;
    if (A_env_step == 0) {
      /* Underflow: reload the counter and try to advance the envelope shape
       * pointer. */
      A_env_step = IX_channel->envelope_speed;
      IX_channel->envelope_step_counter = A_env_step;

      HL_env_shape = IX_channel->envelope_shape_ptr;
      HL_env_shape++;
      A_env_byte = *HL_env_shape;
      if ((s8) A_env_byte >= 0) {
        /* Not an end-of-table marker: commit the advance. */
        IX_channel->envelope_shape_ptr = HL_env_shape;
        IX_channel->envelope_amplitude = A_env_byte;
      }
      /* Else: halt the pointer, keep the previous amplitude. */
    } else {
      IX_channel->envelope_step_counter = A_env_step - 1;
    }
    IX_channel->volume = IX_channel->envelope_amplitude;
  }

  /* $EED0-$EEFC: phase 2, tone-period lookup. */
  B_note = IX_channel->note_index; // Conv: LD A,$00; ADD A,(IX+$12) collapsed to a direct read

  HL_offset_ptr = IX_channel->pitch_offset_cur;
  A_offset_byte = *HL_offset_ptr;
  HL_offset_ptr++;
  if ((s8) A_offset_byte < 0) {
    /* End-of-sequence marker: reset to the default/loop-start pointer and
     * strip the marker bit. */
    HL_offset_ptr = IX_channel->pitch_offset_default;
    A_offset_byte &= 0x7F;
  }
  IX_channel->pitch_offset_cur = HL_offset_ptr;

  A_note_combined = (u8) (A_offset_byte + B_note);
  A_note_lookup   = (u8) (A_note_combined * 2); // ADD A,A: 8-bit doubling, wraps mod 256
  assert((A_note_lookup >> 1) < NELEMS(title_tune_note_periods));
  DE_period = title_tune_note_periods[A_note_lookup >> 1];

  /* $EEFC-$EF4B: phase 3, vibrato. */
  if (IX_channel->flags & 0x40) {
    B_vib_range = IX_channel->vibrato_depth;
    B_vib_range <<= 1;

    A_vib_phase = IX_channel->vibrato_phase;
    if (!((IX_channel->flags & 0x80) && (C_status & 0x01))) {
      /* Conv: $EF09-$EF12 gate whether the phase updates this call at all
       * (runs every other call); not fully resolved against source, mirrored
       * literally from the skool. */
      if (!(IX_channel->flags & 0x20)) {
        /* Descending. */
        if (A_vib_phase >= IX_channel->vibrato_increment) {
          A_vib_phase -= IX_channel->vibrato_increment;
        } else {
          IX_channel->flags |= 0x20; // flip to ascending
          A_vib_phase = 0;
        }
      } else {
        /* Ascending. */
        A_vib_phase += IX_channel->vibrato_increment;
        if (A_vib_phase >= B_vib_range) {
          IX_channel->flags &= ~0x20; // flip to descending
          A_vib_phase = B_vib_range;
        }
      }
      IX_channel->vibrato_phase = A_vib_phase;
    }

    // Conv: EX DE,HL ($EF32/$EF4A) folded -- add the scaled offset directly.
    DEvib_offset = (s8) (A_vib_phase - (B_vib_range >> 1)); // centre phase; (s8)->(s16) sign-extends

    A_shift_test = (u16) A_note_lookup + 0xA0;
    if (A_shift_test <= 0xFF) {
      do {
        DEvib_offset <<= 1;
        A_shift_test = (A_shift_test & 0xFF) + 0x18;
      } while (A_shift_test <= 0xFF);
    }

    DE_period += DEvib_offset;
  }

  /* $EF4B-$EF76: phase 4, portamento/slide. */
  A_status_new = C_status ^ 0x01;
  IX_channel->status = A_status_new;

  if (C_status & 0x04) {
    B_slide_countdown = IX_channel->slide_countdown;
    B_slide_countdown--;
    if (B_slide_countdown != 0) {
      IX_channel->slide_countdown = B_slide_countdown;
    } else {
      C_slide_step = IX_channel->slide_step;

      HL_slide_accum = IX_channel->slide_accum;
      HL_slide_accum += (s16) C_slide_step; // sign-extended add
      IX_channel->slide_accum = HL_slide_accum;

      DE_period += HL_slide_accum;
    }
  }

  /* $EF76-$EFAE: phase 5, mixer/volume finalisation. */
  A_mixer_test = (u8) ~A_status_new & 0x03;
  if (A_mixer_test == 0) {
    /* Every 4th call. */
    A_shared = state->title_music.shared_note_value ^ 0x08; // $EC79
    state->title_music.driver_internal_flag = A_shared;      // $ECC6 (SM)
    A_mixer_val = 0x07;
  } else {
    // Conv: $EF79 "LD A,$00" reads its own self-modified operand byte
    // ($EF7A, pending_mixer_bits), not a literal 0 -- see State.h. A literal
    // 0x00 here permanently forces every channel's noise-enable bit on,
    // producing constant harsh noise; the real driver patches this operand
    // via advance_channel_pattern's mixer-bit commands.
    A_mixer_val = state->title_music.pending_mixer_bits; // $EF7A (SM)
  }

  /* Replace-bits-under-mask: merge this channel's tone-enable bits into the
   * shared mixer cache without disturbing the other channels' bits. */
  A_mixer_val ^= state->title_ay_regs.mixer;
  A_mixer_val &= IX_channel->mixer_mask;
  A_mixer_val ^= state->title_ay_regs.mixer;
  state->title_ay_regs.mixer = A_mixer_val;

  A_mute_flag = IX_channel->mute_pending;
  if ((s8) A_mute_flag < 0) {
    /* One-shot mute transition. */
    A_mute_flag &= 0x7F;
    IX_channel->mute_pending = A_mute_flag;

    A_noise_mask = IX_channel->mixer_mask & 0x38;
    A_noise_mask = (u8) ~A_noise_mask;
    state->title_ay_regs.mixer &= A_noise_mask;

    state->title_music.driver_internal_flag = 0x41; // $ECC6 (SM)
  }

  *A_volume_out = IX_channel->volume;
  return DE_period;
}

/**
 * $EB9E (bank 3): Start playing a tune (AY-3-8912 title-tune driver)
 *
 * Part of the 128K animated title screen's music driver. Looks up the given
 * tune's 7-byte entry (1 tempo/speed byte followed by 3 x 2-byte
 * pattern-data pointers, one per channel) in the tune-select table at
 * $F225, then initialises the 3 channel-tracker records at
 * $EC01/$EC26/$EC4B: resets misc playback state, stores each channel's raw
 * pattern-data pointer, follows it to read a 2-byte header from the start of
 * the pattern data itself (dereferenced into the channel's actual
 * pattern-command read pointer -- every pattern begins with this header),
 * sets the initial speed/divider and counter, and enables the channel.
 * Finally it clears the pattern_driver_flag scratch byte, forces an
 * immediate tempo refresh, and arms the tune-active flag for
 * ts_music_service ($EC71) to pick up on its next call.
 *
 * \param[in,out] state  Pointer to game state.
 * \param[in]     A_tune Tune number to start; index into the 7-byte-stride
 *                       tune-select table at $F225. (was A)
 *
 * Conv: $EBA6-$EBAB computes BC = A_tune * 7 via a repeated doubling/add
 * sequence (the Z80 has no multiply instruction); C uses a direct multiply.
 *
 * Conv: the Z80 counts a channel counter down from 3 to 1 in A while IX
 * walks the 37-byte-stride channel records ($EBBD/$EBF1); C counts
 * channel_index up from 0 to 2 and indexes state->title_music.channel[]
 * directly, which is equivalent and matches how advance_channel_pattern and
 * compute_channel_ay_registers already receive a channel pointer.
 *
 * Conv: the pattern-data blocks tune_select_table's pointers reference have
 * been extracted from bank3.bin as C data for tunes 0 and 1 only (the title
 * tune and the perp-caught success jingle -- the only tunes reachable from
 * code paths wired up so far; see title_tune0_ch*_pattern/title_tune1_ch*_pattern
 * in Data/CommonData.c). Each array covers a fixed 120-row prefix, not the
 * whole tune, so pattern_base/pattern_len are stored alongside pattern_ptr
 * (State.h, Conv fields with no Z80 counterpart) to let advance_channel_pattern
 * wrap back to the start once it runs off the end, rather than reading out of
 * bounds. Tunes 2 and 3 are not extracted; their channels are left with
 * pattern_ptr/pattern_base = NULL, which advance_channel_pattern must treat as
 * silent/idle. DE_pattern_addr (the raw Z80 pointer read from the table) is
 * still read for translation fidelity but is superseded by the tune_patterns
 * lookup below rather than dereferenced as a real address.
 *
 * Conv: pitch_offset_default/_cur and envelope_shape_default/_ptr are meant
 * to be set by the pattern stream's $F07C (pitch-offset select) and $F123
 * (envelope-shape select) commands, neither of which is ported yet
 * (advance_channel_pattern's decode_pattern_command TODO stubs for command
 * bytes $B8-$CF/$D0-$DF leave them unchanged). With real pattern data now
 * flowing, a note event dereferences both unconditionally
 * (advance_channel_pattern's note-value branch, compute_channel_ay_registers'
 * phase 1/2) -- leaving them NULL crashes on the first note. start_tune seeds
 * all 3 channels with default_pitch_offset_seq/default_envelope_shape (below):
 * synthetic single-entry tables, not transcribed Z80 data, that decode to "no
 * pitch offset" / "constant amplitude 15" so playback is audible and stable
 * rather than silent or crashing. TODO: replace with the real $F07C/$F123
 * tables once extracted.
 */
static const u8 default_pitch_offset_seq[] = { 0x80 };       /* Conv: marker bit set, payload 0 -- always resets to itself with zero offset */
static const u8 default_envelope_shape[]   = { 0x0F, 0x80 }; /* Conv: constant amplitude 15, then a halt marker */

static void start_tune(chqstate_t *state, u8 A_tune)
{
  static const struct { const u8 *base; u16 len; } tune_patterns[2][3] = {
    { { title_tune0_ch1_pattern, sizeof(title_tune0_ch1_pattern) },
      { title_tune0_ch2_pattern, sizeof(title_tune0_ch2_pattern) },
      { title_tune0_ch3_pattern, sizeof(title_tune0_ch3_pattern) } },
    { { title_tune1_ch1_pattern, sizeof(title_tune1_ch1_pattern) },
      { title_tune1_ch2_pattern, sizeof(title_tune1_ch2_pattern) },
      { title_tune1_ch3_pattern, sizeof(title_tune1_ch3_pattern) } }
  };                                      /* Conv: real pattern data for tunes 0/1; see prologue */
  int                    BC_offset;       /* byte offset into tune_select_table = A_tune * 7 (was BC) */
  const u8              *HL_tune_entry;   /* -> this tune's 7-byte entry in the tune-select table (was HL) */
  u8                     A_tempo;         /* this tune's tempo/speed byte, first byte of the entry (was A) */
  int                    channel_index;   /* channel 0..2 (was A, counted down 3..1 in the Z80) */
  title_tune_channel_t  *IX_channel;      /* this channel's tracker record (was IX) */
  u16                    DE_pattern_addr; /* raw Z80 address of this channel's pattern-data block, read from the tune-select table (was DE) */

  /* $EB9E-$EBA4: clear the tune-active flag and its companion byte. */
  state->title_music.tune_active           = 0; /* $F223 */
  state->title_music.tune_active_companion = 0; /* $F224 */

  /* $EBA6-$EBAB: BC = A_tune * 7 (the tune-select table's entry stride). */
  BC_offset = A_tune * 7; // Conv: collapses the ADD A,A/ADD A,C doubling sequence
  assert((BC_offset + 7) <= (int) NELEMS(tune_select_table));

  /* $EBAE-$EBB1: HL -> this tune's entry in the tune-select table. */
  HL_tune_entry = &tune_select_table[BC_offset];

  /* $EBB2-$EBB6: first byte = tempo/speed, saved for later use. */
  A_tempo = *HL_tune_entry;
  state->title_music.tune_tempo = A_tempo; /* $EC9A */
  HL_tune_entry++;

  /* $EBB7-$EBBD: IX -> first channel-tracker record; BC = 37 (record
   * stride, folded into array indexing below). */

  /* $EBBD-$EBF4 stu_channel_loop: initialise all 3 channel-tracker
   * records. */
  for (channel_index = 0; channel_index < 3; channel_index++) {
    IX_channel = &state->title_music.channel[channel_index];

    /* $EBBF-$EBC2: read this channel's pattern-data pointer from the tune
     * table. */
    DE_pattern_addr = wordat(HL_tune_entry);
    HL_tune_entry += 2;

    /* $EBC4-$EBC8: reset misc playback state for this channel. */
    IX_channel->transpose           = 0; /* +$20 */
    IX_channel->misc_playback_state = 0; /* +$21 */

    /* Conv: seed the pitch-offset/envelope-shape pointers with the synthetic
     * safe defaults (see prologue) rather than leaving them NULL. */
    IX_channel->pitch_offset_default  = default_pitch_offset_seq; /* +$09/$0A */
    IX_channel->pitch_offset_cur      = default_pitch_offset_seq; /* +$0B/$0C */
    IX_channel->envelope_shape_default = default_envelope_shape;  /* +$14/$15 */
    IX_channel->envelope_shape_ptr     = default_envelope_shape;  /* +$16/$17 */

    /* $EBCC: enable the channel. */
    IX_channel->row_wait = 1; /* +$10 */

    /* $EBD0-$EBD6: clear note/status. */
    IX_channel->status       = 0; /* +$00 */
    IX_channel->flags        = 0; /* +$1D */
    IX_channel->mute_pending = 0; /* +$1F */

    /* $EBD9-$EBDC: store the raw pattern-data pointer. */
    IX_channel->pattern_data_ptr = NULL; /* +$03/+$04: Conv: raw Z80 address
                                           * (DE_pattern_addr) is not modelled as a
                                           * dereferenceable C pointer; never read
                                           * elsewhere in bank 3 (see State.h). */

    /* $EBDF-$EBE2: follow the pattern pointer to read a second,
     * effect/envelope pointer from the start of the pattern data itself --
     * every pattern begins with an envelope-pointer header. */
    if (A_tune < NELEMS(tune_patterns)) {
      IX_channel->pattern_ptr  = tune_patterns[A_tune][channel_index].base;  /* +$01/+$02 */
      IX_channel->pattern_base = tune_patterns[A_tune][channel_index].base; /* Conv: wraparound base, see prologue */
      IX_channel->pattern_len  = tune_patterns[A_tune][channel_index].len;  /* Conv: wraparound length, see prologue */
    } else {
      IX_channel->pattern_ptr  = NULL; /* +$01/$02: tune not extracted -- treated as silent */
      IX_channel->pattern_base = NULL;
      IX_channel->pattern_len  = 0;
    }

    /* $EBE3-$EBE7: initial speed/divider = 2, counter = 0. */
    IX_channel->speed_divider = 2; /* +$05 */
    IX_channel->counter       = 0; /* +$06 */
  }

  /* $EBF6: clear a driver-internal flag. */
  state->title_music.pattern_driver_flag = 0; /* $EED1 */

  /* $EBF9-$EBFA: mark the tempo counter for an immediate refresh. */
  state->title_music.tempo_counter = 1; /* $EC70 */

  /* $EBFD: flag the tune as active. */
  state->title_music.tune_active = 1; /* $F223 */
}

/**
 * $ECD2 (bank 3): Flush the title-tune AY register cache to hardware
 *
 * Writes registers 11 down to 0 from the title-tune AY register soft-copy
 * block (title_ay_regs.env_fine..title_ay_regs.chan_a_pitch) by selecting
 * each register via port $FFFD then writing its value via port $BFFD. Same
 * shape as write_audio_registers_128k's flush of the separate in-game AY
 * block at $A213 -- kept as a distinct function because it walks the
 * title-tune engine's own register cache at $EFAF-$EFBA, not the in-game one.
 *
 * \param[in] state Pointer to game state.
 *
 * Conv: the Z80 uses the OUTD instruction (LD B,$FF / OUT (C),A / LD B,$BF /
 * OUTD in sequence); C issues two separate out() calls per register, as in
 * write_audio_registers_128k.
 */
static void write_title_ay_registers(chqstate_t *state)
{
  zxspectrum_t *speccy; /* ZX Spectrum callbacks (Conv: C-only) */
  const u8     *values; /* pointer walking title AY soft copies downward (was HL) */
  u8            regno;  /* AY register index, 11 down to 0 (was A) */

  speccy = state->speccy;
  values = &state->title_ay_regs.env_fine;
  regno  = 11;
  do {
    speccy->out(speccy, 0xFFFD, regno);
    speccy->out(speccy, 0xBFFD, *values--); /* was OUTD */
  } while ((s8) --regno >= 0);
}

/**
 * $EC71 (bank 3): Per-frame title-tune music service
 *
 * Top-level per-tick driver for the 128K title screen's tune engine. If no
 * tune is active (title_music.tune_active is 0), does nothing. Otherwise:
 * decrements the tempo counter ($EC70) and, once it reaches zero,
 * re-processes all 3 channel-tracker records (advance_channel_pattern) one
 * tracker row further and reloads the counter; then (every frame, tick or
 * not) recomputes the AY tone-period/volume register values for all 3
 * channels from their current tracker state (compute_channel_ay_registers)
 * into the register cache at title_ay_regs; finally flushes the full cached
 * register block to the AY chip via write_title_ay_registers.
 *
 * Called once per frame by the routine at $F82F.
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: the skool re-tests title_music.tune_active a second time at $ECCA
 * (tms_output_registers) before flushing, since that label is also reached
 * directly by the $EC75 JP Z when no tune is active. Nothing between the two
 * tests can change the flag, so the second check here is equivalent to the
 * first -- both are kept, matching the two-guard structure of the original.
 *
 * Conv: $EC99-$EC9B reloads the tempo counter with a fixed 1, not the tune's
 * own stored tempo/speed byte at title_music.tune_tempo -- see the skool
 * comment at $EC99; the driver always ticks every other frame regardless of
 * the selected tune.
 */
static void ts_music_service(chqstate_t *state)
{
  title_tune_channel_t *IX_channel; /* this channel's tracker record (was IX) */
  u16                   HL_period;  /* tone period returned per channel (was HL) */
  u8                    A_volume;   /* volume/mixer byte returned per channel (was A) */

  /* $EC71-$EC75: tune-active flag; skip straight to the output guard when no
   * tune is playing. */
  if (state->title_music.tune_active) {
    /* $EC78-$EC7A: clear a driver-internal flag (consumed elsewhere in the
     * pattern processing, not traced in the skool). */
    state->title_music.driver_internal_flag = 0x00; /* $ECC6 (SM) */

    /* $EC7D-$EC81: decrement the tempo counter; only re-process the tracker
     * patterns when it reaches zero. */
    if (--state->title_music.tempo_counter == 0) {
      /* $EC84-$EC88: advance channel 1's pattern by one tracker row. */
      IX_channel = &state->title_music.channel[0];
      advance_channel_pattern(state, IX_channel);

      /* $EC8B-$EC8F: channel 2. */
      IX_channel = &state->title_music.channel[1];
      advance_channel_pattern(state, IX_channel);

      /* $EC92-$EC96: channel 3. */
      IX_channel = &state->title_music.channel[2];
      advance_channel_pattern(state, IX_channel);

      /* $EC99-$EC9B: reset the tempo counter (see Conv note above). */
      state->title_music.tempo_counter = 1; /* $EC70 */
    }

    /* $EC9E-$ECA8 tms_refresh_registers: recompute the AY register values
     * for channel 1 from its current tracker state. */
    IX_channel = &state->title_music.channel[0];
    HL_period  = compute_channel_ay_registers(state, IX_channel, &A_volume);
    state->title_ay_regs.chan_a_pitch = HL_period; /* $EFAF/$EFB0 */
    state->title_ay_regs.chan_a_vol   = A_volume;  /* $EFB7 */

    /* $ECAB-$ECB5: channel 2. */
    IX_channel = &state->title_music.channel[1];
    HL_period  = compute_channel_ay_registers(state, IX_channel, &A_volume);
    state->title_ay_regs.chan_b_pitch = HL_period; /* $EFB1/$EFB2 */
    state->title_ay_regs.chan_b_vol   = A_volume;  /* $EFB8 */

    /* $ECB8-$ECC2: channel 3. */
    IX_channel = &state->title_music.channel[2];
    HL_period  = compute_channel_ay_registers(state, IX_channel, &A_volume);
    state->title_ay_regs.chan_c_pitch = HL_period; /* $EFB3/$EFB4 */
    state->title_ay_regs.chan_c_vol   = A_volume;  /* $EFB9 */

    /* $ECC5-$ECC7: the operand of this "LD A,$00" is $ECC6 --
     * driver_internal_flag -- the same self-modified byte that
     * compute_channel_ay_registers may have just patched (during any of the
     * three calls above, at $EF82 or $EFA7). So this reads back whatever
     * value was last patched there rather than actually loading a literal 0;
     * see the Conv note above and the driver_internal_flag field comment in
     * State.h. */
    state->title_ay_regs.noise_pitch = state->title_music.driver_internal_flag; /* $EFB5 */
  }

  /* $ECCA-$ECCE tms_output_registers: re-check the tune-active flag -- see
   * Conv note above -- registers are only ever written while a tune is
   * active. */
  if (!state->title_music.tune_active)
    return;

  /* $ECCF-$ECE2: output the cached register block. */
  write_title_ay_registers(state);
}

/**
 * $F7AA: Configure the 128K Z80 mode-2 interrupt vector table
 *
 * Fills the 257-byte interrupt vector table at $BC00-$BDBD with $BD (so every
 * vector points to $BDBD), then writes a JP $F8AD at $BDBD and sets I=$BC and
 * IM 2. Under mode 2 all interrupts are routed through $F8AD
 * (frame_interrupt_handler).
 *
 * Conv: Z80 interrupt wiring has no equivalent in C; SDL delivers events on
 * its own thread. This function is a no-op in the C port, matching the
 * existing 48K setup_interrupts stub.
 *
 * \param[in] state Pointer to game state.
 */
static void setup_im2_interrupt_table(chqstate_t *state)
{
  /* Conv: no equivalent in C — SDL owns interrupt delivery */
}

/**
 * $F8AD: Frame interrupt handler
 *
 * Z80 IM2 interrupt service routine installed by setup_im2_interrupt_table.
 * Sets the "frame occurred" flag at $F8A8, polled by wait_for_frame_flag, and
 * returns. The actual per-frame music/SFX work happens synchronously from the
 * title-screen main loop (ts_wait_loop / boot_and_run_sound_loop), not here.
 *
 * Conv: no equivalent in C — nothing in this port ever waits on the $F8A8
 * flag (the title-screen loops call sfx_music_service directly once per
 * paced iteration instead), so there is no flag to set. Adding one would be
 * dead state, the same way the 48K irq_flag field was.
 *
 * \param[in,out] state Pointer to game state.
 */
static void frame_interrupt_handler(chqstate_t *state)
{
  /* Conv: no equivalent in C — see prologue */
}

/**
 * $F7D6: Start a tune and arm its sound-effect trigger table
 *
 * Plays tune A_tune (via start_tune), looks up a pointer in the table at
 * $FA75 (indexed by A_tune*2) into a per-tune SFX script, and clears the 3
 * SFX "busy" flags at $F837/$F895/$F8A2 before falling into the SFX
 * script-byte-code reader.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in] A_tune Tune number to start (was A).
 *
 * Conv: the digitised-sample SFX subsystem is out of scope (per scope
 * decision — see sfx_music_service). Only the CALL $EB9E is translated; the
 * SFX trigger-table setup ($F7DB-$F82C) is stubbed.
 */
static void start_tune_and_sfx_table(chqstate_t *state, u8 A_tune)
{
  start_tune(state, A_tune);

  /* TODO: SFX trigger-table setup, out of scope — see $F7DB-$F82C */
}

/**
 * $C6C4: Per-frame title-screen animation driver
 *
 * Syncs to the next interrupt, draws the 6 foreground objects ($BB00-$BB2C)
 * via the masked blitter ($C8C5), steps every object's animation script by
 * one frame ($C705), clears the playfield bitmap ($CC04), then draws the 3
 * background objects ($BB36-$BB4A) via the alternate blitter ($C94F). In the
 * Z80 this block ends with an unconditional jump back to its own top
 * ($C702 JP $C6C4), so it never returns to its caller under normal
 * operation — the only exits are a stack-unwinding trick inside the
 * blitters when a frame overruns the interrupt deadline, or the whole call
 * stack being abandoned elsewhere when fire is pressed to start the game.
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: sprite/logo animation is out of scope for this task (per scope
 * decision). The self-looping structure and its stack-unwind escape hatch
 * are not modelled; this function always draws (stubbed) then returns after
 * exactly one frame, matching how other per-frame functions in this port
 * are called once per iteration from a caller-owned loop. Only the
 * EI/HALT frame-pacing point is translated, via the same stamp/sleep idiom
 * used elsewhere (e.g. drive_attract_demo). The foreground/background
 * blitter loops and the object-script interpreter are stubbed — see $C705,
 * $C8C5, $C94F.
 */
static void ts_animate_frame(chqstate_t *state)
{
  state->speccy->stamp(state->speccy);

  /* TODO: object/script animation, out of scope — see $C705, $C8C5, $C94F */

  state->speccy->sleep(state->speccy, STANDARD_SLEEP);
}

/**
 * $F82F: Per-frame SFX and music service
 *
 * Runs the AY title-tune driver (CALL $EC71 -> ts_music_service) once, then
 * clears the "frame occurred" flag at $F8A8. The remainder of the Z80
 * routine ($F836-$F8AC) drives three digitised-sample "busy slot" state
 * machines: SFX slot 1 ($F836-$F894, a selector-byte stream read via a
 * countdown at $F842), SFX slot 2 ($F894-$F8A1, a companion countdown armed
 * by slot 1's bit 7), and a 1-bit sample playback tail ($F8A1-$F8AC) that
 * pulses out sample rows from one of two fixed sample tables ($F8F2/$F95A)
 * or a procedural noise generator ($FA3A). Called once per frame from
 * $C06E, $C16A, $C59E, $F7C7 and $FBC8.
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: music-only translation, per scope decision. Only the CALL $EC71 is
 * translated; the frame-flag clear at $F832-$F833 is omitted because nothing
 * in this port ever polls $F8A8 (wait_for_frame_flag is unused outside the
 * SFX subsystem below). The three sample busy-slot state machines are out of
 * scope and left as a TODO — see $F837/$F895/$F8A2.
 */
static void sfx_music_service(chqstate_t *state)
{
  ts_music_service(state);

  /* TODO: digitised-sample SFX subsystem, out of scope — see
   * $F837 (slot 1), $F895 (slot 2), $F8A2 (1-bit sample playback active). */
}

/**
 * $C890: Clear the screen bitmap and attribute area
 *
 * Zero-fills the attribute area $5900-$5AFF and the bitmap $4800-$57FF --
 * the lower two-thirds of the screen, leaving $4000-$47FF (the top third)
 * untouched. Called by $C0EC and clear_and_fill_border_attrs ($C8A9).
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: the Z80 self-fills via `LD (HL),L` (both ranges start on a $x00
 * boundary, so L is already zero) then LDIR; this collapses to two plain
 * memset calls, per the skool's own Conv note at $C890.
 */
static void clear_screen_bitmap_and_attrs(chqstate_t *state)
{
  memset(ADDRTOATTRS(0x5900), 0, 0x200); /* $C890-$C89B */
  memset(ADDRTOSCREEN(0x4800), 0, 0x1000); /* $C89C-$C8A7 */
}

/**
 * $C8A9: Clear the screen then paint the border attribute rows
 *
 * Calls clear_screen_bitmap_and_attrs, then overwrites the attribute area
 * $5900-$5AFF with a fixed pattern, 32 bytes at a time (16 times, covering
 * all 512 bytes): 2 bytes of attribute 0 (black), 28 bytes of attribute
 * $45 (flash bit set; paper/ink in bits 0-5), then 2 more bytes of
 * attribute 0.
 *
 * \param[in,out] state Pointer to game state.
 */
static void clear_and_fill_border_attrs(chqstate_t *state)
{
  u8 *HLattr;   /* attribute write cursor (was HL) */
  int C_count;  /* outer repeat count, 16 (was C) */
  int B_middle; /* middle-band countdown, 28 (was B) */

  clear_screen_bitmap_and_attrs(state); /* $C8A9 CALL $C890 */

  HLattr = ADDRTOATTRS(0x5900);
  C_count = 0x10;
  do {
    *HLattr++ = 0; /* $C8B2/$C8B4 */
    *HLattr++ = 0;

    B_middle = 0x1C;
    do {
      *HLattr++ = 0x45; /* $C8B8 */
    } while (--B_middle);

    *HLattr++ = 0; /* $C8BD/$C8BF */
    *HLattr++ = 0;
  } while (--C_count);
}

/**
 * $C59E: Title-screen driver
 *
 * Picks one of 5 pre-scripted animation scenes, populates the 9-entry
 * animated-object array at $BB00 from the chosen scene's object table, draws
 * the overlay text (title/credits, "PRESS ENTER FOR OPTIONS" always, and
 * "PRESS GEAR TO PLAY" once controls have been selected), starts tune 0, then
 * falls into the attract-mode wait loop (ts_wait_loop) which animates the
 * scene each frame while polling for coin/fire/keyboard input to start a
 * game. Called from $C000 and $FBC8.
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: $C5A2-$C602 (scene-table pick and object-array population) is
 * stubbed as a TODO, per scope decision -- the five scene tables
 * ($CCB7/$CD4F/$CF10/$CFCD/$D16C) and the object-animation script
 * interpreter do not exist in the C port yet, and drawing/animation is out
 * of scope for this task (the same decision that stubbed ts_animate_frame).
 * The screen clear ($C8A9, clear_and_fill_border_attrs) and the overlay text
 * ($FDA4, print_character) are both ported and called below. The
 * self-modified scene selector operand at $C5A2 is part of the stubbed
 * mechanism; see the TODO in ts_wait_loop where the "any key" restart path
 * reseeds it.
 *
 * Conv: signature is `void`, not `u8`, even though $FBA2 (fire pressed) is a
 * real early-exit path in the Z80. It stays `void`: ts_wait_loop's fire-key
 * branch now calls options_menu_driver's omd_redraw_and_poll directly and
 * returns its result, which in the Z80 is itself a `JP $C59E` hand-off back
 * to this function -- so the fire path rejoins this loop exactly like the
 * "any key" and test-mode restarts, and no caller of title_screen_driver
 * ever needs to see it.
 *
 * Conv: despite the above, this function is *not* guaranteed to loop
 * forever even today -- ts_wait_loop has two genuine RET paths of its own
 * (the initial tune-4-and-180-frame-wait tail, and the coin-inserted tail),
 * both of which fall out of this function normally via a plain C `return`.
 * Only the ordinary polling path (no coin, no key) is unbounded.
 *
 * Conv: the Z80's `$C67E JP $C59E` / `$C693 JP $C59E` restarts are plain
 * jumps -- they do not grow the Z80 stack. Calling title_screen_driver
 * recursively from ts_wait_loop would grow the C stack by one frame per
 * restart with no bound (every "any key"/test-mode restart during a long
 * attract-mode session), so instead ts_wait_loop returns non-zero to
 * request a restart and this function loops.
 */
static void title_screen_driver(chqstate_t *state)
{
  for (;;) {
    clear_and_fill_border_attrs(state); /* $C59E CALL $C8A9 */

    /* $C5A2-$C5C7: pick one of 5 scene tables via the self-modified
     * rotating selector at $C5A2, and push the chosen table pointer.
     * $C5C7-$C5CE: draw the copyright/credits text block. $C5CE-$C602: zero
     * the $BB00-$BB4F object array and copy the 5-byte-per-object scene
     * table into it, reordering fields. Conv: out of scope -- see
     * prologue. */
    /* TODO: scene-table pick and object-array population ($C5A2-$C602) --
     * needs the scene tables and the animation script interpreter. */

    setup_im2_interrupt_table(state); /* $C602 CALL $F7AA */

    ts_animate_frame(state); /* $C605 CALL $C6C4 -- draw the first frame
                               * immediately, so the scene is visible before
                               * the wait loop starts polling. */

    print_character(state, &title_screen_overlay_text[21]); /* $C608-$C60B:
                                                               * "PRESS ENTER
                                                               * FOR OPTIONS"
                                                               * ($CC9D),
                                                               * unconditionally. */

    if (state->controls_selected) /* $C60E-$C611 */
      print_character(state, &title_screen_overlay_text[0]); /* $C612-$C615:
                                                                * "PRESS GEAR
                                                                * TO PLAY"
                                                                * ($CC88). */

    start_tune_and_sfx_table(state, 0); /* $C618 XOR A / $C619 CALL $F7D6 */

    state->speccy->stamp(state->speccy); /* $C61C EI / $C61D HALT: sync to
                                           * the next interrupt before
                                           * entering the wait loop, so the
                                           * first frame drawn above is
                                           * actually presented. */

    if (!ts_wait_loop(state)) /* $C61D falls through to $C61E */
      return;
  }
}

/**
 * $C61E: Title-screen attract-mode wait loop
 *
 * Animates the current scene once per interrupt (via sfx_music_service, the
 * per-frame sound/music tick) and polls for coin-insert / fire / any-key
 * input to start the game or jump to a fresh title screen. Re-entered every
 * frame via $C61E; title_screen_driver ($C59E) is re-run (new scene) when a
 * key other than fire is pressed.
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: the Z80 has no HALT anywhere in this loop body -- the per-frame
 * pacing described in the skool ("one $F82F service call per frame") is
 * informal; the real hardware relies on the background IM2 interrupt firing
 * asynchronously while this loop spins. The C port makes the frame boundary
 * explicit with stamp()/sleep() once per iteration, matching every other
 * per-frame loop in this file (attract_mode_128k, drive_attract_demo,
 * run_pregame_screen_loop).
 *
 * Conv: DI/EI have no C equivalent (SDL owns interrupt delivery, matching
 * setup_im2_interrupt_table) and are omitted throughout.
 *
 * Conv: the skool's inline comments name the wrong keyboard half-row at two
 * sites -- $C638 says "SPACE (fire)" but loads $BF (port_KEYBOARD_ENTERLKJH,
 * the ENTER/L/K/J/H row; bit 0 is ENTER), and $C681 says "ENTER" but loads
 * $F7 (port_KEYBOARD_12345, the 1/2/3/4/5 row). Both are translated here
 * against the actual operand and its Spectrum.h port constant, not the
 * skool's prose.
 *
 * \return 0 if this call ended via a genuine Z80 RET (the tune-4-wait tail
 * or the coin-inserted tail) -- the caller should stop, matching the real
 * control flow back to title_screen_driver's own caller. Non-zero if the
 * Z80 would have done `JP $C59E` to restart the title screen -- the caller
 * should re-run its own setup and call this again, rather than this
 * function recursing into title_screen_driver directly (see
 * title_screen_driver's prologue for why).
 */
static u8 ts_wait_loop(chqstate_t *state)
{
  int B_wait;       /* tune-4 wait countdown, 180 frames (was B) */
  u8  A_fire;       /* ENTER/L/K/J/H half-row, tested for fire (was A) */
  u8  A_coin_mode;  /* controls_selected read as a coin-op mode flag (was A) */
  u8  A_coin_input; /* coin-slot input, read via $800E (was A) */
  u8  A_test_mode;  /* test_mode flag (was A) */
  u8  A_key6;       /* 0/9/8/7/6 half-row, tested for the "6" key (was A) */
  u8  A_anykey;     /* 1/2/3/4/5 half-row, tested for any key (was A) */
  int carry;        /* required by the RRC macro, unused (carry) */

  for (;;) {
    if (state->host_quit)
      longjmp(state->host_quit_jmp, 1);

    state->speccy->stamp(state->speccy);

    sfx_music_service(state); /* $C61E CALL $F82F */

    /* $C621-$C625: LD A,($F223); AND A; JR NZ,$C638 */
    if (!state->title_music.tune_active) {
      /* $C627-$C637: wait out ~180 frames (one sfx_music_service call per
       * iteration) before falling through to the coin/name-table refresh
       * tail at ts_refresh_name_table.
       *
       * Conv: $C629 calls $F7DB (stst_load_sfx_script), NOT $F7D6
       * (start_tune_and_sfx_table's entry point) -- this is a distinct
       * entry point, used only from here, that skips the
       * PUSH AF/CALL $EB9E/POP AF tune-start prologue entirely and jumps
       * straight into the SFX-script-table setup for tune #4's cue table.
       * It must NOT call start_tune (that would incorrectly arm
       * state->title_music.tune_active). The SFX-script-table setup itself
       * is out of scope -- see sfx_music_service/start_tune_and_sfx_table's
       * own TODO for the digitised-sample SFX subsystem. */
      /* TODO: CALL stst_load_sfx_script ($F7DB) -- SFX-table setup for
       * tune #4, out of scope (digitised-sample SFX subsystem). */

      B_wait = 0xB4; /* $C62C LD B,$B4 */
      do {
        sfx_music_service(state); /* $C62F CALL $F82F */
        state->speccy->sleep(state->speccy, STANDARD_SLEEP);
        state->speccy->stamp(state->speccy);
      } while (--B_wait); /* $C633 DJNZ $C62E */
      /* $C635 INC B (B wraps 0 -> 1) has no further use of B afterwards --
       * Conv: DJNZ bookkeeping, omitted. */

      ts_refresh_name_table(state); /* $C636 JR $C69A */
      return 0; /* $C6C3 RET -- returns to title_screen_driver's own caller.
                 * Conv: contrary to the usual framing of this loop as
                 * unbounded, this path is a genuine early exit in the Z80 --
                 * see the prologue note on title_screen_driver. */
    }

    /* ts_check_fire ($C638): fire (ENTER) check.
     * Conv: was IN+CPL+RRA; RRA only tests bit 0, so this is collapsed to a
     * direct bit-0 mask (cf. attract_mode_128k's ENTER check). */
    A_fire = ~state->speccy->in(state->speccy, port_KEYBOARD_ENTERLKJH);
    if (A_fire & 1) /* $C63D RRA / $C63E JP C,$FBA2 */
      return omd_redraw_and_poll(state); /* hands off to the options menu;
        * its own $C59E hand-off matches this function's own "restart
        * title_screen_driver" return contract, so the value passes straight
        * through. */

    /* $C641-$C64C: coin-op mode / coin-slot check. Conv: $8001 is the same
     * address as state->controls_selected; the skool's prose calls it a
     * "coin-op mode" flag here, which may be genuine double duty (arcade
     * coin-op configuration doubling as "controls selected") or just loose
     * wording -- either way this reuses the existing field rather than
     * inventing a second one. */
    A_coin_mode = state->controls_selected; /* $C641 LD A,($8001) */
    if (A_coin_mode) { /* $C644-$C645 AND A; JR Z,$C64F */
      /* TODO: CALL $800E (coin-slot input read) -- common-RAM routine, not
       * disassembled in this bank and no C equivalent yet; treated as "no
       * coin" for now. */
      A_coin_input = 0;
      if (A_coin_input & 0x10) { /* $C64A AND $10; $C64C JP NZ,$C696 */
        ts_coin_inserted(state);
        return 0;
      }
    }

    /* $C64F-$C65C: test-mode "6" key check (skool prose calls these "1"/"2"
     * player-select keys, but the operand $EF is port_KEYBOARD_09876 and the
     * mask is $10 -- bit 4 of that row is the "6" key, not "1" or "2"). */
    /* $C652-$C653: AND A; JR Z,$C61E -- a loop-restart, not a skip-this-
     * block branch: $C61E is the loop head, so test_mode == 0 skips the
     * "any key" check below too, not just this one. */
    A_test_mode = state->test_mode; /* $C64F LD A,($8000) */
    if (!A_test_mode) {
      state->speccy->sleep(state->speccy, STANDARD_SLEEP); /* Conv: balance
        * this iteration's stamp() (see prologue) before restarting the loop */
      continue;
    }

    /* was IN+CPL */
    A_key6 = ~state->speccy->in(state->speccy, port_KEYBOARD_09876);
    if (A_key6 & 0x10) { /* $C65A AND $10; $C65C JP Z,$C681 */
      /* $C65F-$C67B: seed a placeholder score of $87654321, stage 6,
       * 3 retries, check it against the high-score table, then restart. */
      state->score_bcd[0] = 0x21; /* $C666 */
      state->score_bcd[1] = 0x43; /* $C669 */
      state->score_bcd[2] = 0x65; /* $C66C */
      state->score_bcd[3] = 0x87; /* $C66F */
      state->wanted_stage_number = 0x06; /* $C671/$C673 */
      state->retry_count = 0x03; /* $C676/$C678 */

      /* TODO: CALL stop_music_and_silence ($ED0B) -- clears tune_active
       * and the AY mixer/noise register cache; AY driver internals not
       * yet ported (see start_tune_and_sfx_table). */
      /* TODO: CALL $C00C (check high score) -- not disassembled in this
       * bank, no C equivalent yet. */

      return 1; /* $C67E JP $C59E -- ask the caller to restart */
    }

    /* $C681-$C693: "any key" (1/2/3/4/5 row) check -- restarts the title
     * screen with a freshly seeded scene selector. */
    /* was IN+CPL */
    A_anykey = ~state->speccy->in(state->speccy, port_KEYBOARD_12345);
    if ((A_anykey & 0x1F) == 0) { /* $C686 AND $1F; $C688 JP Z,$C61E */
      state->speccy->sleep(state->speccy, STANDARD_SLEEP); /* Conv: balance
        * this iteration's stamp() (see prologue) before restarting the loop */
      continue;
    }

    RRC(A_anykey); /* $C68B RRCA */
    /* TODO: seed scene-selector SM operand ($C5A2) with A_anykey -- the
     * whole scene-selection self-modifying byte is out of scope for this
     * task (see title_screen_driver's $C5A1-$C5C7 stub); when that is
     * implemented, this write must land in the same field. */

    /* TODO: CALL stop_music_and_silence ($ED0B) -- see note above. */

    return 1; /* $C693 JP $C59E -- ask the caller to restart */
  }
}

/**
 * $C696: Coin-inserted entry point
 *
 * Pushes $8011 as an extra "credit awarded" flag/value, then falls through
 * into the shared name-table refresh tail at ts_refresh_name_table.
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: the $8011 push is a stack marker discarded by the shared tail's
 * `POP AF` ($C6C2) -- it has no other effect and is not modelled.
 */
static void ts_coin_inserted(chqstate_t *state)
{
  ts_refresh_name_table(state); /* $C696-$C69A fallthrough */
}

/**
 * $C69A: Refresh the high-score name table
 *
 * Copies the 3 preset high-score name/rank rows from $C403 into the work
 * buffer pointed to by ($800A), each row split into 15+7+6 byte segments
 * with 2-byte gaps skipped between segments.
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: stubbed per scope decision -- nothing in the C port yet models the
 * destination buffer or a $800A-equivalent state field, and this task's
 * State.h changes are handled separately. Reached both directly from
 * ts_wait_loop's ~180-frame tune wait and via ts_coin_inserted; in the Z80
 * both paths end in a RET back to title_screen_driver's own caller, which
 * this function models simply by returning normally.
 */
static void ts_refresh_name_table(chqstate_t *state)
{
  /* TODO: copy 3 rows of high-score name/rank data from $C403 into the
   * buffer pointed to by ($800A) -- needs a destination buffer/state field,
   * out of scope for this task. */
}

/* $FFE5-$FFE9: "list A" -- Sinclair Interface II joystick key-scan codes,
 * installed into state->control_keys[0..4] when "1. SINCLAIR JOYSTICK" is
 * chosen. Genuine emulation of the classic Interface II wiring (keys 6-0),
 * not arbitrary key choices. */
static const u8 sinclair_joystick_keys[5] = { 0x23, 0x1B, 0x13, 0x03, 0x0B };

/* $FFEA-$FFEE: "list B" -- Cursor/Protek joystick key-scan codes, installed
 * when "2. CURSOR JOYSTICK" is chosen (keys 5,6,7,8,0). */
static const u8 cursor_joystick_keys[5]  = { 0x23, 0x0B, 0x03, 0x04, 0x13 };

/* $FF95-$FFE4: key-name lookup table for the "redefine keys" screen (40
 * 2-byte entries: printable character + space, with SYMBOL SHIFT/SPACE/
 * ENTER/CAPS SHIFT spelled out as two-letter codes). Indexed by
 * read_new_key_definition via the same key/halfrow packing produced by
 * scan_keyboard_matrix. Byte-for-byte identical to the 48K ROM's key_names[]
 * ($EDD6, CommonData.c) -- kept as a separate array since it is a distinct
 * copy at a distinct bank-3 address in the original. */
static const u8 control_key_names[80] = {
  'B', ' ', 'N', ' ', 'M', ' ', 'S', 'Y',
  'S', 'P', 'H', ' ', 'J', ' ', 'K', ' ',
  'L', ' ', 'E', 'N', 'Y', ' ', 'U', ' ',
  'I', ' ', 'O', ' ', 'P', ' ', '6', ' ',
  '7', ' ', '8', ' ', '9', ' ', '0', ' ',
  '5', ' ', '4', ' ', '3', ' ', '2', ' ',
  '1', ' ', 'T', ' ', 'R', ' ', 'E', ' ',
  'W', ' ', 'Q', ' ', 'G', ' ', 'F', ' ',
  'D', ' ', 'S', ' ', 'A', ' ', 'V', ' ',
  'C', ' ', 'X', ' ', 'Z', ' ', 'C', 'P',
};

/* $FFEF-$FFF6: "SHOCKED"+ENTER secret test-mode-unlock reference sequence,
 * checked by redefine_keys_screen against the 8 keys just chosen. Byte-for-
 * byte identical to the 48K ROM's shocked_keydefs[] ($EE30, CommonData.c). */
static const u8 shocked_keydef_sequence[8] = {
  0x1E, 0x01, 0x1A, 0x0F, 0x11, 0x15, 0x16, 0x21
};


/**
 * $FF0C: Scans the keyboard matrix for a single currently-held key
 *
 * Walks the eight keyboard half-row ports ($FEFE, $FDFE, $FBFE, $F7FE,
 * $EFFE, $DFFE, $BFFE, $7FFE), rotating the row-select byte through all
 * eight in turn. Whichever row (if any) has a key held has its bit
 * position within that row's 5-bit mask found by repeated halving, and
 * combined with the row number into a single packed code (see
 * read_new_key_definition for how the code is unpacked again).
 *
 * \param[in,out] state Pointer to game state.
 * \param[out] D_key_code_out Packed key code: 8*(4-bit) + (7-row). Left at
 * 0xFF if no key was held in any row (was D).
 * \return 1 if more than one row (or more than one bit within a row) was
 * held simultaneously -- an ambiguous scan the caller should reject and
 * retry. 0 otherwise (D_key_code_out is 0xFF for "no key", or a valid
 * packed code for exactly one key held).
 */
static u8 scan_keyboard_matrix(chqstate_t *state, u8 *D_key_code_out)
{
  int carry;          /* carry from SRL/RLC operations (carry) */
  u8  D_key_code;      /* sentinel 0xFF at entry; row-found flag/result (was D) */
  int E_row_value;     /* row's contribution to the packed code, decremented per row (was E) */
  u8  B_port_hi;       /* high byte of keyboard IN port; rotated through all eight rows (was B) */
  u8  A_pressed_mask;  /* active key bits for the current row: inverted, masked to 5 bits (was A) */
  u8  H_bits;          /* copy of A_pressed_mask, shifted right to find the set bit (was H) */
  u8  A_code;          /* row/bit code accumulator, decremented by 8 per shift (was A) */

  D_key_code  = 0xFF; /* $FF0C LD DE,$FF2F: D half */
  E_row_value = 0x2F; /* $FF0C LD DE,$FF2F: E half */
  B_port_hi   = 0xFE; /* $FF0F LD BC,$FEFE: B half */

  do {
    A_pressed_mask = (u8) (~state->speccy->in(state->speccy, (u16) ((B_port_hi << 8) | 0xFE)) & 0x1F); /* $FF12-$FF15 */

    if (A_pressed_mask != 0) { /* $FF17 JR Z,$FF25 */
      D_key_code++;
      if (D_key_code != 0)
        return 1; /* $FF1A RET NZ: a second row is also held -- ambiguous */

      H_bits = A_pressed_mask; /* $FF1B */
      A_code = (u8) E_row_value; /* $FF1C */
      do {
        A_code -= 8;
        SRL(H_bits);
      } while (!carry); /* $FF21 JR NC,$FF1D */

      if (H_bits != 0)
        return 1; /* $FF23 RET NZ: more than one bit held in this row */

      D_key_code = A_code; /* $FF24 */
    }

    E_row_value--; /* $FF25 DEC E */
    RLC(B_port_hi); /* $FF26 RLC B */
  } while (carry); /* $FF28 JR C,$FF12 */

  *D_key_code_out = D_key_code; /* $FF2A-$FF2B CP A / RET (Z always set here) */
  return 0;
}

/**
 * $FF8B: Advance the key-label print position by one label column
 *
 * Adds 32 to the low byte of the screen address, then adds 8 to the high
 * byte only if that addition overflowed -- i.e. only once every 8 columns,
 * when the low byte wraps back round. Same step as dak_move_down ($EDCC),
 * the 48K equivalent.
 *
 * \param[in] DE_screen Z80 screen address (was DE).
 * \return Screen address advanced by one label column.
 */
static u16 advance_key_label_column(u16 DE_screen)
{
  int carry; /* carry out of the E += 32 addition (carry) */
  int E_sum; /* E + 32 before truncation, to test for overflow (was A) */
  u8  E;     /* low byte of DE_screen: byte column offset + 32 (was E) */
  u8  D;     /* high byte of DE_screen: pixel row within third (was D) */

  E_sum = (DE_screen & 0xFF) + 32; /* $FF8B-$FF8C */
  carry = E_sum > 0xFF;
  E     = (u8) E_sum; /* $FF8E */

  D = (u8) (DE_screen >> 8);
  if (carry) /* $FF8F RET NC */
    D += 8; /* $FF90-$FF93 */

  return (u16) ((D << 8) | E);
}

/**
 * $FF2C: Waits for a fresh single keypress and stores it as one control's key
 *
 * Repeatedly scans the keyboard (scan_keyboard_matrix) until exactly one key
 * is held that is not already assigned to an earlier control in this
 * session (state->control_keys[0..C_control_index-2]), rejecting ambiguous
 * scans, "no key held" scans, and duplicates by looping back to rescan.
 * Stores the accepted key code at state->control_keys[C_control_index-1],
 * looks up its two-character display name in control_key_names[], prints it
 * at *DE_screen via print_character, then advances *DE_screen by one label
 * column (twice, when B_remaining is exactly 4 -- see
 * advance_key_label_column).
 *
 * \param[in,out] state Pointer to game state.
 * \param[in,out] DE_screen Screen address to print the key's name at;
 * updated to the next label position on return (was DE).
 * \param[in] B_remaining Controls remaining in the outer 8-control loop,
 * including this one; when exactly 4, an extra column advance is applied
 * (was B).
 * \param[in] C_control_index 1-based index of the control being defined,
 * into state->control_keys[] (was C).
 */
static void read_new_key_definition(chqstate_t *state, u16 *DE_screen,
                                     u8 B_remaining, u8 C_control_index)
{
  u8  ambiguous;     /* scan_keyboard_matrix ambiguity flag (was flags) */
  u8  D_key_code;    /* packed key code from scan_keyboard_matrix (was D) */
  u8  A_key_code;    /* accepted key code, used for storage/lookup (was A) */
  u8  B_dup_count;   /* duplicate-check count: C_control_index-1 already-
                      * assigned slots (was B) */
  u8  dup_i;         /* duplicate-check loop index (was HL-$FFF7) */
  int index_bytes;   /* byte offset into control_key_names[] (was HL-$FF95) */
  u8  char0;         /* first character of the looked-up key name (was A) */
  u8  char1;         /* second character, with the EOS bit set (was A) */

rescan:
  for (;;) {
    service_sound_and_loop_tune0(state); /* $FF2E CALL $FBC8 */

    ambiguous = scan_keyboard_matrix(state, &D_key_code); /* $FF31 CALL $FF0C */
    if (ambiguous) /* $FF34 JR NZ,$FF2E */
      continue;

    if (D_key_code == 0xFF) /* $FF36 INC D / $FF37 JR Z,$FF2E */
      continue;

    break;
  }
  A_key_code = D_key_code; /* $FF39-$FF3A DEC D / LD A,D */

  B_dup_count = (u8) (C_control_index - 1); /* $FF40-$FF41 LD B,C / DEC B */
  for (dup_i = 0; dup_i < B_dup_count; dup_i++) { /* $FF42 JR Z,$FF4A */
    if (A_key_code == state->control_keys[dup_i]) /* $FF44 CP (HL) */
      goto rescan; /* $FF45 JR Z,$FF2E: duplicate -- rescan */
  }

  state->control_keys[C_control_index - 1] = A_key_code; /* $FF4C-$FF52 */

  index_bytes = 10 * (A_key_code & 0x07) + 2 * (A_key_code >> 3); /* $FF53-$FF68 */
  char0 = control_key_names[index_bytes];           /* $FF71 */
  char1 = control_key_names[index_bytes + 1] | EOS; /* $FF76-$FF77 */

  state->options_key_string[0] = 0xC7; /* Conv: fixed constant resident at
                                         * $FD97; never rewritten by this
                                         * routine (see key-name-table
                                         * comment in the skool). */
  setwordat(&state->options_key_string[1], *DE_screen); /* $FF6D LD ($FD98),DE */
  state->options_key_string[3] = char0; /* $FF72 */
  state->options_key_string[4] = char1; /* $FF79 */
  print_character(state, &state->options_key_string[0]); /* $FF7C-$FF7F */

  *DE_screen = advance_key_label_column(*DE_screen); /* $FF82-$FF83 */
  if (B_remaining == 4) /* $FF87-$FF88 */
    *DE_screen = advance_key_label_column(*DE_screen); /* $FF8A: mid-list row wrap */
}

/**
 * $FEA9: "Redefine keys" screen driver
 *
 * Prints the title/prompt text and the 8 control-name labels (gear,
 * accelerate, brake, left, right, quit, pause, turbo), then captures a
 * fresh keypress for each of the 8 controls in turn via
 * read_new_key_definition, waiting out a keys-"1"-"5" debounce before each
 * capture. After all 8 keys are set, waits ~20 frames, then compares the 8
 * keys just chosen against shocked_keydef_sequence (the hidden "SHOCKED" +
 * ENTER cheat code): on a match, enables test mode, shows the confirmation
 * screen, waits for any key, then loops back to redisplay this screen; on
 * any mismatch, returns immediately (the ordinary case -- the new mapping
 * is kept).
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: modelled as an outer for(;;) that only exits via return (mismatch)
 * -- matching the Z80, which has no path back to the caller once the
 * secret code has been entered other than by looping back to $FEA9 itself.
 *
 * Conv: $FEC1/$FED6 (`PUSH HL` / `INC HL`) walk a pointer that is never
 * read back before the next iteration's `PUSH HL` overwrites it -- dead
 * code, as with the identical stray HL increment noted in redefine_keys_48k
 * ($ECF3). Not modelled.
 */
static void redefine_keys_screen(chqstate_t *state)
{
  u16 DE_screen;       /* current label print position (was DE) */
  u8  B_remaining;     /* controls remaining, counts down from 8 (was B) */
  u8  C_control_index; /* 1-based control index, counts up from 1 (was C) */
  u8  A_key_mask;      /* keys "1".."5" pressed bitmask (was A) */
  u8  B_wait;          /* ~20-frame post-capture wait counter (was B) */
  u8  B_shocked_i;     /* "SHOCKED"+ENTER compare loop index (was B) */

  for (;;) {
    clear_options_screen(state); /* $FEA9 CALL $FE7F */

    print_string(state, &options_menu_text[114]); /* $FEAC-$FEAF: header +
                                                    * GEAR/ACCELERATE/BRAKE */
    service_sound_and_loop_tune0(state); /* $FEB2 CALL $FBC8 */
    print_string(state, &options_menu_text[160]); /* $FEB5-$FEB8:
                                                    * LEFT/RIGHT/QUIT/PAUSE/TURBO */

    DE_screen       = 0x48D6; /* $FEBB LD DE,$48D6 */
    B_remaining     = 8;      /* $FEBE LD BC,$0801: B half */
    C_control_index = 1;      /* $FEBE LD BC,$0801: C half */

    do {
      do {
        service_sound_and_loop_tune0(state); /* $FEC4 CALL $FBC8 */

        A_key_mask = (u8) (~state->speccy->in(state->speccy, port_KEYBOARD_12345) & 0x1F); /* $FECA-$FECE */
      } while (A_key_mask != 0); /* $FED0 JR NZ,$FEC1: wait for keys "1"-"5" to be released */

      read_new_key_definition(state, &DE_screen, B_remaining, C_control_index); /* $FED2 CALL $FF2C */

      C_control_index++; /* $FED5 INC C */
    } while (--B_remaining != 0); /* $FED7 DJNZ $FEC1 */

    B_wait = 0x14; /* $FED9 LD B,$14 */
    do {
      service_sound_and_loop_tune0(state); /* $FEDC CALL $FBC8 */
    } while (--B_wait != 0); /* $FEE0 DJNZ $FEDB */

    for (B_shocked_i = 0; B_shocked_i < 8; B_shocked_i++) { /* $FEE2-$FEEF */
      if (state->control_keys[B_shocked_i] != shocked_keydef_sequence[B_shocked_i]) /* $FEEA-$FEEC */
        return; /* $FEEE RET NZ: mismatch -- ordinary case, keep the new mapping */
    }

    state->test_mode = 1; /* $FEF1-$FEF3 */

    clear_options_screen(state); /* $FEF6 CALL $FE7F */
    print_string(state, &options_menu_text[199]); /* $FEF9-$FEFC: test-mode confirmation text */

    do {
      service_sound_and_loop_tune0(state); /* $FEFF CALL $FBC8 */

      A_key_mask = (u8) (~state->speccy->in(state->speccy, port_KEYBOARD_12345) & 0x1F); /* $FF02-$FF06 */
    } while (A_key_mask == 0); /* $FF08 JR Z,$FEFF: wait for any key */
  }
}

/**
 * $FBC8: Services sound each frame and keeps the options-menu tune looping
 *
 * Runs one frame of the SFX/music service and, if no tune is currently
 * active, restarts tune 0.
 *
 * \param[in,out] state Pointer to game state.
 */
static void service_sound_and_loop_tune0(chqstate_t *state)
{
  sfx_music_service(state); /* $FBC8 CALL $F82F */

  if (!state->title_music.tune_active) /* $FBCB-$FBCF LD A,($F223); AND A; RET NZ */
    start_tune_and_sfx_table(state, 0); /* $FBD0 XOR A / $FBD1 JP $F7D6 */
}

/**
 * $FC14: Joystick-present detector for the control-select sub-screen
 *
 * Samples the Kempston port 20 times, servicing sound each iteration, and
 * bails out as soon as the port's value changes (a joystick is moving or
 * present).
 *
 * \param[in,out] state Pointer to game state.
 *
 * \return 0 if Kempston port activity was detected within the sample
 * window -- the caller must return to the poll loop without installing any
 * control scheme. 1 if no joystick was detected -- the caller falls into
 * the shared control-install tail with the input-method flag set to 1.
 */
static u8 detect_kempston_joystick(chqstate_t *state)
{
  int B_count;     /* sample loop countdown, 20 iterations (was B) */
  u8  C_baseline;  /* first Kempston sample (was C) */
  u8  A_sample;    /* current Kempston sample (was A) */

  A_sample   = state->speccy->in(state->speccy, port_KEMPSTON_JOYSTICK); /* $FC16 */
  C_baseline = A_sample; /* $FC18 LD C,A */

  B_count = 0x14; /* $FC14 LD B,$14 */
  do {
    A_sample = state->speccy->in(state->speccy, port_KEMPSTON_JOYSTICK); /* $FC19 */
    if (A_sample != C_baseline) /* $FC1B CP C / $FC1C JR NZ,$FBAB */
      return 0;

    service_sound_and_loop_tune0(state); /* $FC1E PUSH BC / $FC1F CALL $FBC8 / $FC22 POP BC */
    state->speccy->sleep(state->speccy, STANDARD_SLEEP);
    state->speccy->stamp(state->speccy);
  } while (--B_count); /* $FC23 DJNZ $FC19 */

  return 1; /* $FC25 LD A,$01 / $FC27 JR $FBE5 */
}

/**
 * $FDA4: Print one packed text record (position + colour + characters)
 *
 * Unpacks a 3-byte record header -- style/colour byte, then a 2-byte screen
 * pixel address -- then draws each following character until one with bit 7
 * set (the record terminator) is printed. Despite the "print a single
 * character" name inherited from the skool, this draws a whole run of
 * characters sharing one position/colour, since its own internal loop
 * (mdc-style) only returns after the terminator; #print_string only calls
 * this again if further records follow in memory.
 *
 * Each character byte in the stream (with bit 7 masked off) is either a
 * literal space ($20, advances the column without drawing) or a metric byte
 * mapped through a range ladder to one of 41 glyphs in #font, blitted
 * double-height (7 font bytes -> 15 scanlines across two attribute rows,
 * BRIGHT set on the upper row) or single-height (7 font bytes, one scanline
 * each), selected by the header's style bit.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in] HL_record Pointer to the 3-byte header + character stream (was HL).
 * \return Pointer to the byte following the record's terminator (was HL).
 *
 * Conv: $FDA4-$FDB9 (header unpack), the classification ladder ($FDDA-$FDFE)
 * and the double/single-height blits ($FE16-$FE7E) all follow menu_draw_char
 * ($EC2C) precedent -- an essentially identical blit for an essentially
 * identical font -- but this function has no cross-call persisted state to
 * carry via EXX, so the shadow-register dance the Z80 uses to snapshot the
 * per-character screen pointer ($FE0D-$FE11: EXX/PUSH DE/INC E/EXX/POP DE,
 * "pop scr addr as-was") collapses to a plain local: compute DEscreen from
 * the *current* E_screen, then increment E_screen for the next character.
 * Similarly, $FDBA EXX/$FDBB EX (SP),HL (banking the metric/shape stream
 * pointer while the attribute address sits in shadow HL') has no observable
 * effect in C beyond naming which quantity is "the shape cursor" from this
 * point on; modelled as a plain assignment, not a literal register swap.
 */
static const u8 *print_character(chqstate_t *state, const u8 *HL_record)
{
  u8         C_byte0;      /* packed style-bit + colour byte (was C) */
  u8         C_colour;     /* colour value, bits 0-6 of byte0 (was C) */
  u8         A_style_bit;  /* byte0 bit 7: 0 = double-height shaded glyph, 1 = single-height flat glyph (was carry via EX AF,AF') */
  u8         E_screen;     /* pixel screen address low byte; advances one per column (was E) */
  u8         D_screen;     /* pixel screen address high byte; constant across the whole call (was D) */
  u8         H_attr;       /* attribute address high byte: $58 + third (was H) */
  u8         L_attr;       /* attribute address low byte; advances one per column (was L) */
  const u8  *HLshape;      /* metric/shape-byte stream cursor (was HL) */
  u8         A_metric;     /* current column's metric byte, bits 0-6 (was A) */
  u8         A_terminator; /* bit 7 of the metric byte: terminates the outer loop (was flags) */
  u8         A_diff;       /* metric - $20; classification input (was A) */
  u8         C_class;      /* width-class index (was C) */
  const u8  *HLfont;       /* pointer to this glyph's 7-byte font[] entry (was HL) */
  u8        *DEscreen;     /* pixel destination for this glyph (was DE) */
  int        row;          /* row loop counter; no Z80 equivalent (Conv: rolled) */

  C_byte0     = *HL_record;           /* $FDA4-$FDA9 */
  C_colour    = C_byte0 & 0x7F;
  A_style_bit = (C_byte0 >> 7) & 1;

  E_screen = HL_record[1]; /* $FDAA-$FDAE */
  D_screen = HL_record[2];
  HL_record += 3;

  H_attr = (u8) (0x58 + ((D_screen >> 3) & 0x03)); /* $FDB0-$FDB9 */
  L_attr = E_screen;

  HLshape = HL_record; /* // EXX / EX (SP),HL - bank ($FDBA-$FDBB) */

  do {
    A_metric = *HLshape & 0x7F; /* $FDBE-$FDBF */

    if (A_metric == 0x20) {
      /* $FDD1-$FDD9: space */
      E_screen++;
      L_attr++;
    } else {
      A_diff = (u8) (A_metric - 0x20); /* $FDD1 */

      /* $FDDA-$FDFE classification ladder */
      if (A_diff >= 0x21) {
        C_class = (u8) (A_diff - 18);
      } else if (A_diff >= 0x10) {
        C_class = (u8) (A_diff - 11);
      } else if (A_diff == 1) {
        C_class = 0;
      } else if (A_diff == 8) {
        C_class = 1;
      } else if (A_diff == 9) {
        C_class = 2;
      } else if (A_diff == 12) {
        C_class = 3;
      } else {
        C_class = 4;
      }

      HLfont = &font[C_class * 7]; /* $FDFE-$FE0C */

      /* $FE0D-$FE11: shared destination snapshot for both branches below. */
      DEscreen = ADDRTOSCREEN((D_screen << 8) | E_screen);
      E_screen++;

      if (!A_style_bit) { /* $FE12-$FE15 */
        /* $FE16-$FE4E: double-height, 7 font bytes -> 15 rows */
        for (row = 0; row < 4; row++) { /* Conv: rolled */
          *DEscreen = *HLfont;
          DEscreen += 256;
          *DEscreen = *HLfont++;
          DEscreen += 256;
        }
        /* $FE30-$FE37: crosses 8-scanline group boundary. Conv: the literal
         * Z80 does E += 0x1F, D -= 7 on the *register* DE (D0+7, E0+1 at
         * this point), landing on (D0, E0+32). The rolled loop above instead
         * advances DEscreen by 256 twice per font byte, so it is already 255
         * bytes further along (D0+8, E0) than the literal register state.
         * 0xF820, not 0xF81F, is the constant that lands this pointer on the
         * same (D0, E0+32) target. */
        DEscreen += 0xF820;
        for (row = 0; row < 3; row++) { /* Conv: rolled */
          *DEscreen = *HLfont;
          DEscreen += 256;
          *DEscreen = *HLfont++;
          DEscreen += 256;
        }
        *DEscreen = 0; /* $FE4D-$FE4E: final row always blank */

        *ADDRTOATTRS((H_attr << 8) | L_attr) = C_colour | ATTR_BRIGHT; /* $FE50-$FE53 */
        *ADDRTOATTRS((H_attr << 8) | (u8) (L_attr + 0x20)) = C_colour & ~ATTR_BRIGHT; /* $FE54-$FE5A */
        L_attr++; /* $FE5B-$FE5C */
      } else {
        /* $FE5F-$FE78: single-height, 7 font bytes, one row each */
        for (row = 0; row < 7; row++) { /* Conv: rolled */
          *DEscreen = *HLfont++;
          DEscreen += 256;
        }

        *ADDRTOATTRS((H_attr << 8) | L_attr) = C_colour; /* $FE7B */
        L_attr++; /* $FE7C */
      }
    }

    A_terminator = *HLshape & 0x80; /* $FDC6 */
    HLshape++; /* $FDC8 */
  } while (!A_terminator); /* $FDC9 */

  return HLshape; /* $FDCB-$FDD0 */
}

/**
 * $FD9C: Print one or more back-to-back packed text records
 *
 * Calls print_character to draw the record at HLstring, then repeats for the
 * next record if the byte immediately following the terminator is non-zero.
 * The final zero byte is a pad, not part of any record (e.g. the $FC29
 * block's "$FD96 pad byte").
 *
 * \param[in,out] state Pointer to game state.
 * \param[in] HLstring Pointer to the first record (was HL).
 */
static void print_string(chqstate_t *state, const u8 *HLstring)
{
  for (;;) {
    HLstring = print_character(state, HLstring); /* $FD9C CALL $FDA4 */
    if (*HLstring == 0) /* $FD9F-$FDA1 */
      return;
  }
}

/**
 * $FE7F: Clears the options-menu screen area (attributes and bitmap)
 *
 * Zero-fills the same $5900-$5AFF attribute range and $4800-$57FF bitmap
 * range as clear_screen_bitmap_and_attrs, servicing sound (sfx_music_service,
 * via service_sound_and_loop_tune0) between passes so the title tune keeps
 * advancing during the fill. Called from omd_redraw_and_poll ($FBA2 and
 * $FBE5) and, once ported, the "define keys" screen ($FEA9/$FEF6).
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: the Z80 does this as three LDIR chunks (attrs, then bitmap split
 * into two chunks of $082F and $07D0 bytes) with a sound-service call
 * between each pair; the bitmap fill collapses to one memset since nothing
 * observes it mid-way, but all three service calls are kept, in the same
 * order, so the tune advances by the same number of steps as the Z80.
 * Falls through into service_sound_and_loop_tune0 via a tail jump in the
 * Z80 ($FEA6 JP $FBC8), modelled here as a plain call before returning.
 */
static void clear_options_screen(chqstate_t *state)
{
  memset(ADDRTOATTRS(0x5900), 0, 0x200); /* $FE7F-$FE8A */
  service_sound_and_loop_tune0(state); /* $FE8B CALL $FBC8 */

  memset(ADDRTOSCREEN(0x4800), 0, 0x1000); /* $FE8E-$FEA5 */
  service_sound_and_loop_tune0(state); /* $FE9C CALL $FBC8 */

  service_sound_and_loop_tune0(state); /* $FEA6 JP $FBC8 (tail call) */
}

/**
 * $FBA2 (omd_redraw_and_poll): options-menu redraw + poll + dispatch loop
 *
 * Draws the control-select screen text, then polls half-row $F7FE (keys
 * "1"-"5") and dispatches: "1" -> Sinclair joystick key list, "2" -> Cursor
 * joystick key list (both installed via a shared 5-byte copy), "3" ->
 * Kempston-joystick detection, "4" -> keyboard (no key-list copy, keeps
 * whatever is already in state->control_keys), "5" (falls through
 * unbranched, the default) -> "DEFINE KEYS" screen, after which the whole
 * loop redraws and re-polls.
 *
 * Once a scheme is chosen (any path other than "5"), installs the
 * active-control-config header and hands off to the title screen.
 *
 * \param[in,out] state Pointer to game state.
 *
 * \return 1 always, at the point corresponding to the Z80's `JP $C59E`
 * ($FC11) -- the caller should now (re-)run title_screen_driver.
 *
 * Conv: this function uses goto/labels rather than nested structured loops.
 * $FBA2 (full redraw) and $FBAB (poll only, no redraw) are two genuinely
 * distinct restart points reached from different call sites -- the "no
 * key" and "key 5" exits target $FBA2; the Kempston-detector's bail-out
 * targets $FBAB. Modelling both with a single loop would either duplicate
 * the redraw block or redraw when the Z80 does not.
 *
 * Conv: $FBAE-$FBB3 (`LD A,$F7` / `IN A,($FE)` / `CPL` / `AND $1F`)
 * collapses to a single inverted, masked port_KEYBOARD_12345 read (same
 * collapse as ts_wait_loop's fire/coin/anykey checks).
 *
 * Conv: $FBB7-$FBC1 (four `RRA` / `JR C` pairs testing bits 0-3 of the
 * 5-bit mask in turn) collapse to direct bit tests against A_key_mask; key
 * "5" is whatever remains after all four bits test false, matching the
 * Z80's unbranched fallthrough default.
 */
static u8 omd_redraw_and_poll(chqstate_t *state)
{
  u8         A_key_mask;      /* keys "1".."5" pressed bitmask, bit0=key"1"..
                                * bit3=key"4" (was A) */
  const u8  *HL_ctrl_list;    /* joystick key-list source, list A or B (was HL) */
  u8         A_flag;          /* input-method flag written to the active-
                                * config header byte: 0 = joystick/keyboard
                                * scheme installed normally, 1 = no Kempston
                                * joystick detected (was A) */

redraw: /* $FBA2 */
  clear_options_screen(state); /* $FBA2 CALL $FE7F */

  print_string(state, &options_menu_text[0]); /* $FBA5-$FBA8: "ENTER OPTION" /
                                                * P1-P5 control-scheme list. */

poll: /* $FBAB omd_service_and_read_keys */
  do {
    state->speccy->stamp(state->speccy);
    service_sound_and_loop_tune0(state); /* $FBAB CALL $FBC8 */

    A_key_mask = (u8) (~state->speccy->in(state->speccy, port_KEYBOARD_12345) & 0x1F); /* $FBAE-$FBB3 */

    if (A_key_mask == 0) /* $FBB5 JR Z,$FBAB */
      state->speccy->sleep(state->speccy, STANDARD_SLEEP);
  } while (A_key_mask == 0);

  if (A_key_mask & 0x01) { /* $FBB7/$FBB8: key "1" -> Sinclair joystick */
    HL_ctrl_list = sinclair_joystick_keys; /* $FBD4 LD HL,$FFE5 */
    goto install_joystick_keys;
  }
  if (A_key_mask & 0x02) { /* $FBBA/$FBBB: key "2" -> Cursor joystick */
    HL_ctrl_list = cursor_joystick_keys; /* $FBD9 LD HL,$FFEA */
    goto install_joystick_keys;
  }
  if (A_key_mask & 0x04) { /* $FBBD/$FBBE: key "3" -> Kempston detect */
    if (!detect_kempston_joystick(state)) /* $FC14 */
      goto poll; /* $FC1C JR NZ,$FBAB: joystick activity seen, poll again */
    A_flag = 1; /* $FC25 LD A,$01 */
    goto shared_tail;
  }
  if (A_key_mask & 0x08) { /* $FBC0/$FBC1: key "4" -> keyboard, inline */
    A_flag = 0; /* $FBE4 XOR A */
    goto shared_tail;
  }

  /* $FBC3: key "5" (default, falls through unbranched) -> "DEFINE KEYS" */
  redefine_keys_screen(state); /* $FBC3 CALL $FEA9 */
  goto redraw; /* $FBC6 JR $FBA2 */

install_joystick_keys:
  memcpy(state->control_keys, HL_ctrl_list, 5); /* $FBDC LD DE,$FFF7 /
                                                  * $FBDF LD BC,$0005 /
                                                  * $FBE2 LDIR */
  A_flag = 0; /* $FBE4 XOR A */

shared_tail: /* $FBE5 */
  /* TODO: install the active-control-config header at ($8008): write
   * A_flag, then copy control_keys[5..7] (quit/pause/turbo) followed by
   * control_keys[0..4] (gear/accelerate/brake/left/right) into a 9-byte
   * destination -- $FBE5-$FBF8. Needs a real design once the gameplay
   * input reader that consumes this is ported; not modelled yet. */

  clear_options_screen(state); /* $FBFA CALL $FE7F */

  do {
    state->speccy->stamp(state->speccy);
    service_sound_and_loop_tune0(state); /* $FBFD CALL $FBC8 */

    A_key_mask = (u8) (~state->speccy->in(state->speccy, port_KEYBOARD_12345) & 0x1F); /* $FC00-$FC04 */

    if (A_key_mask != 0) /* $FC06 JR NZ,$FBFD: debounce -- wait for the
                          * selection key to be released before proceeding */
      state->speccy->sleep(state->speccy, STANDARD_SLEEP);
  } while (A_key_mask != 0);

  /* TODO: CALL stop_music_and_silence ($ED0B) -- AY driver internals not
   * yet wired up (see start_tune_and_sfx_table). */

  state->controls_selected = 1; /* $FC0B LD A,$01 / $FC0D LD ($8001),A */

  /* $FC10 DI: omitted -- SDL owns interrupt delivery, matching every other
   * DI/EI site in this file (see ts_wait_loop's prologue). */

  return 1; /* $FC11 JP $C59E: hand off to title_screen_driver */
}

/**
 * $FB99: Options-menu driver entry point
 *
 * One-time setup for the control-select menu: arms the IM2 interrupt
 * vector table, starts tune 0, and syncs to the next interrupt, then falls
 * into the redraw+poll loop at omd_redraw_and_poll ($FBA2).
 *
 * Called once from the cold-boot entry point ($C009, BANK3_INPUT_SELECTION,
 * not yet wired up here). The fire-key exit from the title screen's
 * attract-mode wait loop (ts_wait_loop, $C63E JP C,$FBA2) re-enters at
 * omd_redraw_and_poll directly, skipping this one-time setup -- ts_wait_loop's
 * existing TODO ("fire pressed -> start the game via $FBA2") should call
 * omd_redraw_and_poll(state), not this function.
 *
 * \param[in,out] state Pointer to game state.
 *
 * \return 1 always -- see omd_redraw_and_poll's return-value doc.
 */
static u8 options_menu_driver(chqstate_t *state)
{
  setup_im2_interrupt_table(state);   /* $FB99 CALL $F7AA */
  start_tune_and_sfx_table(state, 0); /* $FB9C XOR A / $FB9D CALL $F7D6 */

  state->speccy->stamp(state->speccy); /* $FBA0 EI / $FBA1 HALT: sync to the
                                         * next interrupt before entering the
                                         * poll loop (same EI/HALT -> stamp()
                                         * convention as title_screen_driver). */

  return omd_redraw_and_poll(state); /* $FBA2: falls straight in */
}

/**
 * $F7C7: Boot entry point — set up interrupts, start tune 1, and run the
 * success-jingle sound loop
 *
 * Calls setup_im2_interrupt_table, starts tune 1, then falls into
 * basl_service_loop ($F7D1), which calls sfx_music_service once per 50Hz
 * interrupt via HALT synchronisation. In the Z80 this loop is unconditional
 * (`CALL $F82F` / `JR $F7D1`) and never returns to its caller.
 *
 * \param[in,out] state Pointer to game state.
 *
 * \return Nothing (was RET never reached).
 *
 * Conv: reached from BANK3_SUCCESS_MUSIC (the perp-caught success jingle) via
 * call_bank_3_128k, whose caller (handle_perp_caught_128k, and in turn its
 * own caller's phase4 state machine) expects a normal return so scoring and
 * fading can proceed on the same call — an infinite loop here would
 * permanently hang the game thread. Per explicit scope decision, the Z80's
 * unconditional loop is quantised into a bounded run of BASL_JINGLE_FRAMES
 * frames (reusing the same 0xB4/180-frame, ~3.6s heuristic already used for
 * the tune-4 wait in ts_wait_loop) and then returns normally. The frame
 * count is a guess at the jingle's real duration; TODO: tune by ear once
 * pattern data exists to actually hear it.
 */
static void boot_and_run_sound_loop(chqstate_t *state)
{
  int B_wait; /* jingle frame countdown (was B, unbounded in the Z80) */

  setup_im2_interrupt_table(state);
  start_tune(state, 1);

  B_wait = BASL_JINGLE_FRAMES;
  do {
    state->speccy->stamp(state->speccy);
    sfx_music_service(state); /* $F7D1 CALL $F82F */
    state->speccy->sleep(state->speccy, STANDARD_SLEEP);
  } while (--B_wait);
}

/**
 * $F3B6: Page in bank 3 and call a banked routine
 *
 * Patches a CALL instruction at $81C5 with [HLroutine], backs up the 4 KB at
 * $B000 to $F000, sets up a temporary stack, pages in bank 3 via page_128k,
 * executes the patched CALL, then pages bank 3 back out, restores SP and
 * refills $B000 from $F000.
 *
 * In C, bank 3 routines are not yet implemented. A switch on [HLroutine]
 * dispatches each Z80 entry-point address constant to its C stub.
 * BANK3_INPUT_SELECTION sets controls_selected and returns 0 to prompt the
 * caller's loop to exit; all other cases return 1.
 *
 * \param[in] state Pointer to game state.
 * \param[in] HLroutine Z80 address of the bank 3 routine to invoke. (was HL)
 *
 * \return 1 on success; 0 to signal an early return in the caller's loop
 * (BANK3_INPUT_SELECTION only).
 *
 * Conv: Z80 uses self-modification and 128K hardware memory paging; C
 * dispatches via switch on the [HLroutine] address constants.
 */
u8 call_bank_3_128k(chqstate_t *state, int HLroutine)
{
  switch (HLroutine) {
  default:
    assert(0);
    break;
  case BANK3_TITLE_SCREEN:
    title_screen_driver(state);
    break;
  case BANK3_HI_SCORE:
    break;
  case BANK3_SUCCESS_MUSIC:
    boot_and_run_sound_loop(state);
    break;
  case BANK3_INPUT_SELECTION:
    state->controls_selected = 1; // temp
    return 0; // cause an exit
  }
  return 1;
}
