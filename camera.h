#pragma once
#include "flecs.h"
#include <SDL2/SDL.h>

typedef struct {
  SDL_Texture *background;
} Camera;

ECS_COMPONENT_EXTERN(Camera);

#ifdef COMPONENT_IMPL
ECS_COMPONENT_DECLARE(Camera);
#endif