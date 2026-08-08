/*******************************************************************
 * File:        Host
 * Purpose:     Pure deferred-action helpers for the native host.
 * Author:      Gerph
 ******************************************************************/

#include "Host.h"

#include <string.h>

#define ICONBAR_WINDOW       (-2)
#define ICONBAR_MENU_BUTTON  (2U)
#define ICONBAR_OPEN_BUTTON  (4U)
#define ICONBAR_TOP           (96)
#define SPRITE_WIDE_ENTRIES   (1 << 5)

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
    *spectrum = zxkey_UNKNOWN;
    *joystick = zxjoystick_UNKNOWN;
    switch (key)
    {
    case 48: *spectrum = zxkey_1; break;
    case 49: *spectrum = zxkey_2; break;
    case 17: *spectrum = zxkey_3; break;
    case 18: *spectrum = zxkey_4; break;
    case 19: *spectrum = zxkey_5; break;
    case 52: *spectrum = zxkey_6; break;
    case 36: *spectrum = zxkey_7; break;
    case 21: *spectrum = zxkey_8; break;
    case 38: *spectrum = zxkey_9; break;
    case 39: *spectrum = zxkey_0; break;
    case 16: *spectrum = zxkey_Q; break;
    case 33: *spectrum = zxkey_W; break;
    case 34: *spectrum = zxkey_E; break;
    case 51: *spectrum = zxkey_R; break;
    case 35: *spectrum = zxkey_T; break;
    case 68: *spectrum = zxkey_Y; break;
    case 53: *spectrum = zxkey_U; break;
    case 37: *spectrum = zxkey_I; break;
    case 54: *spectrum = zxkey_O; break;
    case 55: *spectrum = zxkey_P; break;
    case 65: *spectrum = zxkey_A; break;
    case 81: *spectrum = zxkey_S; break;
    case 50: *spectrum = zxkey_D; break;
    case 67: *spectrum = zxkey_F; break;
    case 83: *spectrum = zxkey_G; break;
    case 84: *spectrum = zxkey_H; break;
    case 69: *spectrum = zxkey_J; break;
    case 70: *spectrum = zxkey_K; break;
    case 86: *spectrum = zxkey_L; break;
    case 73: *spectrum = zxkey_ENTER; break;
    case 0: *spectrum = zxkey_CAPS_SHIFT; break;
    case 97: *spectrum = zxkey_Z; break;
    case 66: *spectrum = zxkey_X; break;
    case 82: *spectrum = zxkey_C; break;
    case 99: *spectrum = zxkey_V; break;
    case 100: *spectrum = zxkey_B; break;
    case 85: *spectrum = zxkey_N; break;
    case 101: *spectrum = zxkey_M; break;
    case 2: *spectrum = zxkey_SYMBOL_SHIFT; break;
    case 98: *spectrum = zxkey_SPACE; break;
    case 57: *joystick = zxjoystick_UP; break;
    case 41: *joystick = zxjoystick_DOWN; break;
    case 25: *joystick = zxjoystick_LEFT; break;
    case 121: *joystick = zxjoystick_RIGHT; break;
    case 103: *joystick = zxjoystick_FIRE; break;
    default: return 0;
    }
    return 1;
}
