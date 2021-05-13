#pragma once
#include "flecs.h"
#include "vector2d.h"
#include <SDL2/SDL.h>

typedef struct {
  Vector2D origin;
  double radius;
  Vector2D *points;
  Uint8 point_count, r, g, b;
} Renderable;

typedef struct {
  float radius;
} Renderable_Component;

ECS_COMPONENT_EXTERN(Renderable_Component);
#ifdef COMPONENT_IMPL
ECS_COMPONENT_DECLARE(Renderable_Component);
#endif

Renderable renderable_new(Uint8 r, Uint8 g, Uint8 b, double x, double y);
void renderable_init_window(int width, int height);
void renderable_push(Renderable *r, Vector2D a, Vector2D b);
void renderable_render(Renderable *r, SDL_Renderer *rend);
void renderable_render_offset(Renderable *r, SDL_Renderer *rend, int x, int y);
void renderable_free(Renderable *r);
