/**
 * ChaseHQ-CommonData.h
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

#ifndef CHASEHQ_COMMONDATA_H
#define CHASEHQ_COMMONDATA_H

#include "C99/Types.h"
#include "../ChaseHQ-State.h"

/* ----------------------------------------------------------------------- */

#define TWOBYTES(addr) (addr) & 0xFF, (addr) >> 8

#define KEYDEF(key, halfrow) (((key) << 3) | (halfrow))

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
const depthset_t shortpole_bottom;
const depthset_t shortpole_middle;
const depthset_t streetlampbottom_left;
const depthset_t streetlampbottom_right;
const depthset_t streetlampmiddle2_left;
const depthset_t streetlampmiddle2_right;
const depthset_t streetlampmiddle_left;
const depthset_t streetlampmiddle_right;
const bitmap_t streetlampbody_bitmaps[15];
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

const u8 attract_messages[38];
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
const u8 continue_messages_template[119];
const u8 stage_n_template[7];

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

const session_t saved_game_state;
const hazard_t saved_game_state_hazard_0;

const u8 font[41 * 7];

const hazard_t hazard_template;

const u8 hazard_pos_speed[3 * 4];

const u8 hero_car_jump_table[10 * 2];
const u8 car_jump_params[5 * 2];
const u8 horizon_table[32]; // byte table; Z80 $B828

const u8 smoke_ce0c_template[13];
const u8 smoke_ce19_template[13];
const u8 smoke_ce26_template[13];

const u8 debris_subtable_1_template[19];
const u8 debris_subtable_2_template[19];
const u8 debris_subtable_3_template[19];
const u8 debris_subtable_4_template[19];
const u8 debris_subtable_5_template[19];

const u8 bitmap_debris_1[2 * 6];
const u8 bitmap_debris_2[2 * 6];
const u8 bitmap_debris_3[2 * 6];
const u8 bitmap_debris_4[2 * 6];

const carpart_t hero_car_parts[9][5];
const carpart_t hero_car_shadow[3];
const carsmokeframe_t hero_car_turbo_smoke[4];

const carframe_t car_frames[39];
const caradornment_t car_adornments[6];

const u8 bitmap_arrow[3 * 2 * 28];
const u8 bitmap_cherry_light[1 * 2 * 7];
const u8 bitmap_cherry_light_lit[3 * 2 * 14];
const u8 bitmap_spark[3 * 2 * 20];
const u8 bitmap_cherryout_1[2 * 2 * 4];
const u8 bitmap_cherryout_2[2 * 2 * 9];
const u8 bitmap_cherryout_3[1 * 2 * 12];
const u8 bitmap_turbo_1[4 * 2 * 16];
const u8 bitmap_turbo_2[4 * 2 * 16];
const u8 bitmap_turbo_3[4 * 2 * 16];
const u8 bitmap_turbo_4[4 * 2 * 16];
const u8 bitmap_hero_centre_straight[5 * 14];
const u8 bitmap_hero_centre_straight_right[5 * 17];
const u8 bitmap_hero_centre_straight_right_hard[5 * 16];
const u8 bitmap_hero_centre_up[5 * 14];
const u8 bitmap_hero_centre_up_right[5 * 17];
const u8 bitmap_hero_centre_up_right_hard[5 * 15];
const u8 bitmap_hero_centre_down[5 * 14];
const u8 bitmap_hero_centre_down_right[5 * 16];
const u8 bitmap_hero_centre_down_right_hard[5 * 16];
const u8 bitmap_hero_top_straight[5 * 2 * 9];
const u8 bitmap_hero_bottom_straight[5 * 2 * 6];
const u8 bitmap_hero_left_straight[1 * 2 * 14];
const u8 bitmap_hero_right_straight[1 * 2 * 14];
const u8 bitmap_hero_top_straight_right[5 * 2 * 8];
const u8 bitmap_hero_bottom_straight_right[5 * 2 * 4];
const u8 bitmap_hero_left_straight_left[1 * 2 * 13];
const u8 bitmap_hero_right_straight_right[1 * 2 * 13];
const u8 bitmap_hero_top_straight_right_hard[5 * 2 * 9];
const u8 bitmap_hero_bottom_straight_right_hard[5 * 2 * 4];
const u8 bitmap_hero_left_straight_right_hard[1 * 2 * 12];
const u8 bitmap_hero_right_straight_right_hard[1 * 2 * 15];
const u8 bitmap_hero_top_up[5 * 2 * 10];
const u8 bitmap_hero_bottom_up[5 * 2 * 6];
const u8 bitmap_hero_left_up[1 * 2 * 13];
const u8 bitmap_hero_right_up[1 * 2 * 14];
const u8 bitmap_hero_top_right[5 * 2 * 9];
const u8 bitmap_hero_bottom_right[5 * 2 * 4];
const u8 bitmap_hero_left_right[1 * 2 * 12];
const u8 bitmap_hero_right_right[1 * 2 * 14];
const u8 bitmap_hero_up_right_hard[5 * 2 * 9];
const u8 bitmap_hero_bottom_right_hard[5 * 2 * 6];
const u8 bitmap_hero_left_right_hard[1 * 2 * 11];
const u8 bitmap_hero_right_right_hard[1 * 2 * 15];
const u8 bitmap_hero_top_down[5 * 2 * 8];
const u8 bitmap_hero_bottom_down[5 * 2 * 6];
const u8 bitmap_hero_left_down[1 * 2 * 13];
const u8 bitmap_hero_right_down[1 * 2 * 14];
const u8 bitmap_hero_top_down_right[5 * 2 * 8];
const u8 bitmap_hero_bottom_down_right[5 * 2 * 4];
const u8 bitmap_hero_left_down_right[1 * 2 * 13];
const u8 bitmap_hero_right_down_right[1 * 2 * 13];
const u8 bitmap_hero_top_down_right_hard[5 * 2 * 8];
const u8 bitmap_hero_bottom_down_right_hard[5 * 2 * 4];
const u8 bitmap_hero_left_down_right_hard[1 * 2 * 13];
const u8 bitmap_hero_right_down_right_hard[1 * 2 * 15];
const u8 bitmap_shadow_straight[7 * 2 * 12];
const u8 bitmap_shadow_turn_right[7 * 2 * 12];
const u8 bitmap_shadow_turn_right_hard[7 * 2 * 12];

#define LEDFONT_HEIGHT (15) // 15 rows/char
const u8 ledfont[10 * LEDFONT_HEIGHT];
#define MINIFONT_HEIGHT (6) // 6 rows/char
const u8 minifont[31 * MINIFONT_HEIGHT];

const bitmap_t fire1_defns[SPRITE_FRAMES];
const bitmap_t fire2_defns[SPRITE_FRAMES];
const bitmap_t fire3_defns[SPRITE_FRAMES];
const bitmap_t fire4_defns[SPRITE_FRAMES];
const bitmap_t fire5_defns[SPRITE_FRAMES];
const bitmap_t fire6_defns[SPRITE_FRAMES];
const bitmap_t smoke_defns[SPRITE_FRAMES];
const bitmap_t floating_arrow_here_defn;
const bitmap_t floating_arrow_left_defn;
const bitmap_t floating_arrow_right_defn;

const depthset_t tunnellight;
const bitmap_t tunnellight_bitmaps[SPRITE_FRAMES];
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

const u8 edge_markings[256]; /* full $E4xx page: zeros/$E400, edges/$E410, lanes/$E4D0 */

const u16 outward_bend_table[32];
const u16 curvature_to_xpos[96];

const u8 persp_y_scale[8][22];
const u8 persp_x_scale_right[8][22];
const u8 persp_x_delta_left[8][22];

typedef struct {
  u8        nframes;  // stores nframes+1
  const u8 *maskbase; // points to the frame before/after the base
} transition_t;

const transition_t transitions_e88e[8];

const u8 square_transition_mask[5 * 8];
const u8 diamond_transition_mask[6 * 8];

const u8 messages_stop_the_tape[45];
const u8 messages_input_methods[112];
const u8 messages_redefine_keys[138];
const u8 messages_test_mode[151];
const u8 messages_cannot_be_remodified[127];

const u8 key_names[10 * 8];
const u8 sinclair_joy_keydefs[5];
const u8 cursor_joy_keydefs[5];
const u8 shocked_keydefs[8];

const u8 drum1[252];
const u8 drum2[108];

const u8 music_patterns[19];
const u8 music_data[271];

const u8 press_gear_messages[17];
const u8 enter_for_options_messages[23];
const u8 credits_messages_128[98];
const u8 best_officers[163];

/* $86F6: backdrop blit instruction templates.
 * Two 18-byte chunks; selected slice is copied to dr_backdrop_copy_instrs by
 * dr_start_backdrop_fill.  0x2C = INC L (skip), 0xED/0xA0 = LDI (copy). */
extern const u8 backdrop_copy_instrs_template[36];

const u8 marquee_initial[SCREEN_BITMAP_ROWBYTES * MARQUEE_HEIGHT];
const u8 marquee_attrs[SCREEN_ATTRIBUTES_WIDTH * MARQUEE_HEIGHT / 8];

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_COMMONDATA_H */

