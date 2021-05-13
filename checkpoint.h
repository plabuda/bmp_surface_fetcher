#pragma once
#include "flecs.h"
#include "position.h"
#include <SDL2/SDL.h>

typedef struct {
  Uint32 timestamp;
} Checkpoint;

typedef struct {
  Uint32 final_timestamp;
  ecs_entity_t first_checkpoint;
  Uint8 checkpoint_count;
  Uint8 current_checkpoint;
} CheckpointTimes;

ECS_COMPONENT_EXTERN(Checkpoint);

#ifdef COMPONENT_IMPL
ECS_COMPONENT_DECLARE(Checkpoint);
#endif

void checkpoint_set(ecs_world_t *world, ecs_entity_t entity, float x, float y,
                    float angle);

Sint8 checkpoint_find_offset(const CheckpointTimes *ct, ecs_iter_t *it);

CheckpointTimes checkpoint_request_new(ecs_world_t *world,
                                       Uint32 checkpoint_count);

ecs_entity_t checkpoint_add_collision_system(ecs_world_t *world,
                                             ecs_entity_t phase,
                                             CheckpointTimes *context);

ecs_entity_t checkpoint_add_rendering_system(ecs_world_t *world,
                                             ecs_entity_t phase,
                                             CheckpointTimes *context,
                                             Position *camera);