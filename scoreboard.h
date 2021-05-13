#pragma once
#include <SDL2/SDL.h>

typedef struct {

  int time_count;

  Uint32 *times;
  Uint32 total;

  Uint32 *best_times;
  Uint32 best_total;

} Scoreboard;

Scoreboard *scoreboard_init(int time_count);
void scoreboard_print_best(Scoreboard *s);
void scoreboard_print(Scoreboard *s);
void scoreboard_submit_checkpoints(Scoreboard *s, Uint32 *checpoints,
                                   int checkpoint_count);
void scoreboard_free(Scoreboard *s);