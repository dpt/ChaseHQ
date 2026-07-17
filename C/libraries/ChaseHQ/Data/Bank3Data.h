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
 * The original game and design is copyright (c) 1988 Taito Corporation
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited
 * The recreated version is copyright (c) 2023-2026 David Thomas
 */

#ifndef CHASEHQ_BANK3DATA_H
#define CHASEHQ_BANK3DATA_H

/* ----------------------------------------------------------------------- */

const u8 title_screen_credits_text[56];
const u8 title_screen_overlay_text[47];

const u8 options_menu_text[366];

/* 128K bank 3: title-tune engine AY tone-period lookup table, $EFBC-$F07B,
 * 96 entries (2 bytes/note, little-endian), indexed by
 * compute_channel_ay_registers ($EE9E@bank3). $F07C onward is a different,
 * unrelated table (an indexed pointer table, see $EE5A@bank3) -- do not
 * extend this array into it. */
const u16 title_tune_note_periods[96];

/* 128K bank 3: title-tune engine tune-select table, $F225-$F240, 4 entries
 * (7 bytes/tune: 1 tempo byte + 3 x 2-byte little-endian pattern-data
 * pointers). Indexed by start_tune ($EB9E@bank3). The pattern-data pointers
 * are raw Z80 addresses, resolved into title_tune0_raw_data/
 * title_tune1_raw_data below by start_tune and advance_channel_phrase. */
const u8 tune_select_table[4 * 7];

/* 128K bank 3: title-tune engine raw pattern-data region for tune 0 (title
 * screen), transcribed byte-exact from bank3.bin, $F241-$F600 (the byte
 * immediately preceding tune 1's own region). Each channel's block within
 * this region holds a 2-byte header pointer, its phrase-pointer table, and
 * its note/command byte stream, all as one contiguous run of real Z80
 * memory -- see advance_channel_phrase ($F1AE@bank3) for how the header and
 * table are walked. Addresses read from the header or phrase table are
 * resolved to a C pointer into this array via simple offset arithmetic from
 * $F241 (see resolve_phrase_addr in Bank3.c). */
const u8 title_tune0_raw_data[960];

/* As title_tune0_raw_data, for tune 1 (perp-caught success jingle),
 * $F601-$F6DE -- covers channel 3's wraparound pattern prefix (156 bytes,
 * see tune_pattern_lens in start_tune) in full, the deepest of the three
 * channels' reach into this region. Tune 2 begins at $F666, inside this
 * range; tune 2 is not itself extracted. */
const u8 title_tune1_raw_data[222];

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK3DATA_H */
