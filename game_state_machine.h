#pragma once

typedef enum {

  Loading,
  Simulation,

  Last // Marker for enum width
} State;

typedef struct {
  void (*states[Last][3])();
  State current_state;

} GameStateMachine;

GameStateMachine gsm_new();

void gsm_set_state(GameStateMachine *g, State s);
void gsm_frame(GameStateMachine *g);