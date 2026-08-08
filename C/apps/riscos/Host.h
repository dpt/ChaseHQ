/*******************************************************************
 * File:        Host
 * Purpose:     Pure deferred-action helpers for the native host.
 * Author:      Gerph
 ******************************************************************/

#ifndef CHASEHQ_RISCOS_HOST_H
#define CHASEHQ_RISCOS_HOST_H

#include "ZXSpectrum/Keyboard.h"
#include "ZXSpectrum/Kempston.h"

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
