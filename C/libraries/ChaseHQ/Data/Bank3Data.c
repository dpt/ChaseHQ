/**
* Bank3Data.c
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

#include <stddef.h>

#include "ZXSpectrum/Pixels.h"

#include "C99/Types.h"

#include "ChaseHQ/ChaseHQ.h"
#include "ChaseHQ/Engine/Types.h"

#include "Bank3Data.h"

/* ----------------------------------------------------------------------- */

// $CC50 -- copyright/credits text, drawn by title_screen_driver via
// print_string ($FD9C) before the $CCB7 scene tables.
const u8 title_screen_credits_text[56] = {
  0xC2, // attribute_BRIGHT_RED_OVER_BLACK + single height bit
  TWOBYTES(0x50C3),
  '(', 'C', ')', ' ', '1', '9', '8', '9', ' ', 'O', 'C', 'E', 'A', 'N', ' ', 'S', 'O', 'F', 'T', 'W', 'A', 'R', 'E' | EOS,
  0xC2, // attribute_BRIGHT_RED_OVER_BLACK + single height bit
  TWOBYTES(0x50E2),
  '(', 'C', ')', ' ', '1', '9', '8', '8', ' ', 'T', 'A', 'I', 'T', 'O', ' ', 'C', 'O', 'R', 'P', 'O', 'R', 'A', 'T', 'I', 'O', 'N' | EOS,
  0
};

// $CC88 -- drawn directly by title_screen_driver via print_character (not
// walked as a list: no end marker, matching the Z80, which runs straight
// into the $CCB7 scene tables afterwards).
const u8 title_screen_overlay_text[47] = {
  0xC7, // attribute_BRIGHT_WHITE_OVER_BLACK + single height bit
  TWOBYTES(0x4826),
  'P', 'R', 'E', 'S', 'S', ' ', 'G', 'E', 'A', 'R', ' ', 'T', 'O', ' ', 'P', 'L', 'A', 'Y' | EOS,
  0xC4, // attribute_BRIGHT_GREEN_OVER_BLACK + single height bit
  TWOBYTES(0x4864),
  'P', 'R', 'E', 'S', 'S', ' ', 'E', 'N', 'T', 'E', 'R', ' ', 'F', 'O', 'R', ' ', 'O', 'P', 'T', 'I', 'O', 'N', 'S' | EOS
};

// $FC29-$FD96 -- 128K control-select, key-redefinition and hidden test-mode
// screen text, printed via print_string/print_character. Unlike the
// messages_* lists above, print_string does not walk an end-marker-terminated
// list: each 0x00 below terminates whichever call is in progress, so this one
// data block actually holds four independent entry points, each reached via
// its own literal HL constant in the original:
//   offset   0 ($FC29): control-select screen -- wired into
//                       omd_redraw_and_poll below.
//   offset 114 ($FC9B): key-redefinition screen, header + GEAR/ACCELERATE/
//                       BRAKE -- not yet wired up (needs redefine_keys_screen,
//                       $FEA9).
//   offset 160 ($FCC9): key-redefinition screen continued, LEFT/RIGHT/QUIT/
//                       PAUSE/TURBO -- not yet wired up (see above).
//   offset 199 ($FCF0): hidden test-mode screen -- not yet wired up (needs a
//                       128K test-mode driver, $C06E).
// "P1."-"P4."/"P5." labels in the skool comments are missing their leading
// "P" in the actual data (confirmed byte-for-byte against the skool) --
// presumably drawn as a separate fixed graphic; transcribed faithfully as-is.
const u8 options_menu_text[366] = {
  // $FC29 (offset 0): control-select screen
  attribute_RED_OVER_BLACK,
  TWOBYTES(0x484A),
  'E', 'N', 'T', 'E', 'R', ' ', 'O', 'P', 'T', 'I', 'O', 'N' | EOS,
  attribute_CYAN_OVER_BLACK,
  TWOBYTES(0x48C6),
  '1', '.', ' ', 'S', 'I', 'N', 'C', 'L', 'A', 'I', 'R', ' ', 'J', 'O', 'Y', 'S', 'T', 'I', 'C', 'K' | EOS,
  attribute_CYAN_OVER_BLACK,
  TWOBYTES(0x5006),
  '2', '.', ' ', 'C', 'U', 'R', 'S', 'O', 'R', ' ', 'J', 'O', 'Y', 'S', 'T', 'I', 'C', 'K' | EOS,
  attribute_CYAN_OVER_BLACK,
  TWOBYTES(0x5046),
  '3', '.', ' ', 'K', 'E', 'M', 'P', 'S', 'T', 'O', 'N', ' ', 'J', 'O', 'Y', 'S', 'T', 'I', 'C', 'K' | EOS,
  attribute_CYAN_OVER_BLACK,
  TWOBYTES(0x5086),
  '4', '.', ' ', 'K', 'E', 'Y', 'B', 'O', 'A', 'R', 'D' | EOS,
  attribute_CYAN_OVER_BLACK,
  TWOBYTES(0x50C6),
  '5', '.', ' ', 'D', 'E', 'F', 'I', 'N', 'E', ' ', 'K', 'E', 'Y', 'S' | EOS,
  0, // terminator ($FC9A)

  // $FC9B (offset 114): key-redefinition screen, header + first 3 labels
  attribute_RED_OVER_BLACK,
  TWOBYTES(0x4849),
  'R', 'E', 'D', 'E', 'F', 'I', 'N', 'E', ' ', ' ', 'K', 'E', 'Y', 'S' | EOS,
  0xC6, // attribute_BRIGHT_YELLOW_OVER_BLACK + single height bit
  TWOBYTES(0x48C9),
  'G', 'E', 'A', 'R' | EOS,
  0xC6,
  TWOBYTES(0x48E9),
  'A', 'C', 'C', 'E', 'L', 'E', 'R', 'A', 'T', 'E' | EOS,
  0xC6,
  TWOBYTES(0x5009),
  'B', 'R', 'A', 'K', 'E' | EOS,
  0, // terminator ($FCC8)

  // $FCC9 (offset 160): key-redefinition screen, remaining 5 labels
  0xC6,
  TWOBYTES(0x5029),
  'L', 'E', 'F', 'T' | EOS,
  0xC6,
  TWOBYTES(0x5049),
  'R', 'I', 'G', 'H', 'T' | EOS,
  0xC4, // attribute_BRIGHT_GREEN_OVER_BLACK + single height bit
  TWOBYTES(0x5089),
  'Q', 'U', 'I', 'T' | EOS,
  0xC4,
  TWOBYTES(0x50A9),
  'P', 'A', 'U', 'S', 'E' | EOS,
  0xC4,
  TWOBYTES(0x50C9),
  'T', 'U', 'R', 'B', 'O' | EOS,
  0, // terminator ($FCEF)

  // $FCF0 (offset 199): hidden test-mode screen
  0xC1, // attribute_BRIGHT_BLUE_OVER_BLACK + single height bit
  TWOBYTES(0x4000),
  'T', 'E', 'S', 'T' | EOS,
  attribute_RED_OVER_BLACK,
  TWOBYTES(0x4826),
  'C', 'H', 'A', 'S', 'E', ' ', 'H', '.', 'Q', '.', ' ', 'T', 'E', 'S', 'T', ' ', 'M', 'O', 'D', 'E' | EOS,
  0xC5, // attribute_BRIGHT_CYAN_OVER_BLACK + single height bit
  TWOBYTES(0x48A2),
  'T', 'I', 'T', 'L', 'E', ' ', 'S', 'C', 'R', 'E', 'E', 'N' | EOS,
  0xC3, // attribute_BRIGHT_MAGENTA_OVER_BLACK + single height bit
  TWOBYTES(0x48E2),
  '1', ' ', 'T', 'O', ' ', '5', '.', ' ', 'L', 'O', 'G', 'O', ' ', 'A', 'N', 'I', 'M', 'A', 'T', 'I', 'O', 'N' | EOS,
  0xC3,
  TWOBYTES(0x5007),
  '6', '.', ' ', 'S', 'C', 'O', 'R', 'E', ' ', 'E', 'N', 'T', 'R', 'Y' | EOS,
  0xC5,
  TWOBYTES(0x5042),
  'I', 'N', ' ', 'G', 'A', 'M', 'E' | EOS,
  0xC4,
  TWOBYTES(0x5087),
  '1', '.', ' ', 'R', 'E', 'S', 'T', 'A', 'R', 'T', ' ', 'L', 'E', 'V', 'E', 'L' | EOS,
  0xC4,
  TWOBYTES(0x50A7),
  '2', '.', ' ', 'N', 'E', 'X', 'T', ' ', 'L', 'E', 'V', 'E', 'L' | EOS,
  0xC4,
  TWOBYTES(0x50C7),
  '3', '.', ' ', 'E', 'N', 'D', ' ', 'S', 'C', 'R', 'E', 'E', 'N' | EOS,
  0xC4,
  TWOBYTES(0x50E7),
  '4', '.', ' ', 'E', 'X', 'T', 'R', 'A', ' ', 'C', 'R', 'E', 'D', 'I', 'T' | EOS,
  0 // terminator / pad byte ($FD96)
};

/* 128K bank 3: title-tune engine AY tone-period lookup table, $EFBC-$F07B.
 * 96 entries, one per note, transcribed directly from the skool's DEFB bytes
 * (little-endian pairs). Used by compute_channel_ay_registers ($EE9E@bank3)
 * to convert a note index into an AY tone-period value. */
const u16 note_periods[96] = {
  0x0EF8, /* NOTE_AS0 */
  0x0E10, /* NOTE_B0 */
  0x0D60, /* NOTE_C1 */
  0x0C80, /* NOTE_CS1 */
  0x0BD8, /* NOTE_D1 */
  0x0B28, /* NOTE_DS1 */
  0x0A88, /* NOTE_E1 */
  0x09F0, /* NOTE_F1 */
  0x0960, /* NOTE_FS1 */
  0x08E0, /* NOTE_G1 */
  0x0858, /* NOTE_GS1 */
  0x07E0, /* NOTE_A1 */
  0x077C, /* NOTE_AS1 */
  0x0708, /* NOTE_B1 */
  0x06B0, /* NOTE_C2 */
  0x0640, /* NOTE_CS2 */
  0x05EC, /* NOTE_D2 */
  0x0594, /* NOTE_DS2 */
  0x0544, /* NOTE_E2 */
  0x04F8, /* NOTE_F2 */
  0x04B0, /* NOTE_FS2 */
  0x0470, /* NOTE_G2 */
  0x042C, /* NOTE_GS2 */
  0x03F0, /* NOTE_A2 */
  0x03BE, /* NOTE_AS2 */
  0x0384, /* NOTE_B2 */
  0x0358, /* NOTE_C3 */
  0x0320, /* NOTE_CS3 */
  0x02F6, /* NOTE_D3 */
  0x02CA, /* NOTE_DS3 */
  0x02A2, /* NOTE_E3 */
  0x027C, /* NOTE_F3 */
  0x0258, /* NOTE_FS3 */
  0x0238, /* NOTE_G3 */
  0x0216, /* NOTE_GS3 */
  0x01F8, /* NOTE_A3 */
  0x01DF, /* NOTE_AS3 */
  0x01C2, /* NOTE_B3 */
  0x01AC, /* NOTE_C4 */
  0x0190, /* NOTE_CS4 */
  0x017B, /* NOTE_D4 */
  0x0165, /* NOTE_DS4 */
  0x0151, /* NOTE_E4 */
  0x013E, /* NOTE_F4 */
  0x012C, /* NOTE_FS4 */
  0x011C, /* NOTE_G4 */
  0x010B, /* NOTE_GS4 */
  0x00FC, /* NOTE_A4 */
  0x00EF, /* NOTE_AS4 */
  0x00E1, /* NOTE_B4 */
  0x00D6, /* NOTE_C5 */
  0x00C8, /* NOTE_CS5 */
  0x00BD, /* NOTE_D5 */
  0x00B2, /* NOTE_DS5 */
  0x00A8, /* NOTE_E5 */
  0x009F, /* NOTE_F5 */
  0x0096, /* NOTE_FS5 */
  0x008E, /* NOTE_G5 */
  0x0085, /* NOTE_GS5 */
  0x007E, /* NOTE_A5 */
  0x0077, /* NOTE_AS5 */
  0x0070, /* NOTE_B5 */
  0x006B, /* NOTE_C6 */
  0x0064, /* NOTE_CS6 */
  0x005E, /* NOTE_D6 */
  0x0059, /* NOTE_DS6 */
  0x0054, /* NOTE_E6 */
  0x004F, /* NOTE_F6 */
  0x004B, /* NOTE_FS6 */
  0x0047, /* NOTE_G6 */
  0x0042, /* NOTE_GS6 */
  0x003F, /* NOTE_A6 */
  0x003B, /* NOTE_AS6 */
  0x0038, /* NOTE_B6 */
  0x0035, /* NOTE_C7 */
  0x0032, /* NOTE_CS7 */
  0x002F, /* NOTE_D7 */
  0x002C, /* NOTE_DS7 */
  0x002A, /* NOTE_E7 */
  0x0027, /* NOTE_F7 */
  0x0025, /* NOTE_FS7 */
  0x0023, /* NOTE_G7 */
  0x0021, /* NOTE_GS7 */
  0x001F, /* NOTE_A7 */
  0x001D, /* NOTE_AS7 */
  0x001C, /* NOTE_B7 */
  0x001A, /* NOTE_C8 */
  0x0019, /* NOTE_CS8 */
  0x0017, /* NOTE_D8 */
  0x0016, /* NOTE_DS8 */
  0x0015, /* NOTE_E8 */
  0x0013, /* NOTE_F8 */
  0x0012, /* NOTE_FS8 */
  0x0011, /* NOTE_G8 */
  0x0010, /* NOTE_GS8 */
  0x000F /* NOTE_A8 */
};

/* 128K bank 3: title-tune engine tune-select table, $F225-$F240, transcribed
 * directly from the skool's DEFB bytes. 4 entries, 7 bytes each: tempo byte,
 * then 3 x 2-byte little-endian pattern-data pointers (channels 1-3). Used
 * by start_tune ($EB9E@bank3). Tune 0's channel-1 pointer ($F241) lands
 * exactly on the byte immediately following this table, confirming its
 * 4-entry extent. The pattern-data blocks these pointers reference are
 * extracted as title_tune0_data/title_tune1_data below, for tunes 0
 * and 1 -- see start_tune's Translation notes. */
const tune_t tunes[4] = {
  { 2, { 0xF241, 0xF25A, 0xF265 } },
  { 4, { 0xF601, 0xF605, 0xF609 } },
  { 2, { 0xF666, 0xF66A, 0xF66E } },
  { 3, { 0xF6F4, 0xF6F8, 0xF6FE } }
};

/* 128K bank 3: title-tune engine raw pattern-data regions for tunes 0 (title
 * screen) and 1 (perp-caught success jingle), transcribed byte-exact from
 * bank3.bin. See declaration comments in Bank3Data.h. Tunes 2 and 3 are not
 * extracted (unreachable from the code paths wired up so far). */

/* $F241-$F600 */
const u8 title_tune0_data[960] = {
  0x6E, /* $F241: HEADER_PATTERN_PTR [tune0ch0] */
  0xF2, /* $F242: (high byte) */
  0x02, /* $F243: PHRASE_TABLE_WORD */
  0x00, /* $F244: (high byte) */
  0x08, /* $F245: PHRASE_TABLE_REPEAT_COUNT */
  0x6E, /* $F246: PHRASE_TABLE_REPEAT_PTR */
  0xF2, /* $F247: (high byte) */
  0x97, /* $F248: PHRASE_TABLE_WORD */
  0xF2, /* $F249: (high byte) */
  0x88, /* $F24A: PHRASE_TABLE_WORD */
  0xF2, /* $F24B: (high byte) */
  0xA7, /* $F24C: PHRASE_TABLE_WORD */
  0xF2, /* $F24D: (high byte) */
  0x88, /* $F24E: PHRASE_TABLE_WORD */
  0xF2, /* $F24F: (high byte) */
  0x88, /* $F250: PHRASE_TABLE_WORD */
  0xF2, /* $F251: (high byte) */
  0x97, /* $F252: PHRASE_TABLE_WORD */
  0xF2, /* $F253: (high byte) */
  0x88, /* $F254: PHRASE_TABLE_WORD */
  0xF2, /* $F255: (high byte) */
  0x39, /* $F256: PHRASE_TABLE_WORD */
  0xF3, /* $F257: (high byte) */
  0x00, /* $F258: PHRASE_TABLE_WORD */
  0x00, /* $F259: (high byte) */
  0x6B, /* $F25A: HEADER_PATTERN_PTR [tune0ch1] */
  0xF2, /* $F25B: (high byte) */
  0x02, /* $F25C: PHRASE_TABLE_WORD */
  0x00, /* $F25D: (high byte) */
  0x07, /* $F25E: PHRASE_TABLE_REPEAT_COUNT */
  0x6B, /* $F25F: PHRASE_TABLE_REPEAT_PTR */
  0xF2, /* $F260: (high byte) */
  0xE1, /* $F261: PHRASE_TABLE_WORD */
  0xF4, /* $F262: (high byte) */
  0x00, /* $F263: PHRASE_TABLE_WORD */
  0x00, /* $F264: (high byte) */
  0x50, /* $F265: HEADER_PATTERN_PTR [tune0ch2] */
  0xF3, /* $F266: (high byte) */
  0x9F, /* $F267: PHRASE_TABLE_WORD */
  0xF3, /* $F268: (high byte) */
  0x00, /* $F269: PHRASE_TABLE_WORD */
  0x00, /* $F26A: (high byte) */
  0xFF, /* $F26B: PCMD_SET_ROW_WAIT(32) [tune0ch1] */
  0x80, /* $F26C: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch1] */
  0x87, /* $F26D: PCMD_ADVANCE_PHRASE [tune0ch1] */
  0x8A, /* $F26E: PCMD_SET_MIXER_BITS_HIGH3 [tune0ch0] */
  0x90, /* $F26F: PCMD_MUTE_CHANNEL [tune0ch0] */
  0xD0, /* $F270: PCMD_SELECT_ENVELOPE_SHAPE(0) [tune0ch0] */
  0xB9, /* $F271: PCMD_SELECT_PITCH_OFFSET(1) [tune0ch0] */
  0x88, /* $F272: PCMD_SET_ENVELOPE_PARAMS [tune0ch0] */
  0x02, /* $F273: PCMD_SET_ENVELOPE_PARAMS_OPERAND [tune0ch0] */
  0x01, /* $F274: PCMD_SET_ENVELOPE_PARAMS_OPERAND [tune0ch0] */
  0x82, /* $F275: PCMD_VIBRATO_ON [tune0ch0] */
  0xE3, /* $F276: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x15, /* $F277: NOTE_G2 [tune0ch0] */
  0x15, /* $F278: NOTE_G2 [tune0ch0] */
  0x21, /* $F279: NOTE_G3 [tune0ch0] */
  0x15, /* $F27A: NOTE_G2 [tune0ch0] */
  0x15, /* $F27B: NOTE_G2 [tune0ch0] */
  0x21, /* $F27C: NOTE_G3 [tune0ch0] */
  0x15, /* $F27D: NOTE_G2 [tune0ch0] */
  0x15, /* $F27E: NOTE_G2 [tune0ch0] */
  0x21, /* $F27F: NOTE_G3 [tune0ch0] */
  0x15, /* $F280: NOTE_G2 [tune0ch0] */
  0x15, /* $F281: NOTE_G2 [tune0ch0] */
  0x21, /* $F282: NOTE_G3 [tune0ch0] */
  0x15, /* $F283: NOTE_G2 [tune0ch0] */
  0x15, /* $F284: NOTE_G2 [tune0ch0] */
  0x12, /* $F285: NOTE_E2 [tune0ch0] */
  0x14, /* $F286: NOTE_FS2 [tune0ch0] */
  0x87, /* $F287: PCMD_ADVANCE_PHRASE [tune0ch0] */
  0x8A, /* $F288: PCMD_SET_MIXER_BITS_HIGH3 [tune0ch0] */
  0x90, /* $F289: PCMD_MUTE_CHANNEL [tune0ch0] */
  0xD0, /* $F28A: PCMD_SELECT_ENVELOPE_SHAPE(0) [tune0ch0] */
  0xB9, /* $F28B: PCMD_SELECT_PITCH_OFFSET(1) [tune0ch0] */
  0x81, /* $F28C: PCMD_VIBRATO_OFF [tune0ch0] */
  0xE3, /* $F28D: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x15, /* $F28E: NOTE_G2 [tune0ch0] */
  0x15, /* $F28F: NOTE_G2 [tune0ch0] */
  0x15, /* $F290: NOTE_G2 [tune0ch0] */
  0x15, /* $F291: NOTE_G2 [tune0ch0] */
  0x15, /* $F292: NOTE_G2 [tune0ch0] */
  0x15, /* $F293: NOTE_G2 [tune0ch0] */
  0x15, /* $F294: NOTE_G2 [tune0ch0] */
  0x15, /* $F295: NOTE_G2 [tune0ch0] */
  0x87, /* $F296: PCMD_ADVANCE_PHRASE [tune0ch0] */
  0x1A, /* $F297: NOTE_C3 [tune0ch0] */
  0x1A, /* $F298: NOTE_C3 [tune0ch0] */
  0x1A, /* $F299: NOTE_C3 [tune0ch0] */
  0x1A, /* $F29A: NOTE_C3 [tune0ch0] */
  0x1A, /* $F29B: NOTE_C3 [tune0ch0] */
  0x1A, /* $F29C: NOTE_C3 [tune0ch0] */
  0x1A, /* $F29D: NOTE_C3 [tune0ch0] */
  0xE7, /* $F29E: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x18, /* $F29F: NOTE_AS2 [tune0ch0] */
  0x17, /* $F2A0: NOTE_A2 [tune0ch0] */
  0x15, /* $F2A1: NOTE_G2 [tune0ch0] */
  0xE3, /* $F2A2: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x13, /* $F2A3: NOTE_F2 [tune0ch0] */
  0x12, /* $F2A4: NOTE_E2 [tune0ch0] */
  0x10, /* $F2A5: NOTE_D2 [tune0ch0] */
  0x87, /* $F2A6: PCMD_ADVANCE_PHRASE [tune0ch0] */
  0x13, /* $F2A7: NOTE_F2 [tune0ch0] */
  0x13, /* $F2A8: NOTE_F2 [tune0ch0] */
  0x13, /* $F2A9: NOTE_F2 [tune0ch0] */
  0x13, /* $F2AA: NOTE_F2 [tune0ch0] */
  0x13, /* $F2AB: NOTE_F2 [tune0ch0] */
  0x13, /* $F2AC: NOTE_F2 [tune0ch0] */
  0x13, /* $F2AD: NOTE_F2 [tune0ch0] */
  0x13, /* $F2AE: NOTE_F2 [tune0ch0] */
  0x12, /* $F2AF: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B0: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B1: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B2: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B3: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B4: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B5: NOTE_E2 [tune0ch0] */
  0x12, /* $F2B6: NOTE_E2 [tune0ch0] */
  0x10, /* $F2B7: NOTE_D2 [tune0ch0] */
  0x10, /* $F2B8: NOTE_D2 [tune0ch0] */
  0x12, /* $F2B9: NOTE_E2 [tune0ch0] */
  0x10, /* $F2BA: NOTE_D2 [tune0ch0] */
  0x13, /* $F2BB: NOTE_F2 [tune0ch0] */
  0x10, /* $F2BC: NOTE_D2 [tune0ch0] */
  0x15, /* $F2BD: NOTE_G2 [tune0ch0] */
  0x13, /* $F2BE: NOTE_F2 [tune0ch0] */
  0x10, /* $F2BF: NOTE_D2 [tune0ch0] */
  0x10, /* $F2C0: NOTE_D2 [tune0ch0] */
  0x12, /* $F2C1: NOTE_E2 [tune0ch0] */
  0x10, /* $F2C2: NOTE_D2 [tune0ch0] */
  0xE1, /* $F2C3: PCMD_SET_ROW_WAIT(2) [tune0ch0] */
  0x10, /* $F2C4: NOTE_D2 [tune0ch0] */
  0x12, /* $F2C5: NOTE_E2 [tune0ch0] */
  0x13, /* $F2C6: NOTE_F2 [tune0ch0] */
  0x15, /* $F2C7: NOTE_G2 [tune0ch0] */
  0x16, /* $F2C8: NOTE_GS2 [tune0ch0] */
  0x17, /* $F2C9: NOTE_A2 [tune0ch0] */
  0x1A, /* $F2CA: NOTE_C3 [tune0ch0] */
  0x19, /* $F2CB: NOTE_B2 [tune0ch0] */
  0xE3, /* $F2CC: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x15, /* $F2CD: NOTE_G2 [tune0ch0] */
  0x14, /* $F2CE: NOTE_FS2 [tune0ch0] */
  0x13, /* $F2CF: NOTE_F2 [tune0ch0] */
  0xE7, /* $F2D0: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x12, /* $F2D1: NOTE_E2 [tune0ch0] */
  0xE3, /* $F2D2: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x12, /* $F2D3: NOTE_E2 [tune0ch0] */
  0x12, /* $F2D4: NOTE_E2 [tune0ch0] */
  0x12, /* $F2D5: NOTE_E2 [tune0ch0] */
  0x17, /* $F2D6: NOTE_A2 [tune0ch0] */
  0x16, /* $F2D7: NOTE_GS2 [tune0ch0] */
  0x15, /* $F2D8: NOTE_G2 [tune0ch0] */
  0xE7, /* $F2D9: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x14, /* $F2DA: NOTE_FS2 [tune0ch0] */
  0xE3, /* $F2DB: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x14, /* $F2DC: NOTE_FS2 [tune0ch0] */
  0x14, /* $F2DD: NOTE_FS2 [tune0ch0] */
  0x14, /* $F2DE: NOTE_FS2 [tune0ch0] */
  0x19, /* $F2DF: NOTE_B2 [tune0ch0] */
  0x18, /* $F2E0: NOTE_AS2 [tune0ch0] */
  0x17, /* $F2E1: NOTE_A2 [tune0ch0] */
  0xE7, /* $F2E2: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x15, /* $F2E3: NOTE_G2 [tune0ch0] */
  0xE3, /* $F2E4: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x15, /* $F2E5: NOTE_G2 [tune0ch0] */
  0x15, /* $F2E6: NOTE_G2 [tune0ch0] */
  0xE7, /* $F2E7: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x19, /* $F2E8: NOTE_B2 [tune0ch0] */
  0x1A, /* $F2E9: NOTE_C3 [tune0ch0] */
  0xE3, /* $F2EA: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x1B, /* $F2EB: NOTE_CS3 [tune0ch0] */
  0x1C, /* $F2EC: NOTE_D3 [tune0ch0] */
  0x10, /* $F2ED: NOTE_D2 [tune0ch0] */
  0x10, /* $F2EE: NOTE_D2 [tune0ch0] */
  0x10, /* $F2EF: NOTE_D2 [tune0ch0] */
  0xE3, /* $F2F0: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x15, /* $F2F1: NOTE_G2 [tune0ch0] */
  0x14, /* $F2F2: NOTE_FS2 [tune0ch0] */
  0x13, /* $F2F3: NOTE_F2 [tune0ch0] */
  0xE7, /* $F2F4: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x12, /* $F2F5: NOTE_E2 [tune0ch0] */
  0xE3, /* $F2F6: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x12, /* $F2F7: NOTE_E2 [tune0ch0] */
  0x12, /* $F2F8: NOTE_E2 [tune0ch0] */
  0x12, /* $F2F9: NOTE_E2 [tune0ch0] */
  0x17, /* $F2FA: NOTE_A2 [tune0ch0] */
  0x16, /* $F2FB: NOTE_GS2 [tune0ch0] */
  0x15, /* $F2FC: NOTE_G2 [tune0ch0] */
  0xE7, /* $F2FD: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x14, /* $F2FE: NOTE_FS2 [tune0ch0] */
  0xE3, /* $F2FF: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x14, /* $F300: NOTE_FS2 [tune0ch0] */
  0x14, /* $F301: NOTE_FS2 [tune0ch0] */
  0x14, /* $F302: NOTE_FS2 [tune0ch0] */
  0x19, /* $F303: NOTE_B2 [tune0ch0] */
  0x18, /* $F304: NOTE_AS2 [tune0ch0] */
  0x17, /* $F305: NOTE_A2 [tune0ch0] */
  0xE7, /* $F306: PCMD_SET_ROW_WAIT(8) [tune0ch0] */
  0x15, /* $F307: NOTE_G2 [tune0ch0] */
  0xE3, /* $F308: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x15, /* $F309: NOTE_G2 [tune0ch0] */
  0x15, /* $F30A: NOTE_G2 [tune0ch0] */
  0xE7, /* $F30B: -- unreached by tune 0/1 playback -- */
  0x16, /* $F30C: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F30D: -- unreached by tune 0/1 playback -- */
  0x80, /* $F30E: -- unreached by tune 0/1 playback -- */
  0xEF, /* $F30F: -- unreached by tune 0/1 playback -- */
  0x18, /* $F310: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F311: -- unreached by tune 0/1 playback -- */
  0x80, /* $F312: -- unreached by tune 0/1 playback -- */
  0x80, /* $F313: -- unreached by tune 0/1 playback -- */
  0x12, /* $F314: -- unreached by tune 0/1 playback -- */
  0x13, /* $F315: -- unreached by tune 0/1 playback -- */
  0x12, /* $F316: -- unreached by tune 0/1 playback -- */
  0x14, /* $F317: -- unreached by tune 0/1 playback -- */
  0x12, /* $F318: -- unreached by tune 0/1 playback -- */
  0x16, /* $F319: -- unreached by tune 0/1 playback -- */
  0x13, /* $F31A: -- unreached by tune 0/1 playback -- */
  0x12, /* $F31B: -- unreached by tune 0/1 playback -- */
  0x12, /* $F31C: -- unreached by tune 0/1 playback -- */
  0x13, /* $F31D: -- unreached by tune 0/1 playback -- */
  0x12, /* $F31E: -- unreached by tune 0/1 playback -- */
  0x14, /* $F31F: -- unreached by tune 0/1 playback -- */
  0x12, /* $F320: -- unreached by tune 0/1 playback -- */
  0x16, /* $F321: -- unreached by tune 0/1 playback -- */
  0x13, /* $F322: -- unreached by tune 0/1 playback -- */
  0x80, /* $F323: -- unreached by tune 0/1 playback -- */
  0x12, /* $F324: -- unreached by tune 0/1 playback -- */
  0x13, /* $F325: -- unreached by tune 0/1 playback -- */
  0x12, /* $F326: -- unreached by tune 0/1 playback -- */
  0x14, /* $F327: -- unreached by tune 0/1 playback -- */
  0x12, /* $F328: -- unreached by tune 0/1 playback -- */
  0x16, /* $F329: -- unreached by tune 0/1 playback -- */
  0x13, /* $F32A: -- unreached by tune 0/1 playback -- */
  0x12, /* $F32B: -- unreached by tune 0/1 playback -- */
  0x12, /* $F32C: -- unreached by tune 0/1 playback -- */
  0x13, /* $F32D: -- unreached by tune 0/1 playback -- */
  0x12, /* $F32E: -- unreached by tune 0/1 playback -- */
  0xE1, /* $F32F: -- unreached by tune 0/1 playback -- */
  0x10, /* $F330: -- unreached by tune 0/1 playback -- */
  0x12, /* $F331: -- unreached by tune 0/1 playback -- */
  0x13, /* $F332: -- unreached by tune 0/1 playback -- */
  0x15, /* $F333: -- unreached by tune 0/1 playback -- */
  0x16, /* $F334: -- unreached by tune 0/1 playback -- */
  0x17, /* $F335: -- unreached by tune 0/1 playback -- */
  0x1A, /* $F336: -- unreached by tune 0/1 playback -- */
  0x19, /* $F337: -- unreached by tune 0/1 playback -- */
  0x87, /* $F338: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F339: PCMD_SET_ROW_WAIT(4) [tune0ch0] */
  0x13, /* $F33A: -- unreached by tune 0/1 playback -- */
  0x13, /* $F33B: -- unreached by tune 0/1 playback -- */
  0x13, /* $F33C: -- unreached by tune 0/1 playback -- */
  0x13, /* $F33D: -- unreached by tune 0/1 playback -- */
  0x13, /* $F33E: -- unreached by tune 0/1 playback -- */
  0x13, /* $F33F: -- unreached by tune 0/1 playback -- */
  0x13, /* $F340: -- unreached by tune 0/1 playback -- */
  0x13, /* $F341: -- unreached by tune 0/1 playback -- */
  0x12, /* $F342: -- unreached by tune 0/1 playback -- */
  0x12, /* $F343: -- unreached by tune 0/1 playback -- */
  0x12, /* $F344: -- unreached by tune 0/1 playback -- */
  0x12, /* $F345: -- unreached by tune 0/1 playback -- */
  0xE1, /* $F346: -- unreached by tune 0/1 playback -- */
  0x10, /* $F347: -- unreached by tune 0/1 playback -- */
  0x12, /* $F348: -- unreached by tune 0/1 playback -- */
  0x13, /* $F349: -- unreached by tune 0/1 playback -- */
  0x15, /* $F34A: -- unreached by tune 0/1 playback -- */
  0x16, /* $F34B: -- unreached by tune 0/1 playback -- */
  0x17, /* $F34C: -- unreached by tune 0/1 playback -- */
  0x1A, /* $F34D: -- unreached by tune 0/1 playback -- */
  0x19, /* $F34E: -- unreached by tune 0/1 playback -- */
  0x87, /* $F34F: -- unreached by tune 0/1 playback -- */
  0x8A, /* $F350: PCMD_SET_MIXER_BITS_HIGH3 [tune0ch2] */
  0x91, /* $F351: PCMD_UNMUTE_CHANNEL [tune0ch2] */
  0xD1, /* $F352: PCMD_SELECT_ENVELOPE_SHAPE(1) [tune0ch2] */
  0x81, /* $F353: PCMD_VIBRATO_OFF [tune0ch2] */
  0xBA, /* $F354: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0xE3, /* $F355: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x39, /* $F356: NOTE_G5 [tune0ch2] */
  0xEB, /* $F357: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F358: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F359: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x37, /* $F35A: NOTE_F5 [tune0ch2] */
  0xE3, /* $F35B: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F35C: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F35D: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F35E: NOTE_E5 [tune0ch2] */
  0xE7, /* $F35F: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F360: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBA, /* $F361: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0xE3, /* $F362: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x37, /* $F363: NOTE_F5 [tune0ch2] */
  0x80, /* $F364: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F365: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F366: NOTE_E5 [tune0ch2] */
  0x80, /* $F367: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBA, /* $F368: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0x39, /* $F369: NOTE_G5 [tune0ch2] */
  0xEB, /* $F36A: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F36B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F36C: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x37, /* $F36D: NOTE_F5 [tune0ch2] */
  0xE3, /* $F36E: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F36F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F370: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F371: NOTE_E5 [tune0ch2] */
  0xE7, /* $F372: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F373: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F374: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0xBA, /* $F375: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0x37, /* $F376: NOTE_F5 [tune0ch2] */
  0x80, /* $F377: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F378: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F379: NOTE_E5 [tune0ch2] */
  0x80, /* $F37A: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBA, /* $F37B: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0xE3, /* $F37C: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x39, /* $F37D: NOTE_G5 [tune0ch2] */
  0xEB, /* $F37E: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F37F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F380: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x37, /* $F381: NOTE_F5 [tune0ch2] */
  0xE3, /* $F382: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F383: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F384: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F385: NOTE_E5 [tune0ch2] */
  0xE7, /* $F386: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F387: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBA, /* $F388: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0xE3, /* $F389: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x37, /* $F38A: NOTE_F5 [tune0ch2] */
  0x80, /* $F38B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F38C: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F38D: NOTE_E5 [tune0ch2] */
  0x80, /* $F38E: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x80, /* $F38F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBA, /* $F390: PCMD_SELECT_PITCH_OFFSET(2) [tune0ch2] */
  0x39, /* $F391: NOTE_G5 [tune0ch2] */
  0xE7, /* $F392: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F393: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F394: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x37, /* $F395: NOTE_F5 [tune0ch2] */
  0x80, /* $F396: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBB, /* $F397: PCMD_SELECT_PITCH_OFFSET(3) [tune0ch2] */
  0x36, /* $F398: NOTE_E5 [tune0ch2] */
  0x80, /* $F399: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F39A: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0xBC, /* $F39B: PCMD_SELECT_PITCH_OFFSET(4) [tune0ch2] */
  0x34, /* $F39C: NOTE_D5 [tune0ch2] */
  0x80, /* $F39D: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x87, /* $F39E: PCMD_ADVANCE_PHRASE [tune0ch2] */
  0x8A, /* $F39F: PCMD_SET_MIXER_BITS_HIGH3 [tune0ch2] */
  0x91, /* $F3A0: PCMD_UNMUTE_CHANNEL [tune0ch2] */
  0xD2, /* $F3A1: PCMD_SELECT_ENVELOPE_SHAPE(2) [tune0ch2] */
  0x81, /* $F3A2: PCMD_VIBRATO_OFF [tune0ch2] */
  0xBD, /* $F3A3: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F3A4: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F3A5: NOTE_B5 [tune0ch2] */
  0xEB, /* $F3A6: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F3A7: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F3A8: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x3B, /* $F3A9: NOTE_A5 [tune0ch2] */
  0xE3, /* $F3AA: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F3AB: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3AC: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3AD: NOTE_G5 [tune0ch2] */
  0xE7, /* $F3AE: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F3AF: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F3B0: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F3B1: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F3B2: NOTE_A5 [tune0ch2] */
  0x80, /* $F3B3: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3B4: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3B5: NOTE_G5 [tune0ch2] */
  0x80, /* $F3B6: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F3B7: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3D, /* $F3B8: NOTE_B5 [tune0ch2] */
  0xEB, /* $F3B9: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F3BA: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F3BB: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x3B, /* $F3BC: NOTE_A5 [tune0ch2] */
  0xE3, /* $F3BD: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F3BE: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3BF: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3C0: NOTE_G5 [tune0ch2] */
  0xE7, /* $F3C1: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F3C2: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F3C3: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0xBD, /* $F3C4: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3B, /* $F3C5: NOTE_A5 [tune0ch2] */
  0x80, /* $F3C6: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3C7: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3C8: NOTE_G5 [tune0ch2] */
  0x80, /* $F3C9: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F3CA: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F3CB: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F3CC: NOTE_B5 [tune0ch2] */
  0xEB, /* $F3CD: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F3CE: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F3CF: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x3B, /* $F3D0: NOTE_A5 [tune0ch2] */
  0xE3, /* $F3D1: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F3D2: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3D3: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3D4: NOTE_G5 [tune0ch2] */
  0xE7, /* $F3D5: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F3D6: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F3D7: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F3D8: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F3D9: NOTE_A5 [tune0ch2] */
  0x80, /* $F3DA: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3DB: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3DC: NOTE_G5 [tune0ch2] */
  0x80, /* $F3DD: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x80, /* $F3DE: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F3DF: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3D, /* $F3E0: NOTE_B5 [tune0ch2] */
  0xE7, /* $F3E1: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F3E2: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F3E3: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F3E4: NOTE_A5 [tune0ch2] */
  0x80, /* $F3E5: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3E6: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F3E7: NOTE_G5 [tune0ch2] */
  0x80, /* $F3E8: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEF, /* $F3E9: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0xBF, /* $F3EA: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0x39, /* $F3EB: NOTE_G5 [tune0ch2] */
  0x80, /* $F3EC: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F3ED: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F3EE: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F3EF: NOTE_B5 [tune0ch2] */
  0xE7, /* $F3F0: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F3F1: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F3F2: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F3F3: NOTE_B5 [tune0ch2] */
  0xEB, /* $F3F4: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F3F5: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F3F6: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F3F7: NOTE_A5 [tune0ch2] */
  0xF7, /* $F3F8: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x8F, /* $F3F9: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE7, /* $F3FA: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F3FB: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F3FC: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0xF7, /* $F3FD: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x39, /* $F3FE: NOTE_G5 [tune0ch2] */
  0xE3, /* $F3FF: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F400: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F401: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3D, /* $F402: NOTE_B5 [tune0ch2] */
  0x80, /* $F403: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F404: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3B, /* $F405: NOTE_A5 [tune0ch2] */
  0x80, /* $F406: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x39, /* $F407: NOTE_G5 [tune0ch2] */
  0x80, /* $F408: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBF, /* $F409: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0x37, /* $F40A: NOTE_F5 [tune0ch2] */
  0xE7, /* $F40B: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F40C: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F40D: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F40E: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F40F: NOTE_B5 [tune0ch2] */
  0xEB, /* $F410: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F411: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F412: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F413: NOTE_B5 [tune0ch2] */
  0xEB, /* $F414: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F415: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F416: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F417: NOTE_A5 [tune0ch2] */
  0xF7, /* $F418: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x8F, /* $F419: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F41A: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F41B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F41C: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x39, /* $F41D: NOTE_G5 [tune0ch2] */
  0xF7, /* $F41E: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x8F, /* $F41F: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F420: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F421: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xC1, /* $F422: PCMD_SELECT_PITCH_OFFSET(9) [tune0ch2] */
  0x38, /* $F423: NOTE_FS5 [tune0ch2] */
  0x80, /* $F424: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x38, /* $F425: NOTE_FS5 [tune0ch2] */
  0xE7, /* $F426: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F427: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F428: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x38, /* $F429: NOTE_FS5 [tune0ch2] */
  0xEB, /* $F42A: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F42B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F42C: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x38, /* $F42D: NOTE_FS5 [tune0ch2] */
  0xE7, /* $F42E: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F42F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F430: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x38, /* $F431: NOTE_FS5 [tune0ch2] */
  0xEF, /* $F432: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x80, /* $F433: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBF, /* $F434: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0xE3, /* $F435: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x39, /* $F436: NOTE_G5 [tune0ch2] */
  0xE7, /* $F437: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F438: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F439: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xEB, /* $F43A: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3A, /* $F43B: NOTE_GS5 [tune0ch2] */
  0xE7, /* $F43C: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F43D: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBF, /* $F43E: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0xE3, /* $F43F: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F440: NOTE_A5 [tune0ch2] */
  0xE7, /* $F441: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F442: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F443: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xEB, /* $F444: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3C, /* $F445: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F446: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F447: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F448: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F449: NOTE_B5 [tune0ch2] */
  0xE7, /* $F44A: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F44B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEB, /* $F44C: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3D, /* $F44D: NOTE_B5 [tune0ch2] */
  0xE3, /* $F44E: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F44F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3D, /* $F450: NOTE_B5 [tune0ch2] */
  0xBF, /* $F451: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0x3E, /* $F452: NOTE_C6 [tune0ch2] */
  0x80, /* $F453: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3F, /* $F454: NOTE_CS6 [tune0ch2] */
  0x40, /* $F455: NOTE_D6 [tune0ch2] */
  0x80, /* $F456: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F457: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x44, /* $F458: NOTE_FS6 [tune0ch2] */
  0x80, /* $F459: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x80, /* $F45A: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBF, /* $F45B: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0xE3, /* $F45C: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x39, /* $F45D: NOTE_G5 [tune0ch2] */
  0xE7, /* $F45E: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F45F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F460: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xEB, /* $F461: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3A, /* $F462: NOTE_GS5 [tune0ch2] */
  0xE7, /* $F463: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F464: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBF, /* $F465: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0xE3, /* $F466: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F467: NOTE_A5 [tune0ch2] */
  0xE7, /* $F468: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F469: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F46A: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xEB, /* $F46B: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3C, /* $F46C: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F46D: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F46E: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F46F: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F470: NOTE_B5 [tune0ch2] */
  0xE7, /* $F471: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F472: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEB, /* $F473: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3D, /* $F474: NOTE_B5 [tune0ch2] */
  0xE3, /* $F475: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0xBF, /* $F476: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0x80, /* $F477: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3C, /* $F478: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F479: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x8F, /* $F47A: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F47B: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F47C: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEB, /* $F47D: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3E, /* $F47E: NOTE_C6 [tune0ch2] */
  0xE3, /* $F47F: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F480: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3C, /* $F481: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F482: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x8F, /* $F483: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F484: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F485: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEB, /* $F486: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3E, /* $F487: NOTE_C6 [tune0ch2] */
  0xE3, /* $F488: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F489: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3C, /* $F48A: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F48B: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x8F, /* $F48C: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F48D: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F48E: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEB, /* $F48F: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3E, /* $F490: NOTE_C6 [tune0ch2] */
  0xE3, /* $F491: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F492: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3C, /* $F493: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F494: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x8F, /* $F495: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F496: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F497: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xEB, /* $F498: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x3E, /* $F499: NOTE_C6 [tune0ch2] */
  0xE3, /* $F49A: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F49B: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x3C, /* $F49C: NOTE_AS5 [tune0ch2] */
  0xE7, /* $F49D: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x8F, /* $F49E: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE7, /* $F49F: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x3E, /* $F4A0: NOTE_C6 [tune0ch2] */
  0xE3, /* $F4A1: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F4A2: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE7, /* $F4A3: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0xBE, /* $F4A4: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0x3B, /* $F4A5: NOTE_A5 [tune0ch2] */
  0xBD, /* $F4A6: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F4A7: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F4A8: NOTE_B5 [tune0ch2] */
  0xE7, /* $F4A9: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F4AA: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F4AB: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F4AC: NOTE_B5 [tune0ch2] */
  0xEB, /* $F4AD: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F4AE: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F4AF: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F4B0: NOTE_A5 [tune0ch2] */
  0xF7, /* $F4B1: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x8F, /* $F4B2: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE7, /* $F4B3: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F4B4: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBE, /* $F4B5: PCMD_SELECT_PITCH_OFFSET(6) [tune0ch2] */
  0xF7, /* $F4B6: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x39, /* $F4B7: NOTE_G5 [tune0ch2] */
  0xE3, /* $F4B8: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F4B9: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F4BA: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3D, /* $F4BB: NOTE_B5 [tune0ch2] */
  0x80, /* $F4BC: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F4BD: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x3B, /* $F4BE: NOTE_A5 [tune0ch2] */
  0x80, /* $F4BF: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x39, /* $F4C0: NOTE_G5 [tune0ch2] */
  0x80, /* $F4C1: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBF, /* $F4C2: PCMD_SELECT_PITCH_OFFSET(7) [tune0ch2] */
  0x37, /* $F4C3: NOTE_F5 [tune0ch2] */
  0xE7, /* $F4C4: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F4C5: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xBD, /* $F4C6: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0xE3, /* $F4C7: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F4C8: NOTE_B5 [tune0ch2] */
  0xE7, /* $F4C9: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F4CA: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F4CB: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3D, /* $F4CC: NOTE_B5 [tune0ch2] */
  0xEB, /* $F4CD: PCMD_SET_ROW_WAIT(12) [tune0ch2] */
  0x80, /* $F4CE: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F4CF: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x3B, /* $F4D0: NOTE_A5 [tune0ch2] */
  0xF7, /* $F4D1: PCMD_SET_ROW_WAIT(24) [tune0ch2] */
  0x8F, /* $F4D2: PCMD_RESET_ROW_COUNTER [tune0ch2] */
  0xE3, /* $F4D3: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0xBD, /* $F4D4: PCMD_SELECT_PITCH_OFFSET(5) [tune0ch2] */
  0x80, /* $F4D5: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x80, /* $F4D6: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F4D7: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0x80, /* $F4D8: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE7, /* $F4D9: PCMD_SET_ROW_WAIT(8) [tune0ch2] */
  0x80, /* $F4DA: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0xE3, /* $F4DB: PCMD_SET_ROW_WAIT(4) [tune0ch2] */
  0xC1, /* $F4DC: PCMD_SELECT_PITCH_OFFSET(9) [tune0ch2] */
  0x38, /* $F4DD: NOTE_FS5 [tune0ch2] */
  0xEF, /* $F4DE: PCMD_SET_ROW_WAIT(16) [tune0ch2] */
  0x80, /* $F4DF: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune0ch2] */
  0x87, /* $F4E0: PCMD_ADVANCE_PHRASE [tune0ch2] */
  0x8A, /* $F4E1: PCMD_SET_MIXER_BITS_HIGH3 [tune0ch1] */
  0xC0, /* $F4E2: -- unreached by tune 0/1 playback -- */
  0x81, /* $F4E3: -- unreached by tune 0/1 playback -- */
  0xD3, /* $F4E4: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F4E5: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4E6: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F4E7: -- unreached by tune 0/1 playback -- */
  0x39, /* $F4E8: -- unreached by tune 0/1 playback -- */
  0x39, /* $F4E9: -- unreached by tune 0/1 playback -- */
  0x39, /* $F4EA: -- unreached by tune 0/1 playback -- */
  0x37, /* $F4EB: -- unreached by tune 0/1 playback -- */
  0x36, /* $F4EC: -- unreached by tune 0/1 playback -- */
  0x37, /* $F4ED: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4EE: -- unreached by tune 0/1 playback -- */
  0x39, /* $F4EF: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4F0: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F4F1: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4F2: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F4F3: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4F4: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F4F5: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4F6: -- unreached by tune 0/1 playback -- */
  0x39, /* $F4F7: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4F8: -- unreached by tune 0/1 playback -- */
  0x34, /* $F4F9: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4FA: -- unreached by tune 0/1 playback -- */
  0x32, /* $F4FB: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4FC: -- unreached by tune 0/1 playback -- */
  0x31, /* $F4FD: -- unreached by tune 0/1 playback -- */
  0x80, /* $F4FE: -- unreached by tune 0/1 playback -- */
  0x39, /* $F4FF: -- unreached by tune 0/1 playback -- */
  0xEB, /* $F500: -- unreached by tune 0/1 playback -- */
  0x80, /* $F501: -- unreached by tune 0/1 playback -- */
  0xE1, /* $F502: -- unreached by tune 0/1 playback -- */
  0x31, /* $F503: -- unreached by tune 0/1 playback -- */
  0x32, /* $F504: -- unreached by tune 0/1 playback -- */
  0x34, /* $F505: -- unreached by tune 0/1 playback -- */
  0x36, /* $F506: -- unreached by tune 0/1 playback -- */
  0x37, /* $F507: -- unreached by tune 0/1 playback -- */
  0x38, /* $F508: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F509: -- unreached by tune 0/1 playback -- */
  0x39, /* $F50A: -- unreached by tune 0/1 playback -- */
  0x80, /* $F50B: -- unreached by tune 0/1 playback -- */
  0x39, /* $F50C: -- unreached by tune 0/1 playback -- */
  0x39, /* $F50D: -- unreached by tune 0/1 playback -- */
  0x39, /* $F50E: -- unreached by tune 0/1 playback -- */
  0x37, /* $F50F: -- unreached by tune 0/1 playback -- */
  0x36, /* $F510: -- unreached by tune 0/1 playback -- */
  0x37, /* $F511: -- unreached by tune 0/1 playback -- */
  0x80, /* $F512: -- unreached by tune 0/1 playback -- */
  0x39, /* $F513: -- unreached by tune 0/1 playback -- */
  0x80, /* $F514: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F515: -- unreached by tune 0/1 playback -- */
  0x80, /* $F516: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F517: -- unreached by tune 0/1 playback -- */
  0x80, /* $F518: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F519: -- unreached by tune 0/1 playback -- */
  0x80, /* $F51A: -- unreached by tune 0/1 playback -- */
  0x39, /* $F51B: -- unreached by tune 0/1 playback -- */
  0x80, /* $F51C: -- unreached by tune 0/1 playback -- */
  0x34, /* $F51D: -- unreached by tune 0/1 playback -- */
  0x80, /* $F51E: -- unreached by tune 0/1 playback -- */
  0x32, /* $F51F: -- unreached by tune 0/1 playback -- */
  0x80, /* $F520: -- unreached by tune 0/1 playback -- */
  0x31, /* $F521: -- unreached by tune 0/1 playback -- */
  0x80, /* $F522: -- unreached by tune 0/1 playback -- */
  0x39, /* $F523: -- unreached by tune 0/1 playback -- */
  0xF7, /* $F524: -- unreached by tune 0/1 playback -- */
  0x80, /* $F525: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F526: -- unreached by tune 0/1 playback -- */
  0x80, /* $F527: -- unreached by tune 0/1 playback -- */
  0xB8, /* $F528: -- unreached by tune 0/1 playback -- */
  0x88, /* $F529: -- unreached by tune 0/1 playback -- */
  0x02, /* $F52A: -- unreached by tune 0/1 playback -- */
  0x03, /* $F52B: -- unreached by tune 0/1 playback -- */
  0x82, /* $F52C: -- unreached by tune 0/1 playback -- */
  0xEB, /* $F52D: -- unreached by tune 0/1 playback -- */
  0x40, /* $F52E: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F52F: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F530: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F531: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F532: -- unreached by tune 0/1 playback -- */
  0xFF, /* $F533: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F534: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F535: -- unreached by tune 0/1 playback -- */
  0x80, /* $F536: -- unreached by tune 0/1 playback -- */
  0xEB, /* $F537: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F538: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F539: -- unreached by tune 0/1 playback -- */
  0x36, /* $F53A: -- unreached by tune 0/1 playback -- */
  0x39, /* $F53B: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F53C: -- unreached by tune 0/1 playback -- */
  0x80, /* $F53D: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F53E: -- unreached by tune 0/1 playback -- */
  0x80, /* $F53F: -- unreached by tune 0/1 playback -- */
  0x39, /* $F540: -- unreached by tune 0/1 playback -- */
  0x80, /* $F541: -- unreached by tune 0/1 playback -- */
  0x37, /* $F542: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F543: -- unreached by tune 0/1 playback -- */
  0x80, /* $F544: -- unreached by tune 0/1 playback -- */
  0x81, /* $F545: -- unreached by tune 0/1 playback -- */
  0xC0, /* $F546: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F547: -- unreached by tune 0/1 playback -- */
  0x80, /* $F548: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F549: -- unreached by tune 0/1 playback -- */
  0x40, /* $F54A: -- unreached by tune 0/1 playback -- */
  0x40, /* $F54B: -- unreached by tune 0/1 playback -- */
  0x40, /* $F54C: -- unreached by tune 0/1 playback -- */
  0x42, /* $F54D: -- unreached by tune 0/1 playback -- */
  0x43, /* $F54E: -- unreached by tune 0/1 playback -- */
  0x43, /* $F54F: -- unreached by tune 0/1 playback -- */
  0x80, /* $F550: -- unreached by tune 0/1 playback -- */
  0x42, /* $F551: -- unreached by tune 0/1 playback -- */
  0x80, /* $F552: -- unreached by tune 0/1 playback -- */
  0x40, /* $F553: -- unreached by tune 0/1 playback -- */
  0x80, /* $F554: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F555: -- unreached by tune 0/1 playback -- */
  0x80, /* $F556: -- unreached by tune 0/1 playback -- */
  0x42, /* $F557: -- unreached by tune 0/1 playback -- */
  0x80, /* $F558: -- unreached by tune 0/1 playback -- */
  0x40, /* $F559: -- unreached by tune 0/1 playback -- */
  0x80, /* $F55A: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F55B: -- unreached by tune 0/1 playback -- */
  0x80, /* $F55C: -- unreached by tune 0/1 playback -- */
  0x40, /* $F55D: -- unreached by tune 0/1 playback -- */
  0x42, /* $F55E: -- unreached by tune 0/1 playback -- */
  0x40, /* $F55F: -- unreached by tune 0/1 playback -- */
  0xFF, /* $F560: -- unreached by tune 0/1 playback -- */
  0x80, /* $F561: -- unreached by tune 0/1 playback -- */
  0x80, /* $F562: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F563: -- unreached by tune 0/1 playback -- */
  0x39, /* $F564: -- unreached by tune 0/1 playback -- */
  0x38, /* $F565: -- unreached by tune 0/1 playback -- */
  0x37, /* $F566: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F567: -- unreached by tune 0/1 playback -- */
  0x36, /* $F568: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F569: -- unreached by tune 0/1 playback -- */
  0x37, /* $F56A: -- unreached by tune 0/1 playback -- */
  0x38, /* $F56B: -- unreached by tune 0/1 playback -- */
  0x39, /* $F56C: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F56D: -- unreached by tune 0/1 playback -- */
  0x3A, /* $F56E: -- unreached by tune 0/1 playback -- */
  0x39, /* $F56F: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F570: -- unreached by tune 0/1 playback -- */
  0x38, /* $F571: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F572: -- unreached by tune 0/1 playback -- */
  0x39, /* $F573: -- unreached by tune 0/1 playback -- */
  0x3A, /* $F574: -- unreached by tune 0/1 playback -- */
  0x80, /* $F575: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F576: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F577: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F578: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F579: -- unreached by tune 0/1 playback -- */
  0x39, /* $F57A: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F57B: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F57C: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F57D: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F57E: -- unreached by tune 0/1 playback -- */
  0x80, /* $F57F: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F580: -- unreached by tune 0/1 playback -- */
  0x80, /* $F581: -- unreached by tune 0/1 playback -- */
  0x3F, /* $F582: -- unreached by tune 0/1 playback -- */
  0x40, /* $F583: -- unreached by tune 0/1 playback -- */
  0x34, /* $F584: -- unreached by tune 0/1 playback -- */
  0x34, /* $F585: -- unreached by tune 0/1 playback -- */
  0x80, /* $F586: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F587: -- unreached by tune 0/1 playback -- */
  0x39, /* $F588: -- unreached by tune 0/1 playback -- */
  0x38, /* $F589: -- unreached by tune 0/1 playback -- */
  0x37, /* $F58A: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F58B: -- unreached by tune 0/1 playback -- */
  0x36, /* $F58C: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F58D: -- unreached by tune 0/1 playback -- */
  0x37, /* $F58E: -- unreached by tune 0/1 playback -- */
  0x38, /* $F58F: -- unreached by tune 0/1 playback -- */
  0x39, /* $F590: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F591: -- unreached by tune 0/1 playback -- */
  0x3A, /* $F592: -- unreached by tune 0/1 playback -- */
  0x39, /* $F593: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F594: -- unreached by tune 0/1 playback -- */
  0x38, /* $F595: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F596: -- unreached by tune 0/1 playback -- */
  0x39, /* $F597: -- unreached by tune 0/1 playback -- */
  0x3A, /* $F598: -- unreached by tune 0/1 playback -- */
  0x80, /* $F599: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F59A: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F59B: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F59C: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F59D: -- unreached by tune 0/1 playback -- */
  0x39, /* $F59E: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F59F: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F5A0: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F5A1: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F5A2: -- unreached by tune 0/1 playback -- */
  0xEB, /* $F5A3: -- unreached by tune 0/1 playback -- */
  0x8F, /* $F5A4: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5A5: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5A6: -- unreached by tune 0/1 playback -- */
  0xFF, /* $F5A7: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5A8: -- unreached by tune 0/1 playback -- */
  0xEF, /* $F5A9: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5AA: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5AB: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5AC: -- unreached by tune 0/1 playback -- */
  0x39, /* $F5AD: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5AE: -- unreached by tune 0/1 playback -- */
  0x45, /* $F5AF: -- unreached by tune 0/1 playback -- */
  0x42, /* $F5B0: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5B1: -- unreached by tune 0/1 playback -- */
  0x39, /* $F5B2: -- unreached by tune 0/1 playback -- */
  0xEF, /* $F5B3: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F5B4: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5B5: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5B6: -- unreached by tune 0/1 playback -- */
  0xEF, /* $F5B7: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5B8: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5B9: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5BA: -- unreached by tune 0/1 playback -- */
  0x39, /* $F5BB: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5BC: -- unreached by tune 0/1 playback -- */
  0x45, /* $F5BD: -- unreached by tune 0/1 playback -- */
  0x42, /* $F5BE: -- unreached by tune 0/1 playback -- */
  0xE1, /* $F5BF: -- unreached by tune 0/1 playback -- */
  0x31, /* $F5C0: -- unreached by tune 0/1 playback -- */
  0x32, /* $F5C1: -- unreached by tune 0/1 playback -- */
  0x34, /* $F5C2: -- unreached by tune 0/1 playback -- */
  0x36, /* $F5C3: -- unreached by tune 0/1 playback -- */
  0x37, /* $F5C4: -- unreached by tune 0/1 playback -- */
  0x38, /* $F5C5: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F5C6: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5C7: -- unreached by tune 0/1 playback -- */
  0xB8, /* $F5C8: -- unreached by tune 0/1 playback -- */
  0x88, /* $F5C9: -- unreached by tune 0/1 playback -- */
  0x02, /* $F5CA: -- unreached by tune 0/1 playback -- */
  0x03, /* $F5CB: -- unreached by tune 0/1 playback -- */
  0x82, /* $F5CC: -- unreached by tune 0/1 playback -- */
  0xEB, /* $F5CD: -- unreached by tune 0/1 playback -- */
  0x40, /* $F5CE: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5CF: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5D0: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F5D1: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5D2: -- unreached by tune 0/1 playback -- */
  0xFF, /* $F5D3: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5D4: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F5D5: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5D6: -- unreached by tune 0/1 playback -- */
  0xEB, /* $F5D7: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5D8: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5D9: -- unreached by tune 0/1 playback -- */
  0x36, /* $F5DA: -- unreached by tune 0/1 playback -- */
  0x39, /* $F5DB: -- unreached by tune 0/1 playback -- */
  0x3C, /* $F5DC: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5DD: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F5DE: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5DF: -- unreached by tune 0/1 playback -- */
  0x39, /* $F5E0: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5E1: -- unreached by tune 0/1 playback -- */
  0x37, /* $F5E2: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F5E3: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5E4: -- unreached by tune 0/1 playback -- */
  0x81, /* $F5E5: -- unreached by tune 0/1 playback -- */
  0xC0, /* $F5E6: -- unreached by tune 0/1 playback -- */
  0xE7, /* $F5E7: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5E8: -- unreached by tune 0/1 playback -- */
  0xE3, /* $F5E9: -- unreached by tune 0/1 playback -- */
  0x40, /* $F5EA: -- unreached by tune 0/1 playback -- */
  0x40, /* $F5EB: -- unreached by tune 0/1 playback -- */
  0x40, /* $F5EC: -- unreached by tune 0/1 playback -- */
  0x42, /* $F5ED: -- unreached by tune 0/1 playback -- */
  0x43, /* $F5EE: -- unreached by tune 0/1 playback -- */
  0x43, /* $F5EF: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5F0: -- unreached by tune 0/1 playback -- */
  0x42, /* $F5F1: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5F2: -- unreached by tune 0/1 playback -- */
  0x40, /* $F5F3: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5F4: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5F5: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5F6: -- unreached by tune 0/1 playback -- */
  0x42, /* $F5F7: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5F8: -- unreached by tune 0/1 playback -- */
  0x40, /* $F5F9: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5FA: -- unreached by tune 0/1 playback -- */
  0x3E, /* $F5FB: -- unreached by tune 0/1 playback -- */
  0x80, /* $F5FC: -- unreached by tune 0/1 playback -- */
  0x3D, /* $F5FD: -- unreached by tune 0/1 playback -- */
  0x3B, /* $F5FE: -- unreached by tune 0/1 playback -- */
  0x39, /* $F5FF: -- unreached by tune 0/1 playback -- */
  0x8E, /* $F600: -- unreached by tune 0/1 playback -- */
};

/* $F601-$F6DE */
const u8 title_tune1_data[222] = {
  0x0D, /* $F601: HEADER_PATTERN_PTR [tune1ch0] */
  0xF6, /* $F602: (high byte) */
  0x00, /* $F603: PHRASE_TABLE_WORD */
  0x00, /* $F604: (high byte) */
  0x2E, /* $F605: HEADER_PATTERN_PTR [tune1ch1] */
  0xF6, /* $F606: (high byte) */
  0x00, /* $F607: PHRASE_TABLE_WORD */
  0x00, /* $F608: (high byte) */
  0x43, /* $F609: HEADER_PATTERN_PTR [tune1ch2] */
  0xF6, /* $F60A: (high byte) */
  0x00, /* $F60B: -- unreached by tune 0/1 playback -- */
  0x00, /* $F60C: -- unreached by tune 0/1 playback -- */
  0x8A, /* $F60D: PCMD_SET_MIXER_BITS_HIGH3 [tune1ch0] */
  0xC0, /* $F60E: PCMD_SELECT_PITCH_OFFSET(8) [tune1ch0] */
  0x81, /* $F60F: PCMD_VIBRATO_OFF [tune1ch0] */
  0xD4, /* $F610: PCMD_SELECT_ENVELOPE_SHAPE(4) [tune1ch0] */
  0x91, /* $F611: PCMD_UNMUTE_CHANNEL [tune1ch0] */
  0xE1, /* $F612: PCMD_SET_ROW_WAIT(2) [tune1ch0] */
  0x3C, /* $F613: NOTE_AS5 [tune1ch0] */
  0x3C, /* $F614: NOTE_AS5 [tune1ch0] */
  0x3C, /* $F615: NOTE_AS5 [tune1ch0] */
  0xE3, /* $F616: PCMD_SET_ROW_WAIT(4) [tune1ch0] */
  0x3B, /* $F617: NOTE_A5 [tune1ch0] */
  0x37, /* $F618: NOTE_F5 [tune1ch0] */
  0x3A, /* $F619: NOTE_GS5 [tune1ch0] */
  0x3A, /* $F61A: NOTE_GS5 [tune1ch0] */
  0xE1, /* $F61B: PCMD_SET_ROW_WAIT(2) [tune1ch0] */
  0x3A, /* $F61C: NOTE_GS5 [tune1ch0] */
  0xE3, /* $F61D: PCMD_SET_ROW_WAIT(4) [tune1ch0] */
  0x39, /* $F61E: NOTE_G5 [tune1ch0] */
  0x35, /* $F61F: NOTE_DS5 [tune1ch0] */
  0xE1, /* $F620: PCMD_SET_ROW_WAIT(2) [tune1ch0] */
  0x38, /* $F621: NOTE_FS5 [tune1ch0] */
  0x38, /* $F622: NOTE_FS5 [tune1ch0] */
  0x38, /* $F623: NOTE_FS5 [tune1ch0] */
  0xE3, /* $F624: PCMD_SET_ROW_WAIT(4) [tune1ch0] */
  0x37, /* $F625: NOTE_F5 [tune1ch0] */
  0x33, /* $F626: NOTE_CS5 [tune1ch0] */
  0xE7, /* $F627: PCMD_SET_ROW_WAIT(8) [tune1ch0] */
  0x35, /* $F628: NOTE_DS5 [tune1ch0] */
  0xE1, /* $F629: PCMD_SET_ROW_WAIT(2) [tune1ch0] */
  0x80, /* $F62A: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch0] */
  0xE7, /* $F62B: PCMD_SET_ROW_WAIT(8) [tune1ch0] */
  0x80, /* $F62C: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch0] */
  0x8E, /* $F62D: PCMD_UNUSED_8E [tune1ch0] */
  0x8A, /* $F62E: PCMD_SET_MIXER_BITS_HIGH3 [tune1ch0] */
  0x90, /* $F62F: PCMD_MUTE_CHANNEL [tune1ch0] */
  0xD3, /* $F630: PCMD_SELECT_ENVELOPE_SHAPE(3) [tune1ch0] */
  0xC2, /* $F631: PCMD_SELECT_PITCH_OFFSET(10) [tune1ch0] */
  0x81, /* $F632: PCMD_VIBRATO_OFF [tune1ch0] */
  0xEB, /* $F633: PCMD_SET_ROW_WAIT(12) [tune1ch0] */
  0x18, /* $F634: NOTE_AS2 [tune1ch0] */
  0xE3, /* $F635: PCMD_SET_ROW_WAIT(4) [tune1ch0] */
  0x80, /* $F636: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch0] */
  0xEB, /* $F637: PCMD_SET_ROW_WAIT(12) [tune1ch0] */
  0x16, /* $F638: NOTE_GS2 [tune1ch0] */
  0xE3, /* $F639: PCMD_SET_ROW_WAIT(4) [tune1ch0] */
  0x80, /* $F63A: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch0] */
  0xEB, /* $F63B: PCMD_SET_ROW_WAIT(12) [tune1ch0] */
  0x14, /* $F63C: NOTE_FS2 [tune1ch0] */
  0xE3, /* $F63D: PCMD_SET_ROW_WAIT(4) [tune1ch0] */
  0x80, /* $F63E: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch0] */
  0xE7, /* $F63F: PCMD_SET_ROW_WAIT(8) [tune1ch0] */
  0x11, /* $F640: NOTE_DS2 [tune1ch0] */
  0x80, /* $F641: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch0] */
  0x87, /* $F642: PCMD_ADVANCE_PHRASE [tune1ch0] */
  0xD5, /* $F643: PCMD_SELECT_ENVELOPE_SHAPE(5) [tune1ch2] */
  0x8A, /* $F644: PCMD_SET_MIXER_BITS_HIGH3 [tune1ch2] */
  0xB8, /* $F645: PCMD_SELECT_PITCH_OFFSET(0) [tune1ch2] */
  0x81, /* $F646: PCMD_VIBRATO_OFF [tune1ch2] */
  0x90, /* $F647: PCMD_MUTE_CHANNEL [tune1ch2] */
  0xE1, /* $F648: PCMD_SET_ROW_WAIT(2) [tune1ch2] */
  0x4D, /* $F649: NOTE_DS7 [tune1ch2] */
  0x4C, /* $F64A: NOTE_D7 [tune1ch2] */
  0x4B, /* $F64B: NOTE_CS7 [tune1ch2] */
  0x4A, /* $F64C: NOTE_C7 [tune1ch2] */
  0x49, /* $F64D: NOTE_B6 [tune1ch2] */
  0x48, /* $F64E: NOTE_AS6 [tune1ch2] */
  0x47, /* $F64F: NOTE_A6 [tune1ch2] */
  0x46, /* $F650: NOTE_GS6 [tune1ch2] */
  0x45, /* $F651: NOTE_G6 [tune1ch2] */
  0x44, /* $F652: NOTE_FS6 [tune1ch2] */
  0x43, /* $F653: NOTE_F6 [tune1ch2] */
  0x42, /* $F654: NOTE_E6 [tune1ch2] */
  0x41, /* $F655: NOTE_DS6 [tune1ch2] */
  0x40, /* $F656: NOTE_D6 [tune1ch2] */
  0x3F, /* $F657: NOTE_CS6 [tune1ch2] */
  0x3E, /* $F658: NOTE_C6 [tune1ch2] */
  0x3D, /* $F659: NOTE_B5 [tune1ch2] */
  0x3C, /* $F65A: NOTE_AS5 [tune1ch2] */
  0x3B, /* $F65B: NOTE_A5 [tune1ch2] */
  0x3A, /* $F65C: NOTE_GS5 [tune1ch2] */
  0x39, /* $F65D: NOTE_G5 [tune1ch2] */
  0x38, /* $F65E: NOTE_FS5 [tune1ch2] */
  0x37, /* $F65F: NOTE_F5 [tune1ch2] */
  0x36, /* $F660: NOTE_E5 [tune1ch2] */
  0xEF, /* $F661: PCMD_SET_ROW_WAIT(16) [tune1ch2] */
  0x8B, /* $F662: PCMD_SET_MIXER_BITS_LOW3 [tune1ch2] */
  0xD6, /* $F663: PCMD_SELECT_ENVELOPE_SHAPE(6) [tune1ch2] */
  0x00, /* $F664: NOTE_AS0 [tune1ch2] */
  0x8E, /* $F665: PCMD_UNUSED_8E [tune1ch2] */
  0x72, /* $F666: NOTE_RAW_0x72(OUT_OF_RANGE) [tune1ch2] */
  0xF6, /* $F667: PCMD_SET_ROW_WAIT(23) [tune1ch2] */
  0x00, /* $F668: NOTE_AS0 [tune1ch2] */
  0x00, /* $F669: NOTE_AS0 [tune1ch2] */
  0xB1, /* $F66A: PCMD_SET_TEMPO(2) [tune1ch2] */
  0xF6, /* $F66B: PCMD_SET_ROW_WAIT(23) [tune1ch2] */
  0x00, /* $F66C: NOTE_AS0 [tune1ch2] */
  0x00, /* $F66D: NOTE_AS0 [tune1ch2] */
  0xEB, /* $F66E: PCMD_SET_ROW_WAIT(12) [tune1ch2] */
  0xF6, /* $F66F: PCMD_SET_ROW_WAIT(23) [tune1ch2] */
  0x00, /* $F670: NOTE_AS0 [tune1ch2] */
  0x00, /* $F671: NOTE_AS0 [tune1ch2] */
  0x8A, /* $F672: PCMD_SET_MIXER_BITS_HIGH3 [tune1ch2] */
  0x81, /* $F673: PCMD_VIBRATO_OFF [tune1ch2] */
  0xC0, /* $F674: PCMD_SELECT_PITCH_OFFSET(8) [tune1ch2] */
  0x91, /* $F675: PCMD_UNMUTE_CHANNEL [tune1ch2] */
  0xD4, /* $F676: PCMD_SELECT_ENVELOPE_SHAPE(4) [tune1ch2] */
  0xE7, /* $F677: PCMD_SET_ROW_WAIT(8) [tune1ch2] */
  0x37, /* $F678: NOTE_F5 [tune1ch2] */
  0xE3, /* $F679: PCMD_SET_ROW_WAIT(4) [tune1ch2] */
  0x39, /* $F67A: NOTE_G5 [tune1ch2] */
  0x37, /* $F67B: NOTE_F5 [tune1ch2] */
  0x36, /* $F67C: NOTE_E5 [tune1ch2] */
  0x34, /* $F67D: NOTE_D5 [tune1ch2] */
  0x32, /* $F67E: NOTE_C5 [tune1ch2] */
  0x80, /* $F67F: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch2] */
  0xE7, /* $F680: PCMD_SET_ROW_WAIT(8) [tune1ch2] */
  0x34, /* $F681: NOTE_D5 [tune1ch2] */
  0xE3, /* $F682: PCMD_SET_ROW_WAIT(4) [tune1ch2] */
  0x37, /* $F683: NOTE_F5 [tune1ch2] */
  0x36, /* $F684: NOTE_E5 [tune1ch2] */
  0x34, /* $F685: NOTE_D5 [tune1ch2] */
  0x31, /* $F686: NOTE_B4 [tune1ch2] */
  0x2D, /* $F687: NOTE_G4 [tune1ch2] */
  0x80, /* $F688: PCMD_RESET_ROW_COUNTER_CLEAR_ENV [tune1ch2] */
  0xE7, /* $F689: PCMD_SET_ROW_WAIT(8) [tune1ch2] */
  0x2B, /* $F68A: NOTE_F4 [tune1ch2] */
  0xE3, /* $F68B: PCMD_SET_ROW_WAIT(4) [tune1ch2] */
  0x2D, /* $F68C: NOTE_G4 [tune1ch2] */
  0x2F, /* $F68D: NOTE_A4 [tune1ch2] */
  0x31, /* $F68E: NOTE_B4 [tune1ch2] */
  0x33, /* $F68F: NOTE_CS5 [tune1ch2] */
  0x35, /* $F690: NOTE_DS5 [tune1ch2] */
  0x37, /* $F691: NOTE_F5 [tune1ch2] */
  0x39, /* $F692: NOTE_G5 [tune1ch2] */
  0x33, /* $F693: NOTE_CS5 [tune1ch2] */
  0x35, /* $F694: NOTE_DS5 [tune1ch2] */
  0x37, /* $F695: NOTE_F5 [tune1ch2] */
  0x39, /* $F696: NOTE_G5 [tune1ch2] */
  0x35, /* $F697: NOTE_DS5 [tune1ch2] */
  0x37, /* $F698: NOTE_F5 [tune1ch2] */
  0x39, /* $F699: NOTE_G5 [tune1ch2] */
  0x3B, /* $F69A: NOTE_A5 [tune1ch2] */
  0x37, /* $F69B: NOTE_F5 [tune1ch2] */
  0x39, /* $F69C: NOTE_G5 [tune1ch2] */
  0x3B, /* $F69D: NOTE_A5 [tune1ch2] */
  0x3D, /* $F69E: NOTE_B5 [tune1ch2] */
  0x39, /* $F69F: NOTE_G5 [tune1ch2] */
  0x3B, /* $F6A0: NOTE_A5 [tune1ch2] */
  0x3D, /* $F6A1: NOTE_B5 [tune1ch2] */
  0x3F, /* $F6A2: NOTE_CS6 [tune1ch2] */
  0x3B, /* $F6A3: NOTE_A5 [tune1ch2] */
  0x3D, /* $F6A4: NOTE_B5 [tune1ch2] */
  0x3F, /* $F6A5: NOTE_CS6 [tune1ch2] */
  0x41, /* $F6A6: NOTE_DS6 [tune1ch2] */
  0xE1, /* $F6A7: PCMD_SET_ROW_WAIT(2) [tune1ch2] */
  0x43, /* $F6A8: NOTE_F6 [tune1ch2] */
  0x41, /* $F6A9: NOTE_DS6 [tune1ch2] */
  0x3F, /* $F6AA: NOTE_CS6 [tune1ch2] */
  0x3D, /* $F6AB: NOTE_B5 [tune1ch2] */
  0x3B, /* $F6AC: NOTE_A5 [tune1ch2] */
  0x39, /* $F6AD: NOTE_G5 [tune1ch2] */
  0x37, /* $F6AE: NOTE_F5 [tune1ch2] */
  0x35, /* $F6AF: NOTE_DS5 [tune1ch2] */
  0x8E, /* $F6B0: PCMD_UNUSED_8E [tune1ch2] */
  0x8A, /* $F6B1: PCMD_SET_MIXER_BITS_HIGH3 [tune1ch2] */
  0x90, /* $F6B2: PCMD_MUTE_CHANNEL [tune1ch2] */
  0xD0, /* $F6B3: PCMD_SELECT_ENVELOPE_SHAPE(0) [tune1ch2] */
  0xB9, /* $F6B4: PCMD_SELECT_PITCH_OFFSET(1) [tune1ch2] */
  0x81, /* $F6B5: PCMD_VIBRATO_OFF [tune1ch2] */
  0xE3, /* $F6B6: PCMD_SET_ROW_WAIT(4) [tune1ch2] */
  0x21, /* $F6B7: NOTE_G3 [tune1ch2] */
  0x15, /* $F6B8: NOTE_G2 [tune1ch2] */
  0x21, /* $F6B9: NOTE_G3 [tune1ch2] */
  0x15, /* $F6BA: NOTE_G2 [tune1ch2] */
  0x21, /* $F6BB: NOTE_G3 [tune1ch2] */
  0x15, /* $F6BC: NOTE_G2 [tune1ch2] */
  0x21, /* $F6BD: NOTE_G3 [tune1ch2] */
  0x15, /* $F6BE: NOTE_G2 [tune1ch2] */
  0x21, /* $F6BF: NOTE_G3 [tune1ch2] */
  0x15, /* $F6C0: NOTE_G2 [tune1ch2] */
  0x21, /* $F6C1: NOTE_G3 [tune1ch2] */
  0x15, /* $F6C2: NOTE_G2 [tune1ch2] */
  0x21, /* $F6C3: NOTE_G3 [tune1ch2] */
  0x15, /* $F6C4: NOTE_G2 [tune1ch2] */
  0x21, /* $F6C5: NOTE_G3 [tune1ch2] */
  0x15, /* $F6C6: NOTE_G2 [tune1ch2] */
  0x21, /* $F6C7: NOTE_G3 [tune1ch2] */
  0x15, /* $F6C8: NOTE_G2 [tune1ch2] */
  0x21, /* $F6C9: NOTE_G3 [tune1ch2] */
  0x15, /* $F6CA: NOTE_G2 [tune1ch2] */
  0x21, /* $F6CB: NOTE_G3 [tune1ch2] */
  0x15, /* $F6CC: NOTE_G2 [tune1ch2] */
  0x21, /* $F6CD: NOTE_G3 [tune1ch2] */
  0x15, /* $F6CE: NOTE_G2 [tune1ch2] */
  0x21, /* $F6CF: NOTE_G3 [tune1ch2] */
  0x15, /* $F6D0: NOTE_G2 [tune1ch2] */
  0x21, /* $F6D1: NOTE_G3 [tune1ch2] */
  0x12, /* $F6D2: NOTE_E2 [tune1ch2] */
  0x1E, /* $F6D3: NOTE_E3 [tune1ch2] */
  0x15, /* $F6D4: NOTE_G2 [tune1ch2] */
  0x21, /* $F6D5: NOTE_G3 [tune1ch2] */
  0x14, /* $F6D6: NOTE_FS2 [tune1ch2] */
  0x20, /* $F6D7: NOTE_FS3 [tune1ch2] */
  0x15, /* $F6D8: NOTE_G2 [tune1ch2] */
  0x21, /* $F6D9: NOTE_G3 [tune1ch2] */
  0x12, /* $F6DA: NOTE_E2 [tune1ch2] */
  0x1E, /* $F6DB: NOTE_E3 [tune1ch2] */
  0x15, /* $F6DC: NOTE_G2 [tune1ch2] */
  0x21, /* $F6DD: NOTE_G3 [tune1ch2] */
  0x14, /* $F6DE: NOTE_FS2 [tune1ch2] */
};
