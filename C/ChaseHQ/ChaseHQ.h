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
 * The original game and design is copyright (c) 1988 Taito Corporation
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited
 * The recreated version is copyright (c) 2023-2026 David Thomas
 */

#ifndef CHASEHQ_H
#define CHASEHQ_H

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
 * Prepare the game screen.
 */
CHQ_API void chq_setup(chqstate_t *state);

// /**
//  * Run the game menu.
//  *
//  * Call this repeatedly until it returns > 0.
//  *
//  * \return > 0 when it's time to continue on to chq_setup2.
//  */
// chq_API int chq_menu(chqstate_t *state);
//
// /**
//  * Prepare the game proper.
//  */
// chq_API void chq_setup2(chqstate_t *state);

/**
 * Invoke the game instance.
 *
 * Call this repeatedly.
 */
CHQ_API void chq_main(chqstate_t *state);

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_H */
