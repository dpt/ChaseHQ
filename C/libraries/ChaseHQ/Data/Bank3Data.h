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
const u16 note_periods[96];

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
const tune_t tunes[4];

/* 128K bank 3: title-tune engine raw pattern-data region for tune 0 (title
 * screen), transcribed byte-exact from bank3.bin, $F241-$F600 (the byte
 * immediately preceding tune 1's own region). Each channel's block within
 * this region holds a 2-byte header pointer, its phrase-pointer table, and
 * its note/command byte stream, all as one contiguous run of real Z80
 * memory -- see advance_channel_phrase ($F1AE@bank3) for how the header and
 * table are walked. Addresses read from the header or phrase table are
 * resolved to a C pointer into this array via simple offset arithmetic from
 * $F241 (see resolve_phrase_addr in Bank3.c). */
const u8 title_tune0_data[1026];

/* As title_tune0_data, for tune 1 (perp-caught success jingle),
 * $F601-$F6DE -- covers channel 3's wraparound pattern prefix (156 bytes,
 * see tune_pattern_lens in start_tune) in full, the deepest of the three
 * channels' reach into this region. Tune 2 begins at $F666, inside this
 * range; tune 2 is not itself extracted. */
const u8 title_tune1_data[222];

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK3DATA_H */
