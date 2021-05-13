#pragma once
#include "bike.h"
#include "box_collider.h"
#include "flecs.h"
#include "renderable.h"
#include <SDL2/SDL.h>

typedef struct {
  Renderable *wall_renderables;
  BoxCollider *wall_colliders;
  int wall_count;

  Renderable *checkpoint_renderables;
  BoxCollider *checkpoint_colliders;
  Uint32 *checkpoint_times;
  int checkpoint_count;

  int current_checkpoint;

  Bike b;
  ecs_world_t *ecs_world;
} World;

World *world_new(double x, double y);
Vector2D world_get_camera(World *w);
void world_steer(World *w, int turn, int speed);
void world_update(World *w, Uint8 *play_beep, Uint8 *play_crash);
void world_render(SDL_Renderer *r, World *w);
void world_render_offset(SDL_Renderer *r, World *w, float x, float y);
void world_add_checkpoint(World *w, double x, double y, double angle);
void world_add_wall(World *w, double x, double y, double angle, double width,
                    double height);

void world_reset_times(World *w);
void world_free(World **wp);
World *world_from_floats(float *stream);