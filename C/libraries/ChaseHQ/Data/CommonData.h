/**
 * CommonData.h
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

#ifndef CHASEHQ_COMMONDATA_H
#define CHASEHQ_COMMONDATA_H

#include "C99/Types.h"
#include "ZXSpectrum/Keyboard.h"
#include "ZXSpectrum/Pixels.h"
#include "ChaseHQ/Engine/State.h"

/* ----------------------------------------------------------------------- */

/* Packs a zxkey_t into the game's own key-definition scan-code byte format,
 * %RRRRRPPP, produced by scan_keyboard_matrix and consumed directly by
 * keyscan_inner ($A11E, Main.c): bits 2-0 (P) select the keyboard half-row
 * port via P+1 RRC rotations of $FE, bits 7-3 (R) select the bit position
 * within that row's 5-key byte, tested via 5-R RR rotations. zxkey_t's own
 * enum order groups keys into the same eight 5-key rows in the same order
 * (see Keyboard.h), so a key's row is zxkey/5 and its position within the
 * row is zxkey%5; R counts from the top of the row (position 0) down, hence
 * the (4 - position). Verified by round-tripping every table that uses it:
 * e.g. shocked_keydefs decodes to exactly S,H,O,C,K,E,D,ENTER. */
#define KEYDEF(zxkey) ((u8) (((4 - ((zxkey) % 5)) << 3) | ((zxkey) / 5)))

/* ----------------------------------------------------------------------- */

extern const pixel_t bitmap_faces[FACEBYTES * NFACES];

extern const depthset_t shortpole_bottom;
extern const depthset_t shortpole_middle;
extern const stretchy_t stretchy_shortpole[3];
extern const depthset_t streetlampbottom_left;
extern const depthset_t streetlampbottom_right;
extern const depthset_t streetlampmiddle2_left;
extern const depthset_t streetlampmiddle2_right;
extern const depthset_t streetlampmiddle_left;
extern const depthset_t streetlampmiddle_right;
extern const bitmap_t streetlampbody_bitmaps[15];
extern const pixel_t bitmap_streetlampbody_1[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_2[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_3[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_4[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_5[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_6[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_7[1 * 2 * 1 * 1];
extern const pixel_t bitmap_streetlampbody_8[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_9[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_7s[1 * 2 * 1 * 1];
extern const pixel_t bitmap_streetlampbody_8s[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_9s[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_10[1 * 2 * 1 * 1];
extern const pixel_t bitmap_streetlampbody_11[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_12[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_10s[1 * 2 * 1 * 1];
extern const pixel_t bitmap_streetlampbody_11s[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_12s[1 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_13[2 * 2 * 1 * 1];
extern const pixel_t bitmap_streetlampbody_14[2 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_15[2 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_13s[2 * 2 * 1 * 1];
extern const pixel_t bitmap_streetlampbody_14s[2 * 2 * 2 * 1];
extern const pixel_t bitmap_streetlampbody_15s[2 * 2 * 2 * 1];

extern const u8 chatterblk_start_stage[4];
extern const u8 chatterblk_tony_giddy_up[3];
extern const u8 chatterblk_tony_hold_on[3];

extern const char *common_chatter_strings[CHATTERSTR__LIMIT];

extern const u8 sfx_crash_table[93];

extern const u8 chatterblk_pilot_turn_left[6];
extern const u8 chatterblk_pilot_turn_right[6];
extern const u8 chatterblk_heroes_acknowledge[4];
extern const u8 chatterblk_tony_loud_clear[3];
extern const u8 chatterblk_raymond_roger[3];
extern const u8 chatterblk_tony_gotcha[3];
extern const u8 chatterblk_raymond_wrong_way[4];
extern const u8 chatterblk_raymond_smash[4];
extern const u8 chatterblk_raymond_bear_down[3];
extern const u8 chatterblk_raymond_push_it[3];
extern const u8 chatterblk_raymond_harder[3];
extern const u8 chatterblk_raymond_oh_man[3];
extern const u8 chatterblk_raymond_random_pleas[4];
extern const u8 chatterblk_raymond_please[3];
extern const u8 chatterblk_raymond_get_moving[3];
extern const u8 chatterblk_nancy_time_running_out[5];
extern const u8 chatterblk_raymond_random_yelps[4];
extern const u8 chatterblk_raymond_ohno[3];
extern const u8 chatterblk_raymond_ouch[3];
extern const u8 chatterblk_raymond_yaow[3];
extern const u8 chatterblk_turbo[4];
extern const u8 chatterblk_tony_whoa[3];
extern const u8 chatterblk_tony_great[3];
extern const u8 chatterblk_raymond_one_more_time[3];
extern const u8 chatterblk_nancy_berates_hero[4];
extern const u8 chatterblk_nancy_wrong_job[3];
extern const u8 chatterblk_nancy_one_more_try[3];
extern const u8 chatterblk_nancy_mediocre_driver[4];
extern const u8 chatterblk_tony_lets_go[3];

extern const u8 *chatter_blocks[CHATTERBLK__LIMIT];

extern const pixel_t font[41 * 7];

extern const u8 hazard_pos_speed[3 * 4];

extern const s8 hero_car_jump_table[10 * 2];
extern const u8 horizon_table[32]; // byte table; Z80 $B828

extern const pixel_t bitmap_arrow[3 * 2 * 28];

#define LEDFONT_HEIGHT (15) // 15 rows/char
#define MINIFONT_HEIGHT (6) // 6 rows/char

extern const depthset_t tunnellight;
extern const bitmap_t tunnellight_bitmaps[SPRITE_FRAMES];
extern const pixel_t bitmap_tunnellight_1[2 * 1 * 16 * 1];
extern const pixel_t bitmap_tunnellight_2[2 * 1 * 12 * 1];
extern const pixel_t bitmap_tunnellight_3[1 * 1 * 8 * 1];
extern const pixel_t bitmap_tunnellight_4[1 * 1 * 6 * 1];
extern const pixel_t bitmap_tunnellight_5[1 * 1 * 5 * 1];
extern const pixel_t bitmap_tunnellight_6[2 * 2 * 6 * 1];
extern const pixel_t bitmap_tunnellight_6s[2 * 2 * 6 * 1];

/* Helicopter sprite (stages 2 and 4 share identical part data; only the
 * per-stage bank addresses of the two frame tables differ in the original).
 */
extern const pixel_t bitmap_heli_part2_frame0[7 * 1 * 16 * 1];
extern const pixel_t bitmap_heli_part1_frame0[3 * 1 * 7 * 1];
extern const pixel_t bitmap_heli_part2_frame1[7 * 1 * 16 * 1];
extern const pixel_t bitmap_heli_part1_frame1[3 * 1 * 7 * 1];
extern const pixel_t bitmap_heli_part0[3 * 2 * 10 * 1];
extern const pixel_t bitmap_heli_part3_frame0[2 * 2 * 3 * 1];
extern const pixel_t bitmap_heli_part4_frame0[1 * 2 * 7 * 1];
extern const pixel_t bitmap_heli_part3_frame1[1 * 2 * 5 * 1];
extern const pixel_t bitmap_heli_part4_frame1[2 * 2 * 7 * 1];
extern const pixel_t bitmap_heli_rotor_frame0[5 * 2 * 10 * 1];
extern const pixel_t bitmap_heli_rotor_frame1[5 * 2 * 10 * 1];
extern const heli_bitmap_t heli_part0;
extern const heli_bitmap_t heli_part1_frame0;
extern const heli_bitmap_t heli_part1_frame1;
extern const heli_bitmap_t heli_part2_frame0;
extern const heli_bitmap_t heli_part2_frame1;
extern const heli_bitmap_t heli_part3_frame0;
extern const heli_bitmap_t heli_part3_frame1;
extern const heli_bitmap_t heli_part4_frame0;
extern const heli_bitmap_t heli_part4_frame1;
extern const heli_bitmap_xonly_t heli_rotor_frame0;
extern const heli_bitmap_xonly_t heli_rotor_frame1;
extern const heli_part_ptr_t heli_table_frame0[SPRITE_FRAMES];
extern const heli_part_ptr_t heli_table_frame1[SPRITE_FRAMES];

extern const u8 perp_escape_curvature[5];
extern const u8 perp_escape_height[5];
extern const u8 fork_hazards[8];
extern const u8 fork_leftrightobjs[6];
extern const u8 forked_road_curvature[6];
extern const u8 forked_road_height[5];
extern const u8 forked_road_lanes[2];

/* Z80 addresses of the shared map sections above, as referenced by the map
 * GOTO commands in CommonData.c and resolved by z80addrtocommonmap (Main.c).
 * Two sections are also entered one byte in, hence the "+ 1" call sites. */
#define PERP_ESCAPE_CURVATURE_ADDR (0xE2AA)
#define PERP_ESCAPE_HEIGHT_ADDR    (0xE2AF)
#define FORK_HAZARDS_ADDR          (0xE2B8)
#define FORK_LEFTRIGHTOBJS_ADDR    (0xE2C0)
#define FORKED_ROAD_CURVATURE_ADDR (0xE2C6)
#define FORKED_ROAD_HEIGHT_ADDR    (0xE2CC)
#define FORKED_ROAD_LANES_ADDR     (0xE2D1)

extern const pixel_t spiral_transition_frames[11 * 8];
extern const pixel_t circle_transition_frames[7 * 8];

extern const pixel_t edge_markings[256]; /* full $E4xx page: zeros/$E400, edges/$E410, lanes/$E4D0 */

extern const u8 persp_y_scale[8][PERSP_TABLE_COLS];

typedef struct
{
  u8        nframes; // stores nframes+1
  const u8 *frames;  // points to the frame before/after the base
} transition_t;

/* Entries in every stageN_map_goto_table[] must be sorted ascending by z80:
 * stage_lookup_map_goto() in Main.c binary-searches over them. */
typedef struct
{
  u16         z80; // original Z80 address
  const void *ptr; // corresponding C data pointer
} map_goto_entry_t;

extern const transition_t transitions[8];

extern const pixel_t square_transition_frames[5 * 8];
extern const pixel_t diamond_transition_frames[6 * 8];

extern const u8 music_patterns[19];
extern const u8 music_data[271];

extern const u8 sinclair_joy_keydefs[5];
extern const u8 temp_keydefs_template[8];

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_COMMONDATA_H */

