/**
 * Stage5Data.c
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

#include <stddef.h>

#include "C99/Types.h"
#include "ZXSpectrum/Pixels.h"
#include "ZXSpectrum/Spectrum.h"

#include "ChaseHQ/Engine/Types.h"
#include "CommonData.h"

#include "Stage5Data.h"

/* Z80 addresses of the map sections, as referenced by the map
 * GOTO/SPLIT commands and stage5_map_goto_table[]. */
#define STAGE5_MAP_CURV_C455_ADDR    (0x6055)
#define STAGE5_MAP_HEIGHT_C468_ADDR  (0x6068)
#define STAGE5_MAP_LANES_C47A_ADDR   (0x607A)
#define STAGE5_MAP_HAZARDS_C492_ADDR (0x6092)
#define STAGE5_MAP_LOBJS_C49A_ADDR   (0x609A)
#define STAGE5_MAP_ROBJS_C4DD_ADDR   (0x60DD)
#define STAGE5_MAP_CURV_C513_ADDR    (0x6113)
#define STAGE5_MAP_HEIGHT_C526_ADDR  (0x6126)
#define STAGE5_MAP_LANES_C53E_ADDR   (0x613E)
#define STAGE5_MAP_HAZARDS_C552_ADDR (0x6152)
#define STAGE5_MAP_LOBJS_C560_ADDR   (0x6160)
#define STAGE5_MAP_ROBJS_C5A3_ADDR   (0x61A3)
#define STAGE5_MAP_CURV_C5E3_ADDR    (0x61E3)
#define STAGE5_MAP_HEIGHT_C5F8_ADDR  (0x61F8)
#define STAGE5_MAP_LANES_C614_ADDR   (0x6214)
#define STAGE5_MAP_HAZARDS_C628_ADDR (0x6228)
#define STAGE5_MAP_LOBJS_C62D_ADDR   (0x622D)
#define STAGE5_MAP_ROBJS_C651_ADDR   (0x6251)
#define STAGE5_MAP_CURV_C65C_ADDR    (0x625C)
#define STAGE5_MAP_HEIGHT_C684_ADDR  (0x6284)
#define STAGE5_MAP_LANES_C6BB_ADDR   (0x62BB)
#define STAGE5_MAP_HAZARDS_C6D1_ADDR (0x62D1)
#define STAGE5_MAP_LOBJS_C6EE_ADDR   (0x62EE)
#define STAGE5_MAP_ROBJS_C7A5_ADDR   (0x63A5)

/* ----------------------------------------------------------------------- */

/* Stage 5 object type macros */

#define MAP_OBJ_S5_OVERHEAD_BRIDGE_VAL    (2)
#define MAP_OBJ_S5_CACTUS_VAL             (4)
#define MAP_OBJ_S5_DOUBLE_STREET_LAMP_VAL (5)
#define MAP_OBJ_S5_HUGE_ROCK_VAL          (6)
#define MAP_OBJ_S5_TELEGRAPH_POLE_VAL     (7)

#define MAP_OBJ_S5_OVERHEAD_BRIDGE(D)     (((D) << 4) | MAP_OBJ_S5_OVERHEAD_BRIDGE_VAL)
#define MAP_OBJ_S5_CACTUS(D)              (((D) << 4) | MAP_OBJ_S5_CACTUS_VAL)
#define MAP_OBJ_S5_DOUBLE_STREET_LAMP(D)  (((D) << 4) | MAP_OBJ_S5_DOUBLE_STREET_LAMP_VAL)
#define MAP_OBJ_S5_HUGE_ROCK(D)           (((D) << 4) | MAP_OBJ_S5_HUGE_ROCK_VAL)
#define MAP_OBJ_S5_TELEGRAPH_POLE(D)      (((D) << 4) | MAP_OBJ_S5_TELEGRAPH_POLE_VAL)

/* ----------------------------------------------------------------------- */

/* Forward declarations */

static const u8 stage5_perp_description[7];
static const char *stage5_chatter_strings[4];
static const u8 stage5_arrest_messages_C1E6[83];
static const hittable_t stage5_hittable_objects_C239[2];
static const obj_t stage5_right_obj_defs_C23F[7];
static const obj_t stage5_left_obj_defs_C270[7];
static const u8 stage5_map_curv_C2A1[40];
static const u8 stage5_map_height_C2C9[50];
static const u8 stage5_map_lanes_C2FB[18];
static const u8 stage5_map_hazards_C30D[31];
static const u8 stage5_map_lobjs_C32C[166];
static const u8 stage5_map_robjs_C3D2[131];
static const u8 stage5_map_curv_C455[19];
static const u8 stage5_map_height_C468[18];
static const u8 stage5_map_lanes_C47A[24];
static const u8 stage5_map_hazards_C492[8];
static const u8 stage5_map_lobjs_C49A[67];
static const u8 stage5_map_robjs_C4DD[54];
static const u8 stage5_map_curv_C513[19];
static const u8 stage5_map_height_C526[24];
static const u8 stage5_map_lanes_C53E[20];
static const u8 stage5_map_hazards_C552[14];
static const u8 stage5_map_lobjs_C560[67];
static const u8 stage5_map_robjs_C5A3[64];
static const u8 stage5_map_curv_C5E3[21];
static const u8 stage5_map_height_C5F8[28];
static const u8 stage5_map_lanes_C614[20];
static const u8 stage5_map_hazards_C628[5];
static const u8 stage5_map_lobjs_C62D[36];
static const u8 stage5_map_robjs_C651[11];
static const u8 stage5_map_curv_C65C[40];
static const u8 stage5_map_height_C684[55];
static const u8 stage5_map_lanes_C6BB[22];
static const u8 stage5_map_hazards_C6D1[29];
static const u8 stage5_map_lobjs_C6EE[183];
static const u8 stage5_map_robjs_C7A5[137];
static const u8 stage5_perp_face[180];
static const bitmap_t stage5_lods_C8E2[6];
static const bitmap_t stage5_lods_C90C[6];
static const bitmap_t stage5_lods_C936[6];
static const stretchy_t stage5_stretchy_CE91[5];
static const depthset_t stage5_depthset_CE9E;
static const depthset_t stage5_depthset_CEB4;
static const depthset_t stage5_depthset_CECA;
static const depthset_t stage5_depthset_CEE0;
static const stretchy_t stage5_stretchy_CEF6[5];
static const depthset_t stage5_depthset_CF03;
static const depthset_t stage5_depthset_CF19;
static const depthset_t stage5_depthset_CF2F;
static const depthset_t stage5_depthset_CF45;
static const bitmap_t stage5_lods_CF5B[20];
static const bitmap_t stage5_lods_CFE7[20];
static const stretchy_t stage5_stretchy_D28C[3];
static const depthset_t stage5_depthset_D293;
static const depthset_t stage5_depthset_D2A9;
static const bitmap_t stage5_lods_D2BF[8];
static const stretchy_t stage5_stretchy_D2F7[3];
static const depthset_t stage5_depthset_D2FE;
static const overhead_span_t stage5_overhead_spans_D314[10];
static const depthset_t stage5_depthset_D332;
static const bitmap_t stage5_lods_D348[8];
static const bitmap_t stage5_lods_D620[6];
static const stretchy_t stage5_stretchy_D6FF[5];
static const stretchy_t stage5_stretchy_D70C[5];
static const depthset_t stage5_depthset_D719;
static const depthset_t stage5_depthset_D72F;
static const depthset_t stage5_depthset_D745;
static const depthset_t stage5_depthset_D75B;
static const depthset_t stage5_depthset_D771;
static const depthset_t stage5_depthset_D787;
static const depthset_t stage5_depthset_D79D;
static const bitmap_t stage5_lods_D7B3[15];
static const stretchy_t stage5_stretchy_D8D2[5];
static const stretchy_t stage5_stretchy_D8DF[5];
static const depthset_t stage5_depthset_D8EC;
static const depthset_t stage5_depthset_D902;
static const bitmap_t stage5_lods_D918[5];
static const stretchy_t stage5_stretchy_D9CF[4];
static const stretchy_t stage5_stretchy_D9D9[4];
static const depthset_t stage5_depthset_D9E3;
static const depthset_t stage5_depthset_D9F9;
static const bitmap_t stage5_lods_DA0F[5];
static const u8 stage5_bitmap_C960[6 * 30];
static const u8 stage5_bitmap_CA14[5 * 22];
static const u8 stage5_bitmap_CA82[3 * 15];
static const u8 stage5_bitmap_CAAF[3 * 2 * 8];
static const u8 stage5_bitmap_CADF[3 * 2 * 8];
static const u8 stage5_bitmap_CB0F[6 * 39];
static const u8 stage5_bitmap_CBF9[5 * 29];
static const u8 stage5_bitmap_CC8A[3 * 20];
static const u8 stage5_bitmap_CCC6[2 * 2 * 12];
static const u8 stage5_bitmap_CCF6[2 * 2 * 12];
static const u8 stage5_bitmap_CD26[6 * 30];
static const u8 stage5_bitmap_CDDA[4 * 20];
static const u8 stage5_bitmap_CE2A[3 * 13];
static const u8 stage5_bitmap_CE51[2 * 2 * 8];
static const u8 stage5_bitmap_CE71[2 * 2 * 8];
static const u8 stage5_bitmap_D073[5 * 8];
static const u8 stage5_bitmap_D09B[7 * 8];
static const u8 stage5_bitmap_D0D3[7 * 8];
static const u8 stage5_bitmap_D10B[8 * 8];
static const u8 stage5_bitmap_D14B[3 * 4];
static const u8 stage5_bitmap_D157[4 * 4];
static const u8 stage5_bitmap_D16C[5 * 6];
static const u8 stage5_bitmap_D18A[6 * 6];
static const u8 stage5_bitmap_D1AE[2 * 4];
static const u8 stage5_bitmap_D1B6[3 * 3];
static const u8 stage5_bitmap_D1BF[3 * 4];
static const u8 stage5_bitmap_D1CB[4 * 4];
static const u8 stage5_bitmap_D1DB[1 * 3];
static const u8 stage5_bitmap_D1DE[2 * 3];
static const u8 stage5_bitmap_D1E4[2 * 3];
static const u8 stage5_bitmap_D1EA[3 * 2];
static const u8 stage5_bitmap_D1F0[7 * 8];
static const u8 stage5_bitmap_D228[5 * 6];
static const u8 stage5_bitmap_D246[3 * 4];
static const u8 stage5_bitmap_D252[2 * 3];
static const u8 stage5_bitmap_D258[1 * 2 * 1];
static const u8 stage5_bitmap_D25A[2 * 2 * 1];
static const u8 stage5_bitmap_D25E[2 * 2 * 2];
static const u8 stage5_bitmap_D266[3 * 2 * 2];
static const u8 stage5_bitmap_D272[1 * 2 * 1];
static const u8 stage5_bitmap_D274[2 * 2 * 1];
static const u8 stage5_bitmap_D278[2 * 2 * 2];
static const u8 stage5_bitmap_D280[3 * 2 * 2];
static const u8 stage5_bitmap_D380[4 * 16];
static const u8 stage5_bitmap_D3C0[4 * 24];
static const u8 stage5_bitmap_D420[24];
static const u8 stage5_bitmap_D438[4 * 16];
static const u8 stage5_bitmap_D478[4 * 24];
static const u8 stage5_bitmap_D4D8[3 * 12];
static const u8 stage5_bitmap_D4FC[3 * 18];
static const u8 stage5_bitmap_D535[18];
static const u8 stage5_bitmap_D547[3 * 12];
static const u8 stage5_bitmap_D56B[3 * 18];
static const u8 stage5_bitmap_D5A1[2 * 8];
static const u8 stage5_bitmap_D5B1[2 * 12];
static const u8 stage5_bitmap_D5C9[12];
static const u8 stage5_bitmap_D5D5[2 * 8];
static const u8 stage5_bitmap_D5E5[2 * 12];
static const u8 stage5_bitmap_D5FD[1 * 4];
static const u8 stage5_bitmap_D601[1 * 6];
static const u8 stage5_bitmap_D607[7];
static const u8 stage5_bitmap_D60E[1 * 4];
static const u8 stage5_bitmap_D612[1 * 6];
static const u8 stage5_bitmap_D64A[4 * 17];
static const u8 stage5_bitmap_D68E[3 * 13];
static const u8 stage5_bitmap_D6B5[2 * 9];
static const u8 stage5_bitmap_D6C7[2 * 2 * 7];
static const u8 stage5_bitmap_D6E3[2 * 2 * 7];
static const u8 stage5_bitmap_D81C[3 * 4];
static const u8 stage5_bitmap_D828[3 * 13];
static const u8 stage5_bitmap_D84F[1 * 7];
static const u8 stage5_bitmap_D856[2 * 3];
static const u8 stage5_bitmap_D85C[1 * 2];
static const u8 stage5_bitmap_D85E[1 * 6];
static const u8 stage5_bitmap_D864[1 * 2];
static const u8 stage5_bitmap_D866[1 * 2 + 4];
static const u8 stage5_bitmap_D86C[6];
static const u8 stage5_bitmap_D872[2 * 2 * 3];
static const u8 stage5_bitmap_D87E[2 * 2 * 9];
static const u8 stage5_bitmap_D8A2[2 * 2 * 2];
static const u8 stage5_bitmap_D8AA[1 * 2 * 2];
static const u8 stage5_bitmap_D8AE[1 * 2 * 6];
static const u8 stage5_bitmap_D8BA[2 * 2 * 2];
static const u8 stage5_bitmap_D8C2[1 * 2 * 2];
static const u8 stage5_bitmap_D8C6[1 * 2 * 6];
static const u8 stage5_bitmap_D93B[5 * 8];
static const u8 stage5_bitmap_D963[4 * 6];
static const u8 stage5_bitmap_D97B[3 * 4];
static const u8 stage5_bitmap_D987[3 * 4];
static const u8 stage5_bitmap_D993[3 * 2 * 3];
static const u8 stage5_bitmap_D9A5[3 * 2 * 3];
static const u8 stage5_bitmap_D9B7[2 * 2 * 2 + 4];
static const u8 stage5_bitmap_D9C3[2 * 2 * 2 + 4];
static const u8 stage5_bitmap_DA32[3 * 13];
static const u8 stage5_bitmap_DA59[3 * 10];
static const u8 stage5_bitmap_DA77[3 * 7];
static const u8 stage5_bitmap_DA8C[3 * 7];
static const u8 stage5_bitmap_DAA1[2 * 2 * 5];
static const u8 stage5_bitmap_DAB5[2 * 2 * 5];
static const u8 stage5_bitmap_DAC9[2 * 2 * 4];
static const u8 stage5_bitmap_DAD9[2 * 2 * 4 + 7];

/* ----------------------------------------------------------------------- */

/** $C000: stage5 */
// clang-format off
const stage_t stage5 = {
  /* $C000 backdrop */
  {
    XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
    ________, ________, ____X_X_, X_XX_XXX, ___XXX_X, _X______, X_X_X_X_, X_XXXXXX, XXXXXXXX, XXXXXXXX,
    ________, ________, ________, _X_XXXX_, X_XXX_X_, X_______, _X_X_XXX, XXXXXXXX, XXXXXXXX, XXXXX_X_,
    X_______, ______X_, X_X__X_X, _XX_XXXX, __XX_X__, ______X_, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X,
    _XXX____, ________, ________, _X_XX_X_, X_XXX___, _____X_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_X_X_,
    _X_XXX__, ________, ________, X_XXXX__, __X__X_X, X_XXXXX_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_XXX,
    X_X_XXX_, ________, ________, _XX_XXX_, X_X_____, _____XXX, XXX__XXX, XXXXXXXX, XXXXXXXX, XXXXX_X_,
    _XXX_XXX, ________, __X_X_X_, X_XX_X__, ________, __X_X_XX, XX_____X, XXXXXXXX, XXXXXXXX, XX_X_X_X,
    X_X_X__X, XX______, _______X, _XXXX___, ___X_X_X, XXXXXXXX, XX_X___X, XXXXXXXX, XXXXX___, ________,
    ___X_X__, _XXX____, ________, XX_XX_XX, ________, X_X_XXXX, X_______, XXX___XX, ________, ________,
    _______X, __XXX___, _______X, X_XXXX_X, _X_____X, XX_XXXXX, X____X__, X_______, _X__X___, _X______,
    ____X___, ___XXX__, __X_X_X_, XXXX____, ______X_, XXXXXXXX, X__X____, _____X__, ______X_, ___X__X_,
    ______X_, _X__XXXX, ____XXXX, XX______, ___XXX_X, XXXXXXXX, X_______, __X_____, ___X____, ________,
    ________, _____X__, _______X, XXXXX_X_, __XX_XX_, X_XXXXXX, X_______, X_______, ________, ____X_X_,
    X_______, __X_XXX_, X_XX_XXX, XX_X____, ______XX, XXXXXXXX, ________, ________, _____X__, _X______,
    ________, _____XXX, __X_X_X_, XXX_____, _______X, _X_XXXXX, ________, ______X_, ________, ______X_,
    ________, _X____X_, _______X, _XX_X___, ________, __XXXXXX, ________, _______X, ________, X__X____,
    ________, ___X_XXX, ________, XXXX_X__, ________, ___XXXX_, ________, ________, ________, ________,
    ________, ______XX, X______X, _XXXXX__, _____XX_, __XXXX_X, ________, _______X, ________, __X_X___,
    ________, ___X_X_X, XX______, XXX__XXX, _X_XXXXX, _XXX____, _X______, ________, X_______, ________,
    _X______, ______X_, XXXX___X, XX_____X, X_XX___X, X_______, ___X____, __X_X___, ________, X____XXX,
    ___X____, _____X_X, ___XXXXX, ________, ________, ________, ____XX__, _X___X__, ________, ___XX___,
    _____X_X, ____XXX_, ________, ________, ________, ________, _______X, X______X, _X______, _XX_____,
    ________, XXXXX___, ________, ________, ________, ________, ________, ________, __XXXXXX, ________,
  },
  /* $C0F0 perstage */
  &stage5_perp_face[FACEBITMAPBYTES],
  NULL,  /* no pilot mugshot on this stage */
  attribute_BRIGHT_BLACK_OVER_YELLOW * 0x0101,
  &stage5_hittable_objects_C239[0],  /* addrof_hittable_objects */
  &stage5_right_obj_defs_C23F[-1].arg,  /* addrof_right_hand_handlers */
  &stage5_right_obj_defs_C23F[-1],  /* addrof_right_hand_objects */
  &stage5_right_obj_defs_C23F[2],  /* addrof_right_hand_short_pole_object */
  &stage5_left_obj_defs_C270[-1].arg,  /* addrof_left_hand_handlers */
  &stage5_left_obj_defs_C270[-1],  /* addrof_left_hand_objects */
  &stage5_left_obj_defs_C270[2],  /* addrof_left_hand_short_pole_object */
  &stage5_perp_description[0],  /* addrof_perp_description */
  &stage5_arrest_messages_C1E6[0],  /* addrof_arrest_messages */
  { NULL, NULL },

  NULL,  /* bitmaps_stones */
  NULL,  /* bitmaps_dust */
  &stage5_lods_C8E2[0],  /* bitmaps_perp_car */
  {
    &stage5_lods_C936[0],
    &stage5_lods_C90C[0],
    &stage5_lods_C936[0],
    &stage5_lods_C8E2[0]
  },  /* bitmaps_vehicles */

  /* $C11A difficulty */
  20,  /* car_spawn_delay */
  40,  /* perp_lane_change_base */
  40,  /* perp_approach_base */

  /* $C11D setupdata */
  {
    158,
    &stage5_map_curv_C2A1[-1],
    &stage5_map_height_C2C9[-1],
    &stage5_map_lanes_C2FB[-1],
    &stage5_map_robjs_C3D2[-1],
    &stage5_map_lobjs_C32C[-1],
    &stage5_map_hazards_C30D[-1],
  },

  /* $C12B attractdata */
  {
    234,
    &stage5_map_curv_C65C[-1],
    &stage5_map_height_C684[-1],
    &stage5_map_lanes_C6BB[-1],
    &stage5_map_robjs_C7A5[-1],
    &stage5_map_lobjs_C6EE[-1],
    &stage5_map_hazards_C6D1[-1],
  },

  stage5_chatter_strings
};

/* ----------------------------------------------------------------------- */

/** $C139: stage5_perp_description */
static const u8 stage5_perp_description[7] = {
  CHATTERCHR_NANCY,
  CHATTERSTR_PERP_DESC_1,
  CHATTERSTR_PERP_DESC_2,
  CHATTERSTR_PERP_DESC_3,
  CHATTERSTR_PERP_DESC_4,
  CHATTERCMD_PAUSE,
  CHATTERBLK_HEROES_ACKNOWLEDGE
};

/** $C145 (bank 7) */
static const char *stage5_chatter_strings[4] = {
  "THIS IS NANCY AT CHASE H.Q. WE'VE GOT A\xCE",
  "EMERGENCY HERE. THE EASTERN BLOC SPY FRO\xCD",
  "WASHINGTON IS FLEEING TOWARDS THE SUBURBS\xAE",
  "THE TARGET VEHICLE IS UNKNOWN... OVER\xAE",
};

/* ----------------------------------------------------------------------- */

/**
 * $C1E6: stage5_arrest_messages_C1E6
 */
static const u8 stage5_arrest_messages_C1E6[83] = {
  6,  // initial delay

  6,  // delay
  DRAWCHARSTYLE_SINGLE,
  0,  // attribute
  CHQBACKBUF(0xF802),
  ZXATTRS(0x5922),
  'O', 'K', '!', ' ', 'Y', 'O', 'U', ' ', 'A', 'R', 'E', ' ', 'U', 'N', 'D', 'E', 'R', ' ', 'A', 'R', 'R', 'E', 'S', 'T', ' ', 'O', 'N' | EOS,

  6,  // delay
  DRAWCHARSTYLE_SINGLE,
  0,  // attribute
  CHQBACKBUF(0xF023),
  ZXATTRS(0x5943),
  'S', 'U', 'S', 'P', 'I', 'C', 'I', 'O', 'N', ' ', 'O', 'F', ' ', 'E', 'S', 'P', 'I', 'O', 'N', 'A', 'G', 'E', ' ', 'A', 'N', 'D' | EOS,

  45,  // delay
  DRAWCHARSTYLE_SINGLE,
  0,  // attribute
  CHQBACKBUF(0xF82C),
  ZXATTRS(0x596C),
  'M', 'U', 'R', 'D', 'E', 'R' | EOS,

  TRANSITIONCONTROL_FILL_ATTRIBUTES,
  DRAWOVERLAY_STOP
};

/* ----------------------------------------------------------------------- */

/** $C239: stage5_hittable_objects_C239 */
static const hittable_t stage5_hittable_objects_C239[2] = {
  { 32, &stage5_lods_D620[0] },
  { 32, &stage5_lods_D620[0] },
};

/** $C23F: stage5_right_obj_defs_C23F */
static const obj_t stage5_right_obj_defs_C23F[7] = {
  { 111, 41, 80, &tunnellight, draw_tunnel_light_right },
  { 111, 53, 60, &stage5_stretchy_D28C[0], draw_stretchy_object_right },
  { 144, 92, 40, &stretchy_shortpole, draw_stretchy_object_right },
  { 116, 56, 80, &stage5_stretchy_D6FF[0], draw_stretchy_object_right },
  { 110, 53, 70, &stage5_stretchy_D8D2[0], draw_stretchy_object_right },
  { 127, 16, 50, &stage5_stretchy_CE91[0], draw_stretchy_object_right },
  { 110, 53, 70, &stage5_stretchy_D9CF[0], draw_stretchy_object_right },
};

/** $C270: stage5_left_obj_defs_C270 */
static const obj_t stage5_left_obj_defs_C270[7] = {
  { 126, 188, 80, &tunnellight, draw_tunnel_light_left },
  { 126, 154, 60, &stage5_stretchy_D2F7[0], draw_overhead },
  { 96, 144, 40, &stretchy_shortpole, draw_stretchy_object_left },
  { 126, 188, 80, &stage5_stretchy_D70C[0], draw_stretchy_object_left },
  { 132, 182, 70, &stage5_stretchy_D8DF[0], draw_stretchy_object_left },
  { 112, 224, 50, &stage5_stretchy_CEF6[0], draw_stretchy_object_left },
  { 132, 182, 70, &stage5_stretchy_D9D9[0], draw_stretchy_object_left },
};

/* ----------------------------------------------------------------------- */

/** $C2A1: stage5_map_curv_C2A1 */
static const u8 stage5_map_curv_C2A1[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(6),
  MAP_CURVE_LEFT(14),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(6),
  MAP_CURVE_RIGHT_VERY_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(10),
  MAP_CURVE_RIGHT(10),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(4),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(1),
  MAP_CURVE_LEFT_VERY_HARD(15),
  MAP_CURVE_LEFT_VERY_HARD(5),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(3),
  MAP_CURVE_LEFT(7),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(9),
  MAP_CMD_SPLIT(STAGE5_MAP_CURV_C513_ADDR, STAGE5_MAP_CURV_C455_ADDR)
};

/** $C2C9: stage5_map_height_C2C9 */
static const u8 stage5_map_height_C2C9[] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(10),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_UP7(1),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(5),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN5(3),
  MAP_HEIGHT_DOWN3(4),
  MAP_HEIGHT_DOWN7(1),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(4),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(3),
  MAP_HEIGHT_DOWN7(3),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(3),
  MAP_CMD_SPLIT(STAGE5_MAP_HEIGHT_C526_ADDR, STAGE5_MAP_HEIGHT_C468_ADDR)
};

/** $C2FB: stage5_map_lanes_C2FB */
static const u8 stage5_map_lanes_C2FB[] = {
  MAP_LANES_2R(36),
  MAP_LANES_2RTO3R(2),
  MAP_LANES_3R(52),
  MAP_LANES_3RTO4(2),
  MAP_LANES_4(254),
  MAP_LANES_4(54),
  MAP_CMD_SPLIT(STAGE5_MAP_LANES_C53E_ADDR, STAGE5_MAP_LANES_C47A_ADDR)
};

/** $C30D: stage5_map_hazards_C30D */
static const u8 stage5_map_hazards_C30D[] = {
  MAP_HAZARD_WAIT(20),
  MAP_CMD_START_BARRIERS_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(45),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(43),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(4),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(76),
  MAP_CMD_STOP_CARS,
  MAP_HAZARD_WAIT(6),
  MAP_CMD_ARROW_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_SPLIT(STAGE5_MAP_HAZARDS_C552_ADDR, STAGE5_MAP_HAZARDS_C492_ADDR)
};

/** $C32C: stage5_map_lobjs_C32C */
static const u8 stage5_map_lobjs_C32C[] = {
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_CMD_SPLIT(STAGE5_MAP_LOBJS_C560_ADDR, STAGE5_MAP_LOBJS_C49A_ADDR)
};

/** $C3D2: stage5_map_robjs_C3D2 */
static const u8 stage5_map_robjs_C3D2[] = {
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(4),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(13),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(11),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_CMD_SPLIT(STAGE5_MAP_ROBJS_C5A3_ADDR, STAGE5_MAP_ROBJS_C4DD_ADDR)
};

/** $C455: stage5_map_curv_C455 */
static const u8 stage5_map_curv_C455[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(10),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(11),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(10),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(2),
  MAP_CMD_GOTO(STAGE5_MAP_CURV_C5E3_ADDR)
};

/** $C468: stage5_map_height_C468 */
static const u8 stage5_map_height_C468[] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(3),
  MAP_CMD_GOTO(STAGE5_MAP_HEIGHT_C5F8_ADDR)
};

/** $C47A: stage5_map_lanes_C47A */
static const u8 stage5_map_lanes_C47A[] = {
  MAP_LANES_4TO3L(2),
  MAP_LANES_TUNNEL_ENTRY(20),
  MAP_LANES_TUNNEL_EXIT(2),
  MAP_LANES_3L(4),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(6),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(74),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(84),
  MAP_CMD_GOTO(STAGE5_MAP_LANES_C614_ADDR)
};

/** $C492: stage5_map_hazards_C492 */
static const u8 stage5_map_hazards_C492[] = {
  MAP_HAZARD_WAIT(10),
  MAP_CMD_START_CARS,
  MAP_HAZARD_WAIT(89),
  MAP_CMD_GOTO(STAGE5_MAP_HAZARDS_C628_ADDR)
};

/** $C49A: stage5_map_lobjs_C49A */
static const u8 stage5_map_lobjs_C49A[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_TUNNEL_LIGHT(10),
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(2),
  MAP_CMD_GOTO(STAGE5_MAP_LOBJS_C62D_ADDR)
};

/** $C4DD: stage5_map_robjs_C4DD */
static const u8 stage5_map_robjs_C4DD[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_TUNNEL_LIGHT(10),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(11),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(2),
  MAP_CMD_GOTO(STAGE5_MAP_ROBJS_C651_ADDR)
};

/** $C513: stage5_map_curv_C513 */
static const u8 stage5_map_curv_C513[] = {
  MAP_CURVE_STRAIGHT(12),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(13),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(10),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(8),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CMD_GOTO(STAGE5_MAP_CURV_C5E3_ADDR)
};

/** $C526: stage5_map_height_C526 */
static const u8 stage5_map_height_C526[] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(3),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP5(3),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN5(3),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(5),
  MAP_CMD_GOTO(STAGE5_MAP_HEIGHT_C5F8_ADDR)
};

/** $C53E: stage5_map_lanes_C53E */
static const u8 stage5_map_lanes_C53E[] = {
  MAP_LANES_4(40),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(124),
  MAP_LANES_TUNNEL_ENTRY(20),
  MAP_LANES_TUNNEL_EXIT(2),
  MAP_LANES_3L(2),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(6),
  MAP_CMD_GOTO(STAGE5_MAP_LANES_C614_ADDR)
};

/** $C552: stage5_map_hazards_C552 */
static const u8 stage5_map_hazards_C552[] = {
  MAP_HAZARD_WAIT(10),
  MAP_CMD_START_CARS,
  MAP_HAZARD_WAIT(8),
  MAP_CMD_START_BARRIERS_R,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(79),
  MAP_CMD_GOTO(STAGE5_MAP_HAZARDS_C628_ADDR)
};

/** $C560: stage5_map_lobjs_C560 */
static const u8 stage5_map_lobjs_C560[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(6),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_TUNNEL_LIGHT(10),
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_CMD_GOTO(STAGE5_MAP_LOBJS_C62D_ADDR)
};

/** $C5A3: stage5_map_robjs_C5A3 */
static const u8 stage5_map_robjs_C5A3[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(6),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_OVERHEAD_BRIDGE(1),
  MAP_OBJ_NONE(7),
  MAP_OBJ_TUNNEL_LIGHT(10),
  MAP_OBJ_NONE(5),
  MAP_CMD_GOTO(STAGE5_MAP_ROBJS_C651_ADDR)
};

/** $C5E3: stage5_map_curv_C5E3 */
static const u8 stage5_map_curv_C5E3[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(5),
  MAP_CURVE_LEFT_HARD(12),
  MAP_CURVE_RIGHT(1),
  MAP_CURVE_RIGHT_HARD(10),
  MAP_CURVE_RIGHT(6),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(4),
  MAP_CURVE_RIGHT(15),
  MAP_CURVE_RIGHT(11),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(2),
  MAP_CMD_GOTO(STAGE5_MAP_CURV_C65C_ADDR)
};

/** $C5F8: stage5_map_height_C5F8 */
static const u8 stage5_map_height_C5F8[] = {
  MAP_HEIGHT_DOWN5(15),
  MAP_HEIGHT_DOWN3(3),
  MAP_HEIGHT_LEVEL(4),
  MAP_HEIGHT_UP7(15),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(13),
  MAP_HEIGHT_UP3(15),
  MAP_HEIGHT_UP3(5),
  MAP_HEIGHT_UP5(6),
  MAP_HEIGHT_UP7(8),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_UP3(2),
  MAP_HEIGHT_LEVEL(1),
  MAP_HEIGHT_DOWN1(2),
  MAP_HEIGHT_DOWN3(4),
  MAP_HEIGHT_DOWN5(4),
  MAP_HEIGHT_DOWN7(11),
  MAP_HEIGHT_DOWN5(10),
  MAP_HEIGHT_DOWN3(11),
  MAP_HEIGHT_DOWN1(2),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(7),
  MAP_CMD_GOTO(STAGE5_MAP_HEIGHT_C684_ADDR)
};

/** $C614: stage5_map_lanes_C614 */
static const u8 stage5_map_lanes_C614[] = {
  MAP_LANES_4(44),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(12),
  MAP_LANES_TUNNEL_ENTRY(102),
  MAP_LANES_TUNNEL_EXIT(2),
  MAP_LANES_3L(18),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(4),
  MAP_CMD_GOTO(STAGE5_MAP_LANES_C6BB_ADDR)
};

/** $C628: stage5_map_hazards_C628 */
static const u8 stage5_map_hazards_C628[] = {
  MAP_HAZARD_WAIT(93),
  MAP_CMD_GOTO(STAGE5_MAP_HAZARDS_C6D1_ADDR)
};

/** $C62D: stage5_map_lobjs_C62D */
static const u8 stage5_map_lobjs_C62D[] = {
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_TELEGRAPH_POLE(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(15),
  MAP_OBJ_TUNNEL_LIGHT(6),
  MAP_OBJ_NONE(12),
  MAP_CMD_GOTO(STAGE5_MAP_LOBJS_C6EE_ADDR)
};

/** $C651: stage5_map_robjs_C651 */
static const u8 stage5_map_robjs_C651[] = {
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(3),
  MAP_CMD_GOTO(STAGE5_MAP_ROBJS_C7A5_ADDR)
};

/** $C65C: stage5_map_curv_C65C */
static const u8 stage5_map_curv_C65C[] = {
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(10),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(11),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(4),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(11),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(9),
  MAP_CURVE_LEFT_HARD(15),
  MAP_CURVE_LEFT_HARD(6),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(10),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(9),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(9),
  MAP_CURVE_RIGHT_VERY_HARD(15),
  MAP_CURVE_RIGHT_VERY_HARD(2),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(15),
  MAP_CURVE_RIGHT_HARD(2),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(15),
  MAP_CURVE_STRAIGHT(1),
  MAP_CMD_GOTO(STAGE5_MAP_CURV_C65C_ADDR)
};

/** $C684: stage5_map_height_C684 */
static const u8 stage5_map_height_C684[] = {
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(12),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_DOWN5(4),
  MAP_HEIGHT_DOWN7(2),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(2),
  MAP_HEIGHT_LEVEL(2),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP5(2),
  MAP_HEIGHT_UP7(8),
  MAP_HEIGHT_DOWN7(4),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(6),
  MAP_HEIGHT_DOWN7(4),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN1(3),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(6),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(1),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP3(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN7(1),
  MAP_HEIGHT_DOWN5(1),
  MAP_HEIGHT_DOWN3(1),
  MAP_HEIGHT_LEVEL(6),
  MAP_HEIGHT_UP5(1),
  MAP_HEIGHT_UP7(2),
  MAP_HEIGHT_DOWN3(8),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(15),
  MAP_HEIGHT_LEVEL(13),
  MAP_CMD_GOTO(STAGE5_MAP_HEIGHT_C684_ADDR)
};

/** $C6BB: stage5_map_lanes_C6BB */
static const u8 stage5_map_lanes_C6BB[] = {
  MAP_LANES_4(254),
  MAP_LANES_4(114),
  MAP_LANES_4TO3R(36),
  MAP_LANES_3RTO4(2),
  MAP_LANES_4(4),
  MAP_LANES_4TO3L(2),
  MAP_LANES_3L(8),
  MAP_LANES_3LTO4(2),
  MAP_LANES_4(22),
  MAP_CMD_GOTO(STAGE5_MAP_LANES_C6BB_ADDR)
};

/** $C6D1: stage5_map_hazards_C6D1 */
static const u8 stage5_map_hazards_C6D1[] = {
  MAP_HAZARD_WAIT(13),
  MAP_CMD_START_TWO_BARRIERS,
  MAP_HAZARD_WAIT(4),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(40),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(91),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(28),
  MAP_CMD_START_BARRIERS_L,
  MAP_HAZARD_WAIT(2),
  MAP_CMD_STOP_BARRIERS,
  MAP_HAZARD_WAIT(40),
  MAP_CMD_GOTO(STAGE5_MAP_HAZARDS_C6D1_ADDR)
};

/** $C6EE: stage5_map_lobjs_C6EE */
static const u8 stage5_map_lobjs_C6EE[] = {
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(9),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_DOUBLE_STREET_LAMP(1),
  MAP_OBJ_NONE(1),
  MAP_CMD_GOTO(STAGE5_MAP_LOBJS_C6EE_ADDR)
};

/** $C7A5: stage5_map_robjs_C7A5 */
static const u8 stage5_map_robjs_C7A5[] = {
  MAP_OBJ_NONE(6),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(12),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(15),
  MAP_OBJ_NONE(2),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_HUGE_ROCK(1),
  MAP_OBJ_NONE(5),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(3),
  MAP_OBJ_S5_CACTUS(1),
  MAP_OBJ_NONE(1),
  MAP_CMD_GOTO(STAGE5_MAP_ROBJS_C7A5_ADDR)
};

/**
 * $C82E: stage5_perp_face
 */
static const u8 stage5_perp_face[FACEBYTES] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXXXX, X_XXXXX_, XXX_XXX_, X_X_X__X,
  XXXXXXXX, X_XX_XX_, XX__XX_X, XXXX_X_X,
  XXXXXXXX, _XXXXX_X, XX_X_XXX, _XX__X_X,
  XXXXXXXX, XXX__XX_, _XXXX_X_, _XXXX__X,
  XXXXXXXX, XX______, _______X, XXX_XX_X,
  XXXXXXXX, X_X_____, ________, XXXXXX_X,
  XXXXXXXX, XX______, _______X, _XXXXX_X,
  XXXXXXXX, X_______, ________, X_XXX__X,
  XXXXXXXX, XX______, ________, _XXXX__X,
  XXXXXXXX, X_X_____, ________, X_XXX__X,
  XXXXXXXX, XX______, __XXXX__, _XXXX__X,
  XXXXXXXX, XXXX____, XXXXXXX_, _XXX___X,
  XXXXXXXX, XXXX____, XXX____X, _XXX___X,
  XXXXXXX_, ___XX___, XXXXXX__, _XXXX__X,
  XXXXXX__, XXXXXX__, XX_XX_X_, __XX_X_X,
  XXXXXXXX, XX_XXX__, _X______, ___X_X_X,
  XXXXXX_X, __XXX___, _XXX____, __XX_X_X,
  XXXXXX__, XX_XXX__, _XX_____, ___X_X_X,
  X_XXX___, __XXX___, ___X____, __XX_X_X,
  X_XXXX_X, _XXX____, ___XX___, ___XX__X,
  X_XXXXX_, X_XXXX__, _XXXXX__, ___XX__X,
  XXXXXXXX, XXXXXXXX, XX__XX__, ___X___X,
  X_XXXXXX, XXXXXXX_, _____XX_, ___X___X,
  X_XXXXXX, XXXXXX_X, _____XX_, X__X___X,
  X__XXXXX, XXXXXXXX, XXX___X_, X_X____X,
  X__XXXXX, XXXXXX__, ___XX_X_, X_X____X,
  X__XXXXX, XXXXX___, ______X_, __X____X,
  X__XXXXX, XXXXXXXX, XXX_____, __X____X,
  X___XXXX, XXXX_XXX, X_XX____, __X____X,
  X___XXXX, XXX_X___, ________, __X____X,
  X____XXX, XXXX_X__, ________, __XX___X,
  X____XXX, XXXXX___, _____X__, _XXXX__X,
  X___XXXX, XXXXXX_X, _X_XX___, _X_XXX_X,
  X__XXXXX, XXXXXXXX, XXX_____, X_XXXX_X,
  X_XXXXX_, XXXXXXX_, X_____XX, X_XXXXXX,
  X_XX_XXX, XXXXXX_X, _X_XXX_X, X_X_XXXX,
  XXXXXXXX, _XXXXXXX, XXX____X, _XXXXXXX,
  XXXXXX_X, X_XXX___, ______X_, XXXXXX_X,
  XXXX_XXX, XXXXXXXX, XXXXXXXX, XX_XXXXX,

  attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_CYAN,
  attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_WHITE,
  attribute_BLACK_OVER_YELLOW, attribute_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BLACK_OVER_WHITE,
  attribute_BRIGHT_BLACK_OVER_CYAN, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_CYAN,
  attribute_BRIGHT_BLACK_OVER_CYAN, attribute_BLACK_OVER_WHITE, attribute_BRIGHT_BLACK_OVER_YELLOW, attribute_BRIGHT_BLACK_OVER_WHITE,
};

/* ----------------------------------------------------------------------- */

/** $C8E2: stage5_lods_C8E2 */
static const bitmap_t stage5_lods_C8E2[6] = {
  { 6, BITMAPFLAG_DEFAULT, 30, &stage5_bitmap_C960[0], &stage5_bitmap_C960[0] },  // [0]
  { 5, BITMAPFLAG_DEFAULT, 22, &stage5_bitmap_CA14[0], &stage5_bitmap_CA14[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 15, &stage5_bitmap_CA82[0], &stage5_bitmap_CA82[0] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 15, &stage5_bitmap_CA82[0], &stage5_bitmap_CA82[0] },  // [3]
  { 3, BITMAPFLAG_MASKED, 8, &stage5_bitmap_CAAF[0], &stage5_bitmap_CAAF[0] },  // [4]
  { 3, BITMAPFLAG_MASKED, 8, &stage5_bitmap_CAAF[0], &stage5_bitmap_CADF[0] },  // [5]
};

/** $C90C: stage5_lods_C90C */
static const bitmap_t stage5_lods_C90C[6] = {
  { 6, BITMAPFLAG_DEFAULT, 39, &stage5_bitmap_CB0F[0], &stage5_bitmap_CB0F[0] },  // [0]
  { 5, BITMAPFLAG_DEFAULT, 29, &stage5_bitmap_CBF9[0], &stage5_bitmap_CBF9[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 20, &stage5_bitmap_CC8A[0], &stage5_bitmap_CC8A[0] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 20, &stage5_bitmap_CC8A[0], &stage5_bitmap_CC8A[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 12, &stage5_bitmap_CCC6[0], &stage5_bitmap_CCF6[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 12, &stage5_bitmap_CCC6[0], &stage5_bitmap_CCF6[0] },  // [5]
};

/** $C936: stage5_lods_C936 */
static const bitmap_t stage5_lods_C936[6] = {
  { 6, BITMAPFLAG_DEFAULT, 30, &stage5_bitmap_CD26[0], &stage5_bitmap_CD26[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 20, &stage5_bitmap_CDDA[0], &stage5_bitmap_CDDA[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 13, &stage5_bitmap_CE2A[0], &stage5_bitmap_CE2A[0] },  // [2]
  { 3, BITMAPFLAG_DEFAULT, 13, &stage5_bitmap_CE2A[0], &stage5_bitmap_CE2A[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 8, &stage5_bitmap_CE51[0], &stage5_bitmap_CE51[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 8, &stage5_bitmap_CE51[0], &stage5_bitmap_CE71[0] },  // [5]
};

/**
 * $C960: stage5_bitmap_C960
 */
static const u8 stage5_bitmap_C960[6 * 30] = {
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  _XXXXXX_, _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, __XXXXXX,
  _XXXX__X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX__XXXX,
  __XXX_XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXX_,
  ___XXXX_, X_X_X_XX, X_X_X_X_, X_X_X_X_, XXX_X_X_, X_XXXX__,
  ___X_X__, _____XXX, XXXXXXXX, XXXXXXXX, XXXX____, ___X_X__,
  __X___X_, X_XXXXXX, _XX_XXXX, XXXXXXX_, XX_XXXX_, X_X___X_,
  __X_XXXX, XXXXXXX_, _X__XXX_, X_X_XX__, X__XXXXX, XXXXX_X_,
  __XXXXXX, XXXXXXXX, XXXXXX_X, _X_X_XXX, XXXXXXXX, XXXXXXX_,
  __XXXXXX, XXXX_XXX, X_X__X__, ____X_X_, XXXXXXXX, XXXXXXX_,
  __XX_XXX, __X_X_X_, X_______, ________, X_XXXXXX, XXXXXXX_,
  __X_X_X_, _X_X_XXX, ________, ________, _XX____X, XXXXXXX_,
  __X___X_, XXXXXXXX, _X_X_X_X, _X_X_X_X, _X_XXXX_, _XXXXXX_,
  __X__X_X, XX____XX, X_X_X_X_, X_X_X_X_, X_X____X, X__XX_X_,
  _X_X_XXX, __X_X__X, XXXXXXXX, XXXXXXXX, XX__X_X_, _XX_XX_X,
  _XX_XX__, _______X, __X_X_X_, X_X_X_X_, _X______, ___XX_XX,
  _XXX_X__, _______X, __XX_X_X, _X_X_XX_, _X______, ___XXXXX,
  _XXX_XXX, _X_X_X_X, XXXXXXXX, XXXXXXXX, XX_X_X_X, _XXXXXXX,
  __XXX__X, XX____XX, X_______, ________, XXX____X, XX_XXXX_,
  ____X___, _XXXXXXX, XXX_X_X_, X_X_X_XX, XXXXXXXX, ____X___,
  ____XX_X, ______XX, XXXXXXXX, XXXXXXXX, XXX_____, _X_XX___,
  ______XX, X_XX___X, _X_X_X_X, _X_X_X_X, _X___XX_, XXX_____,
  __XXX___, XXXXXXXX, XXX_____, ______XX, XXXXXXXX, X___XXX_,
  _X__XX__, _XXX_X_X, _XXXXXXX, XXXXXXXX, _X_X_XXX, ___XX__X,
  _X_XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X,
  _X__X___, ________, ________, ________, ________, ____X__X,
  _XX__XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX__XX,
  ___XXX__, ________, ________, ________, ________, ___XXX__,
};

/**
 * $CA14: stage5_bitmap_CA14
 */
static const u8 stage5_bitmap_CA14[5 * 22] = {
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  __XX__X_, XXXXXXXX, XXXXXXXX, XXXXXXX_, X__XXX__,
  __XX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XX___,
  ___XX___, ___XXXXX, XXXXXXXX, XXXX____, __XX____,
  __X__X_X, _XXXX_XX, _XXXXXXX, _XX_XX_X, _X__X___,
  __X_XXXX, XXXX__X_, _XX__XX_, _X__XXXX, XXX_X___,
  __XXXXXX, _X_XXXXX, XX____XX, XXXXXXXX, XXXXX___,
  __XX_XX_, __X__X_X, _______X, _X_XXXXX, XXXXX___,
  __X_X___, _XXXX___, ________, __XXXXXX, XXXXX___,
  __X____X, X___XX_X, _X_X_X_X, _XX___XX, X_X_X___,
  ___X_XX_, __X_XXXX, XXXXXXXX, XXX_X___, XX_X_X__,
  __XX_X__, ____X__X, _X_X_X_X, __X_____, _XXXXX__,
  __XX_XXX, ___XXXXX, XXXXXXXX, XXXX___X, XXXXXX__,
  ____X___, XXXXXXX_, X_X_X_X_, XXXXXXX_, ___X____,
  _____XX_, ____XXXX, XXXXXXXX, XXXX____, _XX_____,
  ______XX, _XX_XXXX, _X__X__X, _XXX_XX_, XX______,
  __X__X__, XX_XX_XX, XXXXXXXX, XX_XX_XX, __X__X__,
  __X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_X__,
  __XX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XX__,
  ___XX___, ________, ________, ________, __XX____,
};

/**
 * $CA82: stage5_bitmap_CA82
 */
static const u8 stage5_bitmap_CA82[3 * 15] = {
  _XXXXXXX, XXXXXXXX, XXXXXXX_,
  XXX__XXX, XXXXXXXX, XXX__XXX,
  XX_XXXXX, XXXXXXXX, XXXXX_XX,
  _XX___XX, XXXXXXXX, XX___XX_,
  XX_XXXXX, _X_XX_X_, XXXXX_X_,
  XXXXXXXX, XXX__XXX, XXXXXXXX,
  XX__XXXX, ________, _XXXXXXX,
  X__XX__X, XXXXXXXX, X__XXXXX,
  X_X____X, ________, X____XXX,
  X_XXX_XX, XXXXXXXX, XX_XXXXX,
  _XX__XX_, XXXXXXXX, _XXX_XX_,
  ___X___X, X_X__X_X, X_XXX___,
  XXX_X_X_, XXXXXXXX, XXXX_XXX,
  X_XXXXXX, XXXXXXXX, XXXXXX_X,
  _XX_____, ________, _____XX_,
};

/**
 * $CAAF: stage5_bitmap_CAAF
 */
static const u8 stage5_bitmap_CAAF[3 * 2 * 8] = {
  ________, XXXXXXXX, _______X, XXXXXXX_, XXXXXXXX, ________,
  X_______, _XXXXXXX, ______XX, XXXXXX__, XXXXXXXX, ________,
  ________, XX_XXX_X, _______X, _X_XXXX_, XXXXXXXX, ________,
  ________, X_XX_XXX, _______X, XXX_XXX_, XXXXXXXX, ________,
  ________, X_X__XX_, _______X, X_X__XX_, XXXXXXXX, ________,
  X_______, _X_XXXXX, ______XX, XXXXXX__, XXXXXXXX, ________,
  _X______, X_XXX_X_, _____X_X, X_XXX_X_, XXXXXXXX, ________,
  ________, XXXXXXXX, _______X, XXXXXXX_, XXXXXXXX, ________,
};

/**
 * $CADF: stage5_bitmap_CADF
 */
static const u8 stage5_bitmap_CADF[3 * 2 * 8] = {
  XXXX____, ____XXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
  XXXXX___, _____XXX, ________, XXXXXXXX, __XXXXXX, XX______,
  XXXX____, ____XX_X, ________, XX_X_X_X, ___XXXXX, XXX_____,
  XXXX____, ____X_XX, ________, _XXXXXX_, ___XXXXX, XXX_____,
  XXXX____, ____X_X_, ________, _XX_X_X_, ___XXXXX, _XX_____,
  XXXXX___, _____X_X, ________, XXXXXXXX, __XXXXXX, XX______,
  XXXX_X__, ____X_XX, ________, X_X_X_XX, _X_XXXXX, X_X_____,
  XXXX____, ____XXXX, ________, XXXXXXXX, ___XXXXX, XXX_____,
};

/**
 * $CB0F: stage5_bitmap_CB0F
 */
static const u8 stage5_bitmap_CB0F[6 * 39] = {
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  _XXXXX__, X_X_XXXX, XXXXXXXX, XXXXXXXX, XXXX_X_X, __XXXXX_,
  _XXXX__X, _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, X__XXXX_,
  _XXXX_XX, XXXXXXXX, XX______, _______X, XXXXXXXX, XX_XXXX_,
  __XXX_XX, XXXXXXXX, XX______, _______X, XXXXXXXX, XX_XXX__,
  __XXX_XX, XXXXX___, XXX_____, ______XX, X___XXXX, XX_XXX__,
  ___XX_XX, XXXX_X_X, XX_X_X_X, _X_X_X_X, XX_X_XXX, XX_XX___,
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  ___X____, ________, ________, ________, ________, ____X___,
  __X_X_X_, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, _X_X_X__,
  __XX_X_X, _X___X_X, _X_X_X_X, _X_X_X_X, _X_X__X_, X_X_XX__,
  __X_XXX_, ________, ________, ________, ________, _XXX_X__,
  __XXXX__, ________, ________, ________, ________, __XXXX__,
  _X___XX_, ________, ________, ________, ________, _XX___X_,
  _X___X__, ________, ________, ________, ________, __X___X_,
  _X___XX_, ________, ________, ________, ________, _XX___X_,
  __XXXXX_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_XXXXX_,
  _X___XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX___X_,
  _X______, ________, ________, ________, ________, ______X_,
  _X______, ________, ________, ________, ________, ______X_,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  ___XXXX_, _XX_X_X_, X_X_X_XX, X_X_X_X_, X_X_X_X_, _XXXX___,
  ______XX, X_XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX_X, XX______,
  ______XX, XX______, XX_____X, X_____XX, ______XX, XX______,
  _____XX_, __XXXXXX, __XXXXX_, _XXXXX__, XXXXXX__, _XX_____,
  ____X_XX, X_X_X_XX, __X_X_X_, _X_X_X__, XX_X_X_X, XX_X____,
  ____X__X, _X_XXXXX, __XXXXX_, _XXXXX__, XXXXX_X_, X__X____,
  ___X___X, _X_XXXXX, __XXXXX_, _XXXXX__, XXXXX_X_, X___X___,
  ___X___X, _X_XXXXX, __XXXXX_, _XXXXX__, XXXXX_X_, X___X___,
  ___X___X, __X_XXXX, __XXXXX_, _XXXXX__, XXXX_X__, X___X___,
  ___X___X, X_X_XXXX, __XXXXX_, _XXXXX__, XXXX_X_X, X___X___,
  ____X_X_, _XX_XXXX, __XXXXX_, _XXXXX__, XXXX_XX_, _X_X____,
  _____XXX, X__X_XXX, __XXXXX_, _XXXXX__, XXX_X__X, XXX_____,
  ________, _XXX____, ________, ________, ____XXX_, ________,
  ________, ____X___, ________, ________, ___X____, ________,
  ________, _____XXX, XXXXXXXX, XXXXXXXX, XXX_____, ________,
};

/**
 * $CBF9: stage5_bitmap_CBF9
 */
static const u8 stage5_bitmap_CBF9[5 * 29] = {
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  ___XXX_X, _XXXXXXX, XXXXXXXX, XXXXXXX_, X_XXXX__,
  ___XX_XX, XXXXXXXX, ________, _XXXXXXX, XX_XXX__,
  ___XX_XX, XXXX__XX, ________, _XX__XXX, XX_XX___,
  ____X_XX, XXX_XXXX, X_X___X_, XXXXX_XX, XX_X____,
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  ____X___, ________, ________, ________, ___X____,
  ___X_XX_, X_XXXXXX, XXXXXXXX, XXXXXX_X, _XX_X___,
  ___X_X_X, _X______, ________, ______X_, X_X_X___,
  ___XXX__, ________, ________, ________, __XXX___,
  __X___X_, ________, ________, ________, _X___X__,
  __X___X_, ________, ________, ________, _X___X__,
  __XXXXX_, X_X_X_X_, X_X__X_X, _X_X_X_X, __XXXX__,
  __X_____, ________, ________, ________, _____X__,
  __X_____, ________, ________, ________, _____X__,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  ______XX, X_XXXXXX, XXXXXXXX, XXXXXX_X, XX______,
  ______XX, XX____X_, ___XX___, _X____XX, XX______,
  _____XXX, X__X_X_X, X_X__X_X, X_X_X__X, XXX_____,
  _____X_X, __XXXX_X, XXX__XXX, X_XXXX__, X_X_____,
  ____X__X, __XXXX_X, XXX__XXX, X_XXXX__, X__X____,
  ____X__X, _X_XXX_X, XXX__XXX, X_XXX_X_, X__X____,
  ____X__X, XX_XXX_X, XXX__XXX, X_XXX_XX, X__X____,
  ______XX, X_X_XX_X, XXX__XXX, X_XX_X_X, XX______,
  ________, _XX_____, ________, _____XX_, ________,
  ________, ___X____, ________, ____X___, ________,
  ________, ____XXXX, XXXXXXXX, XXXX____, ________,
};

/**
 * $CC8A: stage5_bitmap_CC8A
 */
static const u8 stage5_bitmap_CC8A[3 * 20] = {
  _XXXXXXX, XXXXXXXX, XXXXXXX_,
  XXX_XXXX, XXXXXXXX, XXXX_XXX,
  XX_XXXXX, X______X, XXXXX_XX,
  _X_XXXX_, XX____XX, _XXXX_X_,
  __XXXXXX, XXXXXXXX, XXXXXX__,
  _X______, ________, ______X_,
  _XXXXXXX, XXXXXXXX, XXXXXXXX,
  X_X_X___, ________, ___X_X_X,
  X_X_____, ________, _____X_X,
  _XX_X_X_, X_X_X_X_, X_X_XXX_,
  X_______, ________, _______X,
  _XXXXXXX, XXXXXXXX, XXXXXXX_,
  ___XXX__, ________, ___XX___,
  ___X___X, X_XX_XX_, XX___X__,
  __X_XX_X, X_XX_XX_, XX_XX_X_,
  __X_X__X, X_XX_XX_, XX__X_X_,
  __X_X__X, X_XX_XX_, XX__X_X_,
  ___XXXX_, X_XX_XX_, X_XXXX__,
  _______X, ________, _X______,
  ________, XXXXXXXX, X_______,
};

/**
 * $CCC6: stage5_bitmap_CCC6
 */
static const u8 stage5_bitmap_CCC6[2 * 2 * 12] = {
  ________, XXXXXXXX, _____XXX, XXXXX___,
  ________, XX_XXXXX, _____XXX, XX_XX___,
  ________, XXXXX___, _____XXX, XXXXX___,
  ________, X_X_X_X_, _____XXX, X_X_X___,
  ________, X_______, _____XXX, ____X___,
  ________, X_______, _____XXX, ____X___,
  ________, XXX_X_X_, _____XXX, X_XXX___,
  X_______, _X_X_X_X, ____XXXX, _X_X____,
  ________, X__X_X_X, _____XXX, _X__X___,
  ________, XX_X_X_X, _____XXX, _X_XX___,
  X_______, _XX_____, ____XXXX, __XX____,
  XXX_____, ___XXXXX, __XXXXXX, XX______,
};

/**
 * $CCF6: stage5_bitmap_CCF6
 */
static const u8 stage5_bitmap_CCF6[2 * 2 * 12] = {
  XXX_____, ___XXXXX, ________, XXXXXXXX,
  XXX_____, ___XX_XX, ________, XXXXX_XX,
  XXX_____, ___XXXXX, ________, ___XXXXX,
  XXX_____, ___X_X_X, ________, _X_X_X_X,
  XXX_____, ___X____, ________, _______X,
  XXX_____, ___X____, ________, _______X,
  XXX_____, ___XXX_X, ________, _X_X_XXX,
  XXXX____, ____X_X_, _______X, X_X_X_X_,
  XXX_____, ___X__X_, ________, X_X_X__X,
  XXX_____, ___XX_X_, ________, X_X_X_XX,
  XXXX____, ____XX__, _______X, _____XX_,
  XXXXXX__, ______XX, _____XXX, XXXXX___,
};

/**
 * $CD26: stage5_bitmap_CD26
 */
static const u8 stage5_bitmap_CD26[6 * 30] = {
  _____XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX____,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  __XXXXX_, _X_X_XXX, XXXXXXXX, XXXXXXXX, XXXX_X_X, __XXXXX_,
  _XXXXX_X, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XX_XXXXX,
  _XXXXX_X, __X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, _X_XXXXX,
  _XXXXX__, _X_X____, ___XXXXX, XXXXXXX_, _____X_X, ___XXXXX,
  __XXX_XX, XXXXXXXX, XXXX____, ______XX, XXXXXXXX, XXX_XXX_,
  __XX_X_X, _X_X_X_X, _X_X____, ______XX, _X_X_X_X, _X_X_XX_,
  _XX_X_X_, ________, ___X____, ______X_, ________, __X_X_XX,
  _X______, ________, ___XXXXX, XXXXXXX_, ________, _______X,
  _X______, ________, ________, ________, ________, _______X,
  _X____X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, X_X____X,
  __X_XXXX, XXXXXXXX, _______X, XX______, _XXXXXXX, XXXXX_X_,
  __X_X__X, XXXX_XXX, ______XX, X_______, _XXX_XXX, XX__X_X_,
  __X_X_XX, X_X_X_XX, _______X, XX______, _XX_X_X_, XXX_X_X_,
  __X_X__X, XX_X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X_X, XX__X_X_,
  __X__X_X, X_X___XX, XXXXXXXX, XXXXXXXX, XXX___X_, XX_X__X_,
  ___X__XX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX__X__,
  ___X____, ________, ________, ________, ________, _____X__,
  ___X__X_, X_______, ________, ________, ________, __X__X__,
  ___X_X__, __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_, ___X_X__,
  ___X_XXX, __X_X_X_, X_X_X_X_, X_X_X_X_, X_X_X_X_, _XXX_X__,
  ___XXX_X, X__X_X_X, _X_X_X_X, _X_X_X_X, _X_X_X__, XX_XXX__,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  ___X____, ________, ________, ________, ________, _____X__,
  ___X____, ________, ________, ________, ________, _____X__,
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX___,
  ________, ____X___, ________, ________, ____X___, ________,
  ________, _____XX_, ________, ________, __XX____, ________,
  ________, _______X, XXXXXXXX, XXXXXXXX, XX______, ________,
};

/**
 * $CDDA: stage5_bitmap_CDDA
 */
static const u8 stage5_bitmap_CDDA[4 * 20] = {
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  _XXX__X_, XXXXXXXX, XXXXXXX_, X__XXXX_,
  XXX_XXXX, XXXXXXXX, XXXXXXXX, XXX_XXXX,
  XXXXX_X_, X_X_XXXX, XXX_X_X_, X_X_XXXX,
  _X_X_X_X, _X_XX___, __XX_X_X, _X_X__X_,
  X_______, ____X___, __X_____, _______X,
  X_______, ________, ________, _______X,
  X__XXX_X, X_XX_XX_, XX_XX_XX, _XX_X__X,
  X_X_XXXX, XXX_____, ____XXXX, XXXX_X_X,
  X_X_X_X_, _XX_____, ____XX__, X_XX_X_X,
  X__XX_X_, _XXXXXXX, XXXXXX__, X_XXX__X,
  _X__XXXX, XXXXXXXX, XXXXXXXX, XXXX__X_,
  _X______, ________, ________, ______X_,
  _XX___XX, XXXXXXXX, XXXXXXXX, X____XX_,
  _XXXX__X, X_XX_XX_, XX_XX_XX, X__XXXX_,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXX_,
  _X______, ________, ________, ______X_,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  ________, XX______, _____XX_, ________,
  ________, __XXXXXX, XXXXX___, ________,
};

/**
 * $CE2A: stage5_bitmap_CE2A
 */
static const u8 stage5_bitmap_CE2A[3 * 13] = {
  ___XXXXX, XXXXXXXX, XXXXXX__,
  _XXX___X, XXXXXXXX, XX___XX_,
  _XX_X_X_, X_XXXXX_, X_X_X_XX,
  _X______, ________, _______X,
  _X___XXX, XXXXXXXX, XXXX___X,
  _X_XXXXX, X___X___, XXXXXX_X,
  _X__X___, X___X___, X___X__X,
  _X_XXXXX, XXXXXXXX, XXXXXX_X,
  __XX____, ________, ______X_,
  __XXX__X, XXXXXXXX, XX__XXX_,
  __X__XXX, _X_X_X_X, _XXX__X_,
  ___XXXXX, XXXXXXXX, XXXXXX__,
  _______X, XXXXXXXX, XX______,
};

/**
 * $CE51: stage5_bitmap_CE51
 */
static const u8 stage5_bitmap_CE51[2 * 2 * 8] = {
  ________, XXXXXXXX, _____XXX, XXXXX___,
  ________, XX__XXXX, _____XXX, X__XX___,
  X_______, _XX_X_X_, ____XXXX, X_XX____,
  ________, X_XXX___, _____XXX, XXX_X___,
  ________, XXXXXXXX, _____XXX, XXXXX___,
  ________, X_______, _____XXX, ____X___,
  X_______, _XX_X_X_, ____XXXX, X_XX____,
  XX______, __XXXXXX, __XXXXXX, XX______,
};

/**
 * $CE71: stage5_bitmap_CE71
 */
static const u8 stage5_bitmap_CE71[2 * 2 * 8] = {
  XXX_____, ___XXXXX, ________, XXXXXXXX,
  XXX_____, ___XX__X, ________, XXXX__XX,
  XXXX____, ____XX_X, _______X, _X_X_XX_,
  XXX_____, ___X_XXX, ________, ___XXX_X,
  XXX_____, ___XXXXX, ________, XXXXXXXX,
  XXX_____, ___X____, ________, _______X,
  XXXX____, ____XX_X, _______X, _X_X_XX_,
  XXXXX___, _____XXX, _____XXX, XXXXX___,
};

/** $CE91: stage5_stretchy_CE91 */
static const stretchy_t stage5_stretchy_CE91[5] = {
  { STRETCHY_TYPE_FIXED, &stage5_depthset_CE9E },
  { STRETCHY_TYPE_113PC, &stage5_depthset_CEB4 },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_CECA },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_CEE0 },
  { STRETCHY_TYPE_END, NULL },
};


/** $CE9E: stage5_depthset_CE9E */
static const depthset_t stage5_depthset_CE9E = {
  &stage5_lods_CF5B[0],
  {
    0x00, 0x17,
    0x00, 0x17,
    0x00, 0x33,
    0x00, 0x33,
    0x00, 0x4F,
    0x00, 0x4F,
    0xF8, 0x6B,
    0xF8, 0x6B,
    0xF8, 0x87,
    0xF8, 0x87,
  }
};


/** $CEB4: stage5_depthset_CEB4 */
static const depthset_t stage5_depthset_CEB4 = {
  &stage5_lods_CF5B[0],
  {
    0x10, 0x10,
    0x10, 0x10,
    0x10, 0x2C,
    0x10, 0x2C,
    0x10, 0x48,
    0x10, 0x48,
    0x08, 0x64,
    0x08, 0x64,
    0x08, 0x80,
    0x08, 0x80,
  }
};


/** $CECA: stage5_depthset_CECA */
static const depthset_t stage5_depthset_CECA = {
  &stage5_lods_CF5B[0],
  {
    0x08, 0x09,
    0x08, 0x09,
    0x10, 0x25,
    0x10, 0x25,
    0x08, 0x41,
    0x08, 0x41,
    0x00, 0x5D,
    0x00, 0x5D,
    0x00, 0x79,
    0x00, 0x79,
  }
};


/** $CEE0: stage5_depthset_CEE0 */
static const depthset_t stage5_depthset_CEE0 = {
  &stage5_lods_CF5B[0],
  {
    0x18, 0x02,
    0x18, 0x02,
    0x18, 0x1E,
    0x18, 0x1E,
    0x10, 0x3A,
    0x10, 0x3A,
    0x08, 0x56,
    0x08, 0x56,
    0x08, 0x72,
    0x08, 0x72,
  }
};


/** $CEF6: stage5_stretchy_CEF6 */
static const stretchy_t stage5_stretchy_CEF6[5] = {
  { STRETCHY_TYPE_FIXED, &stage5_depthset_CF03 },
  { STRETCHY_TYPE_113PC, &stage5_depthset_CF19 },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_CF2F },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_CF45 },
  { STRETCHY_TYPE_END, NULL },
};


/** $CF03: stage5_depthset_CF03 */
static const depthset_t stage5_depthset_CF03 = {
  &stage5_lods_CFE7[0],
  {
    0x08, 0x17,
    0x08, 0x17,
    0x08, 0x33,
    0x08, 0x33,
    0x00, 0x4F,
    0x00, 0x4F,
    0x00, 0x6B,
    0x00, 0x6B,
    0x00, 0x87,
    0x00, 0x87,
  }
};


/** $CF19: stage5_depthset_CF19 */
static const depthset_t stage5_depthset_CF19 = {
  &stage5_lods_CFE7[0],
  {
    0x10, 0x10,
    0x10, 0x10,
    0x10, 0x2C,
    0x10, 0x2C,
    0x08, 0x48,
    0x08, 0x48,
    0x08, 0x64,
    0x08, 0x64,
    0x08, 0x80,
    0x08, 0x80,
  }
};


/** $CF2F: stage5_depthset_CF2F */
static const depthset_t stage5_depthset_CF2F = {
  &stage5_lods_CFE7[0],
  {
    0x10, 0x09,
    0x10, 0x09,
    0x18, 0x25,
    0x18, 0x25,
    0x08, 0x41,
    0x08, 0x41,
    0x08, 0x5D,
    0x08, 0x5D,
    0x08, 0x79,
    0x08, 0x79,
  }
};


/** $CF45: stage5_depthset_CF45 */
static const depthset_t stage5_depthset_CF45 = {
  &stage5_lods_CFE7[0],
  {
    0x20, 0x02,
    0x20, 0x02,
    0x20, 0x1E,
    0x20, 0x1E,
    0x10, 0x3A,
    0x10, 0x3A,
    0x10, 0x56,
    0x10, 0x56,
    0x10, 0x72,
    0x10, 0x72,
  }
};


/** $CF5B: stage5_lods_CF5B */
static const bitmap_t stage5_lods_CF5B[20] = {
  { 5, BITMAPFLAG_FLIPPED, 8, &stage5_bitmap_D073[0], &stage5_bitmap_D073[0] },  // [0]
  { 7, BITMAPFLAG_FLIPPED, 8, &stage5_bitmap_D09B[0], &stage5_bitmap_D09B[0] },  // [1]
  { 7, BITMAPFLAG_DEFAULT, 8, &stage5_bitmap_D1F0[0], &stage5_bitmap_D1F0[0] },  // [2]
  { 8, BITMAPFLAG_FLIPPED, 8, &stage5_bitmap_D10B[0], &stage5_bitmap_D10B[0] },  // [3]
  { 3, BITMAPFLAG_FLIPPED, 4, &stage5_bitmap_D14B[0], &stage5_bitmap_D14B[0] },  // [4]
  { 4, BITMAPFLAG_FLIPPED, 4, &stage5_bitmap_D157[0], &stage5_bitmap_D157[0] },  // [5]
  { 5, BITMAPFLAG_DEFAULT, 6, &stage5_bitmap_D228[0], &stage5_bitmap_D228[0] },  // [6]
  { 6, BITMAPFLAG_FLIPPED, 6, &stage5_bitmap_D18A[0], &stage5_bitmap_D18A[0] },  // [7]
  { 2, BITMAPFLAG_FLIPPED, 4, &stage5_bitmap_D1AE[0], &stage5_bitmap_D1AE[0] },  // [8]
  { 3, BITMAPFLAG_FLIPPED, 3, &stage5_bitmap_D1B6[0], &stage5_bitmap_D1B6[0] },  // [9]
  { 3, BITMAPFLAG_DEFAULT, 4, &stage5_bitmap_D246[0], &stage5_bitmap_D246[0] },  // [10]
  { 4, BITMAPFLAG_FLIPPED, 4, &stage5_bitmap_D1CB[0], &stage5_bitmap_D1CB[0] },  // [11]
  { 1, BITMAPFLAG_FLIPPED, 3, &stage5_bitmap_D1DB[0], &stage5_bitmap_D1DB[0] },  // [12]
  { 2, BITMAPFLAG_FLIPPED, 3, &stage5_bitmap_D1DE[0], &stage5_bitmap_D1DE[0] },  // [13]
  { 2, BITMAPFLAG_DEFAULT, 3, &stage5_bitmap_D252[0], &stage5_bitmap_D252[0] },  // [14]
  { 3, BITMAPFLAG_FLIPPED, 2, &stage5_bitmap_D1EA[0], &stage5_bitmap_D1EA[0] },  // [15]
  { 1, BITMAPFLAG_MASKED|BITMAPFLAG_FLIPPED, 1, &stage5_bitmap_D258[0], &stage5_bitmap_D272[0] },  // [16]
  { 2, BITMAPFLAG_MASKED|BITMAPFLAG_FLIPPED, 1, &stage5_bitmap_D25A[0], &stage5_bitmap_D274[0] },  // [17]
  { 2, BITMAPFLAG_MASKED, 2, &stage5_bitmap_D25E[0], &stage5_bitmap_D278[0] },  // [18]
  { 3, BITMAPFLAG_MASKED|BITMAPFLAG_FLIPPED, 2, &stage5_bitmap_D266[0], &stage5_bitmap_D280[0] },  // [19]
};

/** $CFE7: stage5_lods_CFE7 */
static const bitmap_t stage5_lods_CFE7[20] = {
  { 5, BITMAPFLAG_DEFAULT, 8, &stage5_bitmap_D073[0], &stage5_bitmap_D073[0] },  // [0]
  { 7, BITMAPFLAG_DEFAULT, 8, &stage5_bitmap_D09B[0], &stage5_bitmap_D09B[0] },  // [1]
  { 7, BITMAPFLAG_DEFAULT, 8, &stage5_bitmap_D0D3[0], &stage5_bitmap_D0D3[0] },  // [2]
  { 8, BITMAPFLAG_DEFAULT, 8, &stage5_bitmap_D10B[0], &stage5_bitmap_D10B[0] },  // [3]
  { 3, BITMAPFLAG_DEFAULT, 4, &stage5_bitmap_D14B[0], &stage5_bitmap_D14B[0] },  // [4]
  { 4, BITMAPFLAG_DEFAULT, 4, &stage5_bitmap_D157[0], &stage5_bitmap_D157[0] },  // [5]
  { 5, BITMAPFLAG_DEFAULT, 6, &stage5_bitmap_D16C[0], &stage5_bitmap_D16C[0] },  // [6]
  { 6, BITMAPFLAG_DEFAULT, 6, &stage5_bitmap_D18A[0], &stage5_bitmap_D18A[0] },  // [7]
  { 2, BITMAPFLAG_DEFAULT, 4, &stage5_bitmap_D1AE[0], &stage5_bitmap_D1AE[0] },  // [8]
  { 3, BITMAPFLAG_DEFAULT, 3, &stage5_bitmap_D1B6[0], &stage5_bitmap_D1B6[0] },  // [9]
  { 3, BITMAPFLAG_DEFAULT, 4, &stage5_bitmap_D1BF[0], &stage5_bitmap_D1BF[0] },  // [10]
  { 4, BITMAPFLAG_DEFAULT, 4, &stage5_bitmap_D1CB[0], &stage5_bitmap_D1CB[0] },  // [11]
  { 1, BITMAPFLAG_DEFAULT, 3, &stage5_bitmap_D1DB[0], &stage5_bitmap_D1DB[0] },  // [12]
  { 2, BITMAPFLAG_DEFAULT, 3, &stage5_bitmap_D1DE[0], &stage5_bitmap_D1DE[0] },  // [13]
  { 2, BITMAPFLAG_DEFAULT, 3, &stage5_bitmap_D1E4[0], &stage5_bitmap_D1E4[0] },  // [14]
  { 3, BITMAPFLAG_DEFAULT, 2, &stage5_bitmap_D1EA[0], &stage5_bitmap_D1EA[0] },  // [15]
  { 1, BITMAPFLAG_MASKED, 1, &stage5_bitmap_D258[0], &stage5_bitmap_D272[0] },  // [16]
  { 2, BITMAPFLAG_MASKED, 1, &stage5_bitmap_D25A[0], &stage5_bitmap_D274[0] },  // [17]
  { 2, BITMAPFLAG_MASKED, 2, &stage5_bitmap_D25E[0], &stage5_bitmap_D278[0] },  // [18]
  { 3, BITMAPFLAG_MASKED, 2, &stage5_bitmap_D266[0], &stage5_bitmap_D280[0] },  // [19]
};

/**
 * $D073: stage5_bitmap_D073
 */
static const u8 stage5_bitmap_D073[5 * 8] = {
  XXXXXXXX, XXXXXXXX, XXXXXXX_, XXXXXXXX, _X_XXXX_,
  XX_XXXXX, X_XXXXXX, _XXXXXXX, XXXXX_XX, __XX____,
  XXXX_XXX, _XXXXXX_, XXXXXX_X, XXXX_X__, _XX_____,
  X_X_X_XX, XXXXXXXX, _XXXX__X, XX_XXXXX, XX______,
  XXXX_XXX, XX_XXXX_, XXXX__XX, __XX____, ________,
  XX_X__X_, XXXXXX_X, X_X_XXXX, XX______, ________,
  XXXXX___, _XXX___X, XXXXX___, ________, ________,
  XXXXXXXX, XXXXXXX_, ________, ________, ________,
};

/**
 * $D09B: stage5_bitmap_D09B
 */
static const u8 stage5_bitmap_D09B[7 * 8] = {
  X_X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXXX, _XXXX___, XX__XX__,
  X__XXXXX, _XXXXX_X, XXXXXXX_, XXXXXXX_, XXXX_XXX, _XX_XX_X, X__X____,
  X_XXXXX_, XXXXXXXX, XXXXX_XX, XXXXXXXX, XXX_XXX_, _XXX___X, XXX_____,
  X__XXXXX, _XXXXXXX, _XXXXXX_, XXXXXXX_, XXXX_X_X, _XX__XX_, ________,
  XX__XXX_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXX_, XXXXX___, ________,
  XX_XXXXX, XXXXXX_X, XXXXXX_X, XXXXX_XX, XXX_XX__, XX______, ________,
  X_X_XXXX, X_XXXXXX, _XXXXXXX, XXXXXXX_, XX_XXXX_, XX______, ________,
  XX_XXXX_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XX_X, X_______, ________,
};

/**
 * $D0D3: stage5_bitmap_D0D3
 */
static const u8 stage5_bitmap_D0D3[7 * 8] = {
  X_X_XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXXX, _XXXXXX_, _XXXX_XX,
  X__XXXXX, _XXXXX_X, XXXXXXX_, XXXXXXX_, XXXX_XXX, _XX_XX__, _XXX___X,
  X_XXXXX_, XXXXXXXX, XXXXX_XX, XXXXXXXX, XXX_XXX_, _XXXXXX_, _XX_X__X,
  X__XXXXX, _XXXXXXX, _XXXXXX_, XXXXXXX_, XXXX_X_X, _XXXX_X_, _XXX___X,
  XX__XXX_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XXX_, XXXX____, _XX____X,
  XX_XXXXX, XXXXXX_X, XXXXXX_X, XXXXX_XX, XXX_XX__, XX_XXX__, XXX_X_X_,
  X_X_XXXX, X_XXXXXX, _XXXXXXX, XXXXXXX_, XX_XXXX_, XXXXX_X_, XX_X__X_,
  XX_XXXX_, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_XX_X, XX_X_X__, XXX___X_,
};

/**
 * $D10B: stage5_bitmap_D10B
 */
static const u8 stage5_bitmap_D10B[8 * 8] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X__XXXXX, X_XXXXXX, XXX_XXX_, X_XXX_X_, X_XX_X_X, ____XXXX, __XX__XX, _XX_X__X,
  X__XXX_X, XX_XXXX_, XXXXXXXX, _XXXXX__, XXXXX_X_, ___XXXX_, _XX_X_XX, XX_X___X,
  XX__X_XX, XXXXXXXX, XXXXXXXX, XXXXX_X_, XX_X____, _XXX_X__, XXXX__XX, _XX___X_,
  XX___X_X, XX_XX_XX, _XXXXXX_, XXXX_X_X, XXXXX___, XXXXX_X_, XX___XX_, X_XXX_X_,
  X_X___XX, X_XXXXXX, XXXXXX_X, XXXXX__X, XX_X___X, X_XXXX__, XXX__XXX, _____XX_,
  X_XXXXXX, XXXXXXX_, XXXX_XXX, XXXX__XX, XXX___XX, _XX_X__X, __XXXX__, __XXXX__,
  X__XXXXX, XXXXXXXX, XXXXXXXX, XX__XXXX, XXXXXXX_, X_XX__X_, _XXX___X, XX______,
};

/**
 * $D14B: stage5_bitmap_D14B
 */
static const u8 stage5_bitmap_D14B[3 * 4] = {
  XX_XXXXX, XXXX_XXX, _XX_XXXX,
  XX__XXX_, XX__XX__, XX_XX___,
  X_X___XX, XXXXXXXX, XXX_____,
  XXXXXXXX, XXX_____, ________,
};

/**
 * $D157: stage5_bitmap_D157
 */
static const u8 stage5_bitmap_D157[4 * 4] = {
  X_XXXXXX, XX_XXXXX, _XXXXXXX, XX_XX_XX,
  X_XXX_XX, XXXXXX_X, XXXXXXXX, X__X_XX_,
  XXXXXXXX, XXXXXXXX, X_XXXXXX, __XXX___,
  X_X_XXXX, _XXX__XX, XXXXXX__, _XX_____,
};

/**
 * $D16C: stage5_bitmap_D16C
 */
static const u8 stage5_bitmap_D16C[5 * 6] = {
  XX_XX_XX, XXXXXXXX, XXXXXXXX, XX_X_X_X, X_X__XX_,
  X_XXX_XX, XX_XXXXX, XXXXXXX_, XXX__X_X, _X___X_X,
  X_XXXX_X, XXXXXXXX, X_XXXXX_, XXX_XX_X, XXX__X_X,
  X__XX_XX, XX_XXXXX, XXXXXXXX, XX_XX_XX, X____X_X,
  X_XXXX_X, XXX_XXXX, _XXXXX_X, XX_X__X_, XX__XXX_,
  X_XXXX_X, XXXXXXXX, XXXXXXXX, XX_X_XX_, XX_XXX__,
};

/**
 * $D18A: stage5_bitmap_D18A
 */
static const u8 stage5_bitmap_D18A[6 * 6] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_XXXXXX, XXXXXX_X, X_XXXXX_, _XXX_X__, XXX__XX_, XXXXX__X,
  X__XXXXX, XXXXXXXX, XXXXXX__, X_X____X, _X_XXX__, XX_X__X_,
  XX_X_XX_, XXXX_XXX, X_XXX__X, XX_X__X_, __X_X_X_, X_X_X_X_,
  XXX___XX, XXXXXXXX, _XXXXX_X, X___XXXX, XX__XX_X, ___XXX__,
  X_XXXXXX, XXXXXXXX, XXXX__XX, XXXXX_XX, X_X__XX_, _XX_____,
};

/**
 * $D1AE: stage5_bitmap_D1AE
 */
static const u8 stage5_bitmap_D1AE[2 * 4] = {
  X_XXX_XX, _XXXXX_X,
  XX_XXXXX, _XX_XXX_,
  XX__XXX_, XXXXX___,
  XXXXXXXX, ________,
};

/**
 * $D1B6: stage5_bitmap_D1B6
 */
static const u8 stage5_bitmap_D1B6[3 * 3] = {
  X_XX_XX_, XXXXXXXX, _X_XXX__,
  X_XX_XXX, _XXXXX__, XX_XX___,
  X_XXXXX_, XXX_XXX_, X_XX____,
};

/**
 * $D1BF: stage5_bitmap_D1BF
 */
static const u8 stage5_bitmap_D1BF[3 * 4] = {
  X_XX_XX_, XXXXXXXX, XX___X_X,
  X_XX_XXX, _XXXXXX_, X__X_X_X,
  X_XXX_XX, _XX_XX_X, X_X_XX_X,
  X_XXX_XX, XXXXXXXX, X_X_X_X_,
};

/**
 * $D1CB: stage5_bitmap_D1CB
 */
static const u8 stage5_bitmap_D1CB[4 * 4] = {
  X_XXXXXX, XXXXXXX_, XX_X__XX, __X_X_X_,
  X__XXX_X, XXXXXX_X, X_X__X__, _XXXXXX_,
  XX__XXXX, _XX_XXX_, X___XXX_, XXX__X__,
  X_XXXXXX, XXXXX_XX, XXXXXX_X, _X__X___,
};

/**
 * $D1DB: stage5_bitmap_D1DB
 */
static const u8 stage5_bitmap_D1DB[1 * 3] = {
  XXX_XX_X,
  XXXX_XX_,
  XX______,
};

/**
 * $D1DE: stage5_bitmap_D1DE
 */
static const u8 stage5_bitmap_D1DE[2 * 3] = {
  XXXXXXXX, XXXX_X__,
  XXXXX_X_, X_XXX___,
  XX_XXXXX, XXX_____,
};

/**
 * $D1E4: stage5_bitmap_D1E4
 */
static const u8 stage5_bitmap_D1E4[2 * 3] = {
  XXXXXXXX, XXXXXX_X,
  XXXXX_X_, XXX__X_X,
  XX_XXXXX, XX_XX_X_,
};

/**
 * $D1EA: stage5_bitmap_D1EA
 */
static const u8 stage5_bitmap_D1EA[3 * 2] = {
  XXX_XXXX, XX_XX_XX, _X_X____,
  X_XXXX_X, X_X__XX_, XXX_____,
};

/**
 * $D1F0: stage5_bitmap_D1F0
 */
static const u8 stage5_bitmap_D1F0[7 * 8] = {
  XX_XXXX_, _XXXXXX_, XXXX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXX_X_X,
  X___XXX_, __XX_XX_, XXX_XXXX, _XXXXXXX, _XXXXXXX, X_XXXXX_, XXXXX__X,
  X__X_XX_, _XXXXXX_, _XXX_XXX, XXXXXXXX, XX_XXXXX, XXXXXXXX, _XXXXX_X,
  X___XXX_, _X_XXXX_, X_X_XXXX, _XXXXXXX, _XXXXXX_, XXXXXXX_, XXXXX__X,
  X____XX_, ____XXXX, _XXX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _XXX__XX,
  _X_X_XXX, __XXX_XX, __XX_XXX, XX_XXXXX, X_XXXXXX, X_XXXXXX, XXXXX_XX,
  _X__X_XX, _X_XXXXX, _XXXX_XX, _XXXXXXX, XXXXXXX_, XXXXXX_X, XXXX_X_X,
  _X___XXX, __X_X_XX, X_XX_XXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, _XXXX_XX,
};

/**
 * $D228: stage5_bitmap_D228
 */
static const u8 stage5_bitmap_D228[5 * 6] = {
  _XX__X_X, X_X_X_XX, XXXXXXXX, XXXXXXXX, XX_XX_XX,
  X_X___X_, X_X__XXX, _XXXXXXX, XXXXX_XX, XX_XXX_X,
  X_X__XXX, X_XX_XXX, _XXXXX_X, XXXXXXXX, X_XXXX_X,
  X_X____X, XX_XX_XX, XXXXXXXX, XXXXX_XX, XX_XX__X,
  _XXX__XX, _X__X_XX, X_XXXXX_, XXXX_XXX, X_XXXX_X,
  __XXX_XX, _XX_X_XX, XXXXXXXX, XXXXXXXX, X_XXXX_X,
};

/**
 * $D246: stage5_bitmap_D246
 */
static const u8 stage5_bitmap_D246[3 * 4] = {
  X_X___XX, XXXXXXXX, _XX_XX_X,
  X_X_X__X, _XXXXXX_, XXX_XX_X,
  X_XX_X_X, X_XX_XX_, XX_XXX_X,
  _X_X_X_X, XXXXXXXX, XX_XXX_X,
};

/**
 * $D252: stage5_bitmap_D252
 */
static const u8 stage5_bitmap_D252[2 * 3] = {
  X_XXXXXX, XXXXXXXX,
  X_X__XXX, _X_XXXXX,
  _X_XX_XX, XXXXX_XX,
};

/**
 * $D258: stage5_bitmap_D258
 */
static const u8 stage5_bitmap_D258[1 * 2 * 1] = {
  ____XXXX, XXXX____,
};

/**
 * $D25A: stage5_bitmap_D25A
 */
static const u8 stage5_bitmap_D25A[2 * 2 * 1] = {
  ________, XX_X_XXX, XXXXXXXX, ________,
};

/**
 * $D25E: stage5_bitmap_D25E
 */
static const u8 stage5_bitmap_D25E[2 * 2 * 2] = {
  ________, X_XXX_XX, ___XXXXX, X_X_____,
  ________, XXXXXXXX, __XXXXXX, _X______,
};

/**
 * $D266: stage5_bitmap_D266
 */
static const u8 stage5_bitmap_D266[3 * 2 * 2] = {
  ________, XX_XXXXX, ______XX, XX_X_X__, XXXXXXXX, ________,
  ________, XXXXXXX_, _____XXX, X_X_X___, XXXXXXXX, ________,
};

/**
 * $D272: stage5_bitmap_D272
 */
static const u8 stage5_bitmap_D272[1 * 2 * 1] = {
  XXXX____, ____XXXX,
};

/**
 * $D274: stage5_bitmap_D274
 */
static const u8 stage5_bitmap_D274[2 * 2 * 1] = {
  XXXX____, ____XX_X, ____XXXX, _XXX____,
};

/**
 * $D278: stage5_bitmap_D278
 */
static const u8 stage5_bitmap_D278[2 * 2 * 2] = {
  XXXX____, ____X_XX, _______X, X_XXX_X_,
  XXXX____, ____XXXX, ______XX, XXXX_X__,
};

/**
 * $D280: stage5_bitmap_D280
 */
static const u8 stage5_bitmap_D280[3 * 2 * 2] = {
  XXXX____, ____XX_X, ________, XXXXXX_X, __XXXXXX, _X______,
  XXXX____, ____XXXX, ________, XXX_X_X_, _XXXXXXX, X_______,
};

/** $D28C: stage5_stretchy_D28C */
static const stretchy_t stage5_stretchy_D28C[3] = {
  { STRETCHY_TYPE_150PC, &stage5_depthset_D293 },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D2A9 },
  { STRETCHY_TYPE_END, NULL },
};


/** $D293: stage5_depthset_D293 */
static const depthset_t stage5_depthset_D293 = {
  &stage5_lods_D2BF[0],
  {
    0x24, 0x02,
    0x1C, 0x02,
    0x18, 0x09,
    0x18, 0x09,
    0x14, 0x10,
    0x14, 0x10,
    0x14, 0x17,
    0x10, 0x17,
    0x10, 0x17,
    0x10, 0x17,
  }
};


/** $D2A9: stage5_depthset_D2A9 */
static const depthset_t stage5_depthset_D2A9 = {
  &stage5_lods_D2BF[0],
  {
    0x24, 0x1E,
    0x1C, 0x1E,
    0x18, 0x25,
    0x18, 0x25,
    0x14, 0x2C,
    0x14, 0x2C,
    0x14, 0x33,
    0x10, 0x33,
    0x10, 0x33,
    0x10, 0x33,
  }
};


/** $D2BF: stage5_lods_D2BF */
static const bitmap_t stage5_lods_D2BF[8] = {
  { 4, BITMAPFLAG_DEFAULT, 16, &stage5_bitmap_D438[0], &stage5_bitmap_D438[0] },  // [0]
  { 3, BITMAPFLAG_DEFAULT, 12, &stage5_bitmap_D547[0], &stage5_bitmap_D547[0] },  // [1]
  { 2, BITMAPFLAG_DEFAULT, 8, &stage5_bitmap_D5D5[0], &stage5_bitmap_D5D5[0] },  // [2]
  { 1, BITMAPFLAG_DEFAULT, 4, &stage5_bitmap_D60E[0], &stage5_bitmap_D60E[0] },  // [3]
  { 4, BITMAPFLAG_DEFAULT, 24, &stage5_bitmap_D478[0], &stage5_bitmap_D478[0] },  // [4]
  { 3, BITMAPFLAG_DEFAULT, 18, &stage5_bitmap_D56B[0], &stage5_bitmap_D56B[0] },  // [5]
  { 2, BITMAPFLAG_DEFAULT, 12, &stage5_bitmap_D5E5[0], &stage5_bitmap_D5E5[0] },  // [6]
  { 1, BITMAPFLAG_DEFAULT, 6, &stage5_bitmap_D612[0], &stage5_bitmap_D612[0] },  // [7]
};

/** $D2F7: stage5_stretchy_D2F7 */
static const stretchy_t stage5_stretchy_D2F7[3] = {
  { STRETCHY_TYPE_150PC, &stage5_depthset_D2FE },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D332 },
  { STRETCHY_TYPE_END, NULL },
};


/** $D2FE: stage5_depthset_D2FE */
static const depthset_t stage5_depthset_D2FE = {
  &stage5_lods_D348[0],
  {
    0x24, 0x02,
    0x1C, 0x02,
    0x18, 0x09,
    0x18, 0x09,
    0x14, 0x10,
    0x14, 0x10,
    0x14, 0x17,
    0x10, 0x17,
    0x10, 0x17,
    0x10, 0x17,
  },
  &stage5_overhead_spans_D314[0]
};


/**
 * $D314: overhead-bridge span table (Conv: modelled explicitly; see
 * overhead_span_t in Stages.h). Each entry's fill_bytes points into
 * stage5_bitmap_D380, which is defined further below.
 */
static const overhead_span_t stage5_overhead_spans_D314[10] = {
  { 0x18, &stage5_bitmap_D420[0] },
  { 0x18, &stage5_bitmap_D420[0] },
  { 0x12, &stage5_bitmap_D535[0] },
  { 0x12, &stage5_bitmap_D535[0] },
  { 0x0C, &stage5_bitmap_D5C9[0] },
  { 0x0C, &stage5_bitmap_D5C9[0] },
  { 0x07, &stage5_bitmap_D607[0] },
  { 0x07, &stage5_bitmap_D607[0] },
  { 0x07, &stage5_bitmap_D607[0] },
  { 0x07, &stage5_bitmap_D607[0] },
};

/** $D332: stage5_depthset_D332 */
static const depthset_t stage5_depthset_D332 = {
  &stage5_lods_D348[0],
  {
    0x24, 0x1E,
    0x1C, 0x1E,
    0x18, 0x25,
    0x18, 0x25,
    0x14, 0x2C,
    0x14, 0x2C,
    0x14, 0x33,
    0x10, 0x33,
    0x10, 0x33,
    0x10, 0x33,
  }
};


/** $D348: stage5_lods_D348 */
static const bitmap_t stage5_lods_D348[8] = {
  { 4, BITMAPFLAG_DEFAULT, 16, &stage5_bitmap_D380[0], &stage5_bitmap_D380[0] },  // [0]
  { 3, BITMAPFLAG_DEFAULT, 12, &stage5_bitmap_D4D8[0], &stage5_bitmap_D4D8[0] },  // [1]
  { 2, BITMAPFLAG_DEFAULT, 8, &stage5_bitmap_D5A1[0], &stage5_bitmap_D5A1[0] },  // [2]
  { 1, BITMAPFLAG_DEFAULT, 4, &stage5_bitmap_D5FD[0], &stage5_bitmap_D5FD[0] },  // [3]
  { 4, BITMAPFLAG_DEFAULT, 24, &stage5_bitmap_D3C0[0], &stage5_bitmap_D3C0[0] },  // [4]
  { 3, BITMAPFLAG_DEFAULT, 18, &stage5_bitmap_D4FC[0], &stage5_bitmap_D4FC[0] },  // [5]
  { 2, BITMAPFLAG_DEFAULT, 12, &stage5_bitmap_D5B1[0], &stage5_bitmap_D5B1[0] },  // [6]
  { 1, BITMAPFLAG_DEFAULT, 6, &stage5_bitmap_D601[0], &stage5_bitmap_D601[0] },  // [7]
};

/**
 * $D380: stage5_bitmap_D380
 */
static const u8 stage5_bitmap_D380[4 * 16] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ________, ________, __X__XXX,
  X____XXX, ________, ___XXX__, __X____X,
  X__XX___, XXXXXXXX, XXX___XX, __X_X__X,
  X__X_X__, _XXX____, _X___XXX, __X_XX_X,
  X__X_XXX, __XXX__X, X__XXXXX, __X__X_X,
  X__X_XXX, X___XXX_, __XXXX_X, __X__X_X,
  X__X_XXX, _XX__X__, XXXXX__X, __X_XX_X,
  X__X_XXX, ___X___X, XXXX___X, __X_XX_X,
  X__X_XXX, _XX__X__, XXXXX__X, __X_XX_X,
  X__X_XXX, X___XXX_, __XXXX_X, __X__X_X,
  X__X_XXX, __XXXX_X, X__XXXXX, __X__X_X,
  X__X_X__, _XXXX___, _X___XXX, __X_XX_X,
  X__XX___, XXXXXXXX, XXX___XX, __X__X_X,
  X_______, ________, ________, __X____X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXX__X,
};

/**
 * $D3C0: stage5_bitmap_D3C0
 */
static const u8 stage5_bitmap_D3C0[4 * 24] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ________, ________, __X__XXX,
  X____XXX, ________, ___XXX__, __X___XX,
  X__XX___, XXXXXXXX, XXX___XX, __X_XXXX,
  X__X_X__, _XXX____, _X___XXX, __X_XXXX,
  X__X_XXX, __XXX__X, X__XXXXX, __X_X_X_,
  X__X_XXX, X___XXX_, __XXXX_X, __X_XXXX,
  X__X_XXX, _XX__X__, XXXXX__X, __X_XX_X,
  X__X_XXX, ___X___X, XXXX___X, __X_X___,
  X__X_XXX, _XX__X__, XXXXX__X, __X_XXXX,
  X__X_XXX, X___XXX_, __XXXX_X, __X_X_X_,
  X__X_XXX, __XXXX_X, X__XXXXX, __X_X___,
  X__X_X__, _XXXX___, _X___XXX, __X_X___,
  X__XX___, XXXXXXXX, XXX___XX, __X_XX__,
  X____XXX, ________, ___XXX__, __XX_XX_,
  X_______, ________, ________, ____XX_X,
  X______X, XXXXXXXX, XXXXXXXX, XX____XX,
  _XXXX___, ___XXXXX, XXXX____, __X_____,
  _____XXX, X______X, XXXXXXXX, X_X_____,
  ________, _XXXX___, ___XXXXX, XXX____X,
  ________, _____XXX, X______X, XXX____X,
  ________, ________, _XXXX___, __X____X,
  ________, ________, _____XXX, X_______,
  ________, ________, ________, _XXXXXXX,
};

/**
 * $D420: stage5_bitmap_D420
 */
static const u8 stage5_bitmap_D420[24] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_X_X_, XXXXXXXX, _X_X_X_X,
  ________, XXXXXXXX, X_X_X_X_, ________, ________, ________, ________, ________,
  XXXXXXXX, ________, ________, XXXXXXXX, ________, XXXXXXXX, ________, XXXXXXXX,
};

/**
 * $D438: stage5_bitmap_D438
 */
static const u8 stage5_bitmap_D438[4 * 16] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX__X__, ________, ________, _______X,
  X____X__, __XXX___, ________, XXX____X,
  X__X_X__, XX___XXX, XXXXXXXX, ___XX__X,
  X_XX_X__, XXX___X_, ____XXX_, __X_X__X,
  X_X__X__, XXXXX__X, X__XXX__, XXX_X__X,
  X_X__X__, X_XXXX__, _XXX___X, XXX_X__X,
  X_XX_X__, X__XXXXX, __X__XX_, XXX_X__X,
  X_XX_X__, X___XXXX, X___X___, XXX_X__X,
  X_XX_X__, X__XXXXX, __X__XX_, XXX_X__X,
  X_X__X__, X_XXXX__, _XXX___X, XXX_X__X,
  X_X__X__, XXXXX__X, X_XXXX__, XXX_X__X,
  X_XX_X__, XXX___X_, ___XXXX_, __X_X__X,
  X_X__X__, XX___XXX, XXXXXXXX, ___XX__X,
  X____X__, ________, ________, _______X,
  X__XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
};

/**
 * $D478: stage5_bitmap_D478
 */
static const u8 stage5_bitmap_D478[4 * 24] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXX__X__, ________, ________, _______X,
  XX___X__, __XXX___, ________, XXX____X,
  XXXX_X__, XX___XXX, XXXXXXXX, ___XX__X,
  XXXX_X__, XXX___X_, ____XXX_, __X_X__X,
  X_XX_X__, XXXXX__X, X__XXX__, XXX_X__X,
  XXXX_X__, X_XXXX__, _XXX___X, XXX_X__X,
  _X_X_X__, X__XXXXX, __X__XX_, XXX_X__X,
  ___X_X__, X___XXXX, X___X___, XXX_X__X,
  XXXX_X__, X__XXXXX, __X__XX_, XXX_X__X,
  X_XX_X__, X_XXXX__, _XXX___X, XXX_X__X,
  ___X_X__, XXXXX__X, X_XXXX__, XXX_X__X,
  ___X_X__, XXX___X_, ___XXXX_, __X_X__X,
  __XX_X__, XX___XXX, XXXXXXXX, ___XX__X,
  _XX_XX__, __XXX___, ________, XXX____X,
  X_XX____, ________, ________, _______X,
  XX____XX, XXXXXXXX, XXXXXXXX, X______X,
  _____X__, ____XXXX, XXXXX___, ___XXXX_,
  _____X_X, XXXXXXXX, X______X, XXX_____,
  X____XXX, XXXXX___, ___XXXX_, ________,
  X____XXX, X______X, XXX_____, ________,
  X____X__, ___XXXX_, ________, ________,
  _______X, XXX_____, ________, ________,
  XXXXXXX_, ________, ________, ________,
};

/**
 * $D4D8: stage5_bitmap_D4D8
 */
static const u8 stage5_bitmap_D4D8[3 * 12] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ________, ___X__XX,
  X_XX__XX, XXXXXX__, XX_X___X,
  X_X_X__X, X___X__X, XX_X_X_X,
  X_X_XX__, XXXX__XX, XX_X___X,
  X_X_XXXX, ____XXXX, _X_X_X_X,
  X_X_XX_X, X__XXXX_, _X_X_X_X,
  X_X_XXXX, ____XXXX, _X_X_X_X,
  X_X_XX__, XXXX__XX, XX_X___X,
  X_XXX__X, XX__X__X, XX_X_X_X,
  X_XX__XX, XXXXXX__, XX_X___X,
  XXXXXXXX, XXXXXXXX, XXXXXX_X,
};

/**
 * $D4FC: stage5_bitmap_D4FC
 */
static const u8 stage5_bitmap_D4FC[3 * 18] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  X_______, ________, ___X__XX,
  X_XX__XX, XXXXXX__, XX_X_XXX,
  X_X_X__X, X___X__X, XX_X_XXX,
  X_X_XX__, XXXX__XX, XX_X_XX_,
  X_X_XXXX, ____XXXX, _X_X_X_X,
  X_X_XX_X, X__XXXX_, _X_X_X__,
  X_X_XXXX, ____XXXX, _X_X_XX_,
  X_X_XX__, XXXX__XX, XX_X_X__,
  X_X_X__X, XX__X__X, XX_X_X__,
  X_XX__XX, XXXXXX__, XX_X_X__,
  X_______, ________, ___X_XX_,
  X____XXX, XXXXXXXX, XXX_X_XX,
  _XXXX___, XXXXXXX_, ___X____,
  ____XXXX, ____XXXX, XXXX___X,
  ________, XXXX____, XXXX___X,
  ________, ____XXXX, ___X____,
  ________, ________, XXXXXXXX,
};

/**
 * $D535: stage5_bitmap_D535
 */
static const u8 stage5_bitmap_D535[18] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_X_X_X_, _X_X_X_X, ________, X_X_X_X_,
  ________, ________, ________, ________, XXXXXXXX, ________, XXXXXXXX, XXXXXXXX,
  ________, XXXXXXXX,
};

/**
 * $D547: stage5_bitmap_D547
 */
static const u8 stage5_bitmap_D547[3 * 12] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XX__X___, ________, _______X,
  X___X_XX, __XXXXXX, XX__XX_X,
  X_X_X_XX, X__X___X, X__X_X_X,
  X___X_XX, XX__XXXX, __XX_X_X,
  X_X_X_X_, XXXX____, XXXX_X_X,
  X_X_X_X_, _XXXX__X, X_XX_X_X,
  X_X_X_X_, XXXX____, XXXX_X_X,
  X___X_XX, XX__XXXX, __XX_X_X,
  X_X_X_XX, X__X__XX, X__XXX_X,
  X___X_XX, __XXXXXX, XX__XX_X,
  X_XXXXXX, XXXXXXXX, XXXXXXXX,
};

/**
 * $D56B: stage5_bitmap_D56B
 */
static const u8 stage5_bitmap_D56B[3 * 18] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XX__X___, ________, _______X,
  XXX_X_XX, __XXXXXX, XX__XX_X,
  XXX_X_XX, X__X___X, X__X_X_X,
  X_X_X_XX, XX__XXXX, __XX_X_X,
  _XX_X_X_, XXXX____, XXXX_X_X,
  __X_X_X_, _XXXX__X, X_XX_X_X,
  X_X_X_X_, XXXX____, XXXX_X_X,
  __X_X_XX, XX__XXXX, __XX_X_X,
  __X_X_XX, X__X__XX, X__X_X_X,
  __X_X_XX, __XXXXXX, XX__XX_X,
  _XX_X___, ________, _______X,
  XX_X_XXX, XXXXXXXX, XXX____X,
  ____X___, _XXXXXXX, ___XXXX_,
  X___XXXX, XXXX____, XXXX____,
  X___XXXX, ____XXXX, ________,
  ____X___, XXXX____, ________,
  XXXXXXXX, ________, ________,
};

/**
 * $D5A1: stage5_bitmap_D5A1
 */
static const u8 stage5_bitmap_D5A1[2 * 8] = {
  X_______, _____X_X,
  X_X_XXXX, XX_X_XX_,
  X_XX__XX, __XX_X__,
  X_XXXX__, XXXX_XX_,
  X_XXXX__, XXXX_XX_,
  X_XX__XX, __XX_X__,
  X_X_XXXX, XX_X_XX_,
  XXXXXXXX, XXXXXXXX,
};

/**
 * $D5B1: stage5_bitmap_D5B1
 */
static const u8 stage5_bitmap_D5B1[2 * 12] = {
  X_______, _____X_X,
  X_X_XXXX, XX_X_X_X,
  X_XX__XX, __XX_X_X,
  X_XXXX__, XXXX_X_X,
  X_XXXX__, XXXX_X__,
  X_XX__XX, __XX_X_X,
  X_X_XXXX, XX_X_X__,
  X__XXXXX, XXXX__X_,
  _XX___XX, XX__XX__,
  ___XXX__, __XXXX_X,
  ______XX, XX___X__,
  ________, __XXXXXX,
};

/**
 * $D5C9: stage5_bitmap_D5C9
 */
static const u8 stage5_bitmap_D5C9[12] = {
  XXXXXXXX, XXXXXXXX, XXXXXXXX, _X_X_X_X, ________, _X_X_X_X, ________, ________,
  ________, XXXXXXXX, ________, XXXXXXXX,
};

/**
 * $D5D5: stage5_bitmap_D5D5
 */
static const u8 stage5_bitmap_D5D5[2 * 8] = {
  X_X_____, _______X,
  _XX_X_XX, XXXX_X_X,
  __X_XX__, XX__XX_X,
  _XX_XXXX, __XXXX_X,
  _XX_XXXX, __XXXX_X,
  __X_XX__, XX__XX_X,
  _XX_X_XX, XXXX_X_X,
  XXXXXXXX, XXXXXXXX,
};

/**
 * $D5E5: stage5_bitmap_D5E5
 */
static const u8 stage5_bitmap_D5E5[2 * 12] = {
  X_X_____, _______X,
  X_X_X_XX, XXXX_X_X,
  X_X_XX__, XX__XX_X,
  _XX_XXXX, __XXXX_X,
  __X_XXXX, __XXXX_X,
  _XX_XX__, XX__XX_X,
  __X_X_XX, XXXX_X_X,
  _X__XXXX, XXXXX__X,
  __XX__XX, XX___XX_,
  X_XXXX__, __XXX___,
  __X___XX, XX______,
  XXXXXX__, ________,
};

/**
 * $D5FD: stage5_bitmap_D5FD
 */
static const u8 stage5_bitmap_D5FD[1 * 4] = {
  X__XX___,
  XXX__XX_,
  X__XX___,
  XXXXXXXX,
};

/**
 * $D601: stage5_bitmap_D601
 */
static const u8 stage5_bitmap_D601[1 * 6] = {
  X__XX___,
  XXX__XX_,
  X__XX___,
  X__XXX__,
  _XX___X_,
  __XXXXXX,
};

/**
 * $D607: stage5_bitmap_D607
 */
static const u8 stage5_bitmap_D607[7] = {
  ________, XXXXXXXX, XXXXXXXX, _X_X_X_X, X_X_X_X_, ________, XXXXXXXX,
};

/**
 * $D60E: stage5_bitmap_D60E
 */
static const u8 stage5_bitmap_D60E[1 * 4] = {
  ___XX__X,
  _XX__XXX,
  ___XX__X,
  XXXXXXXX,
};

/**
 * $D612: stage5_bitmap_D612
 */
static const u8 stage5_bitmap_D612[1 * 6] = {
  ___XX__X,
  _XX__XXX,
  ___XX__X,
  __XXX__X,
  _X___XX_,
  XXXXXX__,
};

/** $D620: stage5_lods_D620 */
static const bitmap_t stage5_lods_D620[6] = {
  { 4, BITMAPFLAG_DEFAULT, 17, &stage5_bitmap_D64A[0], &stage5_bitmap_D64A[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 17, &stage5_bitmap_D64A[0], &stage5_bitmap_D64A[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 13, &stage5_bitmap_D68E[0], &stage5_bitmap_D68E[0] },  // [2]
  { 2, BITMAPFLAG_DEFAULT, 9, &stage5_bitmap_D6B5[0], &stage5_bitmap_D6B5[0] },  // [3]
  { 2, BITMAPFLAG_DEFAULT, 9, &stage5_bitmap_D6B5[0], &stage5_bitmap_D6B5[0] },  // [4]
  { 2, BITMAPFLAG_MASKED, 7, &stage5_bitmap_D6C7[0], &stage5_bitmap_D6E3[0] },  // [5]
};

/**
 * $D64A: stage5_bitmap_D64A
 */
static const u8 stage5_bitmap_D64A[4 * 17] = {
  ___XXXXX, XX______, ______XX, XXXXX___,
  ___XXXX_, _X____XX, XX____XX, _X__X___,
  ____XX_X, __X___XX, _X___XX_, X__X____,
  ____XXX_, X_X___XX, XX___XXX, _X_X____,
  _____XXX, _X_X__XX, _X__XXX_, X_X_____,
  _____XX_, X__X_XX_, X_X_XX_X, __X_____,
  ______XX, _X__XXXX, __XXX_X_, _X______,
  ______XX, X_X_XXX_, X_XXXX_X, _X______,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXXX_X, _XXXXXX_, _XXXXXX_, X_XXXXXX,
  XXXXX___, _XXXXX__, __XXXXX_, ___XXXXX,
  XXXXX___, _XXXXX__, __XXXXX_, ___XXXXX,
  XXXXX___, _XXXXX__, __XXXXX_, ___XXXXX,
  XXXXX___, _XXXXX__, __XXXXX_, ___XXXXX,
  X_X_X___, _X_X_X__, __X_X_X_, ___X_X_X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX,
  ________, ____XXXX, XXXXX___, ________,
};

/**
 * $D68E: stage5_bitmap_D68E
 */
static const u8 stage5_bitmap_D68E[3 * 13] = {
  __XXXXXX, ________, XXXXXX__,
  __XXX_XX, ___XXX__, XX_X_X__,
  ___XXX_X, X__X_X_X, X_X_X___,
  ___XX_X_, X_XX_X_X, XX__X___,
  ____XX_X, X_X_X_XX, X__X____,
  ____XXX_, _XXXXXXX, XX_X____,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  XXXXX_XX, XXXX_XXX, X_X_XXXX,
  XXXX___X, XXX__XXX, X___XXXX,
  XXXX___X, XXX__XXX, X___XXXX,
  X__X___X, __X__X__, X___X__X,
  XXXXXXXX, XXXXXXXX, XXXXXXXX,
  ________, _XXXXXX_, ________,
};

/**
 * $D6B5: stage5_bitmap_D6B5
 */
static const u8 stage5_bitmap_D6B5[2 * 9] = {
  _XXXX___, ___XXXX_,
  _XX_X__X, X__XX_X_,
  __XX_X_X, X_XX_X__,
  __XXXX_X, X_XXXX__,
  XXXXXXXX, XXXXXXXX,
  XXX__XXX, __XXX__X,
  X_X__X_X, __X_X__X,
  XXXXXXXX, XXXXXXXX,
  ______XX, XX______,
};

/**
 * $D6C7: stage5_bitmap_D6C7
 */
static const u8 stage5_bitmap_D6C7[2 * 2 * 7] = {
  X__XXXXX, _XX_____, X__XXXXX, _XX_____,
  X___X__X, _XXX_XX_, ___XXXXX, XXX_____,
  XX______, __XXXXXX, __XXXXXX, XX______,
  ________, XXXXXXXX, ____XXXX, XXXX____,
  ________, X__XX__X, ____XXXX, X__X____,
  ________, XXXXXXXX, ____XXXX, XXXX____,
  XXXX____, ____XXXX, XXXXXXXX, ________,
};

/**
 * $D6E3: stage5_bitmap_D6E3
 */
static const u8 stage5_bitmap_D6E3[2 * 2 * 7] = {
  XXXXX__X, _____XX_, XXXXX__X, _____XX_,
  XXXXX___, _____XXX, X__X___X, _XX_XXX_,
  XXXXXX__, ______XX, ______XX, XXXXXX__,
  XXXX____, ____XXXX, ________, XXXXXXXX,
  XXXX____, ____X__X, ________, X__XX__X,
  XXXX____, ____XXXX, ________, XXXXXXXX,
  XXXXXXXX, ________, ____XXXX, XXXX____,
};

/** $D6FF: stage5_stretchy_D6FF */
static const stretchy_t stage5_stretchy_D6FF[5] = {
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D719 },
  { STRETCHY_TYPE_25PC, &stage5_depthset_D72F },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D75B },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D787 },
  { STRETCHY_TYPE_END, NULL },
};


/** $D70C: stage5_stretchy_D70C */
static const stretchy_t stage5_stretchy_D70C[5] = {
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D719 },
  { STRETCHY_TYPE_25PC, &stage5_depthset_D745 },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D771 },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D79D },
  { STRETCHY_TYPE_END, NULL },
};


/** $D719: stage5_depthset_D719 */
static const depthset_t stage5_depthset_D719 = {
  &stage5_lods_D7B3[0],
  {
    0x1C, 0x02,
    0x14, 0x02,
    0x10, 0x17,
    0x10, 0x17,
    0x0C, 0x2C,
    0x0C, 0x2C,
    0x0C, 0x56,
    0x08, 0x56,
    0x08, 0x56,
    0x08, 0x56,
  }
};


/** $D72F: stage5_depthset_D72F */
static const depthset_t stage5_depthset_D72F = {
  &stage5_lods_D7B3[0],
  {
    0x1C, 0x48,
    0x14, 0x48,
    0x10, 0x1E,
    0x10, 0x1E,
    0x0C, 0x33,
    0x0C, 0x33,
    0x0C, 0x5D,
    0x08, 0x5D,
    0x08, 0x5D,
    0x08, 0x5D,
  }
};


/** $D745: stage5_depthset_D745 */
static const depthset_t stage5_depthset_D745 = {
  &stage5_lods_D7B3[0],
  {
    0x24, 0x48,
    0x1C, 0x48,
    0x18, 0x1E,
    0x18, 0x1E,
    0x0C, 0x33,
    0x0C, 0x33,
    0x14, 0x5D,
    0x10, 0x5D,
    0x10, 0x5D,
    0x10, 0x5D,
  }
};


/** $D75B: stage5_depthset_D75B */
static const depthset_t stage5_depthset_D75B = {
  &stage5_lods_D7B3[0],
  {
    0x1C, 0x09,
    0x14, 0x09,
    0x10, 0x4F,
    0x10, 0x4F,
    0x0C, 0x3A,
    0x0C, 0x3A,
    0x0C, 0x5D,
    0x08, 0x5D,
    0x08, 0x5D,
    0x08, 0x5D,
  }
};


/** $D771: stage5_depthset_D771 */
static const depthset_t stage5_depthset_D771 = {
  &stage5_lods_D7B3[0],
  {
    0x1C, 0x09,
    0x14, 0x09,
    0x10, 0x4F,
    0x10, 0x4F,
    0x0C, 0x3A,
    0x0C, 0x3A,
    0x14, 0x5D,
    0x10, 0x5D,
    0x10, 0x5D,
    0x10, 0x5D,
  }
};


/** $D787: stage5_depthset_D787 */
static const depthset_t stage5_depthset_D787 = {
  &stage5_lods_D7B3[0],
  {
    0x24, 0x10,
    0x1C, 0x10,
    0x10, 0x25,
    0x10, 0x25,
    0x0C, 0x41,
    0x0C, 0x41,
    0x0C, 0x64,
    0x08, 0x64,
    0x08, 0x64,
    0x08, 0x64,
  }
};


/** $D79D: stage5_depthset_D79D */
static const depthset_t stage5_depthset_D79D = {
  &stage5_lods_D7B3[0],
  {
    0x24, 0x10,
    0x1C, 0x10,
    0x18, 0x25,
    0x18, 0x25,
    0x0C, 0x41,
    0x0C, 0x41,
    0x14, 0x64,
    0x10, 0x64,
    0x10, 0x64,
    0x10, 0x64,
  }
};


/** $D7B3: stage5_lods_D7B3 */
static const bitmap_t stage5_lods_D7B3[15] = {
  { 3, BITMAPFLAG_DEFAULT, 4, &stage5_bitmap_D81C[0], &stage5_bitmap_D81C[0] },  // [0]
  { 3, BITMAPFLAG_DEFAULT, 13, &stage5_bitmap_D828[0], &stage5_bitmap_D828[0] },  // [1]
  { 1, BITMAPFLAG_DEFAULT, 7, &stage5_bitmap_D84F[0], &stage5_bitmap_D84F[0] },  // [2]
  { 2, BITMAPFLAG_DEFAULT, 3, &stage5_bitmap_D856[0], &stage5_bitmap_D856[0] },  // [3]
  { 1, BITMAPFLAG_DEFAULT, 2, &stage5_bitmap_D85C[0], &stage5_bitmap_D85C[0] },  // [4]
  { 1, BITMAPFLAG_DEFAULT, 6, &stage5_bitmap_D85E[0], &stage5_bitmap_D85E[0] },  // [5]
  { 1, BITMAPFLAG_DEFAULT, 2, &stage5_bitmap_D864[0], &stage5_bitmap_D864[0] },  // [6]
  { 1, BITMAPFLAG_DEFAULT, 2, &stage5_bitmap_D866[0], &stage5_bitmap_D866[0] },  // [7]
  { 1, BITMAPFLAG_DEFAULT, 5, &stage5_bitmap_D86C[0], &stage5_bitmap_D86C[0] },  // [8]
  { 1, BITMAPFLAG_DEFAULT, 2, &stage5_bitmap_D86C[4], &stage5_bitmap_D86C[4] },  // [9]
  { 2, BITMAPFLAG_MASKED, 3, &stage5_bitmap_D872[0], &stage5_bitmap_D872[0] },  // [10]
  { 2, BITMAPFLAG_MASKED, 9, &stage5_bitmap_D87E[0], &stage5_bitmap_D87E[0] },  // [11]
  { 2, BITMAPFLAG_MASKED, 2, &stage5_bitmap_D8A2[0], &stage5_bitmap_D8BA[0] },  // [12]
  { 1, BITMAPFLAG_MASKED, 2, &stage5_bitmap_D8AA[0], &stage5_bitmap_D8C2[0] },  // [13]
  { 1, BITMAPFLAG_MASKED, 6, &stage5_bitmap_D8AE[0], &stage5_bitmap_D8C6[0] },  // [14]
};

/**
 * $D81C: stage5_bitmap_D81C
 */
static const u8 stage5_bitmap_D81C[3 * 4] = {
  ________, XXXXXX__, XX______,
  ____XXXX, XXXXXXXX, XXXXXX__,
  __XXXX_X, _XXXXXXX, XXXX____,
  ____XX__, __X_XXXX, X_______,
};

/**
 * $D828: stage5_bitmap_D828
 */
static const u8 stage5_bitmap_D828[3 * 13] = {
  __X_X_XX, _X_XX_XX, XXX_____,
  ___X_XXX, ___X_X_X, _XXX____,
  ___X__XX, __XX____, X_XX____,
  ___X_XXX, ___XX_X_, __XXX___,
  __X___XX, X__XXXX_, X__XX___,
  __XX_XXX, __XXXXXX, ___XX___,
  _XX___XX, X__XX_X_, X_X_XX__,
  __XX_XXX, ___XXX__, X__XX___,
  _XX__XXX, _X_X_X_X, X_XXXX__,
  __X__XXX, ___XXX__, X__XX___,
  __XX_XX_, __XXXXXX, X_XXX___,
  _X_X_XXX, ___XXXX_, _XXX____,
  __X_XXX_, X_X_X_XX, __X_____,
};

/**
 * $D84F: stage5_bitmap_D84F
 */
static const u8 stage5_bitmap_D84F[1 * 7] = {
  X___XXX_,
  X___XXXX,
  XX_X_XX_,
  X___XXX_,
  X___XX__,
  XX_XXX__,
  _XXXX_X_,
};

/**
 * $D856: stage5_bitmap_D856
 */
static const u8 stage5_bitmap_D856[2 * 3] = {
  _XXXXXXX, _XX_____,
  XXXX_XXX, XXXXXX__,
  _XX____X, XX______,
};

/**
 * $D85C: stage5_bitmap_D85C
 */
static const u8 stage5_bitmap_D85C[1 * 2] = {
  __X__XXX,
  _XX_X_XX,
};

/**
 * $D85E: stage5_bitmap_D85E
 */
static const u8 stage5_bitmap_D85E[1 * 6] = {
  XXX__XXX,
  _XX_X_XX,
  __X__XXX,
  _X_X__X_,
  ___X_XX_,
  __X_XX__,
};

/**
 * $D864: stage5_bitmap_D864
 */
static const u8 stage5_bitmap_D864[1 * 2] = {
  _XXXX_XX,
  XXXXXXX_,
};

/**
 * $D866: stage5_bitmap_D866
 */
static const u8 stage5_bitmap_D866[1 * 2 + 4] = {
  __X_XX__,
  __XX_X__,
  // $D868: 4 further bytes, not reached by any LOD entry
  _XX__XX_, _XX_X_XX, _XX_X__X, _XX_XX_X,
};

/**
 * $D86C: stage5_bitmap_D86C
 */
static const u8 stage5_bitmap_D86C[6] = {
  _XX_XX_X, _XX_XXX_, _XX_XX__, __X_XX__, __XX_X__, ___XX___,
};

/**
 * $D872: stage5_bitmap_D872
 */
static const u8 stage5_bitmap_D872[2 * 2 * 3] = {
  XXXXX___, _____XX_, ______X_, _X__XX_X,
  XXXX_X__, ____X_X_, _______X, ___XXXX_,
  XXXXX___, _____XXX, ______XX, __X_XX__,
};

/**
 * $D87E: stage5_bitmap_D87E
 */
static const u8 stage5_bitmap_D87E[2 * 2 * 9] = {
  X_______, _XX_XXXX, __XXXXXX, XX______,
  ________, XXX__XX_, ___XXXXX, X_X_____,
  ________, XXX_XX__, ____XXXX, _XXX____,
  ________, XXX__XX_, ____XXXX, __XX____,
  ________, XXX___XX, _____XXX, X__XX___,
  ________, XXX___XX, ____XXXX, X_XX____,
  ________, X_X_X_XX, _____XXX, ___XX___,
  ________, X_X__XXX, ____XXXX, __XX____,
  ________, X_X_X_XX, X__XXXXX, _XX_____,
};

/**
 * $D8A2: stage5_bitmap_D8A2
 */
static const u8 stage5_bitmap_D8A2[2 * 2 * 2] = {
  ______X_, XXXXXX_X, XXXXXXXX, ________,
  ____XXXX, XXXX____, XXXXXXXX, ________,
};

/**
 * $D8AA: stage5_bitmap_D8AA
 */
static const u8 stage5_bitmap_D8AA[1 * 2 * 2] = {
  X__XXXXX, _XX_____,
  X__XXXXX, _XX_____,
};

/**
 * $D8AE: stage5_bitmap_D8AE
 */
static const u8 stage5_bitmap_D8AE[1 * 2 * 6] = {
  ___XXXXX, XXX_____,
  ____XXXX, XXXX____,
  ____XXXX, XXXX____,
  ___XXXXX, XXX_____,
  X__XXXXX, _XX_____,
  X__XXXXX, _XX_____,
};

/**
 * $D8BA: stage5_bitmap_D8BA
 */
static const u8 stage5_bitmap_D8BA[2 * 2 * 2] = {
  XXXX____, ____XXXX, __X_XXXX, XX_X____,
  XXXX____, ____XXXX, XXXXXXXX, ________,
};

/**
 * $D8C2: stage5_bitmap_D8C2
 */
static const u8 stage5_bitmap_D8C2[1 * 2 * 2] = {
  XXXXX__X, _____XX_,
  XXXXX__X, _____XX_,
};

/**
 * $D8C6: stage5_bitmap_D8C6
 */
static const u8 stage5_bitmap_D8C6[1 * 2 * 6] = {
  XXXX___X, ____XXX_,
  XXXX____, ____XXXX,
  XXXX____, ____XXXX,
  XXXX___X, ____XXX_,
  XXXXX__X, _____XX_,
  XXXXX__X, _____XX_,
};

/** $D8D2: stage5_stretchy_D8D2 */
static const stretchy_t stage5_stretchy_D8D2[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_right },
  { STRETCHY_TYPE_50PC, &streetlampmiddle2_right },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_right },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D8EC },
  { STRETCHY_TYPE_END, NULL },
};


/** $D8DF: stage5_stretchy_D8DF */
static const stretchy_t stage5_stretchy_D8DF[5] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_left },
  { STRETCHY_TYPE_50PC, &streetlampmiddle2_left },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_left },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D902 },
  { STRETCHY_TYPE_END, NULL },
};


/** $D8EC: stage5_depthset_D8EC */
static const depthset_t stage5_depthset_D8EC = {
  &stage5_lods_D918[0],
  {
    0x18, 0x02,
    0x10, 0x02,
    0x08, 0x09,
    0x08, 0x09,
    0x0C, 0x10,
    0x0C, 0x10,
    0x08, 0x17,
    0x08, 0x17,
    0x04, 0x1E,
    0x04, 0x1E,
  }
};


/** $D902: stage5_depthset_D902 */
static const depthset_t stage5_depthset_D902 = {
  &stage5_lods_D918[0],
  {
    0x18, 0x02,
    0x10, 0x02,
    0x10, 0x09,
    0x10, 0x09,
    0x0C, 0x10,
    0x0C, 0x10,
    0x08, 0x17,
    0x08, 0x17,
    0x0C, 0x1E,
    0x0C, 0x1E,
  }
};


/** $D918: stage5_lods_D918 */
static const bitmap_t stage5_lods_D918[5] = {
  { 5, BITMAPFLAG_DEFAULT, 8, &stage5_bitmap_D93B[0], &stage5_bitmap_D93B[0] },  // [0]
  { 4, BITMAPFLAG_DEFAULT, 6, &stage5_bitmap_D963[0], &stage5_bitmap_D963[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 4, &stage5_bitmap_D97B[0], &stage5_bitmap_D987[0] },  // [2]
  { 3, BITMAPFLAG_MASKED, 3, &stage5_bitmap_D993[0], &stage5_bitmap_D9A5[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 2, &stage5_bitmap_D9B7[0], &stage5_bitmap_D9C3[0] },  // [4]
};

/**
 * $D93B: stage5_bitmap_D93B
 */
static const u8 stage5_bitmap_D93B[5 * 8] = {
  __XXXXX_, ________, XXX_XXX_, ________, _XXXXX__,
  _XX___XX, _______X, X_X_X_XX, ________, XX___XX_,
  _X_____X, _____XXX, __X_X__X, XX______, X_____X_,
  _X_____X, __XXXX__, XX___XX_, _XXXX___, X_____X_,
  _XX_X_XX, XXX___XX, _X___X_X, X___XXXX, XX_X_XX_,
  XXXXXXX_, ___XXX__, _XX_XX__, _XXX____, XXXXXXXX,
  X______X, XXX_____, __XXX___, ____XXXX, _______X,
  _XXXXXX_, ________, ________, ________, XXXXXXX_,
};

/**
 * $D963: stage5_bitmap_D963
 */
static const u8 stage5_bitmap_D963[4 * 6] = {
  ____XXXX, _______X, X_XX____, ___XXXX_,
  ___XX__X, X____XX_, X_X_XX__, __XX__XX,
  ___X____, X_XXX__X, X__X__XX, X_X____X,
  ____X_XX, XXX__XX_, X__XXX__, XXXXX_X_,
  ___X_X_X, ___XX___, _XX___XX, ___X_X_X,
  ____XXXX, XXX_____, ________, XXXXXXX_,
};

/**
 * $D97B: stage5_bitmap_D97B
 */
static const u8 stage5_bitmap_D97B[3 * 4] = {
  _X_X___X, XXXXX___, X_X_____,
  X___XXX_, X__X_XXX, ___X____,
  XXXX____, _XX_____, XXXX____,
  _XXX____, ________, XXX_____,
};

/**
 * $D987: stage5_bitmap_D987
 */
static const u8 stage5_bitmap_D987[3 * 4] = {
  _____X_X, ___XXXXX, X___X_X_,
  ____X___, XXX_X__X, _XXX___X,
  ____XXXX, _____XX_, ____XXXX,
  _____XXX, ________, ____XXX_,
};

/**
 * $D993: stage5_bitmap_D993
 */
static const u8 stage5_bitmap_D993[3 * 2 * 3] = {
  XX___XX_, __X_X__X, ____XX__, XXXX__X_, _XXXXXXX, X_______,
  XX______, ____XXX_, _____X__, X_X_X_X_, _XXXXXXX, ________,
  XX_____X, __XXX___, XXXX____, ______XX, _XXXXXXX, X_______,
};

/**
 * $D9A5: stage5_bitmap_D9A5
 */
static const u8 stage5_bitmap_D9A5[3 * 2 * 3] = {
  XXXXXX__, ______X_, _XX_____, X__XXXXX, XX___XXX, __X_X___,
  XXXXXX__, ________, ________, XXX_X_X_, _X___XXX, X_X_____,
  XXXXXX__, ______XX, ___XXXXX, X_______, _____XXX, __XXX___,
};

/**
 * $D9B7: stage5_bitmap_D9B7
 */
static const u8 stage5_bitmap_D9B7[2 * 2 * 2 + 4] = {
  X_XXXX_X, _X____X_, XXX_XXXX, ___X____,
  ______X_, X_XXXX_X, _____XXX, XXX_X___,
  // $D9BF: 4 further bytes, not reached by any LOD entry
  ___XXXXX, XXX_____, XX___XXX, __XXX___,
};

/**
 * $D9C3: stage5_bitmap_D9C3
 */
static const u8 stage5_bitmap_D9C3[2 * 2 * 2 + 4] = {
  XXXXX_XX, _____X__, XX_XXX_X, __X___X_,
  XXXX____, ____X_XX, __X_____, XX_XXX_X,
  // $D9CB: 4 further bytes, not reached by any LOD entry
  XXXX___X, ____XXX_, XXXXXX__, ______XX,
};

/** $D9CF: stage5_stretchy_D9CF */
static const stretchy_t stage5_stretchy_D9CF[4] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_right },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_right },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D9F9 },
  { STRETCHY_TYPE_END, NULL },
};


/** $D9D9: stage5_stretchy_D9D9 */
static const stretchy_t stage5_stretchy_D9D9[4] = {
  { STRETCHY_TYPE_FIXED, &streetlampbottom_left },
  { STRETCHY_TYPE_113PC, &streetlampmiddle_left },
  { STRETCHY_TYPE_FIXED, &stage5_depthset_D9E3 },
  { STRETCHY_TYPE_END, NULL },
};


/** $D9E3: stage5_depthset_D9E3 */
static const depthset_t stage5_depthset_D9E3 = {
  &stage5_lods_DA0F[0],
  {
    0x20, 0x02,
    0x18, 0x02,
    0x10, 0x09,
    0x10, 0x09,
    0x0C, 0x10,
    0x0C, 0x10,
    0x10, 0x17,
    0x10, 0x17,
    0x0C, 0x1E,
    0x0C, 0x1E,
  }
};


/** $D9F9: stage5_depthset_D9F9 */
static const depthset_t stage5_depthset_D9F9 = {
  &stage5_lods_DA0F[0],
  {
    0x20, 0x02,
    0x18, 0x02,
    0x10, 0x09,
    0x10, 0x09,
    0x0C, 0x10,
    0x0C, 0x10,
    0x08, 0x17,
    0x08, 0x17,
    0x04, 0x1E,
    0x04, 0x1E,
  }
};


/** $DA0F: stage5_lods_DA0F */
static const bitmap_t stage5_lods_DA0F[5] = {
  { 3, BITMAPFLAG_DEFAULT, 13, &stage5_bitmap_DA32[0], &stage5_bitmap_DA32[0] },  // [0]
  { 3, BITMAPFLAG_DEFAULT, 10, &stage5_bitmap_DA59[0], &stage5_bitmap_DA59[0] },  // [1]
  { 3, BITMAPFLAG_DEFAULT, 7, &stage5_bitmap_DA77[0], &stage5_bitmap_DA8C[0] },  // [2]
  { 2, BITMAPFLAG_MASKED, 5, &stage5_bitmap_DAA1[0], &stage5_bitmap_DAB5[0] },  // [3]
  { 2, BITMAPFLAG_MASKED, 4, &stage5_bitmap_DAC9[0], &stage5_bitmap_DAD9[0] },  // [4]
};

/**
 * $DA32: stage5_bitmap_DA32
 */
static const u8 stage5_bitmap_DA32[3 * 13] = {
  ____XXXX, XXX__XXX, XXX_____,
  ___X____, ___X_XX_, ___X____,
  ___XXXXX, XXX__XXX, XXXX____,
  ____X_X_, _X___X__, X_X_____,
  ____X_X_, _XX__X__, X_X_____,
  _____X__, _XX__X__, _X______,
  _XXXXXXX, XXXX_XXX, XXXXXX__,
  X_______, ___X_XX_, X_____X_,
  XXXXXXXX, XXX__XXX, XXXXXXX_,
  _X_XX_X_, _X___X__, X_XX_X__,
  _X_XX_X_, _XX__X__, X_XX_X__,
  __X__X__, _X___X__, _X__X___,
  ________, _XXXXX__, ________,
};

/**
 * $DA59: stage5_bitmap_DA59
 */
static const u8 stage5_bitmap_DA59[3 * 10] = {
  ______XX, XX__XXXX, X_______,
  _____X__, __X_XX__, X_______,
  _____X_X, XX__XXX_, X_______,
  ______XX, _X__X__X, ________,
  ________, _XX_X___, ________,
  ____XXXX, XXX_XXXX, XX______,
  ___X____, __X_XX__, __X_____,
  ___X_X_X, XX__X_X_, X_X_____,
  ___X_X_X, _X__X_X_, X_X_____,
  ____X_X_, _XXXX__X, _X______,
};

/**
 * $DA77: stage5_bitmap_DA77
 */
static const u8 stage5_bitmap_DA77[3 * 7] = {
  ______XX, XXXXXX__, ________,
  _____X__, __X__X__, ________,
  _____X_X, X_XX_X__, ________,
  _____XXX, XXXXXXX_, ________,
  ____X___, __XXX__X, ________,
  ____X_XX, X_XXXX_X, ________,
  _____X__, XXX___X_, ________,
};

/**
 * $DA8C: stage5_bitmap_DA8C
 */
static const u8 stage5_bitmap_DA8C[3 * 7] = {
  ________, __XXXXXX, XX______,
  ________, _X____X_, _X______,
  ________, _X_XX_XX, _X______,
  ________, _XXXXXXX, XXX_____,
  ________, X_____XX, X__X____,
  ________, X_XXX_XX, XX_X____,
  ________, _X__XXX_, __X_____,
};

/**
 * $DAA1: stage5_bitmap_DAA1
 */
static const u8 stage5_bitmap_DAA1[2 * 2 * 5] = {
  XXXXXXX_, _______X, ______XX, XX_XXX__,
  XXXXXXX_, _______X, ______XX, _X_X_X__,
  XXXXXXXX, ________, XX__XXXX, ___X____,
  XXXXXX__, ______XX, ______XX, XX_XXX__,
  XXXXXX__, ______X_, ______XX, X___X___,
};

/**
 * $DAB5: stage5_bitmap_DAB5
 */
static const u8 stage5_bitmap_DAB5[2 * 2 * 5] = {
  XXXXXXXX, ________, X_______, _XXX_XXX,
  XXXXXXXX, ________, X_______, _X_X_X_X,
  XXXXXXXX, ________, XXXX__XX, _____X__,
  XXXXXXXX, ________, ________, XXXX_XXX,
  XXXXXXXX, ________, ________, X_X___X_,
};

/**
 * $DAC9: stage5_bitmap_DAC9
 */
static const u8 stage5_bitmap_DAC9[2 * 2 * 4] = {
  XXXXXX__, ______XX, ___XXXXX, XXX_____,
  XXXXXX__, ______XX, ___XXXXX, __X_____,
  XXXXX___, _____XXX, ____XXXX, _XXX____,
  XXXXX___, _____X_X, ____XXXX, __XX____,
};

/**
 * $DAD9: stage5_bitmap_DAD9
 */
static const u8 stage5_bitmap_DAD9[2 * 2 * 4 + 7] = {
  XXXXXXXX, ________, XX_____X, __XXXXX_,
  XXXXXXXX, ________, XX_____X, __XX__X_,
  XXXXXXXX, ________, X_______, _XXX_XXX,
  XXXXXXXX, ________, X_______, _X_X__XX,
  // $DAE9: 7 further bytes, not reached by any LOD entry
  ________, X_______, _XXX_XXX, XXXXXXXX, ________, X_______, _X_X__XX,
};

const map_goto_entry_t stage5_map_goto_table[24] = {
  { STAGE5_MAP_CURV_C455_ADDR,    &stage5_map_curv_C455[0]    },
  { STAGE5_MAP_HEIGHT_C468_ADDR,  &stage5_map_height_C468[0]  },
  { STAGE5_MAP_LANES_C47A_ADDR,   &stage5_map_lanes_C47A[0]   },
  { STAGE5_MAP_HAZARDS_C492_ADDR, &stage5_map_hazards_C492[0] },
  { STAGE5_MAP_LOBJS_C49A_ADDR,   &stage5_map_lobjs_C49A[0]   },
  { STAGE5_MAP_ROBJS_C4DD_ADDR,   &stage5_map_robjs_C4DD[0]   },
  { STAGE5_MAP_CURV_C513_ADDR,    &stage5_map_curv_C513[0]    },
  { STAGE5_MAP_HEIGHT_C526_ADDR,  &stage5_map_height_C526[0]  },
  { STAGE5_MAP_LANES_C53E_ADDR,   &stage5_map_lanes_C53E[0]   },
  { STAGE5_MAP_HAZARDS_C552_ADDR, &stage5_map_hazards_C552[0] },
  { STAGE5_MAP_LOBJS_C560_ADDR,   &stage5_map_lobjs_C560[0]   },
  { STAGE5_MAP_ROBJS_C5A3_ADDR,   &stage5_map_robjs_C5A3[0]   },
  { STAGE5_MAP_CURV_C5E3_ADDR,    &stage5_map_curv_C5E3[0]    },
  { STAGE5_MAP_HEIGHT_C5F8_ADDR,  &stage5_map_height_C5F8[0]  },
  { STAGE5_MAP_LANES_C614_ADDR,   &stage5_map_lanes_C614[0]   },
  { STAGE5_MAP_HAZARDS_C628_ADDR, &stage5_map_hazards_C628[0] },
  { STAGE5_MAP_LOBJS_C62D_ADDR,   &stage5_map_lobjs_C62D[0]   },
  { STAGE5_MAP_ROBJS_C651_ADDR,   &stage5_map_robjs_C651[0]   },
  { STAGE5_MAP_CURV_C65C_ADDR,    &stage5_map_curv_C65C[0]    },
  { STAGE5_MAP_HEIGHT_C684_ADDR,  &stage5_map_height_C684[0]  },
  { STAGE5_MAP_LANES_C6BB_ADDR,   &stage5_map_lanes_C6BB[0]   },
  { STAGE5_MAP_HAZARDS_C6D1_ADDR, &stage5_map_hazards_C6D1[0] },
  { STAGE5_MAP_LOBJS_C6EE_ADDR,   &stage5_map_lobjs_C6EE[0]   },
  { STAGE5_MAP_ROBJS_C7A5_ADDR,   &stage5_map_robjs_C7A5[0]   },
};

// clang-format on
