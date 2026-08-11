/**
 * LoadingScreen.h
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

#ifndef CHASEHQ_LOADINGSCREEN_H
#define CHASEHQ_LOADINGSCREEN_H

#include "C99/Types.h"
#include "ZXSpectrum/Pixels.h"
#include "ZXSpectrum/Spectrum.h"

/* ----------------------------------------------------------------------- */

/* The cassette-tape loading screen. Unlike every other graphics table in
 * Data/, this isn't code the game executes or draws itself: on real
 * hardware it's a raw 6912-byte block loaded straight into screen memory
 * ($4000-$5AFF) by the tape loader, before the BASIC loader even runs the
 * machine code game. Extracted from Speccy/Archives/chase.tap block 2
 * (the second data block, flag byte $FF, length 6912).
 */

extern const pixel_t loading_screen_bitmap[SCREEN_BITMAP_LENGTH];
extern const u8 loading_screen_attributes[SCREEN_ATTRIBUTES_LENGTH];

#endif /* CHASEHQ_LOADINGSCREEN_H */
