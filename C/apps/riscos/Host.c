/*******************************************************************
 * File:        Host
 * Purpose:     Pure deferred-action helpers for the native host.
 * Author:      Gerph
 ******************************************************************/

#include "Host.h"

#include <stdlib.h>
#include <string.h>

#include "swis.h"

#define ICONBAR_WINDOW       (-2)
#define ICONBAR_MENU_BUTTON  (2U)
#define ICONBAR_OPEN_BUTTON  (4U)
#define ICONBAR_TOP           (96)
#define SPRITE_WIDE_ENTRIES   (1 << 5)

const uint32_t chq_host_spectrum_palette[16] =
{
    0x00000000U, 0xCD000000U, 0x0000CD00U, 0xCD00CD00U,
    0x00CD0000U, 0xCDCD0000U, 0x00CDCD00U, 0xCDCDCD00U,
    0x00000000U, 0xFF000000U, 0x0000FF00U, 0xFF00FF00U,
    0x00FF0000U, 0xFFFF0000U, 0x00FFFF00U, 0xFFFFFF00U
};

static _kernel_oserror mode_error =
{
    0x80802, "ChaseHQ: no suitable fullscreen mode"
};

/*******************************************************************
 Function:      chq_host_defer
 Description:   Add an action to a pending deferred-action word.
 Parameters:    pending = current pending actions
                action = action to add
 Returns:       combined pending actions
 ******************************************************************/
unsigned int chq_host_defer(unsigned int pending,
                            chq_host_action_t action)
{
    return pending | (unsigned int) action;
}

/*******************************************************************
 Function:      chq_host_iconbar_action
 Description:   Decode an iconbar button event for the application icon.
 Parameters:    window = source window handle
                icon = source icon handle
                app_icon = ChaseHQ iconbar icon handle
                buttons = Wimp pointer-block button value
 Returns:       decoded iconbar action
 ******************************************************************/
chq_iconbar_action_t chq_host_iconbar_action(int window, int icon,
                                              int app_icon,
                                              unsigned int buttons)
{
    if (window != ICONBAR_WINDOW || icon != app_icon)
        return CHQ_ICONBAR_NONE;
    if (buttons == ICONBAR_MENU_BUTTON)
        return CHQ_ICONBAR_MENU;
    if (buttons == ICONBAR_OPEN_BUTTON)
        return CHQ_ICONBAR_OPEN;
    return CHQ_ICONBAR_NONE;
}

/*******************************************************************
 Function:      chq_host_iconbar_menu_y
 Description:   Calculate the top of an iconbar menu above the iconbar.
 Parameters:    item_height = menu item height in OS units
                item_count = number of top-level menu entries
 Returns:       menu top coordinate in OS units
 ******************************************************************/
int chq_host_iconbar_menu_y(int item_height, int item_count)
{
    return ICONBAR_TOP + item_height * item_count;
}

/*******************************************************************
 Function:      chq_host_scale_factors
 Description:   Calculate SpriteExtend whole-pixel scale factors.
 Parameters:    scale = required integer pixel scale
                factors = returned SpriteExtend factors
 Returns:       none
 ******************************************************************/
void chq_host_scale_factors(int scale,
                            chq_host_scale_factors_t *factors)
{
    factors->xmag = scale;
    factors->ymag = scale;
    factors->xdiv = 1;
    factors->ydiv = 1;
}

/*******************************************************************
 Function:      chq_host_copy_frame
 Description:   Copy a converted frame into its sprite image storage.
 Parameters:    destination = first byte of sprite image storage
                source = first byte of converted frame
                height = number of rows
                stride = bytes per row
 Returns:       none
 ******************************************************************/
void chq_host_copy_frame(unsigned char *destination,
                         const unsigned char *source,
                         int height, int stride)
{
    memcpy(destination, source, height * stride);
}

/*******************************************************************
 Function:      chq_host_fullscreen_depth
 Description:   Return a fullscreen depth in preference order.
 Parameters:    attempt = zero-based depth attempt
 Returns:       mode-selector depth or -1 after the final attempt
 ******************************************************************/
int chq_host_fullscreen_depth(int attempt)
{
    static const int depths[] = { 2, 3, 5 };

    if (attempt < 0 ||
        attempt >= (int) (sizeof(depths) / sizeof(depths[0])))
        return -1;
    return depths[attempt];
}

/*******************************************************************
 Function:      chq_host_sprite_action
 Description:   Select SpriteExtend translation-table plot flags.
 Parameters:    log2bpp = destination Log2BPP mode variable
 Returns:       plot action flags for the translation-table width
 ******************************************************************/
int chq_host_sprite_action(int log2bpp)
{
    return log2bpp > 3 ? SPRITE_WIDE_ENTRIES : 0;
}

/*******************************************************************
 Function:      chq_host_monotonic_time
 Description:   Read the wrapping centisecond monotonic clock.
 Parameters:    none
 Returns:       current OS_ReadMonotonicTime value
 ******************************************************************/
uint32_t chq_host_monotonic_time(void)
{
    uint32_t now;

    now = 0;
    _swix(OS_ReadMonotonicTime, _OUT(0), &now);
    return now;
}

/*******************************************************************
 Function:      chq_host_time_is_before
 Description:   Compare two wrapping monotonic clock values.
 Parameters:    a = candidate earlier value
                b = candidate later value
 Returns:       non-zero if a is before b
 ******************************************************************/
int chq_host_time_is_before(uint32_t a, uint32_t b)
{
    return (int32_t) (a - b) < 0;
}

/*******************************************************************
 Function:      chq_host_advance_clock
 Description:   Accumulate T-states into a capped absolute deadline.
 Parameters:    clock = clock state to update
                duration = elapsed Z80 T-states
                clock_rate = emulated clock rate in T-states per second
                now = current wrapping centisecond time
 Returns:       number of whole centiseconds accumulated
 ******************************************************************/
uint32_t chq_host_advance_clock(chq_host_clock_t *clock,
                                uint32_t duration, uint32_t clock_rate,
                                uint32_t now)
{
#ifdef __riscos64
    uint64_t numerator;
#else
    uint32_t numerator;
#endif
    uint32_t ticks;
    uint32_t lag;

    numerator = duration * CHQ_CLOCK_TICKS_SECOND + clock->remainder;
    ticks = (uint32_t) (numerator / clock_rate);
    clock->remainder = (uint32_t) (numerator % clock_rate);
    if (ticks == 0)
        return 0;
    if (!clock->valid)
    {
        clock->deadline = now + ticks;
        clock->valid = 1;
    }
    else
    {
        clock->deadline += ticks;
        lag = ticks * CHQ_MAX_LAG_FRAMES;
        if ((int32_t) (now - clock->deadline) > (int32_t) lag)
            clock->deadline = now - lag;
    }
    return ticks;
}

/*******************************************************************
 Function:      chq_host_mode_is_suitable
 Description:   Check the current mode can display the Spectrum frame.
 Parameters:    none
 Returns:       non-zero for a suitable 4, 8 or 32-bpp mode
 ******************************************************************/
int chq_host_mode_is_suitable(void)
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
           (log2bpp == CHQ_MODE_DEPTH_4BPP ||
            log2bpp == CHQ_MODE_DEPTH_8BPP ||
            log2bpp == CHQ_MODE_DEPTH_32BPP);
}

/*******************************************************************
 Function:      chq_host_select_fullscreen_mode
 Description:   Select the configured or closest suitable screen mode.
 Parameters:    set_mode = host-specific mode-selection callback
 Returns:       error returned by the final mode selection attempt
 ******************************************************************/
_kernel_oserror *chq_host_select_fullscreen_mode(
    chq_host_set_mode_fn set_mode)
{
    static const int fallback[][2] =
    {
        { 1024, 768 }, { 800, 600 }, { 640, 480 }, { 320, 256 }
    };
    const char *configured;
    _kernel_oserror *error;
    int32_t selector[6];
    int candidates[5][2];
    int xlimit;
    int ylimit;
    int candidate;
    int depth;
    int depth_attempt;

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
    error = NULL;
    if (configured != NULL && configured[0] != '\0')
    {
        error = set_mode(configured, 1);
        if (error == NULL && chq_host_mode_is_suitable())
            return NULL;
    }
    selector[0] = 1;
    selector[4] = -1;
    selector[5] = -1;
    for (depth_attempt = 0;
         (depth = chq_host_fullscreen_depth(depth_attempt)) != -1;
         depth_attempt++)
    {
        selector[3] = depth;
        for (candidate = 0; candidate < 5; candidate++)
        {
            if (candidates[candidate][0] < SCREEN_WIDTH ||
                candidates[candidate][1] < SCREEN_HEIGHT)
                continue;
            selector[1] = candidates[candidate][0];
            selector[2] = candidates[candidate][1];
            error = set_mode(selector, 0);
            if (error == NULL && chq_host_mode_is_suitable())
                return NULL;
        }
    }
    return error == NULL ? &mode_error : error;
}

/*******************************************************************
 Function:      chq_host_programme_palette
 Description:   Install the Spectrum palette through documented VDU calls.
 Parameters:    none
 Returns:       error returned by the OS or ColourTrans
 ******************************************************************/
_kernel_oserror *chq_host_programme_palette(void)
{
    unsigned char commands[16 * 6];
    unsigned char *out;
    uint32_t colour;
    int logical;
    _kernel_oserror *error;

    out = commands;
    for (logical = 0; logical < 16; logical++)
    {
        colour = chq_host_spectrum_palette[logical];
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
 Function:      chq_host_build_translation
 Description:   Build the indexed-Spectrum to screen pixel table.
 Parameters:    translation = returned ColourTrans table
                sprite_action = returned SpriteExtend plot action
 Returns:       error returned by the OS or ColourTrans
 ******************************************************************/
_kernel_oserror *chq_host_build_translation(uint32_t *translation,
                                             int *sprite_action)
{
    _kernel_oserror *error;
    int log2bpp;

    error = _swix(OS_ReadModeVariable, _INR(0, 1) | _OUT(2),
                  -1, 9, &log2bpp);
    if (error != NULL)
        return error;
    error = _swix(ColourTrans_SelectTable, _INR(0, 5),
                  CHQ_SPRITE_MODE_INDEXED4,
                  chq_host_spectrum_palette, -1, -1,
                  translation, 0);
    if (error == NULL)
        *sprite_action = chq_host_sprite_action(log2bpp);
    return error;
}

/*******************************************************************
 Function:      chq_host_save_pointer
 Description:   Save the desktop pointer state.
 Parameters:    pointer = pointer state to populate
 Returns:       error returned by OS_Byte
 ******************************************************************/
_kernel_oserror *chq_host_save_pointer(chq_host_pointer_t *pointer)
{
    _kernel_oserror *error;

    error = _swix(OS_Byte, _INR(0, 2) | _OUT(1),
                  106, 127, 0, &pointer->state);
    if (error == NULL)
        pointer->saved = 1;
    return error;
}

/*******************************************************************
 Function:      chq_host_hide_pointer
 Description:   Hide the pointer and remove software cursors.
 Parameters:    pointer = previously saved pointer state
 Returns:       error returned by the OS
 ******************************************************************/
_kernel_oserror *chq_host_hide_pointer(chq_host_pointer_t *pointer)
{
    _kernel_oserror *error;
    int ignored;

    error = _swix(OS_Byte, _INR(0, 2) | _OUT(1),
                  106, pointer->state & 128, 0, &ignored);
    if (error != NULL)
        return error;
    error = _swix(OS_RemoveCursors, 0);
    if (error == NULL)
        pointer->cursors_removed = 1;
    return error;
}

/*******************************************************************
 Function:      chq_host_restore_cursors
 Description:   Restore software cursors if they were removed.
 Parameters:    pointer = pointer state to update
 Returns:       error returned by OS_RestoreCursors
 ******************************************************************/
_kernel_oserror *chq_host_restore_cursors(chq_host_pointer_t *pointer)
{
    _kernel_oserror *error;

    if (!pointer->cursors_removed)
        return NULL;
    error = _swix(OS_RestoreCursors, 0);
    pointer->cursors_removed = 0;
    return error;
}

/*******************************************************************
 Function:      chq_host_restore_pointer
 Description:   Restore the saved desktop pointer state.
 Parameters:    pointer = pointer state to update
 Returns:       error returned by OS_Byte
 ******************************************************************/
_kernel_oserror *chq_host_restore_pointer(chq_host_pointer_t *pointer)
{
    _kernel_oserror *error;
    int ignored;

    if (!pointer->saved)
        return NULL;
    error = _swix(OS_Byte, _INR(0, 2) | _OUT(1),
                  106, pointer->state, 0, &ignored);
    pointer->saved = 0;
    return error;
}

/*******************************************************************
 Function:      chq_host_map_key
 Description:   Map a RISC OS internal key number to emulated input.
 Parameters:    key = RISC OS internal key number
                spectrum = returned Spectrum key
                joystick = returned Kempston control
 Returns:       non-zero if the key has a mapping
 ******************************************************************/
int chq_host_map_key(int key, zxkey_t *spectrum,
                     zxjoystick_t *joystick)
{
    static const struct
    {
        int key;
        zxkey_t spectrum;
        zxjoystick_t joystick;
    }
    mappings[] =
    {
        { 48, zxkey_1, zxjoystick_UNKNOWN },
        { 49, zxkey_2, zxjoystick_UNKNOWN },
        { 17, zxkey_3, zxjoystick_UNKNOWN },
        { 18, zxkey_4, zxjoystick_UNKNOWN },
        { 19, zxkey_5, zxjoystick_UNKNOWN },
        { 52, zxkey_6, zxjoystick_UNKNOWN },
        { 36, zxkey_7, zxjoystick_UNKNOWN },
        { 21, zxkey_8, zxjoystick_UNKNOWN },
        { 38, zxkey_9, zxjoystick_UNKNOWN },
        { 39, zxkey_0, zxjoystick_UNKNOWN },
        { 16, zxkey_Q, zxjoystick_UNKNOWN },
        { 33, zxkey_W, zxjoystick_UNKNOWN },
        { 34, zxkey_E, zxjoystick_UNKNOWN },
        { 51, zxkey_R, zxjoystick_UNKNOWN },
        { 35, zxkey_T, zxjoystick_UNKNOWN },
        { 68, zxkey_Y, zxjoystick_UNKNOWN },
        { 53, zxkey_U, zxjoystick_UNKNOWN },
        { 37, zxkey_I, zxjoystick_UNKNOWN },
        { 54, zxkey_O, zxjoystick_UNKNOWN },
        { 55, zxkey_P, zxjoystick_UNKNOWN },
        { 65, zxkey_A, zxjoystick_UNKNOWN },
        { 81, zxkey_S, zxjoystick_UNKNOWN },
        { 50, zxkey_D, zxjoystick_UNKNOWN },
        { 67, zxkey_F, zxjoystick_UNKNOWN },
        { 83, zxkey_G, zxjoystick_UNKNOWN },
        { 84, zxkey_H, zxjoystick_UNKNOWN },
        { 69, zxkey_J, zxjoystick_UNKNOWN },
        { 70, zxkey_K, zxjoystick_UNKNOWN },
        { 86, zxkey_L, zxjoystick_UNKNOWN },
        { 73, zxkey_ENTER, zxjoystick_UNKNOWN },
        { 0, zxkey_CAPS_SHIFT, zxjoystick_UNKNOWN },
        { 97, zxkey_Z, zxjoystick_UNKNOWN },
        { 66, zxkey_X, zxjoystick_UNKNOWN },
        { 82, zxkey_C, zxjoystick_UNKNOWN },
        { 99, zxkey_V, zxjoystick_UNKNOWN },
        { 100, zxkey_B, zxjoystick_UNKNOWN },
        { 85, zxkey_N, zxjoystick_UNKNOWN },
        { 101, zxkey_M, zxjoystick_UNKNOWN },
        { 2, zxkey_SYMBOL_SHIFT, zxjoystick_UNKNOWN },
        { 98, zxkey_SPACE, zxjoystick_UNKNOWN },
        { 57, zxkey_UNKNOWN, zxjoystick_UP },
        { 41, zxkey_UNKNOWN, zxjoystick_DOWN },
        { 25, zxkey_UNKNOWN, zxjoystick_LEFT },
        { 121, zxkey_UNKNOWN, zxjoystick_RIGHT },
        { 103, zxkey_UNKNOWN, zxjoystick_FIRE }
    };
    unsigned int index;

    *spectrum = zxkey_UNKNOWN;
    *joystick = zxjoystick_UNKNOWN;
    for (index = 0; index < sizeof(mappings) / sizeof(mappings[0]); index++)
    {
        if (mappings[index].key == key)
        {
            *spectrum = mappings[index].spectrum;
            *joystick = mappings[index].joystick;
            return 1;
        }
    }
    return 0;
}
