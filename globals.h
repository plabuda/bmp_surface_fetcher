#pragma once
#include "bike.h"
#include "game_state_machine.h"
#include "renderable.h"
#include "scoreboard.h"
#include "verlet.h"
#include "world.h"
#include <SDL2/SDL.h>

enum { globals_key_count = 5 };

enum {
  globals_key_right,
  globals_key_left,
  globals_key_down,
  globals_key_up,
  globals_key_space
};

typedef struct Globals {
  SDL_Renderer *renderer;
  SDL_Texture *FontTexture;
  SDL_Texture *t;
  char keys[globals_key_count]; // RIGHT - LEFT - DOWN - UP, see
                                // https://wiki.libsdl.org/SDLScancodeLookup
  char previous_keys[globals_key_count];

  Uint32 LEVEL_LOAD;
  Uint32 FONT_LOAD;
  Uint32 AUDIO_LOAD;
  World *w;
  GameStateMachine gsm;
  Scoreboard *s;
} Globals;
