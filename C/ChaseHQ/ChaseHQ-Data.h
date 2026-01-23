// ChaseHQ-Data.h
//
// Chase H.Q. code model
//
// This file contains only the fixed/engine data from the game in the same
// order as the original.
//
// by dpt

#ifndef CHASEHQ_DATA_H
#define CHASEHQ_DATA_H

#include "Types.h"
#include "ChaseHQ-State.h"
#include "ChaseHQ.h"

/* ----------------------------------------------------------------------- */

#define TWOBYTES(addr) (addr) & 0xFF, (addr) >> 8

/* ----------------------------------------------------------------------- */

const u8 bitmap_turbospin[TURBOFRAMELENGTH * TURBOFRAMES];

const u8 pregame_messages[68];
const u8 pregame_data[207];
const u8 pregame_tiles[45 * 8];

const u8 bitmap_smoke1[4 * 13];
const u8 bitmap_smoke2[4 * 11];
const u8 bitmap_smoke3[4 * 9];
const u8 bitmap_smoke4[2 * 7];
const u8 bitmap_smoke5[2 * 5];
const u8 bitmap_smoke6[2 * 3];

const u8 bitmap_fire1[4 * 16];
const u8 bitmap_fire2[4 * 16];
const u8 bitmap_fire3[6 * 8];
const u8 bitmap_fire4[6 * 8];
const u8 bitmap_fire5[4 * 5];
const u8 bitmap_fire5s[4 * 5];
const u8 bitmap_fire6[4 * 6];
const u8 bitmap_fire6s[4 * 6];

const u8 bitmap_faces[FACEBYTES * NFACES];

const stretchy_t stretchy_shortpole[3];
const stretchyset_t shortpole_bottom;
const stretchyset_t shortpole_middle;
const stretchyset_t streetlampbottom_left;
const stretchyset_t streetlampbottom_right;
const stretchyset_t streetlampmiddle2_left;
const stretchyset_t streetlampmiddle2_right;
const stretchyset_t streetlampmiddle_left;
const stretchyset_t streetlampmiddle_right;
const lod_t streetlampbody_bitmaps[15];
const u8 bitmap_streetlampbody_1[4];
const u8 bitmap_streetlampbody_2[4];
const u8 bitmap_streetlampbody_3[4];
const u8 bitmap_streetlampbody_4[4];
const u8 bitmap_streetlampbody_5[4];
const u8 bitmap_streetlampbody_6[4];
const u8 bitmap_streetlampbody_7[2];
const u8 bitmap_streetlampbody_8[4];
const u8 bitmap_streetlampbody_9[4];
const u8 bitmap_streetlampbody_7s[2];
const u8 bitmap_streetlampbody_8s[4];
const u8 bitmap_streetlampbody_9s[4];
const u8 bitmap_streetlampbody_10[2];
const u8 bitmap_streetlampbody_11[4];
const u8 bitmap_streetlampbody_12[4];
const u8 bitmap_streetlampbody_10s[2];
const u8 bitmap_streetlampbody_11s[4];
const u8 bitmap_streetlampbody_12s[4];
const u8 bitmap_streetlampbody_13[4];
const u8 bitmap_streetlampbody_14[8];
const u8 bitmap_streetlampbody_15[8];
const u8 bitmap_streetlampbody_13s[4];
const u8 bitmap_streetlampbody_14s[8];
const u8 bitmap_streetlampbody_15s[8];

const u8 chatterblk_start_stage[4];
const u8 chatterblk_tony_giddy_up[3];
const u8 chatterblk_tony_hold_on[3];

const char *common_chatter_strings[CHATTERSTR__LIMIT];

const u8 attract_messages[37];
const u8 credits_messages[84];
const u8 copyright_messages[92];

const scenedata_t escape_scene_data;
const hazard_t escape_scene_perp;

const u8 sfx_crash_table[93];

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

const hazard_t hazard_template;

const u8 hazard_pos_speed[3 * 4];

const u8 ledfont[10 * 16];
const u8 minifont[31 * 6];

const light_t tunnellight;
const lod_t tunnellight_lods[6];
const u8 bitmap_tunnellight_1[2 * 16];
const u8 bitmap_tunnellight_2[2 * 12];
const u8 bitmap_tunnellight_3[1 * 8];
const u8 bitmap_tunnellight_4[1 * 6];
const u8 bitmap_tunnellight_5[1 * 5];
const u8 bitmap_tunnellight_6[2 * 2 * 6];
const u8 bitmap_tunnellight_6s[2 * 2 * 6];

const u8 perp_escape_hazards[6];
const u8 perp_escape_curvature[5];
const u8 perp_escape_height[5];
const u8 perp_escape_lanes[4];
const u8 fork_hazards[8];
const u8 fork_leftrightobjs[6];
const u8 forked_road_curvature[6];
const u8 forked_road_height[5];
const u8 forked_road_lanes[2];
const u8 forked_road_exit_hazards[3];
const u8 forked_road_exit_rightobjs[4];
const u8 forked_road_exit_leftobjs[4];
const u8 forked_road_exit_curvature[5];
const u8 forked_road_exit_height[5];
const u8 forked_road_exit_left_lanes[12];
const u8 forked_road_exit_right_lanes[12];

const u8 spiral_transition_mask[13 * 8];
const u8 circle_transition_mask[7 * 8];

const u16 inward_bend_table[96];

const u8 vertical_e600[8][22];
const u8 horizontal_e6b0[8][22];
const u8 horizontal_e760[8][22];

typedef struct {
  u8        nframes;  // stores nframes+1
  const u8 *maskbase; // points to the frame before/after the base
} transition_t;

const transition_t transitions_e88e[8];

const u8 square_transition_mask[5 * 8];
const u8 diamond_transition_mask[6 * 8];

const u8 marquee_initial[SCREEN_BITMAP_ROWBYTES * MARQUEE_HEIGHT];
const u8 marquee_attrs[SCREEN_ATTRIBUTES_WIDTH * MARQUEE_HEIGHT / 8];

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_DATA_H */

