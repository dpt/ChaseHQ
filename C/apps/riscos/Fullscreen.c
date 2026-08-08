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

#define SPRITE_AREA_BYTES   (SCREEN_WIDTH * SCREEN_HEIGHT / 2 + 1024)
#define SPRITE_MODE_4BPP    (27)
#define SPRITE_REASON_SCALE (52 + 512)
#define TRANSLATION_WORDS   (256)
#define CLOCK_128K          (3546900U)
#define CLOCK_TICKS_SECOND  (100U)
#define MAX_LAG_FRAMES      (4U)
#define MODE_SELECTOR_HEAD  (5)
#define MODE_DEPTH_4BPP     (2)
#define MODE_DEPTH_8BPP     (3)
#define MODE_DEPTH_32BPP    (5)
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
    uint32_t translation[TRANSLATION_WORDS];
    zxkeyset_t keys;
    zxkempston_t kempston;
    uint32_t deadline;
    uint32_t clock_remainder;
    int deadline_valid;
    int redraw_pending;
    int stop_requested;
    int fatal_error;
    int fullscreen_scale;
    int sprite_plot_action;
    int pointer_state;
    int pointer_saved;
    int cursors_removed;
    int escape_installed;
    void (*desktop_escape_handler)(int);
    uintptr_t desktop_mode;
    int32_t *desktop_mode_selector;
    int desktop_mode_saved;
}
chq_fullscreen_t;

static volatile int fullscreen_escape;

static const uint32_t spectrum_palette[16] =
{
    0x00000000U, 0xCD000000U, 0x0000CD00U, 0xCD00CD00U,
    0x00CD0000U, 0xCDCD0000U, 0x00CDCD00U, 0xCDCDCD00U,
    0x00000000U, 0xFF000000U, 0x0000FF00U, 0xFF00FF00U,
    0x00FF0000U, 0xFFFF0000U, 0x00FFFF00U, 0xFFFFFF00U
};

static _kernel_oserror memory_error =
{
    0x80801, "ChaseHQ: not enough memory"
};

static _kernel_oserror mode_error =
{
    0x80802, "ChaseHQ: no suitable fullscreen mode"
};

/*******************************************************************
 Function:      monotonic_time
 Description:   Read the wrapping centisecond monotonic clock.
 Parameters:    none
 Returns:       current OS_ReadMonotonicTime value
 ******************************************************************/
static uint32_t monotonic_time(void)
{
    uint32_t now;

    now = 0;
    _swix(OS_ReadMonotonicTime, _OUT(0), &now);
    return now;
}

/*******************************************************************
 Function:      time_is_before
 Description:   Compare two wrapping monotonic clock values.
 Parameters:    a = candidate earlier value
                b = candidate later value
 Returns:       non-zero if a is before b
 ******************************************************************/
static int time_is_before(uint32_t a, uint32_t b)
{
    return (int32_t) (a - b) < 0;
}

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
    int key_in;
    int key_out;
    zxkey_t spectrum;
    zxjoystick_t joystick;

    app = opaque;
    zxkeyset_clear(&app->keys);
    app->kempston = 0;
    for (key_in = 0; ; key_in = key_out + 1)
    {
        key_out = 0xFF;
        _swix(OS_Byte, _INR(0, 2) | _OUT(1), 129,
              key_in ^ 0x7F, 0xFF, &key_out);
        if (key_out == 0xFF || key_out == 1)
            break;
        if (chq_host_map_key(key_out, &spectrum, &joystick))
        {
            if (spectrum != zxkey_UNKNOWN)
                zxkeyset_assign(&app->keys, spectrum, 1);
            if (joystick != zxjoystick_UNKNOWN)
                zxkempston_assign(&app->kempston, joystick, 1);
        }
    }
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
 Function:      current_mode_is_suitable
 Description:   Check that the current mode can display the game.
 Parameters:    none
 Returns:       non-zero for a suitable 4, 8 or 32-bpp mode
 ******************************************************************/
static int current_mode_is_suitable(void)
{
    int xlimit;
    int ylimit;
    int log2bpp;

    if (_swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
              -1, 11, &xlimit) != NULL ||
        _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
              -1, 12, &ylimit) != NULL ||
        _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
              -1, 9, &log2bpp) != NULL)
        return 0;
    return xlimit + 1 >= SCREEN_WIDTH &&
           ylimit + 1 >= SCREEN_HEIGHT &&
           (log2bpp == MODE_DEPTH_4BPP ||
            log2bpp == MODE_DEPTH_8BPP ||
            log2bpp == MODE_DEPTH_32BPP);
}

/*******************************************************************
 Function:      save_desktop_mode
 Description:   Copy the complete current numbered mode or selector.
 Parameters:    app = fullscreen state
 Returns:       error returned by OS_ScreenMode
 ******************************************************************/
static _kernel_oserror *save_desktop_mode(chq_fullscreen_t *app)
{
    _kernel_oserror *error;
    const int32_t *selector;
    int words;

    app->desktop_mode = 0;
    error = _swix(OS_ScreenMode, _IN(0) | _OUT(1), 1,
                  &app->desktop_mode);
    if (error != NULL)
        return error;
    app->desktop_mode_saved = 1;
    if (app->desktop_mode < 256)
        return NULL;
    selector = (const int32_t *) app->desktop_mode;
    words = MODE_SELECTOR_HEAD;
    while (words < 256 && selector[words] != -1)
        words += 2;
    if (words >= 256)
        return &mode_error;
    words++;
    app->desktop_mode_selector = malloc(words * sizeof(int32_t));
    if (app->desktop_mode_selector == NULL)
        return &memory_error;
    memcpy(app->desktop_mode_selector, selector,
           words * sizeof(int32_t));
    return NULL;
}

/*******************************************************************
 Function:      select_fullscreen_mode
 Description:   Select the configured or closest suitable screen mode.
 Parameters:    none
 Returns:       error returned by the final mode selection attempt
 ******************************************************************/
static _kernel_oserror *select_fullscreen_mode(void)
{
    static const int fallback[][2] =
    {
        { 1024, 768 }, { 800, 600 }, { 640, 480 }, { 320, 256 }
    };
    static const int depths[] =
    {
        MODE_DEPTH_4BPP, MODE_DEPTH_8BPP, MODE_DEPTH_32BPP
    };
    const char *configured;
    _kernel_oserror *error;
    int32_t selector[6];
    int candidates[5][2];
    int xlimit;
    int ylimit;
    int candidate;
    int depth;

    error = _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
                  -1, 11, &xlimit);
    if (error != NULL)
        return error;
    error = _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
                  -1, 12, &ylimit);
    if (error != NULL)
        return error;
    candidates[0][0] = xlimit + 1;
    candidates[0][1] = ylimit + 1;
    for (candidate = 0; candidate < 4; candidate++)
    {
        candidates[candidate + 1][0] = fallback[candidate][0];
        candidates[candidate + 1][1] = fallback[candidate][1];
    }

    configured = getenv("ChaseHQ$ScreenMode");
    if (configured != NULL && configured[0] != '\0')
    {
        error = _swix(OS_ScreenMode, _INR(0, 1), 15, configured);
        if (error == NULL && current_mode_is_suitable())
            return NULL;
    }
    selector[0] = 1;
    selector[4] = -1;
    selector[5] = -1;
    for (depth = 0;
         depth < (int) (sizeof(depths) / sizeof(depths[0]));
         depth++)
    {
        selector[3] = depths[depth];
        for (candidate = 0;
             candidate < (int) (sizeof(candidates) /
                                sizeof(candidates[0]));
             candidate++)
        {
            if (candidates[candidate][0] < SCREEN_WIDTH ||
                candidates[candidate][1] < SCREEN_HEIGHT)
                continue;
            selector[1] = candidates[candidate][0];
            selector[2] = candidates[candidate][1];
            error = _swix(OS_ScreenMode, _INR(0, 1), 0, selector);
            if (error == NULL && current_mode_is_suitable())
                return NULL;
        }
    }
    return error == NULL ? &mode_error : error;
}

/*******************************************************************
 Function:      programme_fullscreen_palette
 Description:   Install exact Spectrum colours in a 16-colour mode.
 Parameters:    none
 Returns:       error returned by the documented VDU interface
 ******************************************************************/
static _kernel_oserror *programme_fullscreen_palette(void)
{
    unsigned char commands[16 * 6];
    unsigned char *out;
    unsigned int colour;
    int logical;
    _kernel_oserror *error;

    out = commands;
    for (logical = 0; logical < 16; logical++)
    {
        colour = spectrum_palette[logical];
        *out++ = 19;
        *out++ = logical;
        *out++ = 16;
        *out++ = (colour >> 8) & 0xFF;
        *out++ = (colour >> 16) & 0xFF;
        *out++ = (colour >> 24) & 0xFF;
    }
    error = _swix(OS_WriteN, _INR(0, 1), commands, sizeof(commands));
    if (error != NULL)
        return error;
    return _swix(ColourTrans_InvalidateCache, 0);
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

    app->sprite_area = malloc(SPRITE_AREA_BYTES);
    if (app->sprite_area == NULL)
        return 0;
    memset(app->sprite_area, 0, SPRITE_AREA_BYTES);
    image_bytes = SCREEN_WIDTH * SCREEN_HEIGHT / 2;
    sprite_bytes = sizeof(chq_sprite_header_t) + image_bytes;
    app->sprite_area->size = SPRITE_AREA_BYTES;
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
    app->sprite->mode = SPRITE_MODE_4BPP;
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
    _kernel_oserror *error;
    int log2bpp;

    error = _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
                  -1, 9, &log2bpp);
    if (error != NULL)
        return error;
    error = _swix(ColourTrans_SelectTable, _INR(0, 5),
                  SPRITE_MODE_4BPP, spectrum_palette, -1, -1,
                  app->translation, 0);
    if (error == NULL)
        app->sprite_plot_action = chq_host_sprite_action(log2bpp);
    return error;
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
    int xlimit;
    int ylimit;
    int xeig;
    int yeig;
    int screen_width;
    int screen_height;
    int plot_width;
    int plot_height;

    copy_frame_to_sprite(app);
    _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
          -1, 11, &xlimit);
    _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
          -1, 12, &ylimit);
    _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
          -1, 4, &xeig);
    _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
          -1, 5, &yeig);
    screen_width = (xlimit + 1) << xeig;
    screen_height = (ylimit + 1) << yeig;
    plot_width = SCREEN_WIDTH * app->fullscreen_scale * (1 << xeig);
    plot_height = SCREEN_HEIGHT * app->fullscreen_scale * (1 << yeig);
    factors.xmag = app->fullscreen_scale;
    factors.ymag = app->fullscreen_scale;
    factors.xdiv = 1;
    factors.ydiv = 1;
    return _swix(OS_SpriteOp, _INR(0, 7),
                 SPRITE_REASON_SCALE, app->sprite_area, app->sprite,
                 (screen_width - plot_width) / 2,
                 (screen_height - plot_height) / 2,
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
    uint64_t numerator;
    uint32_t ticks;
    uint32_t now;
    uint32_t lag;

    app = opaque;
    numerator = (uint64_t) (unsigned int) duration * CLOCK_TICKS_SECOND +
                app->clock_remainder;
    ticks = (uint32_t) (numerator / CLOCK_128K);
    app->clock_remainder = (uint32_t) (numerator % CLOCK_128K);
    now = monotonic_time();
    if (ticks != 0)
    {
        if (!app->deadline_valid)
        {
            app->deadline = now + ticks;
            app->deadline_valid = 1;
        }
        else
        {
            app->deadline += ticks;
            lag = ticks * MAX_LAG_FRAMES;
            if ((int32_t) (now - app->deadline) > (int32_t) lag)
                app->deadline = now - lag;
        }
        while (!fullscreen_escape &&
               time_is_before(now, app->deadline))
        {
            _swix(OS_Byte, _INR(0, 2), 19, 0, 0);
            now = monotonic_time();
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
    int ignored;

    error = save_desktop_mode(app);
    if (error != NULL)
        return error;
    error = select_fullscreen_mode();
    if (error != NULL)
        return error;
    app->desktop_escape_handler = signal(SIGINT,
                                          fullscreen_escape_handler);
    app->escape_installed = 1;
    _swix(OS_Byte, _INR(0, 2), 229, 0, 0);
    error = _swix(OS_Byte, _INR(0, 2) | _OUT(1),
                  106, 127, 0, &app->pointer_state);
    if (error != NULL)
        return error;
    app->pointer_saved = 1;
    _swix(OS_Byte, _INR(0, 2) | _OUT(1),
          106, app->pointer_state & 128, 0, &ignored);
    error = _swix(OS_RemoveCursors, 0);
    if (error != NULL)
        return error;
    app->cursors_removed = 1;
    error = _swix(OS_WriteC, _IN(0), 12);
    if (error != NULL)
        return error;
    error = _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
                  -1, 9, &log2bpp);
    if (error != NULL)
        return error;
    if (log2bpp == MODE_DEPTH_4BPP)
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
    int ignored;

    error = NULL;
    if (app->escape_installed)
    {
        _swix(OS_Byte, _INR(0, 2), 229, 1, 0);
        signal(SIGINT, app->desktop_escape_handler);
        app->escape_installed = 0;
    }
    if (app->cursors_removed)
    {
        error = _swix(OS_RestoreCursors, 0);
        app->cursors_removed = 0;
    }
    if (app->desktop_mode_saved)
    {
        if (app->desktop_mode_selector != NULL)
            next = _swix(OS_ScreenMode, _INR(0, 1), 0,
                         app->desktop_mode_selector);
        else
            next = _swix(OS_ScreenMode, _INR(0, 1), 0,
                         app->desktop_mode);
        if (error == NULL)
            error = next;
        app->desktop_mode_saved = 0;
    }
    if (app->pointer_saved)
    {
        next = _swix(OS_Byte, _INR(0, 2) | _OUT(1),
                     106, app->pointer_state, 0, &ignored);
        if (error == NULL)
            error = next;
        app->pointer_saved = 0;
    }
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
    free(app->desktop_mode_selector);
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
