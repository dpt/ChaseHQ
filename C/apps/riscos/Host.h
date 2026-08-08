/*******************************************************************
 * File:        Host
 * Purpose:     Pure deferred-action helpers for the native host.
 * Author:      Gerph
 ******************************************************************/

#ifndef CHASEHQ_RISCOS_HOST_H
#define CHASEHQ_RISCOS_HOST_H

#include <stdint.h>

#include "kernel.h"

#include "ZXSpectrum/Keyboard.h"
#include "ZXSpectrum/Kempston.h"
#include "ZXSpectrum/Spectrum.h"

#define CHQ_GAME_XEIG              (1)
#define CHQ_GAME_WIDTH_OS          (SCREEN_WIDTH << CHQ_GAME_XEIG)
#define CHQ_GAME_HEIGHT_OS         (SCREEN_HEIGHT << CHQ_GAME_XEIG)
#define CHQ_SPRITE_AREA_BYTES      \
    (SCREEN_WIDTH * SCREEN_HEIGHT / 2 + 1024)
#define CHQ_SPRITE_MODE_INDEXED4   (27)
#define CHQ_TRANSLATION_WORDS      (256)
#define CHQ_CLOCK_48K              (3500000U)
#define CHQ_CLOCK_128K             (3546900U)
#define CHQ_CLOCK_TICKS_SECOND     (100U)
#define CHQ_MAX_LAG_FRAMES         (4U)
#define CHQ_MODE_SELECTOR_HEAD     (5)
#define CHQ_MODE_DEPTH_4BPP        (2)
#define CHQ_MODE_DEPTH_8BPP        (3)
#define CHQ_MODE_DEPTH_32BPP       (5)

typedef struct chq_host_clock
{
    uint32_t deadline;
    uint32_t remainder;
    int valid;
}
chq_host_clock_t;

typedef struct chq_host_pointer
{
    int state;
    int saved;
    int cursors_removed;
}
chq_host_pointer_t;

typedef struct chq_host_saved_mode
{
    uintptr_t mode;
    int32_t *selector;
    int saved;
}
chq_host_saved_mode_t;

typedef _kernel_oserror *(*chq_host_set_mode_fn)(const void *mode,
                                                  int text_mode);

extern const uint32_t chq_host_spectrum_palette[16];

typedef enum chq_host_action
{
    CHQ_ACTION_START_GAME = 1 << 0,
    CHQ_ACTION_STOP_GAME = 1 << 1,
    CHQ_ACTION_QUIT_APP = 1 << 2,
    CHQ_ACTION_PAUSE = 1 << 3,
    CHQ_ACTION_RESTART = 1 << 4,
    CHQ_ACTION_NEW_48K = 1 << 5,
    CHQ_ACTION_NEW_128K = 1 << 6,
    CHQ_ACTION_FULLSCREEN = 1 << 7
}
chq_host_action_t;

typedef enum chq_iconbar_action
{
    CHQ_ICONBAR_NONE,
    CHQ_ICONBAR_OPEN,
    CHQ_ICONBAR_MENU
}
chq_iconbar_action_t;

typedef struct chq_host_scale_factors
{
    int xmag;
    int ymag;
    int xdiv;
    int ydiv;
}
chq_host_scale_factors_t;

typedef struct chq_host_fullscreen_geometry
{
    int screen_width;
    int screen_height;
    int plot_width;
    int plot_height;
}
chq_host_fullscreen_geometry_t;

/*******************************************************************
 Function:      chq_host_defer
 Description:   Add an action to a pending deferred-action word.
 Parameters:    pending = current pending actions
                action = action to add
 Returns:       combined pending actions
 ******************************************************************/
unsigned int chq_host_defer(unsigned int pending,
                            chq_host_action_t action);

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
                                              unsigned int buttons);

/*******************************************************************
 Function:      chq_host_iconbar_menu_y
 Description:   Calculate the top of an iconbar menu above the iconbar.
 Parameters:    item_height = menu item height in OS units
                item_count = number of top-level menu entries
 Returns:       menu top coordinate in OS units
 ******************************************************************/
int chq_host_iconbar_menu_y(int item_height, int item_count);

/*******************************************************************
 Function:      chq_host_scale_factors
 Description:   Calculate SpriteExtend whole-pixel scale factors.
 Parameters:    scale = required integer pixel scale
                factors = returned SpriteExtend factors
 Returns:       none
 ******************************************************************/
void chq_host_scale_factors(int scale,
                            chq_host_scale_factors_t *factors);

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
                         int height, int stride);

/*******************************************************************
 Function:      chq_host_poll_keys
 Description:   Poll held physical keys into Spectrum and Kempston state.
 Parameters:    keys = Spectrum key state to replace
                kempston = Kempston state to replace
 Returns:       none
 ******************************************************************/
void chq_host_poll_keys(zxkeyset_t *keys, zxkempston_t *kempston);

/*******************************************************************
 Function:      chq_host_fullscreen_depth
 Description:   Return a fullscreen depth in preference order.
 Parameters:    attempt = zero-based depth attempt
 Returns:       mode-selector depth or -1 after the final attempt
 ******************************************************************/
int chq_host_fullscreen_depth(int attempt);

/*******************************************************************
 Function:      chq_host_sprite_action
 Description:   Select SpriteExtend translation-table plot flags.
 Parameters:    log2bpp = destination Log2BPP mode variable
 Returns:       plot action flags for the translation-table width
 ******************************************************************/
int chq_host_sprite_action(int log2bpp);

/*******************************************************************
 Function:      chq_host_monotonic_time
 Description:   Read the wrapping centisecond monotonic clock.
 Parameters:    none
 Returns:       current OS_ReadMonotonicTime value
 ******************************************************************/
uint32_t chq_host_monotonic_time(void);

/*******************************************************************
 Function:      chq_host_time_is_before
 Description:   Compare two wrapping monotonic clock values.
 Parameters:    a = candidate earlier value
                b = candidate later value
 Returns:       non-zero if a is before b
 ******************************************************************/
int chq_host_time_is_before(uint32_t a, uint32_t b);

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
                                uint32_t now);

/*******************************************************************
 Function:      chq_host_mode_is_suitable
 Description:   Check the current mode can display the Spectrum frame.
 Parameters:    none
 Returns:       non-zero for a suitable 4, 8 or 32-bpp mode
 ******************************************************************/
int chq_host_mode_is_suitable(void);

/*******************************************************************
 Function:      chq_host_select_fullscreen_mode
 Description:   Select the configured or closest suitable screen mode.
 Parameters:    set_mode = host-specific mode-selection callback
 Returns:       error returned by the final mode selection attempt
 ******************************************************************/
_kernel_oserror *chq_host_select_fullscreen_mode(
    chq_host_set_mode_fn set_mode);

/*******************************************************************
 Function:      chq_host_save_mode
 Description:   Save a stable copy of the current desktop mode.
 Parameters:    saved = saved-mode state to populate
 Returns:       error returned by OS_ScreenMode or allocation
 ******************************************************************/
_kernel_oserror *chq_host_save_mode(chq_host_saved_mode_t *saved);

/*******************************************************************
 Function:      chq_host_restore_mode
 Description:   Restore a previously saved desktop mode.
 Parameters:    saved = saved-mode state
                set_mode = host-specific mode-selection callback
 Returns:       error returned by the mode-selection callback
 ******************************************************************/
_kernel_oserror *chq_host_restore_mode(chq_host_saved_mode_t *saved,
                                        chq_host_set_mode_fn set_mode);

/*******************************************************************
 Function:      chq_host_release_mode
 Description:   Release storage owned by a saved desktop mode.
 Parameters:    saved = saved-mode state to clear
 Returns:       none
 ******************************************************************/
void chq_host_release_mode(chq_host_saved_mode_t *saved);

/*******************************************************************
 Function:      chq_host_programme_palette
 Description:   Install the Spectrum palette through documented VDU calls.
 Parameters:    none
 Returns:       error returned by the OS or ColourTrans
 ******************************************************************/
_kernel_oserror *chq_host_programme_palette(void);

/*******************************************************************
 Function:      chq_host_build_translation
 Description:   Build the indexed-Spectrum to screen pixel table.
 Parameters:    translation = returned ColourTrans table
                sprite_action = returned SpriteExtend plot action
 Returns:       error returned by the OS or ColourTrans
 ******************************************************************/
_kernel_oserror *chq_host_build_translation(uint32_t *translation,
                                             int *sprite_action);

/*******************************************************************
 Function:      chq_host_fullscreen_geometry
 Description:   Calculate centred fullscreen sprite geometry.
 Parameters:    scale = whole-pixel sprite scale
                geometry = returned OS-unit dimensions
 Returns:       error returned by OS_ReadModeVariable
 ******************************************************************/
_kernel_oserror *chq_host_fullscreen_geometry(
    int scale, chq_host_fullscreen_geometry_t *geometry);

/*******************************************************************
 Function:      chq_host_save_pointer
 Description:   Save the desktop pointer state.
 Parameters:    pointer = pointer state to populate
 Returns:       error returned by OS_Byte
 ******************************************************************/
_kernel_oserror *chq_host_save_pointer(chq_host_pointer_t *pointer);

/*******************************************************************
 Function:      chq_host_hide_pointer
 Description:   Hide the pointer and remove software cursors.
 Parameters:    pointer = previously saved pointer state
 Returns:       error returned by the OS
 ******************************************************************/
_kernel_oserror *chq_host_hide_pointer(chq_host_pointer_t *pointer);

/*******************************************************************
 Function:      chq_host_restore_cursors
 Description:   Restore software cursors if they were removed.
 Parameters:    pointer = pointer state to update
 Returns:       error returned by OS_RestoreCursors
 ******************************************************************/
_kernel_oserror *chq_host_restore_cursors(chq_host_pointer_t *pointer);

/*******************************************************************
 Function:      chq_host_restore_pointer
 Description:   Restore the saved desktop pointer state.
 Parameters:    pointer = pointer state to update
 Returns:       error returned by OS_Byte
 ******************************************************************/
_kernel_oserror *chq_host_restore_pointer(chq_host_pointer_t *pointer);

/*******************************************************************
 Function:      chq_host_map_key
 Description:   Map a RISC OS internal key number to emulated input.
 Parameters:    key = RISC OS internal key number
                spectrum = returned Spectrum key
                joystick = returned Kempston control
 Returns:       non-zero if the key has a mapping
 ******************************************************************/
int chq_host_map_key(int key, zxkey_t *spectrum,
                     zxjoystick_t *joystick);

#endif
