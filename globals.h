#pragma once
#include <SDL2/SDL.h>

typedef struct Globals {
  SDL_Renderer *renderer;
  SDL_Texture *FontTexture;
  Uint32 FONT_LOAD;
} Globals;
