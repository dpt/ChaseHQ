/*******************************************************************
 * File:        Riscos
 * Purpose:     Native RISC OS host for ChaseHQ.
 * Author:      Gerph
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RISC_OSLib/os.h"
#include "RISC_OSLib/wimp.h"

#define ICONBAR_WINDOW      (-2)
#define ICONBAR_TOP         (96)
#define GAME_WIDTH_OS       (512)
#define GAME_HEIGHT_OS      (384)
#define TEMPLATE_BYTES      (4096)
#define INFO_ITEM           (0)
#define QUIT_ITEM           (1)

typedef struct chq_menu
{
    wimp_menuhdr hdr;
    wimp_menuitem item[2];
}
chq_menu_t;

typedef struct chq_app
{
    wimp_t task;
    wimp_w game_window;
    wimp_w info_window;
    wimp_i iconbar_icon;
    int running;
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
    window.flags = wimp_WNEW | wimp_WMOVEABLE | wimp_REDRAW_OK |
                   wimp_WTITLE | wimp_WQUIT | wimp_WTOGGLE;
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

    app->menu.item[QUIT_ITEM].flags = wimp_MLAST;
    app->menu.item[QUIT_ITEM].submenu = (wimp_menuptr) -1;
    app->menu.item[QUIT_ITEM].iconflags = flags;
    strcpy(app->menu.item[QUIT_ITEM].data.text, "Quit");
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
    case wimp_EOPEN:
        return wimp_open_wind(&event->data.o);

    case wimp_ECLOSE:
        return wimp_close_wind(event->data.o.w);

    case wimp_EBUT:
        mouse = &event->data.but.m;
        if (mouse->w == ICONBAR_WINDOW && mouse->i == app->iconbar_icon)
        {
            if (event->data.but.b == wimp_BMID)
                return wimp_create_menu((wimp_menustr *) &app->menu,
                                        mouse->x - 64,
                                        ICONBAR_TOP + 2 * app->menu.hdr.height);
            if (event->data.but.b == wimp_BLEFT)
                return open_game_window(app);
        }
        break;

    case wimp_EMENU:
        if (event->data.menu[0] == QUIT_ITEM)
            app->running = 0;
        break;

    case wimp_ESEND:
    case wimp_ESENDWANTACK:
        if (event->data.msg.hdr.action == wimp_MCLOSEDOWN)
            app->running = 0;
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
    version = 310;
    status = EXIT_FAILURE;

    error = wimp_taskinit("ChaseHQ", &version, &app.task,
                          wimp_MMODECHANGE, wimp_PALETTECHANGE,
                          wimp_MCLOSEDOWN);
    if (report_error(error))
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
