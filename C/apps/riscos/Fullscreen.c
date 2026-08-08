/*******************************************************************
 * File:        Fullscreen
 * Purpose:     Non-Wimp fullscreen host for 64-bit RISC OS systems.
 * Author:      Gerph
 ******************************************************************/

#ifndef FULLSCREEN_ONLY
#error Fullscreen.c must be built with FULLSCREEN_ONLY defined
#endif

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kernel.h"
#include "swis.h"

#include "ChaseHQ/ChaseHQ.h"
#include "ZXSpectrum/Keyboard.h"
#include "ZXSpectrum/Kempston.h"
#include "ZXSpectrum/Spectrum.h"

#include "Host.h"

#define SPRITE_REASON_SCALE (52 + 512)
#define SPRITE_NAME_BYTES   (12)

typedef struct chq_sprite_area
{
    uint32_t size;
    uint32_t count;
    uint32_t first;
    uint32_t used;
}
chq_sprite_area_t;

typedef struct chq_sprite_header
{
    uint32_t next;
    char name[SPRITE_NAME_BYTES];
    uint32_t width;
    uint32_t height;
    uint32_t left_bit;
    uint32_t right_bit;
    uint32_t image;
    uint32_t mask;
    uint32_t mode;
}
chq_sprite_header_t;

typedef struct chq_scale_factors
{
    int32_t xmag;
    int32_t ymag;
    int32_t xdiv;
    int32_t ydiv;
}
chq_scale_factors_t;

typedef struct chq_fullscreen
{
    zxspectrum_t *zx;
    chqstate_t *game;
    chq_sprite_area_t *sprite_area;
    chq_sprite_header_t *sprite;
    uint32_t translation[CHQ_TRANSLATION_WORDS];
    zxkeyset_t keys;
    zxkempston_t kempston;
    chq_host_clock_t clock;
    int redraw_pending;
    int stop_requested;
    int fatal_error;
    int fullscreen_scale;
    int sprite_plot_action;
    chq_host_pointer_t pointer;
    int escape_installed;
    void (*desktop_escape_handler)(int);
    chq_host_saved_mode_t desktop_mode;
}
chq_fullscreen_t;

static volatile int fullscreen_escape;

static _kernel_oserror memory_error =
{
    0x80801, "ChaseHQ: not enough memory"
};

static _kernel_oserror mode_error =
{
    0x80802, "ChaseHQ: no suitable fullscreen mode"
};

/*******************************************************************
 Function:      report_error
 Description:   Report a RISC OS error to the command stream.
 Parameters:    error = error returned by a SWI
 Returns:       non-zero when an error was supplied
 ******************************************************************/
static int report_error(_kernel_oserror *error)
{
    if (error == NULL)
        return 0;
    fprintf(stderr, "ChaseHQ: %s\n", error->errmess);
    return 1;
}

/*******************************************************************
 Function:      fullscreen_escape_handler
 Description:   Defer Escape termination to the next engine callback.
 Parameters:    signal_number = delivered C signal
 Returns:       none
 ******************************************************************/
static void fullscreen_escape_handler(int signal_number)
{
    (void) signal_number;
    fullscreen_escape = 1;
    signal(SIGINT, fullscreen_escape_handler);
}

/*******************************************************************
 Function:      native_draw
 Description:   Defer display output to the next safe callback boundary.
 Parameters:    dirty = dirty Spectrum rectangle
                opaque = fullscreen state
 Returns:       none
 ******************************************************************/
static void native_draw(const zxbox_t *dirty, void *opaque)
{
    chq_fullscreen_t *app;

    (void) dirty;
    app = opaque;
    app->redraw_pending = 1;
}

/*******************************************************************
 Function:      native_stamp
 Description:   Mark the start of an engine timing segment.
 Parameters:    opaque = fullscreen state
 Returns:       none
 ******************************************************************/
static void native_stamp(void *opaque)
{
    (void) opaque;
}

/*******************************************************************
 Function:      native_key
 Description:   Translate held physical keys into Spectrum input state.
 Parameters:    port = Spectrum input port
                opaque = fullscreen state
 Returns:       emulated input port value
 ******************************************************************/
static int native_key(uint16_t port, void *opaque)
{
    chq_fullscreen_t *app;

    app = opaque;
    chq_host_poll_keys(&app->keys, &app->kempston);
    if (port == port_KEMPSTON_JOYSTICK)
        return app->kempston;
    return zxkeyset_for_port(port, &app->keys);
}

/*******************************************************************
 Function:      native_border
 Description:   Ignore the fixed black ChaseHQ border.
 Parameters:    colour = Spectrum border colour
                opaque = fullscreen state
 Returns:       none
 ******************************************************************/
static void native_border(int colour, void *opaque)
{
    (void) colour;
    (void) opaque;
}

/*******************************************************************
 Function:      set_screen_mode
 Description:   Select a mode without requiring the Wimp.
 Parameters:    mode = mode string or selector
                text_mode = non-zero for a mode string
 Returns:       error returned by OS_ScreenMode
 ******************************************************************/
static _kernel_oserror *set_screen_mode(const void *mode, int text_mode)
{
    if (text_mode)
        return _swix(OS_ScreenMode, _INR(0, 1), 15, mode);
    return _swix(OS_ScreenMode, _INR(0, 1), 0, mode);
}

/*******************************************************************
 Function:      select_fullscreen_mode
 Description:   Select the configured or closest suitable screen mode.
 Parameters:    none
 Returns:       error returned by the final mode selection attempt
 ******************************************************************/
static _kernel_oserror *select_fullscreen_mode(void)
{
    return chq_host_select_fullscreen_mode(set_screen_mode);
}

/*******************************************************************
 Function:      programme_fullscreen_palette
 Description:   Install exact Spectrum colours in a 16-colour mode.
 Parameters:    none
 Returns:       error returned by the documented VDU interface
 ******************************************************************/
static _kernel_oserror *programme_fullscreen_palette(void)
{
    return chq_host_programme_palette();
}

/*******************************************************************
 Function:      create_sprite
 Description:   Construct a private 256 by 192 indexed 4-bpp sprite.
 Parameters:    app = fullscreen state
 Returns:       non-zero on success
 ******************************************************************/
static int create_sprite(chq_fullscreen_t *app)
{
    unsigned int image_bytes;
    unsigned int sprite_bytes;

    app->sprite_area = malloc(CHQ_SPRITE_AREA_BYTES);
    if (app->sprite_area == NULL)
        return 0;
    memset(app->sprite_area, 0, CHQ_SPRITE_AREA_BYTES);
    image_bytes = SCREEN_WIDTH * SCREEN_HEIGHT / 2;
    sprite_bytes = sizeof(chq_sprite_header_t) + image_bytes;
    app->sprite_area->size = CHQ_SPRITE_AREA_BYTES;
    app->sprite_area->count = 1;
    app->sprite_area->first = sizeof(chq_sprite_area_t);
    app->sprite_area->used = sizeof(chq_sprite_area_t) + sprite_bytes;
    app->sprite = (chq_sprite_header_t *)
                  ((unsigned char *) app->sprite_area +
                   app->sprite_area->first);
    app->sprite->next = sprite_bytes;
    memcpy(app->sprite->name, "chqscreen", 10);
    app->sprite->width = SCREEN_WIDTH / 8 - 1;
    app->sprite->height = SCREEN_HEIGHT - 1;
    app->sprite->left_bit = 0;
    app->sprite->right_bit = 31;
    app->sprite->image = sizeof(chq_sprite_header_t);
    app->sprite->mask = sizeof(chq_sprite_header_t);
    app->sprite->mode = CHQ_SPRITE_MODE_INDEXED4;
    return 1;
}

/*******************************************************************
 Function:      update_translation
 Description:   Build the indexed-Spectrum to screen pixel table.
 Parameters:    app = fullscreen state
 Returns:       error returned by ColourTrans
 ******************************************************************/
static _kernel_oserror *update_translation(chq_fullscreen_t *app)
{
    return chq_host_build_translation(app->translation,
                                      &app->sprite_plot_action);
}

/*******************************************************************
 Function:      copy_frame_to_sprite
 Description:   Copy the converted Spectrum frame into sprite storage.
 Parameters:    app = fullscreen state
 Returns:       none
 ******************************************************************/
static void copy_frame_to_sprite(chq_fullscreen_t *app)
{
    const zx_frame_t *frame;
    unsigned char *destination;

    frame = zxspectrum_claim_screen(app->zx);
    destination = (unsigned char *) app->sprite + app->sprite->image;
    chq_host_copy_frame(destination, frame->pixels,
                        frame->height, frame->stride);
    zxspectrum_release_screen(app->zx);
}

/*******************************************************************
 Function:      draw_fullscreen
 Description:   Plot the game centred at the largest whole-pixel scale.
 Parameters:    app = fullscreen state
 Returns:       error returned by OS_SpriteOp
 ******************************************************************/
static _kernel_oserror *draw_fullscreen(chq_fullscreen_t *app)
{
    chq_scale_factors_t factors;
    chq_host_fullscreen_geometry_t geometry;
    _kernel_oserror *error;

    copy_frame_to_sprite(app);
    error = chq_host_fullscreen_geometry(app->fullscreen_scale,
                                         &geometry);
    if (error != NULL)
        return error;
    factors.xmag = app->fullscreen_scale;
    factors.ymag = app->fullscreen_scale;
    factors.xdiv = 1;
    factors.ydiv = 1;
    return _swix(OS_SpriteOp, _INR(0, 7),
                 SPRITE_REASON_SCALE, app->sprite_area, app->sprite,
                 (geometry.screen_width - geometry.plot_width) / 2,
                 (geometry.screen_height - geometry.plot_height) / 2,
                 app->sprite_plot_action, &factors, app->translation);
}

/*******************************************************************
 Function:      native_sleep
 Description:   Pace the engine, draw frames and honour Escape.
 Parameters:    duration = nominal duration in Z80 T-states
                opaque = fullscreen state
 Returns:       non-zero when the game must stop
 ******************************************************************/
static int native_sleep(int duration, void *opaque)
{
    chq_fullscreen_t *app;
    _kernel_oserror *error;
    uint32_t ticks;
    uint32_t now;

    app = opaque;
    now = chq_host_monotonic_time();
    ticks = chq_host_advance_clock(&app->clock,
                                   (uint32_t) duration,
                                   CHQ_CLOCK_128K, now);
    if (ticks != 0)
    {
        while (!fullscreen_escape &&
               chq_host_time_is_before(now, app->clock.deadline))
        {
            _swix(OS_Byte, _INR(0, 2), 19, 0, 0);
            now = chq_host_monotonic_time();
        }
    }
    error = NULL;
    if (app->redraw_pending && !fullscreen_escape)
    {
        error = draw_fullscreen(app);
        app->redraw_pending = 0;
    }
    if (error != NULL)
    {
        report_error(error);
        app->fatal_error = 1;
        app->stop_requested = 1;
    }
    if (fullscreen_escape || app->stop_requested)
    {
        chq_stop(app->game);
        return 1;
    }
    return 0;
}

/*******************************************************************
 Function:      create_spectrum
 Description:   Create the silent indexed Spectrum facade.
 Parameters:    app = fullscreen state
 Returns:       non-zero on success
 ******************************************************************/
static int create_spectrum(chq_fullscreen_t *app)
{
    zxconfig_t config;

    memset(&config, 0, sizeof(config));
    config.width = SCREEN_WIDTH / 8;
    config.height = SCREEN_HEIGHT / 8;
    config.opaque = app;
    config.draw = native_draw;
    config.stamp = native_stamp;
    config.sleep = native_sleep;
    config.key = native_key;
    config.border = native_border;
    config.pixel_format = ZX_PIXEL_INDEXED4;
    app->zx = zxspectrum_create(&config);
    return app->zx != NULL;
}

/*******************************************************************
 Function:      enter_fullscreen
 Description:   Save the desktop and initialise fullscreen output.
 Parameters:    app = fullscreen state
 Returns:       error returned during entry
 ******************************************************************/
static _kernel_oserror *enter_fullscreen(chq_fullscreen_t *app)
{
    _kernel_oserror *error;
    int xlimit;
    int ylimit;
    int scale_x;
    int scale_y;
    int log2bpp;

    error = chq_host_save_mode(&app->desktop_mode);
    if (error != NULL)
        return error;
    error = select_fullscreen_mode();
    if (error != NULL)
        return error;
    app->desktop_escape_handler = signal(SIGINT,
                                          fullscreen_escape_handler);
    app->escape_installed = 1;
    _swix(OS_Byte, _INR(0, 2), 229, 0, 0);
    error = chq_host_save_pointer(&app->pointer);
    if (error != NULL)
        return error;
    error = chq_host_hide_pointer(&app->pointer);
    if (error != NULL)
        return error;
    error = _swix(OS_WriteC, _IN(0), 12);
    if (error != NULL)
        return error;
    error = _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
                  -1, 9, &log2bpp);
    if (error != NULL)
        return error;
    if (log2bpp == CHQ_MODE_DEPTH_4BPP)
    {
        error = programme_fullscreen_palette();
        if (error != NULL)
            return error;
    }
    error = update_translation(app);
    if (error != NULL)
        return error;
    error = _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
                  -1, 11, &xlimit);
    if (error != NULL)
        return error;
    error = _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
                  -1, 12, &ylimit);
    if (error != NULL)
        return error;
    scale_x = (xlimit + 1) / SCREEN_WIDTH;
    scale_y = (ylimit + 1) / SCREEN_HEIGHT;
    app->fullscreen_scale = scale_x < scale_y ? scale_x : scale_y;
    if (app->fullscreen_scale < 1)
        return &mode_error;
    return NULL;
}

/*******************************************************************
 Function:      leave_fullscreen
 Description:   Restore Escape, cursors, desktop mode and pointer state.
 Parameters:    app = fullscreen state
 Returns:       first error encountered while restoring state
 ******************************************************************/
static _kernel_oserror *leave_fullscreen(chq_fullscreen_t *app)
{
    _kernel_oserror *error;
    _kernel_oserror *next;

    error = NULL;
    if (app->escape_installed)
    {
        _swix(OS_Byte, _INR(0, 2), 229, 1, 0);
        signal(SIGINT, app->desktop_escape_handler);
        app->escape_installed = 0;
    }
    error = chq_host_restore_cursors(&app->pointer);
    next = chq_host_restore_mode(&app->desktop_mode, set_screen_mode);
    if (error == NULL)
        error = next;
    next = chq_host_restore_pointer(&app->pointer);
    if (error == NULL)
        error = next;
    return error;
}

/*******************************************************************
 Function:      destroy_app
 Description:   Restore the desktop and release all owned resources.
 Parameters:    app = fullscreen state
 Returns:       none
 ******************************************************************/
static void destroy_app(chq_fullscreen_t *app)
{
    if (app->game != NULL)
    {
        chq_stop(app->game);
        chq_destroy(app->game);
    }
    if (app->zx != NULL)
        zxspectrum_destroy(app->zx);
    report_error(leave_fullscreen(app));
    chq_host_release_mode(&app->desktop_mode);
    free(app->sprite_area);
}

/*******************************************************************
 Function:      main
 Description:   Run a silent 128K game directly in fullscreen mode.
 Parameters:    argc = argument count
                argv = argument vector
 Returns:       process status
 ******************************************************************/
int main(int argc, char **argv)
{
    chq_fullscreen_t app;
    _kernel_oserror *error;
    int status;

    (void) argc;
    (void) argv;
    memset(&app, 0, sizeof(app));
    status = EXIT_FAILURE;
    if (!create_sprite(&app) || !create_spectrum(&app))
    {
        report_error(&memory_error);
        goto cleanup;
    }
    error = enter_fullscreen(&app);
    if (report_error(error))
        goto cleanup;
    error = draw_fullscreen(&app);
    if (report_error(error))
        goto cleanup;
    app.game = chq_create(app.zx);
    if (app.game == NULL)
    {
        report_error(&memory_error);
        goto cleanup;
    }
    chq_start(app.game, 1);
    if (!app.fatal_error)
        status = EXIT_SUCCESS;

cleanup:
    destroy_app(&app);
    return status;
}
