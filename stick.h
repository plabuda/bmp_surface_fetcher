#pragma once
#include "flecs.h"
#include "position.h"

typedef struct {
  int index_begin, index_end;
  double length, stiffness;
} Stick;

typedef struct {
  float length;
  ecs_entity_t first, last;
} Stick_Component;

ECS_COMPONENT_EXTERN(Stick_Component);
#ifdef COMPONENT_IMPL
ECS_COMPONENT_DECLARE(Stick_Component);
#endif

ecs_entity_t stick_add_system(ecs_world_t *world, ecs_entity_t phase);
Position *stick_get_first(const Stick_Component *s, ecs_world_t *world);
Position *stick_get_last(const Stick_Component *s, ecs_world_t *world);