// Data.h
//
// Chase H.Q. code model
//
// by dpt

// vim: ts=8 sts=2 sw=2 et

#ifndef DATA_H
#define DATA_H

#include <stdint.h>

#include "Types.h"
#include "ChaseHQ-State.h"
#include "ChaseHQ.h"

const u8 stage1[16 * 512];

const u8 bitmap_turbospin[TURBOWIDTH / 8 * 2 * TURBOHEIGHT * TURBOFRAMES];
const u8 bitmap_faces[FACEBYTES * NFACES];

const u8 chatterblk_start_stage[4];
const u8 chatterblk_tony_giddy_up[3];
const u8 chatterblk_tony_hold_on[3];

const char *chatter_strings[CHATTERSTR__LIMIT];

const u8 attract_messages[37];
const u8 credits_messages[84];
const u8 copyright_messages[92];
const u8 score_messages_template[90];
const u8 sighting_message[36];
const u8 pull_over_message[30];
const u8 game_over_message[20];
const u8 time_up_message[20];
const u8 continue_messages[119];

const u8 chatterblk_pilot_turn_left[6];
const u8 chatterblk_pilot_turn_right[6];
const u8 chatterblk_heroes_acknowledge[4];
const u8 chatterblk_tony_loud_clear[3];
const u8 chatterblk_raymond_roger[3];
const u8 chatterblk_tony_gotcha[3];
const u8 chatterblk_raymond_wrong_way[4];
const u8 chatterblk_raymond_smash[4];
const u8 chatterblk_raymond_bear_down[3];
const u8 chatterblk_raymond_push_it[3];
const u8 chatterblk_raymond_harder[3];
const u8 chatterblk_raymond_oh_man[3];
const u8 chatterblk_raymond_random_pleas[4];
const u8 chatterblk_raymond_please[3];
const u8 chatterblk_raymond_get_moving[3];
const u8 chatterblk_nancy_time_running_out[5];
const u8 chatterblk_raymond_random_yelps[4];
const u8 chatterblk_raymond_ohno[3];
const u8 chatterblk_raymond_ouch[3];
const u8 chatterblk_raymond_yaow[3];
const u8 chatterblk_turbo[4];
const u8 chatterblk_tony_whoa[3];
const u8 chatterblk_tony_great[3];
const u8 chatterblk_raymond_one_more_time[3];
const u8 chatterblk_nancy_berates_hero[4];
const u8 chatterblk_nancy_wrong_job[3];
const u8 chatterblk_nancy_one_more_try[3];
const u8 chatterblk_nancy_mediocre_driver[4];
const u8 chatterblk_tony_lets_go[3];

const u8 *chatter_blocks[CHATTERBLK__LIMIT];

const stagevars_t saved_game_state;
const hazard_t saved_game_state_hazard_0;

const u8 font[41 * 7];
const u8 ledfont[10 * 16];
const u8 minifont[31 * 6];

const u8 spiral_transition_mask[13 * 8];
const u8 circle_transition_mask[7 * 8];

const u16 inward_bend_table[96];

const u8 horizontal_e6b0[8][22];
const u8 horizontal_e760[8][22];

const u8 transitions_e88e[3 * 8];

const u8 square_transition_mask[5 * 8];
const u8 diamond_transition_mask[6 * 8];

const u8 marquee_initial[SCREEN_BITMAP_ROWBYTES * MARQUEE_HEIGHT];
const u8 marquee_attrs[SCREEN_ATTRIBUTES_WIDTH * MARQUEE_HEIGHT / 8];

const u8 backbufexample[16 * 256];

#endif /* DATA_H */

