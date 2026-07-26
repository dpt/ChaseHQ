/**
 * Stages.c
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

#include "Stage1Data.h"
#include "Stage2Data.h"
#include "Stage3Data.h"
#include "Stage4Data.h"
#include "Stage5Data.h"
#ifdef CHQ_ENABLE_TEST_STAGE
#include "Stage6Data.h"
#endif

#include "Stages.h"

/**
 * stages
 *
 * Conv: host-side index table mapping game stage number to its data
 * struct; the original Z80 code addresses each stage's tables directly
 * via self-modified pointers rather than a single array, so there is no
 * originating address. Built with the test stage (see Stage6Data.c)
 * inserted at index 5.
 */
#ifdef CHQ_ENABLE_TEST_STAGE
const stage_t *stages[7] = {
  &stage1, /* [0]: game stage 1 */
  &stage2, /* [1]: game stage 2 */
  &stage3, /* [2]: game stage 3 */
  &stage4, /* [3]: game stage 4 */
  &stage5, /* [4]: game stage 5 */
  &stage6, /* [5]: game stage 6 (Conv: additional for testing) */
  &stage5, /* [6]: end-sequence reload (wanted_stage_number briefly hits 6) */
};
#else
/**
 * stages
 *
 * Conv: as above, without the test stage.
 */
const stage_t *stages[6] = {
  &stage1, /* [0]: game stage 1 */
  &stage2, /* [1]: game stage 2 */
  &stage3, /* [2]: game stage 3 */
  &stage4, /* [3]: game stage 4 */
  &stage5, /* [4]: game stage 5 */
  &stage5, /* [5]: end-sequence reload (wanted_stage_number briefly hits 6) */
};
#endif
