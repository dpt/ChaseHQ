// ChaseHQ.h
//
// Chase H.Q. code model
//
// by dpt

#ifndef CHASEHQ_H
#define CHASEHQ_H

#include "C99/Types.h"
#include "ZXSpectrum/Spectrum.h"

/* ----------------------------------------------------------------------- */

#define CHQ_API

/* ----------------------------------------------------------------------- */

/* Exports go here... */

/**
 * Holds the current state of the game.
 */
typedef struct chqstate chqstate_t;

/**
 * Create a game instance.
 */
CHQ_API chqstate_t *chq_create(zxspectrum_t *speccy);

/**
 * Destroy a game instance.
 */
CHQ_API void chq_destroy(chqstate_t *state);

/**
 * Prepare the game screen.
 */
CHQ_API void chq_setup(chqstate_t *state);

// /**
//  * Run the game menu.
//  *
//  * Call this repeatedly until it returns > 0.
//  *
//  * \return > 0 when it's time to continue on to chq_setup2.
//  */
// chq_API int chq_menu(chqstate_t *state);
//
// /**
//  * Prepare the game proper.
//  */
// chq_API void chq_setup2(chqstate_t *state);

/**
 * Invoke the game instance.
 *
 * Call this repeatedly.
 */
CHQ_API void chq_main(chqstate_t *state);

/* ----------------------------------------------------------------------- */

// TRY TO MOVE ALL THIS STUFF BELOW TO BE INTERNAL VISIBILITY ONLY

/* ----------------------------------------------------------------------- */

#define BACKBUFFER_WIDTH      (256)
#define BACKBUFFER_ROWBYTES   (BACKBUFFER_WIDTH / 8)
#define BACKBUFFER_HEIGHT     (128)
#define BACKBUFFER_LENGTH     (BACKBUFFER_ROWBYTES * BACKBUFFER_HEIGHT)
#define BACKBUFFER_START_ADDRESS ((u16) 0xF000)
#define BACKBUFFER_END_ADDRESS (BACKBUFFER_START_ADDRESS + BACKBUFFER_LENGTH)

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

#define DRAWCHARSTYLE_GENERIC                  (1)
#define DRAWCHARSTYLE_SINGLE                   (2)
#define DRAWCHARSTYLE_DOUBLE                   (3)
#define DRAWCHARSTYLE_SINGLE_INVERTED          (4)
#define DRAWCHARSTYLE_DOUBLE_INVERTED          (5)
#define DRAWCHARSTYLE__LIMIT                   (5)

#define HAZARD_USED                         (0xFF)
#define HAZARD_UNUSED                       (0x00)

#define USERINPUTMASK_ALLOW_NONE            (0x00)
#define USERINPUTMASK_ALLOW_ALL             (0xFF)

#define LODFLAG_DEFAULT                   (0 << 0)
#define LODFLAG_MASKED                    (1 << 0)
#define LODFLAG_FLIPPED                   (1 << 1)

#define TRANSITIONCONTROL_STOP                 (0)
#define TRANSITIONCONTROL_DRAW_MUGSHOTS        (1)
#define TRANSITIONCONTROL_OVERLAY_MESSAGES     (2)
#define TRANSITIONCONTROL_FILL_ATTRIBUTES      (3)
#define TRANSITIONCONTROL_FADE                 (4)

#define DRAWOVERLAY_STOP                       (0)

/* ----------------------------------------------------------------------- */

typedef void obj_handler_t(struct chqstate *state,
                           u8               Bheight_perhaps,
                           const void      *arg,
                           const u16       *IX,
                           const u8        *IY);

typedef struct hazard hazard_t;

typedef void (hazard_handler_t)(chqstate_t *state, hazard_t *IX);

typedef u8 chatterpriority_t;

typedef struct stagevars stagevars_t;

typedef struct carpart {
    u8        y;
    u8        rows;
    const u8 *bitmap;
} carpart_t;

typedef struct carsmokefram {
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

#endif /* CHASEHQ_H */
