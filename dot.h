#pragma once
#include "vector2d.h"
#include <SDL2/SDL.h>

typedef struct {
  Vector2D position, old_position;
  Vector2D gravity;
  double friction;
  double radius, mass;
} Dot;

Dot dot_from_vector(Vector2D v);
Dot dot_default();

void render_dot(const Dot d, SDL_Renderer *r, double x, double y);
