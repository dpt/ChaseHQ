// Data.h
//
// Chase H.Q. code model
//
// by dpt

// vim: ts=8 sts=2 sw=2 et

#ifndef DATA_H
#define DATA_H

#include <stdint.h>

#include "ChaseHQ-State.h"
#include "ChaseHQ.h"

const uint8_t stage1[16 * 512];

const uint8_t bitmap_turbospin[TURBOWIDTH / 8 * 2 * TURBOHEIGHT * TURBOFRAMES];
const uint8_t bitmap_faces[FACEBYTES * NFACES];

const uint8_t chatterblk_start_stage[4];
const uint8_t chatterblk_tony_giddy_up[3];
const uint8_t chatterblk_tony_hold_on[3];

const char *chatter_strings[CHATTERSTR__LIMIT];

const uint8_t attract_messages[37];
const uint8_t credits_messages[84];
const uint8_t copyright_messages[92];
const uint8_t score_messages_template[90];
const uint8_t sighting_message[36];
const uint8_t pull_over_message[30];
const uint8_t game_over_message[20];
const uint8_t time_up_message[20];
const uint8_t continue_messages[119];

const uint8_t chatterblk_pilot_turn_left[6];
const uint8_t chatterblk_pilot_turn_right[6];
const uint8_t chatterblk_heroes_acknowledge[4];
const uint8_t chatterblk_tony_loud_clear[3];
const uint8_t chatterblk_raymond_roger[3];
const uint8_t chatterblk_tony_gotcha[3];
const uint8_t chatterblk_raymond_wrong_way[4];
const uint8_t chatterblk_raymond_smash[4];
const uint8_t chatterblk_raymond_bear_down[3];
const uint8_t chatterblk_raymond_push_it[3];
const uint8_t chatterblk_raymond_harder[3];
const uint8_t chatterblk_raymond_oh_man[3];
const uint8_t chatterblk_raymond_random_pleas[4];
const uint8_t chatterblk_raymond_please[3];
const uint8_t chatterblk_raymond_get_moving[3];
const uint8_t chatterblk_nancy_time_running_out[5];
const uint8_t chatterblk_raymond_random_yelps[4];
const uint8_t chatterblk_raymond_ohno[3];
const uint8_t chatterblk_raymond_ouch[3];
const uint8_t chatterblk_raymond_yaow[3];
const uint8_t chatterblk_turbo[4];
const uint8_t chatterblk_tony_whoa[3];
const uint8_t chatterblk_tony_great[3];
const uint8_t chatterblk_raymond_one_more_time[3];
const uint8_t chatterblk_nancy_berates_hero[4];
const uint8_t chatterblk_nancy_wrong_job[3];
const uint8_t chatterblk_nancy_one_more_try[3];
const uint8_t chatterblk_nancy_mediocre_driver[4];
const uint8_t chatterblk_tony_lets_go[3];

const uint8_t *chatter_blocks[CHATTERBLK__LIMIT];

const stagevars_t saved_game_state;
const hazard_t saved_game_state_hazard_0;

const uint8_t font[41 * 7];
const uint8_t ledfont[10 * 16];
const uint8_t minifont[31 * 6];

const uint16_t inward_bend_table[96];

const uint8_t horizontal_e6b0[8][22];
const uint8_t horizontal_e760[8][22];

const uint8_t marquee_initial[SCREEN_BITMAP_ROWBYTES * MARQUEE_HEIGHT];
const uint8_t marquee_attrs[SCREEN_ATTRIBUTES_WIDTH * MARQUEE_HEIGHT / 8];

const uint8_t backbufexample[16 * 256];

#endif /* DATA_H */

