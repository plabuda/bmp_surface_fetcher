#pragma once
#include "dot.h"
#include "flecs.h"
#include "vector2d.h"
#include <SDL2/SDL.h>

typedef struct {
  Vector2D center;
  double width, height;
  double angle;
  /* data */
} BoxCollider;

typedef struct {
  double width, height;
  double angle;
  /* data */
} BoxColliderComponent;

typedef BoxColliderComponent SolidBoxCollider;

ECS_COMPONENT_EXTERN(BoxColliderComponent);
ECS_COMPONENT_EXTERN(SolidBoxCollider);

#ifdef COMPONENT_IMPL
ECS_COMPONENT_DECLARE(BoxColliderComponent);
ECS_COMPONENT_DECLARE(SolidBoxCollider);
#endif

Uint8 collider_check(BoxCollider *bc, Dot *d);
Uint8 collider_collide(BoxCollider *bc, Dot *d);

Uint8 collider_check_vector(Vector2D center, double angle, double width,
                            double height, Vector2D v, double radius);

ecs_entity_t collider_add_collision_system(ecs_world_t *world,
                                           ecs_entity_t phase);
