#include "game_state_machine.h"

#include "state_credits.h"
#include "state_loading.h"
#include "state_menu.h"
#include "state_playing.h"
#include "state_scores.h"
#include "state_simulation.h"

#include <stdio.h>

void _gsm_call_map(GameStateMachine *g, int i)
{
    void (*function)() = g->states[g->current_state][i];

    if (function != NULL)
    {
        function();
    }
}

void _gsm_call_enter(GameStateMachine *g)
{
    _gsm_call_map(g, 0);
}

void _gsm_call_frame(GameStateMachine *g)
{
    _gsm_call_map(g, 1);
}

void _gsm_call_exit(GameStateMachine *g)
{
    _gsm_call_map(g, 2);
}

void gsm_set_state(GameStateMachine *g, State s)
{
    _gsm_call_exit(g);
    g->current_state = s;
    _gsm_call_enter(g);
}

void gsm_frame(GameStateMachine *g)
{
    _gsm_call_frame(g);
}

GameStateMachine gsm_new()
{
    return (GameStateMachine){{
                                  [Loading] = {state_loading_enter,                   //
                                               state_loading_frame,                   //
                                               state_loading_exit},                   //
                                  [Menu] = {state_menu_enter,                         //
                                            state_menu_frame,                         //
                                            state_menu_exit},                         //
                                  [Playing_waiting] = {state_playing_waiting_enter,   //
                                                       state_playing_waiting_frame,   //
                                                       state_playing_waiting_exit},   //
                                  [Playing] = {state_playing_enter,                   //
                                               state_playing_frame,                   //
                                               state_playing_exit},                   //
                                  [Playing_finished] = {state_playing_finished_enter, //
                                                        state_playing_finished_frame, //
                                                        state_playing_finished_exit}, //
                                  [Scores] = {state_scores_enter,                     //
                                              state_scores_frame,                     //
                                              state_credits_exit},                    //
                                  [Credits] = {state_credits_enter,                   //
                                               state_credits_frame,                   //
                                               state_credits_exit},                   //
                                  [Simulation] = {state_simulation_enter,             //
                                                  state_simulation_frame,             //
                                                  state_simulation_exit}              //

                              },
                              Last};
}