/**
 * ChaseHQ.h
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

#ifndef CHASEHQ_H
#define CHASEHQ_H

#include "C99/Types.h"
#include "ZXSpectrum/Spectrum.h"

/* ----------------------------------------------------------------------- */

#define CHQ_API

/* ----------------------------------------------------------------------- */

/* Exports go here... */

/**
 * Holds the current state of the game.
 */
typedef struct chqstate chqstate_t;

/**
 * Create a game instance.
 */
CHQ_API chqstate_t *chq_create(zxspectrum_t *speccy);

/**
 * Destroy a game instance.
 */
CHQ_API void chq_destroy(chqstate_t *state);

/**
 * Signal the game to stop at its next drive_attract_demo call.
 * Call this before SDL_WaitThread to ensure the game thread exits cleanly.
 */
CHQ_API void chq_stop(chqstate_t *state);

/**
 * Prepare the game screen.
 *
 * \param mode_128k Non-zero to start the game in 128K mode.
 */
CHQ_API void chq_start(chqstate_t *state, int mode_128k);

/**
 * Fetch the raw $F000 backbuffer, for debug display.
 *
 * The buffer is monochrome, one bit per pixel, packed 8 pixels per byte
 * left-to-right, *width / 8 bytes per row, top-to-bottom.
 *
 * \param state  Game instance.
 * \param width  (out) Backbuffer width in pixels.
 * \param height (out) Backbuffer height in pixels.
 * \return Pointer to the backbuffer. Owned by \p state; do not free.
 */
CHQ_API const u8 *chq_get_backbuffer(chqstate_t *state, int *width, int *height);

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_H */
