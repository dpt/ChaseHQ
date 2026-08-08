/*******************************************************************
 * File:        Host
 * Purpose:     Pure deferred-action helpers for the native host.
 * Author:      Gerph
 ******************************************************************/

#ifndef CHASEHQ_RISCOS_HOST_H
#define CHASEHQ_RISCOS_HOST_H

typedef enum chq_host_action
{
    CHQ_ACTION_START_GAME = 1 << 0,
    CHQ_ACTION_STOP_GAME = 1 << 1,
    CHQ_ACTION_QUIT_APP = 1 << 2
}
chq_host_action_t;

/*******************************************************************
 Function:      chq_host_defer
 Description:   Add an action to a pending deferred-action word.
 Parameters:    pending = current pending actions
                action = action to add
 Returns:       combined pending actions
 ******************************************************************/
unsigned int chq_host_defer(unsigned int pending,
                            chq_host_action_t action);

#endif
