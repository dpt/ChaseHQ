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

extern const u8 chatterblk_nancy_congratulates[6];
extern const u8 chatterblk_press_gear[3];

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK7DATA_H */
