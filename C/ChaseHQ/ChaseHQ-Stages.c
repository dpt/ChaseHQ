/**
 * ChaseHQ-Stages.c
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

#include "Data/ChaseHQ-Stage1Data.h"
#include "Data/ChaseHQ-Stage2Data.h"
#include "Data/ChaseHQ-Stage3Data.h"
#include "Data/ChaseHQ-Stage4Data.h"
#include "Data/ChaseHQ-Stage5Data.h"

#include "ChaseHQ-Stages.h"

const stage_t *stages[MAX_STAGES + 2] = {
  &stage1, /* [0]: pregame (wanted_stage_number=0) */
  &stage1, /* [1]: game stage 1 */
  &stage2, /* [2]: game stage 2 */
  &stage3, /* [3]: game stage 3 */
  &stage4, /* [4]: game stage 4 */
  &stage5, /* [5]: game stage 5 */
  &stage5, /* [6]: end-sequence reload (wanted_stage_number briefly hits 6) */
};
