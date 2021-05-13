#include "game_state_machine.h"

#include "state_loading.h"
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
                                  [Loading] = {state_loading_enter,       //
                                               state_loading_frame,       //
                                               state_loading_exit},       //
                                  [Simulation] = {state_simulation_enter, //
                                                  state_simulation_frame, //
                                                  state_simulation_exit}  //

                              },
                              Last};
}