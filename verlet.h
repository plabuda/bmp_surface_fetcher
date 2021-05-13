#pragma once
#include "dot.h"
#include "flecs.h"
#include "position.h"
#include "stick.h"
#include <SDL2/SDL.h>

typedef struct {
  Dot *dots;
  int dot_count;

  Stick *sticks;
  int stick_count;
} Verlet;

Verlet verlet_init(int dots_max, int sticks_max);

void verlet_step(const Verlet *v);

void verlet_update(const Verlet *v);
void verlet_constrain(const Verlet *v);

void verlet_render(const Verlet *v, SDL_Renderer *r);
void verlet_render_offset(const Verlet *v, SDL_Renderer *r, double x, double y);

void verlet_push_dots(Verlet *v, Vector2D *vectors, int vector_count);
void verlet_free(Verlet *v);

ecs_entity_t verlet_add_move_system(ecs_world_t *world, ecs_entity_t phase);

ecs_entity_t verlet_add_force_system(ecs_world_t *world, ecs_entity_t phase);