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
 * are raw Z80 addresses into pattern-data blocks that have not themselves
 * been extracted as C data yet -- see start_tune's Translation notes. */
const u8 tune_select_table[4 * 7];

/* 128K bank 3: title-tune engine pattern-command byte streams for tunes 0
 * (title screen) and 1 (perp-caught success jingle), transcribed byte-exact
 * from bank3.bin at the addresses reached by following tune_select_table's
 * pointers through their 2-byte header word (see start_tune's Translation
 * notes). Each array covers a fixed 120-row prefix of the real data -- not
 * proven to be the tune's full loop length, just a generous bound comfortably
 * covering BASL_JINGLE_FRAMES/ts_wait_loop's playback window. Tunes 2 and 3
 * are not extracted (unreachable from the code paths wired up so far). */
const u8 title_tune0_ch1_pattern[157];
const u8 title_tune0_ch2_pattern[160];
const u8 title_tune0_ch3_pattern[447];
const u8 title_tune1_ch1_pattern[190];
const u8 title_tune1_ch2_pattern[173];
const u8 title_tune1_ch3_pattern[156];

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK3DATA_H */
