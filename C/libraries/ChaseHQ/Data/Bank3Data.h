/**
* Bank3Data.h
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

#ifndef CHASEHQ_BANK3DATA_H
#define CHASEHQ_BANK3DATA_H

#include "C99/Types.h"

/* ----------------------------------------------------------------------- */

extern const u8 title_screen_credits_text[56];
extern const u8 title_screen_overlay_text[47];

#define TITLE_SCENE_COUNT          (5)
#define TITLE_SCENE_DATA_BASE (0xCCB7)
#define TITLE_GLYPH_COUNT        (115)

typedef struct title_glyph {
  u8        height_pairs; /* number of scanline-pairs to blit */
  u8        width_bytes;  /* glyph width in screen bytes (1-7) */
  const u8 *bitmap;      /* source pixel bytes, OR-blitted verbatim */
} title_glyph_t;

extern const u8 title_scene_data[1467];
extern const u16 title_scene_table_offset[TITLE_SCENE_COUNT];
extern const u8 title_speed_curve[36];
extern const u8 title_glyph_bitmaps[5948];
extern const title_glyph_t title_glyph_table[TITLE_GLYPH_COUNT];

/* Object animation script opcodes (title_object::opcode / script byte-code
 * in title_scene_data above). Bytes with the sign bit clear ($00-$7F) are
 * immediate step deltas, not opcodes -- see oss_op_immediate_step in
 * object_script_step's prologue (Bank3.c).
 *
 * Each opcode has a _VAL form (its raw byte value, used as a switch case
 * label in object_script_step) and a function-like macro of the same name
 * that emits the opcode byte followed by its operand bytes in on-disk
 * order, for use inside title_scene_data's initializer below. */
#define OSS_OP_SET_ROW_VAL        (0xC8) /* set screen-row byte, 1 operand */
#define OSS_OP_VELOCITY_VAL       (0xC9) /* constant velocity, 3 operands: x,y,wait */
#define OSS_OP_DECEL_X_VAL        (0xCA) /* decelerate X, 3 operands */
#define OSS_OP_DECEL_Y_VAL        (0xCB) /* decelerate Y, 3 operands */
#define OSS_OP_ACCEL_X_A_VAL      (0xCC) /* accelerate X variant a, 3 operands */
#define OSS_OP_ACCEL_X_C_VAL      (0xCD) /* accelerate X variant c, 3 operands */
#define OSS_OP_ACCEL_X_B_VAL      (0xCE) /* accelerate X variant b, 3 operands */
#define OSS_OP_WAIT_VAL           (0xCF) /* wait N frames, 1 operand */
#define OSS_OP_JUMP_POSITION_VAL  (0xD0) /* jump to absolute position, 2 operands */
#define OSS_OP_DEAD_VAL           (0xD1) /* unrecognized/dead value -- never explicitly
                                           * emitted as a case, falls to default in both
                                           * switches; see object_script_step's prologue */
#define OSS_OP_END_SCRIPT_VAL     (0xD2) /* end of script -- POP HL; RET stack unwind,
                                           * see object_script_step's own Conv note */

#define OSS_OP_SET_ROW(row)                     OSS_OP_SET_ROW_VAL, (row)
#define OSS_OP_VELOCITY(x_step, y_step, wait)   OSS_OP_VELOCITY_VAL, (x_step), (y_step), (wait)
#define OSS_OP_DECEL_X(y_step_seed, wait, x_step) OSS_OP_DECEL_X_VAL, (y_step_seed), (wait), (x_step)
#define OSS_OP_DECEL_Y(y_step_seed, wait, x_step) OSS_OP_DECEL_Y_VAL, (y_step_seed), (wait), (x_step)
#define OSS_OP_ACCEL_X_A(x_step_seed, wait, y_step) OSS_OP_ACCEL_X_A_VAL, (x_step_seed), (wait), (y_step)
#define OSS_OP_ACCEL_X_C(x_step_seed, wait, y_step) OSS_OP_ACCEL_X_C_VAL, (x_step_seed), (wait), (y_step)
#define OSS_OP_ACCEL_X_B(x_step_seed, wait, y_step) OSS_OP_ACCEL_X_B_VAL, (x_step_seed), (wait), (y_step)
#define OSS_OP_WAIT(wait)                       OSS_OP_WAIT_VAL, (wait)
#define OSS_OP_JUMP_POSITION(x, y)              OSS_OP_JUMP_POSITION_VAL, (x), (y)
#define OSS_OP_DEAD()                           OSS_OP_DEAD_VAL
#define OSS_OP_END_SCRIPT()                     OSS_OP_END_SCRIPT_VAL

/* Index names for note_periods[] below, in scientific pitch notation
 * (A4 = 440 Hz, C4 = middle C), derived from each entry's AY tone period. */
enum note_index {
  NOTE_AS0, NOTE_B0,
  NOTE_C1, NOTE_CS1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_F1, NOTE_FS1, NOTE_G1, NOTE_GS1, NOTE_A1, NOTE_AS1, NOTE_B1,
  NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2,
  NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3,
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
  NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
  NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
  NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7,
  NOTE_C8, NOTE_CS8, NOTE_D8, NOTE_DS8, NOTE_E8, NOTE_F8, NOTE_FS8, NOTE_G8, NOTE_GS8, NOTE_A8
};

/* 128K bank 3: title-tune engine AY tone-period lookup table, $EFBC-$F07B,
 * 96 entries (2 bytes/note, little-endian), indexed by
 * compute_channel_ay_registers ($EE9E@bank3). $F07C onward is a different,
 * unrelated table (an indexed pointer table, see $EE5A@bank3) -- do not
 * extend this array into it. Indices name-checked against enum note_index. */
extern const u16 note_periods[96];

/* One tune's entry in the tune-select table below: a tempo/speed byte plus
 * the raw Z80 address of each of the 3 channels' pattern-data blocks. */
typedef struct tune {
  u8  tempo;                   /* tune tempo/speed byte */
  u16 channel_pattern_addr[3]; /* raw Z80 address of each channel's pattern-data block */
} tune_t;

/* 128K bank 3: title-tune engine tune-select table, $F225-$F240, 4 entries,
 * one per tune. Indexed by start_tune ($EB9E@bank3). The pattern-data
 * pointers are raw Z80 addresses, resolved into title_tune0_data/
 * title_tune1_data below by start_tune and advance_channel_phrase. */
extern const tune_t tunes[4];

/* 128K bank 3: title-tune engine raw pattern-data region for tune 0 (title
 * screen), transcribed byte-exact from bank3.bin, $F241-$F600 (the byte
 * immediately preceding tune 1's own region). Each channel's block within
 * this region holds a 2-byte header pointer, its phrase-pointer table, and
 * its note/command byte stream, all as one contiguous run of real Z80
 * memory -- see advance_channel_phrase ($F1AE@bank3) for how the header and
 * table are walked. Addresses read from the header or phrase table are
 * resolved to a C pointer into this array via simple offset arithmetic from
 * $F241 (see resolve_phrase_addr in Bank3.c). */
extern const u8 title_tune0_data[1026];

/* As title_tune0_data, for tune 1 (perp-caught success jingle),
 * $F601-$F6DE -- covers channel 3's wraparound pattern prefix (156 bytes,
 * see tune_pattern_lens in start_tune) in full, the deepest of the three
 * channels' reach into this region. Tune 2 begins at $F666, inside this
 * range; tune 2 is not itself extracted. */
extern const u8 title_tune1_data[222];

extern const u8 options_menu_text[366];

/* 128K bank 3: two fixed 1-bit PCM "digitised sample" tables played by
 * play_sample_row via the drum-sample dispatch in sfx_music_service. Each
 * byte is one playback row of 8 bits, rotated out with RLC so playback
 * mutates the table in place -- state keeps a mutable per-game copy, these
 * are the pristine templates. Same underlying sample content as CommonData.c's
 * drum1_template/drum2_template and Bank7Data.h's es_drum1/es_drum2 (this
 * game's PCM drum/noise assets are duplicated, at slightly different
 * lengths, across every bank that plays them) -- transcribed separately here
 * because bank 3's copies are shorter than either. */
extern const u8 sfx_sample_1_template[104]; /* $F8F2, played with D=$68 rows */
extern const u8 sfx_sample_2_template[224]; /* $F95A, played with D=$E0 rows */

/* 128K bank 3: drum-sample cue-script/trigger-table data, $FA75-$FB98,
 * transcribed byte-exact from bank3.bin. Covers the per-tune cue-script
 * pointer table ($FA75-$FA7E), the 5 tunes' cue-script byte-code
 * ($FA7F-$FAA3), and the per-drum-ID trigger table ($FAA4-$FB98) --
 * see stst_load_sfx_script/ssa_read_opcode in Bank3.c for how this is
 * walked, and resolve_drum_script_addr for how raw Z80 addresses within it
 * are resolved to C pointers. */
extern const u8 drum_cue_script_data[292];

#define DRUM_CUE_SCRIPT_DATA_BASE (0xFA75)

/* 128K bank 3: one row of the high-score table, $C408-$C42A stride (the
 * fields that actually move when a row is shifted -- see
 * insert_high_score_entry in Bank3.c). The 5-char rank suffix ("1ST  " etc)
 * is fixed to its screen position, never shifts, and is stored separately in
 * high_score_rank_suffixes below rather than per-row. */
typedef struct high_score_row {
  u8 score[8];      /* $C408: ASCII score digits, most significant first */
  u8 stage_code[3]; /* $C414: e.g. " 1 ", "ALL" */
  u8 retry_digit;   /* $C41C: ASCII '1'-'3' (or higher in test-mode data) */
  u8 name[3];        /* $C41F: 3-character initials */
} high_score_row_t;

#define HIGH_SCORE_TABLE_ROWS (10)

/* 128K bank 3: preset high-score table rows, $C408-$C552 (33-byte stride in
 * the original; only the fields that move are transcribed here -- see
 * high_score_row_t above). Row 0 = 1st place .. row 9 = 10th place. Row 0's
 * initials "JOB" are John O'Brien, this game's programmer. */
extern const high_score_row_t high_score_table_template[HIGH_SCORE_TABLE_ROWS];

/* 128K bank 3: $C403-$C52C, the 10 static rank-suffix strings printed beside
 * each high-score row (fixed to screen position, never shifted -- see
 * high_score_row_t's own comment). Not consumed yet: the high-score screen's
 * rendering is not translated (see insert_high_score_entry's Conv note in
 * Bank3.c); kept here for when it is. */
extern const u8 high_score_rank_suffixes[HIGH_SCORE_TABLE_ROWS][5];

/* 128K bank 3: $C567-$C57E, the 6-entry stage-code table read by
 * insert_high_score_entry ($C09F), indexed by wanted_stage_number-1 (state
 * fields are 1-6; the Z80 table is addressed from a base 3 bytes before its
 * first real entry so that a raw 1-based multiply lands correctly). */
extern const u8 high_score_stage_codes[6][3];

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK3DATA_H */
