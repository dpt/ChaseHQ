/*******************************************************************
 * File:        Riscos
 * Purpose:     Native RISC OS host for ChaseHQ.
 * Author:      Gerph
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "swis.h"

#include "RISC_OSLib/os.h"
#include "RISC_OSLib/colourtran.h"
#include "RISC_OSLib/sprite.h"
#include "RISC_OSLib/wimp.h"

#include "ZXSpectrum/Spectrum.h"

#define ICONBAR_WINDOW      (-2)
#define ICONBAR_TOP         (96)
#define GAME_WIDTH_OS       (512)
#define GAME_HEIGHT_OS      (384)
#define TEMPLATE_BYTES      (4096)
#define INFO_ITEM           (0)
#define SCALE_1_ITEM        (1)
#define SCALE_4_ITEM        (4)
#define QUIT_ITEM           (5)
#define SPRITE_AREA_BYTES   (SCREEN_WIDTH * SCREEN_HEIGHT / 2 + 1024)
#define TRANSLATION_BYTES   (1024)
#define SPRITE_MODE_4BPP    (12)

typedef struct chq_menu
{
    wimp_menuhdr hdr;
    wimp_menuitem item[6];
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
    zxspectrum_t *zx;
    sprite_area *sprite_area;
    sprite_id sprite;
    unsigned char translation[TRANSLATION_BYTES];
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
static char icon_sprite[] = "application";
static char screen_sprite[] = "chqscreen";
static const char *scale_labels[] =
{
    "Scale 1x", "Scale 2x", "Scale 3x", "Scale 4x"
};

static const unsigned int spectrum_palette[16] =
{
    0x00000000U, 0xCD000000U, 0x0000CD00U, 0xCD00CD00U,
    0x00CD0000U, 0xCDCD0000U, 0x00CDCD00U, 0xCDCDCD00U,
    0x00000000U, 0xFF000000U, 0x0000FF00U, 0xFF00FF00U,
    0x00FF0000U, 0xFFFF0000U, 0x00FFFF00U, 0xFFFFFF00U
};

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
    return colourtran_select_table(SPRITE_MODE_4BPP,
        (wimp_paletteword *) spectrum_palette, -1,
        (wimp_paletteword *) -1, app->translation);
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

    app->sprite_area = malloc(SPRITE_AREA_BYTES);
    if (app->sprite_area == NULL)
        return (os_error *) NULL;
    sprite_area_initialise(app->sprite_area, SPRITE_AREA_BYTES);
    error = sprite_create_rp(app->sprite_area, screen_sprite,
                             sprite_nopalette, SCREEN_WIDTH, SCREEN_HEIGHT,
                             SPRITE_MODE_4BPP, &sprite_pointer);
    if (error != NULL)
        return error;
    app->sprite.s.addr = sprite_pointer;
    app->sprite.tag = sprite_id_addr;

    memset(&config, 0, sizeof(config));
    config.width = SCREEN_WIDTH / 8;
    config.height = SCREEN_HEIGHT / 8;
    config.opaque = app;
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
    window.box.x1 = window.box.x0 + GAME_WIDTH_OS;
    window.box.y1 = window.box.y0 + GAME_HEIGHT_OS;
    window.flags = wimp_WNEW | wimp_WMOVEABLE | wimp_WTITLE | wimp_WQUIT |
                   wimp_WTOGGLE | wimp_WSIZE;
    window.colours[wimp_WCTITLEFORE] = 7;
    window.colours[wimp_WCTITLEBACK] = 2;
    window.colours[wimp_WCWKAREAFORE] = 7;
    window.colours[wimp_WCWKAREABACK] = 0;
    window.colours[wimp_WCSCROLLOUTER] = 3;
    window.colours[wimp_WCSCROLLINNER] = 1;
    window.colours[wimp_WCTITLEHI] = 3;
    window.ex.x0 = 0;
    window.ex.y0 = -GAME_HEIGHT_OS;
    window.ex.x1 = GAME_WIDTH_OS;
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
 Description:   Copy the top-down frame into the bottom-up RISC OS sprite.
 Parameters:    app = application state
 Returns:       none
 ******************************************************************/
static void copy_frame_to_sprite(chq_app_t *app)
{
    const zx_frame_t *frame;
    sprite_header *header;
    unsigned char *destination;
    const unsigned char *source;
    int y;

    frame = zxspectrum_claim_screen(app->zx);
    header = (sprite_header *) app->sprite.s.addr;
    destination = (unsigned char *) header + header->image;
    source = frame->pixels;
    for (y = 0; y < frame->height; y++)
        memcpy(destination + (frame->height - 1 - y) * frame->stride,
               source + y * frame->stride, frame->stride);
    zxspectrum_release_screen(app->zx);
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
    os_error *error;
    BOOL more;
    int origin_x;
    int origin_y;
    int work_width;
    int work_height;
    int game_width;
    int game_height;
    int plot_x;
    int plot_y;

    memcpy(&redraw, &event->data, sizeof(redraw));
    error = wimp_redraw_wind(&redraw, &more);
    if (error != NULL)
        return error;

    copy_frame_to_sprite(app);
    factors.xmag = app->scale;
    factors.ymag = app->scale;
    factors.xdiv = 1;
    factors.ydiv = 2;

    while (more)
    {
        origin_x = redraw.box.x0 - redraw.scx;
        origin_y = redraw.box.y1 - redraw.scy;
        work_width = redraw.box.x1 - redraw.box.x0;
        work_height = redraw.box.y1 - redraw.box.y0;
        game_width = GAME_WIDTH_OS * app->scale;
        game_height = GAME_HEIGHT_OS * app->scale;
        plot_x = origin_x + (work_width - game_width) / 2;
        plot_y = origin_y - (work_height + game_height) / 2;

        error = sprite_put_scaled(app->sprite_area, &app->sprite, 0,
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
    redraw.box.y0 = -GAME_HEIGHT_OS * 4;
    redraw.box.x1 = GAME_WIDTH_OS * 4;
    redraw.box.y1 = 0;
    return wimp_force_redraw(&redraw);
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
    create.w = ICONBAR_WINDOW;
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
    app->menu.hdr.width = 160;
    app->menu.hdr.height = 44;
    flags = wimp_ITEXT | wimp_IFILLED | (wimp_iconflags) (7 << 24);

    app->menu.item[INFO_ITEM].submenu =
        (wimp_menuptr) app->info_window;
    app->menu.item[INFO_ITEM].iconflags = flags;
    strcpy(app->menu.item[INFO_ITEM].data.text, "Info");

    for (item = SCALE_1_ITEM; item <= SCALE_4_ITEM; item++)
    {
        app->menu.item[item].submenu = (wimp_menuptr) -1;
        app->menu.item[item].iconflags = flags;
        strcpy(app->menu.item[item].data.text,
               scale_labels[item - SCALE_1_ITEM]);
    }

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

    for (item = SCALE_1_ITEM; item <= SCALE_4_ITEM; item++)
    {
        scale = item - SCALE_1_ITEM + 1;
        app->menu.item[item].flags = scale == app->scale ? wimp_MTICK : 0;
        if (GAME_WIDTH_OS * scale > screen_width ||
            GAME_HEIGHT_OS * scale > screen_height)
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
    os_error *error;

    app->scale = scale;
    error = wimp_get_wind_state(app->game_window, &state);
    if (error != NULL)
        return error;
    state.o.box.x1 = state.o.box.x0 + GAME_WIDTH_OS * scale;
    state.o.box.y0 = state.o.box.y1 - GAME_HEIGHT_OS * scale;
    error = wimp_open_wind(&state.o);
    if (error != NULL)
        return error;
    return force_game_redraw(app);
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
    return wimp_open_wind(&state.o);
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

    switch (event->e)
    {
    case wimp_EREDRAW:
        if (event->data.o.w == app->game_window)
            return redraw_game(app, event);
        break;

    case wimp_EOPEN:
        return wimp_open_wind(&event->data.o);

    case wimp_ECLOSE:
        return wimp_close_wind(event->data.o.w);

    case wimp_EBUT:
        mouse = &event->data.but.m;
        if (mouse->w == ICONBAR_WINDOW && mouse->i == app->iconbar_icon)
        {
            if (event->data.but.b == wimp_BMID)
            {
                refresh_scale_menu(app);
                return wimp_create_menu((wimp_menustr *) &app->menu,
                                        mouse->x - 64,
                                        ICONBAR_TOP + 6 * app->menu.hdr.height);
            }
            if (event->data.but.b == wimp_BLEFT)
                return open_game_window(app);
        }
        break;

    case wimp_EMENU:
        if (event->data.menu[0] == QUIT_ITEM)
            app->running = 0;
        else if (event->data.menu[0] >= SCALE_1_ITEM &&
                 event->data.menu[0] <= SCALE_4_ITEM)
            return set_scale(app, event->data.menu[0] - SCALE_1_ITEM + 1);
        break;

    case wimp_ESEND:
    case wimp_ESENDWANTACK:
        if (event->data.msg.hdr.action == wimp_MCLOSEDOWN)
            app->running = 0;
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
    zxspectrum_destroy(app->zx);
    free(app->sprite_area);
    if (app->game_window >= 0)
        wimp_delete_wind(app->game_window);
    if (app->info_window >= 0)
        wimp_delete_wind(app->info_window);
    if (app->task != 0)
        wimp_taskclose(app->task);
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
    version = 310;
    status = EXIT_FAILURE;

    error = wimp_taskinit("ChaseHQ", &version, &app.task,
                          wimp_MMODECHANGE, wimp_PALETTECHANGE,
                          wimp_MCLOSEDOWN);
    if (report_error(error))
        goto cleanup;
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
        error = wimp_poll(0, &event);
        if (report_error(error) || report_error(handle_event(&app, &event)))
            goto cleanup;
    }
    status = EXIT_SUCCESS;

cleanup:
    destroy_app(&app);
    return status;
}
