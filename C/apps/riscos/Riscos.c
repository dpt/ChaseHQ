/*******************************************************************
 * File:        Riscos
 * Purpose:     Native RISC OS host for ChaseHQ.
 * Author:      Gerph
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "swis.h"

#include "RISC_OSLib/os.h"
#include "RISC_OSLib/akbd.h"
#include "RISC_OSLib/colourtran.h"
#include "RISC_OSLib/sprite.h"
#include "RISC_OSLib/wimp.h"
#include "RISC_OSLib/wimpt.h"

#include "ChaseHQ/ChaseHQ.h"
#include "ZXSpectrum/Keyboard.h"
#include "ZXSpectrum/Kempston.h"
#include "ZXSpectrum/Spectrum.h"

#include "Host.h"

#define ICONBAR_CREATE_RIGHT (-1)
#define GAME_BORDER_OS      (16)
#define GAME_WINDOW_WIDTH(scale) \
    (CHQ_GAME_WIDTH_OS * (scale) + GAME_BORDER_OS * 2)
#define GAME_WINDOW_HEIGHT(scale) \
    (CHQ_GAME_HEIGHT_OS * (scale) + GAME_BORDER_OS * 2)
#define TEMPLATE_BYTES      (4096)
#define INFO_ITEM           (0)
#define NEW_128K_ITEM       (1)
#define NEW_48K_ITEM        (2)
#define PAUSE_ITEM          (3)
#define RESTART_ITEM        (4)
#define SCALE_1_ITEM        (5)
#define SCALE_4_ITEM        (8)
#define FULLSCREEN_ITEM     (9)
#define QUIT_ITEM           (10)
#define MENU_ITEMS          (11)
#define WIMP_MIN_VERSION    (310)

typedef struct chq_menu
{
    wimp_menuhdr hdr;
    wimp_menuitem item[MENU_ITEMS];
}
chq_menu_t;

typedef struct chq_app
{
    wimp_t task;
    wimp_w game_window;
    wimp_w info_window;
    wimp_i iconbar_icon;
    int running;
    int scale;
    int mode_128k;
    int game_running;
    int start_requested;
    int stop_requested;
    int redraw_pending;
    int fatal_error;
    int paused;
    int restart_requested;
    int fullscreen_requested;
    int fullscreen;
    int fullscreen_scale;
    int escape_installed;
    int have_caret;
    unsigned int pending_actions;
    unsigned int stamps[MAXSTAMPS];
    int nstamps;
    chq_host_clock_t clock;
    zxspectrum_t *zx;
    chqstate_t *game;
    zxkeyset_t keys;
    zxkempston_t kempston;
    chq_host_saved_mode_t desktop_mode;
    wimp_wstate desktop_window_state;
    wimp_caretstr desktop_caret;
    wimp_palettestr desktop_palette;
    chq_host_pointer_t pointer;
    void (*desktop_escape_handler)(int);
    sprite_area *sprite_area;
    sprite_id sprite;
    unsigned int translation[CHQ_TRANSLATION_WORDS];
    int sprite_plot_action;
    chq_menu_t menu;
}
chq_app_t;

static int info_template_buffer[TEMPLATE_BYTES / sizeof(int)];
static char info_workspace[TEMPLATE_BYTES];
static char info_name[] = "proginfo";
static char info_program[] = "Chase H.Q.";
static char info_purpose[] = "ZX Spectrum arcade conversion";
static char info_author[] = "David Thomas / Gerph";
static char info_version[] = "0.10 (RISC OS)";
static char game_title[] = "Chase H.Q.";
static char icon_sprite[] = "!chasehq";
static char screen_sprite[] = "chqscreen";
static const char *scale_labels[] =
{
    "Scale 1x", "Scale 2x", "Scale 3x", "Scale 4x"
};

static os_error *handle_event(chq_app_t *app, wimp_eventstr *event);
static int native_sleep(int duration, void *opaque);
static os_error *enter_fullscreen(chq_app_t *app);
static os_error *leave_fullscreen(chq_app_t *app);
static os_error *draw_fullscreen(chq_app_t *app);
static void copy_frame_to_sprite(chq_app_t *app);
static os_error *force_game_redraw(chq_app_t *app);

static volatile int fullscreen_escape;
static os_error mode_error =
{
    0x80802, "ChaseHQ: fullscreen mode is not suitable"
};
static os_error wimp_version_error =
{
    0x80803, "ChaseHQ: Wimp 3.10 or later is required"
};

/*******************************************************************
 Function:      dispatch_actions
 Description:   Apply actions at a safe engine or main-loop boundary.
 Parameters:    app = application state
 Returns:       none
 ******************************************************************/
static void dispatch_actions(chq_app_t *app)
{
    unsigned int actions;

    actions = app->pending_actions;
    app->pending_actions = 0;
    if (actions & CHQ_ACTION_QUIT_APP)
    {
        app->running = 0;
        app->stop_requested = 1;
    }
    if (actions & CHQ_ACTION_STOP_GAME)
        app->stop_requested = 1;
    if ((actions & CHQ_ACTION_START_GAME) && !app->game_running)
        app->start_requested = 1;
    if ((actions & CHQ_ACTION_PAUSE) && app->game_running)
        app->paused = !app->paused;
    if (actions & CHQ_ACTION_RESTART)
    {
        app->paused = 0;
        if (app->game_running)
        {
            app->restart_requested = 1;
            app->stop_requested = 1;
        }
        else
        {
            app->restart_requested = 0;
            app->start_requested = 1;
        }
    }
    if (actions & CHQ_ACTION_NEW_48K)
    {
        app->mode_128k = 0;
        app->paused = 0;
        app->restart_requested = app->game_running;
        app->stop_requested = app->game_running;
        app->start_requested = !app->game_running;
    }
    if (actions & CHQ_ACTION_NEW_128K)
    {
        app->mode_128k = 1;
        app->paused = 0;
        app->restart_requested = app->game_running;
        app->stop_requested = app->game_running;
        app->start_requested = !app->game_running;
    }
    if ((actions & CHQ_ACTION_FULLSCREEN) && app->game_running)
        app->fullscreen_requested = 1;
}

/*******************************************************************
 Function:      native_draw
 Description:   Defer a game display update to the next safe boundary.
 Parameters:    dirty = dirty Spectrum rectangle
                opaque = application state
 Returns:       none
 ******************************************************************/
static void native_draw(const zxbox_t *dirty, void *opaque)
{
    chq_app_t *app;

    (void) dirty;
    app = opaque;
    app->redraw_pending = 1;
}

/*******************************************************************
 Function:      native_stamp
 Description:   Record the start of a nested timed engine segment.
 Parameters:    opaque = application state
 Returns:       none
 ******************************************************************/
static void native_stamp(void *opaque)
{
    chq_app_t *app;

    app = opaque;
    if (app->nstamps < MAXSTAMPS)
        app->stamps[app->nstamps++] = chq_host_monotonic_time();
}

/*******************************************************************
 Function:      native_key
 Description:   Supply idle keyboard and joystick ports until controls load.
 Parameters:    port = Spectrum input port
                opaque = application state
 Returns:       inactive port value
 ******************************************************************/
static int native_key(uint16_t port, void *opaque)
{
    chq_app_t *app;

    app = opaque;
    zxkeyset_clear(&app->keys);
    app->kempston = 0;
    if (!app->have_caret)
        goto result;
    chq_host_poll_keys(&app->keys, &app->kempston);

result:
    if (port == port_KEMPSTON_JOYSTICK)
        return app->kempston;
    return zxkeyset_for_port(port, &app->keys);
}

/*******************************************************************
 Function:      native_border
 Description:   Ignore the fixed black ChaseHQ border.
 Parameters:    colour = Spectrum border colour
                opaque = application state
 Returns:       none
 ******************************************************************/
static void native_border(int colour, void *opaque)
{
    (void) colour;
    (void) opaque;
}

/*******************************************************************
 Function:      report_error
 Description:   Report a RISC OS error to the command stream.
 Parameters:    error = error returned by a veneer
 Returns:       non-zero when an error was supplied
 ******************************************************************/
static int report_error(os_error *error)
{
    if (error == NULL)
        return 0;

    fprintf(stderr, "ChaseHQ: %s\n", error->errmess);
    return 1;
}

/*******************************************************************
 Function:      update_translation
 Description:   Rebuild the Spectrum-to-current-mode colour table.
 Parameters:    app = application state
 Returns:       error returned by ColourTrans
 ******************************************************************/
static os_error *update_translation(chq_app_t *app)
{
    return chq_host_build_translation(app->translation,
                                      &app->sprite_plot_action);
}

/*******************************************************************
 Function:      fullscreen_escape_handler
 Description:   Remember an Escape request for the next safe callback.
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
 Function:      set_wimp_mode
 Description:   Select a mode while keeping the Wimp informed.
 Parameters:    mode = mode string or selector
                text_mode = non-zero for a mode string
 Returns:       error returned by the Wimp
 ******************************************************************/
static os_error *set_wimp_mode(const void *mode, int text_mode)
{
    if (text_mode)
        return _swix(OS_ScreenMode, _INR(0, 1), 15, mode);
    return wimp_setmode((int) mode);
}

/*******************************************************************
 Function:      select_fullscreen_mode
 Description:   Select the configured or closest useful screen mode.
 Parameters:    none
 Returns:       error from the last mode selection attempt
 ******************************************************************/
static os_error *select_fullscreen_mode(void)
{
    return chq_host_select_fullscreen_mode(set_wimp_mode);
}

/*******************************************************************
 Function:      programme_fullscreen_palette
 Description:   Install the Spectrum colours through documented VDU calls.
 Parameters:    none
 Returns:       error returned by OS_WriteN
 ******************************************************************/
static os_error *programme_fullscreen_palette(void)
{
    return chq_host_programme_palette();
}

/*******************************************************************
 Function:      draw_fullscreen
 Description:   Plot the game centred at the selected whole-pixel scale.
 Parameters:    app = application state
 Returns:       error returned while plotting the sprite
 ******************************************************************/
static os_error *draw_fullscreen(chq_app_t *app)
{
    sprite_factors factors;
    chq_host_scale_factors_t host_factors;
    chq_host_fullscreen_geometry_t geometry;
    os_error *error;

    copy_frame_to_sprite(app);
    error = chq_host_fullscreen_geometry(app->fullscreen_scale,
                                         &geometry);
    if (error != NULL)
        return error;
    chq_host_scale_factors(app->fullscreen_scale, &host_factors);
    factors.xmag = host_factors.xmag;
    factors.ymag = host_factors.ymag;
    factors.xdiv = host_factors.xdiv;
    factors.ydiv = host_factors.ydiv;
    return sprite_put_scaled(app->sprite_area, &app->sprite,
                             app->sprite_plot_action,
                             (geometry.screen_width - geometry.plot_width) / 2,
                             (geometry.screen_height - geometry.plot_height) / 2,
                             &factors,
                             (sprite_pixtrans *) app->translation);
}

/*******************************************************************
 Function:      create_display
 Description:   Create the converted Spectrum frame and sprite workspace.
 Parameters:    app = application state
 Returns:       error returned while creating the display
 ******************************************************************/
static os_error *create_display(chq_app_t *app)
{
    zxconfig_t config;
    sprite_ptr sprite_pointer;
    os_error *error;

    app->sprite_area = malloc(CHQ_SPRITE_AREA_BYTES);
    if (app->sprite_area == NULL)
        return (os_error *) NULL;
    sprite_area_initialise(app->sprite_area, CHQ_SPRITE_AREA_BYTES);
    error = sprite_create_rp(app->sprite_area, screen_sprite,
                             sprite_nopalette, SCREEN_WIDTH, SCREEN_HEIGHT,
                             CHQ_SPRITE_MODE_INDEXED4, &sprite_pointer);
    if (error != NULL)
        return error;
    app->sprite.s.addr = sprite_pointer;
    app->sprite.tag = sprite_id_addr;

    memset(&config, 0, sizeof(config));
    config.width = SCREEN_WIDTH / 8;
    config.height = SCREEN_HEIGHT / 8;
    config.opaque = app;
    config.draw = native_draw;
    config.stamp = native_stamp;
    config.sleep = native_sleep;
    config.key = native_key;
    config.border = native_border;
    config.speaker = NULL;
    config.ay_out = NULL;
    config.pixel_format = ZX_PIXEL_INDEXED4;
    app->zx = zxspectrum_create(&config);
    if (app->zx == NULL)
        return (os_error *) NULL;

    return update_translation(app);
}

/*******************************************************************
 Function:      create_game_window
 Description:   Create the initially closed ChaseHQ display window.
 Parameters:    app = application state
 Returns:       error returned by the Wimp
 ******************************************************************/
static os_error *create_game_window(chq_app_t *app)
{
    wimp_wind window;

    memset(&window, 0, sizeof(window));
    window.box.x0 = 128;
    window.box.y0 = 160;
    window.box.x1 = window.box.x0 + GAME_WINDOW_WIDTH(1);
    window.box.y1 = window.box.y0 + GAME_WINDOW_HEIGHT(1);
    window.flags = wimp_WNEW | wimp_WMOVEABLE | wimp_WBACK |
                   wimp_WTITLE | wimp_WQUIT;
    window.colours[wimp_WCTITLEFORE] = 7;
    window.colours[wimp_WCTITLEBACK] = 2;
    window.colours[wimp_WCWKAREAFORE] = 7;
    window.colours[wimp_WCWKAREABACK] = 255;
    window.colours[wimp_WCSCROLLOUTER] = 3;
    window.colours[wimp_WCSCROLLINNER] = 1;
    window.colours[wimp_WCTITLEHI] = 3;
    window.ex.x0 = 0;
    window.ex.y0 = -GAME_WINDOW_HEIGHT(1);
    window.ex.x1 = GAME_WINDOW_WIDTH(1);
    window.ex.y1 = 0;
    window.titleflags = wimp_ITEXT | wimp_IHCENTRE | wimp_IVCENTRE |
                        wimp_IFILLED | wimp_INDIRECT;
    window.workflags = (wimp_iconflags) (wimp_BCLICKDEBOUNCE << 12);
    window.spritearea = wimp_spritearea;
    window.minsize = (128 << 16) | 256;
    window.title.indirecttext.buffer = game_title;
    window.title.indirecttext.validstring = "";
    window.title.indirecttext.bufflen = sizeof(game_title);

    return wimp_create_wind(&window, &app->game_window);
}

/*******************************************************************
 Function:      copy_frame_to_sprite
 Description:   Copy the converted frame into the RISC OS sprite.
 Parameters:    app = application state
 Returns:       none
 ******************************************************************/
static void copy_frame_to_sprite(chq_app_t *app)
{
    const zx_frame_t *frame;
    sprite_header *header;
    unsigned char *destination;
    const unsigned char *source;

    frame = zxspectrum_claim_screen(app->zx);
    header = (sprite_header *) app->sprite.s.addr;
    destination = (unsigned char *) header + header->image;
    source = frame->pixels;
    chq_host_copy_frame(destination, source, frame->height, frame->stride);
    zxspectrum_release_screen(app->zx);
}

/*******************************************************************
 Function:      remember_error
 Description:   Retain the first error while completing restoration.
 Parameters:    first = address of retained error
                next = latest operation result
 Returns:       none
 ******************************************************************/
static void remember_error(os_error **first, os_error *next)
{
    if (*first == NULL && next != NULL)
        *first = next;
}

/*******************************************************************
 Function:      enter_fullscreen
 Description:   Save desktop state and enter single-tasking fullscreen play.
 Parameters:    app = application state
 Returns:       error returned during entry or restoration
 ******************************************************************/
static os_error *enter_fullscreen(chq_app_t *app)
{
    os_error *error;
    int xlimit;
    int ylimit;
    int scale_x;
    int scale_y;
    int log2bpp;

    if (app->fullscreen)
        return NULL;

    error = chq_host_save_mode(&app->desktop_mode);
    if (error != NULL)
        return error;
    error = wimp_get_wind_state(app->game_window,
                                &app->desktop_window_state);
    if (error != NULL)
        return error;
    error = wimp_get_caret_pos(&app->desktop_caret);
    if (error != NULL)
        return error;
    error = wimp_readpalette(&app->desktop_palette);
    if (error != NULL)
        return error;
    error = chq_host_save_pointer(&app->pointer);
    if (error != NULL)
        return error;

    app->fullscreen = 1;
    error = select_fullscreen_mode();
    if (error != NULL)
        goto failure;
    fullscreen_escape = 0;
    app->desktop_escape_handler = signal(SIGINT,
                                          fullscreen_escape_handler);
    app->escape_installed = 1;
    _swix(OS_Byte, _INR(0, 2), 229, 0, 0);
    error = chq_host_hide_pointer(&app->pointer);
    if (error != NULL)
        goto failure;

    error = _swix(OS_WriteC, _IN(0), 12);
    if (error == NULL)
        error = _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
                      -1, 9, &log2bpp);
    if (error == NULL && log2bpp == CHQ_MODE_DEPTH_4BPP)
        error = programme_fullscreen_palette();
    if (error == NULL)
        error = update_translation(app);
    if (error != NULL)
        goto failure;

    _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2), -1, 11, &xlimit);
    _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2), -1, 12, &ylimit);
    scale_x = (xlimit + 1) / SCREEN_WIDTH;
    scale_y = (ylimit + 1) / SCREEN_HEIGHT;
    app->fullscreen_scale = scale_x < scale_y ? scale_x : scale_y;
    if (app->fullscreen_scale < 1)
    {
        error = &mode_error;
        goto failure;
    }
    error = draw_fullscreen(app);
    if (error != NULL)
        goto failure;
    app->clock.valid = 0;
    return NULL;

failure:
    remember_error(&error, leave_fullscreen(app));
    return error;
}

/*******************************************************************
 Function:      leave_fullscreen
 Description:   Restore desktop mode, palette, pointer, caret and window.
 Parameters:    app = application state
 Returns:       first error encountered while restoring state
 ******************************************************************/
static os_error *leave_fullscreen(chq_app_t *app)
{
    os_error *error;
    os_error *next;

    if (!app->fullscreen)
        return NULL;

    error = NULL;
    app->fullscreen = 0;
    app->fullscreen_requested = 0;
    if (app->escape_installed)
    {
        _swix(OS_Byte, _INR(0, 2), 229, 1, 0);
        signal(SIGINT, app->desktop_escape_handler);
        app->escape_installed = 0;
    }
    remember_error(&error, chq_host_restore_cursors(&app->pointer));

    next = chq_host_restore_mode(&app->desktop_mode, set_wimp_mode);
    remember_error(&error, next);
    remember_error(&error, wimp_setpalette(&app->desktop_palette));
    next = chq_host_restore_pointer(&app->pointer);
    remember_error(&error, next);
    remember_error(&error, wimp_open_wind(&app->desktop_window_state.o));
    remember_error(&error, wimp_set_caret_pos(&app->desktop_caret));
    remember_error(&error, update_translation(app));
    remember_error(&error, force_game_redraw(app));

    chq_host_release_mode(&app->desktop_mode);
    app->clock.valid = 0;
    fullscreen_escape = 0;
    return error;
}

/*******************************************************************
 Function:      fill_black_rectangle
 Description:   Fill an inclusive rectangle using the current black GCOL.
 Parameters:    x0, y0 = bottom-left corner in screen coordinates
                x1, y1 = top-right corner in screen coordinates
 Returns:       error returned by OS_Plot
 ******************************************************************/
static os_error *fill_black_rectangle(int x0, int y0, int x1, int y1)
{
    os_error *error;

    if (x0 > x1 || y0 > y1)
        return NULL;
    error = _swix(OS_Plot, _INR(0, 2),
                  4, x0, y0);
    if (error != NULL)
        return error;
    return _swix(OS_Plot, _INR(0, 2),
                 101, x1, y1);
}

/*******************************************************************
 Function:      fill_game_border
 Description:   Fill only the four margins around the game with black.
 Parameters:    origin_x = screen x coordinate of the work-area origin
                origin_y = screen y coordinate of the work-area origin
                game_width = plotted game width in OS units
                game_height = plotted game height in OS units
                window_width = work-area width in OS units
                window_height = work-area height in OS units
 Returns:       error returned by ColourTrans or OS_Plot
 ******************************************************************/
static os_error *fill_game_border(int origin_x, int origin_y,
                                  int game_width, int game_height,
                                  int window_width, int window_height)
{
    os_error *error;
    int game_x0;
    int game_y0;
    int game_x1;
    int game_y1;

    game_x0 = origin_x + GAME_BORDER_OS;
    game_y0 = origin_y - GAME_BORDER_OS - game_height;
    game_x1 = game_x0 + game_width - 1;
    game_y1 = game_y0 + game_height - 1;

    error = _swix(ColourTrans_SetGCOL, _IN(0) | _INR(3, 4),
                  0, 0, 0);
    if (error != NULL)
        return error;

    error = fill_black_rectangle(origin_x, game_y1 + 1,
                                 origin_x + window_width - 1,
                                 origin_y - 1);
    if (error != NULL)
        return error;
    error = fill_black_rectangle(origin_x, origin_y - window_height,
                                 origin_x + window_width - 1,
                                 game_y0 - 1);
    if (error != NULL)
        return error;
    error = fill_black_rectangle(origin_x, game_y0,
                                 game_x0 - 1, game_y1);
    if (error != NULL)
        return error;
    return fill_black_rectangle(game_x1 + 1, game_y0,
                                origin_x + window_width - 1, game_y1);
}

/*******************************************************************
 Function:      redraw_game
 Description:   Render the centred integer-scaled Spectrum sprite.
 Parameters:    app = application state
                event = redraw event block
 Returns:       error returned during the redraw protocol
 ******************************************************************/
static os_error *redraw_game(chq_app_t *app, wimp_eventstr *event)
{
    wimp_redrawstr redraw;
    sprite_factors factors;
    chq_host_scale_factors_t host_factors;
    os_error *error;
    BOOL more;
    int origin_x;
    int origin_y;
    int game_width;
    int game_height;
    int window_width;
    int window_height;
    int plot_x;
    int plot_y;

    memcpy(&redraw, &event->data, sizeof(redraw));
    error = wimp_redraw_wind(&redraw, &more);
    if (error != NULL)
        return error;

    copy_frame_to_sprite(app);
    chq_host_scale_factors(app->scale, &host_factors);
    factors.xmag = host_factors.xmag;
    factors.ymag = host_factors.ymag;
    factors.xdiv = host_factors.xdiv;
    factors.ydiv = host_factors.ydiv;

    while (more)
    {
        origin_x = redraw.box.x0 - redraw.scx;
        origin_y = redraw.box.y1 - redraw.scy;
        game_width = CHQ_GAME_WIDTH_OS * app->scale;
        game_height = CHQ_GAME_HEIGHT_OS * app->scale;
        window_width = GAME_WINDOW_WIDTH(app->scale);
        window_height = GAME_WINDOW_HEIGHT(app->scale);
        plot_x = origin_x + GAME_BORDER_OS;
        plot_y = origin_y - GAME_BORDER_OS - game_height;

        error = fill_game_border(origin_x, origin_y,
                                 game_width, game_height,
                                 window_width, window_height);
        if (error != NULL)
            return error;

        error = sprite_put_scaled(app->sprite_area, &app->sprite,
                                  app->sprite_plot_action,
                                  plot_x, plot_y, &factors,
                                  (sprite_pixtrans *) app->translation);
        if (error != NULL)
            return error;
        error = wimp_get_rectangle(&redraw, &more);
        if (error != NULL)
            return error;
    }
    return NULL;
}

/*******************************************************************
 Function:      force_game_redraw
 Description:   Invalidate the complete game work area.
 Parameters:    app = application state
 Returns:       error returned by the Wimp
 ******************************************************************/
static os_error *force_game_redraw(chq_app_t *app)
{
    wimp_redrawstr redraw;

    memset(&redraw, 0, sizeof(redraw));
    redraw.w = app->game_window;
    redraw.box.x0 = 0;
    redraw.box.y0 = -GAME_WINDOW_HEIGHT(4);
    redraw.box.x1 = GAME_WINDOW_WIDTH(4);
    redraw.box.y1 = 0;
    return wimp_force_redraw(&redraw);
}

/*******************************************************************
 Function:      service_fullscreen
 Description:   Enter or leave fullscreen at a safe callback boundary.
 Parameters:    app = application state
 Returns:       error returned by the display transition
 ******************************************************************/
static os_error *service_fullscreen(chq_app_t *app)
{
    os_error *error;

    if (app->fullscreen && fullscreen_escape)
        return leave_fullscreen(app);
    if (!app->fullscreen && app->fullscreen_requested)
    {
        app->fullscreen_requested = 0;
        error = enter_fullscreen(app);
        return error;
    }
    return NULL;
}

/*******************************************************************
 Function:      native_sleep
 Description:   Pace the engine and service Wimp events cooperatively.
 Parameters:    duration = nominal duration in Z80 T-states
                opaque = application state
 Returns:       non-zero when the current game must stop
 ******************************************************************/
static int native_sleep(int duration, void *opaque)
{
    chq_app_t *app;
    wimp_eventstr event;
    os_error *error;
    unsigned int clock_rate;
    unsigned int ticks;
    unsigned int now;
    int paused_waited;

    app = opaque;
    paused_waited = 0;
    if (app->nstamps > 0)
        app->nstamps--;

    clock_rate = app->mode_128k ? CPU_CLOCK_128K : CPU_CLOCK_48K;
    now = chq_host_monotonic_time();
    ticks = chq_host_advance_clock(&app->clock,
                                   (unsigned int) duration,
                                   clock_rate, now);

    if (ticks != 0)
    {
        while (app->running && !app->stop_requested &&
               chq_host_time_is_before(now, app->clock.deadline))
        {
            error = NULL;
            if (app->fullscreen)
            {
                _swix(OS_Byte, _INR(0, 2), 19, 0, 0);
            }
            else
            {
                error = wimp_pollidle(0, &event, app->clock.deadline);
                if (error == NULL)
                    error = handle_event(app, &event);
                if (error == NULL)
                    dispatch_actions(app);
            }
            if (error == NULL)
                error = service_fullscreen(app);
            if (error != NULL)
            {
                report_error(error);
                app->fatal_error = 1;
                app->stop_requested = 1;
                break;
            }
            now = chq_host_monotonic_time();
        }
    }

    error = service_fullscreen(app);
    if (error != NULL)
    {
        report_error(error);
        app->fatal_error = 1;
        app->stop_requested = 1;
    }

    if (app->redraw_pending)
    {
        if (app->fullscreen)
            error = draw_fullscreen(app);
        else
            error = force_game_redraw(app);
        if (error != NULL)
        {
            report_error(error);
            app->fatal_error = 1;
            app->stop_requested = 1;
        }
        app->redraw_pending = 0;
    }

    while (app->paused && app->running && !app->stop_requested)
    {
        paused_waited = 1;
        now = chq_host_monotonic_time();
        if (app->fullscreen)
        {
            _swix(OS_Byte, _INR(0, 2), 19, 0, 0);
            error = service_fullscreen(app);
        }
        else
        {
            error = wimp_pollidle(0, &event, now + 50);
            if (error == NULL)
                error = handle_event(app, &event);
            if (error == NULL)
            {
                dispatch_actions(app);
                error = service_fullscreen(app);
            }
        }
        if (error != NULL)
        {
            report_error(error);
            app->fatal_error = 1;
            app->stop_requested = 1;
            break;
        }
        if (app->redraw_pending)
        {
            if (app->fullscreen)
                error = draw_fullscreen(app);
            else
                error = force_game_redraw(app);
            app->redraw_pending = 0;
            if (error != NULL)
            {
                report_error(error);
                app->fatal_error = 1;
                app->stop_requested = 1;
            }
        }
    }
    if (paused_waited && !app->paused)
        app->clock.valid = 0;

    if (!app->running || app->stop_requested)
    {
        chq_stop(app->game);
        return 1;
    }
    return 0;
}

/*******************************************************************
 Function:      create_info_window
 Description:   Load and create the standard program information window.
 Parameters:    app = application state
 Returns:       error returned by the Wimp
 ******************************************************************/
static os_error *create_info_window(chq_app_t *app)
{
    wimp_template load;
    wimp_icon *icons;
    os_error *error;

    error = wimp_open_template("<ChaseHQ$Dir>.Templates");
    if (error != NULL)
        return error;

    memset(&load, 0, sizeof(load));
    load.buf = (wimp_wind *) info_template_buffer;
    load.work_free = info_workspace;
    load.work_end = info_workspace + sizeof(info_workspace);
    load.name = info_name;
    error = wimp_load_template(&load);
    wimp_close_template();
    if (error != NULL)
        return error;

    icons = (wimp_icon *) (load.buf + 1);
    icons[1].data.indirecttext.buffer = info_program;
    icons[1].data.indirecttext.bufflen = sizeof(info_program);
    icons[2].data.indirecttext.buffer = info_purpose;
    icons[2].data.indirecttext.bufflen = sizeof(info_purpose);
    icons[3].data.indirecttext.buffer = info_author;
    icons[3].data.indirecttext.bufflen = sizeof(info_author);
    icons[4].data.indirecttext.buffer = info_version;
    icons[4].data.indirecttext.bufflen = sizeof(info_version);

    return wimp_create_wind(load.buf, &app->info_window);
}

/*******************************************************************
 Function:      create_iconbar_icon
 Description:   Create a conventional right-side application icon.
 Parameters:    app = application state
 Returns:       error returned by the Wimp
 ******************************************************************/
static os_error *create_iconbar_icon(chq_app_t *app)
{
    wimp_icreate create;

    memset(&create, 0, sizeof(create));
    create.w = ICONBAR_CREATE_RIGHT;
    create.i.box.x0 = 0;
    create.i.box.y0 = 0;
    create.i.box.x1 = 68;
    create.i.box.y1 = 68;
    create.i.flags = wimp_ISPRITE | wimp_IHCENTRE | wimp_IVCENTRE |
                     wimp_INDIRECT |
                     (wimp_iconflags) (wimp_BCLICKDEBOUNCE << 12);
    create.i.data.indirectsprite.name = icon_sprite;
    create.i.data.indirectsprite.spritearea = wimp_spritearea;
    create.i.data.indirectsprite.nameisname = TRUE;

    return wimp_create_icon(&create, &app->iconbar_icon);
}

/*******************************************************************
 Function:      create_menu
 Description:   Construct the persistent iconbar menu.
 Parameters:    app = application state
 Returns:       none
 ******************************************************************/
static void create_menu(chq_app_t *app)
{
    wimp_iconflags flags;
    int item;

    memset(&app->menu, 0, sizeof(app->menu));
    strcpy(app->menu.hdr.title, "Chase H.Q.");
    app->menu.hdr.tit_fcol = 7;
    app->menu.hdr.tit_bcol = 2;
    app->menu.hdr.work_fcol = 7;
    app->menu.hdr.work_bcol = 0;
    app->menu.hdr.width = 192;
    app->menu.hdr.height = 44;
    flags = wimp_ITEXT | wimp_IFILLED | (wimp_iconflags) (7 << 24);

    app->menu.item[INFO_ITEM].submenu =
        (wimp_menuptr) app->info_window;
    app->menu.item[INFO_ITEM].iconflags = flags;
    strcpy(app->menu.item[INFO_ITEM].data.text, "Info");

    app->menu.item[NEW_128K_ITEM].submenu = (wimp_menuptr) -1;
    app->menu.item[NEW_128K_ITEM].iconflags = flags;
    strcpy(app->menu.item[NEW_128K_ITEM].data.text, "New 128K");
    app->menu.item[NEW_48K_ITEM].submenu = (wimp_menuptr) -1;
    app->menu.item[NEW_48K_ITEM].iconflags = flags;
    strcpy(app->menu.item[NEW_48K_ITEM].data.text, "New 48K");
    app->menu.item[PAUSE_ITEM].submenu = (wimp_menuptr) -1;
    app->menu.item[PAUSE_ITEM].iconflags = flags;
    strcpy(app->menu.item[PAUSE_ITEM].data.text, "Pause");
    app->menu.item[RESTART_ITEM].submenu = (wimp_menuptr) -1;
    app->menu.item[RESTART_ITEM].iconflags = flags;
    strcpy(app->menu.item[RESTART_ITEM].data.text, "Restart");

    for (item = SCALE_1_ITEM; item <= SCALE_4_ITEM; item++)
    {
        app->menu.item[item].submenu = (wimp_menuptr) -1;
        app->menu.item[item].iconflags = flags;
        strcpy(app->menu.item[item].data.text,
               scale_labels[item - SCALE_1_ITEM]);
    }

    app->menu.item[FULLSCREEN_ITEM].submenu = (wimp_menuptr) -1;
    app->menu.item[FULLSCREEN_ITEM].iconflags = flags;
    strcpy(app->menu.item[FULLSCREEN_ITEM].data.text, "Fullscreen");

    app->menu.item[QUIT_ITEM].flags = wimp_MLAST;
    app->menu.item[QUIT_ITEM].submenu = (wimp_menuptr) -1;
    app->menu.item[QUIT_ITEM].iconflags = flags;
    strcpy(app->menu.item[QUIT_ITEM].data.text, "Quit");
}

/*******************************************************************
 Function:      refresh_scale_menu
 Description:   Tick the current scale and shade sizes that cannot fit.
 Parameters:    app = application state
 Returns:       none
 ******************************************************************/
static void refresh_scale_menu(chq_app_t *app)
{
    int xlimit;
    int ylimit;
    int xeig;
    int yeig;
    int screen_width;
    int screen_height;
    int item;
    int scale;

    _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2), -1, 11, &xlimit);
    _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2), -1, 12, &ylimit);
    _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2), -1, 4, &xeig);
    _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2), -1, 5, &yeig);
    screen_width = (xlimit + 1) << xeig;
    screen_height = (ylimit + 1) << yeig;
    app->menu.item[NEW_128K_ITEM].flags = app->mode_128k ? wimp_MTICK : 0;
    app->menu.item[NEW_48K_ITEM].flags = app->mode_128k ? 0 : wimp_MTICK;
    app->menu.item[PAUSE_ITEM].flags = app->paused ? wimp_MTICK : 0;

    for (item = SCALE_1_ITEM; item <= SCALE_4_ITEM; item++)
    {
        scale = item - SCALE_1_ITEM + 1;
        app->menu.item[item].flags = scale == app->scale ? wimp_MTICK : 0;
        if (GAME_WINDOW_WIDTH(scale) > screen_width ||
            GAME_WINDOW_HEIGHT(scale) > screen_height)
            app->menu.item[item].iconflags |= wimp_INOSELECT;
        else
            app->menu.item[item].iconflags &= ~wimp_INOSELECT;
    }
}

/*******************************************************************
 Function:      set_scale
 Description:   Resize and redraw the game at an integer scale.
 Parameters:    app = application state
                scale = scale from one through four
 Returns:       error returned by the Wimp
 ******************************************************************/
static os_error *set_scale(chq_app_t *app, int scale)
{
    wimp_wstate state;
    wimp_redrawstr extent;
    os_error *error;

    app->scale = scale;
    error = wimp_get_wind_state(app->game_window, &state);
    if (error != NULL)
        return error;
    memset(&extent, 0, sizeof(extent));
    extent.w = app->game_window;
    extent.box.x0 = 0;
    extent.box.y0 = -GAME_WINDOW_HEIGHT(scale);
    extent.box.x1 = GAME_WINDOW_WIDTH(scale);
    extent.box.y1 = 0;
    error = wimp_set_extent(&extent);
    if (error != NULL)
        return error;
    state.o.box.x1 = state.o.box.x0 + GAME_WINDOW_WIDTH(scale);
    state.o.box.y0 = state.o.box.y1 - GAME_WINDOW_HEIGHT(scale);
    state.o.x = 0;
    state.o.y = 0;
    error = wimp_open_wind(&state.o);
    if (error != NULL)
        return error;
    return force_game_redraw(app);
}

/*******************************************************************
 Function:      claim_game_caret
 Description:   Give gameplay focus to the display without showing a caret.
 Parameters:    app = application state
 Returns:       error returned by the Wimp
 ******************************************************************/
static os_error *claim_game_caret(chq_app_t *app)
{
    wimp_caretstr caret;
    os_error *error;

    memset(&caret, 0, sizeof(caret));
    caret.w = app->game_window;
    caret.i = -1;
    caret.height = 1 << 25;
    error = wimp_set_caret_pos(&caret);
    if (error == NULL)
        app->have_caret = 1;
    return error;
}

/*******************************************************************
 Function:      open_game_window
 Description:   Open the display window at its last requested position.
 Parameters:    app = application state
 Returns:       error returned by the Wimp
 ******************************************************************/
static os_error *open_game_window(chq_app_t *app)
{
    wimp_wstate state;
    os_error *error;

    error = wimp_get_wind_state(app->game_window, &state);
    if (error != NULL)
        return error;
    state.o.behind = -1;
    error = wimp_open_wind(&state.o);
    if (error != NULL)
        return error;
    return claim_game_caret(app);
}

/*******************************************************************
 Function:      run_requested_game
 Description:   Run one blocking engine lifecycle on the Wimp thread.
 Parameters:    app = application state
 Returns:       none
 ******************************************************************/
static void run_requested_game(chq_app_t *app)
{
    os_error *error;

    if (!app->start_requested || app->game_running)
        return;

    app->start_requested = 0;
    app->stop_requested = 0;
    memset(&app->clock, 0, sizeof(app->clock));
    app->nstamps = 0;
    app->game = chq_create(app->zx);
    if (app->game == NULL)
    {
        fprintf(stderr, "ChaseHQ: not enough memory to start the game\n");
        app->fatal_error = 1;
        return;
    }

    app->game_running = 1;
    chq_start(app->game, app->mode_128k);
    app->game_running = 0;
    error = leave_fullscreen(app);
    if (error != NULL)
    {
        report_error(error);
        app->fatal_error = 1;
    }
    chq_destroy(app->game);
    app->game = NULL;
    app->stop_requested = 0;
    if (app->restart_requested && app->running)
    {
        app->restart_requested = 0;
        app->start_requested = 1;
    }
}

/*******************************************************************
 Function:      handle_event
 Description:   Dispatch one desktop event.
 Parameters:    app = application state
                event = event returned by Wimp_Poll
 Returns:       error returned while handling the event
 ******************************************************************/
static os_error *handle_event(chq_app_t *app, wimp_eventstr *event)
{
    wimp_mousestr *mouse;
    chq_iconbar_action_t iconbar_action;

    switch (event->e)
    {
    case wimp_EREDRAW:
        if (event->data.o.w == app->game_window)
            return redraw_game(app, event);
        break;

    case wimp_EOPEN:
        return wimp_open_wind(&event->data.o);

    case wimp_ECLOSE:
        if (event->data.o.w == app->game_window)
        {
            app->have_caret = 0;
            app->paused = 0;
            if (app->game_running)
                app->pending_actions = chq_host_defer(
                    app->pending_actions, CHQ_ACTION_STOP_GAME);
        }
        return wimp_close_wind(event->data.o.w);

    case wimp_EBUT:
        mouse = &event->data.but.m;
        if (mouse->w == app->game_window)
            return claim_game_caret(app);
        iconbar_action = chq_host_iconbar_action(mouse->w, mouse->i,
                                                 app->iconbar_icon,
                                                 mouse->bbits);
        if (iconbar_action == CHQ_ICONBAR_MENU)
        {
            refresh_scale_menu(app);
            return wimp_create_menu((wimp_menustr *) &app->menu,
                                    mouse->x - 64,
                                    chq_host_iconbar_menu_y(
                                        app->menu.hdr.height, MENU_ITEMS));
        }
        if (iconbar_action == CHQ_ICONBAR_OPEN)
        {
            os_error *error;

            error = open_game_window(app);
            if (error == NULL && !app->game_running)
                app->pending_actions = chq_host_defer(
                    app->pending_actions, CHQ_ACTION_START_GAME);
            return error;
        }
        break;

    case wimp_EMENU:
        if (event->data.menu[0] == NEW_128K_ITEM)
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_NEW_128K);
        else if (event->data.menu[0] == NEW_48K_ITEM)
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_NEW_48K);
        else if (event->data.menu[0] == PAUSE_ITEM)
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_PAUSE);
        else if (event->data.menu[0] == RESTART_ITEM)
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_RESTART);
        else if (event->data.menu[0] == FULLSCREEN_ITEM)
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_FULLSCREEN);
        else if (event->data.menu[0] == QUIT_ITEM)
        {
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_QUIT_APP);
        }
        else if (event->data.menu[0] >= SCALE_1_ITEM &&
                 event->data.menu[0] <= SCALE_4_ITEM)
            return set_scale(app, event->data.menu[0] - SCALE_1_ITEM + 1);
        break;

    case wimp_EKEY:
        switch (event->data.key.chcode)
        {
        case akbd_Fn + 2:
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_NEW_128K);
            break;
        case akbd_Fn + 3:
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_NEW_48K);
            break;
        case akbd_Fn + 5:
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_PAUSE);
            break;
        case akbd_Fn + 6:
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_RESTART);
            break;
        case akbd_Fn + 7:
            return set_scale(app, 1);
        case akbd_Fn + 8:
            return set_scale(app, 2);
        case akbd_Fn + 9:
            return set_scale(app, 3);
        case akbd_Fn10:
            return set_scale(app, 4);
        case akbd_Fn11:
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_FULLSCREEN);
            break;
        case 17:
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_QUIT_APP);
            break;
        default:
            return wimp_processkey(event->data.key.chcode);
        }
        break;

    case wimp_EGAINCARET:
        app->have_caret = event->data.c.w == app->game_window;
        break;

    case wimp_ELOSECARET:
        if (event->data.c.w == app->game_window)
            app->have_caret = 0;
        break;

    case wimp_ESEND:
    case wimp_ESENDWANTACK:
        if (event->data.msg.hdr.action == wimp_MCLOSEDOWN)
        {
            app->pending_actions = chq_host_defer(app->pending_actions,
                                                  CHQ_ACTION_QUIT_APP);
        }
        else if (event->data.msg.hdr.action == wimp_MMODECHANGE ||
                 event->data.msg.hdr.action == wimp_PALETTECHANGE)
        {
            os_error *error;

            error = update_translation(app);
            if (error != NULL)
                return error;
            return force_game_redraw(app);
        }
        break;

    default:
        break;
    }
    return NULL;
}

/*******************************************************************
 Function:      destroy_app
 Description:   Release resources in reverse ownership order.
 Parameters:    app = application state
 Returns:       none
 ******************************************************************/
static void destroy_app(chq_app_t *app)
{
    report_error(leave_fullscreen(app));
    if (app->game != NULL)
    {
        chq_stop(app->game);
        chq_destroy(app->game);
    }
    if (app->zx != NULL)
        zxspectrum_destroy(app->zx);
    free(app->sprite_area);
    chq_host_release_mode(&app->desktop_mode);
    if (app->game_window >= 0)
        wimp_delete_wind(app->game_window);
    if (app->info_window >= 0)
        wimp_delete_wind(app->info_window);
}

/*******************************************************************
 Function:      main
 Description:   Initialise and run the ChaseHQ desktop task.
 Parameters:    argc = argument count
                argv = argument vector
 Returns:       process status
 ******************************************************************/
int main(int argc, char **argv)
{
    static wimp_msgaction messages[] =
    {
        wimp_MMODECHANGE,
        wimp_PALETTECHANGE,
        wimp_MCLOSEDOWN
    };
    chq_app_t app;
    wimp_eventstr event;
    os_error *error;
    int version;
    int status;

    (void) argc;
    (void) argv;
    memset(&app, 0, sizeof(app));
    app.game_window = -1;
    app.info_window = -1;
    app.iconbar_icon = -1;
    app.running = 1;
    app.scale = 1;
    app.mode_128k = 1;
    status = EXIT_FAILURE;

    wimpt_wimpversion(WIMP_MIN_VERSION);
    wimpt_messages(messages);
    version = wimpt_init("ChaseHQ");
    if (version < WIMP_MIN_VERSION)
    {
        report_error(&wimp_version_error);
        goto cleanup;
    }
    app.task = wimpt_task();
    if (report_error(create_display(&app)) || app.zx == NULL)
        goto cleanup;
    if (report_error(create_game_window(&app)))
        goto cleanup;
    if (report_error(create_info_window(&app)))
        goto cleanup;
    create_menu(&app);
    if (report_error(create_iconbar_icon(&app)))
        goto cleanup;

    while (app.running)
    {
        run_requested_game(&app);
        if (app.fatal_error)
            goto cleanup;
        error = wimpt_poll(0, &event);
        if (report_error(error) || report_error(handle_event(&app, &event)))
            goto cleanup;
        dispatch_actions(&app);
    }
    status = EXIT_SUCCESS;

cleanup:
    destroy_app(&app);
    return status;
}
