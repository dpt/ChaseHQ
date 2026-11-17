/**
 * main.m
 *
 * This file is part of "Chase H.Q. in C".
 *
 * The recreated version is copyright (c) 2023-2026 David Thomas.
 */

#import "ChaseHQ/GameOps.h"

int main(int argc, char *argv[])
{
  return zxgame_macos_main(argc, argv, &chq_game_ops);
}
