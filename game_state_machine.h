#pragma once

typedef enum {

  Loading,
  Menu,
  Playing_waiting,
  Playing,
  Playing_finished,
  Scores,
  Credits,

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