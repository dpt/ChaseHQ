/**
 * Bank3State.h
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
 *
 * -----------------------------------------------------------------------
 *
 * Full definition of chqstate's opaque bank3 pointer (State.h only forward
 * declares struct chq_bank3_state). Private to the 128K bank 3 title-screen
 * and title-tune engine -- only Bank3.c includes this header, so no other
 * translation unit can see or touch these fields directly.
 */

#ifndef CHASEHQ_BANK3STATE_H
#define CHASEHQ_BANK3STATE_H

#include "C99/Types.h"

#include "State.h"

/* ----------------------------------------------------------------------- */

/* 128K bank 3: one row of the high-score table, $C408-$C42A stride (the
 * fields that actually move when a row is shifted -- see
 * insert_high_score_entry in Bank3.c). The 5-char rank suffix ("1ST  " etc)
 * is fixed to its screen position, never shifts, and is stored separately in
 * high_score_rank_suffixes (Bank3.c) rather than per-row. */
typedef struct high_score_row
{
  u8 score[8];      /* $C408: ASCII score digits, most significant first */
  u8 stage_code[3]; /* $C414: e.g. " 1 ", "ALL" */
  u8 retry_digit;   /* $C41C: ASCII '1'-'3' (or higher in test-mode data) */
  u8 name[3];       /* $C41F: 3-character initials */
} high_score_row_t;

#define HIGH_SCORE_TABLE_ROWS (10)

/**
 * One 37-byte channel-tracker record used by the 128K bank-3 title-tune
 * engine ($EC01/$EC26/$EC4B). Offsets read/written by
 * compute_channel_ay_registers ($EE9E), advance_channel_pattern ($EDD6) and
 * start_tune ($EB9E) are modelled, along with the phrase-pointer-table
 * fields read/written by advance_channel_phrase ($F1AE, reached via pattern
 * command 0x87) -- see that function's prologue for the table format.
 */
typedef struct title_tune_channel
{
  u8        status;                 // +$00 note/status; bit0 toggled every call, bit1 set by pcmd_set_status_bit1, bit2 slide active, bit3 slide direction/upkeep gate, bit5 envelope active, bits 3&7 set by pcmd_set_status_bits_3_7
  const u8 *pattern_ptr;            // +$01/+$02 current read position in the pattern-command byte stream; initialised by start_tune from the first 2 bytes of the pattern-data block that pattern_data_ptr points to (an "envelope-pointer header")
  const u8 *pattern_data_ptr;       // +$03/+$04 raw pattern-data block pointer for this channel, read from the tune-select table by start_tune; base address for phrase_table_offset and the header re-read on phrase-table exhaustion (advance_channel_phrase)
  const u8 *pattern_base;           // Conv: start of the extracted pattern_ptr array; not a Z80 field. Lets advance_channel_pattern wrap pattern_ptr back to the start once it runs off the end of the finite extracted prefix, since the real Z80 data (and its true loop point) is not fully transcribed into C
  u16       pattern_len;            // Conv: byte length of the array pattern_base points to; paired with pattern_base for the same reason
  u16       phrase_table_offset;    // +$05/+$06 byte offset from pattern_data_ptr to the current phrase-pointer-table entry; starts at 2 (immediately past the 2-byte header) and advances by 2 or 3 per table word (advance_channel_phrase)
  u16       slide_accum;            // +$07/+$08 accumulated portamento/slide value
  const u8 *pitch_offset_default;   // +$09/+$0A default/loop-start pitch-offset sequence pointer
  const u8 *pitch_offset_cur;       // +$0B/+$0C current pitch-offset sequence pointer
  s8        slide_step;             // +$0D signed per-tick portamento step
  u8        slide_countdown;        // +$0E portamento reload countdown
  u8        envelope_speed;         // +$0F envelope-step reload value
  u8        row_wait;               // +$10 per-row wait countdown; also doubles as the channel enable flag (start_tune sets it to 1)
  u8        row_wait_reload;        // +$11 reload value for row_wait, set by the row-duration pattern command
  u8        note_index;             // +$12 current note index (post-transpose)
  u8        volume;                 // +$13 volume/envelope amplitude; returned to caller
  const u8 *envelope_shape_default; // +$14/$15 default/base envelope-shape table pointer, reloaded into envelope_shape_ptr on every note
  const u8 *envelope_shape_ptr;     // +$16/+$17 envelope shape table pointer
  u8        envelope_amplitude;     // +$18 current envelope amplitude
  u8        envelope_step_counter;  // +$19 envelope-step counter; 0 = due for reload
  u8        vibrato_depth;          // +$1A vibrato depth * 2
  u8        vibrato_increment;      // +$1B vibrato per-tick increment
  u8        vibrato_phase;          // +$1C vibrato triangle-wave phase counter
  u8        flags;                  // +$1D bit5 vibrato direction, bit6 vibrato enable, bit7 vibrato update gate
  u8        slide_update_flag;      // +$1E bit0 gates whether a new note is echoed to title_music.shared_note_value; set/cleared by the mixer-bit pattern commands
  u8        mute_pending;           // +$1F bit7 = one-shot mute-transition gate
  u8        transpose;              // +$20 added to each raw note value read from the pattern stream before storing to note_index; also reset to 0, or set from an inline phrase-table override, by advance_channel_phrase
  u8        phrase_repeat_count;    // +$21 decrementing repeat count for the phrase currently held in phrase_ptr; 0 means due for a new phrase-table lookup (advance_channel_phrase); reset to 0 by start_tune
  const u8 *phrase_ptr;             // +$22/+$23 pattern-stream cursor for the phrase most recently activated by a repeating (marker==2) phrase-table entry; reused while phrase_repeat_count is still counting down (advance_channel_phrase)
  u8        mixer_mask;             // +$24 mask applied when merging into the shared mixer cache
}
title_tune_channel_t;

/**
 * One 9-byte animated-object record used by the 128K bank-3 title screen
 * ($BB00-$BB4F, 9 records). Populated from a scene table by
 * title_screen_driver, drawn each frame by ts_animate_frame, and advanced
 * by object_script_step ($C705).
 */
struct title_object
{
  u8        opcode; // +$00 active movement-mode opcode, or 0 (idle: fetch next script opcode)
  u8        wait;   // +$01 "wait N frames" countdown, also reused as the decel/accel countdown
  s8        x_step; // +$02 X velocity/step
  s8        y_step; // +$03 Y velocity/step
  const u8 *script; // +$04/+$05 script byte-code cursor
  u8        row;    // +$06 screen row/height byte consumed by the blitters
  u8        x;      // +$07 current X screen position
  u8        y;      // +$08 current Y screen position
};

/* ----------------------------------------------------------------------- */

/**
 * State private to the 128K bank 3 title-screen / title-tune engine.
 * chqstate_t only holds a pointer to this (see State.h); the fields below
 * are reachable only from within Bank3.c.
 */
struct chq_bank3_state
{
  /* $EC01-$F224 (128K bank 3 only) -- title-screen tune engine channel
   * trackers and self-modifying scratch bytes. Same numeric address range as
   * the 48K `music` state but a different bank/context; kept as a distinct
   * struct rather than aliased onto it (see compute_channel_ay_registers,
   * $EE9E@bank3).
   */
  struct
  {
    // $EC01/$EC26/$EC4B
    struct title_tune_channel channel[3];

    /* $EC70 (128K bank 3): per-tick tempo countdown, decremented by
     * ts_music_service ($EC71) each call; the 3 channels' patterns only
     * advance one row when it reaches zero, after which it reloads from
     * tune_tempo ($EC9A) -- $EC99 "LD A,$01" is self-modifying, its operand
     * byte is tune_tempo itself. Set to 1 by start_tune ($EBF9-$EBFA) so the
     * very first service call after a tune starts always advances.
     */
    u8        tempo_counter;

    /* $EC79 (SM): operand of "LD A,$00" at $EC78 in ts_music_service
     * ($EC71@bank3); written by advance_channel_pattern
     * ($EDF6@bank3); read by compute_channel_ay_registers.
     * Purpose not established elsewhere in bank 3 (see skool comment at $EE9E).
     */
    u8        shared_note_value;

    /* $EC9A (SM): tune tempo/speed byte, and also the operand of the
     * self-modifying "LD A,$01" at $EC99 in ts_music_service -- the reload
     * value for tempo_counter. Written by start_tune ($EBB3) and by
     * advance_channel_pattern's decode_pattern_command cascade ($EE71); read
     * by ts_music_service via that self-modified instruction.
     */
    u8        tune_tempo;

    /* $ECC6 (SM): operand of "LD A,$00" at $ECC5 in ts_music_service
     * ($EC71@bank3). Written within compute_channel_ay_registers (forced to
     * a computed value or to $41 on the mute transition); also written
     * directly to 0 by ts_music_service itself at $EC7A on entry. Read back
     * by ts_music_service at $ECC5 -- the "LD A,$00" instruction there
     * executes with whatever value was last patched into its own operand
     * byte, so it is not actually loading a literal 0 once
     * compute_channel_ay_registers has run -- and stored into the
     * noise_pitch AY register cache ($EFB5). Purpose beyond that plumbing
     * not established.
     */
    u8        driver_internal_flag;

    /* $EED1 (128K bank 3): driver-internal flag. Write-only, set by
     * advance_channel_pattern's pcmd_set_driver_flag handler ($ED87) from a
     * pattern-stream operand byte; cleared to 0 by start_tune ($EBF6, not yet
     * translated); never read anywhere in bank 3. Purpose not established.
     */
    u8        pattern_driver_flag;

    /* $EF7A (SM): operand of "LD A,$00" at $EF79 in
     * compute_channel_ay_registers ($EE9E@bank3), phase 5. Merged into by
     * advance_channel_pattern's mixer-bit pattern-command handlers
     * ($ED36/$ED4B/$ED5F) using the same replace-bits-under-mask idiom as
     * the $EFB6 mixer cache. Read back every frame at $EF79 -- the
     * "LD A,$00" instruction there executes with whatever value was last
     * patched into its own operand byte, so on any frame where the JR NZ at
     * $EF7B is taken (skipping the $EC79-derived $07 path), the mixer merge
     * uses this self-modified value rather than a literal 0. Confirmed
     * against a genuine ChaseHQ.ay dump (SlopAY project corpus): the
     * pristine snapshot's static operand byte is 0x00 (the pre-pattern-data
     * startup default), but the real tune patches it via the pattern
     * commands above, driving the AY mixer's noise-enable bits.
     */
    u8        pending_mixer_bits;

    /* $F223 (128K bank 3): tune-active flag. Tested by ts_music_service
     * ($EC71/$ECCA) to decide whether to advance
     * patterns / recompute and flush AY registers at all; also tested
     * directly by the title-screen driver at $C621. Armed to 1 by start_tune
     * ($EBFD) once the 3 channel-tracker records are initialised; cleared to
     * 0 both by start_tune's own entry (so a tune restart is briefly
     * inactive while re-initialising) and by the stop routine at $ED0B (not
     * yet translated).
     */
    u8        tune_active;

    /* $F224 (128K bank 3): cleared to 0 alongside tune_active by start_tune
     * ($EBA1, "its companion byte"); never read anywhere in bank 3. Purpose
     * not established beyond being cleared in lockstep with tune_active.
     */
    u8        tune_active_companion;
  } title_music;

  /* $BB00-$BB4F (128K bank 3 only): the 9 animated-object records populated
   * from the chosen scene table by title_screen_driver, drawn each frame by
   * ts_animate_frame, and advanced by object_script_step ($C705).
   */
  struct title_object title_objects[9];

  /* $C5A2 (SM, 128K bank 3 only): title-screen scene selector, rotated and
   * tested bit-by-bit by title_screen_driver to pick one of the 5 scene
   * tables each time the title screen restarts.
   */
  u8                 title_animation;

  /* $EFAF-$EFBA (128K bank 3 only) -- per-frame AY register cache for the
   * title-tune engine, refreshed by compute_channel_ay_registers ($EE9E) and
   * flushed to the AY chip by ts_music_service ($EC71) via
   * write_title_ay_registers.
   * Separate from the in-game AY block at $A213 (chqstate_t::ay_regs, same
   * numeric address range, different bank/context) -- do not alias the two.
   * Field layout is ay_register_cache_t (State.h) -- read-modify-written by
   * compute_channel_ay_registers.
   */
  ay_register_cache_t title_ay_regs;

  /* $FD97-$FD9B (128K bank 3): print_character scratch record built by
   * read_new_key_definition ($FF2C) each time a control's key name is
   * drawn. Not fixed/static data -- same role as messages_key_string for the
   * 48K equivalent (define_a_key).
   */
  u8        options_key_string[5];

  /* $FFF7-$FFFE (128K bank 3): live scan-key-code buffer for the currently
   * active control scheme. Not fixed/static data -- installed from the
   * Sinclair/Cursor joystick key lists by options_menu_driver ($FBDC), or
   * written key-by-key by read_new_key_definition ($FF2C).
   * Layout: [0..4] = gear/accelerate/brake/left/right (joystick-mappable),
   * [5..7] = quit/pause/turbo (keyboard-only).
   */
  u8        control_keys[8];

  /* $F836-$FA71 (128K bank 3 only): the digitised drum-sample subsystem
   * driven by sfx_music_service ($F82F) and armed by
   * start_tune_and_sfx_table's drum-sample script reader ($F7D6-$F82C).
   * See sfx_music_service's prologue in Bank3.c for the slot1/slot2/tail
   * state machine this drives.
   */
  struct
  {
    /* $F7F5: sfx_script_advance's own re-entry countdown -- throttles how
     * often a fresh script opcode byte is read.
     */
    u8        script_delay;

    /* $F7FC: drum-sample script byte-code cursor, advanced by
     * sfx_script_advance ($F7F4/$F7FE) each time script_delay reaches 0.
     */
    const u8 *script_ptr;

    /* $F837: drum-sample slot 1 busy flag (0 = idle, 1 = armed/busy). */
    u8        slot1_busy;

    /* $F842: slot 1 countdown/selector byte -- doubles as the countdown
     * ticked once per frame while busy, and as the raw selector byte most
     * recently read from the sample-selector stream.
     */
    u8        slot1_countdown;

    /* $F84E: self-modified operand of the "LD A,$00" at sfx1_reenter_stream
     * ($F84D) -- load_drum_op/titlescr_drum_advance patch it to the current
     * trigger-table entry's selector byte, and titlescr_music reloads
     * slot1_countdown from it every time the stream re-enters after a
     * countdown expiry. Despite the name, it IS read (indirectly, via the
     * self-modified instruction) -- it is the per-entry pacing delay.
     */
    u8        slot1_selector_dup;

    /* $F895: drum-sample slot 2 busy/countdown flag, armed alongside
     * slot1_countdown when a stream entry's bit 7 is set.
     */
    u8        slot2_busy;

    /* $F8A2: 1-bit sample playback active flag, tested by sfx_music_service's
     * tail to decide whether to resume play_sample_row.
     */
    u8        sample_active;

    /* $F853: current read position in the sample-selector byte stream. */
    const u8 *stream_ptr;

    /* $F85E: reload source for stream_ptr, set by stst_load_sfx_script/
     * sfx_script_advance each time a new drum-sample entry is read from
     * $FAA4.
     */
    const u8 *stream_reload_ptr;

    /* $F8CE: self-modified operand of the "LD B,$08" at $F8CD
     * (play_sample_row) -- play_fixed_sample_start patches it with the
     * dispatch byte's pitch/rate parameter, and play_sample_row reloads its
     * row-bit-count from it every row. The real per-row iteration count, not
     * a dead write.
     */
    u8        sample_pitch_param;

    /* $F8CC (shadow HL'/D', banked by EXX): mid-sample resume position and
     * row count, saved by play_sample_row when it yields back to
     * titlescr_music after a real interrupt period's worth of bit-banging,
     * and read back by titlescr_music's tail to continue the sample on the
     * next call. Only meaningful while sample_active is 1.
     */
    u8       *sample_resume_ptr;
    u8        sample_resume_rows;

    /* $F8F2/$F95A (Bank3.c: drum_sample_1_template/drum_sample_2_template):
     * mutable per-game copies -- play_sample_row rotates each byte in place
     * with RLC as it plays, so these cannot be the read-only template tables
     * directly (same reasoning as chqstate_t::music.drum1/music.drum2).
     */
    u8        sample1[104];
    u8        sample2[224];

    /* $FA72-$FA74: drum noise burst generator's self-modifying
     * phase-counter/accumulator state, advanced every call by
     * play_drum_noise_burst ($FA3A).
     */
    u8        noise_phase;
    u8        noise_accum;
    u8        noise_rotate;
  } drums;

  /* $C55F (128K bank 3): formatted ASCII score-digit buffer, written by
   * check_high_score from score_bcd and read back by insert_high_score_entry
   * when writing a new row.
   */
  u8 high_score_digits[8];

  /* $C408-$C552 (128K bank 3): the mutable per-game high-score table, one
   * entry per rank (index 0 = 1st place .. index 9 = 10th place). Seeded
   * from high_score_table_template (Bank3.c) by bank3_state_create and
   * updated in place by insert_high_score_entry.
   */
  high_score_row_t high_score_table[HIGH_SCORE_TABLE_ROWS];

  /* Name-entry screen state ($C0EC-$C3AC), driven by insert_high_score_entry. */
  struct
  {
    /* Row being written into (Conv: added, no Z80 field -- passed through
     * from insert_high_score_entry's row parameter, needed by
     * hiscore_finalise and scroll_score_rows once the row-scroll loop no
     * longer has it on the C call stack the way the Z80 keeps it banked via
     * EX AF,AF'). */
    u8 row;

    /* $C593/$C594: pointer into the row's name[] field for the letter
     * currently being selected -- Conv: modelled as an index (0-2) into
     * high_score_table[row].name rather than a raw pointer, since that
     * array is the only thing the Z80 pointer ever points at. */
    u8 char_index;

    /* $C593-pointee: current glyph code the player is cycling through for
     * this letter. $40 ('@') is the blank/"." marker; $41-$5A ("A"-"Z") are
     * the letters themselves -- this is a plain ASCII byte, not a lookup
     * index. */
    u8 letter_code;

    /* $C599: debounce flag -- set once FIRE has confirmed the current
     * letter, cleared on the next non-confirm frame, so a held-down FIRE
     * key does not repeatedly confirm. */
    u8 fire_locked;

    /* $C59A: first blink-phase toggle, flipped each time the 12-frame blink
     * timer reloads; selects attribute $46/$00 on the underline cursor
     * cell. */
    u8 flash_phase_a;

    /* $C59B: second blink-phase toggle, flipped on the alternate half of
     * the cycle from flash_phase_a; selects which of the two cursor-cell
     * highlight colours ($00/$42) is written back. */
    u8 flash_phase_b;

    /* $C58D: alternated each frame by redraw_score_name/redraw_name_frame;
     * selects between drawing (rsn_char_loop) and erasing (rsn_char_loop2)
     * a row's name text, giving the row's letters their own independent
     * blink. */
    u8 draw_erase_toggle;

    /* $C595: 12-frame countdown reloaded by name_entry_input each time it
     * hits zero; paces the cursor blink and the 20-position selector-cell
     * cycle. */
    u8 blink_timer;

    /* $C596: 0-19 offset of the currently highlighted cell within the
     * 20-cell selector row; wraps to 0 (and advances cursor_addr) once it
     * would reach 20. Drives the "ENTER YOUR INITIALS" chase highlight at
     * $5967+offset (see name_entry_dispatch) as well as the idle-timeout
     * below. */
    u8 blink_offset;

    /* $C597 low byte only: climbs by one each time blink_offset wraps, from
     * its template-copied start (10) towards 23 -- 13 row-wraps of the
     * 20-cell selector, i.e. 3120 frames (~62s at 50Hz) of no player input,
     * at which point name_entry_dispatch force-finalises the current letter
     * via hiscore_finalise, matching the real hardware's idle safety net.
     * Also the "BEST OFFICERS" highlight column (see
     * MARQUEE_ROW_ATTR_H/HISCORE_CURSOR_ADDR_INIT in Bank3.c). Conv: the
     * real $C597 is a full screen/attribute address pair advanced by one
     * row each wrap; only the low-byte progression is modelled here since
     * both marquees are painted directly from this byte, not via the
     * pointer. */
    u8 cursor_addr;

    /* Conv: stands in for the self-modified $C2F2 RET patch (NOP -> RET)
     * that name_entry_input writes once all 3 letters are confirmed.
     * ihe_flash_loop and scroll_score_rows both key off this flag instead
     * of relying on a literal self-modifying return. */
    u8 complete;

    /* $C13D: self-modified operand of ihe_flash_loop's "LD A,$A0 / DEC A /
     * LD ($C13D),A" countdown. The skool's own static-disassembly comment
     * calls this "always recomputes to a constant $9F" -- true only of the
     * literal bytes on a single read; because the instruction rewrites its
     * own operand each pass, it actually counts down from $A0 (160) once,
     * over 160 frames. While nonzero, ihe_flash_loop calls scroll_score_rows
     * every frame (the row scroll-in intro); once it reaches zero, control
     * falls through to ihe_entry_loop, which never calls scroll_score_rows
     * again -- this is what stops the row scroll for the rest of name
     * entry. Reset to 160 each session by name_entry_setup_screen. */
    u8 intro_timer;

    /* $C401-$C528 (31-byte stride): per-row screen-address pair used by
     * scroll_score_rows to animate each row's name field scrolling up into
     * view. Seeded from name_entry_row_offsets (Bank3.c) at the start of
     * each name-entry session. Conv: raw Z80 address pairs (E, D), some
     * values transiently below $4000 (not yet scrolled into the visible
     * screen) -- only ADDRTOSCREEN'd once a row's address has scrolled into
     * a valid on-screen window (see scroll_score_rows's own Conv note). */
    u8 row_addr[HIGH_SCORE_TABLE_ROWS][2];
  }
  hiscore;
};

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK3STATE_H */
