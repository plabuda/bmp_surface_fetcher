#pragma once
#include "game_state_machine.h"
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
  Uint32 FONT_LOAD;
  GameStateMachine gsm;
} Globals;
