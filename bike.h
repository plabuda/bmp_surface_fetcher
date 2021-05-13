#pragma once
#include "flecs.h"
#include "verlet.h"
#include <SDL2/SDL.h>

typedef struct {
  Verlet *v;
  int front_index, back_index;
  double turn_angle;
  int turn_force, speed_force; // 1 - 0 - -1, control intent

} Bike;

typedef struct {
  Sint8 turn_force, speed_force;
  double turn_angle;
} Bike_Component;

ECS_COMPONENT_EXTERN(Bike_Component);

#ifdef COMPONENT_IMPL
ECS_COMPONENT_DECLARE(Bike_Component);
#endif

void bike_render(Bike *b, SDL_Renderer *r);
void bike_render_offset(Bike *b, SDL_Renderer *r, double x, double y);

void bike_constrain(Bike *b);
void bike_steer(Bike *b, int turn, int speed);
void bike_update(Bike *b);

////////////////ECS///////////////

ecs_entity_t bike_add_steering_system(ecs_world_t *world, ecs_entity_t phase);