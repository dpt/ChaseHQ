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
#include "ChaseHQ/Data/TitleScreenData.h"

/* ----------------------------------------------------------------------- */

#define BASL_JINGLE_FRAMES (0xB4) /* success-jingle duration; see boot_and_run_sound_loop Conv: */

/* state is always the enclosing function's chqstate_t* parameter. */
#define ADDRTOSCREEN(addr) z80addrtoscreen(state, addr, 0, 0)
#define ADDRTOATTRS(addr)  z80addrtoattrs(state, addr, 0, 0)

/* ----------------------------------------------------------------------- */

/**
 * Destination address and glyph-table lookup result shared by
 * compute_glyph_blit_params and compute_glyph_blit_params_b ($C8C5, $C94F).
 */
typedef struct glyph_blit_geometry
{
  int        H;               /* destination screen address high byte (was D) */
  int        L;               /* destination screen address low byte (was E) */
  const u8  *HLsrc;            /* glyph bitmap pointer (was HL) */
  int        B_height_pairs;   /* scanline-pairs remaining to draw (was B) */
  int        C_width_select;   /* width selector, 1-7 (was C) */
  int        carry_initial;    /* true: Y was within range, nothing to skip (was Carry) */
  u8         A_excess;         /* Y clamp excess; valid only when !carry_initial (was A') */
} glyph_blit_geometry_t;

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
static void clear_playfield_buffer(chqstate_t *state);
static void object_script_step(chqstate_t *state);
static u8   oss_lookup_speed(u8 C_idx);
static void oss_apply_x_step(struct title_object *rec);
static void oss_apply_y_step(struct title_object *rec);
static void oss_op_velocity(struct title_object *rec);
static void oss_op_decel_x(struct title_object *rec);
static void oss_op_decel_y(struct title_object *rec);
static void oss_op_accel_x_a(struct title_object *rec);
static void oss_op_accel_x_b(struct title_object *rec);
static void oss_op_accel_x_c(struct title_object *rec);
static void compute_glyph_geometry(u8 B_y, u8 C_x, u8 L_row,
                                    glyph_blit_geometry_t *out);
static void advance_glyph_scanline(int *H, int *L);
static void blit_glyph_rows(chqstate_t *state, int H, int L, const u8 *src,
                            int B_height_pairs, int row_bytes);
static void blit_width1(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs);
static void blit_width2(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs);
static void blit_width3(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs);
static void blit_width4(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs);
static void blit_width5(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs);
static void blit_width6(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs);
static void blit_width7(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs);
static void blit_masked_sprite_dispatch(chqstate_t *state, int H, int L,
                                        const u8 *src, int B_height_pairs,
                                        int C_width_select);
static void blit_masked_sprite_dispatch_b(chqstate_t *state, int H, int L,
                                          const u8 *src, int B_height_pairs,
                                          int C_width_select);
static void compute_glyph_blit_params(chqstate_t *state, u8 B_y, u8 C_x,
                                       u8 L_row);
static void compute_glyph_blit_params_b(chqstate_t *state, u8 B_y, u8 C_x,
                                         u8 L_row);
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
  u8         C_status;          /* channel status/flags byte, unchanged through most of the function (was C, IX+$00) */
  u8         A_env_step;        /* envelope-step counter, then reload value (was A, IX+$19) */
  const u8  *HL_env_shape;      /* envelope shape table pointer (was HL, IX+$16/$17) */
  u8         A_env_byte;        /* envelope shape byte just read (was A) */
  u8         B_note;            /* base note index for the tone lookup (was B, IX+$12) */
  const u8  *HL_offset_ptr;     /* pitch-offset sequence pointer (was HL, IX+$0B/$0C) */
  u8         A_offset_byte;     /* pitch-offset byte read from the sequence (was A) */
  u8         A_note_combined;   /* offset byte + note index, before doubling (was A) */
  u8         A_note_lookup;     /* doubled combined index: byte offset into the tone-period table, reused in phase 3 as the vibrato scale seed (was A/H) */
  u16        DE_period;         /* running tone period (was DE) */
  u8         B_vib_range;       /* doubled vibrato depth; clamp bound for the phase counter (was B, IX+$1A after SLA) */
  u8         A_vib_phase;       /* vibrato triangle-wave phase counter (was A, IX+$1C) */
  s16        DEvib_offset;      /* signed, scaled vibrato pitch offset (was DE) */
  u16        A_shift_test;      /* shift-loop overflow accumulator (was A, tested via carry) */
  u8         B_slide_countdown; /* portamento reload countdown (was B, IX+$0E) */
  s8         C_slide_step;      /* signed per-tick portamento step (was C, IX+$0D) */
  u16        HL_slide_accum;    /* accumulated portamento/slide value (was HL, IX+$07/$08) */
  u8         A_status_new;      /* status with bit 0 toggled; stored back and re-tested in phase 5 (was A) */
  u8         A_mixer_test;      /* ~status & 3; nonzero except every 4th call (was A) */
  u8         A_shared;          /* shared driver byte, XORed and forwarded (was A, $EC79) */
  u8         A_mixer_val;       /* value merged into the shared mixer cache (was A) */
  u8         A_mute_flag;       /* one-shot mute-transition gate (was A, IX+$1F) */
  u8         A_noise_mask;      /* this channel's noise-enable bits, complemented for clearing (was A) */

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
    IX_channel->pitch_offset_default   = default_pitch_offset_seq; /* +$09/$0A */
    IX_channel->pitch_offset_cur       = default_pitch_offset_seq; /* +$0B/$0C */
    IX_channel->envelope_shape_default = default_envelope_shape;   /* +$14/$15 */
    IX_channel->envelope_shape_ptr     = default_envelope_shape;   /* +$16/$17 */

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
 * $C804: Look up a deceleration/acceleration curve magnitude
 *
 * Shared by all five countdown-driven movement modes (oss_op_decel_x/y and
 * oss_op_accel_x_a/b/c): fetch the curve byte at the given index and halve
 * it twice.
 *
 * \param[in] C_idx Countdown/curve index (was C).
 *
 * \return Curve magnitude for this index (was A).
 *
 * Conv: the Z80 shuttles the outer object-loop's B (DJNZ counter) through A
 * around this lookup (LD A,B / LD B,$00 / ... / LD B,A) purely to protect it
 * from being clobbered by the table-relative ADD HL,BC. With no shared
 * register file in C the outer loop counter cannot be affected by this call,
 * so the shuttle has no equivalent and is omitted.
 *
 * Conv: the skool's own commentary on this table disagrees with itself --
 * one paragraph calls it a 256-byte table, another documents it as a
 * 36-entry table "indexed by a 0-35 countdown value" (matching the 36 bytes
 * actually transcribed into title_speed_curve). A scripted object whose
 * decel/accel phase runs long enough (or whose curve counter is seeded from
 * a bad upstream value) can drive C_idx past 35; on real hardware that would
 * just read whatever byte follows the table in ROM, but this port's table is
 * a 36-byte array, so an unclamped index is a genuine out-of-bounds read
 * (caught by AddressSanitizer). Clamp to the last documented entry rather
 * than fabricate data for the disputed 256-byte range.
 */
static u8 oss_lookup_speed(u8 C_idx)
{
  if (C_idx >= sizeof(title_speed_curve))
    C_idx = sizeof(title_speed_curve) - 1;

  return title_speed_curve[C_idx] >> 2;
}

/**
 * $C7A2: Apply the current X step to the X position
 *
 * \param[in,out] rec Object record to update (was IX).
 */
static void oss_apply_x_step(struct title_object *rec)
{
  rec->x += rec->x_step; /* $C7A2-$C7A8 */
}

/**
 * $C7AC: Apply the current Y step to the Y position
 *
 * \param[in,out] rec Object record to update (was IX).
 */
static void oss_apply_y_step(struct title_object *rec)
{
  rec->y += rec->y_step; /* $C7AC-$C7B2 */
}

/**
 * $C78D: Active mode -- constant velocity
 *
 * Position += velocity every frame; no curve lookup, unlike the other five
 * active modes.
 *
 * \param[in,out] rec Object record to update (was IX).
 */
static void oss_op_velocity(struct title_object *rec)
{
  rec->x += rec->x_step; /* $C78D-$C793 */
  rec->y += rec->y_step; /* $C796-$C79C */
}

/**
 * $C7ED: Active mode -- decelerate X
 *
 * X moves at its constant step (oss_apply_x_step); Y moves by a curve-table
 * magnitude subtracted from Y each frame, looked up from an incrementing
 * counter that overloads the y_step field for the lifetime of this mode
 * (the fetch-side operand order that seeds it is in object_script_step's
 * $CA/$CB case).
 *
 * \param[in,out] rec Object record to update (was IX).
 *
 * Conv: NEG followed by ADD A,(IX+$08) collapses to a single subtraction.
 */
static void oss_op_decel_x(struct title_object *rec)
{
  u8 C_idx;   /* curve counter, aliases the y_step field for this mode (was C, from (IX+3)) */
  u8 A_speed; /* looked-up curve magnitude (was A) */

  oss_apply_x_step(rec); /* $C7ED CALL $C7A2 */

  C_idx   = (u8) rec->y_step;        /* $C7F0 LD C,(IX+3) */
  A_speed = oss_lookup_speed(C_idx); /* $C7F3 CALL $C804 */
  rec->y -= A_speed;                 /* $C7F6 NEG / $C7F8-$C7FB ADD A,(IX+8) */
  rec->y_step = (s8) (C_idx + 1);    /* $C7FE INC (IX+3) */
}

/**
 * $C812: Active mode -- decelerate Y
 *
 * Mirrors oss_op_decel_x: X still moves at its constant step; Y moves by the
 * same curve lookup, added (not subtracted) and counted down instead of up.
 *
 * \param[in,out] rec Object record to update (was IX).
 */
static void oss_op_decel_y(struct title_object *rec)
{
  u8 C_idx;   /* curve counter, aliases the y_step field for this mode (was C, from (IX+3)) */
  u8 A_speed; /* looked-up curve magnitude (was A) */

  oss_apply_x_step(rec); /* $C812 CALL $C7A2 */

  C_idx   = (u8) rec->y_step;        /* $C815 LD C,(IX+3) */
  A_speed = oss_lookup_speed(C_idx); /* $C818 CALL $C804 */
  rec->y += A_speed;                 /* $C81B-$C81E ADD A,(IX+8) */
  rec->y_step = (s8) (C_idx - 1);    /* $C821 DEC (IX+3) */
}

/**
 * $C827: Active mode -- accelerate X, variant a (negated speed, counts down)
 *
 * Y moves at its constant step (oss_apply_y_step); X moves by a curve-table
 * magnitude negated and subtracted from X each frame, looked up from a
 * counter that overloads the x_step field for the lifetime of this mode
 * (seeded via object_script_step's $CC/$CD/$CE case).
 *
 * \param[in,out] rec Object record to update (was IX).
 *
 * Conv: NEG followed by ADD A,(IX+$07) collapses to a single subtraction.
 */
static void oss_op_accel_x_a(struct title_object *rec)
{
  u8 C_idx;   /* curve counter, aliases the x_step field for this mode (was C, from (IX+2)) */
  u8 A_speed; /* looked-up curve magnitude (was A) */

  oss_apply_y_step(rec); /* $C827 CALL $C7AC */

  C_idx   = (u8) rec->x_step;        /* $C82A LD C,(IX+2) */
  A_speed = oss_lookup_speed(C_idx); /* $C82D CALL $C804 */
  rec->x -= A_speed;                 /* $C830 NEG / $C832-$C835 ADD A,(IX+7) */
  rec->x_step = (s8) (C_idx - 1);    /* $C838 DEC (IX+2) */
}

/**
 * $C83E: Active mode -- accelerate X, variant b (positive speed, counts down)
 *
 * Same shape as oss_op_accel_x_a, without the negation.
 *
 * \param[in,out] rec Object record to update (was IX).
 */
static void oss_op_accel_x_b(struct title_object *rec)
{
  u8 C_idx;   /* curve counter, aliases the x_step field for this mode (was C, from (IX+2)) */
  u8 A_speed; /* looked-up curve magnitude (was A) */

  oss_apply_y_step(rec); /* $C83E CALL $C7AC */

  C_idx   = (u8) rec->x_step;        /* $C841 LD C,(IX+2) */
  A_speed = oss_lookup_speed(C_idx); /* $C844 CALL $C804 */
  rec->x += A_speed;                 /* $C847-$C84A ADD A,(IX+7) */
  rec->x_step = (s8) (C_idx - 1);    /* $C84D DEC (IX+2) */
}

/**
 * $C853: Active mode -- accelerate X, variant c (positive speed, counts up)
 *
 * Same shape as oss_op_accel_x_b, counting the curve index up instead of
 * down.
 *
 * \param[in,out] rec Object record to update (was IX).
 */
static void oss_op_accel_x_c(struct title_object *rec)
{
  u8 C_idx;   /* curve counter, aliases the x_step field for this mode (was C, from (IX+2)) */
  u8 A_speed; /* looked-up curve magnitude (was A) */

  oss_apply_y_step(rec); /* $C853 CALL $C7AC */

  C_idx   = (u8) rec->x_step;        /* $C856 LD C,(IX+2) */
  A_speed = oss_lookup_speed(C_idx); /* $C859 CALL $C804 */
  rec->x += A_speed;                 /* $C85C-$C85F ADD A,(IX+7) */
  rec->x_step = (s8) (C_idx + 1);    /* $C862 INC (IX+2) */
}

/**
 * $C705: Object animation script interpreter [Conv: HQ]
 *
 * Advances all 9 title-screen objects (state->title_objects[9]) by one
 * frame. Each object record carries a byte-code cursor into
 * title_scene_data; this is a state machine with two dispatch chains that
 * share six "active movement mode" opcodes ($C9-$CE):
 *
 *  - Active dispatch (oss_object_loop, $C70E): when an object's stored
 *    opcode is non-zero, runs one frame's worth of movement for whichever
 *    mode is active, then ticks its wait counter (oss_countdown, $C731),
 *    going idle (opcode -> 0) once it reaches zero.
 *  - Fetch dispatch (oss_fetch_opcode/_cont, $C740/$C746): when idle, reads
 *    script bytes until it hits a mode-setting opcode. Bytes with the sign
 *    bit clear ($00-$7F) are immediate 2-axis step deltas applied at once
 *    (oss_op_immediate_step, $C868); opcode $C8 (set row) and $D0 (jump to
 *    absolute position) act immediately too and keep fetching; a
 *    mode-setting opcode ($C9-$CF, or any other byte >= $80 as a fallback)
 *    is stored as the new active opcode, its operand bytes are read, and the
 *    object is re-dispatched through the active chain immediately -- so a
 *    freshly fetched mode runs its first frame of movement in the same call
 *    that fetched it.
 *
 * The six active modes are: constant velocity ($C9, oss_op_velocity);
 * decelerate X/Y ($CA/$CB, oss_op_decel_x/oss_op_decel_y); and three
 * accelerate-X variants ($CC/$CD/$CE, oss_op_accel_x_a/oss_op_accel_x_c/
 * oss_op_accel_x_b -- note the fetch dispatch maps CD to variant c and CE to
 * variant b, not alphabetically). See those functions' own prologues for
 * how the decelerate/accelerate modes overload the x_step/y_step fields as
 * curve-lookup counters.
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: opcode $D2 ("end of script") is `POP HL; RET` on real hardware --
 * with no PUSH anywhere in this call chain, that pops object_script_step's
 * own return address as data and returns via the frame beneath it, aborting
 * all the way back into ts_animate_frame's *caller* and skipping the rest of
 * that frame's work (clear_playfield_buffer, background-object draw,
 * stamp/sleep). This is the same class of self-looping/stack-unwind escape
 * hatch already left unmodelled in ts_animate_frame's own prologue (its
 * $C702 self-jump and the blitters' overrun escape). Since this port already
 * treats ts_animate_frame as a plain function that always returns to its
 * caller once per frame, the in-scope equivalent here is to stop processing
 * any further objects this frame (an early `return`) rather than unwind into
 * frames this function does not own.
 *
 * Conv: oss_op_immediate_step's Y-magnitude extraction rotates A right
 * through the carry flag 3 times before masking with AND $03; the carry bit
 * fed into the first rotation (left over from the preceding ADD A,(IX+$07))
 * lands in a bit position the following AND discards, so the result is
 * carry-independent and equals `(byte >> 3) & 0x03`. Translated directly as
 * a shift rather than modelling the rotate/carry chain.
 *
 * Conv: the "JP $C70E" at the end of oss_save_cursor (re-dispatching a
 * freshly fetched mode through the active chain in the same call) is a
 * genuine back-edge, not sequential code -- modelled here as the `continue`
 * of the outer per-object loop, distinct from the `continue`s inside the
 * fetch loop that model oss_fetch_opcode_cont's own re-fetch jumps ($C8,
 * $D0). See the project's "verify back-edges" pitfall.
 */
static void object_script_step(chqstate_t *state)
{
  int                  obj;       /* object-loop index, 0-8 (was B, DJNZ counter) */
  struct title_object *rec;       /* current object record (was IX) */
  const u8            *HLscript;  /* script byte-code cursor while fetching (was HL) */
  u8                   A_byte;    /* fetched script byte (was A) */
  s8                   A_x_delta; /* immediate-step X delta (was A) */
  s8                   A_y_delta; /* immediate-step Y delta (was A) */

  for (obj = 0; obj < 9; obj++) {
    rec = &state->title_objects[obj];

    for (;;) { /* models the "JP $C70E" re-entry after fetching a new opcode */
      if (rec->opcode == 0) {
        /* $C740-$C743 oss_fetch_opcode: idle -- fetch from the script. */
        HLscript = rec->script;

        for (;;) { /* $C746 oss_fetch_opcode_cont: fetch/instant-op loop */
          A_byte = *HLscript++;

          if ((s8) A_byte >= 0) {
            /* $C868-$C88D oss_op_immediate_step: immediate 2-axis step. */
            A_x_delta = (A_byte & 0x03) << 1;     /* $C869-$C871 */
            if (A_byte & 0x04)                     /* $C86B/$C86D BIT 2,C */
              A_x_delta = -A_x_delta;              /* $C86F NEG */
            rec->x += A_x_delta;                   /* $C873-$C876 */

            A_y_delta = ((A_byte >> 3) & 0x03) << 1; /* $C87A-$C87D */
            if (A_byte & 0x20)                        /* $C87F BIT 5,C */
              A_y_delta = -A_y_delta;                 /* $C883 NEG */
            rec->y += A_y_delta;                      /* $C885-$C88A */

            continue; /* $C88D JP $C746 */
          }

          /* Sign bit set: a "real" opcode -- store it as the new active
           * opcode, then read its operand bytes (if any). */
          rec->opcode = A_byte; /* $C74C */

          switch (A_byte) {
            case 0xC8: /* set screen-row byte, 1 operand */
              rec->row = *HLscript++; /* $C7C2-$C7C4 */
              continue;               /* $C7C7 JP $C746 */

            case 0xC9: /* set velocity, 3 operand bytes: x,y,wait */
              rec->x_step = (s8) *HLscript++; /* $C77B-$C77D */
              rec->y_step = (s8) *HLscript++; /* $C780-$C782 */
              rec->wait   = *HLscript++;      /* $C785-$C787 */
              break;                          /* -> save cursor below */

            case 0xCA: /* decelerate X, 3 operand bytes */
            case 0xCB: /* decelerate Y, same operand layout */
              /* $C7DC-$C7E8: note the y_step slot is read first here -- it
               * seeds the deceleration-curve counter, not a real Y step; see
               * oss_op_decel_x/oss_op_decel_y. */
              rec->y_step = (s8) *HLscript++;
              rec->wait   = *HLscript++;
              rec->x_step = (s8) *HLscript++;
              break;

            case 0xCC: /* accelerate X variant a */
            case 0xCD: /* accelerate X variant c */
            case 0xCE: /* accelerate X variant b */
              /* $C7CA-$C7D6: the x_step slot seeds the acceleration-curve
               * counter here, not a real X step; see oss_op_accel_x_*. */
              rec->x_step = (s8) *HLscript++;
              rec->wait   = *HLscript++;
              rec->y_step = (s8) *HLscript++;
              break;

            case 0xCF: /* "wait N frames", 1 operand byte */
              rec->wait = *HLscript++; /* $C785-$C787 (oss_read_wait_operand) */
              break;

            case 0xD0: /* jump to absolute position, 2 operand bytes */
              rec->x = *HLscript++; /* $C7B6-$C7B8 */
              rec->y = *HLscript++; /* $C7BB-$C7BD */
              continue;             /* $C7C0 JR $C746 */

            case 0xD2: /* end of script -- see this function's own Conv note */
              return;

            default: /* $D1 and anything else: no operand bytes, falls
                      * straight to oss_save_cursor ($C76C-$C772) */
              break;
          }

          /* $C772-$C778 oss_save_cursor: persist the advanced cursor. */
          rec->script = HLscript;
          break; /* leave the fetch loop; the outer `continue` below
                  * re-dispatches this object immediately ("JP $C70E") */
        }

        continue; /* re-check rec->opcode (now non-zero) at the top */
      }

      /* $C70E-$C72F oss_object_loop: active-mode dispatch. */
      switch (rec->opcode) {
        case 0xC9: oss_op_velocity(rec);  break;
        case 0xCA: oss_op_decel_x(rec);   break;
        case 0xCB: oss_op_decel_y(rec);   break;
        case 0xCC: oss_op_accel_x_a(rec); break;
        case 0xCD: oss_op_accel_x_c(rec); break;
        case 0xCE: oss_op_accel_x_b(rec); break;
        default: break; /* 0xCF ("wait") and any fallback opcode: no
                         * per-frame movement of its own -- falls straight
                         * to the countdown below */
      }

      /* $C731-$C737 oss_countdown: tick the wait counter; go idle (so the
       * next frame re-fetches) once it reaches 0. */
      if (--rec->wait == 0)
        rec->opcode = 0;

      break; /* $C73B oss_next_object: move on to the next object */
    }
  }
}

/**
 * $CC04: Clear the playfield bitmap
 *
 * Zero-fills the bitmap area used by the title-screen object sprites: all
 * of screen third 2 ($4800-$4FFF, all 8 character rows) and the top 5
 * character rows of screen third 3 ($5000-$57FF), leaving third 3's bottom
 * 3 character rows untouched so the fixed overlay text printed once by
 * title_screen_driver (print_character) is not wiped out every frame. Only
 * 28 of each scanline's 32 bytes are cleared (screen columns 2-29), leaving
 * a 2-column margin on each edge. Classic "LD SP,HL; PUSH x N" fast-fill
 * trick (see the project's known translation pitfall of the same name): SP
 * is repointed at the bitmap and 14 PUSH DE instructions (DE=0) fill 28
 * bytes backward from HL, DJNZ-looped 8 times per character row, stepping
 * through the $x00 third boundary via the usual ADD A,$20 / carry pattern.
 *
 * \param[in,out] state Pointer to game state.
 *
 * Conv: the PUSH-fill collapses to one memset per scanline; the real stack
 * save/restore at $CC04/$CC4C-$CC4F has no C equivalent (SP is never
 * repurposed as a data pointer here) and is omitted.
 */
static void clear_playfield_buffer(chqstate_t *state)
{
  int H;          /* screen address high byte (was H) */
  int L;          /* screen address low byte (was L) */
  int B_scanline; /* scanline countdown within one character row, 8 (was B) */

  H = 0x48; /* $CC08 LD HL,$481E */
  L = 0x1E;

  do { /* $CC10-$CC29: screen third 2, all 8 character rows */
    B_scanline = 8; /* $CC0E/$CC10 LD B,C (C=$08) */
    do {
      memset(ADDRTOSCREEN((H << 8) | L) - 28, 0, 28); /* $CC11-$CC1F */
      H++; /* $CC20 INC H */
    } while (--B_scanline); /* $CC21 DJNZ */
    H = 0x48; /* $CC23 */
    L += 0x20; /* $CC25-$CC28 */
  } while (L <= 0xFF); /* $CC29 JP NC,$CC10 */
  L &= 0xFF;
  H = 0x50; /* $CC2C */

  do { /* $CC2E-$CC49: screen third 3, top 5 character rows only */
    B_scanline = 8; /* $CC2E LD B,C */
    do {
      memset(ADDRTOSCREEN((H << 8) | L) - 28, 0, 28); /* $CC2F-$CC3D */
      H++; /* $CC3E INC H */
    } while (--B_scanline); /* $CC3F DJNZ */
    H = 0x50; /* $CC41 */
    L += 0x20; /* $CC43-$CC46 */
  } while (L < 0xA0); /* $CC47-$CC49 JP C,$CC2E */
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
 * Conv: the self-looping structure and its stack-unwind escape hatch are not
 * modelled; this function always draws one frame then returns, matching how
 * other per-frame functions in this port are called once per iteration from
 * a caller-owned loop (e.g. drive_attract_demo). Only the EI/HALT
 * frame-pacing point is translated, via the same stamp/sleep idiom used
 * elsewhere. Continuous animation during the attract-mode wait is driven by
 * ts_wait_loop calling this function once per iteration — see its own Conv
 * note for why that caller-owned loop, rather than this function's literal
 * self-loop, is where repeated invocation lives in the C port.
 *
 * Conv: the fg/bg draw loops' EXX pairs ($C6D0/$C6DD, $C6E2, $C6E9/$C6FD)
 * only protect the loop counter (B) and record-stride (DE) from being
 * clobbered by the blitter call -- they do not carry any value between main
 * and shadow sets that survives past the following EXX. This is register
 * protection around a call, not persistent banking, so it needs no shadow
 * variables in C: the loop counter is a plain C `for`, and the blitter is
 * called directly with the fields it needs.
 *
 * Conv: added a speccy->draw() call after the bg objects are blitted. The
 * Z80 has no equivalent -- it draws straight into the real display memory
 * -- but the SDL port renders into an off-screen buffer that must be
 * explicitly presented every frame, or the host window only ever shows the
 * single frame drawn by title_screen_driver before this function starts
 * looping.
 */
static void ts_animate_frame(chqstate_t *state)
{
  static const zxbox_t  playfield_box = { /* lower two-thirds of screen */
    0, 0, SCREEN_WIDTH, PLAYFIELD_HEIGHT
  };
  int                   obj; /* object index within the fg/bg loop (was B, DJNZ counter) */
  struct title_object  *rec; /* current object record (was IX) */

  state->speccy->stamp(state->speccy);

  for (obj = 0; obj < 6; obj++) { /* $C6C7-$C6E0: 6 foreground objects */
    rec = &state->title_objects[obj];
    compute_glyph_blit_params(state, rec->y, rec->x, rec->row); /* $C6DA */
  }

  object_script_step(state); /* $C6E3 */

  clear_playfield_buffer(state); /* $C6E6 */

  for (obj = 6; obj < 9; obj++) { /* $C6EA-$C700: 3 background objects */
    rec = &state->title_objects[obj];
    compute_glyph_blit_params_b(state, rec->y, rec->x, rec->row); /* $C6FA */
  }

  state->speccy->draw(state->speccy, &playfield_box); /* Conv: added */

  state->speccy->sleep(state->speccy, STANDARD_SLEEP);
}

/**
 * $C8CD-$C902 / $C957-$C98C: Compute destination address and glyph-table
 * entry for a title-screen object
 *
 * Shared by compute_glyph_blit_params and compute_glyph_blit_params_b, which
 * are otherwise identical apart from the row-offset walk and dispatch table
 * they feed. Clamps the object's Y screen position to a maximum of $6F
 * (rows below that are off the bottom of the drawable window and must be
 * partially skipped by the caller), builds the destination screen address
 * from the clamped Y and the X position, and looks up the glyph's row-pair
 * count/width/bitmap pointer by index (row + ((x >> 1) & 3)) into
 * title_glyph_table.
 *
 * \param[in]  B_y   Object Y screen position (was B).
 * \param[in]  C_x   Object X screen position (was C).
 * \param[in]  L_row Object row/height byte, selects which of 4 glyph
 *                    variants for this animation row (was L).
 * \param[out] out   Filled with the destination address, glyph pointer, row-
 *                    pair count, width selector and Y-clamp state.
 *
 * Conv: the two "RRA/SCF/RRA/RRA" then "XOR B ; AND mask ; XOR B" sequences
 * that build D and E are translated literally with the RR/RLC macros from
 * Z80.h and the replace-bits-under-a-mask idiom, matching the style already
 * used for the AY register merge in compute_channel_ay_registers.
 */
static void compute_glyph_geometry(u8 B_y, u8 C_x, u8 L_row,
                                    glyph_blit_geometry_t *out)
{
  u8  B_clamped;         /* Y, clamped to a maximum of $6F (was B) */
  u8  A;                 /* working accumulator (was A) */
  int carry;             /* Z80 carry flag, used by the RR/RLC macros */
  u8  B_screen_rows;     /* $AF - B_clamped, reused as the mask-merge operand (was B) */
  u8  D;                 /* destination screen address high byte (was D) */
  u8  E;                 /* destination screen address low byte (was E) */
  int glyph_index;       /* index into title_glyph_table (was BC, table offset / 4) */
  const title_glyph_t *glyph;

  /* $C8C5-$C8CC / $C94F-$C956 */
  if (B_y < 0x70) {
    B_clamped           = B_y;
    out->A_excess       = 0;    /* unused: carry_initial skips the row-offset walk */
    out->carry_initial  = 1;
  } else {
    B_clamped           = 0x6F;
    out->A_excess       = (u8) (B_y - 0x6F);
    out->carry_initial  = 0;
  }

  /* $C8CE-$C8DC / $C958-$C966: D = destination screen address high byte. */
  B_screen_rows = (u8) (0xAF - B_clamped);
  A = B_screen_rows;
  carry = 0; RR(A); /* AND A ; RRA */
  carry = 1; RR(A); /* SCF   ; RRA */
  carry = 0; RR(A); /* AND A ; RRA */
  A ^= B_screen_rows;
  A &= 0xF8;
  A ^= B_screen_rows;
  D = A;

  /* $C8DD-$C8E7 / $C967-$C971: E = destination screen address low byte. */
  A = C_x;
  RLC(A); RLC(A); RLC(A);
  A ^= B_screen_rows;
  A &= 0xC7;
  A ^= B_screen_rows;
  RLC(A); RLC(A);
  E = A;

  out->H = D;
  out->L = E;

  /* $C8E8-$C901 / $C972-$C98B: glyph table lookup. The 4-byte table entry
     (height_pairs, width_bytes, bitmap lo, bitmap hi) is title_glyph_t's
     layout exactly, so the raw byte reads collapse to direct indexing. */
  glyph_index = L_row + ((C_x >> 1) & 0x03);
  assert(glyph_index >= 0 && glyph_index < TITLE_GLYPH_COUNT);

  glyph = &title_glyph_table[glyph_index];
  out->B_height_pairs = glyph->height_pairs;
  out->C_width_select = glyph->width_bytes;
  out->HLsrc          = glyph->bitmap;
}

/**
 * $C9F3 (and the 13 further copies at $CA05, $CA40, $CA52, $CA98, $CAAA,
 * $CAF3, $CB05, $CB57, $CB69, $CBA1, $CBB3, $CBE0, $CBF2): Advance a glyph-
 * blit screen address to the next scanline
 *
 * The caller increments H before calling this; this function applies the
 * ZX Spectrum screen memory's non-linear "third boundary" correction when a
 * character row completes (H & 7 == 0 after the increment).
 *
 * \param[in,out] H Screen address high byte.
 * \param[in,out] L Screen address low byte.
 *
 * Conv: factored into a shared helper rather than repeating the 14 near-
 * identical inline copies in the disassembly -- the same "Conv: extracted to
 * function" treatment next_scr_row got in Main.c. next_scr_row itself is
 * static to Main.c and not visible here; clear_playfield_buffer above
 * already established this file's own precedent of modelling this exact
 * address math locally rather than sharing it across files, so this helper
 * follows that precedent instead of exposing next_scr_row.
 */
static void advance_glyph_scanline(int *H, int *L)
{
  if ((*H & 0x07) != 0)
    return;

  *H -= 0x08;
  *L += 0x20;
  if (*L > 0xFF) {
    *L &= 0xFF;
    *H += 0x08;
  }
}

/**
 * $C9D5 (and identically-shaped copies at $CA17, $CA64, $CABC, $CB17,
 * $CB7B, $CBC5): Masked-sprite OR-blit, row_bytes wide
 *
 * Draws B_height_pairs row-pairs (2 scanlines each) from src into the
 * screen bitmap starting at (H,L), OR-ing row_bytes source bytes into each
 * scanline so the glyph never overwrites bits already set by an overlapping
 * sprite. Advances the screen address one scanline at a time via
 * advance_glyph_scanline.
 *
 * \param[in,out] state         Pointer to game state.
 * \param[in]     H             Destination screen address high byte.
 * \param[in]     L             Destination screen address low byte.
 * \param[in]     src           Glyph bitmap source pointer.
 * \param[in]     B_height_pairs Number of row-pairs to draw.
 * \param[in]     row_bytes     Bytes to OR into each scanline.
 *
 * Conv: the Z80 draws each row-pair via "LD SP,HL; POP DE", i.e. two source
 * bytes at a time (E first, then D) -- see the project's known "LD SP,HL;
 * POP x N sprite copy" translation pitfall. Since the bytes are written
 * verbatim with no mask table or flip, this collapses to a plain sequential
 * `*src++` per byte, matching the pitfall's documented equivalence. The 7
 * width-specific unrolled routines ($C9D5-$CBC5) share this exact shape
 * (only row_bytes and the reachable dispatch entry differ), so they are
 * modelled as one parameterised helper rather than 7 near-duplicate bodies.
 * Each width routine's own fixed delay loop (e.g. $C9E8-$C9EB, present on
 * widths 1-5 only) exists purely to pad out real hardware frame timing; the
 * C port has no such deadline to protect (see compute_glyph_blit_params'
 * Conv note on $C93C/$C93D), so none of the delay loops are translated.
 *
 * Conv: compute_glyph_blit_params only clamps the *top* of the glyph (see
 * its own Conv note on the $C906-$C916 skip loop); the disassembly has no
 * symmetric clamp for the bottom, so a fast-moving object (e.g. one driven
 * by oss_op_velocity) can walk this loop's (H,L) address below screen third
 * 3 and off the bottom of the physical display. On real hardware that just
 * pokes stray bytes into attribute memory (or further afield) -- harmless
 * enough that nobody noticed. This port's screen is a fixed-size struct, not
 * flat memory, so the equivalent out-of-range write is skipped instead of
 * performed, rather than asserting or corrupting adjacent struct fields; the
 * address/source advance below still runs unconditionally so the timing and
 * any later in-range rows stay correct.
 */
static void blit_glyph_rows(chqstate_t *state, int H, int L, const u8 *src,
                            int B_height_pairs, int row_bytes)
{
  u8 *dst;    /* current scanline's destination byte(s) (was HL) */
  int row;    /* 0 or 1: which scanline of the current row-pair */
  int i;      /* byte offset within the current scanline */
  int addr;   /* destination Z80 screen address for this scanline */

  do {
    for (row = 0; row < 2; row++) {
      addr = (H << 8) | L;
      if (addr >= SCREEN_START_ADDRESS &&
          addr <  SCREEN_START_ADDRESS + SCREEN_BITMAP_LENGTH) {
        dst = ADDRTOSCREEN(addr);
        for (i = 0; i < row_bytes; i++)
          dst[i] |= *src++;
      } else {
        src += row_bytes; /* Conv: off-screen scanline, see note above */
      }
      H++;
      advance_glyph_scanline(&H, &L);
    }
  } while (--B_height_pairs);
}

/**
 * $C9D5: Width-1 masked-sprite OR-blit (1 byte per scanline).
 */
static void blit_width1(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 1);
}

/**
 * $CA17: Width-2 masked-sprite OR-blit (2 bytes per scanline).
 */
static void blit_width2(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 2);
}

/**
 * $CA64: Width-3 masked-sprite OR-blit (3 bytes per scanline).
 */
static void blit_width3(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 3);
}

/**
 * $CABC: Width-4 masked-sprite OR-blit (4 bytes per scanline).
 */
static void blit_width4(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 4);
}

/**
 * $CB17: Width-5 masked-sprite OR-blit (5 bytes per scanline).
 */
static void blit_width5(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 5);
}

/**
 * $CB7B: "Width-6" masked-sprite OR-blit
 *
 * Conv/bug preserved literally: despite its position in the dispatch chain
 * (reached when the glyph's width selector is 6 or more), this routine's
 * instructions are byte-for-byte identical in shape to blit_width2 -- it
 * draws only 2 bytes per scanline, not 6. The source pointer only advances
 * 2 bytes per scanline to match. This is verified against the disassembly,
 * not assumed: every LD (HL),A / INC L pair at $CB7B-$CB99 matches
 * $CA17-$CA3B exactly. Any glyph whose real width_bytes is 6 or 7 would
 * therefore be drawn with its rightmost columns missing and its source data
 * under-consumed -- an original-game quirk, not a translation bug, and is
 * not "fixed" here.
 */
static void blit_width6(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 2);
}

/**
 * $CBC5: "Width-7" masked-sprite OR-blit
 *
 * Conv/bug preserved literally: only reachable via
 * blit_masked_sprite_dispatch_b's default case. Byte-for-byte identical in
 * shape to blit_width1 -- draws only 1 byte per scanline despite its
 * position at the end of the width-7 dispatch chain. See blit_width6's note
 * above; the same quirk applies here one width class down.
 */
static void blit_width7(chqstate_t *state, int H, int L, const u8 *src,
                        int B_height_pairs)
{
  blit_glyph_rows(state, H, L, src, B_height_pairs, 1);
}

/**
 * $C917: Masked-sprite blit dispatch (foreground objects)
 *
 * Selects one of the 7 width-specific OR-blit routines by C_width_select,
 * decremented against 1..5 with an explicit test; anything else (6 or more)
 * falls through to blit_width6 -- blit_width7 is unreachable from this
 * dispatcher (see its own Conv note).
 *
 * \param[in,out] state          Pointer to game state.
 * \param[in]     H              Destination screen address high byte.
 * \param[in]     L              Destination screen address low byte.
 * \param[in]     src            Glyph bitmap source pointer.
 * \param[in]     B_height_pairs Number of row-pairs to draw.
 * \param[in]     C_width_select Width selector, 1-7.
 */
static void blit_masked_sprite_dispatch(chqstate_t *state, int H, int L,
                                        const u8 *src, int B_height_pairs,
                                        int C_width_select)
{
  switch (C_width_select) {
  case 1:  blit_width1(state, H, L, src, B_height_pairs); break;
  case 2:  blit_width2(state, H, L, src, B_height_pairs); break;
  case 3:  blit_width3(state, H, L, src, B_height_pairs); break;
  case 4:  blit_width4(state, H, L, src, B_height_pairs); break;
  case 5:  blit_width5(state, H, L, src, B_height_pairs); break;
  default: blit_width6(state, H, L, src, B_height_pairs); break;
  }
}

/**
 * $C9AF: Masked-sprite blit dispatch (background objects)
 *
 * Structurally identical to blit_masked_sprite_dispatch but tests widths
 * 1..6 explicitly, so blit_width7 (unreachable from the foreground
 * dispatcher above) is reached here as the default case.
 *
 * \param[in,out] state          Pointer to game state.
 * \param[in]     H              Destination screen address high byte.
 * \param[in]     L              Destination screen address low byte.
 * \param[in]     src            Glyph bitmap source pointer.
 * \param[in]     B_height_pairs Number of row-pairs to draw.
 * \param[in]     C_width_select Width selector, 1-7.
 */
static void blit_masked_sprite_dispatch_b(chqstate_t *state, int H, int L,
                                          const u8 *src, int B_height_pairs,
                                          int C_width_select)
{
  switch (C_width_select) {
  case 1:  blit_width1(state, H, L, src, B_height_pairs); break;
  case 2:  blit_width2(state, H, L, src, B_height_pairs); break;
  case 3:  blit_width3(state, H, L, src, B_height_pairs); break;
  case 4:  blit_width4(state, H, L, src, B_height_pairs); break;
  case 5:  blit_width5(state, H, L, src, B_height_pairs); break;
  case 6:  blit_width6(state, H, L, src, B_height_pairs); break;
  default: blit_width7(state, H, L, src, B_height_pairs); break;
  }
}

/**
 * $C8C5: Draw a foreground title-screen object's glyph [Conv: HQ]
 *
 * Computes the destination address and glyph-table entry via
 * compute_glyph_geometry, then, if the object's Y position was clamped
 * (partially off the bottom of the drawable window), walks the source
 * pointer forward one row-pair's worth of bytes at a time until the clamp
 * excess is consumed, decrementing the row-pair count in step. If the
 * row-pair count reaches zero first, the object is entirely off-screen and
 * nothing is drawn. Otherwise dispatches to the width-specific OR-blit
 * routine.
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     B_y   Object Y screen position (was B).
 * \param[in]     C_x   Object X screen position (was C).
 * \param[in]     L_row Object row/height byte (was L).
 *
 * Conv: $C93D onwards, the Z80 saves the real SP, repoints SP at the glyph
 * source so the blit routines can POP bytes from it, then restores the real
 * SP before returning (or via blit_abort_restore_sp on early abort). This
 * whole mechanism is a way of getting fast sequential byte reads out of the
 * Z80's POP instruction; it has no bearing on control flow -- every path
 * still returns cleanly to this function's caller, exactly like an ordinary
 * nested C call. clear_playfield_buffer above established the same
 * conclusion for its own "LD SP,HL; PUSH x N" fast-fill trick. Accordingly
 * the real-SP save/restore is omitted entirely; blit_glyph_rows above reads
 * the source with plain sequential `*src++`, and both dispatchers return
 * normally with no simulated stack juggling. cgb_delay_tail's fixed delay
 * loop is likewise a hardware frame-timing pad with no C equivalent (there
 * is no frame deadline to protect) and is not translated.
 *
 * Conv: the row-offset skip loop ($C906-$C916) is a post-test loop that
 * decrements the row-pair count first and only tests the skip count
 * afterwards. When the Y clamp excess is exactly 1 (Y = $70 or $71), excess
 * >> 1 is 0, and the u8 skip counter wraps from 0 to 255 on its first
 * decrement, effectively running the skip loop until the row-pair count
 * itself reaches zero -- silently drawing nothing for those two Y values.
 * This is a latent quirk of the original code (compute_glyph_blit_params_b
 * below guards against it explicitly), not a translation bug, and is
 * preserved via A_skip_pairs' u8 wraparound rather than "fixed".
 */
static void compute_glyph_blit_params(chqstate_t *state, u8 B_y, u8 C_x,
                                       u8 L_row)
{
  glyph_blit_geometry_t g;
  u8                    A_skip_pairs; /* row-pairs of source to skip (was A) */

  compute_glyph_geometry(B_y, C_x, L_row, &g);

  if (!g.carry_initial) {
    /* $C906-$C916 */
    A_skip_pairs = (u8) (g.A_excess >> 1);
    do {
      g.HLsrc += g.C_width_select * 2;
      if (--g.B_height_pairs == 0)
        return; /* entirely off-screen -- draw nothing */
    } while (--A_skip_pairs != 0); /* u8 wrap intentional, see Conv note above */
  }

  blit_masked_sprite_dispatch(state, g.H, g.L, g.HLsrc, g.B_height_pairs,
                              g.C_width_select);
}

/**
 * $C94F: Draw a background title-screen object's glyph [Conv: HQ]
 *
 * Structurally identical to compute_glyph_blit_params above (see its Conv
 * notes for the SP-as-pointer and frame-timing decisions, which apply here
 * unchanged), but feeds blit_masked_sprite_dispatch_b, and its row-offset
 * skip loop computes the per-row-pair source stride differently for the
 * "width-6"/"width-7" quirk routines (4 bytes for width 6, 2 bytes for
 * width 7, matching blit_width6/blit_width7's real 2-byte/1-byte-per-row
 * consumption) and explicitly guards the skip count against the u8-wrap
 * quirk noted in compute_glyph_blit_params (forcing a minimum of 1).
 *
 * \param[in,out] state Pointer to game state.
 * \param[in]     B_y   Object Y screen position (was B).
 * \param[in]     C_x   Object X screen position (was C).
 * \param[in]     L_row Object row/height byte (was L).
 */
static void compute_glyph_blit_params_b(chqstate_t *state, u8 B_y, u8 C_x,
                                         u8 L_row)
{
  glyph_blit_geometry_t g;
  u8                    A_skip_pairs; /* row-pairs of source to skip (was A) */
  u8                    E_stride;     /* per-row-pair source advance, bytes (was E) */

  compute_glyph_geometry(B_y, C_x, L_row, &g);

  if (!g.carry_initial) {
    /* $C990-$C9AE */
    if (g.C_width_select < 6)
      E_stride = (u8) (g.C_width_select << 1);
    else
      E_stride = (u8) ((8 - g.C_width_select) << 1); /* width 6 -> 4, width 7 -> 2 */

    A_skip_pairs = (u8) (g.A_excess >> 1);
    if (A_skip_pairs == 0)
      A_skip_pairs = 1; /* $C9A3-$C9A5: guards the wrap quirk noted above */

    do {
      g.HLsrc += E_stride;
      if (--g.B_height_pairs == 0)
        return; /* entirely off-screen -- draw nothing */
    } while (--A_skip_pairs != 0);
  }

  blit_masked_sprite_dispatch_b(state, g.H, g.L, g.HLsrc, g.B_height_pairs,
                                g.C_width_select);
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
  memset(ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR), 0, 0x200); /* $C890-$C89B */
  memset(ADDRTOSCREEN(SCREEN_PLAYFIELD_BITMAP_ADDR), 0, 0x1000); /* $C89C-$C8A7 */
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
  u8 *HLattr;  /* attribute write cursor (was HL) */
  int C_count; /* outer repeat count, 16 (was C) */

  clear_screen_bitmap_and_attrs(state);

  HLattr = ADDRTOATTRS(SCREEN_PLAYFIELD_ATTRS_ADDR);
  C_count = PLAYFIELD_HEIGHT / 8;
  do {
    *HLattr++ = attribute_BLACK_OVER_BLACK;
    *HLattr++ = attribute_BLACK_OVER_BLACK;
    memset(HLattr, attribute_BRIGHT_CYAN_OVER_BLACK, 28); HLattr += 28; /* Conv: replaced loop */
    *HLattr++ = attribute_BLACK_OVER_BLACK;
    *HLattr++ = attribute_BLACK_OVER_BLACK;
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
  static const zxbox_t playfield_box = { /* lower two-thirds of screen */
    0, 0, SCREEN_WIDTH, PLAYFIELD_HEIGHT
  };
  u8         A_scene_bits;   /* rotating scene-selector pseudo-random value (was A) */
  int        carry;          /* required by the RLC/RR macros (carry) */
  int        bit;            /* scene-table bit-test index, 0-3 (Conv: rolled RRA/JR C chain) */
  int        scene_idx;      /* chosen scene table index, 0-4 */
  const u8  *HL_scene_table; /* chosen scene table's object-record base (was HL) */
  int        obj;            /* object-record loop index, 0-8 (was B) */
  u16        DEscript_addr;  /* script pointer, reassembled from 2 Z80-address bytes (was DE) */

  for (;;) {
    clear_and_fill_border_attrs(state); /* $C59E CALL $C8A9 */

    /* $C5A1-$C5A9: read the persisted scene selector, rotate it left,
     * mask to 5 bits, and force it to 1 if that leaves zero -- then persist
     * the new value for the next restart. */
    A_scene_bits = state->title_scene_selector; /* $C5A1 LD A,$00 (SM) */
    RLC(A_scene_bits); /* $C5A3 RLCA */
    A_scene_bits &= 0x1F; /* $C5A4 AND $1F */
    if (!A_scene_bits) /* $C5A6 JR NZ,$C5A9 */
      A_scene_bits++; /* $C5A8 INC A */
    state->title_scene_selector = A_scene_bits; /* $C5A9 LD ($C5A2),A */

    /* $C5AC-$C5C7: pick one of 5 scene tables by testing successive bits of
     * A via RRA; the first bit found set selects the table, defaulting to
     * the 5th if none of the low 4 bits are set.
     * Conv: the four unrolled "LD HL,addr / RRA / JR C" checks collapse to a
     * loop over the same 4 bit tests; behaviourally identical. */
    scene_idx = 4;
    for (bit = 0; bit < 4; bit++) {
      RR(A_scene_bits); /* $C5AF/$C5B5/$C5BB/$C5C1 RRA */
      if (carry) {
        scene_idx = bit;
        break;
      }
    }
    HL_scene_table = &title_scene_data[title_scene_table_offset[scene_idx]];

    print_string(state, title_screen_credits_text); /* $C5C8-$C5CB: draw the
                                                       * copyright/credits
                                                       * text block ($CC50). */

    /* $C5CE-$C602: zero the $BB00-$BB4F object array, then copy the chosen
     * scene's 9 5-byte object records into it, reordering each record's
     * bytes [x, y, row, script_lo, script_hi] into the object fields
     * x/y/row/script (confirmed against object_script_step's own (IX+n)
     * accesses at $C740-$C7AC, not the (misleading) inline comment at
     * $C5DB-$C5E3, which names the wrong offsets for the script pointer). */
    for (obj = 0; obj < 9; obj++) {
      state->title_objects[obj].opcode = 0; /* $C5CE-$C5D9 zero-fill */
      state->title_objects[obj].wait   = 0;
      state->title_objects[obj].x_step = 0;
      state->title_objects[obj].y_step = 0;

      state->title_objects[obj].x = HL_scene_table[0]; /* $C5E5-$C5E7 */
      state->title_objects[obj].y = HL_scene_table[1]; /* $C5EA-$C5EC */
      state->title_objects[obj].row = HL_scene_table[2]; /* $C5EF-$C5F1 */

      DEscript_addr = (u16) (HL_scene_table[3] | (HL_scene_table[4] << 8)); /* $C5F4-$C5FB */
      state->title_objects[obj].script =
        &title_scene_data[DEscript_addr - TITLE_SCENE_DATA_BASE];

      HL_scene_table += 5; /* the 5 INC HL's in the $C5E5-$C5FB read loop;
                             * the Z80's own $C5FE ADD IX,DE (DE=$0009)
                             * instead advances the *destination* record
                             * pointer, modelled here by the obj loop index. */
    }

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

    state->speccy->draw(state->speccy, &playfield_box); /* Conv: added */

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
 * Conv: real hardware drives continuous scene animation from ts_animate_frame
 * ($C6C4) itself -- it never returns under normal operation (see its own
 * prologue). Since the established Conv already treats ts_animate_frame as a
 * single-frame-per-call function (matching its one call site at $C605, which
 * only ever draws frame 1), this loop is the actual caller-owned per-frame
 * loop that must invoke it repeatedly for the scene to animate while waiting
 * for input -- the same shape as drive_attract_demo calling its own per-frame
 * worker once per iteration. ts_animate_frame(state) therefore replaces this
 * loop's own stamp() call below (it already does its own stamp()/sleep(),
 * sandwiching the frame's draw/script-step work), and the two "balance this
 * iteration's stamp()" sleep() calls before `continue` are removed --
 * they are already balanced by ts_animate_frame's internal sleep().
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

    ts_animate_frame(state); /* Conv: drives continuous scene animation from
                               * this caller-owned loop -- see prologue. */

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
      continue; /* Conv: no balancing sleep() needed -- already closed out by
                 * ts_animate_frame's own sleep() (see prologue) */
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
      continue; /* Conv: no balancing sleep() needed -- see prologue */
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
  u8  D_key_code;     /* sentinel 0xFF at entry; row-found flag/result (was D) */
  int E_row_value;    /* row's contribution to the packed code, decremented per row (was E) */
  u8  B_port_hi;      /* high byte of keyboard IN port; rotated through all eight rows (was B) */
  u8  A_pressed_mask; /* active key bits for the current row: inverted, masked to 5 bits (was A) */
  u8  H_bits;         /* copy of A_pressed_mask, shifted right to find the set bit (was H) */
  u8  A_code;         /* row/bit code accumulator, decremented by 8 per shift (was A) */

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
  u8  ambiguous;   /* scan_keyboard_matrix ambiguity flag (was flags) */
  u8  D_key_code;  /* packed key code from scan_keyboard_matrix (was D) */
  u8  A_key_code;  /* accepted key code, used for storage/lookup (was A) */
  u8  B_dup_count; /* duplicate-check count: C_control_index-1 already-
                    * assigned slots (was B) */
  u8  dup_i;       /* duplicate-check loop index (was HL-$FFF7) */
  int index_bytes; /* byte offset into control_key_names[] (was HL-$FF95) */
  u8  char0;       /* first character of the looked-up key name (was A) */
  u8  char1;       /* second character, with the EOS bit set (was A) */

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
  static const zxbox_t playfield_box = { /* lower two-thirds of screen */
    0, 0, SCREEN_WIDTH, PLAYFIELD_HEIGHT
  };

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

    state->speccy->draw(state->speccy, &playfield_box); /* Conv: added */

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

    state->speccy->draw(state->speccy, &playfield_box); /* Conv: added */

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
         * -2016 is the byte delta that lands this pointer on the same
         * (D0, E0+32) target; 0xF820 (65536-2016) is only equivalent to that
         * under 16-bit modular register arithmetic, not real pointer
         * arithmetic, so it must not be added directly to DEscreen. */
        DEscreen -= 2016;
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
  static const zxbox_t playfield_box = { /* lower two-thirds of screen */
    0, 0, SCREEN_WIDTH, PLAYFIELD_HEIGHT
  };

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

  state->speccy->draw(state->speccy, &playfield_box); /* Conv: added */

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

  state->speccy->draw(state->speccy, &playfield_box); /* Conv: added */

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
