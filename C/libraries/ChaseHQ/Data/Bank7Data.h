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
const u8 bitmap_endshot_1[936];
const u8 bitmap_endshot_2[936];
const u8 bitmap_endshot_3[944];
const u8 bitmap_endshot_4[928];

/* Handshake animation frames: 8-byte-wide rows only, no attribute data
 * (handshake's own routine_e3b7 supplies attributes via the fixed $59AC
 * fill). Row counts vary per frame (37/35/34/32) -- sizes taken verbatim
 * from skool label boundaries. */
const u8 bitmap_handshake_1[296];
const u8 bitmap_handshake_2[280];
const u8 bitmap_handshake_3[272];
const u8 bitmap_handshake_4[256];

/* $E06E-$E0FD: end-of-game congratulations text block, targeted by the sole
 * live ESCMD_CALL_WORD script entry (script_data's first byte, Bank7.c).
 *
 * Conv: this is NOT a standard {CHATTERCHR, CHATTERSTR, CHATTERCMD}
 * chatterblk -- byte 2 ($3F = 63) would be read as a CHATTERSTR index by
 * pc_chatter_message and immediately fail its bounds assertion
 * (CHATTERSTR__LIMIT is 36), or read out of bounds in a release build. See
 * es_handler_call_word in Bank7.c for how this is guarded. Included here
 * verbatim for documentation/fidelity only.
 *
 * Layout: 3 header bytes, an 11-byte pointer table (skool: "unproven,
 * confirmed dead end"), then four ASCII text strings, each terminated by
 * its last character having bit 7 set. */
const u8 data_e06e[144];

#define DATA_E06E_TEXT_1_OFFSET  (14) /* "I WOULD LIKE TO CONGRATULATE YOU FOR YOUR" */
#define DATA_E06E_TEXT_2_OFFSET  (55) /* "HEROISM AND BRAVERY. YOU WILL BE REMEMBERED" */
#define DATA_E06E_TEXT_3_OFFSET  (98) /* "FOR A LONG TIME TO COME." */
#define DATA_E06E_TEXT_4_OFFSET (122) /* "PRESS GEAR TO CONTINUE" */

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK7DATA_H */
