/* --------------------------------------------------------------------------
 *    Name: main.c
 * Purpose: Chase H.Q. RISC OS front-end
 * ----------------------------------------------------------------------- */

#include "ChaseHQ/GameOps.h"

/* Chase H.Q. has no saved games, so the file type is never used. It is a
 * placeholder from the user area and has not been allocated. */
static const zxgame_riscos_app_t app =
{
  "ChaseHQ",
  0xFFD,
  "chasehq",
  "ChaseHQ"
};

int main(int argc, char *argv[])
{
  return zxgame_riscos_main(argc, argv, &chq_game_ops, &app);
}
