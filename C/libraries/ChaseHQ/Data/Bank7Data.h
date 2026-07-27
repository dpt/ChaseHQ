/**
 * Bank7Data.h
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

#ifndef CHASEHQ_BANK7DATA_H
#define CHASEHQ_BANK7DATA_H

/* ----------------------------------------------------------------------- */

/* End-game montage shots: 64 bitmap rows (13 bytes each) followed by 8
 * attribute rows (13 bytes each), consumed as one contiguous blob by
 * draw_endshot ($E4A9, Bank7.c). Row/attribute counts are not exactly
 * 64*13+8*13 in every case -- sizes here are taken verbatim from the skool's
 * label boundaries, not recomputed from the nominal 104x64 dimensions. */
extern const u8 bitmap_endshot_1[936];
extern const u8 bitmap_endshot_2[936];
extern const u8 bitmap_endshot_3[936];
extern const u8 bitmap_endshot_4[936];

/* Handshake animation frames: 8-byte-wide rows only, no attribute data
 * (handshake's own routine_e3b7 supplies attributes via the fixed $59AC
 * fill). Row counts vary per frame (37/35/34/32) -- sizes taken verbatim
 * from skool label boundaries. */
extern const u8 bitmap_handshake_1[296];
extern const u8 bitmap_handshake_2[280];
extern const u8 bitmap_handshake_3[272];
extern const u8 bitmap_handshake_4[256];

/* $E3A5 handshake_frames: row-count + source bitmap per animation frame,
 * cycling 1-2-3-4-3-2 (es_handler_handshake_advance, Bank7.c). */
typedef struct {
  u8        rows;
  const u8 *image;
} handshake_frame_t;

extern const handshake_frame_t handshake_frames[6];

extern const u8 chatterblk_nancy_congratulates[6];
extern const u8 chatterblk_press_gear[3];

/* Bank 7's own copy of the 48K music engine's pattern/data tables, played by
 * es_play_music_48k et al (Bank7.c). Same (repeats, offset) / note-stream
 * format as CommonData.c's music_patterns/music_data, but a separate tune
 * and a separate table (relocated base $F53C, not $F0FE). */
extern const u8 es_music_patterns[23];
extern const u8 es_music_data[172];

/* Bank 7's own drum sample templates, played by es_playdrum_2/es_playdrum_1
 * (Bank7.c). Byte-for-byte identical to CommonData.c's drum2_template/
 * drum1_template for all but the final byte -- not aliases of those tables,
 * since that last byte differs (0x00 here vs 0xFF there), and bank 7 uses
 * shorter lengths (94/160 vs 108/252) throughout. */
extern const u8 es_drum_sample_2_template[94];
extern const u8 es_drum_sample_1_template[160];

/* End-screen script command bytes, dispatched by run_script's switch
 * (Bank7.c). Argument-encoding macros (ESCMD_CHATTER, ESCMD_IDLE, etc.) are
 * private to Bank7Data.c, where they build the es_script table below. */
#define ESCMD_CLEAR_DRAW_FRAME_VAL    (1) /* -> $E2D9 es_clear_then_draw_frame, runs immediately */
#define ESCMD_DRAW_WORD_VAL           (2) /* -> $E2DE es_handler_draw_word, runs immediately */
#define ESCMD_FADE_IN_A_VAL           (3) /* -> $E42E es_attribute_fade_in via rs_exit, reload 16 */
#define ESCMD_FADE_IN_B_VAL           (4) /* -> $E472 es_handler_glyph_fade_b via rs_exit, reload 16 */
#define ESCMD_HANDSHAKE_VAL           (5) /* -> $E3B7 es_handler_handshake via rs_exit, reload 16 */
#define ESCMD_FADE_IN_C_VAL           (6) /* -> $E46D es_handler_glyph_fade_c via rs_exit, reload 32 */
#define ESCMD_IDLE_VAL                (7) /* -> rs_exit, handler = no-op, reload = script byte */
#define ESCMD_RESET_HANDSHAKE_VAL     (8) /* -> rs_exit, sets $A172, handler = handshake, reload = script byte */
#define ESCMD_HANDSHAKE_AGAIN_VAL     (9) /* -> rs_exit, handler = handshake, reload = script byte */
#define ESCMD_DRAW_TEXT_NO_CLEAR_VAL (10) /* -> $E2F5 render_text_common, runs immediately, no backbuffer clear */
#define ESCMD_DRAW_TEXT_VAL          (11) /* -> $E2F0 es_handler_render_text, runs immediately, clears backbuffer first */
#define ESCMD_CHATTER_VAL            (12) /* -> $E2B2, runs immediately */
#define ESCMD_DRAW_SCORE_VAL         (13) /* -> $E256, runs immediately */

/* $E0FE-$E209: End-screen script bytecode (268 bytes). See its definition in
 * Bank7Data.c for the full format description. */
extern const u8 es_script[268];

/* Conv: skool $E251 "LD HL,$5E04 / JR $E20D" -- on an unrecognised command
 * byte the Z80 resets HL to the CHATTER(0x5C78) command three bytes back
 * and re-enters the loop rather than returning, which is what makes
 * "PRESS GEAR TO CONTINUE" blink forever instead of a one-shot draw. */
#define ES_SCRIPT_RESET_OFFSET (sizeof(es_script) - 6)

/* Conv: skool $E052 "LD HL,$5DE3 / LD ($A16D),HL" -- the first fire press
 * jumps the script program counter to the congratulations sequence at
 * $E1E3, which is 0xE5 bytes into the script block based at $E0FE. */
#define ES_SCRIPT_CONGRATS_OFFSET (0xE1E3 - 0xE0FE)

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK7DATA_H */
