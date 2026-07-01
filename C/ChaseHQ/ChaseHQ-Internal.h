/**
 * ChaseHQ-Internal.h
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

#ifndef CHASEHQ_INTERNAL_H
#define CHASEHQ_INTERNAL_H

#include "ChaseHQ.h"

/* ----------------------------------------------------------------------- */

#define BACKBUFFER_WIDTH      (256)
#define BACKBUFFER_ROWBYTES   (BACKBUFFER_WIDTH / 8)
#define BACKBUFFER_HEIGHT     (128)
#define BACKBUFFER_LENGTH     (BACKBUFFER_ROWBYTES * BACKBUFFER_HEIGHT)
#define BACKBUFFER_START_ADDRESS 0xF000
#define BACKBUFFER_END_ADDRESS (BACKBUFFER_START_ADDRESS + BACKBUFFER_LENGTH)
// draw_char writes rows at stride 256; addresses near the end of the buffer
// overflow past 0xFFFF in Z80 address space, wrapping into ROM (harmless).
// In C there is no wrap, so pad the allocation to absorb those writes.
#define BACKBUFFER_OVERFLOW   (BACKBUFFER_LENGTH)

#define MAXHAZARDS            (6)

#define TURBOWIDTH            (16) // pixels
#define TURBOROWBYTES         (TURBOWIDTH / 8)
#define TURBOHEIGHT           (14)
#define TURBOFRAMELENGTH      (TURBOROWBYTES * TURBOHEIGHT * 2) // masked
#define TURBOFRAMES           (3)

#define FACEWIDTH             (32)
#define FACEROWBYTES          (FACEWIDTH / 8)
#define FACEHEIGHT            (40)
#define FACEBITMAPBYTES       (FACEROWBYTES * FACEHEIGHT)
#define FACEATTRWIDTH         (FACEWIDTH / 8)
#define FACEATTRHEIGHT        (5)
#define FACEATTRBYTES         (FACEATTRWIDTH * FACEATTRHEIGHT)
#define FACEBYTES             (FACEBITMAPBYTES + FACEATTRBYTES)

#define NFACES                (3)

/* ----------------------------------------------------------------------- */

#define MARQUEE_HEIGHT                     (8 * 8) // rows
#define PLAYFIELD_HEIGHT                  (16 * 8) // rows
#define SCREEN_PLAYFIELD_BITMAP_ADDR      (0x4800) // first playfield scan line (char row 8)
#define SCREEN_PLAYFIELD_ATTRS_ADDR       (0x5900) // first playfield attribute (char row 8)

#define DRAWCHARSTYLE_SCREEN                   (1)
#define DRAWCHARSTYLE_SINGLE                   (2)
#define DRAWCHARSTYLE_DOUBLE                   (3)
#define DRAWCHARSTYLE_SINGLE_INVERTED          (4)
#define DRAWCHARSTYLE_DOUBLE_INVERTED          (5)
#define DRAWCHARSTYLE__LIMIT                   (5)

#define HAZARD_USED                         (0xFF)
#define HAZARD_UNUSED                       (0x00)

/* ----------------------------------------------------------------------- */

#define BITMAPFLAG_DEFAULT                (0 << 0)
#define BITMAPFLAG_MASKED                 (1 << 0)
#define BITMAPFLAG_FLIPPED                (1 << 1)

#define TRANSITIONCONTROL_STOP                 (0)
#define TRANSITIONCONTROL_DRAW_MUGSHOTS        (1)
#define TRANSITIONCONTROL_OVERLAY_MESSAGES     (2)
#define TRANSITIONCONTROL_FILL_ATTRIBUTES      (3)
#define TRANSITIONCONTROL_FADE                 (4)

#define DRAWOVERLAY_STOP                       (0)

#define STRETCHY_TYPE_END                      (1) // terminator (set=NULL)
#define STRETCHY_TYPE_FIXED                    (2) // height = bitmap->width_bytes - 2, no perspective scaling
#define STRETCHY_TYPE_150PC                    (3) // height = 150% of perspective scale
#define STRETCHY_TYPE_50PC                     (4) // height =  50%
#define STRETCHY_TYPE_113PC                    (5) // height = 112.5%
#define STRETCHY_TYPE_38PC                     (6) // height =  37.5%
#define STRETCHY_TYPE_75PC                     (7) // height =  75%
#define STRETCHY_TYPE_25PC                     (8) // height =  25%
#define STRETCHY_TYPE_100PC                    (9) // height = 100%

/* ----------------------------------------------------------------------- */

/* Flag constants */

#define KEYDEF_QUIT                            (0)
#define KEYDEF_PAUSE                           (1)
#define KEYDEF_BOOST                           (2)
#define KEYDEF_GEAR                            (3)
#define KEYDEF_ACCELERATE                      (4)
#define KEYDEF_BRAKE                           (5)
#define KEYDEF_LEFT                            (6)
#define KEYDEF_RIGHT                           (7)

#define USERINPUT_RIGHT                        (0)
#define USERINPUT_LEFT                         (1)
#define USERINPUT_DOWN                         (2) /* aka brake */
#define USERINPUT_UP                           (3) /* aka accelerate */
#define USERINPUT_FIRE                         (4) /* aka gear */
#define USERINPUT_BOOST                        (5)
#define USERINPUT_PAUSE                        (6)
#define USERINPUT_QUIT                         (7)

#define USERINPUTFLAG_RIGHT (1 << USERINPUT_RIGHT)
#define USERINPUTFLAG_LEFT  (1 << USERINPUT_LEFT )
#define USERINPUTFLAG_DOWN  (1 << USERINPUT_DOWN )
#define USERINPUTFLAG_UP    (1 << USERINPUT_UP   )
#define USERINPUTFLAG_FIRE  (1 << USERINPUT_FIRE )
#define USERINPUTFLAG_BOOST (1 << USERINPUT_BOOST)
#define USERINPUTFLAG_PAUSE (1 << USERINPUT_PAUSE)
#define USERINPUTFLAG_QUIT  (1 << USERINPUT_QUIT )

#define USERINPUTFLAGMASK_NONE              (0x00)
#define USERINPUTFLAGMASK_NOT_QUIT          (0x7F) /* mask of all input bits except QUIT */

#define USERINPUTFLAGMASK_ALLOW_NONE        (0x00)
#define USERINPUTFLAGMASK_ALLOW_ALL         (0xFF)

/* ----------------------------------------------------------------------- */

typedef void obj_handler_t(chqstate_t *state,
                           int         Biterations,
                           const void *arg,
                           const s16  *IXxpos,
                           const u8   *IYheight);

typedef struct hazard hazard_t;

typedef void (hazard_handler_t)(chqstate_t *state, hazard_t *IXhazard);

typedef u8 chatterpriority_t;

typedef struct session session_t;

typedef struct carpart {
  u8        y;
  u8        rows;
  const u8 *bitmap;
} carpart_t;

typedef struct carsmokeframe {
  u8        height;
  u8        width;
  u8        unflipped_x;
  u8        flipped_x;
  const u8 *bitmap;
} carsmokeframe_t;

typedef struct carframe {
  u8        y;
  u8        x;
  u8        index;
} carframe_t;

typedef struct aradornment {
  u8        height;
  u8        width;
  const u8 *bitmap;
} caradornment_t;

/* ----------------------------------------------------------------------- */

hazard_handler_t no_op;
hazard_handler_t perp_behaviour;
hazard_handler_t hazard_handler;

obj_handler_t draw_stretchy_object_left;
obj_handler_t draw_stretchy_object_right;
obj_handler_t draw_tunnel_light_left;
obj_handler_t draw_tunnel_light_right;
obj_handler_t draw_object_left;
obj_handler_t draw_object_right;

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_INTERNAL_H */
